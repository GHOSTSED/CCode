/*************************************************************************************************************************************/
/*												  INCLUDE_FILES																		 */
/*************************************************************************************************************************************/
#include "../include/DMailListener.h"
#include <pthread.h>

/*************************************************************************************************************************************/
/*												     TYPES																		     */
/*************************************************************************************************************************************/
/* 用于装载发送方视角下的邮件信息 */
typedef struct _SendListItem
{
    unsigned char receiverAddress[EMAIL_ADDRESS_MAX_LENGTH];/* 收件人邮箱 */
    unsigned long int size;                                 /* 邮件大小 */
    unsigned char sendTime[TIME_MAX_LENGTH];                /* 发送时间 */
} SendListItem;

/* 用于装载接收方视角下的邮件信息 */
typedef struct _ReceiveListItem
{
    unsigned char senderAddress[EMAIL_ADDRESS_MAX_LENGTH];  /* 发件人邮箱 */
    unsigned long int size;                                 /* 邮件大小 */
    unsigned char receiveTime[TIME_MAX_LENGTH];             /* 发送时间 */
} ReceiveListItem;

/* 用于装载一名用户收到的和发出的所有邮件的信息 */
typedef struct _EmailTableItem
{
    unsigned char userAddress[EMAIL_ADDRESS_MAX_LENGTH];    /* 用户的邮箱地址，将作为hash的key使用 */
    DList *sendList;                                        /* 该用户发送的所有邮件 */
    DList *receiveList;                                     /* 该用户收到的所有邮件 */
} EmailTableItem;

/* 用于装载四元组 */
typedef struct _TcpLinkKey
{
    unsigned char srcIP[INET_ADDRSTRLEN];   /* 源IP */
    unsigned char dstIP[INET_ADDRSTRLEN];   /* 目的IP */

    unsigned int srcPort;                   /* 源端口 */
    unsigned int dstPort;                   /* 目的端口 */
} TcpLinkKey;

/* TCP连接表中收集到的邮件信息 */
typedef struct _EmailInfoValue
{
    unsigned char receiverAddress[EMAIL_ADDRESS_MAX_LENGTH];    /* 收件人地址 */
    unsigned char senderAddress[EMAIL_ADDRESS_MAX_LENGTH];      /* 发件人地址 */

    unsigned long int size;                                     /* 邮件大小 */
    unsigned char time[TIME_MAX_LENGTH];                        /* 邮件发送时间或接收时间 */

    unsigned long int startSeq;                                 /* SMTP协议下邮件数据开始的序列号 */
    unsigned long int Ack;                                      /* POP协议下第一个邮件数据分片的序列号 */
    int state;                                                  /* 邮件解析所处状态 */
} EmailInfoValue;

/* 用于存储某个TCP连接情况 */
typedef struct _TcpTableItem
{
    TcpLinkKey key;        /* 四元组作为key */                             
    EmailInfoValue value;  /* 邮件的相关信息作为value */
} TcpTableItem;

/* 邮件监听器对象需要维护的信息 */
typedef struct _DMailListener
{
    DHashTable *emailTable;     /* 用于存储所有邮件信息的hash表，key为用户的邮箱，value为两个链表，其一
                                   为该用户发送的所有邮件的信息列表sendList；其二为该用户所有收到的邮件的信
                                   息列表receiveList */
    DHashTable *tcpTable;       /* 用于存储所有与邮件服务器建立的TCP连接的hash表，key为 */

    pthread_mutex_t emailTableLock;
    pthread_mutex_t tcpTableLock;
} DMailListener;


/*************************************************************************************************************************************/
/*												   FUNCTIONS																		 */
/*************************************************************************************************************************************/

/*
*@fn							    static void dmailListener_callback_print_sendListItem(const void *vSendListItem);
*@brief   						    回调函数，用于打印发件人视角下的邮件信息
*@details	
*
*@param[in] const void* vSendListItem	指向要打印的SendListItem对象的指针     
*
*@return						        N/A
*@retval
*/
static void dmailListener_callback_print_sendListItem(const void *vSendListItem);

/*
*@fn							    static void dmailListener_callback_print_receiveListItem(const void *vReceiveListItem);
*@brief   						    回调函数，用于打印收件人视角下的邮件信息
*@details	
*
*@param[in] const void* vReceiveListItem	指向要打印的ReceiveListItem对象的指针     
*
*@return						            N/A
*@retval
*/
static void dmailListener_callback_print_receiveListItem(const void *vReceiveListItem);

/*
*@fn							    static void dmailListener_callback_print_userInfo(const void *vEmailTableItem);
*@brief   						    回调函数，用于打印用户信息（包括用户邮箱、发件列表、收件列表）
*@details	
*
*@param[in] const void* vEmailTableItem	    指向要打印的vEmailTableItem对象的指针     
*
*@return						            N/A
*@retval
*/
static void dmailListener_callback_print_userInfo(const void *vEmailTableItem);

