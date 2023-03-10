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
//source array does not have to have correct parity bit, parity bit is set by this function
void InsertIntoPage(uint8_t target[TT_ROWS][TT_COLUMNS], int startRow, int startColumn, int height, int width, uint8_t source[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            target[startRow + i][startColumn + 2 + j] = Parity(source[i][j]);
        }
    }
}

void TT_InsertStatusbar(uint8_t page[TT_ROWS][TT_COLUMNS], uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS])
{
    InsertIntoPage(page, 20, 0, TT_STATUSBAR_ROWS, TT_STATUSBAR_COLUMNS, statusbar);
}

//target array is of size 4*40
//source array does not have to have correct parity bit, parity bit is set by this function
void InsertIntoStatusbar(uint8_t target[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS], int startRow, int startColumn, int height, int width, uint8_t source[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            target[startRow + i][startColumn + j] = source[i][j];
        }
    }
}

void TT_InitPage(uint8_t page[TT_ROWS][TT_COLUMNS])
{
    //for easier debugging:
    for (uint8_t i = 0; i < TT_ROWS; i++)
    {
        for (uint8_t j = 0; j < TT_COLUMNS; j++)
        {
            page[i][j] = Parity('0' + ((j - 2) % 10));

            if (i + 2 == j)
            {
                page[i][j - 1] = Parity(TTEXT_ALPHA_MAGENTA);
                page[i][j] = Parity('X');
            }
        }
    }



    uint8_t mpag_bytes[2];

    EncodeMpag(PAGE_MAG, (10 * PAGE_TENS) + PAGE_UNITS, mpag_bytes);

    page[0][0] = mpag_bytes[0];
    page[0][1] = mpag_bytes[1];

    //https://github.com/losso3000/420-years-of-teletext/blob/main/notes/packet-format.md

    uint8_t headerConfigBytes[1][8] = { { Hamming84(PAGE_UNITS), Hamming84(PAGE_TENS), 0x15, 0x15, 0x15, 0x15, 0x15, 0x15 } };
    InsertIntoPage(page, 0, 0, 1, 8, headerConfigBytes);
   
    uint8_t headerContextBytes[1][32] = { { TTEXT_ALPHA_CYAN, 'g', 'i', 't', 'h', 'u', 'b', '.', 'c', 'o', 'm', '/', 'l', 'u', 'k', 'n', 'e', 'u', '/', 'd', 'o', 'o', 'm', '-', 't', 'e', 'l', 'e', 't', 'e', 'x', 't' } };
    InsertIntoPage(page, 0, 8, 1, 32, headerContextBytes);

    //set mpag bytes for each row
    for (size_t row = 1; row < TT_ROWS; row++)
    {
        EncodeMpag(PAGE_MAG, row, mpag_bytes);
        page[row][0] = mpag_bytes[0];
        page[row][1] = mpag_bytes[1];
    }

    uint8_t helloWorldText[1][14] = { { TTEXT_ALPHA_GREEN, 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', TTEXT_ALPHA_WHITE } };
    InsertIntoPage(page, 2, 5, 1, 14, helloWorldText);
}

void TT_InitStatusbar(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS])
{
    for (uint8_t i = 0; i < TT_STATUSBAR_ROWS; i++)
    {
        for (uint8_t j = 0; j < TT_STATUSBAR_COLUMNS; j++)
        {
            statusbar[i][j] = Parity(sprite_statusbar[i][j]);
        }
    }
}

void WriteThreeDigitNumber(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS], int value, uint8_t start_row, uint8_t start_col)
{
    uint8_t digit_1 = value / 100;
    uint8_t digit_2 = (value % 100) / 10;
    uint8_t digit_3 = value % 10;

    InsertIntoStatusbar(statusbar, start_row, start_col, 2, 2, sprite_char_array[digit_1]);
    InsertIntoStatusbar(statusbar, start_row, start_col + 2, 2, 2, sprite_char_array[digit_2]);
    InsertIntoStatusbar(statusbar, start_row, start_col + 4, 2, 2, sprite_char_array[digit_3]);
}

void TT_SetActiveAmmunition(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS], int value)
{
    WriteThreeDigitNumber(statusbar, value, 0, 1);
}

