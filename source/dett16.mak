#
# Borland C++ IDE generated makefile
# Generated 3/20/97 at 8:49:45 AM 
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
LinkerLocalOptsAtW16_DETT16dlib =  -Twd -c -C
ResLocalOptsAtW16_DETT16dlib = 
BLocalOptsAtW16_DETT16dlib = 
CompInheritOptsAt_DETT16dlib = -ID:\BC5\INCLUDE 
LinkerInheritOptsAt_DETT16dlib = -x
LinkerOptsAt_DETT16dlib = $(LinkerLocalOptsAtW16_DETT16dlib)
ResOptsAt_DETT16dlib = $(ResLocalOptsAtW16_DETT16dlib)
BOptsAt_DETT16dlib = $(BLocalOptsAtW16_DETT16dlib)

#
# Dependency List
#
Dep_dett16 = \
   DETT16.lib

dett16 : BccW16.cfg $(Dep_dett16)
  echo MakeNode

DETT16.lib : DETT16.dll
  $(IMPLIB) $@ DETT16.dll


Dep_DETT16ddll = \
   dett16.def\
   dll.obj\
   german.res\
   ttge.res

DETT16.dll : $(Dep_DETT16ddll)
  $(TLINK)   @&&|
 /v $(IDE_LinkFLAGS) $(LinkerOptsAt_DETT16dlib) $(LinkerInheritOptsAt_DETT16dlib) +
D:\BC5\LIB\c0dl.obj+
dll.obj
$<,$*
D:\BC5\LIB\import.lib+
D:\BC5\LIB\mathwl.lib+
D:\BC5\LIB\cwl.lib
dett16.def
german.res+
ttge.res

|
dll.obj :  dll.cpp
  $(BCC)   -c @&&|
 $(CompOptsAt_DETT16dlib) $(CompInheritOptsAt_DETT16dlib) -o$@ dll.cpp
|

german.res :  german\german.rc
  $(BRC) -R @&&|
 $(IDE_ResFLAGS) $(ROptsAt_DETT16dlib) $(CompInheritOptsAt_DETT16dlib)  -FO$@ german\german.rc
|
ttge.res :  german\ttge.rc
  $(BRC) -R @&&|
 $(IDE_ResFLAGS) $(ROptsAt_DETT16dlib) $(CompInheritOptsAt_DETT16dlib)  -FO$@ german\ttge.rc
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


