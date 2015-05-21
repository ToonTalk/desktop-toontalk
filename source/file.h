// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved
#if !defined(__TT_FILE_H)

#define __TT_FILE_H

#if !defined(__TT_ZIP_H)   
#include "zip.h"
#endif

class File {
  // abstract class 
  public:
	  virtual ~File();
	  virtual BOOL Read(IN LPVOID lpBuffer, IN DWORD dwNumberOfBytesToRead,
							  OUT LPDWORD lpNumberOfBytesRead) = 0;
	  virtual BOOL Close() {
		  return(TRUE);
	  };
	  virtual BOOL Empty() = 0;
	  virtual long Size() {
		  return(0);
	  };
};

class LocalFile : public File {
	public:
	 ~LocalFile();
	  LocalFile(LPCSTR lpFileName, boolean warn=FALSE);  // pointer to the filename
	  BOOL Read(IN LPVOID lpBuffer, IN DWORD dwNumberOfBytesToRead,
					OUT LPDWORD lpNumberOfBytesRead);
	  BOOL Close();
	  BOOL Empty() {
		  return(hFile == INVALID_HANDLE_VALUE);
	  };
	  long Size() {
		  return(GetFileSize(hFile,NULL));
	  };
	protected:
	  HANDLE hFile;
};	

#if TT_MICROSOFT
class URLFile : public File {
	public:
	  URLFile(IN LPCSTR lpszUrl, boolean warn=FALSE);
	  ~URLFile();
	  BOOL Read(IN LPVOID lpBuffer, IN DWORD dwNumberOfBytesToRead,
					OUT LPDWORD lpNumberOfBytesRead);
	  BOOL Close();
	  BOOL Empty();
	protected:
		HINTERNET hFile;
};
#endif

File *open_file_or_url(string url, boolean ok_to_cache=TRUE, boolean warn=FALSE);
boolean is_404_error(string buffer, string source_file_name);
boolean copy_file_or_url_to_file(string source, string destination, boolean &need_to_commit);
//boolean copy_file_or_url_to_zip_file(string source, string destination, zipFile archive, int compression_level=default_compression_level);
string copy_file_or_url_to_string(string source, long &size, boolean warn=FALSE);
//string cache_if_url(string file_or_url, string original_name);
string cached_file_name(string name_with_extension); //, string extension);
string system_cached_url(string file_name);


#endif