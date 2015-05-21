// Copyright (c) 1992-2006.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)
#include "defs.h"
#endif
#if !defined(__TT_STRINGS_H)
#include "strings.h"
#endif
#if !defined(__TT_SPRITE_H)
#include "sprite.h"
#endif
#if !defined(__TT_FLOOR_H)
#include "floor.h"
#endif
#if !defined(__TT_PAD_H)
#include "pad.h"
#endif
#if !defined(__TT_EVENT_H)
#include "event.h"
#endif
#if !defined(__TT_ROBOT_H)
#include "robot.h"
#endif
#if !defined(__TT_UTILS_H)
#include "utils.h" // for sound stuff
#endif
#if !defined(__TT_COMMON_H)
#include "common.h" // for copy_wide_string
#endif
#if !defined(__TT_NUMBER_H)
#include "number.h"
#endif
#if !defined(__TT_MOUSE_H)
#include "mouse.h"
#endif
#if !defined(__TT_TEXT_H)
#include "text.h"
#endif
#if !defined(__TT_SOUND_H)
#include "sound.h"
#endif
#if !defined(__TT_HELP_H)
#include "help.h"
#endif
#if !defined(__TT_JAVA_H)
#include "java.h"
#endif
#if !defined(__TT_SPEAK_H)
#include "speak.h"
#endif
#if !defined(__TT_MARTIAN_H)
#include "martian.h"
#endif
#if !defined(__TT_PUZZLE_H)
#include "puzzle.h"
#endif
#if TT_DIRECT_INPUT
#if !defined(__TT_INPUT_H)
#include "input.h"
#endif
#endif
#if TT_XML
#if !defined(__TT_XML_H)
#include "xml.h"
#endif
#endif
#if !defined(__TT_WINMAIN_H)
#include "winmain.h" // for simulate_mouse_up
#endif

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Sprite *Sound::respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected, Sprite *by,
										     boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless) {
   if (record_action) { // commented out on 270499 since interferred with re-training && by != NULL && !by->still_running()) {
      record_event(KEYBOARD_CHARACTER,by,floor,this,TRUE,key);
		record_action = FALSE;
		simulate_mouse_up();
	};
	if (!extended) { // extended test new on 010704 since Delete caused pad to be stopped (treated like '.')
		if (key == tt_run_character) {
			if (!ok_to_make_a_sound(floor)) { // moved into conditional on 180204 since might not be typing run character
				// in another house -- good policy to make no sound?
				by->completed();
				return(this);
			};
			make_sound();
			user_did(TEXT,TEXT_SOUNDED,by); // moved from obsolete commented out code on 161204
//			completed(action_status); // commented out on 080400 since Sprite::respond_to_keyboard does this
		} else if (key == tt_stop_character) { // new on 190799
			stop_making_sound();
//			completed(action_status); // commented out on 080400 since Sprite::respond_to_keyboard does this
//		} else if (tt_log_version_number < 15) { // old behavior
//			make_sound();
//			completed(action_status);
			// following is a good idea for localizing sounds but it is a bigger job than I thought
//		} else { // restored on 180204 -- rewritten on 050304
			// new policy to allow you to change the names of sounds -- even text-to-speech?
//			Text::respond_to_keyboard(key,extended,robot_selected,by,record_action,size_constrained_regardless); // new on 160200
		};
	};
	if (held()) { // condition new 050304 so when held can change the text (including space and period)
		return(Text::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,
													size_constrained_regardless)); 
	} else {
		// updated the following on 080400
		return(Sprite::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,
													  size_constrained_regardless));
	};
};

Sprite *Sound::used(Sprite *, Sprite *by, boolean , boolean record_action,
                    millisecond , boolean , Sprite *original_subject) {
    return(respond_to_keyboard(tt_run_character,FALSE,FALSE,by,record_action,original_subject));
};

//void Sound::prepare_for_deletion(Sprite *by) { // completedly commented out on 060204
////   stop_making_sound(); 
// commented out on 121000 since should finish the sound (e.g. explosion) and the sound buffer is now shared
//   Text::prepare_for_deletion(by);
//};

