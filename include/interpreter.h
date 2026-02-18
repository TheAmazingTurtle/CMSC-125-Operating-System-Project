#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "command.h"

#define MAX_JOBS 256

void handle_command(Command *cmd);
void cleanup_zombies();

#endif