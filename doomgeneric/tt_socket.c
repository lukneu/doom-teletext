#include "tt_socket.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

//based on https://www.geeksforgeeks.org/socket-programming-cc/

int server_fd, new_socket, valread;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);

void TCPSocketCreate()
{
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("tt_socket: socket failed");
        exit(EXIT_FAILURE);
    }
  
    // Forcefully attaching socket to the defined port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        printf("tt_socket: setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
  
    // Forcefully attaching socket to the defined port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        printf("tt_socket: bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        printf("tt_socket: listen");
        exit(EXIT_FAILURE);
    }

    printf("* Waiting for tcp connection on %s:%d\n", "127.0.0.1", PORT);
	printf("* (e.g. 'netcat %s %d')\n", "127.0.0.1", PORT);

    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0)
    {
        printf("tt_socket: accept");
        exit(EXIT_FAILURE);
    }
}

void TCPSocketClose()
{
    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
}

void TCPSocketSend(char* msg)
{
    send(new_socket, msg, strlen(msg), 0);
}