// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved
#ifndef TT_COMMON_H

#define TT_COMMON_H

boolean get_special_folder(int CSIDL, string path);
string show_html_dialog(string html_file_name, HWND window_handle=NULL, string to_dialog=NULL, int dialog_width=-1, int dialog_height=-1, int dialog_left=0, int dialog_top=0); // left and top had -1 as default value prior to 240602
boolean dialog_closed(string from_dialog);
wide_string copy_wide_string(wide_string source, int length=-1);
wide_string copy_wide_string_with_code_page(string source, int length=-1, UINT code_page=CP_ACP);
void copy_wide_string_with_code_page(string source, int source_length, wide_string destination, UINT code_page=CP_ACP);

#endif