// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved.

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
#include "utils.h" 
#endif
#if !defined(__TT_COMMON_H)
#include "common.h" 
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
#if !defined(__TT_WINMAIN_H)
#include "winmain.h" // for simulate_mouse_up
#endif
#if TT_XML
#if !defined(__TT_XML_H)
#include "xml.h"
#endif
#endif

#if !TT_32
#include <mmsystem.h> // for timeGetTime
#endif

#include <stdlib.h> // for min, max
#include <string.h>
////#include <strstrea.h> commented out 210703 // commented out on 210703

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//void set_future_text_callback(void *t) {
//  ((Text *) t)->set_future_text();
//};

//void update_sizes_unconstrained_callback(void *t) {
//	((Text *) t)->update_sizes_unconstrained();
//};

//void update_sizes_unconstrained_with_character_size_fixed_callback(void *t) {
//	((Text *) t)->update_sizes_unconstrained(TRUE);
//};

//typedef Sprite *SpritePointer;

Text::Text(city_coordinate center_x, city_coordinate center_y,
			  wide_string initial_text,
			  long initial_text_length,
			  long initial_priority,
//			  Background *floor,
			  city_coordinate initial_width,
			  city_coordinate initial_height,
//			  boolean ok_to_reuse_initial_text,
           color_index text_color) :
	Sprite(NUMBER_PAD_SPRITE,center_x,center_y,TEXT,
			 0,ground_scale, // should default these
			 initial_width,initial_height),
//	future_value(0),
//	mouse(NULL),
	text_length(initial_text_length), // removed (unsigned short int)  on 161203
//	future_text(NULL),
	saved_right_side(FALSE),
#if TT_WIDE_TEXT
	narrow_text(NULL),
#endif
   text_color(text_color),
   default_plate_state(TEXT_PAD_WITH_PLATE),
   use_variable_width_font_flag(FALSE),
//	fixed_width(0),
//	fixed_height(0),
//#if TT_DEBUG_ON
//	wide_text(initial_text), // needed for some debugging code - 300301
//#endif
//	character_height(-1), // new on 030900 to indicate that it has yet to be initialized -- commented out on 220102 to replace with some initial values just in case
	character_width(300),
	character_height(500), 
	change_size_to_fit_extent_flag(FALSE), // new on 070102 to decide whether to change the font size or the pad size
	change_font_size_to_fit_extent_flag(TRUE),  // new on 080102
	insertion_point(-1) { // not edited so can't have an insertion point
//#if TT_DEBUG_ON
//		if (text_color != 0) {
//			log("debug this");
//		};
//#endif
	 current_priority = initial_priority;
//	 set_priority_function_of_lly(FALSE);
	 set_size_is_true(TRUE); // width and height are its true size
	 set_parameter(TEXT_PAD_WITHOUT_PLATE); // only used if blank
	 if (initial_text == NULL) {
		 text_length = 0;
		 wide_text = NULL;
		 number_of_lines = 1; // until initialized
		 longest_line = 0; // added 070799 - until initialized
	 } else {
		 if (initial_text_length < 0) {
			 text_length = wide_string_length(initial_text); // removed (unsigned short) on 161203
		 };
	//	 text_max_length = text_length; // might grow or shrink
	//    if (ok_to_reuse_initial_text) {
	//		 text = initial_text;
	// commented out on 090499 since there is a difference between "" and NULL and at least robot names care
	//    } else if (text_length == 0) {
	//		 text = NULL;
	//	 } else {
			 wide_text = new wide_character[text_length+1]; //was text_max_length
			 memcpy(wide_text,initial_text,text_length*sizeof(wide_character));
			 wide_text[text_length] = NULL; // be sure it is terminated
	//	 };
		 compute_number_of_lines(); // TRUE added on 060102 since size about to be updated anyway
//		 character_width = width/longest_line;
//		 character_height = height/number_of_lines;
	//    width = character_width*longest_line; // fix round-off inconsistencies
	//    height = character_height*number_of_lines;
//		 update_sizes(TRUE,FALSE); // why TRUE? // made obsolete on 080102
	 };
	 if (tt_log_used_wide_characters && replaying()) { // new on 170300 -- simplified on 240803
		 set_use_variable_width_font(TRUE);
	 };
//	 } else if (!tt_new_pad_look && need_wide_characters()) { // commented out on 040102
		 // too hard to make fixed width things look nice with Unicode font -- but OK if use text pad look (as of 211201)
//		 set_display_just_nice_text(TRUE);
//	 };
//	 set_size_constrained(FALSE); // can grow by default
};

void Text::release_all_references() { // renamed on 130504
	if (releasing_all_references) { // new on 230305
		return;
	};
#if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {
		log("text debug target preparing for deletion");
	};
#endif
	if (wide_text != NULL) {
		delete [] wide_text;
		wide_text = NULL;
	};
#if TT_WIDE_TEXT
	if (narrow_text != NULL) {
		delete [] narrow_text;
		narrow_text = NULL; // new on 130504
	};
#endif
//	if (future_text != NULL) delete [] future_text;
	Sprite::release_all_references();
};

Sprite *Text::copy(boolean also_copy_offsets) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Copying text debug target");
	};
#endif
//	city_coordinate center_x, center_y;
//	center_location(center_x, center_y);
	Text *copy = new Text(llx,lly, //center_x,center_y,
								 wide_text,text_length, // text_length may be wrong if wide_text differs
								 current_priority-1,
								 width,height,text_color);
   finish_making_copy(copy,also_copy_offsets);
	return(copy);
};

void Text::finish_making_copy(Text *copy, boolean also_copy_offsets, boolean copy_followers_too) {
	// following was part of ::copy prior to 121104
	//if (blank && !tt_copy_restores_pre_blank) {
	//	copy->become_blank();
	//};
   copy->set_use_variable_width_font(use_variable_width_font());
	//copy->set_show_all(show_all());
	if (!tt_copying_entire_thought_bubble) { // condition new on 110405
		copy->set_plate_state(default_plate_state);
		//if (default_plate_state == TEXT_PAD_WITH_PLATE_ANIMATED) { remodularized on 130405
		//	copy->set_never_cache(TRUE); // new on 020302
		//};
	};
	copy->set_character_width(character_width); // new on 200903 otherwise can become very small if erased and coerced before any other changes
	copy->set_character_height(character_height);
	if (!animation_in_progress()) { // condition new on 060504 since otherwise text can stick out beyond pad
		copy->set_change_font_size_to_fit_extent(FALSE); // new on 121203 since shouldn't recompute this since set from copy
	};
//	if (saved_width != 0) {
//		copy->set_saved_size(saved_width,saved_height);
//	};
 //  copy->set_priority_fixed(priority_fixed_flag);
	//if (comment != NULL) {
	//	copy->set_comment(comment->copy());
	//};
	Sprite::finish_making_copy(copy,also_copy_offsets,copy_followers_too);
};

// An alternative to the following which implements up and down arrow in terms of character positions is
// to use pixel positions. GetCharacterPlacement seems like a good API to build upon

int characters_back_to_new_line(int index, wide_string text, int text_length) {
	int count = 0;
	for (int i = index-1; i >= 0; i--) {
		if (new_line_wide(text[i])) {
			return(count);
		};
		count++;
	};
	return(index);
};

int characters_forward_to_new_line(int index, wide_string text, int text_length) {
	int count = 0;
	for (int i = index; i < text_length; i++) {
		if (new_line_wide(text[i])) {
			return(count);
		};
		count++;
	};
	return(count);
};

int move_insertion_point_up(int insertion_point, wide_string text, int text_length) { // new on 100604
	int current_position = characters_back_to_new_line(insertion_point,text,text_length);
	if (current_position == insertion_point) { // already on the top line
		play_sound(PLOP_SOUND);
		return(insertion_point); // no change
	};
	int end_of_previous_line = insertion_point-(current_position+1);
	int new_line_characters = 0;
	if (new_line_wide(text[end_of_previous_line])) {
		new_line_characters++;
		if (text[end_of_previous_line-1] == '\n' && text[end_of_previous_line] != text[end_of_previous_line-1]) {
			new_line_characters++;
			end_of_previous_line--;
		};
	};
	int length_of_previous_line = characters_back_to_new_line(end_of_previous_line,text,text_length);
	if (length_of_previous_line >= current_position) {
		return(end_of_previous_line+current_position-length_of_previous_line);
	} else {
		return(end_of_previous_line);
	};
};

int move_insertion_point_down(int insertion_point, wide_string text, int text_length) { // new on 100604
	int current_position = characters_back_to_new_line(insertion_point,text,text_length);
	int distance_to_end_of_current_line = characters_forward_to_new_line(insertion_point,text,text_length);
	int start_of_next_line = insertion_point+distance_to_end_of_current_line+1;
	if (start_of_next_line >= text_length) { // this is the last line
		play_sound(PLOP_SOUND);
		return(insertion_point); // no change
	};
	int new_line_characters = 0;
	if (text[start_of_next_line] == '\n') {
		new_line_characters++;
		start_of_next_line++;
	};
	int length_of_next_line = characters_forward_to_new_line(start_of_next_line,text,text_length);
	if (length_of_next_line >= current_position) {
		return(start_of_next_line+current_position);
	} else {
		return(start_of_next_line+length_of_next_line); // end of the next line since it is too short
	};
};

Sprite *Text::respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected, Sprite *by,
										    boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless) {
	// changed key on 310899 to be unsigned so input of extra characters (e.g. Swedish E works OK
//	if (infinite_stack_flag) return(this); // ignore keystrokes over stack - commented out on 080400 since handled by programmer
   if (!puzzle_oks_typing_to_text_pads(by)) return(this);
	if (animation_in_progress() && tt_log_version_number >= 37) { // commented out on 080702 -- restored 280204
		finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
		update_size(TRUE); // new on 280204 -- this is a better fix than commenting out 
		// and this way text is identical if typed before or after animation finished
	};
	if (key == toggle_infinite_stack && !extended) { // new on 080400 -- !extended added on 110604
		return(Sprite::respond_to_keyboard(key,extended,robot_selected,by,record_action,
				 original_recipient,size_constrained_regardless));
	};
	boolean update_wide_text = TRUE;
	if (extended && arrow_key(key) && tt_log_version_number >= 45) { 
		// new on 100604 to enable editing with arrow keys if held or selected
		// removed && (original_recipient->held() || original_recipient == tt_selection) on 100604
		if (insertion_point < 0) { // not yet initialized
			insertion_point = text_length;
		};
		switch (key) { 
			case VK_LEFT:
				insertion_point = max(0,insertion_point-1);
				break;
			case VK_RIGHT:
				insertion_point = min(text_length,insertion_point+1);
				break;
			case VK_UP:
				insertion_point = move_insertion_point_up(insertion_point,wide_text,text_length);
				break;
			case VK_DOWN:
				insertion_point = move_insertion_point_down(insertion_point,wide_text,text_length);
				break;
		};
		update_wide_text = FALSE; // no need
	 } else if (key == BACKSPACE || (extended && key == VK_DELETE)) { // supporting delete now as well
		if (blank) return(this); // new on 100604 to ignore rubout of an erased text pad
		if (text_length == 0 && key == BACKSPACE && tt_log_version_number < 68) {
			// new on 150105 to not consider this an error anymore
			by->abort_action_if_robot(IDS_ABORT_TEXT_ALREADY_EMPTY,NONE_GIVEN,TRUE);
			return(this);
      //} else if (text_length == 1) { 
			// new on 301002 to fix the font size before it becomes empty -- 
			// done below as of 020903 so looks similar to just carriage reruns
      //   update_size(TRUE);
      };
		if ((key == BACKSPACE && (insertion_point == 0 || 
			 (text_length == 0 && tt_log_version_number >= 79))) || // added text_length == 0 on 041006 (and 061006)
			 (key == VK_DELETE && (insertion_point == text_length || insertion_point < 0))) {
			play_sound(PLOP_SOUND); // new on 100604 -- not bothering to involve the robot
			by->completed(); // sort of
			return(this);
		};
		// new on 100604
		int delete_position;
		if (key == BACKSPACE) {
			delete_position = insertion_point-1;
			insertion_point--; 
		} else if (key == VK_DELETE) {
			delete_position = insertion_point;
		};
		if (delete_position >= 0) {
			for (int i = delete_position; i < text_length; i++) {
				wide_text[i] = wide_text[i+1];
			};
		};
  		text_length--;
		if (narrow_text != NULL) { // new on 060506 since cache is no longer valid
			// narrow_text[text_length] = '\0'; // probably would work and be slightly more efficient but following is safest
			delete [] narrow_text;
			narrow_text = NULL;
		};
		// text_length might be 0 if control-j typed and rubbed out
		if (wide_text[text_length] == '\n' && text_length > 0) text_length--;
		if (text_length == 0) { // updated 020903
         update_size(TRUE);
		};
      if (new_line_wide(wide_text[text_length]) && !held()) {
         lly += character_height; // doesn't jump down this way
      };
		user_did(TEXT,TEXT_LETTERS_REMOVED,by);
	} else if (extended) {
		if (!arrow_key(key)) { // already handled delete and rubout, right?
			if (by != NULL) {
				by->completed(); // new on 010904
			};
			return(this); // ignore it and don't record it -- new on 270804
		};
	} else { // condition new on 110604 -- was !extended prior to 270804
		if (blank) {
			// I wonder how wise all this is (to treat erased text pads as if they were un-erased 0-length pads) 
			// but it dates back at least to 2.39 (noted 180804)
			become_non_blank();
			if (wide_text != NULL) { // act as if it was the empty string - why??
				delete [] wide_text;
				wide_text = NULL;
				text_length = 0;
			};
		};
		if (new_line(key)) {
         if (key == '\r') {
            key = '\n'; // new way of cannonicalizing beginning on 090802 to match XML standard
         };
//			text_length++; // an extra for carriage return -- 
			// commented out on 090802 since no longer require 2 characters (XML pushed things this way)
			user_did(TEXT,TEXT_MADE_MULTIPLE_LINES,by);
         if (!held()) lly -= character_height; // doesn't jump up this way
		} else {
			user_did(TEXT,TEXT_LETTERS_ADDED,by);
		};
		text_length++;
		wide_string new_text = new wide_character[text_length+1];
		// rewrote the following on 100604 to support arrow key editing
		int new_character_index; 
		if (insertion_point >= 0) {
//			if (insertion_point > 0) { // commented out on 211004 since if prepending something to 0th position this screwed up
			if (wide_text != NULL) { // new condition on 211004
				memcpy(new_text,wide_text,insertion_point*sizeof(wide_character));
				// following moved into conditional on 240904
				for (int i = text_length-1; i >= insertion_point; i--) { // shift everything over
					new_text[i+1] = wide_text[i];
				};
			};
			new_character_index = insertion_point;
			insertion_point++;
		} else if (text_length > 1) { // condition new on 100604
			memcpy(new_text,wide_text,(text_length-1)*sizeof(wide_character)); // ok even if last arg => 0?
			new_character_index = text_length-1;
		} else {
			new_character_index = 0;
		};
		if (wide_text != NULL) delete [] wide_text;
#if TT_WIDE_TEXT
		if (narrow_text != NULL) {
			delete [] narrow_text;
			narrow_text = NULL;
		};
#endif
		wide_text = new_text;
//		wide_character wide_key = widen(key); // no difference
      // commented out the following on 090802
		//if (new_line(key)) {
		//	wide_text[text_length-2] = '\r'; // was key prior to 080802
		//	wide_text[text_length-1] = '\n'; // carriage return
		//} else {
			int byte_count; // re-written this way on 250800 so that single byte Japanese characters work OK
			char double_byte[2];
			if (tt_second_byte_of_double_byte_input != 0) { // new on 220800
				byte_count = 2;
				double_byte[1] = key;
				double_byte[0] = tt_second_byte_of_double_byte_input;
			} else {
	//			new_text[text_size] = key;
				byte_count = 1;
				double_byte[0] = key;
			};
			wide_string unicode_character = copy_wide_string(double_byte,byte_count);
			wide_text[new_character_index] = unicode_character[0]; // new_character_index was text_length-1 prior to 100604
#if TT_DEBUG_ON
			if (tt_debug_mode == 240800) {
				tt_error_file() << (byte) key << " and " << (byte) tt_second_byte_of_double_byte_input << " become " 
									 << (int) unicode_character[0] << endl;
			};
#endif
			delete [] unicode_character;
//		} else {
//		   wide_text[text_length-1] = key;
//		};
		wide_text[text_length] = NULL; // termination of string added on 220599
	};
	if (insertion_point < 0) {
		insertion_point = text_length; // new on 040704 so get cursor when doing any editing
	};
//	unsigned int old_number_of_lines = number_of_lines;
//	compute_number_of_lines();
//	if (leader == NULL || 
//		 leader->kind_of() == PROGRAMMER ||
//		 leader->kind_of() == ROBOT_IN_TRAINING ||
//		 leader->kind_of() == ROBOT) {
//		width = min(character_width*longest_line,max_sprite_width);
//		if (old_number_of_lines != number_of_lines) {
//			height = min((height*number_of_lines)/old_number_of_lines,
//							  max_sprite_height);
//		};
//	};
	if (update_wide_text) { // condition new on 100604
		set_wide_text(wide_text,text_length,TRUE,size_constrained_regardless); // size_constrained_regardless new on 281002
	};
//	update_sizes(FALSE);
//	set_clean_status(DIRTY);
//	if (record_action) record_event(KEYBOARD_CHARACTER,by,floor,this,TRUE,key);
//	completed(action_status);
// commented out the above for the following on 080400
   return(Sprite::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,
												  size_constrained_regardless));
};

