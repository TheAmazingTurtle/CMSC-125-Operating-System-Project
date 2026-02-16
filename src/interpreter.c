#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "interpreter.h"

#define MAX_JOBS 100
pid_t background_jobs[MAX_JOBS];
int job_count = 0;

const char *builtin_commands[] = {
    "pwd",
    "cd",
    "exit"
};

bool is_builtin_command(const char *cmd);
bool handle_builtin_command(Command *cmd);

bool handle_command(Command *cmd) {
    if (is_builtin_command(cmd->command)) {
        return handle_builtin_command(cmd);

    } else {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            return -1;
        }

        if (pid == 0) { // child process
            // use input_file and output_file to redirect stdin and stdout
            if (cmd->input_file) {
                int fd = open(cmd->input_file, O_RDONLY);
                if (fd < 0) {
                    perror("open input file");
                    exit(1);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            if (cmd->output_file) {
                int flags = O_WRONLY | O_CREAT;
                flags |= cmd->append ? O_APPEND : O_TRUNC;
                int fd = open(cmd->output_file, flags, 0644);
                if (fd < 0) {
                    perror("open output file");
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            // execute command using execvp()
            execvp(cmd->command, cmd->args);
            perror("exec failed");
            exit(127);

        } else {  // parent process
            // debug print background_jobs
            // printf("Current background jobs:\n");
            // for (int i = 0; i < job_count; i++) {
            //     printf("  [%d] PID: %d\n", i + 1, background_jobs[i]);
            // }

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
    }

    return true;
}

bool is_builtin_command(const char *cmd) {
    for (size_t i = 0; i < sizeof(builtin_commands) / sizeof(builtin_commands[0]); i++) {
        if (strcmp(cmd, builtin_commands[i]) == 0) {
            return true;
        }
    }
    return false;
}

bool handle_builtin_command(Command *cmd) {
    if (strcmp(cmd->command, "exit") == 0) {
        // terminates shell
        printf("Exiting mysh...\n");
        return false;
    }

    if (strcmp(cmd->command, "pwd") == 0) {
        // uses getcwd() from unistd.h
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("getcwd() error");
        }
    }
    
    if (strcmp(cmd->command, "cd") == 0) {
        // uses chdir() from unistd.h
        if (cmd->args[1] == NULL) {
            // no argument provided
            printf("mysh: missing operand for command: cd\n");
        } else {
            if (chdir(cmd->args[1]) == 0) {
                printf("Changing directory...\n");
            } else {
                perror("mysh");
            }
        }
    }
    
    return true;
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