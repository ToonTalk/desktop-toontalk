Translating ToonTalk to another language requires the translation 
of the following files:

english.rc and puzzle.rc -- These files contain all the sentences and
phrases used by Marty the Martian. (Feel free to rename english.rc to
spanish.rc or whatever language you are working on.) It also contains
some information for generating the notebooks and customizing the
sensors.  Only the parts within " marks should be translated.  Lines
preceeded by // are just comments and should not be translated.

Some items have the value " ". This means in the English version of
ToonTalk this entry isn't used. But for languages with grammars very
different from English sometimes these are used. For example, the
entries:

IDS_YOU_ARE, // for example, "you are holding a box"
"you are"
IDS_YOU_ARE2, // used after "holding a box" above
 " "

If you wanted to use the grammar of Yoda from Star Wars, for example, you should edit the entries to read:

IDS_YOU_ARE, // for example, "you are holding a box"
" "
IDS_YOU_ARE2, // used after "holding a box" above
 " you are"

So it says "Holding a box you are." 
Note that the initial space in " you are" is needed.

sensors.rc --
This is used by the sensors that display text. If you don't translate
this then the sensors will default to English. Most display "yes" or
"no". The sensor for the keyboard will display English names for
special characters like "Down arrow" or "Control-a". While ToonTalk
attempts to support the transfer of ToonTalk programs between
different language versions, it would help with international exchange
to leave these in English but it is your choice.

The *.pzl files you'll find in puzzles.zip are all text files that
describe the puzzles.  http://www.toontalk.com/English/make_pzl.htm
provides more information about the format and which parts need
translation. Try translating 1 or 2 puzzles and then email it to
kenkahn@toontalk.com to check that correct portions have been
translated.

The *.htm files you'll find in html.zip are HTML files which includes
the users guide and dialogs. The following files need not be translated but
could be worthwhile anyway:
faq.htm behind.htm computer.htm concur.htm skills.htm 

The *.ust files you'll find in the ust.zip directory.  These
are used to generate subtitles for demos.  Leave the number that
starts each section alone -- it indicates the start time (in milliseconds).
Also the line breaks indicate how much should fit on a line of subtitles.
It will look bad if a line is too long to display.

java.txt -- translate the part in quotes
misc.txt -- please translate these sentences

Don't hesitate to ask questions by sending email to KenKahn@ToonTalk.com.

Good luck and thanks.


