#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

// Forward declaration
unsigned int encrypt(char c, char key);

int main(int argc, char *argv[]) {
    int opt;
    char *filename = NULL;
    char *key = NULL;

    //Flags for filecrypt
    while ((opt = getopt(argc, argv, "f:k:")) != -1) {
        switch (opt) {
            case 'f':
                filename = optarg;
                break;
            case 'k':
                key = optarg;
                break;
            default:
                fprintf(stderr, "Usage: Check 'help' in shell for correct flags.\n");
            return 1;
        }
    }


    // Open the file for reading
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return 1;
    }

    int c;
    int key_len = strlen(key);
    int key_index = 0;

    // Encrypt each character using a rotating key 
    while ((c = fgetc(fp)) != EOF) { //read each character until EOF
        putchar(encrypt((char)c, key[key_index % key_len])); //encrypt the character and print it, using modulo to rotate through the key
        key_index++;
    }

    printf("\n");

    // Secure memory management: zero out the key before exiting
    memset(key, 0, strlen(key));

    fclose(fp);
    return 0;
}

// Encrypts a character by XOR-ing with the key character 
unsigned int encrypt(char c, char key) {  
    return (unsigned int)((unsigned char)c ^ (unsigned char)key);
}