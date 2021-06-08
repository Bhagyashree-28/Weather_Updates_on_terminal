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
#define FAIL -1
#define HOST "community-open-weather-map.p.rapidapi.com"

SSL_CTX *InitCTX(void)
{
    SSL_METHOD *method;
    SSL_CTX *ctx;
    OpenSSL_add_all_algorithms();     /* Load cryptos, et.al. */
    SSL_load_error_strings();         /* Bring in and register error messages */
    method = TLSv1_2_client_method(); /* Create new client-method instance */
    ctx = SSL_CTX_new(method);        /* Create new context */
    if (ctx == NULL)
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

void ShowCerts(SSL *ssl)
{
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if (cert != NULL)
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line); /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);      /* free the malloc'ed string */
        X509_free(cert); /* free the malloc'ed certificate copy */
    }
    else
        printf("Info: No client certificates configured.\n");
}
int main(int argc, char *argv[])
{
    SSL_CTX *ctx;
    SSL *ssl;

    char acClientRequest[1024] = {0};
    SSL_library_init();

    ctx = InitCTX();
    //taking multiple city input using command line
    char str[argc - 1][20];
    for (int i = 1; i < argc; i++)
    {
        strcpy(str[i - 1], argv[i]);
    }
    //creating one parent and multiple child
    pid_t pids[argc - 1];

    for (int i = 0; i < argc - 1; i++)
    {

        if ((pids[i] = fork()) == 0)
        {
            apiresponse(str[i], ctx, ssl);
        }
    }
    /*parent waiting for each child*/
    for (int i = 0; i < argc - 1; i++)
    {
        int status;
        int childpid = waitpid(pids[i], &status, 0);
        // printf("child id %lu with parent %lu\n",pids[i],getppid());
    }

    SSL_CTX_free(ctx); /* release context */
    return 0;
}