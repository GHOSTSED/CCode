/*************************************************************************************************************************************/
/*												  INCLUDE_FILES																		 */
/*************************************************************************************************************************************/
#include "DMemory.h"

/*************************************************************************************************************************************/
/*												   TYPES																		 */
/*************************************************************************************************************************************/
// typedef struct _Header Header;
// typedef struct _HList HList;
// typedef struct _Block Block;
// typedef struct _BlockList BlockList;

/* 用于存储小内存块信息的头部 */
typedef struct _Header
{
    struct _Header *pre;     /* 指向链表中的前驱节点 */
    struct _Header *next;    /* 指向链表中的后继节点 */
    unsigned char available; /* 用来标志该块是空闲块还是占用块的标志位，0x00表示被占用；0xff表示空闲状态 */
    unsigned int size;       /* 用于记录该内存块大小 */
} Header;

/* 用于在头部之间建立联系的链表 */
typedef struct _HList
{
    Header *first; /* 附加头节点，指向链表的头部 */
    Header *last;  /* 附加尾节点，指向链表的尾部 */
} HList;

/* 进行数据分配使用的大内存块（64KB） */
typedef struct _Block
{
    void *block; /* 指向一个大的内存块，大小为64KB */
    struct _Block *pre;  /* 大内存块的前向指针 */
    struct _Block *next; /* 大内存块的后向指针 */
} Block;

/* 用于管理大内存块的链表 */
typedef struct _BlockList
{
    HList *allocList; /* 已分配的所有小内存块组成的链表 */
    HList *spareList; /* 当前空闲的所有小内存块组成的链表 */

    Block *first; /* 指向首个大内存块 */
    Block *last;  /* 指向最后一个大内存块 */
} BlockList;

/*************************************************************************************************************************************/
/*												   VARIABLES																		 */
/*************************************************************************************************************************************/
static BlockList *pBlockList = NULL; /* 静态全局变量，大内存块链表，内存获取来源 */

/*************************************************************************************************************************************/
/*												   FUNCTIONS																		 */
/*************************************************************************************************************************************/
/*
*@fn							static HList *dmemory_HList_init();
*@brief   						用于初始化Header链表，为dmemory_init的组成部分
*@details	
*
*@param[in]                     N/A
*
*@return						若创建链表成功，返回链表指针，否则返回NULL；
*@retval
*/
static HList *dmemory_HList_init();

/*
*@fn							static void dmemory_HList_delete(HList *pHList);
*@brief   						用于销毁Header链表，为dmemory_delete的组成部分
*@details	
*
*@param[in] HList* pHList       指向要销毁的链表的指针
*
*@return						N/A
*@retval
*/
static void dmemory_HList_delete(HList *pHList);

/*
*@fn							static Block *dmemory_Block_init();
*@brief   						用于初始化一个大的内存块用于分配
*@details	
*
*@param[in]                     N/A
*
*@return						若内存块申请成功，返回指向内存块的指针，否则返回NULL
*@retval
*/
static Block *dmemory_Block_init();

/*
*@fn							static void dmemory_Block_delete(Block *ptrBlock);
*@brief   						用于销毁大内存块
*@details	
*
*@param[in] Block* ptrBlock     要销毁的大内存块
*
*@return						N/A
*@retval
*/
static void dmemory_Block_delete(Block *ptrBlock);

/*
*@fn							static void *dmemory_get_memAddr_by_header(Header *pHeader);
*@brief   						用于根据Header获取其对应的内存块的首地址
*@details	
*
*@param[in] Header* pHeader     要获取的内存块对应的Header指针
*
*@return						N/A
*@retval
*/
static void *dmemory_get_memAddr_by_header(Header *pHeader);

/*
*@fn							static void dmemory_move_to_alloc(Header *pHeader);
*@brief   						用于将Header由空闲链表移动到已分配链表中
*@details	
*
*@param[in] Header* pHeader     要进行移动的头部
*
*@return						N/A
*@retval
*/
static void dmemory_move_to_alloc(Header *pHeader);

