/***

This sample server simply echoes onto stdout what it receives on its socket, and relays it
back to the sender. Sending 'exit' closes the program.

THIS IS AN IPV4-ONLY VERSION. JUST FOR EDUCATION AND FUN.

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

    // creates the communication endpoint, gives you the descriptor
    // socket(domain, type, protocol)
    // domain: AF_INET -> IPV4 Address Family
    // type: SOCK_STREAM -> connection-based, reliable (TCP)
    // protocol: 0 (IP)

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket error : %s\n", strerror(errno));
        exit(errno);
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY; //automatically select IP address

    // associate socket with IP and port
    if (bind(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0)
    {
        printf("Bind error : %s\n", strerror(errno));
        exit(errno);
    }

    // mark as passive socket for listening in on the port
    if (listen(sockfd, 1) < 0)
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

        close(connection);
        if (strncmp(buffer, "exit", 4) == 0)
        {
            break;
        }
    }

    printf("Exiting program...\n");
    close(sockfd);

    return 0;
}

