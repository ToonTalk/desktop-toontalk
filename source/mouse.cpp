// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif   
#if !defined(__TT_NUMBER_H)   
#include "number.h"
#endif   
#if !defined(__TT_FLOOR_H)   
#include "floor.h"
#endif   
#if !defined(__TT_ROBOT_H)   
#include "robot.h"
#endif   
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif 
#if !defined(__TT_HELP_H)   
#include "help.h"
#endif  
#if !defined(__TT_PRGRMMR_H)   
#include "prgrmmr.h"
#endif 
#if TT_XML
#if !defined(__TT_XML_H)   
#include "xml.h"
#endif 
#endif
#if !defined(__TT_MOUSE_H)   
#include "mouse.h"
#endif

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const city_coordinate mouse_offscreen_left = -8*tile_width;
const city_coordinate mouse_offscreen_right = 30L*tile_width;
const city_coordinate mouse_offscreen_bottom = -10*tile_height;

//millisecond mouse_change_value_duration() {
//	return(mouse_to_smash_time+mouse_to_run_out_time);
//};

delayed_call_in_a_mouse::delayed_call_in_a_mouse(Sprite *under_item, Sprite *over_item,
							                            Background *floor, Sprite *by,
																 boolean save_copy,
																 boolean delete_smashee,
																 Sprite *original_recipient, Sprite *original_over_item) :
     under_item(under_item),
     over_item(over_item),
     floor(floor),
     saved_by(by),
//     action_status(action_status), // made obsolete on 171102
     save_copy(save_copy),
     delete_smashee(delete_smashee),
     original_recipient(original_recipient),
	  original_over_item(original_over_item) {
	under_item->destroy(); // will just decrement ref count in normal case
	over_item->destroy();
};

void delayed_call_in_a_mouse::run() {
	if (under_item->prepared_for_deletion() || over_item->prepared_for_deletion()) { // were destroyed in the meanwhile
      ::completed(saved_by);
		return;
	};
   call_in_a_mouse(under_item,over_item,
						 floor,saved_by,
						 save_copy,
						 delete_smashee,0,original_recipient,original_over_item);
};

/* moved to Floor
Mouse *previous_mouse = NULL;

void release_previous_mouse() {
	if (previous_mouse != NULL) {
		Mouse *saved_previous_mouse = previous_mouse;
		previous_mouse = NULL;
		// following can call this recursively so set previous_mouse first
		saved_previous_mouse->remove_from_floor();
		saved_previous_mouse->destroy();
	};
};
*/

