#include "backtracking.h"
#include "cell.h"
#include "grid.h"
#include "solving.h"
#include "worklist.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// TODO: only returns an empty cell does not use mrv heuristic yet
coordinate cell_mrv() {
    coordinate best_cell_index = {.row = -1, .column = -1};
    int min_candidates = 10;
    for (int j = 0; j < GRID_DIMENSION; j++) {
        for (int i = 0; i < GRID_DIMENSION; i++) {
            if (grid_array[i][j].type == PENCILED) {
                if (grid_array[i][j].value.set.size < min_candidates) {
                    best_cell_index.row = i;
                    best_cell_index.column = j;
                    min_candidates = grid_array[i][j].value.set.size;
                }
            }
        }
    }
    return best_cell_index;
}
bool validate_cell(coordinate coords, digit_t digit) {

    worklist sees_list = get_sees(coords.row, coords.column);
    int i;
    coordinate compare_cord;
    while (!worklist_is_empty(sees_list)) {
        // gets the location then the value of the cell then compares it to
        // value
        compare_cord = number_to_coordinate(worklist_pop(&sees_list));
        i = grid_array[compare_cord.row][compare_cord.column].value.digit;
        if (digit == i) {
            return false;
        }
    }
    return true;
}

bool invalid_board_state() {
    for (int j = 0; j < GRID_DIMENSION; j++) {
        for (int i = 0; i < GRID_DIMENSION; i++) {
            if (grid_array[i][j].type == PENCILED &&
                set_is_empty(&grid_array[i][j].value.set)) {
                return true;
            }
        }
    }
    return false;
}

bool solve_backtracking(worklist *wl, backtracking_type type,
                        bool with_algorithms, uint64_t *guess_count) {

    if (with_algorithms) {
        if (run_algorithms(wl, guess_count)) {
            return true;
        }
    } else {

        // needs to check if it is solved so it doesn't just continue
        if (is_solved()) {
            return true;
        }
    }

    // we have reached a point where a pencil cell has an empty nine set so
    // invalid board state
    if (invalid_board_state()) {
        return false;
    }

    // need a backup grid if the back tracing fails we should restore to this
    // grid
    cell *backup_grid = malloc(sizeof(cell) * GRID_DIMENSION * GRID_DIMENSION);
    assert(backup_grid != NULL);
    memcpy(backup_grid, grid_array,
           sizeof(cell) * GRID_DIMENSION * GRID_DIMENSION);

    // using heuristics, finds the cell with the least posisble options
    // we know it can't be naked single as solve checks for this
    coordinate next_cell;
    // decides how we pick the next cell to look at
    switch (type) {
    case MRV_HEURISTIC:
        next_cell = cell_mrv();
        break;
    default:
        next_cell = find_empty_cell();
        break;
    }

    nine_set next_cell_set =
        grid_array[next_cell.row][next_cell.column].value.set;
    // digit_t candidate_digit = set_pop(&next_cell_set);
    digit_t candidate_digit =
        set_pop(&next_cell_set, next_cell.row, next_cell.column);
    // printf("picking %u to go in row %u col %u\n", candidate_digit,
    //        next_cell.row, next_cell.column);

    (*guess_count)++;
    place_digit(next_cell.row, next_cell.column, candidate_digit, wl);

    if (with_algorithms) {
        if (run_algorithms(wl, guess_count)) {
            return true;
        }
    } else {
        if (is_solved()) {
            return true;
        }
    }
    // this keeps looping until we get a valid solution, any time we get a wrong
    // one, we try the next candidate digit
    while (!solve_backtracking(wl, type, with_algorithms, guess_count)) {
        // restore the 2d array to the old versoin
        memcpy(grid_array, backup_grid,
               sizeof(cell) * GRID_DIMENSION * GRID_DIMENSION);

        if (set_is_empty(&next_cell_set)) {
            free(backup_grid);
            return false;
        }

        candidate_digit =
            set_pop(&next_cell_set, next_cell.row, next_cell.column);
        (*guess_count)++;
        place_digit(next_cell.row, next_cell.column, candidate_digit, wl);
        // printf("picking %u to go in row %u col %u\n", candidate_digit,
        //        next_cell.row, next_cell.column);
    }
    // printf("digit %u fits in row %u col %u\n", candidate_digit,
    // next_cell.row,
    //        next_cell.column);

    free(backup_grid);
    return true;
}
