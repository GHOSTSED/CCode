#include "DMemory.h"
#include <stdlib.h>
#include <time.h>

int main()
{
    // dmemory_init();

    // int *a = (int *)dmemory_malloc(10 * sizeof(int));
    // int *b = (int *)dmemory_malloc(20 * sizeof(int));

    // dmemory_printAllocInfo();

    // dmemory_free(a);
    // dmemory_free(b);
    // dmemory_printSpareInfo();

    // dmemory_merge_spare_space();

    // dmemory_printSpareInfo();

    // dmemory_delete();
    // system("pause");
    dmemory_init();
    srand(time(NULL));
    void **lloc[512];
    int i;
    for(i = 0; i < 512; i++)
    {
        lloc[i] = dmemory_malloc(40 + rand()%512);
    }
    dmemory_printAllocInfo();

    for(i = 0; i < 256; i++)
    {
        dmemory_free(lloc[i]);
    }
    printf("**************************************before merge******************************************\n");
    dmemory_printAllocInfo();
    dmemory_printSpareInfo();

    dmemory_merge_spare_space();

    printf("**************************************after merge******************************************\n");
    dmemory_printSpareInfo();   

    dmemory_delete();
    system("pause");
}