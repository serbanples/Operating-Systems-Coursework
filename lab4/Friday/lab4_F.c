#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>

#define BUFFER_SIZE 4096

void copy_directory(const char *source, const char *destination) {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char src_path[PATH_MAX];
    char dest_path[PATH_MAX];

    if((dir = opendir(source)) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    if(mkdir(destination, S_IRUSR | S_IWUSR | S_IXUSR) == -1) {
        if(errno == EEXIST){
            fprintf(stderr, "Error: Destination directory already exists.\n");
            closedir(dir);
            exit(EXIT_FAILURE);
        }
        else{
            perror("mkdir");
            closedir(dir);
            exit(EXIT_FAILURE);
        }
    }

    while((entry = readdir(dir)) != NULL) {
        snprintf(src_path, PATH_MAX, "%s%s", source, entry->d_name);
        snprintf(dest_path, PATH_MAX, "%s%s", destination, entry->d_name);

        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if(lstat(src_path, &statbuf) == -1){
            perror("lstat");
            closedir(dir);
            exit(EXIT_FAILURE);
        }

        if(S_ISDIR(statbuf.st_mode)) {
            copy_directory(src_path, dest_path);
        }
        else if (S_ISLNK(statbuf.st_mode)){
            if(symlink(src_path, dest_path) == -1){
                perror("symlink");
                closedir(dir);
                exit(EXIT_FAILURE);
            }
        }
        else {
            FILE *source_file = fopen(src_path, "rb");
            FILE *dest_file = fopen(dest_path, "wb");

            if(source_file == NULL || dest_file == NULL) {
                perror("fopen");
                if (source_file != NULL) fclose(source_file);
                if (dest_file != NULL) fclose(dest_file);
                closedir(dir);
                exit(EXIT_FAILURE);
            }

            char buffer[BUFFER_SIZE];
            size_t n;

            while((n = fread(buffer, 1, sizeof(buffer), source_file)) > 0) {
                if(fwrite(buffer, 1, n, dest_file) != n) {
                    perror("fwrite");
                    fclose(source_file);
                    fclose(dest_file);
                    closedir(dir);
                    exit(EXIT_FAILURE);
                }
            }
            fclose(source_file);
            fclose(dest_file);
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[]){
    if(argc != 3){
        fprintf(stderr, "Usage: %s <source_directory> <destination_directory>", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *source = argv[1];
    const char *destination = argv[2];

    struct stat statbuf;
    if(lstat(source, &statbuf) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);
    }

    if(!S_ISDIR(statbuf.st_mode)){
        fprintf(stderr, "Error: Source is not a directory.\n");
        exit(EXIT_FAILURE);
    }

    copy_directory(source, destination);

    printf("Directory copied successfully.\n");

    return 0;
}