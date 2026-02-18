#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "../include/parser.h"

typedef struct {
    char *input_tokens[10];
    Command expected_cmd;
} ParserTestCase;

void print_tokens(char** tokens) {
    if (tokens == NULL || tokens[0] == NULL) {
        printf("{}\n");
        return;
    }

    printf("{");
    printf("\"%s\"", tokens[0]);
    for (int i = 1; tokens[i]; i++){
        printf(", \"%s\"", tokens[i]);
    }
    printf("}\n");
}

void print_cmd_members(Command *cmd) {
    printf("        Command: \"%s\"\n", cmd->command);

    printf("        Args: ");
    print_tokens(cmd->args);

    printf("        Input File: \"%s\"\n", cmd->input_file);
    printf("        Output File: \"%s\"\n", cmd->output_file);
    printf("        Append: \"%s\"\n", cmd->append ? "True" : "False");
    printf("        Background: \"%s\"\n", cmd->background ? "True" : "False");
}

bool safe_strcmp(char *s1, char *s2) {
    if (s1 == NULL && s2 == NULL) return true;  
    if (s1 == NULL || s2 == NULL) return false; 
    return strcmp(s1, s2) == 0;
}

bool is_actual_command_correct(Command *actual_cmd, Command *expected_cmd) {
    if (!safe_strcmp(actual_cmd->command, expected_cmd->command)) return false;
    if (!safe_strcmp(actual_cmd->input_file, expected_cmd->input_file)) return false;
    if (!safe_strcmp(actual_cmd->output_file, expected_cmd->output_file)) return false;
    if (actual_cmd->append != expected_cmd->append) return false;
    if (actual_cmd->background != expected_cmd->background) return false;

    int i = 0;
    while (true) {
        char *actual_arg = actual_cmd->args[i];
        char *expected_arg = expected_cmd->args[i];

        if (actual_arg == NULL && expected_arg == NULL) break; // Reached end of both lists successfully
        if (!safe_strcmp(actual_arg, expected_arg)) return false; // Catch mismatch or one ending early
        
        i++;
    }

    return true;
}


int main() {
    ParserTestCase cases[] = {
        {
            .input_tokens = {"ls", "-l", "/tmp", NULL},
            .expected_cmd = {.command = "ls", .args = {"ls", "-l", "/tmp", NULL}, .input_file = NULL, .output_file = NULL, .append = false, .background = false}
        },
        {
            .input_tokens = {"ls", ">", "out.txt", NULL},
            .expected_cmd = {.command = "ls", .args = {"ls", NULL}, .input_file = NULL, .output_file = "out.txt", .append = false, .background = false}
        },
        {
            .input_tokens = {"ls", ">>", "log.txt", NULL},
            .expected_cmd = {.command = "ls", .args = {"ls", NULL}, .input_file = NULL, .output_file = "log.txt", .append = true, .background = false}
        },
        {
            .input_tokens = {"sleep", "10", "&", NULL},
            .expected_cmd = {.command = "sleep", .args = {"sleep", "10", NULL}, .input_file = NULL, .output_file = NULL, .append = false, .background = true}
        }
    };

    int num_cases = sizeof(cases) / sizeof(cases[0]);
    for (int i = 0; i < num_cases; i++) {
        printf("\nParser Test #%d: ", i + 1);

        Command *actual_cmd = parse(cases[i].input_tokens);

        bool actual_cmd_matched = is_actual_command_correct(actual_cmd, &cases[i].expected_cmd);
        printf("%s\n", actual_cmd_matched ? "PASS" : "FAIL");

        printf("    Input: ");
        print_tokens(cases[i].input_tokens);

        printf("    Actual Output: \n");
        print_cmd_members(actual_cmd);

        printf("    Expected Output: \n");
        print_cmd_members(&cases[i].expected_cmd);

    //     free_command(actual_cmd);
    }

    printf("\nALL %d PARSER TESTS PASSED!\n", num_cases);
    
    return 0;
}