void call_in_a_mouse(Sprite *under_item, Sprite *over_item,
							Background *floor, Sprite *by,
							boolean save_copy,
							boolean delete_smashee,
                     millisecond delay,
                     Sprite *original_recipient, Sprite *original_item) {
   if (under_item == over_item) {
#if TT_DEBUG_ON
      say_error(_T("Item on itself!"));
#endif
		return; // new on 011004 for robustness
   };
	if (!under_item->selectable()) {
		return; // new on 230203 so you can't pick up the under item and drop it on the over item before being bammed
	};
	if (!under_item->showing_on_a_screen()) { 
		// new on 200105 -- much lower overhead and avoids problems like Leonel's robot that adds to decoration sensors
		under_item->set_to_future_value(over_item,original_recipient,by);
		completed(by); // new on 020405 so that this optimization doesn't cause robots to get stuck
		return;
	};
   over_item->set_selectable(FALSE); // or should grabbing it abort the mouse??
   if (delete_smashee) {  // protect these thing before possibly delaying...
 		over_item->set_available_for_indexing(FALSE);
		over_item->set_to_be_smashed(TRUE);
	} else {
		over_item->set_available_for_indexing(TRUE); // since it's not going away
	};
   long over_item_priority;
   Sprite *region2_source;
   if (under_item != original_recipient && under_item != original_recipient->dereference()) { 
		// dereference added on 150201 so remotes look better
      region2_source = original_recipient;
      over_item_priority = min(original_recipient->ultimate_leader()->priority(),under_item->ultimate_leader()->priority())-1; 
		// min introduced on 120101 in case under_item has lower priority
   } else if (under_item->is_picture() && under_item->kind_of() != PICTURE &&
		        under_item->kind_of() != REMOTE_PICTURE) { // conjunct added 171099
		// is an indirection -- so location might also be
      region2_source = over_item;
      over_item_priority = min(over_item->priority(),0);
   } else {
      region2_source = under_item;
      over_item_priority = under_item->ultimate_leader()->priority()-1;
   };
   // make sure over is on top of under
	over_item->set_priority(over_item_priority);
   over_item->set_priority_fixed(TRUE);
// under_item->set_priority(over_item->priority()+1);
//   if (delay > 0 && tt_log_version_number < 14) { // this wasn't such a good idea so only wait for old demos
//		under_item->increment_ref_count(by); // in case it is destroyed in the meanwhile
//		over_item->increment_ref_count(by);
//      tt_global_picture->do_after(delay,new delayed_call_in_a_mouse(under_item,over_item,
//						                                                  floor,by,
//						                                                  action_status,save_copy,
//						                                                  delete_smashee,original_recipient),
//                                  run_delayed_call_in_a_mouse_callback);
//      return;
//   };
	// runs on heading north east starting off screen
	city_coordinate initial_x, initial_y;
	city_coordinate smashee_center_x,smashee_center_y;
	TTRegion region1,region2,intersection;
   over_item->full_region(region1);
	region2_source->full_region(region2);
	region1.intersect_with(region2,intersection);
	// added the following before I discovered it was needed only because region2_source was equal to over_item and was large
//	TTRegion *screen_region = tt_screen->viewing_region();
//	screen_region->intersect_with(intersection,intersection); // added on 171099
	intersection.region_center(smashee_center_x,smashee_center_y);
	// following new so mouse is never off screen due to large numbers or text pads
	if (smashee_center_x < tt_screen->min_x()) {
		if (intersection.max_x > tt_screen->max_x()) { // off both sides
			smashee_center_x = (tt_screen->min_x()+tt_screen->max_x())/2; // center it
		} else {
			smashee_center_x = tt_screen->min_x();
		};
	} else if (smashee_center_x > tt_screen->max_x()) {
		if (intersection.min_x < 0) {
			smashee_center_x = (tt_screen->min_x()+tt_screen->max_x())/2; // center it
		} else {
			smashee_center_x = tt_screen->max_x();
		};
	};
	if (smashee_center_y < tt_screen->min_y()) {
		if (intersection.max_y > tt_screen->max_y()) { // off both sides
			smashee_center_y = (tt_screen->min_y()+tt_screen->max_y())/2; // center it
		} else {
			smashee_center_y = tt_screen->min_y();
		};
	} else if (smashee_center_y > tt_screen->max_y()) {
		if (intersection.min_y < 0) {
			smashee_center_y = (tt_screen->min_y()+tt_screen->max_y())/2; // center it
		} else {
			smashee_center_y = tt_screen->max_y();
		};
	};
//	over_item->center_location(center_x,center_y);
	if (smashee_center_x > smashee_center_y) { 
		initial_x = (smashee_center_x-smashee_center_y)+mouse_offscreen_left;
		initial_y = mouse_offscreen_bottom;
	} else {
		initial_x = mouse_offscreen_left;
		initial_y = (smashee_center_y-smashee_center_x)+mouse_offscreen_bottom;
	};
	Mouse *mouse = new Mouse(initial_x,initial_y,over_item,under_item,by,
									 smashee_center_x,smashee_center_y,
									 save_copy,delete_smashee,original_recipient,original_item);
   Sprite *true_under_item; // new on 081102 since may be that under_item is the indirection of a picture
   if (original_recipient != NULL) {
      true_under_item = original_recipient;
   } else {
      true_under_item = under_item;
   };
	floor->add_item(mouse,FALSE); // moved here on 230203 so default_duration works OK
	const millisecond head_start = 333;
	millisecond duration_until_smash = mouse->default_duration(mouse_to_run_out_time-head_start); // moved here on 230203 to share with following
	city_coordinate over_item_llx,over_item_lly; // renamed and also moved here on 230203
	over_item->lower_left_corner(over_item_llx,over_item_lly);
	if (tt_parts_change_when_bammed) {
		// new on 230203 to not update parts and container sensor until really smashed
		city_coordinate true_under_item_llx,true_under_item_lly;
		true_under_item->lower_left_corner(true_under_item_llx,true_under_item_lly);
		over_item->animate_location_to(true_under_item,duration_until_smash,NULL,CALLBACK_NOTHING,
												 over_item_llx-true_under_item_llx,over_item_lly-true_under_item_lly,FALSE);
	} else if (true_under_item->ok_for_mouse_to_temporarily_add_follower() && // moved here on 031002 so copy for undo is "pristine" 
		 over_item->pointer_to_leader() == NULL) { // but don't do this twice
		// this way if the under_item is moved before mouse comes it all looks fine
      over_item->remove_from_floor(FALSE,FALSE);
	   true_under_item->add_follower(over_item,FALSE,INSERT_BY_PRIORITY);
   };
   if (floor->in_the_room()) {
      mouse->set_priority_fixed(TRUE);
      mouse->set_priority(min(over_item_priority-1,min_long+2000000)); // over_item->priority()-1); // on top of "upper thing"
   } else {
 	   mouse->recompute_mouse_priority();
		if (by != NULL && by->kind_of() != ROBOT) { // new on 060701
			floor->set_previous_mouse(mouse);
		};
   };
   mouse->resize_if_in_room();
	if (tt_good_sizes_adjust_to_screen_size) { 
		mouse->set_size(shrink_width_from_640x480_screen_size(mouse->current_width()),
							 shrink_height_from_640x480_screen_size(mouse->current_height()));
	};
	mouse->set_selectable(FALSE); // might be fun to catch mouse but a bit of work to support
	mouse->animate_location_to(over_item,
									   mouse->default_duration(mouse_to_run_out_time) 
										// after it is finished - was floor->default_duration(mouse_to_run_out_time) 
                                 +(under_item->animations_stop_time()-tt_current_time),
									   NULL,CALLBACK_NOTHING,
									   (smashee_center_x-over_item_llx)-mouse->current_width(),
									   (smashee_center_y-over_item_lly)-mouse->current_height()/3);
	// the following is so that the mouse doesn't wait so long before beginning to smash
	mouse->do_after(duration_until_smash,mouse,GET_SMASHED_CALLBACK); // was floor->default_duration(mouse_to_run_out_time-head_start)
//	postpone_new_logs(); // what about situations where there are mice non-stop??
};

