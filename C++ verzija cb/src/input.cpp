#include "input.h"

    input::input()
    {
        uint64_t* A = new uint64_t[25];
        memset(A, 0, 25*sizeof(uint64_t));
        blockLen = 200;
        buffer = new uint8_t[blockLen];
        memset(buffer, 0, blockLen*sizeof(uint8_t));
        bufferLen = 0;
    }

    input::~input()
    {
        delete[] A;
        delete[] buffer;
    }

    void input::addData(uint8_t input)
    {
        buffer[bufferLen] = input;
        if(++(bufferLen) == blockLen)
        {
            processBuffer();
        }
    }

    void input::addData(const uint8_t *input, unsigned int off, unsigned int len)
    {
        while (len > 0)
        {
            unsigned int cpLen = 0;
            if((blockLen - bufferLen) > len)
            {
                cpLen = len;
            }
            else
            {
                cpLen = blockLen - bufferLen;
            }

            for(unsigned int i = 0 ; i!=cpLen ; i++)
            {
                buffer[bufferLen+i] = input[off+i];
            }
            bufferLen += cpLen;
            off += cpLen;
            len -= cpLen;
            if(bufferLen == blockLen)
            {
                processBuffer();
            }
        }
    }

    void input::processBuffer()
    {
        for(unsigned int i = 0 ; i < blockLen/8 ; i++)
        {
            A[i] ^= ((uint64_t)buffer[i]);
        }
        keccakf();
        bufferLen = 0;
    }

    void input::addPadding()
    {
        if(bufferLen + 1 == blockLen)
        {
            buffer[bufferLen] = (uint8_t) 0x81;
        }
        else
        {
            buffer[bufferLen] = (uint8_t) 0x80;
            for(unsigned int i = bufferLen + 1 ; i < blockLen - 1 ; i++)
            {
                buffer[i] = 0;
            }
            buffer[blockLen - 1] = (uint8_t) 0x01;
        }
    }

    void input::digest(uint64_t *A,unsigned int hashLength)
    {
        unsigned int lengthInBytes = hashLength / 8;
        addPadding();
        processBuffer();

    }

    void input::keccakf()
    {
        int temp;
        uint64_t B[5][5];
        uint64_t C[5];
        uint64_t D[5];
        uint64_t pi[1][1];


            // constants
        const uint64_t keccakf_rndc[24] = {
            0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
            0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
            0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
            0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
            0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
            0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
            0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
            0x8000000000008080, 0x0000000080000001, 0x8000000080008008
        };
        const int keccakf_rotc[24] = {
            1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14,
            27, 41, 56, 8,  25, 43, 62, 18, 39, 61, 20, 44
        };

        for(int n = 0; n < KECCAKF_ROUNDS; n++)
        {

            //theta
            for (int x = 0; x < 5; x++)
            {
                C[x] = A[index(x, 0)];
                for (int y = 0; y < 5; y++)
                    C[x] = C[x] ^ A[index(x, y)];
            }

            for (int x = 0; x < 5; x++)
            {
                D[x] = C[x-1] ^ rotate(C[x+1],1);
                for (int y = 0; y < 5; y++)
                {
                    A[index(x, y)] = A[index(x, y)] ^ D[x];
                }
            }

            //chi
            for (int x = 0; x < 5; x++)
            {
                for (int y = 0; y < 5; y++)
                    A[index(x, y)] = A[index(x, y)] ^ ((~A[index((x + 1) % 5, y)]) & A[index((x + 2) % 5, y)]);
            }

            //rho
            int x = 1;
            int y = 0;
            for(int t = 0; t < 25; t++)
            {
                A[index(x, y)] = rotate(A[index(x, y)], keccakf_rotc[t]);

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
                    pi[1][1] = A[index(x, y)];
                    temp = x;
                    x = y;
                    y = 2*temp + 3*y;
                    B[index(x, y)] = pi[1][1];
                }
            }
            A[index(x, y)] = B[index(x, y)];

            //iota
                A[n] ^= keccakf_rndc[n];
        }
    }

    void input::reset()
    {
        for(unsigned int i = 0 ; i<25 ; i++)
        {
            A[i] = 0L;
        }
        bufferLen = 0;
    }
