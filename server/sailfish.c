#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "sailfish.h"

void set_up_listener()
{

}

void process_request()
{


}

int main(int argc, char *argv[])
{
    int port;
    int sockfd;

    if (argc < 2)
    {
        printf("Invalid usage.\n");
        print_usage();
    }

    /* Initialize primary socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(atoi(argv[1]));
    sa.sin_addr.s_addr = INADDR_ANY;
    memset(&sa.sin_zero, 0, 8);

    bind(sockfd, (struct sockaddr *)&sa, sizeof(struct sockaddr));

    listen(sockfd, 10);

    /* Exit */
    return 0;
}

void print_usage()
{
    printf("SailFish -> Ultra-Light, Ultra-Fast Web Server\n");
    printf("Usage: SailFish port\n");
}