Mouse::Mouse(city_coordinate initial_x, city_coordinate initial_y,
				 Sprite *the_item_to_smash, Sprite *the_item_underneath,
				 Sprite *by, 
				 city_coordinate smashee_center_x,
				 city_coordinate smashee_center_y,
				 boolean save_copy, boolean delete_smashee,
             Sprite *original_recipient, Sprite *original_item_to_smash) :
		Sprite(MOUSE_SPRITE,initial_x,initial_y,MOUSE_WITH_HAMMER,MOUSE_RUNNING_NORTHEAST),
		item_to_smash(NULL), // new on 140904 since is now set and ref counted
//		item_to_smash(item_to_smash), // set below
//		action_status(action_status), // obsoleted on 171102
// 	item_underneath(item_underneath), // set below
//		floor(floor),
//		by(by),
		smashee_center_x(smashee_center_x),
		smashee_center_y(smashee_center_y),
		previous_smashee_center_x(min_long), // wasn't initialized prior to 131202
		previous_smashee_center_y(min_long), // wasn't initialized prior to 131202
		save_copy((flag) save_copy),
		delete_smashee((flag) delete_smashee),
//		original_smashee(original_recipient), // new on 100901 -- set by set_original_smashee now to maintain ref counts
//		smashee_copy(NULL),
      previous_over_item(NULL) {
		//previous_under_item(NULL),
		//previous_over_item_copy(NULL),
		//previous_under_item_active(FALSE),
		//previous_over_item_copy_active(FALSE) {
//  set_priority_function_of_lly(FALSE);
//	original_smashee = original_recipient; // was item_underneath prior to 100901
//   saved_by = by;
	// original_item_to_smash is new on 101202 and is like item_to_smash (usually ==) 
	// but is the one that needs to be restored (e.g. text on a picture becomes show_all again)
   set_saved_by(by,TRUE); // better to reference count this (new on 191102) -- added TRUE on 050803 so this is saved even if done by programmer
	set_item_to_smash(the_item_to_smash); // new on 171202 to ref count
	set_item_underneath(the_item_underneath); // new on 171202 to ref count
	set_original_smashee(original_recipient); 
   if (by == NULL || by->kind_of() != PROGRAMMER) { 
		// new on 031002 since no need to do this if robot is doing it (or a robot is being trained - 041002)
      previous_under_item_xml = NULL;
   } else {
#if TT_XML
#if TT_DEBUG_ON
      millisecond start_time;
		if (tt_debug_mode == 180702) {
			start_time = timeGetTime(); // new on 180702
		};
#endif
//	   previous_under_item = original_smashee->top_level_copy(); // new on 050701
//      tt_dump_nests_as_networked = TRUE;
		UnwindProtect<DumpReason> set(tt_dump_reason,DUMPING_TO_ENABLE_UNBAMMING); // new on 111202
      previous_under_item_xml = original_smashee->xml_create_document(FALSE); 
		// rewritten on 041002 so bird nests don't cause confusion by being copied
//		tt_dump_nests_as_networked = FALSE;
#if TT_DEBUG_ON 
		if (tt_debug_mode == 180702) { // don't bother unless performance debugging this
			tt_error_file() << "To encode under item before mouse smash took " << (timeGetTime()-start_time) << "ms" << endl;
		};
		if (tt_debug_mode == 708031) { 
			xml_debug_node(previous_under_item_xml);
		};
#endif
#endif
    //  previous_under_item_active = previous_under_item->active();
	   //previous_under_item->set_active(FALSE);
   };
	set_previous_over_item(original_item_to_smash); // changed on 090701 ->top_level_copy(); -- added original on 111002
   if (previous_under_item_xml != NULL) { // conditional new on 031002
	   //boolean saved_tt_dont_connect_things_while_copying = tt_dont_connect_things_while_copying;
	   //tt_dont_connect_things_while_copying = TRUE;
	   //previous_over_item_copy = item_to_smash->top_level_copy(); 
		// new on 100901 since need to use original for robots to undo but need to copy state to restore things properly
      //tt_dont_connect_things_while_copying = saved_tt_dont_connect_things_while_copying;
	   //previous_over_item_copy_active = previous_over_item_copy->active();
	   //previous_over_item_copy->set_active(FALSE);
		//tt_dump_nests_as_networked = TRUE;
		UnwindProtect<DumpReason> set(tt_dump_reason,DUMPING_TO_ENABLE_UNBAMMING); // new on 111202
      previous_over_item_xml = original_item_to_smash->xml_create_document(FALSE); // updated on 100402 -- original added on 101202
//      tt_dump_nests_as_networked = FALSE;
      // could time how long this takes as well...
#if TT_DEBUG_ON
		if (tt_debug_mode == 708031) {
			xml_debug_node(previous_over_item_xml);
		};
#endif
   } else {
      previous_over_item_xml = NULL;
   };
//	previous_smashee_center_x = smashee_center_x-current_width(); //(smashee_center_x-item_to_smash->current_llx())-current_width();
//	previous_smashee_center_y = smashee_center_y-current_height()/3; // (smashee_center_y-item_to_smash->current_lly())-current_height()/3;
	previous_initial_x = initial_x;
	previous_initial_y = initial_y;
	set_show_all(FALSE); // new on 131202 so XML dumps this without mentioning visibility
};

void Mouse::prepare_for_deletion() { // new on 050701
   if (preparing_for_deletion) return; // already done -- new on 060204
	preparing_for_deletion = TRUE; // new on 060204
	release_undo_information();
	if (item_underneath != NULL) {
		if (item_underneath->destroy(this)) { // release
			item_underneath = NULL; // new on 170105 to make sure this isn't deleted twice -- made conditional on 180105
		};
	};
	if (item_to_smash != NULL) {
		if (item_to_smash->destroy(this)) { // release
			item_to_smash = NULL; // new on 170105 to make sure this isn't deleted twice -- made conditional on 180105
		};
	};
	Sprite::prepare_for_deletion(); // new on 100901
};

