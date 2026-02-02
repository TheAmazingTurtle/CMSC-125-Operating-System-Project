#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "mysh.h"

Command parse_line(char *line) {
    // clean line and clear struct
    line[strcspn(line, "\n")] = 0;
    Command cmd = {0};
    
    // tokenize command
    char *token = strtok(line, " ");
    if (token == NULL) return cmd;

    // get command name
    cmd.command = token;
    cmd.args[0] = token;

    // get command arguments
    int i = 1;
    token = strtok(NULL, " ");
    while (token != NULL && i < 255) {
        cmd.args[i++] = token;
        token = strtok(NULL, " ");
    }

    // ensure args is NULL-terminated
    cmd.args[i] = NULL;

    // todo:
    // - interpret command symbols
    // - fill in missing fields

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
    printf("%s", prompt);

    // input loop
    while (fgets(buffer, sizeof(buffer), stdin)) {
        Command cmd = parse_line(buffer);

        // handle invalid input
        if (cmd.command == NULL) break;

        // debug print
        printf("Command: %s\nArgs:\n%s", cmd.command, (cmd.args[1] ? "" : "(none)\n"));
        for (int i = 1; cmd.args[i] != NULL; i++) {
            printf("%d) %s\n", i, cmd.args[i]);
        }
        printf("Output:\n");

        // handle command and check running status
        if (!handle_command(&cmd)) break;

        // next prompt
        printf("%s", prompt);
    }

    return 0;
}