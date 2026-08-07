#include "cell.h"
#include "helpers.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_9SET_SIZE 9
// Lowest 9 bits are set (indicating all 9 digits are present in the set)
#define FULL_9SET_DIGIT_FLAGS 0x1ff

static inline unsigned int get_num_bit_pos(const unsigned int number) {
    return number - 1;
}

static inline bool is_sudoku_digit(const unsigned int digit) {
    return (digit <= MAX_SUDOKU_DIGIT && digit >= MIN_SUDOKU_DIGIT);
}

// Returns an empty set  
nine_set set_create(void) {
    return (nine_set){.digit_flags = 0,
                      .size = 0};
}

// Returns a set containing all sudoku digits
nine_set set_create_full(void) {
    return (nine_set){.digit_flags = FULL_9SET_DIGIT_FLAGS,
                      .size = MAX_9SET_SIZE};
}

// Returns true iff the digit is present in the set
bool set_lookup(const nine_set set, const unsigned int digit) {
    if (!is_sudoku_digit(digit)) {
        return false;
    }

    return is_bit_set(set.digit_flags, get_num_bit_pos(digit));
}

// Returns true iff a digit was actually inserted into the set
bool set_insert(nine_set *set, const unsigned int digit) {
    if (is_sudoku_digit(digit) && !set_lookup(*set, digit)) {
        set_bit(&set->digit_flags, get_num_bit_pos(digit));
        set->size++;
        return true;
    }
    return false;
}

// Return true iff a digit was actually removed from the set
bool set_remove(nine_set *set, const unsigned int digit) {
    if (is_sudoku_digit(digit) && set_lookup(*set, digit)) {
        unset_bit(&set->digit_flags, get_num_bit_pos(digit));
        set->size--;
        return true;
    }
    return false;
}

// Return true iff the set contains only one digit
bool set_is_singleton(const nine_set set) { return set.size == 1; }

// Returns the singleton value in the set, or 0 if erroneous.
// Pre: the set only contains one value (set.size == 1)
unsigned int set_get_singleton(const nine_set set) {
    const unsigned int digit_flags = (unsigned int)set.digit_flags;

    if (set.size != 1) { // If set is not a singleton set
        fprintf(stderr,
                "Nine_set error: Cannot get a singleton from a set with size "
                "'%u'.\n",
                set.size);
        return 0;

    } else if (digit_flags == 0) { // should never happen if size != 0
        fprintf(stderr, "Nine_set error: Set size is 1, but all digit "
                        "positions are unset.\n");
        return 0;
    }

    // __builtin_ctz requires digit_flags != 0 (hence the assertion)
    const int trailing_zeroes = __builtin_ctz(digit_flags);
    // E.g. If the lowest 3 bits (digits) are unset, then 4 is the singleton
    // digit
    return trailing_zeroes + 1;
}
digit_t set_pop(nine_set *set, digit_t row, digit_t col) {
    bool bit_set;
    // WARNING: magic number
    for (int i = 0; i < 9; i++) {
        bit_set = is_bit_set((uint32_t)set->digit_flags, i);
        if (bit_set) {
            set_remove(set, i + 1);
            return i + 1;
        }
    }
    fprintf(stderr,
            "There should have been a bit set in set in row: %u and col: %u",
            row, col);
    exit(EXIT_FAILURE);
}

bool set_is_empty(nine_set *set) { return set->size == 0; }

nine_set set_union(nine_set s1, nine_set s2) {
    nine_set union_set = set_create();
    union_set.digit_flags = s1.digit_flags | s2.digit_flags;
    union_set.size = count_one_bits(union_set.digit_flags);
    return union_set;
}

nine_set n_set_union(nine_set **s, int n) {
    nine_set union_set = set_create();
    for (int i = 0; i<n; i++) { //Find union of all the sets
        union_set.digit_flags |= s[i]->digit_flags;
    }
    union_set.size = count_one_bits(union_set.digit_flags); //Acquire size through counting on bits
    return union_set;
}

nine_set set_difference(nine_set s1, nine_set s2) {
    nine_set union_set = set_create();
    union_set.digit_flags = s1.digit_flags & ~s2.digit_flags;
    union_set.size = count_one_bits(union_set.digit_flags);
    return union_set;
}