void Text::unselect() {
	insertion_point = -1;
	Sprite::unselect();
};

Sprite *Text::released(Sprite *by, boolean top_level, SpecialDrop special_drop) { // new on 080604
	// not clear about Number vs. Rational but only here do we have access to the narrow text
	insertion_point = -1;
	return(Sprite::released(by,top_level,special_drop));
};

void Text::generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection) {
	if (blank) {
		// could say more if done this before
		if ((contained_in(THOUGHT_BUBBLE) && !did(TEXT_BLANKED_INSIDE_THOUGHT_BUBBLE) &&
			 !did(TEXT_BLANK)) ||
			 tt_help_counter == 1) {
//			 if (tt_help_counter == 0) {
				 describe_pointing(message,tool_in_hand,selection);
//			 };
		};
		if ((!did(TEXT_BLANKED_INSIDE_THOUGHT_BUBBLE) && tt_help_counter == 0)
		    || tt_help_counter == 1) {
//			if (tt_help_counter == 0 && contained_in(THOUGHT_BUBBLE)) {
//				message << S(IDS_TEXT_BLANKED_TB_HELP);
//				user_did(TEXT,TEXT_BLANKED_INSIDE_THOUGHT_BUBBLE);
//			} else {
				message << S(IDS_TEXT_BLANKED_TB_HELP2)
						  << S(IDS_TEXT_BLANKED_TB_HELP3);
//			};
		} else if ((!did(TEXT_CONVERSION) &&	tt_help_counter == 0) 
					  || tt_help_counter == 2) {
			tt_help_history_action = TEXT_CONVERSION;
			message << S(IDS_CONVERT_TO_TEXT_HELP);
		};
		return;
	} else if ((tool_in_hand == NULL || tool_in_hand == this) &&
				  ((!did(TEXT_LETTERS_ADDED) && tt_help_counter == 0)
					|| tt_help_counter == 1)) {
		tt_help_history_action = TEXT_LETTERS_ADDED;
//		if (tt_help_counter == 0) {
			describe_pointing(message,tool_in_hand,selection);
//		};
		message << S(IDS_TEXT_LETTERS_ADDED_HELP);
		if (tool_in_hand == NULL) {
			message << S(IDS_TEXT_LETTERS_ADDED_HELP2);
			if (tt_language != BRAZILIAN_PORTUGUESE) { // condtion added 110700
				you_are(message,FALSE);
			};
			message << S(IDS_TEXT_LETTERS_ADDED_HELP3);
			you_are2(message);
		};
	} else if ((!did(TEXT_LETTERS_REMOVED) && tt_help_counter == 0)
				  || tt_help_counter == 2) {
		tt_help_history_action = TEXT_LETTERS_REMOVED;		  
		message << S(IDS_TEXT_LETTERS_REMOVED_HELP);
	} else if ((!did(TEXT_MADE_MULTIPLE_LINES) && tt_help_counter == 0)
				  || tt_help_counter == 3) {
		tt_help_history_action = TEXT_MADE_MULTIPLE_LINES;
		message << S(IDS_TEXT_MADE_MULTIPLE_LINES_HELP);
		message << S(IDS_BACKSPACE_TO_REMOVE_EMPTY_LINE);
	} else if ((!did(TEXT_CONCATENATION) && tt_help_counter == 0) 
				  || tt_help_counter == 4) {
		tt_help_history_action = TEXT_CONCATENATION;
		message << S(IDS_TEXT_CONCATENATION_HELP);
	} else if ((!did(TEXT_NUMBER_CONCATENATION) && tt_help_counter == 0)
				  || tt_help_counter == 5) {
		tt_help_history_action = TEXT_NUMBER_CONCATENATION;
		if (text_length > 1) { // what is wrong with length == 1??
         const int integer = 2;
			message << S(IDS_TEXT_NUMBER_CONCATENATION_HELP)
					  << S(IDS_TEXT_NUMBER_CONCATENATION_HELP2) << integer
					  << S(IDS_TEXT_NUMBER_CONCATENATION_HELP3);
         string text_copy = copy_narrow_string(wide_text,text_length);
         for (int i = 0; i < text_length; i++) {
            if (new_line(text_copy[i])) text_copy[i] = ' ';
         };
         text_copy[text_length-1] = next_in_alphabet(text_copy[text_length-1],integer); 
			message.write(text_copy,text_length);
         delete [] text_copy;
			message << S(IDS_TEXT_NUMBER_CONCATENATION_HELP4);
		} else {
			message << S(IDS_TEXT_NUMBER_CONCATENATION_HELP5)
					  << S(IDS_TEXT_NUMBER_CONCATENATION_HELP6); 
		};
//	 } else if ((!did(TEXT_SOUNDED) && tt_help_counter == 0 &&
//					tt_sound_option&SOUND_BIT)
//					|| tt_help_counter == 6) {
//		tt_help_history_action = TEXT_SOUNDED;
//		message << S(IDS_TEXT_SOUND_HELP1) << " ";
//		message << S(IDS_TEXT_SOUND_HELP2);
//		describe_right_button(message);
//		message << ". ";
//		message << S(IDS_SOUNDS_ON_PAGE8);
	 } else if ((!did(TYPE_HELP_GIVEN) && tt_help_counter == 0)
					|| tt_help_counter == 6) {
//		tt_help_history_action = TYPE_HELP_GIVEN;
		message << S(IDS_TEXT_HELP_GIVEN_HELP)
				  << S(IDS_TEXT_HELP_GIVEN_HELP2);
		no_help_interruptions();
		user_did(TEXT,TYPE_HELP_GIVEN);
	 };
};

void Text::describe_briefly(output_stream &message, Article article, 
									 boolean capitalize_first) {
	Sprite::describe_briefly(message,article,capitalize_first);
	if (!blank) {
		message << " \"";
		string narrow_text = return_narrow_text(); //copy_narrow_string(wide_text,text_length);
		message.write(narrow_text,text_length);
		message << "\"";
//		delete [] narrow_text;
	};
};

void Text::become_blank(boolean by_user, boolean controlee_too) {
	Sprite::become_blank(by_user,controlee_too);
	if (text_length == 0) { // empty text becoming blank
		width = width_given_height(height);
	};
	if (by_user) {
		user_did(TEXT,TEXT_BLANK);
		if (contained_in(THOUGHT_BUBBLE)) {
			user_did(TEXT,TEXT_BLANKED_INSIDE_THOUGHT_BUBBLE);
		};
	};
};

void Text::become_non_blank(boolean controlee_too) {
	if (blank) {
		Sprite::become_non_blank(controlee_too);
		if (text_length == 0) { // empty text becoming blank
			width = 4*tt_screen->one_pixel_in_x_city_units();
		};
	};
};

void Text::adjust_grasp(city_coordinate &adjustment_x,
								city_coordinate &adjustment_y) {
	adjustment_x = tile_width/-2; // so the rightmost character is visible
	adjustment_y = 0;
};

boolean Text::current_value(NumberValuePointer &the_value, boolean &shared, Sprite *by) {
	// convert to long if can
	if (blank) return(FALSE);
	if (text_length > 0) {
//		string temp = new character[text_length+1];
//		memcpy(temp,wide_text,text_length);
//		temp[text_length] = '\0'; // terminated version of text
		string temp = return_narrow_text(); // revised on 010899 since above only converted half of string
		long long_value;
		if (valid_operation_key(temp[0])) {
			long_value = _ttol(temp+1); // skip operation
		} else {
			long_value = _ttol(temp);
		};
//		the_value.set_value(long_value);
		the_value = new LongNumberValue(long_value);
		shared = FALSE;
//      delete [] temp;
		// return TRUE if non-zero or zero and string looks like "0"
		return(long_value != 0 || (text_length == 1 && wide_text[0] == '0'));
	};
	return(FALSE); // null string isn't zero??
};

NumberOperation Text::true_operation() {
	if (blank || text_length == 0) return(MAKE_EQUAL);
	if (valid_operation_key((char) wide_text[0])) return(operation_from_key((char) wide_text[0],TRUE)); // coerecion always OK here??
	return(MAKE_EQUAL);
};

Sprite *Text::select(TTRegion *, SelectionReason reason, Sprite *in_hand) {
	// identical to that of Number -- start sharing??
	if (reason == MIGHT_DROP_ITEM) {
		if (blank) {
			if (in_hand->can_be_text()) {
				return(this);
			} else {
				return(NULL);
			};
		} else if (in_hand->looks_like_is_of_type(TEXT) || in_hand->looks_like_is_of_type(INTEGER)) { 
			// rewritten on 140403 to deal with indirections to text and numbers -- forgot in_hand-> until 111104
			return(this);
		} else {
			return(NULL);
		};
//		switch (in_hand->kind_of()) {
//			case INTEGER:
////            if (!blank) return(NULL); -- commented out on 140403 since can drop numbers on text pads
//				return(this);
//			case TEXT:
//				return(this);
//			default:
//			  return(NULL);
//		};
	};
	return(this); 
};

//Sprite *Text::selected(SelectionReason reason, Sprite *by) { // made obsolete on 080102
//	Sprite *result = Sprite::selected(reason,by);
//	if (reason == GRASPED) {
		// size of letters may need to be recomputed
//      millisecond delay = animations_stop_time()-tt_current_time;
//		if (delay > 0) do_after(delay,this,update_sizes_unconstrained_callback);
//	};
//	return(result);
//};

/*
  // identical to that of Number -- start sharing??
//  boolean inside_thought_bubble = contained_in(THOUGHT_BUBBLE);
	if (animation_in_progress()) {
		finish_instantly();
	};
	if (reason == VACUUM_USED && !infinite_stack_flag &&
		 !contained_in(PROGRAM_PAD)) {
		record_event(VACUUM_APPLY,by,floor,this);
//		if (!blank) {
//			become_blank();
//			tt_toolbox_vacuum->suck_to_make_blank(this,by);
//			user_did(TEXT,TEXT_BLANK);
//			if (contained_in(THOUGHT_BUBBLE)) {
//				user_did(TEXT,TEXT_BLANKED_INSIDE_THOUGHT_BUBBLE);
//			};
//			return(NULL); // not to be vacuumed up
//		};
	};
	return(Sprite::selected(reason,by));
};
*/

void Text::set_plate_state(PlateState new_state) {
   default_plate_state = new_state;
	// following new on 130405 to make things more modular and consistent
	if (new_state == TEXT_PAD_WITH_PLATE_ANIMATED) {
		set_never_cache(TRUE);
	} else {
		set_never_cache(FALSE);
	};
};

ReleaseStatus Text::item_released_on_top(Sprite *item, Sprite *by, 
													  SpritePointer &recipient, int level) {
   if (to_be_smashed() || item->to_be_smashed()) { // e.g. another mouse is in progress // removed || future_text != NULL on 201202
      by->action_aborted();
      return(RELEASE_IGNORED);
   };
	boolean right_side = on_right_side(item);
	if (infinite_stack()) {
		if (!tt_copy_top_of_stack && !by->still_running()) {
			by->action_aborted();
			return(RELEASE_IGNORED);
		};
		if (!blank) {
			recipient = copy_top_of_stack(by);
			UnwindProtect<boolean> set(tt_recording_off,TRUE);
			recipient->item_released_on_top(item,by,recipient,level);
			if (right_side) {
				record_event(RELEASE_ITEM_RIGHT_OF,by,floor,this);
			} else {
				record_event(RELEASE_ITEM_LEFT_OF,by,floor,this);
			};
			return(RELEASE_HANDLED);
		} else {
			by->abort_action_if_robot(IDS_BLANK_TEXT_DIDNT_ACCEPT,
											  MOUSE_WITH_HAMMER,
											  !told_user(TOLD_TEXT_WOULDNT_ACCEPT));
			user_told(TOLD_TEXT_WOULDNT_ACCEPT);
			return(RELEASE_IGNORED);
		};
	};
	if (blank) {
//		if (item->kind_of() == INTEGER || item->kind_of() == TEXT) {
		if (receive_item(item,by,default_duration(),this,item)) { // conditionalized on 010799
			record_event(RELEASE_ITEM_ON,by,floor,this,TRUE); // this was commented out prior to 130499 but it is necessary robots trained to coerce a number to a string
			return(RELEASED_ON_TEXT); // on blank text
		} else {
			return(RELEASE_IGNORED);
		};
	} else {
		switch (item->kind_of()) {
			case TEXT:
				user_did(TEXT,TEXT_CONCATENATION,by);
				break;
			case INTEGER:
				user_did(TEXT,TEXT_NUMBER_CONCATENATION,by);
				if (!item->is_integer() || item->true_operation() != NO_NUMBER_OPERATION) { 
					// new on 050803 -- integers are the only numbers can be dropped on text pads
					return(RELEASE_IGNORED);
				};
				break;
// wrong place to check this
//			default:
//				return(RELEASE_IGNORED);
		};
		// left or right side depending upon relationship of horizontals centers of both
		return(add_to_side(right_side,item,by,default_duration(),this,item));
	};
};

