#include "solving.h"
#include "cell.h"
#include "grid.h"
#include <stdint.h>
#include <stdio.h>

bool is_solved() {
    for (int j = 0; j < GRID_DIMENSION; j++) {
        for (int i = 0; i < GRID_DIMENSION; i++) {
            if (grid_array[i][j].type == PENCILED) {
                return false;
            }
        }
    }
    return true;
}

int naked_singles(worklist *work, uint64_t *guess_count) {
    // worklist_print(*work);
    if (worklist_is_empty(*work)) { // When list is empty, either algorithm is
                                    // finished or got stuck
        if (is_solved()) {
            printf("\n Puzzle solved\n");
        } else {
            /*printf("\n No more naked singles, puzzle not solved\n");
            // WARNING: magic number
            char buffer[300];
            grid_to_string(buffer);
            printf("%s\n", buffer);*/
        }
        return EXIT_SUCCESS;
    }

    int current_index =
        worklist_pop(work); // Get current grid index from worklist
    coordinate current_coord = number_to_coordinate(current_index);
    cell current_cell = grid_array[current_coord.row][current_coord.column];

    if (PENCILED == current_cell.type &&
        set_is_singleton(
            current_cell.value.set)) { // If this is a pencil, and there is only
                                       // one possible value
        (*guess_count)++;
        place_digit(current_coord.row, current_coord.column,
                    set_get_singleton(current_cell.value.set),
                    work); // Fill in this possible value
    }
    return naked_singles(
        work, guess_count); // Now solve next index in list (as work was popped.
}

int get_hidden_single(int row, int col, house_type type, digit_t digit) {
    uint8_t digit_count = 0; // Number of times digit is in house
    nine_set set;            // Candidate set for each cell in row
    int grid_index = -1;     // Grid index of hidden single
    switch (type) {
    case ROW: {
        for (int i = 0; i < GRID_DIMENSION; i++) {
            set = grid_array[row][i].value.set; // Get set for all cells in row
            if (is_pencil(row, i) &&
                set_lookup(set,
                           digit)) { // If set is pencil and digit is in set
                digit_count++;       // Digit count increases
                grid_index = coordinate_to_number(
                    row, i); // Convert coordinate to grid index
            }
        }
        break;
    }
    case COL: {
        for (int i = 0; i < GRID_DIMENSION; i++) {
            set =
                grid_array[i][col].value.set; // Get set for all cells in column
            if (is_pencil(i, col) &&
                set_lookup(set,
                           digit)) { // If set is pencil and digit is in set
                digit_count++;       // Digit count increases
                grid_index = coordinate_to_number(
                    i, col); // Convert coordinate to grid index
            }
        }
        break;
    }
    case BOX: {
        coordinate box_start = get_box_start(row, col);
        for (int i = 0; i < BOX_DIMENSION; i++) {
            for (int j = 0; j < BOX_DIMENSION; j++) {
                set = grid_array[box_start.row + i][box_start.column + j]
                          .value.set; // Get set for all cells in box
                if (is_pencil(box_start.row + i, box_start.column + j) &&
                    set_lookup(set,
                               digit)) { // If set is pencil and digit is in set
                    digit_count++;       // Digit count increases
                    grid_index = coordinate_to_number(
                        box_start.row + i,
                        box_start.column +
                            j); // Convert coordinate to grid index
                }
            }
        }
        break;
    }
    default:
        fprintf(
            stderr,
            "Invalid house type"); // House can only be a row, a column or a box
        return -1;
    }

    if (digit_count == 1) { // Only valid if digit appeared onyl once in house
        return grid_index;
    } else {
        return -1; // If no naked single, return  -1
    };
}

// Bool returns true if this caused any digits to be placed
bool hidden_singles(worklist *work, uint64_t *guess_count) {
    bool changed = false;
    int index;
    coordinate coords;

    // Check the rows
    for (int row = 0; row < GRID_DIMENSION; row++) {
        for (int d = MIN_SUDOKU_DIGIT; d <= MAX_SUDOKU_DIGIT; d++) {
            if ((index = get_hidden_single(row, 0, ROW, d)) != -1) {
                coords = number_to_coordinate(index);
                (*guess_count)++;
                place_digit(coords.row, coords.column, d, work);
                changed = true;
            }
        }
    }

    // Check the columns
    for (int col = 0; col < GRID_DIMENSION; col++) {
        for (int d = MIN_SUDOKU_DIGIT; d <= MAX_SUDOKU_DIGIT; d++) {
            if ((index = get_hidden_single(0, col, COL, d)) != -1) {
                coords = number_to_coordinate(index);

                (*guess_count)++;
                place_digit(coords.row, coords.column, d, work);
                changed = true;
            }
        }
    }

    // Check the boxes
    for (int row = 0; row < GRID_DIMENSION; row += BOX_DIMENSION) {
        for (int col = 0; col < GRID_DIMENSION; col += BOX_DIMENSION) {
            for (int d = MIN_SUDOKU_DIGIT; d <= MAX_SUDOKU_DIGIT; d++) {
                if ((index = get_hidden_single(row, col, BOX, d)) != -1) {
                    coords = number_to_coordinate(index);

                    (*guess_count)++;
                    place_digit(coords.row, coords.column, d, work);
                    changed = true;
                }
            }
        }
    }

    return changed;
}

