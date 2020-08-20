#include "DMemory.h"


struct _Header{
    struct _Header *pre;
    struct _Header *next;
    unsigned char available;
    unsigned int size;
};

struct _HList{
    Header *first;
    Header *last;
};

struct _Block{
    void *block;

    struct _Block *pre;
    struct _Block *next;
};

struct _BlockList{
    HList *allocList;
    HList *spareList;

    Block *first;
    Block *last;
};

typedef struct _Header Header;
typedef struct _HList HList;
typedef struct _Block Block;
typedef struct _BlockList BlockList;

static BlockList *pBlockList = NULL;

static HList *dmemory_HList_init();
static void dmemory_HList_delete(HList *pHList);
static Block *dmemory_Block_init();
static void dmemory_Block_delete(Block *ptrBlock);
static void dmemory_insert_to_spare(Header *pInsertHeader);
int dmemory_init();
static void *dmemory_get_memAddr_by_header(Header *pHeader);
static void dmemory_move_to_alloc(Header *pHeader);
static void dmemory_move_to_spare(Header *pDataHeader);
static void dmemory_insert_to_alloc(Header *pHeader);
static void *dmemory_BestFit_search_spare(unsigned int dataSize);
static Header *dmemory_get_next_header(Header *pHeader);
static void dmemory_remove_from_spare(Header *pHeader);
void dmemory_merge_spare_space();
void dmemory_printAllocInfo();
void dmemory_printSpareInfo();
void dmemory_delete();
void *dmemory_malloc(unsigned int dataSize);
void dmemory_free(void *pData);
void *dmemory_calloc(unsigned int dataSize);
void *dmemory_realloc(void *srcPtr, unsigned int dataSize);


static HList *dmemory_HList_init()
{
    HList *pHList = (HList *)malloc(sizeof(HList));
    if(NULL == pHList)
    {
        return NULL;
    }

    Header *first = (Header *)malloc(sizeof(Header));
    if(NULL == first)
    {
        free(pHList);
        pHList = NULL;
        return NULL;
    }

    Header *last = (Header *)malloc(sizeof(Header));
    if(NULL == last)
    {
        free(pHList);
        free(first);
        pHList = NULL;
        first = NULL;
        return NULL;
    }

    first->size = 0;
    first->next = last;
    first->pre = NULL;

    last->size = 0;
    last->next = NULL;
    last->pre = first;

    pHList->first = first;
    pHList->last = last;

    return pHList;
}

static void dmemory_HList_delete(HList *pHList)
{
    if(NULL == pHList)
    {
        return ;
    }

    free(pHList->first);
    free(pHList->last);
    free(pHList);
}

static Block *dmemory_Block_init()
{
    Block *pBlock = (Block *)malloc(sizeof(Block));
    if(NULL == pBlock)
    {
        return NULL;
    }

    void *block = malloc(BLOCK_SIZE + sizeof(Header));
    if(NULL == block)
    {
        free(pBlock);
        pBlock = NULL;
        return NULL;
    }

    pBlock->block = block;
    pBlock->pre = NULL;
    pBlock->next = NULL;

    Header *endHeader = (Header *)(block + BLOCK_SIZE);
    endHeader->size = 0;
    return pBlock;
}

static void dmemory_Block_delete(Block *ptrBlock)
{
    if(NULL == ptrBlock)
    {
        return;
    }

    free(ptrBlock->block);
    free(ptrBlock);
    ptrBlock = NULL;
}

static void dmemory_insert_to_spare(Header *pInsertHeader)
{
    Header *pTravelHeader = pBlockList->spareList->first->next;
    while (pTravelHeader != pBlockList->spareList->last && pInsertHeader->size > pTravelHeader->size)
    {
        pTravelHeader = pTravelHeader->next;
    }
    
    Header *pFrontHeader = pTravelHeader->pre;
    pFrontHeader->next = pInsertHeader;

    pInsertHeader->pre = pFrontHeader;
    pInsertHeader->next = pTravelHeader;

    pTravelHeader->pre = pInsertHeader;

    pInsertHeader->available = 0xff;
}

int dmemory_init()
{
    pBlockList = (BlockList *)malloc(sizeof(BlockList));
    if(NULL == pBlockList)
    {
        return MALLOC_FAILED;
    }

    Block *firstBlock = dmemory_Block_init();
    if(NULL == firstBlock)
    {
        free(pBlockList);
        pBlockList = NULL;
        return MALLOC_FAILED;
    }

    HList *allocList = dmemory_HList_init();
    if(NULL == allocList)
    {
        free(pBlockList);
        dmemory_Block_delete(firstBlock);
        pBlockList = NULL;
        firstBlock = NULL;
        return MALLOC_FAILED;
    }

    HList *spareList = dmemory_HList_init();
    if(NULL == spareList)
    {
        free(pBlockList);
        dmemory_Block_delete(firstBlock);
        dmemory_HList_delete(allocList);
        allocList = NULL;
        pBlockList = NULL;
        firstBlock = NULL;
        return MALLOC_FAILED;
    }

    pBlockList->allocList = allocList;
    pBlockList->spareList = spareList;
    pBlockList->first = firstBlock;
    pBlockList->last = firstBlock;

    Header *firstSpare = (Header *)(firstBlock->block);
    firstSpare->available = 0xff;
    firstSpare->size = BLOCK_SIZE - sizeof(Header);
    dmemory_insert_to_spare(firstSpare);
    return INIT_SUCCESS;
}

