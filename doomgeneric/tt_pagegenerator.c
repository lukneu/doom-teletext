#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "tt_pagegenerator.h"
#include "tt_encoding.h"
#include "tt_charset.h"
#include "tt_sprites.h"

#define PAGE_MAG 1
#define PAGE_TENS 0
#define PAGE_UNITS 0

//target array is of size 24*42, but first two columns of array is not to be manipulated because it holds MPAG bytes
//watch out: startline and startRow start by 1, not 0. (except for HEADER, where line = 0)
//source array does not have to have correct parity bit, parity bit is set by this function
void InsertIntoPage(uint8_t target[ROWS][COLUMNS], int startRow, int startColumn, int height, int width, uint8_t source[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            target[startRow + i][startColumn + 2 + j - 1] = Parity(source[i][j]);
        }
    }
}

void TT_InitPage(uint8_t page[ROWS][COLUMNS])
{
    for (uint8_t i = 0; i < ROWS; i++)
    {
        for (uint8_t j = 0; j < COLUMNS; j++)
        {
            page[i][j] = 0x20; //space
        }
    }

    uint8_t mpag_bytes[2];

    EncodeMpag(PAGE_MAG, (10 * PAGE_TENS) + PAGE_UNITS, mpag_bytes);

    page[0][0] = mpag_bytes[0];
    page[0][1] = mpag_bytes[1];

    //https://github.com/losso3000/420-years-of-teletext/blob/main/notes/packet-format.md

    uint8_t headerConfigBytes[1][8] = { { Hamming84(PAGE_UNITS), Hamming84(PAGE_TENS), 0x15, 0x15, 0x15, 0x15, 0x15, 0x15 } };
    InsertIntoPage(page, 0, 1, 1, 8, headerConfigBytes);
   
    uint8_t headerContextBytes[1][32] = { { TTEXT_ALPHA_CYAN, 'g', 'i', 't', 'h', 'u', 'b', '.', 'c', 'o', 'm', '/', 'l', 'u', 'k', 'n', 'e', 'u', '/', 'd', 'o', 'o', 'm', '-', 't', 'e', 'l', 'e', 't', 'e', 'x', 't' } };
    InsertIntoPage(page, 0, 9, 1, 32, headerContextBytes);

    //set mpag bytes for each row
    for (size_t row = 1; row < ROWS; row++)
    {
        EncodeMpag(PAGE_MAG, row, mpag_bytes);
        page[row][0] = mpag_bytes[0];
        page[row][1] = mpag_bytes[1];
    }

    uint8_t helloWorldText[1][12] = { {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!' } };
    InsertIntoPage(page, 2, 1, 1, 12, helloWorldText);

    //init statusbar placeholder
    InsertIntoPage(page, 20, 1, 4, 40, sprite_statusbar);

    //for easier debugging:
    for (uint8_t j = 2; j < COLUMNS; j++)
    {
        page[19][j] = Parity('0' + ((j - 1) % 10));
    }

}

void WriteThreeDigitNumber(uint8_t page[ROWS][COLUMNS], int value, uint8_t start_row, uint8_t start_col)
{
    uint8_t digit_1 = value / 100;
    uint8_t digit_2 = (value % 100) / 10;
    uint8_t digit_3 = value % 10;

    InsertIntoPage(page, start_row, start_col, 2, 2, sprite_char_array[digit_1]);
    InsertIntoPage(page, start_row, start_col + 2, 2, 2, sprite_char_array[digit_2]);
    InsertIntoPage(page, start_row, start_col + 4, 2, 2, sprite_char_array[digit_3]);   
}

void TT_SetActiveAmmunition(uint8_t page[ROWS][COLUMNS], int value)
{
    WriteThreeDigitNumber(page, value, 20, 2);
}

void TT_SetActiveAmmunitionToInfinite(uint8_t page[ROWS][COLUMNS])
{
    InsertIntoPage(page, 20, 2, 2, 6, sprite_text_inf);
}

void TT_SetHealth(uint8_t page[ROWS][COLUMNS], int value)
{
    WriteThreeDigitNumber(page, value, 20, 9);
}

void TT_SetArmor(uint8_t page[ROWS][COLUMNS], int value)
{
    WriteThreeDigitNumber(page, value, 20, 31);
}

void TT_SetAvailableWeapons(uint8_t page[ROWS][COLUMNS], bool w2, bool w3, bool w4, bool w5, bool w6, bool w7)
{
    page[20][18] = w2 ? Parity(TTEXT_ALPHA_YELLOW) : Parity(TTEXT_ALPHA_WHITE);
    page[20][20] = w3 ? Parity(TTEXT_ALPHA_YELLOW) : Parity(TTEXT_ALPHA_WHITE);
    page[20][22] = w4 ? Parity(TTEXT_ALPHA_YELLOW) : Parity(TTEXT_ALPHA_WHITE);

    page[21][18] = w5 ? Parity(TTEXT_ALPHA_YELLOW) : Parity(TTEXT_ALPHA_WHITE);
    page[21][20] = w6 ? Parity(TTEXT_ALPHA_YELLOW) : Parity(TTEXT_ALPHA_WHITE);
    page[21][22] = w7 ? Parity(TTEXT_ALPHA_YELLOW) : Parity(TTEXT_ALPHA_WHITE);
}

void TT_SetCards(uint8_t page[ROWS][COLUMNS], bool bluecard, bool yellowcard, bool redcard)
{
    page[20][40] = bluecard ? Parity(TTEXT_GRAPHIC_BLUE) : Parity(TTEXT_GRAPHIC_WHITE);
    page[21][40] = yellowcard ? Parity(TTEXT_GRAPHIC_YELLOW) : Parity(TTEXT_GRAPHIC_WHITE);
    page[22][40] = redcard ? Parity(TTEXT_GRAPHIC_RED) : Parity(TTEXT_GRAPHIC_WHITE);
}

void TT_SetAmmunitionValues(uint8_t page[ROWS][COLUMNS],
                            int bull_avail, int bull_max,
                            int shel_avail, int shel_max,
                            int rckt_avail, int rckt_max,
                            int cell_avail, int cell_max)
{
    //BULL
    page[23][5] = Parity('0' + (bull_avail / 100));
    page[23][6] = Parity('0' + ((bull_avail % 100) / 10));
    page[23][7] = Parity('0' + (bull_avail % 10));

    page[23][9] = Parity('0' + (bull_max / 100));
    page[23][10] = Parity('0' + ((bull_max % 100) / 10));
    page[23][11] = Parity('0' + (bull_max % 10));

    //SHEL
    page[23][15] = Parity('0' + (shel_avail / 100));
    page[23][16] = Parity('0' + ((shel_avail % 100) / 10));
    page[23][17] = Parity('0' + (shel_avail % 10));

    page[23][19] = Parity('0' + (shel_max / 100));
    page[23][20] = Parity('0' + ((shel_max % 100) / 10));
    page[23][21] = Parity('0' + (shel_max % 10));

    //RCKT
    page[23][25] = Parity('0' + (rckt_avail / 100));
    page[23][26] = Parity('0' + ((rckt_avail % 100) / 10));
    page[23][27] = Parity('0' + (rckt_avail % 10));

    page[23][29] = Parity('0' + (rckt_max / 100));
    page[23][30] = Parity('0' + ((rckt_max % 100) / 10));
    page[23][31] = Parity('0' + (rckt_max % 10));

    //CELL
    page[23][35] = Parity('0' + (cell_avail / 100));
    page[23][36] = Parity('0' + ((cell_avail % 100) / 10));
    page[23][37] = Parity('0' + (cell_avail % 10));

    page[23][39] = Parity('0' + (cell_max / 100));
    page[23][40] = Parity('0' + ((cell_max % 100) / 10));
    page[23][41] = Parity('0' + (cell_max % 10));
}
