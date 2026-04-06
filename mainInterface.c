// testing the main interface of the program
// Created by dakot on 4/6/2026.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

const char *whitelist[] = {
    "filecrypt", "timedexec", "filediffadvanced", "processgroup", "loganalyzer", NULL
};

int is_whitelisted(const char *cmd) {
    for (int i = 0; whitelist[i] != NULL; i++) {
        if (strcmp(cmd, whitelist[i]) == 0) return 1;
    }
    return 0;
}

void display_help() {
    printf("\n--- Custom Shell Command Guide ---\n");
    printf("Format: <command> [flags] [arguments]\n\n");
    printf("1. loganalyzer      : loganalyzer -f <filename>\n");
    printf("2. filecrypt        : filecrypt -f <filename> -k <key>\n");
    printf("3. timedexec        : timedexec -t <seconds> -c <command>\n");
    printf("4. filediffadvanced : filediffadvanced -a <file1> -b <file2>\n");
    printf("5. processgroup     : processgroup -g <pgid> -s <signal>\n");
    printf("\nBuilt-in: help, exit\n");
    printf("----------------------------------\n\n");
}

int main() {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];

    printf("Starting Custom OS Shell... Type 'help' for commands.\n");

    while (1) {
        printf("myshell> ");
        if (fgets(input, sizeof(input), stdin) == NULL) break;

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

        if (!is_whitelisted(args[0])) {
            printf("Error: '%s' is not whitelisted. Type 'help' for list.\n", args[0]);
            continue;
        }

        pid_t pid = fork();
        if (pid == 0) {
            char path[256];
            snprintf(path, sizeof(path), "./%s", args[0]);
            if (execvp(path, args) == -1) {
                perror("Execution failed");
                exit(EXIT_FAILURE);
            }
        } else {
            wait(NULL);
        }
    }
    return 0;
}