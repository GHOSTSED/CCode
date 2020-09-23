#include "../include/DList.h"

struct _DListNode
{
	void *data;
	struct _DListNode *next;
	struct _DListNode *pre;
};

struct _DList
{
	struct _DListNode *first; //头节点
	struct _DListNode *last;  //尾节点
	void *lock;				  //链表锁

	fp_delete_data dlist_data_delete;
};

void dlist_lock(DList *pList, fp_lock_list lockIt);
void dlist_unlock(DList *pList, fp_unlock_list unlockIt);
DList *dlist_init(void *mylock, fp_delete_data delete_data);
DListNode *dlist_creat_node(void *data);
void dlist_delete_node(DList *pList, DListNode *pNode);
void dlist_delete(DList *pList);
int dlist_delete_node_by_ptr(DList *pList, DListNode *pNode);
static void dlist_remove_node_from_list(DList *pList, DListNode *pNode);
int dlist_delete_node_by_data(DList *pList, const void *data, fp_compare cmp);
int dlist_delete_node_by_index(DList *pList, int index);
int dlist_insert_node_behind_by_ptr(DList *pList, DListNode *pFrontNode, DListNode *pInsertNode);
int dlist_insert_node_behind_by_index(DList *pList, DListNode *pInsertNode, int index);
int dlist_appened_node(DList *pDstList, void *data);
DListNode *dlist_get_node_by_index(DList *pList, int index);
DListNode *dlist_get_node_by_data(DList *pList, const void *data, fp_compare cmp);
DListNode *dlist_get_node_by_data_from(DListNode *pFromNode, const void *data, fp_compare cmp);
void dlist_print(DList *pList, fp_print print_data);
void dlist_insert_sort(DList *pList, fp_compare cmp, int flag);
static void dlist_merge_by_order(DListNode *pLeftHead, DListNode *pRightHead, fp_compare cmp, int flag);
static void dlist_merge_sort_r(DListNode *pHead, fp_compare cmp, int flag);
void dlist_merge_sort(DList *pList, fp_compare cmp, int flag);
void dlist_merge(DList *pDst, DList *pSrc);
void *dlist_get_data_from_node(DListNode *pNode);
int dlist_get_data_count(DList *pList);




void dlist_lock(DList *pList, fp_lock_list lockIt)
{
	if (NULL != lockIt)
	{
		lockIt(pList->lock);
	}
}

void dlist_unlock(DList *pList, fp_unlock_list unlockIt)
{
	if (NULL != unlockIt)
	{
		unlockIt(pList->lock);
	}
}

DList *dlist_init(void *mylock, fp_delete_data delete_data)
{
	DList *pList = NULL;
	DListNode *pHead = NULL;
	DListNode *pTail = NULL;

	pList = (DList *)malloc(sizeof(DList));
	if (NULL == pList)
	{
		return NULL;
	}

	pHead = (DListNode *)malloc(sizeof(DListNode));
	if (NULL == pHead)
	{
		free(pList);
		return NULL;
	}

	pTail = (DListNode *)malloc(sizeof(DListNode));
	if (NULL == pTail)
	{
		free(pHead);
		free(pList);
		return NULL;
	}

	pHead->pre = NULL;
	pHead->next = pTail;
	pHead->data = NULL;

	pTail->pre = pHead;
	pTail->next = NULL;
	pTail->data = NULL;

	pList->first = pHead;
	pList->last = pTail;

	pList->lock = mylock;
	pList->dlist_data_delete = delete_data;
	return pList;
}

DListNode *dlist_creat_node(void *data)
{
	DListNode *pNewNode = NULL;

	if (NULL == data)
	{
		return NULL;
	}

	pNewNode = (DListNode *)malloc(sizeof(DListNode));
	if (NULL == pNewNode)
	{
		return NULL;
	}

	pNewNode->data = data;

	pNewNode->pre = NULL;
	pNewNode->next = NULL;

	return pNewNode;
}

void dlist_delete_node(DList *pList, DListNode *pNode)
{
	if (NULL == pNode)
	{
		return;
	}

	if (NULL != pNode->data)
	{
		if(NULL != pList->dlist_data_delete)
		{
			pList->dlist_data_delete(pNode->data);
		}
		else
		{
			free(pNode->data); //由内而外释放内存，先释放结构体内部的指针的内存空间
		}
		
	}

	free(pNode); //释放节点的内存空间
}

