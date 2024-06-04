#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int compute_sum(int n) {
    int sum = 0;
    for(int i = 1; i <= n; i++)
        sum = sum + i;
    return sum;
}

int compute_factorial(int n) {
    int factorial = 1;
    for(int i = 1; i <= n; i++)
        factorial = factorial * i;
    return factorial;
}

int main(int argc, char *argv[]) {
    if(argc != 2){
        fprintf(stderr,"Usage: %s <integer_number>", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n1 = atoi(argv[1]);

    pid_t pid_sum, pid_fact;
    int status;

    pid_sum = fork();

    if(pid_sum == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(pid_sum == 0) {
        int sum = compute_sum(n1);
        printf("[ID = %d] Sum of positive integers up to %d is %d\n", getpid(), n1, sum);
        exit(EXIT_SUCCESS);
    }
    else {
        pid_fact = fork();

        if(pid_fact == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if(pid_fact == 0) {
            int fact = compute_factorial(n1);
            printf("[ID = %d] Sum of positive integers up to %d is %d\n", getpid(), n1, fact);
        }
        else {
            int status;
            waitpid(pid_sum, &status, 0);
            waitpid(pid_fact, &status, 0);

            printf("[ID = %d] Done\n", getpid());
        }
    }

    return 0;
}