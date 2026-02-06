#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "command.h"

#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

int main() {
    char buffer[256];
    char *prompt = "mysh> ";

    while (true) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;

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
        if (!handle_command(cmd)) {
            free(cmd);   
            break;
        }

        free(cmd);
    }

    return 0;
}