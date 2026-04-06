#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    int opt;
    // update the parameters of the string "f:k:t:c:a:b:g:s:" to ONLY include the letters assigned to their specific feature.
    while ((opt = getopt(argc, argv, "f:k:t:c:a:b:g:s:")) != -1) {
        switch (opt) {
            case 'f': printf("File: %s\n", optarg); break;
            case 'k': printf("Key: %s\n", optarg); break;
            // Add other cases as needed
            default:
                fprintf(stderr, "Usage: Check 'help' in shell for correct flags.\n");
            return 1;
        }
    }

    printf("Feature logic executing now...\n");
    return 0;
}