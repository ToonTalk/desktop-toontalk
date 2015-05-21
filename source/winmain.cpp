// Copyright (c) 1992-2005. Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_GLOBALS_H)
#include "globals.h"
#endif
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif 
#if !defined(__TT_MAIN_H)   
#include "main.h"
#endif   
#if !defined(__TT_LOG_H)   
#include "log.h"
#endif   
#if !defined(__TT_CLEANUP_H)   
#include "cleanup.h"
#endif   
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif   
#if !defined(__TT_USER_H)   
#include "user.h"
#endif   
#if !defined(__TT_DIRTY_H)   
#include "dirty.h"
#endif   
#if !defined(__TT_SCREEN_H)
#include "screen.h"
#endif   
#if !defined(__TT_BLIT_H)   
#include "blit.h"
#endif   
#if !defined(__TT_HELP_H)
#include "help.h"
#endif
#if !defined(__TT_BIRD_H)
#include "bird.h"
#endif
#if !defined(__TT_PICTURE_H)
#include "picture.h"
#endif
#if !defined(__TT_PRGRMMR_H)   
#include "prgrmmr.h"
#endif
#if !defined(__TT_SPEAK_H)   
#include "speak.h"
#endif
#if !defined(__TT_MARTIAN_H)
#include "martian.h"
#endif
#if !defined(__TT_CITY_H)
#include "city.h"
#endif
#if !defined(__TT_PUZZLE_H)
#include "puzzle.h"
#endif
#if !defined(__TT_FILE_H)
#include "file.h"
#endif
#if !defined(__TT_TEXT_H)
#include "text.h"
#endif
#if !defined(__TT_COMMON_H)
#include "common.h"
#endif
#if TT_DIRECT_INPUT
#if !defined(__TT_INPUT_H)
#include "input.h"
#endif
#endif

#if TT_DIRECT_PLAY
#if !defined(__TT_NETWORK_H)
#include "network.h"
#endif
#endif

#if TT_XML
#if !defined(__TT_XML_H)
#include "xml.h"
#endif
#endif

#if GRAPHICS_TO_MEMORY
#include <mmsystem.h>
#if !TT_32
#include "dispdib.h"
#endif
#endif

#if !defined(__TT_HASH_H)
#include "hash.h"
#endif

#if !defined(__TT_GENPALET_H)
#include "genpalet.h"
#endif

#if TT_ENHANCED_DRAG_AND_DROP
#if !defined(__TT_DRAGDROP_H)
#include "dragdrop.h"
#endif
#endif

#if TT_DIRECTX_TRANSFORM
#include <dxtrans.h>  // DXSurface header file
#include "C:\DXMedia\include\dxtguid.c"
//#include <dxtguid.c>  // Guids for DXSurfaces -- commented out on 180401
#include <dxterror.h> // DXSurface error codes - restored on 270901
//#include <dxbounds.h> // Contains the CDXDVec and CDXDBnds classes used in this program
#endif

#include "wingutil.h"

#include "toontalk.h"   // for dialog constants
	
//#include <imm.h> // added 240800 for Windows Input Method Editor for wide character input (e.g. Japanese)    

#ifndef _INC_WINDOWS   
#include <windows.h>
#endif   
#if WING
#include "wing.h"
#endif
#include <stdlib.h>
#include <string.h>
//#include <strstrea.h> commented out 210703

#if !defined(__TT_WINMAIN_H)
#include "winmain.h"
#endif

#include <crtdbg.h> // new on 290302 for heap debugging

#include <shlobj.h> // new on 290302 to find correct directories to put things
#include <shfolder.h> // new on 260402 to find correct directories to put things

#if !TT_32
#include <shellapi.h>
#endif
//#if TT_DIRECTX
//#include "d:\dxsdk\sdk\inc\ddraw.h"
//#endif
//#include <lzexpand.h>
//#include <ctl3d.h>

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

MainWindow *tt_main_window = NULL;

HINSTANCE Main::hInstance = 0;
HINSTANCE Main::hPrevInstance = 0;
int Main::nCmdShow = 0;

//boolean windows_256_colors = TRUE; // until proven otherwise
ascii_string window_title = NULL;

char szClassName[] = "ToonTalk";

#if DISPDIB_CLASS
HWND dispdib_window_handle = NULL; // could move it inside MainWindow or the like
#endif

#if TT_DIRECTX_TRANSFORM

IDXSurfaceFactory *directx_transform_surface_factory = NULL; 

typedef IDXSurface *transform_surface;

//transform_surface transform_front_surface = NULL;
//transform_surface transform_back_surface = NULL;
flag directx_transform_surface_factory_initialized = FALSE; // new on 090201

#endif

#if TT_GDIPLUS
ULONG_PTR gdiplusToken;
#endif

HINSTANCE main_instance() {
	return(Main::hInstance);
};

// should clean this up and put them in main_window or something
flag paused = FALSE;
//flag need_to_unpause_toontalk = FALSE;
//boolean display_whole_next_frame = TRUE;
flag have_focus = FALSE;
//flag focus_changed = FALSE;
flag button_unchanged = FALSE;
flag initialization_completed = FALSE;
flag query_open = FALSE;
//flag need_to_set_focus = FALSE;
//flag need_to_set_back_and_front_surface_palette = TRUE; // new on 080600

boolean has_initialization_completed() {
  return(initialization_completed);
};

//millisecond tt_mouse_button_up_time = 0;
char mouse_buttons = 0;  // used to determine if all buttons are down
// neither kind of mouse event has happened
flag mouse_down_handled = TRUE;
flag mouse_up_handled = TRUE;

int client_width;// = 320;
int client_height;// = 200; // or is 240 a better default
int client_center_x;// = client_width/2;
int client_center_y;// = client_height/2;
flag full_screen = FALSE; // this means that the window covers the entire screen (including task bar, etc.) -- try to avoid this if possible
//flag non_exclusive_full_screen = FALSE; // same as above?? 050103

int window_width, window_height; // includes borders etc
long physical_screen_width, physical_screen_height;  // the device's

flag open_icon_on_next_frame = FALSE; // new on 250602

HINSTANCE display_dib_library = NULL;

#if TT_32
unsigned char *header_memory = NULL;
LPBITMAPINFO default_header = NULL;
#else
unsigned char header_memory[dib_header_size];
LPBITMAPINFO default_header = (LPBITMAPINFO) header_memory;
#endif

flag MCI_sent_message = FALSE;

flag waiting_for_user_dialog = FALSE;

HPALETTE CreateIdentityPalette(RGBQUAD aRGB[], int nColors,
                               int red_delta=0, int green_delta=0, int blue_delta=0);

//void print_default_header() {
//	RGBQUAD *quad = default_header->bmiColors;
//	tt_error_file() << tt_frame_number << endl;
//	for (int i = 0; i < 256; i++) {
//		tt_error_file() << (int) quad[i].rgbBlue << " "
//						<< (int) quad[i].rgbGreen << " "
//						<< (int) quad[i].rgbRed << " " << i << endl;
//	};
//	output_file_stream pal("toontalk.pal",std::ios_base::out|std::ios_base::binary);
//	pal.write((char *) default_header,dib_header_size);
//	pal.close();
//};

#if !WING
unsigned char header_with_identity_palette_memory[dib_header_size];
LPBITMAPINFO header_with_identity_palette
	 = (LPBITMAPINFO) header_with_identity_palette_memory;
#endif
//LPBITMAPINFO permuted_header = (LPBITMAPINFO) permuted_header_memory;
//unsigned char permuted_header_memory[dib_header_size];
//unsigned char reverse_color_permutation[256];

//unsigned char logical_palette_memory[sizeof(LOGPALETTE)+255*sizeof(RGBQUAD)];
//LOGPALETTE *logical_palette = (LOGPALETTE *) logical_palette_memory;

#if TT_MICROSOFT
typedef WORD (WINAPI *LPDISDIB)(dib lpbi, LPSTR lpBits, WORD wFlags);
WORD (WINAPI *DisplayDib)(dib lpbi, LPSTR lpBits, WORD wFlags);
#else
typedef UINT FAR PASCAL (*LPDISDIB) (LPBITMAPINFOHEADER lpbi, LPSTR lpBits, WORD wFlags);
UINT FAR PASCAL (*DisplayDib) (LPBITMAPINFOHEADER lpbi, LPSTR lpBits, WORD wFlags);
#endif

//WORD WINAPI DisplayDibNoOp(dib , LPSTR , WORD ) {
//	return(0);
//};

boolean displaydib_initialized = FALSE;

//#if DISPDIB_CLASS
//flag ignore_next_killfocus = FALSE;
//long got_focus_on_frame = 0;
//#endif

// not clear if this is worthwhile anymore since bytes are saved anyway
//int saved_background_index = -1;
//HGLOBAL saved_background_resource = 0;
//HGLOBAL saved_memory_handle = 0;
work_page saved_background_page = NULL;

flag cursor_hidden = FALSE;

flag user_wants_to_quit = FALSE;

void set_user_wants_to_quit(boolean new_flag) {
   user_wants_to_quit = (flag) new_flag;
	if (!tt_debugging_leaks) { // new on 180103
		tt_reclaiming_memory = FALSE; // no need to slow down the proces of quiting unless debugging leaks
	};
};

boolean about_to_quit() {
	return(user_wants_to_quit || tt_shutting_down || tt_standard_exit_started); // tt_standard_exit_started added on 270105
};

flag print_frame = FALSE;

//#if WING
HGLOBAL *brush_global_handles = NULL;
HBITMAP original_bitmap = 0;
HBRUSH *brushes = NULL;
//#if TT_DIRECTX
// since device_context is more ephemeral need to select brush before use
//int current_brush_id = 0;
//#endif

HBITMAP hBitmapNew;
long character_width, character_height;
HFONT font_handle = NULL;
double font_width = 0;
double font_height = 0;
flag font_bold = FALSE;
flag font_fixed_width = FALSE;
//#endif

long frame_of_last_pause = 0; // when it ended

HDC main_device_context = NULL;

flag another_toontalk_is_running = FALSE;

#if TT_DIRECTX

// don't understand why linker gave unresolved reference problems with IID_IDirectDraw2
//LPDIRECTDRAW2 direct_draw;
#if TT_DIRECT_DRAW7
LPDIRECTDRAW7 direct_draw = NULL;
#else
LPDIRECTDRAW direct_draw = NULL;
#endif
draw_surface_description surface_description;
//DDPIXELFORMAT pixel_format;
//draw_surface2 front_surface = NULL;
//draw_surface2 back_surface = NULL;
draw_surface front_surface = NULL;
draw_surface back_surface = NULL;
draw_surface true_back_surface = NULL;

boolean has_directx_been_initialized() { // new on 130401
	return(direct_draw != NULL);
};

LPDIRECTDRAWPALETTE direct_draw_palette = NULL;
RECT back_surface_rectangle;
RECT front_surface_rectangle = {0,0,640,480}; // default used by -v 22 -f 1
//LPRECT front_surface_rectangle = NULL;
typedef struct {
  RGNDATAHEADER region_data_header;
  RECT clip_rectangle;
} ClipStructure;
ClipStructure clip_structure;
ClipStructure scratch_clip_structure;
//bytes clip_region_data_memory = NULL;
//RGNDATAHEADER *clip_region_data_header;
//LPRGNDATA clip_region_data;
LPDIRECTDRAWCLIPPER clipper = NULL;
//draw_surface work_surface = NULL;
flag GDI_clipping_needed = FALSE;

int device_context_get_count = 0;
int lock_back_surface_count = 0;
#endif

// these added on 230402
coordinate directional_pad_center_x = -1, directional_pad_center_y, directional_pad_radius; // out here so shared with draw_directional_pad
coordinate directional_pad_mouse_x, directional_pad_mouse_y;
flag directional_pad_active = FALSE;

char user_programs_directory[MAX_PATH]; // new on 080404

#if TT_DIRECTX
void restore_surfaces();
void dderr_log(string message, string extra, boolean exit_afterwards=TRUE);

void dderr_log(string message, string extra, boolean exit_afterwards) {
   log(message,FALSE,TRUE);
   log(extra);
   character full_message[512];
   output_string_stream stream(full_message,512);
   if (exit_afterwards) {
      stream << "Terminating due to Direct Draw Error. ";
   } else {
      stream << "Continuing despite Direct Draw Error. "; // new on 120702
   };
   stream << message << " " << extra;
   stream.put('\0');
   if (exit_afterwards) {
		tt_exit_failure(TROUBLE_SHOOT_DIRECTX,full_message);
	};
};

void dserr(HRESULT result, string extra) {
	// Direct Sound error messages
  if (SUCCEEDED(result)) return;
  switch(result) {
		case DSERR_ALLOCATED:
			dderr_log(_T("The request failed because resources, such as a priority level, were already in use by another caller."),extra,FALSE);
			break;
		case DSERR_BADFORMAT:
			dderr_log(_T("The specified wave format is not supported."),extra,FALSE);
			break;
		case DSERR_INVALIDPARAM:
			dderr_log(_T("An invalid parameter was passed to the returning function."),extra,FALSE);
			break;
		case DSERR_NOAGGREGATION: 
			dderr_log(_T("The object does not support aggregation."),extra,FALSE);
			break;
		case DSERR_OUTOFMEMORY:
			dderr_log(_T("The DirectSound subsystem could not allocate sufficient memory to complete the caller's request."),extra,FALSE);
			break;
		case DSERR_UNINITIALIZED:
			dderr_log(_T("The IDirectSound::Initialize method has not been called or has not been called successfully before other methods were called."),extra,FALSE);
			break;
		case DSERR_UNSUPPORTED:
			dderr_log(_T(" The function called is not supported at this time."),extra,FALSE);
			break; 
		default:
			tt_error_file() << "Unrecognized DirectSound error. " << extra << endl;
  };
};

void dderr(HRESULT result, string extra) {
#if !TT_DEBUG_ON
	if (about_to_quit()) return; // new on 070401 since why include stuff in log file when quiting anyway
#endif
  if (SUCCEEDED(result)) return; // pull this out so breakpoints easy to set
  switch(result) {
    case DDERR_GENERIC:
      dderr_log(_T("There is an undefined error condition. "),extra);
      break;

    case DDERR_HEIGHTALIGN:
      dderr_log(_T("The height of the provided rectangle is not a multiple of the required alignment. "),extra);
      break;

    case DDERR_HWNDALREADYSET:
      dderr_log(_T("The DirectDraw CooperativeLevel HWND has already been set. It cannot be reset while the process has surfaces or palettes created. "),extra);
      break;

    case DDERR_HWNDSUBCLASSED: 
      dderr_log(_T("DirectDraw is prevented from restoring state because the DirectDraw CooperativeLevel HWND has been subclassed. "),extra);
      break;

    case DDERR_IMPLICITLYCREATED:
      log(_T("The surface cannot be restored because it is an implicitly created surface. "),FALSE,TRUE);
      if (extra != NULL) log(extra);
      break;

    case DDERR_INCOMPATIBLEPRIMARY:
      dderr_log(_T("The primary surface creation request does not match with the existing primary surface. "),extra);
      break;

    case DDERR_INVALIDCAPS:
      dderr_log(_T("One or more of the capability bits passed to the callback function are incorrect. "),extra);
      break;

    case DDERR_INVALIDCLIPLIST: 
      dderr_log(_T("DirectDraw does not support the provided clip list. "),extra);
      break;

    case DDERR_INVALIDDIRECTDRAWGUID: 
      dderr_log(_T("The GUID passed to the DirectDrawCreate function is not a valid DirectDraw driver identifier. "),extra);
      break;

    case DDERR_INVALIDMODE:
      dderr_log(_T("DirectDraw does not support the requested mode. "),extra);
      break;

    case DDERR_INVALIDOBJECT: 
      dderr_log(_T("DirectDraw received a pointer that was an invalid DirectDraw object. "),extra);
      break;

    case DDERR_INVALIDPARAMS:
      dderr_log(_T("One or more of the parameters passed to the method are incorrect. "),extra);
      break;

    case DDERR_INVALIDPIXELFORMAT:
      dderr_log(_T("The pixel format was invalid as specified. "),extra);
      break;

    case DDERR_INVALIDPOSITION: 
      dderr_log(_T("The position of the overlay on the destination is no longer legal. "),extra);
      break;

    case DDERR_INVALIDRECT:
      //if (tt_exclusive_screen && tt_debug_mode == 130502) { // with this can put breakpoint after this
      //   toggle_pause(TRUE,FALSE,FALSE);
      //};
      dderr_log(_T("The provided rectangle was invalid. "),extra,FALSE); // FALSE added on 010401 so this isn't fatal
      break;

    case DDERR_INVALIDSURFACETYPE: 
      dderr_log(_T("The requested operation could not be performed because the surface was of the wrong type. "),extra);
      break;

    case DDERR_LOCKEDSURFACES:
      dderr_log(_T("One or more surfaces are locked, causing the failure of the requested operation. "),extra);
      break;

    case DDERR_NO3D:
      dderr_log(_T("No 3D is present. "),extra);
      break;

    case DDERR_NOALPHAHW: 
      dderr_log(_T("No alpha acceleration hardware is present or available, causing the failure of the requested operation. "),extra);
      break;

    case DDERR_NOBLTHW:
      dderr_log(_T("No blitter hardware is present. "),extra);
      break;

    case DDERR_NOCLIPLIST: 
      dderr_log(_T("No clip list is available. "),extra);
      break;

    case DDERR_NOCLIPPERATTACHED:
      dderr_log(_T("No clipper object is attached to the surface object. "),extra);
      break;

    case DDERR_NOCOLORCONVHW:
      dderr_log(_T("The operation cannot be carried out because no color conversion hardware is present or available. "),extra);
      break;

    case DDERR_NOCOLORKEY: 
      dderr_log(_T("The surface does not currently have a color key. "),extra);
      break;

    case DDERR_NOCOLORKEYHW:
      dderr_log(_T("The operation cannot be carried out because there is no hardware support for the destination color key. "),extra);
      break;

    case DDERR_NOCOOPERATIVELEVELSET: 
      dderr_log(_T("A create function is called without the IDirectDraw::SetCooperativeLevel method being called. "),extra);
      break;

    case DDERR_NODC:
      dderr_log(_T("No device context (DC,extra) has ever been created for this surface. "),extra);
      break;

    case DDERR_NODDROPSHW:
      dderr_log(_T("No DirectDraw raster operation (ROP,extra) hardware is available. "),extra);
      break;

    case DDERR_NODIRECTDRAWHW: 
      dderr_log(_T("Hardware-only DirectDraw object creation is not possible; the driver does not support any hardware. "),extra);
      break;

    case DDERR_NODIRECTDRAWSUPPORT:
      dderr_log(_T("DirectDraw support is not possible with the current display driver. "),extra);
      break;

    case DDERR_NOEMULATION: 
      dderr_log(_T("Software emulation is not available. "),extra);
      break;

    case DDERR_NOEXCLUSIVEMODE:
      dderr_log(_T("The operation requires the application to have exclusive mode, but the application does not have exclusive mode. "),extra);
      break;

    case DDERR_NOFLIPHW:
      dderr_log(_T("Flipping visible surfaces is not supported. "),extra);
      break;

    case DDERR_NOGDI: 
      dderr_log(_T("No GDI is present. "),extra);
      break;

    case DDERR_NOHWND:
      dderr_log(_T("Clipper notification requires an HWND, or no HWND has been previously set as the CooperativeLevel HWND. "),extra);
      break;

    case DDERR_NOMIPMAPHW: 
      dderr_log(_T("The operation cannot be carried out because no mipmap texture mapping hardware is present or available. "),extra);
      break;

    case DDERR_NOMIRRORHW:
      dderr_log(_T("The operation cannot be carried out because no mirroring hardware is present or available. "),extra);
      break;

    case DDERR_NOOVERLAYDEST:
      dderr_log(_T("The IDirectDrawSurface::GetOverlayPosition method is called on an overlay that the IDirectDrawSurface::UpdateOverlay method has not been called on to establish a destination. "),extra);
      break;

    case DDERR_NOOVERLAYHW:
      dderr_log(_T("The operation cannot be carried out because no overlay hardware is present or available. "),extra);
      break;

    case DDERR_NOPALETTEATTACHED:
      dderr_log(_T("No palette object is attached to this surface. "),extra);
      break;

    case DDERR_NOPALETTEHW: 
      dderr_log(_T("There is no hardware support for 16- or 256-color palettes. "),extra);
      break;

    case DDERR_NORASTEROPHW:
      dderr_log(_T("The operation cannot be carried out because no appropriate raster operation hardware is present or available. "),extra);
      break;

    case DDERR_NOROTATIONHW:
      dderr_log(_T("The operation cannot be carried out because no rotation hardware is present or available. "),extra);
      break;

    case DDERR_NOSTRETCHHW: 
      dderr_log(_T("The operation cannot be carried out because there is no hardware support for stretching. "),extra);
      break;

    case DDERR_NOT4BITCOLOR:
      dderr_log(_T("The DirectDrawSurface is not using a 4-bit color palette and the requested operation requires a 4-bit color palette. "),extra);
      break;

    case DDERR_NOT4BITCOLORINDEX: 
      dderr_log(_T("The DirectDrawSurface is not using a 4-bit color index palette and the requested operation requires a 4-bit color index palette. "),extra);
      break;

    case DDERR_NOT8BITCOLOR:
      dderr_log(_T("The DirectDrawSurface is not using an 8-bit color palette and the requested operation requires an 8-bit color palette. "),extra);
      break;

    case DDERR_NOTAOVERLAYSURFACE:
      dderr_log(_T("An overlay component is called for a non-overlay surface "),extra);
      break;

    case DDERR_NOTEXTUREHW: 
      dderr_log(_T("The operation cannot be carried out because no texture mapping hardware is present or available. "),extra);
      break;

    case DDERR_NOTFLIPPABLE:
      dderr_log(_T("An attempt has been made to flip a surface that cannot be flipped. "),extra);
      break;

    case DDERR_NOTFOUND: 
      dderr_log(_T("The requested item was not found. "),extra);
      break;

    case DDERR_NOTINITIALIZED:
      dderr_log(_T("An attempt was made to invoke an interface method of a DirectDraw object created by CoCreateInstance before the object was initialized. "),extra);
      break;

    case DDERR_NOTLOCKED:
      dderr_log(_T("An attempt is made to unlock a surface that was not locked. "),extra);
      break;

    case DDERR_NOTPAGELOCKED: 
      dderr_log(_T("An attempt is made to page unlock a surface with no outstanding page locks. "),extra);
      break;

    case DDERR_NOTPALETTIZED:
      dderr_log(_T("The surface being used is not a palette-based surface. "),extra);
      break;

    case DDERR_NOVSYNCHW: 
      dderr_log(_T("The operation cannot be carried out because there is no hardware support for vertical blank synchronized operations. "),extra);
      break;

    case DDERR_NOZBUFFERHW:
      dderr_log(_T("The operation cannot be carried out because there is no hardware support for z-buffers when creating a z-buffer in display memory or when performing a z-aware blit. "),extra);
      break;

    case DDERR_NOZOVERLAYHW:
      dderr_log(_T("The overlay surfaces cannot be z-layered based on their BltOrder because the hardware does not support z-layering of overlays. "),extra);
      break;

    case DDERR_OUTOFCAPS: 
      dderr_log(_T("The hardware needed for the requested operation has already been allocated. "),extra);
      break;

    case DDERR_OUTOFMEMORY:
      dderr_log(_T("DirectDraw does not have enough memory to perform the operation. "),extra);
      break;

    case DDERR_OUTOFVIDEOMEMORY: 
      dderr_log(_T("DirectDraw does not have enough display memory to perform the operation. "),extra);
      break;

    case DDERR_OVERLAYCANTCLIP:
      dderr_log(_T("The hardware does not support clipped overlays. "),extra);
      break;

    case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
      dderr_log(_T("An attempt was made to have more than one color key active on an overlay. "),extra);
      break;

    case DDERR_OVERLAYNOTVISIBLE: 
      dderr_log(_T("The IDirectDrawSurface::GetOverlayPosition method is called on a hidden overlay. "),extra);
      break;

    case DDERR_PALETTEBUSY:
      if (!about_to_quit()) { // conditional added 280201
			log(_T("Access to this palette is refused because the palette is locked by another thread. "));
			if (extra != NULL) log(extra);
			pause_toontalk(TRUE);
			unpause_toontalk(); // resumed but may have lost surfaces 
		};
		break;

    case DDERR_PRIMARYSURFACEALREADYEXISTS: 
      dderr_log(_T("This process has already created a primary surface. "),extra);
      break;

    case DDERR_REGIONTOOSMALL:
      dderr_log(_T("The region passed to the IDirectDrawClipper::GetClipList method is too small. "),extra);
      break;

    case DDERR_SURFACEALREADYATTACHED:
      dderr_log(_T("An attempt was made to attach a surface to another surface to which it is already attached. "),extra);
      break;

    case DDERR_SURFACEALREADYDEPENDENT: 
      dderr_log(_T("An attempt was made to make a surface a dependency of another surface to which it is already dependent. "),extra);
      break;

    case DDERR_SURFACEBUSY:
		if (!about_to_quit()) { // conditional added 280201
				log(_T("Access to this surface is refused because the surface is locked by another thread. "));
			if (extra != NULL) log(extra);
			pause_toontalk(TRUE);
			unpause_toontalk(); // resumed but may have lots surfaces 
		};
		break;

    case DDERR_SURFACEISOBSCURED: 
      dderr_log(_T("Access to the surface is refused because the surface is obscured. "),extra);
      break;

    case DDERR_SURFACELOST:
		 if (!about_to_quit()) { // conditional added 280201
			 restore_surfaces();
			 if (tt_frame_number > 1) { // OK if this happens during initialization - e.g. message box in full screen mod
				 log(_T("Access to this surface is refused because the surface memory is gone. The DirectDrawSurface object representing this surface should have the IDirectDrawSurface::Restore method called on it. "),FALSE,TRUE);
				 if (extra != NULL) log(extra);
			 };
		 };
      break;

    case DDERR_SURFACENOTATTACHED:
      dderr_log(_T("The requested surface is not attached. "),extra);
      break;

    case DDERR_TOOBIGHEIGHT:
      dderr_log(_T("The height requested by DirectDraw is too large. "),extra);
      break;

    case DDERR_TOOBIGSIZE:
      dderr_log(_T("The size requested by DirectDraw is too large. However, the individual height and width are OK. "),extra);
      break;

    case DDERR_TOOBIGWIDTH:
      dderr_log(_T("The width requesteg by DirectDraw is too large. "),extra);
      break;

    case DDERR_UNSUPPORTED:
      dderr_log(_T("The operation is not supported. "),extra);
      break;

    case DDERR_UNSUPPORTEDFORMAT:
      dderr_log(_T("The FourCC format requested is not supported by DirectDraw. "),extra);
      break;

    case DDERR_UNSUPPORTEDMASK:
      dderr_log(_T("The bitmask in the pixel format requested is not supported by DirectDraw. "),extra);
      break;

    case DDERR_UNSUPPORTEDMODE:
      dderr_log(_T("The display is currently in an unsupported mode. "),extra);
      break;

    case DDERR_VERTICALBLANKINPROGRESS:
      dderr_log(_T("A vertical blank is in progress. "),extra);
      break;

    case DDERR_WASSTILLDRAWING:
		 // added FALSE as third arg on 071004 since I believe this isn't important enough to trigger termination -- see issue 559
      dderr_log(_T("The previous blit operation that is transferring information to or from this surface is incomplete. "),extra,FALSE);
      break;

    case DDERR_WRONGMODE:
      dderr_log(_T("This surface cannot be restored because it was created in a different mode. "),extra);
      break;

    case DDERR_XALIGN:
      dderr_log(_T("The provided rectangle was not horizontally aligned on a required boundary."),extra);
      break;
    };
//    if (tt_debug_mode == 3535 && device_context_get_count == 0 &&
//        lock_back_surface_count == 0) {
//       message_box(MB_OK,"Direct Draw error");
//       pause_toontalk();
//       log(extra);  // ideal spot for a breakpoint
//       unpause_toontalk();
//    } else {
//       log(extra);
//    };
};

boolean re_initializing_direct_draw = FALSE; // new on 150404 to avoid infinite recursion trying to fix things

void restore_surface(draw_surface surface) {
	if (about_to_quit()) return; // new on 280201
   HRESULT result;
   result = surface->IsLost();
   if (result == DDERR_SURFACELOST) {
      result = surface->Restore();
   };
   if (FAILED(result) && result != DDERR_IMPLICITLYCREATED) { 
		// ... added && result != DDERR_IMPLICITLYCREATED on 180804 since discussions such as
		// http://groups.google.com/groups?q=%22implicitly+created+surface%22&hl=en&lr=&ie=UTF-8&selm=YNLe6.236%24l21.97173%40news03.micron.net&rnum=4
		// indicate it can be ignored
      if (IsIconic(tt_main_window->get_handle())) {
         // new on 050602 since this can cause errors and isn't needed if minimized
         log("Ignoring problem restoring direct draw surface since ToonTalk is minimized.");
		} else if (result == DDERR_WRONGMODE && !re_initializing_direct_draw) { // new on 150404 trying to recover by re-initializing directX
			UnwindProtect<boolean> set(re_initializing_direct_draw,TRUE);
			tt_main_window->initialize_direct_draw();
      } else {
         dderr(result,_T("Restoring a lost DirectDraw surface."));
      };
   };
};

void restore_surfaces() {
	if (about_to_quit()) return; // new on 280201
   if (front_surface != NULL) restore_surface(front_surface);
   if (back_surface != NULL) restore_surface(back_surface);
   if (true_back_surface != NULL) restore_surface(true_back_surface);
//   restore_surfaces_in_cache();     // restored in different mode sometimes???
};

#endif

inline Window *GetPointer( HWND window_handle )
{
	 return (Window *) GetWindowLong( window_handle, 0 );
}
inline void SetPointer( HWND window_handle, Window *pWindow )
{
	 SetWindowLong( window_handle, 0, (LONG) pWindow );
}

// might want to revisit my attempt below to program around the counter scheme that Windows has here

boolean show_cursor(boolean show, boolean regardless) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 230404) {
		tt_error_file() << "show_cursor " << (int) show << ", " << (int) regardless << " on frame " << tt_frame_number << endl;
	};
#endif
   if (!initialization_completed) return(FALSE);
	if (!tt_mouse_acquired && !show 
		 && (tt_time_travel == TIME_TRAVEL_OFF || !tt_emulate_mouse_for_full_screen_time_travel)) { // new on 050305 so we don't get double cursors
		return(FALSE); // new on 141099
	};
   if (show && !tt_show_mouse_cursor && !regardless) return(FALSE); // new on 220402
#if TT_DEBUG_ON
	if (tt_debug_mode == 230404) {
		tt_error_file() << "show_cursor still running" << endl;
	};
#endif
//#if TT_DEBUG_ON&TT_ABSOLUTE_MOUSE
//	if (!show && !tt_absolute_mouse_mode != RELATIVE_MOUSE_MODE) return(FALSE);
//#endif
#if TT_DEBUG_ON
	add_to_log_if_interesting_error(0,"before ShowCursor");
#endif
	int count = 0;
	if (show) {
		if (!cursor_hidden) {
#if TT_DEBUG_ON
			if (tt_debug_mode == 230404) {
				ShowCursor(TRUE);
				count = ShowCursor(FALSE);
				tt_error_file() << "show_cursor !cursor_hidden so returning; count = " << count << endl;
			};
#endif
			return(FALSE);
		};
		while (ShowCursor(TRUE) < 0) {
			count++;
#if TT_DEBUG_ON
			if (tt_debug_mode == 230404) {
				tt_error_file() << "show_cursor count is " << count << endl;
			};
#endif
			if (count > 16) break; // probably there is no mouse
		};
	} else {
		if (cursor_hidden) return(FALSE);
		while (ShowCursor(FALSE) >= 0) {
			count++;
#if TT_DEBUG_ON
			if (tt_debug_mode == 230404) {
				tt_error_file() << "show_cursor count is " << count << endl;
			};
#endif
			if (count > 16) break; // probably there is no mouse
		};
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error(0,"after ShowCursor");
#endif
	cursor_hidden = (flag) !show;
#if TT_DEBUG_ON
	if (tt_debug_mode == 230404) {
		tt_error_file() << "show_cursor just set cursor_hidden to " << (int) cursor_hidden << " on frame " << tt_frame_number << endl;
	};
#endif
	return(TRUE); // really did change it
};

void focus_set() {
   if (!initialization_completed) return;
#if TT_DEBUG_ON
   if (tt_debug_mode == 1992) {
      tt_error_file() << "Setting focus. tt_shutting_down=" << (int) tt_shutting_down << " f=" << tt_frame_number << endl;
   };
#endif
   if (!tt_shutting_down && !paused && !replaying()) {
		if (!tt_show_mouse_cursor) {
#if TT_DEBUG_ON
			if (tt_debug_mode == 230404) {
				tt_error_file() << "focus_set calling show_cursor since !tt_show_mouse_cursor" << endl;
			};
#endif
			show_cursor(FALSE);
		};
      // following fixes problems with -f 3 and -f 4 on a small screen
      if (full_screen) {
         capture_mouse(); // grab the mouse anywhere
#if TT_DEBUG_ON
         if (tt_debug_mode == 1992) tt_error_file() << "Mouse captured." << endl;
#endif
      };
	};
	if (!tt_shutting_down && tt_screen != NULL) tt_screen->invalidate_cache();
   if (paused && full_screen) {
      unpause_toontalk();
//      log("Just unpaused after WM_SETFOCUS");
   };
#if TT_DIRECT_INPUT
   if (tt_using_direct_input) acquire_input_devices();
#endif
#if TT_DIRECTX
   restore_surfaces();
// 16 bit version seems to have problems going from static to nostatic 
// back and forth
   if (tt_debug_mode == 3333 && !user_wants_to_quit) { 
      // by default use entire palette
      HDC screen_device_context = GetDC(NULL);
      SetSystemPaletteUse(screen_device_context,SYSPAL_NOSTATIC);
      RealizePalette(screen_device_context);
      ReleaseDC(NULL,screen_device_context);
   };
#endif
};

void focus_killed() {
   if (waiting_for_user_dialog) return; // dialog box is up and lost focus
	if (!initialization_completed) return;
#if TT_DEBUG_ON
   if (tt_debug_mode == 1992 || tt_debug_mode == 130502) {
      tt_error_file() << "Killing focus. tt_shutting_down=" << (int) tt_shutting_down << " and about to quit is " << (int) about_to_quit() 
							 << " f=" << tt_frame_number << endl;
   };
#endif
   if (!paused && !replaying()) {
//#if !TT_MICROSOFT
//		if (tt_debug_mode == 0)
//#endif
	   show_cursor(TRUE,TRUE);
	};
#if TT_DIRECT_INPUT
   if (tt_using_direct_input) unacquire_input_devices();
#if TT_DEBUG_ON
   if (tt_debug_mode == 3333) {
      HDC screen_device_context = GetDC(NULL);
      SetSystemPaletteUse(screen_device_context,SYSPAL_STATIC);
      ReleaseDC(NULL,screen_device_context);
   };
#endif
#endif
   if (tt_exclusive_screen && !about_to_quit() && !paused) { // second condition added on 111000
      // third condition added on 240602 since can be in progress of pausing when lose focus
      // first condition was full_screen prior to 100602
      // tried tt_using_dispdib == 1 since caused problems with -f 4 in 640x480
      //  problem with making exception for this for demos is that it breaks on > 256 color systems
//     && !replaying()) { // if demos are running then keyboard or mouse interrrupts them...
      if (!tt_restore_exclusive_screen && !relative_mouse_mode()) { // new on 130502 -- second condition new 100602
         // new policy that Alt-tab of full screen when relative mouse mode minimizes ToonTalk
#if TT_DEBUG_ON
         if (tt_debug_mode == 130502) {
            log("Treating Alt-Tab (or other interruption) like control-F8");
         };
#endif
         tt_main_window->toggle_full_screen();
         tt_main_window->set_handling_transition_from_full_screen_on_frame(); 
			// new on 040602 since the body of this can cause focus to be regained and lost again
 		} else {
         tt_main_window->no_longer_handling_loss_of_focus(); // so the following lasts more than one cycle
			postpone_pause_toontalk(TRUE,TRUE,FALSE); // added 2nd and 3rd args on 100602 -- changed to TRUE on 240602 - 
         // matches better old behavior (could even have a different message on the dialog)
      };
   };
#if TT_ENHANCED_DRAG_AND_DROP
	release_drag_drop(tt_main_window->get_handle()); // new on 291204 since no longer available to receive drags
#endif
};

//millisecond dispdib_total_time = 0;

unsigned char bad_mouse_movement_count = 0;

void adjust_mouse_deltas(city_coordinate &delta_x, city_coordinate &delta_y) {
   if (frame_of_last_pause == tt_frame_number) { // crazy numbers possible
      delta_x = 0;
      delta_y = 0;
   };
   if (full_screen && (labs(delta_x) > tt_screen_width || labs(delta_y) > tt_screen_height)) { // trouble
      bad_mouse_movement_count++;
      if (bad_mouse_movement_count > 50) { // not just some strange intermitant problem
         trouble_shoot(TROUBLE_SHOOT_MOUSE,NULL,NULL,TRUE);
         bad_mouse_movement_count = 0;
      };
   } else {
      bad_mouse_movement_count = 0;
   };
//       GetPrivateProfileInt(C(IDC_SWITCHES),"NoBadMouseMovementWarningGiven",0,C(IDC_TT_INI)) == 0) {
//		WritePrivateProfileString(C(IDC_SWITCHES),"NoBadMouseMovementWarningGiven","1",C(IDC_TT_INI));
////      message_box(MB_ICONINFORMATION,S(IDS_MOUSE_MOVEMENT_WRONG));
//      tt_error_file() << S(IDS_MOUSE_MOVEMENT_WRONG) << endl; // until I eliminate false negatives
//   };
   delta_x = (delta_x*tt_mouse_sensitivity)/100;
   delta_y = (delta_y*tt_mouse_sensitivity)/100;
   if (!tt_exclusive_screen) { // tt_using_dispdib != 1) {
      delta_x = (delta_x*physical_screen_width)/tt_screen_width;
      delta_y = (delta_y*physical_screen_height)/tt_screen_height;
   };
};

/*
#if TT_32
void darken(color ) { // in exclusive mode OK, in windowed mode must in in 256 colors anyway
   darken_with_dark_colors();
};
#else
void darken(color frame_color) {
   if (windows_256_colors) {
      darken_with_dark_colors();
   } else {  // palette not to be used
      darken_with_color(frame_color);
   };
};
#endif
*/

#if TT_DEBUG_ON
POINT center_mouse; // to deal with bad mouse drivers (e.g. under SoftWindows and Virtual PC)
#endif

HCURSOR previous_cursor = NULL;

void wait_cursor() {
   HCURSOR cursor = LoadCursor(NULL,IDC_WAIT); // NULL was Main::hInstance prior to 250100
	if (cursor != NULL) {
		previous_cursor = SetCursor(cursor);
//#if TT_DEBUG_ON
//		tt_error_file() << "Set cursor to hour glass at " << timeGetTime() << endl;
//#endif
	};
};

void restore_cursor() {
	if (previous_cursor != NULL) {
		SetCursor(previous_cursor);
//#if TT_DEBUG_ON
//		tt_error_file() << "Restored cursor at " << timeGetTime() << endl;
//#endif
	};
};

#if TT_ALPHA_FEATURE

boolean mouse_moved() { // new on 031103 - used for auto hiding of time travel buttons
	 if (tt_using_direct_input && tt_mouse_mode == RELATIVE_MOUSE_MODE) {
		 long delta_x = 0, delta_y = 0;
		 UnwindProtect<boolean> set(tt_mouse_acquired,TRUE); // so the following reports changes despite the fact that ToonTalk "isn't listening"
#if TT_DIRECT_INPUT
       direct_input_mouse_delta(delta_x,delta_y);
#endif
		 return(delta_x != 0 || delta_y != 0);
    } else {
		 POINT mouse_position;
		 static long last_mouse_x = 0;
		 static long last_mouse_y = 0;
		 GetCursorPos(&mouse_position);
		 boolean result = (last_mouse_x != mouse_position.x || last_mouse_y != mouse_position.y);
		 last_mouse_x = mouse_position.x;
		 last_mouse_y = mouse_position.y;
		 return(result);
	 };
};

#endif

void Main::really_display_stuff() { // abstracted on 230404
// could hack this so when -v 22 -f 1 it draws on true back surface at hi res
#if TT_DEBUG_ON
	if (tt_debug_mode == 51001) {
		tt_error_file() << "About to restore_surfaces." << endl;
	};
#endif
   restore_surfaces();
//               lock_back_surface(); // commented out on 230803 since no longer serves a function since frame not drawn
//					 if (tt_stop_all && tt_darken_when_stopped) {
//						 darken(BLACK);
//						 tt_screen->screen_dirty(); // new on 190901
//					 };
//		          draw_frame(tt_gray); // restored on 230402 since hard get working right - commented out on 290302
#if TT_DEBUG_ON
	if (tt_debug_mode == 230803 && tt_titles_ended_on_frame <= tt_frame_number) {
		lock_back_surface();
		DWORD hash_size = hash_byte_length;
		byte image_hash[hash_byte_length];
		if (MD5_hash(tt_debug_screen_bytes,tt_screen_bytes_size,image_hash,&hash_size)) {
			char hash_string[hash_string_length+1];
			hash_to_string(image_hash,hash_string);
			tt_error_file() << hash_string << " " << tt_frame_number << endl;
		};
		unlock_back_surface();
	};
#endif
   if (tt_mouse_mode == SIMULATED_DIRECTIONAL_PAD_MOUSE_MODE) {
		lock_back_surface(); // moved here on 230803
      tt_main_window->draw_directional_pad();
		unlock_back_surface();
   };
//               unlock_back_surface(); // commented out on 230803
//                tt_main_window->show_subtitle();
#if TT_DEBUG_ON
	if (tt_debug_mode == 51001) {
		tt_error_file() << "About to flip_surfaces." << endl;
	};
#endif
   flip_surfaces();
#if TT_DEBUG_ON
	if (tt_debug_mode == 51001) {
		tt_error_file() << "Finished flip_surfaces." << endl;
	};
#endif
#if TT_DEBUG_ON
	add_to_log_if_interesting_error(0,"after flip_surfaces");
#endif
};

// following is new on 100105 so if this is run at the "wrong" time the messages are kept and resent at the "right" time
const int buffer_size = 256;
UINT messages[buffer_size];
WPARAM wParams[buffer_size];
LPARAM lParams[buffer_size];
int buffer_index = 0;

boolean buffer_message(UINT iMessage, WPARAM wParam, LPARAM lParam) {
	// new on 100105
	if (buffer_index >= buffer_size || tt_log_version_number < 67) {
		return(FALSE); // too many or running old demo
	};
	messages[buffer_index] = iMessage;
	wParams[buffer_index] = wParam;
	lParams[buffer_index] = lParam;
	buffer_index++;
	return(TRUE);
};

void empty_message_buffer() {
	HWND window_handle = tt_main_window->get_handle();
	for (int i = 0; i < buffer_index; i++) {
		SendMessage(window_handle,messages[i],wParams[i],lParams[i]);
	};
	buffer_index = 0;
};

boolean Main::MessageLoopOnce() {
	if (user_wants_to_quit) return(FALSE);
	MSG msg;
   run_postponed_pause_toontalk(); // if need be - new on 250602
   // new on 160502 -- waited a cycle to restore window again
   if (tt_main_window->return_restoring_exclusive_screen()) { // reorganized on 170502
      tt_restore_exclusive_screen = FALSE;
      tt_main_window->no_longer_restoring_exclusive_screen();
//    toggle_pause(FALSE,FALSE,FALSE);
//    tt_main_window->initialize_direct_draw();
//    tt_main_window->open_icon(); // probably not needed anymore
      get_exclusive_control(); // couldn't all this be done earlier?
      show_cursor(TRUE);
//    tt_exclusive_screen = TRUE; // opening the "icon" should fix this - commented out on 170502
   } else if (tt_main_window->return_handling_loss_of_focus() || open_icon_on_next_frame) {
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         tt_error_file() << "About to open icon since delayed. open_icon_on_next_frame is " << (int) open_icon_on_next_frame 
								 << " and frame is " << tt_frame_number << endl;
      };
#endif
      open_icon_on_next_frame = FALSE;
      tt_main_window->no_longer_handling_loss_of_focus();
//    waiting_for_user_dialog = TRUE; // well was when this was set up - maybe should rename the variable - new on 250602
      tt_main_window->open_icon();
//    waiting_for_user_dialog = FALSE;
//#if TT_DEBUG_ON
//		} else if (tt_main_window->handled_transition_from_full_screen_last_frame()) { // new on 050702
//			// doesn't work
//			log("debug this");
//			SendMessage(tt_main_window->get_handle(),WM_KILLFOCUS,NULL,NULL);
//			SetFocus(tt_main_window->get_handle());
//			SetActiveWindow(tt_main_window->get_handle()); // since otherwise can be impossible to move or minimize (without pausing first)
//#endif
   };
//      if (need_to_set_focus) {
//         query_open = TRUE;
//         if (BringWindowToTop(tt_main_window->get_handle()) == 0) {
//            log("Failed to bring window to top.");
//         };
//         need_to_set_focus = FALSE;
//      };
   if (tt_current_trouble != DONT_TROUBLE_SHOOT) { 
		// moved here from above on 240602 to avoid problems with trouble shooting the first frame - didn't help
		trouble_shoot(tt_current_trouble,tt_current_extra_message,tt_current_extra_message2,TRUE,TRUE);
      tt_current_trouble = DONT_TROUBLE_SHOOT;
	};
//	boolean time_travel_cursor_moved = FALSE;
	if (have_focus && tt_time_travel == TIME_TRAVEL_PAUSED && !IsIconic(tt_main_window->get_handle())) { // new on 230404 
		// added IsIconic(tt_main_window->get_handle()) test on 070904 since it seems it can happen (issue 551)
		if (display_time_travel_cursor()) { // no need if no movement
//			tt_screen->display(tt_screen->update_display());
//			tt_screen->screen_dirty(); // to be sure it is repainted
//			tt_screen->invalidate_cache();
//			tt_main_window->Paint();
			really_display_stuff();
			undisplay_time_travel_cursor();
//			restore_regions();
		};
	};
	if (paused || tt_time_travel == TIME_TRAVEL_PAUSED) {
//		InvalidateRgn(tt_main_window->get_handle(),NULL,FALSE);
//    if (need_to_unpause_toontalk) {
//       need_to_unpause_toontalk = FALSE;
//       OpenIcon(tt_main_window->get_handle());
//     } else
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         tt_error_file() << "Paused so just processing messages at " << timeGetTime() << endl;
      };
#endif
		empty_message_buffer(); // new on 100105
      if (GetMessage(&msg,NULL,0,0)) {
			if (msg.message == WM_SYSKEYDOWN && msg.wParam == VK_F4) return(FALSE); //  ALT-F4 to quit (new on 260205)
#if TT_DEBUG_ON 
//			UnwindProtect<boolean> set(tt_ok_to_process_windows_messages,TRUE); // just in case -- new on 260205
			int message_code = msg.message;
			if (tt_debug_mode == 260205) {
				tt_error_file() << "Paused and just read msg.message " << setiosflags(std::ios_base::hex) << msg.message << endl;
			};
#endif
			TranslateMessage(&msg);
#if TT_DEBUG_ON 
			if (msg.message != message_code) { 
				tt_error_file() << "Translated to " << setiosflags(std::ios_base::hex) << msg.message << endl;
			};
#endif
			DispatchMessage(&msg);
		} else { // if GetMessage returns 0 then WM_QUIT was sent
#if TT_DEBUG_ON 
			if (tt_debug_mode == 260205) {
				tt_error_file() << "Paused and just read WM_QUIT " << msg.message << endl;
			};
#endif
			return(FALSE);
		};
	} else if (!PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
		// no Windows message so run a cycle
		// read mouse rather than respond to WM_MOUSEMOVE
		// to avoid delays
		// moved here on 130800 from after reading mouse buttons since this sometimes that as well
      // and now on 180402 moved back here to deal with absolute mouse mode
		read_shift_and_control_keys();
      boolean left_button_just_went_down = FALSE; // new on 180402 for absolute mouse mode
      boolean left_button_still_down = FALSE; // new on 030502
      tt_hand_needs_to_move_relatively = FALSE; // reset this on every frame
      tt_hand_needs_to_move_to_cursor = FALSE; 
		// until left mouse button clicked again -- moved here from programmer_at_floor::react on 070502
      tt_delta_x_and_delta_y_due_solely_to_arrow_keys = FALSE; // reset this on every frame
      if (tt_dragging_permitted) { // only TRUE if absolute mouse mode
         note_mouse_state_for_dragging();
      };
		if (tt_mouse_button&LEFT_BUTTON || tt_mouse_button&RIGHT_BUTTON || tt_mouse_button&MIDDLE_BUTTON) {
         if (replaying()) { // any button while running a demo
            toggle_pause();
			};
			mouse_down_handled = TRUE;
			if (button_unchanged) {
				add_button_event(MOUSE_DEVICE,tt_mouse_button|BUTTON_STILL_DOWN);
			} else {
				add_button_event(MOUSE_DEVICE,tt_mouse_button);
            left_button_just_went_down = tt_mouse_button&LEFT_BUTTON;
			};
			button_unchanged = TRUE; // unless proven otherwise
         left_button_still_down = tt_mouse_button&LEFT_BUTTON;
			if (!mouse_up_handled) {
				tt_mouse_button = 0;
            mouse_buttons = 0;
				mouse_up_handled = TRUE;
				tt_dragging = FALSE; // new on 020105
			};
		};
		if (have_focus && !replaying()
#if TT_ENHANCED_DRAG_AND_DROP
			 && !tt_dragging
#endif
			) {
         // as of 220402 the mouse is recorded on every frame even if in absolute mouse mode and not button event (only used in some circumstances)
//#if TT_ABSOLUTE_MOUSE
//			&& (tt_mouse_mode == RELATIVE_MOUSE_MODE || left_button_just_went_down || tt_record_absolute_mouse_location)
//       // was (virtual_left_button(tt_mouse_button) && !(tt_mouse_button&BUTTON_STILL_DOWN)) -- and was tt_current_mouse_button prior to 180402
//#endif
 			long delta_x = 0, delta_y = 0;
         POINT mouse_position;
//       tt_hand_needs_to_move_to_cursor = (absolute_mouse_mode() && left_button_just_went_down);
//#if TT_DEBUG_ON
//			if (tt_debug_mode == 290602) {
//				tt_error_file() << "tt_using_direct_input is " << (int) tt_using_direct_input 
									//<< " and tt_mouse_mode is " << (int) tt_mouse_mode << endl;
//			};
//#endif
         if (tt_using_direct_input && tt_mouse_mode == RELATIVE_MOUSE_MODE) {
#if TT_DIRECT_INPUT
            direct_input_mouse_delta(delta_x,delta_y);
				if (tt_replay == NO_REPLAY && tt_log_started_frame == tt_frame_number) { 
					// new on 211103 so that while time travelling don't jump since clicked on time travel button
					delta_x = 0;
					delta_y = 0;
				};
            adjust_mouse_deltas(delta_x,delta_y); 
            // before reading arrow keys or joystick
            set_last_mouse_deltas(delta_x,delta_y);
#endif
         } else {
				GetCursorPos(&mouse_position); // in screen coordinates
#if TT_DEBUG_ON
            if (tt_debug_mode == 1100) {
               tt_error_file() << "Just read mouse's position as " << mouse_position.x << "," << mouse_position.y << endl;
            };
#endif
				if (tt_mouse_acquired) { // conditional added on 240500
#if TT_ABSOLUTE_MOUSE
					if (tt_mouse_mode == ABSOLUTE_MOUSE_MODE) { 
						// branch new on 290801
						delta_x = mouse_position.x-front_surface_rectangle.left;
						delta_y = front_surface_rectangle.bottom-mouse_position.y;
                  if (delta_x < 0 || delta_y < 0 || delta_x > client_width || delta_y > client_height) { 
                     // these were tt_screen_width etc prior to 200502 but didn't work if window wasn't 1x1
                     // new on 060502 -- since offscreen
                     delta_x = 0;
                     delta_y = 0;
                  } else {
                     tt_programmer->note_absolute_mode_deltas(delta_x,delta_y,left_button_still_down,left_button_just_went_down); // new on 030502
                  };
               } else if (tt_mouse_mode == SIMULATED_DIRECTIONAL_PAD_MOUSE_MODE) { // new on 230402
                  if (left_button_still_down) { // conditional new on 030502 so this only works while the pen is down
                  coordinate surface_width = front_surface_rectangle.right-front_surface_rectangle.left;
                  coordinate surface_height = front_surface_rectangle.bottom-front_surface_rectangle.top;
                  directional_pad_center_x = front_surface_rectangle.left+(surface_width*tt_directional_pad_center_x)/100;
                  directional_pad_center_y = front_surface_rectangle.bottom-(surface_height*tt_directional_pad_center_y)/100;
                  directional_pad_radius = (surface_height*tt_directional_pad_center_radius)/100;
                  coordinate x_distance = mouse_position.x-directional_pad_center_x;
                  coordinate y_distance = directional_pad_center_y-mouse_position.y;
                  coordinate mouse_distance_squared = x_distance*x_distance+y_distance*y_distance;
                  directional_pad_active = (mouse_distance_squared < directional_pad_radius*directional_pad_radius); // inside circle
                  if (directional_pad_active) { 
                     // full speed is 2/5 of screen per second
                     delta_x = (surface_width*x_distance*tt_millisecond_delta)/(directional_pad_radius*1000);
                     delta_y = (surface_height*y_distance*tt_millisecond_delta)/(directional_pad_radius*1000);
                     directional_pad_mouse_x = mouse_position.x-front_surface_rectangle.left;
                     directional_pad_mouse_y = mouse_position.y-front_surface_rectangle.top;
                     tt_hand_needs_to_move_relatively = TRUE;
                  };
                  } else {
                     delta_x = 0;
                     delta_y = 0;
                  };
               } else
#endif
					{	 
                  delta_x = mouse_position.x-client_center_x; // tried -center_mouse.x to fix Mac emulator problems -- no luck
						delta_y = client_center_y-mouse_position.y;
						adjust_mouse_deltas(delta_x,delta_y);
					};
				};
				// before reading arrow keys
				set_last_mouse_deltas(delta_x,delta_y);
         };
#if TT_DIRECT_INPUT
         // moved here so that this is done even if the direct_input_mouse wasn't used due to absolute mouse mode
         if (tt_using_direct_input) {
            direct_input_read_joysticks(delta_x,delta_y); // no-op if not around
         };
#endif
         if (tt_debug_mode == 11111 && (delta_x != 0 || delta_y != 0)) {
            tt_error_file() << "On frame " << tt_frame_number << " at " << tt_current_time
//                            << " mouse position is " << mouse_position.x << "," << mouse_position.y
                        << " where the mouse deltas are " << delta_x << "," << delta_y << endl;
         };
//			if (tt_using_dispdib == 1 || client_width > tt_screen_width) {
				// really if full screen (or nearly so)
//				delta_x /= 2;
//				delta_y /= 2;
//			};
			Sprite *tool_in_hand = tt_programmer->pointer_to_tool_in_hand();
//			if (tool_in_hand == NULL || !tool_in_hand->can_be_text()) { // conditional new on 090604
			// rewritten 110604 -- tried can_be_text (maybe rename to can be edited with arrow keys?) 
			// rather than the arrow_key_editing_started so first arrow key doesn't move it
			// but then the empty hand stops if moving by holding down arrow keys
			boolean editing_selection_or_tool_in_hand = FALSE;
			if (tt_programmer->kind_of() == PROGRAMMER_AT_FLOOR) { 
				// condition new on 030405 so can fly with arrow keys with arrow key editable stuff in pocket
				if (tool_in_hand != NULL) {
					// OK here since using arrow keys to move hand while holding is rare (I hope)
					editing_selection_or_tool_in_hand = (tool_in_hand->arrow_key_editing_status() != NO_ARROW_KEY_EDITING_POSSIBLE);
					// prior to 120604 was can_be_text(); 
				} else if (tt_selection != NULL) {
					editing_selection_or_tool_in_hand = (tt_selection->arrow_key_editing_status() == ARROW_KEY_EDITING_IN_PROGRESS);
				};
			};
			if (!editing_selection_or_tool_in_hand || tt_log_version_number < 45) { 
				read_arrow_keys(delta_x,delta_y);
			};
			record_mouse_delta(delta_x,delta_y);
			//if (tt_time_travel == TIME_TRAVEL_PAUSED) { // new on 230404
			//	display_time_travel_cursor();
			//} else
//             if (!tt_using_direct_input) {
				// use mouse like joystick so reset it
         if (tt_mouse_acquired) {
            if (!tt_direct_input_mouse_exclusive_ok && tt_mouse_mode == RELATIVE_MOUSE_MODE) {
//#if TT_ENHANCED_DRAG_AND_DROP
//					if (!tt_dragging) // && tt_current_time != tt_current_time_pause_ended)
						// if dragging then don't reset mouse position -- first cycle after ending pause don't do this either -- new on 020105
//#endif
						SetCursorPos(client_center_x,client_center_y);
#if TT_DEBUG_ON&&TT_ENHANCED_DRAG_AND_DROP
					if (tt_debug_mode == 20105 && !tt_dragging) {// && tt_current_time != tt_current_time_pause_ended) {
						tt_error_file() << "Setting cursor to center on frame " << tt_frame_number << " at time " << tt_current_time << endl;
					};
#endif
            } else if (tt_hand_needs_to_move_relatively) { // need to move the cursor to follow arrow keys or joystick
               city_coordinate programmer_x = min_long, programmer_y;
               TTRegion programmer_region;
               Sprite *tool_in_hand = tt_programmer->pointer_to_tool_in_hand();
               if (tool_in_hand != NULL) {
                  if (tool_in_hand->kind_of() == COPIER) { 
					      Sprite *attached = ((Copier *) tool_in_hand)->attached_tool();
                     if (attached == NULL) {
                        if (tt_programmer->pointer_to_appearance()->kind_of() == ROBOT_IN_TRAINING) {
                           tool_in_hand->active_region(programmer_region);
                        } else { // use upper right corner of hand holding wand
                           Sprite *appearance = tt_programmer->pointer_to_appearance();
                           programmer_x = appearance->current_llx();
                           programmer_y = appearance->current_ury();
                        };
                     } else {
                        attached->active_region(programmer_region);
                     };
                  } else { 
                     tool_in_hand->active_region(programmer_region);
                  };
               };
               if (programmer_x == min_long && // not set above
                  (tool_in_hand != NULL || tt_programmer->compute_selection_region(programmer_region))) {    
                  programmer_region.region_center(programmer_x,programmer_y);
               };
               if (programmer_x != min_long && tt_mouse_mode == ABSOLUTE_MOUSE_MODE) { 
                  coordinate x = tt_screen->screen_x(programmer_x)+front_surface_rectangle.left;
                  coordinate y = front_surface_rectangle.bottom-tt_screen->screen_y(programmer_y);
                  SetCursorPos(x,y);
               };             
//             SetCursorPos(mouse_position.x+delta_x,mouse_position.y+delta_y); // not accurate enough
            };
			};
#if TT_DEBUG_ON
			if (tt_debug_mode == 1100) {
				GetCursorPos(&center_mouse);
				tt_error_file() << "Just set mouse's position to " << client_center_x << "," << client_center_y << endl;
				tt_error_file() << "Reading it right back got " << center_mouse.x << "," << center_mouse.y << endl;
			};
#endif
		} else {
			if (!have_focus) { // ignore mouse movements if don't have focus
				record_mouse_delta(0,0);
			};
		};
      // prior to 180402 read mouse buttons here but now need to read them before calculating mouse position in case running in absolute mode
      tt_mouse_up_simulated = FALSE;
#if TT_DEBUG_ON
		millisecond start;
		if (tt_debug_mode == 51001) {
			tt_error_file() << "About to do one_tt_cycle on frame " << tt_frame_number << " ;tt_reply is "<< (int) tt_replay 
								 << "; tt_millisecond_delta = " << tt_millisecond_delta << endl;
			start = timeGetTime();
		};
#endif
		if (tt_time_travel == TIME_TRAVEL_JUST_PAUSED) {
			tt_time_travel = TIME_TRAVEL_PAUSED; // when paused run only one cycle
			// prior to 170603 the following was to run one_tt_cycle instead
			// moved on 180703
			//tt_screen->propagate_changes(); // needed??
			//tt_screen->update_display();
			//tt_programmer->pointer_to_appearance()->update_display(); // new on 180603 - otherwise hand holding wand is broken
			tt_screen->display(FALSE);
			tt_screen->screen_dirty(); // to be sure it is repainted
			tt_main_window->Paint();
		} else if (one_tt_cycle()) {
#if TT_DEBUG_ON
			if (tt_debug_mode == 51001) {
				tt_error_file() << "one_tt_cycle returned after " << (timeGetTime()-start) << "ms" << endl;
			};
			if (tt_debug_mode == 170502) {
				tt_error_file() << "tt_skip_titles is " << (int) tt_skip_titles 
									 << " and tt_exclusive_screen is " << (int) tt_exclusive_screen
									 << " and dirty comparison is " << (int) (tt_dirty_min_y < tt_dirty_max_y) << endl;
			};
#endif
			boolean cursor_displayed = FALSE; // new on 010305
			if (tt_time_travel == TIME_TRAVEL_ON) { // new on 290404
				cursor_displayed = display_time_travel_cursor();
			};
         if (!tt_skip_titles && 
				(have_focus || tt_exclusive_screen) && // added 311099 so control-f8 is more reliable
				(tt_dirty_min_y < tt_dirty_max_y || tt_screen->subtitles_showing())) { // something changed
				// moved into conditional since no point doing this if nothing changed
				really_display_stuff();
				if (cursor_displayed) { // updated 010305 tt_time_travel == TIME_TRAVEL_ON) { // new on 260205
					undisplay_time_travel_cursor();
				};
				if ((print_frame && tt_dirty_min_y < tt_dirty_max_y) // something has changed
//#if TT_DEBUG_ON
//					 || (tt_debug_mode == 170105 && tt_debug_frame_number == tt_frame_number)
//#endif
					) { 
    				char file_name_buffer[MAX_PATH]; // was 256 prior to 080402
					output_string_stream file_name(file_name_buffer,MAX_PATH);
//					ascii_string main_dir = ini_entry(AC(IDC_DIRECTORIES),AC(IDS_MAIN_DIR),FALSE);
					// rewritten on 080402 to use personal file folder
//					if (SHGetFolderPath(NULL,CSIDL_PERSONAL|CSIDL_FLAG_CREATE,NULL,SHGFP_TYPE_CURRENT,common_documents) != S_OK) {
					// following is older version that is available on nearly all systems (for 95 and NT 4 requires IE 4.0)
//						char personal_file_path[MAX_PATH];
//						if (!SHGetSpecialFolderPath(NULL,personal_file_path,CSIDL_PERSONAL,TRUE)) {
//#if TT_DEBUG_ON
//							tt_error_file() << "Failed to find CSIDL_PERSONAL." << endl;
//#endif
//						};
//						file_name << personal_file_path << "\\ToonTalk\\frame"; // was "frm" prior to 080402
					file_name << tt_user_directory << "frame";
					file_name << tt_frame_number;
					file_name << ".bmp";
//             if (main_dir != NULL) delete [] main_dir;
					file_name.put('\0');
					copy_screen_to_file(file_name_buffer); 
//					file_name.close();
					};
				};
				flush_input_events(); 
				// ready for new ones -- tried to comment out 290803 since this interferes with time travel (at segment boundaries)
				if (tt_time_travel_after_display_updated) {
					tt_time_travel_after_display_updated = FALSE;
					time_travel(TIME_TRAVEL_JUST_PAUSED);
					SetCursor(LoadCursor(NULL,IDC_ARROW)); 
					// added on 250100 since in full screen mode it seems to be needed (otherwise see hour glass)
//					restore_cursor(); // didn't work - don't know why
				}; 
			} else { // ToonTalk is terminating
				// might be paused by programmer during this cycle
				if (!paused) return(FALSE);
			};
			restore_input_events(); // no op if nothing saved
			if (tt_city_document_to_load != NULL) { // new on 300904
				stop_sound(FALSE); 
				// in case a repeating sound is going on -- note helicopter sound is restarted by programmer::load if need be
				if (!tt_initialize()) {
					return(0); // failure
				};
				tt_initialize_objects();
				if (tt_screen != NULL) { // just in case
					tt_screen->remove_current_subtitle();
				};
				tt_city->xml_entity_and_activate(tt_city_document_to_load);
				xml_release_document(tt_city_document_to_load);
				tt_city_document_to_load = NULL;
			};
		} else if (msg.message != WM_QUIT) {
			if (msg.message == WM_SYSKEYDOWN && msg.wParam == VK_F4) return(FALSE); 
			// HUH??? asked 270105 -- must be ALT-F4 (answered 260205)
			if (msg.message != WM_KEYDOWN || (msg.lParam&(0x1L<<30)) == 0) { // skip autorepeat
				empty_message_buffer(); // new on 100105
				TranslateMessage(&msg);
				DispatchMessage(&msg);
#if TT_DEBUG_ON
				add_to_log_if_interesting_error(1008,"after DispatchMessage");
#endif
			};
//		};
	} else {
		return(FALSE);
	};
	return(TRUE);
};

int Main::MessageLoop() {
#if TT_DEBUG_ON
	add_to_log_if_interesting_error(0,"Main::MessageLoop");
#endif
	while (TRUE) {
 // beginning 180703 uses exceptions to deal with both things like end of file for current segment as well as all other errors
		if (!replaying()) {
			if (!MessageLoopOnce()) return(0);
		} else {
			try { // else was missing prior to 040803 so many errors were caught that shouldn't	
#if TT_DEBUG_ON
				millisecond new_time = timeGetTime();
#endif
				if (!MessageLoopOnce()) return(0);
#if TT_DEBUG_ON
				if (tt_debug_mode == 3) {
					millisecond now = timeGetTime();
					tt_error_file() << (now-new_time)
										 << " elapsed during MessageLoop " << tt_frame_number << " at time " << now << endl;
				};
#endif
			} catch (CycleInterruptionReason reason) {
				switch (reason) {
					case EVENTS_COUNTER_TOO_LARGE: // notify user somehow? - e.g. sound or help screen??
					case EVENTS_COUNTER_MISSING:
					case UNRECOGNIZED_SPECIAL_EVENT_TOKEN:
					case LOG_FILE_MISSING_CITY_FILE_NAME:
					case EVENTS_COUNTER_NON_ZERO_WITH_NO_EVENT_QUEUE:
					case LOG_FILE_CORRUPTED:
					case UNABLE_TO_EXTRACT_CITY_FILE:
						play_sound(PLOP_SOUND); // new on 200204
						tt_error_file() << "Something went wrong re-running segment " << tt_current_log_segment 
											 << " reason code is " << (int) reason << endl;
						tt_err_file_important = TRUE;
#if TT_DEBUG_ON
						debug_this();
#endif
						// and the following
					case LOG_FILE_MISSING: // added 031103
					case END_OF_LOG_FILE:
						// condition commented out on 140504 since may not be enabled if running someone else's time travel log
//						if (time_travel_enabled()) { // not thrown if time travel is off so don't really need to check
						if (!about_to_quit()) { // condition new on 270105 -- didn't help
							if (tt_current_log_segment < tt_youngest_log_segment) {
								jump_to_log_segment(tt_current_log_segment+1,(reason == END_OF_LOG_FILE)); 
								// TRUE added 100204 -- replaced on 210404 so only if reached end of file is it possibly_successive
							} else {
								// if log is defective maybe should give a warning here? - asked 140504
								close_input_log(TRUE,FALSE);
								if (tt_youngest_log_segment < 0) { // new on 200804
									trouble_shoot(TROUBLE_SHOOT_BAD_DEMO,S(IDS_LOG_REPLAY_ERROR));
									set_user_wants_to_quit(TRUE);
								};
							};
						};
						break;
				}; // this was commented out rather than the one above from 140504 to 060604
				// will loop around and try again but in a catch 
	//			if (!MessageLoopOnce()) return(0); // try again - commented out on 190703
#if !TT_DEBUG_ON
			} catch (...) {
				if (!about_to_quit()) { // condition new on 260105
					// could catch other errors here - for robustness but bad for debugging
					string replay_file = replay_file_name();
					if (replay_file == NULL) { 
						// new on 250703 so no problem printing this out when there is no replay file name
						// rewritten on 250903
						tt_error_file() << "An error occurred within ToonTalk. To help us fix this problem you need to click on 'Set Options' when starting ToonTalk and say that users are knowledgable enough to help trouble shoot." << endl;
						if (tt_zip_description_archive_element != NULL) {
							dump_xml_to_zip_file(tt_zip_description_archive_element,"0.xml"); 
							// new on 250903 so if abnormal exit the log is updated
						};
					} else {
						tt_error_file() << "Error occurred while replaying " << replay_file << endl; 
						// fixed spelling of occurred on 050805
						tt_error_file() << "To help us improve ToonTalk please send ";
						if (tt_log_in_archive == NULL) {
							tt_error_file() << replay_file;
						} else {
							tt_error_file() << tt_log_in_archive;
						};
						tt_error_file() << " to support@toontalk.com" << endl;
						tt_err_file_important = TRUE;
						// and if there is a next segment jump to it
						if (tt_current_log_segment < tt_youngest_log_segment) {
							jump_to_log_segment(tt_current_log_segment+1,TRUE);
						} else {
							close_input_log(FALSE,FALSE);
						};
					};
					// will loop around and try again but in a catch
	//				if (!MessageLoopOnce()) return(0); // try again - commented out on 190703
				};
#endif
			};
		};
	};
	return(0); //(msg.wParam); updated on 250602 - no one cares what this returns anyway
};

//boolean absolute_mouse_position(city_coordinate &x, city_coordinate &y) { // new on 190402 for absolute mouse mode selection feedback
//   if (tt_mouse_acquired) {
//      POINT mouse_position;
//		GetCursorPos(&mouse_position); // in screen coordinates
//		x = tt_screen->x_pixel_to_screen(mouse_position.x-front_surface_rectangle.left);
//		y = tt_screen->y_pixel_to_screen(mouse_position.y-front_surface_rectangle.top);
//      return(TRUE);
//   } else {
//      return(FALSE);
//   };
//};

//boolean report_anomolous_timings() { // noticed obsolete on 230404
//   // for now just checks dispdib
//   if (dispdib_total_time*2 > tt_current_time && !replaying() &&
//       dispdib_total_time/tt_frame_number > max_dispdib_time &&
//       tt_frame_number > 20 && // skip initialization timings
//       !told_user(TOLD_FULL_SCREEN_PROBLEMS)) {
//      say(IDS_FULL_SCREEN_MODE_TOO_SLOW,4);
//      user_told(TOLD_FULL_SCREEN_PROBLEMS);
//      return(TRUE);
//   };
//   return(FALSE);
//};

flag already_shut_down = FALSE;  // should be member of MainWindow
flag premature_exit = FALSE;

void MainWindow::shut_down() {
   if (already_shut_down) return;
   tt_shutting_down = TRUE;
   already_shut_down = TRUE;
#if TT_ENHANCED_DRAG_AND_DROP
	release_drag_drop(window_handle); // new on 291204
#endif
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
#if TT_DIRECTX_TRANSFORM
	if (directx_transform_surface_factory != NULL) { // was causing errors so moved here on 210201
		try { // ignore errors here - don't understand why they are happening either
			directx_transform_surface_factory->Release();
		} catch (...) {
#if TT_DEBUG_ON
         tt_error_file() << "Error while releasing directx_transform_surface_factory during shut down." << endl;
#endif
		};
		directx_transform_surface_factory = NULL;
	};
#endif
#if GRAPHICS_TO_MEMORY
   if (exclusive_screen_desired()) { // tt_using_dispdib == 1) {
#if DISPDIB_CLASS
		 DisplayDibWindowClose(dispdib_window_handle);
#elif DISPDIB
		 DisplayDib(NULL,NULL,DISPLAYDIB_END|DISPLAYDIB_NOWAIT);
#endif
#if TT_DEBUG_ON
		 add_to_log_if_interesting_error();
#endif
		 show_cursor(TRUE); // shouldn't be necessary
		 write_ini_entry(AC(IDC_SWITCHES),AC(IDC_FULL_SCREEN_MODE_STARTED),"0");
	 };
    // following is annoying -- not needed anymore??
//	 SetCursorPos(client_center_x,client_center_y); // seems to get lost sometimes??
//#if !DISPDIB_CLASS
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	 if (display_dib_library != NULL) { // been loaded during its run
		 FreeLibrary(display_dib_library);
	 };
//#endif
#endif
/*
  if (logical_palette != NULL) {
	  UnlockResource(palette_resource);
	  FreeResource(palette_resource);
//	  delete logical_palette;
	  DeleteObject(palette_handle);
  };
*/
/*
  if (saved_background_resource != 0) {
	  FreeResource(saved_background_resource);
	  GlobalUnlock(saved_memory_handle);
	  GlobalFree(saved_memory_handle);
  };
*/
#if !TT_32
	 // on NT this fails so don't bother anyway when 32-bit since Windows takes care of this
  if (tt_bits_per_pixel == 8 && palette_handle != NULL) DeleteObject(palette_handle);
#endif
//  if ((UINT) tt_library >= 32) FreeLibrary(tt_library);
#if WING
	HBITMAP hBitmapOld;
	if (main_device_context != NULL && original_bitmap != NULL) {
		// Select the stock 1x1 monochrome bitmap back in
		hBitmapOld = (HBITMAP)SelectObject(main_device_context,original_bitmap);
		if (hBitmapOld != NULL) DeleteObject(hBitmapOld);
		if (font_handle != NULL) DeleteObject(font_handle);
		DeleteDC(main_device_context);
	};
#endif // this used to include the brush stuff below
	// do I need to select an old brush back in before this??
	for (int i = 0; i < DIB_brush_count; i++) {
		GlobalUnlock(brush_global_handles[i]);
		GlobalFree(brush_global_handles[i]);
		DeleteObject(brushes[i]);
	};
	delete [] brush_global_handles;
	delete [] brushes;
#if !GRAPHICS_TO_MEMORY
  SelectObject(tt_hidden_device_context,original_hidden_bitmap);
  DeleteDC(tt_hidden_device_context); 
//  DeleteDC(tt_visible_device_context);
  DeleteObject(tt_hidden_bitmap);
  if (previous_bitmap_index != -1) { // background loaded
	  SelectObject(background_device_context,original_background_bitmap);
	  DeleteObject(background_bitmap);
  };
//  ReleaseDC(window_handle,background_device_context);
  DeleteDC(background_device_context);
//  ReleaseDC(window_handle,restore_device_context);
  SelectObject(restore_device_context,original_restore_bitmap);
  DeleteDC(restore_device_context);
//  ReleaseDC(window_handle,tt_hidden_device_context);
  DeleteObject(restore_bitmap);
#endif
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
//  UnlockResource(palette_resource);
//  FreeResource(palette_resource);
  show_cursor(TRUE,TRUE);
  release_last_clipboard_string(); // replaces the following as of 300303
  //if (last_clipboard_string != NULL) {
	 // delete [] last_clipboard_string;
	 // last_clipboard_string = NULL;
  //};
  if (window_title != NULL) delete [] window_title;
#if TT_32
  if (header_memory != NULL) delete [] header_memory;
#endif
  //  Ctl3dUnregister(Main::hInstance);
  close_MCI();
  if (tt_debug_mode == 3333) {
     HDC screen_device_context = GetDC(NULL);
     SetSystemPaletteUse(screen_device_context,SYSPAL_STATIC);
     ReleaseDC(NULL,screen_device_context);
  };
#if TT_DIRECT_INPUT
   if (tt_using_direct_input) release_direct_input();
#endif
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
   if (full_screen && !exclusive_screen_desired()) { // tt_using_dispdib != 1) { // repaint the screen if used the whole thing
      RedrawWindow(NULL,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
	};
	deallocate_resource_strings(); // moved here on 300499 since these are used while cleaning up
#if TT_MICROSOFT
	if (tt_internet_connection != NULL) InternetCloseHandle(tt_internet_connection);
#endif
#if TT_GDIPLUS
	GdiplusShutdown(gdiplusToken);
#endif
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	if (tt_exe_to_run_upon_shutdown != NULL) { // new on 021103
//		execute(tt_exe_to_run_upon_shutdown,tt_command_line_for_exe_to_run_upon_shutdown,FALSE);
		ShellExecute(NULL,"open",tt_exe_to_run_upon_shutdown,tt_command_line_for_exe_to_run_upon_shutdown,tt_main_directory,SW_SHOW);
	};
	// moved here so file remains to run if that's where tt_exe_to_run_upon_shutdown is to be found
   if (tt_extracted_file_temp_directory != NULL) { // new on 291003
	   if (delete_all_files_in_directory(tt_extracted_file_temp_directory)) {
		   RemoveDirectory(tt_extracted_file_temp_directory);
	   };
   };
	if (tt_exe_to_run_upon_shutdown != NULL) {
		delete [] tt_exe_to_run_upon_shutdown;
	};
	if (tt_command_line_for_exe_to_run_upon_shutdown != NULL) {
		delete [] tt_command_line_for_exe_to_run_upon_shutdown;
	};
	if (tt_main_directory != NULL) delete [] tt_main_directory; // moved here on 021103
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	if (tt_extracted_file_temp_directory != NULL) { // moved out of TT_ALPHA_FEATURE on 280104
		delete [] tt_extracted_file_temp_directory;
		tt_extracted_file_temp_directory = NULL; // just in case code to clean up this directory is still about to run
	};
};

void MainWindow::close_MCI() {
   if (MCI_sent_message) {
	   MCI_send_string("close all",FALSE); // ,return_message,error_message);
//	   if (return_message != NULL) delete [] return_message;
//	   if (error_message != NULL) delete [] error_message;
   };
};

MainWindow::MainWindow() :
   Window(), // new on 280204
   palette_handle(NULL),
   saved_palette_handle(NULL),
   need_to_select_palette(TRUE),
	last_clipboard_size(-1),
	last_clipboard_string(NULL),
	last_clipboard_frame_number(tt_frame_number),
	current_clipboard_frame_number(-1),
   handling_loss_of_focus(FALSE),
   handling_transition_from_full_screen_on_frame(-1),
   restoring_exclusive_screen(FALSE),
   previous_window_x(-1),
   previous_window_y(-1),
   previous_window_width(-1),
   previous_window_height(-1) {
//#if DISPDIB_CLASS
//	 load_dispdib();
//	 window_handle =
//		 CreateWindow(DISPLAYDIB_WINDOW_CLASS,window_title,WS_OVERLAPPEDWINDOW,0,0,
//				GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),
//				NULL, NULL,
//				Main::hInstance, NULL);
//	 dispdib_window_handle = window_handle;
//#else
   window_handle = NULL; // might be used otherwise while creating it
	window_handle =
//		  CreateWindowEx(WS_EX_ACCEPTFILES, // was CreateWindowG without WS_EX_ACCEPTFILES prior to 281204
// restored on 291204 since DragAcceptFiles does this just as well
			CreateWindowG(szClassName,window_title,
//				           (tt_using_dispdib==1)?WS_POPUP:WS_OVERLAPPEDWINDOW,
                       WS_OVERLAPPEDWINDOW,
				           CW_USEDEFAULT,0,
				           CW_USEDEFAULT,0,
//                     client_width,client_height,
				           NULL,NULL,Main::hInstance,
				           (LPSTR) this); // Pass 'this' pointer in lpParam of CreateWindow().
//#else
//   window_handle =
//      CreateWindowEx((tt_using_dispdib==1)?WS_EX_TOPMOST:FALSE,
//                     szClassName,
//				         window_title,
//				         (tt_using_dispdib==1)?WS_POPUP:WS_OVERLAPPEDWINDOW,
//				         CW_USEDEFAULT,0,
//				         CW_USEDEFAULT,0,
//				         NULL,NULL,Main::hInstance,
//				         (LPSTR) this);
//#endif
	if (!window_handle) {
#if TT_32
		message_box_last_error();
#endif
		exit(EXIT_FAILURE);
	};
};

void MainWindow::initialize() {
	push_title(S(IDS_LOAD_WAIT,"Loading, please wait.")); // test on 120401
	HDC visible_device_context = GetDC(window_handle);
   device_width = GetDeviceCaps(visible_device_context,HORZRES); 
	// moved here on 060602 since these can be different after entering full-screen mode
	device_height = GetDeviceCaps(visible_device_context,VERTRES);
// pick any background for filling in DIB parameters
//	HRSRC resource_handle 
		  // they all have same palette
//		  = FindResource(tt_library,MAKEINTRESOURCE(FLOOR_BACKGROUND),RT_BITMAP);
//	HGLOBAL bitmap_resource = LoadResource(tt_library,resource_handle);
//	dib bitmap = (dib) LockResource(bitmap_resource);
/*
	memcpy(logical_palette->palPalEntry,
			 default_header->bmiColors,
			 256*sizeof(PALETTEENTRY));
*/
//	tt_memory_available_at_start = compute_memory_available(); // new on 010401
	// following only for 8-bit color
	// don't use the following and they seem only partially supported by DisplayDIB
//	if (tt_dispdib_mode == DISPLAYDIB_MODE_360x480x8 ||
//		 tt_dispdib_mode == DISPLAYDIB_MODE_320x480x8 ||
//		 tt_dispdib_mode == DISPLAYDIB_MODE_320x400x8) {
		 // non-square pixels 320x200 too but not so noticable
//		client_width = 320;
//		client_height = 240;
//	} else {
//   MoveWindow(window_handle,0,0,window_width,window_height,TRUE);
   compute_screen_dimensions(tt_screen_width,tt_screen_height);
#if TT_DEBUG_ON
   add_to_log_if_interesting_error(0,"before MoveWindow");
#endif
#if TT_DEBUG_ON
   if (tt_debug_mode == 130502) {
      tt_error_file() << "New window size including borders is " << window_width << " by " << window_height << endl;
      tt_error_file() << "Moving window's center to " << (physical_screen_width-window_width)/2 << "," 
							 << (physical_screen_height-window_height)/2 << endl;
   };
#endif
   previous_window_x = (physical_screen_width-window_width)/2; // initialize previous_window_x here beginning 060602
   previous_window_y = (physical_screen_height-window_height)/2;
   MoveWindow(window_handle,previous_window_x,previous_window_y,window_width,window_height,FALSE);
#if TT_DEBUG_ON
   add_to_log_if_interesting_error(0,"after MoveWindow");
   if (tt_debug_mode == 130502) {
      tt_error_file() << "Physical dimensions are " << physical_screen_width << " by " << physical_screen_height << endl;
      tt_error_file() << "Window dimensions (including borders) are " << window_width << " by " << window_height << endl;
   };
#endif
//	set_client_size(client_width,client_height);
/*
	HRSRC palette_resource_handle
		= FindResource(tt_library,
							MAKEINTRESOURCE(LOGICAL_PALETTE),
							MAKEINTRESOURCE(PALETTE_RESOURCE));
	palette_resource = LoadResource(tt_library,palette_resource_handle);
	logical_palette = (LOGPALETTE *) LockResource(palette_resource);
	palette_handle = CreatePalette(logical_palette);
*/
	// this check is now only done if using builtin old artwork
	if (tt_builtin_picture_directory == NULL &&
		 (!(GetDeviceCaps(visible_device_context,RASTERCAPS) & RC_PALETTE) ||
		  GetDeviceCaps(visible_device_context,SIZEPALETTE) != 256)) {
		if ( // until I fix the problem with DirectX and non-256 colors...
//#if !TT_DIRECTX
          !exclusive_screen_desired() && // tt_using_dispdib != 1 &&
//#endif
			 !told_user(TOLD_WINDOWS_SHOULD_BE_256_COLORS)) {
//			say(S(IDS_TOONTALK_BETTER_256));
			message_box(MB_ICONINFORMATION|MB_OK,S(IDS_TOONTALK_BETTER_256));
			user_told(TOLD_WINDOWS_SHOULD_BE_256_COLORS);
//		   say("But since you are running full-screen, the worst is that some colors may be wrong.  ");
		};
		tt_bits_per_pixel = 8;
		tt_bytes_per_pixel = 1;
	};
#if !GRAPHICS_TO_MEMORY
   if (!exclusive_screen_desired()) { // tt_using_dispdib != 1) {
		tt_hidden_device_context = CreateCompatibleDC(visible_device_context);
		tt_hidden_bitmap = CreateCompatibleBitmap(visible_device_context,
																tt_screen_width,
																tt_screen_height);
		if (!tt_hidden_bitmap) exit(FALSE);
		original_hidden_bitmap = (HBITMAP) SelectObject(tt_hidden_device_context,tt_hidden_bitmap); 
	};
	// starts off white
	PatBlt(tt_hidden_device_context,0,0,
			 tt_screen_width,tt_screen_height,WHITENESS);
	background_device_context = CreateCompatibleDC(visible_device_context);
//why??? won't it be "given" a background?
//	background_bitmap = CreateCompatibleBitmap(background_device_context,
//															 tt_screen_width,
//															 tt_screen_height);
	previous_bitmap_index = -1;
	restore_device_context = CreateCompatibleDC(visible_device_context);
	restore_bitmap = CreateCompatibleBitmap(visible_device_context,
														 tt_screen_width,
														 tt_screen_height);
	if (!restore_bitmap) exit(FALSE);
	original_restore_bitmap = (HBITMAP) SelectObject(restore_device_context,restore_bitmap);
	SetStretchBltMode(tt_hidden_device_context,COLORONCOLOR);
	if (!tt_hidden_bitmap || !restore_bitmap) {
		ReleaseDC(window_handle,visible_device_context);
		exit(FALSE);
	};
#endif
//	memcpy(header_memory,bitmap,dib_header_size);
//	memcpy(permuted_header,header_memory,dib_header_size);
#if !WING
	memcpy(header_with_identity_palette_memory,header_memory,dib_header_size);
#endif
// taken care of now by read_palette
//	palette_resource 
//	  = LoadResource(Main::hInstance,
//						  FindResource(Main::hInstance,"TOONTALK_PALETTE",RT_RCDATA));
//#if TT_32
//	memcpy(header_memory,LockResource(palette_resource),dib_header_size);
//#else
	// why is this a good idea and what about the header_memory being wasted?
//	default_header = (LPBITMAPINFO) LockResource(palette_resource);

// only for a new palette...
//	OFSTRUCT file_struct;
//	HFILE bmp = OpenFile("palette.bmp",&file_struct,OF_READ);
//	BITMAPFILEHEADER file_header;
//	_lread(bmp,(unsigned char *) &file_header,sizeof(BITMAPFILEHEADER));
//	unsigned char *new_header = new unsigned char[dib_header_size];
//	_lread(bmp,new_header,dib_header_size);
//	_lclose(bmp);
//	default_header = (LPBITMAPINFO) new_header;
//	HFILE new_pal = OpenFile("new.pal",&file_struct,OF_CREATE);
//	_lwrite(new_pal,(unsigned char *) default_header,dib_header_size);
//	_lclose(new_pal);
//#endif
#if TT_DIRECTX
//   LPDIRECTDRAW direct_draw1; // version 1 interface
//   result = direct_draw1->QueryInterface(IID_IDirectDraw2,
//                                         (LPVOID *) direct_draw);
//   if (FAILED(result)) {
      // message box better??
//      log("Problems initializing DirectDraw Interface2.");
//		exit(FALSE);
//   };
//   direct_draw1->Release();
//   ShowWindow(window_handle,SW_SHOWMAXIMIZED);
   initialize_direct_draw();
   // used to initialize direct_input here as well but need to do it before loading note pads
#else // direct X calls following itself
   initialize_palette();
#endif
#if WING
	// some of the following copied from sample code in WinG help files
//	struct {
//		BITMAPINFOHEADER InfoHeader;
//		RGBQUAD ColorTable[256];
//	} Info;
//	void far *pSurfaceBits;
// don't bother just now
	// Set up an optimal bitmap
//	if (WinGRecommendDIBFormat(default_header) == FALSE) {
//		tt_error_file() << "WinG doesn't like the DIB header" << endl;
//		exit(FALSE);
//	};
	// Set the width and height of the DIB but preserve the
	// sign of biHeight in case top-down DIBs are faster
//	tt_top_down_DIBs = (default_header->bmiHeader.biHeight == -1);
//	default_header->bmiHeader.biWidth = tt_screen_width;
//	default_header->bmiHeader.biHeight = tt_screen_height; // *=
	// Create a WinGDC and Bitmap, then select away
	main_device_context = WinGCreateDC();
	if (main_device_context != NULL) {
		SetBkMode(main_device_context,TRANSPARENT); // for text
		// by default rectangles have no borders
		SelectObject(main_device_context,GetStockObject(NULL_PEN));
      if (tt_debug_mode == 3333) { // by default use entire palette
          SetSystemPaletteUse(main_device_context,SYSPAL_NOSTATIC);
          RealizePalette(main_device_context);
      };
		hBitmapNew = WinGCreateBitmap(main_device_context,default_header,
												(void *far *) &tt_memory_graphics_work_page);
		if (hBitmapNew) {
			original_bitmap = (HBITMAP)SelectObject(main_device_context,hBitmapNew);
         WinGSetDIBColorTable(main_device_context,0,256,default_header->bmiColors);
		} else {
			DeleteDC(main_device_context);
			main_device_context = NULL;
		};
		tt_destination_work_page = tt_memory_graphics_work_page; // default
	};
	if (main_device_context == NULL) {
		tt_error_file() << "Problems creating WinG bitmap -- not enough memory?" << endl;
		exit(FALSE);
	};
	clear_page(tt_memory_graphics_work_page,tt_white);
#endif
#if TT_DIRECT_PLAY
	initialize_direct_play();
#endif
#if TT_DEBUG_ON
	if (tt_debug_mode == 3) {
		tt_error_file() << tt_current_time << " ms to initialize ToonTalk." << endl;
	};
#endif
//	tt_current_time = 0; // commented out on 120100
//	FreeResource(bitmap_resource);
	ReleaseDC(window_handle,visible_device_context);
#if TT_32
	ClipCursor(NULL); // added on 090699 to try to fix the Virtual PC mouse problem
#endif
//#if TT_UNICODE
	// this will set a UNicode font as the default for all operations
//BEGIN_GDI
//	tt_main_window->set_font(tile_width/3,(3*tile_height)/2,FALSE,FALSE);
//END_GDI
//#endif
   SetFocus(window_handle); // just in case also should fix color problems with 16 bit version on some systems
	Show(Main::nCmdShow);
	DragAcceptFiles(window_handle,TRUE); // new on 141099
// Update();
};

void MainWindow::initialize_palette() {
	tt_colors_available = 0;
   HDC visible_device_context = GetDC(window_handle);
	if ((GetDeviceCaps(visible_device_context,RASTERCAPS) & RC_PALETTE) &&
		 GetDeviceCaps(visible_device_context,SIZEPALETTE) == 256) {
      if (tt_debug_mode == 3333) { // use entire palette
         HDC screen_device_context = GetDC(NULL); 
         SetSystemPaletteUse(screen_device_context,SYSPAL_NOSTATIC);
         ReleaseDC(NULL,screen_device_context);
      };
		palette_handle = CreateIdentityPalette(default_header->bmiColors,colors_count);
//		if (tt_debug_mode == 71101) {
//			palette_handle = SelectPalette(visible_device_context,palette_handle,FALSE);
//		};
		PALETTEENTRY pe[colors_count];
		GetPaletteEntries(palette_handle,0,colors_count,pe);
		tt_colors[tt_colors_available++] = tt_black; // ok really?
      int whiteness = pe[tt_white].peRed+pe[tt_white].peGreen+pe[tt_white].peBlue;
      int blackness = pe[tt_black].peRed+pe[tt_black].peGreen+pe[tt_black].peBlue;
//      int distance_to_gray = 255; -- commented out on 130202 since this only is run with 8-bit color and done better below
		int darkness_of_darkest_non_black = whiteness;
		for (int i = 1; i < colors_count; i++) {
         int sum = pe[i].peRed+pe[i].peGreen+pe[i].peBlue;
         tt_colors[tt_colors_available++] = RGB(pe[i].peRed,pe[i].peGreen,pe[i].peBlue); // moved up from under conditional below		
			if (sum > 0) { // was 20 and eliminated near black copies
            if (sum > whiteness) { // better than default
               whiteness = sum;
               tt_white = (color_index) i;
            };
            int new_distance_to_gray = abs(127-pe[i].peRed)+
                                       abs(127-pe[i].peGreen)+
                                       abs(127-pe[i].peBlue);
//            if (new_distance_to_gray < distance_to_gray) {
//               distance_to_gray = new_distance_to_gray;
//               tt_gray = (color_index) i;
//            };
			} else if (sum < blackness) { // better than the default -- huh?? isn't this for sum == 0 ??
            tt_black = (color_index) i;
         };
			if (sum < darkness_of_darkest_non_black) {
				darkness_of_darkest_non_black = sum;
				tt_nearest_black = i;
			};
		};
#if TT_DEBUG_ON
      if (tt_debug_mode == 255255255) {
         for (int i = 0; i < colors_count; i++) {
            tt_error_file() << "      Color[" << i << "] = new Color("
                        << (int) GetRValue(tt_colors[i]) << ", "
                        << (int) GetGValue(tt_colors[i]) << ", "
                        << (int) GetBValue(tt_colors[i]) << ");" << endl;
         };
      };
#endif
	} else {
		// following results in different colors in DispDIB mode if
		// windows was running 16 vs 256 colors
		// so only used if not in 256 color mode
		// I believe this is fixed now as of 011001
		tt_colors[tt_colors_available++] = 0; // black
		RGBQUAD *quad = default_header->bmiColors;
		PALETTEENTRY system_palette[256];
		int system_count = GetSystemPaletteEntries(visible_device_context,0,256,system_palette);
		for (int i = 1; i < 256; i++) {
			if (quad[i].rgbBlue+quad[i].rgbGreen+quad[i].rgbRed > 0) {
				tt_colors[tt_colors_available++] = RGB(quad[i].rgbRed,quad[i].rgbGreen,quad[i].rgbBlue);
			} else {
				tt_colors[tt_colors_available++] = RGB(system_palette[i].peRed,system_palette[i].peGreen,system_palette[i].peBlue); 
				// new on 011001 since the above lacks the 20 built in colors (first and last 10)
			};
		};
      tt_colors[tt_white] = RGB(255,255,255);
	};
   const int max_brightness = 220;
   const int max_total_brightness = 10*210;
   int i;
	/*
   for (i = 0; i < tt_colors_available; i++) {
       if (GetRValue(tt_colors[i]) <= max_brightness &&
           GetGValue(tt_colors[i]) <= max_brightness &&
           GetBValue(tt_colors[i]) <= max_brightness &&
           GetRValue(tt_colors[i])*3+GetGValue(tt_colors[i])*6+GetBValue(tt_colors[i]) <= max_total_brightness) {
//           Y = 0.212671 * R + 0.715160 * G + 0.072169 * B;  // is true for modern monitors
// but some bright pink gets thru so I adjusted it a bit
          tt_dark_colors[i] = 255;
       } else {  // to be filled in with index of nearest dark color
          tt_dark_colors[i] = 0;
       };
   };
	*/
//	tt_dark_colors[0] = tt_colors[0];
   const int dark_difference = 50;
//   const int mismatch_difference = 127;
	if (tt_bits_per_pixel == 8) {
		tt_dark_color_indices[0] = 0; // don't permute black
		for (i = 1; i < tt_colors_available; i++) { // don't change black (starting 200901)
	//   if (tt_dark_colors[i] == 0) {
			int red = GetRValue(tt_colors[i]);
			int green = GetGValue(tt_colors[i]);
			int blue = GetBValue(tt_colors[i]);
			int red_goal = max(0,red-dark_difference);
			int green_goal = max(0,green-dark_difference);
			int blue_goal = max(0,blue-dark_difference);
			tt_dark_color_indices[i] = closest_color(red_goal,green_goal,blue_goal,i);
	//      tt_dark_colors[i] = tt_colors[tt_dark_color_indices[i]];
	//   };
		};
#if TT_DEBUG_ON
		if (tt_debug_mode == 190901) {
			 for (i = 0; i < tt_colors_available; i++) {
				 tt_error_file() << i << " = " << (int) GetRValue(tt_colors[i]) << " " << (int) GetGValue(tt_colors[i]) 
									  << " " << (int) GetBValue(tt_colors[i]) << " -> " 
									  << (int) tt_dark_color_indices[i] << " = " << (int) GetRValue(tt_colors[tt_dark_color_indices[i]]) 
									  << " " << (int) GetGValue(tt_colors[tt_dark_color_indices[i]]) << " " 
									  << (int) GetBValue(tt_colors[tt_dark_color_indices[i]]) << endl;
			 };
		};
#endif
		tt_nearest_text_background_color 
			= closest_color(GetRValue(text_background_color),GetGValue(text_background_color),GetBValue(text_background_color),-1); // new on 080202
		tt_nearest_number_background_color 
			= closest_color(GetRValue(number_background_color),GetGValue(number_background_color),GetBValue(number_background_color),-1); 
		tt_nearest_sound_background_color 
			= closest_color(GetRValue(sound_background_color),GetGValue(sound_background_color),GetBValue(sound_background_color),-1); 
		tt_nearest_force_background_color 
			= closest_color(GetRValue(force_background_color),GetGValue(force_background_color),GetBValue(force_background_color),-1);
		tt_nearest_operation_background_color 
			= closest_color(GetRValue(operation_background_color),GetGValue(force_background_color),GetBValue(force_background_color),-1); 
		// new on 130202
	} else { // new on 011001
		int *dark_color_changes = (int *) tt_dark_color_indices;   
		dark_color_changes[all_index] = -dark_difference;
		dark_color_changes[red_index] = 0;
		dark_color_changes[green_index] = 0;
		dark_color_changes[blue_index] = 0;
	};
	tt_gray = closest_color(128,128,128,-1); // new on 130202
	const int number_of_selection_permutations = 7;
	const int change = 20;
	if (tt_bits_per_pixel == 8) {
		const int dark_difference = 20; // moved into this conditional and changed value(and renamed) on 101201
		for (i = 0; i < number_of_selection_permutations; i++) {
			tt_selection_color_permutations[i][0] = 0; // don't permute black
			tt_dark_selection_color_permutations[i][0] = 0;
			tt_mismatch_color_indices[i][0] = 0; // moved here on 210902
		};
		for (i = 1; i < tt_colors_available; i++) {
	//		const int max_intensity = 196;
			int red = GetRValue(tt_colors[i]);
			int green = GetGValue(tt_colors[i]);
			int blue = GetBValue(tt_colors[i]);
			if (red == 255 && green == 0 && blue == 255) { // cyan is used for transparency sometimes so to be safe make it be the identity permuation
				// still make sense??
				for (int j = 0; j < number_of_selection_permutations; j++) {
					tt_selection_color_permutations[j][i] = i;
					tt_dark_selection_color_permutations[j][i] = i;
					tt_mismatch_color_indices[j][i] = i; // OK?
				};
			} else {
				for (int j = 0; j < number_of_selection_permutations; j++) {
					int flags = j+1; // each bit determines which of red, green, or blue is incremented as a goal color
					boolean change_red = flags&1;
					boolean change_green = flags&2;
					boolean change_blue = flags&4;
					int red_goal = red;
					int green_goal = green;
					int blue_goal = blue;
					if (change_red) {
		//				if (red > max_intensity) { // darken others
		//					green_goal -= change;
		//					blue_goal -= change;
		//				} else {
							red_goal += change;
		//					if (!change_green) green_goal -= change;
		//					if (!change_blue) blue_goal -= change;
		//				};
					};
					if (change_green) {
		//				if (green > max_intensity) { // darken others
		//					red_goal -= change;
		//					blue_goal -= change;
		//				} else {
							green_goal += change;
		//					if (!change_red) red_goal -= change;
		//					if (!change_blue) blue_goal -= change;
		//				};
					};
					if (change_blue) {
		//				if (blue > max_intensity) { // darken others
		//					green_goal -= change;
		//					red_goal -= change;
		//				} else {
							blue_goal += change;
		//					if (!change_green) green_goal -= change;
		//					if (!change_red) red_goal -= change;
		//				};
					};
					if (red_goal > 255) {
						int difference = (red_goal-255);
						green_goal -= difference;
						blue_goal -= difference;
						red_goal = 255;
					};
					if (green_goal > 255) {
						int difference = (green_goal-255);
						red_goal -= difference;
						blue_goal -= difference;
						green_goal = 255;
					};
					if (blue_goal > 255) {
						int difference = (blue_goal-255);
						green_goal -= difference;
						red_goal -= difference;
						blue_goal = 255;
					};
					red_goal= max(0,red_goal);
					green_goal = max(0,green_goal);
					blue_goal = max(0,blue_goal);
					tt_selection_color_permutations[j][i] = closest_color(red_goal,green_goal,blue_goal,i);
					red_goal= max(0,red_goal-dark_difference);
					green_goal = max(0,green_goal-dark_difference);
					blue_goal = max(0,blue_goal-dark_difference);
					tt_dark_selection_color_permutations[j][i] = closest_color(red_goal,green_goal,blue_goal,i);
					int mismatch_difference = 30*j;
					tt_mismatch_color_indices[j][i] 
						= closest_color(min(255,red+mismatch_difference),max(0,green-mismatch_difference),max(0,blue-mismatch_difference),i);
				};
			};
		};
#if TT_DEBUG_ON
		if (tt_debug_mode == 200901) {
			for (int j = 0; j < number_of_selection_permutations; j++) {
				tt_error_file() << "Permutation#" << j << endl;
				 for (i = 0; i < tt_colors_available; i++) {
					 tt_error_file() << i << " = " << (int) GetRValue(tt_colors[i]) << " " 
										  << (int) GetGValue(tt_colors[i]) << " " << (int) GetBValue(tt_colors[i]) << " -> " 
										  << (int) tt_selection_color_permutations[j][i] << " = " 
										  << (int) GetRValue(tt_colors[tt_selection_color_permutations[j][i]]) << " " 
										  << (int) GetGValue(tt_colors[tt_selection_color_permutations[j][i]]) << " " 
										  << (int) GetBValue(tt_colors[tt_selection_color_permutations[j][i]]) << endl;
				 };
			};
		};
#endif
	} else { // new on 011001
		const int dark_difference = 15; // moved into this conditional and changed from 50 (and renamed) on 101201
		// just store the deltas only
		for (int i = 0; i < number_of_selection_permutations; i++) {
			int flags = i+1; // each bit determines which of red, green, or blue is incremented as a goal color
			boolean change_red = flags&1;
			boolean change_green = flags&2;
			boolean change_blue = flags&4;
			int *color_changes = (int *) tt_selection_color_permutations[i];
			int *dark_color_changes = (int *) tt_dark_selection_color_permutations[i];
			int *mismatch_color_changes = (int *) tt_mismatch_color_indices[i];
			int mismatch_difference = 30*i;
			mismatch_color_changes[all_index] = 0;
			mismatch_color_changes[red_index] = mismatch_difference;
			mismatch_color_changes[green_index] = -mismatch_difference;
			mismatch_color_changes[blue_index] = -mismatch_difference;
			if (change_red) {
				color_changes[red_index] = change;
			} else {
				color_changes[red_index] = 0;
			};
			if (change_green) {
				color_changes[green_index] = change;
			} else {
				color_changes[green_index] = 0;
			};
			if (change_blue) {
				color_changes[blue_index] = change;
			} else {
				color_changes[blue_index] = 0;
			};
			color_changes[all_index] = -change;
			dark_color_changes[red_index] = color_changes[red_index]-dark_difference;
			dark_color_changes[green_index] = color_changes[green_index]-dark_difference;
			dark_color_changes[blue_index] = color_changes[blue_index]-dark_difference;
			dark_color_changes[all_index] = color_changes[all_index]-dark_difference;
		};
	};
 	ReleaseDC(window_handle,visible_device_context);
};

void MainWindow::compute_screen_dimensions(int width, int height) { // abstracted on 130502
   client_width = width;
	client_height = height;
//	};
//	if (tt_dispdib_mode == DISPLAYDIB_MODE_640x480x8 ||
//		 (tt_dispdib_mode == DISPLAYDIB_MODE_320x240x8 && tt_using_dispdib == 2)) {
	  // for now assume without checking that is the resolution of Windows
//		window_width = client_width;
//		window_height = client_height;
//	} else {	
		int border_width = 2*GetSystemMetrics(SM_CXFRAME);
		int border_height = 2*GetSystemMetrics(SM_CYFRAME)+GetSystemMetrics(SM_CYCAPTION);
		window_width = client_width+border_width; //8;
		window_height = client_height+border_height;
//	};
//#if TT_32
//	OSVERSIONINFO os_version;
//	GetVersionEx(&os_version);
//	if (os_version.dwPlatformId == VER_PLATFORM_WIN32_NT) {
//		window_width += 4;
//		window_height += 4;
//	};
//#endif
   physical_screen_width = GetSystemMetrics(SM_CXSCREEN);
   physical_screen_height = GetSystemMetrics(SM_CYSCREEN);
   if (tt_window_expansion_factor > 1) { // tt_using_dispdib >= 2) { // don't use dispdib but stretch by tt_window_expansion_factor
		window_width += client_width*(tt_window_expansion_factor-1);
		window_height += client_height*(tt_window_expansion_factor-1);
      if (window_width > physical_screen_width && window_height > physical_screen_height) {
         window_width = physical_screen_width;
		   window_height = physical_screen_height;
      };
      if (tt_video_mode == 320) { // why??
			client_width = window_width;
			client_height = window_height;
		};
	};
   if (exclusive_screen_desired()) { // tt_using_dispdib == 1) { -- not using tt_exclusive_screen_desired beginning 140502
      // on some systems with DirectX window must be full size
#if TT_DEBUG_ON
		if (tt_debug_mode == 130502) {
			tt_error_file() << "Reseting window dimensions from " << window_width << "x" << window_height << endl;
		};
#endif
#if TT_ENHANCED_DRAG_AND_DROP
		// experiment 301204 to cover most of the screen when full screen with the "underlying" window
		window_width = (9*physical_screen_width)/10;
		window_height = (9*physical_screen_height)/10;
#else
      window_width = tt_screen_width; // updated on 040602 physical_screen_width;
		window_height = tt_screen_height; // physical_screen_height;
#endif
		// used to go full screen take it so all mouse events go to ToonTalk
      // but made crashes not very friendly so smaller window used
	//} else if (window_width == physical_screen_width && window_height == physical_screen_height) {
	//	// new on 050103
	//	non_exclusive_full_screen = TRUE;
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 130502) {
		tt_error_file() << "Reseting window dimensions to   " << window_width << "x" << window_height << endl;
	};
#endif
};

void MainWindow::resize_window_and_center(int width, int height, boolean cut_in_half_if_full_screen) { 
   // new on 130502
   full_screen = FALSE; // since should be smaller or will be cut in half
#if TT_DEBUG_ON
   if (tt_debug_mode == 130502) {
      tt_error_file() << "resize_window_and_center: full_screen is " << (int) full_screen 
							 << " while tt_exclusive_screen_desired is " << (int) tt_exclusive_screen_desired
							 << " and tt_exclusive_screen is " << (int) tt_exclusive_screen << endl;
   };
#endif
   if (cut_in_half_if_full_screen && physical_screen_width <= width) {
      width /= 2;
      height /= 2;
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         tt_error_file() << "resize_window_and_center: cut width and height in half." << endl;
      };
#endif
   };
   compute_screen_dimensions(width,height); // new on 130502
   //MoveWindow(window_handle,
   //           (physical_screen_width-window_width)/2,(physical_screen_height-window_height)/2,
   //           window_width,window_height,TRUE);
//	if (relative_mouse_mode()) return; // new on 030702 so control-f8 works fine
   // rewritten below on 150502
   coordinate new_x,new_y,new_width,new_height;
   if (previous_window_width > 0 && absolute_mouse_mode()) { // if doing this repeatedly use the previous dimensions
		// added absolute_mouse_mode on 030702 so control-f8 works well
      new_x = previous_window_x;
      new_y = previous_window_y;
      new_width = previous_window_width;
      new_height = previous_window_height;
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         tt_error_file() << "Using previous values in resize_window_and_center." << endl;
      };
#endif
   } else {
      new_x = (physical_screen_width-window_width)/2; // center
      new_y = (physical_screen_height-window_height)/2;
      new_width = window_width;
      new_height = window_height;
   };
#if TT_DEBUG_ON
   if (tt_debug_mode == 130502) {
      tt_error_file() << "New window size including borders is " << new_width << " by " << new_height << endl;
      tt_error_file() << "Moving window's center to " << new_x << "," << new_y 
							 << " used previous values = " << (int) (previous_window_width > 0) << endl;
   };
#endif
   SetWindowPos(window_handle,
					 HWND_TOP, // prior to 240105 was HWND_TOPMOST, // could make this a parameter to control it -- most useful for its current use 
                new_x,new_y,new_width,new_height,
                0); // SWP_SHOWWINDOW|SWP_NOACTIVATE
#if TT_DEBUG_ON
   if (tt_debug_mode == 130502) {
      tt_error_file() << "SetWindowPos [1] called with " << new_x << " " << new_y << " " << new_width << " " << new_height << " and no flags." 
							 << " (frame=" << tt_frame_number << ")" << endl;
   };
#endif
};

/*
		// much redder and a bit brighter unless already pretty bright
		int red_goal, green_goal, blue_goal;
//		if (i == 255) {
//			log("debug this");
//		};
		if (red > 250) {
			red_goal = red;
			green_goal = max(0,green-64);
			blue_goal = max(0,blue-64);
//		} else if (red > 128) {
//			red_goal = min(255,red+15);
//			green_goal = max(0,green-15);
//			blue_goal = max(0,blue-15);
		} else {
			red_goal = red+25;
			green_goal = green; //min(255,green+15);
			blue_goal = blue; // min(255,blue+15);
		};
		tt_red1_color_indices[i] = closest_color(red_goal,green_goal,blue_goal,i);
		if (red > 250) {
			red_goal = red;
			green_goal = max(0,green-48);
			blue_goal = max(0,blue-48);
		} else {
			red_goal = red+20;
			green_goal = green; //min(255,green+15);
			blue_goal = blue; // min(255,blue+15);
		};
		tt_red2_color_indices[i] = closest_color(red_goal,green_goal,blue_goal,i);
		if (red > 250) {
			red_goal = red;
			green_goal = max(0,green-32);
			blue_goal = max(0,blue-32);
		} else {
			red_goal = red+15;
			green_goal = green; //min(255,green+15);
			blue_goal = blue; // min(255,blue+15);
		};
		tt_red3_color_indices[i] = closest_color(red_goal,green_goal,blue_goal,i);
		*/
	/*
   for (i = 1; i < tt_colors_available; i++) { // don't change black (starting 200901)
      int red = GetRValue(tt_colors[i]);
      int green = GetGValue(tt_colors[i]);
      int blue = GetBValue(tt_colors[i]);
		const int increment = 15;
		int change = -increment*number_of_selection_permutations/2;
		for (int j = 0; j < number_of_selection_permutations; j++) {
			int red_goal = red+change;
			int green_goal = green+change;
			int blue_goal = blue+change;
			if (green_goal < 0) {
				int difference = -green_goal;
				red_goal += difference;
				blue_goal += difference;
				green_goal = 0;
			};
			if (blue_goal < 0) {
				int difference = -blue_goal;
				green_goal += difference;
				red_goal += difference;
				blue_goal = 0;
			};
			if (red_goal < 0) {
				int difference = -red_goal;
				green_goal += difference;
				blue_goal += difference;
				red_goal = 0;
			};
			if (green_goal > 255) {
				int difference = 255-green_goal;
				red_goal += difference;
				blue_goal += difference;
				green_goal = 255;
			};
			if (blue_goal > 255) {
				int difference = 255-blue_goal;
				green_goal += difference;
				red_goal += difference;
				blue_goal = 255;
			};
			if (red_goal > 255) {
				int difference = 255-red_goal;
				green_goal += difference;
				blue_goal += difference;
				red_goal = 255;
			};
			red_goal= min(255,max(0,red_goal));
			green_goal = min(255,max(0,green_goal));
			blue_goal = min(255,max(0,blue_goal));
			tt_color_intensity_permutations[j][i] = closest_color(red_goal,green_goal,blue_goal,i);
			change += increment;
			if (change == 0) {
				change += increment;
			};
		};
	};
	*/

color_index closest_color(int red_goal, int green_goal, int blue_goal, int exception) {
   color_index best_so_far = 0;
   long best_score_so_far = max_long;
	for (int i = 1; i < tt_colors_available; i++) { // don't permute to black since might be encoding of transparent
      if (i != exception) { // tt_dark_colors[j] == 255) { // is dark - changed on 190901
         long red_change = abs(red_goal-GetRValue(tt_colors[i]));
         long green_change = abs(green_goal-GetGValue(tt_colors[i]));
         long blue_change = abs(blue_goal-GetBValue(tt_colors[i]));
         long score = red_change*red_change+green_change*green_change+blue_change*blue_change; 
			// square of differences new on 190901 to get more of an intensity change and less a hue change
//			score += (red_change-green_change)*(red_change-green_change)+(red_change-blue_change)*(red_change-blue_change)+
			//				(blue_change-green_change)*(blue_change-green_change);
         if (score < best_score_so_far) {
             best_score_so_far = score;
             best_so_far = i;
          };
      };
   };
	return(best_so_far);
};

	//	HDC screen_device_context = GetDC(NULL);
	// Make sure we are on a palettized device
/*
	if (!GetDeviceCaps(visible_device_context,RASTERCAPS) & RC_PALETTE) {
		tt_error_file() << "Not a palettized device -- setup Windows for 256 colors." << endl
						<< "Running full screen mode instead" << endl;
		tt_using_dispdib = TRUE;
	 } else {
		// Get the number of system colors and the number of palette entries
		// Note that on a palletized device the number of colors is the
		// number of guaranteed colors.  I.e. the number of reserved system colors
		int system_colors = GetDeviceCaps(visible_device_context,NUMCOLORS);
		int palette_size = GetDeviceCaps(visible_device_context,SIZEPALETTE);
		// if there are more than 256 colors we are wasting our time
		if (palette_size != 256) {
			tt_error_file() << "Windows should be setup for 256 colors." << endl
							<< "Running full screen instead." << endl;
			tt_using_dispdib = TRUE;
		} else {
			// Now we force the palette manager to reset its tables so that
			// the next palette to be realized will get its colors in the order they are
			// in the logical palette. This is done by changing the number of
			// reserved colors.
			SetSystemPaletteUse(visible_device_context,SYSPAL_NOSTATIC);
			SetSystemPaletteUse(visible_device_context,SYSPAL_STATIC);
			// Select our palette into the screen DC and realize it so that
			// its colors will be entered into the free slots in the physical palette
			HPALETTE original_palette
				 = SelectPalette(visible_device_context,
									  palette_handle, // ??? (HPALETTE) m_hObject, // our hpal
									  FALSE);
			RealizePalette(visible_device_context);
			// Now replace the old palette (but don't realize it)
			SelectPalette(visible_device_context,original_palette,FALSE);
			// The physical palette now has our colors set in place and its own
			// reserved colors at either end.  We can grab the lot now
			PALETTEENTRY pe[256];
			GetSystemPaletteEntries(visible_device_context,0,palette_size,pe);
			// Set the PC_NOCOLLAPSE flag for each of our colors so that GDI
			// won't merge them together.  Be careful not to set PC_NOCOLLAPSE for the 
			// sys color entries or we'll get multpile copies of these colors in
			// the palette when we realize it.
			int i;
			for (i = 0; i < system_colors/2; i++) {
				pe[i].peFlags = 0;
			};
			for (; i < palette_size-system_colors/2; i++) {
				pe[i].peFlags = PC_NOCOLLAPSE;
			};
			for (; i < palette_size; i++) {
			  pe[i].peFlags = 0;
			};
			// Resize the palette in case it was smaller
			// but I check that it really is 256...
//			ResizesizePalette(palette_handle,palette_size);
			// Update the palette entries with what is now in the physical palette
			SetPaletteEntries(palette_handle,0,palette_size,pe);
			ReleaseDC(window_handle,visible_device_context);
		};
	};
*/
/*
	int color_index = 0;
//	BLACK = tt_black_index;
	tt_rectangle_colors[color_index++] = BLACK;
	tt_rectangle_colors[color_index++] = GRAY;
	tt_rectangle_colors[color_index++] = LIGHT_GRAY;
	tt_rectangle_colors[color_index++] = WHITE;
	tt_rectangle_colors[color_index++] = LIGHT_BLUE;
	tt_rectangle_colors[color_index++] = LIGHT_GREEN;
	tt_rectangle_colors[color_index++] = LIGHT_CYAN;
	tt_rectangle_colors[color_index++] = LIGHT_RED;
	tt_rectangle_colors[color_index++] = LIGHT_MAGENTA;
	tt_rectangle_colors[color_index++] = LIGHT_YELLOW;
	GRAY = (unsigned char) GetNearestPaletteIndex(palette_handle,tt_rectangle_colors[color_index++]);
//	tt_rectangle_colors[color_index] = RGB(63,63,63);
//	DARK_GRAY = (unsigned char) GetNearestPaletteIndex(palette_handle,tt_rectangle_colors[color_index++]);
	tt_rectangle_colors[color_index] = RGB(192,192,192);
	LIGHT_GRAY = (unsigned char) GetNearestPaletteIndex(palette_handle,tt_rectangle_colors[color_index++]);
	tt_rectangle_colors[color_index] = RGB(255,255,255);
	WHITE = (unsigned char) GetNearestPaletteIndex(palette_handle,tt_rectangle_colors[color_index++]);
//	tt_rectangle_colors[color_index] = RGB(0,0,127);
//	BLUE = (unsigned char) GetNearestPaletteIndex(palette_handle,tt_rectangle_colors[color_index++]);
//	tt_rectangle_colors[color_index] = RGB(0,127,0);
//	GREEN = (unsigned char) GetNearestPaletteIndex(palette_handle,tt_rectangle_colors[color_index++]);
//	tt_rectangle_colors[color_index] = RGB(127,0,0);
//	RED = (unsigned char) GetNearestPaletteIndex(palette_handle,tt_rectangle_colors[color_index++]);
//	tt_rectangle_colors[color_index] = RGB(0,127,127);
//	CYAN = (unsigned char) GetNearestPaletteIndex(palette_handle,tt_rectangle_colors[color_index++]);
//	tt_rectangle_colors[color_index] = RGB(127,0,127);
//	MAGENTA = (unsigned char) GetNearestPaletteIndex(palette_handle,tt_rectangle_colors[color_index++]);
//	tt_rectangle_colors[color_index] = RGB(127,127,0);
//	BROWN = (unsigned char) GetNearestPaletteIndex(palette_handle,tt_rectangle_colors[color_index++]);
	tt_rectangle_colors[color_index] = RGB(0,0,255);
	LIGHT_BLUE = (unsigned char) GetNearestPaletteIndex(palette_handle,tt_rectangle_colors[color_index++]);
	tt_rectangle_colors[color_index] = RGB(0,255,0);
	LIGHT_GREEN = (unsigned char) GetNearestPaletteIndex(palette_handle,tt_rectangle_colors[color_index++]);
	tt_rectangle_colors[color_index] = RGB(255,0,0);
	LIGHT_RED = (unsigned char) GetNearestPaletteIndex(palette_handle,tt_rectangle_colors[color_index++]);
	tt_rectangle_colors[color_index] = RGB(0,255,255);
	LIGHT_CYAN = (unsigned char) GetNearestPaletteIndex(palette_handle,tt_rectangle_colors[color_index++]);
	tt_rectangle_colors[color_index] = RGB(255,0,255);
	LIGHT_MAGENTA = (unsigned char) GetNearestPaletteIndex(palette_handle,tt_rectangle_colors[color_index++]);
//	tt_rectangle_colors[color_index] = RGB(191,191,0); // was 255,255,0 ?
//	LIGHT_BROWN = (unsigned char) GetNearestPaletteIndex(palette_handle,tt_rectangle_colors[color_index++]);
	tt_rectangle_colors[color_index] = RGB(255,255,0);
	YELLOW = (unsigned char) GetNearestPaletteIndex(palette_handle,tt_rectangle_colors[color_index++]);
*/

//	SelectPalette(screen_device_context,original_palette,FALSE);
//#if TT_DIRECTX
//   LPDIRECTDRAW direct_draw1; // version 1 interface
//   result = direct_draw1->QueryInterface(IID_IDirectDraw2,
//                                         (LPVOID *) direct_draw);
//   if (FAILED(result)) {
      // message box better??
//      log("Problems initializing DirectDraw Interface2.");
//		exit(FALSE);
//   };
//   direct_draw1->Release();
//   ShowWindow(window_handle,SW_SHOWMAXIMIZED);
//   initialize_direct_draw();

#if TT_DIRECTX

void get_exclusive_control() {
   if (direct_draw == NULL) return; // warn? -- new on 170502
   tt_got_exclusive_control_on_frame = tt_frame_number;
   DWORD flags = DDSCL_FULLSCREEN|DDSCL_EXCLUSIVE|DDSCL_ALLOWREBOOT;
   // better to make it 640x480 and scale up...
//      if (tt_screen_width <= 320) flags |= DDSCL_ALLOWMODEX;
   HRESULT result = direct_draw->SetCooperativeLevel(tt_main_window->get_handle(),flags);
   tt_exclusive_screen = SUCCEEDED(result);
   if (tt_exclusive_screen) {
		SetErrorMode(SEM_NOOPENFILEERRORBOX); // new on 171201
      tt_restore_exclusive_screen = FALSE; // just done - new on 130502
      // following new on 100602 since when changing mode often can otherwise run full-screen with wrong rectangle
      front_surface_rectangle.left = 0;
      front_surface_rectangle.top = 0;
      front_surface_rectangle.right = tt_screen_width;
      front_surface_rectangle.bottom = tt_screen_height;
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         tt_error_file() << "Front surface rectangle is " << front_surface_rectangle.left << " " << front_surface_rectangle.top 
								 << " " << front_surface_rectangle.right << " " << front_surface_rectangle.bottom << endl;
      };
#endif
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502 || tt_debug_mode == 20105) {
         tt_error_file() << "Got exclusive control." << " (frame=" << tt_frame_number << ")" << endl;
         RECT rectangle;
         GetClientRect(tt_main_window->get_handle(),&rectangle);
         tt_error_file() << "Client rectangle is " << rectangle.left << " " << rectangle.top 
								 << " " << rectangle.right << " " << rectangle.bottom << endl;
      };
#endif
//#if TT_POST324
//		HRGN region = CreateRectRgn(0,0,tt_screen_width,tt_screen_height);
//		if (SetWindowRgn(tt_main_window->get_handle(),region,FALSE) == 0) {
//			tt_error_file() << "Error calling SetWindowRgn. Code = " << GetLastError() << endl;
//		};
//#endif
	} else {
      dderr(result,_T("Setting exclusive screen mode."));
      log(_T("Problems going into full screen mode."),FALSE,TRUE);
//      tt_using_dispdib = 2;
      tt_exclusive_screen_desired = FALSE;
      tt_window_expansion_factor = 2;
      tt_got_exclusive_control_on_frame = -1;
   };
};

void release_exclusive_control() {
   if (direct_draw != NULL) { // condition new on 160502
      dderr(direct_draw->SetCooperativeLevel(tt_main_window->get_handle(),DDSCL_NORMAL),
            _T("Releasing exclusive control."));
   };
   tt_exclusive_screen = FALSE;
   tt_got_exclusive_control_on_frame = -1;
#if TT_DEBUG_ON
   if (tt_debug_mode == 130502 || tt_debug_mode == 51103 || tt_debug_mode == 020105) {
      tt_error_file() << "Released exclusive control. " << "(frame=" << tt_frame_number << ")" << endl;
   };
#endif
};

void restore_display_mode() {
#if TT_DEBUG_ON
   if (tt_debug_mode == 1992 || tt_debug_mode == 130502 || tt_debug_mode == 51103) log(_T("restoring display mode"));
#endif
	release_exclusive_control();
   if (direct_draw != NULL) { // condition new on 160502
      dderr(direct_draw->RestoreDisplayMode(),_T("restoring display mode"));
   };
};

boolean windows95() {
	// GDI+ doesn't work in Windows95 so need to check
	OSVERSIONINFO version_info;
	version_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&version_info);
	return(version_info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS && version_info.dwMinorVersion == 0);
};

boolean windows_2000_or_above() { // mini dumps uses this
	OSVERSIONINFO version_info;
	version_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&version_info);
	return(version_info.dwMajorVersion >= 5);
};

void update_directory_and_extension_since_8bit() { // abstracted on 060702
   // new on 190602 so picture directories customized for 8bit can be used when appropriate
   string better_builtin_picture_directory = process_directory_entry(ini_entry(AC(IDC_DIRECTORIES),"BuiltinPictureDirIf8Bit",FALSE));
   if (better_builtin_picture_directory != NULL) {
#if TT_DEBUG_ON
      tt_error_file() << "Now using " << better_builtin_picture_directory;
      if (tt_builtin_picture_directory != NULL) {
         tt_error_file() << " rather than " << tt_builtin_picture_directory;
      }
      tt_error_file() << " to find built-in artwork." << endl;
#endif
      if (tt_builtin_picture_directory != NULL) delete [] tt_builtin_picture_directory;
      tt_builtin_picture_directory = better_builtin_picture_directory;
   };
   string better_missing_builtin_picture_file_extension = ini_entry("FileExtensions","MissingBuiltinPictureFileExtensionIf8Bit",FALSE);
   if (better_missing_builtin_picture_file_extension != NULL) {
#if TT_DEBUG_ON
      tt_error_file() << "Now using " << better_missing_builtin_picture_file_extension;
      if (tt_missing_builtin_picture_file_extension != NULL) {
         tt_error_file() << " rather than " << tt_missing_builtin_picture_file_extension;
      };
      tt_error_file() << " as the file extension for built-in artwork." << endl;
#endif
      if (tt_missing_builtin_picture_file_extension != NULL) delete [] tt_missing_builtin_picture_file_extension;
      tt_missing_builtin_picture_file_extension = better_missing_builtin_picture_file_extension;
   };
};

boolean change_pixel_depth(int desired_bits_per_pixel) { // abstracted out on 100502
   if (desired_bits_per_pixel == tt_bits_per_pixel) return(TRUE); 
	// no need to do anything -- desired_bits_per_pixel was tt_desired_bits_per_pixel prior to 190602 (but is the same for current calls) 
	switch (desired_bits_per_pixel) {
		case 8:
		case 16:
		case 24:
		case 32:
			tt_bits_per_pixel = desired_bits_per_pixel;
			tt_bytes_per_pixel = tt_bits_per_pixel/8;
			return(TRUE);
		case 15:
			// new on 061101 to indicate that the RGB format is 555 not 565
			tt_bits_per_pixel = 16;
			tt_bytes_per_pixel = 2;
			return(TRUE);
		default:
			// warn if non-zero??
			// do nothing since 0 (or invalid number) is interpreted to mean use the current Windows setting
         return(FALSE);
	};
//		tt_prefer_gdiplus = (tt_bits_per_pixel != 8); 
	// works better for greater than 8-bit graphics (some palette issues) but there is no GDI+ in Win95
};

void pixel_depth_has_changed() {
   if (tt_bits_per_pixel > 8) { // new on 100502 to recompute distribution of red, green, and blue bits
		HDC device_context = GetDC(NULL);
#if TT_DEBUG_ON
      if (tt_debug_mode == 90502) {
         tt_error_file() << "Changing red, green, and blue bits per pixel from " << tt_red_bits_per_pixel 
								 << " " << tt_green_bits_per_pixel << " " << tt_blue_bits_per_pixel << endl;
      };
#endif
		GetRGBBitsPerPixel(device_context,&tt_red_bits_per_pixel,&tt_green_bits_per_pixel,&tt_blue_bits_per_pixel);
#if TT_DEBUG_ON
      if (tt_debug_mode == 90502) {
         tt_error_file() << "To " << tt_red_bits_per_pixel << " " << tt_green_bits_per_pixel << " " << tt_blue_bits_per_pixel << endl;
      };
#endif
      ReleaseDC(NULL,device_context);         
   };
};

void set_display_mode() {
#if TT_DEBUG_ON
	add_to_log_if_interesting_error(0,"before SetDisplayMode");
#endif
   HRESULT result;
   if (tt_screen_width == 320) {
#if TT_DIRECT_DRAW7
      result = direct_draw->SetDisplayMode(tt_screen_width*2,tt_screen_height*2,8,0,0);
#else
		result = direct_draw->SetDisplayMode(tt_screen_width*2,tt_screen_height*2,8);
#endif
      update_directory_and_extension_since_8bit(); // copied here on 101002 since 8-bit
   } else {
		if (windows95()) {
			tt_desired_bits_per_pixel = 8; // since GDI+ doesn't work
			tt_prefer_gdiplus = FALSE;
		};
      change_pixel_depth(tt_desired_bits_per_pixel);
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         tt_error_file() << "Setting display mode with dimensions " << tt_screen_width << " by " << tt_screen_height 
								 << " with pixel depth of " << tt_bits_per_pixel << endl;
      };
#endif
#if TT_DIRECT_DRAW7
      result = direct_draw->SetDisplayMode(tt_screen_width,tt_screen_height,tt_bits_per_pixel,0,0);
#else
		result = direct_draw->SetDisplayMode(tt_screen_width,tt_screen_height,tt_bits_per_pixel);
#endif
		//if (FAILED(result) && tt_screen_height == 960) { // too late to do this here
		//	// new on 141204 since 1280x1024 is common and 1280x960 isn't -- what about aspect ratio?
		//	tt_screen_height = 1024;
		//	result = direct_draw->SetDisplayMode(tt_screen_width,tt_screen_height,tt_bits_per_pixel);
		//};
      if (tt_bits_per_pixel == 8) { // moved here on 101002
		   update_directory_and_extension_since_8bit();
	   };
      pixel_depth_has_changed();
#if TT_DEBUG_ON
	   add_to_log_if_interesting_error(0,"after SetDisplayMode");
#endif
   };
   if (FAILED(result)) {
      log(_T("Problems setting the display mode."),FALSE,TRUE);
//		dderr(result,"Setting the display mode."); // new on 280201 - commented out because this can cause a premature exit when things can be fixed
		tt_error_file() << "Tried to run full-screen " << tt_screen_width << "x" << tt_screen_height << endl;
		if (tt_screen_width > 640) { // new on 190300
			int mode_index = video_mode_index(tt_screen_width);
			if (mode_index > 0) {
				int lower_mode = video_mode_number(mode_index-1);
				int version_default = default_setting("Version");
				char string_lower_default[12];
				itoa(version_default-1,string_lower_default,10);
				write_ini_entry(AC(IDC_DEFAULTS),"Version",string_lower_default);
				char time_string[33];
				ltoa(seconds_since_1970(),time_string,10); // record time so user profiles can be updated
				write_ini_entry(AC(IDC_DEFAULTS),"LastChanged",time_string); 
				// new on 020301 to reset this for all users -- instead of time_string was "" prior to 160301
				char buffer[512];
				output_string_stream stream(buffer,sizeof(buffer));
				stream << S(IDS_RESOLUTION_TOO_HIGH,"This computer can't run at a screen resolution of ") << tt_screen_width << "x" << tt_screen_height
						 << ". " << S(IDS_DEFAULT_RESOLUTION_LOWERED,"Default resolution lowered to ") << lower_mode << "x" << (lower_mode*3)/4
						 << ". " << S(IDS_PLEASE_RESTART_TOONTALK,"Please start ToonTalk again.");
				stream.put('\0');
//				release_direct_draw();
//				pause_toontalk(FALSE,FALSE,TRUE);
				tt_shutting_down = TRUE;
				CloseWindow(tt_main_window->get_handle());
				MessageBox(NULL,buffer,C(IDC_MESSAGE_BOX_TITLE),MB_ICONSTOP|MB_OK|MB_SYSTEMMODAL);
				exit(0);
				// following almost works but size of fonts and objects is off - could fix this sometime...
//				tt_video_mode = video_mode_number(mode_index-1);
//				if (tt_setmode(tt_video_mode,TRUE)) { // this is resetting so should undo some things first
//					tt_error_file() << "Trying again with " << tt_screen_width << "x" << tt_screen_height << endl;
//					set_display_mode();
//					return;
//				} else {
//					tt_video_mode = 640;
//					tt_setmode(tt_video_mode,TRUE); // and the stuff below
//				};
			};
		};
		tt_error_file() << "Running in a large window instead." << endl;
//		tt_using_dispdib = 2;
      tt_exclusive_screen_desired = FALSE;
      tt_window_expansion_factor = 2;
#if TT_DEBUG_ON
   } else if (tt_debug_mode == 130502) {
      tt_error_file() << "Set display mode " << tt_desired_bits_per_pixel << " is the desired bits per pixel. "
                  << tt_bits_per_pixel << " is the bits per pixel. " << endl;
      tt_error_file() << "  Screen size is " << tt_screen_width << "x" << tt_screen_height << endl;
      tt_error_file() << "  Time is " << timeGetTime() << endl;
#endif
   };
};

void MainWindow::initialize_back_buffer() {
   if (tt_page_flipping && back_surface == NULL) {
		static boolean initializing_direct_draw = FALSE; // new on 070404
		if (front_surface == NULL) { // new on 070404 -- not clear how but Lennart had a crash report
			if (initializing_direct_draw) {
				tt_exit_failure(TROUBLE_SHOOT_DIRECTX,"Problem initializing the DirectX back buffer.");
			};
//			boolean initializing_direct_draw = TRUE; // to prevent infinite recursion if things are broken
			initializing_direct_draw = TRUE; // to prevent infinite recursion if things are broken -- fixed on 080904
			initialize_direct_draw();
		};
#if TT_DIRECT_DRAW7
      DDSCAPS2 surface_capability;
#else
		DDSCAPS surface_capability;
#endif
      surface_capability.dwCaps = DDSCAPS_BACKBUFFER;
      HRESULT result = front_surface->GetAttachedSurface(&surface_capability,&back_surface);
#if TT_DEBUG_ON
      if (tt_debug_mode == 170502) {
         tt_error_file() << "Getting attached surface as back surface." << endl;
      };
#endif
      if (SUCCEEDED(result)) {
         if (tt_screen_width == 320 || tt_debug_mode != 1953) {
            true_back_surface = back_surface;
            back_surface = NULL;
         };
      } else {
         dderr(result,_T("Problems creating back flipping surface."));
         back_surface = NULL; // what if something is there??
      };
   };
   if (back_surface == NULL) {
//#if TT_DIRECTX_TRANSFORM
//		DDSURFACEDESC surface_description; 
//		memset(&surface_description,0,sizeof(surface_description));
//		surface_description.dwSize = sizeof(surface_description);
//		surface_description.dwWidth = tt_screen_width;
//		surface_description.dwHeight = tt_screen_height;
////		new_surface_description.ddpfPixelFormat = pixel_format;
//		surface_description.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH; // | DDSD_PIXELFORMAT;
//		HRESULT result = directx_transform_surface_factory->CreateSurface(NULL,&surface_description,&DDPF_RGB8,NULL,0,NULL,IID_IDXSurface,
																								//(void **) &transform_back_surface);
//		if (FAILED(result)) {
//			tt_error_file() << "Failed to create tranform back surface." << endl;
//		} else {
//			result = transform_back_surface->GetDirectDrawSurface(IID_IDirectDrawSurface,(void**) &back_surface);
//		};
//#else
      // create another surface from memory (off-screen)
#if TT_DEBUG_ON
      if (tt_debug_mode == 170502) {
         tt_error_file() << "Creating back surface with dimensions  " << tt_screen_width << " by " << tt_screen_height << endl;
      };
#endif
      back_surface = create_surface(tt_screen_width,tt_screen_height);
      // following needed for blitting
      back_surface_rectangle.left = 0;
      back_surface_rectangle.top = 0;
      back_surface_rectangle.right = tt_screen_width;
      back_surface_rectangle.bottom = tt_screen_height;
#if TT_DEBUG_ON
      if (tt_debug_mode == 170502) {
         tt_error_file() << " Back surface rectangle is 0 0 " << tt_screen_width << " " << tt_screen_height << endl;
      };
#endif
//#endif
	};
   if (back_surface == NULL) {
      tt_exit_failure(TROUBLE_SHOOT_DIRECTX,_T("Couldn't create a back surface."));
	};
};

#if TT_DIRECTX_TRANSFORM
void create_directx_transform_factory() {
	directx_transform_surface_factory_initialized = TRUE; // new on 090201 - just means that we tried
	if (directx_transform_surface_factory == NULL) { // new on 240500
//#if TT_DEBUG_ON
//		tt_error_file() << "About to create DirectX Transform Surface Factory." << endl;
//#endif
		if (!initialize_ole()) { // updated 311204 !tt_coinitialized && FAILED(CoInitialize(NULL))) {
//			tt_error_file() << "Could not CoInitialize. Some features won't work. Sorry." << endl;
			return;
		};
//		tt_coinitialized = TRUE; // commented out 311204
		HRESULT result = CoCreateInstance(CLSID_DXTransformFactory, NULL,
													 CLSCTX_INPROC, IID_IDXSurfaceFactory, (void **) &directx_transform_surface_factory);
		if (FAILED(result)) {
			tt_error_file() << "Failed to create DirectX Transform Surface Factory. Error code = " << (long) result << endl;
			switch (result) {
				case REGDB_E_CLASSNOTREG: 
					tt_error_file() << "A specified class is not registered in the registration database. Also can indicate that the type of server you requested in the CLSCTX enumeration is not registered or the values for the server types in the registry are corrupt." << endl;
					break;
				case CLASS_E_NOAGGREGATION:
					tt_error_file() << "This class cannot be created as part of an aggregate. " << endl;
					break;
				case E_NOINTERFACE: 
					tt_error_file() << "The specified class does not implement the requested interface, or the controlling IUnknown does not expose the requested interface." << endl;
					break;
				default:
					tt_error_file() << "Unknown error." << endl;
			};
			tt_error_file() << "ToonTalk might not be able to deal with non-BMP user pictures. For example, GIF, PNG, or JPG files. Sorry." << endl;
			tt_error_file() << "You can download DirectX Media from Microsoft to fix this. Visit http://www.microsoft.com/directx/homeuser/downloads/default.asp" << endl;
			trouble_shoot(TROUBLE_SHOOT_DIRECTX_MEDIA,NULL,NULL,TRUE,TRUE,FALSE); 
			// new on 100600 - this one is not regardless of trouble shooting mode
//#if TT_DEBUG_ON
//		} else {
//			tt_error_file() << "DirectX Transform Surface Factory created." << endl;
//#endif
		};
	};
};

boolean directx_transform_loaded() { // new on 070201
	if (!directx_transform_surface_factory_initialized) { // new on 090201
		create_directx_transform_factory();
	};
	return(directx_transform_surface_factory != NULL);
};
#else
boolean directx_transform_loaded() { // new on 070201
	return(FALSE);
};
#endif

void MainWindow::initialize_direct_draw(boolean only_enough_for_taking_snapshots) {
#if TT_DEBUG_ON
   if (tt_debug_mode == 130502) {
      tt_error_file() << "initialize_direct_draw: full_screen is " << (int) full_screen 
							 << " while tt_exclusive_screen_desired is " << (int) tt_exclusive_screen_desired
							 << " and tt_exclusive_screen is " << (int) tt_exclusive_screen 
							 << " and tt_restore_exclusive_screen is " << (int) tt_restore_exclusive_screen << endl;
   };
#endif
	if (tt_shutting_down) return; // new on 101202 to deal better with extreme exits (e.g. control-alt-delete)
//   if (direct_draw != NULL) direct_draw->Release();
   HRESULT result;
   if (direct_draw == NULL) { // not re-initializing
#if TT_DIRECT_DRAW7
      result = DirectDrawCreateEx(NULL,(void **) &direct_draw,IID_IDirectDraw7,NULL); 
#else
		result = DirectDrawCreate(NULL,&direct_draw,NULL); 
#endif
		if (FAILED(result) || direct_draw == NULL) { // second test added on 160502
         // message box better??
         dderr(result,_T("Problems initializing DirectDraw. Perhaps things are not installed right.")); 
			// removed  "16 bit version of ToonTalk should still work." on 230500
		   exit(FALSE);
      };
   };
   // prior to 070600 created DirectX transform factory here
//   tt_exclusive_screen = TRUE; // new on 140502 so :set_client_size knows what's going on - is reset if the following fails
	if (only_enough_for_taking_snapshots) {
		release_exclusive_control(); // just to be sure
	} else { 
		if (exclusive_screen_desired()) get_exclusive_control();
		if (!exclusive_screen_desired()) release_exclusive_control(); // Why? Isn't this already done?
		if (tt_exclusive_screen) {
			set_display_mode(); // conditional was tt_using_dispdib == 1 prior to 280201 but there could have been an error getting exclusive control
#if TT_DEBUG_ON
			add_to_log_if_interesting_error(0,"After set_display_mode");
			if (tt_debug_mode == 51001) {
				tt_error_file() << "Display mode set." << endl;
			};
#endif
		} else if (tt_bits_per_pixel == 8) {
			update_directory_and_extension_since_8bit(); // new on 060702 so these are used in window mode
		};
	//   if (front_surface != NULL) {
	//      front_surface->Release();
	//      front_surface = NULL;
	//   };
	};
	// changed on 290303 so the following happens regardless of only_enough_for_taking_snapshots
	if (front_surface == NULL) {
		memset(&surface_description,0,sizeof(surface_description));
		surface_description.dwSize = sizeof(surface_description);
		if (exclusive_screen_desired() && !only_enough_for_taking_snapshots) { // && tt_screen_width != 320) {
			// added !only_enough_for_taking_snapshots on 100603 since snapshots interfered with full screen
			surface_description.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
			surface_description.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
			surface_description.dwBackBufferCount = 1;
		} else {
			surface_description.dwFlags = DDSD_CAPS;
			surface_description.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
			tt_page_flipping = FALSE;
			// couldn't get the following to work
//         memset(&pixel_format,0,sizeof(pixel_format));
//         pixel_format.dwSize = sizeof(DDPIXELFORMAT);
//         pixel_format.dwFlags = DDPF_PALETTEINDEXED8;
//         surface_description.ddpfPixelFormat = pixel_format;
		};
//#if TT_DIRECTX_TRANSFORM
//		HRESULT result = directx_transform_surface_factory->CreateSurface(NULL,&surface_description,&DDPF_RGB8,NULL,0,NULL,IID_IDXSurface,
																								//(void **) &transform_front_surface);
//		if (FAILED(result)) {
//			tt_exit_failure(TROUBLE_SHOOT_DIRECTX,_T("Failed to create tranform front surface."));
//		} else {
//			result = transform_front_surface->GetDirectDrawSurface(IID_IDirectDrawSurface,(void**) &front_surface);
			// check for errors??
//		};
//#else
	//   draw_surface front_surface1; // version 1 interface
		surface_description.dwWidth = tt_screen_width; // new on 140502
		surface_description.dwHeight = tt_screen_height; // new on 140502
#if TT_DEBUG_ON
		if (tt_debug_mode == 170502) {
			tt_error_file() << "Creating front surface with dimensions " << surface_description.dwWidth << " by " 
								 << surface_description.dwHeight << endl;
		};
#endif
		result = direct_draw->CreateSurface(&surface_description,&front_surface,NULL);
#if TT_DEBUG_ON
		add_to_log_if_interesting_error(0,"After CreateSurface of front_surface");
#endif
		if (FAILED(result)) {
			if (exclusive_screen_desired()) { // try again without flipping
//          tt_using_dispdib = 2;
				tt_exclusive_screen_desired = FALSE;
				tt_window_expansion_factor = 2;
				tt_error_file() << "Re-initializing DirectDraw since couldn't create front surface in full-screen mode." << endl;
				dderr(result,_T("Couldn't create a front surface."));
				initialize_direct_draw();
			} else {
				dderr(result,_T("Couldn't create a front surface."));
				tt_exit_failure(TROUBLE_SHOOT_DIRECTX,_T("Couldn't create a front surface."));
//          exit(FALSE);
			};
		};
//#endif
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 51001) {
		tt_error_file() << "Front surface created." << endl;
	};
#endif
//   result = front_surface1->QueryInterface(IID_IDirectDrawSurface2,
//                                           (LPVOID *) front_surface);
//   if (FAILED(result)) {
//      log("Problems upgrading a surface to version 2.");
//      exit(FALSE);
//   };
////   front_surface1->Release();
//   if (back_surface != NULL) {
//      back_surface->Release();
//      back_surface = NULL;
//   };
   initialize_back_buffer();
#if TT_DEBUG_ON
   add_to_log_if_interesting_error(0,"After initialize_back_buffer");
	if (tt_debug_mode == 51001) {
		tt_error_file() << "Back surface created." << endl;
	};
#endif
   if (clipper == NULL) {
      result = direct_draw->CreateClipper(NULL,&clipper,NULL);
      dderr(result,_T("Creating clipper object."));
      clip_structure.region_data_header.dwSize = sizeof(RGNDATAHEADER);
      clip_structure.region_data_header.iType = RDH_RECTANGLES;
      clip_structure.region_data_header.nCount = 1;
      clip_structure.region_data_header.nRgnSize = sizeof(RECT);
      clip_structure.region_data_header.rcBound.left = 0;
      clip_structure.region_data_header.rcBound.top = 0;
      clip_structure.region_data_header.rcBound.right = tt_screen_width;
      clip_structure.region_data_header.rcBound.bottom = tt_screen_height;
      // clip structure is used for the back surface so its rectangle is
      clip_structure.clip_rectangle.left = 0;
      clip_structure.clip_rectangle.top = 0;
      clip_structure.clip_rectangle.right = tt_screen_width;
      clip_structure.clip_rectangle.bottom = tt_screen_height;
      scratch_clip_structure = clip_structure;
      result = clipper->SetClipList((LPRGNDATA) &clip_structure,NULL);
      dderr(result,_T("Initializing clipper object."));
   };
   // following didn't work well -- don't know why
   // seems to use screen coordinates so wasn't any good for back_surface
//   result = clipper->SetHWnd(NULL,tt_main_window->get_handle());
   
// WARNING: don't set break points from here until Unlock
//   result = back_surface->Lock(NULL,&surface_description,0,NULL);
//   if (lock_back_surface()) { // updates surface_description
//      unlock_back_surface();
//   } else {
//      log("Problems locking the back surface.");
//   };
   // initialize the following:
//   tt_destination_width = surface_description.lPitch;
//#if TT_DEBUG_ON
//   tt_error_file() << "Pitch is " <<  tt_destination_width << endl;
//#endif
   if (direct_draw_palette == NULL) {  // added test for 8bit color on 160502 -- removed on 070602 since color permutations broken
      initialize_palette();
#if TT_DEBUG_ON
      add_to_log_if_interesting_error(0,"After initialize_palette");
#endif
      if (tt_bits_per_pixel == 8) {
         if (direct_draw_palette != NULL || !tt_exclusive_screen) return;
         PALETTEENTRY pe[colors_count];
         GetPaletteEntries(tt_main_window->return_palette_handle(),0,colors_count,pe);
         result = direct_draw->CreatePalette(DDPCAPS_8BIT,pe,&direct_draw_palette,NULL);
         if (FAILED(result)) {
            dderr(result,_T("Problems setting direct draw palette."));
         };
      };
#if TT_DEBUG_ON
      add_to_log_if_interesting_error(0,"After direct_draw_palette created");
#endif
   };
#if TT_DEBUG_ON
	if (tt_debug_mode == 51001) {
		tt_error_file() << "Palette initialized." << endl;
	};
#endif
	if (!only_enough_for_taking_snapshots) {
		set_back_and_front_surface_palette();
	};
   // following moved here on 050602 from initialize_palette since that is conditionally called now and the following isn't palette stuff
   default_header->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	default_header->bmiHeader.biWidth = tt_screen_width;
	default_header->bmiHeader.biHeight = tt_screen_height;
	default_header->bmiHeader.biBitCount = (tt_bits_per_pixel==32)?24:tt_bits_per_pixel; 
	// new on 051001 -- if 32 bit then default is 24-bit since there is no 32-bit BMPs
   default_header->bmiHeader.biSizeImage = tt_screen_width*tt_screen_height*default_header->bmiHeader.biBitCount/8; 
	// was tt_bits_per_pixel prior to 051001
	default_header->bmiHeader.biCompression = BI_RGB;
#if TT_DEBUG_ON
	if (tt_debug_mode == 51001) {
		tt_error_file() << "Front and back surface palettes set." << endl;
	};
   add_to_log_if_interesting_error(0,"After MainWindow::initialize_direct_draw");
#endif
};

void set_back_and_front_surface_palette() {
	if (tt_bits_per_pixel != 8) return; // new on 230901
	HRESULT result;
	if (front_surface != NULL) {
		restore_surface(front_surface); // new on 280201 to be sure it there
		result = front_surface->SetPalette(direct_draw_palette);
		if (FAILED(result)) {
			if (result != DDERR_NOPALETTEATTACHED) {
				dderr(result,_T("Problems setting direct draw palette on front surface."));
//			} else {
//				need_to_set_back_and_front_surface_palette = TRUE; // new on 080600
			};
		};
	};
	if (back_surface != NULL) {
		if (!tt_page_flipping) {
			restore_surface(back_surface); // new on 280201 to be sure it there
			result = back_surface->SetPalette(direct_draw_palette);
			if (FAILED(result)) {
				if (result != DDERR_NOPALETTEATTACHED) {
					dderr(result,_T("Problems setting direct draw palette on back surface."));
//				} else {
//					need_to_set_back_and_front_surface_palette = TRUE; // new on 080600
				};
			};
		};
	};
};

boolean flip_to_GDI_surface() {
   if (!tt_page_flipping || tt_shutting_down || direct_draw == NULL) return(TRUE);
   // make sure GDI is "on top" before say putting up a message box
	// added direct_draw == NULL on 130704 since message_box now calls this and it may be too early
   HRESULT result = direct_draw->FlipToGDISurface();
   dderr(result,_T("Problems making GDI use primary surface."));
   return(SUCCEEDED(result));
};

boolean lock_back_surface() {
#if TT_DEBUG_ON
   if (device_context_get_count > 0) {
      log(_T("Trying to lock a device while GetDC."),FALSE,TRUE);
      return(FALSE); // exit?
   };
#endif
   if (lock_back_surface_count > 0) { // nested call
#if TT_DEBUG_ON
      debug_this("Nested call to lock back surface");
#endif
      lock_back_surface_count++;
      return(TRUE);
   };
   if (back_surface == NULL) { // seen this happen when a screen saver interrupts a demo
#if TT_DEBUG_ON
      log(_T("Lost back surface -- re-initializing direct draw..."));
#endif
      tt_main_window->initialize_direct_draw(); // any harm doing this again??
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         log("Initialize direct draw from lock_back_surface.");
      };
#endif
		if (back_surface == NULL) { // new on 101202
			return(FALSE); // or exit or warn?
		};
   };
   HRESULT result = back_surface->Lock(NULL,&surface_description,DDLOCK_WAIT,NULL);
   if (result == DDERR_SURFACELOST) {
      back_surface->Restore();
      result = back_surface->Lock(NULL,&surface_description,DDLOCK_WAIT,NULL);
   };
//   int debug = 0;
//   HRESULT result;
//   while ((result = back_surface->Lock(NULL,&surface_description,0,NULL))
//           == DDERR_WASSTILLDRAWING) {
//      debug++;
//      if (debug > 1000) {
//         log("Gave up waiting for lock.");
//         exit(FALSE);
//      };
//   };
   if (SUCCEEDED(result)) {
      lock_back_surface_count = 1;
      tt_destination_work_page = (LPBYTE) surface_description.lpSurface;
#if TT_DEBUG_ON
		tt_debug_screen_bytes = (LPBYTE) surface_description.lpSurface; // new on 230803 - can't use tt_destination_work_page due to operations below
#endif
//      tt_destination_width = -surface_description.lPitch; // -1 since upside down
		tt_destination_width = -1L*surface_description.dwWidth; // new on 240901 since pitch is in bytes while this is in pixels
      tt_destination_work_page -= tt_destination_width*(tt_destination_height-1)*tt_bytes_per_pixel;
//      if (tt_frame_number < 3) tt_error_file() << "Pitch is " << surface_description.lPitch << endl; // debuggin
//      log("got lock ok");
//      tt_error_file() << "Back surface " << surface_description.lpSurface << " "
//                  << surface_description.dwWidth << "x"
//                  << surface_description.dwHeight
//                  << " frame = " << tt_frame_number << endl;
      return(TRUE);
   } else {
      string problem = _T("Troubles locking the back surface.");
      dderr(result,problem);
      tt_exit_failure(TROUBLE_SHOOT_DIRECTX,problem);
      return(FALSE);
   };
};

boolean lock_front_surface() {
   if (front_surface == NULL) { // added on 070699 since crashed when virtual memory was too low
#if TT_DEBUG_ON
      log(_T("Lost front surface -- re-initializing direct draw..."));
#endif
      tt_main_window->initialize_direct_draw(); // any harm doing this again??
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         log("Initialize direct draw from lock_front_surface.");
      };
#endif
		if (front_surface == NULL) return(FALSE);
   };
   HRESULT result = front_surface->Lock(NULL,&surface_description,DDLOCK_WAIT,NULL);
   if (SUCCEEDED(result)) {
      tt_destination_work_page = (LPBYTE) surface_description.lpSurface;
      tt_destination_width = -surface_description.lPitch; // -1 since upside down
      tt_destination_work_page -= tt_destination_width*(tt_destination_height-1);
//      tt_error_file() << "Front surface pitch is " << surface_description.lPitch << endl; // debuggin
      return(TRUE);
   } else {
      dderr(result,_T("Troubles locking the front surface."));
      return(FALSE); // exit??
   };
};

boolean unlock_back_surface() {
	if (back_surface == NULL) return(FALSE);
   lock_back_surface_count--;
   if (lock_back_surface_count > 0) { // nested call
      return(TRUE);
   };
   HRESULT result = back_surface->Unlock(NULL);
   if (SUCCEEDED(result)) {
      return(TRUE);
   } else {
      switch (result) {
         case DDERR_NOTLOCKED:
            log(_T("Back surface not locked."));
            return(TRUE);
         case DDERR_SURFACELOST:
            log(_T("Back surfce lost"));
            back_surface->Restore();
            result = back_surface->Unlock(NULL);
//            return(unlock_back_surface()); // try again
            return(SUCCEEDED(result));
         case DDERR_INVALIDOBJECT:
            tt_exit_failure(TROUBLE_SHOOT_DIRECTX,_T("back surface become invalid."));
         case DDERR_INVALIDPARAMS:
            tt_exit_failure(TROUBLE_SHOOT_DIRECTX,_T("bad parameters to unlocking back surface"));
         case DDERR_INVALIDRECT:
            tt_exit_failure(TROUBLE_SHOOT_DIRECTX,_T("bad rectangle unlocking back surface"));
         case DDERR_GENERIC:
           tt_exit_failure(TROUBLE_SHOOT_DIRECTX,_T("Troubles unlocking back surface."));
      };
      return(FALSE);
   };
};

boolean unlock_front_surface() {
   HRESULT result = front_surface->Unlock(NULL);
   if (SUCCEEDED(result)) {
      return(TRUE);
   } else {
      switch (result) {
         case DDERR_NOTLOCKED:
            log(_T("Front surface not locked."));
            return(TRUE);
         case DDERR_SURFACELOST:
            log(_T("Front surfce lost"));
            back_surface->Restore();
            result = front_surface->Unlock(NULL);
//            return(unlock_back_surface()); // try again
            return(SUCCEEDED(result));
         case DDERR_INVALIDOBJECT:
            tt_exit_failure(TROUBLE_SHOOT_DIRECTX,_T("Front surface become invalid."));
         case DDERR_INVALIDPARAMS:
            tt_exit_failure(TROUBLE_SHOOT_DIRECTX,_T("Bad parameters to unlocking front surface"));
         case DDERR_INVALIDRECT:
            tt_exit_failure(TROUBLE_SHOOT_DIRECTX,_T("Bad rectangle unlocking front surface"));
         case DDERR_GENERIC:
           tt_exit_failure(TROUBLE_SHOOT_DIRECTX,_T("Troubles unlocking front surface."));
      };
      return(FALSE);
   };
};

boolean get_device_context() {
   if (device_context_get_count > 0) { // nested call
      device_context_get_count++;
      return(TRUE);
   };
   if (back_surface == NULL) { // seen this happen when a screen saver interrupts a demo
#if TT_DEBUG_ON
      log(_T("Lost back surface -- re-initializing direct draw..."));
#endif
		if (tt_main_window == NULL) { // new on 290904 for robustness
			return(FALSE); // too early
		};
      tt_main_window->initialize_direct_draw(); // any harm doing this again??
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         log("Initialize direct draw from get_device_context.");
      };
#endif
   };
//   log("about to get device context");
   HRESULT result = back_surface->GetDC(&main_device_context);
   if (SUCCEEDED(result)) {
//      log("got device context ok");
      device_context_get_count = 1;
      if (GDI_clipping_needed) tt_main_window->clip_now();
//      if (tt_using_dispdib != 1) { // non exclusive
//         if (!SelectPalette(main_device_context,
//                            tt_main_window->return_palette_handle(),FALSE)) {
//            log("Select palette failed.");
//            tt_error_file() << GetLastError();
//         } else {
//            tt_error_file() << // for debuggin
//               RealizePalette(main_device_context) << endl;
//         };
//      };
      return(TRUE);
   } else {
      switch (result) {
         case DDERR_DCALREADYCREATED:
            log(_T("DC already created."));
            break;
         case DDERR_INVALIDOBJECT:
         case DDERR_INVALIDPARAMS:
            log(_T("invalid object or paramters to GetDC"),FALSE,TRUE);
            break;
         case DDERR_INVALIDSURFACETYPE:
            log(_T("Invalid surface type for GetDC"),FALSE,TRUE);
            break;
         case DDERR_SURFACELOST:
            log(_T("Surface lost for GetDC"));
            dderr(back_surface->Restore(),_T("Restoring back surface."));
            result = back_surface->GetDC(&main_device_context);
            if (SUCCEEDED(result)) {
               device_context_get_count = 1;
               if (GDI_clipping_needed) tt_main_window->clip_now();
               return(TRUE);
            } else {
               dderr(result,_T("And couldn't restore the lost surface for GetDC."));
               break;
            };
//            return(get_device_context()); // didn't work
         case DDERR_UNSUPPORTED:
            log(_T("GetDC not surported."),FALSE,TRUE);
            break;
         case DDERR_WASSTILLDRAWING:
            log(_T("Still drawing -- could wait to GetDC"));
            break;
         case DDERR_GENERIC:
            log(_T("Generic GetDC error."),FALSE,TRUE);
            break;
      };
      log(_T("Problems getting device context from surface."),FALSE,TRUE);
      return(FALSE);
   };
};

boolean release_device_context() {
   device_context_get_count--;
   if (device_context_get_count > 0) { // nested call
      return(TRUE);
   };
   if (back_surface != NULL && SUCCEEDED(back_surface->ReleaseDC(main_device_context))) {
//      log("released device context ok");
      main_device_context = NULL;
      return(TRUE);
   } else {
      log(_T("Troubles restoring surface device context."),FALSE,TRUE);
      return(FALSE);
   };
};

void loading_warning(string warning, boolean not_interruptable, boolean initializing_direct_play) { 
	// new on 180105 -- caller should pass a large enough string
	// second arg new on 250105; third arg new on 270205
	if (initializing_direct_play) {
		strcpy(warning,S(IDS_INITIALIZE_DIRECT_PLAY,"Please wait. ToonTalk is setting up network connections for a long-distance bird."));
	} else {
		strcpy(warning,S(IDS_LOAD_WAIT,"Loading, please wait."));
	};
#if TT_LOADING_AND_SAVING_INTERRUPTABLE
	strcat(warning,space());
	if (!not_interruptable && tt_loading_is_ok_to_abort) {
		strcat(warning,S(IDS_PRESS_ESC_TO_INTERRUPT,"Press the Esc button to interrupt.")); // moved here on 250105 to look better
		if (tt_sprites_loaded_count > 0 || tt_bytes_read_from_URL > 0) {
			strcat(warning,space());
			if (tt_bytes_read_from_URL > 0) { // new on 030205
				strcat(warning,S(IDS_BYTES_READ_FROM_URL_PREFIX,""));
				itoa(tt_bytes_read_from_URL,warning+strlen(warning),10); // prior to 040205 this displayed tt_sprites_loaded_count instead
			} else if (tt_sprites_loaded_count > 0) {
				strcat(warning,S(IDS_OBJECTS_LOADED_PREFIX,""));
				itoa(tt_sprites_loaded_count,warning+strlen(warning),10);
			};
			strcat(warning,space());
		};
		if (tt_bytes_read_from_URL > 0) { // new on 290105
			strcat(warning,S(IDS_BYTES_READ_FROM_URL,"bytes read from URL."));
		} else if (tt_sprites_loaded_count > 0) {
			strcat(warning,S(IDS_OBJECTS_LOADED,"objects loaded.")); // IDS_OBJECTS_LOADED new on 270105 (but post 3.144)
		};
	};
#endif
};

boolean flip_surfaces(boolean dont_flip) {
	if (tt_shutting_down) return(FALSE); // new on 101202
	if (!have_focus) return(FALSE); // new on 050605
#if TT_DEBUG_ON
   millisecond start;
   if (tt_debug_mode == 3) {
      start = timeGetTime();
   };
	if (tt_debug_mode == 170502) {
		tt_error_file() << "Flip surfaces called on frame " << tt_frame_number << endl;
		tt_error_file() << "tt_page_flipping is " << (int) tt_page_flipping << " and dont_flip is " << (int) dont_flip << endl;
	};
#endif
//	if (need_to_set_back_and_front_surface_palette) { 
		// new on 080600 to fix palette problems - e.g. after message box while initializing
//		need_to_set_back_and_front_surface_palette = FALSE;
//#if TT_DEBUG_ON
//		log("Resetting palette of front and back surfaces.");
//#endif
//		set_back_and_front_surface_palette();
		// above should work but didn't - neither did the following
//		release_direct_draw(FALSE);
//		tt_main_window->initialize_direct_draw();
//	};
	// new on 290904
	if ((tt_still_frame_count == tt_frame_number || tt_titles_ended_on_frame > tt_frame_number) && tt_system_mode != PUZZLE) {
		// on 141104 added condition so this doesn't happen while puzzle game background story is displayed
		// updated on 180105
//		UnwindProtect<boolean> set(tt_loading_is_not_to_be_aborted,(tt_still_frame_count == tt_frame_number));
		char warning[1024];
		loading_warning((string) &warning,FALSE,FALSE);
//		string warning = S(IDS_LOAD_WAIT,"Loading, please wait.");
//#if TT_LOADING_AND_SAVING_INTERRUPTABLE
//		// new on 150105
//		warning = append_3_strings(warning,space(),S(IDS_PRESS_ESC_TO_INTERRUPT,"Press the Esc button to interrupt."));
//#endif
		//tt_main_window->place_text(warning,strlen(warning),20,40,ideal_screen_width,ideal_screen_height,24,36,
		//									  NEVER_BREAK_SENTENCES,FALSE,FALSE,0,0,LIGHT_RED);
		color_index text_color;
		if (tt_still_frame_count == tt_frame_number) {
			text_color = tt_white;
		} else {
			text_color = tt_black;
		};
		city_coordinate character_width = tile_width/4; // was (3*tile_width)/5;
	   city_coordinate character_height = (3*tile_height)/4;
//		tt_main_window->text_out(warning,strlen(warning),16,24,16,24,TRUE,FALSE,TRUE,text_color);
		tt_screen->text_out(warning,strlen(warning),character_width,character_height,character_width,character_height,TRUE,FALSE,TRUE,text_color);
//#if TT_LOADING_AND_SAVING_INTERRUPTABLE
//		delete [] warning;
//#endif
	};
#if TT_DEBUG_ON
		if (tt_debug_mode == 170502) {
			tt_error_file() << "Added Loading warning on frame " << tt_frame_number << endl;
		};
#endif
   HRESULT result;
   if (!tt_page_flipping || dont_flip) {
    // could check here for surfacelost too
//      result = front_surface->BltFast(client_center_x-client_width/2,
//                                      client_center_y-client_height/2,
//                                      back_surface,NULL,//&back_surface_rectangle,
//                                      DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
      if (front_surface == NULL) {
			if (!tt_shutting_down) { // new on 201299 so aren't bothered if shutting down
				tt_main_window->initialize_direct_draw(); // new on 150500 to try again
#if TT_DEBUG_ON
            if (tt_debug_mode == 130502) {
               log("Initialize direct draw from flip_surfaces.");
            };
#endif
				if (front_surface == NULL) {
					tt_exit_failure(TROUBLE_SHOOT_DIRECTX,_T("DirectX front surface missing."));
				} else {
					return(flip_surfaces(dont_flip)); // new on 150500
				};
			};
         return(FALSE);
      };
//#if TT_DIRECTX_TRANSFORM
//		CDXDVec location(front_surface_rectangle.left, front_surface_rectangle.top, 0, 0);
//		result = directx_transform_surface_factory->BitBlt(transform_front_surface,&location,transform_back_surface,NULL,0);
//		if (FAILED(result)) {
//			tt_error_file() << "Failed to copy back transform surface to front surface." << endl;
//			return(FALSE);
//		};     
//#else
#if TT_DEBUG_ON
		if (tt_debug_mode == 230901) {
			tt_error_file() << "Before blt back to front." << endl;
			bytes surface_bytes;
			if (SUCCEEDED(front_surface->Lock(NULL,&surface_description,DDLOCK_WAIT&DDLOCK_READONLY,NULL))) {
				surface_bytes = (bytes) surface_description.lpSurface;
				tt_error_file() << "Front" << endl;
				for (int i = 0; i < tt_screen_width*tt_bytes_per_pixel; i++) {
					tt_error_file() << (int) surface_bytes[i] << " ";
				};
				tt_error_file() << endl;
				front_surface->Unlock(NULL);
			};
			if (SUCCEEDED(back_surface->Lock(NULL,&surface_description,DDLOCK_WAIT&DDLOCK_READONLY,NULL))) {
				surface_bytes = (bytes) surface_description.lpSurface;
				tt_error_file() << "Back" << endl;
				for (int i = 0; i < tt_screen_width*tt_bytes_per_pixel; i++) {
					tt_error_file() << (int) surface_bytes[i] << " ";
				};
				tt_error_file() << endl;
				back_surface->Unlock(NULL);
			};
		};
      if (tt_debug_mode == 211102) {
         tt_error_file() << "Blt top " << front_surface_rectangle.top << " bottom " << front_surface_rectangle.bottom 
								 << " left " << front_surface_rectangle.left << " right " << front_surface_rectangle.right << endl;
      };
#endif
//#if TT_DEBUG_ON
//      if (tt_debug_mode == 130502) {
//         tt_error_file() << "Bltting front to back with rectangle " << front_surface_rectangle.left << " " << front_surface_rectangle.top
//                     << " " << front_surface_rectangle.right << " " << front_surface_rectangle.bottom << endl;
//      };
//#endif
      result = front_surface->Blt(&front_surface_rectangle,back_surface,NULL,DDBLT_WAIT&DDLOCK_READONLY,NULL);
      if (result == DDERR_SURFACELOST) {
         front_surface->Restore();
         back_surface->Restore(); // necessary??
         result = front_surface->Blt(&front_surface_rectangle,back_surface,NULL,DDBLT_WAIT,NULL);
      } else if (result == DDERR_INVALIDRECT) { // new on 151099
			tt_main_window->set_client_size(client_width,client_height);
			result = front_surface->Blt(&front_surface_rectangle,back_surface,NULL,DDBLT_WAIT,NULL);
		};
//#endif
      if (SUCCEEDED(result)) {
#if TT_DEBUG_ON
			if (tt_debug_mode == 230901) {
				tt_error_file() << "After blt back to front." << endl;
				bytes surface_bytes;
				if (SUCCEEDED(front_surface->Lock(NULL,&surface_description,DDLOCK_WAIT&DDLOCK_READONLY,NULL))) {
					surface_bytes = (bytes) surface_description.lpSurface;
					tt_error_file() << "Front" << endl;
					for (int i = 0; i < tt_screen_width*tt_bytes_per_pixel; i++) {
						tt_error_file() << (int) surface_bytes[i] << " ";
					};
					tt_error_file() << endl;
					front_surface->Unlock(NULL);
				};
				if (SUCCEEDED(back_surface->Lock(NULL,&surface_description,DDLOCK_WAIT&DDLOCK_READONLY,NULL))) {
					surface_bytes = (bytes) surface_description.lpSurface;
					tt_error_file() << "Back" << endl;
					for (int i = 0; i < tt_screen_width*tt_bytes_per_pixel; i++) {
						tt_error_file() << (int) surface_bytes[i] << " ";
					};
					tt_error_file() << endl;
					back_surface->Unlock(NULL);
				};
			};
#endif
#if TT_DEBUG_ON
         if (tt_debug_mode == 3) {
            tt_error_file() << (timeGetTime()-start) << " ms to blt entire surface." << endl;
         };
#endif
         return(TRUE);
      } else {
         dderr(result,_T("Problems bltting from back surface to front surface."));
         return(FALSE);
      };
   };
   // else flip but if 320x240 scale first
   if (tt_debug_mode != 1953 || tt_screen_width == 320) {
		if (true_back_surface == NULL) {
			tt_main_window->initialize_back_buffer(); // new on 050304 for robustness
		};
		if (true_back_surface == NULL) {
			return(FALSE);  // new on 050304 for robustness
		};
      result = true_back_surface->Blt(&front_surface_rectangle,back_surface,
                                      &back_surface_rectangle,DDBLT_WAIT,NULL);
      if (SUCCEEDED(result)) {
#if TT_DEBUG_ON
         if (tt_debug_mode == 3) {
            tt_error_file() << (timeGetTime()-start) << " ms to expand entire surface." << endl;
            start = timeGetTime();
         };
#endif
//       } else if (result == DDERR_SURFACELOST)  {
//          result = true_back_surface->Restore();
//       };
       } else {
          dderr(result,_T("trying to expand back buffer to full size"));
          tt_main_window->initialize_back_buffer(); // try to fix things
          // and try again
          true_back_surface->Blt(&front_surface_rectangle,back_surface,
                                 &back_surface_rectangle,DDBLT_WAIT,NULL);
       };
   };
   while(TRUE)  {
       result = front_surface->Flip(NULL,0);
       if (SUCCEEDED(result) ) break;
       if (result == DDERR_SURFACELOST) {
          result = front_surface->Restore();
          if (FAILED(result)) break;
       }
       if (result != DDERR_WASSTILLDRAWING) break;
   };
   if (SUCCEEDED(result)) {
#if TT_DEBUG_ON
         if (tt_debug_mode == 3) {
            tt_error_file() << (timeGetTime()-start) << "ms to flip surfaces." << endl;
         };
#endif
      return(TRUE);
   } else {
      log(_T("Problems flipping from back surface to front surface."),FALSE,TRUE);
      return(FALSE);
   };
};

boolean blt_full_surface(draw_surface source, draw_surface destination) {
   if (destination == NULL) {
      destination = back_surface;
      if (destination == NULL) return(FALSE);
   };         
   HRESULT result = destination->BltFast(0,0,source,NULL,DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
   if (result == DDERR_SURFACELOST) {
      destination->Restore();
      source->Restore();
      result = destination->BltFast(0,0,source,NULL,DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
   };
   dderr(result,_T("Bltting the front surface to the back."));
   return(SUCCEEDED(result));
};

void set_transparent_color(draw_surface surface, color transparent_color) {
	// new on 310101
   DDCOLORKEY color_key;
	color_key.dwColorSpaceLowValue = transparent_color;  
	color_key.dwColorSpaceHighValue = transparent_color;
   dderr(surface->SetColorKey(DDCKEY_SRCBLT,&color_key),_T("setting the color key"));
};

void set_transparent_color_index(draw_surface surface, color_index transparent_color_index) {
	if (tt_bits_per_pixel == 8) { // conditional new on 301001
		set_transparent_color(surface,transparent_color_index);  // these were 0 prior to 060600 and still are by default
	} else {
		set_transparent_color(surface,tt_colors[transparent_color_index]);  // added tt_colors[] on 101001 - restored on 251001
	};
};

#if TT_DEBUG_ON
int surface_count = 0;
#endif 

draw_surface create_surface(coordinate pixel_width, coordinate pixel_height, int transparent_color) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 190301) {
		tt_error_file() << "create_surface call count is " << ++surface_count << endl;
	};
#endif
   surface_description.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
   // easier to debug but typically slower -- but not always???
   // but without DDSCAPS_SYSTEMMEMORY crashed on some systems -- others have reported the same problem on DirectX email list
   //if (tt_debug_mode == 9876) {
   //   // added 2nd and 3rd flag on 300402
   //   surface_description.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY; //  | DDSCAPS_NONLOCALVIDMEM;
   //} else {
      // for these I need to check IsLost and restore
      surface_description.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
   //};
   surface_description.dwWidth = pixel_width;
   surface_description.dwHeight = pixel_height;
//      draw_surface back_surface1; // version 1 interface
   draw_surface surface;
	if (direct_draw == NULL) { // new on 241001 - happens when replaying demos
		tt_main_window->initialize_direct_draw();
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         log("Initialize direct draw from create_surface.");
      };
#endif
	};
#if TT_DEBUG_ON
	MEMORYSTATUS memory_status;
	long memory_available_before;
   memory_status.dwLength = sizeof(MEMORYSTATUS);
	if (tt_debug_mode == 280204) {
		GlobalMemoryStatus(&memory_status);
	   memory_available_before = memory_status.dwAvailPageFile;
	};
#endif
   HRESULT result = direct_draw->CreateSurface(&surface_description,&surface,NULL);
#if TT_DEBUG_ON
	if (tt_debug_mode == 280204) {
		GlobalMemoryStatus(&memory_status);
		long memory_available_after = memory_status.dwAvailPageFile;
		tt_error_file() << "create_draw_surface " << pixel_width << "x" << pixel_height
						<< " and available memory shrank by " << (memory_available_before-memory_available_after) << endl;
	};
#endif
   if (FAILED(result)) {
		if (result == DDERR_OUTOFMEMORY) {
			tt_error_file() << "Creating a picture that should be " << surface_description.dwWidth << " by " 
								 << surface_description.dwHeight << " but there isn't enough memory so it'll be invisible. Sorry." << endl;
			return(NULL);
		};
//		while (result == DDERR_OUTOFMEMORY && tt_cache_percent > 0) { // new on 210301
//			log("Need more memory to create a DirectDraw surface so reducing cache size.");
//			unsigned long memory_before = compute_memory_available();
//			out_of_heap(0); // remove some stuff from the cache and try again
//			if (memory_before-compute_memory_available() <= surface_description.dwWidth*surface_description.dwHeight) { // new on 010401
				// didn't recover enough
//			tt_error_file() << "Creating a picture that should be " << surface_description.dwWidth << " by " 
								 //<< surface_description.dwHeight << " but there isn't enough memory so it'll be smaller. Sorry." << endl;
//			for (int i = 0; i < 10; i++) { // cut size in half up to ten times
//				surface_description.dwWidth /= 2;
//				surface_description.dwHeight /= 2;
//				result = direct_draw->CreateSurface(&surface_description,&surface,NULL);
//				if (SUCCEEDED(result)) break;
//			};
//		};
//		if (FAILED(result)) { // still is
			dderr(result,_T("Problems creating an off screen surface."));
//		};
//      } else {
//         back_surface1->QueryInterface(IID_IDirectDrawSurface2,
//                                       (LPVOID *) back_surface);
   };
//      back_surface1->Release();
   // default color key is 0
	if (surface == NULL) return(NULL); // added on 160399 since while debugging this came up
//	if (tt_prefer_gdiplus) transparent_color = 0; // test on 251001
	set_transparent_color(surface,transparent_color); // was set_transparent_color_index prior to 021101
   DDBLTFX blit_fx;
   blit_fx.dwSize = sizeof(DDBLTFX);
   blit_fx.dwFillColor = transparent_color; // was 0 prior to 041101
   dderr(surface->Blt(NULL,NULL,NULL,DDBLT_COLORFILL,&blit_fx),_T("setting the surface black"));
   return(surface);
};

boolean lock_surface(draw_surface surface) {
   HRESULT result = surface->Lock(NULL,&surface_description,DDLOCK_WAIT,NULL);
   if (SUCCEEDED(result)) {
      tt_destination_work_page = (LPBYTE) surface_description.lpSurface;
      tt_destination_width = -surface_description.lPitch; // -1 since upside down
      tt_destination_height = surface_description.dwHeight;
      tt_destination_work_page -= tt_destination_width*(tt_destination_height-1);
      return(TRUE);
   } else {
      dderr(result,_T("Troubles locking a surface."));
      return(FALSE);
   };
};

boolean local_lock_surface(draw_surface surface, bytes &surface_bytes, int &pitch, int &bits_per_pixel, int &width, int &height) {
	if (surface == NULL) return(FALSE);
   HRESULT result = surface->Lock(NULL,&surface_description,DDLOCK_WAIT,NULL);
   if (SUCCEEDED(result)) {
      surface_bytes = (bytes) surface_description.lpSurface;
      pitch = surface_description.lPitch;
      surface_bytes += pitch*(surface_description.dwHeight-1);
		width = surface_description.dwWidth;
		height = surface_description.dwHeight;
		if (surface_description.dwFlags&DDSD_PIXELFORMAT) { // new on 230901 
			bits_per_pixel = surface_description.ddpfPixelFormat.dwRGBBitCount;
		} else { //  can it be otherwise?
			bits_per_pixel = 8;
		};
      return(TRUE);
   } else {
      dderr(result,_T("Troubles locking a surface."));
      return(FALSE);
   };
};

boolean just_lock_surface(draw_surface surface, bytes &surface_bytes, int &pitch, int &bits_per_pixel, int &width, int &height) {
	if (surface == NULL) return(FALSE);
   HRESULT result = surface->Lock(NULL,&surface_description,DDLOCK_WAIT,NULL);
   if (SUCCEEDED(result)) {
      surface_bytes = (bytes) surface_description.lpSurface;
		pitch = surface_description.lPitch;
		width = surface_description.dwWidth;
		height = surface_description.dwHeight;
		if (surface_description.dwFlags&DDSD_PIXELFORMAT) { // new on 230901 
			bits_per_pixel = surface_description.ddpfPixelFormat.dwRGBBitCount;
		} else { //  can it be otherwise?
			bits_per_pixel = 8;
		};
      return(TRUE);
   } else {
      dderr(result,_T("Troubles locking a surface."));
      return(FALSE);
   };
};

boolean unlock_surface(draw_surface surface) {
	if (surface == NULL) return(FALSE);
   HRESULT result = surface->Unlock(NULL);
   if (SUCCEEDED(result)) {
      return(TRUE);
   } else {
      switch (result) {
         case DDERR_NOTLOCKED:
            return(TRUE);
         case DDERR_SURFACELOST:
            log(_T("Back surfce lost"));
            surface->Restore();
            return(unlock_surface(surface)); // try again
         default:
            dderr(result,_T("Troubles unlocking a surface."));
            return(FALSE);
      };
   };
};

void blt_to_back_surface(draw_surface surface, RECT &source_rectangle,
                         boolean transparent) {
	if (back_surface == NULL || surface == NULL) return;
   coordinate width = source_rectangle.right-source_rectangle.left;
   coordinate height = source_rectangle.bottom-source_rectangle.top;
   coordinate ulx = tt_memory_graphics_mark_x;
   coordinate uly = tt_screen_height-(tt_memory_graphics_mark_y+height+1); //+1 new on 121204 (issue 587)
	if (tt_screen_height == height) { 
		// new on 161204 since otherwise crud accumulates on the bottom of the screen due to the fix above
		uly++;
	};
	if (ulx < tt_destination_llx) {
      if (ulx+width < tt_destination_llx) return; // off the left side
      source_rectangle.left = tt_destination_llx-ulx;  // clip left side
      width -= tt_destination_llx-ulx;
      if (width < 1) return;
      ulx = tt_destination_llx;
   };
   if (ulx >= tt_destination_urx) return; // off the right side
   coordinate off_the_right = ulx+width-tt_destination_urx; // added +1 to tt_destination_urx on 290302 
	// -- removed on 150702 since caused invalid rectangle errors
   if (off_the_right > 0) {
      if (width <= off_the_right) return;
      source_rectangle.right -= off_the_right; // clip right side
   };
   if (source_rectangle.right-source_rectangle.left <= 0) return;
   coordinate destination_ury = tt_screen_height-tt_destination_ury; 
	// added +1 on 290302 -- removed on 150702 since caused invalid rectangle errors
   if (uly+height < destination_ury) return; // off the top
   if (uly < destination_ury) {
      source_rectangle.top = destination_ury-uly; // clip top
      height -= destination_ury-uly;
      if (height < 1) return;
      uly = destination_ury;
   };
   coordinate destination_lly = tt_screen_height-tt_destination_lly;
   if (uly+height > destination_lly) {
      source_rectangle.bottom -= uly+height-destination_lly;
      // really? try removing it
//      source_rectangle.bottom--; // seems to fix an invalid rectangle problem -- commented out on 180702 since handled better elsewhere
		if (source_rectangle.bottom < 1) { // was <= prior to 161204
			return;
		};
      if (source_rectangle.bottom-source_rectangle.top <= 0) return;
   };
#if TT_DEBUG_ON
	if (tt_debug_mode == 230901) {
		tt_error_file() << "Before blt to back." << endl;
		bytes surface_bytes;
		if (SUCCEEDED(surface->Lock(NULL,&surface_description,DDLOCK_WAIT&DDLOCK_READONLY,NULL))) {
			surface_bytes = (bytes) surface_description.lpSurface;
			tt_error_file() << "Source" << endl;
			for (int i = 0; i < width*tt_bytes_per_pixel; i++) {
				tt_error_file() << (int) surface_bytes[i] << " ";
			};
			tt_error_file() << endl;
			surface->Unlock(NULL);
		};
		if (SUCCEEDED(back_surface->Lock(NULL,&surface_description,DDLOCK_WAIT&DDLOCK_READONLY,NULL))) {
			surface_bytes = (bytes) surface_description.lpSurface;
			tt_error_file() << "Back" << endl;
			for (int i = 0; i < width*tt_bytes_per_pixel; i++) {
				tt_error_file() << (int) surface_bytes[i] << " ";
			};
			tt_error_file() << endl;
			back_surface->Unlock(NULL);
		};
	};
   if (tt_debug_mode == 211102) {
      tt_error_file() << "BltFast " << ulx << "," << uly << " from top " << source_rectangle.top << " bottom " << source_rectangle.bottom 
                  << " left " << source_rectangle.left << " right " << source_rectangle.right << endl;
   };
#endif
   HRESULT result = back_surface->BltFast(ulx,uly,
														surface,&source_rectangle,
														transparent?(DDBLTFAST_SRCCOLORKEY|DDBLTFAST_WAIT):
																		(DDBLTFAST_NOCOLORKEY|DDBLTFAST_WAIT));
#if TT_DEBUG_ON
	if (tt_debug_mode == 230901) {
		tt_error_file() << "After blt to back. " << ulx << "," << uly << endl;
		bytes surface_bytes;
		if (SUCCEEDED(surface->Lock(NULL,&surface_description,DDLOCK_WAIT&DDLOCK_READONLY,NULL))) {
			surface_bytes = (bytes) surface_description.lpSurface;
			tt_error_file() << "Source" << endl;
			for (int i = 0; i < width*tt_bytes_per_pixel; i++) {
				tt_error_file() << (int) surface_bytes[i] << " ";
			};
			tt_error_file() << endl;
			surface->Unlock(NULL);
		};
		if (SUCCEEDED(back_surface->Lock(NULL,&surface_description,DDLOCK_WAIT&DDLOCK_READONLY,NULL))) {
			surface_bytes = (bytes) surface_description.lpSurface;
			tt_error_file() << "Back" << endl;
			for (int i = 0; i < width*tt_bytes_per_pixel; i++) {
				tt_error_file() << (int) surface_bytes[i] << " ";
			};
			tt_error_file() << endl;
			back_surface->Unlock(NULL);
		};
	};
#endif
   if (result == DDERR_SURFACELOST) {
      HRESULT restore_result = back_surface->Restore();
      restore_result = surface->Restore(); // necessary??
#if TT_DEBUG_ON
      if (tt_debug_mode == 150702) {   
         tt_error_file() << "Restored backsurface in order to run BltFast." << endl;
      };
      if (tt_debug_mode == 211102) {
         tt_error_file() << "BltFast " << ulx << "," << uly << " from top " << source_rectangle.top << " bottom " << source_rectangle.bottom 
                     << " left " << source_rectangle.left << " right " << source_rectangle.right << " after surface lost." << endl;
      };
#endif
      result = back_surface->BltFast(ulx,uly,
												 surface,&source_rectangle,
												 transparent?(DDBLTFAST_SRCCOLORKEY|DDBLTFAST_WAIT):
																 (DDBLTFAST_NOCOLORKEY|DDBLTFAST_WAIT));
   };
/*
   RECT destination_rectangle;
   destination_rectangle.left = tt_memory_graphics_mark_x;
	destination_rectangle.top = tt_screen_height-tt_memory_graphics_mark_y;
	destination_rectangle.right
     = min(tt_memory_graphics_mark_x+source_rectangle.right-source_rectangle.left,
           tt_screen_width);
	destination_rectangle.bottom
     = destination_rectangle.top+source_rectangle.bottom-source_rectangle.top;
   HRESULT result
        = back_surface->Blt(&destination_rectangle,
                            surface,&source_rectangle,
                            transparent?(DDBLT_KEYSRC|DDBLT_WAIT):DDBLT_KEYSRC,
                            NULL);
*/
   dderr(result,_T("Bltting a surface to the back."));
};

void stretch_blt_to_back_surface(draw_surface surface,
                                 RECT &source_rectangle, RECT destination_rectangle,
                                 boolean transparent) {
	if (back_surface == NULL || surface == NULL) return;
#if TT_DEBUG_ON
	if (destination_rectangle.top < 0) {
		log("debug negative top of destination rectangle.");
	};
#endif
/*
   coordinate width = source_rectangle.right-source_rectangle.left;
   coordinate height = source_rectangle.bottom-source_rectangle.top;
   coordinate ulx = tt_memory_graphics_mark_x;
   coordinate uly = tt_screen_height-(tt_memory_graphics_mark_y+height);
   if (ulx < 0) {
      if (ulx < -width) return; // off the left side
      source_rectangle.left = -ulx;  // clip left side
      ulx = 0;
   };
   if (ulx >= tt_screen_width) return; // off the right side
   coordinate off_the_right = ulx+width-tt_screen_width;
   if (off_the_right > 0) {
      source_rectangle.right -= off_the_right; // clip right side
   };
   if (uly+height < 0) return; // off the top
   if (uly < 0) {
      source_rectangle.top = -uly; // clip top
      uly = 0;
   };
   if (uly+height > tt_screen_height) {
      source_rectangle.bottom -= uly+height-tt_screen_height;
      if (source_rectangle.bottom <= 1) return;
   };
*/
//   HRESULT result = clipper->SetHWnd(NULL,tt_main_window->get_handle());
//   dderr(result,"clipping to main window");
   if (GDI_clipping_needed) {
      scratch_clip_structure.clip_rectangle.left = tt_destination_llx;
      scratch_clip_structure.clip_rectangle.top = tt_screen_height-(tt_destination_ury+1);
      scratch_clip_structure.clip_rectangle.right = 1+tt_destination_urx;
      scratch_clip_structure.clip_rectangle.bottom = tt_screen_height-tt_destination_lly;
      dderr(clipper->SetClipList((LPRGNDATA) &scratch_clip_structure,NULL),_T("Setting clip list of clipper object."));
   };
   HRESULT result = back_surface->SetClipper(clipper);
   dderr(result,_T("Setting clipper of back surface."));
#if TT_DEBUG_ON
   if (tt_debug_mode == 211102) {
      tt_error_file() << "Blt source top " << source_rectangle.top << " bottom " << source_rectangle.bottom 
							 << " left " << source_rectangle.left << " right " << source_rectangle.right
							 << " destination top " << destination_rectangle.top << " bottom " << destination_rectangle.bottom 
							 << " left " << destination_rectangle.left << " right " << destination_rectangle.right << endl;
   };
#endif
   result = back_surface->Blt(&destination_rectangle,
                              surface,&source_rectangle,
                              transparent?(DDBLT_KEYSRC|DDBLT_WAIT):DDBLT_WAIT,
                              NULL);
   if (result == DDERR_SURFACELOST) {
      back_surface->Restore();
      surface->Restore(); // necessary??
      result = back_surface->Blt(&destination_rectangle,
                                 surface,&source_rectangle,
                                 transparent?(DDBLT_KEYSRC|DDBLT_WAIT):DDBLT_WAIT,
                                 NULL);
   };
//#if TT_DIRECTX_TRANSFORM
//	if (result == DDERR_INVALIDOBJECT) { // new on 250500
//		// for now recover by assuming that surface is a transform_surface
//		CDXDVec destination_corner(destination_rectangle.left, destination_rectangle.top, 0, 0);
//		CDXDBnds source_corner(source_rectangle.right, source_rectangle.bottom);
//		result = directx_transform_surface_factory->BitBlt(transform_back_surface,&destination_corner,(transform_surface) surface,&source_corner,0);
//		if (FAILED(result)) {
//			tt_error_file() << "Failed to copy transform surface to back surface." << endl;
//		};     
//	};
//#endif
   back_surface->SetClipper(NULL);
/*
   RECT destination_rectangle;
   destination_rectangle.left = tt_memory_graphics_mark_x;
	destination_rectangle.top = tt_screen_height-tt_memory_graphics_mark_y;
	destination_rectangle.right
     = min(tt_memory_graphics_mark_x+source_rectangle.right-source_rectangle.left,
           tt_screen_width);
	destination_rectangle.bottom
     = destination_rectangle.top+source_rectangle.bottom-source_rectangle.top;
   HRESULT result
        = back_surface->Blt(&destination_rectangle,
                            surface,&source_rectangle,
                            transparent?(DDBLT_KEYSRC|DDBLT_WAIT):DDBLT_KEYSRC,
                            NULL);
*/
   dderr(result,_T("Stretch bltting a surface to the back."));
};

boolean stretch_blt_to_another_surface(draw_surface source,
                                       RECT &source_rectangle,
                                       draw_surface destination,
                                       RECT destination_rectangle,
                                       boolean transparent) {
	if (destination == NULL || source == NULL) return(FALSE);
#if TT_DEBUG_ON
   if (tt_debug_mode == 211102) {
      tt_error_file() << "Blt source top " << source_rectangle.top << " bottom " << source_rectangle.bottom 
                  << " left " << source_rectangle.left << " right " << source_rectangle.right
                  << " destination top " << destination_rectangle.top << " bottom " << destination_rectangle.bottom 
                  << " left " << destination_rectangle.left << " right " << destination_rectangle.right 
						<< " in stretch_blt_to_another_surface" << endl;
   };
#endif
   HRESULT result = destination->Blt(&destination_rectangle,
                                     source,&source_rectangle,
                                     transparent?(DDBLT_KEYSRC|DDBLT_WAIT):DDBLT_WAIT,
                                     NULL);
   if (result == DDERR_SURFACELOST) {
      destination->Restore();
      source->Restore(); // necessary??
      result = destination->Blt(&destination_rectangle,
                                source,&source_rectangle,
                                transparent?(DDBLT_KEYSRC|DDBLT_WAIT):DDBLT_WAIT,
                                NULL);
   };
   dderr(result,_T("Stretch bltting a surface to another."));
   return(SUCCEEDED(result));
};

void release_direct_draw(boolean direct_draw_too) {
//   unlock_back_surface();
//   release_device_context();
   if (back_surface != NULL) {
      back_surface->Release();
      back_surface = NULL;
   };
   if (true_back_surface != NULL) {
      true_back_surface->Release();
      true_back_surface = NULL;
   };
   if (front_surface != NULL) {
      front_surface->Release();
      front_surface = NULL;
   };
   if (!tt_shutting_down) {
		if (tt_deleted_memory != NULL) { // new on 300105
			delete tt_deleted_memory;
			tt_deleted_memory = NULL;
		};
      free_surfaces_in_cache();
   };
   if (direct_draw != NULL && direct_draw_too) {
      if (direct_draw_palette != NULL) {
         dderr(direct_draw_palette->Release(),_T("releasing direct draw palette."));
         direct_draw_palette = NULL;
      };
      if (clipper != NULL) {
         dderr(clipper->Release(),_T("releasing direct draw clipper."));
         clipper = NULL;
      };
      dderr(direct_draw->Release(),_T("releasing direct draw."));
      direct_draw = NULL;
   };
//   if (clip_region_data_memory != NULL) {
//      delete [] clip_region_data_memory;
//   };
//   direct_draw_palette->Release();
};

#endif

bytes DIB_bytes(LPBITMAPINFOHEADER dib) {
//  if (dib->biClrUsed == 2) {
//     return((bytes) dib+dib->biSize); // there are 2 colors in table
//  } else { // wing utils does it fine
     return((bytes) DibPtr(dib));
//  };
};

void display_on_page(work_page page, LPBITMAPINFOHEADER dib, DWORD SizeImage) { //, draw_surface mask_surface) {
	if (dib == NULL) return; // e.g. file was deleted while this was running -- warn??
   if (dib->biCompression == BI_RLE8) {
       // for some reason the GDI screws up on RLE8 and RLE4 and 1 bit files
       DibMapToPalette(dib,tt_main_window->return_palette_handle());
       int pitch = dib->biWidth;
       if (pitch&3) pitch = pitch+4-pitch%4;
       bytes_from_RLE8(DIB_bytes(dib),dib->biWidth,pitch,page);
       return;
   } else if (dib->biCompression == BI_RLE4) {
       bytes_from_RLE4(DIB_bytes(dib),dib->biWidth,page);
       return;
//   } else if (dib->biClrUsed == 2) {
//       bytes_from_1bit(DIB_bytes(dib),dib->biWidth,dib->biSizeImage,
//                       tt_black,tt_white,page);
//       return;
   };
   // otherwise let the GDI take care of it
   int height = dib->biHeight;
   dib->biHeight *= -1;
   HDC window_device_context = tt_main_window->get_windows_device_context();
   HBITMAP bitmap_of_dib = CreateDIBitmap(window_device_context,dib,CBM_INIT,
													   DIB_bytes(dib),
														(LPBITMAPINFO) dib,
														DIB_RGB_COLORS); // always??
#if TT_32
//    int header_size = DibPaletteSize(dib)+dib->biSize;
	int header_size;
	if (tt_bits_per_pixel == 8) {
		header_size = dib_header_size;
	} else { // new on 230901 since don't need palette if > 8 bits
		 header_size = dib_header_size_no_palette;
	 };
    LPBITMAPINFO temp_header = (LPBITMAPINFO) new unsigned char[header_size];
    memcpy(temp_header,default_header,header_size);
    temp_header->bmiHeader.biWidth = dib->biWidth;
    // otherwise they come out upside down
    temp_header->bmiHeader.biHeight = dib->biHeight;
    temp_header->bmiHeader.biSizeImage = SizeImage;
	 temp_header->bmiHeader.biBitCount = tt_bits_per_pixel; // new on 230901
//tt_error_file() <<
    int result = GetDIBits(window_device_context,bitmap_of_dib,0,height,page,temp_header,DIB_RGB_COLORS);
#if TT_DEBUG_ON
	 if (tt_debug_mode == 230901) {
			tt_error_file() << "Display on page. Lines=" << result << endl;
			tt_error_file() << "DIB bytes:  ";
			bytes dib_bytes = DIB_bytes(dib);
			int i;
			int bytes_per_pixel = dib->biBitCount/8;
			for (i = 0; i < dib->biWidth; i++) {
				tt_error_file() << i << ": ";
				for (int j = 0; j < bytes_per_pixel; j++) {
					tt_error_file() << (int) dib_bytes[i*bytes_per_pixel+j] << " ";
				};
				tt_error_file() << "   ";
			};
			tt_error_file() << endl;
			tt_error_file() << "Page bytes: ";
			bytes_per_pixel = temp_header->bmiHeader.biBitCount/8;
			for (i = 0; i < dib->biWidth; i++) {
				tt_error_file() << i << ": ";
				for (int j = 0; j < bytes_per_pixel; j++) {
					tt_error_file() << (int) page[i*bytes_per_pixel+j] << " ";
				};
				tt_error_file() << "   ";
			};
			tt_error_file() << endl;
		};
#endif
    delete [] (bytes) temp_header;
#else
   HDC memory_device_context = CreateCompatibleDC(window_device_context);
//   HBITMAP old_bitmap =
   SelectObject(memory_device_context,bitmap_of_dib);
   GetBitmapBits(bitmap_of_dib,SizeImage,page);
   DeleteDC(memory_device_context);
#endif
//   bytes line = new unsigned char[dib->biWidth];
//   for (int i = 0; i < dib->biHeight; i++) {
//      memcpy(
   DeleteObject(bitmap_of_dib);
	tt_main_window->release_windows_device_context(window_device_context); // added on 061101
//   return(TRUE);
};

/*
boolean display_on_page(work_page page, LPBITMAPINFOHEADER dib, DWORD SizeImage) {
   DibMapToPalette(dib,tt_main_window->return_palette_handle());
//   int source_width = dib->biWidth;
//   int width_with_padding = source_width;
//   if (source_width&3) { // low order bits set
//		width_with_padding += 4-source_width%4; // adjust to long boundary
//	};
//   bytes new_bytes = DIB_bytes(dib);
//   for (int i = 0; i < dib->biHeight; i++) {
//      memcpy(page,new_bytes,source_width);
//      new_bytes += width_with_padding;
//   };
   HMEMCPY(page,DIB_bytes(dib),SizeImage);
//  HMEMCPY(page,bits_of_dib(dib),pixel_width*pixel_height);
   return(TRUE);
};
*/
/*
int  XDest = 0;	// x-coordinate of upper-left corner of dest. rect.
int  YDest = 0;	// y-coordinate of upper-left corner of dest. rect.
DWORD  dwWidth = 3;	// source rectangle width
DWORD  dwHeight = 2;	// source rectangle height
int  XSrc = 0;	// x-coordinate of lower-left corner of source rect.
int  YSrc = 0;	// y-coordinate of lower-left corner of source rect.
UINT  uStartScan = 0;	// first scan line in array
UINT  cScanLines = 2;	// number of scan lines
UINT  fuColorUse  = 0;

boolean display_on_page(work_page page, LPBITMAPINFOHEADER dib) {
   coordinate pixel_width = dib->biWidth;
   coordinate pixel_height = dib->biHeight;
   boolean succeeded = TRUE;
//
   LPBITMAPINFO temp_header
      = (LPBITMAPINFO) new unsigned char[dib_header_size];
   memcpy(temp_header,default_header,dib_header_size);
   // same as default except for dimensions
   temp_header->bmiHeader.biWidth = pixel_width;
   temp_header->bmiHeader.biHeight = pixel_height;
   temp_header->bmiHeader.biSizeImage = pixel_width*pixel_height;
   work_page temp_page;
   HBITMAP new_bitmap
     = WinGCreateBitmap(main_device_context,temp_header,
								(void *far *) &temp_page);
   if (new_bitmap != NULL) {
      HBITMAP saved_bitmap
          = (HBITMAP) SelectObject(main_device_context,new_bitmap);
//      succeeded = (boolean)
   bytes temp = DIB_bytes(dib);
   for (int i = 0; i < 6; i++) {
       tt_error_file() << (int) temp[i] << " " ;
   };
   tt_error_file() << endl;
   for (int i = 0; i < 6; i++) {
       tt_error_file() << (int) temp_page[i] << " ";
   };
      tt_error_file() << endl;
//
tt_error_file() <<
        SetDIBitsToDevice(main_device_context,
                          0,0,pixel_width,pixel_height,
                          0,0,
                          0,pixel_height,
                          DIB_bytes(dib), //bits_of_dib(dib),
                          (BITMAPINFO *) dib,
  //                        (dib->biBitCount>8)?DIB_RGB_COLORS:DIB_PAL_COLORS,
                          DIB_RGB_COLORS);
 tt_error_file() << endl;
//
        SetDIBitsToDevice(main_device_context,
                          XDest,
YDest,
dwWidth,
dwHeight,
XSrc,
YSrc,
uStartScan,
cScanLines,
                          DIB_bytes(dib), //bits_of_dib(dib),
                          (LPBITMAPINFO) dib,
                          (fuColorUse)?DIB_RGB_COLORS:DIB_PAL_COLORS);
   bytes temp = DIB_bytes(dib);
   for (int i = 0; i < 6; i++) {
       tt_error_file() << (int) temp[i] << " " ;
   };
   tt_error_file() << endl;
//
//     Rectangle(main_device_context,0,0,pixel_width,pixel_height);
//     tt_error_file() << endl << "coping lines " <<
//        StretchDIBits(main_device_context,
//							 0,pixel_height-1,pixel_width,pixel_height,
//							 0,0,pixel_width,pixel_height,
//    						 bits_of_dib(dib),
//                      (BITMAPINFO *) dib,
//                      (dib->biBitCount>8)?DIB_RGB_COLORS:DIB_PAL_COLORS,
//                      SRCCOPY);
      GdiFlush();
      work_page temp_page = tt_memory_graphics_work_page;
      for (int i = 0; i < pixel_height; i++) {
         memcpy(page,temp_page,pixel_width);
         temp_page += pixel_width;
      };
//      HMEMCPY(page,temp_page,pixel_width*pixel_height);
      for (int i = 0; i < 6; i++) {
       tt_error_file() << (int) page[i] << " ";
   };
   tt_error_file() << endl << endl;
//
      SelectObject(main_device_context,saved_bitmap);
      DeleteObject(new_bitmap);
   } else {
      succeeded = FALSE;
   };
   delete [] (bytes) temp_header;
//
   return(succeeded);
};
*/

// wingutil.c does it better
//work_page bits_of_dib(LPBITMAPINFOHEADER dib) {
//   return((work_page) dib + (UINT) dib->biSize + DibPaletteSize(dib));
//};

//void read_palette(ifstream &palette) {
//	palette.read(header_memory,dib_header_size);
//};

void read_palette(HFILE &palette) {
	_lread(palette,header_memory,dib_header_size);
	// so you can run with the system palette instead (e.g. when using 24-bit artwork)
//#if TT_DEBUG_ON
//	HDC visible_device_context = GetDC(NULL); // test on 191101
//	PALETTEENTRY system_palette[256];
//	GetSystemPaletteEntries(visible_device_context,0,256,system_palette);
//	ReleaseDC(NULL,visible_device_context);
//	for (int i = 0; i < 256; i++) {
//		default_header->bmiColors[i].rgbRed = system_palette[i].peRed;
//		default_header->bmiColors[i].rgbGreen = system_palette[i].peGreen;
//		default_header->bmiColors[i].rgbBlue = system_palette[i].peBlue;
//	};
//#endif
};

void allocate_brushes() {
	brush_global_handles = new HGLOBAL[DIB_brush_count];
	brushes = new HBRUSH[DIB_brush_count];
};

void create_DIB_brush(int index, bytes brush) {
	default_header->bmiHeader.biWidth = 8;
	default_header->bmiHeader.biHeight = 8;
	brush_global_handles[index] = global_alloc(GHND,dib_header_size+64);
	bytes pattern = (bytes) GlobalLock(brush_global_handles[index]);
	memcpy(pattern,header_memory,dib_header_size);
	memcpy(pattern+dib_header_size,brush,64);
	brushes[index] // would have thought that DIB_PAL_COLORS would work...
		  = CreateDIBPatternBrush(brush_global_handles[index],DIB_RGB_COLORS);
};

void MainWindow::Register()  {
  WNDCLASSG wndclass;   // Structure used to register Windows class.

  wndclass.style         = CS_HREDRAW | CS_VREDRAW | 
//									CS_DBLCLKS |  // enable double clicks
									CS_OWNDC; // changes to DC stick
									// | CS_BYTEALIGNWINDOW ;
  wndclass.lpfnWndProc   = ::WndProc;
  wndclass.cbClsExtra    = 0;
		  // Reserve extra bytes for each instance of the window;
		  // we will use these bytes to store a pointer to the C++
		  // (MainWindow) object corresponding to the window.
		  // the size of a 'this' pointer depends on the memory model.
  wndclass.cbWndExtra    = sizeof( MainWindow * );
  wndclass.hInstance     = Main::hInstance;
  wndclass.hIcon         = LoadIconG(Main::hInstance,"tt_icon");
  wndclass.hCursor       = LoadCursor(NULL,IDC_ARROW);
  wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
  wndclass.lpszMenuName  = NULL;
  wndclass.lpszClassName = szClassName;

  if (!RegisterClassG(&wndclass)) {
#if TT_32
	  message_box_last_error();
#endif
	  exit(FALSE);
  };
};

/*
void MainWindow::display_background(int bitmap_index) {
#if GRAPHICS_TO_MEMORY
//	if (saved_background_index != bitmap_index) {
		saved_background_page = load_background(bitmap_index); 
//	};
#if TT_32
	memcpy(tt_memory_graphics_work_page,
#else
	hmemcpy(tt_memory_graphics_work_page,
#endif	
			  saved_background_page,
			  tt_screen_bytes_size);
#else
	if (bitmap_index != previous_bitmap_index) {
		if (previous_bitmap_index != -1) DeleteObject(background_bitmap);
		background_bitmap = LoadBitmap(tt_library,MAKEINTRESOURCE(bitmap_index));
		if (background_bitmap == 0) {
			tt_error_file() << "Image missing or out of memory.  Background #"
							<< bitmap_index << endl;
			return;
		};
		previous_bitmap_index = bitmap_index;
		original_background_bitmap
			= (HBITMAP) SelectObject(background_device_context,background_bitmap); 
	};
	BitBlt(tt_hidden_device_context,0,0,
			 tt_screen_width,tt_screen_height,
			 background_device_context,0,0,SRCCOPY);
#endif
};
*/

boolean MainWindow::generic_text_out(coordinate ulx, coordinate uly, const_string text, int length) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 140799) {
		tt_error_file() << "Text out " << text << " length=" << length << endl;
	};
#endif
#if TT_WIDE_TEXT
	if (need_wide_characters()) {
		long destination_length = MultiByteToWideChar(tt_code_page[tt_use_spoken_alternative],0,text,length,NULL,0);
		wide_string wide_text = new wide_character[destination_length+1];
		MultiByteToWideChar(tt_code_page[tt_use_spoken_alternative],0,text,length,wide_text,destination_length);
		wide_text[destination_length] = NULL;
		boolean result = TextOutW(main_device_context,ulx,uly,wide_text,destination_length);
		delete [] wide_text;
		return(result);
	} else
#endif
		return(TextOut(main_device_context,ulx,uly,text,length));
};

void MainWindow::place_character(character c, 
											coordinate ulx, coordinate uly,
											double width, double height,
                                 color text_color) { // could add fixed and bold
	if (width < minimum_character_pixel_width || height < minimum_character_pixel_height) return; 
//	if (height < minimum_character_height) {
//		select_stock_object(GRAY_BRUSH);
//		fill_box(ulx-width,
//					tt_screen_height-uly,
//					ulx,
//					tt_screen_height-(uly+height));
//		return;
//	};
	tt_memory_graphics_mark_x = ulx;
	tt_memory_graphics_mark_y = uly;//-height;
//	if (clipped()) {
// following clips but can't show part of a character
//		long total_width = width;
//		long total_height = height;
//		int left_offset, right_offset, top_offset, bottom_offset;
//		clip(total_width,total_height,left_offset,right_offset,top_offset,bottom_offset);
//		if (total_width == 0 || total_height == 0 || 
//			 top_offset > 0 || bottom_offset > 0 || left_offset > 0 || right_offset > 0) {
//			return;
//		};
//	};
//	set_font(width,height,TRUE,TRUE);
	character text[1];
	text[0] = c;
BEGIN_GDI
	set_font(width,height,TRUE,TRUE);
   SetBkMode(main_device_context,TRANSPARENT); // for text
   SetTextColor(main_device_context,text_color); // updated 011001 tt_colors[text_color]);
	generic_text_out(ulx,(coordinate) (tt_screen_height-(uly+height)),text,1);
   deselect_font();
//   SetTextColor(main_device_context,PALETTEINDEX(tt_black)); // necessary??
END_GDI
#if TT_32&&WING
   GdiFlush(); // ensure 32bit GDI and WinG event order
#endif
};

void MainWindow::place_wide_character(wide_character c, 
												  coordinate ulx, coordinate uly,
												  double width, double height,
												  color text_color) { // could add fixed and bold
	if (width < minimum_character_pixel_width || height < minimum_character_pixel_height) return; // updated 150799
//	if (height < minimum_character_height) {
//		select_stock_object(GRAY_BRUSH);
//		fill_box(ulx-width,
//					tt_screen_height-uly,
//					ulx,
//					tt_screen_height-(uly+height));
//		return;
//	};
	tt_memory_graphics_mark_x = ulx;
	tt_memory_graphics_mark_y = uly; //-height;
//	if (clipped()) {
// following clips but can't show part of a character
//		long total_width = width;
//		long total_height = height;
//		int left_offset, right_offset, top_offset, bottom_offset;
//		clip(total_width,total_height,left_offset,right_offset,top_offset,bottom_offset);
//		if (total_width == 0 || total_height == 0 || 
//			 top_offset > 0 || bottom_offset > 0 || left_offset > 0 || right_offset > 0) {
//			return;
//		};
//	};
//	set_font(width,height,TRUE,TRUE);
	wide_character text[1];
	text[0] = c;
BEGIN_GDI
	set_font(width,height,TRUE,TRUE);
   SetBkMode(main_device_context,TRANSPARENT); // for text
   SetTextColor(main_device_context,text_color); //updated on 011001 from tt_colors[text_color]);
#if TT_DEBUG_ON
	if (tt_debug_mode == 140799) {
		tt_error_file() << "Place wide character " << text << " length=1" << endl;
	};
#endif
	TextOutG(main_device_context,ulx,(coordinate) (tt_screen_height-(uly+height)),text,1);
   deselect_font();
//   SetTextColor(main_device_context,PALETTEINDEX(tt_black)); // necessary??
END_GDI
#if TT_32&&WING
   GdiFlush(); // ensure 32bit GDI and WinG event order
#endif
};

void MainWindow::set_clip_region() {
#if TT_DIRECTX
   GDI_clipping_needed // do it when I get_device_context
     = !(tt_destination_llx == 0 &&
         tt_destination_lly == 0 &&
         tt_destination_urx == tt_screen_width && // experiment on 180702 to remove -1 from tt_screen_width
         tt_destination_ury == tt_screen_height); // experiment on 180702 to remove -1
   clip_structure.clip_rectangle.left = tt_destination_llx;
   clip_structure.clip_rectangle.top = tt_destination_lly;
   clip_structure.clip_rectangle.right = tt_destination_urx;
   clip_structure.clip_rectangle.bottom = tt_destination_ury;
   dderr(clipper->SetClipList((LPRGNDATA) &clip_structure,NULL),_T("Resetting the clip list."));
#else
   clip_now();
#endif
#if TT_DEBUG_ON
   if (tt_debug_mode == 10101) {
		if (GDI_clipping_needed) {
			tt_error_file() << "GDI_clipping_needed" << endl;
		};
      tt_error_file() << "Clipping to region " << tt_destination_llx << "," << tt_destination_lly
                  << " to " << tt_destination_urx << "," << tt_destination_ury
                  << " on frame " << tt_frame_number << endl;
   };
#endif
};

void MainWindow::current_clip_region(TTRegion &region) {
#if TT_DIRECTX
   region.min_x = clip_structure.clip_rectangle.left;
   region.max_x = clip_structure.clip_rectangle.right;
   region.min_y = clip_structure.clip_rectangle.bottom;
   region.max_y = clip_structure.clip_rectangle.top;
#else
   region.min_x = tt_destination_llx;
   region.max_x = tt_destination_urx;
   region.min_y = tt_destination_ury;
   region.max_y = tt_destination_lly;
#endif
};

void MainWindow::restore_clip_region(TTRegion &region) {
   tt_destination_llx = region.min_x;
   tt_destination_lly = region.max_y;
   tt_destination_urx = region.max_x;
   tt_destination_ury = region.min_y;
   set_clip_region();
};

void MainWindow::clip_now() {
   // this is both for non-DirectX version and for GDI stuff with DirectX
	HRGN clip_region_handle
		 = CreateRectRgn(tt_destination_llx,
                       tt_screen_height-(1+tt_destination_ury), // 9/23/96 add 1+
							  1+tt_destination_urx,
                       tt_screen_height-(tt_destination_lly-1)); // was ...lly+1
#if TT_DEBUG_ON
   if (tt_debug_mode == 10101) {
      tt_error_file() << "Clip now to region " << tt_destination_llx << "," << tt_destination_lly
                  << " to " << tt_destination_urx << "," << tt_destination_ury
                  << " on frame " << tt_frame_number << endl;
   };
#endif
//was
//		 = CreateRectRgn(tt_destination_llx,tt_screen_height-(tt_destination_ury+1),
//							  1+tt_destination_urx,tt_screen_height-tt_destination_lly);
	SelectClipRgn(main_device_context,clip_region_handle);
	DeleteObject(clip_region_handle); // Windows GDI has already copied it
};

coordinate brush_x=0, brush_y=0;

void MainWindow::set_brush_origin(coordinate x, coordinate y) {
	brush_x = x;
	brush_y = y;
};

static HBRUSH previous_selected_brush = NULL;

void MainWindow::select_brush(BrushId brush_id) {
//#if TT_DIRECTX
//   current_brush_id = brush_id;
//#else
	HBRUSH brush = brushes[brush_id];
   static HBRUSH last_brush = NULL;
#if TT_32
	SetBrushOrgEx(main_device_context,brush_x,brush_y,NULL);
#else
	UnrealizeObject(brush); // not clear if this does anything even for Win16
	SetBrushOrg(main_device_context,brush_x,brush_y);
#endif
#if !TT_DIRECTX
	if (last_brush == brush) return;
#endif
   previous_selected_brush = (HBRUSH) SelectObject(main_device_context,brush);
   if (previous_selected_brush == NULL) {
      log(C(IDC_WINDOWS_ERROR),FALSE,TRUE);
   };
   last_brush = brush;
//#endif
};

void MainWindow::deselect_brush() {
   SelectObject(main_device_context,previous_selected_brush);
};

void MainWindow::clear(BrushId brush_id) {
BEGIN_GDI
	select_brush(brush_id);
   // added 1 here on 090502 since otherwise right and bottom border show "old" stuff since no longer draw_borders
	Rectangle(main_device_context,0,0,tt_screen_width+1,tt_screen_height+1);
   deselect_brush();
END_GDI
#if TT_32&&WING
   GdiFlush(); // ensure 32bit GDI and WinG event order
#endif
};

void MainWindow::draw_synthetic_shape(SyntheticShape shape,
                                      coordinate min_x, coordinate min_y,
						                    coordinate max_x, coordinate max_y) {
   // since these "ancient" primitives don't work for values greater than 32K - updated 300803
//	if (min_x < 0) min_x = 0;
   if (min_x < min_short) min_x = min_short; // rewritten on 021205 since for ellipses this reshaped them
	if (max_x > max_short) max_x = max_short;
//	if (min_y < 0) min_y = 0;
	if (min_y < min_short) min_y = min_short; // new on 021205
	if (max_y > max_short) max_y = max_short;
   switch (shape) {
      case SYNTHETIC_RECTANGLE:
      case SYNTHETIC_HOLLOW_RECTANGLE:
         Rectangle(main_device_context,min_x,min_y,max_x,max_y);
         break;
      case SYNTHETIC_ROUNDED_RECTANGLE:
      case SYNTHETIC_ROUNDED_HOLLOW_RECTANGLE:
         RoundRect(main_device_context,min_x,min_y,max_x,max_y,(max_x-min_x)/5,(max_y-min_y)/5);
         break;
      case SYNTHETIC_CIRCLE:
      case SYNTHETIC_HOLLOW_CIRCLE:
         Ellipse(main_device_context,min_x,min_y,max_x,max_y);
         break;
      case SYNTHETIC_LINE_45:  // from lower left corner to upper right
      case SYNTHETIC_DOTTED_LINE_45:
//#if TT_32
         MoveToEx(main_device_context,min_x,max_y, NULL);
//#else
//         MoveToEx(main_device_context,min_x,max_y);
//#endif
			LineTo(main_device_context,max_x,min_y);
         break;
      case SYNTHETIC_LINE_315: // from upper right to lower left
      case SYNTHETIC_DOTTED_LINE_315:
//#if TT_32
         MoveToEx(main_device_context,min_x,min_y, NULL);
//#else
//         MoveToEx(main_device_context,min_x,min_y);
//#endif
			LineTo(main_device_context,max_x,max_y);
         break;
   };
};

//boolean most_recent_directional_pad_active = FALSE;

void MainWindow::save_direction_pad_region() {
   if (directional_pad_center_x < 0) return; // not yet initialized
   coordinate min_x = directional_pad_center_x-directional_pad_radius-front_surface_rectangle.left;
   coordinate min_y = front_surface_rectangle.bottom-(directional_pad_center_y+directional_pad_radius);
   coordinate max_x = directional_pad_center_x+directional_pad_radius-front_surface_rectangle.left;
   coordinate max_y = front_surface_rectangle.bottom-(directional_pad_center_y-directional_pad_radius);
//   if (directional_pad_active) { // || directional_pad_active != most_recent_directional_pad_active) { // active or just became inactive
      save_region(min_x,max_x,min_y,max_y); 
//   };
      //   most_recent_directional_pad_active = directional_pad_active;
};

void MainWindow::draw_directional_pad() { // new on 230402
   // when in SIMULATED_DIRECTIONAL_PAD_MOUSE_MODE this is displayed
   coordinate min_x = directional_pad_center_x-directional_pad_radius-front_surface_rectangle.left;
   coordinate min_y = directional_pad_center_y-directional_pad_radius-front_surface_rectangle.top;
   coordinate max_x = directional_pad_center_x+directional_pad_radius-front_surface_rectangle.left;
   coordinate max_y = directional_pad_center_y+directional_pad_radius-front_surface_rectangle.top;
BEGIN_GDI
   HGDIOBJ old_selection;
   if (directional_pad_active) {
      old_selection = select_object(CreatePen(PS_SOLID,tt_screen_width/200,active_simulated_directional_pad_color));
   } else {
      old_selection = select_object(CreatePen(PS_DOT,0,inactive_simulated_directional_pad_color));
   }; 
   select_stock_object(NULL_BRUSH);
   draw_synthetic_shape(SYNTHETIC_CIRCLE,min_x,min_y,max_x,max_y);
   if (directional_pad_active) {
      directional_pad_mouse_x, directional_pad_mouse_y;
      const int ARROW_HEAD = 0;
      const int ARROW_TAIL = 1;
      const int ARROW_HEAD_RIGHT = 2;
      const int ARROW_HEAD_LEFT = 3;
      POINT arrow_points[4];
      arrow_points[ARROW_HEAD].x = directional_pad_mouse_x;
      arrow_points[ARROW_HEAD].y = directional_pad_mouse_y;
      arrow_points[ARROW_TAIL].x = directional_pad_center_x-front_surface_rectangle.left;
      arrow_points[ARROW_TAIL].y = directional_pad_center_y-front_surface_rectangle.top;
      if (old_selection != NULL) DeleteObject(select_object(old_selection));
      old_selection = tt_main_window->select_object(CreatePen(PS_SOLID,tt_screen_width/200,simulated_directional_pad_arrow_color));
      coordinate x_distance = arrow_points[ARROW_TAIL].x-arrow_points[ARROW_HEAD].x;
      coordinate y_distance = arrow_points[ARROW_HEAD].y-arrow_points[ARROW_TAIL].y;
      if (x_distance != 0) {
         double tangent = atan2((double) y_distance,(double) x_distance);
         double angle_change = pi_as_double/6;
         double head_length = directional_pad_radius/5;
         double delta_x = cos(tangent+angle_change)*head_length;
         double delta_y = sin(tangent+angle_change)*head_length;
         arrow_points[ARROW_HEAD_RIGHT].x = (LONG) (arrow_points[ARROW_HEAD].x+delta_x);
         arrow_points[ARROW_HEAD_RIGHT].y = (LONG) (arrow_points[ARROW_HEAD].y-delta_y);
         delta_x = cos(tangent-angle_change)*head_length;
         arrow_points[ARROW_HEAD_LEFT].x = (LONG) (arrow_points[ARROW_HEAD].x+delta_x);
         delta_y = sin(tangent-angle_change)*head_length;
         arrow_points[ARROW_HEAD_LEFT].y = (LONG) (arrow_points[ARROW_HEAD].y-delta_y);
//       Polyline(main_device_context,arrow_points,4); 
#if TT_DEBUG_ON
         if (tt_debug_mode == 230402) {
            tt_error_file() << "Tangent   : " << (tangent/radians_per_degree) << endl;
            tt_error_file() << "Head      : " << arrow_points[ARROW_HEAD].x << "," << arrow_points[ARROW_HEAD].y << endl;
            tt_error_file() << "Tail      : " << arrow_points[ARROW_TAIL].x << "," << arrow_points[ARROW_TAIL].y << endl;
            tt_error_file() << "Head Right: " << arrow_points[ARROW_HEAD_RIGHT].x << "," << arrow_points[ARROW_HEAD_RIGHT].y << endl;
            tt_error_file() << "Head Left : " << arrow_points[ARROW_HEAD_LEFT].x << "," << arrow_points[ARROW_HEAD_LEFT].y << endl;
         };
#endif
         for (int i = 1; i < 4; i++) {
            MoveToEx(main_device_context,arrow_points[ARROW_HEAD].x,arrow_points[ARROW_HEAD].y,NULL);
            LineTo(main_device_context,arrow_points[i].x,arrow_points[i].y);
         };
      };
   };
   if (old_selection != NULL) DeleteObject(select_object(old_selection));
END_GDI
};

void MainWindow::fill_box(coordinate min_x, coordinate min_y,
						        coordinate max_x, coordinate max_y) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 170999) {
		tt_error_file() << "Drawing rectangle " << min_x << "," << min_y 
							 << " to " << max_x << "," << max_y 
							 << " frame=" << tt_frame_number << endl;
	};
#endif
	if (max_x < 0 || max_y < 0) return; // new on 070904
	if (min_x > tt_screen_width || min_y > tt_screen_height) return; // new on 070904
BEGIN_GDI
// added min and max on 090702 since on Windows 98 seems not to clip right if numbers are too large or small
	Rectangle(main_device_context,max(0,min_x),max(0,min_y),min(max_x,tt_screen_width),min(max_y,tt_screen_height)); 
END_GDI
};

void MainWindow::select_stock_object(int object) {
//#if TT_DIRECTX
//   get_device_context();
//#endif
   SelectObject(main_device_context,GetStockObject(object));
//#if TT_DIRECTX
//   release_device_context();
//#endif
};

HGDIOBJ MainWindow::select_object(HGDIOBJ object) {
//#if TT_DIRECTX
//   get_device_context();
//#endif
   HGDIOBJ result = SelectObject(main_device_context,object);
//#if TT_DIRECTX
//   release_device_context();
//#endif
   return(result);
};

void MainWindow::select_pen(HPEN pen) {
//#if TT_DIRECTX
//   get_device_context();
//#endif
   SelectObject(main_device_context,pen);
//#if TT_DIRECTX
//   release_device_context();
//#endif
};

HBRUSH MainWindow::create_solid_brush(COLORREF color) { // prior to 011001 was unsigned char color_index) {
   return(CreateSolidBrush(color));
};

HPEN MainWindow::create_solid_pen(COLORREF color) { // prior to 011001 was unsigned char color_index) {
   return(CreatePen(PS_SOLID,0,color));
};

HPEN MainWindow::create_dotted_pen(COLORREF color) { // prior to 011001 was unsigned char color_index) {
   return(CreatePen(PS_DOT,0,color));
};


HFONT previous_font_handle = NULL;
HFONT delete_when_deselected_font_handle = NULL;

//#include <locale.h>

void MainWindow::set_font(double width, double height, // width and heigth were integers prior to 080803 but accurracy lost in converting to points
				              boolean bold, boolean fixed_width) {
   // WARNING:  DirectDraw version expects the caller to
   // have called get_device_context()
#if TT_DEBUG_ON
 	if (tt_debug_mode == 140799) {
		tt_error_file() << "Setting font " << width << "x" << height << endl;
	};
//   if (font_handle != NULL && tt_debug_mode == 1111) return; // avoid GDI32 problem
#endif
// with new scheme of deselecting font following is now broken -- could revive??
//#if !TT_DIRECTX
   // direct_draw can't have this optimization -- sigh
//	if (font_width == width && font_height == height &&
//		 font_bold == bold && font_fixed_width == fixed_width) {
//       previous_font_handle = NULL;
//       return;
//   };
//#endif
   LOGFONT logical_font;
//   memset((bytes) &logical_font,0,sizeof(logical_font));
	ZeroMemory((bytes) &logical_font,sizeof(logical_font)); // probably faster... re-written on 211201
//   if (width == 0) width = 1; // commented out on 170102 so can let Windows pick a width appropriate for the 
   if (height == 0) height = 1;
//   if (fixed_width) { // seems to fit better... -- commented this part out on 050102
//      logical_font.lfHeight = height;
//      logical_font.lfWidth = width;
//   } else {	// 72 points to an inch but 96 pixels
	   // made positive again on 270202 since was having too many problems getting a good size font for numbers and the like
		// made negative on 050102 since this uses the character height rather than the cell height (see LOGFONT documentation)
      //logical_font.lfHeight = (-72*height)/96; 
      //logical_font.lfWidth = (-72*width)/96;
		const double pixels_to_points = -72.0/96.0;
		// experimented on 301004 to round to nearest by subtracting -.5 (no noticeable difference so removed since may cause other changes)
		logical_font.lfHeight = (long) (pixels_to_points*height); 
		// made negative on 050102 since this uses the character height rather than the cell height (see LOGFONT documentation)
      logical_font.lfWidth = (long) (pixels_to_points*width);
//   };
   if (bold) {
      logical_font.lfWeight = FW_BOLD;
   } else {
      logical_font.lfWeight = FW_NORMAL;
   };
   if (fixed_width) {
		logical_font.lfPitchAndFamily = FIXED_PITCH|FF_MODERN;
   } else {
		logical_font.lfPitchAndFamily = VARIABLE_PITCH|FF_SWISS;
//         logical_font.lfFaceName =
   };
   if (fixed_width) {
		strcpy(logical_font.lfFaceName,tt_fixed_font_name);
   } else {
      strcpy(logical_font.lfFaceName,tt_variable_font_name);
	};
//	if (tt_font_name != NULL) {
//		strcpy(logical_font.lfFaceName,tt_font_name);
//   } else if (!fixed_width) {
//		strcpy(logical_font.lfFaceName,"Arial");
//   } else {
//		logical_font.lfFaceName[0] = '\0'; // null string
//   };
   logical_font.lfOutPrecision = OUT_DEFAULT_PRECIS;
   logical_font.lfClipPrecision = CLIP_DEFAULT_PRECIS;
   logical_font.lfQuality = DEFAULT_QUALITY;
	if (tt_language == JAPANESE) {
		logical_font.lfCharSet = SHIFTJIS_CHARSET; // new on 091199
//		string x = setlocale( LC_ALL, "Japanese" );
//		x = setlocale( LC_ALL, ".932" );
//		if (x != NULL) tt_error_file() << x << endl;
	} else {
		logical_font.lfCharSet = DEFAULT_CHARSET; // might try ANSI_CHARSET
	};
   font_handle = CreateFontIndirect(&logical_font);
   if (font_handle == NULL) {
      log(C(IDC_FAIL_TO_CREATE_FONT),FALSE,TRUE);
      return;
   };
   if (delete_when_deselected_font_handle == NULL) {
      delete_when_deselected_font_handle = font_handle;
   } else {
      log(C(IDC_CREATING_FONT_WITHOUT_DELETING_OLD));
   };
   if (previous_font_handle == NULL) {
      previous_font_handle = (HFONT) SelectObject(main_device_context,font_handle);
   } else {
      log(C(IDC_SELECTING_A_FOND_WHILE_OLD_FONT_SELECTED));
   };
   if (previous_font_handle == NULL) { // what to do?
		log("Windows won't select a font.");
      log(C(IDC_WINDOWS_ERROR),FALSE,TRUE);
   };
   font_width = width;
   font_height = height;
   font_fixed_width = (flag) fixed_width;
   font_bold = (flag) bold;
   // get text dimensions
   // are these different???
   TEXTMETRIC tm;
   GetTextMetrics(main_device_context,&tm);
   character_width = tm.tmAveCharWidth;
   character_height = tm.tmHeight+tm.tmExternalLeading;
#if TT_DEBUG_ON 
	if (tt_debug_mode == 50504) {
		tt_error_file() << "tmHeight = " << tm.tmHeight << endl;
		tt_error_file() << "tmAscent = " << tm.tmAscent << endl;
		tt_error_file() << "tmDescent = " << tm.tmDescent << endl;
		tt_error_file() << "tmInternalLeading = " << tm.tmInternalLeading << endl;
		tt_error_file() << "tmExternalLeading = " << tm.tmExternalLeading << endl;
		tt_error_file() << "tmAveCharWidth = " << tm.tmAveCharWidth << endl;
		tt_error_file() << "tmMaxCharWidth = " << tm.tmMaxCharWidth << endl;
		tt_error_file() << "tmOverhang = " << tm.tmOverhang << endl;
		tt_error_file() << "tmDigitizedAspectX = " << tm.tmDigitizedAspectX << endl;
		tt_error_file() << "tmDigitizedAspectY = " << tm.tmDigitizedAspectY << endl;
	};
#endif
#if TT_ALPHA_FEATURE
	if (tt_right_to_left_text) {
		// experimented by adding TA_RIGHT^ on 270104
		SetTextAlign(main_device_context,TA_RTLREADING); // so if a Hebrew or Arabic font is selected then text will be right-to-left - new on 081203
	};
#endif
};

void MainWindow::deselect_font() {
   if (previous_font_handle != NULL) {
      if (SelectObject(main_device_context,previous_font_handle) == delete_when_deselected_font_handle) {
         if (delete_when_deselected_font_handle == font_handle) font_handle = NULL;
         DeleteObject(delete_when_deselected_font_handle);
      };
      previous_font_handle = NULL;
		delete_when_deselected_font_handle = NULL; // moved out of conditional above on 120700
   };
};

#if TT_ALPHA_FEATURE
void MainWindow::set_text_align(UINT alignment) { // new on 081203
//	HDC device_context = get_windows_device_context();
BEGIN_GDI
	SetTextAlign(main_device_context,alignment);
END_GDI
//	release_windows_device_context(device_context);
};
#endif

coordinate MainWindow::get_extent(const_string text, int length,
                                  double width, double height,
				                      boolean bold, boolean fixed_width,
											 boolean might_contain_tabs) { // new on 070903 - not currently used
	if (height < minimum_character_pixel_height || width < minimum_character_pixel_width) {
		return((coordinate) (length*width)); // added 150799
	};
	if (tt_log_version_number >= 59 && !fixed_width && length > max_variable_width_text_length) {
		fixed_width = TRUE; // too expensive and error prone to deal with very long lines -- new on 121104
	};
   SIZE size;
BEGIN_GDI
   set_font(width,height,bold,fixed_width);
   GetTextExtentPoint32(main_device_context,text,length,&size);
   deselect_font(); 
END_GDI
   return(size.cx);
};

void get_text_extent(HDC device_context, wide_string text, int length, boolean might_contain_tabs, boolean fixed_width, int &width, int &height) {
	// new on 070903 so only used tabbed API when needed
	if (might_contain_tabs && wcschr(text,L'\t') != NULL) {
		DWORD dimensions = GetTabbedTextExtentW(device_context,text,length,0,NULL);
		width = LOWORD(dimensions);
		height = HIWORD(dimensions);
	} else {
		if (fixed_width) { 
			// new on 060504 since is both faster 
			// and avoids an incompatibility between Windows 95/98/Me and 2000/XP in calling GetTextExtentPoint32G
			width = character_width*length;
//			width = (int) (font_width*length); // experiment tried on 141104
			height = character_height;
		} else {
			SIZE size;
			GetTextExtentPoint32G(device_context,text,length,&size);
//			const double points_to_pixels = 96.0/72.0; // tried this on 150903 but made the text much too small
			width = size.cx;
			if (width <= 0) { // if text is too large can happen -- new on 121104
				width = character_width*length;
			};
			height = size.cy;
			if (height <= 0) { // probably never happens
				height = character_height;
			};
		};
	};
};

#if TT_WIDE_TEXT

coordinate MainWindow::get_extent(wide_string text, int length,
                                  double width, double height,
				                      boolean bold, boolean fixed_width,
											 boolean might_contain_tabs) { // new on 070903
	// why have this and get_extent_size (and that is now a goofy name)
	if (height < minimum_character_pixel_height || width < minimum_character_pixel_width) { // added 150799
		return((coordinate) (length*width)); 
	};
	if (tt_log_version_number >= 59 && !fixed_width && length > max_variable_width_text_length) {
		fixed_width = TRUE; // too expensive and error prone to deal with very long lines -- new on 121104
	};
//  SIZE size; // updated on 020903 to support tabs
	int extent_width, ignore_height;
BEGIN_GDI
  set_font(width,height,bold,fixed_width);
  get_text_extent(main_device_context,text,length,might_contain_tabs,fixed_width,extent_width,ignore_height);
  deselect_font(); 
END_GDI
//#if TT_DEBUG_ON
//	if (tt_debug_mode == 50504 && fixed_width && extent_width != character_width*length) {
//		debug_this("Fixed width fonts not that simple");
//	};
//#endif
  return(extent_width);
};
#endif

coordinate MainWindow::get_vertical_extent(const_string text, int length,
														 double width, double height,
														 boolean bold, boolean fixed_width) {
	if (height < minimum_character_pixel_height || width < minimum_character_pixel_width) { // added 150799
		return((coordinate) height); 
	};
	if (tt_log_version_number >= 59 && !fixed_width && length > max_variable_width_text_length) {
		fixed_width = TRUE; // too expensive and error prone to deal with very long lines -- new on 121104
	};
  SIZE size;
BEGIN_GDI
  set_font(width,height,bold,fixed_width);
  GetTextExtentPoint32(main_device_context,text,length,&size);
  deselect_font(); 
END_GDI
  return(size.cy);
};

#if TT_WIDE_TEXT
coordinate MainWindow::get_vertical_extent(wide_string text, int length,
														 double width, double height,
														 boolean bold, boolean fixed_width) {
	if (height < minimum_character_pixel_height || width < minimum_character_pixel_width) { // added 150799
		return((coordinate) height); 
	};
	if (tt_log_version_number >= 59 && !fixed_width && length > max_variable_width_text_length) {
		fixed_width = TRUE; // too expensive and error prone to deal with very long lines -- new on 121104
	};
  SIZE size;
BEGIN_GDI
  set_font(width,height,bold,fixed_width);
  GetTextExtentPoint32G(main_device_context,text,length,&size);
  deselect_font(); 
END_GDI
  return(size.cy);
};
#endif

void MainWindow::get_extent_size(wide_string text, int length, int number_of_lines,
                                 double width, double height, int &longest_line, double &new_width, double &new_height,
				                     boolean bold, boolean fixed_width,
											boolean might_contain_tabs) { // new on 070903
#if TT_DEBUG_ON
	if (tt_debug_mode == 50504) {
		tt_error_file() << "get_extent_size: length = " << length << "; width = " << width << "; height = " << height
						<< "; bold = " << (int) bold  << "; fixed_width = " << (int) fixed_width << endl;
	};
#endif
	// simplified on 160102
//	SIZE *size = new SIZE(); // new on 150799
	// commented out on 210201 since this provides the wrong info - caller should avoid all this work when too small instead
//	if (height < minimum_character_pixel_height || width < minimum_character_pixel_width) { // added 150799
//		size->cx = length*width;
//		size->cy = height/number_of_lines;
//		return(size); 
//	};
   // used to call //  GetTextExtentPoint32G(main_device_context,text,length,&line_size) after setting font
   new_width = width/4; // line_size.cx; // changed on 210201 to handle multi-line text better - // new on 020903 to set a minimum width here
   new_height = 0; // line_size.cy;
   if (text == NULL || length == 0) return; // new on 240103 for robustness (crash report did this)
//  SIZE line_size; // had to abandon this to support tabs
//  DWORD dimensions;
   int line_width = 0;
   int line_height = 0;
	if (tt_log_version_number >= 59 && !fixed_width && length > max_variable_width_text_length) {
		fixed_width = TRUE; // too expensive and error prone to deal with very long lines -- new on 121104
	};
BEGIN_GDI
   set_font(width,height,bold,fixed_width);
  // GetTextExtentPoint32 only gives dimensions of a line
  // For multi-line I need to find the max of width and sum of heights
   int start_of_line = 0;
   longest_line = -1; // to be clobbered if end of line found
   int carriage_return_character_count = 1; 
	// must be at least '\r' and sometimes '\n' as well -- new on 060102 -- beginning 090802 just \n should be there
   boolean do_last_line = TRUE;
   if (number_of_lines > 1) { // condition new on 270202 since this is wasteful especially for very long numbers
	  for (int i = 0; i < length; i++) {
		  if (text[i] == '\r' || text[i] == '\n') { // added text[i] == '\n' on 080802
//			  if (i < length-1 && text[i+1] != '\r') 
			  i++; // conditions added on 220800 for multiple returns in a row
			  if (i < length-1 && text[i] == '\n') {
				  i++; // second conjunct added on 150799 so final new line with no text are handled properly -- conditions re-ordered on 210201
				  carriage_return_character_count++;
			  };
			  do_last_line = (i+1 < length); // don't do it if ends with a carriage return (new 020903)
//			  if (i < length) { // experiment on 020903 to not treat last empty line specially
//				  GetTextExtentPoint32G(main_device_context,text+i,length-i,&line_size);
				  int line_length = (i-start_of_line)-carriage_return_character_count; // was -1 prior to 060102
				  carriage_return_character_count = 1; // reset it
				  if (line_length > longest_line) {
					  longest_line = line_length;
				  };
				  int factor = 1; // new on 090702 see comment below
				  int line_length_used = line_length;
				  get_text_extent(main_device_context,text+start_of_line,line_length,might_contain_tabs,fixed_width,line_width,line_height);
//#if TT_DEBUG_ON
//					if (tt_debug_mode == 50504 && fixed_width && line_width != character_width*line_length) {
//						debug_this("Fixed width fonts not that simple");
//					};
//#endif
				  while (line_width == 32767 && line_length_used > 2) { // seems to be at least on Win95/98 that 32K is the limit - new on 090702
					  line_length_used = line_length_used/2; // try half size
					  factor *= 2; // and remember so can fix later
					  get_text_extent(main_device_context,text+start_of_line,line_length,might_contain_tabs,fixed_width,line_width,line_height);
				  };
				  line_width *= factor; // no op if no need to work around GDI limitation
				  start_of_line = i;
				  if (new_width < line_width) new_width = line_width;
//				  size->cy += line_size.cy;
				  // replaced the above by the following on 230800 since each line should have the same height
				  // so compute max and multiply by the number of lines
				  if (new_height < line_height) new_height = line_height;
     //      } else { // part of 020903
     //         do_last_line = (new_width == 0); // new on 090802 to deal better with empty final line
     //         if (do_last_line) {
     //            length--; // ignore final new line
     //         };
			  //};
		  };
	  };
   };
   if (do_last_line) { // do it one last time
     int remaining_length = length-start_of_line;
     int factor = 1;
//     GetTextExtentPoint32G(main_device_context,text+start_of_line,remaining_length,&line_size);
	  get_text_extent(main_device_context,text+start_of_line,remaining_length,might_contain_tabs,fixed_width,line_width,line_height);
#if TT_DEBUG_ON 
	  if (tt_debug_mode == 50504) {
		  tt_error_file() << "get_text_extent returned: line_width = " << line_width << "; line_height = " << line_height << endl;
	  };
	  //if (tt_debug_mode == 50504 && fixed_width && line_width != character_width*remaining_length) {
		 // debug_this("Fixed width fonts not that simple");
	  //};
#endif
     while (line_width == 32767 && remaining_length > 2) { // seems to be at least on Win95/98 that 32K is the limit - new on 090702
	     remaining_length = remaining_length/2; // try half size
	     factor *= 2; // and remember so can fix later
		  get_text_extent(main_device_context,text+start_of_line,remaining_length,might_contain_tabs,fixed_width,line_width,line_height);
     };
     line_width *= factor; // no op if no need to work around GDI limitation
     if (new_width < line_width) new_width = line_width;
     if (new_height < line_height) new_height = line_height;
   };
   int line_length = (length-start_of_line)-1;
   if (longest_line == -1) {
	  // single line
	  longest_line = length;
   } else if (line_length > longest_line) { // new on 270201
	  longest_line = line_length;
   };
   deselect_font(); 
END_GDI
   new_height *= number_of_lines; // new on 230800
#if TT_DEBUG_ON 
   if (tt_debug_mode == 50504) {
	  tt_error_file() << "get_extent_size returned: new_width = " << new_width << "; new_height = " << new_height << endl;
   };
#endif
};

boolean MainWindow::text_out(const_string text, int length, 
									  coordinate ulx, coordinate uly,
									  double new_font_width, double new_font_height,
									  boolean bold, boolean fixed_width,
									  boolean background_transparent,
									  color_index text_color,						 
									  boolean text_is_wide,
									  bytes color_permutation,
									  color background_color,
									  boolean might_contain_tabs, // new arg on 070903
									  int caret_location) { // new on 100604
	if ((length == 0 && caret_location != 0) || new_font_width < minimum_font_width || new_font_height < minimum_font_height) {
		return(FALSE);
	};
	// just checked that font is more than 2 pixels beginning on 090102 -- changed to 4 pixels on 240102 -- 
	// and 3 on 270202 -- changed to < from <= on 110302
	if (ulx > tt_screen_width) return(FALSE); // new on 100804 starts too far to the right
	if (ulx+new_font_width*length < 0) return(FALSE); // new on 100804 too far to the left
	if (uly-new_font_height > tt_screen_height) return(FALSE); // new on 100804 -- off the top
	if (uly < 0) return(FALSE); // new on 100804 -- off the bottom
	tt_memory_graphics_mark_x = ulx;
	tt_memory_graphics_mark_y = uly;
// clipping handled by Windows now
//	long width = length*new_font_width;
//	long height = new_font_height;
//	if (clipped()) {
//		int left_offset, right_offset, top_offset, bottom_offset;
//		clip(width,height,left_offset,right_offset,top_offset,bottom_offset);
//		if (width == 0 || height == 0 || top_offset > 0 || bottom_offset > 0) return;
//		if (left_offset > 0) {
//			ulx += left_offset;
//			int left_characters = left_offset/new_font_width;
//			text += left_characters;
//			length -= left_characters;
//		};
//		if (right_offset > 0) {
//			length -= right_offset/new_font_width;
//		};
//	};
BEGIN_GDI
	set_font(new_font_width,new_font_height,bold,fixed_width);
	uly = tt_screen_height-uly; // but not if WinG in other direction
   if (background_transparent) {
      SetBkMode(main_device_context,TRANSPARENT); // for text
   } else {
      SetBkMode(main_device_context,OPAQUE);
		SetBkColor(main_device_context,background_color);
   };
//	if (color_permutation != NULL) { // new on 200901
//		SetTextColor(main_device_context,tt_colors[color_permutation[text_color]]);
//		SetBkColor(main_device_context,tt_colors[color_permutation[tt_white]]); // darken white background
//	} else {
//		SetTextColor(main_device_context,tt_colors[text_color]);
//		SetBkColor(main_device_context,tt_colors[tt_white]);
//	};
	COLORREF text_color_ref = compute_color(text_color,color_permutation);
	SetTextColor(main_device_context,text_color_ref);
//	SetBkColor(main_device_context,compute_color(tt_white,color_permutation)); // darken white background
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
#if TT_WIDE_TEXT
	if (text_is_wide) {
		wide_string wide_text = (wide_string) text;
#if TT_DEBUG_ON
		if (tt_debug_mode == 140799) {
			tt_error_file() << "text_out " << wide_text << " length=" << length << endl;
		};
#endif
		int result;
		if (might_contain_tabs) { // changed to conditionally supports tabs on 070903
			result = TabbedTextOutW(main_device_context,ulx,uly,wide_text,length,0,NULL,ulx); // changed to support tabs on 010903
		} else {
			result = TextOutW(main_device_context,ulx,uly,wide_text,length); 
		};
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		if (need_to_display_insertion_caret(caret_location)) { // new on 100604
			int before_caret_width, caret_height;
			if (caret_location > 0) {
				get_text_extent(main_device_context,wide_text,caret_location,might_contain_tabs,fixed_width,
					             before_caret_width,caret_height);
			} else {
				before_caret_width = 0;
				caret_height = character_height;
			};
//			COLORREF previous_color = SetDCPenColor(main_device_context,text_color_ref); // new on 110604
			if (text_color == tt_white) {
				select_stock_object(WHITE_PEN);
			};
#if TT_DEBUG_ON
			add_to_log_if_interesting_error();
#endif
			MoveToEx(main_device_context,ulx+before_caret_width,uly,NULL);
			LineTo(main_device_context,ulx+before_caret_width,uly+caret_height);
#if TT_DEBUG_ON
			add_to_log_if_interesting_error();
#endif
//			SetDCPenColor(main_device_context,previous_color); // worth bothering to restore this?
		};
#if TT_DEBUG_ON
		if (result == 0) {
			tt_error_file() << "Something went wrong with a call to TextOut. Last error is " << GetLastError() 
								 << " and its position is " << ulx << "," << uly << " and its length is " << length << endl;
			tt_error_file().write(text,length); // might not make much sense if Japanese...
			tt_error_file() << endl;
		};
#endif
	} else 
#endif
	if (!generic_text_out(ulx,uly,text,length)) {
		log(C(IDC_ERROR_FROM_TEXT_OUT));
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
   deselect_font();
END_GDI
#if TT_32&&WING
   GdiFlush(); // ensure 32bit GDI and WinG event order
#endif
	return(TRUE);
};

//boolean MainWindow::create_caret(coordinate height) {
//	return(::CreateCaret(window_handle,NULL,0,height));
//};
//
//boolean MainWindow::destroy_caret() {
//	return(DestroyCaret());
//};

void MainWindow::draw_line(coordinate start_x, coordinate start_y, coordinate end_x, coordinate end_y) {
BEGIN_GDI
	::MoveToEx(main_device_context,start_x,start_y,NULL);
	::LineTo(main_device_context,end_x,end_y);
END_GDI
};

void MainWindow::correct_font_size(wide_string text, int length, int number_of_lines,
											  double width, double height,
											  boolean bold, boolean fixed_width, // added fixed_width on 050102
											  boolean might_contain_tabs, // new on 070903
											  double &new_font_width, double &new_font_height) {
	// commented out on 220102 since no longer uses old font dimensions
//	if (font_height < minimum_character_pixel_height || font_width < minimum_character_pixel_width) { 
		// moved here on 210201 to avoid lots of work with very small text
//		new_font_width = 1; // or zero?
//		new_font_height = 1;
//		return;
//	};
	int longest_line;
	double new_width, new_height;
	const double font_height = 72.0;
	const double font_width = font_height*0.6; // (3*font_height)/5; // at least for Courier it is 3 to 5 
	get_extent_size(text,length,number_of_lines,font_width,font_height,longest_line,new_width,new_height,bold,fixed_width,might_contain_tabs);
   if (new_width > 1.0 && new_height > 1.0) {
//		new_font_width = (font_width*font_width*length)/extent->cx; // new on 300999
		new_font_height = (font_height*height)/new_height;
//		get_extent_size(text,length,number_of_lines,font_width,new_font_height,longest_line,new_width,new_height,bold,fixed_width);  
		// experiment on 170102
		//if (width > max_long/font_width) { // new on 250202 to deal with overflow -- no need to as of 080803 since dealing with double floats
		//	new_font_width = (city_coordinate) ((((double) font_width)*width)/new_width);
		//} else {
			new_font_width = (font_width*width)/new_width; // updated 210201 
//		};
#if TT_DEBUG_ON
		if (new_font_width < 0) {
			debug_this("Track down why there is a negative font width.");
		};
#endif
//		new_font_height = font_height; // extent->cy; // new on 220800
		// following makes sense but broke Japanese version - e.g. wall decoration in Puzzle Game
//		new_font_width = (font_width*max_total_width)/extent->cx;
//		new_font_height = (font_height*max_total_height)/extent->cy; 
//#if TT_DEBUG_ON
//		if (tt_debug_mode == 40102) {
//			SIZE *new_extent = get_extent_size(text,length,number_of_lines,new_font_width,new_font_height,longest_line,bold,fixed_width);
//			tt_error_file() << extent->cx << "x" << extent->cy << " is the old extent." << endl;
//			tt_error_file() << new_extent->cx << "x" << new_extent->cy << " is the new extent." << endl;
//		};
//#endif
	} else {
		new_font_width = width/4; // was 1.0 prior to 020903 changed to look more like text with just carriage returns
		new_font_height = height; // otherwise text can become small while typing as it is animating to full size in your hand - new on 051003
//		new_font_height = 1.0;
	};
};

/*
coordinate MainWindow::correct_font_height(wide_string text, int length, int number_of_lines,
														 coordinate new_font_width, 
														 coordinate new_font_height,
														 boolean bold) {
#if TT_32
	SIZE extent = get_extent_size(text,length,new_font_width,new_font_height,bold);
	coordinate y_extent = extent.cy;
#else
	return(new_font_height); // doesn't seem to work well
//	DWORD extent = GetTextExtent(main_device_context,text,length);
//	coordinate x_extent = LOWORD(extent);
//	correct_height = FALSE; // doesn't seem to work right
#endif
	if (y_extent > 1) {
		return((new_font_height*new_font_height*number_of_lines)/y_extent);
	} else {
		return(new_font_height);
	};
};
*/

int MainWindow::show_subtitle(wide_string wide_subtitle, int subtitle_length,
										coordinate character_width, coordinate character_height) {
	// updated on 111104 to return number of lines
//	wide_string wide_subtitle = copy_wide_string(subtitle,subtitle_length);
//	subtitle_length = wide_string_length(wide_subtitle);
	/*
	int max_length = max_subtitle_length()*2;
	if (subtitle_length > max_length) { // too long by a factor of 2 so clip it
		subtitle_length = last_space_or_punctuation(wide_subtitle,max_length);
		if (subtitle_length < 1) {
			subtitle_length = max_subtitle_length()*2;
		} else {
			subtitle_length++; // zero-indexing
		};
	};
	coordinate character_height = original_character_height;
	*/
//	coordinate new_character_width, new_character_height;
//	int slop = 0;
//	if (wide_subtitle[1] > 255) { // is really Unicode
//		slop = 2+subtitle_length/10;
//	};
	boolean unicode = contains_non_ascii(wide_subtitle,subtitle_length);
	if (unicode) {
		character_height = (3*character_height)/4; // new on 141199 since otherwise Kanji looks elongated
	};
BEGIN_GDI
/*
	boolean need_multiple_lines = FALSE;
	if (subtitle_length+slop*2 > max_fat_characters_per_subtitle_line) { // might need to shrink font size
		correct_font_size(wide_subtitle,subtitle_length,1,
							   character_width, //(tt_screen_width+subtitle_length/2)/(subtitle_length+slop), // round to nearest
								character_height,TRUE,
								tt_screen_width,character_height,
								new_character_width,new_character_height);
		if (new_character_width < character_width) {
			need_multiple_lines = TRUE;
			// height is proportionally shrunk version
//			character_height = (new_character_width*character_height)/character_width;
			character_width = new_character_width;
//		} else if (new_character_width > character_width) { // new on 270999
			correct_font_size(wide_subtitle,subtitle_length,1,new_character_width,new_character_height,
									TRUE,tt_screen_width,character_height,
									new_character_width,new_character_height);
			if (new_character_width < character_width) {
				need_multiple_lines = TRUE;
				// height is proportionally shrunk version
//				character_height = (new_character_width*character_height)/character_width;
//				character_width = new_character_width;
			};
		};
	};
*/
	set_font(character_width,character_height,TRUE,FALSE);
   SetBkMode(main_device_context,TRANSPARENT);
//	SetBkMode(main_device_context,OPAQUE);
//   SetBkColor(main_device_context,tt_colors[tt_black]);
//   if (blacken) {
//      SetTextColor(main_device_context,tt_colors[tt_black]);
//   } else {
      SetTextColor(main_device_context,tt_colors[tt_white]);
//   };
   long extent;
#if TT_32
   SIZE size;
	GetTextExtentPoint32W(main_device_context,wide_subtitle,subtitle_length,&size);
   extent = size.cx;
#else
	extent = LOWORD(GetTextExtent(main_device_context,subtitle,subtitle_length)); 
#endif
	const int margin = 4; // so it doesn't go straight to the edge
   coordinate new_line_height = (7*character_height)/8; 
	// looks better with new font_size scheme (character not cell) on 050102 was (3*character_height)/4;
	const int total_width = tt_screen_width-2*margin;
	int number_of_lines = (extent/total_width)+1;
	int number_of_lines_left = number_of_lines;
	if (number_of_lines_left < tt_screen->return_number_of_subtitle_lines_last_time()) {
		tt_screen->screen_dirty(); // could be more clever about exactly which region is dirty
	};
	tt_screen->set_number_of_subtitle_lines_last_time(number_of_lines_left);
	coordinate y = tt_screen_height-new_line_height*number_of_lines_left;
   SelectObject(main_device_context,GetStockObject(BLACK_BRUSH));
   SelectObject(main_device_context,GetStockObject(NULL_PEN));
   Rectangle(main_device_context,0,y,tt_screen_width+1,tt_screen_height+1); 
	// added 1 to both on 210602 since without borders box is too high by a pixel
//	int max_characters_to_a_line = ((tt_screen_width*subtitle_length)/extent)-5; // -5 to avoid just a bit too long lines
 	int max_length_this_line, length_this_line;
	int text_offset = 0;
	int total_text_length = subtitle_length-text_offset;
	int total_text_length_left = total_text_length;
	int extent_left = extent;
	while (number_of_lines_left > 0) { // new on 280999 to generate multi-line subtitles if need be
	// following new on 161099 so multi line subtitles look better
		max_length_this_line = (total_text_length_left+number_of_lines_left-1)/number_of_lines_left; // round up
		// need more room since if long word gets broken then the second line won't fit
		if (number_of_lines_left > 1) { // can't shave things off if last line
			int absolute_max = (total_text_length*total_width)/extent_left;
			int good_length_and_extra; // extra is to cover reasonable change line should end in a long word
			if (tt_language == JAPANESE) {
				good_length_and_extra = max_length_this_line+4;
			} else {
				good_length_and_extra = max_length_this_line+10;
			};
			max_length_this_line = min(good_length_and_extra,absolute_max); // new on 200200 so early lines get a chance to have long word at end
			while (extent > total_width) {
				GetTextExtentPoint32W(main_device_context,wide_subtitle+text_offset,max_length_this_line,&size);
				extent = size.cx;
				max_length_this_line--;
			};
			if (space_or_punctuation(wide_subtitle,text_offset+max_length_this_line,subtitle_length)) {
				max_length_this_line++; // just cut off a period or the like so put it back at then end of this line
			};
		} else { // compute extent of last line
			GetTextExtentPoint32W(main_device_context,wide_subtitle+text_offset,max_length_this_line,&size);
			extent = size.cx;
		};
		extent_left -= extent;
		coordinate x = (total_width-extent)/2; // center horizontally
//		boolean recenter = TRUE;
		if (number_of_lines_left > 1) { // not last line
			length_this_line = last_space_or_punctuation(wide_subtitle+text_offset,max_length_this_line,need_wide_characters());
			if (length_this_line == -1 || 
				 (length_this_line < (max_length_this_line*3)/4 && // was 2/3 prior to 091199
				  (number_of_lines_left != 2 || (subtitle_length-text_offset)-length_this_line > max_length_this_line))) {
				// didn't find any punctuation or spaces or need to postpone more than 1/3 of the line 
				// and if next line is last it won't fit otherwise
				length_this_line = max_length_this_line;
			} else {
				length_this_line++; // include punctuation itself
			};
		} else {
			if (max_length_this_line < subtitle_length-text_offset || x < 0) {
				 x = margin; // last line didn't fit - widen as much as I can -- replaced 0 with margin on 121199 to look nicer
				 deselect_font(); // added on 101199 to release old font first -- revisit this whole thing about how fonts are handled sometime???
				 set_font((total_width*character_width)/extent,(total_width*character_height)/extent,TRUE,FALSE); 
				 // 161099 and make the characters a pixel narrower too -- now (121199) reduce proportionately to fit
//				 recenter = FALSE;
			};
			length_this_line = subtitle_length-text_offset;
		};
		int new_line_length;
		// following is new on 221199 since spaces are in text only to indicate possible breaks
		wide_string line_to_display = remove_spaces_if_non_ascii_neighbors(wide_subtitle+text_offset,length_this_line,new_line_length);
//		if (recenter) { // always recenter now (221199) since spaces threw off numbers a bit
			GetTextExtentPoint32W(main_device_context,line_to_display,new_line_length,&size);
			extent = size.cx;
			x = (total_width-extent)/2;
//		};
		TextOutW(main_device_context,x,y,line_to_display,new_line_length);
		delete [] line_to_display;
		text_offset += length_this_line;
		y += new_line_height;
		number_of_lines_left--;
		total_text_length_left -= length_this_line;
	};
   deselect_font();
END_GDI
//	delete [] wide_subtitle;
	return(number_of_lines);
};

int MainWindow::place_text(const_string original_text, int length,
									coordinate ulx, coordinate uly,
									coordinate max_width, coordinate max_height,
									coordinate new_font_width,
									coordinate new_font_height,
									WhenToBreakSentences break_sentences,
									boolean vertically_center,
                           boolean add_dots_if_more,
									coordinate extra_width_at_center,
									coordinate blank_width,
                           color text_color,
                           boolean first_call,
									long max_length) {
	if (new_font_height < minimum_character_pixel_height || new_font_width < minimum_character_pixel_width) return(length); // updated 150799
#if TT_32 // can't run Unicode otherwise
	if (need_wide_characters()) { // added on 180699 e.g. Japanese
		wide_string wide_text = copy_wide_string(original_text);
		// changed the following test so that if there are any wide characters then display differently
		boolean ascii = (strlen(original_text) == wide_string_length(wide_text)); //(wide_text[0] < 256);
		int result;
		if (!ascii) { 
			result = place_wide_text(wide_text,ulx,uly,
											 max_width,max_height-new_font_height, // added -new_font_height on 121000 so never get the text off the bottom
											 new_font_width,new_font_height,
											 break_sentences,
											 vertically_center,
											 add_dots_if_more,
											 extra_width_at_center,
											 blank_width,text_color,first_call,max_length);
			// fixed this on 120999
			// result is in number of wide characters placed but I need to know how many bytes of the original that corresponds to
			result = multi_byte_length(wide_text,result);
		};
		delete [] wide_text;
		if (!ascii) return(result); // else fall thru
	};
#endif
	string original_text_copy = copy_string(original_text); // save this pointer to delete it at end
	string text = original_text_copy;
	// check first if this ends with some blanks and if so ignore them
	while (text[length-1] == ' ') {
      length--;
	};
	if (length > max_length) {
		length = last_space_or_punctuation(text,max_length);
		if (length < 0) {
			length = max_length;
		} else {
			length++; // since 0-indexing
		};
	};
   coordinate original_ulx = ulx;
   coordinate original_uly = uly;
BEGIN_GDI
	set_font(new_font_width,new_font_height,TRUE,FALSE);
	coordinate original_max_width = max_width;
	boolean left_clipped = FALSE;
	boolean right_clipped = FALSE;
	if (ulx < blank_width) { // off the left
		// looks better to leave a bit blank on the left
		max_width = original_max_width+ulx-blank_width;
		ulx = blank_width;
		left_clipped = TRUE;
	} else if (ulx+original_max_width+blank_width > tt_screen_width) {
		// goes off right side
		// looks better to leave a bit blank on the right
		max_width = (tt_screen_width-ulx)-blank_width;
		right_clipped = TRUE;
	};
	if (max_width <= character_width) {
// END_GDI was here
//      tt_main_window->
      deselect_font();
END_GDI 
		delete [] original_text_copy;
      return(length);
   };
	uly = tt_screen_height-uly; // but not if WinG in other direction
	coordinate center_y = uly+(max_height-character_height)/2;
	int max_line_length = max_width/character_width;
	int average_max_line_length;
	if (left_clipped || right_clipped) { 
		// lost half of the extra stuff
		average_max_line_length = (max_width+extra_width_at_center/4)/character_width;
	} else {
		average_max_line_length = (max_width+extra_width_at_center/2)/character_width;
	};
	// round up if 1/2 character or more left over
//	if (max_width%character_width > character_width/2) max_line_length++;
	int characters_printed = 0;
//	int line_count = (max_height+character_height/2)/character_height; // rounding to nearest new on 230500
	int line_count = max_height/character_height;
	if (line_count > 5) line_count--; // added on 160600 since too often extra text (or the ...) appear at the bottom
	coordinate extra_width = 0;
//	if (max_height%character_height > (2*character_height)/3) {
//		line_count++;
//	};
	int length_this_round = length;
	int estimated_blanks_on_right;
	if (length < 6) { // too short to worry much about it fitting
		estimated_blanks_on_right = 0;
	} else if (length < 10) { 
		estimated_blanks_on_right = 1;
	} else {
		// subtracted 4 since words aren't broken
		estimated_blanks_on_right = 4; // was 3 prior to 160600 but caused extra text at bottom of balloon sometimes
	};
	const int estimated_max_length = line_count*(average_max_line_length-estimated_blanks_on_right);
   	// too much text and don't want to break sentences
	if (break_sentences != OK_TO_BREAK_SENTENCES && estimated_max_length < length) {
      // was > (3*estimated_max_length)/4
      int i = estimated_max_length;
      int stop;
      if (break_sentences == BREAK_SENTENCES_TO_NOT_WASTE_HALF_BALLOON) {
         stop = estimated_max_length/2; 
         if (length-i < 20) {
            i = length;
         } else if (i+10 < length) {
            i += 10;
         };
      } else {
//         i = (9*i)/10; // take off 10% to be sure it really will fit -- commented out on 041199 since small sentences are displayed otherwise
         // but first be sure we didn't just chop off the end punctuation
         if (phrase_ending_punctuation(text,i+1,length,(break_sentences != NEVER_BREAK_SENTENCES))) i++;
         stop = 0;
      };
      int first_break_found = 0;
      int second_break_found = 0;
		for (; i > stop; i--) {
			// better to break a sentence than waste 1/2 of the space
			if (phrase_ending_punctuation(text,i,length,(break_sentences != NEVER_BREAK_SENTENCES)) && 
				 // is there a language independent way of doing this?
				 i < length-1 && (text[i+1] == ' ' || text[i+1] == NULL)) {
            if (first_break_found == 0) {
               first_break_found = i+1;
            } else if (second_break_found == 0) {
               second_break_found = i+1;
               break;
            };
			};
		};
      if (first_break_found == 0) {
         if (break_sentences == BREAK_SENTENCES_TO_NOT_WASTE_HALF_BALLOON) {
				first_break_found = stop;
				while (first_break_found > 0) {
					if (text[first_break_found] == ' ') break;
					first_break_found--;
				};
				if (first_break_found == 0) { // still didn't find a break so search forward instead
					first_break_found = stop;
					while (first_break_found < length) {
						if (text[first_break_found] == ' ') break;
						first_break_found++;
					};
				};
         } else {
            deselect_font();
END_GDI
            // didn't fit so try again with a smaller font
            int result = place_text(text,length,
												original_ulx,original_uly,
												max_width,max_height,
												(93*new_font_width)/100, // was 19/20 prior to 230500 but still didn't fit so well so changed to 18/20
												// 93% is a new compromise on 110700
												new_font_height,
												// was (18*new_font_height)/20 prior to 110700 but this looks better and doesn't get bad spacing
												break_sentences,
												vertically_center,
												add_dots_if_more,
												extra_width_at_center,
												blank_width,
												text_color,
												FALSE,
												max_length);  // not first call
				delete [] original_text_copy;
				return(result);
         };
      };
      if (second_break_found == 0 ||
          break_sentences == BREAK_SENTENCES_TO_NOT_WASTE_HALF_BALLOON ||
          first_call) { //
			//(first_call && text[first_break_found-1] == ',' && first_break_found-second_break_found > 10)) {
			// added conditions on 200400 to first_call above to avoid uglies like "OK, " on last line
			// commented out since shouldn't accept commas when NEVER_BREAK_SENTENCES
         length_this_round = first_break_found;
      } else {
         length_this_round = second_break_found;
      };
   };
	if (vertically_center && max_line_length > 3 &&
		 length_this_round/(average_max_line_length-estimated_blanks_on_right) < line_count) {
		// few lines so center them
		int estimated_line_count = length_this_round/(average_max_line_length-estimated_blanks_on_right);
		estimated_line_count++; // looks better to be a bit top heavy than bottom
		uly += (character_height*(line_count-estimated_line_count))/2;
		extra_width = ((long) (max_height/2-labs(center_y-uly))*extra_width_at_center)/max_height;
		if (extra_width < 0) { // new on 140700
			extra_width = 0;
		};
//		max_line_length = (max_width+2*extra_width)/character_width;
	};
   boolean first_line = TRUE;
	if (break_sentences == NEVER_BREAK_SENTENCES) { // rather than break a sentence just keep adding new lines - new on 030600
		line_count = 100;
	};
	for (int lines_left = line_count; lines_left > 0; lines_left--) {
		coordinate new_ulx;
		if (left_clipped) {
			new_ulx = ulx;
		} else {
			new_ulx = ulx-extra_width;
		};
		if (new_ulx < blank_width) { // off the left
			max_width = original_max_width+new_ulx-blank_width;
			new_ulx = blank_width;
		} else if (new_ulx+max_width+blank_width > tt_screen_width) {
		// goes off right side
			max_width = (tt_screen_width-new_ulx)-blank_width;
		};
		city_coordinate full_extra_width = extra_width;
		if (!left_clipped && !right_clipped) full_extra_width += extra_width; // both sides
		int true_max_line_length = (max_width+full_extra_width)/character_width;
		// introduced distinction between true_max_line_length and max_line_length on 221199 in case there are no spaces or other breaks
		// add extra in case most letters are narrow
		// if not then will search backwards for a space and try again
		max_line_length = (4*true_max_line_length)/3;
		if (characters_printed+max_line_length > length) {
			// too long for the text so shorten it
			max_line_length = length-characters_printed+1;
		};
//		if (max_line_length < 3) return(length); // too narrow
//		if (//length_this_round <= max_line_length) { //&& !tt_win31j) {
//			 LOWORD(GetTextExtent(main_device_context,text,length_this_round))
//			  <= max_width+full_extra_width) {
//			TextOutG(main_device_context,new_ulx,uly,text,length_this_round);
//			return(characters_printed+length_this_round);
//		} else {
         int j;
         long extent = max_width; // default 
			for (j = max_line_length-1; j > 0; j--) {
				if (text[j] == '\n' || text[j] == '\r') { // added on 130499 to substitute space for carriage return 
					text[j] = ' ';
				};
				if (text[j] == ' '
//					 && (tt_language != BRAZILIAN_PORTUGUESE || // new on 160600
//					     j < 1 || text[j-1] != '.' || j+1 >= length || text[j+1] < '0' || text[j+1] > '9')) 
					||
					  // for numbers like 1. 024
					text[j] == '_') { 
               // space or hypenated (I use _ to indicate can be hypenated here)
//					if (text[j-1] == ' ') j--; // skip second blank as well 
					// see if this fits
					SIZE size;
					GetTextExtentPoint32A(main_device_context,text,j,&size); // I guess this OK since Marty isn't using tabs or Unicode
               extent = size.cx;
					if (extent <= max_width+full_extra_width) {
                  if (text[j] == '_') {
//                      text[j] = '-';  // done below now on copy
                     j++; // so hypen is on first line
                  };
                  break;
               };
				} else if (phrase_ending_punctuation(text,j,length_this_round,(break_sentences != NEVER_BREAK_SENTENCES)) &&
					// replaced length with length_this_round on 280700 above
                       // so we don't break 1,000,000 at the comma...
                       // if , is the last letter on the line
                       // we don't allow it...
                       // was commented out since long numbers looked bad
                       j < length-1 && (text[j+1] == ' ' || text[j+1] == NULL)) {
					// see if this fits
					SIZE size;
					GetTextExtentPoint32(main_device_context,text,j,&size);
					extent = size.cx;
					if (extent <= max_width+full_extra_width) {
						 // notice above I test length without punctuation
						 // to force it to be on same line even if a little too wide
						 j++; // include punctuation on this line
						 break;
					};
				};
			};
         if (j < 10 && first_line && vertically_center) { // whole thing fits on one line
            new_ulx = ulx+(max_width-extent)/2;  // so center it
         };                     
         int return_location = -1;
		   int i;
         for (i = 0; i < j; i++) {
            if (text[i] == '\r' || text[i] == '\n') { // added text[i] == '\n' on 080802
               text[i] = ' ';
               return_location = i; // to restore the above
               j = i+1; // +1 to include the character (now blank)
               break;
            };
         };
         if (j == 0) { // didn't find a blank so print whole line
//   			j = true_max_line_length;
				// replaced above by following so it fits on 270100
				for (j = min(max_line_length,length_this_round); j > 0; j--) {
					SIZE size;
					GetTextExtentPoint32(main_device_context,text,j,&size);
					if (size.cx <= max_width+full_extra_width) {
						break;
					};
				};
   		};
         if (j > length_this_round) j = length_this_round;
         // why do the following 2 lines each time around 
   		SetBkMode(main_device_context,TRANSPARENT); // for text
         SetTextColor(main_device_context,text_color);
         string new_text = new character[j+1];
         int new_j = 0;
         for (i = 0; i < j-1; i++) {
            if (text[i] != '_') { // remove "ok to hypenate" token
               new_text[new_j++] = text[i];
            };
         };
         if (j > 0) {
            if (text[j-1] == '_') { // turn to hypen since end of line
               new_text[new_j++] = '-';
            } else { // copy it normally
               new_text[new_j++] = text[j-1];
            };
         };
         generic_text_out(new_ulx,uly,new_text,new_j);
         delete [] new_text;
   		uly += character_height;
         first_line = FALSE;
   		extra_width = ((long) (max_height/2-labs(center_y-uly))*extra_width_at_center)/max_height;
			if (extra_width < 0) { // new on 131100 - prevents a crash when standing up and Marty is talking
				extra_width = 0;
			};
   		if (length > j && text[j] == ' ') { // was length_this_round
   			j++;
   			if (length > j && text[j] == ' ') {
   				j++; // second blank too
   			};
   		};
         if (return_location >= 0) {
            text[return_location] = '\n'; // put it back -- was \r prior to 090802
         };
   		characters_printed += j;
   		text += j;
   		length_this_round -= j;
   		if (length_this_round <= 0) break;
//		};
	};
   if (length > characters_printed && add_dots_if_more) {
      // indicate that there is more to say by adding "..." to next line
// this wasn't 
//#if TT_32
//      SIZE size;
//      GetTextExtentPoint32(main_device_context,text,length,&size);
//      coordinate extent = size.cx;
//#else
//    	coordinate extent = LOWORD(GetTextExtent(main_device_context,". . .",5));
//#endif
      generic_text_out(ulx+max_width/4,uly,_T(". . ."),5);
   };
   deselect_font();
END_GDI
#if TT_32&&WING
   GdiFlush(); // ensure 32bit GDI and WinG event order
#endif
   delete [] original_text_copy; // just a local copy
	return(characters_printed);
};

#if TT_32
int MainWindow::place_wide_text(wide_string original_text, 
										  coordinate ulx, coordinate uly,
									     coordinate max_width, coordinate max_height,
									     coordinate new_font_width,
									     coordinate new_font_height,
									     WhenToBreakSentences break_sentences,
									     boolean vertically_center,
                                boolean add_dots_if_more,
									     coordinate extra_width_at_center,
									     coordinate blank_width,
                                color text_color,
                                boolean first_call,
									     long max_length) {
	int length = wide_string_length(original_text);
	if (length == 0) return(0); // new on 200300
	wide_string original_text_copy = copy_wide_string(original_text); // save this pointer to delete it at end
	wide_string text = original_text_copy;
	// check first if this ends with some blanks and if so ignore them
	while (text[length-1] == ' ') {
      length--;
	};
	if (length > max_length) {
		length = last_space_or_punctuation(text,max_length);
		if (length < 0) {
			length = max_length;
		} else {
			length++; // since 0-indexing
		};
	};
   coordinate original_ulx = ulx;
   coordinate original_uly = uly;
BEGIN_GDI
	set_font(new_font_width,new_font_height,TRUE,FALSE);
	coordinate original_max_width = max_width;
	boolean left_clipped = FALSE;
	boolean right_clipped = FALSE;
	if (ulx < blank_width) { // off the left
		// looks better to leave a bit blank on the left
		max_width = original_max_width+ulx-blank_width;
		ulx = blank_width;
		left_clipped = TRUE;
	} else if (ulx+original_max_width+blank_width > tt_screen_width) {
		// goes off right side
		// looks better to leave a bit blank on the right
		max_width = (tt_screen_width-ulx)-blank_width;
		right_clipped = TRUE;
	};
	if (max_width <= character_width) {
// END_GDI was here
//      tt_main_window->
      deselect_font();
END_GDI 
		delete [] original_text_copy;
      return(length);
   };
	uly = tt_screen_height-uly; // but not if WinG in other direction
	coordinate center_y = uly+(max_height-character_height)/2;
	int max_line_length = max_width/character_width;
	int average_max_line_length;
	if (left_clipped || right_clipped) { 
		// lost half of the extra stuff
		average_max_line_length = (max_width+extra_width_at_center/4)/character_width;
	} else {
		average_max_line_length = (max_width+extra_width_at_center/2)/character_width;
	};
	// round up if 1/2 character or more left over
	int characters_printed = 0;
//	int line_count = (max_height+character_height/2)/character_height; // added half to round to nearest on 230500
	int line_count = max_height/character_height;
	coordinate extra_width = 0;
	int length_this_round = length;
	int estimated_blanks_on_right;
	if (length < 6) { // too short to worry much about it fitting
		estimated_blanks_on_right = 0;
	} else if (length < 10) { 
		estimated_blanks_on_right = 1;
	} else {
		// subtracted 3 since words aren't broken
		estimated_blanks_on_right = 3;
	};
	const int estimated_max_length = line_count*(average_max_line_length-estimated_blanks_on_right);
   	// too much text and don't want to break sentences
	if (break_sentences != OK_TO_BREAK_SENTENCES && estimated_max_length < length) {
      // was > (3*estimated_max_length)/4
      int i = estimated_max_length;
      int stop;
      if (break_sentences == BREAK_SENTENCES_TO_NOT_WASTE_HALF_BALLOON) {
         stop = estimated_max_length/2; 
         if (length-i < 20) {
            i = length;
         } else if (i+10 < length) {
            i += 10;
         };
      } else {
         // but first be sure we didn't just chop off the end punctuation
         if (phrase_ending_punctuation(text,i+1,length,(break_sentences != NEVER_BREAK_SENTENCES))) i++;
         stop = 0;
      };
      int first_break_found = 0;
      int second_break_found = 0;
		for (; i > stop; i--) {
			// better to break a sentence than waste 1/2 of the space
			if (phrase_ending_punctuation(text,i,length,(break_sentences != NEVER_BREAK_SENTENCES))) {
            if (first_break_found == 0) {
               first_break_found = i+1;
            } else if (second_break_found == 0) {
               second_break_found = i+1;
               break;
            };
			};
		};
      if (first_break_found == 0) {
         if (break_sentences == BREAK_SENTENCES_TO_NOT_WASTE_HALF_BALLOON) {
				first_break_found = stop;
				while (first_break_found > 0) {
					if (text[first_break_found] == ' ') break;
					first_break_found--;
				};
				if (first_break_found == 0) { // still didn't find a break so search forward instead
					first_break_found = stop;
					while (first_break_found < length) {
						if (text[first_break_found] == ' ') break;
						first_break_found++;
					};
				};
         } else {
            deselect_font();
END_GDI
            // didn't fit so try again with a smaller font
            int result = place_wide_text(text,
												     original_ulx,original_uly,
													  max_width,max_height,
												     (19*new_font_width)/20,
												     (19*new_font_height)/20,
												     break_sentences,
												     vertically_center,
												     add_dots_if_more,
												     extra_width_at_center,
												     blank_width,
												     text_color,
												     FALSE,
												     max_length);  // not first call
				delete [] original_text_copy;
				return(result);
         };
      };
      if (second_break_found == 0 ||
          break_sentences == BREAK_SENTENCES_TO_NOT_WASTE_HALF_BALLOON ||
          first_call) {
         length_this_round = first_break_found;
      } else {
         length_this_round = second_break_found;
      };
   };
	if (vertically_center && max_line_length > 3 &&
		 length_this_round/(average_max_line_length-estimated_blanks_on_right) < line_count) {
		// few lines so center them
		int estimated_line_count = length_this_round/(average_max_line_length-estimated_blanks_on_right);
		estimated_line_count++; // looks better to be a bit top heavy than bottom
//		uly += (character_height*(line_count-estimated_line_count))/2; // commented out on 260100 since often was too bottom heavy
		extra_width = ((long) (max_height/2-labs(center_y-uly))*extra_width_at_center)/max_height;
		if (extra_width < 0) { // new on 120700 but not sure why this happens
			extra_width = 0; 
		};
	};
//   boolean first_line = TRUE;
	// commented out the following on 170700 since in Japanese breaking sentences is OK
//	if (break_sentences == NEVER_BREAK_SENTENCES) { // rather than break a sentence just keep adding new lines - new on 030600
//		line_count = 100;
//	};
	for (int lines_left = line_count; lines_left > 0; lines_left--) {
		coordinate new_ulx;
		if (left_clipped) {
			new_ulx = ulx;
		} else {
			new_ulx = ulx-extra_width;
		};
		if (new_ulx < blank_width) { // off the left
			max_width = original_max_width+new_ulx-blank_width;
			new_ulx = blank_width;
		} else if (new_ulx+max_width+blank_width > tt_screen_width) {
		// goes off right side
			max_width = (tt_screen_width-new_ulx)-blank_width;
		};
		city_coordinate full_extra_width = extra_width;
		if (!left_clipped && !right_clipped) full_extra_width += extra_width; // both sides
		int true_max_line_length = (max_width+full_extra_width)/character_width;
		// introduced distinction between true_max_line_length and max_line_length on 221199 in case there are no spaces or other breaks
		// add extra in case most letters are narrow
		// if not then will search backwards for a space and try again
		max_line_length = true_max_line_length; // (4*true_max_line_length)/3;
		if (characters_printed+max_line_length > length) {
			// too long for the text so shorten it
			max_line_length = length-characters_printed+1;
		};
      int j;
      long extent = max_width; // default
	   for (j = max_line_length-1; j > 0; j--) {
//			if (text[j] > 255) { // Kanji can be broken anywhere
//				SIZE size;
//				GetTextExtentPoint32W(main_device_context,text,j,&size);
//				if (size.cx <= max_width+full_extra_width) {
//					if (phrase_ending_punctuation(text[j])) {
//						j++;
//					};
//					break;
//				};
//			};
			if (text[j] == '\n' || text[j] == '\r') { // added on 130499 to substitute space for carriage return 
				text[j] = ' ';
			};
			if (text[j] == ' ') { // || text[j] == '_') { 
            // space or hypenated (I use _ to indicate can be hypenated here)
				// see if this fits
				SIZE size;
				GetTextExtentPoint32W(main_device_context,text,j,&size);
            extent = size.cx;
				if (extent <= max_width+full_extra_width) {
//               if (text[j] == '_') {
//                  j++; // so hypen is on first line
//               };
               break;
            };
			} else if (phrase_ending_punctuation(text,j,length,(break_sentences != NEVER_BREAK_SENTENCES))) {
				SIZE size;
				GetTextExtentPoint32W(main_device_context,text,j,&size);
				extent = size.cx;
				if (extent <= max_width+full_extra_width) {
					// notice above I test length without punctuation
					// to force it to be on same line even if a little too wide
					j++; // include punctuation on this line
					break;
				};
			};
		};
//       if (j < 10 && first_line && vertically_center) { // whole thing fits on one line
//         new_ulx = ulx+(max_width-extent)/2;  // so center it
//		 };                     
       int return_location = -1;
		 int i;
         for (i = 0; i < j; i++) {
            if (text[i] == '\r' || text[i] == '\n') { // added text[i] == '\n' on 080802
               text[i] = ' ';
               return_location = i; // to restore the above
               j = i+1; // +1 to include the character (now blank)
               break;
            };
         };
         if (j == 0) { // didn't find a blank so print whole line that fits
				// following was j = max_line_length; // prior to 270100
				for (j = min(max_line_length,length_this_round); j > 0; j--) {
					SIZE size;
					GetTextExtentPoint32W(main_device_context,text,j,&size);
					if (size.cx <= max_width+full_extra_width) {
						break;
					};
				};
   		};
         if (j > length_this_round) j = length_this_round;
         // why do the following 2 lines each time around 
   		SetBkMode(main_device_context,TRANSPARENT); // for text
         SetTextColor(main_device_context,text_color);
			// removed all this support for hypenation from wide string version
//         wide_string new_text = new wide_character[j+1];
//         int new_j = 0;
//         for (i = 0; i < j-1; i++) {
//            if (text[i] != '_') { // remove "ok to hypenate" token
//               new_text[new_j++] = text[i];
//            };
//         };
//         if (j > 0) {
//            if (text[j-1] == '_') { // turn to hypen since end of line
//               new_text[new_j++] = '-';
//            } else { // copy it normally
//               new_text[new_j++] = text[j-1];
//            };
//         };
			int length_without_spaces;
			// following is new on 221199 since spaces are in text only to indicate possible breaks
			wide_string text_without_spaces = remove_spaces_if_non_ascii_neighbors(text,j,length_without_spaces);
         TextOutW(main_device_context,new_ulx,uly,text_without_spaces,length_without_spaces);
			delete [] text_without_spaces;
//         delete [] new_text;
   		uly += character_height;
//         first_line = FALSE;
   		extra_width = ((long) (max_height/2-labs(center_y-uly))*extra_width_at_center)/max_height;
			if (extra_width < 0) { // new on 120700
				extra_width = 0;
			};
   		if (length > j && text[j] == ' ') { // was length_this_round
   			j++;
   			if (length > j && text[j] == ' ') {
   				j++; // second blank too
   			};
   		};
         if (return_location >= 0) {
            text[return_location] = '\n'; // put it back -- prior to 090802 was \r
          };
   		characters_printed += j;
   		text += j;
   		length_this_round -= j;
   		if (length_this_round <= 0) break;
//		};
	};
   if (length > characters_printed && add_dots_if_more) {
      generic_text_out(ulx+max_width/4,uly,_T(". . ."),5);
   };
   deselect_font();
END_GDI
   delete [] original_text_copy; // just a local copy
	return(characters_printed);
};
#endif

/* replaced the following with the above (variant of narrow version)
int MainWindow::place_wide_text(wide_string text, 
										  coordinate ulx, coordinate uly,
										  coordinate max_width, coordinate max_height,
										  coordinate new_font_width,
										  coordinate new_font_height,
										  boolean vertically_center,
										  boolean add_dots_if_more,
										  coordinate extra_width_at_center,
										  coordinate blank_width,
										  color text_color,
										  boolean first_call,
										  long max_length) {
	if (max_height < new_font_height) return(0); // could deal with ... here?
	int length = wide_string_length(text);
	if (length == 0) return(0);
	if (length > max_width) {
		length = last_space_or_punctuation(text,max_width,TRUE);
		if (length < 0) {
			length = max_width;
		} else {
			length++; // zero-indexing
		};
	};
	if (new_font_height < minimum_character_pixel_height || new_font_width < minimum_character_pixel_width) return(length); // updated 150799
	int length_without_spaces;
	// following is new on 221199 since spaces are in text only to indicate possible breaks
	wide_string text_without_spaces = remove_spaces_if_non_ascii_neighbors(text,length,length_without_spaces);
BEGIN_GDI
	SIZE extent;
	set_font(new_font_width,new_font_height,TRUE,FALSE);
   GetTextExtentPoint32W(main_device_context,text_without_spaces,length_without_spaces,&extent);
   SetBkMode(main_device_context,TRANSPARENT); // for text
   SetTextColor(main_device_context,tt_colors[text_color]);
	if (extent.cx <= max_width) { // fits so just show it
#if TT_DEBUG_ON
	if (tt_debug_mode == 140799) {
		tt_error_file() << "Place wide text " << text << " length=" << length << endl;
	};
#endif
		TextOutW(main_device_context,ulx,tt_screen_height-uly,text_without_spaces,length_without_spaces);
		deselect_font();
END_GDI
		delete [] text_without_spaces;
		return(length);
	} else { // see how much will fit on a line
		delete [] text_without_spaces;
		int length_this_line = (length*max_width)/extent.cx;
		int good_break = last_space_or_punctuation(text,length_this_line,TRUE);
		if (good_break > length_this_line/2) { 
			// new on 221199 - if find a good break and will waste less than 1/2 of the line use it
			length_this_line = good_break;
		};
	   GetTextExtentPoint32W(main_device_context,text,length_this_line,&extent);
		while (extent.cx > max_width && length_this_line > 1) {
			length_this_line--;
			GetTextExtentPoint32G(main_device_context,text,length_this_line,&extent);
		}; 
		while (length_this_line < length && phrase_ending_punctuation(text[length_this_line])) { // before 221199 was space_or_punctuation
			length_this_line++;
		};
		if (length_this_line+4 >= length) { // just short a few characters - so risk that they fit
			int gap = length-length_this_line;
			coordinate new_ulx = ulx-gap*new_font_width; // to get more room - new on 300999
			if (new_ulx < ulx/3) {
				ulx /= 3; // cut to a third to get more room
			} else {
				ulx = new_ulx; // subtract off gap characters to get more room
			};
			length_this_line = length;
			deselect_font(); // new on 161199
			set_font((9*new_font_width)/10,(9*new_font_height)/10,TRUE,FALSE); // narrow font by 10% to fit better
		};
#if TT_DEBUG_ON
		if (tt_debug_mode == 140799) {
			tt_error_file() << "Place wide text(2) " << text << " length=" << length_this_line << endl;
		};
#endif
		// following is new on 221199 since spaces are in text only to indicate possible breaks
		wide_string text_without_spaces = remove_spaces_if_non_ascii_neighbors(text,length_this_line,length_without_spaces);
		TextOutW(main_device_context,ulx,tt_screen_height-uly,text_without_spaces,length_without_spaces);
		delete [] text_without_spaces;
		deselect_font();
END_GDI
		return(length_this_line+
				 place_wide_text(text+length_this_line, 
									  ulx,uly-new_font_height,
									  max_width,max_height-new_font_height,
									  new_font_width,new_font_height,
									  vertically_center,
									  add_dots_if_more,
									  extra_width_at_center,
									  blank_width,
									  text_color,
									  first_call,
									  max_length));
	};
};
#endif
*/

boolean MainWindow::MCI_send_string(ascii_string message,
                                    boolean report_problems,
                                    ascii_string return_message,
                                    int return_message_length) {
  long error = mciSendStringG(message,return_message,
                              return_message_length,
                              tt_main_window->get_handle());
  if (error != 0) {
//     int say_importance = report_problems?4:0; // is logged if user is debugger
     string full_message = new character[512];
     ascii_string error_message = new char[256];
	  output_string_stream message_stream(full_message,512);
	  message_stream << S(IDS_UNKNOWN_MCI_ERROR) << '"' << message << '"' << end_sentence();
     ascii_string friendlier_message = NULL;
     if (mciGetErrorStringG(error,error_message,256)) {
        // remove error number and stuff
        friendlier_message = strchr(error_message,' ');
        if (friendlier_message != NULL) {
		     message_stream << friendlier_message;
        };
     };
     message_stream << "   ";
     message_stream.put('\0');
	  // commented out the following since say_error does it better (though estimates 30 seconds for limited speech engines)
//#if TT_TEXT_TO_SPEECH
//     if (friendlier_message != NULL && limited_text_to_speech()) { // since can't speak an MCI error message
//		  tt_martian->just_talk_balloons_for(60000L); // for 60 seconds -- should be enough
//     };
//#endif
	  log(message);
     if (report_problems) {
		  // say_error(full_message,TRUE);
		  say(full_message,3,TRUE); // rewritten on 070305 since not a TooTalk error
	  } else {
		  log(full_message);
	  };
     delete [] full_message;
     delete [] error_message;
	  return(FALSE); // went wrong
  } else {
	  MCI_sent_message = TRUE;
	  return(TRUE);
  };
};

HDROP MainWindow::get_drop_handle() {
	if (!IsClipboardFormatAvailable(CF_HDROP)) return(NULL);
	if (OpenClipboard(window_handle) == 0) return(NULL); // clipboard locked
	HDROP result = (HDROP) GetClipboardData(CF_HDROP);
	CloseClipboard();
	return(result);
};

int log_file_names_count = 0;
string *log_file_names_in = NULL;
string *log_file_names_out = NULL;

void release_log_file_names() {
	if (log_file_names_count == 0) return;
	for (int i = 0; i < log_file_names_count; i++) {
		if (log_file_names_in[i] != NULL) delete [] log_file_names_in[i];
		if (log_file_names_out[i] != NULL) delete [] log_file_names_out[i];
	};
	delete [] log_file_names_in;
	delete [] log_file_names_out;
	log_file_names_count = 0;
};

string MainWindow::clipboard_text(boolean &new_text, SpritePointer *sprite_pointer, boolean ignore, boolean initializing_input_log, 
											 boolean check_files_too) {
	// added initializing_input_log on 061003
	// sprite arg added on 290403
	// called if CLIPBOARD_REMOTES are in use
	// ignore added on 080403 to be less wasteful
	// check_files_too new on 280404 so clipboard sensor ignores files on clipboard
	if (tt_frame_number == current_clipboard_frame_number && !initializing_input_log) { // already answered this this frame - new on 310803
		// added  && !initializing_input_log on 061003
		new_text = FALSE; // new on 281199
		return(last_clipboard_string);
	};
	current_clipboard_frame_number = tt_frame_number;
	boolean log_clipboard = (tt_logging && !tt_recording_notebook_for_log && tt_dump_reason == NOT_DUMPING); 
	// added 3rd conjunct on 310803 to prevent bad time travel logs (when tt_saving_media for example)
	if (tt_replay || tt_time_travel == TIME_TRAVEL_ON || tt_time_travel == TIME_TRAVEL_JUST_PAUSED) { 
		// new on 210799 so that clipboard works in DMO files
		// warning this only works if the size of the clipping is less than 64K because strings are dumped with 2 bytes of length
		ClipboardTokens token = replay_clipboard_token();
      switch (token) {
			case NO_CLIPBOARD_CHANGE:
				new_text = FALSE; // new on 281199
				if (log_clipboard) record_no_clipboard_change();
				return(last_clipboard_string);
			case CLIPBOARD_EMPTY:
				if (log_clipboard) record_clipboard(NULL,0);
				last_clipboard_string = NULL;
				return(NULL);
			case CLIPBOARD_STRING_FOLLOWS: {
				string recorded_clipboard_string = replay_clipboard_string();
				if (recorded_clipboard_string == NULL) return(NULL); // new on 110601 - probably too large to log 
            // prior to 111102 to break but if replaying shouldn't look at real clipboard
				last_clipboard_string = recorded_clipboard_string; // this is a fresh copy
				new_text = TRUE; // new on 281199
				if (log_clipboard) record_clipboard(last_clipboard_string,strlen(last_clipboard_string));
				if (ignore) {
					return(NULL); // new on 040803
				} else {
					return(copy_string(last_clipboard_string)); // copies since some callers expect it - not so wasteful now that there is ignore
				};
													 };
			case CLIPBOARD_FILE_NAMES_FOLLOW: 
				// new on 010503
				if (sprite_pointer != NULL) {
					*sprite_pointer = replay_clipboard_file_names();
					new_text = TRUE;
				} else {
					ignore_replay_clipboard_file_names();
				};
				return(NULL);
			default:
				log("Problem reading the log file for clipboard information.");
            tt_error_file() << "Token code is " << (int) token << endl; // new on 091002
				return(NULL);
		};
	};
	boolean text_available = IsClipboardFormatAvailable(CF_TEXT);
   boolean files_available;
	boolean treat_text_as_file = FALSE; // new on 171204
	if (check_files_too) {
		files_available = IsClipboardFormatAvailable(CF_HDROP);
		// tt_shift_down new feature on 171204 to enable pasting of file names and getting their contents
		if (!files_available) {
			treat_text_as_file = tt_shift_down;
		};	
	} else {
		files_available = FALSE;
	};
	if ((!text_available && !files_available) || OpenClipboard(window_handle) == 0) { 
		// warn if someone didn't close clipboard?
		new_text = (last_clipboard_size != -1); // wasn't last time
		if (last_clipboard_frame_number != tt_frame_number) last_clipboard_size = -1;
		if (log_clipboard) record_clipboard(NULL,0);
//		CloseClipboard(); // new on 181204 -- seems right -- removed 191204 since didn't open clipboard (failed above)
		return(NULL);
	};
	string clipboard_string;
	long size;
	long text_length;
	if (files_available && !replaying()) { // new on 141099 -- added  on 111102
		// does this do the right thing with respect to last_clipboard_string??
		HDROP files_handle = (HDROP) GetClipboardData(CF_HDROP);
		if (files_handle != NULL) {
			UINT file_count = DragQueryFile(files_handle,0xFFFFFFFF,NULL,0);
			// for now just deal with single files
			if (file_count == 1) {
				char file_name[MAX_PATH];
				DragQueryFile(files_handle,0,file_name,MAX_PATH);
//				clipboard_string = copy_file_or_url_to_string(file_name,text_length);
				// above rewritten as follows on 290403
				if (sprite_pointer != NULL) {
					boolean aborted = FALSE; // new on 200105
					*sprite_pointer = sprite_from_file_name(file_name,aborted);
					new_text = TRUE; // always?		
				};
				//if (text_length == 0) return(NULL);
				//size = text_length+1;
			} else {
				if (sprite_pointer != NULL) { // new on 010503
					*sprite_pointer = sprites_from_drop_handle(files_handle,TRUE);
					new_text = TRUE; // always?		
				};	
//				files_available = FALSE; // new on 070400 since otherwise if multiple files on clipboard -> trouble
			};
			if (log_clipboard) {
				record_clipboard_files(file_count);
#if TT_DEBUG_ON
			   add_to_log_if_interesting_error();
#endif
				if (log_file_names_count != file_count) {
					release_log_file_names();
					log_file_names_count = file_count;
               log_file_names_in = new string[log_file_names_count];
               log_file_names_out = new string[log_file_names_count];
					for (int i = 0; i < log_file_names_count; i++) {
						log_file_names_in[i] = NULL;
						log_file_names_out[i] = NULL;
					};
				};
				for (unsigned int i = 0; i < file_count; i++) {
					char file_name[MAX_PATH];
					DragQueryFile(files_handle,i,file_name,MAX_PATH);
					record_string(file_name); // used for labelling the box, etc.
					if (log_file_names_in[i] != NULL && strcmp(log_file_names_in[i],file_name) == 0) {
						record_string(log_file_names_out[i]);
					} else {
						string new_name = log_file_name(file_name); // new on 140204 to generate a hash name and keep a copy of the file in the archive
						record_string(new_name);
						if (log_file_names_in[i] == NULL && new_name != NULL) {
							log_file_names_in[i] = copy_string(file_name);
							log_file_names_out[i] = copy_string(new_name);
						};
						if (new_name != NULL) delete [] new_name; // conditional new on 200204 for robustness
					};
				};
			};
			CloseClipboard();
			return(NULL);
		} else {
			files_available = FALSE;
		};
	};
	if (!files_available || replaying()) { // tt_replay new on 111102
		HGLOBAL text_handle;
		if (tt_unicode_clipboard && !files_available) {
			text_handle = GetClipboardData(CF_UNICODETEXT); // new on 101203 to support pasting Unicode
		} else {
			text_handle = GetClipboardData(CF_TEXT);
		};
		if (text_handle == NULL) {
			CloseClipboard();
			new_text = (last_clipboard_size != -1); // wasn't last time
			if (last_clipboard_frame_number != tt_frame_number) last_clipboard_size = -1;
			if (log_clipboard) record_clipboard(NULL,0);
			return(NULL); // not there anymore
		};
		clipboard_string = (ascii_string) GlobalLock(text_handle); 
		size = GlobalSize(text_handle);
		if (clipboard_string == NULL) {
			// new on 120105 for robustness -- if code to put on clipboard screws up
			text_length = 0;
		} else {
			text_length = _tcslen(clipboard_string);
		};
		if (text_length < size) size = text_length+1;
		GlobalUnlock(text_handle); // leave it there
	};
	if (treat_text_as_file && clipboard_string != NULL) { // new on 171204
		if (sprite_pointer != NULL) {
			string local_file_name = NULL;
			boolean aborted = FALSE; // new on 200105
			*sprite_pointer = sprite_from_file_name(clipboard_string,aborted,NULL,&local_file_name);
			if (*sprite_pointer == NULL) {
				CloseClipboard();
				if (!aborted) {
					tt_error_file() << "Control-shift-v entered so tried but failed to find the file: " << clipboard_string << endl;
				};
				new_text = (last_clipboard_size != -1);
				if (last_clipboard_frame_number != tt_frame_number) last_clipboard_size = -1;
				if (log_clipboard) record_clipboard(NULL,0);
				return(NULL);
			};
			if (log_clipboard) { // new on 181204
				record_clipboard_files(1);
				record_string(clipboard_string);
				string new_name;
				if (local_file_name == NULL) {
					// generate a hash name and keep a copy of the file in the archive
					new_name = log_file_name(clipboard_string); // already is a local file name
				} else {
					int extension_start = extension_start_position(clipboard_string);
					if (extension_start > 0) {
						new_name = log_file_name(local_file_name,clipboard_string+extension_start-1); // including dot
					} else {
						new_name = log_file_name(local_file_name,"");
					};
				};
				record_string(new_name);
				if (new_name != NULL) delete [] new_name;
			};
			CloseClipboard();
			return(NULL);
		};
	};
	string result;	
	new_text = (last_clipboard_size != size || last_clipboard_string == NULL || // second condition added 250100
					_tcsncmp(last_clipboard_string,clipboard_string,size) != 0);

	if (new_text) {
		if (files_available) {
			result = clipboard_string;
		} else {
			if (tt_unicode_clipboard) {
				 result = copy_narrow_string((wide_string) clipboard_string); // new on 101203
				 size = strlen(result);
			} else {
				result = new character[size];
				memcpy(result,clipboard_string,size*sizeof(character));
			};
		};
		last_clipboard_size = size;
		if (last_clipboard_string != NULL) delete [] last_clipboard_string;
		last_clipboard_string = copy_string(result,size); 
		// copy_string added 281199 since when new caller deletes result when done with it -- added size arg on 300303 to be a bit faster
		if (log_clipboard) record_clipboard(result,text_length);
	} else {
		result = last_clipboard_string;
		if (log_clipboard) record_no_clipboard_change();
	};
	CloseClipboard();
	return(result);
};

void MainWindow::release_last_clipboard_string() { // new on 111102 since used to make logs independent of previous clipboard setting
   last_clipboard_size = 0;
   if (last_clipboard_string != NULL) {
      delete [] last_clipboard_string;
      last_clipboard_string = NULL;
   };
};

boolean MainWindow::add_to_clipboard(string text, long length, string suffix) {
#if TT_DEBUG_ON
	DWORD start;
	if (tt_debug_mode == 120105) {
		start = timeGetTime();
	};
#endif
	if (text == NULL) return(FALSE); // new on 140105 for robustness
//	if (OpenClipboard(window_handle) == 0) return(FALSE); // moved on 120105
	HGLOBAL text_handle = global_alloc(GHND, length+1);
	if (text_handle == NULL) return(FALSE); // warn??
	string clipboard_text = (string) GlobalLock(text_handle);
	memcpy(clipboard_text,text,length);
//	clipboard_text[length] = 0; // be sure its terminated -- GHND assures this
	GlobalUnlock(text_handle);
	save_clipping_to_file(text_handle,suffix); // moved here on 120105
	if (OpenClipboard(window_handle) == 0) {
		GlobalFree(text_handle); // new on 120105
		return(FALSE);
	};
	EmptyClipboard();
	SetClipboardData(CF_TEXT,text_handle);
	CloseClipboard();
//	save_clipping_to_file(text_handle,suffix);
	// commented on 120105 since seems to cause crashes due to memory corruption
//	GlobalFree(text_handle); // new on 120105 -- Microsoft documentation is confusing about whether this is needed -- sample code doesn't do it
#if TT_DEBUG_ON
	if (tt_debug_mode == 120105) {
		tt_error_file() << "Just added " << length << " characters to the clipboard on frame " << tt_frame_number << endl;
		tt_error_file() << "It took " << (timeGetTime()-start) << "ms" << endl;
	};
#endif
	return(TRUE);
};

boolean MainWindow::add_to_clipboard(HGLOBAL text_handle, string suffix) {
	if (OpenClipboard(window_handle) == 0) return(FALSE);
	if (text_handle == NULL) return(FALSE); // warn??
	save_clipping_to_file(text_handle,suffix); // moved here on 250400 to be sure that text_handle still has data
	EmptyClipboard();
	SetClipboardData(CF_TEXT,text_handle);
	CloseClipboard();
//	GlobalFree(text_handle); // new on 120105 -- all callers are OK with this but be careful
	return(TRUE);
};

boolean MainWindow::clear_clipboard() {
	if (OpenClipboard(window_handle) == 0) return(FALSE);
	EmptyClipboard();
	CloseClipboard();
	last_clipboard_frame_number = tt_frame_number;
	return(TRUE);
};

#if DISPDIB
void load_dispdib() {
	if (display_dib_library != NULL) return; // already loaded
//	if (tt_debug_mode == 67) {
//		DisplayDib = DisplayDibNoOp;
//	} else {
		display_dib_library = LoadLibrary("dispdib.dll");
#if TT_32
		if (display_dib_library != NULL) {
#else
		if (display_dib_library >= HINSTANCE_ERROR) {
#endif
			DisplayDib = (LPDISDIB) GetProcAddress(display_dib_library,"DisplayDib");
		} else {
			tt_error_file() << "Can't load DISPDIB.DLL error code = " 
							<< (UINT) display_dib_library << endl;
//			tt_using_dispdib = 2;
         tt_exclusive_screen_desired = FALSE;
         tt_window_expansion_factor = 2;
			write_ini_entry(C(IDC_SWITCHES),C(IDC_FULL_SCREEN_MODE_STARTED),"0");
			tt_error_file() << "This is normal if installed on Windows NT." << endl;
//			exit(FALSE);
		};
//	};
};
#endif

//void restore_background(int index) {
//	saved_background_index = -1;
//	tt_main_window->display_background(index);
//};

void cache_background() {
#if TT_DIRECTX
  lock_back_surface();
  copy_work_page(tt_destination_work_page,tt_background_page,
                 tt_destination_width*tt_bytes_per_pixel,tt_screen_width*tt_bytes_per_pixel);
  unlock_back_surface();
#else
  HMEMCPY(tt_background_page,tt_destination_work_page,tt_screen_bytes_size);
#endif
};

void display_background_cache() {
#if TT_DIRECTX
  lock_back_surface();
  copy_work_page(tt_background_page,tt_destination_work_page,
                 tt_screen_width*tt_bytes_per_pixel,tt_destination_width*tt_bytes_per_pixel);
  // pitch may be greater than width
//  for (int i = 0; i < tt_screen_height; i++) {
//     memcpy(tt_destination_work_page,
//			   tt_background_page,
//			   tt_screen_width);
//     tt_destination_work_page += tt_destination_width;
//     tt_background_page += tt_screen_width;
//  };
  unlock_back_surface();
#else
  HMEMCPY(tt_destination_work_page,tt_background_page,tt_screen_bytes_size);
#endif
};

/*
	saved_background_resource
		 = LoadResource(tt_library,
							 FindResource(tt_library,
											  MAKEINTRESOURCE(bitmap_index),RT_BITMAP));
	saved_background_page = (work_page) LockResource(saved_background_resource);
	if (saved_background_page == NULL) {
		tt_error_file() << "Problems loading background #" 
						<< bitmap_index << endl;
		return;
	};
	saved_background_page += dib_header_size; // remove header stuff
	if (!tt_using_dispdib) {
		for (long i = 0; i < tt_screen_bytes_size; i++) {
			saved_background_page[i] = tt_color_permutation[saved_background_page[i]];
		};
	};

};
*/

void MainWindow::Paint() {
#if TT_DEBUG_ON
   if (tt_debug_mode == 170502) {
      tt_error_file() << "Paint() called while have_focus is " << (int) have_focus 
                  << " and tt_mouse_acquired is " << (int) tt_mouse_acquired 
						<< " and pause is " << (int) paused
						<< " on frame " << tt_frame_number << endl;
   };
#endif
	PAINTSTRUCT ps;
#if TT_DIRECTX
    HDC visible_device_context = BeginPaint(window_handle,&ps);
	 if (!has_initialization_completed() || IsIconic(window_handle)) { // IsIconic condition new on 250602
		  // this conjunct added on 080600 for message boxes while initializing - moved here on 260900
//		 tt_main_window->set_font(36,24,TRUE);
//BEGIN_GDI
//		 if (TextOut(main_device_context,200,200,"Loading",7) == 0) {
//			 int x = GetLastError();
//			 tt_error_file() << x << " is the last error." << endl;
//		 };
//END_GDI
//		flip_surfaces(TRUE);
#if TT_DEBUG_ON
		if (tt_debug_mode == 170502) {
			tt_error_file() << "Paint ignored on frame " << tt_frame_number << endl;
		};
#endif
	 } else 
		 if (//!tt_exclusive_screen && // removed on 110699 this was causing colors of imported images to come out wrong
		     !paused) {
			if (tt_bits_per_pixel == 8 && (tt_mouse_acquired || have_focus)) { // || have_focus added 061199 so palette is OK when mouse released
				// added tt_bits_per_pixel on 220901
				if (SelectPalette(visible_device_context,palette_handle,FALSE) == NULL) {
					log("Windows reported an error from SelectPalette",FALSE,TRUE);
				};
				if (RealizePalette(visible_device_context) == GDI_ERROR) {
					log("Windows reported an error from RealizePalette",FALSE,TRUE);
				};
			};
			if (tt_mouse_acquired) {
				flip_surfaces(TRUE);
			} else if (tt_time_travel != TIME_TRAVEL_OFF) {
				if (tt_time_travel == TIME_TRAVEL_PAUSED) {
					tt_screen->invalidate_cache();
					tt_screen->display(FALSE); // restore_regions(); // new on 300100 
					// arg was TRUE prior to 170603 but that now means that the background isn't always painted when jumping between segments
				};
				flip_surfaces(!replaying());
			};
		};
    EndPaint(window_handle,&ps);
    return;
#else
#if TT_DEBUG_ON
	long start_time = timeGetTime();
#endif
#if GRAPHICS_TO_MEMORY
	HDC visible_device_context = BeginPaint(window_handle,&ps);
   if (visible_device_context == NULL) {
      return; // warn?? - quit?? -- This happened running TT16 with 24-bit colors on start up
   };
   if (need_to_select_palette) {
       SelectPalette(visible_device_context,palette_handle,FALSE);
		 RealizePalette(visible_device_context);
       need_to_select_palette = FALSE;
   };
   if (exclusive_screen_desired() && !paused) {
		BeginPaint(window_handle,&ps);
		EndPaint(window_handle,&ps);
		return;
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 444) full_screen = FALSE;
#endif
	if (tt_dirty_min_y < tt_dirty_max_y) { // something has changed
		long dirty_width, dirty_height;
//		if (display_whole_next_frame) {
//			dirty_width = tt_screen_width;
//			dirty_height = tt_screen_height;
//			tt_screen->screen_dirty();
//			if (!full_screen || paused) {
//				visible_device_context = GetDC(window_handle);
//			};
//		} else {
			dirty_width = tt_dirty_max_x-tt_dirty_min_x;
			dirty_height = tt_dirty_max_y-tt_dirty_min_y;
//		};
		if (full_screen && !paused) {	// use the whole screen instead
			// don't understand why the following is necessary...
			if (tt_bits_per_pixel == 8) SelectPalette(visible_device_context,palette_handle,FALSE);
			visible_device_context = GetDC(NULL);
		};
		SetStretchBltMode(visible_device_context,COLORONCOLOR);
		int dirty_client_width = (dirty_width*client_width)/tt_screen_width;
		int dirty_client_height = (dirty_height*client_height)/tt_screen_height;
		int dirty_client_min_x = (((long) tt_dirty_min_x)*client_width)/tt_screen_width;
		int dirty_client_max_y = (((long) tt_dirty_max_y)*client_height)/tt_screen_height;
		if (tt_city->stopped() && tt_darken_when_stopped) darken(BLACK);
//		draw_frame(tt_gray);  // restored 230402 - commented out on 290302
      if (tt_mouse_mouse == SIMULATED_DIRECTIONAL_PAD_MOUSE_MODE) {
         tt_main_window->draw_directional_pad();
      };
//    tt_main_window->show_subtitle();
		if (tt_bits_per_pixel == 8) {
			SelectPalette(visible_device_context,palette_handle,FALSE);
			RealizePalette(visible_device_context);
		};
#if WING
//		RealizePalette(main_device_context);
		// optimize and deal with stretching later 
		if (client_width == tt_screen_width && client_height == tt_screen_height) {
			WinGBitBlt(visible_device_context,
						  dirty_client_min_x,client_height-dirty_client_max_y,
						  dirty_client_width,dirty_client_height,
						  main_device_context,
						  tt_dirty_min_x,client_height-dirty_client_max_y);
		} else {
			WinGStretchBlt(visible_device_context,
								dirty_client_min_x,client_height-dirty_client_max_y,
								dirty_client_width,dirty_client_height,
								main_device_context,
								tt_dirty_min_x,tt_screen_height-tt_dirty_max_y,
								dirty_width,dirty_height);
		};
//	  tt_error_file() << dirty_client_min_x << "," << client_height-dirty_client_max_y 
//					  << " " << dirty_client_width << "x" << dirty_client_height << endl
//					  << tt_dirty_min_x << "," << tt_screen_height-tt_dirty_max_y << endl;
//					  << " " << dirty_width << "x" << dirty_height << endl
#elif DISPDIB
		if (exclusive_screen_desired()) { // but paused
			StretchDIBits(visible_device_context,
							  dirty_client_min_x,client_height-dirty_client_max_y,
							  dirty_client_width,dirty_client_height,
							  tt_dirty_min_x,tt_dirty_min_y,
							  dirty_width,dirty_height,
							  tt_memory_graphics_work_page,default_header,
							  DIB_RGB_COLORS,SRCCOPY);
		} else { 
			StretchDIBits(visible_device_context,
							  dirty_client_min_x,client_height-dirty_client_max_y,
							  dirty_client_width,dirty_client_height,
							  tt_dirty_min_x,tt_dirty_min_y,
							  dirty_width,dirty_height,
							  tt_memory_graphics_work_page,header_with_identity_palette,
							  DIB_PAL_COLORS,SRCCOPY);
		};
#endif
//		if (display_whole_next_frame) {
//			display_whole_next_frame = FALSE;
//			if (!full_screen) {
//				ReleaseDC(window_handle,visible_device_context);
//			};
//		};
// commented out the following on 9/10/97 since visible_device_context came from BeginPaint
//		if (full_screen) {
//			ReleaseDC(NULL,visible_device_context);
//		};
		EndPaint(window_handle,&ps);
	};
#else
	 PAINTSTRUCT ps;
	 HDC visible_device_context = BeginPaint(window_handle,&ps);
	 BitBlt(visible_device_context,tt_dirty_min_x,tt_dirty_min_y,
			  tt_dirty_max_x-tt_dirty_min_x,tt_dirty_max_y-tt_dirty_min_y,
			  tt_hidden_device_context,tt_dirty_min_x,tt_dirty_min_y,SRCCOPY);
	 EndPaint(window_handle,&ps);
#endif
#if TT_DEBUG_ON
	 if (tt_debug_mode == 3) {
		 tt_error_file() << (timeGetTime()-start_time) 
						<< " milliseconds to paint the window." << endl;
	 };
#endif
#endif // from directx
};

// from WING Help file
HPALETTE CreateIdentityPalette(RGBQUAD aRGB[], int nColors,
                               int red_delta, int green_delta, int blue_delta)
{
	int i;
	struct {
		WORD Version;
		WORD NumberOfEntries;
		PALETTEENTRY aEntries[256];
	} Palette =
	{
		0x300,
		256
	};

	//*** Just use the screen DC where we need it
	HDC hdc = GetDC(NULL);

	//*** For SYSPAL_NOSTATIC, just copy the color table into
	//*** a PALETTEENTRY array and replace the first and last entries
	//*** with black and white
	if (GetSystemPaletteUse(hdc) == SYSPAL_NOSTATIC)

	{
		//*** Fill in the palette with the given values, marking each
		//*** as PC_NOCOLLAPSE
		for(i = 0; i < nColors; i++)
		{
			Palette.aEntries[i].peRed = (byte) max(0,min(255,aRGB[i].rgbRed+red_delta));
			Palette.aEntries[i].peGreen = (byte) max(0,min(255,aRGB[i].rgbGreen+green_delta));
			Palette.aEntries[i].peBlue = (byte) max(0,min(255,aRGB[i].rgbBlue+blue_delta));
			Palette.aEntries[i].peFlags = PC_NOCOLLAPSE;
		}

		//*** Mark any unused entries PC_NOCOLLAPSE
		for (; i < 256; ++i)
		{
			Palette.aEntries[i].peFlags = PC_NOCOLLAPSE;

		}

		//*** Make sure the last entry is white
		//*** This may replace an entry in the array!
		Palette.aEntries[255].peRed = 255;
		Palette.aEntries[255].peGreen = 255;
		Palette.aEntries[255].peBlue = 255;
		Palette.aEntries[255].peFlags = 0;

		//*** And the first is black
		//*** This may replace an entry in the array!
		Palette.aEntries[0].peRed = 0;
		Palette.aEntries[0].peGreen = 0;
		Palette.aEntries[0].peBlue = 0;
		Palette.aEntries[0].peFlags = 0;

	}
	else
	//*** For SYSPAL_STATIC, get the twenty static colors into
	//*** the array, then fill in the empty spaces with the
	//*** given color table
	{
		int nStaticColors;
		int nUsableColors;

		//*** Get the static colors from the system palette
		nStaticColors = GetDeviceCaps(hdc, NUMCOLORS);
		GetSystemPaletteEntries(hdc, 0, 256, Palette.aEntries);

		//*** Set the peFlags of the lower static colors to zero
		nStaticColors = nStaticColors / 2;

		for (i=0; i<nStaticColors; i++)
			Palette.aEntries[i].peFlags = 0;

		//*** Fill in the entries from the given color table
		nUsableColors = nColors - nStaticColors;
		for (; i<nUsableColors; i++)
		{
			Palette.aEntries[i].peRed = (byte) max(0,min(255,aRGB[i].rgbRed+red_delta));
			Palette.aEntries[i].peGreen = (byte) max(0,min(255,aRGB[i].rgbGreen+green_delta));
			Palette.aEntries[i].peBlue = (byte) max(0,min(255,aRGB[i].rgbBlue+blue_delta));
			Palette.aEntries[i].peFlags = PC_NOCOLLAPSE;
		}

		//*** Mark any empty entries as PC_NOCOLLAPSE

		for (; i<256 - nStaticColors; i++)
			Palette.aEntries[i].peFlags = PC_NOCOLLAPSE;

		//*** Set the peFlags of the upper static colors to zero
		for (i = 256 - nStaticColors; i<256; i++)
			Palette.aEntries[i].peFlags = 0;
//not sure why this causes problems for the 32 bit version
//#if !TT_32
// added by Ken Kahn since these colors match system colors
		for (i=0; i<nStaticColors; i++) {
			aRGB[i].rgbRed = Palette.aEntries[i].peRed;
			aRGB[i].rgbGreen = Palette.aEntries[i].peGreen;
			aRGB[i].rgbBlue = Palette.aEntries[i].peBlue;
		};
		for (i = 256 - nStaticColors; i<256; i++) {
			aRGB[i].rgbRed = Palette.aEntries[i].peRed;
			aRGB[i].rgbGreen = Palette.aEntries[i].peGreen;
			aRGB[i].rgbBlue = Palette.aEntries[i].peBlue;
		};
//#endif
	}

	//*** Remember to release the DC!
	ReleaseDC(NULL, hdc);

	//*** Return the palette
	return CreatePalette((LOGPALETTE *)&Palette);
};

/* identical to what is now in wingutil.c
void ClearSystemPalette(void)
{
	//*** A dummy palette setup
	struct
	{
		WORD Version;
		WORD NumberOfEntries;
		PALETTEENTRY aEntries[256];
	} Palette =
	{
		0x300,
		256
	};

	HPALETTE ScreenPalette;
	HDC ScreenDC;
	int Counter;

	//*** Reset everything in the system palette to black
	for(Counter = 0; Counter < 256; Counter++)
	{
		Palette.aEntries[Counter].peRed = 0;
		Palette.aEntries[Counter].peGreen = 0;
		Palette.aEntries[Counter].peBlue = 0;


		Palette.aEntries[Counter].peFlags = PC_NOCOLLAPSE;
	}

	//*** Create, select, realize, deselect, and delete the palette
	ScreenDC = GetDC(NULL);
	ScreenPalette = CreatePalette((LOGPALETTE *)&Palette);
	if (ScreenPalette)
	{
		ScreenPalette = SelectPalette(ScreenDC,ScreenPalette,FALSE);
		RealizePalette(ScreenDC);
		ScreenPalette = SelectPalette(ScreenDC,ScreenPalette,FALSE);
		DeleteObject(ScreenPalette);
	}
	ReleaseDC(NULL, ScreenDC);
};
*/

boolean currently_paused() {
  return(paused);
};

void set_currently_paused(boolean flag) { // new on 010205 to reuse this while supporting browsing/help in the dialogs while initializing
	paused = flag;
};

// following stuff is new on 250602 to work around a bug on 98 and Me (and probably 95)
boolean postponed_pause_toontalk = FALSE;
boolean postponed_pause_toontalk_interrupted = FALSE;
boolean postponed_pause_toontalk_ask = FALSE;
boolean postponed_pause_toontalk_shutting_down = FALSE;

void postpone_pause_toontalk(boolean interrupted, boolean ask, boolean shutting_down) {
#if TT_DEBUG_ON
   if (tt_debug_mode == 130502) {
      tt_error_file() << "Postponing pause_toontalk. Frame is " << tt_frame_number << endl;
   };
#endif
   postponed_pause_toontalk = TRUE;
   postponed_pause_toontalk_interrupted = interrupted;
   postponed_pause_toontalk_ask = ask;
   postponed_pause_toontalk_shutting_down = shutting_down;
};

void run_postponed_pause_toontalk() {
   if (!postponed_pause_toontalk) return;
#if TT_DEBUG_ON
   if (tt_debug_mode == 130502) {
      tt_error_file() << "About to run postponed pause_toontalk. Frame is " << tt_frame_number << endl;
   };
#endif
   postponed_pause_toontalk = FALSE;
   pause_toontalk(postponed_pause_toontalk_interrupted,postponed_pause_toontalk_ask,postponed_pause_toontalk_shutting_down);
};

void pause_toontalk(boolean interrupted, boolean ask, boolean shutting_down) {
   if (paused || tt_main_window == NULL) return;
   if (shutting_down) {
      ::CloseWindow(tt_main_window->get_handle()); // new on 291002 to be sure it is out of the way in case there is a dialog buried underneath
		if (tt_zip_description_archive_element != NULL && !user_wants_to_quit) {
			// second condition needed otherwise puzzle demos have a 0.xml in them and confusion results
			dump_xml_to_zip_file(tt_zip_description_archive_element,"0.xml"); // new on 250903 so if abnormal exit the log is updated
		};
   };
	// experiment on 310103 to re-order these - not clear why this isn't better
	toggle_pause(interrupted,shutting_down,ask); // shutting_down introduced on 191199 - was FALSE earlier
};

//void unpause_toontalk_soon() {
  // used to restore ToonTalk after a dialog -- needs to happen on beginning of a cycle
//  need_to_unpause_toontalk = TRUE;
//};

void unpause_toontalk() {
   if (tt_main_window == NULL) return;
	if (!initialization_completed) {
		paused = FALSE; // new on 010205
		return; // too early
	};
   if (!paused) { // is already unpaused but be sure it has the focus
//#if TT_DEBUG_ON
//      if (tt_debug_mode == 1992) log("Setting focus since already unpaused.");
//#endif
//      need_to_set_focus = TRUE;
//        SetFocus(tt_main_window->get_handle());
   } else {
      toggle_pause();
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         log("Just toggled_pause.");
      };
#endif
   };
	if (tt_trouble_shoot_java_applet_file_name != NULL && !file_exists(tt_trouble_shoot_java_applet_file_name,FALSE)) { 
		// new on 170201 - moved here on 180201
		trouble_shoot(TROUBLE_SHOOT_JAVA_APPLET,NULL,NULL,TRUE,FALSE,TRUE); // is regardless of whether user has seen this before a good policy??
		delete [] tt_trouble_shoot_java_applet_file_name;
		tt_trouble_shoot_java_applet_file_name = NULL;
	};
	reset_grid(); // new on 190105 to be sure nothing odd happens when returning to ToonTalk
	tt_screen->screen_dirty();
};

flag toggling_pause = FALSE;

void toggle_pause(boolean interrupted, boolean shutting_down, boolean ask) {
#if TT_DEBUG_ON
   if (tt_debug_mode == 130502) {
      tt_error_file() << "toggle_pause called at time " << timeGetTime() << " (frame=" << tt_frame_number << ")" << endl;
		tt_error_file() << "interrupted is " << (int) interrupted << " and shutting_down is " << (int) shutting_down 
							 << " and ask is " << (int) ask << endl;
   };
#endif
   if (toggling_pause) return; // no recursive calls
   if (!initialization_completed) return; // too early
//   if (waiting_for_user_dialog) return; // screen saver with dialog up does this
   toggling_pause = TRUE;
#if TT_DEBUG_ON
   if (tt_debug_mode == 130502) {
      tt_error_file() << "toggling_pause is now " << (int) toggling_pause << endl;
   };
#endif
	// noticed following is obsolete on 211003
//   boolean replaying_log = (replaying());
	// capture this here in case this is turned off herein - Java applets can be created otherwise - new on 170702
   mouse_buttons = 0; // reset multiple mouse button stuff
#if TT_DEBUG_ON
	add_to_log_if_interesting_error(1008,"start of toggle_pause"); // don't know why 1008 occurs
#endif
   flush_input_events();
   simulate_mouse_up();
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	if (tt_screen != NULL) tt_screen->screen_dirty();
 	paused = !paused;
#if TT_DEBUG_ON
   if (tt_debug_mode == 1992 || tt_debug_mode == 51001 || tt_debug_mode == 130502) {
      if (paused) {
         tt_error_file() << "Pause now true at time " << timeGetTime() << " (frame=" << tt_frame_number << ")"  << endl;
      } else {
         tt_error_file() << "Pause now false at time " << timeGetTime() << " (frame=" << tt_frame_number << ")"  << endl;
      };
   };
	add_to_log_if_interesting_error(1008);
#endif
	if (paused) {
		if (tt_real_time_pause_began != 0) {
			tt_real_time_pause_began = timeGetTime(); // new on 130204 to deal better with timing and pauses
		};
	} else {
		tt_real_time_pause_ended = timeGetTime(); // new on 200204
		tt_current_time_pause_ended = tt_current_time; // new on 220204
#if TT_DEBUG_ON
		if (tt_debug_mode == 20105) {
			tt_error_file() << "tt_current_time_pause_ended just set to " << tt_current_time_pause_ended << endl;
		};
#endif
	};
#if TT_TEXT_TO_SPEECH
	if (paused) {
		stop_speaking();
	};
#endif
	if (paused) stop_sound(TRUE);
#if TT_DIRECT_INPUT
   if (paused) { // new on 110602
      stop_force_feedback_effect();
   };
#endif
   if (!paused && !tt_shutting_down) { 
//      initialize_direct_draw(); // restore it
#if TT_ENHANCED_DRAG_AND_DROP
		// moved here on 020105 since want this to be active before full screen mode kicks in
		register_drag_drop(tt_main_window->get_handle()); // new on 301204 to accept OLE drag and drop (e.g. from a browser)
#endif
      OpenIcon(tt_main_window->get_handle());
   };
#if TT_DIRECTX
//   if ((!paused && tt_using_dispdib == 1) ||
//       (paused && saved_using_dispdib == 1)) {
//      if (paused) {
//         tt_using_dispdib = saved_using_dispdib;
//         saved_using_dispdib = 0;
//      } else {
//         saved_using_dispdib = tt_using_dispdib;
//         tt_using_dispdib = 0;
//      };
//      initialize_direct_draw();
//   };
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
   if (!paused && !tt_shutting_down) {
//      initialize_direct_draw(); // restore it
// commented out the conditional below so that it bltter is locked will work when resumed (even if not full screen)
//      if (tt_exclusive_screen) { // && direct_draw != NULL) {
         tt_main_window->initialize_direct_draw();
#if TT_DEBUG_ON
         if (tt_debug_mode == 130502) {
            log("Initialize direct draw from toggle_pause.");
         };
         add_to_log_if_interesting_error(0,"after initialize_direct_draw in toggle_pause");
         if (tt_debug_mode == 1992 || tt_debug_mode == 51001) log(_T("Direct Draw initialized"));
#endif
//         get_exclusive_control();
//         set_display_mode();
//         if (tt_screen_width == 320) {
//            dderr(direct_draw->SetDisplayMode(tt_screen_width*2,tt_screen_height*2,8),
//                  "restoring display mode");
//         } else {
//            dderr(direct_draw->SetDisplayMode(tt_screen_width,tt_screen_height,8),
//                  "restoring display mode");
//         };
//      };
			if (!tt_exclusive_screen) { // conditional new on 070401 since why try this if just initialized DirectX?
				restore_surfaces();
#if TT_DEBUG_ON
            add_to_log_if_interesting_error(0,"after restore_surfaces in toggle_pause");
				if (tt_debug_mode == 1992 || tt_debug_mode == 51001) log(_T("Surfaces restored"));
#endif
			};
//		initialize_sound(); // new on 100700
      resume_repeating_sound();
      // could implement and call resume_force_effect here
//		initialize_text_to_speech(); // new on 100700
//		SetActiveWindow(tt_main_window->get_handle()); // new on 290602 since on Win98 don't have the mouse focus
		SetFocus(tt_main_window->get_handle()); // new on 290602 since on Win98 don't have the mouse focus 
		// -- not clear this really fixed the problem - but was really fixed on 030702
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
   } else {
      release_mouse_capture();
#if TT_DEBUG_ON
      if (tt_debug_mode == 1992 || tt_debug_mode == 51001) tt_error_file() << "Mouse released." << endl;
      add_to_log_if_interesting_error(0,"after release_mouse_capture in toggle_pause");
#endif
      if (direct_draw != NULL) { // may be during initialization -- 
         if (!shutting_down && !ask) { // added !ask on 250602
#if TT_DEBUG_ON
				if (tt_debug_mode == 130502) {
					tt_error_file() << "in toggle_pause about to call flip_to_GDI_surface at time " << timeGetTime() 
										 << " (frame=" << tt_frame_number << ")"  << endl;
				};
#endif
            flip_to_GDI_surface();
#if TT_DEBUG_ON
            add_to_log_if_interesting_error(0,"after flip_to_GDI_surface in toggle_pause");
            if (tt_debug_mode == 1992 || tt_debug_mode == 51001) log(_T("Flipped to GDI surface"));
#endif
         };
         if (tt_exclusive_screen) {
#if TT_DEBUG_ON
            add_to_log_if_interesting_error(0,"before RestoreDisplayMode in toggle_pause");
            if (tt_debug_mode == 1992 || tt_debug_mode == 51001) log(_T("restoring display mode"));
#endif
#if TT_DEBUG_ON
				if (tt_debug_mode == 130502) {
					tt_error_file() << "in toggle_pause about to call RestoreDisplayMode at time " << timeGetTime() 
										 << " (frame=" << tt_frame_number << ")"  << endl;
				};
#endif
            dderr(direct_draw->RestoreDisplayMode(),_T("restoring display mode"));
//             release_exclusive_control();
			}; // release direct draw even if not exclusive now
#if TT_DEBUG_ON
	      if (tt_debug_mode == 1992 || tt_debug_mode == 51001) log(_T("releasing direct draw"));
         add_to_log_if_interesting_error(0,"before release_direct_draw in toggle_pause");
#endif
#if TT_DEBUG_ON
				if (tt_debug_mode == 130502) {
					tt_error_file() << "in toggle_pause about to call release_direct_draw at time " << timeGetTime() 
										 << " (frame=" << tt_frame_number << ")"  << endl;
				};
#endif
			release_direct_draw(TRUE); // - releases DirectDraw object now (160502) - was FALSE); // re-initialize when resumed
//				 tt_exclusive_screen = TRUE; // restore it from release_exclusive_control
//			 };
      };
		release_user_sound_table(); // new on 201199 so these resources are released while the "user is away"
#if TT_DEBUG_ON
		add_to_log_if_interesting_error(0,"after release_user_sound_table in toggle_pause");
#endif
//		release_text_to_speech(); // new on 100700 since this holds on to direct_sound object
//		release_direct_sound(); // new on 100700 so while minimized sound device is completely free
   };
#else
//tt_error_file() << (int) tt_using_dispdib << " " << (int) paused << endl;
	if (exclusive_screen_desired()) {
		if (!paused) {
			displaydib_initialized = FALSE;
		} else {
#if DISPDIB_CLASS
//         ignore_next_killfocus++;
//         log("Incrementing ignore kill focus");
//         ignore_next_killfocus = TRUE;
			DisplayDibWindowEnd(dispdib_window_handle);
#elif DISPDIB
			int display_status = DisplayDib(NULL,NULL,DISPLAYDIB_END|DISPLAYDIB_NOWAIT);
			if (display_status > 0) {
				tt_error_file() << "Ending display status is "
								<< display_status << endl;
			};
#endif
			release_mouse_capture();
		};
	};
#endif // not directx
	if (!paused) {
		update_clock();
	};
	if (paused) { // now
		show_cursor(TRUE,TRUE);
		if (!shutting_down && !tt_shutting_down && ask) { // was just ask
//         toggling_pause = FALSE;  // so continue option below works -- commented out on 250602
#if TT_DEBUG_ON
		   if (tt_debug_mode == 130502) {
			   tt_error_file() << "in toggle_pause about to call ask_continue_or_quit at time " 
									 << timeGetTime() << " (frame=" << tt_frame_number << ")"  << endl;
		   };
#endif
//         release_exclusive_control(); // experiment on 250602
			ask_continue_or_quit(interrupted); 
//         toggling_pause = TRUE; commented out on 250602
//		} else {
//       	CloseWindow(tt_main_window->get_handle()); // so dialog always easy to see
      } else if (full_screen && !exclusive_screen_desired() && !tt_shutting_down) {
         RedrawWindow(NULL,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);  
			// repaint the entire desktop since might have drawn on it
#if TT_DEBUG_ON
		   add_to_log_if_interesting_error(0,"after RedrawWindow in toggle_pause");
#endif
      };
#if TT_ENHANCED_DRAG_AND_DROP
		release_drag_drop(tt_main_window->get_handle()); // new on 301204 since no longer available to receive drags
#endif
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
	} else {
//#if !TT_MICROSOFT
//		if (tt_debug_mode == 0)  // why was the following there? && replaying() == NO_REPLAY) {
//#endif
      if (!tt_show_mouse_cursor) {
         show_cursor(FALSE);
#if TT_DEBUG_ON
		   add_to_log_if_interesting_error(0,"after show_cursor in toggle_pause");
#endif
      };
		SetFocus(tt_main_window->get_handle()); 
#if TT_DEBUG_ON
		add_to_log_if_interesting_error(0,"after SetFocus in toggle_pause");
#endif
      if ((tt_mouse_acquired || (tt_time_travel != TIME_TRAVEL_OFF && tt_emulate_mouse_for_full_screen_time_travel)) 
			// new on 050305 to consider emulated mouse cursor
			// so when you return the emulated mouse cursor is centered
			&& !tt_direct_input_mouse_exclusive_ok	
#if TT_ABSOLUTE_MOUSE
			 && tt_mouse_mode == RELATIVE_MOUSE_MODE
#endif
			) {
//#if !TT_ENHANCED_DRAG_AND_DROP
			// if might be dragging then don't reset mouse position since need to "enter" window -- new on 020105
				SetCursorPos(client_center_x,client_center_y);
//#endif
		};
//      GetCursorPos(&center_mouse);
      frame_of_last_pause = tt_frame_number;
	};
	// commented out on 210303
  // if (paused && tt_system_mode != PUZZLE && !about_to_quit() && !shutting_down && ask && !interrupted && 
	// !interrupted added 180700
  //     tt_programmer->kind_of() == PROGRAMMER_AT_FLOOR && 
	// added on 090502 so when walking with something in pocket no Java is produced
  //     !is_kind_of(tt_programmer->pointer_to_appearance(),ROBOT_IN_TRAINING) &&
  //     !replaying_log) { // was tt_replay prior to 170702
		//// replaced tt_shutting_down with about_to_quit() on 150500 
	// so if clicked on dialog box to leave ToonTalk no Java applet is generated
  //    // only if on the floor???
  //    Sprite *in_hand = tt_programmer->pointer_to_tool_in_hand();
  //    if (in_hand != NULL) {
		//	dump_started();
  //       in_hand->java_start();
  //      	dump_ended(); //release_nest_indices(); // in case built up table to keep bird/nest corespondences
  //    };
  // };
	if (!paused && tt_using_direct_input) { // new on 030702
		acquire_input_devices();
	};
   toggling_pause = FALSE;
#if TT_DEBUG_ON
   if (tt_debug_mode == 130502) {
      tt_error_file() << "toggling_pause [2] is now " << (int) toggling_pause << endl;
   };
	add_to_log_if_interesting_error(0,"after toggle_pause");
#endif
};

flag window_multiple_warning_given = FALSE;

long all_mouse_buttons_reported_on_frame = 0;

boolean all_mouse_buttons() { // is really answering if left and right mouse buttons down
//   if (replaying()) { // really??
//      return(mouse_buttons&LEFT_BUTTON ||
//             mouse_buttons&MIDDLE_BUTTON ||
//             mouse_buttons&RIGHT_BUTTON);
//   } else {
      if (// (!replaying() && tt_mouse_button_count > 1 || tt_replay) &&
			 tt_2_mouse_buttons_is_escape && // replaced the above on 170301
          mouse_buttons&LEFT_BUTTON && mouse_buttons&RIGHT_BUTTON) {
         if (all_mouse_buttons_reported_on_frame != 0 && all_mouse_buttons_reported_on_frame != tt_frame_number) return(FALSE); // once is enough
         all_mouse_buttons_reported_on_frame = tt_frame_number;
         return(TRUE);
      } else {
         all_mouse_buttons_reported_on_frame = 0;
         return(FALSE);
      };
//   };
};

void escape(boolean from_keyboard) {
   if (replaying()) {
//		PostQuitMessage(0);
//		user_wants_to_quit = TRUE;
      pause_toontalk(); // give user a chance to change their mind
   } else if (tt_programmer->kind_of() != PROGRAMMER_AT_FLOOR &&
              tt_programmer->kind_of() != PROGRAMMER_TITLES) {
      toggle_pause();
   } else if (from_keyboard) {
      record_key((unsigned char) VK_ESCAPE);
   };
};

boolean extended_key_code(int code) { // new on 220804
	return((code >= VK_PRIOR && code <= VK_DOWN) || code == VK_INSERT || code == VK_DELETE);
};

void back_to_toontalk(boolean now) { // abstracted on 280403
	// now is new on 200105 to be used by dialogs so there is no waiting until the next cycle
	toggling_pause = FALSE;
#if TT_DEBUG_ON
   if (tt_debug_mode == 130502) {
      tt_error_file() << "toggling_pause is now " << (int) toggling_pause << " because of back to toontalk " << endl;
   };
#endif
   if (!now && exclusive_screen_desired()) {
      // new on 250602
      CloseWindow(tt_main_window->get_handle()); // to get rid of window -- 
      open_icon_on_next_frame = TRUE; // will open on next cycle since handling_loss_of_focus is still TRUE
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         tt_error_file() << "back to toontalk chosen. Closed window before unpausing to be sure to get back to full screen mode."
								 << endl;
      };
#endif
   } else {
      unpause_toontalk();
      tt_main_window->no_longer_handling_loss_of_focus();
   };
//                  OpenIcon(tt_main_window->get_handle()); // to go to full screen
//               SetActiveWindow(tt_main_window->get_handle()); // new on 250602
//               } else {
//               unpause_toontalk();
//               SetActiveWindow(tt_main_window->get_handle()); // new on 250602
//               OpenIcon(tt_main_window->get_handle()); // to go to full screen
//               };
};

Text *tt_IME_feedback_text_pad = NULL; // for now

LRESULT MainWindow::WndProc(UINT iMessage, WPARAM wParam, LPARAM lParam) {
#if TT_DEBUG_ON
	//if (tt_debug_mode == 100105) {
	//	debug_this();
	//};
	if (tt_debug_mode == 290803 || tt_debug_mode == 140105) { // || tt_debug_mode == 3) { // just for now 160702 and 100105
      tt_error_file() << "MainWindow receiving " << iMessage << " with parameters " << wParam << " and " << lParam 
							 << " at time " << timeGetTime() << " on frame " << tt_frame_number << endl;
   };
#endif
    if (already_shut_down && iMessage != WM_DESTROY) {
       return DefWindowProc( window_handle, iMessage, wParam, lParam );
    };
#if TT_DEBUG_ON
	 add_to_log_if_interesting_error(1008,"Start MainWindow::WndProc");
#endif
	 switch (iMessage) {
		  case WM_CREATE:
			  break;
		  case WM_PAINT:
			  if (tt_ok_to_process_windows_messages) { // condition new on 100105 -- don't paint otherwise
				  Paint();
			  };
			  break;
/*
		  case WM_LBUTTONDBLCLK:
			  // double click too easy to do accidentally while working
			  if (paused) toggle_pause();		  
			  break;
*/
		  case WM_LBUTTONDOWN:
			  if (tt_ok_to_process_windows_messages || !buffer_message(iMessage,wParam,lParam)) { // condition new on 100105
				  mouse_button_just_went_down(LEFT_BUTTON);
			  };
			  break;
		  case WM_RBUTTONDOWN:
			  if (tt_ok_to_process_windows_messages || !buffer_message(iMessage,wParam,lParam)) { // condition new on 100105
				  mouse_button_just_went_down(RIGHT_BUTTON);
			  };
			  break;
		  case WM_MBUTTONDOWN:
			  if (tt_ok_to_process_windows_messages || !buffer_message(iMessage,wParam,lParam)) { // condition new on 100105
				  mouse_button_just_went_down(MIDDLE_BUTTON);
			  };
			  break;			
		  case WM_LBUTTONUP:			  
		  case WM_RBUTTONUP:			  
		  case WM_MBUTTONUP:
			  if (tt_ok_to_process_windows_messages || !buffer_message(iMessage,wParam,lParam)) { // condition new on 100105
				  a_mouse_button_is_now_up();
			  };
			  break;
		  case WM_IME_NOTIFY:
			  // IF RECORDING A DMO FILE THIS SHOULD RECORD SOME MARKER!!!
			  // except it seems to work OK - you just don't see the tt_IME_feedback_text_pad
#if TT_DEBUG_ON
			  add_to_log_if_interesting_error(0,"receiving WM_IME_NOTIFY");
#endif
			  if (wParam == IMN_SETCONVERSIONMODE) {
				  play_sound(GLASS_HIGH_SOUND);
			  };
			  break;
		  case WM_IME_COMPOSITION:
#if TT_DEBUG_ON
			  if (tt_debug_mode == 240800) {
				  tt_error_file() << wParam << " with flags " << lParam << endl;
			  };
#endif
			  if (lParam&GCS_COMPSTR) { // display current composition string
				  HWND window_handle = tt_main_window->get_handle();
				  HIMC input_context = ImmGetContext(window_handle);
				  const long buffer_length = 4096;
				  char buffer[buffer_length];
				  LONG bytes_copied = ImmGetCompositionString(input_context,lParam&GCS_COMPSTR?GCS_COMPSTR:GCS_RESULTSTR,buffer,buffer_length);
#if TT_DEBUG_ON
				  if (tt_debug_mode == 240800) {
					  tt_error_file() << bytes_copied << " bytes: ";
					  tt_error_file().write(buffer,bytes_copied);
					  tt_error_file() << endl;
				  };
#endif
				  if (bytes_copied > 0) { 
					  const city_coordinate character_width = tile_width;
					  const city_coordinate character_height = tile_height*2;
					  if (tt_IME_feedback_text_pad == NULL) {
						  tt_IME_feedback_text_pad = new Text();
						  tt_IME_feedback_text_pad->set_use_variable_width_font(TRUE); // new on 030900 in case not running with a wide character set
						  tt_IME_feedback_text_pad->set_size(character_width,character_height); // restored on 160102
						  tt_IME_feedback_text_pad->set_held_status(HELD_BY_PROGRAMMER); // a lie so it doesn't get centered as more text is added - 170102
					  };
					  tt_IME_feedback_text_pad->set_text(buffer,bytes_copied);
//					  if (tt_IME_feedback_text_pad->current_height() < character_height) { // commented out on 170102
//						  tt_IME_feedback_text_pad->set_size(tt_IME_feedback_text_pad->current_width()*2,
//																		 tt_IME_feedback_text_pad->current_height()*2);
//					  };
					  tt_IME_feedback_text_pad->move_to(0,ideal_screen_height-tt_IME_feedback_text_pad->current_height()); 
					  if (tt_IME_feedback_text_pad->showing_on_a_screen()) {
						  tt_screen->invalidate_cache(); 
						  // new on 170102 since when more than one letter dirty region can get confused since this changes size and position of text
					  } else {
						  tt_screen->add(tt_IME_feedback_text_pad);
					  };
				  };
				  ImmReleaseContext(window_handle,input_context);
			  } else if (lParam&GCS_RESULTCLAUSE || // got the result
				          lParam == 0) { // or user has rubbed out entire string (new on 260800)
				  if (tt_IME_feedback_text_pad != NULL) { // get rid of text pad (will be re-created on need)
					  tt_screen->remove(tt_IME_feedback_text_pad);
					  tt_IME_feedback_text_pad->set_held_status(NOT_HELD); // remove lie
					  tt_IME_feedback_text_pad->destroy();
					  tt_IME_feedback_text_pad = NULL;
					  // could process buffer (when GCS_RESULTCLAUSE) but I let Windows send WM_IME_CHAR messages instead
#if TT_DEBUG_ON
					  add_to_log_if_interesting_error();
#endif
					  return DefWindowProc( window_handle, iMessage, wParam, lParam );
				  };	  
			  };
			  break;
		  case WM_IME_CHAR: // new on 220800 to handle wide character input (e.g. Japanese)
//			  tt_second_byte_of_double_byte_input = (char) (wParam >> 8);
			  if (replaying()) { // any character while running a demo
              toggle_pause();
				  break; // added 181000
			  } else {
				  record_key((unsigned char) wParam, FALSE, (unsigned char) (wParam >> 8));
			  };
		     add_key_to_buffer((unsigned char) wParam); // maybe this should send the "real" keystrokes - used by keyboard sensors
#if TT_DEBUG_ON
			  if (tt_debug_mode == 240800) {
				  tt_error_file() << wParam << " is " << (byte) (wParam >> 8) << " and "<< (byte) (wParam & 0xff) << endl;
			  };
#endif
			  break;
		  case WM_CHAR:
//           if ((lParam&max_unsigned_short) > 1) {  // autorepeat
//              break;
//           };
//			  if (wParam == VK_ESCAPE) {
//				  escape(TRUE);
//           } else
//			  if (iMessage == WM_CHAR) tt_second_byte_of_double_byte_input = 0;
			  if (tt_ok_to_process_windows_messages || !buffer_message(iMessage,wParam,lParam)) { // condition new on 100105
#if TT_ALPHA_FEATURE
				  if (tt_time_travel != TIME_TRAVEL_OFF) { // new on 031103 to act like Pause if time travelling and keyboard is used
						wParam = VK_PAUSE; // fall thru and pretend Pause was pressed
				  } else 
#endif
				  {
					  if (replaying()) { // && tt_titles_ended_on_frame <= tt_frame_number) { 
						  // any character while running a demo // tt_titles_ended_on_frame new on 230304
						  toggle_pause();
						  break; // added 181000
					  } else {
						  record_key((unsigned char) wParam);
					  };
					  add_key_to_buffer((unsigned char) wParam);
					  break;
			  				};
			  } else {
				  break;
			  };
		  case WM_KEYDOWN:
           // autorepeat handled before call to TranslateMessage
//			  if (lParam&(0x1L<<30)) { // autorepeat
//					break;
//			  };
			  // commented out the following on 181000 since it happens below as well
//			  if (lParam&(0x1L<<24)) { // extended character
//				  add_key_to_buffer(((unsigned char) wParam)+256);
//			  };
			  if (tt_ok_to_process_windows_messages || !buffer_message(iMessage,wParam,lParam)) { // condition new on 100105
			    switch(wParam) {
				  case VK_PAUSE:
					  if (tt_time_travel == TIME_TRAVEL_PAUSED) { 
						  // new on 200204 since don't want double Pause and return to start things up when watching a demo
						  toggle_pause();
					  } else if (tt_time_travel != TIME_TRAVEL_OFF) { // new on 210100
						  boolean was_in_playback = (tt_time_travel == TIME_TRAVEL_ON); // new on 211003
						  if (was_in_playback) { // new on 211003 since otherwise ends up in the old demo dialog
							  tt_time_travel_after_display_updated = TRUE;
						  } else {
							  time_travel(TIME_TRAVELLING_BEFORE_DIALOG); // was TIME_TRAVEL_OFF prior to 040803
							  toggle_pause(); // right thing if doing time travel playback?? -- no hence the changes of 211003
						  };
					  } else if (prepare_to_time_travel()) {
						  tt_time_travel_after_display_updated = TRUE;
//						  show_cursor(TRUE,TRUE); // new on 110103 - fixed elsewhere on 130103
					  } else { // no time travel log segments available
						  toggle_pause();
					  };
					  user_did(GLOBAL_HISTORY,USER_HAS_PAUSED);
					  break;
				  case VK_F8:
#if TT_DEBUG_ON
					  if (tt_debug_mode >= 1899 && tt_debug_mode <= 1902) {
						  say(S(IDS_WANT_TO_PLAY_NOW+(tt_debug_mode-1899)),99);
						  break;
					  };
#endif
					  if (replaying()) {
						  tt_exit_at_end_of_log = FALSE; // new on 301104
						  close_input_log(TRUE,FALSE);
						  if (!tt_subtitles_and_narration_even_without_demo) {
							  // otherwise want to keep SPK and update DMO
							  stop_sound(TRUE);
						  };
					  } else {
//					  // record it since F8 is also used to stop action
						  record_key((unsigned char) wParam,TRUE);
					  };
					  break;
				  case VK_F11:
					  if (tt_creating_narration_file || tt_debug_mode == 451) { 
						  // removed tt_replay || on 041199 since interferred with getting screen shots
						  // added tt_creating_narration_file on 070201 and 
						  // tt_debug_mode is there only for backwards compatibility (with old documentation)
						  new_speak(); // record time for narration
						  break;
					  };
                 if (GetAsyncKeyState(VK_CONTROL) < 0) { 
						  // was control_down() prior to 130602 but that interferes with recording demo replays 
                    // conditional new on 041199 - new policy
						  print_frame = TRUE; 
						  // and keep doing so until keyup -- moved up here on 130799 so works even when replaying		  
#if TT_DEBUG_ON
						  if (tt_debug_mode == 11099) {
							  tt_screen->screen_dirty();
							  break;
						  };
#endif
					  } else {
						  // record it since F11 is also used to drop while ignoring what's underneath
						  record_key((unsigned char) wParam,TRUE);
					  };
					  break;
              case VK_F12:
					  if (control_down()) { // new policy on 100200 that you need control-F12 to copy to clipboard
						  copy_screen_to_clipboard();
					  } else {
						  record_key((unsigned char) wParam,TRUE);
					  };
                 break;
/*
				  case VK_F12:
#if !TT_32
					  if (!tt_using_dispdib && display_dib_library == NULL) {
						  load_dispdib();
					  };
					  if (tt_using_dispdib) {
						  DisplayDib(NULL,NULL,DISPLAYDIB_END|DISPLAYDIB_NOWAIT);
						  displaydib_initialized = FALSE;
						  set_client_size(client_width,client_height);
//						  ShowCursor(tt_debug_mode != 0);
						  if (!windows_256_colors &&
								!told_user(TOLD_LEAVING_DISPDIB_SHOULD_BE_256_COLORS)) {
							  say(S(IDS_WINDOWS_NOT_256_BAD));
							  user_told(TOLD_LEAVING_DISPDIB_SHOULD_BE_256_COLORS);
						  };
					  } else { // set_client_size already did this
						  tt_screen->screen_dirty();
						  display_whole_next_frame = TRUE;
					  };
					  tt_using_dispdib = !tt_using_dispdib;
#else 
					  tt_screen->screen_dirty();
					  display_whole_next_frame = TRUE;
#endif
//					  toggle_pause();
//					  toggle_pause(); // easy way to reset things
//					  copy_screen_to_clipboard(); // color palette problems...
					  break;     
*/
				  case VK_F9:
					  if (replaying() || tt_debug_mode == 4510) {
                    // for recording the time for subtitles etc.
						  tt_error_file() << tt_current_time << endl;
						  break;
                 };
				  case VK_F1:
					  if (tt_time_travel != TIME_TRAVEL_OFF) { // new on 290903
//						  toggle_pause();
						  close_window();
						  launch_help("ttravel");
						  break;
					  };
				  default:
                 if (replaying()) { // any character while running a demo
//                    toggle_pause(); // handled elsewhere now
						  break; // added 181000
                 } else if (wParam == VK_ESCAPE) {
                    escape(TRUE);
			        } else {
                    if ((wParam >= VK_NUMPAD0 && wParam <= VK_F16) || // non-ascii special keys
//							   was (wParam <= VK_DELETE)
//							   lParam&(0x1L<<24)) { // extended bit
								extended_key_code(wParam)) { // rewritten on 220804 since number pad doesn't set the 24th bit
							  // and number pad enter and / are extended (but not to ToonTalk)
							  // commented out the conditional on 181000 since interferes with logging
//							   if (wParam != VK_LEFT && wParam != VK_RIGHT &&
//							       wParam != VK_UP && wParam != VK_DOWN) {
						         // skip arrow keys since used as mouse alternative
   						      record_key((unsigned char) wParam,TRUE);
						         add_key_to_buffer(((unsigned char) wParam)+256); // extended
//                        };
                        // handled by WM_CHAR now to get virtual-key translation to work
//                    } else {
//                        record_key((unsigned char) wParam);
//                        add_key_to_buffer((unsigned char) wParam);
					     };
                 };
					};
				 };
			  break;
        case WM_KEYUP:
			  if (tt_ok_to_process_windows_messages || !buffer_message(iMessage,wParam,lParam)) { // condition new on 100105
				switch(wParam) {
					case VK_F11:
						if (!ini_int("Switches","KeepSavingScreenShots",FALSE,0)) { // new on 170204
							print_frame = FALSE; // even if control is released first this stops recording frames
						};
						break;
				};
			  };
           break;
//        case WM_ACTIVATE:
//#if TT_DEBUG_ON
//           if (tt_debug_mode == 130502) {
//              tt_error_file() << "WM_ACTIVATE with " << wParam << " and " << lParam << endl;
//           };
//           break;
//#endif
		// consider dealing with WM_ACTIVATEAPP messages
//        case WM_ACTIVATE:
//#if TT_DEBUG_ON
//           if (tt_debug_mode == 1992) log("WM Activate");
//#endif
//           if (query_open) {// && LOWORD(wParam) != WA_INACTIVE && (BOOL) HIWORD(wParam)) {
//              unpause_toontalk(); // was minimized by pause
//           };
//           query_open = FALSE;
//           break;
		  case WM_SETFOCUS:
			  // should this use if (tt_ok_to_process_windows_messages || !buffer_message(iMessage,wParam,lParam)) { // condition new on 100105
#if TT_DEBUG_ON
           if (tt_debug_mode == 1992 || tt_debug_mode == 130502) log(_T("Wm SetFocus"));
#endif
//           if (waiting_for_user_dialog) {
//               unpause_toontalk();
//           } else
			  if (!tt_exclusive_screen) { // tt_exclusive_screen experiment on 040702
				  have_focus = TRUE;
              if (tt_using_direct_input) { // new on 150702 - at least needed while debugging
		           acquire_input_devices();
	           };
           } else if (tt_frame_number == 1 || (!have_focus && !paused && !return_handling_loss_of_focus())) {
              // special cased first frame since can happen if dialog box comes up at start
		        have_focus = TRUE;
#if TT_DEBUG_ON
              if (tt_debug_mode == 1992 || tt_debug_mode == 130502) log(_T("Getting focus now"));
#endif
//#if TT_DIRECTX
              focus_set(); // now
#if TT_DEBUG_ON
              if (tt_debug_mode == 1992 || tt_debug_mode == 130502) log(_T("Got focus now"));
#endif
				  if (initialization_completed) { // new on 311099
					  initialize_direct_draw(); 
#if TT_DEBUG_ON
                 if (tt_debug_mode == 130502) {
                    log("Initialize direct draw from WM_SETFOCUS.");
                 };
					  if (tt_debug_mode == 1992 || tt_debug_mode == 130502) log(_T("Direct Draw initialized"));
#endif
					  if (!tt_exclusive_screen) { // conditional new on 070401 since why try this if just initialized DirectX?
						  restore_surfaces();
					  };
					  resume_repeating_sound();
					  RedrawWindow(NULL,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);  
					  // repaint the entire desktop since might have drawn on it
				  };
			  };
			  if (initialization_completed) set_back_and_front_surface_palette(); // new on 050600 - conditional added on 230901
			  tt_log_initialization_start_time = timeGetTime(); // new on 310803
#if TT_DEBUG_ON
			  if (tt_debug_mode == 20903) {
				  tt_error_file() << "Just reset tt_log_initialization_start_time to " << tt_log_initialization_start_time << endl;
			  };
#endif
//#else      
//           if (!paused) {
//              focus_changed = TRUE; // deal with it later
//              got_focus_on_frame = tt_frame_number;
//              log("Have focus now");
//           };
//#endif
			  return(1); // experiment on 130702 was break;
		  case WM_KILLFOCUS:
			  // should this use if (tt_ok_to_process_windows_messages || !buffer_message(iMessage,wParam,lParam)) { // condition new on 100105
#if TT_DEBUG_ON
           if (tt_debug_mode == 1992 || tt_debug_mode == 130502) log(_T("Wm KillFocus"));
#endif
			  if (!tt_exclusive_screen) { // tt_exclusive_screen experiment on 040702
				  have_focus = FALSE;
           } else if (initialization_completed && !return_handling_loss_of_focus() && !restoring_exclusive_screen && !toggling_pause) {
              // conditions new on 160502) { // moved out here on 160502 to apply broadly
              if (have_focus) { // commented out on 040602 since checked above && !handling_loss_of_focus) {
                  handling_loss_of_focus = TRUE;
//		            have_focus = FALSE; -- moved below and conditionalize on 200502
                  boolean was_full_screen = tt_exclusive_screen;
#if TT_DEBUG_ON
                  if (tt_debug_mode == 1992 || tt_debug_mode == 130502) log(_T("Losing focus"));
#endif
                  focus_killed();
#if TT_DEBUG_ON
                  if (tt_debug_mode == 1992 || tt_debug_mode == 130502) log(_T("Lost focus"));
#endif
				      if (!was_full_screen) { // !was_full_screen since now (150502) focus_killed takes care of this
#if TT_DEBUG_ON
                     if (tt_debug_mode == 130502) {
                        log("release_direct_draw while WM_KILLFOCUS");
                     };
#endif
					      release_direct_draw(TRUE); // was FALSE prior to 160502 -- // new on 311099
                     tt_exclusive_screen = FALSE; // new on 140502
                     // moved here on 170502
                     handling_loss_of_focus = FALSE;
                     // commented out the following on 160502 since why do it if wasn't full screen?
//                     if (tt_exclusive_screen_desired) { // will this ever be true anymore??
//  //                    CloseWindow(window_handle); // new on 150502
//#if TT_DEBUG_ON
//					         if (tt_debug_mode == 1992 || tt_debug_mode == 130502) log(_T("restoring display mode"));
//#endif
//						      dderr(direct_draw->RestoreDisplayMode(),_T("restoring display mode"));
//					      }; // release direct draw even if not exclusive now
				      };
                  if (!tt_exclusive_screen) { //moved here and conditionalize on 200502
                     have_focus = FALSE; 
                  };
//                  handling_loss_of_focus = FALSE; // now set on next cycle if running full screen -- otherwise can cause dialogs to loop
#if TT_DEBUG_ON
                  if (tt_debug_mode == 130502) log(_T("Finished with WM_KILLFOCUS"));
#endif
              };
           };
//#else
//           if (tt_frame_number-got_focus_on_frame < 2) {
//              log("Ignoring killfocus.");
//           } else if (!paused) {
//		        have_focus = FALSE;
//              focus_changed = TRUE;
//              log("Lost focus");
//           };
//#endif
			  return(0); // experiment on 130702 was break;
		  case WM_SIZE:
//           if (window_handle != NULL) {
//              RECT front_surface_rectangle;
//              GetClientRect(window_handle,&front_surface_rectangle);
//              set_client_size(front_surface_rectangle.right-front_surface_rectangle.left,
//                              front_surface_rectangle.bottom-front_surface_rectangle.top);
//           }; // otherwise initialization and handle not ready
#if TT_DEBUG_ON
			  add_to_log_if_interesting_error(0,"WM_SIZE:");
#endif
           if (!return_handling_loss_of_focus() && !about_to_quit()) { // condition new on 160502 -- about_to_quit added 160702
              int return_value = set_client_size(LOWORD(lParam),HIWORD(lParam));
//#if TT_DEBUG_ON
//              if (tt_debug_mode == 19) { // for now on 160702
//                 tt_error_file() << "set_client_size returned " << return_value << endl;
//              };
//#endif
              return(return_value); // new on 130702 that this returns a value to indicate whether Windows should ignore this or not
#if TT_DEBUG_ON
           } else if (tt_debug_mode == 130502) {
              tt_error_file() << "Ignoring WM_SIZE of " << LOWORD(lParam) << "x" << HIWORD(lParam) << endl;
#endif
           };
//#if TT_DEBUG_ON
//           if (tt_debug_mode == 19) { // for now on 160702
//              tt_error_file() << "set_client_size not called " << endl;
//           };
//#endif
			  return(1); // was break; prior to 040602 but haven't really "handled" it here
		  case WM_MOVE: // experiment on 160502
           // commented out on 130502 since the following is done within set_client_size as well
			  //POINT client_center;
			  //client_center.x = client_width/2;
			  //client_center.y = client_height/2;
			  //ClientToScreen(window_handle,&client_center);
			  //client_center_x = client_center.x;
			  //client_center_y = client_center.y;
#if TT_DEBUG_ON
			  add_to_log_if_interesting_error(0,"WM_MOVE:");
#endif
			  if (!return_handling_loss_of_focus() && !about_to_quit()) { // condition new on 160502 -- about_to_quit added 160702
#if TT_DEBUG_ON
              if (tt_debug_mode == 130502) {
                 tt_error_file() << "WM_MOVE to call set_client_size " << client_width << "x" << client_height << endl;
              };
#endif
              set_client_size(client_width,client_height); // without this DirectX gets confused
#if TT_DEBUG_ON
           } else if (tt_debug_mode == 130502) {
              tt_error_file() << "Ignoring WM_MOVE while client dimensions are " << client_width << "x" << client_height << endl;
#endif
           };
           return(1); // was break; prior to 040602 but haven't really "handled" it here
		  case WM_QUERYOPEN:
           if (!waiting_for_user_dialog) {
              unpause_toontalk(); // was minimized by pause   // moved to wm_activate
				  if (//tt_current_log_segment < tt_youngest_log_segment &&  -- commented out on 130204 - not sure what the intent was but it 
					   // makes time travel confused when returning from double Pause clicks
					   tt_time_travel == TIME_TRAVELLING_BEFORE_DIALOG) { // new on 040803
					  time_travel(TIME_TRAVEL_JUST_PAUSED);
				  };
//         query_open = TRUE;
#if TT_DEBUG_ON
              if (tt_debug_mode == 1992 || tt_debug_mode == 130502) log(_T("Query Open"));
			     add_to_log_if_interesting_error(0,"Query Open");
#endif
           };
#if TT_DEBUG_ON
			  if (tt_debug_mode == 20903) {
				  tt_error_file() << "Just reset tt_log_initialization_start_time to " << tt_log_initialization_start_time 
										<< " due to WM_QUERYOPEN" << endl;
			  };
#endif
           return(1); // will prevent "send super" to OS
           // prior to 130702 this was break if waiting_for_user_dialog
		  case WM_GETMINMAXINFO: { // new on 050103 so that 2X windows that are "full screen" look better (no problems with task bar, etc)
			  // loosely based upon msdn magazine article (December 2002)
			  MINMAXINFO *min_max_info = (MINMAXINFO *) lParam;
			  int max_screen_width = GetSystemMetrics(SM_CXSCREEN)+20; // 20 extra pixels should be enough
			  int max_screen_height = GetSystemMetrics(SM_CYSCREEN)+100;
			  min_max_info->ptMaxSize.x = max_screen_width;
			  min_max_info->ptMaxSize.y = max_screen_height;
			  min_max_info->ptMaxTrackSize.x = max_screen_width; 
			  min_max_info->ptMaxTrackSize.y = max_screen_height;
			  return(1); // taken care of here
										 };
		  case WM_COMPACTING:
			  shrink_memory_cache((wParam*100L)/max_unsigned_short);
#if TT_DEBUG_ON
			  add_to_log_if_interesting_error();
#endif
			  return(0);
//#if !GRAPHICS_TO_MEMORY // commented out on 250602
//		  case WM_ERASEBKGND:
//			  break; // no op -- though might want to catch WM_SIZE messages
//#endif
		  case WM_DESTROY:
        case WM_CLOSE:
// following done after top-level loop finishes 
//#if GRAPHICS_TO_MEMORY
//			  if (tt_using_dispdib) { 
//				  DisplayDib(NULL,NULL,DISPLAYDIB_END);
//			  };
//#endif
			  if (!paused) toggle_pause(TRUE,TRUE,FALSE); // abnormal exit
			  PostQuitMessage(0);
			  break;
		  case WM_DROPFILES:  // rewritten 140204 to record here and process in the next case
			 if (tt_ok_to_process_windows_messages || !buffer_message(iMessage,wParam,lParam)) { // condition new on 100105
			  POINT point;
			  DragQueryPoint((HDROP) wParam,&point);
			  tt_drop_files_x = ideal_horizontal_units(point.x);
			  tt_drop_files_y = ideal_screen_height-ideal_vertical_units(point.y);
			  tt_drop_files_count = DragQueryFile((HDROP) wParam,0xFFFFFFFF,NULL,0);
			  if (tt_drop_files_count > 0) { // can it be otherwise?
				  tt_drop_file_names = new string[tt_drop_files_count];
				  tt_drop_file_hash_names = new string[tt_drop_files_count];
				  for (int i = tt_drop_files_count-1; i >= 0; i--) { // counted down before so keep the same order here
			  			tt_drop_file_names[i] = new char[MAX_PATH];
						DragQueryFile((HDROP) wParam,i,tt_drop_file_names[i],MAX_PATH);
						tt_drop_file_hash_names[i] = log_file_name(tt_drop_file_names[i]);
				  };
			  };		  
			  DragFinish((HDROP) wParam);
//			  SetFocus(get_handle()); // something kills focus just after this - I think...
			  if (tt_global_picture != NULL) { // new on 061199
				  tt_global_picture->do_after(1000,NULL,SET_FOCUS_CALLBACK);
			  };
			  if (tt_logging) break; 
			  // rather than fall through send message when recording special event so replay happens on the same frame
			 } else {
				 break; // new on 100105
			 };
		  case WM_USER+DROP_FILES_MESSAGE_ID: // either from above or from replaying a log - new on 140204
			  tt_programmer->add_item_from_outside(sprites_from_drop_globals(),tt_drop_files_x,tt_drop_files_y);
			  for (int i = 0; i < tt_drop_files_count; i++) { // clean up since done with these
				  delete [] tt_drop_file_names[i];
				  if (tt_drop_file_hash_names[i] != NULL) delete [] tt_drop_file_hash_names[i]; 
				  // conditional new on 200204 for robustness
			  };
			  delete [] tt_drop_file_names;
			  delete [] tt_drop_file_hash_names;
			  tt_drop_files_count = 0;
			  tt_drop_file_names = NULL; // just tidy
			  tt_drop_file_hash_names = NULL;
			  break;
		  case WM_USER+FILE_FOR_CLIPBOARD_MESSAGE_ID: { // new on 250300
#if TT_DEBUG_ON
			  add_to_log_if_interesting_error(0,"before WM_USER+FILE_FOR_CLIPBOARD_MESSAGE_ID:");
#endif
			  string file_name;
			  if (tt_file_name_clipboard_message != NULL) {
				  // new on 130204 -- is being replayed from a log
				  file_name = tt_file_name_clipboard_message;
			  } else {
				  file_name = ini_entry("User","TTFileLastDoubleClicked",FALSE); 
				  // added FALSE on 250703 since no need to record and confuses time travel
#if TT_DEBUG_ON
				  if (tt_debug_mode == 121205 && file_name != NULL) {
					  tt_error_file() << "Just read " << file_name << " from TTFileLastDoubleClicked " << endl;
				  };
#endif
				  if (paused) { 
					  // condition new on 121205 in case double clicked twice in a row and haven't finished dealing with first one
					  back_to_toontalk(TRUE);
				  };
			  };
			  if (file_name != NULL) {
				   string local_file_name = NULL; // new on 291204 since might be URL
//					loading_wait(); // new on 190105 -- commented out on 250105 since sprite_from_file_name calls this
					boolean aborted = FALSE; // new on 200105
					tt_add_sprite_when_on_floor = sprite_from_file_name(file_name,aborted,NULL,&local_file_name);
					if (tt_add_sprite_when_on_floor == NULL) { // condition was missing prior to 180903
						if (!aborted) {
							tt_error_file() << "You double clicked on the file " << file_name 
												 << " and yet ToonTalk couldn't find the file. If this was on a web page you might need to save the file on your local disk first." << endl;
							tt_err_file_important = TRUE;
						};
						delete [] file_name; // minor leak fixed on 130204
						if (tt_file_name_clipboard_message != NULL) {
							tt_file_name_clipboard_message = NULL;
						};
					} else {
						if (tt_file_name_clipboard_message != NULL) {
							// this was sent by replay so leave it alone
							delete [] tt_file_name_clipboard_message;
							tt_file_name_clipboard_message = NULL;
						} else {
							// new on 130204 so recorded as special event
							tt_file_name_clipboard_message = file_name;
						};
						if (tt_add_sprite_when_on_floor == tt_global_picture) { 
							// loaded a city or the like not an object -- new on 300505
							tt_add_sprite_when_on_floor = NULL;
						} else {
							// on 240304 the following was moved out of the second branch above
							// so demos that were started by double clicking work
							tt_add_sprite_when_on_floor_but_wait_a_frame = TRUE; 
							// new on 140204 to get the handling of this message and replay to be in synch
						};
					};
			  };
			  // commented out on 140204 so replay stays in synch
			  // note this used to bring the object while the time control buttons were still active - now it is when proceeding
			  //if (tt_programmer->kind_of() == PROGRAMMER_AT_FLOOR &&
					//tt_toolbox != NULL && 
					//tt_add_sprite_when_on_floor != NULL ) {
					//	// note that this means that while paused it is not pasted in
				 // tt_toolbox->add_sprite(tt_add_sprite_when_on_floor);
				 // tt_add_sprite_when_on_floor = NULL;
			  //};
																	 };
			  break;
//         case WM_WINDOWPOSCHANGING: // experiment on 160502        
//           {
//              LPWINDOWPOS window_position = (LPWINDOWPOS) lParam;
////              if (handling_loss_of_focus && window_position->cx == 0) {
////#if TT_DEBUG_ON
////                 if (tt_debug_mode == 130502) {
////                    tt_error_file() << "Was 0 and updated to screen dimensions." << endl;
////                 };
////#endif
////                 window_position->cx = tt_screen_width;
////                 window_position->cy = tt_screen_height;
////              };              
//#if TT_DEBUG_ON
//              if (tt_debug_mode == 130502) {
//                 tt_error_file() << "WM_WINDOWPOSCHANGING size is " << window_position->cx << " by " << window_position->cy << endl;
//              };
//#endif
//           };
//           break;
//#if TT_DEBUG_ON
//        case WM_WINDOWPOSCHANGED: // experiment on 160502
//           {
//              LPWINDOWPOS window_position = (LPWINDOWPOS) lParam;
//              if (tt_debug_mode == 130502) {
//                 tt_error_file() << "WM_WINDOWPOSCHANGED size is " << window_position->cx << " by " << window_position->cy << endl;
//              };
//           };
//           break;
//#endif
		  case WM_SYSCOMMAND:
			  switch (wParam) {
				  case SC_SCREENSAVE:
					  // don't give a screen saver permission to run
                 // if running a demo or full screen
					  if (replaying() || full_screen) {
#if TT_DEBUG_ON
						  add_to_log_if_interesting_error();
#endif
						  return(0);
					  };
				  case SC_KEYMENU:
#if TT_FUNCTION_KEY_HELP
					  if (lParam == 0) { // wasn't some Alt key to use keyboard shortcuts to menus 
						   // new on 280205 -- didn't used to think it worked because F10 is by default used for menu help
						   // but putting it here enables F10 to work
							if (tt_titles_ended_on_frame < tt_frame_number) {
								if (tt_function_key_help == NULL) {
									//Background *background = tt_programmer->pointer_to_background();
									//if (background != NULL) {
										boolean ok;
										tt_function_key_help = new UserPicture(S(IDS_FUNCTION_KEYS_HELP_PICTURE,"fkeys_en.jpg"),ok);
										if (ok) {
											update_function_key_help();
//											tt_screen->add(tt_function_key_help);
										//	background->add_item(tt_function_key_help);
										};
//									};								
								} else { // toggle appearance
//									tt_function_key_help->remove_from_floor(FALSE);
//									tt_screen->remove(tt_function_key_help);
									tt_function_key_help->destroy();
									tt_function_key_help = NULL;
									if (tt_toolbox != NULL) {
										tt_toolbox->set_clean_status(DIRTY,FALSE); // so if cached floor is reset
									};
								};
							};
					  };
					  // and do the following so not paused as a result
#endif
					  return(0); // new on 010503 so F10 and Alt don't pause ToonTalk
			  };
           // else fall thru
//#if TT_DEBUG_ON
//			  add_to_log_if_interesting_error(0,"before WM_WINDOWPOSCHANGING");
//#endif
//           if (tt_using_dispdib == 1) {
//              tt_error_file() << "Skipping WM_WINDOWPOSCHANGING" << " " << WM_WINDOWPOSCHANGING << endl;
//              return(0);
//           }; // else fall through
			  // else do the following too
		  default:
#if TT_DEBUG_ON
			  add_to_log_if_interesting_error(0,"before DefWindowProc");
#endif
			  return DefWindowProc( window_handle, iMessage, wParam, lParam );
	 };
#if TT_DEBUG_ON
	 add_to_log_if_interesting_error(0,"exiting MainWindow::WndProc");
#endif
	 return(0);
};

//HINSTANCE resource_library = NULL;

//void free_resource_source() {
//   FreeLibrary(resource_library);
//};

void MainWindow::mouse_button_just_went_down(int which_button) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 130800) { // || tt_debug_mode == 101) {
		tt_error_file() << "Mouse button#" << which_button << " just went down. Frame=" << tt_frame_number << endl;
	};
#endif
	if (tt_time_travel != TIME_TRAVEL_OFF && which_button == LEFT_BUTTON) {
      POINT mouse_position;
		GetCursorPos(&mouse_position); // in screen coordinates
#if TT_DEBUG_ON
		if (tt_debug_mode == 130800) {
			tt_error_file() << "Mouse x and y is " << mouse_position.x << "," << mouse_position.y << " before ScreenToClient" << endl;
		};
#endif
//#if !TT_POST323 -- bad idea - commented out on 051103 - don't transform if full screen is right (looked at several examples)
		// commented out condition on 041103 since the coordinates are transformed even when full screen
      if (!exclusive_screen_desired())  // tt_using_dispdib) { // if full screen no need to translate
//#endif
			ScreenToClient(window_handle,&mouse_position);
#if TT_DEBUG_ON
		if (tt_debug_mode == 130800) {
			tt_error_file() << "Mouse x and y is " << mouse_position.x << "," << mouse_position.y << " after ScreenToClient" << endl;
		};
#endif
//#endif
//				  if (window_width != tt_screen_width) { // need to take into account borders
//					  mouse_position.x -= GetSystemMetrics(SM_CXFRAME);
//				  };
//				  if (window_height != tt_screen_height) {
//					  mouse_position.y += GetSystemMetrics(SM_CYFRAME)+GetSystemMetrics(SM_CYCAPTION);
//				  };
		time_travel_react_to_mouse_down(tt_screen->x_pixel_to_screen(mouse_position.x),
												  tt_screen->y_pixel_to_screen(mouse_position.y));
	} else if (tt_mouse_acquired) { // conditional new on 311099
		tt_mouse_button = which_button;
		tt_dragging = FALSE; // new on 020105 -- needed?
		mouse_buttons |= which_button;
		if (tt_2_mouse_buttons_is_escape && all_mouse_buttons()) { // flag added on 160301
			escape(FALSE);
		};
//			tt_mouse_button_up_time = 0;
		mouse_down_handled = FALSE;
	}; 
};

void MainWindow::current_mouse_location(city_coordinate &x, city_coordinate &y) { // new on 230404
   POINT mouse_position;
	GetCursorPos(&mouse_position); // in screen coordinates
	if (!exclusive_screen_desired()) {
		ScreenToClient(window_handle,&mouse_position);
	};
	x = tt_screen->x_pixel_to_screen(mouse_position.x);
	y = tt_screen->y_pixel_to_screen(mouse_position.y);
};

void MainWindow::a_mouse_button_is_now_up() { // pull out as a procedure on 130800 so can be called by directinput
	// following was timeGetTime() but replaying log affected
	// adding half last delta a good estimate
	//	 tt_mouse_button_up_time = tt_current_time+tt_millisecond_delta/2;
#if TT_DEBUG_ON
	if (tt_debug_mode == 130800) { // || tt_debug_mode == 101) {
		tt_error_file() << "A mouse button just went up. Frame=" << tt_frame_number << endl;
	};
#endif
	if (tt_mouse_acquired) {
		button_unchanged = FALSE;
		if (mouse_down_handled) {
			tt_mouse_button = 0;
			mouse_buttons = 0;
			tt_mouse_up_simulated = FALSE;
			mouse_up_handled = TRUE;
			tt_dragging = FALSE; // new on 020105
		} else {
			mouse_up_handled = FALSE;
		};
	};
};

HGLOBAL MainWindow::get_resource(ascii_string resource_name, long &size) {
	HRSRC resource = FindResourceG(resource_source(),resource_name,(const char *) RT_RCDATA);
	HGLOBAL resource_handle = LoadResource(resource_source(),resource);
#if TT_DEBUG_ON
	if (resource == NULL) {
		message_box(MB_ICONSTOP,_T("Can't find resource "),a2u(resource_name));
	};
#endif
	size = SizeofResource(resource_source(),resource);
	return(resource_handle);
};

void MainWindow::no_longer_handling_loss_of_focus() {
//   handling_loss_of_focus_on_frame = -1;
   handling_loss_of_focus = FALSE;
   restoring_exclusive_screen = FALSE; // not clear if needed
};

void MainWindow::no_longer_restoring_exclusive_screen() {
//   handling_loss_of_focus_on_frame = -1;
   handling_loss_of_focus = FALSE;
   restoring_exclusive_screen = FALSE; // not clear if needed
};

//int tt_window_size_slop = 10; // percent -- could be user setable -- 
// was 10% prior to 070502 but that meant 1152x864 when device is 1280x1024 was wrong - thanks Leonel

long client_size_prior_to_full_screen_last_handled_on_frame = -1; // new on 050602 since Windows is sending size message that makes no sense

int MainWindow::set_client_size(int width, int height) {
//   if (tt_exclusive_screen) return; // DirectDraw can send a WM_SIZE message - ignore it
#if TT_DEBUG_ON
   if (tt_debug_mode == 130502) {
      tt_error_file() << "New window size is " << width << " by " << height <<  " and tt_got_exclusive_control_on_frame is " 
							 << tt_got_exclusive_control_on_frame << " and frame is " << tt_frame_number << endl;
   };
#endif
   if (width < 2 || height < 2) { // ignore it -- why not < 1?
#if TT_DEBUG_ON
		if (tt_debug_mode == 130502) {
			tt_error_file() << " Ignored since so small." << endl;
		};
#endif
      return(1); // let Windows deal with this
   };
//   if (tt_got_exclusive_control_on_frame == tt_frame_number && (width != tt_screen_width || height != tt_screen_height)) {
//#if TT_DEBUG_ON
//      if (tt_debug_mode == 130502) {
//         tt_error_file() << "Ignoring size change since going to full-screen." << endl;
//      };
//#endif
//      SetWindowPos(get_handle(),HWND_NOTOPMOST,0,0,tt_screen_width,tt_screen_height,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
//      return; // OK? - what about client area? - new on 060602
//   };
   if (tt_got_exclusive_control_on_frame < tt_frame_number && tt_exclusive_screen) {
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         tt_error_file() << "Ignoring size change since already went to full-screen." << endl;
      };
#endif
      return(1); // probably about to leave full-screen
   };
   if (tt_exclusive_screen_desired && !tt_exclusive_screen && !tt_restore_exclusive_screen) {
      if (client_size_prior_to_full_screen_last_handled_on_frame == tt_frame_number &&
          (width > tt_screen_width || height > tt_screen_height)) { // already took care of this
             // should be obsolete now that tt_getting_exclusive_control is around
#if TT_DEBUG_ON
         if (tt_debug_mode == 130502) {
            tt_error_file() << "Ignoring size as work around for getting WM_SIZE message twice - once OK and then too big (device size)." << endl;
         };
#endif
         SetWindowPos(get_handle(),HWND_TOP,0,0,tt_screen_width,tt_screen_height,SWP_NOMOVE); // SWP_NOACTIVATE|SWP_NOZORDER|
			// prior to 240105 HWND_TOP was HWND_TOPMOST
#if TT_DEBUG_ON
         if (tt_debug_mode == 130502) {
            tt_error_file() << "SetWindowPos [2] called with " << 0 << " " << 0 << " " << tt_screen_width << " " << tt_screen_height 
									 << " and SWP_NOMOVE." << " (frame=" << tt_frame_number << ")"  << endl;
         };
#endif
         return(0); // Windows should ignore this since new size set above (new on 130702)
      } else {
         client_size_prior_to_full_screen_last_handled_on_frame = tt_frame_number;
      };
   };
//   int tt_window_size_slop; // revisit this -- why do things this way? 
	// -- changed on 050602 so nearly full screen isn't special - only if truely maximized
   HDC visible_device_context = GetDC(window_handle);
   int border_width = 2*GetSystemMetrics(SM_CXFRAME);
	int border_height = 2*GetSystemMetrics(SM_CYFRAME)+GetSystemMetrics(SM_CYCAPTION);
   RECT maximum_windowed_work_area;
   SystemParametersInfo(SPI_GETWORKAREA,0,&maximum_windowed_work_area,0);
   int maximum_width = maximum_windowed_work_area.right-maximum_windowed_work_area.left-border_width;
   int maximum_height = maximum_windowed_work_area.bottom-maximum_windowed_work_area.top-border_height;
	if (width >= maximum_width && height >= maximum_height) {
		// new on 050103 to look better (no problem seeing task bar, etc)
		width = physical_screen_width;
		height = physical_screen_height;
	};
//   if (tt_exclusive_screen_desired) { // full screen - lots of slop (or beginning 130502 if running under control-f8)
//      // removed  || tt_restore_exclusive_screen on 140502
//      if (handling_transition_from_full_screen_this_frame() && (width > tt_screen_width || height > tt_screen_height)) { // new on 040602
//         coordinate new_width = min(width,tt_screen_width)+border_width;
//         coordinate new_height = min(height,tt_screen_height)+border_height;
//         if (new_width >= device_width) {
//            new_width /= 2;
//         };
//         if (new_height >= device_height) {
//            new_height /= 2;
//         };
//#if TT_DEBUG_ON
//         if (tt_debug_mode == 130502) {
//            tt_error_file() << "Resetting via SetWindowPos to " << new_width << "x" << new_height << endl;
//         };
//#endif 
//         SetWindowPos(get_handle(),HWND_NOTOPMOST,0,0,
//                      new_width,new_height,
//                      SWP_NOACTIVATE|SWP_NOMOVE);
//         // following broke since set too many other variables
////         resize_window_and_center(min(width,tt_screen_width),min(height,tt_screen_height),TRUE);
//         return;
//      };
//      tt_window_size_slop = 10;
//   } else {
//      tt_window_size_slop = 2;
//   };
   if (handling_transition_from_full_screen_this_frame() && (width >= maximum_width || height >= maximum_height)) { 
      // new on 050602 so good size is used when switching from full screen
      coordinate new_x,new_y,new_width,new_height;
      if (previous_window_width > 0) { // if doing this repeatedly use the previous dimensions
         new_x = previous_window_x;
         new_y = previous_window_y;
         new_width = previous_window_width;
         new_height = previous_window_height;
#if TT_DEBUG_ON
         if (tt_debug_mode == 130502) {
            tt_error_file() << "Using previous values." << endl;
         };
#endif
      } else {
         new_width = tt_screen_width;
         new_height = tt_screen_height;
#if TT_DEBUG_ON
         if (tt_debug_mode == 130502) {
            tt_error_file() << "Using tt_screen_width and tt_screen_height." << endl;
         };
#endif
         if (new_width == device_width) { // don't want full size window
            new_width /= 2;
            new_width += border_width; // not clear why needed but otherwise ends up too narrow by borders
            new_height /= 2;
            new_height += border_height;
#if TT_DEBUG_ON
            if (tt_debug_mode == 130502) {
               tt_error_file() << "Cut dimensions in half." << endl;
            };
#endif
         };
         new_x = (physical_screen_width-new_width)/2; // center
         new_y = (physical_screen_height-new_height)/2;
      };
      SetWindowPos(get_handle(),HWND_TOP,new_x,new_y,new_width,new_height,0); //SWP_NOACTIVATE);
		// prior to 240105 HWND_TOP was HWND_TOPMOST
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         tt_error_file() << "SetWindowPos [3] called with " << new_x << " " << new_y << " " << new_width << " " << new_height 
								 << " and no flags." << " (frame=" << tt_frame_number << ")" << endl;
      };
#endif
      return(0); // Windows should ignore this since new size set above (new on 130702)
   };
   if (tt_exclusive_screen && (width != tt_screen_width || height != tt_screen_height)) {
      // new on 240502 to force values to full screen
      width = tt_screen_width; // was device_width; 
      // updated on 040602 since might not yet have reset device so device_width is obsolete -- restored also on 040602
      height = tt_screen_height; // was device_height;
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         tt_error_file() << "Ignoring size. Reset to " << width << " by " << height << endl;
      };
#endif
      SetWindowPos(get_handle(),HWND_TOP,0,0,width,height,0); // SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE -- very important to set it to 0,0
		// prior to 240105 HWND_TOP was HWND_TOPMOST
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         tt_error_file() << "SetWindowPos [4] called with " << 0 << " " << 0 << " " << width << " " << height 
								 << " and no flags." << " (frame=" << tt_frame_number << ")"  << endl;
      };
#endif
      return(0); // Windows should ignore this since new size set above (new on 130702)
   };
	//int width_slop = border_width; // (tt_window_size_slop*device_width)/100;
	//int height_slop = border_height; // (tt_window_size_slop*device_height)/100;
//#if TT_DEBUG_ON
//   if (tt_debug_mode == 130502) {
//      tt_error_file() << "Width border is " << border_width << " while old slop is " << (tt_window_size_slop*device_width)/100 << endl;
//      tt_error_file() << "Height border is " << border_height << " while old slop is " << (tt_window_size_slop*device_height)/100 << endl;
//   };
//#endif
//   int better_width = width; // seemed to be "better" but broke full screen mode on laptop and tablet
//   int better_height = height;
   int full_screen_width = device_width;
   int full_screen_height = device_height;
	if (tt_mouse_acquired && !handling_transition_from_full_screen_this_frame()) { 
      // conditional added 311099 so that when mouse released still have minimize, etc.
      if (width > maximum_width && height > maximum_height) { // checking and updating both new on 060602
         int new_width, new_height;
         if (tt_restore_exclusive_screen) {
            new_width = tt_screen_width; // new on 060602
            full_screen_width = tt_screen_width;
         } else {
            new_width = device_width; 
         }; 
         if (tt_restore_exclusive_screen) {
            new_height = tt_screen_height; // new on 060602
            full_screen_height = tt_screen_height;
         } else {
            new_height = device_height; 
         };
#if TT_DEBUG_ON
         if (tt_debug_mode == 130502) {
            tt_error_file() << "Reset width " << width << " to " << new_width << endl;
            tt_error_file() << "Reset height " << height << " to " << new_height << endl;
         };
#endif
         width = new_width;
         height = new_height;
		};
	};
//  full_screen = (better_width == device_width && better_height == device_height);
//  full_screen = (width == device_width && height == device_height);
#if TT_DEBUG_ON
   if (tt_debug_mode == 130502) {
      tt_error_file() << "tt_mouse_acquired is " << (int) tt_mouse_acquired 
							 << " and width is " << width << " and height is " << height
							 << " and device_width is " << device_width << " and maximum_width is " << maximum_width 
							 << " and device_height is " << device_height << " and maximum_height is " << maximum_height << endl;
   };
#endif
   boolean need_to_toggle_full_screen = FALSE;
   // on 070602 changed the following from tt_screen_width to device_width etc.
   if (!toggling_pause && 
       ((width == full_screen_width && height == full_screen_height && !handling_transition_from_full_screen_this_frame()) ||
      // added !toggling_pause on 250602
       (tt_exclusive_screen_desired && !tt_exclusive_screen && !tt_restore_exclusive_screen))) { 
			 // new on 050602 since might be about to become full-screen
      // last condition added on 040602 
      // since shouldn't go full-screen if leaving it - not clear who is generating this WM_SIZE message
      // rewritten on 140502 so that if window is made full size then
      // we try to use full screen mode
      // prior to 100602 full_screen_width was device_width etc.
      full_screen = TRUE;
      // commented out the following on 050602 since the desirability of full screen is an option not a function of window size
      //if (tt_window_expansion_factor == 1) { // don't try to fix if window has been expanded
      //   tt_exclusive_screen_desired = TRUE;
      //};
      need_to_toggle_full_screen = (has_initialization_completed() && full_screen && tt_exclusive_screen_desired && !tt_exclusive_screen); 
		// new on 160502
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         tt_error_file() << "Set need_to_toggle_full_screen to " << (int) need_to_toggle_full_screen << endl;
         tt_error_file() << "full_screen is " << (int) full_screen << " and tt_exclusive_screen_desired is " 
								 << (int) tt_exclusive_screen_desired << " and tt_exclusive_screen is " << (int) tt_exclusive_screen << endl;
         tt_error_file() << "Started handling loss of focus on frame " << handling_transition_from_full_screen_on_frame << " and is now " 
								 << tt_frame_number << endl;
         tt_error_file() << "And toggling_pause is " << (int) toggling_pause << endl;
      };
#endif
   } else if (!tt_exclusive_screen) { // condition added 200502
      full_screen = FALSE;
   };
#if TT_DEBUG_ON
   if (tt_debug_mode == 130502) {
      tt_error_file() << "full_screen is " << (int) full_screen << " while tt_exclusive_screen_desired is " << (int) tt_exclusive_screen_desired
							 << " and tt_exclusive_screen is " << (int) tt_exclusive_screen << " and tt_restore_exclusive_screen is "
							 << (int) tt_restore_exclusive_screen << endl;
   };
#endif
   //if (full_screen) { // conditional assignment new on 070502 so only goes full screen if both dimensions "agree"
   //   width = better_width;
   //   height = better_height;
   //};
   //coordinate old_client_width = client_width; // only needed if need_to_toggle_full_screen
   //coordinate old_client_height = client_height;
   //coordinate old_client_x = client_center_x-client_width/2;
   //coordinate old_client_y = client_center_y-client_height/2;
   if (full_screen && tt_exclusive_screen_desired) {
      // new on 060602
      client_width = tt_screen_width;
      client_height = tt_screen_height;
   } else {
	   client_width = width;
	   client_height = height;
   };
	POINT client_center;
	client_center.x = client_width/2;
	client_center.y = client_height/2;
   if (!full_screen) {
      if (!IsIconic(get_handle()) && !ClientToScreen(window_handle,&client_center)) { // ClientToScreen if successful maps to screen coordinates 
         // doesn't seem to fail even if minimized -- added !IsIconic on 040602
         // restore things
         client_center.x = client_width/2;
	      client_center.y = client_height/2;
      };
   };
   if (client_center.x > 0 && client_center.y > 0 && client_center.x < physical_screen_width && client_center.y < physical_screen_height) { 
      // first 2 conditions new on 130502 since if window is in process of minimizing then this can get confused
      // and second 2 new on 040602
      // is this an adequate safe guard against this? -- why didn't ClientToScreen fail rather than screw up??
	   client_center_x = client_center.x;
	   client_center_y = client_center.y;
   } else {
      client_center_x = physical_screen_width/2;
	   client_center_y = physical_screen_height/2;
   };
#if TT_DEBUG_ON
   if (tt_debug_mode == 170502) {
      tt_error_file() << "New window center is " << client_center_x << "," << client_center_y << endl;
   };
#endif
#if TT_DIRECTX
//   if (clip_region_data_memory == NULL) {
//      clip_region_data_memory = new unsigned char[sizeof(RGNDATAHEADER)+sizeof(RECT)];
//      clip_region_data_header = (RGNDATAHEADER *) clip_region_data_memory;
//      clip_region_data = (LPRGNDATA) clip_region_data_memory;

//      front_surface_rectangle = &clip_structure.clip_rectangle;
//      front_surface_rectangle
//          = (LPRECT) clip_region_data_memory+sizeof(RGNDATAHEADER);
//   };
   front_surface_rectangle.left = max(0,client_center_x-client_width/2);
   front_surface_rectangle.top = max(0,client_center_y-client_height/2);
   front_surface_rectangle.right = min(device_width,client_center_x+client_width/2);
   front_surface_rectangle.bottom = min(device_height,client_center_y+client_height/2);
#if TT_DEBUG_ON
   if (tt_debug_mode == 130502) {
      tt_error_file() << "Front surface rectangle is " << front_surface_rectangle.left << " " << front_surface_rectangle.top 
							 << " " << front_surface_rectangle.right << " " << front_surface_rectangle.bottom << endl;
      tt_error_file() << "Where client_width is " << client_width << " and client_height is " << client_height
							 << " and device_width is " << device_width << " and device_height is " << device_height << endl;
   };
#endif
/*
   LPRECT clip_rectangle
      = (LPRECT) (clip_region_data_memory+sizeof(RGNDATAHEADER));
   clip_rectangle->left = front_surface_rectangle.left;
   clip_rectangle->top = front_surface_rectangle.top;
   clip_rectangle->right = front_surface_rectangle.right;
   clip_rectangle->bottom = front_surface_rectangle.bottom;
*/
  // if (clipper != NULL) { // not too early in initialization
      // should I remove old one first?
  //    HRESULT result = clipper->SetClipList(clip_region_data,NULL);
  //    dderr(result,"Clipping to the main window.");
  // };
#endif
   if (need_to_toggle_full_screen) { // new on 160502
      toggle_full_screen();
   };
	if (tt_screen != NULL) tt_screen->screen_dirty();
//	display_whole_next_frame = TRUE;
#if !TT_DEBUG_ON
   // commented out the following while debugging (beginning 040602)
	if (tt_exclusive_screen_desired || paused || client_height < 30 || tt_restore_exclusive_screen) { // print nothing
	} else
#endif
   if (client_width%tt_screen_width != 0 || client_height%tt_screen_height != 0) {
#if TT_DEBUG_ON
		tt_error_file() << "Warning window is not a multiple of " << tt_screen_width << " and " << tt_screen_height 
							 << " but is " << client_width << " by " << client_height << "." << endl;
		tt_error_file() << "So it may run slow." << endl;
		window_multiple_warning_given = TRUE;
#endif
	} else if (window_multiple_warning_given) {
#if TT_DEBUG_ON
		tt_error_file() << "Window multiple ok now." << endl;
#endif
		window_multiple_warning_given = FALSE;
	};
   if (have_focus && full_screen) {
      capture_mouse(); // grab the mouse anywhere
//      SetActiveWindow(window_handle);
#if TT_DEBUG_ON
      if (tt_debug_mode == 1992) tt_error_file() << "Mouse captured." << endl;
#endif
   };
   if (!tt_exclusive_screen && !IsIconic(get_handle())) { // conditional new on 040602
      // !tt_exclusive_screen added 060602
      RECT window_rectangle;
      GetWindowRect(get_handle(),&window_rectangle); // new on 200502
      if (window_rectangle.right-window_rectangle.left < device_width && tt_frame_number > 1) { 
			// added tt_frame_number > 1 on 050702 so don't record these values while initializing
         // ignore transitions where window covers the screen
         // save old coordinates so if repeatedly go from window to full-screen that you only need to move things once
         previous_window_x = window_rectangle.left;
         previous_window_y = window_rectangle.top;
         previous_window_width = window_rectangle.right-window_rectangle.left;
         previous_window_height = window_rectangle.bottom-window_rectangle.top;
   #if TT_DEBUG_ON
         if (tt_debug_mode == 130502) {
            tt_error_file() << "previous_window_x is " << previous_window_x << " previous_window_y is " << previous_window_y
                        << " previous_window_width is " << previous_window_width << " previous_window_height " << previous_window_height << endl;
         };
   #endif
      };
   };
   ReleaseDC(window_handle,visible_device_context);
#if TT_DEBUG_ON
   if (tt_debug_mode == 130502) {
      log("Finished with :set_client_size");
   };
#endif
   return(1);
};

LRESULT export_declaration WndProc(HWND window_handle, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	 // Pointer to the (C++ object that is the) window.
    if (premature_exit) return DefWindowProcG( window_handle, iMessage, wParam, lParam );
	 Window *pWindow = NULL;
    if (!already_shut_down) pWindow = GetPointer( window_handle );

	 // The pointer pWindow will have an invalid value if the WM_CREATE
	 // message has not yet been processed (we respond to the WM_CREATE
	 // message by setting the extra bytes to be a pointer to the
	 // (C++) object corresponding to the Window identified
	 // by window_handle).  The messages that
	 // precede WM_CREATE must be processed without using pWindow so we
	 // pass them to DefWindowProc.
	 // How do we know in general if the pointer pWindow is invalid?
	 // Simple: Windows allocates the window extra bytes using LocalAlloc
	 // which zero initializes memory; thus, pWindow will have a value of
	 // zero before we set the window extra bytes to the 'this' pointer.
	 // Caveat emptor: the fact that LocalAlloc will zero initialize the
	 // window extra bytes is not documented; therefore, it could change
	 // in the future.

	 if ( pWindow == NULL) {
		  if ( iMessage == WM_CREATE ) {
				LPCREATESTRUCT lpcs;

				lpcs = (LPCREATESTRUCT) lParam;
				pWindow = (Window *) lpcs->lpCreateParams;

				// Store a pointer to this object in the window's extra bytes;
				// this will enable to access this object (and its member
				// functions) in WndProc where we are
				// given only a handle to identify the window.
				SetPointer( window_handle, pWindow );
				// Now let the object perform whatever
				// initialization it needs for WM_CREATE in its own
				// WndProc.
				return pWindow->WndProc( iMessage, wParam, lParam );
		  } else {
				return DefWindowProcG( window_handle, iMessage, wParam, lParam );
        };
	 } else {
		  return pWindow->WndProc( iMessage, wParam, lParam );
    };
};

int default_setting(const_ascii_string id) {
   int answer = ini_int(AC(IDC_DEFAULTS),id,FALSE,0);
   if (answer != 0) return(answer);
   return(ini_int(AC(IDC_INSTALLATION_DEFAULTS),id,FALSE,0));
};

void update_defaults() {
   int version_answer = default_setting("Version");
	if (version_answer == 0) { // new on 230300 - nothing in toontalk.ini file
		version_answer = 2;
	};
//#if TT_SMALL_TRIAL
//	tt_video_mode = 320;
//#else
   tt_video_mode = video_mode_number(version_answer-1);
//#endif
	if (version_answer > 2) {
		tt_good_sizes_adjust_to_screen_size = TRUE; // since below 640x480 no point in doing this
	};
/* new version as of 230999
   switch (version_answer) { // consider revising -f so it works this way too
      case 1: // 16-bit
         tt_using_dispdib = 1;
         break;
      case 2: // 32-bit -v 22
         tt_video_mode = 320;
         break;
      case 3:
#if TT_32
         tt_video_mode = 640;
#else
         tt_video_mode = 320;
#endif
         break;
   };
	*/
   int window_size_answer = default_setting("WindowSize");
   switch (window_size_answer) { // consider revising -f so it works this way too
      case 1: // full screen
		case 0: // if nothing in toontalk.ini file - new on 230300
//         tt_using_dispdib = 1;
         tt_exclusive_screen_desired = TRUE;
         break;
      case 2: // tiny
//         tt_using_dispdib = 0;
         tt_exclusive_screen_desired = FALSE;
         break;
      case 3:
      case 4:
      case 5:
         tt_window_expansion_factor = (unsigned char) (window_size_answer-1);
         tt_exclusive_screen_desired = FALSE;
         break;
   };
   int button_use_answer = default_setting("MouseButtons");
   if (button_use_answer > 0) {
      tt_mouse_button_count = (char) button_use_answer;
   };
#if !TT_SMALL_TRIAL
   int marty_talk_answer = default_setting("MartyTalk");
   switch (marty_talk_answer) {
		case 0: // if nothing in toontalk.ini file - new on 230300
      case 1:
         tt_marty_talk = MARTY_SPEAKS_AND_TALK_BALLOONS;
         break;
      case 2:
         tt_marty_talk = MARTY_SPEAKS;
         break;
      case 3:  // kid speed
         tt_help_speed_percent = 100;
         tt_marty_talk = MARTY_TALK_BALLOONS;
         break;
      case 4:  // adult speed
         tt_help_speed_percent = 175;
         tt_marty_talk = MARTY_TALK_BALLOONS;
         break;
      case 5:  // wait for clicks
         tt_help_speed_percent = 0;
         tt_marty_talk = MARTY_TALK_BALLOONS;
         break;
		case 6: // new on 270999
			tt_marty_talk = MARTY_SPEAKS_WITH_SUBTITLES;
			tt_closed_caption = TRUE;
			break;
		case 7: // new on 110401
			tt_marty_talk = MARTY_WITH_SUBTITLES;
			tt_closed_caption = TRUE;
			break;
   };
   tt_marty_talk_to_save = tt_marty_talk;
#endif
#if !TT_32
   tt_marty_talk = MARTY_TALK_BALLOONS;  // regardless
#endif;
   int sound_answer = default_setting("SoundOn");
   if (sound_answer == 2) {
		// tt_sound_option = 0; // turned off
		turn_off_sound(TRUE);
	};
   switch (default_setting("KindOfUser")) {
      case 1:   // "pro"
         tt_trouble_shooting = TRUE;
         break;
      case 2:  // "novice"
		case 0: // no information in toontalk.ini file - new on 230300
         tt_trouble_shooting = FALSE;
         break;
      case 3: // "kiosk or museum mode"
         tt_trouble_shooting = FALSE;
         tt_autodemo = REPEAT_INTRO; 
         break;
   };
   // language handled later
	// following is new on 071199
	string variable_font_name = ini_entry("User","VariableFont");
	if (variable_font_name != NULL) {
		tt_variable_font_name = variable_font_name;
		replace_character(tt_variable_font_name,'_',' '); // replace underbars with spaces
	};
	string fixed_font_name = ini_entry("User","FixedFont");
	if (fixed_font_name != NULL) {
		tt_fixed_font_name = fixed_font_name;
		replace_character(tt_fixed_font_name,'_',' '); // replace underbars with spaces
	};
#if !TT_SMALL_TRIAL
	if (tt_marty_talk == MARTY_SPEAKS_AND_TALK_BALLOONS || tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES) { 
		// conditional new on 110401 so this is ignored if speech isn't active
		string spoken_language = ini_entry("User","SpokenLanguage");
		if (spoken_language != NULL) {
			tt_alternative_spoken_language = string_to_language(spoken_language,TRUE);
			if (tt_alternative_spoken_language == tt_language) { // closed caption
				// was unconditional prior to 170300 but "else" case happens latter now
				set_spoken_language(tt_alternative_spoken_language); 
			};
			delete [] spoken_language;
		};
	};
#endif
	tt_keep_center_when_size_changes = !ini_int(AC(IDC_SWITCHES),"OldBehaviorWhenChangingSize"); 
	// new on 280100 -- no new demos should use this switch
	tt_create_subnotebooks_of_remote_notebook = ini_int(AC(IDC_SWITCHES),"CreateRemoteControlNotebooksForParts",FALSE,
																		 tt_create_subnotebooks_of_remote_notebook); // new on 211200
	tt_ask_if_ok_to_load_dll = ini_int(AC(IDC_SWITCHES),"AskBeforeForeignBirdsLoadDLL",FALSE,tt_ask_if_ok_to_load_dll); // new on 280201
	// should the following be recorded in the log since it can change replay, right? asked 020304
	tt_movement_selection_feedback = ini_int(AC(IDC_SWITCHES),"MovementSelectionFeedback",FALSE,tt_movement_selection_feedback); // new on 200901
	tt_color_selection_feedback = ini_int(AC(IDC_SWITCHES),"ColorSelectionFeedback",FALSE,tt_color_selection_feedback); // new on 200901
	tt_dither_to_8bits = ini_int(AC(IDC_SWITCHES),"DitherTo8BitColor",FALSE,tt_dither_to_8bits); // new on 201101
   set_absolute_mouse_mode(ini_int(AC(IDC_SWITCHES),"AbsoluteMouseMode",FALSE,0)); // new on 250402 -- 
	// above should really end up recorded in DMO files -- noted on 050605
   tt_show_mouse_cursor = ini_int(AC(IDC_SWITCHES),"ShowMouseCursor",FALSE,tt_show_mouse_cursor); // new on 250402
   tt_directional_pad_center_x = ini_int(AC(IDC_SWITCHES),"DirectionalPadCenterX",FALSE,tt_directional_pad_center_x); // new on 250402
   tt_directional_pad_center_y = ini_int(AC(IDC_SWITCHES),"DirectionalPadCenterY",FALSE,tt_directional_pad_center_y); // new on 250402
   tt_directional_pad_center_radius = ini_int(AC(IDC_SWITCHES),"DirectionalPadRadius",FALSE,tt_directional_pad_center_radius); // new on 250402
//   tt_allow_multiple_toontalks = ini_int(AC(IDC_SWITCHES),"AllowMultipleToonTalks",FALSE,tt_allow_multiple_toontalks); 
	// new on 250402 -- commented out on 150404 (wasn't documented anyway)
   // following new on 291002
	if (!tt_trouble_shooting) {
		tt_want_exception_breaks = FALSE; // reset the default -- new on 040903 so that naive users don't get to send crash reports
	};
   tt_want_exception_breaks = !ini_int("Switches","TryToRecoverFromCrashes",FALSE,!tt_want_exception_breaks);
   if (tt_want_exception_breaks) {
		if (windows_2000_or_above()) { // new on 090603 since can't generate mini dump in Win 98 (or Me?)
			tt_ok_to_generate_mini_dump_message = ini_entry("User","MessageAskingToReportCrash",FALSE);
			if (tt_ok_to_generate_mini_dump_message == NULL) {
				tt_ok_to_generate_mini_dump_message = copy_string("I'm sorry but ToonTalk crashed. Will you please help us fix the problem?");
			};
			tt_mini_dump_email = ini_entry("User","EmailToReportCrash",FALSE);
			if (tt_mini_dump_email == NULL) {
				// updated on 181202 to include version information.
				tt_mini_dump_email 
					= append_3_strings("mailto:support@toontalk.com?subject=",C(IDS_TOONTALK_VERSION_NUMBER),
											" Crash Report &body=Please add any information that might be useful. And please drag the crash dump file here. ");
			};
			tt_show_mini_dump_directory = ini_int("Switches","ShowCrashDirectoryAfterCrashes",FALSE,tt_show_mini_dump_directory);
			tt_full_memory_dump = ini_int("Switches","OKToCreateVeryLargeCrashReports",FALSE,tt_full_memory_dump);
		} else {
			tt_ok_to_generate_mini_dump_message = NULL;
		};
   };
};

boolean has_main_dir_entry(string ini_file) { // new on 170105 to ignore toontalk.ini files that don't even set MainDir
	if (tt_local_ini_file != NULL && ini_file != tt_local_ini_file && has_main_dir_entry(tt_local_ini_file)) {
		return(TRUE);
	};
	if (ini_file == NULL) return(FALSE);
	if (!file_exists(ini_file,FALSE)) return(FALSE);
	char entry[MAX_PATH];
	if (GetPrivateProfileStringG("Directories","MainDir","",entry,MAX_PATH,ini_file) > 0) {
		return(TRUE);
	};
	if (GetPrivateProfileStringG("Executables","UseThisIniFileFirst","",entry,MAX_PATH,ini_file) > 0) {
		return(has_main_dir_entry(entry));
	};
	return(FALSE);
};

boolean win_main_initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, ascii_string CmdLine, int nCmdShow) {
// was TT_DEBUG_ON prior to 041003
//	 tt_err_file_name = copy_string("c:\\pre_init.txt"); // new on 041003
	 BeginOLE(); // moved here on 260604 since used by both HTML dialogs and text-to-speech
	 tt_err_file_name = new char[MAX_PATH];
	 GetTempPath(MAX_PATH,tt_err_file_name); // new on 041003
	 strcat(tt_err_file_name,"ToonTalk_pre_init.txt"); 
	 tt_err_file.open(tt_err_file_name,std::ios_base::out|std::ios_base::trunc); 
	 // added trunc on 260304 to delete current if it exists // new on 130300
	 if (tt_err_file == NULL) { // new on 290904 for error recovery
		 unable_to_write_file(tt_err_file_name); 
	 };
#if TT_DEBUG_ON
    add_to_log_if_interesting_error(0,"win_main_initialize");
#endif
	 Main::hInstance = hInstance;
	 tt_main_instance = hInstance; // new on 260603 for DynaZip - global variable existed but wasn't used
	 wait_cursor();
	 Main::hPrevInstance = hPrevInstance;
	 Main::nCmdShow = nCmdShow;
	 another_toontalk_is_running = (FindWindow("ToonTalk",NULL) != NULL);
	 HWND other_window = FindWindowG(szClassName,window_title); 
    // beginning 250402 any ToonTalk is fine unless tt_allow_multiple_toontalks
	 // moved here on 150404 since where it was previously was too late on Win98/Me where the string library is loaded and fails to load here
	 // too early to discover self as of 150404
	 if (CmdLine != NULL && strstr(CmdLine,"-allow_multiple_toontalks 1") != NULL) { // moved here and updated on 260904
		  tt_allow_multiple_toontalks = TRUE;
	 };
	 if ((other_window != NULL || another_toontalk_is_running) 
		 // && other_window != tt_main_window->get_handle()) || // beginning 150204 check that haven't just discovered itself
		  && !tt_allow_multiple_toontalks) { 
    // updated conditional on 260904
	 //if ((other_window != NULL) || 
			  // && other_window != tt_main_window->get_handle()) || // beginning 150204 check that haven't just discovered itself
		//  (another_toontalk_is_running && !tt_allow_multiple_toontalks)) { // already one with the same user name so go to it
		 show_cursor(TRUE,TRUE); // just in case
       if (other_window == NULL) {
          other_window = FindWindowG(szClassName,NULL); // ignore the title
       };
       OpenIcon(other_window); // SetForegroundWindow
       premature_exit = TRUE;
		 return(TRUE);
	 };
//    tt_original_command_line = lpszCmdLine; -- obsolete
	 // A Windows class should be registered with Windows before any windows
	 // of that type are created.
	 // Register here all Windows classes that will be used in the program.
	 // Windows classes should not be registered if an instance of
	 // the program is already running.
	 if (Main::hPrevInstance == NULL) {
		 MainWindow::Register();
	 };
//#if !TT_NEW_IO
//    tt_temporary_buffer = new unsigned char[tt_temporary_buffer_size]; // used by pre_tt_err_file
//#endif
	 tt_command_line_for_exe_to_run_upon_shutdown = copy_string(CmdLine); // new on 021103
	 allocate_resource_strings(); // early since used by static strings as well
    ascii_string commands[32]; // for now only 32 args in command line
    int command_count;
    NaturalLanguage language_to_set = process_command_line(CmdLine,command_count,commands);
    // following new on 290302 to match the Windows XP compatibility standards (and seems a good idea anyway)
    char temp_path[MAX_PATH];
//    if (SHGetFolderPath(NULL,CSIDL_COMMON_DOCUMENTS|CSIDL_FLAG_CREATE,NULL,SHGFP_TYPE_CURRENT,common_documents) != S_OK) {
    // following is older version that is available on nearly all systems (for 95 and NT 4 requires IE 4.0)
//    if (!SHGetSpecialFolderPath(NULL,temp_path,CSIDL_PERSONAL,TRUE)) {
	 get_my_documents_folder(temp_path); // updated 050504
//    if (!get_special_folder(CSIDL_PERSONAL,temp_path)) { // rewritten on 260402
//#if TT_DEBUG_ON
//	    tt_error_file() << "Failed to find CSIDL_PERSONAL." << endl;
//#endif
//    };
	 if (tt_ini_file == NULL || tt_ini_file[0] == '\0') { // condition new on 070503
		 set_ini_file(append_3_strings(temp_path,"\\ToonTalk\\",AC(IDC_TT_INI)));
	 };
//    if (!file_exists(tt_ini_file,FALSE)) {
//       // no personal toontalk.ini file exists so use shared common one
//       delete [] tt_ini_file;
//       if (!SHGetSpecialFolderPath(NULL,temp_path,CSIDL_COMMON_DOCUMENTS,TRUE)) {
//#if TT_DEBUG_ON
//	      tt_error_file() << "Failed to find CSIDL_COMMON_DOCUMENTS." << endl;
//#endif
//       };
//       tt_shared_files_directory = append_strings(temp_path,"\\ToonTalk\\"); // new on 030402
//       tt_ini_file = append_3_strings(temp_path,"\\ToonTalk\\",AC(IDC_TT_INI));
    if (!has_main_dir_entry(tt_ini_file)) {
       // create it and initialize it
       if (get_special_folder(CSIDL_WINDOWS,temp_path)) {
          strcat(temp_path,AC(IDC_TT_INI));
          if (has_main_dir_entry(temp_path)) {
				 if (!CopyFile(temp_path,tt_ini_file,TRUE)) {
					 unable_to_write_file(tt_ini_file);
				 };
			 } else if (::GetModuleFileName(NULL,temp_path,MAX_PATH)) {
				 // new on 141204 to get ToonTalk to run off a CD-ROM -- do this in StartTT as well
				 int directory_end = short_file_name_start(temp_path);
				 if (directory_end > 0) {
					 strcpy(temp_path+directory_end,AC(IDC_TT_INI));
					 if (has_main_dir_entry(temp_path)) {
						 if (!CopyFile(temp_path,tt_ini_file,TRUE)) {
//							 unable_to_write_file(tt_ini_file); // ask user? -- 141204
							 strcpy(tt_ini_file,temp_path); // just use the ini file that is on the CD
						 } else { // copied OK
							 temp_path[directory_end] = '\0'; // terminate directory
							 write_ini_entry("Directories","MainDir",temp_path);
							 strcpy(temp_path+directory_end,"Temp");
							 write_ini_entry("Directories","TempDir",temp_path);
							 strcpy(temp_path+directory_end,"Java");
							 write_ini_entry("Directories","BuiltinPictureDir",temp_path);
							 write_ini_entry("Directories","FileSearchPath",temp_path);
						 };
					 };
				 };
			 };
       } else {
#if TT_DEBUG_ON
	       tt_error_file() << "Failed to find CSIDL_WINDOWS." << endl;
#endif
		 };
		 if (!has_main_dir_entry(tt_ini_file)) {
//#if TT_DEBUG_ON
	       tt_error_file() << "Failed to find toontalk.ini on Windows directory or directory where the ToonTalk executable exists." << endl;
//#endif
          delete [] tt_ini_file;
          set_ini_file(copy_string(AC(IDC_TT_INI))); // use default search path
       };
    };
#if TT_DEBUG_ON
    add_to_log_if_interesting_error(0,"before update_defaults");
#endif
    // and the following 2 lines moved after the above so that -ini_file is processed first
	 update_defaults(); // these were moved on 110101 since too early for local ini file
#if TT_DEBUG_ON
    add_to_log_if_interesting_error(0,"after update_defaults");
#endif
	 set_directory_names(); // new on 300500
#if TT_DEBUG_ON
    add_to_log_if_interesting_error(0,"after set_directory_names");
#endif
	 if (language_to_set != NO_LANGUAGE_SPECIFIED) {
		 set_language(language_to_set);
  #if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
       load_string_library();
	 };
#if TT_DEBUG_ON
    add_to_log_if_interesting_error(0,"after load_string_library");
#endif
    tt_country_code[0] = 0; // unless overridden
	 tt_country_code[2] = 0; // terminate string
    if (tt_language == NO_LANGUAGE_SPECIFIED) {
       int language_answer = default_setting(AC(IDC_LANGUAGE));
       ascii_string language_key;
       if (language_answer > 0) {
          language_key = copy_ascii_string("Language1"); // need to copy to clobber
          itoa(language_answer,language_key+8,10); 
       } else {
          language_key = "Language";
       };
       ascii_string default_language = ini_entry(AC(IDC_VERSIONS),language_key,FALSE);
 #if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		 if (language_answer > 0) delete [] language_key;
       if (default_language != NULL) {
          set_language(string_to_language(default_language,FALSE));
          delete [] default_language;
       };
       if (tt_language == NO_LANGUAGE_SPECIFIED) {
          tt_language = UNKNOWN_LANGUAGE;
       };
    };
#if TT_DEBUG_ON
    add_to_log_if_interesting_error();
#endif
    load_string_library(); // no-op if done; otherwise default
//#if TT_32
//	 if (command_line_count == 1) { // just the file name
//		 command_line_size = 0;
//	 };
//#endif
   // command line might read user profile which needs the following
   boolean didnt_exit_ok_last_time = (boolean) GetPrivateProfileIntG(AC(IDC_SWITCHES),AC(IDC_TOONTALK_STARTED),0,tt_ini_file);
   write_ini_entry(AC(IDC_SWITCHES),AC(IDC_TOONTALK_STARTED),"1");
	write_ini_entry("FTP","UserName"," "); // new on 011104 so don't use previous user's name and password if ToonTalk crashed the previous session
//   MainWindow MainWnd;
//   tt_main_window = &MainWnd;
	tt_main_window = new MainWindow();
   tt_favorites = new FavoriteSizeAndLocation[favorites_count];
#if TT_32
	header_memory = new unsigned char[dib_header_size]; // could use dib_header_size_no_palette when tt_bits_per_pixel > 8
   default_header = (LPBITMAPINFO) header_memory;
#endif
	allocate_puzzle_data();
#if TT_32
	// need to do this a bit early since might be needed to interpret command line options
	if (!ini_int("Switches","DontOpenInternetConnection",FALSE)) { // conditional new on 060200
#if TT_MICROSOFT
		tt_internet_connection = InternetOpen("ToonTalk",INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,0);
#endif
	};
#endif
   // moved here 040899
   tt_joystick_pictures = new JoystickRemotePicturePointer[max_number_of_joysticks]; 
	// changed 300799 from tt_joystick_count to max_number_of_joysticks
	// so that programs loaded that use joysticks can still dump things even if joysticks aren't attached
   short int i;
   for (i = 0; i < max_number_of_joysticks; i++) {
      tt_joystick_pictures[i] = new JoystickRemotePicture(i);
		//070904 remove THIS!!!!
//		tt_error_file() << (int) tt_joystick_pictures[i]->kind_of() << " " 
		//					 << (int) tt_joystick_pictures[i]->appearance_code() << " " << tt_joystick_pictures[i]->image_cycle_index() << endl;
   };
#if TT_DYNA_ZIP
	// moved here since if the log opened below is an archive then this needs to be initialized
	// moved even earlier on 121003 since while waiting for name below the user could double click on a TT file that needs to unzip media
	initialize_zip();
	initialize_unzip();
#endif
	interpret_command_line(command_count,commands); //,TRUE);
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "Finished interpret_command_line" << endl;
#endif
	if (!tt_ini_file_from_tt_ini_file && tt_local_ini_file != NULL) { // added !tt_ini_file_from_tt_ini_file on 060304
		// just a double clicked on a CTY or TT file or the like so following needs to be down again local ini file set
		UnwindProtect<ReplayState> set(tt_replay,NO_REPLAY); // new on 070201 since this might replay an ini_entry from a log file
		update_defaults();
		set_directory_names(); // redundant now that compute_local_ini_file_name does this too??
	};
	for (i = 0; i < command_count; i++) { // 0 was command_line_count prior to 300502
		delete [] commands[i];
	};
	HDC visible_device_context = GetDC(NULL); // of screen - moved here on 250901 since need to know while initializing and replaying logs
	tt_bits_per_pixel = GetDeviceCaps(visible_device_context,BITSPIXEL);  // new on 220901
	// warn about lack of 16-bit support??
//	if (tt_bits_per_pixel == 16 || tt_bits_per_pixel == 15) {
//		tt_desired_bits_per_pixel = 8;
//	};
	if (windows95()) {
		tt_bits_per_pixel = 8; // since GDI+ doesn't work
		tt_prefer_gdiplus = FALSE;
	};
   tt_main_window->set_device_bits_per_pixel(tt_bits_per_pixel); 
	// new on 100602 - this is really what Windows is currently set to -- moved after above on 190602
	if (tt_bits_per_pixel > 8) {
		HDC device_context = visible_device_context; // no need to get it again - prior to 300402 was GetDC(NULL);
		GetRGBBitsPerPixel(device_context,&tt_red_bits_per_pixel,&tt_green_bits_per_pixel,&tt_blue_bits_per_pixel);
//		ReleaseDC(NULL,device_context); // commented out on 100502 since no longer makes sense and is done below
   };
	tt_bytes_per_pixel = tt_bits_per_pixel/8;
//	tt_prefer_gdiplus = (tt_bits_per_pixel != 8); // works better for greater than 8-bit graphics (some palette issues) 
	ReleaseDC(NULL,visible_device_context);
	initialize_sound(); // moved here as experiment on 070904 -- shouldn't matter -- apparently the problem was related to increasing the stack size
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "Finished interpret_command_line" << endl;
#endif
#if TT_GDIPLUS
	GdiplusStartupInput gdiplusStartupInput;
   GdiplusStartup(&gdiplusToken,&gdiplusStartupInput,NULL);
	initialize_temp_png();
#endif
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "Finished initialize_temp_png" << endl;
#endif
	tt_missing_picture_file_extension = ini_entry("FileExtensions","MissingPictureFileExtension",FALSE); 
	// new on 021101 - here because used if loading a city below - 
	// on 240901 moved here since can be used when initializing (e.g. backgrounds from files)
	tt_missing_builtin_picture_file_extension = ini_entry("FileExtensions","MissingBuiltinPictureFileExtension",FALSE); // new on 121001
	tt_desired_bits_per_pixel = ini_int("Switches","DesiredFullScreenBitsPerPixel",FALSE,tt_desired_bits_per_pixel);
	tt_number_output_base = ini_int("Switches","DefaultNumberDisplayBase",FALSE,tt_number_output_base);
	if (tt_number_output_base < 2 || tt_number_output_base > 36) {
		tt_error_file() << "Number base must be between 2 and 36. Cannot be " << tt_number_output_base << ". Reset to 10." << endl;
		tt_number_output_base = 10;
	};
	tt_maintaining_private_media_directory = ini_int("Switches","SavePrivateCopiesOfMedia",FALSE,
																	 tt_maintaining_private_media_directory); 
	// new on 200303
	// perhaps the following shouldn't be documented (or used much) since it is very expensive (and there is a memory leak)
	// it is now obsolete and works well but only if TT_ALPHA_FEATURE -- not need for the toontalk.ini switch or the variable
	// on 200105 noticed following isn't used anymore 
//	tt_save_media_in_sub_notebooks  = ini_int("Switches","SaveMediaInSubNotebooks",FALSE,tt_save_media_in_sub_notebooks); 
	// new on 200903
	tt_save_media_in_main_notebook = ini_int("Switches","SaveMediaInMainNotebook",FALSE,tt_save_media_in_main_notebook); 
	// new on 270204
	tt_make_current_exe_default = ini_int("Switches","MakeCurrentExeDefault",FALSE,tt_make_current_exe_default); // new on 020304
//	tt_install_current_exe = ini_int("Switches","InstallCurrentExe",FALSE,tt_install_current_exe); // new on 020304
	tt_digits_after_decimal_point_in_java = ini_int("Java","DigitsAfterDecimalPoint",FALSE,tt_digits_after_decimal_point_in_java);
	if (tt_fixed_font_name == NULL) { 
		// this moved here on 200202 since loading a city might require the font names to calculate sizes of text and number pads
	   if (need_wide_characters()) {
#if TT_FOR_FAR_EAST_WINDOWS
			tt_fixed_font_name = copy_ascii_string("‚l‚r ‚oƒSƒVƒbƒN");
#else
		   tt_fixed_font_name = copy_ascii_string("MS Gothic");
#endif
		} else {
		   tt_fixed_font_name = copy_ascii_string("Courier New");
		};
	};
   if (tt_variable_font_name == NULL) {
	   if (need_wide_characters()) {
#if TT_FOR_FAR_EAST_WINDOWS
			tt_variable_font_name = copy_ascii_string("‚l‚r ‚oƒSƒVƒbƒN");
#else
		   tt_variable_font_name = copy_ascii_string("MS Gothic");
#endif
		} else {
		   tt_variable_font_name = copy_ascii_string("Arial");
		};
   };
   if ((exclusive_screen_desired() || tt_debug_mode == 100502) && tt_bits_per_pixel != tt_desired_bits_per_pixel) {
      change_pixel_depth(tt_desired_bits_per_pixel); // new on 100502 since best to update it soon before initializing too much
      pixel_depth_has_changed();
   };
   if (tt_bits_per_pixel == 8) { // copied here on 101002
		update_directory_and_extension_since_8bit();
	};
#if TT_DIRECT_INPUT
   // do this early so I know how many joysticks are connected -- this is even earlier than prior to 021002
   if (tt_using_direct_input) {
      tt_using_direct_input = initialize_direct_input(Main::hInstance); // window_handle,main_instance()); 
		if (!tt_using_direct_input) { // new on 150404
			tt_error_file() << "Failed to initialize direct input. Proceeding but joysticks and force feedback will be ignored." 
								 << endl;
		};
   };
#endif
//#if TT_DYNA_ZIP
//	// moved here since if the log opened below is an archive then this needs to be initialized
//	// moved even earlier on 121003 since while waiting for name below 
	// the user could double click on a TT file that needs to unzip media
//	initialize_zip();
//	initialize_unzip();
//#endif
	// moved here on 280104 since this will delete a log file opened by open_log below
	string demo_file_left_open_last_session = ini_entry("User","OpenDemoFileName",FALSE);
	if (demo_file_left_open_last_session != NULL) {
		// new on 140904
		char source_files[MAX_PATH];
		strcpy(source_files,tt_extracted_file_temp_directory);
		strcat(source_files,"*.*");
		tt_file_temp_directory_is_up_to_date = zip_file(demo_file_left_open_last_session,source_files,NULL,
																		default_compression_level,TRUE);
		delete [] demo_file_left_open_last_session;
	} else {
		tt_file_temp_directory_is_up_to_date = delete_all_files_in_directory(tt_extracted_file_temp_directory); 
		// to ensure it is up to date
	};
	// following moved here on 271199 so it happens at same phase when recording or replaying
	if (replaying()) { // conditional added 250100
		open_log(NULL,tt_error_file()); // no-op if no log file name set
	};
   if (tt_log_out_file_name != NULL) { // as in -o log_file_name
//  		log_initialization(tt_log_out_file_name); // commented out on 030300 since done later now
//      delete [] log_name;
	} else {
		tt_log_count_max = ini_int(AC(IDC_SWITCHES),AC(IDS_GENERATE_LOGS),FALSE);
		if (tt_time_travel_enabled && !replaying()) { // only off if turned off on the command line - e.g. for Playground city
			// added replaying test on 181203 since if replaying a log shouldn't set time travel parameters
#if TT_SPECIAL_DEBUGGING
		   tt_special_debugging_file << "Possibly turning on time travel since tt_time_travel_enabled is " 
											  << (int) tt_time_travel_enabled << endl;
			tt_special_debugging_file << "and tt_time_between_new_logs is " << tt_time_between_new_logs << endl;
#endif
			if (tt_log_count_max == 0) {
				tt_log_count_max = 40; 
				// default is 10 minutes of logs if nothing in toontalk.ini but if check mark on what is your named dialog
			};
			if (tt_log_count_max != 0 && tt_time_between_new_logs >= 0) { 
				// negative value of tt_time_between_new_logs means infinity (as of 280803)
				int seconds_between_new_logs = ini_int(AC(IDC_SWITCHES),"SecondsBetweenLogs",FALSE);
				if (seconds_between_new_logs == 0 && tt_time_travel_enabled) {
					seconds_between_new_logs = 10; // default if nothing in toontalk.ini -- was 15 prior to 021203
				};
				if (seconds_between_new_logs > 0 && tt_system_mode != PUZZLE) { // new on 250903 to make sure not playing puzzle game
					if (tt_time_between_new_logs <= 0) // so if set on the command line it isn't overridden here -- new on 131003
					tt_time_between_new_logs = seconds_between_new_logs*1000;
//					tt_might_include_media_in_time_travel_archives 
					// = ini_int("Switches","MightWantToExportTimeTravelArchivesWithMedia",FALSE,
					// tt_might_include_media_in_time_travel_archives); // new on 210703
					tt_include_media_in_time_travel_archives = ini_int("Switches","IncludeMediaInTimeTravelArchives",FALSE,
																						tt_include_media_in_time_travel_archives); // new on 210703
					tt_keep_all_time_travel_segments = ini_int("Switches","KeepAllTimeTravelSegments",FALSE,
																			 tt_keep_all_time_travel_segments);
					tt_maximum_time_travel_overhead = ini_int("Switches","MaximumPauseDurationDueToTimeTravel",FALSE,
																			tt_maximum_time_travel_overhead);
					tt_dump_as_xml = TRUE; // time travel just won't work otherwise - new on 170903
				//} else {
				//	tt_might_include_media_in_time_travel_archives = FALSE; 
					// new on 200803 to avoid this extra work if time travel is off
					// why not enable this for ordinary logs as well??
				};
				//if (tt_debug_mode == 250100) { // only when making demos where I want credit screens to be part of it
				//	tt_next_new_log_time = tt_current_time+tt_time_between_new_logs;
				//	close_log_and_open_next();
				//};
			};
		};
	};
	// moved here on 150903 from seconds_between_new_logs > 0 branch above so that if you turn off time travel you still end up where you left off
	if (tt_city_name == NULL && time_travel_enabled()) { 
		// rewritten 211003 since otherwise saved cities not used if there is a replay log and a time travel archive
//	if (tt_city_name == NULL && (tt_time_between_new_logs > 0 || log_in_archive() != NULL)) {
		// default when running with time travel is to resume where you left off - new on 110603
		tt_city_name = most_recent_city_file_name();
//#if TT_ALPHA // wrong modularity and the file doesn't exist (yet) but is typically in time travel archive - commented out 021003
//		if (!file_exists(tt_city_name)) { // new on 011003
//			delete [] tt_city_name;
//			tt_city_name = NULL;
//		};
//#endif
	};
	tt_ok_to_optimize_successive_demos = ini_int("Switches","OKToOptimizeSuccessiveDemos",FALSE,tt_ok_to_optimize_successive_demos); // new on 220703
	tt_max_number_of_time_lines = ini_int("Switches","MaximumNumberOfTimeLines",FALSE,tt_max_number_of_time_lines); // new on 240703
#if TT_ALPHA_FEATURE
	tt_duration_to_trigger_auto_hide = ini_int("Switches","DurationBeforeAutoHideTimeTravelButtons",FALSE,tt_duration_to_trigger_auto_hide); 
	// new on 051103
	tt_subtitles_and_narration_even_without_demo = ini_int("Switches","SubtitlesAndNarrationEvenWithoutDemo",FALSE,
																			 tt_subtitles_and_narration_even_without_demo); // new on 061103 -- obsolete???
	tt_unicode_clipboard = ini_int("Switches","ClipboardTextMayBeUnicode",FALSE,tt_unicode_clipboard); // new on 101203
	tt_remove_playground_active_documentation = ini_int("Switches","RemovePlaygroundActiveDocumentation",FALSE,
																		 tt_remove_playground_active_documentation); // new on 181203
#endif
	set_spoken_language(tt_alternative_spoken_language); // moved here on 170300
   if (tt_city_name == NULL) tt_city_name = copy_ascii_string(tt_file_name);
// following didn't seem to work but setting the language in the control panel does
//#if TT_SWEDISH
//    SetThreadLocale(MAKELCID(MAKELANGID(LANG_SWEDISH,SUBLANG_DEFAULT),SORT_DEFAULT));
//#endif
//	 Ctl3dRegister(hInstance);
//  Ctl3dAutoSubclass(hInstance);
	if ((Main::hPrevInstance != NULL && exclusive_screen_desired()) || user_wants_to_quit) {
//		show_cursor(TRUE); // just in case
//      unload_string_library();
//      tt_exit_failure(DONT_TROUBLE_SHOOT,"Trying to run ToonTalk full-screen and another ToonTalk is already running.");
// also tried standard_exit(TRUE); // but this is a bad time to cleanup since only partially initialized
      premature_exit = TRUE;
		write_ini_entry(AC(IDC_SWITCHES),AC(IDC_TOONTALK_STARTED),"0"); 
		return(TRUE); // ToonTalk already running and trying to create full-screen one
	};
	if (window_title != NULL) delete [] window_title; // new on 130104
	window_title = new_title_for_window(tt_user_name); // updated on 221103
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "Finished new_title_for_window" << endl;
#endif
	if (window_title == NULL) { // new on 050203 to recover better from inability to load string DLL
		premature_exit = TRUE;
		return(TRUE);
	};
	//const_string title = S(IDS_TOONTALK_WINDOW_TITLE);
	//if (title == NULL) { // new on 050203 to recover better from inability to load string DLL
	//	premature_exit = TRUE;
	//	return(TRUE);
	//};
	//string unicode_window_title = new character[_tcslen(title)+_tcslen(tt_user_name)+6];
	//_tcscpy(unicode_window_title,title);
	//_tcscat(unicode_window_title,_T("  -  "));
	//_tcscat(unicode_window_title,tt_user_name);
	//window_title = u2a(unicode_window_title);
	if (!tt_initialize()) {
//      tt_exit_failure(DONT_TROUBLE_SHOOT);  // should this be trouble shot?
      premature_exit = TRUE;
      return(TRUE); // failure
   };
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "Finished tt_initialize" << endl;
#endif
   SetWindowTextG(tt_main_window->get_handle(),window_title);
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "Finished SetWindowTextG(tt_main_window->get_handle(),window_title)" << endl;
#endif
 	tt_initialize_objects();
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "Finished tt_initialize_objects" << endl;
#endif
	initialize_background_sprites(); // moved here on 250901
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "Finished initialize_background_sprites" << endl;
#endif
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "Finished initialize_background_sprites" << endl;
#endif
#if TT_DEBUG_ON||TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "Command line is: " << endl << CmdLine << endl;
#endif
   // swapped these since history sometimes expects things like tt_screen to exist
	if (!replaying() && !(tt_jump_to_current_log_segment || tt_jump_to_youngest_log_segment)) {
		// condition updated 290204
		try {
		   resume_initialize_history(); // added !tt_jump_to_current_log_segment on 210703
		} catch (...) {
		   // new on 011105
		   handle_internal_error("Sorry, an internal error occurred while loading your user profile. Try logging into ToonTalk with a different user name. You can help improve ToonTalk by sending us your USR file. You can find it in the same folder as this file.",FALSE);
		};
	};
//	MainWnd.initialize();
	tt_main_window->initialize();
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "Finished tt_main_window->initialize()" << endl;
#endif
#if !TT_DIRECTX
#if GRAPHICS_TO_MEMORY
	if (tt_dispdib_mode < 0) { // not possible to use dispdib -- e.g. 640x480
//		tt_using_dispdib = 2;
      tt_exclusive_screen_desired = FALSE;
      tt_window_expansion_factor = 2;
		write_ini_entry(C(IDC_SWITCHES),C(IDC_FULL_SCREEN_MODE_STARTED),"0");
	};
	if (exclusive_screen_desired()) {
//#if TT_32
//		tt_using_dispdib = 2; // reset to large screen via WinG
//#else
#if DISPDIB
		load_dispdib();
#endif
#endif
	};
#endif
//	initialize_sound_library();
//	initialize_pictures();
	//if (!report_initialization_problems()) { // user wants to cancel -- commented out on 300904 when noticed to be obsolete
	//	show_cursor(TRUE,TRUE); // just in case
 //     write_ini_entry(AC(IDC_SWITCHES),AC(IDC_TOONTALK_STARTED),"0"); // shouldn't trigger trouble shooting
 //     premature_exit = TRUE;
	//	exit(FALSE);
	//};
#if TT_DEBUG_ON
	if (tt_debug_mode == 51001) {
		tt_error_file() << "No initialization problems reported." << endl;
	};
#endif
#if TT_DEBUG_ON
   add_to_log_if_interesting_error(0,"no initialization problems to report");
#endif
	if (tt_system_mode != PUZZLE) {
      // moved here so that the behavior is the same whether logs are generated or not -- since logs need tt_favorites
		if (tt_toolbox == NULL) {
			tt_toolbox = new Toolbox();
			tt_toolbox->initialize();
		};
      tt_toolbox->update_tt_favorites();
   };
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "Finished tt_toolbox->update_tt_favorites()" << endl;
#endif
	// moved here on 201199 since cities are loaded early when running demos
   // changed on 200799 since CTY is now saved with DMO
	if (tt_system_mode == PUZZLE || tt_jump_to_current_log_segment || (!replaying() && !tt_city->load_city(tt_city_name))) { 
		//  || tt_log_version_number < 16
		tt_city->build_initial_houses();
	};
   // commented out the following since why save and restore only to destroy??
	//if (tt_martian != NULL) { 
	// new on 260601 to create a fresh Marty in case it was saved (since might matter for time travel but then wouldn't be here)
	//	tt_martian->hide_balloon(); //  so that it is shown properly
	//	tt_martian->remove_text(TRUE); 
	//};
	set_random_number_seed(1); // new on 051100 for recording demos right
//#if TT_DEBUG_ON
	// to see what happens when double click on web link to DMO, PZL, etc.
//	log(lpszCmdLine);
//#endif
// used to set focus here
//	MoveWindow(window_handle,
//              (physical_screen_width-window_width)/2,(physical_screen_height-window_height)/2,
//              window_width,window_height,FALSE);
//tt_error_file() << "initialization_completed " << window_width << "x" << window_height << endl;
//#if !TT_MICROSOFT
//	if (tt_debug_mode == 0) 
//#endif
	if (!tt_show_mouse_cursor) show_cursor(FALSE);
   if (tt_autodemo != NO_AUTODEMO) {
      string line = a2u(ini_entry(AC(IDC_SWITCHES),AC(IDS_AUTO_DEMO_SUBTITLE),FALSE));
      if (line != NULL) {
         tt_screen->set_subtitle(line,(short) _tcslen(line),
                                 max_long-tt_current_time); // about 2 million seconds
         delete [] line;
         // this turns off ordinary subtitles
         tt_subtitles_speed = 0;
      };
   };
	// added the following on 300500 since I've seen reports of palette being wrong until pause/resume
//	set_back_and_front_surface_palette(); - moved below on 160301
	int minimum_sample = ini_int("Switches","MinimumTimeSample",FALSE,1); // new on 100203 -- added FALSE on 140203
	if (minimum_sample > 0) {
		if (timeBeginPeriod(minimum_sample) == TIMERR_NOCANDO) {
			// new on 090103 to get 1 millisecond accuracy -- moved here on 100203 and changed 1 to minimum_sample
			tt_error_file() << "Your system does not allow timing accuracy of " << minimum_sample << "ms" << endl;
		};
	};
	// this is where prior to 100803 generated tt_add_sprite_when_on_floor if tt_file_name_to_clipboard != NULL - but toontalk may be reset after this
	//if (time_travel_enabled()) { // moved as experiment on 260304
	//	save_oldest_and_current_version_numbers(0); // to be sure 0.xml is first
	//};
#if TT_ALPHA_FEATURE
//	tt_file_temp_directory_is_up_to_date = delete_all_files_in_directory(tt_extracted_file_temp_directory); 
	// to ensure it is up to date -- moved on 280104
	// following moved here on 120104 from just before adding &tt_current_time to the log 
	// this way no animations are confused by the change in time
	if (tt_current_log_segment == 1) { 
		if (tt_time_between_new_logs > 0) { // condition new on 181203
			tt_next_new_log_time -= tt_current_time; // new on 171203 so first segment is the right size
		};
		tt_current_time = 0; 
		// new on 221103 to reset clock so easier to make subtitle files and the like -- doesn't really work since reset by log anyway (noted 120204)
	};
#endif
	initialization_completed = TRUE; // moved here on Aug 12 '98 from after the building houses
#if TT_ENHANCED_DRAG_AND_DROP
	// following moved here on 020105 since earlier call was no-op since now checks if initialized
	//if (!initialize_ole()) { // updated 311204 !tt_coinitialized && FAILED(CoInitialize(NULL))) {
	//	return;
	//};
	initialize_ole(); // rewrote the above on 020105 since not clear what to do if it fails to initialize
//	tt_coinitialized = TRUE;
//	if (!replaying() && tt_log_in_archive == NULL) { // don't need this running if running a demo and it causes an error while registering
		register_drag_drop(tt_main_window->get_handle()); // new on 291204 to accept OLE drag and drop (e.g. from a browser)
//	};
#endif
	swap_heads(tt_programmer_head); // new on 190603 -- happens after initialization_completed so images can load
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "Finished swap_heads" << endl;
#endif
	if (tt_logging && tt_system_mode != PUZZLE) {
		dump_city_to_log();
	};
	initialize_button_key_substitutions(); // move here on 271199 for demos to record right
#if TT_DEBUG_ON
	if (tt_debug_mode == 290303) {
		mpq_t x;
		mpq_init(x);
		char numerator[] = "365ac38sb1npj741mf8nan2cfqk7ttkna0lcnm6jm1o8u084cil2cm8k7k0uldvo2d5midj6reacp3vo4a9nd099iu30goljpfje916ue8krch2e44e2i1ert2bf9ubvsivp8dabdgrmk5i76smqahpkgaeh4e25jh5df59qmv2kf2skmb7qo1re8tdvju981pm33bmpdhk0fos8eme0od3fpcf3drj1586nocb0gk39ic4rugck2qa70vnapslciljfc9lg3fak2emv8acgoccmmsic754nika7qj5svmeidei204inha81hddune8gehjuu5bvdhqapvfh6vv63ochu38fp8t0cet47r9tkrvsbe9ttfclohv5f4te9lg8bu6jsfp47ilovp3mp70pheqrnhlio4f54hrs81gp7hdmukpjpat0aa72rhpajuh5gcita95m8crpvtv8ghslk9sou9tor";
		char denominator[] = "k0122mllosei6bp8jpglnoj1198l3kgr27qcbomtm6ttan6aecmpj8eag26vii0fkp0t7clfa40oc0f6c5opsfd44pfkt0jpqhbr0gvegk8u12kn0ad4165lvmo512acea6223leb80cu9gueutsqqgcd82lelsmef62p1obpukbhe5k3q9e6b1gvd9vcrc23nrdl4k2fv8ef1viueaccgduh0kn560h0ivg50ifbgcqjgvectqu11cjajdacavk1v64l5lm3811p9s1j0lv3v0io9v6l132894v7jvvmdbg10rj716a27uq5c723sibq8t1efdu01784ctbg6ciio6bt7jn6i96jje38dkq237plh4p6391vq217fli423qmrb93f1esk767e1qr07l6gelsrrgf739pfj4a41f8kbl6k89mr6p7mmt2r8eu8na8mjtb7f9l0e036ho06bt7rkg6hm600000000";
		int length_n = strlen(numerator);
		int length_d = strlen(denominator);
		int result_n = mpz_set_str(mpq_numref(x),numerator,32);
		int result_d = mpz_set_str(mpq_denref(x),denominator,32);
		mpq_canonicalize(x);
	} else if (tt_debug_mode == 140703) {
		GUID guid;
		millisecond start = timeGetTime();
		for (int i = 0; i < 1000000; i++) {
			UuidCreate(&guid);
		};
		tt_error_file() << "Took " << (timeGetTime()-start) << "ms to use UuidCreate a million times." << endl;
		bytes guid_string;
		for (int i = 0; i < 10; i++) {
			UuidCreate(&guid);
			UuidToString(&guid,&guid_string);
			tt_error_file() << guid_string << endl;
			RpcStringFree(&guid_string);
		};
		start = timeGetTime();
		for (int i = 0; i < 1000000; i++) {
			UuidCreateSequential(&guid);
		};
		tt_error_file() << "Took " << (timeGetTime()-start) << "ms to use UuidCreateSequential a million times." << endl;
		for (int i = 0; i < 10; i++) {
			UuidCreateSequential(&guid);
			UuidToString(&guid,&guid_string);
			tt_error_file() << guid_string << endl;
			RpcStringFree(&guid_string);
		};
		// results:
/*
		Took 328ms to use UuidCreate a million times.
		d6afda9b-a8c4-499a-b8fd-e45eebd06393
		4cfa29b1-e174-4468-be35-b86df6ce6e84
		d59ea4e6-7cd6-4f33-a2b0-1c3adad59278
		6bac8c52-cefa-4e1d-ad5c-76713f332d4a
		ac1eef1b-21a2-4fad-83dc-c7964ea1ac2a
		15376c3a-b360-43f7-9e57-332c3977ac7e
		2307c12d-e8c9-4830-b8ac-257315ccceca
		7d45f74a-7886-4f02-865b-3bc2414eff7c
		f849e42c-7b86-4da1-8dc8-3404c027cf64
		d90292f4-08ab-44cf-b4a5-72bc783511dc
		Took 106ms to use UuidCreateSequential a million times.
		0409a5e0-b5df-11d7-9179-00022d6f2ff8
		0409a5e1-b5df-11d7-9179-00022d6f2ff8
		0409a5e2-b5df-11d7-9179-00022d6f2ff8
		0409a5e3-b5df-11d7-9179-00022d6f2ff8
		0409a5e4-b5df-11d7-9179-00022d6f2ff8
		0409a5e5-b5df-11d7-9179-00022d6f2ff8
		0409a5e6-b5df-11d7-9179-00022d6f2ff8
		0409a5e7-b5df-11d7-9179-00022d6f2ff8
		0409a5e8-b5df-11d7-9179-00022d6f2ff8
		0409a5e9-b5df-11d7-9179-00022d6f2ff8
		*/
	} else if (tt_debug_mode == 150703) {
		const int max = 100000;
		SpritePointer sprites[max];
		LPGUID guids[max];
		for (int i = 0; i < max; i++) {
			sprites[i] = new Sprite(HELIOLND);
		};
		millisecond start = timeGetTime();
		for (int i = 0; i < max; i++) {
			guids[i] = sprites[i]->create_guid();
		};
		tt_error_file() << "Took " << (timeGetTime()-start) << "ms to create " << max << " guids and register the sprites." << endl;
		start = timeGetTime();
		for (int i = max-1; i >= 0; i--) {
			if (sprites[i] != find_sprite(guids[i])) {
				tt_error_file() << "bug in sprite table" << endl;
			};
		};
		tt_error_file() << "Took " << (timeGetTime()-start) << "ms to find " << max << " sprites from their guids." << endl;
		// results on '02 laptop Debug version:
		// Took 901ms to create 100000 guids and register the sprites.
		// Took 180ms to find 100000 sprites from their guids.
	} else if (tt_debug_mode == 80803) { // testing unwind protect
		try {
			UnwindProtect<long> set1(tt_frame_number,999);
			UnwindProtect<DumpReason> set2(tt_dump_reason,DUMPING_TO_CLIPBOARD);
			throw(NULL);
		} catch (...) {
			tt_error_file() << tt_frame_number << endl;
		};
	} else if (tt_debug_mode == 40904) {
		mpz_t ten_to_the_millionth;
		mpz_init(ten_to_the_millionth);
		mpz_ui_pow_ui(ten_to_the_millionth,10,3000000);
		char *string = mpz_get_str(NULL,10,ten_to_the_millionth);
		int length = strlen(string);
		mpz_clear(ten_to_the_millionth);
	};
#endif
//#if TT_DEBUG_ON
//	const double pi = 3.1415926535; // just for now
//	for (int a = 0; a < 91; a++) {
//		tt_error_file() << (int) (sin((pi/180)*a)*1000+0.5) << " ";
//	};
//	tt_error_file() << endl;
	//if (tt_debug_mode == 280102) {
	//	millisecond start = timeGetTime();
	//	const int tenmillion = 10000000;
	//	mpz_t a,zero,one;
	//	mpz_init(a);
	//	mpz_set_si(a,tenmillion);
	//	mpz_init(one);
	//	mpz_set_si(one,1);
	//	mpz_init(zero);
	//	while (mpz_cmp(a,zero) > 0) {
	//		mpz_sub(a,a,one);
	//	};
	//	tt_error_file() << (timeGetTime()-start) << "ms with mp ints" << endl;
	//	{
	//		start = timeGetTime();
	//		rational a,zero,one;
	//		mpq_init(a);
	//		mpq_set_si(a,tenmillion,1);
	//		mpq_init(one);
	//		mpq_set_si(one,1,1);
	//		mpq_init(zero);
	//		while (mpq_cmp(a,zero) > 0) {
	//			mpq_sub(a,a,one);
	//		};
	//		tt_error_file() << (timeGetTime()-start) << "ms with mp rationals" << endl;
	//	};
	//	start = timeGetTime();
	//	int a_int = tenmillion;
	//	int one_int = 1;
	//	while (a_int > 0) {
	//		a_int = a_int-one_int;
	//	};
	//	tt_error_file() << (timeGetTime()-start) << "ms with 32-bit ints" << endl;
	//};
//  tt_error_file() << "sum is " << c << "\n";
//  tt_error_file() << "absolute value is " << abs(c) << "\n";
//#if TT_DEBUG_ON&&TT_XML
//    Sprite *test = xml_load_sprite("c:\\temp\\temp.xml");
//#endif
//#if TT_TEXT_TO_SPEECH
//  if (tt_system_mode == PUZZLE && text_to_speech() && tt_language == AMERICAN) {
//     tt_marty_talk = MARTY_TALK_BALLOONS;
//     log("Sorry, the English puzzles are only available with word balloons -- no talking.");
//  };
//#endif
//#if TT_BETA
	if (!another_toontalk_is_running &&
		 GetPrivateProfileIntG(AC(IDC_SWITCHES),AC(IDC_FULL_SCREEN_MODE_STARTED),0,tt_ini_file)) {
		// had troubles last time
      // not clear how many crashes have anything to do with full-screen mode anymore...
		write_ini_entry(AC(IDC_SWITCHES),AC(IDC_FULL_SCREEN_MODE_STARTED),"0");
		if (exclusive_screen_desired()) {
         didnt_exit_ok_last_time = FALSE; // well - taken care of at least  -- don't want 2 trouble shoot dialogs
         trouble_shoot(TROUBLE_SHOOT_DISPDIB_ABNORMAL_EXIT,NULL,NULL,TRUE);
      };
//          && replaying() == NO_REPLAY &&
//		    !told_user(TOLD_FULL_SCREEN_MAY_CAUSE_CRASH)) {
//			 message_box(MB_ICONINFORMATION|MB_OK,S(IDS_ABANDON_FULL_SCREEN));
//			 if (message_box(MB_ICONQUESTION|MB_YESNO,S(IDS_ABANDON_FULL_SCREEN2)) == IDYES) {
//				 tt_using_dispdib = (char) 2;
//				 tt_user_parameters->using_dispdib = tt_using_dispdib;
//			 };
//			 user_told(TOLD_FULL_SCREEN_MAY_CAUSE_CRASH);
	};
   // warn about problems and offer to trouble shoot before really starting
   if (didnt_exit_ok_last_time && !another_toontalk_is_running) {
		// had troubles last time
      trouble_shoot(TROUBLE_SHOOT_GENERAL_ABNORMAL_EXIT,NULL,NULL,TRUE);
   };
//#if !TT_NEW_IO
//    delete [] tt_temporary_buffer;
//    tt_temporary_buffer = NULL;
//#endif
//#if !TT_MICROSOFT
//	 if (tt_debug_mode == 0) 
//#endif
	 if (!tt_show_mouse_cursor) show_cursor(FALSE); // hide cursor by default
	 restore_cursor(); // just in case
	 // setting focus stuff moved here on 160301 seems to be needed recently when running in a window
	 HWND window_handle = tt_main_window->get_handle();
	 // sometimes ToonTalk is started without focus -- not sure why -- figure this will fix it
#if TT_DEBUG_ON
	  add_to_log_if_interesting_error();
#endif
	 SetFocus(window_handle);
	 tt_main_window->restore_title(); // new on 120401
	 ::SetForegroundWindow(window_handle); // new on 150404
 	 focus_set(); // doesn't do much if !initialization_completed, so initialization_completed is set first (not anymore??)
	 ::SetActiveWindow(window_handle); // new on 160301
	 //if (!tt_mouse_acquired && !replaying()) { // new on 150404 since on Win Me (on laptop) don't get mouse (and occassionally elsewhere)
		// tt_main_window->toggle_exclusive_mouse();
	 //};
	 set_back_and_front_surface_palette(); // moved here on 160301
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "Finished set_back_and_front_surface_palette" << endl;
#endif
	 tt_dump_so_equals_share = ini_int(AC(IDC_SWITCHES),"DumpCopiesCompactly",FALSE,tt_dump_so_equals_share); // moved here on 241001
	 if (!(tt_jump_to_current_log_segment || tt_jump_to_youngest_log_segment)) { // updated condition on 290204
		 if (tt_log_out_file_name != NULL && !replaying()) { // new on 030300 -- was replaying() == NO_REPLAY prior to 170903
//			 if (tt_titles_ended_on_frame <= tt_frame_number) { //  condition new on 230304
				 log_initialization(tt_log_out_file_name);
//			 };
		 } else if (tt_log_count_max != 0 && !time_travel_enabled()) { 
			 // new on 270200 -- new on 200703 to not bother recording the titles for time travel
			 close_log_and_open_next();
		 };
	 };
	 // following was spelled EumulateMouseForFullScreenTimeTravel prior to 260205
	 tt_emulate_mouse_for_full_screen_time_travel 
		 = ini_int("Switches","EmulateMouseForFullScreenTimeTravel",FALSE,tt_emulate_mouse_for_full_screen_time_travel);
	 // following moved on 100904
	 //tt_display_demo_subtitles = ini_int("Switches","DisplayAvailableSubtitlesInDemos",FALSE,tt_display_demo_subtitles);
	 //tt_play_demo_sound_files = ini_int("Switches","PlayAvailableDemoSoundFiles",FALSE,tt_play_demo_sound_files);
	 //tt_speak_demo_text = ini_int("Switches","SpeakAvailableDemoText",FALSE,tt_speak_demo_text);
	 set_recordable_ini_options(); 
    // following is new on 170301
	 string toontalk_exe_name = ini_entry(AC(IDC_EXECUTABLES),"ToonTalk32",FALSE);
	 // moved the code that asked if you wanted to change the default EXE for ToonTalk on 021103
#if !TT_SMALL_TRIAL
	 if (replay_file_name() == NULL) { // better test since replay is reset when time travel is enabled - new on 021103
//	 if (!replaying()) {
		 if (toontalk_exe_name != NULL && stricmp(toontalk_exe_name,AC(IDC_TOONTALK_EXE_NAME)) != 0) {
			 //if (tt_install_current_exe) {
				// // worth implementing ask??
				// char full_toontalk_exe_path[MAX_PATH]; 
			   // new on 020304 to first see if running from somewhere other than ToonTalk installation directory
				// if (GetModuleFileName(NULL,full_toontalk_exe_path,MAX_PATH) != 0) {
				//	 string toontalk_directory = ini_entry("Directories","MainDir",FALSE);
				//	 if (toontalk_directory != NULL) { // otherwise bad installation
				//		 char short_toontalk_directory[MAX_PATH];
				//		 GetShortPathName(toontalk_directory,short_toontalk_directory,MAX_PATH);
				//		 int full_path_basic_name_start = start_of_basic_file_name(full_toontalk_exe_path);
				//		 if (full_path_basic_name_start > 0) {
				//			 char saved_start_of_basic_name = full_toontalk_exe_path[full_path_basic_name_start];
				//			 full_toontalk_exe_path[full_path_basic_name_start] = '\0'; // terminate it there
				//			 char short_full_toontalk_exe_directory[MAX_PATH];
				//			 GetShortPathName(full_toontalk_exe_path,short_full_toontalk_exe_directory,MAX_PATH);
				//			 if (stricmp(short_toontalk_directory,short_full_toontalk_exe_directory) != 0) {
				//				 // they are different directories
				//				 full_toontalk_exe_path[full_path_basic_name_start] = saved_start_of_basic_name;
				//				 char destination_file_name[MAX_PATH];
				//				 strcpy(destination_file_name,toontalk_directory);
				//				 strcat(toontalk_directory,full_toontalk_exe_path+full_path_basic_name_start);
				//				 CopyFile(full_toontalk_exe_path,destination_file_name,TRUE); // don't override old if there is one
				//			} ;
				//		 };
				//		 delete [] toontalk_directory;
				//	 };
				// };
			 //};
			 // running a different EXE than the toontalk.ini indicates
			 if (tt_make_current_exe_default != 0 && tt_make_current_exe_default != 1) {
				 char buffer[1024];
				 output_string_stream message(buffer,1024);
				 message << S(IDS_WANT_TO_UPDATE_TT_INI_TOONTALK32_1,"ToonTalk was installed to run") 
							<< space() << toontalk_exe_name << end_sentence();
				 message << S(IDS_WANT_TO_UPDATE_TT_INI_TOONTALK32_2,
								  "Do you want to update it to run instead the version you are running now?") 
							<< space() << "(" << AC(IDC_TOONTALK_EXE_NAME) << ")";
				 message.put('\0');
				 // added NULL,FALSE on 240602
				 if (message_box(MB_ICONQUESTION|MB_YESNO|MB_SYSTEMMODAL,buffer,NULL,TRUE) == IDYES) { 
					 // added |MB_SYSTEMMODAL on 010401
					 // changed last arg to TRUE on 250602 so this doesn't trigger a continue/quit type dialog itself
					 tt_make_current_exe_default = 1;
				 };
			 };
			 if (tt_make_current_exe_default == 1) {
             write_ini_entry(AC(IDC_EXECUTABLES),"ToonTalk32",AC(IDC_TOONTALK_EXE_NAME));
				 write_ini_entry(AC(IDC_EXECUTABLES),"DemoToonTalk32",AC(IDC_TOONTALK_EXE_NAME)); // always a good idea??
			 };
		 };
	 };
#endif
 	 if (toontalk_exe_name != NULL) {
		 strcpy(tt_application_name,toontalk_exe_name); // new on 260803 so dmp files indicate the version number
		 delete [] toontalk_exe_name; // new on 240601
	 };
 #if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "Finished initializing" << endl;
#endif
	 return(FALSE);
};

void win_main_finalize() {
//    tt_shutting_down = TRUE; -- this interferes with keeping a history of some trouble shooting -- like ran fast in low res
	// following moved here to avoid a problem where activating starttt causes ToonTalk to say it was interrupted by another program
    user_wants_to_quit = TRUE; // so about_to_quit is TRUE
	 // to clean up scheduled stuff (including deletions)
	 if (tt_debugging_leaks) { // conditional new on 150204 since the following can cause errors while shutting down
		 tt_global_picture->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
	 };
    ascii_string image_name = NULL;
    if (tt_autodemo != NO_AUTODEMO) { // compute this now since will need it after freeing up strings
       FileNameStatus file_status;
		 // added args so that this won't search path for file
       image_name = existing_file_name(file_status,"StartTT.exe",NULL,NULL,TRUE,TRUE,FALSE,FALSE,FALSE);
    };
    ascii_string command_line = NULL;
    if (image_name != NULL) { // moved here on 180602 so that kiosk mode works right - AC won't work after a standard_exit
       command_line = ini_entry(AC(IDC_USER),AC(IDS_PREVIOUS_START_TOONTALK_COMMAND),FALSE);
    };
    standard_exit();  // moved here since cleaning up too soon interferes with the StartTT stuff above
	 HWND start_toontalk_window = FindWindowG("StartTT",NULL);
	 if (start_toontalk_window != NULL) {
       // might have been buried -- also timer code depends on being active
#if TT_32
		 if (tt_autodemo != NO_AUTODEMO) { // added this because otherwise NT 4.0 crashes on exit
			 SetForegroundWindow(start_toontalk_window);
		 };
#else
		 SetActiveWindow(start_toontalk_window);
#endif 
	 } else if (tt_autodemo != NO_AUTODEMO) {
       if (image_name != NULL) {        
     	    ShellExecuteG(NULL,"open",image_name,command_line,"",SW_SHOW); // don't care about connected directory...
          if (command_line != NULL) delete [] command_line;
       };
    };
    if (image_name != NULL) delete [] image_name;
	 delete tt_main_window; // new on 151299 since on heap now
};

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, ascii_string CmdLine, int nCmdShow) {
	if (CmdLine != NULL && strstr(CmdLine,"-debug_leaks 1") != NULL) { // new on 180103
		tt_debugging_leaks = TRUE;
	};
#if TT_DEBUG_ON && TT_MICROSOFT
	if (tt_debugging_leaks) {
		_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF); 
		// new on 290302 to get heap leak reports upon exit -- this can slow things down significantly
	} else if (CmdLine != NULL && strstr(CmdLine,"-debug_memory 1") != NULL) { // new on 181004 since otherwise slows things down
		_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_CHECK_CRT_DF); 
		// _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_CHECK_CRT_DF new on 181004
	};
	//double sum = 0.0;
	//double fraction = 1.0;
	//for (int i = 0; i < 100; i++) {
	//	fraction *= .5;
	//	sum += fraction;
	//};		
#endif
#if TT_SPECIAL_DEBUGGING
	tt_special_debugging_file.open("c:\\special_toontalk_log.txt",std::ios_base::out);
#endif
 //  if (get_special_folder(CSIDL_PERSONAL,tt_toontalk_crash_directory)) { // new on 241002
	get_my_documents_folder(tt_toontalk_crash_directory); // updated 050504
   strcat(tt_toontalk_crash_directory,"\\ToonTalk\\");
	// commented out following (both calls to CreateDirectory) on 191204 since don't know if where user files are to be yet
	// if crash before reading command line then if non-standard location
//	::CreateDirectory(tt_toontalk_crash_directory,NULL);
   strcat(tt_toontalk_crash_directory,"Crash Reports\\");
//	::CreateDirectory(tt_toontalk_crash_directory,NULL);
   //} else {
   //   tt_toontalk_crash_directory[0] = '\0';
   //};
   if (tt_want_exception_breaks) {
	   if (win_main_initialize(hInstance,hPrevInstance,CmdLine,nCmdShow)) {
		   return(0); // quit - e.g. because another ToonTalk is running
	   };
   } else {
      try { // new on 151299
		   if (win_main_initialize(hInstance,hPrevInstance,CmdLine,nCmdShow)) {
			   return(0); // quit - e.g. because another ToonTalk is running
		   };
	   } catch (...) {
		   handle_internal_error(S(IDS_INTERNAL_ERROR_STARTING_TOONTALK,
											"Sorry but an internal error occurred while starting ToonTalk. Strange things may happen."),
										 TRUE);
	   };
#if TT_DEBUG_ON
	   if (tt_debug_mode == 51001) {
		   tt_error_file() << "win_main_initialize completed OK." << endl;
	   };
#endif
   };
	if (!replaying() && tt_mouse_acquired && tt_time_travel == TIME_TRAVEL_OFF && !tt_direct_input_mouse_exclusive_ok) {
		// this conditional was at the beginning of MessageLoop() prior to 250703 but that is bad modularity
#if TT_ABSOLUTE_MOUSE
		if (tt_mouse_mode == RELATIVE_MOUSE_MODE) 
#endif
//#if TT_ENHANCED_DRAG_AND_DROP
//			if (!tt_dragging) // && tt_current_time != tt_current_time_pause_ended)  
// if dragging then don't reset mouse position -- condition new on 020105
//#endif
				SetCursorPos(client_center_x,client_center_y);
#if TT_DEBUG_ON&&TT_ENHANCED_DRAG_AND_DROP
			if (tt_debug_mode == 20105 && !tt_dragging) {// && tt_current_time != tt_current_time_pause_ended) {
				tt_error_file() << "Setting cursor[2] to center on frame " << tt_frame_number 
									 << " at time " << tt_current_time << endl;
			};
#endif
#if TT_DEBUG_ON
      if (tt_debug_mode == 1100) {
         GetCursorPos(&center_mouse);
         tt_error_file() << "Just set mouse's position to " << client_center_x << "," << client_center_y << endl;
         tt_error_file() << "Reading it right back got " << center_mouse.x << "," << center_mouse.y << endl;
      };
#endif
	};
   // broke the above and following on 291002 so that initialization can reset the debugging flags
   if (tt_want_exception_breaks) {
		Main::MessageLoop();
	   win_main_finalize();
   } else {	   
	   while (TRUE) {
		   try { 
			   Main::MessageLoop();
			   break;
		   } catch (...) {
			   if (!handle_internal_error(S(IDS_INTERNAL_ERROR,
													  "Sorry but an internal error has occurred. Strange things may happen."),TRUE)) { 
				   break; // user wants to quit
			   };
			   if (about_to_quit()) break;
		   };
	   };
	   try { // new on 151299
		   win_main_finalize();
	   } catch (...) {
		   handle_internal_error(S(IDS_INTERNAL_ERROR_EXITING,"An internal error occurred while shutting down."),FALSE); 
	   };
   };
   return(0); // return_value);
};

void copy_screen_to_file(const_ascii_string file_name, int min_x, int max_x, int min_y, int max_y) {
	// added region args on 220303
	//int revised_min_y = tt_screen_height-max_y;
	//int revised_max_y = tt_screen_height-min_y;
	//min_y = revised_min_y;
	//max_y = revised_max_y;
	if (min_x < 0) min_x = 0;
	if (max_x > tt_screen_width) max_x = tt_screen_width;
	if (min_y < 0) min_y = 0;
	if (max_y > tt_screen_height) max_y = tt_screen_height;
	output_ascii_file_stream file;
	file.open(file_name,std::ios_base::out|std::ios_base::binary);
	if (file == NULL) {
		unable_to_write_file(file_name); // new on 290904 for error recovery
		return;
	};
	BITMAPFILEHEADER file_header;
   unsigned char bm[2] = {'B','M'};
	file_header.bfType = *((WORD *) bm);
	int header_size; // new on 051001
//	if (tt_bits_per_pixel == 8) {
		header_size = dib_header_size;
//	} else { 
		// not clear why this didn't work (PhotoShop was OK with the result but Windows and Paint weren't) - this wastes 1K
//		header_size = dib_header_size_no_palette;
//	};
//   int image_size = tt_screen_width*tt_screen_height*((tt_bits_per_pixel==32||tt_bits_per_pixel==16)?3:tt_bytes_per_pixel);
	int width_with_padding = max_x-min_x;
	int padding = 0;
	//if (width_with_padding&3) { // commented out as experiment 150405
	//	padding = 4-width_with_padding%4;
	//	width_with_padding += padding;
	//};
	const int bytes_per_pixel_in_bmp = 4; // was 3 (and open-coded) prior to 150405
	int image_size = width_with_padding*(max_y-min_y)*((tt_bits_per_pixel==32||tt_bits_per_pixel==16)?bytes_per_pixel_in_bmp:tt_bytes_per_pixel);
	file_header.bfSize = image_size+header_size+sizeof(BITMAPFILEHEADER); // first term was tt_screen_bytes_size prior to 051001
	file_header.bfReserved1 = 0;
	file_header.bfReserved2 = 0;
	file_header.bfOffBits = header_size+sizeof(BITMAPFILEHEADER);
	file.write((char *) &file_header,sizeof(BITMAPFILEHEADER));
   int old_image_size = default_header->bmiHeader.biSizeImage;
	default_header->bmiHeader.biSizeImage = image_size; // moved out of conditional on 220303 - always OK?
	default_header->bmiHeader.biWidth = width_with_padding; 
	// max_x-min_x; -- restored to non padded width on 130405 and restored it back on 150405 (since made incorrect images -- slanted)
	default_header->bmiHeader.biHeight = max_y-min_y;
   if (tt_bits_per_pixel==32 || tt_bits_per_pixel == 16) { // new on 130602   
      default_header->bmiHeader.biBitCount = bytes_per_pixel_in_bmp*8; // was 24;
   };
	file.write((char *) default_header,header_size);
   if (tt_bits_per_pixel==32 || tt_bits_per_pixel == 16) { // new on 130602   
      default_header->bmiHeader.biBitCount = 16;
   };
	default_header->bmiHeader.biSizeImage = old_image_size; // moved on 220303 to restore default_header
	default_header->bmiHeader.biWidth = tt_screen_width;
	default_header->bmiHeader.biHeight = tt_screen_height;
//#if TT_DIRECT // was never used since wasn't TT_DIRECTX
//   lock_front_surface();
//#endif
   work_page saved_page = tt_destination_work_page;
	int width_in_pixels = tt_screen_width*tt_bytes_per_pixel; // this is tt_destination_work_page width
	tt_destination_work_page -= width_in_pixels*min_y;
//	int max_x_with_padding = min_x+width_with_padding;
	if (tt_bits_per_pixel == 32) { // new on 051001 since BMP doesn't support 32-bit pictures
		//for (int i = 0; i < tt_screen_height; i++) {
		//	for (int j = 0; j < tt_screen_width; j++) {
		for (int i = min_y; i < max_y; i++) {
			//for (int j = min_x; j < max_x; j++) {
			//	file.write((char *) tt_destination_work_page+j*4,3); // for every word write the first 3 bytes 
			//};
			file.write((char *) tt_destination_work_page,width_with_padding*4); // copy all the bytes now 150405 -- wasteful but faster (and simpler)
//			if (padding != 0) { // new on 220303 // commented out as experiment 150405
//				file.write("\255\255\255\255\255\255\255\255\255\255\255\255",padding*3); // better to be white and transparent - new on 151003
////				file.write("\0\0\0\0\0\0\0\0\0\0",padding*3); // shouldn't matter what I pad with
//			};
			if (i < max_y-1) tt_destination_work_page -= width_in_pixels; // don't do this if last line
		};
   } else if (tt_bits_per_pixel == 16) { // new on 130602
//      for (int i = 0; i < tt_screen_height; i++) {
		for (int i = min_y; i < max_y; i++) {
         color16 *pixels = (color16 *) tt_destination_work_page;
//			for (int j = 0; j < tt_screen_width; j++) {
			for (int j = min_x; j < max_x; j++) {
            unsigned short color16 = pixels[j];
            file.put((byte) get_blue_from_color16(color16));
            file.put((byte) get_green_from_color16(color16));
            file.put((byte) get_red_from_color16(color16));
				file.put('\255'); // new on 150405
			};
			//if (padding != 0) { // new on 230303 // commented out as experiment 150405
			//	file.write("\255\255\255\255\255\255\255\255\255\255",padding*3); // shouldn't matter what I pad with
			//};
			if (i < max_y-1) tt_destination_work_page -= width_in_pixels;
		};
	} else {
//		for (int i = 0; i < tt_screen_height; i++) {
		for (int i = min_y; i < max_y; i++) {
			// for debuggin
//			for (int j = 0; j < tt_screen_width; j++) {
//				tt_destination_work_page[j] = (char) j;
//			};
			file.write((char *) tt_destination_work_page,width_in_pixels);
			if (i < max_y-1) tt_destination_work_page -= width_in_pixels;
		};
//		file.write((char *) tt_destination_work_page,width_in_pixels);
	};
   tt_destination_work_page = saved_page;
//#if TT_DIRECT
//   unlock_front_surface();
//#endif
	file.close();
};

void copy_screen_to_clipboard() {
	HGLOBAL copy_handle = global_alloc(GHND,tt_screen_width*tt_screen_height*((tt_bits_per_pixel==32)?3:tt_bytes_per_pixel)+dib_header_size);
#if TT_32
	unsigned char *copy = (unsigned char *) GlobalLock(copy_handle);
#else
	unsigned char huge *copy = (unsigned char huge *) GlobalLock(copy_handle);
#endif
   memcpy(copy,default_header,dib_header_size);
   copy += dib_header_size;
#if TT_DIRECTX
   lock_back_surface(); // front surface in -f 0 mode got upper right corner of screen??
#endif
   work_page saved_page = tt_destination_work_page;
	if (tt_bits_per_pixel == 32) { // new on 051001 since BMP doesn't support 32-bit pictures
		int copy_pitch = tt_screen_width*3;
		int source_pitch = -tt_screen_width*tt_bytes_per_pixel;
		for (int i = 0; i < tt_screen_height; i++) {
			for (int j = 0; j < tt_screen_width; j++) {
				for (int k = 0; k < 3; k++) {
					copy[j*3+k] = tt_destination_work_page[j*4+k]; // for every word write the first 3 bytes
				};
			};
			if (i < tt_screen_height-1) {
				tt_destination_work_page += source_pitch;
				copy += copy_pitch;
			};
		};
	} else {
		copy_work_page(tt_destination_work_page,copy,
							tt_destination_width,tt_screen_width);
	};
#if TT_DIRECTX
   unlock_back_surface();
#endif
	GlobalUnlock(copy_handle);
	tt_destination_work_page = saved_page;
	if (OpenClipboard(tt_main_window->get_handle())) {
		EmptyClipboard();
		SetClipboardData(CF_DIB,copy_handle);
		CloseClipboard();
//	} else {
//		play_sound(SYSTEM_ERROR_SOUND);
	}; // else clean up and notify?
};

// restored "const" here on 280800
const long divisor = 2500000L; // after 1 second is travelling at 2/5 screen per second
const millisecond max_duration = 3000; // maxs out at 6/5 screen per second

long arrow_delta_x(millisecond duration) {
	if (duration == 0) { // just started so move one pixel
		return(1);
	} else {
		if (duration > max_duration) duration = max_duration;
		long delta = (tt_screen_width*duration*tt_millisecond_delta)/divisor;
		if (delta == 0) delta++;
		return(delta);
	};
};

long arrow_delta_y(millisecond duration) {
	if (duration == 0) {
		return(1);
	} else {
		if (duration > max_duration) duration = max_duration;
		long delta = (tt_screen_height*duration*tt_millisecond_delta)/divisor;
		if (delta == 0) delta++;
		return(delta);
	};
};

millisecond button_start[4] = {0,0,0,0};

void read_arrow_keys(long &delta_x, long &delta_y) {
	int keys[4] = {VK_LEFT,VK_RIGHT,VK_UP,VK_DOWN};
	for (int i = 0; i < 4; i++) {
		if (GetAsyncKeyState(keys[i]) < 0) {
			millisecond duration; 
			if (button_start[i] == 0) {
				button_start[i] = tt_current_time-tt_millisecond_delta/2;
				duration = 0;
			} else {
				duration = tt_current_time-button_start[i];
			};
			switch (keys[i]) {
				case VK_LEFT:
               if (relative_mouse_mode()) {
					   delta_x -= arrow_delta_x(duration);
               } else {
                  delta_x = -arrow_delta_x(duration);
                  if (!tt_hand_needs_to_move_relatively) {
                     delta_y = 0;
                     tt_hand_needs_to_move_relatively = TRUE; // new on 220402 for absolute mouse mode
                     tt_delta_x_and_delta_y_due_solely_to_arrow_keys = TRUE;
                  };
               };
					break;
				case VK_RIGHT:
					if (relative_mouse_mode()) {
                  delta_x += arrow_delta_x(duration);
               } else {
                  delta_x = arrow_delta_x(duration);
                  if (!tt_hand_needs_to_move_relatively) {
                     delta_y = 0;
                     tt_hand_needs_to_move_relatively = TRUE; // new on 220402 for absolute mouse mode
                     tt_delta_x_and_delta_y_due_solely_to_arrow_keys = TRUE;
                  };
               };
					break;
				case VK_UP:
               if (relative_mouse_mode()) {
					   delta_y += arrow_delta_y(duration);
               } else {
                  delta_y = arrow_delta_y(duration);
                  if (!tt_hand_needs_to_move_relatively) {
                     delta_x = 0;
                     tt_hand_needs_to_move_relatively = TRUE; // new on 220402 for absolute mouse mode
                     tt_delta_x_and_delta_y_due_solely_to_arrow_keys = TRUE;
                  };
               };
					break;
				case VK_DOWN:
               if (relative_mouse_mode()) {
					   delta_y -= arrow_delta_y(duration);
               } else {
					   delta_y = -arrow_delta_y(duration);
                  if (!tt_hand_needs_to_move_relatively) {
                     delta_x = 0;
                     tt_hand_needs_to_move_relatively = TRUE; // new on 220402 for absolute mouse mode
                     tt_delta_x_and_delta_y_due_solely_to_arrow_keys = TRUE;
                  };
               };
					break;
			};
		} else {
			button_start[i] = 0;
		};
	};
};

boolean arrow_key(char key) { // new on 011100
	switch (key) {
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
			// what about diagonals?
			return(TRUE);
		default:
			return(FALSE);
	};
};

boolean ignore_shift_button = FALSE;

void read_shift_and_control_keys() {
	if (tt_control_down) {
		tt_mouse_button |= CONTROL_KEY_DOWN;
	} else if (tt_mouse_button&CONTROL_KEY_DOWN) {
// this is currently used only by the bike pump and
// resetting button_unchanged just confuses it
//		button_unchanged = FALSE;
		tt_mouse_button &= (~CONTROL_KEY_DOWN);
	};
	if (tt_shift_down) {
		tt_mouse_button |= SHIFT_KEY_DOWN;
	} else if (tt_mouse_button&SHIFT_KEY_DOWN) {
		tt_mouse_button &= (~SHIFT_KEY_DOWN);
	};
};

//	// I tried VK_SPACE but then space got thru for cubby labels and stuff
//	if (GetAsyncKeyState(VK_SHIFT) < 0) {
//		if (!ignore_shift_button) {
//			tt_mouse_button |= SHIFT_KEY_DOWN;
//		} else {
//			tt_mouse_button &= (~SHIFT_KEY_DOWN);
//		};
//	} else {
//		ignore_shift_button = FALSE;
//		if (tt_mouse_button&SHIFT_KEY_DOWN) {
//			button_unchanged = FALSE;
//			tt_mouse_button &= (~SHIFT_KEY_DOWN);
//		};
//	};

void simulate_mouse_up() {
	tt_mouse_button = 0;
	tt_dragging = FALSE; // new on 020105
	tt_mouse_up_simulated = TRUE;
//   tt_mouse_button &= ~(LEFT_BUTTON|MIDDLE_BUTTON|RIGHT_BUTTON);
	// following was timeGetTime() but replaying log affected
	// adding half last delta a good estimate
//	tt_mouse_button_up_time = tt_current_time+tt_millisecond_delta/2;
	button_unchanged = FALSE;
	if (tt_shift_down) {
		ignore_shift_button = TRUE;
	};
};

#if TT_MICROSOFT
#define export_pascal __declspec(dllexport)
#else
#define export_pascal PASCAL _export
#endif

BOOL CALLBACK what_name_dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

void ask_what_name() {
#if TT_NO_ROBOT_TRAINING
   string from_dialog = append_3_strings("-n ",S(IDS_RUN_ONLY_VERSION,"Run_only_version")," -time_travel_enabled 0"); 
	// new on 111205
#else
	ascii_string old_name = ini_entry(AC(IDC_USER),AC(IDC_PREVIOUS_NAME),FALSE);
	char parameters[512]; // new on 100903
	if (old_name != NULL) {
		strcpy(parameters,"old_name;");
		strcat(parameters,old_name);
		delete [] old_name;
	} else {
		parameters[0] = '\0';
	};
	if (tt_system_mode == PUZZLE) { // new on 250903 to disable time travel when running the puzzle game
		strcat(parameters,";time_travel_enabled;-1");
	} else if (ini_int("Switches","TimeTravelEnabledDefault",FALSE) != 0 && tt_time_travel_enabled) { 
		// second condition new on 011003	
		strcat(parameters,";time_travel_enabled;1");
	} else {
		strcat(parameters,";time_travel_enabled;0");
	};
   string from_dialog = show_html_dialog_named_in_ini_file("HTMLAskName",parameters);
#endif
   if (from_dialog != NULL) {
#if TT_SPECIAL_DEBUGGING
		tt_special_debugging_file << "HTMLAskName returned: " << from_dialog << endl;
		tt_special_debugging_file << "It was given: " << parameters << endl;
#endif
      if (dialog_closed(from_dialog)) {
         user_wants_to_quit = TRUE;
			wait_cursor(); // why??
      } else {
//         tt_programmer_head = from_dialog[0]-'0';
//         set_tt_user_name(copy_string(from_dialog+1));
			tt_additional_command_line = from_dialog; 
			// this way it is interpreted after the current command line so can override - new on 011003
			if (tt_additional_command_line != NULL) { // new on 061103
				string original_name_plus = strstr(tt_additional_command_line,"-n ");
				int length_of_key = strlen("-n ");
				string name = NULL;
				if (original_name_plus != NULL) {
					string name_plus = original_name_plus+length_of_key;
					int length = strlen(name_plus);
					int name_length = 0; // new on 140605
					for (int i = 0; i < length; i++) {
						if (name_plus[i] == ' ' && i+1 < length && name_plus[i+1] == '-') { 
							// NOTE that if a name contained a space followed by '-' there will be problems here!!!!
							if (i == 0) { 
								// new on 140605 since "" is a name that causes some confusion (e.g. files like ".txt")
								name = SC(IDS_NO_NAME_IS_YOUR_NAME2);
							} else {
								name = copy_string(name_plus,i);
								name_length = i;
							};
							break;
						};
					};
					for (int i = 0; i < name_length+length_of_key; i++) {
						// new on 140605 so -n stuff doesn't get run extra times
						original_name_plus[i] = ' ';
					};
					if (name == NULL) {
						name = copy_string(name_plus,length);
					};
					if (stricmp(name,S(IDS_NO_NAME_IS_YOUR_NAME2)) == 0) {
						// new on 140605 since otherwise will get the "old" NoName stuff which may even include the user names
						// from the last demo run
						string new_name = SC(IDS_NO_NAME_IS_YOUR_NAME2,increment_ini_int(AC(IDC_SWITCHES),"NoNameCounter",FALSE));
						delete [] name;
						name = new_name;
					};
					set_tt_user_name(name);
				};
			}; 
         //interpret_command_string(from_dialog);
         //delete [] from_dialog;
      };
   } else { // fall back on old scheme
	   // trying to get the dialog box to work for wide character sets like Japanese
	   if (DialogBoxA(resource_source(),"ASK_NAME_DIALOG",tt_main_window->get_handle(),what_name_dialog) == -1) {
		   message_box(MB_ICONINFORMATION|MB_OK,S(IDS_ASK_NAME_FAILURE));
	   };
   };
	wait_cursor(); // new on 270900 so if city takes a while to load see hour glass
};

//void create_png_snapshot(Sprite *sprite, string snapshot_file_name) {
//	int extension_start = extension_start_position(snapshot_file_name);
//	strcpy(snapshot_file_name+extension_start,"bmp"); // temporarily make a BMP file
//	sprite->save_snapshot(snapshot_file_name);
//	Sprite *temp_sprite = sprite_from_file_name(snapshot_file_name);
//	if (temp_sprite != NULL) { // shouldn't be otherwise
//		temp_sprite->set_show_all(FALSE);
//		TTImage *image = temp_sprite->pointer_to_current_image();
//		if (image != NULL) {
//			image->load_image(TRUE);
//			DeleteFile(snapshot_file_name); // no point keeping the bmp around anymore
//			string temp_snapshot_file_name = image->return_full_file_name();	
//			strcpy(snapshot_file_name+extension_start,"png"); // restore the PNG name
//			move_file(temp_snapshot_file_name,snapshot_file_name);
//			temp_sprite->destroy();
//		};
//	};
//};

void create_snapshot(Sprite *sprite, string snapshot_file_name, boolean gif) {
	char bmp_snapshot_file_name[MAX_PATH];
	int extension_start = extension_start_position(snapshot_file_name);
	if (extension_start < 0) return; // warn??
	memcpy(bmp_snapshot_file_name,snapshot_file_name,extension_start);
	strcpy(bmp_snapshot_file_name+extension_start,"bmp"); // temporarily make a BMP file
	// worth deleting the BMP file?? - asked 060503
	sprite->save_snapshot(bmp_snapshot_file_name,TRUE);
	boolean transparent = !sprite->rectangular();
	if (gif) {
		save_picture_file_as_gif(bmp_snapshot_file_name,transparent,snapshot_file_name);
		// moved here on 151003 
		DeleteFile(bmp_snapshot_file_name); // new on 210703
	} else {
		// beginning 151003 generate 2 versions with different background colors if transparent
		// then transparent is when they differ
		char bmp_snapshot_file_name2[MAX_PATH];
		if (transparent) {
			memcpy(bmp_snapshot_file_name2,snapshot_file_name,extension_start);
			strcpy(bmp_snapshot_file_name2+extension_start-1,"2.bmp"); 
			sprite->save_snapshot(bmp_snapshot_file_name2,FALSE);
		};
		save_picture_file_as_png(bmp_snapshot_file_name,bmp_snapshot_file_name2,transparent,snapshot_file_name);
	};
	// moved to save_picture_file_as_gif branch on 151003 since save_picture_file_as_png does it is it saves the PNG OK
//	DeleteFile(bmp_snapshot_file_name); // new on 210703
};

//BOOL FAR export_pascal ask_continue_or_quit_dialog(HWND hDlg, UINT message, 
//																	 UINT wParam, LONG lParam);

BOOL CALLBACK ask_continue_or_quit_dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM );

//HWND continue_or_quit_window = NULL;

boolean inside_ask_continue_or_quit = FALSE; // new on 250602 to prevent this from being called recursively
int edited_picture_counter = 2; // new on 220303

void ask_continue_or_quit(boolean interrupted) {
   if (tt_shutting_down) return; // interrupted while quitting
   if (trouble_shooting()) return; // don't ask 
   if (inside_ask_continue_or_quit) return;
   inside_ask_continue_or_quit = TRUE;
//   CloseWindow(tt_main_window->get_handle()); // new on 040602 since if HTML dialog don't want old window in the way 
   ascii_string dialog_key;
	Sprite *in_hand = tt_programmer->pointer_to_tool_in_hand();
#if TT_NO_ROBOT_TRAINING
	if (replaying()) {
		dialog_key = "HTMLDemoPause";
	} else {
		dialog_key = "HTMLNoSavePause";
	};
#else
   if (tt_system_mode == PUZZLE) { // moved here on 070803 so alt-tab of puzzle game doesn't give you a chance to save city
		// commented out on 041099 so can save state while training a robot || tt_screen->inside_thought_bubble()) {
#if TT_ENHANCED_PUZZLE_DIALOG
#if TT_POST3187
		if (in_hand != NULL) {
			dialog_key = "HTMLPuzzlePauseHoldingItem"; // new on 130607
		} else
#endif
			dialog_key = "HTMLPuzzlePause"; // new on 141104
#else
      dialog_key = "HTMLNoSavePause";
#endif
	} else if (interrupted) { // new on 310052
      dialog_key = "HTMLGeneralPause"; // shouldn't happen anymore
	} else if (replaying()) {
		dialog_key = "HTMLDemoPause";
   } else if (in_hand != NULL) {
		dialog_key = "HTMLGeneralPauseHoldingItem";
	} else {
		dialog_key = "HTMLGeneralPause";
	};
#endif
   waiting_for_user_dialog = TRUE;
#if TT_DEBUG_ON
	if (tt_debug_mode == 130502) {
		tt_error_file() << "Called show_html_dialog_named_in_ini_file with " << dialog_key;
	};
#endif
   tt_main_window->no_longer_handling_loss_of_focus(); // new on 250602
   string from_dialog = show_html_dialog_named_in_ini_file(dialog_key); // ,tt_file_name
#if TT_DEBUG_ON
	if (tt_debug_mode == 130502) {
		if (from_dialog == NULL) {
			tt_error_file() << " which returned NULL." << endl;
		} else {
			tt_error_file() << " which returned " << from_dialog << endl;
		};
	};
#endif
   if (from_dialog == NULL) {
      ascii_string dialog_name;
      if (interrupted) {
         dialog_name = "INTERRUPTED_DIALOG";
	   } else if (replaying()) {
		   dialog_name = "DEMO_PAUSED_DIALOG";
      } else if (tt_system_mode == PUZZLE) {
		   // commented out on 041099 so can save state while training a robot || tt_screen->inside_thought_bubble()) {
         dialog_name = "PAUSED_NO_SAVE_DIALOG";
	   } else {
		   dialog_name = "PAUSED_DIALOG";
	   };
      int status = DialogBoxA(resource_source(),dialog_name,tt_main_window->get_handle(),ask_continue_or_quit_dialog);
	   if (status == -1) {
		   message_box(MB_ICONSTOP|MB_OK|MB_SYSTEMMODAL,
			   _T("Windows could not create a dialog box.\rSystem resources may be low.\rClose programs or restart to your computer.\rIf this problem persists please report it to Support@ToonTalk.com.\rQuitting, sorry."));
		   user_wants_to_quit = TRUE;
         inside_ask_continue_or_quit = FALSE;
		   return;
	   };
#if TT_BUG_FIXES
		from_dialog = new char[16]; // new on 021203 to act as if a string representing a number was returned
		itoa(status+1,from_dialog,10);
#endif
   };
   while (from_dialog != NULL) {
      waiting_for_user_dialog = FALSE;
      if (dialog_closed(from_dialog)) {
         unpause_toontalk();
         from_dialog = NULL;
      } else {
         int button_number = atoi(from_dialog); // was from_dialog[0]-'0' prior to 210303
         delete [] from_dialog;
         from_dialog = NULL;
          switch (button_number) {
				case 5: // take control (from demo)
               close_input_log(TRUE,FALSE);
					if (!tt_subtitles_and_narration_even_without_demo) { 
						// otherwise want to keep SPK and update DMO
					   stop_sound(TRUE);
					};
					// and do the following - prior to 280103 this was a case on its own but it should all that "back to TOonTalk" does
            case 1: 
					back_to_toontalk(FALSE);
               break;
				// case 2 moved to share with case 12 on 100204
            case 3: // leave ToonTalk
					if (tt_city_document_to_load_after_demo_ends && replaying()) {
						// new on 240105 so the 'Leave Demo' returns to old city
						stop_replay();
						tt_city_document_to_load = tt_city_document_to_load_after_demo_ends;
						tt_city_document_to_load_after_demo_ends = NULL;
						back_to_toontalk(FALSE);
					} else {
						user_wants_to_quit = TRUE;
					};
               break;
				case 6: // save everything including media files (new on 210303)
					tt_dump_as_xml = TRUE; // no choice - new on 170903 - and do the following
            case 4: // save everything
#if TT_TRIAL_VERSION
               from_dialog = show_html_dialog_named_in_ini_file("HTMLNoSavePause",S(IDS_CANT_SAVE_CITY));
#else
					wait_cursor(); // new on 140600
					if (tt_city->dump_to_new_file(tt_file_name,(button_number == 6))) { // basic_name); // was tt_city_name prior to 170599
						// made conditional on 150105
						// basic_file_name added on 150599 so that name is just name it was created with and not full path
                  ascii_string message = append_ascii_strings(tt_file_name,AS(IDS_CITY_SAVED));
//                SetDlgItemText(hDlg,ID_SAVE_EVERYTHING,message);
                  from_dialog = show_html_dialog_named_in_ini_file("HTMLNoSavePause",message);
#if TT_DEBUG_ON
						if (tt_debug_mode == 10399) {
							log(message); // to debug replaying random sensor correctly
						};
#endif
                  delete [] message;
					} else {
						back_to_toontalk(FALSE);
               };
					restore_cursor();
#endif
               break;
				case 7: // save item in hand
				case 8: // save item in hand including media
				case 9: // save item in hand and Java applet
				case 10: { // save item in hand including media and Java applet
					dump_started();
					UnwindProtect<xml_element *> set(tt_zip_description_archive_element,NULL); 
					// new on 060903 so time travel and saving media don't interfere
					UnwindProtect<boolean> set2(tt_saving_media,(button_number == 8 || button_number == 10)); 
					// was an ordinary assignment prior to 060903
					if (tt_saving_media) {
						tt_dump_as_xml = TRUE; // no choice - new on 170903
					};
					if (button_number < 9) {
						// just save what is in the hand
						UnwindProtect<Sprites *> set(tt_sprites_with_media_to_save,NULL); // new on 210703 since time travel also uses this
						char snapshot_file_name[MAX_PATH];
						//if (tt_saving_media) { // include snapshot as well
							// beginning 031003 the snapshot is generated and included regardless of whether media files are saved
							strcpy(snapshot_file_name,tt_user_directory);
							strcat(snapshot_file_name,"snapshot.png");
							create_snapshot(in_hand,snapshot_file_name,FALSE);				
						//};
						UnwindProtect<DumpReason> set2(tt_dump_reason,DUMPING_TO_CLIPBOARD); // new on 300103
						xml_document *document = in_hand->xml_create_document();
						if (document == NULL) { // new on 150105
							dump_ended(); // just in case
							back_to_toontalk(FALSE);
							break;
						};
						char file_name[MAX_PATH];
						//output_string_stream stream(file_name,MAX_PATH);
						//stream << tt_user_directory;
						//stream << "Programs\\"; // new on 100204 -- was \\Programs\\ prior to 080404
						//stream << tt_file_name << ".tt"; 
						//stream.put('\0');
						// rewritten on 080404 since this style avoids problems debugging memory leaks and more flexible (see variant below)
						strcpy(file_name,tt_user_directory);
						char programs_folder_name[MAX_PATH]; // more than enough -- new on 080404 to allow user customization
						string specified_programs_folder_name = ini_entry("Directories","UserProgramsFolderName",FALSE);
						if (specified_programs_folder_name == NULL) {
							strcpy(programs_folder_name,"My Programs\\");
						} else {
							strcpy(programs_folder_name,specified_programs_folder_name);
							strcat(programs_folder_name,"\\");
							delete [] specified_programs_folder_name;
						};
						strcat(file_name,programs_folder_name);
						strcpy(user_programs_directory,file_name); // new on 080404 so Show Folder doesn't have to recompute all this
						strcat(file_name,tt_file_name);
						strcat(file_name,".tt");
						int max_tt_files = ini_int("Switches","TTFileMax",FALSE,10); // new on 080404 -- default of 10 seems reasonable trade-off
						if (max_tt_files > 0 && file_exists(file_name,FALSE)) {
							// don't clobber existing file before copying it to "Old Programs" folder
							char copied_file_name[MAX_PATH];
							strcpy(copied_file_name,tt_user_directory);
							strcat(copied_file_name,programs_folder_name);
							char old_programs_folder_name[MAX_PATH]; // more than enough -- new on 080404 to allow user customization
							string specified_old_programs_folder_name = ini_entry("Directories","OldUserProgramsFolderName",FALSE);
							if (specified_old_programs_folder_name == NULL) {
								strcpy(old_programs_folder_name,"My Old Programs\\");
							} else {
								strcpy(old_programs_folder_name,specified_old_programs_folder_name);
								strcat(old_programs_folder_name,"\\");
								delete [] specified_old_programs_folder_name;
							};
							strcat(copied_file_name,old_programs_folder_name);
							::CreateDirectory(copied_file_name,NULL); // if need be
							strcat(copied_file_name,tt_file_name);
							strcat(copied_file_name,"-saved.tt"); // added -saved to avoid confusion with the original
							maintain_n_versions_youngest_first(copied_file_name,max_tt_files-1,TRUE); // -1 since the unnumbered version is a saved copy
							if (!move_file(file_name,copied_file_name)) { // was CopyFile prior to 291104 but then archive can have old extra stuff in it
								unable_to_write_file(copied_file_name); // new on 011004
							};
						};
						xml_save_document(document,file_name,snapshot_file_name);
						xml_release_document(document);
						tt_file_name_of_tt_object_just_saved = copy_string(file_name); 
						// updated on 140604 to cause this to happen with Java applets as well
					} else {
#if !TT_LOADING_AND_SAVING_INTERRUPTABLE
						CloseWindow(tt_main_window->get_handle());
						tt_main_window->no_longer_handling_loss_of_focus();
#endif // otherwise do it later so can be interrupted
						string java_applet_name = in_hand->java_start();
						if (java_applet_name != NULL) {
							char snapshot_file_name[MAX_PATH];
							strcpy(snapshot_file_name,tt_user_directory);
							strcat(snapshot_file_name,AC(IDC_JAVA_PATH));
							copy_alphanumerics(java_applet_name,snapshot_file_name+strlen(snapshot_file_name)); // was strcat prior to 060605
							strcat(snapshot_file_name,".gif");
							create_snapshot(in_hand,snapshot_file_name,TRUE); // HTML should refer to this
							delete [] java_applet_name;
						};
#if TT_LOADING_AND_SAVING_INTERRUPTABLE
						CloseWindow(tt_main_window->get_handle());
						tt_main_window->no_longer_handling_loss_of_focus();
#endif
					};
					if (tt_file_name_of_tt_object_just_saved != NULL) {
						string to_run_after_saving = ini_entry("Executables","RunAfterSaving",FALSE);
						boolean show_dialog_after_saving = 
//#if TT_ENHANCED_PUZZLE_DIALOG
//							tt_system_mode != PUZZLE && // new on 141007 since saving a city doesn't work in the Puzzle Game
// just left ToonTalk minimized -- need better solution
//#endif
							ini_int("Executables","ShowDialogAfterSaving",FALSE,TRUE);
						if (to_run_after_saving != NULL) {
							char command_line[MAX_PATH*3]; // new on 191204 to send along -ini_file as well
//							strcpy(command_line,tt_file_name_of_tt_object_just_saved);
							quote_file_name(tt_file_name_of_tt_object_just_saved,command_line);
							strcat(command_line," -ini_file ");
							strcat(command_line,tt_ini_file);
							execute(to_run_after_saving,command_line,FALSE); // tt_file_name_of_tt_object_just_saved);
							delete [] to_run_after_saving;
						};
						delete [] tt_file_name_of_tt_object_just_saved;
						tt_file_name_of_tt_object_just_saved = NULL;
						if (show_dialog_after_saving) {
							// following moved here on 140604
							char message[MAX_PATH+100]; 
							// increased on 290903 since there is the path in the link as well -- restored on 051003 since link didn't work
							// rewrote the following on 090203 so you can copy and paste the file path
	//						strcpy(message,S(IDS_SAVED_ON,"Saved ")); // rename?
							//strcpy(message,tt_user_directory);
							//strcat(message,tt_file_name);
							//strcat(message,".tt");
							strcpy(message,S(IDS_SAVED,"")); // generalized to use string tables on 120204
							strcat(message," ");
							strcpy(message,tt_file_name);
							strcat(message,".tt ");
							strcat(message," ");
							strcat(message,S(IDS_SAVED2,"saved"));
							from_dialog = show_html_dialog_named_in_ini_file("HTMLJustSavedPause",message);
						} else {
							CloseWindow(tt_main_window->get_handle());
							tt_main_window->no_longer_handling_loss_of_focus();
						};	
					} else {
						back_to_toontalk(FALSE); // new on 150105
					};
//					tt_saving_media = FALSE;
        			dump_ended(); //release_nest_indices(); // in case built up table to keep bird/nest corespondences
							};
					break;
				case 11: { // edit picture
					CloseWindow(tt_main_window->get_handle());
					tt_main_window->no_longer_handling_loss_of_focus();
					boolean is_a_picture = in_hand->looks_like_is_of_type(PICTURE); // don't count indirections to numbers etc
					TTImage *image = in_hand->pointer_to_current_image();
					string file_name = NULL;
					string short_file_name = NULL;
					if (image == NULL || !is_a_picture || in_hand->is_flipped()) { // synthetic - handled starting 060503
						// added flipped condition on 040603
						file_name = new char[MAX_PATH];
						short_file_name = copy_string("snapshot.png");
						strcpy(file_name,tt_user_directory);
						strcat(file_name,short_file_name);
						in_hand->snapshot(file_name,(in_hand->kind_of() == PICTURE && !in_hand->is_flipped())); 
						// changed on 070503 so that if non-picture then does include followers
					} else if (image != NULL) { 
						file_name = image->return_full_file_name();
						if (file_name != NULL) {
//							short_file_name = image->return_file_name();
							string original_file_name = image->return_file_name(); // updated 060903 since can be full path to IE cache for example
							if (original_file_name != NULL) {
								short_file_name = ::short_file_name(original_file_name,TRUE); // probably a tiny memory leak
							};
						};
					};
					if (file_name != NULL) {
						char temp_file_name[MAX_PATH]; 
						strcpy(temp_file_name,tt_user_directory); 
						if (short_file_name != NULL) { // use image's name if you can
							int short_file_name_extension_start = first_extension_start_position(short_file_name);
							if (short_file_name_extension_start < 0) { // has no extension so use the whole thing -- new on 060404
								short_file_name_extension_start = strlen(short_file_name);
							};
							int start = strlen(temp_file_name);
							memcpy(temp_file_name+start,short_file_name,short_file_name_extension_start);
							temp_file_name[start+short_file_name_extension_start] = '\0';
						} else {
							strcat(temp_file_name,tt_file_name);
						};
						itoa(edited_picture_counter++,temp_file_name+strlen(temp_file_name),10);
						int extension_start = extension_start_position(file_name);
						if (extension_start > 0) {
							strcat(temp_file_name,file_name+extension_start-1); // -1 to include .
						};
						if (!CopyFile(file_name,temp_file_name,FALSE)) {
							unable_to_write_file(temp_file_name); // new on 011004
						};
						string favorite_picture_editor = ini_entry("Executables","PictureEditor",FALSE);
						boolean ok;
						if (favorite_picture_editor != NULL) {
							ok = execute(favorite_picture_editor,temp_file_name);
							delete [] favorite_picture_editor;
						} else {
							ok = execute(temp_file_name);
						};						
						if (ok && is_a_picture && !in_hand->is_flipped()) { // nothing went wrong so paste upon return
							// second condition added 070503 so that doesn't paste in result if non-picture since too confusing 
							// (see yahoogroups discussion)
							// third conditon new on 131103 since it is confusing to have an image of a flipped picture in ToonTalk
							boolean aborted = FALSE; // new on 200105
							tt_add_sprite_when_on_floor = sprite_from_file_name(temp_file_name,aborted);
							if (tt_add_sprite_when_on_floor != NULL) {
								// following adapted from Sprite::top_level_copy on 160605 so the things on the back etc maintain the right connections
								Sprites *saved_copies = save_copied_items();
								{  // to scope the copying properly
									UnwindProtect<Sprite *> set(tt_top_copy,in_hand);
									UnwindProtect<Sprites *> set2(tt_nests_to_set_original_nest,NULL);
									if (in_hand->kind_of() == PICTURE) {
										((Picture *) in_hand)->copy_flipped_followers((Picture *) tt_add_sprite_when_on_floor); 
										// second cast safe since came from picture file
									};
									Sprites *remaining = in_hand->pointer_to_followers();
									if (remaining != NULL) {
										tt_add_sprite_when_on_floor->set_size_and_location_to_those_of(in_hand);
									};
									while (remaining != NULL) {
										// so the followers end up at the right place
										Sprite *sprite = remaining->first();
										tt_add_sprite_when_on_floor->add_follower(sprite->copy(),sprite->completely_inside_leader(),INSERT_AT_END);
										remaining = remaining->rest();
									};
									release_items_copied();
									restore_copied_items(saved_copies);
									if (tt_nests_to_set_original_nest != NULL) {
										process_tt_nests_to_set_original_nest();
									};
								};
								if (tt_top_copy == NULL) { // truely top level
									just_destroy_list(tt_previous_controlee_replacements); // new on 151204
								};						
								tt_add_sprite_when_on_floor->set_show_all(in_hand->show_all());
							};
						};
					} else {
						back_to_toontalk(FALSE); // new on 280403 so if holding an object without an associated file name nothing happens - good policy??
					};
					break;
							};
				case 12: // show user's program directory
					//char users_program_directory[MAX_PATH];
					//strcpy(users_program_directory,tt_user_directory);
					//strcat(users_program_directory,"Programs\\"); // was \\Programs prior to 080404
					// commented out on 080404 since now shared with other code 
					ShellExecute(NULL,"open",user_programs_directory,"","",SW_SHOW); // show directory
					// and do the following
				case 2: // come back later
				   CloseWindow(tt_main_window->get_handle());
               tt_main_window->no_longer_handling_loss_of_focus(); // new on 200502 - moved here on 250602 so doesn't open on next cycle
               break;
#if TT_ENHANCED_PUZZLE_DIALOG
					// new on 141104
//				case 101:
//					previous_puzzle_default(); // hard to implement -- think about this
//					back_to_toontalk();
//					break;
				case 102:
					next_puzzle_default(); // rewritten 151104 -- restored 301104
//					reload_puzzle(); // new on 151104 otherwise can be inside house of new puzzle and not initialized right (and door locked after exiting)
//					next_puzzle_name(); // removed 301104
					House *puzzle_house = pointer_to_puzzle_house();
					if (puzzle_house != NULL) {
						Background *programmer_background = tt_programmer->pointer_to_background();
						if (programmer_background != NULL && programmer_background->pointer_to_house() == puzzle_house) {
							// if bomb is going of in same room as programmer get him out of there -- new on 241104
							tt_programmer->house_about_to_be_destroyed();
							if (tt_toolbox != NULL) tt_toolbox->finish_instantly();
						};
						puzzle_house->blow_up();
					};
					update_slot_ids(FALSE); // new on 301104 so skip a location on the rocket decoration
					back_to_toontalk(FALSE);
					break;
#endif
         }; // else warn??
      };
   }; 
   inside_ask_continue_or_quit = FALSE;
};

BOOL CALLBACK loading_wait_dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

HWND waiting_window = NULL;
flag subtitle_showing = FALSE;

void loading_wait(boolean not_interruptable, boolean initializing_direct_play) { 
	// first arg new on 250105 to override tt_loading_is_ok_to_abort
	// second arg new on 270205 since DirectPlay can take a while to initialize
	if (paused) return; // new on 090401 - can happen while generating a Java applet
#if TT_DEBUG_ON
	if (tt_debug_mode == 230404) {
		tt_error_file() << "loading_wait not_interruptable = " << (int) not_interruptable << endl;
	};
#endif
	if (tt_screen != NULL && tt_variable_font_name != NULL && tt_frame_number > 1) { 
		// full_screen) { // new on 150101 since dialog box and full-screen don't mix well
		// frame number check added on 160301 since too early to see subtitles otherwise
		// added second conjunct on 180101 since might be too early if no font choosen
//#if TT_LOADING_AND_SAVING_INTERRUPTABLE
//		// new on 150105
//		string narrow_subtitle = append_3_strings(S(IDS_LOAD_WAIT,"Loading, please wait."),space(),
																//S(IDS_PRESS_ESC_TO_INTERRUPT,"Press the Esc button to interrupt."));
//		wide_string subtitle = copy_wide_string(narrow_subtitle);
//		delete [] narrow_subtitle;
//#else
//		wide_string subtitle = copy_wide_string(S(IDS_LOAD_WAIT,"Loading, please wait."));
//#endif
		// above rewritten on 180105
		char warning[1024];
		loading_warning((string) &warning,not_interruptable,initializing_direct_play);
		wide_string subtitle = copy_wide_string(warning);
		tt_main_window->show_subtitle(subtitle,wide_string_length(subtitle),
                                    tt_screen->return_subtitle_character_width(),
                                    tt_screen->return_subtitle_character_height()); // moved here on 250105
		tt_screen->display_subtitles(); // rewritten 250105
		//tt_screen->display(FALSE);
		tt_main_window->Paint();
		subtitle_showing = TRUE;
		delete [] subtitle;
	} else {
		if (waiting_window != NULL) return; // already up
		//	toggle_pause(FALSE,FALSE,FALSE);
//		if (tt_frame_number == 1) { // new on 120401
//			CloseWindow(tt_main_window->get_handle());
//			MoveWindow(tt_main_window->get_handle(),0,0,100,100,TRUE);
// 		};
#if TT_32
		waiting_window = CreateDialogA(resource_source(),"LOADING_WAIT",NULL,loading_wait_dialog);
//		SetForegroundWindow(waiting_window); // new on 120401
#else
		DLGPROC dialog_proc = (DLGPROC) MakeProcInstance((FARPROC) loading_wait_dialog,resource_source());
		waiting_window = CreateDialog(resource_source(),"LOADING_WAIT",NULL,dialog_proc);
		FreeProcInstance((FARPROC) dialog_proc);
		SetActiveWindow(waiting_window); // new on 160301
#endif
	};
};

void done_waiting_for_load() {
	if (subtitle_showing) {
		reset_grid();
		tt_screen->screen_dirty();
		subtitle_showing = FALSE;
	} else if (waiting_window != NULL) {
		DestroyWindow(waiting_window);
		waiting_window = NULL;
//		toggle_pause(FALSE,FALSE,FALSE);
//		if (tt_frame_number == 1) { // new on 120401
//			OpenIcon(tt_main_window->get_handle());
//		};
	};
//	if (tt_frame_number < 2) { // problem only when doing this on first frame
//		set_back_and_front_surface_palette(); // new on 160301
//		SetActiveWindow(tt_main_window->get_handle()); // new on 160301
//	};
};

BOOL CALLBACK loading_wait_dialog(HWND , UINT message, WPARAM , LPARAM ) {
	switch (message) {
		case WM_INITDIALOG:
			return(TRUE);
	 };	
	 return(FALSE);
};

char desired_head = -1;

void set_desired_head() {
   // this is called after user profile read so button selection overrides it
   if (desired_head != -1) {
#if !TT_SMALL_TRIAL
      set_programmer_head(desired_head); // was just tt_programmer_head = prior to 180603
#endif
   };
};

int set_programmer_head(int new_value) { // abstracted on 180603
	new_value = new_value%3;
	if (new_value < 0) new_value += 3;
//	update_saved_heads(new_value);
//	if (new_value != tt_programmer_head) { // was (new_value == 2 || tt_programmer_head == 2) prior to 190603
		swap_heads(new_value); // patch for clipping problem with hat brim
//	};
	tt_programmer_head = (char) new_value;
	if (tt_user_parameters != NULL) { // new on 180603 since can be called "too early"
		tt_user_parameters->programmer_head = tt_programmer_head;
		// need to reset cache
		free_all_cache_entries();
	};
	return(new_value);
};

BOOL CALLBACK what_name_dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM ) {
	switch (message) {
		case WM_INITDIALOG: {
			ascii_string old_name = ini_entry(AC(IDC_USER),AC(IDC_PREVIOUS_NAME),FALSE);
			if (old_name != NULL) {
				SetDlgItemTextG(hDlg,IDC_EDIT1,old_name);
				delete [] old_name; // can I delete it now??
			};
//			SetActiveWindow(hDlg); // shouldn't be necessary but I've seen it lose it on some machines
			return(TRUE);
         };
		case WM_COMMAND: 
			switch(wParam) {
            case IDC_BUTTON1:
            case IDC_BUTTON2:
            case IDC_BUTTON3:
				case IDOK: {
//					if (need_wide_characters()) { // new on 290800
//						wide_character buffer[64];
//						if (GetDlgItemTextW(hDlg,IDC_EDIT1,buffer,63) != 0) {
//							set_tt_user_name(copy_narrow_string(buffer));
//						} else {
//							int x = GetLastError();
//							tt_error_file() << "Can't get text from dialog box. Error code=" << x << endl;
//						};
//					} else {
						char buffer[64];
						if (GetDlgItemTextG(hDlg,IDC_EDIT1,buffer,63) != 0) {
							set_tt_user_name(copy_ascii_to_unicode(buffer));
						};
//					};
               switch (wParam) {
                  case IDC_BUTTON1:
                     desired_head = 1; // hair
                     break;
                  case IDC_BUTTON2:
                     desired_head = 2; // hat
                     break;
                  case IDC_BUTTON3:
                     desired_head = 0; // bald
                     break;
               };
					EndDialog(hDlg,0);
					};
					break;
			};
			break;
	  case WM_SYSCOMMAND:
		  switch (wParam) {
				case SC_CLOSE:
					user_wants_to_quit = TRUE;
					EndDialog(hDlg,0);
					wait_cursor();
					break;
		  };
#if TT_DEBUG_ON
        if (tt_debug_mode == 130502) {
           tt_error_file() << "WM_SYSCOMMAND called with " << wParam << endl;
        };
#endif
		  break;
	 };
	 return(FALSE);
};

// CALLBACK was FAR export_pascal

BOOL CALLBACK ask_continue_or_quit_dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM ) {
	switch (message) {
		case WM_INITDIALOG:
//         if (tt_using_dispdib == 1) { // so dialog always easy to see
//             CloseWindow(tt_main_window->get_handle());
//         };
//         pause_toontalk(FALSE,FALSE);
         CloseWindow(tt_main_window->get_handle());
         // so dialog always easy to see
         if (full_screen && !exclusive_screen_desired()) {
            RedrawWindow(NULL,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);  // repaint the entire desktop since might have drawn on it
         };
#if TT_32
         SetForegroundWindow(hDlg);
#else
         SetActiveWindow(hDlg);
#endif
			return(TRUE);
		case WM_COMMAND:
			switch(wParam) {
				case IDCONTINUETOONTALK:
#if TT_DEBUG_ON
               if (tt_debug_mode == 1992) log(_T("User wants to continue."));
					add_to_log_if_interesting_error();
#endif
               waiting_for_user_dialog = FALSE;
//               query_open = TRUE; // equivalent to that
					unpause_toontalk();
               EndDialog(hDlg,0);
//					DestroyWindow(continue_or_quit_window);
					break;
				case IDMINIMIZETOONTALK:
               waiting_for_user_dialog = FALSE;
//               if (!exclusive_screen_desired()) { // not already done -- commented out on 200502
					   CloseWindow(tt_main_window->get_handle());
//               };
#if TT_DEBUG_ON
               if (tt_debug_mode == 1992) log(_T("User wants to minimize."));
#endif;
					EndDialog(hDlg,0);
//					DestroyWindow(continue_or_quit_window);
					break;
				case IDQUITTOONTALK:
#if TT_DEBUG_ON
               if (tt_debug_mode == 1992) log(_T("User wants to quit."));
#endif;
					user_wants_to_quit = TRUE;
               waiting_for_user_dialog = FALSE;
					EndDialog(hDlg,0);
//					DestroyWindow(continue_or_quit_window);
					break;
				case IDQUITDEMO:
#if TT_DEBUG_ON
               if (tt_debug_mode == 1992) log(_T("User wants to quit."));
#endif;
               waiting_for_user_dialog = FALSE;
					close_input_log(TRUE,FALSE);
					if (!tt_subtitles_and_narration_even_without_demo) { 
						// otherwise want to keep SPK and update DMO
					   stop_sound(TRUE);
					};
					unpause_toontalk();
					EndDialog(hDlg,0);
					break;
            case ID_SAVE_EVERYTHING:
#if TT_TRIAL_VERSION
               SetDlgItemTextG(hDlg,ID_SAVE_EVERYTHING,S(IDS_CANT_SAVE_CITY));
#else
               {
//						ascii_string basic_name = basic_file_name(tt_city_name);
						// changed the following on 100200 to use tt_file_name since otherwise with time travel could become current.cty
						wait_cursor(); // new on 140600
						tt_city->dump_to_new_file(tt_file_name); // basic_name); // was tt_city_name prior to 170599
						// basic_file_name added on 150599 so that name is just name it was created with and not full path
                  ascii_string message = append_ascii_strings(tt_file_name,AS(IDS_CITY_SAVED));
//                message_box(MB_ICONINFORMATION,message);
                  SetDlgItemText(hDlg,ID_SAVE_EVERYTHING,message);
						restore_cursor();
                  delete [] message;
//						delete [] basic_name;
               };
               return(TRUE);
#endif
			};
			break;
	  case WM_SYSCOMMAND:
		  switch (wParam) {
				case SC_CLOSE: // same as continue
					waiting_for_user_dialog = FALSE;
					unpause_toontalk();
               EndDialog(hDlg,0);
					break;
//            case SC_SCREENSAVE:
               // dialog box has been up a while if screen saver is running -- minimize to get back into correct state
//					if (full_screen) {
//log("Screen saver running while ToonTalk dialog is active. ");
//                  pause_toontalk(TRUE);
//                  CloseWindow(tt_main_window->get_handle());
//               };
//               break;
		  };
		  break;
	 };
	 return(FALSE);
};


void MainWindow::push_title(string new_title) {
   SetWindowTextG(get_handle(),new_title);
};

#if TT_ALPHA_FEATURE
void MainWindow::restore_title(string new_title) {
	boolean is_a_title_to_restore = (window_title != NULL); 
	// rewritten on 251103 since DMO files mistook themselves for another running TT EXE and exited
	if (new_title != NULL) {
		if (is_a_title_to_restore) delete [] window_title;
		window_title = new_title;
	};
	if (is_a_title_to_restore) {
		SetWindowTextG(get_handle(),window_title);
	};
};
#else
void MainWindow::restore_title() {
	SetWindowTextG(get_handle(),window_title);
};
#endif

void MainWindow::set_window_size(coordinate width, coordinate height, boolean center) { // new on 100602
   width = min(width,device_width); // can't get bigger than the screen
   height = min(height,device_height);
   if (center) {
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         tt_error_file() << "SetWindowPos [5] called with " << ((device_width-width)/2) << " " 
								 << ((device_height-height)/2) << " " << width << " " << height 
								 << " and no flags." << " (frame=" << tt_frame_number << ")"  << endl;
      };
#endif
      SetWindowPos(get_handle(),HWND_TOP,(device_width-width)/2,(device_height-height)/2,width,height,0);
		// prior to 240105 HWND_TOP was HWND_TOPMOST
   } else {
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         tt_error_file() << "SetWindowPos [6] called with " << 0 << " " << 0 << " " << width << " " << height 
								 << " and SWP_NOMOVE." << " (frame=" << tt_frame_number << ")"  << endl;
      };
#endif
      SetWindowPos(get_handle(),HWND_TOP,0,0,width,height,SWP_NOMOVE);
		// prior to 240105 HWND_TOP was HWND_TOPMOST
   };
};

void MainWindow::toggle_full_screen() { // abstracted on 130502
#if TT_DEBUG_ON
   if (tt_debug_mode == 130502) {
      tt_error_file() << "toggle_full_screen called" << endl;
	};
#endif
	if (tt_restore_exclusive_screen) { // tt_using_dispdib == 1) {  // added 311099 -- restructured on 100602
      switch_to_full_screen();
   } else {
      // was (tt_mouse_acquired) prior to 130502 but didn't work in absolute mouse mode
      if (tt_exclusive_screen) { 
			// exclusive_screen_desired()) { // tt_using_dispdib == 1) { -- better to use tt_exclusive_screen here?? yes - updated 100602
         if (tt_desired_bits_per_pixel != device_bits_per_pixel && tt_desired_bits_per_pixel != 0) { // new on 100602
            tt_error_file() << "Can't switch from full-screen mode with a color depth of " << tt_desired_bits_per_pixel 
									 << " to Window's current color depth of " << device_bits_per_pixel << endl;
            pause_toontalk(); // best we can do?
         } else {
            tt_page_flipping = FALSE; // new on 150502
//			toggle_pause(FALSE,FALSE,FALSE); // toggle pause twice to restore picture in new mode
//#if TT_DEBUG_ON
//         if (tt_debug_mode == 130502) {
//            log("Toggling pause first time.");
//         };
//#endif        
            tt_restore_exclusive_screen = TRUE;
            restore_display_mode();
			   release_direct_draw(TRUE); // was FALSE prior to 160502;
#if TT_DEBUG_ON
            if (tt_debug_mode == 130502) {
               tt_error_file() << "After restore_display_mode in toggle_full_screen client size is " << client_width 
										 << " by " << client_height << endl;
            };
#endif
			// next is new on 230800
//			tt_using_dispdib = 0; // replace this with the following
         // following is so that the window doesn't fill the whole screen -- 
         // worth restoring when going back??
            resize_window_and_center(tt_screen_width,tt_screen_height,TRUE); 
				// new on 130502 -- this will use 1/2 size if need be
            initialize_direct_draw(); // moved here on 160502
#if TT_DEBUG_ON
            if (tt_debug_mode == 130502) {
               log("Initialize direct draw from toggle_full_screen.");
            };
#endif
          // experiment 040602: -- removed on 100602 since done above as well
//         resize_window_and_center(tt_screen_width,tt_screen_height,TRUE); // new on 130502 -- this will use 1/2 size if need be
         //CloseWindow(window_handle); 
				// new on 160502 -- seems to be needed to "preserve" this size - no changes while minimized
         //paused = TRUE; // new on 050602 so don't try to restore surfaces or other work
//         toggle_pause(FALSE,FALSE,FALSE); // new on 160502
//         RedrawWindow(NULL,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN); // new on 160502 -- made it worse
//			toggle_pause(FALSE,FALSE,FALSE);
//#if TT_DEBUG_ON
//         if (tt_debug_mode == 130502) {
//            log("Toggling pause second time.");
//         };
//#endif
#if TT_DEBUG_ON
            if (tt_debug_mode == 130502) {
               tt_error_file() << "Switched from full-screen to windowed and gave up the mouse." << endl;
            };
#endif
         };
		};
   };
};

void MainWindow::toggle_exclusive_mouse() {
   // a no-op if running absolute mouse mode
   if (!relative_mouse_mode()) return;
   if (tt_mouse_acquired) {
      // following was before all the above prior to 160502 
      show_cursor(TRUE);
//      if (release_mouse_too) { // was tt_mouse_mode == RELATIVE_MOUSE_MODE
         // conditional new on 130502 since still respond to mouse after control-f8 in absolute mode
      release_mouse_capture(); // added 011199 to release the mouse
      tt_mouse_acquired = FALSE;
      string new_title = S(IDS_MOUSE_RELEASED,"");
		if (new_title[0] != NULL) push_title(new_title);
#if TT_ENHANCED_DRAG_AND_DROP
		register_drag_drop(tt_main_window->get_handle()); // new on 301204 to accept OLE drag and drop (e.g. from a browser)
#endif
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         log("Toggling exclusive mouse while mouse is in relative mode.");
      };
#endif
   } else {
//	   if (tt_mouse_mode == RELATIVE_MOUSE_MODE) { // conditional new on 130502
         tt_mouse_acquired = TRUE;
//      };
	   show_cursor(FALSE);
//      if (tt_mouse_mode == RELATIVE_MOUSE_MODE) { // conditional new on 130502
		restore_title();
#if TT_DEBUG_ON
		if (tt_debug_mode == 130502) {
			tt_error_file() << "About to capture mouse after control-F8" << endl;
		};
#endif
		capture_mouse(); // added 011199 to grab the mouse anywhere
		SetFocus(get_handle()); // added 311099
#if TT_ENHANCED_DRAG_AND_DROP
		release_drag_drop(tt_main_window->get_handle()); // new on 301204 since no longer available to receive drags
#endif
//	   };
   };
};

void MainWindow::switch_to_full_screen() { // made into a method on 100602
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502 || tt_debug_mode == 020105) {
         log("About to switch to full screen and mouse.");
      };
		if (tt_debug_mode == 51103) {
			tt_error_file() << "switch_to_full_screen where tt_restore_exclusive_screen is " << (int) tt_restore_exclusive_screen 
								 << " and tt_exclusive_screen is " << (int) tt_exclusive_screen << " on frame" << tt_frame_number << endl;
		};
#endif
      restoring_exclusive_screen = TRUE;
//			toggle_pause(FALSE,FALSE,FALSE); // toggle pause twice to restore picture in new mode
//			tt_using_dispdib = 1;
//         tt_restore_exclusive_screen = FALSE; // too early
//			initialize_direct_draw(); 
//			restore_surfaces(); // commented out on 070401 since why do this if just initialized
//			get_exclusive_control(); // commented out on 160502 and replaced with a postponed
      tt_exclusive_screen = TRUE; // so when direct_draw is needed it'll do the right thing
      tt_restore_exclusive_screen = FALSE;
		release_direct_draw(TRUE); // was FALSE prior to 160502 // new on 230800
//         CloseWindow(get_handle());
      // tried to replace the above 2 calls with the following on 160502
//			toggle_pause(FALSE,FALSE,FALSE);
//       restoring_exclusive_screen = FALSE; // postponed until next cycle now
#if TT_DEBUG_ON
      if (tt_debug_mode == 130502) {
         tt_error_file() << "Switching back to full-screen from windowed and took back the mouse." << endl;
         tt_error_file() << "full_screen is " << (int) full_screen << " while tt_exclusive_screen_desired is " << (int) tt_exclusive_screen_desired
								 << " and tt_exclusive_screen is " << (int) tt_exclusive_screen 
								 << " and tt_restore_exclusive_screen is " << (int) tt_restore_exclusive_screen << endl;
      };
#endif
};

void print_guid(LPGUID guid, output_stream &stream) {
	bytes guid_string;
	UuidToString(guid,&guid_string);
	stream << (string) guid_string;
	RpcStringFree(&guid_string);
};

boolean guid_from_string(string guid_string, LPGUID guid) {
  return(SUCCEEDED(UuidFromString((bytes) guid_string,guid)));
};

// from ddutil.cpp sample DirectX code
DWORD DDColorMatch(draw_surface pdds, COLORREF rgb) {
    COLORREF                rgbT;
    HDC                     hdc;
    DWORD                   dw = CLR_INVALID;
    draw_surface_description          ddsd;
    HRESULT                 hres;

    //
    //  Use GDI SetPixel to color match for us
    //
    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
    {
        rgbT = GetPixel(hdc, 0, 0);     // Save current pixel value
        SetPixel(hdc, 0, 0, rgb);       // Set our value
        pdds->ReleaseDC(hdc);
    }
    //
    // Now lock the surface so we can read back the converted color
    //
    ddsd.dwSize = sizeof(ddsd);
    while ((hres = pdds->Lock(NULL, &ddsd, 0, NULL)) == DDERR_WASSTILLDRAWING)
        ;
    if (hres == DD_OK)
    {
        dw = *(DWORD *) ddsd.lpSurface;                 // Get DWORD
        if (ddsd.ddpfPixelFormat.dwRGBBitCount < 32)
            dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount) - 1;  // Mask it to bpp
        pdds->Unlock(NULL);
    }
    //
    //  Now put the color that was there back.
    //
    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
    {
        SetPixel(hdc, 0, 0, rgbT);
        pdds->ReleaseDC(hdc);
    }
    return dw;
}

COLORREF MainWindow::convert_pixel_to_current_format(COLORREF color) { // new on 061101
	// converts to the current device pixel format
	if (color == 0) return(0);
//	HDC screen_device_context = GetDC(NULL); // tt_main_window->get_windows_device_context();
////	COLORREF answer = GetNearestColor(screen_device_context,color);
//	COLORREF saved_pixel = GetPixel(window_device_context,0,0);
//	COLORREF answer = SetPixel(window_device_context,0,0,color);
//	if (saved_pixel != -1) SetPixel(window_device_context,0,0,saved_pixel); // restore
//	ReleaseDC(NULL,screen_device_context);
//	return(answer);
	return(DDColorMatch(back_surface,color));
};

void MainWindow::set_cursor_position(coordinate x, coordinate y) { // new on 290404
	::SetCursorPos(x+front_surface_rectangle.left,front_surface_rectangle.bottom-y);
};

#if TT_DIRECTX_TRANSFORM||TT_GDIPLUS
draw_surface surfaces_from_ini_file(ascii_string ini_file_name, boolean warn, boolean palette_ok, int bits_per_pixel, draw_surface *mask_surface) { 
	// new on 260901
	// removed , bytes color_permutation on 291101
	char entry[MAX_PATH];
	char full_entry[MAX_PATH];
	draw_surface primary_surface = NULL;
	if (mask_surface != NULL) {
		*mask_surface = NULL;
	};
	boolean need_mask_too = FALSE; // unless proven otherwise below
	string full_file_name = picture_file_name_of_ini_file(ini_file_name,entry,full_entry); // result might share with entry or full_entry
	if (full_file_name != NULL) {
		if (file_exists(full_file_name)) { 
			// new on 141201 since on Win 98 a missing file when called by GDI+ puts up a dialog box (even when full screen)
			need_mask_too = (mask_surface != NULL && GetPrivateProfileString("Files","PictureMask","",entry,MAX_PATH,ini_file_name) != 0);
			int transparent_color = need_mask_too?1:0; 
			// 1 means don't bother to figure out if it has a transparent color since it is masked no point wasting the effort
			primary_surface = load_any_image(full_file_name,bits_per_pixel,warn,palette_ok,NULL,&transparent_color); 
			// rewritten 291001 -- ,color_permutation
		} else {
			tt_error_file() << ini_file_name << " mentions " << full_file_name << " but Windows can't find the file." << endl;
			tt_err_file_important = TRUE;
		};
//#if TT_DIRECTX_TRANSFORM
//		if (!tt_prefer_gdiplus) {
//		};
//#endif
//#if TT_GDIPLUS
//		if (tt_prefer_gdiplus) {
//			primary_surface = load_image_using_gdiplus(full_file_name,warn,bits_per_pixel,NULL,color_permutation);
//		};
//#endif
	};
	if (primary_surface == NULL) {
		return(NULL);
	};
	if (need_mask_too) {
		if (is_relative_path(entry)) { // new on 270901 - use path from full_file_name
			int start_of_basic_name = start_of_basic_file_name(ini_file_name);
			if (start_of_basic_name < 0) { // not clear how this can happen
				tt_error_file() << "Couldn't separate the directory and basic file name parts of " << ini_file_name << endl;
				return(NULL);
			};
			memcpy(full_entry,ini_file_name,start_of_basic_name);
			strcpy(full_entry+start_of_basic_name,entry);
			full_file_name = full_entry;
		} else {
			full_file_name = entry;
		};
		if (file_exists(full_file_name)) { // new on 171201 since on Win 98 a missing file when called by GDI+ puts up a dialog box (even when full screen)
			*mask_surface = load_any_image(full_file_name,bits_per_pixel,warn); // bits_per_pixel was tt_prefer_gdiplus?32:24 prior to 021101
		} else {
			tt_error_file() << ini_file_name << " mentions " << full_file_name << " but Windows can't find the file." << endl;
			tt_err_file_important = TRUE;
		};
//#if TT_DIRECTX_TRANSFORM
//		if (!tt_prefer_gdiplus) {
//			*mask_surface = load_image_using_directx_transform(full_file_name,warn,24); // new on 270901 - 8 (or even 1 bit) makes most sense but doesn't seem to load correctly via load_image_using_directx_transform
//		};
//#endif
//#if TT_GDIPLUS
//		if (tt_prefer_gdiplus) {
//			*mask_surface = load_image_using_gdiplus(full_file_name,warn,32);
//		};
//#endif	
	};
	return(primary_surface);
};	 
#endif

#if TT_DIRECTX_TRANSFORM
draw_surface load_image_using_directx_transform(ascii_string full_file_name, wide_string wide_file_name, boolean warn, int bits_per_pixel,
																draw_surface *mask_surface) { // warn added 200201; mask added 270901
/*	const int picture_extensions_count = 6;
	string picture_extenions[picture_extensions_count] = {"bmp","png","jpg","gif","dib","jpeg"};
	if (!already_has_extension(full_file_name,picture_extenions,picture_extensions_count)) { // new on 200201
#if TT_DIRECTX_TRANSFORM
		// for now only works when dx transforms are working
		if (already_has_extension(full_file_name,"ini")) { // new on 260901
			return(surfaces_from_ini_file(full_file_name,warn,bits_per_pixel,NULL,mask_surface));
		};
#endif
		if (warn) {
			tt_error_file() << full_file_name << " isn't a picture since it doesn't have one of the following extensions: 
			// bmp, png, jpg, jpeg, gif, or dib" << endl;
		};
		return(NULL);
	};
	*/
//	transform_surface surface = NULL;
	HRESULT result;
//	wide_string wide_file_name = copy_wide_string(full_file_name);
//	DDSURFACEDESC surface_description;
//	ZeroMemory(&surface_description,sizeof(surface_description));
//	surface_description.dwSize = sizeof(surface_description);
////	result = surface->GetSurfaceDesc(&surface_description);
//	surface_description.dwFlags = DDSD_PIXELFORMAT; //DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
   // easier to debug but typically slower -- but not always???
   // but without DDSCAPS_SYSTEMMEMORY crashed on some systems
   // for these I need to check IsLost and restore
//   surface_description.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
//   surface_description.dwWidth = pixel_width;
//   surface_description.dwHeight = pixel_height;
//	DDPIXELFORMAT pixel_format;
//	ZeroMemory(&pixel_format,sizeof(pixel_format));
//	if (back_surface != NULL) {
//		back_surface->GetPixelFormat(&pixel_format);
//		surface_description.ddpfPixelFormat = pixel_format;
//	};
//	pixel_format.dwSize = sizeof(pixel_format);
//	pixel_format.dwFlags = DDPF_PALETTEINDEXEDTO8;
//	pixel_format.dwRGBBitCount = 8; //bits_per_pixel;
//	IDXSurface *dx_surface;
	if (directx_transform_surface_factory == NULL) {
		create_directx_transform_factory();
		if (directx_transform_surface_factory == NULL) {
			tt_using_directx_transform_or_gdiplus = FALSE;
			return(NULL);
		};
	};
	GUID format_ID;
	switch (bits_per_pixel) {
		case 8:
			format_ID = DDPF_RGB8;
			break;
			// following aren't currently in use
		case 24: 
			format_ID = DDPF_RGB24; // why not use the _CK versions to capture transparent color?
			break;
		case 32:
//			format_ID = DDPF_RGB32;
			format_ID = DDPF_ARGB32;
			break;
		case 16:
			format_ID = DDPF_RGB555;
			break;
		default:
			format_ID = DDPF_RGB24;
	};
	IDXSurface *transform_surface;
#if TT_DEBUG_ON
	millisecond start_time;
	if (tt_debug_mode == 31001) {
		start_time = timeGetTime();
	};
#endif
	result = directx_transform_surface_factory->LoadImage(wide_file_name, direct_draw, NULL, // &surface_description,
																			&format_ID, 
																			IID_IDXSurface, (void**) &transform_surface); 
	// IID_IDirectDrawSurface &DDPF_ARGB32 &DDPF_RGB32 &DDPF_RGB8 ,&DDPF_PMARGB32 
#if TT_DEBUG_ON
	if (tt_debug_mode == 31001) {
		tt_error_file() << (timeGetTime()-start_time)
						<< "ms to TranformFactory::LoadImage " << full_file_name << endl;
		start_time = timeGetTime();
	};
#endif
//	delete [] wide_file_name;
	if (FAILED(result)) {
		if (warn) {
			tt_error_file() << "Failed to load image from file " << full_file_name << " using transform surface factory." << endl;
			tt_error_file() << "Error code = " << result << endl;
			trouble_shoot(TROUBLE_SHOOT_DIRECTX_MEDIA,NULL,NULL,TRUE,TRUE,TRUE); // new on 100600
			switch (result) {
				case DXTERR_ALREADY_INITIALIZED:
					log("Aleady initialized");
					break;
				case DXTERR_COPYRIGHT_IS_INVALID:
					log("Invalid copyright");
					break;
				case DXTERR_INVALID_BOUNDS:
					log("Invalid bounds");
					break;
				case DXTERR_INVALID_FLAGS:
					log("Invalid flags");
					break;
				case DXTERR_OUTOFSTACK:
					log("Out of stack"); // stack??
					break;
				case DXTERR_REQ_IE_DLLNOTFOUND:
					log("IE 4.0 or later not installed?"); // deal with specially?
					break;
				case DXTERR_UNINITIALIZED:
					log("Uninitialized");
					break;
				case DXTERR_UNSUPPORTED_FORMAT:
					log("Unsupported format");
					break;
				case E_FAIL:
					log("Failure.");
					break;
				case E_INVALIDARG:
					log("Argument is invalid.");
					break;
				case E_NOTIMPL:
					log("Method is not supported.");
					break;
				case E_POINTER:
					log("NULL pointer argument");
					break;
				case E_UNEXPECTED:
					log("unexpeced error.");
					break;
			}
		};
		return(NULL);
	} else {
		draw_surface surface = NULL;
		result = transform_surface->GetDirectDrawSurface(IID_IDirectDrawSurface,(void **)&surface); // not sure if this helps at all - 270901
#if TT_DEBUG_ON
		if (tt_debug_mode == 101001) {
			HRESULT result;
			ZeroMemory(&surface_description,sizeof(surface_description));
			surface_description.dwSize = sizeof(surface_description);
			result = surface->GetSurfaceDesc(&surface_description);
			if (surface_description.dwFlags&DDSD_CKSRCBLT) {
				log("has transparent color.");
			};
		};
#endif
		// for debuggin
//		bytes surface_bytes;
//		long pitch;
//		local_lock_surface(surface,surface_bytes,pitch);
//		for (int i = 0; i < pitch; i++) {
//			tt_error_file() << (int) surface_bytes[i] << " ";
//		};
//		unlock_surface(surface);
/*		LPDIRECTDRAWPALETTE surface_palette;
		result = surface->GetPalette(&surface_palette);
		if (FAILED(result)) {
			tt_error_file() << "Failed to get palette of " << full_file_name << endl;
			switch (result) {
				case DDERR_GENERIC:
					log("1");
					break;
				case DDERR_INVALIDOBJECT:
					log("2");
					break;
				case DDERR_INVALIDPARAMS:
					log("3");
					break;
				case DDERR_NOEXCLUSIVEMODE:
					log("4");
					break;
				case DDERR_NOPALETTEATTACHED:
					log("5");
					break;
				case DDERR_SURFACELOST:
					log("6");
					break;
				case DDERR_UNSUPPORTED:
					log("7");
					break;
			};
		} else {
			DWORD pCaps;
			result = surface_palette->GetCaps(&pCaps);
			if (FAILED(result)) {
				tt_error_file() << "Failed to get palette caps of " << full_file_name << endl;
			};
		};
		*/
//		DDCOLORKEY color_key; // was DXSAMPLE
//		result = surface->GetColorKey(DDCKEY_SRCBLT,&color_key);
//		long x = 0;
//		if (FAILED(result)) {
//			x = GetLastError();
//		};
//		result = dx_surface->GetDirectDrawSurface(IID_IDirectDrawSurface,(void**) &surface);
//		bytes surface_bytes;
//		long pitch;
//		local_lock_surface(surface,surface_bytes,pitch);
		return(surface);
	};
};

void fix_directx_transform_surface_palette(draw_surface surface, string full_file_name, boolean palette_ok, int *transparent_color_pointer) {
	if (surface != NULL) {
		int color;
		if (!palette_ok && already_has_extension(full_file_name,"gif")) { // new on 311001
			color = tt_white; // seems to work
		} else {
			color = tt_black; // black is default for directx_transform version
		};
		if (transparent_color_pointer != NULL && *transparent_color_pointer == -1) *transparent_color_pointer = color; 
		set_transparent_color(surface,color);
	};
};
#endif

#if TT_DEBUG_ON
int nearest_palette_index(PALETTEENTRY *pe, int red, int green, int blue) {
	int best_so_far = 0;
	int best_score_so_far = max_long;
	for (int i = 0; i < 256; i++) {
//		int score = (pe[i].peRed-red)*(pe[i].peRed-red)+(pe[i].peGreen-green)*(pe[i].peGreen-green)+(pe[i].peBlue-blue)*(pe[i].peBlue-blue);
		// doesn't matter which one I use
		int score = (default_header->bmiColors[i].rgbRed-red)*(default_header->bmiColors[i].rgbRed-red)+
						 (default_header->bmiColors[i].rgbGreen-green)*(default_header->bmiColors[i].rgbGreen-green)+
						 (default_header->bmiColors[i].rgbBlue-blue)*(default_header->bmiColors[i].rgbBlue-blue);
		if (score < best_score_so_far) {
			best_so_far = i;
			best_score_so_far = score;
		};
	};
	return(best_so_far);
};
#endif

void convert_color32_to_color8(bytes image_bytes32, int pitch, int pixel_width, int pixel_height, bytes image_bytes8, HPALETTE default_palette) {
	const int bytes_per_pixel = 4;
	const int alpha_offset = 3;
	const int red_offset = 2;
	const int green_offset = 1;
	const int blue_offset = 0;
	int width_with_padding = pixel_width;
	if (width_with_padding&3) width_with_padding += 4-width_with_padding%4;
//	COLORREF *image_colors = (COLORREF *) image_bytes32;
	for (int y = 0; y < pixel_height; y++) {
		for (int x = 0; x < pixel_width; x++) {
//			COLORREF color = image_colors[0]&0xFFFFFF; // mask off the alpha channel
//			image_colors++;
			int byte_index = x*bytes_per_pixel; 
			boolean opaque = image_bytes32[byte_index+alpha_offset];
			if (!opaque) {
				image_bytes8[x] = 0;
			} else {
				int current_red_index = byte_index+red_offset; // for current pixel
				int current_green_index = byte_index+green_offset;
				int current_blue_index = byte_index+blue_offset;
				COLORREF color = RGB(image_bytes32[current_red_index],image_bytes32[current_green_index],image_bytes32[current_blue_index]);
				int index = GetNearestPaletteIndex(default_palette,color);		
//#if TT_DEBUG_ON
//				if (tt_debug_mode == 191101) {
//					int my_index = nearest_palette_index(pe,image_bytes32[current_red_index],image_bytes32[current_green_index],
				//													 image_bytes32[current_blue_index]);
//					if (my_index != index) {
//						tt_error_file() << "Windows thinks " << index << " while I figure it should be " << my_index << endl;
//					};
//				};
//#endif
				if (index == 0 && color != 0) {
					image_bytes8[x] = tt_nearest_black;
				} else {
					image_bytes8[x] = index;
					if (tt_dither_to_8bits) { // new on 201101
/*						From http://www.efg2.com/Lab/Library/ImageProcessing/DHALF.TXT
						=====================================
						The Floyd-Steinberg filter

						This is where it all began, with Floyd and Steinberg's [4] pioneering
						research in 1975.  The filter can be diagrammed thus:


									 *   7
								3   5   1     (1/16)


						In this (and all subsequent) filter diagrams, the "*" represents the pixel
						currently being scanning, and the neighboring numbers (called weights)
						represent the portion of the error distributed to the pixel in that
						position.  The expression in parentheses is the divisor used to break up the
						error weights.  In the Floyd-Steinberg filter, each pixel "communicates"
						with 4 "neighbors."  The pixel immediately to the right gets 7/16 of the
						error value, the pixel directly below gets 5/16 of the error, and the
						diagonally adjacent pixels get 3/16 and 1/16. 
*/
						PALETTEENTRY pe[256];
						GetPaletteEntries(default_palette,0,256,pe);
						int red_error = image_bytes32[current_red_index]-pe[index].peRed;
						int green_error = image_bytes32[current_green_index]-pe[index].peGreen;
						int blue_error = image_bytes32[current_blue_index]-pe[index].peBlue;
						if (red_error != 0 || green_error != 0 || blue_error != 0) {
							int red_index, green_index, blue_index; // of neighbors of current point
							if (x < pixel_width-1) { // not at the right edge
								red_index = current_red_index+bytes_per_pixel;
								green_index = current_green_index+bytes_per_pixel;
								blue_index = current_blue_index+bytes_per_pixel;
								if (!(image_bytes32[red_index] == 0 && image_bytes32[green_index] == 0 && image_bytes32[blue_index] == 0)) { 
									// not black neighbor (or should I check transparency?)
									// >>4 should be faster than /16
									image_bytes32[red_index] = max(1,min(255,image_bytes32[red_index]+((7*red_error)>>4))); 
									image_bytes32[green_index] = max(1,min(255,image_bytes32[green_index]+((7*green_error)>>4)));
									image_bytes32[blue_index] = max(1,min(255,image_bytes32[blue_index]+((7*blue_error)>>4)));
								};
							};
							if (y < pixel_height-1) { // not at bottom	
								if (x != 0) { // not at left edge
									red_index = current_red_index-bytes_per_pixel+width_with_padding;
									green_index = current_green_index-bytes_per_pixel+width_with_padding;
									blue_index = current_blue_index-bytes_per_pixel+width_with_padding;
									if (!(image_bytes32[red_index] == 0 && image_bytes32[green_index] == 0 && image_bytes32[blue_index] == 0)) { 
										// don't want to make anything black (i.e. transparent) so use 1 rather than 0 here 
										image_bytes32[red_index] = max(1,min(255,image_bytes32[red_index]+((3*red_error)>>4))); 
										image_bytes32[green_index] = max(1,min(255,image_bytes32[green_index]+((3*green_error)>>4)));
										image_bytes32[blue_index] = max(1,min(255,image_bytes32[blue_index]+((3*blue_error)>>4)));
									};
								};
								red_index = current_red_index+width_with_padding;
								green_index = current_green_index+width_with_padding;
								blue_index = current_blue_index+width_with_padding;
								if (!(image_bytes32[red_index] == 0 && image_bytes32[green_index] == 0 && image_bytes32[blue_index] == 0)) {
									image_bytes32[red_index] = max(1,min(255,image_bytes32[red_index]+((5*red_error)>>4))); 
									image_bytes32[green_index] = max(1,min(255,image_bytes32[green_index]+((5*green_error)>>4)));
									image_bytes32[blue_index] = max(1,min(255,image_bytes32[blue_index]+((5*blue_error)>>4)));
								};		
								if (x < pixel_width-1) { // not at the right edge
									red_index = current_red_index+bytes_per_pixel+width_with_padding;
									green_index = current_green_index+bytes_per_pixel+width_with_padding;
									blue_index = current_blue_index+bytes_per_pixel+width_with_padding;
									if (!(image_bytes32[red_index] == 0 && image_bytes32[green_index] == 0 && image_bytes32[blue_index] == 0)) { 
										image_bytes32[red_index] = max(1,min(255,image_bytes32[red_index]+((red_error)>>4))); 
										image_bytes32[green_index] = max(1,min(255,image_bytes32[green_index]+((green_error)>>4)));
										image_bytes32[blue_index] = max(1,min(255,image_bytes32[blue_index]+((blue_error)>>4)));
									};
								};
							};
						};
					};
//#if TT_DEBUG_ON
//					if (tt_debug_mode == 71101 && index != 0) {
//						tt_error_file() << "rgb of palette: " << (int) pe[index].peRed << " " << (int) pe[index].peGreen << " " 
					//						 << (int) pe[index].peBlue << endl;
//						tt_error_file() << "rgb of image:   " << (int) image_bytes32[byte_index+2] << " " 
					//						 << (int) image_bytes32[byte_index+1] << " " << (int) image_bytes32[byte_index] << endl;
//						tt_error_file() << index << " " << endl;
//					};
//#endif
				};
			};
		};
		image_bytes8 += width_with_padding;
		image_bytes32 += pitch;
#if TT_DEBUG_ON
		if (tt_debug_mode == 71101) {
			tt_error_file() << endl;
		};
#endif
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 71101) {
		tt_error_file() << endl;
	};
#endif
};

bytes convert_color32_to_color8(bytes image_bytes32, int pitch, int pixel_width, int pixel_height) {
	int width_with_padding = pixel_width;
	if (width_with_padding&3) width_with_padding += 4-width_with_padding%4;
	int image_size8 = width_with_padding*pixel_height;
	bytes image_bytes8 = new byte[image_size8];
	HPALETTE default_palette = tt_main_window->return_palette_handle();
	convert_color32_to_color8(image_bytes32,pitch,pixel_width,pixel_height,image_bytes8,default_palette);
	return(image_bytes8);
};


/* following works but color matching is poor and near black becomes black
bytes convert_color32_to_color8(bytes image_bytes32, int pitch, int pixel_width, int pixel_height) {
	// new on 071101 -- could do this by hand but using GDI for now
	// uses the default palette
	const int bytes_per_pixel = 4;
//	int picture_size = 0; // pixel_width*pixel_height*bytes_per_pixel;
//	int dib_size = sizeof(BITMAPINFOHEADER)+picture_size;
	byte dib_bytes[sizeof(BITMAPINFOHEADER)];
	LPBITMAPINFOHEADER dib = (PDIB) dib_bytes;
	ZeroMemory(dib,sizeof(BITMAPINFOHEADER)); // don't need to zero the pixels 
	BITMAPINFOHEADER *dib_header = (BITMAPINFOHEADER *) dib;
	dib_header->biSize = sizeof(BITMAPINFOHEADER);
	dib_header->biWidth = pixel_width;
	dib_header->biHeight = pixel_height; 
	dib_header->biPlanes = 1;
	dib_header->biBitCount = bytes_per_pixel*8;
	dib_header->biCompression = BI_RGB; 
//	dib_header->biSizeImage = picture_size;
//	bytes pixels = (bytes) dib+sizeof(BITMAPINFOHEADER);
//	int row_length = pixel_width*bytes_per_pixel;
//	if (row_length&3) row_length += 4-row_length%4;
//	for (int row = 0; row < pixel_height; row++) {
//		memcpy(pixels,image_bytes32,row_length);
//		if (row < pixel_height-1) {
//			pixels += row_length;
//			image_bytes24 += pitch;
//		};
//	};
//   dib->biHeight *= -1; // needed??
//	memcpy(pixels,image_bytes32,picture_size);
   HDC window_device_context = tt_main_window->get_windows_device_context();
   HBITMAP bitmap_of_dib = CreateDIBitmap(window_device_context,dib,CBM_INIT,
													   image_bytes32, // DIB_bytes(dib),
														(LPBITMAPINFO) dib,
														DIB_RGB_COLORS); 
//	int header_size = dib_header_size;
//   LPBITMAPINFO temp_header = (LPBITMAPINFO) new unsigned char[header_size];
//	BITMAPINFO temp_header;
	byte temp_header_memory[dib_header_size];
	LPBITMAPINFO temp_header = (LPBITMAPINFO) header_memory;
   memcpy(temp_header_memory,default_header,dib_header_size);
	int width_with_padding = pixel_width;
	if (width_with_padding&3) width_with_padding += 4-width_with_padding%4;
   temp_header->bmiHeader.biWidth = pixel_width;
   temp_header->bmiHeader.biHeight = pixel_height;
	int image_size8 = width_with_padding*pixel_height;
   temp_header->bmiHeader.biSizeImage = image_size8;
	temp_header->bmiHeader.biBitCount = 8; 
	temp_header->bmiHeader.biCompression = BI_RGB;
	bytes image_bytes8 = new byte[image_size8];
   int result = GetDIBits(window_device_context,bitmap_of_dib,0,pixel_height,image_bytes8,temp_header,DIB_RGB_COLORS);
   DeleteObject(bitmap_of_dib);
//	delete [] (bytes) dib;
	tt_main_window->release_windows_device_context(window_device_context);
	return(image_bytes8);
};
*/

#if TT_GDIPLUS

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) // copied from Microsoft sample coe
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }
   free(pImageCodecInfo);
   return -1;  // Failure
};

//void convert_alpha_to_black_line(bytes line, int width_in_bytes) { // new on 190303
//	// if alpha is 0 then completely transparent so replace with black which is default transparent color
//	// if any pixels are black already replace with nearly black
//	switch (tt_bits_per_pixel) {
//		case 32:
//		default: // do others later 
//			for (int i = 0; i < width_in_bytes; i += 4) {
//				if (line[i+3] == 0) { // transparent
//					line[i+3] = 255;
//					line[i] = 0; 
//					line[i+1] = 0; 
//					line[i+2] = 0; 
//				} else if (line[i] == 0 && line[i+1] == 0 && line[i+2] == 0) { // black
//					line[i+3] = 255; // make nearly black
//					line[i+1] = 1; 
//					line[i+2] = 1; 
//					line[i+3] = 1; 
//				}; // otherwise leave it alone
//			};
//			break;
//	};
//};
//
//void convert_alpha_to_black(bytes image, int pitch, int width, int height) {
//	int width_in_bytes = width*tt_bytes_per_pixel;
//	for (int y = 0; y < height-1; y++) {
//		convert_alpha_to_black_line(image,width_in_bytes);
//		image += pitch;
//	};
//   // do last one afterwards so debugger doesn't complain about
//   // computing bad addresses on last iteration above
//	convert_alpha_to_black_line(image,width_in_bytes);
//};

CLSID pngClsid;
char temp_png_file[MAX_PATH];
wide_character wide_temp_png_file[MAX_PATH];

void initialize_temp_png() {
	// Get the class identifier for the PNG encoder.
	GetEncoderClsid(L"image/png",&pngClsid);
	memcpy(temp_png_file,tt_private_media_directory,tt_private_media_directory_length);
	strcpy(temp_png_file+tt_private_media_directory_length,"temp.png");
	copy_wide_string(temp_png_file,strlen(temp_png_file),wide_temp_png_file);
};

draw_surface create_draw_surface_from_bitmap(int width, int height, int transparent_color, bytes bitmap_bytes, int bitmap_pitch) {
	draw_surface surface = create_surface(width,height,transparent_color);
	bytes surface_bytes;
	int pitch;
	int ignore1, ignore2, ignore3;
   just_lock_surface(surface,surface_bytes,pitch,ignore1,ignore2,ignore3);
#if TT_DEBUG_ON
	if (tt_debug_mode == 51001) {
		tt_error_file() << "About to copy_work_page." << endl;
	};
#endif
//	if (color_permutation == NULL) {
		copy_work_page(bitmap_bytes,surface_bytes,bitmap_pitch,pitch,width,height); 
//	} else {
//		recolor_work_page(image_bytes,surface_bytes,source_pitch,pitch,
//								width,height,color_permutation,transparent_color);
//	};
	unlock_surface(surface);
//	bitmap.UnlockBits(&bitmapData); -- shouldn't be needed
	return(surface);
};

draw_surface load_image_using_gdiplus(ascii_string full_file_name, wide_string wide_file_name, boolean warn, boolean palette_ok, int bits_per_pixel, 
												  draw_surface *mask_surface, int *transparent_color_pointer, ColorPalette **palette_pointer, 
												  hash image_hash, TTImage *image_object) { // new on 170303
	// removed , bytes color_permutation on 291101
#if TT_DEBUG_ON
	millisecond start_time;
	MEMORYSTATUS memory_status;
	long memory_available_before;
   memory_status.dwLength = sizeof(MEMORYSTATUS);
	if (tt_debug_mode == 31001) {
		start_time = timeGetTime();
	};
	if (tt_debug_mode == 280204) {
		GlobalMemoryStatus(&memory_status);
	   memory_available_before = memory_status.dwAvailPageFile;
	};
	if (tt_debug_mode == 51001) {
		tt_error_file() << "About to load bitmap " << full_file_name << endl;
	};
#endif
	Bitmap bitmap(wide_file_name);
//	delete [] wide_file_name;
#if TT_DEBUG_ON
	if (tt_debug_mode == 31001) {
		tt_error_file() << (timeGetTime()-start_time)
						<< "ms to load image using GDI+ " << full_file_name << endl;
		start_time = timeGetTime();
	};
	if (tt_debug_mode == 280204) {
		GlobalMemoryStatus(&memory_status);
		long memory_available_after = memory_status.dwAvailPageFile;
		tt_error_file() << "load_image_using_gdiplus creating bitmap of " << full_file_name
						<< " and available memory shrank by " << (memory_available_before-memory_available_after) << endl;
		memory_available_before = memory_status.dwAvailPageFile;
	};
#endif
	Status status = bitmap.GetLastStatus();
	if (status != Ok) { // new error handling on 201101
		tt_error_file() << "Error loading " << full_file_name << " with status code = " << (int) status << endl;
		return(NULL);
	};
	draw_surface surface = NULL;
	int width = bitmap.GetWidth();
	int height = bitmap.GetHeight();
	Rect rectangle(0, 0, width, height);
#if TT_DEBUG_ON
	if (tt_debug_mode == 280204) {
		GlobalMemoryStatus(&memory_status);
		long memory_available_after = memory_status.dwAvailPageFile;
		tt_error_file() << "load_image_using_gdiplus creating rectangle " << width << "x" << height
						<< " and available memory shrank by " << (memory_available_before-memory_available_after) << endl;
		memory_available_before = memory_status.dwAvailPageFile;
	};
#endif
	BitmapData bitmapData;
	PixelFormat pixel_format;
	switch (bits_per_pixel) {
		case 8: 
			pixel_format = PixelFormat8bppIndexed;
			break;
		case 16:
			if (tt_green_bits_per_pixel == 5) { // new on 061101
				pixel_format = PixelFormat16bppRGB555;
			} else {
				pixel_format = PixelFormat16bppRGB565;
			};
			break;
		case 24:
			pixel_format = PixelFormat24bppRGB;
			break;
		default:
			pixel_format = PixelFormat32bppARGB;
	};
	int transparent_color; // might be either a color or a color index
	if (transparent_color_pointer == NULL) { // rewritten on 021101
		transparent_color = 0;
	} else {
		transparent_color = *transparent_color_pointer;
	};
	if (!palette_ok) { // was (transparent_color == 0) prior to 221101
     	int size = bitmap.GetPaletteSize(); 
		// new on 261001 -- moved here on 310702 because not usually used and this is VERY EXPENSIVE (17ms on 733 MHz machine)
      if (size > 0) {
		// no need to do this if already know transparent color - new on 021101 -- commented out again on 221101 since breaks color selection feedback
			ColorPalette *palette = (ColorPalette *) new byte[size]; // moved inside conditional on 281101
			bitmap.GetPalette(palette,size);
			int count = palette->Count; 
			for (int i = 0; i < count; i++) {
				unsigned int ARGB = palette->Entries[i];
				if (ARGB>>24 == 0) { // alpha channel is zero so this is transparent
					if (tt_bits_per_pixel == 8) {
						if (transparent_color_pointer != NULL) {
							*transparent_color_pointer = i;
						} else {
//							set_transparent_color_index(surface,i);
						};
						transparent_color = i;
					} else {
						if (tt_bits_per_pixel == 16 || tt_bits_per_pixel == 15) { // new on 061101
							transparent_color = tt_main_window->convert_pixel_to_current_format(ARGB);
						} else {
							transparent_color = ARGB; // should be OK since alpha is 0
						};
						*transparent_color_pointer = transparent_color;
					};
					break;
				};
			};
			*palette_pointer = palette; // new on 281001 -- moved inside conditional on 291101
		};
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 51001) {
		tt_error_file() << "About to lock bits." << endl;
	};
#endif
   status = bitmap.LockBits(&rectangle,ImageLockModeRead,pixel_format,&bitmapData);
	bytes image_bytes = NULL;
	boolean responsible_for_image_bytes = FALSE;
	int source_pitch = 0;
	if (status != Ok) {
		if (status == InvalidParameter && bits_per_pixel == 8) { // new on 071101
			// working around a shortcoming of GDI+ that it can't produce 8-bit version unless original was
			bitmap.UnlockBits(&bitmapData); // make sense???
			status = bitmap.LockBits(&rectangle,ImageLockModeRead,PixelFormat32bppARGB,&bitmapData);
			if (status == Ok) {
#if TT_DEBUG_ON
				if (tt_debug_mode == 31001) {
					start_time = timeGetTime();
				};
#endif
				image_bytes = convert_color32_to_color8((bytes) bitmapData.Scan0,bitmapData.Stride,width,height);
#if TT_DEBUG_ON
				if (tt_debug_mode == 31001) {
					tt_error_file() << (timeGetTime()-start_time)
									<< "ms to convert image to 8-bit colors " << full_file_name << endl;
					start_time = timeGetTime();
				};
#endif
				source_pitch = width;
				if (source_pitch&3) source_pitch += 4-source_pitch%4;
				responsible_for_image_bytes = TRUE;
			};
		};
#if TT_DEBUG_ON
		if (image_bytes == NULL) {
			tt_error_file() << "Locking after loading " << full_file_name << " caused error status = " <<  (int) status << endl;
		};
#endif
//		draw_surface surface = load_image_using_directx_transform(full_file_name,wide_file_name,warn,bits_per_pixel,mask_surface); // new on 061101
//#if TT_DIRECTX_TRANSFORM
//		fix_directx_transform_surface_palette(surface,full_file_name,palette_ok,transparent_color_pointer);
//#endif
//		return(surface);
	} else {
		image_bytes = (bytes) bitmapData.Scan0;
	};
	if (image_bytes == NULL) {
		if (warn) {
			tt_error_file() << full_file_name << " isn't recognized as a picture." << endl;
		};
		bitmap.UnlockBits(&bitmapData); // new on 280204
		return(NULL);
	};
	if (image_hash != NULL) { // new on 170303
		//	if (image_bytes == NULL) {
//		image_bytes = (bytes) bitmapData.Scan0;
		if (bitmap.GetFlags()&ImageFlagsHasAlpha) { // new on 190303 -- don't do this again if already a private media file
			transparent_color = 0; // black is the standard
			*transparent_color_pointer = transparent_color; // communicate back to caller
			if (image_object != NULL) {
				image_object->set_converted_to_black_as_transparency(TRUE);
			};
			status = bitmap.UnlockBits(&bitmapData);
			Bitmap new_bitmap(width,height,PixelFormat32bppARGB);
			int near_black_intensity = 1<<3; // 5th bit in case later run with 15 or 16 bit color - not clear how to handle 8-bit
			//if (tt_bits_per_pixel > 16) {
			//	near_black_intensity = 1;
			//} else if (tt_bits_per_pixel > 8) {
			//	near_black_intensity = 1<<3; // 5th bit
			//}; // else I'm in trouble?
			Color near_black(255,near_black_intensity,near_black_intensity,near_black_intensity);
			Color black(255,0,0,0);
			for (int x = 0; x < width; x++) {
				for (int y = 0; y < height; y++) {
					Color color;
					status = bitmap.GetPixel(x,y,&color);
					if (color.GetAlpha() == 0) { // transparent
						color.SetValue(color.Black);
//						status = new_bitmap.SetPixel(x,y,black);
//						status = bitmap_copy.GetPixel(x,y,&color); // for debuggin
					} else if (color.GetValue() == color.Black) {
						color.SetValue(near_black.GetValue());
//						status = new_bitmap.SetPixel(x,y,near_black);
//						status = bitmap_copy.GetPixel(x,y,&color); // for debuggin
//					} else {
//						status = new_bitmap.SetPixel(x,y,color);
					};
					status = new_bitmap.SetPixel(x,y,color);
				};
			};
			status = new_bitmap.Save(wide_temp_png_file,&pngClsid,NULL);
			if (status == Ok) { // condition new on 101004
				status = new_bitmap.LockBits(&rectangle,ImageLockModeRead,pixel_format,&bitmapData);
				if (status == Ok) {
					surface = create_draw_surface_from_bitmap(width,height,0,(bytes) bitmapData.Scan0,bitmapData.Stride);
					new_bitmap.UnlockBits(&bitmapData); // new on 280204
				} else { // new on 211004 for robustness (see issue 567)
					log("Windows refused access to the pixels of a picture.",FALSE,TRUE);
					tt_error_file() << full_file_name << " and the error code is " << (int) status << endl;
				};
				// following didn't always work right - LockBits sometimes returned InvalidParameters when ImageLockModeWrite used
				//BitmapData bitmapData; // experiment 190303
				//status = bitmap.LockBits(&rectangle,ImageLockModeWrite,PixelFormat32bppARGB,&bitmapData); // going to rewrite it - and always want ARGB
				//convert_alpha_to_black((bytes) bitmapData.Scan0,bitmapData.Stride,width,height);
				//status = bitmap.UnlockBits(&bitmapData); // to write out changes to bitmap
				//status = bitmap.LockBits(&rectangle,ImageLockModeRead,pixel_format,&bitmapData); // and lock it again - is it really used below??
				//image_bytes = (bytes) bitmapData.Scan0;
			};
		} else {	
			status = bitmap.Save(wide_temp_png_file,&pngClsid,NULL);
		};
		if (status != Ok) { // error handling new on 101004
			unable_to_write_file(temp_png_file);
			int error_code = GetLastError();
			if (error_code != 0) {
				output_last_error(error_code,tt_error_file());
			};
			if (status != Win32Error) {
				tt_error_file() << "GDI+ error code is " << (int) status << endl;
			};
			return(NULL);
		} else { // may want to pass parameters instead of NULL later
			string private_file_name = hash_and_copy_file(temp_png_file,".png",image_hash,TRUE,tt_private_media_directory,tt_private_media_directory_length);
			if (private_file_name != NULL) {
				if (image_object != NULL) {
					image_object->reset_full_file_name(private_file_name);
				} else {
					delete [] private_file_name;
				};
			};	
		};
	};
	if (surface == NULL) {
		if (source_pitch == 0) {
			source_pitch = bitmapData.Stride;
		};
#if TT_DEBUG_ON
		if (tt_debug_mode == 280204) {
			GlobalMemoryStatus(&memory_status);
			long memory_available_after = memory_status.dwAvailPageFile;
			tt_error_file() << "everything up to create_draw_surface_from_bitmap"
							<< " and available memory shrank by " << (memory_available_before-memory_available_after) << endl;
			memory_available_before = memory_status.dwAvailPageFile;
		};
#endif
		surface = create_draw_surface_from_bitmap(width,height,transparent_color,image_bytes,source_pitch);
#if TT_DEBUG_ON
		if (tt_debug_mode == 280204) {
			GlobalMemoryStatus(&memory_status);
			long memory_available_after = memory_status.dwAvailPageFile;
			tt_error_file() << "before UnlockBits"
								 << " and available memory shrank by " << (memory_available_before-memory_available_after) << endl;
			memory_available_before = memory_status.dwAvailPageFile;
		};
#endif
		bitmap.UnlockBits(&bitmapData); // new on 280204
#if TT_DEBUG_ON
		if (tt_debug_mode == 280204) {
			GlobalMemoryStatus(&memory_status);
			long memory_available_after = memory_status.dwAvailPageFile;
			tt_error_file() << "after UnlockBits"
								 << " and available memory shrank by " << (memory_available_before-memory_available_after) << endl;
			memory_available_before = memory_status.dwAvailPageFile;
		};
#endif
	};
	if (responsible_for_image_bytes) {
		delete [] image_bytes;
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 31001) {
		tt_error_file() << (timeGetTime()-start_time)
							 << "ms to convert to a DirectDraw surface." << endl;
	};
	if (tt_debug_mode == 51001) {
		tt_error_file() << "Finished copy_work_page." << endl;
	};
#endif
	return(surface);	
};

boolean move_file(string old_name, string new_name) { // new on 310803 since Win98 doesn't support MoveFileEx
	// this could use MoveFileEx if Windows 2000 or above -- noted 071004
	if (!CopyFile(old_name,new_name,FALSE)) {
		unable_to_write_file(new_name); // new on 011004
		return(FALSE);
	};
	return(DeleteFile(old_name));
};

string hash_and_copy_file(string file_name, string new_extension, hash media_hash, boolean delete_original, string directory, int directory_length) {
	long file_size;
	BYTE *file_bytes = (BYTE *) copy_file_or_url_to_string(file_name,file_size,TRUE);
	if (file_bytes == NULL) { // rewritten on 121004 to avoid a spurious warning if the clipboard has a 0-length file on it
		return(NULL);
	};
	DWORD hash_size = hash_byte_length; // initialized as of 010304
	if (MD5_hash(file_bytes,file_size,media_hash,&hash_size)) {
		// added file_bytes != NULL test on 101004 since can happen if unable to access Shared Documents
		// convert image_hash to a file name
		string private_file_name = new char[MAX_PATH];
		string question_mark_in_new_extension = strchr(new_extension,'?'); // new on 301204 since happen if coming from a URL
		if (question_mark_in_new_extension != NULL) {
			question_mark_in_new_extension[0] = '\0'; // terminate
		};
		media_file_name(media_hash,new_extension,private_file_name,directory,directory_length);
		if (question_mark_in_new_extension != NULL) {
			question_mark_in_new_extension[0] = '?'; // restore
		};
		if (delete_original) {
			move_file(file_name,private_file_name);
		} else if (!file_exists(private_file_name,FALSE)) { 
			// conditional new on 211104 avoids confusing warnings about inability to write file when file is already there
			if (!copy_string_to_file((string) file_bytes,file_size,private_file_name)) { // rewritten on 290105 to avoid reading the file twice
			//boolean ignore;
			//if (!copy_file_or_url_to_file(file_name,private_file_name,ignore)) { // was CopyFile prior to 181204
				unable_to_write_file(private_file_name); // new on 011004
			};
		};
		delete [] file_bytes; // new on 290105 -- plugging a serious hole when logging pastes or drags of files
		return(private_file_name);
	} else {
		// warning new on 010903
		tt_error_file() << "Failed to create hash name for file so support for media files will be impaired. Please report this to support@toontalk.com" << endl;
		if (file_name != NULL) {
			tt_error_file() << "File name is " << file_name << endl;
		};
		return(NULL);
	};
};

// loosely based upon http://support.microsoft.com/default.aspx?scid=kb;en-us;Q315780 
Status save_gif_with_palette(Bitmap &old_bitmap, const WCHAR* filename, const CLSID* clsidEncoder, DWORD color_count, BOOL transparent) {
    Status status = GenericError;
    // GIF codec supports 256 colors maximum.
    if (color_count > 256)
        color_count = 256;
    if (color_count < 2)
        color_count = 2;
    // Make a new 8-BPP pixel indexed bitmap that is the same size as the source image.
    DWORD   width = old_bitmap.GetWidth();
    DWORD   height = old_bitmap.GetHeight();
    // Always use PixelFormat8BPPIndexed, because that is the color table
    // based interface to the GIF codec.
    Bitmap  new_bitmap(width, height, PixelFormat8bppIndexed); 
    status = new_bitmap.GetLastStatus();
    if (status != Ok)
        return status;        // No point in continuing.
    // Allocate the new color table.
    DWORD   dwSizeColorPalette;
    dwSizeColorPalette = sizeof(ColorPalette);      // Size of core structure.
    dwSizeColorPalette += sizeof(ARGB)*(color_count-1);   // The other entries.

    // Allocate some raw space to back the ColorPalette structure pointer.
    ColorPalette *palette = (ColorPalette *)new BYTE[dwSizeColorPalette];
    if (palette == NULL) return OutOfMemory;
    ZeroMemory(palette, dwSizeColorPalette);
    // Initialize a new color table with entries that are determined by
    // some optimal palette finding algorithm; 
	 // for now just use the ToonTalk palette
	 if (transparent) {
        palette->Flags = PaletteFlagsHasAlpha;
	 } else {
        palette->Flags = 0;
	 };
    palette->Count = color_count;
//	 HPALETTE default_palette = tt_main_window->return_palette_handle();
//	 PALETTEENTRY pe[256];
//	 GetPaletteEntries(default_palette,0,256,pe);
	 BitmapData  new_bitmap_data, old_bitmap_data;;
     Rect        rectangle(0, 0, width, height);
	 if (status == Ok) {
		 status = old_bitmap.LockBits(&rectangle,ImageLockModeRead,PixelFormat32bppARGB,&old_bitmap_data);
	 };
	 CQuantizer q(color_count,8);
	 bytes Old_image = (bytes) old_bitmap_data.Scan0;
	 unsigned int width_in_pixels = width*4;
	 for (unsigned int i = 0; i < height; i++) {
		 for (unsigned int j = 0; j < width_in_pixels; j += 4) { // was i rather than j prior to 200503 and was ++
			 q.ProcessPixel(Old_image[j+2],Old_image[j+1],Old_image[j]); // since is ARGB and the byte order is reversed
		 };
		 Old_image += old_bitmap_data.Stride;
	 };
	 RGBQUAD RGB_palette[256];
	 q.GetColorTable(RGB_palette);
	 HPALETTE palette_handle = CreateIdentityPalette(RGB_palette,color_count);
    for (UINT i = 0; i < color_count; i++) {
        int Alpha = 0xFF;       // Colors are opaque by default.
        // The GIF encoder makes the first entry in the palette with a
        // zero alpha the "transparent" color in the GIF.
        // assumes the first entry is black
		  if ( i == 0 && transparent) {// Make this color index...
            Alpha = 0;          // Transparent
		  };
        palette->Entries[i] = Color::MakeARGB(Alpha,RGB_palette[i].rgbRed,RGB_palette[i].rgbGreen,RGB_palette[i].rgbBlue);
    }
    // Set the palette into the new Bitmap object.
    new_bitmap.SetPalette(palette);
    // Lock the whole of the bitmap for writing.
    status = new_bitmap.LockBits(&rectangle,ImageLockModeWrite,PixelFormat8bppIndexed,&new_bitmap_data);
    if (status == Ok) {
        // Write to the temporary buffer provided by LockBits.
        // Copy the pixels from the source image in this loop.
        // Because you want an index, convert RGB to the appropriate
        // palette index here.
        BYTE *pixels;
		  if (new_bitmap_data.Stride > 0) {
            pixels = (BYTE*) new_bitmap_data.Scan0;
		  } else {
            // If the Stride is negative, Scan0 points to the last             // scanline in the buffer.
            // To normalize the loop, obtain the start of the buffer,
            // which is located (Height-1) scanlines previous.
            pixels = (BYTE*) new_bitmap_data.Scan0 + new_bitmap_data.Stride*(height-1);
		  };
		  convert_color32_to_color8((bytes) old_bitmap_data.Scan0,abs(old_bitmap_data.Stride),width, height,pixels,palette_handle);    
        // To commit the changes, unlock the portion of the bitmap.  
        status = new_bitmap.UnlockBits(&new_bitmap_data);
    };
    // See whether the code has been successful to this point.
    if (status == Ok) {
		 // Write it out to the disk.
       status = new_bitmap.Save(filename, clsidEncoder, NULL);
		 if (status != Ok) { // error handling new on 101004
			 string narrow_file_name = copy_narrow_string((wide_string) filename);
			 unable_to_write_file(narrow_file_name);
			 delete [] narrow_file_name;
			 int error_code = GetLastError();
			 if (error_code != 0) {
				 output_last_error(error_code,tt_error_file());
			 };
			 if (status != Win32Error) {
				 tt_error_file() << "GDI+ error code is " << (int) status << endl;
			 };
		 };
    };
    // Clean up after yourself.
	 Status ignore_status = old_bitmap.UnlockBits(&old_bitmap_data);
    delete [] (LPBYTE) palette;
    palette = NULL;
    return status;
};

void save_picture_file_as_png(string file_name, string file_name2, boolean transparent, string png_file_name) {
	// beginning 151003 if transparent then file_name2 differs from file_name only in its transparent pixels
	boolean delete_file_name = FALSE; // new on 110204
	boolean delete_file_name2 = FALSE;
	{ // new on 110204 since DeleteFile finds files still held on to by Bitmap 
	wide_character wide_file_name[MAX_PATH];
	copy_wide_string(file_name,strlen(file_name),wide_file_name);
	Bitmap bitmap(wide_file_name);
	wide_character wide_png_file_name[MAX_PATH];
	copy_wide_string(png_file_name,strlen(png_file_name),wide_png_file_name);
	if (transparent) {
		wide_character wide_file_name2[MAX_PATH]; // new on 15103
		copy_wide_string(file_name2,strlen(file_name2),wide_file_name2);
		Bitmap bitmap2(wide_file_name2);
		DWORD width = bitmap.GetWidth();
		DWORD height = bitmap.GetHeight();
		Bitmap new_bitmap(width,height,PixelFormat32bppARGB);
		Color color, color2;
		DWORD transparent = Color::MakeARGB(0,255,255,255); 
		// if picture editor can't handle transparency then white is better than black - changed on 15103
//		DWORD transparent = Color::MakeARGB(0,0,0,0);
		for (DWORD y = 0; y < height; y++) {
			for (DWORD x = 0; x < width; x++) {
				bitmap.GetPixel(x,y,&color);
				bitmap2.GetPixel(x,y,&color2); // new on 15103
				if (color.GetValue() != color2.GetValue()) {
//				if (color.GetValue() == color.Black) {
					color.SetValue(transparent);
				};
				new_bitmap.SetPixel(x,y,color);
			};
		};
		Status status = new_bitmap.Save(wide_png_file_name,&pngClsid,NULL);
		if (status == Ok) {
			delete_file_name = TRUE;
			delete_file_name2 = TRUE;
			//DeleteFile(file_name); // don't need original anymore
			//DeleteFile(file_name2); // new on 15103
		} else { // error handling new on 101004
			 unable_to_write_file(png_file_name);
			 int error_code = GetLastError();
			 if (error_code != 0) {
				 output_last_error(error_code,tt_error_file());
			 };
			 if (status != Win32Error) {
				 tt_error_file() << "GDI+ error code is " << (int) status << endl;
			 };
		};
	} else {
		Status status = bitmap.Save(wide_png_file_name,&pngClsid,NULL);
		if (status == Ok) {
			delete_file_name = TRUE;
//			DeleteFile(file_name); // don't need original anymore
		} else { // error handling new on 101004
			unable_to_write_file(png_file_name); 
			 int error_code = GetLastError();
			 if (error_code != 0) {
				 output_last_error(error_code,tt_error_file());
			 };
			 if (status != Win32Error) {
				 tt_error_file() << "GDI+ error code is " << (int) status << endl;
			 };
		};
	};
	};
	// doing the following after the bitmaps are out of scope
	if (delete_file_name) {
		DeleteFile(file_name); 
	}
	if (delete_file_name2) {
		DeleteFile(file_name2); 
	};
};

string save_private_media_picture_file_as_gif(string private_media_file_name, boolean transparent, string directory) {
	// new on 201103 - this differs from save_picture_file_as_gif the PNG unique name is being reused
	// turns out that if I save the same image twice as a GIF I get different bytes so it leads to copies of GIF files
	// can even break the Java applet since the identity of picture matching breaks
	if (private_media_file_name == NULL) { 
		tt_error_file() << "Couldn't find picture file to turn into a GIF." << endl;
		return(NULL); //this happened to Ylva - maybe image hadn't been initialized
	};
	CLSID gifClsid;
	GetEncoderClsid(L"image/gif",&gifClsid);
	string gif_file_name = new char[MAX_PATH];
	int gif_file_name_length = strlen(directory);
	memcpy(gif_file_name,directory,gif_file_name_length);
	int hash_name_start = short_file_name_start(private_media_file_name);
	memcpy(gif_file_name+gif_file_name_length,private_media_file_name+hash_name_start,hash_string_length);
	gif_file_name_length += hash_string_length;
	memcpy(gif_file_name+gif_file_name_length,".gif",5); // 4 plus terminating NULL
	gif_file_name_length += 4;
	wide_character wide_file_name[MAX_PATH];
	copy_wide_string(private_media_file_name,strlen(private_media_file_name),wide_file_name);
	Bitmap bitmap(wide_file_name);
	copy_wide_string(gif_file_name,gif_file_name_length,wide_file_name);
	Status status = save_gif_with_palette(bitmap,wide_file_name,&gifClsid,256,transparent);
	if (status == Ok) {
		return(gif_file_name);
	} else {
		tt_error_file() << "Error making a GIF version of " << private_media_file_name << " error code is " << (int) status << endl;
		return(NULL);
	};
};

string save_picture_file_as_gif(string file_name, boolean transparent, string directory_or_file_name) {
	if (file_name == NULL) { // new on 250303
		tt_error_file() << "Couldn't find picture file to turn into a GIF." << endl;
		return(NULL); //this happened to Ylva - maybe image hadn't been initialized
	};
	char temp_gif_file[MAX_PATH];
	boolean already_gif = already_has_extension(file_name,"gif");
	if (already_gif) {
		strcpy(temp_gif_file,file_name); // just copy the file
	} else {
		CLSID gifClsid;
		GetEncoderClsid(L"image/gif",&gifClsid);
		wide_character wide_file_name[MAX_PATH];
		copy_wide_string(file_name,strlen(file_name),wide_file_name);
		Bitmap bitmap(wide_file_name);
		boolean is_gif_file_name = already_has_extension(directory_or_file_name,"gif");
		if (is_gif_file_name) { // full-path provided
			strcpy(temp_gif_file,directory_or_file_name);
		} else {
			strcpy(temp_gif_file,directory_or_file_name);
			strcat(temp_gif_file,"temp.gif");
		};
		wide_character wide_temp_gif_file[MAX_PATH];
		copy_wide_string(temp_gif_file,strlen(temp_gif_file),wide_temp_gif_file);
		// should deal with transparency if not rectangular
//		Status status = bitmap.Save(wide_temp_gif_file,&pngClsid,NULL);
		Status status = save_gif_with_palette(bitmap,wide_temp_gif_file,&gifClsid,256,transparent);
		if (is_gif_file_name) {
			return(directory_or_file_name);
		};
	};
	byte image_hash[hash_byte_length];
	return(hash_and_copy_file(temp_gif_file,".gif",image_hash,!already_gif,directory_or_file_name,strlen(directory_or_file_name))); 
};

boolean load_image_dimensions_using_gdiplus(wide_string wide_file_name, int &surface_width, int &surface_height) {
#if TT_DEBUG_ON
	millisecond start_time;
	if (tt_debug_mode == 31001) {
		start_time = timeGetTime();
	};
	if (tt_debug_mode == 51001) {
		tt_error_file() << "About to load bitmap dimensions "<< endl;
	};
#endif
	Bitmap bitmap(wide_file_name);
#if TT_DEBUG_ON
	if (tt_debug_mode == 31001) {
		tt_error_file() << (timeGetTime()-start_time)
						<< "ms to load image using GDI+ " << endl;
	};
#endif
	Status status = bitmap.GetLastStatus();
	if (status != Ok) { // new error handling on 201101
		tt_error_file() << "Error loading ";
		string file_name = copy_narrow_string(wide_file_name);
		tt_error_file() << file_name << " with status code = " << (int) status << " to obtain dimensions" << endl;
		delete [] file_name;
		return(FALSE);
	};
	surface_width = bitmap.GetWidth();
	surface_height = bitmap.GetHeight();
#endif
	return(TRUE);
};

draw_surface load_any_image(ascii_string full_file_name, int bits_per_pixel, boolean warn, boolean palette_ok, 
									 draw_surface *mask_surface_pointer, int *transparent_color_pointer, 
									 hash image_hash, TTImage *image_object) { 
	// removed , bytes color_permutation on 291101
	if (full_file_name == NULL) {
		return(NULL); // warn too?
	};
	if (already_has_extension(full_file_name,"ini",FALSE)) { 
		// what about transparent_color_pointer?? -- added FALSE on 021101 since links don't need to be checked
		return(surfaces_from_ini_file(full_file_name,warn,palette_ok,bits_per_pixel,mask_surface_pointer)); // ,color_permutation
	};
	if (mask_surface_pointer != NULL) {
		*mask_surface_pointer = NULL;
	};
	wide_string wide_file_name = copy_wide_string(full_file_name);
	draw_surface surface = NULL;
	ColorPalette *palette = NULL;
	int transparent_color = 0;
	boolean already_know_transparent_color = FALSE;
	if (transparent_color_pointer == NULL) { // new on 291001 bits_per_pixel == 8 && 
		transparent_color_pointer = &transparent_color;
	} else { 
		transparent_color = *transparent_color_pointer; // new on 021101
		already_know_transparent_color = (transparent_color != 0);
	};
#if TT_GDIPLUS
	if (tt_prefer_gdiplus) {
#if TT_DEBUG_ON
		MEMORYSTATUS memory_status;
		long memory_available_before;
		memory_status.dwLength = sizeof(MEMORYSTATUS);
		if (tt_debug_mode == 280204) {
			GlobalMemoryStatus(&memory_status);
			memory_available_before = memory_status.dwAvailPageFile;
		};
#endif
		surface = load_image_using_gdiplus(full_file_name,wide_file_name,warn,palette_ok,bits_per_pixel,mask_surface_pointer,
													  transparent_color_pointer,&palette,image_hash,image_object); // ,palette_ok?color_permutation:NULL
#if TT_DEBUG_ON
		if (tt_debug_mode == 280204) {
			GlobalMemoryStatus(&memory_status);
			long memory_available_after = memory_status.dwAvailPageFile;
			tt_error_file() << "after call load_image_using_gdiplus "
							<< " and available memory shrank by " << (memory_available_before-memory_available_after) << endl;
		};
#endif
	};
#endif
#if TT_DIRECTX_TRANSFORM
	if (!tt_prefer_gdiplus) { // could have (surface == NULL) instead and then also run load_image_using_gdiplus if this returns NULL 
		// but are there many failure modes that only affect one of these APIs??
		surface = load_image_using_directx_transform(full_file_name,wide_file_name,warn,bits_per_pixel,mask_surface_pointer);
		fix_directx_transform_surface_palette(surface,full_file_name,palette_ok,transparent_color_pointer);
	};
#endif
	if (surface == NULL) return(NULL); // new on 311001
	const int number_of_colors = 256; // really maximum number of colors
	if (bits_per_pixel == 8 && !palette_ok) { // && !already_know_transparent_color) { 
		// new on 291001 to remap pixels for default palette
		// added !already_know_transparent_color since no need to compute this if already have transparent color (or don't care) -- 
		// removed on 221101 since messes up user images when selected
		byte palette_permutation[number_of_colors];
		HPALETTE default_palette = tt_main_window->return_palette_handle();
#if TT_DEBUG_ON
		PALETTEENTRY pe[number_of_colors];
		GetPaletteEntries(default_palette,0,number_of_colors,pe);
		if (tt_debug_mode == 291001) {
			tt_error_file() << full_file_name << endl;
		};
#endif
		boolean succeeded = TRUE;
		if (palette != NULL) {
			for (UINT i = 0; i < palette->Count; i++) {
				UINT index = GetNearestPaletteIndex(default_palette,
																RGB((palette->Entries[i]>>16)&0xFF,(palette->Entries[i]>>8)&0xFF,palette->Entries[i]&0xFF));
#if TT_DEBUG_ON
				if (tt_debug_mode == 291001) {
					tt_error_file() << "default " << i << " = " << (int) pe[i].peRed << " " << (int) pe[i].peGreen << " " << (int) pe[i].peBlue << endl;
				   tt_error_file() << "surface " << i << " = " << (int) ((palette->Entries[i]>>16)&0xFF) << " "  
										 << (int) ((palette->Entries[i]>>8)&0xFF) << " " << (int) (palette->Entries[i]&0xFF) << endl;
					tt_error_file() << index << " " << endl;
				};
#endif
				if (index == CLR_INVALID) { // failed
					break;
				};
				palette_permutation[i] = (byte) index;
			};
			succeeded = (palette->Count > 0); // new on 061101
			delete palette;
			palette = NULL;
		} else {
			LPDIRECTDRAWPALETTE surface_palette;
			if (SUCCEEDED(surface->GetPalette(&surface_palette))) {
				PALETTEENTRY entries[number_of_colors];
				HRESULT result = surface_palette->GetEntries(0,0,number_of_colors,entries);
				if (SUCCEEDED(result)) {
					for (int i = 0; i < number_of_colors; i++) {
						UINT index = GetNearestPaletteIndex(default_palette,RGB(entries[i].peRed,entries[i].peGreen,entries[i].peBlue));
#if TT_DEBUG_ON
						if (tt_debug_mode == 291001) {
							tt_error_file() << "default " << i << " = " 
												 << (int) pe[i].peRed << " " << (int) pe[i].peGreen << " " << (int) pe[i].peBlue << endl;
							tt_error_file() << "surface " << i << " = " 
												 << (int) entries[i].peRed << " " << (int) entries[i].peGreen << " " << (int) entries[i].peBlue << endl;
							tt_error_file() << index << " " << endl;
						};
#endif
						if (index == CLR_INVALID) { // failed
							break;
						};
						palette_permutation[i] = (byte) index;
					};
				} else {
					tt_error_file() << "Failed to entries of a palette. Colors may be wrong." << endl;
					succeeded = FALSE;
				};
			} else {
				tt_error_file() << "Failed to get palette of surface. Colors may be wrong. " << endl;
				succeeded = FALSE;
			};
		};
		if (succeeded) {
			int transparent_color = 0; // does this still make sense??
			if (transparent_color_pointer != NULL) {
				transparent_color = *transparent_color_pointer;
				palette_permutation[transparent_color] = transparent_color;
				set_transparent_color_index(surface,transparent_color);
			};
			bytes surface_bytes;
			int pitch, ignore_bits_per_pixel, width, height;
			just_lock_surface(surface,surface_bytes,pitch,ignore_bits_per_pixel,width,height);
			permute_work_page(surface_bytes,surface_bytes,pitch,pitch,width,height,palette_permutation,transparent_color);
			unlock_surface(surface);
		};
	} else if (palette != NULL) { // new on 281101 but why generate it if not needed? - just to find the transparent color?
		delete palette;
	};
	delete [] wide_file_name;
//	if (color_permutation != NULL && (!palette_ok || !tt_prefer_gdiplus)) { // need to recolor now
//		bytes surface_bytes;
//		int pitch, ignore_bits_per_pixel, width, height;
//		just_lock_surface(surface,surface_bytes,pitch,ignore_bits_per_pixel,width,height);
//		recolor_work_page(surface_bytes,surface_bytes,pitch,pitch,width,height,color_permutation,*transparent_color_pointer); 
	// was transparent_color prior to 051101
//		unlock_surface(surface);
//	};
	return(surface);
};

boolean load_image_dimensions(ascii_string full_file_name, int &surface_width, int &surface_height) {
	if (already_has_extension(full_file_name,"ini",FALSE)) { // new on 071201
		char entry[MAX_PATH];
		char full_entry[MAX_PATH];
		string picture_file_name = picture_file_name_of_ini_file(full_file_name,entry,full_entry);
		if (picture_file_name != NULL) {
			if (file_exists(picture_file_name)) { 
				// new on 141201 since on Win 98 a missing file when called by GDI+ puts up a dialog box (even when full screen)
				return(load_image_dimensions(picture_file_name,surface_width,surface_height));
			} else {
				tt_error_file() << full_file_name << " mentions " << picture_file_name << " but Windows can't find the file." << endl;
				tt_err_file_important = TRUE;
			};
		} else {
			tt_error_file() << "Couldn't find the Picture value in " << full_file_name << endl;
			return(FALSE);
		};
	};
	wide_string wide_file_name = copy_wide_string(full_file_name);
	boolean file_read = FALSE;
#if TT_GDIPLUS
	if (tt_prefer_gdiplus) {
		file_read = load_image_dimensions_using_gdiplus(wide_file_name,surface_width,surface_height);
	};
#endif
#if TT_DIRECTX_TRANSFORM
	if (!tt_prefer_gdiplus) { 
		draw_surface surface = load_image_using_directx_transform(full_file_name,wide_file_name,TRUE,8,NULL);
		if (surface != NULL) {
			int bits_per_pixel; // bits_per_pixel new on 230901
			surface_size(surface,surface_width,surface_height,bits_per_pixel);
			surface->Release(); // done with it
			file_read = TRUE;
		};
	};
#endif
	delete [] wide_file_name;
	return(file_read);
};

#if TT_DEBUG_ON
millisecond total_recoloring = 0;
#endif

draw_surface surface_recolored(draw_surface original_surface, bytes color_permutation, color transparent_color) {
	// can add a cache to this 
	// it would be a function of first two args and its size would be limited to the number of color permutations
#if TT_DEBUG_ON
	millisecond start_time;
	if (tt_debug_mode == 31001) {
		start_time = timeGetTime();
	};
   if (tt_debug_mode == 90503) {
      tt_error_file() << "Bits per pixel: " << tt_bits_per_pixel << endl;
   };
#endif
	bytes original_surface_bytes, recolored_bytes;
	int pitch, ignore_bits_per_pixel, width, height;
	just_lock_surface(original_surface,original_surface_bytes,pitch,ignore_bits_per_pixel,width,height);
#if TT_DEBUG_ON
   if (tt_debug_mode == 90503) {
      tt_error_file() << "Original's bits per pixel: " << ignore_bits_per_pixel << endl;
   };
#endif
	draw_surface recolored_surface = create_surface(width,height,transparent_color);
	just_lock_surface(recolored_surface,recolored_bytes,pitch,ignore_bits_per_pixel,width,height);
#if TT_DEBUG_ON
   if (tt_debug_mode == 90503) {
      tt_error_file() << "Recolored's bits per pixel: " << ignore_bits_per_pixel << endl;
   };
#endif
	recolor_work_page(original_surface_bytes,recolored_bytes,pitch,pitch,width,height,color_permutation,transparent_color);
#if TT_DEBUG_ON
   if (tt_debug_mode == 90503) {
      tt_error_file() << "Finished recoloring." << endl;
   };
#endif
	unlock_surface(original_surface);
	unlock_surface(recolored_surface);
#if TT_DEBUG_ON
	if (tt_debug_mode == 31001) {
		tt_error_file() << (timeGetTime()-start_time)
						<< "ms to create surface_recolored" << endl;
      total_recoloring += (timeGetTime()-start_time);
      tt_error_file() << "Total recoloring time is " << total_recoloring << endl;
	};
#endif
	return(recolored_surface);
};

boolean surface_size(draw_surface surface, int &surface_width, int &surface_height, int &bits_per_pixel) {
	HRESULT result;
	draw_surface_description surface_description;
	ZeroMemory(&surface_description,sizeof(surface_description));
	surface_description.dwSize = sizeof(surface_description);
	result = surface->GetSurfaceDesc(&surface_description);
	if (FAILED(result)) {
		tt_error_file() << "Failed to get the surface description." << endl;
		return(FALSE);
	};
	surface_width = surface_description.dwWidth;
	surface_height = surface_description.dwHeight;
	if (surface_description.dwFlags&DDSD_PIXELFORMAT) { // new on 230901 
		bits_per_pixel = surface_description.ddpfPixelFormat.dwRGBBitCount;
	} else { //  can it be otherwise?
		bits_per_pixel = 8;
	};
	return(TRUE);
};

boolean initialize_ole() { // abstracted and updated 311204 so call OleInitialize since needed for drag and drop
	if (!tt_coinitialized) {
		HRESULT status = ::OleInitialize(NULL);
		if (status == S_OK) {
			tt_coinitialized = TRUE;
			return(TRUE);
		};
		tt_error_file() << "Could not initialize OLE. Some features won't work such as speech and drag and drop. Sorry." << endl;
		return(FALSE);
	};
	return(TRUE);
};

/*
// this doesn't work all that much better than the above
void copy_screen_to_clipboard() {
	HGLOBAL copy_handle
		= global_alloc(GHND,tt_screen_bytes_size*3+sizeof(BITMAPINFOHEADER));
	work_page copy = (work_page) GlobalLock(copy_handle);
	BITMAPINFOHEADER *copy_header = (BITMAPINFOHEADER *) copy;
	memcpy(copy,default_header,sizeof(BITMAPINFOHEADER));
	copy_header->biBitCount = 24;
	long j = sizeof(BITMAPINFOHEADER);
	int palette_index;
	for (long i = 0; i < tt_screen_bytes_size; i++) {
//		if (tt_using_dispdib) {
			palette_index = tt_memory_graphics_work_page[i];
//		} else {
//			palette_index = reverse_color_permutation[tt_memory_graphics_work_page[i]];
//		};
		copy[j++] = default_header->bmiColors[palette_index].rgbBlue;
		copy[j++] = default_header->bmiColors[palette_index].rgbGreen;
		copy[j++] = default_header->bmiColors[palette_index].rgbRed;
	};
	GlobalUnlock(copy_handle);
	if (OpenClipboard(tt_main_window->get_handle())) {
		EmptyClipboard();
		SetClipboardData(CF_DIB,copy_handle);
		CloseClipboard();
	} else {
		play_sound(SYSTEM_ERROR_SOUND);
	};
};

void MainWindow::create_identity_palette(HDC visible_device_context) {
	// many thanks to "Herman Rodent" at Microsoft for the sample code that
	// the following palette hacking is based upon
	logical_palette->palVersion = 0x300;
	logical_palette->palNumEntries = 256;
	for (int n = 0; n<256; n++) { // 256 color palettes only
		unsigned char red = default_header->bmiColors[n].rgbRed,
						  green = default_header->bmiColors[n].rgbGreen,
						  blue = default_header->bmiColors[n].rgbBlue;
		logical_palette->palPalEntry[n].peRed = red;
		logical_palette->palPalEntry[n].peGreen = green;
		logical_palette->palPalEntry[n].peBlue = blue;
		logical_palette->palPalEntry[n].peFlags = 0;
	};
	palette_handle = CreatePalette(logical_palette);
};

typedef struct pal
{
	WORD Version;
	WORD NumberOfEntries;
	PALETTEENTRY aEntries[256];
} pal;

pal LogicalPalette =
{
	0x300,
	256
};

// yet another way to create an identity palette

typedef struct header
{
	BITMAPINFOHEADER Header;
	RGBQUAD aColors[256];
} header;

header BufferHeader;

void MainWindow::create_identity_palette(HDC visible_device_context) {
	// based upon WinG's samples\doggie
	//  create an identity palette from the DIB's color table
	//  get the 20 system colors as PALETTEENTRIES
 //	HDC Screen = GetDC(0);
	GetSystemPaletteEntries(visible_device_context,0,10,LogicalPalette.aEntries);
	GetSystemPaletteEntries(visible_device_context,246,10,LogicalPalette.aEntries + 246);
//	ReleaseDC(0,Screen);
	// initialize the logical palette and DIB color table
	for (int Counter = 0;Counter < 10;Counter++) {
		// copy the system colors into the DIB header
		// WinG will do this in WinGRecommendDIBFormat,
		// but it may have failed above so do it here anyway
		BufferHeader.aColors[Counter].rgbRed =
									LogicalPalette.aEntries[Counter].peRed;
		BufferHeader.aColors[Counter].rgbGreen =
									LogicalPalette.aEntries[Counter].peGreen;
		BufferHeader.aColors[Counter].rgbBlue =
									LogicalPalette.aEntries[Counter].peBlue;
		BufferHeader.aColors[Counter].rgbReserved = 0;
		LogicalPalette.aEntries[Counter].peFlags = 0;
		BufferHeader.aColors[Counter + 246].rgbRed =
								LogicalPalette.aEntries[Counter + 246].peRed;
		BufferHeader.aColors[Counter + 246].rgbGreen =
								LogicalPalette.aEntries[Counter + 246].peGreen;
		BufferHeader.aColors[Counter + 246].rgbBlue =
								LogicalPalette.aEntries[Counter + 246].peBlue;
		BufferHeader.aColors[Counter + 246].rgbReserved = 0;
		LogicalPalette.aEntries[Counter + 246].peFlags = 0;
	};
	RGBQUAD far *pColorTable = default_header->bmiColors;
	for (Counter = 10;Counter < 246;Counter++) {
		// copy from the original color table to the WinGBitmap's
		// color table and the logical palette
		BufferHeader.aColors[Counter].rgbRed =
						LogicalPalette.aEntries[Counter].peRed =
							pColorTable[Counter].rgbRed;
		BufferHeader.aColors[Counter].rgbGreen =
						LogicalPalette.aEntries[Counter].peGreen =
							pColorTable[Counter].rgbGreen;
		BufferHeader.aColors[Counter].rgbBlue =
						LogicalPalette.aEntries[Counter].peBlue =
							pColorTable[Counter].rgbBlue;
		BufferHeader.aColors[Counter].rgbReserved = 0;
		LogicalPalette.aEntries[Counter].peFlags = PC_NOCOLLAPSE;
	};
	palette_handle = CreatePalette((LOGPALETTE far *)&LogicalPalette);
};
*/
/*
#if TT_BETA
	 if (command_line_size == 0) {	 
	 // for now since Bounds Checker doesn't let me provide the command line
	 // and BC4 for 32bit apps
		 ifstream command_line;
		 command_line.open("ttw.cmd",std::ios_base::in);
		 while (command_line) {
			 command_line >> temp;
			 int temp_length = strlen(temp);
			 if (temp_length == 0) {
//			    command_line_count--;
				 break;
			 };
			 commands[command_line_count] = new char[temp_length+1];
			 strcpy(commands[command_line_count],temp);
			 command_line_count++;
			 if (command_line_count == 32) break; 
		 };
		 command_line.close();
	 };		
#endif
*/

#if !GRAPHICS_TO_MEMORY
void MainWindow::display_bitmap(HBITMAP bitmap_handle,
										  int width, int height,
										  int x, int y,
										  unsigned int x_scale, unsigned int y_scale,
										  DWORD operation) {
	HDC visible_device_context = GetDC(window_handle);
	HDC temp_device_context = CreateCompatibleDC(visible_device_context);
	SelectObject(temp_device_context,bitmap_handle);
	if (x_scale == 256 && y_scale == 256) {
		BitBlt(tt_hidden_device_context,x,y-height, // y is lower left not upper left
				 width,height,
				 temp_device_context,0,0,operation);
	} else {
		int new_width = (width*x_scale)>>8;
		int new_height = (height*y_scale)>>8;
		if (!StretchBlt(tt_hidden_device_context,x,y-new_height,
							 new_width,new_height,
							 temp_device_context,0,0,width,height,operation)) {
			tt_error_file() << "Troubles stretching image from 0 0 " 
							<< width << " " << height
							<< " to " << x << " " << y-new_height << " "
							<< new_width << " " << new_height << endl;
			return;
		};  
		width = new_width;
		height = new_height;
	};
	DeleteDC(temp_device_context);
	ReleaseDC(window_handle,visible_device_context);
};
#endif

#if !GRAPHICS_TO_MEMORY
void MainWindow::save_region(int min_x, int min_y, int max_x, int max_y) {
//	tt_error_file() << "Saving " << min_x << " " << min_y << " " << max_x << " " << max_y << endl;
	BitBlt(restore_device_context,min_x,min_y,
			 max_x-min_x+1,max_y-min_y+1,
			 tt_hidden_device_context,min_x,min_y,SRCCOPY);
// for debuggin
//	PatBlt(tt_hidden_device_context,min_x,min_y,max_x-min_x,max_y-min_y,WHITENESS);
};

void MainWindow::restore_region(int min_x, int min_y, int max_x, int max_y) {
//	tt_error_file() << "Restoring " << min_x << " " << min_y << " " << max_x << " " << max_y << endl;
	BitBlt(tt_hidden_device_context,min_x,min_y,
			 max_x-min_x+1,max_y-min_y+1,
			 restore_device_context,min_x,min_y,SRCCOPY);
};
#endif

#if !TT_DIRECT_INPUT
// added on 170800 so can compile ttbug32 for example
void release_mouse_capture() {
	::ReleaseCapture(); 
};
void capture_mouse() {
	HWND hwnd = tt_main_window->get_handle();
	::SetForegroundWindow(hwnd); // new on 230800
	::SetCapture(hwnd); 
};
#endif

/* couldn't get this to work right
bytes load_bytes_using_gdiplus(ascii_string full_file_name, boolean warn) { 
	// what about color_permutation??
	// only called when tt_bits_per_pixel == 8 and is needed to map image to current palette
	if (full_file_name == NULL) {
		return(NULL); // warn too?
	};
	if (already_has_extension(full_file_name,"ini")) { 
		return(NULL); // can't handle just yet
	};
	wide_string wide_file_name = copy_wide_string(full_file_name);
#if TT_DEBUG_ON
	millisecond start_time;
	if (tt_debug_mode == 31001) {
		start_time = timeGetTime();
	};
	if (tt_debug_mode == 51001) {
		tt_error_file() << "About to load bitmap " << full_file_name << endl;
	};
#endif
	Bitmap bitmap(wide_file_name);
	delete [] wide_file_name;
#if TT_DEBUG_ON
	if (tt_debug_mode == 31001) {
		tt_error_file() << (timeGetTime()-start_time)
						<< "ms to load image using GDI+ " << full_file_name << endl;
		start_time = timeGetTime();
	};
#endif
	int width = bitmap.GetWidth();
	if (width == 0) {
		if (warn) tt_error_file() << "Couldn't load " << full_file_name << " using GDI+" << endl;
		return(NULL);
	};
	int height = bitmap.GetHeight();
	int image_size = width*height+dib_header_size+1000; // what about padding? -- +1000 a test on 251001
	HBITMAP bitmap_handle;
	Color black(0,0,0);
	Status status = bitmap.GetHBITMAP(black,&bitmap_handle);
	if (status == Ok) {
		bytes answer = new byte[image_size];
		HWND window_handle = tt_main_window->get_handle();
		HDC device_context = GetDC(window_handle);
		byte header_memory[dib_header_size];
		BITMAPINFO *header = (BITMAPINFO *) header_memory;
		ZeroMemory(header,sizeof(BITMAPINFO));
		int result2 = GetDIBits(device_context,bitmap_handle,0,height,NULL,header,DIB_RGB_COLORS); // to fill in header
		header->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		header->bmiHeader.biSizeImage = image_size;
		header->bmiHeader.biWidth = width;
		header->bmiHeader.biHeight = height;
		header->bmiHeader.biPlanes = 1;
		header->bmiHeader.biBitCount = 8;
		header->bmiHeader.biCompression = BI_RGB;
		for (int i = 0; i < 256; i++) {
			header->bmiColors[i] = default_header->bmiColors[i];
		};
		int result = GetDIBits(device_context,bitmap_handle,0,height,answer,header,DIB_RGB_COLORS);
		ReleaseDC(window_handle,device_context);
 		if (result == 0) {
			if (warn) tt_error_file() << "Failed to convert image to proper format. " << full_file_name << endl;
			delete [] answer;
			return(NULL);
		};
		return(answer);
	} else {
		if (warn) tt_error_file() << "Result of getting bitmap from " << full_file_name << " is status code " << (int) status << endl;
		return(NULL);
	};
};

#if TT_IMGSOURCE
	// removed on 251001 since GDI+ is working out better
	// new on 151001 - move this out if it works well
	HISSRC file = _ISOpenFileSource(full_file_name);
	if (file == NULL) {
		UINT32 error_code = _ISGetLastError();
		tt_error_file() << "Error reading " << full_file_name << " code is " << error_code << endl;
		return(NULL);
	};
	int file_type = _ISGuessFileType(file);
	// move the source read pointer back to the beginning of the file.
	// the ISGuessFileType call will have moved the file pointer and
	// we need to be at the beginning of the stream when we do the 
	// actual file read!
	_ISSeek(file, 0,0);
	HGLOBAL bytes;
	UINT32 width, height;
	RGBQUAD palette[256];
	switch (file_type) {
		case BMP_FILE:
			bytes = _ISReadBMP(file,&width,&height,24,palette);
			break;
		default:
			tt_error_file() << "Currently can't hanle files of type #" << file_type << endl;
			return(NULL);
	};
	if (bytes == NULL) {
		UINT32 error_code = _ISGetLastError();
		tt_error_file() << "Error reading " << full_file_name << " code is " << error_code << endl;
		return(NULL);
	};
#endif
*/

// following from http://support.microsoft.com/support/kb/articles/q230/4/92.asp

/**********************************************************

GetOptimalDIBFormat

  Purpose:
   Retrieves the optimal DIB format for a display 
   device. The optimal DIB format is the format that 
   exactly matches the format of the target device. 
   Obtaining this is very important when dealing with 
   16bpp modes because you need to know what bitfields 
   value to use (555 or 565 for example).

   You normally use this function to get the best
   format to pass to CreateDIBSection() in order to
   maximize blt'ing performance.

  Input:
   hdc - Device to get the optimal format for.
   pbi - Pointer to a BITMAPINFO + color table
         (room for 256 colors is assumed).

  Output:
   pbi - Contains the optimal DIB format. In the 
         <= 8bpp case, the color table will contain the 
         system palette. In the >=16bpp case, the "color 
         table" will contain the correct bit fields (see 
         BI_BITFIELDS in the Platform SDK documentation 
         for more information).

  Notes:
   If you are going to use this function on a 8bpp device
   you should make sure the color table contains a identity
   palette for optimal blt'ing.

**********************************************************/ 
BOOL GetOptimalDIBFormat(HDC hdc, BITMAPINFOHEADER *pbi)
{
    HBITMAP hbm;
    BOOL bRet = TRUE;
    
    // Create a memory bitmap that is compatible with the
    // format of the target device.
    hbm = CreateCompatibleBitmap(hdc, 1, 1);
    if (!hbm)
        return FALSE;
    
    // Initialize the header.
    ZeroMemory(pbi, sizeof(BITMAPINFOHEADER));
    pbi->biSize = sizeof(BITMAPINFOHEADER);

    // First call to GetDIBits will fill in the optimal biBitCount.
    bRet = GetDIBits(hdc, hbm, 0, 1, NULL, (BITMAPINFO*)pbi, DIB_RGB_COLORS);
    
    // Second call to GetDIBits will get the optimal color table, o
    // or the optimal bitfields values.
    if (bRet)
        bRet = GetDIBits(hdc, hbm, 0, 1, NULL, (BITMAPINFO*)pbi, DIB_RGB_COLORS);
    
    // Clean up.
    DeleteObject(hbm);

    return bRet;
}

	// Counts the number of set bits in a DWORD.
BYTE CountBits(DWORD dw)
{
    int iBits = 0;
    
    while (dw) {
        iBits += (dw & 1);
        dw >>= 1;
    }
    
    return iBits;
}

/**********************************************************

GetRGBBitsPerPixel

  Purpose:
   Retrieves the number of bits of color resolution for each 
   color channel of a specified.

  Input:
   hdc - Device to get the color information for.
   
  Output:
   pRed   - Number of distinct red levels the device can display.
   pGreen - Number of distinct green levels the device can display.
   pBlue  - Number of distinct blue levels the device can display.

  Notes:
   This function does not return any meaningful information for
   palette-based devices.

**********************************************************/ 
BOOL GetRGBBitsPerPixel(HDC hdc, PINT pRed, PINT pGreen, PINT pBlue)
{
    BITMAPINFOHEADER *pbi;
    LPDWORD lpdw;
    BOOL bRet = TRUE;

    // If the target device is palette-based, then bail because there is no
    // meaningful way to determine separate RGB bits per pixel.
    if (GetDeviceCaps(hdc, RASTERCAPS) & RC_PALETTE) 
        return FALSE;  

    // Shortcut for handling 24bpp cases.
    if (GetDeviceCaps(hdc, PLANES) * GetDeviceCaps(hdc, BITSPIXEL) == 24) {
        *pRed = *pGreen = *pBlue = 8;
        return TRUE;
    }
   
    // Allocate room for a header and a color table.
    pbi = (BITMAPINFOHEADER *)GlobalAlloc(GPTR, sizeof(BITMAPINFOHEADER) + 
                                                sizeof(RGBQUAD)*256);
    if (!pbi)
        return FALSE;

    // Retrieve a description of the device surface.
    if (GetOptimalDIBFormat(hdc, pbi)) {
        // Get a pointer to the bitfields.
        lpdw = (LPDWORD)((LPBYTE)pbi + sizeof(BITMAPINFOHEADER)); // was cast to PINT but the compiler wasn't happy

        *pRed   = CountBits(lpdw[0]);
        *pGreen = CountBits(lpdw[1]);
        *pBlue  = CountBits(lpdw[2]);
    } else
        bRet = FALSE;
    
    // Clean up.
    GlobalFree(pbi);

    return bRet;
};

coordinate adjust_x_coordinate_for_window_size(coordinate x) { // new on 170502
   // does nothing if window size is same as tt_screen_width by tt_screen_height but could have been changed
   if (tt_exclusive_screen) {
      return(x);
   } else { // window can be any size relative to expected
      return((x*client_width)/tt_screen_width);
   };
};

coordinate adjust_y_coordinate_for_window_size(coordinate y) {
   if (tt_exclusive_screen) {
      return(y);
   } else {
      return((y*client_height)/tt_screen_height);
   };
};

coordinate return_client_width() { // new on 170502
   // if full screen then client width isn't useful (or "correct")
   if (tt_exclusive_screen) {
      return(tt_screen_width);
   } else {
      return(client_width);
   };
};

coordinate return_client_height() {
   if (tt_exclusive_screen) {
      return(tt_screen_height);
   } else {
      return(client_height);
   };
};

boolean user_wants_to_interrupt_also_process_messages_now(boolean only_look_for_quit_messages) { // new on 140105
	if (about_to_quit()) return(FALSE); // new on 270105
	HWND window_handle = tt_main_window->get_handle();
	if (tt_loading_is_ok_to_abort && 
		 (IsIconic(window_handle)
//#if !TT_DEBUG_ON // too confusing since lose focus to debugger all the time
// commented out the compile time conditional and added tt_exclusive_screen below on 050605
		 || (tt_exclusive_screen && GetFocus() != window_handle)
//#endif
		 )) { // if minimized or don't have focus
		// e.g. alt tabbed -- new on 170105
		return(TRUE);
	};
	boolean user_input_event = FALSE;
	MSG msg;
	if (only_look_for_quit_messages || !tt_loading_is_ok_to_abort) {
		// prior to 010305 the following PeekMessage conditions were part of this conditional so it typically went to the 
		// other branch even during saving during time travel
		// prior to 240205 the following was PeekMessage(&msg,window_handle,WM_DESTROY,WM_QUIT,PM_REMOVE) 
		// but the messages in between may cause problems
		if (PeekMessage(&msg,window_handle,WM_DESTROY,WM_DESTROY,PM_REMOVE) ||
			 PeekMessage(&msg,window_handle,WM_QUIT,WM_QUIT,PM_REMOVE) ||
			 PeekMessage(&msg,window_handle,WM_CLOSE,WM_CLOSE,PM_REMOVE)) {
			// added || !tt_loading_is_ok_to_abort on 230205 since if say loading a notebook page you can't interrupt (other than quit)
			// PeekMessage moved into condition on 300105 -- fixed a bug where somehow on some systems msg.message was WM_DESTROY
			// new on 160105
			// tried to replace NULL with window_handle but still was getting spurious WM_CLOSE and WM_DESTROY messages
//		PeekMessage(&msg,NULL,WM_QUIT,WM_QUIT,PM_REMOVE); // replaced WM_CLOSE with WM_QUIT on 270105 	
//		if (msg.message == WM_DESTROY) { // testing 300105
////			MessageBox(NULL,"WM_DESTROY ignored","TT",MB_OK); // just for testing!! 300105
//			tt_error_file() << "WM_DESTROY ignored on frame " << tt_frame_number << " with " << tt_sprites_saved_count << " saved." << endl;
//			return(FALSE);
//		} else if (msg.message == WM_CLOSE) { // testing 300105
////			MessageBox(NULL,"WM_CLOSE ignored","TT",MB_OK); // just for testing!! 300105
//			tt_error_file() << "WM_CLOSE ignored on frame " << tt_frame_number << " with " << tt_sprites_saved_count << " saved." << endl;
//			return(FALSE);
//		};
			if (msg.message == WM_QUIT || msg.message == WM_DESTROY || msg.message ==  WM_CLOSE) {
				// commented out on 270105 second and third disjunct issue 625 -- restored on 300105
				standard_exit(about_to_quit());
			};
		};
	} else {
		while (PeekMessage(&msg,NULL,WM_KEYFIRST,WM_KEYLAST,PM_REMOVE)) { 
#if TT_DEBUG_ON 
			if (tt_debug_mode == 260205) {
				tt_error_file() << "Peeked a keyboard message read msg.message " << msg.message << endl;
			};
#endif
			// WM_KEYFIRST,WM_KEYLAST replaced 0,0 on 160105 so only keyboard messages are considered
			if (msg.message == WM_QUIT || msg.message == WM_DESTROY || msg.message ==  WM_CLOSE) {
				// commented out on 270105 second and third disjunct issue 625 -- restored on 300105
//				MessageBox(NULL,"standard_exit from user_wants_to_interrupt_also_process_messages_now[2]","TT",MB_OK); // just for testing!! 270105
				standard_exit(about_to_quit());
			};
			switch (msg.message) {
				case WM_KEYDOWN:
					if (msg.wParam == VK_PAUSE || msg.wParam == VK_ESCAPE) {
						user_input_event = TRUE;
						break;
					};
				//case WM_PAINT: // shouldn't appear
				//	break; // do nothing here -- new on 160105
				default:
					TranslateMessage(&msg);
					DispatchMessage(&msg);
			};
		};
	};
	return(user_input_event);
};

/*
		IDirectDrawSurface *direct_draw_surface = NULL;
		result = surface->GetDirectDrawSurface(IID_IDirectDrawSurface,(void**) &direct_draw_surface);
		DDSURFACEDESC new_surface_description; // for debugging
		memset(&new_surface_description,0,sizeof(new_surface_description));
		new_surface_description.dwSize = sizeof(new_surface_description);
		direct_draw_surface->GetSurfaceDesc(&surface_description);
		new_surface_description.dwWidth = surface_description.dwWidth;
		new_surface_description.dwHeight = surface_description.dwHeight;
//		new_surface_description.ddpfPixelFormat = pixel_format;
		new_surface_description.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH; // | DDSD_PIXELFORMAT;
		transform_surface new_surface = NULL;
		result = directx_transform_surface_factory->
										CreateSurface(NULL,&new_surface_description,&DDPF_RGB8,NULL,0,NULL,IID_IDXSurface,(void **) &new_surface);
		if (FAILED(result)) {
			tt_error_file() << "Failed to create surface for " << full_file_name << endl;
			return(NULL);
		};
		result = directx_transform_surface_factory->BitBlt(new_surface,NULL,surface,NULL,0);
		if (FAILED(result)) {
			tt_error_file() << "Failed to copy surface to new format of " << full_file_name << endl;
			return(NULL);
		};     
//		result = directx_transform_surface_factory->CopySurfaceToNewFormat(surface,direct_draw,NULL,&DDPF_RGB8,&new_surface);
//		if (FAILED(result)) {
//			tt_error_file() << "Failed to copy surface to new format of " << full_file_name << endl;
//			return(NULL);
//		};
		IDirectDrawSurface *final_direct_draw_surface = NULL;
		result = new_surface->GetDirectDrawSurface(IID_IDirectDrawSurface,(void**) &final_direct_draw_surface);
		if (FAILED(result)) {
			tt_error_file() << "Failed to get direct draw surface of " << full_file_name << endl;
		};
		// release surface if this works OK
//		result = direct_draw_surface->SetPalette(direct_draw_palette);
//		if (FAILED(result)) {
//			tt_error_file() << "Failed to set palette of " << full_file_name << endl;
//		};
		memset(&surface_description,0,sizeof(surface_description)); // for debugging
		surface_description.dwSize = sizeof(surface_description);
		final_direct_draw_surface->GetSurfaceDesc(&surface_description);
		return(final_direct_draw_surface);
	};
//	return(surface);
};
*/

// commented out the following on 261204 since obsolete
//#if TT_DIRECTX_TRANSFORM||TT_GDIPLUS||TT_IMGSOURCE
//
//LPBITMAPINFOHEADER surface_to_DIB(draw_surface surface, int &transparent_color) { // new on 250500
//	// transparency arg added 060600
//	// produces a DIB equivalent of the DirectDraw surface
//	HRESULT result;
//	draw_surface_description surface_description;
//	ZeroMemory(&surface_description,sizeof(surface_description));
//	surface_description.dwSize = sizeof(surface_description);
//	result = surface->GetSurfaceDesc(&surface_description);
//	if (FAILED(result)) {
//		tt_error_file() << "Failed to get the surface description." << endl;
//	};
//	int width = surface_description.dwWidth;
//	int height = surface_description.dwHeight;
//	int bit_count;
//	int bytes_per_pixel = 1;
//	DDPIXELFORMAT pixel_format = surface_description.ddpfPixelFormat;
//	DWORD pixel_flags = pixel_format.dwFlags;
//	if (pixel_flags&DDPF_PALETTEINDEXED1) {
//		bit_count = 1;
//	} else if (pixel_flags&DDPF_PALETTEINDEXED2) {
//		bit_count = 2;
//	} else if (pixel_flags&DDPF_PALETTEINDEXED4) {
//		bit_count = 4;
//	} else if (pixel_flags&DDPF_PALETTEINDEXED8) {
//		bit_count = 8;
//	} else if (pixel_flags&DDPF_PALETTEINDEXEDTO8) {
//		bit_count = 8;
//		tt_error_file() << "Not clear if file will be handled correctly due to unusual color palette encoding." << endl;
//	} else {
//		bit_count = 24; // pixel_format.dwRGBBitCount; // I think this is always right...
//		bytes_per_pixel = 3; 
//		if (pixel_flags&DDPF_ALPHAPIXELS) {
//			bytes_per_pixel++;
//			bit_count = 32;
//		};
//	};
//	int picture_size = width*height*bytes_per_pixel;
//	int dib_size = sizeof(BITMAPINFOHEADER)+picture_size;
//	int number_of_colors = 1 << bit_count;
//	if (bit_count <= 8) {
//		dib_size += number_of_colors*sizeof(RGBQUAD);
//	}; 
//	LPBITMAPINFOHEADER dib = (PDIB) new bytes[dib_size];
//	ZeroMemory(dib,sizeof(BITMAPINFOHEADER)); // don't need to zero the pixels or palette info
//	BITMAPINFOHEADER *dib_header = (BITMAPINFOHEADER *) dib;
//	dib_header->biSize = sizeof(BITMAPINFOHEADER);
//	dib_header->biWidth = width;
//	dib_header->biHeight = height; 
//	dib_header->biPlanes = 1;
//	dib_header->biBitCount = bit_count;
//	dib_header->biCompression = BI_RGB; // I think this is always right
//	dib_header->biSizeImage = picture_size;
////	dib_header->biXPelsPerMeter = 3780; // don't know if this makes any sense
////	dib_header->biYPelsPerMeter = 3780;
////	dib_header->biClrUsed = number_of_colors; // can't hurt...
//	bytes pixels = (bytes) dib+sizeof(BITMAPINFOHEADER);
//	if (bit_count <= 8) {
//		// rewritten on 301001 to ignore errors such as no palette
//		RGBQUAD *rgbquad_entries = (RGBQUAD *) pixels;
//		for (int i = 0; i < number_of_colors; i++) {
//			rgbquad_entries[i].rgbRed = default_header->bmiColors[i].rgbRed;
//			rgbquad_entries[i].rgbGreen = default_header->bmiColors[i].rgbGreen;
//			rgbquad_entries[i].rgbBlue = default_header->bmiColors[i].rgbBlue;
//			rgbquad_entries[i].rgbReserved = 0;
//		};
//		pixels += number_of_colors*sizeof(RGBQUAD);
///*
//		LPDIRECTDRAWPALETTE palette; 
//		result = surface->GetPalette(&palette);
//		if (SUCCEEDED(result)) {
//			PALETTEENTRY *entries = new PALETTEENTRY[number_of_colors];
//			result = palette->GetEntries(0,0,number_of_colors,entries);
//			if (SUCCEEDED(result)) {
//				RGBQUAD *rgbquad_entries = (RGBQUAD *) pixels;
//				for (int i = 0; i < number_of_colors; i++) {
//					rgbquad_entries[i].rgbRed = entries[i].peRed;
//					rgbquad_entries[i].rgbGreen = entries[i].peGreen;
//					rgbquad_entries[i].rgbBlue = entries[i].peBlue;
//					rgbquad_entries[i].rgbReserved = 0;
//				};
//				delete [] entries;
//				pixels += number_of_colors*sizeof(RGBQUAD);
//			};
//			palette->Release();
//		};
//		*/
//	};
//   result = surface->Lock(NULL,&surface_description,DDLOCK_WAIT,NULL);
//   if (FAILED(result)) {
//		tt_error_file() << "Failed to lock a surface of a picture." << endl;
//		return(NULL);
//	};
////	memcpy(pixels,surface_description.lpSurface,picture_size);
//	// is upside down so
//	int pitch = surface_description.lPitch;
//	int row_length = width*bytes_per_pixel;
//	if (row_length&3) row_length += 4-row_length%4;
//	bytes surface_bytes = (bytes) surface_description.lpSurface;
//	for (int row = 0; row < height; row++) {
//		memcpy(pixels+row*row_length,surface_bytes+(height-(row+1))*pitch,row_length);
//	};
//	if (!tt_prefer_gdiplus && surface_description.dwFlags&DDSD_CKSRCBLT) {
//		// new on 070600 scan for an alpha value of 0
////		for (int i = 3; i < picture_size; i += 4) {
////			if (surface_bytes[i] == 0) { // transparent
////				transparent_color = find_closest_color(surface_bytes[i-3],surface_bytes[i-2],surface_bytes[i-1]);
////				break;
////			};
////		};
////		DDCOLORKEY color_key;
////		result = surface->GetColorKey(DDCKEY_SRCBLT,&color_key);
//		// commented out on 270901 - but revisit this - e.g. PNG or GIF with transparency
//		transparent_color = 253; // cyan is transparent when Direct Transform converts to DirectDrawSurface
//	};
//	unlock_surface(surface);
////#if TT_DEBUG_ON
//	// for debuggin
////	tt_error_file() << "From surface:" << endl;
////	bytes dib_bytes = (bytes) dib;
////	for (int i = 0; i < 50; i++) {
////		for (int j = 0; j < 40; j++) {
////			tt_error_file() << (int) dib_bytes[0] << " ";
////			dib_bytes++;
////		};
////		tt_error_file() << endl;
////	};
////	tt_error_file() << endl;
////#endif
//	return(dib);
//};
//#endif
