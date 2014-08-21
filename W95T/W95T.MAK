# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=w95t - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to w95t - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "w95t - Win32 Release" && "$(CFG)" != "w95t - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "W95t.mak" CFG="w95t - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "w95t - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "w95t - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "w95t - Win32 Debug"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "w95t - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : ".\w95t.exe"

CLEAN : 
	-@erase "$(INTDIR)\Binapp.obj"
	-@erase "$(INTDIR)\Ews001.obj"
	-@erase "$(INTDIR)\Ews002.obj"
	-@erase "$(INTDIR)\Ews003.obj"
	-@erase "$(INTDIR)\Ews004.obj"
	-@erase "$(INTDIR)\Ews005.obj"
	-@erase "$(INTDIR)\Ews006.obj"
	-@erase "$(INTDIR)\Ewsw95t.obj"
	-@erase "$(INTDIR)\gif.obj"
	-@erase "$(INTDIR)\html.obj"
	-@erase "$(INTDIR)\javaclas.obj"
	-@erase "$(INTDIR)\javagif.obj"
	-@erase "$(INTDIR)\javahtml.obj"
	-@erase "$(INTDIR)\javasrc.obj"
	-@erase "$(INTDIR)\Qw95t.obj"
	-@erase "$(OUTDIR)\w95t.map"
	-@erase ".\w95t.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\inc" /I "..\html" /I "..\java" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\inc" /I "..\html" /I "..\java" /D\
 "WIN32" /D "NDEBUG" /D "_CONSOLE" /Fp"$(INTDIR)/W95t.pch" /YX /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/W95t.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /map /machine:I386 /out:"w95t.exe"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:no /pdb:"$(OUTDIR)/w95t.pdb"\
 /map:"$(INTDIR)/w95t.map" /machine:I386 /out:"w95t.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Binapp.obj" \
	"$(INTDIR)\Ews001.obj" \
	"$(INTDIR)\Ews002.obj" \
	"$(INTDIR)\Ews003.obj" \
	"$(INTDIR)\Ews004.obj" \
	"$(INTDIR)\Ews005.obj" \
	"$(INTDIR)\Ews006.obj" \
	"$(INTDIR)\Ewsw95t.obj" \
	"$(INTDIR)\gif.obj" \
	"$(INTDIR)\html.obj" \
	"$(INTDIR)\javaclas.obj" \
	"$(INTDIR)\javagif.obj" \
	"$(INTDIR)\javahtml.obj" \
	"$(INTDIR)\javasrc.obj" \
	"$(INTDIR)\Qw95t.obj"

".\w95t.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "w95t - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : ".\w95t.exe"

CLEAN : 
	-@erase "$(INTDIR)\Binapp.obj"
	-@erase "$(INTDIR)\Ews001.obj"
	-@erase "$(INTDIR)\Ews002.obj"
	-@erase "$(INTDIR)\Ews003.obj"
	-@erase "$(INTDIR)\Ews004.obj"
	-@erase "$(INTDIR)\Ews005.obj"
	-@erase "$(INTDIR)\Ews006.obj"
	-@erase "$(INTDIR)\Ewsw95t.obj"
	-@erase "$(INTDIR)\gif.obj"
	-@erase "$(INTDIR)\html.obj"
	-@erase "$(INTDIR)\javaclas.obj"
	-@erase "$(INTDIR)\javagif.obj"
	-@erase "$(INTDIR)\javahtml.obj"
	-@erase "$(INTDIR)\javasrc.obj"
	-@erase "$(INTDIR)\Qw95t.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\w95t.map"
	-@erase "$(OUTDIR)\w95t.pdb"
	-@erase ".\w95t.exe"
	-@erase ".\w95t.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MT /W3 /Gm /GX /Zi /Od /I "..\inc" /I "..\html" /I "..\java" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MT /W3 /Gm /GX /Zi /Od /I "..\inc" /I "..\html" /I "..\java"\
 /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /Fp"$(INTDIR)/W95t.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/W95t.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo /subsystem:console /map /debug /machine:I386 /out:"w95t.exe"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib wsock32.lib /nologo\
 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)/w95t.pdb"\
 /map:"$(INTDIR)/w95t.map" /debug /machine:I386 /out:"w95t.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Binapp.obj" \
	"$(INTDIR)\Ews001.obj" \
	"$(INTDIR)\Ews002.obj" \
	"$(INTDIR)\Ews003.obj" \
	"$(INTDIR)\Ews004.obj" \
	"$(INTDIR)\Ews005.obj" \
	"$(INTDIR)\Ews006.obj" \
	"$(INTDIR)\Ewsw95t.obj" \
	"$(INTDIR)\gif.obj" \
	"$(INTDIR)\html.obj" \
	"$(INTDIR)\javaclas.obj" \
	"$(INTDIR)\javagif.obj" \
	"$(INTDIR)\javahtml.obj" \
	"$(INTDIR)\javasrc.obj" \
	"$(INTDIR)\Qw95t.obj"

