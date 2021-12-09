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
        input[length] = 0x81;
    }
    else 
    {
        a = length/RATE;
        input[length] = 0x80;
        for(unsigned int i = length + 1 ; i < (++a)*RATE - 1 ; i++)
        {
            input[i] = 0;
        }
        input[RATE - 1] = 0x01;
    }
    return a;
}

int main(int argc, char *argv[]){

    if(argc != 1) // if there no arguments
    { 
        printf("ERROR: Expected atleast 1 argument\n");
        return 0;
    }
    /*
    TO DO 
    pad the input N using the pad function, yielding a padded bit string P with a length divisible by r such that len(p)/r is an integer)
    break P into n consecutive r-bit pieces P0, ..., Pn−1
    initialize the state S to a string of b zero bits
    absorb the input into the state: for each block Pi:
    extend Pi at the end by a string of c zero bits, yielding one of length b
    XOR that with S
    apply the block permutation f to the result, yielding a new state S

    initialize Z to be the empty string
    while the length of Z is less than d:
    append the first r bits of S to Z
    if Z is still less than d bits long, apply f to S, yielding a new state S
    truncate Z to d bits*/



    int inputLen;
    //concatenate all the args to the input string
    char *input = (char*)malloc(strlen(argv[1]));
    strcpy(input, argv[1]);

    for(int i = 2; i < argc; i++)
    {
        input = (char*)realloc(input, (strlen(argv[i])+1));
        strcat(input, " ");
        strcat(input, argv[i]);
    }
    //calculate the entire length
    for (int i = 1; i <= argc; i++)
    {
        inputLen += strlen(argv[i]);
    }
    //add the spaces
    if(argc > 2)
        inputLen += argc - 2;
    
    //pad the input using the pad function
    int blockCount = pad(input,inputLen - 1);
    
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

    // initialize the state S to a inputing of b zero bits
    char* state[MATRIX_DIM][MATRIX_DIM];
    for(int i = 0; i < MATRIX_DIM; i++)
        for (int k = 0; k < MATRIX_DIM; k++)
            *(state[i][k]) = 0x00;

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
                *(state[j][k]) ^= blocks[i][j][k];
                keccak_f(&state);
            }
        }
    }

    //initialize Z to be the empty string
    char z[OUTPUT_SIZE + 1];
    for (size_t i = 0; i <= OUTPUT_SIZE; i++)
    {
        z[i] = 0x00;
    }
    z[OUTPUT_SIZE] = '\0';
    
    //while the length of Z is less than d:
    //append the first r bits of S to Z
    p=0;
    for (size_t i = 0; i < MATRIX_DIM; i++)
    {
        for (size_t j = 0; j < MATRIX_DIM; j++)
        {
            z[p++] = *(state[i][j]);
            if(p == OUTPUT_SIZE)
                break;
        }
        if(p == OUTPUT_SIZE)
            break;
    }
    printf("Sha3 digest: %s",z);

    //if Z is still less than d bits long, apply f to S, yielding a new state S
    //truncate Z to d bits
    //no need for that ^
    //                 |

    
    return 0;
}