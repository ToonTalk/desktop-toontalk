// Copyright (c) 1992-2006.  Ken Kahn, Animated Programs, All rights reserved.
 
#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif
#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif
#if !defined(__TT_CUBBY_H)   
#include "cubby.h"
#endif
#if !defined(__TT_STRINGS_H)
#include "strings.h"
#endif   
#if !defined(__TT_SCREEN_H)   
#include "screen.h"
#endif   
#if !defined(__TT_BCKGRND_H)   
#include "bckgrnd.h"
#endif   
#if !defined(__TT_FLOOR_H)   
#include "floor.h"
#endif   
#if !defined(__TT_NUMBER_H)   
#include "number.h"
#endif   
#if !defined(__TT_TEXT_H)   
#include "text.h"
#endif   
#if !defined(__TT_PICTURE_H)   
#include "picture.h"
#endif   
#if !defined(__TT_EVENT_H)   
#include "event.h"
#endif   
#if !defined(__TT_MOUSE_H)   
#include "mouse.h"
#endif
#if !defined(__TT_ROBOT_H)   
#include "robot.h"
#endif   
#if !defined(__TT_PRGRMMR_H)   
#include "prgrmmr.h"
#endif   
#if !defined(__TT_PAD_H)   
#include "pad.h"
#endif
#if !defined(__TT_BIRD_H)   
#include "bird.h"
#endif   
#if !defined(__TT_UTILS_H)   
#include "utils.h" // for sound stuff
#endif   
#if !defined(__TT_HELP_H)   
#include "help.h"
#endif
#if !defined(__TT_JAVA_H)
#include "java.h"
#endif
#if !defined(__TT_MARTIAN_H)
#include "martian.h"
#endif
#if !defined(__TT_WINMAIN_H)
#include "winmain.h"
#endif
#if TT_XML
#if !defined(__TT_XML_H)
#include "xml.h"
#endif
#endif

#include <string.h>
#if !TT_32
#include <stdlib.h> // for min, max, range
#include <mmsystem.h> // for timeGetTime
#endif
//#include <strstrea.h> commented out 210703 // for ostrstream

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum CubbyStates {NULL_CUBBY_IMAGE,ONE_CUBBY_IMAGE,REST_CUBBY_IMAGE,
						BLANK_CUBBY_IMAGE,ONE_CUBBY_LABELED_IMAGE,
						REST_CUBBY_LABELED_IMAGE};

// if changed update help.cpp
enum CubbyHistory {CUBBY_HOLE_FILLED=0,
						 CUBBY_CONTENTS_REMOVED,
						 CUBBY_USED_FOR_CONCATENATION,
						 CUBBY_SIZE_CHANGED,
						 CUBBY_LABELED,
						 CUBBY_BLANKED,
						 CUBBY_BLANKED_INSIDE_THOUGHT_BUBBLE,
						 CUBBY_BLANK_EXPLAINED,
						 CUBBY_CONVERSION,
						 CUBBY_BROKEN,
						 CUBBY_ARITY_OBTAINED};

//void set_size_constrained_false_callback(void *t) {
//	((Tool *) t)->set_size_constrained(FALSE);
//};

const int x_pegs_per_first_piece = 6; // 4 for hole and 2 for walls
const int x_pegs_per_following_piece = x_pegs_per_first_piece-1;
const int y_pegs_per_piece = 4;

Cubby::Cubby(city_coordinate x, city_coordinate y, long initial_priority,
//				 Background *floor,
				 city_coordinate initial_width, city_coordinate initial_height,
				 int number_of_holes, Sprite **contents, // removed (cubby_index) coerecion on 070203
				 string *labels) :
  Sprite(CUBBY_SPRITE,x,y,CUBBY,1, // by default 1-cubby
		 ground_scale,//FALSE,
		 initial_width,initial_height,TRUE),
  contents(contents),
  item_being_sucked_up(NULL),
  item_being_sucked_up_index(0), // wasn't initialized prior to 121202
  main_cubby_in_body(FALSE), // unless told otherwise
  number_of_holes(number_of_holes),
//  move_after_sucking_up(FALSE),
  busy_counter(0),
//  action_status(NULL),
//  inside_sprite_flag(FALSE), // noticed obsolete on 121202
  labels(labels),
  previous_width(0), previous_height(0),
  non_busy_callbacks(NULL),
//  saved_by(NULL),
  last_drop_index(-1),
  future_text(NULL), // wasn't initialized prior to 121202
  saved_right_side(FALSE), // wasn't initialized prior to 121202
  returning_items(NULL) {
//   set_priority_function_of_lly(FALSE); .mkb file handles this
	current_priority = initial_priority;
	if (initial_width <= 0) {
		peg_width = tile_height/2; // new on 301103
	} else { // new on 251103 to otherwise not update peg widths while initializing
		update_piece_widths();
	};
	if (number_of_holes > 1 && initial_height == 0) {
		adjust_height();
	};
//	set_collision_region_equals_region(TRUE);
//   if (!true_sizes) {
//      true_size(width,height); // adjust small cubbies relative to maximum size
//   };
   set_size_is_true(TRUE); // width and height are correct -- not current_image dimensions
	if (contents == NULL) {
		initialize_contents(0,number_of_holes);
	} else {
		for (int i = 0; i < number_of_holes; i++) { // removed (cubby_index) coerecion on 070203
			if (contents[i] != NULL) {
				initialize_hole(i,contents[i],TRUE); 
				// was FALSE prior to 240103 since didn't always work 
				// FALSE added on 180103 as an optimization - make sure geometry is OK
//				fit_into_hole(i,contents[i]);
//				contents[i]->absorb_followers(); // especially for pictures
//				add_follower(contents[i],floor);
			};
		};
	};
	update_parameter();
};

void Cubby::adjust_height() {
//	const int x_peg_count 
//	  = x_pegs_per_first_piece+x_pegs_per_following_piece*(number_of_holes-1);
//#if TT_DEBUG_ON
//	if (x_peg_count == 0) {
//		tt_error_file() << "about to devide by zero" << endl;
//		return;
//	};
//#endif	  
//	height = (width*y_pegs_per_piece)/x_peg_count;
	height = y_pegs_per_piece*(peg_width*tt_ideal_units_per_vertical_pixel/tt_ideal_units_per_horizontal_pixel);
};

void Cubby::good_size(city_coordinate &w, city_coordinate &h, Sprite *source, boolean) {
	h = 4*tile_height;
	// could re-organize definition of following to have better interface here
//	w = compute_sprite_width(0,h,appearance_code());
	// since cubby is 6x4 a peg height and width is a tile_height
	if (is_blank()) { 
		// new on 170102 since if blank doesn't matter how many holes it "had"
		w = tile_height*(x_pegs_per_following_piece+1);
	} else if (number_of_holes == 0) { 
		w = 2*tile_height;
	} else {
		w = tile_height*(x_pegs_per_following_piece*number_of_holes+1);
	};
	// moved here on 311003 so can return if not on floor
	if (tt_good_sizes_adjust_to_screen_size) {
		w = shrink_width_from_640x480_screen_size(w);
		h = shrink_height_from_640x480_screen_size(h);
	};
	if (floor != NULL) {
		if (floor->in_the_room_with_programmer()) { // updated 020205 
			tt_programmer->translate_floor_to_room_dimensions(w,h);
			return; // new on 311003
		};
	} else if (tt_programmer != NULL && tt_programmer->kind_of() == PROGRAMMER_ROOM_WALKING) { // new on 311003 to deal with objects dereferenced by robot that don't have a floor 
		tt_programmer->translate_floor_to_room_dimensions(w,h);
		return; // new on 311003
	};
	// prior to 311003 was:
	//if (floor != NULL && floor->in_the_room()) { 
	//	tt_programmer->translate_floor_to_room_dimensions(w,h);
	//};
	//if (tt_good_sizes_adjust_to_screen_size) {
	//	w = shrink_width_from_640x480_screen_size(w);
	//	h = shrink_height_from_640x480_screen_size(h);
	//};
	adjust_size_for_source(w,h,source);
};

void Cubby::initialize_contents(int old_size, int new_size, // removed (cubby_index) coerecion on 070203
										  boolean excess_on_right, int excess_holes) {
	Sprite **new_contents;
	if (new_size > 0) {
		new_contents = new SpritePointer[new_size];
		int i; // removed (cubby_index) coerecion on 070203
		int old_i; // removed (cubby_index) coerecion on 070203
		int stop = min(old_size,new_size); // removed (cubby_index) coerecion on 070203
		for (i = 0; i < stop; i++) {
			if (excess_on_right || excess_holes < 0) {
				old_i = i;
			} else {
				old_i = i+excess_holes; // removed (cubby_index) coerecion on 070203
			};
			new_contents[i] = contents[old_i];
			if (new_contents[i] != NULL) {
				// shouldn't have moved but fix it anyway
				fit_into_hole(i,new_contents[i],TRUE);
				if (new_contents[i]->pointer_to_leader() != NULL) {
               // follower/leader link broken -- for example if blank
               new_contents[i]->record_offsets();
            };
				contents[old_i] = NULL;
			};
		};
		for (; i < new_size; i++) {
			new_contents[i] = NULL;
		};
	} else {
		new_contents = NULL;
	};
	delete [] contents;
	contents = new_contents;
};

//void Cubby::prepare_for_deletion() {
//#if TT_DEBUG_ON
//  if (tt_debug_target == debug_counter) {  
//     tt_error_file() << "Deleting debug target (cubby)" << endl;
//  };
//#endif
//   if (preparing_for_deletion) return; // already done
////	preparing_for_deletion = TRUE; // new on 060204
//   Sprite::prepare_for_deletion();
//};

void Cubby::release_all_references() { // was release_outgoing_connections() prior to 020504
	if (releasing_all_references) { // new on 230305
		return;
	};
//	set_number_of_holes(0); // new on 311003 used to unbam - will be clobbered soon
	// replace the above with the following moved from cubby::prepare_for_deltion
	if (is_main_cubby_in_body() && floor != NULL) { // new on 151204 // second test added on 020405 since can happen in time travel
		floor->need_new_main_cubby();
	};
	if (returning_items != NULL && !tt_resetting_or_destroying_city) { // added on 160799
		// 170999 added && !tt_shutting_down since this crashed ToonTalk while shutting down
		// tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 but can cause memory errors while resetting during time travel
		returning_items->finish_instantly();
	};
	just_destroy_list(returning_items); // should be NULL anyway
   if (blank) { // contents aren't followers if blank
      for (int i = 0; i < number_of_holes; i++) { // removed (cubby_index) coerecion on 070203
         if (contents[i] != NULL) contents[i]->destroy();
      };
   };
	if (labels != NULL) {
		for (int i = 0; i < number_of_holes; i++) { // removed (cubby_index) coerecion on 070203
			if (labels[i] != NULL) {
				delete [] labels[i];
			};
		};
		delete [] labels;
		labels = NULL;
	};
	if (contents != NULL) {
		delete [] contents;
		contents = NULL;
		number_of_holes = 0;
	};
	if (non_busy_callbacks != NULL) {
		delete non_busy_callbacks;
		non_busy_callbacks = NULL;
	};
	Sprite::release_all_references(); // new on 030504
};

Sprite *Cubby::copy(boolean also_copy_offsets) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Copying debug target.");
	};
#endif
  // if (!infinite_stack()) { // copied here on 151204 since can happen with remote looks of the flip side of a picture
  //    Sprite *copied = already_copied(this);
  //    if (copied != NULL) {
		//	return(copied);
		//};
  // };
	Sprite **contents_copy;
	string *labels_copy = NULL;
	if (number_of_holes > 0) {
		contents_copy = new SpritePointer[number_of_holes];
		if (labels != NULL) {
			labels_copy = new string[number_of_holes];
		};
	} else {
		contents_copy = NULL;
	};
   int i; // removed (cubby_index) coerecion on 070203
	for (i = 0; i < number_of_holes; i++) {
		if (labels != NULL) {
			if (labels[i] != NULL) {
				labels_copy[i] = copy_string(labels[i]);
			} else {
				labels_copy[i] = NULL;
			};
		};
	};
	Cubby *copy = new Cubby(llx,lly, //center_x,center_y,
									current_priority-1,
									width,height,
									number_of_holes,NULL,//contents_copy,
									labels_copy);
//	just_copied(this,copy); 
	copy->set_contents(contents_copy);
	for (i = 0; i < number_of_holes; i++) {
		if (contents[i] == NULL || (tt_copying_entire_thought_bubble && blank && !tt_copy_restores_pre_blank)) {
			// second condition new on 151204
			contents_copy[i] = NULL;
		} else {
			contents_copy[i] = contents[i]->copy(!blank || tt_copy_restores_pre_blank); // if blank then temporarily not a follower
//			if (tt_copying_entire_thought_bubble && contents_copy[i]->kind_of() == PICTURE) { 
			// new on 121200 since in thought bubble don't want remote looks
//				Picture *picture_copied = (Picture *) contents_copy[i];
//				Sprite *remote_looks_of = picture_copied->pointer_to_controlee();
//				if (remote_looks_of != NULL) {
//					picture_copied->set_active(FALSE);
//				};
//				if (remote_looks_of != NULL && remote_looks_of->kind_of() == PICTURE) {
					// remove indirection
//					picture_copied->set_controlee(NULL);
//					picture_copied->set_indirection(remote_looks_of,TRUE);
//				};
//			};
			// sometimes reset ?? -- just to be safe...
//			contents_copy[i]->set_size(contents[i]->current_width(),
//												contents[i]->current_height());
//			contents_copy[i]->inside_cubby_hole();
			if (!blank || tt_copy_restores_pre_blank) {
				copy->add_follower(contents_copy[i],TRUE,INSERT_UNORDERED,TRUE); // added TRUE as an optimization on 260103
	         contents_copy[i]->set_priority(copy->priority()-1); // why bother if inserted unordered? - asked 260103
				contents_copy[i]->set_infinite_stack(contents[i]->infinite_stack()); // new on 080400
			};
		};
	};
//	if (saved_width != 0) {
//		copy->set_saved_size(saved_width,saved_height);
//	};
	if (blank) {
		if (tt_copy_restores_pre_blank) {
//			copy->refit_contents(); // commented out on 260103
		} else {
			copy->become_blank();
		};
	};
//#if TT_DEBUG_ON
//	perform_integrity_checks(copy);
//#endif
 //  copy->set_priority_fixed(priority_fixed_flag);
	//if (comment != NULL) {
	//	copy->set_comment(comment->copy());
	//};
   finish_making_copy(copy,also_copy_offsets);
	return(copy);
};

ReleaseStatus Cubby::item_released_on_top(Sprite *item, Sprite *by, SpritePointer &recipient, int level) {
  // following excluded dropping something in a hole if a bird is returning to a different hole
  // now the release is ignored only if they want the same hole
//	if (item_being_sucked_up != NULL) {
//		return(RELEASE_IGNORED); // about to have something in it
//	};
	if (to_be_smashed()) { // new on 210703 so if something is dropped in a hole before the boxes are joined it is ignored
		// replaced abort_action by abort_action_if_robot on 120104 since otherwise Marty says strange things if the programmer is doing this
		by->abort_action_if_robot(-1); // robots should only do this if the programmer is messing around while he's running
		return(RELEASE_IGNORED);
	};
 	if (infinite_stack()) { 
		if (!tt_copy_top_of_stack && !by->still_running()) return(RELEASE_IGNORED);
		recipient = copy_top_of_stack(by);
		UnwindProtect<boolean> set(tt_recording_off,TRUE);
		recipient->item_released_on_top(item,by,recipient,level);
      if (by->kind_of() == ROBOT_IN_TRAINING) {
         set_available_for_indexing(FALSE); // so a path to it is used
      };
		record_event(RELEASE_ITEM_ON,by,floor,this);
      item->now_released_on(this,by);
		return(RELEASE_HANDLED);
	};
	if (level > tt_max_containment_levels) {
		by->abort_action(IDS_TOO_MANY_BOXES_INSIDE_BOXES);
		return(RELEASE_IGNORED);
	};
	if (blank) {
		if (receive_item(item,by,default_duration(),this,item)) { // on 010799 changed to check if really received
			user_did(CUBBY,CUBBY_CONVERSION);
			// moved back to receive_item on 280403 since interferes with retraining (Roundup issue #97)
			//record_event(RELEASE_ITEM_ON,by,floor,this); // moved here from before receive_item on 010799
			//record_event(NEW_ITEM,by,floor,item); // added 010799
			return(RELEASE_HANDLED);
		} else {
			return(RELEASE_IGNORED);
		};
	};
//	if (number_of_holes == 0) {
//		if (item->kind_of() == CUBBY) {
//			// extending this
//			Cubby *other = (Cubby *) item;
//			set_number_of_holes(other->current_number_of_holes());
			// and copy contents
//			return(RELEASED_CUBBY_NEXT_TO_CUBBY);
//		} else {
//			return(RELEASE_IGNORED);
//		};
//	};
	if (number_of_holes == 0) { // moved inside on 121003 && leader == NULL) { // added leader == NULL test on 250903
		if (leader == NULL) {
			return(add_to_side(TRUE,item,by,default_duration(),this,item));
		} else { // new on 121003
			return(RELEASE_IGNORED);
		};
   };
	//if (number_of_holes == 0 && leader == NULL) { // added leader == NULL test on 250903
	//	return(add_to_side(TRUE,item,by,default_duration(),this,item));
	//};
	long hole_index;
   if (by != NULL && by->kind_of() == ROBOT) { // new on 280300 since geometry doesn't matter with robots
		// this should only happen when robot dropped on a sensor like parts or container that forwards to a cubby
		hole_index = 0;
	} else {
		city_coordinate item_center_x,item_center_y;
		// was center_location
		item->true_center(item_center_x,item_center_y); // was center of collision_region
		hole_index = closest_hole(item_center_x,item_center_y);
		if (hole_index < 0) {	// dropped to the left of cubby
			if (level == 0 && item->kind_of() == CUBBY && !item->is_blank()) { // might be extending cubby
	//			city_coordinate item_llx,item_lly;
	//			item->lower_left_corner(item_llx,item_lly);
				city_coordinate item_llx = item->current_llx();
				if (item_llx+item->current_width() < hole_center_x(0) &&
					 // right edge of item is left of first hole's center and
					 // not too much above or below me
					 !(item_center_y < lly || item_center_y > lly+height)) {
					user_did(CUBBY,CUBBY_USED_FOR_CONCATENATION,by);
					return(add_to_side(FALSE,item,by,default_duration(),this,item));
				};
			};
			hole_index = 0; // must have "meant" first hole
		} else if (hole_index == number_of_holes) { // dropped to right of cubby
			if (level == 0 && item->kind_of() == CUBBY && !item->is_blank()) {
	//			city_coordinate item_llx,item_lly;
	//			item->lower_left_corner(item_llx,item_lly);
				city_coordinate item_llx = item->current_llx();
				if (number_of_holes == 0 || // null cubby
					 (item_llx > hole_center_x(number_of_holes-1) && // not too much to left // removed (cubby_index) coerecion on 070203
					  // not too much above or below me
					  !(item_center_y < lly || item_center_y > lly+height))) {
					user_did(CUBBY,CUBBY_USED_FOR_CONCATENATION,by);
					return(add_to_side(TRUE,item,by,default_duration(),this,item));
				};
			};
			hole_index = number_of_holes-1; // must have "meant" first hole
		};
	};
	if (item_being_sucked_up != NULL && item_being_sucked_up_index == hole_index) {
	  return(RELEASE_IGNORED);
	};
	if (contents[hole_index] != NULL) { // already has something in it
//		SpriteType contents_type = contents[hole_index]->kind_of();
		// let contents decide what to do
		boolean geometry_ok;
		if (by != NULL && by->kind_of() == ROBOT) { // new on 210300 to ignore geometry if a robot did this
			geometry_ok = TRUE;
		} else {
			TTRegion contents_region, item_region;
			contents[hole_index]->full_region(contents_region);
			item->full_region(item_region);
			geometry_ok = contents_region.overlaps(&item_region);
		};
		if (geometry_ok) {
			recipient = contents[hole_index];
			return(recipient->item_released_on_top(item,by,recipient,level+1));
		};
		by->abort_action_if_robot(IDS_BOX_WONT_ACCEPT);
      item->released(by);
		return(RELEASE_IGNORED);
	};
//	if (item->kind_of() == CUBBY && ((Cubby *) item)->is_main_cubby_in_body()) {
	if (item->is_main_cubby_in_body()) { // simplified on 060705
		// need to switch to so this guy (the container) is main cubby
		Cubby *container = this;
		while (container->pointer_to_leader() != NULL &&
				 container->pointer_to_leader()->kind_of() == CUBBY) {
			container = (Cubby *) container->pointer_to_leader();
		};
		record_event(NEW_MAIN_CUBBY,by,floor,container);
	};
	if (!hole_receives_item(item,hole_index,by,accept_item_duration,FALSE)) { // action_status // removed (cubby_index) coerecion on 070203
		return(RELEASE_IGNORED); // not quite
	};
	if (by->is_action_aborted()) return(RELEASE_IGNORED);
	if (!by->still_running()) {
		user_did(CUBBY,CUBBY_HOLE_FILLED);
		if (item->fine_kind_of() == SCALE_PICTURE) {
			scale_used();
		};
	};
	boolean training = (by->kind_of() == ROBOT_IN_TRAINING);
   if (training) {
      item->set_available_for_indexing(FALSE); // so a path to it is used
   };
   record_event(RELEASE_ITEM_ON,by,floor,item);
	if (training) {
      item->set_available_for_indexing(TRUE);
   };
   item->now_released_on(this,by);
	if (training && !item->available_for_indexing()) { // release reset its availability -- new on 201003
		((Robot *) by)->remove_tool_from_working_set(item); // training means it must be a Robot
	};
	return(RELEASE_HANDLED);  // was RELEASED_IN_CUBBY_HOLE
};

