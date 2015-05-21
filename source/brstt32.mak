#
# Borland C++ IDE generated makefile
# Generated 8/2/99 at 7:49:32 PM 
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
LinkerLocalOptsAtW32_BRSTT32dlib =  -Tpd -aa -V4.0 -c -LC:\BC5\LIB
ResLocalOptsAtW32_BRSTT32dlib = 
BLocalOptsAtW32_BRSTT32dlib = 
CompInheritOptsAt_BRSTT32dlib = -IC:\BC5\INCLUDE 
LinkerInheritOptsAt_BRSTT32dlib = -x
LinkerOptsAt_BRSTT32dlib = $(LinkerLocalOptsAtW32_BRSTT32dlib)
ResOptsAt_BRSTT32dlib = $(ResLocalOptsAtW32_BRSTT32dlib)
BOptsAt_BRSTT32dlib = $(BLocalOptsAtW32_BRSTT32dlib)

#
# Dependency List
#
Dep_brstt32 = \
   BRSTT32.lib

brstt32 : BccW32.cfg $(Dep_brstt32)
  echo MakeNode

BRSTT32.lib : BRSTT32.dll
  $(IMPLIB) $@ BRSTT32.dll


Dep_BRSTT32ddll = \
   dll.obj\
   brstt32.def\
   sttbr.res

BRSTT32.dll : $(Dep_BRSTT32ddll)
  $(ILINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_BRSTT32dlib) $(LinkerInheritOptsAt_BRSTT32dlib) +
C:\BC5\LIB\c0d32.obj+
dll.obj
$<,$*
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32.lib
brstt32.def
sttbr.res

|
dll.obj :  dll.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_BRSTT32dlib) $(CompInheritOptsAt_BRSTT32dlib) -o$@ dll.cpp
|

sttbr.res :  port\sttbr.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_BRSTT32dlib) $(CompInheritOptsAt_BRSTT32dlib)  -FO$@ port\sttbr.rc
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
-H=STT32SE.csm
-WD
-v-
-R-
-k-
-H-
| $@


