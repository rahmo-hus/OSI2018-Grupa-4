#ifndef QUIZ_H_INCLUDED
#define QUIZ_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct kviz
{
    char tekst_pitanja[2500],prvi_odgovor[100],drugi_odgovor[100],treci_odgovor[100];
    int tacan_odgovor;
} KVIZ;

void kviz_ucitavanje(FILE *fp, KVIZ *niz);
int kviz_ispis(KVIZ *niz);
int Quiz_game(FILE *fp);

#endif // QUIZ_H_INCLUDED
