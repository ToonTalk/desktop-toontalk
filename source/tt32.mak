#
# Borland C++ IDE generated makefile
# Generated 6/11/99 at 4:43:21 PM 
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
LinkerLocalOptsAtW32_tt32dexe =  -Tpe -aa -V4.0 -c -Gm -LC:\BC5\LIB
ResLocalOptsAtW32_tt32dexe = 
BLocalOptsAtW32_tt32dexe = 
CompInheritOptsAt_tt32dexe = -IC:\BC5\INCLUDE;C\MSSDK\INCLUDE;C:\BC5\INCLUDE\WIN32 -DINC_OLE2;TT_TEXT_TO_SPEECH=1;TT_DIRECT_INPUT=1;TT_BORLAND=1;
LinkerInheritOptsAt_tt32dexe = -m
LinkerOptsAt_tt32dexe = $(LinkerLocalOptsAtW32_tt32dexe)
ResOptsAt_tt32dexe = $(ResLocalOptsAtW32_tt32dexe)
BOptsAt_tt32dexe = $(BLocalOptsAtW32_tt32dexe)
CompLocalOptsAtW32_inputdcpp = 
LinkerLocalOptsAtW32_inputdcpp = 
ResLocalOptsAtW32_inputdcpp = 
BLocalOptsAtW32_inputdcpp = 
CompOptsAt_inputdcpp = $(CompOptsAt_tt32dexe) $(CompLocalOptsAtW32_inputdcpp)
CompInheritOptsAt_inputdcpp = -IC:\BC5\INCLUDE;C\MSSDK\INCLUDE;C:\BC5\INCLUDE\WIN32 -DINC_OLE2;TT_TEXT_TO_SPEECH=1;TT_DIRECT_INPUT=1;TT_BORLAND=1;
LinkerInheritOptsAt_inputdcpp = -m
LinkerOptsAt_inputdcpp = $(LinkerOptsAt_tt32dexe) $(LinkerLocalOptsAtW32_inputdcpp)
ResOptsAt_inputdcpp = $(ResOptsAt_tt32dexe) $(ResLocalOptsAtW32_inputdcpp)
BOptsAt_inputdcpp = $(BOptsAt_tt32dexe) $(BLocalOptsAtW32_inputdcpp)

#
# Dependency List
#
Dep_tt32 = \
   tt32.exe

tt32 : BccW32.cfg $(Dep_tt32)
  echo MakeNode

Dep_tt32dexe = \
   ..\mssdk\lib\borland\dsound.lib\
   ..\mssdk\lib\borland\ddraw.lib\
   ..\mssdk\lib\borland\dinput.lib\
   animate.obj\
   bckgrnd.obj\
   bird.obj\
   blit.obj\
   block.obj\
   bomb.obj\
   cache.obj\
   city.obj\
   cleanup.obj\
   constant.obj\
   cubby.obj\
   dirty.obj\
   dsutil.obj\
   event.obj\
   floor.obj\
   globals.obj\
   help.obj\
   house.obj\
   input.obj\
   java.obj\
   log.obj\
   main.obj\
   martian.obj\
   mouse.obj\
   number.obj\
   pad.obj\
   picture.obj\
   picture2.obj\
   prgrmmr.obj\
   prgrmm2.obj\
   puzzle.obj\
   quadtree.obj\
   region.obj\
   remote.obj\
   robot.obj\
   room.obj\
   screen.obj\
   sound.obj\
   speak.obj\
   sprite.obj\
   sprite2.obj\
   sprites.obj\
   text.obj\
   thought.obj\
   tools.obj\
   truck.obj\
   user.obj\
   utils.obj\
   wave.obj\
   wingutil.obj\
   winmain.obj\
   tt32.def\
   toontalk.res

tt32.exe : $(Dep_tt32dexe)
  $(ILINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_tt32dexe) $(LinkerInheritOptsAt_tt32dexe) +
