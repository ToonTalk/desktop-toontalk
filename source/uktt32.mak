#
# Borland C++ IDE generated makefile
# Generated 10/9/2002 at 2:14:43 PM 
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
LinkerLocalOptsAtW32_UKVer22dlib =  -Tpd -aa -V4.0 -c -LC:\BC5\LIB
ResLocalOptsAtW32_UKVer22dlib = 
BLocalOptsAtW32_UKVer22dlib = 
CompInheritOptsAt_UKVer22dlib = -IC:\BC5\INCLUDE 
LinkerInheritOptsAt_UKVer22dlib = -x
LinkerOptsAt_UKVer22dlib = $(LinkerLocalOptsAtW32_UKVer22dlib)
ResOptsAt_UKVer22dlib = $(ResLocalOptsAtW32_UKVer22dlib)
BOptsAt_UKVer22dlib = $(BLocalOptsAtW32_UKVer22dlib)

#
# Dependency List
#
Dep_uktt32 = \
   UKVer22.lib

uktt32 : BccW32.cfg $(Dep_uktt32)
  echo MakeNode

UKVer22.lib : UKVer22.dll
  $(IMPLIB) $@ UKVer22.dll


Dep_UKVer22ddll = \
   ustt32.def\
   dll.obj\
   english.res\
   ttuk.res

UKVer22.dll : $(Dep_UKVer22ddll)
  $(ILINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_UKVer22dlib) $(LinkerInheritOptsAt_UKVer22dlib) +
C:\BC5\LIB\c0d32.obj+
dll.obj
$<,$*
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32.lib
ustt32.def
english.res+
ttuk.res

|
dll.obj :  dll.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_UKVer22dlib) $(CompInheritOptsAt_UKVer22dlib) -o$@ dll.cpp
|

english.res :  uk\english.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_UKVer22dlib) $(CompInheritOptsAt_UKVer22dlib)  -FO$@ uk\english.rc
|
ttuk.res :  uk\ttuk.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_UKVer22dlib) $(CompInheritOptsAt_UKVer22dlib)  -FO$@ uk\ttuk.rc
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


