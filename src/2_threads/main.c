#define _GNU_SOURCE
#include <sched.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

/*Глобальные данные*/
char stack[10000], i, j;

/*Код и данные потока*/
int mythread(void * thread_arg) {
    for(int i=0; i<10; i++){
        sleep(1);
        printf("thread: i=%d\n",i);
    }
    printf("Thread1 finished\n");
}

/*Код и данные «главного» потока*/
int main() {
    int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND;
    clone(mythread, (void*) (stack+10000-1), flags, NULL);
    for(j=0; j<10; j++){
        sleep(1);
        printf("MAIN thread: j=%d\n", j);
    }
}
