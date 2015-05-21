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
#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif   
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif   
#if !defined(__TT_SCREEN_H)   
#include "screen.h"
#endif   
#if !defined(__TT_CUBBY_H)   
#include "cubby.h"
#endif   
#if !defined(__TT_BIRD_H)   
#include "bird.h"
#endif   
#if !defined(__TT_PRGRMMR_H)   
#include "prgrmmr.h"
#endif   
#if !defined(__TT_ROBOT_H)   
#include "robot.h"
#endif   
#if !defined(__TT_TRUCK_H)   
#include "truck.h"
#endif   
#if !defined(__TT_NUMBER_H)
#include "number.h"
#endif   
#if !defined(__TT_PAD_H)   
#include "pad.h"
#endif   
#if !defined(__TT_EVENT_H)   
#include "event.h"
#endif   
#if !defined(__TT_THOUGHT_H)   
#include "thought.h"
#endif   
#if !defined(__TT_BOMB_H)   
#include "bomb.h"
#endif   
#if !defined(__TT_TEXT_H)   
#include "text.h"
#endif
#if !defined(__TT_PICTURE_H)   
#include "picture.h"
#endif   
#if !defined(__TT_HELP_H)   
#include "help.h"
#endif
#if !defined(__TT_PUZZLE_H)
#include "puzzle.h"
#endif
//#include "martian.h"
#if !defined(__TT_DIRTY_H)   
#include "dirty.h"
#endif   
#if TT_WINDOWS
#if !defined(__TT_WINMAIN_H)   
#include "winmain.h"
#endif   
#else
#include "sound.h"
#endif
#if !defined(__TT_TOOLS_H)   
#include "tools.h"
#endif   
#if !defined(__TT_USER_H)   
#include "user.h" // to reset input list
#endif
#if !defined(__TT_BCKGRND_H)
#include "bckgrnd.h"
#endif
#if TT_DIRECTX
#if !defined(__TT_INPUT_H)
#include "input.h"
#endif
#endif
#if !defined(__TT_FLOOR_H)   
#include "floor.h" 
#endif   
#if !defined(__TT_LOG_H)   
#include "log.h" 
#endif
#if !defined(__TT_SPEAK_H)   
#include "speak.h" 
#endif 
   
#include <stdlib.h> // for itoa

#include <math.h>
////#include <strstrea.h> commented out 210703 // commented out on 210703

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// if changed update help.cpp
enum ExpanderHistory {EXPANDER_GREW_ITEM=0,
//							 EXPANDER_SHRUNK_ITEM,
//							 EXPANDER_GREW_HORIZONTALLY,  
//							 EXPANDER_GREW_VERTICALLY,
                      EXPANDER_CHANGED_STATE_VIA_BUTTON,  
                      EXPANDER_CHANGED_STATE_VIA_KEYBOARD,  
							 EXPANDER_CALLED};

// if changed update help.cpp
enum VacuumHistory {VACUUM_USED_TO_SUCK=0,
						  VACUUM_USED_TO_MAKE_BLANK,
						  VACUUM_USED_TO_RESTORE,
                    VACUUM_CHANGED_STATE_VIA_BUTTON,  
                    VACUUM_CHANGED_STATE_VIA_KEYBOARD,  
						  VACUUM_CALLED};

// if changed update help.cpp
enum CopierHistory {COPIER_APPLIED=0,
						  COPIER_ATTACHED_RELEASED,
						  COPIER_CALLED,
						  COPIER_REMOVED_BLANKS,
                    COPIER_CHANGED_STATE_VIA_BUTTON,  
                    COPIER_CHANGED_STATE_VIA_KEYBOARD};  

enum VacuumStates {VACUUM_INERT, VACUUM_MORPHING_TO_LIFE,
						 VACUUM_MORPHING_TO_INERT, VACUUM_SUCKING_UP,
						 VACUUM_SPITTING_OUT, VACUUM_WALKING_NORTH,
						 VACUUM_WALKING_SOUTH, VACUUM_WALKING_WEST,
						 VACUUM_WALKING_EAST, VACUUM_LAYING};

//void Tool::prepare_for_deletion(Sprite *by) {
// taken care of fine by Sprite::
//  if (prepared_for_deletion()) return; // already done
//  if (button != NULL) {
//     remove_follower(button);
//     button->destroy();
//     button = NULL;
//  };
//  Sprite::prepare_for_deletion(by);
//};

Tool::Tool(SpriteCode appearance_code,
			 city_coordinate center_x,
			 city_coordinate center_y,
			 SpriteType col_type,
			 short int initial_parameter,
			 city_scale current_scale,
			 city_coordinate width,
			 city_coordinate height) :
		 Sprite(appearance_code,
				  center_x,center_y,
//				  background,
				  col_type,
				  initial_parameter,
				  current_scale,
				  width,height),
		 running_to_hand_flag(FALSE),
		 being_used_flag(FALSE),
       sound_id(-1),
       on_flag(FALSE),
       used_since_pick_up_flag(FALSE),
       called_since_last_release(FALSE),
       favorite_width(-1),
       button(NULL),
		 default_favorite_llx(tile_width), 
		 default_favorite_lly(tile_height),
		 state(0), 
		 saved_state(0) { // these didn't have default values prior to 180703
			// moved here on 190803 so that following can be shared by all tools
		 	update_display(0,0,UPDATE_INITIALIZE); // new on 100803 since is no longer automatically updated if loaded as part of a city
			// defaults not yet set
}; 

Sprite *Tool::released(Sprite *by, boolean top_level, SpecialDrop special_drop) {
	turn_off(by);
   called_since_last_release = FALSE;
	return(Sprite::released(by,top_level,special_drop));
};

void Tool::set_favorite_size_and_location() {
	// following new on 190999
	if (floor != NULL && floor->inside_thought_bubble()) return; // just the robot's imagination so don't make these changes stick
   favorite_width = width;
   favorite_height = height;
   if (llx+width < 1) { // off the left side
      favorite_llx = tile_width/4-width; // a bit on on screen
   } else if (llx >= ideal_screen_width) {
      favorite_llx = ideal_screen_width-tile_width/4;
   } else {
      favorite_llx = llx;
   };
   if (lly+height < 1) { // off the bottom
      favorite_lly = tile_height-height;
   } else if (lly >= ideal_screen_height) {
      favorite_lly = ideal_screen_height-tile_height/4;
   } else {
      favorite_lly = lly;
   };
};

void Tool::set_default_favorite_size_and_location(city_coordinate width,
                                                  city_coordinate height,
                                                  city_coordinate x,
                                                  city_coordinate y) {
  if (favorite_width <= 0) {
     favorite_width = width;
     favorite_height = height;
	  favorite_llx = x;
     favorite_lly = y;
  };
  // following is new on 011100
  default_favorite_llx = x;
  default_favorite_lly = y;
};

void Tool::favorite_size(city_coordinate &width, city_coordinate &height) {
   if (favorite_width <= 0) {
      good_size(width,height);
   } else {
      width = favorite_width;
      height = favorite_height;
   };
};

Sprite *Tool::select(TTRegion *grasp_region, SelectionReason reason, Sprite *in_hand) {
   if (reason == MIGHT_GRASP && button != NULL && grasp_region != NULL) {
      TTRegion button_region;
		really_propagate_changes(); // experiment 030204 -- seems to fix a time travel bug but not clear why
      button->active_region(button_region);
      if (button_region.overlaps(grasp_region)) return(button);
   };
   return(Sprite::select(grasp_region,reason,in_hand));
};

// obsolete
//void Tool::active_region(TTRegion &region) {
	// update it before returning it
//	region = *(pointer_to_collision_region());
//	true_region(region);
//	region.max_x = (6*region.min_x+region.max_x)/7; // just leftmost 1/7th
//	return(&region);
//};

void Tool::toggle_on(Sprite *by) {
   if (is_on()) {
      turn_off(by);
   } else {
      turn_on(by);
   };
};

void Tool::stop_repeating_sound() {
   if (!showing_on_a_screen()
#if !TT_BUG_FIXES 
		|| used_once_per_click() // removed on 021203 since can leave Pumpy making noise after being dropped
#endif
		) return; // new on 310702
//#if TT_DIRECTX
   stop_sound_id(sound_id);
   sound_id = -1;
//#else
//   stop_sound(999); // this got called even when a tool was turned on off screen
//#endif
};

void Tool::turn_on(Sprite *by) {
   if (by != NULL && by->still_running()) return; // running robots just use tools
   if (showing_on_a_screen()
		 && !used_once_per_click()) { // this works for Pumpy as well as Dusty -- new on 231103
//		 && !tt_vacuum_used_once_per_click) { // added !tt_vacuum_used_once_per_click on 260902
      repeating_sound();
   };
   on_flag = TRUE;
   used_since_pick_up_flag = TRUE;
};

void Tool::turn_off(Sprite *by) {
   if (by != NULL && by->still_running()) return; 
   stop_repeating_sound(); 
   on_flag = FALSE;
   set_being_used(FALSE); // new on 310702
//   set_being_used(FALSE);
//	stop_postponing_new_logs(); // new on 170100
};

boolean Tool::get_grasped() {	
	running_to_hand_flag = FALSE;
	finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
	return(tt_programmer->programmer_at_floor()->pick_up(this));
};

void Tool::button_pushed(Button *, Sprite *by) {
   set_state(state+1,by);
   if (showing_on_a_screen()) play_sound(POP_SOUND); // find something better // condition added 170502
   ::completed(by);
   boolean speak_new_state = (tt_speak_button_name && by != NULL && by->kind_of() != ROBOT);
   switch (kind_of()) {
      case EXPANDER:
        doing(EXPANDER_CHANGED_STATE_VIA_BUTTON,by);
        if (speak_new_state) { // new on 151002
			  stop_speaking(); // new on 291003 so if still describing the previous state that is aborted
           speak(IDS_EXPANDER_STATE0+state);
        };
        break;
      case VACUUM:
        doing(VACUUM_CHANGED_STATE_VIA_BUTTON,by);
        if (speak_new_state) { // new on 151002
			  stop_speaking(); // new on 291003 so if still describing the previous state that is aborted
           speak(IDS_VACUUM_STATE0+state);
        };
        break;
      case COPIER:
        doing(COPIER_CHANGED_STATE_VIA_BUTTON,by);
        if (speak_new_state) { // new on 151002
			  stop_speaking(); // new on 291003 so if still describing the previous state that is aborted
           speak(IDS_COPIER_STATE0+state);
        };
        break;
  };
};

void Tool::update_button() {
   if (button != NULL) {
      button->set_letter(button_label());
      button->set_clean_status(DIRTY);
   };
};

//void Tool::update_button_size_and_location() { // noticed obsolete on 250803
//   city_coordinate button_width, button_height, button_llx, button_lly;
//   good_button_size_and_location(button_width,button_height,button_llx,button_lly);
//   button->set_size_and_location(button_width,button_height,button_llx,button_lly);
//};

void Tool::update_button_position() {
	// new on 160804 otherwise when pasting in the floor contents the button of the magic wand drifts
	if (button != NULL && tt_log_version_number >= 51) {
		city_coordinate button_width, button_height, button_llx, button_lly;
		good_button_size_and_location(button_width,button_height,button_llx,button_lly);
		if (button_width == 0 || button_height == 0) { // made things worse: && tt_log_version_number >= 55) {
			return; // new on 141004 since if city saved while leaving thought bubble can have tool that is just 1x1 and button that is 0
			//  && tt_log_version_number >= 55 new on 191204
		};
		button->update_display();
		button->set_size_and_location(button_width,button_height,button_llx,button_lly);
//		really_propagate_changes();
	//	else warn?
	};
};

void Tool::set_button(Sprite *new_button) { // new on 240103
	if (new_button == button) {
		return;
	};
	if (new_button == NULL || new_button->fine_kind_of() != TOOL_BUTTON) {
		return; // warn??
	};
	if (button != NULL) { // a bit wasteful - could make it so that when created by XML that they don't have buttons
		remove_follower(button);
		button->destroy();
	};
	button = (Button *) new_button;
	add_follower(button,TRUE,INSERT_AT_END,TRUE);
};

void Tool::stop_running() {
	if (!running_to_hand()) return;
	running_to_hand_flag = FALSE;
	Sprite::stop_running();
};

#if TT_XML

void Tool::xml(xml_element *element, xml_document *document) {
//#if TT_DEBUG_ON
//	// 250803
//	if (kind_of() == COPIER) {
//		tt_error_file() << "On frame " << tt_frame_number << " Wand's button is at " << button->current_llx() << "," << button->current_lly() << endl;
//		really_propagate_changes();
//		tt_error_file() << "On frame " << tt_frame_number << " Wand's button is at " << button->current_llx() << "," << button->current_lly() <<  " AFTER PROPAGATION" << endl;
//	};
//#endif
	button->xml_create_and_append_element(element,document)->Release();
	// following added 180703 - important for time travel and saved cities
	if (running_to_hand_flag) {
		xml_set_attribute(element,L"RunningToHand",1);
	};
	if (being_used_flag) {
		xml_set_attribute(element,L"BeingUsed",1);
	};
	if (sound_id >= 0) { // should never be zero
		xml_set_attribute(element,L"SoundID",sound_id);
	};
	if (on_flag) {
		xml_set_attribute(element,L"On",1);
	};
	if (used_since_pick_up_flag) {
		xml_set_attribute(element,L"UsedSincePickedUp",1);
	};
	if (called_since_last_release) {
		xml_set_attribute(element,L"CallSinceLastRelease",1);
	};
	if (favorite_width > 0) {
		xml_set_attribute(element,L"FavoriteWidth",favorite_width);
		xml_set_attribute(element,L"FavoriteHeight",favorite_height);
		xml_set_attribute(element,L"FavoritePositionLeft",favorite_llx);
		xml_set_attribute(element,L"FavoritePositionBottom",favorite_lly);
	};
	if (default_favorite_llx != tile_width) {
		xml_set_attribute(element,L"DefaultFavoritePositionLeft",default_favorite_llx);
	};
	if (default_favorite_lly != tile_height) {
		xml_set_attribute(element,L"DefaultFavoritePositionBottom",default_favorite_lly);
	};
	if (state != 0) {
		xml_set_attribute(element,L"StateCode",state); // last arg was 1 prior to 180803
	};
	if (saved_state != 0) {
		xml_set_attribute(element,L"SavedStateCode",saved_state);  // last arg was 1 prior to 180803
	};
	Sprite::xml(element,document);
};

boolean Tool::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
		case BUTTON_TAG: // new on 240103
			set_button(xml_load_sprite(node,tag,button)); // set_button does nothing if set to == button
			return(TRUE);
      case NO_MORE_TAGS:
			// most of this new on 180703
			running_to_hand_flag =	xml_get_attribute_int(node,L"RunningToHand",0); // prior to 210404 was running_to_hand
			being_used_flag = xml_get_attribute_int(node,L"BeingUsed",0); // prior to 210404 was being_used_flag
			sound_id = xml_get_attribute_int(node,L"SoundID",-1); // prior to 210404 was sound_id
			on_flag = xml_get_attribute_int(node,L"On",0); // prior to 210404 was on_flag
			used_since_pick_up_flag = xml_get_attribute_int(node,L"UsedSincePickedUp",used_since_pick_up_flag);
			called_since_last_release = xml_get_attribute_int(node,L"CallSinceLastRelease",0); 
			// prior to 210404 was called_since_last_release);
			favorite_llx = xml_get_attribute_int(node,L"FavoritePositionLeft",0); // prior to 210404 was favorite_llx
			favorite_lly = xml_get_attribute_int(node,L"FavoritePositionBottom",0); // prior to 210404 was favorite_lly
			favorite_width = xml_get_attribute_int(node,L"FavoriteWidth",0); // prior to 210404 was favorite_width
			favorite_height = xml_get_attribute_int(node,L"FavoriteHeight",0); // prior to 210404 was favorite_height
			default_favorite_llx = xml_get_attribute_int(node,L"DefaultFavoritePositionLeft",tile_width); 
			// prior to 210404 was default_favorite_llx
			default_favorite_lly = xml_get_attribute_int(node,L"DefaultFavoritePositionBottom",tile_height); 
			// prior to 210404 was default_favorite_lly
			set_state(xml_get_attribute_int(node,L"StateCode",0)); // prior to 210404 was state
			saved_state = xml_get_attribute_int(node,L"SavedStateCode",0); // prior to 210404 was saved_state
         // and do the following (new on 121102)
      default:
         return(Sprite::handle_xml_tag(tag,node));
   };
};

#endif

enum ExpanderStates {EXPANDER_INERT,EXPANDER_MORPHING_TO_LIFE,
						   EXPANDER_MORPHING_TO_INERT,EXPANDER_PUMPING,
						   EXPANDER_SUCKING,EXPANDER_WAITING,EXPANDER_JUMPING,
						   EXPANDER_READY_TO_SUCK,EXPANDER_READY_TO_PUMP,
                     EXPANDER_BREATHING,EXPANDER_WAITING_NEW};

Expander::Expander(city_coordinate x, city_coordinate y) :
  Tool(EXPANDER_SPRITE,x,y,EXPANDER),
  last_subject(NULL),
  normal_flag(TRUE),
  modified_previously(FALSE),
  grow_just_width_flag(FALSE),
  use_start(0) {
//  action_status(NULL) {
//  still_working(FALSE)
   state = EXPANDER_BIGGER;
//	set_priority_function_of_lly(FALSE);
   city_coordinate button_width, button_height, button_llx, button_lly;
   good_button_size_and_location(button_width,button_height,button_llx,button_lly);
   button = new Button(BIKEPUMP_BUTTON_SPRITE,button_llx,button_lly,
                       'B',priority()-1,
                       button_width,button_height,
                       tt_white);
   button->set_show_all(FALSE);
   add_follower(button);
   update_button();
};

boolean Expander::ok_to_delete() { // new on 060704 to deal with friends of Tooly getting destroyed (e.g. in a vacuum)
	if (ref_count == 2 && tt_toolbox_expander == this) {
		if (tt_reincarnate_tools) {
			// only reference remaining is from Tooly so replace that with a fresh one
			Expander *expander = new Expander();
			set_toolbox_expander(expander);
			initialize_toolbox_expander(TRUE);
		} else {
			set_toolbox_expander(NULL);
		};
	};
	return(Tool::ok_to_delete());
};

void Expander::good_button_size_and_location(city_coordinate &button_width,
                                             city_coordinate &button_height,
                                             city_coordinate &button_llx,
                                             city_coordinate &button_lly) {
   city_coordinate real_width = true_width();
   city_coordinate real_height = true_height();
   button_width = (2*real_width)/5;
   button_height = real_height/3;
   button_llx = llx+(7*real_width)/20+(width-real_width);
   button_lly = lly+real_height/4;
};


Sprite *Expander::copy(boolean also_copy_offsets) {
//	city_coordinate center_x, center_y;
//	center_location(center_x,center_y);
	Expander *copy = new Expander(llx,lly);
	copy->set_size(width,height,COPY_INITIALIZATION);
   copy->set_state(state);
 //  copy->set_priority_fixed(priority_fixed_flag);
	//if (comment != NULL) {
	//	copy->set_comment(comment->copy());
	//};
   finish_making_copy(copy,also_copy_offsets);
	return(copy);
};

//EXPANDER_BIGGER,EXPANDER_TALLER,EXPANDER_WIDER,
//EXPANDER_SMALLER,EXPANDER_SHORTER,EXPANDER_NARROWER

int state_index(char key, const_string labels, Sprite *by) {
   char key_string[2];
   key_string[0] = key;
   key_string[1] = NULL;
   AnsiUpper(key_string);
   key = key_string[0]; // upper case in local sensitive manner
   int length = _tcslen(labels);
   for (int i = 0; i < length; i++) {
      if (labels[i] == key) {
         button_letter_typed(by);
         return(i);
      };
   };
   return(-1); // not found
};

void record_equivalent_keyboard_event(int state, int new_state, string labels, Sprite *by,
												  Background *floor,Tool *tool) {
	// added on 110699 so that robot's training is language independent
	// replaces shortcut keys to tools with equivalent number of + or -
	if (new_state > state) {
		for (int i = 0; i < new_state-state; i++) {
	      record_event(KEYBOARD_CHARACTER,by,floor,tool,TRUE,'+');
		};
	} else if (new_state < state) {
		for (int i = 0; i < state-new_state; i++) {
	      record_event(KEYBOARD_CHARACTER,by,floor,tool,TRUE,'-');
		};
	};
};

Sprite *Expander::respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected,
                                      Sprite *by, boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless) {
	if (!extended) { // condition new on 120604
		int new_state = state;
		boolean record_key = TRUE; // unless proven otherwise
		int index = state_index(key,tt_expander_keyboard_accelerators,by);
		if (index == state) { // new on 271102 to deal with keyboard characters when already in that state
			key = tt_run_character; // so pretend it was the space bar
			index = -1; // and pretend never saw the index
		};
		if (index >= 0) {
			new_state = index;
			record_key = (new_state != state);
			if (record_key && by->kind_of() == ROBOT_IN_TRAINING) {
				// keystroke changes state and turns on (uses) tool
				((Robot *) by)->increment_training_counter();
			};
			if (new_state != state) doing(EXPANDER_CHANGED_STATE_VIA_KEYBOARD,by);
			if (held()) turn_on(by);
		} else {
			boolean state_cycling = FALSE; // unless proven otherwise -- new on 151002
			if (key == tt_run_character) { // was ' ' prior to 070502
				if (held()) {
					toggle_on(by);
					record_action = FALSE; // since it'll be used and that'll be recorded -- updated on 271102 to not even emit the space bar event
				} else {
					// convenient but I've seen kids confused by this
					new_state++;
					state_cycling = TRUE;
					did_button_cycle(by);                  
				};
			} else if (key == '+') {
				new_state++;
				state_cycling = TRUE;
				did_button_cycle(by);
			} else if (key == '-') {
				if (new_state == EXPANDER_BIGGER) {
					new_state = EXPANDER_NARROWER;
				} else {
					new_state--;
				};
				state_cycling = TRUE;
				did_button_cycle(by);
			} else if (key == tt_stop_character) { // new on 190799
				turn_off(by);
			} else {
				record_key = FALSE;
				if (showing_on_a_screen()) play_sound(PLOP_SOUND); // condition added 170502
				key = 0; // new on 080400 since don't want to use control-i here
			};
			if (state_cycling && tt_speak_button_name && by != NULL && by->kind_of() != ROBOT) { // programmer or robot in training
				stop_speaking(); // new on 291003 so if still describing the previous state that is aborted
				speak(IDS_EXPANDER_STATE0+new_state%EXPANDER_LAST_STATE); // new on 240902 -- updated to new_state on 291003
				// updated on 260105 to %EXPANDER_LAST_STATE since otherwise can speak wrong thing (see issue 624)
//				speak(IDS_EXPANDER_STATE0+state); // new on 240902
			};
		};
		if (record_action && record_key) {
			record_equivalent_keyboard_event(state,new_state,tt_expander_keyboard_accelerators,by,floor,this);
			record_action = FALSE;
//       record_event(KEYBOARD_CHARACTER,by,floor,this,TRUE,key);
		};
		set_state(new_state,by);
		if (is_on()) repeating_sound();
	};
//	completed(the_action_status);
   // commented out the above for the following on 080400
	// added conditions on 160801 so don't record space since turning on and off is handled specially for Pumpy
   // tt_run_character was ' ' prior to 070502
   return(Sprite::respond_to_keyboard(key,extended,robot_selected,by,(record_action && key != tt_run_character),original_recipient,size_constrained_regardless)); // the_action_status replaced action_status on 160801 so robots can use Pumpy
};

void Expander::set_state(int new_state, Sprite *by) {
   if (new_state != state) {
      state = (char) (new_state%EXPANDER_LAST_STATE);
      update_button();
      // moved on 151002 so only when cycling through states
		//if (tt_speak_button_name && by != NULL && by->kind_of() != ROBOT) { // programmer or robot in training
		//	speak(IDS_EXPANDER_STATE0+state); // new on 240902
		//};
	};
};

boolean Expander::used_enough() { // new on 010802 that if in used_once_per_click() mode and enough time has passed then turn off
   return(used_once_per_click() && (tt_current_time-use_start) > tt_expander_usage_maximum);
};

boolean Expander::is_still_on() { // on 270902 tried this but is turned off after a while unless mouse is down anyway
   //if (!on_flag) return(FALSE);
   //if (tt_expander_usage_maximum > 0) {
   //   return(virtual_right_button(tt_current_mouse_button)); // only if mouse button is (still) down
   //};
   return(on_flag);
};

boolean Expander::used_once_per_click() { // new on 310702
   // enabled and right button isn't being held down
   return(tt_expander_usage_maximum > 0 && !virtual_right_button(tt_current_mouse_button));
};

char Expander::button_label() {
   const_string labels = S(IDS_EXPANDER_KEY_COMMANDS);
   return(labels[state]);
};

void Expander::repeating_sound() {
   if (!showing_on_a_screen()) return; // moved here for generality on 260902
   short int new_sound_id;
   if (state < EXPANDER_SMALLER) { // some kind of expansion
      new_sound_id = HAND_PUMP_SOUND;
   } else {
      new_sound_id = DEFLATING_SOUND;
   };
   if (new_sound_id != sound_id) {
      stop_sound_id(sound_id);
      sound_id = new_sound_id;
      // as of 141002 doesn't repeat if going to Good Size (if limited duration will be turned off)
		// removed ,2 on 070604 since caused repeating
      play_sound(new_sound_id,(state != EXPANDER_GOOD_SIZE)); // condition added 170502 - !used_once_per_click() replaced TRUE on 310702 -- restored 260902
   };
};

const millisecond pump_use_quantum = 100; // 100ms for each unit so 25.5 seconds max

char encode_duration(millisecond duration) { // caller as of 280704 supplies duration -- and is no longer a method of Expander
//   millisecond duration = tt_current_time-use_start;
   // commented out the following (without even trying it) because this way the robot would go for the max but while training 
   // it uses tt_millisecond delta
   //if (duration > tt_expander_usage_maximum && used_once_per_click()) { // new on 070802 to keep duration to maximum
   //   duration = tt_expander_usage_maximum;
   //};
	if (duration < pump_use_quantum) {
		return(-127); // was 1 prior to 280704
	};
   int result = duration/pump_use_quantum;
   if (result > 255) result = 255;
   return((char) (result-128));
};

millisecond decode_duration(char encoding) {
   millisecond result = encoding+128;
   result *= pump_use_quantum;
   return(result);
};

void Expander::turn_on(Sprite *by) {
   if (is_on()) return;
   use_start = tt_current_time;
   Tool::turn_on(NULL); // turn on even if being run by a robot
#if TT_DIRECT_INPUT
   if (by != NULL && !by->still_running()) play_force_feedback_effect("Pump");
#endif
};

void Expander::turn_off(Sprite *by) {
   if (!is_on()) return;
   if (by != NULL) {
      record_event(EXPANDER_APPLY,by,floor,last_subject,FALSE,encode_duration(tt_current_time-use_start));
   } else {
      action_completed(); // strange to call this when no "by"
   };
   use_start = 0;
	last_subject = NULL; // added 160200
   Tool::turn_off(NULL);
#if TT_DIRECT_INPUT
   if (by != NULL && !by->still_running()) stop_force_feedback_effect();
#endif
};

