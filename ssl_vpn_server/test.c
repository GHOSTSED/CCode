// #include <openssl/ssl.h>
// #include <openssl/err.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <errno.h>
// #include <string.h>
// #include <sys/types.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <sys/wait.h>
// #include <unistd.h>
// #include <arpa/inet.h>

// #define PRIVATE_KEY_PATH "./privkey.pem"
// #define CERTIFICATION_PATH "./cacert.pem"

// #define SSLVPN_PORT 60000
// #define SSLVPN_IP "127.0.0.1"

// #define MAX_BUF_SIZE 128
// #define BACKLOG 10

// typedef struct sockaddr sockaddr_t;

// void sslvpn_ssl_init()
// {
//     SSL_library_init();
//     OpenSSL_add_all_algorithms();
//     SSL_load_error_strings();
// }

// SSL_CTX *sslvpn_sslctx_create_and_conf(const char *privateKey, const char *certification)
// {
//     SSL_CTX *pCtx = SSL_CTX_new(SSLv23_client_method());
//     if (NULL == pCtx)
//     {
//         return NULL;
//     }

//     if (SSL_CTX_use_certificate_file(pCtx, certification, SSL_FILETYPE_PEM) <= 0)
//     {
//         SSL_CTX_free(pCtx);
//         return NULL;
//     }

//     if(SSL_CTX_use_PrivateKey_file(pCtx, privateKey, SSL_FILETYPE_PEM) <= 0)
//     {
//         SSL_CTX_free(pCtx);
//         return NULL;
//     }

//     if(0 == SSL_CTX_check_private_key(pCtx))
//     {
//         SSL_CTX_free(pCtx);
//         return NULL;
//     }

//     return pCtx;
// }

// int main()
// {
//     int sockfd, connectfd;
//     socklen_t len;
//     struct sockaddr_in serverAddr, clientAddr;
//     unsigned int sslvpnPort;
//     char buf[MAX_BUF_SIZE];
//     SSL_CTX *sslCtx;
//     SSL *ssl;


//     sslvpn_ssl_init();
//     sslCtx = sslvpn_sslctx_create_and_conf(PRIVATE_KEY_PATH, CERTIFICATION_PATH);
//     if(NULL == sslCtx)
//     {
//         printf("SSL_CTX create failed!\n");
//         exit(-1);
//     }

//     sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if (-1 == sockfd)
//     {
//         printf("sockfd error!\n");
//         exit(-1);
//     }

//     bzero(&serverAddr, sizeof(serverAddr));
//     serverAddr.sin_family = AF_INET;
//     serverAddr.sin_port = htons(SSLVPN_PORT);
//     serverAddr.sin_addr.s_addr = inet_addr(SSLVPN_IP);

//     if (-1 == bind(sockfd, (sockaddr_t *)(&serverAddr), sizeof(sockaddr_t)))
//     {
//         printf("bind error!\n");
//         exit(-1);
//     }

//     if (-1 == listen(sockfd, BACKLOG))
//     {
//         printf("listen error!\n");
//         exit(-1);
//     }

//     // while (1)
//     // {
//         len = sizeof(sockaddr_t);

//         connectfd = accept(sockfd, (sockaddr_t *)(&clientAddr), &len);
//         if (-1 == connectfd)
//         {
//             printf("accept error!\n");
//             exit(-1);
//         }
//         printf("TCP link established from %s!\n", inet_ntoa(clientAddr.sin_addr));


//         ssl = SSL_new(sslCtx);

//         SSL_set_fd(ssl, connectfd);

//         if (-1 == SSL_accept(ssl))
//         {
//             printf("accept error!\n");
//             exit(-1);
//         }
//         printf("SSL session established from %s!\n", inet_ntoa(clientAddr.sin_addr));


//         bzero(buf, MAX_BUF_SIZE);
//         len = SSL_read(ssl, buf, MAX_BUF_SIZE);
//         if (len > 0)
//         {
//             printf("receive msg: %s\n", buf);
//         }
//         else
//         {
//             printf("SSL_read_failed!\n");
//             exit(-1);
//         }

//         strcpy(buf, "server received your msg!\0");
//         len = SSL_write(ssl, buf, MAX_BUF_SIZE);
//         if (len <= 0)
//         {
//             printf("SSL_write_failed!\n");
//             exit(-1);
//         }
//     // }

//     SSL_shutdown(ssl);
//     SSL_free(ssl);
//     close(connectfd);
//     close(sockfd);
// }

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <resolv.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
 
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#define MAXSIZE 1024 //每次最大数据传输量
 
#define PKEY_FILE "privkey.pem"
#define CERT_FILE "cacert.pem"

