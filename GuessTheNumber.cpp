#define _CRT_SECURE_NO_WARNINGS 
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
int inputNum()
{
	char numbers[100];
	char *endptr;
	int number;

	do
	{
		printf("Guess the number:\n ");
		scanf("%9s", numbers);

		number = strtol(numbers, &endptr, 10);

	} while (*endptr != '\0' || endptr == numbers);


	return number;
}
int GuessTheNumber()
{
	unsigned int  k_broj, s_broj, brojac = 1, i, broj_bodova = 0, niz[10], j, m, t = 0, s_brojac;

	printf("I imagined number in the range from 0 to 100,try to guess it!!!\n");

	for (i = 0; i < 3; i++)
	{
		srand(time(NULL));
		s_brojac = rand() % 5 + 1;

		srand(time(NULL));
		s_broj = rand() % 100;
		brojac = 1;
		j = 0;
		do
		{
			do
			{
				k_broj = inputNum();

			} while (k_broj < 0 || k_broj>100);


			niz[j] = k_broj;
			for (m = 0, t = 0; m < j; m++)
			{
				if (niz[m] == k_broj)
					t = 1;
			}
			if (t == 0)
			{
				if (k_broj < s_broj && brojac < s_brojac)
				{
					printf("The imagined number is greater than Yours.\n");
				}

				else if (k_broj > s_broj && brojac < s_brojac)
				{
					printf("The imagined number is less than Yours.\n");
				}
				else if (k_broj != s_broj && brojac == s_brojac)
				{
					printf("Congratulations!You have guessed our number from %d.time!\n", brojac);
					broj_bodova += 100 / brojac;
					printf("You currently have: %d points!\n", broj_bodova);
					break;
				}
				else if (k_broj == s_broj && brojac < s_brojac)
				{
					printf("Congratulations!You have guessed our number from %d.time!\n", brojac);
					broj_bodova += 100 / brojac;
					printf("You currently have: %d points!\n", broj_bodova);
					break;
				}
				brojac++;
			}
			else
				printf("You have already entered the same number!\n");
			j++;
		} while (k_broj != s_broj);
	}
	srand(time(NULL));
	s_broj = rand() % 100;
	brojac = 1;
	j = 0;
	do
	{
		do
		{
			k_broj = inputNum();
		} while (k_broj < 0 || k_broj>100);

		niz[j] = k_broj;
		for (m = 0, t = 0; m < j; m++)
		{
			if (niz[m] == k_broj) t = 1;
		}
		if (t == 0)
		{
			{
				if (k_broj < s_broj && brojac < 5)
				{
					printf("The imagined number is greater than Yours.\n");

				}
				else if (k_broj > s_broj && brojac < 5)
				{
					printf("The imagined number is less than Yours.\n");

				}
				else if (k_broj == s_broj && brojac < 5)
				{
					printf("Congratulations!You have guessed our number from %d.time!\n", brojac);
					broj_bodova += 100 / brojac;
					printf("You currently have: %d points!\n", broj_bodova);
					break;
				}
				else if (k_broj == s_broj && brojac > 5)
				{
					printf("We are sorry! The imagined number is: %d.  You have used all attempts. More luck in the next game!  \n ", s_broj);

					printf("You currently have: %d points!\n", broj_bodova);
					break;
				}
				else if (k_broj == s_broj && brojac == 5)
				{
					printf("Congratulations!You have guessed our number from %d.time!\n", brojac);
					broj_bodova += 100 / brojac;
					printf("You currently have: %d points!\n", broj_bodova);
					break;
				}
				else if (k_broj != s_broj && brojac == 5)
				{
					printf("We are sorry! The imagined number is: %d.  You have used all attempts. More luck in the next game! \n ", s_broj);

					printf("You currently have: %d points!\n", broj_bodova);
					break;
				}
				brojac++;
			}
		}
		else printf("You have already entered the same number!\n");
		j++;
		} while (s_broj != k_broj);

	return broj_bodova;
}