static void *dmemory_get_memAddr_by_header(Header *pHeader)
{
    if(NULL == pHeader)
    {
        return NULL;
    }
    void *res = (void *)(pHeader + 1);
    return res;
}

static void dmemory_move_to_alloc(Header *pHeader)
{
    if(NULL == pHeader)
    {
        return ;
    }

    Header *srcFront = pHeader->pre;
    Header *srcBehind = pHeader->next;

    // Header *dstFront = pBlockList->allocList->first;
    // Header *dstBehind = dstFront->next;

    srcFront->next = srcBehind;
    srcBehind->pre = srcFront;

    // dstFront->next = pHeader;
    // pHeader->pre = dstFront;

    // dstBehind->pre = pHeader;
    // pHeader->next = dstBehind;

    // pHeader->available = false;
    /////////////////////////////
    pHeader->pre = NULL;
    pHeader->next = NULL;
    dmemory_insert_to_alloc(pHeader);
}

static void dmemory_move_to_spare(Header *pDataHeader)
{
    Header *srcFront = pDataHeader->pre;
    Header *srcBehind = pDataHeader->next;

    pDataHeader->pre = NULL;
    pDataHeader->next = NULL;

    srcFront->next = srcBehind;
    srcBehind->pre = srcFront;

    dmemory_insert_to_spare(pDataHeader);
}

static void dmemory_insert_to_alloc(Header *pHeader)
{
    Header *pFrontHeader = pBlockList->allocList->first;
    Header *pBehindHeader = pFrontHeader->next;

    pFrontHeader->next = pHeader;

    pHeader->pre = pFrontHeader;
    pHeader->next = pBehindHeader;

    pBehindHeader->pre = pHeader;

    pHeader->available = 0x00;
}

static void *dmemory_BestFit_search_spare(unsigned int dataSize)
{
    if(dataSize > BLOCK_SIZE - 2*sizeof(Header) - 4)
    {
        return NULL;
    }

    HList *pSpareList = pBlockList->spareList;
    Header *pTravelHeader = pSpareList->first->next;

    void *res = NULL;
    while (pTravelHeader != pSpareList->last)
    {
        if(pTravelHeader->size >= dataSize && pTravelHeader->size < (dataSize + sizeof(Header) + 4))
        {
            res = dmemory_get_memAddr_by_header(pTravelHeader);
            dmemory_move_to_alloc(pTravelHeader);
            return res;
        }
        else if(pTravelHeader->size >= (dataSize + sizeof(Header) + 4))
        {
            res = dmemory_get_memAddr_by_header(pTravelHeader);
            Header *newHeader = (Header *)(res + dataSize);
            newHeader->size = pTravelHeader->size - dataSize - sizeof(Header);
            pTravelHeader->size = dataSize;

            dmemory_move_to_alloc(pTravelHeader);
            dmemory_insert_to_spare(newHeader);
            return res;
        }

        pTravelHeader = pTravelHeader->next;
    }
    // if(NULL != res)
    // {
    //     return res;
    // }

    Block *newBlock = dmemory_Block_init();
    if(NULL == newBlock)
    {
        return NULL;
    }

    Block *pFrontBlock = pBlockList->last;
    pBlockList->last = newBlock;

    pFrontBlock->next = newBlock;
    newBlock->pre = pFrontBlock;

    Header *dataHeader = (Header *)(newBlock->block);
    res = dmemory_get_memAddr_by_header(dataHeader);

    Header *remainHeader = (Header *)(res + dataSize);
    remainHeader->size = BLOCK_SIZE - dataSize - 2*sizeof(Header);
    dataHeader->size = dataSize;

    dmemory_insert_to_alloc(dataHeader);
    dmemory_insert_to_spare(remainHeader);

    return res;
}

static Header *dmemory_get_next_header(Header *pHeader)
{
    Header *res = (Header *)((void *)(pHeader + 1) + pHeader->size);
    if(0 == res->size)
    {
        return NULL;
    }
    return res;
}

static void dmemory_remove_from_spare(Header *pHeader)
{
    Header *front = pHeader->pre;
    Header *behind = pHeader->next;

    front->next = behind;
    behind->pre = front;

    pHeader->next = NULL;
    pHeader->pre = NULL;
}


