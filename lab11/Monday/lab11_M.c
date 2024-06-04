#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 100

int main(int argc, char*argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <num_threads>\n", argv[0]);
        exit(1);
    }

    const char* filename = argv[1];
    int numThreads = atoi(argv[2]);

    if (numThreads <= 0 || numThreads > MAX_THREADS) {
        fprintf(stderr, "Invalid number of threads. Must be between 1 and %d.\n", MAX_THREADS);
        return EXIT_FAILURE;
    }

    FILE *file = fopen(filename, "r");
    if(!file) {
        perror("fopen");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    long threadSize = 

    return 0;
}