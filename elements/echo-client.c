/***

Sample client implementation

***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    struct addrinfo hints, *res;
    int sockfd;

    memset(&hints, 0, sizeof(hints));

    // don't care if IPV4 or IPV6
    hints.ai_family = AF_UNSPEC;
    // TCP stream socket
    hints.ai_socktype = SOCK_STREAM;

    // Get info about the address
    // Use function to fill up structs


    if (getaddrinfo(argv[1], argv[2], &hints, &res) < 0)
    {
        printf("Failed to get address info: %s", strerror(errno));
        exit(errno);
    }

    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
    {
        printf("Failed to create socket: %s", strerror(errno));
        exit(errno);
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0)
    {
        printf("Failed to connect: %s", strerror(errno));
        exit(errno);
    }

    return 0;
}
