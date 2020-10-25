#include "../include/Locker.h"

struct _Locker
{
	void *lock;
	int useCount;
	fp_lock lockIt;
	fp_unlock unlockIt;
    int ownerID;
    fp_get_current_threadID getID;
};

Locker *locker_init(void *lock, fp_lock lockIt, fp_unlock unlockIt, fp_get_current_threadID getID)
{
	if(NULL == lock || NULL == lockIt || NULL == unlockIt || NULL == getID)
	{
		return NULL;
	}

	Locker *pLocker = (Locker *)malloc(sizeof(Locker));
	if(NULL == pLocker)
	{
		return NULL;
	}

	pLocker->lock = lock;
	pLocker->lockIt = lockIt;
	pLocker->unlockIt = unlockIt;
	pLocker->useCount = 0;
    pLocker->ownerID = -1;
    pLocker->getID = getID;

	return pLocker;
}

void locker_delete(Locker *pLocker)
{
	if(NULL == pLocker)
	{
		return;
	}

	free(pLocker);
	pLocker = NULL;
}

void locker_lockIt(Locker *pLocker)
{
	if(NULL == pLocker)
	{
		return;
	}

	if(pLocker->getID() == pLocker->ownerID)
	{
		pLocker->useCount++;
	}
	else
	{
		pLocker->lockIt(pLocker->lock);
		pLocker->ownerID = pLocker->getID();
		pLocker->useCount++;
	}
	
}

void locker_unlockIt(Locker *pLocker)
{
	if(NULL == pLocker)
	{
		return;
	}

	if(pLocker->getID() == pLocker->ownerID)
	{
		pLocker->useCount--;
		if(0 == pLocker->useCount)
		{
			pLocker->unlockIt(pLocker->lock);
		}
	}
	
}