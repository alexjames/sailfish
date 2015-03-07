#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "sailfish.h"

int set_up_listener(int port)
{
    /* Initialize listening socket */
    int optval = 1;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = INADDR_ANY;
    memset(&sa.sin_zero, 0, 8);

    // Set SO_REUSEADDR socket option, otherwise we'll be plagued with 'Address already in use'
    // errors. Common wisdom says everything will be OK.
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
    {
        perror("setsocketopt");
        return (-1);
    }
    if (bind(sockfd, (struct sockaddr *)&sa, sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        return (-1);
    }
    if (listen(sockfd, 10) == -1)
    {
        perror("listen");
        return (-1);
    }
    return sockfd;
}

void process_request(int in_soc, struct sockaddr_in sa)
{
    char msg[] = "Thank you for using SailFish. Have a nice day. :)\n";
    int sent = send(in_soc, msg, sizeof(msg), 0);
    printf ("\nWrote %d bytes over connection", sent);
    close(in_soc);
}

void listen_loop(int sockfd)
{
    struct sockaddr_in in_sa;
    int conn, len;
    while (1)
    {
        conn = accept(sockfd, (struct sockaddr *)&in_sa, &len);
        process_request(conn, in_sa);
    }
}

int main(int argc, char *argv[])
{
    int specified_port = atoi(argv[1]);
    int primary_listener;     /* Primary listening socket FD */
    int len;

    if (argc < 2)
    {
        printf("Invalid usage.\n");
        print_usage();
        return 0;
    }

    primary_listener = set_up_listener(specified_port);

    if (primary_listener < 0)
    {
        printf("Looks like we are having some problems. SailFish will now stop.\n");
        return 0;
    }

    listen_loop(primary_listener);

    /* Exit */
    return 0;
}

void print_usage()
{
    printf("SailFish -> Ultra-Light, Ultra-Fast Web Server\n");
    printf("Usage: sailfish port\n");
}