void tcpserver_init(int *sockfd)
{
    socklen_t len;
    struct sockaddr_in my_addr;
    unsigned int myport, lisnum;
 
    myport = 7838;
    lisnum = 1;
 
    if ((*sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }
    else
        printf("socket created\n");
 
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = PF_INET;
    my_addr.sin_port = htons(myport);
    my_addr.sin_addr.s_addr = INADDR_ANY;
 
    if(bind(*sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }
    else
        printf("binded\n");
 
    if (listen(*sockfd, lisnum) == -1)
    {
        perror("listen");
        exit(1);
    }
    else
        printf("begin listen\n");
}
void tcp_accept(int sockfd,int *new_fd)
{
    struct sockaddr_in their_addr;
    socklen_t len;
    len = sizeof(struct sockaddr);
    if ((*new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &len)) == -1)
    {
        perror("accept");
        exit(errno);
    }
    else
        printf("server: got connection from %s, port %d, socket %d\n",inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port), *new_fd);
    int flags;
    flags = fcntl (sockfd, F_GETFL); 
    if (flags & O_NONBLOCK)
    {
        //fcntl (sockfd, F_SETFL, flags-O_NONBLOCK);
        fcntl(sockfd,F_SETFL,flags&(~O_NONBLOCK));
    }
}
struct sockaddr_in tcpclient_init(int *sockfd)
{
    int len;
    struct sockaddr_in dest;
 
    char parainfo[3][20];
    printf("input server IP:\n");
    scanf("%s",parainfo[0]);
    printf("input server port:\n");
    scanf("%s",parainfo[1]);
    if((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket");
        exit(errno);
    }
    printf("socket created\n");
 
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(atoi(parainfo[1]));
    if (inet_aton(parainfo[0], (struct in_addr *) &dest.sin_addr.s_addr) == 0)
    {
        perror(parainfo[0]);
        exit(errno);
    }
    printf("address created\n");
    return dest;
}
void tcp_connect(int *sockfd,struct sockaddr_in dest)
{
    if (connect(*sockfd, (struct sockaddr *) &dest, sizeof(dest)) != 0)
    {
        perror("Connect ");
        exit(errno);
    }
    printf("server connected\n");
    int flags;
    flags = fcntl (sockfd, F_GETFL); 
    if (flags & O_NONBLOCK)
    {
        //fcntl (sockfd, F_SETFL, flags-O_NONBLOCK);
        fcntl(sockfd,F_SETFL,flags&(~O_NONBLOCK));
    }
}

int main()
{
    int sockfd,client_fd;
    socklen_t len;
 
    SSL_CTX *ctx;
  
    char serverbuf[MAXSIZE];
 
    ERR_load_BIO_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(SSLv23_server_method());
    if (ctx == NULL)
    {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    if (!SSL_CTX_use_certificate_file(ctx, CERT_FILE, SSL_FILETYPE_PEM))
    {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    if (!SSL_CTX_use_PrivateKey_file(ctx, PKEY_FILE, SSL_FILETYPE_PEM) )
    {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    if (!SSL_CTX_check_private_key(ctx))
    {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    int chose=0;
    signal(SIGPIPE,SIG_IGN);
    tcpserver_init(&sockfd);
    while(1)
    {
        printf("Please Chose Channel No.:\n");
        printf("1.:SSL Protocol Channel\n");
        printf("2.:TCP Protocol Channel\n");
        scanf("%d",&chose);
        if(chose==1)
        {
            SSL *ssl;
            tcp_accept(sockfd,&client_fd);
            ssl = SSL_new(ctx);
            SSL_set_fd(ssl, client_fd);
            if (SSL_accept(ssl) == -1)
            {
                perror("accept");
                close(client_fd);
                break;
            }
            // 接收消息
            bzero(serverbuf, MAXSIZE);
           
            len = SSL_read(ssl,serverbuf, MAXSIZE);
            if (len > 0)
                printf("接收消息成功:'%s'，共%d个字节的数据\n",serverbuf, len);
            else
                printf("消息接收失败！错误代码是%d，错误信息是'%s'\n",errno, strerror(errno));
           
            SSL_shutdown(ssl);
            SSL_free(ssl);
            close(client_fd);
        }
        else if(chose==2)
        {
            tcp_accept(sockfd,&client_fd);
            len=recv(client_fd,serverbuf, MAXSIZE,0);
            if (len > 0)
                printf("接收消息成功:'%s'，共%d个字节的数据\n",serverbuf, len);
            else
                printf("消息接收失败！错误代码是%d，错误信息是'%s'\n",errno, strerror(errno));
            close(client_fd);
        }
        chose=0;
    }
    close(sockfd);
    SSL_CTX_free(ctx);
    return 0;
 
}//main