void dlist_delete(DList *pList)
{
	if (NULL == pList)
	{
		return;
	}

	DListNode *pNode = NULL;
	//逐个删除并更新链表的头节点，直至完全删除
	while (NULL != pList->first)
	{
		pNode = pList->first;
		pList->first = pNode->next;

		dlist_delete_node(pList, pNode);
	}
	if(NULL != pList->lock)
	{
		free(pList->lock);
	}
	// free(pList->listLocker);
	free(pList);
}

int dlist_delete_node_by_ptr(DList *pList, DListNode *pNode)
{
	int flag = 0;
	if (NULL == pNode || NULL == pList)
	{
		return NULL_POINTER;
	}
	else if (pNode == pList->first || pNode == pList->last) //禁止删除附加头节点
	{
		return ILLEGAL_OPERATION;
	}
	else //检查pNode是否是pList中的节点
	{
		DListNode *pTmpNode = pList->first->next;
		while (pTmpNode != pList->last)
		{
			if (pTmpNode == pNode)
			{
				flag = 1;
				break;
			}
			pTmpNode = pTmpNode->next;
		}
		if (0 == flag)
		{
			return ILLEGAL_OPERATION;
		}
	}

	DListNode *preNode = pNode->pre;
	DListNode *nextNode = pNode->next;

	preNode->next = nextNode;
	nextNode->pre = preNode;

	//更新数据节点数量
	dlist_delete_node(pList, pNode);
	return SUCCESS;
}

//内部工具函数，用于将pNode从pList上摘除，但不delete掉pNode
static void dlist_remove_node_from_list(DList *pList, DListNode *pNode)
{
	if (NULL == pList || NULL == pNode)
	{
		return;
	}
	if (pNode == pList->first || pNode == pList->last)
	{
		return;
	}

	DListNode *preNode = pNode->pre;
	DListNode *nextNode = pNode->next;

	preNode->next = nextNode;
	nextNode->pre = preNode;
}

int dlist_delete_node_by_data(DList *pList, const void *data, fp_compare cmp)
{
	if (NULL == cmp || NULL == pList)
	{
		return NULL_POINTER;
	}
	DListNode *pRemoveNode = dlist_get_node_by_data(pList, data, cmp);
	DListNode *tmp = NULL;

	if (NULL == pRemoveNode)
	{
		return NODE_NOT_EXIST;
	}

	int count = 0;

	do
	{
		++count;
		tmp = pRemoveNode->next;
		dlist_delete_node_by_ptr(pList, pRemoveNode);
		pRemoveNode = tmp;
		pRemoveNode = dlist_get_node_by_data_from(pRemoveNode, data, cmp);
	} while (pRemoveNode != NULL && pRemoveNode != pList->last);

	return count;
}

int dlist_delete_node_by_index(DList *pList, int index)
{
	if (NULL == pList)
	{
		return NULL_POINTER;
	}

	if (index < 1)
	{
		return OUT_OF_RANGE;
	}

	DListNode *pTmpNode = dlist_get_node_by_index(pList, index);
	if (NULL == pTmpNode)
	{
		return NULL_POINTER;
	}

	dlist_delete_node_by_ptr(pList, pTmpNode);
	return SUCCESS;
}

int dlist_insert_node_behind_by_ptr(DList *pList, DListNode *pFrontNode, DListNode *pInsertNode)
{
	if (NULL == pFrontNode || NULL == pInsertNode || NULL == pList)
	{
		return NULL_POINTER;
	}
	if (pFrontNode == pList->last)
	{
		return ILLEGAL_OPERATION;
	}

	//检查pFrontNode是否是pList中的节点
	int flag = 0;
	DListNode *pTravelNode = pList->first;
	while (pList->last != pTravelNode)
	{
		if (pTravelNode == pFrontNode)
		{
			flag = 1;
			break;
		}
		pTravelNode = pTravelNode->next;
	}
	if (0 == flag)
	{
		return ILLEGAL_OPERATION;
	}

	DListNode *pBehindNode = pFrontNode->next;
	pFrontNode->next = pInsertNode;
	pInsertNode->pre = pFrontNode;
	pBehindNode->pre = pInsertNode;
	pInsertNode->next = pBehindNode;

	return SUCCESS;
}

