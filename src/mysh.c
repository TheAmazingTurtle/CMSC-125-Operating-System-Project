#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "command.h"

#include "history.h"
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
        buffer[strcspn(buffer, "\n")] = 0;                              // to strip \n from line

        add_history_entry(buffer);
        char **token_arr = tokenize(buffer);
        if (token_arr == NULL) continue;

        Command *cmd = parse(token_arr);
        free_tokens(token_arr);
        if (cmd == NULL) continue;

        if (handle_command(cmd)) free_command(cmd);                     // check if command is done being used, free if true
    }

    return 0;
}