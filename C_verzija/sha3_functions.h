#ifndef SHA3_FUNCTIONS_H
#define SHA3_FUNCTIONS_H

#include <stdint.h>

#define MATRIX_DIM 5
#define KECCAKF_ROUNDS 24 // 12 + 2*l; l=3

void keccak_f(char* A[MATRIX_DIM][MATRIX_DIM]);
int32_t rotate_right(int32_t a, const int32_t rotc);

#endif