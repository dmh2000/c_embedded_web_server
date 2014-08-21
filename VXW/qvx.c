#include <vxWorks.h>
#include <sysLib.h>
#include <msgQLib.h>
#include <tickLib.h>
#include <taskLib.h>
#include <sockLib.h>
#include <ioLib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/stat.h>

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ewspvt.h"
#include "socketq.h"


/* constructor */
SOCKET_QUEUE socket_qinit(long size)
{
    MSG_Q_ID q;
    
    q = msgQCreate(size,sizeof(SOCKET),MSG_Q_FIFO);
    if (q == 0) {
        printf("can't create socket queue");
    }
    
    return (SOCKET_QUEUE)q;
}

void socket_qsend(SOCKET_QUEUE q,SOCKET s)
{
    int status;
    
    status = msgQSend((MSG_Q_ID)q,(char *)&s,sizeof(SOCKET),WAIT_FOREVER,MSG_PRI_NORMAL);
    if (status == ERROR) {
        printf("error sending to socket queue");
    }
}

SOCKET socket_qrecv(SOCKET_QUEUE q)
{
    SOCKET s;
    int    status;
    
    status = msgQReceive((MSG_Q_ID)q,(char *)&s,sizeof(SOCKET),WAIT_FOREVER);
    if (status == ERROR) {
        printf("error receiving from socket queue");
    }
    
    return s;
}
