#include "DMemory.h"

int main()
{
    dmemory_init();

    int *a = (int *)dmemory_malloc(10 * sizeof(int));
    int *b = (int *)dmemory_malloc(20 * sizeof(int));

    dmemory_printAllocInfo();

    dmemory_free(a);
    dmemory_free(b);
    dmemory_printSpareInfo();

    dmemory_merge_spare_space();

    dmemory_printSpareInfo();

    dmemory_delete();
    system("pause");
}