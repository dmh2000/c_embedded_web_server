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
#include <string.h>
#include <time.h>
#include "ews.h"
#include "ewsapp.h"
#include "html.h"
#include "gif.h"
#include "javahtml.h"
#include "javaclas.h"
#include "javasrc.h"
#include "javagif.h"

/* delay function */
extern long ews_delay(unsigned long milliseconds);

struct UrlDispatch_s;

typedef long (*url_dispatch_fptr)(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep,const struct UrlDispatch_s *f);

typedef struct UrlDispatch_s {
   const char        *name;
   url_dispatch_fptr  head;
   const char        *data;
   unsigned long      size;
   } UrlDispatch_t;

extern const UrlDispatch_t url_dispatch_table[];

/* store the arguments in static data */
char textbox1[128] ="";
char textbox2[128] ="";
char checkbox1[4]  ="";
char checkbox2[4]  ="";
char radiogroup[4] ="";

const char *s_textbox1   = "textbox1";
const char *s_textbox2   = "textbox2";
const char *s_checkbox1  = "checkbox1";
const char *s_checkbox2  = "checkbox2";
const char *s_radiogroup = "radiogroup";

/* data is one super compile time string                         */
/* \r\n's are not required but make it easier to view the source */
const char index_html[] =
"<html>\r\n"
"<head>\r\n"
"<title>Embedded Web Server Index</title>\r\n"
"</head>\r\n"
"<body bgcolor=\"#FFFFFF\">\r\n"
"<h1 align=center>Embedded Systems Conference Session #425</h1>\r\n"
"<h2 align=center>Embedded Web Server</h2>\r\n"
"<h3 align=center>\r\n"
"David M. Howard Sierra Nevada Corporation<br>\r\n"
"daveh@hendrix.reno.nv.us  www.greatbasin.net/~daveh<br>\r\n"
"</h3>\r\n"
"<h5>\r\n"
"<ul>\r\n"
"<li><a href=\"table.html\">Table</a></li>\r\n"
"<li><a href=\"dynamic.html\">Dynamically Generated HTML File</a></li>\r\n"
"<li><a href=\"hexhtml.html\">Hex Encoded HTML File</a>          </li>\r\n"
"<li><a href=\"firebal2.gif\">Large Gif Graphic </a>             </li>\r\n"
"<li><a href=\"firetn.gif\">Thumbnail Gif Graphic</a>            </li>\r\n"
"<li><a href=\"java/example.html\">Java Hello World Program</a>  </li>\r\n"
"<li><a href=\"java/ct.html\">Java System Time</a>               </li>\r\n"
"<li><a href=\"java/streaming.html\">Java Streaming I/O</a>      </li>\r\n"
"</ul>\r\n"
"</h5>\r\n"
"</body>\r\n"
"</html>"
;

long app_SystimeBody(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep)
{
   long status;
   char tbuf[32];
   clock_t t;

   t = clock();
   sprintf(tbuf,"%08lx",t);

   /* write the body */
   status = iop->write(iop,sep->iod,tbuf,8);

   return status;
}

long app_SystimeHead(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep,const struct UrlDispatch_s *f)
{
   /* setup response record */
   rsp->content_length = 8;
   rsp->content_type   = em_GetHeaderString(EM_HDR_TEXT_PLAIN);
   rsp->body           = app_SystimeBody;
   rsp->p              = 0;
   return  0;
}

/* use this macro to simplify the coding of output of strings                  */
/* note it expects the variable names iop,sep and status to be what is defined */
/* in the function                                                             */
#define HTML_WRITE(data)                                     \
         {                                                   \
         status = iop->write(iop,sep->iod,data,(long)strlen(data));\
         if (status != S_200_OK) return status;              \
         }

/* hit count for this page */
static long hit_count = 0;

long app_DynamicHtmlBody(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep)
{
   char tbuf[64]; /* be sure this is long enough !! */
   long status;

   /* write the body, not that crlf's are not required at all */
   /* you could implement some better way of writing out the canned strings */
   /* perhaps with a char *array[] and loop thru it */
   HTML_WRITE("<HTML><BODY><html><head>");
   HTML_WRITE("<title>Example Dynamic HTML Page</title></head>");
   HTML_WRITE("<body bgcolor=\"#FFFFFF\">");

   /* insert some dynamic data, could be anything from the embedded system */
   hit_count++;
   sprintf(tbuf,"<H1> Hit Count For This Page = %ld</H1>",hit_count);
   HTML_WRITE(tbuf);
   HTML_WRITE("<H2>");
   HTML_WRITE(s_textbox1)  ;
   HTML_WRITE(" : ");
   HTML_WRITE(textbox1);
   HTML_WRITE("<BR>");
   HTML_WRITE(s_textbox2)  ;
   HTML_WRITE(" : ");
   HTML_WRITE(textbox2);
   HTML_WRITE("<BR>");
   HTML_WRITE(s_checkbox1) ;
   HTML_WRITE(" : ");
   HTML_WRITE(checkbox1);
   HTML_WRITE("<BR>");
   HTML_WRITE(s_checkbox2) ;
   HTML_WRITE(" : ");
   HTML_WRITE(checkbox2);
   HTML_WRITE("<BR>");
   HTML_WRITE(s_radiogroup);
   HTML_WRITE(" : ");
   HTML_WRITE(radiogroup);
   HTML_WRITE("<BR>");

   /* link to next example */
   HTML_WRITE("<A HREF=\"systime\">System Time</A>");
   HTML_WRITE("</body></html>");

   return status;
}

