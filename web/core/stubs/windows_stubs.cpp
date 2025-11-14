// Stubs for Windows-specific code
// These provide empty implementations for Windows APIs we don't need on web

#include <cstddef>
#include <cstring>

// ============================================================================
// Windows Type Definitions
// ============================================================================

typedef void* HWND;
typedef void* HDC;
typedef void* HBITMAP;
typedef void* HINSTANCE;
typedef unsigned long DWORD;
typedef unsigned long COLORREF;
typedef long LONG;
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;

#define TRUE 1
#define FALSE 0

// ============================================================================
// DirectDraw Stubs (we use our platform layer instead)
// ============================================================================

// Empty DirectDraw interface
class IDirectDraw {};
class IDirectDrawSurface {};

// ============================================================================
// DirectSound Stubs (we use Web Audio instead)
// ============================================================================

class IDirectSound {};
class IDirectSoundBuffer {};

// ============================================================================
// Windows GDI Stubs (we use our platform layer instead)
// ============================================================================

HDC CreateCompatibleDC(HDC hdc) { return nullptr; }
BOOL DeleteDC(HDC hdc) { return TRUE; }
HBITMAP CreateCompatibleBitmap(HDC hdc, int w, int h) { return nullptr; }
BOOL DeleteObject(void* obj) { return TRUE; }

// ============================================================================
// Windows Message/Event Stubs
// ============================================================================

BOOL PostMessage(HWND hwnd, unsigned int msg, size_t wparam, size_t lparam) {
    return TRUE;
}

// ============================================================================
// Windows String Functions (provide basic implementations)
// ============================================================================

int lstrcmpi(const char* s1, const char* s2) {
    return strcasecmp(s1, s2);
}

int lstrcmp(const char* s1, const char* s2) {
    return strcmp(s1, s2);
}

// ============================================================================
// Windows File I/O Stubs (we use our platform layer)
// ============================================================================

void* CreateFile(const char* name, DWORD access, DWORD share,
                 void* security, DWORD creation, DWORD flags, void* tmpl) {
    return nullptr;
}

BOOL CloseHandle(void* handle) {
    return TRUE;
}

BOOL ReadFile(void* handle, void* buffer, DWORD bytes, DWORD* read, void* overlapped) {
    return FALSE;
}

BOOL WriteFile(void* handle, const void* buffer, DWORD bytes, DWORD* written, void* overlapped) {
    return FALSE;
}
