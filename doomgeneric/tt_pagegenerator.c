#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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
    for (uint8_t i = 0; i < TT_ROWS; i++)
    {
        for (uint8_t j = 0; j < TT_COLUMNS; j++)
        {
            page[i][j] = Parity(' ');

            /*
            //for easier debugging:
            page[i][j] = Parity('0' + ((j - 2) % 10));

            if (i + 2 == j)
            {
                page[i][j - 1] = Parity(TTEXT_ALPHA_MAGENTA);
                page[i][j] = Parity('X');
            }
            */
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

void TT_WriteHintMessage(uint8_t page[TT_ROWS][TT_COLUMNS], uint8_t line, char* string)
{
    TT_ClearLine(page, line); //Just in case the old message was longer than the current one

    page[line][2] = Parity(TTEXT_ALPHA_RED);
    int strLen = strlen(string);

    if(strLen > 36)
    {
        strLen = 36;
        page[line][39] = Parity('.');
        page[line][40] = Parity('.');
        page[line][41] = Parity('.');
    }

    for(int i = 0; i < strLen; i++)
    {
        page[line][3 + i] = Parity(string[i]);
    }
}

void TT_ClearLine(uint8_t page[TT_ROWS][TT_COLUMNS], uint8_t line)
{
    for(int i = 0; i < 40; i++)
    {
        page[line][2 + i] = Parity(' ');
    }
}

void WriteThreeDigitNumber(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS],
                           int value,
                           uint8_t start_row,
                           uint8_t start_col)
{
    uint8_t digit_1 = value / 100;
    uint8_t digit_2 = (value % 100) / 10;
    uint8_t digit_3 = value % 10;

    InsertIntoStatusbar(statusbar, start_row, start_col, 2, 2, sprite_char_array[digit_1]);
    InsertIntoStatusbar(statusbar, start_row, start_col + 2, 2, 2, sprite_char_array[digit_2]);
    InsertIntoStatusbar(statusbar, start_row, start_col + 4, 2, 2, sprite_char_array[digit_3]);
}

void TT_ShowDebugInfo(uint8_t page[TT_ROWS][TT_COLUMNS], uint8_t fpsValue, uint8_t graphicMode)
{
    uint8_t char_1 = fpsValue > 9 ? '0' + (fpsValue / 10) : 0x20; 
    uint8_t char_2 = '0' + (fpsValue % 10);

    uint8_t fpsBytes[1][7] = { { TTEXT_ALPHA_YELLOW, char_1, char_2, ' ', 'F', 'P', 'S' } };
    InsertIntoPage(page, 1, 33, 1, 7, fpsBytes);

    uint8_t grapicModeBytes[1][16] = { { TTEXT_ALPHA_YELLOW, 'G', 'r', 'a', 'p', 'h', 'i', 'c', ' ', 'm', 'o', 'd', 'e', ':', ' ', '0' + graphicMode} };
    InsertIntoPage(page, 1, 0, 1, 16, grapicModeBytes);
}

void TT_HideDebugInfo(uint8_t page[TT_ROWS][TT_COLUMNS])
{
    for (int i = 0; i < 40; i++)
    {
        page[1][i + 2] = ' ';
    }
}

void TT_SetActiveAmmunition(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS], int value)
{
    WriteThreeDigitNumber(statusbar, value, 0, 1);
}

void TT_SetActiveAmmunitionToInfinite(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS])
{
    InsertIntoStatusbar(statusbar, 0, 1, 2, 6, sprite_text_inf);
}