boolean Mouse::undo_information_involves(Sprite *sprite) { 
	// new on 071003 - at least needed with foreign birds (if box they are given had been smashed)
	return (sprite == previous_over_item || sprite == original_smashee || sprite == item_underneath || sprite == item_to_smash);
};

void Mouse::release_undo_information() {
	//if (previous_under_item != NULL) {
	//	previous_under_item->destroy();
	//	previous_under_item = NULL;
	//};
   if (previous_under_item_xml != NULL) {
      previous_under_item_xml->Release();
		previous_under_item_xml = NULL; // new on 170603 -- important for time travel and saving cities with multiple Bammers
   };
	if (previous_over_item != NULL) {
		previous_over_item->destroy();
		previous_over_item = NULL;
	};
	//if (previous_over_item_copy != NULL) {
	//	previous_over_item_copy->destroy();
	//	previous_over_item_copy = NULL;
	//};
	// what about previous_under_item_xml? -- asked 100603
   if (previous_over_item_xml != NULL) {
      previous_over_item_xml->Release();
		previous_over_item_xml = NULL;
   };
	//if (smashee_copy != NULL) {
	//	smashee_copy->destroy();
	//	smashee_copy = NULL;
	//};
   if (original_smashee != NULL) {
      // decrement ref count again??
      original_smashee->destroy(this); // really releasing reference - new on 041002
	   original_smashee = NULL;
   };
};

void Mouse::abort() { // new on 171202
	undo_add_follower();
	action_completed(); // so robot dropping an exploding thing doesn't get stuck -- but will it?
	if (floor != NULL) { // can't undo an aborted mouse bam
		floor->forget_previous_mouse();
	};
	remove_mouse();
};

void Mouse::get_smashed() {
	if (item_to_smash->slated_for_deletion() || item_underneath->slated_for_deletion()) {
		// new on 191202 to deal with cases where these were to be destroy - e.g. bomb went off on the back
		abort();
		return;
	};
	recompute_mouse_priority();
//	floor->remove_item(item_to_smash,FALSE,FALSE);  // didn't always work -- different backgrounds... following better
//   if (tt_log_version_number < 14 && item_underneath->ok_for_mouse_to_temporarily_add_follower()) {
//      item_to_smash->remove_from_floor(FALSE,FALSE);
//	   item_underneath->add_follower(item_to_smash,FALSE,INSERT_BY_PRIORITY);
//   };
	set_parameter(MOUSE_USING_HAMMER_TO_SMASH);
	set_cycle_stopped_callback(DO_OPERATION_CALLBACK);
	//	millisecond hit_and_lift = total_cycle_duration();
//	do_after(mouse_to_smash_time,this,do_operation_callback);
//	do_after(hit_and_lift,this,am_smashed_callback);
};

void Mouse::undo_get_smashed() {
//	recompute_mouse_priority();
//	set_parameter(MOUSE_USING_HAMMER_TO_SMASH);
//	set_cycle_stopped_callback(undo_run_out_callback);
	set_parameter(MOUSE_RUNNING_NORTHEAST);
	animate_location(previous_initial_x,previous_initial_y,default_duration(1000),this,DONE_UNDOING_CALLBACK);
};

void Mouse::done_undoing() {
	remove_from_floor(FALSE,TRUE);
	destroy();
//	previous_mouse = NULL;
};

boolean Mouse::undo_add_follower() { // abstracted on 171202
	Sprite *true_item_underneath;
   if (original_smashee != NULL) {
      true_item_underneath = original_smashee;
   } else {
      true_item_underneath = item_underneath;
   };
	boolean was_ok_to_add_follower = true_item_underneath->ok_for_mouse_to_temporarily_add_follower();
   if (was_ok_to_add_follower) {
	   true_item_underneath->remove_follower(item_to_smash,FALSE);
		tt_screen->remove(item_to_smash);
   };
	return(was_ok_to_add_follower);
};

void Mouse::do_operation() {
	if (item_to_smash->slated_for_deletion() || item_underneath->slated_for_deletion()) {
		// new on 191202 to deal with cases where these were to be destroy - e.g. bomb went off on the back
		abort();
		return;
	};
	recompute_mouse_priority();
//	if (item_to_smash->pointer_to_leader() == NULL &&
//		 item_to_smash->current_held_status() == NOT_HELD) { 
		// not held or put somewhere
		// smash center of intersection
	if (save_copy) { // since lots of information loss
		Sprite *copy = item_underneath->copy();
		if (tt_toolbox != NULL) tt_toolbox_vacuum->add_deletee(copy);
	};
	boolean was_ok_to_add_follower = (!tt_parts_change_when_bammed && undo_add_follower());
	if (delete_smashee) { // item_to_smash->kind_of() != PICTURE) {	
		// so this doesn't end up calling in the mouse in appropriately
		// to clobber this and and a neighbor
		floor->give_item_priority(item_underneath);
//		if (tt_log_version_number > 14) {
		if (item_to_smash != previous_over_item) { // condition new on 090701
			item_to_smash->set_slated_for_deletion(TRUE); // new on 220499 so that if say a remote integer can take this into account
		};
		floor->remove_item(item_to_smash,FALSE);
//		tt_toolbox_vacuum->add_deletee(item_to_smash->copy(NULL));
	} else {
		if (was_ok_to_add_follower) floor->add_item(item_to_smash); // remove as follower
		item_to_smash->set_selectable(TRUE); // once again
	};
	item_underneath->set_to_future_value(item_to_smash,original_smashee,saved_by);
	// added original_smashee on 060102 so that for example picture finds out when indirection changes
	// added by and action_status to deal with errors (e.g. divide by zero) - new on 140202
	//if (previous_under_item_xml != NULL) { // conditional new on 120701
	//	smashee_copy = original_smashee->top_level_copy(); // new on 090701 -- was item_underneath prior to 100901
	//};
	//	} else if (item_to_smash->kind_of() != PICTURE) {
		// if picture just do stuff at end of procedure
		// abort operation by undoing it
//		play_sound(PLOP_SOUND,5); // replace with better sound
//		item_underneath->abort_operation();
//		am_smashed(FALSE);
//		return;
//	};
	if (showing_on_a_screen()) { // added on 270199
		play_sound(POP_SOUND); // replace with better sound
	};
	set_parameter(MOUSE_AFTER_SMASH);
	set_cycle_stopped_callback(AM_SMASHED_CALLBACK);
}; 

