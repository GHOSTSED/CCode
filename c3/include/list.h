#ifndef _LIST_H_
#define _LIST_H_

/*************************************************************************************************************************************/
/*												  INCLUDE_FILES																		 */
/*************************************************************************************************************************************/
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

/*************************************************************************************************************************************/
/*												    DEFINES																		 	 */
/*************************************************************************************************************************************/
#define NULL_POINTER -1
#define OUT_OF_RANGE -2
#define SUCCESS 1
#define SHOULD_NOT_GET_HERE -114514
#define CREAT_NODE_FAILED -3
#define ILLEGAL_OPERATION -4
#define NODE_NOT_EXIST -7

/*************************************************************************************************************************************/
/*												     TYPES																		     */
/*************************************************************************************************************************************/
typedef void(*fp_print)(const void *data);//进行输出的函数
typedef int(*fp_compare)(const void *data1, const void *data2);//进行数据比较的函数，若data1<data2 返回负数；若等于，返回0；若大于，返回正数；
typedef void(*fp_lock_list)(void *lock);//用于对链表上锁的函数
typedef void(*fp_unlock_list)(void *lock);//用于对链表解锁的函数

typedef struct listNode listNode;//通用链表节点数据结构

typedef struct list list;//通用链表数据结构



/*************************************************************************************************************************************/
/*												   FUNCTIONS																		 */
/*************************************************************************************************************************************/

/*
*@fn							list *init_list(void *lock);
*@brief   						用于对链表进行初始化
*@details	
*
*@param[in] void* lock			用于锁链表的锁，可以为NULL。若不为NULL，则应为动态分配的空间。且锁传入之前应由使用者自己进行初始化操作
*
*@return						一个只有头节点的链表指针，若申请空间失败，则返回NULL
*@retval
*/
list *init_list(void *lock);

/*
*@fn							listNode *creat_node(void *srcData);
*@brief   						用于根据数据创建节点
*@details			
*
*@param[in] void* srcData		要存在节点中的数据的指针
*
*@return						指向创建的节点的指针
*@retval
*@note							不要在创建节点后将data指针所指空间释放，应通过删除节点/删除链表的方式间接释放
*/
listNode *creat_node(void *srcData);

/*
*@fn							int appened_node_from_data(list *pDstList, void *data);
*@brief   						用于根据数据创建节点，并将节点挂载在链表末尾
*@details	
*
*@param[in] list* pDstList			要进行挂载的链表指针
*@param[in]  void* data			要挂载的数据的指针
*
*@return						返回执行信息，共有三种:SUCCESS：挂载成功  NULL_POINTER：链表指针为空	CREAT_NODE_FAILED：创建节点失败
*@retval						
*/
int appened_node_from_data(list *pDstList, void *data);

/*
*@fn							void delete_node(listNode *pDelNode);
*@brief   						用于删除节点
*@details						仅仅进行简单的节点空间释放，不涉及对链表其他节点的操作
*
*@param[in] listNode* pDelNode	要进行删除的节点指针
*
*@return						N/A
*/
void delete_node(listNode *pDelNode);

/*
*@fn						void delete_list(list *pDelList);
*@brief   					用于删除某个链表，对其所有节点进行空间释放
*@details			
*
*@param[in] list* pDelList		要删除的链表的指针
*
*@return					N/A
*@retval
*/
void delete_list(list *pDelList);

/*
*@fn								int insert_node_behind(list *DstpList, listNode *pSrcNode, int index);
*@brief   							用于在链表的特定位置的后面插入节点
*@details							根据index插入，附加头节点的index是0，以此类推
*
*@param[in] list* pDstList			要插入的链表的指针
*@param[in] listNode* pSrcNode		要插入的节点的指针
*@param[in] int index				要插入的位置，合法值为0~（pList->count）（也即链表中的节点数量）
*
*@return							返回执行情况，共有4种情况：	NULL_POINTTER: 传入了空指针， 
*																OUT_OF_RANGE: 传入了非法的index， 
*																SUCCESS: 插入成功， 
*																SHOULD_NOT_GET_HERE：到达了不应该到达的地方
*\retval			
*/
int insert_node_behind_by_index(list *pDstList, listNode *pSrcNode, int index);

