#
# Borland C++ IDE generated makefile
# Generated 11/11/2002 at 3:58:51 PM 
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
LinkerLocalOptsAtW32_ITSTT32dlib =  -Tpd -aa -V4.0 -c -LC:\BC5\LIB
ResLocalOptsAtW32_ITSTT32dlib = 
BLocalOptsAtW32_ITSTT32dlib = 
CompInheritOptsAt_ITSTT32dlib = -IC:\BC5\INCLUDE 
LinkerInheritOptsAt_ITSTT32dlib = -x
LinkerOptsAt_ITSTT32dlib = $(LinkerLocalOptsAtW32_ITSTT32dlib)
ResOptsAt_ITSTT32dlib = $(ResLocalOptsAtW32_ITSTT32dlib)
BOptsAt_ITSTT32dlib = $(BLocalOptsAtW32_ITSTT32dlib)

#
# Dependency List
#
Dep_itstt32 = \
   ITSTT32.lib

itstt32 : BccW32.cfg $(Dep_itstt32)
  echo MakeNode

ITSTT32.lib : ITSTT32.dll
  $(IMPLIB) $@ ITSTT32.dll


Dep_ITSTT32ddll = \
   sttit.res\
   usstt32.def\
   dll.obj

ITSTT32.dll : $(Dep_ITSTT32ddll)
  $(ILINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_ITSTT32dlib) $(LinkerInheritOptsAt_ITSTT32dlib) +
C:\BC5\LIB\c0d32.obj+
dll.obj
$<,$*
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32.lib
usstt32.def
sttit.res

|
sttit.res :  it\sttit.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_ITSTT32dlib) $(CompInheritOptsAt_ITSTT32dlib)  -FO$@ it\sttit.rc
|
dll.obj :  dll.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ITSTT32dlib) $(CompInheritOptsAt_ITSTT32dlib) -o$@ dll.cpp
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


