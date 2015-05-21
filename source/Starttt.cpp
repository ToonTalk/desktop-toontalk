// Copyright (c) 1992-2007.  Ken Kahn, Animated Programs, All rights reserved

#define TT_32 1
//#define TT_DEBUG_STARTTT 0 -- set by Visual Studio when appropriate
#include "stdafx.h"
//#include <forcelib.h> // recommended by http://support.microsoft.com/default.aspx?scid=KB;EN-US;q148652&
  
// Windows Header Files:
#include <windows.h>
#include <mmsystem.h>

#pragma warning(disable : 4995) // new on 170302 for VS.NET since it complains about OLD_IOSTREAMS

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <shellapi.h>
#include <string.h>
#include <ctype.h>
//#include <strstrea.h> commented out 210703
#include <time.h>
#include <sys\timeb.h> // why both?

//added the following for Brazilian Movie Logo
#include <mmsystem.h>
#include <vfw.h>

#include "starttt.h"
#if !defined(__TT_DEFS_H) 
#include "defs.h"
#endif
#if !defined(__TT_COMMON_H) 
#include "common.h"
#endif

#include <shlobj.h> // new on 080402 to find correct directories to put things
#include <shfolder.h> // new on 260402 to find correct directories to put things

#include <mmsystem.h>
#include <initguid.h>
#include <objbase.h>
#include <objerror.h>
#include <ole2ver.h>
#include "speech.h"

//#include "ip_to_country.h" // new on 200607 -- didn't work in general

// changed all occurences of szModeName to szSpeaker on 1110803 so the names are more user friendly - thanks Amelie

extern char tt_toontalk_crash_directory[MAX_PATH];
char tt_toontalk_crash_directory[MAX_PATH];

//enum AutoDemo {NO_AUTODEMO, REPEAT_INTRO, REPEAT_ALL_DEMOS};

AutoDemo autodemo = NO_AUTODEMO;

//enum NaturalLanguage {NO_LANGUAGE_SPECIFIED,AMERICAN,SWEDISH,
//                      BRAZILIAN_PORTUGUESE,GERMAN,FRENCH,SPANISH,
//                      BRITISH,TURKISH,PORTUGAL_PORTUGUESE,JAPANESE,
//							 UNKNOWN_LANGUAGE,LANGUAGE_COUNT};

NaturalLanguage tt_language;

//const int FILE_FOR_CLIPBOARD_MESSAGE_ID = 1;

//const long min_long = -0x7FFFFFFFL-1;

/////////////////////////////////////////////////////////////////////////
// Global Variables:

HINSTANCE hInst; // current instance

HWND window_handle = NULL; // made global on 210602

int original_CmdShow; // arg to main - placed here on 300402

HINSTANCE resource_library = NULL;

//char szAppName[] = APPNAME; // The name of this application
//char szTitle[]   = APPNAME": ToonTalk"; // The title bar text
//char szHelpFileName[] = APPNAME".HLP";

string tt_local_ini_file = NULL;

string old_toontalk_ini_contents = NULL;
long old_toontalk_ini_file_size = 0;

//char toontalk_directory[256] = "";
//char cdrom_directory[256] = "";
string toontalk_directory = NULL; // re-written on 070201
char doc_subdir[MAX_PATH]; // moved out here on 100304 since is shared
string cdrom_directory = NULL;
BOOL cdrom_installation = FALSE; // obsolete?? - asked 151204

char module_directory[MAX_PATH]; // new on 191204 to re-use this -- location of the EXE

boolean ini_file_name_overridden = FALSE; // made top-level on 151204
boolean running_read_only = FALSE; // new on 151204

extern char tt_application_name[]; // made external on 241002 for mini dumps
char tt_application_name[] = "StartTT";

// following should be made obsolete
//char toontalk_exe_name[64] = "TT16"; // just defaults here too
//char demo_exe_name[64] = "TT16";
string toontalk_exe_name = NULL; // re-written on 070201
string demo_exe_name = NULL;

string language_arg = NULL;

HGLOBAL global_handle = 0;

HACCEL hAccelTable;

BOOL full_screen_mode_off = FALSE;

BOOL fPalette = FALSE;
BOOL fTrueColor = FALSE;

enum Options {SYSTEM_DEFAULTS_OPTION,VERSION_OPTION,WINDOW_SIZE_OPTION,
              MOUSE_BUTTON_OPTION,MARTY_TALK_OPTION,SOUND_OPTION,KIND_OF_USER_OPTION,LANGUAGE_OPTION,VOICE_OPTION};

const int first_option = VERSION_OPTION;
const int last_option = VOICE_OPTION;

BOOL paused = FALSE;

string option_names[] = 
  {"SystemDefaults","Version","WindowSize","MouseButtons","MartyTalk","SoundOn","KindOfUser","Language","Voice"};
// first value was "" prior to 150602

string *languages = NULL; // array of available languages
int language_count = 0;

int voice_option_visit_count = 0;

boolean DMO_file = FALSE;
boolean USR_file = FALSE;
boolean CTY_file = FALSE;
boolean PZL_file = FALSE;
boolean TT_file = FALSE; // new on 200300 - text file typically containing ToonTalk clippings

string tt_ini_file = NULL; // "ToonTalk.ini"; // new on 260601

BOOL set_options_now = FALSE; // new on 080402

int system_defaults = 0; // new on 140602

HWND hwndBtnFreePlay, hwndBtnSeeDemos, hwndBtnPuzzles, hwndBtnOptions, hwndBtnAbout, hwndBtnHelp;
// moved here so I can destroy them second time round

HWND MCI_window = NULL;

int current_option = 0;
const int option_count = 9;
BOOL options_cancelled = FALSE;
BOOL options_done = FALSE;
const int max_choices = 7;

int question_count[option_count] = {4,7,5,3,7,2,3,1,1};
int answers[option_count] = {2,1,0,0,0,0,0,0,0}; // defaults initially

const int explanation_offset = 8;

string *explanation_additions = NULL;

const int max_voices = 100;

TTSMODEINFOA *ModeInfo = NULL;

int option_to_mode[max_voices];

// for use by mdump - can be overridden by toontalk.ini
string tt_ok_to_generate_mini_dump_message = "Sorry, something is wrong with ToonTalk. Is it OK to generate a diagnostic file to help fix the problem?"; // set here in case error occurs before reset by toontalk.ini
string tt_mini_dump_email = "mailto:support@toontalk.com?subject=ToonTalk Crash Report&body=Please add any information that might be useful. And don't forget the attachment."; 
BOOL tt_show_mini_dump_directory = TRUE;
BOOL tt_full_memory_dump = FALSE;
void pause_toontalk(boolean interrupted, boolean ask, boolean shutting_down) {
   // only useful for mdump when DirectX has taken control
};

/////////////////////////////////////////////////////////////////////////
// Forward declarations of functions included in this code module:

BOOL InitApplication(HINSTANCE);
BOOL InitInstance();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CenterWindow (HWND, HWND);
BOOL CenterWindowHorizontally (HWND, HWND);
LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);

boolean interpret_command_line(string command_line);

BOOL option_dialogs(HWND hwnd);
//void options_changed(HWND hwnd);

// The following functions were added to Generic for this AutoRun sample app:
int PrepareWindow (HWND hWnd);
BOOL LaunchApplication(int id);
BOOL BeginOLE();

//BOOL OpenDocument();
BOOL first_time = TRUE;
BOOL already_initialized = FALSE; // new on 210602
BOOL word_view_active = FALSE;
BOOL part2_pending = FALSE;
BOOL message_box_up = FALSE;
char device_of_this_app[3]; //e.g. "d:"

//LPSTR original_command_line = NULL;
char original_command_line[1024]; // new on 161002 so can modify it safely
char current_command_line[1024]; // new on 300402
BOOL new_command_line = FALSE;
char html_dialog_file[MAX_PATH] = "";
BOOL html_dialog_file_is_from_HTMLStartTT = FALSE;
//string beginning_html_dialog_file = NULL;
char beginning_html_dialog_file[MAX_PATH]; // updated on 161002
const int max_history = 1000;
string html_dialog_file_history[max_history];
int last_dialog_index = 0;
BOOL use_html_dialog_file = FALSE;
char demo_name[MAX_PATH] = "";
BOOL html_dialog_returned_command_line = FALSE; // new on 300402

const int max_simultaneous_resource_strings = 8;
const int max_starttt_resource_string_length = 1000; // might be concatenating to make a long explanation

string *string_buffer = NULL;
unsigned int buffer_index = 0;

// BOOL runningHighResolution = FALSE; // commented out on 230300 since not used anymore

BOOL language_option_operational = TRUE; // until proven otherwise

void update_encoded_current_state();

const int encoding_max_size = 10000;
char encoded_current_state[encoding_max_size]; // very large so can include stuff like voices, defaults, etc. (new on 150602)

HPALETTE hpal;
HBITMAP hbmAutoRun;
BITMAP bmAutoRun;
int next_demo = IDS_INTRO_DEMO;
const unsigned int max_idle_time = 30000; // 30 seconds
BOOL toontalk_running_when_timer_set = FALSE;
BOOL CALLBACK about_dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM );


HINSTANCE resource_source();
string existing_file_name(BOOL &on_cdrom, string short_file_name, string extension=NULL,
                          string subdirectory=NULL, BOOL ok_to_bother_user=TRUE);
string S(int id, int number_of_strings_to_include=1, string default_string="");
string S(int id, string default_string) {
	return(S(id,1,default_string));
};

void allocate_resource_strings() {
	string_buffer = new string[max_simultaneous_resource_strings];
	for (int i = 0; i < max_simultaneous_resource_strings; i++) {
		string_buffer[i] = new char[max_starttt_resource_string_length];
	};
};

void deallocate_resource_strings() {
	for (int i = 0; i < max_simultaneous_resource_strings; i++) {
		delete [] string_buffer[i];
	};
	delete [] string_buffer;
};

string S(int id, int number_of_strings_to_include, string default_string) {
	int index = buffer_index++%max_simultaneous_resource_strings;
   int offset = 0;
   for (int i = 0; i < number_of_strings_to_include; i++) {
	   int length = LoadString(resource_source(),id+i,string_buffer[index]+offset,max_starttt_resource_string_length);
	   if (length == 0) {
         if (i == 0) return(default_string); // no entry
         return(string_buffer[index]); // no more entries
      };
      offset += length;
   };
	return(string_buffer[index]);
};

string copy_string(string source, int length) { // length as arg new on 260601
   if (source == NULL) return(NULL);
   if (length < 0) length = strlen(source);
   string destination = new char[length+1];
   memcpy(destination,source,length);
   destination[length] = 0; // terminate the string
   return(destination);
};

string SC(int id) {
  string answer = S(id);
  if (answer != NULL) return(copy_string(answer));
  return(NULL);
};

boolean ini_entry_defined_in_local_ini_file(string category, string id) { // new on 020205
	const int max_size = 512;
	char answer[max_size]; 
	if (tt_local_ini_file != NULL) { // new on 070201
		if (GetPrivateProfileString(category,id,"",answer,max_size,tt_local_ini_file) != 0) {
			return(TRUE);
		};
	};
	return(FALSE);
};

string ini_entry(string category, string id) {
	const int max_size = 512; // silly but changed from 256 to 512 without changing the calls below - fixed on 040304
	string answer = new char[max_size]; 
	if (tt_local_ini_file != NULL) { // new on 070201
		if (GetPrivateProfileString(category,id,"",answer,max_size,tt_local_ini_file) != 0) {
			return(answer);
		};
	};
	if (GetPrivateProfileString(category,id,"",answer,max_size,tt_ini_file) == 0) {
		delete [] answer;
		return(NULL);
	} else {
		return(answer);
	};
};

int ini_int(string category, string id) {
	int answer = min_long;
	if (tt_local_ini_file != NULL) { // new on 070201
		answer = GetPrivateProfileInt(category,id,min_long,tt_local_ini_file);
	};
	if (answer == min_long) {
		answer = GetPrivateProfileInt(category,id,0,tt_ini_file); 
	};
	return(answer);
};

int default_setting(string id) {
   int answer = ini_int("Defaults",id);
   if (answer != 0) return(answer);
   return(ini_int("InstallationDefaults",id));
};

int language_id = LANG_ENGLISH;

NaturalLanguage string_to_language(char *language) {
   if (strnicmp("American",language,8) == 0) {
		language_id = LANG_ENGLISH;
      return(AMERICAN);
   } else if (strnicmp("Swe",language,3) == 0 || strnicmp("Sve",language,3) == 0) { // was just "Swedish" prior to 170400
		language_id = LANG_SWEDISH;
      return(SWEDISH);
   } else if (strnicmp("Port",language,4) == 0) { // was Portuguese prior to 170400
		language_id = LANG_PORTUGUESE;
		if (ini_int("Versions","PortugueseIsBrazilian")) {
         return(BRAZILIAN_PORTUGUESE);
      } else {
         return(PORTUGAL_PORTUGUESE);
      };
   } else if (strnicmp("German",language,6) == 0) {
		language_id = LANG_GERMAN;
      return(GERMAN);
   } else if (strnicmp("French",language,6) == 0) {
		language_id = LANG_FRENCH;
      return(FRENCH);
   } else if (strnicmp("Turkish",language,7) == 0) {
		language_id = LANG_TURKISH;
      return(TURKISH);
   } else if (strnicmp("British",language,7) == 0) {
      language_id = LANG_ENGLISH; // added 210602
      return(BRITISH);
   } else if (strnicmp("Eng",language,3) == 0) { // was "English" prior to 170400
		language_id = LANG_ENGLISH;
      if (ini_int("Versions","EnglishIsAmerican")) {
         return(AMERICAN);
      } else {
         return(BRITISH);
      };
	} else if (strnicmp("Japanese",language,8) == 0) {
		language_id = LANG_JAPANESE;
      return(JAPANESE);
   // now the for country code alternatives
   } else if (strnicmp("US",language,2) == 0) {
		language_id = LANG_ENGLISH;
      return(AMERICAN);
   } else if (strnicmp("UK",language,2) == 0) {
		language_id = LANG_ENGLISH;
      return(BRITISH);
   } else if (strnicmp("SE",language,2) == 0) {
		language_id = LANG_SWEDISH;
      return(SWEDISH);
   } else if (strnicmp("BR",language,2) == 0) {
		language_id = LANG_PORTUGUESE;
      return(BRAZILIAN_PORTUGUESE);
	} else if (strnicmp("PT",language,2) == 0) { // added 070100
		language_id = LANG_PORTUGUESE;
      return(PORTUGAL_PORTUGUESE);
   } else if (strnicmp("DE",language,2) == 0) {
		language_id = LANG_GERMAN;
      return(GERMAN);
   } else if (strnicmp("FR",language,2) == 0) {
		language_id = LANG_FRENCH;
      return(FRENCH);
   } else if (strnicmp("TR",language,2) == 0) {
		language_id = LANG_TURKISH;
      return(TURKISH);
	} else if (strnicmp("JP",language,2) == 0) {
		language_id = LANG_JAPANESE;
      return(JAPANESE);
	} else if (strnicmp("IT",language,2) == 0) {
		language_id = LANG_ITALIAN;
      return(ITALIAN);
	} else if (strnicmp("ES",language,2) == 0) { // new on 210506
		language_id = LANG_SPANISH;
      return(SPANISH);
   };
   return(UNKNOWN_LANGUAGE);
};

BOOL first_call = TRUE;

BOOL reported_dll_load_problem = FALSE;

