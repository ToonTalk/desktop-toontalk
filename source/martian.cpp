// Copyright (c) 1992-2004. Ken Kahn, Animated Programs, All rights reserved.

// this seems better and doesn't take much time to switch
#define TT_SOUND_OFF_ONLY_WHEN_SPEAKING 1   

#if !defined(__TT_MARTIAN_H)   
#include "martian.h"
#endif   
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif   
#if !defined(__TT_BCKGRND_H)   
#include "bckgrnd.h"
#endif   
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif 
#if !defined(__TT_PRGRMMR_H)     
#include "prgrmmr.h"
#endif   
#if !defined(__TT_SCREEN_H)   
#include "screen.h"
#endif
#if !defined(__TT_WINMAIN_H)
#include "winmain.h"
#endif
#if !defined(__TT_HELP_H)
#include "help.h"
#endif
#if !defined(__TT_PICTURE_H)
#include "picture.h"
#endif
#if !defined(__TT_PUZZLE_H)
#include "puzzle.h"
#endif
#if !defined(__TT_ANIMATE_H)   
#include "animate.h"
#endif 
#if !defined(__TT_SPEAK_H)   
#include "speak.h"
#endif
#if !defined(__TT_ROBOT_H)
#include "robot.h"
#endif
#if !defined(__TT_LOG_H)
#include "log.h"
#endif
   
//#include <string.h> // for strlen
#if !TT_32
#include <stdlib.h> // for max
#include <mmsystem.h> // for timeGetTime
#endif

//#include <strstrea.h> commented out 210703
#include <wchar.h> // for wmemcpy

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum MartianState
	{TELEPORT_IN,TELEPORT_OUT,ACTION_CYCLE,
    MARTY_INJURED_AND_DIZZY,MARTY_INJURED_AND_POINTING,MARTY_INJURED,
    MARTY_WAITING};

void martian_just_injured_and_pointing() {
   tt_martian->set_parameter(MARTY_INJURED_AND_POINTING);
};

void martian_just_injured_and_dizzy() {
   tt_martian->set_parameter(MARTY_INJURED_AND_DIZZY);
};

enum BalloonState
	{BALLOON_INERT,BALLOON_GROWS_TO_ALIVE,BALLOON_SHRINKS,BALLOON_ALIVE};

//GlobalPicture *simulated_speech = NULL;

//void remove_text_callback(void *t) {
//	((Talk_Balloon *) t)->remove_text();
//};

millisecond compute_reading_duration(int characters) { // abstracted on 110401
	millisecond duration = ((6000+characters*150L)*100L)/tt_help_speed_percent;
	// ToonTalk speed should not affect Marty's talking   
	duration = (duration*tt_speed)/100;
	if (duration == 0) { // new on 210300
		duration = 1;
	};
	return(duration);
};

Martian::Martian() :
  Sprite(MARTIAN_SPRITE,0,0,MARTIAN),
  on_floor(FALSE),
  balloon(NULL),
  keep_ignoring_button(FALSE),
//  called_count(0),
  potential_help_target(NULL),
//  previous_tool_in_hand(NULL),
  favorite_floor_x(3*tile_width),
  favorite_floor_y(0),
  favorite_room_x(11*tile_width), // was 12 prior to 200400 put clipped sometimes
  favorite_room_y(0),
  dont_interrupt(FALSE),
  pending_message_dont_interrupt(FALSE),
  greeting_needed(FALSE),
  teleport_out_when_nothing_to_say(FALSE),
  first_time(TRUE),
//  automatically_appear_when_sitting(TRUE),
  next_selection_is_target(FALSE),
  timer_trigger_help(FALSE),
  saying_greeting(FALSE),
  called_by_user_flag(FALSE),
  teleporting_out(FALSE),
//  saved_message_stream(NULL),
  pending_message(NULL),
  alternative_spoken_pending_message(NULL),
  time_of_last_help(0),
  ready_to_move_off_screen_time(0),
  time_between_greetings(15000), // 15 seconds since last help given triggers more
  previous_importance(-1),
  pending_message_importance(-1),
  center_counter(0),
  start_of_current_selection(0),
  teleport_in_selection(NULL),
  teleport_in_tool_in_hand(NULL),
  give_help_without_waiting(FALSE),
  previous_target(NULL),
  started_generating_help(0),
  last_greeting_help(0),
  subject(NULL),
  ok_to_repeat(TRUE),
  saved_tt_marty_talk(MARTY_SPEAKS),
  saved_say_id(-1) {
	  if (tt_log_version_number >= 79) { // new on 050805
		  time_of_last_help = -time_between_greetings;
		  // new 050805 since if you were very quick you didn't get initial help 
	  };
  //saved_width(min_long),
  //saved_height(min_long),
  //saved_llx(0),
  //saved_lly(0) {
	last_thing_said[0] = NULL;
	last_thing_said[1] = NULL; // normal and alternative spoken languages
//   if (tt_system_mode == PUZZLE && tt_programmer->kind_of() != PROGRAMMER_TITLES) {
//      set_parameter(MARTY_INJURED_AND_DIZZY);
//   };
//	set_priority_function_of_lly(FALSE);
	set_active(FALSE,FALSE); // to start with
   set_priority_fixed(TRUE);
	set_priority(min_long+1); // floating in front of everything
	move_to_favorite_spot();
   set_never_cache(TRUE); // always moving around
// in the room but not on the floor
//	balloon = new Talk_Balloon(llx-(2*width)/3,lly+height);
//	add_follower(balloon,background,FALSE);
   set_containable_by_background(FALSE); // handled specially
	set_move_between_floor_and_room(FALSE); // handled specially
//   set_ok_to_dump(FALSE); // commented out on 220999 so can save cities with or without Marty available
//	if (tt_good_sizes_adjust_to_screen_size) { // new on 190999
//		set_size(shrink_width_from_640x480_screen_size(width),
//					shrink_height_from_640x480_screen_size(height));
//	};
};

void Martian::re_initialize() { // new on 250203
	// for now just fix this problem for time travel - may be other things that need to be reset
	floor = NULL;
	Sprite::re_initialize(); // new on 160204
};

void Martian::prepare_for_deletion() {
   if (preparing_for_deletion) return; // already done
	preparing_for_deletion = TRUE; // new on 060204
//	if (active()) {// && background->has_item(this)) {
//		background->remove_item(this,FALSE);
//	};
	if (pending_message != NULL) {
		delete [] pending_message;
      pending_message = NULL;
	};
	for (int i = 0; i < 2; i++) {
		if (last_thing_said[i] != NULL) {
			delete [] last_thing_said[i];
			last_thing_said[i] = NULL;
		};
	};
	if (alternative_spoken_pending_message != NULL) { // new on 240101
		delete [] alternative_spoken_pending_message;
		alternative_spoken_pending_message = NULL;
	};
// balloon should be covered by general ~Sprite
//	remove_follower(balloon);
//  background->remove_item(balloon);
//	if (balloon != NULL) delete balloon;
	Sprite::prepare_for_deletion();
	if (this == tt_martian) tt_martian = NULL; // added on 141299 for new demo playback
};

void Martian::teleport_in_after(millisecond delay) {
#if TT_CALL_FOR_MARTY
	if (tt_call_for_marty != NULL) {
		return; // new on 040805 since already waiting for the user to press F1
	};
#endif
	Background *background = tt_screen->pointer_to_background();
	if (!background->has_item(this)) {
		background->add_item(this,FALSE);
		tt_screen->remove(this);
	};
	do_after(delay,this,TELEPORT_IN_CALLBACK);
};

void Martian::teleport_in(TeleportInReason reason) {
	if (still_talking() && reason != MARTY_CALLED_BY_LOADER) { // second condition added 060805
#if TT_DEBUG_ON
		log("Marty beaming in while still talking??");
#endif
		// may have been limited to subtitles and the fix of 220405 eliminated the problem
		return; // to see if this fixes the problem of Marty flashing in and out -- 190205 
		// -- not verified but moved out of TT_DEBUG_ON on 210205
	};
	boolean remove_old_text = TRUE; // new on 030805
#if TT_CALL_FOR_MARTY
	if (reason != MARTY_CALLED_BY_USER && tt_marty_appears_only_if_called && tt_programmer->kind_of() != PROGRAMMER_TITLES
		 && pending_message_importance < 10) { // new condition on 091205 so that important enough messages come thru regardless
		// new on 030805
		if (tt_call_for_marty == NULL) {
			FileNameStatus name_status;
			string full_file_name = existing_file_name(name_status,"call_for_marty.tt");
			if (full_file_name != NULL) {
				boolean aborted = FALSE;
				UnwindProtect<boolean> set(tt_dont_set_tt_loading_is_ok_to_abort,TRUE);
				tt_call_for_marty = sprite_from_file_name1(full_file_name,aborted); // internal version so no loading message
				if (tt_call_for_marty != NULL) {
					tt_call_for_marty->set_graspable(FALSE);
					tt_call_for_marty->set_selectable(FALSE);
					tt_call_for_marty->propagate_changes(); // do any good?? -- can't hurt
					if (tt_call_for_marty_wait_duration > 0) {
						tt_call_for_marty->do_after(tt_call_for_marty_wait_duration,NULL,REMOVE_CALL_FOR_MARTY_CALLBACK);
					};
					if (reason == MARTY_CALLED_BY_TIMER) {
						give_help(teleport_in_selection,teleport_in_tool_in_hand);
					};
					if ((reason == MARTY_CALLED_BY_TIMER || reason == MARTY_CALLED_BY_ERROR_HANDLER) && 
						 tt_martian->is_message_pending()) {
					   // on 021205 added second condition and moved this to after the call to "give_help"
						play_sound(TELEPORT_SOUND);
					};
				} else { // new on 161105
					tt_error_file() << "Your installation is missing the file " << full_file_name << endl;
					tt_err_file_important = TRUE;
				};
			};
		};
//		if (tt_call_for_marty != NULL) { // commented out on 161105 in case call_for_marty.tt is missing
			remove_from_floor(FALSE,FALSE); // new on 040805 in case started to beam in
			return;
//		};
	};
	if (reason == MARTY_CALLED_BY_USER && tt_call_for_marty != NULL) { // not needed since user called for Marty
		remove_call_for_marty(FALSE);
		remove_old_text = FALSE;
		reason = MARTY_CALLED_BY_ERROR_HANDLER; // new on 091005 since most likely called because of call_for_marty
	};
#endif
//	set_priority_function_of_lly(!on_floor);
	called_by_user_flag = (flag) (reason == MARTY_CALLED_BY_USER);
	Background *background = tt_screen->pointer_to_background();
	background->add_item(this,FALSE,FALSE,FALSE);
	on_floor = background->directed_help_ok();
	set_active(TRUE,FALSE);
	tt_screen->add(this);
	move_to_favorite_spot();
	int percent_size = 100;
	update_balloon();
   if (tt_programmer->kind_of() != PROGRAMMER_TITLES) {
		if (reason == MARTY_CALLED_BY_USER || reason == MARTY_CALLED_BY_TIMER || reason == MARTY_CALLED_BY_ERROR_HANDLER) {
			play_sound(TELEPORT_SOUND);
		};
		percent_size = shrink_percent_from_640x480_screen_size(percent_size);
   };
   //saved_width = current_width(); // new on 090502 since if saved while on the ground then this clobbers the saved size
   //saved_height = current_height();
   //lower_left_corner(saved_llx,saved_lly);
	set_size_percent(percent_size);
//	if (balloon != NULL) { 
	// hard to get this to work well and not mess up other size issues with talk balloons (commented out on 150202)
//		balloon->new_character_size(percent_size,FALSE); // new on 220102 so text shrinks of talk balloon and Marty do
//	};
	set_parameter(TELEPORT_IN);
//	set_cycle_stopped_callback(teleport_in_done_callback);
   do_after(total_cycle_duration(),this,TELEPORT_IN_DONE_CALLBACK);
	tt_help_counter = 0;
	if (called_by_user_flag) {
		next_selection_is_target = TRUE;
//		called_count++;
		user_did(GLOBAL_HISTORY,USER_HAS_RECALLED_MARTIAN);
		tt_martian_automatically_appears = TRUE;
		timer_trigger_help = FALSE;
		if (remove_old_text) {
			remove_text();
		};
	};
	dont_interrupt = FALSE;
	set_visible(TRUE); // just in case
//	set_priority(min_long); // on top of everything
};

void Martian::set_priority(long new_priority) {
   // obsolete??
	Sprite::set_priority(new_priority);
	if (balloon != NULL) {
		if (tt_city->stopped()) { // if Marty has come down then balloon shouldn't
			balloon->set_priority(min_long+1);
		} else {
		   balloon->set_priority(new_priority);
		};
	};
};

void Martian::teleport_in_done() {
//	balloon->set_parameter(BALLOON_INERT); //BALLOON_GROWS_TO_ALIVE);
//	balloon->set_size_percent(100);
//	balloon->animate_to_size_percent(100,750);
//	char buffer[512];
//	output_string_stream message(buffer,512);
//	dont_interrupt = TRUE;
   if (!active()) return; // must have beamed out in the meantime
	if (tt_help_target == NULL) greeting_needed = TRUE;
	if (tt_system_mode != PUZZLE || tt_programmer->kind_of() == PROGRAMMER_TITLES) {
      set_parameter(ACTION_CYCLE);	
   };
   if (tt_programmer->kind_of() != PROGRAMMER_TITLES) {
		if (tt_marty_talk == MARTY_SPEAKS || tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES || tt_marty_talk == MARTY_WITH_SUBTITLES) { // new on 311000 
			if (balloon == NULL) {
				initialize_balloon();
			};
		 	hide_balloon();
		};
#if TT_TEXT_TO_SPEECH && !TT_SOUND_OFF_ONLY_WHEN_SPEAKING
      // doesn't really do it if -tts_uses_ds 1
      if (text_to_speech()) turn_on_speech_and_sound_off();
#endif
		give_help(teleport_in_selection,teleport_in_tool_in_hand);
	};
};

void Martian::give_help_after(millisecond delay) {
   tt_global_picture->do_after(delay,this,TELEPORT_IN_DONE_CALLBACK); // good enuf
};

void Martian::teleport_out(boolean by_user) {
	if (teleporting_out) { // already doing this
		// new on 051004 since can happen if the user doesn't move the mouse
		// e.g. retraining
		return;
	};
	finish_instantly(); // new on 061000
	play_sound(TELEPORT_SOUND);
	set_parameter(TELEPORT_OUT);
	set_cycle_stopped_callback(TELEPORT_OUT_DONE_CALLBACK);
	teleporting_out = TRUE;
	remove_text(TRUE);
	switch (tt_frame_number%3) {
		case 0:
			say(IDS_BYE1,2);
			break;
		case 1:
			say(IDS_BYE2,2);
			break;
		case 2:
			say(IDS_BYE3,2);
			break;
	};		
	if (by_user) tt_martian_automatically_appears = FALSE;
	called_by_user_flag = FALSE;
	if (held()) {
		tt_programmer->release_items_held();
	};
};

#if TT_TEXT_TO_SPEECH
void Martian::new_talk_mode(MartyTalk new_mode) {
   tt_marty_talk = new_mode;
	tt_closed_caption = (tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES || tt_marty_talk == MARTY_WITH_SUBTITLES); // new on 101199 MARTY_WITH_SUBTITLES added on 110401
	if (tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES && alternative_language_spoken()) {
		tt_closed_caption_alternative_language = TRUE;
		tt_closed_caption = FALSE;
	};		
   if (text_to_speech()) {
#if !TT_SOUND_OFF_ONLY_WHEN_SPEAKING
      turn_on_speech_and_sound_off();
#endif
   } else {
      turn_on_sound_and_speech_off();
   };
   remove_text(FALSE);
};
#endif

void Martian::teleport_out_done() {
//	remove_balloon();
   if (balloon != NULL) {
		balloon->remove_from_floor(FALSE);
		balloon->set_visible(FALSE);
	};
   remove_text();
	remove_from_floor(TRUE,TRUE);
	set_active(FALSE,FALSE);
	teleporting_out = FALSE;
	// null the following just in case they are destroyed in the meantime
	set_potential_help_target(NULL);
	set_teleport_in_selection(NULL);
	set_teleport_in_tool_in_hand(NULL);
#if TT_TEXT_TO_SPEECH
   if (tt_programmer->kind_of() != PROGRAMMER_TITLES) { // until SAPI 3.0
      turn_on_sound_and_speech_off(2000); // to finish saying last thing
   };
#endif
	teleport_out_when_nothing_to_say = FALSE; 
	// moved here on 111100 to prevent Marty from appearing and saying nothing if error (e.g. missing file) was reported during credits
   //if (saved_width != min_long) { // new on 090402
   //   set_size_and_location(saved_width,saved_height,saved_llx,saved_lly);
   //   remove_balloon();
   //};
//	stop_speaking(); // new on 040803 since if reading something long (e.g. a very large number) then he'll keep talking for a long time otherwise
	do_after(default_duration(3000),NULL,STOP_SPEAKING_CALLBACK); // re-written on 060803 since this sometimes cuts off Marty saying Good bye.
	// added default_duration on 080705 in case is already off screen
};

void Martian::teleport_toggle(Sprite *current_selection, Sprite *tool_in_hand) {
//	boolean dont_teleport_in = FALSE;
	// commented out on 181004
	//if (tt_log_version_number == 20 && tt_frame_number == 17983) { 
	//	// new on 270300 since recording of intro.dmo hits F1 just before Marty left
	//	// and for some languages Marty is beaming out or has left by then
	//	string short_name = short_file_name(replay_file_name()); // pretty unlike to really need this check...
	//	if (short_name != NULL) {
	//		dont_teleport_in = (stricmp(short_name,"intro") == 0);
	//		delete [] short_name;
	//	};
	//};
   if (teleporting_out) {
//		if (dont_teleport_in) return;
      finish_instantly();
		teleporting_out = FALSE; // new on 120505 since if saved while teleporting seems to get confused
   };
   if (active()) {
		teleport_out(TRUE);
	} else {
//		if (dont_teleport_in) return;
		teleport_in(MARTY_CALLED_BY_USER);
		set_teleport_in_selection(current_selection);
		set_teleport_in_tool_in_hand(tool_in_hand);
	};
};

void Martian::change_background(Background *new_background) {
	if (floor == new_background) { // already was
		return;
	};
#if TT_CALL_FOR_MARTY
	if (tt_call_for_marty != NULL) {
		return;
	};
#endif
	// might be still beaming in
	finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
	// background_is_floor true when in thought bubble as well
	if (active()) {
//		background->remove_item(this,TRUE,TRUE);
      remove_from_floor(FALSE,FALSE);
		new_background->add_item(this,FALSE);
	};
	if (balloon != NULL) {
      if (balloon->still_talking()) {
		   ready_to_move_off_screen_time += 10000; // add 10 seconds for transition
	   } else {
         balloon->remove_from_floor(FALSE);
         balloon->set_visible(FALSE);
      };
//		remove_balloon(); // will be recreated on need on new background
	};
//		new_background->add_leader(this); // of the talk balloon
//		balloon->remove_text();
//	if (on_floor) active_flag = TRUE;
	on_floor = new_background->directed_help_ok();
//	on_floor = (flag) background_is_floor;
//	set_priority_function_of_lly(!on_floor);
//	set_priority(-5); // on top of everything
//	background = new_background;
	set_background(new_background);
};

void Martian::hide_balloon() {
	if (balloon != NULL) {
      balloon->set_visible(FALSE);
   };
};

boolean Martian::show_balloon() {
   if (balloon == NULL) {
      initialize_balloon();
      return(TRUE);
   };
	// re-ordered these 2 conditonals on 311000 since if balloon is NULL it needs to be created and made invisible if no balloon to be seen
	if ((tt_marty_talk == MARTY_SPEAKS || tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES || tt_marty_talk == MARTY_WITH_SUBTITLES) && 
		 tt_programmer->kind_of() != PROGRAMMER_TITLES) { // condition new on 311000
		hide_balloon(); // new on 311000 since it always should be
		return(FALSE);
	};
   balloon->set_priority(priority());
   if (balloon->visible()) return(TRUE); // already is
   balloon->set_visible(TRUE);
   tt_screen->add(balloon); // just in case
//   if (balloon->pointer_to_background() == NULL) floor->add_item(balloon);
	return(TRUE);
};

void Martian::remove_balloon() {
	if (balloon != NULL) {
		if (balloon->pointer_to_leader() == this) { // condition added on 060600 if PageUp is pressed under some conditions
			remove_follower(balloon);
		};
      balloon->remove_from_floor(FALSE,TRUE); // no-op if removed follower above as of 070105
		balloon->destroy();
		balloon = NULL;
	};
};

void Martian::new_character_size(unsigned short int percent) {
	tt_character_size_percent = percent; // caller checked range??
	if (balloon != NULL) {
		remove_balloon();
      initialize_balloon();
   };
};

void Martian::repeat_or_speed_up() {
   // user clicked so if talking skip to next -- if not repeat last thing
   if (still_talking()) {
      dont_wait_to_say_more(); // was tt_finish_all_animations = TRUE; // speed up talking with any action
   } else {
      repeat_last_thing_said();
   };
};

string Martian::repeat_last_thing_said_string() {
	int buffer_length = _tcslen(last_thing_said[tt_use_spoken_alternative])+256;
   string buffer = new character[buffer_length];
   output_string_stream stream(buffer,buffer_length);
   stream << S(IDS_REPEAT1+my_random(3)); // simplified on 050999
   stream << last_thing_said[tt_use_spoken_alternative];
   stream.put('\0');
	return(buffer);
};

void Martian::repeat_last_thing_said() {
	if (!showing_on_a_screen()) return; // new on 080401
   if (last_thing_said[tt_use_spoken_alternative] == NULL) return; // was just last_thing_said prior to 260500
	boolean visible_balloon = (balloon != NULL && balloon->visible());
   if (visible_balloon) { // conditions new on 110401 since don't want a new balloon if runing say subtitles
		remove_balloon(); // reset first
	};
   string message = repeat_last_thing_said_string();
   string alternative_spoken_message = NULL;
	if (alternative_language_spoken()) {
		tt_use_spoken_alternative = TRUE;
		alternative_spoken_message = repeat_last_thing_said_string();
		tt_use_spoken_alternative = FALSE;
	};
   ok_to_repeat = FALSE; // don't repeat the extra stuff again...
//#if TT_UNICODE
//	say(stream.str().c_str(),previous_importance+1);
//#else
	say(message,max((short) previous_importance, (short) tt_minimum_say_importance)+1,alternative_spoken_message); // added tt_minimum_say_importance on 070401 since if -1 nothing will happen here
   delete [] message;
	if (alternative_spoken_message != NULL) delete [] alternative_spoken_message;
//#endif
   ok_to_repeat = TRUE;
	if (!visible_balloon && balloon != NULL) { // new on 110401 to restore invisibility
		balloon->set_visible(FALSE);
	};
};

void Martian::say(int text_id, int importance) {
	string spoken_alternative_message = NULL;
	if (alternative_language_spoken()) {
		spoken_alternative_message = S(text_id,"",TRUE);
	};
	say(S(text_id),importance,spoken_alternative_message);
};

string process_for_portuguese(string text) {
	// currently just replaces " em a " with " na " and " em o " with " no "
	// so new length is always shorter (or equal)
	int length = _tcslen(text);
	string new_text = new char[length+1];
	int i = 0;
	int j = 0;
	for (; i < length-5; i++) { // -5 since search strings won't fit at the end
		if (text[i] == ' ' && text[i+1] == 'e' && text[i+2] == 'm' && text[i+3] == ' ' && text[i+5] == ' ' &&
			 (text[i+4] == 'a' || text[i+4] == 'o')) {
			new_text[j++] = ' ';
			new_text[j++] = 'n';
			new_text[j++] = text[i+4];
			new_text[j++] = ' ';
			i += 5;
		} else {
			new_text[j++] = text[i];
		};
	};
	for (; i < length; i++) { // -5 since search strings won't fit at the end
		new_text[j++] = text[i];
	};
	new_text[j] = '\0';
	return(new_text);
};

