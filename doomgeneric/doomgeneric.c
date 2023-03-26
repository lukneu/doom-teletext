#include "doomgeneric.h"

uint32_t* DG_ScreenBuffer = 0;

player_t* DG_Player = NULL;

char DG_HintMessage[39];
boolean DG_NewMessageAvailable = false;

boolean DG_MenuMessageActive = false;
char DG_MenuMessageString[160];

boolean DG_MenuActive = false;
short DG_MenuItemOn = 0;
short DG_MenuItemsCount = 0;
char DG_DoomMenu = 0;

void dg_Create()
{
	DG_ScreenBuffer = malloc(DOOMGENERIC_RESX * DOOMGENERIC_RESY * 4);

	DG_Init();
}