boolean Text::receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item) {
   if (inside_vacuum()) {
      by->abort_action(IDS_ROBOT_MISSING_RECIPIENT_ABORT);
      return(FALSE);
   };
   if (!blank) { // only called if blank
//		by->abort_action_if_robot(action_status,"the text pad underneath should be blank",MOUSE_WITH_HAMMER);
		return(add_to_side(on_right_side(item),item,by,duration,original_recipient,original_item) != RELEASE_IGNORED);
	};
	if (item->is_blank()) {
		// why?? -- could just be a "no op"
		by->abort_action_if_robot(IDS_ITEM_ON_TOP_IS_BLANK,MOUSE_WITH_HAMMER);
		return(FALSE);
	};
	if (!item->can_be_text()) { // new on 201202
		return(TRUE);
	};
	// commented out on 201202 since better to do this in :set_to_future_value
//	wide_string new_text;
//	long new_text_length; // was unsigned short int prior to 090202
////	switch (item->kind_of()) { // commented out on 080900
////      case INTEGER:
////		case TEXT: {
////			string other_text;
//	      wide_string other_text; // made wide on 060102 since the wrong length was returned and lead to serious trouble
//			long other_text_length;
//			if (!item->current_wide_text(other_text,other_text_length)) {
//				return(TRUE);
//			};
//			new_text_length = other_text_length;
//			new_text = copy_wide_string(other_text);
//			blank = FALSE;
			if (item->kind_of() != TEXT) {
				user_did(TEXT,TEXT_CONVERSION);
			};
			Background *items_floor = item->pointer_to_background();
			// following was only in the duration > 0 branch prior to 031202
//			future_text = new_text;
//			future_text_length = new_text_length;
			if (duration > 0) {
				call_in_a_mouse(this,item,items_floor,by,FALSE,::ok_to_delete_bammed(by), // this arg was !blank prior to 131000
									 duration,original_recipient,original_item);
				return(TRUE);
			} else {
				item->set_to_be_smashed(TRUE); // so it acts like the above -- new on 121004
				set_to_future_value(item,original_recipient,by); // new on 060102 and replaces commented out lines below
//				set_wide_text(new_text,new_text_length,FALSE); //TRUE);
				if (items_floor != NULL) { // condition new on 021000 - was breaking the old Bambi game
					items_floor->remove_item(item,FALSE); // !blank); // !blank was TRUE prior to 031000 -- turned off warnings on 300802 since Poddy gives this warning when text is added to SpeechSound
				};
				if (::ok_to_delete_bammed(by)) { // was !blank FALSE prior to 131000 -- this is because might be old version that doesn't coerce - not sure this is still working right (170202)
					item->destroy(by);
				};
//				become_non_blank(); // prior to 031000 this was before the conditional above - but then it is always not blank (and floor isn't always quite right)
            by->completed(); // was added on 9/25/97
			};
//			break;
//		  };	
//	};
   return(TRUE); // I think
};

ReleaseStatus Text::add_to_side(boolean right_side, Sprite *item,
										  Sprite *by, millisecond duration,
                                Sprite *original_recipient, Sprite *original_item) {
	if (infinite_stack()) {
      ::action_aborted(by);
		return(RELEASE_IGNORED);
	};
	switch (item->kind_of()) {
		case INTEGER:
		case TEXT:
			break;
		case PICTURE: {
			// might be an indirection -- added conditions for that on 250703 - otherwise it falls thru
			Picture *picture = (Picture *) item;
			Sprite *indirection = picture->pointer_to_indirection();
			if (!item->is_flipped() && (indirection != NULL && indirection->kind_of() == TEXT)) break; 
			// conditional new on 170103 so dropping flipped things on text pads is handled right
						  }; // otherwise fall thru
		case BOMB:
		case CUBBY:
			by->abort_action_if_robot(IDS_ONLY_TEXT_OR_NUMBERS_ON_TEXT,
											  MOUSE_WITH_HAMMER,!told_user(TOLD_TEXT_WOULDNT_ACCEPT));
			user_told(TOLD_TEXT_WOULDNT_ACCEPT);
			return(RELEASE_IGNORED);
      case SOUND:
         if (kind_of() == SOUND) break; // OK if I am too
		default:
//			by->action_aborted(); // updated on 300704 to be informative
			by->abort_action_if_robot(IDS_ONLY_TEXT_OR_NUMBERS_ON_TEXT,
											  MOUSE_WITH_HAMMER,!told_user(TOLD_TEXT_WOULDNT_ACCEPT));
			user_told(TOLD_TEXT_WOULDNT_ACCEPT);
			return(RELEASE_IGNORED);
	};
//   wide_string new_text;
//   unsigned long new_text_length;
//   if (item->kind_of() == INTEGER) {
//      // used to just do like TEXT -- now used to increment ascii value
//      if (text_length == 0) { // can't do much
//         by->action_aborted();
//		   return(RELEASE_IGNORED);
//	   };
//      long value;
//      if (!item->current_long_value(value) || item->true_operation() != NO_NUMBER_OPERATION) {
//			// error handling is new as of 010302 - if blank or an operation should be rejected
//			by->abort_action_if_robot(IDS_ONLY_TEXT_OR_NUMBERS_ON_TEXT,
//										     MOUSE_WITH_HAMMER,!told_user(TOLD_TEXT_WOULDNT_ACCEPT));
//		   user_told(TOLD_TEXT_WOULDNT_ACCEPT);
//		   return(RELEASE_IGNORED);
//		};
//      new_text = copy_wide_string(wide_text,text_length);
//      new_text_length = text_length;
////      char new_char[2];
////      new_char[1] = NULL;
////      char translated_char[2];
//      if (right_side) {
//         new_text[text_length-1] = next_in_alphabet(new_text[text_length-1],value);
////         new_text[text_length-1] = (char) ((new_text[text_length-1]+value)%128);
////          new_char[0] = (char) ((new_text[text_length-1]+value)%128);
////          OemToAnsi(new_char,translated_char);
////          new_text[text_length-1] = translated_char[0];
//       } else {
//          new_text[0] = next_in_alphabet(new_text[0],value);
////          new_char[0] = (char) ((new_text[0]+value)%128);
////          OemToAnsi(new_char,translated_char);
////          new_text[0] = translated_char[0];
//       };
//   } else {
////	   string other_text;
//		wide_string other_wide_text; // new on 050102
//	   long other_text_length;
//		if (!item->current_wide_text(other_wide_text,other_text_length)) { // was just current_text prior to 050102 but that was wrong for Japanese
//		   by->abort_action_if_robot(IDS_ONLY_TEXT_OR_NUMBERS_ON_TEXT,
//										     MOUSE_WITH_HAMMER,!told_user(TOLD_TEXT_WOULDNT_ACCEPT));
//		   user_told(TOLD_TEXT_WOULDNT_ACCEPT);
//		   return(RELEASE_IGNORED);
//	   };
//	   new_text_length = ((unsigned long) other_text_length)+text_length;
//	   if (new_text_length > max_unsigned_short) {
//		   by->abort_action_if_robot(IDS_TEXT_TOO_LONG,MOUSE_WITH_HAMMER,TRUE);
//		   return(RELEASE_IGNORED);
//	   };
//      if (new_text_length == 0) {
//		   new_text = NULL;
//	   } else {
//		   new_text = new wide_character[new_text_length+1];
////			wide_string other_wide_text = copy_wide_string(other_text);
//		   if (right_side) {
//			   memcpy(new_text,wide_text,text_length*sizeof(wide_character));
//			   memcpy(new_text+text_length,other_wide_text,wide_string_length(other_wide_text)*sizeof(wide_character));
//		   } else {
//			   memcpy(new_text,other_wide_text,wide_string_length(other_wide_text)*sizeof(wide_character));
//			   memcpy(new_text+other_text_length,wide_text,text_length*sizeof(wide_character));
//		   };
//			new_text[new_text_length] = NULL; // terminatation added on 220599
//      };
//	};
	saved_right_side = right_side; // new on 201202
	Background *items_floor = item->pointer_to_background();
//	future_text = new_text; // prior to 210102 this and the following line with only in the first branch of the conditional below (probably broken on 060102)
//	future_text_length = (unsigned short int) new_text_length;
	if (duration > 0) {
		call_in_a_mouse(this,item,items_floor,by,FALSE,TRUE,duration,original_recipient,original_item);
	} else {
		set_to_future_value(item,original_recipient,by); // new on 060102 and replaces commented out lines below
//		set_wide_text(new_text,new_text_length,FALSE); //TRUE);
		if (items_floor != NULL) { // condition new on 100203 for robustness
			items_floor->remove_item(item,FALSE); // FALSE was TRUE prior to 171199 but that generated spurious warnings when running on the back of a picture
		};
		item->destroy(by);
		by->completed();
	};
	if (right_side) {
		record_event(RELEASE_ITEM_RIGHT_OF,by,floor,original_recipient); // this); updated 010305 since if indirection won't work
		return(RELEASED_ITEM_RIGHT_OF_ITEM);
	} else {
		record_event(RELEASE_ITEM_LEFT_OF,by,floor,original_recipient); // this);
		return(RELEASED_ITEM_LEFT_OF_ITEM);
	};
};

boolean Text::on_right_side(Sprite *item) {
	return(item->current_llx()+item->current_width()/2 > llx+width/2);
};

// used to be that dropping an integer on text selected nth
// turn into nth if can otherwise ignore
//			long n;
//			if (!((Integer *) item)->current_value(n)) return;
//			if (n > 0 && n <= text_length) {
//				new_text = new char[1];
//				new_text[0] = text[n-1];
//				new_text_length = 1;
//				if (leader != NULL) {
//					// saved size no longer appropriate
//					saved_width = 2*width_given_scale(y_scale);
//					saved_height = 2*height_given_scale(y_scale);
//				};
//				user_did(TEXT,TEXT_LETTER_SELECTED,by);
////				update_sizes(FALSE);
//			} else { // out of range index -- do nothing
//				if (duration > 0) { //showing_on_a_screen()) 
//					play_sound(PLOP_SOUND,5); 
//				};
//				if (n > 0) {
//					by->abort_action_if_robot(action_status,S(IDS_ABORT_TEXT_BAD_INDEX),MOUSE_WITH_HAMMER);
//				} else if (n == 0) {
//					by->abort_action_if_robot(action_status,"number is zero",MOUSE_WITH_HAMMER);
//				} else { 
//					by->abort_action_if_robot(action_status,"number is negative",MOUSE_WITH_HAMMER);
//				};
//				return;
//			};
//			break;
//		  };	

wide_string Text::compute_new_text(Sprite *other, long &new_text_length, Sprite *by) {
	wide_string new_text;
//	long new_text_length; // was unsigned short int prior to 090202
//	switch (item->kind_of()) { // commented out on 080900
//      case INTEGER:
//		case TEXT: {
//			string other_text;
	if (is_blank()) { // converting other to text
	 //  wide_string other_text; // made wide on 060102 since the wrong length was returned and lead to serious trouble
		//long other_text_length;
		if (!other->current_wide_text(new_text,new_text_length)) {
			return(NULL);
		};
//		new_text_length = other_text_length;
		return(copy_wide_string(new_text,new_text_length));
	};
	// else dropped on left or right side
   if (other->kind_of() == INTEGER) {
      // used to just do like TEXT -- now used to increment ascii value
      if (text_length == 0) { // can't do much
//			return(RELEASE_IGNORED); // used to get "error message" sooner (when dropped rather than when smashed)
		   return(NULL);
	   };
      long value;
      if (!other->current_long_value(value) || other->true_operation() != NO_NUMBER_OPERATION) {
			// error handling is new as of 010302 - if blank or an operation should be rejected -- note this doesn't work for bignums...
			if (by != NULL) { // condition new on 050803
				by->abort_action_if_robot(IDS_ONLY_TEXT_OR_NUMBERS_ON_TEXT,MOUSE_WITH_HAMMER,!told_user(TOLD_TEXT_WOULDNT_ACCEPT));
			};
		   user_told(TOLD_TEXT_WOULDNT_ACCEPT);
		   return(NULL); // RELEASE_IGNORED);
		};
      new_text = copy_wide_string(wide_text,text_length); // clobber my current string - really need to copy it and delete old??
      new_text_length = text_length;
//      char new_char[2];
//      new_char[1] = NULL;
//      char translated_char[2];
      if (saved_right_side) {
         new_text[text_length-1] = next_in_alphabet(new_text[text_length-1],value);
//         new_text[text_length-1] = (char) ((new_text[text_length-1]+value)%128);
//          new_char[0] = (char) ((new_text[text_length-1]+value)%128);
//          OemToAnsi(new_char,translated_char);
//          new_text[text_length-1] = translated_char[0];
       } else {
          new_text[0] = next_in_alphabet(new_text[0],value);
//          new_char[0] = (char) ((new_text[0]+value)%128);
//          OemToAnsi(new_char,translated_char);
//          new_text[0] = translated_char[0];
       };
		 return(new_text);
   } else {
//	   string other_text;
		wide_string other_wide_text; // new on 050102
	   long other_text_length;
		if (!other->current_wide_text(other_wide_text,other_text_length)) { // was just current_text prior to 050102 but that was wrong for Japanese
			if (by != NULL) { // conditional new since happened when dropping a picture on a text pad
				by->abort_action_if_robot(IDS_ONLY_TEXT_OR_NUMBERS_ON_TEXT,
												  MOUSE_WITH_HAMMER,!told_user(TOLD_TEXT_WOULDNT_ACCEPT));
				user_told(TOLD_TEXT_WOULDNT_ACCEPT);
			};
		   return(NULL); // RELEASE_IGNORED);
	   };
	   new_text_length = ((unsigned long) other_text_length)+text_length;
		if (new_text_length > (long) (compute_memory_available(FALSE)/10)) { // was max_unsigned_short prior to 161203 - now is 1/10th of available physical memory
		   by->abort_action_if_robot(IDS_TEXT_TOO_LONG,MOUSE_WITH_HAMMER,TRUE);
		   return(NULL); // RELEASE_IGNORED);
	   };
      if (new_text_length == 0) {
		   return(NULL);
	   } else {
		   new_text = new wide_character[new_text_length+1];
//			wide_string other_wide_text = copy_wide_string(other_text);
		   if (saved_right_side) {
			   memcpy(new_text,wide_text,text_length*sizeof(wide_character));
			   memcpy(new_text+text_length,other_wide_text,wide_string_length(other_wide_text)*sizeof(wide_character));
		   } else {
			   memcpy(new_text,other_wide_text,wide_string_length(other_wide_text)*sizeof(wide_character));
			   memcpy(new_text+other_text_length,wide_text,text_length*sizeof(wide_character));
		   };
			new_text[new_text_length] = NULL; // terminatation added on 220599
			return(new_text);
      };	
	};
};

