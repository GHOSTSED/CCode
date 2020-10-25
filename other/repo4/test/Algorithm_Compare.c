#include "../include/DMemory.h"
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

int main()
{
    struct timeval start, end;
    int i = 0;
    int res = 0;
    int timeUse = 0;
    unsigned int frag = 0;
    void *lloc[512];
    srand(time(NULL));
    
    /* 测试Best-fit下的内存分配效率和碎片状况 */
    printf("************************Best-fit Test**********************\n");
    dmemory_init();

    gettimeofday(&start, NULL);
    for(i = 0; i < 512; i++)
    {
        lloc[i] = dmemory_malloc(4 + rand()%1021);
    }
    gettimeofday(&end, NULL);

    timeUse = 1000000*(end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    printf("alloc memory for 512 times cost: %d us\n", timeUse);

    for(i = 0; i < 256; )
    {
        res = dmemory_free(lloc[rand()%512]);
        if(OP_SUCCESS == res)
        {
            i++;
        }
    }
    frag = dmemory_fragment_space_count();
    printf("fragment size: %d\n", frag);

    dmemory_delete();

    /* 测试Worst-fit下的内存分配效率和碎片状况 */
    printf("************************Worst-fit Test**********************\n");
    dmemory_init();

    dmemory_set_strategy(STRATEGY_WORST_FIT);
    gettimeofday(&start, NULL);
    for(i = 0; i < 512; i++)
    {
        lloc[i] = dmemory_malloc(4 + rand()%1021);
    }
    gettimeofday(&end, NULL);

    timeUse = 1000000*(end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    printf("alloc memory for 512 times cost: %d us\n", timeUse);

    for(i = 0; i < 256; )
    {
        res = dmemory_free(lloc[rand()%512]);
        if(OP_SUCCESS == res)
        {
            i++;
        }
    }
    frag = dmemory_fragment_space_count();
    printf("fragment size: %d\n", frag);

    dmemory_delete();
    return 0;
}