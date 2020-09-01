#include "CUnit/CUnit.h"
#include "CUnit/Automated.h"
#include "CUnit/Basic.h"
#include <assert.h>
#include <string.h>
#include "./DMonitor.c"

int InitSuite()
{
    return 0;
}

int CleanSuite()
{
    return 0;
}

void TEST_dmonitor_create_subProcess_info()
{
    SubProcess *pSubProcess = dmonitor_create_subProcess_info(NULL);
    CU_ASSERT_EQUAL(pSubProcess, NULL);

    pSubProcess = dmonitor_create_subProcess_info("../subProcessExe/sleep_40");
    CU_ASSERT_PTR_NOT_NULL(pSubProcess);

    free(pSubProcess);
}

void TEST_dmonitor_subProcess_init()
{
    int res1 = dmonitor_subProcess_init(NULL);
    CU_ASSERT_EQUAL(res1, NULL_POINTER);

    int res2 = dmonitor_subProcess_init("../subProcessExe/sleep_40");
    CU_ASSERT_EQUAL(res2, INIT_SUCCESS);

    int res3 = dmonitor_subProcess_init("../subProcessExe/notExist");
    CU_ASSERT_EQUAL(res3, INIT_FAILED);
}

void TEST_dmonitor_init_and_delete()
{
    DMonitor *pMonitor1 = dmonitor_init(NULL, NULL);
    CU_ASSERT_PTR_NOT_NULL(pMonitor1);

    DMonitor *pMonitor2 = dmonitor_init("../notExistDir/notExistFile.txt", NULL);
    CU_ASSERT_PTR_NULL(pMonitor2);

    int res1 = dmonitor_delete(pMonitor1);
    CU_ASSERT_EQUAL(res1, SUCCESS);

    int res2 = dmonitor_delete(NULL);
    CU_ASSERT_EQUAL(res2, NULL_POINTER);
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