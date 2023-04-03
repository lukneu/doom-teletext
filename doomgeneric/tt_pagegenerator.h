#ifndef TT_PAGEGENERATOR_H
#define TT_PAGEGENERATOR_H

#define TT_ROWS 24
#define TT_COLUMNS 42

#define TT_FRAMEBUFFER_ROWS 17 //rows needed to display actual game frame
#define TT_FRAMEBUFFER_COLUMNS 40 //columns needed to hold actual game frame

#define TT_STATUSBAR_ROWS 4 //rows needed to hold statusbar
#define TT_STATUSBAR_COLUMNS 40 //columns needed to hold statusbar


#include <stdint.h>
#include <stdbool.h>

struct tt_menu_slider_values
{
    int sfxVol;
    int musicVol;
    int screenSize;
    int mouseSen;
};


void TT_InitPage(uint8_t page[TT_ROWS][TT_COLUMNS]);

void TT_InitStatusbar(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS]);

//debug info
void TT_ShowDebugInfo(uint8_t page[TT_ROWS][TT_COLUMNS], uint8_t fpsValue, uint8_t graphicMode);

void TT_HideDebugInfo(uint8_t page[TT_ROWS][TT_COLUMNS]);

void TT_WriteHintMessage(uint8_t page[TT_ROWS][TT_COLUMNS], uint8_t line, char* string);

void TT_ClearLine(uint8_t page[TT_ROWS][TT_COLUMNS], uint8_t line);

//manipulate statusbar
void TT_SetActiveAmmunition(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS], int value);

void TT_SetActiveAmmunitionToInfinite(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS]);

void TT_SetArmor(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS], int value);

void TT_SetHealth(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS], int value, bool rampageMode);

void TT_SetAvailableWeapons(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS],
                            bool w2, bool w3, bool w4, bool w5, bool w6, bool w7);

void TT_SetCards(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS],
                 bool bluecard,
                 bool yellowcard,
                 bool redcard);

void TT_SetAmmunitionValues(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS],
                            int bull_avail, int bull_max,
                            int shel_avail, int shel_max,
                            int rckt_avail, int rckt_max,
                            int cell_avail, int cell_max);

void TT_InsertStatusbar(uint8_t page[TT_ROWS][TT_COLUMNS], uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS]);

//manipulate rendering area
void TT_RenderInMosaicBlackWhite(uint32_t* DG_ScreenBuffer,
                                 uint8_t rendering[TT_FRAMEBUFFER_ROWS][TT_FRAMEBUFFER_COLUMNS],
                                 bool separate_graphics);

void TT_InsertGameRendering(uint8_t page[TT_ROWS][TT_COLUMNS],
                            uint8_t rendering[TT_FRAMEBUFFER_ROWS][TT_FRAMEBUFFER_COLUMNS]);

void TT_InsertMenuMessage(uint8_t rendering[TT_FRAMEBUFFER_ROWS][TT_FRAMEBUFFER_COLUMNS], char* msg);

void TT_OverlayMenu(uint8_t rendering[TT_FRAMEBUFFER_ROWS][TT_FRAMEBUFFER_COLUMNS],
                    short itemsCount, char** itemsNames, short activeIndex, short* itemsStati,
                    struct tt_menu_slider_values sliderValues);

#endif