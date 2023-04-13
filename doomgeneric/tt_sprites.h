#ifndef TT_SPRITES_H
#define TT_SPRITES_H

#include <stdint.h>

//Sprite for 2*6 text "INF." (can be used for infinite ammunition / armor)
static uint8_t sprite_text_inf[2][6] =
{
    { 0x6A, 0x6A, 0x20, 0x35, 0x37, 0x21 },
    { 0x6A, 0x6A, 0x29, 0x35, 0x37, 0x60 }
};

//Sprites for characters 0-9, each of them with size 2*2
static uint8_t sprite_char_array[10][2][2] =
{
    {
        //0
        { 0x6A, 0x6B },
        { 0x6A, 0x7A }
    },
    {
        //1
        { 0x20, 0x6E },
        { 0x20, 0x6A }
    },
    {
        //2
        { 0x22, 0x6B },
        { 0x6A, 0x73 }
    },
    {
        //3
        { 0x22, 0x6B },
        { 0x60, 0x7B }
    },
    {
        //4
        { 0x6A, 0x6A },
        { 0x22, 0x6B }
    },
    {
        //5
        { 0x6A, 0x23 },
        { 0x62, 0x7B }
    },
    {
        //6
        { 0x6A, 0x23 },
        { 0x6A, 0x7B }
    },
    {
        //7
        { 0x22, 0x6B },
        { 0x20, 0x6B }
    },
    {
        //8
        { 0x6A, 0x6B },
        { 0x6A, 0x7B }
    },
    {
        //9
        { 0x6A, 0x6B },
        { 0x62, 0x7B }
    },
};

// 4*40 statusbar, can display info about health, ammunition, armor, etc.
static uint8_t sprite_statusbar[4][40] =
{
    { 0x11, 0x6A, 0x6B, 0x6A, 0x6B, 0x6A, 0x6B, 0x20, 0x6A, 0x6B, 0x6A, 0x6B, 0x6A, 0x6B, 0x22, 0x3A, 0x07, 0x32, 0x07, 0x33, 0x07, 0x34, 0x17, 0x20, 0x78, 0x7F, 0x7F, 0x74, 0x20, 0x11, 0x6A, 0x6B, 0x6A, 0x6B, 0x6A, 0x6B, 0x22, 0x3A, 0x17, 0x2F },
    { 0x11, 0x6A, 0x7A, 0x6A, 0x7A, 0x6A, 0x7A, 0x20, 0x6A, 0x7A, 0x6A, 0x7A, 0x6A, 0x7A, 0x68, 0x61, 0x07, 0x35, 0x07, 0x36, 0x07, 0x37, 0x17, 0x20, 0x7F, 0x7E, 0x7D, 0x7F, 0x20, 0x11, 0x6A, 0x7A, 0x6A, 0x7A, 0x6A, 0x7A, 0x68, 0x61, 0x17, 0x2F },
    { 0x07, 0x20, 0x41, 0x4D, 0x4D, 0x4F, 0x20, 0x20, 0x20, 0x48, 0x45, 0x41, 0x4C, 0x54, 0x48, 0x20, 0x20, 0x41, 0x52, 0x4D, 0x53, 0x20, 0x17, 0x20, 0x2A, 0x7C, 0x7C, 0x25, 0x20, 0x07, 0x20, 0x41, 0x52, 0x4D, 0x4F, 0x52, 0x20, 0x20, 0x17, 0x2F },
    { 0x03, 0x42, 0x3A, 0x78, 0x78, 0x78, 0x2F, 0x34, 0x30, 0x30, 0x20, 0x53, 0x3A, 0x78, 0x78, 0x78, 0x2F, 0x31, 0x30, 0x30, 0x20, 0x52, 0x3A, 0x78, 0x78, 0x78, 0x2F, 0x31, 0x30, 0x30, 0x20, 0x43, 0x3A, 0x78, 0x78, 0x78, 0x2F, 0x36, 0x30, 0x30 }
};

// 3*4 player face: (80-300 % health)
static uint8_t sprite_player_face_h80[3][4] =
{
    { 0x78, 0x7F, 0x7F, 0x74 },
    { 0x7F, 0x6E, 0x3D, 0x7F },
    { 0x2A, 0x7C, 0x7C, 0x25 }
};

// 3*4 player face: (60-79 % health)
static uint8_t sprite_player_face_h60[3][4] =
{
    { 0x78, 0x7F, 0x7F, 0x74 },
    { 0x7F, 0x7E, 0x7D, 0x7F },
    { 0x2A, 0x7C, 0x7C, 0x25 }
};

// 3*4 player face: (40-59 % health)
static uint8_t sprite_player_face_h40[3][4] =
{
    { 0x78, 0x7F, 0x7F, 0x74 },
    { 0x7F, 0x3E, 0x6D, 0x7F },
    { 0x2A, 0x7D, 0x7E, 0x25 }
};

// 3*4 player face: (20-39 % health)
static uint8_t sprite_player_face_h20[3][4] =
{
    { 0x78, 0x7F, 0x7F, 0x74 },
    { 0x7F, 0x7E, 0x7D, 0x7F },
    { 0x2A, 0x7C, 0x7C, 0x25 }
};

// 3*4 player face: (01-19 % health)
static uint8_t sprite_player_face_h01[3][4] =
{
    { 0x78, 0x7F, 0x7F, 0x74 },
    { 0x7F, 0x2E, 0x2D, 0x7F },
    { 0x2A, 0x7E, 0x7D, 0x25 }
};

// 3*4 player face: (   00 % health)
static uint8_t sprite_player_face_dead[3][4] =
{
    { 0x78, 0x7F, 0x7F, 0x74 },
    { 0x7F, 0x58, 0x58, 0x7F },
    { 0x2A, 0x77, 0x7B, 0x25 }
};

// 7*18 DOOM logo
static uint8_t sprite_doom_logo[7][18] =
{
    { 0x20, 0x7C, 0x7C, 0x74, 0x20, 0x78, 0x7C, 0x74, 0x20, 0x78, 0x7C, 0x74, 0x20, 0x7C, 0x30, 0x60, 0x7C, 0x20 },
    { 0x20, 0x7F, 0x20, 0x7F, 0x20, 0x7F, 0x20, 0x7F, 0x20, 0x7F, 0x20, 0x7F, 0x20, 0x7f, 0x7F, 0x7F, 0x7f, 0x20 },
    { 0x20, 0x7F, 0x20, 0x7F, 0x20, 0x7F, 0x20, 0x7F, 0x20, 0x7F, 0x20, 0x7F, 0x20, 0x7f, 0x6F, 0x3F, 0x7f, 0x20 },
    { 0x20, 0x7F, 0x20, 0x7F, 0x20, 0x7F, 0x20, 0x7F, 0x20, 0x7F, 0x20, 0x7F, 0x20, 0x7f, 0x2A, 0x25, 0x7F, 0x20 },
    { 0x20, 0x7F, 0x9E, 0x27, 0x20, 0x22, 0x2F, 0x21, 0x20, 0x22, 0x2F, 0x21, 0x20, 0x2B, 0x20, 0x20, 0x7F, 0x20 },
    { 0x20, 0x3F, 0x21, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x6F, 0x20 },
    { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20 }
};

#endif