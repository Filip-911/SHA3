#include "sha3_functions.h"

    void keccak_f(char* A[MATRIX_DIM][MATRIX_DIM])
    {
        int temp;
        char B[5][5];
        char C[5];
        char D[5];
        char pi[1][1];

        for(int n = 0; n < KECCAKF_ROUNDS; n++)
        {

            //theta
            for (int x = 0; x < 5; x++)
            {
                C[x] = *(A[x][0]);
                for (int y = 1; y < 5; y++)
                    C[x] ^= *(A[x][y]);
            }

            for (int x = 0; x < 5; x++)
            {
                D[x] = C[x-1] ^ rotate_right(C[x + 1], 1);
                for (int y = 0; y < 5; y++)
                {
                    *(A[x][y]) ^= D[x];
                }
            }

            //chi
            for (int x = 0; x < 5; x++)
            {
                for (int y = 0; y < 5; y++)
                    *(A[x][y]) = *(A[x][y]) ^ (~(*(A[(x + 1) % 5][y])) & *(A[(x + 2) % 5][y]));
            }

            //rho
            int x = 1;
            int y = 0;
            for(int t = 0; t < 24; t++)
            {
                *(A[x][y]) = rotate_right(*(A[x][y]), keccakf_rotc[t]);

                temp = x;
                x = y;
                y = 2*temp + 3*y;
            }
            //merge ?
            //pi
            for(int x = 0; x < 5; x++)
            {
                for(int y = 0; y < 5; y++)
                {
                    pi[1][1] = *(A[x][y]);
                    temp = x;
                    x = y;
                    y = 2*temp + 3*y;
                    B[x][y] = pi[1][1];
                }
            }
            *(A[x][y]) = B[x][y];

            //iota
            //arbitrary lane in the state is xored with the constants
               *(A[x][y]) ^= keccakf_rndc[n];
        }
    }
    char rotate_right(char a, const char rotc)
    {
        char rot = rotc % 8;
        char tmp;
        while(rot)
        {   
            tmp = a & 0x80;
            a <<= 1;
            if(tmp)
                a |= 0x01;

            rot--;
        }

        return a;

    }