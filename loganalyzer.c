//
// Created by dakot on 4/6/2026.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    int opt;
    char *filepath = NULL;
	// using getopt to parse the command line arguments, looking for the -f flag followed by filename
    while ((opt = getopt(argc, argv, "f:")) != -1) {
        if (opt == 'f') filepath = optarg;
    }
	// if filepath is not provided, print usage and exit
    if (!filepath) {
        printf("Usage: loganalyzer -f <filename>\n");
        return 1;
    }
	// open the file for reading only, if it fails print error and exit
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) { perror("open"); return 1; }

    struct stat s;
    fstat(fd, &s);
	// edge case: if file is empty, print message then exit
    if (s.st_size == 0) {
        printf("File is empty.\n");
        close(fd);
        return 0;
    }
	// map the file into memory for efficient access
    // PROT_READ: data can be read but NOT modified
    // MAP_PRIVATE: changes to the mapped data are private to the process and NOT written back to the file
    char *ptr = mmap(NULL, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED) { perror("mmap"); return 1; }
	// analyze the file by counting the number of lines and print the results
    size_t lines = 0;
    for (size_t i = 0; i < s.st_size; i++) {
        if (ptr[i] == '\n') lines++;
    }

    printf("Analysis Complete: %zu bytes, %zu lines found.\n", s.st_size, lines);
 	// cleanup to avoid memory leaks and close file descriptor
    munmap(ptr, s.st_size);
    close(fd);
    return 0;
}