long app_DynamicHtmlHead(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep,const struct UrlDispatch_s *f)
{
   /* setup response record */
   rsp->content_length = 0; /* unknown, not required if you don't know it up front */
   rsp->content_type   = em_GetHeaderString(EM_HDR_TEXT_HTML);
   rsp->body           = app_DynamicHtmlBody;
   rsp->p              = 0;
   return  0;
}


/* functions to extract arguments */


long app_FormHandlerBody(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep)
{
   long status;
   char *s = rqp->Arg;

   /* extract the data from the argument string */
   /* ?textbox1=t1&textbox2=t2&checkbox1=ON&radiogroup=radio3&checkbox2=ON&Submit=Submit */
   if (*s != 0) {
      /* skip the ? */
      s++;
   }

   while(*s != 0) {
      /* use brute force to find the arguments that are set */
      /* can you improve this?                              */
      if (em_argcmp(s_textbox1,s) == 0) {
         s = em_argcopy(textbox1,s);
      }
      else if (em_argcmp(s_textbox2,s) == 0) {
         s = em_argcopy(textbox2,s);
      }
      else if (em_argcmp(s_checkbox1,s) == 0) {
         s = em_argcopy(checkbox1,s);
      }
      else if (em_argcmp(s_checkbox2,s) == 0) {
         s = em_argcopy(checkbox2,s);
      }
      else if (em_argcmp(s_radiogroup,s) == 0) {
         s = em_argcopy(radiogroup,s);
      }
      else {
         /* argument we don't care about, skip it */
         s = em_argnext(s);
      }
   }

   /* write the body, not that crlf's are not required at all */
   /* you could implement some better way of writing out the canned strings */
   /* perhaps with a char *array[] and loop thru it */
   HTML_WRITE("<HTML><BODY><html><head>");
   HTML_WRITE("<title>Form Result</title></head>");
   HTML_WRITE("<body bgcolor=\"#FFFFFF\">");
   HTML_WRITE("<h1>FORM RECEIVED OK</h1><BR>");
   HTML_WRITE("<h3>RAW Argument String :");
   HTML_WRITE(rqp->Arg);
   HTML_WRITE("<BR><BR><A HREF=\"dynamic.html\">Form Input Results</A>");
   HTML_WRITE("</h3></body></html>");

   return status;
}

long app_FormHandlerHead(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep,const struct UrlDispatch_s *f)
{
   /* setup response record */
   rsp->content_length = 0; /* unknown, not required if you don't know it up front */
   rsp->content_type   = em_GetHeaderString(EM_HDR_TEXT_HTML);
   rsp->body           = app_FormHandlerBody;
   rsp->p              = 0;
   return  0;
}



long app_StreamingSystimeBody(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep)
{
   long    status;
   char    tbuf[32];
   clock_t t;

   for(;;) {
      t = clock();
      sprintf(tbuf,"%08lx\n",t);

      /* write the body */
      status = iop->write(iop,sep->iod,tbuf,(long)strlen(tbuf));
      if (status != S_200_OK) {
         /* terminate when connection is closed at other end */
         break;
      }

      /* delay here between writes so as not to       */
      /* overrun the client                           */
      status = ews_delay(200);
      if (status != S_200_OK) {
         /* terminate if the delay function fails */
         break;
      }
   }

   return status;
}

long app_StreamingSystimeHead(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep,const struct UrlDispatch_s *f)
{
   /* setup response record */
   rsp->content_length = 0;
   rsp->content_type   = em_GetHeaderString(EM_HDR_TEXT_HTML);
   rsp->body           = app_StreamingSystimeBody;
   rsp->p              = 0;
   return  0;
}


long app_BinHtmlBody(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep)
{
   long status;
   const char *data;

   data = rsp->p;

   /* write the body */
   status = iop->write(iop,sep->iod,data,rsp->content_length);

   return status;
}

long app_BinHtmlHead(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep,const struct UrlDispatch_s *f)
{
   /* setup response record */
   rsp->content_length = (long)f->size;
   rsp->content_type   = em_GetHeaderString(EM_HDR_TEXT_HTML);
   rsp->body           = app_BinHtmlBody;
   rsp->p              = (void *)f->data;
   return  0;
}


