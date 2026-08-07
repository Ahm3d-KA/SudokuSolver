#include <stdio.h>
#include <stdlib.h>
// #include <math.h>
#include "cell.h"
#include "grid.h"
#include <assert.h>

cell grid_array[GRID_DIMENSION][GRID_DIMENSION];

// Returns (row_num, column_num)
coordinate number_to_coordinate(uint8_t grid_number) {
    return (coordinate){.row = grid_number / GRID_DIMENSION,
                        .column = grid_number % GRID_DIMENSION};
}

// Returns grid index of coordinate
uint8_t coordinate_to_number(uint8_t row, uint8_t column) {
    return (GRID_DIMENSION * row) + column;
}

coordinate get_box_start(uint8_t row, uint8_t col) {
    return (coordinate){.row = (row / BOX_DIMENSION) * BOX_DIMENSION,
                        .column = (col / BOX_DIMENSION) * BOX_DIMENSION};
}

// Returns a set with all the grid indicies that the current grid index sees
worklist get_sees(uint8_t row, uint8_t col) {
    worklist sees_list = worklist_create_empty();
    uint8_t other_cell;
    // get elems in row
    for (int i = 0; i < GRID_DIMENSION; i++) {
        other_cell = coordinate_to_number(row, i);
        if (other_cell != coordinate_to_number(row, col)) {
            sees_list = worklist_append(sees_list, other_cell);
        }
    }

    // get elemns in column
    for (int i = 0; i < GRID_DIMENSION; i++) {
        other_cell = coordinate_to_number(i, col);
        if (other_cell != coordinate_to_number(row, col)) {
            sees_list = worklist_append(sees_list, other_cell);
        }
    }

    // get elems in box
    coordinate box_start = get_box_start(row, col);
    for (int i = 0; i < BOX_DIMENSION; i++) {
        for (int j = 0; j < BOX_DIMENSION; j++) {
            other_cell =
                coordinate_to_number(box_start.row + i, box_start.column + j);
            if (other_cell != coordinate_to_number(row, col)) {
                sees_list = worklist_append(sees_list, other_cell);
            }
        }
    }

    return sees_list;
}

// Function populates the initial grid
int populate_grid() {
    for (int i = 0; i < GRID_DIMENSION; i++) {
        for (int j = 0; j < GRID_DIMENSION;
             j++) {                           // Iterates through all elements
            grid_array[i][j].type = PENCILED; // Sets each cell to penciled as
                                              // it hasn't been filled yet
            grid_array[i][j].value.set =
                set_create_full(); // Creates a set for each cell
            for (int k = MIN_SUDOKU_DIGIT; k <= MAX_SUDOKU_DIGIT; k++) {
                set_insert(&grid_array[i][j].value.set,
                           k); // Adds digits 1-9 to the cell
            }
        }
    }
    return EXIT_SUCCESS;
}

int place_digit(uint8_t row, uint8_t col, uint8_t digit, worklist *work) {
    grid_array[row][col].type = WRITTEN;      // Sets this cell to be writtem
    grid_array[row][col].value.digit = digit; // Gives cell specified digit
    // int grid_index =
    //     coordinate_to_number(row, col); // Convert grid coordinate to grid
    //     index
    int see_index; // Holds grid index of a value in the same house as specified
                   // cell
    coordinate see_index_coord; // Holds coordiante of a value in the same house
                                // as specified cell
    worklist in_house =
        get_sees(row, col); // Holds list of all elements in same house as cell
    while (!worklist_is_empty(
        in_house)) { // Iteratively gets the next seen element
        see_index = worklist_pop(&in_house);
        see_index_coord = number_to_coordinate(see_index);
        cell *see_cell =
            &grid_array[see_index_coord.row][see_index_coord.column];
        if (see_cell->type ==
            PENCILED) { // If type is pencilled, the specified digit is removed
                        // from possible digits of seen cells
            if (set_remove(&(see_cell->value.set), digit)) {
                *work = worklist_append(
                    *work, see_index); // More efficient method to merge
                                       // in-house and worklist can be used
            }
        }
    }
    return EXIT_SUCCESS;
}

bool is_pencil(int row, int col) {
    return (grid_array[row][col].type == PENCILED);
}

int grid_to_string(char buffer[MAX_PRINT_LENGTH]) {
    // this updates every time the buffer is written to to point to th next free
    // space
    char *ptr = buffer;
    // this ensures that we don't overrun the buffer
    char *end = buffer + MAX_PRINT_LENGTH;

    for (int row = 0; row < GRID_DIMENSION; row++) {
        if (row == 3 || row == 6) {
            ptr += snprintf(ptr, end - ptr, "------+-------+------\n");
        }
        for (int col = 0; col < GRID_DIMENSION; col++) {
            if (col == 3 || col == 6) {
                ptr += snprintf(ptr, end - ptr, "| ");
            }
            switch (grid_array[row][col].type) {
            case PENCILED:
                ptr += snprintf(ptr, end - ptr, ". ");
                break;
            case WRITTEN:
                ptr += snprintf(ptr, end - ptr, "%u ",
                                grid_array[row][col].value.digit);
                break;
            }
        }
        ptr += snprintf(ptr, end - ptr, "\n");
    }
    return EXIT_SUCCESS;
}

coordinate find_empty_cell() {
    assert(!is_solved()); // should not reach Here
    for (int j = 0; j < GRID_DIMENSION; j++) {
        for (int i = 0; i < GRID_DIMENSION; i++) {
            if (grid_array[i][j].type == PENCILED) {
                coordinate coord = {.row = i, .column = j};
                return coord;
            }
        }
    }
    fprintf(stderr, "no empty cells found \n");
    exit(EXIT_FAILURE);
}