void Text::set_to_future_value(Sprite *other, Sprite *original_recipient, Sprite *by) {
	if (tt_number_to_text_exact && other->to_be_smashed() && other->kind_of() == INTEGER) {
		// new on 111004
		// following updated since Excel only sort of accepts 2 1/3 (not when there are many digits !?!)
#if TT_SPEAK_ALL_NUMBERS
		Rational *other_number = (Rational *) other;
		flag the_integer_and_fraction_format, the_decimal_notation_if_denominator_is_power_of_ten, the_shrinking_decimal_notation;
		other_number->current_style(the_integer_and_fraction_format,the_decimal_notation_if_denominator_is_power_of_ten,
											 the_shrinking_decimal_notation);
//		if (!(!the_integer_and_fraction_format&&the_decimal_notation_if_denominator_is_power_of_ten&&!the_shrinking_decimal_notation)) {
		if (the_shrinking_decimal_notation) {
			// updated on 040705 since if in fractional format leaves it alone
			// isn't already in the state where it'll produce an exact string
			// no point doing this work if is an integer (and can be lots of work if it is very large) -- new on 081104
			bignum_pointer bignum;
			NumberValue *value = other_number->pointer_to_value();
			if (value != NULL) {
				if (!value->bignum_value(bignum)) {
               other_number->set_current_style(FALSE,TRUE,FALSE); 
					// for a few days this prefered 1 1/3 rather than 4/3 since that is more readable 
					// but screws up taking values to Excel - restored 091104
					// exact decimals are best when possible
					other_number->need_to_recompute_text(); // no point doing this work if it already was OK
				};
			};
		};
#else
		((Rational *) other)->set_current_style(FALSE,TRUE,FALSE); // new on 121004 -- was set_shrinking_decimal_notation(FALSE);
#endif
	};
	long new_text_length;
	wide_string new_text = compute_new_text(other,new_text_length,by);
	if (new_text == NULL) {
		if (by != NULL) { // can it be NULL?
			by->action_aborted();
		};
		return;
	};
	city_coordinate old_width = width;
	city_coordinate old_height = height;
	set_wide_text(new_text,new_text_length); // future_text); // ,future_text_length);
	become_non_blank();
//	future_text = NULL;
	if (original_recipient != NULL && reachable_from_remote_looks(original_recipient,this) && (old_width != width || old_height != height)) { 
		// new on 060102
		original_recipient->set_size(width,height);
	};
//	if (text_length == 1 && leader != NULL) { // selected nth character
//		set_center_location(smashee_center_x,smashee_center_y);
//	};
};

void Text::set_text(string new_text, int new_text_length,
						  boolean text_was_updated, boolean size_constrained_regardless) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Setting debug target's text");
	};
	add_to_log_if_interesting_error();
#endif
#if TT_WIDE_TEXT
//	new_text_length = strlen(new_text);
	wide_string wide_text;
	long destination_length;
	if (new_text == NULL) {
		// special case new on 300704 since otherwise give invalid args below
		wide_text = new wide_character[1];
		destination_length = 0;
	} else {
		destination_length = MultiByteToWideChar(tt_code_page[tt_use_spoken_alternative],0,new_text,new_text_length,NULL,0);
		wide_text = new wide_character[destination_length+1];
		MultiByteToWideChar(tt_code_page[tt_use_spoken_alternative],0,new_text,new_text_length,wide_text,destination_length);
	};
	wide_text[destination_length] = NULL; // just to be safe
	set_wide_text(wide_text,-1,FALSE,size_constrained_regardless); // restored 280104 since the following sometimes left a nonsense character at the end
//	set_wide_text(wide_text,destination_length,FALSE,size_constrained_regardless); // rewritten on 270104 to be simpler and faster
#else
	wide_text = new_text; // no difference for 16-bit code -- shouldn't this be copied?
	if (new_text_length < 0) {
		if (new_text != NULL) {
			new_text_length = strlen(new_text);
		} else {
			new_text_length = 0;
		};
	};
	//if (new_text_length > max_unsigned_short) { // this 64K limit should go for the next file format
	//	// new on 210201
	//	tt_error_file() << "Warning: a text pad was given text that is " << new_text_length << " characters long. 65,536 is the limit for this implementation of ToonTalk. All characters beyond 65,536 are cut off." << endl;
	//	new_text_length = max_unsigned_short;
	//} else {
		text_length = new_text_length; // removed (unsigned short) on 161203
	//};
	// following moved here on 290103 from after compute_number_of_lines(); since set_wide_text above does this better and more generally
	if (!(size_constrained_regardless || size_constrained())) { // new on 070102
		change_size_to_fit_extent_flag = TRUE;
	} else {
		change_font_size_to_fit_extent_flag = TRUE;
	};
#endif // moved here on 090102
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	compute_number_of_lines(); // size_constrained_regardless || size_constrained());
//	update_sizes(size_constrained_regardless); // arg was TRUE prior to 240100 // made obsolete on 080102
};

//void Text::append_wide_text(const wide_string new_text, int new_text_length) {
//   // new on 140802 to deal better with XML that needs to fit a certain width (e.g. for email)
//   if (new_text == NULL) return;
//   if (new_text_length < 0) new_text_length = wide_string_length(new_text);
//   if (text_length = 0) {
//      set_wide_text(copy_wide_string(new_text,new_text_length),new_text_length);
//   } else {
//      int both_length = text_length+new_text_length;
//      wide_string both = new wide_character[both_length+1];
//      wmemcpy(both,wide_text,text_length);
//      wmemcpy(both+text_length,new_text,new_text_length);
//      both[both_length] = NULL; // terminate string
//      set_wide_text(both,both_length);
//   };
//};

void Text::set_wide_text(wide_string new_text, int new_text_length,
								 boolean text_was_updated, boolean size_constrained_regardless) {
 #if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {
		log("text debug target set_wide_text");
	};
#endif
	boolean initialization = (wide_text == NULL);
	city_coordinate old_width = width;
	city_coordinate old_height = height;
   if (new_text == NULL) {
		text_length = 0; // e.g. multple mouse smashes
	} else if (new_text_length < 0) {
		text_length = wide_string_length(new_text);
	} else {
		text_length = new_text_length;
	};
	boolean size_is_constrained = (size_constrained_regardless || size_constrained());
	// commented out on 070604 so remote looks don't have text extending beyond its borders || is_picture()); // is_picture() new on 290103
	if (text_length == 0) {
//		text_length = 0;
		// to visually distinguish empty text from blank text
		if (!size_is_constrained) {
			width = 4*tt_screen->one_pixel_in_x_city_units();
		};
	} else {
		if (!text_was_updated && wide_text != NULL) delete [] wide_text;
      new_text[text_length] = NULL; // was '\0' prior to 270104 but this is a wide string // terminate string 
		wide_text = new_text;
#if TT_WIDE_TEXT
		if (narrow_text != NULL) {
			delete [] narrow_text;
			narrow_text = NULL;
		};
#endif
//		text_max_length = text_length;
		int old_number_of_lines = number_of_lines;
      // removed || is_picture() on 281002 since size_constrained_regardless is now used so if this is an indirection with a leader it won't change size  // is_picture() added on 090102 
		compute_number_of_lines(); // size_constrained);
		if (initialization) {
//			update_sizes(FALSE,FALSE); // was TRUE); // made obsolete on 080102
		} else if (!size_is_constrained) {  
			// try a character width appropriate for height but
			// if too long min will fix it
			if (old_number_of_lines != number_of_lines) {
				height = min((unsigned long) ((height*number_of_lines)/old_number_of_lines),
								 // cast for 32bit compiler's happiness
								 (unsigned long) max_sprite_height);
				scale_to_fit(); // to update y_scale for use below
			};
//			set_character_width(width_given_scale(y_scale)/number_of_lines); // commented out on 020102
//			width = min(character_width*longest_line,max_sprite_width);
//			update_sizes(size_constrained_regardless,TRUE); // FALSE was TRUE prior to 040102 but restored on 050102 // made obsolete on 080102
//			if (!text_was_updated) { // made obsolete on 080102
//				move_by((old_width-width)/2,(old_height-height)/2);
//			};
         if (tt_log_version_number < 64 && text_length == 1) { // commented out on 131204 since this caused a change in size when dropping a number on a single letter
            // special case introduced on 281002 since otherwise going from many to 0 and back again screws up
				// had been commented out and restored on 240904
            set_to_good_size();
         };
			if (change_font_size_to_fit_extent()) { //  new on 130104 so if typed before display computed new font size...
				update_size(TRUE); // do it now	
			};
			change_size_to_fit_extent_flag = TRUE;
         change_font_size_to_fit_extent_flag = FALSE; // new on 281002
			update_size(TRUE); // new on 281002 -- added TRUE (regardless) on 061003
			if (tt_log_version_number >= 58) {
				move_by((old_width-width)/2,(old_height-height)/2); // new on 071104
			};
//        };
		} else {
			change_font_size_to_fit_extent_flag = TRUE;
//         update_size(); // new on 281002 -- needed?? -- commented out as experiment on 150903
		};
	};
	set_clean_status(DIRTY);
};

void Text::signal_good_size() { // new on 080702
	if (tt_log_version_number < 61 && !is_blank()) { 
		// condition new on 180804 since otherwise erasing text pads from notebooks looks bad -- not clear this is needed anymore anyway
		// this breaks the idea that Tooly can be changed in size since the following good_size doesn't have a source -- removed 301104 (except old demos)
		set_to_good_size();
	};
	Sprite::signal_good_size();
};

void Text::set_size_and_location(city_coordinate new_width,
											city_coordinate new_height,
											city_coordinate new_llx,
											city_coordinate new_lly,
											ChangeReason reason) {
	boolean size_changed = (width != new_width || height != new_height);
//#if TT_DEBUG_ON
//	if (wide_text != NULL && wide_text[0] == 'Z' && width != new_width) {
//		tt_error_file() << "debug this" << endl;
//	};
//#endif
	Sprite::set_size_and_location(new_width,new_height,new_llx,new_lly,reason);
	if ((size_changed && reason != LOADING_GEOMETRY && reason != NEW_TEXT_EXTENT) || reason == ANIMATION_STARTING) {
      // || reason == ANIMATION_COMPLETED) { -- animation handled differently beginning 090102
		if (leader == NULL && !is_picture() && reason == NO_REASON_GIVEN) { // || reason == ANIMATION_COMPLETED)) { // simplified on 180102
//			 reason != LEADER_CHANGED && // leader might not yet be set...
//			 reason != TO_FIT_INSIDE && reason != SETTING_TO_GOOD_SIZE && reason != UPDATING && reason != ANIMATION_STARTING && 
//			 reason != EXPANDER_CHANGED_IT && reason != TRANSLATING_FLOOR_TO_ROOM_COORDINATES && reason != TRANSLATING_ROOM_TO_FLOOR_COORDINATES) { // these added on 160102
			change_size_to_fit_extent_flag = TRUE;
		} else {
			change_font_size_to_fit_extent_flag = TRUE;
		};
//		scale_to_fit(); // made obsolete on 080102
		// no need to call compute_number_of_lines(sizes_constrained);
//		update_sizes(TRUE,FALSE); // made obsolete on 080102
	};
	if (reason == LOADING_GEOMETRY && text_length == 0 && tt_log_version_number >= 64) { 
		// new on 131204 to deal with pasting 0-length text pads and making sure they have a reasonable font size
		set_character_height(height);
		set_character_width((9*height)/11); // seems to be the ratio if you have a single letter
	};
};

void Text::update_sizes(boolean sizes_constrained, boolean character_size_fixed) {
	if (change_size_to_fit_extent_flag || wide_text == NULL) return; // nothing to do and this avoid dividing by zero below
	// starting 060102 callers do need to call compute_number_of_lines to avoid calling it twice
//	compute_number_of_lines(sizes_constrained); // prior to 070799 was commented out since "is called elsewhere now" but it isn't always
	boolean by_leader = size_constrained(); // what if is_picture() - asked 290103
	if (sizes_constrained || by_leader || animation_in_progress()) {
		// size constrained by leader or on-going animation
		if (!character_size_fixed) {
			set_character_width(width/longest_line);
			set_character_height(height/number_of_lines);
		};
//         width = character_width*longest_line; // fix round-off inconsistencies
//         height = character_height*number_of_lines;
//		if (!animation_in_progress() && y_scale > 0) {
//			saved_width = (width_given_scale(y_scale)*longest_line)/number_of_lines;
//			if (saved_height == 0 || height == 0) {
//				saved_height = height*number_of_lines;
//			} else { // maintain old saved height and proportionality
//				saved_width = (saved_width*saved_height)/height;
//			};
//			if (saved_width > max_sprite_width) saved_width = max_sprite_width;
//		};
//#if TT_32 // at least for now
//		if (display_just_nice_text()) { // since variable width
//			character_width = tt_screen->correct_font_width((string) wide_text,text_length, 
//															 character_width, 
//															 character_height,
//															 TRUE); // bold by default
//			character_height = tt_screen->correct_font_height((string) wide_text,text_length, 
//															   character_width, 
//															   character_height,
//															   TRUE);
//		};
//#endif
	} else { 
//		if (!character_size_fixed) {
//			update_sizes_unconstrained(); // made obsolete on 080102
//		};
		// e.g. Robot holding the text
		if (leader != NULL) record_offsets();
	};
	if (!sizes_constrained && !by_leader && animation_in_progress()) {
		// only constrained by on-going animation then recompute when done
		millisecond stop_after = animations_stop_time()-tt_current_time;
		if (stop_after <= 0) stop_after = 1; // next frame
		animate_to_good_size(stop_after,tt_log_version_number >= 64?tt_toolbox:NULL);
//		if (character_size_fixed) {
//			do_after(stop_after,this,update_sizes_unconstrained_with_character_size_fixed_callback);
//		} else {
//			do_after(stop_after,this,update_sizes_unconstrained_callback);
//		};
	};
};