Sprite *Expander::used(Sprite *subject, Sprite *by,
							  boolean button_unchanged,
							  boolean record_action, millisecond duration,
							  boolean modified, Sprite *original_subject) {
//   if (tt_new_input_scheme) {
      modified = FALSE; // 3rd button used differently now
      if (!button_unchanged && (by == NULL || !by->still_running())) { // !is_kind_of(by,ROBOT)) {
         respond_to_keyboard(tt_run_character,FALSE,FALSE,by,record_action,original_subject); // tt_run_character was ' ' prior to 070502
//         record_action = FALSE; // recorded as a ' '
			if (!is_on()) {
				return(this); // just turned it off
			};
      };
//   };
	if (duration > 0) { 
		// should be a robot who wants to do this for duration
		if (!is_on()) {
			if (showing_on_a_screen()) {
				// added showing_on_a_screen condition on 160801 so that robot doesn't take such a long time when not watched
				turn_on(by);
//				action_status = the_action_status;
            set_saved_by(by); // replaces the above on 171102
				do_after(duration,this,TURN_OFF_CALLBACK);
			} else {
				by->completed();
			};
		};
   } else {
      ::completed(by); // why not the_action_status?
   };
   if (!is_on()) {
      if (button_unchanged) {
		   flush_input_events(); // so I can hold the button down
		   if (subject != last_subject) {
				// probably slipped off subject while shrinking it
        		if (record_action) {
					record_event(EXPANDER_APPLY,by,floor,subject,FALSE,encode_duration(tt_current_time-use_start));
				};
//			   last_subject = subject;
//            set_being_used(FALSE);
			   return(this); // do nothing
		   };
	   };
   };
   if (subject == NULL) {
		if (state >= EXPANDER_SMALLER && last_subject != NULL) { 
			// new on 070300 - too hard to shrink things since they move off from tool's selection point
			// so now they shrink anyways
			subject = last_subject;
		} else {
			if (!is_on() && showing_on_a_screen()) play_sound(PLOP_SOUND); 
			// duration > 0 condition added 170502 and replaced with showing_on_a_screen on 010802 since duration is for robots
//       last_subject = NULL; // commented out on 160200 since too easy to slip off subject and then change size of wrong thing
//       set_being_used(FALSE);
			return(this);
		};
   };
/*
		if (!button_unchanged) {
			// don't flip modes if just slipped off subject
			if (normal_flag) {
				play_sound(SPIRAL_IN_SOUND);
				set_parameter(EXPANDER_READY_TO_PUMP);
			} else {
				play_sound(SPIRAL_OUT_SOUND);
				set_parameter(EXPANDER_READY_TO_SUCK);
			};
			normal_flag = (flag) !normal_flag;
		};
	if (subject == last_subject && !tt_new_input_scheme) {
	  if (!button_unchanged) {
			if (!modified && !modified_previously) {
				normal_flag = (flag) !normal_flag;
				stop_sound(); // so next sound is right
			} else if (modified) {
				normal_flag = TRUE; // control button always is used while expanding
				if (modified_previously) {
					grow_just_width_flag = (flag) !grow_just_width_flag;
				};
			};
		};
	} else 
*/
	if (!button_unchanged) {
		 // different subject and button changed so reset defaults
		 normal_flag = TRUE;
		 grow_just_width_flag = FALSE;
	};
   modified_previously = (flag) modified;
   if (last_subject == NULL) { // conditional added 160200
		last_subject = subject;
	} else if (last_subject != subject) { // slipped over to a new subject - do nothing (new on 160200)
		set_parameter(EXPANDER_WAITING_NEW);
		return(this);
	};
   floor->give_item_priority(subject);
//   set_being_used(TRUE);
   if (state == EXPANDER_GOOD_SIZE) { // animate subject to good size
      city_coordinate good_width,good_height;
		// as of 081199 don't use Toolbox to calculate good size if doing puzzles
      subject->good_size(good_width,good_height,tt_system_mode==PUZZLE?NULL:tt_toolbox,TRUE); // good size for use of Pumpy
      city_coordinate subject_width = subject->current_width();
		city_coordinate subject_height = subject->current_height();
		const millisecond duration = 2000;
		// new on 070300 to keep centered while making it a good size
		city_coordinate new_llx = subject->current_llx()+(subject_width-good_width)/2;
		city_coordinate new_lly = subject->current_lly()+(subject_height-good_height)/2;
		if (showing_on_a_screen()) { // new on 260804 to not let it disappear off screen
			if (new_llx >= ideal_screen_width-tile_width) { 
				new_llx = ideal_screen_width-tile_width;
			} else if (new_llx+good_width < tile_width) {
				new_llx = tile_width;
			};
			if (new_lly >= ideal_screen_height-tile_height) { 
				new_lly = ideal_screen_height-tile_height;
			} else if (new_lly+good_height < tile_height) {
				new_lly = tile_height;
			};
		};
      subject->animate_size_and_location(good_width,good_height,new_llx,new_lly,											  
												     duration,subject,SET_FAVORITE_SIZE_AND_LOCATION_CALLBACK);
      if (good_width > subject_width) {
         set_parameter(EXPANDER_PUMPING);
      } else {
         set_parameter(EXPANDER_SUCKING);
      };
      do_after(duration,this,WAIT_CALLBACK);
		if (record_action) { // new on 280704
			record_event(EXPANDER_APPLY,by,floor,subject,FALSE,encode_duration(duration));
		};
//      if (tt_log_version_number > 11) {
         turn_off(this);
			if (showing_on_a_screen()) { // condition new on 160801
				if (good_width > subject->current_width()) {
					play_sound(HAND_PUMP_SOUND);
				} else {
					play_sound(DEFLATING_SOUND);
				};
			};
//      };
      if (!by->still_running()) simulate_mouse_up();
		by->completed(); // new on 280704
      return(this);
   };
   if (used_enough() && by->kind_of() != ROBOT) { // new on 010802 -- added ROBOT test on 280704 since they have set a timer to turn off
      turn_off(by); // prior to 280704 was this but that breaks recording robot's use of Pumpy
   } else if (normal_flag && state < EXPANDER_SMALLER) {
		 if (showing_on_a_screen()) { // condition new on 160801
			 if (!is_on()) play_sound(HAND_PUMP_SOUND);
			 // following moved into conditional on 020205
			 if (parameter != EXPANDER_PUMPING || cycle_stopped()) {
//				 if (showing_on_a_screen()) { // condition new on 160801
				 if (is_on()) play_sound(HAND_PUMP_SOUND);
//				 };
				 // sucking and pumping don't cycle so force reset here
				 set_parameter(EXPANDER_PUMPING,TRUE,TRUE);
				 do_after(total_cycle_duration(),this,WAIT_CALLBACK);
			 };
			 grow(subject,tt_screen,modified);
		 } else {
			 UnwindProtect<millisecond> set(tt_millisecond_delta,duration); // the entire thing happens at once when off screen
			 grow(subject,tt_screen,modified);
		 };       
	 } else {
		if (showing_on_a_screen()) { // condition new on 160801
			if (!is_on()) play_sound(DEFLATING_SOUND);
			// following moved into conditional on 020205
			if (parameter != EXPANDER_SUCKING || cycle_stopped()) {
//				if (showing_on_a_screen()) { // condition new on 160801
				if (is_on()) play_sound(DEFLATING_SOUND);
//				};
				// sucking and pumping don't cycle so force reset here
				set_parameter(EXPANDER_SUCKING,TRUE,TRUE);
				do_after(total_cycle_duration(),this,WAIT_CALLBACK);
			};
			shrink(subject,tt_screen);
		} else {
			 UnwindProtect<millisecond> set(tt_millisecond_delta,duration); // the entire thing happens at once when off screen
			 shrink(subject,tt_screen);
		};	
	};
	subject->save_size(); // at least for pictures keep new size
   subject->set_favorite_size_and_location();
	return(this);
};

void Expander::one_cycle_done() {
   if (is_on()) {
      reset_cycle();
   } else {
	   set_parameter(EXPANDER_WAITING_NEW);
      if (showing_on_a_screen()) stop_repeating_sound();
   };
};

//const double log_of_growth = log(2.0);
const long time_to_double = 6000; // was 2000 but grew too fast

void Expander::grow(Sprite *selection, Screen *screen, boolean modified) {
//  const float growth_factor = exp(log_of_growth*tt_delta);
  city_coordinate current_width = selection->current_width();
  city_coordinate current_height = selection->current_height();
  TTRegion viewing_region = *(screen->viewing_region());
//  city_coordinate view_width = viewing_region.max_x-viewing_region.min_x;
//  city_coordinate max_width = view_width;
//  max_width *= 2;
//  city_coordinate max_height = selection->height_given_width(max_width);
//  selection->adjust_max_size(max_width,max_height);
//  if (current_width >= max_width && current_height >= max_height) {
//	  // already too big
//	  return;
//  };
  city_coordinate new_width, new_height; 
  if (modified || state != EXPANDER_BIGGER) {//control_button_down()) {
	  if (grow_just_width_flag || state == EXPANDER_WIDER) {
		  new_width = grow_value(current_width,tt_millisecond_delta,time_to_double);
		  if (new_width == current_width && showing_on_a_screen()) {
			  	play_sound(PLOP_SOUND); // moved here from grow_value on 260804
		  };
		  new_height = current_height;
//		  user_did(EXPANDER,EXPANDER_GREW_HORIZONTALLY);  
	  } else {
		  new_width = current_width;
		  new_height = grow_value(current_height,tt_millisecond_delta,time_to_double);
		  if (new_height == current_height && showing_on_a_screen()) {
			  	play_sound(PLOP_SOUND); // moved here from grow_value on 260804
		  };
//		  user_did(EXPANDER,EXPANDER_GREW_VERTICALLY);
	  };
  } else {
	  doing(EXPANDER_GREW_ITEM);
	  new_width = grow_value(current_width,tt_millisecond_delta,time_to_double);
	  if (new_width == current_width && showing_on_a_screen()) {
			play_sound(PLOP_SOUND); // moved here from grow_value on 260804
	  };
	  new_height = grow_value(current_height,tt_millisecond_delta,time_to_double);
	  if (new_width > current_width && new_height == current_height && showing_on_a_screen()) {
		  play_sound(PLOP_SOUND); // moved here from grow_value on 260804
	  };
  };
  // split apart set_size and set_location on 260804 since this may encounter some maximum values
  selection->set_size(new_width,new_height,EXPANDER_CHANGED_IT);
  new_width = selection->current_width(); // value it was really set to
  new_height = selection->current_height();
  city_coordinate current_llx, current_lly;
  selection->lower_left_corner(current_llx,current_lly);
  city_coordinate new_llx = current_llx-(new_width-current_width)/2;
  city_coordinate new_lly = current_lly-(new_height-current_height)/2;
  if (new_llx < viewing_region.min_x && current_llx >= viewing_region.min_x) { 
	  // current_llx was llx prior to 060202 but that is Pumpy's coordinate
	  // don't go partially offscreen unless already so
 	  new_llx = viewing_region.min_x;
  } else if (new_llx+new_width > viewing_region.max_x && 
				 current_llx+current_width <= viewing_region.max_x) { // ditto 060202
	  new_llx = viewing_region.max_x-new_width;
  };
  if (new_lly < viewing_region.min_y && current_lly >= viewing_region.min_y) { // ditto 060202
	  new_lly = viewing_region.min_y;
  } else if (new_lly+new_height > viewing_region.max_y &&
				 current_lly+current_height <= viewing_region.max_y) {
	  new_lly = viewing_region.max_y-new_height;
  };
#if TT_DEBUG_ON
  if (tt_debug_mode == 60504) {
	  tt_error_file() << "Pumpy growing the size to  " << new_width << "x" << new_height 
					  << " and position to " << new_llx << "," << new_lly << " on frame " << tt_frame_number << endl;
  };
#endif
  selection->move_to(new_llx,new_lly,EXPANDER_CHANGED_IT); // simplified 260804
  if (selection->pointer_to_leader() != NULL) {
	  selection->pointer_to_leader()->follower_changed(selection);
  };
//  selection->unsave_size();
};

const long time_to_halve = 6000; // was 2000;

void Expander::shrink(Sprite *selection, Screen *screen) {
//  const float growth_factor = exp(-log_of_growth*tt_delta);
  city_coordinate current_width = selection->current_width();
  city_coordinate current_height = selection->current_height();
  city_coordinate new_width = current_width;
  city_coordinate new_height = current_height;
  if (!is_on() || state == EXPANDER_SMALLER) {
	  new_width = shrink_value(current_width,tt_millisecond_delta,time_to_halve);
	  new_height = shrink_value(current_height,tt_millisecond_delta,time_to_halve);
  } else if (state == EXPANDER_NARROWER) {
     new_width = shrink_value(current_width,tt_millisecond_delta,time_to_halve);
  } else {
     new_height = shrink_value(current_height,tt_millisecond_delta,time_to_halve);
  };
  TTRegion viewing_region = *(screen->viewing_region());
  city_coordinate min_width, min_height;
  viewing_region.region_size(min_width,min_height);
  min_width /= 50;
  min_height /= 50;
  if (new_width < min_width || new_height < min_height) { // was && prior to 280403 - thanks Leonel
	  stop_sound(FALSE); 
	  return;
  };
//  const city_coordinate new_height =
//     (current_height*new_width)/current_width;
  city_coordinate current_llx, current_lly;
  selection->lower_left_corner(current_llx,current_lly);
  city_coordinate new_llx, new_lly;
  city_coordinate delta_x = new_width-selection->current_width();
  city_coordinate delta_y = new_height-selection->current_height();
  if (current_llx+current_width > viewing_region.max_x) { // off right edge
	  new_llx = current_llx;
  } else {
	  new_llx = current_llx-delta_x/2;
  };
  if (new_llx+new_width < viewing_region.min_x) { // will be off left side -- added new_width on 181204 since otherwise can go offscreen due to this
	  new_llx -= delta_x/2;
  };
  if (current_lly+current_height > viewing_region.max_y) { // off top
	  new_lly = current_lly;
  } else {
	  new_lly = current_lly-delta_y/2;
  };
  if (new_lly+new_height < viewing_region.min_y) { // will be off bottom added new_height on 181204 since otherwise can go offscreen due to this
	  new_lly -= delta_y/2;
  };
#if TT_DEBUG_ON
  if (tt_debug_mode == 60504) {
	  tt_error_file() << "Pumpy shrinking the size to " << new_width << "x" << new_height 
						   << " and position to " << new_llx << "," << new_lly << " on frame " << tt_frame_number << endl;
  };
#endif
  selection->set_size_and_location(new_width,new_height,
											  new_llx,new_lly,
											  EXPANDER_CHANGED_IT);
  if (selection->pointer_to_leader() != NULL) {
	  selection->pointer_to_leader()->follower_changed(selection);
  };
//  selection->unsave_size();
//  user_did(EXPANDER,EXPANDER_SHRUNK_ITEM);
};

boolean Expander::set_parameter(short int new_parameter, boolean warn,
                                boolean reset_cycle_regardless) {
   if (button != NULL) {
       button->set_visible((new_parameter < EXPANDER_PUMPING ||
                            new_parameter >= EXPANDER_BREATHING));
   };
   return(Tool::set_parameter(new_parameter,warn,reset_cycle_regardless));
};

#if TT_XML

//xml_element *Expander::xml_create_element(xml_document *document) {
//   return(create_xml_element(xml_tag(),document));
//};

void Expander::xml(xml_element *element, xml_document *document) {
 //  xml_set_attribute(element,L"StateCode",(int) state); // moved up to Tool::xml on 180703
   if (toolbox_tool(this)) {
//      xml_append_child(create_xml_element(L"FriendOfTooly",document),element); 
      xml_set_attribute(element,L"LivesInTooly",1); // new on 171102
   };
   if (tt_dump_reason != DUMPING_TO_CLIPBOARD) { // new on 151102
      xml_set_attribute(element,L"GoalX",goal_x);
      xml_set_attribute(element,L"GoalY",goal_y);
   };
   Tool::xml(element,document);
};

boolean Expander::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
      //case FRIEND_OF_TOOLY_TAG: { // made obsolete on 171102
      //   if (tt_loading_a_city) {
      //      if (tt_toolbox == NULL) {
				  // tt_toolbox = new Toolbox();
			   //}; 
			   //Expander *old_expander = tt_toolbox_expander; 
			   //set_toolbox_expander(this);
			   //if (old_expander != NULL) {
      //         FavoriteSizeAndLocation f;
      //         old_expander->current_favorite(f); // new on 171102 to transfer favorite settings
      //         set_favorite(f);
				  // old_expander->destroy();
			   //};
      //   };
      //   return(TRUE);
      //                          };
      case NO_MORE_TAGS:
         goal_x = xml_get_attribute_int(node,L"GoalX",goal_x); // not clear what default should be
         goal_y = xml_get_attribute_int(node,L"GoalY",goal_y);
         // and the following
     default:
         return(Tool::handle_xml_tag(tag,node));
   };
};

#endif

boolean Expander::dump(output_stream &stream, boolean just_prepare, boolean is_controlee, boolean on_nest) {
   if (!Tool::dump(stream,just_prepare,is_controlee,on_nest)) return(FALSE);
	dump_if_not_null(stream,comment);
   stream.put((unsigned char) state);
	stream.put(toolbox_tool(this)); // new on 050100
	return(TRUE);
};

Expander *load_bike_pump(InputStream *stream, int notebook_version, NaturalLanguage language, long index) {
   Expander *pump = new Expander(0,0);
	associate_shared_item(index,pump);
	if (notebook_version >= first_version_to_save_copied_nests) {
		pump->set_comment(load_item(stream,notebook_version,language));
	};
   pump->set_state(stream->get());
	if (notebook_version >= first_version_to_save_programmer_and_remotes_accurately) { // new on 050100
		boolean friend_of_tooly = stream->get(); 
		if (friend_of_tooly && tt_loading_a_city && !tt_loading_to_ignore) { // second condition new on 181200 to protect tools from bad encodings
			if (tt_toolbox == NULL) {
				tt_toolbox = new Toolbox();
				tt_toolbox->initialize();
			}; 
			Expander *old_expander = tt_toolbox_expander; // updated 261000
			set_toolbox_expander(pump);
			if (old_expander != NULL) {
				old_expander->destroy();
			};
		};
	};
   return(pump);
};

boolean describe_bike_pump(InputStream *pad_in, output_stream &text_out, int version_number, Article article) {
	if (version_number >= first_version_to_save_copied_nests) {
		describe_comment_encoding(pad_in,text_out,version_number,EXPANDER);
	};
   ExpanderState state = (ExpanderState) pad_in->get(); // describe this later
   print_type(EXPANDER,text_out,article);
   text_out << space() << S(IDS_WHO_IS_READY_TO,"")
            << space() << S(IDS_MAKE_THINGS,"")  
            << space() << S(IDS_EXPANDER_STATE0+state)
				<< S(IDS_MAKE_THINGS2,"")
				<< S(IDS_WHO_IS_READY_TO2,""); // new on 270100
	if (version_number >= first_version_to_save_programmer_and_remotes_accurately) { // new on 050100
		pad_in->get(); // ignore for now whether this is a "friend" of Tooly
	};
   return(TRUE);
};

void Expander::describe(output_stream &message, Article ) {
	describe_comment(message);
	if (tt_histories[EXPANDER] == 0) {
		message << S(IDS_EXPANDER_DESCRIPTION);
	} else {
		message << S(IDS_EXPANDER_DESCRIPTION1);
	};
	message << space() << S(IDS_WHO_IS_READY_TO,"")
           << space() << S(IDS_MAKE_THINGS,"")  
           << space() << S(IDS_EXPANDER_STATE0+state)
			  << S(IDS_MAKE_THINGS2,"")
			  << S(IDS_WHO_IS_READY_TO2,""); // new on 270100
};

void Expander::generate_help(output_stream &message, Sprite *tool_in_hand,
                             Sprite *selection) {
	if ((!did(EXPANDER_GREW_ITEM) && tt_help_counter == 0)
		 || tt_help_counter == 1) {
		tt_help_history_action = EXPANDER_GREW_ITEM;
      describe_pointing(message,tool_in_hand,selection);
		message << S(IDS_EXPANDER_GREW_ITEM_HELP);
//      if (!tt_new_input_scheme) {
//	      message << S(IDS_EXPANDER_GREW_ITEM_HELP2);
//	      describe_right_button(message);
//	      message << space() << S(IDS_EXPANDER_GREW_ITEM_HELP3) << space();
//      } else {
//			message << S(IDS_EXPANDER_GREW_ITEM_HELP3) << space();
//		};
		string first_part = S(IDS_EXPANDER_GREW_ITEM_HELP3,"");
		if (first_part[0] != NULL) {
			message << first_part << space();
			if (tt_help_counter > 0 || selection == this || 
				 selection == NULL || tool_in_hand == NULL) { 
				message << S(IDS_EXPANDER_SELECTION_HELP);
			} else {
				selection->describe_type(message,DEFINITE_ARTICLE);
			};
			message << space() << S(IDS_EXPANDER_STATE0+state) << end_sentence();
		} else { // new on 080100
			message << S_capitalize_first(IDS_EXPANDER_STATE0+state) << space()
				     << S(IDS_EXPANDER_GREW_ITEM_HELP3_2,"") << space();
			if (tt_help_counter > 0 || selection == this || selection == NULL || tool_in_hand == NULL) { 
				message << S(IDS_EXPANDER_SELECTION_HELP);
			} else {
				selection->describe_type(message,DEFINITE_ARTICLE);
			};
		};
      message << S(IDS_TURN_EXPANDER_ON_HELP);
//      if (tt_new_input_scheme) 
		describe_space_bar_alternatives(message);
	} else if ((!did(EXPANDER_CHANGED_STATE_VIA_BUTTON) &&
               tt_help_counter == 0)
		        || tt_help_counter == 2) {
       tt_help_history_action = EXPANDER_CHANGED_STATE_VIA_BUTTON;
       message << S(IDS_EXPANDER_CHANGED_STATE_VIA_BUTTON_HELP);
	} else if ((!did(EXPANDER_CHANGED_STATE_VIA_KEYBOARD) &&
               tt_help_counter == 0)
		        || tt_help_counter == 3) {
       tt_help_history_action = EXPANDER_CHANGED_STATE_VIA_KEYBOARD;
       message << S(IDS_EXPANDER_CHANGED_STATE_VIA_KEYBOARD_HELP1)
               << space() << S(IDS_EXPANDER_KEY_COMMANDS) << end_sentence() 
               << S(IDS_EXPANDER_CHANGED_STATE_VIA_KEYBOARD_HELP2);
	} else if ((!did(EXPANDER_CALLED) && tt_help_counter == 0)
		        || tt_help_counter == 4) {
		tt_help_history_action = EXPANDER_CALLED;
		message << S(IDS_EXPANDER_CALLED_HELP);
		describe_hand(message);
		message << S(IDS_EXPANDER_CALLED_HELP2);
		message << S(IDS_EXPANDER_CALLED_HELP3);
	} else if ((!did(TYPE_HELP_GIVEN) && tt_help_counter == 0)
		        || tt_help_counter == 6) {
		message << S(IDS_EXPANDER_HELP_GIVEN_HELP)
				  << S(IDS_EXPANDER_HELP_GIVEN_HELP2);
		no_help_interruptions();
		doing(TYPE_HELP_GIVEN);
	};
};

Sprite *Expander::selected(SelectionReason reason, Sprite *by) {
	if (reason == GRASPED && 
		 (parameter == EXPANDER_INERT || parameter == EXPANDER_MORPHING_TO_INERT)) {
		set_parameter(EXPANDER_MORPHING_TO_LIFE);
      do_after(total_cycle_duration(),this,WAIT_CALLBACK);
	};
	normal_flag = TRUE; // reset to expand first
   grow_just_width_flag = FALSE;
   last_subject = NULL;
	return(Sprite::selected(reason,by));
};

Sprite *Expander::released(Sprite *by, boolean top_level, SpecialDrop special_drop) {
//   finish_instantly(); // if still pumping finish at once
   check_cycle_callback(TRUE);  // let animations continue...
   boolean was_called_since_last_release = called_since_last_release;
   if (was_called_since_last_release) {
      jump_to_favorite_location();
   };
   millisecond size_change_duration = 2000;
   if (!by->still_running()) {
      animate_to_favorite_size(size_change_duration);
   };
	Sprite *result = Tool::released(by,top_level,special_drop);
   if (!was_called_since_last_release) {
	   if (parameter != EXPANDER_INERT) {
         finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
		   set_parameter(EXPANDER_MORPHING_TO_INERT);
         if (by->kind_of() == PROGRAMMER) {
            do_after(max(size_change_duration,total_cycle_duration()),
                     this,SET_FAVORITE_SIZE_AND_LOCATION_CALLBACK);
         };
      };
	};
   normal_flag = TRUE;
	return(result);
};

void Expander::adjust_grasp(city_coordinate &adjustment_x,
									 city_coordinate &adjustment_y) {
	adjustment_x = tile_width/-2;
	adjustment_y = height/10;
};

void Expander::move_to_hand() { // Programmer_At_Floor *the_programmer) {
	if (inside_vacuum()) {
		say(IDS_PUMP_IN_VACUUM_WHEN_CALLED);
		return;
	};
   if (//!tt_running_old_demos && 
       tt_system_mode != PUZZLE) {
      called_since_last_release = TRUE;
   };
//	if (floor == NULL) return; // hasn't left ToolBox yet
	doing(EXPANDER_CALLED);
	if (running_to_hand() || floor == NULL) { // called twice so hurry up
		tt_finish_all_animations = TRUE;
//		tt_programmer->pointer_to_appearance()->finish_instantly();
//		finish_instantly();
//		running_to_hand_flag = FALSE; // commented out on 221105 since too early to say this
	} else {
		running_to_hand_flag = TRUE;
		set_parameter(EXPANDER_MORPHING_TO_LIFE);
		set_priority(floor->next_item_priority());
//		programmer = the_programmer;
		millisecond duration = default_duration(total_cycle_duration());
      if (!tt_screen->inside_rule_body()) {
		   animate_to_size_percent(shrink_percent_from_640x480_screen_size(100),duration);
      };
		do_after(duration,this,JUMP_TO_GOAL_CALLBACK);
	};
};

void Expander::jump_to_goal() {
//	if (vertical_distance == 0 && horizontal_distance == 0) {
//		get_grasped();
//	} else {
		set_parameter(EXPANDER_JUMPING);
		millisecond duration = default_duration(total_cycle_duration());
		// after a 1/3 of the cycle move for 1/3 and then stay
		do_after(duration/3,this,REALLY_JUMP_CALLBACK);
//	};
};

void Expander::really_jump() {
	TTRegion hand_region;
	tt_programmer->pointer_to_appearance()->collision_region(hand_region);
	city_coordinate goal_x = hand_region.min_x-width/2;
	city_coordinate goal_y = hand_region.min_y-height/2;
//	city_coordinate horizontal_distance = goal_x-llx;
//	city_coordinate vertical_distance = goal_y-lly;
	millisecond duration = default_duration(total_cycle_duration()/3);
	city_coordinate arm_llx, arm_lly;
	tt_programmer->pointer_to_appearance()->lower_left_corner(arm_llx,arm_lly);
   city_coordinate x_offset = goal_x-arm_llx;
   city_coordinate y_offset = goal_y-arm_lly;
	animate_location_to(tt_programmer->pointer_to_appearance(),duration,NULL,CALLBACK_NOTHING,x_offset,y_offset);
	do_after(duration,this,EXPANDER_GET_GRASPED_CALLBACK);
	if (duration > 0) play_sound(BOING1_SOUND); // condition added 170502
};

boolean Expander::get_grasped() {	
	if (Tool::get_grasped()) {
//#if TT_BETA
//      if (beta4_images()) {
//		   set_parameter(EXPANDER_WAITING);
//         return(TRUE);
//      };
//#endif
		running_to_hand_flag = FALSE; // moved here on 221105
      set_parameter(EXPANDER_WAITING_NEW);
		return(TRUE);
	} else {
		set_parameter(EXPANDER_MORPHING_TO_INERT);
		return(FALSE);
	};
};

void Expander::stop_running() {
	Tool::stop_running();
	morph_to_inert();
};

void Expander::dramatic_entrance(millisecond duration) {
   city_coordinate goal_x = llx;
   city_coordinate goal_y = lly;
   city_coordinate goal_width = width;
   city_coordinate goal_height = height;
   // start small and somewhere off the top of the screen
   set_size_and_location(100,100,my_random(ideal_screen_width),21*tile_height);
   animate_size(goal_width,goal_height,duration);
   jump_to(goal_x,goal_y);
};   
   
void Expander::jump_to_favorite_location() {
	if (tt_screen->inside_thought_bubble()) {
		jump_to(default_favorite_llx,default_favorite_lly); // new on 011100 that in thought bubble objects are at standard default
	} else {
		jump_to(favorite_llx,favorite_lly);
	};
};

void Expander::jump_to(city_coordinate x, city_coordinate y) {
	millisecond duration = default_duration(total_cycle_duration());
	// after a 1/3 of the cycle move for 1/3 and then stay
	if (duration > 0) set_parameter(EXPANDER_JUMPING);
	goal_x = x;
	goal_y = y;
	do_after(duration/3,this,REALLY_JUMP_TO_CALLBACK);
};

void Expander::really_jump_to() {
	millisecond duration = default_duration(total_cycle_duration()/3);
	animate_location(goal_x,goal_y,duration);
	if (!tt_finish_all_animations && duration > 0) play_sound(BOING1_SOUND); // duration condition added 170502
	do_after(duration,this,EXPANDER_MORPH_INERT_CALLBACK);
};
	
void Expander::morph_to_inert() {
	if (tt_finish_all_animations) {
		set_parameter(EXPANDER_INERT);
	} else if (held_status == NOT_HELD) {
		set_parameter(EXPANDER_MORPHING_TO_INERT);
		if (tt_system_mode != PUZZLE) set_priority(max_long-2); // toolbox has max_long-1 (-1 as of 111204)
	};
};

