// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.

#include "stdafx.h" // new on 281002 for mini dump

#ifndef __TT_DEFS_H
#define __TT_DEFS_H

#define TT_32 1 // moved here on 040602
#define TT_WINDOWS 1 // moved here on 040602

#define TT_STARTTT 0 // new on 040602

class Picture; // new on 231102

#if !TT_STARTTT
#ifndef __TT_FLAGS
#define __TT_FLAGS
#include "flags.h"
#endif
#endif

#if TT_MICROSOFT
typedef __int64 int64;
#else
typedef long int int64; // best I can do??

#endif


#if !defined(TT_DEBUG_STARTTT) // not worrying about leaks in StartTT just now
// new on 160103 -- ignored if not debugging
//#define _CRTDBG_MAP_ALLOC
//#define  CRTDBG_MAP_ALLOC
#include <stdlib.h>

// found the following on the web codeproject.com
// it works around the problem that with new the bug report shows crtdbg as the source

#include <crtdbg.h>

#ifdef _DEBUG
// due to compiler warning:  no matching operator delete found; memory will not be freed if initialization throws an exception
// adding the following
#pragma warning(disable : 4291)

#define DEBUG_NEW new(THIS_FILE, __LINE__)

//#define MALLOC_DBG(x) _malloc_dbg(x, 1, THIS_FILE, __LINE__);
//#define malloc(x) MALLOC_DBG(x)

#endif // _DEBUG
#endif

//#include<iostream> // can't get this to work 
//std::ostream& operator<< (std::ostream& os, __int64 i); // new on 080202

//#pragma warning(disable : 4995) // new on 140302 for VS.NET since it complains about OLD_IOSTREAMS

typedef long int coordinate; // e.g. screen coordinate
typedef long int dimension;
#if TT_32 //why?? -- besides doesn't this just mean "int"?
typedef long int angle;
#else
typedef short int angle;
#endif

typedef unsigned long History; // for now good enough

#if TT_LARGE_CITIES
typedef int block_number;
#else
typedef unsigned char block_number;
#endif

typedef unsigned char old_block_number; // new on 161105 to be compatible for old files with the above

typedef long city_coordinate; // was long prior to 080202
// user coordinates rarely need to be long but wrap around is rarer this way
typedef long int user_coordinate;

typedef unsigned char cost;

typedef long int city_scale;

// Windows has some .h file that redefines it as unsigned char -- ok with me
#if !TT_32
typedef unsigned char boolean; // defined in 32 bit Windows .H files somewhere
#endif
// like boolean but used in classes with : 1 to indicate 1 bit
typedef unsigned char flag; 

#include <windows.h>

// enum boolean {FALSE,TRUE}; // no &bool

#if TT_DIRECTX
//#include <dsound.h>
// changed above to the following on 040500 since NT 4.0 otherwise can't play sounds
// note prior to 042500 the search paths found the following and not the mssdk version (DX 7.0 just now)
//#include "C:\Program Files\Microsoft Visual Studio\VC98\Include\dsound.h"
#include <dsound.h> // restored on 231102
#include <ddraw.h>
#if TT_DIRECT_INPUT
// added the following on 240500 since I'm relying only upon Direct 7.0 (maybe only 5.0)
#define DIRECTINPUT_VERSION 0x0700
#include <dinput.h>
#endif
#if TT_DIRECT_PLAY
#include <dplay.h>
#include <Rpcdce.h> // to generate GUIDs
#endif
#endif

#if TT_IMGSOURCE
#include "C:\isource\_ISource.h"
#endif

#if TT_GDIPLUS
#include <gdiplus.h>
using namespace Gdiplus; // a good idea??
#endif

#if TT_MULTIPLE_PRECISION
#include "gmp.h"
typedef mpq_t rational;
typedef mpq_ptr rational_pointer;
typedef mpz_t bignum;
typedef mpz_ptr bignum_pointer;
class Rational;
typedef Rational NUMBER;
#else if !TT_STARTTT
typedef Integer NUMBER;
#endif