void Martian::say(const_string original_text, int importance, const_string alternative_spoken_text) {
	// for now importance can be either
	// 0 -- say only if nothing else being said -- ok to interrupt this
	// 1 -- say only if nothing else being said -- not ok to interrupt this
	// 2 -- say if not talking -- otherwise say it later -- not ok to interrupt
	//   -- worth teleporting in if not active
	// Note: 2 will interrupt lower priority text
	// alternative_spoken_text new on 020999
   if (!called_by_user() && importance < tt_minimum_say_importance) return;
	if (tt_system_mode == PUZZLE && !inside_rocket() && (floor == NULL || !floor->is_titles_background())) { 
		// new on 040601 so nothing is even queued up to say when not in rocket -- this is in 2.39c as well
		// added titles_background check so credits show up
		log(original_text);
		return; 
	};
	string text;
	boolean delete_text = FALSE;
	if (tt_language == BRAZILIAN_PORTUGUESE) { // new on 130700
		text = process_for_portuguese(original_text);
		delete_text = TRUE;
	} else {
		text = original_text;
	};
	int length = _tcslen(text);
	if (length == 1 && text[0] == ' ') return; // just saying " "
#if TT_DEBUG_ON
	if (length > 10000) {
		tt_error_file() << "Warning: Marty about to say something with more than 10000 letters!" << endl;
		tt_error_file().write(text,length);
		tt_error_file() << endl;
	};
	if (tt_debug_mode == 40805) {
		tt_error_file() << "Marty about to say something on frame " << tt_frame_number << " at time " << tt_current_time << endl;
		tt_error_file().write(text,length);
		tt_error_file() << endl;
	};
#endif
	boolean talking = still_talking(); 
	// changed on 150301 since still_talking checks now for subtitles - (still_talking() || tt_screen->displaying_subtitle(TRUE)); 
	// second disjunct added 101199
	if (length == 0 && (importance < 2 || !talking)) return;
	if (importance < pending_message_importance) {
#if TT_CALL_FOR_MARTY
		if (tt_call_for_marty == NULL) 
			// new on 040805 to add less important messages to pending if Marty hasn't been called to speak
#endif
		return;
	};
	if ((importance == 0 || importance < previous_importance) && talking) {
		 return;
	};
	if (importance > previous_importance && talking) { // changed 4/7/97
//		delete [] pending_message;
//		pending_message = NULL;
		remove_text(importance > pending_message_importance);
	};
#if TT_TEXT_TO_SPEECH
   if (tt_programmer->kind_of() != PROGRAMMER_TITLES) { 
		if (tt_marty_talk == MARTY_SPEAKS) {
			favorite_floor_y = -4*tile_height; // keep him mostly off the bottom
		} else if (tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES || tt_marty_talk == MARTY_WITH_SUBTITLES) { 
			// new on 200999 - updated 101199
			favorite_floor_y = ideal_screen_height/6-4*tile_height;
		};
   };
#endif   
	if (importance >= pending_message_importance
#if TT_CALL_FOR_MARTY
		 || tt_call_for_marty != NULL // new on 040805 so don't lose messages just because haven't called for Marty
#endif 
		) {
		if (pending_message != NULL) { // add to old one
			// if importance is > should it be pre-pended??
			pending_message_importance = max(pending_message_importance,importance);
			int old_length = _tcslen(pending_message);
			if (old_length >= length && _tcsncmp(pending_message,text,length) == 0) {
				 return; // already knew it
			};
			string temp = new character[length+old_length+1+1]; // 1 for space
			memcpy(temp,pending_message,old_length);
			temp[old_length] = ' ';
			memcpy(temp+old_length+1,text,length);
			length += old_length+1;
			temp[length] = 0; // terminate string
			delete [] pending_message;
			pending_message = temp;
			if (!pending_message_dont_interrupt) {
				 pending_message_dont_interrupt = dont_interrupt;
			};
//			delete [] temp; // new on 140104 -- removed on 150104 since pending_message is a pointer to it
		} else if (last_thing_said[0] != NULL && strcmp(last_thing_said[0],text) == 0) { 
			// no need to say it again -- new on 080705
			return;
		} else {
			pending_message = copy_string(text);
         length = _tcslen(text);
			pending_message_importance = importance;
		   pending_message_dont_interrupt = dont_interrupt;
		};
		if (alternative_spoken_text != NULL) {
			if (alternative_spoken_pending_message != NULL) {
				int old_length = _tcslen(alternative_spoken_pending_message);
				int length = _tcslen(alternative_spoken_text);
				string temp = new character[length+old_length+1+1]; // 1 for space
				memcpy(temp,alternative_spoken_pending_message,old_length);
				temp[old_length] = ' ';
				memcpy(temp+old_length+1,alternative_spoken_text,length);
				length += old_length+1;
				temp[length] = 0; // terminate string
				delete [] alternative_spoken_pending_message;
				alternative_spoken_pending_message = temp;
			} else {
				alternative_spoken_pending_message = copy_string(alternative_spoken_text);
			};
		};
	} else {
		previous_importance = importance;
	};
	if (active() && floor != NULL && !floor->slated_for_destruction()) { // and not blown up
//		boolean saved_dont_interrupt = dont_interrupt; // might be clobbered by say
//		if (balloon == NULL) {
			initialize_balloon();
//       started_talking();         
//		} else if (!talking) {
//         started_talking();
//     };
//      started_talking();
		if (!dont_interrupt || !talking) {
			// following will free storage for pending_message
         string saved_pending_message = pending_message;
			string saved_alternative_spoken_pending_message = alternative_spoken_pending_message;
         pending_message = NULL;
			alternative_spoken_pending_message = NULL;
			boolean add_to_old = (talking && (previous_importance >= pending_message_importance)); // added talking on 290301
			balloon->say(this,saved_pending_message,length,FALSE,add_to_old,
							 saved_alternative_spoken_pending_message,FALSE,add_to_old);
			previous_importance = pending_message_importance;
			pending_message_importance = -1;
			dont_interrupt = pending_message_dont_interrupt;
			pending_message_dont_interrupt = FALSE;
//			if (higher_priority) {
//				no_interruptions();
//			};
		}; // otherwise leave it pending
	} else if (importance >= tt_minimum_say_importance) {
		teleport_out_when_nothing_to_say = !tt_marty_appears_only_if_called; // prior to 050805 was TRUE;
		Background *background = tt_screen->pointer_to_background();
		if (background == NULL || // added this on 310100 since some errors at wrong time don't have a background
			 background->slated_for_destruction() || (background->pointer_to_room() == NULL && !background->on_the_ground())) {
			// so this will be seen when entering a house
			tt_martian_automatically_appears = TRUE; 
		} else {
			set_teleport_in_selection(NULL);
			set_teleport_in_tool_in_hand(NULL);
			teleport_in(MARTY_CALLED_BY_ERROR_HANDLER);
			previous_importance = importance; // to protect it
		};
	};
	if (delete_text) delete [] text; // constructed as a re-write
};

void Martian::initialize_balloon(Talk_Balloon *new_balloon, boolean loading) {
   if (balloon != NULL) {
      show_balloon();
      return;
   };
	if (new_balloon != NULL) {
		balloon = new_balloon; // new on 211103 to restore better animating talk balloons - 
		// some of the following may be unnecessary (and harmful?)
	} else {
		balloon = new Talk_Balloon(0,0);
	};
	if (!loading) {
		update_balloon();
		balloon->set_priority(priority());
	};
	//  more like part of the martian so shouldn't ...		
	//		balloon->set_priority_function_of_lly(!on_floor); 
//	balloon->set_priority_function_of_lly(FALSE);
   balloon->set_priority_fixed(TRUE);
	add_follower(balloon,FALSE,INSERT_AT_END,loading); // last arg new on 040204 to preserve geometry when saved
	tt_screen->add(balloon);
	if (tt_marty_talk == MARTY_WITH_SUBTITLES) { // new on 120401
		balloon->set_visible(FALSE);
	};
   // commented out on 181002 since this produced small talk balloons with small text when running with high resolution
   //int percent = get_size_percent();
   //balloon->set_size_percent(percent); // new on 070502 since otherwise text is too small and balloon too big
	//balloon->new_character_size(percent,TRUE); // new on 150202
//   started_talking();
};

void Martian::started_talking() {
   if (current_parameter() == TELEPORT_OUT) return;
   if (tt_system_mode == PUZZLE && tt_programmer->kind_of() != PROGRAMMER_TITLES) {
//      if (current_parameter() != MARTY_INJURED_AND_DIZZY) {
         set_parameter(MARTY_INJURED);
//      };
   } else {
      set_parameter(ACTION_CYCLE);
   };
};

void Martian::react(boolean &,
						  unsigned char &,
						  boolean &,
						  city_coordinate &,
						  city_coordinate &,
						  char &,
						  millisecond ) {
// use this later for entering comments
  // F7 is better (and more general)
/*
  if (button_status >= 0 ) { // new button click or none
	  keep_ignoring_button = FALSE;
  };
  if (key != 0 || button_status > 0) {
	  if (dont_wait_to_say_more()) {
		  key = 0;
		  keep_ignoring_button = TRUE;
	  };
  };
  if (keep_ignoring_button) {
	  button_status = 0;
  };
*/
};
	  
boolean Martian::dont_wait_to_say_more() {
   dont_wait_to_give_help(FALSE);
	if (balloon != NULL && balloon->visible()) {
		return(balloon->dont_wait_to_say_more());
	} else if (still_talking()) { // added on 210999
		if (saved_say_id >= 0) { // simulated speaking new on 110401
			saved_say_id = -1; // remove the scheduled
			finish_instantly();
			speech_has_finished(-1);
		} else {
			stop_speaking();
			speech_has_finished(-1); // was TRUE prior to 040600 but that is an argument for talk_balloon::speech_has_finished()
		};
		return(TRUE);
	} else {
		return(FALSE); // no balloon so can't be waiting for anything
	};
};

string Martian::currently_saying() {
   if (balloon != NULL) {
      return(balloon->currently_saying());
   } else {
      return(NULL);
   };
};

boolean Martian::still_talking() {
	if (tt_closed_caption || tt_closed_caption_alternative_language) { // re-written on 080301 to work better when showing subtitles and talking
		return(tt_screen->displaying_subtitle(TRUE)); // last disjunct new on 040201 in case displaying subtitles
	} else {
		return(balloon != NULL && balloon->still_talking());
	};
};

void Martian::move_to_favorite_spot() {
	if (on_floor) { //on_floor is used to decide whether to give directed help and favorite spots -- both same now but might not always be
		city_coordinate programmer_x,programmer_y;
		tt_programmer->pointer_to_appearance()->center_location(programmer_x,programmer_y);
		if (programmer_x < favorite_floor_x+current_width()) {
			move_to(favorite_floor_x+9*tile_width,favorite_floor_y);
		} else {
			move_to(favorite_floor_x,favorite_floor_y);
		};
	} else {
		move_to(favorite_room_x,favorite_room_y);
	};
};

void Martian::animate_to_favorite_spot_and_size(millisecond duration) {
//	if (on_floor) {
		animate_location(favorite_floor_x,favorite_floor_y,duration);
//	} else {
//		animate_location(5*tile_width,7*tile_height,duration);	
//	};
	animate_to_size_percent(shrink_percent_from_640x480_screen_size(100),duration);
	dont_interrupt = FALSE; // just arriving
};

//void Martian::update_display() {
//	Sprite::update_display();
//	if (balloon != NULL) {
//		balloon->update_display();
//	};
//};

void Martian::update_balloon() {
	if (balloon == NULL) return;
//	if (on_floor) {
		balloon->move_to(llx+width/2-(balloon->true_width()/2),
							  lly+(5*height)/6);
//	} else {
//		balloon->move_to(llx+(5*width)/6,lly);
//	};
//	int percent = get_size_percent();
	// new on 150202 since with high resolution then the balloon and text are too small - but if it doesn't shrink is too big so
//	if (tt_screen_width > 800) { // 800 wide looks OK
//		percent += (tt_screen_width-800)/8; // so at width of 1600 add 100%
//	};
//	if (percent > 100) {
//		int x = balloon->get_size_percent();
//		balloon->set_size_percent(percent);
//	};
//	balloon->new_character_size(percent,FALSE); // new on 230102 so text shrinks as talk balloon and Marty do 
};

void Martian::set_size_and_location(city_coordinate new_width, city_coordinate new_height, 
												city_coordinate new_llx, city_coordinate new_lly, ChangeReason reason) {
	boolean size_changed = (width != new_width);
	Sprite::set_size_and_location(new_width,new_height,new_llx,new_lly,reason);
	if (size_changed && balloon != NULL) {
//		balloon->new_character_size(get_size_percent()); // new on 230102 so text shrinks as talk balloon and Marty do -- moved here on 150202
		update_balloon();
	};
};

void Martian::say_pending_message() {
	initialize_balloon();
	if (pending_message == NULL) { // troubles initializing caused call to martian::say
      return;
   };
   string saved_pending_message = pending_message;
	pending_message = NULL;
	string saved_alternative_spoken_pending_message = alternative_spoken_pending_message;
	alternative_spoken_pending_message = NULL;
   started_talking();
	boolean add_to_old = (previous_importance >= pending_message_importance);
   balloon->say(this,saved_pending_message,_tcslen(saved_pending_message),FALSE,add_to_old,
					 saved_alternative_spoken_pending_message,FALSE,add_to_old);
   previous_importance = pending_message_importance;
   pending_message_importance = -1;
   dont_interrupt = pending_message_dont_interrupt;
   pending_message_dont_interrupt = FALSE;
};

const city_coordinate max_martian_llx = 16*tile_width;

void Martian::keep_out_of_the_way(TTRegion &programmer_region) {
	if (tt_city->stopped()) { // && tt_log_version_number > 14) { // added 250299 so you can tell Marty about comments
		return;
	};
	if (current_parameter() == TELEPORT_IN ) return;
	// removed animation_in_progress() || on 171002 since that includes things like waiting for speech to finish
	if (balloon == NULL || 
		 (!balloon->visible() && 
		  tt_marty_talk != MARTY_SPEAKS && tt_marty_talk != MARTY_SPEAKS_WITH_SUBTITLES && tt_marty_talk != MARTY_WITH_SUBTITLES) 
		 || !still_talking()) {
		if (pending_message != NULL) {
         say_pending_message();
      } else if (tt_system_mode == PUZZLE) {
         teleport_out(FALSE);
		} else if (ready_to_move_off_screen_time == 0) {
//			if (balloon == NULL) {
				// no balloon so move low to get out of way after 1/4 second
				// if it didn't wait might just start down and then up again
				ready_to_move_off_screen_time = tt_current_time+250;
//			};
		} else if (tt_current_time > ready_to_move_off_screen_time) {
			if (teleport_out_when_nothing_to_say) {
				teleport_out(FALSE);
//				teleport_out_when_nothing_to_say = FALSE; // moved on 111100
			} else {
				animate_location(llx,-4*tile_height,500); // was -11/2 
				ready_to_move_off_screen_time = 0;
			};
		};
		return; // not ready yet
	} else if (lly != favorite_floor_y) {// && !animation_in_progress()) {
		animate_location(llx,favorite_floor_y,300); // move back
	};
//	tt_error_file() << llx << "," << lly << "-" << true_y_offset() << " ---- ";
//	city_coordinate foo_x,foo_y;
//	balloon->lower_left_corner(foo_x,foo_y);
//   tt_error_file() << foo_x << "," << foo_y << endl;
	ready_to_move_off_screen_time = 0;
	city_coordinate programmer_center_x = (programmer_region.min_x+programmer_region.max_x)/2;
	if (programmer_center_x > 8*tile_width && programmer_center_x < 12*tile_width) {
//       (programmer_region.width() > 10*tile_width)) { // probably holding magic wand
		if (center_counter > 1) {
			// programmer too near the center -- so stay put rather than bounce
			// back and forth
			return;
		} else {
			center_counter++;
		};
	} else {
		center_counter = 0;
	};
	city_coordinate original_llx = llx;
	TTRegion my_region;
	full_region(my_region);
//	boolean collided_with_balloon = TRUE;
	boolean jumped = FALSE;
//	city_coordinate balloon_llx,balloon_lly;
//	balloon->lower_left_corner(balloon_llx,balloon_lly);
	if (still_talking()) {
		TTRegion balloon_region;
		balloon->full_region(balloon_region);
		my_region.extend_with(balloon_region);
	};
	boolean colliding = my_region.overlaps(programmer_region);
	city_coordinate jump_right_x = max_martian_llx-3*tile_width;
	city_coordinate jump_left_x = 3*tile_width;
	city_coordinate delta_x;
	city_coordinate left_delta;
	city_coordinate right_delta;
   left_delta = my_region.max_x-programmer_region.min_x;
	right_delta = programmer_region.max_x-my_region.min_x;
	if (left_delta < right_delta) {
		delta_x = -left_delta;
	} else {
		delta_x = right_delta;
	};
   // above was in colliding branch of conditional prior to 110402 but that left delta_x unbound when jumping
	if (colliding) {
		if (delta_x < -4*tile_width || delta_x > 4*tile_width) {
			// big enough move to bother -- otherwise looks bad
			move_by(delta_x,0);
		};
		if (llx < 0) {
			// too far to left so wrap around
			jumped = TRUE;
			move_to(jump_right_x,lly);
		} else if (llx > max_martian_llx) {
			jumped = TRUE;
			move_to(jump_left_x,lly);		
		};
//		balloon->move_by(llx-original_llx,0);
	} else if (llx < jump_left_x) {
		move_to(jump_left_x,lly);
		jumped = TRUE;
	} else if (llx > jump_right_x) {
		move_to(jump_right_x,lly);
		jumped = TRUE;
	};
	if (jumped) { // see if still colliding (like hold a big thing)
//		if (collided_with_balloon) {
			balloon->full_region(my_region);
//		} else {
//			full_region(my_region);
//		};
		if (my_region.overlaps(programmer_region)) {
			// still colliding and not any better then give up and stay where it was to start with
			city_coordinate new_delta_x;
			left_delta = my_region.max_x-programmer_region.min_x;
			right_delta = programmer_region.max_x-my_region.min_x;
			if (left_delta < right_delta) {
				new_delta_x = -left_delta;
			} else {
				new_delta_x = right_delta;
			};
			if (labs(new_delta_x) >= labs(delta_x)) {
				move_to(original_llx,lly);
			};
			return;
		};
//		full_region(my_region); // try again with Martian's region
//		if (my_region.overlaps(programmer_region)) {
//			move_to(original_llx,lly);
//			return;
//		};
	};
};

const int max_message = 20000;

//boolean Martian::stop_generating_help() {
//  if (saved_message_stream == NULL) return(FALSE); // describe called without Marty
//  return(saved_message_stream->pcount() > max_message-256);
//};

void Martian::dont_wait_to_give_help(boolean override_old) {
   give_help_without_waiting = TRUE;
   // so new is more likely to replace old...
   if (override_old) {
      previous_importance--;
//   pending_message_importance--;
      set_potential_help_target(NULL);
   };
};
		
void Martian::give_help(Sprite *current_selection, Sprite *tool_in_hand) {
	if (!active()
#if TT_CALL_FOR_MARTY
		&& tt_call_for_marty == NULL
#endif
		) return; // too early
	if (subject != NULL) return; // ready to receive a comment
	if (teleporting_out) return;
   if (tt_system_mode == PUZZLE) {
      puzzle_talk();
      return;
   };
	boolean talking = still_talking();
   // following was after just_started_selection stuff
  	if (talking &&
		 (dont_interrupt ||
		  ((potential_help_target == current_selection || potential_help_target == tool_in_hand) &&
			current_selection != NULL &&
			current_selection->clean_or_moved()))) return;
//	if (current_selection == NULL) {
//		current_selection = tt_help_target;
//	};
	if ((current_selection != NULL || tool_in_hand != NULL) &&
       !give_help_without_waiting &&
        tt_help_target == NULL) {  // ignore selections if giving directed help
      millisecond minimum_hold = minimum_hold_for_help;
#if TT_TEXT_TO_SPEECH
      if (text_to_speech()) { // was tt_marty_talk == MARTY_SPEAKS) {
         if (tool_in_hand == NULL) {
            minimum_hold = 6000; // 6 seconds
         } else {
            minimum_hold = 4000; // 4 seconds
         };
      };
#endif
		boolean just_started_selection = (tt_current_time-start_of_current_selection < minimum_hold);
		if (potential_help_target != NULL &&
          (potential_help_target == current_selection || potential_help_target == tool_in_hand)) {
			if (// tt_help_target != current_selection && tt_help_target != tool_in_hand &&
             !next_selection_is_target) {
				if (just_started_selection) { //  && !give_help_without_waiting
					return; // just started pointing to it
				} else if (start_of_current_selection == 0) {
					return; // already explained this thing
				} else { // first time
					start_of_current_selection = 0; // so don't explain it multiply
				};
			};
		} else {
         if (tool_in_hand != NULL) {
			   set_potential_help_target(tool_in_hand);
         } else {
            set_potential_help_target(current_selection);
         };
			if (next_selection_is_target) { // new on 130201 since with subtitles can be several second delay before help shows up
				tt_help_target = potential_help_target;
				next_selection_is_target = FALSE; // used up
			};
			start_of_current_selection = tt_current_time;
//			if (!talking) { // so why wait?
//				start_of_current_selection += minimum_hold_for_help;
//			};
			if (tt_help_target == NULL ||
				 (tt_help_target != current_selection && 
				  tt_help_target != tool_in_hand && !next_selection_is_target)) {
//				if (new_selection) {
//					remove_text();
//				};
				return;
			};
		};
	} else {
		set_potential_help_target(NULL);
	};
#if TT_DEBUG_ON
	millisecond start;
	if (tt_debug_mode == 3) {
		start = timeGetTime();
	};
#endif
	int importance = 0; // unimportant unless stated otherwise
#if TT_NEW_IO
	string buffer = new character[max_message+max_message/10]; 
	output_string_stream message(buffer,max_message);
#else
	const long buffer_max = max_message+max_message/10; // 250399 added 10% slop for overruns on very long descriptions
	string buffer = new character[buffer_max];
	output_string_stream message(buffer,buffer_max);
	string alternative_spoken_buffer = NULL;
	if (alternative_language_spoken()) {
		alternative_spoken_buffer = new character[buffer_max];
	};
	output_string_stream alternative_spoken_message(alternative_spoken_buffer,max_message);
#endif
	tt_current_output_stream = &message; // only used within calls below this
   tt_current_output_stream_size = max_message;
	if (pending_message != NULL &&
		 (pending_message_importance > 3 ||
		  (current_selection == NULL && tool_in_hand == NULL))) {
		// no point generating help since pending stuff too important
		message << pending_message;
		delete [] pending_message;
		pending_message = NULL;
		importance = pending_message_importance;
		previous_importance = pending_message_importance;
		pending_message_importance = -1;
		dont_interrupt = pending_message_dont_interrupt;
		pending_message_dont_interrupt = FALSE;
	} else if (!staying_around()) { 
		// new on 210300 to prevent giving help after beaming in with a bit of advice or a warning
		// note that this is necessary for intro.dmo in Brazilian but could it interfere with other demos?
		delete [] buffer;
		if (alternative_spoken_buffer != NULL) delete [] alternative_spoken_buffer;
		tt_current_output_stream = NULL; // new on 021204
		return;
	} else {
		if (current_selection != NULL &&
			 current_selection->pointer_to_leader() != NULL) {
			switch (current_selection->pointer_to_leader()->kind_of()) {
				case NEST:
				case TRUCK:
					current_selection = current_selection->pointer_to_leader();
			};
		};
		if (on_floor) { 
			if (next_selection_is_target) {
				// still beaming in so make this special to get more help
				if (tool_in_hand != NULL) {
					tt_help_target = tool_in_hand;
				} else {
					tt_help_target = current_selection;
				};
				if (tt_help_target != NULL) {
					user_did(GLOBAL_HISTORY,USER_HAS_TARGETED_HELP);
					// following moved into conditional on 061000
					greeting_needed = FALSE;
					next_selection_is_target = FALSE;
				};
			} else if ((tool_in_hand == NULL || tool_in_hand != tt_help_target) &&
						  current_selection != NULL && 
						  tt_help_target != current_selection) {
				tt_help_target = NULL;
			};
		};
		if (current_selection == NULL && tool_in_hand == NULL) {
			if (!dont_interrupt && talking && !saying_greeting && !greeting_needed) {
				// probably what is being said is no longer true
//				remove_text(); 
				// commented out on 121099 since this broke warnings from Marty - e.g. that a robot somewhere stopped because ...
//#if !TT_NEW_IO
				delete [] buffer;
				if (alternative_spoken_buffer != NULL) delete [] alternative_spoken_buffer;
//#endif
				return;
			};
		} else {
         Sprite *target;
         if (tool_in_hand == NULL) {
            target = current_selection;
         } else {
            target = tool_in_hand;
         };
			if (alternative_language_spoken()) { // do it over again with alternate language
				tt_use_spoken_alternative = TRUE;
				UnwindProtect<int> save(tt_help_counter); // so counter is restored
				give_help2(current_selection,tool_in_hand,alternative_spoken_message,TRUE,importance,target);
				tt_use_spoken_alternative = FALSE;
			};
			give_help2(current_selection,tool_in_hand,message,FALSE,importance,target);
		};
	};
	int size = message.LENGTH;
//#if TT_NEW_IO
//	string buffer = copy_string(message.STR);
//#else
//	buffer[size] = NULL; // terminate string
//#endif
	if (size == max_message) {
		buffer[max_message-3] = '.';
		buffer[max_message-2] = '.';
		buffer[max_message-1] = '.';
	};
	if (size <= 1) {// && tt_give_advice) {
		if (!greeting_needed && // maybe timer will cause one to be needed
			 tt_current_time-time_of_last_help > time_between_greetings) {
			greeting_needed = TRUE;
			timer_trigger_help = TRUE;
		};
		if (greeting_needed) {
			if (alternative_language_spoken()) { // do it over again with alternate language
				tt_use_spoken_alternative = TRUE;
				give_greeting(alternative_spoken_message,TRUE);
				tt_use_spoken_alternative = FALSE;
			};
			give_greeting(message,FALSE);
			size = message.LENGTH;
			importance = 2;
         set_previous_target(NULL);
		};
	};
	if (size > 0) {
      give_help_without_waiting = FALSE;
		time_of_last_help = tt_current_time; // reset timer
		greeting_needed = FALSE;
		timer_trigger_help = FALSE;
	};
	if (size > 0 || !saying_greeting) {
		message.put('\0'); // seems to fix problems with untruncated strings passed to talk balloons
		if (alternative_spoken_buffer != NULL) {
			alternative_spoken_message.put('\0');
		};
		say(buffer,importance,alternative_spoken_buffer);
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 3 && current_selection != NULL) {
		tt_error_file() << "Total time to generate help for ";
		current_selection->describe_type(tt_error_file());
		tt_error_file() << " was " << (timeGetTime()-start) << endl;
	};
#endif
   delete [] buffer;
	if (alternative_spoken_buffer != NULL) delete [] alternative_spoken_buffer;
};

