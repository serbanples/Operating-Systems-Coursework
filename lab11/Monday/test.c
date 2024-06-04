#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 100

typedef struct {
    FILE* file;
    long start;
    long end;
    int id;
} ThreadArgs;

void* readFromFile(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;

    fseek(args->file, args->start, SEEK_SET);

    long size = args->end - args->start + 1;

    char* buffer = (char*)malloc(size);

    fread(buffer, 1, size, args->file);

    printf("Thread %d - Content: %.*s\n", args->id, (int)size, buffer);

    free(buffer);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <num_threads>\n", argv[0]);
        exit(1);
    }

    const char* filename = argv[1];
    int numThreads = atoi(argv[2]);

    if (numThreads <= 0 || numThreads > MAX_THREADS) {
        fprintf(stderr, "Invalid number of threads. Must be between 1 and %d.\n", MAX_THREADS);
        exit(1);
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    long threadSize = fileSize / numThreads;

    pthread_t threads[numThreads];

    for (int i = 0; i < numThreads; ++i) {
        ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
        args->file = file;
        args->start = i * threadSize;
        args->end = (i == numThreads - 1) ? fileSize - 1 : (i + 1) * threadSize - 1;
        args->id = i+1;

        pthread_create(&threads[i], NULL, readFromFile, args);
    }

    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], NULL);
    }

    fclose(file);

    return 0;
}