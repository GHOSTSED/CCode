/*************************************************************************************************************************************/
/*												  INCLUDE_FILES																		 */
/*************************************************************************************************************************************/
#include "../include/DMemory.h"

/*************************************************************************************************************************************/
/*												   TYPES																		 */
/*************************************************************************************************************************************/


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

    int strategy;
} BlockList;

/*************************************************************************************************************************************/
/*												   VARIABLES																		 */
/*************************************************************************************************************************************/
static BlockList *pBlockList = NULL; /* 静态全局变量，Block链表，内存获取来源 */

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
*@return						执行情况，分为两种：NULL_POINTER：传入了空指针；OP_SUCCESS：操作成功
*@retval
*/
static int dmemory_HList_delete(HList *pHList);

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
*@return						执行情况，分为两种：NULL_POINTER：传入了空指针；OP_SUCCESS：操作成功
*@retval
*/
static int dmemory_Block_delete(Block *ptrBlock);

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
*@return						执行情况，分为两种：NULL_POINTER：传入了空指针；OP_SUCCESS：操作成功
*@retval
*/
static int dmemory_move_to_alloc(Header *pHeader);

/*
*@fn							static void dmemory_move_to_spare(Header *pDataHeader);
*@brief   						用于将Header由已分配链表转移到空闲链表中
*@details	
*
*@param[in] Header* pHeader     要进行移动的头部
*
*@return						执行情况，分为两种：NULL_POINTER：传入了空指针；OP_SUCCESS：操作成功
*@retval
*/
static int dmemory_move_to_spare(Header *pDataHeader);

/*
*@fn							static void dmemory_insert_to_alloc(Header *pHeader);
*@brief   						用于将Header插入到已分配链表中
*@details	
*
*@param[in] Header* pHeader     要进行插入的头部
*
*@return						执行情况，分为两种：NULL_POINTER：传入了空指针；OP_SUCCESS：操作成功
*@retval
*/
static int dmemory_insert_to_alloc(Header *pHeader);

/*
*@fn							static void dmemory_insert_to_spare(Header *pInsertHeader);
*@brief   						用于将Header插入空闲内存块（spareList）中
*@details	
*
*@param[in] Header* pInsertHeader     要进行插入的Header
*
*@return						执行情况，分为两种：NULL_POINTER：传入了空指针；OP_SUCCESS：操作成功
*@retval
*/
static int dmemory_insert_to_spare(Header *pInsertHeader);

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
*@return						        地址上与该头部相邻的下一个头部, 如果该头部代表的内存已是末尾，则返回NULL
*@retval
*/
static Header *dmemory_get_next_header(Header *pHeader);

/*
*@fn							        static void dmemory_remove_from_HList(Header *pHeader);
*@brief   						        将头部从空闲链表中移除
*@details	
*
*@param[in] Header* pHeader             要移除的头部指针
*
*@return						        执行情况，分为两种：NULL_POINTER：传入了空指针；OP_SUCCESS：操作成功
*@retval
*/
static int dmemory_remove_from_HList(Header *pHeader);

/*
*@fn							        static int dmemory_is_header_in_alloc(Header *pHeader)；
*@brief   						        判断头部是否存在于已分配链表中
*@details	
*
*@param[in] Header* pHeader             要进行查询的头部指针
*
*@return						        若传入空指针或pBlockList未初始化，返回NULL_POINTER；若该头部不存在于已分配链表中，返回NOT_EXIST(-3)；否则，返回EXIST(2)
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
*@fn							    void dmemory_free(void *pData);
*@brief   						    用于进行内存空间释放的接口
*@details	
*
*@param[in] void* pData             指向要释放的空间的指针                  
*
*@return						    执行情况，分为三种：NULL_POINTER：传入空指针或pBlockList未初始化；ILLEGAL_INDEX：传入的地址非法，在allocList中找不到；OP_SUCCESS：释放成功
*@retval
*/
int dmemory_free(void *pData);

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
void *dmemory_calloc(unsigned int dataSize);

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
void *dmemory_realloc(void *srcPtr, unsigned int dataSize);

