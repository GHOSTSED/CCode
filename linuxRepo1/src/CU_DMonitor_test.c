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
    /* 测试创建子进程信息节点时exePath传入NULL的情况 */
    SubProcess *pSubProcess = dmonitor_create_subProcess_info(NULL);
    CU_ASSERT_EQUAL(pSubProcess, NULL);

    /* 测试正常传入路径的情况 */
    pSubProcess = dmonitor_create_subProcess_info("../subProcessExe/sleep_40");
    CU_ASSERT_PTR_NOT_NULL(pSubProcess);

    free(pSubProcess);
}

void TEST_dmonitor_subProcess_init()
{
    /* 测试子进程初始化时exePath传入NULL的情况 */
    int res1 = dmonitor_subProcess_init(NULL);
    CU_ASSERT_EQUAL(res1, NULL_POINTER);

    /* 测试子进程初始化时传入合法（文件存在且可执行）的情况 */
    int res2 = dmonitor_subProcess_init("../subProcessExe/sleep_40");
    CU_ASSERT_EQUAL(res2, INIT_SUCCESS);

    /* 测试子进程初始化时传入不存在的文件路径的情况 */
    int res3 = dmonitor_subProcess_init("../subProcessExe/notExist");
    CU_ASSERT_EQUAL(res3, INIT_FAILED);
}

void TEST_dmonitor_init_and_delete()
{
    /* 测试对DMonitor进行缺省初始化的情况 */
    DMonitor *pMonitor1 = dmonitor_init(NULL, NULL);
    CU_ASSERT_PTR_NOT_NULL(pMonitor1);

    /* 测试传入不存在的exeOrderPath的情况 */
    DMonitor *pMonitor2 = dmonitor_init("../notExistDir/notExistFile.txt", NULL);
    CU_ASSERT_PTR_NULL(pMonitor2);

    /* 测试正常销毁DMonitor的情况 */
    int res1 = dmonitor_delete(pMonitor1);
    CU_ASSERT_EQUAL(res1, SUCCESS);

    /* 测试delete时传入NULL的情况 */
    int res2 = dmonitor_delete(NULL);
    CU_ASSERT_EQUAL(res2, NULL_POINTER);
}

void TEST_dmonitor_find_negative_pid()
{
    SubProcess *pSubProcess1 = dmonitor_create_subProcess_info("../subProcessExe/app_1");
    pSubProcess1->pid = -1;
    /* 测试子进程pid确实为负数的情况 */
    int res1 = dmonitor_find_negative_pid(pSubProcess1, NULL);
    CU_ASSERT_EQUAL(res1, 0);

    pSubProcess1->pid = 1;
    /* 测试传入的子进程pid为正数的情况 */
    int res2 = dmonitor_find_negative_pid(pSubProcess1, NULL);
    CU_ASSERT_EQUAL(res2, -1);

    /* 测试传入空的子进程信息指针的情况 */
    int res3 = dmonitor_find_negative_pid(NULL, NULL);
    CU_ASSERT_EQUAL(res3, -1);

    free(pSubProcess1);
    pSubProcess1 = NULL;
}

void TEST_dmonitor_find_subProcess_by_pid()
{
    /* 测试传入空指针的情况 */
    int res1 = dmonitor_find_subProcess_by_pid(NULL, NULL);
    CU_ASSERT_EQUAL(res1, -1);

    int pid = 10;
    SubProcess *pSubProcess1 = dmonitor_create_subProcess_info("../subProcessExe/app_1");
    pSubProcess1->pid = 10;

    /* 测试vSubProcess的pid与要查找的pid匹配的情况 */
    int res2 = dmonitor_find_subProcess_by_pid(pSubProcess1, &pid);
    CU_ASSERT_EQUAL(res2, 0);

    pSubProcess1->pid = 20;
    
    /* 测试vSubProcess的pid与要查找的pid不匹配的情况 */
    int res3 = dmonitor_find_subProcess_by_pid(pSubProcess1, &pid);
    CU_ASSERT_EQUAL(res3, -1);

    free(pSubProcess1);
    pSubProcess1 = NULL;
}

int addTestModule()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("DMemory test module", InitSuite, CleanSuite);
    if(NULL == pSuite)
    {
        return -1;
    }

    if(
        NULL == CU_add_test(pSuite, "TEST_dmonitor_create_subProcess_info", TEST_dmonitor_create_subProcess_info)
        || NULL == CU_add_test(pSuite, "TEST_dmonitor_subProcess_init", TEST_dmonitor_subProcess_init)
        || NULL == CU_add_test(pSuite, "TEST_dmonitor_init_and_delete", TEST_dmonitor_init_and_delete)
        || NULL == CU_add_test(pSuite, "TEST_dmonitor_find_negative_pid", TEST_dmonitor_find_negative_pid)
        || NULL == CU_add_test(pSuite, "TEST_dmonitor_find_subProcess_by_pid", TEST_dmonitor_find_subProcess_by_pid)
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