void dmemory_merge_spare_space()
{
    Block *ptrBlock = pBlockList->first;
    while (NULL != ptrBlock)
    {
        Header *pTravelHeader = (Header *)(ptrBlock->block);
        while (NULL != pTravelHeader && 0 != pTravelHeader->size)
        {
            if(NULL != pTravelHeader && pTravelHeader->available == 0xff)
            {
                Header *nextHeader = dmemory_get_next_header(pTravelHeader);
                while (NULL != nextHeader && nextHeader->available == 0xff)
                {
                    Header *frontHeader = nextHeader;
                    nextHeader = dmemory_get_next_header(nextHeader);
                    pTravelHeader->size = pTravelHeader->size + frontHeader->size +sizeof(Header);
                    dmemory_remove_from_spare(frontHeader);
                }
                dmemory_remove_from_spare(pTravelHeader);
                dmemory_insert_to_spare(pTravelHeader);

                if(NULL == nextHeader)
                {
                    break;
                }
                else
                {
                    pTravelHeader = dmemory_get_next_header(nextHeader);
                }
                
            }
            else
            {
                pTravelHeader = dmemory_get_next_header(pTravelHeader);
            }
        }
        
        ptrBlock = ptrBlock->next;
    }
}

void dmemory_printAllocInfo()
{
    Header *pHeader = pBlockList->allocList->first->next;
    printf("allocList:\n");
    while (pHeader != pBlockList->allocList->last)
    {
        printf("==>%d", pHeader->size);
        pHeader = pHeader->next;
    }
    printf("\n");
}

void dmemory_printSpareInfo()
{
    Header *pHeader = pBlockList->spareList->first->next;
    printf("spareList:\n");
    while (pHeader != pBlockList->spareList->last)
    {
        printf("==>%d", pHeader->size);
        pHeader = pHeader->next;
    }
    printf("\n");
}

void dmemory_delete()
{
    if(NULL == pBlockList)
    {
        return;
    }
    dmemory_HList_delete(pBlockList->allocList);
    dmemory_HList_delete(pBlockList->spareList);

    Block *pBlock = pBlockList->first->next;

    while (NULL != pBlockList->first)
    {
        pBlock = pBlockList->first;
        pBlockList->first = pBlock->next;
        dmemory_Block_delete(pBlock);
    }

    free(pBlockList);
    pBlockList = NULL;
}

static int dmemory_is_header_in_alloc(Header *pHeader)
{
    if(NULL == pHeader || NULL == pBlockList)
    {
        return NULL_POINTER;
    }
    HList *allocList = pBlockList->allocList;
    Header *pTravelHeader = allocList->first->next;

    while (pTravelHeader != allocList->last)
    {
        if(pTravelHeader == pHeader)
        {
            return EXIST;
        }
        pTravelHeader = pTravelHeader->next;
    }

    return NOT_EXIST;
}

void *dmemory_malloc(unsigned int dataSize)
{
    if(NULL == pBlockList || 0 == dataSize)
    {
        return NULL;
    }
    void *res = dmemory_BestFit_search_spare(dataSize);
    return res;
}

void *dmemory_calloc(unsigned int dataSize)
{
    if(NULL == pBlockList || 0 == dataSize)
    {
        return NULL;
    }
    void *res = dmemory_BestFit_search_spare(dataSize);
    if(NULL == res)
    {
        return NULL;
    }

    memset(res, 0, dataSize);
    return res;
}

void *dmemory_realloc(void *srcPtr, unsigned int dataSize)
{
    if(NULL == srcPtr || 0 == dataSize)
    {
        return NULL;
    }

    Header *srcHeader = (Header *)(srcPtr - sizeof(Header));
    if(dmemory_is_header_in_alloc(srcHeader) < 0)
    {
        return NULL;
    }
    
    void *res = dmemory_BestFit_search_spare(dataSize);
    if(NULL == res)
    {
        return NULL;
    }

    unsigned int cpSize = dataSize > (srcHeader->size)? (srcHeader->size) : dataSize;
    memcpy(res, srcPtr, cpSize);

    dmemory_move_to_spare(srcHeader);
    return res;
}

void dmemory_free(void *pData)
{
    if(NULL == pData)
    {
        return;
    }
    Header *pDataHeader = (Header *)(pData - sizeof(Header));
    HList *allocList = pBlockList->allocList;
    Header *pTravelHeader = allocList->first->next;

    int flag = 0;
    while (pTravelHeader != allocList->last)
    {
        if(pTravelHeader == pDataHeader)
        {
            flag = 1;
            break;
        }
        pTravelHeader = pTravelHeader->next;
    }
    if(0 == flag)
    {
        return;
    }

    dmemory_move_to_spare(pDataHeader);
}