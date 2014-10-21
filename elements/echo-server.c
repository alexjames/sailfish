/***

This sample server simply echoes onto stdout what it receives on its socket, and relays it
back to the sender. Sending 'exit' closes the program.

Usage:
Server - a.out
Client - telnet localhost 8000

***/

#include <stdio.h>
#include <stdlib.h>
// For strerror and errno
#include <string.h>
#include <errno.h>
// Socket headers
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8000
#define SIZE 1024

int main(void)
{
    int status;
    int sockfd;
    struct sockaddr_in server;
    char buffer[SIZE];

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket error : %s\n", strerror(errno));
        exit(errno);
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0)
    {
        printf("Bind error : %s\n", strerror(errno));
        exit(errno);
    }

    if (listen(sockfd, 20) < 0)
    {
        printf("Listen error : %s\n", strerror(errno));
        exit(errno);
    }

    while (1)
    {
        int connection;
        struct sockaddr_in client;
        int addrlen = sizeof(client);

        connection = accept(sockfd, (struct sockaddr *) &client, &addrlen);

        // memset buffer to 0 before every read since we're overwriting every time        
        memset(buffer, 0, sizeof(buffer));
        recv(connection, buffer, SIZE, 0);
        send(connection, buffer, strlen(buffer), 0);
        printf("%s\n", buffer);

        if (strncmp(buffer, "exit", 4) == 0)
        {
            close(connection);
            break;
        }
        close(connection);
    }

    printf("Exiting program...\n");
    close(sockfd);

    return 0;
}