boolean Cubby::receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item) {
   if (inside_vacuum()) {
      by->abort_action(IDS_ROBOT_MISSING_RECIPIENT_ABORT);
      return(FALSE);
   };
   if (blank) {
		long future_text_length; // no longer an instance variable
		if (item->current_wide_text(future_text,future_text_length) ||
			 item->kind_of() == ROBOT || // or exploding the team
			 item->kind_of() == PROGRAM_PAD) { // new on 131200
			// about to become exploded version of text so
			// throw away undo information
			for (int i = 0; i < number_of_holes; i++) {
			  if (contents[i] != NULL) {
				  contents[i]->destroy(by);
				  contents[i] = NULL;
			  };
			};
			Background *items_floor = item->pointer_to_background();
			// prior to 5/21/98 this would destroy item -- but why?
			if (duration > 0) { 
				call_in_a_mouse(this,item,items_floor,by,FALSE,
									 (::ok_to_delete_bammed(by)), // && item->ok_to_delete()), // this arg was FALSE prior to 131000
									 // and on 090105 added second condition -- e.g. dropping the main notebook on an erased box
									 // removed 090105 since better to protect it in notebook::release_all_references
									 duration,original_recipient,original_item);
			} else {
				if (items_floor != NULL) { // condition added on 170800
					items_floor->remove_item(item,!blank); // !blank was TRUE prior to 031000
				};
				set_to_future_value(item,original_recipient,by);	
				// arg was NULL prior to 310399 but crashed when robot drops something on a blank box in another house		  
//				if (::ok_to_delete_bammed(by)) {  
				// new on 131000 -- commented out on 310101 since is wrong modularity and 
				// caused a notebook dropped on an erased box to be doubly deleted
//					item->destroy(by);
//				};
				by->completed();
			};
			if (by != NULL) {
				if (by->kind_of() == ROBOT) { // new on 010799
					Robot *robot = (Robot *) by;
//					if (robot->return_body_version_number() >= first_version_to_correctly_deal_with_pads_dropped_on_blank_boxes) {
						robot->add_to_working_set(item,TRUE);
//					};
				} else if (by->kind_of() == ROBOT_IN_TRAINING) {
					// moved here on 280403
					record_event(RELEASE_ITEM_ON,by,floor,this); // moved here from before receive_item on 010799
					record_event(NEW_ITEM,by,floor,item); // added 010799
				};
			};
			return(TRUE);
		};
		return(FALSE);
	};
	by->abort_action(IDS_ONLY_BLANK_BOX_CAN_ACCEPT,MOUSE_WITH_HAMMER);
	return(FALSE);
};

void Cubby::set_to_future_value(Sprite *other, Sprite *original_recipient, Sprite *by) {
	switch (other->kind_of()) {
		case CUBBY: // adding to the side
			add_to_side(saved_right_side,other,saved_by,0,NULL,other);
			break;
		case ROBOT:
			{
				Robot *robot = (Robot *) other;
				Robot *first_in_line = robot->pointer_to_first_in_line();
				if (first_in_line != NULL) {
					robot = first_in_line;
				};
				//blank = FALSE;
				become_non_blank(); // new on 080904 so size is correct if had been 0-holed
				set_number_of_holes(robot->robots_in_team()); // moved here so runs after the above as of 080904
				set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL); // new on 080904 -- arg new on 141204
				other->remove_from_floor();
				for (unsigned short int i = 0; i < number_of_holes; i++) {
					Robot *next_robot = robot->pointer_to_next_robot();
					robot->set_first_in_line(NULL);
					if (next_robot != NULL) {
						robot->remove_follower(next_robot);
						robot->set_next_robot(NULL);
						robot->set_robot_to_put_behind(NULL); // new on 051204
					};
					// moved following on 180101 here so copy happens after removing it from the team
					initialize_hole(i,robot->copy()); // added ->copy() on 180101 despite the following comment: no need to copy since original is destroyed by Bammer - since otherwise it is destroyed and in box
					if (next_robot == NULL) break;
					robot = next_robot;
				};
			};
			break;
		case PROGRAM_PAD:
			{ 
				Notebook *notebook = (Notebook *) other;
				set_number_of_holes(notebook->last_non_blank_page_number());
				blank = FALSE;
				for (int i = 0; i < number_of_holes; i++) {
					Sprite *page = notebook->pointer_to_page(i+1); // 1-indexing
					if (page != NULL) { // conditional added on 310101
						initialize_hole(i,page); 
					};
				};
			};
			break;
		case INTEGER:
			{
				long long_value;
				if (((NUMBER *) other)->current_long_value(long_value)) { // what if value is negative??
					set_number_of_holes(long_value);
					blank = FALSE;
				}; // else warn??
			};
			break;
		default:
			set_number_of_holes(wide_string_length(future_text)); // was future_text_length prior to 121202
			blank = FALSE;
			int true_number_of_holes = 0; // differs if new line and carriage return means there are 2 characters to start the next line
			for (unsigned short int i = 0; i < number_of_holes; i++) {
				// number_of_holes was future_text_length prior to 160499 - this matters due to tt_max_number_of_holes
				Text *text = new Text();
            if (future_text[i] == '\r' || future_text[i] == '\n') { // prior to 090802 was just == '\r') { // new on 210201
					if (i < number_of_holes-1 && future_text[i+1] == '\n') i++;
					text->set_text(S(IDS_ENTER_KEY));
				} else {
					wide_string letter = new wide_character[2];
					letter[0] = future_text[i];
//					initialize_hole(i,new Text(0,0,letter));
					text->set_wide_text(letter,1);
				};
				initialize_hole(true_number_of_holes,text);
				true_number_of_holes++;
//#if TT_WIDE_TEXT
//				delete [] letter;
//#endif
			};
			if (true_number_of_holes < number_of_holes) {
				set_number_of_holes(true_number_of_holes);
			};
	};
};

void Cubby::initialize_hole(int hole_index, Sprite *item, boolean geometry_needs_to_set) { // removed (cubby_index) coerecion on 070203
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("initialize_hole of debug target.");
	};
#endif
	if (hole_index >= number_of_holes) { // new on 240103 since a crash report had this happening
		tt_error_file() << "Initialing hole number " << hole_index << " of a box that doesn't have such a hole. Ingoring it." << endl;
		return;
	};
	// geometry_needs_to_set new on 180103 so no need to rework the geometry if already fine or about to be reset again (e.g. XML)
//#if TT_DEBUG_ON
//	perform_integrity_checks(this);
//	perform_integrity_checks(item);
//#endif
//   item->activate(FALSE); // only some callers want this
	if (geometry_needs_to_set) {
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
//		contents[hole_index]->absorb_followers(); // especially for pictures
		item->reset_cycle(); // for consistency, otherwise copies etc will look different
		item->update_display(UPDATE_TO_FIT_IN_CUBBY); // put it in correct state wrt offsets, etc.
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
	};
	contents[hole_index] = item;
	// since this does increment_ref_count then set_number_of_holes where save_excess is FALSE will destroy these and produce a warning about negative ref count -- worry about this? 240204
	if (!blank) { // condition new on 260103
		add_follower(item,TRUE,INSERT_UNORDERED);
	};
	// re-ordered these on 140103 so when loading XML the following has leader_offsets when called after size is set below
	if (geometry_needs_to_set) {
		fit_into_hole(hole_index,item,TRUE);
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
//	item->inside_cubby_hole();
		item->set_priority(priority()-1);
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
	//} else if (item->current_width() > 40000 && width < 40000) {
	//	debug_this();
#endif
	};
	// following shouldn't be needed when called by loading XML
	if (tt_suspensions != NULL) tt_suspensions->retry(&(contents[hole_index]));
//	tt_error_file() << "Putting " << item << " in hole " << hole_index
//					<< " of " << this << " on frame " << tt_frame_number << endl; 
//#if TT_DEBUG_ON
//	perform_integrity_checks(this);
//#endif
};

boolean Cubby::hole_receives_item(Sprite *item, int hole_index, Sprite *by, // removed (cubby_index) coerecion on 070203
											 millisecond duration,
//											 ActionStatus *the_action_status,
											 boolean record_this) {
	if (blank) { // hole doesn't get but is probably something like exploding a text pad
		return(receive_item(item,by,duration,this,item)); // the_action_status
	};
	if (item == this) {
		by->abort_action(IDS_BOX_INSIDE_SELF);
		return(FALSE);
	};
   if (hole_index >= number_of_holes) {
      by->abort_action(IDS_BOX_DOESNT_HAVE_EXPECTED_HOLE);
      return(FALSE);
   };
	if (item_being_sucked_up != NULL) { // something else is falling as well - new on 121202
		finish_instantly();
		item_being_sucked_up->finish_instantly(); // depends where it is in the process
	};
	if (contents[hole_index] != NULL) {
		// nearly center item over hole first
		city_coordinate hole_llx,hole_lly,hole_width,hole_height;
		cubby_insides_of_index(hole_index,hole_llx,hole_lly,hole_width,hole_height);
		hole_llx += hole_width/2-(2*item->current_width())/5;
		hole_lly += hole_height/2-(3*item->current_height())/5;
		item->move_to(hole_llx,hole_lly);
		item->update_remote(FALSE); // new on 060605 since there may be sensors that need to know about the move above
		// following screwed up size...
//		fit_into_hole(hole_index,item);
		// above is too exact -- looks confusing
//		item->move_by(item->current_width()/5,item->current_height()/-5);
		Sprite *ignore;
		contents[hole_index]->item_released_on_top(item,by,ignore,1); // ,the_action_status
		return(TRUE); // what if problem above??
	};
	if (!cubby_contents_type(item->kind_of())) {
		if (infinite_stack()) {
			by->action_aborted();
		} else {
			by->abort_action_if_robot(IDS_CUBBY_BAD_CONTENTS_ABORT); // the_action_status
			item->released(by); // the_action_status
		};
		return(FALSE);
	};
   set_saved_by(by); // restored on 171102 since replaces action_status as well
//	action_status = the_action_status;
	item_being_sucked_up = item;
	item_being_sucked_up_index = hole_index;
//   saved_screen = screen;
//	item_being_sucked_up->save_size();
//	item_being_sucked_up->set_size_constrained(TRUE);
	if (record_this) {
		// record_event(RELEASE_ITEM_ON,by,floor,item_being_sucked_up);
		duration = 0; // new on 160405 since can't wait to record this
	};
	if (floor != NULL) { // should always be so
      floor->add_item(item_being_sucked_up,FALSE,FALSE,FALSE); // so it'll be updated
   };
	if (duration > 0) {
		if (item != by) play_sound(NEAT_EFFECT_SOUND); // no sound if bird is returning
		if (priority() <= item_being_sucked_up->priority()) {
			// cubby is on top
			item_being_sucked_up->set_priority(priority()-1);
		};
		item_being_sucked_up->set_selectable(FALSE);
		// until thing falls inside don't allow picking it up
		// since might drop it into item_being_sucked_up
		set_selectable(FALSE);
//		if (animation_in_progress()) { // conditional new on 121202 since otherwise set duration is set 0 and no nice animation happens
		// bad idea since it make it look bad
			duration = animations_stop_time()-tt_current_time;
//		};
		do_after(duration, // animations_stop_time()-tt_current_time, // // might be a stack copy
					this,ANIMATE_SUCK_UP_CALLBACK);
	} else {
		suck_up();
	};
	if (record_this) { // copied here on 160405 since need to have the above happen first
		((Robot *) by)->remove_tool_from_working_set(item); // new on 160405 so the following works during retraining
		record_event(RELEASE_ITEM_ON,by,floor,item); // item_being_sucked_up);
	};
	return(TRUE);
};

#if TT_DEBUG_ON
void Cubby::check_integrity() {
  for (int i = 0; i < number_of_holes; i++) {
	  if (followers != NULL && contents[i] != NULL && 
			followers->member_cons(contents[i]) == NULL) {
		  Sprite *contents_leader = contents[i]->pointer_to_leader();
		  Sprite *contents_leader_leader = NULL;
		  if (contents_leader != NULL) {
			  contents_leader_leader = contents_leader->pointer_to_leader();
		  };
		  Sprite *second_follower = NULL;
		  if (followers->rest() != NULL) {
			  second_follower = followers->rest()->first();
		  };
		  tt_error_file() << "Problems with this cubby " << this
						  << "; follower is " << second_follower << endl
						  << "content is " << contents[i]
						  << "; content leader is " << contents_leader
						  << "; content leader leader is " << contents_leader_leader << endl;
	  };
  };
};
#endif

void Cubby::animate_suck_up() {
	city_coordinate item_llx,item_lly,item_width,item_height;
	cubby_insides_of_index(item_being_sucked_up_index,item_llx,item_lly,item_width,item_height);
	item_being_sucked_up->adjust_size_and_location(item_width,item_height,
																  item_llx,item_lly);
	item_being_sucked_up->animate_true_size_and_location(item_width,item_height, // _true_ added 151199
																		  item_llx,item_lly,
																		  default_duration(),
																		  this,SUCK_UP_CALLBACK);
};

void Cubby::suck_up() {
   action_completed();
	set_selectable(TRUE); // moved here on 210105 so this happens even if item_being_sucked_up == NULL
	if (item_being_sucked_up == NULL) {
		return; // how does this happen?
	};
	item_being_sucked_up->set_selectable(TRUE);
//	contents[item_being_sucked_up_index] = item_being_sucked_up;	
	// sometimes it misses the hole -- don't know why but this patches it
//	fit_into_hole(item_being_sucked_up_index,item_being_sucked_up);
//	add_follower(item_being_sucked_up,floor);
	// can something not have a floor these days??
//	if (floor != NULL) {
//		floor->remove_item(item_being_sucked_up,FALSE);
//	};
//	item_being_sucked_up->finish_instantly(); // new on 140504 since bird can still be animating when returning
	item_being_sucked_up->remove_from_floor(FALSE);
	initialize_hole(item_being_sucked_up_index,item_being_sucked_up);
	item_being_sucked_up->set_priority(priority()-1);
	// commented out on 061100 since when something is dropped in a box it shouldn't start running
//	item_being_sucked_up->activate(FALSE);
	if (returning_items != NULL) {
		returning_items = returning_items->remove(item_being_sucked_up);
	};
// add_follower in initialize_hole does this
//	set_clean_status(DIRTY);
/*
			if (item_being_sucked_up->pointer_to_followers() != NULL) {
				item_being_sucked_up->propagate_changes();
				item_being_sucked_up->clear_changes();
				recursively_remove_leaders(item_being_sucked_up);
			};
*/
	item_being_sucked_up = NULL;
//	if (move_after_sucking_up) {
//		animate_location(llx+tile_width/4,lly-tile_height/4, //tiny bit
//							  showing_on_a_screen()?250:0);
//		move_after_sucking_up = FALSE;
//	};
//      } else {
//       item_being_sucked_up->set_clean_status(DIRTY);
//      };
};

