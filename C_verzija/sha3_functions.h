#ifndef SHA3_FUNCTIONS_H
#define SHA3_FUNCTIONS_H

#include <stdint.h>

#define MATRIX_DIM 5
#define KECCAKF_ROUNDS 24 // 12 + 2*l; l=3

void keccak_f(char* A[MATRIX_DIM][MATRIX_DIM]);
uint64_t rotate_right(uint64_t a, const uint64_t rotc);

#endif