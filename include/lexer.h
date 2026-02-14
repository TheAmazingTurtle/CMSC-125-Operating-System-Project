#ifndef LEXER_H
#define LEXER_H

char **tokenize(char *line);
void free_tokens(char **token_arr);

#endif