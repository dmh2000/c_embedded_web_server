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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define header_delimiter "/*H*/"

char *headers[] = {
#include "headtxt.h"
header_delimiter,
0
};

char buffer[256];

int main(int argc,char *argv[])
{
char **hdr = headers;
char  *s;
int   result = 0;

/* read and throw away lines up to the first instance of the header delimiter */	
while(!feof(stdin)) {
	s = fgets(buffer,sizeof(buffer)-1,stdin);
	if (s == 0) {
		/* no header delimiter, quit immediately */
		fprintf(stderr,"NO HEADER DELIMITER FOUND!\n");
		return 1;
		break;
		}
	if (strncmp(buffer,header_delimiter,strlen(header_delimiter)) == 0) {
		break;
		}
	}
	
/* write the headers out */
while(*hdr != 0) {
	fprintf(stdout,"%s\n",*hdr);
	hdr++;
	}

/* copy the rest of the file */
while(!feof(stdin)) {
	s = fgets(buffer,sizeof(buffer)-1,stdin);
	if (s == 0) {
		break;
		}
	fputs(buffer,stdout);
	}

return 0;
}
