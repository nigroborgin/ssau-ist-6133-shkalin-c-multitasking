#include "lab_01/arithmetic_processes.h"
#include "lab_01/file_paths.h"
#include "lab_01/task_statuses.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

extern pthread_mutex_t file_mutex;

void plus_process(void)
{
    printf("[PLUS %d] started\n", getpid());
    while (1) {
        bool has_job = false;
        TaskStatus status;
        double a = 0.0, b = 0.0;

        // БЛОКИРУЕМ МЬЮТЕКС для чтения файла
        pthread_mutex_lock(&file_mutex);

        FILE *f = fopen(PLUS_FILE, "r");
        if (f) {
            double res_unused;
            int status_int;
            if (fscanf(f, "%d %lf %lf %lf", &status_int, &a, &b, &res_unused) == 4) {
                TaskStatus status = (TaskStatus)status_int;
                if (status == STATUS_READY) {
                    has_job = true;
                    printf("[PLUS %d] got task: a=%.2lf, b=%.2lf\n", getpid(), a, b);
                }
            }
            fclose(f);
        } else {
            perror("[PLUS] fopen read");
        }

        // РАЗБЛОКИРУЕМ МЬЮТЕКС для старта вычислений или ожидания задания
        pthread_mutex_unlock(&file_mutex);

        if (has_job) {
            const double res = a + b;

            // БЛОКИРУЕМ МЬЮТЕКС для записи в файл
            pthread_mutex_lock(&file_mutex);

            FILE *f_out = fopen(PLUS_FILE, "w");
            if (f_out) {
                status = STATUS_DONE;
                fprintf(f_out, "%d %.17g %.17g %.17g\n", status, a, b, res);
                fclose(f_out);
                printf("[PLUS %d] result ready: %.2f\n", getpid(), res);
            } else {
                perror("[PLUS] fopen write");
            }
            // РАЗБЛОКИРУЕМ МЬЮТЕКС для того чтобы забрали результаты вычислений
            pthread_mutex_unlock(&file_mutex);

        } else {
            usleep(10 * 1000);  // 10 мс
        }
    }
}

void minus_process(void)
{
    printf("[MINUS %d] started\n", getpid());
    while (1) {
        usleep(1000 * 1000);
    }
}

void multiplier_process(void)
{
    printf("[MULTIPLIER %d] started\n", getpid());
    while (1) {
        usleep(1000 * 1000);
    }
}

void divider_process(void)
{
    printf("[DIVIDER %d] started\n", getpid());
    while (1) {
        usleep(1000 * 1000);
    }
}

void sqrt_process(void)
{
    printf("[SQRT %d] started\n", getpid());
    while (1) {
        usleep(1000 * 1000);
    }
}
