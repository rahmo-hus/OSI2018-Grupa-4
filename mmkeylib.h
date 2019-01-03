#ifndef _MMKEYLIB_
#define _MMKEYLIB_
#include <stdint.h>
static void scramble(uint8_t key[16]);			//Sums last 3 bits of nth byte with all prior bytes mod 8
static void unscramble(uint8_t key[16]);		//Subtracts all 1--(n-1) from nth 3 bits mod 8
void encode(uint32_t data[4], uint8_t key[16]);
int decode(uint8_t key[16], uint32_t data[4]);
int verify(uint8_t key[16]);
//TODO: Add user friendly functions.

#endif 