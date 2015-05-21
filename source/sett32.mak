#
# Borland C++ IDE generated makefile
# Generated 6/26/2002 at 10:56:56 AM 
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
LinkerLocalOptsAtW32_SEVer22dlib =  -Tpd -aa -V4.0 -c -LC:\BC5\LIB
ResLocalOptsAtW32_SEVer22dlib = 
BLocalOptsAtW32_SEVer22dlib = 
CompInheritOptsAt_SEVer22dlib = -IC:\BC5\INCLUDE 
LinkerInheritOptsAt_SEVer22dlib = -x
LinkerOptsAt_SEVer22dlib = $(LinkerLocalOptsAtW32_SEVer22dlib)
ResOptsAt_SEVer22dlib = $(ResLocalOptsAtW32_SEVer22dlib)
BOptsAt_SEVer22dlib = $(BLocalOptsAtW32_SEVer22dlib)

#
# Dependency List
#
Dep_sett32 = \
   SEVer22.lib

sett32 : BccW32.cfg $(Dep_sett32)
  echo MakeNode

SEVer22.lib : SEVer22.dll
  $(IMPLIB) $@ SEVer22.dll


Dep_SEVer22ddll = \
   swedish.res\
   dll.obj\
   sett32.def\
   ttse.res

SEVer22.dll : $(Dep_SEVer22ddll)
  $(ILINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_SEVer22dlib) $(LinkerInheritOptsAt_SEVer22dlib) +
C:\BC5\LIB\c0d32.obj+
dll.obj
$<,$*
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32.lib
sett32.def
swedish.res+
ttse.res

|
swedish.res :  swedish\swedish.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_SEVer22dlib) $(CompInheritOptsAt_SEVer22dlib)  -FO$@ swedish\swedish.rc
|
dll.obj :  dll.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_SEVer22dlib) $(CompInheritOptsAt_SEVer22dlib) -o$@ dll.cpp
|

ttse.res :  ttse.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_SEVer22dlib) $(CompInheritOptsAt_SEVer22dlib)  -FO$@ ttse.rc
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
-H=Swed32.csm
-WD
-v-
-R-
-k-
-H-
| $@


