#
# Borland C++ IDE generated makefile
# Generated 6/28/99 at 6:55:30 PM 
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
LinkerLocalOptsAtW16_StartTTdexe =  -Twe -c -C -LC:\BC5\LIB
ResLocalOptsAtW16_StartTTdexe =  -31
BLocalOptsAtW16_StartTTdexe = 
CompInheritOptsAt_StartTTdexe = -IC:\BC5\INCLUDE -DTT_32=0
LinkerInheritOptsAt_StartTTdexe = -x
LinkerOptsAt_StartTTdexe = $(LinkerLocalOptsAtW16_StartTTdexe)
ResOptsAt_StartTTdexe = $(ResLocalOptsAtW16_StartTTdexe)
BOptsAt_StartTTdexe = $(BLocalOptsAtW16_StartTTdexe)

#
# Dependency List
#
Dep_starttt = \
   StartTT.exe

starttt : BccW16.cfg $(Dep_starttt)
  echo MakeNode

Dep_StartTTdexe = \
   starttt.def\
   starttt.res\
   starttt.obj

StartTT.exe : $(Dep_StartTTdexe)
  $(TLINK)   @&&|
 /v $(IDE_LinkFLAGS) $(LinkerOptsAt_StartTTdexe) $(LinkerInheritOptsAt_StartTTdexe) +
C:\BC5\LIB\c0wl.obj+
starttt.obj
$<,$*
C:\BC5\LIB\import.lib+
C:\BC5\LIB\mathwl.lib+
C:\BC5\LIB\cwl.lib
starttt.def
starttt.res

|
starttt.res :  starttt.rc
  $(BRC) -R @&&|
 $(IDE_ResFLAGS) $(ROptsAt_StartTTdexe) $(CompInheritOptsAt_StartTTdexe)  -FO$@ starttt.rc
|
starttt.obj :  starttt.cpp
  $(BCC)   -c @&&|
 $(CompOptsAt_StartTTdexe) $(CompInheritOptsAt_StartTTdexe) -o$@ starttt.cpp
|

# Compiler configuration file
BccW16.cfg : 
   Copy &&|
-w
-R
-v
-vi
-H
-H=starttt.csm
-vGt-
-ml
-WS
-v-
-R-
-3
-k
-Hc-
-He-
-H-
| $@


