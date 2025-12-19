#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>  // для va_list, va_start, va_end

#include "lab_01/arithmetic_processes.h"
#include "lab_01/file_paths.h"
#include "lab_01/task_statuses.h"
#include "lab_01/utils.h"


pid_t plus_pid  = -1;
pid_t minus_pid  = -1;
pid_t mul_pid  = -1;
pid_t div_pid  = -1;
pid_t sqrt_pid = -1;

pthread_mutex_t *plus_mutex;
pthread_mutex_t *minus_mutex;
pthread_mutex_t *mul_mutex;
pthread_mutex_t *div_mutex;
pthread_mutex_t *sqrt_mutex;
pthread_mutex_t *stdout_mutex;


const int TIMEOUT_PER_TASK = 5000;      // Время ожидания на каждую попытку (в микросекундах): 5000 мкс = 5 мс
const int MAX_ATTEMPTS_PER_TASK = 1000; // Максимальное количество попыток
// Общий максимальный таймаут на задачу: 1000 попыток * 5 мс = 5 секунд

void sync_printf(const char* format, ...)
{
    va_list args; // список переменных аргументов из "..."
    va_start(args, format);

    pthread_mutex_lock(stdout_mutex);
    vprintf(format, args);
    fflush(stdout);
    pthread_mutex_unlock(stdout_mutex);

    va_end(args);
}

