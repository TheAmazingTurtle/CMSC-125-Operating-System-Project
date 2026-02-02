#include <stdio.h>
#include <string.h>
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

int main() {
    // initialize stuff
    char buffer[256];
    char *prompt = "mysh> ";
    printf("%s", prompt);

    // input loop
    while (fgets(buffer, sizeof(buffer), stdin)) {
        Command cmd = parse_line(buffer);

        // handle command
        if (
            cmd.command == NULL ||
            strcmp(cmd.command, "exit") == 0
        ) {
            break;
        }

        // debug print
        printf("Command: %s\nArgs:\n", cmd.command);
        for (int i = 1; cmd.args[i] != NULL; i++) {
            printf("  %d) %s\n", i, cmd.args[i]);
        }

        // next prompt
        printf("%s", prompt);
    }

    return 0;
}