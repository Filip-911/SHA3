#include <stdio.h>
#include "padding.hpp"

using namespace std;

int main() 
{
    
    //pad the input N using the pad function
    pad(N);
    //yielding a padded bit string P with a length divisible by r (such that n=len(P)/r is an integer)
    //break P into n consecutive r-bit pieces P0, ..., Pn−1
    //initialize the state S to a string of b zero bits
    //absorb the input into the state: for each block Pi:
    //extend Pi at the end by a string of c zero bits, yielding one of length b
    //XOR that with S
    //apply the block permutation f to the result, yielding a new state S
    //initialize Z to be the empty string
    //while the length of Z is less than d:
    //append the first r bits of S to Z
    //if Z is still less than d bits long, apply f to S, yielding a new state S
    //truncate Z to d bits


}