void Mouse::undo_do_operation() {
	if (floor == NULL) {
#if TT_POST3186
		if (original_smashee != NULL && original_smashee->pointer_to_background() != NULL) {
			// new on 051206 since Leonel had a demo that exhibited this problem and this fixed it
			original_smashee->pointer_to_background()->add_item(this);
		} else
#endif
		return; // new on 100901
	};
   if (previous_over_item_xml == NULL) return; // new on 041002 -- maybe done while training a robot
//	recompute_mouse_priority();
	if (showing_on_a_screen()) { // could it be otherwise?
		play_sound(POP_SOUND); // replace with better sound
	};
   Sprite *previous_under_item = NULL; // beginning 041002 reconstruct this if possible rather than save a copy
	if (original_smashee != NULL) {
#if TT_XML
      if (previous_under_item_xml != NULL) {
//			tt_error_file() << "previous_under_item about to be restored" << endl; // REMOVE!!!! 201003
//			original_smashee->describe(tt_error_file());
//			tt_error_file() << " is original_smashee before xml_load_sprite" << endl;
//			xml_debug_node(previous_under_item_xml);
			UnwindProtect <boolean> set(tt_loading_to_undo_bammer,TRUE); // new on 211003
			original_smashee->release_all_references(); // prior to 020504 was release_outgoing_connections(); 
			// new on 311003 - so it doesn't remember its followers or the like
         boolean aborted = FALSE; // new on 140105 -- ignored (at least for now)
			previous_under_item = xml_load_sprite(previous_under_item_xml,aborted,original_smashee); // original_smashee new on 170803
			//if (previous_under_item != NULL) { // REMOVE!!!! 201003
			//	tt_error_file() << "previous_under_item restored" << endl;
			//	previous_under_item->describe(tt_error_file());
			//	tt_error_file() << endl;
			//	tt_error_file() << "Address is " << (unsigned long) original_smashee 
			//						 << " which should be same as " << (unsigned long) previous_under_item << endl;
			//};
			set_sprites_to_active_now(); // shouldn't be needed anymore since these are ToonTalkObject
      };
#endif
      // commented out on 041002 since it doesn't make sense to train robots to undo a smash -- hard to undo at run time
//		if (by != NULL && by->kind_of() == ROBOT_IN_TRAINING) { // new on 090701 -- bad idea???
//			Robot *robot = (Robot *) by;
////			robot->remove_tool_from_working_set(original_smashee,previous_under_item); // since destroyed below -- commented out on 041002
////			robot->add_to_working_set(previous_under_item,FALSE);
//			robot->add_to_working_set(previous_over_item,TRUE);
//			previous_over_item->set_available_for_indexing(TRUE);
//			previous_over_item->set_selectable(TRUE);
//			previous_over_item->set_to_be_smashed(FALSE);
//			floor->undo_previous_event();
//		};
		// commented out the following on 170803 since now clobber original_smashee when loading the XML
		//Sprite *smashees_leader = original_smashee->pointer_to_leader();
		//if (smashees_leader != NULL && previous_under_item != NULL) { // new on 070701
		//	if (smashees_leader->kind_of() == CUBBY) {
		//		Cubby *cubby = (Cubby *) smashees_leader;
		//		int index = cubby->my_index(original_smashee);
		//		if (index < 0) {
		//			say_error("Problems undoing Bammer of something in a box.");
		//			return;
		//		};
		//		cubby->remove_component(index,NO_SELECTION_REASON_GIVEN,0,FALSE);
		//		cubby->initialize_hole(index,previous_under_item);
		//	} else {
		//		smashees_leader->add_follower(previous_under_item);
		//		smashees_leader->remove_follower(original_smashee);
		//	};
		//};
		//original_smashee->remove_from_floor();
		//original_smashee->decrement_ref_count(); // I'm the only one holding on so is really gone -- new on 041002
	 //  original_smashee->destroy();
		original_smashee = NULL;
	};
	if (previous_under_item != NULL && previous_under_item->pointer_to_leader() == NULL && previous_under_item->pointer_to_background() == NULL) { 
		// new on 050701 - added background null check on 310803
		// didn't add it to leader above
		floor->add_item(previous_under_item);
	};
	//if (previous_under_item != NULL) { // new 1000901 -- commented out on 041002
	//	previous_under_item->restore_active(previous_under_item_active);
	//};
	//previous_under_item = NULL; 
   if (previous_under_item_xml != NULL) {
      previous_under_item_xml->Release();
      previous_under_item_xml = NULL;
   };
	if (previous_over_item != NULL) {
		if (previous_over_item->pointer_to_leader() != NULL) { // new on 100901
			previous_over_item->pointer_to_leader()->remove_follower(previous_over_item);
		};
#if TT_XML
  //    Sprite *previous_over_item_copy = xml_load_sprite(previous_over_item_xml);
		//previous_over_item->become(previous_over_item_copy); // new on 100901
		// commented out on 211003 since better to clobber existing when unbamming -- restored soon afterwards
		previous_over_item->release_all_references(); // new on 070803 - otherwise can have copies of followers and maybe worse
//		previous_over_item->remove_all_followers(TRUE); // new on 070803 -- commented out on 020504 since done above now
		UnwindProtect <boolean> set(tt_loading_to_undo_bammer,TRUE); // new on 211003
		boolean aborted = FALSE; // new on 140105 -- ignored (at least for now)
		xml_load_sprite(previous_over_item_xml,aborted,previous_over_item); // rewritten on 111202
      previous_over_item_xml->Release();
      previous_over_item_xml = NULL;
		set_sprites_to_active_now();
#endif
//		previous_over_item->set_active(previous_over_item_copy_active);
//		previous_over_item_copy = NULL;
		floor->add_item(previous_over_item);
		previous_over_item->set_selectable(TRUE);
		previous_over_item->set_to_be_smashed(FALSE);
		previous_over_item->set_slated_for_deletion(FALSE); // new on 111202
		previous_over_item->set_available_for_indexing(TRUE);
		previous_over_item = NULL;
	};
	set_parameter(MOUSE_AFTER_SMASH);
	set_cycle_stopped_callback(UNDO_GET_SMASHED_CALLBACK);
}; 