int dlist_insert_node_behind_by_index(DList *pList, DListNode *pInsertNode, int index)
{
	if (NULL == pList || NULL == pInsertNode)
	{
		return NULL_POINTER;
	}
	if (index < 0)
	{
		return OUT_OF_RANGE;
	}

	DListNode *pTravelNode = pList->first;
	int i = 0;
	//利用pTravelNode进行遍历
	while (pTravelNode != pList->last)
	{
		if (i == index)
		{
			//将pInsertNode插入到pTravelNode和pNextNode之间
			DListNode *pNextNode = pTravelNode->next;
			pTravelNode->next = pInsertNode;

			pInsertNode->pre = pTravelNode;
			pInsertNode->next = pNextNode;

			pNextNode->pre = pInsertNode;
			return SUCCESS;
		}
		i++;
		pTravelNode = pTravelNode->next;
	}

	return OUT_OF_RANGE;
}

int dlist_appened_node(DList *pDstList, void *data)
{
	if (NULL == pDstList || NULL == data)
	{
		return NULL_POINTER;
	}

	DListNode *pNode = NULL;
	pNode = dlist_creat_node(data);
	if (NULL == pNode)
	{
		return CREAT_NODE_FAILED;
	}

	DListNode *pFrontNode = pDstList->last->pre;
	pFrontNode->next = pNode;
	pNode->pre = pFrontNode;

	pNode->next = pDstList->last;
	pDstList->last->pre = pNode;

	return SUCCESS;
}

DListNode *dlist_get_node_by_index(DList *pList, int index)
{
	if (NULL == pList || index < 1)
	{
		return NULL;
	}

	DListNode *pTmpNode = pList->first->next;

	int i = 1;
	while (pTmpNode != pList->last)
	{
		if (i == index)
		{
			return pTmpNode;
		}
		i++;
		pTmpNode = pTmpNode->next;
	}

	return NULL;
}

DListNode *dlist_get_node_by_data(DList *pList, const void *data, fp_compare cmp)
{
	if (NULL == pList || NULL == cmp)
	{
		return NULL;
	}
	DListNode *pTmpNode = pList->first->next;
	while (pList->last != pTmpNode)
	{
		//若根据使用者的函数判断数据相同，则返回结果
		if (0 == cmp(pTmpNode->data, data))
		{
			return pTmpNode;
		}

		pTmpNode = pTmpNode->next;
	}
	return NULL;
}

//从pFromNode开始，查询数据域与data相等的节点
DListNode *dlist_get_node_by_data_from(DListNode *pFromNode, const void *data, fp_compare cmp)
{
	if (NULL == cmp)
	{
		return NULL;
	}
	DListNode *pTmpNode = pFromNode;
	while (NULL != pTmpNode)
	{
		if (NULL != pTmpNode->data && 0 == cmp(pTmpNode->data, data))
		{
			return pTmpNode;
		}

		pTmpNode = pTmpNode->next;
	}
	return NULL;
}

void dlist_print(DList *pList, fp_print print_data)
{
	if (NULL == pList || NULL == print_data)
	{
		return;
	}

	DListNode *pTmpNode = pList->first->next;
	while (pTmpNode != pList->last)
	{
		print_data(pTmpNode->data);
		pTmpNode = pTmpNode->next;
	}
}