void Expander::active_region(TTRegion &region) {
	// this computes a stable region independent of cycling
	full_region(region); 
	region.max_x = (9*region.min_x+region.max_x)/10-true_x_offset(); // just leftmost 1/10th
	region.min_x -= true_x_offset();
   int y_offset = 16; // 4/5ths of way towards min
//#if TT_BETA
//   if (beta4_images()) y_offset = 15;
//#endif
	city_coordinate center_y = (y_offset*region.min_y+5*region.max_y)/20-true_y_offset();
	region.min_y = center_y;
	region.max_y = center_y+height/20;
};

Vacuum::Vacuum(city_coordinate x, city_coordinate y) :
	Tool(VACUUM_SPRITE,x,y,VACUUM),
	sucked_up(NULL),
	blanked(NULL),
	restoring(NULL),
	sucking_up(NULL),
//	restoring_done(FALSE),
//	saved_screen(NULL),
//	action_status(NULL),
	last_sucked_llx(0),
	last_sucked_lly(0),
   item_width(2*tile_width), // something in case restoring from "disk"
	only_first_timers(NULL),
	restoring_contents(FALSE),
	saved_sucked_up(NULL), // these were missing prior to 150603 and broke time travel
	saved_blanked(NULL)
	{
   state = VACUUM_SUCK;
//	set_priority_function_of_lly(FALSE);
   city_coordinate button_width, button_height, button_llx, button_lly;
   good_button_size_and_location(button_width,button_height,button_llx,button_lly);
   button = new Button(VACUUM_BUTTON_SPRITE,button_llx,button_lly,
                       'S',priority()-1,
                       button_width,button_height,
                       tt_black);
   button->set_show_all(FALSE);
   add_follower(button);
   update_button();
};

boolean Vacuum::ok_to_delete() { // new on 060704 to deal with friends of Tooly getting destroyed (e.g. in a vacuum)
	if (ref_count == 2 && tt_toolbox_vacuum == this) {
		// only reference remaining is from Tooly so replace that with a fresh one
		if (tt_reincarnate_tools) {
			set_toolbox_vacuum(new Vacuum());
			initialize_toolbox_vacuum(TRUE);
		} else {
			set_toolbox_vacuum(NULL);
		};
	};
	return(Tool::ok_to_delete());
};

void Vacuum::good_button_size_and_location(city_coordinate &button_width,
                                           city_coordinate &button_height,
                                           city_coordinate &button_llx,
                                           city_coordinate &button_lly) {
   city_coordinate real_width = true_width();
   city_coordinate real_height = true_height();
   button_width = real_width/6;
   button_height = real_height/4;
   button_llx = llx+real_width/7;
   button_lly = lly+(9*real_height)/10;
};

void Vacuum::release_all_references() { // renamed on 130504
	if (releasing_all_references) { // new on 230305
		return;
	};
	// could break this up do use  -- noted on 110504
 //  if (preparing_for_deletion) return; // already done
	//preparing_for_deletion = TRUE; // new on 060204
  // moved the following earlier than sucked_up since otherwise get a spurious warning about a memory leak
   //if (// blanked != NULL && 
	  //  !tt_shutting_down) { // && !tt_shutting_down added on 150400 -- WHY???
	// changed conditional on 190103 
	if (tt_reclaiming_memory) { 
//		// added tt_resetting_or_destroying_city on 150603 since these guys will be destroyed anyway -- right? 
		// -- in any case time travel can get confused by this
//		if (tt_resetting_or_destroying_city) {
////			just_destroy_list(blanked); -- hopefully this isn't a leak - but it can be trash when resetting for time travel - commented out 150603
//		} else {
			destroy_sprites(blanked,this);
//		};
	};
	//} else {
	//	just_destroy_list(blanked); // new on 240101
	//};
	  // re-wrote the following 241000 since erasing incremented ref count so this should decrement it
//	  blanked->remove_all(); // was  prior to 2
//	  delete blanked;
//	  blanked = NULL;
  if (sucked_up != NULL) {
	  Sprites *remaining = sucked_up;
	  while (remaining != NULL) {
		  Sprite *current = remaining->first();
		  // could combine responsible_for & vacuum_should_not_delete into 1??
		  // if shuting down toolbox is deleted
		  if (current != NULL) { 
			  if (!current->destroy()) { // removed by on 060204
              // commented out on 190702 since can have a reference count from other objects in vacuum - e.g. a picture and its notebook of remotes
//				  // new on 251000 to fix a major memory leak - objects are protected from being blown up while in vacuum so have extra ref count
				  if (current->kind_of() != PROGRAM_PAD &&  // condition new on 080604 -- maybe only remote control notebooks need to be excepted here?
					  !current->destroy()) { // restored on 150103 since was a real leak and 
					  // is referenced by vacuum for undo as well as its original count of 1
					  // removed by on 060204
//					  if (by != NULL && by->kind_of() == ROBOT && ((Robot *) by)->version_number() 
						  // < first_robot_version_to_destroy_vacuum_contents_only_twice) {
//						  // commenting out the following broke existing robots since sometimes remote control notebook flew out and sometimes not
//						  current->destroy(by); // added one more check on 121200 - commented out on 230101 since is dangerous since can be held 
						  // by collision records or lots of other things
#if TT_DEBUG_ON
//					  if (current->kind_of() != PROGRAM_PAD) {
					  log("Warning: memory leak since something is inside a vacuum that is being destroyed and it won't be destroyed.");
					  current->print(tt_error_file());
					  tt_error_file() << endl;
//					  };
#endif
				  };
			  };
		  };
		  remaining->set_first(NULL); 
		  remaining = remaining->rest();
	  };
//  now has a private copy so don't bother
	  // can be duplicates if sucked up to implement undoable blanking
//	  remaining = sucked_up;
//	  while (remaining != NULL) {
//		  Sprites *next_remaining = remaining->rest();
//		  if ((next_remaining != NULL && 
//				 next_remaining->member_cons(remaining->first()) != NULL) ||
//				(sucking_up != NULL && 
//				 sucking_up->member_cons(remaining->first()) != NULL)) {
//			  // will be deleted later so don't delete it now	 
//			  remaining->set_first(NULL);
//		  };		
//		  remaining = next_remaining;
//	  };
	  delete sucked_up;
	  sucked_up = NULL;
  };
  if (sucking_up != NULL) {
	  sucking_up->remove_all();
	  delete sucking_up;
	  sucking_up = NULL;
  };
  if (only_first_timers != NULL) {
	  only_first_timers->remove_all();
	  delete only_first_timers;
	  only_first_timers = NULL;
  };
//  if (deleted != NULL) { // finally
//	  delete deleted;
//  };
  Tool::release_all_references(); 
};

Sprite *Vacuum::copy(boolean also_copy_offsets) {
//	city_coordinate center_x, center_y;
//	center_location(center_x,center_y);
	Vacuum *copy = new Vacuum(llx,lly);
	copy->set_size(width,height,COPY_INITIALIZATION);
   copy->set_state(state);
   if (sucked_up != NULL) {
      copy->initialize_sucked_up(sucked_up->copy());
   };
 //  copy->set_priority_fixed(priority_fixed_flag);
	//if (comment != NULL) {
	//	copy->set_comment(comment->copy());
	//};
   finish_making_copy(copy,also_copy_offsets);
	return(copy);
};

void Vacuum::initialize_sucked_up(Sprites *list) {
	destroy_sprites(sucked_up); // new on 070103
   sucked_up = list;
	Sprites *remaining = list;
	while (remaining != NULL) {
		Sprite *item = remaining->first();
		if (item != NULL) { // conditional new on 150400
			item->increment_ref_count(this); // new on 280300
			item->set_inside_vacuum(TRUE); // new on 080400 - important since this stops any robots inside that might otherwise run 
		};
		remaining = remaining->rest();
    };
};

void Vacuum::initialize_blanked(Sprites *list) {
	destroy_sprites(blanked); // new on 070103
   blanked = list;
	if (blanked != NULL) {
		blanked->increment_ref_count(); // new on 240103
	};
};

void Vacuum::is_inert() {
   if (!held()) {
      set_parameter(VACUUM_INERT);
   } else { // if (tt_log_version_number >= 8) {
      set_parameter(VACUUM_MORPHING_TO_LIFE,FALSE,FALSE);
   };
};

boolean Vacuum::set_parameter(short int new_parameter, boolean warn,
                              boolean reset_cycle_regardless) {
   if (button != NULL) {
       button->set_visible((new_parameter == VACUUM_INERT));
   };
   boolean result = Tool::set_parameter(new_parameter,warn,reset_cycle_regardless);
   if (//button != NULL &&
       new_parameter == VACUUM_MORPHING_TO_INERT) { //  && tt_log_version_number >= 8
      // so the above is triggered
      // why not just set the button visiblity after cycle_duration?
      // or check that things haven't changed in the meanwhile??
	   do_after(total_cycle_duration(),this,VACUUM_IS_INERT_CALLBACK);
   };
   return(result);
}; 

void Vacuum::save_contents() {
	// called when entering thought bubble
	// I hope this doesn't cause any problems with ref count of elements of sucked_up...
	saved_sucked_up = sucked_up;
	saved_blanked = blanked;
	sucked_up = NULL;
	blanked = NULL;
};

void Vacuum::restore_contents(Sprite *by) {
	// called when leaving thought bubble to restore vacuum state
//	if (sucked_up != NULL) {
//      sucked_up->remove_all(); // items themselves should be in working set of robot
//		delete sucked_up;
//	};
	// above didn't destroy things properly (robot didn't destroy items in working set) so the following does the trick (6/6/98)
	restoring_contents = TRUE;
	destroy_sprites(sucked_up,by);
	restoring_contents = FALSE;
	//if (blanked != NULL) {
	//	blanked->remove_all();
	//	delete blanked;
 //  };
	destroy_sprites(blanked,by); // replaced the above on 240103 since increased ref_count before adding it to blanked
	sucked_up = saved_sucked_up;
	blanked = saved_blanked;
};

#if TT_XML

//xml_element *Vacuum::xml_create_element(xml_document *document) {
//   return(create_xml_element(xml_tag(),document));
//};

void Vacuum::xml(xml_element *element, xml_document *document) {
//   xml_set_attribute(element,L"StateCode",(int) state); // moved up to Tool::xml on 180703
   if (sucked_up != NULL) {
      xml_element *sucked_element = create_xml_element(L"Sucked",document);
      sucked_up->add_xml(sucked_element,document,NULL,TRUE); // good to preserve geometry too -- really?? 011102
		xml_append_child(sucked_element,element); // moved here on 190804
   };
   if (blanked != NULL) {
      blanked->add_xml(element,L"Erased",document); // simplified on 051202
   };
   if (tt_dump_reason != DUMPING_TO_CLIPBOARD) { // new on 151102
      xml_set_attribute(element,L"Duration",duration);
      xml_set_attribute(element,L"GoalX",goal_x);
      xml_set_attribute(element,L"GoalY",goal_y);
   };
   if (toolbox_tool(this)) {
//      xml_append_child(create_xml_element(L"FriendOfTooly",document),element);
      xml_set_attribute(element,L"LivesInTooly",1); // new on 171102
   };
	if (sucking_up != NULL) { // new on 130703
		sucking_up->add_xml(element,L"BeingSucked",document);
	};
   Tool::xml(element,document);
};

boolean Vacuum::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
      //case FRIEND_OF_TOOLY_TAG: { // made obsolete on 171102
      //   if (tt_loading_a_city) {
      //      if (tt_toolbox == NULL) {
				  // tt_toolbox = new Toolbox();
			   //}; 
			   //Vacuum *old_vacuum = tt_toolbox_vacuum; 
			   //set_toolbox_vacuum(this);
			   //if (old_vacuum != NULL) {
      //         FavoriteSizeAndLocation f;
      //         old_vacuum->current_favorite(f); // new on 171102 to transfer favorite settings
      //         set_favorite(f);
				  // old_vacuum->destroy();
			   //};
      //   };
      //   return(TRUE);
      //                          };
      case SUCKED_TAG:
			{
				UnwindProtect<boolean> set(tt_loading_vacuum_contents,TRUE); // new on 170105 since should activate is this case
				initialize_sucked_up(xml_load_sprites(node,TRUE)); // NULL elements of the list are fine here - arg new on 240103
			};
         return(TRUE);
      case ERASED_TAG:
			{
				UnwindProtect<boolean> set(tt_loading_vacuum_contents,TRUE); // new on 170105 since should activate is this case
				initialize_blanked(xml_load_sprites(node));
			};
         return(TRUE);
		case BEING_SUCKED_TAG:
			sucking_up = xml_load_sprites(node,TRUE);
			return(TRUE);
      case NO_MORE_TAGS:
         duration = xml_get_attribute_int(node,L"Duration",duration);
         goal_x = xml_get_attribute_int(node,L"GoalX",goal_x);
         goal_y = xml_get_attribute_int(node,L"GoalY",goal_y);
         // and the following
     default:
         return(Tool::handle_xml_tag(tag,node));
   };
};

#endif

boolean Vacuum::dump(output_stream &stream, boolean just_prepare, boolean is_controlee, boolean on_nest) {
   if (!Tool::dump(stream,just_prepare,is_controlee,on_nest)) return(FALSE);
	dump_if_not_null(stream,comment);
   stream.put((unsigned char) state);
   if (sucked_up != NULL) {
      sucked_up->dump(stream,TRUE); // good to preserve geometry too
   };
   stream.put((unsigned char) END_OF_LIST_MARKER);
   if (blanked != NULL) {
      blanked->dump(stream,FALSE); // was TRUE prior to 180100 but no need to save size since should also exist elsewhere
   };
   stream.put((unsigned char) END_OF_LIST_MARKER);
	stream.put(toolbox_tool(this)); // new on 050100
	return(TRUE);
};

Vacuum *load_vacuum(InputStream *stream, int version_number, NaturalLanguage language, long index) {
   Vacuum *vacuum = new Vacuum(0,0);
	associate_shared_item(index,vacuum);
	if (version_number >= first_version_to_save_copied_nests) {
		vacuum->set_comment(load_item(stream,version_number,language));
	};
   vacuum->set_state(stream->get());
   // args added on 180100 to avoid associating robots and boxes - and on 160702 added vacuum
   Sprites *sucked_up = load_items(stream,version_number,language,NULL,FALSE,vacuum); 
//   Sprites *remaining = sucked_up;
//   while (remaining != NULL) {
//      remaining->first()->set_size(100,100);
//      remaining = remaining->rest();
//   };
   vacuum->initialize_sucked_up(sucked_up);
   Sprites *blanked = load_items(stream,version_number,language,NULL,FALSE,NULL,FALSE);
//   remaining = blanked;
//   while (remaining != NULL) {
//      remaining->first()->set_size(100,100);
//      remaining = remaining->rest();
//   };
   vacuum->initialize_blanked(blanked);
	if (version_number >= first_version_to_save_programmer_and_remotes_accurately) { // new on 050100
		boolean friend_of_tooly = stream->get(); 
		if (friend_of_tooly && tt_loading_a_city && !tt_loading_to_ignore) { // second condition new on 181200 to protect tools from bad encodings
			if (tt_toolbox == NULL) {
				tt_toolbox = new Toolbox();
				tt_toolbox->initialize();
			};
			Vacuum *old_vacuum = tt_toolbox_vacuum; // new on 261000
			set_toolbox_vacuum(vacuum);
			if (old_vacuum != NULL) {
				old_vacuum->destroy();
			};
		};
	};
   return(vacuum);
};

boolean describe_vacuum(InputStream *pad_in, output_stream &text_out, int version_number, Article article) {
	if (version_number >= first_version_to_save_copied_nests) {
		describe_comment_encoding(pad_in,text_out,version_number,VACUUM);
	};
   VacuumState state = (VacuumState) pad_in->get(); // describe this later
   // ignore stuff inside for now
   string buffer = new character[10000];
   output_string_stream sink_stream(buffer,10000);
   int counter = 0;
   while (TRUE) {
      unsigned char token = (unsigned char) pad_in->get();
      if (token == END_OF_LIST_MARKER) {
         counter++;
         if (counter == 2) break;
      };
      describe_encoding((SpriteType) token,pad_in,sink_stream,version_number);
   };
   delete [] buffer;
   print_type(VACUUM,text_out,article);
   text_out << space() << S(IDS_WHO_IS_READY_TO,"")
            << space() << S(IDS_VACUUM_STATE0+state)
				<< S(IDS_WHO_IS_READY_TO2,""); // new on 270100
	if (version_number >= first_version_to_save_programmer_and_remotes_accurately) { // new on 050100
		pad_in->get(); // ignore for now whether this is a "friend" of Tooly
	};
   return(TRUE);
};

//VACUUM_SUCK,VACUUM_SPIT,VACUUM_BLANK
//string vacuum_labels = "SUB";

Sprite *Vacuum::respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected,
                                    Sprite *by, boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless) {
   if (!extended) { // condition new on 120604
		int new_state = state;
		boolean record_key = TRUE; // unless proven otherwise
		int index;
		if (key >= '1' && key <= '9') {
			index = key-'1';
		} else {
			index = state_index(key,tt_vacuum_keyboard_accelerators,by);
			if (index == state) { // new on 271102 to deal with keyboard characters when already in that state
				key = tt_run_character; // so pretend it was the space bar
				index = -1; // and pretend never saw the index
			};
		};
		if (only_first_timers != NULL) {
			only_first_timers->remove_all();
			delete only_first_timers;
			only_first_timers = NULL;
		};
		if (index >= 0) {
			new_state = index;
			if (new_state != state) doing(VACUUM_CHANGED_STATE_VIA_KEYBOARD,by);
			if (held()) {
				turn_on(by);
				record_key = (new_state != state);
				if (record_key && by->kind_of() == ROBOT_IN_TRAINING) {
					// keystroke changes state and turns on (uses) tool
					((Robot *) by)->increment_training_counter();
				};
			};
		} else {
			boolean state_cycling = FALSE; // until proven otherwise - new on 151002
			if (key == tt_run_character) { // was ' ' prior to 070502
				if (held()) {
					toggle_on(by);
					record_action = FALSE; 
					// since it'll be used and that'll be recorded -- prior to 271102 was just record_key but why record the space bar? 
				} else {
					new_state++;
					state_cycling = TRUE;
					did_button_cycle(by);
				};
			} else if (key == '+') {
				new_state++;
				state_cycling = TRUE;
				did_button_cycle(by);
			} else if (key == '-') {
				if (state == VACUUM_SUCK) {
					new_state = VACUUM_BLANK;
				} else {
					new_state--;
				};
				state_cycling = TRUE;
				did_button_cycle(by);
			} else if (key == tt_stop_character) { // new on 190799
				turn_off(by);
			} else if (key == toggle_infinite_stack || key == toggle_graspable) {
				if (tt_programmer->kind_of() == PROGRAMMER_AT_FLOOR) {
					// new on 160500
					Sprite *selection = tt_programmer->programmer_at_floor()->pointer_to_current_selection();
					if (selection != NULL) {
						selection->respond_to_keyboard(key,extended,robot_selected,by,FALSE,original_recipient,size_constrained_regardless); 
						// restored and updated above and commented out the following on 080604
						//if (key == toggle_infinite_stack) {
						//	selection->set_infinite_stack(!selection->infinite_stack());
						//} else if (key == toggle_graspable) {
						//	selection->set_graspable(!selection->graspable());
						//};
	//					completed(the_action_status);
	//               by->completed(); // handled above now
						// not bothering to record this since this isn't currently a documented feature
						return(this);
					}; 
				} else {
					if (showing_on_a_screen()) play_sound(PLOP_SOUND); // condition added 170502
					key = 0; // new on 080400 since don't want to use control-i here
					record_key = FALSE;
				};
			} else {
				if (showing_on_a_screen()) play_sound(PLOP_SOUND); // condition added 170502
				key = 0; // new on 080400 since don't want to use control-i here
				record_key = FALSE;
			};
			if (state_cycling && tt_speak_button_name && by != NULL && by->kind_of() != ROBOT) { // programmer or robot in training
				stop_speaking(); // new on 291003 so if still describing the previous state that is aborted
				speak(IDS_VACUUM_STATE0+new_state%VACUUM_LAST_STATE); // new on 240902-- updated to new_state on 291003
				// updated on 260105 to %VACUUM_LAST_STATE since otherwise can speak wrong thing (see issue 624)
//				speak(IDS_VACUUM_STATE0+state); // new on 240902
			};
		};
		if (record_action && record_key) {
			record_equivalent_keyboard_event(state,new_state,tt_vacuum_keyboard_accelerators,by,floor,this);
			record_action = FALSE; // done now
	//    record_event(KEYBOARD_CHARACTER,by,floor,this,TRUE,key);
		};
		set_state(new_state,by);
	//   action_status = NULL; // why?? -- commented out on 181102
		if (is_on() && !tt_vacuum_used_once_per_click) { // updated 260902
			repeating_sound();
		};
	};
//	completed(the_action_status);
// commented out the above for the following on 080400
   return(Sprite::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,size_constrained_regardless));
};

void Vacuum::set_state(int new_state, Sprite *by) {
   if (new_state != state) {
      state = (char) (new_state%VACUUM_LAST_STATE);
      update_button();
      // moved on 151002 so only when cycling through states
		//if (tt_speak_button_name && by != NULL && by->kind_of() != ROBOT) { // programmer or robot in training 
		//	speak(IDS_VACUUM_STATE0+state); // new on 240902
		//};
   };
};

boolean Vacuum::used_once_per_click() { // new on 310702
   // enabled and right button isn't being held down
   // used to include tt_current_mouse_button&BUTTON_STILL_DOWN) but it didn't seem reliable enough
   // restored on 260902 and used together with virtual_right_button
   return(tt_vacuum_used_once_per_click && !(tt_current_mouse_button&BUTTON_STILL_DOWN && virtual_right_button(tt_current_mouse_button)));
};

boolean Vacuum::is_still_on() { // new on 270902 -- pays attention to whether mouse is still down when tt_vacuum_used_once_per_click
   if (!on_flag) return(FALSE);
   if (tt_vacuum_used_once_per_click && !virtual_right_button(tt_current_mouse_button)) { 
      // only if mouse button is (still) down
      turn_off(NULL);
   };
   return(on_flag);
};

char Vacuum::button_label() {
   const_string labels = S(IDS_VACUUM_KEY_COMMANDS);
   return(labels[state]);
};

void Vacuum::repeating_sound() {
   if (!showing_on_a_screen()) return; // moved here to be more general on 260902
   short int new_sound_id;
   if (state == VACUUM_SPIT) {
      new_sound_id = SPIT_SOUND;
   } else {
      new_sound_id = DUSTBUSTER_SOUND;
   };
   if (new_sound_id != sound_id) {
      stop_sound_id(sound_id);
      sound_id = new_sound_id;
      play_sound(sound_id,TRUE); // condition added 170502 - !used_once_per_click() replaced TRUE on 310702 (restored on 260902)
   };
};

void Vacuum::turn_on(Sprite *by) {
   if (is_on()) return;
   Tool::turn_on(by);
#if TT_DIRECT_INPUT
   if (state != VACUUM_SPIT && by != NULL && !by->still_running()) {
      play_force_feedback_effect("Vacuum");
   };
#endif
};

void Vacuum::turn_off(Sprite *by) {
   if (!is_on()) return;
   Tool::turn_off(by);
#if TT_DIRECT_INPUT
   stop_force_feedback_effect();
#endif
};

Sprite *Vacuum::used(Sprite *subject, Sprite *by,
							boolean button_unchanged, boolean record_action,
							millisecond ,
							boolean modified, Sprite *original_subject) {
   finish_instantly(); // new on 310702 since can be still animating a suck up and user typed R for example
   // doesn't caller check this now??
 	if (being_used() || (subject != NULL && (subject->prepared_for_deletion() || subject->held()))) { 
		// disjunct added on 201199 since happens somehow
		// could signal a problem if robot is doing this...
		// added condition on 111104 for the subject being held since it may have been picked up 
		// in the second or so between when a robot plans to vacuum
		// and it does
		by->completed();
//		action_status = NULL;
		return(this);
	};
   if (by == NULL || !by->still_running()) { // !is_kind_of(by,ROBOT)) { // tt_new_input_scheme && 
      modified = FALSE; // 3rd button used differently now
      if (!button_unchanged) {
         respond_to_keyboard(tt_run_character,FALSE,FALSE,by,record_action,original_subject); 
			// ' ' was tt_run_character prior to 070502
         // is recorded as used even if via the keyboard
//         record_action = FALSE; // recorded as a ' '
         if (!is_on()) {
            return(this); // just turned it off
         };
      };
   };
   millisecond duration = by->default_duration();
   boolean reverse = FALSE; // unless proven otherwise
//   if (is_on()) {
      switch (state) {
         case VACUUM_SUCK:
            reverse = FALSE;
//				tt_just_vacuumed = subject; 
				// new on  160205 so recorded properly -- e.g. vacuuming a box from a robot's thought bubble
            break;
         case VACUUM_SPIT:
            reverse = TRUE;
            break;
         case VACUUM_BLANK:
            modified = TRUE;
            break;
      };
//   };
	// doesn't matter how long the button is down
//	if (by->kind_of() != ROBOT) simulate_mouse_up();
	// a vacuum sucks up the subject or
	// if there is none spits out the last thing sucked up
	if (button_unchanged && !by->still_running() &&
		 subject != NULL && 
//		 (subject->pointer_to_followers() != NULL ||
		  (only_first_timers != NULL &&
		   only_first_timers->member_cons(subject) != NULL)) {
		// too easy to vacuum up cubby or notebook
		// after sucking up something inside
		// so must be first thing or empty
		by->abort_action_if_robot(-1);
		return(this);
	};
	if (!button_unchanged &&
       // || (!reverse && subject == NULL && !replaying() && tt_log_out_file_name != NULL)) &&  
		 // revisit this -- worried about old demos
       only_first_timers != NULL) {
      // button change or deselection so can now vacuum up container... 
		only_first_timers->remove_all();
		delete only_first_timers;
		only_first_timers = NULL;
//      destroy_sprites(only_first_timers);
	};
//	action_status = the_action_status;
   set_saved_by(by); // replaces the above on 171102
   if (reverse) {
      if (restoring == NULL) {
         restore_last_item(duration,by,record_action);
      } else if (sucked_up == NULL) { // nothing left to spit out
         turn_off(by);
      };
      return(this);
   };
   if (subject == NULL) {
//      if (!tt_new_input_scheme) {
//         if (!button_unchanged) {// only if let up on button
//            if (!by->still_running()) simulate_mouse_up();
//            restore_last_item(duration,by,record_action);
//         };
//      };
      if (used_once_per_click() && !by->still_running()) {
         turn_off(by); // new on 310702
      };
		return(this);
	};
//#if TT_DEBUG_ON
//	if (subject->pointer_to_background() == NULL) {
//		log("debug this");
//	};
//#endif
	if (by->kind_of() == PROGRAMMER) { // new on 200902 so that indication of match failure goes away if vacuumed
		subject->set_cause_of_match_failure(FALSE,TRUE);
	};
   Sprite *subjects_leader = subject->pointer_to_leader();
   if (subjects_leader != NULL && (!is_on() || state == VACUUM_BLANK)) {
      // allow the sucking up of leaders in same "session" if turned on
	   vacuum_only_if_first(subjects_leader); // protect leader until new click
	};
	if (modified) {
      if (duration > 0 && (!is_on() || tt_vacuum_used_once_per_click)) { 
			// prior to 260902 tt_vacuum_used_once_per_click was used_once_per_click()
         make_sucking_sound(); // condition fixed on 260902 to work better with use once per click
      };
		if (suck_to_make_blank(subject,by)) {
    		if (record_action) record_event(VACUUM_APPLY,by,floor,subject,FALSE);
//      } else {
//         if (by != NULL && by->kind_of() == ROBOT) turn_off(by);
      };
      //if (used_once_per_click() && !by->still_running()) { // commented out on 260902
      //    turn_off(by); // new on 310702
      //};
	} else {
		if (subject->inside_vacuum()) {
			// trying to suck up an item that
			// hasn't finished leaving the vacuum
			by->completed();
         set_saved_by(NULL);
//			action_status = NULL;
			return(this);
		};
      if (subjects_leader != NULL && subjects_leader->kind_of() == PROGRAM_PAD &&
			 ((Notebook *) subjects_leader)->pointer_to_picture() != NULL) { // && tt_debug_mode != 8642) { 
			// this is in a notebook of remote controls
			subject->set_infinite_stack(TRUE); // new on 080301 to restore stack since sprite::selected turned it off
         if (!told_user(TOLD_REMOTE_NOTEBOOK_READONLY)) {
            say(IDS_CANT_ADD_TO_REMOTE_NOTEBOOK,5);
            user_told(TOLD_REMOTE_NOTEBOOK_READONLY);
         };
         if (duration > 0) play_sound(PLOP_SOUND);
		   by->abort_action_if_robot(-1);
		   return(this);
      };
		if (subjects_leader != NULL && tt_toolbox_notebook == subjects_leader && by->kind_of() != PROGRAMMER) { // new on 010301
			// programmer check added on 080301
			subject->set_infinite_stack(TRUE); // new on 080301 to restore stack since sprite::selected turned it off
			say(IDS_CANT_ADD_TO_NOTEBOOK_IN_BUBBLE);
			if (duration > 0) play_sound(PLOP_SOUND); // condition added 170502 since otherwise can hear when on back of a picture
		   by->abort_action_if_robot(-1);
		   return(this);
      };
      if (duration > 0 && (tt_vacuum_used_once_per_click || !is_on())) {
         make_sucking_sound(); // added used_once_per_click() on 310702 -- revised on 260902
      };
		if (record_action) {
//			if (subject != original_subject && original_subject != NULL &&
//				 original_subject->kind_of() == REMOTE_PICTURE) {
//				record_event(VACUUM_APPLY,by,floor,original_subject);
//			} else {
				record_event(VACUUM_APPLY,by,floor,subject,FALSE,0,ADD_TO_WORKING_SET_IF_NEW,
								 (original_subject != NULL && original_subject->kind_of() == PROGRAM_PAD)
#if TT_POST3187
								 ,-1,original_subject // new on 280507
#endif
								 );
				// arg new on 110205 so can train a robot to vacuum off a page from a notebook
//			};
		};
		if (subjects_leader != NULL) {
			// this is too late since will be sucking up wrong copy...
//			if (subjects_leader->infinite_stack()) { // new on 150301
//				subjects_leader = subjects_leader->selected(VACUUM_USED,by); 
//			};
			subjects_leader->remove_selected_element(subject,VACUUM_USED,by,(duration > 0)); 
			// duration > 0 was TRUE prior to 300301 but if robot is doing this on back of picture no need to add for animation
		};
		subject->set_selectable(FALSE);
      subject->stop_all(); // new on 160702
		// commented out the following since picture::set_inside_vacuum does this fine
//		subject->stop_all(FALSE); // new on 070400 since things shouldn't be running inside of Dusty - 
		// must type space to start them up again if spit out
//		subject->set_active(FALSE); // also new on 070400
		if (duration > 0) {
			set_being_used(TRUE);
			if (subject->kind_of()!= ROBOT && subject->contained_in(ROBOT)) {
				robot_abstracted();
			};
			floor->give_item_priority(subject);
			set_parameter(VACUUM_SUCKING_UP);
         insert_at_end(subject,sucking_up);
//			if (sucking_up == NULL) {
//				sucking_up = new Sprites(subject);
//			} else {
//				sucking_up->insert_at_end_if_not_member(subject);
//			};
//			if (subject->pointer_to_leader() == NULL) {
				doing(VACUUM_USED_TO_SUCK,by);
//			} else {
//				user_did(VACUUM,VACUUM_USED_TO_SUCK_COMPONENT);
//			};
//			saved_screen = screen;
//			subject->save_size();
			// to support feature that if vacuum doesn't move sucking then
			// spitting out is idempotent
			subject->lower_left_corner(last_sucked_llx,last_sucked_lly);
			// sucking part is 1/6 height and is from 1/3 to 1/2 from bottom
			subject->animate_size(100,height/6,duration); // become thin
			subject->animate_location_to(this, //&llx,&lly,
                                      duration,
//											     this,sucking_up_finished_callback,
											     NULL,CALLBACK_NOTHING,
											     0,height/3);
			// only if vacuum is still around -- e.g. might be held by robot
			if (tt_log_version_number >= 22) { // new on 070400
				duration += 100; // after animation has finished and signaled so can set it to good size for remote looks
			};
			do_after(duration,this,SUCKING_UP_FINISHED_CALLBACK);
		} else { // do it right away
//			subject->set_inside_vacuum(TRUE);
			item_sucked_up(subject);
		};
		tt_just_vacuumed = NULL; // no longer needed (moved here on 091000)
		tt_just_vacuumed_remote = NULL;
	};
	return(this);
};