void Mouse::am_smashed() {
	if ((item_to_smash->slated_for_deletion() && !item_to_smash->to_be_smashed()) || 
		 (item_underneath->slated_for_deletion() && !item_underneath->to_be_smashed())) {
		// new on 191202 to deal with cases where these were to be destroyed - e.g. bomb went off on the back
		// added checks for to_be_smashed so dropping 2 things on something doesn't mess up the mouse
		abort();
		return;
	};
	recompute_mouse_priority();
   ::completed(saved_by); // replaces the following as of 171102
//	if (action_status != NULL) {
////		if (ok) {
//			completed(action_status); // no harm done if already aborted via set_to_future_value
////		} else {
////			by->abort_action(action_status,S(IDS_ABORT_MOUSE_TROUBLES));
////		};
//	};
	city_coordinate distance_to_right_edge, final_x, final_y;
	city_coordinate item_llx,item_lly;
	item_to_smash->lower_left_corner(item_llx,item_lly);
	distance_to_right_edge = ideal_screen_width-item_llx;
	if (distance_to_right_edge > item_lly) { // lly is distance to bottom edge
		final_x = (distance_to_right_edge-item_lly)+
					 (mouse_offscreen_right-ideal_screen_width);
		final_y = mouse_offscreen_bottom;
	} else {
		final_x = mouse_offscreen_right;
//#if TT_BETA
//		if (tt_running_old_demos) {
//			final_y = (item_lly-distance_to_right_edge)-mouse_offscreen_bottom;
//		} else
//#endif
		final_y = (item_lly-distance_to_right_edge)+mouse_offscreen_bottom;
	}; 
	set_parameter(MOUSE_RUNNING_SOUTHEAST);
   // could this need tt_running_old_demos to restore 2000?
	// was floor->default_duration(1000) but that interferes with finish_instantly
	animate_location(final_x,final_y,default_duration(1000),this,REMOVE_MOUSE_CALLBACK);  // was 2000 ms
//	item_underneath->mouse_done();
	if (delete_smashee && item_to_smash != NULL) { // second condition added 180105 in case already released (but probably a bug introduced yesterday)
		item_to_smash->finish_instantly(); // new on 141204 -- e.g. a nest with an egg dropped and the bird is still hatching
		if (item_to_smash->kind_of() != NEST) { // conditional new on 160904 since if smashing nests together don't throw away references
			item_to_smash->release_all_references(); // new on 140904 so nests and the like aren't available if around just to undo a bam
		};
//		item_to_smash->recursively_set_slated_for_deletion(TRUE); // added and removed on 140904
//		item_to_smash->destroy(saved_by); // commented out on 140904 since set_item_to_smash does this now
//		item_to_smash = NULL; // for robustness -- moved this below since should happen unconditionally
	};
//	item_to_smash = NULL; // finished using it new on 070203 so not dumped
	set_item_to_smash(NULL); // rewritten on 140904 to deal with ref counting correctly
};

