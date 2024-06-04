#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    
    if(argc < 2) {
        fprintf(stderr, "Usage: %s <source_file>", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t child_pid;
    int status;

    for(int i = 1; i < argc; i++) {
        child_pid = fork();

        if(child_pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if(child_pid == 0) {
            char *src_file = argv[i];
            char *out_file = malloc(strlen(src_file) + 5); 
            strcpy(out_file, src_file); 
            char *ext = strrchr(out_file, '.'); 
            if (ext != NULL) {
                strcpy(ext, ".out"); 
            } else {
                strcat(out_file, ".out"); 
            }

            char *compiler_args[] = { "gcc", src_file, "-o", out_file, NULL };
            if(execvp("/usr/bin/gcc", compiler_args) == -1) {
                perror("execvp failed 1");
                exit(EXIT_FAILURE);
            }
            free(out_file);
        }
        else {
            waitpid(child_pid, &status, 0);

            if(WIFEXITED(status)) {
                int exit_status = WEXITSTATUS(status);

                if(exit_status == 0) {
                    pid_t bin_pid = fork();

                    if(bin_pid == -1) {
                        perror("fork");
                        exit(EXIT_FAILURE);
                    }

                    if(bin_pid == 0) {
                        char *src_file = argv[i];
                        char *out_file = malloc(strlen(src_file) + 5);
                        out_file = "./"; 
                        strcat(out_file, src_file); 
                        char *ext = strrchr(out_file, '.'); 

                        if (ext != NULL) {
                            strcpy(ext, ".out"); 
                        } else {
                            strcat(out_file, ".out"); 
                        }
                        char *bin_args[] = { out_file, NULL };

                        if(execvp(out_file, bin_args) == -1) {
                            perror("execvp failed 2");
                            exit(EXIT_FAILURE);
                        }
                    }
                    else {
                        waitpid(bin_pid, &status, 0);
                        printf("Process %d executed with exit code: %d\n", bin_pid, WEXITSTATUS(status));
                    }
                }
                else {
                    printf("Compilation of %s failed with exit code: %d\n", argv[i], exit_status);
                }
            }
            else {
                printf("Child process terminated abnormally\n");
            }
        }
    }

    return 0;
}