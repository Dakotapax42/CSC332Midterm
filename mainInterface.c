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
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

// whitelist so users can only strictly type these choices+help/exit for errorhandling
const char *whitelist[] = {
    "filecrypt", "timedexec", "filediffadvanced", "processgroup", "loganalyzer", "history", "sleep", "cat", "ps",  NULL
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
    printf("5. processgroup     : processgroup -g <pgid> -s <signal> -w <watch group seconds>\n");
    printf("\nBuilt-in: help, history, exit\n");
    printf("----------------------------------\n\n");
}

int main() {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];
    char *line;

    using_history();
    stifle_history(50);

    printf("Starting... Please type 'help' for commands and parameters, or 'exit' to close shell.\n");

    while (1) {
        // reads input and displays ui, will continue until exit
        line = readline("myshell> ");

        // handles Ctrl+D (End of File)
        if (line == NULL) {
            printf("\nExiting shell...\n");
            break;
        }

        // adds newline to history
        if (strlen(line) > 0) {
            add_history(line);
        } else {
            free(line); // clean up empty lines and skip
            continue;
        }

        // cleans parentheses to allow (command & command) syntax
        char cleaned[MAX_INPUT];
        int j = 0;
        for (int i = 0; line[i] != '\0' && j < MAX_INPUT - 1; i++) {
            if (line[i] != '(' && line[i] != ')') {
                cleaned[j++] = line[i];
            }
        }
        cleaned[j] = '\0';

        // copies cleaned output into buffer
        strncpy(input, cleaned, MAX_INPUT - 1);
        input[MAX_INPUT - 1] = '\0';

        // frees the memory allocated by readline
        free(line);

        // logic for built in commands
        if (strcmp(input, "exit") == 0) break;
        if (strcmp(input, "help") == 0) {
            display_help();
            continue;
        }

        if (strcmp(input, "history") == 0) {
            register HIST_ENTRY **the_list;
            the_list = history_list();
            if (the_list) {
                for (int i = 0; the_list[i]; i++) {
                    printf("%d: %s\n", i + history_base, the_list[i]->line);
                }
            }
            continue;
        }

        // checks entire input and if &, clean
        int global_background = 0;
        size_t len = strlen(input);


        while (len > 0 && (input[len - 1] == ' ' || input[len - 1] == '\t')) {
            input[--len] = '\0';
        }

        if (len > 0 && input[len - 1] == '&') {
            global_background = 1;
            input[len - 1] = '\0'; // remove the &
        }

        // tokenize the input into args
        int i = 0;
        char *token = strtok(input, " \t\n\r");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " \t\n\r");
        }
        args[i] = NULL;

        if (args[0] == NULL) continue;

        // whitelist check
        if (!is_whitelisted(args[0])) {
            printf("Error: '%s' is not whitelisted.\n", args[0]);
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
        } else if (pid == 0) {
            // child process
            setpgid(0, 0); // needed for processgroup

            char path[256];
            // if it's not a system command like sleep, add ./
            if (strcmp(args[0], "sleep") != 0 && args[0][0] != '.' && args[0][0] != '/') {
                snprintf(path, sizeof(path), "./%s", args[0]);
            } else {
                strncpy(path, args[0], sizeof(path));
            }

            if (execvp(path, args) == -1) {
                execvp(args[0], args);
                perror("Execution failed");
                exit(EXIT_FAILURE);
            }
        } else {
            // parent process
            if (global_background) {
                printf("[Background Process Started: PID %d]\n", pid);
            } else {
                // wait for it to finish
                waitpid(pid, NULL, 0);
            }
        }

        // cleans up any finished background zombie processes
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }
    return 0;
}