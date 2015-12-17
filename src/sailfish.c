#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

#define PORT "8080"
#define BACKLOG 10

#define SF_CLNT_HTTP_VER_09 0
#define SF_CLNT_HTTP_VER_10 1
#define SF_CLNT_HTTP_VER_11 2

#define SF_GET_MTHD 1
#define SF_CLNT_ERR_400 9

char *SF_404_RESP = "<html><title>404 Not Found</title><body><H1>NOT FOUND</H1><p>The requested URL could not be found.</p></body></html>";

struct sailfish_http_rqst
{
    int method;
    int client_http_version;
    char uri[2048];
};

char* sf_get_response_phrase(int status)
{
    if (status == 404)
    {
        return "Not Found";
    }
}

char* sf_get_client_http_version_str(struct sailfish_http_rqst *sf_str)
{
    if (sf_str->client_http_version == SF_CLNT_HTTP_VER_09)
    {
        return "HTTP/0.9";
    }
    else if (sf_str->client_http_version == SF_CLNT_HTTP_VER_10)
    {
        return "HTTP/1.0";
    }
    else
    {
        return "HTTP/1.1";
    }
}

/* Starting message */
void start_msg()
{
    printf("=== Sailfish ===\n\n");
}

/* Parse HTML requests */ 
int html_rqst_parser(char *rqst, struct sailfish_http_rqst *sf_str)
{
    int rc;
    char *tok, *stringp = rqst;

    if (sf_str == NULL)
    {
        /* server error maybe?? */
        return -1;
    }

    if (rqst == NULL)
    {
        return -1;
    }

    printf("%s\n", rqst);
    /* parse HTML status line *
     * Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF *
     */

    /* Check method */
    tok = strsep(&stringp, " ");
    if (tok == NULL)
    {
        /* 400 Bad Request */
        sf_str->method = SF_CLNT_ERR_400;
        return 0;
    }

    if (strncmp(tok, "GET ", 4) == 0)
    {
        /* classic GET method */
        printf("GET method detected\n");

        /* populate request structure */
        sf_str->method = SF_GET_MTHD;
        tok = strsep(&stringp, " ");
        if (tok == NULL)
        {
            /* 400 Bad Request */
            sf_str->method = SF_CLNT_ERR_400;
            return 0;
        }
        strncpy(sf_str->uri, tok, stringp - tok);
        printf("%d %s", sf_str->method, sf_str->uri);
    }
    else
    {
        /* looks like an unsupported method - 405 Method not allowed */
    }
}

int generate_response(struct sailfish_http_rqst *sf_str, char *resp)
{
    int final_status;
    char tmp[1000];
    if (resp == NULL)
    {
        return -1;
    }

    if (sf_str == NULL)
    {
        return -1;   
    }

    /* Construct Status Line */
    final_status = 404;
    sf_str->client_http_version = SF_CLNT_HTTP_VER_11;
    sprintf(tmp, "%s ", sf_get_client_http_version_str(sf_str)); 
    strcat(resp, tmp);
    sprintf(tmp, "%d ", final_status);
    strcat(resp, tmp);
    strcat(resp, sf_get_response_phrase(final_status));
    strcat(resp, "\n"); 

    /* Construct Entity Headers */
    sprintf(tmp, "Content-Length: %d\n", strlen(SF_404_RESP));
    strcat(resp, tmp);
    strcat(resp, "Content-Type: text/html\n");
    strcat(resp, "\n"); 

    /* Append Data */
    strcat(resp, SF_404_RESP);
    printf("%s\n", resp);
}


void process_rqst(int conn, char* rqst)
{
    int rc;
    char resp[1000] = {0};
    struct sailfish_http_rqst st_rqst;
    html_rqst_parser(rqst, &st_rqst);
    generate_response(&st_rqst, resp);
    rc = send(conn, resp, sizeof(resp), 0);
    if (rc == -1)
    {
        fprintf(stderr, "send: %s\n", strerror(errno));
    }
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
        client_rqst[rc] = '\0';
        process_rqst(new_conn, client_rqst);

/*
        rc = send(new_conn, "HTTP/1.0 200 OK\n"
"Content-Length: 46\n"
"Content-Type: text/html\n\n"
"<html><body><H1>Hello world</H1></body></html>", 141, 0);
        if (rc == -1)
        {
            fprintf(stderr, "send: %s\n", strerror(errno));
            continue;
        }
*/

        close(new_conn);
    }

    return 0;
}
