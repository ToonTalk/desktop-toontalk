#
# Borland C++ IDE generated makefile
# Generated 9/30/99 at 4:34:46 PM 
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
IDE_LinkFLAGS32 =  -LC:\BC5\LIB
IDE_ResFLAGS32 = 
LinkerLocalOptsAtW32_UKSTT32dlib =  -Tpd -aa -V4.0 -c -LC:\BC5\LIB
ResLocalOptsAtW32_UKSTT32dlib = 
BLocalOptsAtW32_UKSTT32dlib = 
CompInheritOptsAt_UKSTT32dlib = -IC:\BC5\INCLUDE -DTT_UK=1
LinkerInheritOptsAt_UKSTT32dlib = -x
LinkerOptsAt_UKSTT32dlib = $(LinkerLocalOptsAtW32_UKSTT32dlib)
ResOptsAt_UKSTT32dlib = $(ResLocalOptsAtW32_UKSTT32dlib)
BOptsAt_UKSTT32dlib = $(BLocalOptsAtW32_UKSTT32dlib)

#
# Dependency List
#
Dep_ukstt32 = \
   UKSTT32.lib

ukstt32 : BccW32.cfg $(Dep_ukstt32)
  echo MakeNode

UKSTT32.lib : UKSTT32.dll
  $(IMPLIB) $@ UKSTT32.dll


Dep_UKSTT32ddll = \
   usstt32.def\
   dll.obj\
   sttus.res

UKSTT32.dll : $(Dep_UKSTT32ddll)
  $(ILINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_UKSTT32dlib) $(LinkerInheritOptsAt_UKSTT32dlib) +
C:\BC5\LIB\c0d32.obj+
dll.obj
$<,$*
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32.lib
usstt32.def
sttus.res

|
dll.obj :  dll.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_UKSTT32dlib) $(CompInheritOptsAt_UKSTT32dlib) -o$@ dll.cpp
|

sttus.res :  sttus.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_UKSTT32dlib) $(CompInheritOptsAt_UKSTT32dlib)  -FO$@ sttus.rc
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
-H=stt32us.csm
-WD
-v-
-R-
-k-
-H-
| $@