/*
*@fn							static void dmemory_move_to_spare(Header *pDataHeader);
*@brief   						用于将Header由已分配链表转移到空闲链表中
*@details	
*
*@param[in] Header* pHeader     要进行移动的头部
*
*@return						N/A
*@retval
*/
static void dmemory_move_to_spare(Header *pDataHeader);

/*
*@fn							static void dmemory_insert_to_alloc(Header *pHeader);
*@brief   						用于将Header插入到已分配链表中
*@details	
*
*@param[in] Header* pHeader     要进行插入的头部
*
*@return						N/A
*@retval
*/
static void dmemory_insert_to_alloc(Header *pHeader);

/*
*@fn							static void dmemory_insert_to_spare(Header *pInsertHeader);
*@brief   						用于将Header插入空闲内存块（spareList）中
*@details	
*
*@param[in] Header* pInsertHeader     要进行插入的Header
*
*@return						N/A
*@retval
*/
static void dmemory_insert_to_spare(Header *pInsertHeader);

/*
*@fn							        static void *dmemory_BestFit_search_spare(unsigned int dataSize);
*@brief   						        用于在空闲内存块链表依照Best-fit算法寻找最合适的内存块用于分配
*@details	
*
*@param[in] unsigned int dataSize       要申请的内存大小（单位：字节）
*
*@return						        申请到的内存的首地址
*@retval
*/
static void *dmemory_BestFit_search_spare(unsigned int dataSize);

/*
*@fn							        static Header *dmemory_get_next_header(Header *pHeader);
*@brief   						        获取地址上相邻的下一个头部
*@details	
*
*@param[in] Header* pHeader             要获取下一个头部的头部
*
*@return						        地址上与该头部相邻的下一个头部
*@retval
*/
static Header *dmemory_get_next_header(Header *pHeader);

/*
*@fn							        static void dmemory_remove_from_spare(Header *pHeader);
*@brief   						        将头部从空闲链表中移除
*@details	
*
*@param[in] Header* pHeader             要移除的头部指针
*
*@return						        N/A
*@retval
*/
static void dmemory_remove_from_spare(Header *pHeader);

/*
*@fn							        static int dmemory_is_header_in_alloc(Header *pHeader)；
*@brief   						        判断头部是否存在于已分配链表中
*@details	
*
*@param[in] Header* pHeader             要进行查询的头部指针
*
*@return						        若该头部不存在于已分配链表中，返回NOT_EXIST(-3)；否则，返回EXIST(2)
*@retval
*/
static int dmemory_is_header_in_alloc(Header *pHeader);

/*
*@fn							int dmemory_init();
*@brief   						用于初始化内存块链表
*@details	
*
*@param[in]                     N/A
*
*@return						初始化情况，共分2种：MALLOC_FAILED(-1)：内存分配过程中发生错误；INIT_SUCCESS(1)：初始化成功
*@retval
*/
int dmemory_init();

/*
*@fn							void dmemory_merge_spare_space();
*@brief   						用于对内存块中的空闲区域进行合并，可以由用户显式调用，内部每隔一段时间也会调用此函数
*@details	
*
*@param[in]                     N/A
*
*@return						N/A
*@retval
*/
void dmemory_merge_spare_space();

/*
*@fn							void dmemory_printAllocInfo();
*@brief   						用于打印已分配块的情况，打印出的数字代表空闲的内存块大小
*@details	
*
*@param[in]                     N/A
*
*@return						N/A
*@retval
*/
void dmemory_printAllocInfo();

/*
*@fn							void dmemory_printSpareInfo();
*@brief   						用于打印空闲内存块的情况，打印出的数字代表空闲的内存块大小
*@details	
*
*@param[in]                     N/A
*
*@return						N/A
*@retval
*/
void dmemory_printSpareInfo();

/*
*@fn							void dmemory_delete();
*@brief   						用于销毁内存池
*@details	
*
*@param[in]                     N/A
*
*@return						N/A
*@retval
*/
void dmemory_delete();

