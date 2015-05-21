#
# Borland C++ IDE generated makefile
# Generated 11/11/2002 at 3:51:12 PM 
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
LinkerLocalOptsAtW32_ITVer22dlib =  -Tpd -aa -V4.0 -c -LC:\BC5\LIB
ResLocalOptsAtW32_ITVer22dlib = 
BLocalOptsAtW32_ITVer22dlib = 
CompInheritOptsAt_ITVer22dlib = -IC:\BC5\INCLUDE 
LinkerInheritOptsAt_ITVer22dlib = -x
LinkerOptsAt_ITVer22dlib = $(LinkerLocalOptsAtW32_ITVer22dlib)
ResOptsAt_ITVer22dlib = $(ResLocalOptsAtW32_ITVer22dlib)
BOptsAt_ITVer22dlib = $(BLocalOptsAtW32_ITVer22dlib)

#
# Dependency List
#
Dep_ittt32 = \
   ITVer22.lib

ittt32 : BccW32.cfg $(Dep_ittt32)
  echo MakeNode

ITVer22.lib : ITVer22.dll
  $(IMPLIB) $@ ITVer22.dll


Dep_ITVer22ddll = \
   italian.res\
   ustt32.def\
   dll.obj\
   ttus.res

ITVer22.dll : $(Dep_ITVer22ddll)
  $(ILINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_ITVer22dlib) $(LinkerInheritOptsAt_ITVer22dlib) +
C:\BC5\LIB\c0d32.obj+
dll.obj
$<,$*
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32.lib
ustt32.def
italian.res+
ttus.res

|
italian.res :  it\italian.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_ITVer22dlib) $(CompInheritOptsAt_ITVer22dlib)  -FO$@ it\italian.rc
|
dll.obj :  dll.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ITVer22dlib) $(CompInheritOptsAt_ITVer22dlib) -o$@ dll.cpp
|

ttus.res :  ttus.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_ITVer22dlib) $(CompInheritOptsAt_ITVer22dlib)  -FO$@ ttus.rc
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


