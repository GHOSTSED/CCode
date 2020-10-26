// #include <openssl/ssl.h>
// #include <openssl/err.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <errno.h>
// #include <string.h>
// #include <sys/types.h>
// #include <netinet/in.h>
// #include <fcntl.h>
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

//     if (SSL_CTX_use_PrivateKey_file(pCtx, privateKey, SSL_FILETYPE_PEM) <= 0)
//     {
//         SSL_CTX_free(pCtx);
//         return NULL;
//     }

//     if (0 == SSL_CTX_check_private_key(pCtx))
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
//     if (NULL == sslCtx)
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
//     len = sizeof(sockaddr_t);

//     connectfd = accept(sockfd, (sockaddr_t *)(&clientAddr), &len);
//     if (-1 == connectfd)
//     {
//         printf("accept error!\n");
//         exit(-1);
//     }
//     printf("TCP link established from %s!\n", inet_ntoa(clientAddr.sin_addr));

//     int flags = fcntl(sockfd, F_GETFL);
//     if (flags & O_NONBLOCK)
//     {
//         //fcntl (sockfd, F_SETFL, flags-O_NONBLOCK);
//         fcntl(sockfd, F_SETFL, flags & (~O_NONBLOCK));
//     }

//     flags = fcntl(connectfd, F_GETFL);
//     if (flags & O_NONBLOCK)
//     {
//         //fcntl (sockfd, F_SETFL, flags-O_NONBLOCK);
//         fcntl(connectfd, F_SETFL, flags & (~O_NONBLOCK));
//     }

//     ssl = SSL_new(sslCtx);

//     SSL_set_fd(ssl, connectfd);

//     if (-1 == SSL_accept(ssl))
//     {
//         printf("accept error!\n");
//         exit(-1);
//     }
//     printf("SSL session established from %s!\n", inet_ntoa(clientAddr.sin_addr));

//     bzero(buf, MAX_BUF_SIZE);
//     len = SSL_read(ssl, buf, MAX_BUF_SIZE);
//     if (len > 0)
//     {
//         printf("receive msg: %s\n", buf);
//     }
//     else
//     {
//         printf("SSL_read_failed!\n");
//         exit(-1);
//     }

//     strcpy(buf, "server received your msg!\0");
//     len = SSL_write(ssl, buf, MAX_BUF_SIZE);
//     if (len <= 0)
//     {
//         printf("SSL_write_failed!\n");
//         exit(-1);
//     }
//     // }

//     SSL_shutdown(ssl);
//     SSL_free(ssl);
//     close(connectfd);
//     close(sockfd);
// }

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <sys/epoll.h>
#include <signal.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define MAXSIZE 1024 //每次最大数据传输量

#define PRIVATE_KEY_FILE "privkey.pem"
#define CERTIFICATION_FILE "cacert.pem"
#define SSLVPN_PORT 60000
#define SSLVPN_IP "127.0.0.1"
#define BACKLOG 10
#define SSLVPN_TRUE 0xff
#define SSLVPN_FALSE 0x00

#define log(...)             \
    do                       \
    {                        \
        printf(__VA_ARGS__); \
        fflush(stdout);      \
    } while (0)
#define check0(x, ...)        \
    if (x)                    \
        do                    \
        {                     \
            log(__VA_ARGS__); \
            exit(1);          \
    } while (0)
#define check1(x, ...)        \
    if (!x)                   \
        do                    \
        {                     \
            log(__VA_ARGS__); \
            exit(1);          \
    } while (0)

int setNonBlock(int fd, int value);

typedef struct Channel
{
    int fd_;
    SSL *ssl_;
    // char tcpConnected_;
    char sslConnected_;
    char isListenFd_;
    int events_;
} Channel;

int epollfd;

Channel *Channel_create(int fd, int events, char isListenFd)
{
    Channel *pChannel = (Channel *)malloc(sizeof(Channel));
    if (NULL == pChannel)
    {
        log("Channel memory alloc failed!\n");
        return NULL;
    }
    memset(pChannel, 0, sizeof(Channel));

    pChannel->fd_ = fd;
    pChannel->events_ = events;
    pChannel->isListenFd_ = isListenFd;
    return pChannel;
}

void Channel_delete(Channel *pChannel)
{
    if (NULL == pChannel)
    {
        return;
    }

    close(pChannel->fd_);
    if (NULL != pChannel->ssl_)
    {
        SSL_shutdown(pChannel->ssl_);
        SSL_free(pChannel->ssl_);
    }
    free(pChannel);
}

void Channel_update_event(Channel *pChannel)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = pChannel->events_;
    ev.data.ptr = pChannel;
    log("modifying fd %d events read %d write %d\n",
        pChannel->fd_, ev.events & EPOLLIN, ev.events & EPOLLOUT);
    int r = epoll_ctl(epollfd, EPOLL_CTL_MOD, pChannel->fd_, &ev);
    check0(r, "epoll_ctl mod failed %d %s", errno, strerror(errno));
}

