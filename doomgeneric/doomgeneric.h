#ifndef DOOM_GENERIC
#define DOOM_GENERIC

#include <stdlib.h>
#include <stdint.h>

#include "d_player.h"

#define DOOMGENERIC_RESX 320 //640
#define DOOMGENERIC_RESY 200 //400

extern uint32_t* DG_ScreenBuffer;
extern player_t* DG_Player;

extern boolean DG_PlayerInRampageMode;

extern boolean DG_HintMessageActive;
extern char DG_HintMessage[39];

extern boolean DG_MenuMessageActive;
extern char DG_MenuMessageString[160];

extern boolean DG_MenuActive;
extern short DG_CurrentMenu;
extern short DG_MenuItemOn;
extern short DG_MenuItemsCount;
extern char* DG_MenuEntriesNames[10];
extern short DG_MenuEntriesStati[10];

enum DG_MENUS {
  DOOM_MENU_MAINDEF,
  DOOM_MENU_MAINMENU,
  DOOM_MENU_NEWDEF,
  DOOM_MENU_NEWGAMEMENU,
  DOOM_MENU_EPIDEF,
  DOOM_MENU_EPISODEMENU,
  DOOM_MENU_OPTIONSDEF,
  DOOM_MENU_OPTIONSMENU,
  DOOM_MENU_READDEF1,
  DOOM_MENU_READDEF2,
  DOOM_MENU_SOUNDDEF,
  DOOM_MENU_LOADDEF,
  DOOM_MENU_SAVEDDEF,
}; 

void DG_Init();
void DG_DrawFrame();
void DG_SleepMs(uint32_t ms);
uint32_t DG_GetTicksMs();
int DG_GetKey(int* pressed, unsigned char* key);
void DG_SetWindowTitle(const char * title);

#endif //DOOM_GENERIC
