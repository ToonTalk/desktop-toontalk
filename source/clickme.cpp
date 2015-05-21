// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved

#define TT_ENGLISH 1 // uncommented on 101002
// change the above to generate for other languages
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <shellapi.h>
// Local Header Files
#include "clickme.h"

#include <string.h>

#include <shlobj.h> // new on 120402 to find correct directories to put things
#include <shfolder.h> // new on 260402 to find correct directories to put things

// following added on 020502 - based upon MS documentation associated with ShowHTMLDialog

// for the HTML dialog
#include <urlmon.h>
#include <mshtmhst.h>

#define ARRAYSIZE(a)    (sizeof(a)/sizeof((a)[0]))

/**************************************************************************

   BSTRToLocal()
   
**************************************************************************/

int BSTRToLocal(LPTSTR pLocal, BSTR pWide, DWORD dwChars)
{
*pLocal = 0;

#ifdef UNICODE
lstrcpyn(pLocal, pWide, dwChars);
#else
WideCharToMultiByte( CP_ACP, 
                     0, 
                     pWide, 
                     -1, 
                     pLocal, 
							dwChars, 
                     NULL, 
                     NULL);
#endif

return lstrlen(pLocal);
}

/**************************************************************************

   LocalToBSTR()
   
**************************************************************************/

int LocalToBSTR(BSTR pWide, LPTSTR pLocal, DWORD dwChars)
{
*pWide = 0;

#ifdef UNICODE
lstrcpyn(pWide, pLocal, dwChars);
#else
MultiByteToWideChar( CP_ACP, 
                     0, 
                     pLocal, 
                     -1, 
                     pWide, 
                     dwChars); 
#endif

return lstrlenW(pWide);
}

HINSTANCE hinstMSHTML = NULL;
SHOWHTMLDIALOGFN *pfnShowHTMLDialog = NULL;

string CLOSED_STRING = "***CLOSED***";

string show_html_dialog(string html_file_name) {
   if (hinstMSHTML == NULL) {
      hinstMSHTML = LoadLibrary(TEXT("MSHTML.DLL"));
      if (hinstMSHTML == NULL) {
         return(NULL);
      };
   };
   if (pfnShowHTMLDialog == NULL) {  
      pfnShowHTMLDialog = (SHOWHTMLDIALOGFN *) GetProcAddress(hinstMSHTML, TEXT("ShowHTMLDialog"));
      if (pfnShowHTMLDialog == NULL) {
         // warn??
         return(NULL);
      };
   };
   IMoniker *pmk;
   OLECHAR  bstr[MAX_PATH*2];
   LocalToBSTR(bstr, html_file_name, ARRAYSIZE(bstr));
   CreateURLMoniker(NULL, bstr, &pmk);
   if (pmk) {
      HRESULT  hr;
      VARIANT  varReturn;
      VariantInit(&varReturn);
      const int parameters_size = 1024;
      char parameters[parameters_size];
      // updated to use size of real window
      HDC device_context = GetDC(NULL);
      int screen_width = GetDeviceCaps(device_context,HORZRES);
      int screen_height = GetDeviceCaps(device_context,VERTRES);
      lstrcpy(parameters,TEXT("dialogWidth: "));
      itoa(screen_width,parameters+strlen(parameters),10);
      lstrcat(parameters,TEXT("px; dialogHeight: "));
      itoa(screen_height,parameters+strlen(parameters),10);
//      lstrcat(parameters,TEXT("px; center: Yes; help: No;"));
      lstrcat(parameters,TEXT("px; dialogLeft: 0px; dialogTop: 0px; Yes; help: No;")); // updated 240602 since center doesn't seem to always do the right thing
      LocalToBSTR(bstr, parameters, parameters_size);
      hr = (*pfnShowHTMLDialog)(NULL, pmk, NULL, bstr, &varReturn);
      pmk->Release();
      if(SUCCEEDED(hr)) {
         const int max_result_size = 1024;
         string result = new char[max_result_size];
         switch(varReturn.vt) {
            case VT_BSTR:
               BSTRToLocal(result, varReturn.bstrVal, max_result_size);
               VariantClear(&varReturn);
               return(result);
            default:
               strcpy(result,CLOSED_STRING); // since NULL is used for "errors"
               return(result);
         }
      }
   }
   return(NULL);
};

// controls whether I'm launching 16 or 32 bit version of ToonTalk
// actually it works fine for either so no need to change for 32-bit -- revisit this??
// now controls whether shellex or createprocess (not fully debugged) is operational
#define RUNNING_TT16 1

