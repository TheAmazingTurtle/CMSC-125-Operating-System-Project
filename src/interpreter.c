#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "interpreter.h"

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