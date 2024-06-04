#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char * argv[]) {

    pid_t pid;
    int status;
    
    for(int i = 0; i < 19; i++) {
        pid = fork();

        if(pid == -1){
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if(pid == 0){
            for(int j = 0; j < 10; j++){
                printf("Child %d - PID: %d\n", i + 1, getpid());
            }
            exit(i + 1);
        }
    }

    printf("Parent - PID: %d\n", getpid());

    for(int i = 0; i < 19; i++){
        pid = wait(&status);
        if(pid != -1){
            if(WIFEXITED(status)) {
                printf("Child %d exited with status: %d\n", i + 1 , WEXITSTATUS(status));
            }
            else{
                printf("Child %d exited abnormally\n", i + 1);
            }
        }
        else {
            perror("wait");
        }
    }

    return 0;
}