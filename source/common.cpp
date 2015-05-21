// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.

// procedures used in common between toontalk and startTT -- extracted on 290502
#include "stdafx.h"
//#include <forcelib.h> // recommended by http://support.microsoft.com/default.aspx?scid=KB;EN-US;q148652&

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif
#if !defined(__TT_COMMON_H) 
#include "common.h"
#endif

//#if !defined(__TT_UTILS_H) 
//#include "utils.h"
//#endif

#include "common.h"
//#include <windows.h>
//#include "starttt.h"
#include <shlobj.h> // new on 080402 to find correct directories to put things
#include <shfolder.h> // new on 260402 to find correct directories to put things

//added 300402 for the HTML dialog
#include <urlmon.h>
#include <mshtmhst.h>

#if !defined(TT_DEBUG_STARTTT) // not worrying about leaks in StartTT just now
#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

HMODULE hModSHFolder = NULL; // no need to release this since is released on exit

// copied from shlobj.h on 281002
typedef enum {
    SHGFP_TYPE_CURRENT  = 0,   // current value for user, verify it exists
    SHGFP_TYPE_DEFAULT  = 1,   // default value, may not exist
} SHGFP_TYPE;

boolean get_special_folder(int CSIDL, string path) { // new on 260402
   // based upon http://msdn.microsoft.com/library/default.asp?url=/library/en-us/apcompat/appcomp_0dwz.asp
   // and Microsoft's Q241733
   // SHGetFolderPath can work everywhere SHFOLDER is installed.
   if (hModSHFolder == NULL) {
      hModSHFolder = LoadLibrary("SHFolder.dll");
   };
   if ( hModSHFolder != NULL ) {
      PFNSHGETFOLDERPATHA pSHGetFolderPath = (PFNSHGETFOLDERPATHA)GetProcAddress(hModSHFolder,"SHGetFolderPathA"); 
		if (pSHGetFolderPath != NULL) {
			if (pSHGetFolderPath(NULL, CSIDL|CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, path) == S_OK) {
				int path_length = strlen(path);
				if (path_length > 0 && path[path_length-1] == '\\') {
					// new on 201005 in case get_my_documents_folder returned something like n: followed by a slash
					path[path_length-1] = 0;
				};
				return(TRUE);
			} else {
				return(FALSE);
			};
		};
	};
   return(FALSE);
};

// following added on 300402 - based upon MS documentation associated with ShowHTMLDialog

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

