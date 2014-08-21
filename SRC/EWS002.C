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
#include <stdio.h>
#include "ews.h"
#include "ewsapp.h"
#include "ewspvt.h"

/* these strings provide the entity body for request error messages */
/* this is the text that is displayed on the screen of the client   */
/* they need to be a fixed length of 47 bytes for this implementation */
/* but the clients don't care how long they are */
const char *const em_sStatusBody[] = {
   /*          1         2         3        4 */
   /* 123456789012345678901234567890123456789012345678 */
   "Status = 200 OK                                ", /*  1 */
   "Status = 201 Created                           ", /*  2 */
   "Status = 202 Accepted                          ", /*  3 */
   "Status = 204 No Content for this request       ", /*  4 */
   "Status = 301 Moved Permanently                 ", /*  5 */
   "Status = 302 Moved Temporarily                 ", /*  6 */
   "Status = 304 Not Modified                      ", /*  7 */
   "Status = 400 Bad Request from Browser          ", /*  8 */
   "Status = 401 Unauthorized                      ", /*  9 */
   "Status = 403 Forbidden                         ", /* 11 */
   "Status = 404 Not Found                         ", /* 12 */
   "Status = 500 Internal Server Error             ", /* 13 */
   "Status = 501 Not Implemented                   ", /* 14 */
   "Status = 502 Bad Gateway                       ", /* 15 */
   "Status = 503 Service Unavailable               ", /* 16 */
   };

/* these strings are the error code strings for the status header */
/* they are not displayed by the client, normally */
/* they are specified in the HTML 2.0 spec */
const char *const em_sErrorCode[] = {
   "200 OK    \r\n",
   "201 Created\r\n",
   "202 Accepted\r\n",
   "204 No Content\r\n",
   "301 Moved Permanently\r\n",
   "302 Moved Temporarily\r\n",
   "304 Not Modified\r\n",
   "400 Bad Request\r\n",
   "401 Unauthorized\r\n",
   "403 Forbidden\r\n",
   "404 Not Found\r\n",
   "500 Internal Server Error\r\n",
   "501 Not Implemented\r\n",
   "502 Bad Gateway\r\n",
   "503 Service Unavailable\r\n"
   };

/* version string for this version of the server */
const char s_http_version[] = "HTTP/1.0 ";

/* headers indexed by the headers indices */
const char * const em_sHeaders[] = {
   "Server: Ews/0.1\r\n",
   "Content-Type: text/html\r\n",
   "Content-Type: image/gif\r\n",
   "Content-Type: image/jpeg\r\n",
   "Content-Length: ",
   "Content-Length: 47\r\n",
   "Content-Type: text/plain\r\n",
   "Expires: Tue, 15 Nov 1994 08:21:31 GMT\r\n" /* use date before current date to prevent caching */
   };

/* get header text string , given the index */
const char *em_GetHeaderString(unsigned long index)
{
   const char *s;

   /* stop if index out of range */
   if (index >= (sizeof(em_sHeaders)/sizeof(const char *))) {
      s = "";
   }
   else {
      s = em_sHeaders[index];
   }

   /* return header string pointer */
   return s;
}

/* make the io write function a little simpler */
static long em_ServerWriteString( em_session_t *sep, em_io_t *iop,const char *s )
{
   long result;

   result = em_IoWrite(iop,sep->iod,s,(long)strlen(s));
   return result;
}

/* the server detected an error, indicated by status */
/* return a canned error message to the client       */
void  em_ServerRequestError( em_io_t *iop,em_session_t *sep,long rsp_status )
{
   long status;

   /* limit the status range to known statuses */
   if (( rsp_status < S_200_OK ) || ( S_503_SVCUNV < rsp_status )) {
      /* outside error code range, send internal server error status' */
      rsp_status = S_500_SRVERR;
   }

   /* compose canned output based on 'status' */
   status = S_200_OK;

   /* HTTP/1.0 */
   status = em_ServerWriteString( sep,iop, s_http_version );

   /* Status-Code Reason-Phrase */
   if (status == S_200_OK) {
      status = em_ServerWriteString( sep,iop,em_sErrorCode[rsp_status] );
   }

   /* Response-Header (Server) */
   if (status == S_200_OK) {
      status = em_ServerWriteString( sep,iop,em_GetHeaderString(EM_HDR_SERVER));
   }

   /* Content-Type */
   if (status == S_200_OK) {
      status = em_ServerWriteString( sep,iop,em_GetHeaderString(EM_HDR_TEXT_HTML));
   }

   /* Content-Length */
   if (status == S_200_OK) {
      status = em_ServerWriteString( sep,iop,em_GetHeaderString(EM_HDR_ERROR_LENGTH));
   }

   /* End of Headers */
   if (status == S_200_OK) {
      /* delimit the body from the headers with an extra crlf */
      status = em_ServerWriteString( sep,iop,"\r\n" );
   }

   /* Entity-body */
   if (status == S_200_OK) {
      /* write the body */
      status = em_ServerWriteString(sep,iop,em_sStatusBody[rsp_status]);
   }
}

