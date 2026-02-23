#include <stdlib.h>

#include "command.h"

void free_command(Command **cmd) {
    if (!cmd || !*cmd) return;

    free((*cmd)->command);

    for (int i = 0; (*cmd)->args[i]; i++) {
        free((*cmd)->args[i]);
    }

    free((*cmd)->input_file);
    free((*cmd)->output_file);

    free(*cmd);
    *cmd = NULL;
}