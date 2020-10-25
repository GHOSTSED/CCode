#include "CUnit/CUnit.h"
#include "CUnit/Automated.h"
#include "CUnit/Basic.h"
#include <assert.h>
#include <string.h>
#include "../src/DMailListener.c"

int InitSuite()
{
    return 0;
}

int CleanSuite()
{
    return 0;
}

void TEST_dmailListener_init_and_delete()
{
    DMailListener *pMailListener1 = dmailListener_init(-1, -1);
    CU_ASSERT_PTR_NULL(pMailListener1);

    DMailListener *pMailListener2 = dmailListener_init(10, 10);
    CU_ASSERT_PTR_NOT_NULL(pMailListener2);

    int res1 = dmailListener_delete(NULL);
    CU_ASSERT_EQUAL(res1, NULL_POINTER);

    int res2 = dmailListener_delete(pMailListener2);
    CU_ASSERT_EQUAL(res2, SUCCESS);
}

void TEST_dmailListener_EmailTableItem_init()
{
    unsigned char address[] = "<email@zzy.cn>";
    
    EmailTableItem *pEmailTableItem1 = dmailListener_EmailTableItem_init(NULL);
    CU_ASSERT_PTR_NULL(pEmailTableItem1);

    EmailTableItem *pEmailTableItem2 = dmailListener_EmailTableItem_init(address);
    CU_ASSERT_PTR_NOT_NULL(pEmailTableItem2);

    dmailListener_calback_EmailTableItem_delete(pEmailTableItem2);
}

void TEST_dmailListener_SendListItem_and_ReceiveListItem_init()
{
    EmailInfoValue lEmailInfoValue;

    SendListItem *pSendListItem1 = dmailListener_SendListItem_init(NULL);
    CU_ASSERT_PTR_NULL(pSendListItem1);

    SendListItem *pSendListItem2 = dmailListener_SendListItem_init(&lEmailInfoValue);
    CU_ASSERT_PTR_NOT_NULL(pSendListItem2);

    ReceiveListItem *pReceiveListItem1 = dmailListener_ReceiveListItem_init(NULL);
    CU_ASSERT_PTR_NULL(pReceiveListItem1);

    ReceiveListItem *pReceiveListItem2 = dmailListener_ReceiveListItem_init(&lEmailInfoValue);
    CU_ASSERT_PTR_NOT_NULL(pReceiveListItem2);

    free(pSendListItem2);
    free(pReceiveListItem2);
}

void TEST_dmailListener_callback_hash_by_TcpLinkKey()
{
    TcpLinkKey lTcpLinkKey;
    lTcpLinkKey.dstPort = 35;

    unsigned int hash1 = dmailListener_callback_hash_by_TcpLinkKey(NULL, 9);
    CU_ASSERT_EQUAL(hash1, 0);

    unsigned int hash2 = dmailListener_callback_hash_by_TcpLinkKey(&lTcpLinkKey, 9);
    CU_ASSERT_NOT_EQUAL(hash2, 0);
}

void TEST_dmailListener_callback_hash_by_emailAddress()
{
    char address[] = "my@zzy.cn";

    unsigned int hash1 = dmailListener_callback_hash_by_emailAddress(NULL, 9);
    CU_ASSERT_EQUAL(hash1, 0);

    unsigned int hash2 = dmailListener_callback_hash_by_emailAddress(address, 9);
    CU_ASSERT_NOT_EQUAL(hash2, 0);
}

