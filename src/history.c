#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "history.h"

static HistoryEntry *history[MAX_HISTORY_ENTRY];
static int entry_count = 0;
static int next_index = 0;
static int cur_id = 1;

static void free_entry(HistoryEntry *history);

void add_history_entry(char *line) {
    if (history[next_index] != NULL) {
        free_entry(history[next_index]);
    } else {
        entry_count++;
    }

    HistoryEntry *new_entry = malloc(sizeof(HistoryEntry));
    new_entry->id = cur_id++;
    new_entry->line = strdup(line);

    history[next_index] = new_entry;
    next_index = (next_index + 1) % MAX_HISTORY_ENTRY;
}

void clear_history() {
    for (int i = 0; i < MAX_HISTORY_ENTRY; i++) {
        free_entry(history[i]);
        history[i] = NULL;
    }

    entry_count = 0;
    next_index = 0;
    cur_id = 1;
}

void print_history() {
    int i = entry_count < MAX_HISTORY_ENTRY ? 0 : next_index;

    for (int step = 0; step < entry_count; step++) {
        printf("%d %s\n", history[i]->id, history[i]->line);
        i = (i + 1) % MAX_HISTORY_ENTRY;
    }
}

void free_entry(HistoryEntry *entry) {
    if (!entry) return;

    free(entry->line);
    free(entry);
}