void TT_SetHealth(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS], int value, bool rampageMode)
{
    WriteThreeDigitNumber(statusbar, value, 0, 8);

    uint8_t faceColorByte = rampageMode ? Parity(TTEXT_GRAPHIC_RED) : Parity(TTEXT_GRAPHIC_WHITE);
    
    statusbar[0][23] = faceColorByte;
    statusbar[1][23] = faceColorByte;
    statusbar[2][23] = faceColorByte;

    //choose fitting status bar face
    //values from: https://doom.fandom.com/wiki/Status_bar_face
    if(value >= 80)
    {
        InsertIntoStatusbar(statusbar, 0, 24, 3, 4, sprite_player_face_h80);
    }
    else if(value >= 60)
    {
        InsertIntoStatusbar(statusbar, 0, 24, 3, 4, sprite_player_face_h60);
    }
    else if(value >= 40)
    {
        InsertIntoStatusbar(statusbar, 0, 24, 3, 4, sprite_player_face_h40);
    }
    else if(value >= 20)
    {
        InsertIntoStatusbar(statusbar, 0, 24, 3, 4, sprite_player_face_h20);
    }
    else if(value >= 1)
    {
        InsertIntoStatusbar(statusbar, 0, 24, 3, 4, sprite_player_face_h01);
    }
    else
    {
        InsertIntoStatusbar(statusbar, 0, 24, 3, 4, sprite_player_face_dead);
    }
}

void TT_SetArmor(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS], int value)
{
    WriteThreeDigitNumber(statusbar, value, 0, 30);
}

void TT_SetAvailableWeapons(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS],
                            bool w2, bool w3, bool w4, bool w5, bool w6, bool w7)
{
    statusbar[0][16] = w2 ? Parity(TTEXT_ALPHA_YELLOW) : Parity(TTEXT_ALPHA_WHITE);
    statusbar[0][18] = w3 ? Parity(TTEXT_ALPHA_YELLOW) : Parity(TTEXT_ALPHA_WHITE);
    statusbar[0][20] = w4 ? Parity(TTEXT_ALPHA_YELLOW) : Parity(TTEXT_ALPHA_WHITE);

    statusbar[1][16] = w5 ? Parity(TTEXT_ALPHA_YELLOW) : Parity(TTEXT_ALPHA_WHITE);
    statusbar[1][18] = w6 ? Parity(TTEXT_ALPHA_YELLOW) : Parity(TTEXT_ALPHA_WHITE);
    statusbar[1][20] = w7 ? Parity(TTEXT_ALPHA_YELLOW) : Parity(TTEXT_ALPHA_WHITE);
}

void TT_SetCards(uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS],
                 bool bluecard, bool yellowcard, bool redcard)
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

void TT_RenderInMosaicBlackWhite(uint32_t* DG_ScreenBuffer,
                                 uint8_t rendering[TT_FRAMEBUFFER_ROWS][TT_FRAMEBUFFER_COLUMNS],
                                 bool separate_graphics)
{
    //Source image is 320 pixels in width, we can only display
    // 312 in CONTIGUOUS mode for mosaics, and 308 in SEPARATE mode.
    uint8_t ignoredPixels = separate_graphics ? 12 : 8;

    for (int tt_row = 0; tt_row < TT_FRAMEBUFFER_ROWS; tt_row++)
    {
        for (int tt_col = 0; tt_col < TT_FRAMEBUFFER_COLUMNS; tt_col++)
        {
            if(tt_col == 0)
            {
                rendering[tt_row][tt_col] = TTEXT_GRAPHIC_WHITE;
            }
            else if (separate_graphics && tt_col == 1)
            {
                rendering[tt_row][tt_col] = TTEXT_SEPARATED_GRAPHICS;
            }
            else
            {
                int pixelPos = 1;
                uint8_t mosaicMask = 0;

                for (int cell_y = 0; cell_y < 3; cell_y++) //teletext mosaic is 2*3 'pixels'
                {
                    if(tt_row == TT_FRAMEBUFFER_ROWS - 1 && cell_y == 2)
                    {
                        //no framebuffer data for this, leave black
                        continue;
                    }

                    for (int cell_x = 0; cell_x < 2; cell_x++)
                    {
                        int r = 0;
                        int g = 0;
                        int b = 0;
                        int brightness;

                        //4*4 pixels of DG_ScreenBuffer result in 1 'pixel' in teletext mosaic
                        for (int region_y = 0; region_y < 4; region_y++)
                        {
                            for (int region_x = 0; region_x < 4; region_x++)
                            {
                                int src_pixel_value = DG_ScreenBuffer[(12 * tt_row + 4 * cell_y + region_y) * 320 /*DOOMGENERIC_RESX*/ +
                                                                      (8 * (tt_col - 1) + 4 * cell_x + region_x + ignoredPixels / 2)];
                                r += (uint8_t)(src_pixel_value >> 16);
                                g += (uint8_t)(src_pixel_value >> 8);
                                b += (uint8_t)src_pixel_value;
                            }
                        }

                        r = r / 16;
                        g = g / 16;
                        b = b / 16;

                        brightness = 0.21f * r + 0.72f * g + 0.07f * b;

                        if(brightness > 60)
                        {
                            mosaicMask += pixelPos;
                        }

                        pixelPos = pixelPos * 2;
                    }
                }

                rendering[tt_row][tt_col] = GetTeletextEncodingMosaicByBitMask(mosaicMask);
            }
        }
    }
}

