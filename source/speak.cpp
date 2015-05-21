// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved.

// until using DirectSound is more reliable
//#define TT_SAPI_DIRECT_SOUND 0

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif

#if TT_TEXT_TO_SPEECH

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <mmsystem.h>
#include <initguid.h>
#include <objbase.h>
#include <objerror.h>
#include <ole2ver.h>

//#include <dsound.h>
//#include "d:\dxsdk\include\dsound.h"

#if TT_MICROSOFT
#include <speech.h>
#else
// needed to edit the file for Borland C++
#include "speech.h"
#endif

#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif
#if !defined(__TT_HELP_H)
#include "help.h"
#endif
#if !defined(__TT_MARTIAN_H)
#include "martian.h"
#endif
#if !defined(__TT_UTILS_H)
#include "utils.h"
#endif
#if !defined(__TT_COMMON_H)
#include "common.h" 
#endif
#if !defined(__TT_WINMAIN_H)
#include "winmain.h"
#endif
#if !defined(__TT_PICTURE_H)
#include "picture.h"
#endif
#if !defined(__TT_STRINGS_H)
#include "strings.h"
#endif
#if !defined(__TT_SPEAK_H)
#include "speak.h"
#endif

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/***********************************************************************
This file is roughly based upon

Demo.Cpp - Code to demo tts.

Copyright c. 1995 by Microsoft Corporation

*/


//#include "resource.h"

//flag speech_in_progress_flag = FALSE;
flag ole_started = FALSE;
flag cant_start_speech = FALSE; // don't know yet
long paragraph_id = 0;
short speaks_in_progress = 0;

DWORD notify_sink_registration_key = 0xFFFFFFFF;

wide_string text_remaining_to_speak = NULL; // new on 121104 to be the chunk still needing to be spoken
wide_string original_text_remaining_to_speak = NULL; // pointer to segment copied so can be reclaimed later
const int max_speech_chunk = 200; // about 20 seconds worth -- pauses between segments should be just a second or two

//boolean speech_in_progress() {
//    return(speech_in_progress_flag);
//};

class Text_To_Speech_Buffer_Notify : public ITTSBufNotifySink {
    private:

    public:
        Text_To_Speech_Buffer_Notify() :
          ITTSBufNotifySink() {
        };
//        ~Text_To_Speech_Buffer_Notify();
        // IUnkown members that delegate to m_punkOuter
      // Non-delegating object IUnknown
      STDMETHODIMP         QueryInterface (REFIID, LPVOID FAR *);
      STDMETHODIMP_(ULONG) AddRef(void);
      STDMETHODIMP_(ULONG) Release(void);

      // ITTSNotifySink
	   STDMETHOD (BookMark)		   	(QWORD, DWORD);
	   STDMETHOD (TextDataDone)   	(QWORD, DWORD);
	   STDMETHOD (TextDataStarted)   (QWORD);
	   STDMETHOD (WordPosition)      (QWORD, DWORD);
};

STDMETHODIMP Text_To_Speech_Buffer_Notify::QueryInterface (REFIID riid, LPVOID *ppv)
{
	*ppv = NULL;

	/* always return our IUnknown for IID_IUnknown */
	if (IsEqualIID (riid, IID_IUnknown) || IsEqualIID(riid,IID_ITTSBufNotifySink))
	{
		*ppv = (LPVOID) this;
		return S_OK;
	}

	// otherwise, cant find
	return ResultFromScode (E_NOINTERFACE);
}

STDMETHODIMP_ (ULONG) Text_To_Speech_Buffer_Notify::AddRef (void)
{
	// normally this increases a reference count, but this object
	// is going to be freed as soon as the app is freed, so it doesn't
	// matter
	return 1;
}

STDMETHODIMP_(ULONG) Text_To_Speech_Buffer_Notify::Release (void)
{
	// normally this releases a reference count, but this object
	// is going to be freed when the application is freed so it doesnt
	// matter
	return 1;
}

STDMETHODIMP Text_To_Speech_Buffer_Notify::BookMark (QWORD , DWORD id) {
#if TT_DEBUG_ON
   tt_error_file() << "bookmark " << id << endl;
#endif
   return NOERROR;
}

STDMETHODIMP Text_To_Speech_Buffer_Notify::TextDataDone (QWORD , DWORD ) {
//   speech_in_progress_flag = FALSE;
//   tt_martian->speech_has_finished(paragraph_id);
#if TT_DEBUG_ON
	if (tt_debug_mode == 60998) {
		tt_error_file() << "TextDataDone at " << timeGetTime() << endl;
	};
#endif
   return NOERROR;
};

