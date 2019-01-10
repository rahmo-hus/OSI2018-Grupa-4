#ifndef QUIZ_H_INCLUDED
#define QUIZ_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct quiz
{
    char text_questions[1800],first_answer[51],second_answer[51],third_answer[51];
    int correct_answer;
} QUIZ;

void quiz_loading(FILE *fp, QUIZ *array);
int quiz_print(QUIZ *array);
int Quiz_game();

#endif // QUIZ_H_INCLUDED