void TT_InsertStatusbar(uint8_t page[TT_ROWS][TT_COLUMNS],
                        uint8_t statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS])
{
    InsertIntoPage(page, 20, 0, TT_STATUSBAR_ROWS, TT_STATUSBAR_COLUMNS, statusbar);
}

void TT_InsertGameRendering(uint8_t page[TT_ROWS][TT_COLUMNS],
                            uint8_t rendering[TT_FRAMEBUFFER_ROWS][TT_FRAMEBUFFER_COLUMNS])
{
    InsertIntoPage(page, 3, 0, TT_FRAMEBUFFER_ROWS, TT_FRAMEBUFFER_COLUMNS, rendering);
}

void EncodeString(char* sourceString, uint8_t* targetArray, bool make_uppercase)
{
    size_t strLen = strlen(sourceString);

    for(int i = 0; i < strLen; i++)
    {
        if(make_uppercase && sourceString[i] >= 'a' && sourceString[i] <= 'z')
        {
            targetArray[i] = Parity(sourceString[i] - 32);
        }
        else
        {
            targetArray[i] = Parity(sourceString[i]);
        }
    }
}

/// @brief Translates string identifiers (used by wad files) to actual strings
///        WATCH OUT: currently only valid for doom1.wad -> if other WAD files are
///                   used, this function has to be adapted.
/// @param sourceString holds identifier string
/// @param targetArray is used to save actual string
/// @return length of string stored in targetArray 
int GetMenuEntryText(char* sourceString, uint8_t* targetArray, int showMessagesValue, int detailLevel)
{
    char actualString[38] = {'\0'};

    //DOOM_MENU_MAINDEF entries
    if (strcmp(sourceString, "M_NGAME") == 0)
    {
        strcpy(actualString, "NEW GAME");
        EncodeString(actualString, targetArray, false);
    }
    else if (strcmp(sourceString, "M_OPTION") == 0)
    {
        strcpy(actualString, "OPTIONS");
        EncodeString(actualString, targetArray, false);
    }
    else if (strcmp(sourceString, "M_LOADG") == 0)
    {
        strcpy(actualString, "LOAD GAME");
        EncodeString(actualString, targetArray, false);
    }
    else if (strcmp(sourceString, "M_SAVEG") == 0)
    {
        strcpy(actualString, "SAVE GAME");
        EncodeString(actualString, targetArray, false);
    }
    else if (strcmp(sourceString, "M_RDTHIS") == 0)
    {
        strcpy(actualString, "READ THIS!");
        EncodeString(actualString, targetArray, false);
    }
    else if (strcmp(sourceString, "M_QUITG") == 0)
    {
        strcpy(actualString, "QUIT GAME");
        EncodeString(actualString, targetArray, false);
    }
    //DOOM_MENU_EPIDEF entries
    else if (strcmp(sourceString, "M_EPI1") == 0)
    {
        strcpy(actualString, "KNEE-DEEP IN THE DEAD");
        EncodeString(actualString, targetArray, false);
    }
    else if (strcmp(sourceString, "M_EPI2") == 0)
    {
        strcpy(actualString, "THE SHORES OF HELL");
        EncodeString(actualString, targetArray, false);
    }
    else if (strcmp(sourceString, "M_EPI3") == 0)
    {
        strcpy(actualString, "INFERNO");
        EncodeString(actualString, targetArray, false);
    }
    //DOOM_MENU_NEWDEF entries
    else if (strcmp(sourceString, "M_JKILL") == 0)
    {
        strcpy(actualString, "I'M TOO YOUNG TO DIE.");
        EncodeString(actualString, targetArray, false);
    }
    else if (strcmp(sourceString, "M_ROUGH") == 0)
    {
        strcpy(actualString, "HEY, NOT TOO ROUGH.");
        EncodeString(actualString, targetArray, false);
    }
    else if (strcmp(sourceString, "M_HURT") == 0)
    {
        strcpy(actualString, "HURT ME PLENTY.");
        EncodeString(actualString, targetArray, false);
    }
        else if (strcmp(sourceString, "M_ULTRA") == 0)
    {
        strcpy(actualString, "ULTRA-VIOLENCE.");
        EncodeString(actualString, targetArray, false);
    }
    else if (strcmp(sourceString, "M_NMARE") == 0)
    {
        strcpy(actualString, "NIGHTMARE!");
        EncodeString(actualString, targetArray, false);
    }
    //DOOM_MENU_OPTIONSDEF entries
    else if (strcmp(sourceString, "M_ENDGAM") == 0)
    {
        strcpy(actualString, "END GAME");
        EncodeString(actualString, targetArray, false);
    }
    else if (strcmp(sourceString, "M_MESSG") == 0)
    {
        strcpy(actualString, "MESSAGES: ");
        strcpy(actualString + 10, showMessagesValue ? "ON" : "OFF");

        EncodeString(actualString, targetArray, false);
    }
    else if (strcmp(sourceString, "M_DETAIL") == 0)
    {
        strcpy(actualString, "GRAPHIC DETAIL: ");
        strcpy(actualString + 16, detailLevel ? "LOW" : "HIGH");

        EncodeString(actualString, targetArray, false);
    }
    else if (strcmp(sourceString, "M_SCRNSZ") == 0)
    {
        strcpy(actualString, "SCREEN SIZE");
        EncodeString(actualString, targetArray, false);
    }
    else if (strcmp(sourceString, "M_MSENS") == 0)
    {
        strcpy(actualString, "MOUSE-SENSITIVITY");
        EncodeString(actualString, targetArray, false);
    }
    else if (strcmp(sourceString, "M_SVOL") == 0)
    {
        strcpy(actualString, "SOUND VOLUME");
        EncodeString(actualString, targetArray, false);
    }
    //DOOM_MENU_SOUNDDEF entries
    else if (strcmp(sourceString, "M_SFXVOL") == 0)
    {
        strcpy(actualString, "SFX VOLUME");
        EncodeString(actualString, targetArray, false);
    }
    else if (strcmp(sourceString, "M_MUSVOL") == 0)
    {
        strcpy(actualString, "MUSIC VOLUME");
        EncodeString(actualString, targetArray, false);
    }
    //Unknown entries (should never happen)
    else
    {
        strcpy(actualString, sourceString);
        EncodeString(actualString, targetArray, false);
    }

    return strlen(actualString);
}

