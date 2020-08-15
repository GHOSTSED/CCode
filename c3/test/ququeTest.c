#include"../include/queue.h"

void printInt(void *intNum)
{
    printf("==>%d", *((int *)intNum));
}

int main()
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

    print_queue(myQueue, printInt);

    dequeue(myQueue);
    print_queue(myQueue, printInt);

    delete_queue(myQueue);

    system("pause");
}