// this file was based originally upon the following:
/******************************************************************************\
    PROGRAM:    AutoRun.C
    Author:     Robert B. Hess
    Date:       1-Oct-94
\******************************************************************************/
// The program will be executed relative to the root of the CD,
// no drive letter is necessary.  We strongly recommend that the
// autorun application be a small fast loading executable that
// immediatly presents the option to discontiue execution of the
// autorun.  This is to give the user an easy way out if they did
// not expect, or intend to launch an application.  Autorun CD's
// should always present an uninstall option to clean up the
// hardrive at the users request.  In general Autorun CD's should
// be very cautious about leaving anything on the users hardrive
// on closing.

/////////////////////////////////////////////////////////////////////////
// Global Variables:

HINSTANCE hInst; // current instance
//char szAppName[] = APPNAME; // The name of this application

char app_name[] = "ClickMe";
char toontalk_exe_name[] = "ToonTalk";

//#if TT_ENGLISH
//#if TT_32
// is renamed anyway unless running off of CDROM
//char start_toontalk_name[] = "STT32"; 
//#else
//char start_toontalk_name[] = "StartTT"; 
//#endif
//#elif TT_SWEDISH
//#if TT_32
//char start_toontalk_name[] = "STT32SE"; 
//#else
//char start_toontalk_name[] = "StartaTT"; 
//#endif
//#endif

//char szTitle[]   = APPNAME": ToonTalk"; // The title bar text
//char szHelpFileName[] = APPNAME".HLP";

char toontalk_directory[MAX_PATH] = "";
char cdrom_directory[MAX_PATH] = "";
char users_directory[MAX_PATH] = ""; // new on 120402
char ini_file_name[MAX_PATH]; // moved here on 120402

HGLOBAL global_handle = 0;

char device_of_this_app[3]; //e.g. "d:"

char directory_if_not_installed[MAX_PATH]; // = "a:\\"; // default

/////////////////////////////////////////////////////////////////////////
// Foward declarations of functions included in this code module:

BOOL InitApplication(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CenterWindow (HWND, HWND);
BOOL CenterWindowHorizontally (HWND, HWND);
//LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);

// The following functions were added to Generic for this AutoRun sample app:
int PrepareWindow (HWND hWnd);
BOOL LaunchApplication();

BOOL first_time = TRUE;
//BOOL toontalk_ini_exists; - obsolete beginning 120402

BOOL toontalk_installed; // moved here on 120402

HWND hwndBtnOk = NULL;
HWND hwndBtnExit= NULL;
HWND hwndBtnInstall = NULL; // moved here so I can destroy them second time round

HMODULE hModSHFolder = NULL; // no need to release this since is released on exit

BOOL get_special_folder(int CSIDL, char *path) { // new on 260402
   // based upon http://msdn.microsoft.com/library/default.asp?url=/library/en-us/apcompat/appcomp_0dwz.asp
   // and Microsoft's Q241733
   // SHGetFolderPath can work everywhere SHFOLDER is installed.
   if (hModSHFolder == NULL) {
      hModSHFolder = LoadLibrary("SHFolder.dll");
   };
   if ( hModSHFolder != NULL ) {
      PFNSHGETFOLDERPATHA pSHGetFolderPath = (PFNSHGETFOLDERPATHA)GetProcAddress(hModSHFolder,"SHGetFolderPathA"); 
      if (pSHGetFolderPath != NULL )
         return(pSHGetFolderPath(NULL, CSIDL|CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, path) == S_OK);
   };
   return(FALSE);
};

#if TT_ENGLISH
char clickme_html_relative_path[] = "ToonTalk\\Doc\\English\\clickme.htm"; // original un-installed version -- relative to ClickMe.exe
#elif TT_SWEDISH
char clickme_html_relative_path[] = "ToonTalk\\Doc\\Swedish\\clickme.htm";
#elif TT_PORTUGUESE
char clickme_html_relative_path[] = "ToonTalk\\Doc\\BR\\clickme.htm";
#elif TT_EUROPEAN_PORTUGUESE
char clickme_html_relative_path[] = "ToonTalk\\Doc\\PT\\clickme.htm";
#elif TT_JAPANESE
char clickme_html_relative_path[] = "ToonTalk\\Doc\\JP\\clickme.htm"; // new on 101002
#endif 

// move into language specific once this settles down
char not_installed_html_relative_path[] = "ToonTalk\\Doc\\English\\noinstll.htm"; // for now -- can be more clever later

/************************************************************************\
 *    FUNCTION: WinMain
\************************************************************************/

boolean has_main_dir_entry(string ini_file) { // new on 170105 to ignore toontalk.ini files that don't even set MainDir
	if (ini_file == NULL) return(FALSE);
	OFSTRUCT file_struct;
	if (OpenFile(ini_file,&file_struct,OF_EXIST) == -1) {
//		MessageBox(NULL,ini_file,"does not exist",MB_OK); // for debugging 040205
		return(FALSE);
	};
//	MessageBox(NULL,ini_file,"does exist",MB_OK); // for debugging 040205
	char entry[MAX_PATH];
	if (GetPrivateProfileString("Directories","MainDir","",entry,MAX_PATH,ini_file) > 0) {
		return(TRUE);
	};
	if (GetPrivateProfileString("Executables","UseThisIniFileFirst","",entry,MAX_PATH,ini_file) > 0) {
		return(has_main_dir_entry(entry));
	};
	return(FALSE);
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR , int nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;
   HWND toontalk_window = FindWindow("ToonTalk",NULL);
	if (toontalk_window == NULL) {
		toontalk_window = FindWindow("StartTT",NULL);
	};
	if (toontalk_window != NULL) {
		if (IsIconic(toontalk_window)) {
			ShowWindow(toontalk_window, SW_RESTORE);
		} else {
//#if TT_32      
         SetForegroundWindow(toontalk_window);
//#else         
//			SetActiveWindow(toontalk_window);
//#endif         
		};
		return(FALSE);
	};
//	GetWindowsDirectory(ini_file_name,256);
//   if (!SHGetSpecialFolderPath(NULL,users_directory,CSIDL_PERSONAL,TRUE)) {
   if (!get_special_folder(CSIDL_PERSONAL,users_directory)) { // rewritten on 260402
       // what to do?
   };
   strcat(users_directory,"\\ToonTalk");
//   CreateDirectory(users_directory,NULL); // new on 080402 since might be a new user (in the Windows 2000/XP sense) -- moved on 191204
   strcpy(ini_file_name,users_directory);
   strcat(ini_file_name,"\\ToonTalk.ini");
//	message_box_up = TRUE;
//   allocate_resource_strings();
//	OFSTRUCT file_struct;
	if (!has_main_dir_entry(ini_file_name)) { // prior to 170105 was OpenFile(ini_file_name,&file_struct,OF_EXIST) == -1) {
      char old_toontalk_ini[MAX_PATH];
//      SHGetSpecialFolderPath(NULL,old_toontalk_ini,CSIDL_WINDOWS,TRUE);
      get_special_folder(CSIDL_WINDOWS,old_toontalk_ini); // rewritten on 260402
      strcat(old_toontalk_ini,"\\toontalk.ini");
		if (has_main_dir_entry(old_toontalk_ini)) { // prior to 170105 was OpenFile(old_toontalk_ini,&file_struct,OF_EXIST) != -1) {
			CreateDirectory(users_directory,NULL); // moved here on 191204
         CopyFile(old_toontalk_ini,ini_file_name,FALSE);
         toontalk_installed = TRUE;
      } else {
			// new on 170105
			GetModuleFileName(NULL,old_toontalk_ini,MAX_PATH);
			int length = strlen(old_toontalk_ini);
			int i = length;
			for (; i >= 0; i--) {
				if (old_toontalk_ini[i] == '\\') {
					break;
				};
			};
			if (i > 0) {
				strcpy(old_toontalk_ini+i+1,"ToonTalk\\toontalk.ini"); // assumes the ClickMe.exe is one level above the ToonTalk executable
				if (has_main_dir_entry(old_toontalk_ini)) {
//					CreateDirectory(users_directory,NULL); // moved here on 191204
//					CopyFile(old_toontalk_ini,ini_file_name,FALSE);
					if (GetPrivateProfileInt("Switches","InstallCompleted",0,old_toontalk_ini)) {
						// new on 170105 to use the toontalk.ini on the CD-ROM or key or whatever without installing if it claims to be installed
						strcpy(ini_file_name,old_toontalk_ini); 
						toontalk_installed = TRUE;
					} else {
						toontalk_installed = FALSE;
					};
				} else {
					toontalk_installed = FALSE;
				};
			} else {
				toontalk_installed = FALSE;
			};
      };
   } else {
      toontalk_installed = TRUE;
   };
   if (toontalk_installed) {
      GetPrivateProfileString("Directories","MainDir","",
                              toontalk_directory,sizeof(toontalk_directory),
                              ini_file_name); //"ToonTalk.ini");
      GetPrivateProfileString("Directories","CDROMdir","",
                              cdrom_directory,sizeof(cdrom_directory),
                              ini_file_name); //"ToonTalk.ini");
   };
   char html_dialog_full_path[MAX_PATH] = "";
   if (toontalk_installed) { // new on 020502
      GetPrivateProfileString("Executables","HTMLClickMe","",html_dialog_full_path,sizeof(html_dialog_full_path),ini_file_name);
		if (html_dialog_full_path[0] != NULL) { // new on 040205 so allow short file names that implicitly use the module path
			if (strchr(html_dialog_full_path,':') == NULL) {
				char html_dialog_relative_path[MAX_PATH];
				strcpy(html_dialog_relative_path,html_dialog_full_path); // save it
				GetModuleFileName(NULL,html_dialog_full_path,MAX_PATH);
				int length = strlen(html_dialog_full_path);
				int i = length;
				for (; i >= 0; i--) {
					if (html_dialog_full_path[i] == '\\') {
						break;
					};
				};
				if (i > 0) {
					strcpy(html_dialog_full_path+i+1,html_dialog_relative_path);
				};
			};
		};
   };
   char not_installed_html_file[MAX_PATH];
   if (html_dialog_full_path[0] == NULL) {    
      GetModuleFileName(NULL,not_installed_html_file,MAX_PATH);

//strcpy(not_installed_html_file,"d:\\ClickMe.exe"); // for debugging 191204

      int length = strlen(not_installed_html_file);
      int i;
      for (i = length-1; i > 0; i--) {
         if (not_installed_html_file[i] == '\\' || not_installed_html_file[i] == '/') {
            memcpy(html_dialog_full_path,not_installed_html_file,i+1);
            strcat(html_dialog_full_path,clickme_html_relative_path);
            strcpy(not_installed_html_file+i+1,not_installed_html_relative_path);
//				MessageBox(NULL,html_dialog_full_path,"dialog location",MB_OK);
            break;
         };
      };
      if (i == 0) { // never found a slash - warn? -- can these work??
         strcpy(not_installed_html_file,not_installed_html_relative_path);
         strcpy(html_dialog_full_path,clickme_html_relative_path);
      };
   };
   while (TRUE) {
//		MessageBox(NULL,html_dialog_full_path,"dialog location 2",MB_OK); // for debugging 040205
      string response = show_html_dialog(html_dialog_full_path);
      if (response != NULL) {
         if (strcmp(response,CLOSED_STRING) == 0) {
            PostQuitMessage(0);
            return(0);
         } else if (strcmp(response,"-install 1") == 0) {
            first_time = FALSE;
			   LaunchApplication();
            return(0);
         } else if (strcmp(response,"-starttt 1") == 0) {
            first_time = TRUE;
            LaunchApplication();
            return(0);
         } else if (strcmp(response,"-next_dialog noinstll.htm") == 0) { // special case since should be the only one here (unlike startt)
            delete [] response;
            string ignore = show_html_dialog(not_installed_html_file);
            if (ignore != NULL) delete [] ignore;
            // show original page again
         };
      } else break;
   }; // else something went wrong to fall back on old interface
//	OFSTRUCT file_struct;
//   toontalk_ini_exists = (OpenFile(ini_file_name,&file_struct,OF_EXIST) != -1);
//	if (toontalk_ini_exists) {
//      GetPrivateProfileString("Executables","WelcomeToonTalk","",app_name,sizeof(app_name),"ToonTalk.ini");
//      GetPrivateProfileString("Executables","StartToonTalk","",start_toontalk_name,sizeof(start_toontalk_name),"ToonTalk.ini");
//      GetPrivateProfileString("Executables","ToonTalk","",toontalk_exe_name,sizeof(toontalk_exe_name),"ToonTalk.ini");
	if (!hPrevInstance) { // Will always be TRUE on Windows 95
		// Perform instance initialization:
		if (!InitApplication(hInstance)) {
			return (FALSE);
		}
	}
	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow)) {
		return (FALSE);
	}
	GetModuleFileName(hInstance, device_of_this_app, sizeof(device_of_this_app));
	hAccelTable = LoadAccelerators (hInstance, "CLICKME");
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) {
		int translated = TranslateAccelerator (msg.hwnd, hAccelTable, &msg);
		if (msg.message != WM_QUIT) {
			if (!translated) TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else break;
	}
   if (hwndBtnOk) DestroyWindow(hwndBtnOk);
   if (hwndBtnExit) DestroyWindow(hwndBtnExit);
	return (msg.wParam);

//	lpCmdLine; // This will prevent 'unused formal parameter' warnings
}


