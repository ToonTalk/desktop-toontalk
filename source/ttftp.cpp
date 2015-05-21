// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved

// A ToonTalk publishing tool
// Typically invoked by ToonTalk after saving
// May zip files (if a Java applet)
// FTP's files to a server

// useful for debugging
#define TT_DEBUG_ON 0
#define TT_REAL_FTP 1

#include "stdafx.h"

// Windows Header Files:
#include <windows.h>
//#include <mmsystem.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <shellapi.h>
#include <string.h>
#include <ctype.h>

//#include <time.h>
//#include <sys\timeb.h> // why both?

#include "ttftp.h"
#if !defined(__TT_DEFS_H) 
#include "defs.h"
#endif
#if !defined(__TT_COMMON_H) 
#include "common.h"
#endif

#include <shlobj.h> // new on 080402 to find correct directories to put things
#include <shfolder.h> // new on 260402 to find correct directories to put things

//#include <mmsystem.h>
#include <initguid.h>
#include <objbase.h>
#include <objerror.h>
#include <ole2ver.h>

#include "C:\Program Files\DynaZip Max 5.0\samples\Static Lib\MS Visual C\Common\dzsll32.h"
#include "C:\Program Files\DynaZip Max 5.0\samples\Static Lib\MS Visual C\Common\duzsll32.h"

#include "ftp.h"

extern char tt_toontalk_crash_directory[MAX_PATH];
char tt_toontalk_crash_directory[MAX_PATH];

string tt_local_ini_file = NULL;
string tt_ini_file = NULL; 
string tt_ftp_ini_file = NULL; 

string toontalk_directory = NULL;
string toontalk_main_directory = NULL;

extern char tt_application_name[]; // made external for mini dumps
char tt_application_name[] = "TT_FTP";

HINSTANCE tt_main_instance = NULL;
HWND tt_window_handle = NULL;

// for use by mdump - can be overridden by toontalk.ini
string tt_ok_to_generate_mini_dump_message = "Sorry, something is wrong with the ToonTalk FTP Publishing Tool. Is it OK to generate a diagnostic file to help fix the problem?"; // set here in case error occurs before reset by toontalk.ini
string tt_mini_dump_email = "mailto:support@toontalk.com?subject=ToonTalk Publisher Crash Report&body=Please add any information that might be useful. And don't forget the attachment."; 
BOOL tt_show_mini_dump_directory = TRUE;
BOOL tt_full_memory_dump = FALSE;
void pause_toontalk(boolean interrupted, boolean ask, boolean shutting_down) {
   // only useful for mdump when DirectX has taken control
};

string copy_string(string source, int length) { 
   if (source == NULL) return(NULL);
   if (length < 0) length = strlen(source);
   string destination = new char[length+1];
   memcpy(destination,source,length);
   destination[length] = 0; // terminate the string
   return(destination);
};

