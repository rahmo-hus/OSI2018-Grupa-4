#ifndef QUIZ_C_INCLUDED
#define QUIZ_C_INCLUDED
#include "Quiz.h"

void kviz_ucitavanje(FILE *fp, KVIZ *niz)
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
        while((x=fgetc(fp))!='\n') //ucitavanje svakom pitanja pojedinacno
            string[a++]=x;
        string[a]='\0';

        b=0;
        a=0;
        while(string[a]!='-' && string[a]!='\n')
            niz[i].tekst_pitanja[b++]=string[a++];
        niz[i].tekst_pitanja[b]='\0';

        a++;
        b=0;
        while(string[a]!='-' && string[a]!='\n')
            niz[i].prvi_odgovor[b++]=string[a++];
        niz[i].prvi_odgovor[b]='\0';
        a++;
        b=0;
        while(string[a]!='-' && string[a]!='\n')
            niz[i].drugi_odgovor[b++]=string[a++];
        niz[i].drugi_odgovor[b]='\0';
        a++;
        b=0;
        while(string[a]!='-' && string[a]!='\n')
            niz[i].treci_odgovor[b++]=string[a++];
        niz[i].treci_odgovor[b]='\0';
        a++;
        niz[i].tacan_odgovor=string[a]-'0';

    }
}

int kviz_ispis(KVIZ *niz)
{
    int random_broj,pom[5],br=1,a=0,p=0,j,tacan,broj_bodova=0;

    srand(time(NULL));
    random_broj=rand()%(16+1-0)+0; ///rand() % (max_number + 1 - minimum_number) + minimum_number
    pom[a++]=random_broj;

    char x[2]="";
    do
    {

        printf("\n1.QUESTION: \n%s\n 1.%s\n 2.%s\n 3.%s\nClose the game[0] or enter the ordinal number of the correct answer: ",niz[random_broj].tekst_pitanja,niz[random_broj].prvi_odgovor,niz[random_broj].drugi_odgovor,niz[random_broj].treci_odgovor);
        scanf("%s",x);
        tacan=x[0]-'0';
    }
    while(x[0]<48 || x[0]>51);

    if(tacan==0)
        return broj_bodova;

    else if(tacan==niz[random_broj].tacan_odgovor)
    {
        printf("\n>>>Correct answer!<<<\n");
        broj_bodova+=20;
    }
    else
    {
        printf("\n>>>Incorrect answer! The correct answer is answer number %d.<<<\n",niz[random_broj].tacan_odgovor);
        broj_bodova-=30;
    }

    while(br<=4)
    {
        p=0;
        srand(time(NULL));
        random_broj=rand()%(16+1-0)+0;
        for(j=0; j<a; j++)
            if(random_broj==pom[j]) //provjera da se ne ponovi isto pitanje
                p++;
        if(p==0)
        {
            pom[a++]=random_broj;
            br++;

            do
            {
                printf("\n%d.QUESTION: \n%s\n 1.%s\n 2.%s\n 3.%s\nClose the game[0] or enter the ordinal number of the correct answer: ",br,niz[random_broj].tekst_pitanja,niz[random_broj].prvi_odgovor,niz[random_broj].drugi_odgovor,niz[random_broj].treci_odgovor);
                scanf("%s",x);
                tacan=x[0]-'0';
            }
            while(x[0]<48 || x[0]>51);
            if(tacan==0)
                return broj_bodova;

            else if(tacan==niz[random_broj].tacan_odgovor)
            {
                printf("\n>>>Correct answer!<<<\n");
                broj_bodova+=20;
            }
            else
            {
                printf("\n>>>Incorrect answer! The correct answer is answer number %d.<<<\n",niz[random_broj].tacan_odgovor);
                broj_bodova-=30;
            }

        }
    }

    return broj_bodova;
}

int Quiz_game()
{
    FILE *fp;
    KVIZ *niz;
    int j,ukupan_broj_bodova;
    char x[2]="";
    niz=(KVIZ*)calloc(17,sizeof(KVIZ));
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
            kviz_ucitavanje(fp,niz);
        ukupan_broj_bodova=kviz_ispis(niz);
    }
    else if(j==2)
    {
        if((fp=fopen("general.txt","r"))!=NULL)
            kviz_ucitavanje(fp,niz);
        ukupan_broj_bodova=kviz_ispis(niz);
    }
    else
    {
        if((fp=fopen("music.txt","r"))!=NULL)
            kviz_ucitavanje(fp,niz);
        ukupan_broj_bodova=kviz_ispis(niz);
    }

    printf("\nTHE FINAL NUMBER OF POINTS: %d",ukupan_broj_bodova);
    if(ukupan_broj_bodova==100)
    {
        printf("\nYou have correctly answered all the questions! The prize is an additional 50 points! CONGRATULATIONS!");
        ukupan_broj_bodova=150;
        printf("\nTHE FINAL NUMBER OF POINTS: %d\n",ukupan_broj_bodova);
    }
    return ukupan_broj_bodova;

}

#endif // QUIZ_C_INCLUDED