/************************************************************************\
 *    FUNCTION: InitApplication
\************************************************************************/

BOOL InitApplication(HINSTANCE hInstance)
{
	WNDCLASS  wc;
	HWND      hwnd;

	// Win32 will always set hPrevInstance to NULL, so lets check
	// things a little closer. This is because we only want a single
	// version of this app to run at a time
	hwnd = FindWindow (app_name, NULL);
	if (hwnd) {
		// We found another version of ourself. Lets defer to it:
		if (IsIconic(hwnd)) {
			ShowWindow(hwnd, SW_RESTORE);
		}
//#if TT_32
		SetForegroundWindow (GetLastActivePopup (hwnd));	
//#endif
		// If this app actually had any functionality, we would
		// also want to communicate any action that our 'twin'
		// should now perform based on how the user tried to
		// execute us.
		return FALSE;
	}

	// Fill in window class structure with parameters that describe
	// the main window.
	wc.lpszClassName = app_name;
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

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	char szTitle[64];
	
	hInst = hInstance; // Store instance handle in our global variable

	// Lets replace the default window title, with the name of the
	// actual application we want to launch...
	LoadString (hInst, IDS_WINDOW_NAME, szTitle, sizeof(szTitle));

	hWnd = CreateWindow(app_name, szTitle,
		WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX,
//		WS_OVERLAPPEDWINDOW, // maximize doesn't make sense
		0, 0,
		0, 0,
		NULL, NULL, hInstance, NULL);
//	int x = GetLastError();

	if (!hWnd) {
		return (FALSE);
	}

	// Set the small icon
//#if !defined(WM_SETICON)
//#define WM_SETICON 0x0080
//#endif
//	SendMessage(hWnd, WM_SETICON, FALSE, (LPARAM)LoadIcon(hInst, "SMALL"));

	ShowWindow(hWnd, nCmdShow);

	UpdateWindow(hWnd);
	return (TRUE);
}

