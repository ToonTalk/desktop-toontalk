// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved

#include "ttftp.h"
#include "ftp.h"

// Uses Windows WinInet services to open an FTP connection and upload a file

HINTERNET internet_handle = NULL;

BOOL internet_open(string agent_name) { // or application name
	internet_handle = InternetOpen(agent_name,INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0); // tried INTERNET_OPEN_TYPE_DIRECT as well
	if (internet_handle == NULL) {
		int x = GetLastError();
		return(FALSE);
	};
	return(TRUE);
};

BOOL internet_close() {
	if (internet_handle == NULL) return(FALSE);
   BOOL result = InternetCloseHandle(internet_handle);
	internet_handle = NULL;
	return(result);
};

ftp_handle ftp_open_connection(string server_name, string user_name, string password) {
	if (internet_handle == NULL) return(NULL);
	ftp_handle result = InternetConnect(internet_handle,server_name,INTERNET_DEFAULT_FTP_PORT,user_name,password,INTERNET_SERVICE_FTP,0,0);
	if (result == NULL) {
		int x = GetLastError();
		return(NULL);
	};
	return(result);
};

BOOL ftp_close_connection(ftp_handle ftp) {
	return(InternetCloseHandle(ftp));
};

BOOL ftp_set_directory(ftp_handle ftp, string directory) {
	return(FtpSetCurrentDirectory(ftp,directory));
};

BOOL ftp_put_file(ftp_handle ftp, string local_file, string remote_file) { // assumes binary is all I'll ever need
	return(FtpPutFile(ftp,local_file,remote_file,FTP_TRANSFER_TYPE_BINARY|INTERNET_FLAG_RELOAD,0)); // I guess INTERNET_FLAG_RELOAD is a good idea to bypass the cache
};




