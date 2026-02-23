#ifndef COMMAND_H
#define COMMAND_H

#include <stdbool.h>

typedef struct {
    char *command;
    char *args[256];
    char *input_file;
    char *output_file;
    bool append;
    bool background;
} Command;

void free_command(Command **cmd);

#endif