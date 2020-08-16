#include "CUnit/CUnit.h"
#include "CUnit/Automated.h"
#include "CUnit/Basic.h"
#include <assert.h>
#include "../include/queue.h"
#include <string.h>

int InitSuite()
{
    return 0;
}

int CleanSuite()
{
    return 0;
}

void TEST_init_queue()
{
    queue *myQueue = init_queue(NULL);
    CU_ASSERT_PTR_NOT_NULL(myQueue);
    delete_queue(myQueue);
}

void TEST_enqueue()
{
    queue *myQueue = init_queue(NULL);
    int *num = (int *)malloc(sizeof(int));
    *num = 5;

    int res = enqueue(myQueue, num);
    CU_ASSERT_EQUAL(res, SUCCESS);

    res = enqueue(NULL, NULL);
    CU_ASSERT_EQUAL(res, NULL_POINTER);

    delete_queue(myQueue);
}

void TEST_get_queue_count()
{
    int *intSet1 = (int *)malloc(sizeof(int));
    int *intSet2 = (int *)malloc(sizeof(int));
    int *intSet3 = (int *)malloc(sizeof(int));

    *intSet1 = 5;
    *intSet2 = 2;
    *intSet3 = 7;
    queue *myQueue = init_queue(NULL);

    enqueue(myQueue, intSet1);
    enqueue(myQueue, intSet2);
    enqueue(myQueue, intSet3);

    int count = get_queue_count(myQueue);
    CU_ASSERT_EQUAL(count, 3);

    count = get_queue_count(NULL);
    CU_ASSERT_EQUAL(count, 0);

    delete_queue(myQueue);
}

void TEST_get_queue_head_data()
{
    int *intSet1 = (int *)malloc(sizeof(int));
    int *intSet2 = (int *)malloc(sizeof(int));
    int *intSet3 = (int *)malloc(sizeof(int));

    *intSet1 = 5;
    *intSet2 = 2;
    *intSet3 = 7;
    queue *myQueue = init_queue(NULL);

    enqueue(myQueue, intSet1);
    enqueue(myQueue, intSet2);
    enqueue(myQueue, intSet3);

    int *head = (int *)get_queue_head_data(myQueue);
    CU_ASSERT_PTR_EQUAL(head, intSet1);

    head = (int *)get_queue_head_data(NULL);
    CU_ASSERT_PTR_NULL(head);

    delete_queue(myQueue);
}

void TEST_dequeue()
{
    int *intSet1 = (int *)malloc(sizeof(int));
    int *intSet2 = (int *)malloc(sizeof(int));
    int *intSet3 = (int *)malloc(sizeof(int));

    *intSet1 = 5;
    *intSet2 = 2;
    *intSet3 = 7;
    queue *myQueue = init_queue(NULL);

    enqueue(myQueue, intSet1);
    enqueue(myQueue, intSet2);
    enqueue(myQueue, intSet3);

    dequeue(myQueue);
    dequeue(myQueue);

    int count = get_queue_count(myQueue);
    CU_ASSERT_EQUAL(count, 1);

    int res = dequeue(NULL);
    CU_ASSERT_EQUAL(res, NULL_POINTER);

    dequeue(myQueue);
    res = dequeue(myQueue);
    CU_ASSERT(res < 0);

    delete_queue(myQueue);
}


int addTestModule()
{
    CU_pSuite pSuite = NULL;

    pSuite = CU_add_suite("queue test module", InitSuite, CleanSuite);
    if(NULL == pSuite)
    {
        return -1;
    }

    if(NULL == CU_add_test(pSuite, "init_queue", TEST_init_queue)
    || NULL == CU_add_test(pSuite, "enqueue", TEST_enqueue)
    || NULL == CU_add_test(pSuite, "get_queue_count", TEST_get_queue_count)
    || NULL == CU_add_test(pSuite, "get_queue_head_data", TEST_get_queue_head_data)
    || NULL == CU_add_test(pSuite, "dequeue", TEST_dequeue)
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