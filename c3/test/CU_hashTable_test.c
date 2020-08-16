#include "CUnit/CUnit.h"
#include "CUnit/Automated.h"
#include "CUnit/Basic.h"
#include <assert.h>
#include "../include/hashTable.h"
#include <string.h>

int InitSuite()
{
    return 0;
}

int CleanSuite()
{
    return 0;
}

typedef struct DNS{
    char ip[20];
    char hostName[20];
} DNS;

int find_by_hostName(const void *listData, const void *findData)
{
    DNS *ListData = (DNS *)listData;
    char *hostName = (char *)findData;

    return strcmp(ListData->hostName, hostName);
}

int find_by_dns(const void *listDNS, const void *findDNS)
{
    DNS *ListDNS = (DNS *)listDNS;
    DNS *FindDNS = (DNS *)findDNS;

    if(0 == strcmp(ListDNS->hostName, FindDNS->hostName))
    {
        return (strcmp(ListDNS->ip, FindDNS->ip));
    }
    else
    {
        return -1;
    }
    
}

void TEST_init_hashTable()
{
    hashTable *pHashTable = init_hashTable(0, NULL);
    CU_ASSERT_EQUAL(pHashTable, NULL);
    delete_hashTable(pHashTable);
}

void TEST_insert_data_to_hashTable()
{
    hashTable *pHashTable = init_hashTable(4, NULL);
    assert(NULL != pHashTable);
    int *data = (int *)malloc(sizeof(int));
    *data = 5;
    int res1 = insert_data_to_hashTable(pHashTable, "223", 0, data);//测试keySize传入0的情况
    int res2 = insert_data_to_hashTable(NULL, "223", 3, data);//测试pHashTable传入NULL的情况
    int res3 = insert_data_to_hashTable(pHashTable, NULL, 0, data);//测试key传入NULL的情况
    int res4 = insert_data_to_hashTable(pHashTable, "223", 3, NULL);//测试data传入NULL的情况

    int res5 = insert_data_to_hashTable(pHashTable, "223", 3, data);//测试均传入合法值的情况

    CU_ASSERT_EQUAL(res1, NULL_POINTER);
    CU_ASSERT_EQUAL(res2, NULL_POINTER);
    CU_ASSERT_EQUAL(res3, NULL_POINTER);
    CU_ASSERT_EQUAL(res4, NULL_POINTER);

    CU_ASSERT_EQUAL(res5, SUCCESS);

    delete_hashTable(pHashTable);
}

void TEST_query_hashTable_by_key()
{
    hashTable *myHashTable = init_hashTable(20, NULL);

    DNS *dns1 = (DNS *)malloc(sizeof(DNS));
    DNS *dns2 = (DNS *)malloc(sizeof(DNS));

    memset(dns1, 0, sizeof(DNS));
    memset(dns2, 0, sizeof(DNS));

    strncpy(dns1->ip, "192.168.1.1", 20);
    strncpy(dns1->hostName, "tp-link.net", 20);

    strncpy(dns2->ip, "168.171.137.1", 20);
    strncpy(dns2->hostName, "www.baidu.com", 20);

    insert_data_to_hashTable(myHashTable, dns1->hostName, 20, dns1);
    insert_data_to_hashTable(myHashTable, dns2->hostName, 20, dns2);

    char searchHostName[20];
    strncpy(searchHostName, "www.sina.com", 20);
    DNS *pAnsDNS = (DNS *)query_hashTable_by_key(myHashTable, searchHostName, 20, find_by_hostName);//试图查找不存在的域名
    CU_ASSERT_PTR_NULL(pAnsDNS);

    pAnsDNS = (DNS *)query_hashTable_by_key(NULL, NULL, 0, NULL);//尝试传入非法值的情况
    CU_ASSERT_PTR_NULL(pAnsDNS);

    strncpy(searchHostName, "www.baidu.com", 20);
    pAnsDNS = (DNS *)query_hashTable_by_key(myHashTable, searchHostName, 20, find_by_hostName);//试图查找存在哈希表中的百度的DNS条目
    CU_ASSERT_STRING_EQUAL(pAnsDNS->ip, "168.171.137.1");

    delete_hashTable(myHashTable);
}

