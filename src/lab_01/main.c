#include "lab_01/arithmetic_processes.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>


// PID'ы рабочих процессов
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
        exit(0);
    }

    minus_pid = fork();
    if (minus_pid == 0) {
        minus_process();
        exit(0);
    }

    mul_pid = fork();
    if (mul_pid == 0) {
        multiplier_process();
        exit(0);
    }

    div_pid = fork();
    if (div_pid == 0) {
        divider_process();
        exit(0);
    }

    sqrt_pid = fork();
    if (sqrt_pid == 0) {
        sqrt_process();
        exit(0);
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

    while (1) {
        double a, b, c;
        printf("КВАДРАТНОЕ УРАВНЕНИЕ\n"
               "Введите коэффициенты a, b, c (или Ctrl+D для выхода): ");
        if (scanf("%lf %lf %lf", &a, &b, &c) != 3) {
            break;
        }

        printf("Результат: x1 = ?, x2 = ?\n");
    }

    complete_processes();
    return 0;
}