/*
BOOL installation_manually_removed() {
	// toontalk.ini still exists but if exe gone...
	char toontalk_exe[256];
	strcpy(toontalk_exe,toontalk_directory);
//   strcat(toontalk_exe,start_toontalk_name);
//	strcat(toontalk_exe,".exe");
   // this is different from the executable name which is used by FindWindow
//#if TT_ENGLISH
   strcat(toontalk_exe,"starttt.exe");
//#elif TT_SWEDISH
//   strcat(toontalk_exe,"startatt.exe");
//#endif   
	OFSTRUCT file_struct;
	return(OpenFile(toontalk_exe,&file_struct,OF_EXIST) == -1);
};
*/

/************************************************************************\
 *    FUNCTION: WndProc
\************************************************************************/

HPALETTE hpal;
HBITMAP hbmAutoRun;
BITMAP bmAutoRun;
BOOL fPalette = FALSE;
BOOL fTrueColor = FALSE;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId;// wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	UINT uMappedColors; // Used in WM_QUERYNEWPALETTE
	RECT rect; // Used in WM_PAINT
	HDC  hdcSrc; // Used in WM_PAINT
	HBITMAP hbmOld; // Used in WM_PAINT


	switch (message) {

		case WM_CREATE:
			return PrepareWindow (hWnd);

		case WM_PALETTECHANGED:
			if ((HWND)wParam == hWnd) {
				break;
			}

			// fall through to WM_QUERYNEWPALETTE

		case WM_QUERYNEWPALETTE:
         if (!(fPalette || fTrueColor)) return(FALSE);
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

		case WM_COMMAND:
			wmId    = LOWORD(wParam); // Remember, these are...
//			wmEvent = HIWORD(wParam); // ...different for Win32!
			switch (wmId) {
            case IDM_INSTALL:
               first_time = FALSE;
               // and the following
				case IDM_CONTINUE: // ready to play
				case IDS_YES_INSTALL:
					if (first_time &&
						 (!toontalk_installed || toontalk_directory[0] == '\0')) { // not installed
//						  installation_manually_removed())) {
						first_time = FALSE;
						PrepareWindow (hWnd);
						break;
					};
					if (!LaunchApplication()) {
						// Failed to launch your application, you need
						// to decide what to do in this case and add code
						// to do that here. You can use GetLastError() to
						// determine the cause of this failure.
						char *message;
						if (first_time) {
							message = "Error occurred trying to start ToonTalk. Sorry.";
						} else {
							message = "Error occurred trying to find the installation program on the CDROM. Sorry.";
						};
						MessageBox(NULL,message,"Welcome",MB_ICONSTOP|MB_OK);
					};
//					PostMessage (hWnd, WM_QUIT, 0, 0);
               PostQuitMessage(0);
					break;
				case IDS_NO_INSTALL:
					// new on 270601
					if (MessageBox(NULL,"Do you want to try to run ToonTalk directly off the CD-ROM? Note that some features will not be available without installation.","Run off CD-ROM?",MB_ICONQUESTION|MB_YESNO|MB_SYSTEMMODAL) == IDYES) {
						first_time = TRUE; // a white lie
//						MessageBox(NULL,"Please insert a floppy to store your files before continuing.","Waiting for Floppy",MB_ICONSTOP|MB_OK);
//						if (CreateFile(directory_if_not_installed,0,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL) == INVALID_HANDLE_VALUE) {
//							if (MessageBox(NULL,"Do you want to store your files elsewhere?","No floppy found on the a: drive.",MB_ICONSTOP|MB_YESNO) == IDYES) {
						// following is new on 180901 to default to a: but to enable user specification of where files should be
								OPENFILENAME file_name_structure;
								ZeroMemory(&file_name_structure,sizeof(file_name_structure));
								file_name_structure.lStructSize = sizeof(file_name_structure);
								file_name_structure.lpstrFile = new char[MAX_PATH];
								file_name_structure.nMaxFile = MAX_PATH;
                        if (directory_if_not_installed[0] == NULL) { // new on 120402 to initialize it with new scheme
                           strcpy(directory_if_not_installed,users_directory);
                        };
//								strcpy(file_name_structure.lpstrFile,directory_if_not_installed); // default
								strcpy(file_name_structure.lpstrFile,"ToonTalk.ini"); 
//								file_name_structure.lpstrFile[0] = NULL;
								file_name_structure.lpstrTitle = "Where should ToonTalk store your data?";
								file_name_structure.lpstrInitialDir = directory_if_not_installed;
								if (GetSaveFileName(&file_name_structure)) {
									strcpy(directory_if_not_installed,file_name_structure.lpstrFile);
                           strcpy(ini_file_name,file_name_structure.lpstrFile);
									int length = strlen(directory_if_not_installed);
									for (int i = length-2; i > 0; i--) {
										if (directory_if_not_installed[i] == '\\') {
											directory_if_not_installed[i+1] = '\0'; // just want the directory part
											break;
										};
									};
								};
//							};
//						};
//						int x = GetLastError(); // for now
//						x = CommDlgExtendedError();
						LaunchApplication();
					};
					// and then the following
				case IDM_EXIT:
					DestroyWindow (hWnd);
					break;

//				case IDM_ABOUT:
//					DialogBox(hInst, "AboutBox", hWnd, (DLGPROC)About);
//					break;

//				case IDM_HELPCONTENTS:
//					WinHelp (hWnd, szHelpFileName, HELP_CONTENTS,(DWORD)0);
//					break;

				default:
					return (DefWindowProc(hWnd, message, wParam, lParam));
			}
			break;

		case WM_PAINT:
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
			break;        

		case WM_DESTROY:
			// Tell WinHelp we don't need it any more...
//			WinHelp (hWnd, APPNAME".HLP", HELP_QUIT,(DWORD)0);
			if (hpal !=NULL) DeleteObject (hpal);
			DeleteObject (hbmAutoRun);
			if (global_handle > 0) {
				GlobalUnlock(global_handle);
				GlobalFree(global_handle);
		   };
			PostQuitMessage(0);
			break;

		default:
			return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return (0);
}


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
	int     wScreen, hScreen, xScreen, yScreen, xNew, yNew;
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
	int     wChild, hChild, wParent, hParent;
	int     wScreen, hScreen, xScreen, yScreen, xNew, yNew;
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

