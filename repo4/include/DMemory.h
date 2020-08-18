#ifndef _DMEMORY_H_
#define _DMEMORY_H_

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>

#define BLOCK_SIZE 128*1024
#define MALLOC_FAILED -1

typedef struct _Header{
    struct _Header *pre;
    struct _Header *next;
    // bool available;
    unsigned int size;
} Header;

typedef struct _HList{
    Header *first;
    Header *last;
} HList;

typedef struct _DMemory{
    HList *allocList;
    HList *spareList;

    void *block;
    void *currentEdge;

    struct _DMemory *pre;
    struct _DMemory *next;
} DMemory;

DMemory *firstBlock = NULL;

int dmemory_init()
{
    firstBlock = (DMemory *)malloc(sizeof(DMemory));
    if(NULL == firstBlock)
    {
        return MALLOC_FAILED;
    }

    firstBlock->block = malloc(BLOCK_SIZE);
    if(NULL == firstBlock->block)
    {
        return MALLOC_FAILED;
    }

    firstBlock->currentEdge = firstBlock->block;
    firstBlock->allocList->first = (Header *)(firstBlock->currentEdge);
    firstBlock->allocList->last = (Header *)(firstBlock->currentEdge + sizeof(Header));

    firstBlock->spareList->first = (Header *)(firstBlock->currentEdge + 2 * sizeof(Header));
    firstBlock->spareList->last = (Header *)(firstBlock->currentEdge + 3 * sizeof(Header));


    firstBlock->allocList->first->next = firstBlock->allocList->last;
    firstBlock->allocList->last->pre = firstBlock->allocList->first;

    firstBlock->spareList->first->next = firstBlock->spareList->last;
    firstBlock->spareList->last->pre = firstBlock->spareList->first;

    firstBlock->currentEdge = (firstBlock->currentEdge) + (4 * sizeof(Header));

    firstBlock->pre = NULL;
    firstBlock->next = NULL;
}

static void *dmemory_get_mem_by_header(Header *pHeader)
{
    void *res = (void *)(pHeader + 1);
    return res;
}

static void dmemory_spare_to_alloc(Header *pHeader)
{
    Header *srcFront = pHeader->pre;
    Header *srcBehind = pHeader->next;

    Header *dstFront = firstBlock->allocList->first;
    Header *dstBehind = dstFront->next;

    srcFront->next = srcBehind;
    srcBehind->pre = srcFront;

    dstFront->next = pHeader;
    pHeader->pre = dstFront;

    dstBehind->pre = pHeader;
    pHeader->next = dstBehind;
}

static void dmemory_insert_to_spare(Header *pHeader)
{
    HList *pSpareList = firstBlock->spareList;
    Header *pTravelHeader = pSpareList->first->next;

    while (pTravelHeader != pSpareList->last && pTravelHeader->size < pHeader->size)
    {
        pTravelHeader = pTravelHeader->next;
    }
    
    Header *pFrontHeader = pTravelHeader->pre;
    pFrontHeader->next = pHeader;

    pHeader->pre = pFrontHeader;
    pHeader->next = pTravelHeader;

    pTravelHeader->pre = pHeader;
}

static void dmemory_insert_to_alloc(Header *pHeader)
{
    Header *pFrontHeader = firstBlock->allocList->first;
    Header *pBehindHeader = pFrontHeader->next;

    pFrontHeader->next = pHeader;

    pHeader->pre = pFrontHeader;
    pHeader->next = pBehindHeader;

    pBehindHeader->pre = pHeader;
}

static void *dmemory_search_spare(size_t dataSize)
{
    HList *pSpareList = firstBlock->spareList;
    Header *pTravelHeader = pSpareList->first->next;

    void *res = NULL;
    while (pTravelHeader != pSpareList->last)
    {
        if(pTravelHeader->size >= dataSize && pTravelHeader->size < (dataSize + sizeof(Header) + 4))
        {
            res = dmemory_get_mem_by_header(pTravelHeader);
            dmemory_spare_to_alloc(pTravelHeader);
        }
        else if(pTravelHeader->size >= (dataSize + sizeof(Header) + 4))
        {
            res = dmemory_get_mem_by_header(pTravelHeader);
            Header *newHeader = (Header *)(res + dataSize);
            newHeader->size = pTravelHeader->size - dataSize - sizeof(Header);
            pTravelHeader->size = dataSize;

            dmemory_spare_to_alloc(pTravelHeader);
            dmemory_insert_to_spare(newHeader);
        }

        pTravelHeader = pTravelHeader->next;
    }

    return res;
}
//未判断剩余内存不够的情况
static void *dmemory_get_demain(size_t dataSize)
{
    Header *newHeader = (Header *)(firstBlock->currentEdge);
    newHeader->size = dataSize;
    dmemory_insert_to_alloc(newHeader);

    firstBlock->currentEdge = (firstBlock->currentEdge) + sizeof(Header) + dataSize;

    void *res = (void *)(newHeader + 1);
    return res;
}

void *dmemory_malloc(size_t dataSize)
{
    void *res = dmemory_search_spare(dataSize);
    if(NULL != res)
    {
        return res;
    }

    res = dmemory_get_demain(dataSize);
    return res;
}


#endif