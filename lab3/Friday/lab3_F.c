#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    if (argc != 4) {
        printf("Usage: %s <source_file> -s <size>\n", argv[0]);
        return 1;
    }

    int source_fd = open(argv[1], O_RDONLY);
    if(source_fd == -1){
        perror("Error opening source file");
        return 1;
    }

    char *svalue = argv[3];
    int SIZE = atoi(svalue);

    char buffer[SIZE];
    size_t bytes_read, bytes_written;
    int file_count = 1;
    char out_file_name[100];

    while ((bytes_read = read(source_fd, buffer, SIZE)) > 0) {

        snprintf(out_file_name, 100, "%s%d", argv[1], file_count++);

        int destination_fd = open(out_file_name, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
        if(destination_fd == -1){
            perror("Error opening destination file");
            close(source_fd);
            close(destination_fd);
            return 1;
        }

        bytes_written = write(destination_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing to destination file");
            close(source_fd);
            close(destination_fd);
            return 1;
        }
        
        close(destination_fd);
    }

    if(bytes_read == -1) {
        perror("Error reading from source file");
        close(source_fd);
        return 1;
    }

    close(source_fd);

    printf("File separated successfully.\n");
    return 0;
}
