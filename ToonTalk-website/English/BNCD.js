// Translators:

// all that needs to be translated here are 
// BACK, NEXT, JUMP TO OPTION, CANCEL, and DONE

document.writeln('<P align="center">');
document.writeln('<INPUT onclick="sendBack(\'-previous_dialog\')" type="button" value="<<BACK<<" name="Back">');
document.writeln('<INPUT onclick="sendBack(\'-next_dialog ' + nextDialog + '\')" type="button" value=">>NEXT>>" name="Next">');
document.writeln('<INPUT onclick="sendBack(\'-next_dialog jumpopt.htm\')" type="button" value="JUMP TO OPTION" name="Jump">');
document.writeln('<INPUT onclick="sendBack(\'-cancel_settings\')" type="button" value="CANCEL" name="Cancel">');
document.writeln('<INPUT onclick="sendBack(\'-accept_settings\')" type="button" value="DONE" name="Done">');
document.writeln('</P>');

if (nextDialog == null) { // last one
   Next.disabled = true;
   Jump.disabled = true
};

checkDefaultButton();