/*
*@fn						listNode *get_node_by_index(list *pSrcList, int index);
*@brief   					用于根据index获取链表中对应的节点
*@details			
*
*@param[in] list* pSrcList		指向要获取的节点所属的链表的指针
*@param[in] int index		要获取第几个节点
*
*@return					对应index的节点指针，若传入空指针或非法index，则返回NULL；否则返回对应节点的指针
*@retval			
*/
listNode *get_node_by_index(list *pSrcList, int index);

/*
*@fn				listNode *get_node_by_data(list *pSrcList, void *data, fp_compare cmp);
*@brief   			用于根据数据内容获取链表中对应的节点
*@details			返回的是第一个匹配到该数据的节点的指针
*
*@param[in] list* pSrcList		指向要获取的节点所属的链表的指针
*@param[in] void* data		和要获取的节点的data域相同的数据
*@param[in] fp_cmpare cmp		使用者提供的用于进行数据比较的函数
*
*@return			对应的节点指针，若传入空指针或链表中不存在的数据，则返回NULL；否则返回对应节点的指针
*@retval			
*/
listNode *get_node_by_data(list *pSrcList, const void *data, fp_compare cmp);

/*
*@fn				int delete_node_by_index(list *pSrcList, int index);
*@brief   			用于根据index删除链表中对应的节点
*@details			
*
*@param[in] list* pSrcList		指向要删除的节点所属的链表的指针
*@param[in] int index		要删除第几个节点
*
*@return			执行情况，共有3种情况：NULL_POINTER：传入空指针；SUCCESS：成功；OUT_OF_RANGE：index不合法
*@retval			
*/
int delete_node_by_index(list *pSrcList, int index);

/*
*@fn				int delete_node_by_data(list *pSrcList, void *data, fp_compare cmp);
*@brief   			用于根据数据内容删除链表中对应的节点
*@details			删除链表中所有数据与之匹配的节点
*
*@param[in] list* pSrcList		指向要删除的节点所属的链表的指针
*@param[in] void* data			和要删除的节点的data域相同的数据
*@param[in] fp_compare cmp		使用者提供的用于进行数据比较的函数
*
*@return			执行情况，分为两种：NODE_NOT_EXIST--->要删除的节点不存在；SUCCESS：删除成功
*@retval			
*/
int delete_node_by_data(list *pSrcList, void *data, fp_compare cmp);

/*
*@fn				void insert_sort(list *pDstList, fp_compare cmp, int flag);
*@brief   			用于对链表进行插入排序
*@details			
*
*@param[in] list* pDstList		指向要进行排序的链表的指针
*@param[in] fp_compare* cmp		使用者提供的比较函数
*@param[in] int flag			若flag >= 0，则进行升序排序；否则进行降序排序
*
*@return			N/A
*@retval			
*/
void insert_sort(list *pDstList, fp_compare cmp, int flag);


/*
*@fn				void merge_sort(list *pList, fp_compare cmp, int flag);
*@brief   			用于对链表进行归并排序
*@details			
*
*@param[in] list* pList			指向要进行排序的链表的指针
*@param[in] fp_compare cmp		使用者提供的比较函数
*@param[in] int flag			若flag >= 0，则进行升序排序；否则进行降序排序
*
*@return			N/A
*@retval			
*/
void merge_sort(list *pList, fp_compare cmp, int flag);

/*
*@fn				void merge_list(list *pDst, list *pSrc);
*@brief   			用于将两个链表合并，pSrcList中的节点将合并至pDstList，pSrcList最后只剩下一个附加头节点
*@details			
*
*@param[in] list* pDstList		需要合并的目的链表指针
*@param[in] list* pSrcList		需要合并的源链表指针
*
*@return			N/A
*@retval			
*/
void merge_list(list *pDstList, list *pSrcList);