void Martian::give_help2(Sprite *current_selection, Sprite *tool_in_hand,
								 output_stream &message, boolean alternate, int &importance, Sprite *target) {
	if (on_floor &&
		 tt_help_history_index == tt_last_history_index &&
		 tt_help_history_action == tt_last_history_action) {
		if (!alternate) { // reset it so this happens only once
			tt_help_history_index = -1; // only user of this info??
			tt_help_history_action = -1;
         remove_text(FALSE);
			importance = 5;
		};
		message << S(IDS_GOOD1+tt_frame_number%3,"",alternate); // simplified on 030999
	} else {
		if (tt_help_target != NULL &&
			 (tt_help_target == current_selection ||
			  tt_help_target == tool_in_hand)) {
			  // explictly asked for help about this
			if (!alternate) {
				importance = 4;
			};
			tt_help_counter++; // moved here from conditional above on 290999 - saved and reset by caller
			if (tt_help_counter == 1 && tt_help_target->true_operation() == NO_NUMBER_OPERATION) { 
				// if an operation I don't say enough now to warrant the preface - new on 110202
				preface_full_help(message,tt_help_target);
			};
		} else {
			if (!alternate) {
				tt_help_counter = 0;
				importance = 2;
#if TT_TEXT_TO_SPEECH
            if (tt_marty_talk == MARTY_SPEAKS && target != previous_target) { // waited long enough so interrupt old
               remove_text(TRUE);
            };
#endif
			};
		};
		if (!alternate) {
			tt_help_history_index = target->history_index(); // was just kind_of
		};
#if TT_TEXT_TO_SPEECH
      if (!(tt_help_target == NULL && target == previous_target && text_to_speech() &&
            tt_current_time-started_generating_help < 10000)) {
          // do nothing if same target and just generated help from it less than 10 seconds ago
#endif
			if (target->infinite_stack() && tt_help_target != target) {
				 target->Sprite::generate_help(message,tool_in_hand,current_selection);
			} else {
				 target->generate_help(message,tool_in_hand,current_selection);
			};
			if (tool_in_hand != NULL && !did_user(USER_HAS_DROPPED) && tt_help_counter == 0) {
				message << S(IDS_DROP_HELP) << space(SPACE_BEFORE_PRONOUN);
            print_it(tool_in_hand->kind_of(),message);
            message << space() << S(IDS_PICK_UP_HELP2,"") << space();
				describe_left_button(message);
				message << S(IDS_PICK_UP_HELP3,"") << end_sentence();
			};
#if TT_TEXT_TO_SPEECH
         if (!alternate) {
				set_previous_target(target);
				started_generating_help = tt_current_time;
         };
#endif
		};
	};
};

void Martian::preface_full_help(output_stream &message, Sprite *subject) {
	message << S(IDS_PREFACE_HELP,"");
	subject->describe_type(message,PLURAL_ARTICLE,FALSE);
	message << S(IDS_PREFACE_HELP2,""); // new on 270100
	message << end_sentence();
};

#if TT_TEXT_TO_SPEECH
flag marty_talks_funny_said = FALSE;
#endif

void Martian::give_greeting(output_stream &message, boolean alternate) {
	if (!alternate) {
		saying_greeting = TRUE;
		dont_interrupt = (flag) !timer_trigger_help;
	};
	if (!timer_trigger_help) {
		say_hi(message);
	};
	if (!did_user(USER_HAS_RECALLED_MARTIAN)) {
      if (last_greeting_help != IDS_RECALL_MARTIAN_HELP) {
		   give_name(message);
// commented out on 081105 since with general text to speech engines sounds good and this wastes time
//#if TT_TEXT_TO_SPEECH
//         if (text_to_speech() && return_use_count() == 1 && !marty_talks_funny_said && !alternative_language_spoken()) { 
//				// first use
//            message << S(IDS_MARTY_TALKS_FUNNY);
//            if (!alternate) marty_talks_funny_said = TRUE;
//         };
//#endif
         if (!alternate) last_greeting_help = IDS_RECALL_MARTIAN_HELP;
	     	message << S(IDS_RECALL_MARTIAN_HELP)
				     << S(IDS_IDS_RECALL_MARTIAN_HELP2);
		   if (!did_user(USER_HAS_SAT)) { // must be standing if never sat
//			   if (tt_log_version_number < 15 && tt_language == AMERICAN) {
//					message << S(IDS_SITTING_HELP2); // without parenthetical remark so timing is OK for intro.dmo
//				} else {
					message << S(IDS_SITTING_HELP);
//				};
         };
		};
	} else if (!did_user(USER_HAS_SAT)) {
      if (last_greeting_help != IDS_SITTING_HELP) {
         if (!alternate) last_greeting_help = IDS_SITTING_HELP;
//			if (tt_log_version_number < 15 && current_language() == AMERICAN) {
//				message << S(IDS_SITTING_HELP2); // without parenthetical remark so timing is OK for intro.dmo
//			} else {
		      message << S(IDS_SITTING_HELP);
//			};
      };
	} else if (tt_programmer->pointer_to_background()->on_the_floor() && !did_user(USER_HAS_LEFT_FLOOR)) {
		// on 021205 replaced floor->on_the_floor() with tt_programmer->pointer_to_background()->on_the_floor()
      if (last_greeting_help != IDS_LEAVE_FLOOR_HELP) {
         if (!alternate) last_greeting_help = IDS_LEAVE_FLOOR_HELP;
		   message << S(IDS_LEAVE_FLOOR_HELP)
				     << S(IDS_LEAVE_FLOOR_HELP2);
      };
   } else if (!did_user(USER_HAS_TYPED_PAGE_DOWN)) {
      if (last_greeting_help != IDS_TYPING_PAGE_UP_AND_DOWN_HELP) {
         if (!alternate) last_greeting_help = IDS_TYPING_PAGE_UP_AND_DOWN_HELP;
         message << S(IDS_TYPING_PAGE_UP_AND_DOWN_HELP)
                 << S(IDS_TYPING_PAGE_UP_AND_DOWN_HELP2);
      };
	} else if (!did_user(USER_HAS_PAUSED)) {
      if (last_greeting_help != IDS_PAUSE_HELP) {
         if (!alternate) last_greeting_help = IDS_PAUSE_HELP;
		   message << S(IDS_PAUSE_HELP);
      };
	} else if (!did_user(USER_HAS_TARGETED_HELP)) {
      if (last_greeting_help != IDS_TARGETED_HELP_HELP) {
         if (!alternate) last_greeting_help = IDS_TARGETED_HELP_HELP;
		   message << S(IDS_TARGETED_HELP_HELP);
      };
	} else if (!did_user(USER_LOOKED_AT_OPTIONS_NOTEBOOK)) {
      if (last_greeting_help != IDS_OPTIONS_NOTEBOOK_HELP1) {
         if (!alternate) last_greeting_help = IDS_OPTIONS_NOTEBOOK_HELP1;
		   message << S(IDS_OPTIONS_NOTEBOOK_HELP1)
				     << S(IDS_OPTIONS_NOTEBOOK_HELP2)
                 << S(IDS_OPTIONS_NOTEBOOK_HELP3);
      };
	} else if (!did_user(USER_HAS_STOPPED_ALL)) {
      if (last_greeting_help != IDS_F8_HELP1) {
         if (!alternate) last_greeting_help = IDS_F8_HELP1;
	     	message << S(IDS_F8_HELP1)
                 << S(IDS_F8_HELP2)
                 << S(IDS_F8_HELP3);
      };
   } else if (!did_user(USER_HAS_GENERATED_JAVA_APPLET)) {
      if (last_greeting_help != IDS_GENERATE_JAVA_APPLET_HELP) {
         if (!alternate) last_greeting_help = IDS_GENERATE_JAVA_APPLET_HELP;
	     	message << S(IDS_GENERATE_JAVA_APPLET_HELP);
      };
	} else if (!did_user(USER_HAS_HURRIED_UP)) {
      if (last_greeting_help != IDS_HURRY_UP_HELP) {
         if (!alternate) last_greeting_help = IDS_HURRY_UP_HELP;
		   message << S(IDS_HURRY_UP_HELP)
				     << S(IDS_HURRY_UP_HELP2);
      };
	} else {
		dont_interrupt = FALSE; // just greeting and no info so...
	};
	if (!alternate) {
		greeting_needed = FALSE;
		timer_trigger_help = FALSE;
	};
};

void Martian::say_hi(output_stream &message) {
	int hello_id = IDS_HELLO1+my_random(6);
	if (current_language() == BRITISH && hello_id == IDS_HELLO2) { // "Howdy" isn't very British
		hello_id = IDS_HELLO3;
	};
	message << S(hello_id);
   if (!limited_text_to_speech()) { // can't pronounce every name
	   message << space() << tt_user_name;
   };
	if (my_random(2)) {
		message << end_sentence();
	} else {
		message << end_sentence('!');
	};
};

void Martian::give_name(output_stream &message) {
	if (first_time) {
		message << S(IDS_MARTIAN_INTRODUCTION);
		if (!tt_use_spoken_alternative) first_time = FALSE;
	};
};

void Martian::say_completed() {
	dont_interrupt = FALSE;
//	if (tt_log_version_number > 12) 
	greeting_needed = FALSE;
	if (tt_titles_ended_on_frame <= tt_frame_number) { // condition new on 050805 since credits aren't really help
		time_of_last_help = tt_current_time;
	};
	saying_greeting = FALSE;
   if (current_parameter() == TELEPORT_OUT) return;
   if (pending_message != NULL) {
      say_pending_message();
   } else if (tt_system_mode == PUZZLE && tt_programmer->kind_of() != PROGRAMMER_TITLES) {
      martian_stopped_talking();
   } else {
      set_parameter(MARTY_WAITING);
   };
//#if TT_TEXT_TO_SPEECH
//   if (tt_programmer->kind_of() != PROGRAMMER_TITLES) { // until SAPI 3.0
//      turn_on_sound_and_speech_off();
//   };
//#endif
};

void Martian::action_aborted(int reason, Sprite *by, SpriteType actor, int importance, string default_string) {
	// switch color -- use deconstructor style extension
// following commented out because a high priority "say" does it as well
//	if (!active() && floor != NULL) { // not active and not outside
//		teleport_in(FALSE); // should he teleport out again after some time??
//	};
//	remove_text();
   if (tt_system_mode == PUZZLE) return; // give info some other way??
#if TT_NEW_IO
	output_string_stream message;
#else
	string buffer = new character[max_message];
	output_string_stream message(buffer,max_message);
	string alternative_spoken_buffer = NULL;
	if (alternative_language_spoken()) {
		alternative_spoken_buffer = new character[max_message];
	};
	output_string_stream alternative_spoken_message(alternative_spoken_buffer,max_message);
#endif
   action_aborted2(message,reason,by,actor,importance,default_string);
	if (alternative_language_spoken()) {
		tt_use_spoken_alternative = TRUE;
		action_aborted2(alternative_spoken_message,reason,by,actor,importance);
		tt_use_spoken_alternative = FALSE;
	};
	if (importance < 0) { // i.e. use defaults
      if (by->still_running()) { // by->kind_of() == ROBOT) { // || tt_log_version_number == 3) {
	      importance = 4; // very important
      } else {
 	      importance = 3; // less important
      };
   };
	say(message.STR,importance,alternative_spoken_buffer);
	if (importance > 3) log(message.STR,FALSE,(importance > 4));
#if !TT_NEW_IO
	delete [] buffer;
	if (alternative_spoken_buffer != NULL) delete [] alternative_spoken_buffer;
#endif
	no_interruptions(); //	dont_interrupt = TRUE;
};

void Martian::action_aborted2(output_stream &message,
										int reason, Sprite *by, SpriteType actor, int &importance, string default_string) {
#if TT_CALL_FOR_MARTY
	if (!tt_marty_appears_only_if_called) // so don't get lots of excuse mes if F1 pressed
#endif
	message << S(IDS_EXCUSE_ME1); 
	// used to be +my_random(3) but that made Marty repeat himself when describing a repeated problem
//	Article article;
	long id_after_reason;
	long id_after_name;
	if (by->kind_of() == ROBOT) { // restored on 151202 - was by->still_running() but may not be running anymore...
		Background *background_of_by = by->pointer_to_background();
		if (tt_programmer->pointer_to_background() == background_of_by) { 
			// || background_of_by == NULL) { // second disjunct added 090400 -- removed 300704 
			// prior to 281004 used to test if floor == background_of_by but floor might be NULL
			// and added background_of_by == NULL != NULL tests below
			// first disjunct was == prior to 151202 but that makes no sense
			// but it did make sense and is restored on 300704 so that robots are named
			// when they aren't running on the floor in front of you
//			article = DEFINITE_ARTICLE; // should be clear
			message << S(IDS_ROBOT_ABORT);
			id_after_reason = IDS_ROBOT_ABORT2;
		} else {
			if (background_of_by == NULL != NULL && background_of_by->pointer_to_picture() != NULL) { 
//				article = INDEFINITE_ARTICLE; 
				message << S(IDS_ROBOT_ON_PICTURE_STOPPED);
				id_after_name = IDS_ROBOT_ON_PICTURE_STOPPED2;
			} else if (background_of_by == NULL != NULL && floor != NULL && 
				        floor->pointer_to_room() == background_of_by->pointer_to_room()) {
//				article = INDEFINITE_ARTICLE;
				message << S(IDS_ROBOT_ON_FLOOR_STOPPED);
				id_after_name = IDS_ROBOT_ON_FLOOR_STOPPED2;
			} else {
//				article = INDEFINITE_ARTICLE;
				message << S(IDS_REMOTE_ROBOT_ABORT);
				id_after_name = IDS_REMOTE_ROBOT_ABORT2;
			};
         character name_string[max_resource_string_length]; 
         ((Robot *) by)->name(name_string);
         message << space() << name_string << space();
			if (id_after_name != 0) {
				message << S(id_after_name,"");
			};
			message << S(IDS_STOPPED_BECAUSE_HE) << space();
			id_after_reason = IDS_STOPPED_BECAUSE_HE2;
			// should offer to take user there
		};
	} else {
//		article = DEFINITE_ARTICLE; // should be clear
		message << S(IDS_PROGRAMMER_ABORT);
		id_after_reason = IDS_PROGRAMMER_ABORT2;
	};
//	if (strncmp(reason,"it",2) == 0) {
//		message << reason;
//	} else {
	long extra_reason_id = 0;
	switch (actor) {
		case MOUSE_WITH_HAMMER:
			message << S(IDS_MOUSE_DIDNT_COME_OUT,"");
			extra_reason_id = IDS_MOUSE_DIDNT_COME_OUT2;
			break;
		case TRUCK:
			message << S(IDS_TRUCK_DIDNT_LEAVE_BECAUSE,"");
//			article = DEFINITE_ARTICLE; // typically because THE city is full
			extra_reason_id = IDS_TRUCK_DIDNT_LEAVE_BECAUSE2;
			break;
	};
//		print_article(article,message,reason);
	if (reason != 0) {
		message << S(reason,default_string);
	} else {
		message << S(IDS_SOMETHING_WRONG); // new on 050201
	};
	if (id_after_reason != 0) {
		message << S(id_after_reason,"");
	};
	if (extra_reason_id != 0) {
		message << S(extra_reason_id,"");
	};
	if (reason != 0) { // conditional added 050201
		message << end_sentence();
	};
//   if (reason != NULL) delete [] reason; // commented out on 210599 since came from an S(...) call
   message.put('\0');
};

void Martian::remove_text(boolean pending_message_too) {
	if (pending_message != NULL && pending_message_too) { // && !pending_message_dont_interrupt) {
		delete [] pending_message;
		pending_message = NULL;
		pending_message_importance = -1;
	};
	if (balloon != NULL) {
		balloon->remove_text();
	};
	previous_importance = -1;
};

void Martian::no_interruptions() {
	dont_interrupt = TRUE;
	if (pending_message != NULL) { // really??
		pending_message_dont_interrupt = TRUE;
	};
};

#if TT_TEXT_TO_SPEECH

//void speak_next_sentence_callback(void *b) {
//   ((Talk_Balloon *) b)->speak_next_sentence();
//};

void Martian::speech_has_finished_by_callback() { // restructured on 151102
   if (saved_say_id >= 0) {
		int saved_saved_say_id = saved_say_id; // since speech_has_finished may schedule a new callback and set saved_say_id
		saved_say_id = -1;
#if TT_DEBUG_ON
		if (tt_debug_mode == 110401) {
			tt_error_file() << "At " << tt_current_time << " simulated speech finished id = " << saved_saved_say_id << endl;
		};
#endif
      speech_has_finished(saved_saved_say_id);
   };
};

void Martian::speech_has_finished(long paragraph_id) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 31203) {
		tt_error_file() << "speech_has_finished; paragraph_id=" << paragraph_id << " at " << timeGetTime() << endl;
	};
#endif
   if (balloon == NULL || tt_shutting_down || !still_talking() || currently_paused()) {
		// added currently_paused() on 290999 so won't start the next sentence when paused
		previous_importance = -1; // new on 290301 -- otherwise with subtitles Marty can shut up trying to add_to_old incorrectly
		// this wasn't the problem but left it anyway since it seems "tidier"
      return;
   };
   if (tt_system_mode == PUZZLE && !inside_rocket()) return;
   if (currently_paused()) {
      // otherwise signals keeps Marty talking even when iconized
//      balloon->do_after(100,this,speak_next_sentence_callback);
      balloon->remove_text();
		previous_importance = -1; // new on 290301
      return;
   };
   if (tt_marty_talk != MARTY_SPEAKS && tt_marty_talk != MARTY_SPEAKS_WITH_SUBTITLES && tt_marty_talk != MARTY_WITH_SUBTITLES) {
//      tt_martian-> -- removed on 090502 since now there is a clone of Marty giving the titles
      show_balloon();
   };
   if (balloon->speech_has_finished(paragraph_id == balloon->current_say_id())) {
		previous_importance = -1; // new on 290301
	};
};

#endif

void Martian::just_talk_balloons_for(millisecond duration) {
#if TT_TEXT_TO_SPEECH
	if (!tt_resetting_or_destroying_city &&
       (tt_marty_talk == MARTY_SPEAKS || tt_marty_talk == MARTY_SPEAKS_AND_TALK_BALLOONS || tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES || tt_marty_talk == MARTY_WITH_SUBTITLES)) {
      // tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 but can cause memory errors while resetting during time travel
		saved_tt_marty_talk = tt_marty_talk;
      new_talk_mode(MARTY_TALK_BALLOONS);
      tt_global_picture->do_after(duration,NULL,MARTY_SPEAKS_CALLBACK);
	};
#endif
};

ReleaseStatus Martian::item_released_on_top(Sprite *item, Sprite *, SpritePointer &, int ) {
#if TT_NEW_IO
	output_string_stream message;
#else
	char message_string[1024];
	output_string_stream message(message_string,1024);
#endif
	if (subject == NULL) { // first thing given to Marty
		Sprite *comment = item->pointer_to_comment();
		if (comment == NULL) {
			message << S(IDS_GIVE_ME_AND_ILL_TALK_ABOUT_IT) << space();
			item->describe_type(message,DEFINITE_ARTICLE);
			message << end_sentence();
		} else {
			comment = comment->copy(); // set_comment destroys the old comment
			comment->make_sound(); // no-op if not a sound
//			item->set_comment(NULL); // Marty has his copy now
			comment->set_size_and_location(1,1,item->center_x(),item->center_y());
			floor->add_item(comment);
			tt_screen->add(comment);
			city_coordinate comment_good_width = comment->good_width();
			city_coordinate comment_good_height = comment->good_height();
			comment->animate_size_and_location(comment_good_width,comment_good_height,
														  comment->current_llx()-comment_good_width/2,
														  comment->current_lly()-comment_good_height/2,
														  default_duration(3000));
			dont_wait_to_say_more();
			message << S(IDS_HERES_A_COPY_OF) << space();
			comment->describe_briefly(message,DEFINITE_ARTICLE);
			message << S(IDS_HERES_A_COPY_OF2,"") << end_sentence() << S(IDS_IT_GOES_WITH) << space();
			item->describe_type(message,DEFINITE_ARTICLE);
			message << S(IDS_IT_GOES_WITH2,"") << end_sentence();
		};
		set_subject(item); // for next drop
		// Marty should hold the subject until power back on...
		subject->remove_from_floor(TRUE,FALSE);
		add_follower(subject,FALSE);
	} else {
		dont_wait_to_say_more();
		subject->set_comment(item->copy());
		message << S(IDS_OK) << end_sentence() << S(IDS_ILL_REMEMBER_THAT) << space();
		item->describe_briefly(message,INDEFINITE_ARTICLE);
		message << space() << S(IDS_GOES_WITH) << space();
		subject->describe_briefly(message,DEFINITE_ARTICLE);
		message << end_sentence();
		remove_follower(subject);
		floor->add_item(subject);
		set_subject(NULL);
		floor->give_item_priority(item);
	};
	message.put('\0'); // terminate string
	if (limited_text_to_speech()) just_talk_balloons_for(30000); // 30 seconds should be enough
	say(message.STR,6);	
	no_interruptions();
	return(RELEASE_ACCEPTED); 
};

void Martian::power_toggled() {
	if (!tt_city->stopped()) {
		// restore things even if not visible just now -- moved here on 181299
		if (subject != NULL) {
			remove_follower(subject);
			if (floor != NULL) floor->add_item(subject);
//			tt_screen->add(subject);
			set_subject(NULL);
		};
		set_priority(min_long);
		set_graspable(FALSE);
		set_selectable(FALSE);
	};
	if (floor == NULL) return; // not visible so ignore this  || tt_log_version_number < 15
	if (tt_city->stopped()) {
		floor->give_item_priority(this); // so Marty sinks below your hand, etc.
		if (balloon != NULL) balloon->set_priority(min_long+1); // below only hand
		set_graspable(TRUE);
		set_selectable(TRUE); // for fun
	};
};

void Martian::remove_selected_element(Sprite *element, SelectionReason reason, Sprite *by, boolean add_to_floor) {
	if (element == subject) {
		set_subject(NULL);
		say(IDS_OK,4); // when removing something from Marty... 
		no_interruptions();
	};
	Sprite::remove_selected_element(element,reason,by,add_to_floor);
};

Sprite *Martian::select(TTRegion *, SelectionReason reason, Sprite *) {
	switch (reason) {
		case VACUUM_USED:
		case MIGHT_USE_VACUUM:
		case VACUUM_USED_TO_BLANK:
		case COPIER_USED:
		case MIGHT_USE_COPIER:
			return(NULL); // new on 120505
		default:
			return(this);
	};
};

#if TT_XML

void Martian::xml(xml_element *element, xml_document *document) {
   // checked on 151102 that all state varaiables included here
   if (on_floor) xml_set_attribute(element,L"OnFloor",1);
   if (keep_ignoring_button) xml_set_attribute(element,L"KeepIgnoringButton",1);
   if (dont_interrupt) xml_set_attribute(element,L"DontInterrupt",1);
   if (greeting_needed) xml_set_attribute(element,L"GreetingNeeded",1);
   if (next_selection_is_target) xml_set_attribute(element,L"NextSelectionIsTarget",1);
   if (first_time) xml_set_attribute(element,L"FirstTime",1);
   if (timer_trigger_help) xml_set_attribute(element,L"TimerTriggerHelp",1);
   if (saying_greeting) xml_set_attribute(element,L"SayingGreeting",1);
   if (teleport_out_when_nothing_to_say) xml_set_attribute(element,L"TeleportOutWhenNothingToSay",1);
   if (pending_message_dont_interrupt) xml_set_attribute(element,L"PendingMessageDontInterrupt",1);
   if (called_by_user_flag) xml_set_attribute(element,L"CalledByUser",1);
   if (teleporting_out) xml_set_attribute(element,L"TeleportingOut",1);
   if (give_help_without_waiting) xml_set_attribute(element,L"GivingHelpWithoutWaiting",1);
   if (ok_to_repeat) xml_set_attribute(element,L"OKToRepeat",1);
   xml_set_attribute(element,L"TimeOfLastHelp",time_of_last_help);
   xml_set_attribute(element,L"TimeBetweenGreetings",time_between_greetings);
   xml_set_attribute(element,L"ReadyToMoveOffScreenTime",ready_to_move_off_screen_time);
   xml_set_attribute(element,L"FavoriteFloorX",favorite_floor_x);
   xml_set_attribute(element,L"FavoriteFloorY",favorite_floor_y);
   xml_set_attribute(element,L"FavoriteRoomX",favorite_room_x);
   xml_set_attribute(element,L"FavoriteRoomY",favorite_room_y);
   xml_set_attribute(element,L"PreviousImportance",previous_importance);
   xml_set_attribute(element,L"PendingMessageImportantance",pending_message_importance);
   xml_set_attribute(element,L"CenterCounter",center_counter);
   xml_set_attribute(element,L"StartOfCurrentSelection",start_of_current_selection);
   xml_set_attribute(element,L"StartedGeneratingHelp",started_generating_help);
   xml_set_attribute(element,L"SavedTTMartyTalk",saved_tt_marty_talk);
   xml_set_attribute(element,L"LastGreetingHelp",last_greeting_help);
   if (saved_say_id != -1) { // no need to save default value
      xml_set_attribute(element,L"SavedSayId",saved_say_id); // new on 151102
   };
   if (pending_message != NULL) {
      xml_append_child(xml_character_data_element(pending_message,L"PendingMessage",document),element);
      // rewritten since an attribute can't handle special characters
//      xml_set_attribute(element,L"PendingMessage",pending_message);
   };
   if (alternative_spoken_pending_message != NULL) {
      xml_append_child(xml_character_data_element(alternative_spoken_pending_message,L"AlternativeSpokenPendingMessage",document),element);
   };
   if (last_thing_said[0] != NULL) {
      xml_append_child(xml_character_data_element(last_thing_said[0],L"LastThingSaid0",document),element);
   };
   if (last_thing_said[1] != NULL) {
      xml_append_child(xml_character_data_element(last_thing_said[1],L"LastThingSaid1",document),element);
   };
	// don't need to check the following for independence for time travel include segments since is never ok_to_cache
   if (potential_help_target != NULL) {	
      potential_help_target->add_xml(element,L"PotentialHelpTarget",document);
   };
   if (teleport_in_selection != NULL) {
      teleport_in_selection->add_xml(element,L"TeleportInSelection",document);
   };
   if (teleport_in_tool_in_hand != NULL) {
      teleport_in_tool_in_hand->add_xml(element,L"TeleportInToolInHand",document);
   };
   if (previous_target != NULL) {
      previous_target->add_xml(element,L"PreviousTarget",document);
   };
   if (subject != NULL) {
      subject->add_xml(element,L"Subject",document);
   };
   if (balloon != NULL) {
		// don't need to check for independence for time travel include segments
      balloon->xml_create_and_append_element(element,document)->Release(); // "TalkBallon" is a fine tag...
   };
#if TT_CALL_FOR_MARTY
	if (tt_call_for_marty != NULL) { // new on 030805
		xml_set_attribute(element,L"MartyIconVisible",1);
	};
#endif
   Sprite::xml(element,document);
};


