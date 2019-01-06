#include "mmkeylib.h" 
#include <time.h>
#include <stdlib.h>
//TODO: Document all of this.
static time_t exp_table[4] = { 3600,86400,604800, -1 };
static void scramble(uint8_t key[16])
{
	for (int i = 14; i >=0; i--)
	{
		key[i] = (key[i] & (0xF0)) | (((key[i] & 0x0F) + (key[i + 1] & 0x0F) + 1) % 10);
	}
}
static void unscramble(uint8_t key[16])
{
	for (int i = 0; i <= 14; i++)
	{
		int8_t a = ((key[i] & 0x0F) - (key[i + 1] & 0x0F) - 1);
		a = a < 0 ? 10 + a : a;
		a %= 10;
		key[i] = (key[i] & (0xF0)) | a;
	}
}
//RNum	6 bits	/ data[0]
//CN	6 bits
//Epoch	32 bits	/ data[1]
//gID	2 bits	/ data[2]
//kID	2 bits	/ data[3]
//48 bits total
void encode(uint32_t data[4], uint8_t key[16])
{
	uint64_t prekey= 0;
	prekey |= (((uint64_t)(data[0] & 0x3F)<<42 | ((uint64_t)data[1]&0xF0000000)<<4));
	prekey |= (((uint64_t)(data[1] & 0x0FFFFFFF)) << 4) | ((uint64_t)(data[2] & 0x03) << 2) | ((uint64_t)data[3] & 0x03);
	int8_t sum = 0;
	for (int i = 0; i <= 15; i++)
	{
		sum += ((prekey >> (45 - i * 3)) & 0x07)*(-1*i%2);
	}
	sum = sum < 0 ? -sum : sum;
	prekey |= ((uint64_t)sum & 0x3F) << 36;
	//printf("\nprekey enc:%llx", prekey);
	for (int i = 0; i <= 15; i++)
	{
		key[i] = 0x30 | (prekey >> (i * 3)) & 0x7;
	}
	
	scramble(key);
	printf("\n%.16s", key);
}

int decode(uint8_t key[16], uint32_t data[4])
{
	unscramble(key);
	//printf("\n%.16s", key);
	uint64_t prekey = 0;
	for (int i = 0; i <= 15; i++)
		prekey |= ((uint64_t)key[i] & 0x07) << i * 3;\
	//printf("\nprekey dec:%llx", prekey);
	uint8_t CN = (prekey >> 36) & 0x3F;
	int8_t sum = 0;
	prekey &= 0x0000FC0FFFFFFFFF;
	for (int i = 0; i <= 15; i++)
	{
		sum += ((prekey >> (45 - i * 3)) & 0x07)*(-1 * i % 2);
	}
	data[0] = prekey >> 42;
	data[1] = (prekey >> 4) & 0xFFFFFFFF;
	data[2] = (prekey >> 2) & 0x03;
	data[3] = prekey & 0x03;
	sum = sum < 0 ? -sum : sum;
	scramble(key);
	if (sum == CN) return 1;	//Key Valid
	return 0;					//Key Invalid
}
uint8_t* requestKey(uint32_t id, uint32_t duration)
{
	uint8_t* key = malloc(16);
	uint32_t data[4];
	data[0] = rand() % 64;
	data[1] = time(NULL);
	data[2] = id;
	data[3] = duration;
	encode(data, key);
	return key;
}
time_t getKeyCreationTime(uint8_t key[16])
{
	uint32_t data[4];
	int valid = decode(key, data);
	if (valid)
	{
		return data[1];
	}
	else
		return 0;
}
time_t getKeyExpirationTime(uint8_t key[16])
{
	uint32_t data[4];
	decode(key, data);
	if (data[3] == 3)
	{
		char size = sizeof(time_t);
		if (size == 4) return 0x7FFFFFFF;
		if (size == 8) return 0x7FFFFFFFFFFFFFFF;
	}
	time_t key_time = getKeyCreationTime(key);
	time_t key_expire_time = key_time + exp_table[data[3]];
	return key_expire_time;
}
int getKeyGameID(uint8_t key[16])
{
	uint32_t data[4];
	int valid = decode(key, data);
	if (valid)
	{
		return data[2];
	}
	return -1;
}
int getKeyDuration(uint8_t key[16])
{
	uint32_t data[4];
	int valid = decode(key, data);
	if (valid)
	{
		return exp_table[data[3]];
	}
	return 0;
}
time_t verifyKey(uint8_t key[16])
{
	uint32_t data[4];
	int valid = decode(key, data);
	if (valid)
	{
		time_t time_rem = getKeyExpirationTime(key) - time(NULL);
		if (time_rem > 0) return time_rem;
	}
	return 0;
}