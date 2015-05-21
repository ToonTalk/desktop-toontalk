#
# Borland C++ IDE generated makefile
# Generated 1/14/97 at 7:59:27 PM 
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
LinkerLocalOptsAtW16_BRSTT16dlib =  -Twd -c -C
ResLocalOptsAtW16_BRSTT16dlib = 
BLocalOptsAtW16_BRSTT16dlib = 
CompInheritOptsAt_BRSTT16dlib = -ID:\BC5\INCLUDE 
LinkerInheritOptsAt_BRSTT16dlib = -x
LinkerOptsAt_BRSTT16dlib = $(LinkerLocalOptsAtW16_BRSTT16dlib)
ResOptsAt_BRSTT16dlib = $(ResLocalOptsAtW16_BRSTT16dlib)
BOptsAt_BRSTT16dlib = $(BLocalOptsAtW16_BRSTT16dlib)

#
# Dependency List
#
Dep_brstt16 = \
   BRSTT16.lib

brstt16 : BccW16.cfg $(Dep_brstt16)
  echo MakeNode

BRSTT16.lib : BRSTT16.dll
  $(IMPLIB) $@ BRSTT16.dll


Dep_BRSTT16ddll = \
   sestt16.def\
   dll.obj\
   sttbr.res

BRSTT16.dll : $(Dep_BRSTT16ddll)
  $(TLINK)   @&&|
 /v $(IDE_LinkFLAGS) $(LinkerOptsAt_BRSTT16dlib) $(LinkerInheritOptsAt_BRSTT16dlib) +
D:\BC5\LIB\c0dl.obj+
dll.obj
$<,$*
D:\BC5\LIB\import.lib+
D:\BC5\LIB\mathwl.lib+
D:\BC5\LIB\cwl.lib
sestt16.def
sttbr.res

|
dll.obj :  dll.cpp
  $(BCC)   -c @&&|
 $(CompOptsAt_BRSTT16dlib) $(CompInheritOptsAt_BRSTT16dlib) -o$@ dll.cpp
|

sttbr.res :  port\sttbr.rc
  $(BRC) -R @&&|
 $(IDE_ResFLAGS) $(ROptsAt_BRSTT16dlib) $(CompInheritOptsAt_BRSTT16dlib)  -FO$@ port\sttbr.rc
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


