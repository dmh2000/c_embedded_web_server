#include <string.h>
#include <time.h>
#include "ewsapp.h"

// max number of stored URLS
#define MAX_URLS   32 

typedef struct UrlDispatch_s {
   const char        *name;
   url_dispatch_fptr  head;
   const char        *data;
   unsigned long      size;
   const char        *link;
   } UrlDispatch_t;

extern UrlDispatch_t url_dispatch_table[MAX_URLS];

// =======================================================================
// SYSTEM HANDLERS
// =======================================================================
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
   int                  i;

   /* search the list of bin functions until a match is found */
   /* at this point, it is guaranteed that the sBinUrl string */
   /* is a prefix to this string, so it can be skipped over   */
   urllen = strlen(iop->pagebin);
   fp = url_dispatch_table;
   for(i=0;i<MAX_URLS;i++) {
        if (fp[i].name != 0) {
            /* start comparison at next char after base url string */
            if (strcmp(fp[i].name,rqp->Url + urllen) == 0) {
                /* found a match, call it and quit*/
                return fp[i].head(rsp,rqp,iop,sep,fp);
            }
        }
   }
   return app_BinNotFoundHead(rsp,rqp,iop,sep,0);
}

// dynamically add urls
void webAddUrl(const char *name,const char *linkname,url_dispatch_fptr head)
{
   UrlDispatch_t *fp;
   int   i;

   /* search the list of bin functions until a match is found */
   /* at this point, it is guaranteed that the sBinUrl string */
   /* is a prefix to this string, so it can be skipped over   */
   fp = url_dispatch_table;
   for(i=0;i<MAX_URLS;i++) {
        if (fp[i].name == 0) {
            // found a slot
            fp[i].name = name;
            fp[i].head = head;
            fp[i].link = linkname;
            fp[i].data = 0;
            fp[i].size = 0;
            break;
        }
   }
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
#define HTMLHEX(url,varname) {url,app_BinHtmlHead,(char *)varname,sizeof(varname)}

/* if the data is:                                    */
/*  dynamically generated                             */
/*  you have to compute the content_length at runtime */
/*  you want to set up anything special               */
/* just set up the table entry manually like '/systime' or '/stream' */

extern long app_TasksHead(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep,const struct UrlDispatch_s *f);
extern long app_IndexHead(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep,const struct UrlDispatch_s *f);

UrlDispatch_t url_dispatch_table[MAX_URLS] = {
   /* URL               HEAD FUNCTION             DATA POINTER DATA LENGTH LINKNAME*/
// STATIC MAPPINGS   
   {"/",                app_IndexHead,             0,           0,        "HOME"}, //  0
   {"/index.html",      app_IndexHead,             0,           0,        "HOME"}, //  1
   {"/tasks.html",      app_TasksHead,             0,           0,        "TASKS"}, //  2
// DYNAMIC MAPPINGS   
   {0,0,0,0,0},                                                                    //  3
   {0,0,0,0,0},                                                                    //  4
   {0,0,0,0,0},                                                                    //  5
   {0,0,0,0,0},                                                                    //  6
   {0,0,0,0,0},                                                                    //  7
   {0,0,0,0,0},                                                                    //  8
   {0,0,0,0,0},                                                                    //  9
   {0,0,0,0,0},                                                                    // 10
   {0,0,0,0,0},                                                                    // 11
   {0,0,0,0,0},                                                                    // 12
   {0,0,0,0,0},                                                                    // 13
   {0,0,0,0,0},                                                                    // 14
   {0,0,0,0,0},                                                                    // 15
   {0,0,0,0,0},                                                                    // 16
   {0,0,0,0,0},                                                                    // 17
   {0,0,0,0,0},                                                                    // 18
   {0,0,0,0,0},                                                                    // 19
   {0,0,0,0,0},                                                                    // 20
   {0,0,0,0,0},                                                                    // 21
   {0,0,0,0,0},                                                                    // 22
   {0,0,0,0,0},                                                                    // 23
   {0,0,0,0,0},                                                                    // 24
   {0,0,0,0,0},                                                                    // 25
   {0,0,0,0,0},                                                                    // 26
   {0,0,0,0,0},                                                                    // 27
   {0,0,0,0,0},                                                                    // 28
   {0,0,0,0,0},                                                                    // 29
   {0,0,0,0,0},                                                                    // 30
   {0,0,0,0,0}                                                                     // 31
   };

   /*lint +e786 */
