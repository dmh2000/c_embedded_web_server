#include <vxworks.h>
#include <taskLib.h>
#include <sysLib.h>
#include <tickLib.h>
#include <taskLib.h>
#include <string.h>
#include <time.h>
#include "ews.h"
#include "ewsapp.h"
#include "log.h"
#include "sncnet.h"
 
//lint -e527 

// ============================================
// APPLICATION SPECIFIC HANDLERS
// ============================================
/* ========================================================================== */
/* HEADER                                                                     */
/* ========================================================================== */
const char *header = 
"<html>"
"<head>"
"<title>TGU : @0</title>"
"<meta http-equiv='Content-Type' content='text/html; charset=iso-8859-1'>"
"<style>"
"body         {font-family:monospace;}"
"td           {font-size:xx-small;padding-left: 10px;padding-right: 10px;}"
"td.hdr       {font-weight:bold;}"
"hr           {height: 1px;}"
"</style>"
"</head>"
""
"<body>"
"<h2>TRACKING GROUND UNIT : @0</h2>"
"<hr>"
"<a href='/index.html'>Home</a>&nbsp;&nbsp;<a href='/tasks.html'>Tasks</a>&nbsp;&nbsp;<a href='/status.html'>Status</a>"
"<hr>"
;
                  
void webWriteHeader(em_io_t *iop, em_session_t *sep,const char *title)
{
    const char *sub[1];
    
    sub[0] = title;
    (void)em_replace(sep->scratch,header,sizeof(sep->scratch),sub,1);
    (void)iop->write(iop,sep->iod,sep->scratch,(long)strlen(sep->scratch));
}

// =============================================
// HOME PAGE
// =============================================
long app_IndexBody(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep)
{
    const char *sub[1];
    int         status;
    
    SET_OVERRUN(sep);
    
    // write the header
    sub[0] = "HOME";
    (void)em_replace(sep->scratch,header,sizeof(sep->scratch),sub,1);
    HTML_WRITE(sep->scratch);
    
    HTML_WRITE("</body></html>");
    
    // check for overrun
    CHK_OVERRUN(sep);


    return status;
}

long app_IndexHead(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep,const struct UrlDispatch_s *f)
{
    /* setup response record */
    rsp->content_length = 0; /* unknown, not required if you don't know it up front */
    rsp->content_type   = em_GetHeaderString(EM_HDR_TEXT_HTML);
    rsp->body           = app_IndexBody;
    rsp->p              = 0;
    return  S_200_OK;
}

// ====================================================================================
// TASK DISPLAY
// ====================================================================================

int tb_comp(const void *a,const void *b)
{
    return ((TASK_DESC *)a)->td_priority - ((TASK_DESC *)b)->td_priority;
}

long app_TasksBody(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep)
{
    long status;
    int  count;
    int  i;    
    int  j;
    const char *sub[1];
    
    // STACK OVERRUN DANGER !!
    char      statbuf[32];
    TASK_DESC tb_data[32];
    int       tb_id[32];
    
    SET_OVERRUN(sep);

    // write the header
    sub[0] = "System Tasks";
    (void)em_replace(sep->scratch,header,sizeof(sep->scratch),sub,1);
    HTML_WRITE(sep->scratch);
    
    // get the list of task ids
    count = taskIdListGet(tb_id,32);
    if (count > 32) {
        return 0;
    }
    
    // get the task info for each task
    j = 0;
    for(i=0;i<count;i++) {
        status = taskInfoGet(tb_id[i],&tb_data[j]);
        if (status != ERROR) {
            j++;
        }
    }
    
    // sort by priority
    // j is number of valid tasks, now sort them by priority
    qsort(tb_data,(size_t)j,sizeof(TASK_DESC),tb_comp);

    // write the list of tasks    
    HTML_WRITE("<table>");
    sprintf(sep->scratch,"<tr><td width='5%%'>#</td><td width='10%%'>TID</td><td width='10%%'>NAME</td><td width='10%%'>PRIORITY</td><td width='10%%'>STATUS</td><td width='10%%'>STACK SIZE</td><td width='10%%'>STACK MAX</td></tr>");
    HTML_WRITE(sep->scratch);
    for(i=0;i<j;i++) {
        // get the data for this task
        (void)taskStatusString(tb_data[i].td_id,statbuf);
        
        // format the table row and write it
        sprintf(sep->scratch,"<tr><td width='5%%'>%d</td><td width='10%%'>%08x</td><td width='10%%'>%s</td><td width='10%%'>%d</td><td width='10%%'>%s</td><td width='10%%'>%08x</td><td width='10%%'>%08x</td></tr>",
                i,
                tb_data[i].td_id,
                tb_data[i].td_name,
                tb_data[i].td_priority,
                statbuf,
                tb_data[i].td_stackSize,
                tb_data[i].td_stackHigh
                );
        HTML_WRITE(sep->scratch);
    }
    
    HTML_WRITE("</table></body></html>");
    
    CHK_OVERRUN(sep);

    return S_200_OK;
}

// HIT COUNTER HEAD
long app_TasksHead(em_response_t *rsp,em_request_t *rqp,em_io_t *iop, em_session_t *sep,const struct UrlDispatch_s *f)
{
    /* setup response record */
    rsp->content_length = 0; /* unknown, not required if you don't know it up front */
    rsp->content_type   = em_GetHeaderString(EM_HDR_TEXT_HTML);
    rsp->body           = app_TasksBody;
    rsp->p              = 0;
    return  S_200_OK;
}
// ====================================================================================
// END TASK DISPLAY
// ====================================================================================