boolean Martian::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
      case POTENTIAL_HELP_TARGET_TAG: 
         set_potential_help_target(xml_get_sprite(node,potential_help_target));
			// was ordinary assignement prior to 210203 but ref counts get broken
         return(TRUE);
      case TELEPORT_IN_SELECTION_TAG:
         set_teleport_in_selection(xml_get_sprite(node,teleport_in_selection)); 
			// was ordinary assignement prior to 210203 but ref counts get broken
         return(TRUE);
      case TELEPORT_IN_TOOL_IN_HAND_TAG:
         set_teleport_in_tool_in_hand(xml_get_sprite(node,teleport_in_tool_in_hand)); 
			// was ordinary assignement prior to 210203 but ref counts get broken
         return(TRUE);
      case PREVIOUS_TARGET_TAG:
         set_previous_target(xml_get_sprite(node,previous_target)); // was just = prior to 221102 but that messes up ref counting
         return(TRUE);
      case SUBJECT_TAG:
         set_subject(xml_get_sprite(node,subject)); // was just = prior to 221102 but that messes up ref counting
         return(TRUE);
      case TALK_BALLOON_TAG: {
				Sprite *sprite = xml_load_sprite(node,tag,balloon,TALK_BALLOON);
				if (sprite == NULL || sprite->kind_of() != TALK_BALLOON) {
					return(FALSE);
				};
				initialize_balloon((Talk_Balloon *) sprite,TRUE);  // TRUE added 040204
				// rewritten on 211103 since the old way didn't use the GUIDs properly when the talk balloon was saved while animating
									  };
//         initialize_balloon(); // to set up connections
////         balloon = (Talk_Balloon *) xml_get_sprite(node);
//         xml_entity(node,balloon); // initialize my balloon
         return(TRUE);
      case PENDING_MESSAGE_TAG:
         pending_message = xml_get_narrow_text_copy(node);
         return(TRUE);
      case ALTERNATIVE_SPOKEN_PENDING_MESSAGE_TAG:
         alternative_spoken_pending_message = xml_get_narrow_text_copy(node);
         return(TRUE);
      case LAST_THING_SAID_0_TAG:
         last_thing_said[0] = xml_get_narrow_text_copy(node);
         return(TRUE);
      case LAST_THING_SAID_1_TAG:
         last_thing_said[1] = xml_get_narrow_text_copy(node);
         return(TRUE);
      case NO_MORE_TAGS:
         on_floor = xml_get_attribute_int(node,L"OnFloor",0); // prior to 210404 on_floor
         keep_ignoring_button = xml_get_attribute_int(node,L"KeepIgnoringButton",0); // prior to 210404 keep_ignoring_button
         dont_interrupt = xml_get_attribute_int(node,L"DontInterrupt",0); // prior to 210404 dont_interrupt
         greeting_needed = xml_get_attribute_int(node,L"GreetingNeeded",0); // prior to 210404 greeting_needed
         next_selection_is_target = xml_get_attribute_int(node,L"NextSelectionIsTarget",0); // prior to 210404 next_selection_is_target
         first_time = xml_get_attribute_int(node,L"FirstTime",0); // prior to 210404 first_time
         timer_trigger_help = xml_get_attribute_int(node,L"TimerTriggerHelp",0); // prior to 210404 timer_trigger_help
         saying_greeting = xml_get_attribute_int(node,L"SayingGreeting",0); // prior to 210404 saying_greeting
         teleport_out_when_nothing_to_say = xml_get_attribute_int(node,L"TeleportOutWhenNothingToSay",0); 
			// prior to 210404 teleport_out_when_nothing_to_say
         pending_message_dont_interrupt = xml_get_attribute_int(node,L"PendingMessageDontInterrupt",0); 
			// prior to 210404 pending_message_dont_interrupt
         called_by_user_flag = xml_get_attribute_int(node,L"CalledByUser",0); // prior to 210404 called_by_user_flag
         teleporting_out = xml_get_attribute_int(node,L"TeleportingOut",0); // prior to 210404 teleporting_out
         give_help_without_waiting = xml_get_attribute_int(node,L"GivingHelpWithoutWaiting",0); // prior to 210404 give_help_without_waiting
         ok_to_repeat = xml_get_attribute_int(node,L"OKToRepeat",0); // prior to 210404 ok_to_repeat
         time_of_last_help = xml_get_attribute_int(node,L"TimeOfLastHelp",time_of_last_help);
         time_between_greetings = xml_get_attribute_int(node,L"TimeBetweenGreetings",time_between_greetings);
         ready_to_move_off_screen_time = xml_get_attribute_int(node,L"ReadyToMoveOffScreenTime",ready_to_move_off_screen_time);
         favorite_floor_x = xml_get_attribute_int(node,L"FavoriteFloorX",favorite_floor_x);
         favorite_floor_y = xml_get_attribute_int(node,L"FavoriteFloorY",favorite_floor_y);
         favorite_room_x = xml_get_attribute_int(node,L"FavoriteRoomX",favorite_room_x);
         favorite_room_y = xml_get_attribute_int(node,L"FavoriteRoomY",favorite_room_y);
         previous_importance = xml_get_attribute_int(node,L"PreviousImportance",previous_importance);
         pending_message_importance = xml_get_attribute_int(node,L"PendingMessageImportantance",pending_message_importance);
         center_counter = xml_get_attribute_int(node,L"CenterCounter",center_counter);
         start_of_current_selection = xml_get_attribute_int(node,L"StartOfCurrentSelection",start_of_current_selection);
         started_generating_help = xml_get_attribute_int(node,L"StartedGeneratingHelp",started_generating_help);
         saved_tt_marty_talk = (MartyTalk) xml_get_attribute_int(node,L"SavedTTMartyTalk",saved_tt_marty_talk);
         last_greeting_help = xml_get_attribute_int(node,L"LastGreetingHelp",last_greeting_help);
         saved_say_id = xml_get_attribute_int(node,L"SavedSayId",-1); // prior to 210404 saved_say_id
#if TT_CALL_FOR_MARTY
			if (xml_get_attribute_int(node,L"MartyIconVisible",0)) {
				teleport_in(MARTY_CALLED_BY_LOADER);
			};
#endif
//         tt_global_picture->do_after(15000,this,SPEECH_HAS_FINISHED_CALLBACK); 
			// new on 151102 in case was talking so figure sentence is over in 15 seconds
         //pending_message = xml_get_attribute_string(node,L"PendingMessage",pending_message);
         //alternative_spoken_pending_message 
			// = xml_get_attribute_string(node,L"AlternativeSpokenPendingMessage",alternative_spoken_pending_message);
         //last_thing_said[0] = xml_get_attribute_string(node,L"LastThingSaid0",alternative_spoken_pending_message);
         //last_thing_said[1] = xml_get_attribute_string(node,L"LastThingSaid1",last_thing_said[1]);        
         // and do the following
      default:
         return(Sprite::handle_xml_tag(tag,node));
   };
};

#endif

boolean Martian::dump(output_stream &stream, boolean just_prepare, boolean is_controlee, boolean on_nest) {
   // new on 220999
	if (!Sprite::dump(stream,just_prepare,is_controlee,on_nest)) return(FALSE);
//	dump_geometry(stream); // commented out on 110100 since was redundant
	unsigned short flags = (on_floor |
									(keep_ignoring_button<<1) |
									(dont_interrupt<<2) |
									(greeting_needed<<3) |
									(next_selection_is_target<<4) |
									(first_time<<5) |
									(timer_trigger_help<<6) |
									(saying_greeting<<7) |
									(teleport_out_when_nothing_to_say<<8) |
									(pending_message_dont_interrupt<<9) |
									(called_by_user_flag<<10) |
									(teleporting_out<<11) |
									(give_help_without_waiting<<12) |
									(ok_to_repeat<<13));
	stream.write((char *) &flags,sizeof(flags));
	stream.write((char *) &time_of_last_help,sizeof(time_of_last_help));
	stream.write((char *) &time_between_greetings,sizeof(time_between_greetings));
	stream.write((char *) &ready_to_move_off_screen_time,sizeof(ready_to_move_off_screen_time));
	stream.write((char *) &favorite_floor_x,sizeof(favorite_floor_x));
	stream.write((char *) &favorite_floor_y,sizeof(favorite_floor_y));
	stream.write((char *) &favorite_room_x,sizeof(favorite_room_x));
	stream.write((char *) &favorite_room_y,sizeof(favorite_room_y));
	stream.write((char *) &previous_importance,sizeof(previous_importance));
	stream.write((char *) &pending_message_importance,sizeof(pending_message_importance));
	stream.write((char *) &center_counter,sizeof(center_counter));
	stream.write((char *) &start_of_current_selection,sizeof(start_of_current_selection));
	stream.write((char *) &started_generating_help,sizeof(started_generating_help));
	stream.write((char *) &saved_tt_marty_talk,sizeof(saved_tt_marty_talk));
	stream.write((char *) &last_greeting_help,sizeof(last_greeting_help));
	write_string(stream,pending_message);
	write_string(stream,alternative_spoken_pending_message);
	write_string(stream,last_thing_said[0]);
	write_string(stream,last_thing_said[1]);
	dump_if_not_null(stream,potential_help_target);
	dump_if_not_null(stream,teleport_in_selection);
	dump_if_not_null(stream,teleport_in_tool_in_hand);
	dump_if_not_null(stream,previous_target);
	dump_if_not_null(stream,subject);
	dump_if_not_null(stream,balloon);
	return(TRUE);
};

Martian *load_martian(InputStream *stream, int version_number, NaturalLanguage language) {
	if (tt_martian == NULL) { // new on 141299
		tt_martian = new Martian();
	};
//	tt_martian->load_geometry(pad_in); // commented out on 110100
	if (version_number >= first_version_to_save_cubby_of_robot) {
		tt_martian->load(stream,version_number,language);
      tt_martian->remove_balloon(); // new on 130502 since isn't loaded right now - wrong geometry, priority, and isn't a follower
      // should revisit this if saved while Marty is talking
	} else { // aren't some old versions expecting to load geometry (the old version)??
		tt_martian->started_talking();
	};
	tt_martian->set_active(TRUE); // need?? - right??
	return(tt_martian);
};

void Martian::load(InputStream *stream, int version_number, NaturalLanguage language) {
	unsigned short flags;
	stream->read((string) &flags,sizeof(flags));
	on_floor = !!(flags&1);
	keep_ignoring_button = !!(flags&(1<<1));
	dont_interrupt = !!(flags&(1<<2));
	greeting_needed = !!(flags&(1<<3));
	next_selection_is_target = !!(flags&(1<<4));
	first_time = !!(flags&(1<<5));
	timer_trigger_help = !!(flags&(1<<6));
	saying_greeting = !!(flags&(1<<7));
	teleport_out_when_nothing_to_say = !!(flags&(1<<8));
	pending_message_dont_interrupt = !!(flags&(1<<9));
	called_by_user_flag = !!(flags&(1<<10));
	teleporting_out = !!(flags&(1<<11));
	give_help_without_waiting = !!(flags&(1<<12));
	ok_to_repeat = !!(flags&(1<<13));
	stream->read((string) &time_of_last_help,sizeof(time_of_last_help));
	stream->read((string) &time_between_greetings,sizeof(time_between_greetings));
	stream->read((string) &ready_to_move_off_screen_time,sizeof(ready_to_move_off_screen_time));
	stream->read((string) &favorite_floor_x,sizeof(favorite_floor_x));
	stream->read((string) &favorite_floor_y,sizeof(favorite_floor_y));
	stream->read((string) &favorite_room_x,sizeof(favorite_room_x));
	stream->read((string) &favorite_room_y,sizeof(favorite_room_y));
	stream->read((string) &previous_importance,sizeof(previous_importance));
	stream->read((string) &pending_message_importance,sizeof(pending_message_importance));
	stream->read((string) &center_counter,sizeof(center_counter));
	stream->read((string) &start_of_current_selection,sizeof(start_of_current_selection));
	stream->read((string) &started_generating_help,sizeof(started_generating_help));
	stream->read((string) &saved_tt_marty_talk,sizeof(saved_tt_marty_talk));
	stream->read((string) &last_greeting_help,sizeof(last_greeting_help));
	pending_message= stream->read_string_old();
	alternative_spoken_pending_message = stream->read_string_old();
	last_thing_said[0] = stream->read_string_old();
	last_thing_said[1] = stream->read_string_old();
	set_potential_help_target(load_item(stream,version_number,language));
	set_teleport_in_selection(load_item(stream,version_number,language));
	set_teleport_in_tool_in_hand(load_item(stream,version_number,language));
	set_previous_target(load_item(stream,version_number,language));
	set_subject(load_item(stream,version_number,language));
	balloon = (Talk_Balloon *) load_item(stream,version_number,language);
};

Sprite *Martian::copy(boolean also_copy_offsets) {
	Martian *copy = new Martian();
	copy->set_size_and_location(width,height,llx,lly,COPY_INITIALIZATION);
	// more later
   finish_making_copy(copy,also_copy_offsets); // new on 160702
	return(copy);
};

void Martian::set_potential_help_target(Sprite *sprite) {
	if (potential_help_target != NULL) potential_help_target->destroy(); // release reference
	potential_help_target = sprite;
	if (potential_help_target != NULL) potential_help_target->increment_ref_count();
};

void Martian::set_teleport_in_selection(Sprite *sprite) {
	if (teleport_in_selection != NULL) teleport_in_selection->destroy(); // release reference
	teleport_in_selection = sprite;
	if (teleport_in_selection != NULL) teleport_in_selection->increment_ref_count();
};

void Martian::set_teleport_in_tool_in_hand(Sprite *sprite) {
	if (teleport_in_tool_in_hand != NULL) teleport_in_tool_in_hand->destroy(); // release reference
	teleport_in_tool_in_hand = sprite;
	if (teleport_in_tool_in_hand != NULL) teleport_in_tool_in_hand->increment_ref_count();
};

void Martian::set_previous_target(Sprite *sprite) {
	if (previous_target != NULL) previous_target->destroy(); // release reference
	previous_target = sprite;
	if (previous_target != NULL) previous_target->increment_ref_count();
};

void Martian::set_subject(Sprite *sprite) {
	if (subject != NULL) subject->destroy(); // release reference
	subject = sprite;
	if (subject != NULL) subject->increment_ref_count();
};

const int max_paragraph_count = 64;

Talk_Balloon::Talk_Balloon(city_coordinate initial_x, city_coordinate initial_y) :
	Sprite(TALK_BALLOON_SPRITE,initial_x,initial_y,TALK_BALLOON),
	text(NULL),
	alternative_spoken_text(NULL),
	length(0),
	speech_text_length(0),
	text_offset_increment(0),
	text_offset(0),
	waiting_to_show_more(FALSE),
	full_size_percent(0),
//	max_characters_per_line(28),
//	max_lines(4),
	text_removal_time(0),
   displayed_dots(FALSE),
   recent_sentence_count(6), // consider making this a constant (except outstanding things may not load well)
   oldest_recent_sentence_index(0),
   speech_text_offset(0),
   speech_text_offset_increment(0),
   say_id(0),  
	duration_of_last_sentence_spoken(8000), // initialize with plausible values
   characters_in_last_sentence_spoken(80),
	start_of_current_sentence_spoken(0),
	speech_should_wait_for_first_display(TRUE),
   martian(NULL) { // wasn't initialized prior to 181102
//   remove_text_when_speech_done(FALSE) {
//   current_word_offset(0),
//   spoken_word_length(0) {
  if (!limited_text_to_speech()) {
		duration_of_last_sentence_spoken /= 2; // new on 200400 since general engines tend to speak faster - only an issue for first sentence since after that have measurements
  };
  new_character_size(tt_character_size_percent,TRUE);
  old_llx = llx;
  old_lly = lly;
  paragraph_count = 0;
  paragraph_end = new int[max_paragraph_count]; // can queue up 64 things to say
  recent_sentence = new string[recent_sentence_count];
  for (int i = 0; i < recent_sentence_count; i++) recent_sentence[i] = NULL;
  set_never_cache(TRUE); // always moving around
//  spoken_word = new character[256]; // no word can be this long
  set_containable_by_background(FALSE); // handled specially
  set_move_between_floor_and_room(FALSE); // handled specially
  set_ok_to_dump(FALSE); // maybe it should be OK to dump - but make it load as tt_martian...
  set_selectable(FALSE); // new on 300704 since with F8 can make Marty cover a huge area
};

void Talk_Balloon::new_character_size(long percent, boolean update_characters_fitting_in_full_size) {
	if (percent == 0) {
		characters_fitting_in_full_size = 0;
	} else {
//  if (tt_running_old_demos) {
//     character_width = (percent*(2*tile_width)/3)/100; // 30 characters accross screen
//     character_height = (percent*(19*tile_height)/10)/100; // 11 lines
//     characters_fitting_in_full_size
		  // 6*16 is empirical -- -3 to estimate loss due to avoiding breaking words
//		   = (10000L*6*(16-3))/(percent*percent);
//  } else { // default is now 80% of what it used to be
     character_width = (percent*default_talk_balloon_character_width())/100; 
     character_height = (percent*default_talk_balloon_character_height())/100; // 11 lines -- was 17/10 instead of 2
     if (update_characters_fitting_in_full_size) { // conditional new on 150202
		  characters_fitting_in_full_size = (10000L*5*(18-3))/(percent*percent);
	  };
	  // 5*16 is empirical -- -3 to estimate loss due to avoiding breaking words
	  if (characters_fitting_in_full_size == 0) characters_fitting_in_full_size = 1;
	};
};

void Talk_Balloon::prepare_for_deletion() {
   if (preparing_for_deletion) return; // already done
	preparing_for_deletion = TRUE; // new on 060204
	if (text != NULL) {
		delete [] text;
		text = NULL;
	};
   if (paragraph_end != NULL) {
      delete [] paragraph_end;
      paragraph_end = NULL;
   };
   if (recent_sentence != NULL) {
      for (int i = 0; i < recent_sentence_count; i++) {
         if (recent_sentence[i] != NULL) delete [] recent_sentence[i];
      };
      delete [] recent_sentence;
      recent_sentence = NULL;
   };
	if (alternative_spoken_text != NULL) { // new on 240101
		delete [] alternative_spoken_text;
	};
//   if (spoken_word != NULL) {
//      delete [] spoken_word;
//      spoken_word = NULL;
//   };
	Sprite::prepare_for_deletion();
};

boolean Talk_Balloon::say(Martian *speaking_martian, // new on 090502
                          string new_text, int new_length,
                          boolean need_to_copy, boolean add_to_old,
								  string alternative_spoken_new_text,
								  boolean need_to_copy_alternative_spoken, boolean add_to_old_alternative_spoken) { 
   martian = speaking_martian; // new on 090502
   if (paragraph_count >= max_paragraph_count) { // too much queued up
//      if (!need_to_copy) delete [] new_text;
//      return(FALSE);
		paragraph_count = 0; // new way to recover as of 150301 since otherwise Marty shuts up - better to lose old stuff
		add_to_old = FALSE; 
   };
   if ((new_text == NULL || new_length == 0) && add_to_old) return(FALSE);
	if (new_length < 0) new_length = _tcslen(new_text);
	if (text != NULL) {
      if (new_length > 2 &&
          new_text[new_length-1] == ' ' && new_text[new_length-2] == ' ') {
         new_length--; // only 1 space between sentences
      }; 
		// just said this so skip it
      if (_tcsncmp(text,new_text,paragraph_end[0]) == 0) {
			if (!need_to_copy) delete [] new_text;
			if (!need_to_copy_alternative_spoken && alternative_spoken_new_text != NULL) delete [] alternative_spoken_new_text;
	      return(FALSE);
      };
      for (int i = 1; i < paragraph_count; i++) {
         if (_tcsncmp(text+paragraph_end[i-1],new_text,paragraph_end[i]-paragraph_end[i-1]) == 0) {
				if (!need_to_copy) delete [] new_text;
				if (!need_to_copy_alternative_spoken && alternative_spoken_new_text != NULL) delete [] alternative_spoken_new_text;
		      return(FALSE);
         };
      };
	};
	speech_should_wait_for_first_display = TRUE; // new on 110700 - otherwise Marty's balloon and text can get out of synch - badly
// should be for debuggin but convenient for helping translators...  
//#if TT_DEBUG_ON
	if (tt_debug_mode == 44 || tt_debug_mode == 44444 || tt_debug_mode == 55555) {
		tt_error_file() << "Displayed: ";
		tt_error_file().write(new_text,new_length);
		if (alternative_spoken_new_text != NULL) {
			tt_error_file() << endl << "Spoken: ";
			tt_error_file().write(alternative_spoken_new_text,_tcslen(alternative_spoken_new_text));
		};
		tt_error_file() << endl;
	};
//#endif
   if (text == NULL || 
		 (text_offset >= length && !tt_closed_caption && !tt_closed_caption_alternative_language) || 
		 speech_text_offset >= speech_text_length) { // was just length prior to 081100
      add_to_old = FALSE; // nothing old to add to
      set_clean_status(DIRTY); // seems to help if programmer is idle
   };
	if (alternative_spoken_text == NULL || speech_text_offset >= speech_text_length) {
		add_to_old_alternative_spoken = FALSE;
	};
   if ((tt_marty_talk != MARTY_SPEAKS && tt_marty_talk != MARTY_SPEAKS_WITH_SUBTITLES && tt_marty_talk != MARTY_WITH_SUBTITLES) || 
		  tt_programmer->kind_of() == PROGRAMMER_TITLES) {
		// disjunct added on 080301 so credits appear regardless of mode
		martian->show_balloon(); // was tt_martian prior to 090502
	};
#if TT_TEXT_TO_SPEECH && TT_SOUND_OFF_ONLY_WHEN_SPEAKING
   if (text_to_speech() && !still_talking() && tt_programmer->kind_of() != PROGRAMMER_TITLES) {
      turn_on_speech_and_sound_off(); // obsolete??
   };
#endif
   if (add_to_old) {
      string both_text = new character[length+new_length+3]; // extra in case I need to add ' '
      memcpy(both_text,text,length*sizeof(character));
      if (text[length-1] == '.' || text[length-1] == '?' || text[length-1] == '!') {
         both_text[length++] = ' '; // add a space between sentences
      };
      _tcscpy(both_text+length,new_text);
      if (!need_to_copy) delete [] new_text;
      length += new_length;
      paragraph_end[paragraph_count++] = length;
      new_text = both_text;
#if TT_DEBUG_ON
		if (tt_debug_mode == 290999) {
			log("Adding text");
			tt_error_file() << both_text << endl;
		};
#endif
		need_to_copy = FALSE; // new on 290999 since just allocated here
   } else {
		// instantly finish off old
	   finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
//      remove_text(); // just in case
      if (new_length > 0) {
         paragraph_count = 1; // just this new one
         paragraph_end[0] = new_length;
      };
    	length = new_length;
   };
	if (alternative_spoken_new_text != NULL) {
		if (add_to_old_alternative_spoken) {
			int alternative_spoken_new_length = _tcslen(alternative_spoken_new_text);
			string both_text = new character[speech_text_length+alternative_spoken_new_length+3]; // extra in case I need to add ' '	
			memcpy(both_text,alternative_spoken_text,speech_text_length*sizeof(character));
			if (alternative_spoken_text[speech_text_length-1] == '.' || alternative_spoken_text[speech_text_length-1] == '?' ||
				 alternative_spoken_text[speech_text_length-1] == '!') {
				both_text[alternative_spoken_new_length++] = ' '; // add a space between sentences
			};
			_tcscpy(both_text+speech_text_length,alternative_spoken_new_text);
			both_text[alternative_spoken_new_length+speech_text_length] = 0; // terminate string
			if (!need_to_copy_alternative_spoken) delete [] alternative_spoken_new_text;
			speech_text_length += alternative_spoken_new_length;
			alternative_spoken_new_text = both_text;
//			alternative_spoken_length = speech_text_length; // obsolete variable?
#if TT_DEBUG_ON
			if (tt_debug_mode == 290999) {
				log("Adding alternative spoken text");
				tt_error_file() << both_text << endl;
			};
#endif
			need_to_copy_alternative_spoken = FALSE; // new on 290999 since just consed
		} else {
    		speech_text_length = _tcslen(alternative_spoken_new_text);
		};
	} else {
//		alternative_spoken_length = 0;
		speech_text_length = length;
	};
	if (text != NULL) {
	   delete [] text; // delete private copy
	};
	if (alternative_spoken_text != NULL) {
	   delete [] alternative_spoken_text; // delete private copy
	   alternative_spoken_text = NULL;
	};
	if (new_length == 0) {
		text = NULL;
		return(FALSE);
	};
	if (need_to_copy) {
		text = copy_string(new_text); // ,new_length wasn't always correct
	} else {
		text = new_text;
	};
	if (alternative_spoken_new_text != NULL) {
		if (need_to_copy_alternative_spoken) {
			alternative_spoken_text = copy_string(alternative_spoken_new_text);
		} else {
			alternative_spoken_text = alternative_spoken_new_text;
		};
	};
   if (martian->return_ok_to_repeat()) { // was tt_martian prior to 090502
      martian->set_last_thing_said(FALSE,copy_string(text));
		martian->set_last_thing_said(TRUE,copy_string(alternative_spoken_text));
   };
   if (!add_to_old && (alternative_spoken_text == NULL || !add_to_old_alternative_spoken)) {
	   text_offset_increment = 0;
	   text_offset = 0;
      speech_text_offset = 0;
	   waiting_to_show_more = FALSE;
	   compute_size(length,martian);
#if TT_DEBUG_ON
		if (tt_debug_mode == 5050) {
			 tt_error_file() << "At " << timeGetTime() << " Marty startng to say: ";
			 tt_error_file().write(text,length);
			 tt_error_file() << endl;
		};
#endif
   };
#if TT_TEXT_TO_SPEECH
   if ((alternative_spoken_text == NULL && 
		  (!add_to_old || speech_text_offset >= length)) ||
		 (alternative_spoken_text != NULL && 
		  (!add_to_old_alternative_spoken || speech_text_offset >= length))) {
      // either new or have already spoken rest
      speak_next_sentence();
#if TT_DEBUG_ON
	} else if (tt_debug_mode == 44444) {
		tt_error_file() << "Now speaking at " << tt_current_time << " frame = " << tt_frame_number << endl;
#endif
   };
#endif
   martian->started_talking();  // was tt_martian prior to 090502
	return(TRUE);
};

