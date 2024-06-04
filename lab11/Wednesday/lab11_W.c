#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

#define NUM_THREADS 6
#define RANGE_START 1
#define RANGE_END 100000

sem_t semaphore;
int prime_count = 0;

// Function to check if a number is prime
int is_prime(int num) {
    if (num <= 1) return 0;
    if (num <= 3) return 1;
    if (num % 2 == 0 || num % 3 == 0) return 0;
    for (int i = 5; i * i <= num; i += 6) {
        if (num % i == 0 || num % (i + 2) == 0) return 0;
    }
    return 1;
}

// Thread function to count primes in a range
void *count_primes(void *arg) {
    int id = *((int *)arg);
    int start = id * (RANGE_END - RANGE_START) / NUM_THREADS + 1;
    int end = (id + 1) * (RANGE_END - RANGE_START) / NUM_THREADS;
    int local_count = 0;

    for (int i = start; i <= end; i++) {
        if (is_prime(i)) {
            sem_wait(&semaphore);
            prime_count++;
            local_count++;
            sem_post(&semaphore);
        }
    }

    printf("Thread %d found %d prime numbers.\n", id, local_count);

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // Initialize semaphore with initial value 3
    sem_open(&semaphore, 0, 3);

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, count_primes, &thread_ids[i]);
    }

    // Wait for threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print total number of primes found
    printf("Total number of prime numbers between %d and %d: %d\n", RANGE_START, RANGE_END, prime_count);

    // Destroy semaphore
    sem_close(&semaphore);

    return 0;
}
