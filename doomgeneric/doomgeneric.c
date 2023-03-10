#include "doomgeneric.h"

uint32_t* DG_ScreenBuffer = 0;

player_t* DG_Player = NULL;


void dg_Create()
{
	DG_ScreenBuffer = malloc(DOOMGENERIC_RESX * DOOMGENERIC_RESY * 4);

	DG_Init();
}

