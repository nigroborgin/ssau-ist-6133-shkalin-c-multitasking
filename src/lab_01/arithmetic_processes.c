#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "lab_01/arithmetic_processes.h"
#include "lab_01/file_paths.h"
#include "lab_01/task_statuses.h"


typedef double (*math_op_t)(double a, double b);

extern pthread_mutex_t *plus_mutex;
extern pthread_mutex_t *minus_mutex;
extern pthread_mutex_t *mul_mutex;
extern pthread_mutex_t *div_mutex;
extern pthread_mutex_t *sqrt_mutex;

double op_plus(double a, double b){ return a + b; }
double op_minus(double a, double b) { return a - b; }
double op_mul(double a, double b) { return a * b; }
double op_div(double a, double b) { return (b != 0) ? a / b : 0; } // Защита от деления на 0
double op_sqrt(double a, double b) { return (a >= 0) ? sqrt(a) : 0; } // b игнорируем

void generic_process(const char *process_name, const char *filename, pthread_mutex_t *mutex, math_op_t operation)
{
    printf("[%s %d] started\n", process_name, getpid());

    while (1) {
        bool has_job = false;
        double num1 = 0.0, num2 = 0.0;

        // БЛОКИРУЕМ МЬЮТЕКС для чтения файла
        pthread_mutex_lock(mutex);
        FILE *file = fopen(filename, "r");
        if (file) {
            double res_unused;
            int status_int;
            if (fscanf(file, "%d %lf %lf %lf", &status_int, &num1, &num2, &res_unused) == 4) {
                if ((TaskStatus)status_int == STATUS_READY) {
                    has_job = true;
                    printf("[%s %d] got task: num1=%.2lf, num2=%.2lf\n", process_name, getpid(), num1, num2);
                }
            }
            fclose(file);
        } else {
            if (errno != ENOENT) {
                perror("fopen read");
            }
        }

        // РАЗБЛОКИРУЕМ МЬЮТЕКС для старта вычислений или ожидания задания
        pthread_mutex_unlock(mutex);

        if (has_job) {
            const double res = operation(num1, num2);

            // БЛОКИРУЕМ МЬЮТЕКС для записи в файл
            pthread_mutex_lock(mutex);
            FILE *file_out = fopen(filename, "w");
            if (file_out) {
                fprintf(file_out, "%d %.17g %.17g %.17g\n", STATUS_DONE, num1, num2, res);
                fclose(file_out);
                printf("[%s %d] result ready: %.2f\n", process_name, getpid(), res);
            } else {
                perror("fopen write");
            }

            // РАЗБЛОКИРУЕМ МЬЮТЕКС для того чтобы забрали результаты вычислений
            pthread_mutex_unlock(mutex);
        } else {
            usleep(10 * 1000); // 10 мс
        }
    }
}

void plus_process(void) {
    generic_process("PLUS", PLUS_FILE, plus_mutex, op_plus);
}

void minus_process(void) {
    generic_process("MINUS", MINUS_FILE, minus_mutex, op_minus);
}

void multiplier_process(void) {
    generic_process("MULTIPLIER", MUL_FILE, mul_mutex, op_mul);
}

void divider_process(void) {
    generic_process("DIVIDER", DIV_FILE, div_mutex, op_div);
}

void sqrt_process(void) {
    generic_process("SQRT", SQRT_FILE, sqrt_mutex, op_sqrt);
}
