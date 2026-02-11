#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdbool.h>
#include "command.h"

bool handle_command(Command *cmd);
void cleanup_zombes();

#endif