void Talk_Balloon::compute_size(int text_length, Martian *martian) {
	// doesn't quite work right if too close to the side since not all of
	// balloon is available for text
#if TT_TEXT_TO_SPEECH   
   if ((tt_marty_talk == MARTY_SPEAKS || tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES || tt_marty_talk == MARTY_WITH_SUBTITLES) && 
		 tt_programmer->kind_of() != PROGRAMMER_TITLES) return;
#endif      
	int new_percent;
	if (characters_fitting_in_full_size == 0) {
		new_percent = 0;
	} else {
		new_percent = (text_length*100L)/characters_fitting_in_full_size;
	};
//	if (tt_screen_width <= 800) { // condition new on 150202 since for high resolution can have bigger balloons -- restored on 070502 since sometimes text is too small
		if (new_percent < 10) {
			new_percent = 40; //was 31; // square root of 1/10 is about 1/3
		} else if (new_percent < 20) {
			new_percent = 53; // was 45; 
		} else if (new_percent < 33) {
			new_percent	= 64; //was 58;
		} else if (new_percent < 50) {
			new_percent	= 71; // was 67;
		} else if (new_percent < 70) {
			new_percent	= 81; // was 79;		
		} else {
			new_percent = 90; // was 100;
		};
//	};
//#if TT_32
	// not sure why this is necessary but otherwise balloon too small
//	if (tt_video_mode == 25) new_percent = (new_percent*150)/100;
//#endif
//	new_percent = (new_percent*150)/100; // "natural" size of thought bubble smaller...
	if (//tt_screen_width <= 800 && 
       full_size_percent != new_percent) { // turned this off for high screen resolutions since it looked bad (150202)
		//full_size_percent = new_percent;
		//int marty_percent = martian->get_size_percent();
		//int combined_percent = (full_size_percent*marty_percent)/100;
		//set_size_percent(combined_percent);
      // commented out the above on 181002 since this produced small talk balloons with small text when running with high resolution
      set_size_percent(new_percent); 
		martian->update_balloon();
	}; 
};

//short int recent_sentence_count;
//short int oldest_recent_sentence_index;
//string *recent_sentence;

boolean Talk_Balloon::just_said(string text, int length) {
   int j = 0;
   for (; j < length; j++) {
      if (text[j] == ' ') break;
   };
   if (j == length) return(FALSE); // ingore one word sentences      
   for (int i = 0; i < recent_sentence_count; i++) {
      if (recent_sentence[i] != NULL &&
          _tcsncmp(text,recent_sentence[i],length) == 0) return(TRUE);
   };
   oldest_recent_sentence_index = (short int) ((oldest_recent_sentence_index+1)%recent_sentence_count);
   if (recent_sentence[oldest_recent_sentence_index] != NULL) {
      delete [] recent_sentence[oldest_recent_sentence_index];
   };
   recent_sentence[oldest_recent_sentence_index] = new character[length+1];
   memcpy(recent_sentence[oldest_recent_sentence_index],text,length);
   recent_sentence[oldest_recent_sentence_index][length] = NULL; // terminate string
   return(FALSE);
};

int good_place_to_break1(wide_string to_say, int max_length) {
	// just copied narrow string version on 101000 and updated it appropriately
	int line_length = 0;
    // speak the spaces between sentences to get better text/speech coordination
    boolean searching_for_close_paren = FALSE;
	 boolean found_punctuation = FALSE;
	 int to_say_length = wide_string_length(to_say); // new on 240101
    while (line_length < max_length && line_length < to_say_length) { // search for end of sentence or comma
       if (to_say[line_length] == ')') {
           searching_for_close_paren = FALSE;
			  max_length++; // going to ignore parenthetical remarks so increase max
       } else if (to_say[line_length] == '(') {
           searching_for_close_paren = TRUE;
			  max_length++; // going to ignore parenthetical remarks so increase max
		 } else if (searching_for_close_paren) {
			 max_length++; // going to ignore parenthetical remarks so increase max
       } else if (phrase_ending_punctuation(to_say,line_length,max_length,TRUE) &&
                  (to_say[line_length] > 255 || // added since any Unicode punctuation is OK (while period might be inside a number for example)
						 line_length+1 == max_length || to_say[line_length+1] == ' ')) {
			 // prior to 200999 max_length was length (of the entire text - not just the remaining portion)
          line_length++;  // extended to full phrase or sentence
          while (line_length < max_length && to_say[line_length] == ' ') {
              line_length++; // include spaces
          };
			 // narrow version does /2 instead
			 if (line_length > max_subtitle_length()/4 || line_length == max_length) { // conditions new on 200999 to avoid very short subtitles
				 found_punctuation = TRUE;
				 break;
			 };
      };
      line_length++;
    };
	 if (!found_punctuation) { // new on 171099 so subtitles aren't broken mid-word search for spaces
		 int new_line_length = line_length;
		 while (new_line_length > 0 && new_line_length < to_say_length) { // second condition added 240101) {
			 if (to_say[new_line_length] == ' ') {
				 line_length = new_line_length;
				 break;
			 };
			 new_line_length--;
		 };
	 };
	 return(multi_byte_length(to_say,line_length));
};

int good_place_to_break1(string to_say, int max_length) {
	int line_length = 0;
//       while (line_length+speech_text_offset < length && to_say[line_length] == ' ') {
//          line_length++; // count spaces
//       };
    // speak the spaces between sentences to get better text/speech coordination
    boolean searching_for_close_paren = FALSE;
//		 int first_break = 0;
//		 int second_break = 0;
	 boolean found_punctuation = FALSE;
	 int to_say_length = strlen(to_say); // new on 240101
    while (line_length < max_length && line_length < to_say_length) { // search for end of sentence or comma - second condition added 240101
       if (to_say[line_length] == ')') {
           searching_for_close_paren = FALSE;
			  max_length++; // going to ignore parenthetical remarks so increase max
       } else if (to_say[line_length] == '(') {
           searching_for_close_paren = TRUE;
			  max_length++; // going to ignore parenthetical remarks so increase max
		 } else if (searching_for_close_paren) {
			 max_length++; // going to ignore parenthetical remarks so increase max
       } else if ((to_say[line_length] == '.' || to_say[line_length] == '?' ||
                   to_say[line_length] == ',' || to_say[line_length] == '!') &&
                  (line_length+1 == max_length || to_say[line_length+1] == ' ')) {
			 // prior to 200999 max_length was length (of the entire text - not just the remaining portion)
          line_length++;  // extended to full phrase or sentence
          while (line_length < max_length && to_say[line_length] == ' ') {
              line_length++; // include spaces
          };
//				 if (first_break == 0) {
//					 first_break = line_length;
//				 } else if (second_break == 0) {
//					 second_break = line_length;
//				 } else 
			 if (line_length > max_subtitle_length()/2 || line_length == max_length) { // conditions new on 200999 to avoid very short subtitles
				 found_punctuation = TRUE;
				 break;
			 };
      };
      line_length++;
    };
	 if (!found_punctuation) { // new on 171099 so subtitles aren't broken mid-word search for spaces
		 int new_line_length = line_length;
		 while (new_line_length > 0 && new_line_length < to_say_length) { // second condition added 240101
			 if (to_say[new_line_length] == ' ') {
				 line_length = new_line_length;
				 break;
			 };
			 new_line_length--;
		 };
	 };
	 return(line_length);
};

const int max_breaks = 20; // was 10 prior to 011099

int good_place_to_break(string to_say, int max_length, int sentence_count=0) {
	// sentence_count added on 100102
	if (to_say == NULL || max_length == 0) {
		// new on 101204 since crashed here (issue 576)
		return(0);
	};
	if (need_wide_characters()) {
		wide_string wide_to_say = copy_wide_string(to_say);
		int wide_length = wide_string_length(wide_to_say);
		if (!contains_non_ascii(wide_to_say,wide_length)) {
			delete [] wide_to_say;
			return(good_place_to_break1(to_say,max_length)); // should pass sentence_count along here some day
		};
		if (!alternative_language_spoken() && (tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES || tt_marty_talk == MARTY_WITH_SUBTITLES)) { // new on 101000 since was breaking subtitles poorly when running in Japanese
			int answer = good_place_to_break1(wide_to_say,wide_length);
			delete [] wide_to_say;
			return(answer);
		};
		int result;
		int sentence_break = end_of_nth_sentence(sentence_count,wide_to_say,wide_length);
		if (sentence_break > 0) {
			result = multi_byte_length(wide_to_say,sentence_break);
		} else {
			int true_max_length = min(wide_length,max_length*2); // this had been commented out and replaced with wide_length prior to 011199 but it leads to very big subtitles
			int breaks[max_breaks];
			breaks[0] = last_space_or_punctuation(wide_to_say,true_max_length,need_wide_characters());
			int break_count = 0;
			while (breaks[break_count] > 0 && break_count < max_breaks-1) { // -1 added 011099
				break_count++;
				// on 031199 added condition that ascii is OK as break only if last break wasn't due to ascii
				breaks[break_count] = last_space_or_punctuation(wide_to_say,breaks[break_count-1]-1,
																				(wide_to_say[breaks[break_count-1]] > 255));
			};
			int estimated_spoken_length = min(wide_length,true_max_length/2); // was just max_length1/2 prior to 031199 -- 1/2 as of 110401 -- made true_max_length on 100102

			if (breaks[0] < 0 || breaks[0] < estimated_spoken_length/2) { 
				// couldn't find a place to break it or would need to cut off too much
				int last = true_max_length/2;
				int min_last = last/2;
				while (last > min_last && wide_to_say[last] < 256) { // don't break ascii - new on 031199
					last--;
				};
				result = multi_byte_length(wide_to_say,last);
			} else {
				int best_break = 0;
				int best_distance = abs(breaks[best_break]-estimated_spoken_length);
				for (int i = 1; i < break_count; i++) {
					int distance = abs(breaks[i]-estimated_spoken_length);
					if (phrase_ending_punctuation(wide_to_say,breaks[i],wide_length)) { // added 221199 since prefer these breaks
						distance -= estimated_spoken_length/8; // willing to give up 1/8 to end with period or the like
					};
					if (distance < best_distance) {
						best_break = i;
						best_distance = distance;
					};
				};
				result = multi_byte_length(wide_to_say,breaks[best_break]+1);
			};
		};
		delete [] wide_to_say;
		return(result);
	} else {
		return(good_place_to_break1(to_say,max_length));
	};
};

void Talk_Balloon::display(SelectionFeedback selection_feedback,
									boolean followers_too, TTRegion *region) {
 	// didn't bother to support region
	if (still_talking() && text_offset < length) { // second condition added 120200 since standing up and sitting down while Marty was talking produced empty balloons
      if (!visible()) {
         if (!tt_martian->show_balloon()) return; // conditional part new on 070200 so if invisible and don't want talk balloon nothing happens
      };
      boolean ok_to_display = TRUE;
      boolean in_titles = (tt_programmer->kind_of() == PROGRAMMER_TITLES);
#if TT_TEXT_TO_SPEECH
      if ((tt_marty_talk == MARTY_SPEAKS || tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES || tt_marty_talk == MARTY_WITH_SUBTITLES) && !in_titles) {
         if (waiting_to_show_more) return;   //  || speech_in_progress()
         ok_to_display = FALSE;
      };
#endif
 		int characters_shown;
      boolean ok_to_display_dots = !in_titles;
      if (in_titles) ok_to_display = TRUE; // titles should use text
      if (ok_to_display) {
			if (tt_city->stopped()) {
				// no point showing it if it gets obscured, except by your hand -- added 010399
				set_priority(programmer_hand_priority+1); 
			};
   		if (labs(old_llx-llx) < tile_width/2 &&
   			 labs(old_lly-lly) < tile_height/2) {
   			// moved a tiny bit so don't move
   			llx = old_llx;
   			lly = old_lly;
   		};
   		old_llx = llx;
   		old_lly = lly;
         Sprite::display(selection_feedback,followers_too,region);
   		city_coordinate text_ulx = llx+width/4-character_width/2; // -character_width/2 looks nicer
   		city_coordinate text_uly = lly+(7*height)/8;	
			if (want_feedback(selection_feedback)) {
				text_ulx += selection_delta_x();
				text_uly += selection_delta_y();
			};
   		city_coordinate text_width = (6*width)/10;
   		city_coordinate text_height = (7*height)/10; // was 3/4
			WhenToBreakSentences break_sentences = NEVER_BREAK_SENTENCES; // rewritten on 231103 to look nicer since broken sentences look very bad
//       WhenToBreakSentences break_sentences;
    //   if (text_to_speech() && !alternative_language_spoken()) { 
				// commented out on 041199 to look better - restored on 200400 since sometimes looked much worse
    //      break_sentences = NEVER_BREAK_SENTENCES; // to keep them in synch
    //   } else {
    //      break_sentences = BREAK_SENTENCES_TO_NOT_WASTE_HALF_BALLOON;  // try not to break sentences
    //   };
			int max_characters = length-text_offset;
			if ((tt_closed_caption || tt_closed_caption_alternative_language || alternative_language_spoken()) && tt_titles_ended_on_frame <= tt_frame_number) {
				// alternative_language_spoken() is also included to keep better synch by having small bubbles
				max_characters = max_subtitle_length();
//				if (text[text_offset] < 256) { // ascii so there is more room -- but text isn't Unicode yet
					max_characters += max_characters/2;
//					// add 50% since will work backwards looking for punctuation so full phrases aren't broken
//				};
				if (alternative_language_spoken() && !tt_closed_caption_alternative_language) { 
					// better synch is good but breaking sentences is worse
					max_characters += max_characters/2;
				};
//			} else if (speech_text_offset_increment > 0) { // new on 200400
//				max_characters = speech_text_offset_increment;
			} else if (max_characters > max_characters_in_talk_balloon) {
				max_characters = max_characters_in_talk_balloon;
			};
   		characters_shown = tt_screen->place_text(text+text_offset,length-text_offset,
   															  text_ulx,text_uly,
   															  text_width,text_height,
   															  character_width,character_height,
   															  break_sentences,
   															  TRUE, // vertically center if too little
																  ok_to_display_dots,
   															  width/3, // center can be wider
   															  character_width*2, // if clipping leave character on each side
																  BLACK,TRUE,
																  max_characters); // new on 200999
//      } else { // just a sentence at a time if no talk balloon
//         characters_shown = length-text_offset; // default
//         for (int i = 0; i < length-text_offset; i++) {
//            if ((text[text_offset+i] == '.' ||
//                 text[text_offset+i] == '!' ||
//                 text[text_offset+i] == '?' ||
//                 text[text_offset+i] == ',') &&
//                (i+1 == length-text_offset ||
//                 text[text_offset+i+1] == ' ')) {
//               characters_shown = i+1;
//               if (just_said(text+text_offset,characters_shown)) {
                  // then skip to the next sentence
//                  text_offset += characters_shown;
//                  i = 0;
//               } else break;
//            };
//         };
      };
//#if TT_DEBUG_ON
//		if (characters_shown > 0) {
//			if (text_offset+characters_shown < length && 
//				 IsCharAlphaNumericG(text[text_offset+characters_shown-1]) && 
//				 IsCharAlphaNumericG(text[text_offset+characters_shown])) {
//				log("Warning: Marty is splitting a word or number.");
//			};
//			if (tt_debug_mode == 44444) {
//				tt_error_file().write(text+text_offset,characters_shown);
//				tt_error_file() << endl;
//			};
//		};
//#endif
		if (characters_shown == 0) {
			if (ok_to_display || !tt_closed_caption_alternative_language) {
				waiting_to_show_more = FALSE;
				text_offset = length; // nothing left to say
				paragraph_count = 0;
				tt_martian->say_completed();
				displayed_dots = FALSE;
			} else {
				set_visible(FALSE); // still going for subtitles sake
			};
		} else if (ok_to_display) {
			speech_should_wait_for_first_display = FALSE; 
			// new on 220500 - otherwise when balloon shrinks (e.g. sitting down) can speak and display parts of sentences
         displayed_dots = (// ok_to_display_dots &&
                           (length-text_offset > characters_shown));
			text_offset_increment = characters_shown;
//#if TT_DEBUG_ON
//			if (tt_debug_mode == 44444) {
//				tt_error_file() << "text_offset_increment set to " << text_offset_increment << " (characters_shown)" << endl;
//			};
//#endif
			if (tt_closed_caption_alternative_language && !in_titles) {
			   string subtitle = text+text_offset;
			   tt_screen->set_subtitle(subtitle,characters_shown,-1);
#if TT_DEBUG_ON
				if (tt_debug_mode == 55555) {
					tt_error_file() << "Starting subtitle: ";
					tt_error_file().write(subtitle,characters_shown);
					tt_error_file() << endl;
				};
#endif
			};
			for (int i = text_offset+text_offset_increment; i < length; i++) {
				if (text[i] == ' ') {
					text_offset_increment++; // skip initial blanks
				} else {
					break;
				};
			};
//			if (i+1 == length) { // just one character left
				// skip it since usually garbage (not clear why)
//				text_offset_increment++;
//			};
			if (!waiting_to_show_more) {
				millisecond duration;
#if TT_TEXT_TO_SPEECH
            if (text_to_speech() && !in_titles && !alternative_language_spoken() &&
                (speech_text_offset < speech_text_length || speech_text_offset_increment != 0) &&
					 (speech_text_offset_increment <= text_offset_increment)) { 
					// added on 161099 since if too much text in single sentence then speech will encompase multiple bubbles
               // do nothing since finishing sentence will trigger this
#if TT_DEBUG_ON
               if (tt_debug_mode == 55555) {
                  tt_error_file() << "Just displayed " << characters_shown << " characters." << endl << " ";
                  tt_error_file().write(text+text_offset,characters_shown);
                  tt_error_file() << " at " << tt_current_time << endl;
               };
#endif
            } else
#endif
            if (ok_to_display) {
					if (tt_help_speed_percent == 0 || tt_speed == 0) {
						duration = seconds_in_a_day*1000L; // max_long didn't work since is added to current time
					} else {
						if (text_to_speech() && !alternative_language_spoken()) { 
							// second condition added 200400 and was commented out and restored on 080500
							// following is new on 051199
							int characters = characters_shown;
							// take into account 2 languages have different lengths
							int adjusted_speech_text_offset = (speech_text_offset*length)/speech_text_length;
							// if speech is behind wait extra for speech to catch up
							// if speech is ahead don't wait as long
							characters += text_offset-adjusted_speech_text_offset;
							// following added on 081199 to fix bug when characters becomes negative
							characters = max(characters,10); // no matter how short (especially if negative) treat as if it were a few words
							duration = (characters*duration_of_last_sentence_spoken)/characters_in_last_sentence_spoken; // new on 051199
						} else {
							duration = compute_reading_duration(characters_shown);
						};
					};
#if TT_DEBUG_ON
					if (tt_debug_mode == 55555) {
						tt_error_file() << "Will place rest of text after " << duration << " milliseconds." << endl;
					};
#endif
					do_after(duration,this,PLACE_REST_OF_TEXT_CALLBACK);
               waiting_to_show_more = TRUE;
               if (text_offset+text_offset_increment >= length) {
					   text_removal_time = tt_current_time+duration;
				   };
            // new default is twice old one since PageDown works now
				// default is 3 seconds plus 150ms for each character
//	         } else {
               // longest it might be take
//               duration = characters_shown*200L;    // was 80
            };
			};
		};
	} else if (visible()) { // not talking so ...
		set_visible(FALSE);
//		tt_martian->balloon_is_now_invisible();
//		TTRegion region;
//		tt_programmer->pointer_to_appearance()->full_region(region);
//		tt_martian->finish_instantly();
//		tt_martian->keep_out_of_the_way(region); // since programmer might not be moving
	};
};

#if TT_TEXT_TO_SPEECH

boolean portuguese_spoken() {
	return(spoken_language() == PORTUGAL_PORTUGUESE || spoken_language() == BRAZILIAN_PORTUGUESE);
};

void and_between_parts_of_number(string number, output_stream &stream) {
	// number should be n thousand, n million, n billion, and so on (where n is a single digit) - if British English
	int i = 0; // don't say "and" if followed by n hundred - if British English
	while (number[i] != 0) {
		if (number[i] != '0') {
			if (i%3 == 0 && spoken_language() == BRITISH) { // second conjunct added 010500
				stream << ", ";
				return;
			};
			if (portuguese_spoken()) {
				stream << "exxx "; // updated 300400
			} else {
				stream << S(IDS_AND) << " ";
			};
			return;
		};
		i++;
	};
};

void and_between_parts_of_hundreds_number(string number, output_stream &stream) {
	for (int i = 0; i < 2; i++) {
		if (number[i] != '0') {
			if (portuguese_spoken()) {
				stream << "exxx "; // updated 300400
			} else {
				stream << S(IDS_AND) << " ";
			};
			return;
		};
	};
};

