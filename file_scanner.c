#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

void print_file_details(const char *filename, struct stat *fileStat) {
    printf("File: %s\n", filename);
    printf("Size: %ld bytes\n", fileStat->st_size);

    printf("Permissions: ");
    printf((S_ISDIR(fileStat->st_mode)) ? "d" : "-");
    printf((fileStat->st_mode & S_IRUSR) ? "r" : "-");
    printf((fileStat->st_mode & S_IWUSR) ? "w" : "-");
    printf((fileStat->st_mode & S_IXUSR) ? "x" : "-");
    printf((fileStat->st_mode & S_IRGRP) ? "r" : "-");
    printf((fileStat->st_mode & S_IWGRP) ? "w" : "-");
    printf((fileStat->st_mode & S_IXGRP) ? "x" : "-");
    printf((fileStat->st_mode & S_IROTH) ? "r" : "-");
    printf((fileStat->st_mode & S_IWOTH) ? "w" : "-");
    printf((fileStat->st_mode & S_IXOTH) ? "x" : "-");
    printf("\nLast modified: %s", ctime(&fileStat->st_mtime));
    printf("    \n");
}

int main() {
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;
    time_t currentTime = time(NULL); // Get current time
    double seconds_in_week = 7 * 24 * 60 * 60; // Seconds in one week

    // Open the current directory
    dir = opendir(".");
    if (dir == NULL) {
        perror("opendir");
        return EXIT_FAILURE;
    }

    // Iterate over all files in the directory
    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Get file statistics
        if (stat(entry->d_name, &fileStat) == -1) {
            perror("stat");
            continue;
        }

        // Check if the file was modified more than a week ago
        double diff = difftime(currentTime, fileStat.st_mtime);
        if (diff > seconds_in_week) {
            print_file_details(entry->d_name, &fileStat);
        }
    }

    // Close the directory
    closedir(dir);

    return 0;
}