/*
*@fn							    static EmailTableItem *dmailListener_EmailTableItem_init(unsigned char *emailAddr);
*@brief   						    用于对EmailTableItem进行初始化操作
*@details	
*
*@param[in] unsigned char* emailAddr	    要构建的用户的邮箱地址    
*
*@return						            若初始化成功，返回一个指向EmailTableItem对象的指针；否则，返回NULL
*@retval
*/
static EmailTableItem *dmailListener_EmailTableItem_init(unsigned char *emailAddr);

/*
*@fn							    static void dmailListener_calback_EmailTableItem_delete(void *vEmailTableItem);
*@brief   						    用于对EmailTableItem进行销毁操作，同时也作为销毁EmailTable时使用的回调函数而存在
*@details	
*
*@param[in] void *vEmailTableItem	    指向要销毁的EmailTableItem对象的指针   
*
*@return						        N/A
*@retval
*/
static void dmailListener_calback_EmailTableItem_delete(void *vEmailTableItem);

/*
*@fn							    static SendListItem *dmailListener_SendListItem_init(const EmailInfoValue *pEmailInfo);
*@brief   						    用于对SendListItem对象进行初始化操作，从TCP连接采集到的信息构件对象
*@details	
*
*@param[in] const EmailInfoValue* pEmailInfo	    根据pEmailInfo中的信息构建SendListItem   
*
*@return						                    若初始化成功，返回构建出的SendListItem对象的指针；否则，返回NULL
*@retval
*/
static SendListItem *dmailListener_SendListItem_init(const EmailInfoValue *pEmailInfo);

/*
*@fn							    static ReceiveListItem *dmailListener_ReceiveListItem_init(const EmailInfoValue *pEmailInfo);
*@brief   						    用于对ReceiveListItem对象进行初始化操作，从TCP连接采集到的信息构件对象
*@details	
*
*@param[in] const EmailInfoValue* pEmailInfo	    根据pEmailInfo中的信息构建ReceiveListItem   
*
*@return						                    若初始化成功，返回构建出的ReceiveListItem对象的指针；否则，返回NULL
*@retval
*/
static ReceiveListItem *dmailListener_ReceiveListItem_init(const EmailInfoValue *pEmailInfo);

/*
*@fn							    static unsigned int dmailListener_callback_hash_by_TcpLinkKey(const void *vTcpLinkKey, size_t keySize);
*@brief   						    用于根据TCP四元组进行hash操作
*@details	
*
*@param[in] const void* vTcpLinkKey	                进行哈希操作的四元组key
*@param[in] size_t keySize                          key的大小，在此实现中该参数无意义
*
*@return						                    返回算出的hash值
*@retval
*/
static unsigned int dmailListener_callback_hash_by_TcpLinkKey(const void *vTcpLinkKey, size_t keySize);

/*
*@fn							    static unsigned int dmailListener_callback_hash_by_emailAddress(const void *vEmailAddress, size_t keySize);
*@brief   						    用于根据邮箱地址进行hash操作
*@details	
*
*@param[in] const void* vEmailAddress	                进行哈希操作的邮箱地址key
*@param[in] size_t keySize                              key的大小，在此实现中该参数无意义
*
*@return						                        返回算出的hash值
*@retval
*/
static unsigned int dmailListener_callback_hash_by_emailAddress(const void *vEmailAddress, size_t keySize);

/*
*@fn							    static int dmailListener_callback_compare_by_TcpLinkKey(const void *vTcpTableItem, const void *vTcpLinkKey);
*@brief   						    用于对两个四元组进行比较，寻找四元组与vTcpLinkKey匹配的TcpTableItem
*@details	
*
*@param[in] const void* vTcpTableItem	                参加比较的TcpTableItem
*@param[in] const void *vTcpLinkKey                     要寻找的TCP四元组
*
*@return						                        若vTcpTableItem中的四元组与vTcpLinkKey匹配，返回0；否则，返回非0值
*@retval
*/
static int dmailListener_callback_compare_by_TcpLinkKey(const void *vTcpTableItem, const void *vTcpLinkKey);

/*
*@fn							    static int dmailListener_callback_compare_by_TcpLinkKey(const void *vTcpTableItem, const void *vTcpLinkKey);
*@brief   						    用于对两个邮箱地址进行比较，寻找邮箱地址与vEmailAddress一致的EmailTableItem
*@details	
*
*@param[in] const void* vEmailTableItem	                参加比较的EmailTableItem
*@param[in] const void* vEmailAddress                   要寻找的Email地址
*
*@return						                        若vEmailTableItem中的邮箱地址与vEmailAddress匹配，返回0；否则，返回非0值
*@retval
*/
static int dmailListener_callback_compare_by_EmailAddress(const void *vEmailTableItem, const void *vEmailAddress);

/*
*@fn							    static TcpTableItem *dmailListener_TcpTableItem_init(const TcpLinkKey *pTcpLinkKey);
*@brief   						    用于根据TcpLinkKey对TcpTableItem进行初始化
*@details	
*
*@param[in] const TcpLinkKey* pTcpLinkKey	            用于初始化TcpTableItem的四元组
*
*@return						                        若初始化成功，返回指向TcpTableItem的指针；否则，返回NULL
*@retval
*/
static TcpTableItem *dmailListener_TcpTableItem_init(const TcpLinkKey *pTcpLinkKey);

