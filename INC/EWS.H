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
#ifndef __EWS_H__
#define __EWS_H__

/* HTTP 1.0 STATUS CODES                                                */
/* In all cases, a return code of S_200_OK, or 0 means success          */
/* these error codes index into a string array to generate the proper   */
/* response status line                                                 */
#define S_200_OK        0  /* OK, document to follow */
#define S_201_CREATE    1  /* created */
#define S_202_ACCEPT    2  /* accepted */
#define S_204_NOCONT    3  /* no content */
#define S_301_MOVPRM    4  /* moved permanently */
#define S_302_MOVTMP    5  /* moved temporarily */
#define S_304_NOTMOD    6  /* not modified */
#define S_400_BADREQ    7  /* bad request */
#define S_401_UNAUTH    8  /* unauthorized */
#define S_403_FORBID    9  /* forbidden */
#define S_404_NOTFND    10 /* not found */
#define S_500_SRVERR    11 /* internal server error */
#define S_501_NOTIMP    12 /* not implemented */
#define S_502_BADGAT    13 /* bad gateway */
#define S_503_SVCUNV    14 /* service unavailable */
                           /* internal error codes above S_503 result in S_500_SRVERR return to client */
#define EMERR_NOMEM     15 /* no memory available */
#define EMERR_READ      16 /* read error, lost connection */
#define EMERR_OPEN      17 /* error opening client, no connection */
#define EMERR_WRITE     18 /* error writing, no connection */

/* method opcodes */
#define METHOD_UNDEF -1
#define METHOD_GET    2

/* HTTP Versions */
#define HTTP_NOV   -1
#define HTTP_0_9   90
#define HTTP_1_0  100
#define HTTP_1_1  110

/* Text String Header indices */
#define EM_HDR_SERVER         0 /* server ID */
#define EM_HDR_TEXT_HTML      1 /* text/html */
#define EM_HDR_IMAGE_GIF      2 /* gif files */
#define EM_HDR_IMAGE_JPEG     3 /* jpeg files */
#define EM_HDR_CONTENT_LENGTH 4 /* app must append length digits */
#define EM_HDR_ERROR_LENGTH   5 /* error strings are all the same size */
#define EM_HDR_TEXT_PLAIN     6 /* plain text */
#define EM_HDR_EXPIRES        7 /* expires date header */

/* access function to header text strings */
const char *em_GetHeaderString(unsigned long index);

/* string length limits for allocation of char arrays */
#define EM_MAXMET   16                                              /* max stringlength of a method */
#define EM_MAXURL   256                                             /* max stringlength of a url */
#define EM_MAXARG   256                                             /* max stringlength of url arguments */
#define EM_MAXVER   16                                              /* max stringlength of version */
#define EM_MAXNAME  64                                              /* max stringlength of a header name */
#define EM_MAXVALUE 256                                             /* max stringlength of a header value */
#define EM_DATESIZE 32                                              /* Sun, 06 Nov 1994 08:49:37 GMT0 */
#define EM_MAXLIN   (EM_MAXMET + EM_MAXURL + EM_MAXARG + EM_MAXVER) /* max input line */

/* http content type indices for incoming data*/
#define EM_CT_APP_OCT_STR  0 /* octet stream */
#define EM_CT_TEXT_PLAIN   1 /* plain text */
#define EM_CT_UNSUPPORTED -1 /* not supported */

/* DATA TYPE FORWARD REFS */
struct em_io_s;
struct em_app_s;
struct em_session_s;
struct em_request_s;
struct em_response_s;
struct em_workspace_s;

/* Input/Output Adaptation layer  */
typedef struct em_io_s {
   /* pointer to application data */
   void*       p;

   /* application ews-bin file directory */
   const char *pagebin;

   /* =================== */
   /* SOCKET IO FUNCTIONS */
   /* =================== */
   /* open a connection to a client */
   long        (*open ) (struct em_io_s* io);

   /* close a connection to a client */
   void        (*close) (struct em_io_s* io,long iod);

   /* read bytes from a client      */
   long        (*read ) (struct em_io_s* io,long iod,char *buf,long len,long timeout);

   /* write bytes to a client       */
   long        (*write) (struct em_io_s* io,long iod,const char *buf,long len);

   /* get greenwich mean time as a string */
   const char* (*gmt)   (struct em_io_s* io,char* buffer);

   /* exit the server               */
   void        (*exit)  (struct em_io_s* io,long code);
   } em_io_t;

/* APPLICATION BACK END */
typedef struct em_app_s {
   /* method callbacks to application back end */
   long (*simple_request)   (struct em_workspace_s *wrk, struct em_session_s *sep);
   long (*full_request)   (struct em_workspace_s *wrk, struct em_session_s *sep);
   } em_app_t;

/* Session Descriptor */
typedef struct em_session_s {
   long             iod;       /* io descriptor from io (may need to be void * instead) */
   } em_session_t;

/* REQUEST */
typedef struct em_request_s {
   /* HEADER variables                                      */
   /* headers are decoded and stored in a convenient format */
   /* not necessarily as the raw string                     */
   /* headers not directly supported are thrown away        */
   int  method;                 /* method index, see method opcodes above   */
   int  version;                /* version index, see version defines above */
   char UserAgent[EM_MAXVALUE]; /* browser name             string          */

   /* request parameters as strings                   */
   char Lin[EM_MAXLIN]; /* raw input line             */
   char Met[EM_MAXMET]; /* extracted method string    */
   char Ver[EM_MAXVER]; /* extracted version string   */
   char Arg[EM_MAXARG]; /* extracted argument string  */
   char Url[EM_MAXURL]; /* extracted url string       */

   /* private state data for request processing                 */
   char  fieldname[EM_MAXNAME];   /* field name                 */
   char  fieldvalue[EM_MAXVALUE]; /* field value                */
   char* fp;                      /* current position in        */
   long  fc;                      /* scan count                 */
   long  scan;                    /* state variable for scanner */
   } em_request_t;

/* RESPONSE */
typedef struct em_response_s {
   /* response data filled in by application                       */
   long             content_length; /* number of bytes in response */
   const char      *content_type;   /* content type field          */

   /* application body function                                    */
   long  (*body)(struct em_response_s  *rsp, /* current response object */
                 struct em_request_s   *rqp, /* current request object  */
                 struct em_io_s        *iop, /* io object               */
                 struct em_session_s   *sep  /* current session object  */
                 );

   /* application pointer, to be used to keep application state  */
   /* across calls. application can put anything in it that fits */
   void*        p;
   } em_response_t;

/* ================================================== */
/* Workspace Definition                               */
/* Contains all data required by one instantiation of */
/* the embedded server                                */
/* eliminates need for dynamic memory allocation      */
/* ================================================== */
typedef struct em_workspace_s {
   em_io_t        system;
   em_app_t       application;
   em_request_t   request;
   em_response_t  response;
} em_workspace_t;

/* driver procedure, called by application processes or threads */
extern void embedded_server( em_workspace_t *wrk );

/* support functions for portability */
char *em_strlwr  (char *s);                     /* convert string to all lower case */
char *em_strccvt (char *s, char from, char to); /* change characters in string      */
char *em_unescape(char *s);                     /* change escape sequences to ASCII */
char *em_argcopy(char *d,char *s);              /* copy an argument. be sure d > s  */
int   em_argcmp(const char *template,const char *arg); /* return 0 if match */
char *em_argnext(char *s);                      /* skip over argument */

#endif /* __EWS_H__ */
