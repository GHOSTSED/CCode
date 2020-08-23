#include "CUnit/CUnit.h"
#include "CUnit/Automated.h"
#include "CUnit/Basic.h"
#include <assert.h>
#include <string.h>
#include "../src/DMemory.c"

int InitSuite()
{
    return 0;
}

int CleanSuite()
{
    return 0;
}

void TEST_dmemory_HList_init_and_delete()
{
    /* 测试HList正常初始化功能 */
    HList *pHList = dmemory_HList_init();
    CU_ASSERT_PTR_NOT_NULL(pHList);

    /* 测试HList正常销毁功能 */
    int res1 = dmemory_HList_delete(pHList);
    CU_ASSERT_EQUAL(res1, OP_SUCCESS);

    /* 测试销毁时传入空指针的情况 */
    int res2 = dmemory_HList_delete(NULL);
    CU_ASSERT_EQUAL(res2, NULL_POINTER);
}

void TEST_dmemory_Block_init_and_delete()
{
    /* 测试Block正常初始化功能 */
    Block *pBlock = dmemory_Block_init();
    CU_ASSERT_PTR_NOT_NULL(pBlock);

    /* 测试Block正常销毁功能 */
    int res1 = dmemory_Block_delete(pBlock);
    CU_ASSERT_EQUAL(res1, OP_SUCCESS);

    /* 测试销毁时传入NULL的情况 */
    int res2 = dmemory_Block_delete(NULL);
    CU_ASSERT_EQUAL(res2, NULL_POINTER);
}

void TEST_dmemory_init_and_delete()
{
    /* 测试pBlockList正常初始化功能 */
    int res = dmemory_init();
    CU_ASSERT_EQUAL(res, INIT_SUCCESS);

    /* 测试pBlockList正常销毁功能 */
    dmemory_delete();
    CU_ASSERT_PTR_NULL(pBlockList);
}

void TEST_dmemory_get_memAddr_by_header()
{
    /* 测试传入NULL的情况 */
    void *res1 = dmemory_get_memAddr_by_header(NULL);
    CU_ASSERT_PTR_NULL(res1);

    Header *pHeader = (Header *)malloc(sizeof(Header));
    if(NULL == pHeader)
    {
        return;
    }
    /* 测试正常传入对象的情况 */
    void *res2 = dmemory_get_memAddr_by_header(pHeader);
    CU_ASSERT_PTR_NOT_NULL(res2);

    free(pHeader);
}

void TEST_dmemory_WorstFit_search_spare()
{
    /* 测试查找内存过大的情况 */
    dmemory_init();
    void *res1 = dmemory_WorstFit_search_spare(BLOCK_SIZE);
    CU_ASSERT_PTR_NULL(res1);

    /* 测试正常获取内存的情况 */
    void *res2 = dmemory_WorstFit_search_spare(256);
    CU_ASSERT_PTR_NOT_NULL(res2);
    dmemory_delete();
}

void TEST_dmemory_BestFit_search_spare()
{
    dmemory_init();
    /* 测试查找内存过大的情况 */
    void *res1 = dmemory_BestFit_search_spare(BLOCK_SIZE);
    CU_ASSERT_PTR_NULL(res1);

    /* 测试正常获取内存的情况 */
    void *res2 = dmemory_BestFit_search_spare(256);
    CU_ASSERT_PTR_NOT_NULL(res2);
    dmemory_delete();
}

void TEST_dmemory_get_next_header()
{
    /* 测试传入NULL的情况 */
    Header *res1 = dmemory_get_next_header(NULL);
    CU_ASSERT_PTR_NULL(res1);
}

void TEST_dmemory_move_to_alloc()
{
    /* 测试传入NULL的情况 */
    int res1 = dmemory_move_to_alloc(NULL);
    CU_ASSERT_EQUAL(res1, NULL_POINTER);
}

void TEST_dmemory_move_to_spare()
{
    /* 测试传入NULL的情况 */
    int res1 = dmemory_move_to_spare(NULL);
    CU_ASSERT_EQUAL(res1, NULL_POINTER);
}

void TEST_dmemory_insert_to_alloc()
{
    dmemory_init();
    /* 测试传入NULL的情况 */
    int res1 = dmemory_insert_to_alloc(NULL);
    CU_ASSERT_EQUAL(res1, NULL_POINTER);

    /* 测试正常传入指针的情况 */
    Header myHeader;
    int res2 = dmemory_insert_to_alloc(&myHeader);
    CU_ASSERT_EQUAL(res2, OP_SUCCESS);
    dmemory_delete();
}

void TEST_dmemory_insert_to_spare()
{
    dmemory_init();
    /* 测试传入NULL的情况 */
    int res1 = dmemory_insert_to_spare(NULL);
    CU_ASSERT_EQUAL(res1, NULL_POINTER);

    Header myHeader;
    /* 测试正常传入指针的情况 */
    int res2 = dmemory_insert_to_spare(&myHeader);
    CU_ASSERT_EQUAL(res2, OP_SUCCESS);
    dmemory_delete();
}

void TEST_dmemory_remove_from_HList()
{
    /* 测试传入NULL的情况 */
    int res = dmemory_remove_from_HList(NULL);
    CU_ASSERT_EQUAL(res, NULL_POINTER);
}

