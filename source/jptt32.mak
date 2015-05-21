#
# Borland C++ IDE generated makefile
# Generated 1/27/00 at 10:35:39 AM 
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
LinkerLocalOptsAtW32_JPNT32dlib =  -Tpd -aa -V4.0 -c -LC:\BC5\LIB
ResLocalOptsAtW32_JPNT32dlib =  -l0
BLocalOptsAtW32_JPNT32dlib = 
CompInheritOptsAt_JPNT32dlib = -IC:\BC5\INCLUDE 
LinkerInheritOptsAt_JPNT32dlib = -x
LinkerOptsAt_JPNT32dlib = $(LinkerLocalOptsAtW32_JPNT32dlib)
ResOptsAt_JPNT32dlib = $(ResLocalOptsAtW32_JPNT32dlib)
BOptsAt_JPNT32dlib = $(BLocalOptsAtW32_JPNT32dlib)

#
# Dependency List
#
Dep_jptt32 = \
   JPNT32.lib

jptt32 : BccW32.cfg $(Dep_jptt32)
  echo MakeNode

JPNT32.lib : JPNT32.dll
  $(IMPLIB) $@ JPNT32.dll


Dep_JPNT32ddll = \
   ttus.res\
   japan\jpnt32\release\japan.res\
   jptt32.def\
   dll.obj

JPNT32.dll : $(Dep_JPNT32ddll)
  $(ILINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_JPNT32dlib) $(LinkerInheritOptsAt_JPNT32dlib) +
C:\BC5\LIB\c0d32.obj+
dll.obj
$<,$*
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32.lib
jptt32.def
ttus.res+
japan\jpnt32\release\japan.res

|
ttus.res :  ttus.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_JPNT32dlib) $(CompInheritOptsAt_JPNT32dlib)  -FO$@ ttus.rc
|
dll.obj :  dll.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_JPNT32dlib) $(CompInheritOptsAt_JPNT32dlib) -o$@ dll.cpp
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
-H=Amer32.csm
-WD
-v-
-R-
-k-
-H-
| $@


