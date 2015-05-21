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
#if !defined(__TT_TOOLS_H)   
#include "tools.h"
#endif   
#if !defined(__TT_CUBBY_H)
#include "cubby.h"
#endif   
#if !defined(__TT_BIRD_H)
#include "bird.h"
#endif
#if !defined(__TT_MARTIAN_H)   
#include "martian.h"
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
#if !defined(__TT_PAD_H)
#include "pad.h"
#endif
#if !defined(__TT_PUZZLE_H)
#include "puzzle.h"
#endif
#if !defined(__TT_PRGRMMR_H)
#include "prgrmmr.h"
#endif
//#if !defined(__TT_HYPERBOT_H)
//#include "hyperbot.h"
//#endif
#if !defined(__TT_PRGRMMR_H)
#include "prgrmmr.h"
#endif
#if !defined(__TT_UTILS_H)
#include "utils.h"
#endif 
#if !defined(__TT_LOG_H)
#include "log.h"
#endif
#if !defined(__TT_BCKGRND_H)   
#include "bckgrnd.h"
#endif   
#if !defined(__TT_FLOOR_H)
#include "floor.h"
#endif
#if !defined(__TT_WINMAIN_H)   
#include "winmain.h"
#endif   
#if !defined(__TT_USER_H)
#include "user.h" // for last_mouse_deltas
#endif
#if !defined(__TT_HELP_H)
#include "help.h"
#endif
#if !defined(__TT_JAVA_H)
#include "java.h"
#endif
//#include "martian.h"
#if !defined(__TT_PICTURE_H)
#include "picture.h"
#endif
#if !defined(__TT_REMOTE_H)
#include "remote.h"
#endif
#if !defined(__TT_ROBOT_H)
#include "robot.h"
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

#include <string.h>
#if !TT_32
#include <stdlib.h> // for max
#endif
//#include <strstrea.h> commented out 210703

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// if changed update help.cpp
enum PictureHistory {PICTURE_ADDED_PICTURE=0,
							PICTURE_REMOVED_PICTURE,
							PICTURE_CHANGED_BY_KEYBOARD,
							PICTURE_COLOR_BY_KEYBOARD,
							PICTURE_TOSSED,
							PICTURE_FLIPPED,
							PICTURE_FLIPPED_BACK,
//							PICTURE_NOTEBOOK_COPIED=128,
							PICTURE_CUBBY_OR_ROBOT_ADDED,
							PICTURE_CUBBY_OR_ROBOT_REMOVED,
							PICTURE_REDIRECTED,
							PICTURE_EXPANDER_HELP_GIVEN,
							PICTURE_BLANKED,
							PICTURE_BLANKED_TB,
							PICTURE_BLANK_EXPLAINED,
							SCALE_USED,
							SCALE_KEYBOARDED};

// add_cubby_callback already used in truck.cpp
//void add_dropped_cubby_callback(void *p) {
//	((Picture *) p)->add_dropped_cubby();
//};
//void add_dropped_robot_callback(void *p) {
//	((Picture *) p)->add_dropped_robot();
//};
//void set_size_to_that_of_indirection_callback(void *p) {
//	((Picture *) p)->set_size_to_that_of_indirection();
//};

//user_coordinate a_bit_more(user_coordinate x);

Picture::Picture(city_coordinate x, city_coordinate y,
					  long initial_priority,
					  short int image_index, // sprite::sprite should be checking that this is valid
					  SpriteCode image_code,
//					  Background *floor,
					  city_coordinate initial_width,
					  city_coordinate initial_height) :
	Sprite(image_code,x,y,PICTURE,
			 image_index,ground_scale,initial_width,initial_height),
//	image_index(image_index),
//	image(image),
//	controlling_birds(NULL),
//	remote_cubbys(NULL),
//	remote_robots(NULL),
//	remote_cubby_count(0),
	remote(NULL),
	flipped(FALSE),
	temporarily_flipped(FALSE),
	temporarily_unflipped(FALSE),
	suspended_flag(FALSE),
   stopped_flag(TRUE),
	followers_on_other_side(NULL),
	robots_running_on_back_flag(FALSE),
	remote_notebook(NULL),
//	saved_action_status(NULL), // made obsolete on 171102
	x_speed(NULL),
	y_speed(NULL),
	llx_accumulated_error(0),
	lly_accumulated_error(0),
//	picture_llx(0),
//	picture_lly(0),
	old_parameter(-2), // impossible value
	old_cycle_stopped(FALSE),
	old_grasped(FALSE),
	old_selected_flag(FALSE),
	released_last_frame(FALSE),
	released_flag(FALSE),
   show_some_if_subpicture_flag(FALSE),
	subpictures_with_notebooks(NULL),
	subpicture_flag(FALSE),
	// could be restored if remote control notebooks have different number of pages
//	page_number_of_first_subnotebook(0),
//	mouse(NULL),
//   item_to_add(NULL),
	backside_background(NULL),
	indirection(NULL),
	controlee(NULL),
//	saved_by(NULL),
	saved_indirection(NULL),
	flip_going_on_flag(FALSE),
//	ignore_controlee_when_size_changes_flag(FALSE), // noticed obsolete on 261102
	remote_looks_of_me_count(0),
//	copying_flipped_followers_of_controlee_flag(FALSE), // obselete 151200
//	postponed_copy_flipped_followers_of_controlee_flag(FALSE),
	old_width(min_long), // new on 070800 to indicate unset old values
	width_before_flip(min_long), // wasn't initialized prior to 261102
	placed_new_items_flag(FALSE),
   saved_width(-1), // not initialized prior to 051102 -- commented out around 180405 (why??) and restored 210405
   saved_height(-1),
	programmer_state_before_flip(PROGRAMMER_AT_FLOOR),
	label(NULL),
	version_number(tt_xml_version_number) {
//	frame_flipped_followers_changed_last(-1) // new on 161200 to know when remote looks need to update their flipped followers
//	last_size_change_due_to_indirection_flag(FALSE) {
    set_is_picture(TRUE);
	 current_priority = initial_priority;
//	 set_priority_function_of_lly(FALSE);
	 // more efficient to do subtest after seeing it is blank
	 // and it is sorta blank already
// but it messes up lots of other code
//	 if (code == SYNTHETIC_SPRITE) { // its width and height are the truth
//		 blank = TRUE;
//	 };
//	 set_size_is_true(TRUE);
    if (initial_width > 0) { // condition added on 051102
       save_size();
    };
	 old_llx = llx;
	 old_lly = lly;
};
	
void Picture::set_slated_for_deletion(boolean new_flag) {
	// new on 120200 since sometimes destruction is delayed (e.g. page flipping in notebooks)
	// and yet remote controls can be broken as a consequence (saved while finish_instantly really destroyed controlee)
	if (slated_for_deletion()) return; // new on 170300
	// moved earlier on 241003 so those called via controlee_deleted know this is going away
	Sprite::set_slated_for_deletion(new_flag); // new on 170300
	if (new_flag && !tt_resetting_or_destroying_city) { 
		// was tt_shutting_down prior to 211200 - replaced with tt_resetting_or_destroying_city on 100603
		// what about tt_reclaiming_memory?
		// following moved from prepare_for_deletion
		stop_all(TRUE); // if robots are running on back
		if (remote != NULL) {
			remote->controlee_deleted();
		};
		if (controlee != NULL) {
			controlee->remove_remote(APPEARANCE_REMOTE,this);
//			controlee->recursively_set_background(NULL);
		};
	};
	release_remote_notebook(); // new on 050201 -- removed this on 060204
	// moved earlier on 241003 so those called via controlee_deleted know this is going away
//	Sprite::set_slated_for_deletion(new_flag); // new on 170300
};

void Picture::release_remote_notebook() { // prior to 060204 was (Sprite *by)
	// made into a method on 050201
	// following was commented out because "taken care of by remotes"
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("release_remote_notebook of debug target");
	};
#endif
	if (remote_notebook != NULL && 
		 !remote_notebook->slated_for_deletion()) { 
		// it is not "ok_not_to_delete" so should be left for picture to deal with
		if (!tt_resetting_or_destroying_city && floor != NULL && is_flipped()) {
			// tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 but can cause memory errors while resetting during time travel
			floor->remove_item(remote_notebook,FALSE);
		};
		if (tt_shutting_down) { // restored on 251000 in the case of shutting down since picture:set_slated_for_deletion doesn't do this then
			remote_notebook_deleted(remote_notebook); // commented out on 181000 since taken care of by picture:set_slated_for_deletion
		};
		remote_notebook->just_set_picture(NULL);
		if (!is_flipped()) {
			remote_notebook->no_background(); // added 1/10/99 - is this always the right thing to do here?
			// problem is that floor may have been destroyed by the time this is run...
			//if (tt_shutting_down) { // restored on 251000 in the case of shutting down - commented out on 160103
			//	remote_notebook->destroy(by); // commented out on 170300 since is destroyed by remote_notebook_deleted call above
			//};
		};
		// following removed on 170405 since picture's don't add a ref count for remote_notebook (but maybe should?)
		//if (remote_notebook->pointer_to_background() == NULL) {
		//	// conditional new on 090203 since when notebook goes away it removes itself from this picture
		//	// but otherwise are independent -- if just a local cache then destroy it
		//	remote_notebook->destroy(); // new on 160103 // removed by on 060204
		//};
	};
	remote_notebook = NULL; // new on 050201 
};

void Picture::re_initialize() { // new on 250203 -- used by GlobalPicture for time travel now
	release_all_references();
	if (remote != NULL) {
		delete remote;
		remote = NULL;
	};
	Sprite::re_initialize(); // new on 160204
};

// combined the protocols on 020504
//void Picture::release_outgoing_connections() { // new on 120204 -- why have both protocols?
//	release_all_references();
//};

void Picture::prepare_for_deletion() {
	// is this OK or should all this get merged with release_all_references? - asked 130504
   if (preparing_for_deletion) return; // already done
	preparing_for_deletion = TRUE; // new on 121103 since can be called recursively (e.g. via delete remote)
#if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {  
      log("Preparing to delete debug target (picture).");
   };
#endif
//	release_all_references(); // removed by on 060204 -- called by Sprite::prepare_for_deletion as of 020504
//	release_remote_notebook(); // removed by on 060204 -- moved into release_all_references on 300604
	if (remote != NULL) {
		delete remote;
		remote = NULL;
	};
//	if (followers != NULL) {
//		delete_followers();
//		followers = NULL;
//	};
//	prepared_for_deletion_flag = FALSE; // new on 121103 restore so following doesn't get confused -- 
	// replaced on 020604 by TRUE below
	remove_from_tt_pictures_to_activate(this);
	Sprite::prepare_for_deletion();
	if (backside_background != NULL) {
//		backside_background->remove_all(); 
		// commented out on 080400 since background::prepare_for_deletion does this fine (called by destroy)
//		delete backside_background; // should this be ->destroy() instead??
		backside_background->destroy(); // new on 060400
      backside_background = NULL;
	};
	if (controlee != NULL) { // new on 150200
		controlee->update_remote_looks_of_me_count(-1);
	};
	// commented out the following on 070400 since this should only happen if called directly (not via destroy)
//	if (!tt_shutting_down) remove_from_floor(FALSE); // new on 280300 - condition added on 050400
	if (tt_log_version_number < 22) remove_from_floor(FALSE); // remove these sometime?? - asked 191102
	if (x_speed != NULL) { // new on 160103
		delete x_speed;
	};
	if (y_speed != NULL) { // new on 160103
		delete y_speed;
	};
};

void Picture::release_all_references() { // removed (Sprite *by) on 060204 // on 090400 pulled out pieces of the prepare_for_deletion
	if (releasing_all_references) { // new on 230305
		return;
	};
	if (followers_on_other_side != NULL) {
		boolean picture_in_vacuum = inside_vacuum(); // new on 140605 since got a crash (Gordon email) from remove_from_floor below 
		Sprites *remaining = followers_on_other_side;
		while (remaining != NULL) {
			Sprite *sprite = remaining->first();
			// following caused other problems -- now dealt with by remotes
			if (!tt_resetting_or_destroying_city && !picture_in_vacuum) { 
				// conditional had been commented out but does stop crashes on exit -- was tt_shutting_down prior to 211200
				// became tt_resetting_or_destroying_city on 100603
				// what about tt_reclaiming_memory?
				// if shutting down might be a remote integer that
				// tries to talk to picture which is already gone
				//	sprite->set_background(NULL,TRUE);
				// replaced above with the following on 080400 and 100400
				sprite->stop_all(TRUE);
				if (is_flipped()) {
					sprite->remove_from_floor(FALSE,FALSE,FALSE);
				} else {
					sprite->set_background(NULL,TRUE,FALSE);
				};
			};
			if (!sprite->destroy()) { // removed by on 060204
#if TT_DEBUG_ON
				if (!tt_resetting_or_destroying_city && sprite->kind_of() != ROBOT && sprite->kind_of() != NEST) { 
					// robots are often refered to by tt_queue, etc.
					// added NEST on 280201 since birds can hold on...
					log("Warning: possible memory leak since picture is being destroyed but something on the back isn't.");
					sprite->print(tt_error_file());
					tt_error_file() << endl;
				};
#endif
			};
			remaining->set_first(NULL);
			remaining = remaining->rest();
		};
		delete followers_on_other_side;
		followers_on_other_side = NULL;
	};
	if (subpictures_with_notebooks != NULL) {
		subpictures_with_notebooks->remove_all();
		delete subpictures_with_notebooks;
		subpictures_with_notebooks = NULL;
	};
	if (indirection != NULL) {
//		delete indirection;
		if (!indirection->destroy()) { // removed by on 060204
			if (!indirection->destroy()) { // new on 160103 since nothing else should be holding on to it // removed by on 060204
#if TT_DEBUG_ON
				if (!about_to_quit()) { // otherwise no need to consider warning - 171203
					log("Picture of something else not destroyed. Possible memory leak.");
				};
#endif
			};
		};
		indirection = NULL;
	};
	if (saved_indirection != NULL) {
		saved_indirection->destroy(); // removed by on 060204
		saved_indirection = NULL;
	};
	if (label != NULL) { // new on 110604
		if (comment != NULL) { // new on 120604
			label->remove_follower(comment); // new on 120604
		};
		label->destroy();
		label = NULL;
	};
	release_remote_notebook(); // moved here on 300604 -- might fix bug where remote control notebooks in Dusty refered to destroyed pictures
	Sprite::release_all_references(); // new on 030504
};

Sprite *Picture::now_inside_thought_bubble(boolean in_training) { 
	// new on 260204 so that if remote looks is dropped in a thought bubble of a trained robot it is converted as it would if it were untrained
	Picture *replacement = controlee_replacement();
	if (replacement != NULL) {
		return(replacement);
	};
	return(Sprite::now_inside_thought_bubble(in_training));
};

Picture *Picture::controlee_replacement(boolean also_copy_offsets) { // abstracted on 260204
	if (controlee != NULL) { // commented out the following condition since not true of remote looks
//		 && !tt_copying_for_training) { // this added on 080205 since for training need live remotes 
		// && !is_flipped()) { // && !controlee->is_flipped()) { 
		// is_flipped() is a new condition on 121200 -- commented out on 150202 
		//	turn_off = TRUE; // commented out on 300103
		// changes in the thought bubble should not propagate out
		if (indirection == NULL) { // can it be both a remote control and an indirect?? - asked 300103
			// also don't need to use indirection since controlee is a picture - right??
//				Picture *controlee_copy = (Picture *) controlee->copy();
			// added also_copy_offsets as arg below on 120803
			//if (already_copied(controlee) != NULL) { // new on 151204
			//	return(NULL); // recursive so don't do a replacement
			//};
			// replaced controlee with this since otherwise can have problems with multiple sensors 
			// of the same picture not copying properly
			if (tt_top_copy != NULL) {
				if (tt_previous_controlee_replacements != NULL && tt_previous_controlee_replacements->member_cons(this) != NULL) {
					return(NULL);
				};
				tt_previous_controlee_replacements = new Sprites(this,tt_previous_controlee_replacements);
			} else {
				// added on 040105 to deal with infinite recursions
//				controlee_copy = controlee->top_level_copy(also_copy_offsets); 
				// this being top_level is new on 040105 -- caused non-fatal but confused relationships
				return(NULL); // also new 040105 -- works
			};
			Sprite *controlee_copy = controlee->copy(also_copy_offsets);
//			just_copied(controlee,controlee_copy); // new on 151204
			Picture *replacement;
			if (controlee_copy->kind_of() == PICTURE) {
				replacement = (Picture *) controlee_copy;
			} else {
				// this is what is new as of 181004
				replacement = make_indirect_picture(controlee_copy,FALSE);
			};
			// rewritten on 181004 since the following hasn't been safe since 050504
//			Picture *replacement = (Picture *) controlee->copy(also_copy_offsets); 
			// changed to top_level instead on 120803 but still finds old without the above - 
			// so reverted since followers handled specially below
			//if (also_copy_offsets) {
			//	replacement->copy_leader_offsets(this); // new on 120803 to get the geometry right
			//};
			boolean controlee_flipped = controlee->is_flipped();
			Sprites *controlee_followers;
			if (controlee_flipped == is_flipped()) {
				controlee_followers = controlee->pointer_to_followers();
			} else {
				controlee_followers = controlee->pointer_to_followers_on_other_side();
				if (controlee_flipped) {
					replacement->unflip();
				} else {
					replacement->flip();
				};
			};
			//if ((originally_flipped && !controlee_flipped) ||
			//    (!originally_flipped && controlee_flipped)) { // new on 290103 - thanks Leonel
			//	replacement->flip();
			//};
			if (controlee_followers != NULL) {
				replacement->set_followers_copy(controlee_followers); // new on 300103
			};
//				return(controlee_copy); // new on 300103 - no need for another indirection - right?				
//				copy->set_indirection(controlee_copy,FALSE); // this seems wrong??
			// following copied here -- redundant is some cases but not if called from now_inside_thought_bubble
			replacement->Sprite::set_size_and_location(width,height,llx,lly);
			replacement->set_width_before_flip(width_before_flip); 
			// new on 120803 so can flip remote looks back even if controlee has never been flipped
			replacement->set_height_before_flip(height_before_flip);
			return(replacement);
		};
	};
	return(NULL);
};

Sprite *Picture::copy(boolean also_copy_offsets) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Copying debug target (picture).");
	};
   millisecond start_time = timeGetTime();
#endif
	if (!tt_copying_entire_thought_bubble) { // !tt_copying_entire_thought_bubble new on 151204
		// && controlee == NULL) { // on 201299 added conditional not a remote looks -- commented out a long time 
		// but I'm not sure why - makes sense to not share remote looks, right?
		// commented out !infinite_stack() on 020106 since if a user created a stack then this prevents an infinite recursion
      Sprite *copied = already_copied(this);
      if (copied != NULL) {
//			copied->increment_ref_count(this); // commented out on 311000
			return(copied);
		};
   };
   // commented out on 160702 so that I have better control over activation of pictures from notebooks
   // and I'm guessing the round off errors have been dealt with elsewhere
	//if (leader != NULL && leader->kind_of() == PROGRAM_PAD) {
	//	// a hack to avoid geometry errors due to round off
	//	Sprite *copy = ((Notebook *) leader)->pointer_to_page_i_am_on(this);
	//	copy->set_size_and_location(width,height,llx,lly);
	//	return(copy);
	//};
	boolean originally_flipped = is_flipped();
	if (indirection != NULL && indirection->kind_of() == PICTURE && followers == NULL && no_flipped_followers() && 
		 !originally_flipped) { 
		// new on 241000 - important optimization when copying long chains to collapse them
		return(indirection->copy());
	};
	if (tt_copying_entire_thought_bubble && is_flipped() && followers != NULL && tt_remove_unneeded_things_from_thought_bubble) { 
		// new on 110105 that only the flipped picture but nots its part are copied
		return(shallow_copy());
	};
//	boolean turn_off = FALSE;
	if (tt_copying_entire_thought_bubble && !is_blank()) {
		// commented out the following on 151204 since can cause infinite recursion -- but to avoid the other problem
		// now when tt_copying_entire_thought_bubble Picture doesn't check for copied already
//		UnwindProtect<Sprite *> set(tt_top_copy,NULL); 
		// new on 120803 - otherwise can get picture and its looks mapped to same place
			// scope of the above needs to include set_followers_copy below
		//if (controlee != NULL && indirection == NULL) { // new on 151204
		//	Sprite *copied = already_copied(controlee);
		//	if (copied != NULL) {
		//		return(copied);
		//	};
		//};
		Picture *replacement = controlee_replacement(also_copy_offsets);
		if (replacement != NULL) { // already done
		// changes in the thought bubble should not propagate out
		// commented out on 190902 since now we just copy it as normal but turn it off	
//			restored not is_flipped on 190401 but changed it to be for the remote looks not the controlee
//			Picture *controlee_copy; // = (Picture *) controlee->current_side_copy(); 
			// made current_side_copy on 191200 since don't want stuff on the back or front
//			if (is_flipped() && !controlee->is_flipped()) { // new on 171200
//				controlee->flip(TRUE,FALSE); // new on 190401
//				controlee_copy = (Picture *) controlee->current_side_copy();
//				controlee->unflip(TRUE,FALSE);
////				controlee_copy->flip(); // commented out on 190401
////			} else if (!is_flipped() && controlee->is_flipped()) {
////				controlee->unflip(TRUE,FALSE); // new on 190401
////				controlee_copy = (Picture *) controlee->current_side_copy();
////				controlee->unflip(TRUE,FALSE);
//////				controlee_copy->unflip();
//			} else { // otherwise is in the same flipness state
//				controlee_copy = (Picture *) controlee->current_side_copy();
//			};
//			return(controlee_copy);
			
//		};
		} else if (indirection != NULL) { // commented out on 300103 && followers == NULL) { 
			// does the followers == NULL test make sense now??
			Sprite *indirection_copy = indirection->copy();
			if (indirection_copy->kind_of() == PICTURE) {
				replacement = (Picture *) indirection_copy;
				if (originally_flipped) { // new on 221104
					replacement->flip();
				};
			} else if (!originally_flipped && (indirection_copy->kind_of() == INTEGER || indirection_copy->kind_of() == TEXT)) { 
				// new on 050504 so indirections of sensors in thought bubble are like sensors
				// !originally_flipped test added 221104
				return(indirection_copy);
			} else {
				// added on 240299 since the data type should remain a picture
				// in case something like Remote::changed_appearance coerces it
				Sprite *original_replacement = indirection_copy;
				replacement = make_indirect_picture(indirection_copy,FALSE);
				original_replacement->destroy(); // added on 240101 to decrement ref count since indirection created a new reference
				if (originally_flipped) {
					replacement->flip();
				};			
			};
			if (also_copy_offsets) {
				replacement->copy_leader_offsets(this); // new on 120803 to get the geometry right
			};
//			just_copied(this,replacement); // new on 210203 - breaks infinite recursion -- commented out on 151204
			if (followers && !is_flipped() && !replacement->is_flipped()) { 
				// second and third conditions new on 210203 - breaks infinite recursion and uses fewer resources
				// and don't want to add flipped followers to the wrong side
				replacement->set_followers_copy(followers);
			};
		};
		if (replacement != NULL) {
			replacement->Sprite::set_size_and_location(width,height,llx,lly);
			replacement->set_width_before_flip(width_before_flip); 
			// new on 120803 so can flip remote looks back even if controlee has never been flipped
			replacement->set_height_before_flip(height_before_flip);
			return(replacement);
		}; // otherwise copy as normal
	};
	Picture *copy;
	hash image_hash = return_hash();
	city_coordinate good_width, good_height;
	if (leader == NULL || leader->kind_of() == PICTURE || showing_on_a_screen()) {
		good_width = width;
		good_height = height;
	} else {
		good_size(good_width,good_height); 
		// new on 121203 so if a robot copies a picture off screen it ends up good size to start with
		// e.g. removed from a box
	};
	if (image_hash != NULL) {
		// new on 240303
		ascii_string full_file_name = pointer_to_current_image()->return_full_file_name();
		ascii_string original_file_name = pointer_to_current_image()->return_file_name();
		boolean ok;
//		copy = new UserPicture(full_file_name,ok);
		// starting 140403 warns only if there is no media file name and if original_file_name fails tries with original name
		copy = new UserPicture(original_file_name,ok,TRUE,-1,-1,full_file_name); 
		// starting 280403 pass both private media file name and original here
		if (ok) {
////			copy->set_private_media_file_name(unique_name);
//			if (original_file_name != NULL) {
//				copy->just_set_original_file_name(copy_string(original_file_name)); // copy_string new on 250403
//			};
		} else {
			copy->destroy();
			copy = new UserPicture(original_file_name,ok);
		};
		copy->set_size_and_location(good_width,good_height,llx,lly);
	} else {
		ascii_string user_picture_file_name = return_file_name();
		if (user_picture_file_name != NULL) {
			boolean ok;
			copy = new UserPicture(user_picture_file_name,ok);
			copy->set_size_and_location(good_width,good_height,llx,lly); // _and_location requires reason... - not anymore (240303)
//			copy->move_to(llx,lly);
		} else {
			copy = new Picture(llx,lly,//center_x,center_y, //llx+width/2,lly+height/2,
									current_priority-1,
									parameter,code, //image_index,image_code,
									good_width,good_height);
		};
	};
	// removed conditional on 020106 since otherwise have an infinite recursion (see Leonel's email of 291205)
//   if (!infinite_stack()) { // && controlee == NULL) { // on 201299 added conditional not a remote looks
		just_copied(this,copy);
//	};
	// not sure why the following is so soon but beginning 250103 we need to prevent it from copying followers
	//if (followers != NULL) { // commented out on 270103
	//	copy->initialize_followers(followers); // will be clobbered below so should be safe
	//};
//   finish_making_copy(copy,also_copy_offsets); // moved to the end on 270103
	//if (comment != NULL) {
	//	copy->set_comment(comment->copy());
	//};
	// this works even if update_display generates an offset
//	copy->move_to(llx,lly);
	if (saved_width != 0) {
		copy->set_saved_size(saved_width,saved_height);
	};
	copy->set_version_number(version_number); // new on 210506
	if (code == SCALE_SPRITE) { // no need to do all the following
      if (infinite_stack()) { // hack so scales don't vascilate within ToolBox
         copy->set_parameter(VASCILATE_SCALE);
      };
		finish_making_copy(copy,also_copy_offsets); // copied here on 270103
		return(copy);  // no point calling just_copied
	};
//	copy->set_extra_pointer_while_inaccessible(extra_pointer_while_inaccessible());
	if (indirection != NULL) {
      Sprite *indirection_copy = indirection->copy();
      indirection_copy->set_show_all(show_all());
      copy->set_indirection(indirection_copy,FALSE);
	};
//	copy->set_copying_flipped_followers_of_controlee(copying_flipped_followers_of_controlee_flag);
//	copy->set_postponed_copy_flipped_followers_of_controlee(postponed_copy_flipped_followers_of_controlee_flag);
	// add copies of each to copy AND preserve order
   // if copying with wand in "original" mode don't restore things
   // on the back of pictures
//   boolean need_to_flip_original; // noticed obsolete on 110105
	boolean ok_to_copy_any_followers = (!tt_copying_entire_thought_bubble || !blank || tt_copy_restores_pre_blank || 
													!tt_remove_unneeded_things_from_thought_bubble);
	// above pulled out on 100105 since reused a few places now
	// !tt_remove_unneeded_things_from_thought_bubble added on 110105
   if (((is_flipped() && tt_log_version_number >= 22) || !tt_copying_entire_thought_bubble) && !no_flipped_followers()) {
		// first disjunct added on 050400 since if flipped picture is in thought bubble its followers should be copied
		if (controlee == NULL && ok_to_copy_any_followers) { //  && !tt_copying_entire_thought_bubble) { 
			// second conjunct new on 151204 since if erased and in a thought bubble doesn't need flipped followers
			// restored on 221299 so that only do this when not remote looks since they get flipped followers when needed
			// commented out on 211299 since caused bad copies and they aren't different anymore
			// second condition added 181200 since if in thought bubble no need for flipped followers - 
			// unless already flipped (commented out on 191200)
			copy_flipped_followers(copy);
//		} else { // new on 201299
//			copy_flipped_followers_of_controlee(copy);
		};
//		need_to_flip_original = TRUE;
	//} else {
	//	need_to_flip_original = FALSE; // new on 201299 and the check that there are some flipped_followers
	};		
   Picture *saved_copying_insides_for = tt_copying_insides_for;
	tt_copying_insides_for = copy;
	// moved earlier since add_follower read this...
	copy->set_old_x_offset(old_x_offset);
	copy->set_old_y_offset(old_y_offset);
	Sprites *remaining = unflipped_followers();
	if (remaining != NULL && ok_to_copy_any_followers) {
		// following optimized on 250103 so it doesn't search down to find the end of the followers but constructs them efficiently
		// second conjunct new on 151204 since if erased and in a thought bubble doesn't need followers
		Sprites *copy_followers = new Sprites();
		Sprites *last_followers_cons = copy_followers;
		while (remaining != NULL) {
			Sprite *follower = remaining->first();
			Sprite *copy_follower = follower->copy(TRUE);
			copy_follower->set_infinite_stack(follower->infinite_stack()); // new on 080400
//			copy->add_follower(copy_follower,TRUE,INSERT_AT_END,TRUE); 
			// args added on 250103 so that leader_offsets aren't computed here
//			copy_follower->copy_leader_offsets(follower); 
			// typically the original has "more accurate" relative offsets and the like - done by copy(TRUE) now - 260103
			if (copy_follower->kind_of() == PICTURE) {
				copy_follower->set_subpicture(TRUE,FALSE); // second arg added 190500
			};
			copy_follower->set_priority(follower->priority());
			if (!originally_flipped && follower->completely_inside_leader()) { 
				// copied here on 250103 from Picture::add_follower since that isn't called anymore
				copy_follower->become_part_of_picture(FALSE); // added FALSE on 170803 since this shouldn't mess with show_all
			};
			copy_follower->set_leader(copy); // missing prior to 270103
			if (copy_follower->pointer_to_leader_offsets() == NULL) { // e.g. flipped
				copy_follower->record_offsets(); // new on 270103
			};
			remaining = remaining->rest();
			last_followers_cons->set_first(copy_follower);
			if (remaining != NULL) {
				Sprites *new_last_followers_cons = new Sprites();
				last_followers_cons->set_rest(new_last_followers_cons);
				last_followers_cons = new_last_followers_cons;
			};
		};
		copy->initialize_followers(copy_followers);
	};
	tt_copying_insides_for = saved_copying_insides_for;
	if (originally_flipped) { 
//		if (need_to_flip_original) // I don't understand why I need to flip again when FALSE but I do
		flip(TRUE,FALSE); //(leader==NULL)); // changed on 100100 since flipped pictures sometimes got the wrong size when unflipped
		set_temporarily_flipped(FALSE); // or should it save the original state of this flag at the beginning??
//		if (!is_temporarily_flipped()) { // conditional new on 181200 -- removed 191200
			copy->flip(FALSE,(leader==NULL));
			copy->set_width_before_flip(width_before_flip);
			copy->set_height_before_flip(height_before_flip);
			copy->set_programmer_state_before_flip(programmer_state_before_flip);
//		};
	}; // else leave it unflipped
	// restore in case changed
//	clean_status = original_clean_status;
	if (x_speed != NULL) {
		copy->set_x_speed(x_speed->copy());
	};
	if (y_speed != NULL) {
		copy->set_y_speed(y_speed->copy());
	};
   copy->set_priority_fixed(priority_fixed_flag);
   // moved here on 300499 so that controlee isn't set when adding flipped followers 
	// since that would trigger code in picture::add_follower
	if (controlee != NULL) {
		// commented out on 111200
		// moved this much earlier on 300103
		//if (tt_copying_entire_thought_bubble && !controlee->is_flipped()) { // restored on 121200 and added second condition
		//	// changes in the thought bubble should not propagate out
		//	if (indirection == NULL) {
		//		Picture *controlee_copy = (Picture *) controlee->copy();
		//		if ((originally_flipped && !controlee_copy->is_flipped()) ||
		//		    (!originally_flipped && controlee_copy->is_flipped())) { // new on 290103 - thanks Leonel
		//			controlee_copy->flip();
		//		};
		//		copy->set_indirection(controlee_copy,FALSE); // this seems wrong??
		//	};
  //    } else 
		if (!tt_copying_entire_thought_bubble && controlee->part_of_top_level_copy()) {
			// added !tt_copying_entire_thought_bubble on 030105 since otherwise can blow the stack
         copy->set_controlee((Picture *) controlee->copy()); // sharing is enabled so should work fine
		} else if (tt_copying_insides_for != NULL &&
					  controlee->ultimate_container() == ultimate_container()) {
			// I'm being copied along with my controlee so tt_copying_insides_for
			// should be bound to the copy of my controlee
			copy->set_controlee(tt_copying_insides_for);
		} else {
			copy->set_controlee(controlee);
		};
	};
	if (!tt_copying_entire_thought_bubble && ok_to_activate()) { // new on 141199 -- was just active() prior to 160702
		// !tt_copying_entire_thought_bubble added on 070400
		copy->activate(FALSE); 
		// added 1 as arg on 091100 since robots need to be delayed a frame since stuff is still being constructed
		// args removed on 091203 since is only behavior now
	};
	// moved following here from conditional above on 040100 so inactive pictures are copied as inactive
//	copy->set_active(active(),FALSE); // second arg was TRUE prior to 141299 but that is wasteful 
	// -- commented out 170702 since taken care of by finish_making_copy
	if (is_temporarily_flipped()) {
		copy->unflip(); // doesn't count -- new on 181200
	} else if (!originally_flipped) { // conditional added 230101 so it doesn't happen twice
		copy->set_flipped(is_flipped()); // added 181200 since sometimes is needed
	};
//	copy->set_temporarily_flipped(FALSE); // added 181200 since sometimes is needed
#if TT_DEBUG_ON
   if (tt_debug_mode == 300702) {
      tt_error_file() << "Took " << (timeGetTime()-start_time) << "ms to copy ";
      print(tt_error_file());
      tt_error_file()  << endl;
   };
#endif
	//if (turn_off) { // new on 190902
	//	copy->stop_all(TRUE); // for good is right, right??
	//};
	// last arg added on 100105 -- avoids a very heavy overhead occassionally for things in thought bubbles
	finish_making_copy(copy,also_copy_offsets,ok_to_copy_any_followers);
	return(copy);
};

void Picture::copy_flipped_followers(Picture *copy) {
	if (no_flipped_followers()) return; // added on 220299 for efficiency and because this messed up room decoration sensor size
	// following makes Remote_Integer copies be attached to copy
	Picture *saved_copying_insides_for = tt_copying_insides_for;
	tt_copying_insides_for = copy;
//   boolean change_size = (leader==NULL); // commented out on 140599 since didn't make sense anymore and made things too big
	const boolean change_size = FALSE;
	copy->flip(FALSE,change_size);
	if (!is_flipped()) {
		flip(TRUE,change_size); // so picture is too
	};
   boolean saved_copy_restores_pre_blank = tt_copy_restores_pre_blank;
   tt_copy_restores_pre_blank = FALSE;
//	sort_flipped_followers();
	Sprite *follower_copy = NULL;
	Sprite *previous_follower_copy;
	// following could be rationalized since robots now save links to working cubby 
	// but add_flipped_follower (and others?) would need updating too
	Sprites *sorted_flipped_followers = sort_flipped_followers(); 
   Sprites *remaining = sorted_flipped_followers;
   if (remaining != NULL) {
//      city_coordinate saved_width = width;
//      city_coordinate saved_height = height;
//      set_size(backside_width,backside_height);
      while (remaining != NULL) {
		   Sprite *follower = remaining->first();
			previous_follower_copy = follower_copy;
//			Sprite *copied_before = already_copied(follower); // new on 151204
//			if (copied_before == NULL) {
				follower_copy = follower->copy();
			//} else {
			//	follower_copy = copied_before;
			//};
//			if (copied_before == NULL) { // condition new on 151204 in case infinite recursive structure
				add_flipped_follower(follower_copy,previous_follower_copy,copy);
//			};
			// above added on 160499 since it packages up the following well
//         copy->add_follower(follower_copy);
//			follower_copy->now_on_back_of(copy); // added on 160499 so that remotes get redirected
//		   if (follower_copy->kind_of() == ROBOT &&
//			    previous_follower_copy != NULL &&
//			    previous_follower_copy->kind_of() == CUBBY) {
//			   // associate them but don't run just yet
//			   ((Robot *) follower_copy)->suspend();
//			   ((Robot *) follower_copy)
//				    ->try_clause(((Cubby *) previous_follower_copy));
//		   };
		   remaining = remaining->rest();
      };
//      set_size(saved_width,saved_height);
//        width = saved_width;
//        height = saved_height;
	};
	unflip(TRUE,change_size);
	copy->unflip(FALSE,change_size);
   tt_copy_restores_pre_blank = saved_copy_restores_pre_blank;
   tt_copying_insides_for = saved_copying_insides_for;
   if (sorted_flipped_followers != NULL) {
      sorted_flipped_followers->remove_all();
      delete sorted_flipped_followers;
   };
};

Sprite *Picture::shallow_copy() {
	// doesn't bother with followers
	Sprites *saved_followers = followers;
	Sprites *saved_followers_on_other_side = followers_on_other_side;
	followers = NULL;
   followers_on_other_side = NULL;
	Picture *the_copy = (Picture *) copy();
	followers = saved_followers;
	followers_on_other_side = saved_followers_on_other_side;
//   copy_flipped_followers((Picture *) the_copy); // commented out on 260399 since it can't have any followers of any sort
	return(the_copy);
};

Picture *Picture::current_side_copy() { // new on 191200
	Sprites *saved_followers_on_other_side = followers_on_other_side;
   followers_on_other_side = NULL;
	Picture *the_copy;
	if (this == tt_top_copy) {
		// new on 190902 so that no loops when a remote looks is on the back of a picture
		the_copy = (Picture *) copy(); // made ordinary copy
	// commented out 051201 since the problem was that should not have been called to start with
//	if (controlee == NULL) {
//		the_copy = (Picture *) copy(); // made ordinary copy if not a remote looks on 051201 since otherwise can create a stack overflow when loading 
	} else {
		the_copy = (Picture *) top_level_copy(); // made top_level on 130701 -- not sure why top_level is good idea here
	};
	followers_on_other_side = saved_followers_on_other_side;
	return(the_copy);
};

Picture *Picture::front_side_copy() {
	// doesn't bother with flipped followers 
	boolean originally_flipped = is_flipped();
	// experiment on 041004 to flip and unflip this without temporarily optimization
	if (originally_flipped) unflip(FALSE,FALSE); // added TRUE on 160604 -- FALSE added 130704 so that when flipped back isn't size of flipped picture
	Sprites *saved_followers_on_other_side = followers_on_other_side;
   followers_on_other_side = NULL;
	Picture *the_copy = (Picture *) copy();
	followers_on_other_side = saved_followers_on_other_side;
	if (originally_flipped) flip(FALSE,FALSE); // added TRUE on 160604 -- FALSE added 130704
	return(the_copy);
};

void Picture::set_show_all(boolean new_flag) {
//	if (tt_log_version_number > 14) {
		if (code == SYNTHETIC_SPRITE) { // was != prior to 130499
			if (current_parameter() > 255) {
				// rectangles don't care if they are see all or see some
				set_size_is_true(new_flag);
			} else {
				set_size_is_true(TRUE);
			};
		};
//	} else {
//		if (code != SYNTHETIC_SPRITE) {
//			set_size_is_true(new_flag);
//		};
//	};
	if (indirection != NULL) {
		indirection->set_show_all(new_flag);
	};
	Sprite::set_show_all(new_flag);
};

void Picture::become_part_of_picture(boolean set_show_some) {
	// change them so they are recomputed relative to leader
	// commented out as experiment on 040504 since interferes with time travel and collision_region
	//old_llx = llx+1;
	//old_lly = lly+1;		
	//old_width = width+1;
	//old_height = height+1;
	set_subpicture(TRUE,set_show_some);
};

void Picture::set_subpicture(boolean new_flag, boolean set_show_some) { 
	// second arg new on 190500 so when loaded with show all it sticks
	subpicture_flag = (flag) new_flag;
	if (indirection != NULL && indirection->kind_of() == PICTURE) {
		indirection->set_subpicture(new_flag);
	};
	if (set_show_some) { // conditional part new on 190500
		if (subpicture() && show_some_if_subpicture() && show_all()) {
			// becoming part of a subpicture and should become show some
			// until no longer subpicture
			Sprite *leaders_indirection = NULL;
			if (leader != NULL && leader->kind_of() == PICTURE) { // can it be otherwise??
				leaders_indirection = ((Picture *) leader)->pointer_to_indirection();
			};
			if (leaders_indirection == NULL || (leaders_indirection->kind_of() != CUBBY && leaders_indirection->kind_of() != NEST)) {
				// added != NEST condition on 140600
				// condition new on 060600 since could be a cubby - text and numbers seem OK - what about other objects??
				set_show_all(FALSE);
			};
		} else if (!subpicture() && show_some_if_subpicture() && !show_all()) {
			set_show_all(TRUE);
		};
	};
};

//Sprite *Picture::now_inside_thought_bubble(boolean in_training) {
  // called when thought bubble is connected to robot
  // Remote Pictures are taken care of by Remote_Integer
/* this was a bad policy...
  if (appearance_code() == SCALE_SPRITE) {
	  // rather than become blank scales become "constant" if they can
	  switch (current_parameter()) {
		  case GREATER_THAN_SCALE:
		  case LESS_THAN_SCALE:
		  case EQUAL_SCALE:
			  return(this);
		  case VASCILATE_SCALE:
//			  become_blank();	 // this was too unexpected...
			  return(this);
		  case GREATER_THAN_FROM_EQUAL_SCALE:
			  set_parameter(GREATER_THAN_SCALE);
			  return(this);
		  case LESS_THAN_FROM_EQUAL_SCALE:
			  set_parameter(LESS_THAN_SCALE);		  
			  return(this);
		  case EQUAL_FROM_GREATER_SCALE:
		  case EQUAL_FROM_LESS_SCALE:
			  set_parameter(EQUAL_SCALE);
			  return(this);
//		  default: // don't think there are any more cases
//			  become_blank();
	  };
  };
*/
//  return(this);
//};

boolean Picture::blankable() {
	return(!is_blank() // && (!is_flipped() || controlee != NULL && !controlee->is_flipped()) // commented out on 140504
			 && code != SCALE_SPRITE); // controlee stuff added 181200
};

void Picture::become_blank(boolean by_user, boolean controlee_too) {
	if (is_blank()) return; // already was - added 040299
	Sprite::become_blank(by_user,controlee_too);
   if (controlee_too && controlee != NULL) {
		controlee->become_blank(); //set_clean_status(DIRTY);
	};
	if (indirection != NULL) {
		// changed on 270199 because remote looks only works the first time a picture is indirected (at least to a pad)
		if (contained_in(THOUGHT_BUBBLE) || indirection->kind_of() != PICTURE) { // disjunct added on 130201 so erasing text and number pads work right
			// added on 040399 since matching is broken if indirection becomes NULL (wrong type problem)
			indirection->become_blank(by_user,controlee_too);
		} else {
			saved_indirection = indirection;
			indirection->increment_ref_count(this); // new on 311000
			indirection = NULL;
		};
	} else if (remote != NULL) { // why else if??
		remote->blankness_changed(TRUE);
	};
	if (by_user) {
		user_did(PICTURE,PICTURE_BLANKED);
		if (contained_in(THOUGHT_BUBBLE)) {
			user_did(PICTURE,PICTURE_BLANKED_TB);
		};
	};
};

void Picture::become_non_blank(boolean controlee_too) {
	if (!is_blank()) return; // already was - added 040299
	Sprite::become_non_blank(controlee_too);
	if (controlee != NULL && controlee_too) {
		controlee->become_non_blank(controlee_too); // set_clean_status(DIRTY);
	};
   if (indirection == NULL && saved_indirection != NULL) {
		set_indirection(saved_indirection,FALSE);	// added 280199
	} else if (indirection != NULL) {
		indirection->become_non_blank();
//		return; // why? -- commented out on 260201
	};
	if (remote != NULL) {
		remote->blankness_changed(FALSE);
	};
};

boolean Picture::current_text(const_string &the_text, long &the_text_length) {
	if (is_flipped()) return(FALSE);
	if (indirection != NULL) {
		return(indirection->current_text(the_text,the_text_length));
	};
	if (controlee != NULL) {
		return(controlee->current_text(the_text,the_text_length));
	};
	return(FALSE);
};

boolean Picture::can_be_text() {
	// new on 141299
//	if (is_flipped()) return(FALSE); // commented out on 250303 since can be a controlee that is flipped (or whose indirection is)
	if (indirection != NULL) {
		return(indirection->can_be_text());
	};
	if (controlee != NULL) {
		return(controlee->can_be_text());
	};
	return(FALSE);
};

boolean Picture::current_wide_text(wide_string &the_text, long &the_text_length) {
	// new on 141299
	if (is_flipped()) return(FALSE);
	if (indirection != NULL) {
		return(indirection->current_wide_text(the_text,the_text_length));
	};
	if (controlee != NULL) {
		boolean controlee_was_flipped = controlee->is_flipped();
		if (controlee_was_flipped) { // new on 290303 to deal with controlee being flipped but not remote looks
			controlee->unflip(TRUE,FALSE);
		};
		boolean result = controlee->current_wide_text(the_text,the_text_length);
		if (controlee_was_flipped) {
			controlee->flip(TRUE,FALSE);
		};
		return(result);
	};
	return(FALSE);
};

ascii_string Picture::return_file_name() {
   if (code == USER_SPRITE) { // || tt_picture_directory != NULL) { // tt_picture_directory new on 270901
		TTImage *image = pointer_to_current_image();
		if (image == NULL) { // new on 070403 for robustness
			return(NULL);
		};
      return(image->return_file_name());
   } else {
      return(NULL);
   };
};

boolean Picture::current_value(NumberValuePointer &the_value, boolean &shared, Sprite *by) {
	if (indirection != NULL) { // moved here on 130504 so that remote looks of integers behave better even if controlee is flipped
		return(indirection->current_value(the_value,shared,by));
	};
	if (is_flipped()) { // new on 221299
		if (pointer_to_followers() != NULL) {
			the_value = new LongNumberValue(pointer_to_followers()->length());
		} else {
			the_value = new LongNumberValue(0);
		};
		shared = FALSE;
		return(TRUE);
	} else {
		if (controlee != NULL) {
			return(controlee->current_value(the_value,shared,by));
		};
	};
//	the_value.set_value((long) 0);
	the_value = new LongNumberValue(0);
	shared = FALSE;
	return(FALSE);
};

boolean Picture::is_an_operation_with_no_value() { // new on 091202
	if (indirection != NULL) {
		return(indirection->is_an_operation_with_no_value());
	};
	return(Sprite::is_an_operation_with_no_value());
};

NumberOperation Picture::true_operation() {// new on 091202
	if (indirection != NULL) {
		return(indirection->true_operation());
	};
	return(Sprite::true_operation());
};

//void Picture::increment_screen_ref_count() {
//	Sprite::increment_screen_ref_count();
//	if (indirection != NULL) {
//		indirection->set_screen_ref_count(current_screen_ref_count());
//	}; // what about controlee??
//};

//void Picture::decrement_screen_ref_count() {
//	Sprite::decrement_screen_ref_count();
//	if (indirection != NULL) {
//		indirection->set_screen_ref_count(current_screen_ref_count());
//	}; // what about controlee??
//};

// why since a follower?
//void Picture::set_priority(long new_priority) {
//	image->set_priority(new_priority-1); // on top
//	Tool::set_priority(new_priority);
//};

Sprite *Picture::select(TTRegion *selection_region, SelectionReason reason, Sprite *in_hand) {
//	selected_flag = TRUE;
	if (infinite_stack() && reason != MIGHT_USE_COPIER) return(this); // new on 260500 -- second condition added on 150301 since OK to select subpicture of stack with Wand
	if (!graspable() && (reason == MIGHT_DROP_ITEM || reason == MIGHT_USE_VACUUM || reason == MIGHT_USE_EXPANDER)) {
		return(NULL); // can't drop to add things to a glued down picture, or vacuum them up or pump them -- new on 021204
	};
	TTRegion region = *selection_region; // copy since selection_region belongs to programmer
	if (controlee != NULL) { // new on 171200
		boolean need_to_unflip_controlee = FALSE;
		boolean need_to_flip_controlee = FALSE;
		city_coordinate saved_width = controlee->current_width();
		city_coordinate saved_height = controlee->current_height();
		if (is_flipped() && !controlee->is_flipped()) { // new on 171200
			controlee->flip(); //TRUE,TRUE);
			need_to_unflip_controlee = TRUE;
		} else if (!is_flipped()) {
			if (in_hand != NULL && in_hand->is_flipped()) {
				return(NULL); // new on 070105 since can't drop flipped on non-flipped
			};
			if (controlee->is_flipped()) {
				controlee->unflip(); // TRUE,TRUE);
				need_to_flip_controlee = TRUE;
			};
		}; // otherwise is in the same flipness state
		int64 delta_x_from_llx = (selection_region->min_x-llx);
		int64 delta_y_from_lly = (selection_region->min_y-lly);
		int64 delta_x = (int64) controlee->current_llx()-llx+(delta_x_from_llx*controlee->current_width())/current_width()-delta_x_from_llx;
		int64 delta_y = (int64) controlee->current_lly()-lly+(delta_y_from_lly*controlee->current_height())/current_height()-delta_y_from_lly;
      region.shift_by((city_coordinate) delta_x, (city_coordinate) delta_y);
		Sprite *sub_selection = controlee->select(&region,reason,in_hand);
		if (need_to_flip_controlee) {
			controlee->flip(); //TRUE,TRUE);
			controlee->set_size(saved_width,saved_height); // new on 191200
//			place_new_item();
		} else if (need_to_unflip_controlee) {
			controlee->unflip(); //TRUE,TRUE);
			controlee->set_size(saved_width,saved_height); // new on 191200
		};
		controlee->set_clean_status(DIRTY); // I seem to need this or sometimes when pointing to the remote looks the flipped followers of the controller aren't displayed
		if (sub_selection == controlee) { // didn't select any parts
			return(this);
		} else {
			return(sub_selection);
		};
	};
	if (is_flipped()) { 
		if (reason == MIGHT_DROP_ITEM) {
			//switch (in_hand->kind_of()) {
			//	case NOTEBOOK:
			//		if (in_hand == tt_main_notebook) {
			//			return(NULL); // only notebook you can't put on the back of a picture
			if (in_hand == tt_toolbox_notebook || in_hand->is_tool()) {
				return(NULL); // only exceptions
			} else {
				return(this); // rewritten on 150304 since anything can be dropped on the back of a picture (since 100200)
			};
			//switch (in_hand->kind_of()) {
			//	case CUBBY:
			//		break; // give drop feedback if an unattached robot is found underneath
			//	case ROBOT:
			//	case TEXT: // added these on 120499 so you get drop feedback here
			//	case PICTURE:
			//		return(this);
			//	default:
			//		return(NULL);
			//};
		} else if (reason == MIGHT_USE_EXPANDER) {
         return(this);  // can't pump up individual things on back since tiled
      };
		if (pointer_to_followers() == NULL) return(this);
		Sprites *reversed_followers = NULL;
		Sprites *remaining;
//		if (tt_log_version_number > 14) {
			// prior to 080499 this searched in display order which meant you couldn't select a follower on top of another follower
			reversed_followers = pointer_to_followers()->reverse();
			remaining = reversed_followers;
//		} else {
//			remaining = pointer_to_followers(); 
//		};
		while (remaining != NULL) {
			TTRegion sub_region;
			Sprite *sub = remaining->first();
			if (reason == MIGHT_DROP_ITEM && sub->kind_of() == ROBOT) { // must be a cubby because otherwise handled above
				// added on 120499 so that robot gives good drop feedback
				if (((Robot *) sub)->pointer_to_working_cubby() == NULL) { // doesn't have one so 
					sub->full_collision_region(sub_region); // was full_region prior to 121202 but then feedback wasn't competely accurate
				} else { // ignore this 
					sub_region.min_x = 0;
					sub_region.max_x = 0;
					sub_region.min_y = 0;
					sub_region.max_y = 0;
				};
			} else {
				sub->collision_region(sub_region);
			};
			if (sub_region.overlaps(region)) {
				// commented out so subselection doesn't work on flipped side (Dec 2000 - forgot to add date)	
				// restored on 090100 for nests so you can pick things off the nest without picking up the whole nest
				just_destroy_list(reversed_followers); // moved here on 090100 so it happens independent of the following
				if (sub->kind_of() == NEST) {
					Sprite *subsub = sub->select(selection_region,reason,in_hand);
					if (subsub != NULL) return(subsub);
				};
				return(sub);
			};
			remaining = remaining->rest();
		};
		just_destroy_list(reversed_followers);
//		TTRegion notebook_region;
//		remote_notebook->collision_region(notebook_region);
//		if (notebook_region.contains(selection_region)) {
//			return(remote_notebook);
//		};
	} else if (reason == MIGHT_USE_VACUUM || reason == MIGHT_USE_COPIER) {
		if (pointer_to_followers() == NULL) return(this);
		Sprites *reversed_followers = NULL;
		Sprites *remaining;
//		if (tt_log_version_number > 14) {
			// prior to 080499 this searched in display order which meant you couldn't select a follower on top of another follower
			reversed_followers = pointer_to_followers()->reverse();
			remaining = reversed_followers;
//		} else {
//			remaining = pointer_to_followers(); 
//		};
		while (remaining != NULL) {
			TTRegion subpicture_region;
			Picture *sub_picture = (Picture *) remaining->first();
			sub_picture->collision_region(subpicture_region);
			if (subpicture_region.overlaps(region)) {
				just_destroy_list(reversed_followers);
				return(sub_picture->select(&region,reason,in_hand));
			};
			remaining = remaining->rest();
		};
		just_destroy_list(reversed_followers);
		if (indirection != NULL) {
			Sprite *indirect_selection = indirection->select(&region,reason,in_hand);
			if (indirect_selection != NULL && indirect_selection != indirection) {
				 return(indirect_selection);
			};
		};
	} else if (reason == MIGHT_DROP_ITEM) {
		if (code == SCALE_SPRITE) return(NULL);
		switch (in_hand->fine_kind_of()) { // scales not ok
			case PICTURE:
			case TEXT:
			case INTEGER:
//			case CUBBY: -- what was I thinking? removed on 111104
				return(this);
			default:
				return(NULL);
		};
	} else if (reason != MIGHT_USE_VACUUM && is_kind_of(leader,BIRD)) { // only vacuum can select t_shirt of bird
      return(leader);
   };
	return(this);
};

Sprite *Picture::selected(SelectionReason reason, Sprite *by) {
#if TT_DEBUG_ON
   if (debug_counter == tt_debug_target) {
      log("Selected debug target (Picture)");
   };
#endif
//	boolean contained_in_thought_bubble = contained_in(THOUGHT_BUBBLE);
	if (infinite_stack() || contained_in(PROGRAM_PAD)) {
		return(Sprite::selected(reason,by));
	};
	if (reason == GRASPED) { // condition new on 161004 since otherwise copying a ball in the Pong game resets its speed to zero
		set_x_speed(NULL); // new on 101199 --- really?
		set_y_speed(NULL);
	};
	if (reason != EXPANDER_USED && reason != COPIER_USED && tt_system_mode != PUZZLE) {
//       tt_log_version_number < 10) {  // old policy is pictures stop when picked up
//       tt_system_mode != PUZZLE) {     // puzzle #61 requires this -- policy unclear now
		if (animation_in_progress()) {
			finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
		};
		if (reason != GRASPED) stop_all(); // condition added on 100400 - new policy - should run when held always
	};
	if (reason == VACUUM_USED_TO_BLANK) {// && contained_in_thought_bubble) {
//		if (indirection != NULL) { // sucking it out
//			Sprite *result = indirection;
//			if (floor != NULL) floor->add_item(indirection);
//			indirection = NULL;
//			return(result);
//		};
//		record_event(VACUUM_APPLY,by,floor,this);
//		Picture *super_picture = leader_picture();
//		if (super_picture != NULL) {
//			super_picture->remove_follower(this);
//			return(this);
//		};
		if (!is_blank() && !is_flipped()) {
//			if (leader_picture() == NULL && code != SCALE_SPRITE && visible() && show_all()) {
//				become_blank();
//				tt_toolbox_vacuum->suck_to_make_blank(this,by);
//				user_did(PICTURE,PICTURE_BLANKED);
//				if (contained_in(THOUGHT_BUBBLE)) {
//					user_did(PICTURE,PICTURE_BLANKED_TB);
//				};
//				simulate_mouse_up();
//				return(NULL); // not to be vacuumed up
//			};
			user_did(PICTURE,PICTURE_REMOVED_PICTURE,by);
//  following converted pictures of numbers to numbers -- not good
//			if (indirection != NULL && followers == NULL) {
//				Sprite *real_thing = indirection;
//				real_thing->set_slated_for_deletion(FALSE);
//				set_indirection(NULL);
//				floor->remove_item(this,FALSE,TRUE);
//				floor->add_item(real_thing);
//				if (leader != NULL) {
//					leader->remove_follower(this); // why is it still a follower
//				};
//				tt_deleted_sprites = new Sprites(this,tt_deleted_sprites);
//				return(real_thing);
//			};
		};
		if (is_flipped()) {
			// commented out on 140504
//			unflip(FALSE); // why?? -- very confusing that erasing a flipped pictures unflips it as well -- noticed on 200902
			if (remote_notebook != NULL) {
//            remote_notebook->save_size();
				remote_notebook->remove_from_floor();
			};
		};
		set_subpicture(FALSE); // not anymore
	};
//	if (leader != NULL && reason != COPIER_USED) { 
		// contained in something so let it know
//		leader->remove_selected_element(this,reason);
//	};
	if (leader == NULL && floor != NULL) { // new on 201200
		Picture *picture = floor->pointer_to_picture();
		if (picture != NULL) {
			Sprites *followers_on_other_side = picture->pointer_to_followers_on_other_side();
			if (followers_on_other_side != NULL && followers_on_other_side->member_cons(this) != NULL) { // is on other side so flip and try again
				boolean orginally_flipped = picture->is_flipped();
				if (orginally_flipped) {
					picture->unflip(TRUE,FALSE);
				} else {
					picture->flip(TRUE,FALSE);
				};
				Sprite *result = Sprite::selected(reason,by);
				if (orginally_flipped) {
					picture->flip(TRUE,FALSE);
				} else {
					picture->unflip(TRUE,FALSE);
				};
				if (result != NULL) {
					return(result);
				};
			};
		};
	};
	return(Sprite::selected(reason,by));
};

void Picture::unselect() {
	Sprite::unselect();
	if (indirection != NULL) {
		indirection->unselect();
	};
	if (comment != NULL) {
		comment->unselect(); // new on 120604
	};
};

boolean Picture::is_blank() { // new on 260201
	if (controlee != NULL) {
		return(controlee->is_blank());
	} else if (indirection != NULL) {
		return(indirection->is_blank());
	} else {
		return(Sprite::is_blank());
	};
};

//void picture_notebook_copied(Sprite *by) {
//	user_did(PICTURE,PICTURE_NOTEBOOK_COPIED,by);
//};

void scale_used() {
	user_did(PICTURE,SCALE_USED);
};

void removed_item_from_flip_side() {
	user_did(PICTURE,PICTURE_CUBBY_OR_ROBOT_REMOVED);
};

/*
// generic version fine
void Picture::remove_selected_element(Sprite *element, SelectionReason ) {
	remove_follower(element);
//	element->set_on_flip_side_of_picture(FALSE); // regardless
	floor->add_item(element);
//	element->un_absorb_followers();
	set_clean_status(DIRTY);
//	if (is_flipped()) {
//		element->increment_screen_ref_count();
//	};
};
*/

boolean explained_how_to_change_pictures = FALSE;

void Sprite::respond_to_stop_character() { // abstracted on 310803
	set_active(FALSE); // new on 081199
	stop_all();
};

Sprite *Picture::respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected,
											    Sprite *by, boolean record_action, Sprite *original_recipient, 
												 boolean size_constrained_regardless) {
	if (!extended && keyboard_accelerator(key,FLIP_THING_HELD)) { // !extended added on 090804
		// this is meant for the picture itself not what it is connected to
		if (held() && (indirection == NULL || is_flipped() || indirection->kind_of() != TEXT)) { 
			// second conditions added 051000 since flipped text shouldn't be flipped by typing an f
			return(used(NULL,by,FALSE,record_action,0,FALSE,NULL));
		};
//		return(this); // commented out on Aug 15 '98 since if not held f might get passed thru -- e.g. if a picture of some text.
	};
//	if (!blank) { // || code == SYNTHETIC_SPRITE) { -- commented out on 070900 -- thanks to Sarah for providing the example
	// the following should apply regardless of whether it is blank
	boolean handled = FALSE;
	if (tt_log_version_number >= 46 && is_flipped() && controlee == NULL && 
		 (original_recipient == this || (indirection != NULL && indirection->kind_of() == PICTURE))) { 
		// new on 110604 to add comments to the flipside -- 
		// added test to see if this is the original recipient on 300604 since not adding comment
		// if typing to remote looks (if remote looks of a number or text pad)
		if (comment == NULL) {
			if (!extended) {
				string text = new char[2];
				text[0] = key;
				text[1] = '\0';
				comment = variable_width_text_pad(text,FALSE); // black is fine as of 160604 ,4); // a very dark blue
				comment->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL);
				if (label != NULL) {
					label->destroy();
					label = NULL; // rebuild it
				};
			};
		} else {
			// replaced record_action with FALSE below on 030105 otherwise robots do this twice
			// (e.g. each letter is recorded twice - except first one)
			city_coordinate comment_lly = comment->current_lly(); 
			// new on 210205 since can be "off picture" for a while when new lines are added
			comment->respond_to_keyboard(key,extended,robot_selected,by,FALSE,comment,size_constrained_regardless);
			comment->set_lly(comment_lly); // new on 210205
			if (comment->kind_of() == TEXT && label != NULL) { // commented out as experiment on 120604
				Text *text_comment = (Text *) comment;
				city_coordinate picture_width = 0;
				Sprites *remaining = label->pointer_to_followers();
				while (remaining != NULL) {
					Sprite *follower = remaining->first();
					if (follower != comment && follower->kind_of() == PICTURE) {
						picture_width = follower->current_width();
						break;
					};
					remaining = remaining->rest();
				};
				city_coordinate label_width = label->current_width();
				city_coordinate label_height = label->current_height();
				if (picture_width == 0) { // couldn't find it
					picture_width = label_width/2;
				};
				city_coordinate picture_margins = label_height/5;
				city_coordinate maximum_comment_width = label_width-(picture_width+picture_margins);
				if (comment->current_width()+text_comment->return_character_width() < maximum_comment_width) { 
					// change size due to extra characters but don't get too long (more than half of label's width) -- updated 120604
					text_comment->set_change_size_to_fit_extent(TRUE);
					text_comment->set_change_font_size_to_fit_extent(FALSE);
				} else {
					text_comment->set_width(maximum_comment_width);
				};
				text_comment->update_size(TRUE);
				if (text_comment->current_height() > label_height) { // new on 120604
					text_comment->set_height(label_height);
					text_comment->set_lly(label->current_lly()+label_height/10); // maintain vertical position including margins
				};
			};	
		};
		handled = TRUE;
	} else if (!extended && // extended test new on 010704 since Delete caused pad to be stopped (treated like '.')
					(!held() || !by->can_send_keyboard_editing_keys() || indirection == NULL || 
					 (indirection->kind_of() != TEXT && indirection->kind_of() != INTEGER))) {
		// conditional new on 080604 to accomodate the new editing of text and number pads
      if (key == tt_stop_character) { // new on 190799
			//set_active(FALSE); // new on 081199
			//stop_all();
			if (controlee != NULL) {
				controlee->do_after(1,this,RESPOND_TO_STOP_CHARACTER_CALLBACK); 
				// new on 140605 since otherwise this doesn't happen until the picture is flipped (and the remote looks is updated)
			} else {
				do_after(1,this,RESPOND_TO_STOP_CHARACTER_CALLBACK);
			};
			handled = ultimate_indirection_is_picture(); // was TRUE prior to 261199 but if text pad want both to happen
      } else if (key == tt_run_character) {
//			if (tt_log_version_number > 14) {//  && tt_debug_mode != 191099) {
			if (!replaying() && active() && by != NULL && by->kind_of() != ROBOT // new on 181199
					&& no_flipped_followers() // added 100200 since is annoying if lost track of whether it is on or off
					&& !explained_how_to_change_pictures // this added on 100200 so reminders only happen once per session
					&& !is_flipped() // added 120200
					&& (indirection == NULL || indirection->kind_of() == PICTURE) && controlee == NULL) { // new on 240200
				if (code != SYNTHETIC_SPRITE) {
					martian_says(IDS_PICT_CHANGED_KEY_HELP3,2);
					martian_says(IDS_PICT_CHANGED_KEY_HELP4,2);
				} else {
					martian_says(IDS_PICT_COLOR_KEY_HELP2,2);
					martian_says(IDS_PICT_COLOR_KEY_HELP3,2);
				};
				explained_how_to_change_pictures = TRUE;
			};
#if TT_DEBUG_ON
			if (tt_debug_mode == 70900 && by != NULL && by->kind_of() == ROBOT) {
				tt_error_file() << "Robot pressed space at a picture at " << tt_frame_number << endl;
			};
#endif
			set_active(TRUE);
			activate(FALSE);
			handled = ultimate_indirection_is_picture();
			if (handled && controlee != NULL) { // new on 240200
				handled = controlee->ultimate_indirection_is_picture();
			};
      } else if (key == toggle_graspable) { // new on 100400
			set_graspable(!graspable());
		};
	};
	if (!is_blank() && !handled) {// || code == SYNTHETIC_SPRITE) {
		if (indirection != NULL) { 
			// && !is_flipped()) { 
			// commented out on 170202 since doesn't matter if this was once flipped that it indirects is all the counts
			city_coordinate old_indirection_width = indirection->true_width();
			city_coordinate old_indirection_height = indirection->true_height();
			indirection->respond_to_keyboard(key,extended,robot_selected,by,FALSE,original_recipient,(leader != NULL));
			if (leader == NULL && !held()) { // new on 180102
				indirection->set_change_size_to_fit_extent(TRUE);
			};
//				action_status = NULL; // new on 051000 since is already taken care of above 
			// -- commented out on 181102 -- deal with saved_by instead?
			if (leader == NULL) { 
				// if size changed update indirect picture too new on 221299 -- 
				// commented out on 221200 since size of indirection isn't the "truth" anymore
				// re-written on 290101 since if unconstrained (e.g. held) then should be updated
				wide_string ignore_text;
				long ignore_length;
				current_wide_text(ignore_text,ignore_length); // new on 070604 to force this to compute new string and dimensions
				city_coordinate new_indirection_width = indirection->true_width();
				city_coordinate new_indirection_height = indirection->true_height();
				if	(old_indirection_width != new_indirection_width || old_indirection_height != new_indirection_height) { 
					set_size(new_indirection_width,new_indirection_height);
				};
			};
//		  set_clean_status(indirection->current_clean_status());
//		  return(this);
		} else if (controlee != NULL && !is_flipped()) {
			controlee->respond_to_keyboard(key,extended,robot_selected,by,FALSE,original_recipient);
//				action_status = NULL; // new on 051000 since is already taken care of above -- commented out on 181102
			set_clean_status(controlee->current_clean_status());
//		  return(this);
		} else {
			int old_parameter = current_parameter();
			switch (key) {
				case ' ': // probably overridden above
				case '+':
					if (code == SCALE_SPRITE) {
						increment_scale_parameter(TRUE); // was 1 prior to 130201
						set_active(FALSE); // new on 140201
					} else {
						increment_parameter(1);
					};					
					break;
				case '-':
				case BACKSPACE:
					if (code == SCALE_SPRITE) {
						increment_scale_parameter(FALSE); // was -1 prior to 130201
						set_active(FALSE); // new on 140201
					} else {
						increment_parameter(-1);
					};
					break;
	//        case 'f':
	//        case 'F':
	//           if (held()) return(used(NULL,by,FALSE,TRUE,0,action_status,FALSE));
	//           return(this);
			};
			if (old_parameter != current_parameter()) {
	//			image->update_display(); // to update things like full_width()
	//			update_sizes();
				if (code == SCALE_SPRITE) {
					user_did(PICTURE,SCALE_KEYBOARDED,by);
				} else if (code == SYNTHETIC_SPRITE) {
					user_did(PICTURE,PICTURE_COLOR_BY_KEYBOARD,by);
				} else {
					user_did(PICTURE,PICTURE_CHANGED_BY_KEYBOARD,by);
				};
			};
			// restored so that this happens only on this branch but is this right??
//				completed(action_status); // had been commented out on 080400 since handled by Sprite::respond_to_keyboard
		};
	};
	by->completed(); // moved here on 070900
   // this was in last conditional so was recorded only if no indirection and parameter changed
//   if (record_action) record_event(KEYBOARD_CHARACTER,by,floor,this,TRUE,key); // moved to Sprite::respond_to_keyboard
   return(Sprite::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,
												  size_constrained_regardless));
};

void Picture::increment_scale_parameter(boolean up) {
	// re-written on 130201 to behave sensibly
	switch (current_parameter()) {
		case EQUAL_SCALE:
		case EQUAL_FROM_LESS_SCALE:
		case EQUAL_FROM_GREATER_SCALE:
			if (up) {
				set_parameter(GREATER_THAN_SCALE);
			} else {
				set_parameter(LESS_THAN_SCALE);
			};
			break;
		case GREATER_THAN_SCALE:
			if (up) { // cycle around
				set_parameter(LESS_THAN_SCALE);
			} else {
				set_parameter(EQUAL_SCALE);
			};
			break;
		case LESS_THAN_SCALE:
			if (up) { 
				set_parameter(EQUAL_SCALE);
			} else { // cycle around
				set_parameter(GREATER_THAN_SCALE);
			};
			break;
		default: // can happen?
			set_parameter(EQUAL_SCALE);
	};
};

// old version
//void Picture::increment_scale_parameter(boolean up) {
//   if (current_parameter() == LESS_THAN_SCALE && up) {
//      set_parameter(EQUAL_SCALE);
//   } else if (current_parameter() == EQUAL_SCALE && !up) {
//      set_parameter(LESS_THAN_SCALE);
//   } else if (is_constant_scale()) { // should these happen anymore??
//      if (up) {
//         set_parameter((short) (current_parameter()+1));
//      } else {
//         set_parameter((short) (current_parameter()-1));
//      };
//   } else { // vascilating or something...
//      set_parameter(EQUAL_SCALE);
//   };
//};

void Picture::add_robots_to_flipped_followers() {
	if (tt_queue != NULL) {
		tt_queue->add_robots_to_flipped_followers(this);
	};
   Sprites *remaining = tt_running_robots;
//	Sprites *previous_remaining = NULL;
   while (remaining != NULL) {
      Robot *robot = (Robot *) (remaining->first());
      if (robot->pointer_to_background() == pointer_to_backside() && robot->pointer_to_leader() == NULL 
			 && robot->pointer_to_background() != NULL) { // added on 100400 since if floor is NULL isn't a flipped follower
         add_robot_as_flipped_follower(robot);
			// new on 080400 to remove robot from running robots
//			if (previous_remaining == NULL) {
//				tt_running_robots = remaining->rest();
      };
      remaining = remaining->rest();
	};
};

void Picture::add_robot_as_flipped_follower(Robot *robot) {
   boolean originally_flipped = is_flipped();
   if (!originally_flipped) flip(TRUE,FALSE); // was FALSE,FALSE prior to 240299
   place_new_item(robot);
	// following broke pictures with robots on the back -- Grabbing them broke the connection of robot and cubby
//	if (robot->pointer_to_leader() != NULL) {
//		robot->pointer_to_leader()->remove_follower(robot);
//	};
	robot->add_to_end_of_line(); // added on 090599 so that robot reform in a team after being flipped
	if (robot->pointer_to_leader() == NULL) { // condition new on 181200 since might have gotten leader being flipped above
		add_follower(robot);
	};
   robot->set_selectable(TRUE);
   Cubby *cubby = robot->pointer_to_working_cubby();
   if (cubby != NULL) {
   	cubby->set_selectable(TRUE);
      place_new_item(cubby,robot);
		if (cubby->pointer_to_leader() == NULL) { // condition new on 181200
			add_follower(cubby);
		};
   };
   if (!originally_flipped) unflip(TRUE,FALSE);
};

void Picture::remove_flipped_follower(Sprite *follower) {
#if TT_DEBUG_ON
	if (follower->debug_counter == tt_debug_target) {
		log("Removing debug target from back of a picture");
	} else if (debug_counter == tt_debug_target) {
		log("Removing something from the debug target.");
	};
#endif
	if (is_flipped() && followers != NULL) {
//		followers = followers->remove(follower);
		if (follower->pointer_to_leader() == this) { 
			// condition new on 070105 since sprite::remove_follower calls remove_from_floor which can call this
			remove_follower(follower,FALSE); // better than above since does more "bookkeeping as well" (new on 161200)
		};
	} else if (followers_on_other_side != NULL) {
		followers_on_other_side = followers_on_other_side->remove(follower);
	};
	if (follower->kind_of() == ROBOT && tt_running_robots != NULL) { // new on 210499
//		boolean found;
#if TT_DEBUG_ON
		if (follower->debug_counter == tt_debug_target) {
			tt_error_file() << "Removing robot debug target from running robots." << endl;
		};
#endif
		tt_running_robots = tt_running_robots->remove(follower); //,&found);
//		if (found) follower->destroy(); // release reference
		// commented out tt_running_robot ref count stuff on 090400 since causes robots to not be deleted and to run (as orphans)
		// and when robot is prepared for deletion it removes itself from tt_running_robot
	};
};
// used to be defined as:
//   boolean originally_flipped = is_flipped();
//   if (!originally_flipped) flip(TRUE,FALSE); // was FALSE,FALSE prior to 240299
//   remove_follower(follower,FALSE);
//   if (!originally_flipped) unflip(TRUE,FALSE);
//};

void Picture::stop_all(boolean for_good, boolean suspend_robots_too) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Stopping debug target (picture)");
	};
#endif
	if (controlee != NULL) { // new on 171200
//		controlee->stop_all(for_good); - this caused infinite recursion if remote looks is on the back
		return;
	};
   if (!for_good) {
//		if (!is_flipped()) {
//			return; // new policy on 240200 so that pictures on the back of pictures run so long as they aren't flipped
//		};
		add_robots_to_flipped_followers(); //  && tt_log_version_number > 7
	};
	robots_running_on_back_flag = FALSE;
	stopped_flag = TRUE; // restored on 170702 since needed to stop movement due to x or y speed
//	set_activated(FALSE);
	Sprites *remaining = flipped_followers(); 
	while (remaining != NULL) {
		Sprite *item = remaining->first();
		if (tt_debug_mode != 100400 || item->kind_of() == ROBOT || item->is_flipped()) {
			item->stop_all(for_good);
		};
//		switch (item->kind_of()) {
//			case ROBOT:
//				if (for_good) {
//					item->stop_all();
//				} else {
//					((Robot *) item)->suspend();
//				};
//				break;
//			default: // new on 180200 so for example pictures in boxes are stopped
//				item->stop_all(for_good);
//				break;
//		};
		remaining = remaining->rest();
	};
	remaining = unflipped_followers();
	while (remaining != NULL) {
		if (remaining->first()->kind_of() == PICTURE) {
			((Picture *) remaining->first())->stop_all(for_good);
		};
		remaining = remaining->rest();
	};
//	if (backside_background != NULL) { // new on 070400
//		remaining = backside_background->pointer_to_items();
//		while (remaining != NULL) {
//			remaining->first()->stop_all(for_good);
//			remaining = remaining->rest();
//		};
//	};
	// above is probably redundant and should be removed
// commented out the following on 150500 since just because a remote looks is stopped doesn't mean controlee should
// but maybe if '.' is typed to it...
//	if (controlee != NULL && controlee->robots_running_on_back()) { // new on 070400
//		controlee->stop_all(for_good);
//	};
};

//	if (remote_robots != NULL) { // stop all robots inside
//		for (int i = 0; i < remote_cubby_count; i++) {
//			if (remote_robots[i] != NULL) {
//				remote_robots[i]->stop_all();
//			};
//		};
//	};

//boolean Picture::is_truely_flipped() { // commented out on 130201 since obsolete now
//	if (controlee != NULL) {
//		return(controlee->is_flipped());
//	} else {
//		return(is_flipped());
//	};
//};

MatchStatus Picture::match(Picture *other, SpritePointerPointer &suspension_cell, int version, int index) { 
	// extra args added 141100
	Picture *others_controlee = other->pointer_to_controlee();
	if (others_controlee != NULL && others_controlee->is_flipped() && !other->is_flipped()) {
		// new on 291004 to deal with a controlee that is flipped while remote looks isn't
		others_controlee->unflip(TRUE,TRUE);
		MatchStatus result = match(others_controlee,suspension_cell,version,index);
		others_controlee->flip(TRUE,FALSE);
		return(result);
	};
	if (tt_match_blank_with_sensor_then_run_once_per_cycle) { // new on 030404 to do this before matching
		if (other->robots_running_on_back()) { // wasn't other-> before 9/1/97
			tt_touched_a_remote = ONLY_ON_NEW_CYCLE;
		};
	};
	if (version > 1) {
		if ((is_flipped() && !other->is_flipped()) || (!is_flipped() && other->is_flipped())) { 
			// add truely on 181200 and removed on 230101
			return(MATCH_BAD); // new on 120200
		};
	};
	// above condtionally moved before the following on 120504 so fails to match if different flippedness
   if (is_blank()) return(MATCH_GOOD); // moved this up here on 061102 since if blank doesn't matter if robots are running
   if (!tt_match_blank_with_sensor_then_run_once_per_cycle) { // new on 030404 to do this before matching
		if (other->robots_running_on_back()) { // wasn't other-> before 9/1/97
			tt_touched_a_remote = ONLY_ON_NEW_CYCLE;
		};
	};
	if (version == 1) {
		if ((is_flipped() && !other->is_flipped()) || (!is_flipped() && other->is_flipped())) { 
			// add truely on 181200 and removed on 230101
			return(MATCH_BAD); // new on 120200
		};
	};
	if (is_flipped() && other->is_flipped()) { // new on 171200 - removed truely on 230101
		return(MATCH_GOOD);
	};
	short int other_index;
	SpriteCode other_sprite_code;
	if (!other->current_image_index(other_index,other_sprite_code)) {
		return(MATCH_BAD);
	};
	if (appearance_code() == other_sprite_code) {
#if TT_DEBUG_ON
		if (tt_debug_target == other->debug_counter) {
			log("Matching debug target picture.");
		};
#endif
		if (other_sprite_code == SCALE_SPRITE) { 
			Relation my_relation = constant_relation(); // given by keyboard
			if (my_relation == NO_RELATION) {
				my_relation = compute_relation();
				if (my_relation == NO_RELATION) {
					my_relation = INCOMPARABLE_RELATION;
				};
			};
			Relation other_relation = other->compute_relation(TRUE);
			if (other_relation == NO_RELATION) {
				other_relation = INCOMPARABLE_RELATION;
			};
//			if (tt_log_version_number < 15) {
				// changed this out on 080399 since incomparable relation should match incomparable
				// I have no idea why the following comment says what it does since it wasn't implemented
				// new on May 15 '98 is that two tottering scales match
//				if (my_relation == INCOMPARABLE_RELATION) { // || my_relation == NO_RELATION) {
//					return(MATCH_BAD);
//				};
//				Relation other_relation = other->compute_relation(TRUE);
//				if (other_relation == INCOMPARABLE_RELATION) { // || other_relation == NO_RELATION) {
//					return(MATCH_BAD);
//				};
//			};
			if (my_relation == other_relation) {
				return(MATCH_GOOD);
			} else if (other_relation == SUSPENSION_RELATION) {
				Cubby *others_cubby = (Cubby *) other->pointer_to_leader(); // has to be a cubby, right??
				suspension_cell = others_cubby->pointer_to_component(index+1); 
				// +1 because that is where the nest is // coerced to (cubby_index) prior to 070203
				return(MATCH_SUSPENDED_ON_NEST);
			} else {
				return(MATCH_BAD);
			};
      } else if (other_sprite_code == USER_SPRITE) {
			string my_file_name = return_file_name();
			string others_file_name = other->return_file_name();
         if (others_file_name != NULL && my_file_name != NULL && // conditions added 260900
				 stricmp(others_file_name,my_file_name) == 0) {
            return(MATCH_GOOD);
         } else {
            return(MATCH_BAD);
         };
		} else {
			if (current_parameter() == other_index) {
				return(MATCH_GOOD);
			} else {
				return(MATCH_BAD);
			};
		};
	} else {
		return(MATCH_BAD);
	};
};

Relation numeric_relation(long left_value, long right_value) {
	if (left_value == right_value) {
		return(EQUAL_RELATION);
	};
	if (left_value > right_value) {
		return(GREATER_THAN_RELATION);
	};
	return(LESS_THAN_RELATION);
};

boolean Picture::neighbors(SpritePointer &left_original, SpritePointer &right_original) {
	Sprite *holder = (Cubby *) leader;
	Sprite *me = this; // new on 100205 since if on nest it is the nest that has a location in the cubby
	while (holder != NULL && holder->kind_of() == NEST) {
		me = holder;
		holder = holder->pointer_to_leader();
	};
	if (holder == NULL || holder->kind_of() != CUBBY) {
		return(FALSE);
	};
	Cubby *cubby = (Cubby *) holder;
	int my_location = cubby->my_index(me); // coerced to (cubby_index) prior to 070203 -- was this instead of me prior to 100205
	if (my_location <= 0) { // no room on the left -- was == prior to 100205 but if on a nest then the above returned -1
		return(FALSE);
	};
	int cubby_size = cubby->current_number_of_holes();
	if (my_location == cubby_size-1) { // 0 indexing
		return(FALSE);	// no room on right
	};
	left_original = cubby->component(my_location-1); // coerced to (cubby_index) prior to 070203
	if (left_original == NULL) {
		return(FALSE); // no one to the left
	};
	right_original = cubby->component(my_location+1); // coerced to (cubby_index) prior to 070203
	if (right_original == NULL) {
		return(FALSE); // no one to the right
	};
	return(TRUE);
};

Relation Picture::compute_relation(boolean maintain_touched, 
                                   boolean blanks_as_originals, 
                                   boolean ignore_constant_relation) {
   Relation frozen_relation = constant_relation();   
   if (!ignore_constant_relation) {
		if (frozen_relation != NO_RELATION) {
			return(frozen_relation);
		}; // otherwise continue below
	};
	Sprite *left_original;
	Sprite *right_original;
   if (!neighbors(left_original,right_original)) {
		return(frozen_relation);
	};
	if (left_original->kind_of() == REMOTE_PICTURE && right_original->kind_of() == REMOTE_PICTURE) {
		// new on 190499 to compare avenues with avenues and streets with streets as numerical values
		Remote_Picture *left_picture = (Remote_Picture *) left_original;
		Remote_Picture *right_picture = (Remote_Picture *) right_original;
		if (left_picture->is_remote_address() && right_picture->is_remote_address()) {
			return(numeric_relation(((Remote_Address *) left_picture)->return_address_number(),
											((Remote_Address *) right_picture)->return_address_number()));
/* commented out on 220499 so that you can compare avenues and streets
			long left_value = left_picture->return_avenue_number();
			long right_value = right_picture->return_avenue_number();
			if (left_value > -1) { // is really an avenue number
				if (right_value > -1) { // both are
					return(numeric_relation(left_value,right_value));
				} else {
					return(INCOMPARABLE_RELATION);
				};
			} else if (right_value < 0) { // is a street number
				return(numeric_relation(left_picture->return_street_number(),right_picture->return_street_number()));
			} else {
				return(INCOMPARABLE_RELATION);
			};
*/
		};
	};		
	Sprite *left = left_original->dereference();
	Sprite *right = right_original->dereference();
	// no need to check the left here since that would have suspended already
	if (right->kind_of() == NEST) { // new on 080399 so matches suspend rather than fail if an empty nest in encountered
		return(SUSPENSION_RELATION);
	};
	// just numbers and text for now (think about cubbies, birds, etc)
	switch (left->kind_of()) {
		case INTEGER:
			if (right->kind_of() == INTEGER) {
				NumberValue *left_number_value, *right_number_value;
				boolean left_number_value_shared, right_number_value_shared;
//				long left_value,right_value;
				Number *left_number = (Number *) left; 
				Number *right_number = (Number *) right;
				if (maintain_touched) {
					if (tt_touched_a_remote != ONLY_ON_NEW_CYCLE) {
						// left_number replaced by left_original on 150200 so change_frequency is correct
						// this changes scheduling and suspension of robots
						set_touched_a_remote(left_original->change_frequency()); 
						if (tt_touched_a_remote != ONLY_ON_NEW_CYCLE) {
							set_touched_a_remote(right_original->change_frequency());
						};
					};
				};
				boolean left_number_originally_blank = FALSE;
				if (blanks_as_originals && left_number->is_blank()) { 
					// new on 060202 -- would it be better to add an extra argument to current_value instead??
					left_number->just_set_blank(FALSE);
					left_number_originally_blank = TRUE;
				};
				boolean left_value_ok = left_number->current_value(left_number_value,left_number_value_shared); 
				// re-written on 091100 so is INCOMPARABLE_RELATION unless !blanks_as_originals
				if (left_number_originally_blank) {
					left_number->just_set_blank(TRUE);
				};
				if (!left_value_ok) {
					if (!blanks_as_originals) return(frozen_relation);
//					 if (!left_number->is_blank()) // commented out conditional on 060202
						 return(INCOMPARABLE_RELATION);
				};
				boolean right_number_originally_blank = FALSE;
				if (blanks_as_originals && right_number->is_blank()) {
					right_number->just_set_blank(FALSE);
					right_number_originally_blank = TRUE;
				};
  				boolean right_value_ok = right_number->current_value(right_number_value,right_number_value_shared);
				if (right_number_originally_blank) {
					right_number->just_set_blank(TRUE);
				};
				if (!right_value_ok) {
					if (!left_number_value_shared) delete left_number_value;
					if (!blanks_as_originals) return(frozen_relation);
//					if (!right_number->is_blank()) 
						return(INCOMPARABLE_RELATION); // new on 091100
				};
				Relation result = left_number_value->comparison(right_number_value);
				if (!left_number_value_shared) delete left_number_value;
				if (!right_number_value_shared) delete right_number_value;
				return(result);
			} else {
				return(INCOMPARABLE_RELATION);
			};
		case TEXT:
			if (right->kind_of() == TEXT) {
				string left_string,right_string;
				long left_text_length,right_text_length;
				Text *left_text = (Text *) left;
				Text *right_text = (Text *) right;
				if (!left_text->current_text(left_string,left_text_length) && // blank
					 !blanks_as_originals) return(frozen_relation);
				if (!right_text->current_text(right_string,right_text_length) &&
					 !blanks_as_originals) return(frozen_relation);
				if (left_text_length == 0) {
					if (right_text_length == 0) {
						return(EQUAL_RELATION);
					} else {
						return(LESS_THAN_RELATION);
					};
				} else if (right_text_length == 0) {
					return(GREATER_THAN_RELATION);
				};
				left_string[left_text_length] = NULL; // terminate to be sure
				right_string[right_text_length] = NULL;
  				int comparison;
				if (version_number < 4) {
					comparison = lstrcmpiG(u2a(left_string),u2a(right_string)); 
				} else { // new on 090506
					comparison = lstrcmpG(u2a(left_string),u2a(right_string));
				};
				// note that this doesn't do anything very clever for Japanese but only NT/2000/XP supports Unicode version
				if (comparison == 0) {
					if (left_text_length == right_text_length) {
						return(EQUAL_RELATION);
					} else if (left_text_length > right_text_length) {
						return(GREATER_THAN_RELATION);
					} else {
						return(LESS_THAN_RELATION);
					};
				};
				if (comparison > 0) {
					return(GREATER_THAN_RELATION);
				};
				return(LESS_THAN_RELATION);
			} else {
				return(INCOMPARABLE_RELATION);
			};
		case PICTURE: // added 080399
			if (right->kind_of() == PICTURE) {
				Picture *left_picture = (Picture *) left;
				Picture *right_picture = (Picture *) right;
				ascii_string left_file_name = left_picture->return_file_name();
				ascii_string right_file_name = right_picture->return_file_name();
				if (left_file_name != NULL && right_file_name != NULL) {
					// compare the pictures by using the picture names
					int comparison = lstrcmpi(left_file_name,right_file_name);
					int left_file_name_length = strlen(left_file_name);
					int right_file_name_length = strlen(right_file_name);
					if (comparison == 0) {
						if (left_file_name_length == right_file_name_length) { // corrected bug here on 090399
							return(EQUAL_RELATION);
						} else if (left_file_name_length > right_file_name_length) {
							return(GREATER_THAN_RELATION);
						} else {
							return(LESS_THAN_RELATION);
						};
					};
					if (comparison > 0) {
						return(GREATER_THAN_RELATION);
					};
					return(LESS_THAN_RELATION);
				} else if (left_file_name != NULL) { // user pictures are greater than builtins
					return(GREATER_THAN_RELATION);
				} else if (right_file_name != NULL) {
					return(LESS_THAN_RELATION);
				} else { // both are built so compare by image index
					int left_index = left_picture->image_cycle_resource_index();
					int right_index = right_picture->image_cycle_resource_index();
					if (left_index == right_index) {
						return(EQUAL_RELATION);
					};
					if (left_index > right_index) {
						return(GREATER_THAN_RELATION);
					};
					return(LESS_THAN_RELATION);
				};
			} else {
				return(INCOMPARABLE_RELATION);
			};
	};
	return(INCOMPARABLE_RELATION);
};

Relation Picture::constant_relation() {
	// checks if explicitly given a value (e.g. by user keyboard)
	switch (current_parameter()) {
		case EQUAL_SCALE: // constant
			return(EQUAL_RELATION);
		case GREATER_THAN_SCALE:
			return(GREATER_THAN_RELATION);
		case LESS_THAN_SCALE:
			return(LESS_THAN_RELATION);
		default:
			if (!active()) { // new on 290301 so inactive acts like it looks
				switch (current_parameter()) {
					case GREATER_THAN_FROM_EQUAL_SCALE:
						return(GREATER_THAN_RELATION);
					case LESS_THAN_FROM_EQUAL_SCALE:
						return(LESS_THAN_RELATION);
					case EQUAL_FROM_GREATER_SCALE:
					case EQUAL_FROM_LESS_SCALE:
						return(EQUAL_RELATION);
				};
			};
			return(NO_RELATION);
	};
};

SpriteType Picture::fine_kind_of() {
  if (code == SCALE_SPRITE) {
	  return(SCALE_PICTURE);
  } else {
	  return(Sprite::fine_kind_of());
  };
};

void Picture::compute_scale_value(Relation relation) {
	// only called if this is a non-constant Scale
	switch (relation) {
		case GREATER_THAN_RELATION:
			// for now skip the animation from less than to greater than
			// easier if TTS supported it
			set_parameter(GREATER_THAN_FROM_EQUAL_SCALE);
			return;
		case LESS_THAN_RELATION:
			set_parameter(LESS_THAN_FROM_EQUAL_SCALE);
			return;
		case EQUAL_RELATION:
			switch (current_parameter()) {
				case EQUAL_FROM_GREATER_SCALE:
				case EQUAL_FROM_LESS_SCALE:
					return; // already know it
				case GREATER_THAN_FROM_EQUAL_SCALE:
					set_parameter(EQUAL_FROM_GREATER_SCALE);
					return;
//				case LESS_THAN_FROM_EQUAL_SCALE:
//				case VASCILATE_SCALE:
				default:
					set_parameter(EQUAL_FROM_LESS_SCALE);
					return;
			};
		case INCOMPARABLE_RELATION:
		case NO_RELATION:
		case SUSPENSION_RELATION:
			set_parameter(VASCILATE_SCALE);
			return;
	};
};

Sprites *Picture::robots_on_the_back() {
	if (controlee != NULL) return(NULL); // new on 171200 - remote looks doesn't really have anything on back
   Sprites *robots = NULL;
   Sprites *remaining = flipped_followers();
   while (remaining != NULL) {
		Sprite *sprite = remaining->first();
      if (sprite->kind_of() == ROBOT) {
         insert_at_end(sprite,robots);
      };
      remaining = remaining->rest();
	};
   return(robots);
};

Sprites *Picture::reachable_sprites_of_type(SpriteType type) {
	if (controlee != NULL) return(NULL); // new on 171200 - remote looks doesn't really have anything on back
	if (type == PICTURE) { // not currently use
		return(new Sprites(this));
	};
   Sprites *answer = NULL;
   Sprites *remaining = flipped_followers();
   while (remaining != NULL) {
		Sprite *sprite = remaining->first();
		if (answer == NULL) {
			answer = sprite->reachable_sprites_of_type(type);
		} else {
			answer->last_cons()->set_rest(sprite->reachable_sprites_of_type(type));
		};
      // older way:
//		switch (sprite->kind_of()) {
//         case ROBOT:
//            robots = new Sprites(sprite,robots);
//            break;
//         case PICTURE: // flipped picture on back lending behavior
//            if (robots == NULL) {
//               robots = sprite->reachable_robots();
//            } else {
//               robots->last_cons()->set_rest(sprite->reachable_robots());
//            };
//            break;
//		};
		remaining = remaining->rest();
	};
   remaining = unflipped_followers();
   while (remaining != NULL) {
		Picture *sub_picture = (Picture *) remaining->first();
		if (answer == NULL) {
			answer = sub_picture->reachable_sprites_of_type(type);
		} else {
			answer->last_cons()->set_rest(sub_picture->reachable_sprites_of_type(type));
		};
      remaining = remaining->rest();
   };
   return(answer);
};

Sprites *Picture::reachable_non_equal_sprites_of_type(SpriteType type) { // new on 250601
	if (controlee != NULL) return(NULL); 
	if (type == PICTURE) { // not currently use
		return(new Sprites(this));
	};
   Sprites *answer = NULL;
   Sprites *remaining = flipped_followers();
   while (remaining != NULL) {
		Sprite *sprite = remaining->first();
		Sprites *more = sprite->reachable_non_equal_sprites_of_type(type);
		insert_sprites_at_end_if_not_equal_member(answer,more);
		remaining = remaining->rest();
	};
   remaining = unflipped_followers();
   while (remaining != NULL) {
		Picture *sub_picture = (Picture *) remaining->first();
		Sprites *more = sub_picture->reachable_non_equal_sprites_of_type(type);
      insert_sprites_at_end_if_not_equal_member(answer,more);
      remaining = remaining->rest();
   };
   return(answer);
};

//unsigned int java_picture_index = 1;   // should this be more global?? -- I think not - it is now (261100) maintained in toontalk.ini

Sprites *java_pictures_checked_for_flipped_followers = NULL; // new on 031200
Sprites *java_pictures_initialized = NULL;

void release_java_pictures() {
	just_destroy_list(java_pictures_checked_for_flipped_followers);
	just_destroy_list(java_pictures_initialized);
};

string Picture::java_start(Cubby *cubby, Text *text_pad, Notebook *notebook,
                           Picture *picture, Sprites *more_robots, Sprites *more_notebooks, Sprite *display_this, java_string extra_text) {
   if (is_flipped() || code == SCALE_SPRITE) return(NULL);
   Text *text = NULL;
   Sprites *remaining = flipped_followers();
   while (remaining != NULL) {
//      if (remaining->first()->kind_of() == TEXT) {
//         Text *found_text_pad = (Text *) (remaining->first());
		Sprite *possible_text_pad = remaining->first(); // updated on 081200 since could be a picture that indirects to text
		long the_text_length;
		string the_text;
		if (possible_text_pad->kind_of() != INTEGER && possible_text_pad->current_text(the_text,the_text_length)) { 
			// made a conditional on 211200 -- added first condition on 230101
			if (the_text_length > 0 && the_text_length <= 31 && text == NULL) { // changed from 30 to 31 on 260101
				// new on 201100 ignore "long" directions (or an empty text pad) -- added text == NULL on 260101 in case 2 short text pads on back
	//			text = (Text *) found_text_pad->copy();
				text = new Text();
				text->set_text(the_text,the_text_length); // re-written 081200
	//			break;
			} else { // else might be a nice comment to add to the HTML
				if (extra_text == NULL) {
					extra_text = copy_string(the_text,the_text_length);
				} else {
					java_string new_extra_text = append_strings(extra_text,the_text); // should put a line separator here
					delete [] extra_text;
					extra_text = new_extra_text;
				};
			};
		};
      remaining = remaining->rest();
   };
	if (text == NULL && comment != NULL && comment->kind_of() == TEXT) { // new on 190704
		text = (Text *) comment->copy();
	};
   Sprites *robots_on_flipside = robots_on_the_back();
   Sprites *robots = reachable_non_equal_sprites_of_type(ROBOT); // non_equal added on 250601
	Sprites *notebooks = reachable_non_equal_sprites_of_type(PROGRAM_PAD); // new on 041200
	Picture *top_level_picture; // new on 190704
	if (!rectangular()) { 
		// Java does poorly with repaining non-rectangular top-level pictures so put this on a rectangle
		top_level_picture = new Picture(llx,lly,0,tt_gray,SYNTHETIC_SPRITE,width,height);
		top_level_picture->add_follower(this);
	} else {
		top_level_picture = this; // no need for a background
	};
   if (dereference()->kind_of() == PICTURE || robots_on_flipside == NULL) {
      if (text == NULL) {
         // is Picture1 etc best default names here???
         text = new Text();
			text->set_text(S2_capitalize_first(IDS_JAVA_PICTURE,increment_ini_int(AC(IDC_SWITCHES),"JavaPictureCounter",FALSE))); 
			// java_picture_index++)); - updated 261100
      };
      Robot *robot = (Robot *) tt_toolbox->pointer_to_robot_stack()->copy();
      robot->java_start(NULL,text,NULL,top_level_picture,robots,notebooks,NULL,extra_text);
      robot->destroy();
   } else  {  // is not really a picture -- e.g. a "picture" of a number with robots on the back
      Robot *robot = (Robot *) (robots_on_flipside->first());
//      robot->java_start(robot->pointer_to_working_cubby(),text,NULL,NULL,robots,this);
      robot->java_start(NULL,text,NULL,NULL,robots,notebooks,top_level_picture,extra_text);
   };
	if (top_level_picture != this) {
		top_level_picture->remove_follower(this);
		top_level_picture->destroy();
	};
	string java_applet_name = NULL;
	if (text != NULL) {
		java_applet_name = text->copy_narrow_text_string();
		text->destroy();
	};
	just_destroy_list(robots);
	just_destroy_list(robots_on_flipside);
	just_destroy_list(notebooks);
	return(java_applet_name);
};

//long picture_counter = 1;

java_string Picture::java_name() {
   return((java_string) S2(IDS_JAVA_PICTURE,java_index(this))); 
};
//   long old_counter = (long) item_to_add;
//   if (old_counter > 0 && old_counter < picture_counter) {
//      return(S2(IDS_JAVA_PICTURE,old_counter));
//   } else {
//      string name = S2(IDS_JAVA_PICTURE,picture_counter);
//      item_to_add = (Sprite *) picture_counter; // re-using slot since can't be meaningful during Java output
//      picture_counter++;
//      return(name);
//   };
//};

void Picture::java_before(java_stream &out) {
#if TT_DEBUG_ON
   if (debug_counter == tt_debug_target) {
		log("About to java_before a picture debug target.");
	};
#endif
   if (is_blank()) return;
   if (code == SCALE_SPRITE || code == HIT_OR_MISS_SPRITE) return; // not pictures in Java
//		 controlee != NULL) return; // added 220499 - not sure how to handle remote looks - rewritten on 140600
   if (has_java_index(this)) { // java_pictures != NULL && java_pictures->member_cons(this) != NULL) {
      return; // already declared this
   };
	if (controlee != NULL) {
		if (tt_java_picture_name == NULL || contained_in(THOUGHT_BUBBLE)) { // new on 161100 - updated on 100101 with first disjunct
			controlee->java_before(out);
			if (contained_in(THOUGHT_BUBBLE)) return;
		};
	};
	if (indirection != NULL && indirection->kind_of() == PICTURE) {
		indirection->java_before(out); // added 140600 in case indirection hasn't been referenced yet
	};
   java_declare1(out,S(IDS_JAVA_TT_PICTURE),java_name());
   long args[9];
   if (subpicture()) {
      args[0] = current_user_value(X_REMOTE); // should be updated to deal with non-integers
      args[1] = current_user_value(Y_REMOTE);
      args[2] = current_user_value(WIDTH_REMOTE);
      args[3] = current_user_value(HEIGHT_REMOTE);
   } else {  // fill up the PictureFrame
      args[0] = 500;
      args[1] = 500;
		if (is_flipped()) {
			// new on 050503
			args[2] = user_x(return_width_before_flip());
			args[3] = user_y(return_height_before_flip());
		} else {
			args[2] = 1000;
			args[3] = 1000;
		};
   };
//   args[4] = current_user_value(X_SPEED_REMOTE); // should be updated to deal with non-integers
//   args[5] = current_user_value(Y_SPEED_REMOTE);
	double arg4 = 0.0;
	NumberValue *value = current_speed(X_SPEED_REMOTE,FALSE);
	if (value != NULL) {
		value->double_float_value(arg4,TRUE);
	} else {
		args[4] = 0;
	};
	double arg5 = 0.0;
	value = current_speed(Y_SPEED_REMOTE,FALSE);
	if (value != NULL) {
		value->double_float_value(arg5,TRUE);
	} else {
		args[5] = 0;
	};
   const_java_string showing_all = java_boolean(show_all());
   const_java_string seen = java_boolean(visible());
	const_java_string flipped = java_boolean(is_flipped());
   if (indirection != NULL) {
		if (indirection->kind_of() == INTEGER || indirection->kind_of() == TEXT) { // new on 170201 - correct by a quarter a character width and height
			args[0] += user_x(indirection->return_character_width()/4);
			args[1] += user_y(indirection->return_character_height()/4);
		};
      java_new(out,S(IDS_JAVA_TT_INDIRECT_PICTURE),indirection,args,6,arg4,arg5,showing_all,seen,flipped);
   } else if (controlee != NULL) {
		string picture_name = NULL;
		//following doesn't work since can refer to subpictures by an index - 
		//Java will need to while initializing hold a reference to every picture for this to work
		//if (contained_in(NEST) && tt_java_picture_name != NULL) { // on a nest and know how to find the controlee
		//	// new on 010401 since nests "insulate" from changing allegience
		//	boolean ok = TRUE; // path is ok unless found otherwise
		//	Path *path = controlee->ultimate_leader()->path_to(controlee,ok);
		//	if (ok) {
		//		picture_name = new char[1024];
		//		output_string_stream stream(picture_name,1024);
		//		stream << tt_java_picture_name;
		//		path->java(stream);
		//		stream.put('\0');
		//		delete path;
		//	};
		//}; 
		if (picture_name == NULL) { // didn't set it above
			if (tt_java_picture_name != NULL) { // new on 041100
				picture_name = copy_string(tt_java_picture_name); // added copy_string on 300301 so can clean up regardless of which path
			} else {
				picture_name = copy_string(controlee->java_name()); // controlee-> added on 100101 - not sure why this calls copy_string
			};
		};
      java_new(out,S(IDS_JAVA_TT_REMOTE_APPEARANCE),picture_name,args,6,arg4,arg5,showing_all,seen,flipped);
		delete [] picture_name;
   } else {
      switch (code) {
         case SYNTHETIC_SPRITE:
            {
            COLORREF color = tt_colors[current_parameter()%256];
            args[6] = GetRValue(color);
            args[7] = GetGValue(color);
            args[8] = GetBValue(color);
            java_new(out,S(IDS_JAVA_TT_RECTANGLE+current_parameter()/256),args,9,arg4,arg5,showing_all,seen,flipped);
            };
            break;
         case USER_SPRITE:
            // think about generating full path or copying file to Java directory...
				// full path can be bad - 
				// e.g. ToonTalk installed to different directories on different machines
            // commented out since don't if the code will flip this over and add it to the main picture - sigh - could have a user flag...
 //           if (is_flipped() || on_backside()) { // conditional new on 130702 since if flipped why copy files to Java directory
 //              java_new_quote_arg1(out,S(IDS_JAVA_TT_GIF_PICTURE),_T("unknown.gif"),java_boolean(current_image->rectangular()),args,6,showing_all,
												 //seen,flipped);
 //           } else 
            {
//               java_string bmp_file_name = pointer_to_current_image()->copy_full_file_name(); - commented out on 210303
					char user_java_directory[MAX_PATH]; // new on 210303
					strcpy(user_java_directory,tt_user_directory);
					strcat(user_java_directory,AC(IDC_JAVA_PATH));
					// or jpg?
					java_string gif_file_name = pointer_to_current_image()->save_gif_file(user_java_directory);
//               java_string gif_file_name = ensure_file_for_java(pointer_to_current_image()->return_file_name(),PICTURE,FALSE); 
					// was gif_file_for_java(bmp_file_name) prior to 120201
               if (gif_file_name != NULL) { // there is a gif equivalent so use that
//                int id = current_image->return_resource_index(); // obsolete on 180800
                  java_new_quote_arg1(out,S(IDS_JAVA_TT_GIF_PICTURE),gif_file_name+strlen(user_java_directory), // get short name beginning 210303
												 java_boolean(pointer_to_current_image()->rectangular()),args,6,arg4,arg5,showing_all,seen,flipped); 
						// added is_flipped on 260101
                  delete [] gif_file_name;
        //       } else {
					   //// this only works as an application not an applet...
        //          java_new_quote_arg1(out,S(IDS_JAVA_TT_BMP_PICTURE),bmp_file_name,args,6,showing_all,seen,flipped);
               };
//				   delete [] bmp_file_name;
            };
            break;
	      default:
            if (still_picture(code)) {
               int id = pointer_to_current_image()->return_resource_index(); 
               java_string file_name = ensure_file_for_java(id,kind_of());
//               if (!is_flipped() || on_backside()) { // conditional new on 130702
//                  file_name = ensure_file_for_java(id,kind_of());
//               };
					// commented out id from the following on 260101
               if (file_name == NULL) {
						// new on 021204 to generate the GIF if it has been installed
						char user_java_directory[MAX_PATH]; // new on 210303
						strcpy(user_java_directory,tt_user_directory);
						strcat(user_java_directory,AC(IDC_JAVA_PATH));
						file_name = pointer_to_current_image()->save_gif_file(user_java_directory);
						if (file_name == NULL) { // problems
							java_new_quote_arg1(out,S(IDS_JAVA_TT_GIF_PICTURE),_T("unknown.gif"),java_boolean(pointer_to_current_image()->rectangular()),
													  args,6,arg4,arg5,showing_all,seen,flipped);
						} else {
							java_new_quote_arg1(out,S(IDS_JAVA_TT_GIF_PICTURE),file_name+strlen(user_java_directory), // get short name 
													  java_boolean(pointer_to_current_image()->rectangular()),args,6,arg4,arg5,showing_all,seen,flipped);
						};
               } else {
                  java_new_quote_arg1(out,S(IDS_JAVA_TT_GIF_PICTURE),file_name,java_boolean(pointer_to_current_image()->rectangular()),
												  args,6,arg4,arg5,showing_all,seen,flipped);
                  delete [] file_name;
               };
            } else {
               ensure_image_cycles(code);
					args[6] = parameter; // parameter added on 230700
               java_new(out,S(IDS_JAVA_TT_IMAGE_CYCLE_PICTURE),(int) code,S(IDS_JAVA_FALSE),args,7,arg4,arg5,showing_all,seen,flipped); 
            };
      };
   };
	out << ";" << endl;
};

void Picture::java(java_stream &out) {
   if (is_blank()) {
		if (indirection != NULL) { 
			// new on 270201
			indirection->java(out); // to get the type right
		} else {
			out << S(IDS_JAVA_TT_BLANK_PICTURE);
			if (!contained_in(THOUGHT_BUBBLE)) { // new on 061200 since otherwise it is shared
				out << ".copy()"; // could add to javacom.rc 
			};
		};
	} else if (controlee != NULL && contained_in(THOUGHT_BUBBLE)) { // new on 161100
		controlee->java(out);
   } else {
      switch (code) {
			case SCALE_SPRITE: {
            char relation_character;
//            update_display(); // to be sure it is up to date - new on 100502
				Relation relation = compute_relation(); // new on 050503
				switch (relation) {
					case GREATER_THAN_RELATION:
						relation_character = '>';
						break;
					case EQUAL_RELATION:
						relation_character = '=';
						break;
					case LESS_THAN_RELATION:
						relation_character = '<';
						break;
					default:
						relation_character = '?';
				};
				// rewrote this on 050503 so it works regardless of whether the scale has been updated or set by keyboard
           // switch (current_parameter()) {
		         //case EQUAL_SCALE: // constant
           //    case EQUAL_FROM_GREATER_SCALE:
		         //case EQUAL_FROM_LESS_SCALE:
			        // relation = '=';
           //       break;
		         //case GREATER_THAN_SCALE:
           //    case GREATER_THAN_FROM_EQUAL_SCALE:
			        // relation = '>';
           //       break;
		         //case LESS_THAN_SCALE:
           //    case LESS_THAN_FROM_EQUAL_SCALE:
			        // relation = '<';
           //       break;
		         //default:
			        // relation = '?';
           // };
            java_new(out,S(IDS_JAVA_TT_SCALE),relation_character);
            break;
									 };
         case HIT_OR_MISS_SPRITE:
            // just treat this like a 0 or 1 as boolean whether it is colliding
            java_new(out,"TTNumber",(int) (current_parameter() != NOT_COLLIDING)); // was S(IDS_JAVA_TT_INTEGER) prior to 040503
            break;
         default:
            out << java_name();
      };
   };
};

void Picture::java_after(java_stream &out, const_java_string variable) {
	// re-modularized on 031200
	if (is_blank()) return; // new on 041200
	if (java_pictures_initialized != NULL && java_pictures_initialized->member_cons(this) != NULL) { // new on 041200
		return; 
	};
#if TT_DEBUG_ON
   if (debug_counter == tt_debug_target) {
		log("About to java_after a picture debug target.");
	};
#endif
	if (is_flipped() && !is_temporarily_flipped() && leader != NULL && leader->kind_of() != PICTURE) { // new on 071200
		java_send(out,java_name(),"setFlipped",S(IDS_JAVA_TRUE));
	};
	java_pictures_initialized = new Sprites(this,java_pictures_initialized);
	UnwindProtect<java_string> set(tt_java_picture_name,copy_string(java_name()));
	java_followers(out,tt_java_picture_name);
	java_flipped_followers(out,tt_java_picture_name);
	delete [] tt_java_picture_name;
};

unsigned short int internal_sub_picture_counter;
Sprites *followers_to_convert_to_java = NULL; // new on 290101 - re-written so initialize() won't exceed 64K

int add_to_followers_to_convert_to_java(Sprite *sprite, boolean *old) { // abstracted into a procedure on 290301
	// good_size code new on 100405 since geometry of pictures in boxes can be way off otherwise
	// commented out on 120405 since this wasn't the problem
	//if (sprite->pointer_to_leader() != NULL && sprite->pointer_to_leader()->kind_of() == CUBBY) { 
	//	sprite->set_to_good_size(); // reset after emitting the Java code
	//	sprite->really_propagate_changes();
	//};
	int index_if_member;
	if (followers_to_convert_to_java == NULL) {
		followers_to_convert_to_java = new Sprites(sprite);
		internal_sub_picture_counter = 1; // first one
		if (old != NULL) *old = FALSE;
	} else if (followers_to_convert_to_java->insert_at_end_if_not_equal_member(sprite,&index_if_member)) { 
		// equal test added on 250601
		internal_sub_picture_counter++;
		if (old != NULL) *old = FALSE;
	} else {
		if (old != NULL) *old = TRUE;
		return(index_if_member+1); 
		// new on 250601 - set by call to insert_at_end_if_not_equal_member above -- added +1 on 120701 since these are 1-indexed
	};
	return(internal_sub_picture_counter);
};

int sub_picture_counter(Sprite *sprite) {
	if (followers_to_convert_to_java == NULL) {
		return(-1); // ok? warn?
	} else {
		boolean found;
		int answer = followers_to_convert_to_java->index_of_using_equal(sprite,found);
		if (found) {
			return(answer);
		} else { // warn?
			return(-1);
		};
	};
};

void Picture::java_followers(java_stream &out, const_java_string picture_name) {
   if (unflipped_followers() == NULL) return;
   tt_java_indent += 2;
   Sprites *remaining = unflipped_followers();
	//if (remaining != NULL) {
	//	really_propagate_changes(); // new on 070405 since if in box on back then geometry may not yet be up-to-date - removed 070405
	//};
   while (remaining != NULL) {
		Sprite *item = remaining->first();
		if (item->kind_of() == PICTURE) { // new on 080403 since can be other stuff -
			// e.g. when is a picture that is missing its image file - the text can be here
			Picture *follower = (Picture *) remaining->first();  
//       follower->java_as_follower(out); // restructured on 290101 and then commented out to make smaller methods
			boolean reused;
			int sub_picture_counter = add_to_followers_to_convert_to_java(follower,&reused);
			// javacom.rc and java.cpp could be updated to clean up the following:
			java_indent(out);
			out << picture_name << "." << S(IDS_JAVA_ADD_PICTURE) 
				 << "(" << tt_java_class_name << ".PicturePart" << sub_picture_counter;
			out << "(notebook, " << java_name() << ")";  
			// beginning 141003 adding an extra arg to calls to PicturePart - e.g. if a remote control is on the front of a picture
			if (reused) {
				long args[4]; // was 6 prior to 210205 but now speeds are double floats
				args[0] = follower->current_user_value(X_REMOTE);
				args[1] = follower->current_user_value(Y_REMOTE);
				args[2] = follower->current_user_value(WIDTH_REMOTE);
				args[3] = follower->current_user_value(HEIGHT_REMOTE);
//				args[4] = follower->current_user_value(X_SPEED_REMOTE);
//				args[5] = follower->current_user_value(Y_SPEED_REMOTE);
				out << ".resetGeometry(";
				java_int_args(out,args,4);
				// updated 210205
				double double_arg = 0.0;
				NumberValue *value = current_speed(X_SPEED_REMOTE,FALSE);
				if (value != NULL) {
					value->double_float_value(double_arg,TRUE);
					out << ", " << double_arg;
				} else {
					out << ", 0";					
				};
				value = current_speed(Y_SPEED_REMOTE,FALSE);
				if (value != NULL) {
					value->double_float_value(double_arg,TRUE);
					out << ", " << double_arg;
				} else {
					out << ", 0";				
				};
				out << ")";
			};
			out << ");" << endl; 
//		java_send(out,picture_name,S(IDS_JAVA_ADD_PICTURE),make_follower);
//		delete [] make_follower;
		}; // otherwise ignore it
      remaining = remaining->rest();
   };
   tt_java_indent -= 2;
};

void java_postponed_followers(java_stream &out) {
	Sprites *remaining = followers_to_convert_to_java;
	int counter = 1;
	while (remaining != NULL) {
		java_indent(out);
		out << "public static TTPicture PicturePart" << counter;
		out << "(TTNotebook notebook, TTPicture container) { " << endl; 
		// beginning 141003 adding an extra arg to calls to PicturePart - e.g. if a remote control is on the front of a picture
//			out << "(TTNotebook notebook) { " << endl;
		Picture *follower = (Picture *) remaining->first();
		tt_java_indent += 2;
		follower->java_as_follower(out);
		// commented out on 120405 since this wasn't the problem
		//if (follower->pointer_to_leader() != NULL && follower->pointer_to_leader()->kind_of() == CUBBY) { // new on 100405
		//	follower->pointer_to_leader()->refit_contents();
		//	// OK that this may have set clean status??
		//};
		tt_java_indent -= 2;
		java_indent(out);
		out << "}" << endl;
		counter++;
		remaining = remaining->rest();
	};
	just_destroy_list(followers_to_convert_to_java);
};		

void Picture::java_as_follower(java_stream &out) {
	if (is_blank()) { 
		// new on 140201 - this is a weird situation that is hard to create 
		// (need to erase with remote looks after adding it to another picture)
		java_indent(out);
		out << "return TT.BLANK_PICTURE;" << endl;
		return;
	};
	java_before(out);
	UnwindProtect<java_string> set(tt_java_picture_name,copy_string(java_name())); // SC(IDS_JAVA_PICTURE,sub_picture_counter++);
//      java_declare(out,S(IDS_JAVA_TT_PICTURE),tt_java_picture_name,follower);
//   java_send(out,picture_name,S(IDS_JAVA_ADD_PICTURE),tt_java_picture_name); // move up and rewritten on 290101
//      follower->java_flipped_followers(out,tt_java_picture_name); // was // picture_name prior to 021100
//      follower->java_followers(out,tt_java_picture_name); // picture_name
	java_after(out,NULL); // new on 041200 to replace the above
	java_indent(out);
	out << "return " << tt_java_picture_name << ";" << endl; // new on 290101 (could add to java.cpp)
   delete [] tt_java_picture_name;
	tt_java_picture_name = NULL; // new on 081003 -- after generating 3.14
};

void Picture::java_flipped_followers(java_stream &out, const_java_string picture_name) {
	if (controlee != NULL) return; // new on 171200 - remote looks doesn't really have anything on back
   if (no_flipped_followers()) return;
	if (java_pictures_checked_for_flipped_followers != NULL && 
		 java_pictures_checked_for_flipped_followers->member_cons(this) != NULL) { 
		// already taken care of
		return;
	};
	java_pictures_checked_for_flipped_followers = new Sprites(this,java_pictures_checked_for_flipped_followers);
   boolean picture_name_was_null = (picture_name == NULL);
	UnwindProtect<java_string> save(tt_java_picture_name);
   if (picture_name_was_null) {
		java_before(out); // just in case 
      picture_name = copy_string(java_name());
      tt_java_picture_name = picture_name;
   };
#if TT_DEBUG_ON
	if (tt_debug_mode == 31200) {
		print(tt_error_file());
		tt_error_file() << " is being checked for flipped followers for Java where it is bound to " << tt_java_picture_name << endl;
	};
#endif
   tt_java_indent += 2;
   boolean team_added = java_flipped_followers_each(out,picture_name,flipped_followers());
   if (picture_name_was_null) {
      if (team_added) {
			if (is_flipped()) { 
				java_send(out,java_name(),"setFlipped",S(IDS_JAVA_TRUE)); // new on 161100 (could add to javacom.rc)
			} else { // if (leader == NULL || (leader->kind_of() != PICTURE || !leader->is_flipped())) { 
				// conditions added 301100 so unflipped pictures on back of others don't run
				// commented out since shouldn't have gotten this far
				java_send(out,java_name(),S(IDS_JAVA_START));
			};
      };
      delete [] tt_java_picture_name;
		tt_java_picture_name = NULL; // new on 081003
   };
   tt_java_indent -= 2;
};

int temp_variable_counter = 0;

boolean Picture::java_flipped_followers_each(java_stream &out, const_java_string picture_name, Sprites *remaining) {
	// re-modularized on 130800
	boolean team_added = FALSE;
	Sprites *original_followers = remaining;
	while (remaining != NULL) {
      Sprite *sprite = remaining->first();
      switch (sprite->kind_of()) {
         case ROBOT: {
            Robot *robot = (Robot *) sprite;
            Cubby *cubby = robot->pointer_to_working_cubby();
				if (cubby != NULL) { // condition new on 070405 since a robot alone on the back of a picture should do nothing in Java
					robot->java_team(out,cubby);
					java_send(out,picture_name,S(IDS_JAVA_ADD_TEAM),S2(IDS_JAVA_TEAM,java_index(robot)));
					team_added = TRUE;
//             if (cubby != NULL) cubby->java_flipped_followers(out,picture_name); -- handled elsewhere now
            };
							};
            break;
			case PICTURE: // new on 290301 so these are defined in their own method
				{ 
					int sub_picture_counter = add_to_followers_to_convert_to_java(sprite,NULL);
					// javacom.rc and java.cpp could be updated to clean up the following:
					java_indent(out);
					out << picture_name << ".addOnBack("  << tt_java_class_name << ".PicturePart" << sub_picture_counter;
					// notebook below was null prior to 161203
					out << "(notebook, " << java_name() << "));" << endl; 
					// beginning 141003 adding an extra arg to calls to PicturePart -
					// e.g. if a remote control is on the front of a picture
//					out << "(null));" << endl;	 
				};
				break;
				// commented out the following on 260101 to preserve the structure of which sub picture teams are associated with 
				// (since can be removed now)
//         case PICTURE:
//				{
//					Picture *picture = (Picture *) sprite; 
//					if (picture->pointer_to_controlee() == NULL && picture->is_flipped()) { 
				// condition added 130600 - first condition added 171200
//						boolean another_team_added = java_flipped_followers_each(out,picture_name,picture->flipped_followers()); 
				// new on 130600
//						// re-written on 210700 so that this happens regardless of whether team_added is TRUE
//						team_added = (team_added || another_team_added);									  
//						break;
//					}; // else fall thru
//				};
			default: 
				boolean skip_this_one = FALSE; // new on 040201 since cubbies were on_back and part of team
				if (sprite->kind_of() == CUBBY) { // new on 011200 to skip cubbies that are associated with robots
					Sprites *remaining = original_followers;
					while (remaining != NULL) {
						Sprite *follower = remaining->first();
						if (follower->kind_of() == ROBOT && sprite == ((Robot *) follower)->pointer_to_working_cubby()) {
							skip_this_one = TRUE;
							break;
						};
						remaining = remaining->rest();
					};
				};
				if (!skip_this_one) {
					sprite->java_before(out);
					string sprite_name;
					if (sprite->kind_of() == PICTURE) {
						// new on 031200 since too many "temps" causes confusion and hard to read code (and some conflicts)
						sprite_name = copy_string(((Picture *) sprite)->java_name());
					} else {
						sprite_name = SC(IDS_JAVA_TEMP,temp_variable_counter++); 
						// new on 031200 - somehow using block structure and "temp" didn't work...
						java_declare(out,S(IDS_JAVA_TT_OBJECT),sprite_name,sprite);
					};
					sprite->java_after(out,sprite_name);
	//				if (leader == NULL || leader->kind_of() == PICTURE) { 
					// conditions new on 121200 - commented out on 010201 since is on the back of "this"
						java_send(out,java_name(),"addOnBack",sprite_name);
	//				};
					delete [] sprite_name;
	//				sprite->java_flipped_followers(out,NULL); // new on 031200 - commented out on 041200 since handled by java_after now
	//				java_begin_block(out); // new on 011200 (for temp variable)
	//					java_declare(out,S(IDS_JAVA_TT_OBJECT),S(IDS_JAVA_TEMP),sprite);
	//					sprite->java_after(out,"temp"); // was S(IDS_JAVA_TEMP) prior to 031200 but entry can be recycled
	//					java_send(out,java_name(),"addOnBack",S(IDS_JAVA_TEMP)); // updated on 011200 (could add to javacom.rc)
	//					java_end_block(out); 
	//				};
				};
//				break;
      };
      remaining = remaining->rest();
   };
	return(team_added);
};

const city_coordinate backside_width_on_floor = tile_width*10;
const city_coordinate backside_height_on_floor = tile_height*10;
const city_coordinate label_fraction = 7; // 1/7 of the height

void Picture::backside_size(city_coordinate &w, city_coordinate &h, Sprite *source) {
   w = backside_width_on_floor;
   h = backside_height_on_floor;
	if (tt_good_sizes_adjust_to_screen_size) {
		w = shrink_width_from_640x480_screen_size(w);
		h = shrink_height_from_640x480_screen_size(h);
	};
	// re-ordered on 311003 so can return if standing
	if (floor != NULL) {
		if (floor->in_the_room_with_programmer()) { // updated 020205
			tt_programmer->translate_floor_to_room_dimensions(w,h);
			return;
		};
	} else if (tt_programmer != NULL && tt_programmer->kind_of() == PROGRAMMER_ROOM_WALKING) { 
		// new on 311003 to deal with objects dereferenced by robot that don't have a floor 
		tt_programmer->translate_floor_to_room_dimensions(w,h);
		return; // new on 311003
	};
 //  if (floor != NULL && floor->in_the_room()) {
 //     tt_programmer->translate_floor_to_room_dimensions(w,h);
 //  };
	//if (tt_good_sizes_adjust_to_screen_size) {
	//	w = shrink_width_from_640x480_screen_size(w);
	//	h = shrink_height_from_640x480_screen_size(h);
	//};
	adjust_size_for_source(w,h,source);
};

/* obsolete
boolean Picture::worth_dumping_relative_geometry(boolean is_controlee, boolean on_nest, boolean flipped_originally) {
	boolean non_unflipped_picture_leader = (leader != NULL && leader->kind_of() == PICTURE && !leader->is_flipped());
	return(!tt_dumping_to_test_equality_or_describe && 
		    // added leader condition to the following on 120200 since it may be a controlee but also on the floor
			 // and if this is the first occurance it'll be dumped without its relationship to its leader
			 (!is_controlee || non_unflipped_picture_leader) && // added !is_controlee on 140699
			 !on_nest && // added on 120799
			 (// leader != NULL && leader->kind_of() == PICTURE && 
			 // changed on 080699 since a city or room with moving things should not stop
			  (leader == NULL || non_unflipped_picture_leader) && // new policy is if leaderless parameters saved as well 
			 // && !leader->is_flipped() added on 060899 for pictures of text on back of flipped pictures, etc.
			  !flipped_originally));
};

void Picture::dump_relative_geometry(output_stream &stream) {
//	if (!tt_dumping_to_test_equality_or_describe && !is_controlee && // added !is_controlee on 140699
//		 !on_nest && // added on 120799
//		 (// leader != NULL && leader->kind_of() == PICTURE && 
// changed on 080699 since a city or room with moving things should not stop
//		  (leader == NULL || (leader->kind_of() == PICTURE && !leader->is_flipped())) && 
// new policy is if leaderless parameters saved as well 
//		  // && !leader->is_flipped() added on 060899 for pictures of text on back of flipped pictures, etc.
//		  !flipped_originally)) {
		 // dump relative position, width and like
		for (int j = 0; j < saved_remotes_count; j++) {
			short int value = (short int) current_user_value(location_and_size[j]);
			stream.write((character *) &value, sizeof(value));	
#if TT_DEBUG_ON
			if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
				print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
				tt_error_file() << value << " ";
				if (j+1 == saved_remotes_count) tt_error_file() << endl;
			};
#endif
		};
};
*/

boolean Picture::non_zero_speed() { // new on 070302 to match old scheme (and save a byte or two)
	if (x_speed != NULL && x_speed->sign() != 0) return(TRUE);
	if (y_speed != NULL && y_speed->sign() != 0) return(TRUE);
	return(FALSE);
};

unsigned char Picture::combine_flags() { // abstracted out on 210601 for use by equal_to
	return((unsigned char) (128*non_zero_speed()+64*!graspable()+32*infinite_stack()+ //64*dump_geometry+
									16*!active()+8*show_some_if_subpicture()+is_flipped()*4+
                           visible()*2+show_all()));
};

boolean Picture::dump(output_stream &stream, boolean just_prepare, boolean is_controlee, boolean on_nest) {
   if (just_prepare) return(FALSE);
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log(_T("Dumping debug target."));
	};
#endif
//	boolean dump_geometry = worth_dumping_relative_geometry(is_controlee,on_nest,flipped);
	boolean flipped_originally = is_flipped();
   int dump_index;
   if (appearance_code() != SCALE_SPRITE && // added conjunct on 081299 since can't share scales
      (remote != NULL || controlee != NULL || tt_dump_so_equals_share)) { 
			// new on 270900 - no need to consider sharing if no remote controls involved
		 // or as of 040601 if tt_dump_so_equals_share
      dump_index = about_to_dump(this,stream); // rewritten on 020802
	   if (dump_index < 0) return(FALSE); 
		// following is new on 300699 so that geometry is saved even if not first reference
		// following seems like a bad idea
		// commented out on 181199 since loader doesn't even check for this and it is a bad idea
//		if (dump_geometry) {
//#if TT_DEBUG_ON
//			if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
//				tt_error_file() << "Dumping geometry of OLD item" << endl;
//			};
//#endif
//			dump_relative_geometry(stream);
//		};
//		return(FALSE);
	};
	// moved here from above on 251000 and then commented out since broke saving of the Playground version of the Pong game...
//	if (indirection != NULL && indirection->kind_of() == PICTURE && followers == NULL && flipped_followers() == NULL) { 
	// new on 241000
//		return(indirection->dump(stream,just_prepare,is_controlee,on_nest));
//	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
		print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
		tt_error_file() << "About to dump ";
		print(tt_error_file());
		tt_error_file() << " image_code=" << (int) code; // << " dump_geometry=" << (int) dump_geometry;
		tt_error_file() << endl;
		tt_debug_dump_depth++;
	};
#endif
 // re-write this someday to not flip and unflip but used flipped_followers() and the like...
// just grasping it (selecting) stopped it...
//   stop_all(); // in case robots are running on back
//	if (flipped) {
//		return; // can happen if equal is called
//	};
// following can cause problems in the wrong context -- if revived then make sure isn't a controlee for example
//	recursively_propagate_changes();  // typically just dropped into note book
	// following is new on 171000 to deal with pictures getting too small and losing their relative geometry
	// replaced leader->kind_of() != PICTURE with !leader->is_picture() on 251000 since could be roof of house for example
	boolean temporarily_reset_size = ok_to_temporarily_reset_size() && !flipped_originally; 
	// add flipped condition on 060201 since no point if flipped
	// added tt_dumping_to_test_equality_or_describe on 181000 to prevent size changes when doing equal tests or describing
	// assuming that fit_contents or the like will restore things
//	city_coordinate llx_saved = llx;
//	city_coordinate lly_saved = lly;
	city_coordinate width_saved = width;
	city_coordinate height_saved = height;
	const int64 new_dimension = minimum_dimension; // for accuracy increase the greater dimension to some large number 
	if (temporarily_reset_size && height > width && height < new_dimension) {
		city_coordinate new_width = (city_coordinate) ((width*new_dimension)/height); 
		// should compute this was 64 bits and then should always fit
		set_size(new_width,new_dimension,TEMPORARY_FOR_DUMPING);
	} else if (temporarily_reset_size && width > height && width < new_dimension) {
		city_coordinate new_height = (city_coordinate) ((height*new_dimension)/width); 
		set_size(new_dimension,new_height,TEMPORARY_FOR_DUMPING);
	} else {
		temporarily_reset_size = FALSE; // didn't need to since was big already
	};
   ascii_string user_picture_file_name;
   if (controlee != NULL) {
		stream.put((unsigned char) REMOTE_APPEARANCE);
		dump_if_not_null(stream,comment);
//      if (tt_dumping_background != NULL) {
         controlee->dump(stream,just_prepare,TRUE); // TRUE added on 140699
//      } else {
//         stream.put(NOTHING_MARKER);
//      };
	} else if (indirection != NULL) {
		stream.put((unsigned char) PICTURE_WITH_INDIRECTION);
		dump_if_not_null(stream,comment);
		indirection->dump(stream);
   } else if ((user_picture_file_name = return_file_name()) != NULL) {
      if (is_blank()) {
         stream.put((unsigned char) BLANK_USER_PICTURE); // was this a good idea??
			dump_if_not_null(stream,comment);
      } else {
         stream.put((unsigned char) USER_PICTURE);
			dump_if_not_null(stream,comment);
      };
		string saved_file_name;
		if (tt_dump_reason == DUMPING_TO_CLIPBOARD && tt_mouse_button&SHIFT_KEY_DOWN) { 
			// new on 160200 so control-shift-c saves short names
			// added tt_dumping_to_clipboard on 170200 since appropriate when saving in notebooks, etc.
			saved_file_name = short_file_name(user_picture_file_name);
		} else {
			saved_file_name = copy_string(user_picture_file_name);
		};
      unsigned char file_name_length = (unsigned char) strlen(saved_file_name);
      stream.put(file_name_length);
      stream.write((character *) saved_file_name,file_name_length);
      if (tt_dump_reason == DUMPING_TO_CLIPBOARD && tt_user_initiated_dump) {
			string short_name = short_file_name(user_picture_file_name); // new on 061000
			if (!is_url(saved_file_name)) {
				if (tt_warning_count < 1) {
					say_file_name_error(IDS_REMEMBER_FILE,short_name,4);
					tt_err_file_important = TRUE; // moved here on 091000 - was set when URLs loaded earlier
				} else {
//					int importance; // added on 060200 so don't get too many messages about this (importance was 4 before)
//					if (tt_warning_count > 3) {
//						importance = 1;
//					} else {
//						importance = 2;
//					};
					say_file_name_error(IDS_AND_BEGIN_SENTENCE,short_name,1); 
					// new on 061199 - replaced importance with 1 on 091000 to minimize bothering the user
				};
				tt_warning_count++;
				delete [] short_name;
			};
	   };
		delete [] saved_file_name;
   } else if (is_blank()) {
		stream.put((unsigned char) BLANK_PICTURE);
		dump_if_not_null(stream,comment);
	} else { //if (number_inside == 0) {
		stream.put((unsigned char) PICTURE);
		dump_if_not_null(stream,comment);
//	} else {
//		stream.put((unsigned char) PICTURE_WITH_INSIDES);
	};
	unsigned short int image_index = (unsigned short) parameter;
	stream.write((character *) &image_index, sizeof(image_index));
	stream.put((unsigned char) code);
	// added 16*is_stopped() on 190799 - replace with !active() on 071299
	// following 2 fields made obsolete on 120200
	// added 32*is_next_bit_dump_geometry 111199
	// added 64*dump_geometry on 111199
	// added 128*non_zero_speed on 130200 so speed is saved only if needed
	// removed +32 on 080100 since with version first_version_to_save_programmer_and_remotes_accurately is implicit
	stream.put(combine_flags());
	if (!tt_dumping_to_test_equality_or_describe && !is_controlee && 
		// !is_controlee added on 140699 to make saving of remote looks more reliable
		 (leader == NULL || leader->kind_of() != PICTURE || is_flipped())) {
		// is_flipped() added on 180299 so geometry of flipped pictures saved with flipped pictures looks OK
		// following is default if blank
      // why use saved_width rather than "real" thing? -- because might be flipped over
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299) {
			print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
			tt_error_file() << "Dumping saved width and height." << endl;
		};
#endif
		city_coordinate value; // prior to 130200 this was saved as a user value (short int)
		if (is_flipped()) {
			value = width_before_flip;
		} else {
			value = return_saved_width(); // was just saved_width prior to 050601
		};
		stream.write((character *) &value, sizeof(value));
		if (is_flipped()) {
			value = return_height_before_flip();
		} else {
			value = return_saved_height(); // was just saved_height prior to 050601 
		};
		stream.write((character *) &value, sizeof(value));
	};
	// starting 120200 all pictures dump geometry - and in absolute coordinates
	if (!tt_dumping_to_test_equality_or_describe) {
		if (is_flipped() && (width <= 0 || height <= 0)) { // new on 201200
			set_size(width_before_flip,height_before_flip);
		};
		dump_just_geometry(stream);
		if (non_zero_speed()) { // restored conditional on 070302
//			stream.write((char *) &x_speed,sizeof(x_speed));
//			stream.write((char *) &y_speed,sizeof(y_speed));
			if (x_speed != NULL) {
				x_speed->dump(stream);
			} else {
				stream.put(NOTHING_MARKER);
			};
			if (y_speed != NULL) {
				y_speed->dump(stream);
			} else {
				stream.put(NOTHING_MARKER);
			};
		};
	};
//	unsigned short int short_parameter = current_parameter();
//	stream.write((unsigned char *) &short_parameter, sizeof(short_parameter));
//	if (number_inside > 0 && !contained_in(THOUGHT_BUBBLE)) {
//		stream.put(number_inside);
//		for (int i = 0; i < remote_cubby_count; i++) {
//			if (remote_cubbys[i] != NULL) {
//				remote_cubbys[i]->dump(stream);
//				if (remote_robots[i] != NULL) {
//					remote_robots[i]->dump(stream);
//				} else {
//					stream.put(END_OF_ROBOT_MARKER);
//				};
//			};
//		};
//	};
	city_coordinate saved_width_before_flip, saved_height_before_flip;
	boolean change_size_when_flipping = (flipped_originally && (leader==NULL || leader->kind_of() != PICTURE)); 
	// added flipped_originally on 111200
	if (flipped_originally) {
		// was unflip(FALSE prior to 240299
		// was unflip((tt_log_version_number > 14),(tt_log_version_number < 15 && leader==NULL)); // prior to 030400
		saved_width_before_flip = width_before_flip; // new on 081200
		saved_height_before_flip = height_before_flip;
		unflip(TRUE,change_size_when_flipping); 
		// was (leader==NULL) prior to 190299 to ensure proper geometry - restored on 081200 and added leader type check
	};
	unsigned short int followers_count = 0; 
	if (followers != NULL) { // note this doesn't call pointer_to_followers() since that might report followers of indirection
		followers_count = (unsigned short) followers->length();
	};
	stream.write((character *) &followers_count, sizeof(followers_count));
#if TT_DEBUG_ON
	if (tt_debug_mode == 170299) {
		tt_error_file() << "Dumping picture#" << debug_counter << " whose follower count is " << followers_count << endl;
	};
#endif
	if (followers != NULL) {
//		city_coordinate saved_width = width;
//		city_coordinate saved_height = height;
//		if (flipped_originally) {
			// added on 180299 so that geometry is correct for things on the back of this
//			width = width_before_flip;
//			height = height_before_flip;
//		};
//		boolean saved_tt_dump_so_equals_share = tt_dump_so_equals_share;
//		tt_dump_so_equals_share = FALSE;
		followers->dump(stream,FALSE,FALSE,TRUE);
//		tt_dump_so_equals_share = saved_tt_dump_so_equals_share;
//		if (flipped_originally) { // restore things
//			width = saved_width;
//			height = saved_height;
//		};
   };
//	for (int i = followers_count-1; i >= 0; i--) {
//		Sprite *follower = followers->nth(i);
//		follower->dump(stream);
//	};
//	followers = flipped_followers();
	// changed on 010599 so that new scheme is not to change size
	flip(TRUE,change_size_when_flipping); 
	// was (leader==NULL) prior to 190299 to ensure proper geometry - restored on 081200 and added leader type check
	if (flipped_originally) { // new on 071200
		set_temporarily_flipped(FALSE);
		set_width_before_flip(saved_width_before_flip); // new on 081200
		set_height_before_flip(saved_height_before_flip);
	};
	followers_count = 0;
//	boolean skipped_notebook = FALSE;
//	Sprites *remaining = followers;
//	while (remaining != NULL) {
//		if (remaining->first()->kind_of() == PROGRAM_PAD) {
//			skipped_notebook = TRUE;
//		} else {
//			followers_count++;
//		};
//		remaining = remaining->rest();
//	};
	city_coordinate saved_width = 0, saved_height = 0;
	Sprites *sorted_flipped_followers = sort_flipped_followers();
	if (sorted_flipped_followers != NULL) { 
		// prior to 240399 was just followers but if a top_cubby was temporarily removed this way it is included again
		followers_count = (unsigned short) sorted_flipped_followers->length();
//		sort_flipped_followers();
		// commented out following on 180299 since behaviors lose their size if saved while flipped
//		if (flipped_originally) { // for followers sake go to standard size  && tt_log_version_number <= 7
//			saved_width = width;
//			saved_height = height;
//         city_coordinate backside_width, backside_height;
//         backside_size(backside_width, backside_height);
//			set_size(backside_width,backside_height);
//		};
	};
	stream.write((character *) &followers_count, sizeof(followers_count));
   if (followers != NULL) {
		// following commented out on 120601 since now checking equal for unflipped pictures as well
//		boolean saved_tt_dump_so_equals_share = tt_dump_so_equals_share;
//		if (!tt_dump_so_equals_share && tt_dump_so_equals_share_enabled) { // new on 040601 
//			tt_dump_so_equals_share = TRUE;
//		};
      Sprites *remaining = sorted_flipped_followers;
	   while (remaining != NULL) {
//		   dump_follower(remaining->first(),stream);
			remaining->first()->dump(stream); // new on 250299
		   remaining = remaining->rest();
	   };
//		tt_dump_so_equals_share = saved_tt_dump_so_equals_share;
      sorted_flipped_followers->remove_all();
      delete sorted_flipped_followers;
   };
	if (!flipped_originally) {
		unflip(TRUE,FALSE); // was (leader==NULL) prior to 190299 - this restores original state
	} else if (saved_width != 0) { // restore flipped picture's size
		set_size(saved_width,saved_height);
	}; // following was on else branch prior to 051100 but need to do it even if !flipped_originally 
	if (saved_width == 0 && temporarily_reset_size) { // new on 171000
		set_size(width_saved,height_saved,TEMPORARY_FOR_DUMPING);
	};
/*
	if (dump_geometry) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
		print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
		tt_error_file() << "Dumping relative geometry ";
		print(tt_error_file());
		tt_error_file() << endl;
	};
#endif
		dump_relative_geometry(stream);
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
	*/
	return(TRUE);
};

#if TT_XML

wide_string Picture::xml_tag() { // new on 090703
	if (appearance_code() == SCALE_SPRITE) { 
		return(L"Scale"); 
	};
	if (controlee != NULL) { 
      return(L"RemoteControl");
   };
   if (indirection != NULL) {
      return(L"PictureOf");
   };
	if (appearance_code() == USER_SPRITE) { 
      return(L"UserPicture");
	};
	return(L"Picture");
};

void Picture::xml(xml_element *element, xml_document *document) {
#if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {
      log("Generating XML for debug target.");
   };
#endif
	// following moved here from obsolete xml_create_element on 120703
	if (appearance_code() == SCALE_SPRITE) { // treat Scales as a different type that happens to be implemented as a Picture
      wide_string state = NULL;
      switch (current_parameter()) {
         // first letter is all that is used on input - must be unique
			case GREATER_THAN_SCALE:
//			case GREATER_THAN_FROM_EQUAL_SCALE: // commented out on 071002 since this should be recomputed as soon as possible
            // note that I could simplify scales if I used the active flag and didn't have so many states...
            state = L"LeftLeaning";
            break;
			case LESS_THAN_SCALE:
//			case LESS_THAN_FROM_EQUAL_SCALE:
            state = L"RightLeaning";
            break;
			case EQUAL_SCALE:
//			case EQUAL_FROM_GREATER_SCALE:
//			case EQUAL_FROM_LESS_SCALE:
				state = L"Balanced";
            break;
			default:
            state = L"Moving";
		};
//      xml_element *element = create_xml_element(L"Scale",document,state);
		add_xml_value(element,document,state);
		Sprite::xml(element,document); // added Sprite:: on 290103
      return; // following stuff doesn't apply to scales
   };
	add_robots_to_flipped_followers(); 
	// new on 110505 since robots running on the back may have gotten there by truck and won't otherwise be saved
   if (controlee == NULL && indirection == NULL) {
		ascii_string user_picture_file_name = return_file_name();
		//if (user_picture_file_name = NULL && code == USER_SPRITE && current_image == NULL) { // new on 070403
		//// this catches a problem with old pictures that have never been displayed (I think)
		//// do later if needed
		//};
		hash image_hash = NULL;
		if (pointer_to_current_image() != NULL) {
			image_hash = pointer_to_current_image()->return_hash();
		};
		if (image_hash == NULL && user_picture_file_name != NULL && current_image != NULL) { 
			// new on 190803 to compute it now to dump
			int ignore_bits_per_pixel; 
			boolean ignore_adjust_pitch;
			int ignore_transparent_color;
			work_page image_surface = current_image->retrieve_image(ignore_transparent_color,ignore_bits_per_pixel,
																					  ignore_adjust_pitch);
			if (image_surface != NULL) {
				image_hash = pointer_to_current_image()->return_hash();
				delete [] image_surface; // don't need it anymore but hopefully this generated the hash properly
			};
		};
		if (user_picture_file_name != NULL || image_hash != NULL) {
			if (tt_dump_reason == DUMPING_TO_CLIPBOARD && tt_mouse_button&SHIFT_KEY_DOWN) { 
				// new on 160200 so control-shift-c saves short names
				// added tt_dumping_to_clipboard on 170200 since appropriate when saving in notebooks, etc.
				int start_of_short_name = short_file_name_start(user_picture_file_name);
				add_xml_value(element,document,user_picture_file_name+start_of_short_name);
			} else if (image_hash == NULL) { // condition new on 180303
				add_xml_value(element,document,user_picture_file_name);
			};
			if (image_hash != NULL) {
				// file name not important anymore so no need for add_xml_value
				string hash_string = hash_to_string(image_hash);
				xml_set_attribute(element,L"UniqueName",hash_string);
				delete [] hash_string;
				if (user_picture_file_name != NULL) {
					int start_of_short_name = short_file_name_start(user_picture_file_name);
					xml_set_attribute(element,L"OriginalFile",user_picture_file_name+start_of_short_name);
				};
				if (tt_saving_media) {
					insert_at_end_if_not_member(this,tt_sprites_with_media_to_save);
				};	
			};
			xml_set_attribute(element,L"PixelWidth",pointer_to_current_image()->current_pixel_width());
			xml_set_attribute(element,L"PixelHeight",pointer_to_current_image()->current_pixel_height());
			if (tt_dump_reason == DUMPING_TO_CLIPBOARD && tt_user_initiated_dump && !tt_saving_media && image_hash == NULL) {
				// added && image_hash == NULL on 0303 (approximately)
				// && !tt_saving_media on 070403
				string short_name = short_file_name(user_picture_file_name); // new on 061000
				if (!is_url(user_picture_file_name)) {
					if (tt_warning_count < 1) {
						say_file_name_error(IDS_REMEMBER_FILE,short_name,4);
						tt_err_file_important = TRUE; // moved here on 091000 - was set when URLs loaded earlier
					} else {
						say_file_name_error(IDS_AND_BEGIN_SENTENCE,short_name,1); 
						// new on 061199 - replaced importance with 1 on 091000 to minimize bothering the user
					};
					tt_warning_count++;
					delete [] short_name;
				};
			};
		} else if (code == USER_SPRITE) { // new on 080403
			convert_to_synthetic_image();
		} else if (is_synthetic()) {
//			xml_element *element = create_xml_element(L"Picture",document); // could have "top level" tag for this...
			xml_element *synthetic = create_xml_element(L"Shape",document);
			xml_set_attribute(synthetic,L"Code",parameter/256); // could eliminate this for Description but would be costlier
	//        xml_set_attribute(synthetic,L"ColorIndex",parameter%256);
			// the following is better than an attribute since editable by ordinary users
			xml_append_child(create_xml_element(L"ColorIndex",document,parameter%256),element);
			if (tt_include_descriptions_in_xml) { 
				// tt_dump_reason == DUMPING_TO_CLIPBOARD) { // no need to waste time and space otherwise -- conditional new on 261102
				string description = S(IDS_SYNTHETIC_RECTANGLE+(parameter/256));
				// not clear this is worth bothering with and if so every L"Description" should do it 
				//// need to change "a rectangle" into "rectangle" in a language independent way
				//string space_and_after = strchr(description,' ');
				//if (space_and_after != NULL) {
				//   description = space_and_after+1;
				//};
				xml_set_attribute(synthetic,L"Description",description); // not used but should be helpful
			};
			xml_append_child(synthetic,element);
		} else {
//			xml_element *element = create_xml_element(L"Picture",document); // could have "top level" tag for this...
			xml_element *built_in = create_xml_element(L"BuiltIn",document);
			xml_set_attribute(built_in,L"Code",code); // could eliminate this for Description but would be costlier
			xml_set_attribute(built_in,L"PictureNumber",parameter);
			if (tt_include_descriptions_in_xml) { // tt_dump_reason == DUMPING_TO_CLIPBOARD) { 
				// no need to waste time and space otherwise -- conditional new on 261102
				xml_set_attribute(built_in,L"Description",S(IDS_TOOLBOX_FROM_SIDE+code)); // not used but should be helpful
			};
			xml_append_child(built_in,element);
		};
	};
	// and now for what was here prior to 120703
   boolean flipped_originally = is_flipped();
   boolean temporarily_reset_size = ok_to_temporarily_reset_size() && !flipped_originally; 
	// add flipped condition on 060201 since no point if flipped
	city_coordinate width_saved = width;
	city_coordinate height_saved = height;
	const int64 new_dimension = minimum_dimension; // for accuracy increase the greater dimension to some large number 
	if (temporarily_reset_size && height > width && height < new_dimension) {
		city_coordinate new_width = (city_coordinate) ((width*new_dimension)/height); 
		// should compute this was 64 bits and then should always fit
		set_size(new_width,new_dimension,TEMPORARY_FOR_DUMPING);
	} else if (temporarily_reset_size && width > height && width < new_dimension) {
		city_coordinate new_height = (city_coordinate) ((height*new_dimension)/width); 
		set_size(new_dimension,new_height,TEMPORARY_FOR_DUMPING);
	} else {
		temporarily_reset_size = FALSE; // didn't need to since was big already
	};
   if (controlee != NULL) { // could optimize this when the controlee is "obvious"
      xml_set_attribute(element,L"Code",APPEARANCE_REMOTE);
		if (tt_include_descriptions_in_xml) { 
			// tt_dump_reason == DUMPING_TO_CLIPBOARD) { // no need to waste time and space otherwise -- conditional new on 261102
			xml_set_attribute(element,L"Description",S(IDS_X_LONG_LABEL+APPEARANCE_REMOTE));
		};
//      xml_set_attribute(element,L"Type",L"Looks");
      // since remote looks is just a picture with a controlee there needs to be one to distinguish it from ordinary pictures
      // so always dump controlee even if it is "silly"
      //xml_element *controlee_element = create_xml_element(L"For",document);
      //xml_append_child(controlee_element,element);
      //controlee->xml_create_and_append_element(controlee_element,document); // no need for geometry to be included here
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	controlee->check_for_independence();
		//};
//		if (being_dumped_together(controlee,this)) { // condition new on 150703
		// conditional and branch commented out on 210803 since might be together but linked through controlee links
		// also second branch forgot the "For" part
		//	controlee->add_xml(element,L"For",document); // simplified on 201202
		//} else {
		//	controlee->xml_create_external_link_and_append_element(element,document); // new on 150703
		//};
		// rewrote the above as below on 220803
		xml_element *controlee_element = create_xml_element(L"For",document); // new on 220803 so used in both branches below
		if (being_dumped_together(controlee,this)) {
			// moved here on 261102 from no file_name branch
			// needs to be before Page elements since they might need to know their controlee
			controlee->xml_create_and_append_element(controlee_element,document,TRUE)->Release(); 
		} else {
			controlee->xml_create_external_link_and_append_element(controlee_element,document);
		};
		xml_append_child(controlee_element,element); // moved here on 190804
//      if (is_flipped() != controlee->is_flipped()) { // prior to 141002 was (is_flipped() && !in_notebook())
      if (is_flipped()) { // && !in_notebook()) { // restored 141002
         //  && !in_notebook() to fix a problem with remote looks in notebooks saved flipped
         xml_append_child(create_xml_element(L"FlippedOver",document),element);
      } else if (controlee != NULL) {
         xml_append_child(create_xml_element(L"NotFlippedOver",document),element); 
			// new on 141002 to deal with flippedness of sensor distinct from controlee
      };
      Sprite::xml(element,document);
      return; // I'm done
	};
   if (indirection != NULL) {
//      indirection->xml_create_and_append_element(element,document); // no need for geometry to be included here
		indirection->add_xml(element,L"Appearance",document); 
		// rewritten on 201202 since when loading need to get this first to make indirection and then load the rest
      // and do the usual picture stuff after this
	};
   if (show_some_if_subpicture()) {
      xml_append_child(create_xml_element(L"PartlyVisibleIfPartOfPicture",document),element);
   };
   Sprite::xml(element,document); // moved here on 150802 - needs to be earlier than ON_FRONT stuff for geometry to work OK
	if (!tt_dumping_to_test_equality_or_describe) {
		if (is_flipped() && (width <= 0 || height <= 0)) { 
			set_size(width_before_flip,height_before_flip);
		};
      if (x_speed != NULL && x_speed->sign() != 0) {
         xml_append_child(create_xml_element(L"SpeedToRight",document,x_speed),element);
      };
      if (y_speed != NULL && y_speed->sign() != 0) {
         xml_append_child(create_xml_element(L"SpeedToTop",document,y_speed),element);
      };
	};
	city_coordinate saved_width_before_flip, saved_height_before_flip;
	boolean change_size_when_flipping = (flipped_originally && (leader==NULL || leader->kind_of() != PICTURE)); 
	// added flipped_originally on 111200
	if (flipped_originally) {
		saved_width_before_flip = width_before_flip; // new on 081200
		saved_height_before_flip = height_before_flip;
		temporarily_reset_size = !inside_vacuum(); 
		// was TRUE prior to 300604 // new on 310803 since the following may change the size if saved while flipping
		unflip(TRUE,change_size_when_flipping); 
	};
	if (followers != NULL) {
      // think about whether each element needs to be tagged or just the list - 051102
      followers->add_xml(element,document,L"OnFront",FALSE,TRUE);
  //    xml_element *followers_element = create_xml_element(L"Parts",document);
  //    xml_append_child(followers_element,element);
		//followers->xml_create_and_append_element(followers_element,document);
   };
   if (flipped_originally) { // do this here so that the followers on the front don't end up on the back
      xml_append_child(create_xml_element(L"FlippedOver",document),element);
   };
	flip(TRUE,change_size_when_flipping); 
	// was (leader==NULL) prior to 190299 to ensure proper geometry - restored on 081200 and added leader type check
	if (flipped_originally) { // new on 071200
		set_temporarily_flipped(FALSE);
		set_width_before_flip(saved_width_before_flip); // new on 081200
		set_height_before_flip(saved_height_before_flip);
	};
	city_coordinate saved_width = 0, saved_height = 0;
//	Sprites *sorted_flipped_followers = sort_flipped_followers();
   // re-written on 141002
   Sprites *sorted_flipped_followers = flipped_followers();
   if (sorted_flipped_followers != NULL) {
      // think about whether each element needs to be tagged or just the list - 051102
	   // TT_DEBUG_ON prior to 181203
		if (tt_remove_playground_active_documentation) {
			Sprites *remaining = sorted_flipped_followers;
			Sprites *new_sorted_flipped_followers = NULL;
			boolean keep = TRUE;
			while (remaining != NULL) {
				Sprite *sprite = remaining->first();
				if (sprite->kind_of() == PICTURE) {
					Sprites *pictures_followers = sprite->pointer_to_followers();
					Sprites *remaining2 = pictures_followers;
					Text *text = NULL;
					const int max_text_string = 64;
					char alpha_numeric_text_string[max_text_string+1]; // +1 for terminator
					while (remaining2 != NULL) {
						Sprite *sprite = remaining2->first();
						if (sprite->kind_of() == TEXT) {
							text = (Text *) sprite;
						} else if (sprite->looks_like_is_of_type(TEXT)) { // indirects to a text pad
							text = (Text *) ((Picture *) sprite)->pointer_to_indirection();
						};
						if (text != NULL) {
							string text_string = text->return_narrow_text();
							if (text_string != NULL) {
								copy_alphanumerics(text_string,alpha_numeric_text_string,max_text_string);
								const int string_count = 5;
								static string bad_strings[string_count] = {"IremoveaspaceifImselectedandyoutypespace",
																						"Ichangetexttospeech",
																						"Speechmechanism",
																						"Ispeakthetextwhenyoupointtomeandpressthespacebar",
																						"IremoveaspaceifIyoupointtomeandtypeaspace"};
								for (int i = 0; i < string_count; i++) {		
									if (stricmp(bad_strings[i],alpha_numeric_text_string) == 0) { // found one
										static int removal_count = 0;
										removal_count++;
										tt_error_file() << removal_count << " " << text_string << endl;
										keep = FALSE;
										break;
									};
								};
							};
						};
						if (!keep) break; // found one to remove
						remaining2 = remaining2->rest();
					};
				};
				if (keep) {
					insert_at_end(sprite,new_sorted_flipped_followers);
				};
				remaining = remaining->rest();
			};
//			just_destroy_list(sorted_flipped_followers);
			sorted_flipped_followers = new_sorted_flipped_followers;
		};
		if (sorted_flipped_followers != NULL) // new on 281103
      sorted_flipped_followers->add_xml(element,document,L"OnBack");
//      sorted_flipped_followers = sorted_flipped_followers->reverse();
//   };
//	if (sorted_flipped_followers != NULL) { 
  //    xml_element *followers = create_xml_element(L"OnBack",document);
  //    xml_append_child(followers,element);
		//sorted_flipped_followers->xml_create_and_append_element(followers,document);		
//      sorted_flipped_followers->remove_all();
//      delete sorted_flipped_followers;
   };
	if (!flipped_originally) {
		unflip(TRUE,FALSE); // was (leader==NULL) prior to 190299 - this restores original state
	} else if (saved_width != 0) { // restore flipped picture's size
		set_size(saved_width,saved_height);
	}; // following was on else branch prior to 051100 but need to do it even if !flipped_originally 
	if (saved_width == 0 && temporarily_reset_size) { // new on 171000
		set_size(width_saved,height_saved,TEMPORARY_FOR_DUMPING);
	};
   // following moved here on 150802 since it is interpreted on load depending 
	// upon if it is flipped (saved_width vs. width_before_flip)
   // commented out conditional on 051102 since broke notebook pages dumping in XML and 
	// if they are different then why not save them?
   //if (!tt_dumping_to_test_equality_or_describe && // !is_controlee && 
	// !is_controlee added on 140699 to make saving of remote looks more reliable
		 //((leader != NULL && (leader->kind_of() != PICTURE || leader->is_flipped())) || is_flipped())) {
		// is_flipped() added on 180299 so geometry of flipped pictures saved with flipped pictures looks OK
		// following is default if blank
      // why use saved_width rather than "real" thing? -- because might be flipped over
      // removed leader == NULL from condition since if already free and unflipped then why deal with this?
	if (ok_to_save_geometry_when_free_and_unflipped()) { // new on 170703
      xml_element *saved_width_element = NULL;
      xml_element *saved_height_element = NULL;
		if (is_flipped()) {
         if (width_before_flip != width) {
			   saved_width_element = create_xml_element(L"Width",document,width_before_flip);
         };
			if (height_before_flip != height) {
            saved_height_element = create_xml_element(L"Height",document,height_before_flip);
         };
		} else {
			// re-written on 101004 so that rather than dump with negative values - figures them out now
			// otherwise if a notebook page is selected (pointed to) then can get the wrong saved width and height
			city_coordinate saved_width_for_dumping = return_saved_width(); 
			city_coordinate saved_height_for_dumping = return_saved_height();
			if (saved_width_for_dumping <= 0 || saved_height_for_dumping <= 0) {
				good_size(saved_width_for_dumping,saved_height_for_dumping);
			};
			if (saved_width_for_dumping != width || code == USER_SPRITE) {
				// second disjunct added on 051006 so that when user pictures are saved they remember their current saved size
				// otherwise they will expand to original good size when taken from a stack
			   saved_width_element = create_xml_element(L"Width",document,saved_width_for_dumping);
         };
         if (saved_height_for_dumping != height || code == USER_SPRITE) {
				// second disjunct added on 051006 so that when user pictures are saved they remember their current saved size
				// otherwise they will expand to original good size when taken from a stack
            saved_height_element = create_xml_element(L"Height",document,saved_height_for_dumping);
         };
		};
      if (saved_width_element != NULL || saved_height_element != NULL) {
         xml_element *geometry_element = create_xml_element(L"GeometryWhenFreeAndUnflipped",document);
         if (saved_width_element != NULL) {
            xml_append_child(saved_width_element,geometry_element);
         };
         if (saved_height_element != NULL) {
            xml_append_child(saved_height_element,geometry_element);
         };
         xml_append_child(geometry_element,element);
      };
	};
	if (tt_dump_reason != DUMPING_TO_CLIPBOARD && tt_dump_reason != DUMPING_TO_NOTEBOOK_PAGE) { 
		// conditions copied from Sprite::xml
		// following moved here on 171004 since the remote control notebook needs to have everything else 
		// about the picture already processed
		// e.g. whether it is flipped for the remote looks
		// was in Picture::add_special_xml before 171004
		if (remote_notebook != NULL) { // && remote_notebook->pointer_to_background() != NULL) {
			// need to wrap it in "RemoteNotebook" since too hard to deal with multiple links if shared and so are other elements
			// added second condition on 180103 since why save this if not available
			// removed second condition on 170405 since if inside Dusty then 
			// when picture is deleted it doesn't tell remote_notebook to forget the picture
			//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
			//	remote_notebook->check_for_independence();
			//};
			if (tt_dump_reason != DUMPING_TO_ENABLE_UNBAMMING || remote_notebook->pointer_to_background() != NULL) {
				// condition new on 161105 since dumping the notebook when it isn't visible for unbamming is wasteful
				// and can cause anomalies (e.g. Leonel's email of 081605)
			   remote_notebook->add_xml(element,L"RemoteNotebook",document);
			};
		};
	};
};

xml_element *Picture::xml_set_special_attributes(xml_element *element, xml_document *document) {
	// new on 261102 -- went through all the state variables
	xml_element *special_element = Sprite::xml_set_special_attributes(element,document);
	if (suspended_flag) {
		xml_set_attribute(special_element,L"Suspended",1);
	};
	if (!stopped_flag) {
		xml_set_attribute(special_element,L"NotStopped",1);
	};
	if (old_cycle_stopped) { // needed for END_OF_ANIMATION_REMOTE
		xml_set_attribute(special_element,L"OldCycleStopped",1);
	};
	if (old_grasped) { // needed for GRASPED_REMOTE
		xml_set_attribute(special_element,L"OldGrasped",1);
	};
	if (old_selected_flag) { // needed for SELECTED_REMOTE
		xml_set_attribute(special_element,L"OldSelected",1);
	};
	if (released_last_frame) { // needed for RELEASED_REMOTE
		xml_set_attribute(special_element,L"ReleasedLastFrame",1);
	};
	if (released_flag) { // needed for RELEASED_REMOTE
		xml_set_attribute(special_element,L"Released",1);
	};
	if (flip_going_on_flag) {
		xml_set_attribute(special_element,L"FlipGoingOn",1);
	};
	if (placed_new_items_flag) {
		xml_set_attribute(special_element,L"PlacedNewItems",1);
	};
	if (old_parameter >= 0) { // needed for PARAMETER_REMOTE and scales
		xml_set_attribute(special_element,L"OldParameter",old_parameter);
	};
	xml_set_attribute(special_element,L"OldLLX",old_llx);
   xml_set_attribute(special_element,L"OldLLY",old_lly);
	if (old_width != min_long) {
		xml_set_attribute(special_element,L"OldWidth",old_width);
		xml_set_attribute(special_element,L"OldHeight",old_height);
	};
	if (width_before_flip != min_long) {
		xml_set_attribute(special_element,L"WidthBeforeFlip",width_before_flip);
		xml_set_attribute(special_element,L"HeightBeforeFlip",height_before_flip);
	};
	if (llx_accumulated_error != 0) {
		xml_set_attribute(special_element,L"LLXAccumlatedError",llx_accumulated_error);
	};
	if (lly_accumulated_error != 0) {
		xml_set_attribute(special_element,L"LLYAccumlatedError",lly_accumulated_error);
	};
	return(special_element);
	// temporarily_flipped, temporarily_unflipped - should be reset and not last between frames
	// subpicture_flag, robots_running_on_back_flag, show_some_if_subpicture_flag, remote, 
	//backside_background, and remote_looks_of_me_count known by relationships
};

void Picture::add_special_xml(xml_element *element, xml_document *document) {
	if (saved_indirection != NULL) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	saved_indirection->check_for_independence();
		//};
		saved_indirection->add_xml(element,L"SavedPictureOf",document);
	};
	//if (item_to_add != NULL && tt_dump_reason != DUMPING_TO_ENABLE_UNBAMMING) { 
	// added DUMPING_TO_ENABLE_UNBAMMING since item_to_add is the reason for this
	//	item_to_add->add_xml(element,L"ItemToAdd",document);
	//};
	if (subpictures_with_notebooks != NULL) {
		subpictures_with_notebooks->add_xml(element,document,L"SubPicturesWithNotebooks");
	};
	Sprite::add_special_xml(element,document);
};

boolean Picture::xml_get_special_attributes(xml_node *node) {
	suspended_flag = xml_get_attribute_int(node,L"Suspended",0); // prior to 210404 was suspended_flag
	stopped_flag = !xml_get_attribute_int(node,L"NotStopped",0); // prior to 210404 was !stopped_flag
	old_cycle_stopped = xml_get_attribute_int(node,L"OldCycleStopped",0); // prior to 210404 was old_cycle_stopped
	old_grasped = xml_get_attribute_int(node,L"OldGrasped",0); // prior to 210404 was old_grasped
	old_selected_flag = xml_get_attribute_int(node,L"OldSelected",0); // prior to 210404 was old_selected_flag
	released_last_frame = xml_get_attribute_int(node,L"ReleasedLastFrame",0); // prior to 210404 was released_last_frame
	released_flag = xml_get_attribute_int(node,L"Released",0); // prior to 210404 was released_flag
	flip_going_on_flag = xml_get_attribute_int(node,L"FlipGoingOn",0); // prior to 210404 was flip_going_on_flag
	placed_new_items_flag = xml_get_attribute_int(node,L"PlacedNewItems",0); // prior to 210404 was placed_new_items_flag
	old_parameter = xml_get_attribute_int(node,L"OldParameter",0); // prior to 210404 was old_parameter
	old_llx = xml_get_attribute_int(node,L"OldLLX",old_llx);
	old_lly = xml_get_attribute_int(node,L"OldLLY",old_lly);
	old_width = xml_get_attribute_int(node,L"OldWidth",min_long); // prior to 210404 was old_width
	old_height = xml_get_attribute_int(node,L"OldHeight",min_long); // prior to 210404 was old_height
	width_before_flip = xml_get_attribute_int(node,L"WidthBeforeFlip",min_long); // prior to 210404 was width_before_flip
	height_before_flip = xml_get_attribute_int(node,L"HeightBeforeFlip",min_long); // prior to 210404 was height_before_flip
	llx_accumulated_error = xml_get_attribute_double(node,L"LLXAccumlatedError",0.0); // prior to 210404 was llx_accumulated_error
	lly_accumulated_error = xml_get_attribute_double(node,L"LLYAccumlatedError",0.0); // prior to 210404 was lly_accumulated_error
	return(Sprite::xml_get_special_attributes(node));
};

void picture_handle_geometry_when_free_and_unflipped_tag(Entity *picture, Tag tag, xml_node *node, void *extra_info) {
   ((Picture *) picture)->handle_geometry_when_free_and_unflipped_tag(tag,node);
};

boolean Picture::handle_geometry_when_free_and_unflipped_tag(Tag tag, xml_node *node) {
   switch (tag) {
      case WIDTH_TAG:
         if (is_flipped()) {
            set_width_before_flip(xml_get_int(node));
         } else {
            set_saved_width(xml_get_int(node));
         };
         return(TRUE);
      case HEIGHT_TAG:
         if (is_flipped()) {
            set_height_before_flip(xml_get_int(node));
         } else {
            set_saved_height(xml_get_int(node));
         };
         return(TRUE);
      case NO_MORE_TAGS:
         return(TRUE);
   };
   return(FALSE); // warn?? - save??
};

//Sprite *previous_flipped_follower = NULL; // commented out on 141002

boolean Picture::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
		case PICTURE_OF_TAG: {
			Sprite *sprite = xml_get_sprite(node); 
			if (sprite != NULL) { // conditional new on 250703 for robustness (check also if appropriate??)
				set_indirection(sprite,FALSE); // worth error checking??
#if TT_DEBUG_ON
			} else {
				tt_error_file() << "Didn't load a sprite for PictureOf tag" << endl;
#endif
			};
									};
         return(TRUE);
      case SCALE_TAG:
         set_appearance_code(SCALE_SPRITE);
         switch (xml_get_text_first_character(node)) { // unique first letter
            case 'L': // LeftLeaning
               set_parameter(GREATER_THAN_SCALE);
               return(TRUE);
            case 'R': // RightLeaning
               set_parameter(LESS_THAN_SCALE);
               return(TRUE);
            case 'B': // Balanced
               set_parameter(EQUAL_SCALE);
               return(TRUE);
            case 'M': // Moving
               set_parameter(VASCILATE_SCALE);
					add_to_active_sprites(this); // new on 060403
               return(TRUE);
            default: // - warn?? - save??
               return(TRUE);
         };
      case SHAPE_TAG:
         set_parameter(xml_get_attribute_int(node,L"Code")*256+current_parameter());
         return(TRUE);
      case BUILT_IN_TAG:
         set_appearance_code((SpriteCode) xml_get_attribute_int(node,L"Code"));
			if (code == USER_SPRITE) {
				log("A picture was saved incorrectly (probably due to a bug in versions 2.83 to 2.86). Sorry, but ToonTalk no longer knows what image file it had. Replaced by a black rectangle.",FALSE,TRUE);
//				xml_debug_node(node);
				set_appearance_code(SYNTHETIC_SPRITE);
			} else {
				set_parameter(xml_get_attribute_int(node,L"PictureNumber"));
				set_size_is_true(FALSE); // since that seems to be the default for stuff that might animate
//          update_display(); // probably needed so geometry of followers is right -- 
				// moved on 071002 to NO_MORE_TAGS since doesn't have geometry yet
			};
			return(TRUE);
      case COLOR_INDEX_TAG:
         set_parameter(xml_get_int(node)+current_parameter());
         return(TRUE);
      case ON_FRONT_TAG:
			// if "reloading" a picture shouldn't followers list be reset too? -- 
			// yes caller should call remove_all_followers and release_all_rerferences
         add_unflipped_follower(xml_get_sprite(node),LOADING_FOLLOWER);
         return(TRUE);
      case ON_BACK_TAG: {
            Sprite *flipped_follower = xml_get_sprite(node);
            add_flipped_follower(flipped_follower,NULL,this,LOADING_FOLLOWER); 
				// NULL was previous_flipped_follower prior to 141002
//            previous_flipped_follower = flipped_follower; 
            return(TRUE);
								};
      case FLIPPED_OVER_TAG:
         flip(FALSE,FALSE); // args new on 041103 -- needed otherwise size can go wrong
         return(TRUE);
      case NOT_FLIPPED_OVER_TAG: // new on 141002
         unflip();
         return(TRUE);
      case PARTLY_VISIBLE_IF_PART_OF_PICTURE_TAG:
         set_show_some_if_subpicture(TRUE);
         return(TRUE);
      case GEOMETRY_WHEN_FREE_AND_UNFLIPPED_TAG:
         return(xml_process_children(node,picture_handle_geometry_when_free_and_unflipped_tag,this));
      case SPEED_TO_RIGHT_TAG:
        set_x_speed(xml_get_number_value(node)); 
		  // node was first_node_that_is_an_element(node) prior to 050803 but change was made on on 120703
        return(TRUE);
      case SPEED_TO_TOP_TAG:
		  set_y_speed(xml_get_number_value(node)); 
		  // node was first_node_that_is_an_element(node) prior to 050803 but change was made on on 120703
        return(TRUE);
      case FOR_TAG:  // remote looks
			set_controlee((Picture *) xml_get_sprite(node,controlee,PICTURE));
         //Sprite *controlee = xml_get_sprite(node);
         //if (controlee != NULL) {
         //   if (controlee->kind_of() == PICTURE) {
         //      set_controlee((Picture *) controlee);
         //   } else {
         //      controlee->destroy(); // warn??
         //   };
         //};
         return(TRUE);
		case SAVED_PICTURE_OF_TAG:
			saved_indirection = xml_get_sprite(node,saved_indirection);
			return(TRUE);
		case ITEM_TO_ADD_TAG: // made obsolete on 130103
//			item_to_add = xml_get_sprite(node,item_to_add);
			return(TRUE);
		case REMOTE_NOTEBOOK_TAG:
			remote_notebook = (Notebook *) xml_get_sprite(node,remote_notebook,PROGRAM_PAD); 
			add_pages_to_remote_notebook(); // new on 180903
			return(TRUE);
		case SUB_PICTURES_WITH_NOTEBOOKS_TAG:
			destroy_sprites(subpictures_with_notebooks); // new on 070103
			subpictures_with_notebooks = xml_load_sprites(node);
		case MISSING_PICTURE_FILE_TAG: // new on 120703
#if TT_DEBUG_ON
			debug_this();
#endif
			xml_load_sprite(node,tag_token(node),this,PICTURE);
			return(TRUE);
		case PICTURE_TAG:
			// new on 031003 to deal with old XML that does this sometimes
			xml_load_sprite(node,tag,this,PICTURE);
			return(TRUE);
      case NO_MORE_TAGS:
//         update_display(); // probably needed so geometry of followers is right -- moved here on 071002 -- experiment on 201102
         if (saved_width < 0) { // new on 051102 if size not explictly saved
            save_size();
         };
			if (code != SYNTHETIC_SPRITE && code != USER_SPRITE) { // new on 031202
				Sprite::update_display(0,0,UPDATE_INITIALIZE); // complete the initialization -- in particular set current_image
//				clear_changes(); // not clear it is worthwhile 
			};
			really_propagate_changes(); 
			// new on 210104 since if its parts are referenced first (e.g. via sensors and running robots) then this never happened
         // and do the following (new on 121102)
      default:
         return(Sprite::handle_xml_tag(tag,node));
   };
};

boolean Picture::geometry_worth_saving() {
	if (appearance_code() == SCALE_SPRITE) { // new on 290103
		return(Sprite::geometry_worth_saving());
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 240603) {
		if (!(!is_flipped() || leader == NULL)) {
			log("geometry not worth saving?");
		} else {
			log("geometry worth saving.");
		};
		describe_briefly(tt_error_file());
		tt_error_file() << endl << (int) is_flipped() << " is the value of is_flipped()" << endl;
	};
#endif
	if (leader != NULL && (leader->kind_of() == CUBBY || leader->kind_of() == PROGRAM_PAD)) { 
		// new on 021003 since in some circumstances there is no need to waste time and space with geometry when in a box
		// added leader being a notebook test on 160304 since it is both wasteful and wrong if a SpritePage or XMLPage since shared
		if (dumping_an_individual() || !showing_on_a_screen() || smaller_than_a_pixel()) {
			return(FALSE);
		};
	};
	return(!is_flipped() || leader == NULL); 
	// && (leader == NULL || leader->kind_of() != CUBBY)) || Sprite::geometry_worth_saving());
	// updated on 180103 since no need to save geometry if inside a cubby
	// updated again on 270103 since there is a need since sensors (e.g. width and height) will be wrong until too late
	// added leader == NULL on 010503 since if on floor then should save geometry even if flipped
};

#endif

void Picture::just_apply_delta(city_coordinate delta_x, city_coordinate delta_y) { // new on 051102
   if (indirection != NULL) {
      indirection->just_apply_delta(delta_x,delta_y);
   };
   Sprite::just_apply_delta(delta_x,delta_y); // I suppose it is best to apply it here as well
};

/* -- commented out on 250299
// following is obsolete but kept until time to generate a new file format
void Picture::dump_follower(Sprite *follower, output_stream &stream) {
	follower->dump(stream);
	// dump some geometry for the backside stuff
//	city_coordinate follower_llx,follower_lly;
//	follower->lower_left_corner(follower_llx,follower_lly);
	short int user_value = 0;
//   if (tt_log_version_number <= 7) {
//	   user_value = (short int) (follower_llx-llx); //)*1000)/backside_width;
//   } else {
//      user_value = (short int) follower->priority();   // new use of slot (6/30/97)
//   };
	if (!tt_dumping_to_test_equality_or_describe) {
		stream.write((unsigned char *) &user_value, sizeof(user_value));
	//	if (tt_log_version_number <= 7) user_value = (short int) (follower_lly-lly); //*1000)/backside_height;
		stream.write((unsigned char *) &user_value, sizeof(user_value));
	//	if (tt_log_version_number <= 7) user_value = (short int) follower->current_width();//*1000)/backside_width;
		stream.write((unsigned char *) &user_value, sizeof(user_value));
	//	if (tt_log_version_number <= 7) user_value = (short int) follower->current_height();//*1000)/backside_height;
		stream.write((unsigned char *) &user_value, sizeof(user_value));
	};
};
*/

Sprite *Picture::first_follower_of_type(SpriteType type) { // new on 101100
	if (controlee != NULL) {
//		if (!postponed_copy_flipped_followers_of_controlee_flag) { // check first the remote looks itself (new on 151200)
//			Sprite *result = Sprite::first_follower_of_type(type);
//			if (result != NULL) return(result);
//		};
		Sprites *followers = controlee->flipped_followers(); // flip side is only time this is used, right??
		if (followers == NULL) return(NULL);
		return(followers->last_element_of_type(type)); // major major savings if not needed for other purposes
	};
	return(Sprite::first_follower_of_type(type));
};

boolean Picture::looks_like_is_of_type(SpriteType type) { // new on 210902
	// check if Java stuff needs to be updated as well??
//	if (Sprite::looks_like_is_of_type(type)) return(TRUE);
	// else check if I'm an indirection to something of type
	if (indirection != NULL) {
		return(indirection->looks_like_is_of_type(type));
	};
	if (controlee != NULL) { // new on 130504
		return(controlee->looks_like_is_of_type(type));
	};
	return(Sprite::looks_like_is_of_type(type)); 
	// moved here on 221102 since if indirection is text then it doesn't look like PICTURE
}; 

void Picture::set_width_before_flip(city_coordinate new_width) { // moved here from picture.h on 040603
	width_before_flip = new_width;
	if (tt_programmer != NULL) { // condition new on 120504
		programmer_state_before_flip = tt_programmer->kind_of();
	};
};

void Picture::set_height_before_flip(city_coordinate new_height) {
	height_before_flip = new_height;
	if (tt_programmer != NULL) { // condition new on 120504
		programmer_state_before_flip = tt_programmer->kind_of();
	};
};

city_coordinate Picture::return_width_before_flip() { // new on 161200 // moved here from picture.h on 040603
	// while this is written for the general case I think the only case 
	// that will occur in practice is when they are both on the floor
	// or it was on the floor and now it is in the room
	if (tt_programmer == NULL) return(width_before_flip); 
	// new on 120504 for robustness since while loading an initial city this can happen
	ProgrammerState current_state = tt_programmer->kind_of();
	if (programmer_state_before_flip == current_state) {
		return(width_before_flip);
	};
	// following is new as of 040603
	if (programmer_state_before_flip == PROGRAMMER_AT_FLOOR && 
		 (current_state == PROGRAMMER_CITY_WALKING || current_state == PROGRAMMER_ROOM_WALKING)) {
		return(tt_programmer->translate_floor_to_room_width(width_before_flip));
	} else if (current_state == PROGRAMMER_AT_FLOOR && 
				  (programmer_state_before_flip == PROGRAMMER_CITY_WALKING || 
				   programmer_state_before_flip == PROGRAMMER_ROOM_WALKING)) {
		return(tt_programmer->translate_room_to_floor_width(width_before_flip));
	};
	return(width_before_flip);
};

city_coordinate Picture::return_height_before_flip() { // new on 161200
	if (tt_programmer == NULL) return(height_before_flip); 
	// new on 120504 for robustness since while loading an initial city this can happen
	ProgrammerState current_state = tt_programmer->kind_of();
	if (programmer_state_before_flip == current_state) {
		return(height_before_flip);
	};
	// following is new as of 040603
	if (programmer_state_before_flip == PROGRAMMER_AT_FLOOR && 
		 (current_state == PROGRAMMER_CITY_WALKING || current_state == PROGRAMMER_ROOM_WALKING)) {
		return(tt_programmer->translate_floor_to_room_height(height_before_flip));
	} else if (current_state == PROGRAMMER_AT_FLOOR && 
				  (programmer_state_before_flip == PROGRAMMER_CITY_WALKING || 
				   programmer_state_before_flip == PROGRAMMER_ROOM_WALKING)) {
		return(tt_programmer->translate_room_to_floor_height(height_before_flip));
	};
	return(height_before_flip);
};
	
Sprites *Picture::sort_flipped_followers() {
	if (controlee != NULL) return(NULL); // new on 171200 - remote looks doesn't really have anything on back
	return(sort_items(flipped_followers()));
};

Sprites *Picture::flipped_followers() { // arg is obsolete
//	if (controlees_flipped_followers_too) { // new on 091100
//		copy_flipped_followers_of_controlee_now();
//	};
//	if (controlee != NULL && followers == NULL) { // new on 151200 (haven't copied followers yet)
////		if (controlees_flipped_followers_too) {
//			return(controlee->flipped_followers());
////		} else {
////			return(NULL);
////		};
//	};
	if (is_flipped()) {
		return(followers);
	};
	return(followers_on_other_side);
};

boolean Picture::no_flipped_followers() { // new on 091100
	if (controlee != NULL) { // new on 151200
		return(controlee->no_flipped_followers()); 
	};
	if (is_flipped()) {
//		if (followers == NULL && controlee != NULL) { 
		// && postponed_copy_flipped_followers_of_controlee_flag ) { -- commented out on 151200
//			return(controlee->no_flipped_followers());
//		};
		return(followers == NULL);
	};
//	if (followers_on_other_side == NULL && controlee != NULL) { // && postponed_copy_flipped_followers_of_controlee_flag) { 
	// new on 091100
//		return(controlee->no_flipped_followers());
//	};
	return(followers_on_other_side == NULL);
};

void Picture::just_set_flipped_followers(Sprites *replacement) {
	if (is_flipped()) {
		followers = replacement;
	} else {
		followers_on_other_side = replacement;
	};
};

//void Picture::set_flipped_followers(Sprites *replacement) {
//	if (is_flipped()) {
//		if (followers != NULL) {
//			followers->remove_all();
//			delete followers;
//		};
//		followers = replacement;
//	} else {
//		if (followers_on_other_side != NULL) {
//			followers_on_other_side->remove_all();
//			delete followers_on_other_side;
//		};
//		followers_on_other_side = replacement;
//	};
//};

void Picture::describe(output_stream &message, Article article) {
//	describe_comment(message);
//	if (saved_action_status != NULL) { // commented out on 171102 - could saved_by be used instead???
////		print_article(article,message,S(IDS_FLIPPING_MOVIE));
//      message << S(IDS_FLIPPING_MOVIE);
//	} else 
   if (is_flipped()) {
		message << S(IDS_FLIP_SIDE_OF);
		unflip(TRUE,FALSE); 
		// second arg new on 210205 since this shouldn't change size -- e.g. calling F1 for Marty on a flipped picture
		Sprite::describe(message,article);
		flip(TRUE,FALSE); 
		// second arg new on 210205 since this shouldn't change size -- e.g. calling F1 for Marty on a flipped picture
		message << S(IDS_FLIP_SIDE_OF2,"");
//	} else if (indirection != NULL) {
//		indirection->describe(message,article);
//	} else if (infinite_stack() && english()) {
//		print_article(article,message,S(IDS_STACK_OF));
//		describe_briefly(message,PLURAL_ARTICLE);
   } else if (code == SCALE_SPRITE) {
      describe_briefly(message,article);
	} else {
		Sprite::describe(message,article); // was describe_briefly
	};
};

void Picture::describe_briefly(output_stream &message, Article article, boolean capitalize_first) {
	if (indirection != NULL) {
		if (indirection->kind_of() == PICTURE) { // new on 300500
			indirection->describe_briefly(message,article,capitalize_first);
		} else {
			indirection->describe_type(message,article,TRUE,capitalize_first);
		};
		return;
	};
	if (kind_of() == BLANK_PICTURE) {
		if (contained_in(THOUGHT_BUBBLE)) {
			message << S_maybe_capitalize_first(IDS_ANY_PICTURE,capitalize_first);
		} else {
			describe_type(message,article,TRUE,capitalize_first);
		};
		return; // don't mention followers
	};
	if (code == SCALE_SPRITE) {
		switch (current_parameter()) {
			case GREATER_THAN_SCALE:
			case GREATER_THAN_FROM_EQUAL_SCALE:
//				print_article(article,message,S(IDS_SCALE_TO_LEFT));
            message << S_maybe_capitalize_first(IDS_SCALE_TO_LEFT,capitalize_first);
				return;
			case LESS_THAN_SCALE:
			case LESS_THAN_FROM_EQUAL_SCALE:
//				print_article(article,message,S(IDS_SCALE_TO_RIGHT));
            message << S_maybe_capitalize_first(IDS_SCALE_TO_RIGHT,capitalize_first);
				return;
			case EQUAL_SCALE:
			case EQUAL_FROM_GREATER_SCALE:
			case EQUAL_FROM_LESS_SCALE:
//				print_article(article,message,S(IDS_SCALE_BALANCED));
            message << S_maybe_capitalize_first(IDS_SCALE_BALANCED,capitalize_first);
				return;
			default:
//				print_article(article,message,S(IDS_SCALE_MOVING));
            message << S_maybe_capitalize_first(IDS_SCALE_MOVING,capitalize_first);
				return;
		};
// commented out since while "a picture of a rectangle"
// is a bit odd "a stack of a rectangle" and "here's all I know about a rectangle"
// are odder
//	} else if (code == SYNTHETIC_SPRITE) {
//		describe_appearance(code,message,article);
   } else if (code == USER_SPRITE) {
      message << S_maybe_capitalize_first(IDS_USER_SPRITE,capitalize_first);
		if (pointer_to_current_image() != NULL) { // new on 300500
			string user_picture_file_name = pointer_to_current_image()->return_file_name();
			message << " (" << user_picture_file_name << ") ";
		};
	} else {
		long next_id;
		if (still_picture(code)) {
//			print_article(article,message,S(IDS_PICT_OF));
         message << S_maybe_capitalize_first(IDS_PICT_OF,capitalize_first);
			next_id = IDS_PICT_OF2;
		} else {
//			print_article(article,message,S(IDS_MOVIE_OF));
         message << S_maybe_capitalize_first(IDS_MOVIE_OF,capitalize_first);
			next_id = IDS_MOVIE_OF2;
		};
      message << space();
		describe_appearance(code,message,INDEFINITE_ARTICLE);
		message << S(next_id,"");
	};
	int followers_count = 0;
	if (pointer_to_followers() != NULL) {
		followers_count = pointer_to_followers()->length();
	};
	if (followers_count > 1) { // translation problem??
		message << space() << S(IDS_WITH) << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << followers_count << S(IDS_PICT_DESCRIPTION1);
	} else if (followers_count == 1) {
		message << S(IDS_PICT_DESCRIPTION2);
	};
	int previous_followers_count = followers_count;
	followers_count = 0;
	if (followers_on_other_side != NULL) {
		followers_count = followers_on_other_side->length();
	};
//	if (!remote_notebook_added_flag) followers_count++;
	if (followers_count > 1) {
		if (previous_followers_count == 0) {
			message << space() << S(IDS_WITH) << space(SPACE_BEFORE_OR_AFTER_NUMBERS) //S(IDS_AND) << S(IDS_WITH)
					  << followers_count << space(SPACE_BEFORE_OR_AFTER_NUMBERS)
					  << S(IDS_THINGS) << space() << S(IDS_ON_THE_BACK);
		} else {
			// added end_sentence() new on 150500
			message << end_sentence() << S(IDS_AND_BEGIN_SENTENCE) << space() << S(IDS_ON_THE_BACK) << space() 
					  << S(IDS_ARE) << space(SPACE_BEFORE_OR_AFTER_NUMBERS)
					  << followers_count << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_THINGS);
		};
	};
};

void Picture::describe_type(output_stream &message, Article article, boolean verbose, boolean capitalize_first) {
	if (indirection != NULL) {
//		print_article(article,message,"picture of ");
		indirection->describe_type(message,article,verbose,capitalize_first); // INDEFINITE_ARTICLE
		return;
	};
//	if (code == SYNTHETIC_SPRITE || code == SCALE_SPRITE) {
//		describe_appearance(code,message,article);
//		return;
//	};
//	if (kind_of() == BLANK_PICTURE) {
//      message << S(IDS_BLANK_PICTURE);
//		return;
//	};
   if (code == USER_SPRITE) {
      message << S_maybe_capitalize_first(IDS_USER_SPRITE,capitalize_first);
   } else if (code == SCALE_SPRITE) {
      describe_briefly(message,article,capitalize_first);
   } else if (verbose && fine_kind_of() != TOOL_BUTTON) {
      unsigned short int index;
		unsigned short int next_index;
      if (kind_of() == BLANK_PICTURE) {
         index = IDS_BLANK_PICTURE_OF;
			next_index = IDS_BLANK_PICTURE_OF2;
      } else if (still_picture(code)) {
         index = IDS_PICT_OF;
			next_index = IDS_PICT_OF2;
		} else {
         index = IDS_MOVIE_OF;
			next_index = IDS_MOVIE_OF2;
		};
//      print_with_article(article,index,message);
      message << S_maybe_capitalize_first(index,capitalize_first);
      message << space();
		describe_appearance(code,message,INDEFINITE_ARTICLE);
		message << S(next_index,"");
	} else {
//		describe_appearance(code,message,article);
      Sprite::describe_type(message,article,verbose,capitalize_first);
	};
};

void Picture::inside_thought_bubble() {
	// throw away stuff inside (how to undo?)
//	if (remote != NULL) {
// what goes here??
//	};
	Sprite::inside_thought_bubble();
};

boolean Picture::current_image_index(short int &index, SpriteCode &the_code) {
	if (is_blank()) return(FALSE);
	if (indirection != NULL) {
		if (indirection->kind_of() == PICTURE) {
			return(((Picture *) indirection)->current_image_index(index,the_code));
		} else {
			return(FALSE);
		};
	};
//	if (code == SYNTHETIC_SPRITE) {
		// parameter treated same mod number of colors
//		index = (unsigned short int) (current_parameter()%tt_colors_available);
//		the_code = appearance_code();
//		return(TRUE);
//	};
	index = current_parameter();
	the_code = appearance_code();
	return(TRUE);
};

short int Picture::picture_id() {
	// encodes image_index and cycle number
	return(compute_picture_id(appearance_code(),current_parameter()));
};

short int compute_picture_id(SpriteCode code, short int index) {
//#if TT_BETA
//	if (tt_running_old_demos) { // 3 new ones added since demos created
//		return((short) (((short int) code)*(tt_appearance_count-3)+index));
//	} else
//#endif
//	return((short) (((short int) code)*tt_appearance_count+index));
  // negative to be different from old scheme
  // 100 is maximum images per sprite
   if (code == 0) { // not initialized
      code = HIT_OR_MISS_SPRITE;
      index = NOT_COLLIDING;
   };
   return((short) (((short int) code)*100+index));
};

void decode_picture_id(short id, short int &index, SpriteCode &the_code) {
//#if TT_BETA
//	if (tt_running_old_demos) { // 3 new ones added since demos created
//		index = (short) (id%(tt_appearance_count-3));
//		the_code = (SpriteCode) (id/(tt_appearance_count-3));
//	} else
//#endif
//#if TT_BETA
//   if (id > 0) { // old scheme from beta 4 and before
//      index = (short) (id%60); // where 60 was the old tt_appearance_count
//	   the_code = (SpriteCode) (id/60);
//      return;
//   };
//#endif
	index = (short) (id%100);
	the_code = (SpriteCode) (id/100);
};

void Picture::generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection) {
	if (is_blank()) {// && code != SYNTHETIC_SPRITE) {
		// could say more if done this before
		if ((contained_in(THOUGHT_BUBBLE) && !did(PICTURE_BLANKED_TB) &&
			 !did(PICTURE_BLANK_EXPLAINED)) ||
			 tt_help_counter < 2) { // was > 0 prior to 130201 but that can cause repeated saying of the following
//			 if (tt_help_counter == 0) {
				 describe_pointing(message,tool_in_hand,selection);
//			 };
		} else if ((!did(PICTURE_REDIRECTED) && tt_help_counter == 0) 
					  || tt_help_counter == 1) {
			tt_help_history_action = PICTURE_REDIRECTED;
			message << S(IDS_PICTURE_ON_BLANK_PICTURE)
					  << S(IDS_OR_NUMBER_OR_TEXT);
		} else if ((!did(PICTURE_BLANKED_TB) && tt_help_counter == 0) 
					  || tt_help_counter == 2) {
//			if (tt_help_counter == 0 && contained_in(THOUGHT_BUBBLE)) {
//				message << S(IDS_PICT_BLANKED_TB_HELP);
//				user_did(PICTURE,PICTURE_BLANKED_TB);
//			} else {
				message << S(IDS_PICT_BLANKED_TB_HELP2)
						  << S(IDS_PICT_BLANKED_TB_HELP3);
				user_did(PICTURE,PICTURE_BLANK_EXPLAINED);
//			};
		};
		return;
	};
	if (code == SCALE_SPRITE) { // treat this like a separate data type
		if ((!did(SCALE_USED) && tt_help_counter == 0) || 
			 tt_help_counter == 1) {
			tt_help_history_action = SCALE_USED;
			describe_pointing(message,tool_in_hand,this);
			message << S(IDS_SCALE_USE1)
					  << S(IDS_SCALE_USE2)
					  << S(IDS_SCALE_USE3)
					  << S(IDS_SCALE_USE4);
		} else if ((!did(SCALE_KEYBOARDED) && tt_help_counter == 0) 
					  || tt_help_counter == 2) {
			tt_help_history_action = SCALE_KEYBOARDED;			 
			// better to explain the same stuff as PICTURE_CHANGED_BY_KEYBOARD below??
			message << S(IDS_SCALE_KEYBOARD_HELP);
		};
		return;
	};
	if (indirection != NULL && !is_flipped()) {
//		if (tool_in_hand == this) { 
//			Sprite::generate_help(message,indirection);
//		} else {
			Sprite::generate_help(message,tool_in_hand,selection);
//		};
		return;
	};
	if ((!is_flipped() && !did(PICTURE_ADDED_PICTURE) && tt_help_counter == 0)
		  || tt_help_counter == 1) {
		tt_help_history_action = PICTURE_ADDED_PICTURE;
//		if (tt_help_counter == 0) {
			describe_pointing(message,tool_in_hand,selection);
//		};
		message << S(IDS_PICT_ADDED_PICTURE_HELP);
//		if (tt_help_counter == 0 && still_picture(code)) {
//			message << S(IDS_PICT_ADDED_PICTURE_HELP2);
//		} else {
//			message << S(IDS_PICT_ADDED_PICTURE_HELP3);
//		};
	} else if ((!is_flipped() && pointer_to_followers() != NULL && !did(PICTURE_REMOVED_PICTURE) && tt_help_counter == 0) 
				  || tt_help_counter == 2) {
		tt_help_history_action = PICTURE_REMOVED_PICTURE;
		if (tt_help_counter == 0) {
			describe_pointing(message,tool_in_hand,selection);
		};
		message << S(IDS_PICT_REMOVED_PICTURE_HELP);
		if (tt_histories[VACUUM] == 0 || tt_help_counter == 2) {
			message << S(IDS_PICT_REMOVED_PICTURE_HELP2);
		}; 
	} else if ((!is_flipped() && parameter_count(code) > 1 && code != SYNTHETIC_SPRITE &&
				  !did(PICTURE_CHANGED_BY_KEYBOARD) && tt_help_counter == 0)
				  || tt_help_counter == 3) {
		tt_help_history_action = PICTURE_CHANGED_BY_KEYBOARD;
		if (tt_help_counter == 0) {
			message << S(IDS_PICT_CHANGED_KEY_HELP) << parameter_count(code);
         if (total_cycle_duration() > 1) {
            message << S(IDS_MOVIES);
         } else {
            message << S(IDS_PICTURES);
         }; 
         message << end_sentence() << S(IDS_PICT_CHANGED_KEY_HELP2);
		} else {
			message << S(IDS_PICT_CHANGED_KEY_HELP3);
		};
		message << S(IDS_PICT_CHANGED_KEY_HELP4);
	} else if ((!is_flipped() && code == SYNTHETIC_SPRITE &&
				  !did(PICTURE_COLOR_BY_KEYBOARD) && tt_help_counter == 0)
				  || tt_help_counter == 4) {
		tt_help_history_action = PICTURE_COLOR_BY_KEYBOARD;
		if (tt_help_counter == 0) {
			message << S(IDS_PICT_COLOR_KEY_HELP);
		} else {
			message << S(IDS_PICT_COLOR_KEY_HELP2);
		};
		message << S(IDS_PICT_COLOR_KEY_HELP3);
	} else if ((!is_flipped() && !did(PICTURE_EXPANDER_HELP_GIVEN) && tt_help_counter == 0) 
					|| tt_help_counter == 5) {
		message << S(IDS_EXPANDER_ON_PICTURE_HELP)
				  << S(IDS_EXPANDER_ON_PICTURE_HELP2);
		no_help_interruptions();
		user_did(PICTURE,PICTURE_EXPANDER_HELP_GIVEN);
	} else if ((!is_flipped() && !did(PICTURE_TOSSED) && tt_help_counter == 0)
				  || tt_help_counter == 6) {
		tt_help_history_action = PICTURE_TOSSED;
		if (tt_help_counter == 0 && tool_in_hand == this) {
			message << S(IDS_PICT_TOSSED_HELP,""); 
		} else {
			message << S(IDS_PICT_TOSSED_HELP2,"");
		};
		describe_middle_button(message);
		if (tt_help_counter == 0 && tool_in_hand == this) { // new on 270100
			message << S(IDS_PICT_TOSSED_HELP_2,""); 
		} else {
			message << S(IDS_PICT_TOSSED_HELP2_2,"");
		};
		message << end_sentence();
	} else if ((!is_flipped() && !did(PICTURE_BLANKED) && tt_help_counter == 0)
					|| tt_help_counter == 7) {
		tt_help_history_action = PICTURE_BLANKED;
		message << S(IDS_VACUUM_PICTURE_BLANK);
	} else if ((!is_flipped() && !did(PICTURE_FLIPPED) && tt_help_counter == 0) 
				  || tt_help_counter == 8) {
		tt_help_history_action = PICTURE_FLIPPED;
		message << S(IDS_PICT_FLIPPED_HELP);
		describe_right_button(message,tt_keyboard_accelerators[FLIP_THING_HELD]);
		if (tt_help_counter == 0 && tool_in_hand == this) {
			message << end_sentence(); // S(IDS_NOW) << end_sentence();
		} else {
			message << S(IDS_PICT_FLIPPED_HELP2);
		};
      message << S(IDS_NOTEBOOK_OF_REMOTES_FLIES);
	} else if ((is_flipped() && !did(PICTURE_FLIPPED_BACK) && tt_help_counter == 0)
				  || tt_help_counter == 9) {
		tt_help_history_action = PICTURE_FLIPPED_BACK;
		message << S(IDS_PICT_FLIPPED_BACK_HELP);
		describe_right_button(message,tt_keyboard_accelerators[FLIP_THING_HELD]);
		if (tt_help_counter == 0 && tool_in_hand == this) {
			message << S(IDS_AGAIN) << end_sentence();
		} else {
			message << S(IDS_PICT_FLIPPED_BACK_HELP2);
		};
//	} else if ((is_flipped() && !did(PICTURE_NOTEBOOK_COPIED) && tt_help_counter == 0) || 
//		 tt_help_counter == 9) {
//		tt_help_history_action = PICTURE_NOTEBOOK_COPIED;
//		if (tt_help_counter == 0) {
//			message << S(IDS_PICT_NOTEBOOK_COPIED_HELP);
//		} else {
//			message << S(IDS_PICT_NOTEBOOK_COPIED_HELP2);
//		};
//		message << S(IDS_PICT_NOTEBOOK_COPIED_HELP3);
	} else if ((is_flipped() && !did(PICTURE_CUBBY_OR_ROBOT_ADDED) && tt_help_counter == 0)
					|| tt_help_counter == 10) {
		tt_help_history_action = PICTURE_CUBBY_OR_ROBOT_ADDED;
		message << S(IDS_PICT_PROGRAM_ADDED_HELP)
				  << S(IDS_PICT_PROGRAM_ADDED_HELP2)
				  << S(IDS_PICT_PROGRAM_ADDED_HELP3)
				  << S(IDS_PICT_PROGRAM_ADDED_HELP4);
	} else if ((is_flipped() && !did(PICTURE_CUBBY_OR_ROBOT_REMOVED) && tt_help_counter == 0)
				  || tt_help_counter == 11) {
		tt_help_history_action = PICTURE_CUBBY_OR_ROBOT_REMOVED;
		message << S(IDS_PICT_PROGRAM_REMOVED_HELP);
	} else if ((!did(TYPE_HELP_GIVEN) && tt_help_counter == 0) 
				  || tt_help_counter == 12) {
//		tt_help_history_action = TYPE_HELP_GIVEN;
		message << S(IDS_PICT_HELP_GIVEN_HELP)
				  << S(IDS_PICT_HELP_GIVEN_HELP2);
		no_help_interruptions();
		user_did(PICTURE,TYPE_HELP_GIVEN);
	};
};

boolean Picture::history_full() {
	if (code == SCALE_SPRITE) { // treat this like a separate data type
		 return(did(SCALE_USED) && did(SCALE_KEYBOARDED));
	} else {
		return(Sprite::history_full());
	};
};

// ActionStatus dummy_action_status = 1;

Sprite *Picture::used(Sprite *, Sprite *by,
							 boolean , boolean record_action,
							 millisecond , boolean modified, Sprite *) {
   if (!puzzle_oks_flipping(by)) return(this);
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Using debug target picture.");
	};
#endif
/*
	if (by->kind_of() == ROBOT) { // new on 210300 so a robot can flip over a picture without messing it up - even this one
		Background *background = by->pointer_to_background();
		if (background != NULL) {
			Picture *background_picture = background->pointer_to_picture();
			if (background_picture != NULL) { // on the back of a picture
				if (background_picture->is_flipped()) {
					background_picture->unflip(TRUE,FALSE);
				} else {
					background_picture->flip(TRUE,FALSE);
					if (remote_notebook == NULL) {
						remote_notebook =	new_remote_notebook(floor,FALSE); 
					};
					record_event(NEW_ITEM,by,background,remote_notebook,TRUE); // in case robot uses it
				};
			};
			completed(action_status);
			return(this);
		};
	};
	*/
	if (!by->still_running()) simulate_mouse_up();
	if (appearance_code() == SCALE_SPRITE) {
		by->abort_action(IDS_ABORT_CANT_FLIP_SCALE);
		return(this);
	};
	// stuff about giving the picture a toss was here prior to 101199 but is now part of :released
	// commented out on 210499 the following since you should be able to flip over remote looks
//	if (controlee != NULL) { 
//		by->abort_action(action_status,S(IDS_CANT_FLIP_REMOTES));
//      controlee->respond_to_keyboard(modified?'-':'+',FALSE,by,record_action,action_status);
//      set_clean_status(controlee->current_clean_status());
//		return(this);
//	};
	// finish last flip if still in progress
	finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
	set_saved_by(by); // moved after the above since needed to finish properly
	// ignoring duration -- is that ok??
//	save_size(); -- problem if picked up when flipped depending upon
// whether another save_size occurs, for example
   // without &dummy_action_status will picture::describe do the right thing? -- asked on 171102
	//if (action_status == NULL) {
	//	// used so Picture::describe knows something's happening
	//	saved_action_status = &dummy_action_status;
	//} else {
	//	saved_action_status = action_status;
	//};
	if (!is_flipped()) {
		// following added on 251099
		if (pointer_to_remote_notebook() == NULL && 
			 !(tt_system_mode == PUZZLE && !ok_to_create_remote_notebook() && (saved_by == NULL || saved_by->kind_of() != ROBOT))) {
			if (controlee != NULL) { // new on 121200 - restored 080604
				controlee->just_set_remote_notebook(controlee->new_remote_notebook(floor,FALSE));
			} else {
				remote_notebook = new_remote_notebook(floor,FALSE); // FALSE was (tt_log_version_number < 7);
			};
			//if (controlee != NULL) { // 080604 version
			//	Notebook *notebook = controlee->pointer_to_remote_notebook();
			//	if (notebook == NULL) {
			//		notebook = controlee->new_remote_notebook(floor,FALSE);
			//	};
			//	if (notebook->showing_on_a_screen()) { // conditional new on 080604
			//		notebook = (Notebook *) notebook->copy(); // should be remote_notebook = but experimenting on 080604
			//	};
			//	controlee->just_set_remote_notebook(controlee->new_remote_notebook(floor,FALSE));
			//} else {
			//	remote_notebook = new_remote_notebook(floor,FALSE); // FALSE was (tt_log_version_number < 7);
			//};
		}; // else 
//		if (remote_notebook != NULL) { // moved here on 201299 so doesn't happen if notebook is brand new
//			remote_notebook->reset_picture_copy((Picture *) copy());
//		};
		// commented the following out on 191299 since this lead to confusion where
		// things didn't work without typing space and this was unexpected.
//		set_active(FALSE); // added on 261199 so nothing runs while flipped
		// note that this means you need to start it up again when flipped back over
//		if (showing_on_a_screen()) { // commented out on 311000
//			side_about_to_be_seen(FALSE); // new on 240200
//		} else { // following was unconditional prior to 240200
			stop_all(FALSE); // in case robots are running on the back
//		};
		millisecond duration = default_duration(750); // moved here on 050201
		if (duration > 0) { // conditional new on 050201
			set_width_before_flip(width); // was ordinary assignment prior to 060403
			set_height_before_flip(height);
			animate_size_and_location(width,tile_height/5,
											  llx,lly+height/2,
											  duration,
											  this,FLIP_TO_BACK_CALLBACK);
			user_did(PICTURE,PICTURE_FLIPPED,by);
		} else {
			flip_to_back(); // new on 050201
		};
	} else {
		millisecond duration = default_duration(500); // moved here on 050201
		if (duration > 0) {
			// shrink back to original size first
			animate_size_and_location(return_width_before_flip(),return_height_before_flip(),
											  llx-(return_width_before_flip()-width)/2,
											  lly-(return_height_before_flip()-height)/2,
											  duration,
											  this,START_FLIP_TO_FRONT_CALLBACK);
			user_did(PICTURE,PICTURE_FLIPPED_BACK,by);
		} else {
         // following new on 211002 - since this is the size would have had if watched
         if (width_before_flip > 0 && height_before_flip > 0) {  
            set_size(return_width_before_flip(),return_height_before_flip());
         //} else if (saved_width > 0 && saved_height > 0) { // commented out on 221002
         //   set_size(saved_width,saved_height);
         };
         // and if a robot takes a flipped picture out of a box then this can be important
			start_flip_to_front(); // new on 050201
		};
	};
	if (record_action) record_event(APPLY_GRASPED_ITEM,by,floor);
	return(this);
};

void Picture::flip_to_back() {
#if TT_DEBUG_ON
	millisecond start = timeGetTime();
#endif
//	set_activated(FALSE); // so can be re-activated -- new on 180405
	flip_going_on_flag = TRUE;
	flipped = TRUE;
	// so true_x_offset() is 0 so stuff stuck on back follows leader right
// following commented out since it causes geometry problems when a robot adds
// a picture to a picture after flipping it (to get sensors) 1/10/99
//	old_x_offset = 0;
//	old_y_offset = 0;
	swap_followers_to_back();
#if TT_DEBUG_ON
	if (tt_debug_mode == 91100) {
		millisecond new_start = timeGetTime();
		tt_error_file() << (new_start-start) << "ms to swap followers to back." << endl;
		start = new_start;
	};
#endif
	if (controlee != NULL) { // new on 210499 - remote looks should have copy of flipped followers of controlee
//		if (showing_on_a_screen()) { 
		// conditional added 091100 to postpone what can be a very expensive operation 
		// (and needless if robot just wants the notebook)
//			copy_flipped_followers_of_controlee(this);
#if TT_DEBUG_ON
			if (tt_debug_mode == 91100) {
				millisecond new_start = timeGetTime();
				tt_error_file() << (new_start-start) << "ms to copy flipped followers of controlee." << endl;
				start = new_start;
			};
#endif
//		} else {
//			postponed_copy_flipped_followers_of_controlee_flag = TRUE;
//		};
	};
	remote_notebook_flies_out(); // moved here on 050201
	millisecond duration = default_duration(750);
	if (duration > 0) { // conditional new on 050201
		animate_size_and_location(width,return_height_before_flip(),
										  llx,lly-return_height_before_flip()/2,
										  duration,
										  this,EXPAND_BACK_CALLBACK);
   // it now flies out when sizes are more reasonable...
	} else {
		expand_back(); // new on 050201
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 91100) {
		millisecond new_start = timeGetTime();
		tt_error_file() << (new_start-start) << "ms to expand back and have remote notebook fly out." << endl;
		start = new_start;
	};
#endif
};

/* obsolete on 151200
void Picture::copy_flipped_followers_of_controlee_now() {
	if (controlee != NULL && postponed_copy_flipped_followers_of_controlee_flag && !copying_flipped_followers_of_controlee_flag) {
#if TT_DEBUG_ON
		int start_counter = ::return_debug_counter();
		millisecond start = timeGetTime();
#endif
		copying_flipped_followers_of_controlee_flag = TRUE;
		copy_flipped_followers_of_controlee(this);
		copying_flipped_followers_of_controlee_flag = FALSE;
		postponed_copy_flipped_followers_of_controlee_flag = FALSE;
#if TT_DEBUG_ON
		if (tt_debug_mode == 101100) {
			tt_error_file() << "It took " << (timeGetTime()-start) << "ms to copying flipped followers on frame " << tt_frame_number << " and " << (::return_debug_counter()-start_counter) << " sprites were created. ";
			print(tt_error_file());
			tt_error_file() << endl;
		};
#endif
	};
};

void Picture::copy_flipped_followers_of_controlee(Picture *copy) {
	Sprites *sorted_flipped_followers = controlee->sort_flipped_followers();
	if (sorted_flipped_followers != NULL) {
		// commented out the following since never used since flipping destroyed followers so should always be NULL
//		Sprites *current_flipped_followers = copy->flipped_followers();
//		if (current_flipped_followers != NULL) { // new on 221299 to avoid adding twice
//			if (current_flipped_followers->length() == sorted_flipped_followers->length()) {
//				return; // already added
//			};
//			copy->remove_all_followers(TRUE); // since about to add the "real" ones
//		};			
		// re-written on 191199 to use top_level_copy since old way meant that birds and nests 
		// crossed between "the real world" and the remote control's copy
		boolean controlee_active = controlee->active();
		if (controlee_active) controlee->set_active(FALSE,TRUE); // new on 191199 so stuff below is copied without being activated
		// prior to 201299 the following used tt_copying_entire_thought_bubble rather than tt_dont_connect_things_while_copying
		// which caused problems with pictures like remote looks
		boolean saved_tt_dont_connect_things_while_copying = tt_dont_connect_things_while_copying;
		tt_dont_connect_things_while_copying = TRUE;
//		Sprites *me_and_sorted_flipped_followers = new Sprites(this,sorted_flipped_followers);
		// me_and_sorted_flipped_followers is new on 211299 since controlee might be
		// referenced on by its flipped followers
		Sprites *flipped_followers_copy = sorted_flipped_followers->copy(); // was top_level_copy() prior to 211299
		tt_dont_connect_things_while_copying = saved_tt_dont_connect_things_while_copying;
		if (controlee_active) controlee->set_active(TRUE,TRUE);
//		Picture *controlee_copy = (Picture *) flipped_followers_copy->first();
		Sprites *remaining = flipped_followers_copy; //->rest();
		Sprite *previous_follower = NULL;
		while (remaining != NULL) {
			Sprite *follower = remaining->first();
			add_flipped_follower(follower,previous_follower,copy);
			previous_follower = follower;
			remaining->set_first(NULL); // to delete list afterwards
			remaining = remaining->rest();
		};
//		controlee_copy->just_set_flipped_followers(NULL);
//		controlee_copy->destroy(); // served its purpose of making connections in copy of flipped followers
		sorted_flipped_followers->remove_all();
		delete sorted_flipped_followers;
		delete flipped_followers_copy;
	};			
//		Sprites *remaining = sorted_flipped_followers;
//		Sprite *previous_follower = NULL;
//		while (remaining != NULL) {
//			Sprite *follower = remaining->first()->copy();
//			add_flipped_follower(follower,previous_follower,this);
//			previous_follower = follower;
//			remaining = remaining->rest();
//		};
//		if (sorted_flipped_followers != NULL) {
//			sorted_flipped_followers->remove_all();
//			delete sorted_flipped_followers;
//		};
};
*/

void Picture::action_completed() { // new on 211102
   if (saved_by == NULL && controlee != NULL) { // transfered it so
      controlee->action_completed();
      return;
   };
   Sprite::action_completed();
};

void Picture::remote_notebook_flies_out(Background *background_of_picture) {
	// background_of_picture is a new arg on 280103 since this prevents reuse of the current notebook
	// e.g. the controlee is elsewhere (you are training and its on the floor or 2 floors involved)
	// updated on 070604 to return notebook -- not useful by the end of 070604 so removed on 080604
   if (tt_system_mode == PUZZLE && !ok_to_create_remote_notebook() && (saved_by == NULL || saved_by->kind_of() != ROBOT)) {
		return;
	};
	Notebook *notebook = pointer_to_remote_notebook(); // might come from controlee (new on 121200)
	Background *background_of_by; // moved earlier on 240205 in case need to record the event below
	if (saved_by != NULL && (saved_by->still_running() || saved_by->is_programmer_appearance())) { // was saved_by->pointer_to_programmer() != NULL)) { 
		// second disjunct added 060201 -- and rewritten on 201202
		background_of_by = saved_by->pointer_to_background();
	} else if (background_of_picture != NULL) {
		background_of_by = background_of_picture; // new on 280103
	} else {
		background_of_by = floor; // of picture
	};
	if (notebook != NULL && notebook->showing_on_a_screen()) {
		// new on 130704 so if flipping a remote control and the notebook is already out then nothing happens
		record_event(NEW_ITEM,saved_by,background_of_by,notebook,TRUE); // new on 240205 since robot needs to add to working set
		return;
	};
	if (controlee != NULL) { // new on 121200 - restored 080604
		controlee->set_saved_by(saved_by); 
		saved_by = NULL; // transfered it
		controlee->remote_notebook_flies_out(pointer_to_background());
		return;
	};
//	if (controlee != NULL) { // new version from 070604
//		controlee->set_saved_by(saved_by); 
//		saved_by = NULL; // transfered it
//		if (remote_notebook == NULL) {
//			Notebook *notebook = controlee->pointer_to_remote_notebook();
//			if (notebook == NULL) {
//				notebook = controlee->new_remote_notebook(floor,FALSE);
//			};
//			if (notebook->showing_on_a_screen()) { // conditional new on 080604
//				just_set_remote_notebook((Notebook *) notebook->copy()); 
	// should be just_set_remote_notebook but experimenting on 080604
//			};
//		};
//		//Notebook *old_notebook = pointer_to_remote_notebook();
//		//Notebook *notebook = controlee->remote_notebook_flies_out(pointer_to_background());
//		//if (notebook != old_notebook && old_notebook != NULL) { // was copied -- so let remote looks keep it -- new on 070604
//		//	just_set_remote_notebook(notebook);
//		//};
////		return(remote_notebook);
//	};
	//if (notebook == NULL && controlee != NULL) { // new on 080604
	//	notebook = controlee->pointer_to_remote_notebook();
	//};
	if (notebook == NULL) {
		if (controlee != NULL) { // already taken care of above - new on 070604 - restored on 080604
			notebook = controlee->new_remote_notebook(floor,FALSE);
			controlee->just_set_remote_notebook(notebook);
		} else {
			notebook = new_remote_notebook(floor,FALSE); // FALSE was (tt_log_version_number < 7);
			just_set_remote_notebook(notebook);
		};
	} else {
		notebook->finish_instantly(); // new on 050704 so the following doesn't create multiple copies if this one is still animating
		notebook->set_selectable(TRUE); // new on 270704 since if vacuumed by a robot may have been set to unselectable
	}; // else if (notebook->inside_vacuum()) return; 
	// notebook->showing_on_a_screen() added 161200 - and moved below on 070602 -- why not make a new one?
	// following commented out on 070604 since already taken care of now -- restored on 080604
   if (!tt_programmer->training() && // condition added on 240205 since will confuse robot training to copy the notebook
		 // this wasn't the bug (issue 652) so undone on 240205 and then restored after 3.155 since otherwise when flipped back over
		 // notebook doesn't return
		 ((background_of_picture != NULL && background_of_picture != pointer_to_background() && 
		   background_of_picture->pointer_to_picture() == NULL) || 
		  (!notebook->showing_on_a_screen() && notebook->pointer_to_background() != NULL))) { 
			  // added ! on 130704 -- and NULL background test so just flies out instead
		// following moved below on 260104
//		 notebook->inside_vacuum()) { // moved here (and changed policy) on 230903 -- 
			  // needed for Playground adventure game (but not clear what changed)
		// there already is one out so continue with a copy
      // note that this notebook will not go away when the picture is flipped back over - sigh
		// first disjunct new on 280103 since can be a controlee with a different background
	   // added background_of_picture->pointer_to_picture() == NULL on 150703 
			  // since no need to copy if this is all happening on the back of pictures
#if TT_DEBUG_ON
		debug_this();
#endif    
		notebook = (Notebook *) notebook->copy();
	} else if (notebook->inside_vacuum()) { // moved here on 260104 to update references properly
		Notebook *new_notebook = (Notebook *) notebook->copy();
		release_remote_notebook(); // new on 260104 - had this as arg prior to 020604 (by)
		just_set_remote_notebook(new_notebook);		
   };
	record_event(NEW_ITEM,saved_by,background_of_by,notebook,TRUE);
   if (saved_by != NULL) {
		saved_by->notebook_just_flew_out(notebook); // new on 201200 so it forgets page number it last used
      //saved_by->destroy(); // if ref count is zero -- commented out on 191102 since used for action_completed() now
      //saved_by = NULL; // finished with it
   };
	if (background_of_by == NULL || background_of_by->pointer_to_picture() != NULL) { 
		return; // new on 191200
	};
	notebook->set_visible(TRUE); // new on 191200 since can be made invisible temporarily
	city_coordinate center_x,center_y;
	center_location(center_x,center_y);
	notebook->set_size_and_location(1,1,center_x,center_y);
	city_coordinate goal_x,goal_y;
	city_coordinate good_width,good_height;
	// commented out on 191200 the following to make it unconditional since return if not visible
	// args added to the following on 040899 to avoid spurious warnings
//   if (background_of_by != NULL && background_of_by->pointer_to_picture() == NULL) { 
	// condition added 070900 and second one on 121200
		// was floor prior to 121200 but that isn't right when dealing with remote looks now
//		background_of_by->add_item(notebook,TRUE,FALSE,FALSE);  
	// do this early so good_size is sensitive to whether on floor or standing in room
//	};
	notebook->remove_from_floor(FALSE); // new on 240205
	background_of_by->add_item(notebook,TRUE,FALSE,FALSE);  
	// do this early so good_size is sensitive to whether on floor or standing in room
	notebook->good_size(good_width,good_height,tt_system_mode==PUZZLE?NULL:tt_toolbox); 
	// as of 081199 don't use Toolbox if doing puzzles
	goal_x = llx-good_width/2;
	if (goal_x < 0) {
		goal_x = 0;
	} else if (goal_x > 16*tile_width) {
		goal_x = 16*tile_width;
	};
	// end up just one tile height from top or bottom
	if (center_y < half_screen_height) {
		goal_y = (ideal_screen_height-good_height)-tile_height;//center_y+6*tile_height;
	} else {
		goal_y = tile_height; //(center_y-12*tile_height)-good_height;
	};
	// re-ordered on 161103 since if flipping while scrolling then can add pages while the size is 1 city_cooordinate unit
	notebook->animate_size(good_width,good_height,default_duration(1000));
	notebook->fly_to(goal_x,goal_y,default_duration(1000),TRUE);
	//notebook->fly_to(goal_x,goal_y,default_duration(1000),TRUE);
	//notebook->animate_size(good_width,good_height,default_duration(1000));
	return;
};

void Picture::flip(boolean temporarily, boolean change_size) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Flipping debug target.");
	};
#endif
	if (!is_flipped()) {
		// following moved here on 190299 from the end of this method since geometry was broken of followers
		swap_followers_to_back(change_size,temporarily);
		if (change_size) {
			if (width >= minimum_dimension || height >= minimum_dimension) {
				// temporary size got stuck here somehow - fix introduced on 311000
				city_coordinate good_width, good_height;
				Sprite::good_size(good_width,good_height); // so flipped and the like don't matter using Sprite::
				if (temporarily) { // conditions new on 280103
					set_width_before_flip(good_width);
					set_height_before_flip(good_height);
				};
			} else if (temporarily) { // conditions new on 280103
				set_width_before_flip(width);
				set_height_before_flip(height);
			};
			if (temporarily) {
				if (leader == NULL) {
               backside_size(width,height,tt_system_mode==PUZZLE?NULL:tt_toolbox); 
					// as of 081199 don't use Toolbox to calculate good size if doing puzzles
				} else {
#if TT_DEBUG_ON
               if (tt_debug_target == debug_counter) {
                  log("Temporarily setting the width and height of debug target.");
               };
#endif
					width = old_width;
					height = old_height;
				};
			} else { // set clean status etc
            city_coordinate backside_width, backside_height;
            backside_size(backside_width,backside_height,tt_system_mode==PUZZLE?NULL:tt_toolbox); 
				// as of 081199 don't use Toolbox to calculate good size if doing puzzles
				set_size(backside_width,backside_height);
			};
		};
		// do this after above so that dimension are right for propagation
		flipped = TRUE;
		if (temporarily) {
			set_temporarily_flipped(TRUE);
//			set_temporarily_unflipped(FALSE);
		};
//		if (tt_log_version_number < 15) set_size_is_true(TRUE); // use width and height not current_image dimensions
		set_placed_new_items(FALSE);
	};
};

void Picture::unflip(boolean temporarily, boolean change_size) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Un-flipping debug target.");
	};
#endif
	if (is_flipped()) {
		// following moved here on 190299 from the end of this method since geometry was broken of followers
		swap_followers_to_front(change_size,temporarily);
//		if (change_size && tt_log_version_number < 15) {
			// following commented out 100399 because it broke the geometry of subpictures of a flipped pictures
//			if (temporarily) {
//				width = width_before_flip;
//				height = height_before_flip;
//			} else { // set clean status etc
//				set_size(width_before_flip,height_before_flip);
//			};
//		};
		flipped = FALSE;
		if (temporarily) {
			set_temporarily_flipped(FALSE);
//			set_temporarily_unflipped(TRUE);
			// following commented out on 080105 since causes double deletion when parts sensor is involved
			// and for quite a while the parts sensor works on the front or back so this doesn't represent a change
		//} else if (remote != NULL) { 
			// new on 040100 -- made an else on 300103 since if temporary shouldn't update remotes - caused a crash while saving
		//	remote->followers_changed();
		};
//		if (code != SYNTHETIC_SPRITE) { // synthetic ones stay "true"
//			set_size_is_true(FALSE);
//		};
	};
};

/*
city_coordinate Picture::full_width() {
	if (is_flipped()) return(width);
	return(Sprite::full_width());
};

city_coordinate Picture::full_height() {
	if (is_flipped()) return(height);
	return(Sprite::full_height());
};
*/

void Picture::add_pages_to_remote_notebook() {
	Notebook *notebook = NULL; // initialized to fail much softer (crashed otherwise when notebook not set below) - new on 020404
	int initial_page_number = 1; 
	// new on 061003 since otherwise cities can be saved with remote control notebooks open to non-page one and are loaded as page 1
	// commented out the following on 070604 - restored on 080604
	if (remote_notebook == NULL) { // new on 050201
		if (controlee != NULL) {
			notebook = controlee->pointer_to_remote_notebook();
		}; // else warn??
		initial_page_number = 1; // new with TT_ALPHA
	} else {
		notebook = remote_notebook;
		initial_page_number = notebook->left_page_number(); // new with TT_ALPHA
	};
	if (notebook == NULL) {
		log("Picture couldn't find its notebook of remote controls.",FALSE,TRUE);
		return; 
	};
   recursively_propagate_changes(); // so everything is the right size for the following:
   add_pages_to_notebook(notebook,floor);
   notebook->go_to_page(initial_page_number);
};

void Picture::expand_back() {
	// animate to nice default size centered around current position
	// so easy to manipulate
   millisecond duration = default_duration(500);
	if (duration > 0) { // conditional new on 050201
		city_coordinate backside_width, backside_height;
		backside_size(backside_width,backside_height,tt_system_mode==PUZZLE?NULL:tt_toolbox); 
		// as of 081199 don't use Toolbox to calculate good size if doing puzzles
		city_coordinate future_llx = llx-(backside_width-width); // right edge steady
		city_coordinate future_lly = lly-(backside_height-height)/2; //centered
		boolean x_off_screen = (future_llx < 0 || future_llx > tt_screen->max_x());
		boolean y_off_screen = (future_lly < 0 || future_lly > tt_screen->max_y());
		if ((x_off_screen || y_off_screen) && tt_log_version_number >= 76) {
			// new on 030605 since if flipping a very big picture you end up holding it while it is offscreen
			city_coordinate programmer_llx, programmer_lly;
			tt_programmer->lower_left_corner(programmer_llx,programmer_lly);
			if (x_off_screen) {
				future_llx = programmer_llx-backside_width;
			};
			if (y_off_screen) {
				future_lly = programmer_lly-backside_height;
			};			
		};
		if (tt_log_version_number >= 76) { // new on 030605
			tt_programmer->set_grasp_delta_x(0);
			tt_programmer->set_grasp_delta_y(0);
		};
		animate_size_and_location(backside_width,backside_height,
										  future_llx,future_lly,
										  duration,this,ACTION_COMPLETED_CALLBACK);
//   if (tt_log_version_number < 7) {
//      add_pages_to_remote_notebook(); // do it now
//   } else {
      do_after(duration,this,NOW_FLIPPED_CALLBACK);
//		if (remote_notebook != NULL) { 
		// still want this conditional??? -- 
		// removed on 180903 since causes parts sensors when a remote looks is flipped to have a blank remote control notebook
			do_after(duration,this,ADD_PAGES_TO_REMOTE_NOTEBOOK_CALLBACK);
		//} else if (controlee != NULL) {
		//	do_after(duration,controlee,ADD_PAGES_TO_REMOTE_NOTEBOOK_CALLBACK); // new on 180903
		//};
	} else { // new on 050201
		action_completed();
		now_flipped();
//#if TT_POST318
//		if (remote_notebook != NULL)  
		// commented out conditional on 190803 -- 
		// restored 241003 because very expensive when on the back of a picture a robot flips over a picture
//			// and it seems it sometimes gets the sensors to have the wrong allegience
//#endif
			add_pages_to_remote_notebook(); 
//		};
	};
};

void Picture::send_remote_notebook_home() { // re-modularized on 121200
	if (remote_notebook != NULL && remote_notebook->pointer_to_picture() == this && remote_notebook->pointer_to_leader() == NULL) { 
      // second condition added 161200 -- extended with || remote_notebook->pointer_to_picture() == controlee on 070604 - 
		// restored on 080604
      // added condition that notebook's leader be NULL since if in a box or the like shouldn't move anywhere
      if (remote_notebook->inside_vacuum()) { // prior to 070602 this part of conditional test
         // but if flipping over and notebook has been vacuumed then Picture needs to forget about the notebook
         release_remote_notebook(); // had this as arg prior to 020604 (by)
      } else {
		   city_coordinate center_x,center_y;
		   center_location(center_x,center_y);
		   remote_notebook->go_to_page(1); // new on 161200 since used by robots who expect it to start on page 1	   
//		   remote_notebook->save_size();
		   if (remote_notebook->pointer_to_background() != NULL) { 
				remote_notebook->fly_to(center_x,center_y,1000); 
				// moved into conditional on 050805 since if has no background can't animate
			   // condition added on 280100 since otherwise call back happens when notebook is flying out and then isn't seen
			   remote_notebook->animate_size(1,1,1000,remote_notebook,REMOVE_FROM_FLOOR_CALLBACK);
		   };
		   remote_notebook->set_available_for_indexing(FALSE); // new on 131200
	   };
   };
};

// commented out on 070604 so picture and each remote looks have their own private copy -- restored on 080604
Notebook *Picture::pointer_to_remote_notebook() {
	//if (remote_notebook != NULL) {
	//	// rewritten on 070604 so if remote looks has a notebook it is used
	//	return(remote_notebook);
	//} else if (controlee != NULL) { // new on 121200
	//	return(controlee->pointer_to_remote_notebook());
	//} else {
	//	return(NULL);
 //  };
	if (controlee != NULL) { // new on 121200
		return(controlee->pointer_to_remote_notebook());
	} else {
      return(remote_notebook);
   };
};

void Picture::start_flip_to_front() {
	flip_going_on_flag = TRUE;
	if (controlee != NULL) { // moved here on 050201 from after animate_size_and_location below
		if (!controlee->is_flipped()) { // conditional new on 010301 since if controlee is flipped notebook should stay around
			controlee->send_remote_notebook_home(); 
			// removed controlee-> on 070604 so remote looks can clean up their notebook (if they have one themselves) - 
			// restored 080604
		};
	} else {
		send_remote_notebook_home();
	};
	millisecond duration = default_duration(750);
	if (duration > 0) { // conditional new on 050201
		animate_size_and_location(width,tile_height/5,
										  llx,lly+height/2,
										  duration,
										  this,
										  FINISH_FLIP_TO_FRONT_CALLBACK);	
	} else {
		finish_flip_to_front(); // new on 050201
	};
};
	
void Picture::finish_flip_to_front() {
	flipped = FALSE;
	if (controlee != NULL) { // new on 210499 so that remote looks keeps no state when unflipped
		if (!controlee->part_of(this)) { // condition added on 020500 since if controlee is on back of remote looks don't destroy it
			Picture *saved_controlee = controlee;
			controlee = NULL; // new on 161200 so following doesn't trigger anything
			remove_all_followers(TRUE);
			controlee = saved_controlee;
		};
		controlee->activate();
	};
	swap_followers_to_front();
//   increment_screen_ref_count(); // to fix "lie" when flipped
	millisecond duration = default_duration(750);
	if (duration > 0) { // conditional new on 050201
		animate_size_and_location(width,return_height_before_flip(),
										  llx-width/2,lly,duration,
										  this,FLIP_TO_FRONT_FINISHED_CALLBACK);
	} else {
		flip_to_front_finished();
	};
};

void Picture::flip_to_front_finished() {
	flip_going_on_flag = FALSE;
   action_completed();
//	if (showing_on_a_screen()) { // commented out on 311000
//		side_about_to_be_seen(TRUE); // new on 240200
//	} else { // following was unconditional prior to 240200
	   activate();
//	};
	really_propagate_changes();
	if (controlee != NULL) return; // new on 171200
	Sprites *remaining = flipped_followers(); // arg new on 091100
	if (remaining != NULL) { // new on 151199 so size of things on the back is "correct"
		city_coordinate flipped_width, flipped_height;
		backside_size(flipped_width,flipped_height);
		while (remaining != NULL) { 
			Sprite *follower = remaining->first();
			follower->follow_leader_size_changes(flipped_width,flipped_height);
			remaining = remaining->rest();
		};
	};	
	if (remote != NULL) { // new on 040100
		remote->followers_changed();
	};
	if (label != NULL) { // may have changed
		if (comment != NULL) { // new on 120604
			label->remove_follower(comment);
		};
		label->destroy();
		label = NULL;
	};
   // commented out on 211002 since not needed and breaks size of player in adventure game
	// save_size(); // new on 050601 so that if put on clipboard has the right saved sizes since might have been reset while flipped
};

void Picture::swap_followers_to_back(boolean change_size, boolean temporarily) {
//	city_coordinate saved_width = current_width();
//	city_coordinate saved_height = current_height();
//	if (change_size) { // new on 171200
//		set_size(width_before_flip,height_before_flip);
//	};
	Sprites *temp = followers_on_other_side;
	followers_on_other_side = followers;
	followers = temp;
	Sprites *remaining;
	if (followers != NULL) { // robots, cubbies, and the like
		remaining = followers;
		while (remaining != NULL) {
			Sprite *sprite = remaining->first();
			if (sprite != NULL) { 
				// condition new on 230704 since Gordon had a crash when unbamming -- maybe NULL due to release_all_references
				sprite->set_leader(this);
				if (!temporarily) {
					// conditional added on 191199 since this can happen if a robot (on the back of a picture) flips
					// over a picture and the notebook comes out and needs to dump a copy of itself to the notebook
					// and it probably doesn't make sense if just temporarily flipping it
					sprite->set_background(floor,TRUE,FALSE);
				};
#if TT_DEBUG_ON
			} else {
				debug_this();
#endif
			};
			remaining = remaining->rest();
		};
	};
	if (floor != NULL) { // removed  && !temporarily from here on 230101 since leader list isn't maintained correctly
		if (followers != NULL) { // this used to only be called if !temporarily prior to 201200
			floor->add_leader(this,!temporarily); // second arg added on 080104 to avoid spurious warnings
		} else if (followers_on_other_side != NULL) { 
			// condition added 201200 since wasn't incorrectly identified as a leader unless had followers on other side
			floor->remove_leader(this,!temporarily); // second arg added on 080104 to avoid spurious warnings
		};
		if (!temporarily) { // conditional moved here on 230101
			// changed the following on 150500 so that it isn't just in the second conditional branch above
			// this caused problems when removing the last thing from the back if the front had compound pictures on it
			// added following on 300399 so that pictures with pictures with pictures can be flipped properly
			Sprites *remaining = followers_on_other_side;
			while (remaining != NULL) {
				Sprite *sprite = remaining->first();
				if (sprite->pointer_to_followers() != NULL) {
					floor->recursively_remove_leader(sprite);
				};
				remaining = remaining->rest();
			};
		};
	};
	if (change_size) {
//		set_size(saved_width,saved_height);
		recursively_propagate_changes();
	};
	// commented out the following on 221299
//	if (remote != NULL && !temporarily) { 
	// added  && !temporarily since for example dumping shouldn't change sensors (e.g. CHILDREN_REMOTE)
//		remote->followers_changed(); // don't if flipped??
//	};
};

void Picture::swap_followers_to_front(boolean change_size, boolean temporarily) {
	Sprites *temp = followers_on_other_side;
	followers_on_other_side = followers;
	followers = temp;
	Sprites *remaining;
	if (followers_on_other_side != NULL) { // robots, cubbies, and the like
		if (backside_background == NULL) { // need a dummy background for these guys
			backside_background = new Backside_Background(this);
		};
		remaining = followers_on_other_side;
		while (remaining != NULL) {
			Sprite *sprite = remaining->first();
			if (sprite != NULL) { // for robustness - conditional new on 051103 
				if (sprite->pointer_to_leader() == this) { // conditional added Oct 8 '98
					sprite->set_leader(NULL,TRUE); // why??
				};
				sprite->set_background(backside_background,TRUE,FALSE);
			};
			remaining = remaining->rest();
		};
	};
	if (floor != NULL) { // re-written on 230101 && !temporarily) {
		if (followers != NULL) {
			floor->add_leader(this);
			// added following on 300399 so that pictures with pictures with pictures can be flipped properly
			if (!temporarily) { // conditional moved here on 230101
				Sprites *remaining = followers;
				while (remaining != NULL) {
					Sprite *sprite = remaining->first();
					if (sprite->pointer_to_followers() != NULL) {
						floor->recursively_add_leader(sprite);
					};
					remaining = remaining->rest();
				};
			};
		};
		if (followers == NULL && followers_on_other_side != NULL) { 
			// rewritten on 201200 so this happens whether temporarily is TRUE or FALSE
			floor->remove_leader(this);
		};
	};
	if (change_size) {
//		city_coordinate saved_width = width;
//		city_coordinate saved_height = height;
//		if (temporarily) { 
//			set_size(width_before_flip,height_before_flip); // new on 060201 so following works with better numbers
//		};
		recursively_propagate_changes();
//		set_size(saved_width,saved_height);
	};
	// commented out on 221299
//	if (remote != NULL && !temporarily) { 
	// added  && !temporarily since for example dumping shouldn't change sensors (e.g. CHILDREN_REMOTE)
//		remote->followers_changed();
//	};
};

const int notebooks_remote_count = 19; // followers and ancestors added on 050499
const int page_number_of_first_subnotebook = notebooks_remote_count*2+1;

Notebook *Picture::new_remote_notebook(Background *floor, boolean add_pages_now, boolean tell_remote) { 
	// tell_remote new on 311000 since when subnotebook of container it shouldn't tell the remote
#if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {
      log("Making remote control notebook for debug target.");
   };
	millisecond start = timeGetTime();
#endif
	Picture *notebook_of;
	if (controlee != NULL) { // remote looks flipped -- new on 220499
		notebook_of = controlee;
	} else {
		notebook_of = this;
	};
	Notebook *notebook = new Notebook(0,0,0,copy_ascii_string("rem"),0,0,NULL,NULL,notebook_of); 
	// beginning 210802 the caller copies the file name
	notebook->set_built_in_notebook(LOCAL_REMOTES_NOTEBOOK); // new on 050201
//   notebook->set_picture(this);  // this was in add_pages_to_notebook
   if (add_pages_now) {
      notebook_of->add_pages_to_notebook(notebook,floor);
   };
	if (tell_remote && remote != NULL) {
		remote->remote_notebook_added(notebook);
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 91100) {
		tt_error_file() << (timeGetTime()-start) << "ms to create remote notebook of ";
		print(tt_error_file());
		tt_error_file() << endl;
	};
#endif
   return(notebook);
};

void Picture::add_pages_to_notebook(Notebook *notebook, Background *floor) {
	int page = notebook->last_non_blank_page_number()+1; // was page_count
//   boolean need_to_dump = FALSE;
//#if TT_DEBUG_ON 
	if (tt_create_new_notebooks) {
		page = 0; // for re-creation
	};
//#endif
//	if (page > 3) { // this patches a problem where blank pages end up at the end
//		page -= (short int) 3; // seems to fix it right
//	};
	if (page < 3) { 
//      notebook->destroy();
//		notebook = new Notebook(0,0,0);
//#if TT_POST318
//		notebook->set_picture(this); // new on 241003 since can be wrong and then sensors end up with the wrong allegience
//#endif
		RemoteIdentifier identifiers[notebooks_remote_count]
			  = {X_REMOTE,Y_REMOTE,
				  X_SPEED_REMOTE,Y_SPEED_REMOTE,
				  WIDTH_REMOTE,HEIGHT_REMOTE,
				  PARAMETER_REMOTE,
				  HIT_OR_MISS_REMOTE,
				  COLLIDEE_REMOTE,
				  X_HIT_OR_MISS_REMOTE,
				  Y_HIT_OR_MISS_REMOTE,
				  VISIBLE_REMOTE,
				  END_OF_ANIMATION_REMOTE,
				  GRASPED_REMOTE,
				  SELECTED_REMOTE,
				  RELEASED_REMOTE,
				  APPEARANCE_REMOTE,
				  CHILDREN_REMOTE,
				  ANCESTORS_REMOTE};
		// don't pass "this" to following since notebook should
		// just contain static versions of things
		// copying page provides the controlee	
		page = notebook->add_remotes(identifiers,0,16,NULL); // 17th is APPEARANCE_REMOTE
		// add APPEARANCE_REMOTE add at end specially
		notebook->add_page(page++,variable_width_text_pad(S(IDS_APPEARANCE_LONG_LABEL),text_pad_see_all));
		Picture *picture_copy = new Picture();
		picture_copy->set_background(floor);
		picture_copy->set_controlee(this);
		Cubby *cubby = new Cubby();
		cubby->initialize_hole(0,picture_copy);
		cubby->set_label(0,copy_string(S(IDS_APPEARANCE_SHORT_LABEL)));
		notebook->add_page(page++,cubby);
		notebook->add_remotes(identifiers,page-1,notebooks_remote_count,NULL); // 17th is APPEARANCE_REMOTE
//      need_to_dump = TRUE;
	};
//	Picture *copy_of_me_before_flip = (Picture *) notebook->return_picture_copy()->top_level_copy(); // top_level added 122099
	if (tt_create_subnotebooks_of_remote_notebook && page-1 == notebooks_remote_count*2) { // haven't added sub notebooks (if any)
		Sprites *remaining = unflipped_followers(); 
		// added copy_of_me_before_flip-> on 121199 and removed it on 041299 since
		// then notebook doesn't control the right thing.
	//	if (remaining != NULL) page_number_of_first_subnotebook = (short unsigned) (page+1);
		while (remaining != NULL) {
			if (remaining->first()->kind_of() == PICTURE) {
				Picture *subpicture = (Picture *) remaining->first();
				notebook->add_page(page++,subpicture->shallow_copy()); // left side
				Notebook *subnotebook = subpicture->new_remote_notebook(NULL,TRUE,TRUE); 
				// last arg was FALSE prior to 211200 but then subnotebooks can point to destroyed picture
//				subnotebook->destroy(subpicture); 
				// new on 271000 since shouldn't have extra ref count ... - extra arg to new_remote_notebook deals with this better now
//				subpicture->just_set_remote_notebook(subnotebook); // new on 271000 - since needs to know about it (e.g. ref counts)
				subnotebook->go_to_page(1);
				notebook->add_page(page++,subnotebook);
				if (subpictures_with_notebooks == NULL) {
					subpictures_with_notebooks = new Sprites(subpicture);
				} else {
					subpictures_with_notebooks->insert_at_end(subpicture);
				};
			};
			remaining = remaining->rest();
		};
		// commented out the following on 090200 since dropping the remote looks on a blank picture works better
//		notebook->add_page(page++,variable_width_text_pad(S(IDS_COPY_OF_ME,"Copy\rof me"))); // new on 251099 
//		boolean was_flipped = is_flipped();
//		if (was_flipped) {
//			unflip(TRUE,FALSE);
//		};
		// changed this on 221299 to just copy current picture (even if flipped)
		// much too hard to maintain the copy just before flip
//		boolean saved_tt_dont_connect_things_while_copying = tt_dont_connect_things_while_copying;
//		tt_dont_connect_things_while_copying = TRUE; // new on 221299
//		notebook->add_page(page++,notebook->pointer_to_picture()->top_level_copy()); //copy_of_me_before_flip);
//		tt_dont_connect_things_while_copying = saved_tt_dont_connect_things_while_copying;
//		if (was_flipped) {
//			flip(TRUE,FALSE);
//		};
	};
	notebook->go_to_page(page++); // final blank page
	if (remote == NULL) {
		remote = new Remote(this);
	};
	//if (need_to_dump) {
	//	// this seems very wasteful so was commented out on 150703
	//	notebook->dump();
	//};
//	notebook->save_size();
//	notebook->move_to(llx+tile_width,lly+tile_height);
//	notebook->set_width_maintain_proportionality(tile_width*3);
// following handled better now by notebook::selected
};

void Picture::add_unflipped_follower(Sprite *follower, AddFollowerReason reason) {
	if (follower == NULL) return; // new on 170803 for robustness
   boolean orginally_flipped = is_flipped();
   if (orginally_flipped) {
      //city_coordinate width_saved = width;
      //city_coordinate height_saved = height;
      //set_size(width_before_flip,height_before_flip);
      unflip(TRUE,FALSE);
      add_follower(follower,TRUE,INSERT_AT_END,TRUE,reason); // args added on 101202 so leader_offsets aren't clobbered
      //set_size(width_saved,height_saved);
      flip(TRUE,FALSE);
   } else {
      add_follower(follower,TRUE,INSERT_AT_END,TRUE,reason); // args added on 101202 so leader_offsets aren't clobbered
//      follower->become_part_of_picture(); // new on 190802
   };
};

void Picture::add_follower(Sprite *follower, boolean completely_inside, InsertionOrdering insertion_ordering, 
									boolean dont_record_new_offsets,
									AddFollowerReason reason) { // new arg on 210104
	if (follower == NULL) return; // new on 170803 for robustness
	//	if (indirection != NULL && !is_flipped()) { // this was commented out prior to 040100
//		indirection->add_follower(follower,insertion_ordering);
//		if (remote != NULL) {
//			remote->followers_changed();
//		};
//		return;
//	};
   // TRUE was set_completely_inside_leader
   // but then clipping gets turned off
   // this does mean picture is clipped before mouse smashes it
//#if TT_DEBUG_ON
//	if (leader != NULL && follower->part_of(leader)) {
//		log("debug this");
//	};
//#endif
	// commented out on 181200
//	if (controlee != NULL && is_flipped() && pointer_to_followers() == NULL) { 
	// then controlee's is keeping the "thruth" - new on 161200
//		controlee->add_follower(follower,TRUE,insertion_ordering);
//		return;
//	};
	Sprite::add_follower(follower,completely_inside,insertion_ordering,dont_record_new_offsets,reason); 
	// completely_inside was TRUE prior to 200802 but before Bammer smashes might not be so good
	if (remote != NULL && follower->kind_of() == PICTURE && tt_create_subnotebooks_of_remote_notebook && // third arg added 090100
       remote_notebook != NULL && !is_flipped()) {
		// used to try to keep remote notebook up-to-date but
		// its no longer visible when flipped so easier to reset
		remote_notebook->destroy(this);
		remote_notebook = NULL;
		if (subpictures_with_notebooks != NULL) {
			subpictures_with_notebooks->remove_all();
			delete subpictures_with_notebooks;
			subpictures_with_notebooks = NULL;
      };
   };
	if (reason != LOADING_FOLLOWER) { // condition new on 210104
		if (remote != NULL && !is_flipped()) { // !is_flipped() added 221299
			remote->followers_changed();
		};
		follower->leader_changed();
	};
   if (!is_flipped() && completely_inside) { 
		// moved here on 160802 -- added completely_inside on 091202 since is !completely_inside when mouse adds temporarily 
		// (and indirections shouldn't change see_all)
      follower->become_part_of_picture();
   };
//	set_frame_flipped_followers_changed_last();
//   tt_global_picture->follower_added(this,follower);
//		if (subpictures_with_notebooks == NULL) {
//			subpictures_with_notebooks = new Sprites(follower);
//		} else {
//			subpictures_with_notebooks->insert_at_end(follower);
//		};
};

/* -- commented out on 151200 for new scheme for maintaining copies of flipped followers for remote looks
			boolean controlee_flipped = controlee->is_flipped(); // should be false...
			if (!controlee_flipped) controlee->flip(FALSE,FALSE);
			Sprite *follower_copy = follower->top_level_copy(); // top_level_ new on 211299
			if (follower->kind_of() == CUBBY) {
				Sprites *remaining = followers;
				while (remaining != NULL) {
					if (remaining->first()->kind_of() == ROBOT) {
						Robot *robot = (Robot *) remaining->first();
						if (robot->pointer_to_working_cubby() == follower) {
							Robot *robot_clone = (Robot *) controlee->flipped_followers()->find_copy(robot);
							if (robot_clone != NULL && robot_clone->pointer_to_working_cubby() == NULL) {
	//							robot_clone->suspend();
	//							robot_clone->try_clause((Cubby *) follower_copy);
								robot_clone->associate_with_box((Cubby *) follower_copy);
								break;
							};
						};
					};
					remaining = remaining->rest();
				};
			};
			add_flipped_follower(follower_copy,NULL,controlee);
			if (controlee->showing_on_a_screen()) { // new on 261000
				controlee->place_new_item(follower_copy);
			};
			if (!controlee_flipped) controlee->unflip(FALSE,FALSE);
		};
	*/


boolean Picture::remove_follower(Sprite *follower, boolean warn_if_not_follower) {
//	if (indirection != NULL && !is_flipped()) { // added on 040100
//		indirection->remove_follower(follower,warn_if_not_follower);
//		if (remote != NULL) {
//			remote->followers_changed();
//		};
//		return;
//	};
	if (controlee != NULL && is_flipped() && pointer_to_followers() == NULL) { 
		// then controlee's is keeping the "thruth" - new on 161200
		return(controlee->remove_follower(follower,warn_if_not_follower));
	};
//	Sprite::remove_follower(follower,warn_if_not_follower);
	// rewritten on 191202 to try followers_on_other_side if not there (thanks to Leonel)
	boolean removed_from_other_side = FALSE;
	if (!Sprite::remove_follower(follower,FALSE)) {
		if (followers_on_other_side != NULL) {
			boolean found;
			followers_on_other_side = followers_on_other_side->remove(follower,&found);
			if (!found) { // not anywhere
				if (warn_if_not_follower) {
					say_error(IDS_PART_OF_SOMETHING_AND_NOT);
				};
				return(FALSE);
			};
			removed_from_other_side = TRUE;
		} else {
			if (warn_if_not_follower) {
				say_error(IDS_PART_OF_SOMETHING_AND_NOT);
			};
			return(FALSE);
		};
	};
   if (prepared_for_deletion()) return(TRUE); // called in process of destroying follower
	if (follower->kind_of() == PICTURE) {
		Picture *picture_follower = (Picture *) follower;
		picture_follower->set_subpicture(FALSE); // not anymore -- does this apply regardless of whether we are on the front or back?
		if (is_flipped()) { // added on 180299 to restore controlee when removed
			picture_follower->now_on_back_of(NULL);
		} else {
			picture_follower->save_size(); // preserve geometry -- was unconditional prior to 090100
		};
	};
	if (remote != NULL) {
      if (remote_notebook != NULL && !is_flipped() && !flip_going_on_flag) { 
			// added !flip_going_on_flag on 261000 since shouldn't get a new notebook just due to flip
         // force notebook to be recomputed since may be obsolete
			remote_notebook->just_set_picture(NULL); 
			// added on 110699 since giving it up - otherwise ref count goes negative since notebooks still has a picture
			if (remote_notebook->showing_on_a_screen()) { // added 020899 since if visible shouldn't destroy it
				remote_notebook->set_visible(FALSE);
			} else {
//				remote_notebook->destroy();
				// replaced above with following on 110699 to avoid a dangling reference from remote
				remote->remote_notebook_deleted(remote_notebook);
			};
		   remote_notebook = NULL;
		   if (subpictures_with_notebooks != NULL) {
			   subpictures_with_notebooks->remove_all();
			   delete subpictures_with_notebooks;
			   subpictures_with_notebooks = NULL;
		   };
      };
//		subpictures_with_notebooks 
//			= subpictures_with_notebooks->remove(follower);
	};
//   tt_global_picture->follower_removed(this,follower);
	if (is_flipped() && !removed_from_other_side) {
		millisecond duration = default_duration();
		if (duration > 0) follower->animate_to_good_size(duration,tt_log_version_number >= 64?tt_toolbox:NULL);
		if (follower->kind_of() == ROBOT) {
//         Sprite *attached_cubby = ((Robot *) follower)->pointer_to_working_cubby();
			((Robot *) follower)->set_working_cubby(NULL);
			// made the following inoperative on 210499
//         if (attached_cubby != NULL && // second part added 1/5/99
//				 !attached_cubby->inside_vacuum() &&
//				 tt_log_version_number < 15) return; // don't re-number
//         attached_cubby->set_priority(follower->priority());
		} else if (follower->kind_of() == CUBBY) {
			// find robot who was going to use it
			Sprites *remaining = followers;
			while (remaining != NULL) {
				Sprite *sprite = remaining->first();
				if (sprite->kind_of() == ROBOT &&
					 ((Robot *) sprite)->pointer_to_working_cubby() == follower) {
					((Robot *) sprite)->set_working_cubby(NULL);
//					if (tt_log_version_number > 14) {
						break;
//					} else return;
				};
				remaining = remaining->rest();
			};
      };
//      if (tt_log_version_number > 7) {
//         int location = follower->priority();
//         Sprites *remaining = followers;
//         while (remaining != NULL) {
//            Sprite *sprite = remaining->first();
//           if (sprite->priority() > location) { // fill the gap
//               sprite->set_priority(sprite->priority()-1);
//            };
//            remaining = remaining->rest();
//         };
		if (!follower->prepared_for_deletion()) { // or more precisely is being prepared for deletion
			// added Sept 2 '98
//			place_new_item();
			set_placed_new_items(FALSE); // to force re-compute next time displayed
		};
		if (controlee != NULL) {
			// new on 210499 so remote looks when flipped can have followers removed
			// and the controlee is kept in synch
			Sprite *clone = controlee->flipped_followers()->find_copy(follower);
			if (clone != NULL) {
				controlee->remove_flipped_follower(clone);
				clone->destroy(); // new on 261000
			};
		};
	};
	if (remote != NULL && (!is_flipped() || removed_from_other_side)) { // !is_flipped() added 221299
		remote->followers_changed();
	};
	follower->leader_changed();
//	set_frame_flipped_followers_changed_last();
	return(TRUE);
};

void Picture::leader_changed() {
 	if (remote != NULL) {
		remote->leader_changed();
	};
	Sprites *remaining = followers;
	while (remaining != NULL) {
		remaining->first()->leader_changed();
		remaining = remaining->rest();
	};
};

Picture *Picture::which_sub_picture_on_page(int page_number) {
	if (subpictures_with_notebooks != NULL) {
		return((Picture *) subpictures_with_notebooks
						->nth((page_number-page_number_of_first_subnotebook)/2));
	};
	return(NULL);
};

ReleaseStatus Picture::item_released_on_top(Sprite *item, Sprite *by,
														  SpritePointer &recipient, int level) {
	if (appearance_code() == SCALE_SPRITE && !is_flipped()) { // not flipped added on 130201
//		if (infinite_stack_flag && item->fine_kind_of() == SCALE_PICTURE) {
//			return(RELEASED_ON_ITS_STACK);
//		};
		receive_item(item,by,0,this,item); // to get Marty to give advice
		return(RELEASE_IGNORED);
	};
	// too often the following does unexpected things and happens accidentally
	// Among them if release is rejected a copy is left behind
	if (infinite_stack()) {
		if (!tt_copy_top_of_stack && !by->still_running()) {
         by->action_aborted();
			return(RELEASE_IGNORED);
		};
		switch (item->kind_of()) {
			case PICTURE:
			case INTEGER:
			case TEXT:
			  recipient = copy_top_of_stack(by);
			  return(recipient->item_released_on_top(item,by,recipient,level));
		};
	};
	if (level > tt_max_containment_levels) {
		say(IDS_IGNORED_DROP_TOO_MANY_LEVELS,4);
		by->abort_action_if_robot(-1); // why not combine these two lines? -asked 130205
		return(RELEASE_IGNORED);
	};
	if (is_flipped()) { // || (controlee != NULL && controlee->is_flipped())) { // second disjunct added 151200
		// new policy on 100200 that anything can be put on the back of pictures
		if (!toolbox_tool(item) && item != pointer_to_remote_notebook()) { 
			// except Tooly and friends and the picture's remote notebook -- 
			// does this allow tools on the back of pictures when running in Puzzle mode??
//		switch (item->kind_of()) {
//			case PICTURE:
//				// ok to add flipped pictures to flipped pictures
//				// commented out the following on 120499 - pictures on the back are comments just like text pads
////				if (!((Picture *) item)->is_flipped()) break;
//			case CUBBY:
//         case TEXT: // for comments
//			case BIRD: // for access (e.g. colliding-with-who remote)
//			// make sure that all contained remotes are for this picture
//			case ROBOT: //  need to make room for it if following FALSE
////			if (remote_robots != NULL && remote_robots[0] == NULL) {
////				if (!is_flipped()) {
////					return(RELEASE_IGNORED);
////				};				
				if (receive_item(item,by,default_duration(),this,item)) {
					boolean training = (by->kind_of() == ROBOT_IN_TRAINING);
               if (training) {
                  set_available_for_indexing(FALSE); // so a path to it is used
               };
				   record_event(RELEASE_ITEM_ON,by,floor,this,TRUE);
					if (training) {
                  set_available_for_indexing(TRUE);
               };
               item->now_released_on(this,by);
               item->now_on_back_of(this);  // receive_item already did this, right??
				   user_did(PICTURE,PICTURE_CUBBY_OR_ROBOT_ADDED,by);
				   return(RELEASED_ON_PICTURE);
            } else {
               return(RELEASE_IGNORED);
            };
		};
	} else {
		switch (item->kind_of()) {
			case PICTURE: // composing movies
				{
				Picture *picture = (Picture *) item;
				if (picture->appearance_code() == SCALE_SPRITE) {
					by->abort_action_if_robot(IDS_SCALES_CANT_ADD_TO_PICTURE,
													  MOUSE_WITH_HAMMER,
													  !told_user(TOLD_SCALES_ON_PICTURES));
					user_told(TOLD_SCALES_ON_PICTURES);
					return(RELEASE_IGNORED);
				};
				if (picture->is_flipped() && (controlee == NULL || !controlee->is_flipped())) {
					// second condition added on 030300 so that you can drop flipped pictures on remote looks
					by->abort_action_if_robot(IDS_UNFLIPPED_PICTURES_NOT_ON_FLIPPED_PICTURES,
													  MOUSE_WITH_HAMMER,
													  !told_user(TOLD_FLIPPED_PICTURES_ON_PICTURES));
					user_told(TOLD_FLIPPED_PICTURES_ON_PICTURES);
					return(RELEASE_IGNORED);
				};
				if (receive_item(item,by,default_duration(),this,item)) {
				   user_did(PICTURE,PICTURE_ADDED_PICTURE,by);
// commented out on 9/9/97 since location of picture on picture must be controlled by remote controls
//               if (by->kind_of() == ROBOT_IN_TRAINING) {
//                   set_available_for_indexing(FALSE); // so a path to it is used
//               };
				   record_event(RELEASE_ITEM_ON,by,floor,this,TRUE);
               item->now_released_on(this,by);
				   return(RELEASED_ON_PICTURE);
				} else {
               return(RELEASE_IGNORED);
            };
            };
//			case CUBBY:
			case TEXT:
			case INTEGER: // others??
			case REMOTE_PICTURE:
				if (receive_item(item,by,default_duration(),this,item)) {
					if (indirection == NULL || indirection->kind_of() != TEXT) { 
						// condition new on 010305 since dropping things on text already 
						// recorded the event and shouldn't do the following
						boolean training = (by->kind_of() == ROBOT_IN_TRAINING);
						if (training) {
							set_available_for_indexing(FALSE); // so a path to it is used
						};
						record_event(RELEASE_ITEM_ON,by,floor,this,TRUE);
						if (training) {
							set_available_for_indexing(TRUE); // so a path to it is used
						};
					};
               item->now_released_on(this,by);
//				   user_did(PICTURE,PICTURE_ADDED_PICTURE,by);
				   return(RELEASED_ON_PICTURE);
            } else {
               return(RELEASE_IGNORED);
            };
		};	
	};
/* following may speed things up a tad but it can be ugly and confusing
	if (leader == NULL && // free to change
		 (code != SYNTHETIC_SPRITE || // should really be never_cycles()
		  followers != NULL &&
		  controlee == NULL && indirection == NULL)) {
		// swap priorities because pictures in general are DIRTY
		// and all the stuff on top is redrawn each frame then
		long item_priority = item->priority();
		if (item_priority < priority()) {
			item->set_priority(priority());
			set_priority(item_priority);
		};
	};
*/
	by->abort_action_if_robot(IDS_PICTURE_WONT_ACCEPT_SOMETHING);
	return(RELEASE_IGNORED);
};

//		case PROGRAM_PAD:
//			if (((Notebook *) item)->pointer_to_picture() == this) {
//				Sprites *current_notebook = followers->member_of_type(PROGRAM_PAD);
//				TTRegion current_region;
//				current_notebook->first()->full_region(current_region);
//				TTRegion new_region;
//				item->full_region(new_region);
//				if (new_region.overlaps(current_region)) {
//					// returning a copy of the notebook
//					item->animate_size_and_location(llx+width/20,lly+height/20,
//															  current_region.width(),
//															  current_region.height(),
//															  1000,
//															  item,destroy_callback);
//					return(RELEASED_ON_PICTURE);
//				} else {
//					return(RELEASE_IGNORED);
//				};
//			};

void Picture::update_text_and_widths_of_controlee() {
   controlee->update_text_and_widths(leader != NULL);
};

void Picture::update_text_and_widths(boolean sizes_constrained) {
	if (controlee != NULL) {
		controlee->update_text_and_widths(sizes_constrained);
	} else if (indirection != NULL) {
		indirection->update_text_and_widths(sizes_constrained);
	}; // otherwise ignore it
};

boolean free_robot_callback(Sprite *r, void *arg) {
	return(r->kind_of() == ROBOT &&
			 ((Robot *) r)->pointer_to_working_cubby() == NULL);
};

boolean Picture::receive_item(Sprite *item, Sprite *by,
										millisecond duration, Sprite *original_recipient, Sprite *original_item) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Debug target receiving an item.");
	} else if (tt_debug_target == item->debug_counter) {
		log("Picture receiving the debug target.");
	};
	//if (by != NULL && by->kind_of() == ROBOT && blank) {
	//	string name = ((Robot *) by)->copy_string_name_from_user();
	//	if (name != NULL && strnicmp(name,"exit",4) == 0) {
	//		debug_this();
	//	};
	//};
#endif
	if (appearance_code() == SCALE_SPRITE && !is_flipped()) { // not flipped condition added on 130201
		by->completed();
		if (infinite_stack()) return(FALSE); // goofy case
		if (item->kind_of() == TEXT && !told_user(TOLD_SCALES_AND_TEXT)) {
			say(IDS_SCALE_TEXT_HELP1);
			say(IDS_SCALE_TEXT_HELP2);
			say(IDS_SCALE_HELP3);		
			user_told(TOLD_SCALES_AND_TEXT);
		} else if (item->kind_of() == INTEGER && !told_user(TOLD_SCALES_AND_NUMBERS)) {
			say(IDS_SCALE_NUMBER_HELP1);
			say(IDS_SCALE_NUMBER_HELP2);
			say(IDS_SCALE_HELP3);
			user_told(TOLD_SCALES_AND_NUMBERS);
		};
		by->abort_action_if_robot(IDS_SCALE_WONT_ACCEPT);
		return(FALSE);
	};
	if (controlee != NULL) { // added on 040299 to keep remote looks and picture in synch
		if (!blank && controlee->is_blank()) {
			become_blank();
		} else if (blank && !controlee->is_blank()) {
			become_non_blank();
		};
//		if (controlee->is_flipped()) { // on 151200 commented out && item->is_flipped()) { // new on 030300
			// following new on 161200
//			if (!flip_going_on_flag && followers != NULL) { // if remote looks AND maintaining a copy of flipped followers
//				controlee->add_follower(item->copy(),TRUE,INSERT_AT_END);
//			};
//		};
		// fix geometry - position not size so center of item is at the relatively same place
		// moved here on 121202 since needed for dropping boxes on robots when flipped as well (thanks Gordon)
		city_coordinate center_x, center_y;
		item->center_location(center_x,center_y);
		// use int64 for greater range and accuracy
		int64 item_center_x
			= ((int64) controlee->current_llx())+
						  ((center_x-current_llx())*controlee->current_width())/current_width()-item->current_width()/2;
		int64 item_center_y 
			= ((int64) controlee->current_lly())+
							((center_y-current_lly())*controlee->current_height())/current_height()-item->current_height()/2;
		item->move_to((city_coordinate) item_center_x, (city_coordinate) item_center_y);
		if (item == controlee || controlee->has_leader(item)) { 
			// new on 140301 since you might be dropping a picture on the back of its remote looks
			// moved here from first branch below on 230203 since can be dropping a picture on its parts sensor
			// following is new on 180603
			// changed on 120703 so Marty says this instead of just going in the log
			// but can't do this until IDS_CANT_BE_ON_THE_BACK_OF_ITSELF is in the language DLL...
			log("Couldn't put something on the back of its looks remote control since it would then have itself on its back. Use the object and not the remote looks sensor instead.",FALSE,TRUE);
//			by->abort_action(S(IDS_CANT_BE_ON_THE_BACK_OF_ITSELF,
			// "Couldn't put something on the back of its looks remote control since it would then have itself on its back. Use the object and not the remote looks sensor instead."));
			return(FALSE);
		};
		if (is_flipped()) { // && !controlee->is_flipped()) { // new on 161200
//			item->remove_from_floor(FALSE); // new on 171200
//			add_flipped_follower(item,NULL,controlee);
//			completed(action_status);
//			return(TRUE);
			boolean controlee_originally_flipped = controlee->is_flipped();
			if (!controlee_originally_flipped) {
				controlee->flip(TRUE,FALSE);
			};
			boolean result = controlee->receive_item(item,by,duration,original_recipient,original_item);
			if (!controlee_originally_flipped) {
				controlee->unflip(TRUE,FALSE); // first arg was FALSE prior to 210101
			};
			return(result);
		} else { // new on 171200
			boolean controlee_originally_flipped = controlee->is_flipped();
			if (controlee_originally_flipped) {
				controlee->unflip(FALSE,TRUE);
				duration = 0; // so it receives item instantaenously below
			};
			// added default_duration code below on 060504 in case controlee isn't on screen
			boolean result = controlee->receive_item(item,by,controlee->default_duration(duration),original_recipient,item);
			if (controlee_originally_flipped) {
				controlee->flip(TRUE,TRUE);
			};
			return(result);
		};
	};
   if (inside_vacuum()) { 
		// moved here on 050105 since can have controlee to controllee (in vacuum) to picture (not in vacuum) chains
		// e.g. Express in gene simulator
      by->abort_action(IDS_ROBOT_MISSING_RECIPIENT_ABORT);
      return(FALSE);
   };
//	boolean is_flipped() = (is_flipped() || (controlee != NULL && controlee->is_flipped())); // new on 030300
	if (!is_flipped()) {
		if (!is_blank()) { // condition new on 070301
			Sprite *forward_to = forwarding_to(FALSE);
			if (forward_to != NULL) {
				// size setting done properly by forwarding_to now
				// set size and location so for example mouse knows where to smash
	//			if (duration > 0) {
	//				forward_to->Sprite::set_size_and_location(width,height,llx,lly);
	//				forward_to->set_priority(priority());
	//			};
				if (forward_to == item) { // e.g. drop something on its remote controls for its looks
					by->abort_action(IDS_REMOTE_CANT_ACCEPT_THAT);
					return(FALSE);
				};
				set_clean_status(DIRTY);
	//			if (forward_to == controlee) {
	//				controlee->set_background(NULL); // was set by :display but need to fix the little lie here
	//			};
				boolean result = forward_to->receive_item(item,by,duration,this,item); 
            // prior to 081102 last arg above was original_recipient but then if a remote looks to an indirection 
				// is encountered the wrong guy is the (useful) original_recipient
				if (duration > 0 && controlee != NULL) {
					do_after(duration+mouse_to_run_out_time+mouse_to_smash_time,this,UPDATE_TEXT_AND_WIDTHS_OF_CONTROLEE_CALLBACK);
//				} else if (indirection != NULL && leader == NULL) {
//					// new on 060102
//					do_after(default_duration(mouse_to_run_out_time+mouse_to_smash_time),this,set_size_to_that_of_indirection_callback);
				};
				return(result);
			};
		} else if (indirection != NULL) {
			// new on 190503 to deal with dropping things on erased number or text pads (that happen to have been flipped)
			return(indirection->receive_item(item,by,duration,original_recipient,original_item));
		};
		switch (item->kind_of()) {
			case REMOTE_PICTURE:
				// can capture a snapshot by dropping picture sensor on a blank picture
				if (is_blank()) {
					Sprite *appearance = ((Remote_Picture *) item)->pointer_to_appearance();
					if (appearance != NULL) { // conditional new on 061000
						return(receive_item(appearance->copy(),by,duration,this,item));
					} else { // warn?? signal error?? or punt??
						by->completed();
						return(TRUE);
					};
				};
				break;
			case TEXT:
			case INTEGER:
				if (!is_blank()) {
					boolean original_show_all = item->show_all();
//					item_to_add = make_indirect_picture(item); // open-coded below on 130103
					item->set_show_all(original_show_all); // so when mouse saves item (for F11 undo) it is in the original state
					if (by != NULL && by->kind_of() == ROBOT && !showing_on_a_screen()) {
						// robot has no control over size so act like as if watched
						item->set_to_good_size(tt_toolbox);
					};
					boolean result = receive_item(make_indirect_picture(item),by,duration,this,item);
					item->set_show_all(FALSE);
					return(result);
				} else { // become it
//					item_to_add = item; -- commented out on 130103 and renamed below
					set_saved_by(by); // added on 130499 so that geometry is figured differently if a robot did this
					if (duration > 0) {  // was != prior to 140203 but a crash had a negative duration
						call_in_a_mouse(this,item,floor,by,FALSE,FALSE,
											// restored FALSE 160605 since picture indirects to it now -- 
											// ::ok_to_delete_bammed(by), // this arg was FALSE prior to 131000
											 duration,original_recipient,original_item);
					} else {
						set_to_future_value(item,original_recipient,by); // same action mouse would have caused
//						if (::ok_to_delete_bammed(by)) { // new on 131000 -- commented out on 160605
							item->destroy(by);
//						};
						by->completed();
					};
//					item_to_add = NULL; 
				};
				return(TRUE);
		};
	};
	switch (item->kind_of()) {
		case PICTURE:
			{
			Picture *picture_to_add = (Picture *) item;
			if (picture_to_add->appearance_code() == SCALE_SPRITE && !is_flipped()) { // not flipped condition added on 130201
				by->completed();
				return(TRUE); // really??
			};
			// commented out the following on 120499 so you can add pictures as comments
			// including flipped text which was turned into a picture
//			boolean both_flipped = (flipped && picture_to_add->is_flipped());
//			if (!both_flipped && (is_flipped() || picture_to_add->is_flipped())) {
//				// ok if both are flipped
//				by->abort_action(action_status,S(IDS_ABORT_PICTURE_FLIPPED_OVER),MOUSE_WITH_HAMMER);
//				return(FALSE);
//			};
			if (controlee != NULL && picture_to_add->contains(controlee)) {
				by->abort_action(IDS_REMOTE_CANT_ACCEPT_THAT);
				return(FALSE);
			};
			if (circular_forwarding(picture_to_add)) {
				by->abort_action(IDS_REMOTE_CANT_ACCEPT_THAT); // not quite the right message
				return(FALSE);
			};
			if (picture_to_add->to_be_smashed()) {
				by->abort_action(IDS_PICTURE_WAITING_FOR_MOUSE);
				return(FALSE);
			};			  
//			if (mouse != NULL) {
//				by->abort_action(action_status,S(IDS_PICTURE_UNDERNEATH_WAITING_FOR_MOUSE));
//				return;
//			};
			if (picture_to_add->remote_status() != NOT_REMOTE) {
				by->abort_action(IDS_PICTURE_DROPPED_IS_REMOTE);
				return(FALSE);
			};
			if (!is_flipped()) {
//				item_to_add = item; // commented out on 130103
				set_saved_by(by); // added on 130499 so that geometry is figured differently if a robot did this
				if (by->kind_of() != PROGRAMMER) { 
					// robots have no geometry control -- added 270199 -- e.g. picture::receive_item when flipped on flipped ...
					item->move_to(center_x()-item->current_width()/2,center_y()-item->current_height()/2);
				};
				if (duration > 0 && floor != NULL) { // default_duration() != 0) { 
					// was != prior to 140203 but a crash had a negative duration
					// floor test new on 060504 for robustness (Gordon had a crash via remote looks here)
					call_in_a_mouse(this,picture_to_add,floor,by,FALSE,FALSE,duration,original_recipient,original_item);
				} else {
					set_to_future_value(picture_to_add,original_recipient,by); // same action mouse would have caused
					action_completed(); // by and saved_by same so this is safe
				};
				return(TRUE);
			} else { // if (tt_log_version_number > 7) { // else fall thru to below
            Notebook *others_notebook = picture_to_add->pointer_to_remote_notebook();
            if (others_notebook != NULL) { // remove old notebook -- need to flip twice to get it back
				   others_notebook->remove_from_floor();
			   };
			};
         };
//		case ROBOT: // doesn't care what's underneath but otherwise same as above
//		case TEXT: // for "comments"
//		case BIRD: // for access via sensors
//		case CUBBY: {
		default: { // new policy on 100200 that you can put anything on the back of pictures
// add INTEGER: and REMOTE_PICTURE: later
			if (!is_flipped()) {		
				by->abort_action(IDS_ABORT_PICTURE_NOT_FLIPPED_OVER,MOUSE_WITH_HAMMER);
				return(FALSE);
			};
//			item->set_on_flip_side_of_picture(TRUE);
			// so it is not showing_on_a_screen()
//			item->decrement_screen_ref_count();
//			item->save_size();
// huh??
//			if (item->kind_of() == PICTURE) { 
				// cubbies and robots don't combine on flip side
// following is only
			Sprite *underneath = NULL;
			ReleaseStatus status = RELEASE_IGNORED;
			TTRegion region,item_region;
			item->full_region(item_region);
//			item->now_on_back_of(this); // moved to after remove_from_floor on 240101
			if (item->kind_of() == CUBBY) {
				if (by->kind_of() == ROBOT || temporarily_flipped) { 
					// second condition new on 070305 so can drop a box on the back of the remote looks 
					// if this wasn't flipped originally
					// new on 011204 since geometry doesn't matter if a robot is dropping a box on a robot
					Sprites *remaining = pointer_to_followers();
					while (remaining != NULL) {
						if (free_robot_callback(remaining->first(),NULL)) {
							underneath = remaining->first();
							break;
						};
						remaining = remaining->rest();
					};
				} else {
					underneath = next_to_collide_with(item_region,item,NULL,TRUE,free_robot_callback);
				};
			};
			if (underneath != NULL) {
				status = underneath->item_released_on_top(item,by,underneath,1);
			};
			switch (status) {
				case RELEASED_ROBOT_ON_CUBBY: // no longer interesting...
				case RELEASED_CUBBY_ON_ROBOT:
				case RELEASE_IGNORED:
//               if (tt_log_version_number <= 7) {
//                  place_new_item_old_version(item);
//               } else {
                  place_new_item(item,(status == RELEASED_CUBBY_ON_ROBOT)?underneath:NULL);
//               };
					item->full_region(item_region);
					full_region(region);
			};
//         if (tt_log_version_number <= 7) {
//			   // shrink region by 4% of all sides to look nicer
//			   region.min_x += width/25;
//			   region.max_x -= width/25;
//			   region.min_y += height/25;
//			   region.max_y -= height/25;
//         };
			city_coordinate delta_x,delta_y;
			Background *item_background = item->pointer_to_background();
			if (item_background != NULL && item_background->pointer_to_picture() != this) { 
				// conditional new on 240101 (but it didn't solve the problem I had)
				item->remove_from_floor(FALSE,TRUE); 
				// FALSE was TRUE prior to 030899 but then robots on flipside sometimes generated spurious warnings
			};
			item->now_on_back_of(this);
         city_coordinate new_llx, new_lly;
			switch (status) {
				case RELEASED_ROBOT_ON_CUBBY: // obsolete??
//					if (floor != NULL) {
//						floor->remove_item(item);
//					};
					underneath->lower_left_corner(new_llx,new_lly);
					item->move_to(new_llx,new_lly);
					 // fix if moved too far
					item->full_region(item_region);
					item_region.move_inside(region,delta_x,delta_y);
					item->move_by(delta_x,delta_y);
					// put cubby on top
					remove_follower(underneath);
//					item->absorb_followers();
					add_follower(item); 
					add_follower(underneath);
//					((Robot *) item)->suspend();
//					((Robot *) item)->try_clause((Cubby *) underneath);
					((Robot *) item)->associate_with_box((Cubby *) underneath);
					break;
				case RELEASED_CUBBY_ON_ROBOT:
//					if (floor != NULL) {
//						floor->remove_item(item);
//					};
//               if (tt_log_version_number <= 7) {
//					   underneath->lower_left_corner(new_llx,new_lly);
//					   item->move_to(new_llx,new_lly);
//					   item->full_region(item_region);
//					   item_region.move_inside(region,delta_x,delta_y);
//					   item->move_by(delta_x,delta_y);
//               };
					// want to have robot directly under (before) cubby
//					remove_follower(underneath);
//					add_follower(underneath,floor);
//					((Robot *) underneath)->suspend();
//					((Robot *) underneath)->try_clause((Cubby *) item);
					((Robot *) underneath)->associate_with_box((Cubby *) item);
					add_follower(item); // moved here on 210499 so remote looks can mirror it properly
					break;
				case RELEASE_IGNORED:
//					if (floor != NULL) {
//						floor->remove_item(item,FALSE);
//					};
					if (item != this) { // condition added on 140301 since you might be dropping the controlee on its remote looks
						item_region.move_inside(region,delta_x,delta_y);
						item->move_by(delta_x,delta_y);
//						item->absorb_followers();
						add_follower(item);
					} else {
						return(FALSE); // new on 140301
					};
					break;
			};
//			if (robot_followers != NULL) {
//				item->set_followers(robot_followers);
//			};
			by->completed();
			return(TRUE);
			};
	};
   return(TRUE);
};

//void Picture::set_size_to_that_of_indirection() {
//	if (indirection != NULL) {
//		set_size(indirection->current_width(),indirection->current_height());
//	};
//};

boolean Picture::circular_forwarding(Picture *other) {
	// to prevent loops before they occur
	Sprite *c = other->pointer_to_controlee();
	if (c == NULL || c->kind_of() != PICTURE) return(FALSE);
	if (this == c) return(TRUE);
	Sprite *i = ((Picture *) c)->pointer_to_indirection();
	if (this == i) return(TRUE);
	// worth doing more tests
	return(FALSE);
};

void Picture::place_new_item(Sprite *item, Sprite *underneath) {
	// commented out the following on 261000 since it just created the copy and 
	// set its location and size but didn't add it the controlee
	// :add_follower does add a copy like this...
//	if (controlee != NULL && item != NULL && underneath == NULL) { // new on 030300
//		controlee->place_new_item(item->copy(),underneath); // copy() added 060300
//		return; // commented out now that copy is placed on controlee
//	};
   // new scheme does automatically tiling
	set_placed_new_items(TRUE);
	if (item != NULL) {
		item->finish_instantly(); // new on 171200
	};
   int new_count = 0;
   Sprites *remaining;
	if (controlee == NULL) {
		remaining = flipped_followers(); 
	} else {
		remaining = pointer_to_followers();
	};
   while (remaining != NULL) {
		Sprite *follower = remaining->first();
      if (follower->kind_of() == ROBOT) {
         Robot *robot = (Robot *) remaining->first();
         if (robot->pointer_to_working_cubby() == NULL) {
            new_count++;
         };  // otherwise will count the cubby which shares the grid space with the robot
      } else {
         new_count++;
      };
      remaining = remaining->rest();
   };
   if (item != NULL && underneath == NULL) new_count++; // make room for new guy
   int square_root = 1; // poor man's integer square root
   while (square_root*square_root < new_count) square_root++;
   city_coordinate full_cell_width = width/square_root;
	city_coordinate spacing_width = full_cell_width/(4*square_root); 
	// new on 191200 so when just one thing has big borders so easier to pick up picture   
	city_coordinate height_available;
	if (tt_log_version_number >= 46) {
		height_available = height-height/label_fraction; // for the label
	} else {
		height_available = height;
	};
	city_coordinate full_cell_height = height_available/square_root;
	city_coordinate spacing_height = full_cell_height/(4*square_root); // new on 191200
	// on 021202 double the spacing if square_root is exact since can be hard to (and height too)
	if (square_root*square_root == new_count) {
		spacing_width *= 2;
		spacing_height *= 2;
	};
	city_coordinate cell_width = full_cell_width-spacing_width; // leave some room between cells -- was 90% prior to 191200 
	//if (cell_width <= 0) { // new on 021202
	//	spacing_width += cell_width-50;
	//	cell_width = 50;
	//};
   city_coordinate cell_height = full_cell_height-spacing_height;
	//if (cell_height <= 0) { // new on 021202
	//	spacing_height += cell_height-50;
	//	cell_height = 50;
	//};
   city_coordinate cell_llx_start = llx+spacing_width/2; // full_cell_width/10; // was /20 prior to 191200
//   city_coordinate cell_llx = cell_llx_start;
   city_coordinate cell_lly_start = lly+height_available-(full_cell_height-spacing_height/2); // full_cell_height/10); // was /20 prior to 191200
//   Sprites *sorted_flipped_followers = sort_flipped_followers();
//   remaining = sorted_flipped_followers;
   if (controlee == NULL) {
		remaining = flipped_followers(); 
	} else {
		remaining = pointer_to_followers();
	};
   city_coordinate item_width, item_height;
   int location = 0;
   Sprites *robots = NULL;
	while (remaining != NULL) {
      Sprite *follower = remaining->first();
      Robot *robot = NULL;
      if (follower->kind_of() == CUBBY) {
         robot = robot_attached_to(follower);
      };
//      boolean attached_cubby = (follower->kind_of() == CUBBY &&
//                                remaining->rest() != NULL &&
//          							  remaining->rest()->first()->kind_of() == ROBOT);
//      int location; // using priority for this
//      if (attached_cubby) {
//         location =  remaining->rest()->first()->priority();
//      } else {
//         location = follower->priority();
//      };
      if (robot != NULL) {
//          item_width = cell_width/2;
//          item_height = cell_height/2;
          robots = new Sprites(robot,robots);
      } else {
         int column = location%square_root;
         int row = location/square_root;
         if (follower->kind_of() == ROBOT) { // leave room for thought bubble, etc.
            item_width = cell_width/2;
            item_height = cell_height/3;
         } else {
            item_width = cell_width;
            item_height = cell_height;
         };
         follower->set_size_and_location(item_width,item_height,
                                         cell_llx_start+column*full_cell_width+(cell_width-item_width)/2,
                                         cell_lly_start-row*full_cell_height+(cell_height-item_height)/2);
         location++;
      };
      remaining = remaining->rest();
   };
   if (item != NULL) {
      if (underneath != NULL) {
         item->set_size_and_location(cell_width/4,cell_height/4,
                                     underneath->current_llx(),
                                     underneath->current_lly());
//         item->set_priority(underneath->priority()-1);
      } else {
         if (item->kind_of() == ROBOT) {
            item_width = cell_width/2;
            item_height = cell_height/3;
         } else {
            item_width = cell_width;
            item_height = cell_height;
         };
         int position = new_count-1; // zero indexing
         int column = position%square_root;
         int row = position/square_root;
         item->set_size_and_location(item_width,item_height,
                                     cell_llx_start+column*full_cell_width+(cell_width-item_width)/2,
                                     cell_lly_start-row*full_cell_height+(cell_height-item_height)/2);
//         item->set_priority(position);
      };
   };
//   if (sorted_flipped_followers != NULL) {
//      sorted_flipped_followers->remove_all();
//      delete sorted_flipped_followers;
//   };
   if (robots != NULL) {
      remaining = robots;
      while (remaining != NULL) {
         Robot *robot = (Robot *) (remaining->first());
         Cubby *cubby = robot->pointer_to_working_cubby();
         cubby->set_size_and_location(cell_width/4,cell_height/4,
                                      robot->current_llx(),robot->current_lly());
         remaining->set_first(NULL);
         remaining = remaining->rest();
      };
      delete robots;
   };
};

Robot *Picture::robot_attached_to(Sprite *cubby) {
	if (controlee != NULL) { // new on 171200
		return(controlee->robot_attached_to(cubby));
	};
   Sprites *remaining = flipped_followers();
   while (remaining != NULL) {
      if (remaining->first()->kind_of() == ROBOT) {
         Robot *robot = (Robot *) (remaining->first());
         if (robot->pointer_to_working_cubby() == cubby) return(robot);
      };
      remaining = remaining->rest();
   };
   return(NULL);
};

ReleaseStatus Picture::add_to_side(boolean right_side, Sprite *other,
											  Sprite *by, 
											  millisecond duration, Sprite *original_recipient, Sprite *original_item) {
	Sprite *forward_to = forwarding_to(FALSE);
	if (forward_to != NULL) {
		return(forward_to->add_to_side(right_side,other,by,duration,original_recipient,original_item));
	} else {
		return(Sprite::add_to_side(right_side,other,by,duration,original_recipient,original_item));
	};
};

void Picture::ignore_collision() {
	if (remote != NULL) { 
		remote->ignore_collision();
	};
};

void Picture::set_size_and_location(city_coordinate new_width,
												city_coordinate new_height,
												city_coordinate new_llx,
												city_coordinate new_lly,
												ChangeReason reason) {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) { // && reason != UPDATING && reason != JUST_MOVED) {
		log(_T("Debug picture target size and position about to be set."));
		tt_error_file() << "new_width=" << new_width
							 << "; new_height=" << new_height
							 << "; llx=" << new_llx
							 << "; lly=" << new_lly << endl;
	};
#endif
	Sprite::set_size_and_location(new_width,new_height,new_llx,new_lly,reason);
	if (reason != DUE_TO_NON_ZERO_SPEED) { 
		// new on 040302 so that if moved by hand or position sensors that this error is eliminated 
		// -- should error be kept when x_remote used and is non integer??
		llx_accumulated_error = 0;
		lly_accumulated_error = 0;
	};
	if (reason == FOLLOWING_INDIRECTIONS_CHANGE) {
//			set_last_size_change_due_to_indirection(TRUE);
		return;
	};
	if (reason != LEADER_CHANGED && leader != NULL && leader->decoration_remote()) {
		// new on 170106 -- easiest way to deal with this is to remove this and then add it back
		leader->follower_used_in_decoration_changed(this);
	};
//		if (reason != JUST_MOVED && reason != UPDATING) set_last_size_change_due_to_indirection(FALSE);
	if (reason == FOLLOWING_CONTROLEES_CHANGE || reason == UPDATING || reason == JUST_MOVED) return;
	if (tt_log_version_number >= 67 && 
			(reason == EXPANDER_CHANGED_IT || reason == VACUUM_CHANGED_IT || // reason == LEADER_CHANGED ||
			reason == ANIMATION_COMPLETED || reason == TO_FIT_INSIDE ||
			reason == SETTING_TO_GOOD_SIZE || reason == ANIMATION_STARTING ||
			reason == VACUUM_RESTORING || reason == TO_FIT_ON_A_NEST || reason == ANIMATING)) { 
				// new on 110105 so this doesn't trigger anything
		old_width = 0; // so need to recompute any width or height sensors
		old_height = 0;
	};
	if (indirection != NULL) {
		// used to be Sprite::set_size_and_location -- why??
		indirection->set_size_and_location(new_width,new_height,new_llx,new_lly,reason);
	};
		// commented out the following on 090102 since nothing else happens here
//		if (reason == TO_FIT_INSIDE || reason == LEADER_CHANGED || reason == UPDATE_TO_FIT_IN_CUBBY || 
		//  reason == TEMPORARY_FOR_DUMPING) return; 
		// 4th disjunct added 051100
		// moved here from before indirection check on 290699 so that indirection is updated in these case
		// following added on 010299 so sizes say in synch better
		// commented out on 121600 for new policy that changing the looks sensor in this way has no effect on the size of the picture
//		if (controlee != NULL && leader == NULL && !controlee->size_constrained() 
//			 && !inside_vacuum() // added on 070499 so that vacuuming up remote doesn't change size of controlee
//			 && floor != NULL && floor->pointer_to_picture() == NULL 
		// added on 120499 so size if on flipside doesn't control anything
//			 && !is_flipped() && !flip_going_on_flag && !ignore_controlee_when_size_changes_flag) { // added 210499
//			controlee->set_size(new_width,new_height,reason);
//		};
//	} else { // old behavior
//		if (indirection != NULL && reason != FOLLOWING_INDIRECTIONS_CHANGE) {
			// used to be Sprite::set_size_and_location -- why??
//			indirection->set_size_and_location(new_width,new_height,new_llx,new_lly,reason);
//		};
//	};
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target && reason != UPDATING && reason != JUST_MOVED) {
		log(_T("Debug picture target size and position set."));
	};
#endif
};

const int max_follower_size_ratio = 6;
const int min_follower_size_ratio = 3;

void Picture::now_flipped() {
	flip_going_on_flag = FALSE;
//	if (postponed_copy_flipped_followers_of_controlee_flag) return; // new on 091100
   place_new_item();
	if (controlee != NULL) return; // new on 171200
	// following is new on 120499 so that flipped pictures and other things are updated properly
	Sprites *remaining = flipped_followers();
	while (remaining != NULL) {
        Sprite *follower = remaining->first();
		  if (follower->kind_of() == PICTURE) {
			  Picture *picture = (Picture *) follower;
			  if (picture->is_flipped()) {
				  picture->now_flipped(); // in case sub picture has changed (e.g. trucks or vacuuming up boxes)
			  };
//		  } else {
//			  follower->propagate_changes(); // may have changed while on flipside
		  };
		  remaining = remaining->rest();
	};
	set_placed_new_items(FALSE);
};

//	Sprite::signal_size_and_location();  // was a specialization of  signal_size_and_location but called during flip and unflip
/*
	if (is_flipped() && width > tile_width) {
		Sprites *remaining = flipped_followers();
		while (remaining != NULL) {
         Sprite *follower = remaining->first();
         city_coordinate follower_width = follower->current_width();
         city_coordinate follower_height = follower->current_height();
         if (follower_width*max_follower_size_ratio < width) {
            follower->set_width_maintain_proportionality(width/max_follower_size_ratio);
         } else if (follower_height*max_follower_size_ratio < height) {
            follower->set_height_maintain_proportionality(height/max_follower_size_ratio);
         };
         if (follower_width*min_follower_size_ratio > width) {
            follower->set_width_maintain_proportionality(width/min_follower_size_ratio);
         } else if (follower_height*min_follower_size_ratio > height) {
            follower->set_height_maintain_proportionality(height/min_follower_size_ratio);
         };
			if (follower->kind_of() == CUBBY) {
				((Cubby *) follower)->refit_contents();
			} else if (follower->kind_of() == PICTURE) {
            ((Picture *) follower)->now_flipped();
         };
			remaining = remaining->rest();
		};
	};
*/

boolean Picture::ultimate_indirection_is_picture() {
	if (indirection != NULL) {
		if (indirection->kind_of() != PICTURE) {
			return(FALSE);
		} else {
			return(((Picture *) indirection)->ultimate_indirection_is_picture());
		};
	};
	return(TRUE);
};

void Picture::good_size(city_coordinate &w, city_coordinate &h, Sprite *source, boolean for_expander) {
#if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {
      log("Calculating good size of debug target.");
   };
#endif
	if (is_flipped()) {
      backside_size(w,h,source);
	} else if (!ultimate_indirection_is_picture()) { 
		// commented out on 040299 since set_indirection now updates this...
		// restored 261199 so when indirection is not of a Picture
		city_coordinate old_character_width = indirection->return_character_width();
		city_coordinate old_character_height = indirection->return_character_height();
		indirection->good_size(w,h,source,for_expander);
		indirection->set_character_width(old_character_width); 
		// new on 070604 to restore changes in font size from calling good_size
		indirection->set_character_height(old_character_height);
	} else if (for_expander || saved_width <= 0 || appearance_code() == SCALE_SPRITE) { 
		// added on 010299 since G on Pumpy shouldn't depend upon the history of the object
		// added saved_width <= 0 on 021202
		// added SCALE_SPRITE on 290903 - not sure why it had a saved size that was throwing it off
		if (appearance_code() == USER_SPRITE && pointer_to_current_image() != NULL) { // new on 270603
			w = pointer_to_current_image()->width_without_scaling();
			h = pointer_to_current_image()->height_without_scaling();
			adjust_good_size(w,h,source);
		} else {
			Sprite::good_size(w,h,source,for_expander);
		};
	} else if (controlee != NULL) { // added on 050299 to keep remote looks in synch better  && tt_log_version_number > 14
		controlee->good_size(w,h,source,for_expander); 
		// restored on 070604 since otherwise remote looks of a number wants to have the dimensions of 1
		// commented out the following on 191200 since remote looks no longer track size of controlee
//	} else if (controlee != NULL) { // added on 050299 to keep remote looks in synch better  && tt_log_version_number > 14
////		controlee->good_size(w,h,for_expander);
//		// changed on 220499 to the current size of the controlee not what it is thinks its good size is
//		city_coordinate controlee_width = controlee->current_width(); 
//		city_coordinate controlee_height = controlee->current_height();
//		if (controlee_width < tile_width || controlee_height < tile_height) {
//			// better to see it than to really track size exactly (new on 060300)
//			w = width;
//			h = height;
//		} else {
//			w = controlee_width;
//			h = controlee_height;
//			adjust_size_for_source(w,h,source);
//		};
//	} if (!size_constrained()) { // added on 050299 to keep remote looks in synch better
//		w = width;
//		h = height;
	} else {
		w = saved_width;
		h = saved_height;
      if (floor != NULL && floor->in_the_room_with_programmer()) {
			// as of 020205 also check that programmer is in the room when doing this since wrong if robots are left behind
         tt_programmer->translate_floor_to_room_dimensions(w,h);
      };
	};
};

Sprite *Picture::forwarding_to(boolean pictures_ok) {
	if (is_flipped()) return(NULL);
	Sprite *forward_to = NULL;
	if (controlee != NULL && (forward_to = controlee->pointer_to_indirection()) != NULL) {
	} else if (indirection != NULL) {
//		        && followers == NULL // removed on 040100 since should always be true now
		// act like indirection if not a composite picture
		forward_to = indirection;
	};
	if (forward_to != NULL && !pictures_ok && forward_to->kind_of() == PICTURE) { 
		// moved here from first conditional branch on 040100 so indirections to picture aren't treated specially
		forward_to = NULL; // don't forward
	};
	return(forward_to);
};

void Picture::set_indirection(Sprite *new_indirection, boolean transfer_followers) {
#if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {
      log("set_indirection called on debug target.");
	} else if (new_indirection != NULL && tt_debug_target == new_indirection->debug_counter) {
		log("debug target became indirection");
	};
	//if (new_indirection != NULL && new_indirection->remote_status() != NOT_REMOTE) {
	//	debug_this(); // indirection to a remote control is strange... 120105 -- yes but happens if placed on front of a picture
	//};
#endif
	city_coordinate real_width,real_height,real_llx,real_lly; 
	// restored this on 050100 to get geometry of followers of new_indirection to work right
	// moved the following on 140499 since the true_width etc are changed by the stuff after
	// following added 280199 so that size and location doesn't change when remote looks is used by erasing...
	// changed to true_size on 130499 to improve geometry (e.g. explosion sprite)
//		indirection->set_true_size_and_location_to_those_of(this,FOLLOWING_INDIRECTIONS_CHANGE);
	real_width = true_width();
	real_height = true_height();
	real_llx = true_llx();
	real_lly = true_lly();
	// commented out (for now) on 060400 because the behavior on the back of new_direction may get lost in the process
	// restored on 241000 but only if new_indirection has no followers
	while (new_indirection->kind_of() == PICTURE && 
		    new_indirection->pointer_to_followers() == NULL && new_indirection->no_flipped_followers() && // new conditions on 241000
			 !new_indirection->held() && new_indirection->pointer_to_leader() == NULL) { // more conditions on 110604
		// new on 280300 to prevent chains from forming
		Picture *picture_indirection = (Picture *) new_indirection;
		Sprite *indirections_indirection = picture_indirection->pointer_to_indirection();
		if (indirections_indirection == NULL) break;
		picture_indirection->reset_indirection(); // so it no longer holds on to it
		picture_indirection->destroy(); // should be safe -- what about saved_indirection??
		new_indirection = indirections_indirection;
	};
	new_indirection->set_is_picture(TRUE);
//	new_indirection->set_change_font_size_to_fit_extent(FALSE); // new on 121203 so font isn't wrong due to display within region
//   new_indirection->set_background(floor,FALSE,FALSE);
	new_indirection->remove_from_floor(FALSE); 
	// replaces the above on 211200 - note that now indirections should have a NULL background
	if (indirection != NULL) { // added on 280199 since repeatedly setting indirection could leak memory
		if (saved_indirection == indirection) {
			saved_indirection->destroy(this); // new on 311000
			saved_indirection = NULL;
		};
		indirection->destroy();
	};
	indirection = new_indirection;
	indirection->increment_ref_count(this); // added 110101 -- why was this missing???
	set_clean_status(DIRTY);
	if (new_indirection->kind_of() == PICTURE) { // tt_log_version_number > 14 &&
//#if TT_DEBUG_ON
//		if (((Picture *) new_indirection)->is_flipped()) {
//			log("Debug this!");
//		};
//#endif
		// 2nd condition added on 080499 so that colors of text and the like are saved correctly
		code = indirection->appearance_code(); // changed on 290199 to be more "accurate" - e.g. good size for Pumpy
		parameter = indirection->current_parameter(); // added on 250399 to be consistent with changing code above
		current_image = indirection->pointer_to_current_image(); // changed on 290199 to be more "accurate" - e.g. selection region
		current_index = indirection->image_cycle_index();
		reset_cycle(); // added on 140499
		set_size_is_true(indirection->size_is_true()); // revised on 040299 to be more "accurate"
      // commented out on 221002 since that wasn't the cause of the problem
      //// following new on 211002 since otherwise portal in adventure game makes player too big
      //Picture *indirection_picture = (Picture *) indirection;
      //set_saved_width(indirection_picture->return_saved_width());
      //set_saved_height(indirection_picture->return_saved_height());
	} else {
		code = SYNTHETIC_SPRITE; // less work
		set_size_is_true(TRUE);
	};
/*	if (indirection->kind_of() == PICTURE) { // added on 040299
		set_saved_size(((Picture *) indirection)->return_saved_width(),
							((Picture *) indirection)->return_saved_height());
	} else { // saved size is good size of indirection
		city_coordinate w,h;
		indirection->good_size(w,h);
		set_saved_size(w,h);
	};
	*/
//	indirection->set_size_is_true(TRUE); // changed on 10299 to fix inconsistent sizes when animation cycles involved
	set_old_x_offset(0);
	set_old_y_offset(0);
//	set_old_x_offset(indirection->return_old_x_offset());
//	set_old_y_offset(indirection->return_old_y_offset());
	blank = FALSE; // not anymore
	set_visible(indirection->visible());
	set_show_all(indirection->show_all());
//	if (tt_log_version_number > 14) {
		// following added 280199 so that size and location doesn't change when remote looks is used by erasing...
		// changed to true_size on 130499 to improve geometry (e.g. explosion sprite)
	// following was commented out - restored size part on 220200 (e.g. keyboard to a picture will change it to indirections size)
//		indirection->set_true_size_and_location(real_width,real_height,real_llx,real_lly,FOLLOWING_INDIRECTIONS_CHANGE);
	indirection->set_true_size(real_width,real_height,FOLLOWING_INDIRECTIONS_CHANGE);
	indirection->set_selectable(TRUE); 
	// new on 140403 since dropping a number on an erased picture should be the same as flipping the number over
//	};
   if (!new_indirection->is_blank()) {
      switch (new_indirection->kind_of()) {
         case TEXT:
         case SOUND:
         case INTEGER:
             set_show_some_if_subpicture(TRUE);
         break;
      };
   };
	if (transfer_followers) {
		// following is new on 030100 to implement new policy that indirections get the followers of the replacement
		// and lose their own followers
#if TT_DEBUG_ON
		if (is_flipped()) {
			log("Warning: setting indirection of a flipped picture.");
		};
#endif
		remove_all_followers(TRUE);
		Sprites *remaining = indirection->pointer_to_followers();
		if (remaining != NULL) {
			indirection->set_true_size_and_location(real_width,real_height,real_llx,real_lly,FOLLOWING_INDIRECTIONS_CHANGE);
			indirection->refit_contents(); // new on 070600 to be sure followers have the right geometry before transfer
			while (remaining != NULL) {
				// conditional removed on 070600 since transfering non-pictures works fine now 
				// and is consistent with how everything else works with indirections
//				if (remaining->first()->kind_of() == PICTURE) { 
				// conditional new on 130300 since could be cubby on wall during puzzles for example
				Sprite *item = remaining->first();	
				Picture *follower;
				if (item->kind_of() == PICTURE) {
					follower = (Picture *) remaining->first();
				} else { // this is new on 070600
					follower = make_indirect_picture(item,FALSE);
				};
//				user_coordinate x = follower->current_user_value(X_REMOTE); // added 040199
//				user_coordinate y = follower->current_user_value(Y_REMOTE);
//				user_coordinate relative_width = follower->current_user_value(WIDTH_REMOTE);
//				user_coordinate relative_height = follower->current_user_value(HEIGHT_REMOTE);
//					indirection->remove_follower(follower,FALSE);
				// replaced above with following on 060600 since might be say a box who needs to update contents as well
				indirection->remove_selected_element(item,NO_SELECTION_REASON_GIVEN,NULL,FALSE);
				add_follower(follower);
//				follower->become_part_of_picture(); 
				// moved to picture::add_follower on 160802 -- restored on 190802 since not a part until Bammer smashes them
//				follower->display_changed(X_REMOTE,x);
//				follower->display_changed(Y_REMOTE,y);
//				follower->display_changed(WIDTH_REMOTE,relative_width);
//				follower->display_changed(HEIGHT_REMOTE,relative_height);
//				};
				remaining = remaining->rest();
			};
		};
		if (controlee == NULL) { // I'm just a blank picture so add flipped followers of indirection to my back (new on 140200)
			Sprites *remaining = indirection->flipped_followers();
			if (remaining != NULL) {
				flip(TRUE);
				Sprite *previous_follower = NULL;
				while (remaining != NULL) {
					Sprite *follower = remaining->first();
					add_flipped_follower(follower,previous_follower,this);
					previous_follower = follower;
					remaining = remaining->rest();
				};
				unflip(TRUE); // added on 240200
			};
		};
		indirection->remove_all_flipped_followers(controlee != NULL); 
		// arg new on 241000 so if not transfered are deleted - not sure this matters...
		// arg was FALSE and prior to 240200 that was TRUE 
		// ("but followers were "stolen" above so shouldn't be destroyed here" - was the old comment)
	};
};

//Sprites *Picture::pointer_to_followers() { // new on 040100
	// bad idea -- obsolete now
//	if (indirection != NULL && !is_flipped()) {
//		return(indirection->pointer_to_followers());
//	} else {
//		return(Sprite::pointer_to_followers());
//	};
//};

void Picture::set_controlee(Picture *new_controlee) {
	if (controlee == new_controlee || new_controlee == this) return;
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		tt_error_file() << "Setting controlee of debug target." << endl;
	};
	//if (tt_debug_mode == 21204 && new_controlee != NULL && new_controlee->is_flipped()) {
	//	debug_this();
	//};
#endif
	boolean initializing = (controlee == NULL); // new on 201200
   if (controlee != NULL) {
		if (is_blank() && controlee->inside_vacuum()) { // new on 120105 -- what about if controlee was deleted?
			become_non_blank();
		};
		if (new_controlee == NULL && controlee->slated_for_deletion()) { // new on 120105
			become_blank(FALSE,FALSE);
//			set_size(controlee->current_width(),controlee->current_height());
			if (leader != NULL && leader->kind_of() == CUBBY) {
				((Cubby *) leader)->refit_contents();
			};
		} else { 
			// following was unconditional prior to 120105 but if being deleted no need
			controlee->remove_remote(APPEARANCE_REMOTE,this);
			// commented out ref counts on 280300 since remote looks are used for things 
			// like parts/whole and hit who and can lead to circularities
//			controlee->destroy(); // new on 140200 to maintain ref counts for remote looks
			controlee->update_remote_looks_of_me_count(-1);
		};
	};
	controlee = new_controlee;
	if (controlee != NULL) {
		controlee->add_remote(APPEARANCE_REMOTE,this);
//		controlee->increment_ref_count(); // new on 140200 to maintain ref counts for remote looks
		controlee->update_remote_looks_of_me_count(1);
		if (initializing && controlee->is_flipped() && !controlee->is_temporarily_flipped()) { 
			// new on 151200 so they are kept in synch -- second condition added 181200
//			flip(TRUE,FALSE);
			set_flipped(TRUE); 
			// re-written on 181200 since there are no followers of any sort should be OK - 
			// and setting temporarily_flipped is confusing
			set_width_before_flip(controlee->return_width_before_flip());
			set_height_before_flip(controlee->return_height_before_flip());
		};
	};
};

/*
		case CUBBY:
			item->save_size();
			push_cubby((Cubby *) item);	 // save it for callback to use
			center_location(center_x,center_y);
			item->animate_size_and_location(1,1,center_x,center_y,
													  duration,
													  this,add_dropped_cubby_callback);
			item->set_after(duration,action_status,ACTION_COMPLETED);
			return;
		case ROBOT:
			item->save_size();
			remote_robots[0] = (Robot *) item;
			center_location(center_x,center_y);
			item->animate_size_and_location(1,1,center_x,center_y,
													  duration,
													  this,add_dropped_robot_callback);			
			item->set_after(duration,action_status,ACTION_COMPLETED);
			return;
*/

boolean Picture::stopped() {
   if (tt_city->stopped()) return(TRUE);
	if (locally_stopped()) return(TRUE);
	if (leader == NULL) return(FALSE);
	if (is_top_of_a_stack()) return(FALSE); // added 260500
	if (tt_stop_all_pictures && tt_log_version_number >= 56) {
		return(TRUE); // new on 161004 -- tt_log_version_number added 181204
	};
   // if picture is part of a stopped picture then it is stopped too
   Sprite *container = pointer_to_leader();
   while (container != NULL) {
		if (container->kind_of() == HOUSE_ABOVE || container->kind_of() == HOUSE) return(FALSE); 
		// new on 141299 so games run on roof, etc.
		if (container->is_top_of_a_stack()) return(FALSE); // added 260500
		if (container->kind_of() != PICTURE) {
			//return(TRUE); // revised on 1/4/99
			return(container == leader); // new on 240200 so that pictures on pictures inside of boxes aren't stopped
		};
      if (((Picture *) container)->locally_stopped()) return(TRUE);
      container = container->pointer_to_leader();
   };
   return(FALSE);
//	Sprite *ul = ultimate_leader();
//	if (ul->kind_of() == PICTURE) {
//		return(((Picture *) ul)->locally_stopped());
//	} else {
//		return(TRUE);
//	};
};

void Picture::set_to_future_value(Sprite *item_to_add, Sprite *original_recipient, Sprite *by) {
//	if (floor != NULL) floor->remove_item(item_to_add,FALSE); // was TRUE
//	if (item_to_add->pointer_to_background() != pointer_to_background()) { // new on 211200 that if different backgrounds fix it
	// following no-op as of 070105 if follower removed below
		item_to_add->remove_from_floor(FALSE); 
		// new on 201200 to replace the above that wasn't always right when remote looks involved
//		item_to_add->set_background(floor); // commented out on 221200 since going to become a part or an indirection very soon
//	};
	if (item_to_add->pointer_to_leader() == this) {
		remove_follower(item_to_add); // was added as not completely inside
	};
	item_to_add->set_clean_status(DIRTY); // clipping changes, etc.
	item_to_add->set_to_be_smashed(FALSE);
	if (!is_blank()) { // condition new on 300301 since if replacing old appearance shouldn't reset collision stuff
		ignore_collision(); // collision doesn't count since now part/whole
		item_to_add->ignore_collision(); // less clear if this should be part of conditional
	};
	if (active()) { 
		// conditional added on 021100 since you can do this by hand and you might not want it to start running just then
		item_to_add->set_active(TRUE); // new policy on 261199
		item_to_add->activate(FALSE); // args added on 091100 so robots wait a frame to run (and animation cycle isn't reset)
		// args removed on 091203 since is only behavior now
	};
	if (is_blank()) { // giving other new appearance
		if (item_to_add->kind_of() == PICTURE) { // can it be otherwise?
			Picture *picture_to_add = (Picture *) item_to_add;
			Picture *item_to_add_controlee = (Picture *) picture_to_add->pointer_to_controlee();
			if (item_to_add_controlee != NULL) {
				if (is_flipped()) { // new on 150300 -- removed item_to_add_controlee-> on 120601
					// ISN'T THE NEW SCHEME THAT IT DEPENDS UPON THE FLIPPEDNESS OF THE REMOTE LOOKS NOT THE CONTROLEE??? 
					// -- that is why this changed on 120601
					say_error(S(IDS_CANT_DROP_REMOTE_LOOK_OF_FLIPPED_PICTURE_ON_BLANK_PICTURE,
								 "Can't drop the remote control for the looks of a flipped picture on a blank picture."),TRUE); 
					if (saved_by != NULL && saved_by->kind_of() == ROBOT) {
						Robot *robot = (Robot *) saved_by;
						string name_string = robot->return_a_robot_named(0,0); // new on 140300
						tt_error_file() << name_string << " did it." << endl;
						delete [] name_string;
					};
					return;
				};
				// added on 080499 so it coerces a remote looks to an ordinary picture
				picture_to_add->set_controlee(NULL);
				picture_to_add->remove_from_floor(FALSE); // new on 211200
				picture_to_add->destroy();
				// following changed from copy to top_level_copy on 231104
				item_to_add = item_to_add_controlee->top_level_copy(); 
				// was front_side_copy() prior to 140200 and is once again on 181200 
				// - no point copying stuff on back just to throw it away -- 
				// restored it to copy() on 201200 since could be making a "clone" and surely don't want to lose behaviors
				if (item_to_add->is_flipped()) { // new on 120601 - controlee was flipped but not remote looks
					((Picture *) item_to_add)->unflip(TRUE);
				};
            ((Picture *) item_to_add)->save_size(); // new on 211002 since otherwise can get saved size of item_to_add_controlee
			};
		};
		become_non_blank();
		if (controlee != NULL) {
			// commented out the following on 050100 to get geometry to work out right
//			if ((controlee->pointer_to_leader() == NULL || 
//				  controlee->pointer_to_leader()->kind_of() == PICTURE) &&
//				 (saved_by == NULL || saved_by->kind_of() != ROBOT || tt_log_version_number < 15)) { 
				// added on 130499 so don't do compute size if a robot is responsible
				// compute size using the item_to_add dimensions interpreted
				// as relative to whole window and apply relative to controlee's
				// leader
//				city_coordinate relative_width
//					= controlee->from_user_x(tt_screen->user_x(item_to_add->true_width()));
//				city_coordinate relative_height
//					= controlee->from_user_y(tt_screen->user_y(item_to_add->true_height()));
				// center around new size
				// write a method in sprite for this....
//				controlee->move_by((controlee->current_width()-relative_width)/2,
//										 (controlee->current_height()-relative_height)/2);
//				controlee->set_size(relative_width,relative_height);
//			};
			controlee->set_indirection(item_to_add,TRUE);
		} else {
//			if (tt_log_version_number < 15) { // is this right size and position??
//				item_to_add->set_size(true_width(),true_height());
////			   set_size(item_to_add->true_width(),item_to_add->true_height());
//			}
			set_indirection(item_to_add,TRUE); // moved here on 100599 from inside conditional above 
			// fixed dropping pictures on blank numbers
		};
		user_did(PICTURE,PICTURE_REDIRECTED);
		//if (saved_by != NULL) saved_by->destroy(); 
		// if ref count is zero - added 130499 -- commented out on 191102 since used by action_completed() now
		//saved_by = NULL;
		return;
	};
	if (controlee != NULL) { // am a remote for another
		city_coordinate controlee_llx, controlee_lly;
		controlee->lower_left_corner(controlee_llx,controlee_lly);
//		item_to_add->move_to(controlee_llx,controlee_lly);
		item_to_add->move_by(controlee_llx-llx,controlee_lly-lly); // new version of 101199 to keep the geometry correct
		controlee->add_follower(item_to_add);
//		item_to_add->become_part_of_picture(); // currently no-op if not a picture (revised on 030100) 
		// moved to picture::add_follower on 160802 restored on 190802 since not a part until Bammer smashes them
		item_to_add->released(NULL); // for "dropped" sensor and activation -- added 1/4/99
		//if (saved_by != NULL) saved_by->destroy(); // if ref count is zero - 
		// added 130499 commented out on 191102 since used by action_completed() now
		//saved_by = NULL;
		return;
	};
	add_follower(item_to_add);
	if (item_to_add->kind_of() == PICTURE) {
		Picture *picture = (Picture *) item_to_add;
//		picture->become_part_of_picture(); 
		// moved to picture::add_follower on 160802 restored on 190802 since not a part until Bammer smashes them
		NumberValue *x_speed = picture->current_x_speed();
		NumberValue *y_speed = picture->current_y_speed();
		if (x_speed != NULL && y_speed != NULL) {
			long x_speed_long, y_speed_long; // good enough for the purpose of transfering speed from floor to subpicture
			x_speed->long_value(x_speed_long);
			y_speed->long_value(y_speed_long);
			picture->set_x_speed(new LongNumberValue((x_speed_long*ideal_screen_width)/width)); 
			// new on 180300 to convert to same apparent speed
			picture->set_y_speed(new LongNumberValue((y_speed_long*ideal_screen_height)/height));
		};
		if (active()) { 
			// conditional added on 021100 since you can do this by hand and you might not want it to start running just then
			picture->activate(); 
		};
//      switch (picture->pointer_to_indirection()->kind_of()) {
//          case INTEGER:
//          case TEXT:
//             picture->set_show_all(FALSE);
//      };
   };
	// not clear what the following does and if it should - asked 230903
	if (!stopped() && !is_flipped()) { // huh??
		activate(FALSE); // args added on 091100 so robots wait a frame to run (and animation cycle isn't reset)
		// args removed on 091203 since is only behavior now
	};
	//if (saved_by != NULL) saved_by->destroy(); 
	// if ref count is zero - added 130499 commented out on 191102 since used by action_completed() now
	//saved_by = NULL;
};

void Picture::tell_flipped_followers_now_on_back() { // abstracted 101203
	Sprites *remaining = flipped_followers();
	while (remaining != NULL) {
		remaining->first()->now_on_back_of(this);
		remaining = remaining->rest();
	};
};

void Picture::now_on_floor(Background *floor, Sprite *by) {
	Sprite::now_on_floor(floor,by);
//	if (tt_log_version_number > 14) 
	save_size(); // added on 050299
	if (controlee != NULL) return; // new on 171200
//	if (is_flipped()) { // new on 101199 so connections are ensured
		tell_flipped_followers_now_on_back();
//	};
};

void Picture::recursively_activate_pictures(boolean reset_animation, PictureActivateContext context) { 
	// added 1st arg on 150200 and 2nd on 121000 and third on 091100 (third arg - delay - removed 091203)
   activate(reset_animation,context);
};

void Picture::set_active(boolean new_flag, boolean recur, boolean initialization) {
	//if (!new_flag) { // new on 180405
	//	set_activated(FALSE);
	//};
	boolean turned_on = (new_flag && !active());
	Sprite::set_active(new_flag,recur);
	if (initialization) return; // new on 050604
	if (recur && controlee == NULL) { // make sure its parts aren't stopped as well - second condition added 151200
		Sprites *remaining = flipped_followers(); 
		// commented out the following on 100400 since flipped_followers() does this
//		if (is_flipped()) {
//			remaining = followers; // this branch new on 070400 wasn't conditional before
//		} else {
//			remaining = flipped_followers();
//		};
		while (remaining != NULL) {
			Sprite *follower = remaining->first();
			follower->set_active(new_flag,recur);
			remaining = remaining->rest();
		};
		// commented out on 100400
//		if (backside_background != NULL) { // new on 070400 - but isn't this redundant with flipped followers??
//			remaining = backside_background->pointer_to_items();
//		while (remaining != NULL) {
//				remaining->first()->set_active(new_flag,recur);
//				remaining = remaining->rest();
//			};
//		};
	};
	if (turned_on) { // new on 131299
		update_remote(TRUE);
	};
	if (indirection != NULL) { // new on 040100
		indirection->set_active(new_flag,recur);
	};
	if (controlee != NULL && controlee->active() != new_flag) { // new on 040100
		// checking that indirection is in a different state added on 050100 to prevent looping
 		controlee->set_active(new_flag,recur);
	};
};

void Picture::now_on_back_of(Picture *picture) {
	if (picture != NULL) {
		Picture *pictures_controlee = picture->pointer_to_controlee();
		if (pictures_controlee == this) { // new on 020500
			// checked that picture is non-NULL on 040500
			say(S(IDS_PICTURE_ON_BACK_OF_REMOTE_LOOKS,
				   "Warning. Strange things may happen when you put something on the back of the remote control for its looks."),
				 99,TRUE);
			return;
		} else if (pictures_controlee != NULL) {
			// new on 050105
			now_on_back_of(pictures_controlee);
			return;
		};			
	};
	if (indirection != NULL) { // new on 050504 -- e.g. indirection is a sensor
		indirection->now_on_back_of(picture);
//		return; // commented out on 060504 since this broke allegiance switching in many cases
	};
   if (controlee != NULL) {
//		if (picture != NULL) { // commented out on 180299
      // following was handy but confusing and hard to dump/load
      // can always use birds
//      if (picture != NULL && (controlee->part_of(picture) || picture->part_of(controlee))) {
         // OK to control remotes of followers (and followers' followers, ...)
         // or to control remotes of leaders (and leaders' leaders, ...)
//      } else {
		if (//is_flipped() || // commented out on 191200 since no longer makes sense
			 picture != NULL) { // condition added on 080499 so that remote looks don't lose their connection
			set_controlee(picture);
      };
		return; // new on 171200
   };
	// following moved from top to here on 080499 so that remote looks get a new controlee
   if (!is_flipped()) return; // e.g. inside a cubby and not flipped -- e.g. a user timer...
	// following added on 180299
	if (picture == NULL) picture = this; // restore flipped followers connection to this  && tt_log_version_number > 14
	Sprites *remaining = flipped_followers(); // better name would be backside_followers() ??
	while (remaining != NULL) {
		Sprite *follower = remaining->first();
		follower->now_on_back_of(picture);
		remaining = remaining->rest();
	};
	if (picture->active() && !picture->is_flipped()) {
		// new on 050105 to cover the case where a robot on the back dropped on a remote looks a new gadget
		// if the recipient is active then this should be activated as well
		activate(FALSE,ULTIMATE_PICTURE_CONTAINER_NOT_FLIPPED); // args added on 060105
	};
};

void Picture::activate(boolean reset_animation, PictureActivateContext context) { 
	// added context on 131000
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Debug target activated."));
	};
//	if (tt_debug_mode == 51201) {
//		tt_error_file() << "Activating Picture#" << debug_counter << endl;
//	};
#endif
	if (tt_programmer->training()) {
		return; 
		// new on 011204 since robot's don't know about robots on the back of pictures -- and it makes it hard to train sometimes
	};
	if (pointer_to_top_of_stack() != NULL) {
		pointer_to_top_of_stack()->activate(reset_animation,context);
		return;
	};
	if (!ok_to_activate()) return; // new on 081199 - rewritten on 160702
	if (controlee != NULL && !is_flipped()) { // new on 191199 since if a remote control like looks shouldn't run stuff on the back
		// !is_flipped() added on 191299 since could be a flipped remote looks dropped on another picture
//		controlee->activate(reset_animation); // tried adding this on 191299 but caused stack overflow
		return;
	};
// Following is needed for putting flipped pictures on the back of pictures
//	if (flipped && (floor == NULL || floor->pointer_to_picture() == NULL)) {
		// replaced the following on 121000 so that pictures that are face up on the back of a picture can run when appropriate
//		return; //  || tt_log_version_number < 7
//		only_pictures = TRUE; 
//	} else { // so robots on flipped pictures don't get activated but if unflipped on flipped they do...
//		only_pictures = flipped;
//	};
   if (leader != NULL) {
      if (leader->kind_of() == PROGRAM_PAD) return; // new policy as of 8/26/97 -- consider use timers for example
		if (leader->kind_of() != PICTURE && is_flipped()) { // new on 091100 - don't activate flipped pictures in boxes for example
			return;
		};
   };
	//if (tt_log_version_number >= 73) {
	//	if (activated()) { // new on 180405
	//		return; // already activated
	//	};
	//	set_activated(TRUE); 
	//};
   stopped_flag = FALSE; // new on 170702
	if (context == PICTURE_ACTIVATE_CONTEXT_NOT_INTIALIZED) { // this is the ultimate container of type PICTURE
		if (is_flipped()) {
			context = ULTIMATE_PICTURE_CONTAINER_FLIPPED;
		} else {
			context = ULTIMATE_PICTURE_CONTAINER_NOT_FLIPPED;
		};
	} else if (context == ULTIMATE_PICTURE_CONTAINER_FLIPPED && !is_flipped()) {
		// treat unflipped pictures on the back of a flipped picture as "top level context"
		context = ULTIMATE_PICTURE_CONTAINER_NOT_FLIPPED;
	};
	Sprites *remaining = flipped_followers();
	if (tt_city->stopped() && tt_log_version_number > 14) {
		if (remaining != NULL || unflipped_followers() != NULL) { // actually some value to postponing this
			if (insert_at_end_if_not_member(this,tt_pictures_to_activate)) {
				increment_ref_count(this); // to protect it from deletion
			};
		};
		return;
	};
	while (remaining != NULL) {
		Sprite *sprite = remaining->first();
		if (context == ULTIMATE_PICTURE_CONTAINER_FLIPPED) {
//			if (sprite->kind_of() == PICTURE) { // commented out on 131000 so activate pictures in boxes on back - OK??
				sprite->activate(reset_animation,context);
//			};
		} else {
			switch (sprite->kind_of()) {
				case ROBOT: {
					Robot *robot = (Robot *) sprite;
					if (robot->pointer_to_body() == NULL) { // new on 271100 - do nothing and don't warn
					} else if (robot->waiting_for_release() || robot->suspended() || 
									(robot->pointer_to_working_cubby() != NULL && tt_log_version_number > 21)) {
						// third disjunct added on 210300
						robot->activate(FALSE,context); 
						// args added on 091100 so robots wait a frame to run (and animation cycle isn't reset)
						// args removed 091203 since is default now
						// second arg added on 060105
						robots_running_on_back_flag = TRUE; // might not really be true
#if TT_DEBUG_ON
					} else {
						if (robot->pointer_to_working_cubby() == NULL) { // to avoid spurious warnings for demos
							log("Warning: robot on back of a picture has no box to work on. All might be well."); // added 081299
							string name_string = robot->return_a_robot_named(0,0); // new on 140300
							tt_error_file() << name_string << " is the robot." << endl;
							delete [] name_string;
						};
#endif
					};
					break;
					};
//				case PICTURE: // should be a flipped picture "lending" its behavior
//					((Picture *) sprite)->activate();
//					break;
				default:
//					 if (tt_debug_mode != 100400 || sprite->is_flipped()) { 
					// experimental mode where these things are not turned on or off
						sprite->recursively_activate_pictures(reset_animation,context); 
						// is default as of 091203 ,1); // 1 added to delay robots (on 091100)
//					};
					break;
			};
		};
		remaining = remaining->rest();
	};
	if (context == ULTIMATE_PICTURE_CONTAINER_NOT_FLIPPED) { // conditional new on 131000
		remaining = unflipped_followers();
		while (remaining != NULL) {
			remaining->first()->activate(reset_animation); 
			// args added on 091100 so robots wait a frame to run (and animation cycle isn't reset)
			// args removed on 091203 since is only behavior now
			remaining = remaining->rest();
		};
	};
	if (indirection != NULL && indirection->kind_of() == PICTURE) {
		indirection->activate(reset_animation,context);
	};
	if (reset_animation) reset_cycle(); // especially for one shot animations
};

Sprite *Picture::released(Sprite *by, boolean top_level, SpecialDrop special_drop) {
	if (by != NULL && by->kind_of() != ROBOT && leader == NULL && by->kind_of() != VACUUM) { 
		// and leader == NULL added on 050400 -- last condition added 090100
		save_size(); // new on 170300 to be sure the size is OK especially if dropped on a notebook
	};
	if (tt_log_version_number >= 45) { 
		// why held()?? -- removed on 230704 since interferes with dropping from a wand and what sense did it make?
		if (indirection != NULL && (indirection->looks_like_is_of_type(TEXT) || indirection->looks_like_is_of_type(INTEGER))) {
			indirection->released(by,FALSE,special_drop); // new on 080604
		} else if (controlee != NULL && (controlee->looks_like_is_of_type(TEXT) || controlee->looks_like_is_of_type(INTEGER))) {
			controlee->released(by,FALSE,special_drop); // new on 080604
		};
	};
	Sprite *result = Sprite::released(by,top_level,special_drop);
	if (remote != NULL && top_level) {
		// top_level part added on 130499 so that this isn't triggered if inside of a box or something
      released_flag = TRUE; // no one else cares
		remote->changed(RELEASED_REMOTE,TRUE);
	};
	if (is_flipped()) return(result);
//	stop_all(); // if released on floor will be resumed
//	if (!postponed_copy_flipped_followers_of_controlee_flag) { // condition new on 091100
//		tell_flipped_followers_of_release(by);
//	};
	if (special_drop == GIVE_TOSS_DROP) { // new policy on 100200 that only give toss if F12 triggered it
		tossed(by);
	} else {
//		x_speed = 0;
//		y_speed = 0;
      // updated on 021002
      set_x_speed(NULL);
      set_y_speed(NULL);
	};
	if (comment != NULL) {
		comment->released(by,FALSE,special_drop); // new on 120604
	};
	return(result);
};

void Picture::tossed(Sprite *by) {
	city_coordinate by_x_speed,by_y_speed;
	by->recent_speed(by_x_speed,by_y_speed);
	// speeds given in terms of city units per second
	// need speeds so that 1000 means 1 second to cross screen
	set_x_speed(new LongNumberValue(by_x_speed/milli_width));
	set_y_speed(new LongNumberValue(by_y_speed/milli_height));
	// commented out on 180300 since obsolete now
//		Picture *level_below = leader_picture();
//		if (level_below != NULL) { // isn't this always NULL now??
//			x_speed = (x_speed*width)/level_below->current_width();
//			y_speed = (y_speed*height)/level_below->current_height();
//		};
	user_did(PICTURE,PICTURE_TOSSED,by);
};

void Picture::tell_flipped_followers_of_release(Sprite *by) {
	if (controlee != NULL) return; // new on 171200
	Sprites *remaining = flipped_followers(); 
	while (remaining != NULL) {
		Sprite *flipped_follower = remaining->first();
		if (flipped_follower->kind_of() == CUBBY) {
			flipped_follower->released(by,FALSE); // come back to this .... ,status); -- what?? 171102
		} else if (flipped_follower->kind_of() == PICTURE && flipped_follower->is_flipped()) {
			// added on 270399 so that flipped pictures on the back of flipped pictures with stuff 
			// that cares about release (e.g. unhatched egg in nest) is notified as well
			((Picture *) flipped_follower)->tell_flipped_followers_of_release(by);
		};
		remaining = remaining->rest();
	};
};

boolean displaying_label = FALSE; // new on 060305 so label is updated but not if while it is being displayed

void Picture::display(SelectionFeedback selection_feedback, boolean followers_too, TTRegion *region) {
	city_coordinate adjusted_llx, adjusted_lly;
	city_coordinate real_width, real_height;
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Displaying debug target."));
	};
#endif
	if (pointer_to_top_of_stack() != NULL) { // new on 260500
		pointer_to_top_of_stack()->display(selection_feedback,followers_too,region);
		return;
	};
//	if (blank && controlee != NULL && !controlee->is_blank()) { -- commented out on 191200
		// added 290199 so that if something is dropped on an erased picture
		// the remote looks track the change of looks
//		blank = FALSE;
//	};
	if (tt_programmer->invisible() && leader == NULL) { 
		// new on 021299 since if running a game with hand hidden you don't want wiggling
		selection_feedback = NO_FEEDBACK;
	};
	if (contained_in(THOUGHT_BUBBLE)) { 
		// new policy on 130200 to not display followers when in thought bubble since matching behaves that way
		followers_too = FALSE;
	};
	if (region == NULL) {
		// made obsolete on 230804
	//	if (tt_screen->camera_status() == CAMERA_ABOVE) { // && adjust_height_if_camera_above()) {
 //        boolean roof_decoration = (ultimate_leader()->kind_of() == HOUSE_ABOVE);
 //        city_scale scale = tt_screen->current_scale();
 //        city_coordinate adjusted_height;
 //        if (roof_decoration) {
 //           adjusted_height = height;
 //        } else { // ground decoration as seen from above
 //           adjusted_height = min(tile_height,height); // since from above is just a few pixels deep
 //        };
 //        city_coordinate urx, ury;
 ////        if (code == SYNTHETIC_SPRITE || indirection != NULL) { // commented out as experiment on 200404
 //           // synthetic sprites already deal with scale
 //           // and for indirection let it deal with it
 //           urx = llx+width;
 //           ury = lly+adjusted_height;
 //        //} else {
 //        //   urx = llx+(ground_scale*width)/scale;
 //        //   ury = lly+(ground_scale*adjusted_height)/scale;
 //        //};
 //        TTRegion region_viewed_above(llx,urx,lly,ury);
 //        display(selection_feedback,followers_too,&region_viewed_above);
 //        return;
 //     };
		adjusted_llx = llx;
		adjusted_lly = lly;
		real_width = true_width(); //width;
		real_height = true_height(); //height;
	} else {
		adjusted_llx = region->min_x;
		adjusted_lly = region->min_y;
		real_width = region->width();
		real_height = region->height();
	};
	boolean top_level = (selection_feedback == MOVEMENT_FEEDBACK);
	if (selection_feedback == NO_FEEDBACK_BUT_DISPLAY_OUTSIDE_FOLLOWERS) {
		// pictures are different in that they clip followers
		selection_feedback = NO_FEEDBACK;
	};
	if (selection_feedback != NO_FEEDBACK) {
		// only supporting movement for now
		adjusted_llx += selection_delta_x();
		adjusted_lly += selection_delta_y();
	};
	if (is_flipped() && controlee == NULL) { // second condition added 121700
BEGIN_GDI
		tt_main_window->select_stock_object(GRAY_BRUSH);
		tt_main_window->select_stock_object(WHITE_PEN);
		const city_coordinate one_x_pixel = tt_screen->one_pixel_in_x_city_units();
		const city_coordinate one_y_pixel = tt_screen->one_pixel_in_y_city_units();
		// adding a pixel here otherwise save_region misses a bit
		tt_screen->draw_rectangle(adjusted_llx+one_x_pixel,adjusted_llx+real_width,
										  adjusted_lly+one_y_pixel,adjusted_lly+real_height);
//		tt_main_window->select_stock_object(NULL_PEN);
END_GDI
		// show flip side followers
//		if (controlee != NULL) { 
// new on 151200 - changed to new scheme (where it just looks like there are flipped followers on 171200)
//			ensure_remote_looks_has_flipped_followers();
//		};
		if (followers_too && // added 191200
			// commented out the following on 080704 but why was it there? controlee == NULL as above 
			// and why does indirection matter in comment below?
//			 !is_blank() && // rewritten on 300604 since if has controlee or indirection may be different 
         // was !blank && // new on 140504
			 // this caused some things to display nothing on the back incorrectly
			 (pointer_to_followers() != NULL || tt_log_version_number >= 46) && // even if no followers need label now - 110604
			 real_width > one_x_pixel*10 && 
			 // these conditions added on 031000 since is such a big waste to display stuff on the back of very tiny pictures
			 real_height > one_y_pixel*10) {
//         if (tt_log_version_number <= 7) {
//			   set_clip_region(selection_feedback,region);
//            boolean saved_postpone_dirty_displays = tt_postpone_dirty_displays;
//            tt_postpone_dirty_displays = FALSE; // and don't postpone any of the following
//			   Sprite::display_followers(selection_feedback,region);
//            tt_postpone_dirty_displays = saved_postpone_dirty_displays;
//			   unclip();
//         } else {
			if (!placed_new_items()) { // new on 060201
				place_new_item(); // to look nice
			};
			if (tt_log_version_number >= 46) { // new on 110604
				city_coordinate label_height = real_height/label_fraction; // was just height prior to 300604
				city_coordinate label_lly = lly+real_height-label_height; // was just height prior to 300604
				if (label == NULL) { // geometry too strange while flipping to do this
					if (!flip_going_on()) { // this checks if leader is flipping unlike the old prior to 041204 version
						UnwindProtect<boolean> set(displaying_label,TRUE); // new on 060305
//					if (!flip_going_on_flag) {
						// create a rectangle
						const int label_color_index = 187; // near white
						label = new Picture(llx,label_lly,0,label_color_index,SYNTHETIC_SPRITE,width,label_height);
						Picture *picture_copy = (Picture *) front_side_copy(); 
						// cheaper than full copy -- could have shallow_copy but that may be confusing
//						Picture *picture_copy = make_indirect_picture(this,FALSE); -- didn't work
						picture_copy->unflip(TRUE);
						picture_copy->set_saved_width(-1); 
						// forget the saved width -- new on 291104 so copies of flipped pictures have labels with correct geometry
						picture_copy->set_saved_height(-1);
						picture_copy->set_to_good_size();
						city_coordinate picture_margin = label_height/20;
						city_coordinate picture_height = label_height-2*picture_margin; // leave a little margin
						// picture can take up to half the width
						city_coordinate max_width = width/2-2*picture_margin;
						city_coordinate picture_width 
							= min((picture_copy->current_width()*picture_height)/picture_copy->current_height(),max_width);
						picture_copy->set_size_and_location(picture_width,picture_height,llx+picture_margin,label_lly+picture_margin);
						label->add_follower(picture_copy);
						picture_copy->set_show_all(show_all()); // new on 160604
						if (comment != NULL) { // && comment->looks_like_is_of_type(TEXT)) {
							comment->set_to_good_size();
							city_coordinate comment_width 
								= min((comment->current_width()*picture_height)/comment->current_height(),width-picture_width);
							comment->set_size_and_location(comment_width,picture_height,
								                            llx+picture_width+2*picture_margin,label_lly+picture_margin);
							label->add_follower(comment);
						};
					};
				} else {
					label->set_size_and_location(width,label_height,llx,label_lly); 
					// shouldn't been needed but otherwise is at the wrong place
				};
				if (label != NULL) {
					if (region != NULL) { 
						// new on 300604 since otherwise this takes up the entire flip side
						TTRegion label_region = *region;
						label_region.min_y = label_region.max_y-label_height;
						label->display(selection_feedback,followers_too,&label_region);
					} else {
						label->display(selection_feedback,followers_too,region);  // prior to 160604 was display_as_a_follower
					};
				};
				//if (label->pointer_to_leader() == NULL) {
				//	add_follower(label);
				//};
			};
         display_followers(selection_feedback,region);
//         };
		};
		return;
//	} else if (controlee != NULL && controlee->is_flipped()) { // not flipped but controlee is
//		flip(); // new on 161200
	};
	// since the image of the picture can be a cycle that changes size
	// the frame is computed from the picture 
//	Picture *level_below = leader_picture();
//	if (top_level) { // top level selection so show whole thing
//		unclip();
//	};
//	if (level_below != NULL && !top_level) {
		 // if selection top level don't clip
		// was recursively_... but calling sequence fixes that 
//		level_below->set_clip_region(selection_feedback,region);
//		changed_clipping_region = TRUE;
		// check if clipped off completely
//		if (tt_destination_urx <= tt_destination_llx || 
//			 tt_destination_ury <= tt_destination_lly) {
//			tt_destination_llx = saved_destination_llx;
//			tt_destination_lly = saved_destination_lly;
//			tt_destination_urx = saved_destination_urx;
//			tt_destination_ury = saved_destination_ury;
//			return;
//		};
//	} else if ((leader != NULL && leader->kind_of() == PICTURE) || top_level) { 
		// clip to screen since not a subpicture
		// is probably about to be clobbered by mouse
		// or is top level selection
//		unclip();
//	};
	if (visible() && show_all() && !rectangular() && !is_blank() && indirection == NULL && controlee == NULL) { 
		// on 181000 added controlee == NULL
//		 tt_log_version_number > 14) { 
		// on August 31 '98 restored old behavior of showing all including backing
BEGIN_GDI
		tt_main_window->select_stock_object(BLACK_BRUSH);
      tt_main_window->select_stock_object(NULL_PEN);
		tt_screen->draw_rectangle(adjusted_llx,adjusted_llx+real_width,
										  adjusted_lly,adjusted_lly+real_height);
END_GDI
   };
	if (//visible() && show_all() &&   // new policy on 10/16 to show wire border only when selected
//       (selection_feedback != NO_FEEDBACK || held()) &&
//       (top_level || !subpicture()) &&
       (this == tt_selection || !visible()) && // added invisible case on 290403 so you can see frame around invisible objects
		 (top_level || held()) &&
       fine_kind_of() != TOOL_BUTTON &&
       code != SCALE_SPRITE) {
//       (indirection == NULL || indirection->kind_of() == PICTURE)) {
//		 draw_backing_and_frame()) {
// a remote for looks can be made blank -- shouldn't be black...
BEGIN_GDI
//		if (code == USER_SPRITE && !blank) { // black is transparent when "see some"
//			tt_main_window->select_stock_object(BLACK_BRUSH);
//         tt_main_window->select_stock_object(NULL_PEN);
//		} else {
//			tt_main_window->select_stock_object(WHITE_BRUSH);
//			tt_main_window->select_stock_object(BLACK_PEN);
      tt_main_window->select_stock_object(NULL_BRUSH);
      tt_main_window->select_stock_object(WHITE_PEN);
      // expand region so even rectangles are surrounded by border
//		tt_screen->draw_rectangle(adjusted_llx-tt_screen->one_pixel_in_x_city_units(),adjusted_llx+real_width,
//										  adjusted_lly,adjusted_lly+real_height+tt_screen->one_pixel_in_y_city_units());
		// removed extra pixel expansion on 190301 since that left screen turds behind sometimes
		tt_screen->draw_rectangle(adjusted_llx,adjusted_llx+real_width,
										  adjusted_lly,adjusted_lly+real_height);
END_GDI
	};
	if (want_feedback(selection_feedback)) {
		// recursive calls don't show frame
		selection_feedback = RECURSIVE_MOVEMENT_FEEDBACK;
	};
	if (controlee != NULL) {  // this way after (indirection != NULL) case but when blank didn't do the right thing...
//		city_coordinate controlee_llx,controlee_lly;
//		controlee->lower_left_corner(controlee_llx,controlee_lly);
//		city_coordinate controlee_width = controlee->current_width();
//		city_coordinate controlee_height = controlee->current_width();
//		CleanStatus controlee_clean_status = controlee->current_clean_status();
//		controlee->set_size_and_location(width,height,llx,lly);
//		boolean saved_tt_postpone_dirty_displays = tt_postpone_dirty_displays;
//		tt_postpone_dirty_displays = FALSE;
//		controlee->display(screen,selection_feedback,TRUE);// followers_too);
//		tt_postpone_dirty_displays = saved_tt_postpone_dirty_displays;
//		controlee->set_size_and_location(controlee_width,controlee_height,
//													controlee_llx,controlee_lly);
		// restore status since didn't really change it
//		controlee->just_set_clean_status(controlee_clean_status);
		TTRegion my_region;
		if (region == NULL) {
//			if (tt_log_version_number > 14) {
//				if (controlee->is_flipped()) { // commented out on 171200 since controlee can be flipped and remote not now
					// added on 280399 so things have the right geometry on the remote looks of a flipped picture
//					full_region(my_region); // don't care if it the controlee is flipped about true dimenions
//				} else {
					true_region(my_region); // changed 010299
//				};
//			} else {
//				full_region(my_region);
//			};
		} else {
			my_region = *region;
		};
		// commented out on 150603 since even if controlee is off somewhere the remote needs to display
      //boolean controlee_showing;
      //if (region != NULL && controlee->showing_on_a_screen()) { 
		// added second condion on 150603 but not clear why this is a good idea anyway??
      //   TTRegion controlee_region;
      //   controlee->full_region(controlee_region); 
      //   controlee_showing = controlee_region.overlaps(region);
      //} else {
      //   controlee_showing = TRUE; // or worth checking more carefully??
      //};
      //if (controlee_showing) { // new on 071002 -- no point if completely off screen (or elsewhere) -- 
         // and this fixed a selection bug when controlee was off screen
		   if (!controlee->rectangular() && !controlee->is_flipped() && !is_flipped()) { 
				// new on 171200 so remote looks animate background if not rectangular
			   // added is_flipped test on 210102 since is rectangular while flipped
BEGIN_GDI	
			   if (cycle_background_color()) {
				   tt_main_window->select_stock_object(NULL_PEN); 
					// was WHITE_PEN prior to 181000 since looks bad if drawn over by appearance of remote
				   tt_screen->draw_rectangle(adjusted_llx,adjusted_llx+real_width,
												     adjusted_lly,adjusted_lly+real_height);
				   set_clean_status(DIRTY);
			   };
END_GDI
		   };
//		   controlee->set_background(floor); // to ensure showing_on_a_screen() is correct - 
			// moved here from branch of conditional below on 171200 - then commented out
		   if (controlee->is_flipped() && !is_flipped()) { // restored on 161200
				boolean controlee_was_temporarily_flipped = controlee->is_temporarily_flipped();
			   controlee->unflip(TRUE,TRUE);
			   controlee->set_temporarily_unflipped(TRUE); // to get geometry of parts right
//			   controlee->set_size(controlee->return_width_before_flip(),controlee->return_height_before_flip()); // new 171200
			   controlee->display(selection_feedback,followers_too,&my_region);
			   controlee->flip(TRUE,FALSE);
			   controlee->set_temporarily_unflipped(FALSE); 
			   controlee->place_new_item();
				controlee->set_temporarily_flipped(controlee_was_temporarily_flipped); // new on 120504
		   } else if (!controlee->is_flipped() && is_flipped()) { // new on 171200
				boolean controlee_was_temporarily_flipped = controlee->is_temporarily_flipped();
			   controlee->flip(TRUE,FALSE);
			   controlee->place_new_item(); // to look nice -- tried commenting it out as experiment on 240903
				//boolean saved_placed_new_items_flag = placed_new_items_flag;
				//set_placed_new_items(TRUE); // another 240903 experiment
				// added flip_going_on() on 010305 so that this doesn't cause the label on the back of the picture to have bad geometry
			   controlee->display(selection_feedback,(followers_too&&!flip_going_on()),&my_region); 
				// followers_too was FALSE prior to 191200
				//set_placed_new_items(saved_placed_new_items_flag);
			   controlee->unflip(TRUE,FALSE);
				controlee->set_temporarily_flipped(controlee_was_temporarily_flipped); // new on 120504
		   } else {
			   // commented out on 060201 since handled more generally now
//			   if (controlee->is_flipped() && controlee->placed_new_items()) { // new on 050201 - second conjunct added 060201
//				   controlee->place_new_item(); // to look nice
//			   };
			   controlee->display(selection_feedback,followers_too,&my_region); 
				// followers_too was FALSE prior to 101199 so were not displayed
		   };
//      };
   } else if (is_blank() && (indirection == NULL || indirection->kind_of() == PICTURE)) { 
		// moved here on 260201 from before the controlee branch above
		// conjunct added on 040399 so that erased picture of a number looks like a number
BEGIN_GDI
// forgot BEGIN_GDI added on 240499
		HGDIOBJ old_selection = NULL;
		bytes permutation = color_permutation(selection_feedback);
		if (permutation != NULL) {
			// new on 200901
			old_selection = tt_main_window->select_object(tt_main_window->create_solid_brush(compute_color(tt_white,permutation)));
		} else {
			tt_main_window->select_stock_object(WHITE_BRUSH);
		};
      tt_main_window->select_stock_object(BLACK_PEN);
      tt_screen->draw_rectangle(adjusted_llx-tt_screen->one_pixel_in_x_city_units(),adjusted_llx+real_width,
										  adjusted_lly,adjusted_lly+real_height+tt_screen->one_pixel_in_y_city_units());
		if (old_selection != NULL) DeleteObject(tt_main_window->select_object(old_selection));
END_GDI
	} else if (indirection != NULL) {
//		indirection->set_size_and_location(width,height,llx,lly);
//		                                   llx-true_x_offset(),lly-true_y_offset());
//		boolean saved_tt_postpone_dirty_displays = tt_postpone_dirty_displays;
//		tt_postpone_dirty_displays = FALSE;
//		indirection->display(screen,selection_feedback,TRUE);
//		tt_postpone_dirty_displays = saved_tt_postpone_dirty_displays;
		indirection->set_visible(visible());
		// commenting out the following an experiment on 111202
//		if (leader_picture() == NULL) {
//			indirection->set_show_all(show_all());
//#if TT_DEBUG_ON
//		} else {
//			log("debug this");
//#endif
//		};
		indirection->set_background(floor); // to ensure visible() is correct
      TTRegion my_region;
      if (region == NULL) {
//        if (tt_log_version_number > 14) {
//		   if (!size_constrained()) {
					true_region(my_region); // changed 010299
//				} else {
//					full_region(my_region); // added on 130499 because it works when say an explosion is added to a picture
//				};
//			} else {
//				full_region(my_region);
//			};
			// following is wrong so was commented out on 130499
//         if (!size_constrained()) { // move indirection to where I am but leave its size alone
//            my_region.max_x = my_region.min_x+indirection->current_width();
//            my_region.max_y = my_region.min_y+indirection->current_height();
//         };
      } else {
         my_region = *region;
      };
		// rewrote the following on 060600 since cubby no longer has its followers or content
//		if (indirection->kind_of() == PICTURE) { 
			// new on 200200 since if say a cubby on a wall should display followers here
		if (leader == NULL && region == NULL && !animation_in_progress() && !indirection->looks_like_is_of_type(PICTURE)) {
			// && !held()) { 
            // removed && !held() on 111102 -- put back on 120105 -- issue 612 -- taken out again since didn't help
            // added !indirection->looks_like_is_of_type(PICTURE) on 221102 
			   // since if picture its size shouldn't change (robots have been transfered)
            // if held then this might look bad but code currently relies upon my_region (noticed on 081102)
				// new on 180102 so that if indirection is text or number and changed sized then picture does as well
            // !animation_in_progress() is new on 081102 otherwise timers drop into boxes or wall decoration poorly (leave turds)
				city_coordinate indirection_width = indirection->current_width();
				city_coordinate indirection_height = indirection->current_height();
            city_coordinate indirection_llx = indirection->current_llx();
            city_coordinate indirection_lly = indirection->current_lly();
            // prior to 081102 last arg to display was &my_region but then never changed size appropriately
            indirection->move_to(llx,lly); // new on 081102
			   indirection->display(selection_feedback,FALSE,NULL); 
 //           if (region == NULL) { // condition new on 141002 so this doesn't happen if being displayed within a region
				city_coordinate new_indirection_width = indirection->current_width();
				city_coordinate new_indirection_height = indirection->current_height();
            city_coordinate new_indirection_llx = indirection->current_llx();
            city_coordinate new_indirection_lly = indirection->current_lly();
				if (indirection_width != new_indirection_width || indirection_height != new_indirection_height ||
                indirection_llx != new_indirection_llx || indirection_lly != new_indirection_lly) {
               // added position stuff on 081102 - could I just test if dirty??
					set_size_and_location(new_indirection_width,new_indirection_height,new_indirection_llx,new_indirection_lly,
												 FOLLOWING_INDIRECTIONS_CHANGE);
				};
//            };
			} else {
				indirection->display(selection_feedback,FALSE,&my_region); // FALSE was TRUE prior to 040100
			};
			if (followers_too && followers != NULL) { // new on 030100
				display_followers(selection_feedback,region);
			};
//		} else {
//			indirection->display(selection_feedback,TRUE,&my_region);
//		};
	} else if (!visible()) { // new policy on September 1 '98
		if (followers_too && pointer_to_followers() != NULL) {
			display_followers(selection_feedback,region);
		};
      return; 
	} else {
		// following was a bad idea -- restored old scheme on August 31 '98
//      if (!show_all() && followers != NULL &&
//			 tt_log_version_number < 15) { 
//			// just show the followers -- new policy on 10/16/97
//         move_mark(selection_feedback); // do nothing here but followers will be shown later
//      if (!show_all() && rectangular() && followers != NULL) {
//		} else 
		if (code == SYNTHETIC_SPRITE) {
			bytes permutation = color_permutation(selection_feedback);
#if TT_DEBUG_ON
			millisecond start;
			if (tt_debug_mode == 150500) {
				start = timeGetTime();
			};
         if (tt_debug_mode == 90502 && permutation != NULL) {
            tt_debug_mode = 90503;
         };
#endif
BEGIN_GDI
		   SyntheticShape shape = (SyntheticShape) (parameter/256);
         HGDIOBJ old_selection = NULL;
			color_index color = (color_index) (parameter%tt_colors_available);
//			if (permutation != NULL) { // commented out on 011001
//				color = permutation[color];
//			};
         if (shape == SYNTHETIC_LINE_45 || shape == SYNTHETIC_LINE_315 ||
             shape == SYNTHETIC_HOLLOW_CIRCLE || shape == SYNTHETIC_ROUNDED_HOLLOW_RECTANGLE ||
             shape == SYNTHETIC_HOLLOW_RECTANGLE) {
            if (shape == SYNTHETIC_HOLLOW_CIRCLE || shape == SYNTHETIC_ROUNDED_HOLLOW_RECTANGLE ||
                shape == SYNTHETIC_HOLLOW_RECTANGLE) {
               tt_main_window->select_stock_object(NULL_BRUSH);
            };
//            old_selection = tt_main_window->select_object(tt_main_window->create_solid_pen(color));
				old_selection = tt_main_window->select_object(tt_main_window->create_solid_pen(compute_color(color,permutation))); 
				// re-written on 011001
         } else if (shape == SYNTHETIC_DOTTED_LINE_45 || shape == SYNTHETIC_DOTTED_LINE_315) {
//            old_selection = tt_main_window->select_object(tt_main_window->create_dotted_pen(color));
				old_selection = tt_main_window->select_object(tt_main_window->create_dotted_pen(compute_color(color,permutation))); 
				// re-written on 011001
			} else if (shape != SYNTHETIC_BUT_INVISIBLE) {
//			   old_selection = tt_main_window->select_object(tt_main_window->create_solid_brush(color));
				old_selection = tt_main_window->select_object(tt_main_window->create_solid_brush(compute_color(color,permutation))); 
				// re-written on 011001
            tt_main_window->select_stock_object(NULL_PEN);
         };
		   // parameter nows encodes both color and form
         tt_screen->draw_synthetic_shape(shape,
                                         adjusted_llx,adjusted_llx+real_width,
											        adjusted_lly,adjusted_lly+real_height);
         // restore old brush and delete just created one
         if (old_selection != NULL) DeleteObject(tt_main_window->select_object(old_selection));
END_GDI
#if TT_DEBUG_ON
			if (tt_debug_mode == 150500) {
				tt_error_file() << (timeGetTime()-start) << "ms to display a synthetic shape." << endl;
			};
#endif
         move_mark(selection_feedback);
		} else {
			display_without_followers(selection_feedback,region);
		};
		if (followers_too && pointer_to_followers() != NULL) {
//			coordinate saved_destination_llx = tt_destination_llx,
//						  saved_destination_lly = tt_destination_lly,
//						  saved_destination_urx = tt_destination_urx,
//						  saved_destination_ury = tt_destination_ury;
//			set_clip_region(selection_feedback,region);
			display_followers(selection_feedback,region);
			// restore clipping region
//			tt_destination_llx = saved_destination_llx;
//			tt_destination_lly = saved_destination_lly;
//			tt_destination_urx = saved_destination_urx;
//			tt_destination_ury = saved_destination_ury;
//			tt_main_window->set_clip_region();
		};
	};
};

boolean Picture::flip_going_on() { // new on 041204
	if (leader == NULL) return(flip_going_on_flag);
	if (leader->kind_of() == PICTURE) {
		return(((Picture *) leader)->flip_going_on());
	};
	return(FALSE);
};

void Picture::set_clean_status(CleanStatus status, boolean recur) {
	// new on 060305 to recompute the label if need be
	if (status == DIRTY && !displaying_label && label != NULL) { // status != CLEAN && status != FOLLOWERS_DIRTY
		// FOLLOWERS_DIRTY test added 070305 since any remote on the back will recur up the leader change annoucning this
		// updated again on 070305 since there are other status states like DIRTY_BELOW that should be ignored here
		// should really have BACK_FOLLOWERS_DIRTY and FRONT_FOLLOWERS_DIRTY ...
		if (controlee == NULL) { // condition new on 070305
			if (comment != NULL) {// new on 070305
				label->remove_follower(comment); // new on 070305
			};
			label->destroy();
			label = NULL;
		//} else { // doesn't matter if remote looks changes (e.g. size or position) unless controlee also changes
		//	controlee->set_clean_status(status,recur); // new on 070305
		};
	};
	Sprite::set_clean_status(status,recur);
};

void Picture::display_followers(SelectionFeedback selection_feedback, TTRegion *region) {
   boolean saved_postpone_dirty_displays = tt_postpone_dirty_displays;
   tt_postpone_dirty_displays = FALSE; // and don't postpone any of the following
   coordinate saved_destination_llx = tt_destination_llx,
				  saved_destination_lly = tt_destination_lly,
              saved_destination_urx = tt_destination_urx,
              saved_destination_ury = tt_destination_ury;
   set_clip_region(selection_feedback,region);
   Sprite::display_followers(selection_feedback,region);
   tt_destination_llx = saved_destination_llx;
   tt_destination_lly = saved_destination_lly;
   tt_destination_urx = saved_destination_urx;
   tt_destination_ury = saved_destination_ury;
   tt_main_window->set_clip_region();
   tt_postpone_dirty_displays = saved_postpone_dirty_displays;
};

Sprite *Picture::dereference() {
	if (is_flipped()) return(this);
//	if (robots_running_on_back()) { // moved this from inside the indirection != NULL conditional below on 150200
	if (remote_looks_of_me_count > 0) { // condition updated since this is a better measure of change than just robots on the back 
		// since if robots are running on the back they might change the appearance of this picture...
		// this isn't great since if robots on the back will never change appearance then why have robots
		// wait and schedule this differently??
		tt_touched_a_remote = ONLY_ON_NEW_CYCLE; // always?? -- don't really care which so long as it isn't NEVER
	};
	if (indirection != NULL) {
		return(indirection->dereference());
	};
	if (controlee != NULL) { // I'm a remote looks
      if (tt_system_mode != PUZZLE &&  // why????
			 active()) { // this conjunct added 301000
         // don't know what really is right here -- patch for now
		   tt_touched_a_remote = ONLY_ON_NEW_CYCLE; // always??
      };
		if (controlee->is_flipped()) { 
			// added this branch on 240101 since if remote looks it doesn't matter if the controlee is flipped
			Sprite *controlees_indirection = controlee->pointer_to_indirection();
			if (controlees_indirection != NULL) {
				return(controlees_indirection->dereference());
			} else {
				return(this); 
				// since controlee is flipped and shouldn't behave as if it is flipped - 
				// not sure if every call can deal with this right
			};
		} else {
			return(controlee->dereference());
		};
	};
	return(this);
};

/*
boolean Picture::draw_backing_and_frame() {
// new convention is that indirection will display backing anyway...
// bad idea (since indirection should be "transparent")
	if (indirection == NULL || indirection->kind_of() != PICTURE) {
		return(blank || (controlee == NULL && !rectangular()));
	} else {
		if (indirection->show_all()) { // maybe not due to context
			return(((Picture *) indirection)->draw_backing_and_frame());
		} else {
			return(FALSE);
		};
	};
};
*/

//void Picture::recursively_set_clip_region(SelectionFeedback selection_feedback) {
	// need to clip by all the regions that contain this
//	set_clip_region(selection_feedback);
//	Picture *level_below = leader_picture();
//	if (level_below != NULL) {
//		level_below->recursively_set_clip_region(selection_feedback);
//	};
//};

Picture *Picture::leader_picture() {
	if (subpicture() && leader != NULL && (leader->kind_of() == PICTURE)) { // || leader->kind_of() == REMOTE_PICTURE)) {
		// added leader->kind_of() == REMOTE_PICTURE on 090400 since might be room, roof, or house decoration
		// need to look into this since saving wall decoration has drift (if animated)
		return((Picture *) leader);
	} else {                       
		return(NULL);
   };
};

Sprites *postponed_collision_checks = NULL;

void Picture::update_display(const UpdateStatus update_status) {
	// width & height for top-level picture are truth
//	set_size_is_true(leader==NULL || leader->kind_of() != PICTURE ||
//					  code == SYNTHETIC_SPRITE);
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Updating display for debug target."));
	};
	add_to_log_if_interesting_error();
#endif
	if (pointer_to_top_of_stack() != NULL) {
		pointer_to_top_of_stack()->update_display(update_status);
		return;
	};
	if (!stopped() && !in_notebook()) { // second condition new on 080400
		city_coordinate new_llx = llx, new_lly = lly;
		if (x_speed != NULL && x_speed->sign() != 0) { // second condition added as an optimization on 290802
			Picture *level_below = leader_picture();
			double double_llx;
			double x_speed_as_double;
			x_speed->double_float_value(x_speed_as_double,TRUE); 
			if (level_below == NULL) {
				double_llx = llx+(x_speed_as_double*tt_millisecond_delta*milli_width)/1000;
			} else {
				double_llx = llx+(x_speed_as_double*((tt_millisecond_delta*level_below->current_width())/1000))/1000;
			};
			double_llx += llx_accumulated_error;
			new_llx = (city_coordinate) double_llx;
			llx_accumulated_error = double_llx-new_llx;
#if TT_DEBUG_ON
			if (tt_debug_mode == 151004) {
				tt_error_file() << "Ms delta is " << tt_millisecond_delta << " on frame " << tt_frame_number << endl;
				if (level_below == NULL) {
					tt_error_file() << "No level below." << endl;
				} else {
					level_below->describe_briefly(tt_error_file());
					tt_error_file() << " is below." << endl;
				};
				tt_error_file() << "Old llx is " << llx << endl;
				tt_error_file() << "Delta llx is " << (new_llx-llx) << endl;
				tt_error_file() << "X speed is " << x_speed_as_double << endl;
			};
#endif
		};
		if (y_speed != NULL && y_speed->sign() != 0) { // second condition added as an optimization on 290802
			Picture *level_below = leader_picture();
			double double_lly;
			double y_speed_as_double;
			y_speed->double_float_value(y_speed_as_double,TRUE); 
			if (level_below == NULL) {
				double_lly = lly+(y_speed_as_double*tt_millisecond_delta*milli_height)/1000;
			} else {
				double_lly = lly+(y_speed_as_double*((tt_millisecond_delta*level_below->current_height())/1000))/1000;
			};
			double_lly += lly_accumulated_error;
			new_lly = (city_coordinate) double_lly;
			lly_accumulated_error = double_lly-new_lly;
#if TT_DEBUG_ON
			if (tt_debug_mode == 151004) {
				tt_error_file() << "Old lly is " << lly << endl;
				tt_error_file() << "Delta lly is " << (new_lly-lly) << endl;
				tt_error_file() << "Y speed is " << y_speed_as_double << endl;
				tt_error_file() << "Ms delta is " << tt_millisecond_delta << endl;
				if (level_below == NULL) {
					tt_error_file() << "No level below." << endl;
				} else {
					level_below->describe_briefly(tt_error_file());
					tt_error_file() << " is below." << endl;
				};
			};
#endif
		};
      if (llx != new_llx || lly != new_lly) {
			// was non_zero_speed() prior to 290802 but this is faster and more straight forward
         move_to(new_llx,new_lly,DUE_TO_NON_ZERO_SPEED); // DUE_TO_NON_ZERO_SPEED added on 040302
      };
	};
	if (code == SCALE_SPRITE && indirection == NULL && active() && // active test added on 130201
//		 (leader == NULL || leader->kind_of() == CUBBY) && // commented out on 280403 so that it totters even if on a nest
		 update_status != UPDATE_TO_FIT_IN_CUBBY) {
      boolean relation_frozen = is_constant_scale();
		Relation relation = compute_relation(FALSE,!relation_frozen,TRUE);
      // if frozen then relationship is NO_RELATION if blanks encountered
      if (!relation_frozen || relation != constant_relation()) {
			compute_scale_value(relation);
		};
		Sprite::update_display(update_status);
		return;
	};
	// prior to 290199 the following ended with :UPDATE_CHANGE
	// changed so that indirections to flipped pictures don't change size
	// changed 050299 so blank one's don't change either
	UpdateStatus status = (is_flipped() || is_blank())?NO_UPDATE_CHANGE:update_status;
	if (leader == NULL) {
		if (animation_in_progress()) {
			Sprite::update_display(0,0,status);
		} else if (indirection != NULL) {
			Sprite::update_display(0,0,NO_UPDATE_CHANGE);
		} else if (code == HIT_OR_MISS_SPRITE) { // experiment on 101004 to partially restore this
			city_coordinate saved_llx = llx, 
								 saved_lly = lly;
			Sprite::update_display(0,0,status);
			// frame should move so restore
			llx = saved_llx;
			lly = saved_lly;
		} else {
			//city_coordinate saved_llx = llx, // commented out as experiment on 150704 -- without this animation of pictures that have different sizes 
			// e.g. helicopter animation bounce around
			//					 saved_lly = lly;
			Sprite::update_display(0,0,status);
//			picture_llx = llx;
//			picture_lly = lly;
//			image_delta_x += saved_llx-llx;
//			image_delta_y += saved_lly-lly;
			// frame should move so restore
			//llx = saved_llx;
			//lly = saved_lly;
		};
	} else {
//		picture_llx = llx;
//		picture_lly = lly;
		Sprite *outermost_container = ultimate_leader();
		if (outermost_container != NULL && 
			 indirection == NULL &&
			 !contained_in(THOUGHT_BUBBLE) && // added 280199 so not animating on back of a picture
			 (outermost_container->kind_of() == PICTURE ||
			  outermost_container->kind_of() == REMOTE_PICTURE ||
			  outermost_container->kind_of() == COPIER ||
           outermost_container->kind_of() == BIRD ||  // animated t-shirts are OK
           outermost_container->kind_of() == HOUSE ||  // animated decoration
           outermost_container->kind_of() == HOUSE_ABOVE)) {
			Sprite::update_display(0,0,status);
		} else { // otherwise don't keep cycling (distracting and CPU-consuming)
			Sprite::update_display(0,0,NO_UPDATE_CHANGE);
		};
	};	
	if (indirection != NULL) {
		CleanStatus indirect_status = indirection->current_clean_status();
		if (indirect_status != CLEAN && indirect_status != MOVED) {
//			 !indirection->last_size_change_due_to_indirection()) { // added on 140499
			set_clean_status(indirect_status);
			indirection->just_set_clean_status(CLEAN); // "transfered" the status ...
			// commented out the following on 150499 to get sizes right for explosion behavior
			if (!is_flipped() && !size_constrained()) {
//				if (tt_log_version_number > 14) { // changed on 010299
//					// changed from current_width etc to true_width on 140499 
				// to get the geometry right and avoid exponential growth by cycling
//					set_true_size_and_location_to_those_of(indirection,FOLLOWING_INDIRECTIONS_CHANGE);
//				if (tt_log_version_number < 15) {
//					set_size_and_location_to_those_of(indirection,FOLLOWING_INDIRECTIONS_CHANGE);
//				};
			};
		};
	};
	/* commented out on 161200 - new policy
	if (controlee != NULL) { // added 010299 so changes of size propogate properly  && tt_log_version_number > 14
		CleanStatus controlee_status = controlee->current_clean_status();
		if (controlee_status != CLEAN && controlee_status != MOVED) {
//			set_clean_status(controlee_status);
//       controlee->just_set_clean_status(CLEAN); // "transfered" the status ...
			if (!is_flipped() && !size_constrained() && !controlee->size_constrained()
				 && update_status != UPDATE_CHANGE_FROM_REMOTE) { // added on 220499  || tt_log_version_number < 15
            set_size(controlee->current_width(),controlee->current_height(),FOLLOWING_CONTROLEES_CHANGE);
			};
		};
	};
	*/
	if (update_status != UPDATE_TO_FIT_IN_CUBBY) { // conditional new on 311000
		update_remote(FALSE,update_status);
	};
// this caused double work -- propagate leaders will do this soon enough
//	if (!is_flipped() && unflipped_followers() != NULL) {
//		unflipped_followers()->update_display_all();
//	}; 
	if (!(is_flipped() && showing_on_a_screen()) && !no_flipped_followers() && !prepared_for_deletion()) {
		// !is_flipped() added 191299 since shouldn't do following if flipped
		// added showing_on_a_screen on 221299 since if flipped but on back of another picture then should run too
		// I suspect this is why sometimes robots were seen running after flipping
      // seems to get in trouble here if just readied for deletion
		if (suspended()) {
			set_suspended(FALSE);
			resume_all_robots();
		};
		// following is very heavy way to deal with robots working on cubbies which contain sensors and the like ...
		if (controlee == NULL) { // conditional new on 151200
			flipped_followers()->update_display_all();
		};
	};
// following was uncommented out and then commented out again on 121199 since it broke simple things like
// putting a ball on the top of a paddle and flipping it twice...
// following commented out since it causes geometry problems when a robot adds
// a picture to a picture after flipping it (to get sensors) 1/10/99
//	if (is_flipped() && tt_log_version_number > 14) {
		// so true_x_offset() is 0 so stuff stuck on back follows leader right
//		set_old_x_offset(0);
//		set_old_y_offset(0);
//	};
// don't do this for controlee's since the controlee may or may not
// be around and might even be that this is on the back of the controlee
//	Sprite *forward_to = forwarding_to();
//	if (forward_to != NULL) {
//		forward_to->update_display();
//		if (forward_to->pointer_to_followers() != NULL) {
//			 forward_to->pointer_to_followers()->update_display_all();
//		};
//	};
	if (indirection != NULL) {
//		if (tt_log_version_number > 14) {
			indirection->update_display(is_flipped()?NO_UPDATE_CHANGE:update_status); 
			// argument added 280199 so that when flipped size is fixed
			if (indirection->pointer_to_followers() != NULL) { // can it have any followers anymore??
				// argument added here 290199 so flipped pictures of indirections don't change size
				indirection->pointer_to_followers()->update_display_all(is_flipped()?NO_UPDATE_CHANGE:update_status);
			};
//		} else {
//			indirection->update_display();
//			if (indirection->pointer_to_followers() != NULL) {
//				 indirection->pointer_to_followers()->update_display_all();
//			};
//		};
	};
	if (controlee != NULL && !controlee->rectangular() && (leader == NULL || ultimate_leader()->kind_of() == CUBBY)) { 
		// new on 170301 so remote looks behave like other sensors
		set_clean_status(DIRTY);
	};
};

void Picture::update_remote(boolean regardless, UpdateStatus update_status) {
	// update_status new on 040504
#if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {
      log("update_remote of debug target.");
   };
#endif
	if (remote != NULL) { // no need to bother otherwise
		// is there some way to do the following in a loop??
		if (regardless || old_llx != llx) {
#if !TT_CORRECT_IMPLEMENTATION_OF_DISPLAY_X_AND_Y_AS_INTEGERS
			if (tt_x_and_y_display_as_integers) { // commented out on 230205 || llx_accumulated_error == 0.0) { 
				remote->changed(X_REMOTE,(user_coordinate) current_user_value_double(X_REMOTE));
			} else 
#endif
			{ // new on 040302
				RationalNumberValue new_value(current_user_value_double(X_REMOTE));
				remote->broadcast_new_value(X_REMOTE,&new_value,NULL);
			};		
		};
		if (regardless || old_lly != lly) {
#if !TT_CORRECT_IMPLEMENTATION_OF_DISPLAY_X_AND_Y_AS_INTEGERS
			if (tt_x_and_y_display_as_integers) { // commented out on 230205 || lly_accumulated_error == 0.0) { 
				remote->changed(Y_REMOTE,(user_coordinate) current_user_value_double(Y_REMOTE));
			} else 
#endif	
			{ // new on 040302
				RationalNumberValue new_value(current_user_value_double(Y_REMOTE));
				remote->broadcast_new_value(Y_REMOTE,&new_value,NULL);
			};		
		};
		// commented out on 030302 since speed is handled differently now
//		remote->set_speed_changed(old_x_speed != x_speed || old_y_speed != y_speed);
//		if (regardless || old_x_speed != x_speed) {
//			remote->changed(X_SPEED_REMOTE,x_speed);
//			old_x_speed = x_speed;
//		};
//		if (regardless || old_y_speed != y_speed) {
//			remote->changed(Y_SPEED_REMOTE,y_speed);
//			old_y_speed = y_speed;
//		};
		if (tt_log_version_number >= 67) { // re-organized on 130405
			if (regardless || old_width == 0) { // updated 110105
				// was old_height == 0 prior to 070405 but both should be zero together so shouldn't matter
				remote->changed(WIDTH_REMOTE,current_user_value(WIDTH_REMOTE));
				//if (tt_log_version_number >= 67 || update_status != UPDATE_INITIALIZE) { // condition new on 040504
				//	old_width = width;
				//};
			};
			if (old_width != width && (old_width >= 0 || regardless) && update_status != UPDATE_INITIALIZE) {
				// added last condition on 120405 so old demos (e.g. Pongact3_v2) run fine -- didn't help
				old_width = width; 
				// moved here from conditional above on 070405 since 
				// new conditions of 110105 caused collisions to be triggered incorrectly
			};
			if (regardless || old_height == 0) { // updated 110105
				remote->changed(HEIGHT_REMOTE,current_user_value(HEIGHT_REMOTE));
				//if (tt_log_version_number >= 67 || update_status != UPDATE_INITIALIZE) { // condition new on 040504
				//	old_height = height; // made unconditional on 110105
				//};
			};
			if (old_height != height && (old_height >= 0 || regardless) && update_status != UPDATE_INITIALIZE) {
				// added last condition on 120405 so old demos (e.g. Pongact3_v2) run fine -- didn't help
				old_height = height;
				// moved here from conditional above on 070405 since 
				// new conditions of 110105 caused collisions to be triggered incorrectly
			};
		} else { // the old way:
			if (regardless || old_height == 0 || old_width != width) { // updated 110105
				remote->changed(WIDTH_REMOTE,current_user_value(WIDTH_REMOTE));
				if (update_status != UPDATE_INITIALIZE) { // condition new on 040504
					old_width = width;
				};
			};
			if (regardless || old_height == 0 || old_height != height) { // updated 110105
				remote->changed(HEIGHT_REMOTE,current_user_value(HEIGHT_REMOTE));
				if (update_status != UPDATE_INITIALIZE) { // condition new on 040504
					old_height = height; // made unconditional on 110105
				};
			};
		};			
		if (regardless || old_parameter != current_parameter())  {
			old_parameter = current_parameter();
			remote->changed(PARAMETER_REMOTE,old_parameter%256); // added %256 on 161202
		};
		if (regardless || old_cycle_stopped != cycle_really_stopped()) {			
			old_cycle_stopped = (flag) !old_cycle_stopped;
			remote->changed(END_OF_ANIMATION_REMOTE,old_cycle_stopped);
		};
		if (regardless || old_grasped != (current_held_status() != NOT_HELD)) {
			old_grasped = (flag) !old_grasped;
			remote->changed(GRASPED_REMOTE,old_grasped);
		};
		if (regardless || old_selected_flag != (tt_selection == this)) {
			old_selected_flag = (flag) !old_selected_flag;
			remote->changed(SELECTED_REMOTE,(tt_selection == this));
		};
		if (released_flag) {
			released_last_frame = TRUE;
			released_flag = FALSE;
		} else if (released_last_frame) {
			released_last_frame = FALSE;
			remote->changed(RELEASED_REMOTE,FALSE);
		};
//		boolean postponed_collision_check = FALSE; // new on 290704
		if (!is_flipped() && remote->collision_status_wanted()) { //  || tt_log_version_number < 15
         // postpone them so that if 2 things collide both sensors go off
         // independent of order of update of position
			if (postponed_collision_checks == NULL || postponed_collision_checks->member_cons(this) == NULL) {
				// condition new on 261000
				increment_ref_count(); // new on 261000
				postponed_collision_checks = new Sprites(this,postponed_collision_checks);
//				if (tt_log_version_number >= 50) {
//					postponed_collision_check = TRUE; 
//			   };
			};
		};
		if (!clean_or_moved()) {
			remote->not_clean(current_clean_status());
		};
		if (update_status != UPDATE_INITIALIZE) { // && !postponed_collision_check) { 
			// condition new on 040504 to fix collision sensor problem with time travel
			// second condition added on 290704 since otherwise collision check won't see any movement
			// removed on 180804 since broke the Pong game in the Examples notebook
			old_llx = llx;
			old_lly = lly;
		};
	} else if (update_status != UPDATE_INITIALIZE) { // condition new on 040504 to fix collision sensor problem with time travel 
		// used by collision stuff, etc.
		old_llx = llx;
		old_lly = lly;
		old_width = width;
		old_height = height;
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
};

void Picture::set_x_speed(NumberValue *new_x_speed) { // new on 050302
#if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {
      log("Setting x speed of debug target.");
   };
#endif
	if (x_speed != NULL) delete x_speed;
	x_speed = new_x_speed;
	if (remote != NULL) {
		if (new_x_speed == NULL) {
			LongNumberValue zero(0);
			remote->broadcast_new_value(X_SPEED_REMOTE,&zero,NULL);
		} else {
			remote->broadcast_new_value(X_SPEED_REMOTE,new_x_speed,NULL);
		};
	};
};

void Picture::set_y_speed(NumberValue *new_y_speed) { // new on 050302
	if (y_speed != NULL) delete y_speed;
	y_speed = new_y_speed;
	if (remote != NULL) { 
		if (new_y_speed == NULL) {
			LongNumberValue zero(0);
			remote->broadcast_new_value(Y_SPEED_REMOTE,&zero,NULL);
		} else {
			remote->broadcast_new_value(Y_SPEED_REMOTE,new_y_speed,NULL);
		};
	};
};

// don't need to worry about the picture in postponed collision
// being destroyed since is run before the pictures are really
// deleted (which will set remote = NULL so the check won't happen)

void run_postponed_collision_checks() {
   if (postponed_collision_checks != NULL) {
      Sprites *remaining = postponed_collision_checks;
      while (remaining != NULL) {
			Picture *picture = (Picture *) remaining->first();
         picture->check_collisions();
			picture->destroy(); // release reference (new on 261000)
         remaining->set_first(NULL);
         remaining = remaining->rest();
      };
      delete postponed_collision_checks;
      postponed_collision_checks = NULL;
   };
};

void release_postponed_collision_checks() {
//   if (postponed_collision_checks != NULL) {
//      postponed_collision_checks->remove_all();
//      delete postponed_collision_checks;
//      postponed_collision_checks = NULL;
//   };
	// rewritten on 261000 since this now maintains reference counts
	destroy_sprites(postponed_collision_checks);
};

void Picture::check_collisions(boolean new_collidee) {
	// !is_flipped() added on 160499 so don't check sensors when flipped
   if (remote != NULL && !is_flipped() && //  || tt_log_version_number < 15)
		(leader == NULL || leader->kind_of() == PICTURE)) { 
		 // !contained_in(PROGRAM_PAD) && !contained_in(CUBBY)))) { 
		 // commented out the above with the following on 160200 since it sometimes is expensive but also
		 // could be a game running in a box
		// added PICTURE test on 170799 so if carried by bird or in truck doesn't check for collisions
		// added && !contained_in(NOTEBOOK) && !contained_in(CUBBY) on 110100 since why check if in a box or notebook
		remote->check_collisions(new_collidee);
	};
	if (tt_log_version_number >= 50) {
		set_previous_llx(llx); // new on 290704 since postponed
		set_previous_lly(lly);
	};
};

boolean Picture::cycle_really_stopped() {
	Sprite *forward = forwarding_to(TRUE);
	if (forward == NULL) {
		return(cycle_stopped());
	} else {
		return(forward->cycle_stopped());
	};
};

void Picture::resume_all_robots() {
	if (controlee != NULL) { // new on 171200
		controlee->resume_all_robots();
		return;
	};
	Sprites *remaining = flipped_followers();
	while (remaining != NULL) {
		if (remaining->first()->kind_of() == ROBOT) {
			Robot *robot = (Robot *) remaining->first();
//			if (tt_log_version_number > 14) {
			if (robot->suspended()) { // || (robot->waiting_for_release() && tt_log_version_number > 21)) {
				// added disjunct on 270300 BUT it looks like these are equivalent concepts now
				// commented it out 300300 (but after generating 2.02) because 
				// waiting_for_release() is always false if tt_log_version_number > 21)
				robot->activate();
			};
//			} else {
//				robot->really_try_clause();
//			};
		};
		remaining = remaining->rest();
	};
};

user_coordinate Picture::user_x(city_coordinate value) {
	Picture *level_below = leader_picture();
	if (level_below == NULL) {
		return(tt_screen->user_x(value));
	};
	city_coordinate level_below_width = level_below->current_width();
	if (level_below_width == 0) return(1000);
//	if (tt_log_version_number < 15) {
		// add value/2 to round to nearest integer -- this was WRONG
//		return((1000*value+value/2)/level_below_width);
//	} else {
	// should this use int64s??
		return((1000*value+level_below_width/2)/level_below_width);
//	};
};

user_coordinate Picture::user_y(city_coordinate value) {
	Picture *level_below = leader_picture();
	if (level_below == NULL) {
		return(tt_screen->user_y(value));
	};
	city_coordinate level_below_height = level_below->current_height();
	if (level_below_height == 0) return(1000);
//	if (tt_log_version_number < 15) {
		// add value/2 to round to nearest integer -- this was WRONG
//		return((1000*value+value/2)/level_below_height);
//	} else {
		return((1000*value+level_below_height/2)/level_below_height);
//	};
};

// new versions of the above to deal with floating point numbers (040302)
double Picture::user_x_double_float(double value) {
	Picture *level_below = leader_picture();
	if (level_below == NULL) {
		return(tt_screen->user_x_double_float(value));
	};
	city_coordinate level_below_width = level_below->current_width();
	if (level_below_width == 0) return(1000);
	return((1000*value)/level_below_width);
};

double Picture::user_y_double_float(double value) {
	Picture *level_below = leader_picture();
	if (level_below == NULL) {
		return(tt_screen->user_y_double_float(value));
	};
	city_coordinate level_below_height = level_below->current_height();
	if (level_below_height == 0) return(1000);
	return((1000*value)/level_below_height);
};

user_coordinate Picture::user_saved_width() { // isn't this a goofy name for this?? (observed on 211002)
	Picture *level_below = leader_picture();
	if (level_below == NULL) {
		return(tt_screen->user_x(saved_width?saved_width:width));
	};
	city_coordinate level_below_width = level_below->return_saved_width();
	if (level_below_width == 0) return(1000);
	return((1000*(saved_width?saved_width:width))/level_below_width);
};

user_coordinate Picture::user_saved_height() {
	Picture *level_below = leader_picture();
	if (level_below == NULL) {
		return(tt_screen->user_y(saved_height?saved_height:height));
	};
	city_coordinate level_below_height = level_below->return_saved_height();
	if (level_below_height == 0) return(1000);
	return((1000*(saved_height?saved_height:height))/level_below_height);
};


/* "new" version

user_coordinate Picture::user_saved_width() {
	Picture *level_below = leader_picture();
	if (level_below == NULL) {
		return(tt_screen->user_x(width)); //saved_width?saved_width:width));
	};
	return(1000);
//	city_coordinate level_below_width = level_below->return_saved_width();
//	if (level_below_width == 0) return(1000);
//	return((1000*width)/level_below_width); //(saved_width?saved_width:width))/level_below_width);
};

user_coordinate Picture::user_saved_height() {
	Picture *level_below = leader_picture();
	if (level_below == NULL) {
		return(tt_screen->user_y(height)); // saved_height?saved_height:height));
	};
	return(1000);
//	city_coordinate level_below_height = level_below->return_saved_height();
//	if (level_below_height == 0) return(1000);
//	return((1000*(saved_height?saved_height:height))/level_below_height);
};
*/

city_coordinate Picture::from_user_x(user_coordinate value) {
	Picture *level_below = leader_picture();
	if (level_below == NULL) {
		return(tt_screen->from_user_x(value));
	};
	// so user_x returns the original value
	city_coordinate level_below_width = level_below->current_width();
	// add 1/2 again to adjust for round off
	city_coordinate x1000 = value*level_below_width;
//	if (tt_log_version_number < 15) {
//		x1000 += level_below_width/2; // this was a bad way to deal with round off errors
//	};
//	return((x1000+x1000/2000)/1000);	// to round to nearest
	if (x1000%1000 == 0) {
		return(x1000/1000);
	} else if (x1000 < 0) { // new on 101202 so negative numbers around round up (towards negative infinity)
		return((x1000/1000)-1); // round up
	} else {
		return((x1000/1000)+1); // round up
	};
};

city_coordinate Picture::from_user_y(user_coordinate value) {
	Picture *level_below = leader_picture();
	if (level_below == NULL) {
		return(tt_screen->from_user_y(value));
	};
	city_coordinate level_below_height = level_below->current_height();
	city_coordinate y1000 = value*level_below_height;
//	if (tt_log_version_number < 15) {
//		y1000 += level_below_height/2; // this was a bad way to deal with round off errors
//	};
//	return((y1000+y1000/2000)/1000);
	if (y1000%1000 == 0) {
		return(y1000/1000);
	} else {
		return((y1000/1000)+1);
	};
};

city_coordinate Picture::from_user_x(double value) { // double version new on 110105
	Picture *level_below = leader_picture();
	if (level_below == NULL) {
		return(tt_screen->from_user_x(value));
	};
	double double_result = value*level_below->current_width()*0.001;
	if (double_result >= 0.0) {
		return((city_coordinate) (double_result+0.5)); // round up
	} else {
		return((city_coordinate) (double_result-0.5)); // round up negative number
	};
};

city_coordinate Picture::from_user_y(double value) { // double version new on 110105
	Picture *level_below = leader_picture();
	if (level_below == NULL) {
		return(tt_screen->from_user_y(value));
	};
	double double_result = value*level_below->current_height()*0.001;
	if (double_result >= 0.0) {
		return((city_coordinate) (double_result+0.5)); // round up
	} else {
		return((city_coordinate) (double_result-0.5)); // round up negative number
	};
};

user_coordinate Picture::current_user_value(RemoteIdentifier identifier) {
	switch (identifier) {
		case X_REMOTE: 
      case Y_REMOTE: {
//#if TT_DEBUG_ON
//         tt_error_file() << "Warning, caller should be updated to deal with non-integer values for the position of a picture." 
			// << endl;
//#endif
			double double_value = current_user_value_double(identifier);
			if (double_value > 0) {
				return((user_coordinate) (double_value+0.5)); // new on 100502 -- on 161202 switch to rounding rather than truncating
			} else {
				return((user_coordinate) (double_value-0.5)); // new on 100502 -- on 161202 switch to rounding rather than truncating
			};
                     };
      case X_SPEED_REMOTE:
      case Y_SPEED_REMOTE: { // new on 100502
#if TT_DEBUG_ON
         tt_error_file() << "Warning, caller should be updated to deal with non-integer values for the speed of a picture." 
								 << endl;
#endif
         NumberValue *value = current_speed(identifier,FALSE);
         long long_value;
         if (value == NULL) {
            long_value = 0; 
         } else {
            value->long_value(long_value);
         };
         return(long_value);
                           };
		case WIDTH_REMOTE:
			return(user_x(width));
		case HEIGHT_REMOTE:
			return(user_y(height));
		case PARAMETER_REMOTE:
         if (code == SYNTHETIC_SPRITE) {
            return(current_parameter()%256);
         } else {
			   return(current_parameter());
         };
		case END_OF_ANIMATION_REMOTE:
			return(cycle_really_stopped());
		case SELECTED_REMOTE:
			return(tt_selection == this);
		case RELEASED_REMOTE:
			return(FALSE);
		case GRASPED_REMOTE:
			return(current_held_status() != NOT_HELD);
//		case X_PENETRATION_REMOTE:
//			if (remote == NULL) { // can this happen?
//				return(0);
//			} else {
//				return(remote->current_x_penetration());
//			};
//		case Y_PENETRATION_REMOTE: //{
//			if (remote == NULL) { // can this happen?
//				return(0);
//			} else {
//				return(remote->current_y_penetration());
//			};
		case VISIBLE_REMOTE:
			if (visible()) {
				if (show_all()) return(0); // default as well
				return(1);
			};
			return(2);
		case HIT_OR_MISS_REMOTE:
		case X_HIT_OR_MISS_REMOTE:
		case Y_HIT_OR_MISS_REMOTE: // new on 250799 initialized as not hitting
			return(compute_picture_id(HIT_OR_MISS_SPRITE,NOT_COLLIDING));
      // globals are handled in GlobalPicture
	};
   return(0); // some remotes like the collidee remote don't care about this value
};

NumberValue *Picture::current_speed(RemoteIdentifier identifier, boolean copy) {
	NumberValue *result = NULL;
	switch (identifier) {
		case X_SPEED_REMOTE:
			result = current_x_speed();
			break;
		case Y_SPEED_REMOTE:
			result = current_y_speed();
			break;
#if TT_DEBUG_ON
		default: // shouldn't happen
			log("current_speed called with wrong identifier.");
#endif
	};
	if (copy) {
		if (result == 0) {
			return(new LongNumberValue(0));
		} else {
			return(result->copy());
		};
	} else {
		return(result);
	};
};

double Picture::current_user_value_double(RemoteIdentifier identifier) {
	switch (identifier) {
		case X_REMOTE: {
			Picture *level_below = leader_picture();
			city_coordinate below_llx = 0;
			if (level_below != NULL) {
				below_llx = level_below->current_llx();
			};
			return(user_x_double_float((llx+width*.5-below_llx)+llx_accumulated_error)); 
			// rewritten on 101202 to avoid round-off errors
			//user_coordinate x = user_x(llx+width/2-below_llx);
			//if (llx_accumulated_error == 0) { // if no error then try to keep the answer an integer
			//	return((double) x);
			//} else {
			//	return(x+user_x_double_float(llx_accumulated_error));
			//};
							};
		case Y_REMOTE: {
			Picture *level_below = leader_picture();
			city_coordinate below_lly = 0;
			if (level_below != NULL) {
				below_lly = level_below->current_lly();
			};
			return(user_y_double_float((lly+height*.5-below_lly)+lly_accumulated_error)); 
			// rewritten on 101202 to avoid round-off errors
			//user_coordinate y = user_y(lly+height/2-below_lly);
			//if (llx_accumulated_error == 0) { // if no error then try to keep the answer an integer
			//	return((double) y);
			//} else {
			//	return(y+user_y_double_float(lly_accumulated_error));
			//};
							};
		default:
#if TT_DEBUG_ON
			log("current_user_value_double called with wrong identifier.");
#endif
			return(0); 
	};
};

void Picture::speed_display_changed(RemoteIdentifier identifier, NumberValue *new_value) {
#if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {
      tt_error_file() << "speed_display_changed called on debug target with identifier " << (int) identifier << endl;
   };
#endif
	if (identifier == X_SPEED_REMOTE) {
		set_x_speed(new_value); 
	} else if (identifier == Y_SPEED_REMOTE) {
		set_y_speed(new_value);
	};
};

user_coordinate GlobalPicture::display_changed(RemoteIdentifier identifier, user_coordinate new_value) {
	switch (identifier) {
   	case HAND_VISIBLE_REMOTE:
			if (tt_programmer->kind_of() == PROGRAMMER_AT_FLOOR) {
				// new_value is whether visible
				tt_programmer->programmer_at_floor()->set_visibility((boolean) (new_value&1));
				return(new_value);
			} else { // following is new on 150499 so that if changed when not on floor nothing happens
				return(!tt_programmer->invisible());
			};
		case SOUND_ON_REMOTE:
			if (new_value&1) {
//				tt_sound_option |= SOUND_BIT;
				turn_on_sound(TRUE); // on 160301 this replaces above
			} else {
//				tt_sound_option &= ~SOUND_BIT;
				turn_off_sound(TRUE); // on 160301 this replaces above
			};
			tt_user_parameters->sound_option = tt_sound_option;
			remote->changed(SOUND_ON_REMOTE,new_value);
			dump_history();
			return(new_value);
		case LETTER_SIZE_REMOTE:
			if (new_value < 0) { // can't be negative so go back to positive
				new_value = -new_value;
			};
			tt_martian->new_character_size((unsigned short int) new_value);
			tt_user_parameters->character_size_percent = (unsigned short int) new_value;
			remote->changed(LETTER_SIZE_REMOTE,new_value);
			dump_history();
			return(new_value);
		case READING_SPEED_REMOTE:
			if (new_value < 0) { // can't be negative so go back to positive
				new_value = -new_value;
			};
			tt_help_speed_percent = (unsigned short int) new_value;
			tt_user_parameters->help_speed_percent = (unsigned short int) tt_help_speed_percent;
			remote->changed(READING_SPEED_REMOTE,new_value);
			dump_history();
			return(new_value);
		case MARTIAN_ADVICE_LEVEL_REMOTE:
			if (new_value > 100) {
				// new on 291104 since otherwise can become negative of greater than 128
				new_value = 100;
			};
			tt_minimum_say_importance = (char) new_value;
			tt_user_parameters->minimum_say_importance = (char) tt_minimum_say_importance;
			remote->changed(MARTIAN_ADVICE_LEVEL_REMOTE,new_value);
			dump_history();
			return(new_value);
		case PROGRAMMER_HEAD_REMOTE:
			new_value = set_programmer_head(new_value);
			if (remote != NULL) remote->changed(PROGRAMMER_HEAD_REMOTE,new_value);
			return(new_value);
      case MARTIAN_SPEAK_REMOTE:
			// marty_talk_options_count was 4 prior to 150903 
			// but that didn't support MARTY_SPEAKS_WITH_SUBTITLES, MARTY_WITH_SUBTITLES, and MARTY_NO_TALK
			new_value = new_value%marty_talk_options_count;  
			if (new_value < 0) new_value += marty_talk_options_count;
#if !TT_TEXT_TO_SPEECH
         if (new_value == MARTY_SPEAKS || tt_marty_talk == MARTY_SPEAKS_AND_TALK_BALLOONS || tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES) {
            new_value = MARTY_TALK_BALLOONS;
            say(IDS_MARTIAN_SPEAKS_ONLY_IN_32BITS,4);
         };
#else
			if (tt_marty_talk != new_value) {
            tt_marty_talk_to_save = (MartyTalk) new_value;
            tt_martian->new_talk_mode(tt_marty_talk_to_save);
         };
#endif
			tt_user_parameters->marty_talk = (char) tt_marty_talk;
			remote->changed(MARTIAN_SPEAK_REMOTE,new_value);
			dump_history();
			return(new_value);
		case SCREEN_SIZE_REMOTE:
			new_value = new_value%5;
			if (new_value < 0) new_value += 5;
//			tt_using_dispdib = new_value;
			// see how hard it is to make it happen
//			say(IDS_SORRY_WAIT_TIL_NEXT_TIME,4); // commented out on 070602
#if TT_DEBUG_ON
         if (tt_debug_mode == 130502) {
            tt_error_file() << "New SCREEN_SIZE_REMOTE is now " << new_value 
									 << " while tt_exclusive_screen is " << (int) tt_exclusive_screen << endl;
         };
#endif
         if (new_value == 1 && !tt_exclusive_screen) {
            tt_exclusive_screen_desired = TRUE;
            tt_restore_exclusive_screen = TRUE; // might never have been exclusive but that is OK
            tt_window_expansion_factor = 1; // just in case (new on 100602)
            tt_main_window->switch_to_full_screen(); // rewritten on 100602
            // tt_main_window->toggle_full_screen();
         } else if (new_value != 1 && tt_exclusive_screen) {
            tt_exclusive_screen_desired = FALSE;
            tt_restore_exclusive_screen = FALSE; 
            tt_main_window->toggle_full_screen();  
         };
 //        if (new_value != 1) { 
			// new on 100602 -- condition commented out on 140305 since even if 1 times need to update dimensions
            if (new_value == 0) {
               tt_window_expansion_factor = 1; // since 1 means full-screen
            } else {
               tt_window_expansion_factor = (unsigned char) new_value;
            };
            tt_main_window->set_window_size(tt_screen_width*tt_window_expansion_factor,
                                            tt_screen_height*tt_window_expansion_factor,
                                            TRUE); // center
				tt_main_window->compute_screen_dimensions(tt_screen_width,tt_screen_height); // new on 140305
//         };
			tt_user_parameters->using_dispdib = (char) new_value;
			remote->changed(SCREEN_SIZE_REMOTE,new_value);
			dump_history();
			return(new_value);
		case SYSTEM_SPEED_REMOTE:
			if (new_value < 0) { // can't be negative so go back to default
				new_value = -new_value;
			};
			tt_speed = (unsigned short int) new_value;
			tt_user_parameters->speed = (unsigned short int) tt_speed;
			remote->changed(SYSTEM_SPEED_REMOTE,new_value);
			dump_history();
			return(new_value);
		case CITY_SIZE_REMOTE:
			if (new_value < -255) {
            say(IDS_CITY_SIZE_CANT_BE_LESS_THAN,4);
				new_value = -255; // minus means add random houses
			} else if (new_value > 255) {
            say(IDS_CITY_SIZE_CANT_BE_MORE_THAN,4);
				new_value = 255;
			};
			// see how hard it is to make it happen
			say(IDS_SORRY_WAIT_TIL_NEXT_TIME,4);
//			tt_city_size = new_value;
			tt_user_parameters->city_size = (short int) new_value;
			remote->changed(CITY_SIZE_REMOTE,(short int) new_value);
			dump_history();
			return(new_value);
		case SYSTEM_SLEEP_REMOTE:
			tt_user_parameters->sleep = (unsigned short int) new_value;
			dump_history();
			tt_sleep = (unsigned short int) new_value;
			remote->changed(SYSTEM_SLEEP_REMOTE,new_value);
			return(new_value);
		case MOUSE_BUTTONS_REMOTE: {
			new_value = new_value%3;
			if (new_value <= 0) new_value += 3;
			tt_mouse_button_count = (char) new_value;
			tt_user_parameters->mouse_button_count = (char) new_value;
			remote->changed(MOUSE_BUTTONS_REMOTE,new_value);
			dump_history();
			return(new_value);
         };
		case LEGO_SERIAL_PORT_REMOTE: // ports 1 or 2 or 0 for off
#if TT_32
			say_error(IDS_LEGO_SERIAL_PORT_NOT_IN_32BIT);
#else
			new_value = new_value%3;
			if (new_value < 0) new_value += 3;
			if (tt_serial_port > 0) { // was on so reset stuff first
				for (int i = 0; i < tt_lego_ports; i++) set_motor_value(i,0);
				transmit_buffer_to_serial_port();
				close_serial_port(); // close old connection
			};
			tt_serial_port = (char) new_value;
			remote->changed(LEGO_SERIAL_PORT_REMOTE,new_value);
			if (tt_serial_port > 0)	open_serial_port(tt_serial_port);
#endif
			return(tt_serial_port);
		default: // new on 210301 - deal with Yes/No - e.g. '-' might have made it negative
			if (has_yes_or_no_values(identifier)) {
				new_value = new_value%2;
				if (new_value <= 0) new_value += 2;
			};
   };
   return(new_value);
};

boolean Picture::using_room_coordinates() { // abstracted and updated 111204
	// this is to deal with pictures left in room coordinates when the programmer isn't in the same house anymore
	return(leader == NULL && floor != NULL && tt_programmer->pointer_to_background() != floor);
};

user_coordinate Picture::display_changed(RemoteIdentifier identifier, user_coordinate new_value) {
#if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {
      tt_error_file() << "display_changed called on debug target with identifier " 
							 << (int) identifier << " and new_value " << new_value << endl;
   };
#endif
	boolean can_move = (leader == NULL || leader->is_picture()); // was kind_of() == PICTURE
	switch (identifier) {
		case PARAMETER_REMOTE:
         if (code == SYNTHETIC_SPRITE) { // only set the low-order byte -- rest used for shape
             short int new_parameter = current_parameter();
             unsigned char *bytes = (unsigned char *) &new_parameter;
             bytes[0] = (unsigned char) new_value;
             new_value = new_parameter;
         };
			if (set_parameter((short) new_value,FALSE,TRUE)) {
				new_value = current_parameter(); // added on 300399 since it may have changed (e.g. modulo)
            if (indirection != NULL) {
               indirection->indirection_of_set_parameter((short) new_value,FALSE,TRUE);
            };
				if (code == SYNTHETIC_SPRITE) {
					return(new_value%256); // just low-order byte
				} else {
               return(new_value);
				};
         };
			  // ignore bad values but bleep
			break;
//		case X_PENETRATION_REMOTE:
//		case Y_PENETRATION_REMOTE:
//			return(new_value); // nothing to do
		case VISIBLE_REMOTE:
			set_clean_status(DIRTY);
			new_value = new_value%3;
			if (new_value < 0) new_value += 3;
			switch (new_value) {
				case 0: // SEE ALL
					set_show_all(TRUE);
					set_visible(TRUE);
					break;
				case 1:
					set_show_all(FALSE);
					set_visible(TRUE);
					break;
				case 2:
					set_show_all(FALSE);
					set_visible(FALSE);
			};
         set_show_some_if_subpicture(FALSE); // since explictly set
			return(new_value);
		case HIT_OR_MISS_REMOTE:
		case X_HIT_OR_MISS_REMOTE:
		case Y_HIT_OR_MISS_REMOTE: 
			// should this (and the equivalent Java code) only work if value is changing from hit to no hit??
			if (remote != NULL) {
				remote->uncollide(new_value);
			};
			return(new_value);
// following handled as doubles as of 110105 if tt_log_version_number >= 67
		case X_REMOTE: {
			boolean need_to_restore_floor_coordinates = using_room_coordinates();
			if (need_to_restore_floor_coordinates) { 
				// new on 101204 to deal with sensors on ground or in another house -- NULL test new on 111204
				tt_programmer->translate_from_room_to_floor(this);
			};
			if (can_move) {
				Picture *level_below = leader_picture();
				if (level_below == NULL) {
					set_center_x(tt_screen->from_user_x(new_value)); 
					// added tt_screen-> on 070200 since that's what happens anyway - this skips a step
				} else {
					set_center_x(from_user_x(new_value)+level_below->current_llx());
				};
			};
			if (need_to_restore_floor_coordinates) { // restore -- new on 101204
				tt_programmer->translate_from_floor_to_room(this);
			};
			return(new_value); // moved here on 101204 so the above happens regardless
							};
			break;
		case Y_REMOTE: {
			boolean need_to_restore_floor_coordinates = using_room_coordinates();
			if (need_to_restore_floor_coordinates) { // new on 101204 to deal with sensors on ground or in another house
				tt_programmer->translate_from_room_to_floor(this);
			};
			if (can_move) {
				Picture *level_below = leader_picture();
				if (level_below == NULL) {
					set_center_y(tt_screen->from_user_y(new_value));
				} else {
					set_center_y(from_user_y(new_value)+level_below->current_lly());
				};
			};
			if (need_to_restore_floor_coordinates) { // restore -- new on 101204
				tt_programmer->translate_from_floor_to_room(this);
			};
			return(new_value); // moved here on 101204 so the above happens regardless
							};
			break;
		// speed moved to speed_display_changed
		case WIDTH_REMOTE: {
			if (new_value < 0) {
				new_value = 0; // new policy 110803 was -new_value; // negative width is just trouble
			};
			boolean need_to_restore_floor_coordinates = using_room_coordinates();
			if (need_to_restore_floor_coordinates) { // new on 101204 to deal with sensors on ground or in another house
				tt_programmer->translate_from_room_to_floor(this);
			};
			if (can_move) {
				double x; // changed to double on 070302
				if (tt_keep_center_when_size_changes) { // new on 280100 so keeps the center at the same place
					x = current_user_value_double(X_REMOTE);
				};
				set_width(from_user_x(new_value));
//            if (tt_log_version_number > 14) {
					set_saved_width(width); // to have the right value when saved...
//				} else {
//					saved_width = width;
//				};
				if (tt_keep_center_when_size_changes) {
//					set_center_x(from_user_x(x));
					display_changed(X_REMOTE,(user_coordinate) x); // new on 100200 since this works more generally than the above
				};		
			};
			if (need_to_restore_floor_coordinates) { // restore -- new on 101204
				tt_programmer->translate_from_floor_to_room(this);
			};
			return(new_value);
								 };
			break;
		case HEIGHT_REMOTE: {
			if (new_value < 0) {
				new_value = 0; // new policy 110803 was -new_value;
			};
			boolean need_to_restore_floor_coordinates = using_room_coordinates();
			if (need_to_restore_floor_coordinates) { // new on 101204 to deal with sensors on ground or in another house
				tt_programmer->translate_from_room_to_floor(this);
			};
			if (can_move) {
				double y; // changed to double on 070302
				boolean keep_centered = (tt_keep_center_when_size_changes && 
											    // following new on 210404 so growing things look fine when seen from helicopter
												 // why is this only for HEIGHT and not WIDTH as well? asked 230804
											    !(tt_screen->camera_status() == CAMERA_ABOVE && // && adjust_height_if_camera_above() &&
											      ultimate_leader()->kind_of() != HOUSE_ABOVE)); 
				if (keep_centered) { // new on 280100 so keeps the center at the same place
					y = current_user_value_double(Y_REMOTE);
				};
				set_height(from_user_y(new_value));
//            if (tt_log_version_number > 14) {
					set_saved_height(height);
//				} else {
//					saved_height = height;
//				};
				if (keep_centered) {
//					set_center_y(from_user_y(y));
					display_changed(Y_REMOTE,(user_coordinate) y); // new on 100200 since this works more generally than the above
				};			
			};
			if (need_to_restore_floor_coordinates) { // restore -- new on 101204
				tt_programmer->translate_from_floor_to_room(this);
			};
			return(new_value); // moved here on 101204 so the above happens regardless
								  };
			break;
  };
  // get here for collision detectors... maybe more
  return(new_value);
};

double Picture::display_changed(RemoteIdentifier identifier, double new_value) {
	// new on 110105 to make a version that works with doubles
#if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {
      tt_error_file() << "display_changed called on debug target with identifier " 
							 << (int) identifier << " and new_value " << new_value << endl;
   };
#endif
	if (tt_programmer != NULL && tt_programmer->training() && !showing_on_a_screen()) {
		return(new_value); 
		// new on 240205 to ignore changes to the controlee if training and picture is on floor (not in thought bubble)
	};
	boolean can_move = (leader == NULL || leader->is_picture()); // was kind_of() == PICTURE
	switch (identifier) {
		case X_REMOTE: {
			boolean need_to_restore_floor_coordinates = using_room_coordinates();
			if (need_to_restore_floor_coordinates) {
				// new on 101204 to deal with sensors on ground or in another house -- NULL test new on 111204
				tt_programmer->translate_from_room_to_floor(this);
			};
			if (can_move) {
				Picture *level_below = leader_picture();
				if (level_below == NULL) {
					set_center_x(tt_screen->from_user_x(new_value)); 
					// added tt_screen-> on 070200 since that's what happens anyway - this skips a step
				} else {
					set_center_x(from_user_x(new_value)+level_below->current_llx());
				};
			};
			if (need_to_restore_floor_coordinates) { // restore -- new on 101204
				tt_programmer->translate_from_floor_to_room(this);
			};
			return(new_value); // moved here on 101204 so the above happens regardless
							};
			break;
		case Y_REMOTE: {
			boolean need_to_restore_floor_coordinates = using_room_coordinates();
			if (need_to_restore_floor_coordinates) { // new on 101204 to deal with sensors on ground or in another house
				tt_programmer->translate_from_room_to_floor(this);
			};
			if (can_move) {
				Picture *level_below = leader_picture();
				if (level_below == NULL) {
					set_center_y(tt_screen->from_user_y(new_value));
				} else {
					set_center_y(from_user_y(new_value)+level_below->current_lly());
				};
			};
			if (need_to_restore_floor_coordinates) { // restore -- new on 101204
				tt_programmer->translate_from_floor_to_room(this);
			};
			return(new_value); // moved here on 101204 so the above happens regardless
							};
			break;
		// speed moved to speed_display_changed
		case WIDTH_REMOTE: {
			if (new_value < 0) {
				new_value = 0; // new policy 110803 was -new_value; // negative width is just trouble
			};
			boolean need_to_restore_floor_coordinates = using_room_coordinates();
			if (need_to_restore_floor_coordinates) { // new on 101204 to deal with sensors on ground or in another house
				tt_programmer->translate_from_room_to_floor(this);
			};
			if (can_move) {
				double x; // changed to double on 070302
				if (tt_keep_center_when_size_changes) { // new on 280100 so keeps the center at the same place
					x = current_user_value_double(X_REMOTE);
				};
				set_width(from_user_x(new_value));
//            if (tt_log_version_number > 14) {
					set_saved_width(width); // to have the right value when saved...
//				} else {
//					saved_width = width;
//				};
				if (tt_keep_center_when_size_changes) {
//					set_center_x(from_user_x(x));
					display_changed(X_REMOTE,(user_coordinate) x); // new on 100200 since this works more generally than the above
				};		
			};
			if (need_to_restore_floor_coordinates) { // restore -- new on 101204
				tt_programmer->translate_from_floor_to_room(this);
			};
			return(new_value);
								 };
			break;
		case HEIGHT_REMOTE: {
			if (new_value < 0) {
				new_value = 0; // new policy 110803 was -new_value;
			};
			boolean need_to_restore_floor_coordinates = using_room_coordinates();
			if (need_to_restore_floor_coordinates) { // new on 101204 to deal with sensors on ground or in another house
				tt_programmer->translate_from_room_to_floor(this);
			};
			if (can_move) {
				double y; // changed to double on 070302
				boolean keep_centered = (tt_keep_center_when_size_changes && 
											    // following new on 210404 so growing things look fine when seen from helicopter
												 // why is this only for HEIGHT and not WIDTH as well? asked 230804
											    !(tt_screen->camera_status() == CAMERA_ABOVE && // && adjust_height_if_camera_above() &&
											      ultimate_leader()->kind_of() != HOUSE_ABOVE)); 
				if (keep_centered) { // new on 280100 so keeps the center at the same place
					y = current_user_value_double(Y_REMOTE);
				};
				set_height(from_user_y(new_value));
//            if (tt_log_version_number > 14) {
					set_saved_height(height);
//				} else {
//					saved_height = height;
//				};
				if (keep_centered) {
//					set_center_y(from_user_y(y));
					display_changed(Y_REMOTE,(user_coordinate) y); // new on 100200 since this works more generally than the above
				};			
			};
			if (need_to_restore_floor_coordinates) { // restore -- new on 101204
				tt_programmer->translate_from_floor_to_room(this);
			};
			return(new_value); // moved here on 101204 so the above happens regardless
								  };
			break;
  };
  return(new_value);
};

void Picture::set_saved_width(city_coordinate w) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		tt_error_file() << "Setting saved width of debug target to " << w << endl;
	};
	if (tt_toolbox != NULL && toolbox_tool(this)) {
		log("Why is something inside of Tooly getting its saved_width set?");
	};
#endif
	if (floor != NULL && floor->in_the_room_with_programmer()) { // new on 020205
		w = tt_programmer->translate_room_to_floor_width(w);
	};
	saved_width = w;
	if (indirection != NULL && indirection->kind_of() == PICTURE) { // added on 010299 to help keep pictures and remotes in synch
		((Picture *) indirection)->set_saved_width(w);
	};
};

void Picture::set_saved_height(city_coordinate h) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		tt_error_file() << "Setting saved hight of debug target to " << h << endl;
	};
#endif
	if (floor != NULL && floor->in_the_room_with_programmer()) { // new on 020205
		h = tt_programmer->translate_room_to_floor_height(h);
	};
	saved_height = h;
	if (indirection != NULL && indirection->kind_of() == PICTURE) { // added on 010299 to help keep pictures and remotes in synch
		((Picture *) indirection)->set_saved_height(h);
	};
};

void Picture::set_cause_of_match_failure(boolean new_flag, boolean recur) {
	// new on 220902 so glow red appropriately if indirect picture
	Sprite::set_cause_of_match_failure(new_flag,recur);
	if (indirection != NULL) {
		indirection->set_cause_of_match_failure(new_flag,recur);
	};
};

string Picture::private_media_file_name(boolean &worth_compressing, boolean full_path) {
	if (pointer_to_current_image() == NULL) {
		return(NULL);
	};
	hash image_hash = pointer_to_current_image()->return_hash();
	if (image_hash == NULL) return(NULL);
	string file_name = new char[MAX_PATH];
	if (full_path) {
		media_file_name(image_hash,".png",file_name,tt_private_media_directory,tt_private_media_directory_length);
	} else {
		media_file_name(image_hash,".png",file_name);
	};
	worth_compressing = FALSE;
	return(file_name);
};

void Picture::set_private_media_file_name(string name) {
	if (pointer_to_current_image() == NULL) return;
	pointer_to_current_image()->set_hash(string_to_hash(name));
//	current_image->set_stored_in_private_media_directory(TRUE);
};

hash Picture::return_hash() {
	if (pointer_to_current_image() == NULL) {
		return(NULL);
	} else {
		return(pointer_to_current_image()->return_hash());
	};
};

void Picture::translated_from_floor_to_room() { // created and made obsolete on 040603
	if (width_before_flip > 0) {
		tt_programmer->translate_floor_to_room_dimensions(width_before_flip,height_before_flip);
	};
};

void Picture::translated_from_room_to_floor() { // created and made obsolete on 040603
	if (width_before_flip > 0) {
		tt_programmer->translate_room_to_floor_dimensions(width_before_flip,height_before_flip);
	};
};

// commented out on 010299 since the sizes are now kept in synch...
/*
city_coordinate Picture::true_width(boolean including_followers) {
	if (tt_log_version_number > 14) {
		return(Sprite::true_width(including_followers));
	} else {
	   if (indirection != NULL) {
			return(indirection->true_width(including_followers));
		} else {
			return(Sprite::true_width(including_followers));
		};
	};
};

city_coordinate Picture::true_height(boolean including_followers) {
	if (tt_log_version_number > 14) {
		return(Sprite::true_height(including_followers));
	} else {
		if (indirection != NULL) {
			return(indirection->true_height(including_followers));
		} else {
			return(Sprite::true_height(including_followers));
		};
	};
};
*/

void Picture::set_inside_vacuum(boolean new_flag, boolean recur) {
//   if (inside_vacuum_flag == new_flag) return; 
	// new on 170702 to ignore calls that already know about -- 
	// commented out on 190702 since might not have been called with recur earlier
	boolean changed = (!!inside_vacuum_flag != !!new_flag); // force to true boolean before comparing
   Sprite::set_inside_vacuum(new_flag,recur);
   if (remote != NULL && changed) { // added second condition on 190702
      remote->changed_vacuum_status(new_flag);
   };
	if (new_flag && changed) {
		stop_all();
	};
   if (recur) { // condition added 190702
      Sprites *remaining = pointer_to_followers_on_other_side(); // new on 170702
	   while (remaining != NULL) {
		   remaining->first()->set_inside_vacuum(new_flag,recur);
		   remaining = remaining->rest();
	   };
   };
};

void Picture::set_in_notebook(Notebook *notebook, boolean recur) { // new on 170702 so both sides get updated
	Sprite::set_in_notebook(notebook,recur);
   if (controlee != NULL && notebook != NULL) {
      // new on 210802 so remote looks in notebooks have the right controlee
      Picture *picture = notebook->pointer_to_picture();
      if (picture != NULL) {
         set_controlee(picture);
         if (is_flipped()) { // old versions saved things this way
            flip();
         };
      };
   };
   if (recur) { // condition added 190702
	   Sprites *remaining = pointer_to_followers_on_other_side();
	   while (remaining != NULL) {
			Sprite *follower = remaining->first();
			follower->set_in_notebook(notebook,recur);
		   remaining = remaining->rest();
	   };
   };
};

Sprite *Picture::next_to_collide(TTRegion &region, Sprite *last_one, PredicateCallBack predicate, void *predicate_arg, 
											boolean loop_around_if_not_found) {
   // used to call next_tool_true_region_to_collide_with
	if (is_flipped()) {
//      if (tt_log_version_number >= 7) {
		   return(tt_screen->
						next_tool_full_collision_region_to_collide_with(&region,this,last_one,predicate,predicate_arg,TRUE,
																						loop_around_if_not_found));
//      } else {
//         return(tt_screen->next_tool_true_region_to_collide_with(&region,this,last_one,predicate));
//      };
	};
	Picture *bottom_level = NULL;
   Sprite *next_level = leader;
	while (next_level != NULL && next_level->kind_of() == PICTURE) {
		bottom_level = (Picture *) next_level; // was ultimate_leader();
      if (!bottom_level->visible()) { // prior to Sept 1 '98 visible() was show_all()
         // if just show some then followers interact at next level up
         bottom_level = NULL; // ignore this level
         next_level = next_level->pointer_to_leader();
      } else break;
	};
	if (bottom_level == NULL) {
// following seems the wrong policy -- commented out on 10/15/97
//		Sprite *on_top = NULL;
//		if (followers != NULL) {
//			on_top = next_to_collide_with(region,this,last_one,FALSE,predicate);
//		};
//		if (on_top != NULL) return(on_top);
//      if (tt_log_version_number >= 7) {
		   return(tt_screen->
						next_tool_full_collision_region_to_collide_with(&region,this,last_one,predicate,predicate_arg,TRUE,loop_around_if_not_found));
//      } else {
//         return(tt_screen->next_tool_true_region_to_collide_with(&region,this,last_one,predicate));
//      };
	} else {
		return(bottom_level->next_to_collide_with(region,this,last_one,FALSE,predicate,predicate_arg,loop_around_if_not_found));
	};
};

Sprite *Picture::next_to_collide_with(TTRegion &target_region, Sprite *exception,
												  Sprite *starting_after,
												  boolean want_flipped, PredicateCallBack predicate, void *predicate_arg,
												  boolean loop_around_if_not_found) { // added 290105
   if (tt_log_version_number < 69) {
		loop_around_if_not_found = FALSE; // new on 310105 since this wasn't working earlier
	};
	Sprites *followers_to_check;
	if (is_flipped() == want_flipped) {
		followers_to_check = pointer_to_followers();
	} else {
		followers_to_check = followers_on_other_side;
	};
	boolean first_round = TRUE; // new on 290105 to get it check for earlier parts as well
	Sprites *remaining = followers_to_check;
	if (starting_after != NULL) {
		remaining = remaining->member_cons(starting_after);
		if (remaining != NULL) {
			remaining = remaining->rest();
		};
		if (remaining == NULL) {
			if (loop_around_if_not_found && first_round) {
				// new on 290105 to loop around right away if already at the end of possibilities
				first_round = FALSE;
				remaining = followers_to_check;
			};
		};
	};
	TTRegion this_region;
#if TT_DEBUG_ON
	if (tt_debug_mode == 2612) {
		tt_error_file() << "Checking region collision on frame " << tt_frame_number << " "
							 << target_region.min_x << "," << target_region.min_y << " to "
							 << target_region.max_x << "," << target_region.max_y << endl;
	};
#endif
	while (remaining != NULL) {
		 Sprite *collidee = remaining->first();
       if (collidee->visible()) {
// commented out the conditional on Sept 1 '98
//          if (collidee->show_all()) {  //  || tt_log_version_number <= 9
//             if (tt_log_version_number >= 7) {
		          collidee->full_collision_region(this_region);
//             } else {
//                collidee->true_region(this_region); // was collision_region once
//             };
#if TT_DEBUG_ON
		       if (tt_debug_mode == 2612) {
			       tt_error_file() << "with "
										  << this_region.min_x << "," << this_region.min_y << " to "
										  << this_region.max_x << "," << this_region.max_y << endl;
		       };
#endif
		       if (target_region.overlaps(this_region) &&
			        collidee != exception &&
			        (predicate == NULL || predicate(collidee,predicate_arg))) {
#if TT_DEBUG_ON
			       if (tt_debug_mode == 2612) {
				       tt_error_file() << "overlaps" << endl;
			       };
#endif
			       return(collidee);
              };
// following is really hard if collidee is SHOW SOME and has followers
// since they should be explored and it is hard to make a generator for them...
//          } else { // show some so add collidee's followers to remaining
//              remaining = remaining->append(collidee->pointer_to_followers());
//          };
		 };
		 if (collidee == starting_after) { // could only happen if second round and didn't find anything -- new on 290105
			 return(NULL);
		 };
		 remaining = remaining->rest();
		 if (remaining == NULL && first_round && loop_around_if_not_found) {
			 first_round = FALSE;
			 remaining = followers_to_check;
		 };
	};
	return(NULL);
};

void Picture::full_collision_region(TTRegion &region) {
	full_region(region);
	if (remote != NULL && old_width != min_long) { // second condition added on 070800 to check if old values are set
		// extend to include stuff passed thru since last frame
		if (old_llx < region.min_x) {
			region.min_x = old_llx;
		};
		if (old_llx+old_width > region.max_x) {
			region.max_x = old_llx+old_width;
		};
		if (old_lly < region.min_y) {
			region.min_y = old_lly;
		};
		if (old_lly+old_height > region.max_y) {
			region.max_y = old_lly+old_height;
		};
	};
};

boolean Picture::add_remote(RemoteIdentifier identifier, Sprite *remote_integer) {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log("add_remote of debug target.");
	};
#endif
	if (remote == NULL) {
		remote = new Remote(this);
	};
	return(remote->add_remote(identifier,remote_integer));
};

void Picture::remove_remote(RemoteIdentifier identifier, Sprite *remote_item) {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log("remove_remote of debug target.");
	};
#endif
	if (remote != NULL) remote->remove_remote(identifier,remote_item);
};

//user_coordinate a_bit = 10; // 10/1000 or 1 percent of screen seems ok

//user_coordinate a_bit_more(user_coordinate x) {
  // penetration needs a bit extra to measure how far to go to get out
//  if (x == 0) {
//	  return(0);
//  } else if (x < 0) {
//	  return(x-a_bit);
//  } else {
//	  return(x+a_bit);
//  };
//};

CollisionDirection Picture::best_penetration(Sprite *collidee,
															city_coordinate &x_penetration,
															city_coordinate &y_penetration) {
	// should be faster to just compute it directly...
	city_coordinate best_penetration;
	CollisionDirection best_direction; // = NORTH_COLLISION;
	compute_penetration(collidee,NORTH_COLLISION,x_penetration,y_penetration);
	city_coordinate new_x_penetration,new_y_penetration;
	compute_penetration(collidee,SOUTH_COLLISION,new_x_penetration,new_y_penetration);
	if (labs(new_y_penetration) < labs(y_penetration)) {
		best_penetration = new_y_penetration;
		best_direction = SOUTH_COLLISION;
		x_penetration = new_x_penetration;
		y_penetration = new_y_penetration;
	} else {
		best_penetration = y_penetration;
		best_direction = NORTH_COLLISION; 
	};
	compute_penetration(collidee,EAST_COLLISION,new_x_penetration,new_y_penetration);
	if (labs(best_penetration) > labs(new_x_penetration)) {
		best_penetration = new_x_penetration;
		best_direction = EAST_COLLISION;
		x_penetration = new_x_penetration;
		y_penetration = new_y_penetration;
	};
	compute_penetration(collidee,WEST_COLLISION,new_x_penetration,new_y_penetration);
	if (labs(best_penetration) > labs(new_x_penetration)) {
//		best_penetration = new_x_penetration;
		best_direction = WEST_COLLISION;
		x_penetration = new_x_penetration;
		y_penetration = new_y_penetration;
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 45) {
		tt_error_file() << "Computed best direction " << best_direction
						<< " on frame " << tt_frame_number << endl;
	};
#endif
	return(best_direction);
};

void Picture::compute_penetration(Sprite *other, CollisionDirection direction,
											 city_coordinate &x, city_coordinate &y) {
	// on 180901 commented out one pixel adjustments - more appropriate for caller to do something
	// added min and max on 190503 so that if things have moved or there is round off error no movement happens
	// removed min and max on 230503 since callers now deal with negative penetration
	switch (direction) {
		case WEST_COLLISION:
			x = current_llx()-other->true_urx(); // +tt_screen->one_pixel_in_x_city_units();
			y = 0;
			break;
		case EAST_COLLISION:
			x = true_urx()-other->current_llx(); // -tt_screen->one_pixel_in_x_city_units();
			y = 0;
			break;
		case SOUTH_COLLISION:
			y = current_lly()-other->true_ury(); // -tt_screen->one_pixel_in_y_city_units();
			x = 0;
			break;
		case NORTH_COLLISION:
			y = true_ury()-other->current_lly(); // +tt_screen->one_pixel_in_y_city_units();
			x = 0;
			break;
		case NO_COLLISION:
			x = 0;
			y = 0;
			break;
	};
};

void Picture::remote_notebook_added(Notebook *notebook) {
	if (remote != NULL) {
		remote->remote_notebook_added(notebook);
	};
};	

void Picture::remote_notebook_deleted(Notebook *notebook) {
	if (remote != NULL) {
		remote->remote_notebook_deleted(notebook);
	};
};

void Picture::controlee_deleted() {
	set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL); 
	// new on 070400 since if remote looks and original is deleted this needs some reasonable size
	set_controlee(NULL);
	code = SYNTHETIC_SPRITE; // since is now "dead" - new on 191200
	set_parameter(tt_black); // was 255 -- is white better??
};

void Picture::recursively_notice_on_screen_change(boolean on_screen) {
	if (indirection != NULL) {
		indirection->recursively_notice_on_screen_change(on_screen);
	} else {
		Sprite::recursively_notice_on_screen_change(on_screen);
	};
};

void Picture::notice_on_screen_change(boolean on_screen) {
   if (indirection != NULL) {
      indirection->notice_on_screen_change(on_screen);
   };
	if (!on_screen) {
		Sprite::notice_on_screen_change(on_screen);
		if (remote != NULL && !tt_resetting_or_destroying_city) {
			// tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 
			// but can cause memory errors while resetting during time travel
			remote->no_longer_on_screen();
		};
	};
};

//void Picture::become(Sprite *other) { // new on 100901 // noticed obsolete on 140205
//	if (other == NULL || other->kind_of() != PICTURE) return;
//	Sprite::become(other);
//	remove_all_flipped_followers(TRUE);
//	Picture *other_picture = (Picture *) other;
//	Sprites *remaining = other_picture->flipped_followers();
//	Sprite *previous_flipped_follower = NULL;
//	while (remaining != NULL) {
//		Sprite *flipped_follower = remaining->first();
//		other_picture->remove_flipped_follower(flipped_follower);
//		add_flipped_follower(flipped_follower,previous_flipped_follower,this);
//		previous_flipped_follower = flipped_follower;
//		remaining = remaining->rest();
//	};
//};

boolean Picture::on_backside() { // new on 130702
   if (pointer_to_background() != NULL) {
      return (pointer_to_background()->pointer_to_picture() != NULL);
   };
   return(FALSE);
};

void Picture::just_set_original_file_name(string file_name) {
	if (pointer_to_current_image() != NULL) {
//#if TT_DEBUG_ON
//		if (file_name[0] < 'A' || file_name[0] > 'z') {
//			log("debug this");
//		};
//#endif
		pointer_to_current_image()->just_set_original_file_name(file_name);
	}; // else warn??
};

int number_of_user_images =0;
int max_number_of_user_images = 0;
string *user_image_file_names = NULL ;
UserImagePointer *user_images = NULL ;
const int user_image_table_increment = 50;

void grow_user_image_table() {
	int old_max_number_of_user_images = max_number_of_user_images;
	max_number_of_user_images += user_image_table_increment;
	UserImagePointer *new_user_images = new UserImagePointer[max_number_of_user_images];
	string *new_user_image_file_names = new string[max_number_of_user_images];
	int i = 0;
	for (;i < old_max_number_of_user_images; i++) {
		new_user_images[i] = user_images[i];
		new_user_image_file_names[i] = user_image_file_names[i];
	};
	if (user_images != NULL) {
		delete [] user_images;
	};
	user_images = new_user_images;
	if (user_image_file_names != NULL) {
		delete [] user_image_file_names;
	};
	user_image_file_names = new_user_image_file_names;
};

void release_user_image_table() {
	if (user_images != NULL) {
		for (int i = 0; i < number_of_user_images; i++) {
			delete user_images[i];
		};
		delete [] user_images;
		user_images = NULL;
	};
	if (user_image_file_names != NULL) {
		for (int i = 0; i < number_of_user_images; i++) {
			delete [] user_image_file_names[i];
		};
		delete [] user_image_file_names;
		user_image_file_names = NULL;
	};
	number_of_user_images = 0;
	max_number_of_user_images = 0;
};

#if TT_DEBUG_ON
//millisecond time_to_find_images = 0;
long image_count = 0;
LARGE_INTEGER high_res_start;
LARGE_INTEGER high_res_stop;
int64 high_res_total = 0;
LARGE_INTEGER high_res_frequency;
#endif

void associate_image_and_file_name(UserImage *image, string file_name) { // abstracted on 250403
	if (number_of_user_images >= max_number_of_user_images) {
		grow_user_image_table();
	};
	user_image_file_names[number_of_user_images] = copy_string(file_name);
	user_images[number_of_user_images] = image;
	number_of_user_images++;
};

UserImage *find_image_for_file(ascii_string private_media_file_name, ascii_string original_file_name, 
										 boolean &ok, boolean warn, int pixel_width, int pixel_height) {
	// removed , boolean share_images_with_same_file_name on 280403
#if TT_DEBUG_ON
//	millisecond start_time = timeGetTime();
	if (tt_debug_mode == 2609001) {
		QueryPerformanceCounter(&high_res_start);
		image_count++;
	};
#endif
	if (private_media_file_name != NULL) { // currently compares full path names (noticed 080403) revisit 100403
		for (int i = 0; i < number_of_user_images; i++) {
			if (user_image_file_names[i] != NULL && strcmp(private_media_file_name,user_image_file_names[i]) == 0) {
				UserImage *result = user_images[i];
				result->increment_ref_count(); // not currently used
				ok = TRUE;
#if TT_DEBUG_ON
//			time_to_find_images += timeGetTime()-start_time;
				if (tt_debug_mode == 2609001) {
					QueryPerformanceCounter(&high_res_stop);
					QueryPerformanceFrequency(&high_res_frequency);
					high_res_total += (1000000*(high_res_stop.QuadPart-high_res_start.QuadPart))/high_res_frequency.QuadPart;
					tt_error_file() << "Total microseconds to find images: " << (long) high_res_total << " (this one is old)" << endl;
				};
#endif
				return(result);
			};
		};
	};
	UserImage *result = new UserImage();
   ok = result->set_file_name(private_media_file_name,original_file_name,warn,pixel_width,pixel_height);
	if (ok && private_media_file_name != NULL) {
		associate_image_and_file_name(result,private_media_file_name);
	};
#if TT_DEBUG_ON
//	time_to_find_images += timeGetTime()-start_time;
	if (tt_debug_mode == 2609001) {
		QueryPerformanceCounter(&high_res_stop);
		QueryPerformanceFrequency(&high_res_frequency);
		high_res_total += (1000000*(high_res_stop.QuadPart-high_res_start.QuadPart))/high_res_frequency.QuadPart;
		tt_error_file() << "Total microseconds to find images: " << (long) high_res_total << endl;
	};
#endif
	return(result);
};

UserPicture::UserPicture(ascii_string original_file_name, boolean &ok, 
								 boolean warn, int pixel_width, int pixel_height,
								 ascii_string private_media_file_name) : 
  // warn added 200201 // pixel dimensions added 301002
  // added share_images_with_same_short_file_name on 230303
  Picture(0,0,0,0,USER_SPRITE) {
   set_size_is_true(TRUE);
	current_image = find_image_for_file(private_media_file_name,original_file_name,ok,warn,pixel_width,pixel_height); 
	// new on 201199 so these are shared
//#if TT_DEBUG_ON
//	if (strcmp(file_name,"flash") == 0) {
//		log("debug this");
//	};
//#endif
//   ok = current_image->set_file_name(file_name);
   if (ok) {
      set_size(pointer_to_current_image()->width_without_scaling(),
               pointer_to_current_image()->height_without_scaling());
		saved_width = width;
		saved_height = height;
		if (private_media_file_name != NULL && return_hash() == NULL) { 
			// new on 271204 since otherwise have troubles with transparency when copying since given private_media_file_name
			// but there is no hash
			string short_name = short_file_name(private_media_file_name);
			set_private_media_file_name(short_name);
			delete [] short_name;
		};
   };
	if (original_file_name != NULL) { // new on 280403
		just_set_original_file_name(copy_string(original_file_name));
	};
};

UserPictureFileMissing::UserPictureFileMissing(ascii_string file_name, xml_node *node) : 
  // new on 290103 - this is a picture (not an indirection to text) and dumps specially
  Picture(),
  XML(node) {
	set_parameter(tt_gray); 
	if (node != NULL) {
		node->AddRef();
	};
	file_name_short = short_file_name(file_name,TRUE);
	string partial_message = append_3_strings(S(IDS_UNABLE_TO_OPEN_FILE),"\r",file_name_short);
	string message = append_3_strings("***********************\r",partial_message,"\r***********************");
	Text *text = variable_width_text_pad(message,FALSE,tt_white);
	delete [] message;
//	delete [] short_name; // instance variable now
	text->set_size_and_location((4*width)/5,(4*height)/5,llx+width/10,lly+height/10); // leave some margins
	add_follower(text);
};

void UserPictureFileMissing::release_all_references() { // "renamed" on 130504
	if (releasing_all_references) { // new on 230305
		return;
	};
#if TT_XML
	if (XML != NULL) {
		XML->Release();
		XML = NULL; // new on 130504
	};
#endif
	if (file_name_short != NULL) { // shouldn't be otherwise
		delete [] file_name_short;
		file_name_short = NULL; // new on 130504
	};
	Picture::release_all_references();
};

Sprite *UserPictureFileMissing::copy(boolean also_copy_offsets) {
	Sprite *copied = already_copied(this);
   if (copied != NULL) {
		return(copied);
	};
	Picture *copy = new UserPictureFileMissing(file_name_short,XML);
	just_copied(this,copy); // new on 060503 so the following doesn't cause infinite recursion
	copy_flipped_followers(copy); // new on 060503
	finish_making_copy(copy,also_copy_offsets);
	return(copy);
};

#if TT_XML
//void UserPictureFileMissing::set_xml(xml_node *node) {
//	if (node != NULL) {
//		node->AddRef();
//		XML = node;
//	};
//};

//xml_element *UserPictureFileMissing::xml_create_element(xml_document *document) {
//	if (XML != NULL) {
//		XML->AddRef();
//		return((xml_element *) XML); // should have used xml_node_to_element
//	} else {
//		return(Picture::xml_create_element(document));
//	};
//};

wide_string UserPictureFileMissing::xml_tag() {
	if (XML == NULL) {
		return(Picture::xml_tag());
	} else {
		return(L"MissingPictureFile"); // new on 120703
	};
};

void UserPictureFileMissing::xml(xml_element *element, xml_document *document) {
	if (XML == NULL) {
		Picture::xml(element,document);
	} else {
		// written on 120703 so old XML is added as a child now
		XML->AddRef();
		xml_append_child(XML,element);
		xml_set_attribute(element,L"FlippedOver",is_flipped()); // new on 310105
		xml_set_attribute(element,L"Width",width); // new on 310105
		xml_set_attribute(element,L"Height",height);
		// updated the wording below for clarity on 090305
		log("The XML of a picture whose image file couldn't be found was loaded and saved unchanged. If you or a robot changed this picture the changes are lost.");
		tt_err_file_important_but_no_error = !replaying(); // was TRUE prior to 150505
	};  // else don't want to do the stuff that Picture etc do
};

xml_element *UserPictureFileMissing::xml_set_special_attributes(xml_element *element, xml_document *document) {
	if (XML == NULL) {
		return(Picture::xml_set_special_attributes(element,document));
	} else {
		return(NULL);  // don't want to do the stuff that Picture etc do
	};
};

void UserPictureFileMissing::add_special_xml(xml_element *element, xml_document *document) {
	if (XML == NULL) {
		Picture::add_special_xml(element,document);
	};
};

#endif

void GlobalPicture::update_globals() {
   // run_animations added 4/8/97 so can schedule events that are sprite-independent
   run_animations(FALSE,FALSE); 
	// for "sprite-less" animations -- first arg was FALSE prior to 040903 -- and it is again on 230903 
	// since GlobalPicture is the only way while finishing instantly to schedule things for future
	if (remote != NULL) remote->update_globals();
};

void GlobalPicture::follower_added(Sprite *leader, Sprite *new_follower) {
   if (remote != NULL) remote->follower_added(leader,new_follower);
};

void GlobalPicture::follower_removed(Sprite *leader, Sprite *old_follower) {
   if (remote != NULL) remote->follower_removed(leader,old_follower);
};

user_coordinate GlobalPicture::current_user_value(RemoteIdentifier identifier) {
	switch (identifier) {
      case HAND_VISIBLE_REMOTE:
			return(!tt_programmer->invisible());
		case SOUND_ON_REMOTE:
			return((tt_sound_option&SOUND_BIT) != 0);
		case LETTER_SIZE_REMOTE:
			return(tt_character_size_percent);
		case READING_SPEED_REMOTE:
			return(tt_help_speed_percent);
		case MARTIAN_ADVICE_LEVEL_REMOTE:
			return(tt_minimum_say_importance);
		case PROGRAMMER_HEAD_REMOTE:
			return(tt_programmer_head);
		case SCREEN_SIZE_REMOTE:
			return(current_dispdib_value()); // was old_dispdib_value prior to 100602
		case SYSTEM_SPEED_REMOTE:
			return(tt_speed);
		case CITY_SIZE_REMOTE:
#if TT_LARGE_CITIES
			return(tt_city_width);
#else
			return(tt_city_size);
#endif
		case SYSTEM_SLEEP_REMOTE:
			return(tt_sleep);
		case MOUSE_BUTTONS_REMOTE:
			return(tt_mouse_button_count);
      case MARTIAN_SPEAK_REMOTE:
         return(tt_marty_talk);
		case LEGO_SERIAL_PORT_REMOTE:
			return(tt_serial_port);
//		case MARTIAN_FONT_REMOTE:
//tt_mouse_button_count;
		// these can return 0 since update_globals fixes things
      // no since they might be taken from a notebook and used
      // before update_globals gets a chance to run
		case MOUSE_SPEED_X_REMOTE: {
			city_coordinate delta_x, delta_y;
			last_mouse_deltas(delta_x,delta_y);
			return(user_x((delta_x*1000)/tt_millisecond_delta)); // was just delta_x prior to 290802
		  };
		case MOUSE_SPEED_Y_REMOTE: {
			city_coordinate delta_x, delta_y;
			last_mouse_deltas(delta_x,delta_y);
			return(user_y((delta_y*1000)/tt_millisecond_delta)); // was just delta_y prior to 290802
		  };
		case CLOCK_REMOTE:
//#if TT_DEBUG_ON
//			if (tt_millisecond_delta > 1000) {
//				log("debug this");
//			};
//#endif
			return(tt_millisecond_delta);
		case RANDOM_REMOTE:
			return(my_random(1000)); // changed to 0-999 on 260301 - was 0-1000 inclusive
      default:
        return(0);  // shouldn't really happen
   };
};

#if !TT_32
user_coordinate HyperBotGlobalPicture::current_user_value(RemoteIdentifier identifier) {
   return(handle_lego_sensors(identifier));
};

user_coordinate HyperBotGlobalPicture::display_changed(RemoteIdentifier identifier,
													                user_coordinate new_value) {
   if (identifier >= LEGO_MOTOR1_REMOTE &&
       identifier <= LEGO_SENSOR_COUNT8_REMOTE) {
      return(handle_lego_motors(identifier,new_value));
   };
   return(new_value);
};

void HyperBotGlobalPicture::update_globals() {
	if (remote != NULL) remote->update_hyperbot_globals();
};
#endif

//#if TT_DIRECT_INPUT
// commented out the above since still need to support saved stuff and user moving between 16 and 32 bit versions

void JoystickRemotePicture::update_globals() {
#if TT_DIRECT_INPUT
	if (remote != NULL) remote->update_joystick_globals(joystick_number);
#endif
};

user_coordinate JoystickRemotePicture::display_changed(RemoteIdentifier ,
													                user_coordinate new_value) {
	return(new_value); // they are all read-only
};

user_coordinate JoystickRemotePicture::current_user_value(RemoteIdentifier identifier) {
#if TT_DIRECT_INPUT
	switch (identifier) {
      case JOYSTICK_SPEED_X_REMOTE:
         return(joystick_x(joystick_number));
      case JOYSTICK_SPEED_Y_REMOTE:
         return(joystick_y(joystick_number));
      case JOYSTICK_SPEED_Z_REMOTE:
         return(joystick_z(joystick_number));
      case JOYSTICK_AXIS_ROTATION_X_REMOTE:
         return(joystick_axis_x(joystick_number));
      case JOYSTICK_AXIS_ROTATION_Y_REMOTE:
         return(joystick_axis_y(joystick_number));
      case JOYSTICK_AXIS_ROTATION_Z_REMOTE:
         return(joystick_axis_z(joystick_number));
   };
#endif
   return(0); // shouldn't happen - why not when is a joystick button sensor
};

ForceRemoteControlee::ForceRemoteControlee(int joystick_index, int force_index) :
  GlobalPicture(),
  joystick_index((unsigned char) joystick_index),
  force_index((unsigned short) force_index),
  intensity(default_force_intensity),
  direction(default_force_direction),
  duration(default_force_duration),
  trigger_button(default_force_trigger_button),
  effects_count(0),
  effects(NULL),
  parameters_explicitly_set(FALSE) {
};

void ForceRemoteControlee::release_all_references() { // "renamed" on 130504
	if (releasing_all_references) { // new on 230305
		return;
	};
 //  if (preparing_for_deletion) return;
	//preparing_for_deletion = TRUE; // new on 060204
   GlobalPicture::release_all_references();
#if TT_DIRECT_INPUT
   unload_effects();
#endif
};

#if TT_DIRECT_INPUT
void ForceRemoteControlee::load_effects() {
	if (joystick_index == 0 && tt_force_sensitive_mouse_count > 0) {
		if (!parameters_explicitly_set) {
			parameters_explicitly_set = TRUE; 
			// new on 150801 to avoid recursive calls to the following and once set need not be initialized again
			DWORD value;
			if (get_touch_sensitive_mouse_effect_duration(force_name(force_index),value)) {
				if (value == INFINITE) {
					value = max_long;
				};
				display_changed(FORCE_FEEDBACK_DURATION_REMOTE,value); 
			};
			if (get_touch_sensitive_mouse_effect_intensity(force_name(force_index),value)) {
				display_changed(FORCE_FEEDBACK_GAIN_REMOTE,value/100L);
			};
			long long_value;
			if (get_touch_sensitive_mouse_effect_direction(force_name(force_index),long_value)) {
				display_changed(FORCE_FEEDBACK_DIRECTION_REMOTE,long_value/100L); 
			};
		};
		return; // new on 140801 since mouse is handled elsewhere
	};
   effects = create_force_feedback_effects(force_name(force_index),joystick_index,effects_count);
   if (effects != NULL) {
      if (parameters_explicitly_set) {
         display_changed(FORCE_FEEDBACK_DURATION_REMOTE,duration);
         display_changed(FORCE_FEEDBACK_GAIN_REMOTE,intensity);
//         display_changed(FORCE_FEEDBACK_TRIGGER_BUTTON_REMOTE,trigger_button);
         display_changed(FORCE_FEEDBACK_DIRECTION_REMOTE,direction);
      } else {
         display_changed(FORCE_FEEDBACK_DURATION_REMOTE,force_feedback_duration(effects));
         display_changed(FORCE_FEEDBACK_GAIN_REMOTE,force_feedback_intensity(effects));
//         display_changed(FORCE_FEEDBACK_TRIGGER_BUTTON_REMOTE,force_feedback_trigger_button(effects));
         display_changed(FORCE_FEEDBACK_DIRECTION_REMOTE,force_feedback_direction(effects));
      };
   };
};

boolean ForceRemoteControlee::unload_effects() {
   if (effects != NULL) {
      stop_effect();
      release_force_feedback_effects(joystick_index,effects);
      effects = NULL;
      parameters_explicitly_set = TRUE; // so they are restored properly
      return(TRUE);
   } else {
      return(FALSE);
   };
};
#endif

void ForceRemoteControlee::update_globals() {
	// nothing -- right?
};

user_coordinate ForceRemoteControlee::display_changed(RemoteIdentifier identifier,
													               user_coordinate new_value) {
#if TT_DIRECT_INPUT
   if (effects == NULL) { // && !tt_will_end_up_in_a_notebook) {
      load_effects();
   };
	if (joystick_index > 0 || tt_force_sensitive_mouse_count == 0) { // condition new on 150801
		switch (identifier) {
			case FORCE_FEEDBACK_DURATION_REMOTE:
				if (duration != new_value) {
					 duration = set_force_feedback_duration(effects,effects_count,new_value);
					 new_value = duration;
				};
				break;
			case FORCE_FEEDBACK_GAIN_REMOTE:
				if (intensity != new_value) {
					 intensity = set_force_feedback_intensity(effects,effects_count,new_value);
					 new_value = intensity;
				};
				break;
	//      case FORCE_FEEDBACK_TRIGGER_BUTTON_REMOTE:
	//         if (trigger_button != new_value) {
	//             trigger_button = (unsigned char) set_force_feedback_trigger_button(effects,effects_count,new_value);
	//             new_value = trigger_button;
	//         };
	//         break;
			case FORCE_FEEDBACK_DIRECTION_REMOTE:
				if (direction != new_value) {
					direction = set_force_feedback_direction(effects,effects_count,new_value);
					new_value = direction;
				};
				break;
		};
	} else { // touch sensitive mouse
		switch (identifier) {
			case FORCE_FEEDBACK_DURATION_REMOTE:
				if (duration != new_value) {
					if (new_value < 0) {
						new_value = max_long;
					};
					duration = new_value;
				};
				break;
			case FORCE_FEEDBACK_GAIN_REMOTE:
				if (intensity != new_value) {
					if (new_value > 100) new_value = 100;
					intensity = new_value;
				};
				break;
			case FORCE_FEEDBACK_DIRECTION_REMOTE:
				if (direction != new_value) {
					new_value %= 360;
					if (new_value < 0) new_value += 360;
					direction = new_value;
				};
				break;
		};
	};
   if (remote != NULL) remote->changed(identifier,new_value);
#endif
	return(new_value);
};

user_coordinate ForceRemoteControlee::current_user_value(RemoteIdentifier identifier) {
#if TT_DIRECT_INPUT
   if (effects == NULL && !tt_will_end_up_in_a_notebook) {
      load_effects();
   };
#endif
	switch (identifier) {
      case FORCE_FEEDBACK_DURATION_REMOTE:
         return(duration);
      case FORCE_FEEDBACK_GAIN_REMOTE:
         return(intensity);
//      case FORCE_FEEDBACK_TRIGGER_BUTTON_REMOTE: // commented out on 150801 since never used
//         return(trigger_button);
      case FORCE_FEEDBACK_DIRECTION_REMOTE:
         return(direction);
   };
   return(0); // shouldn't happen
};

boolean ForceRemoteControlee::dump(output_stream &stream, boolean just_prepare, boolean , boolean ) {
   if (just_prepare) return(FALSE);
   int dump_index = about_to_dump(this,stream); // rewritten on 020802
	if (dump_index < 0) return(FALSE); // new on 051099
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
		print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
		tt_error_file() << "About to dump ";
		print(tt_error_file());
		tt_error_file() << endl;
		tt_debug_dump_depth++;
	};
#endif
   stream.put((char) FORCE_REMOTE_CONTROLEE);
	dump_if_not_null(stream,comment);
//   force->dump(stream,just_prepare);
   stream.put(joystick_index);
   stream.write((character *) &force_index, sizeof(force_index));
//   stream.write((unsigned char *) &intensity, sizeof(intensity));
//   stream.write((unsigned char *) &direction, sizeof(direction));
//   stream.write((unsigned char *) &duration, sizeof(duration));
//   stream.put(trigger_button);
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

/*
Sprite *ForceRemoteControlee::copy(boolean also_copy_offsets) {
   if (!infinite_stack()) {
      Sprite *copied = already_copied(this);
      if (copied != NULL) return(copied);
   };
   return(new ForceRemoteControlee(force)); // copy the force too??
};
*/

void ForceRemoteControlee::set_parameters(long new_intensity, long new_direction, long new_duration, 
														unsigned char new_trigger_button) {
   intensity = new_intensity;
   direction = new_direction;
   duration = new_duration;
   trigger_button = new_trigger_button;
   parameters_explicitly_set = TRUE;
};

void ForceRemoteControlee::play_effect() {
#if TT_DIRECT_INPUT
	if (joystick_index == 0 && tt_force_sensitive_mouse_count > 0) { // really the mouse
		modify_and_play_touch_sensitive_mouse_effect(return_force_name(),intensity,direction,duration);
		return;
	};
   if (effects == NULL) {
      load_effects();
   };
   if (effects != NULL) play_force_feedback_effects(effects,effects_count);
#endif
};

void ForceRemoteControlee::stop_effect() {
#if TT_DIRECT_INPUT
	if (joystick_index == 0 && tt_force_sensitive_mouse_count > 0) { // really the mouse
		stop_touch_sensitive_mouse_effect();
		return;
	};
   if (effects != NULL) stop_force_feedback_effects(effects,effects_count);
#endif
};

ascii_string ForceRemoteControlee::return_force_name() {
#if TT_DIRECT_INPUT
  return(force_name(force_index));
#else
  return(AS(IDS_FORCE_FEEDBACK_NOT_AVAILABLE));
#endif
};

// an array of ForceRemoteControlee for each force feedback joystick
ForceRemoteControlee ***force_parameter_controlees = NULL;

typedef ForceRemoteControlee *ForceRemoteControleePointer;

ForceRemoteControlee *force_parameter_controlee(int force_index, int joystick, boolean &old) {
	if (joystick >= tt_force_feedback_joystick_count+tt_force_sensitive_mouse_count+1) return(NULL); 
   // warn too? -- new on 280202 -- commented out on 220802 - restored on 051102 and added +tt_force_sensitive_mouse_count+1 
   // so as to fail in a softer manner if loading corrupt files
   if (force_parameter_controlees == NULL) {
		int count = max(1,tt_force_feedback_joystick_count+tt_force_sensitive_mouse_count); 
		// at least 1 so can load when force is off
      force_parameter_controlees = (ForceRemoteControlee ***) new Pointer[count];
      for (int i = 0; i < count; i++) force_parameter_controlees[i] = NULL;
   };
#if TT_DIRECT_INPUT
   int force_count = return_force_count();
	if (force_count == 0) force_count = 1; // new on 160801 since might be running with DirectInput off
#else
   int force_count = 1; // don't care since no joysticks anyways
#endif
   if (force_parameter_controlees[joystick] == NULL) {
       force_parameter_controlees[joystick] = new ForceRemoteControleePointer[force_count];
       for (int i = 0; i < force_count; i++) force_parameter_controlees[joystick][i] = NULL;
   };
	if (force_count == 0 || force_index >= force_count) return(NULL); // warn too?? -- was just > prior to 021002
   old = (force_parameter_controlees[joystick][force_index] != NULL);
   if (!old) {
      force_parameter_controlees[joystick][force_index] = new ForceRemoteControlee(joystick,force_index);
   };
   return(force_parameter_controlees[joystick][force_index]);
};

void release_force_parameter_controlees() {
   if (force_parameter_controlees != NULL) {
      for (int i = 0; i < tt_force_feedback_joystick_count+tt_force_sensitive_mouse_count; i++) {
         if (force_parameter_controlees[i] != NULL) {
#if TT_DIRECT_INPUT
            int force_count = return_force_count();
#else
            int force_count = 1; // don't care since no joysticks anyways
#endif
            for (int j = 0; j < force_count; j++) {
               if (force_parameter_controlees[i][j] != NULL) {
                  force_parameter_controlees[i][j]->destroy();
               };
            };
            delete [] force_parameter_controlees[i];
         };
      };
      delete [] force_parameter_controlees;
   };
};

int unload_some_effects(int count) {
#if TT_DIRECT_INPUT
   if (force_parameter_controlees != NULL) {
      for (int i = 0; i < tt_force_feedback_joystick_count; i++) {
         if (force_parameter_controlees[i] != NULL) {
            int force_count = return_force_count();
            for (int j = 0; j < force_count; j++) {
               if (force_parameter_controlees[i][j] != NULL) {
                  if (force_parameter_controlees[i][j]->unload_effects()) {
                     count--;
                     if (count == 0) return(0);
                  };
               };
            };
         };
      };
   };
#endif
   return(count); // remaining
};

ForceRemoteControlee *load_force_remote_controlee(InputStream *pad_in, int notebook_version, 
																  NaturalLanguage language, long index) {
	Sprite *comment = NULL;
	if (notebook_version >= first_version_to_save_copied_nests) {
		comment = load_item(pad_in,notebook_version,language);
	};
   unsigned char joystick_index = (unsigned char) pad_in->get();
   unsigned short force_index;
   pad_in->read((string) &force_index, sizeof(force_index));
   // following was a bad idea since subsequent to saving these values may change
   // leading to non-intuitive behavior
//   long intensity, direction, duration;
//   pad_in->read((unsigned char *) &intensity, sizeof(intensity));
//   pad_in->read((unsigned char *) &direction, sizeof(direction));
//   pad_in->read((unsigned char *) &duration, sizeof(duration));
//   unsigned char trigger_button = (unsigned char) pad_in->get();
   boolean old;  // obsolete switch now
   ForceRemoteControlee *controlee = force_parameter_controlee(force_index,joystick_index,old);
	associate_shared_item(index,controlee);
//   if (!old) controlee->set_parameters(intensity,direction,duration,trigger_button);
	if (controlee != NULL) { // not an error
		controlee->set_comment(comment);
	};
   return(controlee);
};

//   ForceFeedback *force = (ForceFeedback *) load_item(pad_in,notebook_version,language);
//   if (force != NULL) {
//      return(new ForceRemoteControlee(force));
//   } else {
//      return(NULL);
//   };


// if changed update help.cpp
enum ButtonHistory {BUTTON_CLICKED=0,BUTTON_CYCLE,BUTTON_LETTER_TYPED};

void did_button_cycle(Sprite *by) {
   user_did(TOOL_BUTTON_HISTORY,BUTTON_CYCLE,by);
};

void button_letter_typed(Sprite *by) {
   user_did(TOOL_BUTTON_HISTORY,BUTTON_LETTER_TYPED,by);
};

Button::Button(SpriteCode image_code,
					city_coordinate llx, city_coordinate lly,
			      char initial_letter,
			      long priority,
			      city_coordinate width, city_coordinate height,
               color_index text_color) :
   Picture(llx,lly,priority,0,image_code,width,height),
   letter(initial_letter),
   text_color(text_color) {
};

void Button::display(SelectionFeedback selection_feedback, boolean followers_too, TTRegion *region) {
   if (visible()) {
		Picture::display(selection_feedback,followers_too,region); // moved into conditional on 210100
      city_coordinate adjusted_llx = llx+width/10;
      city_coordinate adjusted_lly = lly; // experiment on 050102 - was +height/10;
      if (want_feedback(selection_feedback)) {
		   // only supporting movement for now
		   adjusted_llx += selection_delta_x();
		   adjusted_lly += selection_delta_y();
	   };
      tt_screen->place_character(letter,adjusted_llx,adjusted_lly,
                                 width,height, // was (4*width)/5,(4*height)/5 prior to 050102
											compute_color(text_color,color_permutation()));
   };
};

Sprite *Button::selected(SelectionReason reason, Sprite *by) {
   if (leader != NULL) {
      leader->button_pushed(this,by);
      user_did(TOOL_BUTTON_HISTORY,BUTTON_CLICKED,by);
		record_event(KEYBOARD_CHARACTER,by,floor,leader,TRUE,'+'); // equivalent to typing "+" to the leader of the button
#if TT_DIRECT_INPUT
      if (by != NULL && !by->still_running()) {
         play_force_feedback_effect("Click",FALSE);
      };
#endif
      return(NULL);
   };
   return(Picture::selected(reason,by));
};

Sprite *Button::respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected, Sprite *by,
										      boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless) {
    if (leader != NULL) {
       return(leader->respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,
														size_constrained_regardless));
    } else {
       return(Picture::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,
														 size_constrained_regardless));
    };
};

void Button::generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection) {
   if (leader == NULL) return; // shouldn't happen
   if (tool_in_hand == NULL &&
		 ((!did(BUTTON_CLICKED) && tt_help_counter == 0)
		 || tt_help_counter == 1)) {
		tt_help_history_action = BUTTON_CLICKED;
		describe_pointing(message,tool_in_hand,selection);
      message << S(IDS_BUTTON_CLICKED_HELP1) << space(SPACE_BEFORE_PRONOUN);
      print_it(leader->kind_of(),message);
      message << space() << S(IDS_BUTTON_CLICKED_HELP2);
	} else if ((!did(BUTTON_CYCLE) && tt_help_counter == 0)
				  || tt_help_counter == 2) {
		tt_help_history_action = BUTTON_CYCLE;
      describe_pointing(message,tool_in_hand,selection);
		message << S(IDS_BUTTON_CYCLE_HELP1) << space();
      leader->describe_type(message,DEFINITE_ARTICLE,FALSE);
      message << space() << S(IDS_BUTTON_CYCLE_HELP2);
	} else if ((!did(BUTTON_LETTER_TYPED) && tt_help_counter == 0)
				  || tt_help_counter == 3) {
		tt_help_history_action = BUTTON_LETTER_TYPED;
      describe_pointing(message,tool_in_hand,selection);
      message << S(IDS_BUTTON_LETTER_TYPED_HELP1) << space();
      leader->describe_type(message,DEFINITE_ARTICLE,FALSE);
      message << space() << S(IDS_BUTTON_LETTER_TYPED_HELP2) << space();
		string commands = NULL;
      switch (leader->kind_of()) {
         case EXPANDER:
            commands = S(IDS_EXPANDER_KEY_COMMANDS);
            break;
         case VACUUM:
            commands = S(IDS_VACUUM_KEY_COMMANDS);
            break;
         case COPIER:
            commands = S(IDS_COPIER_KEY_COMMANDS);
            break;
      };
		// new on 110700 is to insert commas and spaces between commands
		if (commands != NULL) {
			int count = strlen(commands);
			for (int i = 0; i < count-1; i++) {
				message << commands[i] << ", ";
			};
			message << commands[count-1];
		};
      message << ".";
	} else if ((!did(TYPE_HELP_GIVEN) && tt_help_counter == 0)
					|| tt_help_counter == 4) {
		message << S(IDS_BUTTON_HELP_GIVEN_HELP1)
				  << S(IDS_BUTTON_HELP_GIVEN_HELP2)
  				  << S(IDS_BUTTON_HELP_GIVEN_HELP3) << space();
      leader->describe_type(message,PLURAL_ARTICLE,FALSE);
      if (leader->kind_of() == COPIER) {
         message << S(IDS_COPIER_BUTTON_HELP_GIVEN_HELP);
      } else if (leader->kind_of() == EXPANDER) {
         message << S(IDS_EXPANDER_BUTTON_HELP_GIVEN_HELP);
      } else if (leader->kind_of() == VACUUM){
         message << S(IDS_VACUUM_BUTTON_HELP_GIVEN_HELP);
      };
		no_help_interruptions();  // why?
		user_did(TOOL_BUTTON,TYPE_HELP_GIVEN);
	};
};

void Button::describe(output_stream &message, Article article) {
   describe_appearance(appearance_code(),message,article);
//   describe_type(message,article);
//   if (leader != NULL) {
//      message << " " << S(IDS_OF) << " ";
//      leader->describe(message,article);
//   };
};  

void Button::active_region(TTRegion &region) {
   full_region(region);
//   if (tt_log_version_number == 3) { // this was a bad idea
      // 1/2 size
//      city_coordinate quarter_width = region.width()/4;
//      region.min_x += quarter_width;
//      region.max_x -= quarter_width;
//      city_coordinate quarter_height = region.height()/4;
//      region.min_y += quarter_height;
//      region.max_y -= quarter_height;
//   };
};

#if TT_XML

void Button::xml(xml_element *element, xml_document *document) {
	xml_set_attribute(element,L"ImageCode",appearance_code());
	Picture::xml(element,document);
};

xml_element *Button::xml_set_special_attributes(xml_element *element, xml_document *document) {
	xml_element *special_element = Picture::xml_set_special_attributes(element,document);
	xml_set_attribute(special_element,L"LetterCode",letter);
	xml_set_attribute(special_element,L"ColorCode",text_color);
	return(special_element);
};

boolean Button::xml_get_special_attributes(xml_node *node) {
	letter = (char) xml_get_attribute_int(node,L"LetterCode",letter);
	text_color = xml_get_attribute_int(node,L"ColorCode",text_color);
	return(Picture::xml_get_special_attributes(node));
};

#endif

void add_flipped_follower(Sprite *follower, Sprite *previous_follower, Picture *picture, AddFollowerReason reason) {
	if (follower == NULL) return; // added for robustness on 250903
	boolean orginally_flipped = picture->is_flipped();
	if (!orginally_flipped) { // new on 161200
		picture->flip(TRUE,FALSE);
	};
	if (previous_follower != NULL && // re-ordered these on 141002 since XML no longer provides previous_follower
       follower->kind_of() == ROBOT &&	 
		 previous_follower->kind_of() == CUBBY) {
		// associate them but don't run just yet
//		((Robot *) follower)->suspend();
//		((Robot *) follower)->try_clause(((Cubby *) previous_follower));
		((Robot *) follower)->associate_with_box((Cubby *) previous_follower);
//       if (tt_log_version_number > 7) {
      follower->set_priority(previous_follower->priority()+1); // +1 added on 090200
		picture->add_follower(follower,TRUE,INSERT_BY_PRIORITY,FALSE,reason); 
		// moved here and args added 090200 so robot ends up under its box
//         };
//	} else {
//         if (tt_log_version_number > 7) {
//					picture->place_new_item(follower); 
//         };
   } else {
		picture->add_follower(follower,TRUE,INSERT_AT_END,FALSE,reason); // else add it normally
	};
	follower->now_on_back_of(picture);
//	if (follower->kind_of() == PICTURE && follower->is_flipped() && picture->is_flipped()) { // new on 270300
//		Background *floor = picture->create_backside_background_if_need_be();
//		follower->set_background(floor,TRUE);
//		follower->activate(); 
//	};
	if (!orginally_flipped) {
		picture->unflip(TRUE,FALSE);
	};
};

Background *Picture::create_backside_background_if_need_be() {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("creating backside background of debug target (picture)");
	};
#endif
	if (backside_background == NULL) { 
		backside_background = new Backside_Background(this);
	};
	return(backside_background);
};

// commented out since better that remote looks flips to match...
//boolean Picture::is_flipped() {
//	if (controlee != NULL) {
//		return(controlee->is_flipped()); // new on 151200
//	} else {
//		return(flipped);
//	};
//};

void interpret_picture_flags(unsigned char flags, int notebook_version, boolean &show_all, boolean &visible, 
									  boolean &flipped, boolean &show_some_if_subpicture, boolean &not_active,
									  boolean &know_if_geometry_is_dumped, boolean &geometry_dumped, boolean &non_zero_speed,
									  boolean &infinite_stack_flag, boolean &graspable_flag) {
	show_all = flags&1;
	visible = (flags>>1)&1;
	flipped = (flags>>2)&1;
	show_some_if_subpicture = (flags>>3)&1;
	not_active = (flags>>4)&1;
	boolean old_know_if_geometry_is_dumped = (flags>>5)&1; // new on 111199 -- starting 080400 reused for infinite_stack_flag
	boolean old_geometry_dumped = (flags>>6)&1; // new on 111199
	non_zero_speed = (flags>>7)&1; // new on 130200
	if (notebook_version >= first_version_to_save_programmer_and_remotes_accurately) {
		infinite_stack_flag = old_know_if_geometry_is_dumped;
		graspable_flag = !old_geometry_dumped; // new on 100400 - negated since dumped as !graspable()
		know_if_geometry_is_dumped = TRUE; // new on 080100
		geometry_dumped = FALSE;
	} else {
		infinite_stack_flag = FALSE;
		know_if_geometry_is_dumped = old_know_if_geometry_is_dumped;
		geometry_dumped = old_geometry_dumped;
		graspable_flag = TRUE; // new on 040500 since things always were graspable prior to 100400
	};
};

Picture *load_picture(SpriteType type, InputStream *stream,
							 int notebook_version, NaturalLanguage language,
                      Sprite *container, Picture *controlee, long index, boolean is_controlee, boolean on_nest,
                      boolean descendants_inactive) {
	// changes here should also be reflected in first_page_with_picture
	Sprite *comment = NULL;
	if (notebook_version >= first_version_to_save_copied_nests) {
		comment = load_item(stream,notebook_version,language);
	};
	if (is_controlee && notebook_version < first_version_to_correctly_save_remote_looks) {
		is_controlee = FALSE; // old behavior for old savings
	};
   if (type == REMOTE_APPEARANCE && notebook_version >= first_version_to_save_controlees) {
		// last arg added on 140699
     Sprite *new_controlee = load_item(stream,notebook_version,language,container,NULL,-1,TRUE);
     if (new_controlee != NULL) {
#if TT_DEBUG_ON
		  if (new_controlee->kind_of() != PICTURE) {
			  log("Controlee not a picture.");
		  };
#endif
        if (tt_copying_insides_for == controlee && tt_copying_insides_for != NULL) {
           // ignore the controlee this was saved with and use the current one
           new_controlee->destroy();
//			  new_controlee = NULL;
        } else {
           controlee = (Picture *) new_controlee;
        };
     };
   };
	Sprite *indirection = NULL;
   ascii_string user_picture_file_name = NULL;
   boolean need_to_become_blank = FALSE;
	if (type == PICTURE_WITH_INDIRECTION) {
		indirection = load_item(stream,notebook_version,language);
	} else if (type == USER_PICTURE || type == BLANK_USER_PICTURE) {
      unsigned char file_name_length = (unsigned char) stream->get();
      user_picture_file_name = new char[file_name_length+1];
      stream->read((string) user_picture_file_name,file_name_length);
      user_picture_file_name[file_name_length] = '\0'; // terminate string
//#if TT_DEBUG_ON
//		string short_name = short_file_name(user_picture_file_name);
//		if (strcmp(short_name,"invadera") == 0) {
//			tt_error_file() << "Debug this!" << endl;
//		};
//#endif
      need_to_become_blank = (type == BLANK_USER_PICTURE);
   };
	unsigned short int image_index;
	stream->read((string) &image_index, sizeof(image_index));
	SpriteCode image_code = (SpriteCode) stream->get();
   if (controlee != NULL && type == REMOTE_APPEARANCE) { // && tt_log_version_number > 14) { 
		// added 020299 to get sizes in synch when remote looks used
		if (controlee->appearance_code() != USER_SPRITE) { 
			// conditional added on 090699 to fix problems with remote looks of user pictures
			image_code = controlee->appearance_code();
		};
	} else if (type == PICTURE_WITH_INDIRECTION) {
		image_code = SYNTHETIC_SPRITE; 
		// added on 190799 since if it is USER_PICTURE then it doesn't really have the right current_image
	};
   unsigned char picture_flags = (unsigned char) stream->get();
	boolean show_all,visible,flipped,show_some,not_active,know_if_geometry_is_dumped,geometry_dumped,
			  non_zero_speed,infinite_stack_flag,graspable_flag;
	interpret_picture_flags(picture_flags,notebook_version,show_all,visible,flipped,show_some,not_active,
									know_if_geometry_is_dumped,geometry_dumped,non_zero_speed,infinite_stack_flag,graspable_flag);
   if (!not_active && descendants_inactive) { 
		// new on 220702 to enforce the idea that if the parent is inactive then so must I be
      not_active = TRUE;
   };
	Picture *picture;
   if (type == USER_PICTURE || type == BLANK_USER_PICTURE) { // BLANK_USER_PICTURE added 021199
		boolean ok = TRUE; // moved inside here on 080500 since no else should care
      picture = new UserPicture(user_picture_file_name,ok);
		// commented out on 080500 since this broke followers and sensors of the picture
//		if (!ok) { // new on 150300 since if not OK then picture isn't quite usable so use tt_global_picture for a while
//			picture->destroy();
//			picture = tt_global_picture;
//		};
		if (!ok) { // moved here and updated on 080500 
			//picture->set_size_is_true(TRUE); // convenient "lie"
			//picture->set_size_and_location(100,100,0,0); // another lie
			//picture->set_indirection(variable_width_text_pad(user_picture_file_name),TRUE);
			// rewritten on 290103
			picture->destroy();
			picture = new UserPictureFileMissing(user_picture_file_name);
			tt_error_file() << "Replaced a picture with a missing file with the text pad: " << user_picture_file_name << endl;
		};
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
			print_spaces(tt_debug_load_depth,tt_error_file());
			tt_error_file() << "user_picture_file_name=" << user_picture_file_name 
								 << " geometry_dumped=" << (int) geometry_dumped << endl;
		};
#endif
   } else {
     if (image_code < 0 || image_code >= BACKGROUND_SPRITE || (image_code != SYNTHETIC_SPRITE && image_index > 255)) {  
		  // largest now is about 100 so 255 is pretty safe
        log(S(IDS_PROBLEM_DECODING_A_PICTURE),FALSE,TRUE);
		  if (stream->empty()) {
			  log("End of file while loading a picture.");
		  } else {
			  tt_error_file() << "TellG=" << (int) stream->tellg() << endl;
		  };
        if (image_code < 0 || image_code >= BACKGROUND_SPRITE) image_code = SYNTHETIC_SPRITE;
        image_index = 0;
      };
		picture = new Picture(0,0,0,image_index,image_code);//,4*tile_width);
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
			print_spaces(tt_debug_load_depth,tt_error_file());
			tt_error_file() << "image_code=" << (int) image_code << " geometry_dumped=" << (int) geometry_dumped << endl;
		};
#endif
		if (image_code == SCALE_SPRITE && not_active && notebook_version < first_version_to_save_thought_bubbles_disconnected) { 
			// new on 140201 just to be sure that recent changes don't break old code
			not_active = FALSE; // to get the old behavior
		};
   };
   if (container != NULL) {  // new on 160702
      if (container->kind_of() == PROGRAM_PAD) {
         picture->set_in_notebook((Notebook *) container,FALSE); // no point recuring
      } else if (container->kind_of() == VACUUM) {
         picture->set_inside_vacuum(TRUE,FALSE);
      };
   }; 
   associate_shared_item(index,picture);
	picture->set_show_some_if_subpicture(show_some);
	picture->set_infinite_stack(infinite_stack_flag); // new on 080400
	picture->set_graspable(graspable_flag); // new on 100400
	if (controlee != NULL && // added 020299 to get sizes in synch when remote looks used tt_log_version_number > 14 && 
		 type == REMOTE_APPEARANCE) { // added on 160799
		picture->set_visible(controlee->visible());
		picture->set_show_all(controlee->show_all());
	} else {
		picture->set_visible(visible);
		picture->set_show_all(show_all);
	};
	if (indirection != NULL) {	// since object doesn't normally save this
		indirection->set_visible(visible);
	   indirection->set_show_all(show_all);
	};
//	city_coordinate initial_width = 0;
//	city_coordinate initial_height = 0;
	boolean saved_size_set = FALSE;
	city_coordinate saved_width = -1; // might be saved versions before flipping
   city_coordinate saved_height = -1; // given negative value on 070602 so ignored if not set below
	if (!is_controlee && !tt_dumping_to_test_equality_or_describe && 
		 (container == NULL || container->kind_of() != PICTURE || (flipped && notebook_version > 8))) {
		// added && notebook_version > 8 on 250399 to correctly read old format files (e.g. chase movies)
		// || flipped add on 180299 so flipped pictures saved on flipped picture get a good geometry
		// on 150300 added && !tt_dumping_to_test_equality_or_describe since might be loading as part of a describe
		// (e.g. describe_named_robot)
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299) {
			print_spaces(tt_debug_load_depth,tt_error_file());
			tt_error_file() << "Loading saved width and height." << endl;
		};
#endif
		if (notebook_version >= first_version_to_save_absolute_picture_geometry) { // new on 130200
			stream->read((string) &saved_width, sizeof(saved_width));
			stream->read((string) &saved_height, sizeof(saved_height));
		} else {
			short int value;
			stream->read((string) &value, sizeof(value));
//			if (tt_log_version_number < 15) {
//				picture->display_changed(WIDTH_REMOTE,value);
//			} else {
				// new on 170299 since saved size is lost otherwise
				saved_width = picture->from_user_x((city_coordinate) value); 
	//			initial_width = user_x_value;
	//			picture->set_width(user_x_value); // moved here from first branch above on 140599 so that sizes are restored correctly
//			};
			stream->read((string) &value, sizeof(value));
//			if (tt_log_version_number < 15) {
//				picture->display_changed(HEIGHT_REMOTE,value);
//			} else {
				// new on 170299 since saved size is lost otherwise
				saved_height = picture->from_user_y((city_coordinate) value);
	//			picture->set_height(user_y_value); // in flipped case for followers on the front to have the right geometry
	//			initial_height = user_y_value;
//			};
		};
		if (flipped) { // new on 180299 so saving pictures while flipped preserves geometry OK
			if (saved_width >= minimum_dimension || saved_height >= minimum_dimension) {
				// temporary size got stuck here somehow - fix introduced on 311000
				picture->Sprite::good_size(saved_width,saved_height); // so flipped and the like don't matter using Sprite::
			};
			picture->set_width_before_flip(saved_width);
			picture->set_height_before_flip(saved_height);
		} else {
			picture->set_saved_width(saved_width);
			picture->set_saved_height(saved_height);
		};
		saved_size_set = TRUE;
	};	
	// commented out on 130200 since absolete size is now saved with all pictures
	// added ok here and elsewhere in case user picture is missing BMP - will be destroyed and replaced anyway
//	if (initial_width != 0 && ok) { // moved here so things are initialized OK first
//		picture->set_size(initial_width,initial_height);
//	};
//	Sprite *insides;
//	if (type != BLANK_PICTURE) {
//		unsigned short int image_index;
//		stream->read((unsigned char *) &image_index, sizeof(image_index));
//		insides = new Picture_Insides(image_index,(SpriteCode) stream->get());
//	} else {
//		insides = NULL;
//	};
	switch (type) {
		case BLANK_PICTURE:
         need_to_become_blank = TRUE;
			break; //return(picture);
		case REMOTE_APPEARANCE:
         picture->set_controlee(controlee);
			if (controlee != NULL && notebook_version < first_version_to_save_remote_looks_flipness) { // new on 191200
				// added first conjunct on 210101
				flipped = (controlee->is_flipped() && !controlee->is_temporarily_flipped()); 
				// old way was flippedness of remote looks was identical to controlee
			};
			picture->set_flipped(flipped); // new on 181200
			break;
	};
   if (need_to_become_blank) picture->become_blank();
//	picture->set_image(insides);
//	if (picture->current_height() > 8*tile_height) {
//		picture->set_height_maintain_proportionality(8*tile_height);
//	};
	if (//ok && 
		 indirection != NULL) { // moved here on 040100 since followers don't get added right otherwise
		picture->set_indirection(indirection,FALSE);
		picture->pointer_to_indirection()->destroy(); 
		// new on 240101 since held on to by indirection and nothing here - 
		// use pointer_to_indirection in case it was updated by :set_indirection
// commented out the following on 171199 since it caused pictures that indirect to text (or numbers) to get the original
// color of the picture that was erased before receiving the text pad
//      if (indirection->kind_of() == PICTURE) {
//         indirection->indirection_of_set_parameter(picture->current_parameter());
//      } else {
//         indirection->indirection_of_set_parameter((short int) (picture->current_parameter()%256));  
		// just color info for text and numbers
//      };
	};
	if (notebook_version >= first_version_to_save_absolute_picture_geometry && !tt_dumping_to_test_equality_or_describe) { 
		// new on 120200
		picture->load_just_geometry(stream,notebook_version);
		if (non_zero_speed) {
			if (notebook_version >= first_version_to_save_rational_numbers) {
				// updated on 050302
				picture->set_x_speed(load_number_value(stream,notebook_version));
				picture->set_y_speed(load_number_value(stream,notebook_version));
			} else {
				user_coordinate x_speed,y_speed;
				stream->read((string) &x_speed,sizeof(x_speed));
				stream->read((string) &y_speed,sizeof(y_speed));
				picture->set_x_speed(new LongNumberValue(x_speed));
				picture->set_y_speed(new LongNumberValue(y_speed));
			};
		};
	};
	unsigned short int followers_count;
	stream->read((string) &followers_count, sizeof(followers_count));
//	Picture *old_tt_copying_insides_for = tt_copying_insides_for;
//	tt_copying_insides_for = picture;
//	if (container == NULL) controlee = picture; 
	// commented out on 100399 since this is a confusing lie - it broke for example the visibility flags
	Sprite *follower = NULL;
	Sprite *previous_follower = NULL;
	if (followers_count > 0) {
		picture->update_display(); // added on 300699 so geometry of followers is right
		for (int i = 0; i < followers_count; i++) {
	//		SpriteType type = (SpriteType) stream->get(); // added on 300699 since if OLD_SHARED_ITEM it won't have geometry saved
			// for now only pictures are on pictures
			Sprite *item = load_item(stream,notebook_version,language,picture,controlee,-1,FALSE,FALSE,not_active); 
			// not_active new on 220702
			if (item != NULL) {
				if (item->kind_of() == PICTURE) { 
					Picture *follower = (Picture *) item;
#if TT_DEBUG_ON
					if (follower->is_blank() && !need_to_become_blank && 
						 (indirection == NULL || (!indirection->is_blank() && indirection->kind_of() == PICTURE))) {
						log("Warning: loading an erased picture that is part of another picture.");
					};
#endif
//					if (ok) {
						picture->add_follower(follower);
						follower->set_subpicture(TRUE,FALSE); // second arg added 190500
//					}; // maybe should save followers in a list to add to file-name replacement??
					// commented out conditional because these values aren't loaded if picture is follower of picture
					// so the geometry comes out relative
					// uncommented it out on 031299 since new dumpings have already handled this
					if (notebook_version < first_version_to_save_absolute_picture_geometry) { 
						 if (!tt_last_picture_loaded_knew_if_geometry_was_dumped || tt_last_picture_dumped_geometry) { 
						// added 181199 need this for older dumps
#if TT_DEBUG_ON
							if (tt_debug_mode == 160299) {
								print_spaces(tt_debug_load_depth,tt_error_file());
								tt_error_file() << "Loading relative geometry of follower ";
	//tt_debug_target = picture->debug_counter; // for now
								follower->print(tt_error_file());
								tt_error_file() << endl;
								print_spaces(tt_debug_load_depth,tt_error_file());
							};
#endif
							UnwindProtect<boolean> set(tt_keep_center_when_size_changes,FALSE); // new on 080200
							for (int j = 0; j < saved_remotes_count; j++) {
								short int value;
								stream->read((string) &value, sizeof(value));				 
								follower->display_changed(location_and_size[j],(city_coordinate) value);
#if TT_DEBUG_ON
								if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
									tt_error_file() << value << " ";
									if (j+1 == saved_remotes_count) tt_error_file() << endl;
								};
#endif
							};
						} else { // otherwise something wrong -- added 020299 to be more robust when errors
							// revised 170299 so that this isn't called when item is NULL
							tt_error_file() << S(IDS_NON_PICTURE_ON_TOP_OF_PICTURE) << endl;
							picture->flip(TRUE);
							add_flipped_follower(item,previous_follower,picture,LOADING_FOLLOWER);
							picture->unflip(TRUE);
							previous_follower = item;
						};
					};
				}; // else warn
			};
		};
	};
//   if (stream->get() != END_OF_LIST_MARKER) {
//      say("Something wrong with a picture in a file or clipboard.",4);
//   };
//	picture->update_display(); 
	stream->read((string) &followers_count, sizeof(followers_count));
	if (//ok && 
		 (followers_count > 0 || flipped)) {
		picture->flip(TRUE,flipped); // was !flipped prior to 130200
		if (flipped && saved_width > 0) { // new on 081200 -- second condition new on 070602
			picture->set_width_before_flip(saved_width); 
			picture->set_height_before_flip(saved_height);
		};
	};
	for (int i = 0; i < followers_count; i++) {
		SpriteType follower_type = (SpriteType) stream->get();
		previous_follower = follower;
		follower = load_item(follower_type,stream,notebook_version,language,picture,picture,-1,FALSE,FALSE,not_active); 
		//controlee); - not_active added 220702
		if (follower == NULL) {
			if (tt_dumping_to_test_equality_or_describe || tt_loading_to_describe) { // conditional added on 290300
				log("Something went wrong trying to load a picture.");
			} else {
				say_error(IDS_TOONTALK_OR_FILE_BAD,TRUE);
			};
			return(picture); // was NULL -- should this throw instead??
		};
//			follower->set_on_flip_side_of_picture(TRUE); // for now
//		city_coordinate picture_x,picture_y;
//		picture->lower_left_corner(picture_x,picture_y);
//		city_coordinate picture_width = picture->current_width();
//		city_coordinate picture_height = picture->current_height();
//		city_coordinate real_x,real_y,real_width,real_height;
		if (notebook_version < first_version_to_save_copied_nests) {
		// following is obsolete but kept until time to generate a new file format
			short int user_value;
			stream->read((string) &user_value,sizeof(user_value));
	//		real_x = picture_x+user_value;//*picture_width)/1000;
			stream->read((string) &user_value,sizeof(user_value));
	//		real_y = picture_y+user_value;//*picture_height)/1000;
			stream->read((string) &user_value,sizeof(user_value));
	//		real_width = user_value;//*picture_width)/1000;
			stream->read((string) &user_value,sizeof(user_value));			
	//		real_height = user_value;//*picture_height)/1000;
		};
//      if (tt_log_version_number <= 7) {
//		   follower->update_display(); // initialize offsets
//		   follower->set_size_and_location(real_width,real_height,real_x,real_y);
//		   follower->propagate_changes();
//      } else {
//         if (real_x < 100) {
//            follower->set_priority(real_x); // re-cycled slot
//         } else {   // probably old...
//            follower->set_priority(i);
//         };
//      };
//		if (ok) 
		if (type == REMOTE_APPEARANCE) { // new on 181200 since remote looks don't have followers anymore
//			follower->destroy(); -- can't destroy since might be shared via NEW_ITEM/OLD_ITEM -- 
			// is only a leak when loading old remote looks...
		} else {
			add_flipped_follower(follower,previous_follower,picture);
		};
	};
	// commented out on 060201 since handled differently now
//   if (//ok && 
//		 followers_count > 0) {  //  && tt_log_version_number > 7 
//      picture->place_new_item();
//   };
	// so true_x_offset() is 0 so followers follow right
	if (!flipped) {// && ok) { // leave it flipped if it was to start with
		if (followers_count > 0) {
			picture->unflip(TRUE,flipped); // 2nd arg prior to 130200 was tt_log_version_number < 15;
		};
	} else {
		picture->set_temporarily_flipped(FALSE); // new on 071200
	};
	if (!saved_size_set && (container == NULL || container->kind_of() == PICTURE || flipped)) { 
		// conditions added on 170299 and 180299
		// removed ok &&  on 080500
		// added !saved_size_set on 130200 so this doesn't override an explictly saved size
		picture->save_size(); 
	};
//	tt_copying_insides_for = old_tt_copying_insides_for;
	if (container == NULL  && 
		 !tt_dumping_to_test_equality_or_describe && // added 150300 since might be loading as part of describe
		 ((know_if_geometry_is_dumped && geometry_dumped) || // && (container == NULL || container->kind_of() != PICTURE)) || 
		// know_if_geometry_is_dumped new on 111199
		// rest of condition above added 191199 since these should be set after adding picture as follower (if it is)
		// and commented out on 031299 since if dumped must be loaded
		  (!know_if_geometry_is_dumped && !is_controlee && !on_nest && // on_nest added 120799
		   !flipped && // added on 190999 since when flipped this isn't dumped
		   notebook_version > first_version_to_save_city_items))) { // new on 080699
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299) {
			print_spaces(tt_debug_load_depth,tt_error_file());
			tt_error_file() << "Loading relative geometry ";
			picture->print(tt_error_file());
			tt_error_file() << endl;
		};
#endif
		for (int j = 0; j < saved_remotes_count; j++) {
			short int value;
			stream->read((string) &value, sizeof(value));				 
//			if (ok) 
			picture->display_changed(location_and_size[j],(city_coordinate) value);
#if TT_DEBUG_ON
			if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
				print_spaces(tt_debug_load_depth,tt_error_file());
				tt_error_file() << value << " ";
				if (j+1 == saved_remotes_count) tt_error_file() << endl;
			};
#endif
		};
	};
//	picture->set_old_x_offset(0);
//	picture->set_old_y_offset(0);
//   picture->recursively_propagate_changes();
//	if (ok) {
		picture->set_comment(comment);
		picture->propagate_changes(); // added on 100399 since geometry of subpictures is sometimes wrong until selected, etc.
		// added FALSE arg below on 050100 since shouldn't apply to parts
		picture->set_active(!not_active,FALSE); 
		// new on 190799 -- moved here on 111299 since should be active while changes propogage above (e.g. if it has remotes)
//	} else { // moved here on 100999 so that rest of record is read even if something is wrong with BMP
		// new on 290699
		// Marty should already have made a warning
//		Picture *replacement_picture = make_indirect_picture(variable_width_text_pad(user_picture_file_name),FALSE);
//		replacement_picture->set_size_and_location_to_those_of(picture); // new on 150300 since sometimes they were huge
//		picture->destroy(); - not any more since picture is tt_global_picture that was standing in temporarily
//		picture = replacement_picture;
		// re-wrote the above on 080500 to just make this guy indirect to the text pad after discovering it isn't OK
//	};
	if (user_picture_file_name != NULL) delete [] user_picture_file_name;
	if (notebook_version < first_version_to_save_absolute_picture_geometry) {
		// obsolete as of 120200
		tt_last_picture_loaded_knew_if_geometry_was_dumped = know_if_geometry_is_dumped; // hack so old versions still load OK
		tt_last_picture_dumped_geometry = geometry_dumped; // new on 051299
	};
	return(picture);
};

boolean describe_picture(SpriteType type, InputStream *stream, output_stream &text_out, int version_number,
								 boolean in_thought_bubble,
								 Article article, SpriteType container_type, boolean is_controlee, boolean on_nest) {
	if (version_number < first_version_to_correctly_save_remote_looks) {
		is_controlee = FALSE; // old behavior for old encodings
	};
	if (version_number >= first_version_to_save_copied_nests) {
		describe_comment_encoding(stream,text_out,version_number,PICTURE);
	};
   ascii_string user_picture_file_name = NULL;
	if (type == PICTURE_WITH_INDIRECTION) {
//		text_out << "a picture of ";
		SpriteType indirection_type = (SpriteType) stream->get();
		if (!describe_encoding(indirection_type,stream,text_out,version_number,in_thought_bubble,article)) {
			return(FALSE);
		};
	};
	boolean more_to_say = TRUE;
	if (type == USER_PICTURE || type == BLANK_USER_PICTURE) {
      unsigned char file_name_length = (unsigned char) stream->get();
      user_picture_file_name = new char[file_name_length+1];
      stream->read((string) user_picture_file_name,file_name_length);
      user_picture_file_name[file_name_length] = 0; // terminate string
   } else if (type == REMOTE_APPEARANCE) { 
		text_out << S(IDS_REMOTE_APPEARANCE_CONTROL) << " - "; 
		// " - " added on 120499 so it reads better (but how language independent is this?)
		// prior to 140699 ignored what was next so it was not described well - especially if the controlee is a user picture
      if (!describe_item(stream,text_out,version_number,in_thought_bubble,article,container_type,TRUE)) {
			return(FALSE); // problems so quit
		};
		more_to_say = FALSE;
   };
	unsigned short int image_index;
	stream->read((string) &image_index, sizeof(image_index));
	SpriteCode image_code = (SpriteCode) stream->get();
	unsigned char picture_flags = (unsigned char) stream->get();
	boolean show_all,visible,flipped,show_some,not_active,know_if_geometry_is_dumped,geometry_dumped,non_zero_speed,
			  infinite_stack_flag,graspable_flag;
	interpret_picture_flags(picture_flags,version_number,show_all,visible,flipped,show_some,not_active,
									know_if_geometry_is_dumped,geometry_dumped,non_zero_speed,infinite_stack_flag,graspable_flag);
	if (!tt_dumping_to_test_equality_or_describe && !is_controlee &&
		 (container_type == NONE_GIVEN || container_type != PICTURE || flipped)) {
		  // != PICTURE was == PROGRAM_PAD prior to 140599 - || flipped also added to match picture::dump
//		 !flipped) { // added on 250399 so Marty can describe flipped pictures in new format
		// ignore saved width and height
		if (version_number >= first_version_to_save_absolute_picture_geometry) { // new on 130200
			stream->ignore(sizeof(city_coordinate)*2);
		} else {
			stream->ignore(sizeof(short int)*2);
		};
	};
	if (version_number >= first_version_to_save_absolute_picture_geometry && !tt_dumping_to_test_equality_or_describe) { 
		// new on 120200
		tt_global_picture->load_just_geometry(stream,version_number); // ignore geometry
		if (non_zero_speed) {
         if (version_number >= first_version_to_save_rational_numbers) {
				// new on 150702 (several months late)
            NumberValue *ignore = load_number_value(stream,version_number);
            if (ignore == NULL) return(FALSE);
            delete ignore;
            // do it again for the y speed
            ignore = load_number_value(stream,version_number);
            if (ignore == NULL) return(FALSE);
            delete ignore;
			} else { // old version was just 2 ints
				stream->ignore(sizeof(user_coordinate)*2);
			};		
		};
	};
	if (infinite_stack_flag) { // new on 080400
		print_with_article(article,IDS_STACK_OF,type,text_out);
	};
	unsigned short int next_id = 0;
	if (more_to_say) {
		if (type == BLANK_PICTURE || type == BLANK_USER_PICTURE) {
			if (in_thought_bubble) {
				text_out << S(IDS_ANY_PICTURE) << " (" << S(IDS_FOR_EXAMPLE) << comma() << space();
	//			article = INDEFINITE_ARTICLE;
			} else {
				print_with_article(article,IDS_BLANK_PICTURE_OF,type,text_out);
				next_id = IDS_BLANK_PICTURE_OF2;
				text_out << space();
	//			article = INDEFINITE_ARTICLE;
			};
		};
		if (image_code == SCALE_SPRITE) {
			switch (image_index) {
				case GREATER_THAN_SCALE:
				case GREATER_THAN_FROM_EQUAL_SCALE:
	//				print_article(article,text_out,S(IDS_SCALE_TO_LEFT));
					text_out << S(IDS_SCALE_TO_LEFT);
					break;
				case LESS_THAN_SCALE:
				case LESS_THAN_FROM_EQUAL_SCALE:
	//				print_article(article,text_out,S(IDS_SCALE_TO_RIGHT));
					text_out << S(IDS_SCALE_TO_RIGHT);
					break;
				case EQUAL_SCALE:
				case EQUAL_FROM_GREATER_SCALE:
				case EQUAL_FROM_LESS_SCALE:
	//				print_article(article,text_out,S(IDS_SCALE_BALANCED));
					text_out << S(IDS_SCALE_BALANCED);
					break;
				default: 
	//				print_article(article,text_out,S(IDS_SCALE_MOVING));
					text_out << S(IDS_SCALE_MOVING);
					break;
			};
		} else if (image_code == SYNTHETIC_SPRITE) {
			switch (type) {
	//			case REMOTE_APPEARANCE:
	//            {
	//            char buffer[512];
	//            output_string_stream ignore(buffer,512);
	//            describe_item(stream,ignore,in_thought_bubble);
	//            text_out << S(IDS_REMOTE_APPEARANCE_CONTROL);
	//            };
	//				break;
				case PICTURE_WITH_INDIRECTION:
					break; // say nothing here
				default:
	//				describe_appearance(image_code,text_out,article);
					text_out << S(IDS_SYNTHETIC_RECTANGLE+(image_index/256));
			};
		} else if (type != PICTURE_WITH_INDIRECTION) { 
			// if indirection already described the indirection -- added 040399
			if (image_code == USER_SPRITE) { // moved here on 160799 so doesn't happen when indirection
				text_out << S(IDS_USER_SPRITE);
				if (user_picture_file_name != NULL) {
					text_out << " (" << user_picture_file_name << ")";
				};
				text_out << space(); // space added on 160799
			} else if (type != BLANK_PICTURE && type != BLANK_USER_PICTURE) {
				if (still_picture(image_code)) {
	//            print_with_article(article,IDS_PICT_OF,text_out);
					 text_out << S(IDS_PICT_OF);
				} else {
	//            print_with_article(article,IDS_MOVIE_OF,text_out);
					 text_out << S(IDS_MOVIE_OF);
				};
				text_out << space();
			};
			if (type != USER_PICTURE) { // conditional added on 120999 since otherwise would repeat itself
				describe_appearance(image_code,text_out,INDEFINITE_ARTICLE);
			};
		};
		if (type == PICTURE) { 
			// added on 080499 since otherwise with indirection Marty says "which is partly visible which is partly visible"
			if (image_code == SCALE_SPRITE) { // don't mention visibility
			} else if (!visible) { // updated on 090100
				text_out << space() << S(IDS_WHICH_IS_INVISIBLE);
			} else if (!show_all) { // updated on 090100
				if (image_index/256 != 0) { // added on 120799 so rectangles aren't described as partly visible
					text_out << space() << S(IDS_WHICH_IS_PARTLY_VISIBLE);
				};
			};
		};
	};
	unsigned short int followers_count;
	stream->read((string) &followers_count, sizeof(followers_count));
	if (followers_count > 1) {
		text_out << space() << S(IDS_WITH) << space(SPACE_BEFORE_OR_AFTER_NUMBERS) 
					<< followers_count << S(IDS_PICT_DESCRIPTION1) << end_sentence();
	} else if (followers_count == 1) {
		text_out << S(IDS_PICT_DESCRIPTION2) << end_sentence();
	};
//	boolean first = TRUE;
   int i;
	for (i = 0; i < followers_count; i++) {
//		SpriteType type = (SpriteType) stream->get();
      if (i > 0) text_out << S(IDS_AND_BEGIN_SENTENCE) << space();
      print_ordinal_definite_phrase(i+1,text_out,(i == 0),
                                    S(IDS_THING_ON_TOP1),S(IDS_THING_ON_TOP2));
//			text_out << S(IDS_BEGIN_THE);
//         print_the(text_out,TRUE,NONE_GIVEN,S(IDS_THING_ON_TOP));
//         text_out << " ";
//		} else {
//			text_out << S(IDS_AND_THE);
//			first = FALSE;
//		};
//		print_ordinal(i+1,text_out);
//		text_out << " " << S(IDS_THING_ON_TOP);
		if (stop_generating_help()) {
			text_out << S(IDS_SAID_ENOUGH);
			return(FALSE);
		};
		if (!describe_item(stream,text_out,version_number,in_thought_bubble,INDEFINITE_ARTICLE,PICTURE)) return(FALSE);
		if (i < followers_count-1) text_out << end_sentence();
		if (version_number < first_version_to_save_absolute_picture_geometry) {
			if (!tt_dumping_to_test_equality_or_describe && !tt_last_picture_loaded_knew_if_geometry_was_dumped) {
				for (int j = 0; j < saved_remotes_count; j++) {
					short int value;
					// read and ignore initializations
					stream->read((string) &value, sizeof(value));				 
				};
			};
		};
	};
	int previous_followers_count = followers_count;
	stream->read((string) &followers_count, sizeof(followers_count));
	if (followers_count > 0) {
		if (previous_followers_count == 0) {
			text_out << end_sentence(); // why??
		} else { // add 1 for notebook which is there if you look -- removed that on 120399 since it was too confusing
			text_out << end_sentence() << S(IDS_AND_BEGIN_SENTENCE) << space() << S(IDS_ON_THE_BACK);
			if (followers_count == 0) {
				text_out << space() << S(IDS_JUST_A_NOTEBOOK);   
			} else {
				text_out << space() << S(IDS_ARE) << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << followers_count 
							<< space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_THINGS);
			};
			text_out << end_sentence();
		};
	};
//	first = TRUE;
	for (i = 0; i < followers_count; i++) {
      if (i > 0) text_out << S(IDS_AND_BEGIN_SENTENCE) << space();
		long next_id;
      print_ordinal_definite_phrase(i+1,text_out,
                                    (i == 0),
                                    S(IDS_THING_ON_THE_BACK1),
                                    S(IDS_THING_ON_THE_BACK2));
		next_id = IDS_THING_ON_THE_BACK3;
//      print_the(text_out,first,NONE_GIVEN,S(IDS_THING_ON_THE_BACK));
//      text_out << " ";
//		print_ordinal(i+1,text_out);
//		text_out << " " << S(IDS_THING_ON_THE_BACK);
		SpriteType type = (SpriteType) stream->get();
		if (stop_generating_help()) {
			text_out << S(IDS_SAID_ENOUGH);
			return(FALSE);
		};
		if (!describe_encoding(type,stream,text_out,version_number,FALSE,INDEFINITE_ARTICLE,PICTURE)) {
			// FALSE was in_thought_bubble prior to 161000 but isn't really in thought bubble if part of picture
			return(FALSE);
		};
		if (i != followers_count-1) text_out << end_sentence(); // except for last one since period will come later
		if (!tt_dumping_to_test_equality_or_describe && !is_controlee &&
			 version_number < first_version_to_save_copied_nests) {
			short int user_value;
		   // read and ignore initializations
			stream->read((string) &user_value,sizeof(user_value));
			stream->read((string) &user_value,sizeof(user_value));
			stream->read((string) &user_value,sizeof(user_value));
			stream->read((string) &user_value,sizeof(user_value));	
		};
		if (next_id != 0) {
			text_out << S(next_id,"");
		};
	};
   if (in_thought_bubble && type == BLANK_PICTURE) text_out << ")"; // end the for example
	if (next_id != 0) {
		text_out << S(next_id,"");
	};
	if (version_number < first_version_to_save_absolute_picture_geometry) {
		if ((know_if_geometry_is_dumped && geometry_dumped) ||
		    (!know_if_geometry_is_dumped && !tt_dumping_to_test_equality_or_describe && !is_controlee && 
			 // both of these conjuncts added on 140699
		     !on_nest && // added 120799
		     container_type == NONE_GIVEN && version_number >= first_version_to_save_city_items)) { // new on 080699
			for (int j = 0; j < saved_remotes_count; j++) {
				short int value;
				// read and ignore initializations
				stream->read((string) &value, sizeof(value));				 
			};
		};
		tt_last_picture_loaded_knew_if_geometry_was_dumped = know_if_geometry_is_dumped;
	};
	if (infinite_stack_flag) { // new on 080400
		print_with_article(article,IDS_STACK_OF2,type,text_out,FALSE,""); // ignore if not defined
	};
	return(TRUE);
//	if (followers_count > 0) {
//		text_out << end_sentence();
//	};
};

Path *Picture::path_to(Sprite *target, boolean &ok) {
	if (target->pointer_to_leader() == this) {
		return(new Path(SPECIAL_PATH_MARKER,new Path(target->kind_of()))); //,new Path(SPECIAL_PATH_MARKER)
	};
	ok = FALSE;
	return(NULL);
};

void Picture::animate_size(city_coordinate new_width,
								   city_coordinate new_height,
								   millisecond expansion_duration,
								   Sprite *notify,
								   AnimationCallBack callback,
								   boolean signal_change) {
	// come back and fix this later... broke flipping pictures over
//	if (indirection != NULL) {
//		indirection->animate_size(new_width,new_height,expansion_duration,notify,callback,signal_change);
//	} else {
	if (indirection != NULL && (indirection->kind_of() == TEXT || indirection->kind_of() == INTEGER)) { 
		// new on 090102 so that the font changes as the picture flips or animates (added numbers on 110102)
//		indirection->set_size_and_location(width,height,llx,lly,ANIMATION_STARTING); -- made part of sprite::animate_size on 110102
		indirection->animate_size(new_width,new_height,expansion_duration,notify,callback,signal_change); 
		// just for font to track changes...
	};
	Sprite::animate_size(new_width,new_height,expansion_duration,notify,callback,signal_change);
};

//void Picture::animate_location(city_coordinate new_llx,
//										 city_coordinate new_lly,
//										 millisecond duration,
//										 Sprite *notify,
//										 AnimationCallBack callback,
//										 boolean signal_change) {
////	if (indirection != NULL) {
////		indirection->animate_location(new_llx,new_lly,duration,notify,callback,signal_change);
////	} else {
//		Sprite::animate_location(new_llx,new_lly,duration,notify,callback,signal_change);
////	};
//};

void Picture::set_background(Background *new_background, boolean recur, boolean warn) {
//#if TT_DEBUG_ON
//	if (floor != NULL && floor != new_background && floor->has_item(this)) {
//		log("debug this");
//	};
//#endif
	if (controlee == NULL) { // conditional new on 171200
		// added this so that when flipped pictures are on flipped pictures the background is kept consistent
		if (new_background != NULL && recur && is_flipped()) { // is_flipped() test added 100400
			Picture *picture_of_background = new_background->pointer_to_picture();
			if (picture_of_background != NULL && picture_of_background != this) {
				if (backside_background == NULL) { 
					backside_background = new Backside_Background(this);
				};
				Sprites *remaining = flipped_followers(); // was followers prior to 250399
				while (remaining != NULL) {
					Sprite *follower = remaining->first();
					follower->set_background(backside_background,recur,FALSE);
					remaining = remaining->rest();
				};
				if (is_flipped()) {
					Sprite::set_background(new_background,FALSE,warn);
					return; // took care of everything else above -- added 250399
				};
			};
		};
		if (indirection != NULL) {
			indirection->set_background(new_background,recur,warn);
		};
	};
	Sprite::set_background(new_background,recur,warn);
};

void Picture::remove_all_flipped_followers(boolean destroy_too) {
   Sprites *remaining = flipped_followers();
   while (remaining != NULL) {
      Sprites *next_remaining = remaining->rest();
		Sprite *follower = remaining->first();
      remove_flipped_follower(follower);
      if (destroy_too) remaining->first()->destroy();
      remaining = next_remaining;
   };
//	set_postponed_copy_flipped_followers_of_controlee(FALSE);
};

boolean Picture::has_followers() {
	if (controlee != NULL) {
		return(controlee->has_followers());
	} else { // not clear what to do about indirection??
		return(Sprite::has_followers());
	};
};

boolean Picture::rectangular() { // new on 181000
	if (controlee != NULL) {
		return(controlee->rectangular());
	} else if (indirection != NULL) {
		return(indirection->rectangular());
	} else { 
		return(Sprite::rectangular());
	};
};

ArrowKeyEditing Picture::arrow_key_editing_status() { // new on 100604
	if (comment != NULL) { // new on 120604
		ArrowKeyEditing comment_status = comment->arrow_key_editing_status();
		if (comment_status != NO_ARROW_KEY_EDITING_POSSIBLE) {
			return(comment_status);
		};
	};
	if (controlee != NULL) {
		return(controlee->arrow_key_editing_status());
	} else if (indirection != NULL) {
		return(indirection->arrow_key_editing_status());
	} else {
		return(Sprite::arrow_key_editing_status());
	};
};

ChangeFrequency Picture::change_frequency() { // new on 150200
	if (!active()) {
		return(NEVER_CHANGES); // new on 301000
	} else if (remote_looks_of_me_count > 0 || controlee != NULL) {
		// either there are remote looks of me out there (not counting those in notebooks)
		// or I'm a remote looks
		return(ONLY_ON_NEW_CYCLE); // I guess
	} else if (appearance_code() == SCALE_SPRITE) { // new on 030200
		Sprite *left;
		Sprite *right;
		if (neighbors(left,right)) {
			return(max(left->change_frequency(),right->change_frequency()));
		} else {
			return(NEVER_CHANGES);
		};
	} else {
		return(NEVER_CHANGES);
	};
};

//#if TT_DIRECT_PLAY
//void Picture::set_being_marshalled(boolean new_flag) {
//	if (controlee != NULL) return; // new on 171200 - remote looks doesn't really have anything on back
//	Sprite::set_being_marshalled(new_flag);
//	Sprites *remaining = flipped_followers();
//	while (remaining != NULL) {
//		remaining->first()->set_being_marshalled(new_flag);
//		remaining = remaining->rest();
//	};
//};
//#endif

boolean Picture::equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, 
								  SpritesPointer &expected_corresponding_nests, EqualReason reason) { // new on 050601
	// can have different geometry (and speeds)
   if (other->kind_of() != PICTURE) {
      return(FALSE);
   };
	if (other == this) return(TRUE);
//	if (is_blank()) {
//		return(other->is_blank());
//	} else if (other->is_blank()) {
//		return(FALSE);
//	};
   if (is_blank() != other->is_blank()) {
      return(FALSE); // rewritten on 080601 since blanks still differ with respect to using the wand in Original mode
   };
//	if (other->active() != active()) return(FALSE); // new on 080601
	Picture *other_picture = (Picture *) other;
	// added the following on 210601 to deal with see some and see nothing differences
//   if (combine_flags() != other_picture->combine_flags()) {
   if (active() != other_picture->active() ||
       show_some_if_subpicture() != other_picture->show_some_if_subpicture() ||
       is_flipped() != other_picture->is_flipped() ||
       visible() != other_picture->visible() ||
       show_all() != other_picture->show_all()) {
      // rewritten on 051102 since not all flags make sense (e.g. infinite_stack)
      return(FALSE); 
   };
//	if (is_flipped() != other_picture->is_flipped()) return(FALSE);
	Sprite *others_indirection = other_picture->pointer_to_indirection();
	if (others_indirection == NULL && indirection != NULL) {
		return(FALSE); //indirection->equal_to(other,pending,reason)); // changed on 060601 to maintain old semantics
	};
	if (others_indirection != NULL && indirection == NULL) {
		return(FALSE); // equal_to(others_indirection,pending,reason));
	};
	if (others_indirection != NULL && indirection != NULL) {
		if (!indirection->equal_to(others_indirection,pending,corresponding_nests,expected_corresponding_nests,reason)) {
			return(FALSE);
		};
	};
   if (appearance_code() != other_picture->appearance_code()) {
      return(FALSE);
   };
	if (appearance_code() == USER_SPRITE) { // new on 080601
		string my_file_name = return_file_name();
		string other_file_name = other_picture->return_file_name();
		if (my_file_name != NULL && other_file_name != NULL) {
         if (strcmp(my_file_name,other_file_name) != 0) {
            return(FALSE);
         };
      } else if (!(my_file_name == NULL && other_file_name == NULL)) {
         return(FALSE);
      };
   } else if (appearance_code() == SCALE_SPRITE && active()) {
      return(TRUE); 
		// new on 051102 since neighbors may be remote controls of the equal controlees but have different x and y for example
      // so if both sides of the scale are equal and the scale is active then they are equal
   } if (current_parameter() != other_picture->current_parameter()) {
      return(FALSE); // new on 080601
   };
	// what about active?
	Sprites *others_followers = other_picture->pointer_to_followers();
   if (followers == NULL && others_followers != NULL) {
      return(FALSE);
   };
   if (followers != NULL && others_followers == NULL) {
      return(FALSE);
   };
	Sprites *others_followers_on_other_side = other_picture->pointer_to_followers_on_other_side();
   if (followers_on_other_side == NULL && others_followers_on_other_side != NULL) {
      return(FALSE);
   };
   if (followers_on_other_side != NULL && others_followers_on_other_side == NULL) {
      return(FALSE);
   };
//	if (followers == NULL && followers_on_other_side == NULL) return(TRUE); -- 
	// need to check controlee still (commented out on 080601)
   if (followers != NULL && followers->length() != others_followers->length()) {
      return(FALSE);
   };
   if (followers_on_other_side != NULL && followers_on_other_side->length() != others_followers_on_other_side->length()) {
      return(FALSE);
   };
	// about to recurse on parts but first check if this is a recursive call of the same problem
	if (pending != NULL) {
		if (check_pending_for_equals(other,pending)) return(TRUE);
	};
	Sprite *others_controlee = other_picture->pointer_to_controlee();
   if (others_controlee != NULL && controlee == NULL) {
      return(FALSE);
   };
   if (others_controlee == NULL && controlee != NULL) {
      return(FALSE);
   };
	Sprites *new_pending = new Sprites(this,new Sprites(other,pending));
	if (others_controlee != NULL && controlee != NULL) { // both remote looks
		boolean result = controlee->equal_to(others_controlee,new_pending,corresponding_nests,expected_corresponding_nests,reason);
		new_pending->rest()->set_rest(NULL);
		just_destroy_list(new_pending);
		return(result);
	};
	Sprites *remaining = followers;
	Sprites *others_remaining = others_followers;
	while (remaining != NULL) {
		if (!remaining->first()->equal_to(others_remaining->first(),new_pending,corresponding_nests,expected_corresponding_nests,
													 reason)) {
			new_pending->rest()->set_rest(NULL);
			just_destroy_list(new_pending);
			return(FALSE);
		};
		remaining = remaining->rest();
		others_remaining = others_remaining->rest();
	};
	remaining = followers_on_other_side;
	others_remaining = others_followers_on_other_side;
	while (remaining != NULL) {
		if (!remaining->first()->equal_to(others_remaining->first(),new_pending,corresponding_nests,expected_corresponding_nests,
													 reason)) {
			new_pending->rest()->set_rest(NULL);
			just_destroy_list(new_pending);
			return(FALSE);
		};
		remaining = remaining->rest();
		others_remaining = others_remaining->rest();
	};
	return(TRUE);
};

//void Picture::make_remote() {
//	remote = new Remote(this);
//};

/*

  void Picture::place_new_item_old_version(Sprite *item) {
   city_coordinate item_width = item->current_width();
   city_coordinate item_height = item->current_height();
   city_coordinate max_item_width = width/3;
   city_coordinate max_item_height = height/3;
   int width_percent_of_max = (item_width*100)/max_item_width;
   int height_percent_of_max = (item_height*100)/max_item_height;
   int max_percent = max(width_percent_of_max,height_percent_of_max);
   if (max_percent > 100) {
      item->set_size((item_width*100)/max_percent,
                     (item_height*100)/max_percent);
   };
//					item->compute_dimensions_to_fit(width/5,height/5,
//															  new_width,new_height);
//					if (item_width < new_width) new_width = item_width;
//					if (item_height < new_height) new_height = item_height;
					// animate and this is default -- e.g. robot_on_cubby different
//					item->move_by((item_width-new_width)/2,
//									  (item_height-new_height)/2);
//					item->set_size(new_width,new_height);
};
*/

/* commented out on 171200
void Picture::side_about_to_be_seen(boolean front) { 
// made obsolete on 311000 since doesn't work well and is expensive and typing space isn't a big deal
	//new on 240200 so that pictures which are flipped are stopped while pictures that are seen
	// (even if on flipside of another) are resumed
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		tt_error_file() << "Debug target about to seen " << (int) front << endl;
	};
#endif
   if (!front) {
		add_robots_to_flipped_followers();
	};
	robots_running_on_back_flag = front;
	Sprites *remaining = flipped_followers();
	while (remaining != NULL) {
		Sprite *item = remaining->first();
		if (item->kind_of() == ROBOT) {
			Robot *robot = (Robot *) item;
//#if TT_DEBUG_ON
//			if (front && is_flipped()) {
//				log("Need to conditionalize log 2.");
//			};
//#endif
			if (front && !is_flipped()) {
				// second conjunct added on 210300 since shouldn't be running robots on flipped picture
				if (robot->waiting_for_release() || robot->suspended()) {
					// waiting_for_release() was broken prior to 040500 so for example numbers with robots 
					// on the back didn't run until dropped
               robot->activate();
				};
			} else {
				robot->suspend();
			};
		} else { 
			item->side_about_to_be_seen(!front);
		};
		remaining = remaining->rest();
	};
	remaining = unflipped_followers();
	while (remaining != NULL) {
		remaining->first()->side_about_to_be_seen(front);
		remaining = remaining->rest();
	};
};

  void Picture::ensure_remote_looks_has_flipped_followers() { // new on 151200
	// should only be called if flipped
	if (controlee == NULL) return;
	// if controlee is flipped then this remote looks should have copies of the flipped followers of the controlee
	Picture *saved_controlee = controlee;
	controlee = NULL; // so stuff called by this method ignores the controlee
	if (return_frame_flipped_followers_changed_last() != saved_controlee->return_frame_flipped_followers_changed_last()) {
		remove_all_followers(TRUE);
		set_frame_flipped_followers_changed_last();
	};		
	if (pointer_to_followers() == NULL) {
		Sprites *controlees_flipped_followers = saved_controlee->flipped_followers(); 
		if (controlees_flipped_followers != NULL) {
//			if (!is_flipped()) { // and we already know that the controlee is flipped
//				flip(TRUE,FALSE);
//			};
			Sprites *remaining = controlees_flipped_followers->top_level_copy();
			Sprite *previous_follower_copy = NULL;
			while (remaining != NULL) {
				Sprite *follower_copy = remaining->first();		
				add_flipped_follower(follower_copy,previous_follower_copy,this);
				previous_follower_copy = follower_copy;
				remaining = remaining->rest();
			};
			place_new_item(); // to get the geometry right
		};
	};
	controlee = saved_controlee;
};
*/

/*
void Picture::set_x_speed(double new_x_speed) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		tt_error_file() << "Debug target's x speed is now " << new_x_speed << endl;
	};
#endif
	x_speed = new_x_speed;
	if (remote != NULL) { // new on 040302
		RationalNumberValue new_value(new_x_speed);
		remote->broadcast_new_value(X_SPEED_REMOTE,&new_value,NULL);
	};
};

void Picture::set_y_speed(double new_y_speed) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		tt_error_file() << "Debug target's y speed is now " << new_y_speed << endl;
	};
#endif
	y_speed = new_y_speed;
	if (remote != NULL) { // new on 040302
		RationalNumberValue new_value(new_y_speed);
		remote->broadcast_new_value(Y_SPEED_REMOTE,&new_value,NULL);
	};
};
*/
