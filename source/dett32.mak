#
# Borland C++ IDE generated makefile
# Generated 12/22/96 at 7:30:21 PM 
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCC32   = Bcc32 +BccW32.cfg 
BCC32I  = Bcc32i +BccW32.cfg 
TLINK32 = TLink32
TLIB    = TLib
BRC32   = Brc32
TASM32  = Tasm32
#
# IDE macros
#


#
# Options
#
IDE_LinkFLAGS32 =  -LD:\BC5\LIB
IDE_ResFLAGS32 = 
LinkerLocalOptsAtW32_DETT32dlib =  -Tpd -aa -V4.0 -c
ResLocalOptsAtW32_DETT32dlib = 
BLocalOptsAtW32_DETT32dlib = 
CompInheritOptsAt_DETT32dlib = -ID:\BC5\INCLUDE 
LinkerInheritOptsAt_DETT32dlib = -x
LinkerOptsAt_DETT32dlib = $(LinkerLocalOptsAtW32_DETT32dlib)
ResOptsAt_DETT32dlib = $(ResLocalOptsAtW32_DETT32dlib)
BOptsAt_DETT32dlib = $(BLocalOptsAtW32_DETT32dlib)

#
# Dependency List
#
Dep_dett32 = \
   DETT32.lib

dett32 : BccW32.cfg $(Dep_dett32)
  echo MakeNode

DETT32.lib : DETT32.dll
  $(IMPLIB) $@ DETT32.dll


Dep_DETT32ddll = \
   ttge.res\
   german.res\
   dett32.def\
   dll.obj

DETT32.dll : $(Dep_DETT32ddll)
  $(TLINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_DETT32dlib) $(LinkerInheritOptsAt_DETT32dlib) +
D:\BC5\LIB\c0d32.obj+
dll.obj
$<,$*
D:\BC5\LIB\import32.lib+
D:\BC5\LIB\cw32.lib
dett32.def
ttge.res+
german.res

|
ttge.res :  german\ttge.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_DETT32dlib) $(CompInheritOptsAt_DETT32dlib)  -FO$@ german\ttge.rc
|
german.res :  german\german.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_DETT32dlib) $(CompInheritOptsAt_DETT32dlib)  -FO$@ german\german.rc
|
dll.obj :  dll.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_DETT32dlib) $(CompInheritOptsAt_DETT32dlib) -o$@ dll.cpp
|

# Compiler configuration file
BccW32.cfg : 
   Copy &&|
-w
-R
-v
-vi
-H
-H=Swed32.csm
-WD
-v-
-R-
-k-
-H-
| $@