SelectionReason Vacuum::selection_reason() {
	switch (state) {
      case VACUUM_SPIT:
         return(VACUUM_USED_TO_SPIT);
      case VACUUM_BLANK:
         return(VACUUM_USED_TO_BLANK);
		default:
//			      case VACUUM_SUCK:
         return(VACUUM_USED);
	};
};

void Vacuum::vacuum_only_if_first(Sprite *to_protect) {
	while (to_protect != NULL) {
//		only_first_timers = new Sprites(to_protect,only_first_timers);
		insert_at_end_if_not_member(to_protect,only_first_timers);
		to_protect = to_protect->pointer_to_leader();
	};
};

boolean Vacuum::suck_to_make_blank(Sprite *item, Sprite *by) {
	// used when vacuuming numbers, pictures or cubbies in thought bubbles
//	simulate_mouse_up();  // so I don't suck up the result right away
   if (item->is_blank()) {
      by->completed(); // not using action_status
      return(FALSE); // didn't do anything worth recording...
   };
	if (!item->blankable()) {
		by->abort_action_if_robot(IDS_CANT_BE_MADE_BLANK);
		return(FALSE);
	};
	item->become_blank(!by->still_running());
	set_parameter(VACUUM_SUCKING_UP);
	// put copy here so it can be made non_blank again
//	sucked_up = new Sprites(item->copy(NULL),sucked_up);
// following fixes problem that vacuum needs to delete items
// but can be corrupted if blanked item is deleted first
	// but it is ref counted below so should be OK
	sucked_up = new Sprites(NULL,sucked_up); // a marker
	blanked = new Sprites(item,blanked);
	item->increment_ref_count(this); // new on 180100
	doing(VACUUM_USED_TO_MAKE_BLANK,by);
	if (item->kind_of() != ROBOT && item->contained_in(ROBOT)) {
		robot_abstracted();
	};
	by->completed();
   set_saved_by(NULL);
//	action_status = NULL;
//   do_after(1,this,turn_off_unless_mouse_still_down_callback); 
	// new on 270902 -- can't see if mouse is still down if it just happened on this frame   
   return(TRUE);
};

//void Vacuum::turn_off_unless_mouse_still_down() {
//   if (used_once_per_click()) { 
//      // if right button is held down ignore once_per_click stuff
//      turn_off(NULL); 
//      set_being_used(FALSE); // should this be done by turn_off ??
//   };
//};

void Vacuum::remove_blanked_item(Sprite *item) {
	// vacuum is ready to restore it but item is deleted or something
	if (blanked == NULL) return; // why was it called then?
	// following added on 121099 so doesn't crash if you a train a robot to erase something and then vacuum it up
	if (restoring_contents) return; // since this list is about to be updated anyway
	Sprites *previous_sucked_up;
	Sprites *previous_blanked;
	Sprites *remaining_sucked_up = sucked_up;
	Sprites *remaining_blanked = blanked;
	while (remaining_blanked != NULL) {
		if (remaining_blanked->first() == item) break;
		previous_blanked = remaining_blanked;
		remaining_blanked = remaining_blanked->rest();
		previous_sucked_up = remaining_sucked_up;
		if (remaining_sucked_up != NULL) { // new on 180100 -- for example, quitting after just erasing something can cause trouble
			remaining_sucked_up = remaining_sucked_up->rest();
		};
	};
	if (remaining_blanked == NULL) {
	   // could have been blanked by other means not worth warning about
//		tt_error_file() << "Warning blanked item not in vacuum" << endl;
		return;
	};
	item->destroy(this); // typically just decrements ref count
	Sprites *new_blanked = remaining_blanked->rest();
	Sprites *new_sucked_up = remaining_sucked_up->rest();
	remaining_blanked->set_first(NULL);
	remaining_blanked->set_rest(NULL);
	// first should already be NULL
	remaining_sucked_up->set_rest(NULL);
	if (remaining_blanked == blanked) { // first guy
		delete remaining_sucked_up;
		blanked = new_blanked;
		sucked_up = new_sucked_up;
	} else {
		delete remaining_sucked_up;
		previous_blanked->set_rest(new_blanked);
		previous_sucked_up->set_rest(new_sucked_up);
	};
	delete remaining_blanked;
};

const millisecond delay_before_ready_to_suck_again = 1000;
const millisecond delay_before_ready_to_restore_again = 1000;

void Vacuum::restore_last_item(millisecond duration, Sprite *by, boolean record_action) {
	if (restoring == NULL) {
//		if (modified) {
//			if (deleted != NULL) {
//				deleted = deleted->pop(restoring);
//			} else {
//				play_sound(PLOP_SOUND);
//				return;
//			};
//		} else {
		while (TRUE) { // new on 150103 since may have been item may have been destroyed after being sucked up
			if (sucked_up == NULL) {
				if (is_on()) { // nothing in it
               if (duration > 0) play_sound(SHUTDOWN_SOUND); // condition added 170502
               turn_off(by);
            } else {
               if (duration > 0) play_sound(PLOP_SOUND); // condition added 170502
            };
				return;
			};
			sucked_up = sucked_up->pop(restoring);
			if (restoring != NULL) { // new on 280300
				if (!restoring->destroy(this)) { // typically decrementing ref count
					// not really destroyed
					break; // new on 150103
				};
			} else {
				break; // blanked - new on 150103
			};
		};
		doing(VACUUM_USED_TO_RESTORE,by);   
      if (!by->still_running()) {
         //if (used_once_per_click()) { // commented out on 270902
         //   turn_off(by); // new on 310702
         //} else {
            millisecond wait;
            if (restoring != NULL) {  // tt_log_version_number < 9 ||
               wait = duration+delay_before_ready_to_restore_again;
            } else { // undoing an erase is faster
               wait = duration;
            };
//         if (tt_log_version_number > 8)
            set_being_used(TRUE);
            do_after(wait,this,SET_BEING_USED_FALSE_CALLBACK);
 //        };
      } else {
         set_being_used(FALSE);
      };
		if (restoring == NULL) { // was made blank so
		   if (blanked == NULL) { // something wrong...
			   return;
			};
			blanked = blanked->pop(restoring);
			// commented out the following on 120600 since when done by a robot on the back of a picture it makes no sense
			// and I don't see why it was a good idea even if done on the floor by the programmer
//         if (!restoring->showing_on_a_screen()) {
            // skip restoring blanks for things not visible
//            restoring = NULL;
//            restore_last_item(duration,by,record_action);
//            return;
//         };
			restoring->become_non_blank();
			restoring = NULL;
			if (duration > 0) {
            if (is_on()) stop_repeating_sound();
				make_spitting_sound_callback(this);
			};
			by->completed();
//			action_status = NULL;
         set_saved_by(NULL);
     		if (record_action) record_event(VACUUM_APPLY_RESTORE,by,floor,restoring);
			return;
		};
//		if (by->kind_of() != ROBOT) {
//			record_event(VACUUM_APPLY_RESTORE,by,floor,restoring);
//       floor->record_event(NEW_ITEM,restoring);
//		} else {
//			 floor->add_to_some_working_set(restoring,by);
//		};
//		saved_screen = screen;
//		if (restoring->pointer_to_followers() != NULL) {
//			restoring->recursively_restore_leaders(floor);
//			restoring->set_changes_propagated(FALSE);
//		};
//		restoring->recursively_set_background(floor);
		if (restoring->kind_of() != PICTURE) {
			restoring->set_is_picture(FALSE); 
			// new on 060400 since may have been sucked off a parts or container sensor for example but then should be an ordinary box
		};
#if TT_DEBUG_ON
		if (restoring->debug_counter == tt_debug_target) {
			log("About to restore debug target from Dusty.");
		};
#endif
		floor->add_item(restoring);
//		Picture *picture = floor->pointer_to_picture(); // new on 300301
//		if (picture != NULL) {
//			picture->place_new_item(restoring);
//		};
		// moved here on 7/10/98 since was after call to item_restored which reset restoring to NULL
		if (record_action) {
			record_event(VACUUM_APPLY_RESTORE,by,floor,restoring);
		} else if (by != NULL && by->still_running()) {
			Robot *robot = (Robot *) by;
			if (restoring != robot->pointer_to_working_cubby()) {
				// new on 110699 so that you can suck up and spit out the main notebook
				robot->add_to_working_set(restoring,TRUE);
			};
		};
		if (duration > 0) {
			set_parameter(VACUUM_SPITTING_OUT);
         if (is_on()) stop_repeating_sound();
			do_after(200,this,MAKE_SPITTING_SOUND_CALLBACK);
#if TT_DIRECT_INPUT
         if (by != NULL && !by->still_running()) play_force_feedback_effect("Spit",FALSE);
#endif
//			restoring->update_display(); // why??
//          city_coordinate vacuum_llx, vacuum_lly;
//          appearance->lower_left_corner(vacuum_llx, vacuum_lly);
			restoring->move_to(llx,lly);
			city_coordinate good_height;
			if (tt_log_version_number >= 22) {
				restoring->set_size(100,height/6,VACUUM_RESTORING); // new on 070400 since size is set after sucking up
			};
			restoring->good_size(item_width,good_height,tt_system_mode==PUZZLE?NULL:tt_toolbox);
			restoring->animate_size(item_width,good_height,duration);
			city_coordinate item_llx = llx-item_width,
								 item_lly = lly-(height/3-good_height/2);
			city_coordinate distance_squared = 0;
			if (by != NULL && !by->still_running() &&
				 !(last_sucked_llx == 0 && last_sucked_lly == 0)) {
				city_coordinate delta_x = last_sucked_llx-item_llx;
				city_coordinate delta_y = last_sucked_lly-item_lly;
				distance_squared = delta_x*delta_x+delta_y*delta_y;
			};
			if (distance_squared != 0 &&
				 distance_squared < item_width*item_width &&
				 tt_screen->first_to_contain(last_sucked_llx,last_sucked_lly) == NULL) {
				// within the width of the item and 
				// no one at lower left corner
				item_llx = last_sucked_llx;
				item_lly = last_sucked_lly;
			};
			last_sucked_llx = 0;
			last_sucked_lly = 0;
			// make sure it doesn't end up off screen
			if (item_llx < 0) {
				item_llx = 0;
			} else if (item_llx+item_width > ideal_screen_width) {
				item_llx = ideal_screen_width-item_width;
			};
			if (item_lly < 0) {
				item_lly = 0;
			} else if (item_lly+good_height > ideal_screen_height) {
				item_lly = ideal_screen_height-good_height;
			};
//			if (tt_log_version_number < 13) {
//				restoring->animate_location(item_llx,item_lly,duration,this,item_restored_callback);
//			} else {
				restoring->animate_location(item_llx,item_lly,duration);
				// split in 2 in case vacuum is gone by the time restoring is finished animating...
				do_after(duration,this,ITEM_RESTORED_CALLBACK);
//			};
		} else {
			item_restored();
		};
	};
};

void make_sucking_sound() {
	play_sound(DUSTBUSTER_SOUND); //,FALSE,FALSE);
};

void Vacuum::sucking_up_finished() {
	if (sucking_up == NULL) { // new on 140603 for robustness
		return;
	};
	Sprite *item;
	sucking_up = sucking_up->pop(item);
	item_sucked_up(item);
//	item->increment_ref_count(); // protect it
	if (held_status != NOT_HELD) { // might have been dropped quickly
		set_parameter(VACUUM_LAYING);  
	};
   if (used_once_per_click()) { 
      // if right button is held down ignore once_per_click stuff
      turn_off(NULL); // new on 310702
      set_being_used(FALSE); // added on 300802 (should this be done by turn_off ??)
   } else if (leader == NULL || leader->kind_of() != ROBOT) {
      millisecond delay = delay_before_ready_to_suck_again;
      if (tt_vacuum_used_once_per_click) {
         delay *= 2; // double it since less expected
      };
      do_after(delay,this,SET_BEING_USED_FALSE_CALLBACK);
   } else {
      set_being_used(FALSE);
   };
};

void Vacuum::describe(output_stream &message, Article ) {
	describe_comment(message);
	if (tt_histories[VACUUM] == 0) {
		message << S(IDS_VACUUM_DESCRIPTION);
	} else {
		message << S(IDS_VACUUM_DESCRIPTION1);
	};
	message << space() << S(IDS_WHO_IS_READY_TO,"")
           << space() << S(IDS_VACUUM_STATE0+state)
			  << S(IDS_WHO_IS_READY_TO2,""); // new on 270100
};

void Vacuum::generate_help(output_stream &message, Sprite *tool_in_hand,
                           Sprite *selection) {
//   if (!did(VACUUM_USED_TO_SUCK) &&
//		 !did(VACUUM_USED_TO_MAKE_BLANK) &&
//		 tt_help_counter == 0) {
//		Sprite::generate_help(message,tool_in_hand,selection);
//		message << S(IDS_VACUUM_SUCK_GENERIC);
//	} else 
   if ((//selection != NULL && // what is all this??
					//selection->pointer_to_leader() == NULL &&
					!did(VACUUM_USED_TO_SUCK) && tt_help_counter == 0)
				  || tt_help_counter == 1) {
      describe_pointing(message,tool_in_hand,selection);
		tt_help_history_action = VACUUM_USED_TO_SUCK;
//	   message << S(IDS_VACUUM_SUCK_HELP);
		string default_help = S(IDS_VACUUM_SUCK_HELP); // re-written on 110700 so Brazilian version can use IDS_VACUUM_SUCK_PRESS_HELP
      if (state == VACUUM_SUCK) {
			message << default_help;
         describe_right_button(message);
      } else {
         const_string commands = S(IDS_VACUUM_KEY_COMMANDS);
			string better_help = NULL;
			if (tt_language == BRAZILIAN_PORTUGUESE) { // since click and press shouldn't be mixed up...
				better_help = S(IDS_VACUUM_SUCK_PRESS_HELP,NULL);
			};
			if (better_help == NULL) {
				message << default_help;
			} else {
				message << better_help;
			};
         message << "'" << commands[VACUUM_SUCK] << "'";
      };
      message << S(IDS_VACUUM_SUCK_HELP2,"") << S(IDS_VACUUM_SUCK_HELP4) << S(IDS_VACUUM_SUCK_HELP2_2,"");
//      if (!tt_new_input_scheme) message << S(IDS_TURN_VACUUM_ON_HELP);
//      switch (tt_language) {
//         case AMERICAN:
//         case BRITISH:
//		      if (tt_help_counter == 0 && selection != this && selection != NULL) {
//			      selection->describe_type(message,DEFINITE_ARTICLE);
//			      message << end_sentence() << S(IDS_VACUUM_SUCK_HELP3);
//		      } else {
//			      message << S(IDS_VACUUM_SUCK_HELP4);
//		      };
//            break;
//         default:
//            message << S(IDS_VACUUM_SUCK_HELP4); // den/det problem avoided
//            break;
//      };  

//      if (tt_new_input_scheme) describe_space_bar_alternatives(message);
   } else if ((!did(VACUUM_CHANGED_STATE_VIA_BUTTON) && tt_help_counter == 0)
		        || tt_help_counter == 2) {
		tt_help_history_action = VACUUM_CHANGED_STATE_VIA_BUTTON;              
      message << S(IDS_VACUUM_CHANGED_STATE_VIA_BUTTON_HELP);
   } else if ((!did(VACUUM_CHANGED_STATE_VIA_KEYBOARD) && tt_help_counter == 0)
		        || tt_help_counter == 3) {
		tt_help_history_action = VACUUM_CHANGED_STATE_VIA_KEYBOARD;
      message << S(IDS_VACUUM_CHANGED_STATE_VIA_KEYBOARD_HELP);
	} else if ((//(selection == NULL || selection == this) && 
		        //sucked_up != NULL && 
		        !did(VACUUM_USED_TO_RESTORE) && tt_help_counter == 0)
		        || tt_help_counter == 4) {
		tt_help_history_action = VACUUM_USED_TO_RESTORE;
		Sprite *item;
  		message << S(IDS_VACUUM_RESTORE_HELP);
		if (sucked_up != NULL && tt_help_counter == 0) {
			item = sucked_up->first();
			if (item == NULL) item = blanked->first();
   		if (item == NULL) {
   			message << S(IDS_VACUUM_RESTORE_HELP2);
   		} else {
   			item->describe_type(message,DEFINITE_ARTICLE);
   		};
		} else {
			message << S(IDS_VACUUM_RESTORE_HELP2);
		};
      message << space() << S(IDS_VACUUM_RESTORE_HELP3) << space();
      if (state == VACUUM_SPIT) {
    		describe_right_button(message);
  		} else {
         const_string commands = S(IDS_VACUUM_KEY_COMMANDS);
         message << "'" << commands[VACUUM_SPIT] << "'";
      };
		message << S(IDS_VACUUM_RESTORE_HELP4,"") << end_sentence(); // S(IDS_VACUUM_RESTORE_HELP4,"") new on 080200
	} else if ((//selection != NULL && selection->blankable() &&
					//(selection->pointer_to_leader() == NULL 
					// || selection->pointer_to_leader()->kind_of() != PICTURE) &&   
					!did(VACUUM_USED_TO_MAKE_BLANK) && tt_help_counter == 0)
				  || tt_help_counter == 5) {
		tt_help_history_action = VACUUM_USED_TO_MAKE_BLANK;
		message << S(IDS_VACUUM_BLANK_HELP);
		if (tt_help_counter == 0 && selection != this && selection != NULL &&
          selection->blankable() &&
			 (selection->pointer_to_leader() == NULL 
			  || selection->pointer_to_leader()->kind_of() != PICTURE)) {
			selection->describe_type(message,DEFINITE_ARTICLE);
		} else {
			message << S(IDS_VACUUM_BLANK_HELP2);
		};
		message << space() << S(IDS_VACUUM_BLANK_HELP3) << space();
      if (state == VACUUM_BLANK) {
     		describe_right_button(message);
  		} else {
         const_string commands = S(IDS_VACUUM_KEY_COMMANDS);
         message << "'" << commands[VACUUM_BLANK] << "'";
      };
		message << S(IDS_VACUUM_BLANK_HELP4,""); // new on 140200
		message << end_sentence();
	} else if ((!did(VACUUM_CALLED) && tt_help_counter == 0)
		        || tt_help_counter == 6) {
		tt_help_history_action = VACUUM_CALLED;
		message << S(IDS_VACUUM_CALLED_HELP);
		describe_hand(message);
		message << S(IDS_VACUUM_CALLED_HELP2);
		message << S(IDS_VACUUM_CALLED_HELP3);
	} else if ((!did(TYPE_HELP_GIVEN) && tt_help_counter == 0)
				  || tt_help_counter == 7) {
		message << S(IDS_VACUUM_HELP_GIVEN_HELP2); // order switched on 12/20
		message << S(IDS_VACUUM_HELP_GIVEN_HELP);
		message << S(IDS_VACUUM_HELP_GIVEN_HELP3);
		no_help_interruptions();
		doing(TYPE_HELP_GIVEN);
	};
};

/*
	} else if (!did(VACUUM_USED_TO_SUCK_COMPONENT)) {
			tt_help_history_action = VACUUM_USED_TO_SUCK_COMPONENT;
		message << S(IDS_VACUUM_SUCK_ITEM_HELP);
		describe_right_button(message);
		message << S(IDS_VACUUM_SUCK_ITEM_HELP2);
		describe_type(current_selection,message,DEFINITE_ARTICLE);
		message << S(IDS_OUT_OF);
		describe_type(current_selection->pointer_to_leader(),message,DEFINITE_ARTICLE);
		message << end_sentence() << S(IDS_VACUUM_SUCK_ITEM_HELP3);
		return;
*/

/*
void Vacuum::update_display() {
//	if (sucking_up != NULL) {
//		if (sucking_up->first()->inside_vacuum()) {
//			Sprite *item;
//			sucking_up = sucking_up->pop(item);
//			item_sucked_up(item);
//			set_parameter(VACUUM_LAYING);
//      } else {
//       sucking_up->set_clean_status(DIRTY);
//		};
//	};
	if (restoring != NULL) {
		if (restoring_done) {
			restoring_done = FALSE;
			item_restored();
			set_parameter(VACUUM_LAYING);
		} else {
//			llx_minus_item_width = *pointer_to_llx-item_width;
			restoring->set_clean_status(DIRTY);
		};
	};
	Tool::update_display();
};
*/

void Vacuum::item_sucked_up(Sprite *newly_sucked_up) {
#if TT_DEBUG_ON
	if (newly_sucked_up->debug_counter == tt_debug_target) {
		tt_error_file() << "Vacuuming up the debug target." << endl;
	};
#endif
//	completed(saved_by);
//	saved_by = NULL;
   action_completed();
	newly_sucked_up->set_inside_vacuum(TRUE);
	sucked_up = new Sprites(newly_sucked_up,sucked_up); 
	// moved here on 191202 from after the following since didn't work when tt_vacuum_maximum_item_count == 1
	if (tt_vacuum_maximum_item_count > 0 && leader == NULL) { // new on 190401 - doesn't apply to robots (who are leaders)
		Sprites *excess = sucked_up->truncate_to_length(tt_vacuum_maximum_item_count); 
		// prior to 191202 was tt_vacuum_maximum_item_count-1
		if (excess != NULL) {
			int erasings = excess->count_occurrences(NULL);
			excess->decrement_ref_count_of_each(); 
			// new on 230103 since is double protected from deletion - not clear why I do this? 
			// incremented to protect it and started with ref count of 1
			destroy_sprites(excess);
			if (erasings > 0) {
				excess = blanked->truncate_to_length(erasings);
				destroy_sprites(excess);
			};
		};
	};
	newly_sucked_up->increment_ref_count(this); // new on 280300
//       saved_screen->remove_from_background(sucking_up);
//	floor->remove_item(newly_sucked_up,FALSE);
	// replace above with following on 170299 since on back of picture floor might be NULL
	newly_sucked_up->remove_from_floor(FALSE);
	newly_sucked_up->set_background(NULL,TRUE); 
	// added 010200 to be sure no followers still refer to old floor (which might go away) anyway
	// commented out the following on 160103 since why change it size after sucking it up?
	//if (tt_log_version_number >= 22) { // added on 070400 in case a remote looks for this is still hanging around
	//	newly_sucked_up->set_to_good_size(NULL); 
	//};
//	if (newly_sucked_up->pointer_to_followers() != NULL) {
		// following 2 lines probably not needed anymore
//		newly_sucked_up->propagate_changes();
//		newly_sucked_up->clear_changes();
//		newly_sucked_up->recursively_remove_leaders(floor);
//		newly_sucked_up->recursively_set_background(NULL);
//	};
	// recording event taken care of by cubby or number
};

void Vacuum::add_deletee(Sprite *item) {
	// keep items as a kind of undo facility
//	deleted = new Sprites(item,deleted);
//	item->save_size();
	if (item->return_ref_count() == 0 || !ok_to_rescue_from_exploding_house()) { 
		// new on 080403 - can happen with a local remotes notebook on the floor being blown up
		return;
	};
	sucked_up = new Sprites(item,sucked_up);
	item->increment_ref_count(this); // new on 280300
	item->set_inside_vacuum(TRUE);
	item->set_slated_for_deletion(FALSE);
//	floor->remove_item(item,FALSE,TRUE);
	item->remove_from_floor(FALSE,TRUE);
//	item->recursively_set_background(NULL);
	make_sucking_sound();
	set_parameter(VACUUM_SUCKING_UP);
	do_after(total_cycle_duration(),this,VACUUM_MORPH_TO_INERT_CALLBACK);
};

void Vacuum::item_restored() { 
	if (restoring == NULL) {
		return; // new for robustness on 140603
	};
	restoring->finish_instantly(); // new on 190700 since can be nearly finished animating
	restoring->released(this);
	restoring->set_inside_vacuum(FALSE);
	restoring->set_graspable(TRUE);
   restoring->set_selectable(TRUE);
	restoring->now_on_floor(floor,this);
	restoring = NULL;
	action_completed();
	if (held_status != NOT_HELD) { // might have been dropped quickly
		set_parameter(VACUUM_LAYING);  
	};
};

Sprite *Vacuum::selected(SelectionReason reason, Sprite *by) {
   Sprite *result;
//   if (tt_log_version_number > 8)
   result = Sprite::selected(reason,by);  // do this first since it may run animations etc
	if (reason == GRASPED) {
//	     && (tt_log_version_number > 12 || parameter == VACUUM_INERT || parameter == VACUUM_MORPHING_TO_INERT)) {
		if (current_parameter() == VACUUM_INERT || current_parameter() == VACUUM_MORPHING_TO_INERT || tt_log_version_number < 28) { // conditional new on 071201 since if already in good mode it looks bad and can mess up stable selection region's position
			set_parameter(VACUUM_MORPHING_TO_LIFE);
		} else {
			set_parameter(VACUUM_LAYING); // new on 071201
		};
	};
//   if (tt_log_version_number <= 8) result = Sprite::selected(reason,by); // old order
	return(result);
};

