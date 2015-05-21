// Copyright (c) 1992-2008.  Ken Kahn, Animated Programs, All rights reserved.

#if defined(__BORLANDC__)
#define TT_MICROSOFT 0
//#define TT_DEBUG_ON 1
#else
#define TT_MICROSOFT 1
#endif

#if TT_MICROSOFT // restored on 170800
#define TT_DIRECT_INPUT 1
#define TT_TEXT_TO_SPEECH 1
#define TT_DIRECT_PLAY 1
#define TT_DIRECT_PLAY8 1 // not operational
#define TT_DIRECT_DRAW7 0 // worked but perhaps some bug after saving as Java applet...
#define TT_DIRECTX_TRANSFORM 0 // at least while experimenting with TT_IMGSOURCE -- experiment on 231102 
//#define TT_IMAGES_ARE_USER_IMAGES 1 // set to 1 on 240901 
#define TT_IMGSOURCE 0 // new on 151001 to use this library to read graphics files
#define TT_GDIPLUS 1 // new on 171001
#endif

#define TT_MULTIPLE_PRECISION 1 // new on 280102

#define TT_ABSOLUTE_MOUSE 1 // restored on 160402

#define TT_XML 1 // new on 020802

#define TT_POST3121 1

#define TT_SHRINKING_AND_GROWING TT_POST3121 
// new on 031104 to make this a compile time switch so can generate a version without it
#define TT_SPEAK_ALL_NUMBERS TT_POST3121 // new on 061104 to pre-process text-to-speech to speak all numbers
#define TT_ENHANCED_PUZZLE_DIALOG TT_POST3121 // new on 141104 - at first just to enable skipping to next puzzle
#define TT_ENHANCED_DRAG_AND_DROP TT_POST3121 // new on 281204 to support dragging links from web pages
#define TT_LOADING_AND_SAVING_INTERRUPTABLE TT_POST3121 // new on 140105 to enable interrupting loads or saves
#define TT_DIRECT_PLAY_WARNING TT_POST3121 // new on 280205 to generate warnings when DirectPlay objects are created
#define TT_FUNCTION_KEY_HELP TT_POST3121 // new on 280205 to display function key help
#define TT_LARGE_CITIES TT_POST3121 // new on 020405 so cities aren't limited to 256x256
#define TT_CORRECT_IMPLEMENTATION_OF_DISPLAY_X_AND_Y_AS_INTEGERS TT_POST3121
#define TT_CALL_FOR_MARTY TT_POST3121 // new on 030805

#define TT_SPECIAL_DEBUGGING 0 // new on 021003 for generating a release version with special debugging abilities
#define TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME 0 // new on 021204 for debugging crash while starting on Windows Me

#define TT_WANT_IMAGE_STATISTICS 0 // TT_DEBUG_ON is another reasonable value -- new on 050505

// the following is a bit wasteful if not a patch
#define TT_PATCH 1 
// remember to reset following when releasing
#define TT_TRIAL_EXPIRATION 0
#define TT_TRIAL_LIMITS_PUZZLES 0
// this one turns off saving, java applet generation, and expires as well
#define TT_TRIAL_VERSION 0
#define TT_SMALL_TRIAL 0
// REMEMBER THE IDC_TT_INI ENTRY WHEN CHANGING THIS
// TT_SMALL_TRIAL is used to generate a small trial version that uses only some of the graphics and sounds and only runs puzzles
// following new on 081205 to generate a version that can run but not create programs
#define TT_NO_ROBOT_TRAINING 0

// commented this out on 010500 since shouldn't depend upon which compiler
// and only 32-bit version supported now
//#define TT_DEBUG_ON 1 set by Visual Studio now
//#else
//#ifdef __FLAT__
//#define TT_32 1
//#else
//#define TT_32 0
//#endif
//#endif

#define TT_FOR_FAR_EAST_WINDOWS 1

#define TT_DIRECTX TT_32
//#define TT_DIRECT_INPUT TT_DIRECTX 

#define TT_STRINGS_STATIC 0
#define TT_95 TT_32
#define WING !TT_DIRECTX
#define DISPDIB_CLASS !TT_DIRECTX
#define DISPDIB !TT_DIRECTX

#define TT_UNICODE 0
#if TT_UNICODE
// set internal switch for compiler
#define _UNICODE
#endif

// Borland 5.02 doesn't seem to have std::basic_* ...
//#define TT_NEW_IO TT_MICROSOFT
#define TT_NEW_IO 0

// following used only in restricted ways to avoid hitting the DGROUP 64K limit
//#define TT16_DEBUG_ON 0    // defined in project now

// out of use
#define TT_LEGO 0
#define TT_WATCOM 0
#define TT_SICS_ULTRASOUND_CONTROLLER 0

#define SMART_HEAP 0
#define HEAP_AGENT 0
#define GRAPHICS_TO_MEMORY 1

// to get warning about 16-bit specific code
#if !TT_MICROSOFT
// not sure why but Borland needs this while Microsoft warns about it
#define STRICT
#endif

// following is now set only for release version
//#define TT_WANT_EXECEPTION_BREAKS 1
// replaced on 291002 by tt_want_exception_breaks

#define TT_CAREFUL 1
// when careful does more error checking - TT_DEBUG_ON should be used for reporting problems and catching unlikely ones

// obsolete versions
#define FAST_GRAPH 0
#define FLASH_GRAPHICS 0
//#define USING_RC_LIBRARIES 0
#define SOUND_CAPABLE 0
#define HIGH_RESOLUTION_CLOCK 0
#define TT_BETA 0

#define TT_GMP_VERSION_4_1 1 // the buggy one (at least as I compiled it) -- trying again on 020304

#define TT_DYNA_ZIP 1
//#define TT_DYNA_ZIP_STATIC 0 
// commented out on 080703 defined by the compiler configuration (since static is OK without debugging)
#define TT_WRITE_ZIP_FILE_INCREMENTALLY 0 // new on 300603 

#define TT_BUG_FIXES 1 // since the release of 3.30 (but could be any release) -- new on 301103

#define TT_ALPHA_FEATURE 1 // new on 271003 for enhancements added after "school release"

#define TT_ALPHA 1 // new on 011003 - for changes after the most recent school release
#define TT_ALPHA2 1 // new on 121003 to deal with changes after 3.14
#define TT_ALPHA3 1 // new on 141003 for changes since 3.15
#define TT_ALPHA4 1 // new on 201003 for changes since 3.16
#define TT_ALPHA5 1 // new on 211003 for changes since 3.17

#define TT_POST318 1 // new on 231003
#define TT_POST319 1
#define TT_POST320 1
#define TT_POST321 1
#define TT_POST322 1
#define TT_POST323 1
#define TT_POST324 1
#define TT_POST325 1
#define TT_POST326 1
#define TT_POST327 1
#define TT_POST328 1
#define TT_POST329 1

#define TT_POST3186 1

#define TT_POST3187 1

#define TT_TTINI_ALTERNATIVE_TO_REGISTRY 1
