#include <stdio.h>
#include <pcap.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <string.h>
#include "../include/DList.h"
#include "../include/DHashTable.h"

#define EMAIL_ADDRESS_MAX_LENGTH 50
#define TIME_MAX_LENGTH 50
#define HASH_TABLE_SIZE 128
#define BUFFER_MAX_LENGTH 256

typedef struct _SendListItem
{
    unsigned char receiverAddress[EMAIL_ADDRESS_MAX_LENGTH];
    unsigned long int size;
    unsigned char sendTime[TIME_MAX_LENGTH];
} SendListItem;

typedef struct _ReceiveListItem
{
    unsigned char senderAddress[EMAIL_ADDRESS_MAX_LENGTH];
    unsigned long int size;
    unsigned char receiveTime[TIME_MAX_LENGTH];
} ReceiveListItem;

typedef struct _EmailTableItem
{
    unsigned char userAddress[EMAIL_ADDRESS_MAX_LENGTH]; /* key */
    DList *sendList;                                     /* value */
    DList *receiveList;                                  /* value */
} EmailTableItem;

typedef struct _TcpLinkKey
{
    unsigned char srcIP[INET_ADDRSTRLEN];
    unsigned char dstIP[INET_ADDRSTRLEN];

    unsigned int srcPort;
    unsigned int dstPort;
} TcpLinkKey;

typedef struct _EmailInfoValue
{
    unsigned char receiverAddress[EMAIL_ADDRESS_MAX_LENGTH];
    unsigned char senderAddress[EMAIL_ADDRESS_MAX_LENGTH];

    unsigned long int size;
    unsigned char time[TIME_MAX_LENGTH];

    unsigned long int startSeq;
    unsigned long int Ack;
    int state;
} EmailInfoValue;

typedef struct _TcpTableItem
{
    TcpLinkKey key;
    EmailInfoValue value;
} TcpTableItem;

typedef struct _DMailListener
{
    DHashTable *emailTable;
    DHashTable *tcpTable;
} DMailListener;

void dmailListener_print_sendListItem(const void *vSendListItem)
{
    SendListItem *pSendListItem = (SendListItem *)vSendListItem;
    printf("Receive address: %s\nEmail size: %lu\nSend time: %s\n\n", pSendListItem->receiverAddress, pSendListItem->size, pSendListItem->sendTime);
}

void dmailListener_print_receiveListItem(const void *vReceiveListItem)
{
    ReceiveListItem *pReceiveListItem = (ReceiveListItem *)vReceiveListItem;
    printf("Receive address: %s\nEmail size: %lu\nSend time: %s\n\n", pReceiveListItem->senderAddress, pReceiveListItem->size, pReceiveListItem->receiveTime);
}

void dmailListener_print_mailInfo(const void *vEmailTableItem)
{
    EmailTableItem *pEmailTableItem = (EmailTableItem *)vEmailTableItem;
    printf("-----------userAddress: %s--------------\n", pEmailTableItem->userAddress);
    printf("---------sendList------------\n");
    dlist_print(pEmailTableItem->sendList, dmailListener_print_sendListItem);

    printf("---------receiveList------------\n");
    dlist_print(pEmailTableItem->receiveList, dmailListener_print_sendListItem);

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

    DList *receiveList = dlist_init(NULL);
    if (NULL == receiveList)
    {
        free(pEmailTableItem);
        return NULL;
    }

    DList *sendList = dlist_init(NULL);
    if (NULL == sendList)
    {
        dlist_delete(receiveList);
        free(pEmailTableItem);
        return NULL;
    }

    pEmailTableItem->receiveList = receiveList;
    pEmailTableItem->sendList = sendList;
    strncpy(pEmailTableItem->userAddress, emailAddr, EMAIL_ADDRESS_MAX_LENGTH);

    return pEmailTableItem;
}

SendListItem *dmailListener_SendListItem_init(const EmailInfoValue *pEmailInfo)
{
    SendListItem *pSendListItem = (SendListItem *)malloc(sizeof(SendListItem));
    if (NULL == pSendListItem)
    {
        return NULL;
    }

    strncpy(pSendListItem->receiverAddress, pEmailInfo->receiverAddress, EMAIL_ADDRESS_MAX_LENGTH);
    strncpy(pSendListItem->sendTime, pEmailInfo->time, TIME_MAX_LENGTH);
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

    strncpy(pReceiveListItem->senderAddress, pEmailInfo->senderAddress, EMAIL_ADDRESS_MAX_LENGTH);
    strncpy(pReceiveListItem->receiveTime, pEmailInfo->time, TIME_MAX_LENGTH);
    pReceiveListItem->size = pEmailInfo->size;

    return pReceiveListItem;
}