STDMETHODIMP Text_To_Speech_Buffer_Notify::TextDataStarted (QWORD ) {
//   speech_in_progress_flag = TRUE;
   return NOERROR;
}

STDMETHODIMP Text_To_Speech_Buffer_Notify::WordPosition (QWORD , DWORD ) {
// doesn't work for concatenated text engine
//   tt_martian->next_word_spoken(offset);
   return NOERROR;
}


/*************************************************************************
Text_To_Speech_Notify - Notification object.
*/

class Text_To_Speech_Notify : public ITTSNotifySink {
   private:

   public:
      Text_To_Speech_Notify (void);
      ~Text_To_Speech_Notify (void);

//		void NotifyOutput( const TCHAR *str );

      // IUnkown members that delegate to m_punkOuter
      // Non-delegating object IUnknown
      STDMETHODIMP         QueryInterface (REFIID, LPVOID FAR *);
      STDMETHODIMP_(ULONG) AddRef(void);
      STDMETHODIMP_(ULONG) Release(void);

      // ITTSNotifySink
		STDMETHOD (AttribChanged)  (DWORD);
		STDMETHOD (AudioStart)     (QWORD);
		STDMETHOD (AudioStop)      (QWORD);
		STDMETHOD (Visual)         (QWORD, TCHAR, TCHAR, DWORD, PTTSMOUTH);
   };

Text_To_Speech_Notify::Text_To_Speech_Notify () {
}

Text_To_Speech_Notify::~Text_To_Speech_Notify () {
}

STDMETHODIMP Text_To_Speech_Notify::QueryInterface (REFIID riid, LPVOID *ppv)
{
	*ppv = NULL;

	/* always return our IUnknown for IID_IUnknown */
	if (IsEqualIID (riid, IID_IUnknown) || IsEqualIID(riid,IID_ITTSNotifySink))
	{
		*ppv = (LPVOID) this;
		return S_OK;
	}

	// otherwise, cant find
	return ResultFromScode (E_NOINTERFACE);
}

STDMETHODIMP_ (ULONG) Text_To_Speech_Notify::AddRef ()
{
	// normally this increases a reference count, but this object
	// is going to be freed as soon as the app is freed, so it doesn't
	// matter
	return 1;
}

STDMETHODIMP_(ULONG) Text_To_Speech_Notify::Release ()
{
	// normally this releases a reference count, but this object
	// is going to be freed when the application is freed so it doesnt
	// matter
	return 1;
}

STDMETHODIMP Text_To_Speech_Notify::AttribChanged (DWORD )
{
   return NOERROR;
}

STDMETHODIMP Text_To_Speech_Notify::AudioStart (QWORD )
{
//   tt_error_file() << "Audio Start " << current_time() << endl;
//   speech_in_progress_flag = TRUE;
   return NOERROR;
}

STDMETHODIMP Text_To_Speech_Notify::AudioStop (QWORD )
{
//   tt_error_file() << "Audio Stop " << current_time() << endl;
//   speech_in_progress_flag = FALSE;
   if (about_to_quit()) return NOERROR;
	if (text_remaining_to_speak != NULL) { // new on 121104
		speak(text_remaining_to_speak);
		return NOERROR;
	};
   if (paragraph_id < 0) { // started by "using" text
      speaks_in_progress--;
      if (speaks_in_progress == 0) {
         turn_on_sound_and_speech_off();
      };
   } else {
//	  tt_martian->speech_has_finished(paragraph_id);
		if (!replaying()) { // new on 050600 since if replaying a demo already recorded when speech ended when recording
			tt_paragraph_id_just_finished = paragraph_id; // changed on 241199 so this can be recorded
#if TT_DEBUG_ON
			if (tt_debug_mode == 111203) {
				tt_error_file() << "tt_paragraph_id_just_finished set to " << paragraph_id << " at " << timeGetTime() << endl;
			};
#endif
		};
	};
   return NOERROR;
}

STDMETHODIMP Text_To_Speech_Notify::Visual (QWORD , TCHAR ,
				TCHAR , DWORD , PTTSMOUTH )
{

   return NOERROR;
}

/*************************************************************************
Globals */

HINSTANCE         ghInstance;                // instance handle
PITTSCENTRALW     gpITTSCentral = NULL;
PITTSATTRIBUTES   attributes = NULL; 

Text_To_Speech_Buffer_Notify *notify_buffer_sink = NULL;
Text_To_Speech_Notify *notify_sink = NULL;

/*************************************************************************
FindAndSelect - This finds and selects according to the specific TTSMODEINFOW.

inputs
   PTTSMODEINFOW     pTTSInfo - desired mode
returns
   PITTSCENTRAL - ISRCentral interface to TTS engine
sets:

*/


