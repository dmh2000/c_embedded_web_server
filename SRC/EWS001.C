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
#include "ews.h"
#include "ewspvt.h"

/* the embedded server is a single threaded server. it does not spawn        */
/* threads to handle multiple clients. Multiple clients can connect          */
/* but they queue up for service. This might lead to timeouts on the         */
/* client end.                                                               */
/* This property is important for an embedded system for predictability.     */
/* If multiple simultaneous clients are to be supported, the embedded_server */
/* function should be spawned multiple times as a task.                      */
/* the 'wait_for_request' function uses a queue to parcel out requests       */
/* to any number of waiting tasks.                                           */
/* each spawn of the embedded server allocates  a new workspace              */
void embedded_server(em_workspace_t *wrk)
{
   em_session_t   *sep;
   em_request_t   *rqp;
   em_response_t  *rsp;
   em_io_t        *iop;
   em_app_t       *app;
   em_session_t   session;
   long            status;
   long            read_timeout = 0;

   /* if incoming workspace pointer is 0, give up and return */
   if ( wrk == 0) {
      return;
   }

   /* initialize local pointers to workspace objects */
   sep = &session;
   rsp = &wrk->response;
   rqp = &wrk->request;
   iop = &wrk->system;
   app = &wrk->application;

   /* primary initialization of all objects */
   em_RequestInit(rqp);
   em_ResponseInit(rsp);
   em_SessionInit(sep);
   em_ApplicationInit(app);

   /* loop forever */
   for (;;) {
      /* reinit client for new session */
      em_SessionReInit( sep );

      /* reinit request for new session */
      em_RequestReInit( rqp );

      /* init response for new session */
      em_ResponseReInit( rsp );

      /* wait for an incoming client session */
      status = em_SessionConnect( sep,iop );
      if (status != S_200_OK) {
         /* status indicates failure or this is parent process */
         /* start over */
         continue;
      }

      /* read the client request with timeout */
      status = em_SessionRequest( sep, iop, rqp, read_timeout );
      if (status != S_200_OK) {
         /* force a disconnect */
         em_SessionForceDisconnect( sep,iop );

         /* start over */
         continue;
      }

      /* process the request to generate a response and send it to the server  */
      status = em_SessionRespond( wrk, sep,read_timeout );
      if (status != S_200_OK) {
         /* status indicates could not convert request to a response */
         /* too late to send response to client, because part of the */
         /* response may have already been sent, so just close down  */
         /* and let the client handle it */

         /* force a disconnect */
         em_SessionForceDisconnect( sep,iop );

         /* start over */
         continue;
      }

      /* give client handler chance to disconnect, may or may not */
      /* depending on state of stay-alive bit in client data      */
      em_SessionDisconnect( sep,iop );
   }

   /* never returns this way */
}