/*
*@fn				void printList(list *pSrcList, fp_print print_data);
*@brief   			用于对链表中所有节点的数据进行逐个打印
*@details			
*
*@param[in] list* pSrcList			需要打印的链表指针
*@param[in]	fp_print print_data		使用者需要提供的对数据进行打印的函数
*
*@return			N/A
*@retval			
*/
void printList(list *pSrcList, fp_print print_data);

/*
*@fn				int delete_node_from_list(list *pList, listNode *pNode);
*@brief   			用于从pList中删除pNode指向的节点
*@details			
*
*@param[in] list* pList			需要进行删除操作的链表指针
*@param[in]	listNode* pNode		指向需要删除的节点的指针
*
*@return			返回执行情况，分为3种：NULL_POINTER: 传入了空指针； ILLEGAL_OPERATION: 试图删除附加头节点或pNode不是pList中的节点； SUCCESS: 删除成功
*@retval			
*/
int delete_node_from_list(list *pList, listNode *pNode);

/*
*@fn				int insert_node_behind_by_ptr(list *pDstList, listNode *pFrontNode, listNode *pInsertNode);
*@brief   			用于在pFrontNode节点后插入pInsertNode节点
*@details			
*
*@param[in] list* pDstList					需要进行插入操作的链表
*@param[in]	listNode* pFrontNode			在pFrontNode节点后进行插入操作
*@param[in] listNode* pInsertNode			待插入的节点
*
*@return			返回执行情况，分为3种：NULL_POINTER: 传入了空指针； ILLEGAL_OPERATION: pFrontNode不是pDstList中的节点； SUCCESS: 插入成功
*@retval			
*/
int insert_node_behind_by_ptr(list *pDstList, listNode *pFrontNode, listNode *pInsertNode);

/*
*@fn				listNode *get_node_by_data_from(listNode *pFromNode, void *data, fp_compare cmp);
*@brief   			用于查询从pFromNode开始、数据与data相同的第一个节点
*@details			
*
*@param[in] listNode* pFromNode			查询的起始位置
*@param[in]	void* data					用于进行比较的数据
*@param[in] fp_compare cmp				使用者需要提供的对数据进行比较的函数
*
*@return			N/A
*@retval			
*/
listNode *get_node_by_data_from(listNode *pFromNode, void *data, fp_compare cmp);

/*
*@fn				void lockList(list *pList, fp_lock_list lock);
*@brief   			用于锁住链表
*@details			
*
*@param[in] list *pList				要锁住的链表
*@param[in]	fp_lock_list lock		锁住链表的函数，由使用者提供
*
*@return			N/A
*@retval			
*/
void lockList(list *pList, fp_lock_list lock);

/*
*@fn				void unlockList(list *pList, fp_unlock_list unlock);
*@brief   			用于解锁链表
*@details			
*
*@param[in] list *pList					要解锁的链表
*@param[in]	fp_unlock_list unlock		解锁链表的函数，由使用者提供
*
*@return			N/A
*@retval			
*/
void unlockList(list *pList, fp_unlock_list unlock);

/*
*@fn				void *get_data_from_node(listNode *pNode);
*@brief   			用于获取指定节点中的数据
*@details			
*
*@param[in] listNode* pNode				要获取数据的节点
*
*@return			指向节点数据域的指针
*@retval			
*/
void *get_data_from_node(listNode *pNode);

/*
*@fn				int get_data_count(list *pList);
*@brief   			用于获取链表中的数据节点数量
*@details			
*
*@param[in] list* pList			要获取节点数量的链表
*
*@return			链表中数据节点数量
*@retval			
*/
int get_data_count(list *pList);


// void merge_by_order(listNode *pLeftHead, listNode *pRightHead, fp_compare cmp, int flag);

#endif // !_LIST_H_
