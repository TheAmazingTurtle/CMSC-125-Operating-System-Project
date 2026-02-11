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
        printf("Command: %s\nArgs: ", cmd->command);
        for (int i = 0; cmd->args[i] != NULL; i++) {
            printf("%s ", cmd->args[i]);
        }
        printf("\nInput file: %s\n", cmd->input_file ? cmd->input_file : "");
        printf("Output file: %s\n", cmd->output_file ? cmd->output_file : "");
        printf("Append: %s\n", cmd->append ? "true" : "false");
        printf("Background: %s\n", cmd->background ? "true" : "false");
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