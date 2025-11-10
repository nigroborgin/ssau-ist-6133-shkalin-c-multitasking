#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

void *func1() { /* Поток 1 */
    int i;
    for (i=0;i<10;i++) { printf("Thread 1 is running\n"); sleep(1); }
}

void *func2() { /* Поток 2 */
    int i;
    for (i=0;i<10;i++) { printf("Thread 2 is running\n"); sleep(1); }
}

int main() {
    int result, status1, status2;
    pthread_t thread1, thread2;
    result = pthread_create(&thread1, NULL, func1, NULL);
    result = pthread_create(&thread2, NULL, func2, NULL);
    pthread_join(thread1, &status1);
    pthread_join(thread2, &status2);
    printf("\nПотоки завершены с %d и %d", status1, status2);

    return 0;
}