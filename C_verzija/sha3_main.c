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

/*
    SIZE = 25 * 2^l; 
    l = 3;
    keccak_f rounds = 12 + 2*l;
*/

void hex_value(uint64_t , char* hex);
void hex_value_2(uint64_t , char* hex);
uint64_t rotate_right(uint64_t a, const uint64_t rotc);
int pad(unsigned char *input, size_t *length);
void keccak_f();
void absorb(unsigned char *input, int inputPos);
void squeeze();

uint64_t state[5][5];
uint64_t block[5][5];

int main(int argc, char *argv[]){
  
    if(argc == 1) // if there are no arguments
    { 
        printf("ERROR: Expected at least 1 argument\n");
        return 0;
    }
    int prio = 0;
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

  
    unsigned char *input;
    size_t inputLen = strlen(argv[1]);
    
    //concatenate all the args to the input string
    input = (unsigned char*)malloc(inputLen);
    strcpy(input, argv[1]);

    for(int i = 2; i < argc; i++)
      {
	    inputLen += strlen(argv[i]) + 1; 
        input = (unsigned char*)realloc(input, inputLen);
        strcat(input, " ");
        strcat(input, argv[i]);
      }
    
    //pad the input using the pad function
    int blockCount = pad(input, &inputLen);
	
    printf("%" PRIu64 " ", inputLen);
    if(prio)
    {
        printf("Padded input:\n");
        for (size_t i = 0; i < inputLen; i++)
            printf("%x ", input[i]);
    }
    
    // initialize the state S to a inputing of b zero bits
    memset(state, 0, 5*5*sizeof(uint64_t));

    /*absorb the input into the state: 
    for each block Pi:
    extend Pi at the end by a inputing of CAPACITY zero bits, yielding one of length SIZE
    XOR thafst with state
    apply the block permutation f to the result, yielding a new state S and do it until you run out of blocks to xor with*/

    int pos = 0;
    if(prio)    
        printf("\n\ninput [%d] = %0x\n ", pos, input[pos]);
    
    int inputPos = 0;
    for (size_t i = 0; i < blockCount; i++)
    {
        memset(block, 0, 5*5*sizeof(uint64_t));
        absorb(input, inputPos);
        inputPos += RATE;
        //if(i == 0)
            if(prio)
            {
                printf("\n\nMatrica prije hashovanja :\n");
                for (size_t i = 0; i < 5; i++)
                    for (size_t j = 0; j < 5; j++)
                    {
                        printf("%" PRIu64 " ", block[i][j]);
                        if(j == 4)
                        printf("\n");
                    }
            }
        
        for (int j = 0; j < 5; j++)
            for (int k = 0; k < 5; k++)
                state[j][k] ^= block[j][k];

        keccak_f();
    }
    free(input);
        
	if(prio)
    {
        char *mHex = (char*) malloc(25 * 8 * sizeof(char) + 1);
        mHex[25 * 8] = '\0';
        printf("\n\nMatrica nakon hashovanja :\n");
        for (size_t i = 0; i < 5; i++)
            for (size_t j = 0; j < 5; j++)
            {
                hex_value(state[i][j], mHex);
                printf("%s ", mHex);
                mHex += 16;
                if(j == 4)
                printf("\n");
            }
    }

    squeeze();
    
    //if Z is still less than d bits long, apply f to S, yielding a new state S
    //truncate Z to d bits
    //no need for that ^
    //                 |
    return 0;
}
void squeeze() 
{   

    int hex_amount =  OUTPUT_SIZE * 2 + 1;
    unsigned char hex[hex_amount];

    int z_amount = OUTPUT_SIZE/8 + 1;
    uint64_t z[z_amount];
    
    //initialize Z to be the empty string
    for (size_t i = 0; i < z_amount; i++)
    {
        z[i] = 0x0000000000000000;
    }
    
    z[z_amount -1] = '\0';

    //while the length of Z is less than OUTPUT_SIZE:
    //append the first RATE bytes of STATE to Z
    int p = 0 ; 
    for (size_t i = 0; i < 5; i++)
    {
        for (size_t j = 0; j < 5; j++)
        {
            z[p++] = state[j][i];
            if(p == z_amount)
                break;
        }
        if(p == z_amount)
            break;
    }

    for (size_t i = 0; i < z_amount; i++)
        hex_value_2(z[i], &hex[i * WORD_SIZE *2]);
    hex[hex_amount - 1] = '\0';

    printf("\n\nSha3 digest: 0x%s\n\n", hex);

}
void hex_value(uint64_t dec_value, char* hex)
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

