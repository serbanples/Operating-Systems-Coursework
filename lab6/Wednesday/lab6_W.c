#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char *argv[]){

    if(argc != 3){
        fprintf(stderr, "Usage %s <binary_file> <text_file>", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *text_file = argv[2];
    char *bin_file = argv[1];

    pid_t pid = fork();
    int status;

    if(pid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0){
        int fd = open("binary_output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(fd);

        execl(bin_file, bin_file, ">", "binary_output.txt", NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }
    else {
        waitpid(pid, &status, 0);
        if(!WIFEXITED(status) || WEXITSTATUS(status) != 0){
            printf("ERROR: Failed to execute binary or redirect output");
            exit(EXIT_FAILURE);
        }
    }

    pid = fork();
    if(pid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0) {
        execl("/usr/bin/diff", "diff", "binary_output.txt", text_file, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }
    else {
        waitpid(pid, &status, 0);
        if(WIFEXITED(status)) {
            if(WEXITSTATUS(status) == 0) {
                printf("The %s executable has the desired output.\n", bin_file);
            } else {
                printf("ERROR: The %s executable does not have the desired output.\n", bin_file);
            }
        } else {
            printf("ERROR: Failed to compare files.\n");
        }
    }

    return 0;
}