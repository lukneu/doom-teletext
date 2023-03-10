#ifndef TT_SOCKET_H
#define TT_SOCKET_H

#include "tt_pagegenerator.h"

#define PORT 8080

void TCPSocketCreate();
void TCPSocketClose();
void TCPSocketSend();
void TCPSocketSendTTPage(uint8_t page[TT_ROWS][TT_COLUMNS]);

#endif