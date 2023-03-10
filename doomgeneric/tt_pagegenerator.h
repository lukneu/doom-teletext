#ifndef TT_PAGEGENERATOR_H
#define TT_PAGEGENERATOR_H

#define ROWS 24
#define COLUMNS 42

#include <stdint.h>
#include <stdbool.h>

void TT_InitPage(uint8_t page[ROWS][COLUMNS]);

void TT_SetActiveAmmunition(uint8_t page[ROWS][COLUMNS], int value);
void TT_SetActiveAmmunitionToInfinite(uint8_t page[ROWS][COLUMNS]);
void TT_SetArmor(uint8_t page[ROWS][COLUMNS], int value);
void TT_SetHealth(uint8_t page[ROWS][COLUMNS], int value);
void TT_SetAvailableWeapons(uint8_t page[ROWS][COLUMNS], bool w2, bool w3, bool w4, bool w5, bool w6, bool w7);
void TT_SetCards(uint8_t page[ROWS][COLUMNS], bool bluecard, bool yellowcard, bool redcard);
void TT_SetAmmunitionValues(uint8_t page[ROWS][COLUMNS],
                            int bull_avail, int bull_max,
                            int shel_avail, int shel_max,
                            int rckt_avail, int rckt_max,
                            int cell_avail, int cell_max);

#endif