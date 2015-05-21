Release notes for ToonTalk(TM) Swedish Release 2.0.  April 1997.
Copyright (c) 1992, 1997.  Ken Kahn, Animated Programs. All rights reserved.

1. Installation instructions.  

You need a PC running Microsoft Windows 3.1 or latter (Win95 and Win
NT 3.51 work).  It works best in 256 color mode.  It can run with 4
megabytes of memory but slowly.  It can run on fast 386 or slow 486
but it will be slow.  You'll need at least 10 megabytes of free hard
disk storage (8 more megabytes is needed for the narration of the
introductory tour).

Just double click (or run) the klicka.exe program on the CDROM.
You will be given a choice of minimal, standard or full installation.
If you have lots of disk space choose full, if you have very little
space choose minimal.  

You are also given the choice of 16 bit (default) or 32 bit versions.
Installing both is fine if you have the space.  The 32 bit version
only runs on Windows 95 and Windows NT 4.0.  The 32 bit version can
run in high resolution (640x480) and it mixes sounds while the 16 bit
version one sound eliminates the others.  On some machines the 32 bit
version is faster, on others the 16 bit -- try both.

You also have a box where you can install the English version of ToonTalk
as well.  Since this is a Swedish product you get the Swedish version
unconditionally.

2. Customization options.

When you start up ToonTalk it asks your name.  You can supply your
name on the command line instead.  Nearly all of the options described
below can be controlled by the options notebook in ToonTalk (page 10
of your main notebook).

Alternatively you can customize ToonTalk by editing the Windows
Program Item called ToonTalk (Alt-Enter when the icon is selected.)
After the part that goes toontalk.exe you can add any of the following
options separated by spaces.  Except for the -n option below, the defaults
should be adequate.  You can add any of these command lines to the
"Start ToonTalk" (program item (Win 3.1) or shortcut (Win 95)) which calls
StartTT.exe.

See \toontalk\doc\swedish\command.htm for details.

3. How to provide feedback and report bugs.
In order of preference:
A. Email. Kahn@CSLI.Stanford.edu or KenKahn@ToonTalk.com
B. Mail. 1748 Monticello Road, San Mateo, CA 94402
C. Fax 415-577-1784.

4. Documentation.
Marty the Martian provides much of the documentation needed.  
You can find a manual at http://www.toontalk.com/doc.htm.

5. Reporting bugs.

If a bug causes the system crashes you might try the following to help
track it down.  Copy ttbug16s.exe, cg16.dll and ttbug16s.cgi from the
CDROM's ToonTalk to your ToonTalk directory.  Run ttbug16s.exe.  If an
error is detected then please send me (Bugs@ToonTalk.com)
ttbug16s.cgl and <yourname>.log.  Thanks.

Also if you edit your \windows\toontalk.ini file to change the line
GenerateLogs=0
to 
GenerateLogs=3
Then then last 3 runs of ToonTalk will be rerunable.  They will be named
log1.dmo, log2.dmo, and log3.dmo.  Check creation dates to see which is the
most recent.  If you have captured a bug in a DMO file then please send it
to me.  It is a binary file -- if you need help sending it please send me
email.

6. Uninstall.

Sorry haven't implemented it yet.  You can delete the entire ToonTalk
directory (though you will lose anything you've done).
\windows\toontalk.ini can be deleted as can the ToonTalk program
group.  That should be it.  I recommend leaving the WinG stuff since
it is a Microsoft extension to Win 3.1, is small (300K) and used by
other programs.  Also if you installed the 32 bit version then I would
leave the DirectX extensions in the Window's system directory.

7. Readings.
Check out on the on-line help.  

There are 4 papers about ToonTalk available via anonymous ftp from
csli.stanford.edu.  See www.toontalk.com/papers.htm for details.
These are available on the CDROM on the Doc directory as well as some other
documents.  They can be viewed or printed by Microsoft Word or WordView
(on Doc\WordView on the CDROM).

8. Known Problems.
-f 1 doesn't work on Fahrenheit 1280 video boards if they are in more than
256 color mode.  (And the other -f modes work but are very slow.)  Change
to 256 colors to fix this.

9. Legalities.
ToonTalk is a trademark of Animated Programs.  Everything which can be
copyrighted is (except the public domain sound effects).  All rights
reserved.  The technology underlying ToonTalk is protected by
U.S. Patent No. 5,517,663.  LEGO(tm) is a trademark of the LEGO Group
of companies which does not sponsor, authorize or endorse ToonTalk.

10. Last words.
Enjoy and create neat things!  And send in the neat things and I'll post
them on the web page for others to see and use. (Send them to Contrib@ToonTalk.com).


