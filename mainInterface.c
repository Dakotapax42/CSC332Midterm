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
    "filecrypt", "timedexec", "filediffadvanced", "processgroup", "loganalyzer", "history", "sleep",  NULL
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
    printf("\nBuilt-in: help, exit\n");
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

        // copies cleaned output into your existing 'input' buffer
        strncpy(input, cleaned, MAX_INPUT - 1);
        input[MAX_INPUT - 1] = '\0';

        // frees the memory allocated by readline
        free(line);

        // Logic for exit and help
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

        // checks if the entire input ends with an ampersand for backgrounding
        int global_background = 0;
        size_t len = strlen(input);
        if (len > 0 && input[len-1] == '&') {
            global_background = 1;
        }

        // splits input by & to handle multiple background commands
        char *cmd_ptr;
        char *saveptr1;
        cmd_ptr = strtok_r(input, "&", &saveptr1);

        while (cmd_ptr != NULL) {
            // trims leading spaces from command
            while (*cmd_ptr == ' ') cmd_ptr++;
            if (*cmd_ptr == '\0') {
                cmd_ptr = strtok_r(NULL, "&", &saveptr1);
                continue;
            }

            int i = 0;
            char *saveptr2;
            args[i] = strtok_r(cmd_ptr, " ", &saveptr2);
            while (args[i] != NULL && i < MAX_ARGS - 1) {
                i++;
                args[i] = strtok_r(NULL, " ", &saveptr2);
            }
            args[i] = NULL;

            if (args[0] == NULL) {
                cmd_ptr = strtok_r(NULL, "&", &saveptr1);
                continue;
            }

            if (!is_whitelisted(args[0])) {
                printf("Error: '%s' is not whitelisted. Type 'help' for list.\n", args[0]);
                cmd_ptr = strtok_r(NULL, "&", &saveptr1);
                continue;
            }

            pid_t pid = fork();
            if (pid < 0) {
                perror("Fork failed");
            } else if (pid == 0) {
                // sets the child process to its own group so processgroup can find it
                setpgid(0, 0);

                char path[256];
                // Check if command is a relative path or needs ./ prefix
                if (args[0][0] == '.' || args[0][0] == '/') {
                    strncpy(path, args[0], sizeof(path));
                } else {
                    snprintf(path, sizeof(path), "./%s", args[0]);
                }

                if (execvp(path, args) == -1) {
                    // Fallback to system PATH if local execution fails (useful for 'sleep')
                    execvp(args[0], args);
                    perror("Execution failed");
                    exit(EXIT_FAILURE);
                }
            } else {
                // determines if this specific command should be backgrounded
                if ((saveptr1 && *saveptr1 != '\0') || global_background) {
                    printf("[Process running in background with PID %d]\n", pid);
                } else {
                    waitpid(pid, NULL, 0);
                }
            }
            cmd_ptr = strtok_r(NULL, "&", &saveptr1);
        }

        // Clean up "zombie" background processes
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }
    return 0;
}