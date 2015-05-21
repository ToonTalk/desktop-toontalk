#
# Borland C++ IDE generated makefile
# Generated 12/5/96 at 1:21:15 PM 
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCC     = Bcc +BccW16.cfg 
TLINK   = TLink
TLIB    = TLib
BRC     = Brc
TASM    = Tasm
#
# IDE macros
#


#
# Options
#
IDE_LinkFLAGS =  -LD:\BC5\LIB
IDE_ResFLAGS = 
LinkerLocalOptsAtW16_SESTT16dlib =  -Twd -c -C
ResLocalOptsAtW16_SESTT16dlib = 
BLocalOptsAtW16_SESTT16dlib = 
CompInheritOptsAt_SESTT16dlib = -ID:\BC5\INCLUDE 
LinkerInheritOptsAt_SESTT16dlib = -x
LinkerOptsAt_SESTT16dlib = $(LinkerLocalOptsAtW16_SESTT16dlib)
ResOptsAt_SESTT16dlib = $(ResLocalOptsAtW16_SESTT16dlib)
BOptsAt_SESTT16dlib = $(BLocalOptsAtW16_SESTT16dlib)

#
# Dependency List
#
Dep_SESTT16 = \
   SESTT16.lib

SESTT16 : BccW16.cfg $(Dep_SESTT16)
  echo MakeNode

SESTT16.lib : SESTT16.dll
  $(IMPLIB) $@ SESTT16.dll


Dep_SESTT16ddll = \
   sestt16.def\
   sttse.res\
   dll.obj

SESTT16.dll : $(Dep_SESTT16ddll)
  $(TLINK)   @&&|
 /v $(IDE_LinkFLAGS) $(LinkerOptsAt_SESTT16dlib) $(LinkerInheritOptsAt_SESTT16dlib) +
D:\BC5\LIB\c0dl.obj+
dll.obj
$<,$*
D:\BC5\LIB\import.lib+
D:\BC5\LIB\mathwl.lib+
D:\BC5\LIB\cwl.lib
sestt16.def
sttse.res

|
sttse.res :  sttse.rc
  $(BRC) -R @&&|
 $(IDE_ResFLAGS) $(ROptsAt_SESTT16dlib) $(CompInheritOptsAt_SESTT16dlib)  -FO$@ sttse.rc
|
dll.obj :  dll.cpp
  $(BCC)   -c @&&|
 $(CompOptsAt_SESTT16dlib) $(CompInheritOptsAt_SESTT16dlib) -o$@ dll.cpp
|

# Compiler configuration file
BccW16.cfg : 
   Copy &&|
-w
-R
-v
-vi
-H
-H=STT16se.csm
-ml
-WD
-v-
-R-
-k-
-H-
| $@


