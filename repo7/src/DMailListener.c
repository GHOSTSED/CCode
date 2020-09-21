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
    DList *sendList;                            /* value */
    DList *receiveList;                         /* value */
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

SendListItem *dmailListener_SendListItem_init(const EmailInfoValue *pEmailInfo)
{
    SendListItem *pSendListItem = (SendListItem *)malloc(sizeof(SendListItem));
    if(NULL == pSendListItem)
    {
        return NULL;
    } 

    strncpy(pSendListItem->receiverAddress, pEmailInfo->receiverAddress, EMAIL_ADDRESS_MAX_LENGTH);
    strncpy(pSendListItem->sendTime, pEmailInfo->time, TIME_MAX_LENGTH);
    pSendListItem->size = pEmailInfo->size;

    return pSendListItem;
}

unsigned int dmailListener_hash_by_TcpLinkKey(const void *vTcpLinkKey, size_t keySize)
{
    TcpLinkKey *pTcpLinkKey = (TcpLinkKey *)vTcpLinkKey;
    unsigned int hash = pTcpLinkKey->srcPort + pTcpLinkKey->dstPort;
    int srcIpLen,dstIpLen,i;
    int minLen;

    srcIpLen = strlen(pTcpLinkKey->srcIP);
    dstIpLen = strlen(pTcpLinkKey->dstIP);
    if(srcIpLen < dstIpLen)
    {
        minLen = srcIpLen;
    }
    else
    {
        minLen = dstIpLen;
    }
    
    for(i = 0; i < minLen; i++)
    {
        hash = 31*hash + (unsigned int)(pTcpLinkKey->srcIP[i]) + (unsigned int)(pTcpLinkKey->dstIP[i]);
    }

    return hash;
}

int dmailListener_compare_by_TcpLinkKey(const void *vTcpTableItem, const void *vTcpLinkKey)
{
    TcpTableItem *pTcpTableItem = (TcpTableItem *)vTcpTableItem;
    TcpLinkKey *pTcpLinkKey = (TcpLinkKey *)vTcpLinkKey;
    if(0 == strcmp(pTcpTableItem->key.dstIP, pTcpLinkKey->dstIP) 
    && 0 == strcmp(pTcpTableItem->key.srcIP, pTcpLinkKey->srcIP)
    && pTcpTableItem->key.dstPort == pTcpLinkKey->dstPort
    && pTcpTableItem->key.srcPort == pTcpLinkKey->srcPort)
    {
        return 0;
    }
    else
    {
        return -1;
    }
    
}

TcpTableItem *dmailListener_TcpTableItem_init(const TcpLinkKey *pTcpLinkKey)
{
    TcpTableItem *pItem = (TcpTableItem *)malloc(sizeof(TcpTableItem));
    if(NULL == pItem)
    {
        return NULL;
    }

    memcpy(&(pItem->key), pTcpLinkKey, sizeof(TcpLinkKey));
    pItem->value.size = 0;
    pItem->value.startSeq = 0;
    pItem->value.state = 0;

    return pItem;
}

