#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sha3_functions.h"

#define RATE (64/8)      //  12
#define CAPACITY (136/8) // +13
#define SIZE (200/8)     // =25
#define OUTPUT_SIZE 11

#define MATRIX_DIM 5

/*
    SIZE = 25 * 2^l; 
    l = 3;
    keccak_f rounds = 12 + 2*l;
*/

int pad(char *input, int length)
{    
    int a;
    if(length + 1 == RATE)
    {
      input = (char*)realloc(input, length + 2);
      input[length - 2] = 0x81;
      input[length - 1] = '\0';
    }
    else 
    {
        a = length/RATE;
	a++;
	input = (char*)realloc(input, a*RATE + 1); 
        input[length] = 0x80;
        for(unsigned int i = length + 1; i < a*RATE - 2; i++)
        {
            input[i] = 0x00;
        }
    input[a*RATE - 2] = 0x01;
    input[a*RATE - 1] = '\0';
    }
    return a;
}

int main(int argc, char *argv[]){
  
    if(argc == 1) // if there are no arguments
    { 
        printf("ERROR: Expected at least 1 argument\n");
        return 0;
    }

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
        int temp;
        char B[5][5];
        char C[5];
        char D[5];
        char pi[1][1];
    /*
    TO DO 
    ABSORB
    pad the input N using the pad function, yielding a padded bit string P with a length divisible by r such that len(p)/r is an integer)
    break P into n consecutive r-bit pieces P0, ..., Pn−1
    initialize the state S to a string of b zero bits
    absorb the input into the state: for each block Pi:
    extend Pi at the end by a string of c zero bits, yielding one of length b
    XOR that with S
    apply the block permutation f to the result, yielding a new state S
    SQUEEZE
    initialize Z to be the empty string
    while the length of Z is less than d:
    append the first r bits of S to Z
    if Z is still less than d bits long, apply f to S, yielding a new state S
    truncate Z to d bits*/

  
    int inputLen = strlen(argv[1]);
    
    //concatenate all the args to the input string
    char *input = (char*)malloc(inputLen);
    strcpy(input, argv[1]);

    for(int i = 2; i < argc; i++)
      {
	inputLen += strlen(argv[i]) + 1; 
        input = (char*)realloc(input, inputLen);
        strcat(input, " ");
        strcat(input, argv[i]);
    }
    
    //pad the input using the pad function
    int blockCount = pad(input,inputLen);
  
    // Napravi niz matrica 5x5 i postavi prvih RATE clanova na one iz input stringa
    char blocks[blockCount][MATRIX_DIM][MATRIX_DIM];
    int p=0;
    for (int i = 0; i < blockCount; i++)
    {
        for (int j = 0; j < MATRIX_DIM; j++)
        {
            for (int k = 0; k < MATRIX_DIM; k++)
            {
                if(p < RATE)
                {
                    blocks[i][j][k] = input[i*RATE + p];
                    p++; //broji koliko clanova je unijeto
                }
                else
                {   
                    blocks[i][j][k] = 0x00; //ostali bajtovi idu na 0
                }
            }
        }
        p=0;
    }
    free(input);

    // initialize the state S to a inputing of b zero bits
    char state[MATRIX_DIM][MATRIX_DIM];
    for(int i = 0; i < MATRIX_DIM; i++)
        for (int k = 0; k < MATRIX_DIM; k++)
            state[i][k] = 0x00;

    /*absorb the input into the state: 
    for each block Pi:
    extend Pi at the end by a inputing of CAPACITY (17) zero bits, yielding one of length SIZE*/
    //DONE

    //XOR that with state
    //apply the block permutation f to the result, yielding a new state S and do it until you run out of blocks to xor with
    for (int i = 0; i < blockCount; i++)
    {
        for (int j = 0; j < MATRIX_DIM; j++)
        {
            for (int k = 0; k < MATRIX_DIM; k++)
            {
                state[j][k] ^= blocks[i][j][k];
            }
        }
        for(int n = 0; n < KECCAKF_ROUNDS; n++)
        {

            //theta
            for (int x = 0; x < 5; x++)
            {
                C[x] = state[x][0];
                for (int y = 1; y < 5; y++)
                    C[x] ^= state[x][y];
            }

            for (int x = 0; x < 5; x++)
            {
                D[x] = C[x-1] ^ rotate_right(C[(x + 1) % 5], 1);
                for (int y = 0; y < 5; y++)
                {
                    state[x][y] ^= D[x];
                }
            }

            //chi
            for (int x = 0; x < 5; x++)
            {
                for (int y = 0; y < 5; y++)
                    state[x][y] = state[x][y] ^ (~state[(x + 1) % 5][y] & state[(x + 2) % 5][y]);
            }

            //rho
            int x = 1;
            int y = 0;
            for(int t = 0; t < 24; t++)
            {
                state[x][y] = rotate_right(state[x][y], keccakf_rotc[t]);

                temp = x;
                x = y;
                y = (2*temp + 3*y) % 5;
            }
            //merge ?
            //pi
            for(int x = 0; x < 5; x++)
            {
                for(int y = 0; y < 5; y++)
                {
                    pi[1][1] = state[x][y];
                    temp = x;
                    x = y;
                    y = (2*temp + 3*y) % 5;
                    B[x][y] = pi[1][1];
                }
            state[x][y] = B[x][y];
            }

            //iota
            //arbitrary lane in the state is xored with the constants
               state[x][y] ^= keccakf_rndc[n];
        }

    }

    //initialize Z to be the empty string
    char z[OUTPUT_SIZE + 1];
    for (size_t i = 0; i < OUTPUT_SIZE; i++)
    {
        z[i] = 0x00;
    }
    z[OUTPUT_SIZE] = '\0';
    
    //while the length of Z is less than OUTPUT_SIZE:
    //append the first RATE bits of STATE to Z
    p=0;
    for (size_t i = 0; i < MATRIX_DIM; i++)
    {
        for (size_t j = 0; j < MATRIX_DIM; j++)
        {
            z[p++] = state[i][j];
            if(p == OUTPUT_SIZE - 1)
                break;
        }
        if(p == OUTPUT_SIZE - 1)
            break;
    }
    printf("Sha3 digest: %s",z);

    //if Z is still less than d bits long, apply f to S, yielding a new state S
    //truncate Z to d bits
    //no need for that ^
    //                 |

    
    return 0;
}