#include "Quiz.h"
#include "Quiz.c"

int main()
{
    FILE *fp;
    int i;
    i=Quiz_game(fp);
    printf("\nKonacan broj bodova iz igrice Quiz je: %d",i);
    return 0;
}
