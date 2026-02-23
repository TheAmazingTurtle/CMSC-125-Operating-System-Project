#ifndef BACKGROUND_JOB_H
#define BACKGROUND_JOB_H

#include <unistd.h>
#include "command.h"

typedef struct {
    pid_t pid;
    Command *cmd;
} BackgroundJob;

#endif