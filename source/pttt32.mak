#
# Borland C++ IDE generated makefile
# Generated 3/11/2002 at 3:30:42 PM 
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
LinkerLocalOptsAtW32_PTVer22dlib =  -Tpd -aa -V4.0 -c -LC:\BC5\LIB
ResLocalOptsAtW32_PTVer22dlib = 
BLocalOptsAtW32_PTVer22dlib = 
CompInheritOptsAt_PTVer22dlib = -IC:\BC5\INCLUDE 
LinkerInheritOptsAt_PTVer22dlib = -x
LinkerOptsAt_PTVer22dlib = $(LinkerLocalOptsAtW32_PTVer22dlib)
ResOptsAt_PTVer22dlib = $(ResLocalOptsAtW32_PTVer22dlib)
BOptsAt_PTVer22dlib = $(BLocalOptsAtW32_PTVer22dlib)

#
# Dependency List
#
Dep_pttt32 = \
   PTVer22.lib

pttt32 : BccW32.cfg $(Dep_pttt32)
  echo MakeNode

PTVer22.lib : PTVer22.dll
  $(IMPLIB) $@ PTVer22.dll


Dep_PTVer22ddll = \
   ttpt.res\
   portugal.res\
   dll.obj\
   sett32.def

PTVer22.dll : $(Dep_PTVer22ddll)
  $(ILINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_PTVer22dlib) $(LinkerInheritOptsAt_PTVer22dlib) +
C:\BC5\LIB\c0d32.obj+
dll.obj
$<,$*
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32.lib
sett32.def
ttpt.res+
portugal.res

|
ttpt.res :  portugal\ttpt.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_PTVer22dlib) $(CompInheritOptsAt_PTVer22dlib)  -FO$@ portugal\ttpt.rc
|
portugal.res :  portugal\portugal.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_PTVer22dlib) $(CompInheritOptsAt_PTVer22dlib)  -FO$@ portugal\portugal.rc
|
dll.obj :  dll.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_PTVer22dlib) $(CompInheritOptsAt_PTVer22dlib) -o$@ dll.cpp
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


