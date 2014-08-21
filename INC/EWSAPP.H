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
#ifndef __EWSAPP__
#define __EWSAPP__

/* only allow one target define to be active */
#include <stdio.h>
#include <sys/stat.h>

#define RESPONSE_BUFFER_SIZE 1024

/* structure for application specific response state */
typedef struct app_resp_t {
   /* master socket */
   long          msock;

   /* response data for file type response */
   void         *file;
   char          filename[1024];

   /* output buffer */
   char          buffer[RESPONSE_BUFFER_SIZE];
   } app_resp_t;

/* ems-bin handlers */
extern long app_BinRequest(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep);
#endif
