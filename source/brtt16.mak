#
# Borland C++ IDE generated makefile
# Generated 12/16/96 at 11:55:09 AM 
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
LinkerLocalOptsAtW16_BRTT16dlib =  -Twd -c -C
ResLocalOptsAtW16_BRTT16dlib = 
BLocalOptsAtW16_BRTT16dlib = 
CompInheritOptsAt_BRTT16dlib = -ID:\BC5\INCLUDE 
LinkerInheritOptsAt_BRTT16dlib = -x
LinkerOptsAt_BRTT16dlib = $(LinkerLocalOptsAtW16_BRTT16dlib)
ResOptsAt_BRTT16dlib = $(ResLocalOptsAtW16_BRTT16dlib)
BOptsAt_BRTT16dlib = $(BLocalOptsAtW16_BRTT16dlib)

#
# Dependency List
#
Dep_brtt16 = \
   BRTT16.lib

brtt16 : BccW16.cfg $(Dep_brtt16)
  echo MakeNode

BRTT16.lib : BRTT16.dll
  $(IMPLIB) $@ BRTT16.dll


Dep_BRTT16ddll = \
   dll.obj\
   brtt16.def\
   ttbr.res\
   port.res

BRTT16.dll : $(Dep_BRTT16ddll)
  $(TLINK)   @&&|
 /v $(IDE_LinkFLAGS) $(LinkerOptsAt_BRTT16dlib) $(LinkerInheritOptsAt_BRTT16dlib) +
D:\BC5\LIB\c0dl.obj+
dll.obj
$<,$*
D:\BC5\LIB\import.lib+
D:\BC5\LIB\mathwl.lib+
D:\BC5\LIB\cwl.lib
brtt16.def
ttbr.res+
port.res

|
dll.obj :  dll.cpp
  $(BCC)   -c @&&|
 $(CompOptsAt_BRTT16dlib) $(CompInheritOptsAt_BRTT16dlib) -o$@ dll.cpp
|

ttbr.res :  port\ttbr.rc
  $(BRC) -R @&&|
 $(IDE_ResFLAGS) $(ROptsAt_BRTT16dlib) $(CompInheritOptsAt_BRTT16dlib)  -FO$@ port\ttbr.rc
|
port.res :  port\port.rc
  $(BRC) -R @&&|
 $(IDE_ResFLAGS) $(ROptsAt_BRTT16dlib) $(CompInheritOptsAt_BRTT16dlib)  -FO$@ port\port.rc
|
# Compiler configuration file
BccW16.cfg : 
   Copy &&|
-w
-R
-v
-vi
-H
-H=Swed16.csm
-ml
-WD
-v-
-R-
-k-
-H-
| $@