/*
*@fn							    void *dmemory_malloc(unsigned int dataSize);
*@brief   						    用于进行内存申请的接口
*@details	
*
*@param[in] unsigned int dataSize   申请的字节数                     
*
*@return						    若成功申请，返回指向申请到的内存块的指针；否则返回NULL
*@retval
*/
void *dmemory_malloc(unsigned int dataSize);

/*
*@fn							    void *dmemory_calloc(unsigned int dataSize);
*@brief   						    用于进行内存申请的接口，会将申请到的内存空间用0填充
*@details	
*
*@param[in] unsigned int dataSize   申请的字节数                     
*
*@return						    若成功申请，返回指向申请到的内存块的指针；否则返回NULL
*@retval
*/
void dmemory_free(void *pData);

/*
*@fn							    void *dmemory_realloc(void *srcPtr, unsigned int dataSize);
*@brief   						    用于进行内存重新申请大小的接口
*@details	
*
*@param[in] void *srcPtr            指向原内存空间的指针
*@param[in] unsigned int dataSize   重新申请的字节数                     
*
*@return						    若成功申请，返回指向申请到的内存块的指针；否则返回NULL
*@retval
*/
void *dmemory_calloc(unsigned int dataSize);

/*
*@fn							    void dmemory_free(void *pData);
*@brief   						    用于进行内存空间释放的接口
*@details	
*
*@param[in] void* pData             指向要释放的空间的指针                  
*
*@return						    N/A
*@retval
*/
void *dmemory_realloc(void *srcPtr, unsigned int dataSize);

static HList *dmemory_HList_init()
{
    HList *pHList = (HList *)malloc(sizeof(HList));
    if (NULL == pHList)
    {
        return NULL;
    }

    Header *first = (Header *)malloc(sizeof(Header));
    if (NULL == first)
    {
        free(pHList);
        pHList = NULL;
        return NULL;
    }

    Header *last = (Header *)malloc(sizeof(Header));
    if (NULL == last)
    {
        free(pHList);
        free(first);
        pHList = NULL;
        first = NULL;
        return NULL;
    }

    /* 初始化一个只有附加头节点和附件尾节点的空链表 */
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
    if (NULL == pHList)
    {
        return;
    }
    /* 只用释放头尾节点，中间的头部是Block的一部分，由Block进行free */
    free(pHList->first);
    free(pHList->last);
    free(pHList);
}

static Block *dmemory_Block_init()
{
    Block *pBlock = (Block *)malloc(sizeof(Block));
    if (NULL == pBlock)
    {
        return NULL;
    }

    /* 申请的内存比64KB大一个Header的的大小，用来标识块的空间到此为止 */
    void *block = malloc(BLOCK_SIZE + sizeof(Header));
    if (NULL == block)
    {
        free(pBlock);
        pBlock = NULL;
        return NULL;
    }

    pBlock->block = block;
    pBlock->pre = NULL;
    pBlock->next = NULL;

    /* endHeader的特点是size属性为0，到此表示该内存块遍历完成 */
    Header *endHeader = (Header *)(block + BLOCK_SIZE);
    endHeader->size = 0;
    return pBlock;
}

static void dmemory_Block_delete(Block *ptrBlock)
{
    if (NULL == ptrBlock)
    {
        return;
    }

    /* Header和内存块均是在此时真正释放 */
    free(ptrBlock->block);
    free(ptrBlock);
    ptrBlock = NULL;
}

static void dmemory_insert_to_spare(Header *pInsertHeader)
{
    /* 从附加头节点之后的节点开始遍历 */
    Header *pTravelHeader = pBlockList->spareList->first->next;

    /* 寻找第一个size大于pInsertHeader的size的Header，并将pInsertHeader插入到其之前，以此来保证spareList按照size升序排列 */
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
    if (NULL == pBlockList)
    {
        return MALLOC_FAILED;
    }

    Block *firstBlock = dmemory_Block_init();
    if (NULL == firstBlock)
    {
        free(pBlockList);
        pBlockList = NULL;
        return MALLOC_FAILED;
    }

    HList *allocList = dmemory_HList_init();
    if (NULL == allocList)
    {
        free(pBlockList);
        dmemory_Block_delete(firstBlock);
        pBlockList = NULL;
        firstBlock = NULL;
        return MALLOC_FAILED;
    }

    HList *spareList = dmemory_HList_init();
    if (NULL == spareList)
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

    /* 初始化后第一个空闲内存块大小为64KB-sizeof(Header)，将该内存块插入到spareList中 */
    firstSpare->size = BLOCK_SIZE - sizeof(Header);
    dmemory_insert_to_spare(firstSpare);
    return INIT_SUCCESS;
}

