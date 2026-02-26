#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "command.h"

#define MAX_PROCESSES 256

bool handle_command(Command *cmd);
void cleanup_zombies();

#endif