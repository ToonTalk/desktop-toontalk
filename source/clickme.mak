#
# Borland C++ IDE generated makefile
# Generated 6/27/2001 at 11:35:25 AM 
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
IDE_LinkFLAGS =  -LC:\BC45\LIB
IDE_ResFLAGS = 
LinkerLocalOptsAtW16_clickmedexe =  -Twe -c -C -LC:\BC5\LIB
ResLocalOptsAtW16_clickmedexe =  -31
BLocalOptsAtW16_clickmedexe = 
CompInheritOptsAt_clickmedexe = -IC:\BC5\INCLUDE -DTT_ENGLISH=1
LinkerInheritOptsAt_clickmedexe = -x
LinkerOptsAt_clickmedexe = $(LinkerLocalOptsAtW16_clickmedexe)
ResOptsAt_clickmedexe = $(ResLocalOptsAtW16_clickmedexe)
BOptsAt_clickmedexe = $(BLocalOptsAtW16_clickmedexe)

#
# Dependency List
#
Dep_clickme = \
   clickme.exe

clickme : BccW16.cfg $(Dep_clickme)
  echo MakeNode

Dep_clickmedexe = \
   clickme.obj\
   clickme.def\
   clickme.res

clickme.exe : $(Dep_clickmedexe)
  $(TLINK)   @&&|
 /v $(IDE_LinkFLAGS) $(LinkerOptsAt_clickmedexe) $(LinkerInheritOptsAt_clickmedexe) +
C:\BC5\LIB\c0wl.obj+
clickme.obj
$<,$*
C:\BC5\LIB\import.lib+
C:\BC5\LIB\mathwl.lib+
C:\BC5\LIB\cwl.lib
clickme.def
clickme.res

|
clickme.obj :  clickme.cpp
  $(BCC)   -c @&&|
 $(CompOptsAt_clickmedexe) $(CompInheritOptsAt_clickmedexe) -o$@ clickme.cpp
|

clickme.res :  clickme.rc
  $(BRC) -R @&&|
 $(IDE_ResFLAGS) $(ROptsAt_clickmedexe) $(CompInheritOptsAt_clickmedexe)  -FO$@ clickme.rc
|
# Compiler configuration file
BccW16.cfg : 
   Copy &&|
-w
-R
-v
-vi
-H
-H=clickme.csm
-vGt-
-ml
-WS
-R-
-N-
-vi
-Od
-v-
-k-
-H-
| $@


