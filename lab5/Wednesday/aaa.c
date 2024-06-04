#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    
    pid_t pid1, pid2;
    char muie[5] = "muie";
    int status;

    pid1 = fork();
    if(pid1 == -1){ //verific eroare prunc 1
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(pid1 == 0){ //ma ocup de prunc 1
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
    else{ // ma intorc la parinte 
        waitpid(pid1, &status, 0);
        pid2 = fork();

        if(pid2 == -1){ //verific eroare prunc 2
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if(pid2 == 0) { //ma ocup prunc 2
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
            printf("Child 2 finished!\n");
            close(fd);
            exit(EXIT_SUCCESS);

        }
        else{ //ma intorc la parinte
            waitpid(pid1, &status, 0);
            waitpid(pid2, &status, 0);
            int fd = open("file.txt", O_WRONLY | O_APPEND);
            if(fd == -1){
                perror("open");
                exit(EXIT_FAILURE);
            }
            dprintf(fd, "%s \n",muie);
            printf("Parent finished!\n");
            close(fd);
        }
    }

    return 0;
}