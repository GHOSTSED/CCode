#include "../include/DMemory.h"
#include <string.h>

int main()
{
    int res;
    int i;
    int *numSet = NULL; 
    dmemory_init();
    /* A001:测试dmemory_malloc能否进行正常的内存分配 */
    printf("***********************Test1**************************\n");
    numSet = (int *)dmemory_malloc(20 * sizeof(int));
    for(i = 0; i < 20; i++)
    {
        numSet[i] = i;
    }

    for(i = 0; i < 20; i++)
    {
        printf("%d ", numSet[i]);
    }
    printf("\n");

    /* A002:测试申请过大内存的情况 */
    printf("***********************Test2**************************\n");
    numSet = dmemory_malloc(64*1024);
    if(NULL == numSet)
    {
        printf("malloc failed, Test2 pass!\n");
    }
    else
    {
        printf("malloc success, Test2 failed!\n");
    }
    
    /* A003:测试dmemory_calloc是否将内存块置0 */
    printf("***********************Test3**************************\n");
    numSet = dmemory_calloc(20 * sizeof(int));
    for(i = 0; i < 20; i++)
    {
        printf("%d ", numSet[i]);
    }
    printf("\n");

    /* A004:测试内存正常进行重分配情况 */
    printf("***********************Test4**************************\n");
    numSet = dmemory_realloc(numSet, 40 * sizeof(int));
    for(i = 0; i < 40; i++)
    {
        numSet[i] = i;
    }

    for(i = 0; i < 40; i++)
    {
        printf("%d ", numSet[i]);
    }
    printf("\n");

    /* A005:测试正常进行内存释放的情况 */
    printf("***********************Test5**************************\n");
    res = dmemory_free(numSet);
    if(OP_SUCCESS == res)
    {
        printf("free success!\n");
    }
    else
    {
        printf("free failed\n");
    }
    

    dmemory_delete();
    return 0;
}