int PrepareWindow(HWND hWnd)
{
	HDC hdc;
	RECT rect;
	HFONT hfontBtn;

	int i;
	LPBITMAPINFO lpbmi;

	LPVOID lpv;
	HRSRC hrsrc;
	DWORD dwSize;

	LOGPALETTE *plgpl;
	int cColors;
	LPVOID pimage;
	TEXTMETRIC tm;
	int iHeight, iWidth, x, y;

	char szString[40];

	hdc = GetDC(hWnd);

	hpal = NULL;

	fPalette = GetDeviceCaps (hdc, RASTERCAPS) & RC_PALETTE;
	fTrueColor = ((GetDeviceCaps(hdc, BITSPIXEL) * GetDeviceCaps(hdc, PLANES))>=16); //was 32

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
			hrsrc = FindResource(hInst, "WELCOME", RT_BITMAP);
		} else {
			hrsrc = FindResource(hInst, "INSTALL", RT_BITMAP);
		};
		// Lock it down. This will give us a memory pointer:
		lpv = LockResource(LoadResource(hInst,hrsrc));
		// Find out how big the resource was:
		dwSize = SizeofResource(hInst,hrsrc);
		if (global_handle > 0) {
			GlobalUnlock(global_handle);
			GlobalFree(global_handle);
		};
		// Allocate enough memory:
		global_handle = GlobalAlloc (GHND , dwSize);
		lpbmi = (LPBITMAPINFO) GlobalLock(global_handle); // was LocalAlloc in Win32
		if (!lpbmi) return -1; // failure
		// And make our own private copy of the data:
//#if TT_32      
		memcpy ((void *) lpbmi, (void *) lpv, dwSize);
