#ifndef QUIZ_H_INCLUDED
#define QUIZ_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct kviz
{
    char tekst_pitanja[1800],prvi_odgovor[51],drugi_odgovor[51],treci_odgovor[51];
    int tacan_odgovor;
} KVIZ;

void kviz_ucitavanje(FILE *fp, KVIZ *niz);
int kviz_ispis(KVIZ *niz);
int Quiz_game();

#endif // QUIZ_H_INCLUDED