".\w95t.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "w95t - Win32 Release"
# Name "w95t - Win32 Debug"

!IF  "$(CFG)" == "w95t - Win32 Release"

!ELSEIF  "$(CFG)" == "w95t - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Binapp.c
DEP_CPP_BINAP=\
	"..\html\gif.h"\
	"..\html\html.h"\
	"..\inc\ews.h"\
	"..\inc\ewsapp.h"\
	"..\java\javaclas.h"\
	"..\java\javagif.h"\
	"..\java\javahtml.h"\
	"..\java\javasrc.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	

"$(INTDIR)\Binapp.obj" : $(SOURCE) $(DEP_CPP_BINAP) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Qw95t.c
DEP_CPP_QW95T=\
	".\socketq.h"\
	

"$(INTDIR)\Qw95t.obj" : $(SOURCE) $(DEP_CPP_QW95T) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ewsw95t.c
DEP_CPP_EWSW9=\
	"..\inc\ews.h"\
	"..\inc\ewsapp.h"\
	".\socketq.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	

"$(INTDIR)\Ewsw95t.obj" : $(SOURCE) $(DEP_CPP_EWSW9) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=\projects\EmSysCon\ems8\Src\Ews006.c
DEP_CPP_EWS00=\
	"..\inc\ews.h"\
	"..\inc\ewsapp.h"\
	".\..\src\ewspvt.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	

"$(INTDIR)\Ews006.obj" : $(SOURCE) $(DEP_CPP_EWS00) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\projects\EmSysCon\ems8\Src\Ews002.c
DEP_CPP_EWS002=\
	"..\inc\ews.h"\
	"..\inc\ewsapp.h"\
	".\..\src\ewspvt.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	

"$(INTDIR)\Ews002.obj" : $(SOURCE) $(DEP_CPP_EWS002) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\projects\EmSysCon\ems8\Src\Ews003.c
DEP_CPP_EWS003=\
	"..\inc\ews.h"\
	".\..\src\ewspvt.h"\
	

"$(INTDIR)\Ews003.obj" : $(SOURCE) $(DEP_CPP_EWS003) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\projects\EmSysCon\ems8\Src\Ews004.c
DEP_CPP_EWS004=\
	"..\inc\ews.h"\
	".\..\src\ewspvt.h"\
	

"$(INTDIR)\Ews004.obj" : $(SOURCE) $(DEP_CPP_EWS004) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\projects\EmSysCon\ems8\Src\Ews005.c
DEP_CPP_EWS005=\
	"..\inc\ews.h"\
	

"$(INTDIR)\Ews005.obj" : $(SOURCE) $(DEP_CPP_EWS005) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\projects\EmSysCon\ems8\Src\Ews001.c
DEP_CPP_EWS001=\
	"..\inc\ews.h"\
	".\..\src\ewspvt.h"\
	

"$(INTDIR)\Ews001.obj" : $(SOURCE) $(DEP_CPP_EWS001) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\projects\EmSysCon\ems8\Html\html.c
DEP_CPP_HTML_=\
	"..\Html\exform.hex"\
	"..\Html\hexhtml.hex"\
	"..\html\html.h"\
	"..\Html\table.hex"\
	

"$(INTDIR)\html.obj" : $(SOURCE) $(DEP_CPP_HTML_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\projects\EmSysCon\ems8\Html\gif.c
DEP_CPP_GIF_C=\
	"..\Html\firebal2.gex"\
	"..\Html\firetn.gex"\
	

"$(INTDIR)\gif.obj" : $(SOURCE) $(DEP_CPP_GIF_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\projects\EmSysCon\ems8\Java\javasrc.c
DEP_CPP_JAVAS=\
	"..\Java\ct.jex"\
	"..\Java\example.jex"\
	"..\java\javasrc.h"\
	"..\Java\streaming.jex"\
	

"$(INTDIR)\javasrc.obj" : $(SOURCE) $(DEP_CPP_JAVAS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\projects\EmSysCon\ems8\Java\javaclas.c
DEP_CPP_JAVAC=\
	"..\Java\ct.cex"\
	"..\Java\example.cex"\
	"..\java\javaclas.h"\
	"..\Java\readstream.cex"\
	"..\Java\streaming.cex"\
	

"$(INTDIR)\javaclas.obj" : $(SOURCE) $(DEP_CPP_JAVAC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\projects\EmSysCon\ems8\Java\javahtml.c
DEP_CPP_JAVAH=\
	"..\Java\ct.hex"\
	"..\Java\example.hex"\
	"..\java\javahtml.h"\
	"..\Java\streaming.hex"\
	

"$(INTDIR)\javahtml.obj" : $(SOURCE) $(DEP_CPP_JAVAH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\projects\EmSysCon\ems8\Java\javagif.c
DEP_CPP_JAVAG=\
	"..\Java\java.gex"\
	"..\java\javagif.h"\
	

"$(INTDIR)\javagif.obj" : $(SOURCE) $(DEP_CPP_JAVAG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
