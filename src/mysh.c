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
        cleanup_zombies();

        // ensure prompt outputs properly
        fflush(stdout);
        printf("%s", prompt);
        fflush(stdout);
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;

        char **token_arr = tokenize(buffer);
        if (token_arr == NULL) continue;

        Command *cmd = parse(token_arr);
        free_tokens(token_arr);
        if (cmd == NULL) continue;

        // handle command and check running status
        bool running_status = handle_command(cmd);
        free_command(cmd);
    }

    return 0;
}