void init_shared_mutex(pthread_mutex_t **mutex_ptr)
{
    // Выделяем память (под размер мьютекса), которая будет общей для всех процессов после fork
    // sizeof(pthread_mutex_t) — это всего 24-40 байт

    // PROT_READ | PROT_WRITE - разрешаем чтение и запись
    // MAP_SHARED - изменения видны другим процессам
    // MAP_ANONYMOUS - не используем файловый дескриптор (fd = -1)
    *mutex_ptr = mmap(NULL, sizeof(pthread_mutex_t),
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (*mutex_ptr == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    // Настраиваем атрибуты мьютекса, чтобы он работал между процессами
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    // PTHREAD_PROCESS_SHARED - ключевой флаг! Без него мьютекс работать не будет.
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);

    // Инициализируем мьютекс в этой памяти
    if (pthread_mutex_init(*mutex_ptr, &attr) != 0) {
        perror("pthread_mutex_init");
        exit(EXIT_FAILURE);
    }

    pthread_mutexattr_destroy(&attr);
}

void init_mutexes(void)
{
    init_shared_mutex(&plus_mutex);
    init_shared_mutex(&minus_mutex);
    init_shared_mutex(&mul_mutex);
    init_shared_mutex(&div_mutex);
    init_shared_mutex(&sqrt_mutex);
    init_shared_mutex(&stdout_mutex);
}

void cleanup_mutex_memory(pthread_mutex_t **mutex_ptr)
{
    pthread_mutex_destroy(*mutex_ptr);
    munmap(*mutex_ptr, sizeof(pthread_mutex_t));
}

void cleanup_mutexes(void)
{
    cleanup_mutex_memory(&plus_mutex);
    cleanup_mutex_memory(&minus_mutex);
    cleanup_mutex_memory(&mul_mutex);
    cleanup_mutex_memory(&div_mutex);
    cleanup_mutex_memory(&sqrt_mutex);
    cleanup_mutex_memory(&stdout_mutex);
}

void create_processes(void)
{
    plus_pid = fork();
    if (plus_pid == 0) {
        plus_process();
    }

    minus_pid = fork();
    if (minus_pid == 0) {
        minus_process();
    }

    mul_pid = fork();
    if (mul_pid == 0) {
        multiplier_process();
    }

    div_pid = fork();
    if (div_pid == 0) {
        divider_process();
    }

    sqrt_pid = fork();
    if (sqrt_pid == 0) {
        sqrt_process();
    }
}

void complete_processes(void)
{
    kill(plus_pid, SIGTERM);
    kill(minus_pid, SIGTERM);
    kill(mul_pid, SIGTERM);
    kill(div_pid, SIGTERM);
    kill(sqrt_pid, SIGTERM);

    waitpid(plus_pid, NULL, 0);
    waitpid(minus_pid, NULL, 0);
    waitpid(mul_pid, NULL, 0);
    waitpid(div_pid, NULL, 0);
    waitpid(sqrt_pid, NULL, 0);
}

void init(void)
{
    init_mutexes();
    create_processes();
}

void cleanup(void)
{
    complete_processes();
    cleanup_mutexes();
}

bool create_task(const char *filename, pthread_mutex_t *mutex, const double num1, const double num2)
{
    // БЛОКИРУЕМ МЬЮТЕКС для записи задания
    pthread_mutex_lock(mutex);

    FILE *file_task = fopen(filename, "w");
    if (!file_task) {
        fprintf(stderr, "Ошибка открытия файла %s: %s\n", filename, strerror(errno));
        pthread_mutex_unlock(mutex);
        return false;
    }
    fprintf(file_task, "%d %.17g %.17g %.17g\n", STATUS_READY, num1, num2, 0.0);
    fclose(file_task);

    // РАЗБЛОКИРУЕМ МЬЮТЕКС для передачи задания на выполнение
    pthread_mutex_unlock(mutex);
    return true;
}

double get_task_result(const char *filename, pthread_mutex_t *mutex)
{
    double result = 0.0;

    int attempts = 0;
    while (attempts < MAX_ATTEMPTS_PER_TASK) {
        // БЛОКИРУЕМ МЬЮТЕКС для чтения файла
        pthread_mutex_lock(mutex);
        FILE *file_res = fopen(filename, "r");

        if (file_res) {
            int status_int;
            double num1_readed, num2_readed;
            if (fscanf(file_res, "%d %lf %lf %lf", &status_int, &num1_readed, &num2_readed, &result) == 4) {
                if ((TaskStatus)status_int == STATUS_DONE) {
                    fclose(file_res);

                    // Сбросить файл в состояние IDLE, чтобы родительский поток
                    FILE *clear = fopen(filename, "w");
                    if (clear) {
                        fprintf(clear, "%d 0 0 0\n", STATUS_IDLE);
                        fclose(clear);
                    }

                    // РАЗБЛОКИРУЕМ МЬЮТЕКС т.к. файл прочитан
                    pthread_mutex_unlock(mutex);
                    // выходим из цикла, т.к. результат получен
                    return result;
                }
            }
            fclose(file_res);
        } else {
            if (errno != ENOENT) {
                perror("fopen read");
            }
        }
        // РАЗБЛОКИРУЕМ МЬЮТЕКС ждём результаты дальше
        pthread_mutex_unlock(mutex);

        attempts++;
        usleep(TIMEOUT_PER_TASK); // подождать
    }

    fprintf(stderr, "Timeout: процесс не вернул результат\n");
    return NAN;  // Возвращаем NaN при таймауте
}


int main(void)
{
    init();

    double a, b, c;
    // Даем дочерним процессам время вывести сообщения о запуске, чтобы они не смешивались с сообщением ввода
    usleep(1000);
    sync_printf("\nКВАДРАТНОЕ УРАВНЕНИЕ:\n"
                "Введите коэффициенты a, b, c: ");
    if (scanf("%lf %lf %lf", &a, &b, &c) != 3) {
        fprintf(stderr, "Ошибка ввода\n");
        goto error;
    }

    sync_printf("\n");
    // a = 1; b = -5; c = 6;
    // 1. Посчитать b^2 (через MUL_FILE)
    if (!create_task(MUL_FILE, mul_mutex, b, b)) goto error;
    sync_printf("[%s %d] create task: %s num1=%.2lf, num2=%.2lf\n", "MAIN", getpid(), "MULTI", b, b);
    const double b_squared = get_task_result(MUL_FILE, mul_mutex);
    sync_printf("[%s %d] got result by %s: %.2lf\n\n", "MAIN", getpid(), "MULTI", b_squared);

    // 2.1. Посчитать 4*a (через MUL_FILE)
    if (!create_task(MUL_FILE, mul_mutex, 4.0, a)) goto error;
    sync_printf("[%s %d] create task: %s num1=%.2lf, num2=%.2lf\n", "MAIN", getpid(), "MULTI", 4.0, a);
    const double a_4 = get_task_result(MUL_FILE, mul_mutex);
    sync_printf("[%s %d] got result by %s: %.2lf\n\n", "MAIN", getpid(), "MULTI", a_4);

    // 2.2. Посчитать 4a*c (через MUL_FILE)
    if (!create_task(MUL_FILE, mul_mutex, a_4, c)) goto error;
    sync_printf("[%s %d] create task: %s num1=%.2lf, num2=%.2lf\n", "MAIN", getpid(), "MULTI", a_4, c);
    const double a_c_4 = get_task_result(MUL_FILE, mul_mutex);
    sync_printf("[%s %d] got result by %s: %.2lf\n\n", "MAIN", getpid(), "MULTI", a_c_4);

    // 3. Посчитать D = b^2 - 4ac (через MINUS_FILE)
    if (!create_task(MINUS_FILE, minus_mutex, b_squared, a_c_4)) goto error;
    sync_printf("[%s %d] create task: %s num1=%.2lf, num2=%.2lf\n", "MAIN", getpid(), "MINUS", b_squared, a_c_4);
    const double d = get_task_result(MINUS_FILE, minus_mutex);
    sync_printf("[%s %d] got result by %s: %.2lf\n\n", "MAIN", getpid(), "MINUS", d);

    // 4. Если D < 0:
    if (d < 0) {
        sync_printf("Результат: Нет действительных корней");
        cleanup();
        return 0;
    }

    // 5. Извлечь sqrt(D) (через SQRT_FILE)
    if (!create_task(SQRT_FILE, sqrt_mutex, d, 0.0)) goto error;
    sync_printf("[%s %d] create task: %s num1=%.2lf\n", "MAIN", getpid(), "SQRT", d);
    const double sqrt_d = get_task_result(SQRT_FILE, sqrt_mutex);
    sync_printf("[%s %d] got result by %s: %.2lf\n\n", "MAIN", getpid(), "SQRT", sqrt_d);

    // 6-7. Посчитать x1, x2 = (-b ± sqrt(D)) / 2a

    // -b
    if (!create_task(MINUS_FILE, minus_mutex, 0, b)) goto error;
    sync_printf("[%s %d] create task: %s num1=%.2lf, num2=%.2lf\n", "MAIN", getpid(), "MINUS", 0.0, b);
    const double minus_b = get_task_result(MINUS_FILE, minus_mutex);
    sync_printf("[%s %d] got result by %s: %.2lf\n\n", "MAIN", getpid(), "MINUS", minus_b);

    // 2a
    if (!create_task(MUL_FILE, mul_mutex, a, 2)) goto error;
    sync_printf("[%s %d] create task: %s num1=%.2lf, num2=%.2lf\n", "MAIN", getpid(), "MULTI", a, 2.0);
    const double a_2 = get_task_result(MUL_FILE, mul_mutex);
    sync_printf("[%s %d] got result by %s: %.2lf\n\n", "MAIN", getpid(), "MULTI", a_2);

    // (-b + sqrt(D))
    if (!create_task(PLUS_FILE, plus_mutex, minus_b, sqrt_d)) goto error;
    sync_printf("[%s %d] create task: %s num1=%.2lf, num2=%.2lf\n", "MAIN", getpid(), "PLUS", minus_b, sqrt_d);
    const double minus_b_plus_sqrt_d = get_task_result(PLUS_FILE, plus_mutex);
    sync_printf("[%s %d] got result by %s: %.2lf\n\n", "MAIN", getpid(), "PLUS", minus_b_plus_sqrt_d);

    // (-b - sqrt(D))
    if (!create_task(MINUS_FILE, minus_mutex, minus_b, sqrt_d)) goto error;
    sync_printf("[%s %d] create task: %s num1=%.2lf, num2=%.2lf\n", "MAIN", getpid(), "MINUS", minus_b, sqrt_d);
    const double minus_b_minus_sqrt_d = get_task_result(MINUS_FILE, minus_mutex);
    sync_printf("[%s %d] got result by %s: %.2lf\n\n", "MAIN", getpid(), "MINUS", minus_b_minus_sqrt_d);

    // Посчитать x1 = (-b + sqrt(D)) / 2a
    if (!create_task(DIV_FILE, div_mutex, minus_b_plus_sqrt_d, a_2)) goto error;
    sync_printf("[%s %d] create task: %s num1=%.2lf, num2=%.2lf\n", "MAIN", getpid(), "DIV", minus_b_plus_sqrt_d, a_2);
    const double x1 = get_task_result(DIV_FILE, div_mutex);
    sync_printf("[%s %d] got result by %s: %.2lf\n\n", "MAIN", getpid(), "DIV", x1);

    // Посчитать x2 = (-b - sqrt(D)) / 2a
    if (!create_task(DIV_FILE, div_mutex, minus_b_minus_sqrt_d, a_2)) goto error;
    sync_printf("[%s %d] create task: %s num1=%.2lf, num2=%.2lf\n", "MAIN", getpid(), "DIV", minus_b_minus_sqrt_d, a_2);
    const double x2 = get_task_result(DIV_FILE, div_mutex);
    sync_printf("[%s %d] got result by %s: %.2lf\n\n", "MAIN", getpid(), "DIV", x2);

    // Результат
    sync_printf("\nРезультат: x1 = %.17g, x2 = %.17g\n", x1, x2);

    cleanup();
    return 0;

error:
    cleanup();
    return 1;
}
