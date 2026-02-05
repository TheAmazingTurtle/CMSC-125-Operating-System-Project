#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "mysh.h"

char **tokenize(char *line) {
    char **token_arr  = malloc(sizeof(char *) * 256);
    if (!token_arr) return NULL;                        // check if memory is allocated

    char *token;

    line[strcspn(line, "\n")] = 0;                      // to strip \n from line

    token = strtok(line, " ");
    if (token == NULL) {
        free(token_arr);
        return NULL;
    }

    int i = 0;
    while (token != NULL && i < 255) {                  // limit to 255, leave space for NULL
        token_arr[i++] = token;
        token = strtok(NULL, " ");
    }
    token_arr[i] = NULL;

    return token_arr;
}

Command *parse(char **token_arr) {
    Command *cmd = calloc(1, sizeof *cmd);

    cmd->command = token_arr[0];
    cmd->args[0] = token_arr[0];

    int i = 1;
    while (token_arr[i]){
        char *cur_token = token_arr[i];
        char *next_token = token_arr[i+1];

        if (strcmp(cur_token, ">") == 0 || strcmp(cur_token, ">>") == 0) {
            if (cmd->output_file || !next_token) {
                printf("syntax error: invalid output redirection\n");
                return NULL;
            }

            cmd->append = strcmp(cur_token, ">>") == 0;
            cmd->output_file = next_token;
            i += 2;
            continue;
        }
        
        if (strcmp(cur_token, "<") == 0) {
            if (cmd->input_file || !next_token) {
                printf("syntax error: invalid input redirection\n");
                return NULL;
            }
            cmd->input_file = next_token;
            i += 2;
            continue;
        }
        
        if (strcmp(cur_token, "&") == 0) {
            if (next_token) {
                printf("syntax error: '&' must appear at end of command\n");
                return NULL;
            }
            cmd->background = true;
            i++;
            continue;
        } 

        cmd->args[i++] = cur_token;
    }

    free(token_arr);                                    // prevent memory leak

    return cmd;
}

bool handle_command(Command *cmd) {
    if (strcmp(cmd->command, "echo") == 0) {
        // sample echo implementation
        for (int i = 1; cmd->args[i] != NULL; i++) {
            printf("%s ", cmd->args[i]);
        }
        printf("\n");
    } else if (strcmp(cmd->command, "pwd") == 0) {
        // uses getcwd() from unistd.h
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("getcwd() error");
        }
    } else if (strcmp(cmd->command, "cd") == 0) {
        // uses chdir() from unistd.h
        if (cmd->args[1] == NULL) {
            //  no argument provided
            printf("mysh: missing operand for command: cd\n");
        } else {
            if (chdir(cmd->args[1]) == 0) {
                printf("Changing directory...\n");
            } else {
                perror("mysh");
            }
        }
    } else if (strcmp(cmd->command, "exit") == 0) {
        // terminates shell
        printf("Exiting mysh...\n");
        return false;
    } else {
        // invalid command
        printf("mysh: command not found: %s\n", cmd->command);
    }
    return true;
}

int main() {
    // initialize stuff
    char buffer[256];
    char *prompt = "mysh> ";

    // input loop
    while (true) {
        printf("%s", prompt);
        fgets(buffer, sizeof(buffer), stdin);

        char **token_arr = tokenize(buffer);
        if (token_arr == NULL) continue;

        Command *cmd = parse(token_arr);
        if (cmd == NULL) continue;

        // debug print
        printf("Command: %s\nArgs:\n%s", cmd->command, (cmd->args[1] ? "" : "(none)\n"));
        for (int i = 1; cmd->args[i] != NULL; i++) {
            printf("%d) %s\n", i, cmd->args[i]);
        }
        printf("Output:\n");

        // handle command and check running status
        if (!handle_command(cmd)) break;
    }

    return 0;
}