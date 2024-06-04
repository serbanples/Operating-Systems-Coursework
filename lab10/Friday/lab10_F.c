#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

// Global variables for a0 and r
double a0, r;

// Function to compute a[n+1]
double compute_next_value(double a) {
    return a + 1.0 / r;
}

// SIGUSR1 signal handler for the child process
void sigusr1_handler(int signum) {
    static int print_count = 0;
    static double a_current = 0.0;

    if (print_count == 0) {
        a_current = a0;
    }

    // Print the current value of a[n]
    printf("a[%d] = %.2f\n", print_count, a_current);

    // Compute the next value of a[n+1]
    a_current = compute_next_value(a_current);

    // Increment the print count
    print_count++;

    // If printed twice, exit
    if (print_count == 2) {
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    // Check for correct number of arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s a0 r\n", argv[0]);
        exit(1);
    }

    // Parse command line arguments
    a0 = atof(argv[1]);
    r = atof(argv[2]);

    // Fork a child process
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // Child process
        // Set up SIGUSR1 signal handler
        signal(SIGUSR1, sigusr1_handler);

        // Wait for SIGUSR1 signal
        while (1) {
            sleep(1); // Sleep for 1 second
        }
    } else {
        // Parent process
        // Print initial message
        printf("*****\n");

        // Loop to alternate between +++++ and ------
        int count = 0;
        while (1) {
            if (count % 2 == 0) {
                printf("+++++\n");
            } else {
                printf("------\n");
            }

            // Send SIGUSR1 signal to child every second
            kill(pid, SIGUSR1);
            sleep(2);
            count++;
        }

        // Wait for child process to finish
        int status;
        waitpid(pid, &status, 0);

        // Print exit code of child process
        printf("Child process exited with code: %d\n", WEXITSTATUS(status));
    }

    return 0;
}
