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
#include <ctype.h>
#include <stdio.h>
#include "ews.h"
#include "ewspvt.h"

/* LINE FORMAT IS AS FOLLOWS: */
/* METHOD URL?ARGS VERSION    */
/* METHODS:                   */
/* "GET"                      */
/* "HEAD"                     */

/* read HTTP version from request header string */
long  em_RequestGetHTTPVersion( char *verp )
{
   long version;

   if (verp == 0) {
      version = HTTP_NOV;
   }
   else if (*verp == '\0') {
      /* version 0.9 has no version string */
      version = HTTP_0_9;
   }
   else if (strcmp( verp, "HTTP/1.0" ) == 0) {
      /* version 1.0 is the only one supported that has explicit version string */
      version = HTTP_1_0;
   }
   else {
      /* anything else is unsupported in this revision */
      version = HTTP_NOV;
   }

   return version;
}

/* determine request method from method part of request line */
long em_RequestGetMethod( char *metp )
{
   long  method;

   /* compare string to legal methods and return proper index */
   /* use brute force for now since there aren't many possibilities */
   if (metp == 0) {
      method = METHOD_UNDEF;
   }
   else if (strcmp( metp, "GET" ) == 0) {
      method = METHOD_GET;
   }
   else {
      method = METHOD_UNDEF;
   }

   return method;
}

/* divide the request line into separate fields */
long em_RequestGetFields(em_request_t *rqp,em_io_t *iop,em_session_t *sep)
{
   char *s;
   char *m;
   char *u;
   char *a;
   char *v;

   /* check input parameters */
   if ((rqp == 0)||(iop == 0)||(sep == 0)) {
      /* internal server error */
      return S_500_SRVERR;
   }

   /* point at beginning of line */
   s = rqp->Lin;

   /* skip leading whitespace */
   /* NOTE : isspace includes SP,FF,NL,CR,HT,VT */
   while (isspace( *s )) s++;

   /* quit if at end of string and no method is found */
   if (*s == '\0') {
      return S_400_BADREQ;
   }

   /* Copy method string */
   m = rqp->Met;
   while (isalpha( *s )) {
      *m = *s;
      m++;
      s++;
   }
   /* delimit copied string */
   *m = '\0';

   /* quit if at end of string with no URL */
   /* or if delimiter of method string is not whitespace */
   if (( *s == '\0' ) || ( isspace( *s ) == 0 )) {
      return S_400_BADREQ;
   }

   /* skip whitespace */
   while (isspace( *s )) s++;

   /* copy URL part of string to end of string, ?, or whitespace */
   u = rqp->Url;
   while (( *s != 0 ) && ( *s != '?' ) && ( isspace( *s ) == 0 )) {
      *u = *s;
      u++;
      s++;
   }
   /* delimit copied string */
   *u = '\0';

   /* if delimiter is ?, there is an argument part of the string */
   if (*s == '?') {
      /* copy argument part of string to end of string or next whitespace */
      a = rqp->Arg;
      while (( *s != 0 ) && ( isspace( *s ) == 0 )) {
         *a = *s;
         a++;
         s++;
      }
      /* delimit copied string */
      *a = '\0';
   }

   /* skip whitespace */
   while (isspace( *s )) s++;

   /* copy VERSION part of string if any */
   v = rqp->Ver;
   while (( *s != 0 ) && ( isspace( *s ) == 0 )) {
      *v = *s;
      v++;
      s++;
   }
   /* delimit copied string */
   *v = '\0';

   /* if anything is left in the input string, it is garbage */
   /* and is ignored. */

   return S_200_OK;
}

#define SCAN_BEGIN      0
#define SCAN_FIELDNAME  1
#define SCAN_FIELDVALUE 2
#define SCAN_HDREND     3
#define SCAN_SCANEND    4
#define SCAN_COMPLETE   5
#define SCAN_ERROR      6

/* get all header lines and find end of request headers */
/* this version reads incoming data as a block and then */
/* scans the individual characters. It assumes there    */
/* is no request body, so it is used for GET and HEAD   */
long em_RequestGETHeaders( em_request_t *rqp,em_io_t *iop, em_session_t *sep, long timeout )
{
   /* read bytes from host until none left or line begins with 0d/0a */
   /* parse bytes into lines delimited by 0d/0a */
   long  status;
   long  done;
   long  len;
   char *l;

   /* check input parameters */
   if ((rqp == 0)||(iop == 0)||(sep == 0)) {
      /* internal server error */
      return S_500_SRVERR;
   }

   done = 0;
   status = S_200_OK;
   em_RequestScanInit( rqp );
   while (done == 0) {
      /* read some bytes from client */
      len = em_IoRead( iop, sep->iod, rqp->Lin, ( long ) sizeof( rqp->Lin ), timeout );
      if (len == 0) {
         /* no more data before end of headers */
         /* scanner should detect end of data on last byte */
         /* if premature end of file, probably connection is lost */
         /* or client sent a badly formatted header and body */
         status = S_400_BADREQ;
         break;
      }

      /* feed bytes one by one into header scanner until no more are left */
      l = rqp->Lin; /* point to line buffer */
      while (len > 0) {
         /* feed scanner one byte at a time */
         done = em_RequestScanHeaders( rqp,iop, sep, *l );
         if (done != 0) {
            if (done == SCAN_COMPLETE) {
               /* done, just break, status is ok */
               status = S_200_OK;
               break;
            }
            else if (done == SCAN_ERROR) {
               /* error detected, set status and break */
               status = S_400_BADREQ;
               break;
            }
            else {
               /* fail here, shouldn't get here */
               status = S_500_SRVERR;
               break;
            }
         }
         /* next byte */
         l++;
         /* decrement count */
         len--;
      }
   }
   return status;
}

