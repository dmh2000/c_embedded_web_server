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
	struct _stat filestat;
	int          result;

	if (argc != 3) {
		printf("%s\n",__TIMESTAMP__);
		printf("makedec <filename> <dataname> >includefile\n");
		exit(1);
	}

	// Display some file statistics
	if (_stat( argv[1], &filestat ) == 0 ) {
		printf("extern unsigned char %s[%lu];\n",argv[2],filestat.st_size);
		result = 0;
	}
	else {
		printf("/* file not found %s\n */",argv[2]);
		result = 1;
	}

	return result;
}