//#if TT_WINDOWS&&!GRAPHICS_TO_MEMORY
// removed conditionals on 011001 since color is not always an index anymore
typedef COLORREF color;
//#else
//typedef unsigned char color;
//#endif
typedef unsigned char color_index; // new on 100101

typedef unsigned short int color16; // new on 061101

typedef long millisecond;

#if TT_UNICODE
#include <sstream>
#include <ios>
#include <ostream>
#include <fstream>
typedef WCHAR *string;
typedef const WCHAR *const_string;
typedef WCHAR character;
typedef WCHAR unsigned_character;
#define upper_case CharUpperW
#define lower_case CharLowerW
#else
typedef char *string;
//typedef const char *const_string;
typedef char *const_string;
typedef char character;
typedef unsigned char unsigned_character;
// AnsiLower is the obsolete form of CharLower
// but works in both 16 and 32 bit code
#define upper_case AnsiUpper
#define lower_case AnsiLower
#endif

// the following shouldn't be needed but since only NT has full set of implemented Unicode versions
#define lstrcmpiG lstrcmpi
#define lstrcmpG lstrcmp
#define LoadStringG LoadString
#define LoadLibraryG LoadLibrary
#define CreateWindowG CreateWindow
#define FindWindowG FindWindow
#define SetWindowTextG SetWindowText
#define SetDlgItemTextG SetDlgItemText
#define GetDlgItemTextG GetDlgItemText
#define WritePrivateProfileStringG WritePrivateProfileString
#define GetPrivateProfileStringG GetPrivateProfileString
#define GetPrivateProfileIntG GetPrivateProfileInt
#define ShellExecuteG ShellExecute
#define RegOpenKeyG RegOpenKey
#define RegQueryValueG RegQueryValue
#define RegCreateKeyG RegCreateKey
#define RegSetValueG RegSetValue
#define sndPlaySoundG sndPlaySound
#define IsCharAlphaNumericG IsCharAlphaNumeric
#define WNDCLASSG WNDCLASS
#define LoadIconG LoadIcon
#define RegisterClassG RegisterClass
#define mciSendStringG mciSendString
#define mciGetErrorStringG mciGetErrorString
#define FindResourceG FindResource
#define DefWindowProcG DefWindowProc


#include <tchar.h>
typedef char *ascii_string; // since file names won't be unicode and Java output
//typedef const char *const_ascii_string;
typedef char *const_ascii_string;


#define TT_WIDE_TEXT TT_32
#if TT_WIDE_TEXT
//#if TT_MICROSOFT
typedef WCHAR wide_character;
//#else
// for some reason Borland C++ 5.0 is happy this way
//typedef unsigned short wide_character;
//#endif
#define wide_string_length wcslen
#define compare_wide_strings_of_length wcsncmp // renamed on 050802
#define compare_wide_strings wcscmp // new on 050802
#else
typedef char wide_character; // no alternative for 16-bit code
#define wide_string_length strlen
#define compare_wide_strings_of_length strncmp
#define compare_wide_strings strcmp
#endif

#if TT_WIDE_TEXT
// the following shouldn't be needed but since only NT has full set of implemented Unicode versions
// these exist in both 95 and NT
#define GetTextExtentPoint32G GetTextExtentPoint32W
#define TextOutG TextOutW
#define MessageBoxG MessageBoxW
#else
#define GetTextExtentPoint32G GetTextExtentPoint32A
#if TT_32
#define TextOutG TextOutA
#define MessageBoxG MessageBoxA
#else
#define TextOutG TextOut
#define MessageBoxG MessageBox
#endif
#endif

typedef wide_character *wide_string;

//#include <fstream> // replaced by following on 210703
#include <iostream>
#include <ostream>
#include <sstream> // what is this?
#include <strstream>
#include <fstream>
#include <ios>
#include <iomanip>
//using namespace std;


