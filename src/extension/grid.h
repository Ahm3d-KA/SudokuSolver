#pragma once
#include <stdint.h>
#define GRID_DIMENSION 9
#define BOX_DIMENSION 3
#include "cell.h"
#include "worklist.h"
#define MAX_PRINT_LENGTH 300
extern cell grid_array[GRID_DIMENSION][GRID_DIMENSION];

typedef uint8_t digit_t;
typedef digit_t grid_s[GRID_DIMENSION][GRID_DIMENSION];
typedef grid_s *grid_t;

typedef struct coordinate {
    uint8_t row;
    uint8_t column;
} coordinate;

typedef enum { ROW, COL, BOX } house_type;

coordinate number_to_coordinate(uint8_t grid_number);
uint8_t coordinate_to_number(uint8_t row, uint8_t column);
coordinate get_box_start(uint8_t row, uint8_t col);
worklist get_sees(uint8_t row, uint8_t col);

// Function populates the initial grid, filling each square with a set of 1-9
int populate_grid();

// Writes digit in with pen
int place_digit(uint8_t row, uint8_t col, uint8_t digit, worklist *work);
bool is_pencil(int row, int col);

bool is_solved();

int grid_to_string(char buffer[MAX_PRINT_LENGTH]);

coordinate find_empty_cell();
