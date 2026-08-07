#pragma once
#include <stdbool.h>

// Forward declaring worklist node typedef so recursive adt compiles
typedef struct worklist_node_adt worklist_node;

struct worklist_node_adt {
    unsigned int head;
    worklist_node *tail;
};

typedef worklist_node *worklist;

worklist worklist_append(worklist list, const unsigned int index);

unsigned int worklist_pop(worklist *list);

bool worklist_is_empty(worklist list);

worklist worklist_create_empty(void);

worklist worklist_create_singleton(const unsigned int index);

void worklist_free(worklist list);