void TEST_dmemory_is_header_in_alloc()
{
    /* 测试传入NULL的情况 */
    int res1 = dmemory_is_header_in_alloc(NULL);
    CU_ASSERT_EQUAL(res1, NULL_POINTER);

    /* 测试正常传入指针的情况 */
    dmemory_init();
    Header myHeader;
    int res2 = dmemory_is_header_in_alloc(&myHeader);
    CU_ASSERT_EQUAL(res2, NOT_EXIST);

    dmemory_delete();
}

void TEST_dmemory_malloc()
{
    dmemory_init();
    /* 测试获取内存过大的情况 */
    void *res1 = dmemory_malloc(BLOCK_SIZE);
    CU_ASSERT_PTR_NULL(res1);

    /* 测试正常获取内存的情况 */
    void *res2 = dmemory_malloc(64);
    CU_ASSERT_PTR_NOT_NULL(res2);

    dmemory_delete();
}

void TEST_dmemory_free()
{
    dmemory_init();
    /* 测试传入NULL的情况 */
    int res1 = dmemory_free(NULL);
    CU_ASSERT_EQUAL(res1, NULL_POINTER);

    /* 测试传入非法地址的情况 */
    int mem;
    int res2 = dmemory_free(&mem);
    CU_ASSERT_EQUAL(res2, ILLEGAL_INDEX);

    void *unit64 = dmemory_malloc(64);
    if(NULL != unit64)
    {
        /* 测试正常释放内存的情况 */
        int res3 = dmemory_free(unit64);
        CU_ASSERT_EQUAL(res3, OP_SUCCESS);
    }

    dmemory_delete();
}

void TEST_dmemory_calloc()
{
    dmemory_init();
    /* 测试申请内存过大的情况 */
    void *res1 = dmemory_calloc(BLOCK_SIZE);
    CU_ASSERT_PTR_NULL(res1);

    /* 测试正常审清内存的情况 */
    void *res2 = dmemory_calloc(64);
    CU_ASSERT_PTR_NOT_NULL(res2);

    dmemory_delete();
}

void TEST_dmemory_realloc()
{
    dmemory_init();

    /* 测试传入空指针和0大小的情况 */
    void *res1 = dmemory_realloc(NULL, 0);
    CU_ASSERT_PTR_NULL(res1);

    /* 测试传入非空的非法地址的情况 */
    int mem;
    void *res2 = dmemory_realloc(&mem, 20);
    CU_ASSERT_PTR_NULL(res2);

    void *addr = dmemory_malloc(64);
    if(NULL != addr)
    {
        /* 测试申请内存过大的情况 */
        void *res3 = dmemory_realloc(addr, BLOCK_SIZE);
        CU_ASSERT_PTR_NULL(res3);

        /* 测试正常进行重分配的情况 */
        void *res4 = dmemory_realloc(addr, 128);
        CU_ASSERT_PTR_NOT_NULL(res4);
    }

    dmemory_delete();
}

int addTestModule()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("DMemory test module", InitSuite, CleanSuite);
    if(NULL == pSuite)
    {
        return -1;
    }

    if(NULL == CU_add_test(pSuite, "TEST_dmemory_HList_init_and_delete", TEST_dmemory_HList_init_and_delete)
    || NULL == CU_add_test(pSuite, "TEST_dmemory_Block_init_and_delete", TEST_dmemory_Block_init_and_delete)
    || NULL == CU_add_test(pSuite, "TEST_dmemory_init_and_delete", TEST_dmemory_init_and_delete)
    || NULL == CU_add_test(pSuite, "TEST_dmemory_WorstFit_search_spare", TEST_dmemory_WorstFit_search_spare)
    || NULL == CU_add_test(pSuite, "TEST_dmemory_BestFit_search_spare", TEST_dmemory_BestFit_search_spare)
    || NULL == CU_add_test(pSuite, "TEST_dmemory_get_memAddr_by_header", TEST_dmemory_get_memAddr_by_header)
    || NULL == CU_add_test(pSuite, "TEST_dmemory_get_next_header", TEST_dmemory_get_next_header)
    || NULL == CU_add_test(pSuite, "TEST_dmemory_move_to_alloc", TEST_dmemory_move_to_alloc)
    || NULL == CU_add_test(pSuite, "TEST_dmemory_move_to_spare", TEST_dmemory_move_to_spare)
    || NULL == CU_add_test(pSuite, "TEST_dmemory_insert_to_alloc", TEST_dmemory_insert_to_alloc)
    || NULL == CU_add_test(pSuite, "TEST_dmemory_insert_to_spare", TEST_dmemory_insert_to_spare)
    || NULL == CU_add_test(pSuite, "TEST_dmemory_remove_from_HList", TEST_dmemory_remove_from_HList)
    || NULL == CU_add_test(pSuite, "TEST_dmemory_is_header_in_alloc", TEST_dmemory_is_header_in_alloc)
    || NULL == CU_add_test(pSuite, "TEST_dmemory_malloc", TEST_dmemory_malloc)
    || NULL == CU_add_test(pSuite, "TEST_dmemory_free", TEST_dmemory_free)
    || NULL == CU_add_test(pSuite, "TEST_dmemory_calloc", TEST_dmemory_calloc)
    || NULL == CU_add_test(pSuite, "TEST_dmemory_realloc", TEST_dmemory_realloc)
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