void dmailListener_to_next_state(TcpTableItem *pTcpItem, DMailListener *pMailListener, const u_char *packet, const TcpLinkKey *pTcpLinkKey, unsigned long int Seq, char *pPayLoad, const struct pcap_pkthdr *pkthdr)
{
    int nowState = pTcpItem->value.state;
    if(-1 == nowState)
    {
        if(0 == strncmp(pPayLoad, "MAIL FROM:", strlen("MAIL FROM:")))
        {
            sscanf(pPayLoad, "MAIL FROM: %s", pTcpItem->value.senderAddress);
            pTcpItem->value.state = 0;
        }
        else if(0 == strncmp(pPayLoad, "QUIT", strlen("QUIT")))
        {
            dhashTable_delete_data(pMailListener->tcpTable, pTcpLinkKey, sizeof(TcpLinkKey), NULL, dmailListener_compare_by_TcpLinkKey);
        }
        else
        {
            
        }
    }
    else if(0 == nowState)
    {
        if(0 == strncmp(pPayLoad, "RCPT TO:", strlen("RCPT TO:")))
        {
            sscanf(pPayLoad, "RCPT TO: %s", pTcpItem->value.receiverAddress);
            pTcpItem->value.state = 1;
        }
        else if(0 == strncmp(pPayLoad, "QUIT", strlen("QUIT")))
        {
            dhashTable_delete_data(pMailListener->tcpTable, pTcpLinkKey, sizeof(TcpLinkKey), NULL, dmailListener_compare_by_TcpLinkKey);
        }
        else
        {
            
        }
        
    }
    else if(1 == nowState)
    {
        if(0 == strncmp(pPayLoad, "DATA", strlen("DATA")))
        {
            pTcpItem->value.startSeq = Seq;
            pTcpItem->value.state = 2;
        }
        else if(0 == strncmp(pPayLoad, "QUIT", strlen("QUIT")))
        {
            dhashTable_delete_data(pMailListener->tcpTable, pTcpLinkKey, sizeof(TcpLinkKey), NULL, dmailListener_compare_by_TcpLinkKey);
        }
        else
        {
            
        }
    }
    else if(2 == nowState)
    {
        if(0 == strncmp(pPayLoad, "\r\n.\r\n", strlen("\r\n.\r\n")))
        {
            pTcpItem->value.size = Seq - pTcpItem->value.startSeq - 6;
            strncpy(pTcpItem->value.time, ctime((const time_t *)&pkthdr->ts.tv_sec), TIME_MAX_LENGTH);
            SendListItem *pSendListItem = dmailListener_SendListItem_init(&(pTcpItem->value));
            
        }
    }
    
    
}

void dmailListener_SMTP_parse(DMailListener *pMailListener, const u_char *packet, const TcpLinkKey *pTcpLinkKey, unsigned long int Seq, char *pPayLoad, const struct pcap_pkthdr *pkthdr)
{
    int paraNum = 0;

    TcpTableItem *pTcpTableItem = (TcpTableItem *)dhashTable_query_by_key(pMailListener->tcpTable, pTcpLinkKey, sizeof(TcpLinkKey), dmailListener_compare_by_TcpLinkKey);
    if(NULL == pTcpTableItem)
    {
        if(0 == strncmp(pPayLoad, "EHLO", strlen("EHLO")))
        {
            pTcpTableItem = dmailListener_TcpTableItem_init(pTcpLinkKey);
            if(NULL == pTcpTableItem)
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
        dmailListener_to_next_state(pTcpTableItem, pMailListener, packet, pTcpLinkKey, Seq, pPayLoad, pkthdr);
    }
    
}

void dmailListener_packet_parse(u_char *userarg, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
    DMailListener *pMailListener = (DMailListener *)userarg;
    struct in_addr IpAddr;
    struct iphdr *pIpHeader;
    struct tcphdr *pTcpHeader;
    char *pPayload;
    TcpLinkKey tcpLink;
    unsigned long int Seq;

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

    pPayload = (char *)(packet + sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct tcphdr)); //得到数据包里内容，不过一般为乱码。

    if(25 == tcpLink.dstPort)
    {
        dmailListener_SMTP_parse(pMailListener, packet, &tcpLink, Seq, pPayload, pkthdr);
    }
    else
    {
        
    }

    return;
}

DMailListener *dmailListener_init(int tcpLinkTableSize, int emailInfoTableSize)
{
    if(tcpLinkTableSize <= 0 || emailInfoTableSize <= 0)
    {
        return NULL;
    }

    DMailListener *pMailListener = (DMailListener *)malloc(sizeof(DMailListener));
    if(NULL == pMailListener)
    {
        return NULL;
    }

    DHashTable *pEmailTable = dhashTable_init(emailInfoTableSize, NULL);
    if(NULL == pEmailTable)
    {
        free(pMailListener);
        return NULL;
    }

    DHashTable *pTcpTable = dhashTable_init(tcpLinkTableSize, dmailListener_hash_by_TcpLinkKey);
    if(NULL == pTcpTable)
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

    char filter_app[] = "dst port 25";

    pcap_compile(handle, &filter, filter_app, 0, 0);
    pcap_setfilter(handle, &filter);

    pcap_loop(handle, 20, dmailListener_packet_parse, (u_char *)(pMailListener));
    pcap_close(handle);
}


int main(void)
{
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

    char filter_app[] = "dst port 25";

    pcap_compile(handle, &filter, filter_app, 0, 0);
    pcap_setfilter(handle, &filter);

    int id = 1;

    pcap_loop(handle, 20, dmailListener_packet_parse, (u_char *)&id);
    pcap_close(handle);

    return 0;
}