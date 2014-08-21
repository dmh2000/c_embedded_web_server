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
#include <dos.h>
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>


int main(int argc,char *argv[])
{
	int          result;

	if (argc != 3) {
		printf("%s\n",__TIMESTAMP__);
		printf("makedef <hexfilename> <dataname> >sourcefile\n");
		exit(1);
	}

	// Display some file statistics
	printf("unsigned char %s[] = {\n",argv[2]);
	printf("#include \"%s\"\n",argv[1]);
	printf("};\n");
	
	return 0;
}
