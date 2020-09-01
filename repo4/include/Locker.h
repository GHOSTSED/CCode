#ifndef _LOCKER_H_
#define _LOCKER_H_

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>


typedef struct _Locker Locker;
typedef void(*fp_lock)(void *lock);//用于上锁的函数
typedef void(*fp_unlock)(void *lock);//用于解锁的函数
typedef int(*fp_get_current_threadID)();

Locker *locker_init(void *lock, fp_lock lockIt, fp_unlock unlockIt, fp_get_current_threadID getID);

void locker_delete(Locker *pLocker);

void locker_lockIt(Locker *pLocker);

void locker_unlockIt(Locker *pLocker);

#endif