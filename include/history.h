#ifndef HISTORY_H
#define HISTORY_H

#define MAX_HISTORY_ENTRY 1024

typedef struct {
    int id;
    char* line;
} HistoryEntry;

void add_history_entry(char *line);
void clear_history();
void print_history();

#endif