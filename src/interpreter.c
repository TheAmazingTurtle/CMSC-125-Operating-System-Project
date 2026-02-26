#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>

#include "interpreter.h"
#include "process.h"
#include "history.h"


static Process process_table[MAX_PROCESSES];
static int process_count = 0;

static bool execute_builtin_command(Command *cmd);
static bool launch_external_command(Command *cmd);
static void terminate_child(const char *error_message, Command *cmd, int exit_status);

bool handle_command(Command *cmd) {
    return execute_builtin_command(cmd) || launch_external_command(cmd);
}

void cleanup_zombies() {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {                     // ensures all finished jobs in this interation is exorcised
        printf("\n[Background job %d completed]\n", pid);
        
        for (int i = 0; i < process_count; i++) {                               // search-and-replace, faster implementation than shifting
            if (process_table[i].pid == pid) {
                process_table[i].is_active = 0;
                free_command(process_table[i].cmd_ptr);
                process_table[i] = process_table[--process_count];
                break;
            }
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

    if (strcmp(cmd->command, "history") == 0) {
        if (cmd->args[1] == NULL) {
            print_history();
        } else if (strcmp(cmd->args[1], "-c") == 0) {
            clear_history();
        } else {
            fprintf(stderr, "history: invalid option: %s\n", cmd->args[1]);
        }

        return true;
    }

    return false;
}

static bool launch_external_command(Command *cmd) {
    if (process_count >= MAX_PROCESSES) {
        printf("mysh: Maximum background job limit reached\n");
        return true;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return true;
    }

    if (pid == 0) {     // child process
        if (cmd->input_file) {
            int fd = open(cmd->input_file, O_RDONLY);
            if (fd < 0) terminate_child("open input", cmd, EXIT_FAILURE);
            if (dup2(fd, STDIN_FILENO) == -1) terminate_child("dup2 failed", cmd, EXIT_FAILURE);
            close(fd);
        }
        if (cmd->output_file) {
            int flags = O_WRONLY | O_CREAT;
            flags |= cmd->append ? O_APPEND : O_TRUNC;
            int fd = open(cmd->output_file, flags, 0644);
            if (fd < 0) terminate_child("open output", cmd, EXIT_FAILURE);
            if (dup2(fd, STDOUT_FILENO) == -1) terminate_child("dup2 failed", cmd, EXIT_FAILURE);
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

        terminate_child(NULL, cmd, EXIT_FAILURE);
    }
      
    // parent process
    if (!cmd->background) {
        int status;
        waitpid(pid, &status, 0);
        return true;
    } else {
        printf("[%d] Started: %s (PID: %d)\n", getpid(), cmd->command, pid);
        process_table[process_count++] = (Process){.pid = pid, .cmd_ptr = cmd, .is_active = 1};
    }

    return false;
}

static void terminate_child(const char *error_message, Command *cmd, int exit_status){
    if (error_message) perror(error_message);
    free_command(cmd);
    _exit(exit_status);
}