void TEST_dmailListener_callback_compare_by_TcpLinkKey()
{
    TcpTableItem lTcpTableItem;
    TcpLinkKey lTcpLinkKey;

    lTcpTableItem.key.dstPort = 15;
    lTcpLinkKey.dstPort = 15;

    lTcpTableItem.key.srcPort = 20;
    lTcpLinkKey.srcPort = 20;

    strncpy((char *)lTcpTableItem.key.srcIP, "192.168.1.1", INET_ADDRSTRLEN);
    strncpy((char *)lTcpLinkKey.srcIP, "192.168.1.1", INET_ADDRSTRLEN);

    strncpy((char *)lTcpTableItem.key.dstIP, "192.168.2.1", INET_ADDRSTRLEN);
    strncpy((char *)lTcpLinkKey.dstIP, "192.168.2.1", INET_ADDRSTRLEN);

    int res1 = dmailListener_callback_compare_by_TcpLinkKey(&lTcpTableItem, &lTcpLinkKey);
    CU_ASSERT_EQUAL(res1, 0);

    lTcpLinkKey.srcPort = 50;
    int res2 = dmailListener_callback_compare_by_TcpLinkKey(&lTcpTableItem, &lTcpLinkKey);
    CU_ASSERT_NOT_EQUAL(res2, 0);

    int res3 = dmailListener_callback_compare_by_TcpLinkKey(NULL, NULL);
    CU_ASSERT_NOT_EQUAL(res3, 0);
}

void TEST_dmailListener_callback_compare_by_EmailAddress()
{
    int res1 = dmailListener_callback_compare_by_EmailAddress(NULL, NULL);
    CU_ASSERT_NOT_EQUAL(res1, 0);

    EmailTableItem lEmailTableItem;
    strncpy((char *)lEmailTableItem.userAddress, "user@zzy.cn", EMAIL_ADDRESS_MAX_LENGTH);
    char address[EMAIL_ADDRESS_MAX_LENGTH] = "user@zzy.cn";

    int res2 = dmailListener_callback_compare_by_EmailAddress(&lEmailTableItem, address);
    CU_ASSERT_EQUAL(res2, 0);

    address[0] = '<';
    int res3 = dmailListener_callback_compare_by_EmailAddress(&lEmailTableItem, address);
    CU_ASSERT_NOT_EQUAL(res3, 0);
}

void TEST_dmailListener_TcpTableItem_init()
{
    TcpLinkKey lTcpLinkKey;
    lTcpLinkKey.dstPort = 15;
    lTcpLinkKey.srcPort = 20;
    strncpy((char *)lTcpLinkKey.srcIP, "192.168.1.1", INET_ADDRSTRLEN);
    strncpy((char *)lTcpLinkKey.dstIP, "192.168.2.1", INET_ADDRSTRLEN);

    TcpTableItem *pTcpTableItem1 = dmailListener_TcpTableItem_init(NULL);
    CU_ASSERT_PTR_NULL(pTcpTableItem1);

    TcpTableItem *pTcpTableItem2 = dmailListener_TcpTableItem_init(&lTcpLinkKey);
    CU_ASSERT_PTR_NOT_NULL(pTcpTableItem2);

    free(pTcpTableItem2);
}


int addTestModule()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("DMailListener test module", InitSuite, CleanSuite);
    if(NULL == pSuite)
    {
        return -1;
    }

    if(NULL == CU_add_test(pSuite, "TEST_dmailListener_init_and_delete", TEST_dmailListener_init_and_delete)
    || NULL == CU_add_test(pSuite, "TEST_dmailListener_EmailTableItem_init", TEST_dmailListener_EmailTableItem_init)
    || NULL == CU_add_test(pSuite, "TEST_dmailListener_SendListItem_and_ReceiveListItem_init", TEST_dmailListener_SendListItem_and_ReceiveListItem_init)
    || NULL == CU_add_test(pSuite, "TEST_dmailListener_callback_hash_by_TcpLinkKey", TEST_dmailListener_callback_hash_by_TcpLinkKey)
    || NULL == CU_add_test(pSuite, "TEST_dmailListener_callback_hash_by_emailAddress", TEST_dmailListener_callback_hash_by_emailAddress)
    || NULL == CU_add_test(pSuite, "TEST_dmailListener_callback_compare_by_TcpLinkKey", TEST_dmailListener_callback_compare_by_TcpLinkKey)
    || NULL == CU_add_test(pSuite, "TEST_dmailListener_callback_compare_by_EmailAddress", TEST_dmailListener_callback_compare_by_EmailAddress)
    || NULL == CU_add_test(pSuite, "TEST_dmailListener_TcpTableItem_init", TEST_dmailListener_TcpTableItem_init)
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