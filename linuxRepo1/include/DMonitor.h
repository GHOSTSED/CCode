#ifndef _DMONITOR_H_
#define _DMONITOR_H_

/*************************************************************************************************************************************/
/*												  INCLUDE_FILES																		 */
/*************************************************************************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
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
typedef struct _SubProcess SubProcess;

typedef struct _DMonitor DMonitor;

/*************************************************************************************************************************************/
/*												   FUNCTIONS																		 */
/*************************************************************************************************************************************/

/*
*@fn				                        DMonitor *dmonitor_init(const char *exeOrderPath, const char *fifoPath);
*@brief   			                        用于对DMonitor进行初始化
*@details	                                该函数主要作用为从exeOrderPath中读入要执行的各个文件的路径，还不涉及到创建子进程等操作		
*
*@param[in] const char* exeOrderPath	    用户给定的子进程可执行文件的启动顺序的文本文件，如果传入NULL，默认为inputFile文件夹下的execOrder.txt
*@param[in] const char* fifoPath	        用户给定的用于进行进程间通信的有名管道的路径，如果传入NULL，默认为pipe文件夹下的myFifo	    
*
*@return			                        读取文件信息后的DMonitor指针，如果中间发生错误，则返回NULL
*@retval			
*/
DMonitor *dmonitor_init(const char *exeOrderPath, const char *fifoPath);

/*
*@fn				                        int dmonitor_delete(DMonitor *pMonitor);
*@brief   			                        用于对DMonitor对象进行销毁
*@details	                                		
*
*@param[in] DMonitor* pMonitor	            要销毁的DMonitor对象
*
*@return			                        执行情况，如果传入NULL，返回NULL_POINTER；成功销毁返回SUCCESS
*@retval			
*/
int dmonitor_delete(DMonitor *pMonitor);

/*
*@fn				                        void dmonitor_run(DMonitor *pMonitor);
*@brief   			                        用于启动DMonitor对象
*@details                                   主要内容为根据init读入的可执行文件的路径和顺序，挨个创建子进程并执行。内部为死循环，需要手动退出	                                		
*
*@param[in] DMonitor* pMonitor	            要启动的DMonitor对象
*
*@return			                        N/A
*@retval			
*/
void dmonitor_run(DMonitor *pMonitor);

#endif