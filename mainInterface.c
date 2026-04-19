// testing the main interface of the program
// Created by dakot on 4/6/2026.
// my commands in case I forgot and for testing my interface structure
// cd "/cygdrive/c/Users/dakot/Documents/GitHub/CSC332Midterm"
// make clean
// make
//./myshell
// loganalyzer -f test.log
// test
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64
// whitelist so users can only strictly type these choices+help/exit for errorhandling
const char *whitelist[] = {
    "filecrypt", "timedexec", "filediffadvanced", "processgroup", "loganalyzer", NULL
};

int is_whitelisted(const char *cmd) {
    for (int i = 0; whitelist[i] != NULL; i++) {
        if (strcmp(cmd, whitelist[i]) == 0) return 1;
    }
    return 0;
}
// displays help options with example parameters 
void display_help() {
    printf("\n--- Custom Shell Command Guide ---\n");
    printf("Format: <command> [flags] [arguments]\n\n");
    printf("1. loganalyzer      : loganalyzer -f <filename>\n");
    printf("2. filecrypt        : filecrypt -f <filename> -k <key>\n");
    printf("3. timedexec        : timedexec -t <wall seconds> -c <CPU seconds> -m <MB memory> -- <command>\n");
    printf("4. filediffadvanced : filediffadvanced -a <file1> -b <file2>\n");
    printf("5. processgroup     : processgroup -g <pgid> -s <signal>\n");
    printf("\nBuilt-in: help, exit\n");
    printf("----------------------------------\n\n");
}

int main() {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];

    printf("Starting... Please type 'help' for commands and parameters, or 'exit' to close shell.\n");
    // reads input and displays ui, will continue until exit
    while (1) {
        printf("myshell> ");
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        // remove newline and ignor empty enter inputs
        input[strcspn(input, "\n")] = 0;
        if (strlen(input) == 0) continue;
        
        if (strcmp(input, "exit") == 0) break;
        if (strcmp(input, "help") == 0) {
            display_help();
            continue;
        }

        int i = 0;
        args[i] = strtok(input, " ");
        while (args[i] != NULL && i < MAX_ARGS - 1) {
            i++;
            args[i] = strtok(NULL, " ");
        }
        args[i] = NULL;
        // verifies commands are or arent in approved whitelist section
        if (!is_whitelisted(args[0])) {
            printf("Error: '%s' is not whitelisted. Type 'help' for list.\n", args[0]);
            continue;
        }
        // creates new process to execute selected utility
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("Fork failed");
        }
            
        else if (pid == 0) {
            char path[256];
            snprintf(path, sizeof(path), "./%s", args[0]);
            
            if (execvp(path, args) == -1) {
                perror("Execution failed");
                // prevents child from continuing as second shell
                exit(EXIT_FAILURE);
            }
        } else {
            wait(NULL);
        }
    }
    return 0;
}
