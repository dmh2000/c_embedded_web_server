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
#include <stdlib.h>
#include <string.h>
#include "ews.h"
#include "ewspvt.h"

/* =========================== Session ====================================== */

/* open a session to a Session */
long  em_SessionConnect( em_session_t *sep ,em_io_t *iop)
{
   long status;

   /* check incoming parameters */
   if ((sep == 0)||(iop == 0)) {
      /* internal server error */
      return S_500_SRVERR;
   }

   /* open the io handler and get a session id in return */
   sep->iod = em_IoOpen( iop );

   /* check if open succeeded or failed */
   if (sep->iod == 0){
      /* open failed or this thread is the parent thread */
      status = EMERR_OPEN;
   }
   else {
      /* open succeeded or this thread is the child thread */
      status = S_200_OK;
   }

   return status;
}

/* close the session conditionally, depending on stay-alive bit */
void em_SessionDisconnect( em_session_t *sep, em_io_t *iop)
{
   /* close the connection */
   em_IoClose( iop, sep->iod );

   /* mark io descriptor closed */
   sep->iod = -1;
}

/* close the session unconditionally */
void em_SessionForceDisconnect( em_session_t *sep, em_io_t *iop )
{
   /* close the connection unconditionally */
   em_IoClose( iop, sep->iod);

   /* mark io descriptor closed */
   sep->iod       = -1;
}

/* initialize a Session data structure */
void em_SessionInit( em_session_t *sep)
{
   sep->iod       = -1;
}

/* reinitialize a Session data structure */
void em_SessionReInit( em_session_t *sep )
{
   /* clear io descriptor */
   sep->iod = -1;
}

/* read the REQUEST line from the Session and parse into fields */
long em_SessionRequest( em_session_t *sep, em_io_t *iop, em_request_t *reqp, long timeout )
{
   long status;

   /* read one line from Session */
   status = em_IoReadLine( iop, sep->iod, reqp->Lin, ( long ) sizeof( reqp->Lin ), timeout );
   if (status != S_200_OK) {
      /* error reading from Session */
      /* return error status to server */
      return status;
   }

   /* parse the request into fields */
   status = em_RequestGetFields( reqp,iop,sep );
   if (status != S_200_OK) {
      /* did not succeed, return appropriate error code */
      return status;
   }

   /* convert the method string to a method index */
   reqp->method = em_RequestGetMethod( reqp->Met );
   if (reqp->method == METHOD_UNDEF) {
      /* return appropriate error code */
      return S_501_NOTIMP;
   }

   /* get HTTP version number */
   reqp->version = em_RequestGetHTTPVersion( reqp->Ver );
   if (reqp->version == HTTP_NOV) {
      /* return appropriate error code */
      return S_501_NOTIMP;
   }

   /* success, return OK */
   return S_200_OK;
}

/* ==================IO  functions ================== */
/* read a line from the application layer */
long em_IoReadLine( em_io_t *io, long iod, char *buf, long maxlen, long timeout )
{
   char* s;
   long  l;
   long  count;
   long  status;
   long  ml;

   s      = buf;
   count  = 0;
   status = S_200_OK;
   ml     = maxlen - 1; /* leave space for delimiting '0' */
   for(;;) {
      /* read one byte at a time */
      l =  io->read( io, iod, s, 1, timeout );
      if (l == 0) {
         /* error reading, connection lost before end of line */
         status = EMERR_READ;
         break;
      }

      /* check for end of input line */
      if (*s == 0x0a) {
         /* end of line, break out (assume 0x0d always precedes 0x0a per http spec */
         s++;
         *s = '\0'; /* delimit, there is always room for the 0 */
         break;
      }

      /* increment count */
      count++;
      if (count >= ml) {
         /* too many bytes in the request line (URL is probably too long) */
         status = S_400_BADREQ;
         break;
      }

      /* advance buffer pointer */
      s++;
   }
   return status;
}
