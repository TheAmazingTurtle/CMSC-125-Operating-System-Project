#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "parser.h"

Command *parse(char **token_arr) {
    Command *cmd = calloc(1, sizeof *cmd);

    cmd->command = token_arr[0];
    cmd->args[0] = token_arr[0];

    int i = 1;
    while (token_arr[i]){
        char *cur_token = token_arr[i];
        char *next_token = token_arr[i+1];

        if (strcmp(cur_token, ">") == 0 || strcmp(cur_token, ">>") == 0) {
            if (cmd->output_file || !next_token) {
                printf("syntax error: invalid output redirection\n");
                return NULL;
            }

            cmd->append = strcmp(cur_token, ">>") == 0;
            cmd->output_file = next_token;
            i += 2;
            continue;
        }
        
        if (strcmp(cur_token, "<") == 0) {
            if (cmd->input_file || !next_token) {
                printf("syntax error: invalid input redirection\n");
                return NULL;
            }
            cmd->input_file = next_token;
            i += 2;
            continue;
        }
        
        if (strcmp(cur_token, "&") == 0) {
            if (next_token) {
                printf("syntax error: '&' must appear at end of command\n");
                return NULL;
            }
            cmd->background = true;
            i++;
            continue;
        } 

        cmd->args[i++] = cur_token;
    }

    free(token_arr);                                    // prevent memory leak

    return cmd;
}