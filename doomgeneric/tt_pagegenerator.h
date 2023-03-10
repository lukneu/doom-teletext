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

void TT_InsertStatusbar(uint8_t page[TT_ROWS][TT_COLUMNS], uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS]);

void TT_InitPage(uint8_t page[TT_ROWS][TT_COLUMNS]);
void TT_InitStatusbar(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS]);

//statusbar
void TT_SetActiveAmmunition(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS], int value);
void TT_SetActiveAmmunitionToInfinite(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS]);
void TT_SetArmor(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS], int value);
void TT_SetHealth(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS], int value);
void TT_SetAvailableWeapons(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS], bool w2, bool w3, bool w4, bool w5, bool w6, bool w7);
void TT_SetCards(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS], bool bluecard, bool yellowcard, bool redcard);
void TT_SetAmmunitionValues(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS],
                            int bull_avail, int bull_max,
                            int shel_avail, int shel_max,
                            int rckt_avail, int rckt_max,
                            int cell_avail, int cell_max);

#endif