ReleaseStatus Cubby::add_to_side(boolean right_side, Sprite *sprite,
											Sprite *by, 
											millisecond duration, // ActionStatus *status,
                                 Sprite *original_recipient, Sprite *original_item) {
	if (sprite == NULL || sprite->kind_of() != CUBBY) {
		by->abort_action_if_robot(IDS_CANT_ADD_NON_BOX);
		return(RELEASE_IGNORED);
	};
	if (blank) {
		by->abort_action(IDS_CANT_ADD_TO_SIDE_OF_BLANK_BOX);
		return(RELEASE_IGNORED);
	};
	Cubby *other = (Cubby *) sprite;
	if (other->is_main_cubby_in_body()) {
		record_event(NEW_MAIN_CUBBY,by,floor,this);
//		return(other->add_to_side(!right_side,this,by,duration,status));
	};
   set_saved_by(by);
	if (duration > 0) { // && tt_log_version_number > 12) {
		saved_right_side = right_side;
		call_in_a_mouse(this,sprite,sprite->pointer_to_background(),by,FALSE,TRUE,0,original_recipient,original_item); // status,
	} else {
//		if (duration > 0 && showing_on_a_screen()) play_sound(POP_SOUND);
		int size_of_other = other->current_number_of_holes(); // removed (cubby_index) coerecion on 070203
		city_coordinate old_llx = llx;
		city_coordinate old_lly = lly;
		int original_number_of_holes = number_of_holes; // removed (cubby_index) coerecion on 070203
		set_number_of_holes(number_of_holes+size_of_other);
		boolean any_labels = labeled() || other->labeled();
		if (right_side) {
			for (int i = original_number_of_holes; i < number_of_holes; i++) { // removed (cubby_index) coerecion on 070203
				Sprite *contents = other->remove_component_directly(i-original_number_of_holes); // removed (cubby_index) coerecion on 070203
				if (contents != NULL) {
					initialize_hole(i,contents);
				};
				if (any_labels) {
					is_labeled();
					set_label(i,other->label(i-original_number_of_holes,TRUE)); // removed (cubby_index) coerecion on 070203
				};
		//		other->set_label(i-original_number_of_holes,NULL);
			}; 
		} else {
			long i;
			for (i = original_number_of_holes-1; i >= 0; i--) { // removed (long) coerecion on 070203
				// since already follower, etc. just move these around inside cubby
				Sprite *item = contents[i]; // was remove_component_directly((cubby_index) i);
				if (item != NULL) {
					contents[i] = NULL;
	//				initialize_hole((cubby_index) (i+size_of_other),contents);
					contents[i+size_of_other] = item;
					fit_into_hole(i+size_of_other,item,TRUE); // removed (cubby_index) coerecion on 070203
				};
				if (any_labels) {
					set_label(i+size_of_other,label(i,TRUE)); // removed (cubby_index) coerecion on 070203
				};
		//		set_label(i,NULL); // not sharing strings here
			};
			for (i = 0; i < size_of_other; i++) {
				Sprite *contents = other->remove_component_directly(i); // removed (cubby_index) coerecion on 070203
				if (contents != NULL) {
					initialize_hole(i,contents); // removed (cubby_index) coerecion on 070203
				};
				if (any_labels) {
					set_label(i,other->label(i,TRUE)); // removed (cubby_index) coerecion on 070203
				};
		//		other->set_label(i,NULL);
			};	  
		};
		if (original_recipient != NULL) { // not called from set_to_future_value
			//if (floor != NULL) { // for robustness - was NULL when running Leonel's Roamer rotating program 081003 (really 061003 I think)
			//	floor->remove_item(other,FALSE);
			//};
			other->remove_from_floor(FALSE); // rewritten 081003
			other->destroy(by,FALSE);
			action_completed(); // saved_by and by are the same so this is fine
		};
		if (right_side) {
			move_to(old_llx,old_lly);
		} else {
			// move so original holes don't move
			if (original_number_of_holes == 0 || size_of_other == 0) {
				move_to(old_llx,old_lly);
			} else {
				move_to(old_llx-width/number_of_holes,old_lly);
			};
		};
	};
	if (right_side) {
		if (original_recipient != NULL) {
			record_event(RELEASE_ITEM_RIGHT_OF,by,floor,this);
		};
		return(RELEASED_ITEM_RIGHT_OF_ITEM);			
	} else {
		if (original_recipient != NULL) {
			record_event(RELEASE_ITEM_LEFT_OF,by,floor,this);
		};
		return(RELEASED_ITEM_LEFT_OF_ITEM);			
	};
};

Sprite *Cubby::respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected,
										     Sprite *by, boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless) {
	// could allow entering a number larger than 9
   // some problems with that are entering 12 when holding say a 3 arity box
   // typing the 1 will break the box ...
	// starting 150499 allow keyboard events from robots even if excluded by programmer
	// removed infinite_stack() || from conditional below since toggle infinite stack is supported now and programmer does the right thing
	if (tt_system_mode == PUZZLE && (by == NULL || by->kind_of() != ROBOT)) return(this);
	if (extended) return(this); // new on 100604 since otherwise things are the arrow keys can cause strange labels
   // ignore keystrokes over stack or during puzzles
	if (animation_in_progress()) {
		finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
	};
	long number = -1; 
	boolean excess_on_right = TRUE;
	if (current_held_status() != NOT_HELD || by->still_running()) {
		// following short cuts only apply if held
		if (key >= '0' && key <= '9') { // a digit
			number = key-'0';
		} else {
			switch (key) {
				case '+':
					number = number_of_holes+1L;
					break;
				case '-':
					number = number_of_holes-1L;
					excess_on_right = FALSE;
					break;
				case BACKSPACE:
					number = number_of_holes-1L;
					break;
				default:
					if (key == tt_stop_character) { // new functionality as of 121103
						set_active(FALSE);
					} else if (key == tt_run_character) {
						set_active(TRUE);
					};
			};
		};
	} else {
		update_labels(key,extended,by);
		user_did(CUBBY,CUBBY_LABELED,by);
		return(this);
	};
	if (number >= 0) { // generated a new number
		user_did(CUBBY,CUBBY_SIZE_CHANGED,by);
      set_saved_by(by);
		Cubby *excess = set_number_of_holes(number,excess_on_right,TRUE);
      if (record_action) {
			record_event(KEYBOARD_CHARACTER,by,floor,this,TRUE,key);
			record_action = FALSE; // added on 080400 so don't record again by Sprite::respond_to_keyboard
		};
		if (excess != NULL && floor != NULL) { // floor check added on 140201 for robustness
			floor->add_item(excess);
			if (by->showing_on_a_screen()) {
				// play crack sound
				play_sound(SWITCH_SOUND);
			};
		};
		if (excess != NULL && by != NULL && tt_log_version_number >= 53) {
			if (by->kind_of() == ROBOT) {
				// new on 260904 in case this happens while a robot is running
				if (showing_on_a_screen()) { // why bother otherwise?
					((Robot *) by)->regrasp_tool();
				};				
			} else {
				// new on 240904 since if box is significantly smaller looks bad to hold far from object
				tt_programmer->set_grasp_delta_x(0); // despite changes of 121104 this is still needed
			};
		};
//    if (record_action)   // used to generate working set stuff even if called by robot
		if (excess != NULL) { // new on 181004 since if cubby expanded then nothing was created and robot gets confused
			record_event(NEW_ITEM,by,floor,excess,TRUE,0,ADD_TO_WORKING_SET_REGARDLESS);
		};
	};
//	completed(action_status); // replaced by Sprite::respond_to_keyboard on 080400
   return(Sprite::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,size_constrained_regardless));
};

void Cubby::generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection) {
	if (blank) {
		if ((contained_in(THOUGHT_BUBBLE) && !did(CUBBY_BLANKED_INSIDE_THOUGHT_BUBBLE)) ||
			 !did(CUBBY_BLANK_EXPLAINED) ||
			 tt_help_counter == 1) {
			 if (tt_help_counter == 0) {
				 describe_pointing(message,tool_in_hand,selection);
			 };
		};
		if (tt_help_counter == 1) {
			message << S(IDS_CUBBY_BLANK_HELP);
//		} else if (contained_in(THOUGHT_BUBBLE) &&
//					  !did(CUBBY_BLANKED_INSIDE_THOUGHT_BUBBLE)) {
//			tt_help_history_action = CUBBY_BLANKED_INSIDE_THOUGHT_BUBBLE;
//			message << S(IDS_CUBBY_BLANKED_TB_HELP);
//			user_did(CUBBY,CUBBY_BLANKED_INSIDE_THOUGHT_BUBBLE);
		} else if (!did(CUBBY_BLANK_EXPLAINED) || tt_help_counter == 2) {
			tt_help_history_action = CUBBY_BLANK_EXPLAINED;
			message << S(IDS_CUBBY_BLANK_HELP2);
			no_help_interruptions();
			user_did(CUBBY,CUBBY_BLANK_EXPLAINED);
		} else if (!did(CUBBY_CONVERSION) || tt_help_counter == 3) {
			tt_help_history_action = CUBBY_CONVERSION;
			message << S(IDS_TEXT_OR_NUMBER_TO_BOX_HELP);
		};
	} else if ((!did(CUBBY_HOLE_FILLED) && tt_help_counter == 0) 
				  || tt_help_counter == 1) {
		tt_help_history_action = CUBBY_HOLE_FILLED;
//		if (tt_help_counter == 0) {
			describe_pointing(message,tool_in_hand,selection);
//		};
		message << S(IDS_CUBBY_HOLE_FILLED_HELP);
	} else if ((!did(CUBBY_CONTENTS_REMOVED) && tt_help_counter == 0) 
				  || tt_help_counter == 2) {
		tt_help_history_action = CUBBY_CONTENTS_REMOVED;
		message << S(IDS_CUBBY_CONTENTS_REMOVED_HELP);
		describe_hand(message);
		message << S(IDS_CUBBY_CONTENTS_REMOVED_HELP2);
		message << S(IDS_CUBBY_CONTENTS_REMOVED_HELP3);
	} else if ((!did(CUBBY_USED_FOR_CONCATENATION) && tt_help_counter == 0) 
				  || tt_help_counter == 3) {
		tt_help_history_action = CUBBY_USED_FOR_CONCATENATION;
		message << S(IDS_CUBBY_CONCATENATION_HELP);
	} else if (((!did(CUBBY_SIZE_CHANGED) && tt_help_counter == 0)
					|| tt_help_counter == 4)) {
		if (tool_in_hand == this || tt_help_counter == 4) { // only if held
			tt_help_history_action = CUBBY_SIZE_CHANGED;
			message << S(IDS_CUBBY_SIZE_CHANGED_HELP)
					  << S(IDS_CUBBY_SIZE_CHANGED_HELP2)
					  << S(IDS_CUBBY_SIZE_CHANGED_HELP3)
					  << S(IDS_CUBBY_SIZE_CHANGED_HELP4);
		};
		if (tool_in_hand == NULL || tt_help_counter == 4) {
			message << S(IDS_CUBBY_SIZE_CHANGED_HELP5);
		};
	} else if ((tool_in_hand == NULL || tool_in_hand == this) &&
				  ((!did(CUBBY_LABELED) && tt_help_counter == 0)
					|| tt_help_counter == 5)) {
		tt_help_history_action = CUBBY_LABELED;
		message << S(IDS_CUBBY_LABELED_HELP);
	} else if (((!did(CUBBY_BLANKED) && tt_help_counter == 0)
					|| tt_help_counter == 6)) {
		tt_help_history_action = CUBBY_BLANKED;
		message << S(IDS_VACUUM_TO_BLANK_HELP);
	} else if (((!did(CUBBY_BROKEN) && tt_help_counter == 0)
					|| tt_help_counter == 7)) {
		tt_help_history_action = CUBBY_BROKEN;
		message << S(IDS_DROP_ON_NUMBER_TO_BREAK_HELP);
	} else if (((!did(CUBBY_ARITY_OBTAINED) && tt_help_counter == 0)
					|| tt_help_counter == 8)) {
		tt_help_history_action = CUBBY_ARITY_OBTAINED;
		message << S(IDS_DROP_ON_BLANK_NUMBER_TO_OBTAIN_ARITY_HELP);
//		describe_middle_button(message);
//      message << ".  ";
	} else if ((!did(TYPE_HELP_GIVEN) && tt_help_counter == 0)
					|| tt_help_counter == 9) {
//		tt_help_history_action = TYPE_HELP_GIVEN;
		message << S(IDS_CUBBY_HELP_GIVEN_HELP)
				  << S(IDS_CUBBY_HELP_GIVEN_HELP2);
		no_help_interruptions();
		user_did(CUBBY,TYPE_HELP_GIVEN);
	};
};

void Cubby::update_labels(char key, boolean extended, Sprite *by) {
	long index;
	if (number_of_holes > 1) {
		TTRegion by_region;
		by->pointer_region(by_region);
		city_coordinate by_x,by_y;
		by_region.region_center(by_x,by_y);
		index = closest_hole(by_x,by_y);
	} else {
		index = 0; // only one
	};
	if (index < 0 || index == number_of_holes) return;
	update_label(key,extended,index,by);
	record_event(LABEL_CHARACTER,by,floor,this,TRUE,key,DONT_ADD_TO_WORKING_SET,FALSE,index); // DONT_ADD_TO_WORKING_SET was FALSE in beta 8A
};

void Cubby::update_label(char key, boolean extended, long index, Sprite *by) {
	if (index < 0 || index == number_of_holes) {
		by->abort_action(IDS_CANT_ADD_TO_BOX_LABEL);
		return;
	};
	if (labels == NULL) {
		is_labeled();
	};
   labels[index] = update_string(labels[index],key,extended,254); // 254 added on 061199 -- maybe only if done by robots??
	set_clean_status(DIRTY);
	by->completed();
};

void Cubby::is_labeled() {
   if (labels != NULL) return;
	labels = new string[number_of_holes];
	for (int i = 0; i < number_of_holes; i++) { // removed (cubby_index) coerecion on 070203
		labels[i] = NULL;
	};
	update_parameter();
};

string Cubby::label(int index, boolean remove) { // removed (cubby_index) coerecion on 070203
	if (labels == NULL) return(NULL);
	string label = labels[index];
	if (remove) labels[index] = NULL;
	return(label);
};

void Cubby::set_label(int index, string new_label) { // removed (cubby_index) coerecion on 070203
	if (labels == NULL) {
		labels = new string[number_of_holes];
		for (int i = 0; i < number_of_holes; i++) { // removed (cubby_index) coerecion on 070203
			labels[i] = NULL;
		};
      update_parameter(); // new on 140802
	};
	if (labels[index] != NULL) delete [] labels[index];
	labels[index] = new_label;
};

void Cubby::become_blank(boolean by_user, boolean controlee_too) {
	Sprite::become_blank(by_user,controlee_too);
//	if (followers_cached()) { // can't see them anymore
//		tt_screen->invalidate_cache();
//	};
	update_parameter();
	if (by_user) {
		if (contained_in(THOUGHT_BUBBLE)) {
			user_did(CUBBY,CUBBY_BLANKED_INSIDE_THOUGHT_BUBBLE);
		};
		user_did(CUBBY,CUBBY_BLANKED);
	};
	Sprites *remaining = followers;
	while (remaining != NULL) {
      remaining->first()->set_background(NULL,TRUE,FALSE);
		// re-ordered on 070105 since remove_follower doesn't recursively reset background
		remove_follower(remaining->first());
		remaining = remaining->rest();
	};
};

void Cubby::become_non_blank(boolean controlee_too) {
	Sprite::become_non_blank(controlee_too);
	for (int i = 0; i < number_of_holes; i++) {
		if (contents[i] != NULL) {
			add_follower(contents[i],TRUE,INSERT_UNORDERED);
         contents[i]->set_background(floor,TRUE,FALSE); // or should this warn since floor may not know about this...
         contents[i]->set_priority(priority()-1);
		};
	};
	update_parameter();
   refit_contents();
};

void Cubby::update_parameter() {
	if (blank) {
		set_parameter(BLANK_CUBBY_IMAGE);
	} else if (number_of_holes > 0) {
		set_parameter((short) (labels==NULL?ONE_CUBBY_IMAGE:ONE_CUBBY_LABELED_IMAGE));
	} else {
		set_parameter(NULL_CUBBY_IMAGE);
	};
};

void Cubby::initialize_labels(string *new_labels) {
	labels = new_labels; // should only be called when labels was NULL
	update_parameter();
};

// noticed this was obsolete on 121202
//void Cubby::set_to_planned_number_of_holes() {
//	set_number_of_holes(planned_number_of_holes);
//};

// noticed this was obsolete on 121202
//void Cubby::set_number_of_holes_after(millisecond duration,
//                                      long number) {
////      set_number_of_holes_after_this = not_before;
//	planned_number_of_holes = number;
//	do_after(duration,this,SET_HOLES_CALLBACK);
//};

Cubby *Cubby::set_number_of_holes(long desired_number,
											 boolean excess_on_right,
                                  boolean save_excess,
                                  boolean initializing) {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Changing number of holes of debug target."));
	};