static int dmemory_insert_to_alloc(Header *pHeader)
{
    Header *pFrontHeader = NULL;
    Header *pBehindHeader = NULL;
    if(NULL == pBlockList || NULL == pHeader)
    {
        return NULL_POINTER;
    }
    /* 将节点插入到allocList的附加头节点之后 */
    pFrontHeader = pBlockList->allocList->first;
    pBehindHeader = pFrontHeader->next;

    pFrontHeader->next = pHeader;

    pHeader->pre = pFrontHeader;
    pHeader->next = pBehindHeader;

    pBehindHeader->pre = pHeader;

    pHeader->available = 0x00;
    return OP_SUCCESS;
}

static HList *dmemory_HList_init()
{
    Header *first = NULL;
    Header *last = NULL;
    HList *pHList = (HList *)malloc(sizeof(HList));
    if (NULL == pHList)
    {
        return NULL;
    }

    first = (Header *)malloc(sizeof(Header));
    if (NULL == first)
    {
        free(pHList);
        pHList = NULL;
        return NULL;
    }

    last = (Header *)malloc(sizeof(Header));
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

static int dmemory_HList_delete(HList *pHList)
{
    if (NULL == pHList)
    {
        return NULL_POINTER;
    }
    /* 只用释放头尾节点，中间的头部是Block的一部分，由Block进行free */
    free(pHList->first);
    free(pHList->last);
    free(pHList);
    return OP_SUCCESS;
}

static Block *dmemory_Block_init()
{
    void *block = NULL;
    Header *endHeader = NULL;
    Block *pBlock = (Block *)malloc(sizeof(Block));
    if (NULL == pBlock)
    {
        return NULL;
    }

    /* 申请的内存比64KB大一个Header的的大小，用来标识块的空间到此为止 */
    block = malloc(BLOCK_SIZE + sizeof(Header));
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
    endHeader = (Header *)(block + BLOCK_SIZE);
    endHeader->size = 0;
    endHeader->available = 0x00;
    return pBlock;
}

static int dmemory_Block_delete(Block *ptrBlock)
{
    if (NULL == ptrBlock)
    {
        return NULL_POINTER;
    }

    /* Header和内存块均是在此时真正释放 */
    free(ptrBlock->block);
    free(ptrBlock);
    ptrBlock = NULL;
    return OP_SUCCESS;
}

static int dmemory_insert_to_spare(Header *pInsertHeader)
{
    Header *pTravelHeader = NULL;
    Header *pFrontHeader = NULL;
    if(NULL == pBlockList || NULL == pInsertHeader)
    {
        return NULL_POINTER;
    }
    /* 从附加头节点之后的节点开始遍历 */
    pTravelHeader = pBlockList->spareList->first->next;

    /* 寻找第一个size大于pInsertHeader的size的Header，并将pInsertHeader插入到其之前，以此来保证spareList按照size升序排列 */
    while (pTravelHeader != pBlockList->spareList->last && pInsertHeader->size > pTravelHeader->size)
    {
        pTravelHeader = pTravelHeader->next;
    }

    pFrontHeader = pTravelHeader->pre;
    pFrontHeader->next = pInsertHeader;

    pInsertHeader->pre = pFrontHeader;
    pInsertHeader->next = pTravelHeader;

    pTravelHeader->pre = pInsertHeader;

    pInsertHeader->available = 0xff;
    return OP_SUCCESS;
}

int dmemory_init()
{
    HList *allocList = NULL;
    Block *firstBlock = NULL;
    HList *spareList = NULL;
    Header *firstSpare = NULL;
    pBlockList = (BlockList *)malloc(sizeof(BlockList));
    if (NULL == pBlockList)
    {
        return MALLOC_FAILED;
    }

    firstBlock = dmemory_Block_init();
    if (NULL == firstBlock)
    {
        free(pBlockList);
        pBlockList = NULL;
        return MALLOC_FAILED;
    }

    allocList = dmemory_HList_init();
    if (NULL == allocList)
    {
        free(pBlockList);
        dmemory_Block_delete(firstBlock);
        pBlockList = NULL;
        firstBlock = NULL;
        return MALLOC_FAILED;
    }

    spareList = dmemory_HList_init();
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
    pBlockList->strategy = STRATEGT_BEST_FIT;

    firstSpare = (Header *)(firstBlock->block);
    firstSpare->available = 0xff;

    /* 初始化后第一个空闲内存块大小为64KB-sizeof(Header)，将该内存块插入到spareList中 */
    firstSpare->size = BLOCK_SIZE - sizeof(Header);
    dmemory_insert_to_spare(firstSpare);
    return INIT_SUCCESS;
}

static void *dmemory_get_memAddr_by_header(Header *pHeader)
{
    void *res = NULL;
    if (NULL == pHeader)
    {
        return NULL;
    }
    /* Header后紧跟的就是对应的内存块首地址 */
    res = (void *)(pHeader + 1);
    return res;
}

static int dmemory_move_to_alloc(Header *pHeader)
{
    Header *srcFront = NULL;
    Header *srcBehind = NULL;
    if (NULL == pHeader)
    {
        return NULL_POINTER;
    }

    srcFront = pHeader->pre;
    srcBehind = pHeader->next;

    /* 将Header从spareList中摘下 */
    srcFront->next = srcBehind;
    srcBehind->pre = srcFront;
    pHeader->pre = NULL;
    pHeader->next = NULL;

    /* 将Header插入到allocList中 */
    dmemory_insert_to_alloc(pHeader);
    return OP_SUCCESS;
}

static int dmemory_move_to_spare(Header *pDataHeader)
{
    Header *srcFront = NULL;
    Header *srcBehind = NULL;
    if(NULL == pDataHeader)
    {
        return NULL_POINTER;
    }
    /* 将Header从allocList中摘下 */
    srcFront = pDataHeader->pre;
    srcBehind = pDataHeader->next;

    pDataHeader->pre = NULL;
    pDataHeader->next = NULL;

    srcFront->next = srcBehind;
    srcBehind->pre = srcFront;

    /* 将Header插入到spareList中 */
    dmemory_insert_to_spare(pDataHeader);

    return OP_SUCCESS;
}

static void *dmemory_WorstFit_search_spare(unsigned int dataSize)
{
    HList *pSpareList = NULL;
    Header *pTravelHeader = NULL;
    Header *newHeader = NULL;
    Block *newBlock = NULL;
    Block *pFrontBlock = NULL;
    Header *dataHeader = NULL;
    Header *remainHeader = NULL;
    void *res = NULL;
    /* 若申请内存过大，则不予分配 */
    if (dataSize > BLOCK_SIZE - 2 * sizeof(Header) - 4)
    {
        return NULL;
    }

    pSpareList = pBlockList->spareList;
    pTravelHeader = pSpareList->last->pre;
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
            newHeader = (Header *)(res + dataSize);
            newHeader->size = pTravelHeader->size - dataSize - sizeof(Header);
            pTravelHeader->size = dataSize;

            dmemory_move_to_alloc(pTravelHeader);
            dmemory_insert_to_spare(newHeader);
            return res;
        }
    }

    /* 程序运行到这里，说明spareList中找不到合适的内存块，需要创建新的Block作为内存分配来源 */
    newBlock = dmemory_Block_init();
    if (NULL == newBlock)
    {
        return NULL;
    }

    pFrontBlock = pBlockList->last;
    pBlockList->last = newBlock;

    pFrontBlock->next = newBlock;
    newBlock->pre = pFrontBlock;

    /* 从新创立的Block中划分出用户想要的内存大小交予用户，并为剩余内存设立Header，插入到空闲内存块中 */
    dataHeader = (Header *)(newBlock->block);
    res = dmemory_get_memAddr_by_header(dataHeader);

    remainHeader = (Header *)(res + dataSize);
    remainHeader->size = BLOCK_SIZE - dataSize - 2 * sizeof(Header);
    dataHeader->size = dataSize;

    dmemory_insert_to_alloc(dataHeader);
    dmemory_insert_to_spare(remainHeader);

    return res;
}

