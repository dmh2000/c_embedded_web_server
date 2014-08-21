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
* implied warranty.
*
* A supported version of this software including updates is available.
* Consulting services for porting, modification, web user interface design
* and implementation are available also.
* Contact David M. Howard at the above email address for details.
*/
/*H*/
#ifndef __EWSPVT_H__
#define __EWSPVT_H__

/* session handling functions */
extern void        em_SessionInit           (em_session_t *sep);
extern void        em_SessionReInit         (em_session_t *sep);
extern long        em_SessionConnect        (em_session_t *sep,em_io_t *iop);
extern void        em_SessionDisconnect     (em_session_t *sep,em_io_t *iop);
extern void        em_SessionForceDisconnect(em_session_t *sep,em_io_t *iop);
extern long        em_SessionRequest        (em_session_t *sep,em_io_t *iop,em_request_t* reqp,long timeout);

/* io front end macros make it a little easier to call the 'io->function functions */
#define em_IoOpen(io)                 ((io)->open(io))
#define em_IoClose(io,iod)            ((io)->close(io,iod))
#define em_IoWrite(io,iod,buf,len)    ((io)->write(io,iod,buf,len))
#define em_IoRead(io,iod,buf,len,tmo) ((io)->read(io,iod,buf,len,tmo))

extern long em_IoReadLine( em_io_t *io, long iod, char *buf, long maxlen, long timeout );

/* request functions */
extern void        em_RequestInit            (em_request_t* reqp);
extern void        em_RequestReInit          (em_request_t* reqp);
extern long        em_RequestGetHTTPVersion  (char *verp);
extern long        em_RequestGetMethod       (char* metp);
extern long        em_RequestScanHeaders     (em_request_t* reqp,em_io_t *iop,em_session_t *sep, char c);
extern void        em_RequestScanInit        (em_request_t *reqp);
extern void        em_RequestProcHeader      (em_request_t *reqp,em_io_t *iop,em_session_t *sep);
extern long        em_RequestGetFields       (em_request_t *reqp,em_io_t *iop,em_session_t *sep);
extern long        em_RequestGETHeaders      (em_request_t *reqp,em_io_t *iop,em_session_t *sep,long timeout);

/* application functions */
extern long        em_SessionRespond(em_workspace_t *wrk,
                                     em_session_t   *sep,
                                     long           timeout
                                     );

/* response functions */
extern void        em_ResponseInit           (em_response_t *rsp);
extern void        em_ResponseReInit         (em_response_t *rsp);
extern long        em_ResponseSendWithBody   (em_response_t *rsp,em_request_t *rqp,em_io_t *iop,em_session_t* sep,long rsp_status);
extern long        em_ResponseSendHeaders    (em_response_t *rsp,em_request_t *rqp,em_io_t *iop,em_session_t *sep,long rsp_status);

/* server functions */
extern void        em_ServerRequestError(em_io_t *iop,em_session_t* sep,long status);

/* application functions */
extern void        em_ApplicationInit(em_app_t *app);

#endif /* __EWSPVT_H__ */

