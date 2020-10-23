#include<openssl/ssl.h>
#include<openssl/err.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PRIVATE_KEY_PATH "./privkey.pem"
#define CERTIFICATION_PATH "./cacert.pem"

#define SSLVPN_PORT 60000
#define SSLVPN_IP   "127.0.0.1"

#define MAX_BUF_SIZE 1024
#define BACKLOG 10

typedef struct sockaddr sockaddr_t;

void sslvpn_ssl_init()
{
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
}

SSL_CTX *sslvpn_sslctx_create_and_conf(const char *privateKey, const char *certification)
{
    SSL_CTX *pCtx = SSL_CTX_new(SSLv23_client_method());
    if(NULL == pCtx)
    {
        return NULL;
    }

    if(SSL_CTX_use_certificate_file(pCtx, certification, SSL_FILETYPE_PEM) <= 0)
    {
        SSL_CTX_free(pCtx);
        return NULL;
    }

    if(SSL_use_PrivateKey_file(pCtx, privateKey, SSL_FILETYPE_PEM) <= 0)
    {
        SSL_CTX_free(pCtx);
        return NULL;
    }

    if(0 == SSL_check_private_key(pCtx))
    {
        SSL_CTX_free(pCtx);
        return NULL;
    }

    return pCtx;
}



int main()
{
    int sockfd, connectfd;
    socklen_t len;
    struct sockaddr_in serverAddr, clientAddr;
    unsigned int sslvpnPort;
    char buf[MAX_BUF_SIZE];
    SSL_CTX *sslCtx;
    

    sslvpn_ssl_init();
    sslvpn_sslctx_create_and_conf(PRIVATE_KEY_PATH, CERTIFICATION_PATH);

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if(-1 == sockfd)
    {
        printf("sockfd error!\n");
        exit(-1);
    }

    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SSLVPN_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SSLVPN_IP);

    if(-1 == bind(sockfd, (sockaddr_t *)(&serverAddr), sizeof(sockaddr_t)))
    {
        printf("bind error!\n");
        exit(-1);
    }

    if(-1 == listen(sockfd, BACKLOG))
    {
        printf("listen error!\n");
        exit(-1);
    }


    while (1)
    {
        SSL *ssl;
        len = sizeof(sockaddr_t);

        connectfd = accept(sockfd, (sockaddr_t *)(&clientAddr), &len);
        if(-1 == connectfd)
        {
            printf("accept error!\n");
            exit(-1);
        }

        ssl = SSL_new(sslCtx);
        SSL_set_fd(ssl, connectfd);

        if(-1 == SSL_accept(ssl))
        {
            printf("accept error!\n");
            exit(-1);
        }

        bzero(buf, MAX_BUF_SIZE);
        strcpy(buf, "dsfaahh");

        len = SSL_write(ssl, buf, strlen(buf+1));
        if(len <= 0)
        {
            printf("SSL_write_failed!\n");
            exit(-1);
        }

        len = SSL_read(ssl, buf, MAX_BUF_SIZE);
        if(len > 0)
        {
            printf("receive msg: %s\n", buf);
        }
        else
        {
            printf("SSL_read_failed!\n");
            exit(-1);
        }
        
    }
    
}