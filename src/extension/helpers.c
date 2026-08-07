#include "helpers.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_INSTR_BIT_POSITION 31
#define MAX_INT_32_BIT_POSITION 31
#define FIELD_SIZE_32_BITS 32
#define LEAST_POS 0

#define MIN_REG_NO 0
#define MAX_REG_NO 31

#define BASE_10 10
#define BASE_16 16

#define MAX_TOK_LEN 15 // 15 characters
#define INSTRUCTION_TOK_DELIMS                                                 \
    " ,\n" // Delimit by spaces and commas and newlines

#define MAX_UINT_VALUE ((uint32_t)0xffffffff)
#define MIN_UINT_VALUE 0

#define MAX_INT_VALUE ((int32_t)0x7fffffff)
#define MIN_INT_VALUE ((int32_t)0x80000000)

#define LABEL_SEP "\n"
#define LABEL_PREFIXES ""
#define WORD_SIZE 4
#define MAX_SIMM19_OFFSET                                                      \
    (int32_t)((1 << 20) - WORD_SIZE)            // 1MiB - sizeof(word)
#define MIN_SIMM19_OFFSET (int32_t)(-(1 << 20)) // -2^20, or -1MiB
#define SIMM19_SCALAR 4
#define REGEXEC_SUCCESS_CODE 0

// Returns the number of bits covered by the range from bit positions 'from' to
// 'to' inclusive NOTE: This function asserts that from <= to
static unsigned int num_bits(const unsigned int from, const unsigned int to) {
    if (from > to) {
        fprintf(stderr,
                "Error: Passed bit range starts above from %d and ends below "
                "at %d.\n",
                from, to);
        exit(EXIT_FAILURE);
    }
    // E.g. from position 3 to 7 (inclusive) there are 7 - 3 + 1 = 5 bits in the
    // field
    return (to - from) + 1;
}

// Returns true iff the bit at pos in the instruction is set to 1
bool is_bit_set(const uint32_t instr, const unsigned int pos) {
    if (pos > MAX_INSTR_BIT_POSITION) {
        fprintf(stderr,
                "Error: Cannot test a 32 bit integer at bit position %d.\n",
                pos);
        exit(EXIT_FAILURE);
    }

    const uint32_t lsb_mask = 1; // Mask for the least significant bit
    return (instr >> pos) & lsb_mask;
}

bool sign_bit_32(const uint32_t value) { return (((int32_t)value) < 0); }
bool sign_bit_64(const uint64_t value) { return (((int64_t)value) < 0); }

static uint32_t get_contig_mask(const unsigned int num_bits,
                                const unsigned int start_pos) {
    // Caller cannot get a mask with bits outside the instruction's 32 bit
    // bounds
    if (start_pos + num_bits > FIELD_SIZE_32_BITS) {
        fprintf(stderr,
                "Error: Contig mask of %d bits starting from position %d "
                "exceeds 32 bit mask range.\n",
                num_bits, start_pos);
        exit(EXIT_FAILURE);
    }

    // 1 << num_bits gives 2^num_bits
    // Subtracting one gives a bit mask extracting only the lowest num_bits bits
    // Using unsigned long long 1 so that a 32 bit mask does not get shited to
    // become 0
    return (uint32_t)(((1ULL << num_bits) - 1) << start_pos);
}
// ============ bit setters ============

// Sets the bit 'pos' in the instruction to 1
// Note that it is the job of the caller to pass in a valid pos
void set_bit(uint32_t *instr, const unsigned int pos) { *instr |= (1U << pos); }

void unset_bit(uint32_t *instr, const unsigned int pos) {
    *instr &= ~(1U << pos);
}

// ORRs the passed value into the instruction between bit positions from and to
// inclusive
// Note that only the least [from..to] bits of value are ORRed into
// the instruction
void set_bits(uint32_t *instr, const uint32_t value, const unsigned int from,
              const unsigned int to) {
    const uint32_t value_bits =
        value & get_contig_mask(num_bits(from, to), LEAST_POS);
    *instr |= (value_bits << from);
}

int count_one_bits(unsigned int n) {
    int count = 0;
    while (n != 0) {
        n &= n-1;
        count++;
    }
    return count;
}
