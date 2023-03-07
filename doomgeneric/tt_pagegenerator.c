#include <stdint.h>
#include <stdio.h>

#include "tt_pagegenerator.h"
#include "tt_encoding.h"
#include "tt_charset.h"

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
}

//for testing purposes only
void TT_SetAmmunition(uint8_t page[ROWS][COLUMNS], int value)
{
    uint8_t ammunationText[1][9] = { { TTEXT_ALPHA_RED, 'A', 'm', 'm', 'u', ':', 'x', 'y', 'z' } };
    ammunationText[0][6] = Parity('0' + (value / 100));
    ammunationText[0][7] = Parity('0' + ((value % 100) / 10));
    ammunationText[0][8] = Parity('0' + (value % 10));

    InsertIntoPage(page, 23, 1, 1, 9, ammunationText);
}