static void *dmemory_get_memAddr_by_header(Header *pHeader)
{
    if (NULL == pHeader)
    {
        return NULL;
    }
    /* Header后紧跟的就是对应的内存块首地址 */
    void *res = (void *)(pHeader + 1);
    return res;
}

static void dmemory_move_to_alloc(Header *pHeader)
{
    if (NULL == pHeader)
    {
        return;
    }

    Header *srcFront = pHeader->pre;
    Header *srcBehind = pHeader->next;

    /* 将Header从spareList中摘下 */
    srcFront->next = srcBehind;
    srcBehind->pre = srcFront;
    pHeader->pre = NULL;
    pHeader->next = NULL;

    /* 将Header插入到allocList中 */
    dmemory_insert_to_alloc(pHeader);
}

static void dmemory_move_to_spare(Header *pDataHeader)
{
    /* 将Header从allocList中摘下 */
    Header *srcFront = pDataHeader->pre;
    Header *srcBehind = pDataHeader->next;

    pDataHeader->pre = NULL;
    pDataHeader->next = NULL;

    srcFront->next = srcBehind;
    srcBehind->pre = srcFront;

    /* 将Header插入到spareList中 */
    dmemory_insert_to_spare(pDataHeader);
}

static void dmemory_insert_to_alloc(Header *pHeader)
{
    /* 将节点插入到allocList的附加头节点之后 */
    Header *pFrontHeader = pBlockList->allocList->first;
    Header *pBehindHeader = pFrontHeader->next;

    pFrontHeader->next = pHeader;

    pHeader->pre = pFrontHeader;
    pHeader->next = pBehindHeader;

    pBehindHeader->pre = pHeader;

    pHeader->available = 0x00;
}

static void *dmemory_WorstFit_search_spare(unsigned int dataSize)
{
    /* 若申请内存过大，则不予分配 */
    if (dataSize > BLOCK_SIZE - 2 * sizeof(Header) - 4)
    {
        return NULL;
    }

    HList *pSpareList = pBlockList->spareList;
    Header *pTravelHeader = pSpareList->last->pre;
    void *res = NULL;
    if (pTravelHeader != pSpareList->first)
    {
        /* 如果空闲空间够用，但分出dataSize的空间后，剩下的不足一个Header大小+4个字节，便将整个内存块都交给用户，也即是说，用户实际上获得了更大的内存空间 */
        if (pTravelHeader->size >= dataSize && pTravelHeader->size < (dataSize + sizeof(Header) + 4))
        {
            res = dmemory_get_memAddr_by_header(pTravelHeader);
            dmemory_move_to_alloc(pTravelHeader);
            return res;
        }
        /* 如果空闲空间够用，且分出dataSize的空间后，剩下空间的足够一个Header大小+4个字节，便给用户划出dataSize的内存空间，剩下的内存另设新的头部进行管理 */
        else if (pTravelHeader->size >= (dataSize + sizeof(Header) + 4))
        {
            res = dmemory_get_memAddr_by_header(pTravelHeader);
            Header *newHeader = (Header *)(res + dataSize);
            newHeader->size = pTravelHeader->size - dataSize - sizeof(Header);
            pTravelHeader->size = dataSize;

            dmemory_move_to_alloc(pTravelHeader);
            dmemory_insert_to_spare(newHeader);
            return res;
        }
    }

    /* 程序运行到这里，说明spareList中找不到合适的内存块，需要创建新的Block作为内存分配来源 */
    Block *newBlock = dmemory_Block_init();
    if (NULL == newBlock)
    {
        return NULL;
    }

    Block *pFrontBlock = pBlockList->last;
    pBlockList->last = newBlock;

    pFrontBlock->next = newBlock;
    newBlock->pre = pFrontBlock;

    /* 从新创立的Block中划分出用户想要的内存大小交予用户，并为剩余内存设立Header，插入到空闲内存块中 */
    Header *dataHeader = (Header *)(newBlock->block);
    res = dmemory_get_memAddr_by_header(dataHeader);

    Header *remainHeader = (Header *)(res + dataSize);
    remainHeader->size = BLOCK_SIZE - dataSize - 2 * sizeof(Header);
    dataHeader->size = dataSize;

    dmemory_insert_to_alloc(dataHeader);
    dmemory_insert_to_spare(remainHeader);

    return res;
}