void number_replacement(string original_number, output_stream &stream) {
// 100 "etthundra"
// 200 "tvåhundra"
// 1000 "ettusen"
// 2000 "tvåtusen"
// 1000000 "en miljon"
// 2000000 "två miljoner"
// 1000000000 "en miljard"
// 2000000000 "två miljarder"
// 1000000000000 "en biljon"
// 2000000000000 "två biljoner"
   if (original_number[0] == '0') { // let MS TTS handle it
      stream << original_number;
//		if (ordinal == ORDINAL_TH) stream << "th"; // not a great solution...
      return;
   };
   if (original_number[0] == '-') { // n < 0) {
      stream << "minus "; 
      original_number++;
   };
   string number = new character[_tcslen(original_number)+1];
   int length = 0;
	int i = 0;
	while (original_number[i] != '\0') { 
		if (original_number[i] >= '0' && original_number[i] <= '9') {
			number[length] = original_number[i];
			length++;
		}; // otherwise skip over commas, spaces and the like
		i++;
	};
	number[length] = '\0';
   string new_number = new character[length];
   string original_new_number = new_number;
   if (length > 12) {
      if (length == 13 && number[0] == '1') {
			stream << S(IDS_TRILLION_SINGULAR) << " "; // updated on 050499 to be easier to localize
         _tcscpy(new_number,number+length-12); // last 12 digits
      } else {
         _tcscpy(new_number,number+length-12);
         number[length-12] = '\0'; // truncate last 12 digits
         number_replacement(number,stream);
			stream << S(IDS_TRILLION_PLURAL) << " ";
      };
		if (spoken_language() == BRITISH || portuguese_spoken()) { // added  || portuguese_spoken() on 010500
			and_between_parts_of_number(new_number,stream);
		};
      while (new_number[0] == '0') {
         new_number++; // remove preceeding zeros
      };
      if (new_number[0] != '\0') {
         number_replacement(new_number,stream);
      };
   } else if (length > 9) {
      if (length == 10 && number[0] == '1') {
			stream << S(IDS_BILLION_SINGULAR) << " ";
         _tcscpy(new_number,number+length-9); // last 9 digits
      } else {
         _tcscpy(new_number,number+length-9); // last 9 digits
         number[length-9] = '\0'; // truncate last 9 digits
         number_replacement(number,stream);
			stream << S(IDS_BILLION_PLURAL) << " ";
      };
		if (spoken_language() == BRITISH || portuguese_spoken()) { // added  || portuguese_spoken() on 010500
			and_between_parts_of_number(new_number,stream);
		};
      while (new_number[0] == '0') {
         new_number++; // remove preceeding zeros
      };
      if (new_number[0] != '\0') {
         number_replacement(new_number,stream);
      };
   } else if (length > 6) {
      if (length == 7 && number[0] == '1') {
		   stream << S(IDS_MILLION_SINGULAR) << " ";
         _tcscpy(new_number,number+length-6);
      } else {
         _tcscpy(new_number,number+length-6);
         number[length-6] = '\0';
         number_replacement(number,stream);
			stream << S(IDS_MILLION_PLURAL) << " ";
      };
		if (spoken_language() == BRITISH || portuguese_spoken()) { // added  || portuguese_spoken() on 010500
			and_between_parts_of_number(new_number,stream);
		};
      while (new_number[0] == '0') {
         new_number++; // remove preceeding zeros
      };
      if (new_number[0] != '\0') {
         number_replacement(new_number,stream);
      };
   } else if (length > 3) {
      _tcscpy(new_number,number+length-3);
      number[length-3] = '\0';
		if (number[0] != '1' || number[1] != '\0' || spoken_language() != PORTUGAL_PORTUGUESE) { 
			// conditional new on 260400 so it is "mil" not "1 mil"
			number_replacement(number,stream);
		};
	   stream << S(IDS_THOUSAND_PLURAL) << " "; // singular and plural treated same for now
		if (spoken_language() == BRITISH) {
			and_between_parts_of_number(new_number,stream);
		} else if (portuguese_spoken()) { // new on 260400
			if (strcmp(new_number,"100") == 0 || 
				 (new_number[0] != '1' && strcmp(new_number,"000") != 0)) { // add "and" if is exactly 100 or not in the 100s
				// check for 000 added on 300400 so 1000 is OK
				stream << "exxx "; // S(IDS_AND) << " "; - updated 300400
			};
		};
//		boolean removed_zeros = FALSE;
      while (new_number[0] == '0') {
         new_number++; // remove preceeding zeros
//			removed_zeros = TRUE;
      };
      if (new_number[0] != '\0') {
         number_replacement(new_number,stream);
      };
   } else if (length == 3) {
      _tcscpy(new_number,number+length-2);
      number[length-2] = '\0';
		if (portuguese_spoken()) { // new on 260400
			// replaced spoken_language() == PORTUGAL_PORTUGUESE with portuguese_spoken() on 100500 since they both have the same rules
			if (number[0] == '1' && new_number[0] == '0' && new_number[1] == '0') {
				stream << S(IDS_ONE_HUNDRED_ALONE) << " ";
			} else {
				stream << S(IDS_ONE_HUNDRED+(number[0]-'1')) << " ";
			};
		} else {
			number_replacement(number,stream);
			stream << S(IDS_HUNDRED_PLURAL) << " "; // singular and plural treated same for now
		};
		if (spoken_language() == BRITISH || portuguese_spoken()) { // Portuguese added 260400
			and_between_parts_of_hundreds_number(new_number,stream);
		};
      while (new_number[0] == '0') {
         new_number++; // remove preceeding zeros
      };
      if (new_number[0] != NULL) {
         number_replacement(new_number,stream);
      };
	} else if (length == 2 && portuguese_spoken()) { // added 260400 so you get "fifty and four"
		if (number[0] != '0' && number[0] != '1' && number[1] != '0') {
//			stream << number[0] << '0' << " " << S(IDS_AND) << " " << number[1] << " ";
			stream << number[0] << '0' << " exxx " << number[1] << " "; // updated 300400
		} else {
			stream << number << " ";
		};
   } else {
      stream << number << " ";
   };
   delete [] original_new_number;
	delete [] number;
};

#if TT_SPEAK_ALL_NUMBERS

// question: Should this deal specially with non base 10 numbers by reading them after converting to base 10?

int copy_all_but_leading_zeros(wide_string input, wide_string &output, int digit_count) {
	int output_count = 0;
	for (int i = 0; i < digit_count; i++) {
		if (input[0] != L'0' || output_count > 0) {
			output[0] = input[0];
			output++;
			output_count++;
		};
		input++;
	};
	return(output_count);
};

wide_character in_common(wide_string s1, wide_string s2) {
	if (s1 == NULL || s2 == NULL) return(NULL); // new on 071104
	int length1 = wide_string_length(s1);
	int length2 = wide_string_length(s2);
	for (int i = 0; i < length1; i++) {
		for (int j = 0; j < length2; j++) {
			if (s1[i] == s2[j] || (s1[i] == L'*' && (s2[j] == L's' || s2[j] == L'x'))) { // special rule of tre represented by * matching s or x
				return(s2[j]); // was s1[i] but that is wrong for *
			};
		};
	};
	return(NULL);
};

void add_to_output(string narrow_text, wide_string &output) {
	if (narrow_text == NULL || narrow_text[0] == '\0') return;
	int length = strlen(narrow_text);
	// following rewritten on 050705 since did the wrong thing with non-ASCII characters -- e.g. biliões
	copy_wide_string(narrow_text,length,output);
	output += length;
	//for (int i = 0; i < length; i++) {
	//	output[0] = narrow_text[i]; // no need to convert to wide since ASCII is a subset, right?
	//	output++;
	//};
	// alternative if needed:
	//wide_string wide_string = copy_wide_string(narrow_text,length);
	//memcpy(output,wide_string,length*sizeof(wide_character));
	//output += length;
};

void add_to_output(wide_string wide_text, wide_string &output) {
	if (wide_text == NULL || wide_text[0] == NULL) return;
	int length = wide_string_length(wide_text);
	// could use memcpy instead here
	for (int i = 0; i < length; i++) {
		output[0] = wide_text[i];
		output++;
	};
};
	// largely based on information in http://home.earthlink.net/~mrob/pub/math/largenum.html
/*
 Rules for one system extending up to 103000 are given in The Book of Numbers by Conway and Guy. This system was developed by John Conway and Allan Wechsler after significant research into Latin5 but Olivier Miakinen4 has refined it, as described below. 
   The name is built out of pieces representing powers of 10^3, 10^30 and 10^300 as shown by this table: 
   
 1's				10's							100's  
0  -				-								-  
1  un				(n) deci						(nx) centi  
2  duo			(ms) viginti				(n) ducenti  
3  tre (*)		(ns) triginta				(ns) trecenti  
4  quattuor		(ns) quadraginta			(ns) quadringenti  
5  quin			(ns) quinquaginta			(ns) quingenti  
6  se (sx)		(n) sexaginta				(n) sescenti  
7  septe (mn)  (n) septuaginta			(n) septingenti  
8  octo			(mx) octoginta  			(mx) octingenti  
9  nove (mn)	nonaginta					nongenti  

   The rules are: 
 
- Take the power of 10 you're naming and subtract 3. 
- Divide by 3. If the remainder is 0, 1 or 2, put one, ten or one hundred at the beginning of your name (respectively). 
- Break the quotient up into 1's, 10's and 100's. Find the appropriate name segments for each piece in the table. (NOTE: The original Conway-Wechsler system specifies quinqua for 5, not quin.) 
- String the segments together, inserting an extra letter if the letter shown in parentheses at the end of one segment match a letter in parentheses at the beginning of the next. For example: septe(mn) + (ms)viginti = septemviginti; se(sx) + (mx)octoginta = sexoctoginta. For the special case of tre, the letter s should be inserted if the following part is marked with either an s or an x. 
- If the result ends in a, change the a to i. 
- Add llion at the end. You're done. 
*/
	// note that the above web site does not have "prefix" "m" for nonaginta or nongenti but many sites seem to require it -- e.g. http://www.webster-dictionary.org/definition/Names%20of%20large%20numbers

void wordify(wide_string input, int digit_count, wide_string &output, boolean top_level=TRUE) {
	// added top_level so recursive calls don't copy lots of zeros
	if (top_level && (input == NULL || input[0] < '1' || input[0] > '9')) {
		memcpy(output,input,digit_count*sizeof(wide_character));
		output += digit_count;
		return; // to avoid bugs due to strings of 0s
	};
	const wide_string one_names[10] = {L"",L"un",L"duo",L"tre",L"quattuor",L"quin",L"se",L"septe",L"octo",L"nove"};
	const wide_string one_suffixes[10] = {L"",L"",L"",L"*",L"",L"",L"sx",L"mn",L"",L"mx"};
	const wide_string ten_names[10] = {L"",L"deci",L"viginti",L"triginta",L"quadraginta",L"quinquaginta",
												  L"sexaginta",L"septuaginta",L"octoginta",L"nonaginta"};
	const wide_string ten_prefixes[10] = {L"",L"n",L"ms",L"ns",L"ns",L"ns",L"n",L"n",L"mx",L"m"};
	const wide_string hundred_names[10] = {L"",L"centi",L"ducenti",L"trecenti",L"quadringenti",L"quingenti",
														L"sescenti",L"septingenti",L"octingenti",L"nongenti"};
	const wide_string hundred_prefixes[10] = {L"",L"nx",L"n",L"ns",L"ns",L"ns",L"n",L"n",L"mx",L"m"};
	// following are my attempt to generalize for higher powers:
	const wide_string thousand_names[10] = {L"",L"milli",L"dumilli",L"tremilli",L"quadrinmilli",L"quinmilli",
														 L"sesmilli",L"septinmilli",L"octinmilli",L"nonmilli"};
	const wide_string ten_thousand_names[10] = {L"",L"decimilli",L"vigintimilli",L"trigintamilli",L"quadragintanmilli",L"quinquagintamilli",
															  L"sexagintamilli",L"septuagintamilli",L"octogintamilli",L"nonagintamilli"};
	const wide_string hundred_thousand_names[10] = {L"",L"centimilli",L"ducentimilli",L"trecentimilli",L"quadringentimilli",L"quingentimilli",
																	L"sescentimilli",L"septingentimilli",L"octingentimilli",L"nongentimilli"};
	const wide_string million_names[10] = {L"",L"milli-milli",L"milli-dumilli",L"milli-tremilli",L"milli-quadrinmilli",L"milli-quinmilli",
														L"milli-sesmilli",L"milli-septinmilli",L"milli-octinmilli",L"milli-nonmilli"};
	while (TRUE) {
		if (digit_count < 4) {
			if (copy_all_but_leading_zeros(input,output,digit_count) == 0) {
				output--; // remove trailing space if exact multiple of 1000
			};
			return;
		//} else if (digit_count > 3003) {
		//	// millillion for 10^3003 and milli-millillion for 10^3000003
		//	int unit;
		//	if (digit_count > 3000003) {
		//		unit = 3000003;
		//	} else {
		//		unit = 3003;
		//	};
		//	int plural = (digit_count > unit+1 || input[0] != L'1');
		//	int extra_digits = digit_count-unit;
		//	//int digits_copied = copy_all_but_leading_zeros(input,output,extra_digits);
		//	//// extra_digits should be the same as digits_copied
		//	//input += digits_copied;
		//	//digit_count -= extra_digits;
		//	wordify(input,extra_digits,output,FALSE);
		//	input += extra_digits;
		//	digit_count -= extra_digits;
		//	while (input[0] == L'0' && digit_count > 0) {
		//		// strip off leading zeros
		//		input++;
		//		digit_count--;
		//	};
		//	output[0] = L' ';
		//	output++;
		//	if (unit == 3000003) {
		//		add_to_output("milli-",output);
		//	};
		//	add_to_output("milli",output);
		//	add_to_output(S(IDS_LLION_SINGULAR+plural,"llion"),output);
		//	output[0] = L' ';
		//	output++;
		} else {
			int power_minus_3 = digit_count-4; // e.g. 1234 (4 digits) is 10 to the power 3
			int up_to_three_digits = 1+power_minus_3%3; 
			int digits_copied = copy_all_but_leading_zeros(input,output,up_to_three_digits);
			input += up_to_three_digits;
			digit_count -= up_to_three_digits;
			if (digits_copied > 0) { // may be 1000000...
				int plural = (digits_copied != 1 || output[-1] != L'1'); // i.e. didn't just output "1"
				output[0] = L' ';
				output++;
				int to_name = power_minus_3/3;
				int ones = to_name%10;
				if (to_name < 10) {
					// use familar names thousands, millions, billions, and trillions
					string name = NULL;
					switch (to_name) {
						case 0:
							name = S(IDS_THOUSAND_SINGULAR+plural,"thousand");
							break;
						case 1:
							name = S(IDS_MILLION_SINGULAR+plural,"million");
							break;
						case 2:
							name = S(IDS_BILLION_SINGULAR+plural,"billion");
							break;
						case 3:
							name = S(IDS_TRILLION_SINGULAR+plural,"trillion");
							break;
						case 4:
							name = S(IDS_QUADRILLION_SINGULAR+plural,"quadrillion");
							break;
						case 5:
							name = S(IDS_QUINTILLION_SINGULAR+plural,"quintillion");
							break;
						case 6:
							name = S(IDS_SEXTILLION_SINGULAR+plural,"sextillion");
							break;
						case 7:
							name = S(IDS_SEPTILLION_SINGULAR+plural,"septillion");
							break;
						case 8:
							name = S(IDS_OCTILLION_SINGULAR+plural,"octillion");
							break;
						case 9:
							name = S(IDS_NONILLION_SINGULAR+plural,"nonillion");
							break;
					};
					add_to_output(name,output);
					output[0] = L' ';
					output++;
				} else {
					int tens = (to_name/10)%10;
					int hundreds = (to_name/100)%10;
					int thousands = (to_name/1000)%10;
					int ten_thousands = (to_name/10000)%10;
					int hundred_thousands = (to_name/100000)%10;
					int millions = (to_name/1000000)%10;
					wide_string one_name = one_names[ones];
					wide_string ten_name = ten_names[tens];
					wide_string hundred_name = hundred_names[hundreds];
					wide_string thousand_name = thousand_names[thousands];
					wide_string ten_thousand_name = ten_thousand_names[ten_thousands];
					wide_string hundred_thousand_name = hundred_thousand_names[hundred_thousands];
					wide_string million_name = million_names[millions];
					wide_string previous_suffix = NULL;
					if (one_name[0] != NULL) {
						add_to_output(one_name,output);
						previous_suffix = one_suffixes[ones];
					};
					if (ten_name[0] != NULL) {
						wide_character suffix = in_common(previous_suffix,ten_prefixes[tens]);
						if (suffix != NULL) {
							output[0] = suffix;
							output++;
						};
						previous_suffix = NULL; // used it up
						add_to_output(ten_name,output);
					};
					if (hundred_name[0] != NULL) {
						wide_character suffix = in_common(previous_suffix,hundred_prefixes[hundreds]);
						if (suffix != NULL) {
							output[0] = suffix;
							output++;
						};
						previous_suffix = NULL; // used it up
						add_to_output(hundred_name,output);
					};
					// not clear what should be the suffix so skipping it for the following
					add_to_output(thousand_name,output);
					add_to_output(ten_thousand_name,output);
					add_to_output(hundred_thousand_name,output);
					add_to_output(million_name,output);
					if (output[-1] == L'a') {
						output[-1] = L'i';
					};
					add_to_output(S(IDS_LLION_SINGULAR+plural,"llion"),output);
					output[0] = ' ';
					output++;
				};
			};
		};
	};	
};

void process_for_text_to_speech(wide_string input, int length, wide_string output) {
	for (int i = 0; i < length; i++) {
		//if (input[i] == L'-' && i+1 < length && is_digit(input[i+1]) { // no need to treat specially since -1 million is fine
		//} else 
		if (is_digit(input[i])) {
			// search forward to end (or ignore if not really a number -- e.g. 123ert456)
			int j = i+1;
			while (j < length) {
				if (is_digit(input[j])) {
					j++;
				} else if (input[j] == L'.' || // either end of number (e.g. a sentence) or about to have decimal part
					        input[j] == L' ' || // end of word or about to be followed by fraction
							  input[j] == L',' || // end of word (phrase) -- not supporting 1,000,000 (at least just yet)
							  (input[j] == L'/' && j+1 < length && is_digit(input[j+1]))) {
					wordify(input+i,j-i,output);
					i = j-1;
					if (input[j] == L'.' && j+i < length && is_digit(input[j+1])) {
						// wasn't a period but is a decimal point
						output[0] = L'.';
						output++;
						i += 2; // since was to just before the decimal point
						while (i < length) {
							if (is_digit(input[i])) {
								output[0] = input[i];
								output++;
								output[0] = L' '; // since after decimal point just read off the digits but if too many good to have spaces
								output++;
								i++;
							} else {
								break;
							};
						};
						if (output[-1] == L' ') {
							output--; // remove trailing space
						};
						i--; // go back one for next iteration of outer for loop
						break;
					} else if (input[j] == L'/') {
						// not all text-to-speech engines pronounce 1/4 as "one over four"
						// and 1/4000 becomes 1/4 thousand which is easy to mistake for 250
						output[0] = L' ';
						output++;
						add_to_output(S(IDS_OVER,"over"),output);
						output[0] = L' ';
						output++;
						i = j; // handled the /
						break;
					} else if (input[j] == L' ' && j+1 < length && is_digit(input[j+1])) {
						// make sure the following is a fraction
						int search_forward = j+2;
						while (search_forward < length) {
							if (input[search_forward] == L'/') {
								if (search_forward+1 < length && is_digit(input[search_forward+1])) {
									output[0] = L' ';
									output++;
									add_to_output(S(IDS_AND,"and"),output);
									output[0] = L' ';
									output++;
									i = j;
								};
								break;
							};
							search_forward++;
						};
						break; // wasn't followed immediately by a fraction
					} else {
						break; // done with this number continue with outer loop
					};
				} else {
					// wasn't a number after all
					int segment_length = j-i;
					memcpy(output,input+i,segment_length*sizeof(wide_character));
					i += segment_length;
					output += segment_length;
					break;
				};
			};
			if (j == length) { // entire thing ended with a digit
				wordify(input+i,j-i,output);
				output[0] = NULL; // terminate it
				return;
			};
		} else { // isn't a digit so just copy it over
			output[0] = input[i];
			output++;
		};
	};
	output[0] = NULL; // terminate it
};

#endif

string process_for_text_to_speech(string to_say, int line_length) {
   const int max_text_to_speech_length = 2000;
   string text_out = new character[max_text_to_speech_length+1];
   int j = 0;
   for (int i = 0; i < line_length; i++) {
      if (to_say[i] == '(') { // remove parenthetical remark
         while (i < line_length) {
            if (to_say[i] == ')') {
               i++; // was commented out but restored on 211200 so you don't hear 'right paren' spoken
               break; // for while
            };
            i++;
         };
			if (i >= line_length) break; // new on 230500 in case close paren is coming from another balloon - condition added on 211200
      } else if (to_say[i] == ')') { // new on 230500 since sometimes text is split by parenthetical remark between bubbles
			j = 0; // ignore everything before )
			i++;
		}
      if (to_say[i] == '_') { // || // used as a marker for a potential hypen
//          to_say[i] == '\'') { // apostrophes treated differently in SAPI 3.0 than 2.0
         i++; // skip over it
      };
      if (to_say[i] == '-' &&
          i > 0 && (to_say[i-1] == ' ' || to_say[i-1] == '\'' || to_say[i-1] == '"') &&
          i+1 < line_length && !IsCharAlphaNumericA(to_say[i+1]) && to_say[i+1] != ' ') { 
			// to_say[i+1] != ' ' added on 220499 to avoid saying minus for dash
         // problems with hypen vs minus -- this works around it
			// but this says "minus" when there is just a dash...
         string replacement = _T(" minus ");
         for (unsigned int k = 0; k < _tcslen(replacement); k++) {
            text_out[j++] = replacement[k];
            if (j == max_text_to_speech_length) break;
         };
         i++; // skip the space too
/*
      } else if (tt_language == BRITISH && to_say[i] >= '1' && to_say[i] <= '9' &&
			        (i == 0 || !IsCharAlpha(to_say[i-1]))) { 
         // without commas MSTTS just says the digits
			boolean contains_comma = FALSE; // assume it doesn't already contain commas
         int last_digit = i;
         while (to_say[last_digit] >= '0' && to_say[last_digit] <= '9') {
           last_digit++;
			  if (to_say[last_digit] == ',') {
					contains_comma = TRUE;
					last_digit++;
			  };
         };
			last_digit--; // went one too far
			boolean at_least_a_billion = (last_digit-i>8);  // don't speak it as a billion
			for (; i <= last_digit; i++) {
				text_out[j++] = to_say[i];
				if (!contains_comma && i < last_digit && !at_least_a_billion && i%3 == last_digit%3) {
					text_out[j++] = ','; // add comma for speaking
				};
			};
*/
		} else if (to_say[i] == '0' && i+1 < line_length && to_say[i+1] == '.' && 
					  (i+2 == line_length || to_say[i+2] < '0' || to_say[i+2] > '9')) {
			// engine says "zero point" when it encounters "0."
			i += 2;
			text_out[j++] = '0'; // but not the period
		} else if (to_say[i] == '.' && 
					  i > 0 && IsCharAlpha(to_say[i-1]) && i+1 < line_length && IsCharAlpha(to_say[i+1]) &&
					  spoken_language() == BRAZILIAN_PORTUGUESE) { // new on 140800
			// to speak www.toontalk.com better - every language could benefit but would need to generate new VCE
			text_out[j++] = ' '; 
			text_out[j++] = 'p';
			text_out[j++] = 'o'; 
			text_out[j++] = 'n';
			text_out[j++] = 't';
			text_out[j++] = 'o';
			text_out[j++] = ' ';
		} else if (portuguese_spoken() && (to_say[i] == a224 || to_say[i] == a192) && // new on 240500
					  (i+1 == line_length || !IsCharAlpha(to_say[i+1]))) { // and is followed by non alphabetic - new on 080600
			text_out[j++] = a225; // spoken the same (at least in Brazil) and don't have separate entries
		} else if (spoken_language() == BRAZILIAN_PORTUGUESE && to_say[i] == ':' && // new on 080600
					  i > 0 && (to_say[i-1] == '"' || to_say[i-1] == '\'') && 
					  i+1 < line_length && (to_say[i+1] == '"' || to_say[i+1] == '\'')) { // these conditions added on 150600
			// and on 160600 enabled ':' as well as ":"
			// really any language but no entries for other languages
			memcpy(text_out+j," doispontos ",12); // new on 100600 - space eliminated between dois and pontos since conflicts with "os dois" entry
			j += 12; 
		} else if (portuguese_spoken() && to_say[i] == 'h' && // new on 080600
					  i > 1 && i+1 < line_length && 
					  to_say[i-1] >= '0' && to_say[i-1] <= '9' &&
					  to_say[i+1] >= '0' && to_say[i+1] <= '9') { // h with digits on both sides is a time of day - new on 080600
			// spoken as e (meaning and) which is exxx
			text_out[j++] = ' '; 
			text_out[j++] = 'e';
			text_out[j++] = 'x'; 
			text_out[j++] = 'x';
			text_out[j++] = 'x';
			text_out[j++] = ' ';
      } else if ((spoken_language() == AMERICAN || spoken_language() == BRITISH || spoken_language() == SWEDISH || 
						// AMERICAN added on 090202 since bignums and rationals need it
						spoken_language() == BRAZILIAN_PORTUGUESE || spoken_language() == PORTUGAL_PORTUGUESE) && // new on 210300
						// added check for - followed by digit
						((to_say[i] >= '1' && to_say[i] <= '9') || (to_say[i] == '-' && i+1 < line_length && to_say[i+1] >= '1' && to_say[i+1] <= '9')) 
						&& (i == 0 || !IsCharAlpha(to_say[i-1]))) {
         // engine does bad job with Swedish and Portuguese numbers
         string number = new character[line_length+1];
         int number_index = 0;
         int say_index = i;
			boolean separator = FALSE;
         while (say_index < line_length &&
                ((to_say[i] >= '0' && to_say[i] <= '9') ||
					 to_say[i] == '-' || // for negative numbers - added 300400
                to_say[i] == ' ')) { // || to_say[i] == ',' || to_say[i] == '.')) { 
						 // '.' added on 210300 -- commented out on 090202 so that "1.5" isn't pronounced fifteen
            if (to_say[i] != ' ') { // && to_say[i] != ',' && to_say[i] != '.') { -- commented out on 090202 so that "1.5" isn't pronounced fifteen
					// commas shouldn't be there in Swedish but if they are is it OK to ignore them?
               number[number_index++] = to_say[i];
					separator = FALSE;
            } else if (separator) {
					break; // two in a row
				} else {
					separator = TRUE;
					if (spoken_language() == BRAZILIAN_PORTUGUESE) { // new on 050600
						// condition should really be when "thousands comma" is ". " (or could support ", " as well)
						if (to_say[i] == '.' && i+2 < line_length && to_say[i+1] == ' ' && to_say[i+2] >= '0' && to_say[i+2] <= '9') {
							separator = FALSE; // since ". " is one seperator
						};
					};
				};
            i++;
         };
//			Ordinal ordinal = CARDINAL;
//			if (english()) {
//				if (to_say[i] == 't' && to_say[i+1] == 'h') {
//					ordinal = ORDINAL_TH;
//				} else if (to_say[i] == 's' && to_say[i+1] == 't') {
//					ordinal = ORDINAL_ST;
//				} else if (to_say[i] == 'r' && to_say[i+1] == 'd') {
//					ordinal = ORDINAL_RD;
//				} else if (to_say[i] == 'n' && to_say[i+1] == 'd') {
//					ordinal = ORDINAL_ND;
//				};
//			};
         i--; // restore last non-digit
         number[number_index] = '\0'; // terminate string
			long number_word_size = _tcslen(number)*12; // let kids play with really long numbers
         string replacement = new character[number_word_size]; 
         output_string_stream stream(replacement,number_word_size);
         number_replacement(number,stream); //,ordinal);
         stream.put('\0'); // terminate string
         for (unsigned int k = 0; k < _tcslen(replacement) && j < max_text_to_speech_length; k++) {
            text_out[j++] = replacement[k];
         };
         delete [] number;
         delete [] replacement;
      } else if //(to_say[i] == '.' && i+2 < line_length && to_say[i+1] == '.' && to_say[i+2] == '.') {
					(to_say[i] == '"' && i+3 < line_length && to_say[i+1] == '.' && to_say[i+2] == '.' && to_say[i+3] == '.') {
			// updated on 110700 so that this substitution only happens when quoted
         string replacement;
         switch (spoken_language()) {
            case SWEDISH:
              replacement = _T(" punktpunktpunkt ");
              break;
//            case AMERICAN:
            default:
              replacement = _T(" dotdotdot ");
              break;
         };
         i += 3;
         for (unsigned int k = 0; k < _tcslen(replacement); k++) {
            text_out[j++] = replacement[k];
            if (j == max_text_to_speech_length) break;
         };
		} else if ((portuguese_spoken()) && // (tt_language == BRAZILIAN_PORTUGUESE || tt_language == PORTUGAL_PORTUGUESE) &&
					  (to_say[i] == 'a' || to_say[i] == 'e' || to_say[i] == 'o' ||
					   to_say[i] == 'A' || to_say[i] == 'E' || to_say[i] == 'O') &&
					  (i == 0 || to_say[i-1] == ' ') &&
					  (i+1 == line_length || to_say[i+1] == ' ')) {
			// new on 030100 so that single letter words are pronounced correctly - they have extries like axxx
			text_out[j++] = to_say[i];
			for (int k = 0; k < 3; k++) {
				text_out[j++] = 'x';
			};
		} else if (to_say[i] == '-') { 
			// new on 060300 since generation of phrase.ini file treated hypens like spaces but tts engine doesn't so fix here
//			if (tt_language != BRAZILIAN_PORTUGUESE) { // condition new on 040500
			// commented out conditional on 080500 since now phrase.ini treats - as space in these cases
			// somehow Brazilian Portuguese was recorded with - as nothing (e.g. ajudá-lo as ajudálo)
			// problem fixed on 080500 and was due to - being nothing in puzzle files but space in RC files
			if (tt_language != PORTUGAL_PORTUGUESE || tt_system_mode != PUZZLE) { // patch until Portugal generates a new VCE file
				text_out[j++] = ' ';
			};
			if (spoken_language() == BRAZILIAN_PORTUGUESE && i+1 < line_length && to_say[i+1] == 'o' && 
				 ((i > 8 && strnicmp(to_say+(i-8),"soltando",8) == 0) || (i > 9 && strnicmp(to_say+(i-9),"colocando",9) == 0))) { 
				// new on 160800
				// from Bete:
//				1) Regarding robots and nests (Free Play): "soltando-o" and "colocando-o"
//				The problem is the pronunciation of the last "o" (after the "-").
//				Attached you will find wavs s000083 (ó)  and s000089 (ô).
//				You have to use s000089, because that letter "o" is a "closed" one; it is
//				not an "open" sound like wave s000083.
				text_out[j++] = o244;
				i++;
			};
		} else if (to_say[i] == '/' && english() && i > 1 && (to_say[i-1] >= '0' && to_say[i-1] <= '9') && i+1 < line_length && (to_say[i+1] >= '0' && to_say[i+1] <= '9')) { 
			// new on 090202 to speak fractions better in English -- think about other languages
			text_out[j++] = ' '; 
			text_out[j++] = 'o';
			text_out[j++] = 'v'; 
			text_out[j++] = 'e';
			text_out[j++] = 'r';
			text_out[j++] = ' ';
		} else if (to_say[i] == '.' && english() && i > 1 && (to_say[i-1] >= '0' && to_say[i-1] <= '9') && i+1 < line_length && (to_say[i+1] >= '0' && to_say[i+1] <= '9')) { 
			// new on 090202 to speak fractions better in English -- think about other languages
			text_out[j++] = ' '; 
			text_out[j++] = 'p';
			text_out[j++] = 'o'; 
			text_out[j++] = 'i';
			text_out[j++] = 'n';
			text_out[j++] = 't';
			text_out[j++] = ' ';
			i++;
			while (i < line_length && (to_say[i] >= '0' && to_say[i] <= '9')) { // don't want to read 2.101 as "two point one hundred and one"
				text_out[j++] = to_say[i++];
				text_out[j++] = ' '; // spaces so they are read as digits
			};
      } else if (!((to_say[i] == '\'' || to_say[i] == '\"') && i > 0 &&
                   (to_say[i-1] == ' ' ||
                    i+1 < line_length &&
                    (to_say[i+1] == ' ' || to_say[i+1] == '.' ||
                     to_say[i+1] == '?' || to_say[i+1] == '!' || to_say[i+1] == ',')) )) {
         // skip ' if before or after a space since
         // spoken as "apostrophe"
         // quotes seem to throw things off and aren't needed here
//         if (need_wide_characters() || (to_say[i] > 31 && to_say[i] < 128)) { 
				// added on 030699 to avoid crashing MSTTS - but is it needed?
				text_out[j++] = to_say[i];
//			};
         if (j == max_text_to_speech_length) break;
      };
   };
   text_out[j] = NULL; // terminate
	lower_case(text_out); // why??
#if TT_DEBUG_ON
   if (tt_debug_mode == 55555) log(text_out);
#endif
   return(text_out);
};

