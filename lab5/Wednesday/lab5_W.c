#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    
    pid_t pid;
    int status;

    pid = fork();
    if(pid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0){
        int fd = open("file.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
        if(fd == -1){
            perror("open");
            exit(EXIT_FAILURE);
        }
        
        for(int i = 0; i < 100; i++){
            for(int j = 1; j < argc; j++){
                dprintf(fd, "%s ", argv[j]);
            }
            dprintf(fd,"\n");
        }

        printf("Child finished!\n");

        close(fd);

        exit(EXIT_SUCCESS);
    }
    else{
        waitpid(pid, &status, 0);
        
        int fd = open("file.txt", O_WRONLY | O_APPEND);
        if(fd == -1){
            perror("open");
            exit(EXIT_FAILURE);
        }

        for(int i = 0; i < 100; i++){
            for(int j = argc - 1; j > 0; j--){
                dprintf(fd, "%s ", argv[j]);
            }
            dprintf(fd, "\n");
        }

        printf("Parent finished!\n");

        close(fd);
    }

    return 0;
}