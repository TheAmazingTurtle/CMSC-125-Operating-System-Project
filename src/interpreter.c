#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>

#include "interpreter.h"

pid_t background_jobs[MAX_JOBS];
int job_count = 0;

static bool execute_builtin_command(Command *cmd);
static bool launch_external_command(Command *cmd);

void handle_command(Command *cmd) {
    if (execute_builtin_command(cmd)) return;

    launch_external_command(cmd);
}


void cleanup_zombies() {
    for (int i = 0; i < job_count; i++) {
        int status;
        pid_t result = waitpid(background_jobs[i], &status, WNOHANG);

        if (result > 0) {
            // result is PID of completed child process
            printf("\n[Background job %d completed]\n", result);
            
            // shift remaining jobs to the left to remove completed job
            for (int j = i; j < job_count - 1; j++) {
                background_jobs[j] = background_jobs[j + 1];
            }
            job_count--;
            i--;
        } else if (result == -1) {
            perror("waitpid failed");
        }
    }
}

static bool execute_builtin_command(Command *cmd) {
    if (strcmp(cmd->command, "exit") == 0) {
        printf("Exiting mysh...\n");
        exit(0);
    }

    if (strcmp(cmd->command, "pwd") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("getcwd() error");
        }

        return true;
    }
    
    if (strcmp(cmd->command, "cd") == 0) {
        if (cmd->args[1] == NULL) {
            printf("mysh: Missing operand for command: cd\n");
        } else if (chdir(cmd->args[1]) == 0) {
            printf("Changing directory...\n");
        } else {
            perror("mysh");
        }

        return true;
    }

    return false;
}

static bool launch_external_command(Command *cmd) {
    if (job_count >= MAX_JOBS) {
        printf("mysh: Maximum background job limit reached\n");
        return;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return;
    }

    if (pid == 0) {     // child process
        if (cmd->input_file) {
            int fd = open(cmd->input_file, O_RDONLY);
            if (fd < 0) {
                perror("open input file");
                _exit(1);
            }

            if (dup2(fd, STDIN_FILENO) == -1) {
                perror("dup2 failed");
                _exit(EXIT_FAILURE);
            }
            close(fd);
        }
        if (cmd->output_file) {
            int flags = O_WRONLY | O_CREAT;
            flags |= cmd->append ? O_APPEND : O_TRUNC;
            int fd = open(cmd->output_file, flags, 0644);
            if (fd < 0) {
                perror("open output file");
                _exit(1);
            }

            if (dup2(fd, STDOUT_FILENO) == -1) {
                perror("dup2 failed");
                _exit(EXIT_FAILURE);
            }
            close(fd);
        }

        // execute command using execvp()
        execvp(cmd->command, cmd->args);

        if (errno == ENOENT) {
            fprintf(stderr, "Command '%s' not found\n", cmd->command);
        } else if (errno == EACCES) {
            fprintf(stderr, "Permission denied for command '%s'\n", cmd->command);
        } else {
            perror("execvp");
        }

        _exit(127);
    }
      
    // parent process
    if (!cmd->background) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code != 0) {
                printf("Command exited with code %d\n", exit_code);
            }
        }
    } else {
        printf("[%d] Started: %s (PID: %d)\n", getpid(), cmd->command, pid);
        background_jobs[job_count++] = pid;
    }
}