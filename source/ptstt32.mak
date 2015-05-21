#
# Borland C++ IDE generated makefile
# Generated 7/25/99 at 3:34:28 PM 
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
LinkerLocalOptsAtW32_PTSTT32dlib =  -Tpd -aa -V4.0 -c -LC:\BC5\LIB
ResLocalOptsAtW32_PTSTT32dlib = 
BLocalOptsAtW32_PTSTT32dlib = 
CompInheritOptsAt_PTSTT32dlib = -IC:\BC5\INCLUDE 
LinkerInheritOptsAt_PTSTT32dlib = -x
LinkerOptsAt_PTSTT32dlib = $(LinkerLocalOptsAtW32_PTSTT32dlib)
ResOptsAt_PTSTT32dlib = $(ResLocalOptsAtW32_PTSTT32dlib)
BOptsAt_PTSTT32dlib = $(BLocalOptsAtW32_PTSTT32dlib)

#
# Dependency List
#
Dep_ptstt32 = \
   PTSTT32.lib

ptstt32 : BccW32.cfg $(Dep_ptstt32)
  echo MakeNode

PTSTT32.lib : PTSTT32.dll
  $(IMPLIB) $@ PTSTT32.dll


Dep_PTSTT32ddll = \
   sttpt.res\
   dll.obj\
   portugal\ptstt32.def

PTSTT32.dll : $(Dep_PTSTT32ddll)
  $(ILINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_PTSTT32dlib) $(LinkerInheritOptsAt_PTSTT32dlib) +
C:\BC5\LIB\c0d32.obj+
dll.obj
$<,$*
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32.lib
portugal\ptstt32.def
sttpt.res

|
sttpt.res :  portugal\sttpt.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_PTSTT32dlib) $(CompInheritOptsAt_PTSTT32dlib)  -FO$@ portugal\sttpt.rc
|
dll.obj :  dll.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_PTSTT32dlib) $(CompInheritOptsAt_PTSTT32dlib) -o$@ dll.cpp
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


