// #include <openssl/ssl.h>
// #include <openssl/err.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <errno.h>
// #include <string.h>
// #include <sys/types.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <fcntl.h>
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


// int main()
// {
//     int sockfd, connectRes;
//     socklen_t len;
//     struct sockaddr_in serverAddr, clientAddr;
//     unsigned int sslvpnPort;
//     char buf[MAX_BUF_SIZE];
//     SSL_CTX *sslCtx;
//     SSL *ssl;

//     sslvpn_ssl_init();
//     sslCtx = SSL_CTX_new(SSLv23_client_method());
//     if (NULL == sslCtx)
//     {
//         return -1;
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

//     connectRes = connect(sockfd, (sockaddr_t *)(&serverAddr), sizeof(serverAddr));
//     if(0 != connectRes)
//     {
//         printf("TCP connect to server failed!\n");
//         exit(-1);
//     }

//     int flags = fcntl (sockfd, F_GETFL); 
//     if (flags & O_NONBLOCK)
//     {
//         //fcntl (sockfd, F_SETFL, flags-O_NONBLOCK);
//         fcntl(sockfd,F_SETFL,flags&(~O_NONBLOCK));
//     }

//     ssl = SSL_new(sslCtx);
//     if(NULL == ssl)
//     {
//         printf("Memory loss, can't apply ssl!\n");
//         exit(-1);
//     }

//     int sslsetfd = SSL_set_fd(ssl, sockfd);

//     int sslconnect = SSL_connect(ssl);

//     strcpy(buf, "114514\0");
//     SSL_write(ssl, buf, MAX_BUF_SIZE);

//     SSL_read(ssl, buf, MAX_BUF_SIZE);
//     printf("%s", buf);




//     SSL_shutdown(ssl);
//     SSL_free(ssl);
//     close(sockfd);
//     SSL_CTX_free(sslCtx);
// }

    #include <stdio.h>
    #include <stdlib.h>
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

    void ShowCerts(SSL * ssl)
    {
        X509 *cert;
        char *line;
     
        cert = SSL_get_peer_certificate(ssl);
        if (cert != NULL)
        {
            printf("数字证书信息:\n");
            line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
            printf("证书: %s\n", line);
            free(line);
            line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
            printf("颁发者: %s\n", line);
            free(line);
            X509_free(cert);
        }
        else
            printf("无证书信息！\n");
    }
    int main()
    {
        char *hostname="127.0.0.1";
        int sockfd, len;
        char clientbuf[MAXSIZE];
        struct hostent *host;//gethostbyname函数的参数返回
        struct sockaddr_in serv_addr;
        SSL_CTX *ctx;
        SSL *ssl;
       
        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();
        ctx = SSL_CTX_new(SSLv23_client_method());
        if (ctx == NULL)
        {
            ERR_print_errors_fp(stdout);
            exit(1);
        }
        int chose=0;
        printf("Please Chose Channel No.:\n");
        printf("1.:SSL Protocol Channel\n");
        printf("2.:TCP Protocol Channel\n");
        scanf("%d",&chose);
        serv_addr=tcpclient_init(&sockfd);
        if(chose==1)
        {
            tcp_connect(&sockfd,serv_addr);
           
            ssl = SSL_new(ctx);
            SSL_set_fd(ssl, sockfd);
           
            if (SSL_connect(ssl) == -1)
                ERR_print_errors_fp(stderr);
            else
            {
                printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
                ShowCerts(ssl);
            }
      
            bzero(clientbuf, MAXSIZE);
            strcpy(clientbuf, "id:am3517&pw:am3517\n");
            len = SSL_write(ssl, clientbuf, strlen(clientbuf));
            if (len < 0)
                printf("消息'%s'发送失败！错误代码是%d，错误信息是'%s'\n",clientbuf, errno, strerror(errno));
            else
                printf("消息'%s'发送成功，共发送了%d个字节！\n",clientbuf, len);
     
            SSL_shutdown(ssl);
            SSL_free(ssl);
        }
        else if(chose==2)
        {
            tcp_connect(&sockfd,serv_addr);
           
            bzero(clientbuf, MAXSIZE);
            strcpy(clientbuf, "id:am3517&pw:am3517\n");
            len = send(sockfd, clientbuf, strlen(clientbuf),0);
            if (len < 0)
                printf("消息'%s'发送失败！错误代码是%d，错误信息是'%s'\n",clientbuf, errno, strerror(errno));
            else
                printf("消息'%s'发送成功，共发送了%d个字节！\n",clientbuf, len);
        }
        close(sockfd);
        SSL_CTX_free(ctx);
        return 0;
    }