#ifndef TT_ENCODING_H
#define TT_ENCODING_H

#include <stdint.h>

uint8_t Hamming84(uint8_t d);
uint8_t Parity(uint8_t d);

void EncodeMpag(uint8_t mag, uint8_t adr, uint8_t result[2]);

#endif