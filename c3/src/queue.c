#include "../include/queue.h"

/*************************************************************************************************************************************/
/*												     TYPES																		     */
/*************************************************************************************************************************************/
struct queue
{
    list *queueList;
};

/*************************************************************************************************************************************/
/*												   FUNCTIONS																		 */
/*************************************************************************************************************************************/

/*
*@fn				queue *init_queue(void *lock)
*@brief   			用于对队列进行初始化
*@details			
*
*@param[in] void* lock		对队列上的锁，传入前当由使用者进行锁的初始化操作
*
*@return			指向生成的队列的指针
*@retval			
*/
queue *init_queue(void *lock)
{
    queue *pQueue = (queue *)malloc(sizeof(queue));
    if(NULL == pQueue)
    {
        return NULL;
    }

    list *pList = init_list(lock);
    if(NULL == pList)
    {
        free(pQueue);
        return NULL;
    }

    pQueue->queueList = pList;
    return pQueue;
}

/*
*@fn				            void delete_queue(queue *pQueue)
*@brief   			            用于删除队列
*@details			
*
*@param[in] queue* pQueue		指向要删除的队列的指针
*
*@return			            N/A
*@retval			
*/
void delete_queue(queue *pQueue)
{
    delete_list(pQueue->queueList);
    free(pQueue);
}

/*
*@fn				            void *get_queue_head_data(queue *pQueue)
*@brief   			            获取队头节点中的数据
*@details			
*
*@param[in] queue* pQueue		指向要获取队头数据的队列的指针
*
*@return			            指向队列队头数据的指针
*@retval			
*/
void *get_queue_head_data(queue *pQueue)
{
    if(NULL == pQueue)
    {
        return NULL;
    }

    listNode *pNode = get_node_by_index(pQueue->queueList, 1);
    if(NULL == pNode)
    {
        return NULL;
    }

    return (get_data_from_node(pNode));
}

/*
*@fn				            int enqueue(queue *pQueue, void *data)
*@brief   			            将data加入队尾
*@details			
*
*@param[in] queue* pQueue		指向要加入数据的队列的指针
*@param[in] void* data          指向要入队的数据的指针
*
*@return			            返回执行信息，共有三种:SUCCESS：挂载成功  NULL_POINTER：链表指针为空	CREAT_NODE_FAILED：创建节点失败
*@retval			
*/
int enqueue(queue *pQueue, void *data)
{
    int res = appened_node_from_data(pQueue->queueList, data);
    return res;
}

/*
*@fn				            int dequeue(queue *pQueue)
*@brief   			            删除当前队头节点
*@details			
*
*@param[in] queue* pQueue		指向要进行出队操作的队列的指针
*
*@return			            执行情况，共有3种情况：NULL_POINTER：传入空指针；SUCCESS：成功；OUT_OF_RANGE：index不合法
*@retval			
*/
int dequeue(queue *pQueue)
{
    int res = delete_node_by_index(pQueue->queueList, 1);
    return res;
}

/*
*@fn				            int get_queue_count(queue *pQueue)
*@brief   			            获取当前队列长度
*@details			
*
*@param[in] queue* pQueue		指向要获取长度的队列的指针
*
*@return			            当前队列长度
*@retval			
*/
int get_queue_count(queue *pQueue)
{
    if(NULL == pQueue)
    {
        return NULL_POINTER;
    }
    return (get_data_count(pQueue->queueList));
}

/*
*@fn				            void print_queue(queue *pQueue, fp_print printData)
*@brief   			            打印当前队列中的数据
*@details			
*
*@param[in] queue* pQueue		指向要打印的队列的指针
*@param[in] fp_print printData  打印单个数据的函数指针，由使用者传入
*
*@return			            N/A
*@retval			
*/
void print_queue(queue *pQueue, fp_print printData)
{
    printList(pQueue->queueList, printData);
}

/*
*@fn				            void lock_queue(queue *pQueue, fp_lock_list lock_it)
*@brief   			            对队列进行加锁操作
*@details			
*
*@param[in] queue* pQueue		    指向要加锁的队列的指针
*@param[in] fp_lock_list lock_it    加锁函数指针，由使用者实现并提供
*
*@return			            N/A
*@retval			
*/
void lock_queue(queue *pQueue, fp_lock_list lock_it)
{
    lockList(pQueue->queueList, lock_it);
}

/*
*@fn				            void unlock_queue(queue *pQueue, fp_lock_list unlock_it)
*@brief   			            对队列进行解锁操作
*@details			
*
*@param[in] queue* pQueue		        指向要解锁的队列的指针
*@param[in] fp_lock_list unlock_it      解锁函数指针，由使用者实现并提供
*
*@return			            N/A
*@retval			
*/
void unlock_queue(queue *pQueue, fp_lock_list unlock_it)
{
    unlockList(pQueue->queueList, unlock_it);
}