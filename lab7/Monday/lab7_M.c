#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file1> <file2>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int pipe1_fd[2];
    int pipe2_fd[2];
    pid_t pid;
    int status;

    if (pipe(pipe1_fd) < 0 || pipe(pipe2_fd) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { 
        close(pipe2_fd[0]); 
        close(pipe1_fd[1]); 

        char buffer[BUFFER_SIZE];

        ssize_t bytes_read = read(pipe1_fd[0], buffer, BUFFER_SIZE);
        if (bytes_read == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        close(pipe1_fd[0]); 

        FILE *file2 = fopen(argv[2], "r");
        if (file2 == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        size_t bytes_read_file2 = fread(buffer + bytes_read, 1, BUFFER_SIZE - bytes_read, file2);
        if (ferror(file2)) {
            perror("fread");
            exit(EXIT_FAILURE);
        }
        fclose(file2);

        ssize_t total_bytes_to_write = bytes_read + bytes_read_file2;

        ssize_t bytes_written = write(pipe2_fd[1], buffer, total_bytes_to_write);
        if (bytes_written == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }

        close(pipe2_fd[1]); 

        exit(EXIT_SUCCESS);
    } else { 
        close(pipe2_fd[1]); 
        close(pipe1_fd[0]); 

        FILE *file1 = fopen(argv[1], "r");
        if (file1 == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        char buffer[BUFFER_SIZE];

        size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, file1);
        if (ferror(file1)) {
            perror("fread");
            exit(EXIT_FAILURE);
        }
        fclose(file1);

        ssize_t bytes_written = write(pipe1_fd[1], buffer, bytes_read);
        if (bytes_written == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        close(pipe1_fd[1]); // Close write end of pipe1

        waitpid(pid, &status, 0);

        ssize_t total_bytes_read = 0;
        ssize_t bytes_read_pipe2;
        while ((bytes_read_pipe2 = read(pipe2_fd[0], buffer + total_bytes_read, BUFFER_SIZE - total_bytes_read)) > 0) {
            total_bytes_read += bytes_read_pipe2;
        }
        if (bytes_read_pipe2 == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        printf("%s\n", buffer);

        close(pipe2_fd[0]); 
    }

    return 0;
}
