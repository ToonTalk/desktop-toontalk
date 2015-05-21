// Copyright (c) 1992-2002.  Ken Kahn, Animated Programs, All rights reserved

#define BITMAP_9	9
#define BITMAP_8	8
#define BITMAP_7	7
#define BITMAP_6	6
#define BITMAP_5	5
#define BITMAP_2	2
//#define WELCOM16	106
#define BITMAP_1	1
//#define WELCOME	105
#define IDM_NEW            100
#define IDM_OPEN           101
#define IDM_SAVE           102
#define IDM_SAVEAS         103
#define IDM_PRINT          104
#define IDM_PRINTSETUP     105
#define IDM_EXIT           106
#define IDM_UNDO           200
#define IDM_CUT            201
#define IDM_COPY           202
#define IDM_PASTE          203
#define IDM_LINK           204
#define IDM_LINKS          205
#define IDM_HELPSEARCH     301
#define IDM_HELPHELP       302
#define IDM_HELPTOPICS     304
#define IDM_CONTINUE       305
#define IDM_INSTALL        306

#define IDC_STATIC -1

#define DLG_VERFIRST        400
#define IDC_COMPANY			DLG_VERFIRST
#define IDC_FILEDESC       	DLG_VERFIRST+1
#define IDC_PRODVER         DLG_VERFIRST+2
#define IDC_COPYRIGHT       DLG_VERFIRST+3
#define IDC_OSVERSION       DLG_VERFIRST+4
#define IDC_TRADEMARK       DLG_VERFIRST+5
#define DLG_VERLAST         DLG_VERFIRST+5

#define IDC_LABEL           DLG_VERLAST+1

#define ID_COMPANY			1
#define ID_APPNAME			2
#define IDS_WINDOW_NAME	3
#define IDS_YES_INSTALL	4
#define IDS_NO_INSTALL	5
#define ID_CMDLINE			3

BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK About  (HWND, UINT, WPARAM, LPARAM);

typedef char *string;