HINSTANCE resource_source() {
   // static version could just return hInst...
   if (resource_library == NULL) {
      // load library
      char file_name[MAX_PATH]; // was 256 prior to 290502
      output_string_stream stream(file_name,MAX_PATH);
      switch (tt_language) {
         case AMERICAN:
            stream << "US";
            break;
         case SWEDISH:
            stream << "SE";
            break;
         case BRAZILIAN_PORTUGUESE:
            stream << "BR";
            break;
			case PORTUGAL_PORTUGUESE:
				stream << "PT";
				break; // break was missing prior to 041099 - thanks Tiago
         case BRITISH:
            stream << "UK";
            break;
         case GERMAN:
            stream << "DE";
            break;
         case FRENCH:
            stream << "FR";
            break;
         case TURKISH:
            stream << "TR";
            break;
			case JAPANESE:
				stream << "JP";
				break;
         default: // assume first 2 letters are country code
            if (language_arg != NULL && strlen(language_arg) > 1) {
               stream.write(language_arg,2);
            } else {
               int language_answer = default_setting("Language");
               string language_key;
               if (language_answer > 0) {
                  language_key = copy_string("Language1");
                  itoa(language_answer,language_key+8,10);
               } else {
                  language_key = copy_string("Language");
               };
               string default_language = ini_entry("Versions",language_key);
					delete [] language_key;
               if (default_language != NULL) {
                  tt_language = string_to_language(default_language);
                  delete [] default_language;
               };
               if (tt_language == UNKNOWN_LANGUAGE || !first_call) {
                  stream << "US";
               } else {
                  first_call = FALSE; // don't recursive twice...
                  return(resource_source());
               };
            };
      };
#if TT_32
      stream << "STT32";
#else
      stream << "STT16";
#endif
      stream.put((char) 0); // terminate string
      BOOL on_cdrom;
      string full_file_name = existing_file_name(on_cdrom, file_name, "DLL");
      if (full_file_name != NULL) {
         resource_library = LoadLibrary(full_file_name);
         delete [] full_file_name;
      };
#if TT_32
      if (resource_library == NULL) {
#else
      if ((int) resource_library <= 32 && !reported_dll_load_problem) {
#endif
          MessageBox(NULL,"Couldn't load resource file. Sorry.\n Maybe you need to restart the computer or, if this persists, re-install ToonTalk.","Start ToonTalk",
						   MB_ICONSTOP|MB_OK|MB_SYSTEMMODAL);
			 reported_dll_load_problem = TRUE;
          PostQuitMessage(0);
      };
   };
   return(resource_library);
};

// consider moving this to common.cpp
string existing_file_name(BOOL &on_cdrom, string short_file_name, string extension, string subdirectory, BOOL ok_to_bother_user) {
	if (short_file_name == NULL) return(NULL); // new on 110304 for robustness
    on_cdrom = FALSE;
	 char full_short_name[MAX_PATH]; // new on 060401
	 output_string_stream short_name(full_short_name,MAX_PATH);
    short_name << short_file_name;
    if (strchr(short_file_name,'.') != NULL) { // already has an extension so ignore "advice" -- new on 161002
       extension = NULL;
    };
    if (extension != NULL) short_name << "." << extension;
    short_name.put((char) 0);
    char relative_path[MAX_PATH]; // everything after main or cdrom directory
    output_string_stream path(relative_path,MAX_PATH);
    if (subdirectory != NULL) path << subdirectory << "\\";
    path << short_file_name;
    if (extension != NULL) path << "." << extension;
    path.put((char) 0);
    string name_buffer = new char[MAX_PATH];
    output_string_stream name(name_buffer,MAX_PATH);
    if (toontalk_directory != NULL) { // added conditional on 070201
		 name << toontalk_directory;
	 };
	 if (relative_path != NULL) { // added conditional on 070201
		 name << relative_path;
	 };
    name.put((char) 0);
    OFSTRUCT file_struct;
    if (OpenFile(name_buffer,&file_struct,OF_EXIST) != -1) {
       return(name_buffer);
    };
    if (cdrom_installation) {
       // try cdrom now
       output_string_stream cd_name(name_buffer,MAX_PATH);
       cd_name << cdrom_directory << relative_path;
       cd_name.put((char) 0);
       on_cdrom = TRUE;
       if (OpenFile(name_buffer,&file_struct,OF_EXIST) != -1) {
          return(name_buffer);
       };
       if (ok_to_bother_user) {
          if (subdirectory == NULL ||
              stricmp(subdirectory,"Demos") != 0 ||
              ini_int("CDROMDemoFiles",full_short_name)) { // arg was relative_path prior to 060401
             // am not using string library since this might be called to find the library...
			    string message = "If you insert the ToonTalk CD-ROM and wait a few seconds,\rToonTalk will be able to continue. ";
				 // new on 110304 for robustness (in case looking on the CD-ROM for StartTT's DLL)
				 if (resource_library != NULL) { // must use English string if library isn't loaded
					 message = S(IDS_INSERT_CDROM,message);
				 };
				 // starting using S here on 060401 so can be translated
             if (MessageBox(NULL,message,"Start ToonTalk",MB_RETRYCANCEL|MB_ICONQUESTION) == IDRETRY) {
                if (OpenFile(name_buffer,&file_struct,OF_EXIST) != -1) {
                   return(name_buffer);
                };
             };
          } else {
             delete [] name_buffer;
             return(NULL);
          };
       };
    };
	 char url_path[MAX_PATH]; // new on 210507
	 strcpy(url_path,name_buffer);
	 strcat(url_path,".url");
	 if (OpenFile(url_path,&file_struct,OF_EXIST) != -1) {
       return(name_buffer); // original name OK since toontalk.exe will deal with it OK
    };
    if (ok_to_bother_user) {
       char message_buffer[512];
       output_string_stream message(message_buffer,512);
       message << "Warning: The file " << " " << name_buffer << " couldn't be found.";
       message.put((char) 0);
       MessageBox(NULL,message_buffer,"Start ToonTalk",MB_ICONINFORMATION);
    };
    delete [] name_buffer;
    return(NULL);
};


// using time(NULL) now
//time_t seconds_since_1970() { // returned long prior to 240100
  // for some reason the 16 bit and 32 bit versions believe they are in different time zones
  // this corrects for the problem
//   timeb time_struct;
//   ftime(&time_struct);
//   return(time_struct.time-time_struct.timezone*60);
//};

HFILE open_starttt_log_file() {
   OFSTRUCT file_struct;
   char log_name[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,log_name); // changed on 270601 since toontalk_directory might not be set yet
	strcat(log_name,"\\StartTT.log"); 
//   output_string_stream name(log_name,MAX_PATH);
//   name << toontalk_directory << "StartTT.log";
//   name.put('\0');
//      output_file_stream file(log_name,std::ios_base::out);
      // I use OpenFile rather than output_file_stream to keep the EXE small
   return(OpenFile(log_name,&file_struct,OF_CREATE));
};

string command_line_arg(string command_line, string search_for) {
	// this is case sensitive
	string found = strstr(command_line,search_for);
	if (found == NULL) return(NULL);
	found += strlen(search_for);
	if (found != NULL) {
		while (found[0] == ' ') { // remove spaces
			found++;
		};
	};
	return(found);
};

string copy_string_without_quotes(string source, int length) { // new on 260601
	if (source == NULL) return(NULL);
	if (source[0] == '"') {
		if (length < 0) length = strlen(source);
		while (source[length-1] == ' ') { // new on 270601 to remove trailing space
			length--; 
		};
		if (source[length-1] == '"') { // closing quote found
			return(copy_string(source+1,length-2));
		};
	};
	return(copy_string(source,length));
};

void launch_help(string short_file_name) {
   char htm_help_file_name[MAX_PATH];
   output_string_stream file_name(htm_help_file_name,MAX_PATH);
//   file_name << toontalk_directory << "Doc\\" << S(IDS_DOC_SUBDIRECTORY_NAME) << "\\help.htm";
	if (short_file_name[0] == '.' && short_file_name[1] == '.') { //  something like ../tools/foo.htm
		// new on 180207 -- note this only supports one level of ../
		file_name << toontalk_directory << "Doc";
		file_name << short_file_name+2;
	} else {
		file_name << toontalk_directory << "Doc\\" << S(IDS_DOC_SUBDIRECTORY_NAME) << "\\";
		file_name << short_file_name; // rewritten on 010205
	};	
   file_name.put((char) 0);
   int status = (int) ShellExecute(NULL,"open",htm_help_file_name,"","",SW_SHOWMAXIMIZED); // was just SW_SHOW prior to 250402
   if (status <= 32) { // had some problems
      MessageBox(NULL,"ToonTalk received an error when starting a web browser on the help file. Sorry.","Start ToonTalk",MB_ICONINFORMATION);
      HFILE file = open_starttt_log_file();
		_lwrite(file,htm_help_file_name,strlen(htm_help_file_name));
      _lwrite(file," ;Status=",9);
      char status_string[16];
      ltoa(status,status_string,10);
      _lwrite(file,status_string,strlen(status_string));
		_lclose(file);
	};
   if (use_html_dialog_file) { // new on 210602
      PostQuitMessage(0);
   };
};

string copy_file_to_string(string file_name, long &file_size) {
	HANDLE file_handle = CreateFile(file_name,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (file_handle == INVALID_HANDLE_VALUE) {
		file_size = 0;
		return(NULL);
	};
   file_size = GetFileSize(file_handle,NULL);
	if (file_size <= 0) {
      CloseHandle(file_handle);
		return(NULL);
   } else {
	   string result = new char[file_size+1];;
		DWORD bytes_read;
      ::ReadFile(file_handle,result,file_size,&bytes_read,NULL);
      CloseHandle(file_handle);  // missing prior to 170602
      // warn if bytes_read != file_size ??
		result[file_size] = '\0'; // terminate string
	   return(result);
   };
};

boolean copy_string_to_file(string value, long string_length, string file_name) {
   HANDLE file_handle = CreateFile(file_name,GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (file_handle == INVALID_HANDLE_VALUE) {
		return(FALSE);
	};
   DWORD bytes_written;
   ::WriteFile(file_handle,value,string_length,&bytes_written,NULL);
   CloseHandle(file_handle); // missing prior to 170602
   return(bytes_written == string_length);
};

void copy_toontalk_ini_to_old_location() { 
	// new on 161002 -- not needed once all old demos are redone
	// but is still needed to deal with multiple "ordinary" windows user on the same machine
   char old_toontalk_ini[MAX_PATH];
   get_special_folder(CSIDL_WINDOWS,old_toontalk_ini); 
   strcat(old_toontalk_ini,"\\toontalk.ini");
   CopyFile(tt_ini_file,old_toontalk_ini,FALSE);
	SetFileAttributes(old_toontalk_ini,FILE_ATTRIBUTE_NORMAL); // new on 191204
};

int short_file_name_start(ascii_string path) { // new on 180303 -- copied here on 060404
	int i = strlen(path)-1; // end of string if no dot otherwise just prior to dot
	while (i > 0) {
		if (path[i] == '\\' || path[i] == ':' || path[i] == '/') {
			return(i+1);		
		};
		i--;
	};
	return(0);
};

void get_my_documents_folder(string path) { // copied and updated on 050504 from utils.cpp
	if (!get_special_folder(CSIDL_PERSONAL,path)) {
		if (toontalk_directory != NULL) {
			strcpy(path,toontalk_directory);
			strcat(path,"My Documents");
		} else {
			strcpy(path,"C:"); // need something
		};
	};
};

string expand_question_mark_as_module_path(string directory) { // new on 191204 -- so UserFiles can reference MainDir
	int length = strlen(directory);
	int module_directory_length = strlen(module_directory);
	int new_length = length+3+module_directory_length;
	int user_directory_length = 0;
	string new_directory = new char[new_length];
	strcpy(new_directory,module_directory);
	int skip = 1; // to skip the ? itself
	if (directory[1] == '\\') {
		skip++; // and to skip \ (since tt_main_directory has a slash)
	};
	strcat(new_directory,directory+skip);
	length = strlen(new_directory);
	if (new_directory[length-1] != '\\') {
		new_directory[length] = '\\';
		new_directory[length+1] = '\0';
	};
	return(new_directory);
};

boolean has_main_dir_entry(string ini_file) { // new on 170105 to ignore toontalk.ini files that don't even set MainDir
	if (tt_local_ini_file != NULL && ini_file != tt_local_ini_file && has_main_dir_entry(tt_local_ini_file)) {
		return(TRUE);
	};
	if (ini_file == NULL) return(FALSE);
	OFSTRUCT file_struct;
	if (OpenFile(ini_file,&file_struct,OF_EXIST) == -1) return(FALSE);
	char entry[MAX_PATH];
	if (GetPrivateProfileString("Directories","MainDir","",entry,MAX_PATH,ini_file) > 0) {
		return(TRUE);
	};
	if (GetPrivateProfileString("Executables","UseThisIniFileFirst","",entry,MAX_PATH,ini_file) > 0) {
		return(has_main_dir_entry(entry));
	};
	return(FALSE);
};


int PASCAL WinMain_internal(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
   hInst = hInstance; // Store instance handle in our global variable -- moved here on 300402
   original_CmdShow = nCmdShow; 
	BeginOLE(); // moved here on 260604
//   original_command_line = lpCmdLine;
//   if (get_special_folder(CSIDL_PERSONAL,tt_toontalk_crash_directory)) { // new on 241002
	get_my_documents_folder(tt_toontalk_crash_directory); // updated 050504
   strcat(tt_toontalk_crash_directory,"\\ToonTalk\\");
//   CreateDirectory(tt_toontalk_crash_directory,NULL); // commented out on 191204
   strcat(tt_toontalk_crash_directory,"Crash Reports\\");
//   CreateDirectory(tt_toontalk_crash_directory,NULL);
   //} else {
   //   tt_toontalk_crash_directory[0] = '\0';
   //};
   strcpy(original_command_line,lpCmdLine);
	char ini_file_name[MAX_PATH]; // this is to find out if the file exists
	if (lpCmdLine != NULL) { // new on 260601
		string ini_file_command_arg = command_line_arg(lpCmdLine,"-ini_file");
		if (ini_file_command_arg != NULL) {
			tt_ini_file = copy_string_without_quotes(ini_file_command_arg);
			strcpy(ini_file_name,tt_ini_file);
			ini_file_name_overridden = TRUE;
		};
		string local_ini_file_command_arg = command_line_arg(lpCmdLine,"-local_ini_file");
		if (local_ini_file_command_arg != NULL) {
			// new on 050404
			tt_local_ini_file = copy_string_without_quotes(local_ini_file_command_arg);
			boolean relative = (strchr(local_ini_file_command_arg,':') == NULL && local_ini_file_command_arg[0] != '\\'); // new on 060404
			if (relative) {
				// append the directory of this EXE
				string full_path = new char[MAX_PATH];
				full_path[0] = '\0'; // just in case the following fails
				GetModuleFileName(NULL,full_path,MAX_PATH);
				int end_of_directory_path = short_file_name_start(full_path);
				strcpy(full_path+end_of_directory_path,tt_local_ini_file);
				delete [] tt_local_ini_file;
				tt_local_ini_file = full_path;
			};				
		} else {
			tt_local_ini_file = ini_entry("Executables","UseThisIniFileFirst"); 
			// new on 030304 -- was in the wrong branch above prior to 060404
		};
	};
//HFILE file = open_starttt_log_file(); // for debugging
//if (current_command_line == NULL) {
//	_lwrite(file,"No command line.",strlen("No command line."));
//} else {
//	_lwrite(file,current_command_line,strlen(current_command_line));
//};
//_lclose(file);
	if (!ini_file_name_overridden) {
//		GetWindowsDirectory(ini_file_name,MAX_PATH);
      // above was rewritten on 080402 to be compatible with Windows XP guidelines
//    if (SHGetFolderPath(NULL,CSIDL_COMMON_DOCUMENTS|CSIDL_FLAG_CREATE,NULL,SHGFP_TYPE_CURRENT,common_documents) != S_OK) {
      // following is older version that is available on nearly all systems (for 95 and NT 4 requires IE 4.0)
//      if (!SHGetSpecialFolderPath(NULL,ini_file_name,CSIDL_PERSONAL,TRUE)) {
      //if (!get_special_folder(CSIDL_PERSONAL,ini_file_name)) { // rewritten on 260402
      //   // what to do?
      //};
		get_my_documents_folder(ini_file_name); // updated 050504
      strcat(ini_file_name,"\\ToonTalk\\toontalk.ini");
      tt_ini_file = copy_string(ini_file_name);
		if (tt_local_ini_file == NULL) { // condition new on 020205 since otherwise this clobbers the command line option
			tt_local_ini_file = ini_entry("Executables","UseThisIniFileFirst"); // new on 030304 
		};
	};
	message_box_up = TRUE;
   allocate_resource_strings();
	if (::GetModuleFileName(NULL,module_directory,MAX_PATH)) {
		// new on 141204 to get ToonTalk to run off a CD-ROM -- do this in StartTT as well
		int directory_end = max(0,short_file_name_start(module_directory)); // if somehow this returns -1 then make module_directory the empty string
		module_directory[directory_end] = '\0';
//strcpy(module_directory,"d:\\toontalk\\");  // for debugging 191204
//MessageBox(NULL,module_directory,"module_directory",MB_OK); // for debugging 191204
	};
	OFSTRUCT file_struct;
	if (!has_main_dir_entry(ini_file_name)) { // prior to 170105 was OpenFile(ini_file_name,&file_struct,OF_EXIST) == -1)
      int directory_length = strlen(ini_file_name)-12; // strlen("toontalk.ini");
//    boolean toontalk_installed = FALSE; // obsolete as of 151204
      char old_toontalk_ini[MAX_PATH];
//    SHGetSpecialFolderPath(NULL,old_toontalk_ini,CSIDL_WINDOWS,TRUE);
      get_special_folder(CSIDL_WINDOWS,old_toontalk_ini); // rewritten on 260402
      strcat(old_toontalk_ini,"\\toontalk.ini");
		if (has_main_dir_entry(old_toontalk_ini)) { // updated 170105 was OpenFile(old_toontalk_ini,&file_struct,OF_EXIST) != -1) {
			// following moved here from a few lines above on 191204 since shouldn't create this unless there is an old toontalk.ini
			ini_file_name[directory_length-1] = '\0';
			CreateDirectory(ini_file_name,NULL); // new on 080402 since might be a new user (in the Windows 2000/XP sense)
			ini_file_name[directory_length-1] = '\\';
         CopyFile(old_toontalk_ini,ini_file_name,FALSE);
			SetFileAttributes(ini_file_name,FILE_ATTRIBUTE_NORMAL); // new on 191204
//       toontalk_installed = TRUE;
      } else if (module_directory[0] != '\0') { // "else" was missing prior to 090805
			// new on 141204 to get ToonTalk to run off a CD-ROM -- do this in StartTT as well
//			char main_directory[MAX_PATH];
			strcpy(old_toontalk_ini,module_directory);
			strcat(old_toontalk_ini,"toontalk.ini");
			if (has_main_dir_entry(old_toontalk_ini)) { // updated 170105 was OpenFile(old_toontalk_ini,&file_struct,OF_EXIST) != -1) {
				char user_files[MAX_PATH];
				boolean user_files_specified = (GetPrivateProfileString("Directories","UserFiles","",user_files,MAX_PATH,old_toontalk_ini) > 0);
				if (user_files_specified) {
					// new on 191204 -- no need to ask the user anything since ini file answers the question
					ini_file_name_overridden = TRUE; // so the new ini file is used
					string user_files_full_path = expand_question_mark_as_module_path(user_files);
					if (!::CreateDirectory(user_files_full_path,NULL)) { // new on 040105
						int last_error = GetLastError(); // code 183 is fine - means directory already existed
						if (last_error != 183) {
							char message[1024+MAX_PATH];
							strcpy(message,"ToonTalk is trying to copy ");
							strcat(message,old_toontalk_ini);
							strcat(message," to the folder given as UserFiles ");
							strcat(message,user_files_full_path);
							strcat(message," but can't create that directory.");
							MessageBox(NULL,message,"Start ToonTalk",MB_OK);
							exit(0);
						};
					};
					strcpy(ini_file_name,user_files_full_path);
					strcat(ini_file_name,"toontalk.ini");
					CopyFile(old_toontalk_ini,ini_file_name,TRUE); // no-op if already there
					SetFileAttributes(ini_file_name,FILE_ATTRIBUTE_NORMAL); // new on 191204
					tt_ini_file = copy_string(ini_file_name);
				} else {
					boolean need_to_try_again = TRUE;
					while (need_to_try_again) { // to stay in this loop if given a folder that can't written to
						need_to_try_again = FALSE;
						char dialog_path[MAX_PATH];
						strcpy(dialog_path,module_directory);
						strcat(dialog_path,"Doc\\English\\whatdir.htm");
						string from_dialog = show_html_dialog(dialog_path,NULL,"");
						if (from_dialog != NULL) {
							boolean using_my_documents = FALSE;
							char user_directory[MAX_PATH];
							if (strcmp(from_dialog,"My Documents") == 0) {
								get_my_documents_folder(user_directory); // updated 050504
								strcat(user_directory,"\\ToonTalk\\");
								using_my_documents = TRUE;
							} else if (strcmp(from_dialog,"***NULL***") == 0) { // not fully implemented (noted 191204)
								user_directory[0] = '\0'; // null string
								running_read_only = TRUE;
								tt_ini_file = copy_string(old_toontalk_ini); // the one typically on the CD-ROM
								ini_file_name_overridden = TRUE;
							} else if (strcmp(from_dialog,"***CLOSED***") == 0) {
								exit(0);
							} else {
								// commented out since this doesn't make sense for thinks like a:\users -- and no longer use Browse to get a file path
								// that needs to be converted to a directory path
								//int new_directory_end = short_file_name_start(from_dialog);
								//if (new_directory_end > 0) {
								//	from_dialog[new_directory_end] = '\0';
								//	memcpy(user_directory,from_dialog,new_directory_end);
								//	user_directory[new_directory_end] = '\0';
								//} else {
									strcpy(user_directory,from_dialog);
									int directory_length = strlen(user_directory);
									if (user_directory[directory_length-1] != '\\') {
										user_directory[directory_length++] = '\\'; // add final slash
										user_directory[directory_length] = '\0';
										::CreateDirectory(user_directory,NULL); // new on 191204
									};
//									MessageBox(NULL,user_directory,"user_directory",MB_OK); // for debugging 191204
//								};
							};
							if (user_directory[0] != '\0') {
								strcpy(ini_file_name,user_directory);
								strcat(ini_file_name,"toontalk.ini");
//								MessageBox(NULL,ini_file_name,"ini_file_name",MB_OK); // for debugging 191204
								if (!has_main_dir_entry(ini_file_name)) { // updated 170105 was OpenFile(ini_file_name,&file_struct,OF_EXIST) == -1) {
									if (!CopyFile(old_toontalk_ini,ini_file_name,TRUE)) {
										strcpy(dialog_path,module_directory);
										strcat(dialog_path,"Doc\\English\\cantwrite.htm");
										show_html_dialog(dialog_path,NULL,"");
										need_to_try_again = TRUE;
										//strcpy(ini_file_name,old_toontalk_ini); // just use the ini file that is on the CD
										//tt_ini_file = copy_string(ini_file_name);
									} else { // copied OK
										SetFileAttributes(ini_file_name,FILE_ATTRIBUTE_NORMAL); // new on 191204
//										MessageBox(NULL,ini_file_name,"writing ini_file_name",MB_OK); // for debugging 191204
										WritePrivateProfileString("Directories","MainDir",module_directory,ini_file_name);
										char sub_directory[MAX_PATH];
										strcpy(sub_directory,module_directory);
										strcat(sub_directory,"Temp");
										WritePrivateProfileString("Directories","TempDir",sub_directory,ini_file_name);
										strcpy(sub_directory,module_directory);
										strcat(sub_directory,"Java");
										WritePrivateProfileString("Directories","BuiltinPictureDir",sub_directory,ini_file_name);
										WritePrivateProfileString("Directories","FileSearchPath",sub_directory,ini_file_name);
										if (!using_my_documents) {
											WritePrivateProfileString("Directories","UserFiles",user_directory,ini_file_name);
											ini_file_name_overridden = TRUE; // so the new ini file is used 
										};
										char tt_ftp_ini_file[MAX_PATH];
										strcpy(tt_ftp_ini_file,module_directory);
										strcat(tt_ftp_ini_file,"tt_ftp.ini");
										if (OpenFile(tt_ftp_ini_file,&file_struct,OF_EXIST) != -1) {
											char local_tt_ftp_ini_file[MAX_PATH];
											strcpy(local_tt_ftp_ini_file,user_directory);
											strcat(local_tt_ftp_ini_file,"tt_ftp.ini");
											CopyFile(tt_ftp_ini_file,local_tt_ftp_ini_file,TRUE);
											SetFileAttributes(local_tt_ftp_ini_file,FILE_ATTRIBUTE_NORMAL); // new on 191204
										};
//										toontalk_installed = TRUE; // well at least the CD-ROM is available
									};
								} else {
									// using an old non-standard ini file -- new on 151204
									ini_file_name_overridden = TRUE; // so the new ini file is used 
//									toontalk_installed = TRUE; // well at least the CD-ROM is available
								};
								tt_ini_file = copy_string(ini_file_name);
							};
						};
					};
				};
			};
		};
		// following made obsolete on 151204
     // if (!toontalk_installed) {
     //    message_box_up = TRUE;
		   //if (MessageBox(NULL,"ToonTalk is not installed. Want to install it now?","Start ToonTalk", 
					//			// was S(IDS_START_TOONTALK) prior to 151204 but it may not be accessible
					//		   MB_ICONQUESTION|MB_OKCANCEL) == IDOK) {
			  // char setup_path[MAX_PATH];
			  // strcpy(setup_path,device_of_this_app);
			  // strcat(setup_path,"\\Install\\setup.exe");
			  // if ((int) ShellExecute(NULL, "open", setup_path, NULL, NULL, SW_SHOW) <= 32) { // was < 32 prior to 250402
				 //  MessageBox(NULL,"Couldn't find installation files. Sorry.","Start ToonTalk",MB_ICONSTOP|MB_OK);
					//// was S(IDS_START_TOONTALK) prior to 151204 but it may not be accessible
			  // };
			  // PostQuitMessage(0);	  // quit since install will offer to run toontalk again
		   //};
		   //message_box_up = FALSE;
		   //return(TRUE); // installation problem will run this (starttt) again.
     // };
	};
   char completed[2];
	message_box_up = TRUE;
   if (!running_read_only && GetPrivateProfileString("Switches","InstallCompleted","",completed,2,tt_ini_file) == 0) {
      // so you only get warning once -- use 2 rather than 1 to keep to history
      WritePrivateProfileString("Switches","InstallCompleted","2",tt_ini_file);
      if (MessageBox(NULL,"Installation did not complete normally. You may need to re-install.\rShould I try to run it anyway?","Start Toontalk",
						   MB_ICONQUESTION|MB_OKCANCEL) != IDOK) {
		   PostQuitMessage(0);
         return(TRUE);
      };
   };
   // new on 291002
   tt_ok_to_generate_mini_dump_message = ini_entry("User","MessageAskingToReportCrash");
   if (tt_ok_to_generate_mini_dump_message == NULL) {
      tt_ok_to_generate_mini_dump_message = copy_string("I'm sorry but ToonTalk crashed. Will you please help us fix the problem?");
   };
   tt_mini_dump_email = ini_entry("User","EmailToReportCrash");
   if (tt_mini_dump_email == NULL) {
      tt_mini_dump_email = copy_string("mailto:support@toontalk.com?subject=ToonTalk Crash Report&body=Please add any information that might be useful. And please drag the crash dump file here.");
   };
   int show_mini_dump_directory = ini_int("Switches","ShowCrashDirectoryAfterCrashes");
   if (show_mini_dump_directory != min_long) {
      tt_show_mini_dump_directory = show_mini_dump_directory;
   };
	int full_memory_dump = ini_int("Switches","OKToCreateVeryLargeCrashReports");
	if (full_memory_dump != min_long) {
      tt_full_memory_dump = full_memory_dump;
   };
   // moved here on 300402
   //	GetPrivateProfileString("Directories","MainDir","",toontalk_directory,sizeof(toontalk_directory),tt_ini_file);
	toontalk_directory = ini_entry("Directories","MainDir"); // re-wrote the above on 070201
	if (toontalk_directory == NULL) {
		MessageBox(NULL,"The toontalk.ini file doesn't have a setting for MainDir so Start ToonTalk doesn't know where any of ToonTalk's files are.",
					  "Start ToonTalk",MB_ICONSTOP|MB_OK);
		exit(-1);
	} else if (toontalk_directory[0] == '?') { // new on 191204
		strcpy(toontalk_directory,expand_question_mark_as_module_path(toontalk_directory));
	};
//   cdrom_installation = (GetPrivateProfileString("Directories","CDROMdir","",cdrom_directory,sizeof(cdrom_directory),tt_ini_file) > 0);
	cdrom_directory = ini_entry("Directories","CDROMdir"); // re-wrote the above on 070201 
	cdrom_installation = (cdrom_directory != NULL); // obsolete?? -- asked 191204
   use_html_dialog_file = (GetPrivateProfileString("Executables","HTMLStartTT","",html_dialog_file,MAX_PATH,tt_ini_file) > 0); 
	// there is such an entry
  	strcpy(doc_subdir,"Doc\\"); // moved here on 10304
	strcat(doc_subdir,S(IDS_DOC_SUBDIRECTORY_NAME));
	if (use_html_dialog_file) {
//      beginning_html_dialog_file = copy_string(html_dialog_file);
      strcpy(beginning_html_dialog_file,html_dialog_file);
      html_dialog_file_is_from_HTMLStartTT = TRUE;
   };
   strcpy(current_command_line,original_command_line);
   // now to remove -next_dialog and its arg from original_command_line since it only makes sense the first time
	// (otherwise loops back to this dialog)
   string next_dialog = strstr(original_command_line,"-next_dialog "); // new on 161002
   if (next_dialog != NULL) {
      int length = strlen("-next_dialog ");
      for (int i = 0; i < length; i++) {
         next_dialog[i] = ' '; // spaces are ignored
      };
      string remainder = next_dialog+length;
      length = strlen(remainder);
      boolean inside_quote = FALSE;
      for (int i = 0; i < length; i++) {
         if (remainder[i] == '"') {
            inside_quote = !inside_quote;
         } else if (remainder[i] == ' ' && !inside_quote) {
            break;
         };
         remainder[i] = ' ';
      };
   };
   while (TRUE) {
      new_command_line = FALSE; // unless html dialog changes it
      boolean result = interpret_command_line(current_command_line);
      MSG msg;
      while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) { // new on 210602 so messages are taken care of while in this loop
         if (msg.message == WM_QUIT) return(result);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		};
      if (!new_command_line) return(result);
   };
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
   int result = WinMain_internal(hInstance,hPrevInstance,lpCmdLine,nCmdShow);
	if (!ini_file_name_overridden) { // condition new on 191204 since now only needed if My Docs are involved
		copy_toontalk_ini_to_old_location(); // new on 161002 so any changes to toontalk.ini apply for old demos and new Windows users as well
	};
   return(EXIT_SUCCESS); // updated on 090805 since 1 means error exit was (result);
};

void copy_string_up_to_space(string target, string source) {
   int length = strlen(source);
   boolean inside_a_quote = FALSE; // new on 151002
   int target_index = 0;
   for (int i = 0; i <= length; i++) { // <= to copy the terminator as well if there is nothing else
      if (source[i] == '"') {
         inside_a_quote = !inside_a_quote;
      } else if (inside_a_quote || source[i] != ' ') {
         target[target_index++] = source[i];
      } else {
         target[target_index++] = '\0'; // terminate it
         break; // new on 151002
      };
   };
};

const int answer_offset = 1;

string *additions = NULL;

string option_answer(int answer_number) {
   int offset = current_option*10+answer_offset+answer_number;
   string answer = S(IDS_OPTION1+offset);
   if (additions != NULL && additions[offset] != NULL && answer != NULL) {
      strcat(answer,additions[offset]);
   };
   return(answer);
};

void add_to(int option, int answer, string addition) {
   if (additions == NULL) {
      additions = new string[option_count*10];
      for (int i = 0; i < option_count*10; i++) additions[i] = NULL;
   };
   string old_addition = additions[option*10+answer_offset+answer];
   if (old_addition != NULL) {
      delete [] old_addition;
   };
   additions[option*10+answer_offset+answer] = addition;
};

void remove_addition(int option, int answer) {
   if (additions == NULL) return;
   string old_addition = additions[option*10+answer_offset+answer];
   if (old_addition == NULL) return;
   delete [] old_addition;
   additions[option*10+answer_offset+answer] = NULL;
};

string process_language_name(string name) {
  switch (tt_language) {
     case AMERICAN:
     case BRITISH:
     case BRAZILIAN_PORTUGUESE: // for now
        {
        string new_name = new char[strlen(name)+2];
        strcpy(new_name,name);
        strcat(new_name,".");
//        delete [] name;
        return(new_name);
        };
     default:
        {
        int id = 0;
        if (stricmp(name,"english") == 0) {
           id = IDS_ENGLISH_CHOICE;
        } else if (stricmp(name,"swedish") == 0) {
           id = IDS_SWEDISH_CHOICE;
        }; // more can be added here
        string replacement = NULL; // updated on 280699 so English names of the languages are used when no alternatives are known
		  if (id != 0) replacement = SC(id);
        if (replacement == NULL || replacement[0] == NULL) return(name);
//        delete [] name;
        return(replacement);
        };
  };
};

void update_defaults() { // based upon current state of toontalk.ini
//   if (!default_setting("32Bit")) {
//      answers[VERSION_OPTION] = 0; // 16-bit is the old default
//   } else if (!default_setting("640x480")) {
//      answers[VERSION_OPTION] = 1;
//   } else {
//      answers[VERSION_OPTION] = 2;
//   };
	int i;
   for (i = first_option; i <= last_option; i++) {
      int old_default = default_setting(option_names[i]);
      if (old_default != 0) answers[i] = old_default-1;
   };
	// by commenting out the following (and fixing toontalk.rul) on 271299 
	// all the unavailable options are disabled not just the 2 of them
//   if (!ini_int("Versions","16Bit")) {
//      add_to(VERSION_OPTION,0,SC(IDS_OPTION_NOT_INSTALLED));
//      disable_answer(VERSION_OPTION,0);
//      answers[VERSION_OPTION] = 1; // 0th option not available
//   };
//   if (!ini_int("Versions","32Bit")) {
//      add_to(VERSION_OPTION,1,SC(IDS_OPTION_NOT_INSTALLED));
//      disable_answer(VERSION_OPTION,1);
//      add_to(VERSION_OPTION,2,SC(IDS_OPTION_AVAILABLE_ONLY_ON_CDROM));
//      disable_answer(VERSION_OPTION,2);
//      answers[VERSION_OPTION] = 0;
//   } else 
	/*
	// commented out the following on 200300 since can run with 320x240 graphics now
	if (!ini_int("Versions","640x480")) {
//      question_count[VERSION_OPTION]--;
		for (int i = 1; i < question_count[VERSION_OPTION]; i++) {
			add_to(VERSION_OPTION,i,SC(IDS_OPTION_AVAILABLE_ONLY_ON_CDROM));
			disable_answer(VERSION_OPTION,i);
		};
      if (answers[VERSION_OPTION] > 1) { // not clear how it got set like this -- user mucked around??
         answers[VERSION_OPTION] = 1; // high resolution isn't available but 32-bit is
      };
   };
	*/
	if (languages != NULL) delete [] languages;
	languages = new string[max_choices];
	string key = copy_string("Language0");
	language_count = 0;
	for (i = 0; i < max_choices; i++) { 
		itoa(i+1,key+8,10);
		languages[i] = ini_entry("Versions",key);
		if (languages[i] == NULL) break;
		language_count++;
	};
	if (language_count > 1) { // give user a choice
      question_count[LANGUAGE_OPTION] = language_count;
		for (int i = 0; i < language_count; i++) {
			add_to(LANGUAGE_OPTION,i,process_language_name(languages[i])); 
      };
   };
};

VOID CALLBACK TimerProc(HWND hwnd,         // handle to window
                        UINT uMsg,         // WM_TIMER message
                        UINT_PTR idEvent,  // timer identifier
                        DWORD dwTime) {    // current system time
   // new on 180602 based upon the handling of WM_TIMER: messages - this way it works in HTML or old mode
//   if (idEvent != 7023) return; // not my timer -- then how did this get called?
	HWND toontalk_window = FindWindow("ToonTalk",NULL); // was toontalk_exe_name
	if (toontalk_window == NULL && !toontalk_running_when_timer_set) { //  && !message_box_up
      // too easy to be non-active in a kiosk context
		// my timer triggered this and no ToonTalk running (now or when timer set)
		if (!LaunchApplication(next_demo)) {
			message_box_up = TRUE;
			MessageBox(NULL,"Sorry. Something went wrong trying to launch a demo.\rRestarting your computer or re-installing ToonTalk might help.",
							S(IDS_START_TOONTALK),MB_ICONSTOP|MB_OK);
			PostMessage(hwnd, WM_QUIT, 0, 0);
         return;
		};
      if (autodemo == REPEAT_INTRO) {
         next_demo = IDS_INTRO_DEMO;
      } else {
			next_demo++;
			if (next_demo > IDS_APPEND_DEMO) next_demo = IDS_INTRO_DEMO;
      };
	};
	SetTimer(hwnd, 7023, max_idle_time, TimerProc); // check again in a while
	toontalk_running_when_timer_set = (toontalk_window != NULL);
};

void back_to_previous_dialog() { // abstracted 010205
	if (last_dialog_index > 0) {
		strcpy(html_dialog_file,html_dialog_file_history[last_dialog_index-1]);
		delete [] html_dialog_file_history[last_dialog_index-1];
		last_dialog_index--;
		new_command_line = TRUE; // since the dialog worked fine so interpret the command line again - new on 210602 
	}; // else??
};

boolean interpret_command_line(string incoming_command_line) { // abstracted on 300402 so can be used by HTML dialog stuff
   int length =  0;
	string command_line = NULL;
   if (incoming_command_line != NULL) {
      length = strlen(incoming_command_line);
		command_line = new char[length+1]; // in case we need to strip off quotes below -- minor leak
		if (incoming_command_line[0] == '"' && incoming_command_line[length-1] == '"') { // new on 050903 - file name is quoted
			memcpy(command_line,incoming_command_line+1,length-2);
			length -= 2;
			command_line[length] = '\0';
		} else {
			strcpy(command_line,incoming_command_line);
		};
      if (command_line[0] != '-' && length > 4) { // new on 220602, in case command ended with this - e.g. -city foo.cty ?
         // added length > 4 condition on 161002 since the calls to stricmp aren't proper if too short
		   // new on 291199 - since now starttt deals with double clicking
		   DMO_file = (stricmp(command_line+length-4,".dmo") == 0);
		   if (!DMO_file) {
			   USR_file = (stricmp(command_line+length-4,".usr") == 0);
			   if (!USR_file) {
				   CTY_file = (stricmp(command_line+length-4,".cty") == 0);
				   if (!CTY_file) {
					   PZL_file = (stricmp(command_line+length-4,".pzl") == 0);
					   if (!PZL_file) {
						   TT_file = (stricmp(command_line+length-3,".tt") == 0);  // new on 032000
                     if (TT_file) {
                        if (length > 7 && (stricmp(command_line+length-7,".htm.tt") == 0 || stricmp(command_line+length-8,".html.tt") == 0)) {
                           // new on 220602 so can double click on a dialog file
                           string insert = "-next_dialog \""; // quote is new on 261103
                           int insert_length = strlen(insert);
                           // first shift over by insert_length and then copy insert into command_line
                           for (int i = length; i >= 0; i--) { // should copy the terminating null as well
                              current_command_line[i+insert_length] = current_command_line[i];
                           };
									current_command_line[length+insert_length] = '\"'; // new on 261103
                           memcpy(current_command_line,insert,insert_length);                        
                           new_command_line = TRUE;
                           TT_file = FALSE; // not a normal one
									original_command_line[0] = '\0'; // new on 040903
                           return(TRUE);
                        };
                     };
					   };
				   };
			   };
		   };
      };
      //if (TT_file) {
      //   if (length >= 11 && stricmp(command_line+length-11,"stoption.tt") == 0) { // new on 080402
      //      // dummy TT file just for indicating want to set options - could be paranoid and check that it indeed contains 1 byte
      //      TT_file = FALSE;
      //      set_options_now = TRUE;
      //   } else if (length >= 11 && stricmp(command_line+length-11,"pickdemo.tt") == 0) { // new on 250402
      //      // dummy TT file just for indicating want to pick demo
      //      char hta_file[MAX_PATH];
      //      strcpy(hta_file,command_line);
      //      int length = strlen(hta_file);
      //      strcpy(hta_file+length-2,"hta"); // replace .tt with .hta
      //      if ((int) (ShellExecute(NULL,"open",hta_file,NULL,NULL,SW_SHOWMAXIMIZED)) < 32) {
				   // MessageBox(NULL,"Couldn't find pickdemo.hta file. Sorry. Perhaps you need to re-install.",
									  //S(IDS_START_TOONTALK),MB_ICONSTOP|MB_OK);
      //      };
      //      PostQuitMessage(0);
      //      return(TRUE);
      //   } else if (length >= 7 && stricmp(command_line+length-7,"help.tt") == 0) { // new on 250402
      //      // dummy TT file just for indicating want help
      //      // too early to call launch_help since haven't found toontalk_directory and the like
      //      char html_file[MAX_PATH];
      //      strcpy(html_file,command_line);
      //      int length = strlen(html_file);
      //      strcpy(html_file+length-2,"htm"); // replace .tt with .htm
      //      if ((int) (ShellExecute(NULL,"open",html_file,NULL,NULL,SW_SHOWMAXIMIZED)) < 32) {
				   // MessageBox(NULL,"Couldn't find help.htm file. Sorry. Perhaps you need to re-install.",S(IDS_START_TOONTALK),MB_ICONSTOP|MB_OK);
      //      };
      //      PostQuitMessage(0);
      //      return(TRUE);
      //   };
      //};
	   if (ini_int("Switches","UseLocalIniFilesWhenLoading") && (DMO_file || USR_file || CTY_file || PZL_file || TT_file)) { // new on 070201
			// ini_int("Switches","UseLocalIniFilesWhenLoading") new on 030304
		   BOOL on_cdrom;
		   int last_period = -1;
		   for (int i = length-1; i > 0; i--) {
			   if (command_line[i] == '.') {
				   last_period = i;
				   command_line[i] = '\0'; // temporarily terminate it
				   break;
			   };
		   };
		   tt_local_ini_file = existing_file_name(on_cdrom, command_line, "ini", NULL, FALSE);
		   if (last_period > 0) { // restore period
			   command_line[last_period] = '.';
		   };
      }; 
		// new on 260103 to remove else so the following happens when USR_file (doesn't seem to hurt for the others) 
		// following new on 300402
         if (strstr(command_line,"-next_dialog ") != NULL) { 
            html_dialog_file_history[last_dialog_index++] = copy_string(html_dialog_file);
            string prefix = "-next_dialog ";
            char new_html_dialog_file[MAX_PATH];
            copy_string_up_to_space(new_html_dialog_file,strstr(command_line,prefix)+strlen(prefix));
				if (strchr(new_html_dialog_file,':') == NULL) {
               int length = strlen(html_dialog_file);
               // if relative should be intrepreted wrt to previous html_dialog_file
					boolean found_path = FALSE; // new on 100304 to deal with previous name not being a full path
               if (length > 0) {
                  for (int i = length-1; i > 0; i--) { // go backwards looking for either kind of slash
                     if (html_dialog_file[i] == '/' || html_dialog_file[i] == '\\') {
                        html_dialog_file[i+1] = '\0'; // terminate instead
								found_path = TRUE;
                        break;
                     };
                  }; // what if never found?? -- dealt with now (100304)
               };
					if (!found_path) {
						BOOL ignore_on_cdrom;
						string full_html_dialog_file = existing_file_name(ignore_on_cdrom,new_html_dialog_file,"htm",doc_subdir,FALSE); // new on 100304
						if (full_html_dialog_file != NULL) {
							strcpy(html_dialog_file,full_html_dialog_file);
							delete [] full_html_dialog_file;
						} else {
							strcat(html_dialog_file,new_html_dialog_file);
						};
					} else {
						strcat(html_dialog_file,new_html_dialog_file);
					};
            } else { // new on 220602
               strcpy(html_dialog_file,new_html_dialog_file);
            };
            html_dialog_file_is_from_HTMLStartTT = FALSE;
            use_html_dialog_file = TRUE;
            // commented this out and made following "else if" since too hard to run dialogs and other programs since
            // dialog tends to cover them up (new on 020502) 
            // also all return(TRUE) below was conditional: if (!use_html_dialog_file) 
   //         } else if (strstr(command_line,"-keep_dialog 1") != NULL) { // keep the current dialog page
   //            use_html_dialog_file = TRUE;
         } else if (strstr(command_line,"-next_puzzle 1") != NULL || strstr(command_line,"-puzzle") != NULL || 
						  strstr(command_line,"-free_play 1")) {
            // second disjunct added on 110702 
				// added third disjunc so can have desk top icons that set flags and jump to free_play
            LaunchApplication(-2); // run these without giving the user much choice
		      return(TRUE); 
         } else if (strstr(command_line,"-set_options 1") != NULL) {
            set_options_now = TRUE;
            use_html_dialog_file = FALSE; // new on 210602
         } else if (strstr(command_line,"-help 1") != NULL || strstr(command_line,"-browse ") != NULL) {
				// -browse is new on 010205
				::CloseWindow(window_handle); // new on 010205
				if (strstr(command_line,"-help 1") != NULL) {
					launch_help("help.htm");
				} else {
					string file_name = strstr(command_line,"-browse ")+strlen("-browse ");
					string first_space = strchr(file_name,' ');
					if (first_space != NULL) {
						first_space[0] = '\0'; // restored in a second
					};
					launch_help(file_name);
					if (first_space != NULL) {
						first_space[0] = ' '; // restored 
					};
				};
				MSG msg;
				paused = TRUE;
				use_html_dialog_file = TRUE; // continue when maximized again
				while (paused && GetMessage(&msg, NULL, 0, 0) != -1) { // wait to be maximized or closed
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				};           
 //         return(TRUE); // commented out on 010205
         } else if (strstr(command_line,"-I ") != NULL) {
            string prefix = "-I ";
            copy_string_up_to_space(demo_name,strstr(command_line,prefix)+strlen(prefix));
            LaunchApplication(-3); // new code which means use existing demo_name
		      return(TRUE);
         } else if (html_dialog_returned_command_line) {
            LaunchApplication(-2); // run these without giving the user much choice
		      return(TRUE);
         };
 //     };
      if (strstr(command_line,"-autodemo 1") != NULL) {
         autodemo = REPEAT_INTRO;
      } else if (strstr(command_line,"-autodemo 2") != NULL) {
         autodemo = REPEAT_ALL_DEMOS;
      } else if (default_setting("KindOfUser") == 3) { // "kiosk or museum mode"
         autodemo = REPEAT_INTRO;
      }; // default is NO_AUTODEMO
   };
   message_box_up = FALSE;
//HFILE file = open_starttt_log_file(); // for debugging
//_lwrite(file,command_line,strlen(command_line));
//_lclose(file);
	if (TT_file || DMO_file || CTY_file) { // new on 250300
		// added DMO and CTY files on 260105 though it really should only be if X version is enabled
	   HWND hwnd = FindWindow("ToonTalk",NULL);
		if (hwnd != NULL) {
			// need to put command_line somewhere for ToonTalk to read so
			WritePrivateProfileString("User","TTFileLastDoubleClicked",command_line,tt_ini_file);
			PostMessage(hwnd,WM_USER+FILE_FOR_CLIPBOARD_MESSAGE_ID,0,0);
			return(TRUE);
		};
	};
	if (DMO_file || CTY_file || PZL_file | TT_file) { // new on 291199
		LaunchApplication(-2); // run these without giving the user much choice
		return(TRUE); // I guess
	};
   if (!already_initialized) {
      // followed move here on 210602 so if already running StartTT this will "defer" to the other even when using HTML dialogs
      window_handle = FindWindow ("StartTT", NULL); // was app_name
	   if (window_handle != NULL) {
		   // We found another version of ourself. Lets defer to it:
		   if (IsIconic(window_handle)) {
			   ShowWindow(window_handle, SW_RESTORE);
		   } else {
            SetForegroundWindow(window_handle);
		   };
		   return(FALSE);
	   };
	   if (!InitApplication(hInst)) {
		   return(FALSE);
	   };
      if (!InitInstance()) {
		   return(FALSE);
	   };
      already_initialized = TRUE;
   };
	boolean ok_to_update_encoding_state = TRUE; // new on 310105
	boolean handling_set_option_dialog = FALSE; // new on 010205
	boolean set_entry_in_local_ini = FALSE; // new on 020205
	char set_option_dialog_file_name[MAX_PATH]; // new on 010205
   try { // new on 021203 to catch errors and recover by using old interface
		if (use_html_dialog_file) { // new on 090402 to provide an HTML alternative to this interface
			OFSTRUCT file_struct;
			while (use_html_dialog_file) {
				BOOL on_cdrom = FALSE; // new on 110304
				string full_html_dialog_file = NULL; // moved out here on 110304
				if (OpenFile(html_dialog_file,&file_struct,OF_EXIST) == -1) {
					full_html_dialog_file = existing_file_name(on_cdrom,html_dialog_file,"htm",doc_subdir,FALSE); // new on 161002
					if (full_html_dialog_file == NULL) {
						// following seems to hackish -- what is special about StartTT.htm?
						//if (stricmp(html_dialog_file,"StartTT.htm") != 0) {
							// new on 091005 since might be reset because 
							// one language version has a special starttt HTML dialog
						//	full_html_dialog_file = existing_file_name(on_cdrom,"StartTT.htm","htm",doc_subdir,FALSE); 
						//};
                  if (full_html_dialog_file == NULL) {
							// following commented out since an error message is better than the following
							// which can even just terminate without a message
							//if (html_dialog_file_is_from_HTMLStartTT) { // new on 161002
							//	new_command_line = FALSE;
							//	use_html_dialog_file = FALSE;
							//	PrepareWindow(window_handle);
							//	return(interpret_command_line(command_line));
							//};
							// new on 100304
							char message[MAX_PATH+300];
							strcpy(message,"Sorry. ToonTalk could not find the dialog file: ");
							strcat(message,html_dialog_file);
							MessageBox(NULL,message,"Start ToonTalk",MB_ICONINFORMATION);
							return(FALSE); // new on 151002 to deal better with missing files
						};
					};
					strcpy(html_dialog_file,full_html_dialog_file);
					strcpy(beginning_html_dialog_file,full_html_dialog_file); // to terminate properly            
//					delete [] full_html_dialog_file;
				};
				char local_file_name[MAX_PATH] ; // new on 040903 -- might end up be identical - I'm assuming copyFile is a no-op then
				if (!ok_to_update_encoding_state) { // new on 310105
					strcpy(local_file_name,html_dialog_file);
				} else if (!on_cdrom) { // conditional new on 110304
					// dialog files should only be run from the subdirectory of doc since it needs common.js, a background, style sheet, etc.
					strcpy(local_file_name,toontalk_directory);
					strcat(local_file_name,doc_subdir);
					int length = strlen(html_dialog_file);
					int short_name_start = 0;
					for (int i = length-1; i > 0; i--) {
						if (html_dialog_file[i] == '\\' || html_dialog_file[i] == '/') {
							short_name_start = i+1;
							break;
						};
					};
		//			strcat(local_file_name,"\\copy_of_"); // a bit of a hack that should protect the existing files -- removed on 050903
					strcat(local_file_name,"\\");
					strcat(local_file_name,html_dialog_file+short_name_start);
					CopyFile(html_dialog_file,local_file_name,FALSE);
				} else {
					strcpy(local_file_name,full_html_dialog_file); // new on 110304
				};
				if (full_html_dialog_file != NULL) delete [] full_html_dialog_file; // new on 110304
				// use_html_dialog_file in case backing out of "sub" dialogs
				// and it exists and can be found - note it currently can't be a URL -- but why not if didn't check for existence?
				if (autodemo != NO_AUTODEMO) {   
					SetTimer(window_handle, 7023, max_idle_time, TimerProc); // if idle for long time start running demos
					toontalk_running_when_timer_set = (FindWindow("ToonTalk", NULL) != NULL); 
				};
				if (ok_to_update_encoding_state) { // condition new on 310105
					update_encoded_current_state();
				};
#if TT_DEBUG_STARTTT
				string from_dialog = show_html_dialog(local_file_name,NULL,encoded_current_state); // too hard to debug otherwise
#else
				string from_dialog = show_html_dialog(local_file_name,window_handle,encoded_current_state);
#endif
				MSG msg;
				while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) { // new on 210602 so messages are taken care of while in this loop
					if (msg.message == WM_QUIT) return(FALSE);
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				};
				if (from_dialog == NULL) {
					// new on 220602
					HFILE file = open_starttt_log_file();
					string message = "Failed to show HTML dialog: ";
					_lwrite(file,message,strlen(message));
					_lwrite(file,html_dialog_file,strlen(html_dialog_file));
					message = " Command line is ";
					_lwrite(file,message,strlen(message));
					_lwrite(file,command_line,strlen(command_line));
					_lclose(file);
				} else {
					string from_dialog_remaining = from_dialog;
					string cancel_settings = strstr(from_dialog,"-cancel_settings");
					string previous_dialog = strstr(from_dialog,"-previous_dialog");         
					if (cancel_settings != NULL) {
						if (old_toontalk_ini_contents != NULL) {
							copy_string_to_file(old_toontalk_ini_contents,old_toontalk_ini_file_size,tt_ini_file);
						};
						new_command_line = TRUE;  // new on 210602 so go back 
						strcpy(html_dialog_file,beginning_html_dialog_file); // new on 170602 - cancelled 'Set Options' but go back to start
					} else if (previous_dialog != NULL) {
						back_to_previous_dialog();						
					} else {  // new structure on 170602 so the following doesn't happen if cancel or back clicked       
						string system_defaults_option = strstr(from_dialog,"-system_defaults_option ");
						string set_option = strstr(from_dialog,"-set_option(");
						string accept_settings = strstr(from_dialog,"-accept_settings");
						string backup_settings = strstr(from_dialog,"-backup_settings");
						if (backup_settings != NULL) {
							// put current state of toontalk.ini file into memory in case -cancel_settings comes up
							if (old_toontalk_ini_contents != NULL) delete [] old_toontalk_ini_contents;
							old_toontalk_ini_contents = copy_file_to_string(tt_ini_file,old_toontalk_ini_file_size); // to do
						};
						if (system_defaults_option != NULL) {
							system_defaults_option += strlen("-system_defaults_option ");
							system_defaults = system_defaults_option[0]-'0'; // should be 1, 2, 3, or 4 
						};
						string set_option_remaining = set_option;
						while (set_option_remaining != NULL) { // was if prior to 040903
							const int set_option_string_length = 12; // strlen("-set_option(")
							set_option += 12;
							while (set_option[0] == ' ') set_option++; // skip over white space
							string args[3]; // Category, Key, Value               
							int args_filled = 0;
							for (int arg_index = 0; arg_index < 3; arg_index++) {
								int arg_length = 0; // in args_string
								char stop_character; // look for comma unless last arg then for close paren
								if (arg_index == 2) {
									stop_character = ')';
								} else {
									stop_character = ','; 
								};
								int total_length = strlen(set_option); // could be more clever for handling malformed strings
								for (int i = 0; i < total_length; i++) {
									if (set_option[i] == stop_character) {
										arg_length = i;
										if (stop_character == ')') { // new on 040903
											// finished processing this option so truncate part processed
											set_option_remaining = strstr(set_option_remaining+set_option_string_length,"-set_option(");
										};
										break;
									};
								};
								if (arg_length > 0) {
									args[arg_index] = new char[arg_length+1];
									memcpy(args[arg_index],set_option,arg_length);
									args[arg_index][arg_length] = '\0'; // terminate string
									args_filled++;
								}; // else warn?
								set_option += arg_length+1; // skip over comma as well
								while (set_option[0] == ' ') set_option++; // skip over white space
							};
							if (args_filled == 2) {
								args[2] = new char[1];
								args[2][0] = '\0'; // empty value
								args_filled = 3;
							};
							if (args_filled == 3) {
								if (strcmp(args[2],"***?***") == 0) { // new on 310105
									// user wants to set this option -- use special dialog to accomplish this
									strcpy(set_option_dialog_file_name,html_dialog_file); // save this to come back to
									string current_value = ini_entry(args[0],args[1]);
									strcpy(html_dialog_file,toontalk_directory);
									strcat(html_dialog_file,doc_subdir);
									strcat(html_dialog_file,"\\setopt.htm");
									strcpy(encoded_current_state,"category;");
									strcat(encoded_current_state,args[0]);
									strcat(encoded_current_state,";key_name;");
									strcat(encoded_current_state,args[1]);
									strcat(encoded_current_state,";old_value;");
									if (current_value != NULL) {
										strcat(encoded_current_state,current_value);
                              delete [] current_value;
										set_entry_in_local_ini = ini_entry_defined_in_local_ini_file(args[0],args[1]);
									} else {
										strcat(encoded_current_state,"***value not yet set***");
										set_entry_in_local_ini = FALSE;
									};
									ok_to_update_encoding_state = FALSE; // for the next cycle since done specially above
									handling_set_option_dialog = TRUE;									
								} else {
									ok_to_update_encoding_state = TRUE; // new on 020205 so next time is treated as normal
									string ini_file_to_update;
									if (set_entry_in_local_ini) {
										// new on 020205
										ini_file_to_update = tt_local_ini_file;
										set_entry_in_local_ini = FALSE;
									} else {
										ini_file_to_update = tt_ini_file;
									};
									// changed for else if on 020205
									if (strcmp(args[2],"***value not yet set***") == 0) { 
									// new on 010205 to do nothing if had no value and wasn't changed
									} else if (!WritePrivateProfileString(args[0],args[1],args[2],ini_file_to_update)) { // updated 020205 was tt_ini_file
										// should this only be if debugging is on??
										int error_code = GetLastError();
										char message[1024];
										strcpy(message,"ToonTalk was unable to update the file "),
										strcat(message,ini_file_to_update);
										strcat(message,". Windows returned the error code #");
										itoa(error_code,message+strlen(message),10);
										LPVOID lpMsgBuf;
										if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
																FORMAT_MESSAGE_FROM_SYSTEM | 
																FORMAT_MESSAGE_IGNORE_INSERTS,
																NULL,
																error_code,
																MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
																(LPTSTR) &lpMsgBuf,
																0,
																NULL)) {
											strcat(message,".\nIt is described as ");
											if (lpMsgBuf != NULL) {
												strcat(message,(string) lpMsgBuf);
											};
										};
										MessageBox(NULL,message,"Start ToonTalk",MB_OK);
		//                        cout << error << " is the error code from WritePrivateProfileString" << endl; 
									} else if (!handling_set_option_dialog) { // copied here on 170602 -- condition added 020205
										char time_string[33];
										if (system_defaults == 3) { // changing for all users 
											// from 230999 to 041199 things were broken since timeGetTime()/1000 replaced time(NULL)
											ltoa(time(NULL),time_string,10); // record time so user profiles can be updated
										} else {
											time_string[0] = NULL; // empty string
										};
										if (system_defaults > 0) { // condition new on 151002 for use by custom option pages (e.g. spoklang).
											WritePrivateProfileString("Defaults","LastChanged",time_string,tt_ini_file);
											if (strcmp(args[1],"Voice") == 0) { // need to do extra work when language is chosen (new on 180602)
												int option_index = atoi(args[2])-1; // one-indexing
												int mode_index = option_to_mode[option_index];
												WritePrivateProfileString("User","TextToSpeechMode",ModeInfo[mode_index].szModeName,tt_ini_file); 
												// prior to 201103 was .szSpeaker but that can be ambiguous
												string flag;
												if (ModeInfo[mode_index].dwFeatures&TTSFEATURE_ANYWORD) {
													flag = "1";
												} else {
													flag = "0";
												};
												WritePrivateProfileString("Switches","GeneralTextToSpeechEngine",flag,tt_ini_file);
											} else if (strcmp(args[1],"KindOfUser") == 0) {
												// copied here on 180602
												if (args[2][0] == '3') { // "kiosk" mode
													autodemo = REPEAT_INTRO;
													SetTimer(window_handle, 7023, max_idle_time, TimerProc); // if idle for long time start running demos
													toontalk_running_when_timer_set = (FindWindow("ToonTalk", NULL) != NULL);
												} else {
													autodemo = NO_AUTODEMO;
												};
											};
										};
									};
									if (handling_set_option_dialog) { // new on 010205 -- at different level of conditional as of 020205
										strcpy(html_dialog_file,set_option_dialog_file_name);
									};
								};
							};
							for (int i = 0; i < args_filled; i++) {
								delete [] args[i];
							};
						};
						if (accept_settings != NULL) { // should be after other options
							//PostQuitMessage(0);
							//return(TRUE);
							strcpy(html_dialog_file,beginning_html_dialog_file);
						} else if (dialog_closed(from_dialog)) { // dialog was just closed
							if (strcmp(html_dialog_file,beginning_html_dialog_file) == 0) {
								PostQuitMessage(0);
								return(TRUE);
							} else {
								handling_set_option_dialog = FALSE; // new on 020205
								strcpy(html_dialog_file,beginning_html_dialog_file); 
								// new on 170602 - why not accept what has been done so far and go back to first dialog?
							};
							//if (last_dialog_index > 0) {
							//   strcpy(html_dialog_file,html_dialog_file_history[last_dialog_index-1]);
							//   delete [] html_dialog_file_history[last_dialog_index-1];
							//   last_dialog_index--;
							//} else {
							//   PostQuitMessage(0);
							//   return(TRUE);
							//};
						} else if (!handling_set_option_dialog) { 
							// updated 020205 ok_to_update_encoding_state) { // condition new on 310105
							// is removed if running options and DONE is selected -- OK??
							if (original_command_line != NULL) {
								strcpy(current_command_line,from_dialog); // made this come before original_command_line
								strcat(current_command_line," ");
								strcat(current_command_line,original_command_line);
							} else {
								strcpy(current_command_line,from_dialog);
							};
							new_command_line = TRUE;
							use_html_dialog_file = FALSE; // unless new command line contains -next_dialog <file name>
							html_dialog_returned_command_line = TRUE; // so I don't put up the old dialog
						};
					};
					
					if (from_dialog != NULL) delete [] from_dialog;
	//           return(TRUE); // unless HTML dialog failed -- replaced below on 210602
					};
				};
				if (new_command_line) return(TRUE);
			};
		} catch (...) { // catch any error - new on 021203
			new_command_line = FALSE;
			use_html_dialog_file = FALSE;
			PrepareWindow(window_handle);
			return(interpret_command_line(command_line)); 
		};  
      /* rewritten on 150402 to use JavaScript to launch StartTT.htm so it is full screen but has minimize and close buttons
         and to work with any browser
      char html_file[MAX_PATH+3] = "-k "; // kiosk mode so it is full screen and no other distracting things
      if (GetPrivateProfileString("Executables","HTMLStartTT","",html_file+3,MAX_PATH,tt_ini_file) > 0) { // there is such an entry
         OFSTRUCT file_struct;
	      if (OpenFile(html_file+3,&file_struct,OF_EXIST) != -1) { // and it exists and can be found - note it can't be a URL
            int offset;
            if (GetPrivateProfileInt("Switches","HTMLStartTTKioskMode",1,tt_ini_file)) { // by default Kiosk mode is on
               offset = 0;
            } else {
               offset = 3;
            };
            if ((int) ShellExecute(NULL, "open", "iexplore.exe", html_file+offset, NULL, SW_SHOWMAXIMIZED) > 32) { // all went well
               return(TRUE);
            };
         };
      };
      */