PITTSCENTRALW FindAndSelect (PTTSMODEINFOW pTTSInfo, ascii_string mode_name) {
   HRESULT        hRes;
   TTSMODEINFOW        ttsResult;        // final result
//   WCHAR          Zero = 0;
   PITTSFINDW      pITTSFind = NULL;             // find interface
//#if !TT_SAPI_DIRECT_SOUND
   PIAUDIOMULTIMEDIADEVICE    pIAMM = NULL;      // multimedia device interface for audio-dest
//#endif
   PITTSCENTRALW  pITTSCentral = NULL;           // central interface
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
   hRes = CoCreateInstance(CLSID_TTSEnumerator, NULL, CLSCTX_ALL, IID_ITTSFindW, (void**) &pITTSFind);
   if (FAILED(hRes)) {
      tt_error_file() << "CoCreateInstance of TTSEnumerator failed causing text-to-speech to fail. Code=" << (unsigned int) hRes << endl;
      return NULL;
   };
   if (mode_name != NULL) {
//#if TT_UNICODE
//		_tcscpy(pTTSInfo->szModeName,mode_name);
//#else
      MultiByteToWideChar(tt_code_page[tt_use_spoken_alternative], 0, mode_name, -1, pTTSInfo->szModeName,
                          sizeof(pTTSInfo->szModeName)/sizeof(WCHAR));
//#endif
   }; 
   hRes = pITTSFind->Find(pTTSInfo, NULL, &ttsResult);

   if (hRes) {
      pITTSFind->Release();
      tt_error_file() << "Couldn't find " << mode_name << " causing text-to-speech to fail Code=" << (unsigned int) hRes << endl;
      return NULL;     // error
   }
#if TT_DEBUG_ON
	add_to_log_if_interesting_error(); // error 18 is OK
#endif
//#if TT_SAPI_DIRECT_SOUND
  if (tt_tts_uses_direct_sound) {
// Get the audio dest
   PIAUDIODIRECT pIAD;
	hRes = CoCreateInstance (CLSID_AudioDestDirect, NULL, CLSCTX_ALL, IID_IAudioDirect, (void**)&pIAD);
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
   if (hRes) {
      pITTSFind->Release();
      return NULL;     // worth falling back on old MultiMediaDevice??
   };
   // tell the audio object about our stuff
	LPDIRECTSOUND lpDirectSound = pointer_to_direct_sound();
// commented out the following after receiving this message from Mike Rozak at Microsoft
// That's because some TTS engines don't properly release the audio object. If
// the multimedia audio object is left in limbo then it's a minor memory leak.
// If direct sound is left in limbo it hangs.
// lpDirectSound->AddRef();
	hRes = pIAD->Init ((PVOID) lpDirectSound, IID_IDirectSound);
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	hRes = pIAD->AddRef();
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
//		hRes = lpDirectSound->SetCooperativeLevel (tt_main_window->get_handle(), DSSCL_NORMAL); // new on 140301 but not clear it does anything useful
	hRes = pITTSFind->Select(ttsResult.gModeID, &pITTSCentral, (LPUNKNOWN) pIAD); //pIAMM);
//	ITTSEnumW *pITTSEnum = NULL;
//	hRes = CoCreateInstance(CLSID_TTSEnumerator, NULL, CLSCTX_ALL, IID_ITTSEnumW, (void**) &pITTSEnum);
//#if TT_DEBUG_ON
//	add_to_log_if_interesting_error();
//#endif
//	hRes = pITTSEnum->Select(ttsResult.gModeID, &pITTSCentral, (LPUNKNOWN) pIAD);
//#if TT_DEBUG_ON
//	add_to_log_if_interesting_error();
//#endif
//	hRes = pITTSEnum->Release();
//SetPriorityClass(GetCurrentThread(),REALTIME_PRIORITY_CLASS);
//SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_HIGHEST);
//#else
  } else {
// Get the audio dest
		hRes = CoCreateInstance(CLSID_MMAudioDest, NULL, CLSCTX_ALL, IID_IAudioMultiMediaDevice, (void**)&pIAMM);
		if (hRes)
			{
			pITTSFind->Release();
			tt_error_file() << "CoCreateInstance of MMAudioDest failed causing text-to-speech to fail. Code=" << (unsigned int) hRes << endl;
			return NULL;     // error
			}
		hRes = pIAMM->DeviceNumSet (WAVE_MAPPER);
		// Pass off the multi-media-device interface as an IUnknown (since it is one)
		hRes = pITTSFind->Select(ttsResult.gModeID, &pITTSCentral, (LPUNKNOWN) pIAMM);
//#endif
  };
#if TT_DEBUG_ON
  int last_error = GetLastError();
  if (last_error == 6) { // 6 is expected for some engines since pITTSCentral is Wide
	  set_previous_last_error(6);
  } else if (interesting_error(last_error)) { 
	  tt_error_file() << "A new error selecting a TTS. Error code is " << last_error << endl;
  };
//	add_to_log_if_interesting_error();
#endif
   if (hRes) {
      tt_error_file() << "Select of TTS failed causing text-to-speech to fail. Code=" << (unsigned int) hRes << endl;
      pITTSFind->Release();
//    if (hRes == AUDERR_NODRIVER || hRes == TTSERR_WAVEFORMATNOTSUPPORTED) {
			trouble_shoot(TROUBLE_SHOOT_TTS_SELECT_FAILURE,NULL,NULL,TRUE);
//		};
      return NULL;
   };
   if (tt_debug_mode != 60198) { // just for debugging 
		hRes = pITTSCentral->QueryInterface(IID_ITTSAttributes, (VOID **) &attributes);
		if (attributes != NULL) hRes = attributes->VolumeSet(TTSATTR_MAXVOLUME);
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
// free random stuff up
   hRes = pITTSFind->Release();
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
   hRes = notify_sink->AddRef();
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	hRes = pITTSCentral->Register((void*) notify_sink, IID_ITTSNotifySinkW, &notify_sink_registration_key); // added W on 150301
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
   if (hRes != NOERROR) {
      log(_T("Problems registering text to speech notification sink."),FALSE,TRUE);
		switch (hRes) {
			case TTSERR_INVALIDINTERFACE:
				log("Invalid interface to text-to-speech engine.");
				break;
			case E_INVALIDARG:
				log("Text-to-speech engine wouldn't accept an argument.");
				break;
		};
		release_text_to_speech(pITTSCentral); // new on 150301
		log("Sorry but turning off speech - try again with a different voice.");
		return(NULL);
   };
	tt_limited_text_to_speech = !(ttsResult.dwFeatures&TTSFEATURE_ANYWORD);
   return pITTSCentral;
}


/*************************************************************************
BeginOLE - This begins the OLE.

inputs
   none
returns
   BOOL - TRUE if is succeeds
*/

BOOL BeginOLE() {
   if (ole_started) return(TRUE);
   DWORD    dwVer;
// Initialize OLE
   SetMessageQueue(96); // is this stuff still needed? -- asked 311204
#if TT_DEBUG_ON
	add_to_log_if_interesting_error(0,"BeginOLE after SetMessageQueue");
#endif
   dwVer = CoBuildVersion();
#if TT_DEBUG_ON
	add_to_log_if_interesting_error(0,"BeginOLE after CoBuildVersion");
#endif
   if (rmm != HIWORD(dwVer)) return FALSE;         // error
   if (!initialize_ole()) { // updated 311204 // !tt_coinitialized && FAILED(CoInitialize(NULL))) {
		return FALSE;
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error(126,"BeginOLE after CoInitialize"); // not clear why 126 crops up here
#endif
//	tt_coinitialized = TRUE; // new on 020401 commented out 311204
   ole_started = TRUE;
   return TRUE;
};

// commented out on 311204 since no longer called
//BOOL EndOLE() {
//// Free up all of OLE
//   CoUninitialize();
//   ole_started = FALSE;
//	tt_coinitialized = FALSE; // new on 020401
//   return TRUE;
//}

flag turned_off_direct_sound = FALSE;

wide_string saved_to_say = NULL;

wide_string return_saved_to_say() {
   return(saved_to_say);
};

long saved_id = 0;

long return_saved_id() { // new on 121102
   return(saved_id);
};

#if TT_SPEAK_ALL_NUMBERS

wide_string copy_and_process_for_text_to_speech(wide_string wide_text, int text_length) { // abstracted on 071104
	if (wide_text == NULL) return(NULL);
	if (text_length < 0) {
		text_length = wide_string_length(wide_text);
	};
	if (text_length > 10000000) { // new on 091104
		// way too expensive and may end up filling up the heap -- note that this is called after turning a number into a name
		return(copy_wide_string(wide_text,text_length));
	};
	wide_string result = new wide_character[text_length*24]; 
	// I believe that expansion will always be less than 24 times since every 3 digits produces a name 
	// (long ones seem to be 40 letters long) and the 3 digits and spaces
	// the 37th Mersenne prime is 909527 digits and its name takes 17281190 characters which is 19 times; 
	// the next prime expands by 20.3
	// 2 to the 10,000,000 is 3010302 and its name is 62,223,235 characters long (expands by 20.6)
//	DWORD start = timeGetTime(); // for now 111104
	process_for_text_to_speech(wide_text,text_length,result);
//	tt_err_file << "Took " << (timeGetTime()-start) << "ms to process " << text_length << " characters." << endl;
	return(result); // caller may want to trim this since its size is an upper bound
};
#endif

boolean speak_after_preprocessing(wide_string wide_text, int text_length) {
	// turned into a procedure on 100800
	if (wide_text != NULL) {
		if (tt_limited_text_to_speech) { // new on 210300 so text is processed even for text-to-speech sound objects
			string text = copy_narrow_string(wide_text,text_length);
			string replacement = process_for_text_to_speech(text,text_length);
			wide_string wide_replacement = copy_wide_string(replacement);
			speak(wide_replacement,-1);
			delete [] text;
			delete [] wide_replacement;
		} else if (wide_text[0] != NULL) { // condition new on 140405 since speaking "" was OK but then deleting it caused problems
#if TT_SPEAK_ALL_NUMBERS
			// new on 061104 -- loosely based upon old process_for_text_to_speech code
			wide_string to_speak = copy_and_process_for_text_to_speech(wide_text,text_length);
			speak(to_speak);
			delete [] to_speak;
#else
			speak(wide_text);
#endif
		};
		return(TRUE);
	} else {
		return(FALSE);
	};
};

flag spoken_successfully = FALSE;

boolean speak(wide_string text, long id) { // changed to boolean on 110401 so it returns FALSE if unable to speak
#if TT_DEBUG_ON
	if (tt_debug_mode == 60998) {
		tt_error_file() << "Started speaking: " << timeGetTime() << endl;
	};
#endif
   if (text == NULL || text[0] == NULL) return(FALSE); // second disjunct new on 110505 in case text is ""
   if (gpITTSCentral == NULL) turn_on_speech_and_sound_off();
   if (gpITTSCentral == NULL) return(FALSE);
   if (cant_start_speech) return(FALSE);
#if TT_DEBUG_ON
	if (tt_debug_mode == 60998 || tt_debug_mode == 111203) {
		tt_error_file() << "Started at " << timeGetTime() << " id is " << id << " " << copy_narrow_string(text) << endl;
	};
#endif
   if (id < 0) speaks_in_progress++;
   paragraph_id = id;
   SDATA data;
//#if TT_WIDE_TEXT
//	data.dwSize = (DWORD) (wcslen(text)+1)*sizeof(wide_character); // added plus one on 180799 to include the terminator
	int length = wcslen(text);
	boolean delete_original_text_remaining_to_speak = FALSE;
	if (length > max_speech_chunk) { 
		// new on 121104 to program around the limits of most text-to-speech engines -- 
		// some can take a very long time when given a long string
		int text_break = max_speech_chunk;
		while (text_break < length) {
			if (text[text_break] == L' ' && !(is_digit(text[text_break-1]) && text_break+1 < length && 
				 IsCharAlphaW(text[text_break+1]))) {
				// break at the first space that doesn't separate some digits from some words (e.g. "23 million")
				break;
			};
			text_break++;
		};
		if (text_break < length) { // broke at a space
//			text[text_break] = NULL; // seems some engines want the string to be terminated -- restored by AudioStop
			if (text_remaining_to_speak == text) { // working on a subsequent chunk already
				text_remaining_to_speak = text+text_break+1;
			} else {
				// maintain a pointer to original_text_remaining_to_speak to delete the memory afterwards
				if (original_text_remaining_to_speak != NULL) { // maybe am speaking while speaking is already in progress
					delete [] original_text_remaining_to_speak;
				};
				original_text_remaining_to_speak = copy_wide_string(text+text_break+1,(length-(text_break+1)));
				text_remaining_to_speak = original_text_remaining_to_speak;
			};
			length = text_break;
		} else {
			text_remaining_to_speak = NULL;
		};
	} else if (text_remaining_to_speak != NULL) {
		delete_original_text_remaining_to_speak = TRUE;
		text_remaining_to_speak = NULL;
	};
	data.dwSize = (DWORD)(length+1)*sizeof(wide_character);
//	WCHAR wszSpeak[1024];
//	memcpy(wszSpeak,text,data.dwSize);
//	wszSpeak[data.dwSize] = 0;
//	data.pData = wszSpeak;
	data.pData = text; 
//#else
//	WCHAR wszSpeak[1024]; // why is this enough? - not used anymore anyway
//	data.dwSize = (DWORD) MultiByteToWideChar(tt_code_page[tt_use_spoken_alternative], 0, text, -1, wszSpeak,
//															sizeof(wszSpeak) / sizeof(WCHAR)) * sizeof(WCHAR);
//   data.pData = wszSpeak;
//#endif
//#if TT_DEBUG_ON
//	tt_error_file() << "Size is " << data.dwSize/2 << endl;
//	for (DWORD i = 0; i < data.dwSize/2+1; i++) {
//		tt_error_file() << (char) text[i];
//	};
//	tt_error_file() << endl;
//#endif
   HRESULT result = gpITTSCentral->TextData (CHARSET_TEXT, TTSDATAFLAG_TAGGED ,
                                             data, notify_buffer_sink,
                                             IID_ITTSBufNotifySinkW);
   switch (result) {
      case NOERROR:
         turned_off_direct_sound = FALSE;
         spoken_successfully = TRUE;
			tt_paragraph_id_just_finished = -1; // in case just finished and am now speaking something new -- new on 031203
			if (delete_original_text_remaining_to_speak) {
				delete [] original_text_remaining_to_speak;
				original_text_remaining_to_speak = NULL;
			};
         return(TRUE);
      case E_FAIL:
      case E_INVALIDARG:
      case E_OUTOFMEMORY:
      case TTSERR_INVALIDCHAR:
      case TTSERR_QUEUEFULL:
         turned_off_direct_sound = FALSE;
         log(_T("Text to speech engine returned error code:"),FALSE,TRUE);
			tt_error_file() << (long) result << endl;
         break;
      case TTSERR_WAVEDEVICEBUSY:
         if (!turned_off_direct_sound) {
//            release_direct_sound();
            turn_off_direct_sound();
            turned_off_direct_sound = TRUE;
            if (saved_to_say != NULL) delete [] saved_to_say;
            saved_to_say = copy_wide_string(text);
            saved_id = id;
            tt_global_picture->do_after(1000,NULL,SPEAK_CALLBACK);
//            speak(szSpeak,id);
         } else if (!spoken_successfully) {
            log(_T("Turning off direct sound didn't help speech engine. Reverting to talk balloons.")); // give up on speech
				if (tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES) {
					tt_marty_talk = MARTY_WITH_SUBTITLES;
				} else {
					tt_marty_talk = MARTY_TALK_BALLOONS;
				};
            cant_start_speech = TRUE;
            turn_on_direct_sound();
         } else { // try again and wait another second
            turn_off_direct_sound();
            tt_global_picture->do_after(1000,NULL,SPEAK_CALLBACK);
         };
         break;
   };
	return(TRUE); // at least optimistically so since may have been postponed
};

boolean speak(string text, long id) { // changed to boolean on 110401 so it returns FALSE if unable to speak
#if TT_DEBUG_ON
	if (tt_debug_mode == 60998) {
		tt_error_file() << "Started speaking: " << timeGetTime() << endl;
	};
#endif
   if (text == NULL) return(FALSE);
   if (gpITTSCentral == NULL) turn_on_speech_and_sound_off();
   if (gpITTSCentral == NULL) return(FALSE);
   if (cant_start_speech) return(FALSE);
#if TT_DEBUG_ON
	if (tt_debug_mode == 60998) {
		tt_error_file() << "Started at " << timeGetTime() << " " << text << endl;
	};
#endif
   if (id < 0) speaks_in_progress++;
   paragraph_id = id;
	WCHAR wszSpeak[1024];
   SDATA data;
   data.dwSize = (DWORD)
      MultiByteToWideChar(CP_ACP, 0, text, -1, wszSpeak,
                          sizeof(wszSpeak) / sizeof(WCHAR)) * sizeof(WCHAR);
   data.pData = wszSpeak;
   HRESULT result = gpITTSCentral->TextData (CHARSET_TEXT, TTSDATAFLAG_TAGGED, // TTSDATAFLAG_TAGGED added 020999
                                             data, notify_buffer_sink,
                                             IID_ITTSBufNotifySinkW);
   switch (result) {
      case NOERROR:
         turned_off_direct_sound = FALSE;
         spoken_successfully = TRUE;
         return(TRUE);
      case E_FAIL:
      case E_INVALIDARG:
      case E_OUTOFMEMORY:
      case TTSERR_INVALIDCHAR:
      case TTSERR_QUEUEFULL:
         turned_off_direct_sound = FALSE;
         log(_T("Text to speech engine returned error code:"),FALSE,TRUE);
			tt_error_file() << (long) result << endl;
         break;
      case TTSERR_WAVEDEVICEBUSY:
         if (!turned_off_direct_sound) {
//            release_direct_sound();
            turn_off_direct_sound();
            turned_off_direct_sound = TRUE;
            if (saved_to_say != NULL) delete [] saved_to_say;
            saved_to_say = copy_wide_string(text);
            saved_id = id;
            tt_global_picture->do_after(1000,NULL,SPEAK_CALLBACK);
//            speak(szSpeak,id);
         } else if (!spoken_successfully) {
            log(_T("Turning off direct sound didn't help speech engine. Reverting to talk balloons.")); // give up on speech
            if (tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES) {
					tt_marty_talk = MARTY_WITH_SUBTITLES;
				} else {
					tt_marty_talk = MARTY_TALK_BALLOONS;
				};
            cant_start_speech = TRUE;
            turn_on_direct_sound();
         } else { // try again and wait another second
            turn_off_direct_sound();
            tt_global_picture->do_after(1000,NULL,SPEAK_CALLBACK);
         };
         break;
   };
	return(TRUE);
};

boolean speak(int text_id) { // abstracted on 081002
   if (alternative_language_spoken()) {
      return(speak(S(text_id,"",TRUE))); // new on 081002
   } else {
		return(speak(S(text_id))); // new on 240902
   };
};

void stop_speaking() {
	if (original_text_remaining_to_speak != NULL) {
		// don't go on to the next chunk
		delete [] original_text_remaining_to_speak;
		original_text_remaining_to_speak = NULL;
		text_remaining_to_speak = NULL;
	};
   if (gpITTSCentral != NULL && gpITTSCentral->AudioReset() != NOERROR) {
      log(_T("Error trying to do AudioReset."),FALSE,TRUE);
   };
   speaks_in_progress = 0;
};

BOOL initialize_text_to_speech() {
  if (tt_sound_option == 0) return(FALSE); // no point trying since sound has been turned off
  if (gpITTSCentral != NULL) return(TRUE);
  if (about_to_quit()) return(FALSE);
#if TT_DEBUG_ON
	add_to_log_if_interesting_error(0,"initialize_text_to_speech");
#endif
  ascii_string speaker = ini_entry(AC(IDC_USER),AS(IDS_TEXT_TO_SPEECH_MODE),FALSE); // FALSE added on 120100 since should be able to switch voices 
  // and also this can get called during titles or at first use and that screws up segmented logs
  if (speaker == NULL) {
//	  if (ini_int(AC(IDC_VERSIONS),AS(IDS_TEXT_TO_SPEECH),FALSE)) { // obsolete so commented out on 140404
////		  speaker = ASC(IDS_SPEECH_ENGINE_VOICE);
//		  speaker = copy_ascii_string(S(IDS_SPEECH_ENGINE_VOICE,"",alternative_language_spoken()));
//	  };
     if (speaker == NULL || speaker[0] == NULL) { // there is no speaker for this language
        if (tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES) {
				tt_marty_talk = MARTY_WITH_SUBTITLES;
			} else {
			  tt_marty_talk = MARTY_TALK_BALLOONS;
			};
         cant_start_speech = TRUE;
         return(FALSE);
     };
  };
  TTSMODEINFOW   ModeInfo;
#if TT_DEBUG_ON
	add_to_log_if_interesting_error(0,"initialize_text_to_speech before beginning OLE");
#endif
    if (!BeginOLE()) {  // try to begin OLE -- is this redundant as of 260604?
      log(_T("Can't create OLE."),FALSE,TRUE);
      if (tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES) {
			tt_marty_talk = MARTY_WITH_SUBTITLES;
		} else {
			tt_marty_talk = MARTY_TALK_BALLOONS;
		};
      cant_start_speech = TRUE;
      return(FALSE);
   }
//   unsigned char dll_path[256];
//   unsigned long path_size;
//   long result = RegOpenKeyEx(HKEY_CLASSES_ROOT,
//                               "CLSID\\{2a46E4C0-4EDA-101B-931A-00AA0047BA4F}\\InprocServer32",
//                                NULL,KEY_READ,&key);
//   if (result == ERROR_SUCCESS) {
//      DWORD dwType;
//      result = RegQueryValueEx(key,NULL,&dwType,NULL,dll_path,&path_size);
//   };
//   RegCloseKey(key);
//   if (result != ERROR_SUCCESS) {
//      log("Problem finding text to speech engine");
//      tt_marty_talk = MARTY_TALK_BALLOONS;
//      return(FALSE);
//   };
#if TT_DEBUG_ON
	add_to_log_if_interesting_error(0,"initialize_text_to_speech before creating notify sink");
#endif
   if (notify_sink == NULL) {
      notify_sink = new Text_To_Speech_Notify();
   };
// find the right object
   memset(&ModeInfo, 0, sizeof(ModeInfo));
   gpITTSCentral = FindAndSelect(&ModeInfo, speaker);
   if (speaker != NULL) delete [] speaker;
   if (!gpITTSCentral) {
      log(S(IDS_PROBLEM_STARTING_TEXT_TO_SPEECH_ENGINE));
//			 FALSE,!did_user(TROUBLE_SHOOT_TTS_SELECT_FAILURE,TROUBLE_SHOOT_HISTORY));
		// updated on 040205 since not a ToonTalk error but important anyway
		tt_err_file_important_but_no_error = !did_user(TROUBLE_SHOOT_TTS_SELECT_FAILURE,TROUBLE_SHOOT_HISTORY);
		log(S(IDS_SET_OPTIONS_TO_TURN_OFF_SPEECH));
      if (tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES) {
			tt_marty_talk = MARTY_WITH_SUBTITLES;
		} else {
			tt_marty_talk = MARTY_TALK_BALLOONS;
		};
      cant_start_speech = TRUE;
      return(FALSE);
   };
   if (notify_buffer_sink == NULL) {
      notify_buffer_sink = new Text_To_Speech_Buffer_Notify();
   };
	// in starttt.cpp I use SAPI to find out how general -- better isn't it??
//	tt_limited_text_to_speech = (ini_int(AC(IDC_SWITCHES),AC(IDC_GENERAL_TEXT_TO_SPEECH_ENGINE),FALSE) == 0);
	// commented out on 150301 since I now find out from the voice itself
   return(TRUE);
};

void reset_text_to_speech() {
   if (gpITTSCentral != NULL) gpITTSCentral->AudioReset();
};

void release_text_to_speech(PITTSCENTRALW tts_central) { // optional arg added on 150301
	if (tts_central != NULL) {
		gpITTSCentral = tts_central;
	};
   if (gpITTSCentral == NULL) return; // new on 070802 since otherwise interferes with release_direct_sound
	HRESULT result;
   if (gpITTSCentral != NULL) {
		if (attributes != NULL) result = attributes->Release();
      if (notify_sink_registration_key != 0xFFFFFFFF) {
			result = gpITTSCentral->UnRegister(notify_sink_registration_key);
		};
      result = notify_sink->Release();
#if TT_DEBUG_ON
		if (tt_debug_mode != 300400)
			// a work around to the following crashing - for now
#endif
      result = gpITTSCentral->Release();
      gpITTSCentral = NULL;
//      speech_in_progress_flag = FALSE;
      if (notify_buffer_sink != NULL) {
         delete notify_buffer_sink;
         notify_buffer_sink = NULL;
      };
      if (notify_sink != NULL) {
         delete notify_sink;
         notify_sink = NULL;
      };
   };
// try to close ole
//   if (!EndOLE()) log(S(IDS_CAN_SHUT_DOWN_OLE),FALSE,TRUE); // commented out on 260604 since used now for HTML dialogs as well
	if (!about_to_quit() && tt_martian != NULL) { // new on 130700 since pause now toggles speech
		tt_martian->remove_text(FALSE);
	};
};

flag sound_on = TRUE;

void set_sound_on(flag new_flag) {
   sound_on = new_flag;
};

flag speech_on = FALSE;

flag return_speech_on() {
   return(speech_on);
};

void turn_on_sound_and_speech_off(millisecond delay) {
//#if !TT_SAPI_DIRECT_SOUND
  if (!tt_tts_uses_direct_sound) {
 // not needed in SAPI 3.0
		if (!cant_start_speech && tt_sound_option&SOUND_BIT) {
			if (speech_on) {
				if (delay == 0) {
					reset_text_to_speech();
					speech_on = FALSE;
				} else {
					tt_global_picture->do_after(delay,NULL,TURN_ON_SOUND_AND_SPEECH_OFF_CALLBACK);
					return;
				};
			};
			if (!sound_on) { // wait a bit for text-to-speech to really release
				const millisecond wait = 1000;
				tt_global_picture->do_after(wait,NULL,TURN_ON_DIRECT_SOUND_CALLBACK);
//          sound_on = TRUE;    // callback does it now
			};
		};
  };
//#endif
};

void turn_on_speech_and_sound_off() {
   if (!cant_start_speech) {
//#if !TT_SAPI_DIRECT_SOUND
    if (!tt_tts_uses_direct_sound) {
       if (sound_on && tt_sound_option&SOUND_BIT) {
          turn_off_direct_sound();
          sound_on = FALSE;
       };
    };
//#endif
       if (!speech_on) {
          if (initialize_text_to_speech()) {
             speech_on = TRUE;
          } else {
             turn_on_direct_sound();
             sound_on = TRUE;
          };
       };
   };
};

#endif
