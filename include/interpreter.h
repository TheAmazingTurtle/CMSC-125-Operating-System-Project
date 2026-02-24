#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "command.h"

void handle_command(Command *cmd);
void cleanup_zombies();

#endif