#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int pipefd[2];

void sigusr1_handler(int signum) {
    char msg[256];
    int round = 0;
    
    read(pipefd[0], msg, sizeof(msg));

    //printf("Child %d received 'Win' message in round %d\n", getpid(), round);
    exit(round);
}

int main() {
    int i;
    pid_t children[10];

    if (pipe(pipefd) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < 10; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            signal(SIGUSR1, sigusr1_handler);
            while (1) {
                pause();
            }
        } else {
            children[i] = pid;
        }
    }

    close(pipefd[0]);

    // Shuffle the array of child process IDs
    srand(time(NULL));
    for (i = 9; i > 0; i--) {
        int j = rand() % (i + 1);
        pid_t temp = children[i];
        children[i] = children[j];
        children[j] = temp;
    }

    for (i = 1; i <= 10; i++) {
        char msg[256] = "Win";
        write(pipefd[1], msg, sizeof(msg));
        kill(children[i - 1], SIGUSR1);

    }

    int status;
    for (i = 0; i < 9; i++) {
        waitpid(children[i], &status, 0);
        printf("Child %d eliminated in round %d\n", children[i], i + 1);
    }
    printf("Child %d is the winner!\n", children[i]);

    return 0;
}
