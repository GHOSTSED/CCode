/*************************************************************************************************************************************/
/*												  INCLUDE_FILES																		 */
/*************************************************************************************************************************************/
#include "../include/DMonitor.h"

/*************************************************************************************************************************************/
/*												     TYPES																		     */
/*************************************************************************************************************************************/
/* 存储要创建的子进程相关信息的数据结构 */
struct _SubProcess
{
    /* 子进程的PID */
    int pid;
    /* 子进程要执行的可执行程序的路径 */
    char path[MAX_LENGTH];
};

/* 用链表形式对子进程相关信息进行管理的数据结构 */
struct _DMonitor
{
    /* 存储子进程相关信息的链表 */
    DList *subProcessList;
    /* 存储有名管道路径的数组 */
    char fifoPath[MAX_LENGTH];
};

/*************************************************************************************************************************************/
/*												   FUNCTIONS																		 */
/*************************************************************************************************************************************/
/*
*@fn				                        static int dmonitor_find_negative_pid(void *vSubProcess, void *any);
*@brief   			                        内部工具函数，用于为通用链表提供回调函数，筛选出链表中pid为负数的子进程，以便将其删除
*@details	                                子进程初始化时pid为-10，pid为负数的子进程也就是没有成功启动的子进程		
*
*@param[in] const void* vSubProcess	        要进行判断的子进程指针
*@param[in] const void* any                 传入任何值均可，为了满足通用链表回调函数格式而添加的变量
*
*@return			                        判断结果，若vSubProcess的pid小于0，返回0；否则，返回-1
*@retval			
*/
static int dmonitor_find_negative_pid(const void *vSubProcess, const void *any);

/*
*@fn				                        static int dmonitor_find_subProcess_by_pid(const void *vSubProcess, const void *vPid);
*@brief   			                        内部工具函数，用于为通用链表提供回调函数，在链表中查找指定pid的子进程
*@details	                                	
*
*@param[in] const void* vSubProcess	        要进行判断的子进程指针
*@param[in] const void* vPid                需要查找的pid
*
*@return			                        判断结果，若vSubProcess的pid与vPid的值相等，返回0；否则，返回-1
*@retval			
*/
static int dmonitor_find_subProcess_by_pid(const void *vSubProcess, const void *vPid);

/*
*@fn				                        static int dmonitor_subProcess_init(const char *exePath);
*@brief   			                        内部工具函数，题目要求的子进程初始化函数
*@details	                                主要负责判断exePath路径的文件是否存在且可执行                       	
*
*@param[in] const char* exePath	            子进程对应的可执行文件的路径
*
*@return			                        判断结果，若路径下的文件存在且可执行，返回1；否则，返回-1
*@retval			
*/
static int dmonitor_subProcess_init(const char *exePath);

/*
*@fn				                        static SubProcess *dmonitor_create_subProcess_info(const char *exePath);
*@brief   			                        内部工具函数，用于创建承载子进程相关信息的结构体
*@details	                                创建出的子进程信息结构体的pid为-10                                              	
*
*@param[in] const char* exePath	            要创建的子进程对应的可执行文件的路径
*
*@return			                        倘若创建成功，返回创建的子进程信息结构体的指针；否则，返回NULL
*@retval			
*/
static SubProcess *dmonitor_create_subProcess_info(const char *exePath);


