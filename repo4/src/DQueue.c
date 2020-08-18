#include "../include/DQueue.h"

/*************************************************************************************************************************************/
/*												     TYPES																		     */
/*************************************************************************************************************************************/
struct _DQueue
{
    DList *queueList;
};

/*************************************************************************************************************************************/
/*												   FUNCTIONS																		 */
/*************************************************************************************************************************************/

DQueue *dqueue_init(void *lock);
void dqueue_delete(DQueue *pQueue);
void *dqueue_get_head_data(DQueue *pQueue);
int dqueue_in(DQueue *pQueue, void *data);
int dqueue_out(DQueue *pQueue);
int dqueue_get_count(DQueue *pQueue);
void dqueue_print(DQueue *pQueue, fp_print printData);
void dqueue_lock(DQueue *pQueue, fp_lock_list lock_it);
void dqueue_unlock(DQueue *pQueue, fp_lock_list unlock_it);

/*
*@fn				DQueue *dqueue_init(void *lock)
*@brief   			用于对队列进行初始化
*@details			
*
*@param[in] void* lock		对队列上的锁，传入前当由使用者进行锁的初始化操作
*
*@return			指向生成的队列的指针
*@retval			
*/
DQueue *dqueue_init(void *lock)
{
    DQueue *pQueue = (DQueue *)malloc(sizeof(DQueue));
    if(NULL == pQueue)
    {
        return NULL;
    }

    DList *pList = dlist_init(lock);
    if(NULL == pList)
    {
        free(pQueue);
        return NULL;
    }

    pQueue->queueList = pList;
    return pQueue;
}

/*
*@fn				            void dqueue_delete(DQueue *pQueue)
*@brief   			            用于删除队列
*@details			
*
*@param[in] DQueue* pQueue		指向要删除的队列的指针
*
*@return			            N/A
*@retval			
*/
void dqueue_delete(DQueue *pQueue)
{
    dlist_delete(pQueue->queueList);
    free(pQueue);
}

/*
*@fn				            void *dqueue_get_head_data(DQueue *pQueue)
*@brief   			            获取队头节点中的数据
*@details			
*
*@param[in] DQueue* pQueue		指向要获取队头数据的队列的指针
*
*@return			            指向队列队头数据的指针
*@retval			
*/
void *dqueue_get_head_data(DQueue *pQueue)
{
    if(NULL == pQueue)
    {
        return NULL;
    }

    DListNode *pNode = dlist_get_node_by_index(pQueue->queueList, 1);
    if(NULL == pNode)
    {
        return NULL;
    }

    return (dlist_get_data_from_node(pNode));
}

/*
*@fn				            int dqueue_in(DQueue *pQueue, void *data)
*@brief   			            将data加入队尾
*@details			
*
*@param[in] DQueue* pQueue		指向要加入数据的队列的指针
*@param[in] void* data          指向要入队的数据的指针
*
*@return			            返回执行信息，共有三种:SUCCESS：挂载成功  NULL_POINTER：链表指针为空	CREAT_NODE_FAILED：创建节点失败
*@retval			
*/
int dqueue_in(DQueue *pQueue, void *data)
{
    if(NULL == pQueue || NULL == data)
    {
        return NULL_POINTER;
    }
    int res = dlist_appened_node(pQueue->queueList, data);
    return res;
}

/*
*@fn				            int dqueue_out(DQueue *pQueue)
*@brief   			            删除当前队头节点
*@details			
*
*@param[in] DQueue* pQueue		指向要进行出队操作的队列的指针
*
*@return			            执行情况，共有3种情况：NULL_POINTER：传入空指针；SUCCESS：成功；OUT_OF_RANGE：index不合法
*@retval			
*/
int dqueue_out(DQueue *pQueue)
{
    if(NULL == pQueue)
    {
        return NULL_POINTER;
    }
    int res = dlist_delete_node_by_index(pQueue->queueList, 1);
    return res;
}

/*
*@fn				            int dqueue_get_count(DQueue *pQueue)
*@brief   			            获取当前队列长度
*@details			
*
*@param[in] DQueue* pQueue		指向要获取长度的队列的指针
*
*@return			            当前队列长度
*@retval			
*/
int dqueue_get_count(DQueue *pQueue)
{
    if(NULL == pQueue)
    {
        return 0;
    }
    return (dlist_get_data_count(pQueue->queueList));
}

/*
*@fn				            void dqueue_print(DQueue *pQueue, fp_print printData)
*@brief   			            打印当前队列中的数据
*@details			
*
*@param[in] DQueue* pQueue		指向要打印的队列的指针
*@param[in] fp_print printData  打印单个数据的函数指针，由使用者传入
*
*@return			            N/A
*@retval			
*/
void dqueue_print(DQueue *pQueue, fp_print printData)
{
    dlist_print(pQueue->queueList, printData);
}

/*
*@fn				            void dqueue_lock(DQueue *pQueue, fp_lock_list lock_it)
*@brief   			            对队列进行加锁操作
*@details			
*
*@param[in] DQueue* pQueue		    指向要加锁的队列的指针
*@param[in] fp_lock_list lock_it    加锁函数指针，由使用者实现并提供
*
*@return			            N/A
*@retval			
*/
void dqueue_lock(DQueue *pQueue, fp_lock_list lock_it)
{
    dlist_lock(pQueue->queueList, lock_it);
}

/*
*@fn				            void dqueue_unlock(DQueue *pQueue, fp_lock_list unlock_it)
*@brief   			            对队列进行解锁操作
*@details			
*
*@param[in] DQueue* pQueue		        指向要解锁的队列的指针
*@param[in] fp_lock_list unlock_it      解锁函数指针，由使用者实现并提供
*
*@return			            N/A
*@retval			
*/
void dqueue_unlock(DQueue *pQueue, fp_lock_list unlock_it)
{
    dlist_unlock(pQueue->queueList, unlock_it);
}