string ini_entry(char *category, char *id) {
	const int max_size = 512; 
	string answer = new char[max_size]; 
	if (tt_local_ini_file != NULL) { 
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

boolean ftp_ini_entry(string category, string key, string answer, int max_size, boolean required) {
	if (tt_ftp_ini_file == NULL) {
		return(FALSE);
	};
	boolean OK = (GetPrivateProfileString(category,key,"",answer,max_size,tt_ftp_ini_file) != 0);
	if (!OK && required) {
		char message[512];
		strcpy(message,"The tt_ftp.ini file doesn't have a setting for ");
		strcat(message,key);
		strcat(message," in the ");
		strcat(message,category); // was strcpy prior to 041004
		strcat(message,"section.\nQuitting, sorry.");
		MessageBox(NULL,message,"TT FTP Publishing Tool Error",MB_ICONSTOP|MB_OK);
		exit(-1);
	};
	return(OK);
};

int ini_int(char *category, char *id) {
	int answer = min_long;
	if (tt_local_ini_file != NULL) { 
		answer = GetPrivateProfileInt(category,id,min_long,tt_local_ini_file);
	};
	if (answer == min_long) {
		answer = GetPrivateProfileInt(category,id,0,tt_ini_file); 
	};
	return(answer);
};

int short_file_name_start(ascii_string path) { 
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

string show_html_dialog_named_in_ftp_ini_file(string key, string to_dialog) { // new on 300502
   char html_dialog_file[MAX_PATH];
   ftp_ini_entry("HTML",key,html_dialog_file,MAX_PATH,TRUE);
   int dialog_width = ini_int("Switches","DialogWidth");
   int dialog_height = ini_int("Switches","DialogHeight");
   int dialog_left = ini_int("Switches","DialogLeft"); // 0 was -1 prior to 240602
   int dialog_top = ini_int("Switches","DialogTop");
	char full_file_name[MAX_PATH];
	strcpy(full_file_name,toontalk_main_directory);
	strcat(full_file_name,"Doc\\");
	strcat(full_file_name,html_dialog_file);
   return(show_html_dialog(full_file_name,tt_window_handle,to_dialog,dialog_width,dialog_height,dialog_left,dialog_top));
};

inline boolean new_line(char c) { // new on 080802 since if coming from XML '\n' will be there but not if typed to a text pad
   return(c == '\r' || c == '\n');
};

int message_box(string message, UINT style=MB_ICONSTOP|MB_OK);

int message_box(string message, UINT style) {
   // this covers the current usage of message_box (as of 030602)
	if (toontalk_main_directory == NULL) { // new on 191204
		return(MessageBox(NULL,message,"ToonTalk FTP",style));
	};
   char style_string[1024];
   style_string[0] = '\0';
	byte style_byte = (byte) (style&0x7); // new on 300304 since old test mistook MB_RETRYCANCEL for MB_YESNO
	if (style_byte == MB_YESNO) { // was style&MB_YESNO
      strcat(style_string,"YESNO;");
	} else if (style_byte == MB_RETRYCANCEL) { 
      strcat(style_string,"RETRYCANCEL;");
   } else {
      strcat(style_string,"OK;");
   };
   if (style&MB_ICONSTOP) {
      strcat(style_string,"ICONSTOP;");
   } else if (style&MB_ICONEXCLAMATION) {
      strcat(style_string,"ICONEXCL;");
   } else if (style&MB_ICONINFORMATION) {
      strcat(style_string,"ICONINFO;");
   } else if (style&MB_ICONQUESTION) {
      strcat(style_string,"ICONQUES;");
   } else {
      strcat(style_string,"NO ICON;");
   };
   if (message != NULL) { // do this last in case it has semicolons inside
      // can't use strcat because need to turn newlines into <br>
      int message_length = strlen(message); // what about Japanese?
      int style_string_index = strlen(style_string);
      for (int i = 0; i < message_length; i++) {
         if (!new_line(message[i])) { // was  != '\r' prior to 090802
            style_string[style_string_index++] = message[i];
         } else { // two <br>
            style_string[style_string_index++] = '<';
            style_string[style_string_index++] = 'b';
            style_string[style_string_index++] = 'r';
            style_string[style_string_index++] = '>';
            style_string[style_string_index++] = '<';
            style_string[style_string_index++] = 'b';
            style_string[style_string_index++] = 'r';
            style_string[style_string_index++] = '>';
         };
      };
      style_string[style_string_index] = '\0'; // terminate string
   };
   string result = show_html_dialog_named_in_ftp_ini_file("MessageBox",style_string);
	if (dialog_closed(result)) return(-1);
   if (strcmp(result,"OK") == 0) return(IDOK);
   if (strcmp(result,"YES") == 0) return(IDYES);
   if (strcmp(result,"NO") == 0) return(IDNO);
   if (strcmp(result,"RETRY") == 0) return(IDRETRY);
   if (strcmp(result,"CANCEL") == 0) return(IDCANCEL);
   return(-1);
};

int zip_file(string archive, string source) {
	DzipProcessInit(tt_main_instance);
	DzipThreadInit(); // assumes that only one thread uses this
	ZIPCMDSTRUCT zip_commands;
	ZeroMemory(&zip_commands, sizeof(ZIPCMDSTRUCT)); 
	zip_commands.zipStructSize = sizeof(ZIPCMDSTRUCT);
	zip_commands.compFactor = 9; // not really needed here
	zip_commands.quietFlag = TRUE;
	char zip_items_list[MAX_PATH+2]; // 2 extra in case quoted 
	zip_commands.lpszItemList = zip_items_list;
	zip_commands.dontCompressTheseSuffixesFlag = TRUE;
	zip_commands.lpszStoreSuffixes = "PNG:GIF:JPG"; // no point compressing these - I think it is case insensitive - new on 210703
	zip_commands.lpszZIPFile = archive;
	zip_commands.function = ZIP_ADD;
	strcpy(zip_items_list,source);
	zip_commands.noDirectoryNamesFlag = TRUE;
	int result = dzip(&zip_commands);
	DzipThreadDeInit();
	DzipProcessDeInit();
	return(result);
};

boolean ftp_error() {
	int error_code = GetLastError();
	const int max_message_size = 10000;
	char message[max_message_size]; // should be more than enough
	boolean expert = FALSE;
	if (ftp_ini_entry("Messages","ExpertFTPError",message,max_message_size,FALSE)) { // don't display anything if no entry
		expert = TRUE; // can provide more information
		strcat(message,"\n");
		string error_message = NULL;
		DWORD error;
		int initial_message_length = strlen(message);
		DWORD size_remaining = max_message_size-initial_message_length;
		if (error_code == ERROR_INTERNET_EXTENDED_ERROR && InternetGetLastResponseInfo(&error,message+initial_message_length,&size_remaining)) {
         //An extended error was returned from the server. This is
         //typically a string or buffer containing a verbose error
         //message. Call InternetGetLastResponseInfo to retrieve the
         //error text.
		} else if (error_code == ERROR_FTP_TRANSFER_IN_PROGRESS) {
			strcat(message,"The requested operation cannot be made on the FTP session handle because an operation is already in progress.");
		} else if (error_code == ERROR_FTP_DROPPED) {
			strcat(message,"The FTP operation was not completed because the session was aborted.");
		} else if (error_code == ERROR_INTERNET_OUT_OF_HANDLES) {
         strcat(message,"No more handles could be generated at this time.");
		} else if (error_code == ERROR_INTERNET_TIMEOUT) {
         strcat(message,"The request has timed out. Try again later.");
  		} else if (error_code == ERROR_INTERNET_INTERNAL_ERROR) {
         strcat(message,"An internal error has occurred.");           
  		} else if (error_code == ERROR_INTERNET_NAME_NOT_RESOLVED) {
         strcat(message,"The server name could not be resolved.");  
   	} else if (error_code == ERROR_INTERNET_PROTOCOL_NOT_FOUND) {
         strcat(message,"The requested protocol could not be located.");                  
		} else if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
										  NULL,
										  error_code,
										  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
										  (LPTSTR) &error_message,
										  0,
										  NULL)) {
			itoa(error_code,message+strlen(message),10);
		} else {
			strcat(message,error_message);
			// Free the buffer.
			LocalFree(error_message);
		};
		message_box(message);
		ftp_ini_entry("Messages","NonExpertFTPError",message,5120,TRUE);
		message_box(message);
	};
	return(expert);
};

std::ofstream tt_err_file;

#if TT_DEBUG_ON
void open_log() {
	tt_err_file.open("c:\\tt\\ttftp.txt",std::ios_base::out);
};
#endif

#if TT_DEBUG_ON
void log(string message) {
	tt_err_file << message << endl;
	if (GetLastError() != 0) {
		tt_err_file << "And last error is " << GetLastError() << endl;
	};
};
#endif

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {

		//case WM_CREATE:
		//	return PrepareWindow (hWnd);


		case WM_PAINT:
         {
            // if showing HTML dialogs then could show background pattern - only see this because it takes time to paint a new HTML dialog - new on 210602
            	PAINTSTRUCT ps;
				HDC hdc;
				string message = "Please wait...";
            hdc = BeginPaint (hWnd, &ps);
            TextOut(hdc,0,0,message,strlen(message));
            EndPaint (hWnd, &ps);
         };
			break;        

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		// else fall thru to the following
		default:
			return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return (0);
}

BOOL register_class_name() {
	WNDCLASS  wc;
	// Fill in window class structure with parameters that describe
	// the main window.
	wc.lpszClassName = tt_application_name;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = (WNDPROC)WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = tt_main_instance;
	wc.lpszMenuName  = NULL;
	wc.hIcon         = LoadIcon (tt_main_instance, "LARGEICON");
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	return(RegisterClass(&wc));
};

boolean file_exists(string name) { // copied here on 270904 (and simplified)
	HANDLE handle = CreateFile(name,0,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (handle == INVALID_HANDLE_VALUE) {
		return(FALSE);
	} else {
		CloseHandle(handle);
		return(TRUE);
	};  
};

string expand_question_mark_as_module_path(string directory) { // new on 191204 -- so UserFiles can reference MainDir
	int length = strlen(directory);
	char module_directory[MAX_PATH];
	if (::GetModuleFileName(NULL,module_directory,MAX_PATH)) {
		int directory_end = max(0,short_file_name_start(module_directory)); // if somehow this returns -1 then make module_directory the empty string
		module_directory[directory_end] = '\0';
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
	} else {
		return(NULL);
	};
};

int PASCAL WinMain(HINSTANCE instance, HINSTANCE previous_instance, LPSTR command_line, int show) {
	tt_main_instance = instance;
#if TT_DEBUG_ON
	open_log();
#endif
	//if (!register_class_name()) {
	//	log("Problem registering class name"); // do what?
	//};
	//tt_window_handle = CreateWindow(tt_application_name,"ToonTalk FTP Publishing Tool",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,0,CW_USEDEFAULT,0,NULL,NULL,instance,NULL);
	//int x = 0;
	//if (tt_window_handle == NULL) {
	//	x = GetLastError();		
	//};
	if (FAILED(CoInitialize(NULL))) return(FALSE); // new on 150604
	if (command_line == NULL) return(FALSE); // new on 191204
	
	char ini_file_name[MAX_PATH]; // this is to find out if the file exists -- huh? asked 270904
	string search_for = " -ini_file ";
	string ini_file_command_line = strstr(command_line,search_for);
	if (ini_file_command_line == NULL) {
		get_my_documents_folder(tt_toontalk_crash_directory); 
		strcat(tt_toontalk_crash_directory,"\\ToonTalk\\");
//    CreateDirectory(tt_toontalk_crash_directory,NULL); // commented out on 191204
		strcat(tt_toontalk_crash_directory,"Crash Reports\\");
//    CreateDirectory(tt_toontalk_crash_directory,NULL);
		get_my_documents_folder(ini_file_name); 
		strcat(ini_file_name,"\\ToonTalk\\toontalk.ini");
	} else {
		// new on 191204
		strcpy(ini_file_name,ini_file_command_line+strlen(search_for));
		ini_file_command_line[0] = '\0'; // leave just the file path
		// OK that first look in My Docs for tt_ftp.ini ??
	};
   tt_ini_file = copy_string(ini_file_name);
	tt_ftp_ini_file = new char[MAX_PATH];
	get_my_documents_folder(tt_ftp_ini_file);
	strcat(tt_ftp_ini_file,"\\ToonTalk\\tt_ftp.ini");
	if (!file_exists(tt_ftp_ini_file)) { // new on 270904 if file is missing copy it from Windows directory
		char backup_ftp_ini_file[MAX_PATH];
		if (get_special_folder(CSIDL_WINDOWS,backup_ftp_ini_file)) {
          strcat(backup_ftp_ini_file,"\\tt_ftp.ini");
			 if (!file_exists(backup_ftp_ini_file) || !CopyFile(backup_ftp_ini_file,tt_ftp_ini_file,TRUE)) {
//				 string main_toontalk_directory = ini_entry("Directories","MainDir"); // new on 011204 -- giving priority to UserFiles as of 191204
				 string user_files_directory = ini_entry("Directories","UserFiles"); // new on 181204
				 if (user_files_directory == NULL) {
					 user_files_directory = ini_entry("Directories","MainDir");
				 };
				 if (user_files_directory != NULL) {
					 if (user_files_directory[0] == '?') {
						 strcpy(user_files_directory,expand_question_mark_as_module_path(user_files_directory));
					 };
					 strcpy(backup_ftp_ini_file,user_files_directory);
					 if (user_files_directory[strlen(user_files_directory)-1] != '\\') {
						 strcat(backup_ftp_ini_file,"\\tt_ftp.ini");
					 } else {
						 strcat(backup_ftp_ini_file,"tt_ftp.ini");
					 };
				 };
				 if (file_exists(backup_ftp_ini_file)) {
//					 if (!CopyFile(backup_ftp_ini_file,tt_ftp_ini_file,TRUE)) { // if fails to copy then just use backup version
					 // new on 191204 to just use the backup as the "real" one
						 strcpy(tt_ftp_ini_file,backup_ftp_ini_file);
//					 };
				 } else {
					// new on 041004
					char message[2*MAX_PATH+1000];
					strcpy(message,"Sorry. The ToonTalk FTP Publishing Tool could not find either\n");
					strcat(message,tt_ftp_ini_file);
					strcat(message," or\n");
					strcat(message,backup_ftp_ini_file);
					strcat(message,"\nPerhaps you need to re-install ToonTalk.");
					MessageBox(NULL,message,"TT FTP Publishing Tool Error",MB_ICONSTOP|MB_OK);
					exit(-1);
				 };
			 };
		};
	};
	tt_local_ini_file = ini_entry("Executables","UseThisIniFileFirst");  
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
	toontalk_main_directory = ini_entry("Directories","MainDir");
	if (toontalk_main_directory == NULL) {
		message_box("The tt_ftp.ini file doesn't have a setting for MainDir so the ToonTalk FTP Publishing Tool doesn't know where its files are.",
					  MB_ICONSTOP|MB_OK);
		exit(-1);
	} else if (toontalk_main_directory[0] == '?') {
		if (toontalk_main_directory[0] == '?') {
			strcpy(toontalk_main_directory,expand_question_mark_as_module_path(toontalk_main_directory));
		};
	};
	string old_user_and_password = ini_entry("FTP","UserName");
	string response = show_html_dialog_named_in_ftp_ini_file("Start",old_user_and_password); // "name;Demo;password;thule");
	if (response == NULL) {
		message_box("The tt_ftp.ini file doesn't have a setting for Start in the HTML section so the ToonTalk FTP Publishing Tool doesn't know where the starting dialog is.",
					   MB_ICONSTOP|MB_OK);
		exit(-1);
	};
	string user_and_password = new char[strlen(response)+17]; // extra for name; and ;password;
	if (dialog_closed(response)) {
		exit(0); // user didn't want to publish
	} else if (response != NULL && response[0] == '.') { // see folder -- new on 090904
		char user_programs_directory[MAX_PATH];
		string full_path = command_line;
		if (command_line[0] == '"') {
			full_path++; // skip the quote
		};
		int directory_length = short_file_name_start(full_path);
		if (directory_length > 0) {
			memcpy(user_programs_directory,full_path,directory_length);
			user_programs_directory[directory_length] = '\0';
		} else { // broken -- warn?
			exit(0);
		};
		int result = (int) ShellExecute(NULL,"open",user_programs_directory,"","",SW_SHOW); // show directory
#if TT_DEBUG_ON
		if (result <= 32) {
			log("ShellExecute failed.");
		};
#endif
		exit(0);
	} else {
		char server_name[512];
		ftp_ini_entry("FTP","Server",server_name,512,TRUE);
#if TT_DEBUG_ON
		log("About to call internet_open");
#endif
		char wait_message[1024]; // moved here on 011104 in case opening the connection takes noticable amount of time
		if (ftp_ini_entry("Messages","PleaseWait",wait_message,1024,FALSE)) {
			message_box(wait_message,MB_OK);
		};
#if TT_REAL_FTP
		internet_open("ToonTalk FTP Publishing Tool");
#endif
		string password = strchr(response,':');
		if (password != NULL) {
			password[0] = '\0'; // terminate the user name
			password++; // skip over the :
		};
		string user_name = response;
#if TT_DEBUG_ON
		log("About to call ftp_open_connection");
#endif
#if TT_REAL_FTP
		ftp_handle ftp = ftp_open_connection(server_name,response,password);
#else
		int ftp = 1;
#endif
		if (ftp == NULL) {
			ftp_error();
			exit(-1);
		} else {
			strcpy(user_and_password,"name;");
			strcat(user_and_password,user_name);
			strcat(user_and_password,";password;");
			strcat(user_and_password,password);
			char directory[MAX_PATH];
			if (ftp_ini_entry("FTP","Directory",directory,MAX_PATH,FALSE)) { // if not set put file at top level
				string user_name_location = strchr(directory,'*');
				if (user_name_location != NULL) {
					char rest_of_path[MAX_PATH];
					strcpy(rest_of_path,user_name_location+1); // skip over the *
					strcpy(user_name_location,user_name);
					strcat(user_name_location,rest_of_path);
				};
#if TT_DEBUG_ON
				log("About to call ftp_set_directory");
#endif
#if TT_REAL_FTP
				if (!ftp_set_directory(ftp,directory)) {
					if (ftp_error()) { // expert so say more
						char message[MAX_PATH*2];
						strcpy(message,"FTP set directory to ");
						strcat(message,directory);
						message_box(message);
					};
					exit(-1);
				};
#endif
			};
			if (command_line[0] == '"') { // 150604
				command_line++;
				int command_line_length = strlen(command_line);
				if (command_line[command_line_length-1] == '"') {
					command_line[command_line_length-1] = '\0'; // terminate it early
				};
			};
			char local_file_name[MAX_PATH];
			char remote_file_name[MAX_PATH];
			int short_file_start = short_file_name_start(command_line);
			boolean local_file_is_temp = FALSE;
			if (strstr(command_line,"\\Java\\") != NULL) {
				// need to zip them all up
				char source[MAX_PATH];
				int command_line_length = strlen(command_line);
				memcpy(local_file_name,command_line,command_line_length-2); // except for TT
				memcpy(local_file_name+command_line_length-2,"zip",4); // including terminator
				source[0] = '"'; // quote the name
				memcpy(source+1,command_line,short_file_start); // copy all but short file name
				memcpy(source+short_file_start+1,"*.*\"",5); // including terminator
				int result = zip_file(local_file_name,source);
				if (result != ZE_OK) {
					char message[1024];
					ftp_ini_entry("Messages","ZipError",message,1024,TRUE);
					message_box(message);
					exit(-1);
				};
				strcpy(remote_file_name,local_file_name+short_file_name_start(local_file_name));
				local_file_is_temp = TRUE;
			} else {
				strcpy(local_file_name,command_line);
				strcpy(remote_file_name,command_line+short_file_start);
			};
#if TT_DEBUG_ON
			log("About to call ftp_put_file");
#endif
#if TT_REAL_FTP
			if (!ftp_put_file(ftp,local_file_name,remote_file_name)) {
				if (ftp_error()) { // expert so say more
					char message[MAX_PATH*3];
					strcpy(message,local_file_name);
					strcat(message," was being uploaded as "); // added ed to upload on 090105
					strcat(message,remote_file_name);
					message_box(message);
				};
				exit(-1);
			};
#endif
			if (local_file_is_temp) {
				DeleteFile(local_file_name);
			};
#if TT_DEBUG_ON
			log("About to call ftp_close_connection");
#endif
#if TT_REAL_FTP
			ftp_close_connection(ftp);
#endif
		};
#if TT_REAL_FTP
		internet_close();
#endif
		if (user_and_password != NULL) {
			WritePrivateProfileString("FTP","UserName",user_and_password,tt_ini_file);
		};
		string launch_browser = show_html_dialog_named_in_ftp_ini_file("Finished",""); // NULL);
		if (launch_browser != NULL && launch_browser[0] == '1') {
			char website[512];
			if (ftp_ini_entry("HTML","WebSite",website,512,FALSE)) {
				ShellExecute(NULL,"open",website,"","",SW_SHOWMAXIMIZED);
			};
		};	
   };
	return(0);
};
