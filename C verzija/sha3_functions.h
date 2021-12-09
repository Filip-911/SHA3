#ifndef SHA3_FUNCTIONS_H
#define SHA3_FUNCTIONS_H

#include <stdint.h>

#define MATRIX_DIM 5
#define KECCAKF_ROUNDS 18 // 12 + 2*l; l=3

void keccak_f(char* A[MATRIX_DIM][MATRIX_DIM]);
char rotate_right(char a, const char rotc);

#endif