bool is_naked_subset(int *chosen, int subset_size, nine_set *out_union) {
    nine_set *subsets[subset_size];
    coordinate coords;
    for (int i = 0; i < subset_size; i++) {
        coords = number_to_coordinate(chosen[i]);
        if (!is_pencil(coords.row,
                       coords.column)) { // Non-pencils cannot be naked subsets
            return false;
        }
        subsets[i] =
            &grid_array[coords.row][coords.column]
                 .value.set; // Build array of sets for group or operation
    }

    *out_union = n_set_union(subsets, subset_size);
    return (out_union->size == (unsigned int)
            subset_size); // If union size is the same as the subset size, there
                          // is a naked subset
}

// Recurisve algorithm to identify and handle naked subsets
bool check_subsets(int *elements, int num_possible_elems, int subset_size,
                   int start, int depth, int *chosen) {
    if (num_possible_elems <
        subset_size) { // Subset can't exist with higher cardinality than the
                       // set of possible elemets
        return false;
    }
    bool changed = false;       // If algorithm has made ant changes
    if (depth == subset_size) { // Recursive base-case
        nine_set union_set;
        if (is_naked_subset(
                chosen, subset_size,
                &union_set)) { // If satisifed naked_subset condition
            for (int house_elem = 0; house_elem < num_possible_elems;
                 house_elem++) {
                bool is_chosen = false;
                coordinate coords = number_to_coordinate(elements[house_elem]);
                cell *c = &grid_array[coords.row][coords.column];

                // Checks if the house element is part of the subset
                for (int chosen_elem = 0; chosen_elem < subset_size;
                     chosen_elem++) {
                    if (chosen[chosen_elem] == elements[house_elem]) {
                        is_chosen = true;
                        break;
                    }
                }
                // If so, we do not need to remove anything from this element
                if (is_chosen) {
                    continue;
                }
                // If not a pencil, no removal needed
                if (!is_pencil(coords.row, coords.column)) {
                    continue;
                }
                nine_set old_set = c->value.set;
                c->value.set = set_difference(
                    c->value.set, union_set); // Removed union elements from
                                              // other sets in house
                if (c->value.set.size != old_set.size) {
                    changed =
                        true; // If size changed, there is a change to the grid
                }
            }
        }
        return changed;
    }

    for (int i = start; i <= num_possible_elems - (subset_size - depth); i++) {
        // Build subset in each call
        chosen[depth] = elements[i];

        // Recursive call
        if (check_subsets(elements, num_possible_elems, subset_size, i + 1,
                          depth + 1, chosen)) {
            changed = true;
        };
    }

    return changed;
}

// Returns true if algorithm caused a change to a candidate list
bool naked_subsets(void) {
    bool changed = false;

    for (int subset_size = MIN_SUDOKU_DIGIT + 1; subset_size < MAX_SUDOKU_DIGIT;
         subset_size++) {
        int chosen_cells[subset_size];
        int house_cells[GRID_DIMENSION];
        int house_size = 0;
        // Check the rows for naked subsets
        for (int row = 0; row < GRID_DIMENSION; row++) {
            house_size = 0;
            for (int col = 0; col < GRID_DIMENSION; col++) {
                if (is_pencil(row, col)) {
                    house_cells[house_size++] = coordinate_to_number(row, col);
                }
            }
            if (check_subsets(house_cells, house_size, subset_size, 0, 0,
                              chosen_cells)) {
                changed = true;
            }
        }

        // Check the columns for naked subsets
        for (int col = 0; col < GRID_DIMENSION; col++) {
            int house_size = 0;
            for (int row = 0; row < GRID_DIMENSION; row++) {
                if (is_pencil(row, col)) {
                    house_cells[house_size++] = coordinate_to_number(row, col);
                }
            }
            if (check_subsets(house_cells, house_size, subset_size, 0, 0,
                              chosen_cells)) {
                changed = true;
            }
        }

        // Check the boxes for naked subsets
        for (int row = 0; row < GRID_DIMENSION; row += BOX_DIMENSION) {
            for (int col = 0; col < GRID_DIMENSION; col += BOX_DIMENSION) {
                int house_size = 0;
                for (int i = 0; i < BOX_DIMENSION; i++) {
                    for (int j = 0; j < BOX_DIMENSION; j++) {
                        if (is_pencil(row + i, col + j)) {
                            house_cells[house_size++] =
                                coordinate_to_number(row + i, col + j);
                        }
                    }
                }
                if (check_subsets(house_cells, house_size, subset_size, 0, 0,
                                  chosen_cells)) {
                    changed = true;
                }
            }
        }
    }

    return changed;
}

bool run_algorithms(worklist *wl, uint64_t *guess_count) {
    bool changed;
    do {
        changed = false;
        if (naked_singles(wl, guess_count)) {
            changed = true;
        }

        if (hidden_singles(wl, guess_count)) {
            changed = true;
        }

        if (naked_subsets()) {
            changed = true;
        }

    } while (changed);

    return is_solved();
}