void Sound::generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection) {
	// on 120699 replaced did(TEXT_BLANKED_INSIDE_THOUGHT_BUBBLE) with 
	// did_user(TEXT_BLANKED_INSIDE_THOUGHT_BUBBLE,TEXT) since TEXT is where the info is stored
	if (blank) {
		// could say more if done this before
		if ((contained_in(THOUGHT_BUBBLE) && !did_user(TEXT_BLANKED_INSIDE_THOUGHT_BUBBLE,TEXT) && 
			 !did_user(TEXT_BLANK,TEXT)) ||
			 tt_help_counter == 1) {
			 describe_pointing(message,tool_in_hand,selection);
		};
		if ((!did_user(TEXT_BLANKED_INSIDE_THOUGHT_BUBBLE,TEXT) && tt_help_counter == 0)
		    || tt_help_counter == 1) {
			message << S(IDS_TEXT_BLANKED_TB_HELP2)
					  << S(IDS_TEXT_BLANKED_TB_HELP3);
		};
		return;
	 } else if ((!did_user(TEXT_SOUNDED,TEXT) && tt_help_counter == 0 &&
					tt_sound_option&SOUND_BIT)
					|| tt_help_counter == 6) {
		tt_help_history_action = TEXT_SOUNDED;
		message << S(IDS_SOUND_HELP);
		describe_right_button(message);
		message << S(IDS_SOUND_HELP2,"") << end_sentence();
		message << S(IDS_SOUNDS_ON_PAGE8);
	 };
};

// commented out on 101202 since doesn't do anything
//boolean Sound::receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient) {
//   return(Text::receive_item(item,by,duration,original_recipient)); // ignore it -- was Sprite:: prior to 140301
//};

// commented out on 101202 since doesn't do anything
//ReleaseStatus Sound::add_to_side(boolean right_side, Sprite *item,
//										   Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item) {
//   return(Text::add_to_side(right_side,item,by,duration,original_recipient,original_item)); // ignore it -- was Sprite:: prior to 140301 but that made an infinite loop if a user sound is dropped on another
//};

void Sound::java(java_stream &out) {
   if (blank) {
      out << S(IDS_JAVA_TT_BLANK_SOUND);
   } else {
      java_new_quote_arg1(out,S(IDS_JAVA_TT_SOUND),_T("unknown.au"),_T("\"UNKNOWN\""));
   };
};

void Sound::set_inside_vacuum(boolean new_flag, boolean recur) {
  	boolean changed = (!!inside_vacuum_flag != !!new_flag); // force to true boolean before comparing
   Text::set_inside_vacuum(new_flag,recur);
   if (new_flag && changed) stop_making_sound();
};

WaveSound::WaveSound(city_coordinate llx, city_coordinate lly,
			            wide_string text, long text_length,
		               long priority,
			            city_coordinate width,
			            city_coordinate height,
//                     boolean ok_to_reuse_initial_text,
                     color_index text_color) :
      Sound(llx,lly,text,text_length,priority,width,height,text_color),
		sound_file_name(NULL),
		sound_buffer_table_index(-1),
		sound(NULL),
		sound_hash(NULL) {
	initialize_sound(text);
//#if TT_POST327
//	change_font_size_to_fit_extent_flag = TRUE; // new on 161103 so if pasted computes a good size
//	change_size_to_fit_extent_flag = FALSE;
//#endif
//	if (text != NULL) Sound::set_wide_text(short_file_name(text)); // commented out on 080401
};

void WaveSound::initialize_sound(wide_string text) {
	if (sound_file_name != NULL) return; // new on 180204 so that if already initialized don't do this again
#if TT_DIRECTX
   if (text != NULL) {
		// text+5 changed to text here on 240599
		string narrow_text = NULL; // moved out here on 311000 - not clear how it could have been deleted below??
		if (sound_file_name == NULL) {
			narrow_text = copy_narrow_string(text);
			FileNameStatus file_status;
			// is it OK that this might be a URL temporary cache file name?? - now it is
			string new_sound_file_name = existing_file_name(file_status,narrow_text,"wav","NewUsers",TRUE,FALSE);
			if (new_sound_file_name != NULL) {
				boolean different_file_name = (strncmp(new_sound_file_name,narrow_text,strlen(narrow_text)) != 0); // narrow_text might not include extension but that's OK
				sound_file_name = new_sound_file_name; // do this before the following since it can recursively call this method
				if (tt_maintaining_private_media_directory || (different_file_name && !is_url(narrow_text))) { // added !is_url on 110401 since that shouldn't change
					// new on 080401 since if narrow_text can't be found then just short version of narrow text is all that is being used
					// new on 250303 that if tt_maintaining_private_media_directory then use short version too
					string new_text = basic_file_name(narrow_text);
					if (strcmp(new_text,narrow_text) != 0) {
						boolean reset_sound_hash = (sound_hash == NULL);
						// the following will clober the sound_file_name unless sound_hash is non-NULL
						sound_hash = (hash) "temp"; // pretty ugly
						set_text(new_text);
						sound_hash = NULL;
					};
				};			
			};
      };
		if (sound_file_name == NULL) { // moved here from initialize_sound on 241199 so only when used is this work done (first time)
			say_file_name_error(IDS_NO_FILE_NAME_FOR_USER_MEDIA_FILE,narrow_text,4,"wav");
			sound = NULL;
//#if TT_DEBUG_ON
//		} else if (strchr(sound_file_name,'[') != NULL) {
//			log("debug this");
//#endif
		};
//		if (file_status == FILE_ON_INTERNET_AND_DELETE_COPY) DeleteFile(sound_file_name); // should I delete the string too??
		if (narrow_text != NULL) delete [] narrow_text;
   } else
#endif
      sound = NULL;
};

