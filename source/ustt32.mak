#
# Borland C++ IDE generated makefile
# Generated 3/2/2002 at 6:46:55 PM 
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
LinkerLocalOptsAtW32_USVer22dlib =  -Tpd -aa -V4.0 -c -LC:\BC5\LIB
ResLocalOptsAtW32_USVer22dlib = 
BLocalOptsAtW32_USVer22dlib = 
CompInheritOptsAt_USVer22dlib = -IC:\BC5\INCLUDE 
LinkerInheritOptsAt_USVer22dlib = -x
LinkerOptsAt_USVer22dlib = $(LinkerLocalOptsAtW32_USVer22dlib)
ResOptsAt_USVer22dlib = $(ResLocalOptsAtW32_USVer22dlib)
BOptsAt_USVer22dlib = $(BLocalOptsAtW32_USVer22dlib)

#
# Dependency List
#
Dep_ustt32 = \
   USVer22.lib

ustt32 : BccW32.cfg $(Dep_ustt32)
  echo MakeNode

USVer22.lib : USVer22.dll
  $(IMPLIB) $@ USVer22.dll


Dep_USVer22ddll = \
   ustt32.def\
   dll.obj\
   english.res\
   ttus.res

USVer22.dll : $(Dep_USVer22ddll)
  $(ILINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_USVer22dlib) $(LinkerInheritOptsAt_USVer22dlib) +
C:\BC5\LIB\c0d32.obj+
dll.obj
$<,$*
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32.lib
ustt32.def
english.res+
ttus.res

|
dll.obj :  dll.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_USVer22dlib) $(CompInheritOptsAt_USVer22dlib) -o$@ dll.cpp
|

english.res :  english.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_USVer22dlib) $(CompInheritOptsAt_USVer22dlib)  -FO$@ english.rc
|
ttus.res :  ttus.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_USVer22dlib) $(CompInheritOptsAt_USVer22dlib)  -FO$@ ttus.rc
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