//#else
//		hmemcpy ((void *) lpbmi, (void *) lpv, dwSize);
//#endif
		// How many colors does it use?
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
		plgpl->palNumEntries = cColors;
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
		if (first_time) {
			hbmAutoRun = LoadBitmap (hInst, "WELCOME16");
		} else {
			hbmAutoRun = LoadBitmap (hInst, "INSTALL16");
		};
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
	hfontBtn = (HFONT) GetStockObject (ANSI_VAR_FONT);
	SelectObject (hdc, hfontBtn);
	GetTextMetrics (hdc, &tm);

	// Ok, our image is loaded, and our window properly sized
	// Lets add in some controls: The text to use for these
	// buttons is coming out of the resource of this app
	int button_id = first_time?IDM_CONTINUE:IDS_YES_INSTALL;
	LoadString (hInst, button_id, szString, sizeof(szString));
   if (hwndBtnOk) DestroyWindow(hwndBtnOk);
	hwndBtnOk = CreateWindow ("BUTTON", szString,
		                       WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
		                       0, 0, 0, 0,
		                       hWnd, (HMENU)button_id, hInst, NULL);

	button_id = first_time?IDM_EXIT:IDS_NO_INSTALL;
	LoadString (hInst, button_id, szString, sizeof(szString));
   if (hwndBtnExit) DestroyWindow(hwndBtnExit);
	hwndBtnExit = CreateWindow ("BUTTON", szString,
		                         WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | BS_PUSHBUTTON,
		                         0, 0, 0, 0,
		                         hWnd, (HMENU)button_id, hInst, NULL);

   LoadString (hInst, IDM_INSTALL, szString, sizeof(szString));
   if (first_time) {
	   hwndBtnInstall = CreateWindow ("BUTTON", szString,
		                               WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | BS_PUSHBUTTON,
		                               0, 0, 0, 0,
		                               hWnd, (HMENU)IDM_INSTALL, hInst, NULL);
   } else {
      DestroyWindow(hwndBtnInstall);
   };
	hfontBtn = (HFONT) GetStockObject (ANSI_VAR_FONT);
	SendMessage (hwndBtnOk, WM_SETFONT, (WPARAM)hfontBtn,(LPARAM)TRUE);
	SendMessage (hwndBtnExit, WM_SETFONT, (WPARAM)hfontBtn, (LPARAM)TRUE);
   SendMessage (hwndBtnInstall, WM_SETFONT, (WPARAM)hfontBtn, (LPARAM)TRUE);
	SelectObject (hdc, hfontBtn);
	GetTextMetrics (hdc, &tm);

   // set this so it fits even if user has set large font for Windows
	iHeight = 26; //(tm.tmHeight + tm.tmExternalLeading) * 2;
//	iWidth = (bmAutoRun.bmWidth / 2) - (iHeight * 2);

   if (first_time) {
      iWidth = (3*bmAutoRun.bmWidth) / 10; //  - (iHeight * 3);
   } else {
      iWidth = (bmAutoRun.bmWidth / 2) - (iHeight * 2);
   };
			
	x = ((bmAutoRun.bmWidth/2) / 2) - (iWidth/2);
//	y = bmAutoRun.bmHeight - iHeight - (iHeight/2);
	y = bmAutoRun.bmHeight - 5*iHeight/4;

   if (first_time) {
      x -= bmAutoRun.bmWidth/10;
   };
	SetWindowPos (hwndBtnExit, NULL, x, y, iWidth, iHeight,
		SWP_NOZORDER | SWP_NOACTIVATE);
   if (first_time) {
      x += (7*bmAutoRun.bmWidth/20);
   } else {
	   x += bmAutoRun.bmWidth/2;
   };
	SetWindowPos (hwndBtnOk, NULL, x, y, iWidth, iHeight,
		           SWP_NOZORDER | SWP_NOACTIVATE);

   if (first_time) {
      x += (7*bmAutoRun.bmWidth/20);
      SetWindowPos (hwndBtnInstall, NULL, x, y, iWidth, iHeight,
		              SWP_NOZORDER | SWP_NOACTIVATE);
   };

	ShowWindow (hWnd, SW_SHOW);

	ReleaseDC (hWnd, hdc);

	return 0; // Sucess
	
}


/************************************************************************\
 *    FUNCTION: 
\************************************************************************/
// This function is called to launch the 'real' application when the
// user instructs us to. The name of the application to be run is coming
// out of the resource of this app.

BOOL launched_already = FALSE;
BOOL setup_launched_already = FALSE;

