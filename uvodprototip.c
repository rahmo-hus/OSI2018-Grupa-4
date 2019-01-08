// uvodprototip.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <Windows.h>
#include <time.h>

#define PERR(bSuccess, api){if(!(bSuccess)) printf("%s:Error %d from %s \
on line %d\n", __FILE__, GetLastError(), api, __LINE__);}
void cls(HANDLE hConsole)
{
	COORD coordScreen = { 0, 0 };    /* here's where we'll home the
									 cursor */
	BOOL bSuccess;
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */
	DWORD dwConSize;                 /* number of character cells in
									 the current buffer */

									 /* get the number of character cells in the current buffer */

	bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
	PERR(bSuccess, "GetConsoleScreenBufferInfo");
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	/* fill the entire screen with blanks */

	bSuccess = FillConsoleOutputCharacter(hConsole, (TCHAR) ' ',
		dwConSize, coordScreen, &cCharsWritten);
	PERR(bSuccess, "FillConsoleOutputCharacter");

	/* get the current text attribute */

	bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
	PERR(bSuccess, "ConsoleScreenBufferInfo");

	/* now set the buffer's attributes accordingly */

	bSuccess = FillConsoleOutputAttribute(hConsole, csbi.wAttributes,
		dwConSize, coordScreen, &cCharsWritten);
	PERR(bSuccess, "FillConsoleOutputAttribute");

	/* put the cursor at (0, 0) */

	bSuccess = SetConsoleCursorPosition(hConsole, coordScreen);
	PERR(bSuccess, "SetConsoleCursorPosition");
	return;
}
typedef struct _SCORE_DATE
{
	int32_t score;
	time_t date;
	struct _SCORE_DATE* next;
}SCORE;
typedef struct _MENU_ITEM
{
	char title[16];
	char description[1024]; // 0 length for no description 
	int(*game)();
	SCORE* score_history;
	int unlocked;
	uint8_t key[16];
	int unlock_price;
	int play_price;
}MENU_ITEM;

typedef struct _USER
{
	char name[16];
	int32_t points;
}USER;

int GetBufferWidth(HANDLE hConsoleOutput)
{
	CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
	GetConsoleScreenBufferInfo(hConsoleOutput, &bufferInfo);
	return bufferInfo.dwSize.X;
}
int GetBufferHeight(HANDLE hConsoleOutput)
{
	CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
	GetConsoleScreenBufferInfo(hConsoleOutput, &bufferInfo);
	return bufferInfo.dwSize.Y;
}
int SetCursorPosition(HANDLE hConsoleOutput, short x, short y)
{
	COORD pos = { x,y };
	return SetConsoleCursorPosition(hConsoleOutput, pos);
}
int GetConsoleColor(HANDLE hConsoleOutput)
{
	CONSOLE_SCREEN_BUFFER_INFO conBufInfo;
	GetConsoleScreenBufferInfo(hConsoleOutput, &conBufInfo);
	return conBufInfo.wAttributes;
}

//Loads game information from games.config into an array of menu items and loads game DLLs and returns number of games loaded
int LoadGames(FILE* fp, MENU_ITEM *menu_out)
{
	int dummy_item_count = 0;
	MENU_ITEM menu[4];
	char buffer[1024];
	int incomplete;
	int len;
#define BREAKIFEOF() if (feof(fp)){incomplete = i;break;}
	for (int i = 0; i < 4; i++)
	{
		fgets(menu[i].title, 16, fp);
		menu[i].title[16] = '\0';
		menu[i].title[strcspn(menu[i].title, "\n")] = '\0';
		BREAKIFEOF();
		fgets(menu[i].description, 1024, fp);
		menu[i].description[1024] = '\0';
		menu[i].description[strcspn(menu[i].description, "\n")] = '\0';
		BREAKIFEOF();
		fgets(buffer, 1024, fp);
		buffer[strcspn(buffer, "\n")] = '\0';
		HANDLE gameDLL = LoadLibraryA(buffer);
		if (!gameDLL) { incomplete = i; break; }
		fgets(buffer, 1024, fp);
		buffer[strcspn(buffer, "\n")] = '\0';
		menu[i].game = (int(*)())GetProcAddress(gameDLL, buffer);
		fgets(buffer, 1024, fp);
		buffer[strcspn(buffer, "\n")] = '\0';
		menu[i].unlocked = atoi(buffer);
		fgets(buffer, 1024, fp);
		buffer[strcspn(buffer, "\n")] = '\0';
		strncpy(menu[i].key, buffer, 16);
		fgets(buffer, 1024, fp);
		buffer[strcspn(buffer, "\n")] = '\0';
		menu[i].unlock_price = atoi(buffer);
		fgets(buffer, 1024, fp);
		buffer[strcspn(buffer, "\n")] = '\0';
		menu[i].play_price = atoi(buffer);
		sprintf(buffer, "%s_score.scr",menu[i].title);
		FILE* scr = fopen(buffer, "rb");
		if(scr)
		{
			_LoadHistory(scr, &menu[i]);
			fclose(scr);
		}
	}
	return 4;
}
//Loads history into a specific menu item. For use internally by LoadGames function
int _LoadHistory(FILE* fp, MENU_ITEM *mitem)
{	
	int32_t score = 0;
	time_t time = 0;
	if (!fread(&score, sizeof(int32_t), 1, fp) || !fread(&time, sizeof(time_t), 1, fp)) //Empty score
		return -1;
	SCORE* SD = malloc(sizeof(SCORE));
	SD->score = score;
	SD->date = time;
	SD->next = NULL;
	mitem->score_history = SD;
	SCORE *current = SD;
	while (1)
	{
		fread(&score, sizeof(int32_t), 1, fp);
		fread(&time, sizeof(time_t), 1, fp);
		if (feof(fp))
			break;
		SD = malloc(sizeof(SCORE));
		SD->score = score;
		SD->date = time;
		SD->next = NULL;
		current->next = SD;
		current = current->next;
	}
	return 0;
}
//Saves history into a specified binary file from a specific menu item;
int SaveHistory(FILE* fp, MENU_ITEM *mitem)
{
	SCORE *current = mitem->score_history;
	if (!current) return -1;
	while (current)
	{
		fwrite(&current->score, sizeof(current->score), 1, fp);
		fwrite(&current->date, sizeof(current->date), 1, fp);
		current = current->next;
	}
	return 0;
}
//Exports history into CSV file from a specific menu item.
int ExportScore(FILE* fp, SCORE *scores)
{
	if (!scores) return -1;
	struct tm *date;
	fprintf(fp, "Num,Date,Score\n");
	int i = 1;
	char str_date[20];
	for (SCORE *current = scores; current; current = current->next)
	{
		date = localtime(&current->date);
		strftime(str_date, 20, "%F", date);
		fprintf(fp,"%d,%s,%d\n", i, str_date, current->score);
		i++;
	}
	return 0;
}

