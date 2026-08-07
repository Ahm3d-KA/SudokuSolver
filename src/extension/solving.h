#include "worklist.h"
#include <stdlib.h>

#include <stdint.h>

bool is_solved(void);
int naked_singles(worklist *work, uint64_t *guess_count);
bool hidden_singles(worklist *work, uint64_t *guess_count);
bool naked_subsets(void);
bool run_algorithms(worklist *wl, uint64_t *guess_count);
