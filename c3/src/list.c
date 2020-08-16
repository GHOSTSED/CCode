#include "../include/list.h"

struct listNode{
	void *data;
	struct listNode *next;
	struct listNode *pre;
};


struct list{
	struct listNode *first;//头节点
	struct listNode *last;//尾节点
	void *lock;//链表锁
};

void lockList(list *pList, fp_lock_list lockIt)
{
	if(NULL != lockIt)
	{
		lockIt(pList->lock);
	}
}

void unlockList(list *pList, fp_unlock_list unlockIt)
{
	if(NULL != unlockIt)
	{
		unlockIt(pList->lock);
	}
}

list *init_list(void *mylock)
{
	list *pList = NULL;
	listNode *pHead = NULL;
	listNode *pTail = NULL;
	// locker *pLocker = NULL;

	// if ((NULL == mylock && NULL == lockList && NULL == unlockList) || (NULL != mylock && NULL != lockList && NULL != unlockList))
	// {
		pList = (list *)malloc(sizeof(list));
		if (NULL == pList)
		{
			return NULL;
		}

		pHead = (listNode *)malloc(sizeof(listNode));
		if (NULL == pHead)
		{
			free(pList);
			return NULL;
		}

		pTail = (listNode *)malloc(sizeof(listNode));
		if (NULL == pTail)
		{
			free(pHead);
			free(pList);
			return NULL;
		}

		// pLocker = (locker *)malloc(sizeof(locker));
		// if (NULL == pLocker)
		// {
		// 	free(pHead);
		// 	free(pTail);
		// 	free(pList);
		// 	return NULL;
		// }

		pHead->pre = NULL;
		pHead->next = pTail;
		pHead->data = NULL;

		pTail->pre = pHead;
		pTail->next = NULL;
		pTail->data = NULL;

		pList->first = pHead;
		pList->last = pTail;

		pList->lock = mylock;
		// pList->listLocker->lockIt = lockList;
		// pList->listLocker->unlockIt = unlockList;
		return pList;
	// }
	// else
	// {
	// 	return NULL;
	// }
	
}

listNode *creat_node(void *data)
{
	listNode *pNewNode = NULL;

	if (NULL == data)
	{
		return NULL;
	}

	pNewNode = (listNode *)malloc(sizeof(listNode));
	if (NULL == pNewNode)
	{
		return NULL;
	}

	pNewNode->data = data;

	pNewNode->pre = NULL;
	pNewNode->next = NULL;

	return pNewNode;
}

void delete_node(listNode *pNode)
{
	if (NULL == pNode)
	{
		return;
	}

	if (NULL != pNode->data)
	{
		free(pNode->data); //由内而外释放内存，先释放结构体内部的指针的内存空间
	}

	free(pNode); //释放节点的内存空间
}

void delete_list(list *pList)
{
	if (NULL == pList)
	{
		return;
	}

	listNode *pNode = NULL;
	//逐个删除并更新链表的头节点，直至完全删除
	while (NULL != pList->first)
	{
		pNode = pList->first;
		pList->first = pNode->next;

		delete_node(pNode);
	}
	free(pList->lock);
	// free(pList->listLocker);
	free(pList);
}

int delete_node_from_list(list *pList, listNode *pNode)
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
		listNode *pTmpNode = pList->first->next;
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

	listNode *preNode = pNode->pre;
	listNode *nextNode = pNode->next;

	preNode->next = nextNode;
	nextNode->pre = preNode;

	//更新数据节点数量
	delete_node(pNode);
	return SUCCESS;
}

//内部工具函数，用于将pNode从pList上摘除，但不delete掉pNode
static void remove_node_from_list(list *pList, listNode *pNode)
{
	if (NULL == pList || NULL == pNode)
	{
		return;
	}
	if (pNode == pList->first || pNode == pList->last)
	{
		return;
	}

	listNode *preNode = pNode->pre;
	listNode *nextNode = pNode->next;

	preNode->next = nextNode;
	nextNode->pre = preNode;
}

int delete_node_by_data(list *pList, const void *data, fp_compare cmp)
{
	if (NULL == cmp || NULL == pList)
	{
		return NULL_POINTER;
	}
	listNode *pRemoveNode = get_node_by_data(pList, data, cmp);
	listNode *tmp = NULL;

	if (NULL == pRemoveNode)
	{
		return NODE_NOT_EXIST;
	}

	int count = 0;

	do
	{
		++count;
		tmp = pRemoveNode->next;
		delete_node_from_list(pList, pRemoveNode);
		pRemoveNode = tmp;
		pRemoveNode = get_node_by_data_from(pRemoveNode, data, cmp);
	} while (pRemoveNode != NULL && pRemoveNode != pList->last);

	return count;
}