#include <shellapi.h>
BOOL LaunchApplication() {
//	char szCompany[40];
	char szImageName[MAX_PATH];
//	char szCmdLine[40];

//	STARTUPINFO         si;
//	PROCESS_INFORMATION pi;

//	int directory_length;

//	LoadString (hInst, ID_COMPANY, szCompany, sizeof(szCompany));
	char full_command_line[512]; // new on 260601
	if (first_time) {
		if (launched_already) return(TRUE); // no-op
      char start_toontalk_name[64];
      if (GetPrivateProfileString("Executables","StartToonTalk","",
										    start_toontalk_name,sizeof(start_toontalk_name),
										    ini_file_name) == 0) {
         strcpy(start_toontalk_name,"StartTT");   // default
      };
		strcpy(szImageName,toontalk_directory);
      strcat(szImageName,start_toontalk_name);
		strcat(szImageName, ".EXE");
      OFSTRUCT file_struct;
      if (OpenFile(szImageName,&file_struct,OF_EXIST) == -1) { // doesn't exist so run off CDROM
//			char ini_file_name[MAX_PATH];
//			strcat(ini_file_name,"\\toontalk.ini"); // added \\ on 110402
			if (cdrom_directory[0] == NULL) { // new on 270601
//				GetCurrentDirectory(MAX_PATH,cdrom_directory);
//				strcat(cdrom_directory,"\\toontalk\\");
				char exe_file_name[MAX_PATH];
				GetModuleFileName(NULL,exe_file_name,MAX_PATH);

//strcpy(exe_file_name,"d:\\ClickMe.exe"); // for debugging 191204

				// remove all but directory path and then replace with toontalk.ini
				if (exe_file_name != NULL) {
					int length = strlen(exe_file_name);
					for (int i = length-1; i > 0; i--) {
						if (exe_file_name[i] == '\\' || exe_file_name[i] == ':' || exe_file_name[i] == '/') {
							strcpy(exe_file_name+i+1,"toontalk\\toontalk.ini");
							memcpy(cdrom_directory,exe_file_name,i+1); // may have been copied so not really a CD-ROM anymore
							memcpy(cdrom_directory+i+1,"toontalk\\",10);
							break;
						};
					};
				} else { // shouldn't really happen
					strcpy(exe_file_name,"d:\\toontalk\\toontalk.ini");
					strcpy(cdrom_directory,"d:\\toontalk\\");
				};
            // calling the ini file an exe_file_name is confusing here - rationalize someday
				// commented out on 191204
				//if (GetFileAttributes(exe_file_name)&FILE_ATTRIBUTE_READONLY) { // then copy it somewhere and continue
				//	if (CopyFile(exe_file_name,ini_file_name,TRUE)) { // or ask where to put it?
				//		 SetFileAttributes(ini_file_name,FILE_ATTRIBUTE_NORMAL);
				//		 WritePrivateProfileString("Directories","MainDir",cdrom_directory,ini_file_name);						 
  		//			};
				//};
			};
         strcpy(szImageName,cdrom_directory);
         strcat(szImageName,start_toontalk_name);
		   strcat(szImageName, ".EXE");
			if (OpenFile(ini_file_name,&file_struct,OF_EXIST) != -1) {
				strcpy(full_command_line,"-ini_file ");
				strcat(full_command_line,ini_file_name);
				strcat(full_command_line," ");
			} else {
				// new on 191204 that if ini file doesn't exist then don't tell StartTT about it
				strcpy(full_command_line," ");
			};
      } else {
			full_command_line[0] = '\0'; // new on 270601 so strcat below works fine
		};
		launched_already = TRUE;
#if TT_ENGLISH
//      char *command_line = ""; //-language American";
#elif TT_SWEDISH
//      char *command_line = "-language Swedish";
		strcat(full_command_line,"-language Swedish"); // new version on 260601
#elif TT_PORTUGUESE
//      char *command_line = "-language BR";
		strcat(full_command_line,"-language BR");
#elif TT_EUROPEAN_PORTUGUESE
//		char *command_line = "-language PT";
		strcat(full_command_line,"-language PT");
#elif TT_JAPANESE
      strcat(full_command_line,"-language JP");	// new on 101002
#endif
//		LoadString (hInst, ID_CMDLINE, szCmdLine, sizeof(szCmdLine));
#if RUNNING_TT16
		return((int) ShellExecute(NULL,"open",szImageName,full_command_line,"",SW_SHOW) > 32);
#else
		si.cb = sizeof(STARTUPINFO);
		si.lpReserved =  NULL;
		si.lpDesktop =  NULL;
		si.lpTitle	= NULL;
		si.dwX	=  0;
		si.dwY = 0;
		si.dwXSize	= 0;
		si.dwYSize = 0;
		si.dwXCountChars = 0;
		si.dwYCountChars = 0;
		si.dwFillAttribute =   0;
		si.dwFlags = 0;
		si.wShowWindow = 0;
		si.cbReserved2 = 0;
		si.lpReserved2 = NULL;
		si.hStdInput =  NULL;
		si.hStdOutput = NULL;
		si.hStdError =	NULL;
		return CreateProcess (szImageName, szCmdLine, NULL, NULL, FALSE,
			                   CREATE_NEW_PROCESS_GROUP, NULL, toontalk_directory, &si, &pi);
#endif
	} else {
      if (setup_launched_already) return(TRUE);
		setup_launched_already = TRUE;
		char setup_path[MAX_PATH];
		GetModuleFileName(NULL, device_of_this_app, sizeof(device_of_this_app)); // missing prior to 191204
		strcpy(setup_path,device_of_this_app);
		strcat(setup_path,"setup.exe");
		return((int) ShellExecute(NULL, "open", setup_path, "", "", SW_SHOW) > 32); // was >= prior to 250402 (changed everywhere)
	}
};

