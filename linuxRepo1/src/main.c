#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO "./myFifo"

// int dmonitor_creat_son_process(char *pipePath, char *exePath)

int main()
{
    int ret, fd;
    char buf[20];
    if(-1 == access(FIFO, F_OK))
    {
        ret = mkfifo(FIFO, 0777);
        if(-1 == ret)
        {
            printf("create FIFO failed!\n");
            exit(-1);
        }
    }
    
    int pid = fork();
    if(0 == pid)
    {
        fd = open(FIFO, O_RDONLY);
        int readLen = read(fd, buf, 20);
        if(readLen > 0)
        {
            printf("%s\n", buf);
        }
        else
        {
            printf("read nothing...\n");
        }
        
    }
    else
    {
        fd = open(FIFO, O_WRONLY);
        int writeLen = write(fd, "233\0", 4);
        if(writeLen > 0)
        {
            printf("write success!\n");
        }
        else
        {
            printf("write failed!\n");
        }
    }
    
    
    return 0;
}
