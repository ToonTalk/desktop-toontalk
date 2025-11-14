// JScript source code in common between ToonTalk dialogs used to set options
// last modified 040903 by Ken Kahn

//var parameters = window.dialogArguments.split(";");
var parameters = null; // changed on 011002

function currentParameter(name) {
   if (parameters == null) { // new on 011002
      if (typeof window.dialogArguments == "undefined") {
         return("***DEFINED ONLY WHEN RUN FROM TOONTALK***")
      };
      parameters = window.dialogArguments.split(";")
   };
   for (var i = 0; i < parameters.length; i = i+2) {
      if (parameters[i] == name) {
         return(parameters[i+1]) // like an alist
      }    
   };
   return(0) // just in case
}
function checkDefaultButton() {
   if (document.Buttons.RadioGroup != null) {
      var index = currentParameter(category);
      for (var i = 0; i < document.Buttons.RadioGroup.length; i++) {
         if (document.Buttons.RadioGroup[i].value == index) {
            document.Buttons.RadioGroup[i].checked = true;
            break
         }
      }
   }
}      
function getSelectedButton() {
   if (document.Buttons.RadioGroup != null) {
      for (var i = 0; i < document.Buttons.RadioGroup.length; i++) {
         if (document.Buttons.RadioGroup[i].checked) {
            return document.Buttons.RadioGroup[i].value
         }
      }
   };
   return(0) // just in case
}
function sendBack(message) {
   window.returnValue = "-set_option(Defaults," + category + "," + getSelectedButton() + ") " + message; 
   window.close()
}
function setOption(section, key, value) { // new on 040903
	if (typeof window.returnValue == "undefined") {
		window.returnValue = ""; // initialize it
	};
   window.returnValue = window.returnValue + " -set_option(" + section + "," + key + "," + value + ") ";
}
function closeWindow(message) { // new on 040903
	if (typeof window.returnValue == "undefined") {
		window.returnValue = ""; // initialize it
	};
	window.returnValue = window.returnValue + message;
	window.close()
}