#if TT_NEW_IO
typedef std::wostream output_stream;
typedef std::wistream input_stream;
typedef std::basic_ostringstream<character> output_string_stream;
// don't know why the following doesn't work right...
//typedef std::basic_ostringstream<char> output_ascii_string_stream;
//typedef output_string_stream output_ascii_string_stream;
typedef std::basic_istringstream<character> input_string_stream;
typedef std::basic_istringstream<char> input_ascii_string_stream;
typedef std::basic_ifstream<character> input_file_stream;
typedef std::basic_ofstream<character> output_file_stream;
typedef std::basic_ofstream<char> output_ascii_file_stream;
typedef std::basic_ifstream<char> input_ascii_file_stream;
typedef std::basic_istream<char> ascii_input_stream;
typedef std::basic_ifstream<char> ascii_input_file_stream;
typedef std::basic_ostream<char> ascii_output_stream;
typedef std::basic_ofstream<char> ascii_output_file_stream;
// following used on io stream to deal with difference between old and new versions
#define STR str().c_str()
#define LENGTH str().length()
#else
typedef std::ostream output_stream;
typedef std::istream input_stream;
// following should work
//typedef output_string_stream output_string_stream;
//typedef output_string_stream output_ascii_string_stream;
//typedef istrstream input_string_stream;
//typedef istrstream input_ascii_string_stream;
#define output_string_stream std::ostrstream
#define output_ascii_string_stream std::ostrstream
#define input_string_stream std::istrstream
#define input_ascii_string_stream std::istrstream
typedef std::ifstream input_file_stream;
typedef std::ofstream output_file_stream;
typedef std::ofstream output_ascii_file_stream;
typedef std::ifstream input_ascii_file_stream;
typedef std::istream ascii_input_stream;
typedef std::ifstream ascii_input_file_stream;
typedef std::ostream ascii_output_stream;
typedef std::ofstream ascii_output_file_stream;
#define STR str()
#define LENGTH pcount()
#endif

#define endl std::endl
// had is as ascii_output_stream but hard to interoperate with
// code to describe items used by Marty and this
typedef output_stream java_stream;
typedef string java_string;
typedef const_string const_java_string;

output_stream & tt_error_file();

#if TT_WINDOWS&&GRAPHICS_TO_MEMORY
#if TT_32
typedef unsigned char *work_page;
#else
typedef unsigned char huge *work_page;
#endif
#endif

#if TT_DIRECTX
typedef LPDIRECTSOUNDBUFFER sound_buffer;

#if TT_DIRECT_DRAW7
typedef IDirectDrawSurface7 *draw_surface; // replaced the above on 210303
typedef DDSURFACEDESC2 draw_surface_description; // new on 210303
#else
typedef LPDIRECTDRAWSURFACE draw_surface;
typedef DDSURFACEDESC draw_surface_description; // new on 210303
#endif
#else if !TT_STARTTT
typedef work_page sound_buffer;
typedef work_page draw_surface;
#endif

#if !TT_STARTTT
typedef sound_buffer *sound_buffer_pointer;
#endif

typedef unsigned char *bytes;

typedef unsigned char *hash; // typically 128-bits - new on 170303

#if !TT_32
typedef unsigned char byte;
#endif

typedef string *strings;
typedef ascii_string *ascii_strings;

//typedef BITMAPINFOHEADER huge* dib;
typedef LPBITMAPINFO dib;

typedef long int image_id;

#include "constant.h" // for convience

// once used the following but it was too buggy and space wasteful
// typedef BI_ArrayAsVector<string> Strings;
// typedef BI_ArrayAsVectorIterator<string> Strings_iterator;

//typedef void (*ImageDisplayFunction) (char *image, int width, int height);

class Sprite;
typedef Sprite *SpritePointer;
typedef Sprite **SpritePointerPointer;

class Text;
typedef Text *TextPointer;

class Sprites;
typedef Sprites *SpritesPointer;

typedef void *Pointer;
typedef void **PointerPointer;

//typedef void (*AnimationCallBack) (void *sprite); // replaced by an enum of indices into a table of AnimationCallBackProcedure on 121102
typedef void (*AnimationCallBackProcedure) (Sprite *sprite); // new on 121102

typedef boolean (*PredicateCallBack) (Sprite *sprite, void *arg); // arg added on 100410

// a better solution would have been to have just one arg which is a structure
// the first element indicates the size of the structure -- think about it
typedef BOOL (*PrimitiveReceiver) (HANDLE handle, Pointer *data, ascii_string types, ascii_string label, ascii_string country_code, 
											  PointerPointer *out, ascii_string *out_types, ascii_string *to_say, BOOL *ok_to_speak, HINSTANCE string_library);
