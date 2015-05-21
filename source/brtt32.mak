#
# Borland C++ IDE generated makefile
# Generated 8/2/99 at 6:18:05 PM 
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
LinkerLocalOptsAtW32_BRNT32dlib =  -Tpd -aa -V4.0 -c -LC:\BC5\LIB
ResLocalOptsAtW32_BRNT32dlib = 
BLocalOptsAtW32_BRNT32dlib = 
CompInheritOptsAt_BRNT32dlib = -IC:\BC5\INCLUDE 
LinkerInheritOptsAt_BRNT32dlib = -x
LinkerOptsAt_BRNT32dlib = $(LinkerLocalOptsAtW32_BRNT32dlib)
ResOptsAt_BRNT32dlib = $(ResLocalOptsAtW32_BRNT32dlib)
BOptsAt_BRNT32dlib = $(BLocalOptsAtW32_BRNT32dlib)

#
# Dependency List
#
Dep_brtt32 = \
   BRNT32.lib

brtt32 : BccW32.cfg $(Dep_brtt32)
  echo MakeNode

BRNT32.lib : BRNT32.dll
  $(IMPLIB) $@ BRNT32.dll


Dep_BRNT32ddll = \
   dll.obj\
   sett32.def\
   port.res\
   ttbr.res

BRNT32.dll : $(Dep_BRNT32ddll)
  $(ILINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_BRNT32dlib) $(LinkerInheritOptsAt_BRNT32dlib) +
C:\BC5\LIB\c0d32.obj+
dll.obj
$<,$*
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32.lib
sett32.def
port.res+
ttbr.res

|
dll.obj :  dll.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_BRNT32dlib) $(CompInheritOptsAt_BRNT32dlib) -o$@ dll.cpp
|

port.res :  port\port.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_BRNT32dlib) $(CompInheritOptsAt_BRNT32dlib)  -FO$@ port\port.rc
|
ttbr.res :  port\ttbr.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_BRNT32dlib) $(CompInheritOptsAt_BRNT32dlib)  -FO$@ port\ttbr.rc
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
-H=Swed32.csm
-WD
-v-
-R-
-k-
-H-
| $@