/*
*@fn							    static void dmailListener_SMTP_to_next_state(TcpTableItem *pTcpItem, DMailListener *pMailListener, const u_char *packet, const TcpLinkKey *pTcpLinkKey, unsigned long int Seq, char *pPayLoad, const struct pcap_pkthdr *pkthdr);
*@brief   						    用于控制STMP连接的状态转换
*@details	
*
*@param[in] TcpTableItem *pTcpItem	            STMP报文四元组对应的TCP连接
*@param[in] DMailListener *pMailListener        邮件监听器句柄
*@param[in] const u_char *packet                指向抓取到的STMP报文的指针
*@param[in] const TcpLinkKey *pTcpLinkKey       STMP报文对应的TCP四元组指针
*@param[in] unsigned long int Seq               STMP报文的TCP序列号
*@param[in] char *pPayLoad                      STMP报文的数据部分的起始指针
*@param[in] const struct pcap_pkthdr *pkthdr    STMP报文的长度等相关参数的指针
*
*@return						                        N/A
*@retval
*/
static void dmailListener_SMTP_to_next_state(TcpTableItem *pTcpItem, DMailListener *pMailListener, const u_char *packet, const TcpLinkKey *pTcpLinkKey, unsigned long int Seq, char *pPayLoad, const struct pcap_pkthdr *pkthdr);

/*
*@fn							    static void dmailListener_SMTP_parse(DMailListener *pMailListener, const u_char *packet, const TcpLinkKey *pTcpLinkKey, unsigned long int Seq, char *pPayLoad, const struct pcap_pkthdr *pkthdr);
*@brief   						    用于解析STMP报文
*@details	
*
*@param[in] DMailListener* pMailListener        邮件监听器句柄
*@param[in] const u_char* packet                指向抓取到的STMP报文的指针
*@param[in] const TcpLinkKey* pTcpLinkKey       STMP报文对应的TCP四元组指针
*@param[in] unsigned long int Seq               STMP报文的TCP序列号
*@param[in] char* pPayLoad                      STMP报文的数据部分的起始指针
*@param[in] const struct pcap_pkthdr *pkthdr    STMP报文的长度等相关参数的指针
*
*@return						                        N/A
*@retval
*/
static void dmailListener_SMTP_parse(DMailListener *pMailListener, const u_char *packet, const TcpLinkKey *pTcpLinkKey, unsigned long int Seq, char *pPayLoad, const struct pcap_pkthdr *pkthdr);

/*
*@fn							    static void dmailListener_POP_insert_to_emailTable(DMailListener *pMailListener, TcpTableItem *pTcpItem);
*@brief   						    用于将TCP连接中收集到的邮件信息插入到邮件统计表中
*@details	
*
*@param[in] DMailListener* pMailListener        邮件监听器句柄
*@param[in] TcpTableItem* pTcpItem              根据此中的邮件信息ReceiveListItem创建对象，并插入到邮件统计表中
*
*@return						                N/A
*@retval
*/
static void dmailListener_POP_insert_to_emailTable(DMailListener *pMailListener, TcpTableItem *pTcpItem);

/*
*@fn							    static void dmailListener_POP_parse(DMailListener *pMailListener, const u_char *packet, const TcpLinkKey *pTcpLinkKey, unsigned long int Seq, unsigned long int Ack, char *pPayLoad, const struct pcap_pkthdr *pkthdr);
*@brief   						    用于解析POP报文
*@details	
*
*@param[in] DMailListener* pMailListener        邮件监听器句柄
*@param[in] const u_char* packet                指向抓取到的STMP报文的指针
*@param[in] const TcpLinkKey* pTcpLinkKey       POP报文对应的TCP四元组指针
*@param[in] unsigned long int Seq               POP报文的TCP序列号
*@param[in] unsigned long int Ack               POP报文请求的下一个报文的TCP序号
*@param[in] char* pPayLoad                      POP报文的数据部分的起始指针
*@param[in] const struct pcap_pkthdr *pkthdr    POP报文的长度等相关参数的指针
*
*@return						                N/A
*@retval
*/
static void dmailListener_POP_parse(DMailListener *pMailListener, const u_char *packet, const TcpLinkKey *pTcpLinkKey, unsigned long int Seq, unsigned long int Ack, char *pPayLoad, const struct pcap_pkthdr *pkthdr);

/*
*@fn							    static void dmailListener_packet_parse(u_char *userarg, const struct pcap_pkthdr *pkthdr, const u_char *packet);
*@brief   						    用于解析所有抓取到的报文，报文解析入口，各种具体协议的报文解析函数的聚合
*@details	
*
*@param[in] u_char *userarg                     用户传入的参数
*@param[in] const struct pcap_pkthdr *pkthdr    指向报文的长度等相关参数的指针
*@param[in] const u_char* packet                指向抓取到的报文的指针
*
*@return						                N/A
*@retval
*/
static void dmailListener_packet_parse(u_char *userarg, const struct pcap_pkthdr *pkthdr, const u_char *packet);