/* header scanner, could be changed to table driven */
long em_RequestScanHeaders( em_request_t *rqp, em_io_t *iop, em_session_t *sep, char c )
{
   long done;

   done = 0;
   switch (rqp->scan) {
   /*========================= */
   /* beginning of a header */
   /*========================= */
   case SCAN_BEGIN:
      if (c == 0x0d) {
         /* delimiting CR of CRLF pair, looking for LF to end scan */
         rqp->scan = SCAN_SCANEND;
      }
      else if (c == 0x0a) {
         /* for tolerance, accept single LF as indicating CRLF */
         /* so this is end of headers                          */
         rqp->scan = done = SCAN_COMPLETE;
      }
      else {
         /* beginning of a header, start storing fieldname */
         *( rqp->fp ) = c;
         rqp->fp++;
         rqp->fc++;
         rqp->scan = SCAN_FIELDNAME;
      }
      break;
   /*========================= */
   /* receiving field-name */
   /*========================= */
   case SCAN_FIELDNAME:
      if (c == ':') {
         /* end of fieldname */
         /* delimit the fieldname, ':' is not stored */
         *( rqp->fp ) = '\0';

         /* start looking for field value */
         rqp->fp = rqp->fieldvalue;   /* start position in header buffer */
         rqp->fc = 0;                 /* header character count */
         rqp->scan = SCAN_FIELDVALUE; /* new state */
      }
      else if (c <= 0x20) {
         /* any control character is an error */
         rqp->scan = done = SCAN_ERROR;
      }
      else {
         /* valid character */
         /* not end, store character in fieldname */
         /* truncate it if it is too long */
         if (rqp->fc < ( EM_MAXNAME - 1 )) {
            *( rqp->fp ) = c;
            rqp->fp++;
            rqp->fc++;
         }
      }
      break;
   /*========================= */
   /* receiving field-value */
   /*========================= */
   case SCAN_FIELDVALUE:
      if (c == 0x0d) {
         /* start looking for end of this header */
         rqp->scan = SCAN_HDREND;
      }
      else if (c == 0x0a) {
         /* for tolerance, accept single LF as indicating CRLF */
         /* the header is done, delimit it */
         *( rqp->fp ) = '\0';

         /* process the header */
         em_RequestProcHeader( rqp,iop,sep );

         /* reinitialize for next scan */
         em_RequestScanInit( rqp );
      }
      else {
         /* not end, store character in current fieldvalue position */
         /* truncate it if it is too long */
         if (rqp->fc < ( EM_MAXVALUE - 1 )) {
            *( rqp->fp ) = c;
            rqp->fp++;
            rqp->fc++;
         }
      }
      break;
   /*============================ */
   /* end of one header, get next */
   /*============================ */
   case SCAN_HDREND:
      if (c == 0x0a) {
         /* the header is done, delimit it */
         *( rqp->fp ) = '\0';

         /* process the header */
         em_RequestProcHeader( rqp,iop, sep );

         /* reinitialize for next scan */
         em_RequestScanInit( rqp );
      }
      else {
         /* error */
         rqp->scan = done = SCAN_ERROR;
      }
      break;
   /*================================== */
   /* no more headers, quit             */
   /*================================== */
   case SCAN_SCANEND:
      /* previous char was final <CR>, now looking for <LF> */
      if (c == 0x0a) {
         /* got delimiting LF, end of headers */
         rqp->scan = done = SCAN_COMPLETE;
      }
      else {
         /* error */
         rqp->scan = done = SCAN_ERROR;
      }
      break;
   /*========================= */
   /* default is an error */
   /*========================= */
   default:
      /* error */
      rqp->scan = done = SCAN_ERROR;
      break;
   }

   return done;
}

/* given a header string, figure out what it is and set request variables appropriately */
/* uses brute force compares, could be changed to token scanner                         */
void em_RequestProcHeader( em_request_t *rqp, em_io_t *iop, em_session_t *sep )
{
   char *val;
   char *nam;

   /* use locals to point to character fields */
   nam = rqp->fieldname;
   val = rqp->fieldvalue;

   /* skip leading whitespace of field value            */
   /* easier done here than adding new state in scanner */
   while (isspace( *val )) {
      val++;
   }

   /* translate string to lower case since browsers differ */
   /* on what they send                                         */
   em_strlwr( nam );

   /* compare various supported field types and handle appropriately */
   if (strcmp( nam, "user-agent" ) == 0) {
      /* store user agent name */
      strcpy( rqp->UserAgent, val );
   }
   else {
   /* do nothing, other headers are ignored */
   }
}

/* initialize the header line scanner */
void em_RequestScanInit( em_request_t *rqp )
{
   rqp->scan = SCAN_BEGIN;     /* start state */
   rqp->fp   = rqp->fieldname; /* start position in header buffer */
   rqp->fc   = 0;              /* header character count */
}
