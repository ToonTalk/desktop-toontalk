// Copyright (c) 1992-2007. Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif   
#if !defined(__TT_UTILS_H)
#include "utils.h"
#endif 
#if !defined(__TT_COMMON_H)
#include "common.h"
#endif  
#if !defined(__TT_USER_H)   
#include "user.h"
#endif   
#if !defined(__TT_REGION_H)   
#include "region.h"
#endif
#if !defined(__TT_LOG_H)   
#include "log.h"
#endif
#if !defined(__TT_SCREEN_H)   
#include "screen.h"
#endif   
#if !defined(__TT_CITY_H)
#include "city.h"
#endif   
#if !defined(__TT_HOUSE_H)
#include "house.h"
#endif
#if !defined(__TT_SPRITE_H)
#include "sprite.h"
#endif
#if !defined(__TT_ROOM_H)
#include "room.h"
#endif
#if !defined(__TT_NUMBER_H)
#include "number.h"
#endif
#if !defined(__TT_TOOLS_H)
#include "tools.h"
#endif
#if !defined(__TT_CUBBY_H)
#include "cubby.h"
#endif
#if !defined(__TT_ROBOT_H)
#include "robot.h"
#endif
#if !defined(__TT_THOUGHT_H)
#include "thought.h"
#endif
#if !defined(__TT_STRINGS_H)
#include "strings.h"
#endif
#if !defined(__TT_WINMAIN_H)
#include "winmain.h"
#endif
#if !defined(__TT_PAD_H)
#include "pad.h"
#endif
#if !defined(__TT_PICTURE_H)
#include "picture.h"
#endif
#if !defined(__TT_EVENT_H)
#include "event.h"
#endif
#if !defined(__TT_MARTIAN_H)
#include "martian.h"
#endif
#if !defined(__TT_FLOOR_H)
#include "floor.h"
#endif
#if !defined(__TT_HELP_H)
#include "help.h"
#endif
#if !defined(__TT_PUZZLE_H)
#include "puzzle.h"
#endif
#if !defined(__TT_SPEAK_H)
#include "speak.h"
#endif
#if !defined(__TT_CLEANUP_H)
#include "cleanup.h"
#endif
#if !defined(__TT_MOUSE_H)
#include "mouse.h"
#endif
#if TT_DIRECT_INPUT
#if !defined(__TT_INPUT_H)
#include "input.h"
#endif
#endif
#include <math.h>
#if !TT_WINDOWS
#include "sound.h"
#endif
#include <time.h>
#if !defined(__TT_PRGRMMR_H)
#include "prgrmmr.h"
#endif

#include "guid.h"

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// millisecond time_left_last_house = 0;


Programmer::Programmer() : // City *city) :
//  current_screen(screen),
//  city(city),
  room(NULL),
  next_status(PROGRAMMER_NORMAL),
  state(NULL), // new on 190103
//  saved_state(NULL),
//  saved_leaders(NULL),
  empty_helicopter(NULL),
//  floor(new Floor()),
  saved_hand(EMPTY_HAND),
  tool_in_hand(NULL),
//  just_hit_door(FALSE),
  timer(0),
  x_speed(0),
  y_speed(0),
  keyboard_accelerators_enabled(FALSE),
  starting_scale(initial_scale),
  starting_status(START_FLYING), // unless overridden by loading a city
  starting_location_set(FALSE),  // unless overridden by loading a city
  starting_direction(NORTH),
  starting_view_center_x(0), // wasn't initialized prior to 111202
  starting_view_center_y(0),
//  titles_over_flag(FALSE),
  starting_robot(NULL), // used only if saved while training
  starting_floor(NULL),
  starting_cubby(NULL),
  starting_original_cubby(NULL),
  file_format_version_number(latest_version_number),
  saved_geometry_and_misc(NULL),
  empty_helicopter_x(0), // wasn't initialized prior to 050103
  empty_helicopter_y(0), 
  saved_delta_xs(NULL), // wasn't initialized prior to 061102
  saved_delta_ys(NULL),
  saved_delta_times(NULL),
  starting_x(0), // wasn't initialized prior to 060103
  starting_y(0),
  time_of_last_user_input(max_long), // until reset
  invisible_flag(FALSE),
  sit_llx(0), // these were not initialized prior to 230603 so on the ground can suddenly jump to the water
  sit_lly(0),
  grasp_delta_x(0),
  grasp_delta_y(0),
  width_at_time_of_grasp(0),
  height_at_time_of_grasp(0) { 
//  city_dumped_to_log(FALSE),
//  city_load_from_log_frame(-1) {
	state = new Programmer_Titles_Flying();
//	set_timer(5000); // new on 200800 - wait 5 seconds
//	set_next_status(PROGRAMMER_RUNNING_TIMER);
//	status_when_timer_finished = NEXT_TITLE_IF_SHOWING_TITLES;
   person_on_ground_width = maximum_width(PERSON_WALKING_SPRITE)/2; // width when walking in the city
//   person_on_ground_height = (person_on_ground_width*ideal_screen_height)/ideal_screen_height;
	person_on_ground_height = (person_on_ground_width*ideal_screen_height)/ideal_screen_width; // fixed on 070699
   initialize();
};

void Programmer::initialize() { // abstracted on 040204
	if (tt_system_mode == PUZZLE || !tt_scrolling_ok) { // !tt_scrolling_ok new on 030204
      floor_width_sitting_ratio = 1;
      floor_height_sitting_ratio = 1;
   } else {
      floor_width_sitting_ratio = 3;
      floor_height_sitting_ratio = 4;
   };
   llx_right_room_limit = (((floor_width_percent*ideal_screen_width)/100)*(floor_width_sitting_ratio-1))/floor_width_sitting_ratio+left_room_margin;
   lly_top_room_limit = (ideal_screen_height*floor_height_percent*(floor_height_sitting_ratio-1))/(floor_height_sitting_ratio*100);
   Sprite *appearance = pointer_to_appearance();
	if (appearance != NULL) {
	   tt_screen->add(appearance);
   };
};

Programmer::~Programmer() {
	if (state != NULL) delete state;
	//if (saved_state != NULL) {
	//	delete saved_state;
	//};
	if (tool_in_hand != NULL) {
		tool_in_hand->destroy(); // new on 071100
	};
   // following moved here on 061102
   if (saved_delta_xs != NULL) delete [] saved_delta_xs;
	if (saved_delta_ys != NULL) delete [] saved_delta_ys;
	if (saved_delta_times != NULL) delete [] saved_delta_times;
//#if TT_XML
//	if (xml_after_titles != NULL) {
//		xml_after_titles->Release();
//	};
//#endif
};

//Tool *Programmer::pointer_to_current_selection() {
//	return(state->pointer_to_current_selection());
//};

boolean Programmer::part_of_selection(Sprite *item) {
   if (state == NULL) return(FALSE);
	return(state->part_of_selection(item));
};

void Programmer::display_appearance() {
   if (state == NULL) return;
	state->display_appearance();
};

Sprite *Programmer::pointer_to_appearance() {
   if (state == NULL) return(NULL);
	return(state->pointer_to_appearance());
};

boolean Programmer::training() { // new on 050504
	if (state == NULL) return(FALSE);
	Sprite *appearance = state->pointer_to_appearance();
	if (appearance == NULL) return(FALSE);
	return(appearance->kind_of() == ROBOT_IN_TRAINING);
};

Background *Programmer::pointer_to_background() {
   if (state == NULL) return(NULL);
	Sprite *appearance = state->pointer_to_appearance();
   if (appearance == NULL) return(NULL);
   return(appearance->pointer_to_background());
};

boolean Programmer::save_appearance_regions() {
   if (state == NULL) return(TRUE);
	return(state->save_appearance_regions());
};

void Programmer::completed() { // new on 181102 -- forwards to state
   if (state != NULL) {
	   state->completed();
   };
};

Programmer_At_Floor *Programmer::programmer_at_floor() {
	// prior to 241000 it was just trust the caller
#if TT_CAREFUL
	if (state == NULL || state->kind_of() != PROGRAMMER_AT_FLOOR) {
		log("Warning: something wrong with Programmer::programmer_at_floor()",FALSE,TRUE);
	};
#endif
	return((Programmer_At_Floor *) state);
};

void Programmer::set_timer(millisecond new_time) {
//	timer = (100*new_time)/tt_speed;
   timer = new_time;
	// commented out on 201000 since subsumed by set_next_status
//	if (new_time > 0) {
//		postpone_new_logs(); // new on 120100 so don't dump within a timed programmer sequence
//	};
};

void Programmer::set_next_status(ProgrammerStatus new_next_status) {
	if (next_status == new_next_status) return;
	//if (next_status == PROGRAMMER_NORMAL) {
	//	postpone_new_logs(); // new on 200100 
	//} else if (new_next_status == PROGRAMMER_NORMAL) {
	//	stop_postponing_new_logs();
	//};
	next_status = new_next_status;
};

void Programmer::forget_tool_in_hand() {
   if (tool_in_hand == NULL) return;
	tool_in_hand->set_held_status(NOT_HELD);
   tool_in_hand->destroy();
   tool_in_hand = NULL;
//   hand = EMPTY_HAND;
};

void Programmer::release_items_held() {
	if (kind_of() == PROGRAMMER_AT_FLOOR) {
		programmer_at_floor()->release_items_held();
		programmer_at_floor()->pointer_to_appearance()->set_parameter(HAND_OPEN);
	} else {
		tool_in_hand = NULL;
//		hand = EMPTY_HAND; 
	};
};

Sprite *Programmer::release_tool_in_hand() {
   if (tool_in_hand == NULL) return(NULL);
	tool_in_hand->set_held_status(NOT_HELD);
   Sprite *saved = tool_in_hand;
   tool_in_hand = NULL;
//   hand = EMPTY_HAND;
   return(saved);
};

void Programmer::set_tool_in_hand(Sprite *sprite, boolean tell_state_too) {
	// added ref counting stuff on 231000
	if (tool_in_hand != NULL) {
		tool_in_hand->destroy(pointer_to_appearance()); // typically decrements ref count
	};
   tool_in_hand = sprite;
	if (tool_in_hand != NULL) { // conditional missing prior to 031100 so retraining breaks
		tool_in_hand->increment_ref_count(pointer_to_appearance());
	};
	if (tell_state_too && kind_of() == PROGRAMMER_AT_FLOOR) {
      programmer_at_floor()->set_tool_in_hand(sprite);
	};
};

Sprite *Programmer::pointer_to_tool_in_hand() {
   if (kind_of() == PROGRAMMER_AT_FLOOR) {
      return(programmer_at_floor()->pointer_to_tool_in_hand());
   };
   return(tool_in_hand);
};

ProgrammerState Programmer::kind_of() {
	if (state != NULL) return(state->kind_of());
   return(PROGRAMMER_IN_TRANSITION);
};

city_scale Programmer::return_city_scale() {
   if (state != NULL) return(state->return_city_scale());
   return(ground_scale);
};

SpriteCode sit_sprite() {
	switch (tt_programmer_head) {
		case 1:
			return(SIT_WITH_HAIR_SPRITE);
		case 2:
			return(SIT_WITH_HAT_SPRITE);
	};
   return(SIT_SPRITE);
};

void Programmer::translate_ground_to_city_coordinates(city_coordinate &w, city_coordinate &h, city_coordinate &x, city_coordinate &y) {
	// new on 110100
	w = (city_coordinate) ((w*(int64) person_on_ground_width)/ideal_screen_width); // int64 added on 080202
	h = (city_coordinate) ((h*(int64) person_on_ground_height)/ideal_screen_height);
   x = (city_coordinate) ((x*(int64) person_on_ground_width)/ideal_screen_width+saved_city_x);
   y = (city_coordinate) ((y*(int64) person_on_ground_height)/ideal_screen_height+saved_city_y);
};

void Programmer::translate_ground_to_city_coordinates(Sprite *item) {
	// could use the above for modularity...
   city_coordinate corner_llx = saved_city_x;  // lower left corner of screen
   city_coordinate corner_lly = saved_city_y;
   city_coordinate item_x, item_y;
   item->lower_left_corner(item_x,item_y);
	if (tt_log_version_number >= 63 && pointer_to_background()->on_the_ground()) {	//  new on 121204 to deal with issue 588
		item_x += saved_x;
		item_y += saved_y;
	};
   item_x = (city_coordinate) (((int64) item_x*person_on_ground_width)/ideal_screen_width+corner_llx);
   item_y = (city_coordinate) (((int64) item_y*person_on_ground_height)/ideal_screen_height+corner_lly);
	// fixed the placement of (int64) in the following on 150202
   item->set_size_and_location((city_coordinate) ((item->current_width()*(int64) person_on_ground_width)/ideal_screen_width), // added int64 on 060202 to deal with overflow
                               (city_coordinate) ((item->current_height()*(int64) person_on_ground_height)/ideal_screen_height),
                               item_x,item_y,TRANSLATING_GROUND_TO_CITY_COORDINATES); // switch coordinate systems
};

void Programmer::ground_containment_region(TTRegion &region) {
   region.min_x = (saved_city_x/person_on_ground_width)*-ideal_screen_width;
 	region.max_x = region.min_x+(tt_city->max_x()/person_on_ground_width)*ideal_screen_width;
 	region.min_y = (saved_city_y/person_on_ground_height)*-ideal_screen_height;
 	region.max_y = region.min_y+(tt_city->max_y()/person_on_ground_height)*ideal_screen_height;
//	tt_city->extent(region.min_x,region.max_x,region.min_y,region.max_y);
};

void Programmer::transfer_item_to_city(Sprite *item, Background *floor) {
	// was transfer_picture_to_city prior to 060699
	translate_ground_to_city_coordinates(item);
//            region_picture->add_follower(sprite);
//            city->add_extra(sprite,picture_x,picture_y);
   floor->remove_item(item,FALSE); // don't warn if not there
	// commented out the first branch since it is too hard to know if it might move - e.g. might have a speed
//   picture->set_priority_function_of_lly(TRUE);
//   if (picture->flipped_followers() == NULL) {
      // not quite the right location -- might cause a problem in the street
//     city->add_extra(picture,corner_llx,corner_lly); // associated with block for performance
//  } else {
   tt_city->add_item(item); // free-floating but more costly
//   };
};

void Programmer::add_stuff_on_ground_to_city() {
   Sprite *appearance = pointer_to_appearance();
   Background *floor = appearance->pointer_to_background();
   if (floor == NULL) return; // can this happen?
   Sprites *remaining = floor->pointer_to_items();
   while (remaining != NULL) {
      Sprite *sprite = remaining->first();
		sprite->save_floor_priority();
		transfer_item_to_city(sprite,floor); // new on 060699
/*
      switch (sprite->kind_of()) {
         case PICTURE:
           {
            Picture *picture = (Picture *) sprite;
            if (!picture->is_flipped()) transfer_picture_to_city(picture,floor);
           };
            break;
         case TEXT:
         case INTEGER:
           transfer_picture_to_city(make_indirect_picture(sprite,FALSE),floor);
           break;
      };
*/
      remaining = remaining->rest();
   };
};

/* oboselete as of 060699
void Programmer::remove_stuff_on_ground() {
   Sprite *appearance = pointer_to_appearance();
   Background *floor = appearance->pointer_to_background();
   if (floor == NULL) return; // can this happen?
   Sprites *remaining = floor->pointer_to_items();
   while (remaining != NULL) {
      Sprite *sprite = remaining->first();
      switch (sprite->kind_of()) {
         case PICTURE:
         case TEXT:
         case INTEGER:
           break;
         default:
           // shouldn't do this to Tooly and his stuff
           if (!sprite->infinite_stack() && sprite != tt_toolbox &&
               !(tt_toolbox != NULL && toolbox_tool(sprite)) && !sprite->held()) {
              sprite->set_visible(FALSE); // since they aren't picked up or retrievable -- will be destroyed when ground is
           };
      };
      remaining = remaining->rest();
   };
};
*/

void Programmer::translate_floor_to_room_coordinates(city_coordinate &x, city_coordinate &y) {
#if TT_DEBUG_ON
   if (tt_debug_mode == 1924) {
      tt_error_file() << "From floor to room:" << endl;
      tt_error_file() << "x=" << x << " ;sit_llx=" << sit_llx << endl;
      tt_error_file() << "y=" << y << " ;sit_lly=" << sit_lly << endl;
   };
#endif
   x = (floor_width_percent*x)/(floor_width_sitting_ratio*100)+sit_llx;
   y = (floor_height_percent*y)/(floor_height_sitting_ratio*100)+sit_lly;
#if TT_DEBUG_ON
   if (tt_debug_mode == 1924) {
      tt_error_file() << "new_x=" << x << endl;
      tt_error_file() << "new_y=" << y << endl;
   };
#endif
};

void Programmer::translate_room_to_floor_coordinates(city_coordinate &x, city_coordinate &y) {
#if TT_DEBUG_ON
   if (tt_debug_mode == 1924) {
      tt_error_file() << "From room to floor:" << endl;
      tt_error_file() << "x=" << x << " ;sit_llx=" << sit_llx << endl;
      tt_error_file() << "y=" << y << " ;sit_lly=" << sit_lly << endl;
   };
#endif
//   x = (((100*(x-left_room_margin))/floor_width_percent)-sit_llx)*floor_width_sitting_ratio;
   x = (100*floor_width_sitting_ratio*(x-sit_llx))/floor_width_percent;
   y = (100*floor_height_sitting_ratio*(y-sit_lly))/floor_height_percent;
#if TT_DEBUG_ON
   if (tt_debug_mode == 1924) {
      tt_error_file() << "new_x=" << x << endl;
      tt_error_file() << "new_y=" << y << endl;
   };
#endif
};

void Programmer::translate_floor_to_room_dimensions(city_coordinate &w, city_coordinate &h) {
	if (w > 1000000) { // new on 150202 to deal with overflows
		w = floor_width_percent*((w/floor_width_sitting_ratio)/100);
	} else {
		w = ((floor_width_percent*w)/floor_width_sitting_ratio)/100;
	};
	if (h > 1000000) {
		h = floor_height_percent*((h/floor_height_sitting_ratio)/100);
	} else {
		h = ((floor_height_percent*h)/floor_height_sitting_ratio)/100;
	};
};


city_coordinate Programmer::translate_floor_to_room_width(city_coordinate w) {
	if (w > 1000000) { // new on 150202 to deal with overflows
		return(floor_width_percent*((w/floor_width_sitting_ratio)/100));
	} else {
		return(((floor_width_percent*w)/floor_width_sitting_ratio)/100);
	};
};

city_coordinate Programmer::translate_floor_to_room_height(city_coordinate h) {
	if (h > 1000000) {
		return(floor_height_percent*((h/floor_height_sitting_ratio)/100));
	} else {
		return(((floor_height_percent*h)/floor_height_sitting_ratio)/100);
	};
};

void Programmer::translate_room_to_floor_dimensions(city_coordinate &w, city_coordinate &h) {
	if (w > 1000000) { // new on 150202 to deal with overflows
		w = 100*floor_width_sitting_ratio*(w/floor_width_percent);
	} else {
		w = (100*w*floor_width_sitting_ratio)/floor_width_percent;
	};
	if (h > 1000000) {
		h = 100*floor_height_sitting_ratio*(h/floor_height_percent);
	} else {
		h = (100*h*floor_height_sitting_ratio)/floor_height_percent;
	};
};

city_coordinate Programmer::translate_room_to_floor_width(city_coordinate w) { // new on 040603
	if (w > 1000000) { // new on 150202 to deal with overflows
		return(100*floor_width_sitting_ratio*(w/floor_width_percent));
	} else {
		return((100*w*floor_width_sitting_ratio)/floor_width_percent);
	};
};

city_coordinate Programmer::translate_room_to_floor_height(city_coordinate h) { // new on 040603
	if (h > 1000000) {
		return(100*floor_height_sitting_ratio*(h/floor_height_percent));
	} else {
		return((100*h*floor_height_sitting_ratio)/floor_height_percent);
	};
};

const long priority_drop = 1000000L;

void Programmer::move_stuff_on_floor_to_room(Room *room) {
   Background *floor = room->pointer_to_floor();
   if (floor == NULL) return; // can this happen?
   Sprites *remaining = floor->pointer_to_items();
	if (remaining == NULL) return;
	remaining->finish_instantly(); // do this first since it may change what is in the room (added 150999)
	remaining = floor->pointer_to_items(); // in case it did change
	tt_moving_stuff_from_floor_to_room = TRUE;
   while (remaining != NULL) {
      Sprite *sprite = remaining->first();
//		sprite->save_floor_priority(); // saved before this as of 300803
//		if (tt_log_version_number > 16) sprite->finish_instantly(); // added on 070999 since otherwise might animate to wrong size
      if (sprite->move_between_floor_and_room() && sprite->pointer_to_background() == floor) { // and hasn't been moved
			//boolean is_active = sprite->active();
			//sprite->set_active(FALSE,FALSE); // new on 040603
			UnwindProtect<Background *> set(tt_next_background,room); // new on 121204
         floor->remove_item(sprite,FALSE);
			translate_from_floor_to_room(sprite);
         room->add_item(sprite,!sprite->priority_fixed(),FALSE,FALSE); // FALSE added on 310899 to eliminate spurious warnings since while getting up a robot might have moved something to the room
			//if (is_active) {
			//	sprite->set_active(TRUE,FALSE);
			//};
      };
      remaining = remaining->rest();
   };
	tt_moving_stuff_from_floor_to_room = FALSE;
};

void Programmer::move_stuff_in_room_to_floor(Room *room) {
   Background *floor = room->pointer_to_floor();
   if (floor == NULL) return; // can this happen?
   Sprites *remaining = room->pointer_to_items();
	if (remaining == NULL) return; // added 071100 - happens when sent back to an earlier puzzle and maybe more
	remaining->finish_instantly(); // do this first since it may change what is in the room (added 150999)
	remaining = room->pointer_to_items(); // in case it did change
	tt_moving_stuff_from_room_to_floor = TRUE;
   while (remaining != NULL) {   // not decorations when rooms have them -- use coordinates to tell??
      Sprite *sprite = remaining->first();
		Sprites *next_remaining = remaining->rest(); // follow pointer now in case list is changed
      if (sprite->move_between_floor_and_room() && sprite->pointer_to_background() == room) { // check in case moved while say moving robot from room to floor
         // moved following before remove_item so has floor when running
//			if (tt_log_version_number > 16) sprite->finish_instantly(); // added on 070999 since otherwise might animate to wrong size
			UnwindProtect<Background *> set(tt_next_background,floor); // new on 121204
         room->remove_item(sprite,FALSE);
         long sprite_priority = sprite->priority();
         if (sprite_priority >= priority_drop && !sprite->priority_fixed()) sprite->set_priority(sprite_priority-priority_drop);  // restore priority
			translate_from_room_to_floor(sprite);
			// 2nd arg below was !sprite->priority_fixed() prior to 080600
         floor->add_item(sprite,FALSE,FALSE,FALSE); // FALSE added on 310899 to eliminate spurious warnings since while getting up a robot might have moved something to the floor
//			if (sprite->priority_fixed()) { // commented out on 080600 since the priority does go wrong otherwise
				// conditional added on 021199 since if priority is given no need to fix floor priority
				sprite->restore_floor_priority();
//			};
      };
      remaining = next_remaining;
   };
	tt_moving_stuff_from_room_to_floor = FALSE;
};

void Programmer::translate_from_floor_to_room(Sprite *sprite) {
#if TT_DEBUG_ON
	if (tt_debug_target == sprite->debug_counter) {
		log("Translating floor to room for debug target.");
	};
#endif
	// pulled out of Programmer::move_stuff_on_floor_to_room on 010500
   // move_between_floor_and_room was containable_by_background prior to 020999
   if (!sprite->priority_fixed() && sprite->priority()+priority_drop > sprite->priority()) {
		// and no overflow added on 100899
		sprite->set_priority(sprite->priority()+priority_drop);  // so programmer and Tooly "step" on stuff on the floor
   };
	city_coordinate llx, lly;
   sprite->lower_left_corner(llx,lly);
   // convert to room scale and map to the visible part of the floor
   city_coordinate sprite_width = sprite->current_width();
   city_coordinate sprite_height = sprite->current_height();
   translate_floor_to_room_coordinates(llx,lly);
   translate_floor_to_room_dimensions(sprite_width,sprite_height);
   sprite->set_size_and_location(sprite_width,sprite_height,llx,lly,TRANSLATING_FLOOR_TO_ROOM_COORDINATES);
//	sprite->translated_from_floor_to_room(); // new on 040603
};

void Programmer::translate_from_room_to_floor(Sprite *sprite) {
	city_coordinate llx, lly;
   sprite->lower_left_corner(llx,lly);
   // convert to room scale and map to the visible part of the floor
   // which is the bottom 7/17 of the screen and the centered 19/24 of the width
   city_coordinate sprite_width = sprite->current_width();
   city_coordinate sprite_height = sprite->current_height();
   translate_room_to_floor_coordinates(llx,lly);
   translate_room_to_floor_dimensions(sprite_width,sprite_height);
   if (tt_system_mode == PUZZLE || !tt_scrolling_ok) { // !tt_scrolling_ok new on 030204
		// sometimes things get caught in the transition and end up way off screen
      // and in puzzle mode there is no scrolling
		if (llx >= ideal_screen_width) {
         llx = ideal_screen_width-tile_width;
		} else if (llx+sprite_width <= 0) {
			llx = tile_width-sprite_width;
		};
		if (lly >= ideal_screen_height) {
         lly = ideal_screen_height-tile_height;
		} else if (lly+sprite_height <= 0) {
			lly = tile_height-sprite_height;
		};
   };
   sprite->set_size_and_location(sprite_width,sprite_height,llx,lly,TRANSLATING_ROOM_TO_FLOOR_COORDINATES);
//	sprite->translated_from_room_to_floor(); // new on 040603
};

void Programmer::shift_sit_corner(Direction direction,
                                  city_coordinate &new_sit_llx,
                                  city_coordinate &new_sit_lly,
                                  city_coordinate &new_sit_urx,
                                  city_coordinate &new_sit_ury) {
   // hypothetical shift NORTH, EAST, SOUTH or WEST
   // following are in "room" coordinates
   city_coordinate screen_width = (floor_width_percent*ideal_screen_width)/(100*floor_width_sitting_ratio);
   city_coordinate screen_height = (floor_height_percent*ideal_screen_height)/(100*floor_height_sitting_ratio);
   const int screen_percent = 10;
   new_sit_llx = sit_llx;
   new_sit_lly = sit_lly;
   switch (direction) {
      case SOUTHEAST:
      case NORTHEAST:
      case EAST:
         new_sit_llx += (screen_percent*screen_width)/100;
         break;
      case SOUTHWEST:
      case NORTHWEST:
      case WEST:
         new_sit_llx -= (screen_percent*screen_width)/100;
         break;
      case NORTH:
         new_sit_lly += (screen_percent*screen_height)/100;
         break;
      case SOUTH:
         new_sit_lly -= (screen_percent*screen_height)/100;
         break;
   };
   switch (direction) { // and missed above
      case SOUTHEAST:
      case SOUTHWEST:
         new_sit_lly -= (screen_percent*screen_height)/100;
         break;
      case NORTHEAST:
      case NORTHWEST:
         new_sit_lly += (screen_percent*screen_height)/100;
         break;
   };
	Background *background = pointer_to_appearance()->pointer_to_background();
	if (background != NULL && background->on_the_floor()) { // restored (and made safer by the NULL check) on 300803
//	if (!floor_not_scrollable()) { // rewritten on 150103 since this is simpler and 
		// needn't worry if the appearance doesn't know its background (crash report from Yishay)
// but it was wrong and caused scrolling problems on the ground
		if (new_sit_llx < left_room_margin) {
			new_sit_llx = left_room_margin;
		} else if (new_sit_llx > llx_right_room_limit) {
			new_sit_llx = llx_right_room_limit;
		};
		if (new_sit_lly < 0) {
			new_sit_lly = 0;
		} else if (new_sit_lly > lly_top_room_limit) {
			new_sit_lly = lly_top_room_limit;
		};
	};
	new_sit_urx = new_sit_llx+screen_width;
	new_sit_ury = new_sit_lly+screen_height;
};

boolean Programmer::set_sit_corner(city_coordinate new_sit_llx, city_coordinate new_sit_lly,
                                   Background *floor,
                                   city_coordinate &delta_x, city_coordinate &delta_y) {
#if TT_DEBUG_ON
   if (tt_debug_mode == 190304) {
		tt_error_file() << "set_sit_corner called. " << endl
							 << "new_sit_llx=" << new_sit_llx << "; new_sit_lly=" << new_sit_lly
							 << "; sit_llx=" << sit_llx << "; sit_lly=" << sit_lly << " on frame " << tt_frame_number << endl;
	};
#endif
	city_coordinate old_sit_llx = sit_llx; // new on 140504
	city_coordinate old_sit_lly = sit_lly;
	just_set_sit_corner(new_sit_llx,new_sit_lly);
	if (tt_log_version_number >= 63 && floor->on_the_ground()) { // moved here on 121204
      // if on the ground uses saved_x and y as cummulative deltas
      saved_x += delta_x;
      saved_y += delta_y;
   };
	delta_x = old_sit_llx-sit_llx; // rewritten on 140504 since the above may not just set sit corner if up against a wall
	delta_y = old_sit_lly-sit_lly;
   //delta_x = sit_llx-new_sit_llx;
   //delta_y = sit_lly-new_sit_lly;
	if (delta_x == 0 && delta_y == 0) {
		return(FALSE); // no change
	};
   city_coordinate floor_delta_x = delta_x,
                   floor_delta_y = delta_y;
   translate_room_to_floor_dimensions(floor_delta_x,floor_delta_y);
	floor->shift_all(floor_delta_x,floor_delta_y);
//   if (floor->on_the_floor() || floor->on_the_ground())
	if (tt_toolbox != NULL && !tt_toolbox->gone()) { // second condition added 030300
		tt_toolbox->shifted(floor_delta_x,floor_delta_y);
	};
	pointer_to_appearance()->move_by(floor_delta_x,floor_delta_y);
//   just_set_sit_corner(new_sit_llx,new_sit_lly); // moved up on 140504
#if TT_DEBUG_ON
   if (tt_debug_mode == 1924) {
      tt_error_file() << "Shifted by " << delta_x << "x" << delta_y << endl;
   };
#endif
   tt_screen->invalidate_cache();
   return(TRUE); // !floor->on_the_ground());
};

HandState compute_hand_state(Sprite *tool_in_hand, boolean robot_in_control) {
	if (tool_in_hand == NULL) {
		return(EMPTY_HAND);
	} else if (!robot_in_control && tool_in_hand->kind_of() == COPIER) {
		return(HOLDING_COPIER);
	} else {
		return(HOLDING_ITEM);
	};
};

boolean Programmer::react(Screen *screen) {
//  just_hit_door = FALSE; // set true when hitting a door
  city_coordinate x,y;
  state->position(x,y);
#if TT_DEBUG_ON
  if (tt_debug_mode == 180703 && tt_titles_ended_on_frame <= tt_frame_number) {
	  tt_error_file() << "Programmer x,y is " << x << "," << y << " on frame " << tt_frame_number 
							<< " and segment number is " << tt_current_log_segment << endl;
  };
  if (tt_debug_mode == 290803) { // || tt_debug_mode == 3) { // == 3 just for now 100105
	  tt_error_file() << "Programmer react while next_status is " << (int) next_status 
		               << " on frame " << tt_frame_number << endl;
  };
#endif
  unsigned char key = 0;
  if (room != NULL) {
	  if (room->slated_for_destruction()) { // get out of there!
		  house_about_to_be_destroyed();
	  };
	  millisecond explosion_time = room->when_will_house_explode();
	  if (explosion_time > 0) { // will be reset so this happens only once
		  if (state->kind_of() == PROGRAMMER_AT_FLOOR) {
			  // just set off a bomb so
			  set_next_status(LEAVE_FLOOR_WHEN_TOOLBOX_READY);
		  };
		  if ((state->kind_of() == PROGRAMMER_ROOM_WALKING && 
				 room->has_item(tt_martian)) ||
				(state->kind_of() == PROGRAMMER_AT_FLOOR &&
				 room->pointer_to_floor()->has_item(tt_martian))) {
			  tt_martian->teleport_out();
			  // get out right away
			  tt_martian->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
		  };
	  };
  };
//  if (titles_over_flag && !city_dumped_to_log) {
	  // wait until titles are over to dump city for log
//	  if (tt_log_out_file_name != NULL) {
//		  dump_city_to_log(); 
//	  };
//	  city_dumped_to_log = TRUE;
//  };
#if TT_XML
  if (next_status == START_AFTER_LOADING_XML) { // new on 261202 -- moved here on 060103 since may have been saved in any state
	  // was next_status which seems to never be true - updated 061003 -- 
	  // removed starting_status on 071003 and restored next_status
//	  starting_status = START_FLYING; // so this doesn't happen again - new on 061003 -- commented out on 071003
	  tt_city->process_xml_after_titles();
//	  tt_screen->initialize_background(NULL); 
	  // copied from case START_FLYING: -- commented out since should be set by activate() below
	  //delete state;
	  //state = NULL;  
	  state->activate(); // if state is NULL is there a way to recover? -
  };
#endif
  ProgrammerStatus old_next_status = next_status; // just a place to store it
  switch (next_status) {
		case PROGRAMMER_NORMAL:
			break; // do nothing special
		case FLYING_AGAIN:
			// landed on the street so take off over it
			screen->remove(pointer_to_appearance());
			x = saved_city_x;
			y = tt_city->nearest_x_street(y);
//         if (tt_mouse_mode == ABSOLUTE_MOUSE_MODE) { // new on 060502
//            y += 100*tile_height;
//         };
		case FLYING:
			state->cleanup(screen,next_status);
			delete state;
			state = NULL; // added 061099 in case some of the following triggers error
			state = new Programmer_City_Flying(x,y,tt_min_flying_scale);
			screen->flush_all();
			screen->add(pointer_to_appearance());
			screen->new_view(CAMERA_ABOVE);
//         if (tt_log_version_number > 9)
         tt_city->now_on_screen(); // this will cause decoration to be added to the screen
//         tt_global_picture->update_roof_decoration();  // add missing decoration to roofs of houses
			break;
		case START_FLYING: { // titles are over or a city was saved while flying
//			state->cleanup(screen,next_status);
			tt_screen->initialize_background(NULL); // added 250100
			delete state;
         state = NULL;
			Direction direction = EAST;
			if (starting_location_set) {
//				if (tt_log_version_number > 18) { 
				// new on 050100
				x = starting_view_center_x; // this is used to keep the camera centered around the helicopter
				y = starting_view_center_y;
//				} else {
//					x = city_view_x; //starting_x;
//					y = city_view_y; //starting_y;
//				};
				direction = starting_direction;
			} else {
				tt_city->center(x,y);
			};
			state = new Programmer_City_Flying(x,y,starting_scale,direction,starting_location_set);
			transfer_saved_gemometry_and_misc();
//         if (saved_geometry_and_misc != NULL) { 
//				appearance->move_to(starting_x,starting_y); // this is where the helicopter was when saved
//				appearance->set_old_x_offset(starting_old_x_offset);
//			   appearance->set_old_y_offset(starting_old_y_offset);
//          screen->update_viewing_region(starting_view_center_x,starting_view_center_y,starting_scale);
//          ((Programmer_City_Flying *) state)->set_center(city_view_x,city_view_y);
//				screen->set_city_to_user_units((starting_scale*ideal_screen_width/1000)/ground_scale,
//                                           (starting_scale*ideal_screen_height/1000)/ground_scale); // new on 161299
//				screen->update_viewing_region(starting_view_center_x,starting_view_center_y,starting_scale);
//			};
         screen->update_viewing_region(x,y,starting_scale);
			screen->new_view(CAMERA_ABOVE); // moved ahead of the following on 240100 for log segment switch
			screen->switch_to(tt_city,TRUE,FALSE);
			screen->add(pointer_to_appearance());
         set_next_status(PROGRAMMER_NORMAL); // added 12/9/97
         tt_city->now_on_screen(); // added 12/9/97
			starting_location_set = FALSE;
			state->position(x,y); // since updated
			};
			break;
		case LANDING:
			state->cleanup(screen,next_status);
         delete state;
			state = NULL; // added 061099 in case some of the following triggers error
         tt_city->now_on_screen(); // this will cause decoration to be added to the screen
			state = new Programmer_City_Landing();
			screen->add(pointer_to_appearance());
//         tt_global_picture->update_house_decoration();
			saved_city_x = previous_x;
#if TT_DEBUG_ON
			if (tt_debug_mode == 190404) {
				tt_error_file() << "LANDING on frame " << tt_frame_number << " segment is " << tt_current_log_segment << endl;
				tt_error_file() << "saved_city_x=" << saved_city_x << endl;
			};
#endif
			break;
		case TAKING_OFF:
			pointer_to_appearance()->set_parameter(HELICOPTER_LANDING);
         pointer_to_appearance()->set_priority(-2);  // in front of everything again
         pointer_to_appearance()->set_priority_fixed(TRUE);
			set_timer(1000);
			x_speed = 0;
			y_speed = 8*tile_height;
			set_next_status(PROGRAMMER_RUNNING_TIMER);
			status_when_timer_finished = PROGRAMMER_NORMAL;
			play_sound(HELICOPTER_SOUND,TRUE);
			break;
		case LEAVE_HELICOPTER:
			pointer_to_appearance()->set_parameter(HELICOPTER_PERSON_LEAVING);
			set_timer(pointer_to_appearance()->total_cycle_duration()+100);
			x_speed = 0;
			y_speed = 0; // don't move just wait for sequence to complete
			set_next_status(PROGRAMMER_RUNNING_TIMER);
			status_when_timer_finished = WALKING_AWAY_FROM_HELICOPTER; //CITY_WALKING;
         stop_sound_id(HELICOPTER_SOUND);
//			state->center(x,y);
//			state->cleanup(screen);
//			delete state;
//			state = new Programmer_Leave_Helicopter(FALSE,x,y,this);
//			screen->add(pointer_to_appearance());
			break;
		case WALKING_AWAY_FROM_HELICOPTER:
			set_timer(500);
			x_speed = 8*tile_width;
			y_speed = -1*tile_height; 
			set_next_status(PROGRAMMER_RUNNING_TIMER);
			status_when_timer_finished = PROGRAMMER_NORMAL;
//			fall thru to the following
		case CITY_WALKING: {
//			play_sound(RUNNING_SOUND,1,TRUE);
			stop_sound(FALSE);
			empty_helicopter = pointer_to_appearance();
         empty_helicopter->set_priority_fixed(FALSE);
			empty_helicopter->set_parameter(HELICOPTER_EMPTY);
			city_coordinate helicopter_width = state->current_width();
			state->cleanup(screen,next_status);
			delete state;
			state = NULL; // added 061099 in case some of the following triggers error
			// programmer starts off behind empty copter facing north
			state = new Programmer_City_Walking(x+(2*helicopter_width)/3,y+tile_height*6,NORTH);
         state->finish_initializing();
			// the above might want to use a specialization like Image
			// the following should be elsewhere (maybe the new "state" should return this)
			empty_helicopter_x = x;
			empty_helicopter_y = y;
		   tt_city->add_extra(empty_helicopter,x,y);
			tt_screen->add(empty_helicopter);
//			empty_helicopter->set_priority_function_of_lly(TRUE);
//			empty_helicopter_screen = screen;
/*
			timer = 1000;
			x_speed = 10*tile_width;
			y_speed = -2*tile_height;
			set_next_status(PROGRAMMER_RUNNING_TIMER);
			status_when_timer_finished = PROGRAMMER_NORMAL;
*/
		 };
			break;
		case START_LANDING: {
         state->cleanup(screen,next_status);
			delete state;
			state = NULL; // added 061099 in case some of the following triggers error
			state = new Programmer_City_Landing();
			transfer_saved_gemometry_and_misc();
			state->position(x,y); // since updated
//         if (starting_location_set && file_format_version_number >= first_version_to_save_programmer_and_remotes_accurately) { 
//				appearance->move_to(starting_x,starting_y); // this is where the helicopter was when saved
//				appearance->set_old_x_offset(starting_old_x_offset);
//				appearance->set_old_y_offset(starting_old_y_offset);
//			};
         screen->new_view(CAMERA_IN_FRONT);
         screen->switch_to(tt_city,FALSE,FALSE);
         screen->add(pointer_to_appearance());
         set_next_status(PROGRAMMER_NORMAL);
								  };
         break;
       case START_CITY_WALKING:
         state->cleanup(screen,next_status);
			delete state;
			state = NULL; // added 061099 in case some of the following triggers error
			state = new Programmer_City_Walking(starting_x,starting_y,starting_direction);
			transfer_saved_gemometry_and_misc();
			state->position(x,y); // since updated
//         if (starting_location_set && file_format_version_number >= first_version_to_save_programmer_and_remotes_accurately) { 
//				Sprite *appearance = pointer_to_appearance();
//				appearance->move_to(starting_x,starting_y);
//				appearance->set_old_x_offset(starting_old_x_offset);
//				appearance->set_old_y_offset(starting_old_y_offset);
//			};
			screen->new_view(CAMERA_IN_FRONT);
         screen->switch_to(tt_city,FALSE,FALSE);
//			screen->add(pointer_to_appearance());
//         screen->add(tt_toolbox);
         add_empty_helicopter(TRUE);
//         state->add_toolbox();
         state->finish_initializing();
         set_next_status(PROGRAMMER_NORMAL);
         break;
       case START_ROOM_WALKING:
         state->cleanup(screen,next_status);
			delete state;
			state = NULL; // added 061099 in case some of the following triggers error
			state = new Programmer_Room_Walking(room,starting_x,starting_y,TRUE,starting_direction);
			transfer_saved_gemometry_and_misc();
			state->position(x,y); // since updated
//         if (starting_location_set && file_format_version_number >= first_version_to_save_programmer_and_remotes_accurately) { 
//				Sprite *appearance = pointer_to_appearance();
//				appearance->move_to(starting_x,starting_y);
//				appearance->set_old_x_offset(starting_old_x_offset);
//				appearance->set_old_y_offset(starting_old_y_offset);
//			};
         screen->new_view(CAMERA_IN_FRONT);
         screen->switch_to(room,FALSE,FALSE);
//         screen->add(tt_toolbox);
         add_empty_helicopter(FALSE); // outside so when leaving room it is there
         state->finish_initializing();
         set_next_status(PROGRAMMER_NORMAL);
         break;
       case START_AT_FLOOR:
		 case START_IN_THOUGHT_BUBBLE: {
         state->cleanup(screen,next_status);
			delete state;
//#if TT_XML
//			if (xml_after_titles != NULL) { // new on 261202
//				process_xml_after_titles();
//				screen->new_view(CAMERA_MOSTLY_ABOVE);
//				screen->switch_to(starting_floor,FALSE,FALSE);
//			} else {
//#else
//			if (TRUE) {
//#endif
				state = NULL; // added 061099 in case some of the following triggers error
				Programmer_At_Floor *programmer_at_floor;
				if (next_status == START_IN_THOUGHT_BUBBLE) {
					programmer_at_floor = new Programmer_At_Floor(room->pointer_to_floor(),starting_x,starting_y,saved_hand,NULL); 
					// why is tool_in_hand NULL here??
					starting_floor->set_room(room); // didn't know it earlier and this is used during training
					// e.g. to see if house is exploding
				} else {
					starting_floor = room->pointer_to_floor();
					programmer_at_floor = new Programmer_At_Floor(starting_floor,starting_x,starting_y,saved_hand,tool_in_hand);
				}; 
				state = programmer_at_floor;
				screen->new_view(CAMERA_MOSTLY_ABOVE);
				screen->switch_to(starting_floor,FALSE,FALSE);
				if (next_status == START_IN_THOUGHT_BUBBLE) { // move this? Asked 261202
//					programmer_at_floor->set_appearance(starting_robot);
//					programmer_at_floor->robot_in_control(TRUE);
					programmer_at_floor->set_thought_bubble_robot(starting_robot);
					Thought_Bubble *bubble = starting_robot->pointer_to_thought_bubble();
	//					Cubby *starting_cubby_copy = (Cubby *) starting_cubby->copy();
					bubble->set_cubby(starting_original_cubby);
//					bubble->add_follower(starting_original_cubby); // set_cubby does this now
					starting_original_cubby = NULL;
					programmer_at_floor->set_original_thought_bubble(bubble);
					starting_robot->set_thought_bubble(NULL);
					if (tool_in_hand != NULL) {
						starting_robot->pick_up_and_store_tool(tool_in_hand);
						tool_in_hand->update_display(); // otherwise right size appears only when dropped -- still true?
	//						hand = HOLDING_ITEM;
	//						programmer_at_floor->set_hand_state(hand);
						programmer_at_floor->set_tool_in_hand(tool_in_hand);
					};
					starting_robot = NULL;
					if (tt_toolbox != NULL && !starting_location_set && !tt_toolbox->gone()) { // last condition added 030300
						// !starting_location_set added 130100 since no need if dumped
						tt_toolbox->enter(FALSE);
					};
					programmer_at_floor->enter_thought_bubble(NULL,starting_floor);
				} else {
					state->finish_initializing();
				};
				transfer_saved_gemometry_and_misc(); // moved here on 170100 since above may switch which appearance 
				// (e.g. holding wand) is set and this clobbers it
				state->position(x,y); // since updated
				if (starting_location_set && file_format_version_number >= first_version_to_save_programmer_and_remotes_accurately &&
					 tt_toolbox != NULL) {
					tt_toolbox->add_self_and_friends(starting_floor); // new on 130100
				} else {
					starting_floor->restore_toolbox();
				};
//			};
//         starting_floor->add_item(tt_toolbox,FALSE,FALSE);
//         screen->add(tt_toolbox);
//         tt_toolbox->enter();
         add_empty_helicopter(FALSE);
			set_next_status(PROGRAMMER_NORMAL);
//         if (starting_location_set && file_format_version_number >= first_version_to_save_programmer_and_remotes_accurately) { 
//				Sprite *appearance = pointer_to_appearance();
//				appearance->move_to(starting_x,starting_y);
//				appearance->set_old_x_offset(starting_old_x_offset);
//				appearance->set_old_y_offset(starting_old_y_offset);
//			};
//			screen->add(pointer_to_appearance());
												 };
         break;
       case START_ON_THE_GROUND: {
         state->cleanup(screen,next_status);
			delete state;
			Ground *ground = new Ground();
			TTRegion region(saved_city_x,saved_city_x+person_on_ground_width,saved_city_y,saved_city_y+person_on_ground_height);
			tt_city->transfer_pictures(ground,region);
			screen->update_viewing_region(10*tile_width,10*tile_height,ground_scale);
			boolean processed_xml = FALSE;
			state = NULL; // added 061099 in case some of the following triggers error
			state = new Programmer_At_Floor(ground,starting_x,starting_y,saved_hand,tool_in_hand);
			transfer_saved_gemometry_and_misc();
//				state->position(x,y); // since updated
//         if (starting_location_set && file_format_version_number >= first_version_to_save_programmer_and_remotes_accurately) { 
//				Sprite *appearance = pointer_to_appearance();
//				appearance->move_to(starting_x,starting_y);
//				appearance->set_old_x_offset(starting_old_x_offset);
//				appearance->set_old_y_offset(starting_old_y_offset);
//			};
			state->position(x,y); // since updated
//         starting_floor->add_item(tt_toolbox,FALSE,FALSE);
//         screen->add(tt_toolbox);
//         tt_toolbox->enter();
         add_empty_helicopter(FALSE);
			set_next_status(PROGRAMMER_NORMAL);
         screen->new_view(CAMERA_MOSTLY_ABOVE);
         screen->switch_to(ground,FALSE,FALSE);
			if (!processed_xml) {
				transfer_saved_gemometry_and_misc(); 
				// moved here on 170100 since above may switch which appearance (e.g. holding wand) is set and this clobbers it
				if (starting_location_set && file_format_version_number >= first_version_to_save_programmer_and_remotes_accurately &&
					 tt_toolbox != NULL) {
					tt_toolbox->add_self_and_friends(starting_floor); // new on 130100
				} else {
					starting_floor->restore_toolbox();
				};
				state->finish_initializing(); // not if just loaded XML - 261202
				if (starting_location_set && file_format_version_number >= first_version_to_save_programmer_and_remotes_accurately &&
					 tt_toolbox != NULL) {
					tt_toolbox->add_self_and_friends(ground);
				} else {
					ground->restore_toolbox();
				};
			};
         add_empty_helicopter(FALSE);
         set_next_status(PROGRAMMER_NORMAL);
											};
         break;
       case PUZZLE_START:
			delete state;
         state = NULL;
         {
         House *rocket = pointer_to_rocket(); // tt_rocket better??
         city_coordinate exit_x, exit_y;
         rocket->exit_point(exit_x,exit_y);
         screen->update_viewing_region(exit_x-tile_width*5,exit_y-tile_height, // was *6
                                       ground_scale);
         state = new Programmer_City_Walking(exit_x,exit_y,NORTH); // was exit_y-tile_height*8
			// should I add transfer_saved_gemometry_and_misc(); here??
         screen->new_view(CAMERA_IN_FRONT);
         screen->switch_to(tt_city,FALSE,FALSE);
//			screen->add(pointer_to_appearance());
         state->finish_initializing();
         set_next_status(PROGRAMMER_NORMAL);
         };
			break;
		case NEED_HELICOPTER:
		  // if helicopter is off screen bring it near
		  if (!screen->viewing_region()->inside(empty_helicopter_x,empty_helicopter_y) &&
            empty_helicopter != NULL) {
			 // might be moving it to a new block so remove it
			 tt_city->remove_extra(empty_helicopter,empty_helicopter_x,empty_helicopter_y);
			 empty_helicopter_x = x; //+empty_helicopter->current_width()/2,
			 empty_helicopter_y = tt_city->nearest_x_street(y)-3*tile_height;
			 if (y-empty_helicopter_y > tt_block_height/2) { // too low
				 empty_helicopter_y = tt_city->nearest_x_street(y+tt_block_height)-3*tile_height;
			 };
			 empty_helicopter->move_to(empty_helicopter_x,empty_helicopter_y+tt_block_height);
			 empty_helicopter->set_size_percent(5);
			 const millisecond helicopter_appear_duration = 667;
			 empty_helicopter->animate_to_size_percent(100,helicopter_appear_duration);
			 empty_helicopter->animate_delta_location(0,-tt_block_height,helicopter_appear_duration);
			 play_sound(TWING_SOUND);
			 tt_city->add_extra(empty_helicopter,empty_helicopter_x,empty_helicopter_y);
			 tt_screen->add(empty_helicopter);
		  };
		  break;
		case ENTER_HELICOPTER:
			stop_sound(FALSE); 
			// door opening sound?
			empty_helicopter->set_parameter(HELICOPTER_PERSON_RETURNING);
			state->cleanup(screen,next_status);
			delete state;
			state = NULL; // added 061099 in case some of the following triggers error
			state = new Programmer_City_Landing(empty_helicopter);
			tt_city->remove_extra(empty_helicopter,empty_helicopter_x,empty_helicopter_y);
//			screen->add(pointer_to_appearance());
			set_timer(empty_helicopter->total_cycle_duration());
			empty_helicopter = NULL; // isn't this a memory leak??
			x_speed = 0;
			y_speed = 0;
			set_next_status(PROGRAMMER_RUNNING_TIMER);
			status_when_timer_finished = TAKING_OFF;
//			state->cleanup(screen);
//			delete state;
//			enter_helicopter(empty_helicopter_x,empty_helicopter_y,screen);
			break;
		case ABOUT_TO_ENTER_THOUGHT_BUBBLE:
//			timer = 1000; // not quite right if nest or birds cause waiting
//			set_next_status(PROGRAMMER_RUNNING_TIMER);
//			status_when_timer_finished = READY_TO_ENTER_THOUGHT_BUBBLE;
//			x_speed = 0;
//			y_speed = 0;
//			break;
		case READY_TO_ENTER_THOUGHT_BUBBLE:
			set_timer(programmer_at_floor()->expand_thought_bubble());
			x_speed = 0;
			y_speed = -10*tile_height;
			set_next_status(PROGRAMMER_RUNNING_TIMER);
			status_when_timer_finished = ENTERING_THOUGHT_BUBBLE;
			break;
		case ENTERING_THOUGHT_BUBBLE:
			if ((tt_toolbox != NULL && tt_toolbox->pointer_to_leader() != NULL) || // thought bubble fully expanded
             (tt_system_mode == PUZZLE && !is_toolbox_part_of_puzzle())) {
				programmer_at_floor()->enter_thought_bubble(room);
            // above switches to thought bubble background (finishes all animations)
            // so want other stuff to happen on next frame
//            if (!tt_running_old_demos) {
               set_timer(50);
               set_next_status(PROGRAMMER_RUNNING_TIMER);
               status_when_timer_finished = ENTERED_THOUGHT_BUBBLE;
               break;
//            }; // otherwise don't break but fall thru
			} else { // otherwise just "busy" wait in this state
				set_next_status(PROGRAMMER_RUNNING_TIMER);
				status_when_timer_finished = ENTERING_THOUGHT_BUBBLE;
            set_timer(250);
            break;
			};
      case ENTERED_THOUGHT_BUBBLE:
         {
         Robot *robot = programmer_at_floor()->enter_thought_bubble2();
         robot->set_sprite_type(ROBOT_IN_TRAINING);
         robot->set_selectable(FALSE);
//       robot->set_programmer(this);
         robot->appear_working();
         Sprite *initial_tool = robot->pointer_to_initial_tool();
         if (initial_tool != NULL) {
            tool_in_hand = initial_tool;
//            hand = HOLDING_ITEM;
            programmer_at_floor()->set_tool_in_hand(tool_in_hand);
//            programmer_at_floor()->set_hand_state(hand);
            if (initial_tool->kind_of() == COPIER) {
               ((Copier *) initial_tool)->save_counter();
            };
         };
			if (robot->pointer_to_body() != NULL) { // retraining this robot
				robot->restore_original_training_counter();
				millisecond delay;
				if (tt_toolbox == NULL) {
					delay = 2000; // e.g. in puzzles
				} else {
					delay = 4000; // wait for stuff to settle down a bit
				};
				robot->reset_drop_scroll_deltas(); // new on 160405 so location of drops isn't scrolled offscreen
				robot->try_clause(robot->pointer_to_background()->pointer_to_body_cubby(),delay); 
//				robot->receive_item(robot->pointer_to_background()->pointer_to_body_cubby(),pointer_to_appearance(),0,NULL,robot);
         };
			};
         break;
		case LEAVE_FLOOR_WHEN_TOOLBOX_READY:
//#if TT_BETA
//         if (tt_running_old_demos) {
//			   programmer_at_floor()->release_items_held();
//         };
//#endif
  	     programmer_at_floor()->turn_off_item_held();
        if (screen->inside_rule_body()) {
           Robot *robot = (Robot *) pointer_to_appearance();
           Sprite *initial_tool = robot->pointer_to_initial_tool();
           if (initial_tool != NULL && initial_tool->kind_of() == COPIER) {
              ((Copier *) initial_tool)->restore_counter();
           };
			  Sprite *current_tool = robot->pointer_to_tool();
           if (current_tool != initial_tool) {
              if (current_tool != NULL && current_tool->kind_of() != VACUUM) {
					  // new on 090999 - don't drop vacuum since holding vacuum is way to indicate
					  // don't want vacuum_items_unused
					  programmer_at_floor()->release_items_held();
				  };
           } else if (initial_tool != NULL &&
                      initial_tool->kind_of() == COPIER &&
                      ((Copier *) initial_tool)->attached_tool() != NULL) {
              // drop anything attached to wand even
              // if robot keeps the wand
              programmer_at_floor()->release_object(TRUE);
           };
           if (robot->pointer_to_tool() == NULL && initial_tool != NULL) {
          // must have set it down -- since still in thought bubble
               robot->pick_up_and_store_tool(initial_tool);
  //             thought_bubble_robot->finish_instantly();
//               programmer_at_floor()->pick_up(robot->pointer_to_initial_tool());
           };
         };
			if (tt_toolbox != NULL && !tt_toolbox->gone()) { // second condition added 030300
            set_timer(tt_toolbox->leave(TRUE,pointer_to_appearance()));
         } else {
            set_timer(1000);
         };
			// following sometimes overwritten below
			if (room != NULL) {
            status_when_timer_finished = LEAVING_FLOOR;
			   millisecond explosion_time = room->when_will_house_explode();
			   if (explosion_time > 0) {
				    room->forget_explosion_time(); // so only noticed once
				    if (!screen->inside_rule_body()) {
					    previous_x = saved_city_x;
					    previous_y = saved_city_y;
 #if TT_DEBUG_ON
						 if (tt_debug_mode == 190404) {
							 tt_error_file() << "Set previous_x and previous_y to saved_city_x and saved_city_y: " 
								              << previous_x << ", " << previous_y << " on frame " << tt_frame_number << endl;
						 };
#endif
					    status_when_timer_finished = HOUSE_DESTROYED;
				    };
				    programmer_at_floor()->set_bombed();
                programmer_at_floor()->release_items_held();
				    if (explosion_time > tt_current_time) {
					   // bomb will hurry things up if need be
					   // this makes sure it explodes before timer finished
					   set_timer(explosion_time-tt_current_time+1000);
				    } else {
					   set_timer(0); // is this ok?
				    };
			   };
            if (martian_active() && tt_system_mode != PUZZLE) {
				   tt_martian->hide_balloon();
					tt_martian->set_active(FALSE);
               tt_martian->animate_to_size_percent(150,timer/2);
				   tt_martian->animate_location(8*tile_width,21L*tile_height,timer/2);
				   tt_martian->do_after(timer/2,tt_martian,SET_ACTIVE_TRUE_CALLBACK);
            };
			} else {
            status_when_timer_finished = LEAVING_GROUND;
//            remove_stuff_on_ground(); // obsolete on 060699
            if (martian_active()) {
				   tt_martian->teleport_out();
            };
         };
			x_speed = 0;
			y_speed = -2L*ideal_screen_height; // quickly get offscreen
//  good place to shrink thought bubble down, etc.
//			if (screen->inside_rule_body()) {
//				programmer_at_floor()->
			set_next_status(PROGRAMMER_RUNNING_TIMER);
			break;
		case LEAVING_GROUND:
			if (tt_log_version_number >= 63) {
				// new on 121204 since now keep saved_x in ground coordinates
				x = saved_city_x+(city_coordinate) ((saved_x*(int64) person_on_ground_width)/ideal_screen_width); 
				y = saved_city_y+(city_coordinate) ((saved_y*(int64) person_on_ground_height)/ideal_screen_height);
			} else {
				x = saved_city_x+saved_x;
				y = saved_city_y+saved_x;
			};
		case LEAVING_FLOOR:
			if (screen->inside_rule_body()) {
				if ((tt_system_mode != PUZZLE || is_toolbox_part_of_puzzle()) &&
                !tt_toolbox->now_offstage() && // haven't finished leaving yet
					 !tt_toolbox->gone()) { // added 061100
					status_when_timer_finished = next_status;
               set_next_status(PROGRAMMER_RUNNING_TIMER);
					set_timer(250);
					break;
				};
				if (tt_toolbox != NULL && !tt_toolbox->gone()) { // second condition added 030300
					tt_toolbox->remove_from_floor(FALSE);
				};
				Floor *floor = room->pointer_to_floor();
				Programmer_At_Floor *the_programmer_at_floor = programmer_at_floor();
				the_programmer_at_floor->fix_before_leaving_thought(screen);
				// so leaders are cleaned up
//	         pointer_to_toolbox_notebook()->go_to_last_blank_page();
            tt_martian->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
            screen->new_view(CAMERA_MOSTLY_ABOVE);  // isn't changing (swapped with following on 240100)
				screen->switch_to(floor,FALSE);
				if (tt_system_mode != PUZZLE) tt_martian->change_background(floor);
//				tt_toolbox->recursively_set_background(floor);
#if TT_DEBUG_ON
				if (tt_debug_mode == 11 && tt_events != NULL) {
					 tt_events->debug();
					 tt_error_file() << endl;
				};
#endif
//				state->cleanup(screen);
//				delete state;
//				state = saved_state;
//				saved_state = NULL;
//				screen->add(pointer_to_appearance());
				the_programmer_at_floor->returning_from_body(floor);
				pointer_to_appearance()->do_after(250,NULL,RESTORE_TOOLBOX_CALLBACK); 
				// second arg was floor prior to 131102 -- and was pointer_to_appearance() prior to 070103
				set_timer(750);
				x_speed = 0;
				y_speed = 4*tile_height;
				set_next_status(PROGRAMMER_RUNNING_TIMER);
				status_when_timer_finished = PROGRAMMER_NORMAL; //AT_FLOOR;
				HandState hand = the_programmer_at_floor->return_hand_state();
				if (hand == ABOUT_TO_RELEASE_ITEM) { // ignore the wait
					the_programmer_at_floor->set_release_time(tt_current_time);
            } else if (hand == ABOUT_TO_HOLD_ITEM) {
               // what to do here?? -- do I care?
				};
            tool_in_hand = NULL;
				saved_hand = EMPTY_HAND;
				left_thought_bubble_help();
				user_did(GLOBAL_HISTORY,USER_HAS_LEFT_THOUGHT_BUBBLE,pointer_to_appearance());
				break;
			} else {
            if (tt_toolbox != NULL && !tt_toolbox->gone()) { // second condition added 030300
					tt_toolbox->finish_instantly(); // new on 010505
					tt_toolbox->remove_from_floor(FALSE);
				};
				// save state of hand
				tool_in_hand = programmer_at_floor()->hand_state(saved_hand); // since leaving
            if (tool_in_hand != NULL) {
               tool_in_hand->remove_from_floor(FALSE);
               tool_in_hand->turn_off(pointer_to_appearance());
            };
				// should show getting up
//				set_timer(100); // brief pause
//				room->move_chair(FALSE,timer); // restore chair
//				x = 10*tile_width;
//				y = 6*tile_height;

//				x_speed = -6*tile_width;  // head towards center
//				y_speed = -3*tile_height;
//				set_timer(500);
//				x_speed = 0;
//				y_speed = -10*tile_height;
//				set_next_status(PROGRAMMER_RUNNING_TIMER);
//				status_when_timer_finished = STANDING_UP;
            if (old_next_status == LEAVING_GROUND) {
               set_next_status(STANDING_UP_OUTSIDE);
            } else {
				   set_next_status(STANDING_UP);
               x = saved_x;
               y = saved_y;
            };
				user_did(GLOBAL_HISTORY,USER_HAS_LEFT_FLOOR,pointer_to_appearance());
//				if (tt_martian != NULL) {
//					room->pointer_to_floor(screen)->remove_item(tt_martian);
//				};
			};
			// drop down and actually become room_walking
		case ENTERING_ROOM:
			if (next_status == ENTERING_ROOM) {
				// didn't just fall here from above
//				x = 4*tile_width;
//				y = 2*tile_height;
				saved_city_x = previous_x;
				saved_city_y = previous_y;
#if TT_DEBUG_ON
				if (tt_debug_mode == 190404) {
					tt_error_file() << "ENTERING_ROOM on frame " << tt_frame_number 
						             << " segment is " << tt_current_log_segment << endl;
					tt_error_file() << "saved_city_x=" << saved_city_x << " ;saved_city_y=" << saved_city_y << endl;
				};
#endif
				city_scale ignore_scale; // not used just now
				screen->viewing_region(city_view_x,city_view_y,ignore_scale);
				room->set_programmer(this);
				set_timer(1000); 
				// automatically walk into room so hard to leave accidentally
				x = tile_width;
				y = 3*tile_height;
				x_speed = 6*tile_width;
				y_speed = -tile_height;
				set_next_status(PROGRAMMER_RUNNING_TIMER);
				status_when_timer_finished = PROGRAMMER_NORMAL;
            // set the following to the corner in case first time in house (puzzle or truck built)
            just_set_sit_corner(left_room_margin,0);
//				if (martian_active()) {
//					tt_martian->teleport_in();
//				};
			};
/*
			if (next_status == ENTERING_ROOM) {
				// didn't just fall here from above
				set_timer(1000); // was 100
				x = tile_width;
				y = 2*tile_height;
				x_speed = tile_width;
				y_speed = -tile_height;
				set_next_status(PROGRAMMER_RUNNING_TIMER);
				status_when_timer_finished = PROGRAMMER_NORMAL;
				city_scale scale; // not used just now
				screen->viewing_region(city_view_x,city_view_y,scale);
				// and save state for when leaving the room
				saved_city_x = previous_x;
				saved_city_y = previous_y;
				room->set_programmer(this);
			};
*/
//       if (tt_sound_option&MUSIC_BIT) {
//          play_MIDI("sounds\\reggae2.mid",TRUE);
//       };
			 // and then continue with the following
//		case STANDING_UP:
//		case ROOM_WALKING: {
//			play_sound(WALKING_SOUND,1,TRUE);
        {
         Background *ground = NULL;
         screen->new_view(CAMERA_IN_FRONT);
			screen->save_floor_priorities(); // new on 300803 to save these now before they get clobbered by switch_to
			if (old_next_status == LEAVING_GROUND) {
            ground = pointer_to_appearance()->pointer_to_background();
            add_stuff_on_ground_to_city();
			   tt_martian->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
				// no time to fade out
            tt_martian->remove_from_floor(FALSE,TRUE);
				// following added on 070200 to fix user coordinates when standing
				city_coordinate floor_x_unit = ideal_screen_width/1000;
				city_coordinate floor_y_unit = ideal_screen_height/1000;
				translate_floor_to_room_dimensions(floor_x_unit,floor_y_unit);
				tt_screen->set_city_to_user_units(floor_x_unit,floor_y_unit);
            screen->switch_to(tt_city,TRUE,TRUE,TRUE);
         } else {
            screen->switch_to(room,TRUE,TRUE,TRUE);
//            if (tt_log_version_number > 9)
            move_stuff_on_floor_to_room(room);
            if (tt_system_mode != PUZZLE && tt_martian != NULL) { // second condition added 201003 for robustness
				   tt_martian->change_background(room);
			   };
         };
			state->cleanup(screen,next_status);
			delete state;
			state = NULL; // added 061099 in case some of the following triggers error
         if (ground != NULL) {
            ground->destroy();
         };
			boolean entering = (next_status == PROGRAMMER_RUNNING_TIMER);
         Sprite *tool_in_hand = return_tool_in_hand();
         if (old_next_status == LEAVING_GROUND) {
             // following are order senstitive since city walking programmer reads screen region
             screen->update_viewing_region(x,y,ground_scale);
//             screen->set_city_to_user_units(person_on_ground_width/1000,person_on_ground_height/1000);
             state = new Programmer_City_Walking(x,y,NORTH);
             state->finish_initializing();
             short int sit_state;
             if (tool_in_hand == NULL) {
                sit_state = MAN_STAND_UP;
             } else {
                sit_state = MAN_STAND_AND_POCKET;
             };
             state->swap_appearance(sit_sprite(),sit_state);
             //new Programmer_Sprite(sit_sprite(),x,y,this,city,sit_state));
             set_timer(pointer_to_appearance()->total_cycle_duration());
				 x_speed = 0;
				 y_speed = 0;
				 set_next_status(PROGRAMMER_RUNNING_TIMER);
				 status_when_timer_finished = STOOD_UP_FROM_GROUND;
         } else {
             state = new Programmer_Room_Walking(room,x,y,entering,EAST);
             state->finish_initializing();
//             screen->set_city_to_user_units(ideal_screen_width/1000,ideal_screen_height/1000);
             screen->update_viewing_region(10*tile_width,10*tile_height,ground_scale);
         };
			flush_input_events();
			screen->add(pointer_to_appearance());
         if (old_next_status != LEAVING_GROUND) {
			   room->enter();
            if (tt_toolbox != NULL && !tt_toolbox->gone()) { // second condition added 030300
					// not too early
				   room->pointer_to_floor()->remove_item(tt_toolbox,FALSE);
			   };
         };
         if (inside_rocket()) {
            tt_finish_all_animations = FALSE; // for animations generated below
            if (tt_martian->pointer_to_background() != room) {
               room->add_item(tt_martian);
            };
            tt_martian->set_priority(min_long+1); // in front of everything - except time time travel buttons (240100)
            screen->add(tt_martian);
            tt_martian->set_active(TRUE);
            tt_martian->move_to_favorite_spot();
				if (tt_martian->still_talking()) { 
					// new on 040600 - typically left while Marty was talking and returned before he finished
					tt_martian->remove_text(FALSE);
					// switched which was commented out on 080600 since the following caused Marty to sometimes say things twice
//					tt_martian->speech_has_finished(tt_martian->current_say_id());
				};
            tt_martian->teleport_in_done(); // except it didn't take any time
         } else if (entering && tt_martian_automatically_appears && tt_system_mode != PUZZLE
#if TT_CALL_FOR_MARTY
							&& tt_call_for_marty == NULL // new on 040805 so don't teleport in if already waiting for F1
#endif
				) {
//				if (tt_martian == NULL) {	// first time
//					tt_martian = new Martian(room,FALSE);
//				};
				status_when_timer_finished = TELEPORT_MARTIAN_IN;
			};
			if (next_status == STANDING_UP) {
				set_timer(pointer_to_appearance()->total_cycle_duration());
				x_speed = 0;
				y_speed = 0;
				set_next_status(PROGRAMMER_RUNNING_TIMER);
				status_when_timer_finished = STOOD_UP;
				if (martian_active() && tt_system_mode != PUZZLE) {
					tt_martian->set_size_percent(10);
					tt_martian->move_to(x+2*tile_width,y+3*tile_height);
				};
			};
        };
			break;
		case TELEPORT_MARTIAN_IN: 
         if (tt_system_mode != PUZZLE && tt_martian != NULL) { 
				// shouldn't really have gotten here if doing puzzles -- second condition added 201003 for robustness
            tt_martian->teleport_in_after(400);
         };
			break;
      case STOOD_UP_FROM_GROUND:
         ((Programmer_Room_Walking *) state)->finished_standing_up(tt_city);
         pointer_to_appearance()->set_size_percent(50);
			set_next_status(PROGRAMMER_NORMAL);
         break;
		case STOOD_UP:
			((Programmer_Room_Walking *) state)->finished_standing_up(room);
			set_next_status(PROGRAMMER_NORMAL);
         if (martian_active() && tt_system_mode != PUZZLE) {
				tt_martian->animate_to_favorite_spot_and_size(1200);
			};
			break;
		case LEAVING_ROOM:
			room->set_programmer(NULL);
         if (tt_system_mode == PUZZLE) { 
            player_leaving_room(room);
#if TT_TEXT_TO_SPEECH
            turn_on_sound_and_speech_off();
#endif
			} else if (martian_active()) {
				tt_martian->teleport_out();
				tt_martian->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
				// no time to fade out
			};
//         time_left_last_house = tt_current_time;
			// and the following
		case HOUSE_DESTROYED:  {
         tt_main_window->close_MCI();
			boolean house_destroyed = (next_status == HOUSE_DESTROYED);
			if (tt_system_mode != PUZZLE) tt_martian->set_background(NULL,FALSE,FALSE); 
			// FALSE args added on 170201 so I don't get a warning
//			if (tt_toolbox != NULL) tt_toolbox->recursively_set_background(NULL);
			if (next_status == LEAVING_ROOM) {
				room->pointer_to_house()->door_sound(); 
			}; // explosion handled by house itself
  			room = NULL;
			state->cleanup(screen,next_status);
			delete state;
         state = NULL;
			if (about_to_quit()) { // new on 050401 since might have shown tt_puzzles_over_html_file
				return(TRUE);
			};
			flush_input_events();
//       screen->flush_all();
			x = saved_city_x;
			y = saved_city_y;
#if TT_DEBUG_ON
			if (tt_debug_mode == 190404) {
				tt_error_file() << "Set x and y to saved_city_x and saved_city_y: " << x << ", " << y 
					             << " on frame " << tt_frame_number << endl;
			};
#endif
			set_timer(1000);
			x_speed = 0;
			y_speed = -4*tile_height;
			screen->update_viewing_region(city_view_x,city_view_y,ground_scale);
//         screen->set_city_to_user_units(person_on_ground_width/1000,person_on_ground_height/1000);
			state = new Programmer_City_Walking(x,y,SOUTH);
         // following line used to be just after deleting old state and 
			// switch_to called update_display which expected programmer state to be OK
         screen->new_view(CAMERA_IN_FRONT);
			screen->switch_to(tt_city,TRUE,!house_destroyed);
         set_next_status(PROGRAMMER_RUNNING_TIMER);
			if (house_destroyed) {
				status_when_timer_finished = DIZZY;
#if TT_DIRECT_INPUT
				play_force_feedback_effect("Explode",FALSE);
#endif
			} else {
				status_when_timer_finished = PROGRAMMER_NORMAL;
			};
// doesn't seem to have any effect
//         pointer_to_appearance()->set_priority(min_long); 
			// so first frame outside doesn't have the house half obscure programmer
//			screen->add(pointer_to_appearance());
//         if (tt_toolbox != NULL) screen->add(tt_toolbox);
//         state->add_toolbox();
         state->finish_initializing();
//			empty_helicopter->update_display();
			if (empty_helicopter != NULL) screen->add(empty_helicopter);
//       if (tt_sound_option&MUSIC_BIT) {
//          play_MIDI("sounds\\minuet1.mid",TRUE);
//       };
//         tt_global_picture->update_house_decoration();
		  };
			break;
		 case DIZZY: {
			 SpriteCode dizzy_code;
			 switch (tt_programmer_head) {
				 case 0:
					 dizzy_code = DIZZY_PERSON_SPRITE;
					 break;
				 case 1:
					 dizzy_code = DIZZY_PERSON_WITH_HAIR_SPRITE;
					 break;
				 default: // should only be 2
					 dizzy_code = DIZZY_PERSON_WITH_HAT_SPRITE;
			 };
			 city_coordinate x,y;
			 pointer_to_appearance()->lower_left_corner(x,y);
			 state->swap_appearance(dizzy_code,0); //new Programmer_Sprite(dizzy_code,x,y,this));
			};
			 break;
		 case SPINNING:
			 set_timer(500); // 1/2 second minumum -- will spin til mouse moves
			 x_speed = 0;
          y_speed = 0; // spin in place
			 ((Programmer_Walking *) state)->set_spinning(TRUE);
			 set_next_status(PROGRAMMER_RUNNING_TIMER);
			 status_when_timer_finished = PROGRAMMER_NORMAL;
			 break;
       case SITTING_DOWN_OUTSIDE:
         {
          city_coordinate x,y;
          Sprite *appearance = pointer_to_appearance();
          appearance->lower_left_corner(x,y);
          set_saved_city_location(x,y);
//		    tt_screen->remove(appearance);
//		    appearance->destroy();
          short int sit_state;
          if (return_tool_in_hand() == NULL) {
             sit_state = MAN_SIT_DOWN;
          } else {
             sit_state = MAN_SIT_TAKE_OUT_OF_POCKET;
          };
//          appearance = new Programmer_Sprite(sit_sprite(),x,y,this,tt_city,sit_state);
          state->swap_appearance(sit_sprite(),sit_state); //appearance);
//          appearance->set_size_percent(50);
//          tt_screen->add(appearance);
         }; // and the following
		 case SITTING_DOWN:
			 set_timer(pointer_to_appearance()->total_cycle_duration());
//			 set_timer(2000); // 2 seconds max
//			 pointer_to_appearance()->set_cycle_stopped_callback(reset_programmer_timer_callback);
			 x_speed = 0;
			 y_speed = 0;
          set_next_status(PROGRAMMER_RUNNING_TIMER);
          if (old_next_status == SITTING_DOWN_OUTSIDE) {
			     status_when_timer_finished = ON_GROUND;
              saved_x = 0; // will track deltas while on the ground
			     saved_y = 0;
          } else {
              status_when_timer_finished = AT_FLOOR;
              just_set_sit_corner(x,y);
          };
//			 if (martian_active() && tt_system_mode != PUZZLE) { // 121102 just got rid of the whole thing
////				 tt_martian->hide_balloon(); -- commented out on 121102 so it tracks the size change properly
//             tt_martian->remove_balloon(); // this way it'll be recreated with the right size
//				 tt_martian->animate_to_size_percent(20,timer);
//				 tt_martian->animate_location(x-tile_width,y+2*tile_height,timer);
//			 };
			 user_did(GLOBAL_HISTORY,USER_HAS_SAT,pointer_to_appearance());
			 break;
		 case AT_FLOOR:
          screen->new_view(CAMERA_MOSTLY_ABOVE);
//          if (tt_log_version_number > 9)
          move_stuff_in_room_to_floor(room);
          // and the following
       case ON_GROUND: {
//			stop_sound();
			Floor *floor;
			screen->new_view(CAMERA_MOSTLY_ABOVE); // moved here on 060699 so that priorities stop tracking lly earlier
         if (old_next_status == ON_GROUND) {
            floor = new Ground();
            TTRegion region(saved_city_x,saved_city_x+person_on_ground_width,  
                            saved_city_y,saved_city_y+person_on_ground_height);
            tt_city->transfer_pictures(floor,region);
            screen->update_viewing_region(10*tile_width,10*tile_height,ground_scale);
//            screen->set_city_to_user_units(ideal_screen_width/1000,ideal_screen_height/1000);
         } else {
            floor = room->pointer_to_floor();
            floor->set_containment_active(FALSE); // but typically overriden inside robot_in_control soon
         };
			// nothing animating and don't want toolbox to "act" too quick
			// added 200499 so coordinate and size remote controls work in a room
			tt_screen->set_city_to_user_units(ideal_screen_width/1000,ideal_screen_height/1000);
			screen->switch_to(floor,FALSE);
			if (tt_system_mode != PUZZLE && tt_martian != NULL) { // second condition added for robustness 201003
				tt_martian->change_background(floor);
				if (tt_martian->active()) { // && tt_system_mode != PUZZLE) {
               // following commented out on 121102 since can make balloon very big while sitting down
					//if (tt_good_sizes_adjust_to_screen_size) {
					//	tt_martian->set_size_percent(100); // was 150 prior to 150202
					//} else {
					//	tt_martian->set_size_percent(100); // was 150 prior to 150202
					//};
					// start off above middle
					tt_martian->move_to(10*tile_width,40L*tile_height);
					tt_martian->animate_to_favorite_spot_and_size(4000);
				};
//				tt_martian->set_active(TRUE);
			};
			// give priority to queued activity in this place
			if (tt_queue) tt_queue->move_on_screen_to_front();
			floor->restore_toolbox();
			flush_input_events();
//       screen->flush_all();
			const int duration_until_normal = 750;
			set_timer(duration_until_normal);
			x_speed = 0;
			y_speed = 12*tile_height;
			set_next_status(PROGRAMMER_RUNNING_TIMER);
         if (old_next_status == ON_GROUND && tt_martian_automatically_appears) {
            status_when_timer_finished = TELEPORT_MARTIAN_IN;
         } else {
			   status_when_timer_finished = PROGRAMMER_NORMAL;
         };
         state->cleanup(screen,next_status); // moved this from before if (tt_queue) ... above
			delete state;
			state = NULL; // added 061099 in case some of the following triggers error
			state = new Programmer_At_Floor(floor,9*tile_width,-9*tile_height,compute_hand_state(tool_in_hand,FALSE),tool_in_hand);
			state->finish_initializing();
			if (tool_in_hand != NULL) { // new on 060803 to get selection region updated properly
				tool_in_hand->do_after(duration_until_normal+1,tool_in_hand,TELL_PROGRAMMER_AM_READY_CALLBACK);
			};
							  };
			break;
		case PROGRAMMER_RUNNING_TIMER: // taken care of below
			break;
//		case NEXT_TITLE_IF_SHOWING_TITLES: // new on 200800
//			if (state->kind_of() == PROGRAMMER_TITLES) {
//				ProgrammerStatus status = state->next_title_if_showing_titles(); // new on 200800 - if showing titles
//				if (status == PROGRAMMER_NORMAL) {
//					set_timer(5000); //  wait 5 seconds
//					set_next_status(PROGRAMMER_RUNNING_TIMER);
//					status_when_timer_finished = NEXT_TITLE_IF_SHOWING_TITLES;
//				} else {
//					set_next_status(status);
//				};
//			};
//			break;
		case PROGRAMMER_QUIT:
			return(TRUE);
		default:
			say_error(IDS_CONFUSED_ABOUT_PROGRAMMER);
			return(TRUE);
  };
  previous_x = x;
  previous_y = y;
  city_coordinate delta_x, delta_y; // in ideal screen units
  boolean new_user_input, extended_key;
  char mouse_button_status,joystick_button_status;
  millisecond duration;
  if (next_status == PROGRAMMER_RUNNING_TIMER) {
	  for (;;) {
		  boolean more = log_or_user_actions(new_user_input,
														 key,extended_key,
														 delta_x,delta_y,
														 mouse_button_status,joystick_button_status,duration,
														 tt_second_byte_of_double_byte_input);
		  if ((!extended_key && key == VK_ESCAPE) ||
				(extended_key && key == VK_F7)) {
//            all_mouse_buttons()) {
				set_timer(0); // don't wait if user is in a hurry
				tt_finish_all_animations = TRUE;
//				tt_toolbox->finish_instantly();
		  };
        if (!more) break;
	  };
	  millisecond elapsed_time = tt_millisecond_delta;
	  if (timer <= tt_millisecond_delta) { // last bit left
		  elapsed_time = timer;
		  set_timer(0);
		  set_next_status(status_when_timer_finished);
//		  stop_postponing_new_logs(); // new on 120100
//		  if (next_status == HOUSE_DESTROYED) { // be sure everything runs first
//			  tt_finish_all_animations = TRUE;
//		  };
		  flush_input_events();
	  } else {
		  timer -= tt_millisecond_delta;
	  };
#if TT_DEBUG_ON
	  if (tt_debug_mode == 101) {
		  Sprite *appearance = pointer_to_appearance();
		  tt_error_file() << elapsed_time << "ms in programmer's timer; frame=" 
						  << tt_frame_number << " x,y = " << x << "," << y;
		  if (appearance != NULL) {
			  tt_error_file() << " index=" << appearance->resource_index();
		  };
		  tt_error_file() << endl;
	  };
#endif
/*
	  if (set_timer(1)) { // last bit left)
		  elapsed_time = 1;
		  timer = 0;
		  next_status = status_when_timer_finished;
		  flush_input_events();
	  } else {
		  timer -= tt_millisecond_delta;
		  if (timer <= 0) {
			  // still some to move
			  elapsed_time = tt_millisecond_delta-timer+1;
			  timer = 1;
			  // leave a millisecond so last movement is tiny
			  // to avoid spurious collisions
		  };
	  };
*/
	  // ignore any user movements and substitute these
	  mouse_button_status = 0;
     joystick_button_status = 0;
	  unsigned char ignored_key = 0;
	  flag ignored_extended_key = FALSE;
#if TT_DEBUG_ON
	  if (tt_debug_mode == 180703) {
		  tt_error_file() << "x_speed=" << x_speed << "; y_speed=" << y_speed << "; elapsed_time=" << elapsed_time << endl;
	  };
#endif
	  state->react(TRUE,
						ignored_key, // ignore key stroke as well
						ignored_extended_key,
						(x_speed*elapsed_time)/1000, (y_speed*elapsed_time)/1000,
						0, TRUE, // ignore buttons
						0,
						TRUE);
//     pointer_to_appearance()->update_display();
  } else {
	  for (;;) {
		  boolean more = log_or_user_actions(new_user_input,
														 key,extended_key,
														 delta_x,delta_y,
														 mouse_button_status,
                                           joystick_button_status,
														 duration,tt_second_byte_of_double_byte_input);
		  if (mouse_button_status != 0) {  // save for sensor
			  if (tt_current_mouse_button == mouse_button_status || tt_mouse_up_simulated) {
				  tt_current_mouse_button |= BUTTON_STILL_DOWN;
			  } else {
				  tt_current_mouse_button = mouse_button_status;
			  };
//			  tt_mouse_up_simulated = FALSE; // might still be TRUE
		  } else if (tt_mouse_up_simulated) { // real mouse is still down
			  tt_current_mouse_button |= BUTTON_STILL_DOWN;
		  } else {
			  tt_current_mouse_button = 0;
		  };
#if TT_DEBUG_ON
		  if (tt_debug_mode == 1010 || tt_debug_mode == 131102) {
			  tt_error_file() << (int) tt_current_mouse_button 
									<< " mouse_button_status=" << (int) mouse_button_status
									<< " joystick_button_status=" << (int) joystick_button_status
									<< " mouse_up_simulated=" << (int) tt_mouse_up_simulated << endl;
		  };
		  if (tt_debug_mode == 101 || tt_debug_mode == 131102) {
			  Sprite *appearance = pointer_to_appearance();
			  tt_error_file() << " x,y = " << x << "," << y;
			  if (appearance != NULL) {
				  tt_error_file() << " index=" << appearance->resource_index();
			  };
			  tt_error_file() << endl;
		  };
        if (tt_debug_mode == 131102 && key != 0) {
           tt_error_file() << "'" << (char) key << "' at " << tt_current_time << " on frame " << tt_frame_number << endl;
        };
#endif
#if TT_DIRECT_INPUT
        if (joystick_button_status != 0) {
           button_status = joystick_button_status;
        } else
#endif
           button_status = mouse_button_status;
		  if (martian_active() && new_user_input) {
			  // might "consume" some of this so programmer doesn't see clicks
			  tt_martian->react(new_user_input,
									  key,extended_key,delta_x,delta_y,
									  button_status,duration);
		  };
		  if (extended_key) {
           if (key == VK_F8) {
              if (control_down()) { // control test new on 141099 
                 if (relative_mouse_mode() && !replaying()) { // new on 220900 to ignore these if replaying
                    // new on 100602 to ignore control-f8 if absolute mouse mode
                    tt_main_window->toggle_full_screen(); // no-op if running in a window from the start
                    tt_main_window->toggle_exclusive_mouse();
//						  tt_main_window->set_focus(); // new on 030702
                 };
              } else {
                 if (!did_user(USER_HAS_STOPPED_ALL) && !tt_city->stopped()) {
						  // second condition new on 110205 since otherwise Marty says wrong thing if city was saved stopped
					     user_did(GLOBAL_HISTORY,USER_HAS_STOPPED_ALL,pointer_to_appearance());
					     say(IDS_F8_PRESSED_HELP,5);
				     };
				     stop_all(!tt_city->stopped());
				     // restore screen may have been darkened
				     tt_screen->screen_dirty();
              };
              extended_key = FALSE;
			     key = 0;
           } else if (key == VK_F9) { // && tt_log_version_number > 14) { // moved here on 230499
				  tt_programmer->set_visibility(tt_programmer->invisible());
				  record_key(key,extended_key); // added 220499
				  extended_key = FALSE;
				  key = 0;
		     };
        };
        if (tt_autodemo != NO_AUTODEMO && !replaying()) {
           // user has interrupted demos so check idle time 
           if (new_user_input) {
              time_of_last_user_input = tt_current_time;
           } else {
              int idle_max = ini_int(AC(IDC_SWITCHES),AC(IDC_AUTO_DEMO_MAX_IDLE),FALSE,120); // 2 minute default
              if ((tt_current_time-time_of_last_user_input)/1000 > idle_max) {
                  return(TRUE); // should terminate this
              };                            
           };
        };
        if (enable_keyboard_accelerators(joystick_button_status,mouse_button_status,button_status,key,extended_key)) {
           keyboard_accelerators_enabled = TRUE;
//           button_status = 0; // button action turned into keyboard action
        };
		  set_next_status(state->react(new_user_input,
												 key,extended_key,delta_x,delta_y,
												 button_status,
												 button_status&BUTTON_STILL_DOWN,
												 duration,
												 FALSE));
        if (keyboard_accelerators_enabled) disable_keyboard_accelerators();
#if TT_DEBUG_ON
		  if (tt_debug_mode == 52 && (key || extended_key)) {
			  tt_error_file() << "Key " << (int) key << " extended " << extended_key << endl;
		  };
#endif
		  if (!more) break;
	  };
  };
//	  tt_error_file() << pointer_to_appearance()->resource_index() << " " 
//					  << pointer_to_appearance()->current_parameter() << " "
//					  << tt_frame_number << endl;
  switch (next_status) {
	  case PROGRAMMER_QUIT:
		  return(tt_exit_at_end_of_log);
	  default:
		  return(FALSE);
  };
}; 

void Programmer::load_deltas(InputStream *stream) { // was global prior to 061102
	saved_delta_xs = new city_coordinate[number_of_deltas];
	stream->read((string) saved_delta_xs,number_of_deltas*sizeof(city_coordinate));
	saved_delta_ys = new city_coordinate[number_of_deltas];
	stream->read((string) saved_delta_ys,number_of_deltas*sizeof(city_coordinate));
	saved_delta_times = new millisecond[number_of_deltas];
	stream->read((string) saved_delta_times,number_of_deltas*sizeof(millisecond));
};

void Programmer::dump_saved_deltas(output_stream &stream) { // new on 251000
	stream.write((char *) saved_delta_xs,number_of_deltas*sizeof(city_coordinate));
	stream.write((char *) saved_delta_ys,number_of_deltas*sizeof(city_coordinate));
	stream.write((char *) saved_delta_times,number_of_deltas*sizeof(millisecond));
};

boolean Programmer::inside_a_house() {
	Sprite *appearance = pointer_to_appearance();
	if (appearance == NULL) return(FALSE); // e.g. in titles
	return(appearance->inside_a_house());
};

void Programmer::activate_state() { // new on 140103
	if (state != NULL) {
		state->activate();
	}; // else warn??
};

#if TT_XML

xml_element *Programmer::xml(xml_document *document) { // new on 011102 - completely rewritten on 050103
   xml_element *programmer_element = create_xml_element(L"Programmer",document);
	if (room != NULL) { // is this reset to NULL when no longer valid? -- this is earlier than state->add_xml so floor exists when loading state
		xml_append_child(room->xml_path(document),programmer_element);
	};
	xml_element *state_element = create_xml_element(state->xml_tag(),document); 
	state->add_xml(state_element,document);
	xml_append_child(state_element,programmer_element); // moved here on 190804
	if (empty_helicopter == NULL) {
//		empty_helicopter->xml_create_and_append_element(programmer_element,document);
		xml_set_attribute(programmer_element,L"EmptyHelicopter",0); // easy to reconstruct
	};
	if (next_status != PROGRAMMER_NORMAL) {
		xml_set_attribute(programmer_element,L"NextStatusCode",(int) next_status);
	};
	if (status_when_timer_finished != PROGRAMMER_NORMAL) {
		xml_set_attribute(programmer_element,L"StatusCodeWhenTimerFinished",(int) status_when_timer_finished);
	};
	if (tool_in_hand != NULL) {
		tool_in_hand->add_xml(programmer_element,L"InHand",document); // added FALSE on 110703 so doesn't generate an include file
	};
	if (timer != 0) {
		xml_set_attribute(programmer_element,L"Timer",timer);
	};
	if (x_speed != 0) {
		xml_set_attribute(programmer_element,L"XSpeed",x_speed);
	};
	if (y_speed != 0) {
		xml_set_attribute(programmer_element,L"YSpeed",y_speed);
	};
	if (keyboard_accelerators_enabled) {
		xml_set_attribute(programmer_element,L"KeyboardAcceleratorsEnabled",1);
	};
	// maybe all this starting stuff not needed anymore
	if (starting_location_set) { 
		xml_set_attribute(programmer_element,L"StartingLocationSet",1);
	};
	if (starting_x != 0) {
		xml_set_attribute(programmer_element,L"StartingX",starting_x);
	};
	if (starting_y != 0) {
		xml_set_attribute(programmer_element,L"StartingY",starting_y);
	};
	if (starting_view_center_x != 0) {
		xml_set_attribute(programmer_element,L"StartingViewCenterX",starting_view_center_x);
	};
	if (starting_view_center_y != 0) {
		xml_set_attribute(programmer_element,L"StartingViewCenterY",starting_view_center_y);
	};
	xml_set_attribute(programmer_element,L"StartingStatusCode",starting_status);
	xml_set_attribute(programmer_element,L"StartingScale",starting_scale);			
	xml_set_attribute(programmer_element,L"StartingDirection",starting_direction);
	if (tt_titles_ended_on_frame > tt_frame_number) { // or should it be >= (asked 230804)
		xml_set_attribute(programmer_element,L"TitlesOver",0);
	};
	// not bothering with these since handled "directly" now
	//Robot *starting_robot; // added 051099 so when dumped while training can be restored
	//Floor *starting_floor;
	//Cubby *starting_cubby; 
	//Cubby *starting_original_cubby;
   // not supporting xml_after_titles so maybe shouldn't allow the saving in titles if this is non-NULL
   //xml_set_attribute(programmer_element,L"BlockX",block_x);
   //xml_set_attribute(programmer_element,L"BlockY",block_y);
   //xml_set_attribute(programmer_element,L"HouseNumber",index);
   xml_set_attribute(programmer_element,L"CityX",saved_city_x);
   xml_set_attribute(programmer_element,L"CityY",saved_city_y);
	if (empty_helicopter_x != 0) {
		xml_set_attribute(programmer_element,L"HelicopterX",empty_helicopter_x);
	};
	if (empty_helicopter_y != 0) {
		xml_set_attribute(programmer_element,L"HelicopterY",empty_helicopter_y);
	};
   xml_set_attribute(programmer_element,L"CityViewX",city_view_x);
   xml_set_attribute(programmer_element,L"CityViewY",city_view_y);
   xml_set_attribute(programmer_element,L"FloorWidthSittingRation",floor_width_sitting_ratio);
   xml_set_attribute(programmer_element,L"FloorHeightSittingRation",floor_height_sitting_ratio);
   xml_set_attribute(programmer_element,L"RoomRightLimit",llx_right_room_limit);
   xml_set_attribute(programmer_element,L"RoomTopLimit",lly_top_room_limit);
   xml_set_attribute(programmer_element,L"SittingX",sit_llx);
#if TT_DEBUG_ON
	if (sit_llx < -1000000) {
		log("sit_llx very negative");
	};
#endif
   xml_set_attribute(programmer_element,L"SittingY",sit_lly);
   xml_set_attribute(programmer_element,L"PreviousX",previous_x);
   xml_set_attribute(programmer_element,L"PreviousY",previous_y);
   xml_set_attribute(programmer_element,L"SavedX",saved_x);
   xml_set_attribute(programmer_element,L"SavedY",saved_y);
	xml_set_attribute(programmer_element,L"PersonOnGroundWidth",person_on_ground_width);
	xml_set_attribute(programmer_element,L"PersonOnGroundHeight",person_on_ground_height);
	if (invisible()) { // new on 090103
		xml_set_attribute(programmer_element,L"Invisible",1);
	};
	if (tt_events != NULL) { // new on 140603
      xml_element *events_element = create_xml_element(L"EventsSoFar",document);
		tt_events->xml(events_element,document);
		xml_append_child(events_element,programmer_element); // moved here on 190804
	};
	if (grasp_delta_x != 0 || grasp_delta_y != 0) { // new on 130604 -- grasp_delta_y test added on 121104
		xml_set_attribute(programmer_element,L"GraspDeltaX",grasp_delta_x);
		xml_set_attribute(programmer_element,L"GraspDeltaY",grasp_delta_y);
		xml_set_attribute(programmer_element,L"WidthAtTimeOfGrasp",width_at_time_of_grasp); // new on 121104
		xml_set_attribute(programmer_element,L"HeightAtTimeOfGrasp",height_at_time_of_grasp);		
	};
	// saved_hand, saved_geometry_and_misc, and file_format_version_number not saved since only used when restoring using old format 
	// time_of_last_user_input is just for auto demo mode
	return(programmer_element);
};

boolean Programmer::handle_xml_tag(Tag tag, xml_node *node) { // new on 051102
   switch (tag) {
      case IN_HAND_TAG:
         set_tool_in_hand(xml_get_sprite(node)); // was just an assignment prior to 191202
         return(TRUE);
		//case EMPTY_HELICOPTER_TAG:
		//	empty_helicopter = xml_get_sprite(node);
		//	return(TRUE);
		case HOUSE_LOCATION_TAG: {
			House *house = house_at_location(node);
			if (house == NULL) return(FALSE);
			room = house->pointer_to_room();
			return(room != NULL);
									  };
		case PROGRAMMER_AT_FLOOR_TAG: {
			Floor *floor;
			if (room != NULL) {
				floor = room->pointer_to_floor();
			} else {
				floor = NULL;
			};
			if (state != NULL) delete state; // new on 170103 -- restored on 100803 since need this to trigger titles_over - commented out on 190103
			state = new Programmer_At_Floor(floor);
			return(xml_entity(node,state));
												};
		case PROGRAMMER_ROOM_WALKING_TAG: 
			if (state != NULL) delete state; // new on 170103 -- restored on 100803 since need this to trigger titles_over - commented out on 190103
			state = new Programmer_Room_Walking(room);
			return(xml_entity(node,state));
		case PROGRAMMER_CITY_WALKING_TAG: 
			if (state != NULL) delete state; // new on 170103 -- restored on 100803 since need this to trigger titles_over - commented out on 190103
			state = new Programmer_City_Walking();
			return(xml_entity(node,state));
		case PROGRAMMER_CITY_LANDING_TAG:
			if (state != NULL) delete state; // new on 170103 -- restored on 100803 since need this to trigger titles_over - commented out on 190103
			state = new Programmer_City_Landing();
			return(xml_entity(node,state));
		case PROGRAMMER_CITY_FLYING_TAG: 
			if (state != NULL) delete state; // new on 170103 -- restored on 090803 since need this to trigger titles_over
			state = new Programmer_City_Flying();
			return(xml_entity(node,state));
		case EVENTS_SO_FAR_TAG: // new on 140603
			tt_events = xml_load_events(node,FALSE,NULL);
			return(TRUE);
      case NO_MORE_TAGS: {
			next_status = (ProgrammerStatus) xml_get_attribute_int(node,L"NextStatusCode",PROGRAMMER_NORMAL);
			status_when_timer_finished = 
				(ProgrammerStatus) xml_get_attribute_int(node,L"StatusCodeWhenTimerFinished",PROGRAMMER_NORMAL);
			timer = xml_get_attribute_int(node,L"Timer",0); // prior to 210404 was timer
			x_speed = xml_get_attribute_int(node,L"XSpeed",0); // prior to 210404 was x_speed
			y_speed = xml_get_attribute_int(node,L"YSpeed",0); // prior to 210404 was y_speed
			keyboard_accelerators_enabled = xml_get_attribute_int(node,L"KeyboardAcceleratorsEnabled",0); 
			// prior to 210404 was keyboard_accelerators_enabled
			// maybe all this starting stuff not needed anymore
			// or maybe only needed when loaded old format and continuing in new...
			starting_location_set = xml_get_attribute_int(node,L"StartingLocationSet",0); // prior to 210404 was starting_location_set
			starting_x = xml_get_attribute_int(node,L"StartingX",0); // prior to 210404 was starting_x
			starting_y = xml_get_attribute_int(node,L"StartingY",0); // prior to 210404 was starting_y
			starting_view_center_x = xml_get_attribute_int(node,L"StartingViewCenterX",0); 
			// prior to 210404 was starting_view_center_x
			starting_view_center_y = xml_get_attribute_int(node,L"StartingViewCenterY",0); 
			// prior to 210404 was starting_view_center_y
//			tt_titles_ended_on_frame = xml_get_attribute_int(node,L"TitlesOver",1);
			if (tt_titles_ended_on_frame == max_long && xml_get_attribute_int(node,L"TitlesOver",1)) { 
				// rewritten 230804 -- wasn't set yet
				tt_titles_ended_on_frame = tt_frame_number-1; // doesn't matter how long in the past
			};
         saved_hand = (HandState) xml_get_attribute_int(node,L"HandCode",0);
         saved_city_x = xml_get_attribute_int(node,L"CityX",0);
         saved_city_y = xml_get_attribute_int(node,L"CityY",0);
         empty_helicopter_x = xml_get_attribute_int(node,L"HelicopterX",0);
         empty_helicopter_y = xml_get_attribute_int(node,L"HelicopterY",0);
         city_view_x = xml_get_attribute_int(node,L"CityViewX",0);
         city_view_y = xml_get_attribute_int(node,L"CityViewY",0);
			if (tt_scrolling_ok) { // conditional new on 040204
				floor_width_sitting_ratio = xml_get_attribute_int(node,L"FloorWidthSittingRation",0);
				floor_height_sitting_ratio = xml_get_attribute_int(node,L"FloorHeightSittingRation",0);
				if (tt_system_mode != PUZZLE && floor_width_sitting_ratio == 1 && floor_height_sitting_ratio == 1) {
					// saved with scrolling off and now it is on so restore full scrolling floor -- 051006
					floor_width_sitting_ratio = 3;
					floor_height_sitting_ratio = 4;
				} else { // and override these only if not saved without scrolling
					llx_right_room_limit = xml_get_attribute_int(node,L"RoomRightLimit",0);
					lly_top_room_limit = xml_get_attribute_int(node,L"RoomTopLimit",0);
			   };
				sit_llx = xml_get_attribute_int(node,L"SittingX",0);
				sit_lly = xml_get_attribute_int(node,L"SittingY",0);
				// new on 090404 to fix the lack of containment region (but it is computable from sit_llx, etc.)
				state->set_containment_region(); // experiment 190404 - left
			};
         saved_x = xml_get_attribute_int(node,L"SavedX",0);
         saved_y = xml_get_attribute_int(node,L"SavedY",0);
			previous_x = xml_get_attribute_int(node,L"PreviousX",0);
			previous_y = xml_get_attribute_int(node,L"PreviousY",0);
#if TT_DEBUG_ON
			if (tt_debug_mode == 190404) {
				tt_error_file() << "Loaded programmer on frame " << tt_frame_number << " segment is " 
									 << tt_current_log_segment << endl;
				tt_error_file() << "saved_city_x=" << saved_city_x << " ;saved_city_y=" << saved_city_y << endl;
				tt_error_file() << "saved_x=" << saved_x << " ;saved_y=" << saved_y << endl;
				tt_error_file() << "previous_x=" << previous_x << " ;previous_y=" << previous_y << endl;
			};
#endif
			person_on_ground_width = xml_get_attribute_int(node,L"PersonOnGroundWidth",person_on_ground_width);
			person_on_ground_height = xml_get_attribute_int(node,L"PersonOnGroundHeight",person_on_ground_height);
			if (xml_get_attribute_int(node,L"EmptyHelicopter",1)) {
				add_empty_helicopter(FALSE);
			};
			invisible_flag = xml_get_attribute_int(node,L"Invisible",FALSE); // new on 090103 -- invisible only if explicitly set
			grasp_delta_x = xml_get_attribute_int(node,L"GraspDeltaX",0); // new on 130604
			grasp_delta_y = xml_get_attribute_int(node,L"GraspDeltaY",0);
			width_at_time_of_grasp = xml_get_attribute_int(node,L"WidthAtTimeOfGrasp",0);
			if (tool_in_hand != NULL && width_at_time_of_grasp == 0 && (grasp_delta_x != 0 || grasp_delta_y != 0)) {
				// for backward's compatibility fill this in if saved with a version prior to 3.125
				width_at_time_of_grasp = tool_in_hand->current_width();
				height_at_time_of_grasp = tool_in_hand->current_height();
			};
         return(TRUE);
                         };
      default:
         return(Entity::handle_xml_tag(tag,node));
   };
};

#endif

boolean Programmer::dump(output_stream &stream, boolean just_prepare, boolean , boolean) {
   if (just_prepare) return(FALSE);
	ProgrammerStatus status_after_restarting = PROGRAMMER_NORMAL;
	ProgrammerStatus start_status_regardless;
	if (tt_titles_ended_on_frame <= tt_frame_number) { // use current values - not starting ones
		city_coordinate x,y;
		Sprite *appearance = pointer_to_appearance();
		appearance->lower_left_corner(x,y);
		stream.write((character *) &x, sizeof(x));
		stream.write((character *) &y, sizeof(y)); // used in start_at_floor: and the like -
// following is obsolete as of 130100
//		x = appearance->return_old_x_offset(); 
//		y = appearance->return_old_y_offset();
//		stream.write((character *) &x, sizeof(x)); // new on 050100
//		stream.write((character *) &y, sizeof(y)); // new on 050100
		city_coordinate city_view_x,city_view_y;
		city_scale scale;
		// Oct 16 '98 discovered that saving a city before landing left this
		// uninitialized since it is (stupidly) dumped twice (scroll down a page)
		tt_screen->viewing_region(city_view_x,city_view_y,scale);
		stream.write((character *) &city_view_x, sizeof(city_view_x));
		stream.write((character *) &city_view_y, sizeof(city_view_y));
		stream.write((character *) &scale, sizeof(scale));
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299) {
			tt_error_file() << "Dumping programmer." << endl
//							<< "starting_x = " << x
//							<< " ;starting_y = " << y << endl
							<< " city_view_x = " << city_view_x
							<< " city_view_y = " << city_view_y
							<< " ;starting_scale = " << scale << endl;
		};
#endif
		stream.put((byte) state->return_current_direction());
		status_after_restarting = state->restart_status();
		stream.put((byte) status_after_restarting);
		start_status_regardless = status_after_restarting;
	} else {
		if (!starting_location_set) { // new on 060899 for demos starting from "scratch"
         tt_city->center(starting_x,starting_y);
			city_view_x = starting_x;
			city_view_y = starting_y;
			for (int i = 0; i < 5; i++) { // new on 171299 to indicate not set // 5 was 7 prior to
				stream.write((character *) &min_long, sizeof(min_long));
			};
      } else {
			stream.write((character *) &starting_x, sizeof(starting_x));
			stream.write((character *) &starting_y, sizeof(starting_y));
//			city_coordinate ignored_old_offset = 0; // new on 171299
//			stream.write((character *) &ignored_old_offset, sizeof(ignored_old_offset));
//			stream.write((character *) &ignored_old_offset, sizeof(ignored_old_offset));
			stream.write((character *) &starting_view_center_x, sizeof(starting_view_center_x));
			stream.write((character *) &starting_view_center_y, sizeof(starting_view_center_y));		
			stream.write((character *) &starting_scale, sizeof(starting_scale));
		};
		stream.put((byte) starting_direction);
		stream.put((byte) starting_status);
		start_status_regardless = starting_status;  // for following conditional
	};
	// moved here on 280300 from branch where titles were over - but needs to happen in both cases
//		boolean saved_tt_dumping_with_sharing_of_all_types = tt_dumping_with_sharing_of_all_types;
	if (start_status_regardless == START_IN_THOUGHT_BUBBLE) { // added on 041099 since need to dump more stuff
		Robot *robot = (Robot *) pointer_to_appearance();
		tt_dumping_with_sharing_of_all_types = TRUE;
		robot->dump(stream,just_prepare);
//		robot->dump_geometry(stream);
		robot->dump_working_set(stream);
		Background *floor = pointer_to_appearance()->pointer_to_background();
		floor->pointer_to_body_cubby()->dump(stream);
		floor->dump(stream);
		if (tt_events != NULL) {
			tt_events->dump(stream);
		};
		stream.put(END_OF_ROBOT_MARKER);
		// and dump the cubby that was given to the robot originally (the copy dumped above may have been changed)
		programmer_at_floor()->pointer_to_original_thought_bubble()->pointer_to_cubby()->dump(stream);
	} else if (start_status_regardless == START_AT_FLOOR) { // new on 150100
		if (kind_of() == PROGRAMMER_AT_FLOOR) { // condition new on 241000
			programmer_at_floor()->dump_deltas(stream);
		} else { // new on 241000
			dump_saved_deltas(stream);
		};
	};
	if (status_after_restarting == START_AT_FLOOR) {
		stream.put((byte) programmer_at_floor()->return_hand_state());
	} else {
      stream.put((byte) saved_hand);
	};
   Sprite *tool = pointer_to_tool_in_hand();
   if (tool != NULL) {
		tool->finish_instantly(); // new on 150100
      tool->dump(stream);
      tool->dump_geometry(stream);
   } else {
      stream.put(NOTHING_MARKER);
   };
	tt_dumping_with_sharing_of_all_types = FALSE; // postponed here on 101099 since tool might be in robot's working set
   block_number block_x = 0,block_y = 0;
   short int index = -1;
   Background *floor = NULL;
	if (pointer_to_appearance() != NULL) {
		floor = pointer_to_appearance()->pointer_to_background();
	};
	House *house = NULL;
   if (floor != NULL) { // floor was room before 6/5/98
      house = floor->pointer_to_house();
	} else if (room != NULL) { // new on 210799
		house = room->pointer_to_house();
	};
	if (house != NULL) {
		Block *block = house->block_im_on();
		block->location(block_x,block_y);
		index = block->lot_index(house);
   };
   stream.write((character *) &block_x, sizeof(block_x));
   stream.write((character *) &block_y, sizeof(block_y));
   stream.write((character *) &index, sizeof(index));
   stream.write((character *) &saved_city_x, sizeof(saved_city_x));
   stream.write((character *) &saved_city_y, sizeof(saved_city_y));
   stream.write((character *) &empty_helicopter_x, sizeof(empty_helicopter_x));
   stream.write((character *) &empty_helicopter_y, sizeof(empty_helicopter_y));
   stream.write((character *) &city_view_x, sizeof(city_view_x));
   stream.write((character *) &city_view_y, sizeof(city_view_y));
   stream.write((character *) &floor_width_sitting_ratio, sizeof(floor_width_sitting_ratio));
   stream.write((character *) &floor_height_sitting_ratio, sizeof(floor_height_sitting_ratio));
   stream.write((character *) &llx_right_room_limit, sizeof(llx_right_room_limit));
   stream.write((character *) &lly_top_room_limit, sizeof(lly_top_room_limit));
   stream.write((character *) &sit_llx, sizeof(sit_llx));
   stream.write((character *) &sit_lly, sizeof(sit_lly));
   stream.write((character *) &saved_x, sizeof(saved_x));
   stream.write((character *) &saved_y, sizeof(saved_y));
	if (tt_titles_ended_on_frame <= tt_frame_number) {
		Sprite *appearance = pointer_to_appearance();
		appearance->dump_geometry(stream); // new on 130100
	} else if (saved_geometry_and_misc != NULL) { // added 150300 since demos starting from saved cities were broken
		saved_geometry_and_misc->dump_geometry(stream);
   // following just didn't work when loading XML city and then making demo - should just re-use XML anyway...
	//} else if (starting_x != min_long && tt_city->xml_processing_pending()) { 
	//	// new on 031202 since expect some geometry here in this case (seems to happen if some stuff is XML and some not)
	//	// second condition new on 090103 since only need this when loading an XML city
	//	saved_geometry_and_misc = new Sprite(HELIOFLY);
	//	saved_geometry_and_misc->dump_geometry(stream);
	//	//saved_geometry_and_misc->destroy(); // new on 111202 -- commented out on 161202 since interferes with making demos - but what about saving cities now?
	//	//saved_geometry_and_misc = NULL;
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299) {
		tt_error_file() << "block x = " << (int) block_x
						<< " ;block y = " << (int) block_y
						<< " ;index = " << (int) index 
						<< " ;city_view_x =" << city_view_x 
						<< " ;city_view_y =" << city_view_y << endl;
	};
#endif
	return(TRUE);
};

void load_deltas(InputStream *stream);

void Programmer::load(InputStream *stream, int version_number, NaturalLanguage language) {
	file_format_version_number = version_number; // new on 050100
	stream->read((string) &starting_x, sizeof(starting_x));
	starting_location_set = (starting_x != min_long); // was TRUE prior to 171299
	stream->read((string) &starting_y, sizeof(starting_y));
	// following handled by dump_misc now
//	stream->read((string) &starting_old_x_offset, sizeof(starting_old_x_offset));
//	stream->read((string) &starting_old_y_offset, sizeof(starting_old_y_offset));
   stream->read((string) &starting_view_center_x, sizeof(starting_view_center_x));
   stream->read((string) &starting_view_center_y, sizeof(starting_view_center_y));
   if (starting_location_set) {
		stream->read((string) &starting_scale, sizeof(starting_scale));
	} else {
		// doesn't matter so long as starting_scale is left alone
		stream->ignore(sizeof(city_scale));
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299) {
		tt_error_file() << "Loading programmer." << endl
						<< "starting_x = " << starting_x
						<< " ;starting_y = " << starting_y << endl
						<< " starting_view_center_x = " << starting_view_center_x
						<< " starting_view_center_y = " << starting_view_center_y
						<< " ;starting_scale = " << starting_scale << endl;
	};
#endif
   starting_direction = (Direction) stream->get();
   starting_status = (ProgrammerStatus) stream->get();
	if (starting_status == START_IN_THOUGHT_BUBBLE) { // then robot was dumped as well
		starting_robot = (Robot *) load_item(stream,version_number,language); // had better be a robot
		starting_robot->set_sprite_type(ROBOT_IN_TRAINING);
		starting_robot->appear_working();
		starting_robot->set_selectable(FALSE);
//    starting_robot->set_programmer(this);
//		starting_robot->load_geometry(stream);
		starting_robot->load_working_set(stream,version_number,language);
		starting_cubby = (Cubby *) load_item(stream,version_number,language); // had better be a cubby
		if (starting_cubby == NULL) {
			log("Problems loading a city saved while a robot was being trained.");
		};
		starting_cubby->remove_from_floor();
		starting_robot->remove_from_floor();
		starting_floor = new Floor(THOUGHT_BUBBLE_BACKGROUND,room,starting_cubby,starting_robot); 
		starting_cubby->remove_from_floor(); // was just added above but will be added below too
		starting_floor->load(stream,version_number,language);
		starting_robot->remove_from_floor(FALSE);
		tt_events = load_events(stream,version_number);
		starting_original_cubby = (Cubby *) load_item(stream,version_number,language); 
	} else if (starting_status == START_AT_FLOOR && version_number >= first_version_to_save_number_colors) { // new on 150100
		load_deltas(stream);
	};
   saved_hand = (HandState) stream->get();
   tool_in_hand = load_item(stream,version_number,language);
   if (tool_in_hand != NULL) {
      tool_in_hand->load_geometry(stream,version_number);
   };
   old_block_number x,y; // changed to old_block_number on 161105
   stream->read((string) &x, sizeof(x));
   stream->read((string) &y, sizeof(y));
   short int index;
   stream->read((string) &index, sizeof(index));
   Block *block = tt_city->block_at(x,y);
   if (block != NULL && index >= 0) {
      House *house = block->house_at(index);
      if (house != NULL) room = house->pointer_to_room();
   };
   previous_x = starting_x;
   previous_y = starting_y;
   stream->read((string) &saved_city_x, sizeof(saved_city_x));
   stream->read((string) &saved_city_y, sizeof(saved_city_y));
   stream->read((string) &empty_helicopter_x, sizeof(empty_helicopter_x));
   stream->read((string) &empty_helicopter_y, sizeof(empty_helicopter_y));
   stream->read((string) &city_view_x, sizeof(city_view_x));
   stream->read((string) &city_view_y, sizeof(city_view_y));
   stream->read((string) &floor_width_sitting_ratio, sizeof(floor_width_sitting_ratio));
   stream->read((string) &floor_height_sitting_ratio, sizeof(floor_height_sitting_ratio));
   stream->read((string) &llx_right_room_limit, sizeof(llx_right_room_limit));
   stream->read((string) &lly_top_room_limit, sizeof(lly_top_room_limit));
   stream->read((string) &sit_llx, sizeof(sit_llx));
   stream->read((string) &sit_lly, sizeof(sit_lly));
   stream->read((string) &saved_x, sizeof(saved_x));
   stream->read((string) &saved_y, sizeof(saved_y));
	if (starting_location_set && version_number >= first_version_to_save_number_colors && stream != NULL) {
		if (saved_geometry_and_misc == NULL) {
			saved_geometry_and_misc = new Sprite(HELIOFLY); // arg shouldn't really matter
		};
		saved_geometry_and_misc->load_geometry(stream,version_number,FALSE); // new on 130100
		// added FALSE arg on 150400 since this isn't a real load
	};
	if (tt_current_time > 0) { // new on 161299 - if not first log
		set_next_status(starting_status);
//		if (starting_status == START_FLYING || starting_status == START_LANDING) {
//			play_sound(HELICOPTER_SOUND,TRUE); // added 120100 since shouldn't stop sound if still flying
//		} else {
//			stop_sound();
//		};
	};
};

void Programmer::transfer_saved_gemometry_and_misc() { // new on 140100
	if (saved_geometry_and_misc == NULL) {
		return;
	};
   Sprite *appearance = pointer_to_appearance();
	appearance->transfer_geometry_and_misc(saved_geometry_and_misc); // new on 130100
	saved_geometry_and_misc->destroy(); // done with it
	saved_geometry_and_misc = NULL;
};

void Programmer::add_empty_helicopter(boolean screen_too) {
   empty_helicopter = new Sprite(HELIOLND,empty_helicopter_x,empty_helicopter_y,HELICOPTER);
	empty_helicopter->set_background(tt_city); // new on 100103 so will dump with geometry
   empty_helicopter->set_parameter(HELICOPTER_EMPTY);
   empty_helicopter->set_ok_to_dump(FALSE); // handled by programmer since might get 2 otherwise
   tt_city->add_extra(empty_helicopter,empty_helicopter_x,empty_helicopter_y);
   if (screen_too) tt_screen->add(empty_helicopter); //  || tt_log_version_number < 15
};

ProgrammerStatus Programmer::status_when_dumped() {
//#if TT_XML
//	if (xml_after_titles != NULL) { // new on 060103
//		return(START_AFTER_LOADING_XML);
//	};
//#endif
   return(starting_status);
};

void Programmer::titles_over() {
   if (tt_shutting_down) return;
	// commented out the following on 230803 since now read the log just before this --- restored on 280803
//	if (!replaying() && !time_travel_enabled()) { // new on 300104
//		tt_current_time = 0; // experiment 310104
//	} else
	if (tt_initial_current_time >= 0) { // && !tt_logging && !replaying()) { // new on 131202 -- 
		// !tt_logging && !replaying() new on 280104 to fix a timing bug in non-time travel demos -- not sure this helps...
		 tt_current_time = tt_initial_current_time;
		 if (tt_current_log_segment == tt_oldest_log_segment) { // condition new on 200304
			 tt_current_time_at_beginning_of_first_segment = tt_current_time; // new on 010304
		 };
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 40803) {
//		tt_debug_mode = 0; // experiment on 280104
		tt_error_file() << "Titles over." << endl;
	};
#endif
	tt_skip_titles = FALSE;
//	if (!starting_location_set) { // new on 171299
//		city->center(starting_view_center_x,starting_view_center_y);
//		starting_scale = 100;
//	};
	if (starting_location_set) { // condition new on 111202
		tt_screen->update_viewing_region(starting_view_center_x,starting_view_center_y,starting_scale);
	};
   // let robots start running
//   tt_darken_when_stopped = TRUE;
//   tt_stop_all = FALSE;
	stop_all(FALSE);
//	tt_city->set_stopped(FALSE); // new on 210803 since loading old format cities start running too soon otherwise
	tt_titles_ended_on_frame = tt_frame_number; // made global on 300104
//	city_load_from_log_frame = tt_frame_number+2;
	// commented out the following because logs now save such things better
//	tt_finish_all_animations = TRUE; // added 220999 so if loading a city you don't see Tooly sit down for example
	// but following doesn't seem to fix the problem??
	simulate_mouse_up(); // added 151099 so mouse click to get out of titles doesn't get used again
	// used to do lots here but this is in the middle of a cycle - updated 230803
	// this is all that remains
	if (tt_time_between_new_logs > 0 && tt_time_travel == TIME_TRAVEL_OFF) { // will be reset if a city is loaded -- made else on 190703
		tt_next_new_log_time = tt_current_time; // added on 240100 to start a log after titles are over
#if TT_DEBUG_ON
		if (tt_debug_mode == 290903) {
			tt_error_file() << "[2] Just set tt_next_new_log_time to tt_current_time to make it " << tt_next_new_log_time << endl;
		};
#endif
	};
	//if (tt_replay == REPLAY_IGNORE_TIMING_AFTER_TITLES) { // new on 300104
	//	tt_replay = REPLAY_IGNORE_TIMING;
	//} else if (tt_replay == REPLAY_REPRODUCE_TIMING_AFTER_TITLES) {
	//	tt_replay = REPLAY_REPRODUCE_TIMING;
	//};
	tt_real_time_pause_ended = timeGetTime(); // new on 200204
	tt_current_time_pause_ended = tt_current_time;
#if TT_DEBUG_ON
	if (tt_debug_mode == 20105) {
		tt_error_file() << "tt_current_time_pause_ended just set to " << tt_current_time_pause_ended << endl;
	};
#endif
	if (tt_dump_city_to_file_name_when_titles_over != NULL) { // new on 050805 to postpone this until here
		if (tt_city->dump_to_new_file(tt_dump_city_to_file_name_when_titles_over,TRUE,TRUE,FALSE)) { // don't zip it here since zipped below
			zip_file(tt_log_out_archive,tt_dump_city_to_file_name_when_titles_over); // this could be done later when "there is time"
		};
		delete [] tt_dump_city_to_file_name_when_titles_over;
		tt_dump_city_to_file_name_when_titles_over = NULL;
	};
};

void Programmer::stop_retraining(Robot *robot) {
	state->stop_retraining(robot);
};

void Programmer::just_set_sit_corner(city_coordinate x, city_coordinate y) {
   city_coordinate delta_x = x-sit_llx;
   city_coordinate delta_y = y-sit_lly;
   sit_llx = x;  // but "clipped" below if on the floor
   sit_lly = y;
   Sprite *appearance = pointer_to_appearance();
   if (appearance == NULL) return;
   Background *background = appearance->pointer_to_background();
   if (background == NULL) return;
   Floor *floor = background->pointer_to_floor();
   if (floor == NULL) return;
   if (floor->on_the_ground()) { 
      // if on the ground uses saved_x and y as cummulative deltas
		if (tt_log_version_number < 63) { 
			// commented out on 121204 since this aren't the right deltas -- done in Programmer::set_sit_corner now
			saved_x += delta_x;
			saved_y += delta_y;
		};
      return;
   };
   if (x < left_room_margin) {
      sit_llx = left_room_margin;
   } else if (x > llx_right_room_limit) {
      sit_llx = llx_right_room_limit;
   } else {
      sit_llx = x;
   };
   if (y < 0) {
      sit_lly = 0;
   } else if (y > lly_top_room_limit) {
      sit_lly = lly_top_room_limit;
   } else {
      sit_lly = y;
   };
   saved_x = sit_llx;  // set them again in case sit corner clipped
   saved_y = sit_lly;
   boolean at_left_wall = (x <= left_room_margin);
   boolean at_right_wall = (x >= llx_right_room_limit);
   boolean at_front_wall = (y >= lly_top_room_limit);
   floor->up_against_the_wall(at_left_wall,at_right_wall,at_front_wall);
};

boolean Programmer::enable_keyboard_accelerators(char joystick_button_status, char mouse_button_status,
                                                 char &button_status, unsigned char &key, boolean &extended_key) {
   if (state != NULL) {
      return(state->enable_keyboard_accelerators(joystick_button_status,mouse_button_status,button_status,key,extended_key));
   };
   return(FALSE);
};

void Programmer::disable_keyboard_accelerators() {
   keyboard_accelerators_enabled = (tt_mouse_button_count == 0);
};

void Programmer::set_visibility(boolean flag) {
	if (invisible_flag == !flag) return; // already knew it - stops infinite recursion
	invisible_flag = !flag; // new on 090103
	if (state != NULL) {
      state->set_visibility(flag);
   };
#if TT_DEBUG_ON
	if (tt_debug_mode == 20105) {
		tt_error_file() << "Programmer's visibility is " << (int) flag << " on frame " << tt_frame_number << endl;
	};
#endif
};

// noticed obsolete on 020105
//void Programmer::add_items_from_outside(Sprites *items, city_coordinate x, city_coordinate y) {
//	if (kind_of() == PROGRAMMER_AT_FLOOR) {
//		programmer_at_floor()->add_items_from_outside(items,x,y);
//	} else {
//		log("For now, you can't drag things to ToonTalk unless you are sitting down.");
//		// make noise - complain - or make it work somehow
//	};
//};

void Programmer::add_item_from_outside(Sprite *item, city_coordinate x, city_coordinate y) {
	if (kind_of() == PROGRAMMER_AT_FLOOR) {
		programmer_at_floor()->add_item_from_outside(item,x,y);
	} else {
//		log("For now, you can't drag things to ToonTalk unless you are sitting down.");
		tt_add_sprite_when_on_floor = item; // new on 020105
		if (tt_add_sprite_when_on_floor != NULL) {
			tt_add_sprite_when_on_floor->set_to_good_size(tt_toolbox); // new on 020105 (after releasing 3.135 and 47b)
		};
	};
};

//inline //commented out for now
boolean control_button_down() {
	return(button_status&CONTROL_KEY_DOWN);
};

//inline
//boolean shift_button_down() {
//	return(button_status&SHIFT_KEY_DOWN);
//};

//inline
boolean right_button(char button_status) {
  return(button_status&RIGHT_BUTTON);
};

//inline
boolean left_button(char button_status) {
  return(button_status&LEFT_BUTTON);
};

boolean middle_button(char button_status) {
  return(button_status&MIDDLE_BUTTON);
};

boolean button_modified() {
	return(control_button_down() || 
			 (tt_mouse_button_count == 3 && middle_button(button_status)));
};

char button_status = 0; // combined mouse and joystick

void Programmer::house_about_to_be_destroyed() {
	previous_x = saved_city_x;
	previous_y = saved_city_y;
#if TT_DEBUG_ON
	if (tt_debug_mode == 190404) {
		tt_error_file() << "house_about_to_be_destroyed: Set previous_x and previous_y to saved_city_x and saved_city_y: " << previous_x << ", " << previous_y << " on frame " << tt_frame_number << endl;
	};
#endif
	set_next_status(HOUSE_DESTROYED);
};

unsigned char Programmer::simulated_key() { // new on 030900
	if (state != NULL) {
		return(state->simulated_key());
	} else {
		return(0);
	};
};

boolean Programmer::center(city_coordinate &center_x, city_coordinate &center_y) {
	if (state == NULL) return(FALSE);
	state->center(center_x,center_y);
	return(TRUE);
};

boolean Programmer::lower_left_corner(city_coordinate &llx, city_coordinate &lly) { // new on 140901
	if (state == NULL) return(FALSE);
	state->lower_left_corner(llx,lly);
	return(TRUE);
};

city_coordinate Programmer::current_width() { // new on 140901
	if (state == NULL) return(0);
	return(state->current_width());
};

city_coordinate Programmer::current_height() { // new on 140901
	if (state == NULL) return(0);
	return(state->current_height());
};

void Programmer::tool_in_hand_is_ready(Sprite *tool) {
	if (state != NULL) {
		state->tool_in_hand_is_ready(tool);
	};
};

boolean Programmer::compute_selection_region(TTRegion &region) {
   if (state != NULL) {
      return(state->compute_selection_region(region));
   } else {
      return(FALSE);
   };
};

void Programmer::note_absolute_mode_deltas(coordinate &delta_x, coordinate &delta_y, boolean left_button_down, boolean left_button_just_went_down) {
   if (state != NULL) {
      state->note_absolute_mode_deltas(delta_x,delta_y,left_button_down,left_button_just_went_down);
   };
};

void Programmer::drop_on_floor() { // new on 200105
	state->drop(next_status); // may update state (e.g. drop copy of box on erased robot)
};

Programmer_State::~Programmer_State() {
//#if TT_FUNCTION_KEY_HELP
//	if (tt_function_key_help != NULL) { // new on 280205 to remove help picture if you get up or go outside or whatever
//		tt_function_key_help->remove_from_floor(FALSE);
//		tt_function_key_help->destroy();
//		tt_function_key_help = NULL;
//	};
//#endif
   if (appearance != NULL) {
	   // commented out the following on 051202 since it can call set_selection_feedback_callback and that refers to state (to get programmer_at_floor())
	   // this now happens in state::cleanup
	   //if (!tt_shutting_down) { // in case something used it for timer
		 // appearance->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
	   //};
	   appearance->destroy();
   };
};

void Programmer_State::swap_appearance(SpriteCode code, short int initial_parameter) { // Sprite *new_appearance) {
	int percent = appearance->get_size_percent();
   appearance->set_appearance_code(code);
   appearance->set_parameter(initial_parameter); // for the following to work properly
   appearance->update_display(0,0,UPDATE_INITIALIZE); // complete the initialization
   appearance->clear_changes();
   appearance->set_clean_status(DIRTY);
// used to destroy old and create new but that interferes with birds who are following a person with nest in pocket
//	tt_screen->remove(appearance);
//   appearance->finish_instantly(); // in case something used it for timer
//	appearance->destroy();
//	appearance = new_appearance;
	appearance->set_size_percent(percent);
//	tt_screen->add(appearance);
};

void Programmer_State::colliding_with(Sprite *other,
												  city_coordinate &delta_x,
												  city_coordinate &delta_y,
												  boolean &changed,
												  ProgrammerStatus &next_status) {
//	if (timer > 0) return; // on auto-pilot
	const SpriteType type = other->kind_of();
   if (kind_of() == PROGRAMMER_ROOM_WALKING && type != MARTIAN) return;
	TTRegion region,my_region;
	other->collision_region(region); 
	appearance->collision_region(my_region);
	switch (type) {
	  case TRUCK:
		 if (!other->outside_truck()) break; // new on 070699 so don't say ouch if hitting a truck from Tooly
		 play_sound(OUCH_SOUND);
#if TT_DIRECT_INPUT
       play_force_feedback_effect("Hit Truck",FALSE);
       changed = TRUE;
#endif
		 next_status = SPINNING;
		 // and the following
     case PICTURE:
       // the fact that collision is based upon rectangles becomes very apparent and confusing so
       // only rectangles are "solid"
       // user can add invisible rectangles to have fine control over anything
       // caller already checked that this holds
       if (tt_city->stopped()) break; // ignore picture collisions if the power is off
	  case HOUSE: // already checked that not also door
//		 if (!appearance->active_region()
//				 ->overlaps(((House_Sprite *) other)->door_collision_region())) { 
			 // hit just the house and not door
          bumped_into_house(move_back_out_of(region,my_region,delta_x,delta_y));
			 changed = TRUE;
//		 }; // else door will take care of it
		 break;
	  case HELICOPTER: {
		 city_coordinate helicopter_width = region.max_x-region.min_x;
		 // pick out second half of collision region
		 city_coordinate helicopter_min_x = region.min_x+helicopter_width/2;
		 city_coordinate my_center = (my_region.min_x+my_region.max_x)/2;
		 if (helicopter_min_x < my_center && my_center < region.max_x) {
			 next_status = ENTER_HELICOPTER;
		 };
		 move_back_out_of(region,my_region,delta_x,delta_y);
		 changed = TRUE;
#if TT_DIRECT_INPUT
       play_force_feedback_effect("Hit Helicopter",FALSE);
#endif
		}; 
		break;
	  case DOOR: { // open it 
        Door *door = (Door *) other;
        House *house = door->pointer_to_room()->pointer_to_house();
        if (tt_system_mode != PUZZLE || ok_to_enter_house(house)) {
			  house->door_sound();
			  door->open_door();
#if TT_DIRECT_INPUT
           play_force_feedback_effect("Open Door",FALSE);
#endif
		  } else {
           play_sound(BOOK_DROP_SOUND);
#if TT_DIRECT_INPUT
           play_force_feedback_effect("Wall North");
#endif
       };
		 changed = TRUE;
		 move_back_out_of(region,my_region,delta_x,delta_y);
		 break;
					 };
	  case OPEN_DOOR: // go in if close enough
		  if (region.min_y-my_region.max_y < tile_width //&& (
//#if TT_BETA
//            tt_running_old_demos ||
//#endif
//            (tt_current_time-time_left_last_house) > 4000 // and didn't just leave
           ) {
			  Room *room = ((Door *) other)->pointer_to_room();
// not too useful if robot hasn't run yet...
//			  millisecond explosion_time = room->when_will_house_explode();
//			  if (explosion_time <= 0) { // not about to explode
				  next_status = ENTERING_ROOM;
				  tt_programmer->set_room(room);
				  ((Door *) other)->open_door();
//			  };
		  };
		  move_back_out_of(region,my_region,delta_x,delta_y);
		  changed = TRUE;
		  break;
     case MARTIAN:
        if (tt_system_mode == PUZZLE) {
           colliding_with_marty();
        };
        break;
//	  case OPENING_DOOR:
//		 just_hit_door = TRUE;
//		 break;
/*
	  case FLOOR_CHAIR: // sit down and get to work
		  next_status = SITTING_DOWN;
		  break;
*/
//	  default:
//		  state->note_colliding_with(other,type);
	};
};

void Programmer_State::dampen_big_deltas(city_coordinate &delta_x,
													  city_coordinate &delta_y) {
#if TT_DEBUG_ON // was for Release version as well prior to 081004
   if (tt_debug_mode == 723) return; // a bit strange but OK for testing and strange IO devices?
#endif
#if TT_ABSOLUTE_MOUSE
	if (absolute_mouse_mode()) return; // new on 140901
#endif
	if (delta_x != 0) { // no need to bother if it didn't move
		const city_coordinate max_delta_x = (max_x_speed*tt_millisecond_delta)/1000;
		const city_coordinate min_delta_x = -max_delta_x;
		if (delta_x > max_delta_x) {
			delta_x = max_delta_x;
		} else if (delta_x < min_delta_x) {
			delta_x = min_delta_x;
		};
	};
	if (delta_y != 0) {
		const city_coordinate max_delta_y = (max_y_speed*tt_millisecond_delta)/1000;
		const city_coordinate min_delta_y = -max_delta_y;
		if (delta_y > max_delta_y) {
			delta_y = max_delta_y;
		} else if (delta_y < min_delta_y) {
			delta_y = min_delta_y;
		};
	};
};

void Programmer_State::cleanup(Screen *screen, ProgrammerStatus ) {
	if (appearance == NULL) return; // new on 051202
	if (!tt_resetting_or_destroying_city) { // in case something used it for timer -- moved here on 
		// tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 but can cause memory errors while resetting during time travel
		appearance->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
	};
	screen->remove(appearance);
};

void Programmer_State::display_appearance() {
	// do ordinary sprite thing by default
	if (!tt_programmer->invisible()) { // added conditional on 120499 so you can hide helicopter etc. with F9
		appearance->Sprite::display();
	};
};

boolean Programmer_State::save_appearance_regions() {
	// first condition added for robustness on 200105
	return(appearance != NULL && appearance->Sprite::save_region());
};

void Programmer_State::note_absolute_mode_deltas(coordinate &delta_x, coordinate &delta_y, boolean left_button_down, boolean left_button_just_went_down) {
   // new on 030502 - default but overridden on floor
   // use (and rename) tt_hand_needs_to_move_relatively??
   if (left_button_down) { // reset speed etc - delta_x and delta_y is the "goal" -- tried left_button_just_went_down
      city_coordinate center_x, center_y;
//      appearance->center_location(center_x,center_y);
      true_center(center_x,center_y); // rewritten on 060502 so that flying helicopter deals with scale issues correctly
      coordinate center_x_in_pixels = tt_screen->screen_x(center_x);
      coordinate center_y_in_pixels = tt_screen->screen_y(center_y);
      center_x_in_pixels = adjust_x_coordinate_for_window_size(center_x_in_pixels); // new on 170502 to deal with windows that are not 1X
      center_y_in_pixels = adjust_y_coordinate_for_window_size(center_y_in_pixels);
      coordinate x_distance_in_pixels = delta_x-center_x_in_pixels;
      coordinate y_distance_in_pixels = delta_y-center_y_in_pixels;
//      city_coordinate x_distance = tt_screen->x_pixel_to_screen(x_distance_in_pixels);
//      city_coordinate y_distance = tt_screen->y_pixel_to_screen(y_distance_in_pixels);
      int client_width = return_client_width(); // new on 170502 since window might not be 1-to-1 with screen coordinates
      int client_height = return_client_height();
      // replaced tt_screen_width with client_width etc below
      millisecond time_to_cover_x_distance = (abs(x_distance_in_pixels)*1000)/client_width; // 1 second to cross screen
      millisecond time_to_cover_y_distance = (abs(y_distance_in_pixels)*1000)/client_height;
      if (time_to_cover_x_distance == 0 && time_to_cover_y_distance == 0) { // clicked where it already is
         delta_x = 0;
         delta_y = 0;
         absolute_mouse_remaining_travel_time = 0;
      } else {
         if (time_to_cover_x_distance > time_to_cover_y_distance) {
            absolute_mouse_remaining_travel_time = time_to_cover_x_distance;
            if (x_distance_in_pixels > 0) {
               absolute_mouse_x_speed = tt_screen_width; // assuming takes 1 second to cross the screen
            } else {
               absolute_mouse_x_speed = -tt_screen_width;
            };
            absolute_mouse_y_speed = (y_distance_in_pixels*1000)/absolute_mouse_remaining_travel_time;
         } else {
            absolute_mouse_remaining_travel_time = time_to_cover_y_distance;
            if (y_distance_in_pixels > 0) {
               absolute_mouse_y_speed = tt_screen_height;
            } else {
               absolute_mouse_y_speed = -tt_screen_height;
            };
            absolute_mouse_x_speed = (x_distance_in_pixels*1000)/absolute_mouse_remaining_travel_time;
         };
         //absolute_mouse_x_speed = (absolute_mouse_x_speed*tt_screen_width)/client_width; // convert back to 
         //absolute_mouse_y_speed = (absolute_mouse_y_speed*tt_screen_height)/client_height;
#if TT_DEBUG_ON
         if (tt_debug_mode == 30502) {
            tt_error_file() << "distance_x = " << x_distance_in_pixels << ";distance_y = " << y_distance_in_pixels << endl;
            tt_error_file() << "absolute_mouse_remaining_travel_time = " << absolute_mouse_remaining_travel_time << "; " 
                        << "absolute_mouse_x_speed = " << absolute_mouse_x_speed << "; "
                        << "absolute_mouse_y_speed = " << absolute_mouse_y_speed << endl;
         };
#endif
      };
   };
//   if (!left_button_down) { // ignore deltas
      if (absolute_mouse_remaining_travel_time > 0) {
         millisecond delta_time = min(tt_millisecond_delta,absolute_mouse_remaining_travel_time);
         absolute_mouse_remaining_travel_time -= delta_time;
         delta_x = (absolute_mouse_x_speed*delta_time+((absolute_mouse_x_speed>0)?500:-500))/1000; // speed is in pixels per second and delta in milliseconds
         delta_y = (absolute_mouse_y_speed*delta_time+((absolute_mouse_y_speed>0)?500:-500))/1000; // added +500 for round off on 080502
#if TT_DEBUG_ON
         if (tt_debug_mode == 30502) {
            tt_error_file() << "delta_x and delta_y = " << delta_x << "," << delta_y << "; delta_t = " << tt_millisecond_delta << " on frame " << tt_frame_number << endl;
         };
#endif
      } else {
         delta_x = 0;
         delta_y = 0;
      };
//  };
};

void Programmer_State::true_center(city_coordinate &x, city_coordinate &y) {
   if (appearance != NULL) {
      appearance->true_center(x,y);
   } else { // warn??
      x = 0;
      y = 0;
   };
};

#if TT_XML

void Programmer_State::add_xml(xml_element *element, xml_document *document) {
	xml_set_attribute(element,L"MinX",min_x); // moved here on 090103
	xml_set_attribute(element,L"MaxX",max_x);
	xml_set_attribute(element,L"MinY",min_y);
	xml_set_attribute(element,L"MaxY",max_y);
	if (max_x_speed != 0) {
		xml_set_attribute(element,L"MaxXSpeed",max_x_speed);
	};
	if (max_y_speed != 0) {
		xml_set_attribute(element,L"MaxYSpeed",max_y_speed);
	};
	if (appearance != NULL) {
		appearance->add_xml(element,L"Appearance",document); // added FALSE on 110703 so doesn't generate an include file
	};
	if (keyboard_accelerators_enabled) {
		xml_set_attribute(element,L"KeyboardAcceleratorsEnabled",1);
	};
	if (current_direction != NO_DIRECTION) {
		xml_set_attribute(element,L"CurrentDirection",(int) current_direction);
	};
	if (absolute_mouse_x_speed != 0) {
		xml_set_attribute(element,L"AbsoluteMouseXSpeed",absolute_mouse_x_speed);
	};
	if (absolute_mouse_y_speed != 0) {
		xml_set_attribute(element,L"AbsoluteMouseYSpeed",absolute_mouse_y_speed);
	};
	if (absolute_mouse_remaining_travel_time != 0) {
		xml_set_attribute(element,L"AbsoluteMouseRemainingTravelTime",absolute_mouse_remaining_travel_time);
	};
	xml_append_child(tt_screen->xml(document),element);
};

boolean Programmer_State::handle_xml_tag(Tag tag, xml_node *node) {
	switch (tag) {
		case APPEARANCE_TAG:
			//if (appearance != NULL) { // new on 070103 -- shared in Programmer_At_Floor - leak otherwise??
			//	appearance->destroy();
			//};
			appearance = xml_get_sprite(node);
#if TT_DEBUG_ON
			if (tt_debug_mode == 190304 && appearance != NULL) {
				TTRegion region;
				appearance->true_region(region);
				tt_error_file() << "Loaded true region is ";
				region.print(tt_error_file());
				tt_error_file() << " on frame " << tt_frame_number << " during segment " << tt_current_log_segment << endl;
				appearance->update_display(); // experiment
				appearance->true_region(region);
				tt_error_file() << "Updated true region is ";
				region.print(tt_error_file());
				tt_error_file() << " on frame " << tt_frame_number << " during segment " << tt_current_log_segment << endl;
			};
#endif
			return(appearance != NULL);
		case SCREEN_TAG:
			return(tt_screen->handle_xml_tag(NO_MORE_TAGS,node));
		case NO_MORE_TAGS:
			min_x = xml_get_attribute_int(node,L"MinX",min_x); // moved here on 090103
			max_x = xml_get_attribute_int(node,L"MaxX",max_x);
			min_y = xml_get_attribute_int(node,L"MinY",min_y);
			max_y = xml_get_attribute_int(node,L"MaxY",max_y);
			max_x_speed = xml_get_attribute_int(node,L"MaxXSpeed",0); // prior to 210404 was max_x_speed
			max_y_speed = xml_get_attribute_int(node,L"MaxYSpeed",0); // prior to 210404 was max_y_speed
			keyboard_accelerators_enabled = xml_get_attribute_int(node,L"KeyboardAcceleratorsEnabled",0); 
			// prior to 210404 was keyboard_accelerators_enabled
			current_direction = (Direction) xml_get_attribute_int(node,L"CurrentDirection",NO_DIRECTION); 
			// prior to 210404 was (int) current_direction
			absolute_mouse_x_speed = xml_get_attribute_int(node,L"AbsoluteMouseXSpeed",0); 
			// prior to 210404 was absolute_mouse_x_speed
			absolute_mouse_y_speed = xml_get_attribute_int(node,L"AbsoluteMouseYSpeed",0); 
			// prior to 210404 was absolute_mouse_y_speed
			absolute_mouse_remaining_travel_time = xml_get_attribute_int(node,L"AbsoluteMouseRemainingTravelTime",0); 
			// prior to 210404 was absolute_mouse_remaining_travel_time
			return(TRUE);
		default:
			return(Entity::handle_xml_tag(tag,node));
	};
};

void Programmer_State::activate() { // new on 060103 -- called after loaded to connect things up
//	tt_screen->switch_to(pointer_to_background(),TRUE,FALSE); 
	// -- not clear where this should happen Programmer_At_Floor::activate does it now (070103)
	if (appearance != NULL) {
	   tt_screen->add(appearance);
   };
	if (tt_martian == NULL) { // new on 140103 since can happen while time travelling
		tt_martian = new Martian();
	};
};

#endif

Programmer_Titles_Flying::Programmer_Titles_Flying() :
  displayed(FALSE),
  displayed_expired_message(FALSE),
  text_to_speech_initialized(FALSE),
  expiration_warning_given(FALSE),
//  saved_martian_llx(min_long),
//  saved_martian_lly(min_long),
//  saved_martian_background(NULL),
  martian(NULL),
  saved_tt_help_speed_percent(-1) {
	appearance = NULL;
	background = new Titles_Background(OPENING_TITLE_BACKGROUND);
   tt_screen->update_viewing_region(half_screen_width,half_screen_height,ground_scale);
//   character_width = (4*character_width)/3; // extra since variable width
	background->set_text(C(IDS_TOONTALK_VERSION_NUMBER));
   background->set_text2(S(IDS_WELCOME_MESSAGE));
	if (tt_system_mode == PUZZLE) {
		tt_delay_between_titles = 10; // to read the story - was old default but re-organized on 170903
	} else if ((tt_log_out_file_name != NULL && !replaying()) || (tt_log_count_max != 0 && !time_travel_enabled())) { 
		// updated on 170903 (added roughly 070903 when condition was replaying())
//		next_title_if_after = timeGetTime()+500; // half a second so it doesn't flash too quickly
		// removed !tt_log_out_file_name_generated && on 170204
		tt_delay_between_titles = 1; // for the credits...
	};
	next_title_if_after = timeGetTime()+tt_delay_between_titles*1000; // new on 200800 - wait 10 seconds by default
};

Programmer_Titles_Flying::~Programmer_Titles_Flying() {
	background->destroy();
   tt_programmer->titles_over();
	//if (tt_martian != NULL) {
	//	if (saved_martian_llx != min_long) {
	//		tt_martian->move_to(saved_martian_llx,saved_martian_lly);
	//	};
	//	if (saved_martian_background != NULL) {
	//		tt_martian->set_background(saved_martian_background);
	//		tt_martian->set_active(TRUE);
	//	};
	//	tt_martian->set_speech_should_wait_for_first_display(TRUE); // new on 050600
	//};
	if (martian != NULL) {
		martian->destroy();
	};
};

unsigned char Programmer_Titles_Flying::simulated_key() { // new on 030900
	if (displayed && (millisecond) timeGetTime() > next_title_if_after) {
		// !replaying() added on 070900 (but it shouldn't be called with tt_replay anyway) -- removed on 050903
		return(' '); // any one will do
	} else {
		return(0);
	};
};

#if TT_TRIAL_EXPIRATION

enum Seller {ANIMATED_PROGRAMS=0,LOGOTRON,LOGOTRON_UK,EKELUNDS,CNOTINFOR,DIVERTIRE,MICRONET};
string purchase_dialogs[] =
{"HTMLPurchaseAnimatedPrograms", 
 "HTMLPurchaseLogotron", 
 "HTMLPurchaseLogotronUK", 
 "HTMLPurchaseEkelunds", 
 "HTMLPurchaseCnotinfor", 
 "HTMLPurchaseDivertire",
 "HTMLPurchaseMicronet"};

Seller seller_from_geoID(GEOID country_code) {
	switch (country_code) {
		case 0xDD:
			return(EKELUNDS); // Sweden
		case 0xC1:
			return(CNOTINFOR); // Portugal
		case 0x20:
			return(DIVERTIRE); // Brazil
		case 0x7A:
//		case 0x86: // Korean version never made
			return(MICRONET); // Japan
		case 0xF2:
			return(LOGOTRON_UK);
		case 0x108:
		case 0x107:
		case 0xD1:
		case 0xF0:
		case 0xEF:
		case 0xEA:
		case 0xE8:
		case 0x29:
		case 0x104:
		case 0x77:
		case 0xD8:
		case 0xD2:
		case 0xD5:
		case 0x157:
		case 0x72:
		case 0xDB:
		case 0xD0:
		case 0xCC:
		case 0xCE:
		case 0xAF:
		case 0x68:
		case 0xAD:
		case 0xF5:
		case 0xA8:
		case 0x9C:
		case 0xA0:
		case 0xA2:
		case 0x9D:
		case 0x95:
		case 0x9F:
		case 0x94:
		case 0x92:
		case 0x8E:
		case 0x32:
		case 0x81:
		case 0xC4:
		case 0x45:
		case 0x56:
		case 0x59:
		case 0x57:
		case 0x1E:
		case 0x43:
		case 0x4:
		case 0x3E:
		case 0x39:
		case 0x31:
		case 0x2B:
		case 0x37:
		case 0x13:
		case 0x1C:
		case 0x26:
		case 0x9:
		case 0xF1:
		case 0x2:
		case 0x8F:
		case 0xCB:
		case 0xC8:
		case 0xBF:
		case 0xB1:
		case 0xB0:
		case 0xA3:
		case 0x93:
		case 0x8D:
		case 0x91:
		case 0x8C:
		case 0x46:
		case 0x76:
		case 0x6D:
		case 0x62:
		case 0x54:
		case 0x4D:
		case 0x3D:
		case 0x5E:
		case 0x4B:
		case 0xD9:
		case 0xDF:
		case 0x1D:
		case 0x15:
		case 0xE:
		case 0x7:
		case 0x44:
		case 0xA7:
		case 0x71:
		case 0xD7:
		case 0xC:
		case 0xB7:
			return(LOGOTRON);
		default:
			return(ANIMATED_PROGRAMS);
	};
};

enum purchaseResponse {PURCHASED, PURCHASING, NOT_PURCHASED};

purchaseResponse purchase_dialog(string html_dialog_page=NULL);

purchaseResponse check_purchase_key(string response) {
	long key = strtol(response,NULL,16);
	time_t now = seconds_since_1970();
	if (key >= now-seconds_in_a_day && key-seconds_in_a_day*90 < now) {
		// updated on 190408 so that if sent a key and clock (or time zone?) is a few hours off still works
		// if less than 24 hours off
		// OK -- changed to 90 days on 140807 (note if changed also change thankyou.htm JavaScript) 
		return(PURCHASED);
	} else {
		return(purchase_dialog("HTMLPurchaseKeyInvalid"));
	};
};

purchaseResponse purchase_dialog(string html_dialog_page) {
	if (html_dialog_page == NULL) {
		GEOID geoID = GetUserGeoID(GEOCLASS_NATION);
		Seller seller = seller_from_geoID(geoID);
		html_dialog_page = purchase_dialogs[seller];
	};
	string response = show_html_dialog_named_in_ini_file(html_dialog_page);
	if (response != NULL) {
		if (response[1] == '\0') { // single digit response
			switch (response[0]) {
				case '0': // show location specific dialog
					return(purchase_dialog(NULL));
				case '1': // location incorrect
					return(purchase_dialog("HTMLPurchaseNoLocation"));
				case '2': // in Sweden so send email to Lennart
					// now is just like order.htm
					launch_help("lennart.htm");
					return(PURCHASING); // not yet
				case '3': // quit
					return(NOT_PURCHASED);
				case '4': // buy now
					launch_help("paypal.htm");
					return(PURCHASING); // not yet
				case '5': // try key again
					return(purchase_dialog("HTMLPurchaseAnimatedPrograms"));
				case '6': // visit Logotron
					launch_help("logotron.htm");
					return(PURCHASING); // not yet
				case '7': // visit Cnotinfor
					launch_help("cnotinfor.htm");
					return(PURCHASING); // not yet
				case '8': // visit Divertire
					launch_help("divertire.htm");
					return(PURCHASING); // not yet
				case '9': // visit Logotron reseller
					launch_help("logotron_worldwide.htm");
					return(PURCHASING); // not yet
				case 'a': // visit Micronet
					launch_help("micronet.htm");
					return(PURCHASING); // not yet
			};
		} else { // should be a purchase key
			return(check_purchase_key(response));
		};
	};
	return(NOT_PURCHASED);
};

#endif

ProgrammerStatus Programmer_Titles_Flying::react(boolean ,
																 unsigned char key,
																 boolean extended_key,
																 city_coordinate ,
																 city_coordinate ,
																 char button_status,
																 boolean ,
																 millisecond ,
																 boolean ) {
	if (!displayed) {
		tt_screen->switch_to(background,FALSE,FALSE);
		displayed = TRUE;
	} else {
      // do stuff now to off load some time spent starting or when first speaking
      if (tt_toolbox == NULL) { // && tt_system_mode != PUZZLE) {
         tt_toolbox = new Toolbox();
			tt_toolbox->initialize();
      };
#if TT_TEXT_TO_SPEECH
      if (text_to_speech() && !text_to_speech_initialized) {
         initialize_text_to_speech(); // this can take a couple seconds so do it while titles are displayed
         text_to_speech_initialized = TRUE;
      };
#endif
		compute_decibal_level();
   };
	if (key != 0 || button_status != 0) { // || (millisecond) timeGetTime() > next_title_if_after) {
		// commented out the next_title_if_after since it now (030900) generates a keystroke for logging purposes as well
		// using timeGetTime() rather than tt_current_time because a frame can take less than 1 millisecond when in titles
		next_title_if_after = timeGetTime()+tt_delay_between_titles*1000; // wait 10 seconds by default - new on 200800
//		// user pushed a mouse button or a key
//		tt_programmer->set_timer(0); // turn off any timers - new on 200800 since using timers to flip screens
      int background_id = background->return_background_index();
      short int publisher_background_id = PUBLISHER_LOGO_BACKGROUND;
#if !TT_SMALL_TRIAL
      if (!valid_image_id(publisher_background_id))  // older m22.nda
#endif
         publisher_background_id = THOUGHT_BUBBLE_BACKGROUND;
#if (TT_TRIAL_EXPIRATION||TT_TRIAL_VERSION)&&!TT_SMALL_TRIAL
      time_t now = seconds_since_1970();
      char buffer[80];
      LONG buffer_size;
      HKEY expires_key;
      long expires = 0;
      boolean key_open = FALSE;
      boolean expiration_changed = FALSE;
      if (RegOpenKeyG(HKEY_CLASSES_ROOT,BETA_GUID,&expires_key) == ERROR_SUCCESS) {
         key_open = TRUE;
         buffer_size = sizeof(buffer);
         if (RegQueryValueG(expires_key,"",buffer,&buffer_size) == ERROR_SUCCESS) {
            if (buffer_size > 0) {
               buffer[buffer_size] = '\0'; // terminate the string -- do I need to??
               expires = atol(buffer);
            };
         };
      };
#if TT_TTINI_ALTERNATIVE_TO_REGISTRY
		if (expires >= 0) {
			int new_expires = ini_int("Versions", "TT3", FALSE, 0);
			if	(new_expires != 0)	{
				expires = new_expires;
			};
		};
#endif
      if (!key_open) {
         if (RegCreateKeyG(HKEY_CLASSES_ROOT,BETA_GUID,&expires_key) == ERROR_SUCCESS) {
            key_open = TRUE;
            expires = now+7*seconds_in_a_day; // 7 days -- updated from 60 days on 290904 -- updated from 75 days on 220907
            expiration_changed = TRUE;
         } else {  // something wrong with key -- maybe it has been hacked or files copied rather than using installer
				// but if copied then the create call above should work fine (noted on 220907)
#if TT_TTINI_ALTERNATIVE_TO_REGISTRY
				// new on 190408
				// If user doesn't have admin rights to update registry then use toontalk.ini instead
				expires = ini_int("Versions", "TT3", FALSE, 0);
				if (expires == 0) {
					// no entry so make one
				   expires = now+7*seconds_in_a_day; // 7 days -- should be 75 if beta release
					write_ini_entry("Versions", "TT3", expires);
               expiration_changed = TRUE;
				};
#else
#if TT_POST3187
				// updated 050707
				tt_error_file() << "No key found so will expire in 7 days." << endl;
				expires = now+7*seconds_in_a_day;
				expiration_changed = TRUE;
#else			
				tt_error_file() << "Expires on April 1, 2006 instead." << endl; // updated 280904
            expires = (365L*36+9+31+28+31+30)*seconds_in_a_day;
#endif
#endif
         };
      } else if (expires < 0) { // do nothing
      } else if (expires < 1000) { // just installed -- number of days
         show_cursor(TRUE);
         character message_buffer[512];
         output_string_stream message(message_buffer,512);
         message << S(IDS_BETA_TO_EXPIRE2) << " " << expires << " " << S(IDS_BETA_TO_EXPIRE3);
         message.put((char) 0);
         message_box(MB_SYSTEMMODAL|MB_OK,message_buffer);
         show_cursor(FALSE);
         expires = now+expires*seconds_in_a_day;
         expiration_changed = TRUE;
      }; // else leave expiration alone
#elif TT_TRIAL_VERSION
		// trial version but not based on number of days but a particular date
		// following makes no sense -- noticed 280904
      long expires = (365L*32+8); // +31+28+31+30+31+30+31+31+30+31)*seconds_in_a_day; // Jan 1, 2002
		tt_expires = TRUE;
#endif
#if (TT_TRIAL_EXPIRATION||TT_TRIAL_VERSION)&&!TT_SMALL_TRAIL
      long warning = expires-(14L*seconds_in_a_day); // two weeks earlier
		tt_expires = (expires > 0); // new on 090807
      if (tt_debug_mode == 1970 && replaying()) { // ignore expiration date so old demos can be run
      } else if (expires > 0 && now > warning) { 
			if (now >= expires) {
//				tt_trouble_shooting = TRUE; // show following dialog regardless
				// following new on 050707
				pause_toontalk(FALSE,FALSE,FALSE);
				purchaseResponse purchased = purchase_dialog("HTMLTrialOver");
				if (purchased == PURCHASED) {
					expires = -1; // never
					expiration_changed = TRUE;
					expiration_warning_given = TRUE; // not really but don't want one
					unpause_toontalk();
				} else {
					 tt_shutting_down = TRUE;
					 tt_system_mode = FREE_PLAY; // so no hints are offered
					 return(PROGRAMMER_QUIT);
				};
 //           tt_exit_failure(TROUBLE_SHOOT_TRIAL_PERIOD_OVER); // following made it say it twice ,S(IDS_BETA_EXPIRED));
//            if (tt_debug_mode == 0) show_cursor(TRUE);
//				message_box(MB_SYSTEMMODAL|MB_OK,S(IDS_BETA_EXPIRED));
//				if (!displayed_expired_message) {
//					background->set_text(S(IDS_BETA_EXPIRED));
//					displayed_expired_message = TRUE;
//					return(PROGRAMMER_NORMAL);
//				} else {
//					return(PROGRAMMER_QUIT);
//				};
			} else if (!expiration_warning_given) {
				// new on 060707
				long days_left = (expires-now)/seconds_in_a_day;
				character parameters[512];
            output_string_stream params(parameters,512);
				params << "daysLeft;" << days_left;
				params.put((char) 0);
				pause_toontalk(FALSE,FALSE,FALSE);
				string response = show_html_dialog_named_in_ini_file("HTMLTrialDaysLeft",parameters);
				expiration_warning_given = TRUE;
				if (response != NULL) {
					if (response[1] == '\0') { // single digit response
						switch (response[0]) {
							case '1': // buy
								purchaseResponse purchased = purchase_dialog(NULL);
								if (purchased == PURCHASED) {
									expires = -1; // never
									expiration_changed = TRUE;
								} else if (purchased == PURCHASING) {
									tt_shutting_down = TRUE;
									tt_system_mode = FREE_PLAY; // so no hints are offered
									return(PROGRAMMER_QUIT); // force a restart after going to PayPal page
								};
								break;
						};
					} else if (check_purchase_key(response) == PURCHASED) {
						expires = -1; // never
						expiration_changed = TRUE;
					};
				};
				unpause_toontalk();
				// re-designed on 060707 to always give a warning dialog page
            //show_cursor(TRUE);
            //long days_left = (expires-now)/seconds_in_a_day;
            //character message_buffer[512];
            //output_string_stream message(message_buffer,512);
            //message << S(IDS_BETA_TO_EXPIRE1) << endl << S(IDS_BETA_TO_EXPIRE2) << " "
            //        << days_left << " " << S(IDS_BETA_TO_EXPIRE3);
            //message.put((char) 0);
            //message_box(MB_SYSTEMMODAL|MB_OK,message_buffer);
            //expiration_warning_given = TRUE;
            //show_cursor(FALSE);
            // following interferred with demos -- now fall thru to old behavior
//            return(initial_programmer_status());
			};
		};
		if (key_open) { // moved here on 060707 to accept user provided "keys"
         if (expiration_changed) {
            char expiration_string[33];
            ltoa(expires,expiration_string,10);
#if TT_TTINI_ALTERNATIVE_TO_REGISTRY
				boolean writeFailed = (RegSetValueG(expires_key,"",REG_SZ,expiration_string,0) != ERROR_SUCCESS);
				if (writeFailed) {
					// can't write to the registry
					write_ini_entry("Versions", "TT3", expires);
				};
#else
            RegSetValueG(expires_key,"",REG_SZ,expiration_string,0);
#endif
         };
         RegCloseKey(expires_key);
#if TT_TTINI_ALTERNATIVE_TO_REGISTRY
		} else if (expiration_changed) {
         write_ini_entry("Versions", "TT3", expires);
#endif
      };
#endif
#if TT_DEBUG_ON
		if (tt_debug_mode == 300104) { // just too much trouble since debug counters get off by 2 (Marty and his talk balloon)
			// was tt_debug_mode != 0 but this can itself cause problems (with demo timing/recording)
			key = VK_ESCAPE;
			extended_key = FALSE;
		};
#endif
		if (key == VK_ESCAPE && !extended_key) {
			// commented out on 300104 the following since it caused many problems since the log still contains logs of timer and keyboard etc.
			// || (key != 0 && tt_system_mode != PUZZLE && ((tt_logging && !tt_log_out_file_name_generated) || replaying()))) { // new on 170903 to not display credits if replaying
         if (background_id == publisher_background_id && !tt_subtitles_and_narration_even_without_demo) {
//             !replaying() && tt_log_out_file_name == NULL) {
            martian->teleport_out_done();
				martian->remove_from_floor(FALSE);
//            background->remove_item(tt_martian);
//            tt_help_speed_percent = (short int) (tt_help_speed_percent/2); // restore 
				if (saved_tt_help_speed_percent > 0) { // more reliable way of restoring the help speed - new on 110401
					tt_help_speed_percent = saved_tt_help_speed_percent;
				};
         };
			tt_titles_just_ended = TRUE; // new on 230803 to this happens at the right time and at the end of the cycle
         return(initial_programmer_status());
      };
      if (background_id == OPENING_TITLE_BACKGROUND) { // && !replaying()) {
         background->set_text(NULL);
         background->set_index(publisher_background_id);
         simulate_mouse_up();
//         if (!replaying() && tt_log_out_file_name == NULL) {
         if (!tt_subtitles_and_narration_even_without_demo) { // tt_log_version_number > 3 &&
				saved_tt_help_speed_percent = tt_help_speed_percent; // new on 110401
            tt_help_speed_percent = (short int) (tt_help_speed_percent*2); // double speed for credits
//				saved_martian_background = tt_martian->pointer_to_background();
//				tt_martian->lower_left_corner(saved_martian_llx,saved_martian_lly);
//				if (saved_martian_background != NULL) { // E.g. Martian was saved with the city so is in a room
//					tt_martian->set_background(NULL,FALSE,FALSE); // args added to avoid spurious warnings
//				};
            martian = new Martian(); // new on 090502
            martian->teleport_in(MARTY_CALLED_BY_TITLES);
//				WCHAR test[5] = {27700,12288,12434,12288,0};
//				tt_martian->say(test,99);
//				tt_martian->say(" clipping: ",99);
//				ascii_string test2 = "Before …@‚ð@  after";
//				wide_string test3 = new WCHAR[10];
//MultiByteToWideChar(932,0,test2,-1,test3,10);
//message_box(MB_OK,test3);
//				tt_martian->say(test2,99);
//				tt_martian->set_size_porportional_to_screen_size(TRUE);
				// new on 160801 so when running with subtitles the credits come out OK
				UnwindProtect<boolean> set(tt_closed_caption,FALSE);
				UnwindProtect<boolean> set2(tt_closed_caption_alternative_language,FALSE);
				martian->say(S(IDS_TOONTALK_CREDITS),99);
            martian->say(S(IDS_TOONTALK_CREDITS2),99);
         };
         return(PROGRAMMER_NORMAL);
//       } else if ((background_id == PUBLISHER_LOGO_BACKGROUND) &&
//                  tt_martian->still_talking()) {
//          tt_martian->dont_wait_to_say_more();
//          return(PROGRAMMER_NORMAL);
//      } else if ((background_id == PUBLISHER_LOGO_BACKGROUND) ||
//                 (background_id == OPENING_TITLE_BACKGROUND && replaying())) {
//         background->set_index(THOUGHT_BUBBLE_BACKGROUND);
//         string credits = new char[512]; // too long for 16 bit resources
//         strcpy(credits,S(IDS_TOONTALK_CREDITS));
//         strcat(credits,S(IDS_TOONTALK_CREDITS2));
//         background->set_text(credits);
//         delete [] credits;
//         background->set_text(S(IDS_WELCOME_MESSAGE));
//         simulate_mouse_up();
//         return(PROGRAMMER_NORMAL);
      } else if (tt_system_mode == PUZZLE) {
         if (background_id == publisher_background_id && !tt_subtitles_and_narration_even_without_demo) {  // && tt_log_version_number > 3
//             !replaying() && tt_log_out_file_name == NULL) {
            martian->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
            martian->teleport_out_done();
//            background->remove_item(tt_martian);
//            tt_help_speed_percent = (short int) (tt_help_speed_percent/2); // restore
				if (saved_tt_help_speed_percent > 0) { // more reliable way of restoring the help speed - new on 110401
					tt_help_speed_percent = saved_tt_help_speed_percent;
				};
         };
         if (!starting_with_first_tutorial_puzzle()) return(PUZZLE_START);
//#if TT_BETA
//         if (!valid_image_id(PUZZLE_RESCUE_BACKGROUND)) return(PUZZLE_START);
//#endif
         if (background_id == publisher_background_id) {         
            background->set_index(PUZZLE_SINKING_BACKGROUND);
            background->set_text(S(IDS_PUZZLE_SINKING));
         } else {
            switch (background_id) {
//            case publisher_background_id: 
//               background->set_index(PUZZLE_SINKING_BACKGROUND);
//               background->set_text(S(IDS_PUZZLE_SINKING));
//               break;
               case PUZZLE_SINKING_BACKGROUND:
                  background->set_index(PUZZLE_CRASH_BACKGROUND);
                  background->set_text(S(IDS_PUZZLE_CRASH));
   //               background->set_text_color(tt_white);
                  break;
               case PUZZLE_CRASH_BACKGROUND:
                  background->set_index(PUZZLE_HURT_BACKGROUND);
                  background->set_text(S(IDS_PUZZLE_HURT));
   //               background->set_text_color(tt_black);
                  break;
               case PUZZLE_HURT_BACKGROUND:
                  background->set_index(PUZZLE_RESCUE_BACKGROUND);
                  background->set_text(S(IDS_PUZZLE_RESCUE));
   //               background->set_text_color(tt_white);
                  break;
               case PUZZLE_RESCUE_BACKGROUND:
                  return(PUZZLE_START);
            };
         };
         background->set_y_offset_percent(60); // was 80
         simulate_mouse_up();
         return(PROGRAMMER_NORMAL);
      } else {
//         background->set_text_color(tt_black);
//         if (!replaying() && tt_log_out_file_name == NULL) { // better tt_martian->active() ??
        if (!tt_subtitles_and_narration_even_without_demo) { // tt_log_version_number > 3 &&
            martian->teleport_out_done();
//            background->remove_item(tt_martian);
//            tt_help_speed_percent = (short int) (tt_help_speed_percent/2); // restore
				if (saved_tt_help_speed_percent > 0) { // more reliable way of restoring the help speed - new on 110401
					tt_help_speed_percent = saved_tt_help_speed_percent;
				};
         };
  			tt_titles_just_ended = TRUE; // new on 230803 to this happens at the right time and at the end of the cycle
		   return(initial_programmer_status());
      };
	} else {
		return(PROGRAMMER_NORMAL);
	};
};


/*
#if REGIONALLY_SENSITIVE
	// here just for testing
//	string country_code = System.Globalization.RegionInfo.Name;
	GEOID geoID = GetUserGeoID(GEOCLASS_NATION);
	Seller seller = seller_from_geoID(geoID);
	#if REGIONALLY_SENSITIVE
	// here just for testing
//	string country_code = System.Globalization.RegionInfo.Name;
	GEOID geoID = GetUserGeoID(GEOCLASS_NATION);
	Seller seller = seller_from_geoID(geoID);
	Seller seller = seller_from_IP(IP_address());
	seller = seller_from_IP(htonl(inet_addr("89.242.211.193"))); // talk talk address from home today
	seller = seller_from_IP(htonl(inet_addr("72.14.207.99"))); // Google.com
	seller = seller_from_IP(htonl(inet_addr("200.234.196.118"))); // divertire.com.br
	seller = seller_from_IP(htonl(inet_addr("193.10.66.195"))); // sics.se
	seller = seller_from_IP(htonl(inet_addr("193.136.40.13"))); // utad.pt (cnotinfor.pt is in Texas!)
	seller = seller_from_IP(htonl(inet_addr("211.9.254.66"))); // www.micronet.co.jp
	seller = seller_from_IP(htonl(inet_addr("203.99.65.70"))); // www.nzherald.co.nz
	seller = seller_from_IP(htonl(inet_addr("203.58.234.68"))); // www.smh.com.au
#endif
#endif
*/
//#endif


//ProgrammerStatus Programmer_Titles_Flying::next_title_if_showing_titles() {
	// act like a space bar was pressed
//	return(react(FALSE,' ',0,0,0,0,FALSE,0,0));
//};

ProgrammerStatus Programmer_Titles_Flying::initial_programmer_status() {
   if (tt_system_mode == PUZZLE) return(PUZZLE_START);
   return(tt_programmer->status_when_dumped());
};

//void Programmer_Titles_Flying::display_appearance(Screen *screen) {
//  if (displayed) return;
//  displayed = TRUE;

//const int Programmer_City_Flying::minimum_pixel_movement_to_reorient = 10;
//const int Programmer_City_Flying::maximum_rotation_per_second = 360;

// this uses the log of the scale so I can change it arithmetically
//const float Programmer_City_Flying::scale_growth = log(3.0);  // default is to triple every second

//#if TT_DIRECT_INPUT
//flag given_joystick_warning = FALSE;
//#endif

void set_mouse_mode(MouseMode new_mode) {
   tt_mouse_mode = new_mode;
   tt_dragging_permitted = (tt_mouse_mode == ABSOLUTE_MOUSE_MODE); // new on 070502
//	if (tt_mouse_mode == ABSOLUTE_MOUSE_MODE) {
//		tt_error_file() << "Absolute mouse mode set." << endl;
//	} else {
//		tt_error_file() << "Mouse mode set to relative mouse mode." << endl;
//	};
};

Programmer_City_Flying::Programmer_City_Flying(city_coordinate view_center_x,
															  city_coordinate view_center_y,
															  city_scale scale,
//															  Programmer *the_programmer,
                                               Direction starting_direction,
                                               boolean ) :
  center_x(view_center_x),
  center_y(view_center_y),
  scale(scale),
#if TT_DIRECT_INPUT
  stuck_count(0),
#endif
  minimum_distance_to_reorient(tile_width) {
//   programmer = the_programmer;
   current_direction = starting_direction;
  // while flying the programmer position is always the center of the view
  // so no need for this to deal with both coordinates
//  x = view_center_x;
//  y = view_center_y;
  tt_city->extent(min_x,max_x,min_y,max_y);
  // stop movement near the city borders
  // commented out on 221299 since prevented flying near edge of city
//  if (tt_city_size > 1 || tt_city_size < -1) {
//	  min_x += tile_width*20;
//	  max_x -= tile_width*20;
//	  min_y += tile_height*20;
//	  max_y -= tile_height*20;
//  };
//  if (tt_log_version_number > 8) {
     max_x_speed = 2*ideal_screen_width; // 1/2 second to cross the screen
     max_y_speed = 2*ideal_screen_height;
//  } else {
//     max_x_speed = ideal_screen_width;  // was 1 second
//     max_y_speed = ideal_screen_height;
//  };
#if TT_LARGE_CITIES
  int size = max(abs(tt_city_width),abs(tt_city_height));
  max_scale = size*125*tt_houses_to_a_block; // 50,000 was ok for a 100x100 city
  if (max_scale/50 > tt_screen_width*tt_city_width) {
	  max_scale = 50*tt_screen_width*tt_city_width; // new on 020405 so can't fly so high that the city is sub-pixel
  };
  if (max_scale/50 > tt_screen_height*tt_city_height) {
	  max_scale = 50*tt_screen_height*tt_city_height; // new on 020405 so can't fly so high that the city is sub-pixel
  };
#else
  if (tt_city_size > 0) {
	  max_scale = tt_city_size*500L*tt_houses_to_a_block/4; // 50,000 was ok for a 100x100 city
  } else {
	  max_scale = tt_city_size*-500L*tt_houses_to_a_block/4; // negative to indicate fill randomly
  };
#endif
//  adjust_width_to_640x480_screen(max_scale);
  appearance = new Programmer_Sprite(HELIOFLY,view_center_x,view_center_y,tt_city,(short) starting_direction,scale);
  appearance->set_scale_for_height_of_camera(FALSE); // new on 230804
  initialize_appearance();
  play_sound(HELICOPTER_SOUND,TRUE);
#if TT_DIRECT_INPUT
  set_joystick_sensitivity(33);
  set_background_force_feedback_effect("Helicopter Flying");
#endif
  set_mouse_mode(tt_mouse_mode_not_on_floor); // new on 250402
  show_cursor(tt_mouse_mode != RELATIVE_MOUSE_MODE);
};

void Programmer_City_Flying::initialize_appearance() { // abstracted on 100103
//  appearance->set_size_porportional_to_screen_size(FALSE);
  old_x_offset = (appearance->current_width()*scale)/(-2*ground_scale);
  old_y_offset = (appearance->current_height()*scale)/(-2*ground_scale);
//  adjust_size_from_640x480_screen(old_x_offset,old_y_offset);
//  if (resuming_saved_state) {
//     appearance->move_by(appearance->current_width()/-2,appearance->current_height()/-2);
//  } else {
     appearance->move_by(old_x_offset,old_y_offset);
//  };
//  appearance->set_size(appearance->current_width()/2,
//                       appearance->current_height()/2);
//  appearance->set_priority_function_of_lly(FALSE);
  appearance->set_priority_fixed(TRUE);
  appearance->set_priority(min_long+1); // above everything else (including birds) +1 added 240100 so time travel buttons on top
};

#if TT_DIRECT_INPUT
void play_wall_force_feedback_effect(Direction direction) {
   switch (direction) {
      case NORTH:
      case NORTHWEST:
         play_force_feedback_effect("Wall North");
         break;
      case SOUTH:
      case SOUTHEAST:
         play_force_feedback_effect("Wall South");
         break;
      case NORTHEAST:
      case EAST:
         play_force_feedback_effect("Wall East");
         break;
      case WEST:
      case SOUTHWEST:
         play_force_feedback_effect("Wall West");
         break;
      case NO_DIRECTION:
         stop_force_feedback_effect();
         break;
   };
};
#endif

ProgrammerStatus Programmer_City_Flying::react(boolean new_user_input,
															  unsigned char key,
															  boolean extended_key,
															  city_coordinate delta_x,
															  city_coordinate delta_y,
															  char button_status,
															  boolean ,
															  millisecond duration,
															  boolean ) {
#if !TT_DIRECTX
	play_sound(HELICOPTER_SOUND,TRUE);
#endif
	ProgrammerStatus status = PROGRAMMER_NORMAL;
	if (tt_programmer->invisible()) return(status); // new on 221299 so if invisible can play games, etc.
	if (new_user_input) {
		boolean scale_changed = FALSE;
		// following updated on 030605 so works even if ABSOLUTE_MOUSE_MODE
      boolean move_up = (//tt_mouse_mode != ABSOLUTE_MOUSE_MODE &&
                         (right_button(button_status) || // real right button
                          virtual_right_button(button_status,FALSE) ||  // shift any button
                          (!extended_key && keyboard_accelerator(key,UP_IF_IN_HELICOPTER))));
		// following updated on 030605 so if ABSOLUTE_MOUSE_MODE then keyboard buttons work
      boolean move_down = ((tt_mouse_mode != ABSOLUTE_MOUSE_MODE && left_button(button_status)) ||
                           (!extended_key && keyboard_accelerator(key,DOWN_IF_IN_HELICOPTER)));
      if (tt_delta_x_and_delta_y_due_solely_to_arrow_keys) {
         // new on 060502 so up and down arrow keys move in z rather y
         if (delta_y > 0) {
            move_up = TRUE;
            delta_y = 0;
         } else if (delta_y < 0) {
            move_down = TRUE;
            delta_y = 0;
         };
      };
		dampen_big_deltas(delta_x,delta_y);
		minimum_distance_to_reorient -= labs(delta_x);
		minimum_distance_to_reorient -= labs(delta_y);
		if (minimum_distance_to_reorient < 0) {
			minimum_distance_to_reorient = tile_width;
			Direction new_direction = direction(delta_x,delta_y);
			current_direction = dampen_turn(new_direction,current_direction);
		};
		if (move_up) {
//			growth = exp(duration*scale_growth);
//			scale *= growth;
			if (duration > 750) {
				duration = 750; // limit change if long duration
			};
			scale = grow_value(scale,duration,750); // 3/4 second to double
			if (scale > max_scale) {
				scale = max_scale;
			};
			flush_input_events();
			appearance->set_scale(scale);
			scale_changed = TRUE;			
		} else if (move_down) { 
//			growth = exp(-duration*scale_growth);
//			scale *= growth;
			if (duration > 750) duration = 750; // limit change if long duration
			scale = shrink_value(scale,duration,750); // 3/4 second to halve
			if (scale < tt_min_flying_scale) {
				scale = tt_min_flying_scale;
				status = LANDING; // near bottom
            // had adjusted location now undo adjustment so just up and down doesn't move
//            appearance->move_by((appearance->current_width()*scale)/(2*ground_scale),
//                                (appearance->current_height()*scale)/(2*ground_scale));
    		};
			flush_input_events(); // so I can hold the button down
			scale_changed = TRUE;
			appearance->set_scale(scale);
		};
//#if TT_DEBUG_ON
//		if (tt_debug_mode == 7023 && scale_changed) {
//			tt_error_file() << scale << " scale at frame = " << tt_frame_number << endl;
//		};
//#endif
      city_coordinate x,y;
		true_center(x,y);
#if TT_DIRECT_INPUT
      boolean x_clipped = FALSE;
      boolean y_clipped = FALSE;
      Direction direction = NO_DIRECTION;
#endif
		if (delta_x != 0) {
			delta_x = (delta_x*scale)/ground_scale;
			x += delta_x;
			if (x < min_x) {
				delta_x += min_x-x;
#if TT_DIRECT_INPUT
            x_clipped = TRUE;
            direction = WEST;
#endif
//				x = min_x;
			} else if (x > max_x) {
				delta_x -= x-max_x;
#if TT_DIRECT_INPUT
            x_clipped = TRUE;
            direction = EAST;
#endif
//				x = max_x;
			};
		};
		if (delta_y != 0) {
			delta_y = (delta_y*scale)/ground_scale;
			y += delta_y;
			if (y < min_y) {
				delta_y += min_y-y;
#if TT_DIRECT_INPUT
            y_clipped = TRUE;
            direction = SOUTH;
#endif
//				y = min_y;
			} else if (y > max_y) {
				delta_y -= y-max_y;
#if TT_DIRECT_INPUT
            y_clipped = TRUE;
            direction = NORTH;
#endif
//				y = max_y;
			};
		};
#if TT_DIRECT_INPUT
      if (//!given_joystick_warning &&
          !tt_shutting_down &&
          ((x_clipped && joystick_x(0) != 0) || (y_clipped && joystick_y(0) != 0))) { // stuck in a corner or edge
         stuck_count++;
         if (stuck_count > 50) { // long enuf to give a warning
            trouble_shoot(TROUBLE_SHOOT_JOYSTICK_CALIBRATION); // ,NULL,NULL,TRUE);
				stuck_count = 0;
//            given_joystick_warning = TRUE;
//            if (message_box(MB_YESNO,S(IDS_JOYSTICK_STUCK),NULL,FALSE) == IDYES) {
//               launch_help("joycalib");
//            } else {
//               OpenIcon(tt_main_window->get_handle());
////               unpause_toontalk_soon();
//            };
         };
//      } else {
//if (stuck_count > 0) { // debuggin
//tt_error_file() << "Count is " << stuck_count << " joy x is " << joystick_x(0) << " and joy y is " << joystick_y(0) << endl;
//tt_error_file() << "Delta x is " << delta_x << " and delta y is " << delta_y << endl;
//};
//         stuck_count = 0;
      };
      play_wall_force_feedback_effect(direction);
#endif
		// if moved then moved camera as well
//		if (only_z_movement) {
//			city_scale ignore;
//			screen->viewing_region(x,y,ignore); // restore x and y
//		   appearance->center(x,y);
//		};
//		adjust_size_from_640x480_screen(delta_x,delta_y);
		center_x += delta_x;
		center_y += delta_y;
		tt_screen->update_viewing_region(center_x,center_y,scale);
		if (scale_changed) {
			tt_screen->set_city_to_user_units((scale*ideal_screen_width/1000)/ground_scale,
                                           (scale*ideal_screen_height/1000)/ground_scale);
			city_coordinate new_x_offset = (appearance->current_width()*scale)/(-2*ground_scale);
			city_coordinate new_y_offset = (appearance->current_height()*scale)/(-2*ground_scale);
//			grow_width_to_640x480_screen_size(new_x_offset); 
//			grow_height_to_640x480_screen_size(new_y_offset);
         delta_x += new_x_offset-old_x_offset;
			delta_y += new_y_offset-old_y_offset; // was new_x_offset prior to 150999
//			adjust_size_from_640x480_screen(delta_x,delta_y); // commented out on 090502 since update_display no longer scales the deltas
			old_x_offset = new_x_offset;
			old_y_offset = new_y_offset;
		};
	};
	appearance->set_parameter((short) current_direction);
//	delta_x = shrink_width_from_640x480_screen_size(delta_x);
//	delta_y = shrink_height_from_640x480_screen_size(delta_y);
	appearance->update_display(delta_x,delta_y);
	if (key == '\026' && !extended_key) { // control-v added on 300904
		add_item_from_clipboard(NULL,appearance,key);
	} else if (key == '\003' && !extended_key) { // control-c added on 300904
		// copied here so can copy entire city
		put_on_clipboard(NULL,appearance->pointer_to_background(),appearance,key);
		play_sound(POP_SOUND);
	};
//	if (!extended_key && key == VK_ESCAPE) toggle_pause();
	return(status);
};

void Programmer_City_Flying::true_center(city_coordinate &x, city_coordinate &y) { // abstracted into a method on 060502
	appearance->lower_left_corner(x,y);
	// compute true center in ground coordinates
	x += (appearance->true_width()*scale)/(ground_scale*2);
	y += (appearance->true_height()*scale)/(ground_scale*2);
};

void Programmer_City_Flying::set_visibility(boolean visible) {
	if (visible) {
		play_sound(HELICOPTER_SOUND,TRUE);
	} else {
		stop_sound_id(HELICOPTER_SOUND);
	};
};

#if TT_XML

void Programmer_City_Flying::add_xml(xml_element *element, xml_document *document) {
	xml_set_attribute(element,L"CenterX",center_x);
	xml_set_attribute(element,L"CenterY",center_y);
	xml_set_attribute(element,L"Scale",scale);
	xml_set_attribute(element,L"OldXOffset",old_x_offset);
	xml_set_attribute(element,L"OldYOffset",old_y_offset);
	xml_set_attribute(element,L"MinimumDistaneToReorient",minimum_distance_to_reorient);
	xml_set_attribute(element,L"MaxScale",max_scale);
#if TT_DIRECT_INPUT
	if (stuck_count > 0) xml_set_attribute(element,L"StuckCount",stuck_count);
#endif
	Programmer_State::add_xml(element,document);
};

boolean Programmer_City_Flying::handle_xml_tag(Tag tag, xml_node *node) {
	switch (tag) {
		case NO_MORE_TAGS:
			center_x = xml_get_attribute_int(node,L"CenterX",center_x);
			center_y = xml_get_attribute_int(node,L"CenterY",center_y);
			scale = xml_get_attribute_int(node,L"Scale",scale);
			old_x_offset = xml_get_attribute_int(node,L"OldXOffset",old_x_offset);
			old_y_offset = xml_get_attribute_int(node,L"OldYOffset",old_y_offset);
			minimum_distance_to_reorient = xml_get_attribute_int(node,L"MinimumDistaneToReorient",minimum_distance_to_reorient);
			max_scale = xml_get_attribute_int(node,L"MaxScale",max_scale);
#if TT_DIRECT_INPUT
			stuck_count = xml_get_attribute_int(node,L"StuckCount",0); // prior to 210404 was stuck_count
#endif		
			// and the following
		default:
			return(Programmer_State::handle_xml_tag(tag,node));
	};
};

void Programmer_City_Flying::activate() {
	tt_screen->switch_to(tt_city,TRUE,FALSE);
	appearance->set_background(tt_city);
//	initialize_appearance(); 
	// to get offsets to work right with the current scale -- maybe better to just restore all this instead...
	old_x_offset = (appearance->current_width()*scale)/(-2*ground_scale);
   old_y_offset = (appearance->current_height()*scale)/(-2*ground_scale);
	appearance->set_scale(scale);
	Programmer_State::activate();
};

#endif

// about 2 seconds to land just with left button -- prior to 100103 was an instance variable of Programmer_City_Landing
const int button_speed = 5*tile_height; 

Programmer_City_Landing::Programmer_City_Landing(//Programmer *the_programmer,
																 //Screen *screen,
																 Sprite *old_appearance) {
//	programmer = the_programmer;
	city_scale scale;
	city_coordinate center_view_x, center_view_y, x, y;
	tt_screen->viewing_region(center_view_x,center_view_y,scale);
//   if (tt_log_version_number > 8) {
      max_x_speed = 3*ideal_screen_width; // 1/3 second to cross the screen
      max_y_speed = 3*ideal_screen_height;
//   } else {
//      max_x_speed = ideal_screen_width;  // was 1 second
//      max_y_speed = ideal_screen_height;
//   };
	if (old_appearance == NULL) { // landing
		x = center_view_x-8*tile_width;
		center_view_y = tt_city->nearest_x_street(center_view_y)+10*tile_height; 
		// adjust camera so a street is on the bottom of screen
		y = center_view_y+6*tile_height; // start off near top of screen
//#if TT_BETA
//		if (tt_running_old_demos) {
//			min_y = center_view_y-11*tile_height;
//		} else
//#endif
      // changed to make more room for trucks driving around
		min_y = center_view_y-13*tile_height;
		tt_screen->update_viewing_region(center_view_x,center_view_y,ground_scale);
//      screen->set_city_to_user_units(person_on_ground_width/1000,person_on_ground_height/1000);
		tt_screen->new_view(CAMERA_IN_FRONT);
		appearance = new Sprite(HELIOLND,x,y,HELICOPTER);
		appearance->set_background(tt_city); // new on 100103 so will dump with geometry
      appearance->set_ok_to_dump(FALSE);
//		appearance->set_size_porportional_to_screen_size(TRUE);
	} else {
		appearance = old_appearance;
		min_y = appearance->current_lly();
	};
	appearance->set_visible(!tt_programmer->invisible());
//	tt_error_file() << center_view_y << ";f=" << tt_frame_number << endl;
	max_y = center_view_y+10*tile_height;
	min_x = center_view_x-10*tile_width;
	max_x = center_view_x;
//   appearance->set_priority_function_of_lly(FALSE);
   appearance->set_priority_fixed(TRUE);
	appearance->just_set_priority(programmer_landing_priority);  // so this is front of houses etc.
#if TT_DIRECT_INPUT
   set_joystick_sensitivity(50);
   set_background_force_feedback_effect("Helicopter Flying");
#endif
   set_mouse_mode(tt_mouse_mode_not_on_floor); // new on 250402
   show_cursor(tt_mouse_mode != RELATIVE_MOUSE_MODE);
   if (tt_mouse_mode == ABSOLUTE_MOUSE_MODE) { // hard to land or take off so made top and bottom active regions wider (060502)
//      min_y += 5*tile_height;
      max_y -= (2*appearance->true_height())/3; // so if 1/3 goes off top we'll switch views
   };
};

ProgrammerStatus Programmer_City_Landing::react(boolean new_user_input,
																unsigned char key,
																boolean extended_key,
																city_coordinate delta_x,
																city_coordinate delta_y,
																char button_status,
																boolean ,
																millisecond duration,
																boolean ) {
#if !TT_DIRECTX
	play_sound(HELICOPTER_SOUND,TRUE);
#endif
	ProgrammerStatus status = PROGRAMMER_NORMAL;
	if (tt_programmer->invisible()) return(status); // new on 221299 so if invisible can play games, etc.
	if (new_user_input) {
      boolean move_up = (right_button(button_status) || // real right button
                          virtual_right_button(button_status,FALSE) ||  // shift any button
                          (!extended_key && keyboard_accelerator(key,UP_IF_IN_HELICOPTER)));
      boolean move_down = (left_button(button_status) ||
                           (!extended_key && keyboard_accelerator(key,DOWN_IF_IN_HELICOPTER)));
      if (move_up) {
			if (delta_y < 0) {
				delta_y = 0; // ignore mouse/joystick if button down
			};
			flush_input_events();
			delta_y += (button_speed*duration)/1000;
//#if TT_DEBUG_ON
//   		   tt_error_file() << "Frame=" << tt_frame_number
//   						   << ";delta_y=" << delta_y
//   						   << ";button_speed=" << button_speed
//                        << ";duration=" << duration << endl;
//#endif
      } else if (move_down) {
			if (delta_y > 0) {
				delta_y = 0;
			};
			flush_input_events();
			delta_y -= (button_speed*duration)/1000;
		};
		dampen_big_deltas(delta_x,delta_y);
      city_coordinate x,y;
		appearance->lower_left_corner(x,y);
		y += delta_y;
		if (y > max_y && delta_y > 0) { // second condition added on 060503
			status = FLYING_AGAIN;
//         if (tt_mouse_mode == ABSOLUTE_MOUSE_MODE) {
//            y += (2*appearance->true_height())/3; // new on 060502 since lower switching level
//         };
		};
		if (y < min_y) {
			delta_y += min_y-y;
//			y = min_y;
			status = LEAVE_HELICOPTER;
		};
		x += delta_x;
		city_coordinate x_adjustment = 0;
		if (x < min_x) {
			x_adjustment = x-min_x;
// this should only be triggered if hitting true limits which aren't checked here
// this means that you can fly far out over the water this way
//#if TT_DIRECT_INPUT
//         play_wall_force_feedback_effect(WEST);
//#endif
		} else if (x > max_x) {
			x_adjustment = x-max_x;
//#if TT_DIRECT_INPUT
//         play_wall_force_feedback_effect(EAST);
//#endif
		};
		// too close to left or right edge
		min_x += x_adjustment;
		max_x += x_adjustment;
		tt_screen->move_by(x_adjustment,0);
	};
	appearance->update_display(delta_x,delta_y);
	if (key == '\026' && !extended_key) { // control-v added on 300904
		add_item_from_clipboard(NULL,appearance,key);
	} else if (key == '\003' && !extended_key) { // control-c added on 300904
		// copied here so can copy entire city
		put_on_clipboard(NULL,appearance->pointer_to_background(),appearance,key);
		play_sound(POP_SOUND);
	};
//	tt_error_file() << appearance->resource_index() << " " << tt_frame_number << endl;
//	if (!extended_key && key == VK_ESCAPE) toggle_pause();
	return(status);
};

void Programmer_City_Landing::cleanup(Screen *, ProgrammerStatus next_status) {
	// leaves appearance on screen (empty helicopter)
	if (next_status != FLYING && next_status != FLYING_AGAIN) {
	   // i.e. leaving copter
		appearance = NULL; // so it is not deleted
#if TT_DIRECT_INPUT
      set_background_force_feedback_effect(NULL);
#endif
	};
};

void Programmer_City_Landing::set_visibility(boolean visible) {
	if (visible) {
		play_sound(HELICOPTER_SOUND,TRUE);
	} else {
		stop_sound_id(HELICOPTER_SOUND);
	};
	if (appearance != NULL) {
		appearance->set_visible(visible);
	};
};

void Programmer_City_Landing::true_center(city_coordinate &x, city_coordinate &y) {
   // this uses approximately center of the door
   if (appearance != NULL) {
      appearance->lower_left_corner(x,y);
      x += (appearance->true_width()*3)/4;
      y += appearance->true_height()/3;
   } else { // warn?
      x = 0;
      y = 0;
   };
};

#if TT_XML

void Programmer_City_Landing::activate() { // new on 100103
	tt_screen->switch_to(tt_city,TRUE,FALSE);
	appearance->set_background(tt_city);
	if (!tt_programmer->invisible()) {
		play_sound(HELICOPTER_SOUND,TRUE);
	};
	//tt_screen->new_view(CAMERA_IN_FRONT);
	//city_coordinate x,y;
	//position(x,y);
	//tt_screen->update_viewing_region(x,y,ground_scale);
	Programmer_State::activate();
};

#endif

// below the minimum only part of the maximum rotation is allowed
//const city_coordinate
//  Programmer_Walking::minimum_movement_to_reorient = tile_width/2;
// .67 seconds to turn around
//const int Programmer_Walking::maximum_rotation_per_second = 540;

Programmer_Walking::Programmer_Walking(Visible_Background *background) :
	previous_appearance_image_index(-1),
	image_change_count(0),
	toolbox(NULL),
	spinning(FALSE),
	delta_x(0), // wasn't initialized prior to 090103
	delta_y(0),
	minimum_distance_to_reorient(0),
	toolbox_x(0),
	toolbox_y(0),
	background(background) {
	 // added 200499 so coordinate and size remote controls work in a room
	city_coordinate floor_x_unit = ideal_screen_width/1000;
	city_coordinate floor_y_unit = ideal_screen_height/1000;
	tt_programmer->translate_floor_to_room_dimensions(floor_x_unit,floor_y_unit);
	tt_screen->set_city_to_user_units(floor_x_unit,floor_y_unit);
   set_mouse_mode(tt_mouse_mode_not_on_floor); // new on 250402
   show_cursor(tt_mouse_mode != RELATIVE_MOUSE_MODE);
};

Programmer_Walking::~Programmer_Walking() {
	if (toolbox != NULL) {
		toolbox->destroy();
		toolbox = NULL;
	};
};

void Programmer_Walking::finished_standing_up(Background *background) {
// following causes grief because something like a bird may be trying to fly to programmer
// and it was destroyed
//	city_coordinate llx, lly;
//	appearance->lower_left_corner(llx,lly);
//	tt_screen->remove(appearance);
//	appearance->destroy();
//	appearance = new Programmer_Sprite(PERSON_WALKING_SPRITE,llx-1000,lly-700,
//												  programmer,background,(short) current_direction);
//	tt_screen->add(appearance);
// now we try to update the old appearance appropriately
	if (tt_programmer->invisible()) { // new on 131202 - new policy to become visible as you stand up
		tt_programmer->set_visibility(TRUE);
	};
   swap_appearance(PERSON_WALKING_SPRITE,(short int) current_direction);
   appearance->move_by(-1000,-700); // fix drift so stand up then sit down doesn't move
   if (background->in_the_room()) {
      appearance->set_priority_fixed(TRUE); // so he can step on everything on the floor
      appearance->set_priority(programmer_room_walking_priority);
   };
	appearance->set_ideal_speed(appearance->current_height()/2);
   tt_selection = NULL;
};

Direction move_back_out_of(TTRegion &others_region, TTRegion &my_region,
							      city_coordinate &delta_x, city_coordinate &delta_y) {
//	tt_error_file() << "Originals deltas are " << delta_x << " and " << delta_y << endl;
//	tt_error_file() << "Others and my region are " 
//					<< others_region->min_x << "," << others_region->max_x << " "
//					<< others_region->min_y << "," << others_region->max_y << " and " 
//					<< my_region->min_x << "," << my_region->max_x << " "
//					<< my_region->min_y << "," << my_region->max_y << endl;
	// re-define deltas based upon collisions regions collision
	city_coordinate new_delta_x = 0, new_delta_y = 0;
   Direction direction = NORTH; // need a default
	if (others_region.overlaps(&my_region)) { 
		// would have collided even without moving
		boolean my_left_inside = (others_region.min_x < my_region.min_x && my_region.min_x < others_region.max_x);
		boolean my_right_inside = (others_region.min_x < my_region.max_x && my_region.max_x < others_region.max_x);
		if (my_left_inside) {
			if (my_right_inside) { // I'm in it
				new_delta_x = delta_x/-2; 
			} else {
				new_delta_x = (others_region.max_x-my_region.min_x)+1;
			};
         direction = WEST;
		} else if (my_right_inside) {
			new_delta_x = (my_region.min_x-others_region.max_x)-1;
         direction = EAST;
		} else if (my_region.min_x < others_region.min_x &&
					  my_region.max_x > others_region.max_x) {
			// it is inside of me
			new_delta_x = delta_x/-2;
		};
		boolean my_bottom_inside = (others_region.min_y < my_region.min_y && my_region.min_y < others_region.max_y);
		boolean my_top_inside = (others_region.min_y < my_region.max_y && my_region.max_y < others_region.max_y);
		if (my_bottom_inside) {
			if (my_top_inside) { // I'm inside
				new_delta_y = delta_y/-2;
			} else {
				new_delta_y = (others_region.max_y-my_region.min_y)+1;
			};
         direction = SOUTH;
		} else if (my_top_inside) {
			new_delta_y = (others_region.min_y-my_region.max_y)-1;
         direction = NORTH;
		} else if (my_region.min_y < others_region.min_y &&
					  my_region.max_y > others_region.max_y) {
			new_delta_y = delta_y/-2;
		};
		// make the smaller non-zero movement
		if (new_delta_x == 0 || new_delta_y == 0) {
			delta_x = new_delta_x;
			delta_y = new_delta_y;
		} else if (labs(new_delta_x) > labs(new_delta_y)) {
			delta_x = 0;
			delta_y = new_delta_y;			
		} else { 
			delta_x = new_delta_x;
			delta_y = 0;
		};
	} else { // went too far causing collision need to shrink deltas
		if (delta_x > 0) { // moving right
			new_delta_x = (others_region.min_x-my_region.max_x)-1;
			if (new_delta_x > 0) {
				delta_x = min(delta_x,new_delta_x);
			} else {
				delta_x = 0;
			};
//         direction = EAST;
		} else {  // moving left
			new_delta_x = (my_region.min_x-others_region.max_x)-1;
			if (new_delta_x < 0) {
				delta_x = max(delta_x,new_delta_x); // since negative
			} else {
				delta_x = 0;
			};
//         direction = WEST;
		};
		if (delta_y > 0) { // moving up
			new_delta_y = (others_region.min_y-my_region.max_y)-1;
			if (new_delta_y > 0) {
				delta_y = min(delta_y,new_delta_y);
			} else {
				delta_y = 0;
			};
         direction = NORTH;
		} else {  // moving down
			new_delta_y = (my_region.min_y-others_region.max_y)-1;
			if (new_delta_y < 0) {
				delta_y = max(delta_y,new_delta_y); // since negative
			} else {
				delta_y = 0;
			};
         direction = SOUTH;
		};
//		if (others_region->overlaps(my_region)) {
//			tt_error_file() << "whoops!" << endl;
//		};
	};
   return(direction);
//	tt_error_file() << "New deltas are " << delta_x << " and " << delta_y << endl;
};

/*
void Programmer_Walking::move_just_below_region(TTRegion *region) {
	city_coordinate ignore,lly;
	appearance->lower_left_corner(ignore,lly); 
	delta_y = (region->min_y-1)-lly; // 1 unit below door collision region
};

void Programmer_Walking::move_back_out_of(TTRegion *, boolean move_extra) {
  city_coordinate extra_x = 0;
  city_coordinate extra_y = 0;
  if (move_extra) {
	  extra_x = delta_x/2; 
	  extra_y = delta_y/2;  
  };
//  x -= delta_x+extra_x;
//  y -= delta_y+extra_y;
  delta_x = -extra_x;
  delta_y = -extra_y;
};
*/

//  } else {
//	  appearance->avoid_collision(region,delta_x,delta_y,delta_x,delta_y);
//	  x += delta_x;
//	  y += delta_y;
//	  appearance->update_display(delta_x,delta_y,TRUE,TRUE,orientation);
//  };

void Programmer_Walking::toolbox_center(city_coordinate &new_toolbox_x,
													 city_coordinate &new_toolbox_y,
													 boolean stay_in_region) {
  appearance->lower_left_corner(new_toolbox_x,new_toolbox_y);
  city_coordinate x_delta = appearance->current_height()/2;
  city_coordinate y_delta = appearance->current_height()/2;
  switch (current_direction) {
	 case EAST:
		 new_toolbox_x -= x_delta;
		 break;
	 case SOUTHEAST:
		 new_toolbox_x -= x_delta;
		 new_toolbox_y += y_delta;
		 break;
	 case SOUTH:
		 new_toolbox_y += y_delta;
		 break;
	 case SOUTHWEST:
		 new_toolbox_x += x_delta;
		 new_toolbox_y += y_delta;
		 break;
	 case WEST:
		 new_toolbox_x += x_delta;
		 break;
	 case NORTHWEST:
		 new_toolbox_x += x_delta;
		 new_toolbox_y -= y_delta;
		 break;
	 case NORTH:
		 new_toolbox_y -= y_delta;
		 break;
	 case NORTHEAST:
		 new_toolbox_x -= x_delta;
		 new_toolbox_y -= y_delta;
		 break;
  };
  if (stay_in_region) {
	  if (new_toolbox_x > max_x) {
		  new_toolbox_x = max_x;
	  } else if (new_toolbox_x < min_x) {
		  new_toolbox_x = min_x;
	  };
	  if (new_toolbox_y > max_y) {
		  new_toolbox_y = max_y;
	  } else if (new_toolbox_y < min_y) {
		  new_toolbox_y = min_y;
	  };
  };
};

boolean Programmer_Walking::add_toolbox() {
	// needed sometimes but does this mean if saved without Tooly he'll reincarnate?
	//if (!tt_reincarnate_tools) { // new on 080704
	//	return(FALSE);
	//};
   if (tt_toolbox == NULL) {
	   tt_toolbox = new Toolbox();
		tt_toolbox->initialize();
   };
	// moved down here on 150499 so tt_toolbox exists for pre-built robots to use even if Tooly isn't available
	if (tt_system_mode == PUZZLE && !is_toolbox_part_of_puzzle()) return(FALSE);
   if (tt_toolbox->gone()) return(FALSE);
//       !appearance->pointer_to_background()->ok_for_toolbox_entry()) return;
	toolbox_center(toolbox_x,toolbox_y);
	toolbox = new Sprite(TOOLBOX_SIDE_SPRITE,toolbox_x,toolbox_y,
								TOOLBOX,(short) current_direction,
								ground_scale);
	toolbox->set_ideal_speed(appearance->current_height());
//	toolbox->set_priority_function_of_lly(FALSE); // handled specially
   toolbox->set_containable_by_background(FALSE); // handled specially
	toolbox->set_move_between_floor_and_room(FALSE); // handled specially
	toolbox->set_visible(!tt_programmer->invisible());
	toolbox->set_background(appearance->pointer_to_background()); // new on 090103 so geometry is dumped
	tt_screen->add(toolbox);
   return(TRUE);
};

void Programmer_Walking::set_visibility(boolean visible) {
	if (toolbox != NULL) {
		toolbox->set_visible(visible);
	};
};

void Programmer_Walking::update_toolbox(Direction previous_direction,
													 city_coordinate delta_x,
													 city_coordinate delta_y,
													 boolean moved,
													 boolean stay_in_region,
													 boolean ) {
   if (toolbox == NULL) return;
	city_coordinate new_toolbox_x, new_toolbox_y;
	if (previous_direction == current_direction) {
		new_toolbox_x = toolbox_x+delta_x;
		new_toolbox_y = toolbox_y+delta_y;
	} else {
		toolbox_center(new_toolbox_x,new_toolbox_y,stay_in_region);
	};
	city_coordinate toolbox_llx, toolbox_lly; // real coordinates -- not goals
	toolbox->lower_left_corner(toolbox_llx,toolbox_lly);
	city_coordinate toolbox_delta_x = new_toolbox_x-toolbox_llx;
	city_coordinate toolbox_delta_y = new_toolbox_y-toolbox_lly;
	dampen_big_deltas(toolbox_delta_x,toolbox_delta_y);
// must scale helicopter, houses, etc. with y as well (or change vertical scrolling)
//	if (scale_with_y) {
		// ranges from 50 to 40% as it moves back 
//		long percent = 50-((toolbox_y+toolbox_delta_y-min_y)*10)/(max_y-min_y);
//		toolbox->set_size_percent(percent);
//	};
//	if (tt_toolbox != NULL) {
//		toolbox->set_size_percent(tt_toolbox->get_size_percent());
//	};
	toolbox->set_parameter((short) current_direction);
	toolbox->update_display(toolbox_delta_x,toolbox_delta_y,
									moved?UPDATE_CHANGE:NO_UPDATE_CHANGE);
	toolbox_x = new_toolbox_x;  
	toolbox_y = new_toolbox_y;  
// cure worse than problem:
//	city_coordinate lly = appearance->current_lly();
//	if (toolbox_y < lly) { // so a house or something doesn't get inbetween
//		toolbox->set_priority(lly-1);
//	} else {
//		toolbox->set_priority(lly+1);
//	};
	// another try
	city_coordinate lly = appearance->current_lly();
	if (toolbox_y < lly) { // so a house or something doesn't get inbetween
		if (appearance->priority() == min_long) { // new on 021205 to fix bug reported by Leonel on Nov 22
			appearance->set_priority(min_long+2); // so the following works OK (and an extra just in case for time travel buttons)
		};
		toolbox->set_priority(appearance->priority()-1);
	} else {
		toolbox->set_priority(appearance->priority()+1);
	};
};

void Programmer_Walking::cleanup(Screen *screen, ProgrammerStatus next_status) {
	Programmer_State::cleanup(screen,next_status);
	if (toolbox != NULL) screen->remove(toolbox);
	if (tt_puzzles_over_html_file != NULL) { // new on 080500 - leaving room after reaching last puzzle
		show_puzzles_over_html();
	};
};

boolean good_collision(SpritePointer &collider, TTRegion &region) {
   if (!collider->is_rectangle()) {
      Sprites *remaining = collider->pointer_to_followers();
      TTRegion sub_region;
      while (remaining != NULL) {
         Sprite *follower = remaining->first();
         follower->full_region(sub_region);
         if (sub_region.overlaps(region) && good_collision(follower,region)) {
            collider = follower;
            return(TRUE);
         };
         remaining = remaining->rest();
      };
      return(FALSE);
	} else {
// commented out the following lines on Sept 1 '98
//		if (collider->show_all() || collider->pointer_to_followers() == NULL) {
      return(TRUE);
//   } else {
//      return(FALSE);
   };
};

boolean Programmer_Walking::handle_collisions(city_coordinate &delta_x,
															 city_coordinate &delta_y,
															 ProgrammerStatus &status) {
//	city_coordinate original_delta_x = delta_x;
//	city_coordinate original_delta_y = delta_y;
   if (kind_of() == PROGRAMMER_ROOM_WALKING && tt_system_mode != PUZZLE) return(FALSE); // no point checking
	boolean changed = FALSE;
	TTRegion region;
	appearance->collision_region(region);
//	TTRegion scratch = original_region;
	// region plus movement
	if (delta_x > 0) {
		region.max_x += delta_x;
	} else if (delta_x < 0) {
		region.min_x += delta_x;
	};
	if (delta_y > 0) {
		region.max_y += delta_y;
	} else if (delta_y < 0) {
		region.min_y += delta_y;
	};
	Sprites *colliders = tt_screen->all_to_collide_with(&region);
	if (colliders != NULL && 
		 (colliders->member_of_type(DOOR) != NULL ||
		  colliders->member_of_type(OPEN_DOOR) != NULL)) {
		if (delta_y < 0) { // moving south so ignore collision with door
			colliders = colliders->remove_type(DOOR);			
		} else { // ignore house if also colliding with door
			colliders = colliders->remove_type(HOUSE);
		};
	 };
	Sprites *remaining = colliders;
	while (remaining != NULL) {
		Sprite *collider = remaining->first();
//      if (collider->is_outside_collection()) {
//          TTRegion picture_region;
//          Sprites *remaining = collider->pointer_to_followers();
//          while (remaining != NULL) {
//             Sprite *picture = remaining->first();
//             if (picture->visible()) {
//                picture->collision_region(picture_region);
//                if (region.overlaps(picture_region)) {
//                    colliding_with(remaining->first(),delta_x,delta_y,changed,status);
//                };
//                remaining = remaining->rest();
//             };
//          };
//      } else
      if (collider != appearance && 
			 // graspable condition added on 060605 since if glued down then should be able to walk all over it
          (collider->kind_of() != PICTURE || (good_collision(collider,region) && collider->graspable()))) {
			colliding_with(collider,delta_x,delta_y,changed,status);
      };
		remaining->set_first(NULL);
		remaining = remaining->rest();
	};
#if TT_DIRECT_INPUT
   if (!changed) {
      not_colliding();
   };
#endif
	delete colliders; // each element should be NULL
	return(changed);
};

void Programmer_Walking::step_sound() {
	// rewritten to use resource_index so can just save an integer in XML
	int current_appearance_image_index = appearance->pointer_to_current_image()->return_resource_index();
	if (previous_appearance_image_index != current_appearance_image_index) {
		previous_appearance_image_index = current_appearance_image_index;
		image_change_count++;
		make_step_sound();
	};
};

void Programmer_Walking::make_step_sound() {
  if (image_change_count%2 == 0) { // every other frame
	  play_sound(STEP_SOUND);
  };
};

#if TT_XML

void Programmer_Walking::add_xml(xml_element *element, xml_document *document) {
	if (delta_x != 0) xml_set_attribute(element,L"DeltaX",delta_x);
	if (delta_y != 0) xml_set_attribute(element,L"DeltaY",delta_y);
	if (minimum_distance_to_reorient != 0) xml_set_attribute(element,L"MinimumDistanceToReorient",minimum_distance_to_reorient);
	if (toolbox_x != 0) xml_set_attribute(element,L"ToolboxX",toolbox_x);
	if (toolbox_y != 0) xml_set_attribute(element,L"ToolboxY",toolbox_y);
	if (previous_appearance_image_index >= 0) xml_set_attribute(element,L"PreviousAppearanceImageIndex",previous_appearance_image_index);
	if (image_change_count != 0) xml_set_attribute(element,L"ImageChangeCount",image_change_count);
	if (spinning) {
		xml_set_attribute(element,L"Spinning",1);
	};
	if (toolbox != 0) { // from the side that follows programmer around
		toolbox->xml_create_and_append_element(element,document,TRUE)->Release();
	};
	Programmer_State::add_xml(element,document);
};

boolean Programmer_Walking::handle_xml_tag(Tag tag, xml_node *node) {
	switch (tag) {
		case TOOLBOX_VIEWED_FROM_SIDE_TAG:
			if (toolbox != NULL) {
				toolbox->destroy();
			};
			toolbox = xml_load_sprite(node,tag);
			return(toolbox != NULL);
		case NO_MORE_TAGS:
			delta_x = xml_get_attribute_int(node,L"DeltaX",0); // prior to 210404 was delta_x
			delta_y = xml_get_attribute_int(node,L"DeltaY",0); // prior to 210404 was delta_y
			minimum_distance_to_reorient = xml_get_attribute_int(node,L"MinimumDistanceToReorient",0); // prior to 210404 was minimum_distance_to_reorient
			toolbox_x = xml_get_attribute_int(node,L"ToolboxX",0); // prior to 210404 was toolbox_x
			toolbox_y = xml_get_attribute_int(node,L"ToolboxY",0); // prior to 210404 was toolbox_y
			previous_appearance_image_index = xml_get_attribute_int(node,L"PreviousAppearanceImageIndex",0); // prior to 210404 was previous_appearance_image_index
			image_change_count = xml_get_attribute_int(node,L"ImageChangeCount",0); // prior to 210404 was image_change_count
			spinning = xml_get_attribute_int(node,L"Spinning",0);
			// and the following
		default:
			return(Programmer_State::handle_xml_tag(tag,node));
	};
};

void Programmer_Walking::activate() {
	if (toolbox == NULL) { // new on 220603 - happened during time travel
		add_toolbox();
	};
	if (background != NULL) { // can it be otherwise? -- moved up here on 090103 since city_walking needs to keep track of background as well
		tt_screen->switch_to(background,TRUE,FALSE);
		appearance->set_background(background);
		if (toolbox != NULL) { // conditional new on 101204 due to crash (issue 586) -- may have been vacuumed up
			toolbox->set_background(background);
		};
	};
	if (toolbox != NULL) { // conditional new on 101204 due to crash (issue 586) -- may have been vacuumed up
		tt_screen->add(toolbox);
	};
//	tt_screen->new_view(CAMERA_IN_FRONT);
	Programmer_State::activate();
};

#endif

Programmer_City_Walking::Programmer_City_Walking(city_coordinate initial_x,
																 city_coordinate initial_y,
																 Direction initial_direction,
//																 Programmer *the_programmer,
																 boolean dizzy_first) :
	 Programmer_Walking(tt_city) {
//	programmer = the_programmer;
	minimum_distance_to_reorient = tile_width;
	current_direction = initial_direction;
	if (dizzy_first) {   // obsolete now
		SpriteCode dizzy_code;
		switch (tt_programmer_head) {
			case 0:
				dizzy_code = DIZZY_PERSON_SPRITE;
				break;
			case 1:
				dizzy_code = DIZZY_PERSON_WITH_HAIR_SPRITE;
				break;
			default: // should only be 2
				dizzy_code = DIZZY_PERSON_WITH_HAT_SPRITE;
				break;
		};
		appearance = new Programmer_Sprite(dizzy_code,initial_x,initial_y,tt_city);
	} else {
		appearance = new Programmer_Sprite(PERSON_WALKING_SPRITE,initial_x,initial_y,tt_city,(short) current_direction);
		appearance->set_ideal_speed(appearance->current_height()/4);
	};
};

void Programmer_City_Walking::finish_initializing() {
   // separated from the above so that appearance is there when switching backgrounds but
   // this is after screen as finished switching
   long percent = 50;
	appearance->set_size_percent(percent);
//	appearance->set_size_porportional_to_screen_size(TRUE);
	// true but handled specially by handle_collisions
//	appearance->set_motion_extends_collision_region(TRUE);
	// speed is height per second
	if (add_toolbox()) {
      int x_percent = (percent*tt_toolbox->get_width_percent())/100;
      int y_percent = (percent*tt_toolbox->get_height_percent())/100;
      if (x_percent < 1) x_percent = 1;
      if (y_percent < 1) y_percent = 1;
      while (x_percent < 10 || y_percent < 10) { // otherwise too small to see
         x_percent += x_percent;
         y_percent += y_percent;
      };
	   toolbox->set_size_percents(x_percent,y_percent);
   };
   city_coordinate center_view_x, center_view_y;
	city_scale scale;
	tt_screen->viewing_region(center_view_x,center_view_y,scale);
	tt_city->extent(city_min_x,city_max_x,city_min_y,city_max_y);
	min_x = center_view_x-8*tile_width;
	max_x = center_view_x+6*tile_width;
	min_y = center_view_y-9*tile_height;
	max_y = center_view_y+2*tile_height;
//   if (tt_log_version_number > 8) {
      max_x_speed = ideal_screen_width;
      max_y_speed = ideal_screen_height;
//   } else {
//	   max_x_speed = 15*tile_width; // 1.33 seconds to cross the screen
//	   max_y_speed = 15*tile_height;
//   };
	appearance->set_priority(appearance->current_lly()); // fix bug where first frame out of door cut off by door
#if TT_DIRECT_INPUT
   set_joystick_sensitivity(150);
#endif
   tt_screen->add(appearance);
	tt_city->set_containment();
};

ProgrammerStatus Programmer_City_Walking::react(boolean ,
														      unsigned char key,
														      boolean extended_key,
														      city_coordinate new_delta_x,
														      city_coordinate new_delta_y,
														      char button_status,
														      boolean ,
														      millisecond duration,
														      boolean ) {
	ProgrammerStatus status = PROGRAMMER_NORMAL;
	if (tt_programmer->invisible()) return(status); // new on 221299
	delta_x = new_delta_x;
	delta_y = new_delta_y;
	boolean moved = (delta_x != 0) || (delta_y != 0);
	Direction previous_direction = current_direction;
	boolean collision_caused_movement = FALSE;
	boolean dizzy = (appearance->appearance_code() != PERSON_WALKING_SPRITE);
	city_coordinate x,y;
	if (moved) {
		spinning = FALSE;
		appearance->lower_left_corner(x,y);
		if (dizzy) {
			swap_appearance(PERSON_WALKING_SPRITE,(short) current_direction);
         //new Programmer_Sprite(PERSON_WALKING_SPRITE,x,y,programmer,tt_city,(short) current_direction));
			appearance->set_ideal_speed(appearance->current_height()/2);
			dizzy = FALSE;
		};
		dampen_big_deltas(delta_x,delta_y);
		x += delta_x;		
		y += delta_y;
		step_sound();
		minimum_distance_to_reorient -= labs(delta_x);
		minimum_distance_to_reorient -= labs(delta_y);
		if (minimum_distance_to_reorient < 0) {
			minimum_distance_to_reorient = tile_width;
			Direction new_direction = direction(delta_x,delta_y);
			current_direction = dampen_turn(new_direction,current_direction);
		};
      Direction wall_direction = NO_DIRECTION;
		if (x < city_min_x) {
			delta_x += city_min_x-x;
			x = city_min_x;
         wall_direction = WEST;
		} else if (x > city_max_x) {
			delta_x += city_max_x-x;
			x = city_max_x;
         wall_direction = EAST;
		};
		if (y < city_min_y) {
			delta_y += city_min_y-y;
			y = city_min_y;
         wall_direction = SOUTH;
		} else if (y > city_max_y) {
			delta_y += city_max_y-y;
			y = city_max_y;
         wall_direction = NORTH;
		};
		city_coordinate x_adjustment = 0;
		if (x < min_x) {
			x_adjustment = x-min_x;
		} else if (x > max_x) {
			x_adjustment = x-max_x;
		};
		city_coordinate y_adjustment = 0;
		if (y < min_y) {
			y_adjustment = y-min_y;
		} else if (y > max_y) {
			y_adjustment = y-max_y;
		};
		if (x_adjustment != 0 || y_adjustment != 0) {
			// too close to an edge
			min_x += x_adjustment;
			max_x += x_adjustment;
			min_y += y_adjustment;
			max_y += y_adjustment;
			tt_screen->move_by(x_adjustment,y_adjustment);
		};
#if TT_DIRECT_INPUT
      play_wall_force_feedback_effect(wall_direction);
#endif
//		appearance->lower_left_corner(x,y);
		if (duration > 0) { // not on auto pilot
			collision_caused_movement = handle_collisions(delta_x,delta_y,status);
		};
//		long percent = 50-((y+delta_y-min_y)*10)/(max_y-min_y);
//		appearance->set_size_percent(percent);
//		toolbox->set_size_percent(percent);
	};
   boolean button_clicked = (right_button(button_status) || 
									  (left_button(button_status) && tt_mouse_mode == RELATIVE_MOUSE_MODE));
   // beginning 030502 don't count left mouse clicks here if in absolute or directional pad mode
	if (key == '\026' && !extended_key) { // control-v added on 030200 --  && !extended_key new on 300904
		add_item_from_clipboard(NULL,appearance,key);
	} else if (key == '\003' && !extended_key) { // control-c added on 300904
		// copied here so can copy entire city
		put_on_clipboard(NULL,appearance->pointer_to_background(),appearance,key);
		play_sound(POP_SOUND);
   } else if (// button_clicked || commented out on 230902 since also re-ordered this so that the 'h' key works
		        (!extended_key && keyboard_accelerator(key,CALL_FOR_HELICOPTER_IF_WALKING)) // !extended_key added on 090804
				  || (extended_key && key == VK_F1)) {
		// or as David suggested truck it over
		// why button_clicked??
		status = NEED_HELICOPTER;
		simulate_mouse_up();
   } else if (tt_system_mode != PUZZLE &&
              (button_clicked || 
               middle_button(button_status) ||
               (key != 0 && !extended_key))) { // updated on 090502 to include key strokes and middle button
      status = SITTING_DOWN_OUTSIDE;
   } else {
	   // easiest way to say not to cache them
	   appearance->set_clean_status(MOVED);
   };
	if (toolbox != NULL) toolbox->set_clean_status(MOVED);	
	if (dizzy) {
		appearance->update_display();
	} else {
		if (spinning) {
			appearance->set_parameter((short) ((tt_current_time%500)/63)); // spin around twice in a second
		} else {
			appearance->set_parameter((short) current_direction);
		};
		appearance->update_display(delta_x,delta_y,
											moved?UPDATE_CHANGE:NO_UPDATE_CHANGE);
	};
	if (!collision_caused_movement) {
		update_toolbox(previous_direction,delta_x,delta_y,moved,FALSE,TRUE);
	} else if (toolbox != NULL) {
		toolbox->update_display();
	};
	appearance->lower_left_corner(x,y);
	tt_programmer->set_saved_city_location(x,y);
#if TT_DEBUG_ON
	if (tt_debug_mode == 190404) {
		tt_error_file() << "Programmer_City_Walking::react on frame " << tt_frame_number << " segment is " << tt_current_log_segment << endl;
		tt_error_file() << "saved_city_x=" << x << " ;saved_city_y=" << y << endl;
	};
#endif
#if TT_DEBUG_ON
   if (moved && tt_debug_mode == 30502) {
      tt_error_file() << "appearance x and y = " << x << "," << y << " on frame " << tt_frame_number << endl;
   };
#endif
//	if (!extended_key && key == VK_ESCAPE) toggle_pause();
	return(status);
};

void Programmer_City_Walking::bumped_into_house(Direction direction) {
#if TT_DIRECT_INPUT
   play_wall_force_feedback_effect(direction);
#endif
};

#if TT_DIRECT_INPUT
void Programmer_City_Walking::not_colliding() {
   stop_force_feedback_effect();
};
#endif

#if TT_XML

void Programmer_City_Walking::add_xml(xml_element *element, xml_document *document) {
	xml_set_attribute(element,L"CityMinX",city_min_x); 
	xml_set_attribute(element,L"CityMaxX",city_max_x);
	xml_set_attribute(element,L"CityMinY",city_min_y);
	xml_set_attribute(element,L"CityMaxY",city_max_y);
	Programmer_Walking::add_xml(element,document);
};

boolean Programmer_City_Walking::handle_xml_tag(Tag tag, xml_node *node) {
	if (tag == NO_MORE_TAGS) {
		city_min_x = xml_get_attribute_int(node,L"CityMinX",city_min_x);
		city_max_x = xml_get_attribute_int(node,L"CityMaxX",city_max_x); 
		city_min_y = xml_get_attribute_int(node,L"CityMinY",city_min_y); 
		city_max_y = xml_get_attribute_int(node,L"CityMaxY",city_max_y);
	};
	return(Programmer_Walking::handle_xml_tag(tag,node));
};

//void Programmer_City_Walking::activate() {
//	city_coordinate x,y;
//	position(x,y);
//	tt_screen->update_viewing_region(x,y,ground_scale); 
//};

#endif

Programmer_Room_Walking::Programmer_Room_Walking(Room *room,
					                                  city_coordinate initial_x, city_coordinate initial_y,
//					                                  Programmer *the_programmer,
					                                  boolean entering,
                                                 Direction starting_direction) :
	Programmer_Walking(room) {
//	room(room) {
//	programmer = the_programmer;
   current_direction = starting_direction;
	minimum_distance_to_reorient = tile_width;
//	x = initial_x;
//	y = initial_y;
//   if (tt_log_version_number > 9) {
	   min_x = 0;
//   } else {
//      min_x = tile_width/2;
//   };
//   if (tt_log_version_number > 9) {
      max_x = 16*tile_width; // to reach stuff that hit the edge
//   } else {
//	   max_x = 15*tile_width;
//   };
	min_y = -4*tile_height;
	max_y = 6*tile_height;
   if (room->pointer_to_house() == pointer_to_rocket()) {
       max_y = 3*tile_height;
   };
//   if (tt_log_version_number > 8) {
      max_x_speed = ideal_screen_width; // 1 second
      max_y_speed = ideal_screen_height;
//   } else {
//	   max_x_speed = 8*tile_width; // 2.5 seconds to cross the screen
//	   max_y_speed = 8*tile_height;
//   };
	if (entering) {
		current_direction = starting_direction;
		appearance = new Programmer_Sprite(PERSON_WALKING_SPRITE,
													  initial_x,initial_y,
													  background,(short) current_direction);
	// true but handled specially by handle_collisions
//		appearance->set_motion_extends_collision_region(TRUE);
		appearance->set_ideal_speed(appearance->current_height()/2);
	} else {
		current_direction = NORTH; // for toolbox at least
      short int sit_state;
      if (tt_programmer->return_tool_in_hand() == NULL) {
         sit_state = MAN_STAND_UP;
      } else {
         sit_state = MAN_STAND_AND_POCKET;
      };
		appearance = new Programmer_Sprite(sit_sprite(),
													  initial_x,initial_y,
													  background,sit_state);
	};
};

void Programmer_Room_Walking::finish_initializing() {
   appearance->set_priority_fixed(TRUE); // so he can step on everything on the floor
   appearance->set_priority(programmer_room_walking_priority);
	if (add_toolbox()) {
 	   toolbox->set_size_percents(tt_toolbox->get_width_percent(),
                                 tt_toolbox->get_height_percent());
	};
   TTRegion containment_region;
   containment_region.min_x = left_room_margin;
   containment_region.min_y = 0;
   containment_region.max_x = (floor_width_percent*ideal_screen_width)/100+left_room_margin;
   containment_region.max_y = (floor_height_percent*ideal_screen_height)/100;
   background->set_containment_region(containment_region);
   tt_screen->add(appearance);
};

// how to have this share more with walking outside??
ProgrammerStatus Programmer_Room_Walking::react(boolean ,
																unsigned char key,
																boolean extended_key,
																city_coordinate new_delta_x,
																city_coordinate new_delta_y,
																char button_status,
																boolean button_unchanged,
																millisecond duration,
																boolean) {
	ProgrammerStatus status = PROGRAMMER_NORMAL;
	if (tt_programmer->invisible()) { 
		return(status); // new on 221299 so if invisible can play games, etc.
	};
	delta_x = new_delta_x;
	delta_y = new_delta_y;
	boolean moved = (delta_x != 0) || (delta_y != 0);
	Direction previous_direction = current_direction;
	boolean collision_caused_movement = FALSE;
   boolean sit_down = FALSE; // new on 100602 so if in absolute mouse mode you click on the floor you sit
	city_coordinate x,y;
	appearance->lower_left_corner(x,y);
	if (moved) {
		dampen_big_deltas(delta_x,delta_y);
		x += delta_x;
		y += delta_y;
		step_sound();
		minimum_distance_to_reorient -= labs(delta_x);
		minimum_distance_to_reorient -= labs(delta_y);
		if (minimum_distance_to_reorient < 0) {
			minimum_distance_to_reorient = tile_width;
			Direction new_direction = direction(delta_x,delta_y);
			current_direction = dampen_turn(new_direction,current_direction);
		};
#if TT_DIRECT_INPUT
      Direction direction = NO_DIRECTION;
#endif
		if (x < min_x) {
			delta_x += min_x-x;
			x = min_x;
			status = LEAVING_ROOM;
		} else if (x > max_x) {
#if TT_DIRECT_INPUT
         direction = EAST;
#endif
			delta_x -= x-max_x;
			x = max_x;
		};
		if (y < min_y) {
			delta_y += min_y-y;
			y = min_y;
         sit_down = absolute_mouse_mode();
         if (sit_down) {
            y = y+tile_height; // add a tile_height on 210602 so that doesn't bump against the bottom (south) wall
            // though the problem remains if the mouse cursor stays at the very bottom
         };
#if TT_DIRECT_INPUT
         direction = SOUTH;
#endif
		} else if (y > max_y) {
			delta_y -= y-max_y;
			y = max_y;
#if TT_DIRECT_INPUT
         direction = NORTH;
#endif
		};
		if (duration > 0) { // not on auto pilot
			collision_caused_movement = handle_collisions(delta_x,delta_y,status);
		};
#if TT_DIRECT_INPUT
      play_wall_force_feedback_effect(direction);
#endif
	};
	if (key == '\026' && !extended_key) { // control-v added on 030200 --  && !extended_key new on 300904
		add_item_from_clipboard(NULL,appearance,key);
   } else if (key == '\003' && !extended_key) { // control-c added on 030200 to be able to save an entire house -- && !extended_key added 300904
      put_on_clipboard(NULL,background,appearance,key);
		play_sound(POP_SOUND);
	} else if (right_button(button_status) || 
              (left_button(button_status) && tt_mouse_mode == RELATIVE_MOUSE_MODE) || 
              middle_button(button_status) ||
              (key != 0 && !extended_key) ||
              sit_down) {
      // beginning 030502 don't count left mouse clicks here if in absolute or directional pad mode
      if (inside_rocket()) {
//         play_sound(PLOP_SOUND);
          if (key != 0 || !button_unchanged) {
             tt_martian->repeat_or_speed_up();
          };
      } else {
		   status = SITTING_DOWN;
//		   tt_screen->remove(appearance);
//		   appearance->destroy();
         short int sit_state;
         if (tt_programmer->return_tool_in_hand() == NULL) {
            sit_state = MAN_SIT_DOWN;
         } else {
            sit_state = MAN_SIT_TAKE_OUT_OF_POCKET;
         };
//		   Sprite *appearance = new Programmer_Sprite(sit_sprite(),x,y,
//													     programmer,room,sit_state);
         appearance->set_priority_fixed(TRUE); // so he can step on everything on the floor
         appearance->set_priority(programmer_room_walking_priority);
//		   tt_screen->add(appearance);
         swap_appearance(sit_sprite(),sit_state);
         appearance->move_to(x,y);
      };
	};
	// easiest way to say not to cache them
	appearance->set_clean_status(MOVED);
	if (toolbox != NULL) toolbox->set_clean_status(MOVED);
	SpriteCode code = appearance->appearance_code();
	if (code == SIT_SPRITE ||
		 code == SIT_WITH_HAIR_SPRITE ||
		 code == SIT_WITH_HAT_SPRITE) {
		appearance->update_display(delta_x,delta_y);
	} else {
		appearance->set_parameter((short) current_direction); 
		appearance->update_display(delta_x,delta_y,
											moved?UPDATE_CHANGE:NO_UPDATE_CHANGE);
	};
	if (!collision_caused_movement) {
		update_toolbox(previous_direction,delta_x,delta_y,moved,TRUE,FALSE);
	} else if (toolbox != NULL) {
		toolbox->update_display();
	};
	if (extended_key) {
		switch (key) {
			case VK_F1:
            if (tt_system_mode == PUZZLE && inside_rocket()) {
               tt_martian->repeat_last_thing_said();
            } else if (tt_system_mode != PUZZLE) {
               tt_martian->teleport_toggle(NULL,NULL);
            };
				break;
         case VK_NEXT: // page down
            user_did(GLOBAL_HISTORY,USER_HAS_TYPED_PAGE_DOWN);
            tt_martian->dont_wait_to_say_more();
            break;
         case VK_PRIOR: // page up
//            user_did(GLOBAL_HISTORY,USER_HAS_TYPED_PAGE_UP);
            tt_martian->repeat_last_thing_said();
            break;
			case VK_F7:
				tt_finish_all_animations = TRUE;
				user_did(GLOBAL_HISTORY,USER_HAS_HURRIED_UP,appearance);
				break;
//         case VK_F6: // save room
//            room->dump();
//            break;
		};
	};
#if TT_DIRECT_INPUT
   set_joystick_sensitivity(50);
#endif
	return(status);
};

void Programmer_Room_Walking::make_step_sound() {
  if (image_change_count%4 == 0) { // every fourth frame
	  play_sound(WALKING_INSIDE_SOUND); 
  };
};

//Room *Programmer_Room_Walking::pointer_to_room() {
//	// beginning 090103 compute this rather than store it
//	return(appearance->pointer_to_background()->pointer_to_room());
//};

//void Programmer_Room_Walking::activate() {
//	if (room != NULL) { // can it be otherwise?
//		tt_screen->switch_to(room,TRUE,FALSE);
//		appearance->set_background(room);
//		toolbox->set_background(room);
//	};
//	Programmer_Walking::activate();
//};

//flag programmer_speed_set = FALSE;

//boolean programmer_speed_available() {
//   return(programmer_speed_set);
//};

Programmer_At_Floor::Programmer_At_Floor(Floor *floor,
//													  Programmer *the_programmer,
													  city_coordinate initial_x,
													  city_coordinate initial_y,
													  HandState hand_state,
													  Sprite *tool_in_hand) :
	hand(hand_state),
	tool_in_hand(tool_in_hand), // tool_in_hand),
	using_arm(TRUE),
	bombed(FALSE),
	current_selection(NULL),
	saved_selection(NULL),
//	selection_delta_x(0),
//	selection_delta_y(0),
	original_thought_bubble(NULL),
	thought_bubble_robot(NULL), // wasn't initialized prior to 060103
	floor(floor),
	delta_index(0),
	selection_feedback_flag(TRUE),
	thought_bubble_added_to_floor(FALSE),
//	game_element_in_hand_added(FALSE),
	moved(TRUE),
	thought_bubble_entered(NULL),
   cubby_that_started_training(NULL),
	waiting_for(NULL),
	retraining_stopped_on_frame(0),
	ignore_underneath_on_next_drop(FALSE),
	give_toss_on_next_drop(FALSE),
// distance_dragged(0), 
   move_count(0),
   use_selection_region(FALSE),
   time_of_next_scroll_with_absolute_mouse(0),
	thought_bubble_llx(0), // wasn't initialized prior to 050103
	thought_bubble_lly(0),
	saved_button_unchanged(FALSE),
	saved_modified(FALSE),
	saved_reason(NO_SELECTION_REASON_GIVEN),
	release_time(0),
	last_released(NULL),
	current_delta_x(0),
	current_delta_y(0) {
		// following fixes a serious bug if time travel starts with helicopter landing 
		// and then one goes backward to being on the floor - new on 150204
	 tt_screen->set_city_to_user_units(ideal_screen_width/1000,ideal_screen_height/1000);
//	programmer = the_programmer;
//   if (tt_log_version_number < 7) {
//   	min_x = 2*tile_width;
//	   min_y = 2*tile_height;
//   };
//	max_y = ideal_screen_height; //20*tile_height;
//   if (tt_log_version_number > 8) {
	   max_x_speed = 3*ideal_screen_width; // 1/3 seconds to cross screen
	   max_y_speed = 3*ideal_screen_height;
//   } else {
//	   max_x_speed = ideal_screen_width; // was 1 second
//	   max_y_speed = ideal_screen_height;
//   };
	appearance = new Programmer_Sprite(HAND_SPRITE,initial_x,initial_y,floor);
	hand_appearance = appearance;
#if TT_DEBUG_ON
	if (tt_debug_mode == 300803) {
		tt_debug_target = hand_appearance->debug_counter;
	};
#endif
	appearance->set_priority(programmer_hand_priority);
//	appearance->set_followers_on_top(FALSE);
//	appearance->set_priority_function_of_lly(FALSE); // .mkb file should
	hand_with_wand = new Programmer_Sprite(HAND_HOLDING_WAND_SPRITE,0,0,floor);
	hand_with_wand->set_priority(programmer_hand_priority);
	if (hand_state == HOLDING_COPIER) { // new on 110200 to fix bug where wand was invisible if you stood up with it
		set_appearance(hand_with_wand);
		appearance->set_parameter(HAND_GRASP_WAND);
	};
	arm = new Sprite(ARM_ONLY_SPRITE,0,0,PROGRAMMER_ARM);
	arm->update_display(); // new on 180803 otherwise can be invisible
	arm->set_background(floor); // new on 080103 since otherwise doesn't dump with geometry
	hand_with_wand->set_background(floor); // new on 180103 since otherwise doesn't dump with geometry
	// note that the following is needed only if video mode greater than 640 even is running with m22
	if (tt_good_sizes_adjust_to_screen_size) { // new on 190999
		hand_appearance->set_size_percent(shrink_percent_from_640x480_screen_size(100));
		hand_with_wand->set_size_percent(shrink_percent_from_640x480_screen_size(100));
		arm->set_width(shrink_width_from_640x480_screen_size(arm->current_width()));
	};
   if (tool_in_hand != NULL) {
//      tool_in_hand->recursively_set_background(floor);
      floor->add_item(tool_in_hand);
   };
//	arm->set_priority(programmer_hand_priority);
//	arm->set_parameter(1); // lengthened version -- flush the short one?
//	tt_screen->add(arm);
/*
	if (hand != EMPTY_HAND) {
		tool_in_hand->recursively_set_background(floor);
		// not using add_follower since only want some of the functionality
		appearance->set_followers(new Sprites(tool_in_hand));
		tool_in_hand->set_leader(appearance);
//      tool_in_hand->set_screen_updates(FALSE);
		recursively_restore_leaders(appearance,floor);
//      appearance->propagate_changes();
		floor->add_item(tool_in_hand,FALSE);
//      tool_in_hand->update_display();
//      grasp_x_offset = the_grasp_x_offset;
//      grasp_y_offset = the_grasp_y_offset;
//      tool_in_hand->move_to(x+grasp_x_offset,y+grasp_y_offset);
	};
*/
   set_mouse_mode(tt_mouse_mode_on_floor);
   if (tt_mouse_mode_on_floor == ABSOLUTE_MOUSE_MODE) { // new on 250402
      tt_hand_needs_to_move_relatively = FALSE; // no longer relative
      set_last_mouse_deltas(0,0);
   };
   if (!relative_mouse_mode()) {
      show_cursor(TRUE);
   };
	for (int i = 0; i < number_of_deltas; i++) { // this was in Programmer_At_Floor::finish_initializing() prior to 130703
		delta_xs[i] = 0;
		delta_ys[i] = 0;
		delta_times[i] = 0;
	};
//	tt_toolbox->check_tt_file_name_to_clipboard(); // new on 030903 -- can cause crashes while loading a city - moved on 050903
};

#if TT_XML

void Programmer_At_Floor::add_xml(xml_element *element, xml_document *document) {
	if (floor != NULL && ((thought_bubble_entered != NULL && thought_bubble_entered->pointer_to_background() == NULL) || floor->on_the_ground())) { 
		// added thought_bubble_entered->pointer_to_background() == NULL on 130703 since don't want to save this while thought bubble is expanding
		xml_append_child(floor->xml(document),element); // save the thought bubble floor or the ground
	};
	if (hand != EMPTY_HAND) {
		xml_set_attribute(element,L"HandCode",(int) hand);
	};
	if (!using_arm) {
		xml_set_attribute(element,L"UsingArm",0);
	};
	if (bombed) {
		xml_set_attribute(element,L"Bombed",1);
	};
	if (tool_in_hand != NULL) {
		tool_in_hand->add_xml(element,L"InHand",document);
	};
	if (thought_bubble_entered != NULL) {
		thought_bubble_entered->add_xml(element,L"ThoughtBubbleEntered",document);
	};
	if (original_thought_bubble != NULL) {
		original_thought_bubble->add_xml(element,L"OriginalThoughtBubble",document);
	};
	if (thought_bubble_added_to_floor) {
		xml_set_attribute(element,L"ThoughtBubbleAddedToFloor",1);
	};
	if (thought_bubble_llx != 0) {
		xml_set_attribute(element,L"ThoughtBubbleLLX",thought_bubble_llx);
	};
	if (thought_bubble_lly != 0) {
		xml_set_attribute(element,L"ThoughtBubbleLLY",thought_bubble_lly);
	};
	if (thought_bubble_robot != NULL) {
		thought_bubble_robot->add_xml(element,L"ThoughtBubbleRobot",document); // added FALSE on 110703 so doesn't generate an include file
	};
	// following taken care of by Programmer_State::add_xml
	//if (appearance != NULL) { // can it be otherwise?
	//	appearance->add_xml(element,L"ProgrammerAppearance",document);
	//};
	if (hand_appearance != NULL) {
		hand_appearance->xml_create_and_append_element(element,document,FALSE)->Release(); // added FALSE on 100703 since can't be shared
	};
	if (hand_with_wand != NULL) {
		hand_with_wand->xml_create_and_append_element(element,document,FALSE)->Release(); // added FALSE on 100703 since can't be shared
	};
	if (arm != NULL) {
		arm->xml_create_and_append_element(element,document,FALSE)->Release(); // added FALSE on 100703 since can't be shared
	};
	if (saved_button_unchanged) {
		xml_set_attribute(element,L"SavedButtonUnchanged",1);
	};
	// experiment on 260803 to not bother to save selections since they can be to a notebook's left or right page which is a pain to get right
	// restored on 280803 since now handle notebook left and right page specially
	if (current_selection != NULL) {
		current_selection->add_xml(element,L"CurrentSelection",document);
	};
	if (saved_selection != NULL) {
		saved_selection->add_xml(element,L"SavedSelection",document);
	};
	if (saved_modified) {
		xml_set_attribute(element,L"SavedModifiied",1);
	};
	if (saved_reason != NO_SELECTION_REASON_GIVEN) {
		xml_set_attribute(element,L"SavedReasonCode",(int) saved_reason);
	};
	if (delta_index != 0) {
		xml_set_attribute(element,L"DeltaIndex",delta_index);
	};
	if (release_time != 0) {
		xml_set_attribute(element,L"ReleaseTime",release_time);
	};
	if (!selection_feedback_flag) {
		xml_set_attribute(element,L"SelectionFeedback",0);
	};
	xml_set_attribute(element,L"CurrentRegionMinX",current_region.min_x);
	xml_set_attribute(element,L"CurrentRegionMaxX",current_region.max_x);
	xml_set_attribute(element,L"CurrentRegionMinY",current_region.min_y);
	xml_set_attribute(element,L"CurrentRegionMaxY",current_region.max_y);
	if (!moved) {
		xml_set_attribute(element,L"Moved",0);
	};
	if (last_released != NULL) {
		last_released->add_xml(element,L"LastReleased",document);
	};
	if (cubby_that_started_training != NULL) {
		cubby_that_started_training->add_xml(element,L"BoxThatStartedTraining",document);
	};
	if (waiting_for != NULL) {
		waiting_for->add_xml(element,L"WaitingFor",document);
	};
	if (retraining_stopped_on_frame != 0) {
		xml_set_attribute(element,L"RetrainingStoppedOnFrame",retraining_stopped_on_frame);
	};
	if (ignore_underneath_on_next_drop) {
		xml_set_attribute(element,L"IgnoreUnderneathOnNextDrop",1);
	};
	if (give_toss_on_next_drop) {
		xml_set_attribute(element,L"GiveTossOnNextDrop",1);
	};
	if (use_selection_region) {
		xml_set_attribute(element,L"UseSelectionRegion",1);
		// moved into conditional on 060103 since no point saving this otherwise
		xml_set_attribute(element,L"SelectionRegionMinX",selection_region.min_x);
		xml_set_attribute(element,L"SelectionRegionMaxX",selection_region.max_x);
		xml_set_attribute(element,L"SelectionRegionMinY",selection_region.min_y);
		xml_set_attribute(element,L"SelectionRegionMaxY",selection_region.max_y);
	};
	if (move_count != 0) {
		xml_set_attribute(element,L"MoveCount",move_count);
	};
	if (time_of_next_scroll_with_absolute_mouse != 0) {
		xml_set_attribute(element,L"TimeOfNextScrollWithAbsoluteMouse",time_of_next_scroll_with_absolute_mouse);
	};
   // moved here on 050103 since only used and saved if programmer at floor
//	xml_element *deltas_element = create_xml_element(L"RecentMovements",document);
//	xml_append_child(deltas_element,element);
   for (int i = 0; i < number_of_deltas; i++) {
      if (delta_xs[i] != 0 || delta_ys[i] != 0) { // otherwise why bother
         xml_element *recording_element = create_xml_element(L"RecentMovement",document);
         xml_set_attribute(recording_element,L"Index",i);
         xml_set_attribute(recording_element,L"DeltaX",delta_xs[i]);
         xml_set_attribute(recording_element,L"DeltaY",delta_ys[i]);
         xml_set_attribute(recording_element,L"DeltaTime",delta_times[i]);
#if TT_DEBUG_ON
			if (delta_times[i] < 0) {
				debug_this();
			};
#endif
         xml_append_child(recording_element,element);
      };
   };
	Programmer_State::add_xml(element,document);
};

boolean Programmer_At_Floor::handle_xml_tag(Tag tag, xml_node *node) {
	switch (tag) {
      case RECENT_MOVEMENT_TAG: { // moved here on 050103 
         //if (saved_delta_xs == NULL) {
         //   initialize_saved_deltas();
         //};
         int index = xml_get_attribute_int(node,L"Index",0);
         delta_xs[index] = xml_get_attribute_int(node,L"DeltaX",0);
         delta_ys[index] = xml_get_attribute_int(node,L"DeltaY",0);
         delta_times[index] = xml_get_attribute_int(node,L"DeltaTime",0);
#if TT_DEBUG_ON
			if (delta_times[index] < 0) {
				debug_this();
			};
#endif
         return(TRUE);
                                };
		//case HOUSE_LOCATION_TAG: { // commented out since this comes from Programmer::handle_xml_tag
		//	House *house = house_at_location(node);
		//	if (house == NULL) return(FALSE);
		//	floor = house->pointer_to_floor();
		//	return(floor != NULL);
		//							  };
		case IN_HAND_TAG:
			if (tool_in_hand != NULL) {
				tool_in_hand->destroy();
			};
			tool_in_hand = xml_get_sprite(node); // don't want all the stuff that set_tool_in_hand does
			if (tool_in_hand != NULL) { // just to be safe
				tool_in_hand->update_display(); // new on 021203 -- this fixes a bug where Pumpy jumps during time travel (I believe)
				// the problem was that its true_width() and true_height() weren't right until first display but programmer reads them first
				// not clear if there are other cases like this
				tool_in_hand->set_held_status(HELD_BY_PROGRAMMER); // new on 180103
				tool_in_hand->set_background(floor,TRUE,FALSE); // new on 081004 since otherwise can cause time travel problems 
				// -- added FALSE above on 070305 to avoid spurious warnings
				// following new on 060605 so can load city if saved while holding BIG picture
				tt_programmer->set_width_at_time_of_grasp(tool_in_hand->current_width()); 
				tt_programmer->set_height_at_time_of_grasp(tool_in_hand->current_height());
#if TT_DEBUG_ON
			} else {
				log("Couldn't find thing in hand.");
#endif
			};
			return(TRUE);
		case THOUGHT_BUBBLE_ENTERED_TAG: 
			thought_bubble_entered = (Thought_Bubble *) xml_get_sprite(node,thought_bubble_entered,THOUGHT_BUBBLE);
			return(thought_bubble_entered != NULL);
		case ORIGINAL_THOUGHT_BUBBLE_TAG: 
			original_thought_bubble = (Thought_Bubble *) xml_get_sprite(node,original_thought_bubble,THOUGHT_BUBBLE);
			return(original_thought_bubble != NULL);
		case THOUGHT_BUBBLE_ROBOT_TAG: 
			thought_bubble_robot = (Robot *) xml_get_sprite(node,thought_bubble_robot,ROBOT);
#if TT_DEBUG_ON
			if (thought_bubble_robot != NULL && thought_bubble_robot->trashed()) {
				log("Just loaded thought_bubble_robot but it has been trashed.");
			};
#endif
			return(thought_bubble_robot != NULL);
		case HAND_TAG:
			if (hand_appearance != NULL) {
				hand_appearance->destroy();
			};
			hand_appearance = xml_load_sprite(node,tag);
			return(hand_appearance != NULL);
		case HAND_HOLDING_WAND_TAG:
			if (hand_with_wand != NULL) {
				hand_with_wand->destroy();
			};
			hand_with_wand = xml_load_sprite(node,tag);
			return(hand_with_wand != NULL);
		case ARM_TAG:
			if (arm != NULL) {
				arm->destroy();
			};
			arm = xml_load_sprite(node,tag);
			if (arm != NULL) {
				arm->update_display(); // new on 180803 - otherwise can be invisible while jumping between time travel segments
			};
			return(arm != NULL);
		case CURRENT_SELECTION_TAG:
			if (current_selection != NULL) {
				current_selection->destroy();
			};
			current_selection = xml_get_sprite(node);
			return(current_selection != NULL);
		case SAVED_SELECTION_TAG:
			if (saved_selection != NULL) {
				saved_selection->destroy();
			};
			saved_selection = xml_get_sprite(node);
			return(saved_selection != NULL);
		case LAST_RELEASED_TAG:
			if (last_released != NULL) {
				last_released->destroy();
			};
			last_released = xml_get_sprite(node);
			return(last_released != NULL);
		case BOX_THAT_STARTED_TRAINING_TAG: 
			cubby_that_started_training = (Cubby *) xml_get_sprite(node,cubby_that_started_training,CUBBY);
			return(cubby_that_started_training != NULL);
		case WAITING_FOR_TAG:
			if (waiting_for != NULL) {
				waiting_for->destroy();
			};
			waiting_for = xml_get_sprite(node);
			return(waiting_for != NULL);
		//case PROGRAMMER_APPEARANCE_TAG:
		//	if (!Programmer_State::handle_xml_tag(tag,node)) return(FALSE);
		//	// need to tell appearance about floor - new on 060103
		//	appearance->set_background(floor); // -- not clear if activate() does this better now
		//	return(TRUE);
		case FLOOR_TAG: 
//			if (floor == NULL) { // commented out on 240105 since this caused cities saved while training to have
			// a mixture of the thought bubble and floor "above"
				// condition new on 160105 -- can cause a crash with time travel since two floors think they share the same room
				floor = new Floor();
//			};
			return(xml_entity(node,floor));
		case GROUND_TAG: // obsolete??
			floor = new Ground();
			return(xml_entity(node,floor));
		case NO_MORE_TAGS:
			hand = (HandState) xml_get_attribute_int(node,L"HandCode",EMPTY_HAND);
			using_arm = xml_get_attribute_int(node,L"UsingArm",1);
			bombed = xml_get_attribute_int(node,L"Bombed",0);
			thought_bubble_added_to_floor = xml_get_attribute_int(node,L"ThoughtBubbleAddedToFloor",0);
			thought_bubble_llx = xml_get_attribute_int(node,L"ThoughtBubbleLLX",0); // prior to 210404 was thought_bubble_llx
			thought_bubble_lly = xml_get_attribute_int(node,L"ThoughtBubbleLLY",0); // prior to 210404 was thought_bubble_lly
			saved_button_unchanged = xml_get_attribute_int(node,L"SavedButtonUnchanged",0);
			saved_modified = xml_get_attribute_int(node,L"SavedModifiied",0);
			saved_reason = (SelectionReason) xml_get_attribute_int(node,L"SavedReasonCode",NO_SELECTION_REASON_GIVEN);
			delta_index = xml_get_attribute_int(node,L"DeltaIndex",0);
			release_time = xml_get_attribute_int(node,L"ReleaseTime",0);
			selection_feedback_flag = xml_get_attribute_int(node,L"SelectionFeedback",1);
			current_region.min_x = xml_get_attribute_int(node,L"CurrentRegionMinX",current_region.min_x);
			current_region.max_x = xml_get_attribute_int(node,L"CurrentRegionMaxX",current_region.max_x);
			current_region.min_y = xml_get_attribute_int(node,L"CurrentRegionMinY",current_region.min_y);
			current_region.max_y = xml_get_attribute_int(node,L"CurrentRegionMaxY",current_region.max_y);
			moved = xml_get_attribute_int(node,L"Moved",1);
			retraining_stopped_on_frame = xml_get_attribute_int(node,L"RetrainingStoppedOnFrame",0); 
			// prior to 210404 was retraining_stopped_on_frame
			ignore_underneath_on_next_drop = xml_get_attribute_int(node,L"IgnoreUnderneathOnNextDrop",0);
			give_toss_on_next_drop = xml_get_attribute_int(node,L"GiveTossOnNextDrop",0);
			use_selection_region = xml_get_attribute_int(node,L"UseSelectionRegion",0);
			if (use_selection_region) { // no need otherwise - new on 220803
				selection_region.min_x = xml_get_attribute_int(node,L"SelectionRegionMinX",selection_region.min_x);
				selection_region.max_x = xml_get_attribute_int(node,L"SelectionRegionMaxX",selection_region.max_x);
				selection_region.min_y = xml_get_attribute_int(node,L"SelectionRegionMinY",selection_region.min_y);
				selection_region.max_y = xml_get_attribute_int(node,L"SelectionRegionMaxY",selection_region.max_y);
#if TT_DEBUG_ON
				 if (tt_debug_mode == 220803) {
					  tt_error_file() << "On frame " << tt_frame_number << " loading selection region ";
					  current_region.print(tt_error_file());
					  tt_error_file() << endl;
				 };
#endif
			};
			move_count = xml_get_attribute_int(node,L"MoveCount",0);
			time_of_next_scroll_with_absolute_mouse = xml_get_attribute_int(node,L"TimeOfNextScrollWithAbsoluteMouse",0);
			// and the following
		default:
			return(Programmer_State::handle_xml_tag(tag,node));
	};
};

void Programmer_At_Floor::activate() { // new on 060103 -- called after loaded to connect things up
	if (floor == NULL) {
		return; // things are broken -- warn??
	};
	tt_screen->switch_to(floor,TRUE,FALSE);
	Programmer_State::activate();
//	tt_screen->new_view(CAMERA_MOSTLY_ABOVE);
//	floor->add_item(appearance); // new on 070103
	appearance->set_background(floor);
	if (hand_appearance != NULL) { // shouldn't be otherwise -- new on 180103
		hand_appearance->set_background(floor);
	};
	if (hand_with_wand != NULL) { // shouldn't be otherwise -- new on 180103
		hand_with_wand->set_background(floor);
	};
	if (arm != NULL) { // shouldn't be otherwise -- new on 180103
		arm->set_background(floor);
	};
	if (appearance->kind_of() == ROBOT) {
		appearance->set_sprite_type(ROBOT_IN_TRAINING);
	} else {
		if (tool_in_hand != NULL) {
			if (hand == USED_COPIER || hand == USING_COPIER) { // second disjunt new on 311003 or else you see 2 wands		 
			   Sprite *attached = ((Copier *) tool_in_hand)->attached_tool();
				if (attached != NULL) { // condition new on 311003 since if USING_COPIER there may not be anything attached
					tt_screen->add(attached);
					attached->set_background(floor,FALSE,FALSE); // new on 031103 so that if for example saved a city while copying a nest the bird will hatch
				};
				tool_in_hand->set_background(floor,FALSE,FALSE); // new on 031103
			} else if (hand != HOLDING_COPIER) { // && tool_in_hand->pointer_to_background() == NULL) {
				// added tool_in_hand->pointer_to_background() == NULL on 281004 to avoid spurious warning
				floor->add_item(tool_in_hand);
			};
		};
	};
};

#endif

void Programmer_At_Floor::dump_deltas(output_stream &stream) {
	stream.write((char *) delta_xs,number_of_deltas*sizeof(city_coordinate));
	stream.write((char *) delta_ys,number_of_deltas*sizeof(city_coordinate));
	stream.write((char *) delta_times,number_of_deltas*sizeof(millisecond));
};

//void Programmer_At_Floor::initialize_with_saved_deltas() { // noticed this was obsolete on 061102
//	for (int i = 0; i < number_of_deltas; i++) {
//		delta_xs[i] = saved_delta_xs[i];
//		delta_ys[i] = saved_delta_ys[i];
//		delta_times[i] = saved_delta_times[i];
//	};
//	delete [] saved_delta_xs;
//	saved_delta_xs = NULL;
//	delete [] saved_delta_ys;
//	saved_delta_ys = NULL;
//	delete [] saved_delta_times;
//	saved_delta_times = NULL;
//};

void Programmer_At_Floor::finish_initializing() {
	robot_in_control(FALSE);
   if (tt_system_mode != PUZZLE || is_toolbox_part_of_puzzle()) {
      // was 8000
      turn_off_selection_feedback_for(7000); // until everything settles down
   };
   if (tt_system_mode == PUZZLE) player_has_sat();
   if (tool_in_hand != NULL) {
//      hand = EMPTY_HAND;
//      pick_up(tool_in_hand);
		initialize_tool_in_hand(tool_in_hand); // replaced above with this on 140100 since is more accurate (for segmented logs)
		Sprite *appearance = pointer_to_appearance();
		if (tool_in_hand->kind_of() == COPIER) {
//			hand = HOLDING_COPIER;
			set_appearance(hand_with_wand);
			appearance->set_parameter(HAND_GRASP_WAND);
		} else {
			set_appearance(hand_appearance); // added 170100
//			hand = HOLDING_ITEM;
			if (horizontal_shape(tool_in_hand->kind_of())) { // new on 150100
				appearance->set_parameter(HAND_GRASP_HORIZONTAL);
			} else {
				appearance->set_parameter(HAND_GRASP_VERTICAL);
			};
		};
//		appearance->set_cycle_start_time(0); // added on 150100 as a hack to get it to switch to last image in cycle
   };
#if TT_DIRECT_INPUT
   set_joystick_sensitivity(25);
#endif
   tt_screen->add(appearance);
//   tt_screen->add(arm);
   if ((tt_toolbox != NULL && tt_toolbox->pointer_to_background() == NULL) && // condition added 110100
		 !tt_toolbox->gone() && // condition added 030300
		 (tt_system_mode != PUZZLE  || (is_toolbox_part_of_puzzle() && !inside_rocket()))) {
      floor->add_item(tt_toolbox,FALSE,FALSE,FALSE);
      tt_screen->add(tt_toolbox);
   };
};

Programmer_At_Floor::~Programmer_At_Floor() {
	if (appearance != hand_appearance) {
		hand_appearance->destroy();
	};
	if (appearance != hand_with_wand) {
		hand_with_wand->destroy();
	};
	if (original_thought_bubble != NULL) {
		original_thought_bubble->destroy();
	};
	if (thought_bubble_entered != NULL && !thought_bubble_added_to_floor) {
		thought_bubble_entered->destroy();
	};
	arm->destroy();
   if (tool_in_hand != NULL) {
		if (tt_shutting_down) {  // added 6/7/98
			tool_in_hand->set_held_status(NOT_HELD);
			tool_in_hand->destroy();
// commented out on 141199 since breaks puzzle 63 and is a bad idea anyway
// just need to make sure no crashes result
//		} else { // new on 140599 so won't crash if you stand up holding a running game
//			tool_in_hand->stop_all();
		};
	};
	tt_selection = NULL; // new on 260304 since can happen with time travel that this becomes a dangling reference
//	if (!tt_shutting_down && floor != NULL && floor->inside_rule_body()) { // commented out on 190103 - OK?
//		// must be during training
////		delete floor;
//      floor->destroy();
//	};
};

void Programmer_At_Floor::cleanup(Screen *screen, ProgrammerStatus next_status) {
	Programmer_State::cleanup(screen,next_status);
	screen->remove(arm);
};

boolean Programmer_At_Floor::ok_to_scroll() {
   // policy now is that in puzzles or in thought bubble or with invisible hand no "scrolling"
	// added tt_scrolling_ok on 030204
   return(tt_system_mode != PUZZLE && using_arm && !tt_programmer->invisible() && tt_scrolling_ok);  // tt_log_version_number > 9 &&
};

void Programmer_At_Floor::robot_in_control(boolean robot) {
  using_arm = (flag) !robot;
  if (robot || tt_system_mode == PUZZLE) {  // puzzles don't use scrolling so need some extra room
//     if (tt_log_version_number >= 7) { // changed on 4/7/97 because sometimes robot couldn't pick things up
        // restored on 5/29 since these deal with robot's region and so take into account height and width
	     // modified again on 3/17/98 since kids were losing their hand (especially with joystick)
	     // since these are used to constrain entire appearance
//	  if (tt_log_version_number > 12) {
		  min_x = tile_width;  // -appearance->true_width(); // -tile_width;
	     min_y = tile_height; // -appearance->true_height(); // -tile_height;  // made it off screen too much   changed this on 5/22/97
        max_x = ideal_screen_width-tile_width/8; 
	     max_y = ideal_screen_height-tile_height/8; 
//	  } else {
//		  min_x = -tile_width; 
//	     min_y = -tile_height;   
//        max_x = 21*tile_width;
//	     max_y = 21*tile_height;
//	  };
//     } else if (tt_log_version_number > 4) {
//        min_y = -appearance->current_height()-tile_height;
//        min_x = -appearance->current_width()-tile_width;
//        max_x = 21*tile_width;
//	     max_y = 21*tile_height;
//     } else {
//     	  min_x = 2*tile_width; // -8*tile_width;
//	     min_y = 2*tile_height; //-20*tile_height;
//	     max_x = 19*tile_width;
//	     max_y = 19*tile_height;
//     };
     // in thought bubble -- no constraints now
     floor->set_containment_active(FALSE);
  } else if (absolute_mouse_mode()) { // new on 180402
     min_x = tile_width;
     max_x = 19*tile_width;
     min_y = tile_height;
     max_y = 19*tile_height;
  } else {
//     if (tt_log_version_number >= 7) {
	     min_x = tile_width; //  was 2*tile_width before 5/22/97
//     } else {
//        min_x = 2*tile_width;
//     };
	  min_y = 2*tile_height;
     max_x = 18*tile_width;
     boolean scrolling = ok_to_scroll();
     if (scrolling) {
        max_y = 16*tile_height; // easier to "scroll" up
     } else {
        max_y = 18*tile_height;
     };
     if (//floor->on_the_ground() || // new policy on 080699
		   !scrolling) { 
        floor->set_containment_active(FALSE); // or the entire city size when outside??
     } else {
        set_containment_region();
     };
  };
};

void Programmer_At_Floor::set_containment_region() {
	TTRegion containment_region;
   if (floor->on_the_ground()) {
		tt_programmer->ground_containment_region(containment_region);
#if TT_DEBUG_ON
		if (tt_debug_mode == 300803) {
			tt_error_file() << "New containment region is ";
			containment_region.print(tt_error_file());
			tt_error_file() << endl;
		};
#endif
	} else {
		containment_region.min_x = left_room_margin;
		containment_region.min_y = 0; 
		containment_region.max_x = (floor_width_percent*ideal_screen_width)/100+left_room_margin;
		containment_region.max_y = (floor_height_percent*ideal_screen_height)/100;
		tt_programmer->translate_room_to_floor_coordinates(containment_region.min_x,containment_region.min_y);
		tt_programmer->translate_room_to_floor_coordinates(containment_region.max_x,containment_region.max_y);
	};
	floor->set_containment_region(containment_region);
};
/*
   if (floor->on_the_ground()) {
		return;
		// following just made for very strange scrolling
//		tt_programmer->ground_containment_region(containment_region);
//		floor->set_containment_region(containment_region);
	} else {
		TTRegion containment_region;
		containment_region.min_x = left_room_margin;
		containment_region.min_y = 0; 
		programmer->translate_room_to_floor_coordinates(containment_region.min_x,containment_region.min_y);
		containment_region.max_x = (floor_width_percent*ideal_screen_width)/100+left_room_margin;
		containment_region.max_y = (floor_height_percent*ideal_screen_height)/100;
		programmer->translate_room_to_floor_coordinates(containment_region.max_x,containment_region.max_y);
		floor->set_containment_region(containment_region);
	};
	*/

void Programmer_At_Floor::set_appearance(Sprite *new_appearance) {
//	if (saved_appearance == NULL) {
//		saved_appearance = appearance;
//		floor->remove_item(appearance);
//	};
	// might have say turned off selection feedback
	appearance->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
	appearance = new_appearance;
//	appearance->set_followers_on_top(FALSE);
	appearance->set_priority(programmer_hand_priority);
};

void Programmer_At_Floor::fix_before_leaving_thought(Screen *) {
//	floor->remove_item(pointer_to_toolbox_notebook());
//	floor->remove_toolbox();
//	release_items_held(screen);
	if (!bombed) {
		if (tool_in_hand == NULL || tool_in_hand->kind_of() != VACUUM) { // new policy on 090999
			vacuum_items_unused();
		} else {
			Sprite *saved_tool_in_hand = tool_in_hand;
			release_items_held();
			if (tt_toolbox != NULL && toolbox_tool(saved_tool_in_hand)) { // new on 090999
				saved_tool_in_hand->remove_from_floor(); // otherwise it'll be clobbered
			};
		};
	} else {
		bombed = FALSE; // restore flag
	};
//	Cubby *body_cubby = floor->pointer_to_body_cubby();
//	if (body_cubby->inside_vacuum() || body_cubby->contained_in(TRUCK)) {
//		floor->record_event(DESTROY_HOUSE,appearance);
//	};
//	floor->remove_all();
	tt_screen->flush_all(); // redone later by switch_to
};

void Programmer_At_Floor::release_items_held() {
	release_object(TRUE);
	release_object(TRUE); // e.g. released attached to copy wand above
};
//	if (hand != EMPTY_HAND) {
//		Tool *released = tool_in_hand->released(appearance,screen);
//      floor->record_event(RELEASE_ITEM,appearance,released);
//      if (released != tool_in_hand) {
			// e.g. released attached to copy wand above
//			floor->record_event(RELEASE_ITEM,appearance,tool_in_hand);
//      };
//		appearance->remove_follower(tool_in_hand,floor);
//   };
//};

void Programmer_At_Floor::turn_off_item_held() {
   if (tool_in_hand != NULL) tool_in_hand->turn_off(appearance);
};

void Programmer_At_Floor::vacuum_items_unused() {
//   if (tt_system_mode == PUZZLE && !is_toolbox_part_of_puzzle()) return;
	Robot *robot = (Robot *) appearance;		
	// maybe a better modularity would be that the programming has this
	// working set rather than the floor
//	Sprites *working_set = floor->pointer_to_working_set();
	Vacuum *vacuum;
	if (tt_toolbox != NULL) {
		vacuum = tt_toolbox_vacuum;
	} else { // as in a puzzle -- create a temporary vacuum for the following
		vacuum = new Vacuum();
	};
	int current_length;
	SpritePointer *working_set = robot->working_set_and_size(current_length);
	boolean first_time = TRUE;
	boolean vacuum_grasped = FALSE;
   Sprite *initial_tool = thought_bubble_robot->pointer_to_initial_tool();
	for (int i = 0; i < current_length; i++) {
	  Sprite *item = working_set[i]; //floor->working_set_nth(i);
	  if (item != NULL && item != initial_tool &&
         item->vacuum_if_left_on_floor(working_set,current_length)) {
		  if (first_time) {
			  release_items_held();
			  record_event(GRASP_ITEM,appearance,floor,vacuum,TRUE,'1'); 
			  // use '1' as marker that this was automatically generated (and all that follows)
//			  if (tt_log_version_number > 13) {
			     record_event(KEYBOARD_CHARACTER,appearance,floor,vacuum,TRUE,'1'); // make sure vacuum is on "suck" mode
//			  };
			  first_time = FALSE;
			  vacuum_grasped = TRUE;
		  };
		  record_event(VACUUM_APPLY,appearance,floor,item);
//		  floor->remove_item(item);
	  };
	};
	if (vacuum_grasped) {
		record_event(RELEASE_ITEM,appearance,floor,vacuum,TRUE);
		// commented out on 111104 since this just confuses the robot when it runs and the problem it fixed doesn't exist anymore
		//Sprite *initial_tool = robot->pointer_to_initial_tool();
		//if (initial_tool != NULL && initial_tool != robot->pointer_to_tool()) { // second conjunct added on 070401
		//	robot->pick_up_and_store_tool(initial_tool);
		//	record_event(GRASP_ITEM,appearance,floor,initial_tool,TRUE,'1'); // use '1' as marker that this was automatically generated (and all that follows)
		//};
	};
	if (tt_toolbox == NULL) {
		vacuum->destroy();
	};
};

//void Programmer_At_Floor::body_done() {
//};

void Programmer_At_Floor::returning_from_body(Floor *new_floor) {
	set_appearance(hand_appearance);
	appearance->set_background(new_floor); // new on 070103
	arm->set_background(new_floor); // new on 080103 since otherwise doesn't dump with geometry
	if (hand_appearance != NULL) { // new on 130703 since otherwise can't save since still referring to deleted floor
		// shouldn't really need to check for NULL...
		hand_appearance->set_background(new_floor);
	};
	if (hand_with_wand != NULL) { // new on 130703 since otherwise can't save since still referring to deleted floor
		hand_with_wand->set_background(new_floor);
	};
	appearance->move_to(tile_width*4,-appearance->current_height());
	robot_in_control(FALSE);
	tt_screen->add(appearance);
//	tt_screen->add(arm);// now driven by display_appearance
//	city_coordinate saved_width, saved_height;
//	floor->remove_item(tt_toolbox);
//	recursively_remove_leaders(tt_toolbox,floor);
//	ToolPointer *working_set;
	int body_working_set_size;
	thought_bubble_robot->working_set_and_size(body_working_set_size);
	// following cleans up stuff like deleted sprites
	thought_bubble_robot->done_running_body(FALSE);
//	floor->remove_item(tt_toolbox);
	if (tt_toolbox != NULL && !tt_toolbox->gone()) { // second condition added 030300
		tt_toolbox->remove_from_floor(FALSE);
	};
	Background *thought_floor = floor; // this is the "imaginary" one in the thought bubble
	floor = new_floor;
//	floor->remove_all_leaders();
//	tt_toolbox->recursively_restore_leaders(floor);
//	tt_toolbox->recursively_set_background(floor);
	if (tt_system_mode != PUZZLE || is_toolbox_part_of_puzzle()) {
      floor->add_item(tt_toolbox,FALSE);
   };
   if (thought_bubble_entered != NULL) thought_bubble_entered->destroy();
	thought_bubble_entered = original_thought_bubble;
	original_thought_bubble = NULL;
//	thought_bubble_entered->recursively_set_background(floor);
//	floor->add_item(thought_bubble_entered,FALSE);
	if (martian_active() && tt_system_mode != PUZZLE) {
		tt_martian->animate_to_favorite_spot_and_size(1000);
	};
//	recursively_restore_leaders(thought_bubble_entered,floor);
/*
	thought_bubble_entered->saved_size(saved_width,saved_height);
	thought_bubble_entered->set_size_and_location(saved_width,
																 saved_height,
																 thought_bubble_llx,
																 thought_bubble_lly,
																 NO_REASON_GIVEN);
*/
	thought_bubble_robot->set_priority(floor->next_item_priority());
	thought_bubble_robot->morph_to_inert();
	thought_bubble_robot->set_sprite_type(ROBOT);
	thought_bubble_robot->set_selectable(TRUE);
//	thought_bubble_robot->set_programmer(NULL);
//	thought_bubble_robot->set_background(floor);
	thought_bubble_robot->remove_from_floor(FALSE); // the old background
	floor->add_item(thought_bubble_robot);
	thought_bubble_entered->center_and_resize_cubby();
//	thought_bubble_entered->blank_all_remotes();
   tt_screen->add(thought_bubble_entered); // since it is an outside follower
	thought_bubble_robot->connect_to_thought_bubble(thought_bubble_entered,FALSE);
	thought_bubble_entered = NULL;
//	floor->add_item(thought_bubble_robot);
//	floor->add_item(thought_bubble_entered); // seems necessary??
//	tt_screen->add(thought_bubble_entered);
//	y += 8*tile_height;
	appearance->move_by(0,8*tile_height);
//   if (hand != EMPTY_HAND) {
//      tool_in_hand->move_by(0,8*tile_height);
//   };
//   tt_events->is_first();
	tt_events = new Events(create_serial_number(),
                          new Events(create_first_event(body_working_set_size),tt_events));
	tt_events->insert_at_end(create_last_event());
	thought_bubble_robot->set_body(tt_events);
	thought_bubble_robot->set_max_working_set_size(body_working_set_size);
	if (tt_log_version_number >= 64) {
		thought_bubble_robot->morph_to_inert(); // new on 151204
	};
	tt_events = NULL;
   Sprites *remaining = thought_floor->pointer_to_items();
   while (remaining != NULL) {
      Sprites *next_remaining = remaining->rest();
      remaining->first()->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
      remaining = next_remaining;
   };
	if (tool_in_hand != NULL && tool_in_hand->pointer_to_leader() == NULL) { // second condition added 160600
		// otherwise initial tool of robot doesn't know that it is held and behaves badly 
		// (e.g. wand thinks its state is being changed when used by ' ')
		tool_in_hand->set_held_status(NOT_HELD);
	};
//   tool_in_hand = NULL;
//   hand = EMPTY_HAND;
	set_tool_in_hand(NULL); // re-written on 220901
	cubby_that_started_training->set_priority(floor->next_tool_priority()); // new on 110103 - thanks Leonel for the suggestion that the cubby be on the robot at the end
	cubby_that_started_training = NULL; // done with it - new on 090203
	thought_bubble_robot = NULL; // new on 070105 since otherwise can become trash and cause a crash upon save city
	thought_floor->destroy(); // moved here on 230105 since is same as floor used above
};

/*
void Programmer_At_Floor::note_colliding_with(Sprite *sprite_of_collider,
																  SpriteType type) {
	// just record it, so can react to buttons, etc. later
	sprite_of_last_collider = sprite_of_collider;
//   last_collider = other;
	type_of_last_collider = type;
};
*/

unsigned char key_substitute[joystick_button_count] = {0,0,0}; // no defaults as of 041199 - toontalk.ini is complete control
// last default changed from VK_F1 to . on 200799 
unsigned long key_substitute_extended_key = 0; // was 1<<3 prior to 041199

void initialize_button_key_substitutions() {
   ascii_string substitution;
   char key[3];
   for (int i = 0; i < joystick_button_count; i++) { // was i = 1 but if someone really wants to redefine button one then why not??
      itoa(i+1,key,10); 
      substitution = ini_entry("ButtonKeyboardEquivalents",key); 
		//was AS(IDS_BUTTON_KEYBOARD_EQUIVALENTS) which was pretty wasteful
      // either a character or the name of a special key within <>
      if (substitution != NULL) {
         boolean extended_key;
         key_substitute[i] = name_to_character(substitution,extended_key);
         if (extended_key) key_substitute_extended_key |= (1<<i);
         delete [] substitution;
      };
   };
};

boolean install_keyboard_accelerator(int first_new_button_down, unsigned char &key, boolean &extended_key) {
   if (key_substitute[first_new_button_down] != 0) {
      key = key_substitute[first_new_button_down];
      extended_key = (boolean) (key_substitute_extended_key&(1<<first_new_button_down));
      return(TRUE);
   };
   return(FALSE);
};

boolean Programmer_At_Floor::enable_keyboard_accelerators(char joystick_button_status, char mouse_button_status,
                                                          char &button_status, unsigned char &key, boolean &extended_key) {
   int first_button_down = -1;
#if TT_DIRECT_INPUT
   if (joystick_button_status != 0) {
      first_button_down = first_joystick_button_down(0);
      if (first_button_down > 0 && joystick_button_still_down(0,first_button_down)) { // already took care of this
         button_status = 0;
         return(FALSE);
      };
   } else
#endif
	if (mouse_button_status != 0) { // && tt_new_input_scheme) {
      if (virtual_right_button(mouse_button_status,FALSE)) {
         first_button_down = 1;
      } else if (virtual_middle_button(mouse_button_status,FALSE)) { // was just middle_button prior to 041199
         first_button_down = 2;
      };
      if (first_button_down > 0 && (mouse_button_status&BUTTON_STILL_DOWN)) { // already took care of this
         button_status = 0;
         return(FALSE);
      };
   };
   if (first_button_down > 1) {  // button 0 and 1 is built-in
      if (install_keyboard_accelerator(first_button_down,key,extended_key)) {
         button_status = 0; // converted to keyboard action
      };
   };
   return(FALSE);
};

ProgrammerStatus Programmer_At_Floor::get_up() {
   tt_finish_all_animations = TRUE; // stuff might be happening like still entering...
   move_count = 0; // new on 130502 - reset counter
   if (tool_in_hand == tt_toolbox) { // can't get up holding toolbox
      release_object(TRUE);
   };
   return(LEAVE_FLOOR_WHEN_TOOLBOX_READY);
};

void Programmer_At_Floor::tool_called_for(Sprite *tool, int sound_id, unsigned char key) {
	if (tt_programmer->invisible()) { // implicit F9 if needed
		tt_screen->add(appearance);
		tt_programmer->set_visibility(TRUE);
//		tt_game_switch_on = FALSE;
	};
   if (hand != EMPTY_HAND && tool != tt_toolbox && tool != tt_toolbox_notebook && tool != NULL) { 
		// new conditions on 200999 so don't drop because called Tooly or notebook -- added tool != NULL for robustness on 070704
		tool->set_called_since_last_release(TRUE); // new on 090699 so released object goes back to favorite spot
		boolean was_holding_it_already = (tool_in_hand == tool);
      release_object(TRUE,TRUE);
		if (was_holding_it_already) return; // new on 090699 so don't grab it again
	};
	if (tt_system_mode == PUZZLE) {  // added this on 9/22/97 -- F3 and F5 were already handled
		// re-ordered on 280499
      function_key_pressed(key); // ,this);
   } else if (tool != NULL && tool_in_hand != tool) {
		play_sound(sound_id);
		if (floor != NULL && tool->pointer_to_background() == NULL && !tool->inside_vacuum()) { 
			//  && !tool->inside_vacuum() new on 070704
			// this had been commented out (ages ago?) and was restored on 070704 
			// since tool may have been recently reincarnated (if the original was destroyed)
			floor->add_item(tool);
		};
      tool->move_to_hand(); // this); -- called even if tool->inside_vacuum() since generates a nice message
		if (!tool->inside_vacuum()) { // condition new on 070704
			called_for(tool);
		};
   };
};

ProgrammerStatus Programmer_At_Floor::react(boolean ,
														  unsigned char key,
														  boolean extended_key,
														  city_coordinate delta_x,
														  city_coordinate delta_y,
														  char button_status,
														  boolean button_unchanged,
														  millisecond ,
														  boolean running_timer) {
	if (tt_add_sprite_when_on_floor != NULL) { 
		// copied here on 220303 so Edit Picture dialog works right and if double click on TT file and go to saved city on the floor
		if (tt_add_sprite_when_on_floor_but_wait_a_frame) {
			tt_add_sprite_when_on_floor_but_wait_a_frame = FALSE; // new on 140204 to keep this in synch when recording
		} else {
			tt_toolbox->add_sprite(tt_add_sprite_when_on_floor);
			tt_add_sprite_when_on_floor = NULL;
		};
	};
	ProgrammerStatus status = PROGRAMMER_NORMAL;
	boolean retraining =	(appearance->kind_of() == ROBOT_IN_TRAINING &&
		                   (((Robot *) appearance)->still_running() ||
		                   ((Robot *) appearance)->pointer_to_working_cubby() != NULL)); // or waiting to run
	if (retraining) { // robot is in control until you click 
		delta_x = 0;
		delta_y = 0;
   };
   Direction direction = NO_DIRECTION;
   boolean shift_ok = !running_timer && ok_to_scroll();
//#if TT_DEBUG_ON
//   if (tt_hand_needs_to_move_to_cursor) {
//      log("debug this");
//   };
//#endif
   if (absolute_mouse_mode()) {
      moved = (tt_hand_needs_to_move_to_cursor || tt_hand_needs_to_move_for_tool_use);
      if (!moved && (delta_x != 0 || delta_y != 0)) { // added zero check on 080502 so don't scroll while standing
         // tt_dragging_enabled added on 180602 so can drag and scroll at the same time -- removed 180602
         if (shift_ok && tt_current_time > time_of_next_scroll_with_absolute_mouse) {
            // tt_dragging_enabled added on 180602 so can drag and scroll at the same time -- removed 180602
            // not moving but might be scrolling
#if TT_DEBUG_ON
            if (tt_debug_mode == 180602) {
               tt_error_file() << "Checking for need to scroll on frame " << tt_frame_number << endl;
            };
#endif
            if (delta_x < min_x) {
               direction = WEST;
            } else if (delta_x > max_x) {
               direction = EAST;
            };
            if (delta_y < min_y) {
               switch (direction) {
                  case WEST:
                     direction = SOUTHWEST;
                     break;
                  case EAST:
                     direction = SOUTHEAST;
                     break;
                  default:
                     direction = SOUTH;
               };
            } else if (delta_y > max_y) {
               switch (direction) {
                  case WEST:
                     direction = NORTHWEST;
                     break;
                  case EAST:
                     direction = NORTHEAST;
                     break;
                  default:
                     direction = NORTH;
               };
            };
            if (direction != NO_DIRECTION) {
               time_of_next_scroll_with_absolute_mouse = tt_current_time+1000; // 1 second between scrollings
            };
         };
         if (!running_timer) { 
				// don't interfere with automatic movements (e.g. hand moving to center after sitting) - new on 250402
            delta_x = 0;
            delta_y = 0;
         };
      };
   } else {
	   moved = (flag) ((delta_x != 0) || (delta_y != 0));
   };
	if (moved) {
#if TT_ABSOLUTE_MOUSE
      if (absolute_mouse_mode()) { 
			// reworked on 180402 so that here we interpret delta_x and delta_y as the location for the center of the selection region
         TTRegion target_region;
         if (tool_in_hand == NULL) {
            compute_selection_region(target_region);
         } else if (tt_hand_needs_to_move_for_tool_use) { // new on 190402
            // tt_hand_needs_to_move_for_tool_use is here because if using a tool 
				// rather than dropping it want center vs selection region
            tool_in_hand->active_region(target_region);
         } else if (tool_in_hand->kind_of() == COPIER) {
            Sprite *attached = ((Copier *) tool_in_hand)->attached_tool();
            if (attached != NULL) {
               attached->true_region(target_region);
            } else { 
               tool_in_hand->true_region(target_region); // is this right??
            };
         } else if (tool_in_hand->is_tool()) { // new on 130502 so moves so active region is where clicked
            tool_in_hand->active_region(target_region);
         } else { // so you can drop accurately
            tool_in_hand->true_region(target_region);
         };
         city_coordinate programmer_x, programmer_y;
         target_region.region_center(programmer_x,programmer_y);
#if TT_DEBUG_ON
         if (tt_debug_mode == 180602) {
            tt_error_file() << "Direction is " << (int) direction << " and tt_dragging_enabled is " << (int) tt_dragging_enabled 
									 << " on frame " << tt_frame_number << endl;
            tt_error_file() << "programmer_y is " << programmer_y << " and delta_y is " << delta_y;
         };
#endif
         if (delta_x != 0 || delta_y != 0) { // conditional new on 180602 to scroll and drag
            delta_x = delta_x-programmer_x;
            delta_y = delta_y-programmer_y;
         };
#if TT_DEBUG_ON
         if (tt_debug_mode == 180602) {
            tt_error_file() << " and became " << (delta_y-programmer_y) << endl;
         };
#endif
         moved = (flag) ((delta_x != 0) || (delta_y != 0)); // can keep button down without moving anything
         //if (tt_dragging_enabled) {
         //   distance_dragged += abs(delta_x)+abs(delta_y); // Manhattan distance good enough
         //};
         if (!tt_dragging_enabled && virtual_left_button(button_status,FALSE)) {
            move_count++; // new on 130502
         };
      } else
#endif
		dampen_big_deltas(delta_x,delta_y);
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 180703) {
		tt_error_file() << "Programmer moved from " << appearance->current_llx() << "," << appearance->current_lly() 
							 << " by " << delta_x << "," << delta_y 
							 << " at frame " << tt_frame_number << " during log segment " << tt_current_log_segment 
//						    << " appearance#" << appearance->debug_counter 
							 << endl;
	};
#endif
   city_coordinate delta_x_moved = delta_x; // so can be undone below
   city_coordinate delta_y_moved = delta_y;
   move_appearance(moved,delta_x,delta_y); // moved here on 170402
   tt_hand_needs_to_move_for_tool_use = FALSE; // if was set since is handled now
	Sprite *old_selection = current_selection;
   // beginning 180402 compute the selection after moving
	if (hand != ABOUT_TO_RELEASE_ITEM && (!tt_programmer->invisible() || tool_in_hand != NULL) &&
		 tt_programmer->return_next_status() != PROGRAMMER_RUNNING_TIMER) { 
		// hand != ABOUT_TO_RELEASE_ITEM new on 101199
		// check that not running timer added on 020106
		// save deltas for things like initial picture speed
		delta_xs[delta_index] = delta_x;
		delta_ys[delta_index] = delta_y;
		delta_times[delta_index] = tt_millisecond_delta;
		delta_index = (delta_index+1)%number_of_deltas;
//		if (!tt_game_switch_on) { // commented out on 091199 since should be able to use tools with hand hidden
			current_selection = selection();  // compute new one
			if (current_selection != NULL && current_selection->slated_for_deletion()) {
				current_selection = NULL;
			};
//		} else {
//			current_selection = NULL;
//		};
	} else {
		current_selection = NULL;
		moved = FALSE;
		button_status = 0;
	};
	if (old_selection != current_selection) {
		if (old_selection != NULL) {
//         old_selection->set_selected(FALSE);
         if (current_selection == NULL ||
			    current_selection->pointer_to_leader() != old_selection ||
			    current_selection->pointer_to_leader() == NULL ||
			    current_selection->pointer_to_leader()->kind_of() != PROGRAM_PAD) {
			   // special case for notebook to type to whole
            // notebook but select pages
			   old_selection->unselect();
			   if (old_selection->pointer_to_leader() != NULL &&
				    old_selection->pointer_to_leader()->kind_of() == PROGRAM_PAD) {
				   old_selection->pointer_to_leader()->unselect();
			   };
			   if (tt_toolbox != NULL && !tt_toolbox->gone()) { // second condition added 030300
               tt_toolbox->reset_last_contents_selection();
            };
			};
         tt_screen->invalidate_cache(); // since changing selection
		};
//      if (current_selection != NULL) {
//         current_selection->set_selected(TRUE);
//         current_selection->set_clean_status(DIRTY);
//      };
	};
   tt_selection = current_selection;
   if (old_selection != tt_selection && old_selection != NULL) { 
		// added first condition on 171202 since it happens immediately below as well when they are equal
      old_selection->set_clean_status(MOVED); // updated 070305 DIRTY);
      old_selection->set_clean_status_of_outside_followers(MOVED); // updated 070305 DIRTY); // so it doesn't disappear if cached
   };
   if (tt_selection != NULL) {
#if TT_DEBUG_ON
		if (tt_selection->debug_counter == tt_debug_target) {
			log(_T("Debug target selected."));
		};
#endif
      tt_selection->set_clean_status(MOVED); // prior to 070305 was DIRTY but that causes lots of extra work 
		// (including now back of picture labels being recomputed)
      tt_selection->set_clean_status_of_outside_followers(MOVED); // DIRTY); updated 070305
   };
	if (retraining && ((key != 0
#if TT_BUG_FIXES
								&& key != VK_F7 // new on 081203 since if retraining hurrying up or a snapshot shouldn't change things
								&& key != VK_F1 // new on 130405 since calling Marty shouldn't interfere
								&& key != VK_F10 // new on 130405 since putting up function key shouldn't interfere
//								&& key != VK_F12 // don't really know what it is bound to and not worth checking if control is down (or is it?)
#endif
								) || 
		                button_status&RIGHT_BUTTON || button_status&LEFT_BUTTON || button_status&MIDDLE_BUTTON)) {
		play_sound(SWITCH_SOUND);
		stop_retraining((Robot *) appearance);
		simulate_mouse_up(); // mouse click taken care of
		retraining_stopped_on_frame = tt_frame_number;
		return(status);
	};
   TTRegion region; // moved out here on 190602
   appearance->true_region(region);
#if TT_DEBUG_ON
	if (tt_debug_mode == 300803 && delta_x < 0) {
		tt_error_file() << "Delta_x is " << delta_x << endl;
		tt_error_file() << appearance->current_llx() << " is the llx; true region is ";
		region.print(tt_error_file());
		tt_error_file() << endl;
	};
	if (tt_debug_mode == 190304) {
		tt_error_file() << "Initial true region is ";
		region.print(tt_error_file());
		tt_error_file() << " on frame " << tt_frame_number << " during segment " << tt_current_log_segment << endl;
	};
#endif
	city_coordinate x = appearance->current_llx(); 
	// new on 081004 -- initially used for stopping the wand from drifting if used on the floor (i.e. missed the target)
   city_coordinate y = appearance->current_lly(); // since already moved - new on 170402
	if (moved && relative_mouse_mode()) {
//		dampen_big_deltas(delta_x,delta_y);
//		appearance->lower_left_corner(x,y);
//		x += delta_x;
//		y += delta_y;
//    if (tt_mouse_mode) {
//       y = delta_y; // y is where the hand will go (new on 170402)
//    } else {
//		   y = delta_y+appearance->current_lly();       
//    };
//		if (tt_log_version_number > 12) {
//			appearance->true_region(region);
//		} else {
//		   appearance->full_region(region);
//		};
		if (tool_in_hand != NULL) { // && !shift_ok) {
			TTRegion tool_region;
			if (!using_arm || tool_in_hand->kind_of() != COPIER) {
				tool_in_hand->full_region_including_outside_followers(tool_region);
				region.extend_with(tool_region);
			} else {
				Sprite *attached = ((Copier *) tool_in_hand)->attached_tool();
				if (attached != NULL) {
					attached->full_region_including_outside_followers(tool_region);
					region.extend_with(tool_region);
				};
			};
		};
   };
#if TT_DEBUG_ON
	if (tt_debug_mode == 21103) {
		shift_ok = FALSE;
	};
#endif
	if (moved && (relative_mouse_mode() || (tt_dragging_enabled && tt_current_time > time_of_next_scroll_with_absolute_mouse)) && 
		 !running_timer && direction == NO_DIRECTION) { // || tt_log_version_number < 13) {
		// reorganized on 190602 so that can drag and scroll in absolute mouse mode
#if TT_DEBUG_ON
      if (tt_debug_mode == 180602) {
         tt_error_file() << "Checking for need to scroll where y is " << y << " and region.max_x is " << region.max_x 
								 << " on frame " << tt_frame_number << endl;
      };
#endif
      city_coordinate adjust_min,adjust_max;
      if (relative_mouse_mode()) {
         adjust_min = 0;
         adjust_max = 0;
      } else {
         adjust_min = 1500;
         adjust_max = 4000; // since when dragging the hand you can't move it as close to the edge as when in relative mode (new on 190602)
      };
      if (region.max_x < min_x+adjust_min) {
			if (shift_ok && delta_x < 0) {
				direction = WEST;
			} else {
				if (!shift_ok) { // tt_log_version_number > 12 && 
//					delta_x = min_x-region.max_x;
					appearance->move_by(min_x-region.max_x,0); // new on 060702
				} else {
					delta_x += min_x-region.max_x;
				};
			};
		};
		if (region.min_x > max_x-adjust_max) {
			if (shift_ok && delta_x > 0) {
				direction = EAST;
			} else {
				if (!shift_ok) { // tt_log_version_number > 12 && 
//					delta_x = max_x-region.min_x;
					appearance->move_by(max_x-region.min_x,0); // new on 060702
				} else {
					delta_x += max_x-region.min_x;
				};
			};
		};
		if (region.max_y < min_y) {
			if (shift_ok && delta_y < 0) {
				switch (direction) {
					case NO_DIRECTION:
						direction = SOUTH;
						break;
					case EAST:
						direction = SOUTHEAST;
						break;
					case WEST:
						direction = SOUTHWEST;
						break;
				};
			} else {
				if (!shift_ok) { // tt_log_version_number > 12 && 
//					delta_y = min_y-region.max_y;
					appearance->move_by(0,min_y-region.max_y); // new on 060702
				} else {
					delta_y += min_y-region.max_y;
				};
			};
		};
		if (using_arm) {
			if (y > max_y-adjust_max) { // using y rather than region.min_y so end of arm doesn't show
				if (shift_ok && delta_y > 0) {
					switch (direction) {
						case NO_DIRECTION:
							direction = NORTH;
							break;
						case EAST:
							direction = NORTHEAST;
							break;
						case WEST:
							direction = NORTHWEST;
							break;
					};
				} else {
					if (!shift_ok) { // tt_log_version_number > 12 && 
//						delta_y = max_y-y;
						appearance->move_by(0,max_y-y); // new on 060702
					} else {
						delta_y += max_y-y;
					};
				};
			};
		} else {
			if (region.min_y > max_y-adjust_max) {
				if (shift_ok && delta_y > 0) {
					switch (direction) {
						case NO_DIRECTION:
							direction = NORTH;
							break;
						case EAST:
							direction = NORTHEAST;
							break;
						case WEST:
							direction = NORTHWEST;
							break;
					};
				} else {
					if (!shift_ok) { // tt_log_version_number > 12 && 
//						delta_y = max_y-region.min_y;
						appearance->move_by(0,max_y-region.min_y); // new on 060702
					} else {
						delta_y += max_y-region.min_y;
					};
				};
			};
		};	
	};
   // was else so selection feedback only if no movement
   if (direction != NO_DIRECTION) { // moved here on 220402 so works for both relative and absolute modes
#if TT_DEBUG_ON
      if (tt_debug_mode == 180602) {
         tt_error_file() << "Scrolling in direction " << direction << " with delta_x and delta_y " << delta_x << "," << delta_y 
								 << " on frame " << tt_frame_number << endl;
		};
      if (tt_debug_mode == 190304) {
         tt_error_file() << "Found need to scroll where delta_x and delta_y are " << delta_x << " and " << delta_y << " and region is ";
			region.print(tt_error_file());
			tt_error_file() << " on frame " << tt_frame_number << endl;
      };
#endif
		if (shift_view(direction,delta_x,delta_y)) {
         time_of_next_scroll_with_absolute_mouse = tt_current_time+1000; 
			// 1 second between scrollings -- only applies to absolute mouse mode - new on 190602
			set_containment_region(); // re-compute since new sit corner
			direction = NO_DIRECTION; // for turning off force feedback wall
         if (!absolute_mouse_mode()) { // || tt_dragging_enabled) { // tt_dragging_enabled added on 180602 for scrolling and dragging - removed 180602
            appearance->move_by(-delta_x_moved,-delta_y_moved); // new on 070602 to undo the movement since shifted viewpoint
            if (tool_in_hand != NULL) { // new on 120602 so tool follows immediately
               tool_in_hand->move_by(-delta_x_moved,-delta_y_moved);
            };
         };
      } else { // else hit a wall
         // new on 250402 to "undo" the last movement in the direction blocked
         switch (direction) {
            case NORTH:
               if (absolute_mouse_mode()) { // new on 110602 so can stand up by clicking on the wall
                  return(get_up());                  
               };
            case SOUTH:
               appearance->move_by(0,-delta_y_moved);
               break;
            case EAST:
            case WEST:
               appearance->move_by(-delta_x_moved,0);
               break;
            default:
               appearance->move_by(-delta_x_moved,-delta_y_moved);
         };
      };
	};
#if TT_DIRECT_INPUT
   if (moved) {
      play_wall_force_feedback_effect(direction); // moved here on 220402 so works for both relative and absolute modes
   };
#endif
#if TT_DEBUG_ON
	if (tt_debug_mode == 300803 && delta_x < 0) {
		tt_error_file() << appearance->current_llx() << " is the llx; true region is ";
		TTRegion region;
		appearance->true_region(region);
		region.print(tt_error_file());
		tt_error_file() << " after reacting." << endl << endl;
	};
#endif
//	compute_recent_speed(); // commented out 101199
//   if (extended_key && key == VK_F6) { // save floor (or city if on the ground)
//      floor->dump_to_clipboard();
//   };
	if (key == '\026') { // control-v added on 010499
		add_item_from_clipboard(floor,appearance,key);
		key = 0; // added on 101199 since used up
	};
/*
// I think the following was a suggestion from Dean and/or MarkM
// The idea is to cycle thru for a click to pick up, turn on, turn off, drop
// right button if you want to use it again
   if (tt_new_input_scheme && tool_in_hand != NULL && tool_in_hand->is_tool()) {
      // trying it with just real tools -- clean up following if this works well
      if //(!extended_key &&
          //(key == ' ' ||
           (!button_unchanged &&
            (button_status&RIGHT_BUTTON ||
             button_status&LEFT_BUTTON ||
             button_status&MIDDLE_BUTTON)) {
         // user pressed button
         // space bar -- too many problems -- repeated and typing to text pads
         if (virtual_right_button(button_status)) { //button_status&SHIFT_KEY_DOWN) { // act like just picked it up
             tool_in_hand->reset_used_since_pick_up();
         };
         if (hand == EMPTY_HAND) {
            pick_up(current_selection);
         } else if (tool_in_hand != NULL && tool_in_hand->kind_of() == COPIER &&
                    ((Copier *) tool_in_hand)->attached_tool() != NULL) {
            drop(status);
         } else if (!tool_in_hand->used_since_pick_up() && tool_in_hand != NULL &&
                    tool_in_hand->is_tool() && !tool_in_hand->is_on()) {
            tool_in_hand->turn_on(appearance);
         } else if (tool_in_hand != NULL && tool_in_hand->is_on()) {
            tool_in_hand->turn_off(appearance);
         } else if (tool_in_hand->used_since_pick_up() ||
                    (tool_in_hand != NULL && !tool_in_hand->is_tool())) {
            drop(status);
         };
      };
	} else
*/
   // if true then waiting for robot to finish but no more new steps
   // following commented out on 130502 since was too confusing an interface criteria
   //boolean ignore_left_button_for_drop = (distance_dragged > tile_width*2); // doubled on 100502
   //if (!tt_dragging_enabled) {
   //   distance_dragged = 0;
   //};
   boolean ignore_left_button_for_drop = (move_count <= 2 && absolute_mouse_mode()); // drop if moved twice without use in between
   // added absolute_mouse_mode test on 050602
   boolean robot_exhausted = (!using_arm && ((Robot *) appearance)->exhausted_training_counter());
//#if TT_DEBUG_ON
//   if (button_status > 0) {
//      log("debug this");
//   };
//#endif
   if (robot_exhausted) { // do nothing
   } else if (hand == EMPTY_HAND &&
              (virtual_left_button(button_status,FALSE) ||
//             (virtual_right_button(button_status) && !tt_new_input_scheme) ||
//		         (extended_key && key == VK_F11) ||  // new on 041199 -- commented out on 05071
              (keyboard_accelerators_enabled && !extended_key &&
		         (keyboard_accelerator(key,PICKUP_IF_HAND_EMPTY) || keyboard_accelerator(key,PICKUP_AND_FLIP_IF_HAND_EMPTY))))) {
//         (key == 'p' || key == 'P' || key == 'f' || key == 'F')))) {
		// rather than give kids a hard time let them use "right" button
		// if hand is empty
      if (current_selection != NULL) {
         // didn't use to have conditional but remotes were broken by simulating mouse up
		   simulate_mouse_up(); // so tool grasped isn't dropped right away
		   pick_up(current_selection);
         if (keyboard_accelerator(key,PICKUP_AND_FLIP_IF_HAND_EMPTY)) use_tool(); // pick up and flip
         key = 0; // used up
      };
   } else if (current_selection == NULL && hand == EMPTY_HAND &&
              !extended_key && keyboard_accelerator(key,STANDUP_IF_HAND_EMPTY) &&
				  !tt_programmer->invisible() && // added on 220300 so you must be visible for this to work
				  current_selection == NULL) { // this added on 160300 so that applications can read keyboard when pointed to
      return(get_up());
	} else if (((!ignore_left_button_for_drop && virtual_left_button(button_status,FALSE)) ||
				  (extended_key && key == VK_F11) ||  // new on 041199
				  (extended_key && key == VK_F12) ||  // new on 100200
              (tool_in_hand != NULL &&
//               tool_in_hand->kind_of() != TEXT && // so you can type Ds
// commented out the above and added the following since bad behavior now that comments are supported
               tt_mouse_button_count == 0 &&
               !extended_key && keyboard_accelerator(key,DROP_THING_HELD))) &&
              hand != EMPTY_HAND &&
				  hand != ABOUT_TO_RELEASE_ITEM &&
				  hand != ABOUT_TO_RELEASE_COPIER &&
				  hand != USING_COPIER) {
		// new on 041199 - only via F11 is stuff underneath ignored
      ignore_underneath_on_next_drop = (extended_key && key == VK_F11);
		give_toss_on_next_drop = (extended_key && key == VK_F12);
		key = 0; // used up
		simulate_mouse_up(); // just drop it once
      drop(status);
	} else if (extended_key && key == VK_F11 && hand == EMPTY_HAND) { // new on 070501
      if (appearance->kind_of() == PROGRAMMER) { 
         floor->undo_previous_mouse();
      } else {
         play_sound(PLOP_SOUND); // new on 041002	   
      };
	} else if (virtual_right_button(button_status,FALSE) && // tt_new_input_scheme &&
				  hand != ABOUT_TO_RELEASE_ITEM && hand != EMPTY_HAND) {
      if (!(button_status&BUTTON_STILL_DOWN) && tool_in_hand->being_used()) { // new on 300902 to toggle when right button is pressed (unless held down)
         tool_in_hand->turn_off(appearance);
      } else {
		   saved_button_unchanged = button_unchanged;
		   use_tool(TRUE,x,y); // args new on 081004
      };
//	 } else if (virtual_right_button(button_status) &&	hand == EMPTY_HAND) {
		// give some advice here
//		if (tt_mouse_button_count == 2) {
//			say("The right mouse button is for using whatever is in your hand.  ");
//			say("The left button is for picking up and dropping things.  ");
//		} else if (tt_mouse_button_count == 1) {
//			say("A mouse button while hold down the 'Shift' button is for using whatever is in your hand.  ");
//			say("Just a mouse button is for picking up and dropping things.  ");
//		};
//		simulate_mouse_up();
   }; // was else if...
   if (((hand == HOLDING_ITEM && tool_in_hand != NULL) ||
       (hand == HOLDING_COPIER &&
        ((Copier *) tool_in_hand)->attached_tool() == NULL)) && tool_in_hand->is_still_on() && !robot_exhausted) { 
			  // was is_on prior to 270902
      use_tool(FALSE,x,y); // keep using it
	};
	if (hand == ABOUT_TO_HOLD_ITEM) {
		// this way I add follower to grasping hand not the open hand
//		TTRegion *grasp_region = pointer_to_grasp_region();
//		city_coordinate adjustment_x, adjustment_y;
//		tool_in_hand->adjust_grasp(grasp_region,adjustment_x,adjustment_y);
//      appearance->move_by(adjustment_x,adjustment_y);
/*
		if (adjustment_x > 3*tile_width) adjustment_x = 3*tile_width;
		if (adjustment_x < -3*tile_width) adjustment_x = -3*tile_width;
		if (adjustment_y > 3*tile_height) adjustment_y = 3*tile_height;
		if (adjustment_y < -3*tile_height) adjustment_y = -3*tile_height;
*/
//      adjustment_y += 3*tile_height/2; // open handed arm is shorter
//		x += adjustment_x;
//		y += adjustment_y;
/*
		city_coordinate ideal_hand_llx, ideal_hand_lly;
		// to start with
		tool_in_hand->lower_left_corner(ideal_hand_llx,ideal_hand_lly);
		ideal_hand_llx
			+= tool_in_hand->true_width()-appearance->true_width()/2;
		ideal_hand_lly
			+= tool_in_hand->true_height()/2-appearance->true_height()/2;
		appearance->lower_left_corner(x,y);
		delta_x = ideal_hand_llx-x;
		delta_y = ideal_hand_lly-y;
*/
		if (using_arm && tool_in_hand->kind_of() == COPIER) {
			hand = HOLDING_COPIER;
			tt_screen->remove(appearance);
			city_coordinate current_x, current_y;
			appearance->lower_left_corner(current_x,current_y);
			current_x += appearance->true_width(); // rightmost x
			set_appearance(hand_with_wand);
			tt_screen->add(appearance);
         tt_screen->remove(tool_in_hand); // wand is in hand artwork instead
         Sprite *attached = ((Copier *) tool_in_hand)->attached_tool();
         if (attached != NULL) tt_screen->add(attached); 
			appearance->set_parameter(HAND_GRASP_WAND);
			appearance->reset_cycle(); // may have already been in that state
			appearance->move_to(current_x-appearance->true_width(),
									  current_y+2*tile_height);
//         arm->move_by(tile_width,0);
//			appearance->set_size((5*appearance->current_width())/4,
//										(5*appearance->current_height())/4);
//			arm->set_size((4*arm->current_width())/5,
//							  (4*arm->current_height())/5);
//			floor->remove_item(tool_in_hand);
		} else if (!robot_exhausted) {
			hand = HOLDING_ITEM;
			if (using_arm) {
            // should just happen if tool removed from something
//				tool_in_hand->animate_to_good_size(1000);
				if (horizontal_shape(tool_in_hand->kind_of())) {
					appearance->move_by(0,2*tile_height); // looks better
					appearance->set_parameter(HAND_GRASP_HORIZONTAL);
				} else {
					appearance->set_parameter(HAND_GRASP_VERTICAL);
				};
				// moved here on 291204
				if (tt_log_version_number >= 65) {
					// "moved away from here" on 291204 since the hand is still about to grasp and have its parameter set
					if (appearance->kind_of() != ROBOT) { 
						// conditionalized on 220102 since this is now handled within Robot::pick_up_and_store_tool
						millisecond duration = max(appearance->total_cycle_duration(),tool_in_hand->total_cycle_duration())+1; 
						// +1 was +500 prior to 230203
						// programmer_appearance->total_cycle_duration() is new on 130702 and +500 to work around bad selection region
						tool_in_hand->do_after(duration,tool_in_hand,TELL_PROGRAMMER_AM_READY_CALLBACK); 
						// new on 031201 so that selection region is set properly
					};
				};
			} else {
				((Robot *) appearance)->pick_up_and_store_tool(tool_in_hand);
			};
		};
#if TT_DIRECT_INPUT
      city_coordinate size = tool_in_hand->current_width()*tool_in_hand->current_height();
      const city_coordinate square_tile = tile_width*tile_height;
      if (size < 4*square_tile) {
         set_background_force_feedback_effect("Weight Light");
      } else if (size > 100*square_tile) {
         set_background_force_feedback_effect("Weight Heavy");
      } else {
         set_background_force_feedback_effect("Weight");
      };
#endif
	};
	if (hand == ABOUT_TO_RELEASE_ITEM && tt_current_time >= release_time) {
		// need to wait a frame for stuff to settle down 
		// e.g. robot released item actually added to screen
		if (release_time == 0) {
			status = release_object();
		} else {
			release_time = 0;
		};
	};
// move_appearance(delta_x,delta_y); 
	// was here prior to 170402 but better to happen much earlier (for selection and computing need to scroll)
   move_arm(); // need to do this after the above since hand may move multiple times
   if ((tt_2_mouse_buttons_is_escape && all_mouse_buttons()) || // like escape
       (appearance->kind_of() == ROBOT_IN_TRAINING && ((Robot *) appearance)->is_inert())) {
      return(get_up());
   };
	if (extended_key && !arrow_key(key) && (key == VK_F11 || puzzle_oks_function_keys(appearance))) { 
		// added F11 test on 070604 since has been working but made error sound
		// added !arrow_key(key) on 011100 so puzzles don't make a sound when you use the arrow keys
		switch (key) {
		  case VK_F1:
            if (tt_system_mode != PUZZLE) {
//					tt_martian = NULL; // to test dumping
					tt_martian->teleport_toggle(current_selection,tool_in_hand);
				};
				break;
         case VK_NEXT: // page down
            user_did(GLOBAL_HISTORY,USER_HAS_TYPED_PAGE_DOWN);
            tt_martian->dont_wait_to_say_more();
            break;
         case VK_PRIOR: // page up
//            user_did(GLOBAL_HISTORY,USER_HAS_TYPED_PAGE_UP);
            tt_martian->repeat_last_thing_said();
            break;
		   case VK_F2:
			   if (tt_toolbox != NULL) {
				   tool_called_for(tt_toolbox_vacuum,TWING_SOUND,key);
				};
				break;
		   case VK_F3:
			   if (tt_toolbox != NULL) {
				   tool_called_for(tt_toolbox_expander,BIKE_BELL_SOUND,key);
				};
				break;
		   case VK_F4:
			   // 140599 rewrote all of these to use tool_called_for
			   // and changed the meaning of this one to call for the main notebook
			   // not the most recent one used
			   if (tt_toolbox != NULL) {
				   tool_called_for(tt_toolbox_notebook,WHIT_SOUND,key);
				};
			   break;
		   case VK_F5:
			   if (tt_toolbox != NULL) {
				   tool_called_for(tt_toolbox_copier,GLASS_HIGH_SOUND,key);
				};
			   break;
			case VK_F6:
				if (tt_toolbox != NULL && !tt_toolbox->gone_for_good()) { // new on 170999
					tool_called_for(tt_toolbox,TWING_SOUND,key);
				};
			   break;
		  case VK_F7:
			  tt_finish_all_animations = TRUE;
			  user_did(GLOBAL_HISTORY,USER_HAS_HURRIED_UP,appearance);
			  return(status);
// commented out since this is handled now for all states of the programmer
//	  case VK_F9:
//			  tt_game_switch_on = !tt_game_switch_on; 
//			  hand_visibility(tt_game_switch_on);
//			  break;
		};
		if (tt_log_version_number < 45) { // removed  || tool_in_hand == NULL on 100604
			return(status); // new policy on 100604 to pass extended keys to things held
		};
	};
	// ordinary character
   if (key == 0 && tool_in_hand == NULL && virtual_right_button(button_status,FALSE) && // && tt_new_input_scheme
       (current_selection == NULL || current_selection->kind_of() != TEXT)) {
      // act like a space bar was typed when pointing to something -- except for TEXT
      key = ' ';
   };
	switch (key) {
		  case 0:
			  return(status); // nothing there
		  case VK_ESCAPE:
			  if (retraining_stopped_on_frame == tt_frame_number) { // already reacted to Escape so ignore this
				  return(status);
			  };
			  return(get_up());
		  default:
			  if (key == '\003' && !extended_key) { // control-c added on 010499
				  // moved here on 150599 so can also use to this to copy the floor when empty handed
			     put_on_clipboard(tool_in_hand,floor,appearance,key);
				  play_sound(POP_SOUND);
			  } else if (extended_key && arrow_key(key) && tt_log_version_number < 45) { 
				  // || tool_in_hand == NULL || !tool_in_hand->can_be_text())) {
				  // added extended_key on 131100 since lost 4 characters, e.g. (
				  // new on 011100 to do nothing
				  // excluded case where holding a text or number pad on 090604 -- updated it on 100604
			  } else if (hand == HOLDING_ITEM || // && tool_in_hand->kind_of() != VACUUM) || 
				          hand == HOLDING_COPIER || 
				          hand == USED_COPIER) {
              if (tool_in_hand->is_tool()) { // made conditional on 250402 so hand doesn't move when typing number or text
                 tt_hand_needs_to_move_for_tool_use = TRUE;
              };
              boolean was_off = !tool_in_hand->is_on();
				  ProgrammerStatus old_status = tt_programmer->return_next_status(); 
				  // new on 200105 since now the following can change status
				  new_tool_in_hand(tool_in_hand->respond_to_keyboard(key,extended_key,FALSE,appearance,TRUE,tool_in_hand));
				  ProgrammerStatus new_status = tt_programmer->return_next_status();
				  if (old_status != new_status) {
					  status = new_status; // so change is returned back up
				  };
				  play_sound(TYPING_SOUND);
              if (was_off && tool_in_hand != NULL && tool_in_hand->is_on()) { // keyboard turned it on so (new on 270902)
                 // added tool_in_hand test on 221002 since could be a bomb that is no longer in the hand
                 use_tool(FALSE,x,y); // x and y new on 081004
              };
			  } else {   // if (tt_mouse_button_count > 0)
              // not holding anything but might be above something
				  if ((key == toggle_infinite_stack || key == toggle_graspable) && current_selection == NULL) {
					  // new on 160500 so you can turn off glue
					  current_selection = selection(TRUE);
				  };
				  if (current_selection != NULL &&
                  (tt_mouse_button_count > 0 || current_selection->kind_of() != TEXT) &&
                  !button_modified()) {
                 // if -b 0 then text must be picked up to be typed at
					  if (current_selection->pointer_to_leader() != NULL &&
							current_selection->pointer_to_leader()->kind_of() == PROGRAM_PAD) {
						  // keyboard events should go to notebook not one of its pages
						  current_selection = current_selection->pointer_to_leader();
						  tt_selection = current_selection; // new on 111100 so sensors work right away
						  // so unselect isn't called so for example
						  // notebook can get multiple digits for page number
//						  old_selection = current_selection;
					  };
					  // commented out the conditional on 260500 since now becomes a special follower...
//               if (!current_selection->infinite_stack() || key == toggle_infinite_stack || key == toggle_graspable) {
						  // key == toggle_graspable added 160500
						  // tried commenting out the conditional on 160500 since may want to start a stack of games or behaviors
					     // but then you really want to send the key to a copy of the stack but then such copies can easily
						  // be made accidentially and lead to confusion
						  if ((current_selection->infinite_stack() && !extended_key) || 
							   ((key == toggle_infinite_stack || key == toggle_graspable) && !current_selection->graspable())) { 
							  // added extended_key on 191004 since function keys shouldn't trigger the following
							  // added graspable conditions on 021204 so can toggle graspability -- 
							  // note may wish to change glued down to also be an infinite stack
//							  if (current_selection->pointer_to_leader() == NULL) { 
//								  // new on 070800 - e.g. glued to floor
//								  current_selection = current_selection->selected(NO_SELECTION_REASON_GIVEN,appearance); 
									// so a copy is made
//							  } else {
							  if (key == toggle_infinite_stack || key == toggle_graspable) { // conditional new on 111000
								  if (current_selection != tt_toolbox && (current_selection->pointer_to_leader() == NULL || 
									   current_selection->pointer_to_leader()->kind_of() != TOOLBOX)) { 
									  // conditions to ignore Tooly or contents of Tooly on 080604
			//						  current_selection = current_selection->top_of_stack_or_create(appearance); // commented out on 021204
									  // following new on 021204
									  if (key == toggle_infinite_stack) {
										  current_selection->set_infinite_stack(!current_selection->infinite_stack());
									  } else if (key == toggle_graspable) {
										  current_selection->set_graspable(!current_selection->graspable());
										  current_selection->set_priority_fixed(!current_selection->graspable()); 
										  // new on 060605 priority is fixed if not graspable
									  };
									  tt_selection = current_selection; // new on 111100 so sensors work right away
								  } else {
									  return(status); // I guess this is the best thing
								  };
							  } else if (current_selection->pointer_to_leader() == NULL || 
											 current_selection->pointer_to_leader()->kind_of() != PROGRAM_PAD) {
								  // condition new on 180204 since this behaviour is desirable 
								  // when something is glued to the floor or in a box
								  // was != PROGRAM_PAD was == TOOLBOX prior to 240304
								  // but not when in a Tooly for example
								  // new on 111000
								  Sprite *original_selection = current_selection; // new on 191004
								  current_selection = current_selection->top_level_copy();
								  tt_selection = current_selection; // new on 111100 so sensors work right away
								  if (floor != NULL) { 
									  floor->add_item(current_selection);
									  // added current_selection as arg below on 220804 so behaves like grasp of a stack on the floor
									  current_selection->animate_to_good_size(1000,current_selection); // new on 240304
								  };
								  // what about record_event?? -- not a problem as of 180204 since won't happen while training
								  // comment above not true so added following on 191004
								  record_event(SELECT_STACK_ITEM,appearance,appearance->pointer_to_background(),original_selection,TRUE);
								  // following is just to create an index
								  record_event(NEW_ITEM,appearance,appearance->pointer_to_background(),current_selection,TRUE,0,
													ADD_TO_WORKING_SET_REGARDLESS);
							  } else {
								  return(status); 
								  // new on 180204 so if in Toolbox (or other leader) don't do this -- 
								  // as of 240304 only Notebooks take this path
							  };
						  };
						  if (key != toggle_infinite_stack && key != toggle_graspable) { // conditional new on 230103
							  current_selection->respond_to_keyboard(key,extended_key,FALSE,appearance,TRUE,current_selection);
							  tt_hand_needs_to_move_for_tool_use = TRUE;
							  play_sound(TYPING_SOUND);
							  if (!current_selection->clean() &&
									current_selection->pointer_to_leader() != NULL &&
									current_selection->pointer_to_leader()->kind_of() == PROGRAM_PAD) {
								  // keystrokes changed something inside a notebook so record it
								  Notebook *notebook = (Notebook *) (current_selection->pointer_to_leader());
								  // might be say animating a page flip
								  current_selection->finish_instantly(FALSE,TRUE); 
								  // added TRUE on 010399 so that this happens even if power is off  
								  notebook->save_encoding();
							  };
						   };
//					  };
				  };
			  };
			  return(status);
	};
};

void Programmer_At_Floor::move_appearance(boolean moved, city_coordinate delta_x, city_coordinate delta_y) { 
	// abstracted to a method on 170402
	if (tt_log_version_number >= 47) {
		current_delta_x = delta_x; // new on 010704 to deal with tool's selection region while moving hand
		current_delta_y = delta_y;
	};
   if (!tt_programmer->invisible() || tool_in_hand != NULL) {
		// don't cache programmer since it is nearly always moving...
		appearance->set_clean_status(MOVED);
		appearance->update_display(delta_x,delta_y); // always cycle to final image
#if TT_DEBUG_ON
		if (tt_debug_mode == 180703) {
			tt_error_file() << "New deltas are " << delta_x << "," << delta_y << endl;
		};
#endif
	};
//   Sprite *real_tool_in_hand = tool_in_hand; // new on 190402
//   if (real_tool_in_hand != NULL && tool_in_hand->kind_of() == COPIER) { 
//		Sprite *attached = ((Copier *) tool_in_hand)->attached_tool();
//		if (attached != NULL) real_tool_in_hand = attached;
//	};
	if (tool_in_hand != NULL && moved) {
		tool_in_hand->set_clean_status(MOVED);
      if (tt_hand_needs_to_move_for_tool_use && absolute_mouse_mode()) { // e.g. tool is a follower of robot - new on 180402
         if (using_arm) {
            tool_in_hand->move_by(delta_x,delta_y);
         } else {
            appearance->propagate_changes();
         };
		};
	};
	if (hand != EMPTY_HAND && (hand != ABOUT_TO_RELEASE_ITEM || (absolute_mouse_mode() && !tt_hand_needs_to_move_for_tool_use))) { 
      // if absolute mouse then need to update before drop
      if (using_arm) {
//#if TT_BUG_FIXES
//			 && tt_current_time != tt_initial_current_time 
			// new on 021203 to not run this since just loading and have yet to display this
//			 // so true_width() and the like aren't up-to-date (maybe they should be...)
//#endif	
			// maintain copier wand so attached stuff tracks
			// need to maintain good spatial relation
			city_coordinate ideal_tool_llx, ideal_tool_lly,
								 adjustment_x, adjustment_y;
			// conditionalize this on using_arm...
			appearance->lower_left_corner(ideal_tool_llx,ideal_tool_lly);
			// from right side aligned with 1/2 to right side of hand without adjustment
			// and vertical center aligned with vertical center of hand
			tool_in_hand->adjust_grasp(adjustment_x,adjustment_y);
			// cache appearance->true_width()/2 ??
			city_coordinate tool_width, tool_height;
//			if (tool_in_hand->kind_of() == PICTURE && tool_in_hand->show_all()) { // want the frame to be stable not the contents
			// THIS DIDN'T FIX THINGS!!!
//			if (!moved && tool_in_hand->stable_while_in_hand()) { // new on 210901 - didn't move and stability is important
//				tool_width = tool_in_hand->current_width();
//				tool_height = tool_in_hand->current_height();
//			} else {
			tool_width = tool_in_hand->true_width();
			tool_height = tool_in_hand->true_height();
//			};
			city_coordinate grasp_delta_x = tt_programmer->return_grasp_delta_x();
			city_coordinate grasp_delta_y = tt_programmer->return_grasp_delta_y();
			ideal_tool_llx -= tool_width-appearance->true_width()/2;
			ideal_tool_llx +=	adjustment_x;
			ideal_tool_llx += grasp_delta_x;
			ideal_tool_lly -= tool_height/2-appearance->true_height()/2;
			ideal_tool_lly	+=	adjustment_y;
			ideal_tool_lly += grasp_delta_y;
			if (tt_log_version_number >= 59) { // new on 121104
				if (grasp_delta_x != 0) {
					ideal_tool_llx += (tool_width-tt_programmer->return_width_at_time_of_grasp());
				};
				if (grasp_delta_y != 0) {
					ideal_tool_lly += (tool_height-tt_programmer->return_height_at_time_of_grasp());
				};
			};
			// added offsets on 120604
			//if ((ideal_tool_llx+tool_width > 0 && ideal_tool_lly+tool_height > 0) || !tool_in_hand->animation_in_progress() || 
			//	 tt_log_version_number < 76) {
				// condition new on 290505 so when flipping a big picture it doesn't end up offscreen
			// removed condition on 030605 since this was the wrong fix
			tool_in_hand->move_to(ideal_tool_llx,ideal_tool_lly); 
//#if TT_DEBUG_ON
//			} else {
//				tt_error_file() << "OK?" << endl; // alway OK?
//#endif
//			};		
      };
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 180703) {
		tt_error_file() << "New current_llx and current_lly are " << appearance->current_llx() << "," << appearance->current_llx() 
							 << " on frame " << tt_frame_number << " during segment " << tt_current_log_segment << endl;
	};
#endif
};

void Programmer_At_Floor::move_arm() { // abstracted out on 190402
   if (using_arm) {
      city_coordinate x,y;
		// "glue arm to the lower right corner
		// better to work out concrete values for each hand image
		appearance->lower_left_corner(x,y);
		x += appearance->true_width();
//      if (tt_mouse_mode) { // moved here on 170402
//         y = delta_y; // y is where the hand will go (new on 170402)
//      } else {
//		     y = delta_y+appearance->current_lly();       
//      };
//      y = appearance->current_lly(); // commented out on 180602 since this is redundant
		// arm has only one image so true_width == current_width
		// adjust a bit to really fit
		if (hand == USED_COPIER) {
			arm->move_to(x-(80*arm->current_width())/100,
							 y-(90*arm->current_height())/100);
      } else if (hand == ABOUT_TO_RELEASE_COPIER) {
			arm->move_to(x-(90*arm->current_width())/100,
							 y-(92*arm->current_height())/100);
      } else if (hand == HOLDING_COPIER) {
			arm->move_to(x-(90*arm->current_width())/100,
							 y-(92*arm->current_height())/100);
      } else if (hand == USING_COPIER) {
			arm->move_to(x-(90*arm->current_width())/100,
							 y-(92*arm->current_height())/100);
		} else {
         int extra_x = 0;
			switch(appearance->current_parameter()) {
				case HAND_GRASP_VERTICAL:
					// use true_width etc instead??
               if (appearance->image_cycle_index() == 0) extra_x = 4;
					arm->move_to(x-((17+extra_x)*arm->current_width())/20,
									 y-(19*arm->current_height())/20);
					break;
				case HAND_RELEASE_VERTICAL: // since it ends with pointing
               if (appearance->image_cycle_index() == 0) extra_x = -4;
               if (appearance->image_cycle_index() == 1) extra_x = -2;
               arm->move_to(x-((21+extra_x)*arm->current_width())/20,
									 y-(19*arm->current_height())/20);
					break;
				case HAND_RELEASE_HORIZONTAL:
               if (appearance->image_cycle_index() == 1) extra_x = -2;
               arm->move_to(x-((21+extra_x)*arm->current_width())/20,
									 y-(19*arm->current_height())/20);
					break;
				case HAND_OPEN:
					arm->move_to(x-(21*arm->current_width())/20,
									 y-(19*arm->current_height())/20);
					break;
				case HAND_GRASP_HORIZONTAL:
               if (appearance->image_cycle_index() == 0) extra_x = -2;
               if (appearance->image_cycle_index() == 1) extra_x = -4;
					arm->move_to(x-((23+extra_x)*arm->current_width())/20,
									 y-(19*arm->current_height())/20);
					break;
			};
		};
		arm->just_set_clean_status(MOVED); // moved but not dirty for sure
		arm->update_display();
	};
};

void Programmer_At_Floor::stop_retraining(Robot *robot) {
//	play_sound(POP_SOUND); // too often last act also makes a sound -- is confusing
	boolean running = robot->still_running();
	robot->stop_running_body(); 
	if (running) {
		// since a grasp or the like might be in progress
	   robot->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
	} else {
		robot->stop_animations(); // stopping it before even try_clause runs
	};
	robot->set_working_cubby(NULL);
	if (!robot->exhausted_training_counter()) robot->appear_working();
	update_tool_in_hand(robot);
};

void Programmer_At_Floor::update_tool_in_hand(Robot *robot) {
//	tool_in_hand = robot->pointer_to_tool();
//	if (tool_in_hand == NULL) {
//		hand = EMPTY_HAND;
//	} else if (tool_in_hand->kind_of() == COPIER) {
//		hand = HOLDING_COPIER;
//	} else {
//		hand = HOLDING_ITEM;
//	};
	set_tool_in_hand(robot->pointer_to_tool()); // re-written on 220901
	tt_programmer->set_tool_in_hand(tool_in_hand);
//	programmer->set_hand_state(hand);
};

void Programmer_At_Floor::called_for(Sprite *tool) {
	if (waiting_for == tool) return;
//	postpone_new_logs(); // new on 170100
	if (waiting_for != NULL) {
		waiting_for->stop_running();
	};
	if (tool->kind_of() != PROGRAM_PAD && tool->kind_of() != TOOLBOX) {
		// added conditional on 090699 so that don't wait for tools that don't jump in your hand
		waiting_for = tool;
	};
};

boolean Programmer_At_Floor::shift_view(Direction direction, city_coordinate &delta_x, city_coordinate &delta_y) {
// COMBINE  shift_sit_corner AND  set_sit_corner!!!!!!!!!!!
//   TTRegion containment_region;
//   if (!floor->current_containment_region(containment_region)) return(FALSE);
   city_coordinate new_sit_llx, new_sit_lly, new_sit_urx, new_sit_ury;
   tt_programmer->shift_sit_corner(direction,new_sit_llx,new_sit_lly,new_sit_urx,new_sit_ury);
//   if (new_sit_llx < containment_region.min_x) {
//      new_sit_llx = containment_region.min_x;
//   } else if (new_sit_urx > containment_region.max_x) {
//      new_sit_llx -= (new_sit_urx-containment_region.max_x);
//   };
//   if (new_sit_lly < containment_region.min_y) {
//      new_sit_lly = containment_region.min_y;
//   } else if (new_sit_ury > containment_region.max_y) {
//      new_sit_lly -= (new_sit_ury-containment_region.max_y);
//   };
   if (tt_programmer->set_sit_corner(new_sit_llx,new_sit_lly,floor,delta_x,delta_y)) { // false if un-changed
		tt_shift_frame_number = tt_frame_number;
#if TT_DEBUG_ON
		if (tt_debug_mode == 190304) {
			tt_error_file() << "tt_shift_frame_number set to " << (int) tt_shift_frame_number << " during segment " << tt_current_log_segment << endl;
		};
#endif
      return(TRUE);
   } else {
#if TT_DEBUG_ON
		if (tt_debug_mode == 190304) {
			tt_error_file() << "Playing thud in Programmer_At_Floor::shift_view during segment " 
								 << tt_current_log_segment << " on frame " << tt_frame_number << endl;
		};
#endif
      play_sound(BOOK_DROP_SOUND); // thud since up against the "wall"
      return(FALSE);
   };
};

void Programmer_At_Floor::drop(ProgrammerStatus &status) {
   move_count = 0; // new on 130502 - reset counter -- OK if dropped from end of wand??
   if (using_arm) {
      if (hand == HOLDING_COPIER) {
			release_copier();
      } else if (tt_finish_all_animations) {
			status = release_object(); // do it now
      } else if (hand == USED_COPIER) {
         status = release_object(); // do it now
         move_count = 1; // new on 110602
      } else {
			hand_release_cycle();
         // hand still has stuff to do after releasing
         hand = ABOUT_TO_RELEASE_ITEM;
         release_time = tt_current_time+appearance->total_cycle_duration()/3;
      };
	} else {
		hand = ABOUT_TO_RELEASE_ITEM;
//			release_time = tt_current_time+
		((Robot *) appearance)->begin_drop_action();
      release_time = 0; // for more accurate dropping -- come back to this
   };
};

void Programmer_At_Floor::hand_release_cycle() {
	if (horizontal_shape(tool_in_hand->kind_of())) {
		appearance->set_parameter(HAND_RELEASE_HORIZONTAL);
	} else {
		appearance->set_parameter(HAND_RELEASE_VERTICAL);
	};
};

void wand_counter_location(HandState hand,
                           int &x_percent, int &y_percent) {
   switch (hand) {
      case HOLDING_COPIER:
         x_percent = 54;
         y_percent = 44; // was 42
         break;
      case USED_COPIER:
         x_percent = 61;
         y_percent = 69;
         break;
//      case USING_COPIER:
   };
};

void Programmer_At_Floor::display_appearance() {
 //  if current_selection ...
	//	TTRegion selection_region;
	//	current_selection->full_region_including_outside_followers(selection_region); // was true_region
	//	// some how there is an off by one problem here -- following patches it
	//	selection_region.min_x -= 3*tt_screen->one_pixel_in_x_city_units();
	//	selection_region.min_y -= 3*tt_screen->one_pixel_in_y_city_units();
	//	selection_region.max_x += 3*tt_screen->one_pixel_in_x_city_units();
	//	selection_region.max_y += 3*tt_screen->one_pixel_in_y_city_units();
 //     current_selection->display(MOVEMENT_FEEDBACK);
	//	current_selection->set_clip_region(NO_FEEDBACK,&selection_region); // MOVEMENT_FEEDBACK
	//	tt_screen->redisplay_above(current_selection,selection_region);
	//	unclip();
	//};
	SelectionFeedback feedback = NO_FEEDBACK;
	if (tool_in_hand != NULL && !tool_in_hand->followers_completely_inside()) {
		feedback = NO_FEEDBACK_BUT_DISPLAY_OUTSIDE_FOLLOWERS;
	};
	if (appearance->kind_of() == ROBOT_IN_TRAINING) {
//		if (!tt_programmer->invisible()) // redundant test so commented out on 200704
		Programmer_State::display_appearance();
		if (tool_in_hand != NULL) {
			tool_in_hand->display(feedback);
		};
	} else {
		if (tool_in_hand != NULL && tool_in_hand->kind_of() != COPIER) {
			tool_in_hand->display(feedback);
#if TT_DEBUG_ON
			if (tt_debug_mode == 50304) { // see GUID of thing in hand
				GUID *guid = tool_in_hand->pointer_to_guid();
				if (guid != NULL) {
					print_guid_as_hexadecimal(guid,tt_error_file());
				};
			};
#endif
		};
		if (!tt_programmer->invisible()) {
			Programmer_State::display_appearance();
			if (using_arm) {
//            boolean shift_arm
//              = (hand == USING_COPIER &&
//                 appearance->current_parameter() == HAND_USING_WAND); // &&
//              appearance->pointer_to_current_image()->return_resource_index() == 356); // fix this!!!!
//            if (shift_arm) {
//               arm->move_by(tile_width,0);
//            };
      		arm->display();
//            if (shift_arm) {
//               arm->move_by(tile_width,0);
//            };
			};
         if (appearance->cycle_stopped() &&
             (hand == HOLDING_COPIER || hand == USED_COPIER)) { //appearance == hand_with_wand
            city_coordinate counter_llx,counter_lly;
            appearance->lower_left_corner(counter_llx,counter_lly);
            int x_percent,y_percent;
            wand_counter_location(hand,x_percent,y_percent);
            counter_llx += (x_percent*appearance->current_width())/100;
            counter_lly += (y_percent*appearance->current_height())/100;
            ((Copier *) tool_in_hand)->display_counter(counter_llx,counter_lly);
         };
		};
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 111) {
//		TTRegion region;
//		if (tool_in_hand == NULL) {   // not quite right when holding wand
//			appearance->collision_region(region);
//		} else {
//			tool_in_hand->active_region(region);
//		};
BEGIN_GDI
		tt_main_window->select_stock_object(NULL_BRUSH);
		tt_main_window->select_stock_object(WHITE_PEN);
		tt_screen->draw_rectangle(current_region.min_x,current_region.max_x,
										  current_region.min_y,current_region.max_y);
		tt_main_window->select_stock_object(NULL_PEN); // default
END_GDI
	};
#endif
	if (martian_active() && tt_system_mode != PUZZLE &&
		 tt_programmer->return_next_status() != PROGRAMMER_RUNNING_TIMER) {
		if (tool_in_hand == NULL) {
		   TTRegion region;
			appearance->full_region(region);
			region.extend_with(current_region);
			tt_martian->keep_out_of_the_way(region);
		} else {
			tt_martian->keep_out_of_the_way(current_region);
		};
//		if (current_selection != NULL || tool_in_hand != NULL) {
		if (selection_feedback_flag) { // && (tt_martian->called_by_user() || tt_log_version_number <= 20)) { 
			// commented out the whole second condition on 010400 since give_help is smarter now
			// second conjunct added on 140300 to prevent help being given if beaming in to give specific advice
			// added staying_around() on 200300 so you get help if Marty is there because he beamed in himself 
			// but not due to an error or advice
			// removed || tt_martian->staying_around() on 210300 since now inside of give_help
			tt_martian->give_help(current_selection,tool_in_hand);
		};
	};
};

boolean Programmer_At_Floor::save_appearance_regions() {
	boolean on_screen = FALSE;
	if (current_selection != NULL && selection_feedback_flag) {
//		current_selection->save_region(screen);
		TTRegion region;
		current_selection->full_region_including_outside_followers(region);
		// grow region by 1 pixel in every direction
		const city_coordinate one_x_pixel = tt_screen->one_pixel_in_x_city_units();
		const city_coordinate one_y_pixel = tt_screen->one_pixel_in_y_city_units();
		region.min_x -= one_x_pixel;
		region.max_x += one_x_pixel;
		region.min_y -= one_y_pixel;
		region.max_y += one_y_pixel;
#if TT_DEBUG_ON
      if (tt_debug_mode == 103) {
         tt_error_file() << "Current selection: " << type_name(current_selection) << " ";
		};
#endif
		if (tt_screen->save_city_region(region)) on_screen = TRUE;
	};	
	if (Programmer_State::save_appearance_regions()) on_screen = TRUE;
	if (tool_in_hand != NULL && 
		 (!using_arm || tool_in_hand->kind_of() != COPIER)) {
//		if (tool_in_hand->save_region(screen)) on_screen = TRUE;
		TTRegion region;
		tool_in_hand->full_region_including_outside_followers(region);
#if TT_DEBUG_ON
      if (tt_debug_mode == 103) {
         tt_error_file() << "Tool in hand: " << type_name(tool_in_hand) << " ";
		};
#endif
		if (tt_screen->save_city_region(region)) on_screen = TRUE;
	};
	if (using_arm) {
		if (arm->save_region()) on_screen = TRUE;
	};
	return(on_screen);
};

//city_coordinate programmer_x_speed = 0;
//city_coordinate programmer_y_speed = 0;

void Programmer_At_Floor::recent_speed(city_coordinate &x_speed, 
													city_coordinate &y_speed) {
	// rationalized on 101199
	millisecond total_duration = 0;
	city_coordinate total_delta_xs = 0;
	city_coordinate total_delta_ys = 0;	
	for (int i = 0; i < number_of_deltas; i++) {
		total_delta_xs += delta_xs[i];
		total_delta_ys += delta_ys[i];		
		total_duration += delta_times[i];
	};
	if (total_duration > 0) {
		// speed in city units per millisecond
		x_speed = (1000*total_delta_xs)/total_duration;
		y_speed = (1000*total_delta_ys)/total_duration;
	} else {
		x_speed = 0;
		y_speed = 0;
	};
};

boolean can_grasp(Sprite *sprite, void *arg) {
	// added infinite_stack on 021204 so can glue down infinite stacks
	return(sprite->graspable() || sprite->infinite_stack());
};

boolean can_receive(Sprite *sprite, void *arg) {
	if (!sprite->graspable() || !sprite->selectable()) return(FALSE);
	// added selectable test on 200105 due to Logotron bug 20.2 with its delayed occur check
	if (sprite->is_tool()) return(FALSE);
	if (sprite->kind_of() == BOMB) return(FALSE);
	return(TRUE);
};

boolean Programmer_At_Floor::compute_selection_region(TTRegion &region) { // abstracted to a method on 170402
   if (using_arm) {
		appearance->collision_region(region);
	} else {
		appearance->pointer_region(region);
	};
   return(TRUE); // on floor so always have a selection region
};

Sprite *Programmer_At_Floor::selection(boolean even_if_ungraspable) {
	if (appearance->kind_of() == ROBOT_IN_TRAINING && ((Robot *) appearance)->still_running()) {
		// retraining a robot and it is still in control
		return(NULL);
	};
	Sprite *underneath;
	SelectionReason reason;
	Sprite *attached;
	saved_reason = NO_SELECTION_REASON_GIVEN; // by default
   Sprite *real_tool_in_hand = tool_in_hand;
   if (absolute_mouse_mode()) { // new on 190402 so selection feedback tracks the mouse
//      absolute_mouse_position(current_region.min_x,current_region.min_y);
      last_mouse_deltas(current_region.min_x,current_region.min_y); // updated 220402
      current_region.max_x = current_region.min_x;
      current_region.max_y = current_region.min_y;
      if (tool_in_hand != NULL && !tool_in_hand->is_tool()) { 
         // mouse is where center of tool in hand will end up
         city_coordinate half_tool_width = tool_in_hand->current_width()/2;
         city_coordinate half_tool_height = tool_in_hand->current_height()/2;
         current_region.min_x -= half_tool_width;
         current_region.max_x += half_tool_width;
         current_region.min_y -= half_tool_height;
         current_region.max_y += half_tool_height;
      };
   };
	switch (hand) {
	  case EMPTY_HAND:
		  reason = MIGHT_GRASP;
		  saved_reason = GRASPED;
		  if (moved && !absolute_mouse_mode()) {
           compute_selection_region(current_region);			  
		  };
		  underneath = tt_screen->first_tool_to_collide_with(&current_region);
        // when first_tool... was called with can_grasp then it found
        // graspable stuff buried under ungraspable -- too wierd
		  if (!even_if_ungraspable && underneath != NULL && !underneath->graspable() && !underneath->infinite_stack()) {
			  // added infinite_stack test on 021204 since can have glued down infinite stacks
			  underneath = NULL;
		  };
		  if (underneath == NULL && using_arm) {
			  // if the active region of hand is too "short" 
			  // then it can't pick up what you expect
			  // if it is too "long" it might pick up "wrong" thing if there is more
			  // than one
			  // Here it uses the active region of the hand (finger tip)
			  // and if there is nothing
			  // tries again with rest of finger
			  // returns TRUE if anything underneath
			  TTRegion finger_region;
			  appearance->collision_region(finger_region);
//			  current_region.min_x = finger_region.min_x;
//			  current_region.max_x = finger_region.max_x;
//			  current_region.min_y = finger_region.min_y-appearance->current_height()/2;
//			  current_region.max_y = finger_region.min_y;
//			  underneath = tt_screen->first_tool_to_collide_with(&current_region);
			  finger_region.max_y = finger_region.min_y;
			  finger_region.min_y -= appearance->current_height()/2;
			  underneath = tt_screen->first_tool_to_collide_with(&finger_region);
           if (underneath != NULL && !underneath->graspable()) underneath = NULL;
		  };
		  break;
	  case HOLDING_COPIER:
	  case USED_COPIER:
		  attached = ((Copier *) tool_in_hand)->attached_tool();
		  if (attached != NULL) { // need to get drop feedback
//			  if (tt_log_version_number > 12) {
				  long min_priority = max(attached->priority(),appearance->priority())+1;
				  underneath = tt_screen->first_to_collide_with(attached,min_priority,can_grasp); 
//					attached->collision_region(current_region);
//					underneath = tt_screen->first_to_collide_with(&current_region,appearance);
				  if (underneath != NULL) {
					  if (!absolute_mouse_mode()) attached->collision_region(current_region);
					  return(underneath->select(&current_region,MIGHT_DROP_ITEM,attached));
				  } else {
					  return(NULL);
				  };
//			  } else {
//				  underneath = NULL;
//			  };
		  } else {
			  reason = MIGHT_USE_COPIER;
			  saved_reason = COPIER_USED;
			  if (moved) {
				  // use upper left corner
				  appearance->lower_left_corner(current_region.min_x,current_region.min_y);
				  city_coordinate width = appearance->true_width(),
										height = appearance->true_height();
				  current_region.max_x = current_region.min_x+width/8;
				  current_region.max_y = current_region.min_y+height;
				  current_region.min_y = current_region.max_y-height/8;
			  };
			  underneath = tt_screen->first_tool_to_collide_with(&current_region,tool_in_hand);
			  break;
		  }; // if not using arm fall thru to below
	  case HOLDING_ITEM:
//		  if (tt_log_version_number > 12) 
		  {
			  if (real_tool_in_hand->kind_of() == COPIER) { //  && tt_log_version_number > 12
					Sprite *attached = ((Copier *) tool_in_hand)->attached_tool();
					if (attached != NULL) real_tool_in_hand = attached;
			  };
//			  boolean holding_a_tool = is_wand(real_tool_in_hand->kind_of()); // noticed this was obsolete on 220901
			  boolean holding_a_tool = real_tool_in_hand->is_tool();
           if (!absolute_mouse_mode()) {
			     if (holding_a_tool && use_selection_region && tt_log_version_number > 27) { // new on 220901
				     current_region = selection_region;
				     city_coordinate llx,lly;
				     appearance->lower_left_corner(llx,lly);
				     current_region.shift_by(llx,lly); // stored relative to hand coordinates
			     } else {
				     real_tool_in_hand->active_region(current_region);
			     };
           };
			  // noticed that both branches did the same thing so simplified on 220901
//			  if (using_arm || !holding_a_tool) {
				  // arm or robot holding non wand
//				  real_tool_in_hand->active_region(current_region);
//			  } else { // if (moved) { // ok ?? -- sometimes the tool moved by itself -- e.g. wand use
//				  real_tool_in_hand->active_region(current_region);
//			  };
           if (!absolute_mouse_mode()) {
				  //if (tt_log_version_number >= 38 && real_tool_in_hand->is_tool()) {
					 // // new on 290204
					 // TTRegion active_region_of_tool;
					 // real_tool_in_hand->active_region(active_region_of_tool);
					 // underneath = tt_screen->first_tool_to_collide_with(&active_region_of_tool,real_tool_in_hand);
					 // if (underneath != NULL) {
						//  	switch (real_tool_in_hand->kind_of()) {
						//		case EXPANDER:
						//			reason = MIGHT_USE_EXPANDER;
						//			break;
						//		case VACUUM:
						//			reason = MIGHT_USE_VACUUM;
						//			break;
						//		case COPIER:
						//			reason = MIGHT_USE_COPIER;
						//			break;
						//	};
						//   underneath = underneath->select(&active_region_of_tool,reason,real_tool_in_hand);
					 // };
				  //} else {
					  // new on 190402
				  if (holding_a_tool) { // && use_selection_region) { // && tt_log_version_number > 27) { // new on 220901
					 // if (use_selection_region) { // moved out of the conditional above on 290204
				  //      current_region = selection_region;
						//};
						underneath = tt_screen->first_tool_to_collide_with(&current_region,real_tool_in_hand);
					} else {
						// new on 290204
					   underneath = drop_recipient(real_tool_in_hand,real_tool_in_hand->priority());
				   };
				  // does the following makes sense? - asked 290204
			  } else if (!holding_a_tool) { // tool selection isn't by drop ... tt_log_version_number > 12 && 
				  long min_priority = max(real_tool_in_hand->priority(),appearance->priority())+1;
				  underneath = tt_screen->first_to_collide_with(real_tool_in_hand,min_priority,can_grasp);
			  } else if (using_arm) {
				  underneath = tt_screen->first_tool_to_collide_with(&current_region,real_tool_in_hand);
			  } else { // except for the robot (and what it is holding -- being above it)
				  underneath = tt_screen->first_to_collide_with(&current_region,appearance);
				  if (underneath == appearance) { // a robot in training picked up something big
					  underneath = NULL; // ignore it
				  };
			  };
			  if (tt_log_version_number < 38 && underneath == NULL && moved && !holding_a_tool) {
				  // made inoperational (except for old logs) on 290204
				  // try again with full region of non-wand
				  real_tool_in_hand->full_region(current_region);
				  underneath = tt_screen->first_tool_to_collide_with(&current_region,real_tool_in_hand);
			  };
			  if (underneath != NULL) {
 #if TT_DEBUG_ON
				  if (tt_debug_mode == 220803) {
					  tt_error_file() << "On frame " << tt_frame_number << " setting selection region to ";
					  current_region.print(tt_error_file());
					  tt_error_file() << " and found ";
					  underneath->print(tt_error_file());
					  tt_error_file() << endl;
				  };
#endif
				  switch (real_tool_in_hand->kind_of()) {
					  case EXPANDER:
						  reason = MIGHT_USE_EXPANDER;
						  saved_reason = EXPANDER_USED;
						  break;
					  case VACUUM:
						  reason = MIGHT_USE_VACUUM;
						  saved_reason = VACUUM_USED;
						  break;
					  case COPIER:
						  reason = MIGHT_USE_COPIER;
						  saved_reason = COPIER_USED;
						  break;
					  case TOOLBOX:
						  return(NULL);
					  default:
						  reason = MIGHT_DROP_ITEM;
	//					  saved_reason = ITEM_DROPPED;
						  break;
				  };
			  };
		  };
/*
		  } else {
			  if (using_arm || !is_wand(tool_in_hand->kind_of())) {
				  // arm or robot holding non wand
				  if (moved) {
					  tool_in_hand->active_region(current_region);
				  };
				  // tool_in_hand is now part of programmer appearance
	//			  underneath = tt_screen->first_tool_to_collide_with(&region,tool_in_hand);
			  } else if (moved) { // ok ?? -- sometimes the tool moved by itself -- e.g. wand use
				  tool_in_hand->active_region(current_region);
				  // following was confusing since tool is what is being used
				  // use robot's hand even if holding something
	//			  appearance->pointer_region(current_region);
			  };
			  if (using_arm) {
				  underneath = tt_screen->first_tool_to_collide_with(&current_region,tool_in_hand);
			  } else { // except for the robot (and what it is holding -- being above it)
				  underneath = tt_screen->first_to_collide_with(&current_region,appearance);
				  if (underneath == appearance) { // a robot in training picked up something big
					  underneath = NULL; // ignore it
				  };
			  };
			  if (underneath == NULL && moved && !is_wand(tool_in_hand->kind_of())) {
				  // try again with full region of non-wand
				  tool_in_hand->full_region(current_region);
				  underneath = tt_screen->first_tool_to_collide_with(&current_region,tool_in_hand);
			  };
			  if (underneath != NULL) {
				  switch (tool_in_hand->kind_of()) {
					  case EXPANDER:
						  reason = MIGHT_USE_EXPANDER;
						  saved_reason = EXPANDER_USED;
						  break;
					  case VACUUM:
						  reason = MIGHT_USE_VACUUM;
						  saved_reason = VACUUM_USED;
						  break;
					  case COPIER:
						  reason = MIGHT_USE_COPIER;
						  saved_reason = COPIER_USED;
						  break;
					  case TOOLBOX:
						  return(NULL);
	//				  case INTEGER:
	//					  reason = MIGHT_USE_NUMBER;
	//					  saved_reason = NUMBER_USED;
	//					  break;
	//				  case CUBBY:
	//					  reason = MIGHT_USE_CUBBY;
	//					  saved_reason = CUBBY_USED;
	//					  break;
					  default:
						  reason = MIGHT_DROP_ITEM;
	//					  saved_reason = ITEM_DROPPED;
						  break;
				  };
			  };
		  };
*/
		  break;
	  default: // what is left??
		  return(NULL);
	};
	if (underneath == NULL || 
		 underneath->to_be_smashed() || underneath->slated_for_deletion()) {
		return(NULL);
	};
	if (underneath->infinite_stack() && reason != MIGHT_USE_COPIER && reason != MIGHT_USE_VACUUM) {
		// second and third conditions added on 080400
		return(underneath);
	};
	boolean held_by_robot = (underneath->kind_of() != THOUGHT_BUBBLE &&
									 underneath->contained_in(ROBOT) &&
									 !underneath->contained_in(THOUGHT_BUBBLE));
	Sprite *leader = underneath->pointer_to_leader();
	if (leader != NULL &&
		 reason != MIGHT_USE_COPIER &&
		 (held_by_robot || leader->kind_of() == COPIER)) {
		// except for copying don't allow selection of something held
		// by robot (or by robot's wand)
		Sprite *ultimate_container = underneath->ultimate_leader(); // presumably the robot
		if (ultimate_container == appearance) {
			return(NULL); // robot somehow selected itself
		} else {
			return(ultimate_container->select(&current_region,reason,real_tool_in_hand));
		};
	};
	return(underneath->select(&current_region,reason,real_tool_in_hand));
};

boolean Programmer_At_Floor::part_of_selection(Sprite *item) {
	if (current_selection == NULL) return(FALSE);
	if (current_selection == item) return(TRUE);
	if (current_selection->priority() > item->priority() &&
		 // item is above selection and overlaps
		 current_selection->colliding(item)) {
		return(TRUE);
	};
	Sprite *sprite = item;
	while ((sprite = sprite->pointer_to_leader()) != NULL) {
		if (sprite == NULL) return(FALSE);
		if (sprite == current_selection) return(TRUE);
	};
	return(FALSE);
};

//ActionStatus action_status = -1; // new on 030702

void Programmer_At_Floor::use_tool(boolean via_mouse_buttons, city_coordinate starting_x, city_coordinate starting_y) {
	if (tool_in_hand->being_used() || tool_in_hand->infinite_stack()) return;
 #if TT_DEBUG_ON
	if (tt_debug_mode == 220803) {
		tt_error_file() << "Using tool now." << endl;
	};
#endif
//   move_count = 0; // new on 130502 - reset counter -- moved onto appropriate branches below on 110602
	Sprite *original_current_selection = current_selection;
   if (tool_in_hand->kind_of() == COPIER) {
      Copier *copier = (Copier *) tool_in_hand;
      if (hand == HOLDING_COPIER && copier->is_on()) {
			if (current_selection == NULL && copier->need_subject_to_work()) {
				play_sound(PLOP_SOUND); // new on 081004
				copier->turn_off(appearance); // new on 081004 so if you miss (and there is no subject) the wand still works fine next time
				return;
			};
         if (copier->return_counter() == 0) {
            play_sound(SHUTDOWN_SOUND);
            copier->turn_off(appearance);
            return;
         };
      };
	  	Sprite *attached = copier->attached_tool();
	   if (using_arm) {
			appearance->set_parameter(HAND_USING_WAND);
			if (attached == NULL && saved_selection == NULL) {
				// to move wand down on target move up first
				if (appearance->animation_in_progress()) { // new on 160904
					appearance->finish_instantly(); // before moving as follows be sure old animations are finished
					appearance->move_by(0,tile_height/-4); // since moved up 2 tile_heights below and then corrected by 7/4 
				};
				appearance->move_by(0,tile_height*2);
         };
      };
		if (tool_in_hand == NULL) {
			// new on 141204 for robustness - can happen if "overclicking" the wand in puzzle 4
			return;
		};
//      if (hand == HOLDING_COPIER && attached == NULL) { 
		// commented out on 160904 to stop drift depending upon timing of repeated pressing of space bar
//			// delay use until wand is down 
//         if (saved_selection == NULL) {
//				// haven't scheduled wand use yet
//            saved_selection = current_selection;
//            saved_modified = (flag) button_modified();
//            hand = USING_COPIER;
//            // time it takes to move wand down
//            appearance->do_after(320,NULL,USE_TOOL_CALLBACK);
//         }; // otherwise ignore -- will get to it
//         return;
//      };
   };
	boolean modified = button_modified();
	if (saved_selection != NULL) {
		current_selection = saved_selection;
		modified = saved_modified; // state when wand was first used counts
		saved_selection = NULL;
	};
	if (current_selection != NULL && saved_reason != NO_SELECTION_REASON_GIVEN) {
		if (saved_reason == VACUUM_USED) {
			// new on 220499
			saved_reason = ((Vacuum *) tool_in_hand)->selection_reason();
//         if (modified) {
//			   saved_reason = VACUUM_USED_TO_BLANK;
//         } else if (((Vacuum *) tool_in_hand)->ready_to_spit()) {
//            saved_reason = VACUUM_USED_TO_SPIT;
//         };
		};
		// NULL ok for say vacuum spitting out
		if (current_selection->pointer_to_leader() != NULL && current_selection->pointer_to_leader()->kind_of() == PROGRAM_PAD) {
			original_current_selection = current_selection->pointer_to_leader(); // new on 110205 so recording knows more about what is going on
		};
		current_selection = current_selection->selected(saved_reason,appearance);
		if (current_selection == NULL) return; // refused
	};
	user_did(GLOBAL_HISTORY,USER_HAS_APPLIED,appearance);
	if (floor != tool_in_hand->pointer_to_background()) { // condition new on 211200
		tool_in_hand->remove_from_floor(); // new on 211200
		tool_in_hand->set_background(floor); // so tool knows for recording etc.
	};
   // for now
   boolean mouse_unchanged;
   if (via_mouse_buttons) {
      mouse_unchanged = saved_button_unchanged;
   } else {
      mouse_unchanged = TRUE; // for now
//      if (appearance->kind_of() == ROBOT_IN_TRAINING) {
         // don't count the keyboard action and the use as 2 events 
//         ((Robot *) appearance)->increment_training_counter();
//      };
   };
//   action_status = -1; // new on 110602 to see if the following completed so update move_count
	// rewritten on 020702 since this is held on to by Dusty and maybe more - so was made global on 030702
	new_tool_in_hand(tool_in_hand->used(current_selection,appearance,
                                       mouse_unchanged,
 						                     TRUE,0,modified,original_current_selection));
   //if (is_completed(&action_status)) { // is this OK - what if Dusty hasn't finished yet?
   //   move_count = 1; // moved here on 110602 and set to 1 rather than 0 and made conditional
   //};
	if (hand == HOLDING_COPIER || hand == USING_COPIER) {
		if (((Copier *) tool_in_hand)->attached_tool() == NULL) {
			// copy refused (or missed)
			appearance->set_parameter(HAND_RESTORE_WAND);
			play_sound(PLOP_SOUND);
         hand = HOLDING_COPIER;
			if (starting_x != max_long && tt_log_version_number >= 54) { 
				// new on 081004 to stop drift
				appearance->move_to(starting_x,starting_y);
			};
		} else {
			hand = USED_COPIER;
		};
	};
   if (absolute_mouse_mode()) {
      tt_hand_needs_to_move_for_tool_use = TRUE; // new on 190402 so tool moves (though one frame later)
   };
};

void Programmer_At_Floor::completed() { // new on 181102 -- forwarded by appearance
   // some tool has completed so reset move count (for absolute mouse mode)
   move_count = 1;
};

void Programmer_At_Floor::new_tool_in_hand(Sprite *replacement) {
   if (replacement == NULL) {
		// if a bomb ignore what is underneath (e.g. stack of bombs)
		release_object((tool_in_hand->kind_of() == BOMB),TRUE);
	} else if (replacement != tool_in_hand) {
		if (appearance->kind_of() == ROBOT_IN_TRAINING ||
			 appearance->kind_of() == ROBOT) {
			((Robot *) appearance)->swap_tool(replacement);
		};
//		if (tool_in_hand != NULL) tool_in_hand->set_held_status(NOT_HELD);
//		tool_in_hand = replacement;
		set_tool_in_hand(replacement); // rewritten 220901
//      tool_in_hand->set_held_status(HELD_BY_PROGRAMMER);
	};
};

void Programmer_At_Floor::adjust_tool_in_hand() {
	 // might not be in the hand anymore
	TTRegion tool_region, grasp_region;
	tool_in_hand->collision_region(tool_region);
	appearance->pointer_region(grasp_region);
	city_coordinate adjustment_x = 0,
						 adjustment_y = 0;
	if (grasp_region.min_x > tool_region.max_x) {
		adjustment_x = grasp_region.max_x-tool_region.max_x;
	} else if (tool_region.min_x > grasp_region.max_x) {
		adjustment_x = grasp_region.max_x-tool_region.max_x;
	};
	if (grasp_region.min_y > tool_region.max_y) {
		adjustment_y = grasp_region.max_y-tool_region.max_y;
	} else if (tool_region.min_y > grasp_region.max_y) {
		adjustment_y = grasp_region.max_y-tool_region.max_y;
	};
	tool_in_hand->move_by(adjustment_x,adjustment_y);
};

//void Programmer_At_Floor::compute_grasp_region(TTRegion &region) {
//	if (using_arm) {
//		appearance->collision_region(region);
//	} else {
//		appearance->pointer_region(region);
//	};
//};

boolean Programmer_At_Floor::pick_up(Sprite *item, SelectionReason reason) {
#if TT_DEBUG_ON
   if (item->debug_counter == tt_debug_target) {
      log("Programmmer picking up debug target.");
   };
#endif
	if (hand != EMPTY_HAND) {
		// typically false if since accelerator key push something picked up
		if (item != NULL) {
			item->morph_to_inert(); // new on 300604
		};
		return(item != tool_in_hand);
	};
	if (waiting_for != NULL && waiting_for != item) {
		waiting_for->stop_running();
		waiting_for = NULL;
//		stop_postponing_new_logs();
	};
	if (item != NULL) {
//		if (item->graspable()) {
			item = item->selected(reason,appearance);
//		} else {
//			item = NULL;
//		};
	};
	if (item == NULL) return(FALSE); // nothing there
//   tool_in_hand_used = FALSE;
   item->reset_used_since_pick_up();
	user_did(GLOBAL_HISTORY,USER_HAS_PICKED_UP,appearance);
	// may still be animating
// item->remove_from_floor();
   // still an item of the floor to animate etc
	initialize_tool_in_hand(item);
	hand = ABOUT_TO_HOLD_ITEM;
	tt_programmer->set_grasp_delta_x(0);
	tt_programmer->set_grasp_delta_y(0);
	if (tt_log_version_number >= 46 && !tool_in_hand->is_tool()) {
		// new on 120604 that if you pick up something bigger than the screen you grab it "in the middle" rather than the side
		boolean too_wide = (tool_in_hand->current_width() > ideal_screen_width);
		boolean too_high = (tool_in_hand->current_height() > ideal_screen_height);
		if (too_wide || too_high) {
			city_coordinate ideal_tool_llx, ideal_tool_lly, tool_in_hand_llx, tool_in_hand_lly, adjustment_x, adjustment_y;
			appearance->lower_left_corner(ideal_tool_llx,ideal_tool_lly);
			tool_in_hand->adjust_grasp(adjustment_x,adjustment_y);
			city_coordinate tool_width = tool_in_hand->true_width();
			city_coordinate tool_height = tool_in_hand->true_height();
			ideal_tool_llx -= tool_width-appearance->true_width()/2;
			ideal_tool_llx +=	adjustment_x;
			ideal_tool_lly -= tool_height/2-appearance->true_height()/2;
			ideal_tool_lly	+=	adjustment_y;
			tool_in_hand->lower_left_corner(tool_in_hand_llx,tool_in_hand_lly);
			if (too_wide) {
				tt_programmer->set_grasp_delta_x(tool_in_hand_llx-ideal_tool_llx);
				tt_programmer->set_width_at_time_of_grasp(tool_width);
			};
			if (too_high) {
				tt_programmer->set_grasp_delta_y(tool_in_hand_lly-ideal_tool_lly);
				tt_programmer->set_height_at_time_of_grasp(tool_height);
			};
		};
	};
//	item->set_to_good_size(); // new on 050203 needed if removed from back of picture - otherwise might be redundant -- 
	// commented out on 120203 since causes things to be good size too often
	return(TRUE);
};

void Programmer_At_Floor::initialize_tool_in_hand(Sprite *item) {
	tt_screen->remove(item); // since will be displayed by arm or robot
//	if (using_arm) item->recursively_increment_screen_ref_count();
	if (tool_in_hand != NULL) tool_in_hand->set_held_status(NOT_HELD);
//	tool_in_hand = item;
	set_tool_in_hand(item); // re-written on 220901
//   tool_in_hand->set_background(floor);
//	tool_in_hand->set_held_status(HELD_BY_PROGRAMMER);
// following does no good since not on screen anymore
// except if using wand
	tool_in_hand->set_priority(appearance->priority()+1); //was +2
   tool_in_hand->add_outside_followers_to_screen();  // but outside followers won't be displayed by default
};

void Programmer_At_Floor::release_on_floor(Sprite *released) {
   released->now_on_floor(floor,appearance);
	if (!using_arm || tt_system_mode == PUZZLE) { // i.e. no scrolling  && tt_log_version_number > 12
		// make sure nothing ends up off screen
		city_coordinate llx,lly;
		released->lower_left_corner(llx,lly);
		city_coordinate sprite_width = released->current_width();
		city_coordinate sprite_height = released->current_height();
		if (llx >= ideal_screen_width) {
			llx = ideal_screen_width-tile_width;
		} else if (llx+sprite_width <= 0) {
			llx = tile_width-sprite_width;
		};
		if (lly >= ideal_screen_height) {
         lly = ideal_screen_height-tile_height;
		} else if (lly+sprite_height <= 0) {
			lly = tile_height-sprite_height;
		};
		released->animate_location(llx,lly,500); // no-op if no change
	};
};

ProgrammerStatus Programmer_At_Floor::release_object(boolean ignore_underneath,
																	  boolean hand_release,
                                                     boolean treat_copier_specially) {
	ProgrammerStatus next_status = PROGRAMMER_NORMAL;
	if (tool_in_hand == NULL) return(next_status);
	boolean original_ignore_underneath_on_next_drop = ignore_underneath_on_next_drop;
	if (ignore_underneath_on_next_drop) {
		ignore_underneath = TRUE;
		ignore_underneath_on_next_drop = FALSE; // used up here
		play_sound(BOOK_DROP_SOUND);
	};
	boolean holding_copier = (treat_copier_specially && (tool_in_hand->kind_of() == COPIER));
	if (hand_release && !holding_copier) {
		hand_release_cycle();
	};
#if TT_DIRECT_INPUT
   set_background_force_feedback_effect(NULL); // no longer feeling the weight
   play_force_feedback_effect("Drop",FALSE);
#endif
	SpecialDrop drop;
	if (original_ignore_underneath_on_next_drop) {
		drop = IGNORE_UNDERNEATH_DROP;
	} else if (give_toss_on_next_drop) {
		drop = GIVE_TOSS_DROP;
	} else {
		drop = NOT_SPECIAL_DROP;
	};
	Sprite *released = tool_in_hand->released(appearance,TRUE,drop);
//   if (!ignore_underneath) { // a normal release
//      tool_in_hand->set_favorite_size_and_location();
//   } else if (toolbox_tool(tool_in_hand)) {
      // "forced" release and dropping one of the named tools
//      tool_in_hand->locomote_to_favorite_location_and_size();
//   };
	if (released == NULL) return(next_status);
	if (released->kind_of() == PICTURE && ((Picture *) released)->flipped_followers() != NULL) {
		// condition new on 250203 since don't want Dusty and the like to do this since messes up their animation
		released->finish_instantly(); // new on 230203 so that if you takes things out of a notebook or the like 
		// and drop before animating to good size all works fine 
		// (Leonel had an example where this went wrong.)
	};
	user_did(GLOBAL_HISTORY,USER_HAS_DROPPED,appearance);
	if (released == tool_in_hand) { // really released it
      tt_programmer->release_tool_in_hand(); // so it is kept in synch
		if (holding_copier && using_arm) {
			release_copier();
		} else {
			tt_screen->add(tool_in_hand,FALSE); // removed when picked up
//			tool_in_hand->set_held_status(NOT_HELD);
			tool_in_hand->set_priority(programmer_hand_priority+1); // so its not obscured until it hits the floor
//			tool_in_hand = NULL;
//			hand = EMPTY_HAND;
			set_tool_in_hand(NULL);
			if (!using_arm) {
				((Robot *) appearance)->drop_tool();		
			};
         // new on 190802 since if holding very large object hand can be way off screen - and needs to come back when object is dropped
         // fixes a bug where can grab last thing wand pointed to
         city_coordinate llx = appearance->current_llx();
         city_coordinate lly = appearance->current_lly();
         if (llx > tt_screen->max_x()) {
            llx = tt_screen->max_x();
         } else if (llx+appearance->current_width() < tt_screen->min_x()) {
            llx = tt_screen->min_x()-appearance->current_width();
         };
         if (lly > tt_screen->max_y()) {
            lly = tt_screen->max_y();
         } else if (lly+appearance->current_width() < tt_screen->min_y()) {
            lly = tt_screen->min_y()-appearance->current_width();
         };
         appearance->move_to(llx,lly); // should be no-op if was on screen to begin with
         if (using_arm) {
            appearance->collision_region(current_region);
         } else {
            appearance->pointer_region(current_region);
         };
		};
	} else if (using_arm && holding_copier) {
		// dropped something attached to copier
		// was hand == USED_COPIER but can be confused by 2 quick clicks
		hand = HOLDING_COPIER;
		appearance->set_parameter(HAND_RESTORE_WAND);
		city_coordinate drift_correction;
		if (tt_log_version_number >= 52) { // new on 130904 -- commented out 160904 since only fixed problem is space typed quickly
//			drift_correction += (6*tile_height)/5; // drifted down a little bit too much
			drift_correction = (-7*tile_height)/4; // more accurate and now quickly repeated spaces work better due to finish_instantly() - 160904
		} else {
			drift_correction = (-3*tile_height)/2;
		};
#if TT_DEBUG_ON
		if (tt_debug_mode == 160904) {
			tt_error_file() << "LLY while restoring hand after using wand is " << appearance->current_lly() << endl;
//			tt_debug_target = appearance->debug_counter;
		};
#endif
      appearance->animate_delta_location(0,drift_correction,500); // avoid tendency to drift upwards
      if (absolute_mouse_mode()) { 
         // new on 190402 so is dropped the right place
         city_coordinate x,y;
//         absolute_mouse_position(x,y); // should have been recorded for logging
         last_mouse_deltas(x,y); // updated on 220402
         released->move_to(x-released->current_width()/2,y-released->current_height()/2);
      };
//      arm->move_by(2*tile_width,0);
//      tt_screen->add(released);
//			appearance->do_after(appearance->total_cycle_duration(),
//										this,&hand_holding_copier_callback); 
	} else {
		hand = HOLDING_ITEM;
	};
	if (ignore_underneath) { // was also is_wand(released->kind_of()) || 
		Room *room = floor->pointer_to_room();
		if (room != NULL && room->when_will_house_explode() == 0) {
			record_event(RELEASE_ITEM,appearance,floor,released);
		};
		release_on_floor(released);
	} else {
      long min_priority;
      if (tool_in_hand != NULL) {
         min_priority = max(tool_in_hand->priority(),appearance->priority())+1;
      } else {
         min_priority = appearance->priority()+1;
      };
		Sprite *underneath;
		if (tt_log_version_number > 36) {
			underneath = drop_recipient(released,min_priority);
		} else {
			underneath = tt_screen->first_to_collide_with(released,min_priority,can_grasp); //released->priority());
		};
		if (underneath != NULL && //underneath->selectable() &&
			 !underneath->has_leader(released) &&
			 !released->has_leader(underneath)) {
			// could have sounds depend upon what it was dropped on
//			play_sound(BOOK_DROP_SOUND,1);
//			if (!floor->has_item(released)) 
			floor->add_item(released,FALSE,FALSE,FALSE);
			next_status = release_on_top(underneath,released);
		} else {
			play_sound(BOOK_DROP_SOUND); // fell on the floor
			Room *room = floor->pointer_to_room();
			if (room != NULL && room->when_will_house_explode() == 0) {
				// the room might be NULL
				record_event(RELEASE_ITEM,appearance,floor,released);
			};
			release_on_floor(released);
		};
	};
   // commented out the following on 190802 since did nothing
//	if ((!using_arm || !holding_copier) && !released->slated_for_deletion()) {
////		if (using_arm) released->recursively_decrement_screen_ref_count();
//		if (released->pointer_to_leader() == NULL ||
//			 !released->completely_inside_leader()) {
////			if (!screen->add(released)) { // already there
////				released->increment_screen_ref_count(); // restore ref count
////			};
//		};
//	};
	return(next_status);
};

boolean not_below_minimum_priority(Sprite *sprite, void *minimum_priority_void) {
	long *minimum_priority = (long *) minimum_priority_void;
	return(sprite->priority() >= *minimum_priority);
};

Sprite *Programmer_At_Floor::drop_recipient(Sprite *released, int min_priority) { 
	// abstracted on 290204 - so the drop feedback matches the drop action exactly
	Sprite *underneath = NULL;
	TTRegion current_region_of_released;
	released->active_region(current_region_of_released);
	underneath = tt_screen->first_tool_to_collide_with(&current_region_of_released,released,not_below_minimum_priority,&min_priority); 
	// predicate args added 041004
	// new on 260204 so drop selection feedback matches drop response
	if (underneath != NULL) { // copied here on 290204 so reject those underneath that won't accept this - e.g. tools
		Sprite *still_underneath = underneath->select(&current_region_of_released,MIGHT_DROP_ITEM,released);
		if (still_underneath != NULL && still_underneath != released) { 
			// still_underneath != released new condition on 011004 since can happen if robot has an attached (initial tool)
			underneath = still_underneath;
		} else { // rejected
			if (tt_log_version_number < 43) {
				underneath = tt_screen->next_to_collide_with(released,underneath,can_receive);
			} else { // new on 210404 since the above used priority depth
				underneath = tt_screen->next_tool_to_collide_with(&current_region_of_released,released,underneath);
			};
		};
	};
	if (underneath == NULL) {
		// the following used can_grasp (indirectly) rather than can_receive prior to 290204
		underneath = tt_screen->first_to_collide_with(released,min_priority,can_receive); // new on 270204 - good policy?
	};
	if (underneath != NULL && underneath->infinite_stack()) { // new on 150704 for returning objects to stacks
		return(underneath); // equal test to come
	};
	// following copied here from :release_object on 290204 - earlier this didn't apply to drop feedback
	if (underneath != NULL && (underneath->contained_in(TRUCK) || underneath->contained_in(MARTIAN))) {
		// don't drop into stuff in truck but in truck (or martian)
		// assumes truck itself isn't in something
		underneath = underneath->ultimate_leader();
	};
	if (underneath != NULL) { // copied here on 290204 so reject those underneath that won't accept this - e.g. tools
		Sprite *still_underneath = underneath->select(&current_region_of_released,MIGHT_DROP_ITEM,released);
		if (still_underneath != NULL) { 
			underneath = still_underneath;
		} else { // rejected
			underneath = tt_screen->next_to_collide_with(released,underneath,can_receive);
			if (underneath != NULL) {
				underneath = underneath->select(&current_region_of_released,MIGHT_DROP_ITEM,released);
			};
		};
	};
	return(underneath);
};

void Programmer_At_Floor::release_copier() {
	if (tt_finish_all_animations || hand == ABOUT_TO_RELEASE_COPIER) {
		restore_copier();
	} else {
		hand = ABOUT_TO_RELEASE_COPIER;
		appearance->set_parameter(HAND_RELEASE_WAND);
//			appearance->reset_cycle();
//		postpone_new_logs();
		appearance->do_after(appearance->total_cycle_duration(),NULL,RESTORE_COPIER_CALLBACK);
   };
};

void Programmer_At_Floor::restore_copier() {
   if (about_to_quit()) return; // prior to 030504 was tt_shutting_down and that can be FALSE when quitting
//	stop_postponing_new_logs();
	appearance->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
	if (tool_in_hand == NULL) {// seem to call this twice sometimes
//		say("ToonTalk confused and tried to release the wand but it wasn't held.");
		return;
	};
	tool_in_hand->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
//	tt_screen->add(tool_in_hand);
	city_coordinate current_x,current_y;
	appearance->lower_left_corner(current_x,current_y);
	tool_in_hand->move_to(current_x,current_y+appearance->true_height()/2);
	current_x += appearance->true_width();
	tool_in_hand->set_held_status(NOT_HELD);
	tool_in_hand->set_priority(floor->next_item_priority());
   tt_screen->add(tool_in_hand);
   release_object(FALSE,FALSE,FALSE);
//	tool_in_hand = NULL;
	hand = EMPTY_HAND;
	// to restore size since hand holding wand is smaller
//	appearance->set_size((4*appearance->current_width())/5,
//								(4*appearance->current_height())/5);
//	arm->set_size((5*arm->current_width())/4,
//					  (5*arm->current_height())/4);
	selection_feedback_flag = TRUE; // in case timer is attached to appearance
// following broke both because it looped and stuff above isn't idempotent
//	appearance->finish_instantly();
	tt_screen->remove(appearance);
	appearance = hand_appearance;
	tt_screen->add(appearance);
	appearance->set_parameter(HAND_OPEN);
	appearance->move_to(current_x-appearance->true_width(),
							  current_y-2*tile_height);
//	play_sound(BOOK_DROP_SOUND);
};

ProgrammerStatus Programmer_At_Floor::release_on_top(Sprite *underneath, Sprite *released, boolean ok_to_try_again) {
	if (underneath->to_be_smashed()) {  // mouse is coming
		Sprite *survivor = underneath->pointer_to_leader();
		if (survivor != NULL) {
			return(release_on_top(survivor,released,ok_to_try_again));
		};
	};
	ProgrammerStatus next_status = PROGRAMMER_NORMAL;
	ReleaseStatus status;
	if (underneath->infinite_stack() && (equal(underneath,released) ||
                                        (underneath->fine_kind_of() == SCALE_PICTURE && released->fine_kind_of() == SCALE_PICTURE))) {
		status = RELEASED_ON_ITS_STACK;
	} else {
		status = underneath->item_released_on_top(released,appearance,underneath,0);
//#if TT_DEBUG_ON
//		perform_integrity_checks(underneath);
//#endif
	};
	// the following may reset underneath to true recipient
	switch (status) {
//		case RELEASED_THOUGHT_BUBBLE_ON_ROBOT:
			// swapped from usual case
//			underneath = released;
			// and continue as before
		case RELEASED_CUBBY_ON_UNTRAINED_ROBOT: {
//		case RELEASED_ROBOT_ON_THOUGHT_BUBBLE:
			next_status = ABOUT_TO_ENTER_THOUGHT_BUBBLE;
         cubby_that_started_training = (Cubby *) released;
//			switch (underneath->kind_of()) {
//				case ROBOT:
					thought_bubble_robot = (Robot *) underneath;
					thought_bubble_robot->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
//					break;
//				case THOUGHT_BUBBLE: // passed it along to the robot
//					thought_bubble_robot 
//						 = ((Thought_Bubble *) underneath)->pointer_to_robot();
//					break;
//				default:
//					tt_error_file() << "Warning: expected " << type_name(underneath)
//									<< " to be either a robot or thought bubble.  " << endl;
//					return(next_status);
//			};
			original_thought_bubble = thought_bubble_robot->pointer_to_thought_bubble();
			thought_bubble_robot->set_thought_bubble(NULL); // for now
//			if (original_thought_bubble == NULL) {
//				original_thought_bubble = (Thought_Bubble *) underneath;
//				floor->remove_item(original_thought_bubble);			
//			};
			tt_copying_entire_thought_bubble = TRUE;
//			tt_copying_for_training = TRUE; // new on 080205 commented out on 110405 since the copy should work fine since 
			// tt_copying_entire_thought_bubble is TRUE and it should make things become "ordinary" -- e.g. textual sensor becomes ordinary text
			thought_bubble_entered = (Thought_Bubble *) (original_thought_bubble->copy()); // restored on 010305
//			thought_bubble_entered = new Thought_Bubble(); // rewritten on 080205
			thought_bubble_entered->set_size_and_location_to_those_of(original_thought_bubble);
			Cubby *copied_cubby = original_thought_bubble->pointer_to_cubby(); // removed on 010305 ->top_level_copy()); 
			// added top_level_copy above on 240205 since otherwise training can change the real world via remote looks
			// doesn't really help since other changes of 240205 really deal with this -- undid this change on 010305
			original_thought_bubble->set_cubby(NULL); // will get a different one below -- new on 080205
			thought_bubble_entered->set_cubby(copied_cubby); 
   		thought_bubble_entered->now_inside_thought_bubble(TRUE);
			floor->add_item(thought_bubble_entered,FALSE);
			thought_bubble_added_to_floor = TRUE;
         if (tt_toolbox_notebook != NULL && (tt_system_mode != PUZZLE || is_toolbox_part_of_puzzle())) {
				// tt_toolbox_notebook != NULL new on 150704
         	tt_toolbox_notebook->go_to_a_blank_page();
         };
//			tt_copying_for_training = FALSE; // commented out on 110405
			original_thought_bubble->set_cubby((Cubby *) cubby_that_started_training->top_level_copy()); // new on 080205
			original_thought_bubble->now_inside_thought_bubble(FALSE);
			tt_copying_entire_thought_bubble = FALSE;	
//			thought_bubble_robot = (Robot *) released;
			if (tt_system_mode != PUZZLE) {
            tt_martian->remove_text();
			   turn_off_selection_feedback_for(4000);
         };
			break;
															 };
//		case RELEASED_ROBOT_ON_CUBBY:
//			((Robot *) released)->try_clause((Cubby *) underneath,screen);
//			break;
		case RELEASED_CUBBY_ON_ROBOT:
//			((Robot *) underneath)->try_clause((Cubby *) released,screen);
			break;
//		case RELEASED_IN_CUBBY_HOLE:
//			record_event(RELEASE_ITEM_ON,appearance,floor,released);????
//         released->now_released_on(appearance);
//			break;
		case RELEASED_CUBBY_ON_THOUGHT_BUBBLE:	
//			turn_off_selection_feedback_for(accept_item_duration);
			break;
//		case RELEASED_BODY_CUBBY_LEFT_OF_CUBBY:
//			record_event(RELEASE_ITEM_LEFT_OF,appearance,floor,released);			
//			break;
//		case RELEASED_BODY_CUBBY_RIGHT_OF_CUBBY:
//			record_event(RELEASE_ITEM_RIGHT_OF,appearance,floor,released);			
//			break;
		case RELEASED_IN_TRUCK:
			record_event(DROP_ITEM_IN_TRUCK,appearance,floor,underneath);
			//			turn_off_selection_feedback_for(2000);
			break;
		case RELEASE_ACCEPTED:
			record_event(RELEASE_ITEM,appearance,floor,released);
			break;
		case RELEASE_IGNORED: {
			 // too often I drop things and they are ignored because
			 // another object happens to be "higher" 
			 Sprite *new_underneath = tt_screen->next_to_collide_with(released,underneath,can_receive);
			  // robot carries things on chest so it is underneath too
			 if (new_underneath != NULL && new_underneath->part_of(underneath) && new_underneath->kind_of() != ROBOT) { 
				 // e.g. thought bubble and robot
				 // on 090804 added not robot check since can get infinite recursion with a team if something inappropriate is dropped on them 
				 // (e.g. a picture)
				 new_underneath = tt_screen->next_to_collide_with(released,new_underneath,can_receive);
			 };
			 if (new_underneath != NULL && released != tt_toolbox && !new_underneath->part_of(underneath)) {
				 // second condition added 130200 since dropping Tooly on stuff sometimes caused infinite recursion
				 // third condition added on 050805 to stop infinite recursion as well
				 if (new_underneath->infinite_stack() && ok_to_try_again) { // new on 100400 so can reach underneath
					 return(release_on_top(new_underneath,released,FALSE)); // don't risk infinite recursion here
				 };
				 TTRegion underneath_region, new_underneath_region;
				 underneath->collision_region(underneath_region);
				 new_underneath->collision_region(new_underneath_region);
				 if (!underneath_region.mostly_contains(&new_underneath_region) ||
					  new_underneath->kind_of() == TRUCK) {
					 // don't count things underneath that are mostly covered	 
					 // except it is ok if it is a truck with a cubby on covering it
					 next_status = release_on_top(new_underneath,released,ok_to_try_again);
				 } else {
					 record_event(RELEASE_ITEM,appearance,floor,released);			
					 release_on_floor(released);
//					 turn_off_selection_feedback_for(1500);
				 };
			 } else { 
				 record_event(RELEASE_ITEM,appearance,floor,released);
				 release_on_floor(released);
//				 turn_off_selection_feedback_for(1500);
			 };
			}; 
			break;
		case RELEASED_ON_ITS_STACK:
//			if (appearance->kind_of() == ROBOT_IN_TRAINING) {
//				floor->remove_last_n_events(2);
//				((Robot *) appearance)->remove_last_n_working_set_items(1);
//			};
			{
			boolean training = (appearance->kind_of() == ROBOT_IN_TRAINING);
         if (training) {
            underneath->set_available_for_indexing(FALSE); // so a path to it is used
         };
			if (underneath->contained_in(PROGRAM_PAD)) { // obsolete??
				record_event(RELEASE_ITEM_ON,appearance,floor,underneath->pointer_to_leader(),TRUE,1);
			} else {
				// use "key" as flag for whether this is just returning to a stack
				record_event(RELEASE_ITEM_ON,appearance,floor,underneath,TRUE,1);
			};
			if (training) {
            underneath->set_available_for_indexing(TRUE); 
         };
         underneath->now_released_on(underneath,appearance);
			last_released = released; 
			released->set_slated_for_deletion(TRUE);
			released->animate_size_and_location(underneath->current_width(),
															underneath->current_height(),
															underneath->current_llx(),
															underneath->current_lly(),
															750,
															NULL,DELETE_RELEASED_CALLBACK);
//			floor->remove_item(released);
//			destroy(released,appearance,FALSE);
			};
			break;
		case RELEASED_ON_NOTEBOOK:
			if (released->kind_of() == PICTURE && 
				 released->remote_status() == NOT_REMOTE) { // why?
				// in case something is running inside
				Picture *picture = (Picture *) released;
				picture->stop_all();
//				picture->unflip();
//				picture->unsave_size();
//				picture->recursively_save_size(); // since pictures dump their size
			};
			// and the following
		case RELEASED_ON_NUMBER:    // took care of these already
		case RELEASED_ON_TEXT:
		case RELEASED_ON_PICTURE:
		case RELEASED_ITEM_TO_BIRD:
		case RELEASED_ITEM_ON_NEST:
		case RELEASED_ITEM_LEFT_OF_ITEM: 
		case RELEASED_ITEM_RIGHT_OF_ITEM: 
      case RELEASE_HANDLED:
			// don't want selection feedback to screw up the animation
			// but don't know how long
//			turn_off_selection_feedback_for(2000);
			break; // do nothing
	};
	return(next_status);
};

void Programmer_At_Floor::delete_released() {
	floor->remove_item(last_released);
	last_released->destroy(appearance,FALSE);
	last_released = NULL; // new on 140103 - important for XML dumping
};

void Programmer_At_Floor::turn_off_selection_feedback_for(millisecond duration) {
	if (duration > 0) {
		selection_feedback_flag = FALSE;
//		appearance->true_after(duration,&selection_feedback_flag);
      appearance->do_after(duration,appearance,SET_SELECTION_FEEDBACK_CALLBACK);
      tt_selection = NULL;  
		current_selection = NULL;
	};
};

millisecond expansion_duration = 2000;

millisecond Programmer_At_Floor::expand_thought_bubble() {
	release_items_held(); // just in case
	// first the robot shrinks and
	// moves into area below and left of the center of the thought bubble
	// as does the toolbox (but not quite the same so they dont overlap much)
	// when they are done then
	// the thought bubble expands to fill the screen
	// order them as follows:
	tt_screen->remove(original_thought_bubble);
	Cubby *cubby = thought_bubble_entered->pointer_to_cubby();
	thought_bubble_entered->set_priority(floor->next_tool_priority());
	cubby->set_priority(floor->next_tool_priority());
	thought_bubble_robot->set_priority(floor->next_tool_priority());
//	thought_bubble_entered->save_size();
	// store away the thought bubble location first
	thought_bubble_entered->lower_left_corner(thought_bubble_llx,thought_bubble_lly);
//	tt_toolbox->save_size();
	city_coordinate bubble_center_x, bubble_center_y,
						 bubble_region_width, bubble_region_height;
	TTRegion bubble_region; 
	thought_bubble_entered->collision_region(bubble_region);
	bubble_region.region_center(bubble_center_x,bubble_center_y);
	bubble_region.region_size(bubble_region_width,bubble_region_height);
	int percent_scale = (bubble_region_width*100)/ideal_screen_width;
	millisecond time_to_move = 500;   // default
   if (tt_toolbox != NULL && !tt_toolbox->gone()) { // second condition added 030300
     // moves to 3/4 to side
		time_to_move = tt_toolbox->go_to(bubble_center_x,//+bubble_region_width/4,
								               bubble_center_y-bubble_region_height/2,
								               percent_scale);
   };
//	thought_bubble_robot->save_size();
	thought_bubble_robot->morph_to_life();
	thought_bubble_robot->animate_size_and_location((percent_scale*thought_bubble_robot->current_width())/100,
																	(percent_scale*thought_bubble_robot->current_height())/100,
																	 bubble_center_x-bubble_region_width/4,
																	 bubble_center_y-bubble_region_height/2,
																	 time_to_move);
	thought_bubble_entered->remove_follower(cubby); // restored when returning from body defining
//	cubby->recursively_set_background(floor);
	floor->add_item(cubby,FALSE);
//	cubby->save_size();
   city_coordinate new_cubby_width, new_cubby_height;
   city_coordinate left_margin;
//   if (tt_running_old_demos) {
//	   if (cubby->current_number_of_holes() == 1) {
//		   new_cubby_width = bubble_region_width/3;
//      } else {
//		   new_cubby_width = (2*bubble_region_width)/3;
//	   };
//      new_cubby_height
//		   = (cubby->current_height()*new_cubby_width)/cubby->current_width();
//   } else {  // new version is that the size is doubled
       new_cubby_width = (2*cubby->current_width()*bubble_region_width)/ideal_screen_width;
       left_margin = bubble_region_width/20;
//       if (new_cubby_width < tile_width) new_cubby_width = tile_width;
       new_cubby_height = (2*cubby->current_height()*bubble_region_height)/ideal_screen_height;
//   };
   cubby->animate_size_and_location(new_cubby_width,new_cubby_height,
												bubble_region.min_x+left_margin,
                                    bubble_center_y,//+bubble_region_height/2,
                                    time_to_move);
	cubby->do_after(time_to_move+250, // 1/4 second later
						 NULL,EXPAND_THOUGHT_BUBBLE2_CALLBACK);
	return(time_to_move+expansion_duration);
};

void Programmer_At_Floor::expand_thought_bubble2() {
	// they should all be inside the thought bubble so start expanding it
	// in case real slow
	thought_bubble_robot->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
	floor->remove_item(thought_bubble_robot,FALSE);
	thought_bubble_entered->add_follower(thought_bubble_robot);
   if (tt_system_mode != PUZZLE || is_toolbox_part_of_puzzle()) {
      floor->remove_item(tt_toolbox);
	   thought_bubble_entered->add_follower(tt_toolbox);
   };
//	floor->remove_item(tt_toolbox);
	Cubby *cubby = thought_bubble_entered->pointer_to_cubby();
	if (cubby != NULL) {
		cubby->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
//		floor->remove_item(cubby);
		cubby->remove_from_floor(FALSE);
		if (cubby->pointer_to_leader() == NULL) { 
			// condition new on 191204 since can happen with time travel and otherwise get spurious warning
			// could get a valid warning if the current leader is not thought_bubble_entered
			thought_bubble_entered->add_follower(cubby);
		};
	};
	TTRegion bubble_region;
	thought_bubble_entered->collision_region(bubble_region);
	int x_percent_growth = (ideal_screen_width*100L)/bubble_region.width();
   int y_percent_growth = (ideal_screen_height*100L)/bubble_region.height();
//	if (tt_log_version_number > 14) { // so proportionality is kept
		int max_growth = max(x_percent_growth,y_percent_growth);
		x_percent_growth = max_growth;
		y_percent_growth = max_growth;
//	};
	city_coordinate new_width = (thought_bubble_entered->current_width()*x_percent_growth)/100;
	city_coordinate new_height = (thought_bubble_entered->current_height()*y_percent_growth)/100;
	city_coordinate bubble_llx, bubble_lly;
	thought_bubble_entered->lower_left_corner(bubble_llx,bubble_lly);
	thought_bubble_entered->animate_size_and_location(new_width,new_height,
																	  ((bubble_llx-bubble_region.min_x)*x_percent_growth)/100,
																	  ((bubble_lly-bubble_region.min_y)*y_percent_growth)/100,
																	  expansion_duration);
/*
	tt_toolbox->animate_to_saved_size(expansion_duration);
	city_coordinate default_robot_height = 4*tile_height;
	thought_bubble_robot->animate_size_and_location(
									compute_sprite_width(0,default_robot_height,
																ROBOT_SPRITE),
									default_robot_height,
									tile_width*12,tile_height*6,
									expansion_duration);
	if (expansion_duration > time_to_move) {
		return(expansion_duration);
	} else {
		return(time_to_move);
	};
*/
};

//void animate_to_saved_size_one_second_callback(Sprite *sprite) {
//	sprite->animate_to_saved_size(1000);
//};

void Programmer_At_Floor::enter_thought_bubble(Room *room, Floor *starting_floor) {
	// in case some expansion still to do
	Cubby *cubby;
	if (thought_bubble_entered == NULL && original_thought_bubble != NULL) {
		thought_bubble_entered = (Thought_Bubble *) original_thought_bubble->copy(); // new on 051099 for restoring state when saved during training
		cubby = thought_bubble_entered->pointer_to_cubby();
	} else {
		thought_bubble_entered->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
		cubby = thought_bubble_entered->pointer_to_cubby();
	//	floor->remove_item(thought_bubble_entered);
		thought_bubble_entered->remove_from_floor();
		if (tt_system_mode != PUZZLE || is_toolbox_part_of_puzzle()) {
			thought_bubble_entered->remove_follower(tt_toolbox,FALSE);
		};
		thought_bubble_entered->remove_follower(thought_bubble_robot,FALSE);
	};
	thought_bubble_added_to_floor = FALSE;
	if (cubby != NULL) { // for robustness on 280803 but shouldn't happen
		thought_bubble_entered->remove_follower(cubby,FALSE);
//	floor->remove_leader(thought_bubble_entered);
//	tt_toolbox->recursively_remove_leaders(floor);
// don't need the following since new Floor takes care of it
//	cubby->recursively_remove_leaders(floor);
//		cubby->remove_from_floor(FALSE); // commented out on 070105 since sprite::remove_follower now deals with this
#if TT_DEBUG_ON
	} else {
		log("Entering thought bubble without a cubby.");
#endif
	};
//	floor = room->create_floor(cubby);
	if (room != NULL) { // new on 061099
		floor = new Floor(THOUGHT_BUBBLE_BACKGROUND,room,cubby,thought_bubble_robot);
		// original floor will be restored from the room
		tt_screen->switch_to(floor);
		if (tt_toolbox != NULL && !tt_toolbox->gone()) { // second condition added 030300
			tt_toolbox->remove_from_floor(FALSE);
			if (tt_system_mode != PUZZLE  || is_toolbox_part_of_puzzle()) { // moved inside this conditional on 030300
				floor->add_item(tt_toolbox);
			};
		};
		//	if (tt_martian != NULL) {
		if (tt_system_mode != PUZZLE) tt_martian->change_background(floor);
	} else {
		floor = starting_floor;
	};
	floor->add_item(thought_bubble_robot,FALSE);
//	cubby->recursively_restore_leaders(floor);
	// enter will do this again
//	tt_toolbox->recursively_restore_leaders(floor);
//	cubby->recursively_set_background(floor);
   set_appearance(thought_bubble_robot);
	robot_in_control(TRUE);
};

Robot *Programmer_At_Floor::enter_thought_bubble2() {
   Cubby *cubby = thought_bubble_entered->pointer_to_cubby();
   thought_bubble_entered->set_cubby(NULL);
	city_coordinate start_llx = cubby->current_llx();
	cubby->animate_location(start_llx,tile_height*10,1000);
	// on 101199 changed so that if cubby is too wide it is narrowed and centered
	city_coordinate new_width = cubby_that_started_training->current_width();
//	if (tt_log_version_number > 15) {
		new_width = min(new_width,ideal_screen_width-2*start_llx);
//	};
   cubby->animate_size(new_width,cubby_that_started_training->current_height(),1000);
	if (tt_toolbox != NULL && !tt_toolbox->gone()) { // second condition added 030300
      tt_toolbox->animate_to_favorite_size(500);
	   tt_toolbox->enter(FALSE);
   };
	if (thought_bubble_robot->pointer_to_body() == NULL) { // condition new on 130405 since if retraining don't want this help
		enter_thought_bubble_help();
	};
	return(thought_bubble_robot);
};

// RobotPointer &robot_pointer,Screen *screen) {
//	  floor->remove_toolbox();
//	  robot_pointer = thought_bubble_robot;
//	  robot_pointer->set_priority(appearance->priority());
//	  return(thought_bubble_robot
//				  ->enter_thought_bubble(thought_bubble_entered,screen));


Sprite *Programmer_At_Floor::hand_state(HandState &hand_state) {
	hand_state = hand;
//	floor->remove_toolbox();
	if (hand == EMPTY_HAND) return(NULL);
//	floor->remove_item(tool_in_hand); // about to leave
//   appearance->remove_follower(tool_in_hand,floor);
//	appearance->recursively_remove_leaders(floor);
//   grasp_x = grasp_x_offset;
//   grasp_y = grasp_y_offset;
	return(tool_in_hand);
};

ProgrammerStatus Programmer_At_Floor::restart_status() {
   if (floor->on_the_ground()) {
      return(START_ON_THE_GROUND);
   } else if (appearance->kind_of() == ROBOT_IN_TRAINING) {
		return(START_IN_THOUGHT_BUBBLE);
	} else {
      return(START_AT_FLOOR);
   };
};

void Programmer_At_Floor::set_visibility(boolean visible) {
	// added on 300499 so that for example Marty explains what's happening the first time
//	tt_game_switch_on = !visible;
	tt_programmer->set_visibility(visible);
	if (!visible) {
//		tt_screen->remove(appearance);
		if (!did_user(USER_HAS_HIDDEN_HAND)) {
			say(IDS_F9_PRESSED_HELP,2);
			user_did(GLOBAL_HISTORY,USER_HAS_HIDDEN_HAND,appearance);
		};
//	} else {
//		tt_screen->add(appearance);					
	};
#if TT_DEBUG_ON
	if (tt_debug_mode != 0) {
		tt_error_file() << "Hand visibility now " << (int) visible
						<< " at frame " << tt_frame_number 
						<< " at time " << tt_current_time << endl;
	};
#endif
};

// noticed obsolete on 020105
//void Programmer_At_Floor::add_items_from_outside(Sprites *items, city_coordinate x, city_coordinate y) {
//	::add_items_from_outside(items,floor,NULL,x,y);
//};

void Programmer_At_Floor::add_item_from_outside(Sprite *item, city_coordinate x, city_coordinate y) {
	::add_item_from_outside(item,floor,NULL,x,y);
};

void Programmer_At_Floor::set_tool_in_hand(Sprite *new_tool) {
	if (tool_in_hand != NULL) tool_in_hand->set_held_status(NOT_HELD);
	tool_in_hand = new_tool;
	hand = compute_hand_state(tool_in_hand,!using_arm); // new on 270100
	if (tool_in_hand != NULL) { // new on 220901
		tool_in_hand->set_held_status(HELD_BY_PROGRAMMER);
		// made max of time for hand and tool to calm down on 071201
		// also was sent to tool_in_hand rather than appearance
		if (tt_log_version_number > 27) {
			appearance->do_after(max(appearance->total_cycle_duration(),tool_in_hand->total_cycle_duration()),tool_in_hand,
										TELL_PROGRAMMER_AM_READY_CALLBACK);
		} else {
			tool_in_hand->do_after(tool_in_hand->total_cycle_duration(),tool_in_hand,TELL_PROGRAMMER_AM_READY_CALLBACK);
		};
	} else {
		use_selection_region = FALSE;
	};
};

void Programmer_At_Floor::tool_in_hand_is_ready(Sprite *tool) { // new on 220901
	// tool has finished animating (e.g. morphing from inert) and can now record active region
	if (tool != tool_in_hand) return; // things have changed
	if (tool->kind_of() != VACUUM && tool->kind_of() != EXPANDER) { 
		// new on 250102 since only Dusty and Pumpy animate and have fixed selection regions
		use_selection_region = FALSE;
		return;
	};
	use_selection_region = TRUE;
//	tool_in_hand->finish_instantly(); // added on 210102 so things like F2, F2 work while training a robot -- didn't help
	tool_in_hand->active_region(selection_region);
	city_coordinate llx,lly;
	appearance->lower_left_corner(llx,lly);
	selection_region.shift_by(current_delta_x-llx,current_delta_y-lly);
	if (tt_log_version_number >= 79) { // new on 050805
		millisecond duration = appearance->total_cycle_duration();
		if (duration > 0) {
			tt_global_picture->do_after(duration,tool_in_hand,TELL_PROGRAMMER_AM_READY_CALLBACK);
		};
	};
};

void Programmer_At_Floor::note_absolute_mode_deltas(coordinate &delta_x, coordinate &delta_y, boolean left_button_down, 
																	 boolean left_button_just_went_down) {
   // did nothing prior to 170502
   delta_x = (delta_x*tt_screen_width)/return_client_width(); // new on 170502 in case window isn't 1 to 1 with tt_screen_width
   delta_y = (delta_y*tt_screen_height)/return_client_height();
};

boolean horizontal_shape(SpriteType type) {
	switch(type) {
	  case EXPANDER: // maybe pads too??
//	  case BIRD:
		  return(FALSE);
	  default:
		  return(TRUE);
	};
};

// obsolete?? -- though I can use it to avoid total_cycle_duration
// and use instead
// pointer_to_appearance()->set_cycle_stopped_callback(reset_programmer_timer_callback);

Programmer_Sprite::Programmer_Sprite(SpriteCode code,
												 city_coordinate x, city_coordinate y,
//												 Programmer *programmer,
                                     Visible_Background *background,
												 short int initial_parameter,
												 city_scale current_scale) :
	Sprite(code,x,y,PROGRAMMER,initial_parameter,current_scale) {
//	programmer(programmer)
  set_background(background);
  set_containable_by_background(FALSE); // handled specially
  set_move_between_floor_and_room(FALSE); // handled specially
  set_show_all(FALSE); // new on 060103 so dumps without SeeAll tag
//  set_size_porportional_to_screen_size(TRUE);
};

void Programmer_Sprite::display(SelectionFeedback , boolean , TTRegion *) {
//	Tool *selection = programmer->pointer_to_current_selection();
//	if (selection != NULL) {
//		selection->display(screen,TRUE);
//	};
//	Sprite::display(screen,selection_feedback);
	tt_programmer->display_appearance();
};

boolean Programmer_Sprite::save_region() {
//	Tool *selection = programmer->pointer_to_current_selection();
//	if (selection != NULL) {
//		selection->save_region(screen); // really a few extra pixels too
//	};
//	Sprite::save_region(screen);
	if (tt_programmer == NULL) return(FALSE); // new on 210105 since can happen with the loading_wait() now
	return(tt_programmer->save_appearance_regions());
};

void Programmer_Sprite::recent_speed(city_coordinate &x_speed,
												 city_coordinate &y_speed) {
	tt_programmer->programmer_at_floor()->recent_speed(x_speed,y_speed);
};

#if TT_XML

void Programmer_Sprite::xml(xml_element *element, xml_document *document) { // new on 191202
//	xml_element *element = Sprite::xml_create_element(document); // create_xml_element(xml_tag(),document); // L"ProgrammerAppearance"
	xml_set_attribute(element,L"AppearanceCode",(int) appearance_code());
	Sprite::xml(element,document); // new on 120703
};

#endif

/*
void Programmer::enter_helicopter(city_coordinate x, city_coordinate y,
											 Screen *screen) {
  state = new Programmer_Leave_Helicopter(TRUE,x,y,this);
  // center it
  state->pointer_to_appearance()
	  ->move_by(state->current_width()/2,state->current_height()/2);
  screen->add(pointer_to_appearance());
//  pointer_to_appearance()->set_notify_if_colliding(FALSE);
  screen->increment_number_needing_collision_notification(-1);
  // for now get rid of old one and create new one
  city->remove_extra(empty_helicopter,
							empty_helicopter_x,
							empty_helicopter_y);
  screen->remove(empty_helicopter);
  delete empty_helicopter;
  empty_helicopter = NULL;
};
*/


/*
Programmer_Leave_Helicopter::Programmer_Leave_Helicopter(
					  boolean entering,
					  city_coordinate old_x,
					  city_coordinate old_y,
					  Programmer *the_programmer)  {
  programmer = the_programmer;
//	x = old_x;
//	y = old_y;
	appearance = new Sprite(HELIOLEV,old_x,old_y);
	appearance->set_priority_function_of_lly(FALSE);
	appearance->set_parameter(entering); // to control reversal of cycle
	cycle_duration = appearance->total_cycle_duration();
	mode_switch_time = tt_current_time+cycle_duration;
	if (entering) {
		mode = WILL_BE_INSIDE;
	} else {
		mode = WILL_BE_OUTSIDE;
	};
};

ProgrammerStatus Programmer_Leave_Helicopter::react(boolean new_user_input,
																	 unsigned char,
																	 boolean ,
																	 city_coordinate ,
																	 city_coordinate ,
																	 char button_status,
																	 boolean ,
																	 millisecond ,
																	 Screen *) {
	ProgrammerStatus status = PROGRAMMER_NORMAL;
	switch (mode) {
	  case WILL_BE_INSIDE:
		 if (tt_current_time >= mode_switch_time) {
			 mode = INSIDE;
		 };
		 break;
	  case WILL_BE_OUTSIDE:
		 if (tt_current_time >= mode_switch_time) {
			 mode = OUTSIDE;
		 };
		 break;
	};
	if (mode == INSIDE) {
		status = TAKING_OFF;
	};
	if (new_user_input) {
		if (button_status == 2) {
			if (mode == OUTSIDE) {
				mode = WILL_BE_INSIDE;
				mode_switch_time = tt_current_time+cycle_duration;
			};
		} else if (mode == OUTSIDE) { // if outside and anything other than right button
			status = CITY_WALKING;
		};
	};
	appearance->update_display();
	return(status);
};

int compute_maximum_orientation_delta(city_coordinate total_cursor_movement,
												  int maximum_rotation_per_second,
												  city_coordinate minimum_movement_to_reorient,
												  millisecond duration) {
	int maximum_orientation_delta = (duration*maximum_rotation_per_second)/1000;
	if (total_cursor_movement < minimum_movement_to_reorient) {
		maximum_orientation_delta =
			 (maximum_orientation_delta*total_cursor_movement)
			 /minimum_movement_to_reorient;
	};
	return(maximum_orientation_delta);
};

int direction(long orientation) {
	// which of 8 directions
	int result = (orientation+22)/45;
	if (result > 7) result %= 8;
	return(result);
};

int compute_new_orientation(int orientation,
									 city_coordinate delta_x,
									 city_coordinate delta_y,
									 int maximum_orientation_delta) {
	if (delta_x == 0 && delta_y == 0) {
		return(orientation);
	};
	Direction new_direction = direction(delta_x,delta_y);
	int new_orientation = new_direction*45;
//	tt_error_file() << new_direction << " " << new_orientation << " "
//					<< delta_x << " " << delta_y << endl;
	int orientation_delta = new_orientation-orientation;
	if (orientation_delta < -180) {
		orientation_delta += 360;
	} else if (orientation_delta > 180) {
		orientation_delta -= 360;
	};
	if (orientation_delta > maximum_orientation_delta) {
		orientation += maximum_orientation_delta;
	} else if (orientation_delta < -maximum_orientation_delta) {
		// counter clockwise
		orientation -= maximum_orientation_delta;
	} else {
		orientation = new_orientation;
	};
	return(orientation);
};
*/


/* Old style -- removed on 050103
	ProgrammerStatus status_after_restarting = PROGRAMMER_NORMAL;
	ProgrammerStatus start_status_regardless;
	if (are_titles_over()) { // use current values - not starting ones
		city_coordinate llx,lly;
		Sprite *appearance = pointer_to_appearance();
		appearance->lower_left_corner(llx,lly);
      xml_set_attribute(programmer_element,L"Left",llx);
      xml_set_attribute(programmer_element,L"Bottom",lly);
		city_coordinate city_view_x,city_view_y;
		city_scale scale;
		tt_screen->viewing_region(city_view_x,city_view_y,scale);
      xml_set_attribute(programmer_element,L"CameraCenterX",city_view_x);
      xml_set_attribute(programmer_element,L"CameraCenterY",city_view_y);
      xml_set_attribute(programmer_element,L"CameraCenterZ",scale);
      xml_set_attribute(programmer_element,L"Direction",state->return_current_direction());
      status_after_restarting = state->restart_status();
      xml_set_attribute(programmer_element,L"StatusCode",(int) status_after_restarting);
		start_status_regardless = status_after_restarting;
	} else {
      xml_set_attribute(programmer_element,L"Left",starting_x);
      xml_set_attribute(programmer_element,L"Bottom",starting_y);
      xml_set_attribute(programmer_element,L"CameraCenterX",starting_view_center_x);
      xml_set_attribute(programmer_element,L"CameraCenterY",starting_view_center_y);
      xml_set_attribute(programmer_element,L"CameraCenterZ",starting_scale);			
		xml_set_attribute(programmer_element,L"Direction",starting_direction);
		xml_set_attribute(programmer_element,L"StatusCode",starting_status);
		start_status_regardless = starting_status;  // for following conditional
	};
	if (start_status_regardless == START_IN_THOUGHT_BUBBLE) {
      xml_element *start_element = create_xml_element(L"InThoughtBubble",document);
      xml_append_child(start_element,programmer_element);
		Robot *robot = (Robot *) pointer_to_appearance();
		tt_dumping_with_sharing_of_all_types = TRUE; // still make sense?
		robot->xml_create_and_append_element(start_element,document);  
//		robot->dump_working_set(stream); // if robot has a top cubby this happens -- deal with this?? 011102
		Background *floor = pointer_to_appearance()->pointer_to_background();
		floor->pointer_to_body_cubby()->xml_create_and_append_element(start_element,document); 
//		floor->dump(stream); -- why?? how can programmer be dumped if floor isn't also dumped??
		if (tt_events != NULL) {
         xml_element *events_element = create_xml_element(L"EventsSoFar",document);
         xml_append_child(events_element,start_element);
			tt_events->xml(events_element,document); 
		};
		xml_element *original_box_element = create_xml_element(L"OriginalBox",document);
      xml_append_child(original_box_element,start_element);
		// and dump the cubby that was given to the robot originally (the copy dumped above may have been changed)
		programmer_at_floor()->pointer_to_original_thought_bubble()->pointer_to_cubby()->xml_create_and_append_element(original_box_element,document);  
	} else if (start_status_regardless == START_AT_FLOOR) { 
		if (kind_of() == PROGRAMMER_AT_FLOOR) { 
			programmer_at_floor()->xml_deltas(programmer_element,document);
		} else { 
			xml_deltas_internal(number_of_deltas,saved_delta_xs,saved_delta_ys,saved_delta_times,programmer_element,document);
		};
	};
	if (status_after_restarting == START_AT_FLOOR) {
      xml_set_attribute(programmer_element,L"HandCode", (int) programmer_at_floor()->return_hand_state());
	} else {
      xml_set_attribute(programmer_element,L"HandCode", (int) saved_hand);
	};
   Sprite *tool = pointer_to_tool_in_hand();
   if (tool != NULL) {
		tool->finish_instantly(); 
      xml_element *tool_element = create_xml_element(L"InHand",document);
      xml_append_child(tool_element,programmer_element);
      tool->xml_create_and_append_element(tool_element,document);
//      tool->dump_geometry(stream); 011102 -- look into this??
   };
	tt_dumping_with_sharing_of_all_types = FALSE; // postponed here on 101099 since tool might be in robot's working set
   block_number block_x = 0,block_y = 0;
   short int index = -1;
   Background *floor = NULL;
	if (pointer_to_appearance() != NULL) {
		floor = pointer_to_appearance()->pointer_to_background();
	};
	House *house = NULL;
   if (floor != NULL) { // floor was room before 6/5/98
      house = floor->pointer_to_house();
	} else if (room != NULL) { // new on 210799
		house = room->pointer_to_house();
	};
	if (house != NULL) {
		Block *block = house->block_im_on();
		block->location(block_x,block_y);
		index = block->lot_index(house);
   };
	// revisit geometry issues - 011102
	//if (are_titles_over()) {
	//	Sprite *appearance = pointer_to_appearance();
	//	appearance->dump_geometry(stream); // new on 130100
	//} else if (saved_geometry_and_misc != NULL) { // added 150300 since demos starting from saved cities were broken
	//	saved_geometry_and_misc->dump_geometry(stream);
	//};
	*/

/* version prior to 050103:
boolean Programmer::handle_xml_tag(Tag tag, xml_node *node) { // new on 051102
   switch (tag) {
      case IN_HAND_TAG:
         set_tool_in_hand(xml_get_sprite(node)); // was just an assignment prior to 191202
         return(TRUE);
      case NO_MORE_TAGS: {
         if (saved_delta_xs == NULL) {
            initialize_saved_deltas();
         };
         starting_x = xml_get_attribute_int(node,L"Left",min_long);
         starting_location_set = (starting_x != min_long);
         starting_y = xml_get_attribute_int(node,L"Bottom",min_long);
         starting_view_center_x = xml_get_attribute_int(node,L"CameraCenterX",0);
         starting_view_center_y = xml_get_attribute_int(node,L"CameraCenterY",0);
         starting_scale = xml_get_attribute_int(node,L"CameraCenterZ",100);
         starting_direction = (Direction) xml_get_attribute_int(node,L"Direction",0);
         starting_status = (ProgrammerStatus) xml_get_attribute_int(node,L"StatusCode",0);
//	if (starting_status == START_IN_THOUGHT_BUBBLE) { // then robot was dumped as well
      // to do -- 051102
		//starting_robot = (Robot *) load_item(stream,version_number,language); // had better be a robot
		//starting_robot->set_sprite_type(ROBOT_IN_TRAINING);
		//starting_robot->appear_working();
		//starting_robot->set_selectable(FALSE);
  //    starting_robot->set_programmer(this);
		//starting_robot->load_working_set(stream,version_number,language);
		//starting_cubby = (Cubby *) load_item(stream,version_number,language); // had better be a cubby
		//if (starting_cubby == NULL) {
		//	log("Problems loading a city saved while a robot was being trained.");
		//};
		//starting_cubby->remove_from_floor();
		//starting_robot->remove_from_floor();
		//starting_floor = new Floor(THOUGHT_BUBBLE_BACKGROUND,room,starting_cubby,starting_robot); 
		//starting_cubby->remove_from_floor(); // was just added above but will be added below too
		//starting_floor->load(stream,version_number,language);
		//starting_robot->remove_from_floor(FALSE);
		//tt_events = load_events(stream,version_number);
		//starting_original_cubby = (Cubby *) load_item(stream,version_number,language); 
         saved_hand = (HandState) xml_get_attribute_int(node,L"HandCode",0);
         block_number x = xml_get_attribute_int(node,L"BlockX",0);
         block_number y = xml_get_attribute_int(node,L"BlockY",0);
         int index = xml_get_attribute_int(node,L"HouseNumber",0);
         Block *block = tt_city->block_at(x,y);
         if (block != NULL && index >= 0) {
            House *house = block->house_at(index);
            if (house != NULL) room = house->pointer_to_room();
         };
         //previous_x = starting_x;
         //previous_y = starting_y;
         saved_city_x = xml_get_attribute_int(node,L"CityX",0);
         saved_city_y = xml_get_attribute_int(node,L"CityY",0);
         empty_helicopter_x = xml_get_attribute_int(node,L"HelicopterX",0);
         empty_helicopter_y = xml_get_attribute_int(node,L"HelicopterY",0);
         city_view_x = xml_get_attribute_int(node,L"CityViewX",0);
         city_view_y = xml_get_attribute_int(node,L"CityViewY",0);
         floor_width_sitting_ratio = xml_get_attribute_int(node,L"FloorWidthSittingRation",0);
         floor_height_sitting_ratio = xml_get_attribute_int(node,L"FloorHeightSittingRation",0);
         llx_right_room_limit = xml_get_attribute_int(node,L"RoomRightLimit",0);
         lly_top_room_limit = xml_get_attribute_int(node,L"RoomTopLimit",0);
         sit_llx = xml_get_attribute_int(node,L"SittingX",0);
         sit_lly = xml_get_attribute_int(node,L"SittingY",0);
         saved_x = xml_get_attribute_int(node,L"SavedX",0);
         saved_y = xml_get_attribute_int(node,L"PSavedY",0);
			previous_x = xml_get_attribute_int(node,L"PreviousX",0);
			previous_y = xml_get_attribute_int(node,L"PreviousY",0);
   // revisit 051102
	//if (starting_location_set) {
	//	if (saved_geometry_and_misc == NULL) {
	//		saved_geometry_and_misc = new Sprite(HELIOFLY); // arg shouldn't really matter
	//	};
	//	saved_geometry_and_misc->load_geometry(stream,version_number,FALSE); // new on 130100
	//	// added FALSE arg on 150400 since this isn't a real load
	//};
	      if (tt_current_time > 0) { // new on 161299 - if not first log
		      set_next_status(starting_status);
	      };
         return(TRUE);
                         };
      default:
         return(Entity::handle_xml_tag(tag,node));
   };
};

void Programmer::initialize_saved_deltas() { // abstracted on 181102
   saved_delta_xs = new city_coordinate[number_of_deltas];
   saved_delta_ys = new city_coordinate[number_of_deltas];
   saved_delta_times = new millisecond[number_of_deltas];
   for (int i = 0; i < number_of_deltas; i++) {
      saved_delta_xs[i] = 0;
      saved_delta_ys[i] = 0;
      saved_delta_times[i] = 0;
   };
};
*/
/*
// the following only works if connected to the Internet and not via a NAT router

#include <winsock.h>
unsigned long IP_address() {
	// Init WinSock
	WSADATA wsa_Data;
	int wsa_ReturnCode = WSAStartup(MAKEWORD(2,2), &wsa_Data);
	// Get the local hostname
	char hostName[255];
	gethostname(hostName, 255);
	struct hostent *host_entry;
	host_entry=gethostbyname(hostName);
	string localIP = inet_ntoa (*(struct in_addr *)*host_entry->h_addr_list);
	WSACleanup();
	return(htonl(inet_addr(localIP)));
};
string Logotron_countries[] = 
 {"GB","UK","NZ","AU","HK","SG","IN","MY",
  // Europe except Sweden and Portugal:
  "IE","IM","AM","AT","BE","BY","CH","UK","ES","CZ","DE","DK","FI","FR","GR","HU","IT","EE","LV","LI","LT","LU","MT","MD",
  "NL","NO","PL","RO","RU","SK","UA",
  // Africa:P
  "AO","BF", "BI", "BJ", "BW", "CD", "CF", "CG", "CI", "CM", "CV", "DJ", "DZ", "EG", "EH", "ER", "ET", "GA", "GH", "GM", "GN", 
  "GQ", "GW", "KE", "KM", "LR", "LS", "LY", "MA", "MG", "ML", "MR", "MU", "MW", "MZ", "NA", "NE", "NG", "RE", "RW", "SC", "SD", 
  "SH", "SL", "SN", "SO", "ST", "SZ", "TD", "TG", "TN", "TZ", "UG", "ZA", "ZM", "ZW", ""
 };

Seller seller_from_country_code(string country_code) {
	if (country_code == NULL) return(ANIMATED_PROGRAMS); // and warn?
	switch (country_code[0]) {
		case 'S':
			if (country_code[1] == 'E') return(EKELUNDS); // Sweden
			break;
		case 'P':
			if (country_code[1] == 'T') return(CNOTINFOR); // Portugal
			break;
		case 'B':
			if (country_code[1] == 'R') return(DIVERTIRE); // Brazil
			break;
		case 'J':
			if (country_code[1] == 'P') return(MICRONET); // Japan
			break;
		case 'K':
			if (country_code[1] == 'R') return(MICRONET); // Korea
			break;
	};
	for (int i = 0; i < 100; i++) {
		if (Logotron_countries[i][0] == '\0') break; // ran out of countries
		if (country_code[0] == Logotron_countries[i][0] &&
			country_code[1] == Logotron_countries[i][1]) {
			return(LOGOTRON);
		};
	};
	return(ANIMATED_PROGRAMS);
};

Seller seller_from_IP(unsigned long local_IP_address) {
	string country_code = NULL;
	Seller seller = ANIMATED_PROGRAMS; // default
	// following could do a binary search but I doubt this takes much time anyway
	for (int i = 0; i < 1000000; i++) {
		if (IP_country_table[i].ipto == 0) {
			break; // didn't find one
		};
		if (local_IP_address >= IP_country_table[i].ipfrom &&
			 local_IP_address <= IP_country_table[i].ipto) {
			country_code = IP_country_table[i].country;
			seller = seller_from_country_code(country_code);
			break;
		};
	};
	return(seller);
};
*/

