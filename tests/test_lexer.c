#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "lexer.h"

typedef struct {
    char* input;
    char* expected_tokens[10];
    int expected_count;
} LexerTestCase;

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

bool is_actual_tokens_correct(int expected_count, char** expected_tokens, char** actual_tokens) {
    int actual_count = 0;
    while (actual_tokens[actual_count] != NULL) actual_count++;

    if (actual_count != expected_count) {
        return false;
    } 

    for (int j = 0; j < actual_count; j++) {
        if (strcmp(actual_tokens[j], expected_tokens[j]) != 0) {
            return false;
        }
    }

    return true;
}

int main() {
    LexerTestCase cases[] = {
        {"pwd", {"pwd", NULL}, 1},                                                  // simple command
        {"ls -l /home", {"ls", "-l", "/home", NULL}, 3},                            // command w/ argument
        {"  echo   hello  ", {"echo", "hello", NULL}, 2},                           // extra spaces
        {"", {NULL}, 0}                                                             // empty input
    };

    int num_cases = sizeof(cases) / sizeof(cases[0]);
    
    for (int i = 0; i < num_cases; i++) {
        printf("\nLexer Test #%d: ", i+1);
        
        char* writable_input = strdup(cases[i].input);
        char** actual_tokens = tokenize(writable_input);

        bool actual_tokens_matched = is_actual_tokens_correct(cases[i].expected_count, cases[i].expected_tokens, actual_tokens);
        printf("%s\n", actual_tokens_matched ? "PASS" : "FAIL");

        printf("    Input: \"%s\"\n", cases[i].input);

        printf("    Actual Output: ");
        print_tokens(actual_tokens);

        printf("    Expected Output: ");
        print_tokens(cases[i].expected_tokens);

        free(writable_input);
        free_tokens(actual_tokens);

        assert(actual_tokens_matched);
    }

    printf("\nALL %d LEXER TESTS PASSED!\n", num_cases);
    return 0;
}

