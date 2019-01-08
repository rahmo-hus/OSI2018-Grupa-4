#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int check_number_of_hits(int br)
{
    int tmp=100-br;
    if(tmp==0)
        return 0;
    if(tmp==100)
        return 4;
    if(tmp==67 || tmp==75 || tmp==80)
        return 3;
    if(tmp==50)
        return 2;
}
int check_existing(int array[],int number,int n)
{
    int i;
    for(i=0; i<n; i++)
        if(array[i]==number)
            return 1;
    return 0;
}
int check_hits(int array[],int number)
{
    int i;
    for(i=0; i<7; i++)
        if(array[i]==number)
            return 1;
    return 0;
}
int lottery(int br)
{
    printf("Welcome to the LOTTERY!\n");
    int user_numbers[7];
    int lottery_numbers[20];
    printf("Please enter 7 numbers from 1-45:\n");
    int i,j;
    for(i=0; i<7; i++)
    {
        printf("Enter %d. number:",i+1);
        scanf("%d",&user_numbers[i]);
    }
    int check;
    do
    {
        check=0;
        for(i=0; i<7; i++)
            if(user_numbers[i]<1 || user_numbers[i]>45)//making sure that user did not enter a number outside number limitations//
            {
                printf("Your %d. entered number is not correct because it is not between 1 and 45.Please enter a new one:",i+1);
                scanf("%d",&user_numbers[i]);
                check++;
            }
        for(i=0; i<6; i++)
            for(j=i+1; j<7; j++)
                if(user_numbers[i]==user_numbers[j])//making sure that user did not enter the same number multiple times//
                {
                    printf("Your %d. entered number is not correct because you have already entered it as your %d. number.Please enter a new one:",j+1,i+1);
                    scanf("%d",&user_numbers[j]);
                    check++;
                }
    }
    while(check);
    printf("All the numbers are correct. We will start the LOTTERY now!GOOD LUCK!!!\n");
    srand(time(NULL));//every time that lottery is played numbers will be different//
    int k,hits=0,tmp=check_number_of_hits(br);
    for(int i=0; i<20; i++)
    {
        lottery_numbers[i]=rand()%45 +1;
        do
        {
            check=0;
            while(check_existing(lottery_numbers,lottery_numbers[i],i))//making sure that lottery numbers are all different//
            {
                lottery_numbers[i]=rand()%45 +1;
                check++;
            }
            for(j=0;j<i;j++)
                for(k=0;k<7;k++)
                if(lottery_numbers[j]==user_numbers[k])
                hits++;
            if(hits>tmp)
                while(check_hits(user_numbers,lottery_numbers[i]))
            {
                lottery_numbers[i]=rand()%45 +1;
                check++;
            }
        }while(check);
        printf("%d. number is: %d\n",i+1,lottery_numbers[i]);
    }
    int number_of_hits=0,points=0,guessed_numbers[7];
    for(i=0; i<20; i++)
        for(j=0; j<7; j++)
            if(user_numbers[j]==lottery_numbers[i])
            {
                guessed_numbers[number_of_hits++]=lottery_numbers[i];
                points+=number_of_hits*10;//points are being given according to what the ordinal number of hit that is//
            }
    printf("Numbers you have guessed are:\n");
    if(number_of_hits==0)
        printf("Sorry,you haven't guessed any numbers!\n");
    for(i=0; i<number_of_hits; i++)
        printf("%d ",guessed_numbers[i]);
    printf("\nYour score for this round of LOTTERY is: %d\n",points);
    printf("Hope you are happy with your score! See you soon!\n");
    return points;
}
