#include "../include/hashTable.h"

/*************************************************************************************************************************************/
/*												     TYPES																		     */
/*************************************************************************************************************************************/
struct hashTable
{
    size_t tableSize;//桶的个数
    list **listArray;//桶数组
    int count;//内部容纳的数据节点数量

    fp_hash_func hashIt;//用户传入的hash函数
};


/*************************************************************************************************************************************/
/*												   FUNCTIONS																		 */
/*************************************************************************************************************************************/

/*
*@fn				hashTable *init_hashTable(size_t tableSize, size_t dataSize)
*@brief   			用于初始化哈希表
*@details			
*
*@param[in] size_t tableSize				哈希表的桶的数量
*
*@return			若成功初始化，返回空的哈希表的指针；否则，返回NULL
*@retval			
*/
hashTable *init_hashTable(size_t tableSize, fp_hash_func hashFunc)
{
    if(0 == tableSize)
    {
        return NULL;
    }

    hashTable *pHashTable = (hashTable *)malloc(sizeof(hashTable));
    if(NULL == pHashTable)
    {
        return NULL;
    }
    pHashTable->tableSize = tableSize;
    
    list **pListArray = (list **)malloc(sizeof(list *) * tableSize);
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
*@fn				void delete_hashTable(hashTable *pHashTable)
*@brief   			用于销毁哈希表
*@details			
*
*@param[in] size_t tableSize				指向要销毁的哈希表的指针
*
*@return			N/A
*@retval			
*/
void delete_hashTable(hashTable *pHashTable)
{
    if(NULL == pHashTable)
    {
        return ;
    }

    int i = 0;
    for(i = 0; i < pHashTable->tableSize; i++)
    {
        delete_list(pHashTable->listArray[i]);
    }

    free(pHashTable);
}

/*
*@fn				unsigned int hash_function(const void *key, size_t keySize)
*@brief   			用于将key映射成hash值
*@details			
*
*@param[in] const void* key				要进行映射的key
*@param[in] size_t keySize              key所指空间的大小
*
*@return			根据key映射的hash值
*@retval			
*/
static unsigned int hash_function(hashTable *pHashTable, const void *key, size_t keySize)
{
    unsigned int hash = 0;

    if(NULL == pHashTable->hashIt)//缺省情况
    {
        unsigned char *pData = (unsigned char *)key;

        int i = 0;
        for(i = 0; i < keySize; ++i)
        {
            hash = 31 * hash + (unsigned int)(pData[i]);
        }
    }
    else//用户传入了自定义哈希函数的情况
    {
        hash = pHashTable->hashIt(key, keySize);
    }
    
    return hash;
}

/*
*@fn				int insert_data_to_hashTable(hashTable *pHashTable, const void *key, size_t keySize, void *data)
*@brief   			用于将数据插入到哈希表中
*@details			
*
*@param[in] hashTable* pHashTable		要进行插入操作的hash表	
*@param[in] const void* key             插入所依据的key  
*@param[in] size_t keySize              key的大小（单位：字节）
*@param[in] void* data                  要进行插入的数据
*
*@return			                    返回执行信息，共有三种:SUCCESS：挂载成功  NULL_POINTER：链表指针为空	CREAT_NODE_FAILED：创建节点失败   
*@retval			
*/
int insert_data_to_hashTable(hashTable *pHashTable, const void *key, size_t keySize, void *data)
{
    if(NULL == pHashTable || NULL == data || NULL == key || 0 == keySize)
    {
        return NULL_POINTER;
    }

    unsigned int hash = hash_function(pHashTable, key, keySize);
    unsigned int index = hash % (pHashTable->tableSize);

    if(NULL == pHashTable->listArray[index])
    {
        pHashTable->listArray[index] = init_list(NULL);
    }

    int res = appened_node_from_data(pHashTable->listArray[index], data);
    if(SUCCESS == res)
    {
        pHashTable->count++;
    }

    return res;
}

/*
*@fn				listNode *query_hashTable_by_key(hashTable *pHashTable, const void *key, size_t keySize, void *data, fp_compare cmp)
*@brief   			用于在hash表中依据key和data进行查找
*@details			
*
*@param[in] hashTable* pHashTable		    要进行查询操作的哈希表		
*@param[in] const void* key                 hash操作所依赖的key
*@param[in] size_t keySize                  key的大小（单位：字节）
*@param[in] void* data                      用于进行比较的数据
*@param[in] fp_compare cmp                  用于进行比较的函数
*
*@return			若查到了，则返回查询到的节点的数据域的指针；否则返回NULL
*@retval			
*/
void *query_hashTable_by_key(hashTable *pHashTable, const void *key, size_t keySize, fp_compare cmp)
{
    if(NULL == pHashTable || NULL == key || 0 == keySize || NULL == cmp)
    {
        return NULL;
    }

    unsigned int hash = hash_function(pHashTable, key, keySize);
    unsigned int index = hash % (pHashTable->tableSize);

    listNode *pNode = get_node_by_data(pHashTable->listArray[index], key, cmp);
    
    return get_data_from_node(pNode);
}

/*
*@fn				int delete_node_from_hashTable(hashTable *pHashTable,const void *key, size_t keySize, void *data, fp_compare cmp)
*@brief   			用于在hash表中根据key对指定数据进行删除
*@details			
*
*@param[in] hashTable* pHashTable		    要进行查询操作的哈希表		
*@param[in] const void* key                 hash操作所依赖的key
*@param[in] size_t keySize                  key的大小（单位：字节）
*@param[in] void* data                      用于进行比较的数据，若传入NULL，则根据key和cmp进行删除操作；若传入非NULL的值，将根据key进行映射，在对应的链表中根据data和cmp进行删除操作
*@param[in] fp_compare cmp                  用于进行比较的函数
*
*@return			执行情况，分为三种：NODE_NOT_EXIST--->要删除的节点不存在；SUCCESS：删除成功；NULL_POINTER：传入了空指针或非法值
*@retval			
*/
int delete_node_from_hashTable(hashTable *pHashTable, const void *key, size_t keySize, const void *data, fp_compare cmp)
{
    if(NULL == pHashTable || NULL == key || 0 == keySize)
    {
        return NULL_POINTER;
    }

    unsigned int hash = hash_function(pHashTable, key, keySize);
    unsigned int index = hash % (pHashTable->tableSize);
    int res = 0;
    if(NULL == data)
    {
        res = delete_node_by_data(pHashTable->listArray[index], key, cmp);
    }
    else
    {
        res = delete_node_by_data(pHashTable->listArray[index], data, cmp);
    }
    
    if(res > 0)
    {
        (pHashTable->count) -= res;
    }
    return res;
}

/*
*@fn				void print_hashTable(hashTable *pHashTable, fp_print printData)
*@brief   			用于对hash表中的数据进行打印
*@details			
*
*@param[in] hashTable* pHashTable			指向要打印的hash表的指针
*@param[in] fp_print printData              用于打印单个数据的函数指针，其实现由使用者提供
*
*@return			N/A
*@retval			
*/
void print_hashTable(hashTable *pHashTable, fp_print printData)
{
    if(NULL == pHashTable || NULL == printData)
    {
        return;
    }
    int i = 0;
    for(i = 0; i < pHashTable->tableSize; i++)
    {
        printf("*************************HashTable[%d]***************************\n", i);
        printList(pHashTable->listArray[i], printData);
    }
}

int get_count_of_hashTable(const hashTable *pHashTable)
{
    if(NULL == pHashTable)
    {
        return 0;
    }

    return pHashTable->count;
}