void hex_value_2(uint64_t dec_value, char* hex)
{   
    uint64_t tmp = dec_value;
    uint64_t rem;
    int k = 1;
    for (size_t i = 0; i < 16; i++)
    {
        rem = tmp % 16;
        if(rem < 10)
            hex[i + k] = 48 + rem;
        else
            hex[i + k] = 55 + rem;
        tmp /= 16;
        k = -k;
    }
}

uint64_t rotate_right(uint64_t a, uint64_t rot)
{
    rot &= 0x3f;
    return (a << rot) | (a >> (64 - rot));
}

int pad(unsigned char *input, size_t *length)
{    
    int a;
    if((*length) + 1 == RATE)
    {
      input = (unsigned char*)realloc(input, RATE + 1);
      input[RATE - 1] = 0x18;
      input[RATE] = '\0'; 
    }
    else 
    {
        a = (*length)/RATE;
        a++;
        input = (unsigned char*)realloc(input, a*RATE + 1); 

        input[*length] = 0x01;
        for( int i = (*length) + 1; i < a*RATE - 1; i++)
        {
            input[i] = 0x00;
        }
        *length = a*RATE;
        input[a*RATE - 1] = 0x80;
        input[a*RATE] = '\0';
    }
    return a;
}

void absorb(unsigned char *input, int inputPos)        
{
    uint64_t temp = 0;
    int p = 0;
    for (size_t i = 0; i < 5; i++)
        for (size_t j = 0; j < 5; j++)
        {
            p++;
            if(p <= RATE/8) // if you reached 17th member of the matrix - done
            {
                for (size_t k = 0; k < WORD_SIZE; k++) // put 8 bytes into one member of the matrix
                    { 
                        block[j][i] |= input[i*5*WORD_SIZE + j*WORD_SIZE + WORD_SIZE - 1 - k + inputPos];
                        if(k != WORD_SIZE - 1)
                            block[j][i] <<= WORD_SIZE;
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
        const uint64_t keccak_rotCM[5][5] =
        {
            {0, 36, 3, 41, 18},
            {1, 44, 10, 45, 2},
            {62, 6, 43, 15, 61},
            {28, 55, 25, 21, 56},
            {27, 20, 39, 8, 14}
        };

         uint64_t temp;
         uint64_t B[5][5];
         uint64_t C[5];
         uint64_t D[5];

         
        for(int n = 0; n < KECCAKF_ROUNDS; n++)
        {

            //theta
            for (int x = 0; x < 5; x++)
            {
                C[x] = 0L;
                for (int y = 0; y < 5; y++)
                    C[x] ^= state[x][y];
            }
            for (int x = 0; x < 5; x++)
            {
                D[x] = C[(x + 4) % 5] ^ rotate_right(C[(x + 1) % 5], 1);
                for (int y = 0; y < 5; y++)
                    state[x][y] ^= D[x];
            }

            //rho pi
            memset(B, 0, 5* 5* sizeof(uint64_t));

            for(int x = 0; x < 5; x++)
                for(int y = 0; y < 5; y++)
                    B[y][(2*x + 3*y) % 5] = rotate_right(state[x][y], keccak_rotCM[x][y]);

            //chi
            for (int x = 0; x < 5; x++)
                for (int y = 0; y < 5; y++)
                    state[x][y] = B[x][y] ^ ((~B[(x + 1) % 5][y]) & B[(x + 2) % 5][y]);

            //iota
            //first lane in the state is xored with the constants
               state[0][0] ^= keccakf_rndc[n];
        }
}