static void dmailListener_emailTable_lock(DMailListener *pMailListener)
{
    if(NULL == pMailListener)
    {
        return;
    }

    pthread_mutex_lock(&(pMailListener->emailTableLock));
}

static void dmailListener_emailTable_unlock(DMailListener *pMailListener)
{
    if(NULL == pMailListener)
    {
        return;
    }

    pthread_mutex_unlock(&(pMailListener->emailTableLock));
}

static void dmailListener_tcpTable_lock(DMailListener *pMailListener)
{
    if(NULL == pMailListener)
    {
        return;
    }

    pthread_mutex_lock(&(pMailListener->tcpTableLock));
}

static void dmailListener_tcpTable_unlock(DMailListener *pMailListener)
{
    if(NULL == pMailListener)
    {
        return;
    }

    pthread_mutex_unlock(&(pMailListener->tcpTableLock));
}

static void dmailListener_callback_print_sendListItem(const void *vSendListItem)
{
    SendListItem *pSendListItem = (SendListItem *)vSendListItem;
    printf("Receive address: %s\nEmail size: %lu\nSend time: %s\n", pSendListItem->receiverAddress, pSendListItem->size, pSendListItem->sendTime);
}

static void dmailListener_callback_print_receiveListItem(const void *vReceiveListItem)
{
    ReceiveListItem *pReceiveListItem = (ReceiveListItem *)vReceiveListItem;
    printf("Sende address: %s\nEmail size: %lu\nReceive time: %s\n", pReceiveListItem->senderAddress, pReceiveListItem->size, pReceiveListItem->receiveTime);
}

static void dmailListener_callback_print_userInfo(const void *vEmailTableItem)
{
    EmailTableItem *pEmailTableItem = (EmailTableItem *)vEmailTableItem;
    printf("-----------userAddress: %s--------------\n", pEmailTableItem->userAddress);
    printf("---------sendList------------\n");
    dlist_print(pEmailTableItem->sendList, dmailListener_callback_print_sendListItem);
    printf("\n");

    printf("---------receiveList------------\n");
    dlist_print(pEmailTableItem->receiveList, dmailListener_callback_print_receiveListItem);
    printf("\n");
}

static EmailTableItem *dmailListener_EmailTableItem_init(unsigned char *emailAddr)
{
    if (NULL == emailAddr)
    {
        return NULL;
    }

    EmailTableItem *pEmailTableItem = (EmailTableItem *)malloc(sizeof(EmailTableItem));
    if (NULL == pEmailTableItem)
    {
        return NULL;
    }

    DList *receiveList = dlist_init(NULL, NULL);
    if (NULL == receiveList)
    {
        free(pEmailTableItem);
        return NULL;
    }

    DList *sendList = dlist_init(NULL, NULL);
    if (NULL == sendList)
    {
        dlist_delete(receiveList);
        free(pEmailTableItem);
        return NULL;
    }

    pEmailTableItem->receiveList = receiveList;
    pEmailTableItem->sendList = sendList;
    strncpy((char *)pEmailTableItem->userAddress, (char *)emailAddr, EMAIL_ADDRESS_MAX_LENGTH);

    return pEmailTableItem;
}

static void dmailListener_calback_EmailTableItem_delete(void *vEmailTableItem)
{
    EmailTableItem *pEmailTableItem = (EmailTableItem *)vEmailTableItem;
    dlist_delete(pEmailTableItem->sendList);
    dlist_delete(pEmailTableItem->receiveList);
    free(pEmailTableItem);
}

static SendListItem *dmailListener_SendListItem_init(const EmailInfoValue *pEmailInfo)
{
    if(NULL == pEmailInfo)
    {
        return NULL;
    }

    SendListItem *pSendListItem = (SendListItem *)malloc(sizeof(SendListItem));
    if (NULL == pSendListItem)
    {
        return NULL;
    }

    strncpy((char *)pSendListItem->receiverAddress, (char *)pEmailInfo->receiverAddress, EMAIL_ADDRESS_MAX_LENGTH);
    strncpy((char *)pSendListItem->sendTime, (char *)pEmailInfo->time, TIME_MAX_LENGTH);
    pSendListItem->size = pEmailInfo->size;

    return pSendListItem;
}

static ReceiveListItem *dmailListener_ReceiveListItem_init(const EmailInfoValue *pEmailInfo)
{
    if(NULL == pEmailInfo)
    {
        return NULL;
    }

    ReceiveListItem *pReceiveListItem = (ReceiveListItem *)malloc(sizeof(ReceiveListItem));
    if (NULL == pReceiveListItem)
    {
        return NULL;
    }

    strncpy((char *)pReceiveListItem->senderAddress, (char *)pEmailInfo->senderAddress, EMAIL_ADDRESS_MAX_LENGTH);
    strncpy((char *)pReceiveListItem->receiveTime, (char *)pEmailInfo->time, TIME_MAX_LENGTH);
    pReceiveListItem->size = pEmailInfo->size;

    return pReceiveListItem;
}

