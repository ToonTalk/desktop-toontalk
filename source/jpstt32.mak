#
# Borland C++ IDE generated makefile
# Generated 10/13/99 at 9:37:29 AM 
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
LinkerLocalOptsAtW32_JPSTT32dlib =  -Tpd -aa -V4.0 -c -LC:\BC5\LIB
ResLocalOptsAtW32_JPSTT32dlib = 
BLocalOptsAtW32_JPSTT32dlib = 
CompInheritOptsAt_JPSTT32dlib = -IC:\BC5\INCLUDE 
LinkerInheritOptsAt_JPSTT32dlib = -x
LinkerOptsAt_JPSTT32dlib = $(LinkerLocalOptsAtW32_JPSTT32dlib)
ResOptsAt_JPSTT32dlib = $(ResLocalOptsAtW32_JPSTT32dlib)
BOptsAt_JPSTT32dlib = $(BLocalOptsAtW32_JPSTT32dlib)

#
# Dependency List
#
Dep_jpstt32 = \
   JPSTT32.lib

jpstt32 : BccW32.cfg $(Dep_jpstt32)
  echo MakeNode

JPSTT32.lib : JPSTT32.dll
  $(IMPLIB) $@ JPSTT32.dll


Dep_JPSTT32ddll = \
   sttjp.res\
   jpstt32.def\
   dll.obj

JPSTT32.dll : $(Dep_JPSTT32ddll)
  $(ILINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_JPSTT32dlib) $(LinkerInheritOptsAt_JPSTT32dlib) +
C:\BC5\LIB\c0d32.obj+
dll.obj
$<,$*
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32.lib
jpstt32.def
sttjp.res

|
sttjp.res :  sttjp.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_JPSTT32dlib) $(CompInheritOptsAt_JPSTT32dlib)  -FO$@ sttjp.rc
|
dll.obj :  dll.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_JPSTT32dlib) $(CompInheritOptsAt_JPSTT32dlib) -o$@ dll.cpp
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
-H=stt32us.csm
-WD
-v-
-R-
-k-
-H-
| $@