/*
   // just for debugging
   timeb time_struct;
   ftime(&time_struct);
   OFSTRUCT time_file_struct;
   char time_string[33];
   HFILE file = OpenFile("c:\\time.txt",&time_file_struct,OF_CREATE);
   ltoa(time(NULL),time_string,10);
   _lwrite(file,time_string,strlen(time_string));
   ltoa(time_struct.time,time_string,10);
   _lwrite(file," ",1);
   _lwrite(file,time_string,strlen(time_string));
   itoa(time_struct.timezone,time_string,10);
   _lwrite(file," ",1);
   _lwrite(file,time_string,strlen(time_string));
   ltoa(time(NULL)-time_struct.timezone*60,time_string,10);
   _lwrite(file," ",1);
   _lwrite(file,time_string,strlen(time_string));
   _lclose(file);
*/
	// Perform application initialization:
   if (set_options_now) { // new on 080402
      option_dialogs(window_handle);
      return(TRUE);
   };
	GetModuleFileName(hInst, device_of_this_app, sizeof(device_of_this_app));
	hAccelTable = LoadAccelerators (hInst, "StartTT");

	// Main message loop:
   MSG msg;
	while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) { // prior to 010205 was GetMessage(&msg, NULL, 0, 0)) {
		int translated = TranslateAccelerator (msg.hwnd, hAccelTable, &msg);
		if (msg.message != WM_QUIT) {
			if (!translated) TranslateMessage(&msg);
			DispatchMessage(&msg);
      } else {
         break;
      };
	}