static unsigned int dmailListener_callback_hash_by_TcpLinkKey(const void *vTcpLinkKey, size_t keySize)
{
    if(NULL == vTcpLinkKey)
    {
        return 0;
    }

    TcpLinkKey *pTcpLinkKey = (TcpLinkKey *)vTcpLinkKey;
    unsigned int hash = pTcpLinkKey->srcPort + pTcpLinkKey->dstPort;
    int srcIpLen, dstIpLen, i;
    int minLen;

    srcIpLen = strlen((char *)pTcpLinkKey->srcIP);
    dstIpLen = strlen((char *)pTcpLinkKey->dstIP);
    if (srcIpLen < dstIpLen)
    {
        minLen = srcIpLen;
    }
    else
    {
        minLen = dstIpLen;
    }

    for (i = 0; i < minLen; i++)
    {
        hash = 31 * hash + (unsigned int)(pTcpLinkKey->srcIP[i]) + (unsigned int)(pTcpLinkKey->dstIP[i]);
    }

    return hash;
}

static unsigned int dmailListener_callback_hash_by_emailAddress(const void *vEmailAddress, size_t keySize)
{
    if(NULL == vEmailAddress)
    {
        return 0;
    }

    unsigned char *pEmailAddress = (unsigned char *)vEmailAddress;
    int addressLen = strlen((char *)pEmailAddress);
    int i;
    unsigned int hash = 0;
    for (i = 0; i < addressLen; i++)
    {
        hash = 31 * hash + (unsigned int)(pEmailAddress[i]);
    }
    return hash;
}

