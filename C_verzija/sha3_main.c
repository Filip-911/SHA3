#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#define WORD_SIZE (64/8)
#define RATE (1088/8)
#define CAPACITY (512/8)
#define SIZE (1600/8)
#define OUTPUT_SIZE (256/8)

#define KECCAKF_ROUNDS 24
#define MATRIX_DIM 5

/*
    SIZE = 25 * 2^l; 
    l = 3;
    keccak_f rounds = 12 + 2*l;
*/

char hex_value(uint64_t , char* hex);
uint64_t rotate_left(uint64_t a, const uint64_t rotc);
int pad(char *input, size_t length);
void keccak_f();
void absorb();

uint64_t state[MATRIX_DIM][MATRIX_DIM];
uint64_t block[MATRIX_DIM][MATRIX_DIM];
char *input;

int main(int argc, char *argv[]){
  
    if(argc == 1) // if there are no arguments
    { 
        printf("ERROR: Expected at least 1 argument\n");
        return 0;
    }
    
        // constants
        const uint64_t keccakf_rndc[25] = {
            0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
            0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
            0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
            0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
            0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
            0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
            0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
            0x8000000000008080, 0x0000000080000001, 0x8000000080008008
        };
        const uint64_t keccakf_rotc[25] = {
            1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14,
            27, 41, 56, 8,  25, 43, 62, 18, 39, 61, 20, 44
        };
         uint64_t temp;
         uint64_t B[5][5];
         uint64_t C[5];
         uint64_t D[5];
         uint64_t pi[1][1];

    int p;
    /*TO DO 
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

  
    size_t inputLen = strlen(argv[1]);
    
    //concatenate all the args to the input string
    input = (char*)malloc(inputLen);
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
        puts(input);

    // initialize the state S to a inputing of b zero bits
    for(int i = 0; i < MATRIX_DIM; i++)
        for (int k = 0; k < MATRIX_DIM; k++)
            state[i][k] = 0x0000000000000000;



    /*absorb the input into the state: 
    for each block Pi:
    extend Pi at the end by a inputing of CAPACITY zero bits, yielding one of length SIZE
    XOR that with state
    apply the block permutation f to the result, yielding a new state S and do it until you run out of blocks to xor with*/

    for (int i = 0; i < blockCount; i++)
    {    
        for(int i = 0; i < MATRIX_DIM; i++)
            for (int k = 0; k < MATRIX_DIM; k++)
                block[i][k] = 0x0000000000000000;
        absorb();
        int p = 0;/*
        for (size_t i = 0; i < MATRIX_DIM; i++)
        {
            for (size_t j = 0; j < MATRIX_DIM; j++)
            {
                if(p < RATE)
                {
                    for (size_t k = 0; k < 8; k++)
                    {
                        block[i][j] += input[i*5*WORD_SIZE/8 + j*WORD_SIZE/8 + k];
                        block[i][j] <<= 8;
                    }
                    p += 8;
                }
            }
        }*//*
        input+= RATE;
        printf("matrica prije hashovanja :\n");
        for (size_t i = 0; i < MATRIX_DIM; i++)
        {
            for (size_t j = 0; j < MATRIX_DIM; j++)
            {
                printf("%" PRIu64 " ", block[i][j]);
                if(j == 4)
                printf("\n");
            }
        }*/

        for (int j = 0; j < MATRIX_DIM; j++)
        {
            for (int k = 0; k < MATRIX_DIM; k++)
            {
                state[j][k] ^= block[j][k];
            }
        }
        //keccak_f();
        
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
                D[x] = C[(x - 1) % MATRIX_DIM] ^ rotate_left(C[(x + 1) % MATRIX_DIM], 1);
                for (int y = 0; y < MATRIX_DIM; y++)
                {
                    state[x][y] ^= D[x];
                }
            }
            //rho
            int x = 1;
            int y = 0;
            for(int t = 0; t < 24; t++)
            {
                state[x][y] = rotate_left(state[x][y], keccakf_rotc[t]);

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
            //chi
            for (int x = 0; x < MATRIX_DIM; x++)
            {
                for (int y = 0; y < MATRIX_DIM; y++)
                    state[x][y] = state[x][y] ^ (~state[(x + 1) % MATRIX_DIM][y] & state[(x + 2) % MATRIX_DIM][y]);
            }

            //iota
            //arbitrary lane in the state is xored with the constants
               state[0][0] ^= keccakf_rndc[n];
        }
    }
    free(input);

    //initialize Z to be the empty string
    int z_amount = OUTPUT_SIZE/8;
    uint64_t z[z_amount + 1];
    for (size_t i = 0; i < z_amount; i++)
    {
        z[i] = 0x0000000000000000;
    }
    z[z_amount] = '\0';
    
    //while the length of Z is less than OUTPUT_SIZE:
    //append the first RATE bytes of STATE to Z
    p=0;
    for (size_t i = 0; i < MATRIX_DIM; i++)
    {
        for (size_t j = 0; j < MATRIX_DIM; j++)
        {
            z[p++] = state[i][j];
            if(p == z_amount)
                break;
        }
        if(p == z_amount)
            break;
    }

    int hex_amount =  OUTPUT_SIZE * 2 + 1;
    char hex[hex_amount];
    for (size_t i = 0; i < z_amount; i++)
    {
        hex_value(z[i], &hex[i * WORD_SIZE *2]);
    }
    hex[hex_amount - 1] = '\0';

    printf("\n\nSha3 digest: 0x%s\n\n", hex);
    for (size_t i = 0; i < hex_amount; i++)
    {
        printf("%c", hex[i]);
    }
    printf("\n\n");
    for (size_t i = 0; i < z_amount; i++)
    {
        printf("%" PRIu64 " ", z[i]);
    }
    printf("\n\n");
    
    //if Z is still less than d bits long, apply f to S, yielding a new state S
    //truncate Z to d bits
    //no need for that ^
    //                 |
    return 0;
}

