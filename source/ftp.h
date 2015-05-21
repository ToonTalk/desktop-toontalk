// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved

#include <windows.h>
#include <Wininet.h>

typedef char *string;
typedef HINTERNET ftp_handle;

BOOL internet_open(string agent_name);

BOOL internet_close();

ftp_handle ftp_open_connection(string server_name, string user_name, string password);

BOOL ftp_close_connection(ftp_handle ftp);

BOOL ftp_set_directory(ftp_handle ftp, string directory);

BOOL ftp_put_file(ftp_handle ftp, string local_file, string remote_file);