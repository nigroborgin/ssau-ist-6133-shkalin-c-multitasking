#include <stdio.h>
#include <unistd.h>
#include <wait.h>

pid_t p, q;
char *e[]={"",""};

int main() {

    p = fork();                     /* Копирование адресного пространства и процесса */
    if (p) {                        /* Родитель получает PID ребенка */
        printf("Child pid = %d. Parent (%d) START wait\n", p, getpid());
        // wait(&q);                /* Ждать завершения потомков */
        sleep(5);                   /* Задержка на 5 сек */
        printf("Parent FINISH wait\n");
        kill(p, SIGKILL);           /* Ребенок усыплён */
        printf("Child pid = %d. Child was killed\n", p);
    } else {                        /* Ребенок получает 0 */
        printf("pid = %d. Child START\n", p);
        execv("./out/1_processes/hello", e);   /* Ребенок замещает себя другой программой */
        printf("pid = %d. Child FINISH\n", p);
    }

    return 0;
}
