#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#define KECCAKF_ROUNDS 24

#include <cstdio>
#include <cstdint>
#include <cstring>

#include <vector>
#include <array>

#include <iostream>
#include <sstream>
#include <stddef.h>
#include <stdint.h>

class input
{
public:
    input();
    ~input();
    void addPadding();
    void keccakf();
    void reset();
    void addData(uint8_t input);
    void addData(const uint8_t *input, unsigned int off, unsigned int len);
    void processBuffer();
    void digest(uint64_t *A,unsigned int hashLength);

protected:
    uint64_t A[25];
	unsigned int blockLen;
	uint8_t *buffer;
	unsigned int bufferLen;

};

#endif // INPUT_H_INCLUDED
