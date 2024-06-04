#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define NUM_CHILD  10

int round_number = 1;

void sigusr1_handler(int signo) {
    (void)signo;
    printf("Process %d caught the 'Win' message in round %d.\n", getpid(), round_number);
    exit(0);
}

int main() {
    pid_t pid, pids[NUM_CHILD];
    int pipe_fd[2];

    if(pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(1);
    }

    signal(SIGUSR1, sigusr1_handler);

    for(int i = 0; i < NUM_CHILD; i++) {
        pid = fork();
        
        if(pid == -1) {
            perror("fork");
            exit(1);
        }

        else if(pid == 0) {
            close(pipe_fd[1]);

            signal(SIGUSR1, sigusr1_handler);
            pause();

            char buffer[4];
            read(pipe_fd[0], buffer, sizeof(buffer));
            if(strcmp(buffer, "Win") == 0) {
                //printf("Process %d caught the 'Win' message in round %d\n", getpid(), round_number);
                exit(0);
            }
        }
        else {
            pids[i] = pid;
            round_number ++;
        }
    }

    close(pipe_fd[0]);

    for(int i = 0; i < NUM_CHILD; i++) {
        kill(pids[i], SIGUSR1);
    }

    write(pipe_fd[1], "Win", sizeof("Win"));

    for(int i = 0; i < NUM_CHILD; i++) {
        waitpid(pids[i], NULL, 0);
    }

    close(pipe_fd[1]);

    return 0;
}