/*
 * Copyright (c) 1995,1996
 * David M. Howard
 * daveh@hendrix.reno.nv.us
 * www.greatbasin.net/~daveh
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  
 * David M. Howard makes no representations about the suitability of this
 * software for any purpose.  It is provided 'as is' without express or
 * implied warranty. Do not use it in systems where damage to property
 * or injury to people may occur in the event of a malfunction
 *
 * A supported version of this software including updates is available.
 * Consulting services for porting, modification, web user interface design
 * and implementation are available also.
 * Contact David M. Howard at the above email address for details.
 */
/*H*/
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <winsock.h>
#include <process.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ews.h>
#include <ewsapp.h>
#include "socketq.h"

/* master socket queue for all threads to use */
SOCKET_QUEUE thread_sockq;

/* server port number , 80 is default for HTTP */
const unsigned short w95t_server_port = 80;

/* pseudo bin directory for executables */
const char *app_binurl = "";

/* statically allocated server workspace object */
#define NUMBER_OF_SERVER_TASKS 8

#define WRITEBLOCKSIZE 1024

/* per task data */
em_workspace_t       w95t_workspace[NUMBER_OF_SERVER_TASKS];
int                  server_tid[NUMBER_OF_SERVER_TASKS];
app_resp_t           w95t_dresp[NUMBER_OF_SERVER_TASKS];


/* ====================================================== */
/* DEBUG SUPPORT                                          */
/* ====================================================== */
#ifndef NDEBUG

/* forward reference */
void ews_printf(const char *fmt,...);

/* ====================================================== */
/* DEBUGGING, IO FUNCTIONS ARE REAL                       */
/* ====================================================== */
/* need mutual exclusion around putn and printf so output */
/* is not interleaved on the display                      */
HANDLE printf_mutex;

void printf_block(void)
{
   DWORD wait_result;

   /* lock access */
   wait_result = WaitForSingleObject(printf_mutex,INFINITE);
   if (wait_result != WAIT_OBJECT_0) {
      ews_printf("Print_block: Mutex error\n");
      exit(1);
   }
}

void printf_unblock(void)
{
   int signal_result;

   /* unlock access */
   signal_result = ReleaseMutex(printf_mutex);
   if (signal_result != TRUE) {
      ews_printf("Printf_unblock: mutex error\n");
      exit(1);
   }
}

void printf_init(void)
{
   /* create a critical section for the printout functions */
   /* mutex is unnamed, and initially is not owned         */
   printf_mutex = CreateMutex(0,FALSE,0);
}

/* put 'n' characters routine */
void ews_putn(const char *s,long len)
{
   long i;
   if (s == 0) return;

   printf_block();
   for(i=0;i<len;i++) {
      fputc(s[i],stdout);
   }
   printf_unblock();
}

/* printf type function */
void ews_printf(const char *fmt,...)
{
   va_list ap;

   va_start(ap,fmt);

   printf_block();
   vprintf(fmt,ap);
   printf_unblock();

   va_end(ap);
}

#else
/* ============================================== */
/* NOT DEBUGGING, IO FUNCTIONS ARE STUBS          */
/* ============================================== */
/* if not debugging, use macros to eliminate */
#define ews_putn(x,l)

void ews_printf(const char *fmt,...)
{
}
#endif

/* ================================================================== */
/* System Specific Delay Function                                     */
/* ================================================================== */
long ews_delay(unsigned long milliseconds)
{
   HANDLE m;
   DWORD  result;
   BOOL   b;
   long   status = S_200_OK;

   /* create a semaphore to use to generate a delay */
   /* USE WHATEVER DELAY MECHANISM YOUR SYSTEM HAS  */
   m = CreateSemaphore(0,0,1,0);
   if (m == 0) {
      return S_500_SRVERR;
   }

   /* USE WHATEVER DELAY MECHANISM YOUR SYSTEM HAS */
   result = WaitForSingleObject(m,200);
   if (result == WAIT_FAILED) {
      status = S_500_SRVERR;
   }

   /* get rid of the semaphore */
   b = CloseHandle(m);
   if (b != TRUE) {
      status = S_500_SRVERR;
   }

   return status;
}

/* GMT return Greenwich mean time string */
const char *w95t_GMT( em_io_t *io, char *buffer )
{
   time_t ltime;
   struct tm *gmt;

   /* render the RFC822 time into the input buffer and return it */
   time( &ltime );
   gmt = gmtime( &ltime );
   strftime( buffer, EM_DATESIZE, "%a, %d %b %Y %H:%M:%S GMT", gmt );

   return buffer;
}

/* terminate the server */
void  w95t_Exit( em_io_t *io, long code )
{
   ews_printf("Exit code=%ld\n\r",code);

   exit(code);
}

/* wait for an incoming connection */
long w95t_Open( em_io_t *io )
{
   SOCKET              clientSock;

   /* make sure input is not null */
   if (io == 0) {
      ews_printf("Open : io pointer is null\n");
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
void w95t_Close( em_io_t *io, long iod )
{
   long result;
   int  err;

   /* return immediately if input parameter is null */
   if (io == 0) {
      return;
   }

   ews_printf("Close:clientSock=%ld\n\r",iod);

   /* close the clientSock which is stored in iod */
   result = closesocket((SOCKET)iod );
   if (result == SOCKET_ERROR) {
      err = WSAGetLastError();
      ews_printf("Error closing socket : %ld : %d",iod,err);
   }
}

/* write data out a connected socket */
long w95t_Write( em_io_t *io, long iod, const char* buf,long len)
{
   long status = 0;
   long        l;
   long        s;
   const char *b;

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

      /* debug message */
      ews_printf("write blocklen = %ld\n\r",s);
   }

   return status;
}

/* read data in from a connected socket */
long w95t_Read( em_io_t *io, long iod, char *line, long len, long tout )
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

   /* output what was read */
   if (l > 0) {
      ews_putn(line,l);
   }

   /* return length of data read */
   return l;
}

