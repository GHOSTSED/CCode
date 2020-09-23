#ifndef _DMAILLISTENER_H_
#define _DMAILLISTENER_H_

/*************************************************************************************************************************************/
/*												  INCLUDE_FILES																		 */
/*************************************************************************************************************************************/
#include <stdio.h>
#include <pcap.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <string.h>
#include "DList.h"
#include "DHashTable.h"

/*************************************************************************************************************************************/
/*												    DEFINES																		 	 */
/*************************************************************************************************************************************/
#define EMAIL_ADDRESS_MAX_LENGTH 50
#define TIME_MAX_LENGTH 50
#define HASH_TABLE_SIZE 128
#define BUFFER_MAX_LENGTH 256

/*************************************************************************************************************************************/
/*												     TYPES																		     */
/*************************************************************************************************************************************/
typedef struct _DMailListener DMailListener;

/*************************************************************************************************************************************/
/*												   FUNCTIONS																		 */
/*************************************************************************************************************************************/
/*
*@fn							    DMailListener *dmailListener_init(int tcpLinkTableSize, int emailInfoTableSize);
*@brief   						    用于对邮件监听器进行初始化
*@details	
*
*@param[in] int tcpLinkTableSize	用于指定邮件监听器维护的tcp连接的hash表大小
*@param[in] int emailInfoTableSize  用于指定邮件监听器维护的存储监听到的所有邮件信息的hash表大小        
*
*@return						    一个只有头节点的链表指针，若申请空间失败，则返回NULL
*@retval
*/
DMailListener *dmailListener_init(int tcpLinkTableSize, int emailInfoTableSize);

void dmailListener_run(DMailListener *pMailListener ,const char *filePath);

int dmailListener_delete(DMailListener *pMailListener);

void dmailListener_print_all_mailInfo(DMailListener *pMailListener);

#endif