void tcp_listenfd_create(int *sockfd, const char *ip, int port)
{
    socklen_t len;
    struct sockaddr_in my_addr;

    /*
    *未来会有添加多个socket监听的情况
    */
    if (-1 == (*sockfd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0)))
    {
        perror("socket");
        return;
    }
    else
    {
        printf("socket created\n");
    }

    setNonBlock(sockfd, 1);

    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);

    if (NULL == ip)
    {
        my_addr.sin_addr.s_addr = INADDR_ANY;
    }
    else
    {
        my_addr.sin_addr.s_addr = inet_addr(ip);
    }

    if (-1 == bind(*sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)))
    {
        perror("bind");
        return;
    }
    else
    {
        printf("binded\n");
    }

    if (-1 == listen(*sockfd, BACKLOG))
    {
        perror("listen");
        return;
    }
    else
    {
        printf("begin listen\n");
    }

    Channel *pChannel = Channel_create(sockfd, EPOLLIN|EPOLLET , SSLVPN_TRUE);
    if (NULL == pChannel)
    {
        log("Channal create failed!\n");
        return;
    }

    addEpollFd(epollfd, pChannel);
}

void accept_handle(int sockfd)
{
    struct sockaddr_in their_addr;
    socklen_t len;
    len = sizeof(struct sockaddr);
    int new_fd;

    while (!((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &len)) <= 0 && EAGAIN == errno))
    {
        printf("server: got connection from %s, port %d, socket %d\n",
               inet_ntoa(their_addr.sin_addr),
               ntohs(their_addr.sin_port),
               new_fd);

        setNonBlock(new_fd, 1);
        Channel *pChannel = Channel_create(new_fd, EPOLLIN|EPOLLET, SSLVPN_FALSE);
        if (NULL == pChannel)
        {
            log("Create channel failed because of memory lack!\n");
            return;
        }
        addEpollFd(epollfd, pChannel);
    }
}

int setNonBlock(int fd, int value)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
    {
        return errno;
    }
    if (value > 0)
    {
        return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }
    return fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
}

void addEpollFd(int epollfd, Channel *pChannel)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = pChannel->events_;
    ev.data.ptr = pChannel;
    log("adding fd %d events %d\n", pChannel->fd_, ev.events);
    int r = epoll_ctl(epollfd, EPOLL_CTL_ADD, pChannel->fd_, &ev);
    check0(r, "epoll_ctl add failed %d %s", errno, strerror(errno));
}

SSL_CTX *ssl_init(const char *privateKeyPath, const char *certPath)
{
    SSL_CTX *sslCtx;

    ERR_load_BIO_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    sslCtx = SSL_CTX_new(SSLv23_server_method());
    if (sslCtx == NULL)
    {
        ERR_print_errors_fp(stdout);
        return NULL;
    }
    if (SSL_CTX_use_certificate_file(sslCtx, certPath, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stdout);
        return NULL;
    }
    if (SSL_CTX_use_PrivateKey_file(sslCtx, privateKeyPath, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stdout);
        return NULL;
    }
    if (SSL_CTX_check_private_key(sslCtx) <= 0)
    {
        ERR_print_errors_fp(stdout);
        return NULL;
    }

    return sslCtx;
}

void handleHandShake(Channel *ch)
{

}

int main()
{
    int sockfd, client_fd;
    int len;
    SSL_CTX *ssl_ctx;
    char serverbuf[MAXSIZE];

    ssl_ctx = ssl_init(PRIVATE_KEY_FILE, CERTIFICATION_FILE);
    check0(NULL == ssl_ctx, "create SSL_CTX failed!\n");

    signal(SIGPIPE, SIG_IGN);
    tcp_listenfd_create(&sockfd, SSLVPN_IP, SSLVPN_PORT);
    while (1)
    {
        SSL *ssl;
        accept_handle(sockfd, &client_fd);
        ssl = SSL_new(ssl_ctx);
        SSL_set_fd(ssl, client_fd);
        if (SSL_accept(ssl) == -1)
        {
            perror("accept");
            close(client_fd);
            break;
        }
        // 接收消息
        bzero(serverbuf, MAXSIZE);

        len = SSL_read(ssl, serverbuf, MAXSIZE);
        if (len > 0)
        {
            printf("接收消息成功:'%s'，共%d个字节的数据\n", serverbuf, len);
        }
        else
        {
            printf("消息接收失败！错误代码是%d，错误信息是'%s'\n", errno, strerror(errno));
        }

        strcpy(serverbuf, "server has received your message!");
        len = SSL_write(ssl, serverbuf, strlen(serverbuf));
        if (len > 0)
        {
            printf("信息发送成功！\n");
        }
        else
        {
            printf("消息发送失败！错误代码是%d，错误信息是'%s'\n", errno, strerror(errno));
        }

        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client_fd);
    }
    close(sockfd);
    SSL_CTX_free(ssl_ctx);
    return 0;

} //main