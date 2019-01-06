#include "Hangman.h";
#include <stdio.h>
#include <math.h>
int main()
{
	//function playHangman() returns a number of scored points. An example of calling a function is given below
	int pts = playHangman();
	printf("\nYou %s %d points. ", pts < 0 ? "lost" : "achieved", pts < 0 ? (int)fabs(pts) : pts);
	getchar();
	return 0;

}