/* generate and send response headers based on data received from application layer callback */
long em_ResponseSendHeaders(em_response_t *rsp,em_request_t *rqp,em_io_t *iop,em_session_t *sep,long rsp_status)
{
   long status;

   /*           1         2         3 */
   /* "123456789012345678901234567890 */
   char digits[64]; /* "content_length: xxxxxxxxxxxx0  */

   /* limit rsp_status to legal values */
   if (( rsp_status < S_200_OK ) || ( S_503_SVCUNV < rsp_status )) {
      /* outside error code range, send internal server error status' */
      rsp_status = S_500_SRVERR;
   }

   /* write version string = HTTP/1.0 */
   status = em_ServerWriteString(sep,iop, s_http_version);

   /* Status-Code Reason-Phrase = ... */
   if (status == S_200_OK) {
      status = em_ServerWriteString(sep,iop,em_sErrorCode[rsp_status]);
   }

   /* Response-Header (Server-type) */
   if (status == S_200_OK) {
      status = em_ServerWriteString(sep,iop,em_GetHeaderString(EM_HDR_SERVER));
   }

   /* Expires date                                      */
   if (status == S_200_OK) {
      status = em_ServerWriteString(sep,iop,em_GetHeaderString(EM_HDR_EXPIRES));
   }

   /* Content-Type */
   if (status == S_200_OK) {
      status = em_ServerWriteString(sep,iop,rsp->content_type);
   }

   /* Content-Length from application */
   if (status == S_200_OK) {
      /* if content length field is greater than 0, use it */
      /* otherwise there is no content length field        */
      if (rsp->content_length > 0) {
         /* get content length as a long and build the string here */

         /* WARNING : make sure variable 'digits' is long enough for all cases */
         sprintf(digits,"%s %ld\r\n",em_GetHeaderString(EM_HDR_CONTENT_LENGTH),rsp->content_length);

         /* write the content length field */
         status = em_ServerWriteString(sep,iop,digits);
      }
   }
   return status;
}

/* send response for request that had a body */
long  em_ResponseSendWithBody(em_response_t *rsp,em_request_t *rqp,em_io_t *iop,em_session_t *sep,long rsp_status)
{
   long status;

   /* send appropriate headers */
   status = em_ResponseSendHeaders(rsp,rqp,iop,sep,rsp_status);

   /* delimiter */
   if (status == S_200_OK) {
      /* delimit the body from the headers with an extra crlf */
      status = em_ServerWriteString(sep,iop,"\r\n");
   }

   /* body */
   if (status == S_200_OK) {
      /* write the body */
      status = rsp->body(rsp,rqp,iop,sep);
   }

   return status;
}

/* initialize a request data structure */
void em_RequestInit( em_request_t *reqp )
{
   memset( reqp, 0, sizeof( em_request_t ));
}

/* reinitialize a request data structure */
void em_RequestReInit( em_request_t *reqp )
{
   memset( reqp, 0, sizeof( em_request_t ));
}

/* initialize a response data structure */
void em_ResponseInit( em_response_t *resp )
{
   memset( resp, 0, sizeof( em_response_t ));
}

/* initialize a response data structure */
void em_ResponseReInit( em_response_t *resp )
{
   memset( resp, 0, sizeof( em_response_t ));
}

/* call the application layer response handler for a given request method */
long em_SessionRespond( em_workspace_t *wrk,em_session_t *sep,long timeout )
{
   long status = S_200_OK;
   em_app_t      *app;
   em_response_t *rsp;
   em_request_t  *rqp;
   em_io_t       *iop;

   if (wrk == 0) {
      /* internal server error */
      return S_500_SRVERR;
   }

   if (sep == 0) {
      /* internal server error */
      return S_500_SRVERR;
   }

   app = &wrk->application;
   iop = &wrk->system;
   rsp = &wrk->response;
   rqp = &wrk->request;

   /*=========================== */
   /* process request by method */
   /*=========================== */
   switch (rqp->method) {
   case METHOD_GET:
      /* METHOD=GET */
      /* HTTP VERSION? */
      if (rqp->version < HTTP_1_0) {
         /* no HTTP version string, it is a simple request from an 0.9 client */
         /* no headers, no body, process it as is */
         status = app->simple_request( wrk,sep );
      }
      else {
         /* HTTP version is 1.0 */
         /* it is a full request from an 1.x client */
         /* get request headers */
         /* no request body for GET method */
         /* pass request to Get function */

         /* get the rest of the message from server */
         status = em_RequestGETHeaders( rqp, iop, sep, timeout );
         if (status == S_200_OK) {
            /* a valid GET request was received, dispatch it to the application layer */
            status = app->full_request( wrk,sep );
         }

         /* return the response to the client */
         switch (status) {
         case S_200_OK:
            /* good response */
            status = em_ResponseSendWithBody(rsp,rqp,iop,sep,status);
            break;
         default:
            /* return failure response based on status code */
            em_ServerRequestError(iop,sep,status);
            break;
         }
      }
      break;
   default:
      /* unsupported method                      */
      /* return error code 501 'not implemented' */
      status = S_501_NOTIMP;

      /* return failure response based on status code */
      em_ServerRequestError(iop,sep,status);
      break;
   }

   return status;
}
