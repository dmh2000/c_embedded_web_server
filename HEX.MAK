.SUFFIXES : $(SUFFIXES) .jav .java .class .cls .html .htm .gif .hex .gex .jex .cex

#default filenames
HTMLFILE=html
JAVASRC=javasrc
JAVACLASS=javaclas
GIFFILE=gif

.htm.hex:
	makehex $*.htm         >$*.hex
	makedec $*.htm $*_html >>$(HTMLFILE).h
	makedef $*.hex $*_html >>$(HTMLFILE).c
	
.html.hex:
	makehex $*.html         >$*.hex
	makedec $*.html $*_html >>$(HTMLFILE).h
	makedef $*.hex $*_html >>$(HTMLFILE).c
	
.gif.gex:
	makehex $*.gif         >$*.gex
	makedec $*.gif $*_gif  >>$(GIFFILE).h
	makedef $*.gex $*_gif  >>$(GIFFILE).c
	
.jav.jex:
	makehex $*.jav         >$*.jex
	makedec $*.jav $*_java >>$(JAVASRC).h
	makedef $*.jex $*_java >>$(JAVASRC).c
	
.java.jex:
	makehex $*.java         >$*.jex
	makedec $*.java $*_java >>$(JAVASRC).h
	makedef $*.jex $*_java >>$(JAVASRC).c
	
.cls.cex:
	makehex $*.cls            >$*.cex
	makedec $*.cls $*_class   >>$(JAVACLASS).h
	makedef $*.cex $*_class   >>$(JAVACLASS).c
	
.class.cex:
	makehex $*.class           >$*.cex
	makedec $*.class $*_class >>$(JAVACLASS).h
	makedef $*.cex $*_class   >>$(JAVACLASS).c
