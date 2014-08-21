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


/* master socket queue for all threads to use */
SOCKET_QUEUE thread_sockq;

/* server port number , 80 is default for HTTP */
const unsigned short server_port = 80;

/* pseudo bin directory for executables */
const char *app_binurl = "";

/* statically allocated server workspace object */
#define NUMBER_OF_SERVER_TASKS 4

#define WRITEBLOCKSIZE 1024

/* per task data */
em_workspace_t       vxt_workspace[NUMBER_OF_SERVER_TASKS];
int                  server_tid[NUMBER_OF_SERVER_TASKS];
app_resp_t           vxt_dresp[NUMBER_OF_SERVER_TASKS];

/* ================================================================== */
/* System Specific Delay Function                                     */
/* ================================================================== */
long ews_delay(unsigned long milliseconds)
{
    (void)taskDelay((int)((milliseconds * (unsigned long)sysClkRateGet()) / 1000));

    return 0;
}

/* GMT return Greenwich mean time string */
const char *vxt_GMT( em_io_t *io, char *buffer )
{
    time_t ltime;
    struct tm *gmt;

    /* render the RFC822 time into the input buffer and return it */
    (void)time( &ltime );
    gmt = gmtime( &ltime );
    (void)strftime( buffer, EM_DATESIZE, "%a, %d %b %Y %H:%M:%S GMT", gmt );

    return buffer;
}

/* terminate the server */
void  vxt_Exit( em_io_t *io, long code )
{
    (void)taskSuspend(0);
}

/* wait for an incoming connection */
long vxt_Open( em_io_t *io )
{
    SOCKET              clientSock;

    /* make sure input is not null */
    if (io == 0) {
        printf("Open : io pointer is null");
        return (long)INVALID_SOCKET;
    }

    /* wait for a connection at the socket queue instead */
    /* of directly at the accept call                    */
    clientSock = socket_qrecv(thread_sockq);

    /* logic in connection thread guarantees this socket */
    /* will be valid                                     */

    /* returning clientSock always */
    return (long)clientSock;
}

/* close a connection */
void vxt_Close( em_io_t *io, long iod )
{
    long result;
    char line[80];

    /* return immediately if input parameter is null */
    if (io == 0) {
        return;
    }

#ifdef DEBUG_SERVER    
    printf("Close:clientSock=%ld",iod);
#endif
    

    /* close the clientSock which is stored in iod */
    result = close((SOCKET)iod );
    if (result == SOCKET_ERROR) {
        printf("Error closing socket : %d",errno);
        printf(line);
    }
}

/* write data out a connected socket */
long vxt_Write( em_io_t *io, long iod, const char* buf,long len)
{
    long status = 0;
    long        l;
    long        s;
    const char *b;
#ifdef DEBUG_SERVER    
    char        line[80];
#endif    

    /* check that input parameters are valid */
    if ((io == 0)||(buf == 0)) {
        return S_500_SRVERR;
    }

    /* write data in a loop with small write buffer */
    /* tailored to implementation packet size       */
    b = buf;
    l = len;
    while(l > 0) {
        /* get length of block to write out */
        if (l > WRITEBLOCKSIZE) {
            s = WRITEBLOCKSIZE;
        }
        else {
            s = l;
        }
        /* decrement total byte count */
        l -= s;

        /* write out next block */
        status = send((SOCKET)iod,(char *)b,s,0);
        if (status == SOCKET_ERROR) {
            status = EMERR_WRITE; /* lost the connection */
            break;
        }
        else {
            status = 0;
        }

        /* point to next block */
        b += s;

#ifdef DEBUG_SERVER
        /* debug message */
        printf("write blocklen = %ld",s);
#endif        
        
    }

    return status;
}

/* read data in from a connected socket */
long vxt_Read( em_io_t *io, long iod, char *line, long len, long tout )
{
    long l;

    if ((io == 0)||(line == 0)) {
        return 0;
    }

    /* get data from socket */
    l = recv((SOCKET)iod,line,len,0);
    if (l == SOCKET_ERROR) {
        /* socket error is treated as end of file */
        l = 0;
    }

    /* return length of data read */
    return l;
}

/* ========================================================== */
/* Windows 2000 FILE IO                                       */
/* ========================================================== */
int ews_feof(void *stream)
{
    FILE *f = (FILE *)stream;
    return feof(f); //lint !e740 !e611
}

void *ews_fopen(const char *filename,const char *mode)
{
    return fopen(filename,mode);
}

void  ews_fclose(void *stream)
{
    fclose((FILE *)stream);
}

unsigned long ews_fread(char *buff,unsigned long size,unsigned long nobj,void *stream)
{
    return (unsigned long)fread(buff,size,nobj,(FILE *)stream);
}

/* ================================================================== */
/* EWS_FSIZE                                   */
/* return size of file in bytes                */
/* stream must be open FILE * to same filename */
long ews_fsize(const char *filename,void *stream)
{
    struct  stat filestat;
    int          result;
    long         size;

    result = stat((char *)filename, &filestat );
    if (result == 0) {
        /* got a filesize */
        size = (long)filestat.st_size;
    }
    else {
        /* indicate no size found */
        size = 0;
    }

    return size;
}

