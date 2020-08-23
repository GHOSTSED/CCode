#ifndef _DMEMORY_H_
#define _DMEMORY_H_

/*************************************************************************************************************************************/
/*												  INCLUDE_FILES																		 */
/*************************************************************************************************************************************/
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>

/*************************************************************************************************************************************/
/*												    DEFINES																		 	 */
/*************************************************************************************************************************************/
#define BLOCK_SIZE 64*1024
#define MALLOC_FAILED -1
#define NULL_POINTER -2
#define EXIST 2
#define NOT_EXIST -3
#define INIT_SUCCESS 1
#define OP_SUCCESS 1
#define BLOCK_MERGE_INDEX 50
#define ILLEGAL_INDEX -5
#define STRATEGT_BEST_FIT 10
#define STRATEGY_WORST_FIT 20
#define FRAGMENT_EDGE 256

/*************************************************************************************************************************************/
/*												   FUNCTIONS																		 */
/*************************************************************************************************************************************/
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
*@fn							    int dmemory_set_strategy(int strategy);
*@brief   						    用于设置内存分配策略的函数
*@details	
*
*@param[in] int strategy            策略对应的序号（目前有STRATEGY_BEST_FIT和STRATEGY_WORST_FIT两种）               
*
*@return						    执行情况，分为3种：NULL_POINTER：pBlockList未初始化；ILLEGAL_INDEX：传入的策略标号非法；OP_SUCCESS：设置成功
*@retval
*/
int dmemory_set_strategy(int strategy);

unsigned int dmemory_fragment_space_count();

#endif