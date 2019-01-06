#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int lottery()
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
            if(user_numbers[i]<1 || user_numbers[i]>45)
            {
                printf("Your %d. entered number is not correct because it is not between 1 and 45.Please enter a new one:",i+1);
                scanf("%d",&user_numbers[i]);
                check++;
            }
        for(i=0; i<6; i++)
            for(j=i+1; j<7; j++)
                if(user_numbers[i]==user_numbers[j])
                {
                    printf("Your %d. entered number is not correct because you have already entered it as your %d. number.Please enter a new one:",j+1,i+1);
                    scanf("%d",&user_numbers[j]);
                    check++;
                }
    }
    while(check);
    printf("All the numbers are correct. We will start the LOTTERY now!GOOD LUCK!!!\n");
    srand(time(NULL));
    for(int i=0; i<20; i++)
    {
        lottery_numbers[i]=rand()%45 +1;
        printf("%d. number is: %d\n",i+1,lottery_numbers[i]);
    }
    int number_of_hits=0,points=0,guessed_numbers[7];
    for(i=0; i<20; i++)
        for(j=0; j<7; j++)
            if(user_numbers[j]==lottery_numbers[i])
            {
                guessed_numbers[number_of_hits++]=lottery_numbers[i];
                points+=number_of_hits*10;
            }
    printf("Numbers you have guessed are:\n");
    for(i=0; i<number_of_hits; i++)
        printf("%d ",guessed_numbers[i]);
    printf("\nYour score for this round of LOTTERY is: %d\n",points);
    printf("Hope you are happy with your score! See you soon!\n");
    return points;
}
