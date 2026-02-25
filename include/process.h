#ifndef PROCESS_H
#define PROCESS_H

#include "command.h"
#include <unistd.h>

typedef struct {
    pid_t pid;
    Command *cmd_ptr;
    int is_active;
} Process;

#endif