//	HWND toontalk_window = FindWindow("ToonTalk",NULL);
//	if (toontalk_window != NULL) {
//		SetActiveWindow(toontalk_window); // seems it sometimes gets buried
//	};
//	char temp_directory[256];
//	if (GetPrivateProfileString("Directories","TempDir","",temp_directory,sizeof(temp_directory),tt_ini_file) != 0 &&
	string temp_directory = ini_entry("Directories","TempDir");
   OFSTRUCT file_struct;
	if (temp_directory != NULL && FindWindow("ToonTalk",NULL) == NULL) { // was toontalk_exe_name
		char *file_name = new char[MAX_PATH];
		const int temp_file_count = 2;
		char *dat_files[temp_file_count] = {"resind.","m25."}; // removed "m22.", on 070201
 //     char *data_suffix = ini_entry("FileExtensions","DataFileSuffix");
		string data_suffix = copy_string("us1"); // made copy_string on 110304
		// since now use this only for old version -- new versions use the small version with the PNGs -- 080304
		for (int i = 0; i < temp_file_count; i++) { // not too elegant but it works
			strcpy(file_name,temp_directory);
			strcat(file_name,dat_files[i]);
         strcat(file_name,data_suffix);
			OpenFile(file_name,&file_struct,OF_DELETE);
		};
		delete [] file_name;
      delete [] data_suffix;
	};
	if (global_handle > 0) {
		GlobalUnlock(global_handle);
		GlobalFree(global_handle);
	};
   if (resource_library != NULL) FreeLibrary(resource_library);
   deallocate_resource_strings();
   if (language_arg != NULL) delete [] language_arg;
	return (msg.wParam);

}


/************************************************************************\
 *    FUNCTION: InitApplication
\************************************************************************/

BOOL InitApplication(HINSTANCE hInstance) {
	WNDCLASS  wc;
	// Fill in window class structure with parameters that describe
	// the main window.
	wc.lpszClassName = "StartTT"; //app_name;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = (WNDPROC)WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.lpszMenuName  = NULL;
	wc.hIcon         = LoadIcon (hInstance, "LARGEICON");
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);

	return RegisterClass(&wc);
}


