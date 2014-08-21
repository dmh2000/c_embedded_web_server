# MAKEFILE TO PERFORM LINT AND HEADER MODIFICATIONS
.SUFFIXES : .src .cpp .c .inc .h .caw .rtf .lob .obj .out .abs .hex .map .lst .tab .asm .i .lib

.c.out:
	lint ..\msc.lnt ..\ews.lnt -u -os($*.out) $*.c

.c.cbk:
	..\bin\header <$*.c >z
	copy $*.c $*.cbk
	copy z $*.c
	erase z

.h.hbk:
	..\bin\header <$*.h >z
	copy $*.h $*.hbk
	copy z $*.h
	erase z

.c.bak:
	cformat -h..\cvision.how $*.c
	