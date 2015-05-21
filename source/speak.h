// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved

#ifndef __TT_SPEAK_H

#define __TT_SPEAK_H

#if TT_TEXT_TO_SPEECH

#if TT_MICROSOFT
#include <speech.h>
#else
// needed to edit the file for Borland C++
#include "speech.h"
#endif

//boolean speech_in_progress();

BOOL BeginOLE();

//BOOL EndOLE();

#if TT_SPEAK_ALL_NUMBERS
wide_string copy_and_process_for_text_to_speech(wide_string wide_text, int text_length);
#endif

boolean speak_after_preprocessing(wide_string wide_text, int text_length);
boolean speak(wide_string text, long id=max_long);
boolean speak(string text, long id=max_long); // uncommented on 240902 -- why was this commented out?
boolean speak(int text_id);
void stop_speaking();

BOOL initialize_text_to_speech();
void reset_text_to_speech();
void release_text_to_speech(PITTSCENTRALW tts_central=NULL);

void turn_on_sound_and_speech_off(millisecond delay=0);
void turn_on_speech_and_sound_off();

wide_string return_saved_to_say();
long return_saved_id();
void set_sound_on(flag new_flag);
flag return_speech_on();

#endif

#endif