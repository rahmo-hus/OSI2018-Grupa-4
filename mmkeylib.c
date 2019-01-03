#include "mmkeylib.h" 
//TODO: Document all of this.
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
	prekey |= ((uint64_t)data[1] & 0x0FFFFFF << 4) | ((uint64_t)data[2] & 0x03 << 2) | ((uint64_t)data[3] & 0x03);
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
	//printf("\n%.16s", key);
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
	if (sum == CN) return 1;	//Key Valid
	return 0;					//Key Invalid
}