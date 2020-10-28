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
#define SSLVPN_IP "192.168.137.100"
#define BACKLOG 10
#define SSLVPN_TRUE 0x01
#define SSLVPN_FALSE 0x00
#define EVENT_MAX_NUM 20
#define HTTP_HEADER_MAX_SIZE 512
#define HTTP_BODY_MAX_SIZE 1024

//status
#define WAIT_READ_HTTPS_REQUEST_HEADER 5
#define WAIT_WRITE_HTTP_RESPONSE_HEADER 6
#define WAIT_WRITE_HTTP_RESPONSE_BODY 7

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

int sslvpn_set_nonblock(int fd, int value);

typedef struct Connection
{
    int fd_;
    SSL *ssl_;
    char sslConnected_;
    char isListenFd_;
    int events_;

    int status;
    char *https_request_header;
    int buffer_offset;

    char *URL_prefix;           /*  */
} Connection;

typedef struct Peer_Connection
{
    int fd_;
    char tcpConnected_;
    int events_;

    int status;
    char *http_response_header;
    char *http_response_body;
} Peer_Connection;

int g_conn_epollfd;
int g_peer_conn_epollfd;
SSL_CTX *g_ssl_ctx;

Connection *Connection_create(int fd, int events, char isListenFd)
{
    Connection *pConnection = (Connection *)malloc(sizeof(Connection));
    if (NULL == pConnection)
    {
        log("Connection memory alloc failed!\n");
        return NULL;
    }
    memset(pConnection, 0, sizeof(Connection));

    pConnection->fd_ = fd;
    pConnection->events_ = events;
    pConnection->isListenFd_ = isListenFd;
    pConnection->status = -1;

    return pConnection;
}

void Connection_delete(Connection *pConnection)
{
    if (NULL == pConnection)
    {
        return;
    }

    close(pConnection->fd_);
    if (NULL != pConnection->ssl_)
    {
        SSL_shutdown(pConnection->ssl_);
        SSL_free(pConnection->ssl_);
    }
    free(pConnection);
}

void Connection_update_event(Connection *pConnection)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = pConnection->events_;
    ev.data.ptr = pConnection;
    log("modifying fd %d events read %d write %d\n",
        pConnection->fd_, ev.events & EPOLLIN, ev.events & EPOLLOUT);
    int r = epoll_ctl(g_conn_epollfd, EPOLL_CTL_MOD, pConnection->fd_, &ev);
    check0(r, "epoll_ctl mod failed %d %s", errno, strerror(errno));
}

void sslvpn_tcp_listenfd_create(int sockfd, const char *ip, int port)
{
    socklen_t len;
    struct sockaddr_in my_addr;

    /*
    *未来会有添加多个socket监听的情况
    */
    if (-1 == (sockfd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0)))
    {
        perror("socket");
        return;
    }
    else
    {
        printf("socket created\n");
    }

    sslvpn_set_nonblock(sockfd, 1);

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

    if (-1 == bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)))
    {
        perror("bind");
        return;
    }
    else
    {
        printf("binded\n");
    }

    if (-1 == listen(sockfd, BACKLOG))
    {
        perror("listen");
        return;
    }
    else
    {
        printf("begin listen\n");
    }

    Connection *pConnection = Connection_create(sockfd, EPOLLIN, SSLVPN_TRUE);
    if (NULL == pConnection)
    {
        log("Channal create failed!\n");
        return;
    }

    sslvpn_epollctl_add_conn(g_conn_epollfd, pConnection);
}

void sslvpn_ssl_try_accept(Connection *pConn)
{
    int res;

    if (NULL == pConn->ssl_)
    {
        pConn->ssl_ = SSL_new(g_ssl_ctx);
        if (NULL == pConn->ssl_)
        {
            log("ssl create failed because of memory lack!\n");
            return;
        }

        res = SSL_set_fd(pConn->ssl_, pConn->fd_);
        if (res <= 0)
        {
            log("ssl set fd failed!\n");
            SSL_free(pConn->ssl_);
            pConn->ssl_ = NULL;
            return;
        }
    }

    res = SSL_accept(pConn->ssl_);
    if (1 == res)
    {
        pConn->sslConnected_ = SSLVPN_TRUE;
        log("fd %d ssl connected!\n", pConn->fd_);
        pConn->events_ = EPOLLIN | EPOLLRDHUP;
        Connection_update_event(pConn);
        pConn->status = WAIT_READ_HTTPS_REQUEST_HEADER;
    }
    else
    {
        log("try accept failed!\n");
    }
}

void sslvpn_handle_accept(int sockfd)
{
    struct sockaddr_in their_addr;
    socklen_t len;
    len = sizeof(struct sockaddr);
    int new_fd;

    while (!((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &len)) <= 0 && EAGAIN == errno))
    {
        if (new_fd <= 0)
        {
            continue;
        }

        sslvpn_set_nonblock(new_fd, 1);
        printf("server: got connection from %s, port %d, socket %d\n",
               inet_ntoa(their_addr.sin_addr),
               ntohs(their_addr.sin_port),
               new_fd);

        Connection *pConnection = Connection_create(new_fd, EPOLLIN | EPOLLOUT | EPOLLRDHUP, SSLVPN_FALSE);
        if (NULL == pConnection)
        {
            log("Create Connection failed because of memory lack!\n");
            return;
        }

        sslvpn_ssl_try_accept(pConnection);

        sslvpn_epollctl_add_conn(g_conn_epollfd, pConnection);
    }
}

