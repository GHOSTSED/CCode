#ifndef _DMONITOR_H_
#define _DMONITOR_H_

/*************************************************************************************************************************************/
/*												  INCLUDE_FILES																		 */
/*************************************************************************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <error.h>
#include "./DList.h"

/*************************************************************************************************************************************/
/*												    DEFINES																		 	 */
/*************************************************************************************************************************************/
#define DEFAULT_FIFO_PATH "../pipe/myFifo"
#define MAX_LENGTH 50
#define INIT_SUCCESS 1
#define INIT_FAILED -1

/*************************************************************************************************************************************/
/*												     TYPES																		     */
/*************************************************************************************************************************************/
typedef struct _SubProcess
{
    int pid;
    char path[MAX_LENGTH];
    char isExit;
} SubProcess;

typedef struct _DMonitor
{
    DList *subProcessList;
    char fifoPath[MAX_LENGTH];
} DMonitor;

/*************************************************************************************************************************************/
/*												   FUNCTIONS																		 */
/*************************************************************************************************************************************/
int dmonitor_subProcess_init(const char *exePath)
{
    if (0 == access(exePath, F_OK | X_OK))
    {
        return INIT_SUCCESS;
    }
    else
    {
        return INIT_FAILED;
    }
}

void dmonitor_print_subProcess(void *vSubProcess)
{
    SubProcess *pSubProcess = (SubProcess *)vSubProcess;
    printf("PID:%d\nPath:%s\n", pSubProcess->pid, pSubProcess->path);
}

SubProcess *dmonitor_create_subProcess_info(const char *exePath)
{
    SubProcess *pSubProcess = (SubProcess *)malloc(sizeof(SubProcess));
    if (NULL == pSubProcess)
    {
        return NULL;
    }

    pSubProcess->isExit = 0x00;
    strncpy(pSubProcess->path, exePath, MAX_LENGTH);
    pSubProcess->pid = -10;

    return pSubProcess;
}

DMonitor *dmonitor_init(const char *exeOrderPath, const char *fifoPath)
{
    FILE *inputFile = NULL;
    int paramentCount;
    char exePath[MAX_LENGTH];
    SubProcess *pSubprocess = NULL;
    int op_result;
    DMonitor *pMonitor = NULL;

    pMonitor = (DMonitor *)malloc(sizeof(DMonitor));
    if (NULL == pMonitor)
    {
        return NULL;
    }

    DList *pList = dlist_init(NULL, NULL);
    if (NULL == pList)
    {
        free(pMonitor);
        return NULL;
    }

    if (NULL == exeOrderPath)
    {
        inputFile = fopen("../InputFile/execOrder.txt", "r");
    }
    else
    {
        inputFile = fopen(exeOrderPath, "r");
    }

    if (NULL == inputFile)
    {
        free(pMonitor);
        dlist_delete(pList);
        return NULL;
    }

    while ((paramentCount = fscanf(inputFile, "%s\n", exePath)) && 1 == paramentCount)
    {
        pSubprocess = dmonitor_create_subProcess_info(exePath);
        if (NULL == pSubprocess)
        {
            free(pMonitor);
            dlist_delete(pList);
            return NULL;
        }

        op_result = dlist_appened_node(pList, pSubprocess);
        if (op_result < 0)
        {
            free(pMonitor);
            dlist_delete(pList);
            free(pSubprocess);
            return NULL;
        }

        memset(exePath, 0, MAX_LENGTH);
    }

    pMonitor->subProcessList = pList;
    if (NULL == fifoPath || 0 == strlen(fifoPath))
    {
        memset(pMonitor->fifoPath, 0, MAX_LENGTH);
    }
    else
    {
        strncpy(pMonitor->fifoPath, fifoPath, MAX_LENGTH);
    }

    return pMonitor;
}

void dmonitor_delete(DMonitor *pMonitor)
{
    if (NULL == pMonitor)
    {
        return;
    }

    dlist_delete(pMonitor->subProcessList);
    free(pMonitor);
}

void dmonitor_run(DMonitor *pMonitor)
{
    if (NULL == pMonitor)
    {
        printf("don't input NULL pointer!\n");
        return;
    }

    int fifoRet, fifoFd;
    char *fifoPath;
    char fifoBuf[MAX_LENGTH];

    if (0 == pMonitor->fifoPath[0])
    {
        fifoPath = DEFAULT_FIFO_PATH;
    }
    else
    {
        fifoPath = pMonitor->fifoPath;
    }

    if (-1 == access(fifoPath, F_OK))
    {
        fifoRet = mkfifo(fifoPath, 0777);
        if (-1 == fifoRet)
        {
            printf("create FIFO failed!\n");
            return;
        }
    }

    DList *pList = pMonitor->subProcessList;
    DListNode *pNode = pList->first->next;
    int subPid;

    while (pNode != pList->last)
    {
        SubProcess *pSubProcess = (SubProcess *)(pNode->data);
        subPid = fork();
        if (subPid < 0)
        {
            printf("create subProcess failed!\n");
            break;
        }
        else if (0 == subPid)
        {
            int ret = dmonitor_subProcess_init(pSubProcess->path);
            fifoFd = open(fifoPath, O_WRONLY);
            if (-1 == fifoFd)
            {
                printf("subProcess %d open fifo failed!\n", getpid());
                break;
            }
            /* 子进程初始化成功的情况下，子进程的处理 */
            if (ret > 0)
            {
                strncpy(fifoBuf, "init_ok\0", MAX_LENGTH);
                write(fifoFd, fifoBuf, MAX_LENGTH);
                break;
            }
            /* 子进程初始化失败的情况，子进程的处理 */
            else
            {
                strncpy(fifoBuf, "init_failed\0", MAX_LENGTH);
                write(fifoFd, fifoBuf, MAX_LENGTH);
                break;
            }
        }
        else
        {
            fifoFd = open(fifoPath, O_RDONLY);
            if(-1 == fifoFd)
            {
                printf("parent process open fifo failed!\n", getpid());
                return;
            }

            read(fifoFd, fifoBuf, MAX_LENGTH);

            if(0 == strcmp(fifoBuf, "init_ok"))
            {
                pSubProcess->pid = subPid;
            }
            else
            {
                printf("subProcess %d init failed!\n", subPid);
                break;
            }
        }
        
    }

    /* 是子进程的情况 */
    if(0 == subPid)
    {

    }
    /* 是父进程的情况 */
    else
    {
        
    }
    

}

#endif