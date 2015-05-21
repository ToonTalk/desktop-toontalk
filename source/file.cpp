// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)
#include "defs.h"
#endif
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif  
#if !defined(__TT_STRINGS_H)
#include "strings.h"
#endif  
#if !defined(__TT_UTILS_H)
#include "utils.h"
#endif  
#if !defined(__TT_WINMAIN_H)
#include "winmain.h"
#endif
#if !defined(__TT_FILE_H)
#include "file.h"
#endif

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This file is so I can interchangably use handles from OpenFile and InternetOpenUrl

File::~File() { // new on 240601
};

LocalFile::LocalFile(LPCSTR file_name, boolean warn) { // warn arg new on 101004
	if (strnicmp(file_name,"file://",7) == 0) {
		file_name += 7; // nwe on 210201 to skip over the URL part
		if (file_name[0] == '/') file_name++; // don't know why there is often an extra slash in the name
	};
	hFile = ::CreateFile(file_name,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile == INVALID_HANDLE_VALUE && warn) {
		// new on 101004
		unable_to_read_file((string) file_name);
	};
//  hFile = OpenFile(lpFileName,lpReOpenBuff,uStyle);
}; 

BOOL LocalFile::Read(IN LPVOID lpBuffer, IN DWORD dwNumberOfBytesToRead,
							OUT LPDWORD lpNumberOfBytesRead) {
	return(::ReadFile(hFile,lpBuffer,dwNumberOfBytesToRead,lpNumberOfBytesRead,NULL));
};

BOOL LocalFile::Close() {
	return(CloseHandle(hFile));
};

LocalFile::~LocalFile() { // new on 240601
	Close();
};

flag explained_internet_error = FALSE;

#if TT_MICROSOFT
URLFile::URLFile(IN LPCSTR lpszUrl, boolean warn) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 20105) {
		tt_error_file() << lpszUrl << " is a URL about to be opened." << endl;
	};
#endif
	if (tt_internet_connection == NULL) {
		hFile = NULL;
		return;
	};
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	loading_wait(TRUE); // new on 150101
	hFile = InternetOpenUrl(tt_internet_connection,lpszUrl,NULL,0,INTERNET_FLAG_EXISTING_CONNECT,0);
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	if (hFile == NULL) {
		done_waiting_for_load();
		if (!explained_internet_error) { // new on 130999
			const int error_message_max_size = 1000; // who knows?
			DWORD error_number;
			string error_message = new char[error_message_max_size+1];
			DWORD error_message_size = error_message_max_size;
			if (!InternetGetLastResponseInfo(&error_number,error_message,&error_message_size)) {
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) { // new on 110101
					delete [] error_message;
					error_message = new char[error_message_size+1];
					InternetGetLastResponseInfo(&error_number,error_message,&error_message_size);
				};
			};
			if (error_message_size == 0) {
				strcpy(error_message,S(IDS_UNKNOWN_PROBLEM_READING_A_URL));
			};
			boolean ignore_error = (strstr(error_message,"550") != NULL);
			// added this test on 110100 since I get this error when just file is missing (and there is such a directory)
			if (!ignore_error &&
				 message_box(MB_YESNO|MB_SYSTEMMODAL,error_message,S(IDS_SHOULD_TOONTALK_STOP_READING_URLS)) == IDYES) { 
				InternetCloseHandle(tt_internet_connection);
				tt_internet_connection = NULL;
			};
			delete [] error_message;
			if (!ignore_error) {
				explained_internet_error = TRUE;
			};
		};
	} else {
		Sleep(0); // new on 150101 so that if loading lots of URLs that the mouse and the like doesn't freeze
	};
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
};

URLFile::~URLFile() { // new on 020301
	Close();
};

BOOL URLFile::Read(IN LPVOID lpBuffer, IN DWORD dwNumberOfBytesToRead,
						 OUT LPDWORD lpNumberOfBytesRead) {
	if (Empty()) { // new on 160301 for robustness
		lpNumberOfBytesRead = 0;
		return(FALSE);
	};
	return(InternetReadFile(hFile,lpBuffer,dwNumberOfBytesToRead,lpNumberOfBytesRead));
};