// following commented out on 240304 since obsolete 
//void Text::update_sizes_unconstrained() { // boolean character_size_fixed) { // added character_size_fixed on 020102 -- removed on 060102 since do nothing if TRUE
//   if (change_size_to_fit_extent_flag || infinite_stack()) return;
//	city_coordinate one_horizontal_pixel = tt_screen->one_pixel_in_x_city_units();
//	city_coordinate one_vertical_pixel = tt_screen->one_pixel_in_y_city_units();
//	city_coordinate new_character_width, new_character_height;
////	if (display_just_nice_text()) {  || tt_new_pad_look) { // && tt_log_version_number > 14) { // new on 060699 -- conditonal commented out on 040102
////		new_character_width = default_talk_balloon_character_width(); // was set_character_width prior to 211201
////		new_character_height = default_talk_balloon_character_height();
//		// commented out the following on 050102 but maybe should revive it using edge_size_proportion and a show_all() test? 
////		if (!display_just_nice_text()) { // displaying within a pad so adjust sizes (new on 020102) -- is a 6x8 peg pad
////			new_character_width = (6*new_character_width)/4;
////			new_character_height = (8*new_character_height)/6;
////		};
////	} else {
////		if (text_length > 0) {
//////		character_width = width/longest_line;
////		// following avoids occassional pixel wide gaps between plates
//////		city_coordinate one_pixel = 
//////		character_width = tt_screen->horizontal_pixel_length(character_width)*one_pixel;
////			new_character_width = width_given_scale(y_scale)/number_of_lines; // was set_character_width prior to 211201
//////			character_height = height_given_scale(y_scale)/number_of_lines; // new on 060699 -- // commented out on 111299 since text got smaller since this rounded down and that changed y_scale and so on...
////			if (new_character_width == 0) new_character_width = one_horizontal_pixel;  // was set_character_width prior to 211201
////			// adjust width accordingly
//////		width = min(character_width*longest_line,max_sprite_width);
////		};
////		if (number_of_lines > 1) {
////		if (character_height < 0) { // new conditional and restored the following on 030900 since not always intialized properly
//////			if (tt_new_pad_look) { // new on 171201 since except for first line overlaps 1 out of 8 pegs
//////				new_character_height = (8*height)/((number_of_lines-1)*7+8);
//////			} else {
////				new_character_height = height/number_of_lines;
//////			};
////			new_character_height = tt_screen->vertical_pixel_length(new_character_height)*one_vertical_pixel; // commented out on 111299 since text got smaller since this rounded down and that changed y_scale and so on...
////			if (new_character_height == 0) new_character_height = one_vertical_pixel;
////		} else {
////			new_character_height = character_height;
////		};
////	};
////	if (!character_size_fixed) {
//		new_character_width = default_talk_balloon_character_width(); // was set_character_width prior to 211201
//		new_character_height = default_talk_balloon_character_height();
//		set_character_width((tt_character_size_percent*new_character_width)/100);
//		set_character_height((tt_character_size_percent*new_character_height)/100); // was character_height = prior to 211201
//		// following doesn't call set_size since that will call this as well
//		height = character_height*number_of_lines;
//		width = character_width*longest_line;
////	};
////#if TT_32 // at least for now
////	if (display_just_nice_text()) { // variable width
////		 character_width = tt_screen->correct_font_width((string) wide_text,text_length, 
////														  character_width, 
////														  character_height,
////														  TRUE); // bold by default
////		 character_height = tt_screen->correct_font_height((string) wide_text,text_length, 
////															 character_width, 
////															 character_height,
////															 TRUE);
////	};
////#endif
//};

void Text::good_size(city_coordinate &good_width, city_coordinate &good_height, Sprite *source, boolean for_expander) {
	// 8/3 and 4/3 in the following since set_font now uses character size not cell size (110102)
//	if (need_wide_characters()) {
	// made  8/3 (was 4/3) on 220102 
	if (tt_log_version_number >= 72) {
		good_width = default_talk_balloon_character_width(); // new on 210205 to look better by being much narrower
	} else {
		good_width = (8*default_talk_balloon_character_width())/3; // not sure why this looks better (new on 170102)
	};
//	} else {
//		good_width = (8*default_talk_balloon_character_width())/3;
//	};
	good_height = (4*default_talk_balloon_character_height()/3);
	if (is_blank()) { // moved out here on 140202
		good_width = (4*good_height)/3; //  new on 170102
	} else if (text_length == 0) {
		good_width = (2*good_height)/edge_size_proportion; // new on 110102
	} else if (text_length > 0) { // conditional added on 200202
//#if TT_DEBUG_ON
//		if (!tt_uudecoding_a_sprite && !showing_on_a_screen()) {
//			debug_this(); // since can be costly
//		};
//#endif
		int ignore_longest_line;
		tt_screen->get_extent_size(wide_text,text_length,number_of_lines,									  
											good_width,good_height,
											ignore_longest_line,good_width,good_height,
											TRUE,!use_variable_width_font());
		city_coordinate edge_size = compute_edge_size(good_width,good_height);
		good_width += edge_size*2; // new on 270202
		good_height += edge_size*2;
		// prior to 270202 was the following but that only works for text pads
//		good_width = (edge_size_proportion*good_width)/(edge_size_proportion-2); // restored on 220102 since the above doesn't include the edges
//		good_height = (edge_size_proportion*good_height)/(edge_size_proportion-2);
	};
	if (tt_log_version_number >= 40) {
		// the following does the right thing if standing in room while robot is working - new on 040404
		adjust_good_size(good_width,good_height,source);
	} else {
		adjust_size_for_source(good_width,good_height,source);
	};
//	if (display_just_nice_text() || tt_new_pad_look) { // new on 180699
//		good_width = default_talk_balloon_character_width()*longest_line;
//		good_height = default_talk_balloon_character_height()*number_of_lines;
	// updated on 080102 to be the extent needed when using default character sizes	
//		if (!display_just_nice_text()) { // displaying within a pad so adjust sizes (new on 020102) - revised on 050102
//			good_width = (edge_size_proportion*good_width)/(edge_size_proportion-2);
//			good_height = (edge_size_proportion*good_height)/(edge_size_proportion-2);
//		};
//	} else {
//		Sprite::good_size(good_width,good_height,source,for_expander); // width and height of single text pad
//		good_width /= 3; // natural size is too big
//		good_height /= 3;
//		if (text_length == 0) {
//			good_width = 4*tt_screen->one_pixel_in_x_city_units();
//		} else {
//			good_width *= longest_line;
//		};
//		good_height *= number_of_lines;
//	};
};

MatchStatus Text::match(Text *other_text_object, int version) {
   // used by BuiltInSound but they could just compare indices...
	if (blank) return(MATCH_GOOD);
	wide_string others_text;
	long others_text_length;
	other_text_object->current_wide_text(others_text,others_text_length);
	if (text_length == others_text_length &&
		 (text_length == 0 ||
		  compare_wide_strings_of_length(wide_text,others_text,text_length) == 0)) {
//#if TT_DEBUG_ON
//		if (compare_wide_strings(wide_text,L"Down arrow",10) == 0) {
//			log("debug this");
//		};
//#endif
		return(MATCH_GOOD);
	} else {
		return(MATCH_BAD); 
	};
};

SpriteType Text::dump_type() {
   if (blank) {
		return(BLANK_TEXT);
   } else if (use_variable_width_font()) {
		return(VARIABLE_WIDTH_TEXT);
   } else {
		return(TEXT);
	};
};

boolean Text::dump(output_stream &stream, boolean just_prepare, boolean , boolean ) {
   if (just_prepare) return(FALSE);
   int dump_index = about_to_dump(this,stream); // rewritten on 020802
	if (dump_index < 0) return(FALSE); // new on 051099
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
		print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
		tt_error_file() << "About to dump ";
		print(tt_error_file());
		tt_debug_dump_depth++;
		tt_error_file() << " whose dump type is " << type_string(dump_type()) << endl;
	};
#endif
   stream.put((unsigned char) dump_type());
	dump_if_not_null(stream,comment);
	// moved here from conditional on 311099
	string narrow_text = return_dump_text(); // return_narrow_text();
	unsigned short int narrow_text_length; // update this to long when format changes
	if (narrow_text == NULL) {  
		narrow_text_length = 0; // added 011199
	} else {
		 int true_length = strlen(narrow_text);
		 if (true_length > max_unsigned_short) {
			 tt_error_file() << "Saving a text pad with more than " << max_unsigned_short << " characters. Currently only the first " << max_unsigned_short << " is saved. Sorry." << endl;
			 narrow_text_length = max_unsigned_short;
		 } else {
			narrow_text_length = true_length;
		 };
	};
	stream.write((character *) &narrow_text_length, sizeof(narrow_text_length));
	stream.write((character *) narrow_text,narrow_text_length);
	delete [] narrow_text; // was consed by return_dump_text (170200)
//	unsigned short byte_length = text_length*sizeof(wide_character);
//	stream.write((character *) &byte_length, sizeof(byte_length));
//	stream.write((char *) wide_text,byte_length);
	stream.put(text_color); // new on 030399
	stream.put((unsigned char) (infinite_stack()*4+visible()*2+show_all())); // infinite_stack()*4 added on 080400
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
		print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
		tt_error_file() << "Finished dumping ";
		print(tt_error_file());
		tt_error_file() << endl;
		tt_debug_dump_depth--;
	};
#endif
	return(TRUE);
};

#if TT_XML

xml_character_data *Text::xml_create_character_data(xml_element *element, xml_document *document) {
	long length;
   wide_string text;
   current_wide_text(text,length);
	if (length == 0) return(NULL); // new on 270103 -- moved here on 040704
   xml_character_data *cdata = create_xml_character_data(text,document);
	if (cdata == NULL) {
		if (length > 1) { // new error recovering as of 040704
			// split into first character and the rest (problem is presumably that the text is itself XML
			int index = 0;
			int previous_index = 0;
			while (index < length) {
				if (text[index] == L']' && index+2 < length && text[index+1] == L']' && text[index+2] == L'>') { 
					// break at every ]]> to "quote" XML since ]] closes a CDATA section -- updated on 050704 to only do this with ]]> not just ]]
					if (previous_index > 0) { // not the first one so save the current segment
						text[index] = NULL;
						cdata = create_xml_character_data(text+previous_index,document);
						if (cdata != NULL) { // otherwise warn??
							xml_append_child(cdata,element);
						};
						text[index] = L']';
					};
					index++;
               wide_character saved_character = text[index];
					text[index] = NULL; // terminate it
					cdata = create_xml_character_data(text+previous_index,document);
					text[index] = saved_character; // restore it
					previous_index = index;
					if (cdata != NULL) { // otherwise warn??
						xml_append_child(cdata,element);
					};
				} else {
					index++;
				};
			};
			cdata = create_xml_character_data(text+previous_index,document);
		};
	};
	return(cdata);
};

void Text::xml(xml_element *element, xml_document *document) {
//   xml_element *element = create_xml_element(xml_tag(),document);
	if (text_length > 0) { // was data != NULL prior to 040704 // new on 270103 to deal with NULL strings
		xml_element *value_element = create_xml_element(L"TextValue",document);
		xml_character_data *data = xml_create_character_data(value_element,document); // moved here so can pass the correct element on 040704
		xml_append_child(data,value_element);
		xml_append_child(value_element,element);
	};
   if (text_color != 0) { // if black don't bother
      xml_append_child(create_xml_element(L"ColorIndex",document,text_color),element);
   };
   if (use_variable_width_font()) { // new on 080802
      xml_append_child(create_xml_element(L"VariableWidthFont",document),element);
   };
	if (insertion_point >= 0) { // new on 120604
		xml_set_attribute(element,L"EditInsertionPoint",insertion_point);
	};
//   append_standard_elements(element,document); // handled by Sprite::xml now (121102)
	Sprite::xml(element,document);
};

xml_element *Text::xml_set_special_attributes(xml_element *element, xml_document *document) {
	// new on 201202 -- went through all the state variables
	xml_element *special_element = Sprite::xml_set_special_attributes(element,document);
	// following two attributes were commented out (since first written?) and restored as experiment on 240904
	//if (change_size_to_fit_extent_flag) {
	//	xml_set_attribute(special_element,L"ChangeSizeToFitExtent",1);
	//};
	//if (!change_font_size_to_fit_extent_flag) {
	//	xml_set_attribute(special_element,L"DontChangeFontSizeToFitExtent",1);
	//};
	if (default_plate_state != TEXT_PAD_WITH_PLATE) {
		xml_set_attribute(special_element,L"DefaultPlateStateCode",(int) default_plate_state);
	};
	if (saved_right_side) {
		xml_set_attribute(special_element,L"DroppedOnRightSide",1);
	};
	// narrow_text is just a "cache"
	// text_length, number_of_lines, longest_line easy to recompute
	// character_width, character_height recomputed
	return(special_element);
};

boolean Text::xml_get_special_attributes(xml_node *node) {
	// following two lines restored as experiment on 240904
//	change_size_to_fit_extent_flag = xml_get_attribute_int(node,L"ChangeSizeToFitExtent",change_size_to_fit_extent_flag);
//	change_font_size_to_fit_extent_flag = !xml_get_attribute_int(node,L"DontChangeFontSizeToFitExtent",!change_font_size_to_fit_extent_flag);
	default_plate_state = (PlateState) xml_get_attribute_int(node,L"DefaultPlateStateCode",TEXT_PAD_WITH_PLATE); // prior to 210404 was (int) default_plate_state);
	saved_right_side = xml_get_attribute_int(node,L"DroppedOnRightSide",0); // prior to 210404 was saved_right_side
	return(Sprite::xml_get_special_attributes(node));
};

boolean Text::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
      case TEXT_VALUE_TAG:
         {
            int length;
            // using xml_get_text_all_copy in case it has been split up to avoid very wide lines that won't travel thru email
            wide_string text_value = xml_get_text_all_copy(node,length);
            set_wide_text(text_value,length); 
//#if TT_DEBUG_ON
//				if (wide_text != NULL && wide_text[0] == 'Z') {
//					tt_error_file() << "debug this" << endl;
//				};
//#endif
            return(TRUE);
         };       
      case COLOR_INDEX_TAG:
//         xml_color_index(this,node);
         set_color(xml_get_int(node));
         return(TRUE);
      case VARIABLE_WIDTH_FONT_TAG:
         set_use_variable_width_font(TRUE);
         return(TRUE);
		case NO_MORE_TAGS:
			insertion_point = xml_get_attribute_int(node,L"EditInsertionPoint",insertion_point);
			// commented out the following on 150903 since very wasteful and can wait until first time they are displayed instead
			//	if (text_length > 0) { // to get the font sizes right - new on 290803
			//		update_size(TRUE); 
			//	} else { 
			//		set_character_height(height);
			//		set_character_width((3*height)/5); // 5x3 looks nice (good modularity?)
			//	};
			// and do the following
      default:
         return(Sprite::handle_xml_tag(tag,node));
   };
};

