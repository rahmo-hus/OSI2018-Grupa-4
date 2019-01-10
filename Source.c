#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <string.h>
int is_valid(char* str)
{
	int i = 0;
	while (str[i]) {
		if (!strcmp(str, "q") || !strcmp(str, "Q"))
			return 1;
		if (!(str[i] >= '0' && str[i++] <= '9'))
			return 0;
	}
	return 1;
}
int inputNum(int* checkpoint)
{
	char array[50];
AGAIN: printf("Input: ");
	gets(array);
	if (!is_valid(array))
	{
		printf("Invalid input.\n");
		goto AGAIN;
	}
	if (!strcmp(array, "q") || !strcmp(array, "Q")) *checkpoint = 0;
	else *checkpoint = 1;
	return atoi(array);
}

int main()
{
	unsigned int  k_number, s_number, count = 1, points = 0, arr[10], j, m, t = 0, s_count;
	int checkpoint;
	printf("**************************************************");
	printf("\n*************** *GUESS THE NUMBER* ***************");
	printf("\n**************************************************");
	printf("\n\nI imagined number in the range from 0 to 100,try to guess it!!! Press 'q' to quit.\n\n");
	int numberOfGame;

	FILE* f;
	f = fopen("Playlist.dat", "rb+");
	if (!f)
	{
		f = fopen("Playlist.dat", "wb");
		numberOfGame = 1;
		fwrite(&numberOfGame, 1, sizeof(int), f);
		fclose(f);
	}
	else {
		fread(&numberOfGame, sizeof(int), 1, f);
		rewind(f);
		numberOfGame++;
		fwrite(&numberOfGame, 1, sizeof(int), f);
		numberOfGame--;
		fclose(f);
	}

	if (numberOfGame <= 3)
	{
		srand(time(NULL));
		s_count = rand() % 5 + 1;

		srand(time(NULL));
		s_number = rand() % 100;
		count = 1;
		j = 0;
		int tmp=0;      // prethodno unesen broj
		int manji = 0;  // 0 ili 1
		do
		{
			do
			{
				k_number = inputNum(&checkpoint);
				if (!checkpoint) return printf("You pressed Quit button. \nYou currently have 0 points.GAME OVER\n"), 0;
				if (k_number < 1 || k_number>100) printf("Out of range.\n");
				if (manji==0 && tmp>k_number || manji==1 && tmp<k_number) 
				{
					if (tmp>k_number)
					{
						printf("The number you have entered was smaller that your previously entered and we told you out number is bigger than yours.Please enter again.\n");
					}
					else
					{
						printf("The number you have entered was bigger that your previously entered and we told you out number is smaller than yours.Please enter again.\n");
					}
					count--;
					continue;
				}
			} while (k_number < 1 || k_number>100);


			arr[j] = k_number;
			for (m = 0, t = 0; m < j; m++)
			{
				if (arr[m] == k_number)
					t = 1;
			}
			if (t == 0)
			{
				if (k_number < s_number && count < s_count)
				{   
			
					printf("The imagined number is greater than Yours.\n");
					manji = 0;
					tmp = k_number;
				}

				else if (k_number > s_number && count < s_count)
				{
					printf("The imagined number is less than Yours.\n");
					manji = 1;
					tmp = k_number;
				}
				else if (k_number != s_number && count == s_count)
				{
					printf("Congratulations!You have guessed our number from %d.time!\n", count);
					points += 100 / count;
					printf("You currently have: %d points!\n", points);
					break;
				}
				else if (k_number == s_number && count < s_count)
				{
					printf("Congratulations!You have guessed our number from %d.time!\n", count);
					points += 100 / count;
					printf("You currently have: %d points!\n", points);
					break;
				}
				count++;
			}
			else
				printf("You have already entered the same number!\n");
			j++;
		} while (k_number != s_number);
	}
	else
	{
		srand(time(NULL));
		s_number = rand() % 100;
		count = 1;
		j = 0;
		do
		{
			do
			{
				k_number = inputNum(&checkpoint);
				if (!checkpoint) return printf("You pressed Quit button. \nYou currently have 0 points.GAME OVER\n"), 0;
				if (k_number < 1 || k_number>100) printf("Out of range.\n");
			} while (k_number < 1 || k_number>100);

			
			arr[j] = k_number;
			for (m = 0, t = 0; m < j; m++)
			{
				if (arr[m] == k_number) t = 1;
			}
			if (t == 0)
			{
				{
					if (k_number < s_number && count < 5)
					{
						printf("The imagined number is greater than Yours.\n");

					}
					else if (k_number > s_number && count < 5)
					{
						printf("The imagined number is less than Yours.\n");

					}
					else if (k_number == s_number && count < 5)
					{
						printf("Congratulations!You have guessed our number from %d.time!\n", count);
						points += 100 / count;
						printf("You currently have: %d points!\n", points);
						break;
					}
					else if (k_number == s_number && count > 5)
					{
						printf("We are sorry! The imagined number is: %d.  You have used all attempts. More luck in the next game!  \n ", s_number);

						printf("You currently have: %d points!\n", points);
						break;
					}
					else if (k_number == s_number && count == 5)
					{
						printf("Congratulations!You have guessed our number from %d.time!\n", count);
						points += 100 / count;
						printf("You currently have: %d points!\n", points);
						break;
					}
					else if (k_number != s_number && count == 5)
					{
						printf("We are sorry! The imagined number is: %d.  You have used all attempts. More luck in the next game! \n ", s_number);

						printf("You currently have: %d points!\n", points);
						break;
					}
					count++;
				}
			}
			else printf("You have already entered the same number!\n");
			j++;
		} while (s_number != k_number);
	}
	getchar();
	getchar();
}

