#include "../include/hashTable.h"
#include<string.h>

typedef struct dns{
    char ip[20];
    char hostName[20];
} dns;

void print_dns(const void *pDNS)
{
    dns *pdns = (dns *)pDNS;
    printf("HostName:%s\nIP:%s\n", pdns->hostName, pdns->ip);
}

int find_by_hostName(const void *listData, const void *findData)
{
    dns *ListData = (dns *)listData;
    char *hostName = (char *)findData;

    return strcmp(ListData->hostName, hostName);
}

int main()
{
    hashTable *myHashTable = init_hashTable(20, NULL);

    dns *dns1 = (dns *)malloc(sizeof(dns));
    dns *dns2 = (dns *)malloc(sizeof(dns));

    memset(dns1, 0, sizeof(dns));
    memset(dns2, 0, sizeof(dns));

    strncpy(dns1->ip, "192.168.1.1", 20);
    strncpy(dns1->hostName, "tp-link.net", 20);

    strncpy(dns2->ip, "168.171.137.1", 20);
    strncpy(dns2->hostName, "www.baidu.com", 20);

    insert_data_to_hashTable(myHashTable, dns1->hostName, 20, dns1);
    insert_data_to_hashTable(myHashTable, dns2->hostName, 20, dns2);

    print_hashTable(myHashTable, print_dns);

    printf("*******************************************************************************\n");
    char hn[20];
    memset(hn, 0, 20);
    strncpy(hn, "www.baidu.com", 20);
    dns *pDns = (dns *)query_hashTable_by_key(myHashTable, hn, 20, find_by_hostName);
    if(NULL == pDns)
    {
        printf("NULL!\n");
    }
    else
    {
        printf("%s", pDns->ip);
    }
    

    delete_hashTable(myHashTable);
    system("pause");
}