string WaveSound::return_dump_text() {
//	return(sound_file_name); // commented out 170200 this is wrong - can be temporary cache file name
	// added 170200 so this behaves like pictures
	if (tt_dump_reason == DUMPING_TO_CLIPBOARD && tt_mouse_button&SHIFT_KEY_DOWN) { // new on 160200 so control-shift-c saves short names
		// added tt_dumping_to_clipboard on 170200 since appropriate when saving in notebooks, etc.
		return(short_file_name(return_narrow_text()));
	} else {
		return(copy_narrow_text_string()); // restored on 080401 since if file wasn't found locally but was on FileSearchPath then text is the right value and if still local text should still be equal to sound_file_name
//		return(copy_string(sound_file_name)); // revised on 101000 so copy and paste on same machine works for sound not on FileSearchPath
	};
};

#if TT_XML

xml_character_data *WaveSound::xml_create_character_data(xml_element *element, xml_document *document) {
   if (tt_dump_reason == DUMPING_TO_CLIPBOARD && tt_mouse_button&SHIFT_KEY_DOWN) { // control-shift-c saves short names
		string short_text = short_file_name(return_narrow_text());
		if (short_text == NULL) return(NULL); // error handling new on 040704
      wide_string text = copy_wide_string(short_text);
      xml_character_data *result = create_xml_character_data(text,document);
      delete [] short_text;
      delete [] text;
      return(result);
	} else {
      return(Sound::xml_create_character_data(element,document)); // Text::xml_create_character_data can handle it fine
	};
};

void WaveSound::xml(xml_element *element, xml_document *document) { // new on 200303
	Text::xml(element,document);
	load_sound_and_generate_hash(); // new on 190803 in case needed
	if (sound_hash != NULL) { // conditional new on 240303
		string hash_string = hash_to_string(sound_hash);
		xml_set_attribute(element,L"UniqueName",hash_string);
		delete [] hash_string;
		if (tt_saving_media) {
			insert_at_end_if_not_member(this,tt_sprites_with_media_to_save);
		};
	};
};

#endif

void WaveSound::set_hash_string(string hash_string) {
	if (hash_string != NULL) {
		if (sound_file_name != NULL) {
			delete [] sound_file_name;
		};
		sound_file_name = new char[MAX_PATH];
		media_file_name(hash_string,".wav",sound_file_name,tt_private_media_directory,tt_private_media_directory_length);
		set_hash(string_to_hash(hash_string));
	};
};

string WaveSound::private_media_file_name(boolean &worth_compressing, boolean full_path) {
	if (sound_hash == NULL) return(NULL);
	string file_name = new char[MAX_PATH];
	if (full_path) {
		media_file_name(sound_hash,".wav",file_name,tt_private_media_directory,tt_private_media_directory_length);
	} else {
		media_file_name(sound_hash,".wav",file_name);
	};
	worth_compressing = TRUE; // until I start compressing these myself
	return(file_name);
};

void WaveSound::set_wide_text(wide_string text, int new_text_length,
										boolean text_was_updated,
										boolean size_constrained_regardless) {
	if (text == NULL) return; // for robustness - added 171199 -- warn??
//	Sound::set_wide_text(short_file_name(text),-1,text_was_updated,size_constrained_regardless);
	Sound::set_wide_text(text,new_text_length,text_was_updated,size_constrained_regardless);
	if (sound_file_name != NULL) return; // new on 180204
	if (sound != NULL) {
		// commented out since these are shared now
//#if TT_DIRECTX
//		release_sound_buffer(sound);
//#endif
		sound = NULL;
	};
	if (sound_file_name != NULL && sound_hash == NULL) { // new on 241199
		delete [] sound_file_name;
		sound_file_name = NULL;
	};
	sound_buffer_table_index = -1;
	initialize_sound(text); // added on 140699 so saved user sounds are restored right
//	delete [] text; // new on 311000 - since others save this while this guys saves a modified copy
};