unsigned int dmailListener_hash_by_TcpLinkKey(const void *vTcpLinkKey, size_t keySize)
{
    TcpLinkKey *pTcpLinkKey = (TcpLinkKey *)vTcpLinkKey;
    unsigned int hash = pTcpLinkKey->srcPort + pTcpLinkKey->dstPort;
    int srcIpLen, dstIpLen, i;
    int minLen;

    srcIpLen = strlen(pTcpLinkKey->srcIP);
    dstIpLen = strlen(pTcpLinkKey->dstIP);
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
    int addressLen = strlen(pEmailAddress);
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
    if (0 == strcmp(pTcpTableItem->key.dstIP, pTcpLinkKey->dstIP) && 0 == strcmp(pTcpTableItem->key.srcIP, pTcpLinkKey->srcIP) && pTcpTableItem->key.dstPort == pTcpLinkKey->dstPort && pTcpTableItem->key.srcPort == pTcpLinkKey->srcPort)
    {
        return 0;
    }
    else if (0 == strcmp(pTcpTableItem->key.dstIP, pTcpLinkKey->srcIP) && 0 == strcmp(pTcpTableItem->key.srcIP, pTcpLinkKey->dstIP) && pTcpTableItem->key.dstPort == pTcpLinkKey->srcPort && pTcpTableItem->key.srcPort == pTcpLinkKey->dstPort)
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

    return strcmp(pEmailTableItem->userAddress, pEmailAddress);
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
    unsigned char tmp[100];
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
        u_char *end5 = packet + pkthdr->caplen - 5;
        if (0 == strncmp(pPayLoad, "\r\n.\r\n", strlen("\r\n.\r\n")) || 0 == strncmp(end5, "\r\n.\r\n", strlen("\r\n.\r\n")))
        {
            pTcpItem->value.size = Seq - pTcpItem->value.startSeq - 6;
            if (0 == strncmp(end5, "\r\n.\r\n", strlen("\r\n.\r\n")))
            {
                pTcpItem->value.size += (pkthdr->caplen - 54 - 5);
            }

            strncpy(pTcpItem->value.time, ctime((const time_t *)&pkthdr->ts.tv_sec), TIME_MAX_LENGTH);

            SendListItem *pSendListItem = dmailListener_SendListItem_init(&(pTcpItem->value));
            if (NULL == pSendListItem)
            {
                pTcpItem->value.state = -1;
                return;
            }

            EmailTableItem *pEmailTableItem = dhashTable_query_by_key(pMailListener->emailTable, pTcpItem->value.senderAddress, strlen(pTcpItem->value.senderAddress), dmailListener_compare_by_EmailAddress);
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
                // dhashTable_print(pMailListener->emailTable, dmailListener_print_mailInfo);
            }
            else
            {

                // dhashTable_insert_data(pMailListener->emailTable, pTcpItem->value.senderAddress, EMAIL_ADDRESS_MAX_LENGTH, pEmailTableItem);
                dlist_appened_node(pEmailTableItem->sendList, pSendListItem);
                // dhashTable_print(pMailListener->emailTable, dmailListener_print_mailInfo);
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
            strncpy(pTcpTableItem->value.time, ctime((const time_t *)&pkthdr->ts.tv_sec), TIME_MAX_LENGTH);
            unsigned char dataBuf[BUFFER_MAX_LENGTH];

            strncpy(dataBuf, pPayLoad, BUFFER_MAX_LENGTH);

            unsigned char *pToken = NULL;
            unsigned char *pSave = NULL;

            unsigned char receiverAddr[EMAIL_ADDRESS_MAX_LENGTH];
            receiverAddr[0] = '<';

            pToken = strtok_r(dataBuf, "\r\n", &pSave);
            sscanf(pToken, "+OK %lu octets", &(pTcpTableItem->value.size));

            pToken = strtok_r(NULL, "\r\n", &pSave);
            sscanf(pToken, "Return-Path: %s", pTcpTableItem->value.senderAddress);

            pToken = strtok_r(NULL, "\r\n", &pSave);
            pToken = strtok_r(NULL, "\r\n", &pSave);
            // char buf[100];
            // strncpy(buf, pToken, 100);
            sscanf(pToken, "Delivered-To: %s", receiverAddr + 1);

            int len = strlen(receiverAddr);
            receiverAddr[len] = '>';
            receiverAddr[len+1] = '\0';

            strncpy(pTcpTableItem->value.receiverAddress, receiverAddr, EMAIL_ADDRESS_MAX_LENGTH);

            dmailListener_POP_insert_to_emailTable(pMailListener, pTcpTableItem);
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

    // printf("id: %d\n", (*id)++);
    // printf("length: %d\n", pkthdr->len);
    // printf("Number of bytes: %d\n", pkthdr->caplen);
    // printf("time: %s", ctime((const time_t *)&pkthdr->ts.tv_sec));

    pIpHeader = (struct iphdr *)(packet + sizeof(struct ether_header)); //得到ip包头
    IpAddr.s_addr = pIpHeader->daddr;
    strncpy(tcpLink.dstIP, inet_ntoa(IpAddr), INET_ADDRSTRLEN);
    IpAddr.s_addr = pIpHeader->saddr;
    strncpy(tcpLink.srcIP, inet_ntoa(IpAddr), INET_ADDRSTRLEN);

    pTcpHeader = (struct iphdr *)(packet + sizeof(struct ether_header) + sizeof(struct iphdr)); //得到tcp包头
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

    DHashTable *pEmailTable = dhashTable_init(emailInfoTableSize, dmailListener_hash_by_emailAddress);
    if (NULL == pEmailTable)
    {
        free(pMailListener);
        return NULL;
    }

    DHashTable *pTcpTable = dhashTable_init(tcpLinkTableSize, dmailListener_hash_by_TcpLinkKey);
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

void dmailListener_run(DMailListener *pMailListener)
{
    /* 查找LAN口网卡并获取handle */
    pcap_t *handle;
    char errBuf[PCAP_ERRBUF_SIZE];
    struct bpf_program filter;
    struct pcap_pkthdr header;
    u_char *packet;

    handle = pcap_open_offline("../InputFile/test.pcapng", errBuf);

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

int main()
{
    DMailListener *pl = dmailListener_init(20, 20);
    dmailListener_run(pl);
    dhashTable_print(pl->emailTable, dmailListener_print_mailInfo);

    return 0;
}