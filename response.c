#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <json-c/json.h>
#include "./include/myheader.h"
#define FAIL -1

void apiresponse(char *str, SSL_CTX *ctx, SSL *ssl)
{
    char buf[2025];
    int server;
    server = OpenConnection("community-open-weather-map.p.rapidapi.com", 443);
    ssl = SSL_new(ctx);           /* create new SSL connection state */
    SSL_set_fd(ssl, server);      /* attach the socket descriptor */
    if (SSL_connect(ssl) == FAIL) /* perform the connection */
        ERR_print_errors_fp(stderr);

    else
    {
        int bytes;
        char *front = "GET /find?q=";
        char *last = "&cnt=1&mode=null&lon=0&type=link%2C%20accurate&lat=0&units=imperial%2C%20metric HTTP/1.1\r\n"
                     "Host:  community-open-weather-map.p.rapidapi.com \r\n"
                     "Content-Type: application/x-www-form-urlencoded \r\n"
                     "x-rapidapi-key: 93d6478421mshcff35c0e1ff6140p1061cbjsnce6120e66263\r\n"
                     "x-rapidapi-host: community-open-weather-map.p.rapidapi.com\r\n"
                     "Connection: close\r\n"
                     "\r\n";

        char write_buf[2048] = "";
        strcat(write_buf, front);
        strcat(write_buf, str);
        strcat(write_buf, last); /*storing get request in write_buf*/

        SSL_write(ssl, write_buf, strlen(write_buf)); /* encrypt & send message */

        int size;
        for (;;)
        {
            //  Get chunks of the response 1024 at the time.
            bytes = SSL_read(ssl, buf, sizeof(buf)); /* get reply & decrypt */
            //  If no more data, then exit the loop
            if (bytes <= 0)
            {
                break;
            }

            parsejson(str, buf);

            buf[bytes] = 0; //  Terminate the string with a 0, to let know C when the string end.

            //printf("%s\n", buf); //  ->  Print out the response
            strcpy(buf, "");
        }

        SSL_free(ssl); /* release connection state */
    }
    close(server); /* close socket */
    exit(0);
}
/*opening the connection*/
int OpenConnection(const char *hostname, int port)
{
    int sd;
    struct hostent *host;
    struct sockaddr_in addr;
    if ((host = gethostbyname(hostname)) == NULL)
    {
        perror(hostname);
        abort();
    }
    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long *)(host->h_addr);
    if (connect(sd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        close(sd);
        perror(hostname);
        abort();
    }
    return sd;
}
