#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sha3_functions.h"

#define WORD_SIZE 16
#define RATE (256/WORD_SIZE)       //  12
#define CAPACITY (144/WORD_SIZE)    // +13
#define SIZE (400/WORD_SIZE)       // =25
#define OUTPUT_SIZE (224/WORD_SIZE)

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
        for( int i = length + 1; i < a*RATE - 2; i++)
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
        const uint16_t keccakf_rndc[24] = {
            0x0001, 0x0082, 0x89ff,
            0x9080, 0xa08b, 0xf0f1,
            0x00a1, 0x0089, 0xc08a,
            0x00af, 0x0d09, 0x890a,
            0x00cb, 0xd088, 0x089f,
            0x9083, 0x90f2, 0x0089,
            0x0089, 0x938a, 0x0089,
            0x0089, 0x00c1, 0x0898
        };
        const uint16_t keccakf_rotc[25] = {
            1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14,
            27, 41, 56, 8,  25, 43, 62, 18, 39, 61, 20, 44
        };
         uint16_t temp;
         uint16_t B[5][5];
         uint16_t C[5];
         uint16_t D[5];
         uint16_t pi[1][1];
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
    uint16_t blocks[blockCount][MATRIX_DIM][MATRIX_DIM];
    int p=0;
    for (int i = 0; i < blockCount; i++)
    {
        for (int j = 0; j < MATRIX_DIM; j++)
        {
            for (int k = 0; k < MATRIX_DIM; k++)
            {
                if(p < RATE)
                {
                    blocks[i][j][k] = (uint16_t) input[i*RATE + p]; //
                    p++; //broji koliko clanova je unijeto
                }
                else
                {   
                    blocks[i][j][k] = 0x0000; //ostali bajtovi idu na 0
                }
            }
        }
        p=0;
    }
    free(input);

    // initialize the state S to a inputing of b zero bits
     uint16_t state[MATRIX_DIM][MATRIX_DIM];
    for(int i = 0; i < MATRIX_DIM; i++)
        for (int k = 0; k < MATRIX_DIM; k++)
            state[i][k] = 0x0000;

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
            for (int x = 0; x < MATRIX_DIM; x++)
            {
                C[x] = state[x][0];
                for (int y = 1; y < MATRIX_DIM; y++)
                    C[x] ^= state[x][y];
            }

            for (int x = 0; x < MATRIX_DIM; x++)
            {
                D[x] = C[x-1] ^ rotate_right(C[(x + 1) % MATRIX_DIM], 1);
                for (int y = 0; y < MATRIX_DIM; y++)
                {
                    state[x][y] ^= D[x];
                }
            }

            //chi
            for (int x = 0; x < MATRIX_DIM; x++)
            {
                for (int y = 0; y < MATRIX_DIM; y++)
                    state[x][y] = state[x][y] ^ (~state[(x + 1) % MATRIX_DIM][y] & state[(x + 2) % MATRIX_DIM][y]);
            }

            //rho
            int x = 1;
            int y = 0;
            for(int t = 0; t < 24; t++)
            {
                state[x][y] = rotate_right(state[x][y], keccakf_rotc[t]);

                temp = x;
                x = y;
                y = (2*temp + 3*y) % MATRIX_DIM;
            }
            //merge ?
            //pi
            for(int x = 0; x < MATRIX_DIM; x++)
            {
                for(int y = 0; y < MATRIX_DIM; y++)
                {
                    pi[0][0] = state[x][y];
                    temp = x;
                    x = y;
                    y = (2*temp + 3*y) % MATRIX_DIM;
                    state[x][y] = pi[0][0];
                }
            }

            //iota
            //arbitrary lane in the state is xored with the constants
               state[0][3] ^= keccakf_rndc[n];
        }

    }

    //initialize Z to be the empty string
    uint16_t z[OUTPUT_SIZE + 1];
    for (size_t i = 0; i < OUTPUT_SIZE; i++)
    {
        z[i] = 0x0000;
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
            if(p == OUTPUT_SIZE)
                break;
        }
        if(p == OUTPUT_SIZE)
            break;
    }
    printf("Sha3 digest: %s\n\n0x",z);
    for (size_t i = 0; i < OUTPUT_SIZE; i++)
    {
         printf("%04x", z[i]);
    }
    printf("\n\n");
    for (size_t i = 0; i < OUTPUT_SIZE; i++)
    {
         printf("%04x ", z[i]);
    }    

    //if Z is still less than d bits long, apply f to S, yielding a new state S
    //truncate Z to d bits
    //no need for that ^
    //                 |

    
    return 0;
}