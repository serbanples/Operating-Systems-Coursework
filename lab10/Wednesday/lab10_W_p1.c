#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t alarm_triggered = 0;

void alarm_handler(int signo) {
    (void)signo;
    alarm_triggered = 1;
}

int main() {

    struct sigaction sa;

    sa.sa_handler = alarm_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_flags);

    if(sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    alarm(10);

    printf("Press any key within 10 seconds...\n");

    while(!alarm_triggered) {
        char buff[32];
        int n = read(STDIN_FILENO, buff, 32);
        if(n > 0) {
            printf("Key pressed. Resetting timer...\n");
            alarm(10);
        }
    }

    if(alarm_triggered) {
        printf("No key pressed. Exiting...\n");
    }

    return 0;
}