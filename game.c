#include <stdio.h>

#include "game.h"
__declspec(dllexport) int game()
{
	int x;
	scanf_s("%d", &x);
	printf("This is a DLL test. Also your number is %d", x);
	return x + 1;
}