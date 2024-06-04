#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]) {

    if(argc < 1){
        fprintf(stderr, "Usage: %s <command1> | <command2> \n", argv[0]);
        exit(1);
    }

    char *cmd1_args[argc];
    char *cmd2_args[argc];
    int cmd1_argc = 0, cmd2_argc = 0;

    int is_pipe = 0;

    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "|") == 0) {
            cmd1_args[cmd1_argc] = NULL;
            is_pipe = 1;
            continue;
        }

        if (!is_pipe) {
            cmd1_args[cmd1_argc++] = argv[i];
        } else {
            cmd2_args[cmd2_argc++] = argv[i];
        }
    }

    cmd2_args[cmd2_argc] = NULL;

    int pipe_fd[2];

    if(pipe(pipe_fd) == -1){
        perror("pipe");
        exit(1);
    }

    pid_t pid1, pid2;
    int status;

    pid1 = fork();

    if(pid1 == -1) {
        perror("fork");
        exit(1);
    }
    else if(pid1 == 0) {
        close(pipe_fd[0]);

        if(dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
            perror("dup2 1");
            exit(1);
        }

        if(execvp(strcat("/usr/bin/", cmd1_args[0]), cmd1_args) == -1){
            perror("execvp 1");
            exit(1);
        }

        exit(0);
    }
    else{
        waitpid(pid1, NULL, 0);
        pid2 = fork();

        if(pid2 == -1) {
            perror("fork");
            exit(1);
        }
        else if(pid2 == 0) {
            close(pipe_fd[1]);

            if(dup2(pipe_fd[0], STDIN_FILENO) == -1) {
                perror("dup2");
                exit(1);
            }

            if(execvp(strcat("/usr/bin/", cmd2_args[0]), cmd2_args) == -1) {
                perror("execvp");
                exit(1);
            }

            exit(0);
        }
        else {
            close(pipe_fd[0]);
            close(pipe_fd[1]);

            wait(NULL);
            wait(NULL);
        }
    }

    return 0;
}