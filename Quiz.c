#ifndef QUIZ_C_INCLUDED
#define QUIZ_C_INCLUDED
#include "Quiz.h"

void quiz_loading(FILE *fp, QUIZ *array)
{
    int i,a,b;

    rewind(fp);
    fseek(fp, 0, SEEK_END);
    int n = ftell(fp);
    rewind(fp);
    for(i=0; i<17; i++)
    {
        char *string;
        string=(char*)calloc(n,sizeof(char));
        a=0;
        char x;
        while((x=fgetc(fp))!='\n') //loading every question individualy//
            string[a++]=x;
        string[a]='\0';

        b=0;
        a=0;
        while(string[a]!='-' && string[a]!='\n')
            array[i].text_questions[b++]=string[a++];
        array[i].text_questions[b]='\0';

        a++;
        b=0;
        while(string[a]!='-' && string[a]!='\n')
            array[i].first_answer[b++]=string[a++];
        array[i].first_answer[b]='\0';
        a++;
        b=0;
        while(string[a]!='-' && string[a]!='\n')
            array[i].second_answer[b++]=string[a++];
        array[i].second_answer[b]='\0';
        a++;
        b=0;
        while(string[a]!='-' && string[a]!='\n')
            array[i].third_answer[b++]=string[a++];
        array[i].third_answer[b]='\0';
        a++;
        array[i].correct_answer=string[a]-'0';

    }
}

int quiz_print(QUIZ *array)
{
    int random_number,pom[5],br=1,a=0,p=0,j,correct,points=0;

    srand(time(NULL));
    random_number=rand()%(16+1-0)+0; ///rand() % (max_number + 1 - minimum_number) + minimum_number
    pom[a++]=random_number;

    char x[2]="";
    do
    {

        printf("\n1.QUESTION: \n%s\n 1.%s\n 2.%s\n 3.%s\nClose the game[0] or enter the ordinal number of the correct answer: ",array[random_number].text_questions,array[random_number].first_answer,array[random_number].second_answer,array[random_number].third_answer);
        scanf("%s",x);
        correct=x[0]-'0';
    }
    while(x[0]<48 || x[0]>51);

    if(correct==0)
        return points;

    else if(correct==array[random_number].correct_answer)
    {
        printf("\n>>>Correct answer!<<<\n");
        points+=20;
    }
    else
    {
        printf("\n>>>Incorrect answer! The correct answer is answer number %d.<<<\n",array[random_number].correct_answer);
        points-=30;
    }

    while(br<=4)
    {
        p=0;
        srand(time(NULL));
        random_number=rand()%(16+1-0)+0;
        for(j=0; j<a; j++)
            if(random_number==pom[j]) //making sure that the same questions does not appear more than once//
                p++;
        if(p==0)
        {
            pom[a++]=random_number;
            br++;

            do
            {
                printf("\n%d.QUESTION: \n%s\n 1.%s\n 2.%s\n 3.%s\nClose the game[0] or enter the ordinal number of the correct answer: ",br,array[random_number].text_questions,array[random_number].first_answer,array[random_number].second_answer,array[random_number].third_answer);
                scanf("%s",x);
                correct=x[0]-'0';
            }
            while(x[0]<48 || x[0]>51);
            if(correct==0)
                return points;

            else if(correct==array[random_number].correct_answer)
            {
                printf("\n>>>Correct answer!<<<\n");
                points+=20;
            }
            else
            {
                printf("\n>>>Incorrect answer! The correct answer is answer number %d.<<<\n",array[random_number].correct_answer);
                points-=30;
            }

        }
    }

    return points;
}

int Quiz_game()
{
    FILE *fp;
    QUIZ *array;
    int j,final_points;
    char x[2]="";
    array=(QUIZ*)calloc(17,sizeof(QUIZ));
    do
    {
        printf("Close the game[0] or choose a category: GEOGRAPHY[1] GENERAL[2] MUSIC[3] \nEnter the number: ");
        scanf("%s",x);
        j=x[0]-'0';
        printf("\n");
    }
    while(x[0]<48 || x[0]>51);

    if(j==0)
        return 0;

    if(j==1)
    {
        if((fp=fopen("geography.txt","r"))!=NULL)
            quiz_loading(fp,array);
        final_points=quiz_print(array);
    }
    else if(j==2)
    {
        if((fp=fopen("general.txt","r"))!=NULL)
            quiz_loading(fp,array);
        final_points=quiz_print(array);
    }
    else
    {
        if((fp=fopen("music.txt","r"))!=NULL)
            quiz_loading(fp,array);
        final_points=quiz_print(array);
    }

    printf("\nTHE FINAL NUMBER OF POINTS: %d",final_points);
    if(final_points==100)
    {
        printf("\nYou have correctly answered all the questions! The prize is an additional 50 points! CONGRATULATIONS!");
        final_points=150;
        printf("\nTHE FINAL NUMBER OF POINTS: %d\n",final_points);
    }
    return final_points;

}

#endif // QUIZ_C_INCLUDED