boolean local_file_exists(string name) { // doesn't work for URLs (though it could by copying stuff from file_exists(...)
   HANDLE handle = CreateFile(name,0,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (handle == INVALID_HANDLE_VALUE) {
		return(FALSE);
	} else {
		CloseHandle(handle);
		return(TRUE);
	};
};

string show_html_dialog(string html_file_name, HWND window_handle, string to_dialog, int dialog_width, int dialog_height, int dialog_left, int dialog_top) {
   if (!local_file_exists(html_file_name)) {
#if TT_DEBUG_ON
      tt_error_file() << html_file_name << " not found. Using old dialogs instead." << endl;
#endif
      return(NULL); // new on 300502
   };
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
   OLECHAR  bstr[MAX_PATH*2]; // warning: this assume will be fed local path names not URLs longer than MAX_PATH
   LocalToBSTR(bstr, html_file_name, ARRAYSIZE(bstr));
   if (FAILED(CreateURLMoniker(NULL, bstr, &pmk))) {
      return(NULL); // warn? -- new on 221002
   };
   if (pmk != NULL) {
      HRESULT  hr;
      VARIANT  varArgs, varReturn;
      VariantInit(&varReturn);
      varArgs.vt = VT_BSTR; // new on 290502
      wide_string to_dialog_in_unicode = copy_wide_string_with_code_page(to_dialog);
      varArgs.bstrVal = SysAllocString(to_dialog_in_unicode);
      const int parameters_size = 1024;
      char parameters[parameters_size];
      // updated to use size of real window
      HDC device_context = GetDC(NULL);
      if (dialog_width <= 0) {
         dialog_width = GetDeviceCaps(device_context,HORZRES);
      };
      if (dialog_height <= 0) {
         dialog_height = GetDeviceCaps(device_context,VERTRES);
      };
      lstrcpy(parameters,TEXT("dialogWidth: "));
      itoa(dialog_width,parameters+strlen(parameters),10);
      lstrcat(parameters,TEXT("px"));
      lstrcat(parameters,TEXT("; dialogHeight: "));
      itoa(dialog_height,parameters+strlen(parameters),10);
      lstrcat(parameters,TEXT("px"));
      //if (dialog_left < 0 || dialog_top < 0) {
      //   lstrcat(parameters,TEXT("; center: Yes")); // this worked fine for weeks and then started causing strange locations
      //} else {
         lstrcat(parameters,TEXT("; dialogLeft: "));
         itoa(dialog_left,parameters+strlen(parameters),10);
         lstrcat(parameters,TEXT("px"));
         lstrcat(parameters,TEXT("; dialogTop: "));
         itoa(dialog_top,parameters+strlen(parameters),10);
         lstrcat(parameters,TEXT("px"));
      //};
      lstrcat(parameters,TEXT("; help: No;"));
      OLECHAR  bparameters[parameters_size*2]; // new on 200602 to stop reusing bstr
      LocalToBSTR(bparameters, parameters, ARRAYSIZE(bparameters));
      // don't understand why when compiling with VC 6.0 it wanted (char *) bparameters
      hr = (*pfnShowHTMLDialog)(window_handle, pmk, &varArgs, bparameters, &varReturn); // added window_handle on 200602
      VariantClear(&varArgs);
      pmk->Release();
      if (to_dialog_in_unicode != NULL) delete [] to_dialog_in_unicode;
      if (SUCCEEDED(hr)) {
         const int max_result_size = 10240; // added an extra 0 on 141002 just in case
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
#if TT_DEBUG_ON
		} else {
			int x = GetLastError(); // new on 150604 to debug this
			x = x+0; // for debugging
#endif
		};
   };
   return(NULL);
};

boolean dialog_closed(string from_dialog) {
   return(from_dialog == NULL || strcmp(from_dialog,CLOSED_STRING) == 0); // added from_dialog == NULL on 140604
};

wide_string copy_wide_string(wide_string source, int length) {
	if (source == NULL && length != 0) { // second condition new on 220104 since for example dropping a zero-length text pad on an erased pad expects non-NULL if can proceed
		return(NULL);
	};
   if (length < 0) length = wide_string_length(source);
   wide_string destination = new wide_character[length+1];
   memcpy(destination,source,length*sizeof(wide_character));
   destination[length] = NULL; // terminate the string
   return(destination);
};

wide_string copy_wide_string_with_code_page(string source, int source_length, UINT code_page) { // code_page added as arg on 310502
	if (source == NULL) return(NULL);	
//	if (source_length < 0) {
//		source_length = strlen(source);
//	};
	long destination_length = MultiByteToWideChar(code_page,0,source,source_length,NULL,0);
   wide_string destination = new wide_character[destination_length+1]; 
   MultiByteToWideChar(code_page,0,source,source_length,destination,destination_length+1);
	destination[destination_length] = NULL;
//#if TT_DEBUG_ON
//   tt_error_file() << "Length of " << source << " is " << destination_length << endl;
//#endif
	return(destination);
};

void copy_wide_string_with_code_page(string source, int source_length, wide_string destination, UINT code_page) { // new on 170102
	if (source == NULL) return;	
	long destination_length = MultiByteToWideChar(code_page,0,source,source_length,NULL,0);
   MultiByteToWideChar(code_page,0,source,source_length,destination,destination_length+1);
	destination[destination_length] = NULL;
};