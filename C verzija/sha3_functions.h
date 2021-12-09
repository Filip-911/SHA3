#ifndef SHA3_FUNCTIONS_H
#define SHA3_FUNCTIONS_H
#include <stdint.h>

#define MATRIX_DIM 5
#define KECCAKF_ROUNDS 18 // 12 + 2*l; l=3

void keccak_f(char* A[MATRIX_DIM][MATRIX_DIM]);
char rotate_right(char a, const char rotc);

// constants
    const char keccakf_rndc[24] = {
        0x01, 0x82, 0xff,
        0x80, 0x8b, 0xf1,
        0xa1, 0x89, 0x8a,
        0xaf, 0x09, 0x0a,
        0xcb, 0x88, 0x8f,
        0x83, 0xf2, 0xa0,
        0x0a, 0x8a, 0xc1,
        0xf0, 0xc1, 0xb8
    };
    const char keccakf_rotc[25] = {
        1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14,
        27, 41, 56, 8,  25, 43, 62, 18, 39, 61, 20, 44
    };

#endif