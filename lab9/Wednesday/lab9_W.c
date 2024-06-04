#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

volatile int sigusr1Count = 0;

void handler_alrm(int signo) {
    (void)signo; // Suppress unused parameter warning
    printf("Parent received SIGALRM\n");
    kill(getpid() + 2, SIGUSR1);  // Send SIGUSR1 to subprocess 2
}

void handler_usr1(int signo) {
    (void)signo; // Suppress unused parameter warning
    printf("Subprocess 2 received SIGUSR1\n");
    sigusr1Count++;

    if (sigusr1Count == 4) {
        exit(sigusr1Count);
    }
}

int main() {
    pid_t pid1, pid2;
    int status;

    signal(SIGALRM, handler_alrm);

    if ((pid1 = fork()) == 0) {
        // Subprocess 1
        while (1) {
            sleep(5);
            kill(getppid(), SIGALRM);  // Send SIGALRM to parent
        }
    } else if ((pid2 = fork()) == 0) {
        // Subprocess 2
        signal(SIGUSR1, handler_usr1);
        while (1) {
            pause(); // Wait for signals
        }
    } else {
        // Parent process
        waitpid(pid2, &status, 0);

        if (WIFEXITED(status)) {
            printf("Subprocess 2 exited with status: %d\n", WEXITSTATUS(status));
        }

        kill(pid1, SIGTERM);  // Send SIGTERM to subprocess 1

        // Wait for subprocess 1 to finish
        waitpid(pid1, NULL, 0);

        printf("Parent process finished.\n");
    }

    return 0;
}