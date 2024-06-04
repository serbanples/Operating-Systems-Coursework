#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define THREAD_NR 6

int primes = 0;
sem_t semID, crs;

void *start(void *arg) {
    int threadID = *((int*) arg);
    int p = 0;
    sem_
}

int main(int argc, char *argv[]) {
    pthread_t threads[THREAD_NR];
    int *p;

    sem_init(semID, 0, 3);

    for(int i = 0; i < THREAD_NR; i++) {
        p = (int *)malloc(sizeof(int));
        *p = i;
        pthread_create(&threads[i], NULL, start, p);
    }

    return 0;
}