void TT_InsertMenuMessage(uint8_t rendering[TT_FRAMEBUFFER_ROWS][TT_FRAMEBUFFER_COLUMNS], char* msg)
{
    char string[strlen(msg)];
    strcpy(string, msg);

    char delimiter[] = "\n";
    char *linePtr;

    linePtr = strtok(string, delimiter);

    int msgLine = 1;

    while(linePtr != NULL)
    {
        int lineLen = strlen(linePtr);

        if(lineLen >= 38)
        {
            lineLen = 38;
        }

        int startCol = 20 - lineLen / 2;

        rendering[5 + msgLine][startCol - 1] = Parity(TTEXT_ALPHA_RED);

        uint8_t stringArray[lineLen];
        EncodeString(linePtr, stringArray, true);
        for (int i = 0; i < lineLen; i++)
        {
            rendering[5 + msgLine][startCol + i] = stringArray[i];
        }

        rendering[5 + msgLine][startCol + lineLen] = Parity(TTEXT_GRAPHIC_WHITE);

        linePtr = strtok(NULL, delimiter);
        msgLine++;
    }
}

void TT_OverlayMenu(uint8_t rendering[TT_FRAMEBUFFER_ROWS][TT_FRAMEBUFFER_COLUMNS],
                    short itemsCount, char** itemsNames, short activeIndex, short* itemsStati,
                    struct tt_menu_slider_values sliderValues, int showMessagesValue, int detailLevel)
{
    //determine start column for all menu entries
    int longestStringLength = 0;
    uint8_t actualNamesArray[itemsCount][38];
    int actualNameLengthsArray[itemsCount];