void Mouse::undo_am_smashed(Floor *current_floor) { // new on 050701
	if (original_smashee->return_ref_count() == 1) { // I'm the only one holding on so is really gone -- why test this here??
		original_smashee->destroy();
      original_smashee = NULL; // new on 041002
//		if (previous_mouse != NULL) {
//			previous_mouse->destroy();
//			previous_mouse = NULL;
//		};
		if (floor != NULL) { // new version on 080701
			floor->set_previous_mouse(NULL);
		};
		return;
	} else if (original_smashee->pointer_to_background() != current_floor) {
		return;
   } else if (original_smashee->pointer_to_leader() != NULL && original_smashee->pointer_to_leader()->kind_of() == PROGRAM_PAD) {
      // new on 041002 since dropped in notebook since smash
      release_undo_information();
      return; 
      // commented out on 031002 since this is unexpected -- e.g. add something to running game and then want to remove it
      // but it does mean if F11 is called much later strange things may happen
//	} else if (!equal(original_smashee,smashee_copy)) { // new on 090701 so don't undo if value has been changed again
//		return;
	};
	original_smashee->pointer_to_background()->add_item(this,TRUE,FALSE,FALSE); // added args on 111202 so no warnings if already there
	set_parameter(MOUSE_RUNNING_SOUTHEAST);
	city_coordinate original_smashee_llx,original_smashee_lly;
	original_smashee->lower_left_corner(original_smashee_llx,original_smashee_lly);
//	original_smashee->recursively_set_slated_for_deletion(FALSE);
	// new on 140904 to restore this and its parts (e.g. nests) // added and removed on 140904
	animate_location(original_smashee_llx-current_width()/2,original_smashee_lly-current_height()/6,
						  default_duration(1000),this,UNDO_DO_OPERATION_CALLBACK); // was floor->default_duration(1000)
};

void Mouse::remove_mouse() {
	if (floor != NULL) { // for robustness added 230804 -- apparently can happen if time travel catches the mouse at the end
		floor->remove_item(this);
	};
   no_background(); // shouldn't be needed but doesn't do any harm
//	tt_deleted_sprites = new Sprites(this,tt_deleted_sprites); // commented out ages ago?
	// conditional removed on 080202 since this was preventing garbage collection of both the mice and the smashee
//	if (previous_under_item == NULL) { 
	// rewritten on 120701 to work with multiple Bammers - was (by == NULL || by->kind_of() == ROBOT) { // not undoable
 		destroy();
//	};
//	stop_postponing_new_logs();
};
	
void Mouse::recompute_mouse_priority() {
   if (floor != NULL && floor->in_the_room()) return; // handled differently if room view
   if (priority() == -3) return; // stay up there 
	if (item_to_smash->priority() < 0 ||
		 tt_programmer->part_of_selection(item_to_smash)) {
		// need to get on top of things...
		set_priority(-3); //over_item->priority()-1);
	} else {
		set_priority(0); //-2); // runs on top of most anything
	};
};

void Mouse::set_original_smashee(Sprite *the_original_smashee) {
	original_smashee = the_original_smashee;
	if (original_smashee != NULL) {
		original_smashee->increment_ref_count(this);
	};
};

void Mouse::set_previous_over_item(Sprite *the_previous_over_item) {
	previous_over_item = the_previous_over_item; 
	if (previous_over_item != NULL) {
		previous_over_item->increment_ref_count();
	};
};

void Mouse::set_item_to_smash(Sprite *the_item_to_smash) { 
	// new on 171202 since need to ref count these things in case robots on the back blow them up
	if (item_to_smash != NULL) {
		item_to_smash->destroy(); // release old reference
	};
	item_to_smash = the_item_to_smash; 
	if (item_to_smash != NULL) {
		item_to_smash->increment_ref_count();
	};
};

void Mouse::set_item_underneath(Sprite *the_item_underneath) { 
	// new on 171202 since need to ref count these things in case robots on the back blow them up
	item_underneath = the_item_underneath; 
	if (item_underneath != NULL) {
		item_underneath->increment_ref_count();
	};
};

#if TT_XML

void Mouse::xml(xml_element *element, xml_document *document) {
   // checked on 121202 that all state varaiables included here
   if (save_copy) xml_set_attribute(element,L"SaveCopy",1);
	if (delete_smashee) xml_set_attribute(element,L"DeleteSmashee",1);
	xml_set_attribute(element,L"SmasheeCenterX",smashee_center_x);
	xml_set_attribute(element,L"SmasheeCenterY",smashee_center_y);
	xml_set_attribute(element,L"PreviousInitialX",previous_initial_x);
	xml_set_attribute(element,L"PreviousInitialY",previous_initial_y);
	if (previous_smashee_center_x != min_long) {
		xml_set_attribute(element,L"PreviousSmasheeCenterX",previous_smashee_center_x);
	};
	if (previous_smashee_center_y != min_long) {
		xml_set_attribute(element,L"PreviousSmasheeCenterY",previous_smashee_center_y);
	};
	if (item_underneath != NULL) {
      item_underneath->add_xml(element,L"ItemUnderneath",document);
   };
   if (item_to_smash != NULL) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 160703) {
			tt_error_file() << "Generating ItemToSmash XML for #" << item_to_smash->debug_counter << endl;
		};
#endif
//      xml_element *item_to_smash_element = item_to_smash->add_xml(element,L"ItemToSmash",document);
		// replaced above with following on 200804 to move xml_append_child to end
		xml_element *item_to_smash_element = create_xml_element(L"ItemToSmash",document);
		item_to_smash->xml_create_and_append_element(item_to_smash_element,document,TRUE)->Release(); // missing item_to_smash-> prior to 240804
		if (item_to_smash->pointer_to_leader() != NULL) {
			xml_set_attribute(item_to_smash_element,L"FollowsItemUnderneath",1); // not clear if this ever happens anymore - observed on 150703
		};
		xml_append_child(item_to_smash_element,element);

#if TT_DEBUG_ON
		if (tt_debug_mode == 160703) {
			tt_error_file() << "Done generating ItemToSmash XML for #" << item_to_smash->debug_counter << endl;
		};