static void *dmemory_BestFit_search_spare(unsigned int dataSize)
{
    /* 若申请内存过大，则不予分配 */
    if (dataSize > BLOCK_SIZE - 2 * sizeof(Header) - 4)
    {
        return NULL;
    }

    HList *pSpareList = pBlockList->spareList;
    Header *pTravelHeader = pSpareList->first->next;

    void *res = NULL;
    /* 遍历spareList，找到第一个能够满足要求的内存块 */
    while (pTravelHeader != pSpareList->last)
    {
        /* 如果空闲空间够用，但分出dataSize的空间后，剩下的不足一个Header大小+4个字节，便将整个内存块都交给用户，也即是说，用户实际上获得了更大的内存空间 */
        if (pTravelHeader->size >= dataSize && pTravelHeader->size < (dataSize + sizeof(Header) + 4))
        {
            res = dmemory_get_memAddr_by_header(pTravelHeader);
            dmemory_move_to_alloc(pTravelHeader);
            return res;
        }
        /* 如果空闲空间够用，且分出dataSize的空间后，剩下空间的足够一个Header大小+4个字节，便给用户划出dataSize的内存空间，剩下的内存另设新的头部进行管理 */
        else if (pTravelHeader->size >= (dataSize + sizeof(Header) + 4))
        {
            res = dmemory_get_memAddr_by_header(pTravelHeader);
            Header *newHeader = (Header *)(res + dataSize);
            newHeader->size = pTravelHeader->size - dataSize - sizeof(Header);
            pTravelHeader->size = dataSize;

            dmemory_move_to_alloc(pTravelHeader);
            dmemory_insert_to_spare(newHeader);
            return res;
        }
        /* 如果空间不够，继续往后寻找 */
        pTravelHeader = pTravelHeader->next;
    }

    /* 程序运行到这里，说明spareList中找不到合适的内存块，需要创建新的Block作为内存分配来源 */
    Block *newBlock = dmemory_Block_init();
    if (NULL == newBlock)
    {
        return NULL;
    }

    Block *pFrontBlock = pBlockList->last;
    pBlockList->last = newBlock;

    pFrontBlock->next = newBlock;
    newBlock->pre = pFrontBlock;

    /* 从新创立的Block中划分出用户想要的内存大小交予用户，并为剩余内存设立Header，插入到空闲内存块中 */
    Header *dataHeader = (Header *)(newBlock->block);
    res = dmemory_get_memAddr_by_header(dataHeader);

    Header *remainHeader = (Header *)(res + dataSize);
    remainHeader->size = BLOCK_SIZE - dataSize - 2 * sizeof(Header);
    dataHeader->size = dataSize;

    dmemory_insert_to_alloc(dataHeader);
    dmemory_insert_to_spare(remainHeader);

    return res;
}

