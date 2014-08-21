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

unsigned char buffer[16*100];
int main(int argc,char *argv[])
{
size_t i,j,s;
size_t len;
FILE   *f;

	if (argc != 2) {
		printf("%s\n",__TIMESTAMP__);
		printf("makehex <filename> >output\n");
		exit(0);
	}

	f = fopen(argv[1],"rb");
	if (f == 0) {
		printf("can't open %s\n",argv[1]);
		exit(1);
	}

	s = fread(buffer,1,1,f);
	if (s != 1) {
		printf("empty file %s\n",argv[1]);
		exit(1);
		}
	printf(" 0x%02x",buffer[0]);
   len = 1;	
	j   = 1;
	for(;;) {
		s = fread(buffer,1,sizeof(buffer),f);
		if (s == 0) {
			break;
		}

		i = 0;
		for(;s>0;s--) {
			len++;
			printf(",0x%02x",buffer[i]);
			i++;
			j++;
			if (j >= 16) {
				printf("\n");
				j = 0;
			}
		}
	}
	printf("\n/* ADD THIS DECLARATION TO THE APPROPRIATE INCLUDE FILE */\n");
	printf("/* extern const char filename_ext[%lu]; */\n",len);
	return 0;
}
