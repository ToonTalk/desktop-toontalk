#
# Borland C++ IDE generated makefile
# Generated 6/27/2001 at 11:34:05 AM 
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
LinkerLocalOptsAtW32_ClickMedexe =  -Tpe -aa -V4.0 -c -LC:\BC5\LIB
ResLocalOptsAtW32_ClickMedexe = 
BLocalOptsAtW32_ClickMedexe = 
CompInheritOptsAt_ClickMedexe = -IC:\BC5\INCLUDE -DTT_ENGLISH=1;TT_PORTUGUESE=0;TT_EUROPEAN_PORTUGUESE=0;TT_32=1
LinkerInheritOptsAt_ClickMedexe = -x
LinkerOptsAt_ClickMedexe = $(LinkerLocalOptsAtW32_ClickMedexe)
ResOptsAt_ClickMedexe = $(ResLocalOptsAtW32_ClickMedexe)
BOptsAt_ClickMedexe = $(BLocalOptsAtW32_ClickMedexe)

#
# Dependency List
#
Dep_click32 = \
   ClickMe.exe

click32 : BccW32.cfg $(Dep_click32)
  echo MakeNode

Dep_ClickMedexe = \
   clickme.obj\
   clickme.def\
   clickme.res

ClickMe.exe : $(Dep_ClickMedexe)
  $(ILINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_ClickMedexe) $(LinkerInheritOptsAt_ClickMedexe) +
C:\BC5\LIB\c0w32.obj+
clickme.obj
$<,$*
C:\BC5\LIB\bidsf.lib+
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32.lib
clickme.def
clickme.res

|
clickme.obj :  clickme.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ClickMedexe) $(CompInheritOptsAt_ClickMedexe) -o$@ clickme.cpp
|

clickme.res :  clickme.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_ClickMedexe) $(CompInheritOptsAt_ClickMedexe)  -FO$@ clickme.rc
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
-H=Klicka32.csm
-W
-v-
-R-
-k-
-O-d
-H-
| $@