Sprite *Vacuum::released(Sprite *by, boolean top_level, SpecialDrop special_drop) {
	boolean was_called_since_last_release = called_since_last_release;
	Sprite *result = Tool::released(by,top_level,special_drop); // moved here on 260201 so that if dropped while on is turned off first so it animates better
   if (was_called_since_last_release) {
      walk_to_favorite_location();
   };
   millisecond size_change_duration = 2000;
   if (!by->still_running()) {
      animate_to_favorite_size(size_change_duration);
   };
	if (!was_called_since_last_release && parameter != VACUUM_INERT) {
		if (showing_on_a_screen()) { // condition new on 140104 since wasteful otherwise
			set_parameter(VACUUM_MORPHING_TO_INERT);
			if (by->kind_of() == PROGRAMMER) {
				do_after(max(size_change_duration,total_cycle_duration()),
							this,SET_FAVORITE_SIZE_AND_LOCATION_CALLBACK);
			};
		} else {
			set_parameter(VACUUM_INERT); // new on 140104
		};
	} else {
		set_parameter(VACUUM_INERT); // new on 010704 since otherwise can end up running in place when dropped at the "wrong" moment
	};
	return(result);
};

void Vacuum::adjust_grasp(city_coordinate &adjustment_x,
								  city_coordinate &adjustment_y) {
   adjustment_x = width/2; // hold his waist...
	adjustment_y = 0;
};

void Vacuum::move_to_hand() { 
	if (inside_vacuum()) { 
		say(IDS_VACUUM_IN_VACUUM_HELP);
		return;
	};
	if (!showing_on_a_screen()) return; // new on 110700 can happen - e.g. vacuum up Tooly and leave Dusty elsewhere
   if (//!tt_running_old_demos && 
       tt_system_mode != PUZZLE) {
      called_since_last_release = TRUE;
   };
	if (running_to_hand() || floor == NULL) { // called twice so hurry up
		// rewrote above as following on 031201
		if (tt_log_version_number > 27) {
			finish_instantly(FALSE,TRUE); // new on 300604 so Dusty comes fast with F2,F2 even with the power off
			// note that other methods for ::move_to_hand just set tt_finish_all_animations = TRUE; instead
         Sprite *programmer_appearance = tt_programmer->pointer_to_appearance(); // was the_programmer
			programmer_appearance->finish_instantly();
			if (tt_log_version_number < 65) {
				// "moved away from here" on 291204 since the hand is still about to grasp and have its parameter set
				if (programmer_appearance->kind_of() != ROBOT) { 
					// conditionalized on 220102 since this is now handled within Robot::pick_up_and_store_tool
					millisecond duration = max(programmer_appearance->total_cycle_duration(),total_cycle_duration())+1; // +1 was +500 prior to 230203
					// programmer_appearance->total_cycle_duration() is new on 130702 and +500 to work around bad selection region
					do_after(duration,this,TELL_PROGRAMMER_AM_READY_CALLBACK); // new on 031201 so that selection region is set properly
				};
			};
		} else { // otherwise do the old thing
			tt_finish_all_animations = TRUE;
		};
		running_to_hand_flag = FALSE;
	} else {
//		if (floor == NULL) return; // hasn't left ToolBox yet
		doing(VACUUM_CALLED);
		running_to_hand_flag = TRUE;
		set_parameter(VACUUM_MORPHING_TO_LIFE);
//		programmer = the_programmer;
		set_priority(floor->next_item_priority());
		millisecond duration = total_cycle_duration();
		do_after(duration,this,WALK_TO_GOAL_CALLBACK);
//		play_sound(HORSE_TROT_SOUND);
	};
};

void Vacuum::dramatic_entrance(millisecond duration) {
   city_coordinate goal_x = llx;
   city_coordinate goal_y = lly;
   city_coordinate goal_width = width;
   city_coordinate goal_height = height;
   // start small off the left side
   set_size_and_location(100,100,-tile_width,my_random(ideal_screen_height));
   animate_size(goal_width,goal_height,duration);
   walk_to_location(goal_x,goal_y,duration);
};  

const long vacuum_speed = 2000; // 2 seconds to cross entire screen

void Vacuum::walk_to_favorite_location(millisecond new_duration) {
	if (tt_screen->inside_thought_bubble()) {
		walk_to_location(default_favorite_llx,default_favorite_lly,new_duration); // new on 011100 that in thought bubble objects are at standard default
	} else {
		walk_to_location(favorite_llx,favorite_lly,new_duration);
	};
};

void Vacuum::walk_to_location(city_coordinate new_goal_x,
										city_coordinate new_goal_y,
										millisecond new_duration) {
	duration = new_duration/2; // half for each dimension
	goal_x = new_goal_x;
	goal_y = new_goal_y;
	walk_to_location();
};

void Vacuum::walk_to_location() {
	city_coordinate horizontal_distance = goal_x-llx;
	city_coordinate vertical_distance = goal_y-lly;
	millisecond new_duration = duration;
	if (vertical_distance == 0) {
		if (horizontal_distance == 0) {
			set_parameter(VACUUM_MORPHING_TO_INERT);
			if (tt_system_mode != PUZZLE) set_priority(max_long-2); // Tooly has max_long-1 (-1 as of 111204)
		} else {
			if (horizontal_distance > 0) {
				if (duration == 0) { // none given
					new_duration = (horizontal_distance*vacuum_speed)/ideal_screen_width;
				};
				set_parameter(VACUUM_WALKING_EAST);
			} else {
				set_parameter(VACUUM_WALKING_WEST);
				if (duration == 0) {
					new_duration = (-horizontal_distance*vacuum_speed)/ideal_screen_width;
				};
			};
			animate_location(goal_x,goal_y,new_duration,this,VACUUM_MORPH_TO_INERT_CALLBACK);
		};
	} else {
		if (vertical_distance > 0) { // moving up or north
			if (duration == 0) {
				new_duration = (vertical_distance*vacuum_speed)/ideal_screen_height;
			};
			set_parameter(VACUUM_WALKING_NORTH);
		} else {
			set_parameter(VACUUM_WALKING_SOUTH);
			if (duration == 0) {
				new_duration = (-vertical_distance*vacuum_speed)/ideal_screen_height;
			};
		};
		animate_location(llx,goal_y,new_duration,this,WALK_TO_LOCATION_CALLBACK);
	};
};

void Vacuum::walk_to_goal() {
	if (!showing_on_a_screen()) return; // new on 110700 can happen - e.g. vacuum up Tooly and leave Dusty elsewhere
	TTRegion hand_region; 
	tt_programmer->pointer_to_appearance()->collision_region(hand_region);
	city_coordinate goal_x = hand_region.min_x-width/2;
	city_coordinate goal_y = hand_region.min_y-height/2;
//	hand_region->region_center(goal_x,goal_y);
	city_coordinate horizontal_distance = goal_x-llx;
	city_coordinate vertical_distance = goal_y-lly;
	millisecond duration; 
	if (vertical_distance == 0) {
		if (horizontal_distance == 0) {
			get_grasped();
		} else {
			if (horizontal_distance > 0) {
				duration = (horizontal_distance*vacuum_speed)/ideal_screen_width;
				set_parameter(VACUUM_WALKING_EAST);
			} else {
				set_parameter(VACUUM_WALKING_WEST);
				duration = (-horizontal_distance*vacuum_speed)/ideal_screen_width;
			};
			city_coordinate arm_llx, arm_lly;
			tt_programmer->pointer_to_appearance()->lower_left_corner(arm_llx,arm_lly);
			animate_location_to(tt_programmer->pointer_to_appearance(),duration,
								     this,VACUUM_GET_GRASPED_CALLBACK,
								     goal_x-arm_llx,goal_y-arm_lly);
		};
	} else {
		if (vertical_distance > 0) { // moving up or north
			duration = (vertical_distance*vacuum_speed)/ideal_screen_height;
			set_parameter(VACUUM_WALKING_NORTH);
		} else {
			set_parameter(VACUUM_WALKING_SOUTH);
			duration = (-vertical_distance*vacuum_speed)/ideal_screen_height;
		};
//      if (tt_finish_all_animations) duration = 0;
		animate_location(llx,goal_y,duration,this,WALK_TO_GOAL_CALLBACK);
      if (!tt_screen->inside_rule_body()) {
         animate_to_size_percent(shrink_percent_from_640x480_screen_size(100),duration+500);
      };
	};
};

void Vacuum::morph_to_inert() {
	if (!held()) {
		set_parameter(VACUUM_MORPHING_TO_INERT);
		if (tt_system_mode != PUZZLE) set_priority(max_long-2); // toolbox has max_long-1 (-1 as of 111204)
	};
};

boolean Vacuum::get_grasped() {	
	stop_sound(FALSE);
	set_parameter(VACUUM_LAYING); 
	// was in the first branch below prior to 071201 but then Programmer_At_Floor::set_tool_in_hand is called with wrong vacuum parameter
	if (Tool::get_grasped()) {
		return(TRUE);
	} else {
		set_parameter(VACUUM_MORPHING_TO_INERT);
		return(FALSE);
	};
};

void Vacuum::stop_running() {
	Tool::stop_running();
	morph_to_inert();
};

void Vacuum::active_region(TTRegion &region) {
	full_region(region);
	// stable despite cycling
	// leftmost 1/20th 
	region.min_x = (19*region.min_x+region.max_x)/20-true_x_offset();
	region.max_x = region.min_x+width/20;
	city_coordinate center_y = (region.min_y+region.max_y)/2-true_y_offset();
   int y_offset = 10; // 10% of height
//#if TT_BETA
//   if (beta4_images()) y_offset = -10;
//#endif
	region.min_y = center_y+y_offset*height/100; // looks better
	region.max_y = region.min_y+height/20;
//	return(&region);
};

//void Vacuum::update_display(const UpdateStatus update_status) { // new on 210901
//	if (held()) {
//		Sprite::update_display(NO_UPDATE_MOVE); // so selection is stable
//	} else {
//		Sprite::update_display(update_status);
//	};
//};

Copier::Copier(city_coordinate x, city_coordinate y) :
  Tool(COPIER_SPRITE,x,y,COPIER),
  attached(NULL),
  counter(-1), // negative is infinite
  saved_counter(-1) {
    state = COPIER_NORMAL;
//	 set_priority_function_of_lly(FALSE);
    city_coordinate button_width, button_height, button_llx, button_lly;
    good_button_size_and_location(button_width,button_height,button_llx,button_lly);
    button = new Button(WAND_BUTTON_SPRITE,
								button_llx,button_lly,
                        'C',priority()-1,
                        button_width,button_height,
                        tt_black);
    button->set_show_all(FALSE);
//	 button->update_display(); // new on 140100 -- commented out on 150704 since causes self copy of wand to have button at wrong place
	 if (tt_log_version_number >= 51) {
		 update_display(); // new on 160804
	 };
    add_follower(button);
    update_button();
	 if (tt_log_version_number >= 51) { // condition new on 080904
		update_button_position(); // seems to be needed as of 160804 otherwise button can be off
	 };
//    recursively_propagate_changes();
};

boolean Copier::ok_to_delete() { // new on 060704 to deal with friends of Tooly getting destroyed (e.g. in a vacuum)
	if (ref_count == 2 && tt_toolbox_copier == this) {
		// only reference remaining is from Tooly so replace that with a fresh one
		if (tt_reincarnate_tools) {
			set_toolbox_copier(new Copier());
			initialize_toolbox_copier(TRUE);
		} else {
			set_toolbox_copier(NULL);
		};
	};
	return(Tool::ok_to_delete());
};

void Copier::good_button_size_and_location(city_coordinate &button_width,
                                           city_coordinate &button_height,
                                           city_coordinate &button_llx,
                                           city_coordinate &button_lly) {
   button_width = width/11;
   button_height = height/3;
	if (tt_log_version_number < 51) {
		button_llx = llx+width/10;
		button_lly = lly+(22*height)/25;
	} else {// updated 160804
		button_llx = llx+width/100;
		button_lly = lly+(10*height)/25;
	};
};

//void Copier::prepare_for_deletion(Sprite *by) {
//  if (prepared_for_deletion()) return; // already done
//  if (attached != NULL) {
//	  if (!tt_shutting_down) { // was commented out but caused problems
//		  remove_follower(attached);
//	  };
//	  attached->destroy();
//	  attached = NULL;
//  };
//  Sprite::prepare_for_deletion(by);
//};

Sprite *Copier::copy(boolean also_copy_offsets) {
	Copier *copy = new Copier(llx,lly);
	copy->set_size(width,height,COPY_INITIALIZATION);
   copy->set_counter(counter);
   copy->set_state(state);
 //  copy->set_priority_fixed(priority_fixed_flag);
	//if (comment != NULL) {
	//	copy->set_comment(comment->copy());
	//};
   finish_making_copy(copy,also_copy_offsets);
	return(copy);
};

//COPIER_NORMAL,COPIER_ORIGINAL,COPIER_SELF,COPIER_LAST_STATE
//string copier_labels = "COS";

Sprite *Copier::respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected,
                                    Sprite *by, boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless) {
   if (attached != NULL) {
      if (key == tt_run_character) { // consistent with other branch?? -- was ' ' prior to 070502
         if (by->still_running()) {
            ((Robot *) by)->really_release_tool();
//         } else if (tt_programmer->kind_of() == PROGRAMMER_AT_FLOOR) {
//				tt_programmer->programmer_at_floor()->release_object(FALSE,TRUE);
				// rewritten on 200105 since the above didn't work if dropping a copy of a box on an untrained robot
			} else {
				tt_programmer->drop_on_floor();
			};
         ::completed(by);
      } else { // is this really useful?? -- yes, e.g. Factorial demo
         // and what if it wants to be replaced??
			// should this just let the following do the recording???
		   attached->respond_to_keyboard(key,extended,robot_selected,by,FALSE,original_recipient,size_constrained_regardless);
         if (record_action) { // commented out the following on 090399 since not bound until below && record_key) {
 //           record_event(KEYBOARD_CHARACTER,by,floor,this,TRUE,key);
				if (by != NULL) {
					by->add_completion(); // new on 270804 since rewrote the above to use Sprite::respond_to_keyboard below
				};
				Tool::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,size_constrained_regardless);
         };
      };
      return(this);
	};
	if (!extended) { // condition new on 120604
		int new_state = state;
		boolean record_key = TRUE;
		int index = state_index(key,tt_copier_keyboard_accelerators,by);
		if (index == state) { // new on 271102 to deal with keyboard characters when already in that state
			key = tt_run_character; // so pretend it was the space bar
			index = -1; // and pretend never saw the index
		};
		if (index >= 0) {
			new_state = index;
			if (new_state != state) doing(COPIER_CHANGED_STATE_VIA_KEYBOARD,by);
			if (held()) {
				turn_on(by);
				record_key = (new_state != state);
				if (record_key && by->kind_of() == ROBOT_IN_TRAINING) {
					// keystroke changes state and turns on (uses) tool
					((Robot *) by)->increment_training_counter();
				};
			};
		} else {
			boolean state_cycling = FALSE; // until proven otherwise -- new on 151002
			if (key ==  tt_run_character) { // was ' ' prior to 070502
				if (held()) {
					turn_on(by);
					record_action = FALSE; // since it'll be used and that'll be recorded -- updated 271102 to prevent the recording even of space
				} else {
					new_state = state+1;
					state_cycling = TRUE;
					did_button_cycle(by);
				}; // else fall
			} else if (key == '+') {
				new_state = state+1;
				did_button_cycle(by);
			} else if (key == '-') {
				if (new_state == COPIER_NORMAL) {
					new_state = COPIER_SELF;
				} else { 
					new_state = state-1;
					state_cycling = TRUE;
				};
				did_button_cycle(by);
			} else if (key >= '0' && key <= '9') {
				if (tt_system_mode != PUZZLE) { // no cheating!
					if (counter <= 0) {
						counter = key-'0';
					} else {
						if (counter > max_long/10) { // added on 090399 as a way to get rid of counter
							counter = -1; // infinity
						} else {
							counter = (counter*10)+key-'0';
						};
					};
					set_clean_status(DIRTY);
				};
			} else if (key == BACKSPACE) {
				if (tt_system_mode != PUZZLE) {
					counter /= 10;
					if (counter == 0) counter = -1; // removed it completely - new on 090399
				};
			} else if (key == toggle_infinite_stack || key == toggle_graspable) {
				if (tt_programmer->kind_of() == PROGRAMMER_AT_FLOOR) {
					// copied from vacuum::respond_to_keyboard on 021204 since vacuum no longer selects
					Sprite *selection = tt_programmer->programmer_at_floor()->pointer_to_current_selection();
					if (selection != NULL) {
						selection->respond_to_keyboard(key,extended,robot_selected,by,FALSE,original_recipient,size_constrained_regardless); 
						// not bothering to record this since this isn't currently a documented feature
						return(this);
					};
				};
			} else {
				if (showing_on_a_screen()) play_sound(PLOP_SOUND); // condition added 170502
				record_key = FALSE;
				key = 0; // new on 080400 since don't want to use control-i here
			};
			if (state_cycling && tt_speak_button_name && by != NULL && by->kind_of() != ROBOT) { // programmer or robot in training
				stop_speaking(); // new on 291003 so if still describing the previous state that is aborted
				speak(IDS_COPIER_STATE0+new_state%COPIER_LAST_STATE); // new on 240902 -- updated to new_state on 291003
				// updated on 260105 to %COPIER_LAST_STATE since otherwise can speak wrong thing (see issue 624)
//				speak(IDS_COPIER_STATE0+state);
			};
		};
		if (record_action) {
			if (record_key) {
				record_equivalent_keyboard_event(state,new_state,tt_copier_keyboard_accelerators,by,floor,this);
			};
			// rewritten on 021100 so the following happens in either case
			record_action = FALSE;
	//      record_event(KEYBOARD_CHARACTER,by,floor,this,TRUE,key);
		};
		set_state(new_state,by);
	};
//   completed(the_action_status);
// commented out the above for the following on 080400
   return(Sprite::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,size_constrained_regardless));
};

void Copier::set_state(int new_state, Sprite *by) {
	if (tt_system_mode == PUZZLE && new_state == COPIER_SELF) {
		// commented out && tt_programmer->kind_of() != ROBOT on 101204 since wasn't a good idea and was the wrong implementation of it
		// broke the puzzle game if a robot was trained to change copier state and you stood up (since PRGRAMMER_ROOM_WALKING == ROBOT)
      new_state = COPIER_NORMAL;
   };
   if (new_state != state) {
      state = (char) (new_state%COPIER_LAST_STATE);
      update_button();
      // moved on 151002 so only when cycling through states
		//if (tt_speak_button_name && by != NULL && by->kind_of() != ROBOT) { // programmer or robot in training
  //       speak(IDS_COPIER_STATE0+state); // new on 240902
		//};
   };
};

char Copier::button_label() {
   const_string labels = S(IDS_COPIER_KEY_COMMANDS);
   return(labels[state]);
};

//void Copier::restore_counter() {
//   if (saved_counter >= 0) { // initialized negative
//      counter = saved_counter;
//   } else {
//      saved_counter = counter;
//   };
//};

//void Copier::add_attached_as_follower() {
//   add_follower(attached,FALSE);
//};

//void add_attached_as_follower_callback(void *c) {
//   ((Copier *) c)->add_attached_as_follower();
//};

Sprite *Copier::used(Sprite *subject, Sprite *by,
							boolean button_changed, boolean record_action,
							millisecond , boolean modified, Sprite *original_subject) {
//#if TT_DEBUG_ON
//	if (by == NULL || by->kind_of() != ROBOT) {
//		debug_this();
//	};
//#endif
	if (attached == NULL) {   // but still need to toss attached (tt_new_input_scheme && )
      modified = FALSE; // 3rd button used differently now
   };
	// commented out the following on 170500 since should be attached to wand
//   if (state == COPIER_SELF) {
//      // cuter would be for this only to work when held
//      // and that the copy is attached...
////      if (tt_system_mode != PUZZLE) { // must be since set_state checks
//         Sprite *new_wand = copy();
//         new_wand->move_by(width/10,-height);
//         floor->add_item(new_wand);
//         play_sound(MAGIC_SOUND);
//#if TT_DIRECT_INPUT
//         if (by != NULL && !by->still_running()) play_force_feedback_effect("Magic Wand",FALSE);
//#endif
//         turn_off(by);
//         if (!by->still_running()) {
//            simulate_mouse_up();
//         };
//     		completed(action_status);
//          return(this);
//      } else {
//         set_state(COPIER_NORMAL); // fix it for them...
//      };
//   };
   millisecond duration = by->default_duration(); // the millisecond arg to ::used is also duration - OK to ignore the caller? asked 280403
   if (state == COPIER_ORIGINAL) modified = TRUE; // same idea
	if (attached == NULL) {
      if (counter == 0) {
         if (showing_on_a_screen()) play_sound(SHUTDOWN_SOUND);
         by->abort_action(IDS_OUT_OF_MAGIC);
         if (by->kind_of() != ROBOT) simulate_mouse_up();
         return(this);
      };
		by->completed();
		if (!by->still_running()) simulate_mouse_up();
		if (state == COPIER_SELF) { // moved here on 170500
			if (by == NULL || by->kind_of() == PROGRAMMER) { 
				// idea is that you can't copy the programmer - just the wand itself
				attached = copy();
			} else {
				Robot *first_robot = ((Robot *) by)->pointer_to_first_in_line();
				Robot *robot_copy;
				if (first_robot != NULL) {
					robot_copy = (Robot *) first_robot->top_level_copy();
				} else {
					robot_copy = (Robot *) by->top_level_copy();
				};
				// following is new on 140101 so clear that the robot copy is of this and also needed for some operations
				if (robot_copy->pointer_to_thought_bubble() == NULL && // e.g. while training
					 floor != NULL) {
					Cubby *cubby = floor->pointer_to_body_cubby();
					if (cubby != NULL) {
						Thought_Bubble *thought_bubble_copy = new Thought_Bubble();
						thought_bubble_copy->set_cubby((Cubby *) cubby->top_level_copy(),FALSE);
						robot_copy->connect_to_thought_bubble(thought_bubble_copy);
					};
				};
				attached = robot_copy;
				subject = NULL; // ignored in any case
			};
		} else {
			if (subject == NULL || subject->kind_of() == MOUSE_WITH_HAMMER || subject->kind_of() == MARTIAN) {
				// added Martian test on 200603 since with F8 can copy Marty
				return(this); // can't copy the floor or the mouse
			};
			tt_copy_restores_pre_blank = modified;
			attached = subject->top_level_copy();
			tt_copy_restores_pre_blank = FALSE;
		};
		if (attached == NULL) return(this); // refused to be copied
		attached->increment_ref_count(this); // new on 271100 -- I would have thought this is redundant (leading to a memory leak) with set_attached
		// - but apparently not (tried removing it on 220702)
      if (counter > 0) counter--;
//      boolean size_is_animating = FALSE; // until proven otherwise
		if (duration > 0) {
			play_sound(MAGIC_SOUND);
#if TT_DIRECT_INPUT
         if (by != NULL && !by->still_running()) play_force_feedback_effect("Magic Wand",FALSE);
#endif
      };
      // following was part of the duration > 0 conditional prior to 061102
		if (subject == NULL || // new on 170500 since might be copying self
				subject->pointer_to_leader() != NULL && // taking it out of something
				(subject->pointer_to_leader()->kind_of() != PICTURE ||
				subject->pointer_to_leader()->is_flipped())) {
         if (duration > 0) {
			// and it is not an unflipped picture
			   attached->recursively_notice_on_screen_change(TRUE); // in case item is lazy in computing things like Integer is
//				attached->animate_to_good_size(duration,subject);
         // animation above messed up when the wand was not a good size
            attached->set_background(floor,FALSE,FALSE); // so good_size knows what it'll soon be
//				if (tt_log_version_number > 12) {
				city_coordinate old_width = attached->current_width();
				city_coordinate old_height = attached->current_height();
				// following updated on 141204 to use Tooly as source (rather than always NULL)
				attached->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL); // didn't take source into account
				if (tt_log_version_number >= 34) { // new on 180204
					attached->move_by((old_width-attached->current_width())/2,(old_height-attached->current_height())/2); // stay centered
				};
//				} else {
//               attached->set_to_good_size(subject);
//				};
            attached->set_background(NULL);
//            size_is_animating = TRUE;
				// following commented out on 040503 since this is costly even if happening in another room (e.g. large numbers computing their current_text)
				// retored for pictures since they may be copied and dropped in boxes and need to have the "correct" size
			} else if (floor != NULL && floor->pointer_to_picture() == NULL && attached->kind_of() == PICTURE) { 
				// conditional new 160103 - why waste time otherwise
            attached->set_to_good_size(tt_toolbox); // tt_toolbox new on 020205
				// new on 061102 so size is good whether watched or not
         };
		};
		if (modified) {
			doing(COPIER_REMOVED_BLANKS,by);
		} else {
			doing(COPIER_APPLIED,by);
		};
      if (record_action) {
		   record_event(COPIER_APPLY,by,floor,subject,FALSE,(char) modified);
		   // following is just to create an index
      };
      // following maintains working sets
		if (subject != NULL && subject->pointer_to_leader() != NULL && subject->pointer_to_leader()->kind_of() == PROGRAM_PAD) { // new on 180204
			// copying a page from a notebook with the wand (rather than implicitly) should have the same behaviour
			// added subject != NULL on 050304 since if copying self it will be NULL
			record_event(NEW_ITEM,by,floor,attached,TRUE,(char) modified,ADD_TO_WORKING_SET_REGARDLESS);
		} else {
			record_event(NEW_ITEM,by,floor,attached,FALSE,(char) modified); // shouldn't this also be ADD_TO_WORKING_SET_REGARDLESS - asked 180204
		};
		if (current_held_status() == HELD_BY_PROGRAMMER) {
			attached->set_priority(tt_programmer->pointer_to_appearance()->priority()-1);
		} else {
		   attached->set_priority(current_priority-1); 
		};	
		if (duration > 0) { // conditional new on 280403 to avoid unncessary work
			if (tt_log_version_number >= 35) { // new on 180204 to place better the newly copied object
				// changed from 34 to 35 on 210204
//				attached->move_by(-tile_width/2,-tile_height/4);
			} else if (leader != NULL && (leader->kind_of() == ROBOT || leader->kind_of() == ROBOT_IN_TRAINING)) {
				// just above it on the chest with same width
//				attached->set_width_maintain_proportionality(width);
				attached->move_to(llx,lly+(2*height)/3); // was 4/3 height
//				attached->set_priority(leader->priority()-1); // on top
			} else { // used by programmer
//				floor->give_item_priority(attached);
//				attached->animate_to_saved_size(1000);
				// move so that middle touches wand end
				// and it is vertically centered around the wand
				// adding width/3 since llx is too far left
	//			attached->move_to(llx+width/3-attached->true_width()/2,
	//									lly+(height+attached->true_height())/2);
				Sprite *hand_with_wand = tt_programmer->pointer_to_appearance();
				city_coordinate attached_width;
				if (subject == NULL || subject->pointer_to_leader() == NULL || duration == 0) {
					attached_width = attached->true_width();
				} else {
					attached_width = attached->good_width();
				};
				attached->move_to(hand_with_wand->current_llx()-attached_width/2,
										hand_with_wand->current_lly());
			};
		};
		set_attached(attached,FALSE);
      turn_off(by); // wand works just once when turned on
		// following commented out on 140403
//	} else if (modified) {
//		// if picture is attached this will try to toss it
//      // not sure if this should get recorded??
//		if (attached->used(subject,by,button_changed,FALSE,duration,modified,original_subject) == NULL) {
//			if (tt_programmer->kind_of() == PROGRAMMER_AT_FLOOR) { // how could it be otherwise?
//				tt_programmer->programmer_at_floor()->release_object(FALSE,TRUE);
//			};
////			released(by,screen)->now_on_floor();
//		};
	};
	return(this);
};

void Copier::set_attached(Sprite *item, boolean just_set) { // new on 170100
//	if (item == attached) return; // new on 200202 - had been commented out but messed up reference count scheme (restored on 170702)
// above commented out again on 180702 since need to do some of this work while copying
// instead the ref count actions are now conditionalized with item != attached
   boolean different = (item != attached); // there was an old attachment 
   if (attached != NULL && different) { // new on 271100
		attached->destroy(this); // release reference
	};
	attached = item;
	if (!just_set && item != NULL) {
		attached->set_priority_fixed(TRUE); // taken care of by wand until dropped
		add_follower(attached,FALSE,INSERT_BY_PRIORITY); // moved earlier so that good_size knows its background
		if (floor == tt_screen->pointer_to_background()) {
			tt_screen->add(attached);
		};
	};
	if (attached != NULL && different) {
		attached->increment_ref_count(this); // new on 271100
	};
};

