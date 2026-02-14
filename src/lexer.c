#include <stdlib.h>
#include <string.h>

#include "lexer.h"

char **tokenize(char *line) {
    char **token_arr  = malloc(sizeof(char *) * 256);
    if (!token_arr) return NULL;                        // check if memory is allocated

    char *token;

    line[strcspn(line, "\n")] = 0;                      // to strip \n from line

    token = strtok(line, " ");
    if (token == NULL) {
        free(token_arr);
        return NULL;
    }

    int i = 0;
    while (token != NULL && i < 255) {                  // limit to 255, leave space for NULL
        token_arr[i++] = strdup(token);
        token = strtok(NULL, " ");
    }
    token_arr[i] = NULL;

    return token_arr;
}

void free_tokens(char **token_arr){
    if (!token_arr) return;

    for (int i = 0; token_arr[i] != NULL; i++) {
        free(token_arr[i]);
    }

    free(token_arr);
}