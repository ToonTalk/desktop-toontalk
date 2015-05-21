Release notes for ToonTalk(TM) beta release 7A.  April 18, 1997.
Copyright (c) 1992, 1997.  Ken Kahn, Animated Programs. All rights reserved.

1. Agreement.
You have received this beta version of the ToonTalk system in return for a
promise to provide Ken Kahn of Animated Programs with feedback.  Feedback can
include comments, suggestions, or bug reports regarding any aspect of the
ToonTalk system.  Feel free to show the system to anyone but you do not
have the right to distribute it to anyone else without prior permission.

The beta software will refuse to run on July 1.  Warnings will begin
to appear on June 15. All software is provided as is and no one is
liable for any bugs.

As a reward for providing good feedback, you are entitled to a free copy of
the final ToonTalk product after it is released.

2. How to provide feedback.
In order of preference:
A. Email. Kahn@CSLI.Stanford.edu or KenKahn@ToonTalk.com
B. Mail. 1748 Monticello Road, San Mateo, CA 94402
C. Fax 415-577-1784.
D. Phone. ONLY AS A LAST RESORT! 415-577-1785

3. Installation instructions.  

You need a PC running Microsoft Windows 3.1 or latter (Win95, Win
NT 3.51, and NT 4.0 work).
It works best in 256 color mode.  It can run with 4 megabytes
of memory but slowly.  It can run on fast 386 or slow 486
but it will be slow.  You'll need at least 10 megabytes of free hard
disk storage (8 more megabytes is needed for the narration of the
introductory tour).

To install just select "run" from Window's program manager and enter
a:setup  (or b: if your floppy drive is named b or d: if you are installing
from a CDROM and your drive is called d.).  If you are running Win95 with
autoplay enabled (the default) then installation will start automatically.

If you are installing from files obtained over the internet then run
beta7a16.exe to extract the files to a temporary directory.
Install it by running <temp directory>\setup.
You can delete the temporary directory and beta7a16.exe when done.

If you installed via floppies or the net and need space you may
delete intro.spk and you will only lose the narration for the
introductory tour.

There are additional narration files for many of the demos.  See
http://www.toontalk.com/English/narrate.htm for details.

The 32 bit version for Windows 95 and Windows NT 4.0 is available.
See http://www.toontalk.com/English/tt32.htm for details.

Good luck.

4. Customization options.

When you start up ToonTalk it asks your name.  You can supply your
name on the command line instead.  Nearly all of the options described
below can be controlled by the options notebook in ToonTalk (page 10
of your main notebook).
[Note if you installed an earlier version of ToonTalk then the file
<YourName>.bok may exist and prevent you from seeing this notebook.]

Alternatively you can customize ToonTalk by editing the Windows
Program Item called ToonTalk (Alt-Enter when the icon is selected.)
After the part that goes toontalk.exe you can add any of the following
options separated by spaces.  Except for the -n option below, the defaults
should be adequate.  You can add any of these command lines to the
"Start ToonTalk" (program item (Win 3.1) or shortcut (Win 95)) which calls
StartTT.exe.

-n <name>
Name can be any length but should contain spaces or special characters.
The first 8 letters of the name will be used as the file name for saved work.
Default name is "NoName".