/* ========================================================== */
/* Windows 2000 thread stub for embedded server                 */
/* ========================================================== */
void vxt_server_thread(int iwrk)
{
#ifdef DEBUG_SERVER
    char line[80];
    printf("Server Thread wrk = %08lx",(unsigned long)iwrk);
#endif    

    /* execute the embedded server with workspace parameter */
    embedded_server((em_workspace_t *)iwrk);

    /* if the server returns, the thread returns and dies normally */
}

/* =========================== */
/* MAIN ENTRY POINT            */
/* main connection accept task */
/* =========================== */
void ews_task(int addr,int port)
{
    struct sockaddr_in   servAddr;
    int                  result;
    int                  err;
    SOCKET               masterSock;
    SOCKET               clientSock;
    em_workspace_t       *wrk;
    int                  i;
    int                  clientAddrLen;
    struct sockaddr_in   clientAddr;
    char                 line[80];
    char                 b1[32];
    int                  status;

    /* Create a socket and bind address, listen for incoming connections */
    masterSock = socket( PF_INET, SOCK_STREAM, 0 );
    if ( masterSock == INVALID_SOCKET ) {
        printf("socket(..) failed");
        return;
    }
    
    //set up the address
    memset(&servAddr,0,sizeof(servAddr));
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_port        = (unsigned short)port;
    servAddr.sin_family      = AF_INET;
    servAddr.sin_len         = sizeof(servAddr);

    printf("WEB binding to %s:%d",net_inetntoa(servAddr.sin_addr,b1),ntohs(servAddr.sin_port));

    /*lint -e740 unusual pointer cast */
    result = bind( masterSock, ( struct sockaddr * ) &servAddr, sizeof(servAddr) );
    if ( result == -1 ) {
        printf("bind failed %d",errno);
        printf(line);
        return;
    }

    /*lint _e740 */

    result = listen( masterSock, 3 );
    if ( result == -1 ) {
        printf("listen failed %d",errno);
        printf(line);
        return;
    }

#ifdef DEBUG_SERVER
    printf("masterSock listening on port %hu",ntohs(servAddr.sin_port));
#endif    

    /* ================================== */
    /* create the client connection queue */
    /* ================================== */
    thread_sockq = socket_qinit(NUMBER_OF_SERVER_TASKS);

    /* for each vrtx connection thread */
    for(i=0;i<NUMBER_OF_SERVER_TASKS;i++) {
        /* initialize a workspace  */
        wrk = &vxt_workspace[i];

        /* clear it to all 0's to start */
        memset(wrk,0,sizeof(em_workspace_t));

        /* SOCKET IO FUNCTIONS */
        wrk->system.p           = &vxt_dresp[i];
        wrk->system.read        = vxt_Read;
        wrk->system.write       = vxt_Write;
        wrk->system.open        = vxt_Open;
        wrk->system.close       = vxt_Close;
        wrk->system.exit        = vxt_Exit;
        wrk->system.gmt         = vxt_GMT;
        wrk->system.pagebin     = app_binurl;

        /* spawn the thread and pass it the workspace pointer */
        printf("WEB%d",i);
        status = taskSpawn(line,
                  TASK_PRIORITY_WEB,
                  VX_FP_TASK,
                  0x2000,
                  (FUNCPTR)vxt_server_thread,
                  (int)wrk,
                  0,0,0,0,0,0,0,0,0
                  );
        if (status == ERROR) {
            printf("Can't start WEB worker thread");
        }              
    }

    /* all threads are spawned */
    /* loop, waiting for connections */
    /* when a connection is received, enqueue it for the server threads */
    for(;;) {
        /* specify the max length of the address structure and accept the
        next connection.  clientAddrLen is passed as a pointer as it is
        updated by the accept call.
        retry accept until it works
        */
#ifdef DEBUG_SERVER        
        printf("Accept : masterSock = %ld",masterSock);
#endif        

        clientAddrLen = sizeof (clientAddr);
        clientSock = accept( masterSock, (struct sockaddr *)&clientAddr, &clientAddrLen );


        /* if accept call fails */
        if ( clientSock == INVALID_SOCKET ) {
            /* get winsock error */
            err = errno;

            /* print it to error output */
            printf("Accept failed : %d",err);
            printf(line);

            /* quit */
            break;
        }

#ifdef DEBUG_SERVER
        printf("Connect : clientSock = %ld",clientSock);
#endif        

        /* client socket is accepted  */
        /* enqueue it for the servers */
        /* return (long)clientSock;   */
        socket_qsend(thread_sockq,clientSock);
    }
}

void ews_start(struct sockaddr_in sin)
{
    int status;
    status = taskSpawn("WEBS",
              TASK_PRIORITY_WEB,
              VX_FP_TASK,
              0x2000,
              (FUNCPTR)ews_task,
              (int)sin.sin_addr.s_addr,
              (int)sin.sin_port,
              0,0,0,0,0,0,0,0
              );
    if (status == ERROR) {
        printf("Can't start WEB main thread");
    }           
}