/************************************************************************\
 *    FUNCTION: InitInstance
\************************************************************************/

BOOL InitInstance() { // dummy_window if using HTML dialogs
   //	HWND hWnd; -- made global on 210602
	char szTitle[64];	
   // first get the language right
   char *language = strstr(current_command_line,"-language ");
   if (language != NULL) {
      language += strlen("-language ");
      language_arg = copy_string(language); // copy so it can be deleted regardless of source
      tt_language = string_to_language(language);
   };
	// Lets replace the default window title, with the name of the
	// actual application we want to launch...
	LoadString (resource_source(), IDS_WINDOW_TITLE, szTitle, sizeof(szTitle));
   //if (use_html_dialog_file) {
   //   window_handle = CreateWindow("StartTT", szTitle,
		 //                             WS_MAXIMIZE,
		 //                             0, 0,
		 //                             0, 0,
		 //                             NULL, NULL, hInst, NULL);
   //} else {
	   window_handle = CreateWindow("StartTT", szTitle,
		                              WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX, // removed WS_THICKFRAME
                              //		WS_OVERLAPPEDWINDOW, // maximize doesn't make sense
		                              0, 0,
		                              0, 0,
		                              NULL, NULL, hInst, NULL);
   //};
	if (!window_handle) {
		return (FALSE);
	}

	if (tt_language == BRAZILIAN_PORTUGUESE) { // new on 200300
		char avi_file[MAX_PATH];
		strcpy(avi_file,toontalk_directory);
		strcat(avi_file,"320x240cpkb07.avi");
		OFSTRUCT file_struct;
		if (OpenFile(avi_file,&file_struct,OF_EXIST) == -1) { // not on ToonTalk directory so try CD-ROM
			strcpy(avi_file,cdrom_directory);
			strcat(avi_file,"320x240cpkb07.avi");
		};
		if (OpenFile(avi_file,&file_struct,OF_EXIST) != -1) {
			MCI_window = MCIWndCreate(window_handle,hInst,
												 MCIWNDF_NOAUTOSIZEMOVIE | // after debugging add MCIWNDF_NOERRORDLG 
												 MCIWNDF_NOMENU |
												 MCIWNDF_NOOPEN |
												 MCIWNDF_NOPLAYBAR |
												 MCIWNDF_NOTIFYMODE |
												 WS_CHILD |
												 WS_VISIBLE,
												 avi_file);
			if (MCI_window != NULL) MCIWndPlay(MCI_window);
		};
	};

// debuggin
//	HINSTANCE display_dib_library = LoadLibrary(DISPLAYDIB_DLL);
//	for (int i = 0; i < 1000; i++) {
//		HWND dispdib_window_handle = DisplayDibWindowCreate(hWnd,hInstance);
//      DisplayDibWindowBegin(dispdib_window_handle);
//		DisplayDibWindowClose(dispdib_window_handle);
//	};
//	FreeLibrary(display_dib_library);
	// Set the small icon
//#if !defined(WM_SETICON)
//#define WM_SETICON 0x0080
//#endif
//	SendMessage(hWnd, WM_SETICON, FALSE, (LPARAM)LoadIcon(hInst, "SMALL"));

   if (!set_options_now && !html_dialog_returned_command_line) { // conditional new on 080402
      ShowWindow(window_handle, original_CmdShow);
   	UpdateWindow(window_handle);
   };
	return (TRUE);
};


/************************************************************************\
 *    FUNCTION: WndProc
\************************************************************************/

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId; //, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	UINT uMappedColors; // Used in WM_QUERYNEWPALETTE
	RECT rect; // Used in WM_PAINT
	HDC  hdcSrc; // Used in WM_PAINT
	HBITMAP hbmOld; // Used in WM_PAINT

   int default_demo_id = 0; // free play

	switch (message) {

		case WM_CREATE:
			return PrepareWindow (hWnd);

		case WM_PALETTECHANGED:
			if ((HWND)wParam == hWnd) {
				break;
			}

			// fall through to WM_QUERYNEWPALETTE

		case WM_QUERYNEWPALETTE:
         if (!(fPalette || fTrueColor) || MCI_window != NULL) return(FALSE);
			hdc = GetDC(hWnd);
		   if (hpal) {
			   SelectPalette(hdc, hpal, FALSE);
		   }
		   uMappedColors = RealizePalette(hdc);
			ReleaseDC(hWnd,hdc);

			if (uMappedColors>0) {
				InvalidateRect(hWnd,NULL,TRUE);
				return TRUE;
			} else {
				return FALSE;
			}
//			break;

		case WM_COMMAND:
			wmId    = LOWORD(wParam); // Remember, these are...
//			wmEvent = HIWORD(wParam); // ...different for Win32!

			switch (wmId) {
            case IDM_SET_OPTIONS:
               option_dialogs(hWnd);
               break;
            case IDM_HELP:
               launch_help("help.htm");
               break;
            case IDM_ABOUT:
//#if TT_32
//					DialogBox(resource_library,"AboutBox",hWnd,about_dialog);
//#else
	            {
               DLGPROC dialog_proc = (DLGPROC) MakeProcInstance((FARPROC) about_dialog,resource_library);
					DialogBox(resource_library,MAKEINTRESOURCE(IDD_ABOUT_DIALOG),hWnd,dialog_proc);
					if (dialog_proc != NULL) FreeProcInstance((FARPROC) dialog_proc);
               };
//#endif
					break;
            case IDM_RUN_PUZZLES:
            case IDS_PLAY_PUZZLE_GAME:
               default_demo_id = -1;
               // and the following
				case IDS_FREE_PLAY: // free play chosen
					if (!LaunchApplication(default_demo_id)) {
						// Failed to launch your application, you need
						// to decide what to do in this case and add code
						// to do that here. You can use GetLastError() to
						// determine the cause of this failure.
						message_box_up = TRUE;
						MessageBox(NULL,"Error occurred trying to run ToonTalk. Sorry.\rPerhaps you need to restart your computer.\rIf the problem persists contact support@toontalk.com.",S(IDS_START_TOONTALK),
									  MB_ICONSTOP|MB_OK);
						message_box_up = FALSE;
					} else if (first_time) { // will put up another bitmap and buttons...
						PostMessage (hWnd, WM_QUIT, 0, 0);
					}
					break;
				case IDM_SEE_DEMOS:
               if (autodemo != NO_AUTODEMO) {
					   SetTimer(hWnd, 7023, max_idle_time, NULL); // if idle for long time start running demos
					   toontalk_running_when_timer_set = FALSE; // need to check?
               };
					first_time = FALSE;
					if (PrepareWindow (hWnd) == -1) {
						message_box_up = TRUE;
						MessageBox(NULL,"Something is wrong. Sorry.\rMaybe you need to restart your computer or re-install ToonTalk.",
									  S(IDS_START_TOONTALK),
									  MB_ICONSTOP|MB_OK);
						DestroyWindow (hWnd);
					};
					hAccelTable = LoadAccelerators (hInst, "WHICHDEMO");
					break;
				case IDS_INTRO_DEMO:
				case IDS_EXPLODE_DEMO:
				case IDS_SWAP_DEMO:
				case IDS_PONG1_DEMO:
				case IDS_PONG2_DEMO:
				case IDS_PONG3_DEMO:
				case IDS_FIBONACCI_DEMO:
				case IDS_FACTORIAL_DEMO:
				case IDS_BANK_DEMO:
				case IDS_APPEND_DEMO:
					if (!LaunchApplication(wmId)) {
						// Failed to launch your application, you need
						// to decide what to do in this case and add code
						// to do that here. You can use GetLastError() to
						// determine the cause of this failure.
						message_box_up = TRUE;
						MessageBox(NULL,"Error occurred trying to run this demo. Sorry.\rTry restarting your computer or re-installing ToonTalk.",
									  S(IDS_START_TOONTALK),
									  MB_ICONSTOP|MB_OK);
						message_box_up = FALSE;
//					} else { // will put up another bitmap and buttons...
//						PostMessage (hWnd, WM_CLOSE, 0, 0);
					};
					break;
/*
				case IDS_EXPLODE_INSTRUCTIONS:
					if (!OpenDocument()) {
						message_box_up = TRUE;
						MessageBox(NULL,"Sorry.  Error occurred starting the viewer for the instructions.  See the \DOC\readme.txt file on the CDROM for more details.","Start ToonTalk",
									  MB_ICONSTOP|MB_OK);
						message_box_up = FALSE;
					} else {
						SetTimer(hWnd, 7023, 2000, NULL); // check again in a couple of seconds
					};
					break;
*/
				case IDS_FORGET_IT:
					DestroyWindow (hWnd);
					break;
/*
				case IDM_HELPCONTENTS:
					WinHelp (hWnd, szHelpFileName, HELP_CONTENTS,(DWORD)0);
					break;
*/
				default:
					return (DefWindowProc(hWnd, message, wParam, lParam));
			}
			break;
		case WM_PAINT:
         if (use_html_dialog_file) {
            // if showing HTML dialogs then could show background pattern - only see this because it takes time to paint a new HTML dialog - 
				// new on 210602
            string message = S(IDS_PLEASE_WAIT,"Please wait..."); // new on 210602
            hdc = BeginPaint (hWnd, &ps);
            TextOut(hdc,0,0,message,strlen(message));
            EndPaint (hWnd, &ps);
         } else if (MCI_window == NULL) { // not showing an AVI movie 
				hdc = BeginPaint (hWnd, &ps);
				if (fPalette || fTrueColor) {
					SelectPalette(hdc, hpal, FALSE);
					RealizePalette(hdc);
				};
				GetClientRect (hWnd, &rect);
				hdcSrc = CreateCompatibleDC (hdc);
				hbmOld = (HBITMAP) SelectObject (hdcSrc, hbmAutoRun);
				if (fPalette || fTrueColor) {
					SelectPalette(hdcSrc, hpal, FALSE);
					RealizePalette(hdcSrc);
				};
				if (!BitBlt (hdc, 0, 0, bmAutoRun.bmWidth, bmAutoRun.bmHeight, hdcSrc, 0, 0, SRCCOPY)) {
					MessageBeep(0);
				}
				SelectObject (hdcSrc, hbmOld);
				DeleteDC (hdcSrc);
				EndPaint (hWnd, &ps);
			} else {
				hdc = BeginPaint (hWnd, &ps);
				EndPaint (hWnd, &ps);
			};
			break;        
//		case WM_SETFOCUS:
//			break;
		case WM_TIMER: {
			if (IsIconic(hWnd)) break;
//			if (first_time && autodemo != REPEAT_INTRO) break; // haven't gotten to demo choice screen 
			HWND toontalk_window = FindWindow("ToonTalk",NULL); // was toontalk_exe_name
			if (toontalk_window == NULL && wParam == 7023 &&
				 !toontalk_running_when_timer_set &&
				 !message_box_up) { // too easy to be non-active in a kiosk context
//             GetActiveWindow() == hWnd) {  // and am active
				// my timer triggered this and no ToonTalk running (now or when timer set)
				if (!LaunchApplication(next_demo)) {
					message_box_up = TRUE;
					MessageBox(NULL,
								  "Sorry. Something went wrong trying to launch a demo.\rRestarting your computer or re-installing ToonTalk might help.",
								  S(IDS_START_TOONTALK),MB_ICONSTOP|MB_OK);
					PostMessage(hWnd, WM_QUIT, 0, 0);
					break;
				};
            if (autodemo == REPEAT_INTRO) {
               next_demo = IDS_INTRO_DEMO;
            } else {
				   next_demo++;
				   if (next_demo > IDS_APPEND_DEMO) next_demo = IDS_INTRO_DEMO;
            };
			};
			SetTimer(hWnd, 7023, max_idle_time, NULL); // check again in a while
			toontalk_running_when_timer_set = (toontalk_window != NULL);
		  };
			break;
		case WM_DESTROY:
			// Tell WinHelp we don't need it any more...
//			WinHelp (hWnd, APPNAME".HLP", HELP_QUIT,(DWORD)0);
			if (hpal != NULL) {
            DeleteObject (hpal);
            hpal = NULL;
         };
			DeleteObject (hbmAutoRun);
			if (global_handle > 0) {
				GlobalUnlock(global_handle);
				GlobalFree(global_handle);
            global_handle = 0;
			};
			PostQuitMessage(0);
			break;
		case WM_QUERYOPEN: // new on 010205
			paused = FALSE;
			return(TRUE);
		case MCIWNDM_NOTIFYMODE:
			if (lParam == MCI_MODE_STOP) {
				if (MCI_window != NULL) {
					MCIWndClose(MCI_window);
					MCI_window = NULL;
				};
//			} else { // if (lParam == MCI_MODE_PLAY || lParam == MCI_MODE_OPEN) {
//				SetTimer(hWnd, 1, 5000, NULL); // 5 seconds since sometimes notification doesn't come thru
			};
			break;
		case WM_SYSCOMMAND:
			switch (wParam) {
				case SC_SCREENSAVE:
				  // don't give a screen saver permission to run if autodemo is on
				  if (autodemo != NO_AUTODEMO) return(0);
			}; // else fall thru to the following
		default:
			return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return (0);
}

BOOL CALLBACK about_dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM ) {
	switch (message) {
		case WM_INITDIALOG:
			return(TRUE);
		case WM_COMMAND:
			switch(wParam) {
				case IDM_EXIT:
					EndDialog(hDlg,0);
					break;
         };
         break;
	   case WM_SYSCOMMAND:
		   switch (wParam) {
				case SC_CLOSE:
					EndDialog(hDlg,0);
					break;
		   };
		   break;
	 };
	 return(FALSE);
};