//void Text::xml_dump(output_stream &stream) { // new on 010802
//   int dump_index = about_to_dump(this,stream); // rewritten on 020802
//	if (dump_index < 0) return; // new on 051099
//   string narrow_text = return_dump_text(); // what if need wide characters??
//   string tag = "TextValue"; // xml_tag();
//   stream << "<" << tag << ">" << endl;
//   xml_dump_index(dump_index,stream);
//   stream << "<Value xml:space=\"preserve\">";
//   print_string_as_xml_string(narrow_text,stream);
//   stream << "</Value>" << endl;
//   xml_dump_comment(stream);
////   xml_guid(stream);
//   if (leader == NULL || (leader->kind_of() == PICTURE && !leader->is_flipped())) { // what if this is an indirection?
//      xml_dump_geometry(stream);
//   };
//   if (text_color != 0) { // if black don't bother
//      stream << "<ColorIndex>" << (int) text_color << "</ColorIndex>" << endl;
//   };
//   xml_dump_flags(stream);
//   stream << "</" << tag << ">" << endl;
//   if (narrow_text != NULL) {
//      delete [] narrow_text;
//   };
//};

#endif

void Text::java(java_stream &out) {
   if (blank) {
      out << S(IDS_JAVA_TT_BLANK_TEXT);
   } else {
      java_string text_copy = java_text_value();
      java_new(out,S(IDS_JAVA_TT_TEXT),text_copy);
      delete [] text_copy;
   };
};

java_string Text::java_text_value() {
   java_string text_copy = new character[text_length*2+3]; // in case just /r
   int j = 0;
   text_copy[j++] = '"';
//	string text = copy_narrow_string(wide_text,text_length);
   // rewritten on 120702 since the above missed out half the string in Japanese
   string text = return_narrow_text();
	if (text == NULL) { // new on 160605
		text = ""; // good enuf?
	};
   int stop = strlen(text);
   for (int i = 0; i < stop; i++) {
   // this makes \r print as \r rather than a return
      if (new_line(text[i])) {
         text_copy[j++] = '\\';
         text_copy[j++] = 'r';
			if (text[i+1] == 10) { // carriage return
				i++;
			};
		} else if (text[i] == '\\') { // new on 190601 to double back slashes
			text_copy[j++] = '\\';
         text_copy[j++] = '\\';
		} else if (text[i] == '"') { // new on 241003
			text_copy[j++] = '\\';
         text_copy[j++] = '"';
      } else {
         text_copy[j++] = text[i];
      };
   };
   text_copy[j++] = '"';
   text_copy[j] = NULL; // terminate string
//	delete [] text;
   return(text_copy);
};

boolean Text::to_windows(Pointer *data, short int &data_offset,
                         ascii_string types, short int &types_offset,
                         BirdPointer *, short int &) {
   // updates data and types
   types[types_offset] = 'S'; // token for string
   types_offset++;
   // following assumes it is terminated and won't change ...
	// it will however be deleted once the text value changes
   data[data_offset] = return_narrow_text(); 
   data_offset++;
   return(TRUE);
};

void Text::display(SelectionFeedback selection_feedback, boolean followers_too, TTRegion *region) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("About to display text debug target.");
	};
#endif
	if (!visible()) return;
//	if (rescale_if_camera_above(selection_feedback,followers_too,region)) return; -- commented out on 080202 - 
	// should everyone comment this out?? --- many were on 160804 but what about the few remaining?
	if (pointer_to_top_of_stack() != NULL) { // new on 260500
		pointer_to_top_of_stack()->display(selection_feedback,followers_too,region);
		return;
	};
	if (want_feedback(selection_feedback)) {
		selection_feedback = MOVEMENT_FEEDBACK; // don't support growth
	};
//	if (is_blank()) { // commented out && !display_just_nice_text() on 050102 -- added !is_blank() on 050102 -- then commented out on 050102 so can get right blank plate
		// removed text_length == 0 || on 050102 
//		if (!show_all()) return;
//		Sprite::display(selection_feedback,followers_too,region); // only one plate
//	} else {
		city_coordinate adjusted_llx, adjusted_lly;
		city_coordinate adjusted_width, adjusted_height;
		if (region == NULL) {
			adjusted_llx = llx;
			adjusted_lly = lly;
			adjusted_width = width;
			adjusted_height = height;
		} else {
			adjusted_llx = region->min_x;
			adjusted_lly = region->min_y;
			adjusted_width = region->width();
			adjusted_height = region->height();
		};
		city_coordinate one_x_pixel = tt_screen->one_pixel_in_x_city_units();
      if (width < tt_ideal_units_per_horizontal_pixel) {
         return; // was one_x_pixel prior to 240100 but that broken time travel buttons when flying very high
      };
      if (want_feedback(selection_feedback)) {
			adjusted_llx += selection_delta_x();
			adjusted_lly += selection_delta_y();
//			adjusted_width = width+
//			adjusted_height = height+one_y_pixel*selection_y_growth();
//			adjusted_character_width = adjusted_width/longest_line;
//			adjusted_character_width 
//				= tt_screen->horizontal_pixel_length(adjusted_character_width)
//				  *one_x_pixel;
		};
		//if (insertion_point >= 0) { // new on 100604
		//	adjusted_width += character_width; // for vertical bar
		//};
		if (tt_screen->off_screen(adjusted_llx,adjusted_lly,adjusted_width,adjusted_height)) return; // new on 160102
//	   if (display_just_nice_text()) { // commented out on 040102
			// saving of character size is new on 160200 since the region may be temporarily imposed
			// this shows up most when a remote looks is on the back of a flipped picture
//			city_coordinate saved_character_width, saved_character_height;
//			boolean character_size_saved = FALSE;
			color background_color;
			bytes permutation = color_permutation(selection_feedback);
			if (is_blank()) { // new on 050102
				display_plate(adjusted_llx,adjusted_lly,adjusted_width,adjusted_height,plate_state(),TRUE,permutation);
				return;
			};
			if (text_length == 0) {
            if (show_all()) { // conditional new on 220702 so looks better when rubbing out a robot's name
				   // new on 050102 so is drawn as 2 vertical edges
				   display_two_plate_borders(adjusted_llx,adjusted_lly,adjusted_width,adjusted_height,plate_state(),permutation);
            };
				return;
			};
			if (permutation != NULL) {
				if (tt_bits_per_pixel == 8) {
					// new on 080202
					background_color = compute_color(appropriate_background_color_index(),permutation);
				} else {
					background_color = alter_color24(appropriate_background_color(),(int *) permutation); // 24-bit version of color
				};
			} else {
				background_color = appropriate_background_color();
			};
			/* obsolete as of 080102
			if (region != NULL) { // moved here on 180699 so when part of a picture its size changes correctly
				saved_character_width = character_width; 
				saved_character_height = character_height;
				character_size_saved = TRUE;	
				if (!compute_display_text_character_size(adjusted_width,adjusted_height)) {
					set_character_width(saved_character_width); // new on 160200
					set_character_height(saved_character_height);
//					if (show_all()) { // new on 120201 so a white rectangle is shown when text is too small
					// used to draw rectangle here but do it earlier now
//					};
					return; // too small
				};
//			} else if (!clean_or_moved()) { // condition added 221299 -- commented out on 060102
//				// no need to call compute_number_of_lines(sizes_constrained);
//				update_sizes(TRUE,TRUE); // new on 180699 -- this is pretty wasteful -- really necessary??
			};
			*/
         city_coordinate edge_size;
         city_coordinate adjusted_text_width, adjusted_text_height;
         city_coordinate adjusted_character_width,adjusted_character_height;
         update_size_internal(region,adjusted_width,adjusted_height,edge_size,
                              adjusted_text_width,adjusted_text_height,adjusted_character_width,adjusted_character_height,FALSE);
			// following commented out since display_text will get a 0 sized adjusted_character_width and return then
//			if (character_width < one_x_pixel*2 || character_height < one_x_pixel*2) return; // should really compare with one_y_pixel
			if (show_all()) { // new on 040102 - moved here on 220102
				// this does a better job then letting the TextOut do it - both for very long text and multi-line
BEGIN_GDI
				tt_main_window->select_stock_object(NULL_PEN);
				HGDIOBJ old_selection = tt_main_window->select_object(tt_main_window->create_solid_brush(background_color));
				tt_screen->draw_rectangle(adjusted_llx,adjusted_llx+adjusted_width, // new on 170799
												  adjusted_lly,adjusted_lly+adjusted_height);
				if (old_selection != NULL) DeleteObject(tt_main_window->select_object(old_selection));
END_GDI
			};
         display_text(adjusted_llx+edge_size,adjusted_lly+edge_size,adjusted_character_width,adjusted_character_height,adjusted_text_width,
							 adjusted_text_height,color_permutation(selection_feedback),TRUE,background_color); 
         // FALSE above changed to TRUE on 040102 -- restored on 080702 -- 
			// and then changed back on 120702 since draw background of text above if needed
#if TT_DEBUG_ON
			if (tt_debug_mode == 170102) {
				tt_error_file() << "display_text llx =" << (adjusted_llx+edge_size) << "; width=" << adjusted_text_width << endl;
			};
#endif
//			if (character_size_saved) { // obsolete as of 080102
//				set_character_width(saved_character_width); // new on 160200
//				set_character_height(saved_character_height);
//			};		 
			if (show_all()) { // edge_size > 0) { // new on 040102 -- could check that it is greater than one pixel ... 180102 experiment
				display_plate_borders(adjusted_llx,adjusted_lly,adjusted_width,adjusted_height,plate_state(),permutation);
#if TT_DEBUG_ON
				if (tt_debug_mode == 170102) {
					tt_error_file() << "display_plate_borders llx =" << adjusted_llx << "; width=" << adjusted_width << endl;
				};
#endif
			};
//		};
};

void Text::update_size(boolean regardless) {
   city_coordinate edge_size;
   city_coordinate adjusted_text_width, adjusted_text_height;
   city_coordinate adjusted_character_width,adjusted_character_height;
   // ignore all these values
   update_size_internal(NULL,width,height,edge_size,
                        adjusted_text_width,adjusted_text_height,adjusted_character_width,adjusted_character_height,
                        regardless);
};

void Text::update_size_internal(TTRegion *region, city_coordinate adjusted_width, city_coordinate adjusted_height,
                                city_coordinate &edge_size,
                                city_coordinate &adjusted_text_width, city_coordinate &adjusted_text_height,
                                city_coordinate &adjusted_character_width, city_coordinate &adjusted_character_height,
                                boolean regardless) { 
   // abstracted on 281002
//#if TT_DEBUG_ON
//	if (wide_text != NULL && wide_text[0] == 'Z') {
//		tt_error_file() << "debug this" << endl;
//	};
//#endif
   adjusted_text_width = adjusted_width;
	adjusted_text_height = adjusted_height;
	city_coordinate old_adjusted_width = adjusted_width;
	city_coordinate old_adjusted_height = adjusted_height;
//			city_coordinate edge_size = 0;
//			if (show_all()) { // new on 040102 -- commenting out on 180102 since looks nicer and this way the text doesn't change when show_all is changed
	edge_size = compute_edge_size(adjusted_width,adjusted_height);
	adjusted_text_width -= 2*edge_size;
	adjusted_text_height -= 2*edge_size;
//			};
	// made following obsolete on 230804
//	if (!adjust_height_if_camera_above()) { // new on 210100 for time travel buttons -- moved here on 220102 from :display_text
////		restore_old_city_scale = tt_screen->current_scale();
////		tt_screen->set_scale(100);
//		city_scale scale = tt_screen->current_scale();
//		adjusted_width = (adjusted_width*scale)/ground_scale;
//		adjusted_height = (adjusted_height*scale)/ground_scale;
////		start_x = (start_x*scale)/100;
////		start_y = (start_y*scale)/100;
//	};
//	if (fixed_width == 0) { // introduced on 240100 for stability for text like those in time travel task bar
		// updated to use adjusted_width etc on 170799 so displays right if a remote looks and size is different
	adjusted_character_width = character_width; // new on 211201
	adjusted_character_height = character_height;
	city_coordinate old_adjusted_text_width = adjusted_text_width;
	city_coordinate old_adjusted_text_height = adjusted_text_height;
	if (change_size_to_fit_extent_flag && !change_font_size_to_fit_extent()) { // new on 070102 -- moved here on 220102 from :display_text
		// new policy on 050202 so if change_font_size_to_fit_extent_flag then do it first (so character_width etc are up-to-date for changing size)
		if ((leader != NULL || animation_in_progress()) // things have changed or is animating (changed on 110102) -- added () on 130104 to be sure of associativity
			 && !regardless) { // new on 061003
			change_size_to_fit_extent_flag = FALSE;
			change_font_size_to_fit_extent_flag = TRUE;
		} else if (character_width > 0 && character_height > 0) {
			int ignore_longest_line;
			tt_screen->get_extent_size(wide_text,text_length,number_of_lines,
												character_width,character_height,
												ignore_longest_line,adjusted_text_width,adjusted_text_height,
												TRUE,!use_variable_width_font());
			if (adjusted_text_width > adjusted_text_height) { // so edge is fraction of height
				adjusted_height = (adjusted_text_height*edge_size_proportion)/(edge_size_proportion-2);
				adjusted_width = adjusted_text_width+3*compute_edge_size(adjusted_width,adjusted_height); // experiment on 150903 3* was 2* -- does look nicer - a little space between text and border
			} else {
				adjusted_width = (edge_size_proportion*adjusted_text_width)/(edge_size_proportion-2);
				adjusted_height = adjusted_text_height+2*compute_edge_size(adjusted_width,adjusted_height);
			};
//				if (!animation_in_progress()) { // commented out on 110102
				change_size_to_fit_extent_flag = FALSE;
//				};
			change_font_size_to_fit_extent_flag = FALSE; // new on 180102 since already took care of things
		};
	};
	if (change_font_size_to_fit_extent() || (region != NULL)) { 
		// region check added on 210102 since with remote looks and the like same thing can be displayed with different dimensions) {
		tt_screen->correct_font_size(wide_text,text_length,number_of_lines,
											  adjusted_text_width,adjusted_text_height,TRUE,!use_variable_width_font(),TRUE, // /longest_line /number_of_lines
											  adjusted_character_width,adjusted_character_height); 
		if (change_font_size_to_fit_extent() && (regardless || !animation_in_progress())) { 
			// no point caching if displaying within a region or animating
			// removed  && region == NULL on 281002 to indirections to text look like ordinary text pads
         // regardless added on 301002
         // restored NULL region test on 250102 since otherwhise breaks when a remote looks is involved
			if (text_length > 0) { // new on 061003 since no point changing this for zero length text
				set_character_width(adjusted_character_width);
			};
			set_character_height(adjusted_character_height);
			change_font_size_to_fit_extent_flag = FALSE; // prior to 220102 this was conditionalized by !animation_in_progress()
		};		
	};
	// adjusted_text_width was updated to deal with change in text
	// done right above...
//			if (edge_size > 0) { // need to compute width that would produce adjusted_text_width after subtracting edge size
//				if (old_adjusted_text_width != adjusted_text_width) {
//					adjusted_width = (adjusted_text_width*edge_size_proportion)/(edge_size_proportion-2); // restored on 220102 since if size changed due to extent this is more accurate
////					adjusted_text_width += 2*edge_size; // new version on 160102
//				};
//				if (old_adjusted_text_height != adjusted_text_height) { 
//					adjusted_height = (adjusted_text_height*edge_size_proportion)/(edge_size_proportion-2);
////					adjusted_text_height += 2*edge_size; // new version on 160102
//				};
//			};
	if (old_adjusted_text_width != adjusted_text_width) { // || old_adjusted_text_height != adjusted_text_height) { 
//		if (leader == NULL && !held()) { // commented out on 121202 since causes pads to move on the floor if robots on back are changing number of characters
//			move_by((width-adjusted_width)/2,0); 
////					if (adjusted_width < width) { // shrinking -- new on 170102 -- commented out conditional on 220102 since needs to happen regardless now
//				// so since size is being changed while displaying the dirty region code gets confused so flush it
//				tt_screen->invalidate_cache();
////					};
//		};
		Sprite::set_size(adjusted_width,adjusted_height,NEW_TEXT_EXTENT); // don't want to trigger changes by text::set_size...
		// not clear why the following works better than set_size -- restored on 170102
//				width = adjusted_text_width;
	} else if (old_adjusted_text_height != adjusted_text_height) { 
//				height = adjusted_text_height;
		Sprite::set_size(adjusted_width,adjusted_height,UPDATING); // re-written on 180102
	};
};