void TEST_get_count_of_hashTable()
{
    hashTable *myHashTable = init_hashTable(20, NULL);

    DNS *dns1 = (DNS *)malloc(sizeof(DNS));
    DNS *dns2 = (DNS *)malloc(sizeof(DNS));

    memset(dns1, 0, sizeof(DNS));
    memset(dns2, 0, sizeof(DNS));

    strncpy(dns1->ip, "192.168.1.1", 20);
    strncpy(dns1->hostName, "tp-link.net", 20);

    strncpy(dns2->ip, "168.171.137.1", 20);
    strncpy(dns2->hostName, "www.baidu.com", 20);

    insert_data_to_hashTable(myHashTable, dns1->hostName, 20, dns1);
    insert_data_to_hashTable(myHashTable, dns2->hostName, 20, dns2);

    int count = get_count_of_hashTable(myHashTable);
    CU_ASSERT_EQUAL(count, 2);

    delete_hashTable(myHashTable);
}

void TEST_delete_node_from_hashTable()
{
    hashTable *myHashTable = init_hashTable(20, NULL);

    DNS *dns1 = (DNS *)malloc(sizeof(DNS));
    DNS *dns2 = (DNS *)malloc(sizeof(DNS));
    DNS *dns3 = (DNS *)malloc(sizeof(DNS));

    memset(dns1, 0, sizeof(DNS));
    memset(dns2, 0, sizeof(DNS));
    memset(dns3, 0, sizeof(DNS));

    strncpy(dns1->ip, "192.168.1.1", 20);
    strncpy(dns1->hostName, "www.baidu.com", 20);

    strncpy(dns2->ip, "168.171.137.1", 20);
    strncpy(dns2->hostName, "www.baidu.com", 20);

    strncpy(dns3->ip, "172.138.1.1", 20);
    strncpy(dns3->hostName, "www.baidu.com", 20);

    insert_data_to_hashTable(myHashTable, dns1->hostName, 20, dns1);
    insert_data_to_hashTable(myHashTable, dns2->hostName, 20, dns2);
    insert_data_to_hashTable(myHashTable, dns3->hostName, 20, dns3);

    DNS findDNS;
    strncpy(findDNS.hostName, "www.baidu.com", 20);
    strncpy(findDNS.ip, "172.138.1.1", 20);
    int res1 = delete_node_from_hashTable(myHashTable, findDNS.hostName, 20, &findDNS, find_by_dns);//测试根据data删除
    CU_ASSERT_EQUAL(res1, 1);

    int res2 = delete_node_from_hashTable(myHashTable, findDNS.hostName, 20, NULL, find_by_hostName);//测试根据key删除
    CU_ASSERT_EQUAL(res2, 2);

    int res3 = delete_node_from_hashTable(NULL, NULL, 0, NULL, NULL);//测试传入非法值
    CU_ASSERT_EQUAL(res3, NULL_POINTER);

    delete_hashTable(myHashTable);
}


int addTestModule()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("hashTable test module", InitSuite, CleanSuite);
    if(NULL == pSuite)
    {
        return -1;
    }

    if(NULL == CU_add_test(pSuite, "init_hashTable", TEST_init_hashTable)
    || NULL == CU_add_test(pSuite, "insert_data_to_hashTable", TEST_insert_data_to_hashTable)
    || NULL == CU_add_test(pSuite, "query_hashTable_by_key", TEST_query_hashTable_by_key)
    || NULL == CU_add_test(pSuite, "get_count_of_hashTable", TEST_get_count_of_hashTable)
    || NULL == CU_add_test(pSuite, "delete_node_from_hashTable", TEST_delete_node_from_hashTable)
    )
    {
        return -1;
    }

    return 0;
}

void run_test()
{
    if(CU_initialize_registry())
    {
        printf("CU_initialize_registry()!\n");
        return;
    }

    assert(NULL != CU_get_registry());
    assert(!CU_is_test_running());

    if(0 != addTestModule())
    {
        CU_cleanup_registry();
        return;
    }

    CU_basic_set_mode(CU_BRM_NORMAL);
    CU_basic_run_tests();

    CU_cleanup_registry();
}

int main()
{
    run_test();
    return 0;
}