void dlist_insert_sort(DList *pList, fp_compare cmp, int flag)
{
	//判断空指针和没有数据节点的情况
	if (NULL == pList || NULL == cmp)
	{
		return;
	}

	//pEdge表示已经有序的部分的边界，pEdge及其之前的部分均是有序的
	DListNode *pEdge = pList->first->next;

	//pInsertNode表示待插入的节点
	DListNode *pInsertNode = pEdge->next;

	//pTravelNode用于在有序的部分进行遍历，找到pInsertNode的插入位置
	DListNode *pTravelNode = NULL;

	//升序排列的情况
	if (flag >= 0)
	{
		while (pInsertNode != pList->last)
		{
			//保存pInsertNode下一个节点的入口
			DListNode *pNextInsertNode = pInsertNode->next;

			//pTravelNode从pEdge开始遍历，找到第一个比pInsertNode小（或相等）的节点
			pTravelNode = pEdge;
			while (pTravelNode != pList->first && 0 > cmp(pInsertNode->data, pTravelNode->data))
			{
				pTravelNode = pTravelNode->pre;
			}

			//从链表上摘除pInsertNode
			dlist_remove_node_from_list(pList, pInsertNode);

			//将pInsertNode插入到pTravelNode之后
			dlist_insert_node_behind_by_ptr(pList, pTravelNode, pInsertNode);

			//如果恰好是插入到pEdge后，则pEdge需要更新，pInsertNode成为新的有序边界
			if (pEdge == pTravelNode)
			{
				pEdge = pInsertNode;
			}

			//准备进行下一轮插入
			pInsertNode = pNextInsertNode;
		}
		//最后需要更新pList的尾指针为有序边界
	}
	//降序排列的情况
	else
	{
		while (pInsertNode != pList->last)
		{
			DListNode *pNextInsertNode = pInsertNode->next;
			pEdge->next = pNextInsertNode;
			// if(pNextInsertNode != NULL)
			// {
			// 	pNextInsertNode->pre = pEdge;
			// }

			//pTravelNode从pEdge开始遍历，找到第一个比pInsertNode大（或相等）的节点
			pTravelNode = pEdge;
			while (pTravelNode != pList->first && 0 < cmp(pInsertNode->data, pTravelNode->data))
			{
				pTravelNode = pTravelNode->pre;
			}

			dlist_remove_node_from_list(pList, pInsertNode);
			dlist_insert_node_behind_by_ptr(pList, pTravelNode, pInsertNode);

			if (pEdge == pTravelNode)
			{
				pEdge = pInsertNode;
			}

			pInsertNode = pNextInsertNode;
		}
	}
}

/*
*@fn				void dlist_merge_by_order(DListNode *pLeftHead, DListNode *pRightHead, fp_compare cmp, int flag);
*@brief   			归并排序用到的内部工具函数，用于将两个已经局部有序的链表合并成一个有序的链表，全部合并到pLeftHead所处的链表上
*@details			
*
*@param[in] DListNode* pLeftHead			待合并的有序链表1的头结点指针
*@param[in]	DListNode* pRightHead		待合并的有序链表2的头结点指针
*@param[in] fp_compare cmp				使用者需要提供的对数据进行比较的函数
*@param[in] int flag					flag>=0，升序归并；flag<0，降序归并。必须与归并排序的flag一致，否则会出错
*
*@return			N/A
*@retval			
*/
static void dlist_merge_by_order(DListNode *pLeftHead, DListNode *pRightHead, fp_compare cmp, int flag)
{
	if (NULL == pLeftHead || NULL == pRightHead || NULL == cmp)
	{
		return;
	}
	//跳过附加头节点
	DListNode *pLeft = pLeftHead->next;
	DListNode *pRight = pRightHead->next;

	//将两边的附加头节点摘下来
	DListNode *pNowNode = pLeftHead;
	pLeftHead->next = NULL;
	pLeft->pre = NULL;
	pRightHead->next = NULL;
	pRight->pre = NULL;

	//升序归并
	if (flag >= 0)
	{
		while (NULL != pLeft && NULL != pRight)
		{
			//选两者中较大的添加到pLeftHead所处链表的结尾
			if (cmp(pLeft->data, pRight->data) <= 0)
			{
				pNowNode->next = pLeft;
				pLeft->pre = pNowNode;

				pNowNode = pNowNode->next;
				pLeft = pLeft->next;
				pNowNode->next = NULL;
			}
			else
			{
				pNowNode->next = pRight;
				pRight->pre = pNowNode;

				pNowNode = pNowNode->next;
				pRight = pRight->next;
				pNowNode->next = NULL;
			}
		}

		//将剩余的节点全部添加到pLeftHead所处链表的末尾
		if (NULL == pLeft && NULL != pRight)
		{
			pNowNode->next = pRight;
		}
		else if (NULL != pLeft && NULL == pRight)
		{
			pNowNode->next = pLeft;
		}
		return;
	}

	//降序归并
	else
	{
		while (NULL != pLeft && NULL != pRight)
		{
			if (cmp(pLeft->data, pRight->data) >= 0)
			{
				pNowNode->next = pLeft;
				pLeft->pre = pNowNode;

				pNowNode = pNowNode->next;
				pLeft = pLeft->next;
				pNowNode->next = NULL;
			}
			else
			{
				pNowNode->next = pRight;
				pRight->pre = pNowNode;

				pNowNode = pNowNode->next;
				pRight = pRight->next;
				pNowNode->next = NULL;
			}
		}
		if (NULL == pLeft && NULL != pRight)
		{
			pNowNode->next = pRight;
		}
		else if (NULL != pLeft && NULL == pRight)
		{
			pNowNode->next = pLeft;
		}
		return;
	}
}