#endif
   int number; // removed (cubby_index) coerecion on 070203
   if (desired_number > tt_max_number_of_holes) {
		if (limited_text_to_speech()) tt_martian->just_talk_balloons_for(30000); // has URL etc in it
      say(IDS_TOO_MANY_HOLES,4,TRUE);
      number = tt_max_number_of_holes; // removed (cubby_index) coerecion on 070203
   } else {
      number = desired_number; // removed (cubby_index) coerecion on 070203
   };
	if (number == number_of_holes) {
       if (!save_excess) {
          //if (saved_by != NULL) { // commented out on 191102 since when action_completed runs this is done
          //   saved_by->destroy(); // if zero ref count
          //   saved_by = NULL; // finished with it
          //};
          return(NULL);
       };
       // so that in general a robot can change the size of things even when sometimes it is the same size
		 Cubby *excess = new Cubby(llx+width+tile_width/2,
                                 lly+tile_height/-2+my_random(tile_height));
       excess->set_number_of_holes(0,TRUE,FALSE,TRUE);
       excess->set_size(peg_width*2,height);
       //if (saved_by != NULL) { // commented out on 191102 since when action_completed runs this is done
       //   saved_by->destroy(); // if zero ref count
       //   saved_by = NULL; // finished with it
       //};
		 if (is_main_cubby_in_body() && tt_programmer->training()) { // new on 151204
			 excess->increment_ref_count();
			 floor->add_alternative_body_cubby(excess);
		 };
       return(excess);
   };
	set_clean_status(DIRTY);
	Cubby *excess = NULL;
	int first_excess; // removed (cubby_index) coerecion on 070203
	if (excess_on_right) {
		first_excess = number;
	} else {
		first_excess = 0;
	};
	int excess_holes = number_of_holes-number;
	if (excess_holes > 0) {
		if (save_excess) {
			city_coordinate excess_llx;
         city_coordinate excess_width = width_given_hole_count(excess_holes); // removed (cubby_index) coerecion on 070203
			if (excess_on_right) {
				if (held_status != NOT_HELD &&
					 (leader != NULL &&
					  (leader->kind_of() == ROBOT || leader->kind_of() == ROBOT_IN_TRAINING))) {
					excess_llx = llx+width-excess_width;
				} else {
//#if TT_BETA
//               if (tt_running_old_demos) {
//                  excess_llx = llx+width;
//               } else
//#endif
					excess_llx = llx+width_given_hole_count(number);
				};
			} else {
				if (held_status == NOT_HELD) {
					excess_llx = llx;
				} else {
					excess_llx = llx-excess_width;
				};
			};
			if (excess_llx < 0) {
				excess_llx = 0;
			} else if (excess_llx >= ideal_screen_width) {
				excess_llx = 19*tile_width;
			};
			// random so that if used frequently easier to see individual pieces
			excess = new Cubby(excess_llx,lly+tile_height/-2+my_random(tile_height));
			excess->set_number_of_holes(excess_holes,TRUE,FALSE,TRUE); // removed (cubby_index) coerecion on 070203
         excess->set_size(excess_width,height);
			if (is_main_cubby_in_body() && tt_programmer->training()) { // new on 151204
				excess->increment_ref_count();
				floor->add_alternative_body_cubby(excess);
			};
//			excess->set_to_good_size();
		};
		string *excess_labels = NULL;
		if (save_excess && labels != NULL) {
			excess_labels = new string[excess_holes];
		};
		for (int i = 0; i < excess_holes; i++) {
			cubby_index old_index = first_excess+i; // removed (cubby_index) coerecion on 070203
			// must empty the holes before destroying them
			Sprite *old_contents = remove_component_directly(old_index);
			if (old_contents != NULL) {
				if (save_excess) {
					excess->initialize_hole(i,old_contents); // removed (cubby_index) coerecion on 070203
				} else {
					// not clear why it is a floor item but
					// this seems to prevent leaving deleted sprites on the floor's
					// list of items
					if (floor != NULL) { // conditional new since this can be called now within prepare_for_deletion
						floor->remove_item(old_contents,FALSE);
					};
					old_contents->destroy(saved_by,showing_on_a_screen());
				};
			};
			if (labels != NULL) {
				if (save_excess) {
					excess_labels[i] = labels[old_index];
					labels[old_index] = NULL;
				} else if (labels[old_index] != NULL) {
					delete [] labels[old_index];
					labels[old_index] = NULL;
				};
			};
		};
		if (excess_labels != NULL) {
			excess->initialize_labels(excess_labels);
		};
	};
	if (labels != NULL) {
		if (excess_holes < 0) {
			string *new_labels = new string[number];
         int i; // removed (cubby_index) coerecion on 070203
			for (i = 0; i < number_of_holes; i++) {
				new_labels[i] = labels[i];
				labels[i] = NULL;
			};
			for (;i < number; i++) {
				new_labels[i] = NULL;
			};
			delete [] labels;
			labels = new_labels;
		} else if (!excess_on_right) {
			for (int i = 0; i < number; i++) {
				labels[i] = labels[i+excess_holes];
			};
		};
	};
   if (excess != NULL && is_blank()) excess->become_blank();
	if (number == 0) {
		number_of_holes = 0;
		if (!size_constrained() && !is_blank()) { // added is_blank condition on 170102
			city_coordinate old_width = width;
			// divide by 3 since a zero hole box is only 2 pegs not 6 across
			set_width(width_given_scale(y_scale)/3); // size not constrained -- prior to 201003 was just width = but then if held by robot leader offsets not updated
			move_by((old_width-width)/2,0); // to stay centered - e.g. if held by robot so still in hand -- new on 201003
//			width = width_given_scale(y_scale)/3; // size not constrained
		};
		set_parameter(NULL_CUBBY_IMAGE);
      //if (saved_by != NULL) { // commented out on 191102 since when action_completed runs this is done
      //   saved_by->destroy(); // if zero ref count
      //   saved_by = NULL; // finished with it
      //};
		return(excess);
	};
	if (leader == NULL || leader->kind_of() != CUBBY) {
		 // what about nests?
		  // && leader->kind_of() != THOUGHT_BUBBLE)) {
		  // size is unconstrained so recompute width
		long new_peg_count = compute_peg_count(number); // x_pegs_per_following_piece*number+1;
//      if (tt_running_old_demos) {
		   // width of peg is scaled proportionately
//		   peg_width = width_given_scale(y_scale)/x_pegs_per_first_piece;
//      };
		city_coordinate new_width = peg_width*new_peg_count;
		if (leader != NULL &&
			 (leader->kind_of() == PROGRAMMER ||
			  leader->kind_of() == ROBOT ||
			  leader->kind_of() == ROBOT_IN_TRAINING)) {
           leader->forget_tool_dimensions();
//			notify_leader(number < number_of_holes);
		} else {
//			new_width = min(new_width,max_sprite_width);
			if (save_excess) {
				// shouldn't the following use set_position or the like? asked 201003
				if (!excess_on_right) { // otherwise leave llx alone
					llx += width_given_hole_count(excess_holes); // removed (cubby_index) coerecion on 070203
				};
         } else if (!initializing) {
				llx -= (new_width-width)/2; // grow and shrink centered
			};
		};
		if (held()) {// new on 161103 since otherwise can end up way off screen
			move_by((width-new_width)/2,0); // to stay centered - e.g. if held by robot so still in hand -- new on 201003
		};
		set_width(new_width); // better to always use :set_size... -- new on 201003
//    width = new_width; //min(new_width,max_sprite_width);
		if (leader != NULL) record_offsets();
//      previous_width = width; // peg_width should be fine so don't trigger update with possible round-off problems
//		if (saved_width != 0) {
//			saved_width = new_width;
//			saved_height = height;
//		};
	};
	int old_number = number_of_holes; // removed (cubby_index) coerecion on 070203
	number_of_holes = number; // removed (cubby_index) coerecion on 070203
	update_parameter();
	update_piece_widths();
	initialize_contents(old_number,number_of_holes,excess_on_right,excess_holes);
   //if (saved_by != NULL) { // commented out on 191102 since when action_completed runs this is done
   //   saved_by->destroy(); // if zero ref count
   //   saved_by = NULL; // finished with it
   //};
	return(excess);
};

city_coordinate Cubby::width_given_hole_count(int hole_count) { // removed (cubby_index) coerecion on 070203
   return((hole_count*5+1)*peg_width);
};

//void Cubby::adjust_grasp(city_coordinate &adjustment_x,
//								 city_coordinate &adjustment_y) {
//	if (number_of_holes == 0) {
//		// width is for 1 hole cubby not just 2 peg 0 hole
//		adjustment_x = (2*width)/3;
//		adjustment_y = 0;
//	} else {
//		Sprite::adjust_grasp(adjustment_x,adjustment_y);
//	};
//};

//void Cubby::notify_leader(boolean smaller) {
//	record_offsets();
//	// don't really understand why this works
//	leader->recursively_propagate_changes();
//   if (smaller) {
//      // shrinking so re-grasp
//      changed_so_update();
//      ((Programmer_Sprite *) leader)->adjust_tool_in_hand();
//		record_offsets();
//	};
//};

void Cubby::inside_thought_bubble() {
	for (int i = 0; i < number_of_holes; i++) { // removed (cubby_index) coerecion on 070203
		if (contents[i] != NULL) {
			contents[i]->inside_thought_bubble();
		};
	};
};

void Cubby::now_on_back_of(Picture *picture) {
	for (int i = 0; i < number_of_holes; i++) { // removed (cubby_index) coerecion on 070203
		if (contents[i] != NULL && !(contents[i]->is_flipped() && contents[i]->pointer_to_controlee() == NULL)) { 
         // flipped pictures in a box on the back of a picture aren't really on the back of a picture (new on 150201)
         // checked that it is not a remote looks on 281002
			contents[i]->now_on_back_of(picture);
		};
	};
};

void Cubby::cubby_insides(Sprite *item,
								  city_coordinate &insides_llx,
								  city_coordinate &insides_lly,
								  city_coordinate &insides_width,
								  city_coordinate &insides_height) {
	Sprite *real_item = item;
	while (real_item->pointer_to_leader() != this) {
	   real_item = real_item->pointer_to_leader();
	};
	for (int i = 0; i < number_of_holes; i++) { // removed (cubby_index) coerecion on 070203
		if (contents[i] == real_item) {
			cubby_insides_of_index(i,insides_llx,insides_lly,insides_width,insides_height);
			return;
		};
	};
	// use 0 index so reasonable values are returned at least
	cubby_insides_of_index(0,insides_llx,insides_lly,insides_width,insides_height);
//	if (tt_user_is_debugger) {
#if TT_DEBUG_ON
//		perform_integrity_checks(this);
		say_error(IDS_UNEXPECTED_NOT_IN_BOX);
#endif
//		tt_error_file() << "This (cubby) is " << this
//						<< "; follower is " << followers->rest()->first() << endl
//						<< "; content is " << contents[1]
//						<< "; content leader is " << contents[1]->pointer_to_leader()
//						<< "; content leader leader is " << contents[1]->pointer_to_leader()->pointer_to_leader() << endl;
//	};
};

void Cubby::cubby_insides_of_index(int hole_index, // removed (cubby_index) coerecion on 070203
											  city_coordinate &insides_llx,
											  city_coordinate &insides_lly,
											  city_coordinate &insides_width,
											  city_coordinate &insides_height) {
	int x_tiles = 5*number_of_holes+1;
	int y_tiles = 4;
//	insides_width = (4*width)/x_tiles;
	insides_width = peg_width*4;
	insides_height = (2*height)/y_tiles;
//	insides_height = peg_height*4;
   if (width > 1000000 && number_of_holes > 200) { // cruder but avoids overflow
      insides_llx = llx+(hole_index*5+1)*(width/x_tiles);
   } else {
	   insides_llx = llx+((hole_index*5+1)*width)/x_tiles;
//		city_coordinate one_x_pixel = tt_screen->one_pixel_in_x_city_units();
//		if (tt_log_version_number > 11 && (width/number_of_holes) > one_x_pixel*10) { // minor correction
//		   insides_llx -= one_x_pixel*hole_index; // so round off can't make a one pixel gap between boxes
//		};
   };
	insides_lly = lly+height/y_tiles;
};

city_coordinate Cubby::hole_center_x(int hole_index) { // removed (cubby_index) coerecion on 070203
	int x_tiles = 5*number_of_holes+1;
	return(llx+((hole_index*5+3)*width)/x_tiles);
};

void Cubby::set_size_and_location(city_coordinate new_width,
											 city_coordinate new_height,
											 city_coordinate new_llx,
											 city_coordinate new_lly,
											 ChangeReason reason) {
	// new on 260804 since each piece can't get too big to display
	if (new_width > width && number_of_holes > 0) { // second condition added 080904
		int64 maximum_total_width = (int64) number_of_holes*maximum_width(code)*256/4; // /4 since this adjusted for screen size relative to 640x480
		if (new_width > maximum_total_width) {
			if (maximum_total_width > max_long) {
				new_width = max_long;
			} else {
				new_width = (city_coordinate) maximum_total_width;
			};
			if (reason != LEADER_CHANGED && showing_on_a_screen()) {
				// added not LEADER_CHANGED condition on 090105 -- probably there are others
				play_sound(PLOP_SOUND);
			};
		};
	};
	Sprite::set_size_and_location(new_width,new_height,new_llx,new_lly,reason);
};

void Cubby::update_piece_widths() {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log("update_piece_widths of debug target");
	};
#endif
	if (number_of_holes == 0) { // new on 201003 since otherwise when it gets more holes it has a width of 1/3 of normal
		peg_width = width/2;
	} else if (number_of_holes <= 1) {
//tt_error_file() << peg_width << " ";
		peg_width = width/x_pegs_per_first_piece;
//tt_error_file() << peg_width << endl;
		return;
	};
	long peg_count = compute_peg_count(number_of_holes); // x_pegs_per_following_piece*number_of_holes+1;
	peg_width = width/peg_count;
//	if (!animation_in_progress()) {
		// to avoid pixel round-off errors from accumulating
//		city_coordinate one_pixel = tt_screen->one_pixel_in_x_city_units();
// seeing if it looks better without this
//		peg_width = tt_screen->horizontal_pixel_length(peg_width)*one_pixel;
//		if (peg_width == 0) peg_width = one_pixel;
//	};
//	first_piece_width = peg_width*x_pegs_per_first_piece;
//	piece_width  = peg_width*x_pegs_per_following_piece;
	// adjust width to be exact pixel length
	if (!animation_in_progress()) {
		if (leader == NULL) {
			width = peg_width*peg_count;
		} else if (leader->kind_of() == PROGRAMMER ||
					  leader->kind_of() == ROBOT_IN_TRAINING ||
					  leader->kind_of() == ROBOT) {
			width = peg_width*peg_count;
			record_offsets();
		};
	};
};

int Cubby::compute_peg_count(int number) { // abstracted on 201003
	if (number == 0) return(2); // not 1 as was the case prior to 201003
	return(x_pegs_per_following_piece*number+1);
};

// needed to be an instance variable but this is a bit wasteful -- try to be clever???
//long last_drop_index = -1; // only used for selection feedback of a drop

void Cubby::display(SelectionFeedback selection_feedback,
						  boolean followers_too, TTRegion *region) {
//	if (rescale_if_camera_above(selection_feedback,followers_too,region)) return; // good idea???? -- commented out on 150202
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Displaying debug target."));
	};
#endif
	if (is_blank()) { // new on 170102 since none of this stuff with pegs and labels applies if blank
		Sprite::display(selection_feedback,followers_too,region);
		return;
	};
//	if (llx+width < 0 || llx > ideal_screen_width) return; // new on 211100
//	if (lly+height < 0 || lly > ideal_screen_height) return; // new on 211100
	if (region == NULL && // replaced the above on 150202 since works outside as well
		 tt_screen->off_screen(llx,lly,width,height)) return; 
	if (pointer_to_top_of_stack() != NULL) { // new on 260500
		pointer_to_top_of_stack()->display(selection_feedback,followers_too,region);
		return;
	};
	if (!visible()) return;
   city_coordinate width_change = labs(width-previous_width);
	if (//(tt_log_version_number < 9 &&
       // (width != previous_width || height != previous_height)) ||
       // following prevents gradually widening due to round-off (via update leaders in tt_screen)
       //tt_log_version_number >= 9 &&
		 (width < tile_width && width_change > 0) || // added  && width_change > 0 on 121099
		 (width_change > tt_screen->one_pixel_in_x_city_units() && width_change*100 > width)) {
//		 (tt_log_version_number > 13 && width_change > tt_screen->one_pixel_in_x_city_units() && width_change*100 > width) ||
//		 (tt_log_version_number <= 13 && width_change >= peg_width)) { 
	// above can lose to round off error unlike the following
//	if (labs(tt_screen->horizontal_pixel_length(width-previous_width)) > 1 ) ||
//		 labs(tt_screen->vertical_pixel_length(height-previous_height)) > 1) {
		// update_piece_widths(); -- commented out on 070705 since is also called by refit_contents below
		if (leader != NULL) {
			leader->really_propagate_changes(); // to avoid round off problems -- still needed??
		};
      refit_contents();
      previous_width = width;
		previous_height = height;
	};
	city_coordinate one_x_pixel = tt_screen->one_pixel_in_x_city_units();
	city_coordinate one_y_pixel = tt_screen->one_pixel_in_y_city_units();
	city_coordinate real_llx, real_lly;
	city_coordinate real_width, real_height;
	city_coordinate real_peg_width;
	unsigned short int real_y_scale;
	if (region == NULL) {
		real_llx = llx;
		real_lly = lly;
		real_width = width;
		real_height = height;
		real_peg_width = peg_width;
		real_y_scale = y_scale;
	} else {
		real_llx = region->min_x;
		real_lly = region->min_y;
		real_width = region->width();
		if (real_width < one_x_pixel) return;
		real_height = region->height();
		if (real_height < one_y_pixel) return;
		real_peg_width = (real_width*peg_width)/width;
		real_y_scale = (unsigned short int) ((real_height*y_scale)/height);
	};
	city_scale scale = tt_screen->current_scale();
	real_y_scale = (unsigned short int) ((real_y_scale*ground_scale)/scale); 
	if (real_y_scale == 0) return; // new on 221105 -- can happen when flying high enough with boxes on the ground
	// above new on 150202 so that height of boxes adjusts as the helicopter moves up
