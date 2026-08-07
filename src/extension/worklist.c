#include "worklist.h"
#include <stdio.h>
#include <stdlib.h>
#define ERRONEOUS_INDEX 255

static worklist worklist_create_node(void) {
    worklist node = malloc(sizeof(worklist_node));
    if (node == NULL) {
        fprintf(stderr,
                "Worklist error: Couldn't create a new worklist node.\n");
        exit(EXIT_FAILURE);

    } else {
        // Initialise with invalid head value (caller should write head value);
        node->head = ERRONEOUS_INDEX;
        node->tail = NULL;
        return node;
    }
}

worklist worklist_create_singleton(const unsigned int index) {
    worklist node = worklist_create_node();
    node->head = index;
    return node;
}

// Return true iff the digit was successfully appended to the head of the
// worklist. Pre: digit is a valid sudoku digit)
worklist worklist_append(worklist list, const unsigned int index) {
    worklist new_head_node = worklist_create_singleton(index);
    new_head_node->tail = list;
    return new_head_node;
}

unsigned int worklist_pop(worklist *list) {
    if (worklist_is_empty(*list)) {
        fprintf(stderr,
                "Worklist error: Tried to pop from an empty worklist.\n");
        return ERRONEOUS_INDEX;
    } else {
        const unsigned int return_value = (*list)->head;

        // Detach list head from tail
        worklist popped_node = *list;
        *list = (*list)->tail;

        // Free detached head and return old head value
        popped_node->tail = NULL;
        worklist_free(popped_node);
        return return_value;
    }
}

bool worklist_is_empty(worklist list) { return list == NULL; }

worklist worklist_create_empty(void) { return NULL; }

// Frees all memory allocated for the worklist
void worklist_free(worklist list) {
    while (list != NULL) {
        worklist next_node = list->tail;
        free(list);
        list = next_node;
    }
}