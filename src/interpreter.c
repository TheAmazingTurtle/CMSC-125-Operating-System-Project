#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>

#include "background_job.h"
#include "interpreter.h"

static BackgroundJob background_jobs[MAX_JOBS];
static int job_count = 0;

static bool execute_builtin_command(Command *cmd);
static bool launch_external_command(Command *cmd);
static void terminate_child(Command *cmd, char *error_msg, int exit_code);

void handle_command(Command **cmd) {
    if (execute_builtin_command(*cmd)) return;

    Command *to_exec = detach_command(cmd);
    if (!launch_external_command(to_exec)){
        free_command(&to_exec);
    }
}


void cleanup_zombies() {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {                     // ensures all finished jobs in this interation is exorcised
        printf("\n[Background job %d completed]\n", pid);
        
        for (int i = 0; i < job_count; i++) {                               // search-and-replace, faster implementation than shifting
            if (background_jobs[i].pid == pid) {
                free_command(&(background_jobs[i].cmd));
                background_jobs[i] = background_jobs[--job_count];
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

    return false;
}

static bool launch_external_command(Command *cmd) {
    if (job_count >= MAX_JOBS) {
        printf("mysh: Maximum background job limit reached\n");
        return false;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return false;
    }

    if (pid == 0) {     // child process
        if (cmd->input_file) {
            int fd = open(cmd->input_file, O_RDONLY);
            if (fd < 0) terminate_child(cmd, "open input file", EXIT_FAILURE);
            if (dup2(fd, STDIN_FILENO) == -1) terminate_child(cmd, "dup2 failed", EXIT_FAILURE);
            close(fd);
        }
        if (cmd->output_file) {
            int flags = O_WRONLY | O_CREAT;
            flags |= cmd->append ? O_APPEND : O_TRUNC;
            int fd = open(cmd->output_file, flags, 0644);
            if (fd < 0) terminate_child(cmd, "open output file", EXIT_FAILURE);
            if (dup2(fd, STDOUT_FILENO) == -1) terminate_child(cmd, "dup2 failed", EXIT_FAILURE);
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

        terminate_child(cmd, NULL, 127);
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
        free_command(&cmd);
    } else {
        printf("[%d] Started: %s (PID: %d)\n", getpid(), cmd->command, pid);
        background_jobs[job_count++] = (BackgroundJob){.pid = pid, .cmd = cmd};
    }
    return true;
}

static void terminate_child(Command *cmd, char *error_msg, int exit_code){
    if (cmd) free_command(&cmd);
    if (error_msg) perror(error_msg);
    _exit(exit_code);
}