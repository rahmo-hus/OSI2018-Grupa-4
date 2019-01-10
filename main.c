#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <Windows.h>
#include <time.h>
#include "mmkeylib.h"
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
	char *dllName;
	char *procName;
	int(*game)();
	SCORE* score_history;
	int unlocked;
	uint8_t key[16];
	int unlock_price;
	int play_price;
	int disabled;
	int id;
	int kd;
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
int GetXPos(HANDLE hConsoleOutput)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(hConsoleOutput, &info);
	return info.dwCursorPosition.X;
}
int GetYPos(HANDLE hConsoleOutput)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(hConsoleOutput, &info);
	return info.dwCursorPosition.Y;
}
int GetConsoleColor(HANDLE hConsoleOutput)
{
	CONSOLE_SCREEN_BUFFER_INFO conBufInfo;
	GetConsoleScreenBufferInfo(hConsoleOutput, &conBufInfo);
	return conBufInfo.wAttributes;
}

//Loads game information from games.config into an array of menu items and loads game DLLs and returns number of games loaded
int LoadGames(FILE* fp, MENU_ITEM **menu_out)
{
	int dummy_item_count = 0;
	MENU_ITEM menu[4];
	char buffer[1024];
	int count = 0;
	int len;
#define BREAKIFEOF() if (feof(fp)){count = i;break;}
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
		char* dllName = malloc(sizeof(char)*(strlen(buffer) + 1));
		strncpy(dllName, buffer, strlen(buffer) + 1);
		menu[i].dllName = dllName;
		HANDLE gameDLL = LoadLibraryA(buffer);
		if (!gameDLL) { count = i; break; }
		fgets(buffer, 1024, fp);
		buffer[strcspn(buffer, "\n")] = '\0';
		char* procName = malloc(sizeof(char)*(strlen(buffer) + 1));
		strncpy(procName, buffer, strlen(buffer) + 1);
		menu[i].procName = procName;
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
		sprintf(buffer, "%s_score.scr", menu[i].title);
		FILE* scr = fopen(buffer, "rb");
		menu[i].score_history = NULL;
		if (scr)
		{
			_LoadHistory(scr, &menu[i]);
			fclose(scr);
		}
		count = i;
		fgets(buffer, 1024, fp);
		buffer[strcspn(buffer, "\n")] = '\0';
		menu[i].disabled = atoi(buffer);
		fgets(buffer, 1024, fp);
		buffer[strcspn(buffer, "\n")] = '\0';
		menu[i].id = atoi(buffer);
		fgets(buffer, 1024, fp);
		buffer[strcspn(buffer, "\n")] = '\0';
		menu[i].kd = atoi(buffer);
	}
	*menu_out = malloc(sizeof(MENU_ITEM)*(count + 1));
	for (int i = 0; i < count + 1; i++)
	{
		menu_out[0][i] = menu[i];
	}
	return count + 1;
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
		fprintf(fp, "%d,%s,%d\n", i, str_date, current->score);
		i++;
	}
	return 0;
}
//Loads user data. Returns -1 if no user file is present. Usually means the game was run for the first time.
int LoadUser(FILE* fp, USER **user)
{
	if (!fp)
		return -1;
	*user = malloc(sizeof(USER));
	USER *u = *user;
	fread(u->name, sizeof(char), 16, fp);
	fread(&u->points, sizeof(int32_t), 1, fp);
	return 0;
}
int SaveUser(FILE* fp, USER *user)
{
	if (!fp)
		return -1;
	rewind(fp);
	fwrite(user->name, sizeof(char), 16, fp);
	fwrite(&user->points, sizeof(int32_t), 1, fp);
	return 0;
}
void AddHistory(MENU_ITEM *mitem, SCORE score)
{
	SCORE* new_score = malloc(sizeof(SCORE));
	score.next = NULL;
	*new_score = score;
	SCORE* cur = mitem->score_history;
	if (cur)
	{
		for (; cur->next; cur = cur->next);
		cur->next = new_score;
	}
	else
	{
		mitem->score_history = new_score;
	}
}
SCORE* TopTenFromHistory(SCORE* history)
{
	if (!history) return NULL;
	SCORE* top10[10];
	for (int i = 0; i < 10; i++)
		top10[i] = NULL;
	int size = 0;
	int first_pass = 1;
	for (int i = 0; i < 10; i++)
	{
		for (SCORE* cur = history; cur; cur = cur->next)
		{
			if (first_pass)
				size++;
			if (top10[i] == NULL || top10[i]->score < cur->score)
			{
				int present = 0;
				for (int j = 0; j <= i; j++)
					if (cur == top10[j])
						present = 1;
				if (!present)
					top10[i] = cur;
			}
		}
		first_pass = 0;
	}
	MENU_ITEM dummy;
	dummy.score_history = NULL;
	if (size > 10)size = 10;
	for (int i = 0; i < size; i++)
	{
		AddHistory(&dummy, top10[i][0]);
	}
	return dummy.score_history;
}
int SaveGames(const char* save, MENU_ITEM *menu, int count)
{
	FILE* temp = fopen("temp.tmp", "w");
	for (int i = 0; i < count; i++)
	{
		fprintf(temp, "%s\n%s\n%s\n%s\n%d\n%s\n%d\n%d\n%d\n%d\n%d\n",
			menu[i].title,
			menu[i].description,
			menu[i].dllName,
			menu[i].procName,
			menu[i].unlocked,
			menu[i].key,
			menu[i].unlock_price,
			menu[i].play_price,
			menu[i].disabled,
			menu[i].id,
			menu[i].kd
		);
	}
	if (remove(save)!=0)
	{
		perror("Error deleting file:");
	}
	else
	{ 
//		puts("Successfully deleted file");
	}
	fclose(temp);
	if (rename("temp.tmp", save) != 0)
	{
		perror("Error renaming file:");
	}
	fclose(temp);
	return 0;
}
void GenerateKeys(MENU_ITEM *menu, int count)
{
	printf("\nHere are your game keys. Write them down!\n");
	for (int i = 0; i < count; i++)
	{
		char* key = requestKey(menu[i].id, menu[i].kd);
		printf("%-15s key:  %.16s\n",menu[i].title, key);
		free(key);
	}
	printf("\nPress any key when you're ready to continue.");
	_getch();
}
#define GAME_INFO_FILENAME "gameinfo.txt"
//Save all data
#define SAVE_ALL()	{\
SaveUser(userinfo, user);\
SaveGames(GAME_INFO_FILENAME, mitems, count); \
	for (int i = 0; i < count; i++) \
	{ \
		char buffer[128]; \
		sprintf(buffer, "%s_score.scr", mitems[i].title); \
		FILE * scr = fopen(buffer, "wb"); \
		SaveHistory(scr, &mitems[i]); \
		fclose(scr); \
	}\
} 
//Draw the description box
#define DESC_BOX(){SetConsoleTextAttribute(conOutHandle, color = (color & 0xFF00) | 0x07);\
		printf("%c", 178);\
		SetCursorPosition(conOutHandle, 0, 3);\
		color &= 0xFF00;\
		SetConsoleTextAttribute(conOutHandle, color |= 0x88);\
		printf("%c", 178);\
		printf("%*c", GetBufferWidth(conOutHandle) * 11 - 1, 178);\
		SetConsoleTextAttribute(conOutHandle, color = (color & 0xFF00) | 0x07);\
	}