//typedef BOOL (*PrimitiveHelper) (HANDLE handle, string label, string country_code, string *to_say);
//typedef BOOL (*PrimitiveDumper) (HANDLE handle, string label, string country_code, string *to_save);
//typedef BOOL (*PrimitiveRestorer) (HANDLE handle, string label, string country_code, string to_save, PointerPointer *out, string *out_types, string *to_say);

class Bird;
typedef Bird *BirdPointer;

class Nest;
typedef Nest *NestPointer;

class DeletedMemory;
typedef DeletedMemory *DeletedMemoryPointer;

typedef SpriteType *SpriteTypePointer;

typedef unsigned short int cubby_index;

class FavoriteSizeAndLocation{
  public:
     city_coordinate favorite_width,favorite_height,favorite_llx,favorite_lly;
     FavoriteSizeAndLocation() :
       favorite_width(0),
       favorite_height(0),
       favorite_llx(0),
       favorite_lly(0) {};
};

#if TT_DIRECTX
#define BEGIN_GDI get_device_context();
#define END_GDI release_device_context();
#else
#define BEGIN_GDI
#define END_GDI
#endif

// moved 280199 from sprite.h
enum UpdateStatus {NO_UPDATE_CHANGE,UPDATE_INITIALIZE,
                   UPDATE_CYCLE,UPDATE_CHANGE,UPDATE_TO_FIT_IN_CUBBY,UPDATE_CHANGE_FROM_REMOTE,UPDATE_TO_FIT_ON_PAGE}; // UPDATE_TO_FIT_ON_PAGE new on 071004

// moved 051100 from sprite.h
enum ChangeReason {JUST_MOVED, EXPANDER_CHANGED_IT, VACUUM_CHANGED_IT,
						 LEADER_CHANGED, COPY_INITIALIZATION, NO_REASON_GIVEN,
						 AVOID_COLLISION, UPDATING, THOUGHT_BUBBLE_ENTERED,
						 ROBOT_ATTACHED_TO_THOUGHT_BUBBLE,
						 SELECTION_HIGHLIGHT, ANIMATION_COMPLETED,
                   TO_FIT_INSIDE,
						 FOLLOWING_INDIRECTIONS_CHANGE, FOLLOWING_CONTROLEES_CHANGE,
						 TRANSLATING_ROOM_TO_FLOOR_COORDINATES,TRANSLATING_FLOOR_TO_ROOM_COORDINATES,
						 TRANSLATING_CITY_TO_GROUND_COORDINATES,TRANSLATING_GROUND_TO_CITY_COORDINATES, // new on 070202
						 TEMPORARY_FOR_DUMPING,
						 LOADING_GEOMETRY, SETTING_TO_GOOD_SIZE, ANIMATION_STARTING, // new on 090102
						 NEW_TEXT_EXTENT, // new on 180102
						 DUE_TO_NON_ZERO_SPEED, // new on 040302
                   VACUUM_RESTORING, // new on 310702
						 TO_FIT_ON_A_NEST, // new 150803
						 ANIMATING}; // new on 311004

enum Relation {GREATER_THAN_RELATION,EQUAL_RELATION,LESS_THAN_RELATION, // moved here from picture.h on 300102
					INCOMPARABLE_RELATION, // type mismatch
					NO_RELATION, // args missing
					SUSPENSION_RELATION // encountered an empty nest (or empty hole?)
					}; 

enum NumberFormat {TT_LONG=0,TT_BIGNUM,TT_RATIONAL,TT_BIGNUM_DECIMAL,TT_BIGNUM_WITH_FRACTION,TT_DOUBLE_FLOAT,
						 TT_COMPUTE_VALUE_WHEN_LOADED, 
						 // new on 020302 used for dumping sensors that need to compute value rather than store it
						 TT_NEW_LONG, // in contrast to TT_LONG which is compatible with the old format (new on 050302)
						 TT_UNDETERMINED_NUMBER_FORMAT};

