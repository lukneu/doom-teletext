#ifndef TT_PAGEGENERATOR_H
#define TT_PAGEGENERATOR_H

#define ROWS 24
#define COLUMNS 42

#include <stdint.h>

void TT_InitPage(uint8_t page[ROWS][COLUMNS]);

void TT_SetAmmunation(uint8_t page[ROWS][COLUMNS], int value);

#endif