static void *dmemory_BestFit_search_spare(unsigned int dataSize)
{
    HList *pSpareList = NULL;
    Header *pTravelHeader = NULL;
    Header *newHeader = NULL;
    Block *newBlock = NULL;
    Block *pFrontBlock = NULL;
    Header *dataHeader = NULL;
    Header *remainHeader = NULL;
    void *res = NULL;

    /* 若申请内存过大，则不予分配 */
    if (dataSize > BLOCK_SIZE - 2 * sizeof(Header) - 4)
    {
        return NULL;
    }

    pSpareList = pBlockList->spareList;
    pTravelHeader = pSpareList->first->next;

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
            newHeader = (Header *)(res + dataSize);
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
    newBlock = dmemory_Block_init();
    if (NULL == newBlock)
    {
        return NULL;
    }

    pFrontBlock = pBlockList->last;
    pBlockList->last = newBlock;

    pFrontBlock->next = newBlock;
    newBlock->pre = pFrontBlock;

    /* 从新创立的Block中划分出用户想要的内存大小交予用户，并为剩余内存设立Header，插入到空闲内存块中 */
    dataHeader = (Header *)(newBlock->block);
    res = dmemory_get_memAddr_by_header(dataHeader);

    remainHeader = (Header *)(res + dataSize);
    remainHeader->size = BLOCK_SIZE - dataSize - 2 * sizeof(Header);
    dataHeader->size = dataSize;

    dmemory_insert_to_alloc(dataHeader);
    dmemory_insert_to_spare(remainHeader);

    return res;
}