/*
//		if (text_length > 1) {
//			set_character_width(adjusted_width/longest_line);
//			set_character_width(tt_screen->horizontal_pixel_length(character_width)*one_x_pixel);
		// rewrote the above 2 lines on 211201 since no need to call set_character_width twice
//		set_character_width(tt_screen->horizontal_pixel_length(adjusted_width/longest_line)*one_x_pixel); -- commented out on 020102 so font size is stable
//         if (region == NULL) {
//            width = character_width*longest_line;
//         };
			if (character_width <= one_x_pixel) {
//				Sprite::display(selection_feedback,followers_too,region);
            // need the above to get the color right in general
            display_plate(adjusted_llx,adjusted_lly,adjusted_width,adjusted_height,plate_state(),blank,color_permutation(selection_feedback)); // was just width, height prior to 160101 so when tiny and remote looks displayed big
				return;
			};
//		};
		city_coordinate one_y_pixel = tt_screen->one_pixel_in_y_city_units();
      if (height < tt_ideal_units_per_horizontal_pixel) return; // was one_y_pixel prior to 240100
//		if (number_of_lines > 1) {
			if (tt_new_pad_look) { // new on 171201 since except for first line overlaps 1 out of 8 pegs
//				character_height = (8*adjusted_height)/((number_of_lines-1)*7+8); -- commented out on 030102
			} else {
				character_height = adjusted_height/number_of_lines;
			};
			// commented out the following on 240400 since it causes letters to get smaller when height isn't a multiple - e.g. 1024x768
//			character_height = tt_screen->vertical_pixel_length(character_height)*one_y_pixel;
			if (character_height <= one_y_pixel) {
//				Sprite::display(selection_feedback,followers_too,region);
            display_plate(adjusted_llx,adjusted_lly,width,height,plate_state(),blank,color_permutation(selection_feedback));
				return;
			};
//		};
#if TT_DEBUG_ON
		millisecond start_time = timeGetTime();	
#endif
		long offset = 0;
		city_coordinate adjusted_character_height; // new on 171201 so multi-line text only has a single not a double divider
		if (tt_new_pad_look) {
			adjusted_character_height = (7*character_height)/8; // 8 pegs tall so skip one
		} else {
			adjusted_character_height = character_height;
		};
		city_coordinate initial_y_offset = (number_of_lines-1)*adjusted_character_height;
		city_coordinate y_offset = initial_y_offset;
		boolean showed_last_line = FALSE;
		bytes permutation = color_permutation(selection_feedback);
		while (offset < text_length) {
			int characters_printed = display_plated_wide_text(wide_text+offset,text_length-offset,
																			  adjusted_llx,adjusted_lly+y_offset,
																			  character_width,character_height,
																			  plate_state(),show_all(),!blank,text_color,permutation);
			if (characters_printed == 0) break; // better than looping but what should it really do?
			offset += characters_printed;
			y_offset -= adjusted_character_height-one_y_pixel;
			if (!showed_last_line && offset == text_length && wide_text[text_length-1] == '\r') {
				// last character is a new line so show blank line
				offset--;
				showed_last_line = TRUE;
			};
		};
		if (!blank && tt_new_pad_look) {
			// new on 211201 since this way the text is displayed by the GDI inside the pad
			// adjust position and size so "frame" isn't written on
			display_text(adjusted_llx+character_width/6,adjusted_lly+character_height/8,adjusted_width-character_width/3,adjusted_height-adjusted_character_height/4,permutation,TRUE);
		};				
#if TT_DEBUG_ON
		if (tt_debug_mode == 3) {
			tt_error_file() << (timeGetTime()-start_time) << " ms to display -- TEXT of length " 
							<< text_length << endl;
		};
#endif		
	};
*/

void Text::indirection_of_set_parameter(short int new_parameter, boolean ,
                                        boolean ) {
   text_color = (color_index) (new_parameter%tt_colors_available);
};

void Text::compute_number_of_lines() { // boolean sizes_constrained) {
	number_of_lines = 1;
	long current_line = 0; // was unsigned short int prior to 090202
	int old_longest_line = longest_line; // new on 060102
	longest_line = 0;
	if (wide_text == NULL) return; // added on 070799 since it can happen that text_length is 1 while wide_text is NULL
	for (int i = 0; i < text_length; i++) {
		if (new_line_wide(wide_text[i])) {
			number_of_lines++; // moved before the following on 050704
			if (wide_text[i+1] == '\0') break; // new on 300604 to deal with text ending with new line
			if (current_line > longest_line) {
				longest_line = current_line;
			};
			if (wide_text[i+1] == '\n' && wide_text[i] != wide_text[i+1]) { // carriage return - 
				// second condition new on 100604 to deal with 2 or more new lines in a row
				i++; // skip it
			}; 
			current_line = 0;
		} if (wide_text[i] == '\t') { // new on 020903 to deal with tabs which currently go the next multiple of 8
			current_line = ((current_line/8)+1)*8;
		} else {
			current_line++;
		};
	};
	if (current_line > longest_line)  {
		longest_line = current_line;
	};
	if (longest_line == 0) {
		longest_line++; // blank return
	};
//	if (!sizes_constrained && old_longest_line != longest_line && leader == NULL) { // new on 060102
		// following doesn't call set_size since that will call this as well
//		height = character_height*number_of_lines;
//		width = character_width*longest_line;
//	};
};

boolean Text::compute_display_text_character_size(city_coordinate region_width, 
																  city_coordinate region_height) {
	// added arguments on 190299 so that this works when displayed in a region - e.g. when a part of something
   // returns FALSE if too small
   city_coordinate one_x_pixel = tt_screen->one_pixel_in_x_city_units();
   if (region_width < one_x_pixel || longest_line == 0) return(FALSE); // disjuncted add on 20300
	set_character_width(region_width/longest_line);
   // can only set it to integer pixel values anyways
   set_character_width((character_width/one_x_pixel)*one_x_pixel);
	// rewritten on 290499 because it looks better (at least on the wall with puzzles)
//				= tt_screen->horizontal_pixel_length(character_width)*one_x_pixel;
   if (character_width <= one_x_pixel) return(FALSE);
   // problem with following is that size can't grow gradually...
//   width = character_width*longest_line; // fix round-off inconsistencies
   city_coordinate one_y_pixel = tt_screen->one_pixel_in_y_city_units();
   if (region_height < one_y_pixel) return(FALSE);
//	if (tt_new_pad_look) { // new on 171201 since except for first line overlaps 1 out of 8 pegs
//		character_height = (8*region_height)/((number_of_lines-1)*7+8);
//	} else {
		character_height = region_height/number_of_lines;
//	};
//#if TT_DEBUG_ON
//		 if (character_height < 1000) {
//			 log("debug this");
//		 };
//#endif
   character_height = (character_height/one_y_pixel)*one_y_pixel;
//			  = tt_screen->vertical_pixel_length(character_height)*one_y_pixel;
   if (character_height <= one_y_pixel) return(FALSE);
//   height = character_height*number_of_lines;
   return(TRUE);
};

void Text::display_text(city_coordinate start_x, city_coordinate start_y, 
								city_coordinate adjusted_character_width, city_coordinate adjusted_character_height, 
								city_coordinate adjusted_width, city_coordinate adjusted_height, bytes color_permutation, 
								boolean background_transparent, color background_color) { 
#if TT_DEBUG_ON
	debug_log(debug_counter,"display_text of debug target.");
#endif
   // just displays the text with variable spacing etc.
	// just_show_text new on 211201 since used by new text pad looks
	if (!visible()) return;
	if (text_length == 0 || wide_text == NULL) return; // wide_text disjunct added on 160603 for robustness
//   if (!size_constrained()) { // commented out on 180699
//		if (!compute_display_text_character_size(adjusted_width,adjusted_height)) return; // too small
//	};
   long offset = 0;
//	const city_coordinate one_y_pixel = tt_screen->one_pixel_in_y_city_units();
	// changed on 250699 so multi-line text all has same font size
//	if (character_width == 0 || character_height == 0) {
		// following commented out on 040102 since done elsewhere (by caller)
//		if (!show_all()) return; // new on 040899
//BEGIN_GDI
//		tt_main_window->select_stock_object(NULL_PEN);
//		HGDIOBJ old_selection = tt_main_window->select_object(tt_main_window->create_solid_brush(background_color));
//		tt_screen->draw_rectangle(start_x,start_x+adjusted_width, // new on 170799
//										  start_y,start_y+adjusted_height);
//		if (old_selection != NULL) DeleteObject(tt_main_window->select_object(old_selection));
//END_GDI
//		return; // on 140799 check before and after since can't correct 0 but might become 0 -- commented out on 080102
//	};
//	city_scale restore_old_city_scale = -1;
	city_coordinate y_offset = adjusted_height; // (number_of_lines-1)*character_height; -- moved here on 220102 in case adjusted_height changed above
	city_coordinate vertical_extent = adjusted_height/number_of_lines; // new on 150799
	vertical_extent -= tt_screen->one_pixel_in_y_city_units(); // restored on 170799 to subtract a pixel to deal with round off
//		vertical_extent = character_height;
	// noticed this was obsolete on 211201
//	} else {
//		set_character_width(fixed_width*tt_screen->one_pixel_in_x_city_units()+tt_screen->one_pixel_in_x_city_units()/2);
//		set_character_height(fixed_height*tt_screen->one_pixel_in_y_city_units()+tt_screen->one_pixel_in_y_city_units()/2);
//	};
	// following was just a zero test prior to 160799
	if (!background_transparent && 
//		 (adjust_height_if_camera_above() && // added on 210100 to get time travel buttons to work
		  ((character_width < minimum_character_width() || character_height < minimum_character_height()) || blank)) {
		// test for blank added on 121099 so blank variable width pads display better
//		if (restore_old_city_scale > 0) {
//			tt_screen->set_scale(restore_old_city_scale);
//		};
		// following commented out on 040102
//		if (!show_all()) {
//			return; // new on 040899
//		};
//BEGIN_GDI
//		tt_main_window->select_stock_object(NULL_PEN);
//		HGDIOBJ old_selection = tt_main_window->select_object(tt_main_window->create_solid_brush(background_color));
//		tt_screen->draw_rectangle(start_x,start_x+adjusted_width, // new on 170799
//										  start_y,start_y+adjusted_height);
//		if (old_selection != NULL) DeleteObject(tt_main_window->select_object(old_selection));
//END_GDI
		return;
	};
	//tt_screen->get_vertical_extent((char *) wide_text,text_length,character_width,character_height,TRUE,FALSE);	
	if (tt_right_to_left_text) { // new on 081203 for Hebrew
		start_x += adjusted_width;
	};
	//wide_string output_text; // new on 100604 // commented out the same day
	//int output_text_length;
	//if (insertion_point >= 0 && !held() && (tt_selection != this)) { // || leader != NULL)) { // new on 100604
	//	// alternatively should reset this when no longer selected // or when it gets a leader
	//	insertion_point = -1;
	//};
	//boolean reclaim_output_text = current_wide_text_with_insertion_character(insertion_point,wide_text,text_length,output_text,output_text_length);
   while (offset < text_length) { 
      int i = 0;
		int skip = 1;
      for (; offset+i < text_length; i++) {
         if (new_line_wide(wide_text[offset+i])) {
				// commented out 050704 since didn't display last line right
				//if (offset+i+1 == text_length) { // new on 040704 to fix a display bug with a line followed by a return
				//	return;
				//} else 
				if (wide_text[offset+i+1] == '\n' && wide_text[offset+i] != wide_text[offset+i+1]) {
					skip = 2; // skip both
				};
				// added test on 100604 to deal with two new lines in a row and not assume one is redundant
//				if (wide_text[offset+i+1] == '\0') { // && i > 0) { // ends with a new line
////					i--; // new on 300604 so when pasting something in there isn't a spurious empty line at the end
//					skip = 2; // so don't go one more time through with just new line character
//				};
				break;
			};
      };
		int caret_position = -1;
		if (insertion_point >= offset && insertion_point-offset <= i) {
			caret_position = insertion_point-offset;
		};
      tt_screen->text_out((char *) (wide_text+offset),i,start_x,start_y+y_offset,
								  adjusted_character_width,adjusted_character_height,TRUE,!use_variable_width_font(),(!show_all() || background_transparent),text_color,
								  // bold looks nicer
								  TRUE,color_permutation,background_color,adjusted_width,TRUE,
								  caret_position);
//								  start_x+adjusted_width,start_y+adjusted_height+y_offset);
		//  prior to 080702 background_color was WHITE  
		y_offset -= vertical_extent; // prior to 150799 was computed each line (which on just new lines was 0)	            
      offset += i+skip;
      // substract 1 pixel to avoid gaps caused by round off
		// -one_y_pixel was commented away but gap was visible -- restored on Aug 15 '98
//      y_offset -= character_height-one_y_pixel;
	};
	//if (reclaim_output_text) {
	//	delete [] output_text;
	//};
//	if (restore_old_city_scale > 0) {
//		tt_screen->set_scale(restore_old_city_scale);
//	};
};

