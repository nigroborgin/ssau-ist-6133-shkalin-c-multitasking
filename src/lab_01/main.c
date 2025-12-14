#include "lab_01/arithmetic_processes.h"
#include "lab_01/file_paths.h"
#include "lab_01/task_statuses.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

pid_t plus_pid  = -1;
pid_t minus_pid  = -1;
pid_t mul_pid  = -1;
pid_t div_pid  = -1;
pid_t sqrt_pid = -1;

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

int main(void)
{
    create_processes();

    // double a1, b1, c1;
    // printf("КВАДРАТНОЕ УРАВНЕНИЕ\n"
    //        "Введите коэффициенты a, b, c (или Ctrl+D для выхода): ");
    // scanf("%lf %lf %lf", &a1, &b1, &c1);

    // БЛОКИРУЕМ МЬЮТЕКС для записи задания
    pthread_mutex_lock(&file_mutex);

    FILE *file_task = fopen(PLUS_FILE, "w");
    if (!file_task) {
        perror("fopen PLUS_FILE");
        pthread_mutex_unlock(&file_mutex);
        complete_processes();
        return 1;
    }
    const double a = 10.5, b = 1.5, res = 0.0; // тестовые данные
    fprintf(file_task, "%d %.17g %.17g %.17g\n", STATUS_READY, a, b, res);
    fclose(file_task);

    // РАЗБЛОКИРУЕМ МЬЮТЕКС для передачи задания на выполнение
    pthread_mutex_unlock(&file_mutex);

    // Ждем результат
    double result_plus;
    while (1) {
        // БЛОКИРУЕМ МЬЮТЕКС для чтения файла
        pthread_mutex_lock(&file_mutex);
        FILE *file_res = fopen(PLUS_FILE, "r");

        if (file_res) {
            int status_int;
            double a_readed, b_readed;
            if (fscanf(file_res, "%d %lf %lf %lf", &status_int, &a_readed, &b_readed, &result_plus) == 4) {
                TaskStatus status = (TaskStatus)status_int;
                if (status == STATUS_DONE) {
                    fclose(file_res);
                    // РАЗБЛОКИРУЕМ МЬЮТЕКС т.к. файл прочитан
                    pthread_mutex_unlock(&file_mutex);
                    // выходим из цикла, т.к. результат получен
                    break;
                }
            }
            fclose(file_res);
        }
        // РАЗБЛОКИРУЕМ МЬЮТЕКС ждём результаты дальше
        pthread_mutex_unlock(&file_mutex);
        usleep(5000);  // подождать 5 мс
    }

    printf("result_plus: %.17g\n", result_plus);
    complete_processes();
    return 0;
}