enum NumberValueFormat {TT_LONG_VALUE,TT_DOUBLE_FLOAT_VALUE,TT_BIGNUM_VALUE,TT_RATIONAL_VALUE}; // new on 050302

enum NumberOperation // new on 100202
	{SUBTRACT_FROM=0,MULTIPLY_BY,INCREASE_BY,DIVIDE_BY,MODULUS_BY,MAKE_EQUAL,TO_THE_POWER,
	 BITWISE_EXCLUSIVE_OR,BITWISE_AND,BITWISE_OR,LAST_BINARY_NUMBER_OPERATION,
	 INTEGER_PART,FRACTION_PART,NUMERATOR,DENOMINATOR,
	 SINE,COSINE,TANGENT,ARCSINE,ARCCOSINE,ARCTANGENT, // worth having both degree and radian versions??
	 NATURAL_LOG,LOG10, // new on 270202
	 COMPLEMENT, // used this way??
	 NO_NUMBER_OPERATION};

//#if TT_32
// from dispdib.h - only this is needed when running 32-bit
//#define DISPLAYDIB_MODE_320x200x8   0x0001  // 320-by-200
//#define DISPLAYDIB_MODE_320x240x8   0x0005  // 320-by-240
//#endif
// Ken Kahn's addition 
//#define DISPLAYDIB_MODE_640x480x8 -1 // no dispdib but a good mode
//#endif

/*
// borrowed from bc4/include/stdlib for still using bc 3.1
#ifndef __MINMAX_DEFINED
#define __MINMAX_DEFINED
template <class T> inline const T& min( const T& t1, const T& t2 )
{
	 return t1>t2 ? t2 : t1;
}

template <class T> inline const T& max( const T& t1, const T& t2 )
{
	 return t1>t2 ? t1 : t2;
}
#endif
*/

//#if !TT_WATCOM
#if !TT_MICROSOFT&&!TT_STARTTT
long inline max(long x, long y) {
	return(x>y?x:y);
};
inline short max(short x, short y) {
	return(x>y?x:y);
};
inline long min(long x, long y) {
	return(x<y?x:y);
};
inline short min(short x, short y) {
	return(x<y?x:y);
};
#endif

// commented out on 231001 since is now a run-time flag rather than compile time
//#if TT_IMAGES_ARE_USER_IMAGES
//class UserImage;
//typedef UserImage GenericImage;
//#else
//class TTImage;
//typedef TTImage GenericImage;
//#endif
#if !TT_STARTTT

class UserImage;
typedef UserImage *UserImagePointer; // new on 231001
class TTImage;
typedef TTImage *TTImagePointer;

#if TT_XML
typedef IXMLDOMDocument xml_document;
typedef IXMLDOMComment xml_comment;
typedef IXMLDOMNode xml_node;
typedef IXMLDOMElement xml_element;
typedef IXMLDOMProcessingInstruction xml_processing_instruction;
typedef IXMLDOMCDATASection xml_character_data;
typedef IXMLDOMText xml_text;
typedef IXMLDOMNamedNodeMap xml_node_map; // new on 220803

typedef xml_element *xml_element_pointer;
typedef xml_node *xml_node_pointer;

#endif

#if !defined(__TT_ENTITY_H)
#include "entity.h" // needs the above XML stuff
#endif

#if TT_XML
// needs the above Entity stuff
typedef void (ChildTagHandler) (Entity *entity, Tag tag, xml_node *element, void *extra);
typedef void (AttributeHandler) (BSTR attribute, void *extra);
#endif

//class InputStream; // others use it but 
#if !defined(__ISTREAM_H)
#include "istream.h" // new on 060703
#endif

#if !defined(__NUMBERVALUE_H)
#include "numvalue.h"
#endif

typedef NumberValue *NumberValuePointer;

#endif

//typedef NumberValuePointer speed; // new on 050302 -- old convention was to use user_coordinate
// too confusing - just use NumberValue directly now

#if TT_DYNA_ZIP
typedef char *zipFile;
typedef char *unzFile;
#else
#include "C:\misc\zlib114\zip.h"
#include "C:\misc\zlib114\unzip.h" // for the archive data types
#endif

#include "callback.h" // since so widely used

#endif
