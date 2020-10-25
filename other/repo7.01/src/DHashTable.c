#include "../include/DHashTable.h"

/*************************************************************************************************************************************/
/*												     TYPES																		     */
/*************************************************************************************************************************************/
struct _DHashTable
{
    size_t tableSize;//桶的个数
    DList **listArray;//链表数组
    int count;//当前数据个数

    fp_hash_func hashIt;//用户传入的哈希函数
};


/*************************************************************************************************************************************/
/*												   FUNCTIONS																		 */
/*************************************************************************************************************************************/

/*
*@fn				DHashTable *dhashTable_init(size_t tableSize, size_t dataSize)
*@brief   			用于初始化哈希表
*@details			
*
*@param[in] size_t tableSize				哈希表的桶的数量
*
*@return			若成功初始化，返回空的哈希表的指针；否则，返回NULL
*@retval			
*/
DHashTable *dhashTable_init(size_t tableSize, fp_hash_func hashFunc)
{
    if(0 == tableSize)
    {
        return NULL;
    }

    DHashTable *pHashTable = (DHashTable *)malloc(sizeof(DHashTable));
    if(NULL == pHashTable)
    {
        return NULL;
    }
    pHashTable->tableSize = tableSize;
    
    DList **pListArray = (DList **)malloc(sizeof(DList *) * tableSize);
    if(NULL == pListArray)
    {
        free(pHashTable);
        return NULL;
    }
    pHashTable->listArray = pListArray;

    int i = 0;
    for(i = 0; i < tableSize; i++)
    {
        pHashTable->listArray[i] = NULL;
    }
    pHashTable->hashIt = hashFunc;
    pHashTable->count = 0;

    return pHashTable;
}

/*
*@fn				void dhashTable_delete(DHashTable *pHashTable)
*@brief   			用于销毁哈希表
*@details			
*
*@param[in] size_t tableSize				指向要销毁的哈希表的指针
*
*@return			N/A
*@retval			
*/
void dhashTable_delete(DHashTable *pHashTable)
{
    if(NULL == pHashTable)
    {
        return ;
    }

    int i = 0;
    for(i = 0; i < pHashTable->tableSize; i++)
    {
        dlist_delete(pHashTable->listArray[i]);
    }

    free(pHashTable);
}

/*
*@fn				unsigned int dhashTable_hash_function(const void *key, size_t keySize)
*@brief   			用于将key映射成hash值
*@details			
*
*@param[in] const void* key				要进行映射的key
*@param[in] size_t keySize              key所指空间的大小
*
*@return			根据key映射的hash值
*@retval			
*/
static unsigned int dhashTable_hash_function(DHashTable *pHashTable, const void *key, size_t keySize)
{
    unsigned int hash = 0;

    if(NULL == pHashTable->hashIt)
    {
        unsigned char *pData = (unsigned char *)key;

        int i = 0;
        for(i = 0; i < keySize; ++i)
        {
            hash = 31 * hash + (unsigned int)(pData[i]);
        }
    }
    else
    {
        hash = pHashTable->hashIt(key, keySize);
    }
    
    return hash;
}

/*
*@fn				int dhashTable_insert_data(DHashTable *pHashTable, const void *key, size_t keySize, void *data)
*@brief   			用于将数据插入到哈希表中
*@details			
*
*@param[in] DHashTable* pHashTable		要进行插入操作的hash表	
*@param[in] const void* key             插入所依据的key  
*@param[in] size_t keySize              key的大小（单位：字节）
*@param[in] void* data                  要进行插入的数据
*
*@return			                    返回执行信息，共有三种:SUCCESS：挂载成功  NULL_POINTER：链表指针为空	CREAT_NODE_FAILED：创建节点失败   
*@retval			
*/
int dhashTable_insert_data(DHashTable *pHashTable, const void *key, size_t keySize, void *data)
{
    if(NULL == pHashTable || NULL == data || NULL == key || 0 == keySize)
    {
        return NULL_POINTER;
    }

    unsigned int hash = dhashTable_hash_function(pHashTable, key, keySize);
    unsigned int index = hash % (pHashTable->tableSize);

    if(NULL == pHashTable->listArray[index])
    {
        pHashTable->listArray[index] = dlist_init(NULL);
    }

    int res = dlist_appened_node(pHashTable->listArray[index], data);
    if(SUCCESS == res)
    {
        pHashTable->count++;
    }

    return res;
}