BOOL URLFile::Empty() {
   return (hFile == NULL);
};

BOOL URLFile::Close() { // new on 150101
	if (Empty()) { // new on 160301 for robustness
		return(TRUE); // closed the non-existent file just fine
	};
	BOOL result = InternetCloseHandle(hFile);
	hFile = NULL;
	done_waiting_for_load();
#if TT_DEBUG_ON
	// probably no need to warn - this seems to happen sometimes - clearing the error for debugging purposes
//	if (!result) { // new on 160301
		add_to_log_if_interesting_error(6); // add error number
//	};
#endif 
	return(result);
};

#endif
//	DWORD bytes_available = 0;
//	if (InternetQueryDataAvailable(hFile,&bytes_available,0,0)) {
//		return(bytes_available == 0);
//	} else {
//		return(TRUE);
//	};

File *open_file_or_url(string url, boolean ok_to_cache, boolean warn) {
	if (url == NULL) return(NULL);
#if TT_DEBUG_ON
   add_to_log_if_interesting_error(min_long,"open_file_or_url");
#endif
	string new_file_name = extract_file_name_from_link(url);
	if (new_file_name == NULL) return(NULL);
	// Only URLs beginning with ftp:, gopher:, http:, or https: are supported.
	File *answer = NULL;
	if (is_url(new_file_name)) {
#if TT_MICROSOFT
		string cached_name = NULL;
		if (ok_to_cache) {
			cached_name = cached_file_name(new_file_name);
		};
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		if (cached_name == NULL) {
			answer = new URLFile(new_file_name,warn);
		} else { // new on 200201
			answer = new LocalFile(cached_name,warn);
			delete [] cached_name;
		};
#endif
	} else {
		answer = new LocalFile(new_file_name);
	};
	delete [] new_file_name;
	return(answer);
};

boolean is_404_error(string buffer, string source_file_name) { // made a procedure on 070201
	const int html_extension_count = 2;
	string html_extensions[html_extension_count] = {"htm","html"};
	return(strstr(buffer,"404 Not Found") != NULL || // updated on 020600
			  (!already_has_extension(source_file_name,html_extensions,html_extension_count) && 
				strstr(buffer,"<html>") != NULL));
};	

typedef File *FilePointer;

