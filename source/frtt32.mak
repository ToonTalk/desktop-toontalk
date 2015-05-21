#
# Borland C++ IDE generated makefile
# Generated 12/13/99 at 3:52:29 PM 
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
LinkerLocalOptsAtW32_FRNT32dlib =  -Tpd -aa -V4.0 -c -LC:\BC5\LIB
ResLocalOptsAtW32_FRNT32dlib = 
BLocalOptsAtW32_FRNT32dlib = 
CompInheritOptsAt_FRNT32dlib = -IC:\BC5\INCLUDE 
LinkerInheritOptsAt_FRNT32dlib = -x
LinkerOptsAt_FRNT32dlib = $(LinkerLocalOptsAtW32_FRNT32dlib)
ResOptsAt_FRNT32dlib = $(ResLocalOptsAtW32_FRNT32dlib)
BOptsAt_FRNT32dlib = $(BLocalOptsAtW32_FRNT32dlib)

#
# Dependency List
#
Dep_frtt32 = \
   FRNT32.lib

frtt32 : BccW32.cfg $(Dep_frtt32)
  echo MakeNode

FRNT32.lib : FRNT32.dll
  $(IMPLIB) $@ FRNT32.dll


Dep_FRNT32ddll = \
   ttfr.res\
   english.res\
   french\frtt32.def\
   dll.obj

FRNT32.dll : $(Dep_FRNT32ddll)
  $(ILINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_FRNT32dlib) $(LinkerInheritOptsAt_FRNT32dlib) +
C:\BC5\LIB\c0d32.obj+
dll.obj
$<,$*
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32.lib
french\frtt32.def
ttfr.res+
english.res

|
ttfr.res :  french\ttfr.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_FRNT32dlib) $(CompInheritOptsAt_FRNT32dlib)  -FO$@ french\ttfr.rc
|
english.res :  french\english.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_FRNT32dlib) $(CompInheritOptsAt_FRNT32dlib)  -FO$@ french\english.rc
|
dll.obj :  dll.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_FRNT32dlib) $(CompInheritOptsAt_FRNT32dlib) -o$@ dll.cpp
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