Sprite *WaveSound::copy(boolean also_copy_offsets) {
   WaveSound *copy = new WaveSound(llx,lly,
											  NULL,0, // so this doesn't generate a spurious warning about a missing file - new on 241103
//						  	              wide_text,text_length, // restored on 080401 since wide_text is now either local file or short name
//											  NULL,0, // new on 140301
								           current_priority-1,
								           width,height,text_color);
	// commented out on 080401
//	if (sound_file_name != NULL) { // new on 140301 since file might not be on FileSearchPath
//		copy->set_text(sound_file_name);
//	};
	//if (blank && !tt_copy_restores_pre_blank) {
	//	copy->become_blank();
	//};
 //  copy->set_priority_fixed(priority_fixed_flag);
	//if (comment != NULL) {
	//	copy->set_comment(comment->copy());
	//};
	// somehow the code for copying the private media stuff for wave sounds was missing prior to 151103
	copy->set_sound_file_name(copy_string(sound_file_name));
	if (sound_hash != NULL) { // condition new on 091203 - Leonel had a crash about this
		hash hash_copy = new byte[hash_byte_length];
		memcpy(hash_copy,sound_hash,hash_byte_length);
		copy->set_hash(hash_copy);
	};
	copy->set_wide_text(copy_wide_string(wide_text,text_length),text_length); // on 251103 changed to copy the wide_string
   finish_making_copy(copy,also_copy_offsets);
	return(copy);
};

boolean WaveSound::make_sound() {
#if TT_DIRECTX
	load_sound_and_generate_hash();
   if (sound != NULL) {
		play_sound_buffer(sound);
		return(TRUE);
	} else if (sound_file_name != NULL) {
		// warn??
	   return(PlaySound(sound_file_name, NULL, SND_ASYNC|SND_FILENAME)); 
		// new on 031205 so that if can't play WAV format -- e.g. is MP3 interally - then play it this way
		// not default since too hard to stop just this sound without stopping all sounds
		// and is loaded from file each time
	};
#else
	if (wide_text != NULL) {
		tt_main_window->MCI_send_string(wide_text);
		return(TRUE);
	};
#endif
	return(FALSE);
};

void WaveSound::load_sound_and_generate_hash() { // abstracted on 190803
	if (sound_file_name != NULL) { // && sound == NULL) { 
		// commented out on 050100 since might have been flushed from cache - should be a quick check 
		// moved here from initialize_sound on 241199 so only when used is this work done (first time)
		sound = load_sound_from_file(sound_file_name,sound_buffer_table_index);
		if (sound_hash == NULL && sound != NULL && tt_maintaining_private_media_directory) { // new on 200303 -
			// should only happen first time loaded from a user file - otherwise sound_hash will be non-NULL
			// could someday deal with sound compression here
			sound_hash = new byte[hash_byte_length];
			string private_file_name = hash_and_copy_file(sound_file_name,".wav",sound_hash,FALSE,
				                                           tt_private_media_directory,tt_private_media_directory_length);
			if (private_file_name != NULL) {
				delete [] sound_file_name;
				sound_file_name = private_file_name;
			};
		};
   };
};

SpriteType WaveSound::dump_type() {
 	if (blank) {
      return(BLANK_USER_SOUND);
   } else {
      return(USER_SOUND);
   };
};

void WaveSound::stop_making_sound() {
#if TT_DIRECTX
	sound_buffer buffer = previously_loaded_sound(sound_buffer_table_index);
	if (buffer != NULL) stop_sound_buffer(buffer); // new on 210300 since this makes sure sound is still available first
//   if (sound != NULL) stop_sound(sound);
#endif
};

void WaveSound::release_all_references() { // renamed on 130504
	if (releasing_all_references) { // new on 230305
		return;
	};
   Sound::release_all_references();
//   release_sound_buffer(sound); // shared start 201199
   sound = NULL;
	if (sound_file_name != NULL) { // new on 311000
		delete [] sound_file_name;
	};
};

void WaveSound::java(java_stream &out) {
	if (blank) { // new on 221100
      out << S(IDS_JAVA_TT_BLANK_SOUND);
		return;
	};
   java_string text_string = java_text_value();
//	string file_name = copy_narrow_string(wide_text); // commented out on 230104
	if (sound_file_name == NULL) { // new on 100401
		log("While making a Java applet found a sound from a file whose name is empty. The sound is ignored.");
		Sound::java(out);
		return;
	};
   java_string au_file_name = au_file_for_java(sound_file_name); // on 240599 no longer skips over the "play" part -- 
	// sound_file_name changed from file_name on 230104 so user sounds work correctly in Java applets
//   delete [] narrow_text; // commented out on 080201 - makes no sense and broke repeated Java-izations
	if (au_file_name != NULL) { // there is a au equivalent so use that
      java_new_quote_arg1(out,S(IDS_JAVA_TT_SOUND),au_file_name,text_string);
      delete [] au_file_name;
   } else {
		tt_error_file() << "While generating Java applet couldn't generate the \"au\" version of " << sound_file_name << endl;
      Sound::java(out);
   };
//	delete [] file_name; // new on 031200 // commented out on 230104
};

