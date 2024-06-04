#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

void listFilesAndSizes(const char *dirname) {
    DIR *dir;
    struct dirent *entry;

    if(!(dir = opendir(dirname))) {
        perror("opendir");
        return;
    }

    while((entry = readdir(dir)) != NULL) {
        char path[512];
        snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);
        struct stat fileStat;
        if(lstat(path, &fileStat) < 0){
            perror("stat");
            return;
        }

        if(S_ISDIR(fileStat.st_mode)) {
            if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
                listFilesAndSizes(path);
            }
        }
        else if(S_ISREG(fileStat.st_mode)) {
            printf("%s Size: %ld bytes\n", path, (long)fileStat.st_size);
        }
    }

    closedir(dir);

}

int main(int argc, char *argv[]){

    if(argc != 2){
        fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]);
        return 2;
    }

    const char *dirname = argv[1];
    listFilesAndSizes(dirname);

    return 0;
}