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

    while ((opt = getopt(argc, argv, "f:")) != -1) {
        if (opt == 'f') filepath = optarg;
    }

    if (!filepath) {
        printf("Usage: loganalyzer -f <filename>\n");
        return 1;
    }

    int fd = open(filepath, O_RDONLY);
    if (fd < 0) { perror("open"); return 1; }

    struct stat s;
    fstat(fd, &s);

    if (s.st_size == 0) {
        printf("File is empty.\n");
        close(fd);
        return 0;
    }

    char *ptr = mmap(NULL, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED) { perror("mmap"); return 1; }

    size_t lines = 0;
    for (size_t i = 0; i < s.st_size; i++) {
        if (ptr[i] == '\n') lines++;
    }

    printf("Analysis Complete: %zu bytes, %zu lines found.\n", s.st_size, lines);

    munmap(ptr, s.st_size);
    close(fd);
    return 0;
}