boolean Text::size_and_location_of_characters(wide_character character, 
															 city_coordinate adjusted_width, city_coordinate adjusted_height,
															 city_coordinate &box_width, city_coordinate &box_height,
															 city_coordinate &box_llx, city_coordinate &box_lly) {
   // returns true if character occurs in string
   // and sets the box to region where those characters are
   // character can be repeated
   int start_position = -1;
   int end_position = -1;
   int line_start_index = 0;
   int position = 0;
//   int line = 0;
	box_lly = lly+height;
//	string narrow_text = return_narrow_text(); // on 180699 switched to referring to narrow_text rather than wide_text
//	int length = strlen(narrow_text);
	if (!compute_display_text_character_size(adjusted_width,adjusted_height)) return(FALSE); // too small
	city_coordinate adjusted_character_width,adjusted_character_height;
	tt_screen->correct_font_size(wide_text,text_length,number_of_lines, 
		// updated the following 2 args on 180799 to more accurately reflect how ::display_text works
										  adjusted_width,adjusted_height,TRUE,!use_variable_width_font(),TRUE, // /longest_line /number_of_lines
										  adjusted_character_width,adjusted_character_height);
	city_coordinate y_delta = adjusted_height/number_of_lines-tt_screen->one_pixel_in_y_city_units();
   int i;
   for (i = 0; i < text_length; i++) {
      if (wide_text[i] == character) {
         if (start_position == -1) { // first occurence
            start_position = position;
            end_position = position;
         } else {
            end_position++;
         };
      } else {
         if (start_position != -1) break; // done
         if (new_line_wide(wide_text[i])) {
				box_lly -= y_delta;
//					tt_screen->get_vertical_extent(wide_text+line_start_index,i-line_start_index,
//																	   adjusted_character_width,adjusted_character_height,TRUE,FALSE);
            position = 0;
 //           line++;
            line_start_index = i+1;
         } else {
            position++;
         };
      };
   };
   if (start_position == -1) return(FALSE); // didn't find one
//   if (!compute_display_text_character_size(adjusted_width,adjusted_height)) return(FALSE); // too small -- removed 180699
//   box_lly = lly+height-(line+1)*character_height;
//	city_coordinate vertical_extent = tt_screen->get_vertical_extent(wide_text+line_start_index,end_position-start_position,
//																						  adjusted_character_width,adjusted_character_height,TRUE,FALSE);
	box_lly -= y_delta; // vertical_extent;
//   box_llx = llx+start_position*character_width;
   box_llx = llx+tt_screen->get_extent(wide_text+line_start_index,start_position, 
                                       adjusted_character_width,adjusted_character_height,TRUE,FALSE); // FALSE was TRUE prior to 180699
	box_llx += compute_edge_size(adjusted_width,adjusted_height)/2-tt_screen->one_pixel_in_x_city_units();
	// new on 010704 for more accuracy -- why was following commented out??
//   box_llx += compute_edge_size(adjusted_width,adjusted_height)/2; // new on 230902 for more accuracy
	box_height = y_delta; // vertical_extent; //character_height;
//   box_width = character_width*(1+end_position-start_position);
   box_width = tt_screen->get_extent(wide_text+line_start_index+start_position,1+end_position-start_position, 
                                     adjusted_character_width,adjusted_character_height,TRUE,FALSE); // FALSE was TRUE prior to 180699
	box_width += tt_screen->one_pixel_in_x_city_units(); // new on 010704 since can be off by a pixel
   return(TRUE);
};

boolean Text::rectangular() {
	// good enuf but if every line same length is still rectangular
	return(number_of_lines == 1 && show_all()); // show_all() added 261199 (important, for example, when sitting on a nest)
};

Sprite *Text::used(Sprite *subject, Sprite *by, boolean button_unchanged, boolean record_action,
                   millisecond duration, boolean modified, Sprite *original_subject) { // modified flag is for MCI
//   if (tt_new_input_scheme) {
      Picture *replacement = make_indirect_picture(this);
//		replacement->set_stopped(FALSE); // added on 101199 so flipped numbers can be tossed, etc.
      // this won't be recorded right will it??
	   replacement->used(subject,by,button_unchanged,record_action,duration,FALSE,original_subject);
      user_did(TEXT,TEXT_FLIPPED);
	   return(replacement);
//   };
};

#if TT_DEBUG_ON
void Text::set_character_width(city_coordinate new_width) { // new on 240100 for debugging
//	if (character_width != new_width) {
//		if (tt_debug_target == debug_counter) {
		//	tt_error_file() << "Changing character width of debug target to " << (long) new_width << endl;
		//} else if (new_width < 300) {
		//	log("debug this");
//		};
//	};
	character_width = new_width;
};

void Text::set_character_height(city_coordinate new_height) { // new on 240100 for debugging
	if (tt_debug_target == debug_counter && character_height != new_height) {
		tt_error_file() << "Changing character height of debug target to " << (long) new_height << endl;
	};
	//if (new_height < 1000 && character_height != new_height) {
	//	tt_error_file() << "debug this" << endl;
	//};
	character_height = new_height;
};
#endif

boolean Text::equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, SpritesPointer &expected_corresponding_nests, EqualReason reason) { // new on 060601
	if (other == this) return(TRUE);
	if (other->dump_type() != dump_type()) return(FALSE); // using dump_type so this works for sounds and force effects as well (i.e. same name and same type)
	if (is_blank() != other->is_blank()) return(FALSE);
	Text *other_text = (Text *) other;
	if (return_text_color() != other_text->return_text_color()) return(FALSE);
	long my_length, other_length;
	string my_string, other_string;
	current_text(my_string,my_length);
	other->current_text(other_string,other_length);
	if (my_length != other_length) return(FALSE);
	return(memcmp(my_string,other_string,my_length) == 0);
};

boolean Text::ok_to_dump_as_text() {
	// updated on 010704 to check also that it is show_all and black
	if (use_variable_width_font_flag && text_color == 0 && show_all_flag) { // since that is the default format when loaded back in
		long my_length;
		string my_string;
		current_text(my_string,my_length);
		if (!white_space_only_at_ends(my_string,my_length)) {
			// since will load as a text pad
			return(TRUE);
		};
	   return(!can_be_rational_number(my_string,my_length)); // but don't let "123 become 123
	} else {
		return(FALSE);
	};
};

//void Text::set_is_picture(boolean new_flag) {
//	if (new_flag) {
//		change_font_size_to_fit_extent_flag = FALSE; // new on 121203 so it doesn't remember the font when displayed within the region of the Picture
//	};
//	Sprite::set_is_picture(new_flag);
//};

/* made obsolete when tt_new_input_scheme became alway TRUE
	if (!by->still_running()) {
		if (record_action) record_event(APPLY_GRASPED_ITEM,by,floor,this,TRUE,(char) modified);
		user_did(TEXT,TEXT_SOUNDED,by);
		simulate_mouse_up();
	};
	if (!(tt_sound_option&SOUND_BIT) ||
       (tt_screen->pointer_to_background()->pointer_to_room() !=
		  floor->pointer_to_room())) { // in another house -- good policy?
		completed(action_status);
		return(this);
	};
   if (modified && text_length != 0) { // MCI interface for now
      ascii_string temp = copy_narrow_string(wide_text,text_length);
		ascii_string return_message = new char[256];
		tt_main_window->MCI_send_string(temp,TRUE,return_message,256);
		if (return_message[0] != NULL) {
			set_text(a2u(return_message)); // become return value
		} else {
			delete [] return_message;
		};
      delete [] temp;
		return(this);
	};
#if TT_TEXT_TO_SPEECH
   if (text_length > 0 && !modified) {
		string text = copy_narrow_string(wide_text,text_length);
      string replacement = process_for_text_to_speech(text,text_length);
		wide_string wide_replacement = copy_wide_string(replacement);
      speak(wide_replacement,-1);
      delete [] replacement;
		delete [] text;
		delete [] wide_replacement;
   };
   completed(action_status);
   return(this);
#else
//	completed_after(3000,action_status,this); // trouble if on back of picture
	completed(action_status);
	const int sound_count = LAST_SOUND;
	// could leave open a way to "speak" the text
	// e.g. sounds start with something or control is held down too
	int sound_name_length;
	for (int i = 0; i < sound_count; i++) {
		if (!skipped_sound_index(i)) { // not one of the one's skipped
			string sound_name = S(IDS_FIRST_SOUND+i);
			sound_name_length = strlen(sound_name);
			if (sound_name_length == text_length &&
				 strnicmp(sound_name,wide_text,text_length) == 0) {
				// play this if tied with earlier call to this
				// had 4th arg TRUE -- will be redone with MCI anyway
				play_sound(i+1,10,FALSE); // sound indices are 1-indexed
            if (!told_user(TOLD_TYPING_SOUND_NAMES_OBSOLETE)) {
               say(IDS_TYPING_SOUND_NAMES_OBSOLETE,4);
               user_told(TOLD_TYPING_SOUND_NAMES_OBSOLETE);
            };
				return(this);
			};
		};
	};
	play_sound(PLOP_SOUND);
	return(this);
#endif
*/

Text *load_text(SpriteType type, InputStream *pad_in, int notebook_version, NaturalLanguage language, long index, Sprite *container, boolean descendants_inactive) {
	Sprite *comment = NULL;
	if (notebook_version >= first_version_to_save_copied_nests) {
		comment = load_item(pad_in,notebook_version,language);
	};
	unsigned short int length; // if this is ever changed also change pages::first_page_matching_text
   pad_in->read((string) &length, sizeof(length));
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	wide_string contents;
//	if (unicode_version(notebook_version)) {
		string narrow_contents = new char[length+1];
		pad_in->read((string) narrow_contents,length);
		narrow_contents[length] = '\0';
//	   contents = copy_wide_string(narrow_contents);
      contents = canonicalize_new_lines(narrow_contents); // new on 090802
#if TT_DEBUG_ON
		if (tt_debug_text_pad != NULL && strncmp(tt_debug_text_pad,narrow_contents,strlen(tt_debug_text_pad)) == 0) {
			// substring match
			tt_error_file() << "Creating debug text pad " << narrow_contents << endl;
		};
#endif
		delete [] narrow_contents;
//	} else {
//		contents = read_old_string(pad_in,length);		
//	};
   Text *text = new Text(0,0,contents); // commented out on 091199 so this works with Unicode strings length
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
   if (container != NULL) {  // new on 160702
      if (container->kind_of() == PROGRAM_PAD) {
         text->set_in_notebook((Notebook *) container,FALSE); // no point recuring
      } else if (container->kind_of() == VACUUM) {
         text->set_inside_vacuum(TRUE,FALSE);
      };
   }; 
	associate_shared_item(index,text);
	delete [] contents;
   switch (type) {                   
	   case BLANK_TEXT:
		   text->become_blank();
         break;
	    case VARIABLE_WIDTH_TEXT:
         text->set_use_variable_width_font(TRUE);
         break;
   };
   text->set_comment(comment);
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	if (notebook_version >= first_version_to_save_copied_nests) {
		text->set_color(pad_in->get());
		unsigned char flags = (unsigned char) pad_in->get();
		text->set_visible((boolean) (flags>>1));
		text->set_show_all((boolean) (flags&1));
		text->set_infinite_stack((boolean) (flags>>2)); // new on 080400 - will be FALSE for all older dumps
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
	};
   return(text);
};

boolean describe_text(SpriteType type,
                      InputStream *pad_in, output_stream &text_out, int version_number,
                      boolean in_thought_bubble, Article ) {
	if (version_number >= first_version_to_save_copied_nests) {
		describe_comment_encoding(pad_in,text_out,version_number,TEXT);
	};
   unsigned short int length;
	pad_in->read((string) &length, sizeof(length));
//	char contents[1024]; // what if longer than 1024?  use ... ??
   string contents = new char[length+1];
	if (length > 0) {
		pad_in->read((string) contents,length);
		contents[length] = NULL; // terminate the string
	};
//			print_article(article,text_out);
	if (type == BLANK_TEXT) {
		if (in_thought_bubble) {
			text_out << S(IDS_TEXT_WITH_ANYTHING);
		} else {
			text_out << S(IDS_BLANK_TEXT_PAD);
		};
	} else if (length == 0) {
	 	text_out << S(IDS_EMPTY_TEXT);
	} else {
		replace_carriage_returns_with_spaces(contents,length);
		if (tt_describing_a_comment) {
			text_out << " \"" << contents << "\" "; // new on 210205
		} else {
			text_out << S(IDS_PAD_WITH_TEXT) << " \"" << contents << "\" " << S(IDS_ON_IT);
		};
	};
	if (version_number >= first_version_to_save_copied_nests) {
		pad_in->get(); // ignore color
		pad_in->get(); // ignore flags like show all
	};
   delete [] contents;
	return(TRUE);
};

Text *variable_width_text_pad(const_string s, boolean show_all, color_index text_color) {
	Text *text = new Text();
	text->set_text(s);
	text->set_use_variable_width_font(TRUE);
	text->set_show_all(show_all);
	text->set_color(text_color);
	return(text);
};

Text *make_label(const_string s, color_index text_color, boolean show_all) {
	Text *text = new Text();
	text->set_text(s);
	text->set_use_variable_width_font(TRUE);
	text->set_show_all(show_all);
	text->set_graspable(FALSE);
	text->set_selectable(FALSE);
	text->set_color(text_color);
	return(text);
};

Text *make_label(wide_string s, color_index text_color, boolean show_all) { // wide_string version new on 120802
	Text *text = new Text();
	text->set_wide_text(s);
	text->set_use_variable_width_font(TRUE);
	text->set_show_all(show_all);
	text->set_graspable(FALSE);
	text->set_selectable(FALSE);
	text->set_color(text_color);
	return(text);
};

wide_string Text::copy_text_string() {
	return(copy_wide_string(wide_text));
};

string Text::copy_narrow_text_string() {
	return(copy_string(return_narrow_text()));
};

boolean Text::current_text(string &the_text, long &the_text_length) {
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	the_text = return_narrow_text(); 
	// just set the_text_length to text_length prior to 091199 but that is wrong for Unicode strings
	// why? asked on 060506
	if (the_text == NULL) { 
		the_text_length = 0;
	} else {
		the_text_length = strlen(the_text); 
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	return(!blank);
};

boolean Text::current_wide_text(wide_string &the_text, long &the_text_length) {
	the_text = wide_text;
	the_text_length = text_length;
	return(!blank && the_text != NULL); // second conjunct added 241104 but this is safer since caller won't assume it has some text
};

string Text::return_narrow_text() {
#if TT_WIDE_TEXT
	if (narrow_text == NULL) {
		narrow_text = copy_narrow_string(wide_text,text_length);
	};
	return(narrow_text);
#else
	return(wide_text);
#endif
};

void Text::now_on_floor(Background *new_background, Sprite *by) {
	// new on 141204 so when typing to them they act like an ordinary copy (with Maggie)
	// added text_length == 0 since that is the only case that really needs it 
	// and otherwise it makes control-c of text have bad font size
	if (text_length == 0 && tt_log_version_number >= 64) {
		set_change_font_size_to_fit_extent(FALSE);
	};
	Sprite::now_on_floor(new_background,by);
};