void Talk_Balloon::speak_next_sentence() {
	if (!text_to_speech() && tt_marty_talk != MARTY_WITH_SUBTITLES) return; 
	// second condition added 110401 since MARTY_WITH_SUBTITLES uses "simulated" speech
   speech_text_offset_increment = 0;
   if (tt_programmer->kind_of() != PROGRAMMER_TITLES &&
       speech_text_offset < speech_text_length) {
//       (!displayed_dots || speech_text_offset < text_offset+text_offset_increment)) {
//       tt_martian->turn_on_speech_and_sound_off();  // no-op if already is on
       string to_say;
		 if (alternative_spoken_text != NULL) {
			 to_say = alternative_spoken_text+speech_text_offset;
		 } else {
			 to_say = text+speech_text_offset;
		 };
		 int max_length = speech_text_length-speech_text_offset;
// commented out the following on 171099 since now have multi-line subtitles if sentence is very long
// but I don't want to break spoken sentences just to get subtitles looking nice
//		 if (tt_closed_caption || tt_closed_caption_alternative_language) {
//			 int new_max_length = max_subtitle_length()*2;
//			 if (max_length > new_max_length) {
//				 max_length = new_max_length;
//			 };
//		 }
       int line_length = 0;
		 if (tt_marty_talk == MARTY_SPEAKS_AND_TALK_BALLOONS && !alternative_language_spoken()) { 
			 // new on 040500 so that speech and text balloons are in better synch
			 if (speech_should_wait_for_first_display) { // first time
//				 speech_should_wait_for_first_display = FALSE; 
				 // by using tt_dummy_picture this will run later regardless of tt_finish_all_animations (new 070600)
				 tt_global_picture->do_after(1,this,SPEAK_NEXT_SENTENCE_CALLBACK); // wait a frame
				 return;
			 };
//#if TT_DEBUG_ON
//			 if (tt_debug_mode == 210405) {
//				 if (strlen(to_say) < text_offset_increment) {
//					 debug_this();
//				 };
//			 };
//#endif
			 line_length = text_offset_increment; 
			 int to_say_length = strlen(to_say);
			 if (to_say_length < line_length) {
				 // new on 210405 since text_offset_increment can be too big
				line_length = to_say_length;
			 };
		 };
		 if (line_length == 0) {
			 line_length = good_place_to_break(to_say,max_length);
		 };
//		 if (speech_text_offset == 0 && tt_marty_talk == MARTY_SPEAKS_AND_TALK_BALLOONS && second_break != 0) {
//			 line_length = second_break;
//		 };
       speech_text_offset_increment = line_length;
#if TT_DEBUG_ON
		 if (tt_debug_mode == 44444) { 
			 tt_error_file() << "Starting to speak the following at " << timeGetTime() << "ms. Frame = " << tt_frame_number << endl;
			 tt_error_file().write(to_say,line_length);
			 tt_error_file() << endl;
		 };
#endif
		 wide_string wide_text;
		 if (tt_limited_text_to_speech) {
			 if (alternative_language_spoken()) tt_use_spoken_alternative = TRUE; 
			 string a_balloon_full = process_for_text_to_speech(to_say,line_length);
#if TT_DEBUG_ON
			 if (tt_debug_mode == 44444) { 
				 tt_error_file() << "And it was converted to the following for the speech engine:" << endl;
				 tt_error_file().write(a_balloon_full,strlen(a_balloon_full));
				 tt_error_file() << endl;
			 };
#endif
			 tt_use_spoken_alternative = FALSE;
			 wide_text = copy_wide_string(a_balloon_full);
			 delete [] a_balloon_full;
#if TT_SPEAK_ALL_NUMBERS
		 } else {
			 // new on 071104
			 char saved_last_char = to_say[line_length]; // new on 210405 since the following shouldn't check entire to_say
			 to_say[line_length] = '\0'; // new on 210405
			 if (strcspn(to_say,"123456789") == line_length) { 
				 // note that this didn't do the right thing prior to 210405 if line_length was different from strlen(to_say)
				 wide_text = copy_wide_string(to_say,line_length); // no digits (other than 0) so no need to do this extra work
			 } else {
				wide_string wide_to_say = copy_wide_string(to_say,line_length); // pretty wasteful...
				wide_text = copy_and_process_for_text_to_speech(wide_to_say,line_length);
				delete [] wide_to_say;
			 };
			 to_say[line_length] = saved_last_char; // restore
#else
		 } else { // new on 060200 so that if general speech engine I let it deal with things like numbers, hypens, elipsis, etc.
			 wide_text = copy_wide_string(to_say,line_length);
#endif
		 };
       if (tt_marty_talk == MARTY_WITH_SUBTITLES || !speak(wide_text,say_id)) { // new on 110401 - need to trigger speech
			 tt_martian->set_saved_say_id(say_id);
			 millisecond duration = compute_reading_duration(wide_string_length(wide_text));
			 if (need_wide_characters()) {
//				 duration = (duration*3)/2; // 50% longer since slower to read Kanji
			 } else {
				 duration -= 3000; // subtract 3 seconds since seems faster to read subtitles
			 };
#if TT_DEBUG_ON
			 if (tt_debug_mode == 110401) {
				 tt_error_file() << "At " << tt_current_time << " there is " 
									  << duration << "ms until the simulated end of saying id = " << say_id << " " << copy_narrow_string(wide_text) << endl;
			 };
#endif
//			 if (simulated_speech == NULL) { // new on 110401 a private scheduler just for simulating speech - very minor memory leak
//				 simulated_speech = new GlobalPicture();
//			 };
//			 simulated_speech->do_after(duration,tt_martian,speech_has_finished_callback);
			 tt_martian->do_after(duration,tt_martian,SPEECH_HAS_FINISHED_CALLBACK);
		 };
		 start_of_current_sentence_spoken = tt_current_time; // timeGetTime();
	    characters_in_last_sentence_spoken = wide_string_length(wide_text);
		 if (tt_closed_caption) { // displays text as subtitles spoken
			 tt_screen->set_subtitle(to_say,line_length,-1);
		 } else if (tt_closed_caption_alternative_language) {
			 int offset = 1;
			 char last_spoken_character = to_say[line_length-offset];
			 while (last_spoken_character == ' ' && line_length-offset-1 > 0) {
				 offset++;
				 last_spoken_character = to_say[line_length-offset];
			 };
			 int sentence_count = 0;
			 if (last_spoken_character == '.' || last_spoken_character == '?' || last_spoken_character == '!') {
				 sentence_count = count_sentences(to_say,line_length);
			 };
			 next_subtitle_for_closed_caption_alternative_language(line_length,sentence_count);
		 };
		 if (tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES || tt_marty_talk == MARTY_WITH_SUBTITLES) {
			 text_removal_time = tt_current_time+10000; // leave it up for 10 seconds if nothing else is spoken
		 };
		 delete [] wide_text;
   };
};

void Talk_Balloon::next_subtitle_for_closed_caption_alternative_language(int speech_line_length, int sentence_count) { 
	// abstracted into a method from the above on 160701
	// if finished speaking (e.g. with different languages) then speech_line_length will be zero
	// last_spoken_character added on 100102 so that we try to match sentence for sentence if last_spoken_character is '.', '?', or '!'.
	if (speech_text_offset_increment == 0 || length > text_offset) {
		// either nothing more to say or more display - but don't remove subtitles if speaking and last subtitle
		int max_length = length-text_offset;
		int new_max_length = max_subtitle_length()*2;
		if (max_length > new_max_length) {
			max_length = new_max_length;
		};
		int max_length_to_equal_speech;
		if (need_wide_characters()) {
			max_length_to_equal_speech = speech_line_length; // Kanji is denser
			if (max_length > max_length_to_equal_speech) { // moved here on 091199
				max_length = max_length_to_equal_speech;
			};
		} else {
			// on 171099 changed following so subtitles can be long since spoken text may have paused at a comma
			// that doesn't even exist in the translation
			// following was max_length; prior to 081199
			max_length = max(max_length,(speech_line_length*3)/2); // 50% more since translations can be longer
		};
		if (max_length == 0) { // new on 160701
			max_length = length-text_offset;
		};
		text_offset_increment = good_place_to_break(text+text_offset,max_length,sentence_count);
		millisecond duration;
		if (speech_text_offset >= speech_text_length) { // new on 160701
			duration = compute_reading_duration(text_offset_increment);
		} else {
			duration = -1; // let it stay up until speech is finished
		};
		if (text != NULL) { // condition new on 170105 for robustness (happened visiting a city saved in the PT version)
			tt_screen->set_subtitle(text+text_offset,text_offset_increment,duration); // last line was -1 prior to 160701
		};
#if TT_DEBUG_ON
		if (tt_debug_mode == 55555) {
			tt_error_file() << "Subtitle: ";
			tt_error_file().write(text+text_offset,text_offset_increment);
			tt_error_file() << endl;
		};
		if (tt_debug_mode == 44444) {
			tt_error_file() << "text_offset_increment set to " << text_offset_increment << endl;
		};
#endif
		text_offset += text_offset_increment;
	};
	// following new on 160701
	if (speech_text_offset >= speech_text_length) { // finished speaking
		if (length > text_offset) { // and still more subtitles to display
			do_after(compute_reading_duration(text_offset_increment),this,NEXT_SUBTITLE_FOR_CLOSED_CAPTION_ALTERNATIVE_LANGUAGE_CALLBACK);
		} else {
			tt_screen->remove_current_subtitle(); // better to delay this??
		};
	};
};

#if TT_XML

void Talk_Balloon::xml(xml_element *element, xml_document *document) {
   // updated on 151102 to be sure that all state variables are included
   if (waiting_to_show_more) xml_set_attribute(element,L"WaitingToShowMore",1);
   if (displayed_dots) xml_set_attribute(element,L"DisplayedDots",1);
   if (text != NULL) {
      xml_append_child(xml_character_data_element(text,L"Text",document),element);
   };
   if (alternative_spoken_text != NULL) {
      xml_append_child(xml_character_data_element(alternative_spoken_text,L"AlternativeSpokenText",document),element);
   };
   xml_set_attribute(element,L"Length",length);
   xml_set_attribute(element,L"TextOffset",text_offset);
   xml_set_attribute(element,L"TextOffsetIncrement",text_offset_increment); // missing prior to 151102
   if (!speech_should_wait_for_first_display) {
      xml_set_attribute(element,L"SpeechShouldNotWaitForFirstDisplay",!speech_should_wait_for_first_display); // missing prior to 151102
   };
   xml_set_attribute(element,L"FullSizePercent",full_size_percent);
   xml_set_attribute(element,L"CharactersFittingInFullSize",characters_fitting_in_full_size);
   xml_set_attribute(element,L"CharacterWidth",character_width);
   xml_set_attribute(element,L"CharacterHeight",character_height);
   xml_set_attribute(element,L"TextRemovalTime",text_removal_time);
   xml_set_attribute(element,L"OldLLX",old_llx);
   xml_set_attribute(element,L"OldLLY",old_lly);
   xml_set_attribute(element,L"ParagraphCount",paragraph_count);
   xml_set_attribute(element,L"RecentSentenceCount",recent_sentence_count);
   xml_set_attribute(element,L"OldestRecentSentenceIndex",oldest_recent_sentence_index);
   xml_set_attribute(element,L"SpeechTextOffset",speech_text_offset);
   xml_set_attribute(element,L"SpeechTextOffsetIncrement",speech_text_offset_increment);
   xml_set_attribute(element,L"SpeechTextLength",speech_text_length);
   xml_set_attribute(element,L"SayID",say_id);
   xml_set_attribute(element,L"DurationOfLastSentenceSpoken",duration_of_last_sentence_spoken);
   xml_set_attribute(element,L"StartOfCurrentSentenceSpoken",start_of_current_sentence_spoken);
   xml_set_attribute(element,L"CharactersInLastSentenceSpoken",characters_in_last_sentence_spoken);
   if (martian != NULL) { // new on 151102
		// don't need to check for independence for time travel include segments since is never ok_to_cache
      martian->xml_create_and_append_element(element,document)->Release();
   };
   //wide_character recent_sentence_with_number[32];
   //int recent_sentence_length = strlen("RecentSentence");
   //wmemcpy(recent_sentence_with_number,L"RecentSentence",recent_sentence_length);
   //wide_character paragraph_end_with_number[32];
   //int paragraph_end_length = strlen("ParagraphEnd");
   //wmemcpy(paragraph_end_with_number,L"ParagraphEnd",paragraph_end_length);
	for (int i  = 0; i < recent_sentence_count; i++) {
      //_itow(i,recent_sentence_with_number+recent_sentence_length,10);
      //_itow(i,paragraph_end_with_number+paragraph_end_length,10);
//      xml_set_attribute(element,recent_sentence_with_number,recent_sentence[i]);
      if (recent_sentence[i] != NULL || paragraph_end[i] > 0) { 
         // added paragraph_end to conditional on 161102 so that an empty element is created so it can have the ParagraphEnd attribute
         xml_element *data_element = xml_character_data_element(recent_sentence[i],L"RecentSentence",document);
         xml_set_attribute(data_element,L"Index",i);
         xml_set_attribute(data_element,L"ParagraphEnd",paragraph_end[i]);
			xml_append_child(data_element,element); // moved here on 190804
      };
//      xml_set_attribute(element,paragraph_end_with_number,paragraph_end[i]);
	};
   Sprite::xml(element,document);
};

boolean Talk_Balloon::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
      case RECENT_SENTENCE_TAG: {
         int index = xml_get_attribute_int(node,L"Index",-1);
         if (index >= 0 && index < recent_sentence_count) {
            paragraph_end[index] = xml_get_attribute_int(node,L"ParagraphEnd",paragraph_end[index]);
            recent_sentence[index] = xml_get_narrow_text_copy(node);
         }; // else warn? return(FALSE)?
         return(TRUE);
                                };
      case TEXT_TAG:
         text = xml_get_narrow_text_copy(node);
         return(TRUE);
      case ALTERNATIVE_SPOKEN_TEXT_TAG:
         alternative_spoken_text = xml_get_narrow_text_copy(node);
         return(TRUE);
      case MARTIAN_TAG:
         martian = (Martian *) xml_load_sprite(node,tag,martian,MARTIAN);
         return(TRUE);
      //case LINK_TAG: { // new on 221102 -- should be Martian -- not needed anymore (070103)
      //   Sprite *sprite = xml_load_sprite(node,tag);
      //   if (sprite != NULL) {
      //      if (sprite->kind_of() == MARTIAN) {
      //         martian = (Martian *) sprite;
      //      } else {
      //         sprite->destroy();
      //         // warn?
      //         return(FALSE);
      //      };
      //   };
      //   return(TRUE);
      //               };
      case NO_MORE_TAGS: {
         waiting_to_show_more = xml_get_attribute_int(node,L"WaitingToShowMore",0); // prior to 210404 waiting_to_show_more
         displayed_dots = xml_get_attribute_int(node,L"DisplayedDots",0); // prior to 210404 displayed_dots
         //text = xml_get_attribute_string(node,L"Text",text);
         //alternative_spoken_text = xml_get_attribute_string(node,L"AlternativeSpokenText",alternative_spoken_text);
         length = xml_get_attribute_int(node,L"Length",length);
         text_offset = xml_get_attribute_int(node,L"TextOffset",text_offset);
         full_size_percent = xml_get_attribute_int(node,L"FullSizePercent",full_size_percent);
         characters_fitting_in_full_size = xml_get_attribute_int(node,L"CharactersFittingInFullSize",characters_fitting_in_full_size);
         character_width = xml_get_attribute_int(node,L"CharacterWidth",character_width);
         character_height = xml_get_attribute_int(node,L"CharacterHeight",character_height);
         text_removal_time = xml_get_attribute_int(node,L"TextRemovalTime",text_removal_time);
         old_llx = xml_get_attribute_int(node,L"OldLLX",old_llx);
         old_lly = xml_get_attribute_int(node,L"OldLLY",old_lly);
         paragraph_count = xml_get_attribute_int(node,L"ParagraphCount",paragraph_count);
         recent_sentence_count = xml_get_attribute_int(node,L"RecentSentenceCount",recent_sentence_count);
         oldest_recent_sentence_index = xml_get_attribute_int(node,L"OldestRecentSentenceIndex",oldest_recent_sentence_index);
         speech_text_offset = xml_get_attribute_int(node,L"SpeechTextOffset",speech_text_offset);
         speech_text_offset_increment = xml_get_attribute_int(node,L"SpeechTextOffsetIncrement",speech_text_offset_increment);
         speech_text_length = xml_get_attribute_int(node,L"SpeechTextLength",speech_text_length);
         say_id = xml_get_attribute_int(node,L"SayID",say_id);
         duration_of_last_sentence_spoken = xml_get_attribute_int(node,L"DurationOfLastSentenceSpoken",duration_of_last_sentence_spoken);
         start_of_current_sentence_spoken = xml_get_attribute_int(node,L"StartOfCurrentSentenceSpoken",start_of_current_sentence_spoken);
         characters_in_last_sentence_spoken = xml_get_attribute_int(node,L"CharactersInLastSentenceSpoken",characters_in_last_sentence_spoken);
         text_offset_increment = xml_get_attribute_int(node,L"TextOffsetIncrement",text_offset_increment); 
         speech_should_wait_for_first_display = !xml_get_attribute_int(node,L"SpeechShouldNotWaitForFirstDisplay",0); // prior to 210404 !speech_should_wait_for_first_display 
       //  wide_character recent_sentence_with_number[32];
       //  int recent_sentence_length  = strlen("RecentSentence");
       //  wmemcpy(recent_sentence_with_number,L"RecentSentence",recent_sentence_length);
       //  wide_character paragraph_end_with_number[32];
       //  int paragraph_end_length  = strlen("ParagraphEnd");
       //  wmemcpy(paragraph_end_with_number,L"ParagraphEnd",paragraph_end_length);
	      //for (int i  = 0; i < recent_sentence_count; i++) {
       //     _itow(i,recent_sentence_with_number+recent_sentence_length,10);
       //     _itow(i,paragraph_end_with_number+paragraph_end_length,10);
       //     recent_sentence[i] = xml_get_attribute_string(node,recent_sentence_with_number,recent_sentence[i]);
       //     paragraph_end[i] = xml_get_attribute_int(node,paragraph_end_with_number,paragraph_end[i]);
	      //}; 
                         };
         // and do the following
      default:
         return(Sprite::handle_xml_tag(tag,node));
   };
};

#endif

