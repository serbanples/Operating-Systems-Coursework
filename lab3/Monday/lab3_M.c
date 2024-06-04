#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Usage: %s <source_file> <destination_file>\n", argv[0]);
        return 1;
    }

    int source_fd = open(argv[1], O_RDONLY);
    if(source_fd == -1){
        perror("Error opening source file");
        return 1;
    }

    int destination_fd = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, S_IWUSR | S_IRUSR);
    if(destination_fd == -1 ){
        perror("Error opening destination file");
        close(source_fd);
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read, bytes_written;
    while ((bytes_read = read(source_fd, buffer, BUFFER_SIZE)) > 0) {
        bytes_written = write(destination_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing to destination file");
            close(source_fd);
            close(destination_fd);
            return 1;
        }
    }

    if(bytes_read == -1) {
        perror("Error reading from source file");
        close(source_fd);
        close(destination_fd);
        return 1;
    }

    close(source_fd);
    close(destination_fd);

    printf("File copied successfully.\n");
    return 0;
}