int main()
{
	SCORE score1;
	score1.date = time(NULL);
	score1.score = 10;
	SCORE score2;
	score1.next = &score2;
	score2.date = time(NULL);
	score2.score = 12;
	score2.next = NULL;
	FILE *scorefile = fopen("score.csv", "w");
	ExportScore(scorefile, &score1);
	fclose(scorefile);
	FILE *gameinfo = fopen("gameinfo.txt", "r");
	LoadGames(gameinfo, NULL);
	SetConsoleTitle(L"Uvod Prototip");
	HANDLE conOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	cls(conOutHandle);
	BYTE lg = 0x07;
	BYTE dg = 0x08;
	short color = 0x07;
	MENU_ITEM mitems[4];
	strncpy_s(mitems[0].title,16, "Pogodi Broj", 15);
	strncpy_s(mitems[1].title,16 ,"Pitanjca", 15);
	strncpy_s(mitems[2].title,16 ,"Loto<:)", 15);
	strncpy_s(mitems[3].title,16 ,"Fortnite", 15);

	for (int i = 0; i < 4; i++)
	{
		strncpy_s(mitems[i].description, 1024, "Lorem Ipsum oh s***.", 1023);
	}
	strncpy_s(mitems[2].description, 1024, "Somebody once told me the world is gonna roll me "
		"I ain't the sharpest tool in the shed" 
		"She was looking kind of dumb with her finger and her thumb" 
		"In the shape of an \"L\" on her forehead", 1023);
	int c;
	int conWidth = GetBufferWidth(conOutHandle);
	int menuIndex = 0;
	HANDLE gameDLL = LoadLibraryA("game.dll");
	printf("%d\n", GetLastError());
	int(*gameptr)(int) = (int(*)(int))GetProcAddress(gameDLL, "game");
	printf("%d", GetLastError());
	gameptr(10);
	while (TRUE)
	{
		//cls(conOutHandle);
		SetCursorPosition(conOutHandle, 0, 0);
		SetConsoleTextAttribute(conOutHandle, color = (color & 0xFF00)|0x07);
		printf("%s\t%s", "Pozdrav John Rambo", "Poeni:10");
		
		c = _getch();				//TODO: use w version of this function in later versions. _getch is broken on april update.
		//printf("%d %c\n", c,c);
		SetCursorPosition(conOutHandle, 0, 2);
		color = GetConsoleColor(conOutHandle);
	//	color &= 0xFF00;
	//	SetConsoleTextAttribute(conOutHandle, color |= 0x78);
		if (c == 0 || c == 224)
		{
			c = _getch();
			switch (c)
			{
			case 75: 
				menuIndex = (unsigned)(menuIndex - 1) % 4;
				break;
			case 77:
				menuIndex = (menuIndex + 1) % 4;
				break;
			}
		}
		for (int i = 0; i < 4; i++) //4 is the current number of menu items. Change later.
		{
			if (i == menuIndex) SetConsoleTextAttribute(conOutHandle, color ^= COMMON_LVB_REVERSE_VIDEO);
			printf("%-15s", mitems[i].title);
			if (i == menuIndex) SetConsoleTextAttribute(conOutHandle, color ^= COMMON_LVB_REVERSE_VIDEO);
		}
		SetConsoleTextAttribute(conOutHandle, color = (color & 0xFF00) | 0x07);
		printf("%c", 178);
		SetCursorPosition(conOutHandle, 0, 3);
		color &= 0xFF00;
		SetConsoleTextAttribute(conOutHandle, color |= 0x88);
		printf("%c", 178);
		printf("%*c", GetBufferWidth(conOutHandle)*10-1, 178);
		SetConsoleTextAttribute(conOutHandle, color = (color & 0xFF00) | 0x87);
		SetCursorPosition(conOutHandle, 0, 3);
		printf("%s", mitems[menuIndex].description);

	}

    return 0;
}