void Copier::remove_selected_element(Sprite *element, SelectionReason reason, Sprite *by, boolean add_to_floor) {
	// new on 271100
	if (element == attached) {
		set_attached(NULL,TRUE);
	};
	Tool::remove_selected_element(element,reason,by,add_to_floor);
};

void Copier::finished_loading() {
	// postponed this until copier geometry is known so add_follower gets the right relationship
	set_attached(attached,FALSE);
};

/*
	} else if (attached->kind_of() == INTEGER) {
		 // consider number as part of the tool
//		 if (subject == attached) subject = NULL;
		 Integer *attached_number = (Integer *) attached;
		 if (attached_number->is_error()) return;
		 long attacheds_value,subjects_value;
//       boolean attached_positive, subject_positive;
		 switch(subject->kind_of()) {
			 case INTEGER:
				// multiply subject by attached
				if (attached_number->current_value(attacheds_value)) {
					Integer *number = (Integer *) subject;
					if (number->is_error()) return;
					if (number->current_value(subjects_value)) {
						long product = attacheds_value*subjects_value;
						if (product/attacheds_value != subjects_value) {
							if (duration > 0) play_sound(B_WOP_SOUND);
							number->become_error();
						} else {
							if (duration > 0) play_sound(POP_SOUND);
							number->set_value(product);
						};
						record_event(COPY_NUMBER_APPLY,by,floor,subject);
					};
				};
				break;
			 case CUBBY:
				if (attached_number->current_value(attacheds_value)) {
					if (duration > 0) play_sound(MAGIC_SOUND);
					((Cubby *) subject)->set_number_of_holes(attacheds_value);
					record_event(COPY_NUMBER_APPLY,by,floor,subject);
				};
				break;
		 };
*/

ReleaseStatus Copier::item_released_on_top(Sprite *item, Sprite *by, SpritePointer &recipient, int level) {
    if (leader != NULL) {
       recipient = leader;
       return(leader->item_released_on_top(item,by,recipient,level));
    } else {
       return(Tool::item_released_on_top(item,by,recipient,level));
    };
};

void Copier::display(SelectionFeedback selection_feedback, boolean followers_too, TTRegion *region) {
//#if TT_DEBUG_ON
//	// 250803
//	tt_error_file() << "Copier's position is " << llx << "," << lly << " on frame " << tt_frame_number << endl;
//	tt_error_file() << " its button's position is " << button->current_llx() << "," << button->current_lly() << endl;
//#endif
//	if (rescale_if_camera_above(selection_feedback,followers_too,region)) return; -- commented out on 160804
   Tool::display(selection_feedback,followers_too,region);
   city_coordinate counter_llx = llx+(71*width)/100;
   city_coordinate counter_lly = lly+(87*height)/100;
   if (want_feedback(selection_feedback)) {
		 counter_llx += selection_delta_x();
       counter_lly += selection_delta_y();
   };
   display_counter(counter_llx,counter_lly);
};

void Copier::display_counter(city_coordinate counter_llx, city_coordinate counter_lly) {
   if (counter >= 0) {
//      string string_counter = new char[15];
      character string_counter[15];
      _itot (counter,string_counter,10);
      city_coordinate font_width, font_height;
      if (counter < 100) {
         font_width = height/3;
         font_height = (7*height)/10;
      } else {
         font_width = height/6;
         font_height = (7*height)/20;
      };
BEGIN_GDI
	   tt_screen->text_out(string_counter,_tcslen(string_counter),
                          counter_llx,counter_lly,
							     font_width,font_height,TRUE);
END_GDI
   };
};

void Copier::java(java_stream &out) {
   out << S(IDS_JAVA_TT_MAGIC_WAND);
};

#if TT_XML

//xml_element *Copier::xml_create_element(xml_document *document) {
//   return(create_xml_element(xml_tag(),document)); // updated on 090703 to xml_tag
//};

void Copier::xml(xml_element *element, xml_document *document) {
//   xml_set_attribute(element,L"StateCode",(int) state); // moved up to Tool::xml on 180703
   if (counter >= 0) {
      xml_append_child(create_xml_element(L"Counter",document,counter),element);
   };
   if (attached != NULL) {
      attached->add_xml(element,L"Attached",document);
   };
   if (toolbox_tool(this)) {
//      xml_append_child(create_xml_element(L"FriendOfTooly",document),element);
      xml_set_attribute(element,L"LivesInTooly",1); // new on 171102
   };
   Tool::xml(element,document);
};

boolean Copier::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
      //case FRIEND_OF_TOOLY_TAG: { // made obsolete on 171102
      //   if (tt_loading_a_city) {
      //      if (tt_toolbox == NULL) {
				  // tt_toolbox = new Toolbox();
			   //}; 
			   //Copier *old_copier = tt_toolbox_copier; 
			   //set_toolbox_copier(this);
			   //if (old_copier != NULL) {
      //         FavoriteSizeAndLocation f;
      //         old_copier->current_favorite(f); // new on 171102 to transfer favorite settings
      //         set_favorite(f);
				  // old_copier->destroy();
			   //};
      //   };
      //   return(TRUE);
      //                          };
		case ATTACHED_TAG: // new on 181202
			if (attached != NULL) { // new on 070103
				remove_follower(attached); // new on 260904
				attached->destroy();
			};
			attached = xml_get_sprite(node);
			if (attached != NULL) {
				add_follower(attached,FALSE,INSERT_BY_PRIORITY,TRUE);
				if (tt_screen != NULL) tt_screen->add(attached); // new on 260904
				attached->increment_ref_count(this);
			}; // else warn?
			return(TRUE); 
      case COUNTER_TAG:
         set_counter(xml_get_int(node,-1));
         return(TRUE);
		case NO_MORE_TAGS:
			Tool::handle_xml_tag(tag,node); // before the following so geometry is all up-to-date
			if (tt_log_version_number >= 51) { // condition new on 080904
				update_display(); // new on 080904 since otherwise button is wrong place if saved at different resolution than is current
				update_button_position(); // new on 160804
			};
			return(TRUE);
     default:
         return(Tool::handle_xml_tag(tag,node));
   };
};

#endif

boolean Copier::dump(output_stream &stream, boolean just_prepare, boolean is_controlee, boolean on_nest) {
   if (!Tool::dump(stream,just_prepare,is_controlee,on_nest)) return(FALSE);
	dump_if_not_null(stream,comment);
   stream.put((unsigned char) state);
   stream.write((string) &counter,sizeof(counter));
	stream.put(toolbox_tool(this)); // new on 050100
	dump_if_not_null(stream,attached); // new on 170100
	if (attached != NULL) attached->dump_geometry(stream);
	return(TRUE);
};

Copier *load_copier(InputStream *stream, int notebook_version, NaturalLanguage language, long index, boolean descendants_inactive) {
   Copier *copier = new Copier(0,0);
	associate_shared_item(index,copier);
	if (notebook_version >= first_version_to_save_copied_nests) {
		copier->set_comment(load_item(stream,notebook_version,language));
	};
   copier->set_state(stream->get());
   long counter;
   stream->read((string) &counter,sizeof(counter));
   copier->set_counter(counter);
	if (notebook_version >= first_version_to_save_programmer_and_remotes_accurately) { // new on 050100
		boolean friend_of_tooly = stream->get(); 
		if (friend_of_tooly && tt_loading_a_city && !tt_loading_to_ignore) { // second condition new on 181200 to protect tools from bad encodings
			if (tt_toolbox == NULL) {
				tt_toolbox = new Toolbox();
				tt_toolbox->initialize();
			};
			Copier *old_copier = tt_toolbox_copier; // new on 261000
			set_toolbox_copier(copier);
			if (old_copier != NULL) {
				old_copier->destroy();
			};
		};
		Sprite *attached = load_item(stream,notebook_version,language); // new on 170100
		if (attached != NULL) {
			attached->load_geometry(stream,notebook_version);
			copier->set_attached(attached,TRUE); 
		};
	};
   return(copier);
};

boolean describe_copier(InputStream *pad_in, output_stream &text_out, int version_number, Article article) {
	if (version_number >= first_version_to_save_copied_nests) {
		describe_comment_encoding(pad_in,text_out,version_number,COPIER);
	};
   CopierState state = (CopierState) pad_in->get(); // describe this later
   long counter;
   pad_in->read((string) &counter,sizeof(counter));
   print_type(COPIER,text_out,article);
   if (counter >= 0) {
      text_out << space() << S(IDS_WHICH_HAS_ENOUGH_MAGIC) << space(SPACE_BEFORE_OR_AFTER_NUMBERS) 
					<< counter << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
      if (counter == 1) {
        text_out << S(IDS_USE);
      } else {
        text_out << S(IDS_USES);
      };
		text_out << S(IDS_WHICH_HAS_ENOUGH_MAGIC2,"");
   };
   text_out << space() << S(IDS_THAT_IS_READY_TO,"")
            << space() << S(IDS_COPIER_STATE0+state)
				<< S(IDS_THAT_IS_READY_TO2,"");
	if (version_number >= first_version_to_save_programmer_and_remotes_accurately) { // new on 050100
		pad_in->get(); // ignore for now whether this is a "friend" of Tooly
	};
   return(TRUE);
};

void Copier::dramatic_entrance(millisecond duration) {
   city_coordinate goal_x = llx;
   city_coordinate goal_y = lly;
   city_coordinate goal_width = width;
   city_coordinate goal_height = height;
   // start shortened off the right side
   set_size_and_location(goal_width,100,21*tile_width,my_random(ideal_screen_height));
   animate_size_and_location(goal_width,goal_height,goal_x,goal_y,duration);
};  

void Copier::move_to_hand() { 
	if (inside_vacuum()) {
		say(IDS_WAND_IN_VACUUM_WHEN_CALLED);
		return;
	};
   if (tt_system_mode != PUZZLE) {
      called_since_last_release = TRUE;
   };
   //	if (floor == NULL) return; // hasn't left ToolBox yet
	doing(COPIER_CALLED);
	if (running_to_hand() || floor == NULL) { // called twice so hurry up
		tt_finish_all_animations = TRUE;
		running_to_hand_flag = FALSE;
//		tt_programmer->pointer_to_appearance()->finish_instantly();
//		finish_instantly();
	} else {
//		finish_instantly(); // new on 050603 in case already animating (e.g. leaving Tooly)
		running_to_hand_flag = TRUE;
		set_priority(floor->next_item_priority());
//		programmer = the_programmer;
		city_coordinate goal_x, goal_y;
		Sprite *appearance_sprite = tt_programmer->pointer_to_appearance();
		appearance_sprite->lower_left_corner(goal_x,goal_y);
		millisecond duration = duration_delta_location(llx,lly,goal_x,goal_y)/2;
		animate_location_to(appearance_sprite,duration,this,COPIER_GET_GRASPED_CALLBACK,
							     appearance_sprite->current_width()/2-width,
							     appearance_sprite->current_height()/2);
	};
};

Sprite *Copier::if_released() {
  if (attached == NULL) {
	  return(this);
  } else {
	  return(attached);
  };
};	  

void Copier::generate_help(output_stream &message, Sprite *tool_in_hand,
                           Sprite *selection) {
	if ((attached == NULL && !did(COPIER_APPLIED) && tt_help_counter == 0)
		 || tt_help_counter == 1) {
		Sprite::generate_help(message,tool_in_hand,selection);
		tt_help_history_action = COPIER_APPLIED;
		message << S(IDS_COPIER_APPLIED_HELP);
		if (tt_help_counter == 1 || selection == this || selection == NULL) {
			message << S(IDS_COPIER_APPLIED_HELP2);
		} else {
			selection->describe_type(message,DEFINITE_ARTICLE);
		};
		message << S(IDS_COPIER_APPLIED_HELP3,"");
		describe_right_button(message);
		message << S(IDS_COPIER_APPLIED_HELP3_2,""); // new on 270100
		message << end_sentence();
//      if (!tt_new_input_scheme) message << S(IDS_TURN_COPIER_ON_HELP);
//      if (tt_new_input_scheme) describe_space_bar_alternatives(message);
	} else if ((attached != NULL && !did(COPIER_ATTACHED_RELEASED) 
					&& tt_help_counter == 0)
				  || tt_help_counter == 2) {
		tt_help_history_action = COPIER_ATTACHED_RELEASED;
		message << S(IDS_COPIER_RELEASED_HELP);
		if (tt_help_counter == 2) {
			message << S(IDS_COPIER_RELEASED_HELP2);
		} else {
			attached->describe_type(message,DEFINITE_ARTICLE);
		};
		message << S(IDS_COPIER_RELEASED_HELP3,"") << space();
		describe_left_button(message);
		message << S(IDS_COPIER_RELEASED_HELP3_2,""); // new on 270100
		message << end_sentence();
		message << S(IDS_COPIER_RELEASED_HELP4);
	} else if ((!did(COPIER_CALLED) && tt_help_counter == 0)
		 || tt_help_counter == 3) {
		tt_help_history_action = COPIER_CALLED;
		message << S(IDS_COPIER_CALLED_HELP);
		describe_hand(message);
		message << S(IDS_COPIER_CALLED_HELP2);
		message << S(IDS_COPIER_CALLED_HELP3);
    } else if ((!did(COPIER_CHANGED_STATE_VIA_BUTTON) && tt_help_counter == 0)
		        || tt_help_counter == 4) {
		tt_help_history_action = COPIER_CHANGED_STATE_VIA_BUTTON;              
      message << S(IDS_COPIER_CHANGED_STATE_VIA_BUTTON_HELP);
   } else if ((!did(COPIER_CHANGED_STATE_VIA_KEYBOARD) && tt_help_counter == 0)
		        || tt_help_counter == 4) {
		tt_help_history_action = COPIER_CHANGED_STATE_VIA_KEYBOARD;
      message << S(IDS_COPIER_CHANGED_STATE_VIA_KEYBOARD_HELP);
	} else if ((!did(COPIER_REMOVED_BLANKS) && 
	            did_user(VACUUM_USED_TO_MAKE_BLANK,VACUUM)
				   && tt_help_counter == 0)
		        || tt_help_counter == 5) {
		tt_help_history_action = COPIER_REMOVED_BLANKS;
		message << S(IDS_IF_YOU_PRESS) << space();
//      if (state == COPIER_ORIGINAL && !tt_new_input_scheme) {
//		   describe_middle_button(message);
//      } else {
         const_string commands = S(IDS_COPIER_KEY_COMMANDS);
         message << "'" << commands[COPIER_ORIGINAL] << "'";
//         message << S(IDS_CHAR_O);
//      };
		message << comma() << space() << S(IDS_COPY_BLANKS_TO_NON_BLANKS);
	} else if ((!did(TYPE_HELP_GIVEN) && tt_help_counter == 0)
		 || tt_help_counter == 6) {
		message << S(IDS_COPIER_HELP_GIVEN_HELP);
		message << S(IDS_COPIER_HELP_GIVEN_HELP2);
		no_help_interruptions();
		doing(TYPE_HELP_GIVEN);
	};
};

Sprite *Copier::released(Sprite *by, boolean top_level, SpecialDrop special_drop) {
	if (attached == NULL) { // not "holding" anything
      if (called_since_last_release) {
         travel_to_favorite_location(1500);
      } else if (by->kind_of() == PROGRAMMER) {
         set_favorite_size_and_location();
      };
      if (by->kind_of() == PROGRAMMER) set_size_percent(shrink_percent_from_640x480_screen_size(100)); // and animate from there
      const millisecond animation_duration = 1200;
		if (!by->still_running()) {
			animate_to_favorite_size(animation_duration);
			do_after(animation_duration,this,SINK_DOWN_CALLBACK);
		};
		return(Tool::released(by,top_level,special_drop)); // release me
	} else {
		if (!by->still_running()) {
			simulate_mouse_up(); // so I don't release both right away
		};
      // do the release while attached is still an item of the floor
		Sprite *released = attached->released(by);
//		if (leader == NULL || leader->kind_of() != ROBOT) {
			// robot takes care of this itself
//			remove_follower(attached);
//		};
//		Sprite *to_release = attached;
      attached->set_priority_fixed(FALSE);
		remove_follower(attached,FALSE);
		if (special_drop == GIVE_TOSS_DROP && attached->kind_of() == PICTURE) { // new on 180300
			((Picture *) attached)->tossed(by);			
		};
		attached->tell_flipped_followers_now_on_back(); // new on 101203 so that if dropping a picture on a picture its remotes get hooked up
		attached->destroy(this); // new on 271100 released reference
		attached = NULL;
		doing(COPIER_ATTACHED_RELEASED,by);
		return(released);
	};
};

void Copier::set_priority(long new_priority) {
   Tool::set_priority(new_priority);
   if (attached != NULL) {
	   attached->set_priority(current_priority-1);
	};
};

void Copier::active_region(TTRegion &region) {
	// used when robot is in training
	full_region(region);
	// leftmost 1/20th 
	region.min_x = (19*region.min_x+region.max_x)/20;
	region.max_x = region.min_x+width/20;
	city_coordinate center_y = (region.min_y+region.max_y)/2-true_y_offset();
	region.min_y = center_y-height/20;
	region.max_y = region.min_y+height/20;
};

void Copier::travel_to_favorite_location(millisecond duration) {
	if (tt_screen->inside_thought_bubble()) {
		animate_location(default_favorite_llx,default_favorite_lly,duration); // new on 011100 that in thought bubble objects are at standard default
	} else {
		animate_to_favorite_location(duration);
	};
};

enum ToolboxStates {TOOLBOX_INERT_RANDOM=8,
						  TOOLBOX_TURNING_CLOCKWISE,    
						  TOOLBOX_TURNING_COUNTER_CLOCKWISE,
						  TOOLBOX_INERT_OPEN, 
						  TOOLBOX_MORPHING_TO_INERT,
						  TOOLBOX_MORPHING_TO_LIFE};

const int number_of_rows = 4;
const int number_of_columns = 2;

Toolbox::Toolbox() : // Notebook *initial_notebook) :
	Sprite(TOOLBOX_ABOVE_SPRITE,0,0,TOOLBOX,TOOLBOX_INERT_RANDOM,ground_scale,0,0),
   //vacuum(NULL),
   //expander(NULL),
   //copier(NULL),
   //notebook(initial_notebook), // ref count is dealt with below
	cached_width(0),
	cached_height(0),
	background_cached(FALSE),
	offstage_flag(FALSE),
	leave_floor_when_offstage(FALSE),
	called_since_last_release(FALSE),
	running_to_hand_flag(FALSE),
	saved_followers(NULL),
	goal_x(0), // not initialized prior to 060103
	goal_y(0),
	frame_to_check_tt_file_name_to_clipboard(-1), // new on 101203
	gone_for_good_flag(FALSE) { 
		// commented out on 150499 so stacks in Tooly can be used while running puzzles
//   if (tt_system_mode == PUZZLE &&
//       (!is_toolbox_part_of_puzzle() || inside_rocket())) return;
	if (tt_good_sizes_adjust_to_screen_size) {
		set_size(shrink_width_from_640x480_screen_size(width),
					shrink_height_from_640x480_screen_size(height));
	};
	for (int i = 0; i < TOOLBOX_STACK_COUNT; i++) {
		stacks[i] = NULL;
	};
};

boolean Toolbox::ok_to_delete() { // new on 070704 to deal with  Tooly getting destroyed (e.g. in a vacuum)
	boolean will_be_deleted = Sprite::ok_to_delete();
	if (tt_toolbox == this && will_be_deleted) {
		if (tt_reincarnate_tools) {
			tt_toolbox = new Toolbox();
//			set_toolbox_vacuum(vacuum);
//			vacuum = NULL;
//			set_toolbox_copier(copier);
//			copier = NULL;
//			set_toolbox_expander(expander);
//			expander = NULL;
//			set_toolbox_notebook(notebook);
//			notebook = NULL;
			tt_toolbox->initialize();
		} else {
			inside_vacuum_flag = FALSE; // since can't be spit out again since is really destroyed
			gone_for_good_flag = TRUE;
			return(FALSE); // new on 120704 to keep old one around but make sure it is never seen
		};
	};
	return(will_be_deleted);
};

void Toolbox::initialize() {
	// seperated from the above on 161202 so tt_toolbox is bound before 
	// running the following which might require it (e.g. BOK notebook)
	if (tt_toolbox_expander == NULL) { // added conditional on 160103
		set_toolbox_expander(new Expander(6*tile_width,12*tile_height)); 
		// was tt_toolbox_expander =  prior to 240105
		tt_toolbox_expander->increment_ref_count(this); // new on 280300
	};
	initialize_toolbox_expander();
	if (tt_toolbox_vacuum == NULL) { // conditional new on 160103
		set_toolbox_vacuum(new Vacuum(10*tile_width,12*tile_height));
		// prior to 240105 was tt_toolbox_vacuum = 
		tt_toolbox_vacuum->increment_ref_count(this); // new on 280300
	};
	initialize_toolbox_vacuum();
	if (tt_toolbox_copier == NULL) {
		set_toolbox_copier(new Copier(17*tile_width,16*tile_height));
		// prior to 240105 was tt_toolbox_copier = 
		tt_toolbox_copier->increment_ref_count(this); // new on 280300
	};
	initialize_toolbox_copier();
//	notebook = NULL; // new on 241199 initialized later perhaps from the log
//	pointer_to_notebook(); // update and record now
	if (tt_toolbox_notebook == NULL) {
		tt_toolbox_notebook = new Notebook(0,0,0,copy_ascii_string("bok")); // beginning 210802 the caller copies the file name
//		notebook->dont_ref_count(); // new on 260803 // commented out as an experiment on 130103 to fix a leak
		//// rewritten on 161202 to avoid this being recursively called from creating BOK notebook
		//notebook->set_file_name(copy_ascii_string("bok"));
		//notebook->load();
//		tt_error_file() << (int) tt_toolbox_notebook->kind_of() << " " 
		// << (int) tt_toolbox_notebook->appearance_code() << " " << tt_toolbox_notebook->image_cycle_index() << endl;
		tt_toolbox_notebook->increment_ref_count(this); 
		// toolbox and typically floor -- moved here on 070704 since no need to do this if an old notebook
	};
	initialize_toolbox_notebook();
	boolean need_to_refit_contents = FALSE;
	if (stacks[TOOLBOX_INTEGER_STACK] == NULL) {
		NUMBER *number = new NUMBER(0L,0,0);//,9*tile_width);
		number->set_long_value(1,FALSE,TRUE); // args added on 060202 so doesn't bother to compute size
		number->set_shrinking_decimal_notation(TRUE); // new on 230603
		number->update_text_and_widths(TRUE);
		stacks[TOOLBOX_INTEGER_STACK] = number;
		need_to_refit_contents = TRUE; // if one then all
	};
	if (stacks[TOOLBOX_CUBBY_STACK] == NULL) {
		stacks[TOOLBOX_CUBBY_STACK] = new Cubby(0,0,0,0,4*tile_height);
	};
	if (stacks[TOOLBOX_NEST_STACK] == NULL) {
		stacks[TOOLBOX_NEST_STACK] = new Nest(0,0,0); //,5*tile_width);
	};
//	thought_bubble_stack	= new Thought_Bubble(0,0,0,floor);//6*tile_width);
	if (stacks[TOOLBOX_SCALE_STACK] == NULL) {
		stacks[TOOLBOX_SCALE_STACK] = new Picture(0,0,0,0,SCALE_SPRITE);
		stacks[TOOLBOX_SCALE_STACK]->set_show_all(FALSE); // see through
		stacks[TOOLBOX_SCALE_STACK]->set_parameter(GREATER_THAN_SCALE);
	};
	if (stacks[TOOLBOX_ROBOT_STACK] == NULL) {
		stacks[TOOLBOX_ROBOT_STACK] = new Robot(0,0,0); //,5*tile_width);
	};
	if (stacks[TOOLBOX_TRUCK_STACK] == NULL) {
		stacks[TOOLBOX_TRUCK_STACK] = new Truck_Inside(0,0,0); //5*tile_width);
	};
	if (stacks[TOOLBOX_BOMB_STACK] == NULL) {
		stacks[TOOLBOX_BOMB_STACK] = new Bomb(0,0,0);
	};
	if (stacks[TOOLBOX_TEXT_STACK] == NULL) {
		Text *text = new Text();
		text->set_text("A");
		stacks[TOOLBOX_TEXT_STACK] = text;
	};
	if (need_to_refit_contents) { // condition new on 250803
		refit_contents();
	};
	for (int column = 0; column < number_of_columns; column++) {
		for (int row = 0; row < number_of_rows; row++) {
			Sprite *contents = compartment_contents(row,column);
			contents->set_infinite_stack(TRUE);
			if (contents->pointer_to_leader() == NULL) { 
				// condition new on 271204 -- since can get initialized twice (if loading old demos at least)
				add_follower(contents);
			};
		};
	};
	move_to(-ideal_screen_width,ideal_screen_height/4);
   favorite_llx = tile_width*12;
//   if (tt_running_old_demos) {
//      favorite_lly = ideal_screen_height/3;
//   } else {
      favorite_lly = 8*tile_height;
//   };
	default_favorite_llx = favorite_llx;
   default_favorite_lly = favorite_lly;
   // following rewritten on 161102
   if (favorites_set(VACUUM_FAVORITE) && tt_toolbox_vacuum != NULL) {
      tt_toolbox_vacuum->set_favorite(tt_favorites[VACUUM_FAVORITE]);
   };
   if (favorites_set(EXPANDER_FAVORITE) && tt_toolbox_expander != NULL) {
      tt_toolbox_expander->set_favorite(tt_favorites[EXPANDER_FAVORITE]);
   };
   if (favorites_set(COPIER_FAVORITE) && tt_toolbox_copier != NULL) {
      tt_toolbox_copier->set_favorite(tt_favorites[COPIER_FAVORITE]);
   };
   if (favorites_set(TOOLBOX_FAVORITE)) {
      set_favorite(tt_favorites[TOOLBOX_FAVORITE]);
      set_size(favorite_width,favorite_height);
   } else {
      good_size(favorite_width,favorite_height); // initialize toolbox favorites
   };
};

//void Toolbox::toontalk_reset() { // new on 050204 -- obsolete 100204
//	// initially just used to make sure the right version of the main notebook is loaded during time travel
//	//if (tt_load_new_main_notebook_files) {
//	//	notebook->release_all_references(); // prior to 060204 was (this) -- i.e. by
//	//	notebook->load();
//	//	tt_load_new_main_notebook_files = FALSE;
//	//};
//	// rewritten on 090204 since the above wasn't reliable or stable
//	if (notebook != NULL) {
//		notebook->destroy();
//		notebook = NULL; // no point doing the following until needed
////		pointer_to_notebook(); // this will recreate it if needed
//	};
//};


// made global variants in utils.cpp on 080704
//void Toolbox::set_notebook(Notebook *new_notebook) {
//	Notebook *old_notebook = notebook;
//	notebook = new_notebook;
//	if (old_notebook != NULL) { // moved here on 251000 since otherwise is protected by tt_toolbox
//		old_notebook->destroy();
//	};
//	if (notebook != NULL) notebook->increment_ref_count(this);
//};
//
//void Toolbox::set_expander(Expander *tool) {
//   Expander *old_expander = expander;
//	expander = tool;
//	if (old_expander != NULL) { // moved here on 251000 since otherwise is protected by tt_toolbox
//		old_expander->remove_from_floor(FALSE);
//		old_expander->destroy();
//	};
//	if (expander != NULL) expander->increment_ref_count(); // new on 261000
//};
//
//void Toolbox::set_vacuum(Vacuum *tool) {
//	Vacuum *old_vacuum = vacuum;
//	vacuum = tool;
//	if (old_vacuum != NULL) { // moved here on 251000 since otherwise is protected by tt_toolbox
//		old_vacuum->remove_from_floor(FALSE);
//		old_vacuum->destroy();
//	};
//	if (vacuum != NULL) vacuum->increment_ref_count(); // new on 261000
//};
//
//void Toolbox::set_copier(Copier *tool) {
//	Copier *old_copier = copier;
//	copier = tool;
//	if (old_copier != NULL) {
//		old_copier->remove_from_floor(FALSE); // moved here on 251000 since otherwise is protected by tt_toolbox
//		old_copier->destroy();
//	};
//	if (copier != NULL) copier->increment_ref_count(); // new on 261000
//};

void Toolbox::refit_contents() {
	city_coordinate compartment_width, compartment_height,
						 compartment_llx, compartment_lly;
	for (int column = 0; column < number_of_columns; column++) {
		for (int row = 0; row < number_of_rows; row++) {
			compartment_size_and_location(row,column,
													compartment_width,compartment_height,
													compartment_llx,compartment_lly);
			Sprite *contents = compartment_contents(row,column);
			contents->set_size_and_location(compartment_width,compartment_height,
													  compartment_llx,compartment_lly,TO_FIT_INSIDE);
		};
	};
	set_clean_status(DIRTY);
};