// commented out on 071104 since does nothing
//boolean WaveSound::receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item) {
//	// added on 241199 so is changed just like text
//   return(Text::receive_item(item,by,duration,original_recipient,original_item));
//};

boolean MCISound::make_sound() {
   if (wide_text != NULL) {
		string narrow_text = copy_narrow_string(wide_text);
		tt_main_window->MCI_send_string(narrow_text);
		delete [] narrow_text;
		return(TRUE);
	};
	return(FALSE);
};

Sprite *MCISound::copy(boolean also_copy_offsets) {
   MCISound *copy = new MCISound(llx,lly,
						  	            wide_text,text_length,
                                 current_priority-1,
                                 width,height);
	//if (blank && !tt_copy_restores_pre_blank) {
	//	copy->become_blank();
	//};
 //  copy->set_priority_fixed(priority_fixed_flag);
	//if (comment != NULL) {
	//	copy->set_comment(comment->copy());
	//};
   finish_making_copy(copy,also_copy_offsets);
	return(copy);
};

SpriteType MCISound::dump_type() {
 	if (blank) {
      return(BLANK_MCI_SOUND);
   } else {
      return(MCI_SOUND);
   };
};

boolean SpeechSound::make_sound() {
#if TT_TEXT_TO_SPEECH
	return(speak_after_preprocessing(wide_text,text_length));
#endif
};

SpriteType SpeechSound::dump_type() {
 	if (blank) {
      return(BLANK_SPEECH_SOUND);
   } else {
      return(SPEECH_SOUND);
   };
};

void SpeechSound::stop_making_sound() {
#if TT_TEXT_TO_SPEECH
	if (!tt_shutting_down && showing_on_a_screen()) {
		stop_speaking();  // this stops all speaking not just this one...
	};
#endif
};

Sprite *SpeechSound::copy(boolean also_copy_offsets) {
   SpeechSound *copy = new SpeechSound(llx,lly,
						  	                  wide_text,text_length,
                                       current_priority-1,
                                       width,height);
	//if (blank && !tt_copy_restores_pre_blank) {
	//	copy->become_blank();
	//};
 //  copy->set_priority_fixed(priority_fixed_flag);
	//if (comment != NULL) {
	//	copy->set_comment(comment->copy());
	//};
   finish_making_copy(copy,also_copy_offsets);
	return(copy);
};

// commented out 071104 since does not
//ReleaseStatus SpeechSound::add_to_side(boolean right_side, Sprite *item,
//										         Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item) {
//   return(Text::add_to_side(right_side,item,by,duration,original_recipient,original_item)); // don't ignore it
//};

// commented out on 071104 since does nothing
//boolean SpeechSound::receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item) {
//	// added on 171199 so is changed just like text
//   return(Text::receive_item(item,by,duration,original_recipient,original_item));
//};

void SpeechSound::java(java_stream &out) { // new on 301100
   if (blank) {
		out << "TT.BLANK_TEXT_TO_SPEECH_SOUND"; // could put string in javacom.rc
   } else {
      java_string text_string = java_text_value();
      java_new(out,"TTTextToSpeechSound",text_string); // could put string in javacom.rc
      delete [] text_string;
   };
};

#if TT_SPEAK_ALL_NUMBERS

void SpeechSound::set_wide_text(wide_string text, int new_text_length,
										  boolean text_was_updated,
										  boolean size_constrained_regardless) {
	// new on 071104 to replace numbers with names
	if (text == NULL || new_text_length == 0 || text_was_updated) { // don't do this while typing (since too confusing)
		Sound::set_wide_text(text,new_text_length,text_was_updated,size_constrained_regardless);
		return;
	};
	wide_string new_text = copy_and_process_for_text_to_speech(text,new_text_length);
	int length_of_new_text = wide_string_length(new_text);
	if (length_of_new_text == new_text_length) { // no change
		if (length_of_new_text > 0) {
			delete [] new_text;
		};
		Sound::set_wide_text(text,new_text_length,text_was_updated,size_constrained_regardless);
	} else {
		wide_string trimmed_new_text = copy_wide_string(new_text,length_of_new_text);
		Sound::set_wide_text(trimmed_new_text,length_of_new_text,text_was_updated,size_constrained_regardless);
		delete [] new_text;
		delete [] text; // not needed and was being passed here to be kept by the speech sound
	};
};

#endif

Sprite *BuiltInSound::copy(boolean also_copy_offsets) {
   BuiltInSound *copy = new BuiltInSound(llx,lly,
						  	                    wide_text,text_length,
								                 current_priority-1,
								                 width,height);
	//if (blank && !tt_copy_restores_pre_blank) {
	//	copy->become_blank();
	//};
   copy->set_index(index);
 //  copy->set_priority_fixed(priority_fixed_flag);
	//if (comment != NULL) {
	//	copy->set_comment(comment->copy());
	//};
   finish_making_copy(copy,also_copy_offsets);
	return(copy);
};