char hex_value(uint64_t dec_value, char* hex)
{   
    uint64_t tmp = dec_value;
    uint64_t rem;
    for (size_t i = 0; i < 16; i++)
    {
        rem = tmp % 16;
        if(rem < 10)
            hex[15 - i] = 48 + rem;
        else
            hex[15 - i] = 55 + rem;
        tmp /= 16;
    }
}

uint64_t rotate_left(uint64_t a, const uint64_t rotc)
{
    uint64_t rot = rotc % 64;
    uint64_t tmp;
    while(rot)
    {   
        tmp = a & 0x0000000000000001;
        a >>= 1;
        if(tmp)
            a |= 0x8000000000000000;

        rot--;
    }
    return a;
}

int pad(char *input, size_t length)
{    
    int a;
    if(length + 1 == RATE)
    {
      input = (char*)realloc(input, length + 2);
      input[length - 2] = 0x81; //strcat(input, "0x81");
      input[length - 1] = '\0'; //strcat(input, "\n");
    }
    else 
    {
        a = length/RATE;
        a++;
        input = (char*)realloc(input, a*RATE + 1); 

        input[length] = 0x80; //strcat(input, "0x80");
        for( int i = length + 1; i < a*RATE - 2; i++)
        {
            input[i] = 0x00; //strcat(input, "0x00");
        }
        input[a*RATE - 2] = 0x01; //strcat(input, "0x01");
        input[a*RATE - 1] = '\0'; //strcat(input, "\n");
    }
    return a;
}
void absorb()        
{
    int p = 0;
    for (size_t i = 0; i < MATRIX_DIM; i++)
    {
        for (size_t j = 0; j < MATRIX_DIM; j++)
        {
            if(p < RATE/8)
            {
                for (size_t k = 0; k < 8; k++)
                {
                    block[i][j] += input[i*5*WORD_SIZE/8 + j*WORD_SIZE/8 + k];
                    block[i][j] <<= 8;
                }
                p += 8;
            }
        }
    }
}
void keccak_f()
{

        // constants
        const uint64_t keccakf_rndc[25] = {
            0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
            0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
            0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
            0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
            0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
            0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
            0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
            0x8000000000008080, 0x0000000080000001, 0x8000000080008008
        };
        const uint64_t keccakf_rotc[25] = {
            1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14,
            27, 41, 56, 8,  25, 43, 62, 18, 39, 61, 20, 44
        };
         uint64_t temp;
         uint64_t B[5][5];
         uint64_t C[5];
         uint64_t D[5];
         uint64_t pi[1][1];

         
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
                D[x] = C[(x - 1) % MATRIX_DIM] ^ rotate_left(C[(x + 1) % MATRIX_DIM], 1);
                for (int y = 0; y < MATRIX_DIM; y++)
                {
                    state[x][y] ^= D[x];
                }
            }
            //rho
            int x = 1;
            int y = 0;
            for(int t = 0; t < 24; t++)
            {
                state[x][y] = rotate_left(state[x][y], keccakf_rotc[t]);

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
            //chi
            for (int x = 0; x < MATRIX_DIM; x++)
            {
                for (int y = 0; y < MATRIX_DIM; y++)
                    state[x][y] = state[x][y] ^ (~state[(x + 1) % MATRIX_DIM][y] & state[(x + 2) % MATRIX_DIM][y]);
            }

            //iota
            //arbitrary lane in the state is xored with the constants
               state[0][0] ^= keccakf_rndc[n];
        }
}