void TT_SetActiveAmmunitionToInfinite(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS])
{
    InsertIntoStatusbar(statusbar, 0, 1, 2, 6, sprite_text_inf);
}

void TT_SetHealth(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS], int value)
{
    WriteThreeDigitNumber(statusbar, value, 0, 8);
}

void TT_SetArmor(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS], int value)
{
    WriteThreeDigitNumber(statusbar, value, 0, 30);
}

void TT_SetAvailableWeapons(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS], bool w2, bool w3, bool w4, bool w5, bool w6, bool w7)
{
    statusbar[0][16] = w2 ? Parity(TTEXT_ALPHA_YELLOW) : Parity(TTEXT_ALPHA_WHITE);
    statusbar[0][18] = w3 ? Parity(TTEXT_ALPHA_YELLOW) : Parity(TTEXT_ALPHA_WHITE);
    statusbar[0][20] = w4 ? Parity(TTEXT_ALPHA_YELLOW) : Parity(TTEXT_ALPHA_WHITE);

    statusbar[1][16] = w5 ? Parity(TTEXT_ALPHA_YELLOW) : Parity(TTEXT_ALPHA_WHITE);
    statusbar[1][18] = w6 ? Parity(TTEXT_ALPHA_YELLOW) : Parity(TTEXT_ALPHA_WHITE);
    statusbar[1][20] = w7 ? Parity(TTEXT_ALPHA_YELLOW) : Parity(TTEXT_ALPHA_WHITE);
}

void TT_SetCards(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS], bool bluecard, bool yellowcard, bool redcard)
{
    statusbar[0][38] = bluecard ? Parity(TTEXT_GRAPHIC_BLUE) : Parity(TTEXT_GRAPHIC_WHITE);
    statusbar[1][38] = yellowcard ? Parity(TTEXT_GRAPHIC_YELLOW) : Parity(TTEXT_GRAPHIC_WHITE);
    statusbar[2][38] = redcard ? Parity(TTEXT_GRAPHIC_RED) : Parity(TTEXT_GRAPHIC_WHITE);
}

void TT_SetAmmunitionValues(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS],
                            int bull_avail, int bull_max,
                            int shel_avail, int shel_max,
                            int rckt_avail, int rckt_max,
                            int cell_avail, int cell_max)
{
    //BULL
    statusbar[3][3] = Parity('0' + (bull_avail / 100));
    statusbar[3][4] = Parity('0' + ((bull_avail % 100) / 10));
    statusbar[3][5] = Parity('0' + (bull_avail % 10));

    statusbar[3][7] = Parity('0' + (bull_max / 100));
    statusbar[3][8] = Parity('0' + ((bull_max % 100) / 10));
    statusbar[3][9] = Parity('0' + (bull_max % 10));

    //SHEL
    statusbar[3][13] = Parity('0' + (shel_avail / 100));
    statusbar[3][14] = Parity('0' + ((shel_avail % 100) / 10));
    statusbar[3][15] = Parity('0' + (shel_avail % 10));

    statusbar[3][17] = Parity('0' + (shel_max / 100));
    statusbar[3][18] = Parity('0' + ((shel_max % 100) / 10));
    statusbar[3][19] = Parity('0' + (shel_max % 10));

    //RCKT
    statusbar[3][23] = Parity('0' + (rckt_avail / 100));
    statusbar[3][24] = Parity('0' + ((rckt_avail % 100) / 10));
    statusbar[3][25] = Parity('0' + (rckt_avail % 10));

    statusbar[3][27] = Parity('0' + (rckt_max / 100));
    statusbar[3][28] = Parity('0' + ((rckt_max % 100) / 10));
    statusbar[3][29] = Parity('0' + (rckt_max % 10));

    //CELL
    statusbar[3][33] = Parity('0' + (cell_avail / 100));
    statusbar[3][34] = Parity('0' + ((cell_avail % 100) / 10));
    statusbar[3][35] = Parity('0' + (cell_avail % 10));

    statusbar[3][37] = Parity('0' + (cell_max / 100));
    statusbar[3][38] = Parity('0' + ((cell_max % 100) / 10));
    statusbar[3][39] = Parity('0' + (cell_max % 10));
}