static int dmailListener_callback_compare_by_TcpLinkKey(const void *vTcpTableItem, const void *vTcpLinkKey)
{
    if(NULL == vTcpLinkKey || NULL == vTcpTableItem)
    {
        return -1;
    }

    TcpTableItem *pTcpTableItem = (TcpTableItem *)vTcpTableItem;
    TcpLinkKey *pTcpLinkKey = (TcpLinkKey *)vTcpLinkKey;
    if (0 == strcmp((char *)pTcpTableItem->key.dstIP, (char *)pTcpLinkKey->dstIP) && 0 == strcmp((char *)pTcpTableItem->key.srcIP, (char *)pTcpLinkKey->srcIP) && pTcpTableItem->key.dstPort == pTcpLinkKey->dstPort && pTcpTableItem->key.srcPort == pTcpLinkKey->srcPort)
    {
        return 0;
    }
    else if (0 == strcmp((char *)pTcpTableItem->key.dstIP, (char *)pTcpLinkKey->srcIP) && 0 == strcmp((char *)pTcpTableItem->key.srcIP, (char *)pTcpLinkKey->dstIP) && pTcpTableItem->key.dstPort == pTcpLinkKey->srcPort && pTcpTableItem->key.srcPort == pTcpLinkKey->dstPort)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

static int dmailListener_callback_compare_by_EmailAddress(const void *vEmailTableItem, const void *vEmailAddress)
{
    if(NULL == vEmailAddress || NULL == vEmailTableItem)
    {
        return -1;
    }

    EmailTableItem *pEmailTableItem = (EmailTableItem *)vEmailTableItem;
    unsigned char *pEmailAddress = (unsigned char *)vEmailAddress;

    return strcmp((char *)pEmailTableItem->userAddress, (char *)pEmailAddress);
}

static TcpTableItem *dmailListener_TcpTableItem_init(const TcpLinkKey *pTcpLinkKey)
{
    if(NULL == pTcpLinkKey)
    {
        return NULL;
    }

    TcpTableItem *pItem = (TcpTableItem *)malloc(sizeof(TcpTableItem));
    if (NULL == pItem)
    {
        return NULL;
    }

    memcpy(&(pItem->key), pTcpLinkKey, sizeof(TcpLinkKey));
    pItem->value.size = 0;
    pItem->value.startSeq = 0;
    pItem->value.state = 0;

    return pItem;
}

static void dmailListener_SMTP_to_next_state(TcpTableItem *pTcpItem, DMailListener *pMailListener, const u_char *packet, const TcpLinkKey *pTcpLinkKey, unsigned long int Seq, char *pPayLoad, const struct pcap_pkthdr *pkthdr)
{
    int nowState = pTcpItem->value.state;
    char tmp[100];
    strncpy(tmp, pPayLoad, 100);
    if (-1 == nowState)
    {
        if (0 == strncmp(pPayLoad, "MAIL FROM:", strlen("MAIL FROM:")))
        {
            sscanf(pPayLoad, "MAIL FROM: %s", pTcpItem->value.senderAddress);
            pTcpItem->value.state = 0;
        }
        else if (0 == strncmp(pPayLoad, "QUIT", strlen("QUIT")))
        {
            dhashTable_delete_data(pMailListener->tcpTable, pTcpLinkKey, sizeof(TcpLinkKey), NULL, dmailListener_callback_compare_by_TcpLinkKey);
        }
        else
        {
        }
    }
    else if (0 == nowState)
    {
        if (0 == strncmp(pPayLoad, "RCPT TO:", strlen("RCPT TO:")))
        {
            sscanf(pPayLoad, "RCPT TO: %s", pTcpItem->value.receiverAddress);
            pTcpItem->value.state = 1;
        }
        else if (0 == strncmp(pPayLoad, "QUIT", strlen("QUIT")))
        {
            dhashTable_delete_data(pMailListener->tcpTable, pTcpLinkKey, sizeof(TcpLinkKey), NULL, dmailListener_callback_compare_by_TcpLinkKey);
        }
        else
        {
        }
    }
    else if (1 == nowState)
    {
        if (0 == strncmp(pPayLoad, "DATA", strlen("DATA")))
        {
            pTcpItem->value.startSeq = Seq;
            pTcpItem->value.state = 2;
        }
        else if (0 == strncmp(pPayLoad, "QUIT", strlen("QUIT")))
        {
            dhashTable_delete_data(pMailListener->tcpTable, pTcpLinkKey, sizeof(TcpLinkKey), NULL, dmailListener_callback_compare_by_TcpLinkKey);
        }
        else
        {
        }
    }
    else if (2 == nowState)
    {
        const u_char *end5 = packet + pkthdr->caplen - 5;
        if (0 == strncmp(pPayLoad, "\r\n.\r\n", strlen("\r\n.\r\n")) || 0 == strncmp((const char *)end5, "\r\n.\r\n", strlen("\r\n.\r\n")))
        {
            pTcpItem->value.size = Seq - pTcpItem->value.startSeq - 6;
            if (0 == strncmp((const char *)end5, "\r\n.\r\n", strlen("\r\n.\r\n")))
            {
                pTcpItem->value.size += (pkthdr->caplen - 54 - 5);
            }

            strncpy((char *)pTcpItem->value.time, ctime((const time_t *)&pkthdr->ts.tv_sec), TIME_MAX_LENGTH);

            SendListItem *pSendListItem = dmailListener_SendListItem_init(&(pTcpItem->value));
            if (NULL == pSendListItem)
            {
                pTcpItem->value.state = -1;
                return;
            }

            EmailTableItem *pEmailTableItem = dhashTable_query_by_key(pMailListener->emailTable, pTcpItem->value.senderAddress, strlen((char *)pTcpItem->value.senderAddress), dmailListener_callback_compare_by_EmailAddress);
            if (NULL == pEmailTableItem)
            {
                pEmailTableItem = dmailListener_EmailTableItem_init(pTcpItem->value.senderAddress);
                if (NULL == pEmailTableItem)
                {
                    pTcpItem->value.state = -1;
                    free(pSendListItem);
                    return;
                }

                dlist_appened_node(pEmailTableItem->sendList, pSendListItem);
                dhashTable_insert_data(pMailListener->emailTable, pEmailTableItem->userAddress, EMAIL_ADDRESS_MAX_LENGTH, pEmailTableItem);
            }
            else
            {
                dlist_appened_node(pEmailTableItem->sendList, pSendListItem);
            }

            pTcpItem->value.state = -1;
        }
        else
        {
        }
    }
}

static void dmailListener_SMTP_parse(DMailListener *pMailListener, const u_char *packet, const TcpLinkKey *pTcpLinkKey, unsigned long int Seq, char *pPayLoad, const struct pcap_pkthdr *pkthdr)
{
    TcpTableItem *pTcpTableItem = (TcpTableItem *)dhashTable_query_by_key(pMailListener->tcpTable, pTcpLinkKey, sizeof(TcpLinkKey), dmailListener_callback_compare_by_TcpLinkKey);
    if (NULL == pTcpTableItem)
    {
        if (0 == strncmp(pPayLoad, "EHLO", strlen("EHLO")))
        {
            pTcpTableItem = dmailListener_TcpTableItem_init(pTcpLinkKey);
            if (NULL == pTcpTableItem)
            {
                return;
            }

            pTcpTableItem->value.state = -1;
            dhashTable_insert_data(pMailListener->tcpTable, &(pTcpTableItem->key), sizeof(TcpTableItem), pTcpTableItem);
            return;
        }
    }
    else
    {
        dmailListener_SMTP_to_next_state(pTcpTableItem, pMailListener, packet, pTcpLinkKey, Seq, pPayLoad, pkthdr);
    }
}

static void dmailListener_POP_insert_to_emailTable(DMailListener *pMailListener, TcpTableItem *pTcpItem)
{
    ReceiveListItem *pReceiveListItem = dmailListener_ReceiveListItem_init(&(pTcpItem->value));
    if (NULL == pReceiveListItem)
    {
        return;
    }

    EmailTableItem *pEmailItem = dhashTable_query_by_key(pMailListener->emailTable, pTcpItem->value.receiverAddress, EMAIL_ADDRESS_MAX_LENGTH, dmailListener_callback_compare_by_EmailAddress);
    if (NULL == pEmailItem)
    {
        pEmailItem = dmailListener_EmailTableItem_init(pTcpItem->value.receiverAddress);
        if (NULL == pEmailItem)
        {
            free(pReceiveListItem);
            return;
        }

        dlist_appened_node(pEmailItem->receiveList, pReceiveListItem);
        dhashTable_insert_data(pMailListener->emailTable, pTcpItem->value.receiverAddress, EMAIL_ADDRESS_MAX_LENGTH, pEmailItem);
        return;
    }
    else
    {
        dlist_appened_node(pEmailItem->receiveList, pReceiveListItem);
    }
}

static void dmailListener_POP_parse(DMailListener *pMailListener, const u_char *packet, const TcpLinkKey *pTcpLinkKey, unsigned long int Seq, unsigned long int Ack, char *pPayLoad, const struct pcap_pkthdr *pkthdr)
{
    TcpTableItem *pTcpTableItem = (TcpTableItem *)dhashTable_query_by_key(pMailListener->tcpTable, pTcpLinkKey, sizeof(TcpLinkKey), dmailListener_callback_compare_by_TcpLinkKey);
    if (NULL == pTcpTableItem)
    {
        if (110 == pTcpLinkKey->dstPort)
        {
            if (0 == strncmp(pPayLoad, "RETR", strlen("RETR")))
            {
                pTcpTableItem = dmailListener_TcpTableItem_init(pTcpLinkKey);
                if (NULL == pTcpTableItem)
                {
                    return;
                }

                pTcpTableItem->value.Ack = Ack;
                dhashTable_insert_data(pMailListener->tcpTable, &(pTcpTableItem->key), sizeof(TcpTableItem), pTcpTableItem);
                return;
            }
        }
    }
    else
    {
        if (110 == pTcpLinkKey->srcPort && Seq == pTcpTableItem->value.Ack)
        {
            int paraNum = 0;
            strncpy((char *)pTcpTableItem->value.time, ctime((const time_t *)&pkthdr->ts.tv_sec), TIME_MAX_LENGTH);
            char dataBuf[BUFFER_MAX_LENGTH];

            strncpy(dataBuf, pPayLoad, BUFFER_MAX_LENGTH);

            char *pToken = NULL;
            char *pSave = NULL;
            unsigned char receiverAddr[EMAIL_ADDRESS_MAX_LENGTH];
            receiverAddr[0] = '<';

            pToken = strtok_r(dataBuf, "\r\n", &pSave);
            paraNum = sscanf(pToken, "+OK %lu octets", &(pTcpTableItem->value.size));
            if(paraNum != 1)
            {
                goto end;
            }

            pToken = strtok_r(NULL, "\r\n", &pSave);
            paraNum = sscanf(pToken, "Return-Path: %s", pTcpTableItem->value.senderAddress);
            if(paraNum != 1)
            {
                goto end;
            }

            pToken = strtok_r(NULL, "\r\n", &pSave);
            pToken = strtok_r(NULL, "\r\n", &pSave);
            paraNum = sscanf(pToken, "Delivered-To: %s", receiverAddr + 1);
            if(paraNum != 1)
            {
                goto end;
            }

            int len = strlen((char *)receiverAddr);
            receiverAddr[len] = '>';
            receiverAddr[len+1] = '\0';

            strncpy((char *)pTcpTableItem->value.receiverAddress, (char *)receiverAddr, EMAIL_ADDRESS_MAX_LENGTH);

            dmailListener_POP_insert_to_emailTable(pMailListener, pTcpTableItem);
        end:
            dhashTable_delete_data(pMailListener->tcpTable, pTcpLinkKey, sizeof(TcpLinkKey), NULL, dmailListener_callback_compare_by_TcpLinkKey);
        }
    }
}

static void dmailListener_packet_parse(u_char *userarg, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
    pthread_testcancel();

    if (pkthdr->len <= 54)
    {
        return;
    }

    DMailListener *pMailListener = (DMailListener *)userarg;
    struct in_addr IpAddr;
    struct iphdr *pIpHeader;
    struct tcphdr *pTcpHeader;
    char *pPayload;
    TcpLinkKey tcpLink;
    unsigned long int Seq, Ack;

    pIpHeader = (struct iphdr *)(packet + sizeof(struct ether_header)); //得到ip包头
    IpAddr.s_addr = pIpHeader->daddr;
    strncpy((char *)tcpLink.dstIP, inet_ntoa(IpAddr), INET_ADDRSTRLEN);
    IpAddr.s_addr = pIpHeader->saddr;
    strncpy((char *)tcpLink.srcIP, inet_ntoa(IpAddr), INET_ADDRSTRLEN);

    pTcpHeader = (struct tcphdr *)(packet + sizeof(struct ether_header) + sizeof(struct iphdr)); //得到tcp包头
    tcpLink.dstPort = ntohs(pTcpHeader->dest);
    tcpLink.srcPort = ntohs(pTcpHeader->source);
    Seq = ntohl(pTcpHeader->seq);
    Ack = ntohl(pTcpHeader->ack_seq);

    pPayload = (char *)(packet + sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct tcphdr)); //得到数据包里内容，不过一般为乱码。

    dmailListener_tcpTable_lock(pMailListener);
    dmailListener_emailTable_lock(pMailListener);
    if (25 == tcpLink.dstPort)
    {
        dmailListener_SMTP_parse(pMailListener, packet, &tcpLink, Seq, pPayload, pkthdr);
    }
    else
    {
        dmailListener_POP_parse(pMailListener, packet, &tcpLink, Seq, Ack, pPayload, pkthdr);
    }
    dmailListener_emailTable_unlock(pMailListener);
    dmailListener_tcpTable_unlock(pMailListener);

    return;
}

