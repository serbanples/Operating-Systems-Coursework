#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]) {

    if(argc < 1){
        fprintf(stderr, "Usage: %s <command> > <file> \n", argv[0]);
        exit(1);
    }

    char *cmd_args[argc];
    char *file_name = argv[argc - 1];
    int cmd_argc = 0;

    int is_redirect = 0;

    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], ">") == 0) {
            cmd_args[cmd_argc] = NULL;
            is_redirect = 1;
            continue;
        }

        if (!is_redirect) {
            cmd_args[cmd_argc++] = argv[i];
        }
    }

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

        close(pipe_fd[1]);

        if(execvp(strcat("/usr/bin/", cmd_args[0]), cmd_args) == -1){
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
            waitpid(pid1, NULL, 0);

            close(pipe_fd[1]);
            
            int fd = open(file_name, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
            if(fd == -1) {
                perror("open");
                exit(1);
            }

            FILE *stream;
            stream = fdopen(pipe_fd[0],"r");

            char string[1024];
            fscanf(stream, "%s", string);

            int bytes_written = write(fd, string, strlen(string));

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