// not only is this extra work it makes the cubby DIRTY
//	update_piece_widths(); // really only need to call when width changes
	city_coordinate peg_height = real_height/y_pegs_per_piece;
	if (want_feedback(selection_feedback)) {
		selection_feedback = MOVEMENT_FEEDBACK;
	};
	// hard to grow cubbies a little because each part has
	// small integer dimensions
	// so just move cubby for selection instead
   city_coordinate ten_x_pixels = ideal_horizontal_units(10); //one_x_pixel*10; -- 
	// why isn't it better to use one_x_pixel*10 since that will work when not on the floor?
	city_coordinate ten_y_pixels = ideal_vertical_units(10);
	if (blank) {
		followers_too = FALSE;
	} else if (width < ten_x_pixels || height < ten_y_pixels || peg_width <= 10) { // new 171200 so if very tiny don't display parts
		// added peg_width <= 10 on 240102 since could have many holes
		followers_too = FALSE;
	} else if (tt_log_version_number < 51) { // commented out on 160804 -- restored for old logs on 080904
	//} else { // if (region != NULL || !(tt_screen->camera_status() == CAMERA_ABOVE && adjust_height_if_camera_above())) {
		// conditions added on 160804 so that when seen from helicopter this doesn't display twice -- and then removed the same day
		followers_too = TRUE; // new on 191200 since caching followers doesn't work well for cubbies so it is disabled
	};
	boolean feedback_wanted = want_feedback(selection_feedback);
   SelectionFeedback followers_selection_feedback;
	if (followers_too && followers != NULL &&
		 feedback_wanted && last_drop_index != -1) {
		followers_selection_feedback = NO_FEEDBACK;
	} else {
		followers_selection_feedback = selection_feedback;
	};
	bytes permutation = color_permutation(selection_feedback);
	if (number_of_holes <= 1 && !blank) {
		if (number_of_holes == 0) {
			scale_to_fit();
			x_scale *= (unsigned short int) 3; // since only 2 pegs wide not 6
		};
		Sprite::display(selection_feedback,FALSE,region); // followers_too covered at end
		if (number_of_holes == 1) {
			if (!blank && followers_too && contents[0] != NULL) {
				contents[0]->display_as_a_follower(followers_selection_feedback,region);
			};
			if (labels != NULL || is_main_cubby_in_body()) {
				// is_main_cubby_in_body() new on 160499
				city_coordinate delta_x = 0;
				city_coordinate delta_y = 0;			
				if (want_feedback(selection_feedback)) {
					delta_x = selection_delta_x();
					delta_y = selection_delta_y();
				};
				if (!blank && show_all()) {
					display_label(0,real_llx+real_peg_width+delta_x,
									  real_lly+peg_height+delta_y,
									  real_peg_width,peg_height,permutation);
				};
				if (is_main_cubby_in_body()) {
					display_the_label(S(IDS_MY_BOX),
											real_llx+real_peg_width+delta_x,
											real_lly+4*peg_height+delta_y,
											real_peg_width,peg_height,tt_white,permutation);
				};
			};
		};
	} else {
#if TT_DEBUG_ON
		millisecond start_time = timeGetTime();	
#endif
		city_coordinate piece_llx = real_llx;
		if (animation_in_progress()) {
			peg_width = real_width/(x_pegs_per_following_piece*number_of_holes+1);
		};
		city_coordinate piece_width = real_peg_width*x_pegs_per_first_piece;
		long piece_pixel_width, piece_pixel_height; // height not used but needed for calls to display_cubby_hole
		if (piece_width == 0) piece_width = 1; // can't divide by zero
		// make the pieces a few pixels too wide to avoid gaps
		int pixel_extra;
//		if (tt_log_version_number > 12) { // need one more pixel typically but just one if really small
         if (piece_width > ten_x_pixels) {
		      pixel_extra = 3;
				if (tt_screen_width > tt_graphics_video_mode_width) { // added on 181199 since gaps occur with higher resolution
					// revised on 241199 since it looked bad on small boxes but I don't really understand the logic here
//					pixel_extra += piece_width/(one_x_pixel*30); // new on 240102 -- but why? and it doesn't work in general
					if (piece_width > one_x_pixel*100) pixel_extra++;
				};
			} else if (piece_width > one_x_pixel*3) {
            pixel_extra = 2;
			} else {
				pixel_extra = 0; // was 1 prior to 240102
			};
//		} else { // old behavior was always 2 pixels
//			pixel_extra = 2;
//		};
//      unsigned short int piece_x_scale 
			// = (unsigned short) (((piece_width+ideal_horizontal_units(pixel_extra))*256)/maximum_width(code));
//		piece_x_scale = (unsigned short) grow_width_to_640x480_screen_size(piece_x_scale);
		// re-written on 240102 to deal better with round off errors
		// added (int64) on 250804 below and that fixed a bug when the value is very large
		int64 x_scale_times_max_width = 
			   grow_width_to_640x480_screen_size((piece_width+ideal_horizontal_units(pixel_extra)))*(int64) 256; 
		// changed to int64 on 050202
		int max_width = maximum_width(code);
		int piece_x_scale_error = (int) x_scale_times_max_width%max_width;
		int accumulated_piece_x_scale_error = 0;
		int64 piece_x_scale_as_long = x_scale_times_max_width/max_width; // new on 230804
		unsigned short int piece_x_scale;
		if (piece_x_scale_as_long > max_unsigned_short) {
			piece_x_scale = max_unsigned_short; // new on 230804 to deal with enormous boxes
		} else {
			piece_x_scale = (unsigned short int) piece_x_scale_as_long;
		};
//		if (piece_x_scale == 0) piece_x_scale = 1; // new on 210102 - can easily happen with boxes with hundreds of holes
		if (piece_x_scale < 3 || real_y_scale < 3) { // no point trying to show followers too - new on 210102
			followers_too = FALSE;
		};
		city_coordinate adjusted_lly = real_lly;
		if (feedback_wanted && last_drop_index < 0) { // no hole selected
			// integer pixel movements to avoid roundoff quirks
			piece_llx += selection_delta_x();
			adjusted_lly += selection_delta_y();
		};
		int index = 0; // removed (cubby_index) coerecion on 070203
		city_coordinate extra_delta_x, extra_delta_y;
		if (show_all()) {
//			boolean display_part = (!feedback_wanted || // not a feedback display
//											last_drop_index < 0 || // no hole selected
//											last_drop_index == index); // it was this one
         if (last_drop_index == index) {
			   extra_delta_x = selection_delta_x(); // new on 181199 so labels and "My box" wiggle too
				extra_delta_y = selection_delta_y();
				permutation = color_permutation(selection_feedback);
         } else {
            extra_delta_x = 0;
				extra_delta_y = 0;
				if (last_drop_index >= 0) permutation = NULL; // not selecting the whole thing
         };
			tt_screen->move_mark_to(piece_llx+extra_delta_x,adjusted_lly+extra_delta_y);
			boolean displayed_hole = FALSE;
			if (!blank) {
				displayed_hole = display_cubby_hole(labels==NULL?ONE_CUBBY_IMAGE:ONE_CUBBY_LABELED_IMAGE,
					                                 piece_x_scale,real_y_scale,permutation,
																piece_pixel_width,piece_pixel_height);
				if (displayed_hole) { // conditional new on 011100 - no sense displaying contents if off screen or too small
					if (contents[index] != NULL && followers_too) { // followers_too added 261100
//						&& clean()) { // second conjunct added on 261100 since if dirty need to update location and size
						contents[index]->display_as_a_follower(followers_selection_feedback,region);
					};
				};
			} else {
				display_cubby_hole(BLANK_CUBBY_IMAGE,
										 piece_x_scale,real_y_scale,permutation,piece_pixel_width,piece_pixel_height);
			};
//			piece_width = piece_pixel_width*one_x_pixel; 
			// above new on 240102 to get an accurate value so for small boxes with lots of holes round off 
			// doesn't make it look terrible
			if (displayed_hole && labels != NULL && labels[0] != NULL && !blank) { // && display_part) {
				display_label(0,piece_llx+real_peg_width+extra_delta_x,adjusted_lly+peg_height+extra_delta_y,
								  real_peg_width,peg_height,permutation);
			};
			if (is_main_cubby_in_body()) {
				display_the_label(S(IDS_MY_BOX),
										piece_llx+real_peg_width+extra_delta_x,adjusted_lly+4*peg_height+extra_delta_y,
										real_peg_width,peg_height,tt_white,permutation);
			};
			piece_llx += piece_width;
			accumulated_piece_x_scale_error += piece_x_scale_error; // new on 240102
//         if (tt_log_version_number == 12 && piece_width > ten_x_pixels) {
				// after version 12 made them wider as well
//            piece_llx -= one_x_pixel; // so round off can't make a one pixel gap between boxes
//			};
			piece_width = real_peg_width*x_pegs_per_following_piece; 
			// above commented out on 240102 since now compute more accurate version while displaying hole
			//if (tt_log_version_number >= 51) { 
			// new on 230804 to avoid ugly gaps -- got rid of the gaps but the whole box looked bad as you fly up
			//	piece_width -= (one_x_pixel*scale)/ground_scale;
			//};
			if (piece_width == 0) piece_width = 1; // musn't divide by zero
			index = 1;
			if (piece_llx < 0) {
				index -= piece_llx/piece_width; // removed (cubby_index) coerecion on 070203
				piece_llx %= piece_width;
			};
			for (; index < number_of_holes; index++) {
				// commented out the following on 011100 since it wasn't right 
				// and now display_sprite is optimized for skipping off screen work
//				 if (piece_llx > tt_screen->max_x()) break; // was > ideal_screen_width
//				 display_part = (!feedback_wanted ||
//									  last_drop_index < 0 ||
//									  last_drop_index == index);
	// subtract a pixel to avoid pixel wide gaps between pieces
	//			piece_llx -= screen->one_pixel_in_x_city_units();
				if (blank) piece_llx -= real_peg_width;
				if (last_drop_index == index) {
					extra_delta_x = selection_delta_x(); // new on 181199 so labels and "My box" wiggle too
					extra_delta_y = selection_delta_y();
					permutation = color_permutation(selection_feedback);
				} else {
					extra_delta_x = 0;
					extra_delta_y = 0;
					if (last_drop_index >= 0) permutation = NULL;
				};
		      tt_screen->move_mark_to(piece_llx+extra_delta_x,adjusted_lly+extra_delta_y);
				boolean displayed_hole = FALSE;
				int accumulated_error = 0;
				if (accumulated_piece_x_scale_error > max_width) {
					accumulated_error = accumulated_piece_x_scale_error/max_width;
					accumulated_piece_x_scale_error %= max_width;
				};
				if (!blank) {
					displayed_hole = display_cubby_hole(labels==NULL?REST_CUBBY_IMAGE:REST_CUBBY_LABELED_IMAGE,
																	piece_x_scale+accumulated_error,real_y_scale,permutation,
																	piece_pixel_width,piece_pixel_height);
					if (displayed_hole && contents[index] != NULL && followers_too) {
						contents[index]->display_as_a_follower(followers_selection_feedback,region);
					};
				} else {
 					display_cubby_hole(BLANK_CUBBY_IMAGE,piece_x_scale+accumulated_error,real_y_scale,permutation,
						                piece_pixel_width,piece_pixel_height);
				};
				if (!followers_too && region == NULL) { 
					// conditional added on 250102 since dealing with round off 
					// like this will put the contents in the wrong (original calculated) place
					// added second condition on 070705 since otherwise goals in puzzle game are distorted
					piece_width = piece_pixel_width*one_x_pixel; 
					// new on 240102 to get an accurate value so 
					// for small boxes with lots of holes round off doesn't make it look terrible
				};
				if (displayed_hole && labels != NULL && labels[index] != NULL && !blank) { // && display_part) {
					display_label(index,piece_llx+extra_delta_x,adjusted_lly+peg_height+extra_delta_y,
									  real_peg_width,peg_height,permutation);
				};
				// commented out the following since index is always greater than 0
//				if (index == 0 && is_main_cubby_in_body()) {
//					display_the_label(S(IDS_MY_BOX),
//											piece_llx,adjusted_lly+4*peg_height+extra_delta_x,
//											real_peg_width,peg_height+extra_delta_y,
//											tt_white);
//				};
				if (piece_x_scale+accumulated_error > 0) { // if zero then don't move llx
					piece_llx += piece_width;
				};
				accumulated_piece_x_scale_error += piece_x_scale_error; // new on 240102
//            if (tt_log_version_number == 12 && piece_width > ten_x_pixels) {
//               piece_llx -= one_x_pixel; // so round off can't make a one pixel gap between boxes
//            };
				// if piece_llx is to right of screen return
			};
#if TT_DEBUG_ON
			if (tt_debug_mode == 3) {
				tt_error_file() << (timeGetTime()-start_time) << " ms to display -- CUBBY of size "	
									 << number_of_holes << endl;
			};
#endif
		};
	};
	last_drop_index = -1; // must be re-selected for drop feedback
};

void Cubby::display_label(int index, // removed (cubby_index) coerecion on 070203
								  city_coordinate character_llx,
								  city_coordinate character_ury,
								  city_coordinate peg_width,
								  city_coordinate peg_height,
								  bytes color_permutation) {
	if (labels == NULL || labels[index] == NULL) return;
	display_the_label(labels[index],character_llx,character_ury,peg_width,peg_height,tt_black,color_permutation);
};

void Cubby::display_the_label(const_string label,
										city_coordinate character_llx,
										city_coordinate character_ury,
										city_coordinate peg_width,
										city_coordinate peg_height,
										color_index text_color,
										bytes color_permutation) {
	const city_coordinate one_pixel_x = tt_screen->one_pixel_in_x_city_units();
	if (peg_width < 6*one_pixel_x) { // new on 210800
		return;
	};
	const city_coordinate one_pixel_y = tt_screen->one_pixel_in_y_city_units();
	if (peg_height < 6*one_pixel_y) {
		return;
	};
//	if (need_wide_characters()) { // added on 200800, for Japanese, e.g. -- commented out on 130702 so labels on boxes can comes from Japan etc.
		wide_string wide_text = copy_wide_string(label);
		display_the_label(wide_text,character_llx,character_ury,peg_width,peg_height,text_color,color_permutation);
		delete [] wide_text;
		return;
//	};
};
      /* commented out on 130702
	// don't know why increasing character width by 50% works here
	// maybe I should ask Windows how long the label will be first
	int number_of_characters = _tcslen(label);
   if (number_of_characters == 0) return;
	long label_length = number_of_characters;
	if (number_of_characters < 8) number_of_characters = 8;
   city_coordinate label_width = 4*peg_width; //(7*peg_width)/2; // leave a little room on right
	city_coordinate original_character_width = label_width/number_of_characters;  //(6*peg_width)/number_of_characters;
//	city_coordinate minimum_character_width = one_pixel_x*5;  // was 4
	if (original_character_width < minimum_character_width() && label_length < number_of_characters) {
		// leaving enough room for 8 characters won't work here so
		original_character_width = label_width/label_length; // new on 290699
	};
	city_coordinate character_width = original_character_width;
	city_coordinate character_height = peg_height; // was (3*peg_height)/2 prior to 210800
//	if (character_width < one_pixel_x) { // commented out on 210800 since peg_height does this just fine
//		return; // just too small -- on 140799 changed from 1* to 3* -- restored *1 on 210800
//	};
BEGIN_GDI
//	tt_screen->set_font(character_width,character_height,TRUE,FALSE);
	city_coordinate extent = tt_screen->get_extent(label,label_length,
                                                  original_character_width,character_height,TRUE,FALSE); // FALSE added on 290699 so it fits better
	if (label_length > 6 || extent > label_width) {
		// revised on 010699 to look nicer
		city_coordinate real_character_width = min(extent,label_width)/label_length;
		city_coordinate min_character_width;
		if (need_wide_characters()) {
			min_character_width = minimum_character_width()*3; // triple new on 210800
		} else {
			min_character_width = minimum_character_width(); // new on 080900
		};
		if (real_character_width < min_character_width && extent > label_width) { // too narrow so truncate
			// new on 290699
			int new_label_length = label_length-1;
			while (tt_screen->get_extent(label,new_label_length,min_character_width,character_height,TRUE,FALSE) > label_width) {
				new_label_length--;
				if (new_label_length == 0) return; // too small
			};
//     		int new_label_length = label_width/minimum_character_width; 
//			if (new_label_length < label_length) {
				label_length = new_label_length;
     			character_width = min_character_width;
//			};
		} else {
			character_width = (label_width*original_character_width)/extent;
			city_coordinate new_extent = tt_screen->get_extent(label,label_length,character_width,character_height,TRUE,FALSE);
			if (new_extent > label_width) {
				character_width = (label_width*character_width)/new_extent; // was original_character_width prior to 210800
				new_extent = tt_screen->get_extent(label,label_length,character_width,character_height,TRUE,FALSE); // new on 140201
			};
			if (original_character_width < min_character_width) { // was just character_width prior to 210201 
				// was else if prior to 140201 and added new min_character_width test
//				if (label_width/8 > min_character_width) { // can truncated (new on 210201)
//					label_length = 8;
//					character_width = label_width/8;
//				} else {
END_GDI
					return;
//				};
			} else if (new_extent > label_width) { // new on 210101 to ignore those labels that are too long and small - updated 220101 to use label_width rather than peg_width
				// rather than give up better to truncate and use original character width - as of 210201							
				label_length = min(label_length,label_width/original_character_width); 
				character_width = original_character_width;
			};
		};
	};
//		character_width = (label_width*original_character_width)/extent;
//      extent = tt_screen->get_extent(labels[index],label_length,
//                                     character_width,character_height,TRUE);
//   tt_main_window->deselect_font(); // may have recomputed its width
//	if (label_length <= 5) {
//		character_width = (3*character_width)/2;
//	};
	tt_screen->text_out(label,label_length,
							  character_llx,character_ury,//+peg_height/2,
							  character_width,character_height,TRUE,FALSE,TRUE,text_color,FALSE,color_permutation);
//   tt_main_window->deselect_font();
END_GDI
};
*/
// implemented a wide variant on 200800
void Cubby::display_the_label(wide_string label,
										city_coordinate character_llx,
										city_coordinate character_ury,
										city_coordinate peg_width,
										city_coordinate peg_height,
										color_index text_color,
										bytes color_permutation) {
	int number_of_characters = wide_string_length(label);
   if (number_of_characters == 0) return;
	long label_length = number_of_characters;
	if (number_of_characters < 8) number_of_characters = 8;
   city_coordinate label_width = 4*peg_width; 
	const city_coordinate one_pixel_x = tt_screen->one_pixel_in_x_city_units();
	const city_coordinate one_pixel_y = tt_screen->one_pixel_in_y_city_units();
	city_coordinate original_character_width = label_width/number_of_characters;  
//	city_coordinate minimum_character_width = one_pixel_x*5;  // was 4
	if (original_character_width < minimum_character_width() && label_length < number_of_characters) {
		// leaving enough room for 8 characters won't work here so
		original_character_width = label_width/label_length; // new on 290699
	};
	city_coordinate character_width = original_character_width;
	if (peg_height < 6*one_pixel_y) return;
	city_coordinate character_height = peg_height; // was (3*peg_height)/2 prior to 210800 but that led to descenders off the box
	if (character_width < 3*one_pixel_x) return; // just too small -- on 140799 changed from 1* to 3*
BEGIN_GDI
//	tt_screen->set_font(character_width,character_height,TRUE,FALSE);
	city_coordinate extent = tt_screen->get_extent(label,label_length,
                                                  original_character_width,character_height,TRUE,FALSE,FALSE); // FALSE added on 290699 so it fits better
	if (label_length > 6 && extent > label_width) { // was || prior to 180702
		// revised on 010699 to look nicer
		city_coordinate real_character_width = min(extent,label_width)/label_length;
		city_coordinate min_character_width = minimum_character_width()*4; // quadruple for wide characters (new on 210800)
		if (real_character_width < min_character_width) { 
			// new on 290699
			int new_label_length = label_length-1;
			while (tt_screen->get_extent(label,new_label_length,min_character_width,character_height,TRUE,FALSE,FALSE) > label_width) {
				new_label_length--;
				if (new_label_length == 0) return; // too small
			};
//     		int new_label_length = label_width/minimum_character_width; 
//			if (new_label_length < label_length) {
				label_length = new_label_length;
     			character_width = min_character_width;
//			};
		} else {
			character_width = (label_width*original_character_width)/extent;
			city_coordinate new_extent = tt_screen->get_extent(label,label_length,character_width,character_height,TRUE,FALSE,FALSE);
			if (new_extent > label_width) {
				character_width = (label_width*character_width)/new_extent; // was original_character_width prior to 210800
			};
		};
	};
//		character_width = (label_width*original_character_width)/extent;
//      extent = tt_screen->get_extent(labels[index],label_length,
//                                     character_width,character_height,TRUE);
//   tt_main_window->deselect_font(); // may have recomputed its width
//	if (label_length <= 5) {
//		character_width = (3*character_width)/2;
//	};
	tt_screen->text_out((string) label,label_length,
							  character_llx,character_ury,//+peg_height/2,
							  character_width,character_height,TRUE,FALSE,TRUE,text_color,TRUE,color_permutation);
//   tt_main_window->deselect_font();
END_GDI
};

Sprite *Cubby::released(Sprite *by, boolean top_level, SpecialDrop special_drop) {
	// let contents (recursively know) about release (e.g., so birds can hatch)
	Sprite *released_item = Sprite::released(by,top_level,special_drop);
	for (int i = 0; i < number_of_holes; i++) { // removed (cubby_index) coerecion on 070203
		if (contents[i] != NULL) {
			contents[i]->released(by,FALSE);
		};
	};
// bird may be waiting for cubby's birds to return
//	busy_counter = 0; // if something was waiting for it...
	return(released_item);
};

void Cubby::set_priority(long new_priority) {
	Sprite::set_priority(new_priority);
	for (int i = 0; i < number_of_holes; i++) {
		if (contents[i] != NULL) {
			contents[i]->set_priority(new_priority-1);
		};
	};
};

Sprite *Cubby::now_inside_thought_bubble(boolean in_training) {
	if (!is_blank()) { // condition new on 160405 since if erased don't recur down the structure
		for (int i = 0; i < number_of_holes; i++) { // removed (cubby_index) coerecion on 070203
			if (contents[i] != NULL) {
				contents[i] = contents[i]->now_inside_thought_bubble(in_training);
			};
		};
	};
	return(this);
};

