#include "tt_encoding.h"

//https://github.com/ali1234/raspi-teletext/blob/master/hamming.c
uint8_t Hamming84(uint8_t d)
{
    uint8_t d1 = d&1;
    uint8_t d2 = (d>>1)&1;
    uint8_t d3 = (d>>2)&1;
    uint8_t d4 = (d>>3)&1;

    uint8_t p1 = (1 + d1 + d3 + d4) & 1;
    uint8_t p2 = (1 + d1 + d2 + d4) & 1;
    uint8_t p3 = (1 + d1 + d2 + d3) & 1;
    uint8_t p4 = (1 + p1 + d1 + p2 + d2 + p3 + d3 + d4) & 1;

    return (p1 | (d1<<1) | (p2<<2) | (d2<<3)
     | (p3<<4) | (d3<<5) | (p4<<6) | (d4<<7));
}

//https://github.com/ali1234/raspi-teletext/blob/master/hamming.c
uint8_t Parity(uint8_t d)
{
    d &= 0x7f;
    uint8_t p = 1;
    uint8_t t = d;
    int i;
    for (i=0; i<7; i++) {
        p += t&1;
        t = t>>1;
    }
    p &= 1;
    return d|(p<<7);
}

void EncodeMpag(uint8_t mag, uint8_t adr, uint8_t result[2])
{
    // see https://github.com/losso3000/420-years-of-teletext/blob/main/notes/packet-format.md

    //lower 3 bits = magazine, upper 5 bits = packet address (row)
    uint8_t byte = mag | (adr << 3);

    uint8_t nibble1 = byte & 0b00001111;
    uint8_t nibble2 = (byte & 0b11110000) >> 4;

    result[0] = Hamming84(nibble1);
    result[1] = Hamming84(nibble2);
}