int sslvpn_set_nonblock(int fd, int value)
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

void sslvpn_epollctl_add_conn(int g_conn_epollfd, Connection *pConnection)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = pConnection->events_;
    ev.data.ptr = pConnection;
    int r = epoll_ctl(g_conn_epollfd, EPOLL_CTL_ADD, pConnection->fd_, &ev);
    if (0 != r)
    {
        log("adding fd %d failed!\n", pConnection->fd_);
    }
    else
    {
        log("adding fd %d events %d\n", pConnection->fd_, ev.events);
    }
}

void sslvpn_epollctl_del_conn(int g_conn_epollfd, Connection *pConn)
{
    struct epoll_event ee;
    int res = epoll_ctl(g_conn_epollfd, EPOLL_CTL_DEL, pConn->fd_, &ee);
    if (0 == res)
    {
        log("delete fd %d\n", pConn->fd_);
    }
    else
    {
        log("delete fd %d failed!\n", pConn->fd_);
    }
}

SSL_CTX *sslvpn_ssl_init(const char *privateKeyPath, const char *certPath)
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

void sslvpn_handle_conn_read_events(Connection *pConn, char *readBuf, int bufSize)
{
    char *buf;
    int readLen;
    if (SSLVPN_FALSE == pConn->sslConnected_)
    {
        sslvpn_ssl_try_accept(pConn);
    }
    else if (WAIT_READ_HTTPS_REQUEST_HEADER == pConn)
    {
        readLen = SSL_read(pConn->ssl_, readBuf, bufSize);
        if (readLen > 0 && readLen < bufSize)
        {
            //https头部在readBuf中，需要对其进行处理

            //创建peer_connection和其socketfd，从处理过后的请求头中读取真正的URL，进行connect；建立映射
            

            //处理完之后,拷贝到pConn的缓冲区中

            pConn->status = WAIT_WRITE_HTTP_RESPONSE_HEADER;
            pConn->events_ = EPOLLOUT|EPOLLRDHUP;
            Connection_update_event(pConn);
        }
        else
        {
            // sslvpn_epollctl_del_conn(g_conn_epollfd, pConn);
            // Connection_delete(pConn);
            return;
        }
    }
    else
    {
        
    }
}

void sslvpn_handle_conn_write(Connection *pConn)
{

}

void sslvpn_handle_conn_events(int waitms, char *buf, int bufSize)
{
    int i;
    struct epoll_event activeEvents[EVENT_MAX_NUM];
    struct epoll_event ee;
    int eventNum = epoll_wait(g_conn_epollfd, activeEvents, EVENT_MAX_NUM, waitms);
    for (i = 0; i < eventNum; i++)
    {
        Connection *pConn = (Connection *)activeEvents[i].data.ptr;
        int event = activeEvents[i].events;

        if (SSLVPN_TRUE == pConn->isListenFd_ && (event & EPOLLIN))
        {
            //listenfd还有可能是端口映射
            sslvpn_handle_accept(pConn->fd_);
        }
        else if ((event & EPOLLRDHUP) || (event & EPOLLERR))
        {
            epoll_ctl(g_conn_epollfd, EPOLL_CTL_DEL, pConn->fd_, &ee);
            log("fd %d unconnect!\n");
            Connection_delete(pConn);
        }
        else if (event & EPOLLIN)
        {
            if (SSLVPN_FALSE == pConn->sslConnected_)
            {
                sslvpn_ssl_try_accept(pConn);
            }
            else
            {
                char a[1024];
                int len = SSL_read(pConn->ssl_, a, 1024);
                if (len > 0)
                {
                    log("recv msg: %s", a);
                }
                else
                {
                    log("recv no msg: \n");
                }
            }
        }
        else if (event & EPOLLOUT)
        {
            if (SSLVPN_FALSE == pConn->sslConnected_)
            {
                sslvpn_ssl_try_accept(pConn);
            }
            else
            {
                SSL_write(pConn->ssl_, "dfaaa", 6);
            }
        }
    }
}

int main()
{
    int sockfd, client_fd;
    int len;
    char serverbuf[MAXSIZE];

    g_ssl_ctx = sslvpn_ssl_init(PRIVATE_KEY_FILE, CERTIFICATION_FILE);
    check0(NULL == g_ssl_ctx, "create SSL_CTX failed!\n");

    g_conn_epollfd = epoll_create(10);
    g_peer_conn_epollfd = epoll_create(10);

    // signal(SIGPIPE, SIG_IGN);
    sslvpn_tcp_listenfd_create(sockfd, SSLVPN_IP, SSLVPN_PORT);
    while (1)
    {
        sslvpn_handle_conn_events(50, serverbuf, MAXSIZE);
    }
    close(sockfd);
    SSL_CTX_free(g_ssl_ctx);
    return 0;
} //main