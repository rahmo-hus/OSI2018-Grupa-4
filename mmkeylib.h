#ifndef _MMKEYLIB_
#define _MMKEYLIB_
#include <stdint.h>
#include <time.h>

static void scramble(uint8_t key[16]);			//Sums last 3 bits of nth byte with all prior bytes mod 8
static void unscramble(uint8_t key[16]);		//Subtracts all 1--(n-1) from nth 3 bits mod 8
void encode(uint32_t data[4], uint8_t key[16]);
int decode(uint8_t key[16], uint32_t data[4]);
int verify(uint8_t key[16]);
//TODO: Add user friendly functions.
uint8_t* requestKey(uint32_t id, uint32_t duration);	//Don't forget to free the key when it's no longer needed.
time_t getKeyCreationTime(uint8_t key[16]);
time_t getKeyExpirationTime(uint8_t key[16]);
int getKeyGameID(uint8_t key[16]);
int getKeyDuration(uint8_t key[16]);
time_t verifyKey(uint8_t key[16]);
int blacklistKey(uint8_t key[16]); // Adds key to the blacklist so it's no longer usable on this system.


#endif 