static Header *dmemory_get_next_header(Header *pHeader)
{
    Header *res = NULL;
    if(NULL == pHeader)
    {
        return NULL;
    }
    /* 获取地址上连续的下一个Header。譬如：已知一个Header地址为x，其管理的内存块大小为n，则下一个Header地址为x+sizeof(Header)+n */
    res = (Header *)((void *)(pHeader + 1) + pHeader->size);
    if (0 == res->size)
    {
        return NULL;
    }
    return res;
}

static int dmemory_remove_from_HList(Header *pHeader)
{
    Header *front = NULL;
    Header *behind = NULL;
    if(NULL == pHeader)
    {
        return NULL_POINTER;
    }
    front = pHeader->pre;
    behind = pHeader->next;

    front->next = behind;
    behind->pre = front;

    pHeader->next = NULL;
    pHeader->pre = NULL;
    return OP_SUCCESS;
}

void dmemory_merge_spare_space()
{
    Header *pTravelHeader = NULL;
    Header *nextHeader = NULL;
    Header *frontHeader = NULL;
    /* 逐个Block进行空闲内存块的合并 */
    Block *ptrBlock = pBlockList->first;
    while (NULL != ptrBlock)
    {
        /* 从首个头部开始遍历 */
        pTravelHeader = (Header *)(ptrBlock->block);
        while (NULL != pTravelHeader && 0 != pTravelHeader->size)
        {
            /* 如果头部代表的内存块是空闲的 */
            if (NULL != pTravelHeader && pTravelHeader->available == 0xff)
            {
                /* 尝试与下一个内存块进行合并，直到nextHeader不是空闲内存块为止 */
                nextHeader = dmemory_get_next_header(pTravelHeader);
                while (NULL != nextHeader && nextHeader->available == 0xff)
                {
                    frontHeader = nextHeader;
                    nextHeader = dmemory_get_next_header(nextHeader);
                    pTravelHeader->size = pTravelHeader->size + frontHeader->size + sizeof(Header);
                    dmemory_remove_from_HList(frontHeader);
                }
                dmemory_remove_from_HList(pTravelHeader);
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
    Block *pBlock = NULL;
    if (NULL == pBlockList)
    {
        return;
    }
    /* 释放已分配链表和空闲链表 */
    dmemory_HList_delete(pBlockList->allocList);
    dmemory_HList_delete(pBlockList->spareList);

    pBlock = pBlockList->first->next;

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
    HList *allocList = NULL;
    Header *pTravelHeader = NULL;

    if (NULL == pHeader || NULL == pBlockList)
    {
        return NULL_POINTER;
    }
    allocList = pBlockList->allocList;
    pTravelHeader = allocList->first->next;

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
    void *res;
    if (NULL == pBlockList || 0 == dataSize)
    {
        return NULL;
    }

    if(STRATEGT_BEST_FIT == pBlockList->strategy)
    {
        res = dmemory_BestFit_search_spare(dataSize);
    }
    else
    {
        res = dmemory_WorstFit_search_spare(dataSize);
    }
    
    return res;
}

void *dmemory_calloc(unsigned int dataSize)
{
    void *res;
    if (NULL == pBlockList || 0 == dataSize)
    {
        return NULL;
    }

    if(STRATEGT_BEST_FIT == pBlockList->strategy)
    {
        res = dmemory_BestFit_search_spare(dataSize);
    }
    else
    {
        res = dmemory_WorstFit_search_spare(dataSize);
    }

    if (NULL == res)
    {
        return NULL;
    }

    memset(res, 0, dataSize);
    return res;
}

void *dmemory_realloc(void *srcPtr, unsigned int dataSize)
{
    void *res = NULL;
    Header *srcHeader = NULL;
    unsigned int cpSize = 0;
    if (NULL == srcPtr || 0 == dataSize)
    {
        return NULL;
    }

    srcHeader = (Header *)(srcPtr - sizeof(Header));
    if (dmemory_is_header_in_alloc(srcHeader) < 0)
    {
        return NULL;
    }

    if(STRATEGT_BEST_FIT == pBlockList->strategy)
    {
        res = dmemory_BestFit_search_spare(dataSize);
    }
    else
    {
        res = dmemory_WorstFit_search_spare(dataSize);
    }
    
    if (NULL == res)
    {
        return NULL;
    }

    cpSize = dataSize > (srcHeader->size) ? (srcHeader->size) : dataSize;
    memcpy(res, srcPtr, cpSize);

    dmemory_move_to_spare(srcHeader);
    return res;
}

int dmemory_free(void *pData)
{
    Header *pDataHeader = NULL;
    HList *allocList = NULL;
    Header *pTravelHeader = NULL;

    static unsigned int free_count = 0;
    if (NULL == pData || NULL == pBlockList)
    {
        return NULL_POINTER;
    }
    pDataHeader = (Header *)(pData - sizeof(Header));
    allocList = pBlockList->allocList;
    pTravelHeader = allocList->first->next;

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
        return ILLEGAL_INDEX;
    }
    dmemory_move_to_spare(pDataHeader);

    free_count++;
    if (free_count >= BLOCK_MERGE_INDEX)
    {
        dmemory_merge_spare_space();
        free_count = 0;
    }
    return OP_SUCCESS;
}

int dmemory_set_strategy(int strategy)
{
    if(NULL == pBlockList)
    {
        return NULL_POINTER;
    }

    if(STRATEGT_BEST_FIT == strategy)
    {
        pBlockList->strategy = strategy;
        return OP_SUCCESS;
    }
    else if(STRATEGY_WORST_FIT == strategy)
    {
        pBlockList->strategy = strategy;
        return OP_SUCCESS;
    }
    else
    {
        return ILLEGAL_INDEX;
    }
    
}

unsigned int dmemory_fragment_space_count()
{
    char preAvailable = 0x00;
    unsigned int fragmentSpace = 0;
    Block *pTravelBlock = NULL;
    Header *pTravelHeader = NULL;
    Header *pNextHeader = NULL;

    if(NULL == pBlockList)
    {
        return 0;
    }
    pTravelBlock = pBlockList->first;
    while (NULL != pTravelBlock)
    {
        preAvailable = 0x00;
        pTravelHeader = (Header *)(pTravelBlock->block);

        while (NULL != pTravelHeader)
        {
            pNextHeader = dmemory_get_next_header(pTravelHeader);
            if(NULL != pNextHeader 
            && 0x00 == preAvailable 
            && 0x00 == pNextHeader->available 
            && 0xff == pTravelHeader->available
            && pTravelHeader->size < FRAGMENT_EDGE)
            {
                fragmentSpace = fragmentSpace + pTravelHeader->size;
                pTravelHeader = dmemory_get_next_header(pNextHeader);
                preAvailable = pNextHeader->available;
            }
            else if(NULL == pNextHeader 
            && 0x00 == preAvailable 
            && 0xff == pTravelHeader->available
            && pTravelHeader->size < FRAGMENT_EDGE)
            {
                fragmentSpace = fragmentSpace + pTravelHeader->size;
                break;
            }
            else
            {
                preAvailable = pTravelHeader->available;
                pTravelHeader = dmemory_get_next_header(pTravelHeader);
            }
        }
        pTravelBlock = pTravelBlock->next;
    }

    return fragmentSpace;
    
}