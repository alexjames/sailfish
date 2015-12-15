#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

#define PORT "8080"
#define BACKLOG 10

/* Starting message */
void start_msg()
{
    printf("=== Sailfish ===\n\n");
}

int main(int argc, char* argv[])
{
    int sockfd, new_conn;
    int rc, yes = 1;
    int addrlen = sizeof(struct sockaddr);
    start_msg();
    struct addrinfo hints, *serv_info, *p;
    struct sockaddr conn_info;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;  /* allow ipv4 and ipv6 */
    hints.ai_socktype = SOCK_STREAM; /* connection oriented */
    hints.ai_flags = AI_PASSIVE; /* bind() will fill in the hostname later */

    rc = getaddrinfo(NULL, PORT, &hints, &serv_info);

    if (rc != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
        return -1;
    }

    for (p = serv_info; p != NULL; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1)
        {
            fprintf(stderr, "socket: %s\n", strerror(errno));
            continue;
        }

        /* set SO_REUSEADDR so that bind can reuse local addresses */
        rc = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (rc == -1)
        {
            fprintf(stderr, "setsockopt: %s\n", strerror(errno));
            return -1;
        }

        rc = bind(sockfd, p->ai_addr, p->ai_addrlen);
        if (rc == -1)
        {
            fprintf(stderr, "bind: %s\n", strerror(errno));
            continue;
        }
        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "Failed to create server socket. Exiting...\n");
        return -1;
    }

    rc = listen(sockfd, BACKLOG);
    if (rc == -1)
    {
        fprintf(stderr, "Failed to configure server socket. Exiting...\n");
        return -1;
    }

    printf("Sailfish ready to glide. Awaiting connections...\n");

    /* Listen loop */
    while (1)
    {
        new_conn = accept(sockfd, &conn_info, &addrlen);
        char client_rqst[10024];
        if (new_conn == -1)
        {
            fprintf(stderr, "accept: %s\n", strerror(errno));
            continue;
        }

        rc = recv(new_conn, client_rqst, sizeof(client_rqst), 0);
        if (rc == -1)
        {
            fprintf(stderr, "send: %s\n", strerror(errno));
            continue;
        }
        printf("%s\n", client_rqst);

        rc = send(new_conn, "HTTP/1.0 200 OK\n"
"Content-Length: 46\n"
"Content-Type: text/html\n\n"
"<html><body><H1>Hello world</H1></body></html>", 141, 0);
        if (rc == -1)
        {
            fprintf(stderr, "send: %s\n", strerror(errno));
            continue;
        }


        close(new_conn);
    }

    return 0;
}
