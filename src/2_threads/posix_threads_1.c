#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

void *func(void *arg) {
    int loc_id = * (int *) arg;
    while (1) {
        printf("Thread %i is running\n", loc_id);
        sleep(1);
    }
}
int main() {
    int id1, id2, result;
    pthread_t thread1, thread2;
    id1 = 1;
    result = pthread_create(&thread1, NULL, func, &id1);
    pthread_detach(thread1);
    id2 = 2;
    result = pthread_create(&thread2, NULL, func, &id2);
    pthread_detach(thread2);

    return 0;
}