#endif
   };
	if (original_smashee != NULL) {
      original_smashee->add_xml(element,L"OriginalSmashee",document);
   };
	if (previous_over_item != NULL) {
      previous_over_item->add_xml(element,L"PreviousOverItem",document);
   };
	if (previous_under_item_xml != NULL) {
		xml_element *previous_under_item_xml_element = create_xml_element(L"PreviousUnderItemEncoding",document);
//		xml_append_child(first_node_that_is_an_element(previous_under_item_xml),previous_under_item_xml_element);
		// rewritten on 100603 since this causes strange crashes and isn't a good anyway
		xml_append_child(xml_copy_first_element(previous_under_item_xml),previous_under_item_xml_element); // rewritten on 201003
		xml_append_child(previous_under_item_xml_element,element); // moved here on 190804
//		xml_append_child(xml_copy_document(previous_under_item_xml),previous_under_item_xml_element); // xml_copy_node is new on 140603
//#if TT_DEBUG_ON
//		xml_debug_node(previous_under_item_xml);
//#endif
		// backed off from the following on 140603 since it caused many problems - e.g. with sprites_to_set_active
//		Sprite *previous_under_item = xml_load_sprite(previous_under_item_xml);
//		if (previous_under_item != NULL) {
//			previous_under_item->add_xml(element,L"PreviousUnderItemEncoding",document);
//			remove_from_active_sprites(previous_under_item); // new on 110603
////			previous_under_item->destroy(); // commenting out is an experiment on 130603
//		};
	};
	if (previous_over_item_xml != NULL) {
		xml_element *previous_over_item_xml_element = create_xml_element(L"PreviousOverItemEncoding",document);
//		xml_append_child(first_node_that_is_an_element(previous_over_item_xml),previous_over_item_xml_element);
		xml_append_child(xml_copy_first_element(previous_over_item_xml),previous_over_item_xml_element); // rewritten on 201003
		xml_append_child(previous_over_item_xml_element,element); // moved here on 190804
//		xml_append_child(xml_copy_document(previous_over_item_xml),previous_over_item_xml_element);
//		Sprite *previous_over_item = xml_load_sprite(previous_over_item_xml);
//		if (previous_over_item != NULL) {
//			previous_over_item->add_xml(element,L"PreviousOverItemEncoding",document);
//			remove_from_active_sprites(previous_over_item); // new on 110603
////			previous_over_item->destroy(); // commenting out is an experiment on 130603
//		};
	};
   Sprite::xml(element,document);
};

boolean Mouse::handle_xml_tag(Tag tag, xml_node *node) {
	switch (tag) { 
		case ITEM_TO_SMASH_TAG: 
			set_item_to_smash(xml_get_sprite(node,item_to_smash));
			if (xml_get_attribute_int(node,L"FollowsItemUnderneath",0) && item_underneath != NULL && item_to_smash != NULL
				 && item_to_smash->pointer_to_leader() == NULL) { // test new on 220203 since Yishay had a city where this wasn't true
				// new on 131202 to reconstruct temporary leader/follower relationship
				item_underneath->add_follower(item_to_smash,FALSE,INSERT_BY_PRIORITY,TRUE);
//				item_underneath->really_propagate_changes(LOADING_GEOMETRY);
			};
			return(TRUE);
		case ITEM_UNDERNEATH_TAG:
			set_item_underneath(xml_get_sprite(node,item_underneath));
			return(TRUE);
		case ORIGINAL_SMASHEE_TAG:
			set_original_smashee(xml_get_sprite(node,original_smashee));
			return(TRUE);
		case PREVIOUS_OVER_ITEM_TAG:
			set_previous_over_item(xml_get_sprite(node,previous_over_item));
			return(TRUE);
		case PREVIOUS_UNDER_ITEM_ENCODING_TAG:
			if (previous_under_item_xml != NULL) { // new on 070103
				previous_under_item_xml->Release();
			};
			previous_under_item_xml = create_xml_document();
			xml_append_child(first_node_that_is_an_element(node),previous_under_item_xml); // ok? asked 100603
			return(TRUE);
		case PREVIOIUS_OVER_ITEM_ENCODING_TAG:
			if (previous_over_item_xml != NULL) { // new on 070103
				previous_over_item_xml->Release();
			};
			previous_over_item_xml = create_xml_document();
			xml_append_child(first_node_that_is_an_element(node),previous_over_item_xml);
			return(TRUE);
      case NO_MORE_TAGS:
         save_copy = xml_get_attribute_int(node,L"SaveCopy",0); // prior to 210404 save_copy);
			delete_smashee = xml_get_attribute_int(node,L"DeleteSmashee",0); // prior to 210404 delete_smashee
			smashee_center_y = xml_get_attribute_int(node,L"SmasheeCenterX",min_long); // prior to 210404 smashee_center_y -- wrong!
			smashee_center_x = xml_get_attribute_int(node,L"SmasheeCenterY",min_long); // prior to 210404 smashee_center_x
			previous_initial_x = xml_get_attribute_int(node,L"PreviousInitialX",previous_initial_x);
			previous_initial_y = xml_get_attribute_int(node,L"PreviousInitialY",previous_initial_y);
			previous_smashee_center_x = xml_get_attribute_int(node,L"PreviousSmasheeCenterX",min_long); // prior to 210404 previous_smashee_center_x);
			previous_smashee_center_y = xml_get_attribute_int(node,L"PreviousSmasheeCenterY",min_long); // prior to 210404 previous_smashee_center_y);
         // and do the following
      default:
         return(Sprite::handle_xml_tag(tag,node));
   };
};

#endif
