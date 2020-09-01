#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include "../include/DMonitor.h"
#include "../include/DList.h"


int main()
{
    DMonitor *myMonitor = dmonitor_init(NULL, NULL);
    if(NULL == myMonitor)
    {
        printf("NULL\n");
    }
    
    dmonitor_run(myMonitor);
    dmonitor_delete(myMonitor);

    return 0;
}