/************************************************************************\
 *    FUNCTION: CenterWindow
\************************************************************************/
// This is a 'utility' function I find usefull. It will center one
// window over another. It also makes sure that the placement is within
// the 'working area', meaning that it is both within the display limits
// of the screen, -and- not obscured by the tray or other frameing
// elements of the desktop.
BOOL CenterWindow (HWND hwndChild, HWND hwndParent)
{
	RECT    rChild, rParent, rWorkArea = {0,0,0,0};
	int     wChild, hChild, wParent, hParent;
	int     wScreen, hScreen, xScreen, xNew, yNew, yScreen;
//	BOOL bResult;

	// Get the Height and Width of the child window
	GetWindowRect (hwndChild, &rChild);
	wChild = rChild.right - rChild.left;
	hChild = rChild.bottom - rChild.top;

	// Get the Height and Width of the parent window
	GetWindowRect (hwndParent, &rParent);
	wParent = rParent.right - rParent.left;
	hParent = rParent.bottom - rParent.top;

	// Get the limits of the 'workarea'
#if !defined(SPI_GETWORKAREA)
#define SPI_GETWORKAREA 48
#endif
//	bResult = 
   SystemParametersInfo(
		SPI_GETWORKAREA,	// system parameter to query or set
		sizeof(RECT),	// depends on action to be taken
		&rWorkArea,	// depends on action to be taken
		0);	

	wScreen = rWorkArea.right - rWorkArea.left;
	hScreen = rWorkArea.bottom - rWorkArea.top;
	xScreen = rWorkArea.left;
	yScreen = rWorkArea.top;

	// On Windows NT, the above metrics aren't valid (yet), so they all return
	// '0'. Lets deal with that situation properly:
	if (wScreen==0 && hScreen==0) {
		wScreen = GetSystemMetrics(SM_CXSCREEN);
		hScreen = GetSystemMetrics(SM_CYSCREEN);
		xScreen = 0; // These values should already be '0', but just in case
		yScreen = 0;
	}

	// Calculate new X position, then adjust for screen
	xNew = rParent.left + ((wParent - wChild) /2);
	if (xNew < xScreen) {
		xNew = xScreen;
	} else if ((xNew+wChild) > wScreen) {
		xNew = (xScreen + wScreen) - wChild;
	}

	// Calculate new Y position, then adjust for screen
	yNew = rParent.top  + ((hParent - hChild) /2);
	if (yNew < yScreen) {
		yNew = yScreen;
	} else if ((yNew+hChild) > hScreen) {
		yNew = (yScreen + hScreen) - hChild;
	}

	// Set it, and return
	return SetWindowPos (hwndChild, NULL, xNew, yNew, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}


BOOL CenterWindowHorizontally (HWND hwndChild, HWND hwndParent)
{
	RECT    rChild, rParent, rWorkArea = {0,0,0,0};
	int     wChild, wParent; //, hParent,hChild ;
	int     wScreen, hScreen, xScreen, xNew; // yNew, yScreen;
//	BOOL bResult;

	// Get the Height and Width of the child window
	GetWindowRect (hwndChild, &rChild);
	wChild = rChild.right - rChild.left;
//	hChild = rChild.bottom - rChild.top;

	// Get the Height and Width of the parent window
	GetWindowRect (hwndParent, &rParent);
	wParent = rParent.right - rParent.left;
//	hParent = rParent.bottom - rParent.top;

	// Get the limits of the 'workarea'
#if !defined(SPI_GETWORKAREA)
#define SPI_GETWORKAREA 48
#endif
//	bResult = 
   SystemParametersInfo(
		SPI_GETWORKAREA,	// system parameter to query or set
		sizeof(RECT),	// depends on action to be taken
		&rWorkArea,	// depends on action to be taken
		0);	

	wScreen = rWorkArea.right - rWorkArea.left;
	hScreen = rWorkArea.bottom - rWorkArea.top;
	xScreen = rWorkArea.left;
//	yScreen = rWorkArea.top;

	// On Windows NT, the above metrics aren't valid (yet), so they all return
	// '0'. Lets deal with that situation properly:
	if (wScreen==0 && hScreen==0) {
		wScreen = GetSystemMetrics(SM_CXSCREEN);
//		hScreen = GetSystemMetrics(SM_CYSCREEN);
		xScreen = 0; // These values should already be '0', but just in case
//		yScreen = 0;
	}

	// Calculate new X position, then adjust for screen
	xNew = rParent.left + ((wParent - wChild) /2);
	if (xNew < xScreen) {
		xNew = xScreen;
	} else if ((xNew+wChild) > wScreen) {
		xNew = (xScreen + wScreen) - wChild;
	}
/*
	// Calculate new Y position, then adjust for screen
	yNew = rParent.top  + ((hParent - hChild) /2);
	if (yNew < yScreen) {
		yNew = yScreen;
	} else if ((yNew+hChild) > hScreen) {
		yNew = (yScreen + hScreen) - hChild;
	}
*/
	// Set it, and return
	return SetWindowPos (hwndChild, NULL, xNew, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

/* /************************************************************************\
 *    FUNCTION: About
\************************************************************************/
// To allow us better flexibility over the contents of the 'About' box,
// lets go the extra mile and pull out values from the 'Version' resource.
// This means that by properly keeping that information up-to-date, it
// will automatically keep your about box information current. Remember,
// that the 'version' strings can also be seen from the 'Properties'
// page of FileManager on Windows NT, or the Explorer on Windows 95.
/*
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static  HFONT hfontDlg;		// Font for dialog text
	static	HFONT hFinePrint;	// Font for 'fine print' in dialog
	DWORD   dwVerInfoSize;		// Size of version information block
	LPSTR   lpVersion;			// String pointer to 'version' text
	DWORD   dwVerHnd=0;			// An 'ignored' parameter, always '0'
	UINT    uVersionLen;
	WORD    wRootLen;
	BOOL    bRetCode;
	int     i;
	char    szFullPath[256];
	char    szResult[256];
	char    szGetName[256];
	DWORD	dwVersion;
	char	szVersion[40];
	DWORD	dwResult;

	switch (message) {
		case WM_INITDIALOG:
			ShowWindow (hDlg, SW_HIDE);
			hfontDlg = GetStockObject (ANSI_VAR_FONT);
			hFinePrint = CreateFont(11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				VARIABLE_PITCH | FF_SWISS, "");
			CenterWindow (hDlg, GetWindow (hDlg, GW_OWNER));
			GetModuleFileName (hInst, szFullPath, sizeof(szFullPath));

			// Now lets dive in and pull out the version information:
			dwVerInfoSize = GetFileVersionInfoSize(szFullPath, &dwVerHnd);
			if (dwVerInfoSize) {
				LPSTR   lpstrVffInfo = (LPVOID)LocalAlloc (LPTR, dwVerInfoSize);
				GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpstrVffInfo);
				// The below 'hex' value looks a little confusing, but
				// essentially what it is, is the hexidecimal representation
				// of a couple different values that represent the language
				// and character set that we are wanting string values for.
				// 040904E4 is a very common one, because it means:
				//   US English, Windows MultiLingual characterset
				// Or to pull it all apart:
				// 04------        = SUBLANG_ENGLISH_USA
				// --09----        = LANG_ENGLISH
				// ----04E4 = 1252 = Codepage for Windows:Multilingual
				lstrcpy(szGetName, "\\StringFileInfo\\040904E4\\");	 
				wRootLen = lstrlen(szGetName); // Save this position
			
				// Set the title of the dialog:
				lstrcat (szGetName, "ProductName");
				bRetCode = VerQueryValue((LPVOID)lpstrVffInfo,
					(LPSTR)szGetName,
					(LPVOID)&lpVersion,
					(UINT *)&uVersionLen);
				lstrcpy(szResult, "About ");
				lstrcat(szResult, lpVersion);
				SetWindowText (hDlg, szResult);

				// Walk through the dialog items that we want to replace:
				for (i = DLG_VERFIRST; i <= DLG_VERLAST; i++) {
					GetDlgItemText(hDlg, i, szResult, sizeof(szResult));
					szGetName[wRootLen] = (char)0;
					lstrcat (szGetName, szResult);
					uVersionLen   = 0;
					lpVersion     = NULL;
					bRetCode      =  VerQueryValue((LPVOID)lpstrVffInfo,
						(LPSTR)szGetName,
						(LPVOID)&lpVersion,
						(UINT *)&uVersionLen);

					if ( bRetCode && uVersionLen && lpVersion) {
					// Replace dialog item text with version info
						lstrcpy(szResult, lpVersion);
						SetDlgItemText(hDlg, i, szResult);
					} else {
						dwResult = GetLastError();
						wsprintf (szResult, "Error %lu", dwResult);
						SetDlgItemText (hDlg, i, szResult);
					}
					SendMessage (GetDlgItem (hDlg, i), WM_SETFONT, 
						(UINT)((i==DLG_VERLAST)?hFinePrint:hfontDlg),
						TRUE);
				} // for (i = DLG_VERFIRST; i <= DLG_VERLAST; i++)
				LocalFree ((HLOCAL)lpstrVffInfo);

			} else {
				// No version information available.
			} // if (dwVerInfoSize)

			SendMessage (GetDlgItem (hDlg, IDC_LABEL), WM_SETFONT, (WPARAM)hfontDlg,(LPARAM)TRUE);

			// We are  using GetVersion rather then GetVersionEx
			// because earlier versions of Windows NT and Win32s
			// didn't include GetVersionEx:
			dwVersion = GetVersion();

			if (dwVersion < 0x80000000) {
				// Windows NT
				wsprintf (szVersion, "Microsoft Windows NT %u.%u (Build: %u)",
					(DWORD)(LOBYTE(LOWORD(dwVersion))),
					(DWORD)(HIBYTE(LOWORD(dwVersion))),
					(DWORD)(HIWORD(dwVersion)) );
			} else if (LOBYTE(LOWORD(dwVersion))<4) {
				// Win32s
				wsprintf (szVersion, "Microsoft Win32s %u.%u (Build: %u)",
					(DWORD)(LOBYTE(LOWORD(dwVersion))),
					(DWORD)(HIBYTE(LOWORD(dwVersion))),
					(DWORD)(HIWORD(dwVersion) & ~0x8000) );
			} else {
				// Windows 95
				wsprintf (szVersion, "Microsoft Windows 95 (osv:%u.%u)",
					(DWORD)(LOBYTE(LOWORD(dwVersion))),
					(DWORD)(HIBYTE(LOWORD(dwVersion))) );
			}

			SetWindowText (GetDlgItem(hDlg, IDC_OSVERSION), szVersion);
			ShowWindow (hDlg, SW_SHOW);
			return (TRUE);

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
				EndDialog(hDlg, TRUE);
				return (TRUE);
			}
			break;

		case WM_DESTROY:
			DeleteObject (hFinePrint);
			break;			
	}

	return FALSE;
}

*/

/************************************************************************\
 *    FUNCTION: PrepareWindow
\************************************************************************/
// This function will be called when the WM_CREATE message is processed
// by our application. This is where we will load the bitmap to be
// displayed, create a palette for the image, resize the window to fit
// the bitmap, and put in our control buttons that will allow the user
// to tell us how to proceed.

// if not first_time then re-compute window with new background and buttons


HFONT hfontBtn; // moved here so font is used elsewhere too

int PrepareWindow(HWND hWnd) {
   if (use_html_dialog_file) {
      HDC device_context = GetDC(NULL);
#if TT_DEBUG_STARTTT
      // easier to debug with the following
      CloseWindow(hWnd);
#else
      SetWindowPos(hWnd,HWND_TOP,0,0,GetDeviceCaps(device_context,HORZRES),GetDeviceCaps(device_context,VERTRES),0);
		// prior to 240105 HWND_TOP was HWND_TOPMOST
#endif
      return(TRUE); // new on 210602
   };
	HDC hdc;
	RECT rect;
	int i;
	LPBITMAPINFO lpbmi;
	LPVOID lpv;
	HRSRC hrsrc;
	DWORD dwSize;
	LOGPALETTE *plgpl;
	int cColors;
	LPVOID pimage;
	TEXTMETRIC tm;
	int iHeight, iWidth, x, y, x_right;
	char szString[40];

	hdc = GetDC(hWnd);

	hpal = NULL;

	fPalette = GetDeviceCaps (hdc, RASTERCAPS) & RC_PALETTE;
	fTrueColor = ((GetDeviceCaps(hdc, BITSPIXEL) * GetDeviceCaps(hdc, PLANES))>=16); //was 32
//	int temp = GetDeviceCaps(hdc, PLANES);
//	int temp2 = GetDeviceCaps(hdc, NUMCOLORS	);
//	int temp3 = GetDeviceCaps(hdc, BITSPIXEL	);

	// At this point, 'fPalette' will be true if we are running
	// on a display that supports (and -needs- to support) the
	// palette manager. 'fTrueColor' will be true if the display
	// we are running on can handle any color we can throw at
	// it, thus doesn't need to use the palette manager. If
	// either of these are true, then we can display our full
	// color image, otherwise, we need to shift down to our
	// 16 color version.

	if (fPalette || fTrueColor) {
//	if (!(GetDeviceCaps(hdc,RASTERCAPS) & RC_PALETTE) ||
//		 GetDeviceCaps(hdc,SIZEPALETTE) == 256) {
		// Locate the resource:
		if (first_time) {
			hrsrc = FindResource(resource_source(), "DEMOFREE", RT_BITMAP);
		} else {
			hrsrc = FindResource(resource_source(), "PICKONE", RT_BITMAP);
		};
		// Lock it down. This will give us a memory pointer:
		lpv = LockResource(LoadResource(resource_source(),hrsrc));
		// Find out how big the resource was:
		dwSize = SizeofResource(resource_source(),hrsrc);
		if (global_handle > 0) {
			GlobalUnlock(global_handle);
			GlobalFree(global_handle);
		};
		// Allocate enough memory:
		global_handle = GlobalAlloc (GHND , dwSize);
		lpbmi = (LPBITMAPINFO) GlobalLock(global_handle); // was LocalAlloc in Win32
		if (!lpbmi) return -1; // failure
		// And make our own private copy of the data:
#if TT_32
		memcpy ((void *) lpbmi, (void *) lpv, dwSize);
#else
		hmemcpy ((void *) lpbmi, (void *) lpv, dwSize);
#endif
		// How many colors does it use?:
		cColors = (int) lpbmi->bmiHeader.biClrUsed;
		if (cColors == 0) {
			cColors = (1 << (int)(lpbmi)->bmiHeader.biBitCount);
		}

		// Use that to determine where the actual bitmap image starts:
		pimage = &(lpbmi->bmiColors[cColors]);

		// Now lets create a palette based on the image we loaded:
//		plgpl = (LOGPALETTE *)LocalAlloc (LPTR, sizeof(LOGPALETTE) + (cColors-1)*sizeof(PALETTEENTRY));
		int size = sizeof(LOGPALETTE) + (cColors-1)*sizeof(PALETTEENTRY);
		unsigned char *palette_memory = new unsigned char[size];
		memset(palette_memory,0,size);
		plgpl = (LOGPALETTE *) palette_memory;
		if (!plgpl) return -1; // failure

		plgpl->palVersion = 0x300;
		plgpl->palNumEntries = (WORD) cColors;
		for (i=0; i<cColors; i++) {
			plgpl->palPalEntry[i].peRed   = lpbmi->bmiColors[i].rgbRed;
			plgpl->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
			plgpl->palPalEntry[i].peBlue  = lpbmi->bmiColors[i].rgbBlue;
			//plgpl->palPalEntry[i].peFlags = PC_NOCOLLAPSE; // is this needed?
		}
		hpal = CreatePalette(plgpl);

		// And free up the memory we allocated:
//		LocalFree ((HLOCAL)plgpl);

		if (!hpal) return -1; // Fail on no palette

		// Now create a true DIBitmap from all of this:
		// First assign the palette to the DC:
		SelectPalette(hdc, hpal, FALSE);
		RealizePalette(hdc);

		// Now create a DIB based off the the DC with the bitmap image from the resource:
		hbmAutoRun = CreateDIBitmap (hdc, (BITMAPINFOHEADER *)lpbmi,
			CBM_INIT, pimage, lpbmi, DIB_RGB_COLORS);

//		LocalFree ((HLOCAL) lpbmi);
		delete [] palette_memory;

	} else {
		// At this point, you know that the current display
		// device does not support palettes. So you may want
		// to keep track of this and later warn the user that
		// they may not get the full visual impact of your
		// application...
		hbmAutoRun = NULL; // just to make sure we force load the 16 color version
	}

	if (hbmAutoRun == NULL) {  // whimpy 16 color version...
//		if (first_time) {
//			hbmAutoRun = LoadBitmap (resource_source(), "DEMOFREE16");
//		} else {
//			hbmAutoRun = LoadBitmap (resource_source(), "PICKONE16");
//		};
		MessageBox(NULL,"Windows is reporting that only 16 colors are available. Change your display's color properties to proceed",
			        S(IDS_START_TOONTALK),MB_ICONINFORMATION|MB_OK);
		if (!hbmAutoRun) {
			return -1;
		}
	}

	// And get the header data:
	GetObject (hbmAutoRun, sizeof(BITMAP), &bmAutoRun);

	SetWindowPos (hWnd, NULL, 0, 0, bmAutoRun.bmWidth, bmAutoRun.bmHeight,
		SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	// Since what we really want to do is size the 'Client' window,
	// Lets find out how far off we were, adjust our values, and
	// try it again...
	GetClientRect (hWnd, &rect);
	x = bmAutoRun.bmWidth - (rect.right-rect.left);
	y = bmAutoRun.bmHeight - (rect.bottom-rect.top);
	SetWindowPos (hWnd, NULL, 0, 0, bmAutoRun.bmWidth+x,
		bmAutoRun.bmHeight+y, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

	CenterWindowHorizontally (hWnd, GetDesktopWindow ());

	// Ok, our image is loaded, and our window properly sized
	// Lets add in some controls: The text to use for these
	// buttons is coming out of the resource of this app
	if (tt_language == JAPANESE) {
		hfontBtn = CreateFont(12, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, 0, 0, 0,
									 VARIABLE_PITCH | FF_SWISS, "ms gothic");
	} else {
		hfontBtn = (HFONT) GetStockObject(ANSI_VAR_FONT); 
	};
	SelectObject (hdc, hfontBtn);
	GetTextMetrics (hdc, &tm);
   // set this so it fits even if user has set large font for Windows
	iHeight = 26; //(tm.tmHeight + tm.tmExternalLeading) * 2;
//	iWidth = (bmAutoRun.bmWidth / 2) - (iHeight * 2);
   iWidth = (bmAutoRun.bmWidth / 3) - iHeight/2;
//	x = ((bmAutoRun.bmWidth/2) / 2) - (iWidth/2);
   x += iHeight/8; // was /4
//	y = bmAutoRun.bmHeight - iHeight - (iHeight/2);
	y = bmAutoRun.bmHeight - (9*iHeight)/4;   // was 5

	if (first_time) {
		LoadString (resource_source(), IDS_FREE_PLAY, szString, sizeof(szString));
		hwndBtnFreePlay = CreateWindow ("BUTTON", szString,
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
			0, 0, 0, 0,
			hWnd, (HMENU)IDS_FREE_PLAY, resource_source(), NULL);
		LoadString (resource_source(), IDM_SEE_DEMOS, szString, sizeof(szString));
		hwndBtnSeeDemos = CreateWindow ("BUTTON", szString,
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0,
			hWnd, (HMENU) IDM_SEE_DEMOS, resource_source(), NULL);
      LoadString (resource_source(), IDM_RUN_PUZZLES, szString, sizeof(szString));
		hwndBtnPuzzles = CreateWindow ("BUTTON", szString,
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0,
			hWnd, (HMENU)IDM_RUN_PUZZLES, resource_source(), NULL);

      LoadString (resource_source(), IDM_SET_OPTIONS, szString, sizeof(szString));
		hwndBtnOptions = CreateWindow ("BUTTON", szString,
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0,
			hWnd, (HMENU)IDM_SET_OPTIONS, resource_source(), NULL);

      LoadString (resource_source(), IDM_ABOUT, szString, sizeof(szString));
		hwndBtnAbout = CreateWindow ("BUTTON", szString,
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0,
			hWnd, (HMENU)IDM_ABOUT, resource_source(), NULL);

      LoadString (resource_source(), IDM_HELP, szString, sizeof(szString));
		hwndBtnHelp = CreateWindow ("BUTTON", szString,
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0,
			hWnd, (HMENU)IDM_HELP, resource_source(), NULL);

		SendMessage (hwndBtnFreePlay, WM_SETFONT, (WPARAM)hfontBtn,(LPARAM)TRUE);
		SendMessage (hwndBtnSeeDemos, WM_SETFONT, (WPARAM)hfontBtn, (LPARAM)TRUE);
		SendMessage (hwndBtnPuzzles, WM_SETFONT, (WPARAM)hfontBtn, (LPARAM)TRUE);
      SendMessage (hwndBtnOptions, WM_SETFONT, (WPARAM)hfontBtn, (LPARAM)TRUE);
      SendMessage (hwndBtnAbout, WM_SETFONT, (WPARAM)hfontBtn, (LPARAM)TRUE);
      SendMessage (hwndBtnHelp, WM_SETFONT, (WPARAM)hfontBtn, (LPARAM)TRUE);

		SetWindowPos (hwndBtnPuzzles, NULL, x, y, iWidth, iHeight,
			           SWP_NOZORDER | SWP_NOACTIVATE);
      SetWindowPos (hwndBtnOptions, NULL, x, y+(9*iHeight)/8, iWidth, iHeight,
			           SWP_NOZORDER | SWP_NOACTIVATE);
		x += bmAutoRun.bmWidth/3;
		SetWindowPos (hwndBtnSeeDemos, NULL, x, y, iWidth, iHeight,
			           SWP_NOZORDER | SWP_NOACTIVATE);
      SetWindowPos (hwndBtnAbout, NULL, x, y+(9*iHeight)/8, iWidth, iHeight,
			           SWP_NOZORDER | SWP_NOACTIVATE);
      x += bmAutoRun.bmWidth/3;
		SetWindowPos (hwndBtnFreePlay, NULL, x, y, iWidth, iHeight,
			           SWP_NOZORDER | SWP_NOACTIVATE);
      SetWindowPos (hwndBtnHelp, NULL, x, y+(9*iHeight)/8, iWidth, iHeight,
			           SWP_NOZORDER | SWP_NOACTIVATE);
      if (autodemo != NO_AUTODEMO) {
		   SetTimer(hWnd, 7023, max_idle_time, NULL); // if idle for long time start running demos
		   toontalk_running_when_timer_set = FALSE; // need to check?
      };
	} else { // put up all those demo buttons
		int i, middle;
		HWND hwndButton;
		ShowWindow(hwndBtnSeeDemos,SW_HIDE);
		ShowWindow(hwndBtnFreePlay,SW_HIDE);
      ShowWindow(hwndBtnPuzzles,SW_HIDE);
      ShowWindow(hwndBtnOptions,SW_HIDE);
      ShowWindow(hwndBtnHelp,SW_HIDE);
      ShowWindow(hwndBtnAbout,SW_HIDE);
		iWidth = (bmAutoRun.bmWidth / 2);
		x = 0;
//		y = bmAutoRun.bmHeight-iHeight;
		x_right = x+bmAutoRun.bmWidth/2;
//		LoadString (resource_source(), IDS_FORGET_IT, szString, sizeof(szString));
//		hwndButton = CreateWindow ("BUTTON", szString,
//				WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | BS_PUSHBUTTON,
//				bmAutoRun.bmWidth/4 , y, iWidth, iHeight,
//				hWnd, (HMENU) IDS_FORGET_IT, resource_source(), NULL);
//		SendMessage (hwndButton, WM_SETFONT, (WPARAM)hfontBtn,(LPARAM)TRUE);
		y = bmAutoRun.bmHeight-iHeight; //-2*iHeight;
		middle = (IDS_INTRO_DEMO+IDS_LAST_BUTTON)/2+1;
		for (i = IDS_LAST_BUTTON; i >= IDS_INTRO_DEMO; i--) { 
			BOOL left_side = (i < middle);
			LoadString (resource_source(), i, szString, sizeof(szString));
			hwndButton = CreateWindow ("BUTTON", szString,
				WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | ((i==IDS_INTRO_DEMO)?BS_DEFPUSHBUTTON:BS_PUSHBUTTON),
				left_side?x:x_right , y, iWidth, iHeight,
				hWnd, (HMENU) i, resource_source(), NULL);
			SendMessage (hwndButton, WM_SETFONT, (WPARAM)hfontBtn,(LPARAM)TRUE);
			y -= iHeight;
			if (i == middle) y = bmAutoRun.bmHeight-iHeight;
		};
	};

   if (!set_options_now) {
      ShowWindow (hWnd, SW_SHOW);
   };

	ReleaseDC (hWnd, hdc);

	return 0; // Success
	
}
//BOOL audio_warning_given = FALSE; // commented out on 120304
//
//void warn_audio_important() {
//	if (audio_warning_given) return;
//	if (GetPrivateProfileInt("Switches","NoAudioWarningGiven",0,tt_ini_file) == 0) {
//		// important to write first since might ignore message box and cause
//		// another to be spawned
//		WritePrivateProfileString("Switches","NoAudioWarningGiven","1",tt_ini_file);
//		audio_warning_given = TRUE;
//		message_box_up = TRUE;
//      char message[256];
//      LoadString(resource_source(),IDS_SPEAKERS_ON,message,sizeof(message));
//		MessageBox(NULL,message,S(IDS_START_TOONTALK),MB_ICONINFORMATION|MB_OK);
//		message_box_up = FALSE;
//	};
//};

//BOOL slow_warning_given = FALSE; 
// commented out on 120304 since this shows up as a message box under the HTML dialog and CD-ROM drives are fast now
//
//void warn_slow_cdrom_demos() {
//	if (slow_warning_given) return;
//	if (GetPrivateProfileInt("Switches","NoCDROMSlowWarningGiven",0,tt_ini_file) == 0) {
//		WritePrivateProfileString("Switches","NoCDROMSlowWarningGiven","1",tt_ini_file);
//		slow_warning_given = TRUE;
//		message_box_up = TRUE;
//      MessageBox(NULL,S(IDS_CDROM_MAY_BE_SLOW),
//					       S(IDS_START_TOONTALK),MB_ICONINFORMATION|MB_OK);
//		message_box_up = FALSE;
//	};
//};

DWORD wait_before_launch = 0;

/************************************************************************\
 *    FUNCTION: 
\************************************************************************/
// This function is called to launch the 'real' application when the
// user instructs us to. The name of the application to be run is coming
// out of the resource of this app.
BOOL LaunchApplication(int demo_id) {
//	char szCompany[40];
//	char szImageName[260];
//	char szCmdLine[260];
   BOOL result;
//	HWND existing_toontalk = FindWindow("ToonTalk","ToonTalk"); // asking name
//	if (existing_toontalk != NULL) {
//		MessageBox(NULL,"ToonTalk already running.","Start ToonTalk",
//					  MB_ICONINFORMATION|MB_OK);
//		if (IsIconic(existing_toontalk)) {
//			ShowWindow(existing_toontalk, SW_RESTORE);
//		} else {
//			SetActiveWindow(existing_toontalk);
//		};
//		return(TRUE);
//	};
	if (timeGetTime() < wait_before_launch) {
		return(TRUE); // too soon after a successful launch
	};
//	existing_toontalk = FindWindow("ToonTalk",NULL);
//	if (existing_toontalk != NULL &&
//		 MessageBox(NULL,"ToonTalk already running.  Start a new one?","Start ToonTalk",
//						MB_ICONINFORMATION|MB_YESNO) == IDNO) {
//		SetActiveWindow(existing_toontalk); 
//		return(TRUE);
//	};
//#if !RUNNING_TT16
//	STARTUPINFO         si;
//	PROCESS_INFORMATION pi;
//#endif
//	LoadString (hInst, ID_COMPANY, szCompany, sizeof(szCompany));
//	LoadString (hInst, ID_APPNAME, szImageName, sizeof(szImageName));
//	strcpy(szImageName,toontalk_directory);
   //char *toontalk_key; // commented out on 030304
   //char *demo_key;
//   if (lpCommandLine != NULL && strstr(lpCommandLine,"-set_options 1") != NULL) {
//      option_dialogs(hwnd);
//   };
//   if (current_command_line != NULL) {
//      if (strstr(current_command_line,"-v 22") != NULL) {
//         runningHighResolution = FALSE; // regardless of defaults
//      } else if (strstr(current_command_line,"-v 25") != NULL) {
//         runningHighResolution = TRUE;
//      };
//   };
/*
   if (tt_language == NO_LANGUAGE_SPECIFIED) {
//      char *default_language = ini_entry("Versions","Language");
      int language_answer = default_setting("Language");
      string language_key;
      if (language_answer > 0) {
         language_key = "Language1";
         itoa(language_answer,language_key+8,10);
      } else {
         language_key = "Language";
      };
      string default_language = ini_entry("Versions",language_key);
      if (default_language != NULL) {
         tt_language = string_to_language(default_language);
//         delete [] default_language;
         language_arg = default_language;
      };
      if (tt_language == NO_LANGUAGE_SPECIFIED) {
         tt_language = UNKNOWN_LANGUAGE;
      };
   };
*/
 //  BOOL running16bit = FALSE; // commented out on 030304
	//if (current_command_line != NULL && strstr(current_command_line,"-32 0") != NULL) {
 //     running16bit = TRUE;
 //  };
   int version_answer = default_setting("Version");
//   runningHighResolution = (version_answer > 1);
   WritePrivateProfileString("User","PreviousStartToonTalkCommandLine",current_command_line,tt_ini_file);
	//if (running16bit) { // commented out on 030304
	//	toontalk_key = "ToonTalk16";
	//	demo_key = "DemoToonTalk16";
	//} else {
	//	toontalk_key = "ToonTalk32";
	//	demo_key = "DemoToonTalk32";
	//};
	string exe_specified_in_command_line = command_line_arg(current_command_line,"-exe ");
	if (exe_specified_in_command_line != NULL) {
		// new on 030304
		toontalk_exe_name = new char[MAX_PATH];
		copy_string_up_to_space(toontalk_exe_name,exe_specified_in_command_line);
	} else {
//		GetPrivateProfileString("Executables",toontalk_key,"",toontalk_exe_name,64,tt_ini_file);
		toontalk_exe_name = ini_entry("Executables","ToonTalk32"); // re-written on 070201
	};
	string demo_exe_specified_in_command_line = command_line_arg(current_command_line,"-demo_exe ");
	if (demo_exe_specified_in_command_line != NULL) {
		demo_exe_name = new char[MAX_PATH];
		copy_string_up_to_space(demo_exe_name,demo_exe_specified_in_command_line);
	} else {
//    GetPrivateProfileString("Executables",demo_key,"",demo_exe_name,64,tt_ini_file);
		demo_exe_name = ini_entry("Executables","DemoToonTalk32");
		if (demo_exe_name == NULL) { // new on 110304 for robustness
			demo_exe_name = copy_string(toontalk_exe_name);
		};
	};
   char demo_directory[MAX_PATH];
   strcpy(demo_directory,toontalk_directory);
   string program_name;
   BOOL image_on_cdrom;
   string image_name;
	if (demo_id <= 0 && !DMO_file && demo_id != -3) { // changed on 291199 so DMO files (typically double clicked on) work OK
		program_name = toontalk_exe_name;
      image_name = existing_file_name(image_on_cdrom,program_name,"EXE");
	} else {
		program_name = demo_exe_name;
      image_name = existing_file_name(image_on_cdrom,program_name,"EXE");
	};
   if (image_name == NULL) return(FALSE);
	if (DMO_file || PZL_file || CTY_file | TT_file) { // just run it now
      char new_command_line[512];
      if (DMO_file) { // new on 090402 so you can double click and use an expired beta exe to run old demos
         strcpy(new_command_line,"-d 1970 -I ");
      } else {
         new_command_line[0] = '\0';
      };
      strcat(new_command_line,current_command_line);
		ShellExecute(NULL, "open", image_name, new_command_line, NULL, SW_SHOW); 
		// will be toontalk_directory again once beta8c isn't needed
		return(TRUE);
	};
//	char *full_speak_name = NULL;
//	char *command_line;
//	char full_demo_name[260];
	char command_line[500];
   BOOL current_command_line_handled = FALSE;
   if (current_command_line != NULL) {
      int length = strlen(current_command_line);
      if (USR_file) {  // see if it ends in .USR because user clicked on profile file
          // is really a user name file so extract it from full path
          string name = new char[MAX_PATH];
          int name_index = MAX_PATH-1;
          name[name_index--] = 0; // terminate it and build it from back
          for (int i = length-5; i >= 0; i--) {
				  if (current_command_line[i] == '\\' || current_command_line[i] == ':') {
					  strcpy(command_line," -n ");
                 strcat(command_line,name+name_index+1);
                 current_command_line_handled = TRUE;
					  break;
				  } else {
					  name[name_index--] = current_command_line[i];
				  };
			  };
			  delete [] name;
      };
   };
	if (demo_id == 0 || demo_id == -2) { // -2 condition new on 300402
//		LoadString (hInst, ID_CMDLINE, szCmdLine, sizeof(szCmdLine));
//		command_line = current_command_line;
      if (!current_command_line_handled) strcpy(command_line,current_command_line);
   } else if (demo_id == -1) { // puzzles
      if (current_command_line_handled) {
         strcat(command_line," -next_puzzle 1 ");
      } else {
         strcpy(command_line," -next_puzzle 1 ");
         strcat(command_line,current_command_line);
      };
//      strcat(full_command_line,toontalk_directory); // could have own directory
//      strcat(full_command_line,"p1.pzl");
//      command_line = full_command_line;
	} else { 
//      char demo_name[32]; // global beginning 300402
      if (demo_name[0] == NULL) { // conditional new on 300402
		   LoadString (resource_source(), demo_id+DEMO_ID_OFFSET, demo_name, sizeof(demo_name));
      };
//		GetPrivateProfileString("Directories","DemoDir","",
//										full_demo_name,128,tt_ini_file);
//		strcat(full_demo_name,szCmdLine);
//    string full_demo_name = existing_file_name(on_cdrom,demo_name,"DMO","Demos");
		BOOL on_cdrom;
		string dmo_file_name = existing_file_name(on_cdrom,demo_name,"dmo","Demos",cdrom_installation); // new on 060401
		if (dmo_file_name == NULL) { // e.g. a trial version with only some demos installed
			MessageBox(NULL,S(IDS_DEMO_FILE_MISSING,"Sorry, this demo has not been installed."),S(IDS_START_TOONTALK),MB_ICONSTOP|MB_OK|MB_SYSTEMMODAL);
			return(TRUE); // don't report an error
		} else {
			delete [] dmo_file_name; // don't need it
		};
		if (ini_int("Switches","UseLocalIniFilesWhenLoading")) {
			string new_local_ini_file = existing_file_name(on_cdrom,demo_name,"ini","Demos",FALSE); // new on 160201
			if (new_local_ini_file != NULL) {
				if (tt_local_ini_file != NULL) {
					delete [] tt_local_ini_file;
				};
				tt_local_ini_file = new_local_ini_file;
				if (image_name != NULL) {
					delete [] image_name;
				};
				if (demo_exe_name != NULL) {
					delete [] demo_exe_name;
				};
				demo_exe_name = ini_entry("Executables","DemoToonTalk32"); // re-fetch it now that there is a new local ini file
				image_name = existing_file_name(image_on_cdrom,demo_exe_name,"EXE");
			};
		};
      char suffix[32];
      LoadString(resource_source(),IDS_NARRATION_FILE_SUFFIX,suffix,sizeof(suffix));
      string full_speak_name = existing_file_name(on_cdrom,demo_name,suffix,"Demos",FALSE);  // don't bother user
  //		OFSTRUCT file_struct;
		//if (cdrom_installation) { // condition added on 060401 -- commented out on 120304
		//	warn_audio_important();
		//};
		if (full_speak_name == NULL) {
          char *cdrom = ini_entry("Versions","CDROM");
          if (cdrom == NULL) {
//				 HGLOBAL sound_resource
//						= LoadResource(resource_source(),FindResource(resource_source(),"NOSPEAK_WAVE",RT_RCDATA));
//				 PlaySound((char *) LockResource(sound_resource),NULL,SND_SYNC|SND_MEMORY);
//				 UnlockResource(sound_resource);
//				 FreeResource(sound_resource);
				 PlaySound("NOSPEAK_WAVE",resource_source(),SND_SYNC|SND_MEMORY);
				 delete [] cdrom;
         };
		} else {
         char *wave_name;
//         if (user_initiated || autodemo == REPEAT_INTRO) {
         wave_name = "OK_WAVE";
//         } else {
//            wave_name = "ILL_PICK_WAVE"; // why waste disk space, etc. for this rare event
//         };
			HGLOBAL sound_resource = LoadResource(resource_source(),FindResource(resource_source(),wave_name,RT_RCDATA));
			sndPlaySound((char *) LockResource(sound_resource),SND_SYNC|SND_MEMORY);
			UnlockResource(sound_resource);
			FreeResource(sound_resource);
			// don't know why the following wasn't working but uncommented and updated the above on 180400 and commented out the following
//			PlaySound(wave_name,resource_source(),SND_SYNC|SND_RESOURCE); // was SND_MEMORY prior to 180400
		};
		strcpy(command_line,"-I ");
		strcat(command_line,demo_name);
		strcat(command_line," -d 1970"); // new on 070401 so that can run old expired EXEs but only to see demos
		strcat(command_line," ");
      switch (default_setting("WindowSize")) {
         case 2:
            strcat(command_line,"-f 0 ");
            break;
         case 3:
            strcat(command_line,"-f 2 ");
            break;
         case 4:
            strcat(command_line,"-f 3 ");
            break;
         case 5:
            strcat(command_line,"-f 4 ");
            break;
      };
//    strcat(command_line,"-speak 0 "); // built-in demos and speech don't mix -- even if narration not installed -- 
		// commented out on 300402 since no longer true
		if (!current_command_line_handled) strcat(command_line,current_command_line); // from the call to this starttt
//		command_line = full_command_line;
      delete [] full_speak_name;
//		full_demo_name = copy_to_exe_directory(szCmdLine,".DMO");
//		if (full_demo_name == NULL) { // troubles copying
//			command_line = szCmdLine;
//		} else {	//  move the SPK over too
//			command_line = full_demo_name;
//			full_speak_name = copy_to_exe_directory(szCmdLine,".SPK");
//		};
	};
//   if (runningHighResolution && demo_id <= 0) { // for now demos can't run in high res reliably
//      strcat(command_line," -v 25 ");
//   };
//#if RUNNING_TT16
//	char cdrom_drive[260];
//	cdrom_drive[0] = 0; // initialized at no possible drive
//	char main_drive[260];
//	main_drive[0] = 0;
// 	GetPrivateProfileString("Directories","CDROMdir","",cdrom_drive,128,tt_ini_file);
//	GetPrivateProfileString("Directories","MainDir","",main_drive,128,tt_ini_file);
//	BOOL running_off_cdrom = (toupper(cdrom_directory[0]) == toupper(szImageName[0]) &&
//									  toupper(toontalk_directory[0]) != toupper(szImageName[0]));
	//if (image_on_cdrom && demo_id != 0)  { // commented out on 120304
	//	warn_slow_cdrom_demos();
	//};
/*
	if (demo_id != 0 &&
		 !GetPrivateProfileInt("Switches","FullScreenDemoProblemExplained",0,tt_ini_file)) {
		if (GetPrivateProfileInt("Switches","FullScreenModeStarted",0,tt_ini_file)) {
			// started but never finished 
			message_box_up = TRUE;
			MessageBox(NULL,"It seems that ToonTalk crashed last time.  Sorry.  It might be because \"full screen\" mode doesn't work right on this computer.  You can turn it off by adding \"-f 2\" to the command line for this program (StartTT.EXE).",
						  "Start ToonTalk",MB_ICONINFORMATION|MB_OK);
			if (MessageBox(NULL,"Should I turn it off for now?","ToonTalk Start",
								MB_ICONQUESTION|MB_YESNO) == IDYES) {
				full_screen_mode_off = TRUE;
			};
			message_box_up = FALSE;
			WritePrivateProfileString("Switches","FullScreenDemoProblemExplained","1",tt_ini_file);
		};
	};
	if (full_screen_mode_off) strcat(command_line," -f 2");
*/
//	if (demo_id > 0 && demo_id != 4) { // demo 4 (exploding houses) redone
//		strcat(command_line," -w 1"); // for running demos made under version 4
//	};
   if (running_read_only) {
		strcat(command_line,"-read_only 1 ");
   };
	if (ini_file_name_overridden) { // new on 151204
		strcat(command_line,"-ini_file ");
		strcat(command_line,tt_ini_file); // didn't bother to add a space at the end since nothing else is added after this
	};
	long status = (long) ShellExecute(NULL, "open", image_name, command_line, demo_directory, SW_SHOW); 
	// will be toontalk_directory again once beta8c isn't needed
	if (status > 32) { // launch went ok
      // wait 5 seconds before another -- so impatient fingers don't start lots of them
		wait_before_launch = timeGetTime()+5000L;
   };
/*
	OFSTRUCT copy_struct;
	if (to_delete1 != NULL) {
		OpenFile(to_delete1,&copy_struct,OF_DELETE);
		delete [] to_delete1;
		to_delete1 = NULL;
	};
	if (to_delete2 != NULL) {
		OpenFile(to_delete2,&copy_struct,OF_DELETE);
		delete [] to_delete2;
		to_delete2 = NULL;
	};
*/
	result = (status > 32);
//#else
//	si.cb = sizeof(STARTUPINFO);
	// Yeah, this could be done with a 'memset', but this is more illustrative:
//	si.lpReserved =  NULL;
//		si.lpDesktop =  NULL;
//		si.lpTitle	= NULL;
//		si.dwX	=  0;
//		si.dwY = 0;
//		si.dwXSize	= 0;
//		si.dwYSize = 0;
//		si.dwXCountChars = 0;
//		si.dwYCountChars = 0;
//		si.dwFillAttribute =   0;
//		si.dwFlags = 0;
//		si.wShowWindow = 0;
//		si.cbReserved2 = 0;
//		si.lpReserved2 = NULL;
//		si.hStdInput =  NULL;
//		si.hStdOutput = NULL;
//		si.hStdError =	NULL;
//	result = CreateProcess (image_name, szCmdLine, NULL, NULL, FALSE,
//		                      CREATE_NEW_PROCESS_GROUP, NULL, toontalk_directory, &si, &pi);
//#endif
	if (!result) {
      HFILE file = open_starttt_log_file();
// following good for debuggin..
//      if (file == HFILE_ERROR) {
//         MessageBox(NULL,szImageName,command_line,MB_ICONINFORMATION|MB_OK);
//      };
//      file << "Error launching ToonTalk: " << message << endl;
//      file << image_name << " " << command_line << " in directory " << demo_directory << endl;
//      file.close();
      _lwrite(file,"Status=",7);
      char status_string[16];
      ltoa(status,status_string,10);
      _lwrite(file,status_string,strlen(status_string));
      char *message = " ;Error launching ToonTalk: ";
      _lwrite(file,message,strlen(message));
      _lwrite(file," ",1);
      _lwrite(file,image_name,strlen(image_name));
      _lwrite(file," ",1);
      _lwrite(file,command_line,strlen(command_line));
      _lwrite(file," ",1);
      _lwrite(file,demo_directory,strlen(demo_directory));
      _lclose(file);
   };
   delete [] image_name;
   if (use_html_dialog_file) { // new on 210602
      PostQuitMessage(0);
   };
   return(result);
};

string option_question() {
  return(S(IDS_OPTION1+current_option*10));
};

void add_to_explanation(int option, string addition) {
	if (explanation_additions == NULL) {
		explanation_additions = new string[option_count];
		for (int i = 0; i < option_count; i++) {
			explanation_additions[i] = NULL;
		};
	};
	if (explanation_additions[option] != NULL) delete [] explanation_additions[option];
	explanation_additions[option] = addition;
};

string option_explanation() {
   string answer = S(IDS_OPTION1+current_option*10+explanation_offset,2); // 2 was 3 prior to 230999
   if (explanation_additions != NULL && explanation_additions[current_option] != NULL) {
		strcat(answer,explanation_additions[current_option]);
	};
	return(answer);
};

unsigned char *disabled = NULL;

void disable_answer(int option, int answer) {
   if (disabled == NULL) {
      disabled = new unsigned char[option_count*10];
      for (int i = 0; i < option_count*10; i++) disabled[i] = 0;
   };
   disabled[option*10+answer_offset+answer] = 1;
};

void enable_answer(int option, int answer) {
   if (disabled == NULL) return;
   disabled[option*10+answer_offset+answer] = 0;
};

BOOL is_disabled_button(int answer) {
   if (disabled == NULL) return(FALSE);
   return(disabled[current_option*10+answer_offset+answer]);
};

int default_answer() {
	if (current_option == VOICE_OPTION) { // might be more than max_choices 
		if (answers[current_option]/max_choices != voice_option_visit_count-1) {
			return(-1); // no default currently displayed
		} else {
			return(answers[current_option]%max_choices);
		};
	};
   return(answers[current_option]);
};

BOOL button_checked(HWND hDlg, int answer) {
  // IsDlgButtonChecked(hDlg,IDC_OPTION_ANSWER1+answer) == BST_CHECKED only works in Win32
   DWORD result = SendDlgItemMessage(hDlg, IDC_OPTION_ANSWER1+answer, BM_GETSTATE, 0, 0);
   return((BOOL) (result & 0x0003)); // the documentation says to mask it
};

void set_dialog_button(HWND hDlg, int answer, BOOL new_state) {
   // CheckDlgButton(hDlg,IDC_OPTION_ANSWER1+answer,BST_CHECKED);  is Win32 only
   SendDlgItemMessage(hDlg, IDC_OPTION_ANSWER1+answer, BM_SETCHECK, new_state, 0);
};

int answer_selected(HWND hDlg) {
  for (int i = 0; i < question_count[current_option]; i++) {
     if (button_checked(hDlg,i)) return(i);
  };
  return(0); // shouldn't happen
};

BOOL CALLBACK option_dialog(HWND hDlg, UINT message, UINT wParam, LONG ) {
	switch (message) {
		case WM_INITDIALOG:
         SetActiveWindow(hDlg); // so dialog always easy to see
         SetDlgItemText(hDlg,IDC_OPTION_BACK,S(IDC_OPTION_BACK_TEXT));
         SetDlgItemText(hDlg,IDC_OPTION_NEXT,S(IDC_OPTION_NEXT_TEXT));
         SetDlgItemText(hDlg,IDC_OPTION_CANCEL,S(IDC_OPTION_CANCEL_TEXT));
         SetDlgItemText(hDlg,IDC_OPTION_DONE,S(IDC_OPTION_DONE_TEXT));
//			SendMessage (hDlg, WM_SETFONT, (WPARAM)hfontBtn,(LPARAM)TRUE); // this didn't seem to have any affect
         if (current_option == option_count) {
            if (answers[SYSTEM_DEFAULTS_OPTION] == 1) {
               SetDlgItemText(hDlg,IDC_OPTION_EXPLANATION,S(IDS_OPTIONS_RESTORED_TEXT));
            } else {
               SetDlgItemText(hDlg,IDC_OPTION_EXPLANATION,S(IDS_OPTIONS_CHANGED_TEXT,2));
            };
            SetWindowText(hDlg,S(IDC_OPTIONS_CHANGED_CAPTION));
         } else {
            SetDlgItemText(hDlg,IDC_OPTION_EXPLANATION,option_explanation());
            SetDlgItemText(hDlg,IDC_OPTION_QUESTION,option_question());
            SetWindowText(hDlg,S(IDC_OPTION_CAPTION));
            for (int i = 0; i < question_count[current_option]; i++) {
               SetDlgItemText(hDlg,IDC_OPTION_ANSWER_TEXT1+i,option_answer(i));
            };
				int default_choice = default_answer();
            if (default_choice >= 0) {
					set_dialog_button(hDlg,default_choice,TRUE);
				} else if (question_count[current_option] == 1) { // only choice so check it
					set_dialog_button(hDlg,0,TRUE);
				};
         };
//         for (int i = question_count[current_option]; i < 5; i++) {
//            Enable(hDlg,IDC_OPTION_ANSWER1+i,BST_INDETERMINATE);
//         };

//         CheckDlgButton(hDlg,IDC_OPTION_ANSWER1+default_answer(),BST_CHECKED);
//         SendDlgItemMessage(hDlg,IDC_OPTION_NEXT,BN_SETFOCUS,0,0);
			return(TRUE);
		case WM_COMMAND:
			switch(wParam) {
            case IDC_OPTION_DONE:
				case IDC_OPTION_NEXT:
              if (current_option == option_count) {
                 options_done = TRUE;
                 EndDialog(hDlg,0);
                 break;
              };
              answers[current_option] = answer_selected(hDlg);
				  if (current_option == VOICE_OPTION) {
					  answers[current_option] += (voice_option_visit_count-1)*max_choices;
				  };
              if (current_option == 0) {
                 if (answers[current_option] == 0) { // didn't mean to do this at all
                    options_cancelled = TRUE;
                 } else if (answers[current_option] == 1) {  // user wants to restore defaults
                    options_done = TRUE;
                 } else if (wParam == IDC_OPTION_DONE) { // clicked on DONE without setting any options
                    options_cancelled = TRUE;
                 } else {
                    current_option++;
                 };
              } else if (wParam == IDC_OPTION_DONE) {
                 options_done = TRUE;
              } else {
                 current_option++;
//                 if (current_option >= option_count) options_done = TRUE;
              };
              EndDialog(hDlg,0);
              break;
				case IDC_OPTION_BACK:
              while (current_option > 0) {
                 current_option--;
//                 if (!language_option_operational && current_option == LANGUAGE_OPTION) current_option--;
					  if (question_count[current_option] > 1) break; // there is more than one choice						  
              };
              EndDialog(hDlg,0);
              break;
            case IDC_OPTION_CANCEL:
              options_cancelled = TRUE;
				  voice_option_visit_count = 0;
              EndDialog(hDlg,0);
              break;
            case IDC_OPTION_ANSWER1:
            case IDC_OPTION_ANSWER2:
            case IDC_OPTION_ANSWER3:
            case IDC_OPTION_ANSWER4:
            case IDC_OPTION_ANSWER5:
				case IDC_OPTION_ANSWER6:
            case IDC_OPTION_ANSWER7:
              {
              int answer = wParam-IDC_OPTION_ANSWER1;
              if (button_checked(hDlg,answer)) { // IsDlgButtonChecked(hDlg,wParam) == BST_CHECKED) {
                 if (is_disabled_button(answer)) {
						  int default_choice = default_answer();
						  if (default_choice >= 0) {
							  set_dialog_button(hDlg,default_choice,TRUE);
						  };
                    set_dialog_button(hDlg,answer,FALSE);
//                    CheckDlgButton(hDlg,IDC_OPTION_ANSWER1+default_answer(),BST_CHECKED);
//                    CheckDlgButton(hDlg,IDC_OPTION_ANSWER1+answer,BST_UNCHECKED);
                 } else {
                    answers[current_option] = answer;
                 };
              };
              break;
              };
//            case IDC_OPTION_HELP:
//              EndDialog(hDlg,0);
//              break; // obsolete??
			};
			break;
	  case WM_SYSCOMMAND:
		  switch (wParam) {
				case SC_CLOSE:
					EndDialog(hDlg,0);
               options_cancelled = TRUE;
					break;
		  };
		  break;
	 };
	 return(FALSE);
};

int dialog_id() {
   if (current_option == 0) return(IDD_DIALOG4_FIRST);
   if (current_option == option_count) return(IDD_OPTIONS_CHANGED);
   return(IDD_DIALOG1+question_count[current_option]-1);
};

BOOL BeginOLE() {

   DWORD    dwVer;

// Initialize OLE

   SetMessageQueue(96);
   dwVer = CoBuildVersion();

   if (rmm != HIWORD(dwVer)) return FALSE;         // error

   if (FAILED(CoInitialize(NULL))) return FALSE;

   return TRUE;
}

// commented out on 311204 since no longer called
//BOOL EndOLE (void)
//{
//// Free up all of OLE
//
//   CoUninitialize ();
//
//   return TRUE;
//}; 

int choose_text_to_speech_voice(boolean just_check_number_of_voices) {
	if (ModeInfo != NULL) delete [] ModeInfo; // recomputing it - maybe for a different language
	ModeInfo = new TTSMODEINFOA[max_voices];
   int i;
	for (i = 0; i < max_voices; i++) {
		memset((void *) &ModeInfo[i],0,sizeof(TTSMODEINFOA));
//		ModeInfo[i].language.LanguageID = language_id; // this should filter them but doesn't seem to
	};
   HRESULT        hRes;
//   TTSMODEINFOW        ttsResult;        // final result
   PITTSENUMA      pITTSEnum;             // find interface
//   PIAUDIOMULTIMEDIADEVICE    pIAMM;      // multimedia device interface for audio-dest
//   PITTSCENTRALW  pITTSCentral;           // central interface
	
//	memset (&ModeInfo, 0, sizeof(ModeInfo));
//	BeginOLE(); // moved elsewhere on 260604 so that this is called regardless of speech situation (for HTML dialogs)
   hRes = CoCreateInstance(CLSID_TTSEnumerator, NULL, CLSCTX_ALL, IID_ITTSEnum, (void **) &pITTSEnum);
   if (FAILED(hRes)) {
//		EndOLE(); // commented out on 260604
		return(0);
	};

   ULONG mode_count;

	hRes = pITTSEnum->Next(max_voices,ModeInfo,&mode_count);

//	EndOLE(); // commented out on 260604
	
	if (FAILED(hRes)) {
		return(0);
	};
	if (just_check_number_of_voices) {
		question_count[VOICE_OPTION] = mode_count; // will be refined by language when called with !just_check_number_of_voices
		return(mode_count);
	};
	int voice_count = 0;
	if (mode_count > 0) { // was > 1 prior to 210602 but that caused problems when only voice for only one language was installed
		if (language_count > 1) {
			string spoken_language = ini_entry("User","SpokenLanguage");
			if (spoken_language != NULL) { // new on 200800 so that voice choices are offered for spoken language
				string_to_language(spoken_language); // to set language_id
				delete [] spoken_language;
			} else {
				string_to_language(languages[answers[LANGUAGE_OPTION]]); // to set language_id
			};
		};
		string default_voice = NULL; // new on 020401
		if (answers[current_option] == 0) { // haven't initialized it yet
			default_voice = ini_entry("User","TextToSpeechMode");
		};
		for (unsigned int i = 0; i < max_voices; i++) {
			if (PRIMARYLANGID(ModeInfo[i].language.LanguageID) == language_id && ModeInfo[i].szSpeaker != NULL) { 
				// second conjunct added on 120401 to avoid screens with "I want ToonTalk to use the voice named " and nothing follows - not sure it works
				// don't give the option of using wrong language option
				boolean skip_it = FALSE;
				for (unsigned int j = 0; j < i; j++) { // check old ones to eliminate duplicates
					if (strcmp(ModeInfo[i].szModeName,ModeInfo[j].szModeName) == 0) { // used szSpeaker prior to 281004 but that isn't unique
						skip_it = TRUE;
						break;
					};
				};
				if (!skip_it) {
					if (default_voice != NULL && strcmp(ModeInfo[i].szModeName,default_voice) == 0) { // new on 020401 // used szSpeaker prior to 281004 but that isn't unique
						answers[current_option] = voice_count;
					};
					option_to_mode[voice_count] = i;
					voice_count++;
				};
			};
		};
	};
	int start_voice = voice_option_visit_count*max_choices;
	if (start_voice > voice_count) { // seen them all so cycle back to first set of choices
		voice_option_visit_count = 0;
		start_voice = 0;
	};
	int stop_voice = start_voice+min(voice_count-start_voice,max_choices);
	for (i = start_voice; i < stop_voice; i++) {	
		add_to(VOICE_OPTION,i-start_voice,copy_string(ModeInfo[i].szSpeaker));
	};
	question_count[VOICE_OPTION] = min(voice_count,max_choices);
	if (voice_count > max_choices) {
		add_to_explanation(VOICE_OPTION,copy_string("There are more voices available. Return to this page to see more.")); // english.rc
	};
	return(voice_count);
};

void remove_default_entry(string key) {
   WritePrivateProfileString("Defaults",key,"",tt_ini_file);
};

void set_default_entry(string key, int new_value) {
   char new_string[20];
   itoa(new_value,new_string,10);
   WritePrivateProfileString("Defaults",key,new_string,tt_ini_file);
};

void set_new_defaults(HWND hwnd) {
  if (current_option == 0 && answers[SYSTEM_DEFAULTS_OPTION] == 1) { // restore installation defaults
     for (int i = VERSION_OPTION; i <= LANGUAGE_OPTION; i++) {
        remove_default_entry(option_names[i]);
     };
  } else {
     int stop;
     if (current_option == option_count) {
        stop = current_option;
     } else {
        stop = current_option+1;
     };
     for (int i = VERSION_OPTION; i < stop; i++) {
        set_default_entry(option_names[i],answers[i]+1);
     };
	  if (stop >= VOICE_OPTION) {
		  int mode_index = option_to_mode[answers[VOICE_OPTION]];
		  WritePrivateProfileString("User","TextToSpeechMode",ModeInfo[mode_index].szSpeaker,tt_ini_file);
		  string flag;
		  if (ModeInfo[mode_index].dwFeatures&TTSFEATURE_ANYWORD) {
			  flag = "1";
		  } else {
			  flag = "0";
		  };
		  WritePrivateProfileString("Switches","GeneralTextToSpeechEngine",flag,tt_ini_file);
	  };
	  delete [] ModeInfo; // done with it
	  ModeInfo = NULL;
  };
//  options_changed(hwnd);
  char time_string[33];
  if (answers[SYSTEM_DEFAULTS_OPTION] == 2) { // changing for all users - old commment "restore or just new users" was wrong
	  // from 230999 to 041199 things were broken since timeGetTime()/1000 replaced time(NULL)
     ltoa(time(NULL),time_string,10); // record time so user profiles can be updated
  } else {
     time_string[0] = NULL; // empty string
  };
  if (answers[KIND_OF_USER_OPTION] == 2) { // "kiosk" mode
     autodemo = REPEAT_INTRO;
     SetTimer(hwnd, 7023, max_idle_time, NULL); // if idle for long time start running demos
     toontalk_running_when_timer_set = FALSE; // need to check?
  } else {
     autodemo = NO_AUTODEMO;
  };
  WritePrivateProfileString("Defaults","LastChanged",time_string,tt_ini_file);
};

BOOL option_dialogs(HWND hwnd) {
	DLGPROC dialog_proc = (DLGPROC) MakeProcInstance((FARPROC) option_dialog,resource_source());
   options_cancelled = FALSE;
   options_done = FALSE;
   current_option = 0;
	voice_option_visit_count = 0;
   update_defaults();
   int status;
   while (!options_cancelled && !options_done) {
      while (current_option < option_count && question_count[current_option] < 2 && current_option != VOICE_OPTION) { 
			// no point asking (last conjunct added on 121199
         current_option++;
      };
      if (current_option > option_count) break;
      if (current_option == MARTY_TALK_OPTION) {
         BOOL disable;
			int voice_count = choose_text_to_speech_voice(TRUE); // to see if these options make sense
//         if (answers[VERSION_OPTION] == 0) {
//            add_to(MARTY_TALK_OPTION,0,SC(IDS_OPTION_AVAILABLE_ONLY_FOR_32BIT_VERSION));
//            add_to(MARTY_TALK_OPTION,1,SC(IDS_OPTION_AVAILABLE_ONLY_FOR_32BIT_VERSION));
//            disable = TRUE;
//         } else if (!ini_int("Versions","TextToSpeech")) {
			if (voice_count == 0) {
            add_to(MARTY_TALK_OPTION,0,SC(IDS_OPTION_NOT_INSTALLED));
            add_to(MARTY_TALK_OPTION,1,SC(IDS_OPTION_NOT_INSTALLED));
				add_to(MARTY_TALK_OPTION,5,SC(IDS_OPTION_NOT_INSTALLED)); // new on 070401 since subtitles depend upon speech
            disable = TRUE;
         } else {
            remove_addition(MARTY_TALK_OPTION,0);
            remove_addition(MARTY_TALK_OPTION,1);
            disable = FALSE;
         };
         if (disable) {
            disable_answer(MARTY_TALK_OPTION,0);
            disable_answer(MARTY_TALK_OPTION,1);
				disable_answer(MARTY_TALK_OPTION,5);
            if (answers[MARTY_TALK_OPTION] < 2) {
               answers[MARTY_TALK_OPTION] = 2; // first 2 aren't available
            };
         } else {
            enable_answer(MARTY_TALK_OPTION,0);
            enable_answer(MARTY_TALK_OPTION,1);
				enable_answer(MARTY_TALK_OPTION,5);
         };
      } else if (current_option == VOICE_OPTION) {
			choose_text_to_speech_voice(FALSE); // do this now since can be influenced by earlier choices
			voice_option_visit_count++;
		};
		if (question_count[current_option] == 0) { // no choices offered so skip this (new on 121199)
			status = 0;
			current_option++;
		} else {
			status = DialogBox(hInst,MAKEINTRESOURCE(dialog_id()),hwnd,dialog_proc);
			if (status == -1) break;
		};
   };
   if (current_option == option_count) current_option--; // last one is special -- not really a question
   if (!options_cancelled) set_new_defaults(hwnd);
   if (dialog_proc != NULL) FreeProcInstance((FARPROC) dialog_proc);
   if (additions != NULL) {
      for (int i = 0; i < option_count*10; i++) {
         if (additions[i] != NULL) delete [] additions[i];
      };
      delete [] additions;
      additions = NULL;
   };
   if (disabled != NULL) {
      delete [] disabled;
      disabled = FALSE;
   };
   return(status != -1);
};

void update_encoded_current_state() { // new on 150602
   // designed to be easy for JavaScript to parse
   // top-level items separated by ;
   // alternates between names and values (like an alist)
   // values that are lists have elements separated by commas
   output_string_stream stream(encoded_current_state,encoding_max_size);
   update_defaults();
	int i;
   for (i = 0; i <= last_option; i++) {
      stream << option_names[i] << ";" << answers[i]+1 << ";"; // +1 new on 180602 since JavaScript code expects one-indexing
   };
   stream << "Voices;";
   // warning: if any voices have semi-colons in their names this will get confused
   int voice_count = choose_text_to_speech_voice(FALSE); // new on 170602
	for (i = 0; i < voice_count; i++) {
// rewritten on 281004 to generate name in parentheses (since some vendors use name and some speaker as a name)
		// was just speaker earlier
		string speaker = ModeInfo[option_to_mode[i]].szSpeaker;
		string name = ModeInfo[option_to_mode[i]].szModeName;
		if (speaker != NULL) {
			stream << speaker;
			if (name != NULL && stricmp(speaker,name) != 0) {
				stream << " (" << name << ")";
			};
		} else if (name != NULL) {
			stream << speaker;
		};
		// rewrote the following as above on 281004
//		stream << ModeInfo[option_to_mode[i]].szSpeaker;
      if (i+1 < voice_count) stream << "{"; // unless last one -- using { rather than comma beginning 200602 since less likely to occur in name
	};
   stream << ";"; // close list
   stream << "Languages;";
   for (i = 0; i < language_count; i++) {
       stream << languages[i];
       if (i+1 < language_count) stream << "{"; // unless last one
   };
   stream.put('\0'); // new on 141002
};

/*
BOOL FAR PASCAL _export options_changed_dialog(HWND hDlg, UINT message, UINT wParam, LONG ) {
	switch (message) {
		case WM_INITDIALOG:
         SetActiveWindow(hDlg);
         if (current_option == 0 && answers[SYSTEM_DEFAULTS_OPTION] == 1) {
            SetDlgItemText(hDlg,IDS_OPTIONS_CHANGED_TEXT,S(IDS_OPTIONS_RESTORED_TEXT));
         } else {
            SetDlgItemText(hDlg,IDS_OPTIONS_CHANGED_TEXT,S(IDS_OPTIONS_CHANGED_TEXT));
         };
         SetDlgItemText(hDlg,IDC_OPTION_OK,S(IDC_OPTION_OK_TEXT));
         return(TRUE);
		case WM_COMMAND:
			switch(wParam) {
				case IDC_OPTION_OK:
               EndDialog(hDlg,0);
               break;
         };
         break;
      case WM_SYSCOMMAND:
		  switch (wParam) {
				case SC_CLOSE:
					EndDialog(hDlg,0);
               break;
        };
        break;
   };
   return(FALSE);
};

void options_changed(HWND hwnd) {
   DLGPROC dialog_proc = (DLGPROC) MakeProcInstance((FARPROC) options_changed_dialog,resource_source());
   DialogBox(hInst,MAKEINTRESOURCE(IDD_OPTIONS_CHANGED),hwnd,dialog_proc);
   if (dialog_proc != NULL) FreeProcInstance((FARPROC) dialog_proc);
};
*/

/*
BOOL OpenDocument()  {
	char message[256];
	if (part2_pending) {
		LoadString(hInst, IDS_CONFIRM_PRINT2, message, sizeof(message));
	} else {
		LoadString(hInst, IDS_CONFIRM_PRINT, message, sizeof(message));
	};
	message_box_up = TRUE;
	if (MessageBox(NULL,message,"Start ToonTalk",MB_ICONQUESTION|MB_OKCANCEL) == IDOK) {
		message_box_up = FALSE;
		GetPrivateProfileString("Directories","DOCDir","\\DOC",
										 toontalk_directory,sizeof(toontalk_directory),
										 tt_ini_file);
		// reuse message for file path
		strcpy(message,toontalk_directory);
		HINSTANCE status;
		int directory_length = strlen(message);
		int remaining_length = sizeof(message)-directory_length;
		if (part2_pending) {
			LoadString(hInst, IDS_EXPLODE_INSTRUCTIONS_FILE2, message+directory_length, remaining_length );
		} else {
			LoadString(hInst, IDS_EXPLODE_INSTRUCTIONS_FILE1, message+directory_length, remaining_length);
		};

//		status = ShellExecute(NULL, "print", message, NULL, toontalk_directory, SW_SHOW);
		char word_view[256];
		strcpy(word_view,toontalk_directory);
		strcat(word_view,"WordView\\WordView.exe");
		status = ShellExecute(NULL, "open", word_view, message, toontalk_directory, SW_SHOW);
		if (status < 32) return(FALSE);
		word_view_active = TRUE;
		part2_pending = TRUE;
//		LoadString(hInst, IDS_CONFIRM_PRINT2, message, sizeof(message));
//		if (MessageBox(NULL,message,"Start ToonTalk",MB_ICONQUESTION|MB_OKCANCEL) == IDOK) {
//			LoadString(hInst, IDS_EXPLODE_INSTRUCTIONS_FILE2, message, sizeof(message));
//			status = ShellExecute(NULL, "print", message, NULL, toontalk_directory, SW_SHOW);
//		   status = ShellExecute(NULL, "open", word_view, message, toontalk_directory, SW_SHOW);
//			return (status >= 32);
//		};
	};
   message_box_up = FALSE;
	return(TRUE);
};

#include <lzexpand.h>
char *to_delete1 = NULL;
char *to_delete2 = NULL;

char *copy_to_exe_directory(char *demo_name, char *extension) {
	// this might not be necessary once I stop using tt_beta3
	// extension will be ".dmo" or ".spk"
	char *full_demo_name = new char[128];
	GetPrivateProfileString("Directories","MainDir","",
									full_demo_name,128,tt_ini_file);
	strcat(full_demo_name,demo_name);
	strcat(full_demo_name,extension);
	OFSTRUCT destination_struct;
	if (OpenFile(full_demo_name,&destination_struct,OF_EXIST) != -1) { 
		return(full_demo_name); // is already there -- assume SPK is too
	};
	char *cdrom_demo_name = new char[128];
	GetPrivateProfileString("Directories","CDROMdir","",
									cdrom_demo_name,128,tt_ini_file);
	strcat(cdrom_demo_name,demo_name);
	strcat(cdrom_demo_name,extension);
	int last_char_location = strlen(cdrom_demo_name)-1;
	char last_char = cdrom_demo_name[last_char_location];
	cdrom_demo_name[last_char_location] = '_'; // used for compression
	OFSTRUCT source_struct;
	if (OpenFile(cdrom_demo_name,&source_struct,OF_EXIST) == -1) { // oh well
		cdrom_demo_name[last_char_location] = last_char; // restored
	};
	HFILE source = LZOpenFile(cdrom_demo_name,&source_struct,OF_READ);
	HFILE destination = LZOpenFile(full_demo_name,&destination_struct,OF_CREATE);
	LZStart();
	// could put up a window informing that loading is happening
	// and get rid of it when my window ready...
	long status = CopyLZFile(source,destination); // will work whether compressed or not
	LZDone();
	delete [] cdrom_demo_name;
	if (to_delete1 == NULL) {
		to_delete1 = full_demo_name;
	} else {
		to_delete2 = full_demo_name;
	if (sutatus > 0) { // went ok
		return(full_demo_name);
	};
	return(NULL); // something wrong
};
*/