Sprite *Cubby::select(TTRegion *region, SelectionReason reason, Sprite *in_hand) {
	if (blank || (reason == MIGHT_GRASP && contained_in(TRUCK))) { // moved here from below on 080904 so works even if 0 holes
		return(this);
	};
	last_drop_index = -1; // unless overridden
	if (reason == MIGHT_DROP_ITEM && leader == NULL && in_hand != NULL && in_hand->kind_of() == CUBBY && tt_log_version_number >= 38) { 
		// is another cubby on the sides -- in_hand->kind_of() == CUBBY moved here so following isn't computed if not holding a cubby
		// maybe some of this could be used below rather than recomputed
		city_coordinate item_center_x,item_center_y; // new on 290204
		in_hand->true_center(item_center_x,item_center_y); // was center of collision_region
		int hole_index = closest_hole(item_center_x,item_center_y);
		if (hole_index < 0 || hole_index == number_of_holes) {
			return(this);
		};
	};
	if (number_of_holes == 0) {
		if (reason == MIGHT_DROP_ITEM && tt_log_version_number >= 38) {
			return(NULL); // new on 290204
		} else {
			return(this);
		};
	};
	if (item_being_sucked_up != NULL && // receiption of an item in progress
		 reason != MIGHT_USE_EXPANDER &&
		 reason == MIGHT_USE_COPIER) {
		return(NULL);
	};
	// if there is something in the cubby see if region is inside
	if (reason == MIGHT_GRASP || reason == MIGHT_DROP_ITEM ||
		 reason == MIGHT_USE_VACUUM || reason == MIGHT_USE_COPIER) {
//		  reason == MIGHT_USE_COPIER_NUMBER || reason == MIGHT_USE_NUMBER) &&
//		 !all_holes_empty()) {
		city_coordinate region_center_x, region_center_y;
		region->region_center(region_center_x, region_center_y);
		// maybe this should be closest_hole_that_can_accept(in_hand) -- idea on 230902
		// note that it isn't simple
		long hole_index = closest_hole(region_center_x, region_center_y);
		if (hole_index < 0) {
			hole_index = 0;
		} else if (hole_index == number_of_holes) {
			hole_index--;
		};
		if (contents[hole_index] == NULL || // empty or
			 (reason == MIGHT_DROP_ITEM && !contents[hole_index]->colliding(in_hand)) ||
			 (reason != MIGHT_DROP_ITEM && !hole_contents_contains(hole_index,region_center_x,region_center_y))) { // removed (cubby_index) coerecion on 070203
//			 (tt_log_version_number >= 13 && reason == MIGHT_DROP_ITEM && !contents[hole_index]->colliding(in_hand)) ||
//			 ((tt_log_version_number < 13 || reason != MIGHT_DROP_ITEM) && !hole_contents_contains((cubby_index) hole_index,region_center_x,region_center_y))) {
			 // closest hole is still not good enough
			 // or really grasping whole cubby
			if (reason == MIGHT_DROP_ITEM) {
            if (cubby_contents_type(in_hand->kind_of())) {
   				if (contents[hole_index] == NULL) {
                  last_drop_index = hole_index;
					} else if (tt_log_version_number >= 38) {
						return(contents[hole_index]->select(region,reason,in_hand)); // new on 290204
					};
               return(this);
			   } else {
               return(NULL);
			   };
         };
			return(this);
		};
		Sprite *selection = contents[hole_index]->select(region,reason,in_hand);
		if (selection == NULL) {
//			record_grasp_or_vacuum(reason,this,selection_status);
			if (reason == MIGHT_DROP_ITEM && tt_log_version_number >= 38) {
				return(NULL); // new on 280204
			} else {
				return(this);
			};
		};
		// if a component has been vacuumed or is a cubby it will record it
		if (reason == MIGHT_GRASP && selection->kind_of() != CUBBY) {
//			floor->record_event(GRASP_ITEM,selection);
			return(selection);
		};
//      record_grasp_or_vacuum(reason,selection);
//		if (selection == contents[hole_index] && // contents selected
//			 (reason == MIGHT_GRASP || reason == MIGHT_USE_VACUUM)) {
//			remove_component(hole_index,reason,showing_on_a_screen());
//		};
		return(selection);
	} else {
		return(this);
	};
};

Sprite *Cubby::selected(SelectionReason reason, Sprite *by) {
//#if TT_DEBUG_ON
//	perform_integrity_checks(this);
//#endif
	if (infinite_stack()) {
		return(Sprite::selected(reason,by));
	};
//	if (by->kind_of() == PROGRAMMER) {
//		busy_counter = 0; // reset counter so it can be given by programmer
		// note that robot in training should never run accross a busy cubby
//	};
	if (reason == VACUUM_USED || reason == GRASPED) { // why here rather than at sprite::selected ??
		if (leader != NULL && leader->kind_of() == PICTURE) {
			removed_item_from_flip_side();
			now_on_back_of(NULL); // of nothing
		};
	};
	if (reason == VACUUM_USED && is_main_cubby_in_body()) { // new on 151204
		floor->need_new_main_cubby();
	};
	return(Sprite::selected(reason,by));
};

void Cubby::remove_selected_element(Sprite *element, SelectionReason reason, Sprite *by, boolean add_to_floor) {
//#if TT_DEBUG_ON
//	perform_integrity_checks(this);
//#endif
	user_did(CUBBY,CUBBY_CONTENTS_REMOVED,by);
	remove_me(element,FALSE,reason,add_to_floor);
};

//Sprite *Cubby::component(int index) {
//   if (contents[index] == NULL && item_being_sucked_up_index == index) {
      // nothing there but soon will be
//      return(item_being_sucked_up);
//   } else {
//      return(contents[index]);
//   };
//};

//Tool *Cubby::dereference_component(int index) {
//  if (index >= number_of_holes) return(NULL);
//  if (contents[index] == NULL) return(NULL);
//  return(contents[index]->dereference()); // if nest become binding
//};

Sprite *Cubby::component(int index) { 
	if (index >= number_of_holes) {
		say_error(IDS_BOX_DOESNT_HAVE_EXPECTED_HOLE);
		return(NULL);
	} else {
		return(contents[index]);
	};
};

SpritePointerPointer Cubby::pointer_to_component(int index) {
	if (index >= number_of_holes) {
		say_error(IDS_BOX_DOESNT_HAVE_EXPECTED_HOLE);
		return(NULL);
   } else {
      return(&(contents[index]));
	};
};

long Cubby::my_index(Sprite *item) {
	for (int i = 0; i < number_of_holes; i++) {
		if (contents[i] == item) {
			return(i);
		};
	};
	return(-1);
};

void Cubby::remove_me(Sprite *item, boolean will_return, SelectionReason reason, boolean add_to_floor) {
	if (will_return && !tt_dump_as_xml) { // new on 160799 so that birds and the like return before being dumped
		// added tt_dump_as_xml on 121202 since not needed
		insert_at_end_if_not_member(item,returning_items);
	};
	for (int i = 0; i < number_of_holes; i++) {
		if (contents[i] == item) {
			remove_component(i,reason,default_duration(),add_to_floor);
			return;
		};
	};
	say_error(IDS_REMOVE_FROM_BOX_BUT_NOT_THERE);
};

void Cubby::remove_component(int hole_index, SelectionReason reason,
									  millisecond duration, boolean add_to_floor) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("remove_component of debug target.");
	};
#endif
	if (hole_index >= number_of_holes) {
		say_error(IDS_REMOVE_FROM_BOX_BUT_NOT_THERE);
		return;
	};
//	tt_error_file() << "Removing " << contents[hole_index] << " from hole " << hole_index
//					<< " of " << this << " on frame " << tt_frame_number << endl; 
	remove_follower(contents[hole_index]);
//	contents[hole_index]->un_absorb_followers(); // especially for pictures
	if (add_to_floor) {
		if (floor != NULL) {
			floor->add_item(contents[hole_index],
								 TRUE,FALSE,FALSE); // args new on 311003 since can already be there (at least while replaying time travel logs)
		};
		if (reason != VACUUM_USED && (showing_on_a_screen() || contents[hole_index]->kind_of() == PICTURE || tt_log_version_number < 67)) {
			// second condition added on 120105 since the following can be very costly if for example a large multi-lined text pad
			// added Picture condition on 020205 since they may need to keep saved_size etc.
			contents[hole_index]->animate_to_good_size(duration,tt_log_version_number >= 64?tt_toolbox:NULL);
	//		do_after(duration,contents[hole_index],set_size_constrained_false_callback);
	//	} else {
	//		contents[hole_index]->set_size_constrained(FALSE);
		};
	};
	contents[hole_index] = NULL;
	set_clean_status(DIRTY);
};

Sprite *Cubby::remove_component_directly(int hole_index) {
	// used by primitives
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("remove_component_directly of debug target.");
	};
#endif
	Sprite *component = contents[hole_index];
	if (component != NULL) {
		remove_follower(component);
		contents[hole_index] = NULL;
	};
	return(component);
};

boolean Cubby::to_windows(Pointer *data, short int &data_offset,
                          ascii_string types, short int &types_offset,
                          BirdPointer *birds, short int &bird_count) {
   // updates data and types
   // returns FALSE if encounters an empty nest
   types[types_offset] = '[';
   types_offset++;
   if (types_offset > to_windows_types_max_length) { // truncate
      //say_error(IDS_TOO_MUCH_TO_PASS_TO_WINDOWS,TRUE);
		say(IDS_TOO_MUCH_TO_PASS_TO_WINDOWS,3,TRUE); // rewritten on 070305 since may be user error
      return(TRUE);
   };
   for (int i = 0; i < number_of_holes; i++) {  
      if (contents[i] == NULL) {
         types[types_offset] = '-'; // token for nothing there
         types_offset++;
         data_offset++; // skip over this
      } else if (!contents[i]->to_windows(data,data_offset,types,types_offset,birds,bird_count)) {
         return(FALSE);
      };
      if (data_offset > to_windows_data_max_length || types_offset > to_windows_types_max_length || bird_count > to_windows_birds_max_length) { // truncate
         //say_error(IDS_TOO_MUCH_TO_PASS_TO_WINDOWS,TRUE);
			say(IDS_TOO_MUCH_TO_PASS_TO_WINDOWS,3,TRUE); // rewritten on 070305 since may be user error
         return(TRUE);
      };
   };
   types[types_offset] = ']';
   types_offset++;
   return(TRUE);
};

boolean Cubby::current_value(NumberValuePointer &the_value, boolean &shared, Sprite *by) { // used to get the arity -- updated on 300102
	if (blank) return(FALSE);
//	the_value.set_value(number_of_holes);
	the_value = new LongNumberValue(number_of_holes);
	shared = FALSE;
	return(TRUE);
};

boolean Cubby::all_holes_empty() {
   for (int i = 0; i < number_of_holes; i++) {
     if (contents[i] != NULL) return(FALSE);
   };
   return(TRUE);
};

boolean Cubby::blankable() {
	return(!blank); // && all_holes_empty()); // could have empty or blank??
};

void Cubby::fit_into_hole(int index, Sprite *item, boolean initial) {
	city_coordinate item_llx,item_lly,item_width,item_height;
	cubby_insides_of_index(index,item_llx,item_lly,item_width,item_height);
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
//	Sprite *dereferenced_item = NULL;
//	Sprite *original_item = item;
//	if (item->kind_of() != PICTURE && item->kind_of() != REMOTE_PICTURE) {
		// picture dereference is semantic not geometric
//		item = item->dereference();
//	};
//	if (dereferenced_item != NULL && dereferenced_item != item) {
//		dereferenced_item->set_size_and_location(item_width,item_height,item_llx,item_lly);
//	};
//	if (//!initial ||
//		 item->kind_of() != NEST || item->pointer_to_followers() == NULL) {
		// if nest is covered don't adjust things
//		item->adjust_size_and_location(item_width,item_height,item_llx,item_lly);
//	};
	if (item->kind_of() == TEXT) { // new on 151100 to display empty text in boxes differently from erased text
		string text;
		long length;
		if (item->current_text(text,length) && length == 0) {
			item_width = item_width/3;
			item_llx += item_width; // center it
		};
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
	} else if (!item->is_blank() && item->kind_of() == CUBBY && ((Cubby *) item)->current_number_of_holes() == 0) { // added is_blank test on 170102
		// new on 151100 to make it easier to distinguish 0-hole boxes from erased boxes
		item_width = item_width/2;
		item_llx += item_width/2; // center it
	};
//	if (initial || item->kind_of() != NEST) {
		// if refiting leave it alone if nest
		item->set_true_size_and_location(item_width,item_height,item_llx,item_lly,TO_FIT_INSIDE); // TO_FIT_INSIDE added 090102
		if (item_llx+item_width < 0 || item_llx > ideal_screen_width) return; 
		// moved here on 261100 where it had been moved twice before - need to update location for off screen followers so do this late now
      if (initial) { // conditional new on 151100
			item->refit_contents();
		};
//	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
};

void Cubby::signal_size_and_location() {
//	if (tt_log_version_number < 15) return; 
	Sprite::signal_size_and_location();
	refit_contents();
};

void Cubby::notice_on_screen_change(boolean on_screen) {
	if (on_screen) refit_contents(); // might not have been computed well off screen
};

void Cubby::really_propagate_changes(ChangeReason reason) {
   if (//tt_log_version_number > 12 || 
		 (tt_log_version_number == 67?(width > tile_width/2 && height > tile_height/2):(width > tile_width/5 && height > tile_height/5)) && 
		  // prior to 110105 was /5 but that meant that often when flipping a picture that has embedded boxes ran this
		  // changed to == 67 for >= 67 on 160105 since it looked bad sometimes... Must be a better way to get quality without cost here
		 (leader == NULL || leader->kind_of() != PROGRAM_PAD) &&
		 reason != TEMPORARY_FOR_DUMPING && reason != JUST_MOVED && reason != UPDATING) { 
			 // TEMPORARY_FOR_DUMPING new conjunct on 051100; JUST_MOVED added on 080102; UPDATING added 311004 (to avoid lots of unnecessary work)
		for (int i = 0; i < number_of_holes; i++) { 
			if (contents[i] != NULL) {
				fit_into_hole(i,contents[i],FALSE); // re-wrote the following as this on 151100
//				city_coordinate item_llx,item_lly,item_width,item_height;
//	         cubby_insides_of_index(i,item_llx,item_lly,item_width,item_height);
//            contents[i]->set_true_size_and_location(item_width,item_height,item_llx,item_lly,reason);
			};
		};
		if (!followers_completely_inside()) { // e.g. follower because mouse is about to smash 
			Sprites *remaining = followers;
			while (remaining != NULL) {
				Sprite *follower = remaining->first();
				if (!follower->completely_inside_leader()) {
					follower->follow_leader_changes(width,height,llx,lly,reason);
				};
				remaining = remaining->rest();
			};
		};
	} else {
      Sprite::really_propagate_changes(reason);
   };
};

// following was faster but looked bad
/*
	if (width > tile_width/5 && height > tile_height/5) {
      // don't bother if very small
		update_piece_widths();
		// just got to new size and round-off error may have made it look bad
		for (int i = 0; i < number_of_holes; i++) {
			if (contents[i] != NULL) { // just low-level set the coordinates
            city_coordinate item_llx,item_lly,item_width,item_height;
	         cubby_insides_of_index((cubby_index) i,item_llx,item_lly,item_width,item_height);
            if (item_llx+item_width >= 0 && item_llx <= ideal_screen_width) {
               contents[i]->just_set_size_and_location(item_width,item_height,item_llx,item_lly);
            };
			};
		};
	};
*/

void Cubby::refit_contents() {
	update_piece_widths(); // moved out of conditional on 261199 since should be updated even if small
	if (width > tile_width/5 && height > tile_height/5) {
      // don't bother if very small
		// just got to new size and round-off error may have made it look bad
		for (int i = 0; i < number_of_holes; i++) {
			if (contents[i] != NULL) {
				fit_into_hole(i,contents[i],FALSE);
			};
		};
	};
};

long Cubby::closest_hole(city_coordinate x, city_coordinate ) {
	// returns -1 if left of 0th hole
	// returns number_of_holes if right of last hole
	// otherwise index of closest hole
	if (number_of_holes == 0) return(-1);
	// ignoring y for now -- one dimensional
	city_coordinate distance_to_left_side = x-llx;
	if (distance_to_left_side < 0) return(-1); // first one if left of it
	if (distance_to_left_side >= width) return(number_of_holes); // past last one
 #if TT_DEBUG_ON
	if (tt_debug_mode == 220803) {
		tt_error_file() << "Cubby's llx is " << llx << " and center of selection region is " << x << endl;
	};
#endif
#if TT_MICROSOFT
	// new on 011100 for accuracy (e.g. 1000 plus holes)
	return((long) ((((int64) distance_to_left_side)*number_of_holes)/width));
#else
	return((distance_to_left_side*number_of_holes)/width);
#endif
//	return((((float) distance_to_left_side)/width)*number_of_holes);
};

void Cubby::hole_region(int hole_index, TTRegion &region) { // abstracted 230804
	city_coordinate hole_width,hole_height;
	cubby_insides_of_index(hole_index,
					           region.min_x,region.min_y,hole_width,hole_height);
	region.max_x = region.min_x+hole_width;
	region.max_y = region.min_y+hole_height;
};

boolean Cubby::hole_contents_contains(int hole_index, 
												  city_coordinate center_x, 
												  city_coordinate center_y) { 
	TTRegion region;
//	contents[hole_index]->collision_region(contents_region);
	hole_region(hole_index,region);
	return(region.inside(center_x,center_y));
};

Path *Cubby::path_to(Sprite *target, boolean &ok) {
	for (int i = 0; i < number_of_holes; i++) {
		ok = TRUE; // unless proven otherwise
		if (target == contents[i] ||
			 (contents[i] == NULL &&
			  i == item_being_sucked_up_index &&
			  item_being_sucked_up == target) ||
			 (contents[i] != NULL && contents[i]->kind_of() == NEST && // or target is on a nest
			  contents[i]->dereference() == target)) {
			return(new Path(i));
		};
		// commented out on 091000 since tt_just_vacuumed_remote does it better
		if (contents[i] != NULL && // contents[i]->kind_of() == REMOTE_PICTURE && // or just vacuumed it off a remote
			 contents[i] == tt_just_vacuumed_remote)  { // prior to 091000 was tt_just_vacuumed == target
			return(new Path(i,new Path(SPECIAL_PATH_MARKER,new Path(contents[i]->kind_of())))); // ,new Path(SPECIAL_PATH_MARKER)
		};
		if (contents[i] != NULL) { // && contents[i]->kind_of() == CUBBY) {
			Path *sub_path = contents[i]->path_to(target,ok);
         if (ok && sub_path != NULL) {
//				if (contents[i]->kind_of() == CUBBY) {
					return(new Path(i,sub_path));
//				} else {
//					sub_path->add_to_end(new Path(SPECIAL_PATH_MARKER));
//					return(new Path(i,new Path(SPECIAL_PATH_MARKER,sub_path));
//				};					
         };
      };
   };
	ok = FALSE;
   return(NULL);
};

MatchStatus Cubby::match(Cubby *other_cubby, SpritePointerPointer &suspension_cell, int version) {
	if (blank) return(MATCH_GOOD);
	// I'm in the thought bubble the other guy is "real"
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter || tt_debug_target == other_cubby->debug_counter) {
		log("About to match debug target.");
	};