boolean copy_file_or_url_to_file_start(string source, DWORD &bytes_read, FilePointer &source_file) { // abstracted on 180303
	source_file = open_file_or_url(source,FALSE); // FALSE added on 020301 to prevent infinite recursion when used in caching
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	boolean empty = source_file->Empty();
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	if (!empty) {
		source_file->Read(shared_buffer,shared_buffer_size,&bytes_read);
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		if (bytes_read < shared_buffer_size) { // assumes that error message will never be longer than shared_buffer_size
			if (is_404_error(shared_buffer,source)) {
				empty = TRUE; // no file just an error from server
				source_file->Close(); // not clear if needed
#if TT_DEBUG_ON
				add_to_log_if_interesting_error(6);
#endif
			};
		};
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	if (empty) {
//		CloseHandle(destination_handle);
//		DeleteFile(destination);
		return(FALSE);
	};
	return(TRUE); // so far so good
};

boolean copy_file_or_url_to_file(string source, string destination, boolean &need_to_commit) {
	// if source (and destination?) is not a URL why not using ::CopyFile instead?? -asked 181204
	if (destination == NULL) return(FALSE); // new on 010600
	if (!is_url(source)) { // new on 060205
		if (CopyFile(source,destination,FALSE)) {
			return(TRUE);
		}; // otherwise run old code -- just to be sure
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 50401) {
		tt_error_file() << "Copying from " << source << " to " << destination << endl;
	};
	add_to_log_if_interesting_error();
#endif
	DWORD bytes_read;
	File *source_file = NULL;
	if (!copy_file_or_url_to_file_start(source,bytes_read,source_file)) {
		return(FALSE);
	};
	// moved here on 050401 since no point if source is empty or can't be reached
	HANDLE destination_handle = CreateFile(destination,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if (destination_handle == INVALID_HANDLE_VALUE) {
		need_to_commit = FALSE;
//		return(GetLastError() == ERROR_FILE_EXISTS); // should be OK to use it again
		return(FALSE);
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	need_to_commit = TRUE;
	DWORD bytes_written;
	tt_bytes_read_from_URL = 0;
#if TT_LOADING_AND_SAVING_INTERRUPTABLE
	UnwindProtect<boolean> set(tt_loading_is_ok_to_abort,TRUE); // new on 290105
#endif
	while (bytes_read > 0) {
#if TT_LOADING_AND_SAVING_INTERRUPTABLE
		loading_wait(FALSE); // new on 290105
		if (user_wants_to_interrupt_also_process_messages_now(FALSE)) { // new on 290105
			if (!ask_response_to_copy_url_interruption(tt_bytes_read_from_URL)) { // aborted
				delete source_file;
				CloseHandle(destination_handle);
				tt_bytes_read_from_URL = 0;
				return(FALSE);
			};
		};
		tt_bytes_read_from_URL += bytes_read;
#endif
		WriteFile(destination_handle,shared_buffer,bytes_read,&bytes_written,NULL);
		source_file->Read(shared_buffer,shared_buffer_size,&bytes_read);
		// could check bytes_written to see if all is well
	};
//	source_file->Close(); // commented out on 071201 since deleting will Close it
#if TT_LOADING_AND_SAVING_INTERRUPTABLE
	tt_bytes_read_from_URL = 0;
#endif
	delete source_file; // new on 150101
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	CloseHandle(destination_handle);
	return(TRUE);
};

string copy_file_or_url_to_string(string source, long &file_size, boolean warn) {
	File *source_file = open_file_or_url(source,FALSE,warn); // FALSE new on 200201 so this doesn't cache the file
	if (source_file->Empty()) {
		file_size = 0;
		delete source_file; // new on 240601
		return(NULL);
	};
   file_size = source_file->Size();
	if (file_size < 0) {
		delete source_file; // new on 240601
		return(NULL); // was <= prior to 200201
	};
	string result;
	if (file_size > 0) {
		result = new char[file_size+1];
		DWORD bytes_read;
		source_file->Read(result,file_size,&bytes_read);
		result[file_size] = '\0'; // terminate string
	} else { // new on 200201 since you can't always know the size
		DWORD buffer_size = 1000; // OK to start with - will double as needed
		int allocated_buffer_size = buffer_size+1; // new on 040205 to not recopy when not reading large chunks anymore
		result = new char[allocated_buffer_size]; 
		file_size = 0;
		DWORD bytes_read;
		string buffer = result;
		tt_bytes_read_from_URL = 0;
#if TT_LOADING_AND_SAVING_INTERRUPTABLE
		UnwindProtect<boolean> set(tt_loading_is_ok_to_abort,TRUE);
#endif
		while (TRUE) {
#if TT_LOADING_AND_SAVING_INTERRUPTABLE
			loading_wait(FALSE); // new on 290105
			tt_bytes_read_from_URL = file_size;
			if (user_wants_to_interrupt_also_process_messages_now(FALSE)) { // new on 150105
				if (!ask_response_to_copy_url_interruption(file_size)) { // aborted
					delete source_file;
					delete [] result;
					tt_bytes_read_from_URL = 0;
					return(NULL);
				};
			};
#endif
			source_file->Read(buffer,buffer_size,&bytes_read);
			if (bytes_read == 0) { // experiment on 040205 was < buffer_size) { // got to the end
				file_size += bytes_read+1;
				result[file_size-1] = '\0'; // terminate string
				break;
			} else { // double buffer size and try again
//				string new_result = new char[buffer_size*3]; 
//				memcpy(new_result,buffer,buffer_size);
				// fixed a few bugs here on 020105
//				loading_wait(FALSE); // new on 250105 -- done earlier as of 290105
				file_size += bytes_read;
				int new_buffer_size;
				int next_total_buffer_size;
				if (bytes_read == buffer_size) {
					new_buffer_size = buffer_size*2; // double the buffer
					next_total_buffer_size = file_size+new_buffer_size+1;
				} else {
					new_buffer_size = buffer_size; // new on 040205 -- reached some limit on chunk size
					if (allocated_buffer_size < file_size+new_buffer_size+1) {
						next_total_buffer_size = file_size+max(file_size,new_buffer_size)+1;
					} else {
						next_total_buffer_size = allocated_buffer_size; // already is enough room
					};
				};
				if (next_total_buffer_size > allocated_buffer_size) {
					// branch new since don't need to reallocate otherwise
					string new_result = new char[next_total_buffer_size];
					allocated_buffer_size = next_total_buffer_size;
					memcpy(new_result,result,file_size);
					buffer = new_result+file_size; // fixed on 020105 was +buffer_size;
					delete [] result; // new on 020105 to fix a leak here
					result = new_result;
				} else {
					buffer += bytes_read;
				};
				buffer_size = new_buffer_size;		
			};
		};
	};
#if TT_LOADING_AND_SAVING_INTERRUPTABLE
	tt_bytes_read_from_URL = 0;
#endif
//	source_file->Close(); // replaced with the following which closes as well
	delete source_file; // new on 240601
	return(result);
};

#if TT_MICROSOFT
string system_cached_file_name(string url) { // , string extension) { - this was eliminated on 300500 since it was a misreading of Windows documentation
	const int allocated_buffer_size = 1000; // who knows?? -- why isn't Microsoft documentation better here
	DWORD buffer_size = allocated_buffer_size; 
	char buffer[allocated_buffer_size];
	LPINTERNET_CACHE_ENTRY_INFO cache_entry_info = (LPINTERNET_CACHE_ENTRY_INFO) buffer;
	memset(buffer,0,allocated_buffer_size); // new on 070401
	cache_entry_info->dwStructSize = buffer_size;
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	LPFILETIME lpdtmExpire = new FILETIME;
   LPSYSTEMTIME lpdtmSysTime = new SYSTEMTIME;
	GetLocalTime(lpdtmSysTime);
   SystemTimeToFileTime(lpdtmSysTime,lpdtmExpire);
	LARGE_INTEGER *expires = new LARGE_INTEGER; // was LARGE_INTEGER() prior to 040803
	expires->LowPart = lpdtmExpire->dwLowDateTime;
	expires->HighPart = lpdtmExpire->dwHighDateTime;
	int64 units_in_a_day = 10000000; // 100 nanoseconds is a unit
	units_in_a_day *= seconds_in_a_day;
	expires->QuadPart = expires->QuadPart+units_in_a_day*3; // three days - new on 070401 so lasts over the weekend -- better to be another toontalk.ini option??
	lpdtmExpire->dwLowDateTime = expires->LowPart;
	lpdtmExpire->dwHighDateTime = expires->HighPart;
	LPFILETIME lpdtmNow = new FILETIME;
   SystemTimeToFileTime(lpdtmSysTime,lpdtmNow);
   delete lpdtmSysTime;
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	if (RetrieveUrlCacheEntryFile(url,cache_entry_info,&buffer_size,0)) {
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		if (cache_entry_info->lpszLocalFileName != NULL &&
			 file_exists(cache_entry_info->lpszLocalFileName)) { // already there -- added second condition on 070401 since can be in cache entry but not found soon afterwards
#if TT_DEBUG_ON
			add_to_log_if_interesting_error();
#endif
			cache_entry_info->ExpireTime = *lpdtmExpire;
			cache_entry_info->LastAccessTime = *lpdtmNow;
			if (!SetUrlCacheEntryInfo(cache_entry_info->lpszSourceUrlName,cache_entry_info,CACHE_ENTRY_EXPTIME_FC||CACHE_ENTRY_ACCTIME_FC)) {
				tt_error_file() << "Warning: had troubles updating the cache entry for " << cache_entry_info->lpszSourceUrlName << endl;
			};
#if TT_DEBUG_ON
			add_to_log_if_interesting_error();
#endif
			return(copy_string(cache_entry_info->lpszLocalFileName));
		};
	};
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	string file_name = new char[MAX_PATH];
	if (!CreateUrlCacheEntry(url,0,NULL,file_name,0)) {
		// report error?
		delete [] file_name;
		return(NULL);
	};
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	boolean need_to_commit;
	if (!copy_file_or_url_to_file(url,file_name,need_to_commit)) {
		// warn?
		// cleanup
		return(NULL);
	};
	if (!need_to_commit) return(file_name); // already there - no need to do more
//	FILETIME time;
//	time.dwHighDateTime = 0;
//	time.dwLowDateTime = 0;
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	boolean OK = CommitUrlCacheEntry(url,file_name,*lpdtmExpire,*lpdtmNow,NORMAL_CACHE_ENTRY,NULL,0,NULL,0); 
	// might use STICKY_CACHE_ENTRY??
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	delete lpdtmNow;
	delete lpdtmExpire;
	if (!OK) {
		tt_error_file() << "Error commiting " << url << " to cache file: " << file_name
			         << " error is " << last_error() << endl;
//		delete [] file_name;
//		return(NULL);
	};
	return(file_name);
};

string system_cached_url(string file_name) { // inverse of the above - new on 110101
	if (strchr(file_name,'[') == NULL) return(NULL); // new on 130302 to not bother if its name doesn't match the way files are named in the system cache
	DWORD size = 0;
//	if (!GetUrlCacheEntryInfo(file_name,NULL,&size)) {
//	string short_name = short_file_name(file_name,TRUE);
	string pattern_name = NULL;
	int start_extension = extension_start_position(file_name);
	if (start_extension >= 0) {
		pattern_name = append_strings("*.",file_name+start_extension);
	};
	FindFirstUrlCacheEntry(pattern_name,NULL,&size); // pattern_name was NULL prior to 130302
#if TT_DEBUG_ON
	add_to_log_if_interesting_error(122); // don't know why
#endif
	if (size == 0) {
		return(NULL);
	};
	size = size*5; // to avoid having to restart with a larger size -- was *3 prior to 130302
	INTERNET_CACHE_ENTRY_INFO *info = (INTERNET_CACHE_ENTRY_INFO *) new char[size];
	memset(info,0,size); // new on 070401
	info->dwStructSize = size;
//	if (GetUrlCacheEntryInfo(file_name,info,&size)) {
	DWORD new_size = size; // moved here on 130302 
	HANDLE more_entries = FindFirstUrlCacheEntry(pattern_name,info,&new_size); // pattern_name was NULL prior to 130302
#if TT_DEBUG_ON
	add_to_log_if_interesting_error(); 
#endif
	if (info->lpszLocalFileName != NULL && strcmp(info->lpszLocalFileName,file_name) == 0) {
		string answer = copy_string(info->lpszSourceUrlName); // new on 070401
		delete [] (char *) info;
		return(answer);
	} else { // search other entries
		while (TRUE) {
			new_size = size;
			if (FindNextUrlCacheEntry(more_entries,info,&new_size)) {
#if TT_DEBUG_ON
				add_to_log_if_interesting_error(); 
#endif
				if (info->lpszLocalFileName != NULL && strcmp(info->lpszLocalFileName,file_name) == 0) {
					string answer = copy_string(info->lpszSourceUrlName);
					delete [] (char *) info;
					if (pattern_name != NULL) delete [] pattern_name;
					return(answer);
				};
			} else if (new_size > size) {
#if TT_DEBUG_ON
				add_to_log_if_interesting_error(); // add error number
#endif
				delete [] (char *) info;
				size = new_size;
				size *= 2; // to avoid having to restart with a larger size
				info = (INTERNET_CACHE_ENTRY_INFO *) new char[size];
				more_entries = FindFirstUrlCacheEntry(pattern_name,info,&new_size); // and try again - // pattern_name was NULL prior to 130302
			} else {
#if TT_DEBUG_ON
				add_to_log_if_interesting_error(259); // no more data - not really an error
#endif
				delete [] (char *) info;
				if (pattern_name != NULL) delete [] pattern_name;
				return(NULL); // give up
			};
		};
	};
};

string user_cached_file_name(string url) {
	string short_name = basic_file_name(url,TRUE);
	if (short_name == NULL) { // warn??
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		return(system_cached_file_name(url)); // will have to do
	};
	string file_name = new char[MAX_PATH];
	output_string_stream stream(file_name,MAX_PATH);
	stream << tt_URL_cache_directory << short_name;
//	int extension_start = extension_start_position(url);
//	if (extension_start >= 0) {
//		stream << "." << url+extension_start;
//	};
	stream.put('\0');
	delete [] short_name;
//	FileNameStatus file_status;
//	string existing_file = existing_file_name(file_status,file_name,NULL,NULL,FALSE,FALSE);
//	if (existing_file != NULL) {
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	if (file_exists(file_name,FALSE)) { // re-written since the above fails
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		return(file_name); // already cached
	} else {
		boolean need_to_commit;
		if (!copy_file_or_url_to_file(url,file_name,need_to_commit)) { // was existing_file prior to 010600
			// warn?
#if TT_DEBUG_ON
			add_to_log_if_interesting_error();
#endif
			return(NULL);
		};
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		return(file_name);
	};
};

string cached_file_name(string url) {//, string extension) {
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	if (tt_URL_cache_directory == NULL) {
		return(system_cached_file_name(url));
	} else { // new on 300500
		string in_user_cache = user_cached_file_name(url);
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		if (in_user_cache != NULL) {
			return(in_user_cache);
		} else { // new on 040301 to try system cache if not in user cache
			return(system_cached_file_name(url));
		};
	};
};
#endif

/*
string cache_if_url(string file_or_url, string original_name) {
	if (file_or_url == NULL) return(NULL);
	// Only URLs beginning with ftp:, gopher:, http:, or https: are supported.
	if (is_url(file_or_url)) {
		 return(copy_to_temp_directory(file_or_url,original_name));
	};
	if (strnicmp(file_or_url,"file:",5) == 0) {
		return(file_or_url+5);
	};
	return(file_or_url);
};
*/

/*
string cached_file_name(string url,string extension) {
	ascii_string temp_directory = ini_entry(AC(IDC_DIRECTORIES),AC(IDS_TEMP_DIR));
	string file_name = new char[MAX_PATH];
	output_string_stream stream(file_name,MAX_PATH);
	if (temp_directory != NULL) stream << temp_directory;
	stream << "URLtemp";
	if (extension != NULL) {
		stream << "." << extension;
	};
	stream.put('\0');
	boolean need_to_commit;
	if (!copy_file_or_url_to_file(url,file_name,need_to_commit)) {
		return(NULL); // warn? cleanup
	};
	return(file_name);
};
*/
/*
string cached_file_name(string url,string extension) {
	string file_name = new char[MAX_PATH];
	if (URLDownloadToCacheFile(NULL,url,file_name,URLOSTRM_GETNEWESTVERSION,0,NULL) == S_OK) {
		return(file_name);
	} else {
		return(NULL);
	};
};
*/
/*
// noticed on 290105 that not used anymore
boolean copy_file_or_url_to_zip_file(string source, string destination, zipFile archive, int compression_level) {
	if (destination == NULL) return(FALSE); // new on 010600
#if TT_DYNA_ZIP
	if (!is_url(source)) { // new on 280603
		return(zip_file(archive,source,destination,compression_level));
	};
#endif
	DWORD bytes_read;
	File *source_file;
	if (!copy_file_or_url_to_file_start(source,bytes_read,source_file)) {
		return(FALSE);
	};
//	if (compress) { // experiment 200303
	open_zip_file(archive,destination,compression_level);
	while (bytes_read > 0) {
		write_zip_file(archive,(bytes) shared_buffer,bytes_read);
		source_file->Read(shared_buffer,shared_buffer_size,&bytes_read);
		// could check bytes_written to see if all is well
	};
//	source_file->Close(); // commented out on 071201 since deleting will Close it
	delete source_file; // new on 150101
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	close_zip_file(archive);
	//if (archive == tt_log_out_archive) { // new on 080703 -- obsolete 160204
	//	tt_log_out_archive_size++;
	//};
	return(TRUE);
};
*/

