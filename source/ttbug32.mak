#
# Borland C++ IDE generated makefile
# Generated 9/21/2001 at 11:04:59 AM 
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
LinkerLocalOptsAtW32_ttbug32dexe =  -Tpe -aa -V4.0 -c -LC:\BC5\LIB;C:\MSSDK\LIB\BORLAND;C:\DXMEDIA\LIB; -S:100000
ResLocalOptsAtW32_ttbug32dexe = 
BLocalOptsAtW32_ttbug32dexe = 
CompInheritOptsAt_ttbug32dexe = -IC:\MSSDK\INCLUDE;C:\BC5\INCLUDE;C:\BC5\INCLUDE\WIN32;"C:\PROGRAM FILES\MICROSOFT SPEECH SDK\INCLUDE";C:\DXMEDIA\INCLUDE; -DINC_OLE2;TT_TEXT_TO_SPEECH=1;TT_INSTALL_SHIELD2=0;TT_DIRECT_INPUT=0;TT_BORLAND=1;
LinkerInheritOptsAt_ttbug32dexe = -x
LinkerOptsAt_ttbug32dexe = $(LinkerLocalOptsAtW32_ttbug32dexe)
ResOptsAt_ttbug32dexe = $(ResLocalOptsAtW32_ttbug32dexe)
BOptsAt_ttbug32dexe = $(BLocalOptsAtW32_ttbug32dexe)

#
# Dependency List
#
Dep_ttbug32 = \
   ttbug32.exe

ttbug32 : BccW32.cfg $(Dep_ttbug32)
  echo MakeNode

Dep_ttbug32dexe = \
   ..\mssdk\lib\borland\dinput.lib\
   ..\mssdk\lib\borland\dxguid.lib\
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
   file.obj\
   globals.obj\
   help.obj\
   house.obj\
   input.obj\
   java.obj\
   log.obj\
   main.obj\
   martian.obj\
   mouse.obj\
   network.obj\
   number.obj\
   pad.obj\
   picture.obj\
   prgrmmr.obj\
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

ttbug32.exe : $(Dep_ttbug32dexe)
  $(TLINK32) @&&|
 /v $(IDE_LinkFLAGS32) $(LinkerOptsAt_ttbug32dexe) $(LinkerInheritOptsAt_ttbug32dexe) +
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
file.obj+
globals.obj+
help.obj+
house.obj+
input.obj+
java.obj+
log.obj+
main.obj+
martian.obj+
mouse.obj+
network.obj+
number.obj+
pad.obj+
picture.obj+
prgrmmr.obj+
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
..\mssdk\lib\borland\dinput.lib+
..\mssdk\lib\borland\dxguid.lib+
C:\BC5\LIB\cg32.lib+
C:\BC5\LIB\bidsf.lib+
C:\BC5\LIB\ole2w32.lib+
C:\BC5\LIB\import32.lib+
C:\BC5\LIB\cw32.lib
tt32.def
toontalk.res

|
animate.obj :  animate.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ animate.cpp
|

bckgrnd.obj :  bckgrnd.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ bckgrnd.cpp
|

bird.obj :  bird.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ bird.cpp
|

blit.obj :  blit.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ blit.cpp
|

block.obj :  block.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ block.cpp
|

bomb.obj :  bomb.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ bomb.cpp
|

cache.obj :  cache.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ cache.cpp
|

city.obj :  city.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ city.cpp
|

cleanup.obj :  cleanup.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ cleanup.cpp
|

constant.obj :  constant.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ constant.cpp
|

cubby.obj :  cubby.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ cubby.cpp
|

dirty.obj :  dirty.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ dirty.cpp
|

dsutil.obj :  dsutil.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ dsutil.c
|

event.obj :  event.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ event.cpp
|

floor.obj :  floor.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ floor.cpp
|

file.obj :  file.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ file.cpp
|

globals.obj :  globals.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ globals.cpp
|

help.obj :  help.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ help.cpp
|

house.obj :  house.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ house.cpp
|

input.obj :  input.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ input.cpp
|

java.obj :  java.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ java.cpp
|

log.obj :  log.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ log.cpp
|

main.obj :  main.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ main.cpp
|

martian.obj :  martian.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ martian.cpp
|

mouse.obj :  mouse.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ mouse.cpp
|

network.obj :  network.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ network.cpp
|

number.obj :  number.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ number.cpp
|

pad.obj :  pad.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ pad.cpp
|

picture.obj :  picture.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ picture.cpp
|

prgrmmr.obj :  prgrmmr.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ prgrmmr.cpp
|

puzzle.obj :  puzzle.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ puzzle.cpp
|

quadtree.obj :  quadtree.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ quadtree.cpp
|

region.obj :  region.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ region.cpp
|

remote.obj :  remote.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ remote.cpp
|

robot.obj :  robot.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ robot.cpp
|

room.obj :  room.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ room.cpp
|

screen.obj :  screen.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ screen.cpp
|

sound.obj :  sound.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ sound.cpp
|

speak.obj :  speak.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ speak.cpp
|

sprite.obj :  sprite.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ sprite.cpp
|

sprites.obj :  sprites.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ sprites.cpp
|

text.obj :  text.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ text.cpp
|

thought.obj :  thought.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ thought.cpp
|

tools.obj :  tools.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ tools.cpp
|

truck.obj :  truck.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ truck.cpp
|

user.obj :  user.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ user.cpp
|

utils.obj :  utils.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ utils.cpp
|

wave.obj :  wave.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ wave.c
|

wingutil.obj :  wingutil.c
  $(BCC32) -P- -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ wingutil.c
|

winmain.obj :  winmain.cpp
  $(BCC32) -c @&&|
 $(CompOptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe) -o$@ winmain.cpp
|

toontalk.res :  toontalk.rc
  $(BRC32) -R @&&|
 $(IDE_ResFLAGS32) $(ROptsAt_ttbug32dexe) $(CompInheritOptsAt_ttbug32dexe)  -FO$@ toontalk.rc
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
-v
-vGd
-vGt
-vGc
-5
-Hc-
-W
-N
-Od
-vi
-He-
-r-
-A-
| $@


