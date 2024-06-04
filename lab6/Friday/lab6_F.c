#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

int main(int argc, char*argv[]) {
    if(argc < 2){
        fprintf(stderr, "Usage: %s <bin1> <bin2> <bin3> ...", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t child_pid;
    int status;
    struct timespec start, end;

    for(int i = 1; i < argc; i++) {

        clock_gettime(CLOCK_MONOTONIC, &start);

        child_pid = fork();
        if(child_pid == -1){
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if(child_pid == 0) {
            
            execl(argv[i], NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        }
        else{
            waitpid(child_pid, &status, 0);
            if(WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                clock_gettime(CLOCK_MONOTONIC, &end);
                double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
                printf("%s: %.2fs\n", argv[i], elapsed_time);
            } 
            else {
                printf("%s: Failed to execute\n", argv[i]);
            } 
        }
    }

    return 0;
}