/*
*@fn				DListNode *dhashTable_query_by_key(DHashTable *pHashTable, const void *key, size_t keySize, void *data, fp_compare cmp)
*@brief   			用于在hash表中依据key和data进行查找
*@details			
*
*@param[in] DHashTable* pHashTable		    要进行查询操作的哈希表		
*@param[in] const void* key                 hash操作所依赖的key
*@param[in] size_t keySize                  key的大小（单位：字节）
*@param[in] void* data                      用于进行比较的数据
*@param[in] fp_compare cmp                  用于进行比较的函数
*
*@return			若查到了，则返回查询到的节点的数据域的指针；否则返回NULL
*@retval			
*/
void *dhashTable_query_by_key(DHashTable *pHashTable, const void *key, size_t keySize, fp_compare cmp)
{
    if(NULL == pHashTable || NULL == key || 0 == keySize || NULL == cmp)
    {
        return NULL;
    }

    unsigned int hash = dhashTable_hash_function(pHashTable, key, keySize);
    unsigned int index = hash % (pHashTable->tableSize);

    DListNode *pNode = dlist_get_node_by_data(pHashTable->listArray[index], key, cmp);
    
    return dlist_get_data_from_node(pNode);
}

/*
*@fn				int dhashTable_delete_data(DHashTable *pHashTable,const void *key, size_t keySize, void *data, fp_compare cmp)
*@brief   			用于在hash表中根据key对指定数据进行删除
*@details			
*
*@param[in] DHashTable* pHashTable		    要进行查询操作的哈希表		
*@param[in] const void* key                 hash操作所依赖的key
*@param[in] size_t keySize                  key的大小（单位：字节）
*@param[in] void* data                      用于进行比较的数据，若传入NULL，则根据key和cmp进行删除操作；若传入非NULL的值，将根据key进行映射，在对应的链表中根据data和cmp进行删除操作
*@param[in] fp_compare cmp                  用于进行比较的函数
*
*@return			执行情况，分为三种：NODE_NOT_EXIST--->要删除的节点不存在；SUCCESS：删除成功；NULL_POINTER：传入了空指针或非法值
*@retval			
*/
int dhashTable_delete_data(DHashTable *pHashTable, const void *key, size_t keySize, const void *data, fp_compare cmp)
{
    if(NULL == pHashTable || NULL == key || 0 == keySize)
    {
        return NULL_POINTER;
    }

    unsigned int hash = dhashTable_hash_function(pHashTable, key, keySize);
    unsigned int index = hash % (pHashTable->tableSize);
    int res = 0;
    if(NULL == data)
    {
        res = dlist_delete_node_by_data(pHashTable->listArray[index], key, cmp);
    }
    else
    {
        res = dlist_delete_node_by_data(pHashTable->listArray[index], data, cmp);
    }
    
    if(res > 0)
    {
        (pHashTable->count) -= res;
    }
    return res;
}

/*
*@fn				void dhashTable_print(DHashTable *pHashTable, fp_print printData)
*@brief   			用于对hash表中的数据进行打印
*@details			
*
*@param[in] DHashTable* pHashTable			指向要打印的hash表的指针
*@param[in] fp_print printData              用于打印单个数据的函数指针，其实现由使用者提供
*
*@return			N/A
*@retval			
*/
void dhashTable_print(DHashTable *pHashTable, fp_print printData)
{
    if(NULL == pHashTable || NULL == printData)
    {
        return;
    }
    int i = 0;
    for(i = 0; i < pHashTable->tableSize; i++)
    {
        printf("*************************HashTable[%d]***************************\n", i);
        dlist_print(pHashTable->listArray[i], printData);
    }
}

int dhashTable_get_count(const DHashTable *pHashTable)
{
    if(NULL == pHashTable)
    {
        return 0;
    }

    return pHashTable->count;
}