/* ========================================================== */
/* Windows 95 thread stub for embedded server                 */
/* ========================================================== */
int ews_feof(void *stream)
{
   return feof((FILE *)stream);
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
   return fread(buff,size,nobj,(FILE *)stream);
}

/* ================================================================== */
/* EWS_FSIZE                                   */
/* return size of file in bytes                */
/* stream must be open FILE * to same filename */
long ews_fsize(const char *filename,void *stream)
{
   struct _stat filestat;
   int          result;
   long         size;

   result = _stat( filename, &filestat );
   if (result == 0) {
      /* got a filesize */
      size = filestat.st_size;
   }
   else {
      /* indicate no size found */
      size = 0;
   }

   return size;
}

/* ========================================================== */
/* Windows 95 thread stub for embedded server                 */
/* ========================================================== */
void w95t_server_thread(void *arg)
{
   ews_printf("Server Thread wrk = %08lx\n",arg);

   /* execute the embedded server with workspace parameter */
   embedded_server(arg);

   /* if the server returns, the thread returns and dies normally */
}

/* =========================== */
/* MAIN ENTRY POINT            */
/* main connection accept task */
/* =========================== */
int main(int argc,char *argv[])
{
   unsigned short       port;
   int                  result;
   int                  err;
   SOCKET               masterSock;
   SOCKET               clientSock;
   em_workspace_t       *wrk;
   struct sockaddr_in   servAddr;
   int                  i;
   WORD                 wVersionRequested;
   WSADATA              wsaData;
   int                  clientAddrLen;
   struct sockaddr      clientAddr;

   fprintf(stdout,"Embedded Web Server - Windows 95 : %s\n",__TIMESTAMP__);
   fprintf(stdout,"Copyright (c) 1995-1996 David M. Howard\n");

   port = w95t_server_port;

   /* initialize printf */
   printf_init();

   /* =============================================== */
   /* initialize winsock library                      */
   /* =============================================== */

   wVersionRequested = MAKEWORD(1, 1);

   err = WSAStartup(wVersionRequested, &wsaData);
   if (err != 0) {
      fprintf(stderr,"Can't initialize winsock lib\n");
      exit(1);
   }

   /* Confirm that the Windows Sockets DLL supports 1.1.*/
   /* Note that if the DLL supports versions greater */
   /* than 1.1 in addition to 1.1, it will still return */
   /* 1.1 in wVersion since that is the version we */
   /* requested. */

   if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
      /* Tell the user that we couldn't find a useable */
      /* winsock.dll. */
      WSACleanup();
      fprintf(stderr,"Winsock doesn't support version 1.1\n");
      exit(1);
   }

   ews_printf("Winsock Initialized OK\n");

   /* =============================================== */
   /* The Windows Sockets DLL is acceptable. Proceed. */
   /* =============================================== */

   /* Create a socket and bind address, listen for incoming connections */
   masterSock = socket( PF_INET, SOCK_STREAM, 0 );
   if ( masterSock == INVALID_SOCKET ) {
      ews_printf("socket(..) failed\n");
      exit(1);
   }

   /*lint -e740 unusual pointer cast */
   memset((char *) &servAddr, 0, sizeof servAddr );
   servAddr.sin_family      = AF_INET;
   servAddr.sin_addr.s_addr = htonl( INADDR_ANY );
   servAddr.sin_port        = htons( port );
   result = bind( masterSock, ( struct sockaddr * ) &servAddr, sizeof servAddr );
   if ( result == -1 ) {
      ews_printf("bind(..) failed\n");
      exit(1);
   }

   /*lint _e740 */

   result = listen( masterSock, 3 );
   if ( result == -1 ) {
      ews_printf("listen(..) failed\n");
      exit(1);
   }

   ews_printf("masterSock listening on port %hu\n",port);

   /* ================================== */
   /* create the client connection queue */
   /* ================================== */
   thread_sockq = socket_qinit(NUMBER_OF_SERVER_TASKS);

   /* for each vrtx connection thread */
   for(i=0;i<NUMBER_OF_SERVER_TASKS;i++) {
      /* initialize a workspace  */
      wrk = &w95t_workspace[i];

      /* clear it to all 0's to start */
      memset(wrk,0,sizeof(em_workspace_t));

      /* SOCKET IO FUNCTIONS */
      wrk->system.p           = &w95t_dresp[i];
      wrk->system.read        = w95t_Read;
      wrk->system.write       = w95t_Write;
      wrk->system.open        = w95t_Open;
      wrk->system.close       = w95t_Close;
      wrk->system.exit        = w95t_Exit;
      wrk->system.gmt         = w95t_GMT;
      wrk->system.pagebin     = app_binurl;

      /* spawn the thread and pass it the workspace pointer */
      _beginthread(w95t_server_thread,0,wrk);
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
      ews_printf("Accept : masterSock = %ld\n",masterSock);

      clientAddrLen = sizeof (clientAddr);
      clientSock = accept( masterSock, &clientAddr, &clientAddrLen );


      /* if accept call fails */
      if ( clientSock == INVALID_SOCKET ) {
         /* get winsock error */
         err = WSAGetLastError();

         /* print it to error output */
         ews_printf("Accept failed : %d\n",err);

         /* quit */
         break;
      }

      ews_printf("Connect : clientSock = %ld\n",clientSock);

      /* client socket is accepted  */
      /* enqueue it for the servers */
      /* return (long)clientSock;   */
      socket_qsend(thread_sockq,clientSock);
   }

   return 0;
}
