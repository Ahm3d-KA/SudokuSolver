#include "grid.h"
#include "worklist.h"
#include <stdint.h>

typedef enum { SIMPLE, MRV_HEURISTIC } backtracking_type;
bool solve_backtracking(worklist *wl, backtracking_type type,
                        bool with_algorithms, uint64_t *guess_count);
coordinate cell_mrv();