Sprite *Toolbox::pointer_to_stack(SpriteType type) {
	switch (type) {
		case INTEGER:
			return(pointer_to_integer_stack());
		case TEXT:
			return(pointer_to_text_stack());
		case CUBBY:
			return(pointer_to_cubby_stack());
		case NEST:
			return(pointer_to_nest_stack());
		case TRUCK:
			return(pointer_to_truck_stack());
		case BOMB:
			return(pointer_to_bomb_stack());
		case ROBOT:
			return(pointer_to_robot_stack());
		case PICTURE: // since that is the kind_of
		case SCALE_PICTURE: // maybe I should switch to fine_kind_of?
			return(pointer_to_scale_stack());
		default:
			return(NULL); // and warn??
	};
};

void Toolbox::release_all_references() { // renamed 130504
	if (releasing_all_references) { // new on 230305
		return;
	};
	// could break this up do use release_all_references -- noted on 110504
	//if (preparing_for_deletion) return;
	//preparing_for_deletion = TRUE; // new on 060204
   boolean part_of_puzzle = (tt_system_mode == PUZZLE && !is_toolbox_part_of_puzzle()); 
	// third conjuct added on 071100 to fix leak -- was !about_to_quit() prior to 180103
	// following was conditional on part_of_puzzle prior to 240101 but it leaked the pad, etc.
//       Sprite::prepare_for_deletion(by);
//       return;
	// commented out on 080704 the following since just because Tooly is vacuumed and destroyed no reason to get rid of his tools
	//if (this == tt_toolbox && !part_of_puzzle && tt_reclaiming_memory) { // moved  && tt_reclaiming_memory on 130104 since more logical
	//	if (vacuum != NULL) {
	//		vacuum->remove_from_floor(FALSE,FALSE);
	//	};
	//	if (expander != NULL) {
	//		expander->remove_from_floor(FALSE,FALSE);
	//	};
	//	if (copier != NULL) {
	//		copier->remove_from_floor(FALSE,FALSE);
	//	};
	//	if (notebook != NULL) {
	//		notebook->remove_from_floor(FALSE,FALSE);
	//	};
	//};
	// following segment commented out on 080704
	//UnwindProtect<boolean> set(tt_reincarnate_tools,FALSE); // new on 070704 so the following doesn't reincarnate the tools
	//if (expander != NULL && !expander->inside_vacuum() && expander->pointer_to_background() == NULL) { // background test new on 080704
	//	if (!expander->destroy()) { // removed by on 060204
	//		expander->destroy(); // new on 160103
	//	};
	//};
	//expander = NULL; // new on 130504 -- moved here on 080704 so happens regardless of conditions above
	//if (copier != NULL && !copier->inside_vacuum() && copier->pointer_to_background() == NULL) {
	//	if (!copier->destroy()) { // removed by on 060204
	//		copier->destroy(); // new on 160103
	//	};
	//};
	//copier = NULL; // new on 130504
	//if (notebook != NULL && !notebook->inside_vacuum() && notebook->pointer_to_leader() == NULL && notebook->pointer_to_background() == NULL) {
	//	if (!notebook->destroy()) { // removed by on 060204
	//		notebook->destroy(); // new on 160103
	//	};
	//};
	//notebook = NULL; // new on 130504
	//// needs to be last in case stuff above is inside
	//if (vacuum != NULL && !vacuum->inside_vacuum() && !inside_vacuum() && vacuum->pointer_to_background() == NULL) {
	//	if (!vacuum->destroy()) { // removed by on 060204
	//		vacuum->destroy(); // new on 160103
	//	};
	//};
	//vacuum = NULL; // new on 130504
	// contents of stacks are also followers so should be deleted as such -- stacks itself is part of ToolBox (not a pointer)
	if (followers == NULL) {
		// restore stacks so they are deleted too
		followers = saved_followers;
	};
	Sprite::release_all_references();
	if (this == tt_toolbox) {
		tt_toolbox = NULL; // new on 141299
	};
};

void Toolbox::update_tt_favorites() {
	if (tt_toolbox_vacuum != NULL) {
		tt_toolbox_vacuum->current_favorite(tt_favorites[VACUUM_FAVORITE]);
	};
	if (tt_toolbox_expander != NULL) {
		tt_toolbox_expander->current_favorite(tt_favorites[EXPANDER_FAVORITE]);
	};
	if (tt_toolbox_copier != NULL) {
		tt_toolbox_copier->current_favorite(tt_favorites[COPIER_FAVORITE]);
	};
	if (tt_toolbox_notebook != NULL) {
		tt_toolbox_notebook->current_favorite(tt_favorites[NOTEBOOK_FAVORITE]);
	};
	// rewrote the following as the above on 070704 since don't want to create a notebook if there is none
//   pointer_to_notebook()->current_favorite(tt_favorites[NOTEBOOK_FAVORITE]); // prior to 130100 was commented out with comment: will get done later
   current_favorite(tt_favorites[TOOLBOX_FAVORITE]);
};

Sprite *Toolbox::copy(boolean also_copy_offsets) {
	Toolbox *copy = new Toolbox();
	copy->initialize();
	copy->set_parameter(TOOLBOX_INERT_OPEN);
	copy->set_size_and_location(width,height,llx,lly,COPY_INITIALIZATION);
 //  copy->set_priority_fixed(priority_fixed_flag);
	//if (comment != NULL) {
	//	copy->set_comment(comment->copy());
	//};
   finish_making_copy(copy,also_copy_offsets);
	return(copy);
};

Sprite *Toolbox::compartment_contents(int row, int column) {
	int index = row*number_of_columns+column;
	if (index >= TOOLBOX_STACK_COUNT) {
		return(NULL); // warn?
	};
	return(stacks[index]);
	// rewritten on 250803
	//switch (index) {
	//	case 0:
	//		return(integer_stack);
	//	case 1:
	//		return(text_stack);
	//	case 2:
	//		return(cubby_stack);
	//	case 3:
	//		return(nest_stack);
	//	case 4:
	//		return(scale_stack);
	//	case 5:
	//		return(robot_stack);
	//	case 6:
	//		return(truck_stack);
	//	case 7:
	//		return(bomb_stack);
	//	default:
	//		return(NULL);
	//};
};

void Toolbox::set_priority(long new_priority) {
	if (settled_down() && held_status == NOT_HELD) {
		Sprite::set_priority(max_long-1); // stay on bottom even if say pumped up
		// changed to -1 on 111204 so glued down things are even lower
	} else {
		Sprite::set_priority(new_priority);
	};
};	

void Toolbox::move_to_hand() { 
	if (inside_vacuum()) { 
		say(IDS_TOOLBOX_IN_VACUUM_HELP);
		return;
	};
   if (tt_system_mode != PUZZLE) {
      called_since_last_release = TRUE;
   };
	if (running_to_hand() || floor == NULL) { // called twice so hurry up
		tt_finish_all_animations = TRUE;
//		running_to_hand_flag = FALSE; // commented out since once settled will be set and otherwise tools move too
//		tt_programmer->pointer_to_appearance()->finish_instantly();
//		finish_instantly();
	} else {
		running_to_hand_flag = TRUE;
		enter(FALSE); // for now good enough
	};
};

void Toolbox::enter(boolean move_toolbox_offscreen_first) {
   if (tt_system_mode == PUZZLE && !is_toolbox_part_of_puzzle()) return;
//	postpone_new_logs(); // new on 120100 since looks bad to save during Tooly's entrance
//	vacuum->set_background(floor);
//	copier->set_background(floor);
//	expander->set_background(floor);
//	notebook->recursively_set_background(floor);
//	recursively_set_background(floor);
	offstage_flag = FALSE;
	if (move_toolbox_offscreen_first) {
		// comes in from the left
		move_to(-width,ideal_screen_height/3);
	};
	set_parameter(EAST); // walking east -- not always!!!
	set_priority(0); // floor->next_item_priority());	
	do_after(default_duration(1350),this,TOOLBOX_TURN_SOUTHEAST_CALLBACK);
	city_coordinate goal_x, goal_y;
	if (tt_screen->inside_thought_bubble()) { // new on 011100 that in thought bubble objects are at standard default
		 goal_x = default_favorite_llx;
		 goal_y = default_favorite_lly; 
	} else {
		goal_x = favorite_llx;
		goal_y = favorite_lly;
	};
	animate_location(goal_x,goal_y,
						  default_duration(1500),this,TOOLBOX_TURN_VERTICAL_CALLBACK);
};

const millisecond toolbox_opening_time = 500;

void Toolbox::turn_vertical() {
	millisecond duration = default_duration(toolbox_opening_time);
	if (duration == 0) { // new on 190999
		set_parameter(TOOLBOX_INERT_OPEN);
	} else {
		set_parameter(TOOLBOX_MORPHING_TO_INERT);
	};
	if (floor != NULL) set_priority(floor->next_item_priority());
	// do after opening have tools leave
	do_after(duration,this,TOOLBOX_OPENED_CALLBACK);
};

const	millisecond time_to_leave = 1500;
const	millisecond time_to_grow = 1000;

void Toolbox::become_inert() { // new on 041102 for XML load
   set_parameter(TOOLBOX_INERT_OPEN);
};

void Toolbox::bring_out_vacuum() {
	// have it run out later
	if (tt_toolbox_vacuum == NULL || this != tt_toolbox) return; // new on 070704
	if (tt_toolbox_vacuum->running_to_hand() || tt_toolbox_vacuum->held()) { // new on 221105 in case F2 pushed early on
		return;
	};
	city_coordinate center_x = llx+width/3,
						 center_y = lly+height/2;
//	vacuum->save_size();
	tt_toolbox_vacuum->set_size_and_location(1,1,center_x,center_y); // 1,1 was 0,0 prior to 130200
	tt_toolbox_vacuum->update_display(); // fixes starting full size bug
	if (floor != NULL) {
		if (tt_toolbox_vacuum->pointer_to_background() == NULL) {
			// condition added on 111104 since may have been called by a function key before this runs
			floor->add_item(tt_toolbox_vacuum,TRUE,TRUE,FALSE); // don't warn
		};
		if (floor->inside_rule_body()) {
			tt_toolbox_vacuum->save_contents();
			tt_toolbox_vacuum->save_state();
			tt_toolbox_copier->save_state();
			tt_toolbox_expander->save_state();
		};
	};
//#if TT_BETA
//   if (tt_running_old_demos) {
//   	vacuum->animate_to_good_size(time_to_grow);
//	   vacuum->walk_to_location(tile_width,tile_height/2);
//      return;
//   };
//#endif
	tt_toolbox_vacuum->animate_to_favorite_size(default_duration(time_to_grow));
	tt_toolbox_vacuum->walk_to_favorite_location();
	tt_toolbox_vacuum->set_selectable(TRUE); // new on 220505 since Leonel had a city where Dusty wasn't selectable (issue 671)
//	vacuum->animate_location(tile_width,tile_height/2,time_to_leave);
};

void Toolbox::bring_out_copier() {
	if (tt_toolbox_copier == NULL || this != tt_toolbox) return; // new on 070704
	if (tt_toolbox_copier->running_to_hand() || tt_toolbox_copier->held()) { // new on 221105 in case F5 pushed early on
		return;
	};
	city_coordinate center_x = llx+width/3,
						 center_y = lly+height/2;
//	copier->save_size();
	tt_toolbox_copier->set_size_and_location(1,1,center_x,center_y);
	tt_toolbox_copier->update_display();
	if (floor != NULL && tt_toolbox_copier->pointer_to_background() == NULL) {
		// second condition added on 111104 since may have been called by a function key before this runs
		floor->add_item(tt_toolbox_copier,TRUE,FALSE,FALSE); 
	};
	// removed default_duration() below and put animation on tt_global_picture on 021199 
	// so happens at least a frame later so geometry of button is always OK
	tt_global_picture->do_after(time_to_leave,tt_toolbox_copier,SINK_DOWN_CALLBACK);
//#if TT_BETA
//   if (tt_running_old_demos) {
//   	copier->animate_to_good_size(time_to_grow);
//	   copier->animate_location(tile_width/4,tile_height*5,time_to_leave);
//      return;
//   };
//#endif
	tt_toolbox_copier->animate_to_favorite_size(default_duration(time_to_grow));
	tt_toolbox_copier->travel_to_favorite_location(default_duration(time_to_leave));
	tt_toolbox_copier->set_selectable(TRUE); // new on 220505 just in case since Leonel had a city where Dusty wasn't selectable (issue 671)
};

void Toolbox::bring_out_expander() {
	if (tt_toolbox_expander == NULL || this != tt_toolbox) return; // new on 070704
	if (tt_toolbox_expander->running_to_hand() || tt_toolbox_expander->held()) { // new on 221105 in case F3 pushed early on
		return;
	};
	city_coordinate center_x = llx+width/3,
						 center_y = lly+height/2;
//	expander->save_size();
	tt_toolbox_expander->set_size_and_location(tile_width/2,tile_height,center_x,center_y);
	tt_toolbox_expander->update_display();
	if (floor != NULL && tt_toolbox_expander->pointer_to_background() == NULL) {
		// second condition added on 111104 since may have been called by a function key before this runs
		floor->add_item(tt_toolbox_expander,TRUE,FALSE,FALSE);
	};
//#if TT_BETA
//   if (tt_running_old_demos) {
//	   expander->animate_to_good_size(time_to_grow);
//	   expander->jump_to(9*tile_width,tile_height/2);
//      return;
//   };
//#endif
	tt_toolbox_expander->animate_to_favorite_size(default_duration(time_to_grow));
	tt_toolbox_expander->jump_to_favorite_location();
	tt_toolbox_expander->set_selectable(TRUE); 
	// new on 220505 just in case since Leonel had a city where Dusty wasn't selectable (issue 671)
};

void Toolbox::bring_out_notebook() {
	if (tt_toolbox_notebook == NULL || this != tt_toolbox) return; // new on 080704
	city_coordinate center_x = llx+width/3,
					    center_y = lly+height/2;
   city_coordinate old_width = tt_toolbox_notebook->current_width(); // this will create a notebook if needed -OK? asked 070704 -- updated 150704 to use tt_toolbox_notebook
   city_coordinate old_height = tt_toolbox_notebook->current_height();
//#if TT_BETA
//   if (tt_running_old_demos) {
//	   if (old_width < tile_width || old_height < tile_height) {
//		   // seems that if I get up and sit too fast can get stuck as 1x1
//         notebook->good_size(old_width,old_height);
//         long percent_x, percent_y;
//         current_percent_of_good_size(percent_x,percent_y);
//		   old_width = 7*tile_width;
//		   old_height = notebook->height_given_width(old_width);
//	   };
//	   notebook->set_size_and_location(old_width/10,old_height/10,center_x,center_y);
//	   notebook->update_display();
//      floor->add_item(notebook);
//      notebook->fly_to(13*tile_width,tile_height/2,time_to_leave);
//      notebook->animate_size(old_width,old_height,time_to_grow);
//      return;
//   };
//#endif
   tt_toolbox_notebook->set_size_and_location(old_width/3,old_height/3,center_x,center_y);
   tt_toolbox_notebook->update_display();
	if (floor != NULL) floor->add_item(tt_toolbox_notebook,TRUE,TRUE,FALSE); // don't warn if already there
   tt_toolbox_notebook->animate_to_favorite_size(default_duration(time_to_leave));
   tt_toolbox_notebook->fly_to_favorite_location(default_duration(time_to_leave));
	Sprite *label = tt_toolbox_notebook->pointer_to_label(); // following moved here on 190100
	if (label != NULL && label->kind_of() == TEXT) { // conditional added 201099
		label->set_visible(FALSE); // added on 170999 so invisible until displayed which will fix its geometry
	};
	tt_toolbox_notebook->set_selectable(TRUE); // new on 220505 just in case since Leonel had a city where Dusty wasn't selectable (issue 671)
};

const int delay = 500; // was 6000
const int random_delay = 1000;  // was 4000

void Toolbox::opened() {
	// guessing that 8000 milliseconds is enough time for the tools to leave and settle down
	//if (tt_logging) {
	//	tt_global_picture->do_after(8000,NULL,STOP_POSTPONING_NEW_LOGS_CALLBACK); // new on 120100
	//};
	if (inside_vacuum()) return;
   if (floor == NULL) { // warn??
//      set_background(tt_programmer->pointer_to_appearance()->pointer_to_background());
		Background *new_floor = tt_programmer->pointer_to_appearance()->pointer_to_background(); // rewritten 010505
		if (new_floor != NULL) {
			new_floor->add_item(this);
		};
   };
	city_coordinate compartment_width, compartment_height,
						 compartment_llx, compartment_lly;
	millisecond max_animation_duration = 0; // new on 161105
	for (int column = 0; column < number_of_columns; column++) {
		for (int row = 0; row < number_of_rows; row++) {
			Sprite *contents = compartment_contents(row,column);
			contents->set_background(floor,FALSE,FALSE); // args added 010500 to avoid spurious warnings
			compartment_size_and_location(row,column,
													compartment_width,compartment_height,
													compartment_llx,compartment_lly);
			// to ease strain on cache could have these appear at different
			// times not just start small
			// or does a small size mean no use of cache???
         if (tt_city->stopped() || tt_finish_all_animations) {
            contents->set_size_and_location(compartment_width,compartment_height,
													     compartment_llx,compartment_lly);
         } else {
			   contents->set_size_and_location(1,1, // was 0,0 prior to 130200
													     compartment_llx+compartment_width/2,
													     compartment_lly+compartment_height/2);
				millisecond duration = default_duration(delay+my_random(random_delay));
				if (duration > max_animation_duration) max_animation_duration = duration;
			   contents->animate_size_and_location(compartment_width,
															   compartment_height,
															   compartment_llx,compartment_lly,
															   duration);
         };
		};
	};
	do_after(max_animation_duration+1000,this,CUBBY_REFIT_CONTENTS_CALLBACK); 
	// new on 161105 since if scrolled in the meanwhile looks bad
	if (tt_toolbox_vacuum != NULL && !tt_toolbox_vacuum->inside_vacuum() && !tt_toolbox_vacuum->held() && !running_to_hand()) {
		do_after(default_duration(my_random(2000)),this,BRING_OUT_VACUUM_CALLBACK);
	};
	if (tt_toolbox_copier != NULL && !tt_toolbox_copier->inside_vacuum() && !tt_toolbox_copier->held() && !running_to_hand()) {
		do_after(default_duration(my_random(1000)),this,BRING_OUT_COPIER_CALLBACK);
	};
	if (tt_toolbox_expander != NULL && !tt_toolbox_expander->inside_vacuum() && !tt_toolbox_expander->held() 
		 && !running_to_hand()) {
		do_after(default_duration(my_random(1500)),this,BRING_OUT_EXPANDER_CALLBACK);
	};
	if (tt_toolbox_notebook != NULL && !tt_toolbox_notebook->inside_vacuum() && !tt_toolbox_notebook->held() && 
		 tt_toolbox_notebook->pointer_to_leader() == NULL && !running_to_hand()) {
		// prior to 070704 was !pointer_to_notebook()->inside_vacuum() but that created a new one on demand
		do_after(default_duration(my_random(500)),this,BRING_OUT_NOTEBOOK_CALLBACK);
	};
	check_tt_file_name_to_clipboard();
	set_priority(max_long-1); // new on 111204
	set_selectable(TRUE); // new on 220505 just in case
//	floor->add_tools(vacuum,copier,expander,notebook);
//	Sprites *remaining = followers;
//	while (remaining != NULL) {
//		remaining->first()->set_background(floor);
//		remaining = remaining->rest();
//	};
};

//void Toolbox::add_sprite_from_file_name(string file_name) {
//	Sprite *sprite = sprite_from_file_name(tt_file_name_to_clipboard);
void Toolbox::add_sprite(Sprite *sprite) { // rewritten 220303
	if (sprite != NULL) {
		play_sound(MAGIC_SOUND);
		if (floor == NULL) { // new on 140405 since can happen when clicking on a TT file
			floor = tt_programmer->pointer_to_background();
		};
		if (floor == NULL) {
			return; // warn??
		};
		floor->add_item(sprite);
//		sprite->set_size_and_location(500,500,center_x(),center_y());
		sprite->move_to((ideal_screen_width-sprite->current_width())/2,ideal_screen_height+sprite->current_height()); 
		// rewritten 081003 since otherwise size is wrong when double clicking a tt file
		const millisecond duration = 2000;
//		sprite->animate_to_good_size(duration); // commented out on 081003
		sprite->animate_location(ideal_screen_width/3,ideal_screen_height/2,duration); // left of center of screen
//		sprite->activate(); // new on 270300 -- commented out on 230304 since the following works for pictures as well as pictures in boxes
		sprite->do_after(duration,sprite,NOW_ON_FLOOR_CALLBACK); // new on 301003 so things in boxes really run correctly
#if TT_DEBUG_ON
		if (tt_debug_mode == 230304) {
			tt_error_file() << "Tooly is adding a sprite at " << tt_current_time << " and frame is " << tt_frame_number << endl;
		};
#endif
//		do_after(duration,this,SCREEN_DIRTY_CALLBACK); // ultimatedly unneeded experiment on 250105
	};
};	

void Toolbox::check_tt_file_name_to_clipboard() { // abstracted on 030903
	if (tt_file_name_to_clipboard != NULL) { // moved here on 100803
		boolean aborted = FALSE; // new on 200105
		string file_name_to_clipboard = tt_file_name_to_clipboard; // new on 250105
		tt_file_name_to_clipboard = NULL; // since this can be called recursively via loading_wait that displays the screen
		// but not if just display_subtitles rather than the whole screen
		UnwindProtect<boolean> set(tt_loading_is_ok_to_abort,TRUE); // new on 250105
		tt_add_sprite_when_on_floor = sprite_from_file_name(file_name_to_clipboard,aborted);
		delete [] tt_file_name_to_clipboard;
		tt_file_name_to_clipboard = NULL;
	};
	if (tt_add_sprite_when_on_floor != NULL) { // new on 250300
		add_sprite(tt_add_sprite_when_on_floor);
		tt_add_sprite_when_on_floor = NULL;
		tt_screen->remove_current_subtitle(); // new on 250105
//		floor->set_dirty(); // new on 250105
		//tt_screen->display(FALSE); // new on 250105
		//reset_grid(); // new on 250105
	};
};

void Toolbox::compartment_size_and_location(int row, int column,
	                                         city_coordinate &compartment_width, city_coordinate &compartment_height,
	                                         city_coordinate &compartment_llx, city_coordinate &compartment_lly) {
	compartment_width = width/8;
	compartment_height = height/10;
	compartment_llx = llx+compartment_width+(3*column*compartment_width)/2;
	compartment_lly = lly+compartment_height*5-(6*row*compartment_height)/5;
};

const millisecond time_to_exit_stage = 1000;

millisecond Toolbox::leave(boolean leave_floor_too, Sprite *by) {
   if (tt_system_mode == PUZZLE && !is_toolbox_part_of_puzzle()) {
      return(500); // isn't even in there
   };
	leave_floor_when_offstage = (flag) leave_floor_too;
	return(go_to(ideal_screen_width,ideal_screen_height/3,100,by));
};
	
millisecond Toolbox::go_to(city_coordinate new_goal_x, 
									city_coordinate new_goal_y,
									int new_percentage_size_goal,
									Sprite *by) {
	if (inside_vacuum() || floor == NULL) return(0); // just stay there
   if (tt_system_mode == PUZZLE && !is_toolbox_part_of_puzzle()) return(0);
	Sprite::set_priority(floor->next_item_priority());
//	pointer_to_notebook()->set_priority(floor->next_item_priority());
	set_parameter(TOOLBOX_MORPHING_TO_LIFE); // moved here on 080704 since shouldn't be in the conditional below
	millisecond morph_time = total_cycle_duration();
	millisecond close_time = morph_time-toolbox_opening_time;
	if (this == tt_toolbox) { // condition new on 080704 -- might not be necessary since won't be called if a copy of Tooly
		if (tt_toolbox_notebook != NULL) { // rewritten on 070704
			tt_toolbox_notebook->set_priority(floor->next_item_priority());
		};
		if (tt_toolbox_expander != NULL) { // here and many other places added NULL tests for tools on 070704
			tt_toolbox_expander->set_priority(floor->next_item_priority());
		};
		if (tt_toolbox_vacuum != NULL) {
			tt_toolbox_vacuum->set_priority(floor->next_item_priority());
		};
		if (tt_toolbox_copier != NULL) {
			tt_toolbox_copier->set_priority(floor->next_item_priority());
		};
		goal_x = new_goal_x;
		goal_y = new_goal_y;
		percentage_size_goal = new_percentage_size_goal;
		city_coordinate center_x = llx+width/3,
							center_y = lly+height/2;
		if (tt_toolbox_vacuum != NULL && !tt_toolbox_vacuum->inside_vacuum() && !tt_toolbox_vacuum->held()) { // may have been copied
			tt_toolbox_vacuum->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
//			vacuum->save_size();
			tt_toolbox_vacuum->animate_size(0L,0L,close_time*3);
			tt_toolbox_vacuum->walk_to_location(center_x,center_y,close_time);
			if (floor->inside_rule_body()) {
				tt_toolbox_vacuum->restore_contents(by);
				tt_toolbox_vacuum->restore_state();
			};
		};
		if (tt_toolbox_expander != NULL && !tt_toolbox_expander->inside_vacuum() && !tt_toolbox_expander->held()) {
			tt_toolbox_expander->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
//			expander->save_size();
  //		expander->animate_size(0,0,close_time);
			tt_toolbox_expander->jump_to(center_x,center_y);
			if (floor->inside_rule_body()) {
				tt_toolbox_expander->restore_state();
			};
		};
		if (tt_toolbox_copier != NULL && !tt_toolbox_copier->inside_vacuum() && !tt_toolbox_copier->held()) {
			tt_toolbox_copier->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
//			copier->save_size();
			tt_toolbox_copier->animate_size_and_location(0,0,center_x,center_y,default_duration(close_time));
			if (floor->inside_rule_body()) {
				tt_toolbox_copier->restore_state();
			};
		};
		// was !pointer_to_notebook()->inside_vacuum() prior to 070704
		if (tt_toolbox_notebook != NULL && !tt_toolbox_notebook->inside_vacuum() && !tt_toolbox_notebook->held() && tt_toolbox_notebook->pointer_to_leader() == NULL) {
			tt_toolbox_notebook->go_to_a_blank_page();
		// following screws up leaders if going to thought bubble
//			notebook->go_to_page(1);
//			notebook->save_size();
//			notebook->animate_size_and_location(0,0,center_x,center_y,close_time);
			tt_toolbox_notebook->animate_size(0L,0L,close_time);
			tt_toolbox_notebook->fly_to(center_x,center_y,default_duration(close_time),FALSE);
		};
		do_after(default_duration(close_time),this,REMOVE_TOOLS_CALLBACK);
	};
	do_after(default_duration(morph_time),this,TOOLBOX_EXIT_STAGE_RIGHT_CALLBACK);
	return(morph_time+time_to_exit_stage);
};

