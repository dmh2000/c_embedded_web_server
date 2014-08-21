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

/* ======================================================= */
/* HTTP DEFAULT REQUEST HANDLERS                           */
/* ======================================================= */
long em_FullRequest(em_workspace_t *wrk, em_session_t *sep)
{
   long result;

   if (strncmp(wrk->request.Url,wrk->system.pagebin,strlen(wrk->system.pagebin)) == 0) {
      result = app_BinRequest(&wrk->response,&wrk->request,&wrk->system,sep);
   }
   else {
      result = S_404_NOTFND;
   }

   return result;
}

long em_SimpleRequest( em_workspace_t *wrk,em_session_t *sep)
{
   return em_FullRequest(wrk,sep);
}

void em_ApplicationInit(em_app_t *app)
{
   /* initialize application object with default functions */
   app->simple_request = em_SimpleRequest;
   app->full_request   = em_FullRequest;
}
