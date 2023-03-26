#ifndef DOOM_GENERIC
#define DOOM_GENERIC

#include <stdlib.h>
#include <stdint.h>

#include "d_player.h"

#define DOOMGENERIC_RESX 320 //640
#define DOOMGENERIC_RESY 200 //400

extern uint32_t* DG_ScreenBuffer;
extern player_t* DG_Player;
extern char DG_HintMessage[39];
extern boolean DG_NewMessageAvailable;

extern boolean DG_MenuMessageActive;
extern char DG_MenuMessageString[160];

extern boolean DG_MenuActive;
extern short DG_MenuItemOn;
extern short DG_MenuItemsCount;
extern char DG_DoomMenu;



void DG_Init();
void DG_DrawFrame();
void DG_SleepMs(uint32_t ms);
uint32_t DG_GetTicksMs();
int DG_GetKey(int* pressed, unsigned char* key);
void DG_SetWindowTitle(const char * title);

#endif //DOOM_GENERIC
