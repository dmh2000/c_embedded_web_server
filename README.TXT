NOTE : THE EXECUTABLE FILES IN THIS DISTRIBUTION HAVE BEEN VIRUS CHECKED
BUT NO CHECK IS PERFECT. RUN YOUR OWN VIRUS CHECK ON THEM BEFORE YOU
USE THEM!

==============================================================================
PKZIP THAT I USED DESTROYS THE LONG FILENAMES. GOTO DIRECTORY 'JAVA' 
AND RENAME THE FILES THERE ACCORDING TO THE FILE "FIXNAME.BAT". THOSE ARE
THE ONLY LONG FILENAMES
==============================================================================
BUILDING AND RUNNING THE PROTOTYPE EMBEDDED WEB SERVER ON WIN/95
1. you need Microsoft Visual C++/4.2 or later
// NEW ================================================
2. go to the directories java and html and using command line
   nmake, run the makefiles. This turns the web pages and java
   classes into compiled data structures. If you get some "include
   file not found" errors when you build the main program, you skipped
   this step.
// END OF NEW ==========================================
3. open the w95t.mak project file in directory w95t
4. rebuild all
5. run the program w95t from the command line prompt

NOTE : Only the files in directory w95t are system specific. everything
in 'src' and 'inc' is portable, and needs only a few ANSI header files.
'binapp.c' is portable. look at ewsw95t.c to find the system dependent
code.

to figure out the code, start at "int main(...)" in ewsw95t.c and follow
it thru. 
==============================================================================
ADDING A NEW STRING BASED HTML FILE

1. edit 'binapp.c'
2. look at the example data string "index_html". That is the model for
   setting up a static HTML data string.
3. create a new html data string following that model and give it a name.
4. goto the bottom of 'binapp.c' and add a Url dispatch table entry for
   that filename, using the model for index_html and the macro HTMLSTR
==============================================================================
ADDING A NEW DYNAMIC HTML OUTPUT

1. edit 'binapp.c'
2. goto the bottom of 'binapp.c' and find the table entry for 'dynamic.html'
3. make a new entry for the URL you want to add. note that the data pointer
  and data length are 0, since you will fill them in at runtime
4. find the function app_DynamicHtmlHead and DynamicHtmlBody, and using those
	as models, construct your dynamic HTML output function
==============================================================================
ADDING A NEW HTML FILE

1. copy the html file to directory "html"
2. add an entry with the filename on the HTM or GIF line at the top of the file
3. make clean
4. make
5. inf ile w95t\binapp.c, add a table entry for the new URL.
   Use the macros provided and commented at the end of that file.
==============================================================================
Adding a new java file
	do the same as above except you need the '.class' file as well as an html
	file. by convention the java stuff is in directory "java" and w95t\java.h and
	w95t\java.c. remember that the filename part of the url for a java.class file
	must be exactly as named by the java development tools or it won't load.
	for example /java/Clock2.class is OK, /java/clock2.class won't work.
===============================================================================
in general, the html.h/html.c and java.h/java.c are just conventions, and
	you can add new files or whatever as long as the declarations are visible
	to the bin_function_list table at the end of "binapp.c".
===============================================================================	
Most of the work you need to do to add or delete files in is "binapp.c". study the
source for that file. Trace from the  url table at the bottom of the file back to 
the functions invoked and figure out how those work. you shouldn't have to
change any other files.
===============================================================================
A set of presentation slides with more information will appear on the web
site eventually.

check http://www.greatbasin.net/~daveh for source code updates
pass any comments or suggestions or requests for help to
daveh@hendrix.reno.nv.us

=============================================================================
I apologize if you see a CTRL-Z at the end of any of these files. One of
my editors appends it and other editors show it. just delete it. 

