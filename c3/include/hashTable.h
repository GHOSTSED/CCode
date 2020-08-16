#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

/*************************************************************************************************************************************/
/*												  INCLUDE_FILES																		 */
/*************************************************************************************************************************************/
#include"list.h"

/*************************************************************************************************************************************/
/*												     TYPES																		     */
/*************************************************************************************************************************************/
typedef struct hashTable hashTable;
typedef unsigned int(*fp_hash_func)(const void *key, size_t keySize);


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
hashTable *init_hashTable(size_t tableSize, fp_hash_func hashFunc);

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
void delete_hashTable(hashTable *pHashTable);

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
// unsigned int hash_function(const void *key, size_t keySize);

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
int insert_data_to_hashTable(hashTable *pHashTable, const void *key, size_t keySize, void *data);

/*
*@fn				listNode *query_hashTable_by_key(hashTable *pHashTable, const void *key, size_t keySize, void *data, fp_compare cmp)
*@brief   			用于在hash表中依据key和data进行查找
*@details			
*
*@param[in] hashTable* pHashTable		    要进行查询操作的哈希表		
*@param[in] const void* key                 hash操作所依赖的key
*@param[in] size_t keySize                  key的大小（单位：字节）
*@param[in] fp_compare cmp                  用于进行比较的函数
*
*@return			若查到了，则返回查询到的节点数据域的指针；否则返回NULL
*@retval			
*/
void *query_hashTable_by_key(hashTable *pHashTable, const void *key, size_t keySize, fp_compare cmp);

/*
*@fn				int delete_node_from_hashTable(hashTable *pHashTable,const void *key, size_t keySize, void *data, fp_compare cmp)
*@brief   			用于在hash表中根据key对指定数据进行删除
*@details			
*
*@param[in] hashTable* pHashTable		    要进行查询操作的哈希表		
*@param[in] const void* key                 hash操作所依赖的key
*@param[in] size_t keySize                  key的大小（单位：字节）
*@param[in] void* data                      用于进行比较的数据
*@param[in] fp_compare cmp                  用于进行比较的函数
*
*@return			执行情况，分为两种：NODE_NOT_EXIST--->要删除的节点不存在；SUCCESS：删除成功
*@retval			
*/
int delete_node_from_hashTable(hashTable *pHashTable, const void *key, size_t keySize, const void *data, fp_compare cmp);

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
void print_hashTable(hashTable *pHashTable, fp_print printData);

int get_count_of_hashTable(const hashTable *pHashTable);

#endif

