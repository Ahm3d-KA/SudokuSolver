#pragma once
#include <stdbool.h>
#include <stdint.h>

#define MAX_INSTR_BIT_POSITION 31

bool is_bit_set(const uint32_t instr, const unsigned int pos);

bool sign_bit_32(const uint32_t value);
bool sign_bit_64(const uint64_t value);

// ============ bit setters ============

// Writes the result of ORRing 1 to the instruction at position pos
// Note that it is the job of the caller to pass in a valid pos
void set_bit(uint32_t *instr, const unsigned int pos);

// Writes the result of unsetting a bit in an instruction.
// Pre: Pos is between [0..31]. Bit does not actually have to be set
void unset_bit(uint32_t *instr, const unsigned int pos);

// Writes the result of ORRing the value into the instruction between bit
// positions from and to inclusive.
// Note that only the least [from..to] bits of value are used
void set_bits(uint32_t *instr, const uint32_t value, const unsigned int from,
              const unsigned int to);

//Counts the number of 1 bits in a binary value
int count_one_bits(unsigned int n);
