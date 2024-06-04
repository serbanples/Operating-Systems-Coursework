#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

volatile sig_atomic_t should_exit = 0;

void sig_handler(int signum) {
    if(signum == SIGTSTP) {
        printf("Subprocess 2 received SIGTSTP.\n");
        should_exit = 1;
    }
    if(signum == SIGINT) {
        printf("Subprocess 3 received SIGINT\n");
        should_exit = 1;
    }
}

int main(int argc, char* arv[]) {
    pid_t pid1, pid2, pid3;

    pid1 = fork();

    if(pid1 == -1) {
        perror("fork");
        exit(1);
    }
    else if(pid1 == 0) {
        while(1) {
            sleep(10);
            kill(getppid(), SIGALRM);
        }
    }
    else {
        pid2 = fork();

        if(pid2 == -1) {
            perror("fork");
            exit(1);
        }
        else if(pid2 == 0) {
            signal(SIGTSTP, sig_handler);
            while(!should_exit) {
                pause();
            }
            printf("Subprocess 2 finished.\n");
            exit(0);
        }
        else {
            pid3 = fork();

            if(pid3 == -1) {
                perror("fork");
                exit(1);
            }
            else if(pid3 == 0) {
                signal(SIGINT, sig_handler);
                while(!should_exit) {
                    pause();
                }
                printf("Subprocess 3 finished.\n");
                exit(0);
            }
            else {
                signal(SIGALRM, SIG_IGN);

                int count = 0;
                while(count < 2 && !should_exit) {
                    sleep(10);
                    if(count == 0) {
                        kill(pid2, SIGTSTP);
                        printf("Sent SIGTSTP to subprocess 2.\n");
                    }
                    else {
                        kill(pid3, SIGINT);
                        printf("Sent SIGINT to subprocess 3.\n");
                    }
                    count++;
                }

                waitpid(pid2, NULL, 0);
                waitpid(pid3, NULL, 0);

                kill(pid1, SIGKILL);

                waitpid(pid1, NULL, 0);

                printf("All processes finished.\n");
            }
        }
    }
    return 0;
}
