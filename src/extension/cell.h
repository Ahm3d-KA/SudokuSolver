#pragma once
#include <stdbool.h>
#include <stdint.h>

#define NUM_SUDOKU_DIGITS 9
#define MAX_SUDOKU_DIGIT 9
#define MIN_SUDOKU_DIGIT 1

typedef uint8_t digit_t;
// NOTE: Invalid sudoku digits are never inserted, deleted, or looked up in the
// set (asserted by the ADT)

typedef struct {
    uint32_t digit_flags;
    unsigned int size;
} nine_set;

typedef union {
    nine_set set;
    unsigned int digit;
} cell_value;

typedef enum { WRITTEN, PENCILED } cell_type;

typedef struct {
    cell_type type;
    cell_value value;
} cell;

nine_set set_create_full(void);

bool set_lookup(const nine_set set, const unsigned int digit);

bool set_insert(nine_set *set, const unsigned int digit);

bool set_remove(nine_set *set, const unsigned int digit);

bool set_is_singleton(const nine_set set);

// Returns 0 if call was erroneous
unsigned int set_get_singleton(const nine_set set);

nine_set set_create(void);

// digit_t set_pop(nine_set *set);
digit_t set_pop(nine_set *set, digit_t row, digit_t col);

bool set_is_empty(nine_set *set);

nine_set set_union(nine_set s1, nine_set s2);

nine_set n_set_union(nine_set **s, int n);

nine_set set_difference(nine_set s1, nine_set s2);