    for (int i = 0; i < itemsCount; i++)
    {
        int itemLen = GetMenuEntryText(itemsNames[i], actualNamesArray[i], showMessagesValue, detailLevel);

        actualNameLengthsArray[i] = itemLen;

        if(itemLen > longestStringLength)
        {
            longestStringLength = itemLen;
        }
    }
    int startCol = 20 - longestStringLength / 2;

    int currentLine = 0;
    int sliderValue = 0;
    int maxSliderValue = 0;

    for (int i = 0; i < itemsCount; i++)
    {
        bool isActive = (i == activeIndex);
        
        if(itemsStati[i] > 0) //not a slider
        {
            //if item has slider in next line, remember values
            if(strcmp(itemsNames[i], "M_SFXVOL") == 0)
            {
                sliderValue = sliderValues.sfxVol;
                maxSliderValue = 15;
            }
            else if(strcmp(itemsNames[i], "M_MUSVOL") == 0)
            {
                sliderValue = sliderValues.musicVol;
                maxSliderValue = 15;
            }
            if(strcmp(itemsNames[i], "M_SCRNSZ") == 0)
            {
                sliderValue = sliderValues.screenSize;
                maxSliderValue = 8;
            }
            else if(strcmp(itemsNames[i], "M_MSENS") == 0)
            {
                sliderValue = sliderValues.mouseSen;
                maxSliderValue = 9;
            }

            uint8_t colorByte = isActive ? Parity(TTEXT_ALPHA_YELLOW) : Parity(TTEXT_ALPHA_RED);

            rendering[5 + currentLine][startCol - 1] = colorByte;

            for (int j = 0; j < actualNameLengthsArray[i]; j++)
            {
                rendering[5 + currentLine][startCol + j] = actualNamesArray[i][j];
            }

            rendering[5 + currentLine][startCol + actualNameLengthsArray[i]] = Parity(TTEXT_GRAPHIC_WHITE);
        }
        else //slider
        {
            // value range: 0 - maxSliderValue
            int itemLen = maxSliderValue + 1;

            rendering[5 + currentLine][startCol - 1] = Parity(TTEXT_GRAPHIC_BLUE);
            rendering[5 + currentLine][startCol] = Parity(GetTeletextEncodingMosaicByBitMask(0b111111));

            for (int j = 0; j < itemLen; j++)
            {
                rendering[5 + currentLine][startCol + j + 1] =
                    j == sliderValue ?
                        Parity(GetTeletextEncodingMosaicByBitMask(0b110011)) :
                        Parity(GetTeletextEncodingMosaicByBitMask(0b111111));
            }

            rendering[5 + currentLine][startCol + itemLen + 1] = Parity(GetTeletextEncodingMosaicByBitMask(0b111111));
            rendering[5 + currentLine][startCol + itemLen + 2] = Parity(TTEXT_GRAPHIC_WHITE);
        }

        currentLine++;
    }
}

