#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <winsock.h>
#include <assert.h>


int main(int argc,char *argv[])
{
	SOCKET				 s;
	struct sockaddr_in sin;
	int                err;
	int					 count;
	int					 c;
	char					 b;
	int                i;
	int                state;
   WORD           wVersionRequested;
   WSADATA        wsaData;
	struct hostent *hptr;

	/* initialize winsock */

   /* initialize winsock library */
   wVersionRequested = MAKEWORD(1, 1);

   err = WSAStartup(wVersionRequested, &wsaData);
   if (err != 0) {
      fprintf(stderr,"Can't initialize winsock lib\n");
      exit(1);
   }

   /* Confirm that the Windows Sockets DLL supports 1.1.*/
   /* Note that if the DLL supports versions greater */
   /* than 1.1 in addition to 1.1, it will still return */
   /* 1.1 in wVersion since that is the version we */
   /* requested. */

   if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
      /* Tell the user that we couldn't find a useable */
      /* winsock.dll. */
      WSACleanup();
      fprintf(stderr,"Winsock doesn't support version 1.1\n");
      exit(1);
   }

	/* clear socket address */

	memset(&sin,0,sizeof(sin));

	/* AF INET */
	sin.sin_family = AF_INET;

	/* set up hardwired port number into socket address */
	sin.sin_port = htons(80);

	/* set up hardwired ip address */
	if (argc == 2) {
		hptr = gethostbyname(argv[1]);
		if (hptr != 0) {
			memcpy((char *)&sin.sin_addr.s_addr,hptr->h_addr,hptr->h_length);
			}
		else {
			printf("can't resolve hostname\n");
			exit(1);
			}
	}
	else {
		sin.sin_addr.s_addr = inet_addr("172.16.0.6");
   }

	/* get a new socket with hardwired protocol type */
	s = socket(AF_INET,SOCK_STREAM,0);
	assert(s != INVALID_SOCKET);

	/* connect the socket */
	err = connect(s,(struct sockaddr *)&sin,sizeof(sin));
	if (err < 0) {
		perror("connect");
		exit(1);
	}
	else {
		printf("connected\n");
	}

	/* get keystrokes and send them until eof is entered */
	state = 0;
	while(state != 2) {
		c = getchar();

		// look for two cr/lf pairs to terminate
		switch(state) {
		case 0:
			if (c == '\n') {
				state = 1;
			}
			break;
		case 1:
			if (c == '\n') {
				state = 2;
			}
			else {
				state = 0;
			}
			break;
		default:
			state = 0;
			break;
		}

		/* send one byte on the socket */
		if (b == '\n') {
			b = '\r';
			count = send(s,&b,1,0);
			assert(count == 1);
			b = '\n';
			count = send(s,&b,1,0);
			assert(count == 1);
		}
		else {
			b = (char)c;
			count = send(s,&b,1,0);
			assert(count == 1);
		}
	}

	/* get reply data and print it out */
	i = 0;
	for(;;) {
		count = recv(s,&b,1,0);
		if (count != 1) {
			break;
		}
		/* output the character */
		putchar((int)b);

		/* count output bytes */
		i++;
	}

	printf("read bytes : %d\n",i);

	return 0;
}