int main()
{
	//TODO: Utilize these functions to make a menu.
	SetConsoleTitle("Lucky Rouza");
	MENU_ITEM  *mitems;
	FILE *gameinfo = fopen("gameinfo.txt", "r");
	int count = LoadGames(gameinfo, &mitems);
	fclose(gameinfo);
	USER *user;
	FILE *userinfo = fopen("USER_DAT.bin", "rb+");
	if (LoadUser(userinfo, &user) == -1)
	{
		user = malloc(sizeof(USER));
		memset(user->name, 0, 16);
		printf("Greetings.\nWelcome to Lucky Rouza. To continue, please enter your username.\n");
		scanf("%15s", &user->name);
		int c;
		while ((c = fgetc(stdin)) != '\n' && c != EOF); /* Flush stdin */
		user->points = 10;
		fclose(fopen("USER_DAT.bin", "wb"));
		//Erase any potential old game data.
		for (int i = 0; i < count; i++)
		{

			memcpy(mitems[i].key, "0000000000000000", 16);
			mitems[i].unlocked = 0;
			mitems[i].disabled = 0;
			char buffer[128];
			sprintf(buffer, "%s_score.scr", mitems[i].title);
			remove(buffer);
			for (SCORE* cur = mitems[i].score_history; cur; )
			{
				SCORE* temp = cur;
				cur = cur->next;
				free(temp);
			}
			mitems[i].score_history = NULL;

		}
		printf("Welcome %s", user->name);
		GenerateKeys(mitems, count);
		SaveGames(GAME_INFO_FILENAME, mitems, count);
	}
	else
	{
		fclose(userinfo);
	}

	userinfo = fopen("USER_DAT.bin", "rb+");
	SaveUser(userinfo, user);

	SaveGames(GAME_INFO_FILENAME, mitems, count);
	HANDLE conOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	cls(conOutHandle);
	short color = 0x07;
	int c;
	int menuIndex = 0;
	int32_t temp_score = 0;
	int show_stats = 0;
	FILE* sc_file;
	char sc_name[20];
	SCORE* top_scores = NULL;
	while (TRUE)
	{
		for (int i = 0; i < count; i++)
		{
			if (mitems[i].unlocked && !verifyKey(mitems[i].key))
				mitems[i].disabled = 1;
		}
		SetCursorPosition(conOutHandle, 0, 0);
		SetConsoleTextAttribute(conOutHandle, color = (color & 0xFF00) | 0x08);
		//printf("%s\t%s", "Pozdrav John Rambo", "Poeni:10");
		printf("Welcome, %s.\tPoints:%d", user->name, user->points);
		if (!mitems[menuIndex].disabled)
		{
			printf("\nGame:%s\tKey %s", mitems[menuIndex].unlocked ? "Unlocked" : "Locked", mitems[menuIndex].unlocked ? "" : "required");
			if (mitems[menuIndex].unlocked)
			{
				if (!(verifyKey(mitems[menuIndex].key)&& getKeyGameID(mitems[menuIndex].key) == mitems[menuIndex].id))
				{
					mitems[menuIndex].unlocked = 0;
					for (int i = 0; i < 16; i++)
						mitems[menuIndex].key[i] = '0';
				}
				if (getKeyDuration(mitems[menuIndex].key) != -1)
				{
					char str_date[64];
					time_t key_time = getKeyExpirationTime(mitems[menuIndex].key);
					struct tm *date = localtime(&key_time);
					strftime(str_date, 64, "%c", date);
					printf("expires on %s", str_date);
				}
				else
				{
					printf("never expires");
				}
			}
		}
		else
		{
			printf("\nGame:Disabled\n");
		}
		
		SetCursorPosition(conOutHandle, 0, 2);
		color = GetConsoleColor(conOutHandle);
		//Print out menu game buttons
		for (int i = 0; i < count; i++)
		{
			if (i == menuIndex) SetConsoleTextAttribute(conOutHandle, color ^= COMMON_LVB_REVERSE_VIDEO);
			printf("%-15s", mitems[i].title);
			if (i == menuIndex) SetConsoleTextAttribute(conOutHandle, color ^= COMMON_LVB_REVERSE_VIDEO);
		}

		DESC_BOX();

		SetConsoleTextAttribute(conOutHandle, color = (color & 0xFF00) | 0x37);
		printf(" E(x)it | %s | S(a)ve stats | (U)ninstall | (R)eset all ", !show_stats ? "(S)how stats" : "(S)how desc.");
		SetConsoleTextAttribute(conOutHandle, color = (color & 0xFF00) | 0x87);
		SetCursorPosition(conOutHandle, 0, 3);
		if (show_stats)
		{
			int i = 1;
			char str_date[20];
			printf("Num\tDate\t\tScore\n");
			top_scores = TopTenFromHistory(mitems[menuIndex].score_history);
			for (SCORE *cur = top_scores; cur; cur = cur->next)
			{
				struct tm *date = localtime(&cur->date);
				strftime(str_date, 20, "%F", date);
				printf("%d\t%s\t%d\n", i++, str_date, cur->score);
			}
			for (SCORE* cur = top_scores; cur; )
			{
				SCORE* tmp = cur;
				cur = cur->next;
				free(tmp);
			}
			top_scores = NULL;

		}
		else
		{
			printf("%s", mitems[menuIndex].description);
		}
		c = _getch();				//TODO: use w version of this function in later versions. _getch is broken on april update.
		switch (c)
		{
		case 'x':

			goto exit;
			break;
		case 's':
			show_stats = !show_stats;
			break;
		case 'a':
			top_scores = TopTenFromHistory(mitems[menuIndex].score_history);
			strncpy(sc_name, mitems[menuIndex].title, 16);
			strcat(sc_name, ".csv");
			sc_file = fopen(sc_name, "w");
			ExportScore(sc_file, top_scores);
			fclose(sc_file);
			for (SCORE* cur = top_scores; cur; )
			{
				SCORE* tmp = cur;
				cur = cur->next;
				free(tmp);
			}
			top_scores = NULL;
			break;
		case 'u':
			if (mitems[menuIndex].disabled) break;
			DESC_BOX();
			SetCursorPosition(conOutHandle, GetBufferWidth(conOutHandle) / 2 - strlen("Are you sure? This will permanently disable this game until reinstall") / 2, 9);
			printf("Are you sure? This will permanently disable this game until reinstall. (y/(any no))");
			if (_getch() == 'y')
			{
				DESC_BOX();
				SetCursorPosition(conOutHandle, GetBufferWidth(conOutHandle) / 2 - strlen("Game Disabled. Reset to re-enable") / 2, 9);
				printf("Game Disabled. Reset to re-enable");
				mitems[menuIndex].disabled = 1;
				_getch();
			}
			break;
		case 'r':
			cls(conOutHandle);
			printf("Are you sure? (y/(any no))");
			if(_getch()=='y')
				goto nuke;
		}
		if (c == 0 || c == 224) // Non AlphaNum key pressed
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
		if (c == 13 && !mitems[menuIndex].disabled) //Enter
		{
//Todo:: Add entering and exiting the game
			if (verifyKey(mitems[menuIndex].key) > 0)
			{

			}
			else
			{
				DESC_BOX();
				char new_key[20] = { 0 };
				int key_index = 0;
				//0000-0000-0000-0000
				SetCursorPosition(conOutHandle, GetBufferWidth(conOutHandle) / 2 - strlen("Input Game Key") / 2, 9);
				printf("Input Game Key");
				SetCursorPosition(conOutHandle, GetBufferWidth(conOutHandle) / 2 - 9, 8);
				for (int i = 0; i < 3; i++)
				{
					printf("    -");
				}
				printf("    ");
				SetCursorPosition(conOutHandle, GetBufferWidth(conOutHandle) / 2 - 9, 8);
				
				int leave_key_loop = 0;
				while (!leave_key_loop)
				{
					
					c = _getch();
					switch (c)
					{
					case 13: //Enter
						if (verifyKey(new_key)&&getKeyGameID(new_key) == mitems[menuIndex].id)
						{
							const char* success_message = "Key appears correct. Press any key to proceed";
							if (mitems[menuIndex].unlock_price > user->points && mitems[menuIndex].unlock_price>0)
							{
								success_message = "You do not have enough points to unlock this game. Try again later.";
							}
							else
							{
								user->points -= mitems[menuIndex].unlock_price;
								for (int i = 0; i < 16; i++)
									mitems[menuIndex].key[i] = new_key[i];
								mitems[menuIndex].unlocked = 1;
							}
							
							SetCursorPosition(conOutHandle, GetBufferWidth(conOutHandle) / 2 - strlen(success_message) / 2, 9);
							printf("%s", success_message);
							leave_key_loop = 1;
							_getch();

						}
						else
						{
							while (!leave_key_loop)
							{
								const char *invalid_message = "Invalid key. Try Again? (y/n)";
								SetCursorPosition(conOutHandle, GetBufferWidth(conOutHandle) / 2 - strlen(invalid_message) / 2, 9);
								printf("%s", invalid_message);
								c = _getch();
								if (c == 'y')
								{

									key_index = 0;
									DESC_BOX();
									SetCursorPosition(conOutHandle, GetBufferWidth(conOutHandle) / 2 - strlen("Input Game Key") / 2, 9);
									printf("Input Game Key");
									SetCursorPosition(conOutHandle, GetBufferWidth(conOutHandle) / 2 - 9, 8);
									for (int i = 0; i < 3; i++)
									{
										printf("    -");
									}
									printf("    ");
									SetCursorPosition(conOutHandle, GetBufferWidth(conOutHandle) / 2 - 9, 8);
									break;
								}
								else if (c == 'n')
									leave_key_loop = 1;
							}
						}
						break;
					case 'x':
						leave_key_loop = 1;
						break;
					default:
						if (c >= '0'&&c <= '9')
						{
							if (key_index < 16)
							{
								new_key[key_index++] = c;
								printf("%c", c);
								if (key_index % 4 == 0)
									SetCursorPosition(conOutHandle, GetXPos(conOutHandle) + 1, GetYPos(conOutHandle));

							}
						}
						if (c == 8)
						{
							// Backspace
							if (key_index > 0)
							{

								if (key_index % 4 == 0)
									SetCursorPosition(conOutHandle, GetXPos(conOutHandle) - 1, GetYPos(conOutHandle));
								SetCursorPosition(conOutHandle, GetXPos(conOutHandle) - 1, GetYPos(conOutHandle));
								printf(" ");
								SetCursorPosition(conOutHandle, GetXPos(conOutHandle) - 1, GetYPos(conOutHandle));
								new_key[key_index--] = '\0';
							}
						}
					}
				}
			}
		}
		else if(c == 13 && mitems[menuIndex].disabled)
		{
			DESC_BOX();
			SetCursorPosition(conOutHandle, GetBufferWidth(conOutHandle) / 2 - strlen("Game Disabled. Reset to re-enable") / 2, 9);
			printf("Game Disabled. Reset to re-enable");
		}
		SetConsoleTextAttribute(conOutHandle, color = (color & 0xFF00) | 0x07);
		cls(conOutHandle);
	}
exit:
	SAVE_ALL();
	fclose(userinfo);
	return 0;

nuke: // Erase all data
	cls(conOutHandle);
	_fcloseall();
	int fail = 0;
	fail =remove("USER_DAT.bin");
	remove("Playlist.dat");
	for (int i = 0; i < count; i++)
	{
		
		memcpy(mitems[i].key, "0000000000000000", 16);
		mitems[i].unlocked = 0;
		mitems[i].disabled = 0;
		char buffer[128]; 
		sprintf(buffer, "%s_score.scr", mitems[i].title);
		fail = remove(buffer)?fail:1;
	}
	SaveGames(GAME_INFO_FILENAME, mitems, count);
	printf("Bye Bye");
	Sleep(1000);
	return 0;
}

}