#endif
	if (number_of_holes == other_cubby->current_number_of_holes()) {
		for (int i = 0; i < number_of_holes; i++) {
			if (contents[i] != NULL) {
				Sprite *component = contents[i]->dereference();
				Sprite *other_component = other_cubby->component(i);
				Sprite *original_other_component = other_component;
				if (other_component == NULL) {
					suspension_cell = other_cubby->pointer_to_component(i);
					return(MATCH_SUSPENDED_ON_EMPTY_CUBBY_HOLE);
				};
//#if TT_DEBUG_ON
//				if (original_other_component->debug_counter == tt_debug_target) {
//					Integer *n = (Integer *) original_other_component;
//					long val;
//					n->current_value(val);
//					if (val == 1) {
//						log("debug this");
//					};
//				};
//#endif
				SpriteType contents_type = component->kind_of();
//				if (contents_type != NEST) {
				// commented out on 120399 since this interfered with matching empty nests (a bit like unknown(X))
//					other_component = other_component->dereference(); // moved below on 080704
//				};
// dereference now obsoletes the following
//				if (contents_type == NEST) {
//					// might have contents due to history or editting
//					Sprite *nest_contents
//						  = ((Nest *) component)->pointer_to_contents();
//					if (nest_contents != NULL) {
//						component = nest_contents;
//						contents_type = nest_contents->kind_of();
//					}; // otherwise act like an empty nest
//				} else if (contents_type == PICTURE) {
//					Sprite *indirection 
//						  = ((Picture *) component)->pointer_to_indirection();
//					if (indirection != NULL) {
//						component = indirection;
//						contents_type = indirection->kind_of();
//					};
//				};
				SpriteType other_type = other_component->kind_of();
				if (other_type == NEST) {
					Sprite *nest_contents = ((Nest *) other_component)->pointer_to_contents(TRUE); // TRUE arg added on 120203
					if (nest_contents == NULL) {
						if (contents_type != NEST) {
							// suspend until nest receives something
							suspension_cell = other_cubby->pointer_to_component(i);
							//&other_component;
							return(MATCH_SUSPENDED_ON_NEST);
						};
					} else {
						other_component = nest_contents->dereference(); 
						// prior to 080704 was other_component->dereference(); // moved here on 080704
						// put dereference back on 130704
					};
				} else {
					other_component = other_component->dereference(); // copied here on 110704
//					} else { // behave like "binding"
//						other_component = nest_contents;
//						other_type = other_component->kind_of();
//					};
//				} else if (other_type == PICTURE) {
//					Sprite *indirection
//						  = ((Picture *) other_component)->pointer_to_indirection();
//					if (indirection != NULL) {
//						other_component = indirection;
//						other_type = indirection->kind_of();
//					};
				};
				other_type = other_component->kind_of(); // copied here on 080704
				if (other_component != original_other_component && contents_type != other_type && contents_type != NEST &&
					// following added so a flipped text pad or number pad doesn't match an erased picture
					 (other_type == PICTURE || original_other_component->kind_of() != PICTURE)) {
					// new on 060899 so, for example, remote looks of a picture of a number can match a number
					// added contents_type != NEST on 140200 since it stops nests in thought bubbles 
					// from matching any other than empty nests
					other_component = original_other_component;
					other_type = other_component->kind_of();
					if (other_type == REMOTE_PICTURE) { // new on 160101 (restored on 240101)
						Sprite *appearance = ((Remote_Picture *) original_other_component)->pointer_to_appearance();
						if (appearance != NULL) {
							other_component = appearance; // but not dereferenced
							other_type = other_component->kind_of();
						};
					};
				};
				if (contents_type == other_type) {
					// following commented out on 020300 since it does matter since this is used both to suspend
					// robots and to decide after running the body whether to run again on the same cycle
					// besides what below changes tt_touched_a_remote??
//              ChangeFrequency saved_touched_a_remote = tt_touched_a_remote;
              MatchStatus submatch_status = same_type_match(contents_type,component,other_component,suspension_cell,version,i);
				  /* commented out the following on 141100 since it broke nested nest activation
				     and the point of this was to deal with scales with nests better and that is done elsewhere now
				  if (submatch_status == MATCH_SUSPENDED_ON_NEST) { // added 080399 so matching scales with nests works better
					  Sprite **component = other_cubby->pointer_to_component((cubby_index) i);
					  if (component != NULL && *component != NULL &&
						   (*component)->kind_of() == NEST) { // new on 080799
//						  	suspension_cell = component; // commented out since is already bound by same_type_match above
					  } else {
					  	   // really should check that it is a scale
						  suspension_cell = other_cubby->pointer_to_component((cubby_index) (i+1)); 
						  // +1 because that is where the nest is
							// and why can't the nest be on the other side of the scale (i.e. -1)?
					  };
				  };
				  */
				  if (submatch_status != MATCH_GOOD) { 
					  // condition below added on 210300 since at least 
					  // if original_other_component is a container like object (e.g. Cubby)
					  // then should leave the binding alone
					  if (tt_cause_of_last_mismatch == NULL) {
						  tt_cause_of_last_mismatch = original_other_component;
//#if TT_DEBUG_ON
//					  } else {
//						  log("Demo needs conditional behavior here.");
//#endif
					  };
					  if (submatch_status == MATCH_BAD) { // condition new on 210506
						  // so that for example suspended on empty hole doesn't turn things red in thought bubble
						  component->set_cause_of_match_failure(TRUE,FALSE); // new on 250302
					  };
					  return(submatch_status);
				  };
              // doesn't matter if a remote was touched if matched anyway
//              tt_touched_a_remote = saved_touched_a_remote;
// dereferencing the remote_picture now accomplishes this
//				} else if (contents_type == PICTURE &&
//				           other_type == REMOTE_PICTURE) {
//					  Picture *other_picture =
//						  ((Remote_Picture *) other_component)->pointer_to_picture();
//					  if (other_picture == NULL) return(MATCH_BAD);
//					  tt_touched_a_remote = TRUE;
//					  MatchStatus submatch_status =
//							((Picture *) component)->match(other_picture);
//					  if (submatch_status != MATCH_GOOD) {
//						  return(submatch_status);
//					  };
				} else {
					if (tt_cause_of_last_mismatch == NULL && other_type == REMOTE_PICTURE) { 
						// new on 131200 so suspends waiting for sensor to have an appearance
						tt_cause_of_last_mismatch = original_other_component;
					};
               component->set_cause_of_match_failure(TRUE,FALSE); // new on 250302
					return(MATCH_BAD);
				};
				if (tt_touched_a_remote == ONLY_ON_NEW_CYCLE) { 
					// new on 200902 so successful matches with sensors followed by constants failing doesn't suspend
					tt_touched_a_remote = ONLY_ON_NEW_CYCLE_IF_SUCCEEDS; // so if eventually successful only runs once this cycle
					// but if it fails it doesn't do anything special
				};
			};
		};
		return(MATCH_GOOD); // made it this far so it matches
	} else {
      set_cause_of_match_failure(TRUE,FALSE); // new on 250302
		return(MATCH_BAD);
	};
};

boolean Cubby::contains(Sprite *item) {
   for (int i = 0; i < number_of_holes; i++) {
	  if (contents[i] != NULL && contents[i]->contains(item)) return(TRUE);
	};
	return(FALSE);
};

#if TT_XML

//xml_element *Cubby::xml_create_element(xml_document *document) {
//   return(create_xml_element(xml_tag(),document)); // ,number_of_holes);
//};

void Cubby::xml(xml_element *cubby_element, xml_document *document) {
	//if (returning_items != NULL) { // still make sense?? - decided not on 121202 since this is for dumping before birds return
	//	returning_items->finish_instantly();
	//};
   xml_append_child(create_xml_element(L"NumberOfHoles",document,number_of_holes),cubby_element);
#if TT_DEBUG_ON
	if (tt_debug_mode == 160703) {
		tt_error_file() << "About to generate Hole XML for cubby#" << debug_counter << endl;
	};
#endif
   for (int i = 0; i < number_of_holes; i++) {
      if (contents[i] != NULL || (labels != NULL && labels[i] != NULL)) {
         xml_element *hole = create_xml_element(L"Hole",document,i+1); // one indexing best???
         if (contents[i] != NULL) {
            contents[i]->xml_create_and_append_element(hole,document)->Release();
         };
         if (labels != NULL && labels[i] != NULL) {
            xml_append_child(xml_character_data_element(labels[i],L"Label",document),hole); // prior to 220203 was create_xml_element(L"Label",document,labels[i]),hole);
         };
         xml_append_child(hole,cubby_element); // moved here on 190804
		};
   };
#if TT_DEBUG_ON
	if (tt_debug_mode == 160703) {
		tt_error_file() << "Done generating Hole XML for cubby#" << debug_counter << endl;
	};
#endif
   Sprite::xml(cubby_element,document);
};

xml_element *Cubby::xml_set_special_attributes(xml_element *element, xml_document *document) {
	// new on 121202 -- went through all the state variables
	xml_element *special_element = Sprite::xml_set_special_attributes(element,document);
	if (busy_counter != 0) {
		xml_set_attribute(special_element,L"BusyCounter",busy_counter);
	};
	if (main_cubby_in_body) {
		xml_set_attribute(special_element,L"MainBoxInBody",1);
	};
	if (future_text != NULL) {
		xml_set_attribute(special_element,L"FutureText",future_text);
	};
	if (last_drop_index >= 0) {
		xml_set_attribute(special_element,L"LastDropIndex",last_drop_index);
	};
	if (saved_right_side) {
		xml_set_attribute(special_element,L"RightSide",1);
	};
	// peg_width, previous_width, previous_height can be recomputed so not saved
	return(special_element);
};

boolean Cubby::xml_get_special_attributes(xml_node *node) {
	busy_counter = xml_get_attribute_int(node,L"BusyCounter",0); // prior to 210404 busy_counter
	main_cubby_in_body = xml_get_attribute_int(node,L"MainBoxInBody",0); // prior to 210404 main_cubby_in_body
	future_text = xml_get_attribute_wide_string(node,L"FutureText",NULL); // prior to 210404 future_text
	last_drop_index = xml_get_attribute_int(node,L"LastDropIndex",0); // prior to 210404 last_drop_index
	saved_right_side = xml_get_attribute_int(node,L"RightSide",0); // prior to 210404 saved_right_side
	return(Sprite::xml_get_special_attributes(node));
};

void Cubby::add_special_xml(xml_element *element, xml_document *document) {
	if (item_being_sucked_up != NULL) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	item_being_sucked_up->check_for_independence();
		//};
//		xml_element *into_hole_element = item_being_sucked_up->add_xml(element,L"ItemGoingIntoHole",document);
		// replaced above with following on 200804 to move xml_append_child to end
		xml_element *into_hole_element = create_xml_element(L"ItemGoingIntoHole",document);
		item_being_sucked_up->xml_create_and_append_element(into_hole_element,document,TRUE)->Release(); // prior to 240804 was missing item_being_sucked_up->
		xml_set_attribute(into_hole_element,L"Hole",item_being_sucked_up_index);
		xml_append_child(into_hole_element,element);
	};
	if (non_busy_callbacks != NULL) {
		non_busy_callbacks->add_xml_each(element,document);
	};
	// no need to deal with returning_items in XML since that is for birds returning to this cubby
	Sprite::add_special_xml(element,document); // added 131202
};

void cubby_handle_xml_hole_tag(Entity *cubby, Tag tag, xml_node *node, void *extra_info) {
   ((Cubby *) cubby)->handle_xml_hole_tag(tag,node,(int) extra_info);
};

boolean Cubby::handle_xml_hole_tag(Tag tag, xml_node *node, int hole_number) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("handle_xml_hole_tag of debug target.");
//		xml_debug_node(node);
	};
#endif
   if (tag == LABEL_TAG) {
      set_label(hole_number,xml_get_narrow_text_copy(node));
   } else if (sprite_tag(tag)) {
//#if TT_BUG_FIXES
		// this didn't help since has already released connections
//		Sprite *item = xml_load_sprite(node,tag,contents[hole_number]); // added third arg on 011203 in case undo Bammer or the like
//#else
      Sprite *item = xml_load_sprite(node,tag);
//#endif
      if (item == NULL) return(FALSE); // warn??
		if (item->pointer_to_leader_offsets() != NULL) {
//#if TT_DEBUG_ON
//			if (item->current_width() > 40000 && width < 40000) {
//				debug_this();
//			};
//#endif
			// new on 250103 where geometry has been dumped
			contents[hole_number] = item;
			if (!blank) { // condition new on 260103
				add_follower(item,TRUE,INSERT_UNORDERED,TRUE);
//#if TT_DEBUG_ON
//				xml_debug_node(node);
//#endif
			};
		} else { // for old code
			initialize_hole(hole_number,item,TRUE); 
			// since will be reset when cubby's size is set by geometry tag - new on 180103 -- changed last arg to TRUE on 250103
		};
   }; // else save?? unless NO_MORE_TAGS
   return(TRUE);
};

boolean Cubby::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
      case NUMBER_OF_HOLES_TAG:
         set_number_of_holes(xml_get_int(node));
         return(TRUE);
      case HOLE_TAG: {
            int hole_number = xml_get_int(node,0);
            if (hole_number == 0) return(TRUE); // couldn't parse as number (or was 0 but these are one-indexed) -- warn?? return FALSE?
            if (hole_number > number_of_holes) {
               return(TRUE); // warn?? -- return FALSE?
            };
            hole_number--; // switch to zero indexing
            return(xml_process_children(node,cubby_handle_xml_hole_tag,this,(void *) hole_number));
         };
		case ANIMATION_TAG:
			non_busy_callbacks = new AnimationCallBacks(NULL,CALLBACK_NOTHING);
			return(xml_entity(node,non_busy_callbacks));
		case ITEM_GOING_INTO_HOLE_TAG: {
			xml_node *child_node = first_node_that_is_an_element(node);
			item_being_sucked_up = xml_load_sprite(child_node,tag_token(child_node));
			item_being_sucked_up_index = xml_get_attribute_int(node,L"Hole",item_being_sucked_up_index); // prior to 261003 this looked in the wrong node and always used the default value
//			item_being_sucked_up_index = xml_get_attribute_int(child_node,L"Hole",item_being_sucked_up_index);
//#if TT_DEBUG_ON
//			xml_debug_node(child_node,L"child_node");
//			xml_debug_node(node);
//#endif
			return(TRUE);
												 };
      default:
         return(Sprite::handle_xml_tag(tag,node));
   };
};

#endif

boolean Cubby::dump(output_stream &stream, boolean just_prepare, boolean , boolean ) {
   if (just_prepare) {
      if (followers != NULL) followers->dump(stream,FALSE,just_prepare);
      return(FALSE);
   };
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Dumping cubby debug target.");
	};
	if ((tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) || tt_debug_target == debug_counter) {
		print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
		tt_error_file() << "About to dump ";
		print(tt_error_file());
		tt_error_file() << endl;
		tt_debug_dump_depth++;
	};
#endif
   int dump_index = about_to_dump(this,stream); // rewritten on 020802
   if (dump_index < 0) {
      return(FALSE); // new on 051099
   };
	if (returning_items != NULL) {
		returning_items->finish_instantly();
	};
	// cubby -> size -> contents-0 ... contents-n
//	if (blank) {
//		stream.put((unsigned char) BLANK_CUBBY);
//	} else 
	if (labels != NULL) {
		stream.put((unsigned char) LABELED_CUBBY);
	} else {
		stream.put((unsigned char) CUBBY);
	};
	dump_if_not_null(stream,comment);
//   stream.put((unsigned char) blank);
	dump_common_flags(stream); // on 080400 this replaces the above
// fix up following when updating file format
	cubby_index short_number_of_holes;
	if (number_of_holes > max_unsigned_short) { // new on 070203 - cuts off at 64K if have to
		tt_error_file() << "Box saved in old style format had " << number_of_holes << " but only the first 65,353 saved. Use the SaveInXML=1 option to avoid this problem." << endl;
		short_number_of_holes = max_unsigned_short;
	} else {
		short_number_of_holes = number_of_holes;
	};
	stream.write((character *) &short_number_of_holes, sizeof(short_number_of_holes));
	boolean originally_blank = blank;
	if (blank) {
		become_non_blank(); // so follower relation is correct for dumping pictures and the like
	};
	for (int i = 0; i < number_of_holes; i++) {
		if (labels != NULL) {
			string label = labels[i];
			if (label == NULL) {
				stream.put('\0'); 
			} else {
				int length = _tcslen(label)+1;
				if (length > 255) { // shouldn't be possible anymore
					say_error(IDS_CUBBY_LABEL_TOO_LONG,TRUE);
					length = 255; // was 255 prior to 140599
					label[254] = '\0'; // new on 140599 so error becomes self correcting
				};
				stream.put((unsigned char) length);
				stream.write(label,length);
			};
		};
		if (contents[i] != NULL) {
			contents[i]->dump(stream);
		} else {
			stream.put(EMPTY_HOLE_MAKER);
		};
	};
	if (originally_blank) {
		become_blank(); // so follower relation is correct for dumping pictures and the like
	};
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

void Cubby::set_contents(Sprite **new_contents) {
	if (contents != NULL) delete [] contents; // should be all NULLs
	contents = new_contents;
};

void Cubby::run_when_non_busy(Sprite *sprite, AnimationCallBack callback) {
   // shouldn't be called if busy_counter == 0
	non_busy_callbacks = new AnimationCallBacks(sprite,callback,non_busy_callbacks);
};

void Cubby::decrement_busy_counter() {
  // shouldn't go negative
  if (busy_counter > 0) {
	  busy_counter--;
	  if (busy_counter == 0 && non_busy_callbacks != NULL) {
		  non_busy_callbacks->run();
		  delete non_busy_callbacks;
		  non_busy_callbacks = NULL;
	  };
  };
};

Cubby *load_cubby(SpriteType type, InputStream *pad_in,
                  int notebook_version, NaturalLanguage language,
                  Sprite *container, Picture *controlee, long index, boolean descendants_inactive) {
	boolean blank = FALSE;
	boolean infinite_stack = FALSE;
	Sprite *comment = NULL;
	if (notebook_version >= first_version_to_save_copied_nests) {
		comment = load_item(pad_in,notebook_version,language);
	};
	if (notebook_version > 2) { // can anything with version 1 or 2 still load??
//		blank = (boolean) pad_in->get();
		// replaced above with below on 080400
		load_common_flags(pad_in,blank,infinite_stack);
	};
	cubby_index size; // this really relies upon cubby_index rather than an int since stuff has been saved as short int
	if (notebook_version == 0) {
		size = (unsigned char) pad_in->get();
	} else {
		pad_in->read((string) &size, sizeof(size));
	};
	Cubby *cubby = new Cubby(0,0,0,(4L+size)*tile_width,0,size);
   if (container != NULL) {  // new on 160702
      if (container->kind_of() == PROGRAM_PAD) {
         cubby->set_in_notebook((Notebook *) container,FALSE); // no point recuring
      } else if (container->kind_of() == VACUUM) {
         cubby->set_inside_vacuum(TRUE,FALSE);
      };
   }; 
	associate_shared_item(index,cubby);
	cubby->set_comment(comment);
	cubby->set_infinite_stack(infinite_stack); // new on 080400
//	cubby->save_size();
	if (notebook_version == 0 && type == BLANK_CUBBY) { 
		cubby->become_blank();
		return(cubby);
	};
	boolean labeled = (type == LABELED_CUBBY);
	if (labeled) {
		cubby->is_labeled();
	};
	for (int i = 0; i < size; i++) {
		if (labeled) {
			int label_length = pad_in->get();
			if (label_length > 0) {
				string label;
//				if (unicode_version(notebook_version)) {
					label = new char[label_length];
					pad_in->read((string) label,label_length);
//				} else {
//					label = read_old_string(pad_in,label_length);
//				};
				cubby->set_label(i,label);
			};
		};
		unsigned char content_type = (unsigned char) pad_in->get();
		if (content_type != EMPTY_HOLE_MAKER) {
			Sprite *content = load_item((SpriteType) content_type,pad_in,
												 notebook_version,language,cubby,controlee,-1,FALSE,FALSE,descendants_inactive); // cubby was NULL prior to 170299
			if (content != NULL) {
#if TT_CAREFUL
				if (content->pointer_to_leader() != NULL) { // new on 070601 to repair excessive sharing -- still needed???
               // loading Playground2001 calls this just once with a nest - is top level copy the right kind of copy here?
					content = content->copy(); // removed top_level_ as an experiment on 180702
				};
#endif
				cubby->initialize_hole(i,content);
#if TT_DEBUG_ON
				add_to_log_if_interesting_error();
#endif
			} else {
				say_error(IDS_FILE_DESCRIBING_BOX_BAD,TRUE);
#if TT_DEBUG_ON
				cubby->print(tt_error_file());
#endif
				tt_error_file() << " had troubles loading contents of hole " << (i+1) << endl;
				throw(tt_global_picture); // new on 290803
//				return(NULL); // commented out on 300100 since the problem is always fatal
			};
		};
	};
	if (blank || notebook_version <= 2 && type == BLANK_CUBBY) {
		cubby->become_blank();
	};
	return(cubby);
};

