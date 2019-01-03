#define _CRT_SECURE_NO_WARNINGS
#define  QUARTER_SECOND 250
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include "Hangman.h";

bool hangman(bool x)
{
	static int i = 0;
	if (!x)
		i++;
	char *message = "PRESS 'esc' BUTTON TO QUIT PLAYING.";
	char staticline[16] = { ' ',' ', 219, 219 ,219,219,219,219,219,219,219,219,219,219,219,'\0' },//extended ASCII characters
		bar[4] = { ' ', ' ', 219, '\0' }, //loop 7
		stand[7] = { 219,219, 219, 219,219,219,'\0' },
		hang1[13] = { ' ',' ',219,' ',' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ',179,'\0' },
		hang2[13] = { ' ',' ',219,' ',' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ','O','\0' },
		hang3x2[13] = { ' ',' ',219,' ',' ',' ' ,' ' ,' ' ,' ' ,' ' ,0x5C,179,'\0' },
		hang4x1[12] = { ' ',' ',219,' ',' ' ,' ' ,' ' ,' ' ,' ' ,' ',0x2F, '\0' },
		hang4x2[14] = { ' ',' ',219,' ',' ' ,' ' ,' ' ,' ' ,' ' ,' ',0x5C,179,0x2F, '\0' },
		hang5[14] = { ' ',' ',219,' ',' ' ,' ' ,' ' ,' ' ,' ' ,' ',0x2F,' ', 0x5C, '\0' };
	switch (i)
	{
	case 0:
		printf("%s%105s", staticline, message);
		for (int j = 0; j < 7; j++, printf("%s\n", bar));
		printf("%s\n", stand);
		return true;
	case 1:
		printf("%s%105s%s\n", staticline, message, hang1);
		for (int j = 0; j < 6; j++, printf("%s\n", bar));
		printf("%s\n", stand);
		return true;
	case 2:
		printf("%s%105s%s\n%s\n", staticline, message, hang1, hang2);
		for (int j = 0; j < 5; j++, printf("%s\n", bar));
		printf("%s\n", stand);
		return true;
	case 3:
		printf("%s%105s%s\n%s\n%s\n%s\n", staticline, message, hang1, hang2, hang1, hang1);
		for (int j = 0; j < 3; j++, printf("%s\n", bar));
		printf("%s\n", stand);
		return true;
	case 4:
		printf("%s%105s%s\n%s\n%s\n%s\n", staticline, message, hang1, hang2, hang3x2, hang1);
		for (int j = 0; j < 3; j++, printf("%s\n", bar));
		printf("%s\n", stand);
		return true;
	case 5:
		printf("%s%105s%s\n%s\n%s\n%s\n", staticline, message, hang1, hang2, hang4x2, hang1);
		for (int j = 0; j < 3; j++, printf("%s\n", bar));
		printf("%s\n", stand);
		return true;
	case 6:
		printf("%s%105s%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", staticline, message, hang1, hang2, hang4x2, hang1, hang4x1, bar, bar, stand);
		return true;
	default:
		printf("%s%105s%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", staticline, message, hang1, hang2, hang4x2, hang1, hang5, bar, bar, stand);
		return false;

	}

}
int playHangman()
{
	bool found = true, play = false;
	const char *array[] = { "luxembourg", "bruxelles", "developer","schwartzenegger", "compiler", "algebra", "whatsapp", "minecraft",
		"error", "loser", "resistor", "arbitrary", "windows", "class","object", "software", "megalopolis", "programmer" };
	char  c, wrong[16] = { ' ' }, *tmp;
	srand(time(NULL));
	int i = rand() % sizeof(array) / sizeof(void*), k = 0, count = 0, remaining = strlen(array[i]), pts = 0;
	tmp = (char*)calloc(strlen(array[i]) * 2 + 1, sizeof(char));
	for (int j = 0; j < strlen(array[i]) * 2; j += 2) { tmp[j] = '_'; tmp[j + 1] = ' '; }
	hangman(found);
	printf("\n\n%s", tmp);
	while(1)
	{
		found = false;
		char key;
		printf("\n\nInsert letter: \n");
	A: c = _getch();
		if (c == 27) { //esc button
			printf("\nYou pressed 'esc' button. \n");
			break;
		}
		else {
			putchar(c);
			Sleep(QUARTER_SECOND);
		}
		if (c >= 'A' && c <= 'Z') c += 32; //uppercase characters are also accepted
		if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == 13)) {
			system("cls"); hangman(true);
			printf("Wrong: %s\n\n%s\n", wrong, tmp);
			printf("\nUnrecognized character!"); continue;
		}if (c == 13) goto A; //if user hits 'enter' nothing happens
		strchr(array[i], c) ? found = true : found = false;
		if (strchr(wrong, c)) { //perform a check if the entered character was not prevously entered
			system("cls"); 
			hangman(true); 
			printf("Wrong: %s\n\n%s\n", wrong, tmp); 
			printf("\nCharacter '%c' already exists!!! \n", c); continue;
		}
		if (!found) { //draw a body part
			wrong[count++] = c; wrong[count++] = ' ';  system("cls");
			play = hangman(found); printf("Wrong: %s", wrong); pts -= 20;
			if (!play) {
				pts -= 10;
				printf("\n\nCorrect word was '%s'.", array[i]);
				break;
			}
			printf("\n\n%s", tmp); continue;
		}
		else
		{
			if (strchr(tmp, c))
			{
				system("cls"); hangman(true); printf("Wrong: %s\n\n%s\n", wrong, tmp);
				printf("\nCharacter '%c' already exists!!! \n", c); continue;
			}
			system("cls"); hangman(found);
			int b, j;
			for (b = 0, j = 0; j < strlen(array[i]); j++, b += 2)
			{
				if (array[i][j] == c) {
					tmp[b] = c; remaining--;
				}
			}
			tmp[b - 1] = '\0'; printf("Wrong: %s", wrong); printf("\n\n%s", tmp);
			if (!remaining) {
				printf("\n\nCONGRATULATIONS! YOU GUESSED THE CORRECT WORD\n");
				pts += 100;
				break;
			}
		}

	}
	printf("\n\nGAME OVER");
	free(tmp);
	return pts;
}