long app_BinDataBody(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep)
{
   long status;
   const char *data;

   data = rsp->p;

   /* write the body */
   status = iop->write(iop,sep->iod,data,rsp->content_length);

   return status;
}

long app_BinTextPlainHead(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep,const struct UrlDispatch_s *f)
{
   /* setup response record */
   rsp->content_length = (long)f->size;
   rsp->content_type   = em_GetHeaderString(EM_HDR_TEXT_PLAIN);
   rsp->body           = app_BinDataBody;
   rsp->p              = (void *)f->data;
   return  0;
}

long app_BinImageGiffHead(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep,const struct UrlDispatch_s *f)
{
   /* setup response record */
   rsp->content_length = (long)f->size;
   rsp->content_type   = em_GetHeaderString(EM_HDR_IMAGE_GIF);
   rsp->body           = app_BinDataBody;
   rsp->p              = (void *)f->data;
   return  0;
}

long app_BinNotFoundHead(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep,const struct UrlDispatch_s *f)
{
   /* can't find a matching bin function */
   rsp->content_length = 0;
   rsp->content_type   = 0;
   rsp->body           = 0;
   rsp->p              = 0;
   return S_404_NOTFND;
}

long app_BinHead(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep)
{
   /* can't find a matching bin function */
   rsp->content_length = 0;
   rsp->content_type   = 0;
   rsp->body           = 0;
   rsp->p              = 0;
   return S_404_NOTFND;
}

long app_BinRequest(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep )
{
   size_t               urllen;
   const UrlDispatch_t *fp;

   /* search the list of bin functions until a match is found */
   /* at this point, it is guaranteed that the sBinUrl string */
   /* is a prefix to this string, so it can be skipped over   */
   urllen = strlen(iop->pagebin);
   fp = url_dispatch_table;
   while(fp->name != 0) {
      /* start comparison at next char after base url string */
      if (strcmp(fp->name,rqp->Url + urllen) == 0) {
         /* found a match, call it */
         return fp->head(rsp,rqp,iop,sep,fp);
      }
      fp++;
   }
   return app_BinNotFoundHead(rsp,rqp,iop,sep,0);
}

/* put this table last so it can see all the head functions        */
/* without always having to add a forward declaration for each one */
/*lint -e786 string concatenation in initializer */
/*lint -e750 local macros not used               */
/* use this for compiled in binary data from the 'makehex' program */
#define GIFFBIN(url,varname) {url,app_BinImageGiffHead,varname,sizeof(varname)}

/* use this one for compiled in text data from the 'makehex' program */
#define TEXTPLN(url,varname) {url,app_BinTextPlainHead,varname,sizeof(varname)}

/* use this one for HTML data that is compiled as strings, like index_html above */
/* (it subtracts 1 from the length so that the null terminator is not sent out   */
#define HTMLSTR(url,varname) {url,app_BinHtmlHead,varname,sizeof(varname)-1}

/* use this one for compiled in HTML data from the 'makehex' program */
/* no null terminator on these                                       */
#define HTMLHEX(url,varname) {url,app_BinHtmlHead,varname,sizeof(varname)}

/* if the data is:                                    */
/*  dynamically generated                             */
/*  you have to compute the content_length at runtime */
/*  you want to set up anything special               */
/* just set up the table entry manually like '/systime' or '/stream' */

const UrlDispatch_t url_dispatch_table[] = {
   HTMLSTR("/index.html",index_html),
   HTMLHEX("/hexhtml.html",hexhtml_html),
   HTMLHEX("/table.html",table_html),
   HTMLHEX("/exform.html",exform_html),

   GIFFBIN("/firebal2.gif",firebal2_gif),
   GIFFBIN("/firetn.gif"  ,firetn_gif),

   HTMLHEX("/java/ct.html",ct_html),
   TEXTPLN("/java/ct.class",ct_class),
   TEXTPLN("/java/ct.java",ct_java),

   HTMLHEX("/java/example.html" ,example_html),
   TEXTPLN("/java/example.class",example_class),
   TEXTPLN("/java/example.java" ,example_java),

   HTMLHEX("/java/streaming.html",streaming_html),
   TEXTPLN("/java/streaming.class",streaming_class),
   TEXTPLN("/java/readStream.class",readstream_class),
   TEXTPLN("/java/streaming.java",streaming_java),
   GIFFBIN("/java/Java.gif",java_gif),

   /* URL              HEAD FUNCTION             DATA POINTER DATA LENGTH */
   {"/dynamic.html",   app_DynamicHtmlHead,      0,           0},
   {"/formhndl.html",  app_FormHandlerHead,      0,           0},
   {"/systime",        app_SystimeHead,          0,           0},
   {"/stream",         app_StreamingSystimeHead, 0,           0},
   {0,0,0,0}
   };

   /*lint +e786 */
