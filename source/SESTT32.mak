#
# Borland C++ IDE generated makefile
# Generated 10/30/99 at 1:44:17 AM 
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
LinkerLocalOptsAtW32_SESTT32dlib =  -Tpd -aa -V4.0 -c -LC:\BC5\LIB
ResLocalOptsAtW32_SESTT32dlib = 
BLocalOptsAtW32_SESTT32dlib = 
CompInheritOptsAt_SESTT32dlib = -IC:\BC5\INCLUDE 
LinkerInheritOptsAt_SESTT32dlib = -x
LinkerOptsAt_SESTT32dlib = $(LinkerLocalOptsAtW32_SESTT32dlib)
ResOptsAt_SESTT32dlib = $(ResLocalOptsAtW32_SESTT32dlib)
BOptsAt_SESTT32dlib = $(BLocalOptsAtW32_SESTT32dlib)

#
# Dependency List
#
Dep_sestt32 = \
   SESTT32.lib

sestt32 : BccW32.cfg $(Dep_sestt32)
  echo MakeNode

SESTT32.lib : SESTT32.dll
  $(IMPLIB) $@ SESTT32.dll


Dep_SESTT32ddll = \
   sestt32.def\
   dll.obj\
   sttse.res

SESTT32.dll : $(Dep_SESTT32ddll)
  $(ILINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_SESTT32dlib) $(LinkerInheritOptsAt_SESTT32dlib) +
C:\BC5\LIB\c0d32.obj+
dll.obj
$<,$*
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32.lib
sestt32.def
sttse.res

|
dll.obj :  dll.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_SESTT32dlib) $(CompInheritOptsAt_SESTT32dlib) -o$@ dll.cpp
|

sttse.res :  sttse.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_SESTT32dlib) $(CompInheritOptsAt_SESTT32dlib)  -FO$@ sttse.rc
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