boolean BuiltInSound::make_sound() {
	play_sound(index,FALSE);
	return(TRUE);
};

SpriteType BuiltInSound::dump_type() {
 	if (blank) {
      return(BLANK_BUILT_IN_SOUND);
   } else {
      return(BUILT_IN_SOUND);
   };
};

boolean BuiltInSound::dump(output_stream &stream, boolean just_prepare, boolean is_controlee, boolean on_nest) {
   if (!Text::dump(stream,just_prepare,is_controlee)) return(FALSE); // puts right type 
   stream.write((string) &index, sizeof(index));
	return(TRUE);
};

#if TT_XML

void BuiltInSound::xml(xml_element *element, xml_document *document) {
//   xml_element *element = Text::xml_create_element(document);
	Text::xml(element,document);
   xml_set_attribute(element,L"Index",index);
//   return(element);
};

boolean BuiltInSound::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
		case NO_MORE_TAGS: // was BUILT_IN_SOUND_TAG: prior to 070103
         set_index(xml_get_attribute_int(node,L"Index",0));
//         return(TRUE); // and the following (as of 070103)
      default:
         return(Text::handle_xml_tag(tag,node));
   };
};
#endif

void BuiltInSound::java(java_stream &out) {
   if (blank) {
      out << S(IDS_JAVA_TT_BLANK_SOUND);
      return;
   };
   java_string file_name = ensure_file_for_java(index,kind_of());
   if (file_name == NULL) {
      Sound::java(out); // java_new_quote_arg1(out,S(IDS_JAVA_TT_SOUND),"unknown.au","\"UNKNOWN\"");
   } else {
      java_string text_string = java_text_value();
      java_new_quote_arg1(out,S(IDS_JAVA_TT_SOUND),file_name,text_string);
      delete [] text_string;
      delete [] file_name;
   };
};

Sound *load_sound(SpriteType type, InputStream *pad_in, int notebook_version, 
						NaturalLanguage language, long index, Sprite *container, boolean descendants_inactive) {
	Sprite *comment = NULL;
	if (notebook_version >= first_version_to_save_copied_nests) {
		comment = load_item(pad_in,notebook_version,language);
	};
	unsigned short int length;
   pad_in->read((string) &length, sizeof(length));
	wide_string contents = NULL;
   if (length > 0) {
		string narrow_contents = new char[length+1];
		pad_in->read((string) narrow_contents,length);
		narrow_contents[length] = '\0';
	   contents = copy_wide_string(narrow_contents);
		delete [] narrow_contents;
   };
	boolean set_color_and_flags = FALSE;
	color_index color,flags;
	if (notebook_version >= first_version_to_save_copied_nests) {
		color = (color_index) pad_in->get();
		flags = (unsigned char) pad_in->get();
		set_color_and_flags = TRUE; // new on 010899
	};
   Sound *sound = NULL;
   boolean need_to_make_blank = FALSE;
   switch (type) {
      case BLANK_BUILT_IN_SOUND:
      case BLANK_SOUND: // older version when there was only built in sounds
         need_to_make_blank = TRUE;
      case BUILT_IN_SOUND:
      case SOUND: // older version when there was only built in sounds
        {
         unsigned short index;
         pad_in->read((string) &index,sizeof(index));
         sound = new BuiltInSound(0,0,contents,-1,0,0,0,TRUE); // -1 was length prior to 161199 but if there was an error it will be shorter
         ((BuiltInSound *) sound)->set_index(index);
         break;
        };
      case BLANK_MCI_SOUND:
         need_to_make_blank = TRUE;
      case MCI_SOUND:
         sound = new MCISound(); //0,0,contents,length,0,0,0,TRUE);
         break;
      case BLANK_SPEECH_SOUND:
         need_to_make_blank = TRUE;
      case SPEECH_SOUND:
         sound = new SpeechSound(); 
         break;
      case BLANK_USER_SOUND:
         need_to_make_blank = TRUE;
      case USER_SOUND:
         sound = new WaveSound();
         break;
      case BLANK_FORCE_FEEDBACK:
         need_to_make_blank = TRUE;
      case FORCE_FEEDBACK:
         {
         unsigned char joystick_number = (unsigned char) pad_in->get();
         boolean ignore_old;
#if TT_DIRECT_INPUT
			string narrow_contents = copy_narrow_string(contents);
         int index = force_index(narrow_contents);
			if (narrow_contents != NULL) delete [] narrow_contents;
#else
         int index = 0; // no joysticks anyways
#endif
         sound = new ForceFeedback(joystick_number,
                                   force_parameter_controlee(index,joystick_number,ignore_old));
//                                   intensity,direction,duration,trigger_button,
//                                   0,0,contents,length,0,0,0,TRUE);
         };
         break;
   };
   if (container != NULL) {  // new on 160702
      if (container->kind_of() == PROGRAM_PAD) {
         sound->set_in_notebook((Notebook *) container,FALSE); // no point recuring
      } else if (container->kind_of() == VACUUM) {
         sound->set_inside_vacuum(TRUE,FALSE);
      };
   }; 
	associate_shared_item(index,sound);
   if (need_to_make_blank) {
      sound->become_blank();
   };
	sound->set_comment(comment);
	if (contents != NULL) {
		sound->set_wide_text(contents);
	} else { // new on 100401
		log("Loading a sound from a file whose name is empty. Sorry, the sound won't work. ");
	};
	if (set_color_and_flags) { // and more...
		sound->set_color(color);
		sound->set_visible((boolean) (flags>>1));
		sound->set_show_all((boolean) (flags&1));
		sound->set_infinite_stack((boolean) (flags>>2)); // new on 190500
	};
   // what about active flag?
   return(sound);
};