boolean Talk_Balloon::dump(output_stream &stream, boolean just_prepare, boolean is_controlee, boolean on_nest) {
   // new on 250100
	if (!Sprite::dump(stream,just_prepare,is_controlee,on_nest)) return(FALSE);
	unsigned short flags = (waiting_to_show_more |
									((!!displayed_dots)<<1));
	stream.write((char *) &flags,sizeof(flags));
	write_string(stream,text);
	write_string(stream,alternative_spoken_text);
	stream.write((char *) &length,sizeof(length));
	stream.write((char *) &text_offset,sizeof(text_offset));
	stream.write((char *) &text_offset_increment,sizeof(text_offset_increment));
	stream.write((char *) &full_size_percent,sizeof(full_size_percent));
	stream.write((char *) &characters_fitting_in_full_size,sizeof(characters_fitting_in_full_size));
	stream.write((char *) &character_width,sizeof(character_width));
	stream.write((char *) &character_height,sizeof(character_height));
	stream.write((char *) &text_removal_time,sizeof(text_removal_time));
	stream.write((char *) &old_llx,sizeof(old_llx));
	stream.write((char *) &old_lly,sizeof(old_lly));
	stream.write((char *) &paragraph_count,sizeof(paragraph_count));
	stream.write((char *) &recent_sentence_count,sizeof(recent_sentence_count));
	stream.write((char *) &oldest_recent_sentence_index,sizeof(oldest_recent_sentence_index));
	stream.write((char *) &speech_text_offset,sizeof(speech_text_offset));
	stream.write((char *) &speech_text_offset_increment,sizeof(speech_text_offset_increment));
	stream.write((char *) &speech_text_length,sizeof(speech_text_length));
	stream.write((char *) &say_id,sizeof(say_id));
	stream.write((char *) &duration_of_last_sentence_spoken,sizeof(duration_of_last_sentence_spoken));
	stream.write((char *) &start_of_current_sentence_spoken,sizeof(start_of_current_sentence_spoken));
	stream.write((char *) &characters_in_last_sentence_spoken,sizeof(characters_in_last_sentence_spoken));
	stream.write((char *) &length,sizeof(length)); // twice (noticed on 121102)
	stream.write((char *) paragraph_end,sizeof(int)*max_paragraph_count);
	for (int i = 0; i < recent_sentence_count; i++) {
      write_string(stream,recent_sentence[i]);
	};
	return(TRUE);
};

Talk_Balloon *load_talk_balloon(InputStream *stream, int version_number, NaturalLanguage language) {
	Talk_Balloon *balloon = new Talk_Balloon(0,0);
	balloon->load(stream,version_number,language);
	return(balloon);
};

void Talk_Balloon::load(InputStream *stream, int version_number, NaturalLanguage language) {
	unsigned short flags;
	stream->read((string) &flags,sizeof(flags));
	waiting_to_show_more = !!(flags&1);
	displayed_dots = !!(flags&(1<<1));
	text = stream->read_string_old();
	alternative_spoken_text = stream->read_string_old();
	stream->read((string) &length,sizeof(length));
	stream->read((string) &text_offset,sizeof(text_offset));
	stream->read((string) &text_offset_increment,sizeof(text_offset_increment));
	stream->read((string) &full_size_percent,sizeof(full_size_percent));
	stream->read((string) &characters_fitting_in_full_size,sizeof(characters_fitting_in_full_size));
	stream->read((string) &character_width,sizeof(character_width));
	stream->read((string) &character_height,sizeof(character_height));
	stream->read((string) &text_removal_time,sizeof(text_removal_time));
	stream->read((string) &old_llx,sizeof(old_llx));
	stream->read((string) &old_lly,sizeof(old_lly));
	stream->read((string) &paragraph_count,sizeof(paragraph_count));
	stream->read((string) &recent_sentence_count,sizeof(recent_sentence_count));
	stream->read((string) &oldest_recent_sentence_index,sizeof(oldest_recent_sentence_index));
	stream->read((string) &speech_text_offset,sizeof(speech_text_offset));
	stream->read((string) &speech_text_offset_increment,sizeof(speech_text_offset_increment));
	stream->read((string) &speech_text_length,sizeof(speech_text_length));
	stream->read((string) &say_id,sizeof(say_id));
	stream->read((string) &duration_of_last_sentence_spoken,sizeof(duration_of_last_sentence_spoken));
	stream->read((string) &start_of_current_sentence_spoken,sizeof(start_of_current_sentence_spoken));
	stream->read((string) &characters_in_last_sentence_spoken,sizeof(characters_in_last_sentence_spoken));
	stream->read((string) &length,sizeof(length));
	stream->read((string) paragraph_end,sizeof(int)*max_paragraph_count);
	for (int i = 0; i < recent_sentence_count; i++) {
      recent_sentence[i] = stream->read_string_old();
	};
};


//void Talk_Balloon::next_word_spoken(int ) {
//   if (spoken_word_length != 0) {
      // restore old
//      memcpy(text+current_word_offset,spoken_word,spoken_word_length);
//      current_word_offset += spoken_word_length;
//      while (text[current_word_offset] == ' ') {
//         current_word_offset++;
//      };
//   };
//   spoken_word_length = 0;
//   while (current_word_offset+spoken_word_length < length && text[current_word_offset+spoken_word_length] != ' ') {
//      spoken_word[spoken_word_length] = text[current_word_offset+spoken_word_length];
//      spoken_word_length++;
//   };
//   if (spoken_word_length < 2) {
//      spoken_word_length = 0;
//      return;
//   };
//   spoken_word[spoken_word_length] = NULL; // terminate it
//   string word_copy = AnsiUpper(copy_string(spoken_word));
//   memcpy(text+current_word_offset,spoken_word,spoken_word_length);
//   delete [] word_copy;
//};

#endif

boolean Talk_Balloon::save_region() {
	TTRegion region;
	full_region(region);
  // stretch by a bit because of hack to keep it stationary
	region.min_x -= tile_width/2;
	region.max_x += tile_width/2;
	region.min_y -= tile_height/2;
	region.max_y += tile_height/2;
//   if (tt_frame_number >= 1554) {
//      region.print(tt_error_file());
//      tt_error_file() << " " << tt_frame_number << endl;
//   };
#if TT_DEBUG_ON
   if (tt_debug_mode == 103) {
      tt_error_file() << type_name(this) << " ";
   };
#endif
	return(tt_screen->save_city_region(region));
};

void Talk_Balloon::remove_text(boolean speech_too) {
	paragraph_count = 0;
   if (text == NULL || length == text_offset) return; // already removed (second disjunct added 110700 - since following broke teleport animation)
#if TT_DEBUG_ON
	if (tt_debug_mode == 55555) {
		log("Removing text from talk balloon");
	};
#endif
//#if TT_TEXT_TO_SPEECH
//   if (tt_marty_talk == MARTY_SPEAKS_AND_TALK_BALLOONS &&
//       tt_programmer->kind_of() != PROGRAMMER_TITLES &&
//       !regardless &&
//       text_offset < length &&
//       speech_in_progress()) {
//      remove_text_when_speech_done = TRUE;
//      return;
//   };
//#endif
//tt_error_file() << "removing text" << endl;
	waiting_to_show_more = FALSE;
	if ((tt_current_time >= text_removal_time && text_removal_time != 0)|| finishing_instantly()) {
		// been up long enough (or user pressed F7) 
		// that it won't repeat until something else is said in between
		// added text_removal_time != 0 on 181004 since if never set (e.g. MARTY_SPEAKS and no balloons) then this does the wrong thing
		// if you press F1 repeatedly Marty doesn't go away
		text_offset = length; // nothing left to say
//#if TT_TEXT_TO_SPEECH
//   } else if (tt_marty_talk == MARTY_SPEAKS) {
//		text_offset = length; // to avoid repetitions
//#endif
	} else {
		length = 0;
		text_offset = 0;
      say_id++;
		if (text != NULL) {
			delete [] text;
			text = NULL;
		};
	};
	if (speech_too || !alternative_language_spoken()) { // can get out of synch if alternate language is being spoken
		speech_text_offset = 0;
		speech_text_offset_increment = 0;
		speech_text_length = 0;
	};
	if (tt_closed_caption || tt_closed_caption_alternative_language) {
		tt_screen->remove_current_subtitle();
	};
	if (tt_martian != NULL) { // condition new since this may be inside of titles
		tt_martian->say_completed(); // moved to after removing the current subtitle since this can generate a subtitle (260201)
	};
};

boolean Talk_Balloon::dont_wait_to_say_more() {
#if TT_TEXT_TO_SPEECH
	// moved to be earlier than finish_instantly on 110700 since speech_has_finished caused problems
   if (text_to_speech()) {
      // could make this conditional on speaking now and return right thing but nothing cares -=
      stop_speaking();
//      say_id = -1; // so when it really finishes it doesn't do anything
//      speech_has_finished(TRUE); // commented out on 110700
		// and added the following
		speech_text_offset += speech_text_offset_increment;
		speech_text_offset_increment = 0;
		speech_should_wait_for_first_display = TRUE;
   };
#endif
   text_removal_time = 0;
	if (waiting_to_show_more) {
		// instantly finish off old
		finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
		return(TRUE);
	};
	return(FALSE);
};

string Talk_Balloon::currently_saying() {
   if (still_talking()) {
		// rewrote the following to use last_space_or_punctuation since it is more general
		if (tt_use_spoken_alternative) {
			if (alternative_spoken_text == NULL) return(NULL);
			int offset = last_space_or_punctuation(alternative_spoken_text,
																speech_text_offset+speech_text_offset_increment,FALSE);
			if (offset < 0) {
				return(alternative_spoken_text);
			} else {
				return(alternative_spoken_text+offset+1);
			};
//			while (offset > 1) {
//				if (alternative_spoken_text[offset] == ' ' &&
//					 (alternative_spoken_text[offset-1] == '.' || 
//					  alternative_spoken_text[offset-1] == '?' || 
//					  alternative_spoken_text[offset-1] == '!')) {
//					return(alternative_spoken_text+offset+1);
//				};
//				offset--; // go back to last sentence break
//			};
//			return(alternative_spoken_text);
		} else {
			int offset;
			if (need_wide_characters()) { // new on 041199
				wide_string wide_text = copy_wide_string(text,text_offset);
				if (contains_non_ascii(wide_text)) {
					offset = last_space_or_punctuation(wide_text,wide_string_length(wide_text),FALSE);
				} else {
					offset = last_space_or_punctuation(text,text_offset,FALSE);
				};
				if (offset >= 0) {
					offset = multi_byte_length(wide_text,offset+1); // new on 270100
				};
				delete [] wide_text;
			} else {
				offset = last_space_or_punctuation(text,text_offset,FALSE);
				if (offset >= 0) offset++; // to include space or punctuation
			};
			if (offset < 0) {
				return(text);
			} else {
				return(text+offset); // removed +1 on 270100 since computed above now
			};
//			while (offset > 1) {
//				if (text[offset] == ' ' &&
//					 (text[offset-1] == '.' || text[offset-1] == '?' || text[offset-1] == '!')) {
//					return(text+offset+1);
//				};
//				offset--; // go back to last sentence break
//			};
//			return(text);
		};
   } else {
      return(NULL);
   };
};

boolean Talk_Balloon::still_talking() {
//   return(text != NULL && (text_offset < length || (speech_text_offset_increment != 0 && speech_text_offset < speech_text_length)));
	// re-written on 080301
	if (text == NULL) return(FALSE);
	if (tt_marty_talk == MARTY_TALK_BALLOONS || tt_programmer->kind_of() == PROGRAMMER_TITLES) { // disjunct added on 080301 after beta 27b
		return(text_offset < length);
	} else if (tt_marty_talk == MARTY_SPEAKS_AND_TALK_BALLOONS) {
		return(text_offset < length || (speech_text_offset_increment != 0 && speech_text_offset < speech_text_length));
	} else if (tt_marty_talk == MARTY_SPEAKS) {
		return(speech_text_offset_increment != 0 && speech_text_offset < speech_text_length);
	} else if (tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES || tt_marty_talk == MARTY_WITH_SUBTITLES) {
		return(tt_screen->displaying_subtitle(TRUE) || text_offset < length); // added disjunct on 050401
	} else { // possible??
		return(FALSE);
	};		
};

#if TT_TEXT_TO_SPEECH
boolean Talk_Balloon::speech_has_finished(boolean not_old_topic) {
	// made boolean on 290301 so it returns TRUE if there is nothing more to say
	speech_should_wait_for_first_display = TRUE; // new on 040500
   // shouldn't not_old_topic be called old_topic ??
   if (say_id == -1 && not_old_topic) {
      say_id = 1; // start over again
      return(TRUE); // already taken care of by dont_wait_to_say_more
   };
	if (start_of_current_sentence_spoken > 0) { // not the very first time
		// using timeGetTime() might be better but then might not work so well with logs
		duration_of_last_sentence_spoken = tt_current_time-start_of_current_sentence_spoken;
	};
#if TT_DEBUG_ON
   if ((tt_debug_mode == 44444 || tt_debug_mode == 55555 || tt_debug_mode == 60998 || tt_debug_mode == 31203) && text_to_speech()) {
      tt_error_file() << "Speech just finished at " << timeGetTime() << " frame= " << tt_frame_number << endl << " ";
		if (alternative_spoken_text != NULL) {
			tt_error_file().write(alternative_spoken_text+speech_text_offset,speech_text_offset_increment);
		} else {
			tt_error_file().write(text+speech_text_offset,speech_text_offset_increment);
		};
      tt_error_file() << endl;
		tt_error_file() << duration_of_last_sentence_spoken << " is the duration of the last sentence." << endl;
		tt_error_file() << characters_in_last_sentence_spoken << " is the length of the sentence." << endl;
   };
#endif
//   millisecond wait, wait_per_digit;
//   switch (tt_language) {
//      SWEDISH:
//         wait = 120*speech_text_offset_increment;
//         wait_per_digit = 1000;
//         break;
//      default:
//         wait = 70*speech_text_offset_increment;
//         wait_per_digit = 750;
//   };
//   for (int i = 0; i < speech_text_offset_increment; i++) {
//      if (text[speech_text_offset+i] >= '0' && text[speech_text_offset+i] <= '9') wait += wait_per_digit; // number take longer to say
//   };
//   tt_error_file() << "At " << timeGetTime() << " finished saying ";
//   tt_error_file().write(text+speech_text_offset,speech_text_offset_increment);
//   tt_error_file() << ".  Will place rest of text after " << wait << "ms." << endl;
   speech_text_offset += speech_text_offset_increment;
   boolean done;
//	if (alternative_language_spoken()) {
//		done = (speech_text_offset == alternative_spoken_length);
//	} else {
		done = (speech_text_offset == speech_text_length); // prior to 070401 was length
//	};
	boolean messages_pending = tt_martian->is_message_pending();
#if TT_BUG_FIXES
	boolean new_stuff_to_say = FALSE; // new on 031203
#endif
   if (not_old_topic && !alternative_language_spoken()) {
#if TT_BUG_FIXES
		string text_copy = copy_string(text);
#endif
		place_rest_of_text(); //do_after(wait,this,place_rest_of_text_callback);
#if TT_BUG_FIXES
		if (text == NULL || strcmp(text_copy,text) != 0) { // something new was added by call to place_rest_of_text
			new_stuff_to_say = TRUE; // new on 031203 so that don't quit if just found something more to say -- a problem with subtitles and speaking
		};
		delete [] text_copy;
#endif
	};
//   tt_error_file() << speech_text_offset << " speech_text_offset" << endl;
//   if (remove_text_when_speech_done || speech_text_offset == length) {
//      remove_text(TRUE);
//      remove_text_when_speech_done = FALSE;
//      return;
//   };
//   speech_text_offset_increment = 0;
//   do_after(wait,this,speak_next_sentence_callback);
   if (done) {
#if TT_SOUND_OFF_ONLY_WHEN_SPEAKING
      if (!messages_pending) {
			turn_on_sound_and_speech_off(2000); // wait 2 seconds in case saying "good bye"...
		};
#endif
//		if (alternative_language_spoken()) {
//			speech_text_offset = 0;
//			speech_text_offset_increment = 0;
//			speech_text_length = 0;
//		};
		if (//tt_closed_caption_alternative_language || 
			 // -- commented out on 160701 since caused subtitles to sometimes stop too soon - e.g. Japanese/English on puzzle 1 intro
			 tt_closed_caption) { // new on 070401
			if (!new_stuff_to_say) { // otherwise subtitles can go away while speaking -new 031203
				tt_screen->subtitle_need_not_wait_for_speech(); // new on 281004 -- moved into conditional on 080705
				tt_screen->remove_current_subtitle(); // was remove_subtitle prior to 111104 but that one didn't invalidate the cache
			};
#if TT_DEBUG_ON
			if (tt_debug_mode == 31203) {
				tt_error_file() << "remove_current_subtitle() at " << timeGetTime() << endl;
			};
#endif
		} else if (tt_closed_caption_alternative_language) {
			next_subtitle_for_closed_caption_alternative_language(); // new on 160701 what to return?
		};
		return(TRUE); // new on 290301
   } else {
      speak_next_sentence();
		return(FALSE);
   };
};
#endif

void Talk_Balloon::place_rest_of_text() {
#if TT_DEBUG_ON
	if (tt_debug_mode == 55555) {
		log("place_rest_of_text called.");
	};
#endif
#if TT_TEXT_TO_SPEECH
   if (tt_marty_talk == MARTY_SPEAKS_AND_TALK_BALLOONS &&
       tt_programmer->kind_of() != PROGRAMMER_TITLES &&
		 !alternative_language_spoken() &&
       speech_text_offset < speech_text_length &&
//		 speech_text_offset_increment <= text_offset_increment && //added on 101699
		 speech_text_offset <= text_offset && // removed _increment from above on 221199
//		 speech_text_offset+speech_text_offset_increment <= text_offset+text_offset_increment && // new on 040500 since may be speaking a longer sentence than being displayed
       (speech_text_offset == 0 || // new topic has come up since starting to say this sentence
        speech_text_offset < text_offset+text_offset_increment)) {
#if TT_DEBUG_ON
      if (tt_debug_mode == 55555) {
         tt_error_file() << "At " << timeGetTime() << " ";
         tt_error_file() << "no balloon change, speech_text_offset is " << speech_text_offset
                     << " while text offset is " << (text_offset+text_offset_increment) << endl;
      };
#endif
      return; // speech is behind so ignore and wait
   } else if ((tt_marty_talk == MARTY_SPEAKS || tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES || tt_marty_talk == MARTY_WITH_SUBTITLES) && 
		        speech_text_offset >= speech_text_length && !alternative_language_spoken()) {
		// speech_text_length was just length prior to 030999
      remove_text();  // to remove the text so still_talking knows the truth
      return;
   };
#endif
	if (!alternative_language_spoken() && speech_text_offset_increment > 0) {
		if (speech_text_offset >= text_offset+text_offset_increment) { // new on 200400
			text_offset = speech_text_offset; 
#if TT_DEBUG_ON
			if (tt_debug_mode == 44444) {
				tt_error_file() << "Balloon beginning to show rest at " << timeGetTime() << " frame = " << tt_frame_number << endl;
				if (text+text_offset != NULL) tt_error_file() << (text+text_offset) << endl;
			};
#endif
		};
	} else if (text_offset+text_offset_increment < length) {
		text_offset += text_offset_increment;
		text_offset_increment = 0;
		// following commented out on 221199 since otherwise some of the text is skipped over
//#if TT_TEXT_TO_SPEECH
//      if (tt_marty_talk == MARTY_SPEAKS_AND_TALK_BALLOONS &&
//          tt_programmer->kind_of() != PROGRAMMER_TITLES &&
//          text_offset < speech_text_offset) { // text fell behind -- should only happen if sentence was too long for bubble
//         if (!alternative_language_spoken()) { 
//				text_offset = speech_text_offset;
//			}; 
//      };
//#endif
	   waiting_to_show_more = FALSE;
	   compute_size(length-text_offset,martian);
#if TT_DEBUG_ON
      if (tt_debug_mode == 55555) {
         tt_error_file() << "At " << timeGetTime() << " placing more text. Frame = " << tt_frame_number << endl;
			tt_error_file().write(text+text_offset,length-text_offset);
         tt_error_file() << endl << text_offset << " new text_offset" << endl;
      } else if (tt_debug_mode == 5050) {
         tt_error_file() << "At " << timeGetTime() << " Marty says ";
			tt_error_file().write(text+text_offset,length-text_offset);
         tt_error_file() << endl;
      };
#endif
   } else if (tt_marty_talk != MARTY_SPEAKS &&
				  tt_marty_talk != MARTY_SPEAKS_WITH_SUBTITLES && // added on 101199
				  tt_marty_talk != MARTY_WITH_SUBTITLES &&
				  (!alternative_language_spoken() || 
				   tt_help_speed_percent == 0 || // added on 200800 for when there is an alternative language but speech is off???
				   speech_text_offset+speech_text_offset_increment >= speech_text_length)) {
		// if only speaking this sometimes causes him to repeat himself (depends upon timing)
#if TT_DEBUG_ON
      if (tt_debug_mode == 55555) {
         tt_error_file() << "At " << timeGetTime() << " removing text." << endl;
      };
#endif
      remove_text();
   };
};

boolean martian_active() {
	return(tt_martian != NULL && tt_martian->active());
};

#if TT_CALL_FOR_MARTY

void remove_call_for_marty(boolean remove_text) {
	if (tt_call_for_marty != NULL) {
		tt_call_for_marty->destroy();
		tt_call_for_marty = NULL;
		tt_screen->screen_dirty(); // since image of call_for_marty may have been cached
		if (tt_martian != NULL && remove_text) {
			// second conjunct new on 161105 since if called other than by callback this shouldn't do this
			tt_martian->remove_text(); // new on 081105 so old stuff doesn't show up when finally called
		};
	};
};

void update_call_for_marty() {
	if (tt_call_for_marty != NULL) {
		if (tt_martian != NULL && !tt_martian->is_message_pending()) { // nothing pending to say
			remove_call_for_marty(TRUE);
			return;
		};
		TTRegion *screen_region = tt_screen->viewing_region();
		tt_call_for_marty->set_width_maintain_proportionality(tt_screen->viewing_width()/10);
		tt_call_for_marty->move_to(screen_region->min_x,screen_region->min_y);
		if (!tt_finish_all_animations) { // don't update if finishing instantly (e.g. scene changing)
			tt_call_for_marty->update_display(); // in case animating a timer so it goes away
		};
	};
};
#endif


//boolean Talk_Balloon::full_size() {
	// within 2 percentage points
//	int current_percent = get_size_percent();
//	return(current_percent-2 <= full_size_percent &&
//			 current_percent+2 >= full_size_percent);
//};

//void Talk_Balloon::text_region(city_coordinate &new_width,
//										 city_coordinate &new_height,
//										 city_coordinate &new_llx,
//										 city_coordinate &new_lly) {
//};	

/*		 
	city_coordinate center_x,center_y;
	programmer_appearance->center_location(center_x,center_y);
	const city_coordinate center_screen_x = 10*tile_width;
	city_coordinate delta_x = 0;
	if (center_y < 4*tile_height || center_x < 0 || center_x > ideal_screen_width) {
		// off screen or nearly so
		if (llx+tile_width < favorite_floor_x) {
			delta_x = tile_width;
		} else if (llx-tile_width > favorite_floor_x) {	
			delta_x = -tile_width;
		};
	} else {
		if (center_x < center_screen_x && llx < center_screen_x) {
			// on left but so is programmer
			if (llx+width < center_x) { // but is more left than programmer
				delta_x = -tile_width;
			} else {
				delta_x = tile_width;
			};
		} else if (center_x > center_screen_x && llx+width > center_screen_x) {
			// on right but so is programmer
			if (llx > center_x) {
				delta_x = tile_width;
			} else {
				delta_x = -tile_width;
			};
		};
	};
	boolean colliding = FALSE;
			colliding = TRUE;
			city_coordinate balloon_center_x,balloon_center_y;
			balloon_region.region_center(balloon_center_x,balloon_center_y);
			if (balloon_center_x < center_x) { // balloon already left of hand
				delta_x = -2*tile_width;
			} else {
				delta_x = 2*tile_width;				
			};
		} else {
			return; // delta is still zero
		};
	};
	city_coordinate balloon_llx,balloon_lly, balloon_urx;
	balloon->lower_left_corner(balloon_llx,balloon_lly);
	balloon_llx += delta_x;
	balloon_urx = balloon_llx+balloon->true_width();
	if (balloon_llx < -2*tile_width && !colliding) {
		if (center_x > center_screen_x) {
			// skoot back on screen
			move_to(2*tile_width,lly);
			return;
		} else if (center_x < 8*tile_width) {  
			// rather than go off the screen jump to other side
			delta_x = 22*tile_width-(balloon_urx-delta_x);
		};
	} else if (balloon_urx > 22*tile_width && !colliding) {
		if (center_x < center_screen_x) {
			move_to(14*tile_width,lly);
			return;
		} else if (center_x > 14*tile_width) {
			delta_x = -(balloon_llx-delta_x);
		};
	};
//		if (delta_x > 0 && llx+delta_x > ideal_screen_width) {
//			delta_x = ideal_screen_width-(llx-tile_width/2);
//		} else if (delta_x < 0 && llx+width/2+delta_x < 0) {
//			delta_x = -(llx+width/2);
//		};
	move_by(delta_x,0);
	balloon->move_by(delta_x,0);
	if (colliding) {
		// check if still is
		balloon->full_region(balloon_region);
		if (balloon_region.overlaps(programmer_region)) {
			city_coordinate balloon_center_x,balloon_center_y;
			balloon_region.region_center(balloon_center_x,balloon_center_y);
			if (balloon_center_x < center_x) { // balloon already left of hand
				delta_x = -2*tile_width;
			} else {
				delta_x = 2*tile_width;				
			};
		};
		move_by(delta_x,0);
		balloon->move_by(delta_x,0);
	};
};

*/
