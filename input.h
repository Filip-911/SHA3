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

    void addPadding();
    void keccakf();
    void reset();

protected:
    uint64_t A[25];
	unsigned int blockLen;
	uint8_t *buffer;
	unsigned int bufferLen;
	unsigned int length;

};

#endif // INPUT_H_INCLUDED