boolean describe_sound(SpriteType type,
                       InputStream *pad_in, output_stream &text_out, int version_number,
                       boolean in_thought_bubble, Article article) {
	if (version_number >= first_version_to_save_copied_nests) {
		describe_comment_encoding(pad_in,text_out,version_number,SOUND);
	};
   unsigned short int length;
	pad_in->read((string) &length, sizeof(length));
   string contents = new character[length+1];
	if (length > 0) {
		pad_in->read((string) contents,length);
		contents[length] = NULL; // terminate the string
	};
	if (version_number >= first_version_to_save_copied_nests) {
		pad_in->get(); // ignore color
		pad_in->get(); // ignore flags like show all
	};
   switch (type) {
      case BLANK_BUILT_IN_SOUND:
      case BLANK_USER_SOUND:
      case BLANK_SPEECH_SOUND:
      case BLANK_MCI_SOUND:
         if (in_thought_bubble) {
			   text_out << S(IDS_ANY_SOUND);
		   } else {
			   text_out << S(IDS_BLANK_SOUND);
		   };
         if (type == BLANK_BUILT_IN_SOUND) { // was BLANK_SOUND prior to Sept. 5 '98
            unsigned short index;
            pad_in->read((string) &index,sizeof(index));
         };
         break;
      case BUILT_IN_SOUND:
      case USER_SOUND:
      case SPEECH_SOUND:
      case MCI_SOUND: 
			{
         print_type(SOUND,text_out,article);
         for (int i = 0; i < length; i++) {
			   if (new_line(contents[i])) { // was  == '\r' prior to 090802
				   contents[i] = ' '; // replace carriage returns with spaces
		      };
		   };
			if (length > 0) {
				text_out << " \"" << contents << "\"";
			} else {
				text_out << " \"\""; // new on 100401
			};
         if (type == BUILT_IN_SOUND) {
            unsigned short index;
            pad_in->read((string) &index,sizeof(index));
         };
			};
         break;
		case BLANK_FORCE_FEEDBACK: // new on 160799
			if (in_thought_bubble) {
			   text_out << S(IDS_ANY_FORCE_EFFECT);
		   } else {
			   text_out << S(IDS_BLANK_FORCE_EFFECT);
		   };
			pad_in->get(); // ignore joystick number
			break;
		case FORCE_FEEDBACK: // new on 160799
			{
			print_type(FORCE_FEEDBACK,text_out,article);
         for (int i = 0; i < length; i++) {
            if (new_line(contents[i])) { // was == '\r' prior to 090802
				   contents[i] = ' '; // replace carriage returns with spaces
		      };
		   };
         text_out << " \"" << contents << "\"";
			char joystick_number = pad_in->get();
			text_out << " (" << S(IDS_JOYSTICK_LABEL) << (int) (joystick_number+1) << ")";
			};
			break;
	};
   delete [] contents;
	return(TRUE);
};

/* commented out on 190799 since sound::respond_to_keyboard does a good job now
Sprite *ForceFeedback::respond_to_keyboard(char key, boolean , Sprite *by,
												       boolean record_action, ActionStatus *action_status) {
   if (record_action) { // commented out on 270499 for retraining && by != NULL && !by->still_running()) {
      record_event(KEYBOARD_CHARACTER,by,floor,this,TRUE,key);
		simulate_mouse_up();
	};
	completed(action_status);
   if (toggle_on) {
   	if (!ok_to_make_a_sound(floor)) {
         // in another house -- good policy to make no effect?
		   return(this);
	   };
      make_sound();
   } else {
      stop_making_sound();
   };
   toggle_on = !toggle_on;
	return(this);
};
*/