DMailListener *dmailListener_init(int tcpLinkTableSize, int emailInfoTableSize)
{
    if (tcpLinkTableSize <= 0 || emailInfoTableSize <= 0)
    {
        return NULL;
    }

    DMailListener *pMailListener = (DMailListener *)malloc(sizeof(DMailListener));
    if (NULL == pMailListener)
    {
        return NULL;
    }

    DHashTable *pEmailTable = dhashTable_init(emailInfoTableSize, dmailListener_callback_hash_by_emailAddress, dmailListener_calback_EmailTableItem_delete);
    if (NULL == pEmailTable)
    {
        free(pMailListener);
        return NULL;
    }

    DHashTable *pTcpTable = dhashTable_init(tcpLinkTableSize, dmailListener_callback_hash_by_TcpLinkKey, NULL);
    if (NULL == pTcpTable)
    {
        free(pMailListener);
        free(pEmailTable);
        return NULL;
    }

    pthread_mutex_init(&(pMailListener->emailTableLock), NULL);
    pthread_mutex_init(&(pMailListener->tcpTableLock), NULL);
    pMailListener->emailTable = pEmailTable;
    pMailListener->tcpTable = pTcpTable;

    return pMailListener;
}

typedef struct para
{
    pcap_t *handle;
    DMailListener *pMailListener;
} para;