int delete_node_by_index(list *pList, int index)
{
	if (NULL == pList)
	{
		return NULL_POINTER;
	}

	if (index < 1)
	{
		return OUT_OF_RANGE;
	}

	listNode *pTmpNode = get_node_by_index(pList, index);
	if (NULL == pTmpNode)
	{
		return NULL_POINTER;
	}

	delete_node_from_list(pList, pTmpNode);
	return SUCCESS;
}

int insert_node_behind_by_ptr(list *pList, listNode *pFrontNode, listNode *pInsertNode)
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
	listNode *pTravelNode = pList->first;
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

	listNode *pBehindNode = pFrontNode->next;
	pFrontNode->next = pInsertNode;
	pInsertNode->pre = pFrontNode;
	pBehindNode->pre = pInsertNode;
	pInsertNode->next = pBehindNode;

	return SUCCESS;
}

int insert_node_behind_by_index(list *pList, listNode *pInsertNode, int index)
{
	if (NULL == pList || NULL == pInsertNode)
	{
		return NULL_POINTER;
	}
	if (index < 0)
	{
		return OUT_OF_RANGE;
	}

	listNode *pTravelNode = pList->first;
	int i = 0;
	//利用pTravelNode进行遍历
	while (pTravelNode != pList->last)
	{
		if (i == index)
		{
			//将pInsertNode插入到pTravelNode和pNextNode之间
			listNode *pNextNode = pTravelNode->next;
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

int appened_node_from_data(list *pDstList, void *data)
{
	if (NULL == pDstList || NULL == data)
	{
		return NULL_POINTER;
	}

	listNode *pNode = NULL;
	pNode = creat_node(data);
	if (NULL == pNode)
	{
		return CREAT_NODE_FAILED;
	}

	listNode *pFrontNode = pDstList->last->pre;
	pFrontNode->next = pNode;
	pNode->pre = pFrontNode;

	pNode->next = pDstList->last;
	pDstList->last->pre = pNode;

	return SUCCESS;
}

listNode *get_node_by_index(list *pList, int index)
{
	if (NULL == pList || index < 1)
	{
		return NULL;
	}

	listNode *pTmpNode = pList->first->next;

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

listNode *get_node_by_data(list *pList, const void *data, fp_compare cmp)
{
	if (NULL == pList || NULL == cmp)
	{
		return NULL;
	}
	listNode *pTmpNode = pList->first->next;
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
listNode *get_node_by_data_from(listNode *pFromNode, const void *data, fp_compare cmp)
{
	if (NULL == cmp)
	{
		return NULL;
	}
	listNode *pTmpNode = pFromNode;
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

void printList(list *pList, fp_print print_data)
{
	if (NULL == pList || NULL == print_data)
	{
		return;
	}

	listNode *pTmpNode = pList->first->next;
	while (pTmpNode != pList->last)
	{
		print_data(pTmpNode->data);
		// printf("\n");
		pTmpNode = pTmpNode->next;
	}
	printf("\n");
}

void insert_sort(list *pList, fp_compare cmp, int flag)
{
	//判断空指针和没有数据节点的情况
	if (NULL == pList || NULL == cmp)
	{
		return;
	}

	//pEdge表示已经有序的部分的边界，pEdge及其之前的部分均是有序的
	listNode *pEdge = pList->first->next;

	//pInsertNode表示待插入的节点
	listNode *pInsertNode = pEdge->next;

	//pTravelNode用于在有序的部分进行遍历，找到pInsertNode的插入位置
	listNode *pTravelNode = NULL;

	//升序排列的情况
	if (flag >= 0)
	{
		while (pInsertNode != pList->last)
		{
			//保存pInsertNode下一个节点的入口
			listNode *pNextInsertNode = pInsertNode->next;
			// pEdge->next = pNextInsertNode;
			// if(NULL != pNextInsertNode)
			// {
			// 	pNextInsertNode->pre = pEdge;
			// }

			//pTravelNode从pEdge开始遍历，找到第一个比pInsertNode小（或相等）的节点
			pTravelNode = pEdge;
			while (pTravelNode != pList->first && 0 > cmp(pInsertNode->data, pTravelNode->data))
			{
				pTravelNode = pTravelNode->pre;
			}

			//从链表上摘除pInsertNode
			remove_node_from_list(pList, pInsertNode);

			//将pInsertNode插入到pTravelNode之后
			insert_node_behind_by_ptr(pList, pTravelNode, pInsertNode);

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
			listNode *pNextInsertNode = pInsertNode->next;
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

			remove_node_from_list(pList, pInsertNode);
			insert_node_behind_by_ptr(pList, pTravelNode, pInsertNode);

			if (pEdge == pTravelNode)
			{
				pEdge = pInsertNode;
			}

			pInsertNode = pNextInsertNode;
		}
	}
}

/*
*@fn				void merge_by_order(listNode *pLeftHead, listNode *pRightHead, fp_compare cmp, int flag);
*@brief   			归并排序用到的内部工具函数，用于将两个已经局部有序的链表合并成一个有序的链表，全部合并到pLeftHead所处的链表上
*@details			
*
*@param[in] listNode* pLeftHead			待合并的有序链表1的头结点指针
*@param[in]	listNode* pRightHead		待合并的有序链表2的头结点指针
*@param[in] fp_compare cmp				使用者需要提供的对数据进行比较的函数
*@param[in] int flag					flag>=0，升序归并；flag<0，降序归并。必须与归并排序的flag一致，否则会出错
*
*@return			N/A
*@retval			
*/
static void merge_by_order(listNode *pLeftHead, listNode *pRightHead, fp_compare cmp, int flag)
{
	if (NULL == pLeftHead || NULL == pRightHead || NULL == cmp)
	{
		return;
	}
	//跳过附加头节点
	listNode *pLeft = pLeftHead->next;
	listNode *pRight = pRightHead->next;

	//将两边的附加头节点摘下来
	listNode *pNowNode = pLeftHead;
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
*@fn				void merge_sort_r(listNode *pHead, fp_compare cmp, int flag);
*@brief   			归并排序用到的内部工具函数，用于进行递归排序
*@details			
*
*@param[in] listNode* pHead				待排序的无序链表的头结点指针
*@param[in] fp_compare cmp				使用者需要提供的对数据进行比较的函数
*@param[in] int flag					flag>=0，升序排序；flag<0，降序排序。必须与归并排序的flag一致，否则会出错
*
*@return			N/A
*@retval			
*/
static void merge_sort_r(listNode *pHead, fp_compare cmp, int flag)
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

	listNode *pLeft = pHead->next;
	listNode *pRight = pLeft;
	listNode *pEnd = pLeft;
	listNode *pRightHead = NULL;
	listNode *pRightFront = pHead;

	//找到中间位置，pEnd一次走两步，pRight一次走一步，pEnd到达结尾处时，pRight到达中间位置，pRight到达中间位置的前一个位置
	while (pEnd != NULL && pEnd->next != NULL)
	{
		pEnd = pEnd->next->next;
		pRight = pRight->next;
		pRightFront = pRightFront->next;
	}

	//自pRightFront后斩断链表为两部分
	pRightFront->next = NULL;
	//listNode *pMidNode = pRight->pre;
	//pMidNode->next = NULL;

	//为后半部分添加附加头节点
	pRightHead = (listNode *)malloc(sizeof(listNode));

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
	merge_sort_r(pHead, cmp, flag);
	//递归对右半部分进行排序
	merge_sort_r(pRightHead, cmp, flag);

	//合并两部分
	merge_by_order(pHead, pRightHead, cmp, flag);

	//释放动态分配的右边附加头节点
	free(pRightHead);
}

void merge_sort(list *pList, fp_compare cmp, int flag)
{
	if (NULL == pList || NULL == cmp || pList->first->next == pList->last)
	{
		return;
	}

	listNode *tail = pList->last;
	listNode *front = tail->pre;
	front->next = NULL;
	tail->pre = NULL;
	pList->last = front;

	merge_sort_r(pList->first, cmp, flag);
	listNode *pTravel = pList->first;
	while (pTravel->next != NULL)
	{
		pTravel = pTravel->next;
	}
	pTravel->next = tail;
	pList->last = tail;
	tail->pre = pTravel;
}

void merge_list(list *pDst, list *pSrc)
{
	if (NULL == pDst || NULL == pSrc || pSrc->first->next == pSrc->last)
	{
		return;
	}
	// listNode *leftEnd = pDst->last;
	listNode *rightStart = pSrc->first->next;

	rightStart->pre = NULL;
	pSrc->first->next = NULL;

	listNode *pDstLast = pDst->last;
	listNode *pLeftEnd = pDstLast->pre;

	pLeftEnd->next = rightStart;
	rightStart->pre = pLeftEnd;

	pDst->last = pSrc->last;

	pSrc->last = pDstLast;
	pSrc->first->next = pSrc->last;
	pSrc->last->pre = pSrc->first;

	// pDst->last->next = rightStart;
	// rightStart->pre = pDst->last;

	// pDst->last = pSrc->last;
	// pSrc->last = pSrc->first;
	// pSrc->count = 0;
}

void *get_data_from_node(listNode *pNode)
{
	if(NULL != pNode)
	{
		return pNode->data;
	}
	else
	{
		return NULL;
	}
	
}

int get_data_count(list *pList)
{
	if(NULL == pList)
	{
		return NULL_POINTER;
	}

	listNode *pTravelNode = pList->first->next;
	int count = 0;


	while (pTravelNode != pList->last)
	{
		count++;
		pTravelNode = pTravelNode->next;
	}
	return count;
}