boolean ForceFeedback::make_sound() {
#if TT_DIRECT_INPUT
   if (parameter_controlee == NULL) return(FALSE); // new on 300799
	parameter_controlee->play_effect();
	return(TRUE);
//      play_force_feedback_effect(text,FALSE,joystick_number); // should be joystick index...
#else
// else beep or something??
	return(FALSE);
#endif
};

void ForceFeedback::generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection) {
	// new on 310799
	if (blank) {
		// could say more if done this before
		if ((contained_in(THOUGHT_BUBBLE) && !did_user(TEXT_BLANKED_INSIDE_THOUGHT_BUBBLE,TEXT) && 
			 !did_user(TEXT_BLANK,TEXT)) ||
			 tt_help_counter == 1) {
			 describe_pointing(message,tool_in_hand,selection);
		};
		if ((!did_user(TEXT_BLANKED_INSIDE_THOUGHT_BUBBLE,TEXT) && tt_help_counter == 0)
		    || tt_help_counter == 1) {
			message << S(IDS_TEXT_BLANKED_TB_HELP2)
					  << S(IDS_TEXT_BLANKED_TB_HELP3);
		};
		return;
	 } else if ((!did_user(TEXT_FORCE_EFFECT_FELT,TEXT) && tt_help_counter == 0 &&
					tt_force_feedback_joystick_count > 0)
					|| tt_help_counter == 6) {
		tt_help_history_action = TEXT_FORCE_EFFECT_FELT;
		message << S(IDS_FORCE_EFFECT_HELP);
		describe_right_button(message);
		message << S(IDS_FORCE_EFFECT_HELP2,"") << end_sentence();
		if (tt_force_feedback_joystick_count > 0) {
			message << S(IDS_FORCE_EFFECTS_ON_PAGE4);
		};
	};
};

//void ForceFeedback::toggle() {
   // unlike Sound this really does toggle since lots of effects are INFINITE
   // but it does mean that you
//   if (making_sound) {
//      stop_making_sound();
//   } else {
//      make_sound();
//   };
//   making_sound = !making_sound;
//};

void ForceFeedback::stop_making_sound() {
#if TT_DIRECT_INPUT
   if (parameter_controlee != NULL) parameter_controlee->stop_effect();
#endif
};

Sprite *ForceFeedback::copy(boolean also_copy_offsets) {
   ForceFeedback *copy = new ForceFeedback(joystick_number,parameter_controlee,
//                                           intensity,direction,duration,trigger_button,
                                           llx,lly,
						  	                      wide_text,text_length,
                                           current_priority-1,
                                           width,height);
	//if (blank && !tt_copy_restores_pre_blank) {
	//	copy->become_blank();
	//};
 //  copy->set_priority_fixed(priority_fixed_flag);
	//if (comment != NULL) {
	//	copy->set_comment(comment->copy());
	//};
   finish_making_copy(copy,also_copy_offsets);
	return(copy);
};

SpriteType ForceFeedback::dump_type() {
 	if (blank) {
      return(BLANK_FORCE_FEEDBACK);
   } else {
      return(FORCE_FEEDBACK);
   };
};

boolean ForceFeedback::dump(output_stream &stream, boolean just_prepare, boolean is_controlee, boolean on_nest) {
   if (!Sound::dump(stream,just_prepare,is_controlee)) return(FALSE);
   stream.put(joystick_number);
//   controlee->dump(stream,just_prepare);
	return(TRUE);
};

#if TT_XML

void ForceFeedback::xml(xml_element *element, xml_document *document) {
   Sound::xml(element,document);
   xml_set_attribute(element,L"JoystickNumber",joystick_number);
   if (parameter_controlee != NULL) {
      xml_set_attribute(element,L"ForceIndex",parameter_controlee->return_force_index());
		if (tt_include_descriptions_in_xml) { // tt_dump_reason == DUMPING_TO_CLIPBOARD) { // no need to waste time and space otherwise -- conditional new on 261102
			xml_set_attribute(element,L"Description",parameter_controlee->return_force_name());
		};
   };
};

boolean ForceFeedback::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
		case NO_MORE_TAGS: { // was FORCE_FEEDBACK_TAG: prior to 070103
         boolean ignore_old;
         set_parameter_controlee(force_parameter_controlee(xml_get_attribute_int(node,L"ForceIndex",0),xml_get_attribute_int(node,L"JoystickNumber",0),ignore_old));
//         return(TRUE); // and the following
								 };
      default:
         return(Sound::handle_xml_tag(tag,node));
   };
};
#endif


