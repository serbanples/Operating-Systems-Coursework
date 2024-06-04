#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

void remove_whitespace(char *str) {
    int i, j = 0;
    for (i = 0; str[i]; i++) {
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r') {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}

void save_letter_case_and_digits(char *str) {
    int i, j = 0;
    for (i = 0; str[i]; i++) {
        if ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= '0' && str[i] <= '9')) {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}

void remove_digits(char *str) {
    int i, j = 0;
    for (i = 0; str[i]; i++) {
        if (str[i] < '0' || str[i] > '9') {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}

int main(int argc, char *argv[]) {
    if(argc != 2){
        fprintf(stderr, "Usage %s <file>", argv[0]);
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);
    if(fd == -1){
        perror("open");
        exit(1);
    }
    
    int pipe_p_1[2], pipe_1_2[2], pipe_2_3[2];

    if(pipe(pipe_p_1) == -1 || pipe(pipe_1_2) == -1 || pipe(pipe_2_3) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid1, pid2, pid3;
    pid1 = fork();

    if(pid1 == -1) {
        perror("fork");
        exit(1);
    }
    else if(pid1 == 0) {
        close(pipe_p_1[1]);
        close(pipe_1_2[0]);

        char buffer[BUFFER_SIZE];
        int bytes_read = read(pipe_p_1[0], buffer, BUFFER_SIZE);
        if(bytes_read == -1){
            perror("read");
            exit(1);
        }

        buffer[bytes_read] = '\0';

        remove_whitespace(buffer);

        write(pipe_1_2[1], buffer, bytes_read);

        close(pipe_p_1[0]);
        close(pipe_1_2[1]);

        exit(0);
    }
    else{
        pid2 = fork();

        if(pid2 == -1){
            perror("fork");
            exit(1);
        }
        else if(pid2 == 0) {
            close(pipe_1_2[1]);
            close(pipe_2_3[0]);

            char buffer[BUFFER_SIZE];
            int bytes_read = read(pipe_1_2[0], buffer, BUFFER_SIZE);

            if(bytes_read == -1){
                perror("read");
                exit(1);
            }

            buffer[bytes_read] = '\0';
            save_letter_case_and_digits(buffer);

            write(pipe_2_3[1], buffer, bytes_read);

            close(pipe_1_2[0]);
            close(pipe_2_3[1]);

            exit(0);
        }
        else {
            pid3 = fork();

            if(pid3 == -1){
                perror("fork");
                exit(1);
            }
            else if(pid3 == 0) {
                close(pipe_2_3[1]);

                char buffer[BUFFER_SIZE];
                int bytes_read = read(pipe_2_3[0], buffer, BUFFER_SIZE);

                if(bytes_read == -1) {
                    perror("read");
                    exit(1);
                }

                buffer[bytes_read] = '\0';
                remove_digits(buffer);

                printf("%s\n", buffer);

                close(pipe_2_3[0]);
                
                exit(0);
            }
            else {
                close(pipe_p_1[0]);
                close(pipe_1_2[0]);
                close(pipe_1_2[1]);
                close(pipe_2_3[0]);
                close(pipe_2_3[1]);

                char buffer[BUFFER_SIZE];
                int bytes_read;
                while((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
                    write(pipe_p_1[1], buffer, bytes_read);
                }

                close(fd);

                close(pipe_p_1[1]);

                wait(NULL);

                exit(0);
            }
        }
    }

    return 0;
}