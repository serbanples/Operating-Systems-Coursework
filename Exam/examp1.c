#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid1, pid2, pid3;

    pid1 = fork();
    if (pid1 < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid1 == 0) {
        // Child 1
        pid2 = getppid(); // Get parent's PID
        printf("Child 1 PID: %d\n", getpid());
        exit(pid2);
    } else {
        pid2 = fork();
        if (pid2 < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid2 == 0) {
            // Child 2
            pid1 = getpid(); // Get own PID
            pid_t received_pid;
            read(STDIN_FILENO, &received_pid, sizeof(received_pid)); // Read PID from pipe
            printf("Child 2 received PID from Child 1: %d\n", received_pid);
            printf("Child 2 PID: %d\n", pid1);
            exit(EXIT_SUCCESS);
        } else {
            pid3 = fork();
            if (pid3 < 0) {
                perror("Fork failed");
                exit(EXIT_FAILURE);
            } else if (pid3 == 0) {
                // Child 3
                pid2 = getppid(); // Get parent's PID
                printf("Child 3 PID: %d\n", getpid());
                printf("Parent PID: %d\n", pid2);
                exit(EXIT_SUCCESS);
            }
        }
    }

    // Parent process
    wait(NULL); // Wait for child 1 to finish
    printf("Parent PID: %d\n", getpid());
    wait(NULL); // Wait for child 2 to finish
    wait(NULL); // Wait for child 3 to finish

    return 0;
}