-h <head>
Head can be 0 (bald), 1 (girl's hair), or 2 (hat). Default is 2.

-s <sound>
Sound can be 0 to turn off sound.  If the system crashes intermittently
it may be due to bad interactions between the sound driver and ToonTalk.
See if 0 fixes the problem and please report it.

-f <full screen>
Full screen can be 
  0 (small screen), 
  1 (full screen with Windows graphics shut down), or
  2 (full screen, Windows normal).
1 tends to look a bit nicer and on some machines is faster but it has the
problem that it often hangs when encountering a bug.  Sometimes when that
happens the machine needs to be rebooted.  1 also will work regardless of the
screen resolution or number of colors.  Also under 1 Windows <alt> commands
don't work.  1 does not work under Windows NT. 
To quit type pause then exit the normal Windows way.

-o <output log file name>
Everything done is recorded in the file.  Can be played back by -i.  Great
way of capturing a bug.  The extension .DMO is added.

-i <input log file name>
Redoes all the actions recorded when the -o option was used. -I behaves the
same but will slow down to the original recording speed if running on a
faster machine.

-c <city size>
City size by default is 10x10 blocks (each block holds 4 houses). 
Maximum city size is 255x255.  
If negative will initialize the city with random houses.

-r <reading speed percentage>
Reading speed controls how long text stays in a talk balloon.  100 percent
is the default.  Larger numbers makes the text stay a shorter time.

-l <letter size in talk balloons>
Default is 100 percent.

-e <font name>
Font used by Martian.
If it has a space like New Modern enter it as new_modern.

-m <memory devoted to cache>
Default is 100 (which uses 25% of free memory).
Larger numbers should speed up the system somewhat but leave less
memory for other purposes.  

-u <user profile maintained>
1 means to keep track in *.usr which operations user has performed and
customize help accordingly.  0 turns it off.  1 is the default.

-a <advice level>
Ranges from 0 to 5.  0 and the Martian will say everything possible.
5 and he will say nothing.  2 is the default.

-t <temporary user>
1 means do not read or generate user files for notebooks. 0 is the default.

-q <ToonTalk speed>
100 is default.  Higher numbers speeds up all animations.

-y <minimum frame duration in milliseconds>
0 is default.   50, for example, will result in a frame rate of 20 fps
maximum even if on a very fast computer.

-k <user is system debugger>
If -k 1 then Martian and <user>.log file contains more information about
any problems encountered.  Default is 0.

-z <Drop on stacks copy>
If -z 1 then the top element of stacks in tool box or notebooks can accept
things dropped on them.  Experimental feature that can speed up some actions
but can also be confusing.  Default is 0.

-bird <see birds returning outside>
If 0 then birds take no time to return after delivering to a nest when
the programmer is watching from outside.  Default is 1.

-j <running Japanese version of Windows 3.1>
1 means don't show title screen.  Seems to work around an incompatibility
between Win 3.1J and WinG both from Microsoft.  Usually the first time
text is displayed it will be blank.

-puzzle <file name>
Starts up ToonTalk running the puzzle contained in file name.

-next_puzzle 1
Starts up ToonTalk at the next puzzle by this user name.  First puzzle
if no puzzles have been solved by this user.

-subtitles <speed>
If showing a demo and subtitles exist then show them at a reading speed
whose default is 100.  If speed is 0 then no subtitles are shown.

There are more -- see http::/www.toontalk.com/English/command.htm.

5. Release notes.
Lots of stuff will change before the final product is ready.  They include:
- Record professional voice actor for the demo narrations.
- cities on the back of pictures.
- ability to move around on the floor to other parts of the room.
- ability to see stuff on the floor when standing in a room.
- ability to decorate both inside houses and out (and other objects too).
- much faster robots when off screen.
- joystick support.
- allow robots to work on things other than boxes.
- ToonTalk to Java translator (half done now).
- interface to external functionality like Windows API (maybe release 2)

Documentation.
Marty the Martian provides much of the documentation needed.  
You can find a manual at http://www.toontalk.com/English/doc.htm.

File list.
readme.txt -- this file
ttbeta7a.exe -- system executable
starttt.exe -- Convienent way of starting ToonTalk and its demos
m22.bt7 -- images at 320x240 resolution
resind.bt7 -- sounds and other data
dispdib.dll -- Microsoft's full screen extensions (used if -f 1 switch given)
intro.dmo -- introductory demo
intro.spk -- narration for intro.dmo (if deleted works without narration)   
explode.dmo -- demo of building exploding houses
swap.dmo -- demo of swapping 2 numbers if out of order
fibs.dmo -- demo of building Fibinocci numbers generator
fact.dmo -- demo of building a factorial program
append.dmo -- demo of building a list append program
account.dmo -- demo of building a simple bank account
pongact1.dmo -- part 1 of demo of building a ping pong like game
pongact2.dmo -- part 2 of demo of building a ping pong like game
pongact3.dmo -- part 3 of demo of building a ping pong like game
*.ust -- subtitle text for demos
*.pzl -- puzzle description files
*.dll -- various dynamically loaded libraries

If installing from CDROM then the following files are also available:
tt16.exe -- 16bit version of ToonTalk.
tt32.exe -- 32bit version of ToonTalk (runs only on Windows 95 or NT 4.0)
ttbug16.exe -- debugging version of 16 bit beta release
ttbug32.exe -- debugging version of tt32.exe
ClickMe.exe -- AutoPlay welcome application
m25.bt7 -- high resolution (640x480) data.  (32 bit version only)  
*.spk -- narration files for every dmo file.
USMarty.VCE -- voice data base for Marty (32 bit version only)

User files (where name is the name given in -n option) are created when
needed.
name.bok -- main notebook data
name.2 -- notebook on page 2 of main notebook
name.4 -- notebook on page 4 of main notebook
etc.
name.usr -- record of what user has done (used by help system)

WARNING: There is no guarantee that any of the files you create in this 
version will work in the next version of ToonTalk.  Sorry.

WinG files -- Microsoft's game extensions for Windows.
These are added to the system subdirectory of your Windows
directory.  (If you already have WinG installed, then this part of the
installation will do nothing.)  WING.DLL WING32.DLL WINGDE.DLL
WINGDIB.DRV WINGPAL.WND DVA.386

If DVA.386 was not previously installed on the your system (for example,
by Video for Windows), installation adds the following line to the
[386Enh] section of the SYSTEM.INI file, after which you must reboot
Windows for optimal WinG performance:

device=dva.386

DirectX.  These are a newer set of Microsoft extensions to Windows
for games.  The 32 bit version of ToonTalk requires these.

Tips.
The Martian will give a complete description of an item if you call for him
(via F1) while holding or pointing to an item.  This is particularly useful
for getting a description of what a robot does.

If you hold down the control button while copying the box inside a robot's
thought bubble, then blanks are restored to their original values.  This is
often a good way to obtain an example box for a robot to work on.

F8 currently stops all sprites and robots.  F8 again will start them
up again.  Will be replaced by a nice graphical switch.

The 'Pause' button is a convenient way to pause or quit a demo or
session with ToonTalk.

More tips available at http://www.toontalk.com/English/tips.htm

Bugs.
If a bug causes the system crashes you might try the following to help me
track it down.  If you have the CDROM release, then copy ttbug16.exe,
cg16.dll and toontalk.cgi to the ToonTalk directory.  You might want
to rename toontalk.exe so you can restore things afterwards.
Rename ttbug16.exe as toontalk.exe.  If an error is detected then please
send me toontalk.cgl and <yourname>.log.  Thanks.  If you don't have
CDROM release then send email to support@toontalk.com for help.

Also if you edit your \windows\toontalk.ini file to change the line
GenerateLogs=0
to 
GenerateLogs=3
Then then last 3 runs of ToonTalk will be rerunable.  They will be named
log1.dmo, log2.dmo, and log3.dmo.  Check creation dates to see which is the
most recent.  If you have captured a bug in a DMO file then please send it
to me.  It is a binary file -- if you need help sending it please send me
email.

Uninstall.
Sorry haven't implemented it yet.  You can delete the entire ToonTalk
directory (though you will lose anything you've done which is in
the Users subdirectory).
\windows\toontalk.ini can be deleted as can the ToonTalk program
group.  That should be it.  I recommend leaving the WinG stuff since
it is a Microsoft extension to Win 3.1, is small (300K) and used by
other programs.  These extensions are part of Windows 95.

Readings.
Check out http://www.toontalk.com.

There are 4 papers about ToonTalk available via anonymous ftp from
csli.stanford.edu.  See www.toontalk.com/papers.htm for details.

Known Problems.
-f 1 doesn't work (screen goes black) on Fahrenheit 1280 video boards
f they are in more than 256 color mode.  (And the other -f modes work
but are very slow.)  Change to 256 colors to fix this.

Legalities.
ToonTalk is a trademark of Animated Programs.  Everything which can be
copyrighted is (except the public domain sound effects).  All rights
reserved.  The technology underlying ToonTalk is protected by
U.S. Patent No. 5,517,663.  LEGO(tm) is a trademark of the LEGO Group
of companies which does not sponsor, authorize or endorse ToonTalk.

Thanks.
To all my beta testers.

Last words.
Enjoy and create neat things!  And send me the neat things and I'll post
them on the web page for others to see and use.


