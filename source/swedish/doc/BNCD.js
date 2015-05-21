// Translators:

// all that needs to be translated here are 
// BACK, NEXT, JUMP TO OPTION, CANCEL, and DONE

document.writeln('<P align="center">');
document.writeln('<INPUT onclick="sendBack(\'-previous_dialog\')" type="button" value="<<TILLBAKA<<" name="Back">');
document.writeln('<INPUT onclick="sendBack(\'-next_dialog ' + nextDialog + '\')" type="button" value=">>NÄSTA>>" name="Next">');
document.writeln('<INPUT onclick="sendBack(\'-next_dialog jumpopt.htm\')" type="button" value="HOPPA TILL TILLVAL" name="Jump">');
document.writeln('<INPUT onclick="sendBack(\'-cancel_settings\')" type="button" value="AVBRYT" name="Cancel">');
document.writeln('<INPUT onclick="sendBack(\'-accept_settings\')" type="button" value="FÄRDIG" name="Done">');
document.writeln('</P>');

if (nextDialog == null) { // last one
   Next.disabled = true;
   Jump.disabled = true
};

checkDefaultButton();

