#include "../include/DMailListener.h"

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
} DMailListener;


void dmailListener_callback_print_sendListItem(const void *vSendListItem)
{
    SendListItem *pSendListItem = (SendListItem *)vSendListItem;
    printf("Receive address: %s\nEmail size: %lu\nSend time: %s\n\n", pSendListItem->receiverAddress, pSendListItem->size, pSendListItem->sendTime);
}

void dmailListener_callback_print_receiveListItem(const void *vReceiveListItem)
{
    ReceiveListItem *pReceiveListItem = (ReceiveListItem *)vReceiveListItem;
    printf("Receive address: %s\nEmail size: %lu\nReceive time: %s\n\n", pReceiveListItem->senderAddress, pReceiveListItem->size, pReceiveListItem->receiveTime);
}

void dmailListener_callback_print_userInfo(const void *vEmailTableItem)
{
    EmailTableItem *pEmailTableItem = (EmailTableItem *)vEmailTableItem;
    printf("-----------userAddress: %s--------------\n", pEmailTableItem->userAddress);
    printf("---------sendList------------\n");
    dlist_print(pEmailTableItem->sendList, dmailListener_callback_print_sendListItem);

    printf("---------receiveList------------\n");
    dlist_print(pEmailTableItem->receiveList, dmailListener_callback_print_receiveListItem);

    printf("\n\n");
}

EmailTableItem *dmailListener_EmailTableItem_init(unsigned char *emailAddr)
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

SendListItem *dmailListener_SendListItem_init(const EmailInfoValue *pEmailInfo)
{
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

ReceiveListItem *dmailListener_ReceiveListItem_init(const EmailInfoValue *pEmailInfo)
{
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

unsigned int dmailListener_hash_by_TcpLinkKey(const void *vTcpLinkKey, size_t keySize)
{
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

unsigned int dmailListener_hash_by_emailAddress(const void *vEmailAddress, size_t keySize)
{
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

int dmailListener_compare_by_TcpLinkKey(const void *vTcpTableItem, const void *vTcpLinkKey)
{
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

int dmailListener_compare_by_EmailAddress(const void *vEmailTableItem, const void *vEmailAddress)
{
    EmailTableItem *pEmailTableItem = (EmailTableItem *)vEmailAddress;
    unsigned char *pEmailAddress = (unsigned char *)vEmailAddress;

    return strcmp((char *)pEmailTableItem->userAddress, (char *)pEmailAddress);
}

TcpTableItem *dmailListener_TcpTableItem_init(const TcpLinkKey *pTcpLinkKey)
{
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

void dmailListener_SMTP_to_next_state(TcpTableItem *pTcpItem, DMailListener *pMailListener, const u_char *packet, const TcpLinkKey *pTcpLinkKey, unsigned long int Seq, char *pPayLoad, const struct pcap_pkthdr *pkthdr)
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
            dhashTable_delete_data(pMailListener->tcpTable, pTcpLinkKey, sizeof(TcpLinkKey), NULL, dmailListener_compare_by_TcpLinkKey);
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
            dhashTable_delete_data(pMailListener->tcpTable, pTcpLinkKey, sizeof(TcpLinkKey), NULL, dmailListener_compare_by_TcpLinkKey);
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
            dhashTable_delete_data(pMailListener->tcpTable, pTcpLinkKey, sizeof(TcpLinkKey), NULL, dmailListener_compare_by_TcpLinkKey);
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

            EmailTableItem *pEmailTableItem = dhashTable_query_by_key(pMailListener->emailTable, pTcpItem->value.senderAddress, strlen((char *)pTcpItem->value.senderAddress), dmailListener_compare_by_EmailAddress);
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

void dmailListener_SMTP_parse(DMailListener *pMailListener, const u_char *packet, const TcpLinkKey *pTcpLinkKey, unsigned long int Seq, char *pPayLoad, const struct pcap_pkthdr *pkthdr)
{
    TcpTableItem *pTcpTableItem = (TcpTableItem *)dhashTable_query_by_key(pMailListener->tcpTable, pTcpLinkKey, sizeof(TcpLinkKey), dmailListener_compare_by_TcpLinkKey);
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

void dmailListener_POP_insert_to_emailTable(DMailListener *pMailListener, TcpTableItem *pTcpItem)
{
    ReceiveListItem *pReceiveListItem = dmailListener_ReceiveListItem_init(&(pTcpItem->value));
    if (NULL == pReceiveListItem)
    {
        return;
    }

    EmailTableItem *pEmailItem = dhashTable_query_by_key(pMailListener->emailTable, pTcpItem->value.receiverAddress, EMAIL_ADDRESS_MAX_LENGTH, dmailListener_compare_by_EmailAddress);
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

void dmailListener_POP_parse(DMailListener *pMailListener, const u_char *packet, const TcpLinkKey *pTcpLinkKey, unsigned long int Seq, unsigned long int Ack, char *pPayLoad, const struct pcap_pkthdr *pkthdr)
{
    TcpTableItem *pTcpTableItem = (TcpTableItem *)dhashTable_query_by_key(pMailListener->tcpTable, pTcpLinkKey, sizeof(TcpLinkKey), dmailListener_compare_by_TcpLinkKey);
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
            dhashTable_delete_data(pMailListener->tcpTable, pTcpLinkKey, sizeof(TcpLinkKey), NULL, dmailListener_compare_by_TcpLinkKey);
        }
    }
}

void dmailListener_packet_parse(u_char *userarg, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
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

    if (25 == tcpLink.dstPort)
    {
        dmailListener_SMTP_parse(pMailListener, packet, &tcpLink, Seq, pPayload, pkthdr);
    }
    else
    {
        dmailListener_POP_parse(pMailListener, packet, &tcpLink, Seq, Ack, pPayload, pkthdr);
    }

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

    DHashTable *pEmailTable = dhashTable_init(emailInfoTableSize, dmailListener_hash_by_emailAddress, dmailListener_calback_EmailTableItem_delete);
    if (NULL == pEmailTable)
    {
        free(pMailListener);
        return NULL;
    }

    DHashTable *pTcpTable = dhashTable_init(tcpLinkTableSize, dmailListener_hash_by_TcpLinkKey, NULL);
    if (NULL == pTcpTable)
    {
        free(pMailListener);
        free(pEmailTable);
        return NULL;
    }

    pMailListener->emailTable = pEmailTable;
    pMailListener->tcpTable = pTcpTable;

    return pMailListener;
}

void dmailListener_run(DMailListener *pMailListener, const char *filePath)
{
    /* 查找LAN口网卡并获取handle */
    pcap_t *handle;
    char errBuf[PCAP_ERRBUF_SIZE];
    struct bpf_program filter;
    if(NULL != filePath)
    {
        handle = pcap_open_offline(filePath, errBuf);
    }
    else
    {
        
    }
    
    if (NULL != handle)
    {
        printf("open ok!\n");
    }
    else
    {
        printf("open failed!\n");
    }

    char filter_app[] = "dst port 25 or src port 110 or dst port 110";

    pcap_compile(handle, &filter, filter_app, 0, 0);
    pcap_setfilter(handle, &filter);

    pcap_loop(handle, -1, dmailListener_packet_parse, (u_char *)(pMailListener));
    pcap_close(handle);
}

void dmailListener_print_all_mailInfo(DMailListener *pMailListener)
{
    dhashTable_print(pMailListener->emailTable, dmailListener_callback_print_userInfo);
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