void Toolbox::remove_tools() {
//	floor->remove_tools();
	if (tt_toolbox_vacuum != NULL && !tt_toolbox_vacuum->inside_vacuum() && !tt_toolbox_vacuum->held()) {
		tt_toolbox_vacuum->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off  // just in case
		tt_toolbox_vacuum->remove_from_floor();
	};
	if (tt_toolbox_copier != NULL && !tt_toolbox_copier->inside_vacuum() && !tt_toolbox_copier->held()) {
		tt_toolbox_copier->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
		tt_toolbox_copier->remove_from_floor();
	};
	if (tt_toolbox_expander != NULL && !tt_toolbox_expander->inside_vacuum() && !tt_toolbox_expander->held()) {
		tt_toolbox_expander->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
		tt_toolbox_expander->remove_from_floor(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
	};
	if (tt_toolbox_notebook != NULL && !tt_toolbox_notebook->inside_vacuum() && !tt_toolbox_notebook->held()) {   
		//  && notebook->pointer_to_leader() == NULL
		tt_toolbox_notebook->finish_instantly();
		tt_toolbox_notebook->remove_from_floor();
	};
};

void Toolbox::exit_stage_right() {
	set_parameter(EAST); // headed east again
	animate_size_and_location((percentage_size_goal*width)/100,
									  (percentage_size_goal*height)/100,
									  goal_x,goal_y,
									  default_duration(time_to_exit_stage),
                             this,TOOLBOX_OFFSTAGE_CALLBACK);
};

void Toolbox::offstage() {
//	recursively_set_background(NULL);
	if (leave_floor_when_offstage) {
		remove_from_floor(FALSE); // was commented out but is necessary (e.g. stacks in Toolbox can get a destroyed floor)
	};
	offstage_flag = TRUE;
};

Sprite *Toolbox::select(TTRegion *grasp_region, SelectionReason reason, Sprite *in_hand) {
	if (reason == MIGHT_USE_EXPANDER) {
		return(this);
	};
	if (reason == MIGHT_DROP_ITEM) {
		switch (in_hand->kind_of()) {
			case INTEGER:
			case TEXT:
         case SOUND:
			case CUBBY:
			case NEST:
			case BIRD:
			case PICTURE:
			case ROBOT:
			case TRUCK:
			case BOMB:
				break;
			default:
			  return(NULL);
		};
	};
	if (!settled_down())	{
		return(NULL);
	};
	// if in compartment region pass selection along to part
	TTRegion compartment_region;
//	full_region(compartment_region);
	// modify copy
//	compartment_region.min_x += true_width()/10;
//	compartment_region.min_y += true_height()/6;
//	compartment_region.max_x -= (3*true_width())/5; // take away part for lid
//	compartment_region.max_y -= (2*true_height())/5; // take away part for lid
   // rewritten on 180402 to be more accurate (needed for absolute mouse mode)
   Sprite *upper_left = compartment_contents(0,0);
   city_coordinate upper_left_llx = upper_left->current_llx();
   city_coordinate upper_left_ury = upper_left->current_ury();
   city_coordinate inner_width = upper_left->current_width(); // just element in compartment - not counting comparment walls
   city_coordinate inner_height = upper_left->current_height();
   Sprite *lower_right = compartment_contents(number_of_rows-1,number_of_columns-1);
   city_coordinate lower_right_urx = lower_right->current_urx();
   city_coordinate lower_right_lly = lower_right->current_lly();
   // following assumes there is more than one column and row
   city_coordinate compartment_wall_width = ((lower_right_urx-upper_left_llx)-number_of_columns*inner_width)/(number_of_columns-1);
   city_coordinate compartment_wall_height = ((upper_left_ury-lower_right_lly)-number_of_rows*inner_height)/(number_of_rows-1);
   compartment_region.min_x = upper_left_llx-compartment_wall_width;
   compartment_region.max_x = lower_right_urx+compartment_wall_width;
   compartment_region.min_y = lower_right_lly-compartment_wall_height;
   compartment_region.max_y = upper_left_ury+compartment_wall_height;
#if TT_DEBUG_ON
	if (tt_debug_mode == 1111) {
		tt_error_file() << compartment_region.min_x << "," 
						<< compartment_region.min_y << " " 
						<< compartment_region.max_x << "," 
						<< compartment_region.max_y << "  Compartment " << endl;
		tt_error_file() << grasp_region->min_x << "," 
						<< grasp_region->min_y << " " 
						<< grasp_region->max_x << "," 
						<< grasp_region->max_y << "  Grasp " << endl;						
	};
#endif
	city_coordinate grasp_x, grasp_y;
	grasp_region->region_center(grasp_x,grasp_y);
	if (compartment_region.overlaps(grasp_region)) {
		city_coordinate compartment_width = compartment_region.max_x-compartment_region.min_x;
		city_coordinate compartment_height = compartment_region.max_y-compartment_region.min_y;
		int column, row; 
		if (grasp_x < compartment_region.min_x) {
			column = 0;
		} else if (grasp_x >= compartment_region.max_x) {
			column = number_of_columns-1;
		} else {
			column = (number_of_columns*(grasp_x-compartment_region.min_x))/compartment_width;
		};
		if (grasp_y < compartment_region.min_y) {
			row = number_of_rows-1;
		} else if (grasp_y >= compartment_region.max_y) {
			row = 0;
		} else {
			row = (number_of_rows*(compartment_region.max_y-grasp_y))/compartment_height;
		};
		last_contents_selection = compartment_contents(row,column);
#if TT_DEBUG_ON
		if (tt_debug_mode == 1111) {
			tt_error_file() << column << "," << row << endl;
		};
#endif	
		if (last_contents_selection == NULL) { // not full just now
			return(NULL);
		} else if (last_contents_selection->infinite_stack()) { // new on 150704 to return trucks, nests, and bombs to Tooly
			return(last_contents_selection);
		} else {
			return(last_contents_selection->select(grasp_region,reason,in_hand));
		};
	} else if (grasp_x <= compartment_region.max_x) {
		last_contents_selection = NULL;
		return(NULL);
//	} else if (reason == MIGHT_USE_COPIER || reason == MIGHT_USE_VACUUM) {
		// think about this policy...
//		return(NULL);
	} else {
      last_contents_selection = NULL;
		return(this);
	};
};	

ReleaseStatus Toolbox::item_released_on_top(Sprite *item, Sprite *by,
														  SpritePointer &recipient, int level) {
// too often the following lead to the wrong behavior
//	if (last_contents_selection != NULL) {
//		recipient = last_contents_selection;
//		if (recipient->infinite_stack() && equal(item,recipient)) {
//			completed(action_status);
//			return(RELEASED_ON_ITS_STACK);
//		} else {
//			return(recipient->item_released_on_top(item,by,recipient,level+1,action_status));
//		};
//	};
	TTRegion item_region;
	item->collision_region(item_region);
	for (int column = 0; column < number_of_columns; column++) {
		for (int row = 0; row < number_of_rows; row++) {
			Sprite *contents = compartment_contents(row,column);
			TTRegion contents_region;
			contents->collision_region(contents_region);
			if (contents_region.overlaps(item_region)) {
				recipient = contents;
				if (equal(contents,item) ||
                (contents->fine_kind_of() == SCALE_PICTURE &&
                 item->fine_kind_of() == SCALE_PICTURE)) {
					by->completed();
					return(RELEASED_ON_ITS_STACK);
				};
				ReleaseStatus status = contents->item_released_on_top(item,by,recipient,level+1);
				if (status != RELEASE_IGNORED) {
					return(status);
				};
			};
		};
	};
   by->abort_action_if_robot(IDS_TOOLBOX_DIDNT_ACCEPT_SOMETHING);
	return(RELEASE_IGNORED);
};

Sprite *Toolbox::selected(SelectionReason reason, Sprite *by) {
   refit_contents();
   return(Sprite::selected(reason,by));
};

Sprite *Toolbox::released(Sprite *by, boolean top_level, SpecialDrop special_drop) {
	Sprite *result = Sprite::released(by,top_level,special_drop);
	set_priority(max_long-1); // always on the bottom -- except glued and infinite stacks as of 121204
   if (!by->still_running()) set_favorite_size_and_location();
   refit_contents();
	return(result);
};

void Toolbox::favorite_size(city_coordinate &width, city_coordinate &height) {
   if (favorite_width <= 0) {
      good_size(width,height);
   } else {
      width = favorite_width;
      height = favorite_height;
   };
};

void Toolbox::set_favorite(FavoriteSizeAndLocation &f) {
	if (f.favorite_width < 10 || f.favorite_width > 50000 || // changed 0 to 10 on 220300
		 f.favorite_height < 10 || f.favorite_height > 50000) return; // new on 030300 to recover gracefully from goofy sizes
   favorite_width = f.favorite_width;
   favorite_height = f.favorite_height;
   favorite_llx = f.favorite_llx;
   favorite_lly = f.favorite_lly;
};

void Toolbox::set_favorite_size_and_location() {
	if (floor == NULL || floor->inside_thought_bubble()) return; // new on 070103 to ignore changes while in thought bubble
   favorite_width = width;
   favorite_height = height;
   if (llx+true_width() < tile_width/4) { // off the left side
      favorite_llx = tile_width/4-true_width(); // a bit on on screen
   } else if (llx >= ideal_screen_width-true_width()/2) { // can only grab him by the right side
      favorite_llx = ideal_screen_width-true_width()/2;
   } else {
      favorite_llx = llx;
   };
   if (lly+true_height() < 1) { // off the bottom
      favorite_lly = tile_height-true_height();
   } else if (lly >= ideal_screen_height-tile_height/4) {
      favorite_lly = ideal_screen_height-tile_height/4;
   } else {
      favorite_lly = lly;
   };
//   animate_location(favorite_llx,favorite_lly,default_duration(500)); // commented out on 110100
};

void Toolbox::set_clean_status(CleanStatus new_status, boolean recur) {
	// new on 260500
	if (new_status != CLEAN && background_cached && !tt_resetting_or_destroying_city
		 && new_status != FOLLOWERS_DIRTY // new on 071103 since the followers are just selected so Tooly is OK - 
		 // this was wasteful and lead to an empty Toolbox sometimes when going to time travel: issue 357
		) {
		// tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 but can cause memory errors while resetting during time travel
		set_background_cached(FALSE);
	};
	Sprite::set_clean_status(new_status,recur);
};

void Toolbox::display(SelectionFeedback selection_feedback,
							 boolean followers_too, TTRegion *region) {
	if (tt_toolbox->gone()) return; // new on 061100
	if (this != tt_toolbox) { // a copy
		Sprite::display(selection_feedback,followers_too,region);
		return;
	};
//	if (!completely_clean()) {
//		set_background_cached(FALSE); // new on 260500 since followers can change appearance now
//	};
	if (background_cached) { // part of floor now
		return;
	};
//	int index = resource_index();
//	if (background_cached && index != TOOL_BOX_MORPH16_IMAGE) {
		// only cache last image
//		restore_background();
//		reset_grid();
//		background_cached = FALSE;
//		cached_width = 0;
//	};
//	int index = resource_index();
	if (settled_down()) { 
//		tt_toolbox->check_tt_file_name_to_clipboard(); // new on 050903
		if (frame_to_check_tt_file_name_to_clipboard < 0) { // rewritten on 101203 since this can have the clipboard object appear too soon if time travel is on and click through titles
			// also this is lighter weight
			frame_to_check_tt_file_name_to_clipboard = tt_frame_number+1;
		} else if (frame_to_check_tt_file_name_to_clipboard == tt_frame_number) {
			check_tt_file_name_to_clipboard();
		};
		running_to_hand_flag = FALSE;
//	if (index > TOOL_BOX_MORPH12_IMAGE && index <= TOOL_BOX_MORPH16_IMAGE) {
		// if open show followers as well
		// unless cached
//		if (index == TOOL_BOX_MORPH16_IMAGE) { // last image
//			if (!(cached_llx == llx && cached_lly == lly && // but not cached
//					cached_width == width && cached_height == height)) {
//				if (background_cached) {
//					restore_background();
//					reset_grid();
//				};
//				Sprite::display(screen);
//				cache_background();
//				reset_grid();
//				cached_llx = llx;
//				cached_lly = lly;
//				cached_width = width;
//				cached_height = height;
//				background_cached = TRUE;
//			} else {
//				reset_grid(); // else do nothing -- thats why there is a cache
//			};
//		} else { // not last image
		Sprite::display(selection_feedback,followers_too,region);
//		};
	} else { // otherwise just the image
		display_without_followers(selection_feedback,region);
	};
};

ToolboxCacheStatus Toolbox::cache_status() {
#if TT_WANT_IMAGE_STATISTICS
	return(TOOLBOX_CACHE_INVALID);
#else
	if (tt_time_travel != TIME_TRAVEL_OFF && tt_time_travel != TIME_TRAVEL_ON) { // paused is some way -- new on 041103
//		background_cached = FALSE;
		set_background_cached(FALSE); // updated on 050704 to be sure this really does all the right things
		return(TOOLBOX_CACHE_INVALID); // otherwise some of its contents aren't displayed
	};
	if (background_cached) {
		if (settled_down() &&
//			 cached_llx == llx && cached_lly == lly &&
			 cached_width == width && cached_height == height &&
			 held_status == NOT_HELD) {
			return(TOOLBOX_CACHE_VALID);
		} else {
			return(TOOLBOX_CACHE_INVALID);
		};
	} else if (!tt_screen->first_layer_is(this) && tt_log_version_number >= 63) { // log version check new on 191204
		return(TOOLBOX_NOT_READY_TO_CACHE); // new on 111204 since other things are underneath
	} else if (settled_down() && held_status == NOT_HELD && clean() && !inside_vacuum() && this != tt_selection) {
		// if selected then is wiggling so don't cache -- new on 050704
		set_priority(max_long-1); // sink to bottom
		for (int column = 0; column < number_of_columns; column++) {
		  for (int row = 0; row < number_of_rows; row++) {
			  Sprite *contents = compartment_contents(row,column);
			  contents->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
			  contents->update_display();
		  };
		};
      refit_contents(); // just to be sure
		display_cost = 100; // lie about the cost to get it cached as part of image
		return(TOOLBOX_READY_TO_CACHE);
	} else {
		return(TOOLBOX_NOT_READY_TO_CACHE);
	};
#endif
};

void Toolbox::shifted(city_coordinate delta_x, city_coordinate delta_y) {
   if (followers == NULL) { // has settled down and followers have been optimized away
      for (int column = 0; column < number_of_columns; column++) {
         for (int row = 0; row < number_of_rows; row++) {
			   Sprite *contents = compartment_contents(row,column);
			   contents->move_by(delta_x,delta_y);
         };
      };
      if (background_cached) {
         just_set_clean_status(CLEAN); // lie to get it to re-cache
         background_cached = FALSE;
         cached_width = 0; // good enough impossible value
      };
      restore_followers();
   };
};

void Toolbox::set_background_cached(boolean flag) {
	background_cached = flag;
	if (background_cached) {
//		cached_llx = llx;
//		cached_lly = lly;
		cached_width = width;
		cached_height = height;
		if (saved_followers == NULL) { // new on 061003 since with time travel can get confused here
			saved_followers = followers;
			followers->set_background(NULL); // new on 060204
		};
		followers = NULL;
	} else {
		cached_width = 0; // good enough impossible value
		restore_followers();
		propagate_changes();
	};
// don't need to since these are followers
//	for (int i = 0; i < number_of_rows; i++) {
//		for (int j = 0; j < number_of_columns; j++) {
//			compartment_contents(i,j)->set_cached_in_background(flag);
//		};
//	};
};

void Toolbox::restore_followers() {
	if (followers != NULL && saved_followers == NULL) return; // new on 041103
	followers = saved_followers;
	Sprites *remaining = saved_followers;
	while (remaining != NULL) {
		remaining->first()->set_background(floor,FALSE,FALSE);
		remaining = remaining->rest();
	};
	saved_followers = NULL;
};

void Toolbox::set_inside_vacuum(boolean new_flag, boolean recur) {
	if (followers == NULL && recur) {
		followers = saved_followers;
		Sprite::set_inside_vacuum(new_flag,recur);
//		followers = NULL; // commented out on 070704 since they still are followers and this can remain as a leader in the floor's list otherwise
	} else {
		Sprite::set_inside_vacuum(new_flag,recur);
	};
};

boolean Toolbox::save_region() {
	if (!background_cached) {
		return(Sprite::save_region());
	}; // otherwise do nothing
	return(TRUE);
};

void Toolbox::add_self_and_friends(Background *background) { // new on 110100
	set_parameter(TOOLBOX_INERT_OPEN); // new on 120100
	if (floor == NULL && !inside_vacuum()) { // second condition added 030300
		background->add_item(this,FALSE);
	};
	if (tt_toolbox_vacuum != NULL && tt_toolbox_vacuum->pointer_to_background() == NULL && !tt_toolbox_vacuum->inside_vacuum()) { // last condition added 030300
		background->add_item(tt_toolbox_vacuum,FALSE);
	};
	if (tt_toolbox_copier != NULL && tt_toolbox_copier->pointer_to_background() == NULL && !tt_toolbox_copier->inside_vacuum()) { // last condition added 030300
		background->add_item(tt_toolbox_copier,FALSE);
	};
	if (tt_toolbox_expander != NULL && tt_toolbox_expander->pointer_to_background() == NULL && !tt_toolbox_expander->inside_vacuum()) { // second condition added 030300
		background->add_item(tt_toolbox_expander,FALSE);
	};
	if (tt_toolbox_notebook != NULL && tt_toolbox_notebook->pointer_to_background() == NULL && !tt_toolbox_notebook->inside_vacuum()) { // last condition added 030300 
		// new on 140100
		background->add_item(tt_toolbox_notebook,FALSE);
	};
	if (tt_add_sprite_when_on_floor != NULL) { // new on 250300
	   tt_toolbox->add_sprite(tt_add_sprite_when_on_floor);
		tt_add_sprite_when_on_floor = NULL;
	};
/*
	} else { // new on 140100
		if (vacuum != NULL) {
			vacuum->update_button_size_and_location();
		};
		if (copier != NULL) {
			copier->update_button_size_and_location();
		};
		if (expander != NULL) {
			expander->update_button_size_and_location();
		};
//		if (notebook != NULL) {
//			notebook->propagate_changes();
//		};
*/
};

#if TT_XML

void Toolbox::xml(xml_element *toolbox_element, xml_document *document) {
//   xml_element *toolbox_element = create_xml_element(xml_tag(),document); // replaced L"Toolbox" with xml_tag() on 090703
	if (offstage_flag) { // new on 060504
		xml_set_attribute(toolbox_element,L"Offstage",1);
	};
	if (gone_for_good_flag) { // new on 120704
		xml_set_attribute(toolbox_element,L"GoneForGood",1);
	};
	if (tt_toolbox == this) { // new on 170404 to record state of tools -- tt_toolbox == this condition new on 080704
		// removed the condition floor == NULL on 211004 since a tool can have state (e.g. saved_state) and not yet on floor
		// (e.g. while entering and leaving thought bubble)
		if (tt_toolbox_vacuum != NULL && tt_toolbox_vacuum->pointer_to_background() == NULL) {
			tt_toolbox_vacuum->xml_create_and_append_element(toolbox_element,document)->Release();
//			xml_append_child(vacuum->xml(toolbox_element),document);
		};
		if (tt_toolbox_expander != NULL && tt_toolbox_expander->pointer_to_background() == NULL) {
			tt_toolbox_expander->xml_create_and_append_element(toolbox_element,document)->Release();
//			xml_append_child(expander->xml(toolbox_element),document);
		};
		if (tt_toolbox_copier != NULL && tt_toolbox_copier->pointer_to_background() == NULL) {
			tt_toolbox_copier->xml_create_and_append_element(toolbox_element,document)->Release();
//			xml_append_child(copier->xml(toolbox_element),document);
		};
		if (tt_toolbox_notebook != NULL) { // new on 170105
			Sprite *page4 = tt_toolbox_notebook->pointer_to_page(4); // hard wiring that the Sensor notebook is on page 4 -- OK??
			if (page4 != NULL && page4->kind_of() == PROGRAM_PAD) {
				Notebook *sensor_notebook = (Notebook *) page4;
				if (sensor_notebook->pages_instatiated()) {
					xml_set_attribute(toolbox_element,L"SensorNotebookPagesInstatiated",1);
				};
			};
		};
//		return; // no need to record more stuff (I assume); -- commented out on 060504
	};
   if (tt_toolbox == this) {
      xml_set_attribute(toolbox_element,L"IsTooly",1);
   };
	for (int i = 0; i < TOOLBOX_STACK_COUNT; i++) { // new on 250803 so cities load perfectly accurately
		xml_element *hole = create_xml_element(L"Hole",document,i+1); // one indexing best???
      stacks[i]->xml_create_and_append_element(hole,document)->Release();
		xml_append_child(hole,toolbox_element); // moved on 190804
	};
   if (tt_dump_reason != DUMPING_TO_CLIPBOARD) { // new on 151102
      xml_set_attribute(toolbox_element,L"GoalX",goal_x);
      xml_set_attribute(toolbox_element,L"GoalY",goal_y);
		xml_set_attribute(toolbox_element,L"GoalSize",percentage_size_goal); // missing prior to 180803
   };
//#if TT_POST323
//	if (saved_followers != NULL) {
//		followers = saved_followers;
//	};
//#endif
	Sprite::xml(toolbox_element,document);
//#if TT_POST323
//	if (saved_followers != NULL) {
//		followers = NULL; // restore
//	};
//#endif
//   append_standard_elements(toolbox_element,document); // handled by Sprite::xml now (121102)
};

const SpriteType stack_type[TOOLBOX_STACK_COUNT] = {INTEGER,TEXT,CUBBY,NEST,PICTURE,ROBOT,TRUCK,BOMB};

boolean Toolbox::handle_xml_tag(Tag tag, xml_node *node) {
	switch (tag) {
		case HOLE_TAG: {
			int stack_number = xml_get_int(node,0);
			if (stack_number == 0) {
				return(FALSE); // couldn't parse as number (or was 0 but these are one-indexed) -- warn?? 
			};
         if (stack_number > TOOLBOX_STACK_COUNT) {
            return(FALSE); // warn?? 
         };
         stack_number--; // switch to zero indexing
         Sprite *item = xml_get_sprite(node,stacks[stack_number],stack_type[stack_number]);
			if (item == NULL) {
				return(FALSE); // warn??
			};
			if (item != stacks[stack_number]) { // not just updating existing elements 
				// new on 231103
				if (stacks[stack_number] != NULL) { // fixes what must be a tiny leak -- I think this only happens if selected so that the item is a Link 
					item->set_size_and_location_to_those_of(stacks[stack_number]);
					remove_follower(stacks[stack_number]);
					stacks[stack_number]->destroy();
				};
				stacks[stack_number] = item;
				add_follower(item);
				//stacks[stack_number] = item;
				//add_follower(item,TRUE,INSERT_UNORDERED,TRUE);
//			} else if (followers == NULL || followers->member_cons(item) == NULL) { // removed probably during special toolbox caching - updated 041103
//				add_follower(item,TRUE,INSERT_UNORDERED,TRUE);
			};
							};
			return(TRUE);
		case VACUUM_TAG: // new on 170404
			if (tt_toolbox_vacuum != NULL) {
				return(xml_load_sprite(node,tag,tt_toolbox_vacuum,VACUUM,FALSE) != NULL);
			} else {
				tt_error_file() << "Tooly recorded the state of Dusty, but Dusty isn't around to receive the information." << endl;
				return(FALSE);
			};
		case MAGIC_WAND_TAG: // new on 170404
			if (tt_toolbox_copier != NULL) {
				return(xml_load_sprite(node,tag,tt_toolbox_copier,COPIER,FALSE) != NULL);
			} else {
				tt_error_file() << "Tooly recorded the state of Maggie, but Maggie isn't around to receive the information." << endl;
				return(FALSE);
			};
		case BIKE_PUMP_TAG: // new on 170404
			if (tt_toolbox_expander != NULL) {
				return(xml_load_sprite(node,tag,tt_toolbox_expander,EXPANDER,FALSE) != NULL);
			} else {
				tt_error_file() << "Tooly recorded the state of Pumpy, but Pumpy isn't around to receive the information." << endl;
				return(FALSE);
			};
		case NO_MORE_TAGS: // new on 041103 since the old code did this for every tag (e.g. ACTIVE_TAG, GEOMTRY_TAG, ...)
			goal_x = xml_get_attribute_int(node,L"GoalX",goal_x);
			goal_y = xml_get_attribute_int(node,L"GoalY",goal_y);
			percentage_size_goal = xml_get_attribute_int(node,L"GoalSize",percentage_size_goal);
			offstage_flag = xml_get_attribute_int(node,L"Offstage",FALSE); // new on 060504
			gone_for_good_flag =	xml_get_attribute_int(node,L"GoneForGood",FALSE); // new on 120704
			if (xml_get_attribute_int(node,L"SensorNotebookPagesInstatiated",0)) {
				if (tt_toolbox_notebook != NULL) {
					Sprite *page4 = tt_toolbox_notebook->pointer_to_page(4); // hard wiring that the Sensor notebook is on page 4 -- OK??
					if (page4 != NULL && page4->kind_of() == PROGRAM_PAD) {
						Notebook *sensor_notebook = (Notebook *) page4;
						sensor_notebook->instantiate_pages();
					}; // otherwise warn??
				};
			};
			// and fall through to Sprite::handle_xml_tag
		default: // new on 151102
			//goal_x = xml_get_attribute_int(node,L"GoalX",goal_x); // commented out 041103
			//goal_y = xml_get_attribute_int(node,L"GoalY",goal_y);
			//percentage_size_goal = xml_get_attribute_int(node,L"GoalSize",percentage_size_goal);		
		   return(Sprite::handle_xml_tag(tag,node));
	};
};

#endif

//boolean is_wand(SpriteType type) { // noticed this was obsolete on 220901
//  return(type == VACUUM || type == COPIER || type == EXPANDER);
//};

/*
Tool *Tool::pop_off_stack(TTRegion *region, SelectionReason reason,
								  Screen *screen, Floor *floor,
								  SelectionStatus &selection_status) {
	if (reason == COPIER_USED) {
		infinite_stack_flag = FALSE; // pretend its not a stack for a second
		Tool *selection = select(region,reason,screen,floor,selection_status);
		infinite_stack_flag = TRUE;
		return(selection);
	} else {
		Tool *the_copy = (Tool *) copy(floor);
		floor->record_event(SELECT_STACK_ITEM,this);
		// following is just to create an index
		floor->record_event(NEW_ITEM,the_copy);
		floor->add_item(the_copy);
		// can't select something animating (too general a restriction?)
		// so select it first and then begin animation
		the_copy = 
			the_copy->select(region,reason,screen,floor,selection_status);
		the_copy->animate_to_saved_size(1000);
		return(the_copy);
	};
};
*/


/*

void Tool::generate_help(output_stream &message, Sprite *tool_in_hand) {
	History history = tt_histories[kind_of()];
	boolean done_all = history_full();
	if (!done_all || tt_help_counter > 0) {
      boolean full_help = (this == tt_help_target || this == tool_in_hand);
		if (tt_help_counter < 2 && !full_help) { // just once
			Sprite::generate_help(message,tool_in_hand);
			if (tool_in_hand != this && !done_all && this != tt_help_target) {
				message << S(IDS_GENERIC_TOOL_HELP);
				if (history == 0) {
					if (kind_of() == COPIER) {
						message << S(IDS_IT_AS_COPY_WAND); // already introduced in Sprite::generate_help
						message << S(IDS_GENERIC_TOOL_HELP2);
					} else {
						message << S(IDS_HIM_AS_NON_WAND_TOOL); // ok??
						message << " " << S(IDS_IF_YOU_PICK_HIM);
					};
				} else {
					describe_type(message,DEFINITE_ARTICLE);
					message << S(IDS_GENERIC_TOOL_HELP2);
				};
			};
		};
		if (full_help) {
			// misnamed but generates full help
			held_help(message,this);			
		};
	};
};

//	if (selection == NULL) return; // why not give help here too??
	if ((!did(EXPANDER_SHRUNK_ITEM) && tt_help_counter == 0)
		 || tt_help_counter == 2) {
		tt_help_history_action = EXPANDER_SHRUNK_ITEM;
		message << S(IDS_EXPANDER_SHRUNK_ITEM_HELP);
		describe_right_button(message);
		message << S(IDS_EXPANDER_SHRUNK_ITEM_HELP2);
		selection->describe_type(message,DEFINITE_ARTICLE);
		message << S(IDS_BIGGER) << end_sentence() << S(IDS_EXPANDER_SHRUNK_ITEM_HELP3);
		selection->describe_type(message,DEFINITE_ARTICLE);			
		message << S(IDS_EXPANDER_SHRUNK_ITEM_HELP4);
		return;
	};
	if ((!did(EXPANDER_GREW_VERTICALLY) && tt_help_counter == 0)
		 || tt_help_counter == 3) {
		tt_help_history_action = EXPANDER_GREW_VERTICALLY;
		message << S(IDS_EXPANDER_GREW_Y_HELP) << " ";
		describe_middle_button(message);
		message << " " << S(IDS_THEN) << " ";
//		message << S(IDS_EXPANDER_GREW_Y_HELP2);
		selection->describe_type(message,DEFINITE_ARTICLE);				
		message << " " << S(IDS_EXPANDER_GREW_Y_HELP3) << end_sentence();
		return;
	};
	if ((!did(EXPANDER_GREW_HORIZONTALLY) && tt_help_counter == 0)
		 || tt_help_counter == 4) {
		tt_help_history_action = EXPANDER_GREW_HORIZONTALLY;
		message << S(IDS_EXPANDER_GREW_Y_HELP) << " ";
		describe_middle_button(message);
		message << " " << S(IDS_THEN) << " ";
//		message << S(IDS_EXPANDER_GREW_Y_HELP2);
		selection->describe_type(message,DEFINITE_ARTICLE);				
		message << " " << S(IDS_EXPANDER_GREW_Y_HELP3) << " " << S(IDS_AS_BEFORE);
		message << S(IDS_EXPANDER_GREW_X_HELP4);
		return;
	};
*/

