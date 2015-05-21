#
# Borland C++ IDE generated makefile
# Generated 10/4/99 at 4:00:33 PM 
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCC32   = Bcc32 +BccW32.cfg 
BCC32I  = Bcc32i +BccW32.cfg 
TLINK32 = TLink32
ILINK32 = Ilink32
TLIB    = TLib
BRC32   = Brc32
TASM32  = Tasm32
#
# IDE macros
#


#
# Options
#
IDE_LinkFLAGS32 =  -Ld:\BC5\LIB
IDE_ResFLAGS32 = 
LinkerLocalOptsAtW32_startttdexe =  -Tpe -aa -V4.0 -c -LC:\BC5\LIB
ResLocalOptsAtW32_startttdexe = 
BLocalOptsAtW32_startttdexe = 
CompInheritOptsAt_startttdexe = -IC:\MSSDK\INCLUDE;C:\BC5\INCLUDE;C:\BC5\INCLUDE\WIN32 -DTT_32=1
LinkerInheritOptsAt_startttdexe = -x
LinkerOptsAt_startttdexe = $(LinkerLocalOptsAtW32_startttdexe)
ResOptsAt_startttdexe = $(ResLocalOptsAtW32_startttdexe)
BOptsAt_startttdexe = $(BLocalOptsAtW32_startttdexe)

#
# Dependency List
#
Dep_stt32 = \
   starttt.exe

stt32 : BccW32.cfg $(Dep_stt32)
  echo MakeNode

Dep_startttdexe = \
   starttt.res\
   starttt.obj

starttt.exe : $(Dep_startttdexe)
  $(ILINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_startttdexe) $(LinkerInheritOptsAt_startttdexe) +
C:\BC5\LIB\c0w32.obj+
starttt.obj
$<,$*
C:\BC5\LIB\bidsf.lib+
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32.lib

starttt.res

|
starttt.res :  starttt.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_startttdexe) $(CompInheritOptsAt_startttdexe)  -FO$@ starttt.rc
|
starttt.obj :  starttt.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_startttdexe) $(CompInheritOptsAt_startttdexe) -o$@ starttt.cpp
|

# Compiler configuration file
BccW32.cfg : 
   Copy &&|
-w
-R
-v
-WM-
-vi
-H
-H=stt32.csm
-W
-v-
-R-
| $@


