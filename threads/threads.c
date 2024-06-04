#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *op1(void *arg) {
    int *params = (int *)arg;
    int n = params[0];
    int m = params[1];
    int *ret = malloc(sizeof(int));

    // Operation 1
    m = m + n / 2;
    *ret = m;

    pthread_exit(ret);
}

void *op2(void *arg) {
    int *params = (int *)arg;
    int n = params[0];
    int m = params[1];
    int *ret = malloc(sizeof(int));

    // Operation 2
    m = m - (n - 2);
    *ret = m;

    pthread_exit(ret);
}

void *op3(void *arg) {
    int *params = (int *)arg;
    int n = params[0];
    int m = params[1];
    int *ret = malloc(sizeof(int));

    // Operation 3
    n = m * 4 / n;
    *ret = m;

    pthread_exit(ret);
}

int main() {
    int params[2] = {4, 2};
    pthread_t thrID1, thrID2, thrID3;
    int *thrRet1, *thrRet2, *thrRet3;

    // Set higher priority for thrID1
    struct sched_param param1;
    param1.sched_priority = 1;  // Higher priority

    pthread_create(&thrID1, NULL, &op1, (void *)params);
    pthread_setschedparam(thrID1, SCHED_FIFO, &param1);
    pthread_join(thrID1, (void **)&thrRet1);

    // Create other threads after thrID1 finishes
    pthread_create(&thrID2, NULL, &op2, (void *)params);
    pthread_create(&thrID3, NULL, &op3, (void *)params);

    // Wait for the completion of thrID2 and thrID3
    pthread_join(thrID2, (void **)&thrRet2);
    pthread_join(thrID3, (void **)&thrRet3);

    // Print results
    printf("Thread 1 Result: %d\n", *thrRet1);
    printf("Thread 2 Result: %d\n", *thrRet2);
    printf("Thread 3 Result: %d\n", *thrRet3);

    // Free allocated memory
    free(thrRet1);
    free(thrRet2);
    free(thrRet3);

    return 0;
}
