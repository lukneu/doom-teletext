#ifndef TT_SOCKET_H
#define TT_SOCKET_H

#include "tt_pagegenerator.h"

void TCPSocketCreate(char* targetIP, int targetPort);
void TCPSocketClose();
void TCPSocketSend();
void TCPSocketSendTTPage(uint8_t page[TT_ROWS][TT_COLUMNS]);

#endif