C:\BC5\LIB\c0w32.obj+
animate.obj+
bckgrnd.obj+
bird.obj+
blit.obj+
block.obj+
bomb.obj+
cache.obj+
city.obj+
cleanup.obj+
constant.obj+
cubby.obj+
dirty.obj+
dsutil.obj+
event.obj+
floor.obj+
globals.obj+
help.obj+
house.obj+
input.obj+
java.obj+
log.obj+
main.obj+
martian.obj+
mouse.obj+
number.obj+
pad.obj+
picture.obj+
picture2.obj+
prgrmmr.obj+
prgrmm2.obj+
puzzle.obj+
quadtree.obj+
region.obj+
remote.obj+
robot.obj+
room.obj+
screen.obj+
sound.obj+
speak.obj+
sprite.obj+
sprite2.obj+
sprites.obj+
text.obj+
thought.obj+
tools.obj+
truck.obj+
user.obj+
utils.obj+
wave.obj+
wingutil.obj+
winmain.obj
$<,$*
..\mssdk\lib\borland\dsound.lib+
..\mssdk\lib\borland\ddraw.lib+
..\mssdk\lib\borland\dinput.lib+
C:\BC5\LIB\bidsf.lib+
C:\BC5\LIB\ole2w32.lib+
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32.lib
tt32.def
toontalk.res

|
animate.obj :  animate.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ animate.cpp
|

bckgrnd.obj :  bckgrnd.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ bckgrnd.cpp
|

bird.obj :  bird.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ bird.cpp
|

blit.obj :  blit.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ blit.cpp
|

block.obj :  block.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ block.cpp
|

bomb.obj :  bomb.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ bomb.cpp
|

cache.obj :  cache.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ cache.cpp
|

city.obj :  city.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ city.cpp
|

cleanup.obj :  cleanup.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ cleanup.cpp
|

constant.obj :  constant.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ constant.cpp
|

cubby.obj :  cubby.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ cubby.cpp
|

dirty.obj :  dirty.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ dirty.cpp
|

dsutil.obj :  dsutil.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ dsutil.c
|

event.obj :  event.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ event.cpp
|

floor.obj :  floor.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ floor.cpp
|

globals.obj :  globals.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ globals.cpp
|

help.obj :  help.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ help.cpp
|

house.obj :  house.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ house.cpp
|

input.obj :  input.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_inputdcpp) $(CompInheritOptsAt_inputdcpp) -o$@ input.cpp
|

java.obj :  java.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ java.cpp
|

log.obj :  log.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ log.cpp
|

main.obj :  main.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ main.cpp
|

martian.obj :  martian.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ martian.cpp
|

mouse.obj :  mouse.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ mouse.cpp
|

number.obj :  number.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ number.cpp
|

pad.obj :  pad.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ pad.cpp
|

picture.obj :  picture.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ picture.cpp
|

picture2.obj :  picture2.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ picture2.cpp
|

prgrmmr.obj :  prgrmmr.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ prgrmmr.cpp
|

prgrmm2.obj :  prgrmm2.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ prgrmm2.cpp
|

puzzle.obj :  puzzle.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ puzzle.cpp
|

quadtree.obj :  quadtree.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ quadtree.cpp
|

region.obj :  region.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ region.cpp
|

remote.obj :  remote.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ remote.cpp
|

robot.obj :  robot.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ robot.cpp
|

room.obj :  room.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ room.cpp
|

screen.obj :  screen.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ screen.cpp
|

sound.obj :  sound.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ sound.cpp
|

speak.obj :  speak.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ speak.cpp
|

sprite.obj :  sprite.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ sprite.cpp
|

sprite2.obj :  sprite2.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ sprite2.cpp
|

sprites.obj :  sprites.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ sprites.cpp
|

text.obj :  text.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ text.cpp
|

thought.obj :  thought.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ thought.cpp
|

tools.obj :  tools.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ tools.cpp
|

truck.obj :  truck.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ truck.cpp
|

user.obj :  user.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ user.cpp
|

utils.obj :  utils.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ utils.cpp
|

wave.obj :  wave.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ wave.c
|

wingutil.obj :  wingutil.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ wingutil.c
|

winmain.obj :  winmain.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe) -o$@ winmain.cpp
|

toontalk.res :  toontalk.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_tt32dexe) $(CompInheritOptsAt_tt32dexe)  -FO$@ toontalk.rc
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
-H=tt32.csm
-R-
-v-
-H-
-R-
-v-
-vGd-
-vGt-
-vGc-
-W
-k-
-OS
-O2
-Oc
-Oi
-Ov
-N-
-5
-r-
| $@