void TT_OverlayLoadMenu(uint8_t rendering[TT_FRAMEBUFFER_ROWS][TT_FRAMEBUFFER_COLUMNS],
                        char savegameStrings[10][24], short activeIndex)
{
    int itemsCount = 6;

    //determine start column for all entries
    int longestStringLength = 0;

    for (int i = 0; i < itemsCount; i++)
    {
        int itemLen = strlen(savegameStrings[i]);
        if(itemLen == 0) //empty slot
        {
            itemLen = 10; //EMPTY SLOT len
        }

        if(itemLen > longestStringLength)
        {
            longestStringLength = itemLen;
        }
    }
    int startCol = 20 - longestStringLength / 2;

    int currentLine = 0;

    for (int i = 0; i < itemsCount; i++)
    {
        bool isEmptySlot = false;
        int itemLen = strlen(savegameStrings[i]);
        
        if(itemLen == 0)
        {
            isEmptySlot = true;
            itemLen = 10; //strlen of "EMPTY SLOT"
        }

        bool isActive = (i == activeIndex);
        
        uint8_t colorByte = isActive ? Parity(TTEXT_ALPHA_YELLOW) : Parity(TTEXT_ALPHA_RED);

        rendering[5 + currentLine][startCol - 1] = colorByte;

        if (! isEmptySlot)
        {
            for (int j = 0; j < itemLen; j++)
            {
                rendering[5 + currentLine][startCol + j] = savegameStrings[i][j];
            }
        }
        else
        {
            char placeholder[10] = "EMPTY SLOT";
            
            for (int j = 0; j < itemLen; j++)
            {
                rendering[5 + currentLine][startCol + j] = placeholder[j];
            }
        }

        rendering[5 + currentLine][startCol + itemLen] = Parity(TTEXT_GRAPHIC_WHITE);

        currentLine++;
    }
}

void TT_OverlaySaveMenu(uint8_t rendering[TT_FRAMEBUFFER_ROWS][TT_FRAMEBUFFER_COLUMNS],
                        char savegameStrings[10][24], short activeIndex, int inEditMode)
{
    int itemsCount = 6;

    //determine start column for all entries
    int longestStringLength = 0;

    for (int i = 0; i < itemsCount; i++)
    {
        int itemLen = strlen(savegameStrings[i]);
        if(itemLen == 0) //empty slot
        {
            itemLen = 10; //EMPTY SLOT len
        }

        if(itemLen > longestStringLength)
        {
            longestStringLength = itemLen;
        }
    }
    int startCol = 20 - longestStringLength / 2;

    int currentLine = 0;

    for (int i = 0; i < itemsCount; i++)
    {
        bool isEmptySlot = false;
        int itemLen = strlen(savegameStrings[i]);
        
        if(itemLen == 0)
        {
            isEmptySlot = true;
            itemLen = 10; //strlen of "EMPTY SLOT"
        }

        bool isActive = (i == activeIndex);
        
        uint8_t colorByte = isActive ? 
            inEditMode ? Parity(TTEXT_ALPHA_MAGENTA) : Parity(TTEXT_ALPHA_YELLOW) :
            Parity(TTEXT_ALPHA_RED);
    

        rendering[5 + currentLine][startCol - 1] = colorByte;

        if (! isEmptySlot)
        {
            for (int j = 0; j < itemLen; j++)
            {
                rendering[5 + currentLine][startCol + j] = savegameStrings[i][j];
            }
        }
        else
        {
            char placeholder[10] = "EMPTY SLOT";
            
            for (int j = 0; j < itemLen; j++)
            {
                rendering[5 + currentLine][startCol + j] = placeholder[j];
            }
        }

        rendering[5 + currentLine][startCol + itemLen] = Parity(TTEXT_GRAPHIC_WHITE);

        currentLine++;
    }
}