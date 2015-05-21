// from http://www.codeproject.com/debug/postmortemdebug_standalone1.asp?print=true

#include "stdafx.h"

//#include <forcelib.h> // recommended by http://support.microsoft.com/default.aspx?scid=KB;EN-US;q148652&
//#include <Shellapi.h> // for ShellExecute

#include "mdump.h"

#include <stdio.h> // for sprintf

extern char tt_toontalk_crash_directory[MAX_PATH];
extern char tt_application_name[];
extern char * tt_ok_to_generate_mini_dump_message;
extern char * tt_mini_dump_email;
extern BOOL tt_show_mini_dump_directory;
extern BOOL tt_full_memory_dump; // new on 031202

void pause_toontalk(boolean interrupted, boolean ask, boolean shutting_down);

LPCSTR MiniDumper::m_szAppName;

MiniDumper mini_dumper(tt_application_name);

MiniDumper::MiniDumper( LPCSTR szAppName )
{
	// if this assert fires then you have two instances of MiniDumper
	// which is not allowed
//	assert( m_szAppName==NULL );
	OSVERSIONINFO version_info;
	version_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&version_info);
	if (version_info.dwMajorVersion >= 5) { // condition new on 150404 since crash dumps don't seem to work in Win 98 or Me
		m_szAppName = szAppName ? strdup(szAppName) : "Application";
		::SetUnhandledExceptionFilter( TopLevelFilter );
	};
}

LONG MiniDumper::TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo )
{
	LONG retval = EXCEPTION_CONTINUE_SEARCH;
	HWND hParent = NULL;						// find a better value for your app

	// firstly see if dbghelp.dll is around and has the function we need
	// look next to the EXE first, as the one in System32 might be old 
	// (e.g. Windows 2000)
	HMODULE hDll = NULL;
	char szDbgHelpPath[_MAX_PATH];

	if (GetModuleFileName( NULL, szDbgHelpPath, _MAX_PATH ))
	{
		char *pSlash = _tcsrchr( szDbgHelpPath, '\\' );
		if (pSlash)
		{
			_tcscpy( pSlash+1, "DBGHELP.DLL" );
			hDll = ::LoadLibrary( szDbgHelpPath );
		}
	}

	if (hDll==NULL)
	{
		// load any version we can
		hDll = ::LoadLibrary( "DBGHELP.DLL" );
	}

	// LPCTSTR szResult = NULL;
	char *szResult = NULL; // rewritten on 150505

	if (hDll)
	{
		MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
		if (pDump)
		{
			char szDumpPath[_MAX_PATH];
			char szScratch [_MAX_PATH];

			// work out a good place for the dump file
         if (tt_toontalk_crash_directory[0] != '\0') {
            _tcscpy(szDumpPath, tt_toontalk_crash_directory);
         } else if (GetTempPath( _MAX_PATH, szDumpPath )) {
            strcpy(tt_toontalk_crash_directory, szDumpPath);
         } else {
				_tcscpy( szDumpPath, "c:\\temp\\" );
            strcpy(tt_toontalk_crash_directory, szDumpPath);
         };
			_tcscat( szDumpPath, tt_application_name); // changed on 260803 m_szAppName );
			_tcscat( szDumpPath, ".dmp" );

         pause_toontalk(FALSE,FALSE,TRUE);

			// ask the user if they want to save a dump file
			if (tt_ok_to_generate_mini_dump_message != NULL &&
             ::MessageBox( NULL, tt_ok_to_generate_mini_dump_message, m_szAppName, MB_YESNO )==IDYES)
			{
				// create the file
				HANDLE hFile = ::CreateFile( szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
											       FILE_ATTRIBUTE_NORMAL, NULL );

				if (hFile!=INVALID_HANDLE_VALUE)
				{
					_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

					ExInfo.ThreadId = ::GetCurrentThreadId();
					ExInfo.ExceptionPointers = pExceptionInfo;
					ExInfo.ClientPointers = NULL;

					// write the dump
					// added MiniDumpWithFullMemory option on 031202
					// add more switches to capture the stack - noted on 090203
					BOOL bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, tt_full_memory_dump?MiniDumpWithFullMemory:MiniDumpNormal,
										  &ExInfo, NULL, NULL );
					if (bOK)
					{
//						sprintf( szScratch, "The diagnostic file was saved as '%s'. Please email it as an attachment to support@toontalk.com.",
									//szDumpPath );
                  if (tt_mini_dump_email != NULL) {
                     ShellExecute(NULL,"open",tt_mini_dump_email,"","",SW_SHOW);
                  };
                  if (tt_show_mini_dump_directory) {
                     ShellExecute(NULL,"open",tt_toontalk_crash_directory,"","",SW_SHOW); // show directory
                  };
//						szResult = szScratch;
                  szResult = NULL;
						retval = EXCEPTION_EXECUTE_HANDLER;
					}
					else
					{
						sprintf( szScratch, "Failed to save dump file to '%s' (error %d)", szDumpPath, GetLastError() );
						szResult = szScratch;
					}
					::CloseHandle(hFile);
				}
				else
				{
					sprintf( szScratch, "Failed to create dump file '%s' (error %d)", szDumpPath, GetLastError() );
					szResult = szScratch;
				}
			}
		}
		else
		{
//			szResult = "DBGHELP.DLL too old";
			// rewritten on 150505
			szResult = new char[1024]; // harmless leak since exiting now anyway
			strcpy(szResult,"An error occurred trying to create a crash report. The system file DBGHELP.DLL seems to be too old. The error code is ");
			itoa(GetLastError(),szResult+strlen(szResult),10);
		}
	}
	else
	{
		szResult = "DBGHELP.DLL not found";
	}

	if (szResult)
		::MessageBox( NULL, szResult, m_szAppName, MB_OK );

	return retval;
}
