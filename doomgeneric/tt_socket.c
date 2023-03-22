#include "tt_socket.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

//based on https://www.geeksforgeeks.org/socket-programming-cc/
//this socket acts as client, server must be started at target device

int client_fd;
char ip[16];
int port = 0;

void TCPSocketCreate(char* targetIP, int targetPort)
{
    strncpy(ip, targetIP, 15);
    port = targetPort;

    printf("TCP socket parameters: %s:%d\n", targetIP, targetPort);

    int status;
    struct sockaddr_in serv_addr;

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }

    if ((status
        = connect(client_fd, (struct sockaddr*)&serv_addr,
                sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        printf("Make sure that a TCP server is running on %s:%d\n\n", ip, port);
        exit(EXIT_FAILURE);
    }

    printf("\nTeletext signal is sent via TCP to %s:%d\n\n", ip, port);
}

void TCPSocketClose()
{
    // closing the connected socket
    close(client_fd);
}

void TCPSocketSend(char* msg)
{
    send(client_fd, msg, strlen(msg), 0);
}

void TCPSocketSendTTPage(uint8_t page[TT_ROWS][TT_COLUMNS])
{
    for(int i = 0; i < TT_ROWS; i++)
    {
        send(client_fd, page[i], TT_COLUMNS, 0);
    }
}