static void *dmailListener_pcap_loop(void *vPara)
{
    para *pPara = (para *)vPara;
    pcap_loop(pPara->handle, -1, dmailListener_packet_parse, (u_char *)(pPara->pMailListener));
    return NULL;
}

void dmailListener_run(DMailListener *pMailListener, const char *filePath, int mode)
{
    if(NULL == pMailListener)
    {
        return;
    }
    
    pcap_t *handle;
    char errBuf[PCAP_ERRBUF_SIZE];
    struct bpf_program filter;
    char devTable[20][30];
    int i;

    if(MODE_OFFLINE == mode)
    {
        handle = pcap_open_offline(filePath, errBuf);
        if(NULL == handle)
        {
            printf("open file failed!\n");
            return;
        }
    }
    else if(MODE_ONLINE == mode)
    {
        pcap_if_t *pDev;
        int devIndex;
        i = 1;
        pcap_findalldevs(&pDev, errBuf);
        printf("find these devices:\n");
        while (NULL != pDev)
        {
            printf("%d. %s\n", i, pDev->name);
            strncpy(devTable[i], pDev->name, 30);
            i++;
            pDev = pDev->next;
        }
        
    enter_dev_index:
        printf("\nplease enter a number to decide the interface you want to listen:\n");
        scanf("%d", &devIndex);
        if(devIndex >= i)
        {
            printf("illegal index!\n");
            goto enter_dev_index;
        }

        handle = pcap_open_live(devTable[devIndex], 65535, 1, 1000, errBuf);
        if(NULL == handle)
        {
            printf("listen device failed!\n");
            return;
        }
    }
    else
    {
        printf("illegal mode index!\n");
        return;
    }
    

    char filter_app[] = "dst port 25 or src port 110 or dst port 110";

    pcap_compile(handle, &filter, filter_app, 0, 0);
    pcap_setfilter(handle, &filter);

    int tid;
    para lPara;
    lPara.pMailListener = pMailListener;
    lPara.handle = handle;
    int res = pthread_create((pthread_t *)&tid, NULL, dmailListener_pcap_loop, (void *)&lPara);

    if(0 != res)
    {
        printf("creat loop thread failed!\n");
        return;
    }

    char commend[20];
    while (1)
    {
        printf(">");
        scanf("%s", commend);
        if(0 == strcmp(commend, "print"))
        {
            dmailListener_print_all_mailInfo(pMailListener);
        }
        else if(0 == strcmp(commend, "quit"))
        {
            pthread_cancel((pthread_t)&tid);
            break;
        }
        
    }
    

    // pcap_loop(handle, -1, dmailListener_packet_parse, (u_char *)(pMailListener));
    pcap_close(handle);
}

void dmailListener_print_all_mailInfo(DMailListener *pMailListener)
{
    if(NULL == pMailListener)
    {
        return;
    }

    dmailListener_emailTable_lock(pMailListener);
    dhashTable_print(pMailListener->emailTable, dmailListener_callback_print_userInfo);
    dmailListener_emailTable_unlock(pMailListener);
}

int dmailListener_delete(DMailListener *pMailListener)
{
    if(NULL == pMailListener)
    {
        return NULL_POINTER;
    }

    dhashTable_delete(pMailListener->emailTable);
    dhashTable_delete(pMailListener->tcpTable);
    free(pMailListener);

    return SUCCESS;
}

