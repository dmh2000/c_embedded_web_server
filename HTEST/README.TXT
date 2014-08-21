to compile any of the C files, just set up your compiler environment
and compile. There is nothing tricky in any of these files, but the 
htest.c program wants socket support. the code here uses winsock
but change over to normal sockets should require only a couple of
fixes.

with Microsoft visual C++, I just do 
c:\bin>cl htest.c

getreq.txt is an example of what to type in.

to run htest.exe, it wants a hostname that points to a web server.
and then type in the get request. It is sort of a dumb telnet.
if you have telnet, just use it to open a connection to port 80
of a host with a web server. then you can do the same thing.

c:\bin>htest www.yahoo.com
GET /index.html HTTP/1.0<CR>
<CR>
...results come back
