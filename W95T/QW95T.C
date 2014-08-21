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
#include <windows.h>
#include <winsock.h>
#include <assert.h>
#include "socketq.h"

#define SOCKET_QSIZE 4
/* these functions implement a single statically allocated */
/* queue for socket id's to be used by the windows 95      */
/* multithreaded web server application                    */
typedef struct socket_queue_s {
   long     count; /* count of objects for debug purposes */
   long     qhead; /* elements inserted at head of queue  */
   long     qtail; /* elements removed from tail of queue */
   long     qsize; /* fixed size of queue                 */
   HANDLE   sdata; /* semaphore for data available        */
   HANDLE   sroom; /* semaphore for room in queue         */
   HANDLE   qmutx; /* mutex for queue data structure      */
   SOCKET   qsock[SOCKET_QSIZE];
   } socket_queue_t;

/* instantiate a single socket queue of fixed size  */
/* for this implementation                          */
socket_queue_t w95t_socketq;

/* constructor */
SOCKET_QUEUE socket_qinit(long size)
{
   int i;

   /* initialize the queue data structure */
   w95t_socketq.count = 0;
   w95t_socketq.qhead = 0;
   w95t_socketq.qtail = 0;
   w95t_socketq.qsize = SOCKET_QSIZE; /* ignore input size in this implementation */

   /* initial q data elements to a known value */
   for(i=0;i<SOCKET_QSIZE;i++) {
      w95t_socketq.qsock[i] = INVALID_SOCKET;
   }

   /* create an unnamed  semaphore for ROOM_IN_QUEUE with an initial */
   /* count = SOCKET_QSIZE                                           */
   w95t_socketq.sroom = CreateSemaphore(0,SOCKET_QSIZE,SOCKET_QSIZE,0);
   assert(w95t_socketq.sroom != 0);

   /* create an unnamed semaphore for DATA_IN_QUEUE with an initial */
   /* count = 0                                                     */
   w95t_socketq.sdata = CreateSemaphore(0,0,SOCKET_QSIZE,0);
   assert(w95t_socketq.sdata != 0);

   /* create a critical section for the data structure     */
   /* mutex is unnamed, and initially is not owned         */
   w95t_socketq.qmutx = CreateMutex(0,FALSE,0);

   return &w95t_socketq;
}

void socket_qsend(SOCKET_QUEUE q,SOCKET s)
{
   socket_queue_t *qp = q;
   DWORD           wait_result;
   BOOL            signal_result;

   /* wait on semaphore for ROOM_IN_QUEUE */
   wait_result = WaitForSingleObject(qp->sroom,INFINITE);
   assert(wait_result == WAIT_OBJECT_0);

   /* lock access */
   wait_result = WaitForSingleObject(qp->qmutx,INFINITE);
   assert(wait_result == WAIT_OBJECT_0);

   /* insert an element at qhead */
   qp->qsock[qp->qhead] = s;

   /* increment head index with wrap */
   qp->qhead++;
   if (qp->qhead >= qp->qsize) {
      qp->qhead = 0;
   }

   /* increment count */
   qp->count++;

   /* unlock access */
   signal_result = ReleaseMutex(qp->qmutx);
   assert(signal_result == TRUE);

   /* signal DATA_IN_QUEUE */
   signal_result = ReleaseSemaphore(qp->sdata,1,0);
   assert(signal_result == TRUE);
}

SOCKET socket_qrecv(SOCKET_QUEUE q)
{
   socket_queue_t *qp = q;
   SOCKET          s;
   BOOL            signal_result;
   DWORD           wait_result;

   /* wait on semaphore for DATA_IN_QUEUE */
   wait_result = WaitForSingleObject(qp->sdata,INFINITE);
   assert(wait_result == WAIT_OBJECT_0);

   /* lock access */
   wait_result = WaitForSingleObject(qp->qmutx,INFINITE);
   assert(wait_result == WAIT_OBJECT_0);

   /* remove one data item from qtail */
   s = qp->qsock[qp->qtail];

   /* increment tail pointer with wrap */
   qp->qtail++;
   if (qp->qtail >= qp->qsize) {
      qp->qtail = 0;
   }

   /* decrement count */
   qp->count--;

   /* unlock access */
   signal_result = ReleaseMutex(qp->qmutx);
   assert(signal_result == TRUE);

   /* signal ROOM IN QUEUE */
   signal_result = ReleaseSemaphore(qp->sroom,1,0);
   assert(signal_result == TRUE);

   return s;
}