/*
*@fn				void dlist_merge_sort_r(DListNode *pHead, fp_compare cmp, int flag);
*@brief   			归并排序用到的内部工具函数，用于进行递归排序
*@details			
*
*@param[in] DListNode* pHead				待排序的无序链表的头结点指针
*@param[in] fp_compare cmp				使用者需要提供的对数据进行比较的函数
*@param[in] int flag					flag>=0，升序排序；flag<0，降序排序。必须与归并排序的flag一致，否则会出错
*
*@return			N/A
*@retval			
*/
static void dlist_merge_sort_r(DListNode *pHead, fp_compare cmp, int flag)
{
	if (NULL == pHead || NULL == cmp)
	{
		return;
	}
	//递归出口，当传入的链表只剩下一个附加头节点和一个数据节点时，递归结束
	if (pHead->next->next == NULL)
	{
		return;
	}

	DListNode *pLeft = pHead->next;
	DListNode *pRight = pLeft;
	DListNode *pEnd = pLeft;
	DListNode *pRightHead = NULL;
	DListNode *pRightFront = pHead;

	//找到中间位置，pEnd一次走两步，pRight一次走一步，pEnd到达结尾处时，pRight到达中间位置，pRight到达中间位置的前一个位置
	while (pEnd != NULL && pEnd->next != NULL)
	{
		pEnd = pEnd->next->next;
		pRight = pRight->next;
		pRightFront = pRightFront->next;
	}

	//自pRightFront后斩断链表为两部分
	pRightFront->next = NULL;
	//DListNode *pMidNode = pRight->pre;
	//pMidNode->next = NULL;

	//为后半部分添加附加头节点
	pRightHead = (DListNode *)malloc(sizeof(DListNode));

	//如果申请不到空间，排序将无法进行
	if (NULL == pRightHead)
	{
		pRightFront->next = pRight;
		printf("----------------------------------Cause of memory problem, sort failed!-------------------------------------\n");
		return;
	}

	//为后半部分添加附加头节点
	pRightHead->next = pRight;
	pRight->pre = pRightHead;

	//递归对左半部分进行排序
	dlist_merge_sort_r(pHead, cmp, flag);
	//递归对右半部分进行排序
	dlist_merge_sort_r(pRightHead, cmp, flag);

	//合并两部分
	dlist_merge_by_order(pHead, pRightHead, cmp, flag);

	//释放动态分配的右边附加头节点
	free(pRightHead);
}

void dlist_merge_sort(DList *pList, fp_compare cmp, int flag)
{
	if (NULL == pList || NULL == cmp || pList->first->next == pList->last)
	{
		return;
	}

	DListNode *tail = pList->last;
	DListNode *front = tail->pre;
	front->next = NULL;
	tail->pre = NULL;
	pList->last = front;

	dlist_merge_sort_r(pList->first, cmp, flag);
	DListNode *pTravel = pList->first;
	while (pTravel->next != NULL)
	{
		pTravel = pTravel->next;
	}
	pTravel->next = tail;
	pList->last = tail;
	tail->pre = pTravel;
}

void dlist_merge(DList *pDst, DList *pSrc)
{
	if (NULL == pDst || NULL == pSrc || pSrc->first->next == pSrc->last)
	{
		return;
	}
	// DListNode *leftEnd = pDst->last;
	DListNode *rightStart = pSrc->first->next;

	rightStart->pre = NULL;
	pSrc->first->next = NULL;

	DListNode *pDstLast = pDst->last;
	DListNode *pLeftEnd = pDstLast->pre;

	pLeftEnd->next = rightStart;
	rightStart->pre = pLeftEnd;

	pDst->last = pSrc->last;

	pSrc->last = pDstLast;
	pSrc->first->next = pSrc->last;
	pSrc->last->pre = pSrc->first;
}

void *dlist_get_data_from_node(DListNode *pNode)
{
	if (NULL != pNode)
	{
		return pNode->data;
	}
	else
	{
		return NULL;
	}
}

int dlist_get_data_count(DList *pList)
{
	if (NULL == pList)
	{
		return NULL_POINTER;
	}

	DListNode *pTravelNode = pList->first->next;
	int count = 0;

	while (pTravelNode != pList->last)
	{
		count++;
		pTravelNode = pTravelNode->next;
	}
	return count;
}