static int dmonitor_find_negative_pid(const void *vSubProcess, const void *any)
{
    if(NULL == vSubProcess)
    {
        return -1;
    }

    SubProcess *pSubProcess = (SubProcess *)vSubProcess;

    if (pSubProcess->pid < 0)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

static int dmonitor_find_subProcess_by_pid(const void *vSubProcess, const void *vPid)
{
    if(NULL == vSubProcess || NULL == vPid)
    {
        return -1;
    }

    SubProcess *pSubProcess = (SubProcess *)vSubProcess;
    int pid = *(int *)vPid;

    if (pSubProcess->pid == pid)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

static int dmonitor_subProcess_init(const char *exePath)
{
    if(NULL == exePath)
    {
        return NULL_POINTER;
    }
    
    if (0 == access(exePath, F_OK | X_OK))
    {
        return INIT_SUCCESS;
    }
    else
    {
        return INIT_FAILED;
    }
}

static SubProcess *dmonitor_create_subProcess_info(const char *exePath)
{
    if(NULL == exePath)
    {
        return NULL;
    }

    SubProcess *pSubProcess = (SubProcess *)malloc(sizeof(SubProcess));
    if (NULL == pSubProcess)
    {
        return NULL;
    }

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
    DList *pList = NULL;
    

    pMonitor = (DMonitor *)malloc(sizeof(DMonitor));
    if (NULL == pMonitor)
    {
        return NULL;
    }

    pList = dlist_init(NULL, NULL);
    if (NULL == pList)
    {
        free(pMonitor);
        return NULL;
    }

    /* exeOrderPath为空，则缺省使用execOrder.txt作为可执行程序顺序文件 */
    if (NULL == exeOrderPath)
    {
        inputFile = fopen("../InputFile/execOrder.txt", "r");
    }
    /* 否则，使用用户定义的文件路径 */
    else
    {
        inputFile = fopen(exeOrderPath, "r");
    }

    /* 文件打开失败的情况 */
    if (NULL == inputFile)
    {
        free(pMonitor);
        dlist_delete(pList);
        return NULL;
    }

    /* 读入可执行文件路径并储存在链表中 */
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
    /* 如果未传入fifoPath，将pMonitor中的fifoPath为缺省值*/
    if (NULL == fifoPath || 0 == strlen(fifoPath))
    {
        strncpy(pMonitor->fifoPath, DEFAULT_FIFO_PATH, MAX_LENGTH);
    }
    /* 传入了fifoPath，将pMonitor中的fifoPath置为对应值 */
    else
    {
        strncpy(pMonitor->fifoPath, fifoPath, MAX_LENGTH);
    }

    return pMonitor;
}

int dmonitor_delete(DMonitor *pMonitor)
{
    if (NULL == pMonitor)
    {
        return NULL_POINTER;
    }

    dlist_delete(pMonitor->subProcessList);
    free(pMonitor);
    return SUCCESS;
}

void dmonitor_run(DMonitor *pMonitor)
{
    int fifoRet, fifoFd;
    char *fifoPath;
    char fifoBuf[MAX_LENGTH];

    DList *pList = pMonitor->subProcessList;
    DListNode *pNode = pList->first->next;
    int subPid;
    SubProcess *pSubProcess;
    int initOK;

    if (NULL == pMonitor)
    {
        printf("don't input NULL pointer!\n");
        return;
    }

    /* 创建/打开有名管道 */
    fifoPath = pMonitor->fifoPath;
    if (-1 == access(fifoPath, F_OK))
    {
        fifoRet = mkfifo(fifoPath, 0777);
        if (-1 == fifoRet)
        {
            printf("create FIFO failed!\n");
            return;
        }
    }

    /* 遍历链表，创建子进程 */
    while (pNode != pList->last)
    {
        pSubProcess = (SubProcess *)(pNode->data);
        subPid = fork();

        /* fork失败的情况，主进程中 */
        if (subPid < 0)
        {
            printf("create subProcess failed!\n");
            break;
        }
        /* 子进程中 */
        else if (0 == subPid)
        {
            /* 子进程进行初始化 */
            initOK = dmonitor_subProcess_init(pSubProcess->path);
            /* 子进程尝试以写方式打开管道 */
            fifoFd = open(fifoPath, O_WRONLY);
            if (-1 == fifoFd)
            {
                printf("subProcess %d open fifo failed!\n", getpid());
                break;
            }

            /* 子进程初始化成功的情况下，子进程的处理 */
            if (initOK > 0)
            {
                strncpy(fifoBuf, "init_ok\0", MAX_LENGTH);
            }
            /* 子进程初始化失败的情况，子进程的处理 */
            else
            {
                strncpy(fifoBuf, "init_failed\0", MAX_LENGTH);
            }

            write(fifoFd, fifoBuf, MAX_LENGTH);
            close(fifoFd);

            /* 子进程需要尽快退出while循环，否则子进程又会fork出新的孙子进程 */
            break;
        }
        /* 父进程中 */
        else
        {
            fifoFd = open(fifoPath, O_RDONLY);
            if (-1 == fifoFd)
            {
                printf("parent process open fifo failed!\n");
                return;
            }

            read(fifoFd, fifoBuf, MAX_LENGTH);
            close(fifoFd);

            if (0 == strcmp(fifoBuf, "init_ok"))
            {
                pSubProcess->pid = subPid;
                printf("create subprocess %s success!\n", pSubProcess->path);
            }
            else
            {
                /* 子进程未能成功启动，父进程给已经fork出的子进程收尸 */
                waitpid(subPid, NULL, 0);
                printf("subProcess %s init failed!\n", pSubProcess->path);
                /* break出去，停止启动后续进程 */
                break;
            }
        }

        pNode = pNode->next;
    }

    /* 是子进程的情况 */
    if (0 == subPid)
    {
        /* 初始化成功的情况(文件存在且可执行) */
        if (initOK > 0)
        {
            /* 子进程的init函数已经检查该路径的文件是存在且可执行的，因此execl不需要进行判断 */
            execl(pSubProcess->path, "", NULL);
            exit(0);
        }
        /* 文件不存在或不可执行 */
        else
        {
            exit(-1);
        }
    }
    /* 是父进程的情况:1、父进程顺利启动了所有的子进程 2、父进程启动某个进程失败，并从那个地方break出来 */
    else
    {
        /* 删除链表中pid为负值、未能成功启动的子进程 */
        dlist_delete_node_by_data(pMonitor->subProcessList, NULL, (fp_compare)dmonitor_find_negative_pid);
        while (1)
        {
            /* 以非阻塞方式wait子进程退出 */
            int exitPid = waitpid(-1, NULL, WNOHANG);

            /* 如果没有子进程退出 */
            if (exitPid <= 0)
            {
                /* 3秒之后继续监控 */
                printf("parent process find no exit subprocess.\n");
                sleep(3);
            }
            /* 如果有子进程退出 */
            else
            {
                /* 根据pid在链表中查找该子进程相关信息 */
                DListNode *pFindNode = dlist_get_node_by_data(pMonitor->subProcessList, &exitPid, (fp_compare)dmonitor_find_subProcess_by_pid);
                if (NULL != pFindNode)
                {
                    pSubProcess = (SubProcess *)dlist_get_data_from_node(pFindNode);
                    printf("parent process find process %s has exited, try to reload it...\n", pSubProcess->path);

                    /* 创建新的子进程以重启退出的进程 */
                    subPid = fork();
                    if (subPid < 0)
                    {
                        printf("reload process %s failed because of creating subprocess failed!\n", pSubProcess->path);
                    }
                    else if (0 == subPid)
                    {
                        break;
                    }
                    else
                    {
                        /* 父进程修改存储的子进程的pid */
                        pSubProcess->pid = subPid;
                        printf("reload process %s success!\n", pSubProcess->path);
                    }
                }
            }
        }
    }

    if (0 == subPid)
    {
        execl(pSubProcess->path, "", NULL);
        exit(0);
    }
}