static Header *dmemory_get_next_header(Header *pHeader)
{
    /* 获取地址上连续的下一个Header。譬如：已知一个Header地址为x，其管理的内存块大小为n，则下一个Header地址为x+sizeof(Header)+n */
    Header *res = (Header *)((void *)(pHeader + 1) + pHeader->size);
    if (0 == res->size)
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
    /* 逐个Block进行空闲内存块的合并 */
    Block *ptrBlock = pBlockList->first;
    while (NULL != ptrBlock)
    {
        /* 从首个头部开始遍历 */
        Header *pTravelHeader = (Header *)(ptrBlock->block);
        while (NULL != pTravelHeader && 0 != pTravelHeader->size)
        {
            /* 如果头部代表的内存块是空闲的 */
            if (NULL != pTravelHeader && pTravelHeader->available == 0xff)
            {
                /* 尝试与下一个内存块进行合并，直到nextHeader不是空闲内存块为止 */
                Header *nextHeader = dmemory_get_next_header(pTravelHeader);
                while (NULL != nextHeader && nextHeader->available == 0xff)
                {
                    Header *frontHeader = nextHeader;
                    nextHeader = dmemory_get_next_header(nextHeader);
                    pTravelHeader->size = pTravelHeader->size + frontHeader->size + sizeof(Header);
                    dmemory_remove_from_spare(frontHeader);
                }
                dmemory_remove_from_spare(pTravelHeader);
                dmemory_insert_to_spare(pTravelHeader);

                if (NULL == nextHeader)
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
    if (NULL == pBlockList)
    {
        return;
    }
    /* 释放已分配链表和空闲链表 */
    dmemory_HList_delete(pBlockList->allocList);
    dmemory_HList_delete(pBlockList->spareList);

    Block *pBlock = pBlockList->first->next;

    /* 释放Block */
    while (NULL != pBlockList->first)
    {
        pBlock = pBlockList->first;
        pBlockList->first = pBlock->next;
        dmemory_Block_delete(pBlock);
    }

    /* 释放BlockList */
    free(pBlockList);
    pBlockList = NULL;
}

static int dmemory_is_header_in_alloc(Header *pHeader)
{
    if (NULL == pHeader || NULL == pBlockList)
    {
        return NULL_POINTER;
    }
    HList *allocList = pBlockList->allocList;
    Header *pTravelHeader = allocList->first->next;

    while (pTravelHeader != allocList->last)
    {
        if (pTravelHeader == pHeader)
        {
            return EXIST;
        }
        pTravelHeader = pTravelHeader->next;
    }

    return NOT_EXIST;
}

void *dmemory_malloc(unsigned int dataSize)
{
    if (NULL == pBlockList || 0 == dataSize)
    {
        return NULL;
    }
    void *res = dmemory_BestFit_search_spare(dataSize);
    return res;
}

void *dmemory_calloc(unsigned int dataSize)
{
    if (NULL == pBlockList || 0 == dataSize)
    {
        return NULL;
    }
    void *res = dmemory_BestFit_search_spare(dataSize);
    if (NULL == res)
    {
        return NULL;
    }

    memset(res, 0, dataSize);
    return res;
}

void *dmemory_realloc(void *srcPtr, unsigned int dataSize)
{
    if (NULL == srcPtr || 0 == dataSize)
    {
        return NULL;
    }

    Header *srcHeader = (Header *)(srcPtr - sizeof(Header));
    if (dmemory_is_header_in_alloc(srcHeader) < 0)
    {
        return NULL;
    }

    void *res = dmemory_BestFit_search_spare(dataSize);
    if (NULL == res)
    {
        return NULL;
    }

    unsigned int cpSize = dataSize > (srcHeader->size) ? (srcHeader->size) : dataSize;
    memcpy(res, srcPtr, cpSize);

    dmemory_move_to_spare(srcHeader);
    return res;
}

void dmemory_free(void *pData)
{
    static unsigned int free_count = 0;
    if (NULL == pData)
    {
        return;
    }
    Header *pDataHeader = (Header *)(pData - sizeof(Header));
    HList *allocList = pBlockList->allocList;
    Header *pTravelHeader = allocList->first->next;

    int flag = 0;
    while (pTravelHeader != allocList->last)
    {
        if (pTravelHeader == pDataHeader)
        {
            flag = 1;
            break;
        }
        pTravelHeader = pTravelHeader->next;
    }
    if (0 == flag)
    {
        return;
    }
    dmemory_move_to_spare(pDataHeader);

    free_count++;
    if (free_count >= BLOCK_MERGE_INDEX)
    {
        dmemory_merge_spare_space();
        free_count = 0;
    }
}