// handled by Sprite::reachable_robots() now (031200)
//Sprites *Cubby::reachable_robots() {
//   Sprites *robots = NULL;
//   for (int i = 0; i < number_of_holes; i++) {
//      if (contents[i] != NULL) {
//         if (robots == NULL) {
//            robots = contents[i]->reachable_robots();
//         } else {
//            robots->last_cons()->set_rest(contents[i]->reachable_robots());
//         };
//      };
//   };
//   return(robots);
//};

// sprite::java_flipped_followers does this now (031200)
//void Cubby::java_flipped_followers(java_stream &out, const_java_string ) {
//   for (int i = 0; i < number_of_holes; i++) {
//      if (contents[i] != NULL) {
//         contents[i]->java_flipped_followers(out,NULL);
//      };
//   };
//};

Sprites *boxes_already_declared = NULL;
Sprites *boxes_already_initialized = NULL;

void release_boxes_already_declared() {
	just_destroy_list(boxes_already_declared);
	just_destroy_list(boxes_already_initialized);
};

void Cubby::java_before(java_stream &out) {
   if (blank) return;
	if (boxes_already_declared != NULL && boxes_already_declared->member_cons(this) != NULL) return; // new on 041200
	java_declare_new(out,S(IDS_JAVA_TT_BOX),S2(IDS_JAVA_BOX,java_index(this)),S(IDS_JAVA_TT_BOX),(int) number_of_holes);
	boxes_already_declared = new Sprites(this,boxes_already_declared);
   for (int i = 0; i < number_of_holes; i++) {
      if (contents[i] != NULL) {
         contents[i]->java_before(out);
      };
   };
//	if (leader != NULL && leader->kind_of() == CUBBY) { // new on 041200 so boxes within boxes are generated correctly
//		java_begin_block(out);
//		java_declare_new(out,S(IDS_JAVA_TT_BOX),S(IDS_JAVA_BOX),S(IDS_JAVA_TT_BOX),(int) number_of_holes);
//	};
};

void Cubby::java(java_stream &out) {
   if (blank) {
      out << S(IDS_JAVA_TT_BLANK_BOX);
//	} else if (leader != NULL && leader->kind_of() == CUBBY) { // new on 041200 so boxes within boxes are generated correctly
//		out << S(IDS_JAVA_BOX); // declare above
   } else {
//      java_new(out,S(IDS_JAVA_TT_BOX),(int) number_of_holes);
		out << S2(IDS_JAVA_BOX,java_index(this));
   };
};

void Cubby::java_after(java_stream &out, const_java_string variable_temp) {
   if (blank) return;
	if (boxes_already_initialized != NULL && boxes_already_initialized->member_cons(this) != NULL) return; // new on 041200
	boxes_already_initialized = new Sprites(this,boxes_already_initialized);
   string variable = copy_string(variable_temp); // might be from string table
   for (int i = 0; i < number_of_holes; i++) {
      if (contents[i] == NULL) {
         if (labels != NULL && labels[i] != NULL) {
            string hole_label = java_quote(labels[i]);
            string new_nothing = java_new(S(IDS_JAVA_TT_NOTHING),hole_label);
            java_send(out,variable,S(IDS_JAVA_SET_HOLE),i,new_nothing);
            delete [] hole_label;
            delete [] new_nothing;
         } else {
            java_send(out,variable,S(IDS_JAVA_SET_HOLE),i,S(IDS_JAVA_TT_DOT_NOTHING));
         };
      } else {
         java_send(out,variable,S(IDS_JAVA_SET_HOLE),i,contents[i]);
         string path = new character[1024];
			output_string_stream expression(path,1024);
         java_just_send(expression,variable,S(IDS_JAVA_HOLE),i);
//         expression << variable << "." << S(IDS_JAVA_HOLE) << "(" << i << ")";
         expression.put('\0');
         contents[i]->java_after(out,path);
         delete [] path;
      };
//      out << "  " << variable << ".SetHole(" << i << ", ";
//      if (contents[i] == NULL) {
//         out << "TT.NOTHING);" << endl;
//      } else {
//         contents[i]->java(out);
//         out << ");" << endl;
//      };
   };
   delete [] variable;
//	if (leader != NULL && leader->kind_of() == CUBBY) { // new on 041200 so boxes within boxes are generated correctly
//		java_end_block(out);
//	};
};

//void Cubby::become(Sprite *other) { // new on 100901 // noticed obsolete on 140205
//	if (other == NULL || other->kind_of() != CUBBY) return; // warn??
//	Cubby *other_cubby = (Cubby *) other;
//	int other_number_of_holes = other_cubby->current_number_of_holes();
//	string *labels = other_cubby->pointer_to_labels();
//	for (int i = 0; i < other_number_of_holes; i++) {
//		Sprite *contents = other_cubby->remove_component_directly(i);
//		if (contents != NULL) {
//			initialize_hole(i,contents);
//		};
//		if (labels != NULL && labels[i] != NULL) { // added labels != NULL on 210102 since undoing an arity bam can trigger this
//			set_label(i,labels[i]);
//			labels[i] = NULL;
//		};
//	};
//};

boolean describe_cubby(SpriteType type, InputStream *pad_in, output_stream &text_out, int version_number,
						     boolean in_thought_bubble, Article ) {
	if (version_number >= first_version_to_save_copied_nests) {
		describe_comment_encoding(pad_in,text_out,version_number,CUBBY);
	};	
	boolean blank;
//	if (tt_notebook_version > 2) {
		blank = (boolean) pad_in->get();
//	} else {
//		blank = (type == BLANK_CUBBY);
//	};
	cubby_index size; // this means that Marty can't describe boxes greater than 32K -- noted on 070203
//	if (tt_notebook_version == 0) {
//		size = (unsigned char) pad_in->get();
//	} else {
		pad_in->read((string) &size, sizeof(size));
//	};
	if (blank) {
		if (in_thought_bubble) {
			text_out << S(IDS_ANY_BOX);
		} else {
			print_type(type,text_out);
		};
		// skip over rest of description
		string temp = new character[10000];
#if TT_NEW_IO
		output_string_stream sink;
#else
		output_string_stream sink(temp,10000);
#endif
      // continue to process but don't say anything
      boolean result = describe_cubby2(type,pad_in,sink,version_number,in_thought_bubble,size);
//		for (cubby_index i = 0; i < size; i++) {
//			describe_encoding((SpriteType) pad_in->get(),pad_in,sink,
//									in_thought_bubble);
//		};
#if !TT_32
		delete [] temp;
#endif
		return(result);
	} else {
      return(describe_cubby2(type,pad_in,text_out,version_number,in_thought_bubble,size));
   };
};

boolean describe_cubby2(SpriteType type, InputStream *pad_in, output_stream &text_out, int version_number,
						      boolean in_thought_bubble, cubby_index size) {
	boolean labeled = (type == LABELED_CUBBY);
	character label[256];
	int label_length;
//	boolean first = TRUE;
//	print_article(article,text_out,S(IDS_BOX_WITH));
   text_out << S(IDS_BOX_WITH);
	if (size == 1) {
		text_out << S(IDS_ONE_HOLE);
	} else {
		text_out << S(IDS_HOLES0,"") << (int) size << S(IDS_HOLES,""); // ,"" added on 071299 for Japanese and the like
	};
	text_out << S(IDS_BOX_WITH2,"");
	for (cubby_index i = 0; i < size; i++) {
		if (labeled) {
			label_length = pad_in->get();
			if (label_length > 0) {
				pad_in->read((string) label,label_length);
				label[label_length] = 0; // terminate the string
			};
		} else {
			label_length = 0;
		};
		unsigned char content_type = (unsigned char) pad_in->get();
		if (content_type != EMPTY_HOLE_MAKER) {
//			if (first) {
            if (size > 1) {
//				   text_out << S(IDS_WHOSE);
					if (tt_language == BRAZILIAN_PORTUGUESE) { // new on 150600
						text_out << comma() << space() << S(IDS_HOLE_OF1,""); 
					} else {
						text_out << end_sentence() << S_capitalize_first(IDS_HOLE_OF1,""); // ,"" added on 071299
					};
//               capitalize_first_letter(S(IDS_HOLE_OF1),text_out);
//				   first = FALSE;
            };
//			} else {
//				text_out << S(IDS_AND_WHOSE);
//			};
         text_out << space();
			long next_string_id;
			if (i > 12 && spoken_language() == BRAZILIAN_PORTUGUESE) { // new on 160600 
				// first 13 ordinals have names
				// so "26º buraco contenha" becomes "buraco 26 contenha"
				text_out << "buraco " << (i+1) << " contenha ";
				next_string_id = IDS_HOLE_CONTAINS2; // will be empty string anyway...
			} else {
				if (size != 1 && label_length == 0) {
					print_ordinal(i+1,text_out);
					text_out << space();
				};
				if (size == 1) {
					text_out << S(IDS_WHICH_CONTAINS) << space(); // space moved here on 300699
					next_string_id = IDS_WHICH_CONTAINS2;
				} else if (label_length > 0) {
					replace_carriage_returns_with_spaces(label,label_length); // new on 031200
					text_out << S(IDS_HOLE_LABELED_WITH) << "\"" << label << "\"" << space() << S(IDS_CONTAINS);
					next_string_id = IDS_CONTAINS2;
				} else {
					text_out << S(IDS_HOLE_CONTAINS);
					next_string_id = IDS_HOLE_CONTAINS2;
				};
			};
//			text_out << " "; // added on 140699
			// probably don't want the article to be passed along
			// which would result in "the cubby whose first hole has the number ..."
			boolean result = describe_encoding((SpriteType) content_type,pad_in,text_out,version_number,in_thought_bubble,
														  INDEFINITE_ARTICLE,CUBBY);
			if (!result) { // these added on 140599 so that description comes out right
				return(FALSE); // something wrong
			};
			text_out << S(next_string_id,"");
		};
	};
	return(TRUE);
};

// noticed obsolete on 121202
//void Cubby::set_inside_sprite(boolean new_flag) {
//	inside_sprite_flag = (flag) new_flag;
//	if (inside_sprite_flag && floor != NULL) {
//		floor->remove_item(this,FALSE);
//	};
//};

boolean Cubby::equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, SpritesPointer &expected_corresponding_nests, EqualReason reason) { // new on 050601
   if (other->kind_of() != CUBBY) {
      return(FALSE);
   };
	if (other == this) return(TRUE);
   if (is_blank() != other->is_blank()) {
      return(FALSE);
   };
	Cubby *other_cubby = (Cubby *) other;
   if (current_number_of_holes() != other_cubby->current_number_of_holes()) {
      return(FALSE);
   }
   if (is_blank() != other_cubby->is_blank()) {
      return(FALSE);
   };
	string *my_labels = pointer_to_labels();
	string *others_labels = other_cubby->pointer_to_labels();
   if (my_labels == NULL && others_labels != NULL) {
      return(FALSE);
   };
	if (my_labels != NULL) {
      if (others_labels == NULL) {
         return(FALSE);
      };
		for (int i = 0; i < number_of_holes; i++) {
			if (my_labels[i] != NULL) {
            if (others_labels[i] == NULL) {
               return(FALSE);
            };
            if (strcmp(my_labels[i],others_labels[i]) != 0) {
               return(FALSE);
            };
         } else if (others_labels[i] != NULL) {
            return(FALSE);
         };
		};
	};
	for (int i = 0; i < number_of_holes; i++) {
		Sprite *my_component = component(i);
		Sprite *other_component = other_cubby->component(i);
		if (my_component != NULL && other_component != NULL) {
			if (!my_component->equal_to(other_component,pending,corresponding_nests,expected_corresponding_nests,reason)) {
				return(FALSE);
			};
      } else if (my_component != NULL || other_component != NULL) {
         return(FALSE); // only one is NULL
      };
	};
	return(TRUE);
};

boolean cubby_contents_type(SpriteType contents_type) {
  // what about BLANK_x?
  return(contents_type == CUBBY || 
			contents_type == INTEGER || // contents_type == ERROR_INTEGER ||
			contents_type == TEXT ||
			contents_type == NEST ||
			contents_type == BIRD ||
			contents_type == PICTURE ||
			contents_type == REMOTE_PICTURE ||
         contents_type == SOUND ||
         (// tt_log_version_number > 3 &&
          (contents_type == ROBOT ||   // added 3/1/97
           contents_type == PROGRAM_PAD ||
           contents_type == TRUCK ||
           contents_type == BOMB ||
			  contents_type == FORCE_FEEDBACK))); // added 100400
};

void user_broke_cubby() {
	user_did(CUBBY,CUBBY_BROKEN);
};

void user_obtained_arity() {
	user_did(CUBBY,CUBBY_ARITY_OBTAINED);
};

void Cubby::active_region(TTRegion &region) {
	// new on 230902 since drop feedback when holding a cubby was inconsistent
	full_region(region);
};

boolean Cubby::vacuum_if_left_on_floor(SpritePointer *others_being_vaccuumed, int others_count) { // new on 010106
	// don't vacuum new main cubby
	if (is_main_cubby_in_body()) return(FALSE);
	return(Sprite::vacuum_if_left_on_floor(others_being_vaccuumed,others_count));
};

/*
void Cubby::used(Tool *subject, Sprite *by,
					  boolean , Screen *screen,
					  millisecond duration, boolean *true_when_done) {
	if (true_when_done != NULL) *true_when_done = TRUE;
	// a cubby "used" on another cubby increases its size
	if (subject == NULL) return; // used on nothing
	// following may select cubby inside a cubby for example
	if (by->kind_of() != ROBOT) { // robot in training ok
		TTRegion region;
		active_region(region);
		subject = subject->select(&region,MIGHT_USE_CUBBY,by,screen,floor);
		if (subject != NULL) {
			subject = subject->selected(CUBBY_USED,by);
		};
	};
	if (subject == NULL) return; // if it refused
	switch (subject->kind_of()) {
		case CUBBY:
			Cubby *other = (Cubby *) subject;
			int new_number = other->current_number_of_holes()+number_of_holes;
			if (duration > 0) play_sound(MAGIC_SOUND);
			other->set_number_of_holes(new_number);
			floor->record_event2(NEW_CUBBY_SIZE,by,subject,new_number);
//       floor->record_event(CUBBY_APPLY,subject);
			break;
	};
};
*/
	
/*	
	if (infinite_stack_flag) {
		return(this);
	};
	if (number_of_holes == 0) {
		selection_status = SELECTION_OK;
		return(this);
	};
	if (item_being_sucked_up != NULL) { // receiption of an item in progress
		selection_status = SELECTION_REFUSED;
		return(NULL);
	};
	if (leader != NULL &&
		 reason != MIGHT_USE_COPIER &&
		 reason != MIGHT_USE_COPIER_NUMBER &&
		 ((contained_in(ROBOT) && !contained_in(THOUGHT_BUBBLE)) ||
		  contained_in(COPIER))) {
		selection_status = SELECTION_REFUSED;
		return(NULL);
	};
	// if there is something in the cubby see if region is inside
	if ((reason == MIGHT_GRASP ||
		  reason == MIGHT_USE_VACUUM || reason == MIGHT_USE_COPIER||
		  reason == MIGHT_USE_COPIER_NUMBER || reason == MIGHT_USE_NUMBER) &&
		 !all_holes_empty()) {
		city_coordinate region_center_x, region_center_y;
		region->region_center(region_center_x, region_center_y);
		int hole_index = closest_hole(region_center_x, region_center_y);
		if ((contents[hole_index] == NULL) || // empty hole or
			 ((reason == MIGHT_GRASP ||
			  (reason == MIGHT_USE_VACUUM && !contained_in(ROBOT)) || // not blanking
			  reason == MIGHT_USE_COPIER) &&
			  !contents[hole_index]->pointer_to_collision_region()
				 ->contains(region))) { // closest hole is still not good enough

			return(this);
		};
		Tool *selection
			= contents[hole_index]
				 ->select(region,reason,screen,floor,selection_status);
		if (selection_status == SELECTION_BLANKED) {
			return(NULL);
		};
		if (selection_status == SELECTION_REFUSED) {
			record_grasp_or_vacuum(reason,this,selection_status);
			return(this);
		};
		// if a component has been vacuumed or is a cubby it will record it
		if (reason == MIGHT_GRASP && selection->kind_of() != CUBBY) {
			floor->record_event(GRASP_ITEM,selection);
//         floor->record_event(NEW_ITEM,selection);
			selection_status = SELECTION_OK_AND_RECORDED;
		};
//      record_grasp_or_vacuum(reason,selection);
		if (selection == contents[hole_index] && // contents selected
			 (reason == MIGHT_GRASP || reason == MIGHT_USE_VACUUM)) {
			remove_component(hole_index,reason,showing_on_a_screen());
		};
		return(selection);
	} else {
		record_grasp_or_vacuum(reason,this,selection_status);
		return(this);
	};
};

  #if !TT_32
boolean Cubby::vectorize(int arity, int offset,
								 SpriteType *types, long *vector) {
	// converts internal state to vector if state matches types
	// return FALSE if mismatch
	if (arity+offset > number_of_holes) return(FALSE);
	for (int i = 0; i < arity; i++) {
		Sprite *item = contents[i+offset];
		if (item == NULL) return(FALSE); // what about type ANY?
		item = item->dereference();
		SpriteType type = item->kind_of();
		if (type != types[i]) return(FALSE); // what about nest?
		switch (type) {
			case REMOTE_PICTURE: // I think this is right
			case INTEGER: {
				long value;
				((Integer *) item)->current_value(value);
				vector[i] = value;
			  };	
			  break;
			case PICTURE: 
				vector[i] = ((Picture *) item)->picture_id();
			  break; 
			default:
				vector[i] = (long) item;
		};
	};
	return(TRUE);
};
#endif
*/
