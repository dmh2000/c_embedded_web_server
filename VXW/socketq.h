#ifndef __SOCKETQ__
#define __SOCKETQ__

/* keep implementation local to file containing function bodies */
typedef void *SOCKET;
typedef void *SOCKET_QUEUE;

extern SOCKET_QUEUE socket_qinit(long size);
extern void         socket_qsend(SOCKET_QUEUE q,SOCKET s);
extern SOCKET       socket_qrecv(SOCKET_QUEUE q);

#endif
