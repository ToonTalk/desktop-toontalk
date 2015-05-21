// Copyright (c) 1992-2005. Ken Kahn, Animated Programs, All rights reserved.

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
#if !defined(__TT_SCREEN_H)   
#include "screen.h"
#endif   
#if !defined(__TT_BIRD_H)   
#include "bird.h"
#endif   
#if !defined(__TT_CUBBY_H)   
#include "cubby.h"
#endif
#if !defined(__TT_TEXT_H)
#include "text.h"
#endif
#if !defined(__TT_TRUCK_H)   
#include "truck.h"
#endif   
#if !defined(__TT_ROOM_H)   
#include "room.h"
#endif   
#if !defined(__TT_FLOOR_H)   
#include "floor.h"
#endif   
#if !defined(__TT_PAD_H)   
#include "pad.h"
#endif   
#if !defined(__TT_BCKGRND_H)   
#include "bckgrnd.h"
#endif   
#if !defined(__TT_EVENT_H)   
#include "event.h"
#endif   
#if !defined(__TT_PRGRMMR_H)   
#include "prgrmmr.h"
#endif   
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif
#if !defined(__TT_PUZZLE_H)
#include "puzzle.h"
#endif
#if !defined(__TT_HELP_H)   
#include "help.h"
#endif
#if !defined(__TT_JAVA_H)
#include "java.h"
#endif
#if !defined(__TT_MOUSE_H)
#include "mouse.h"
#endif
#if !defined(__TT_WINMAIN_H)
#include "winmain.h"
#endif
#if !defined(__TT_MARTIAN_H)
#include "martian.h"
#endif
#if TT_XML
#if !defined(__TT_XML_H)
#include "xml.h"
#endif
#endif
#if !defined(__TT_ROBOT_H)   
#include "robot.h"
#endif   
   
#if TT_DEBUG_ON
string debug_robot_name = "broadcastannouncement"; // is case insensitive and nothing for new lines -- -d 70900
#endif     

#if !TT_32
#include <stdlib.h> // for min
#include <shellapi.h>
#endif

//#include <strstrea.h> commented out 210703

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// if changed update help.cpp
enum RobotHistory {ROBOT_GIVEN_CUBBY=0,
						 ROBOT_ABSTRACTED,
						 ROBOT_TRIED,
						 ROBOT_SAVED,
						 ROBOTS_LINED_UP};

void robot_saved() {
  user_did(ROBOT,ROBOT_SAVED);
};

void robot_abstracted() {
	user_did(ROBOT,ROBOT_ABSTRACTED);
};

RobotState robot_waiting_state = ROBOT_WAITING_NEW; // unless overriden by backwards compatibility (beta 4)

const unsigned char no_subject_index = 255; // abstracted on 131202

Robot::Robot(city_coordinate center_x,
				 city_coordinate center_y,
				 long initial_priority,
//				 Background *floor,
				 city_coordinate width,
				 city_coordinate height,
				 Events *body,
				 int max_working_set_size) :
//				 Thought_Bubble *thought_bubble,
//				 Robot *next_robot) :
	Sprite(ROBOT_SPRITE,
		    center_x,center_y,ROBOT,
		    ROBOT_INERT,ground_scale,width,height),
//	thought_bubble(thought_bubble),
	thought_bubble(NULL),
//	thought_bubble_attached(FALSE),
	body(body),
//	programmer(NULL),
	working_set(NULL),
	working_set_length(0),
	max_working_set_size(max_working_set_size),
	next_robot(NULL),
	saved_next_robot(NULL),
	first_in_line(NULL),
	top_cubby(NULL),
//	true_when_ready_to_try_clause(TRUE),
	running_body_flag(FALSE), // do I still need this and body_started and body_status???? - asked 131202
//   nothing_grasped_flag(FALSE),
	body_started_flag(FALSE), // same as above??
	waiting_for_release_flag(FALSE), // e.g. was running on flip side -- now in cubby
	suspended_on(NULL),
	suspended_flag(FALSE),
	run_again_this_cycle(FALSE),
//	true_when_body_finished(FALSE),
	body_status(0), // really hasn't even started -- changed from 1 to 0 on 100500 so robots saved in a city while running resume correctly
	tool(NULL),
   initial_tool(NULL),
//	vacuum_grasped(FALSE),
//	copier_grasped(FALSE),
//   expander_grasped(FALSE),
	target(NULL),
	//receiving_bird(NULL),
	//receiving_truck(NULL),
	vacuum(NULL),
	copier(NULL),
   expander(NULL),
//	action_status(NULL), // made obsolete on 171102
	set_down_counter(0),
//	deleted_sprites(NULL),
	abort_reason(0),
	saved_subject_index(no_subject_index),
	saved_subject(NULL), // new on 161202
	saved_stack(NULL), // new on 161202
	robot_to_put_behind(NULL), // new on 161202
	saved_key(0),
	page_number_offsets(NULL),
	recent_page_number(0),
   team_notebook(NULL), // this one was put in the truck with the team
	home_notebook(NULL), // this is where the robot came from
	use_home_notebook(FALSE),
   home_notebook_last_page_number(0),
	released_on_its_stack(FALSE),
//#if TT_DEBUG_ON // commented out on 180200 since not used anymore
//   body_version_number(latest_version_number),
//#endif
//   notebook_not_yet_used(TRUE),
   training_counter(NO_TRAINING_COUNTER),
	original_training_counter(NO_TRAINING_COUNTER),
	name_from_serial_number(TRUE),
	name_from_user(NULL),
	first_to_fail_for_unstable_reasons(NULL),
	remove_me_from_backside_flag(FALSE),
	team_running_flag(FALSE),
//	inside_sprite_flag(FALSE), // was missing prior to 101199 - noticed obsolete on 131202
	target_index(-1), // wasn't initialized prior to 131202
	saved_character(0), // wasn't initialized prior to 131202
	saved_select(FALSE), // wasn't initialized prior to 131202
	tool_width(0), // wasn't initialized prior to 131202
	tool_height(0),
	drop_scroll_delta_x(0),
	drop_scroll_delta_y(0),
	number_of_recent_page_numbers(0), 
	// notebooks_with_recent_page_numbers ignored if number_of_recent_page_numbers is zero so no need to initialize
	last_failure_was_for_unstable_reasons_flag(FALSE), // new on 301000
	saved_next_event_send_character_to_same_subject(FALSE), // new on 270704
	version(default_robot_version) { 
	//	set_priority_function_of_lly(FALSE); // fix .mkb files
	current_priority = initial_priority;
	set_show_all(FALSE); // new on 070103 so dumps without SeeAll tag
/*
	if (thought_bubble != NULL) {
		thought_bubble_attached = TRUE;
		thought_bubble->set_priority(current_priority);
		add_follower(thought_bubble,floor,FALSE);
	};
	if (next_robot != NULL) {
		if (first_in_line == NULL) { // I am
			next_robot->set_first_in_line(this);
		} else {
			next_robot->set_first_in_line(first_in_line);
		};
		put_behind_me(next_robot);
//		add_follower(next_robot,floor,FALSE);
	};
*/
#if TT_DEBUG_ON
   if (tt_debug_mode == 1717) {
      tt_error_file() << "Making robot#" << debug_counter << " ";
      tt_error_file() << java_class_name() << " on frame " << tt_frame_number << endl;
   };
	if (tt_debug_mode == 70900) {
		character robot_name[max_resource_string_length];
		name(robot_name);
		if (strnicmp(robot_name,debug_robot_name,strlen(debug_robot_name)) == 0) {
			tt_error_file() << "Making robot " << robot_name << "#" << debug_counter << " on frame " << tt_frame_number << endl;
		};
	};
#endif
};

void Robot::prepare_for_deletion() { // restored on 080604
   if (preparing_for_deletion) return; // already done
	if (body != NULL && body->decrement_ref_count()) { // moved here on 130504
#if TT_DEBUG_ON
      if (tt_debug_mode == 13578 && body != NULL) {
         tt_error_file() << body << " deleting it since ref count is zero." << endl;
      };
#endif
		delete body;
      body = NULL;
	};
	Sprite::prepare_for_deletion();
};

//	preparing_for_deletion = TRUE; // new on 060204
//#if TT_DEBUG_ON
//   if (tt_debug_mode == 1717) {
//      tt_error_file() << "Deleting robot#" << debug_counter << " on frame " << tt_frame_number << endl;
//   };
//	if (debug_counter == tt_debug_target) {
//		log(_T("Preparing for deletion of robot debug target."));
//	};
//#endif
//
//#if TT_DEBUG_ON
//   if (tt_debug_mode == 13578 && body != NULL) {
//      tt_error_file() << body << " decrementing ref count since robot is being destroyed. " << this << endl;
//   };
//#endif
////	if (first_in_line != NULL) {
////		first_in_line->destroy(); // decrement ref count (new on 251000)
////	};
//	// following moved back to the end on 030504 since removing followers and the like work better and original reason for it to be in the middle
//	// is now handled better by preparing_for_deletion = TRUE
//	Sprite::prepare_for_deletion(); // moved here from the bottom on 211200 since some of the below (Queue::remove_items_of_robot for example) can recursively call this
//};

void Robot::release_all_references() { // pulled from Robot::prepare_for_deletion on 110504
	if (releasing_all_references) { // new on 230305
		return;
	};
	boolean saved_releasing_all_references = releasing_all_references;
	releasing_all_references = TRUE;
	if (name_from_user != NULL) {
		if (name_from_user->pointer_to_leader() == NULL) { // new on 160103
			// only becomes a follower the first time it is displayed
			name_from_user->destroy(this);
		};
		name_from_user = NULL; // beginning 120504 this is done even if the leader is NULL
	};
	if (tt_queue != NULL) { // moved the deletion of tt_queue later so should be safe && !tt_shutting_down) {
		tt_queue->remove_items_of_robot(this);
	};
//	if (tt_running_queue != NULL) { // new on 090400 - can it be otherwise??
//		tt_running_queue->remove_items_of_robot(this);
//	};
	if (tt_running_robots != NULL) { // cleanup of tt_running_robots moved to later so should be OK && !tt_shutting_down) {
//		boolean found; // added 290100
		tt_running_robots = tt_running_robots->remove(this); //,&found);
		// commented out tt_running_robot ref count stuff on 090400 since causes robots to not be deleted and to run (as orphans)
		// and when robot is prepared for deletion it removes itself from tt_running_robot
//		if (found) decrement_ref_count(); 
	};
//	if (!tt_shutting_down) { // otherwise ~Sprite will take care of things
//		if (thought_bubble != NULL) {
//			remove_follower(thought_bubble);
//         tt_screen->remove(thought_bubble);
//			thought_bubble->destroy(by);
//		};
//		if (next_robot != NULL) {// && responsible_for_all_parts) {
//			remove_follower(next_robot);
//			next_robot->destroy(by);
//		};
//	};
	// following is new on 090400 since if saved_next_robot isn't a follower
	if (tt_reclaiming_memory) { 
		if (saved_next_robot != NULL) {// && responsible_for_all_parts) {
			saved_next_robot->remove_from_floor(FALSE);
			saved_next_robot->set_first_in_line(NULL); // new on 271000
			saved_next_robot->destroy(); // removed by on 060204
		};
	};
	saved_next_robot = NULL; // new on 110504
	if (next_robot != NULL) { // new on 271000
		next_robot->set_first_in_line(NULL);
		if (next_robot->pointer_to_first_to_fail_for_unstable_reasons() == this) {
			next_robot->set_first_to_fail_for_unstable_reasons(NULL);
		};
		next_robot = NULL; // OK? - new on 110504
	};
//	robot_to_put_behind = NULL; // new on 130504
	set_robot_to_put_behind(NULL); // rewrote the above on 230105
	if (working_set != NULL) { // items themselves part of backgrounds items
		release_working_set_elements(); // new on 280300 to release ref counts
		delete [] working_set;
      working_set = NULL;
	};
	if (vacuum != NULL) {
		// if grasped floor will take care of it
//      floor->remove_item(vacuum);
		if (vacuum == tool) {
			remove_follower(vacuum,FALSE);
		};
		vacuum->destroy();
		vacuum = NULL; // was commented out prior to 090999
	};
	if (copier != NULL) {
//		floor->remove_item(copier);
		if (copier == tool) {
			remove_follower(copier,FALSE); // FALSE added 100101
		};
		copier->destroy();
		copier = NULL; // was commented out prior to 090999
	};
   if (expander != NULL) {
		if (expander == tool) {
			remove_follower(expander,FALSE);
		};
		expander->destroy();
		expander = NULL; // new on 090999
	};
//	if (deleted_sprites != NULL) {
//		deleted_sprites->delete_all();
//		delete deleted_sprites;
////		deleted_sprites = NULL;
//	};
	if (page_number_offsets != NULL) {
		delete page_number_offsets;
		page_number_offsets = NULL; // added 261199 since this crashed when robot vacuumed up his box and used page offsets
	};
	reset_recent_page_numbers(); // new on 301000
   // both the following had  && !tt_shutting_down -- why
	if (home_notebook != NULL) {
		home_notebook->destroy(); // will only decrement_ref_count if shared
		home_notebook = NULL; // new on 110504
	};
	if (team_notebook != NULL) {
		team_notebook->destroy(); // ditto
		team_notebook = NULL; // new on 110504
	};
   // initial_tool should be item of the floor or follower so don't need the following - uncommented out on 071100 since was a leak
   if (initial_tool != NULL) { 
		//  && initial_tool != tool) { - not clear why I had this condition (removed 071100)
      if (initial_tool->pointer_to_leader() != this && !initial_tool->destroy()) {
			// added first condition on 010704 since no need to do this if is a follower as well (typically is so -- always?)
			initial_tool->destroy(); // added on 071100 since could be referenced by both picking it up and by being the initial_tool
		};
		initial_tool = NULL;
   };
//   if (saved_next_robot != NULL) {
//      tt_error_file() << "debug" << endl;
//   };
//	if (name_from_user != NULL) delete [] name_from_user;
	if (top_cubby != NULL) { // commented out rest on 190103 && !tt_shutting_down -- could use tt_reclaiming_memory) { // new on 190100
		top_cubby->destroy(); // release reference
		top_cubby = NULL; // since this can be called by stop_team later - but isn't that screwy? 270103
	};
	thought_bubble = NULL; // still a follower and will be taken care of as such
	releasing_all_references = saved_releasing_all_references; 
	// restore setting so the following doesn't get confused and think it is in a recursive call
	Sprite::release_all_references();
};

Sprite *Robot::copy(boolean also_copy_offsets) {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log("Copying debug target (robot)");
	};
#endif
	Robot *next_robot_copy = NULL;
	if (next_robot != NULL) {
		next_robot_copy = (Robot *) (next_robot->copy(TRUE));
	} else if (saved_next_robot != NULL) {
		next_robot_copy = (Robot *) (saved_next_robot->copy(TRUE));
	};
	Thought_Bubble *thought_bubble_copy = NULL;
	if (thought_bubble != NULL) {
		UnwindProtect<boolean> set(tt_copy_restores_pre_blank,FALSE); // 090799 don't restore thought bubble cubby recursively
		thought_bubble_copy = (Thought_Bubble *) thought_bubble->copy(TRUE); 
// added by add_item's recursive call when robot is added
//		if (floor != NULL) {
//			floor->add_item(thought_bubble_copy);
//		};
	} else if (infinite_stack()) {
      thought_bubble_copy = new_thought_bubble(FALSE);
	};
	Robot *copy = new Robot(llx,lly,
									current_priority-1,
									width,height,
//									thought_bubble_copy,
									body,max_working_set_size);
	if (body != NULL) {
		body->increment_ref_count();
		copy->set_version(version); // moved here on 120604 since if not trained then is always default_robot_version
#if TT_DEBUG_ON
      if (tt_debug_mode == 13578) {
         tt_error_file() << body << " incrementing ref count since robot is being copied. " << copy << endl;
      };
#endif
//#if TT_DEBUG_ON
//      copy->set_body_version_number(body_version_number);
//#endif
	};
//	copy->update_display(); // new on 161199 important so robot behind and name are placed correctly -- commented out on 260103
//						next_robot_copy);
//	if (saved_width != 0) {
//		copy->set_saved_size(saved_width,saved_height);
//	};
   if (initial_tool != NULL) {
//      copy->add_initial_tool(initial_tool->copy());
		// rewrote the above on 260103
		Sprite *initial_tool_copy = initial_tool->copy(TRUE);
		initial_tool_copy->increment_ref_count(); // since add_initial_tool does so indirectly
		copy->set_initial_tool(initial_tool_copy);
		initial_tool_copy->set_priority_fixed(TRUE); // taken care of by robot until released
		initial_tool_copy->set_priority(copy->priority()-1);
		initial_tool_copy->set_held_status(HELD_BY_ROBOT);
		copy->add_follower(initial_tool_copy,FALSE,INSERT_BY_PRIORITY,TRUE);		
   };
   copy->set_training_counter(training_counter);
	if (name_from_user != NULL) { // moved here on 121199 so is made blank below if need be
//		// and then on 151199 moved even earlier since this triggers update_display which updates old_x_offset etc.
//		copy->set_name_from_user((Text *) name_from_user->copy());
////		copy->set_parameter(parameter); // commented out on 151199 // to trigger stuff if there is a name
//		fit_name_on_chest(TRUE); // new on 151199
		// rewrote the above on 260103
		copy->set_name_from_user((Text *) name_from_user->copy(TRUE));
	//} else {
	//	copy->update_display(); // commented out on 260103
	};
	if (next_robot_copy != NULL) {
//		copy->put_behind_me(next_robot_copy,0); // restored here on 161199
		// rewrote the above on 260103
		next_robot_copy->set_priority(copy->priority()+1); // under me too
		next_robot_copy->set_priority_fixed(TRUE); // until broken apart
		copy->just_set_next_robot(next_robot_copy);
		copy->add_follower(next_robot_copy,FALSE,INSERT_BY_PRIORITY,TRUE);
//		copy->set_next_robot(next_robot_copy);
//		city_coordinate behind_robot_llx, behind_robot_lly;
//		copy->location_behind_me(behind_robot_llx,behind_robot_lly);
//		next_robot_copy->move_to(behind_robot_llx,behind_robot_lly);
//		copy->add_follower(next_robot_copy); // new on 151199 - both simpler and avoids bad geometry problems
//		copy->set_next_robot(next_robot_copy);
		next_robot_copy->reset_first_in_line(copy); 
//		copy->put_behind_me(next_robot_copy);
	};
	if (thought_bubble_copy != NULL) {
//		thought_bubble_copy->set_robot(copy);
//		copy->set_parameter(ROBOT_DROPING_ITEM); // commented out on 151199 // to attach to a better spot
//		copy->update_display();
		if (thought_bubble == NULL) {
			// was unconditional prior to 260103
			copy->update_display();
			copy->connect_to_thought_bubble(thought_bubble_copy);
		} else {
			thought_bubble_copy->set_priority(copy->priority());
			copy->add_follower(thought_bubble_copy,FALSE,INSERT_BY_PRIORITY,TRUE);
			copy->set_thought_bubble(thought_bubble_copy);
			thought_bubble_copy->set_robot(copy);
		};
//		if (tt_log_version_number < 15) { // added on 270499 so looks nicer including name
//			copy->set_parameter(current_parameter());
//		};
	};
   if (home_notebook != NULL ) copy->set_home_notebook(home_notebook);
//   copy->set_priority_fixed(priority_fixed_flag);
	if (thought_bubble != NULL && thought_bubble->is_blank() && !tt_copy_restores_pre_blank) {
		// add !tt_copy_restores_pre_blank on 090799 so wand restores properly
		copy->become_blank();
	};
	//if (comment != NULL) {
	//	copy->set_comment(comment->copy());
	//};
   finish_making_copy(copy,also_copy_offsets);
	return(copy);
};

Thought_Bubble *Robot::new_thought_bubble(boolean connect_now, Thought_Bubble *thought_bubble) { 
	// abstracted on 230802 -- optional thought_bubble arg added for XML loading
	if (thought_bubble == NULL) {
		thought_bubble = new Thought_Bubble(0,0,current_priority);
	};
	thought_bubble->set_size(thought_bubble->width_given_scale(x_scale),
									 thought_bubble->height_given_scale(y_scale));
   if (connect_now) {
      connect_to_thought_bubble(thought_bubble);
   };
   return(thought_bubble);
};

void Robot::generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection) {
	if ((running_body_flag || suspended() ||
		  current_parameter() == robot_waiting_state) &&
		 tt_help_counter == 0)  {
		Sprite::generate_help(message,tool_in_hand,selection);
		return;
	};
	if (body != NULL) {
		describe_pointing(message,tool_in_hand,selection);
	};
	Cubby *cubby = NULL;
	if (!infinite_stack()) {
		cubby = thought_bubble->pointer_to_cubby();
	};
	if ((cubby == NULL && !did(ROBOT_GIVEN_CUBBY)
		  && tt_help_counter == 0) || 
		 tt_help_counter == 1) {
		tt_help_history_action = ROBOT_GIVEN_CUBBY;
		if (body == NULL) { // "robot without a thought" case
			describe_pointing(message,tool_in_hand,selection);
		};
		message << S(IDS_ROBOT_TRAIN1) << S(IDS_ROBOT_TRAIN2) << S(IDS_ROBOT_TRAIN3);
	} else if ((body != NULL && !did(ROBOT_ABSTRACTED)
				  && tt_help_counter == 0) || 
				  tt_help_counter == 2) {
		tt_help_history_action = ROBOT_ABSTRACTED;
		if (tt_help_counter == 0) {
			message << S(IDS_ROBOT_ACCEPTS);
			cubby->describe(message,english()?PLURAL_ARTICLE:INDEFINITE_ARTICLE);
			message << end_sentence();
		} else {
			message << S(IDS_ROBOT_ACCEPTS2);
		};
		message << S(IDS_ROBOT_LESS_FUSSY1)
				  << S(IDS_ROBOT_LESS_FUSSY2)
				  << S(IDS_ROBOT_LESS_FUSSY3);
	} else if ((body != NULL && !did(ROBOT_TRIED)
				  && tt_help_counter == 0) || 
				  tt_help_counter == 3) {
		tt_help_history_action = ROBOT_TRIED;
//		if (tt_help_counter == 0) {
//			describe_pointing(message,tool_in_hand,this);
//		};
		message << S(IDS_ROBOT_TRY_OUT1)
				  << S(IDS_ROBOT_TRY_OUT2)
				  << S(IDS_ROBOT_TRY_OUT3);
	} else if ((body != NULL && !did(ROBOT_SAVED)
				  && tt_help_counter == 0) || 
				  tt_help_counter == 4) {
		tt_help_history_action = ROBOT_SAVED;
		message << S(IDS_ROBOT_SAVE);
	} else if ((body != NULL && !robot_loaded_in_truck()
				  && tt_help_counter == 0) || 
				  tt_help_counter == 5) {
		message << S(IDS_ROBOT_LOAD_TRUCK1)
				  << S(IDS_ROBOT_LOAD_TRUCK2)
				  << S(IDS_ROBOT_LOAD_TRUCK3);
	} else if ((body != NULL && !did(ROBOTS_LINED_UP)
				  && tt_help_counter == 0) || 
				  tt_help_counter == 6) {
		tt_help_history_action = ROBOTS_LINED_UP;
		message << S(IDS_ROBOTS_LINE_UP1);
		message << S(IDS_ROBOTS_LINE_UP2);
		message << S(IDS_ROBOTS_LINE_UP3);
		message << S(IDS_ROBOTS_LINE_UP4);
		message << S(IDS_ROBOTS_LINE_UP5);
		message << S(IDS_ROBOTS_LINE_UP6);
		message << S(IDS_ROBOTS_LINE_UP7);
		message << S(IDS_ROBOTS_LINE_UP8);
		message << S(IDS_ROBOTS_LINE_UP9);
	} else if ((!did(TYPE_HELP_GIVEN) && tt_help_counter == 0) || 
				  tt_help_counter == 7) {
//		tt_help_history_action = TYPE_HELP_GIVEN;
		message << S(IDS_ROBOT_HELP_GIVEN1) << S(IDS_ROBOT_HELP_GIVEN2);
		no_help_interruptions();
		user_did(ROBOT,TYPE_HELP_GIVEN);
	};
};

void Robot::set_body(Events *events) { 
	// only used by programmer
	boolean retrained = (body != NULL);
	if (retrained && body->decrement_ref_count()) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 13578 && body != NULL) {
			tt_error_file() << body << " deleting it since ref count is zero." << endl;
		};
#endif
		delete body;
	};
	body = events;
	if (body != NULL) { // new on 040504 so alway have the correct max_working_set_size
		if (body->first()->event_code() == SERIAL_NUMBER) {
			Event *start_event = body->rest()->first();
			if (start_event->event_code() == START_EVENT) {
            max_working_set_size = start_event->pointer_to_path()->return_index(); 
				// set_max_working_set_size does too much -- this is just initialization so should be OK
			} else {
				tt_error_file() << "Warning. The robot's actions are missing a start event." << endl;
				max_working_set_size = 100;
			};
		} else {
			tt_error_file() << "Warning. The robot's actions are missing a serial number." << endl;
			max_working_set_size = 100;
		};
	};
   training_counter = NO_TRAINING_COUNTER;
   // new policy on 311002 after much email discussion that this was a bad idea
	//if (retrained) { // conditional added 140599 so name is reset only if retrained
	//	set_name_from_user(NULL); // added on 270499 so that retrained robots show a different name
	//	name_from_user->set_visible(FALSE); // added 140599 since will be reset to TRUE once it settles down
	//};
	if (body != NULL && parameter == ROBOT_BREATHING) { // new on 141204
		set_parameter(ROBOT_INERT);
	};
};

unsigned long int *robot_classes_already_defined; // to avoid translating the same robot twice -- was short prior to 240702
flag *robots_already_named;
flag *saved_robots_already_named;
int robot_classes_already_defined_count = 0;
flag *cycles_handled; // to avoid setting up same image cycle twice in Java

void start_new_java_context() { // new on 180201
	// warning this doesn't nest properly
	saved_robots_already_named = robots_already_named;
	robots_already_named = NULL;
};

void end_new_java_context() { // new on 180201
	robots_already_named = saved_robots_already_named;
};

void ensure_image_cycles(int code) {
   if (cycles_handled[code]) return; // taken care of already
   cycles_handled[code] = TRUE;   // do later
};

void really_ensure_image_cycles(java_stream &out) {
	java_indent(out);
	out << "private void intializeToonTalkImages() {" << endl;
	tt_java_indent += 2;
   for (int i = 0; i < tt_appearance_count; i++) {
      if (cycles_handled[i]) {
         java_image_cycle(out,i);
      };
   };
	tt_java_indent -= 2;
	java_indent(out);
	out << "}" << endl;
};

Sprites *Robot::reachable_sprites_of_type(SpriteType type) {
	if (type == ROBOT) {
		return(new Sprites(this)); // others in team handled elsewhere (I think)
	} else {
		return(NULL);
	};
};

Sprites *Robot::reachable_non_equal_sprites_of_type(SpriteType type) {
	if (type == ROBOT) {
		return(new Sprites(this)); // others in team handled elsewhere (I think)
	} else {
		return(NULL);
	};
};

string Robot::java_start(Cubby *cubby, Text *text, Notebook *notebook,
                         Picture *picture, Sprites *more_robots, Sprites *more_notebooks, Sprite *display_this, java_string extra_text) {
   if (first_in_line != NULL) { 
      return(first_in_line->java_start(cubby,text,notebook,picture,more_robots,more_notebooks,display_this));
   };
   if (body == NULL && thought_bubble->pointer_to_cubby() == NULL &&
       // just holding a fresh untrained robot so ignore this
       cubby == NULL && text == NULL && notebook == NULL && picture == NULL &&
		 display_this == NULL) return(NULL);
   if (!did_user(USER_HAS_GENERATED_JAVA_APPLET)) { // first time
#if TT_TRIAL_VERSION
		message_box(MB_ICONINFORMATION,S(IDS_TRIAL_VERSION_DOES_NOT_GENERATE_JAVA),NULL,FALSE);
#else
      message_box(MB_ICONINFORMATION,S(IDS_ABOUT_TO_GENERATE_JAVA_APPLET),NULL,FALSE);
#endif
   };
	ascii_string short_name = NULL; // moved out here from !TT_TRIAL_VERSION on 180604 since this is returned now
#if !TT_TRIAL_VERSION
   if (text != NULL) {
      string text_string;
      long text_length;
      if (text->current_text(text_string,text_length) &&
          text_length > 0) {
//         short_name = new char[text_length+1];
//         memcpy(short_name,text_string,text_length);
//         short_name[text_length] = '\0'; // terminate string
			short_name = copy_unicode_to_ascii(text_string);
			keep_only_alphanumerics(short_name); // added 130600 since names often contain new lines and spaces
      };
   };
   int count;
   char file_name[MAX_PATH];
   output_string_stream name(file_name,MAX_PATH);
	if (short_name == NULL && name != NULL) {
		short_name = copy_string_name_from_user();
		if (short_name != NULL) keep_only_alphanumerics(short_name);
	};
   boolean user_provided_name = (short_name != NULL && short_name[0] != '\0'); 
	// second condition new on 040904 since name could be all white space
   if (user_provided_name) {
		if (short_name[0] >= '0' && short_name[0] <= '9') { 
			// new on 150700 - user provided name starts with digit so prepend "Applet_"
			string new_short_name = append_strings("Applet_",short_name);
			delete [] short_name;
			short_name = new_short_name;
		};
	} else {
      short_name = new char[16];
      if (fully_defined()) {
//         short_name = copy_string(java_class_name()); // of this robot
#if TT_32
         strcpy(short_name,"Applet_");
#else
         strcpy(short_name,"AP_");
#endif
         serial_number_to_name(serial_number(),short_name+strlen(short_name),TRUE);
         user_provided_name = TRUE; // a little white lie
      } else {  // should use registry instead??
			count = increment_ini_int(AC(IDC_SWITCHES),AC(IDS_TEAM_NAME_COUNTER),FALSE); 
			// FALSE replace TRUE on 050404 since this would break demos that were paused and an applet generated
 //        count = GetPrivateProfileIntG(AC(IDC_SWITCHES),AC(IDS_TEAM_NAME_COUNTER),0,AC(IDC_TT_INI));
 // 	      char count_string[16];
 //       	itoa(count+1,count_string,10);
 // 	      WritePrivateProfileStringG(AC(IDC_SWITCHES),AC(IDS_TEAM_NAME_COUNTER),count_string,AC(IDC_TT_INI));
         output_string_stream name(short_name,16);
         name << "TT" << count;
         name.put('\0');
      };
   };
	tt_java_class_name = short_name; // new on 310101
//#if TT_INSTALL_SHIELD2
//   string java_directory = ini_entry("Directories","JavaDir");
//#else
//   string java_directory = append_strings(tt_main_directory,"Java\\"); // new_file_name("Java","\\");
//#endif
//   tt_java_subdirectory = short_name;
   char java_directory[MAX_PATH];
   output_string_stream java_directory_stream(java_directory,MAX_PATH);
   char directory_short_path[MAX_PATH];
   string main_directory;
   if (tt_java_files_shared) {
      main_directory = tt_shared_files_directory;
   } else {
      main_directory = tt_user_directory;         
   };
   GetShortPathName(main_directory,directory_short_path,MAX_PATH); 
	// since otherwise have problems calling Jikes and other Java programs from javashow.bat
   // following idea of putting each applet in a subdirectory is cute but package path name problems break it
   // but it doesn't always - very confusing why long path names sometimes confuses IE
   java_directory_stream << directory_short_path << AC(IDC_JAVA_PATH); 
	// << short_name << "\\"; // was tt_main_directory prior to 030402
   java_directory_stream.put('\0');
	if (::CreateDirectory(java_directory,NULL)) { // new on 050402
      // wasn't already created so need to copy Java archives over here (new on 060402)
		// prior to 050503 these were harded-coded to use ttclass
	};
	ascii_string archive = ini_entry(AS(IDS_JAVA),AS(IDS_ARCHIVE),FALSE); // moved here on 050503
	if (archive == NULL) {
		archive = copy_string("ttclass2.zip"); // new on 041200 -- 2 added on 060503
//		archive = ensure_file_for_java("ttclass",NONE_GIVEN,TRUE,"zip"); 
		// better than the above since will check CD-ROM and copy if needed (new on 240201)
//		if (archive == NULL) { // new on 240201
//			archive = copy_string("ttclass.zip");
//			tt_error_file() << "Java applet won't compile without the file: " << archive << endl;
//		};
	};
	ascii_string cabbase = ini_entry(AS(IDS_JAVA),AS(IDS_CABBASE),FALSE);
	// starting 050503 to copy the following anyway since may have been updated - used to be in the conditional above
   copy_system_file_to_directory(archive,AC(IDC_JAVA_PATH),java_directory);
   copy_system_file_to_directory(cabbase,AC(IDC_JAVA_PATH),java_directory);
//      copy_system_file_to_directory("system.zip",AC(IDC_JAVA_PATH),java_directory); // needed only for Jikes - if I switch to using javac then don't bother
   name << java_directory;
   ascii_string java_extension = ini_entry(AC(IDS_FILE_EXTENSIONS),AS(IDS_JAVA_FILE_SUFFIX),FALSE);
   if (java_extension == NULL) {    // shouldn't really happen
      java_extension = copy_ascii_string("txt");
   };
   name << short_name << "." << java_extension;
   name.put('\0');
   output_file_stream out(file_name,std::ios_base::out);
	if (out == NULL) { // new on 290904 for error recovery
		unable_to_write_file(file_name); 
		return(short_name);
	};
   out << S(IDS_JAVA_FILE_GENERATED_COMMENT) << " " << tt_user_name << endl;
   out << S(IDS_JAVA_IMPORT_TOONTALK_PACKAGE) << endl;
   robot_classes_already_defined = new unsigned long int[max_robot_classes_per_file];
   robots_already_named = new flag[max_robot_classes_per_file];
   int i;
   for (i = 0; i < max_robot_classes_per_file;i++) {
      robot_classes_already_defined[i] = 0;
      robots_already_named[i] = FALSE;
   };
   cycles_handled = new flag[tt_appearance_count];
   for (i = 0; i < tt_appearance_count;i++) cycles_handled[i] = 0;
   tt_java_indent = 2;
   tt_main_notebook_referenced = FALSE; // until proven otherwise - probably other occurance does the real work
	boolean needs_restoration = FALSE;
	if (cubby == NULL && thought_bubble != NULL) {
       cubby = thought_bubble->pointer_to_cubby();
		 if (cubby != NULL) needs_restoration = TRUE; // since can from thought bubble
   };
	Cubby *cubby_restored; // new on 310101
   if (cubby == NULL || needs_restoration) {
      cubby_restored = restored_cubby(cubby);
		// moved here on 271100 so applies even if the cubby is NULL
		// tried commenting out the following since this is the box (like using the wand with 'o') 
		// and so shouldn't behave like it is in the thought bubble
		// but that broke lots of things (e.g. dumping of "hitting?" sensor in thought bubble. Besides the above deals with the 'o' behavior
		thought_bubble->add_follower(cubby_restored); // new on 220700 since some :java(...) checks this - why am I making a copy anyway?
   } else {
      cubby_restored = cubby;
   };
	// need to see if there are robots or notebooks in the cubby_restored
	Sprites *extra_robots = cubby_restored->reachable_non_equal_sprites_of_type(ROBOT); // made non_equal on 250601
	insert_sprites_at_end_if_not_equal_member(more_robots,extra_robots); // abstracted on 180201 (was "open coded: previously)
	if (notebook != NULL) { // new on  180201
		extra_robots = notebook->reachable_non_equal_sprites_of_type(ROBOT);
		insert_sprites_at_end_if_not_equal_member(more_robots,extra_robots);
	};
	insert_at_end_if_not_equal_member(this,more_robots);
	Sprites *extra_notebooks = cubby_restored->reachable_non_equal_sprites_of_type(PROGRAM_PAD);
	insert_sprites_at_end_if_not_equal_member(more_notebooks,extra_notebooks); // started using equal test on 250601
	if (notebook != NULL) { // new on  180201
		extra_notebooks = notebook->reachable_non_equal_sprites_of_type(PROGRAM_PAD);
		insert_sprites_at_end_if_not_equal_member(more_notebooks,extra_notebooks); // started using equal test on 250601
		insert_at_end_if_not_equal_member(notebook,more_notebooks); // started using equal test on 250601
	};
//   java_class(out); // Robots first - commented out on 180201 since part of more_robots now
   if (more_robots != NULL) { 
		// these are reachable from picture but if there is no picture might want to see what's reachable from this (e.g. in a box)
      Sprites *remaining = more_robots;
      while (remaining != NULL) {
         remaining->first()->java_class(out);
         remaining = remaining->rest();
      };
   };
//   if (notebook != NULL) notebook->java_class(out); -- added to more_notebooks now
	if (more_notebooks != NULL) { 
      Sprites *remaining = more_notebooks;
      while (remaining != NULL) {
         remaining->first()->java_class(out);
         remaining = remaining->rest();
      };
   };
   out << S(IDS_JAVA_PUBLIC_CLASS) << " " << short_name << " "
       << S(IDS_JAVA_EXTENDS_TT_APPLET) << endl;
   out << S(IDS_JAVA_VOID_MAIN) << endl;
   out << S(IDS_JAVA_MAIN_BODY1) << short_name << S(IDS_JAVA_MAIN_BODY2) << endl;
   out << " }" << endl;
   out << " " << S(IDS_JAVA_PUBLIC_VOID_INIT) << endl;
	java_send_this(out,"intializeToonTalkImages"); // new on 290101 -- moved here on 310101
	java_send_this(out,"intializeNests"); // new on 170201
	dump_started(); // new on 111200 since use nest indices when "javaizing" nests
   if (notebook != NULL) {
      java_declare(out,S(IDS_JAVA_TT_NOTEBOOK),S(IDS_JAVA_NOTEBOOK),notebook);
//      out << "  TTNotebook notebook = ";
//      notebook->java(out);
//      out << ";" << endl;
      notebook->java_after(out,S(IDS_JAVA_NOTEBOOK));
   } else {
//      out << "  TTNotebook notebook = TT.NOTEBOOK;" << endl;
      if (tt_main_notebook_referenced) {
         java_declare(out,S(IDS_JAVA_TT_NOTEBOOK),S(IDS_JAVA_NOTEBOOK),tt_toolbox_notebook);
         // Java generator only works in Free Play where tt_toolbox must be non-NULL
			tt_toolbox_notebook->java_after(out,S(IDS_JAVA_NOTEBOOK));
      } else {
         java_declare(out,S(IDS_JAVA_TT_NOTEBOOK),S(IDS_JAVA_NOTEBOOK),S(IDS_JAVA_MAIN_NOTEBOOK));
      };
   };
	// moved here on 140600 so don't have multiple declarations of same thing
	java_declare(out,S(IDS_JAVA_TT_ROBOT),S(IDS_JAVA_NEXT_ROBOT));
	UnwindProtect<java_string> save(tt_java_picture_name); // new on 021100
	java_string team_name = SC(IDS_JAVA_TEAM,java_index(this)); // new on 061200
   if (picture != NULL) {
//      picture->java_associate_team();
//      java_declare(out,S(IDS_JAVA_TT_PICTURE),picture_name,picture);
//      java_send_this(out,S(IDS_JAVA_ADD_PICTURE),picture_name);
//      java_assign(out,picture_name,picture);
		picture->java_before(out);
      tt_java_picture_name = copy_string(picture->java_name()); // SC(IDS_JAVA_PICTURE);  
		// used deep inside of generating Java for remote controls	
//      picture->java_followers(out,tt_java_picture_name);
//      picture->java_flipped_followers(out,tt_java_picture_name);
      java_send_this(out,S(IDS_JAVA_SET_PICTURE),tt_java_picture_name);
		picture->java_after(out,tt_java_picture_name); // added on 031200 to deal with followers more modularly
		java_send(out,tt_java_picture_name,S(IDS_JAVA_START)); // moved here on 041200
		string screen_snapshot_name = append_strings(short_name,".gif");
		string quoted_screen_snapshot_name = java_quote(screen_snapshot_name);
		java_assign(out,"screen_snapshot_file_name",quoted_screen_snapshot_name); 
		// new on 110405 to display snapshot before starting
		delete [] screen_snapshot_name;
		delete [] quoted_screen_snapshot_name;
   } else {
      if (display_this == NULL) {
//         java_assign(out,S(IDS_JAVA_DISPLAYED_BOX),S(IDS_JAVA_BOX));
         if (fully_defined()) { // not an untrained robot
            java_team(out,cubby_restored); // arg was cubby prior to 310101
            java_send_this(out,S(IDS_JAVA_SET_STARTING_TEAM),team_name);
         } else if (cubby != NULL) {
//            java_assign(out,S(IDS_JAVA_BOX),cubby);
            cubby->java_after(out,S2(IDS_JAVA_BOX,java_index(cubby))); // updated 041200
            cubby->java_describe_as_comment(out,S(IDS_JAVA_AND_THEN_CREATE));
         };
			string the_box = append_strings(team_name,".getBox()"); // new on 231200 - could add to javacom.rc
         java_send_this(out,S(IDS_JAVA_DISPLAY_THIS),the_box); // S2(IDS_JAVA_BOX,java_index(cubby)));
			delete [] the_box;
      } else {
         display_this->java_before(out);
//          java_assign(out,S(IDS_JAVA_DISPLAYED_BOX),display_cubby);
         display_this->java_after(out,S(IDS_JAVA_DISPLAYED_BOX));
         if (display_this->kind_of() == PICTURE) {
            Picture *picture = (Picture *) display_this;
            tt_java_picture_name = copy_string(picture->java_name());
            picture->java_flipped_followers(out,tt_java_picture_name);
            java_send(out,tt_java_picture_name,S(IDS_JAVA_START));
         };
         java_send_this(out,S(IDS_JAVA_DISPLAY_THIS),display_this);
      };
   };
	if (tt_java_picture_name != NULL) {
	   delete [] tt_java_picture_name;
		tt_java_picture_name = NULL; // new on 081003
	};
//   if (more_robots != NULL) {
//      Sprites *remaining = more_robots;
//      while (remaining != NULL) {
//         Robot *robot = (Robot *) remaining->first();
//         robot->java_team(out,robot->pointer_to_working_cubby());
//         java_send(out,S(IDS_JAVA_PICTURE),S(IDS_JAVA_ADD_TEAM),S(IDS_JAVA_TEAM));
//         remaining = remaining->rest();
//      };
//   };
   out << " }" << endl;
	java_postponed_followers(out); // new on 290101
	java_declare_all_nests(out); // new on 170201
	java_assign_all_nests(out);
	dump_ended(); // new on 111200 -- moved here on 170201
   really_ensure_image_cycles(out);
   out << "}" << endl;
   out.close();
   ascii_string compiler = ini_entry(AC(IDC_EXECUTABLES),AS(IDS_JAVA_COMPILER),FALSE);
   boolean default_compiler = (compiler == NULL);
   FileNameStatus file_status;
	// don't allow URL caching for security reasons
	// added args on 170200 so won't search paths
//   if (default_compiler) compiler = 
	// existing_file_name(file_status,AS(IDS_JUMP),AS(IDS_EXE),AS(IDS_JUMP),FALSE,TRUE,FALSE,FALSE,FALSE);
	if (default_compiler) compiler = existing_file_name(file_status,"Jikes",AS(IDS_EXE),"Jikes",FALSE,TRUE,FALSE,FALSE,FALSE); 
	// updated 071200
   if (compiler == NULL) return(NULL);
   char batch_file_name[MAX_PATH];
   // create my own batch file because passing args exceeds DOS command line limit
   // the batch file ensures that first the compiler finishes and then the browser is started
   output_string_stream batch_name(batch_file_name,MAX_PATH);
   batch_name << java_directory << S(IDS_JAVASHOW_BAT);
   batch_name.put((char) 0);
   output_file_stream batch(batch_file_name,std::ios_base::out);
	if (batch == NULL) { // new on 290904 for error recovery
		unable_to_write_file(batch_file_name); 
		return(short_name);
	};
	batch << "del " << short_name << ".java" << endl; // new on 140201 in case already exists (e.g. left over from a previous error)
	batch << "del " << short_name << ".class" << endl; // new on 170201 in case the compilation fails and an old class is still around
	batch << "rename " << short_name << ".txt " << short_name << ".java" << endl; // new on 281100
   batch << S(IDS_CALL) << compiler;
   batch << S(IDS_O_CLASSPATH);  // optimized
	// don't cache if URL the following since will be used by browser
	// added args on 170200 so won't search paths
   ascii_string system_archive = 
		existing_file_name(file_status,AS(IDS_SYSTEM),AS(IDS_ZIP),AS(IDS_JAVA),FALSE,TRUE,FALSE,FALSE,FALSE);
   if (system_archive != NULL) {
      batch << system_archive << ";";
		if (file_status != FILE_ON_HARD_DRIVE) {
			copy_file_to_toontalk_directory(system_archive,AS(IDS_JAVA),AS(IDS_SYSTEM),AS(IDS_ZIP)); // new on 260201
		};
      delete [] system_archive;
   };
   /* commented out on 060402 since now these files are created when user's Java directory is created
   // since this is needed to run the applet as well
	// added args on 170200 so won't search paths
   ascii_string toontalk_package_archive = existing_file_name(file_status,AS(IDS_TT_CLASS),AS(IDS_ZIP),AS(IDS_JAVA),FALSE,TRUE,FALSE,FALSE,FALSE);
   if (toontalk_package_archive != NULL) {
      batch << toontalk_package_archive << ";";
		if (file_status != FILE_ON_HARD_DRIVE) {
			copy_file_to_toontalk_directory(toontalk_package_archive,AS(IDS_JAVA),AS(IDS_TT_CLASS),AS(IDS_ZIP)); // new on 260201
		};
      delete [] toontalk_package_archive;
   };
   */
   batch << archive << " " << short_name << ".java" << endl; // was file_name prior to 281100
	batch << "rename " << short_name << ".java " << short_name << ".txt" << endl; 
	// new on 281100 - since browser can only show source as "txt"
	if (tt_commands_to_convert_sounds_to_au) {
		batch << tt_commands_to_convert_sounds_to_au << endl;
		delete [] tt_commands_to_convert_sounds_to_au;
		tt_commands_to_convert_sounds_to_au = NULL;
	}	
	output_string_stream html_name(file_name,sizeof(file_name));
	html_name << java_directory; 
	html_name << short_name << ".htm";  // removed "l" for Win 3.1
	html_name.put('\0');
	output_file_stream html(file_name,std::ios_base::out);
	if (html == NULL) { // new on 290904 for error recovery
		unable_to_write_file(file_name); 
		return(short_name);
	};
	html << S(IDS_HTML_TITLE_TOONTALK) << " ";
	if (user_provided_name) {
		html << short_name << S(IDS_HTML_END_TITLE) << endl;
	} else {
		html << S(IDS_HTML_ROBOT_TEAM_NUMBER) << count << S(IDS_HTML_END_TITLE) << endl;
	};
	html << S(IDS_HTML_HR) << endl;
	html << S(IDS_HTML_APPLET_CODE) << short_name;
	int run_in_new_window = ini_int(AC(IDC_SWITCHES),AS(IDS_RUN_APPLET_IN_NEW_WINDOW),FALSE,0);
	int picture_width;
	int picture_height;
	if (picture != NULL) {
		picture_width = tt_screen->horizontal_pixel_length(picture->current_width());
		picture_height = tt_screen->vertical_pixel_length(picture->current_height());
	};
	int applet_width, applet_height;
	if (picture == NULL) { // these should be toontalk.ini -- right??
		applet_width = 600;
		applet_height = 220; // was 200 but text area scroll bar overlapped sometimes
	} else if (run_in_new_window) {
		applet_width = 200; // 150 was too small for Macs -- then
		applet_height = 40;
	} else {
		applet_width = max(picture_width+2,200);
		applet_height = picture_height+40;
	};
	html << S(IDS_DOT_CLASS) << endl;
	html << S(IDS_HTML_WIDTH) << applet_width << " " << S(IDS_HTML_HEIGHT) << applet_height << endl;
	html << S(IDS_HTML_ARCHIVE) << archive;
	delete [] archive;
	html << endl << ">" << endl;
//	string system_archive = ensure_file_for_java("ttclass",NONE_GIVEN,TRUE,"zip"); // new on 240201
//	if (system_archive == NULL) {
//		tt_error_file() << "Java applet won't compile without the file: ttclass.zip" << endl;
//	} else {
//		delete [] system_archive; // just ensured that it is locally available
//	};
	ascii_string html_extra = ini_entry(AC(IDC_USER),AS(IDS_HTML_EXTRA),FALSE);
	if (picture != NULL) {
		html << S(IDS_HTML_PARAM) << " " << S(IDS_HTML_NAME_EQUALS) << S(IDS_HTML_PICTURE_WIDTH) << " ";
		html << S(IDS_HTML_VALUE_EQUALS) << picture_width << ">" << endl;
		html << S(IDS_HTML_PARAM) << " " << S(IDS_HTML_NAME_EQUALS) << S(IDS_HTML_PICTURE_HEIGHT) << " ";
		html << S(IDS_HTML_VALUE_EQUALS) << picture_height << ">" << endl;
		html << S(IDS_HTML_PARAM) << " " << S(IDS_HTML_NAME_EQUALS) << S(IDS_HTML_RUN_IN_NEW_WINDOW) << " ";
		html << S(IDS_HTML_VALUE_EQUALS) << run_in_new_window << ">" << endl;
	};
	html << S(IDS_HTML_PARAM) << " " << S(IDS_HTML_NAME_EQUALS) << S(IDS_HTML_LANGUAGE) << " ";
	html << S(IDS_HTML_VALUE_EQUALS) << S(IDS_NATURAL_LANGUAGE) << ">" << endl;
	html << S(IDS_HTML_PARAM) << " " << S(IDS_HTML_NAME_EQUALS) << "DigitsAfterDecimalPoint" << " ";
	html << S(IDS_HTML_VALUE_EQUALS) << tt_digits_after_decimal_point_in_java << ">" << endl;
	if (cabbase != NULL) {
		html << S(IDS_HTML_PARAM) << " " << S(IDS_HTML_NAME_EQUALS) << S(IDS_HTML_CABBASE) << " ";
		html << S(IDS_HTML_VALUE_EQUALS) << cabbase << ">" << endl;
		// following is new on 240201
//		string dot_and_extension = strchr(cabbase,'.');
//		if (dot_and_extension != NULL) {
//			dot_and_extension[0] = '\0'; // terminate it
//		};
//		string cabbase_file_name = ensure_file_for_java(cabbase,NONE_GIVEN,TRUE,"cab"); // better to call existing_file_name
		FileNameStatus file_status;
		string cabbase_file_name = existing_file_name(file_status,cabbase,NULL,AS(IDS_JAVA),FALSE,TRUE,FALSE,FALSE,FALSE); 
		// called to copy from CD-ROM to disk if need be
		delete [] cabbase;
		if (cabbase_file_name != NULL) {
			string short_name = short_file_name(cabbase_file_name,TRUE);
			if (file_status != FILE_ON_HARD_DRIVE) {
				copy_file_to_toontalk_directory(cabbase_file_name,AS(IDS_JAVA),short_name); // new on 260201
			};
			delete [] short_name;
			delete [] cabbase_file_name;
		};
	};
	html << S(IDS_HTML_ALT_NO_JAVA) << endl;
	html << S(IDS_HTML_APPLET_END) << endl;
	html << S(IDS_HTML_HR) << endl;
	if (extra_text != NULL) {
		html << "<p> " << extra_text << "</p>" << endl;
	};
	// note that while it makes sense for this to be in the Java category of toontalk.ini
	// there is the older (unused?) HTMLExtra in User
	string java_source_html = ini_entry("Java","JavaSourceHTML",FALSE); // new on 240902
	if (java_source_html != NULL) {
		html << S(IDS_HTML_START_HREF) << short_name << "." << java_extension << ">" << java_source_html << "</a>" << endl;
		delete [] java_source_html;
	};
	string tt_source_html = ini_entry("Java","ToonTalkSourceHTML",FALSE); // new on 240902
	if (tt_source_html != NULL) {
		Sprite *in_hand = tt_programmer->pointer_to_tool_in_hand();
		HGLOBAL clipping_handle;
		//flag saved_dumping_to_clipboard = tt_dumping_to_clipboard;
		//tt_dumping_to_clipboard = TRUE;
		UnwindProtect<DumpReason> set(tt_dump_reason,DUMPING_TO_CLIPBOARD); // maybe DUMPING_TO_XML_FILE is better
#if TT_XML
      // new on 240902
//		tt_saving_media = TRUE; // new on 180303 - caller sets this as of 210303
      clipping_handle = in_hand->xml_global_handle();
//		tt_saving_media = FALSE;
#else
		long encoding_length;
		string encoding = in_hand->return_encoding(encoding_length,FALSE,FALSE);
		clipping_handle = uuencode(encoding,encoding_length,in_hand->kind_of()); 
		// last arg new on 090600 since birds and nests don't want long preamble
	   delete [] encoding;
#endif
#if TT_LOADING_AND_SAVING_INTERRUPTABLE
		if (clipping_handle != NULL) { // rewritten 140604 -- restored on 150105
#endif
			// not sure why this is a good idea to generate the file name even if there is no handled to fill it with -- 
			// noted on 150105
			tt_file_name_of_tt_object_just_saved = new char[MAX_PATH];
			output_string_stream tt_name(tt_file_name_of_tt_object_just_saved,MAX_PATH);
			tt_name << java_directory; 
			tt_name << short_name << ".tt";
			tt_name.put('\0');
			if (clipping_handle != NULL) {
				save_clipping_to_file_name(clipping_handle,tt_file_name_of_tt_object_just_saved);
				::GlobalFree(clipping_handle);
			};
#if TT_LOADING_AND_SAVING_INTERRUPTABLE
		} else {
			tt_file_name_of_tt_object_just_saved = NULL; // just to be sure -- new on 150105
		};
#endif
//		tt_dumping_to_clipboard = saved_dumping_to_clipboard;
		html << S(IDS_HTML_START_HREF) << short_name << ".tt>" << tt_source_html << "</a>" << endl;
		delete [] tt_source_html;
	};
//	html << S(IDS_HTML_START_HREF) << short_name << "." << java_extension << S(IDS_HTML_HREF_END) << endl;
	if (html_extra != NULL) {
		html << html_extra << endl;
		delete [] html_extra;
	};
	html.close();
#if TT_LOADING_AND_SAVING_INTERRUPTABLE
	if (tt_file_name_of_tt_object_just_saved == NULL) { // new on 150105 -- probably interrupted
		return(NULL);
	};
#endif
//   line << " " << file_name; // html file name
//   line.put((char) 0); // terminate string
	boolean progman = (boolean) ini_int(AC(IDC_SWITCHES),AS(IDS_SHELL_IS_PROGMAN),FALSE,0);
	if (!progman) { // doesn't work for 3.1 or NT 3.51
		batch << C(IDC_START) << file_name << endl;
	};
	batch.close();
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
//   int result = (int) ShellExecute(NULL,"open",compile_and_show,command_line,java_directory,SW_SHOWMINIMIZED);
	int result = (int) ShellExecuteG(tt_main_window->get_handle(),AC(IDC_OPEN),batch_file_name,"",java_directory,SW_SHOWMINIMIZED); // first arg was NULL prior to 220601
#if TT_DEBUG_ON
	add_to_log_if_interesting_error(126);
#endif
	if (result < 32) {
		tt_error_file() << S(IDS_JAVA_COMPILER_PROBLEM) << result << S(IDS_RUNNING) << batch_file_name << endl;
      if (tt_java_files_shared) {
         string temp_name = append_strings(batch_file_name,".saved");
			//  on 280103 added MOVEFILE_REPLACE_EXISTING
         move_file(batch_file_name,temp_name); // new on 050402 since leaving it around prevents sharing the same directory with different users
	      delete [] temp_name;
      };
   } else if (progman) { // still have to start browser
		ShellExecuteG(NULL,AS(IDS_OPEN),file_name,"",java_directory,SW_SHOW);
      if (tt_java_files_shared) DeleteFile(batch_file_name); 
		// new on 050402 since leaving it around prevents sharing the same directory with different users
	};
	tt_trouble_shoot_java_applet_file_name = new char[MAX_PATH]; // new on 180201
	output_string_stream class_name(tt_trouble_shoot_java_applet_file_name,MAX_PATH); 
	// new on 170201 to check if class file created
	class_name << java_directory;
   class_name << short_name << ".class";  
   class_name.put('\0');
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	// check itself moved to unpause_toontalk
//   delete [] command_line;
   delete [] compiler;
//   delete [] compile_and_show;
/*
   string viewer = ini_entry("Executables","AppletViewer");
   if (viewer == NULL) {
      // trying this out -- not clear how good this is
      ShellExecute(NULL,"open",file_name,"",java_directory,SW_SHOW);
   } else {
      char short_html_name[128];
      output_string_stream just_html_file_name(short_html_name,sizeof(short_html_name));
      just_html_file_name << short_name << ".htm";
      just_html_file_name.put((char) 0);
      ShellExecute(NULL,"open",viewer,short_html_name,java_directory,SW_SHOW);
      delete [] viewer;
   };
*/
//   if (user_java_directory != NULL) delete [] user_java_directory;
   delete [] java_extension;
   delete [] robot_classes_already_defined;
   delete [] robots_already_named;
   delete [] cycles_handled;
//   delete [] short_name;
	delete [] team_name;
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
   release_java_pictures(); // so we start afresh next time
	release_java_notebook_pages();
	release_java_sprites();
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	release_boxes_already_declared();
	if (cubby_restored != cubby && cubby != NULL) {
		thought_bubble->remove_follower(cubby_restored); // new on 050201 -- was add_follower prior to 120201
		cubby_restored->destroy();
	};
#endif
   user_did(GLOBAL_HISTORY,USER_HAS_GENERATED_JAVA_APPLET);
	return(short_name);
};

Cubby *Robot::restored_cubby(Cubby *cubby) {
	Cubby *cubby_restored = NULL;
   if (cubby != NULL) {
      //  package this up as Sprite::top_level_copy??
      tt_copy_restores_pre_blank = TRUE;
      tt_copying_entire_thought_bubble = TRUE;
      cubby_restored = (Cubby *) cubby->top_level_copy();
      tt_copying_entire_thought_bubble = FALSE;
      tt_copy_restores_pre_blank = FALSE;
   } else { // thought bubble is empty so use an empty 1-hole box
	   // new convention is empty thought bubble means ready to retrain -- what do to here??
      cubby_restored = (Cubby *) tt_toolbox->pointer_to_cubby_stack()->copy();
		cubby_restored->become_blank(); // new on 271100
    };
	 return(cubby_restored);
};

void Robot::java_team(java_stream &out, Cubby *cubby) {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log("About to create Java of robot debug target.");
	};
	if (tt_debug_mode == 20000) {
		character robot_name[max_resource_string_length];
		name(robot_name);
		if (strnicmp(robot_name,debug_robot_name,strlen(debug_robot_name)) == 0) {
			log(_T("About to create Java of robot who needs debugging."));
		};
	};
#endif
   Cubby *cubby_restored;
	boolean needs_restoration = FALSE;
	if (cubby == NULL && thought_bubble != NULL) {
       cubby = thought_bubble->pointer_to_cubby();
		 if (cubby != NULL) needs_restoration = TRUE; // since can from thought bubble
   };
   if (cubby == NULL || needs_restoration) {
      cubby_restored = restored_cubby(cubby); // made a method on 310101
		// moved here on 271100 so applies even if the cubby is NULL
		// tried commenting out the following since this is the box (like using the wand with 'o') and so shouldn't behave like it is in the thought bubble
		// but that broke lots of things (e.g. dumping of "hitting?" sensor in thought bubble. Besides the above deals with the 'o' behavior
		thought_bubble->add_follower(cubby_restored); // new on 220700 since some :java(...) checks this - why am I making a copy anyway?
   } else {
      cubby_restored = cubby;
   };
	java_string cubby_name = SC(IDS_JAVA_BOX,java_index(cubby_restored));
   if (cubby_restored != NULL && !cubby_restored->is_blank()) {
	   dump_started(); // dumping and Java share this
      cubby_restored->java_before(out);
//      java_assign(out,S(IDS_JAVA_BOX),cubby_restored);
      cubby_restored->java_after(out,cubby_name);
		dump_ended(); //release_sprites_being_dumped(); // just in case
      cubby_restored->java_describe_as_comment(out,S(IDS_JAVA_AND_THEN_CREATE));
      if (cubby != cubby_restored) {
        // might this mess things up if a robot without a cubby is on the back of a picture??
//         release_sprites_being_dumped(); // this covers both birds and nests and sensors and pictures
			// release_nest_indices(); // in case birds or nests were in thought bubble
			thought_bubble->remove_follower(cubby_restored); // new on 220700 
         cubby_restored->destroy();
      };
   }; // else??
   java_string robot_name = copy_string(java_name()); 
	// was SC(IDS_JAVA_ROBOT) prior to 061200 but robots on the back of things in a robot's box confused it
	if (body == NULL && thought_bubble->pointer_to_cubby() == NULL) { // new on 271100
       // just holding a fresh untrained robot so ignore this by using an instance of TTRobot
		java_declare(out,S(IDS_JAVA_ROBOT_CLASS),robot_name,"TT.UNTRAINED_ROBOT"); // javacom.rc?? -- prior to 061200 was java_assign
	} else {
		java_declare_new(out,S(IDS_JAVA_TT_ROBOT),robot_name,java_class_name(),S(IDS_JAVA_NOTEBOOK)); 
		// prior to 061200 was java_assign_new
	};
   java_after(out,robot_name);
//   out << "  TTRobot robot = new Robot" << serial_number() << "(notebook);" << endl;
   java_declare_new(out,S(IDS_JAVA_TT_TEAM),S2(IDS_JAVA_TEAM,java_index(this)),S(IDS_JAVA_TT_TEAM),cubby_name,robot_name); // last arg was S(IDS_JAVA_ROBOT) and java_assign_new prior to 061200
//	delete [] robot_name;
//   out << "  TTTeam team = new TTTeam(box, robot);" << endl;
//   if (user_provided_name) {
//      out << "  " << short_name << "Team team = new " << short_name << "Team();" << endl;
//    } else {
//      out << "  Team" << count << " team = new Team" << count << "();" << endl;
//   };
//   out << "  team.Gets(box);" << endl;
//   out << "  team.start();" << endl;
//   out << " }" << endl;
//   out << "}" << endl;
//   if (user_provided_name) {
//      out << "class " << short_name << "Team extends Thread {" << endl;
//   } else {
//      out << "class Team" << count << " extends Thread {" << endl;
//   };
//   out << " TTBox given;" << endl;
   int team_count = robots_in_team();
   Robot *next_robot = pointer_to_next_robot();
//   if (team_count > 1) {
//     java_declare(out,S(IDS_JAVA_TT_ROBOT),S(IDS_JAVA_NEXT_ROBOT));
//      out << "  TTRobot next_robot;" << endl;
//   };
	string previous_robot_name = robot_name;
	for (int i = 1; i < team_count; i++) {
		java_string next_robot_name = copy_string(next_robot->java_name()); 
		// was SC(IDS_JAVA_NEXT_ROBOT) and outside of the loop prior to 061200
		java_declare_new(out,S(IDS_JAVA_TT_ROBOT),next_robot_name,next_robot->java_class_name(),S(IDS_JAVA_NOTEBOOK));
		next_robot->java_after(out,next_robot_name);
		java_send(out,previous_robot_name,S(IDS_JAVA_SET_NEXT_ROBOT),next_robot_name); 
		// prior to 061200 previous_robot_name was S(IDS_JAVA_ROBOT)
//		java_assign(out,S(IDS_JAVA_ROBOT),next_robot_name);
		next_robot = next_robot->pointer_to_next_robot();
		delete [] previous_robot_name;
		previous_robot_name = next_robot_name;
	};
	delete [] previous_robot_name;
	delete [] cubby_name;
};

void Robot::java_class(java_stream &out) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 1411 && -tt_debug_target == serial_number()) {
		log("Debug robot java generation.");
	};
#endif
   if (!fully_defined()) return; // untrained robot
//   unsigned short int number = serial_number(); -- updated on 240702
   unsigned long number = body_hash(); // removed serial_number()^ on 271102 since serial is part of the body anyway 
	// isn't there a chance that 2 robots will have the same body but different names so that java_class_name needs to know the "right" name??
#if TT_DEBUG_ON
	if (tt_debug_mode == 1411 && tt_debug_target == number) {
		log("Debug robot java generation.");
	};
#endif
   for (int i = 1; i <= robot_classes_already_defined_count; i++) {
      if (robot_classes_already_defined[i] == number) {   // already done
         if (pointer_to_next_robot() != NULL) { // was just next_robot prior to 140201
            pointer_to_next_robot()->java_class(out);
         };
         return;
      };
   };
	comment_java_name(out); // moved here on 280900 so comment doesn't appear if already defined
   robot_classes_already_defined_count++;
   if (robot_classes_already_defined_count < max_robot_classes_per_file) {
      robot_classes_already_defined[robot_classes_already_defined_count] = number;
   } else {
      robot_classes_already_defined_count--; // warn??
   };
   out << S(IDS_JAVA_CLASS)<< " " << java_class_name()
       << " " << S(IDS_JAVA_EXTENDS_TT_ROBOT) << endl;
   out << " " << java_class_name() << S(IDS_JAVA_TT_NOTEBOOK_ARGLIST) << endl;
   java_assign(out,S(IDS_JAVA_NOTEBOOK),S(IDS_JAVA_N));
//   out << "  notebook = n;" << endl;
   out << " }" << endl;
   out << " " << S(IDS_JAVA_GETS_METHOD_HEADING) << endl;
   Cubby *cubby = NULL;
   if (thought_bubble != NULL) {
      cubby = thought_bubble->pointer_to_cubby();
   };
   if (cubby != NULL && !cubby->is_blank()) {
      out << "  " << S(IDS_JAVA_IF_NOT) << S(IDS_JAVA_WANTS) << "."
          << S(IDS_JAVA_MATCHES) << "(" << S(IDS_JAVA_GIVEN) << ")) " << S(IDS_JAVA_RETURN_NULL) << endl;
      out << "  " << S(IDS_JAVA_ROBOT_MATCH_COMMENT) << endl;
      out << "  " << S(IDS_JAVA_ROBOT_MATCH_COMMENT2) << endl;
   };
   if (body != NULL) {
      java_declare(out,S(IDS_JAVA_TT_OBJECT),S(IDS_JAVA_HAND),initial_tool);
//      out << "  TTObject hand";
//      if (initial_tool != NULL) {
//         out << " = ";
//         initial_tool->java(out);
//      };
//      out << ";" << endl;
		tt_main_notebook_referenced = FALSE; // since just starting (new on 141100)
#if TT_DEBUG_ON
		if (tt_debug_mode == 1411 && -tt_debug_target == serial_number()) {
			log("Debug robot java generation.");
		};
#endif
      body->java(out,this);
   } else {
//      out << "  " << S(IDS_JAVA_THROW_TT_EXCEPTION) << endl;   // why??
      java_indent(out);
      out << S(IDS_JAVA_RETURN_THIS) << endl;
   };
   out << " }" << endl;
   out << "}" << endl;
   if (pointer_to_next_robot() != NULL) { // was just next_robot prior to 140201
      pointer_to_next_robot()->java_class(out);
   };
};

Robot *Robot::previous_robot(Robot *me) {
   if (next_robot == me) return(this);
   if (next_robot == NULL) return(NULL);
   return (next_robot->previous_robot(me));
};

void Robot::comment_java_name(java_stream &out) {
	// new on 210700 since class name no longer includes user given name
	char robot_name[512];
   name(robot_name); // revised on 141100 to use name whether given by user or not for comment
//	string user_name = copy_string_name_from_user();
//	if (user_name != NULL) { 
		java_string comment = append_3_strings(java_class_name(),": ",robot_name);
//		delete [] user_name;
		java_comment(out,comment);
		delete [] comment;
//	};
};

void Robot::java_before(java_stream &out) {
//   unsigned short int number = serial_number();
   unsigned long number = body_hash(); // updated on 240702 // removed serial_number()^ on 271102 since serial is part of the body anyway 
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Java before of robot debug target.");
	} else if (tt_debug_mode == 1411 && -tt_debug_target == number) {
		log("Debug robot java generation.");
	};
#endif
	comment_java_name(out);
	// commented out as experiment on 080205
   //for (int i = 1; i <= robot_classes_already_defined_count; i++) {
   //   if (robot_classes_already_defined[i] == number) {   // already named
   //      if (robots_already_named != NULL) { // condition new on 180201
			//	if (robots_already_named[i]) return;
			//	robots_already_named[i] = TRUE;
			//};
   //      break;
   //   };
   //};
   java_string robot_name = copy_string(java_name());
   java_string robot_class_name = copy_string(java_class_name());
   java_declare_new(out,S(IDS_JAVA_TT_ROBOT),robot_name,robot_class_name,S(IDS_JAVA_NOTEBOOK)); // IDS_JAVA_NOTEBOOK was IDS_JAVA_NULL prior to 070205
   java_after(out,robot_name);
//   out << "  TTRobot robot" << serial_number() << " = new Robot" << serial_number() << "(null);" << endl;
   if (first_in_line != NULL) {
      Robot *previous = first_in_line->previous_robot(this);
      if (previous != NULL) {
         java_string previous_robot_name = copy_string(previous->java_name()); //SC(IDS_JAVA_ROBOT,previous->serial_number());
         java_send(out,previous_robot_name,
                   S(IDS_JAVA_SET_NEXT_ROBOT),robot_name);
         previous->java_after(out,previous_robot_name);
         delete [] previous_robot_name;
//         out << "  robot" << previous->serial_number() << ".SetNextRobot(robot" << serial_number() << ");" << endl;
      };
   };
   if (next_robot != NULL) next_robot->java_before(out);
   delete [] robot_name;
   delete [] robot_class_name;
};

void Robot::java(java_stream &out) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Java of robot debug target.");
	} else if (tt_debug_mode == 1411 && -tt_debug_target == serial_number()) {
		log("Debug robot java generation.");
	};
#endif
//   java_new(out,S2(IDS_JAVA_ROBOT_CLASS,serial_number()));
    out << java_name(); //S2(IDS_JAVA_ROBOT,serial_number());
//    out << "robot" << serial_number();
};

unsigned short int wants_counter = 1;

void Robot::java_after(java_stream &out, const_java_string robot_name) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 1411 && -tt_debug_target == serial_number()) {
		log("Debug robot java generation.");
	};
#endif
   Cubby *cubby = NULL;
   if (thought_bubble != NULL) {
      cubby = thought_bubble->pointer_to_cubby();
   };
   boolean wants = (cubby != NULL && !cubby->is_blank());
//  	int count = GetPrivateProfileInt("Switches","TeamNameCounter",0,C(IDC_TT_INI));
   if (wants) {
//      out << " " << S(IDS_JAVA_STATIC_FINAL); // and the following
		cubby->java_before(out); // moved here on 041200
      java_string wants_name = SC(IDS_JAVA_WANTS, wants_counter++);
      java_declare(out,S(IDS_JAVA_TT_BOX),wants_name,cubby);
      cubby->java_describe_as_comment(out,S(IDS_ROBOT_ACCEPTS));
      cubby->java_after(out,wants_name);
      java_send(out,robot_name,S(IDS_JAVA_SET_WANTS),wants_name);
//      release_nest_indices(); // in case birds or nests were in thought bubble
      delete [] wants_name;
   };
};

//boolean Robot::is_on_back_of(Picture *picture) {
//   return(floor != NULL && floor->pointer_to_picture() == picture);
//};

int Robot::place_in_team() {
   Robot *remaining = first_in_line;
   int answer = 0;
   while (remaining != NULL) {
      if (remaining == this) return(answer);
      remaining = remaining->pointer_to_next_robot();
      answer++;
   };
   return(answer); // should be 0 if I'm first
};

int Robot::robots_in_team() {
   if (first_in_line != NULL) {
      return(first_in_line->robots_in_team());
   };
   Robot *remaining = pointer_to_next_robot(); // was next_robot prior to 140201 but that missed robots while running (saved_next_robot)
   int answer = 1;
   while (remaining != NULL) {
      remaining = remaining->pointer_to_next_robot();
      answer++;
   };
   return(answer);
};

unsigned short Robot::serial_number() {
   unsigned short n = 0;
   if (body != NULL) {
      n = body->serial_number();
      if (n == 0) {
         Events *new_body = new Events(create_serial_number(),body);
         new_body->set_ref_count(body->current_ref_count());
#if TT_DEBUG_ON
			if (tt_debug_mode == 13578) {
				tt_error_file() << body << " copying ref count to " << new_body << " since serial number is missing." << endl;
			};
#endif
//			body->set_ref_count(1); // transfered the rest
			body = new_body;
         n = body->serial_number();
      };
   };
   return(n);
};

unsigned long Robot::body_hash() {
   if (body == NULL) {
      return(0);
   } else {
      return(body->events_hash());
   };
};

Robot *initial_selection;

void Robot::describe(output_stream &message, Article article) {
	if (infinite_stack() && thought_bubble == NULL) {
		// don't do this for stack of robots in notebook
		Sprite::describe(message,article);
		return;
	};
//	describe_comment(message);
	if (blank) {
		print_type(BLANK_ROBOT,message,article);
		return;
	};
	if (!infinite_stack() && initial_selection != this && 
		 initial_selection != NULL && !initial_selection->infinite_stack()) {
		// really pointing to a different robot in line
		initial_selection->describe(message,article);
		return;
	};
	if (abort_reason != 0) { // was under if (running_body_flag) prior to 151202
//		print_article(article,message,S(IDS_ROBOT_WHO_STOPPED));
//    message << S(IDS_ROBOT_WHO_STOPPED); // commented out on 030699
		message << S(IDS_ROBOT_ABORT) << S(abort_reason) << S(IDS_ROBOT_ABORT2);
	} else if (running_body_flag) {
		if (tt_city->stopped()) {
//         print_article(article,message,S(IDS_ROBOT_STOPPED_BY_F8));
         message << S(IDS_ROBOT_STOPPED_BY_F8);
		} else {
//			print_article(article,message,S(IDS_ROBOT_BUSY));
         message << S(IDS_ROBOT_BUSY);
		};
	} else if (suspended()) {
		if (leader != NULL && leader->kind_of() == PICTURE) {
//			print_article(article,message,S(IDS_ROBOT_WAITING_FOR_PICTURE));
         message << S(IDS_ROBOT_WAITING_FOR_PICTURE);
		} else {
//			print_article(article,message,S(IDS_ROBOT_WAITING_FOR_ROBOT));
         message << S(IDS_ROBOT_WAITING_FOR_ROBOT);
		};
		message << end_sentence(); 
		message << S(IDS_ROBOT_TRAINED_TO);
		message << " ";
		body->describe(message);
	} else if (current_parameter() == robot_waiting_state) {
//		print_article(article,message,S(IDS_ROBOT_WAITING_FOR_ITEM));
      message << S(IDS_ROBOT_WAITING_FOR_ITEM);
		if (suspended_on != NULL) {
			boolean ok = FALSE;
			Path *path = top_cubby->path_to(suspended_on,ok);
			if (ok && path != NULL) {
				path->describe(message,CUBBY);
			   delete path;
			};
		} else {
			message << " " << S(IDS_A_HOLE);
		};
		message << S(IDS_HIS_BOX) << S(IDS_ROBOT_WAITING_FOR_ITEM2,"") 
			     << end_sentence() << S(IDS_HE_WILL_THEN);
		body->describe(message);
		message << S(IDS_HE_WILL_THEN2,NULL);
	} else if (thought_bubble != NULL && 
				  thought_bubble->pointer_to_cubby() != NULL && body != NULL) {
		Sprite::describe(message,article);
/*
		print_article(article,message,S(IDS_ROBOT_WHO_WHEN_GIVEN));
		thought_bubble->pointer_to_cubby()->describe(message,INDEFINITE_ARTICLE);
		message << S(IDS_WILL_THEN);
		body->describe(message);
		int count = 0;
		if (first_in_line != NULL) {
			Robot *next = first_in_line;
			while (next != this) {
				count++; 
				next = next->pointer_to_next_robot();
			};
		};
		if (count > 1) {
			message << end_sentence() << S(IDS_AND_THERE_ARE) << " " << count << " " << S(IDS_ROBOTS_IN_FRONT);
		} else if (count == 1) {
			message << end_sentence() << S(IDS_AND_ONE_ROBOT_IN_FRONT);
		};
		count = 0;
		if (next_robot != NULL) {
			Robot *next = next_robot;
			while (next != NULL) {
				count++;
				next = next->pointer_to_next_robot();
			};
		};
		if (count > 1) {
			message << end_sentence() << S(IDS_AND_THERE_ARE) << " " << count << " " << S(IDS_ROBOTS_BEHIND);
		} else if (count == 1) {
			message << end_sentence() << S(IDS_AND_ONE_ROBOT_BEHIND);
		};
*/
	} else {
//		print_article(article,message,S(IDS_ROBOT_WITHOUT_A_THOUGHT));
      message << S(IDS_ROBOT_WITHOUT_A_THOUGHT);
      if (tool != NULL) {
         message << space() << S(IDS_WHO_IS_HOLDING) << space();
         tool->describe(message,INDEFINITE_ARTICLE);
      };
		message << S(IDS_WHO_IS_HOLDING2,"");
      if (training_counter != NO_TRAINING_COUNTER) {
         if (tool != NULL) message << space() << S(IDS_AND);
         message << space() << S(IDS_WHO_CAN_ONLY_REMEMBER) << space(SPACE_BEFORE_OR_AFTER_NUMBERS)
                 << (int) training_counter << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_ACTIONS);
      };
	};
};

void Robot::set_first_in_line(Robot *first) {
	// commented out 261000? since causes circularities and stuff is never reclaimed
//	if (first_in_line != NULL && first_in_line != this) { // second condition added 271000
//		first_in_line->destroy(); // decrement ref count (new on 171000)
//	};
//#if TT_DEBUG_ON
//	if (first != NULL && first->pointer_to_first_in_line() != first && first->pointer_to_first_in_line() != NULL) {
//		debug_this("not really first in line");
//	};
//#endif
	first_in_line = first;
//	if (first_in_line != NULL && first_in_line != this) { // conditional new on 241000
//		first_in_line->increment_ref_count(); // new on 171000
//	};
   if (next_robot != NULL) {
      next_robot->set_first_in_line(first); // since he probably thinks I'm first - new on 211002
	} else if (saved_next_robot != NULL) {
		saved_next_robot->set_first_in_line(first); // new on 180804
	};
};

void Robot::reset_first_in_line(Robot *first) {
	set_first_in_line(first);
	if (next_robot != NULL) {
		next_robot->reset_first_in_line(first);
	};
};

void Robot::grasp_tool(Sprite *new_tool,
							  Sprite *old_target, int tool_index_in_target,
                       millisecond after) { 
	if (kind_of() == ROBOT_IN_TRAINING) {
		if (old_target != NULL && old_target->kind_of() == PROGRAM_PAD) {
			record_event(GRASP_ITEM,this,floor,old_target,FALSE,0,DONT_ADD_TO_WORKING_SET,TRUE);
			record_event(NEW_ITEM,this,floor,new_tool,TRUE,0,ADD_TO_WORKING_SET_IF_NEW);
		} else {
			record_event(GRASP_ITEM,this,floor,new_tool);
			if (still_running()) {
				// above new on 161105 for retraining since needed to postpone this so paths could be computed
				new_tool = new_tool->selected(GRASPED,this);
			};	
		};
	};
	tool = new_tool;
//	tool->increment_ref_count(this); // new on 011199 - commented out on 270200 since pick_up_and_store_tool will do this
	tool->set_selectable(FALSE); // can't grab it from him
	target = old_target;
	target_index = tool_index_in_target;
//	if (after > 0) play_sound(ROBOT_SOUND,1);
//   new_tool->animate_to_saved_size(after,FALSE);
	do_after(after,this,REALLY_GRASP_TOOL_CALLBACK);
};

void Robot::apply_tool(Sprite *subject,
							  Sprite *old_target, int tool_index_in_target,
							  millisecond duration,
							  char key) {
//	if (nothing_grasped_flag) {
//		completed(new_action_status);
//		return;
//	};
#if TT_CAREFUL
	if (tool == NULL) {
		abort_action(IDS_ABORT_ROBOT_CANT_FIND_TOOL); 
		return;
	};
#endif
	if (tool->kind_of() == VACUUM && duration > hold_before_event_duration) {
		// since it takes time to suck up or spit out stuff don't have to
		// hold before acting
		duration -= hold_before_event_duration; // remove hold
	};
//   apply_tool_after = not_before;
	saved_subject = subject;
	target = old_target;
	target_index = tool_index_in_target;
	saved_key = key;
//	if (duration > 0) play_sound(ROBOT_SOUND,1);
	do_after(duration,this,REALLY_APPLY_TOOL_CALLBACK);
};

void Robot::select_from_stack(Sprite *stack, millisecond duration) {
	saved_stack = stack;
//   select_from_stack_after = not_before;
//	if (duration > 0) play_sound(ROBOT_SOUND,1);
	do_after(duration,this,REALLY_SELECT_FROM_STACK_CALLBACK);
};

Sprite *Robot::released(Sprite *by, boolean top_level, SpecialDrop special_drop) { 
	if (leader == NULL || leader->kind_of() == ROBOT) {
		// conditional added on 121099
		stop_team(ITEM_DROPPED);
		morph_to_inert_all();
	};
   if (top_level && thought_bubble != NULL) {
      thought_bubble->set_cause_of_match_failure(FALSE,TRUE); // new on 260302
   };
//	if (thought_bubble != NULL) {
//		thought_bubble->set_alive(FALSE);
//	};
	if (name_from_user != NULL) { // new on 110604
		name_from_user->released(by,FALSE,special_drop);
	};
	return(Sprite::released(by,top_level,special_drop));
};

void Robot::unselect() { // new on 110604
	if (name_from_user != NULL) {
		name_from_user->unselect();
	};
	Sprite::unselect();
};

void Robot::line_up(Background *floor, Sprite *by, millisecond duration) { // broken off from now_on_floor on 270704
	// restore line if grabbed a robot while running
	if (first_in_line != NULL) {	
		if (tt_log_version_number >= 49) { // condition added 180804 to preserve old demos
			first_in_line->line_up(floor,by); // new on 270704
		};
		return;
	};
	// moved following from :released to here on 090599 since it didn't look good when robots are released on back of pictures
	// added the following on 120499 so the robots regroup when dropped.
	Robot *front_robot = this;
	Robot *next_robot;
	city_coordinate delta_x, delta_y;
	delta_for_robot_behind_me(delta_x,delta_y); // new on 151199 so all robots end up in the right place
	int i = 1;
	while ((next_robot = front_robot->pointer_to_next_robot()) != NULL) {
//		city_coordinate behind_llx, behind_lly;
//		front_robot->location_behind_me(behind_llx,behind_lly);
		if (duration == 0 && tt_log_version_number >= 49) { // second condition added 180804 to preserve old demos
			finish_instantly(); // new on 270704 in case other things going on
		};
		next_robot->animate_location(llx+delta_x*i,lly+delta_y*i,duration);
//		next_robot->update_display();
		front_robot = next_robot;
		i++;
	};
};

void Robot::now_on_floor(Background *floor, Sprite *by) {
	if (body == NULL && tt_log_version_number >= 64) { // moved here on 161204 since should apply in Tooly
		set_parameter(ROBOT_BREATHING); // new on 141204 so untrained robots are visually distinguished
	};
	millisecond duration = default_duration();
	if (first_in_line != NULL) {
		// restore line if grabbed a robot while running
		first_in_line->add_to_end_of_line(duration); // duration added on 010503 
	} else { // I'm first
		add_to_end_of_line(duration); // duration added on 010503
		Sprite::now_on_floor(floor,by);
	};
	line_up(floor,by,duration);
	if (tt_log_version_number >= 22) {
		set_suspended_entire_team(FALSE); // new on 080400 since confusing if suspended after being dropped
		set_running_body_entire_team(FALSE); // new on 060504
	};
	reset_drop_scroll_deltas(); // new on 050404
};

void Robot::set_suspended_entire_team(boolean flag) {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log("Setting suspended of team of debug target (robot)");
	};
#endif
	Robot *current_robot = first_in_line;
	if (current_robot == NULL) current_robot = this;
	while (current_robot != NULL) {
		current_robot->set_suspended(flag);
		current_robot = current_robot->pointer_to_next_robot();
	};
};

void Robot::set_running_body_entire_team(boolean flag) { 
	// new on 060504 since can cause problems dropping boxes on large teams on the back of pictures
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log("set_running_body_entire_team of debug target (robot)");
	};
#endif
	Robot *current_robot = first_in_line;
	if (current_robot == NULL) current_robot = this;
	while (current_robot != NULL) {
		current_robot->set_running_body(flag);
		current_robot = current_robot->pointer_to_next_robot();
	};
};

#if TT_DEBUG_ON
Robot *activated_debug_target = NULL;
//int activate_count = 0;
#endif

void Robot::activate(boolean reset_animation, PictureActivateContext context) {
   // OK that this doesn't care about activatable() ?
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Activating debug target.");
		activated_debug_target = this;
	};
	if (tt_debug_mode == 70900 || tt_debug_mode == 51201) {
		character robot_name[max_resource_string_length];
		name(robot_name);
		if (tt_debug_mode == 51201 || strnicmp(robot_name,debug_robot_name,strlen(debug_robot_name)) == 0) {
			tt_error_file() << "Activating robot " << robot_name << "#" << debug_counter << " on frame " << tt_frame_number << endl;
//			activate_count++;
//			if (activate_count > 92) { 
//				log("debug this");
//			};
		};
	};
#endif
	if (context == ULTIMATE_PICTURE_CONTAINER_FLIPPED) return; // don't activate robots on the back of flipped picture 
//	boolean suspended_or_waiting = (waiting_for_release() || suspended());
   set_waiting_for_release(FALSE);
   set_suspended_entire_team(FALSE);
	// commented out on 070900
//	if (top_cubby != NULL && tt_debug_mode != 100400) {
//		top_cubby->recursively_activate_pictures(); // new on 270200
//	};
//   if (suspended_or_waiting || !body_started) {
	if (tt_log_version_number >= 73) {
		// new on 190405 -- simple and doesn't do expensive robot_already_in_queue test
		if (top_cubby != NULL && !team_running() && !any_teammate_suspended_on_something() && 
			 (tt_running_robots == NULL || tt_running_robots->member_cons(this) == NULL)) { // moved here on 200405
			// don't want the same robot running twice - can cause inconsistencies
			// also don't activate a robot that is waiting on a nest
#if TT_DEBUG_ON
			if (is_any_teammate_still_running()) { // new on 180405 so if robot is running don't activate it
				// not the same as team_running below -- but maybe could be rationalized
				// should this only apply if tt_log_version_number >= 73 ??
				debug_this();
			};
#endif
			try_clause(top_cubby,1);
		};
	} else if (top_cubby != NULL && // new on 120704 since if NULL is wasteful and causes the robot to move_to_side since stoppped
				  ((tt_log_version_number >= 22 && !team_running()) ||
					(tt_log_version_number < 22 && !body_started()))) { // was !body_started prior to 050400
					 if (tt_queue == NULL || !tt_queue->robot_already_in_queue(this)) { // conditional new on with 3.18 -- still needed? asked 190405
						 // don't want the same robot running twice - can cause inconsistencies
						 try_clause(top_cubby,1);
					 };
			// as of 091203 all robots wait a cycle when activated - this way they are run as tt_running_robots 
			// and then the body runs as tt_queue without any possible change of sensors inbetween
			//if (delay == 0) { // conditional new on 091100
			//	really_try_clause();
			//} else {
			//	try_clause(top_cubby,delay); // new on 091100
			//};
			// conditions added on 300300 since can be flipping over a remote control of looks and so the robot while running is activated
			// hopefully body_started is always accurate
	};
};

void Robot::suspend() {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Debug target suspended."));
	};
	if (tt_debug_mode == 70900) {
		character robot_name[max_resource_string_length];
		name(robot_name);
		if (strnicmp(robot_name,debug_robot_name,strlen(debug_robot_name)) == 0) {
			tt_error_file() << "Suspending robot " << robot_name << "#" << debug_counter << " on frame " << tt_frame_number << endl;
		};
	};
#endif
	set_suspended_entire_team(TRUE); 
//	if (top_cubby != NULL) {
//		top_cubby->decrement_busy_counter(); // unlock it
//	};
};

void Robot::stop_team(SelectionReason reason) {
	// commented out the following on 040500 since might not have started because say saved floor and yet need to update some of these things - at least top_cubby
//	if (!body_started && tt_log_version_number >= 22) return; // new on 070400 since lots of this is wasteful if already stopped
	// and might end up adding cubby to the "real" floor
//	animations = NULL;
//	if (top_cubby != NULL) {
//		top_cubby->decrement_busy_counter(); // unlock it
//	};
//	running_body_flag = FALSE; // new on 210300 since I rely upon this more now
#if TT_DEBUG_ON
	if (tt_debug_mode == 70900) {
		character robot_name[max_resource_string_length];
		name(robot_name);
		if (strnicmp(robot_name,debug_robot_name,strlen(debug_robot_name)) == 0) {
			tt_error_file() << "Stopping robot " << robot_name << "#" << debug_counter << " on frame " << tt_frame_number << endl;
		};
	};
   add_to_log_if_interesting_error();
#endif
	tt_suspensions->remove_if_robot_is(this);
	if (tt_queue != NULL) {
		tt_queue->remove_items_of_robot(this);
	};
	boolean suspended_on_back_of_picture = FALSE;
	if (top_cubby != NULL && next_robot == NULL) { // second conjunct added 271000 since all members of the team share the same top_cubby
		Background *cubby_background = top_cubby->pointer_to_background();
		if (cubby_background != NULL) {
			Picture *picture = cubby_background->pointer_to_picture();
			if (picture != NULL && !picture->slated_for_deletion()) {
				boolean originally_flipped = picture->is_flipped();
				if (!originally_flipped) picture->flip(TRUE,FALSE); // added FALSE on 170300 since shouldn't be changing size
				if (top_cubby->pointer_to_leader() == NULL) { // added on 250399 since cubby can end up part of two things at once
					// conditional moved here since flip might change leader status...
					add_flipped_follower(top_cubby,NULL,picture);
				};
				if (!originally_flipped) picture->unflip(TRUE,FALSE); // added FALSE on 170300 since shouldn't be changing size
				suspend();
				suspended_on_back_of_picture = TRUE;
			};
		} else if (floor != NULL && top_cubby->pointer_to_background() == NULL && 
					  top_cubby->pointer_to_leader() == NULL &&
					  !top_cubby->inside_vacuum() &&
					  top_cubby->is_main_cubby_in_body()) { // this conjunct added 080705 since may have been sent off in truck or bird
						  // this conjunct added on 250800 so if box is vacuumed up it doesn't reappear - 
						  // thanks to Micke for reporting this
			// top_cubby->pointer_to_leader() == NULL added on 310399
			floor->add_item(top_cubby,TRUE,FALSE,FALSE); 
			// added on 260399 so doesn't get "lost" -- args added on 231000 to avoid spurious warnings
		};
	};
	if (!suspended_on_back_of_picture) {
		set_working_cubby(NULL); // to stop try_clause
		no_longer_suspended();
	};
	finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
//	true_when_ready_to_try_clause = TRUE; // so it can be re-used
	forget_tool();
	done_running_body(FALSE,FALSE);
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	if (floor != NULL && floor->pointer_to_picture() == NULL) {
		set_priority(floor->next_item_priority());
		// new on 150103 that the following is under this conditional
		// since all this work is done when a robot stops because picture it is on the back of is stopped or being destroyed
// conditional meant that sitting down say when first robot vacuumed
// up main cubby that second robot would be left wiating and at wierd place
//	if (showing_on_a_screen()) { // commented out long ago
		if (reason != GRASPED && reason != ITEM_DROPPED &&
			 reason != COPIER_USED && reason != EXPANDER_USED && reason != VACUUM_USED &&
			 floor != NULL && !held()) { // added !held() on 260802
//			if (floor != NULL && floor->pointer_to_picture() == NULL) set_priority(floor->next_item_priority());
			move_to_side();
//			if (thought_bubble != NULL) {
//				thought_bubble->set_alive(FALSE);
//			};
		};
		if (reason == GRASPED || reason == ITEM_DROPPED || // ITEM_DROPPED added on 151199
			 reason == EXPANDER_USED || reason == VACUUM_USED ||
			 reason == COPIER_USED) {
//		   set_parameter(ROBOT_INERT);
		} else if (parameter == ROBOT_INERT || parameter == ROBOT_CONFUSED) {
			set_parameter(ROBOT_MORPH_TO_LIFE);
		} else if (parameter != ROBOT_INERT && parameter != ROBOT_MORPH_TO_INERT) {
			set_parameter(ROBOT_CONFUSED);
		};
	};
	set_suspended(FALSE);
	set_waiting_for_release(FALSE);
	if (tt_running_robots != NULL) { // added on 210499 so tt_running_robots doesn't have robots that are no longer running
//		boolean found;
#if TT_DEBUG_ON
		if (debug_counter == tt_debug_target) {
			tt_error_file() << "Removing robot debug target from running robots." << endl;
		};
#endif
		tt_running_robots = tt_running_robots->remove(this); //,&found);
//		if (found) destroy(); // release reference - found check added on 280100
		// commented out tt_running_robot ref count stuff on 090400 since causes robots to not be deleted and to run (as orphans)
		// and when robot is prepared for deletion it removes itself from tt_running_robot
	};
	if (next_robot != NULL) next_robot->stop_team(reason);
};

void Robot::move_to_side_of_box(millisecond duration) { // new on 130205
	if (top_cubby == NULL) return;
	city_coordinate cubby_llx, cubby_lly;
	top_cubby->lower_left_corner(cubby_llx,cubby_lly);
	animate_location(cubby_llx+top_cubby->current_width(),cubby_lly,duration);
};

void Robot::forget_tool() { // abstracted on 130205
	if (tool != NULL &&
       (tool != initial_tool ||
        (tool->kind_of() == COPIER &&
		   ((Copier *) tool)->attached_tool() != NULL))) {
		// drop what is being held unless it is one of the robot's tools
		if (tool->kind_of() != BOMB) { 
			// conditional added on 031202 since bombs set of by visible robots should animate - thanks Augusto
			tool->finish_instantly(); // new on 291002 so if say you grab a robot while Dusty is spitting then it finishes
		};
		running_body_flag = FALSE; // so release_tool can avoid spurious warnings
		release_tool(0,NULL,0);
	} else if (tool == NULL && initial_tool != NULL) {
      // go back to holding tool
      pick_up_and_store_tool(initial_tool);
   };
};

// Suspensions:: now adds and removes reference so the following isn't needed anymore
//boolean Robot::ok_to_delete() {
//#if TT_DEBUG_ON
//   if (tt_debug_mode == 1717) {
//      tt_error_file() << "Checking if ok to delete robot#" << debug_counter
//                  << " ref count is " << ref_count << " and suspended on " << suspended_on << endl;
//   };
//#endif
//   return(Sprite::ok_to_delete() && suspended_on == NULL);
//};

void Robot::no_longer_suspended() {
	if (suspended_on != NULL && suspended_on->kind_of() == NEST) {
#if TT_POST3187
		((Nest *) suspended_on)->no_longer_waiting(this);
		suspended_on->destroy(this); // new on 120707 to maintain ref counts
#else
		((Nest *) suspended_on)->no_longer_waiting();
#endif
		suspended_on = NULL;
	};
};

void Robot::shift_viewpoint(city_coordinate delta_x, city_coordinate delta_y) {
	// new on 040404 to help implement !tt_robot_drop_location_follows_camera 
	if (!tt_robot_drop_location_follows_camera) {
		drop_scroll_delta_x += delta_x;
		drop_scroll_delta_y += delta_y;
	};
	Sprite::shift_viewpoint(delta_x,delta_y);
};

void Robot::next_set_down_location(city_coordinate &subject_llx, city_coordinate &subject_lly) {
	if (tt_log_version_number >= 40) { // new on 040404
		const int rows = 4;
		const int columns = 4;
		const city_coordinate region_width = ideal_screen_width/(columns+2); // +2 to pad so outermost columns and rows aren't used
		const city_coordinate region_height = ideal_screen_height/(rows+2);
		const int region_count = rows*columns;
		city_coordinate x_offset = 0; 
		city_coordinate y_offset = 0;
		if (tt_log_version_number >= 76) {
			if (tool != NULL) {
				// new on 020605 since the old code centered the region when this is the lower left corner
				x_offset = tool->current_llx()-llx;
				y_offset = tool->current_lly()-lly;
			};
		};
		for (int i = 0; i < region_count; i++) {
			int region_number = set_down_counter%region_count;
			set_down_counter++;
			int x;
			if (tt_log_version_number == 76) { // this was a bad idea -- changed from >= on 140605
				x = region_number%columns; // changed on 050605 since don't need to skip edges now since region_width takes that into account
			} else {
				x = region_number%columns+1; // +1 to skip edges
			};
			int y = rows-(region_number/columns);
			// look for an open area --
			subject_llx = x*region_width+drop_scroll_delta_x;
			subject_lly = y*region_height+drop_scroll_delta_y;
			city_coordinate translated_region_width = 0; // checking translated dimensions in case standing in room
			city_coordinate translated_region_height = 0;
			if (tt_log_version_number >= 76 && tool != NULL) {
				// new on 050605 so small objects are placed even if region is occupied
				translated_region_width = tool->current_width();
				translated_region_height = tool->current_height();
			} else {
				translated_region_width = region_width;
				translated_region_height = region_height;
			};
			if (floor != NULL && floor->in_the_room_with_programmer()) { // updated 020205 
				tt_programmer->translate_floor_to_room_coordinates(subject_llx,subject_lly);
				tt_programmer->translate_floor_to_room_dimensions(translated_region_width,translated_region_height);
			};
			if (tt_log_version_number >= 76) {
				// new on 020605 since the old code centered the region when this is the lower left corner
				// new on 050605 since robot will move to location returned but offsets what it is holding
				TTRegion region(subject_llx+x_offset,subject_llx+x_offset+translated_region_width,
									 subject_lly+y_offset,subject_lly+y_offset+translated_region_height);
				if (tt_screen->region_is_free(&region,this)) { // free spot
					return;
				};
			} else {
				TTRegion region(subject_llx-translated_region_width/2,subject_llx+translated_region_width/2,
									 subject_lly-translated_region_height/2,subject_lly+translated_region_height/2);
				if (tt_screen->region_is_free(&region,this)) { // free spot
					return;
				};
			};
		};
		// all full so use this one but increment counter so at least when full covers the grid repeatedly
		set_down_counter++;
		if (tt_log_version_number >= 76) { // if full be more random
			subject_llx += (my_random(5)-2)*tile_width; //+ or - 2 tiles
			subject_lly += (my_random(5)-2)*tile_height; //+ or - 2 tiles
		};
	} else { // old behavior
		// uses the top region of the screen cycling thru location
		// doesn't use right side since toolbox is typically there
		if (tool != NULL && tool->kind_of() == PROGRAM_PAD) {
			subject_lly = 2*tile_height;
		} else {
			subject_lly = 11*tile_height;
		};
		const int location_count = 3;
		city_coordinate chest_x_offset, chest_y_offset;
		// need this because will be used to offset set down location
		chest_offsets(chest_x_offset,chest_y_offset);
		for (int i = 0; i < location_count; i++) {
			subject_llx = (set_down_counter%location_count)*tile_width*4+tile_width*2;
			set_down_counter++;
			if (tt_screen->first_to_contain(subject_llx+chest_x_offset+tile_width,subject_lly+chest_y_offset+tile_height) == NULL) {
				// free spot
				return;
			};
		};
		// all locations were full -- give up but move down a bit
		subject_lly -= 3*tile_height;
		set_down_counter++; // and move over for next time
		subject_llx = (set_down_counter%location_count)*tile_width*4+tile_width*2;
	};
};

void Robot::release_tool(millisecond duration,
								 Sprite *new_target, int new_target_index,
								 boolean new_released_on_its_stack) {
//	if (nothing_grasped_flag) {
//		completed(new_action_status);
//		nothing_grasped_flag = FALSE;
//		return;
//	};
	target = new_target;
	target_index = new_target_index;
	released_on_its_stack = (flag) new_released_on_its_stack;
//	if (duration > 0) play_sound(ROBOT_SOUND,1);
	if (duration == 0) {
		if (tool != NULL && showing_on_a_screen() && !(tool->kind_of() == COPIER && ((Copier *) tool)->attached_tool() != NULL)) { 
			// repair the geometry -- why??
			// don't know but on 041104 added the condition that this not be dropping what is attached to the wand
			tool->move_to(llx,lly);
		};
		really_release_tool();
	} else {
		do_after(duration,this,REALLY_RELEASE_TOOL_CALLBACK);
	};
};

void Robot::really_release_tool() {
	if (tt_shutting_down) return;
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Debug target really releasing tool."));
	};
#endif
	if (tool == NULL) {
		abort_action(IDS_ABORT_ROBOT_CANT_RELEASE);
		return;
	};
	Sprite *released = tool;
	if (target != NULL) {
		if (released_on_its_stack) {// equal(target,released)) {
			// got here because user returned an item to its stack -- so get rid of it
			if (!on_back_of_picture()) remove_follower(released); // condition added on 300399
			millisecond duration = default_duration(750);
			// use "key" as flag for whether this is just returning to a stack
			record_event(RELEASE_ITEM_ON,this,floor,target,TRUE,1);
			tool->now_released_on(target,this); // so it updates working set if need be
			if (duration > 0) {
				released->animate_size_and_location(target->current_width(),
																target->current_height(),
																target->current_llx(),
																target->current_lly(),
																duration,
																released,REMOVE_FROM_FLOOR_AND_DESTROY_CALLBACK);
				released->now_on_floor(floor,this); // but not for long
				released->set_slated_for_deletion(TRUE); // so can't be used
			} else {
//				if (tt_log_version_number > 14) {
					released->remove_from_floor(FALSE); 
					// added 1/23/99 since otherwise if notebook still referred to floor and might have active sensors
//				};
				released->destroy();
			};
         do_after(duration,this,ACTION_COMPLETED_CALLBACK); // rewritten on 131102
			target = NULL;
			tool->destroy(this); // decrement ref count - new on 280300
			tool = NULL;
			return;
		} else if (target->infinite_stack()) { // not currently operational
			target = target->copy_top_of_stack(this);
			add_to_working_set(target,FALSE);
		} else if (target->kind_of() == CUBBY && target_index >= 0 && !target->is_blank()) { 
			// moved this condition up from below on 220505 so can postpone things before making any changes
			Cubby *cubby_target = (Cubby *) target;
			Sprite *contents = cubby_target->component(target_index);
			if (contents != NULL && !contents->selectable()) { 
				do_after(500+my_random(1000),this,REALLY_RELEASE_TOOL_CALLBACK); // wait about a second and try again
				return;
			};
		};
	};
   released = tool->released(this); 
   boolean released_tool = (tool == released);
	millisecond duration = default_duration(); // for now
	if (released->pointer_to_leader() == this) {
      released->set_priority_fixed(FALSE);
		remove_follower(released);
	} else if (target != NULL && released->pointer_to_leader() != NULL) {
		// added on 120499 so for example when running on the back of a picture will be removed from backside first
		released->pointer_to_leader()->remove_follower(released);
	};
//	floor->add_item(released);
	if (released_tool) {
		if (duration > 0) set_parameter(ROBOT_DROPING_ITEM);
      tool->set_held_status(NOT_HELD);
      if (tool != initial_tool) tool->set_selectable(TRUE); // not the robot's anymore but if initial tool can't be "stolen"
		tool->destroy(this); // decrement ref count - new on 011199
		tool = NULL;
	};
	if (kind_of() == ROBOT_IN_TRAINING && target == NULL) { // && receiving_truck == NULL && receiving_bird == NULL) {
		record_event(RELEASE_ITEM,this,floor,released);
	};
	if (released == vacuum || released == copier || released == expander) {
		// was if(released->kind_of() == VACUUM || released->kind_of() == COPIER) {
		if (floor != NULL) floor->remove_item(released,FALSE);
//		vacuum_grasped = FALSE;
//		copier_grasped = FALSE;
//      expander_grasped = FALSE;
      released->set_selectable(FALSE);  // belong to the robot -- can't cheat in puzzles by getting robot's wand...
		if (tool != NULL) {
			tool->destroy(this); // decrement ref count - new on 011199
			tool = NULL;
		};
//		completed(action_status); // release above took care of this
		return;
	};
	if (released->slated_for_deletion()) { // added Sept 6 '98
		// this can happen if a robot sets off a bomb and then gets schedule out and the bomb goes off
		// then he releases the bomb
		return;
	};
	if (floor != NULL && 
		 floor->pointer_to_picture() == NULL) { // added conjunct on 260399 so if on flipside don't do this  || tt_log_version_number < 15
		// can it already be??
		// this is a bit wasteful since most recipients will remove it again
		// but otherwise item is lost if a recipient aborts
		floor->add_item(released,TRUE,FALSE,FALSE);
	};
   if (target != NULL && target->pointer_to_background() != floor &&
       target->current_held_status() == HELD_BY_PROGRAMMER &&
		 target->kind_of() != PROGRAM_PAD) {
      // been taken by programmer -- what about truck or bird?
      abort_action(IDS_ABORT_ROBOT_CANT_RELEASE); 
		return;
   };
	if (target == NULL) { // moved here on 151202 to simplfy the code
//		completed(action_status);
		if (pointer_to_working_cubby() == NULL && released->kind_of() == CUBBY) {
			// added on 280199 because picking up and dropping the main cubby broke
			// when running on the back of pictures
			set_working_cubby((Cubby *) released);
		};
		if (pointer_to_working_cubby() != released && 
		    !(tt_toolbox != NULL && toolbox_tool(released)) &&
			 running_body_flag) { // added on 070499 to avoid spurious warnings when grabbing a working robot
			if (add_to_working_set(released,TRUE) == -1) { // add to working set
				abort_action(IDS_ROBOT_LOST_TRACK);
				return;
			};
		};
		if (released == initial_tool) { // leave the size alone
		} else if (floor != NULL) { // had && duration > 0
			if (floor->pointer_to_picture() == NULL) { //  || tt_log_version_number < 15
				// condition added on 260399 so this does nothing if on back of picture
				released->now_on_floor(floor,this);
				if (released_tool && tool_width > 0 && tool_height > 0) {
					city_coordinate new_width = tool_width;
					city_coordinate new_height = tool_height;
					tool_width = 0;
					tool_height = 0;
					// commented out on 020999 since handled by :set_size... now
//					if (floor != NULL && floor->in_the_room()) {
//						tt_programmer->translate_floor_to_room_dimensions(new_width,new_height);
//					};
					released->animate_size(new_width,new_height,duration);
				} else {  // e.g. used wand so ignore wand's dimensions if releasing attached
					released->animate_to_good_size(duration,tt_log_version_number >= 64?tt_toolbox:NULL);
				};
//			} else { // on flipside
				// commented out on 210300 because this is breaking Dusty game and is expensive and isn't well motivated
//				if (!tt_stop_all) released->recursively_activate_pictures();
//#if TT_DEBUG_ON
//				log("Conditionalize demos 4");
//#endif
//			} else { // added on 161000 so that even though on the back of a picture the size is big enough that round off doesn't kill things
			} else if (floor->pointer_to_picture() == NULL) { // conditional new 160103 - why waste time otherwise
				released->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL);
			};
			if (released->pointer_to_background() == NULL) {
				floor->add_item(released); // new on 080105 since this happened to work before if dropped from a wand since add_follower
			};
			// set background and if released is a bird it won't accept anything if it doesn't know its background
		} else {
			released->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL);
		};
//		if (toolbox_tool(released)) {
			// was is_wand(released->kind_of())
//			released->animate_to_saved_size(duration);
//		};
//		return;
	} else if (target_index == - (int) RELEASE_ITEM_LEFT_OF) {
		add_completion(); // this and the release
//		if (action_status != NULL) *action_status = ACTION_IN_PROGRESS;
		target->add_to_side(FALSE,released,this,duration,target,released); 
	} else if (target_index == - (int) RELEASE_ITEM_RIGHT_OF) {
		add_completion(); // this and the release
		target->add_to_side(TRUE,released,this,duration,target,released); 
	} else if (target != NULL && target->kind_of() == CUBBY && target_index >= 0 && !target->is_blank()) { 
		// added target_index >= 0 on 060803 since it seems it can happen
		// added !target->is_blank() on 110205 if a robot is trained to erase a zero hole box, erase it, and drop something on top
//		if (target == tt_toolbox->pointer_to_cubby_stack()) {
//			target = target->selected(GRASPED,this);
//		};
		Cubby *cubby_target = (Cubby *) target;
		Sprite *contents = cubby_target->component(target_index);
		if (contents == NULL || contents->selectable()) { // condition new on 071004
			add_completion(); // this and the release
			cubby_target->hole_receives_item(released,target_index,this, // coerced to (cubby_index) prior to 070203
														duration,(kind_of() == ROBOT_IN_TRAINING));
		} else {
			// this branch is obsolete as of 220505 since check for this much earlier and deal with it
			//Sprite *top_of_stack = contents->pointer_to_top_contents(); 
			//// new on 220505 to rather than postpone to drop on the top of the stack (even if removed)
			//if (top_of_stack != NULL) {
			//	top_of_stack->receive_item(released,this,duration,target,released);
			//} else {
				// the following didn't work so well since hard to undo the above code once this problem is discovered
				// new on 071004 -- for example the top of the stack may be temporarily removed by a bird
				floor->remove_item(released,TRUE,TRUE,FALSE); // new on 210505 to try again
				initialize_tool(released); // new on 210505 since if trying again need to hold the tool again
				do_after(500+my_random(1000),this,REALLY_RELEASE_TOOL_CALLBACK); // wait about a second and try again
				return;
			// };
		};
//		if (kind_of() == ROBOT_IN_TRAINING) record_event(RELEASE_ITEM_ON,this,floor,target);
	} else if (target->kind_of() == BIRD) { // prior to 151202 was receiving_bird != NULL
		// could this be a stack?? (e.g. in a notebook)
		add_completion(); // this and the release
//		if (released == top_cubby) {
//			top_cubby->decrement_busy_counter();
//		};
		if (kind_of() == ROBOT_IN_TRAINING) record_event(RELEASE_ITEM_ON,this,floor,target);
//		if (version_number() >= first_robot_version_to_remove_from_working_set_when_giving_to_bird) { 
		// new on 040201 since the following interfered with portals in the adventure game
			remove_tool_from_working_set(released); 
			// new on 220101 - important if ends up in a vacuum that is destroyed that robot releases ref count now
//		};
		// what if the following fails?
		target->receive_item(released,this,duration,target,released);
	} else if (target->kind_of() == TRUCK) { // prior to 151202 was receiving_truck != NULL
		if (target->infinite_stack()) {
			target = target->copy_top_of_stack(this);
			add_to_working_set(target,FALSE);
//			Truck *copy = (Truck *) receiving_truck->copy(floor);
//			receiving_truck = copy;
		};
//		boolean truck_was_empty = receiving_truck->empty();
		add_completion(); // this and the release
//		if (released == top_cubby) {
//			top_cubby->decrement_busy_counter();
//		};
		// can the following fail?
		if (target->receive_item(released,this,duration,target,released)) { // conditionalized 010799
			if (kind_of() == ROBOT_IN_TRAINING) record_event(DROP_ITEM_IN_TRUCK,this,floor,target);
		}; // else??
//		if (!truck_was_empty)
	} else { // nothing special about drop target
	   city_coordinate x,y;
	   target->center_location(x,y);
	   // center it over the target
	   released->animate_location(x-released->current_width()/2,
										   y-released->current_height()/2,
										   duration/2); // quickly -- but not always quickly enough for mouse to know where to smash
		add_completion(); // this and the release
		if (target->kind_of() == ROBOT && released->kind_of() == ROBOT) { // new on 150303 - joining robots in a team (by a robot)
			add_completion(); // and smash
		};
		if (target->kind_of() == PROGRAM_PAD) {
			if (kind_of() == ROBOT_IN_TRAINING) {
				set_available_for_indexing(FALSE); // so a path to it is used
				record_event(RELEASE_ITEM_ON,this,floor,target);
				set_available_for_indexing(TRUE); 
			};
			Notebook *notebook = (Notebook *) target;
         if (!notebook->showing_on_a_screen()) duration = 0; // e.g. stood up and notebook is now inside of Tooly
			if (target->receive_item(released,this,duration,target,released)) {
				if (page_number_offsets == NULL) {
					page_number_offsets = new Page_Number_Offsets();
				};
				page_number_offsets->update(notebook);
				// commented out the following on 301000 since notebook::set_recent_page_number now deals with this
//				if (tt_toolbox != NULL && notebook == pointer_to_toolbox_notebook()) { // shared	
//               set_recent_page_number(notebook->return_recent_page_number());
//#if TT_DEBUG_ON
//					if (tt_debug_mode == 291000 || tt_debug_target == debug_counter) {
//						notebook->print(tt_error_file());
//						tt_error_file() << " shared. (really_release_tool)" << endl;
//					};
//#endif
//               notebook_not_yet_used = FALSE;
//            };
			} else if (home_notebook != NULL && tt_toolbox != NULL && 
						  notebook == tt_toolbox_notebook &&
						  home_notebook->page_number_of_item(released) != -1 &&
						  kind_of() != ROBOT_IN_TRAINING) { // last condition added on 160801 so retraining doesn't try to find home notebook
				// main notebook would not take item but home notebook can
				use_home_notebook = TRUE;
				if (duration > 0 && home_notebook->default_duration() == 0) { // added Sept 6 '98
					// robot is visible but home notebook was left somewhere else
					// so generate a copy now
					Notebook *home_notebook_copy = (Notebook *) home_notebook->copy();
					home_notebook_copy->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL);
					// this is a bit mysterious so set it off screen so you don't
					// see notebook appear
					home_notebook_copy->move_to(llx,lly-ideal_screen_height);
					floor->add_item(home_notebook_copy);
					tt_screen->add(home_notebook_copy);
					set_home_notebook(home_notebook_copy);
				};
				home_notebook->receive_item(released,this,duration,home_notebook,released);
				if (page_number_offsets == NULL) {
					page_number_offsets = new Page_Number_Offsets();
				};
				page_number_offsets->update(home_notebook);
			} else {
				abort_action(IDS_COULDNT_FIND_PAGE,ROBOT);
			};
		} else {
			// perhaps these two should be re-ordered
			// can the following fail?
			if (target->receive_item(released,this,duration,target,released)) { // conditionalized on 010799
				if (kind_of() == ROBOT_IN_TRAINING) {
					record_event(RELEASE_ITEM_ON,this,floor,target);
					released->now_released_on(target,this); // new on 160405 since if retraining then this is important
					// otherwise can be vacuumed up at the end
				};
			} else {
				if (kind_of() == ROBOT_IN_TRAINING) {
					record_event(RELEASE_ITEM,this,floor); // new on 010799
				};
			};
		};
	};
	target = NULL; // moved here on 161202 since all branches above do this (except when target was already NULL)
	if (is_action_aborted() && // this was < 0 but now aborted is min_long
		 // action_status != NULL && is_action_aborted(action_status) &&
		 released != NULL && !released->prepared_for_deletion()) { 
			 // added check if already prepared for deletion on 200202 - typically because some action aborted
      if (released_tool) {  // can this happen here?
         city_coordinate new_width = tool_width;
         city_coordinate new_height = tool_height;
         if (floor != NULL && floor->in_the_room_with_programmer()) { // updated 020205
            tt_programmer->translate_floor_to_room_dimensions(new_width,new_height);
         };
         released->set_size(new_width,new_height);
      } else {
         released->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL);
      };
	};
};

void Robot::really_grasp_tool() {
	if (tool == NULL) { // new on 111102 since a crash dump from Augusto had tool being NULL (or bad?)
		abort_action(IDS_ROBOT_LOST_TRACK); // is this best message?
		return;
	};
//	if (tool != top_cubby && tool != pointer_to_toolbox_notebook() &&
//		 add_to_working_set(tool) == -1) {
//		abort_action(action_status,S(IDS_ROBOT_LOST_TRACK));
//		return;
//	};
	if (tt_shutting_down) return;
#if TT_DEBUG_ON
	if (tt_debug_mode == 120105) {
		tt_error_file() << timeGetTime() << " start of really_grasp_tool" << endl;
	};
#endif
	if (target != NULL && target->kind_of() == CUBBY &&
		 tool->pointer_to_leader() == target) { 
		// is directly contained in cubby (not via a nest)
		remove_from_cubby();
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 120105) {
		tt_error_file() << timeGetTime() << " aftr remove_from_cubby" << endl;
	};
#endif
	Sprite *leader = tool->pointer_to_leader();
	// if leader is nest find topmost one
//	Sprite *top_nest = leader;
//	while (top_nest != NULL && top_nest->kind_of() == NEST) {
//		leader = top_nest;
//		top_nest = top_nest->pointer_to_leader();
//	};
//	millisecond duration = default_duration();
	if (leader != NULL && !leader->is_flipped()) { //  || tt_log_version_number < 15)
		// on 260399 added && !leader->is_flipped() since shouldn't be removed from back of picture
		if (tool->kind_of() == BIRD) {
			((Bird *) tool)->leave_leader(); // was leave_nest() ???
		} else if (leader->kind_of() == NEST) {
			((Nest *) leader)->remove_contents(tool,floor); // 2nd arg was TRUE prior to 090100
		} else if (!ultimate_leader()->held()) { // (leader->kind_of() == TRUCK) {
			leader->remove_selected_element(tool,GRASPED,this,TRUE);
//			if (tool == top_cubby) { 
				// e.g. was dropped in a truck and then taken out
//				top_cubby->increment_busy_counter();
//			};
		} else { // already held so get a copy
			Sprite *saved_tool = tool;
			tool = tool->copy();
			saved_tool->destroy(this); // decrement ref count - new on 011199 - moved here on 280300
			tool->increment_ref_count(this); // new on 011199
			if (floor != NULL) {
				floor->add_item(tool);
			};
		};
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 120105) {
		tt_error_file() << timeGetTime() << " after remove_selected_element" << endl;
	};
#endif
	if (tool->kind_of() == ROBOT) {
      Robot *robot = (Robot *) tool;
      if (home_notebook != NULL) {
	      // picking up other robots so they are part of this "thread"
		   robot->set_home_notebook(home_notebook);
      };
      if (team_notebook != NULL) {
		   robot->set_team_notebook(team_notebook);
      };
	};
//	tool->animate_to_saved_size(duration,FALSE);
//	if (leader == NULL) { 
		pick_up_and_store_tool(tool);
	#if TT_DEBUG_ON
	if (tt_debug_mode == 120105) {
		tt_error_file() << timeGetTime() << " after pick_up_and_store_tool" << endl;
	};
#endif
//	};
//	if (!is_wand(tool->kind_of())) {
//		add_to_working_set(tool);
//	};
};

void Robot::pick_up_and_store_tool(Sprite *item) {
   if (training_counter == 0 && kind_of() == ROBOT_IN_TRAINING && item != initial_tool) return;  // done
	// e.g. might be a bird hatching from a nest
	item->finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
//	item->save_size();
	tool = item;
	tool->increment_ref_count(this); // new on 011199 - good idea in general and needed sometimes when tool is on nest  && tt_log_version_number > 14
	if (on_back_of_picture()) { // added on 260399 since rest of the work is crazy to do on flipside
//		item->set_to_good_size(); // new on 220200 so that if say used to replace appearance of top picture the size is reasonable
		action_completed();
		return;
	};
   // the following is to restore the original size but sometimes this leads to bad animation
   // e.g. if the tool was just taken from a notebook then its size isn't so good yet
   if (tool != initial_tool) {
 		tool_width = tool->current_width();
      tool_height = tool->current_height();
	};
	// commented out following on 020999 since handled by :set_size... now
//   if (floor != NULL && floor->in_the_room()) {
//      tt_programmer->translate_room_to_floor_dimensions(tool_width,tool_height);
//   };
//	if (default_duration() != 0) {
		if (kind_of() != ROBOT_IN_TRAINING) {
			item->set_selectable(FALSE); // can't grab it from him
		};
		millisecond grab_time = default_duration(total_cycle_duration()/3);
      if (training_counter != 0 && grab_time > 0) set_parameter(ROBOT_PICKING_UP_AND_RESTORING);
		// tool on top of robot -- lots of room for layered parts
		// e.g. team of robots
//		if (tool != initial_tool)    // why??
      tool->set_priority(current_priority-1);
		city_coordinate x_offset, y_offset;
      if (kind_of() == ROBOT_IN_TRAINING) {
         x_offset = 0;
         y_offset = (3*height)/5;
      };
//		if ((tt_toolbox != NULL && toolbox_tool(tool)) ||
//          tool->is_tool()) { // e.g. wand
      if (tool->kind_of() == COPIER || tool->kind_of() == EXPANDER || tool->kind_of() == VACUUM) {
         resize_to_fit_chest(tool);
			if (kind_of() == ROBOT_IN_TRAINING && tool->kind_of() != COPIER) { 
				// added copier test on 240102 since selection works differently for the wand
				// new on 220102 to fix a problem with selection region when holding a tool called for twice (e.g. F2, F2)
				tt_programmer->tool_in_hand_is_ready(tool);
			};
         add_to_chest();
//			city_coordinate chest_width, chest_height;
//			chest_offsets(x_offset,y_offset);
//			chest_size(chest_width,chest_height);
//         if (chest_width < tile_width) chest_width = tile_width;
//			tool->animate_width_maintain_proportionality(chest_width,grab_time);
//         if (kind_of() == ROBOT_IN_TRAINING) {
//            x_offset -= chest_width/2;
//            y_offset -= (chest_width*tool->current_height())/(tool->current_width()*2); // proportional
//            y_offset += height/5; // not sure why this is needed but it looks better
//         }
		} else {
			if (kind_of() == ROBOT_IN_TRAINING) {
            x_offset -= tool->current_width()/2;
            y_offset -= tool->current_height()/2;  // to reach robot's hand
         } else {
            tool->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL);
            x_offset = 0;
			   y_offset = height/3;
         };
			millisecond duration;
			if (tool->showing_on_a_screen()) { //  || tt_log_version_number < 15
				duration = grab_time;
			} else { // this happens for example when standing in a room and tool is the main notebook
				duration = 0;
			};
		   // can move after grabbing and it should still end up right
		   tool->animate_location_to(this, // &llx,&lly,
									        duration,
									        this,ADD_TO_CHEST_CALLBACK,
                                   x_offset,y_offset);
      };
/*
	} else {
		if ((kind_of() != ROBOT_IN_TRAINING) && // || tt_running_old_demos) &&
          !(tt_toolbox != NULL && toolbox_tool(tool))) {
			if (tool->kind_of() == COPIER || tool->kind_of() == EXPANDER || tool->kind_of() == VACUUM) {
			   city_coordinate chest_width, chest_height;
//			   chest_offsets(x_offset,y_offset);
			   chest_size(chest_width,chest_height);
            if (chest_width < tile_width) chest_width = tile_width;
			   tool->set_width_maintain_proportionality(chest_width);
         } else {
            tool->set_to_good_size();
         };
		};
		add_to_chest();
	};
*/
};

void Robot::regrasp_tool() { // new on 260904
	if (tool != NULL) {
		tool->move_to(llx,lly+height/3);
	};
};

void Robot::set_first_to_fail_for_unstable_reasons(Robot *robot) {
	// commented out 271000 since causes circularities and stuff is never reclaimed
//	if (first_to_fail_for_unstable_reasons != NULL && first_to_fail_for_unstable_reasons != this) { // second conjunct added 271000
//		first_to_fail_for_unstable_reasons->destroy(this); // drop reference
//	};
	first_to_fail_for_unstable_reasons = robot;
//	if (first_to_fail_for_unstable_reasons != NULL && first_to_fail_for_unstable_reasons != this) { // second conjunct added 271000
//		first_to_fail_for_unstable_reasons->increment_ref_count(this);
//	};
};
	
void Robot::set_background(Background *new_floor, boolean recur, boolean warn) {
#if TT_DEBUG_ON
//	if (new_floor == NULL && leader == NULL && warn) {
//		Picture *picture = NULL;
//		if (floor != NULL) {
//			picture = floor->pointer_to_picture();
//		};
//		if (picture == NULL || !picture->prepared_for_deletion()) {
//			say_error("Robot is about to be orphaned.");
//		};
//	};
	if (warn && new_floor != NULL && !new_floor->has_item(this)) { // this is costly and maybe should be removed normally
		log("Robot getting background but background doesn't know about it.");
		debug_this();
	};
#endif
	boolean currently_on_back_of_picture = on_back_of_picture(); 
	// new on 300301 to compute this before setting background so if on picture don't forget the fact below
   Sprite::set_background(new_floor,recur,warn);
//   if (new_floor == NULL) return;
   // following is needed for example if a robot is waiting for a tool
   // to finish some animation and programmer has stood up -- changing the background
   if (tool != NULL && !currently_on_back_of_picture) { 
		// added !on_back_of_picture on 240299 since not only is it unnecessary extra work it can lead to an infinite loop
		Sprite *container = tool->ultimate_leader(); // prior to 6/12/98 this was pointer_to_leader()
		if (container != this &&
			 container->kind_of() != ROBOT) { 
			// added check that it isn't part of a team on 290499 since otherwise tools in hand might 
			// not be displayed anymore due to this
			// not already part of the robot
			if (new_floor != NULL && tool == container) {
				tool->remove_from_floor(FALSE); // just in case (moved into conditional on 010999)
				new_floor->add_item(tool,TRUE,FALSE,FALSE); // flags added on 300899 to avoid spurious warnings
			};
		};
   };
//   if (initial_tool != NULL && tool != initial_tool) {
//      initial_tool->set_background(new_floor);
//   };
	if (top_cubby != NULL && recur && top_cubby != tool && // && tt_log_version_number > 14 
		 on_back_of_picture() && top_cubby->on_back_of_picture()) {
		// added top_cubby != tool on 010999
		// added on_back_of_picture() on 020999 (second one added 100200)
		// maybe this whole thing is a bad idea but it seems to be needed when 
		// on the back of pictures...
		top_cubby->set_background(new_floor,recur,warn);
	};
	if (new_floor != NULL && new_floor->showing_on_a_screen()) { // new on 021199
		fit_name_on_chest();
	};
	if (saved_next_robot != NULL && saved_next_robot->pointer_to_background() == NULL && new_floor != NULL) { // new on 170603
		if (new_floor->has_item(saved_next_robot)) { 
			// new on 131004 seems to be needed when large teams (only when running?)
			saved_next_robot->set_background(new_floor);
		} else {
			// older clippings sometimes have just the saved_next_robot link and not enough to put it together
			// added new_floor != NULL on 030903
			city_coordinate behind_robot_llx, behind_robot_lly;
			location_behind_me(behind_robot_llx,behind_robot_lly);
			saved_next_robot->move_to(behind_robot_llx,behind_robot_lly);
	//		add_to_end_of_line();
//			robot_to_put_behind = saved_next_robot;
			set_robot_to_put_behind(saved_next_robot); // rewrote the above on 230105
			splice_in_line();
			//if (recur) {
			//	saved_next_robot->set_background(new_floor,recur,warn);
			//};
		};
	};
};

void Robot::resize_to_fit_chest(Sprite *sprite) {
	city_coordinate chest_delta_x, chest_delta_y, chest_width, chest_height;
	chest_offsets(chest_delta_x,chest_delta_y);
	chest_size(chest_width,chest_height);
	sprite->set_width_maintain_proportionality(chest_width);
	sprite->move_to(llx+chest_delta_x,lly+chest_delta_y);
};

void Robot::add_to_chest() {
	if (tool != NULL) {
		if (on_back_of_picture()) { // added on 170299 so that box doesn't disappear from the back of a picture
			tool->remove_from_floor(FALSE,FALSE); // added on 160399 to keep consistent
			add_follower(tool,FALSE,INSERT_BY_PRIORITY); // added on 160399 to keep consistent
			tool->set_held_status(HELD_BY_ROBOT); // added on 120600 since broke robot's use of vacuum on back of picture
			completed();
			return;
		};
		if (tool->current_held_status() != HELD_BY_PROGRAMMER || kind_of() == ROBOT_IN_TRAINING) { 
			if (tool->pointer_to_leader() != NULL) { // new on 120203
				if (tool->pointer_to_leader() == this) { // new on 040603 -- somehow already added -- perhaps saved with initial tool
					completed();
					return;
				};
				do_after(1,this,REALLY_GRASP_TOOL_CALLBACK); // try again very soon
				// was ADD_TO_CHEST_CALLBACK prior to 111204 but if a bird has in the meanwhile put it on a nest then this will never "wake up" without
				// running the ::really_grasp_tool code (pretty sure the other stuff done there is OK to to do more than once)
				return;
			};
			// robot in training is the programmer
			// was !tool->held() prior to 160499
			// hasn't been stolen by programmer
//			if (tt_log_version_number < 15) {
				tool->remove_from_floor(FALSE,FALSE);
//			};
			if (showing_on_a_screen()) {
				tt_screen->add(tool);
				if (tool->kind_of() == PROGRAM_PAD && tt_toolbox != NULL && 
					tool->pointer_to_background() == NULL && floor != NULL && floor->in_the_room()) {
//					tt_log_version_number > 14) {
					// since notebook lives in Tooly but a robot is using him while
					// standing need to fake things. And fix the geometry for a room view
					city_coordinate tool_width = tool->current_width();
					city_coordinate tool_height = tool->current_height();
               tt_programmer->translate_floor_to_room_dimensions(tool_width,tool_height);
               tool->set_size(tool_width,tool_height);
				};
			};
//			if (tt_log_version_number > 14) {				 
//				tool->remove_from_floor(FALSE,FALSE); // commented out on 021202 since just done above
//			};
			tool->set_priority_fixed(TRUE); // taken care of by robot until released
			tool->set_priority(priority()-1);
			add_follower(tool,FALSE,INSERT_BY_PRIORITY);
			tool->set_held_status(HELD_BY_ROBOT);
			completed();
		} else {
			abort_action(IDS_ABORT_ROBOT_CANT_FIND_TOOL); 
		};
	};
//	floor->remove_item(tool); 
}; 

void Robot::begin_drop_action() {
   if (training_counter == 0 && kind_of() == ROBOT_IN_TRAINING) return;  // done
	if (tool == NULL) return; // new on 051099 since can happen when restarting a city saved while training a robot
   propagate_changes(); // new on 250402 since if in absolute mode robot may have moved and tool hasn't been updated yet
	millisecond duration = default_duration();
	if (duration > 0 && training_counter != 0) set_parameter(ROBOT_DROPING_ITEM); // ok since asked just be doing this
	Sprite *to_release = tool->if_released();
	if (to_release != tool && kind_of() != ROBOT_IN_TRAINING) { 
		// releasing attached item
      to_release->set_priority_fixed(FALSE);
		remove_follower(to_release);
	};
   if (kind_of() == ROBOT_IN_TRAINING) {  // so he goes back to "pointing"
      do_after(total_cycle_duration(),this,APPEAR_WORKING_CALLBACK);
   };
};
/*
	millisecond drop_time = total_cycle_duration()/2;
	city_coordinate saved_tool_width, saved_tool_height;
	to_release->saved_size(saved_tool_width,saved_tool_height);
	to_release->animate_size_and_location(saved_tool_width,saved_tool_height,
													  llx+width-saved_tool_width/2,
													  lly+height/2-saved_tool_height/2,
													  drop_time);//,NULL,
//											  this,&really_drop_tool_callback);
	return(drop_time);
*/

void Robot::drop_tool() {
	// only called by programmer when training a robot
	if (tool == NULL) return;
	// moved the following up so that happens even if training is over or on back of picture
	if (tool->pointer_to_leader() == this) { // have finished picking it up
		remove_follower(tool);
	};
   if (training_counter == 0) { 
		tool->destroy(this); // decrement ref count new on 011199
		tool = NULL; // new on 011199
		return;  // done
	};
	if (on_back_of_picture()) { // added on 170299 so that box doesn't disappear from the back of a picture
		tool->destroy(this); // decrement ref count new on 011199
		tool = NULL;
		return;
	};
//	tool->update_display(); //why??
//	if (kind_of() == ROBOT) { // if acting for programmer already taken care of
//		floor->add_item(tool);
//	};
	tool->set_priority_fixed(FALSE);
	if (floor != NULL) {
      floor->give_item_priority(tool);
//		tool->set_priority(floor->next_item_priority());
	};
   tool->set_selectable(TRUE);
	tool->destroy(this); // decrement ref count new on 011199
	tool = NULL;
};

void Robot::swap_tool(Sprite *new_tool) {
   tool->set_priority_fixed(FALSE);
	remove_follower(tool,FALSE); // FALSE added on 010301 since warning didn't seem right
	tool->destroy(this); // decrement ref count new on 011199
	tool = new_tool;
	new_tool->increment_ref_count(this);
	floor->remove_item(tool,FALSE,FALSE,FALSE);
	if (showing_on_a_screen()) {
		tt_screen->add(tool);
	};
   tool->set_priority_fixed(TRUE);
	add_follower(tool,FALSE);
};

//void really_drop_tool_callback(void *p) {
//	((Robot *) p)->really_drop_tool();
//};
		
void Robot::really_apply_tool() {
//	if (tool->kind_of() == VACUUM) {
//		if (saved_subject == NULL) { // NULL if vacuum restore
//		} else {
//			Sprite *subject_leader = saved_subject->pointer_to_leader();
//			if (subject_leader != NULL &&
//				 subject_leader->kind_of() == NEST &&
//				 ((Nest *) subject_leader)->pointer_to_contents() != NULL) {
//				((Nest *) subject_leader)
//					 ->remove_contents(default_duration(),FALSE);
//			} else if (target != NULL && target->kind_of() == CUBBY) {
//				remove_from_cubby();
//			};
//		};
//	};
//	if (saved_subject != NULL && saved_subject->kind_of() == NEST) {
//		Tool *nest_contents = ((Nest *) saved_subject)->pointer_to_contents();
//		if (nest_contents != NULL) {
//			saved_subject = nest_contents;
//		};
//	};
   if (tool == NULL) return; // what about action_status?
   if (tool->kind_of() == PICTURE) { // about to flip a picture so don't keep old dimensions
		tool_width = 0;
		tool_height = 0;
	};
	millisecond duration = decode_duration(saved_key);
	boolean retraining = (kind_of() == ROBOT_IN_TRAINING);
	Sprite *new_tool = tool->used(saved_subject,this,FALSE,retraining,duration,(saved_key != 0),NULL);
	if (tool != NULL && tool->kind_of() == EXPANDER) { // only Pumpy works this way to work for some recorded duration
		if (!retraining || body_status < 2) { // condition new on 130205 since otherwise when retraining a robot it never goes to next step
			// branch new on 280704 to keep pumping for duration
			if (!retraining) { // condition new on 130205
				add_completion(); // new on 280704 since following completes now
			};
			do_after(default_duration(duration),this,FORGET_SAVED_SUBJECT_CALLBACK); // default_duration new on 020205
		};
	} else {
		saved_subject = NULL; // don't need it any more and shouldn't save it if saving city (new on 231202) 
	};
//   if (tool->kind_of() == COPIER) {
//      Sprite *attached = ((Copier *) tool)->pointer_to_attached();
//      if (attached != NULL) { // can it be otherwise?
//         attached->good_size(tool_width,tool_height);
//     };
//   };
	if (tool == NULL) { // attempt to use tool aborted - new on 181200
		return;
	};
	if (new_tool != NULL && tool != new_tool) {
      // hasn't been dropped but has changed
		swap_tool(new_tool);
		tool_width = 0;
		tool_height = 0;
	}; 
   if (tool->is_on() && top_cubby != NULL && still_running() && !finishing_instantly()) { 
		// still_running() added on 080599 to stop looping if Esc pressed while retraining
		// finishing_instantly added on 160801 since caused stack overflow run_animations or memory exhaustion (e.g. robot using Pumpy is grabbed)
      // still being used and robot hasn't been stopped
      do_after(1,this,KEEP_APPLYING_TOOL_CALLBACK); // was really_apply_tool_callback prior to 160801
   };
};

void Robot::forget_saved_subject() { // new on 280704
	saved_subject = NULL;
	completed(); // so don't start next event until this happens
};

void Robot::keep_applying_tool() { // new on 160801
	if (tool->is_on()) {
		really_apply_tool();
	};
};

void Robot::starting_event() {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Starting running robot debug target."));
	};
#endif
// following was correct but distracting and confusing since
// robot would flip to page 1 when starting
//	boolean showing = showing_on_a_screen();
//	if (home_notebook != NULL && home_notebook->showing_on_a_screen()) {
//		home_notebook->go_to_page(1,TRUE,FALSE,!showing,TRUE);
//	};
//	pointer_to_toolbox_notebook()->go_to_page(1,TRUE,FALSE,!showing,TRUE);
// following could be moved into done_running_body and flush this method
//   set_recent_page_number(1);
//	reset_recent_page_numbers(); // new on 301000 -- commented out on 141200 since might be working through notebook
   home_notebook_last_page_number = 0;
//   notebook_not_yet_used = TRUE; // this cycle
   if (initial_tool != NULL) {
        // a way to cheat
//      if (initial_tool->kind_of() == COPIER) {
//         ((Copier *) initial_tool)->restore_counter();
//      };
      if (tool == NULL) {
         pick_up_and_store_tool(initial_tool);
      };
   };
	if (tt_log_version_number < 73) {
		set_team_running(TRUE); // new on 050400 -- "moved" on 190405
	};
};

void Robot::last_event() {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Finished running robot debug target."));
	};
#endif
   if (initial_tool != NULL && tool == NULL) {
      add_completion(); // since add_to_chest "completes"
      pick_up_and_store_tool(initial_tool);
   };
};

Notebook *Robot::notebook_to_use(short int page_number) {
  // the rule is the notebook from the tool box is used unless
  // text was dropped and its not in there but is in the home_notebook
  // then it is just used once (unless the exact same page is referenced again)
  if (use_home_notebook ||
      (page_number != 0 && page_number == home_notebook_last_page_number) ||
      tt_toolbox == NULL) {
	  use_home_notebook = FALSE;
     home_notebook_last_page_number = page_number;
	  return(home_notebook);
  } else {
     home_notebook_last_page_number = 0;
     if (team_notebook != NULL) {
        return(team_notebook);
     } else if (tt_toolbox != NULL) {
	     return(tt_toolbox_notebook);
     } else {
		  return(NULL); // OK?
	  };
  };
};

boolean Robot::save_region() {
	if (kind_of() == ROBOT_IN_TRAINING) {
		return(tt_programmer->save_appearance_regions()); // was just programmer prior to 131202
	} else {
		return(Sprite::save_region());
	};
};

Sprite *Robot::respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected, Sprite *by,
										     boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless) {
	//if (extended) { // commented out on 170205 since this prevented arrow key editing of robot names
	//	// added on 030105 since otherwise you see an untrained robot get 'p' for a name when you press F1
	//	return(Sprite::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,size_constrained_regardless));
	//};
	boolean changed_training_counter = FALSE;
   if (training_counter == 0 && kind_of() == ROBOT_IN_TRAINING) {  // done -- do nothing
	} else if (!extended) { // condition new on 170205
		if (tt_system_mode == PUZZLE && training_counter != NO_TRAINING_COUNTER && (key >= '0' && key <= '9')) { 
			// no cheating
			play_sound(PLOP_SOUND);
		} else if (thought_bubble != NULL &&
					  thought_bubble->pointer_to_cubby() == NULL &&
					  kind_of() != ROBOT_IN_TRAINING) {
			if (key >= '0' && key <= '9' && name_from_serial_number) {
				if (training_counter == 0 || training_counter == NO_TRAINING_COUNTER) {
					set_training_counter((unsigned char) (key-'0'));
				} else {
					set_training_counter((unsigned char) ((training_counter*10)+key-'0'));
				};
				set_clean_status(DIRTY);
				changed_training_counter = TRUE;
			} else if (key == BACKSPACE && name_from_serial_number && 
						  training_counter > 0 && training_counter != NO_TRAINING_COUNTER) {
				set_training_counter((unsigned char) (training_counter/10));
				changed_training_counter = TRUE;
			};
		};
	};
   if (!changed_training_counter) { // new on 0020399
		if (name_from_user != NULL && by->can_send_keyboard_editing_keys()) { // new on 110604
			// replaced name_from_user with original_recipient on 030105 so can train robots to do this
			name_from_user->respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,size_constrained_regardless);
			name_from_serial_number = FALSE; // new on 150604
			fit_name_on_chest(); // new on 120604
			return(this);
		};
		if (extended) { // new on 240205 since F1 to unnamed robot gives it the name 'p'
			return(Sprite::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,
				    size_constrained_regardless));
		};
		string current_name = copy_string_name_from_user();
		if (current_name == NULL && rubout(key) && !extended) { 
			// added on 090499 so you can rubout generated names // !extended new on 170205
			// pretend that generated name is a user name
			current_name = new character[512];
			name(current_name);
			set_string_name_from_user(current_name); // moved inside conditional on 070799 so isn't rubbed out immediately afterwards
			delete [] current_name;
		} else {
			string new_name = update_string(current_name,key,extended);
			boolean just_modified = (current_name == new_name);
			set_string_name_from_user(new_name);
			if (new_name == NULL) { // || new_name[0] == NULL) { // removed user name so enable training counter again
				name_from_serial_number = TRUE;
			};
			if (just_modified) {
				if (current_name != NULL) delete [] current_name;
			} else {
	//			if (name != NULL) delete [] name; // taken care of by update_string
				if (new_name != NULL) delete [] new_name;
			};
		};
	};
//   completed(the_action_status);
  	// commented out the above for the following on 080400 - note that robots can be trained to label robots now
   return(Sprite::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,size_constrained_regardless));
};

ArrowKeyEditing Robot::arrow_key_editing_status() { // new on 120604
	if (name_from_user != NULL) {
		return(name_from_user->arrow_key_editing_status());
	} else { 
		return(Sprite::arrow_key_editing_status());
	};
};

void Robot::increment_training_counter() {
   if (training_counter != NO_TRAINING_COUNTER) training_counter++;
};

boolean Robot::decrement_training_counter() {
   if (training_counter == NO_TRAINING_COUNTER) return(TRUE); // ignore
   floor->set_dirty(); // since step counter needs updating
   if (training_counter == 0) { // used up all the steps
      play_sound(PLOP_SOUND);
      morph_to_inert();
      return(FALSE);
   };
   training_counter--;
   if (training_counter == 0) { // last step
      do_after(1500,this,MORPH_TO_INERT_CALLBACK); // was morph_to_inert();
      play_sound(SWITCH_SOUND);
   };
   return(training_counter > 0);
};

void Robot::send_keyboard_character(millisecond duration,
												int character,
												Sprite *subject,
												boolean label_character,
												boolean select,
												int subject_index,
												boolean next_event_send_character_to_same_subject) {
//	if (nothing_grasped_flag) {
//		completed(new_action_status);
//		return;
//	};
//#if TT_DEBUG_ON
//	if (subject->kind_of() == BIRD) {
//		log("debug this");
//	};
//#endif
	saved_subject = subject;
	if (label_character) {
		saved_subject_index = (unsigned char) subject_index;
	} else {
		saved_subject_index = no_subject_index; 
	};
//	action_status = new_action_status;
	saved_character = character;
	saved_select = (flag) select;
	saved_next_event_send_character_to_same_subject = next_event_send_character_to_same_subject;
	if (duration == 0) {
		really_send_keyboard_character();
	} else {
		do_after(duration,this,REALLY_SEND_KEYBOARD_CHARACTER_CALLBACK);
	};
};

void Robot::really_send_keyboard_character() {
	if (showing_on_a_screen()) play_sound(TYPING_SOUND);
	int original_saved_character = saved_character;
	boolean extended = (saved_character > 256); // new on 100604
	if (extended) {
		saved_character -= 256;
	};
	if (saved_subject_index != no_subject_index) { // updating label of a cubby
		((Cubby *) saved_subject)->update_label(saved_character,extended,saved_subject_index,this);
		if (kind_of() == ROBOT_IN_TRAINING) {
			record_event(LABEL_CHARACTER,this,floor,saved_subject,TRUE,original_saved_character,
				          DONT_ADD_TO_WORKING_SET,FALSE,saved_subject_index); 
		};
		saved_subject_index = no_subject_index;
		saved_subject = NULL; // don't need it any more and shouldn't save it if saving city (new on 231202)
	} else {
		if (saved_character == '\003') { // control-c to copy to clipboard
			put_on_clipboard(tool,floor,this,saved_character);
			if (showing_on_a_screen()) play_sound(POP_SOUND);
			completed();
		} else if (saved_character == '\026') { // control-v to paste from clipboard
			add_item_from_clipboard(floor,this,saved_character);
//			completed_after(clipboard_paste_duration,action_status,this);
			// added default_duration() on 020205 so this works if off screen
         do_after(default_duration(clipboard_paste_duration),this,ACTION_COMPLETED_CALLBACK); // rewritten on 131102
		} else {
			if (tt_log_version_number >= 79
#if TT_POST3187
				 && version >= 3		 
#endif
				) {
				// added on 040606 since if a robot was trained to type something to the Ether it hung
				// conditional added 061006 -- second part added on 180607 since inappropriate for saved old Pong games 
//				describe(tt_error_file(),DEFINITE_ARTICLE);
				describe_briefly(tt_error_file()); // the above is too flakey (061006)
				tt_error_file() << " typed the character " << (char) saved_character 
									 << " but there was nothing to respond to the key. Ignored" << endl;
				completed();
			};
		};
		// commented out the following on 301000
// 		if (tt_toolbox != NULL &&
//          saved_subject == pointer_to_toolbox_notebook() && // shared 
//          notebook_not_yet_used) { // then "pretend it was on page 1"
//         pointer_to_toolbox_notebook()->go_to_page(1,FALSE,FALSE,TRUE,this);
//        notebook_not_yet_used = FALSE;
//      };
		if (saved_select && kind_of() == ROBOT_IN_TRAINING &&
			 saved_subject->kind_of() == INTEGER && 
			 // obsolete feature -- should rubout instead -- once isn't always enough but good enough
			 (saved_character >= '0' && saved_character <= '9')) {
			saved_subject->respond_to_keyboard(BACKSPACE,FALSE,FALSE,this,TRUE,saved_subject); // does rubout before typing digit
			saved_select = FALSE;
		};
		Sprite *replacement = saved_subject;
		if (saved_subject != NULL) {
			saved_subject->respond_to_keyboard(saved_character,extended,saved_select,this,(kind_of() == ROBOT_IN_TRAINING),
														  saved_subject);
      };
		if (replacement != saved_subject) {
         swap_tool(replacement);
      };
		if (!saved_next_event_send_character_to_same_subject && saved_subject != NULL) {
			saved_subject->unselect(); // new on 270704 so if edited then canonicalized
		};
		if (saved_subject != NULL && saved_subject->kind_of() == PROGRAM_PAD) {
			Notebook *notebook = (Notebook *) saved_subject; 
//			 page_number_offsets != NULL &&
//			 page_number_offsets->page_number_offset((Notebook *) saved_subject) != 0) {
//			// already have a relative offset so this changed it
			if (page_number_offsets == NULL) { //was commented out
				page_number_offsets = new Page_Number_Offsets();
			};
			page_number_offsets->update(notebook);
			// commented out the following on 301000 since notebook::set_recent_page_number now deals with this
//			if (tt_toolbox != NULL &&
//             notebook == pointer_to_toolbox_notebook()) { // shared
//			   set_recent_page_number(notebook->return_recent_page_number());
//#if TT_DEBUG_ON
//				if (tt_debug_mode == 291000 || tt_debug_target == debug_counter) {
//					notebook->print(tt_error_file());
//					tt_error_file() << " shared. (really_send_keyboard_character)" << endl;
//				};
//#endif
//			};
		};
		saved_subject = NULL; // don't need it any more and shouldn't save it if saving city (new on 231202)
	};
//	completed(action_status);
};

void Robot::set_recent_page_number(short int page_number, Notebook *notebook) {
//	if (tt_toolbox != NULL && pointer_to_toolbox_notebook() == notebook) { 
	// new on 180101 since shouldn't remember from one cycle to the next what page it was on
//		return;
//	};
   // moved here and rewritten on 301000 to deal with multiple notebooks (e.g. remote controls)
#if TT_DEBUG_ON
	if (tt_debug_mode == 301000 || tt_debug_target == debug_counter || tt_debug_target == notebook->debug_counter) {
		print(tt_error_file());
		tt_error_file() << " recent page number set to " << page_number <<  " of ";
		notebook->print(tt_error_file());
		tt_error_file() << endl;
	};
#endif
	// added the following on 160103 so that this can be reset when a robot is done and still keep track of the main notebook
	if (number_of_recent_page_numbers == 0) {
		if (notebook == tt_toolbox_notebook) { // new on 180103
			number_of_recent_page_numbers = 1;
		} else {
			number_of_recent_page_numbers = 2;
		};
		notebook_ids_with_recent_page_numbers = new int[number_of_recent_page_numbers];
		recent_page_numbers = new int[number_of_recent_page_numbers]; // removed short on 060505
		notebook_ids_with_recent_page_numbers[0] = tt_toolbox_notebook->return_id();
		recent_page_numbers[0] = page_number; 
		// shouldn't matter since should be clobbered before being used if notebook is not main notebook
		if (notebook != tt_toolbox_notebook) { // new on 180103
			notebook_ids_with_recent_page_numbers[1] = notebook->return_id();
			recent_page_numbers[1] = page_number;
		};
		return;
	};
//	recent_page_number = page_number;
	// rewritten on 301000 so the robot can keep track of several notebooks that potentially are shared
	int i;
	for (i = 0; i < number_of_recent_page_numbers; i++) {
		if (notebook->return_id() == notebook_ids_with_recent_page_numbers[i]) { // re-written on 151200
			recent_page_numbers[i] = page_number;
			return;
		};
	};
	number_of_recent_page_numbers++;
	int *new_notebook_ids_with_recent_page_numbers = new int[number_of_recent_page_numbers];
	new_notebook_ids_with_recent_page_numbers[number_of_recent_page_numbers-1] = notebook->return_id();
	int *new_recent_page_numbers = new int[number_of_recent_page_numbers]; // removed both occurances of short on 060505
	new_recent_page_numbers[number_of_recent_page_numbers-1] = page_number;
	for (i = 0; i < number_of_recent_page_numbers-1; i++) {
		new_notebook_ids_with_recent_page_numbers[i] = notebook_ids_with_recent_page_numbers[i];
		new_recent_page_numbers[i] = recent_page_numbers[i];
	};
	if (number_of_recent_page_numbers > 1) { // new on 31100
		delete [] notebook_ids_with_recent_page_numbers;
		delete [] recent_page_numbers;
	};
	notebook_ids_with_recent_page_numbers = new_notebook_ids_with_recent_page_numbers;
	recent_page_numbers = new_recent_page_numbers;
};

void Robot::notebook_just_flew_out(Notebook *notebook) { // new on 201200
	set_recent_page_number(1,notebook); // reset to page 1 since notebook just flew out
};

short int Robot::return_recent_page_number(Notebook *notebook) {
//	return(recent_page_number);
	// rewritten on 301000
	for (int i = 0; i < number_of_recent_page_numbers; i++) {
		if (notebook->return_id() == notebook_ids_with_recent_page_numbers[i]) { // re-written on 151200
#if TT_DEBUG_ON
			if (tt_debug_mode == 301000 || tt_debug_target == debug_counter || tt_debug_target == notebook->debug_counter) {
				print(tt_error_file());
				tt_error_file() << " recent page number of ";
				notebook->print(tt_error_file());
				tt_error_file() << " remembered as " << recent_page_numbers[i] << endl;
			};
#endif
			return(recent_page_numbers[i]);
		};
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 301000 || tt_debug_target == debug_counter) {
		print(tt_error_file());
		tt_error_file() << " recent page number of ";
		notebook->print(tt_error_file());
		tt_error_file() << " is " << 1 << endl; //notebook->return_recent_page_number() << endl;
	};
#endif
	// restored the following on 201200 since robots start with page 1 or else whould have remembered what page they left it on
//#if TT_BUG_FIXES // commented out on 111203
//	return(-1); // new on 301103 to indicate that the robot doesn't know
//#else
	if (notebook == tt_toolbox_notebook) { // condition new on 090804
		return(1); // if don't know about it then treat it as first page - was notebook->return_recent_page_number());
	} else {
		return(notebook->left_page_number()); // new on 090804
	};
//#endif
	//	return(notebook->left_page_number()); // new on 141200
};

void Robot::reset_recent_page_numbers() { // boolean for_good) {
	if (number_of_recent_page_numbers == 0) return;
#if TT_DEBUG_ON
	if (tt_debug_mode == 301000 || tt_debug_target == debug_counter) {
		tt_error_file() << "Resetting recent page numbers of ";
		print(tt_error_file());
		tt_error_file() << endl;
	};
#endif
//	if (for_good) {
		delete [] notebook_ids_with_recent_page_numbers;
		delete [] recent_page_numbers;
		number_of_recent_page_numbers = 0;
	//} else { // keep the first one which is the main notebook - new on 160103 -- beginning 110803 I don't think this branch is ever called
	//	number_of_recent_page_numbers = 1;
	//};
};

void Robot::reset_main_notebook_page_numbers() { // new on 090804 since the above clobbers all the notebook page info
	// but main notebook is special (and index 0) since when training it is always on page 1
	if (number_of_recent_page_numbers > 0) {
		recent_page_numbers[0] = 1;
	};
};

void Robot::remove_from_cubby() {
//	if (target == NULL) return; // can't do more
	Cubby *cubby_target = (Cubby *) target; // already checked
//	Sprite *component = cubby_target->component(target_index);
//	if (component->kind_of() != NEST ||
//		 ((Nest *) component)->pointer_to_contents() != tool) {
		 cubby_target->remove_component(target_index,ROBOT_EXTRACTED,0,TRUE); // coerced to (cubby_index) prior to 070203
//												  FALSE); // was showing_on_a_screen()
//	};
	target = NULL;
};

void Robot::really_select_from_stack() {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Debug target really selecting something from a stack."));
	};
#endif
	Sprite *the_copy = saved_stack->selected(GRASPED,this); 
   // above should have added the_copy to robot's working set
	// was (saved_stack->copy(floor));
	millisecond duration = default_duration(750);
#if TT_DEBUG_ON
	if (the_copy == NULL) {
		log("Null copy - something wrong.");
	};
#endif
	if (duration > 0) { // condition new on 280403 since wasteful and when off screen robot is probably dropping it on something very soon anyway
		the_copy->animate_to_good_size(duration,tt_log_version_number >= 64?tt_toolbox:NULL);
		if (floor != NULL) {
			if (floor->in_the_room()) {
				the_copy->move_to(llx,lly); // where robot is
			};
#if TT_DEBUG_ON
		} else {
			tt_error_file() << "Selecting something from stack but isn't added to background -- memory loss." << endl;
#endif
		};
	};
	// following is important when say true_width, etc. is used
	// before update_display
//	the_copy->scale_to_fit();
//	floor->add_item(the_copy);
	// since NEW_EVENT not recorded here need to add to working set myself
//	add_to_working_set(the_copy);
//   select_from_stack_after = max_long;
	completed();
};

void Robot::destroy_house() {
	if (floor != NULL) floor->destroy_house();
};

void Robot::delta_for_robot_behind_me(city_coordinate &delta_x, city_coordinate &delta_y) { // new on 151199
	delta_x = width/5;
	delta_y = height/2;
};

void Robot::location_behind_me(city_coordinate &x, city_coordinate &y) {
	city_coordinate delta_x,delta_y;
	delta_for_robot_behind_me(delta_x,delta_y);
	x = llx+delta_x;
	y = lly+delta_y;
};

void Robot::set_home_notebook(Notebook *notebook) {
	if (home_notebook != NULL) home_notebook->destroy(); // will only decrement_ref_count if shared
	home_notebook = notebook;
	if (notebook != NULL) { // for robustness - new on 100703
		notebook->increment_ref_count(this);
	};
	if (next_robot != NULL) next_robot->set_home_notebook(notebook);
};

void Robot::set_team_notebook(Notebook *notebook) {
	if (notebook == team_notebook) { // new on 230305
		return; // already knew this
	};
	if (team_notebook != NULL) team_notebook->destroy(); // will only decrement_ref_count if shared
	team_notebook = notebook;
	if (team_notebook != NULL) { // for robustness - new on 100703
		team_notebook->increment_ref_count(this);
	};
	if (next_robot != NULL) next_robot->set_team_notebook(notebook);
};

boolean Robot::dump(output_stream &stream, boolean just_prepare, boolean , boolean ) {
   if (just_prepare) {
      return(FALSE);  // will start over again -- is that OK? -- or should it finish the body first??
		// should it finish instantly?
   };
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log("About to dump robot debug target.");
	};
	if (tt_debug_mode == 20000) {
		character robot_name[max_resource_string_length];
		name(robot_name);
		if (strnicmp(robot_name,debug_robot_name,strlen(debug_robot_name)) == 0) {
			log(_T("Dumping robot who needs debugging."));
		};
	};
#endif
   int dump_index = about_to_dump(this,stream); // rewritten on 020802
	if (dump_index < 0) return(FALSE); // new on 051099
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
		print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
		tt_error_file() << "About to dump ";
		print(tt_error_file());
		if (name_from_user != NULL) {
			tt_error_file() << " (";
			name_from_user->top_level_describe(tt_error_file());
			tt_error_file() << ")";
		};
		tt_error_file() << endl;
		tt_debug_dump_depth++;
	};
#endif
   if (training_counter != NO_TRAINING_COUNTER) {
      stream.put((unsigned char) ROBOT_WITH_TRAINING_COUNTER);
		dump_if_not_null(stream,comment);
//		dump_string(stream,name_from_user,255);
		if (name_from_serial_number) {
			stream.put(NOTHING_MARKER);
		} else {
			dump_if_not_null(stream,name_from_user);
		};
      stream.put(training_counter);
      stream.put((char) (initial_tool != NULL));
      if (initial_tool != NULL) {
         initial_tool->dump(stream);
      };
      return(TRUE);
   }; 
   if (((body != NULL || initial_tool != NULL) && thought_bubble != NULL) || kind_of() == ROBOT_IN_TRAINING) {
		// clipboard needs the thought bubble even if empty  -- not anymore??
		if (initial_tool != NULL) {
			if (blank) {
				stream.put((unsigned char) BLANK_ROBOT_WITH_TOOL);
			} else {
				stream.put((unsigned char) ROBOT_WITH_TOOL);
			};
			dump_if_not_null(stream,comment);
			if (name_from_serial_number) {
				stream.put(NOTHING_MARKER);
			} else {
				dump_if_not_null(stream,name_from_user);
			};
         initial_tool->dump(stream);
      } else if (blank) {
			stream.put((unsigned char) BLANK_ROBOT);
			dump_if_not_null(stream,comment);
			if (name_from_serial_number) {
				stream.put(NOTHING_MARKER);
			} else {
				dump_if_not_null(stream,name_from_user);
			};
		} else {
         stream.put((unsigned char) ROBOT);
			dump_if_not_null(stream,comment);
			if (name_from_serial_number) {
				stream.put(NOTHING_MARKER);
			} else {
				dump_if_not_null(stream,name_from_user);
			};
		};
      serial_number(); // creates one if it doesn't have one (Not necessary after Beta 6B)
		if (thought_bubble == NULL) {
			stream.put((byte) NONE_GIVEN);
		} else {
			thought_bubble->dump(stream);
			if (body != NULL) {
				body->dump(stream);
			} else if (thought_bubble->pointer_to_cubby() != NULL) {
				stream.put(END_OF_ROBOT_MARKER);
			};
		};
      if (!tt_dumping_to_test_equality_or_describe) {
			boolean notebooks_worth_dumping = TRUE; // added August 19 '98
			if (first_in_line == NULL) {
				if (leader == NULL) notebooks_worth_dumping = FALSE;
			} else {
				if (first_in_line->pointer_to_leader() == NULL) notebooks_worth_dumping = FALSE;
			};
			if (notebooks_worth_dumping && (contained_in(PICTURE) || contained_in(THOUGHT_BUBBLE))) { // new on 090599
				// contained_in(THOUGHT_BUBBLE) added on 080799 since no need to dump notebook if in thought bubble
				// since too many home_notebooks are being saved needlessly bloating the size of the saved encoding
				notebooks_worth_dumping = FALSE;
			};
         if (home_notebook != NULL && notebooks_worth_dumping) { // && !part_of(home_notebook)) {
            // removed !part_of stuff on Oct 6 since it didn't make sense since
				// robots that are part of notebook shouldn't be saved anyway
				// added !part_of(home_notebook) on Sept 20 since the notebook won't even exist yet when loading
				// and maybe causes other problems dumping loading notebooks with robots in them
				// commented out condition on 090599 since notebook::dump does it as well
				// doing about_to_dump twice can lead to inconsistencies
//				if (!about_to_dump(home_notebook,stream)) {
					home_notebook->dump(stream);
//				};
         } else {
            stream.put(NOTHING_MARKER);
         };
         if (team_notebook != NULL && notebooks_worth_dumping) { // && !part_of(team_notebook)) {
				// added !part_of(home_notebook) on Sept 20 (see comment above)
				// commented out condition on 090599 since notebook::dump does it as well
//				if (!about_to_dump(team_notebook,stream)) {
               team_notebook->dump(stream);
//				};
         } else {
            stream.put(NOTHING_MARKER);
         };
			// moved inside conditional on 270300
			dump_if_not_null(stream,top_cubby); // new on 190100 -- should have ref_count > 1 so will be shared with real thing
			if (top_cubby != NULL) {
				dump_working_set(stream);
				dump_robot_flags(stream);
			};
		};
	} else {
		stream.put(UNDEFINED_ROBOT_MARKER);
		dump_if_not_null(stream,comment);
		if (name_from_serial_number) {
			stream.put(NOTHING_MARKER);
		} else {
			dump_if_not_null(stream,name_from_user);
		};
		return(TRUE);
	};
	if (next_robot != NULL) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299) {
			log("About to dump next robot.");
		};
#endif
		next_robot->dump(stream);
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299) {
			log("Finished dumping next robot.");
		};
#endif
	} else if (saved_next_robot != NULL) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299) {
			log("About to dump saved next robot.");
		};
#endif
      stream.put(UNDEFINED_ROBOT_MARKER); // new on 110402 as a marker that the following isn't current next_robot but is saved_next_robot
      // NOTE should really increment file version type so old versions can load in cities with robots working on the floor
		saved_next_robot->dump(stream);
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299) {
			log("Finished dumping saved next robot.");
		};
#endif
	} else {
		stream.put(END_OF_ROBOT_MARKER);
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299) {
			log("Dumped end of robot marker.");
		};
#endif
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

void Robot::dump_robot_flags(output_stream &stream) {
	stream.write((char *) &set_down_counter, sizeof(set_down_counter));
	stream.write((char *) &recent_page_number, sizeof(recent_page_number));
	stream.write((char *) &home_notebook_last_page_number, sizeof(home_notebook_last_page_number));
//	 Page_Number_Offsets *page_number_offsets; -- another reason to flush home_notebook and the like?
	dump_if_not_null(stream,first_to_fail_for_unstable_reasons);
   unsigned char flags = (body_started_flag | 
								  (running_body_flag<<1) |
								  (waiting_for_release_flag<<2) |
								  (suspended_flag<<3) |
//								  (inside_sprite_flag<<4) | // noticed obsolete on 131202
								  (use_home_notebook<<5)); // probably covered elsewhere
//								  (notebook_not_yet_used<<6)); -- free for re-used
	stream.put(flags);
};

void Robot::load_robot_flags(InputStream *stream, int version_number, NaturalLanguage language, boolean ignore) {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Load robot flags of debug target."));
	};
#endif
	stream->read((string) &set_down_counter, sizeof(set_down_counter));
	stream->read((string) &recent_page_number, sizeof(recent_page_number));
	stream->read((string) &home_notebook_last_page_number, sizeof(home_notebook_last_page_number));
	if (ignore) {
		if (!ignore_next_item(stream,version_number)) {
			return; // new on 120101 - should this return a boolean??
		};
	} else {
		set_first_to_fail_for_unstable_reasons((Robot *) load_item(stream,version_number,language));
	};
   unsigned char flags = stream->get();
	body_started_flag = !!(flags&1);
	running_body_flag = !!(flags&(1<<1));
	set_waiting_for_release(!!(flags&(1<<2)));
	suspended_flag = !!(flags&(1<<3));
//	inside_sprite_flag = !!(flags&(1<<4));
	use_home_notebook = !!(flags&(1<<5));
//	notebook_not_yet_used = !!(flags&(1<<6)); -- can be re-used (301000)
};

void Robot::dump_working_set(output_stream &stream) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
		log("About to dump working set.");
	};
#endif
	stream.write((char *) &working_set_length, sizeof(working_set_length));
	for (int i = 0; i < working_set_length; i++) {
		// all of these should refer to "old" (i.e. already dumped) items
		// paths??
		if (working_set[i] != NULL) {
			working_set[i]->dump(stream);
			stream.put((char) working_set[i]->available_for_indexing());
		} else {
			stream.put(NOTHING_MARKER);
		};
	};
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
		log("Finished dumping working set.");
	};
#endif
};

void Robot::load_working_set(InputStream *pad_in, int version_number, NaturalLanguage language, boolean ignore) {
	pad_in->read((string) &working_set_length, sizeof(working_set_length));
	if (working_set_length == 0) return; // no need to do more
	if (!ignore && body != NULL) { // conditional new on 140301 - second conjunct added 290701
		max_working_set_size = body->rest()->first()->pointer_to_path()->return_index(); // new on 300300 to get max size from START_EVENT
	};
	if (working_set_length > max_working_set_size || working_set_length < 0) {
		log("Loading a robot's memory went wrong. Sorry.",FALSE,TRUE); 
		return;
	};
	if (ignore) {
		for (int i = 0; i < working_set_length; i++) {
			unsigned char type = pad_in->get();
			if (type != NOTHING_MARKER) {
				ignore_next_item((SpriteType) type,pad_in,version_number,language);
				pad_in->get(); // and ignore is_available_for_indexing
			};
		};		
	} else {
#if TT_DEBUG_ON
		if (debug_counter == tt_debug_target) {
			log("loading working set of debug target.");
		};
#endif
		while (working_set_length > max_working_set_size && max_working_set_size < working_set_size_limit) { 
			// need to grow it - new on 303000
			max_working_set_size = min(max(2,max_working_set_size*2),working_set_size_limit);
		};
		working_set = new SpritePointer[max_working_set_size]; 
		int i;
		for (i = 0; i < working_set_length; i++) {
			// all of these should refer to "old" (i.e. already dumped) items
			// rewrote the following on 300300 from using set_working_set_element since then I'd need to initialize things with NULL just to reset them
			working_set[i] = load_item(pad_in,version_number,language);
			if (working_set[i] != NULL) {
				working_set[i]->increment_ref_count(this); 
				boolean is_available_for_indexing = (boolean) pad_in->get();
				working_set[i]->set_available_for_indexing(is_available_for_indexing);
			};
		};
		for (; i < max_working_set_size; i++) { // new on 280300
			working_set[i] = NULL;
		};
	};
};

Robot *load_robot_with_training_counter(InputStream *pad_in,
                                        int notebook_version, NaturalLanguage language, long index, Sprite *container, boolean descendants_inactive) {
   Robot *robot = undefined_robot(pad_in,notebook_version,language,index,container,descendants_inactive);
   robot->set_training_counter((unsigned char) pad_in->get());
   if (pad_in->get()) {
      robot->add_initial_tool(load_item(pad_in,notebook_version,language));
   };
   return(robot);
};

Robot *undefined_robot(InputStream *pad_in,
                       int notebook_version, NaturalLanguage language, long index, Sprite *container, boolean descendants_inactive) {
	Sprite *comment = NULL;
	Text *name = NULL;
	if (notebook_version >= first_version_to_save_copied_nests) {
		comment = load_item(pad_in,notebook_version,language);
		name = (Text *) load_item(pad_in,notebook_version,language);
	};
   Thought_Bubble *thought_bubble = new Thought_Bubble(0,0,2,2*tile_width);
   Robot *robot = new Robot(0,0,3,2*tile_width,0);
   if (container != NULL) {  // new on 160702
      if (container->kind_of() == PROGRAM_PAD) {
         robot->set_in_notebook((Notebook *) container,FALSE); // no point recuring
      } else if (container->kind_of() == VACUUM) {
         robot->set_inside_vacuum(TRUE,FALSE);
      };
   }; 
	associate_shared_item(index,robot);
	robot->update_display(); // added 241199 to improve geometry
   robot->connect_to_thought_bubble(thought_bubble);
	robot->set_comment(comment);
	robot->set_name_from_user(name);
   return(robot);
};

Robot *load_robot(SpriteType type, InputStream *pad_in, int notebook_version, NaturalLanguage language, int index, Sprite *container, boolean descendants_inactive) {
	Cubby *bubble_cubby = NULL;
	if (type == OLD_SHARED_ITEM || type == NEW_SHARED_ITEM || type == EQUAL_TO_OLD_ITEM ) { //  added on 060300
		// no need for || type == EQUAL_TO_OLD_ITEM_AND_GEOMETRY since robots aren't saved with geometry
		Sprite *sprite = load_item(type,pad_in,notebook_version,language,NULL,NULL,index);
		if (sprite != NULL && sprite->kind_of() == ROBOT) {
			return((Robot *) sprite);
		} else if (sprite != NULL && sprite->kind_of() == CUBBY) {
			bubble_cubby = (Cubby *) sprite;
		} else {
			say_error(IDS_TROUBLES_LOADING_ROBOT,TRUE);
			return(NULL);
		};
#if TT_CAREFUL
	} else if (type != CUBBY && type != LABELED_CUBBY && type != BLANK_CUBBY && type != NONE_GIVEN && type != NOTHING_MARKER) {
//		tt_error_file() << "Load_robot assumes thought bubble contains a cubby or is empty" << endl;
		if (tt_dumping_to_test_equality_or_describe || tt_loading_to_describe) { // conditional added on 290300
			log("Something went wrong trying to describe a robot.");
		} else {
			say_error(IDS_TROUBLES_LOADING_ROBOT,TRUE);
		};
		return(NULL); // should this throw instead??
#endif
	};
	Events *body = NULL;
	if (type != NONE_GIVEN) {
		if (bubble_cubby == NULL && type != NOTHING_MARKER) {
			// following added on 280199 to ensure that nothing is shared within or between thought bubbles
			// new on 090799 so robot's in thought bubbles load properly
			UnwindProtect<boolean> set(tt_copying_entire_thought_bubble,TRUE); // a little white lie - really loading thought bubble
			bubble_cubby = (Cubby *) load_item(type,pad_in,notebook_version,language,NULL,NULL,-1,FALSE,FALSE,TRUE); // added all these args on 220702 to make the items in the cubby be inactive
		}; // else is a trained robot with empty thought bubble
		body = load_events(pad_in,notebook_version);
	};
	Robot *robot = new Robot(0,0,3,6*tile_width,0,body);
   if (container != NULL) {  // new on 160702
      if (container->kind_of() == PROGRAM_PAD) {
         robot->set_in_notebook((Notebook *) container,FALSE); // no point recuring
      } else if (container->kind_of() == VACUUM) {
         robot->set_inside_vacuum(TRUE,FALSE);
      };
   }; 
	associate_shared_item(index,robot); // moved here from caller on 010200 so that self references work OK
//#if TT_DEBUG_ON
	// this is only heurstically accurate since can copy to clipboard and lose notebook version
	// well if this is important can save this as well
//   robot->set_body_version_number((unsigned char) notebook_version);
//#endif
//	robot->save_size();
	Thought_Bubble *thought_bubble = new Thought_Bubble(0,0,2,6*tile_width);
	if (bubble_cubby != NULL) {
		bubble_cubby->set_priority(1);
		thought_bubble->receive_cubby(bubble_cubby);
		thought_bubble->propagate_changes(); // is this a good idea??
		robot->update_display(); // and this??
	};
	robot->connect_to_thought_bubble(thought_bubble);
   if (notebook_version >= first_version_to_save_cities && !tt_dumping_to_test_equality_or_describe) {
		UnwindProtect<boolean> set(tt_loading_robot_notebook,TRUE);
      Sprite *home_notebook = load_item(pad_in,notebook_version,language);
      if (home_notebook != NULL) {
			if (home_notebook->kind_of() != PROGRAM_PAD) {
				say_error(IDS_TROUBLES_LOADING_ROBOT,TRUE);
			} else {
				robot->set_home_notebook((Notebook *) home_notebook);
//         home_notebook->decrement_ref_count(); // created with 1 and set_home_notebook incremented it
#if TT_DEBUG_ON
				if (tt_debug_mode == 1717) {
					tt_error_file() << "Ref count of home pad#" << home_notebook->return_debug_counter() 
										 << " is " <<  home_notebook->return_ref_count() << endl;
				};
#endif
			};
      };
      Sprite *team_notebook = load_item(pad_in,notebook_version,language);
      if (team_notebook != NULL) {
			if (team_notebook->kind_of() != PROGRAM_PAD) {
				say_error(IDS_TROUBLES_LOADING_ROBOT,TRUE);
			} else {
				robot->set_team_notebook((Notebook *) team_notebook);
//         team_notebook->decrement_ref_count(); // created with 1 and set_home_notebook incremented it
#if TT_DEBUG_ON
				if (tt_debug_mode == 1717) {
					tt_error_file() << "Ref count of team pad#" << team_notebook->return_debug_counter() 
										 << " is " <<  team_notebook->return_ref_count() << endl;
				};
#endif
			};
      };
   };
	if (notebook_version >= first_version_to_save_cubby_of_robot && // new on 190100
		 !tt_dumping_to_test_equality_or_describe) { // conjunct added on 270300
		UnwindProtect<boolean> set(tt_copying_entire_thought_bubble,FALSE); 
		// in case this is a robot in a thought bubble (perhaps on the back of a picture)
		Cubby *working_cubby = (Cubby *) load_item(pad_in,notebook_version,language);
		if (working_cubby != NULL) {
			robot->set_working_cubby(working_cubby); 
			robot->load_working_set(pad_in,notebook_version,language);
			robot->load_robot_flags(pad_in,notebook_version,language);
		};
	};
	return(robot);
};

Robot *load_robot_line(InputStream *pad_in,
                       int notebook_version, NaturalLanguage language,
							  unsigned char type, long index, Sprite *container, boolean descendants_inactive) {
   Sprite *initial_tool = NULL;
	Sprite *comment = NULL;
	Text *name = NULL;
	if (notebook_version >= first_version_to_save_copied_nests) {
	   comment = load_item(pad_in,notebook_version,language);
		name = (Text *) load_item(pad_in,notebook_version,language);
	};
   if (type == ROBOT_WITH_TOOL || type == BLANK_ROBOT_WITH_TOOL) {
      initial_tool = load_item(pad_in,notebook_version,language);
   };
   SpriteType cubby_type = (SpriteType) pad_in->get(); // get next type token
	Robot *first_robot = load_robot(cubby_type,pad_in,notebook_version,tt_language,index,container,descendants_inactive);
	if (first_robot == NULL) return(NULL); // problems
	first_robot->set_comment(comment);
	if (first_robot->pointer_to_body() != NULL) { // conditional added on 081099
		first_robot->set_name_from_user(name);
	};
	if (type == BLANK_ROBOT_WITH_TOOL || type == BLANK_ROBOT) {
		first_robot->become_blank();
	};
   if (initial_tool != NULL) first_robot->add_initial_tool(initial_tool);
	Robot *current_robot = first_robot;
   boolean robot_is_saved_next_robot = FALSE;
	for (;;) {
     initial_tool = NULL; // unless told otherwise
     unsigned char training_counter = NO_TRAINING_COUNTER; // default
     boolean load_initial_tool = TRUE;
     boolean last_type_was_new_shared = (type == NEW_SHARED_ITEM);
	  type = (unsigned char) pad_in->get();
     if (type == UNDEFINED_ROBOT_MARKER) { // new on 110402 
        robot_is_saved_next_robot = TRUE;
        type = (unsigned char) pad_in->get();
     } else if (!last_type_was_new_shared) {
         robot_is_saved_next_robot = FALSE;
     };
	  if (type == END_OF_ROBOT_MARKER) {
		  return(first_robot);
	  };
	  name = NULL;
	  boolean read_comment_and_name = FALSE;
	  switch (type) {
        case ROBOT_WITH_TOOL:
		  case BLANK_ROBOT_WITH_TOOL:
			  if (notebook_version >= first_version_to_save_copied_nests) { 
				  // moved here on 260200 to deal properly with OLD_SHARED_ITEM and NEW_SHARED_ITEM 
				  comment = load_item(pad_in,notebook_version,language);
				  name = (Text *) load_item(pad_in,notebook_version,language);
				  read_comment_and_name = TRUE;
			  };
           if (load_initial_tool) {
              initial_tool = load_item(pad_in,notebook_version,language);
           };
           // and then the following too
		  case BLANK_ROBOT:
		  case ROBOT: {
			  if (!read_comment_and_name && notebook_version >= first_version_to_save_copied_nests) { 
				  // moved here on 260200 to deal properly with OLD_SHARED_ITEM and NEW_SHARED_ITEM 
				  comment = load_item(pad_in,notebook_version,language);
				  name = (Text *) load_item(pad_in,notebook_version,language);
//				  read_comment_and_name = TRUE;
			  };
			  SpriteType cubby_type = (SpriteType) pad_in->get();
			  Robot *new_robot = load_robot(cubby_type,pad_in,notebook_version,language,index,container,descendants_inactive);
			  if (new_robot == NULL) return(NULL); // problems - warn??
			  new_robot->set_comment(comment);
			  new_robot->set_name_from_user(name);
//           new_robot->set_first_in_line(first_robot); // commented out on 250802 since handled by set_saved_next_robot and set_next_robot now
           if (robot_is_saved_next_robot) {
              current_robot->set_saved_next_robot(new_robot);
           } else {
//			     current_robot->put_behind_me(new_robot); // commented out on 250802 since handled by set_saved_next_robot and set_next_robot now
   			  current_robot->set_next_robot(new_robot);
           };
			  current_robot = new_robot;
           if (initial_tool != NULL) new_robot->add_initial_tool(initial_tool);
			  new_robot->set_training_counter(training_counter);
			  if (type == BLANK_ROBOT_WITH_TOOL || type == BLANK_ROBOT) {
				  new_robot->become_blank();
			  };
           break;
			 };
		  case OLD_SHARED_ITEM: // added on 010200 since robots now saved pointers between themselves a bit
		  case EQUAL_TO_OLD_ITEM: // new on 060601
//		  case EQUAL_TO_OLD_ITEM_AND_GEOMETRY: // should occur here
			 {
				unsigned short int saved_index;
				pad_in->read((string) &saved_index, sizeof(saved_index));
				Sprite *item = shared_item(saved_index);
				if (item == NULL) {
					log("NULL old index."); // OK to ignore it??
				} else if (item->kind_of() == ROBOT) {
					if (type == EQUAL_TO_OLD_ITEM) { // new on 110601
						item = item->top_level_copy();
					};
					Robot *new_robot = (Robot *) item;
//               new_robot->set_first_in_line(first_robot); // commented out on 250802 since handled by set_saved_next_robot and set_next_robot now
               if (robot_is_saved_next_robot) {
                  current_robot->set_saved_next_robot(new_robot); // new on 110402
               } else {
//					   current_robot->put_behind_me(new_robot); // commented out on 250802 since handled by set_saved_next_robot and set_next_robot now
				      current_robot->set_next_robot(new_robot);
               };
//				   current_robot = new_robot;
				} else {
					log("Non-robot loaded while loading robot team!");
				};
				return(first_robot); // new on 270200 since if old has saved entire chain of robots
         };
		  case NEW_SHARED_ITEM: //  added on 010200
			  {
            unsigned short int saved_index;
				pad_in->read((string) &saved_index, sizeof(saved_index));
				index = saved_index; // and then loop around and use this new index
				break;
			  };
		  default:
			 say_error(IDS_ROBOT_FILE_BAD,TRUE);
			 return(NULL);
	  };
	};
};

void describe_named_robot(SpriteType thought_bubble_type, InputStream *pad_in, output_stream &text_out, 
								  int version_number, Text *name_from_user) {
	Robot *robot = load_robot(thought_bubble_type,pad_in,version_number,tt_language,-1,NULL,FALSE);
	if (robot == NULL) { // something wrong
		return;
	};
	robot->set_name_from_user(name_from_user);
	character robot_name[max_resource_string_length];
   robot->name(robot_name);
	print_type(ROBOT,text_out,INDEFINITE_ARTICLE);
	text_out << space() << robot_name;
	robot->destroy();
};

boolean describe_robot_line(InputStream *pad_in, output_stream &text_out, int version_number, Article article, 
									 boolean in_thought_bubble) {
	Text *name_from_user = NULL;
	if (version_number >= first_version_to_save_copied_nests) {
		describe_comment_encoding(pad_in,text_out,version_number,ROBOT);
		UnwindProtect<boolean> save(tt_loading_to_describe);
		SpriteType type = (SpriteType) pad_in->get(); // new on 190201
		if (type == NOTHING_MARKER) { // then do nothing (new on 080301)
		} else if (type == TEXT || type == VARIABLE_WIDTH_TEXT || type == NEW_SHARED_ITEM || type == OLD_SHARED_ITEM  || type == EQUAL_TO_OLD_ITEM) { 
			// last added on 060601 - previous last 2 disjuncts added on 260201 - 
			// 2nd one added on 280201 - EQUAL_TO_OLD_ITEM_AND_GEOMETRY not needed here
			tt_loading_to_describe = TRUE;
			Sprite *name = load_item(type,pad_in,latest_version_number,tt_language);
			if (name != NULL && name->kind_of() == TEXT) {
				name_from_user = (Text *) name;
			} else {
				if (name != NULL) name->destroy(); // new on 240601
				return(FALSE); // new on 260201
			};		
		} else {
			return(FALSE); // new on 190201
		};
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299) {
			if (name_from_user != NULL) {
				long length;
				string name;
				name_from_user->current_text(name,length);
				tt_error_file() << "About to load a robot named " << name << endl; 
			} else {
				tt_error_file() << "About to load a nameless robot." << endl;
			};
		};
#endif
	};
	SpriteType thought_bubble_type = (SpriteType) pad_in->get();
//	print_article(article,text_out);
	if (thought_bubble_type == UNDEFINED_ROBOT_MARKER) {
		text_out << S(IDS_ROBOT_DESCRIBE1);
		return(TRUE);
	};
	int count = 1;
	if (in_thought_bubble) {
		describe_named_robot(thought_bubble_type,pad_in,text_out,version_number,name_from_user);
		name_from_user = NULL; // new on 250103 since the above now uses it up
	} else {
		text_out << S(IDS_ROBOT_DESCRIBE2);
		if (!describe_robot(thought_bubble_type,count,pad_in,text_out,version_number)) {
			if (name_from_user != NULL) name_from_user->destroy(); // new on 240601
			return(FALSE); // stopped
		};
	};
   boolean describe_initial_tool = TRUE;
   int training_counter = NO_TRAINING_COUNTER;
	for (;;) {
	  count++;
	  unsigned char type = (unsigned char) pad_in->get();
	  switch (type) {
		  case END_OF_ROBOT_MARKER:
			  if (name_from_user != NULL) name_from_user->destroy(); // new on 240601
			  return(TRUE);
		  case BLANK_ROBOT_WITH_TOOL:
		  case BLANK_ROBOT:
			  {
				  if (version_number >= first_version_to_save_copied_nests) {
					  describe_comment_encoding(pad_in,text_out,version_number,ROBOT);
					  if (!ignore_next_item(pad_in,version_number,tt_language)) { // name
						  if (name_from_user != NULL) name_from_user->destroy(); // new on 240601
						  return(FALSE); // new on 120101
					  };
				  };
				  thought_bubble_type = (SpriteType) pad_in->get();
				  UnwindProtect<boolean> set(tt_loading_to_describe,TRUE);
				  Robot *ignore = load_robot(thought_bubble_type,pad_in,version_number,tt_language,-1,NULL,FALSE);
				  if (in_thought_bubble) {
					  print_type(BLANK_ROBOT,text_out,INDEFINITE_ARTICLE);
				  } else {
					  print_type(BLANK_ROBOT,text_out,article);
				  };
				  ignore->destroy();
			  };
			  break;
        case ROBOT_WITH_TRAINING_COUNTER:
			  // restored the following on 250103 since this should use set_name_from_user
			  if (name_from_user != NULL) name_from_user->destroy(); 
			  // new on 240601 -- commented out on 160103 since set_name_from_user does this (once again)
			  if (version_number >= first_version_to_save_copied_nests) {
				  describe_comment_encoding(pad_in,text_out,version_number,ROBOT);
				  name_from_user = (Text *) load_item(pad_in,latest_version_number,tt_language); 
			  } else {
				  name_from_user = NULL;
			  };
			  thought_bubble_type = (SpriteType) pad_in->get(); 
			  if (in_thought_bubble) {
				  describe_named_robot(thought_bubble_type,pad_in,text_out,version_number,name_from_user);
				  name_from_user = NULL; // new on 250103 since the above now uses it up
			  };
			  training_counter = pad_in->get();
			  describe_initial_tool = (boolean) pad_in->get();
           // and the following
        case ROBOT_WITH_TOOL:
		  case ROBOT:
			  if ((type == ROBOT_WITH_TOOL || type == ROBOT) && 
				    version_number >= first_version_to_save_copied_nests) {
				  describe_comment_encoding(pad_in,text_out,version_number,ROBOT);
				  UnwindProtect<boolean> set(tt_loading_to_describe,TRUE);
				  // shouldn't this destroy the old name_from_user? -asked 250103
				  name_from_user = (Text *) load_item(pad_in,latest_version_number,tt_language); 
			  } else {
				  name_from_user = NULL;
			  };
#if TT_DEBUG_ON
				if (tt_debug_mode == 160299) {
					if (name_from_user != NULL) {
						long length;
						string name;
						name_from_user->current_text(name,length); 
						tt_error_file() << "About to load another robot. His name is " << name << endl;
					} else {
						tt_error_file() << "About to load another robot. He doesn't have a name." << endl;
					};
				};
#endif
			  thought_bubble_type = (SpriteType) pad_in->get();
			  if (in_thought_bubble) {
				  describe_named_robot(thought_bubble_type,pad_in,text_out,version_number,name_from_user);
				  name_from_user = NULL; // new on 250103 since the above now uses it up
				  break;
			  } else if (!describe_robot(thought_bubble_type,count,pad_in,text_out,version_number)) return(FALSE);
           if (training_counter != NO_TRAINING_COUNTER) {
              text_out << space() << S(IDS_WHO_CAN_ONLY_REMEMBER) << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << training_counter
                       << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_ACTIONS);
           };
           if (type == ROBOT_WITH_TOOL) {
              if (describe_initial_tool) {
					  text_out << space() << S(IDS_WHO_IS_HOLDING) << space();
					  describe_item(pad_in,text_out,version_number);
					  text_out << S(IDS_WHO_IS_HOLDING2,"");
				  } else {
					  if (!ignore_next_item(pad_in,version_number)) {
						  return(FALSE); // new on 120101
					  };
				  };
           };
			  break;
		  case OLD_SHARED_ITEM: // new on 290300
		  case NEW_SHARED_ITEM: // new on 290300
		  case EQUAL_TO_OLD_ITEM: // new on 060601
		  case EQUAL_TO_OLD_ITEM_AND_GEOMETRY: { // new on 120601
				  unsigned short int ignore_index;
				  pad_in->read((string) &ignore_index, sizeof(ignore_index));
				  if (type == EQUAL_TO_OLD_ITEM_AND_GEOMETRY) {
					  tt_global_picture->load_geometry(pad_in,version_number); // ignore geometry
				  };
				  count--; // new on 280103 - since will go again and number shouldn't have been incremented - thanks Gordon and Leonel
			  			  };
			  break;
		  default:
//			 tt_error_file() << "Warning: Bad notebook file. Robot missing end marker." << endl;
			 text_out << space() << S(IDS_TROUBLE_FIGURING_OUT_ROBOT) << space();
			 if (name_from_user != NULL) name_from_user->destroy(); // new on 240601
			 return(FALSE);
	  };
	};
};

#if TT_XML

//xml_element *Robot::xml_create_element(xml_document *document) {
//   return(create_xml_element(xml_tag(),document));
//};

void Robot::xml_working_set(xml_element *parent, xml_document *document) { // abstracted on 140603
	xml_element *working_set_element = create_xml_element(L"MadeOrMovedSoFar",document,working_set_length);
   for (int i = 0; i < working_set_length; i++) {
		if (working_set[i] != NULL) {
			//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
			//	// need to check since robot and working set could all be on the back of a picture
			//	working_set[i]->check_for_independence();
			//};
         xml_element *working_element_element = working_set[i]->xml_create_and_append_element(working_set_element,document,TRUE);
         xml_set_attribute(working_element_element,L"Count",i);
			working_element_element->Release(); // new on 20 0804
//       working_set[i]->set_attributes_misc_elements(working_element_element); // happens anyway
		};
	};
	xml_append_child(working_set_element,parent); // missing prior to 231202 // moved here on 190804
   xml_set_attribute(parent,L"Max",max_working_set_size); // new on 071102 -- was redundant so is probably obsolete as of 040504
	// but kept in case this is loaded before the body is?
};

void Robot::xml(xml_element *element, xml_document *document) {
   if (training_counter != NO_TRAINING_COUNTER) {
      xml_append_child(create_xml_element(L"MaximumTrainingSteps",document,training_counter),element);
   };
   if (initial_tool != NULL) {
//      xml_append_child(create_xml_element(L"ToolInHand",document,initial_tool),element);
		initial_tool->add_xml(element,L"ToolInHand",document); // rewritten 180803 to get sharing right
   };
   if (!name_from_serial_number && name_from_user != NULL) {
      wide_string name_string;
      long ignore_name_string_length;
      name_from_user->current_wide_text(name_string,ignore_name_string_length);
      xml_append_child(xml_character_data_element(name_string,L"Label",document),element); 
		// prior to 220203 was create_xml_element but didn't deal with new lines and special characters right
		int insertion_point = name_from_user->return_insertion_point();
		if (insertion_point >= 0) { // new on 120604
			xml_set_attribute(element,L"EditInsertionPoint",insertion_point);
		};
	};
   if (thought_bubble != NULL) {
      thought_bubble->xml_create_and_append_element(element,document)->Release();
	};
   if (body != NULL) {
      xml_element *body_element = create_xml_element(L"Actions",document);
      body->xml(body_element,document);
      xml_append_child(body_element,element);
      if (top_cubby != NULL) {
			//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 -- is this needed? - can it be also on the floor or elsewhere?
			//	top_cubby->check_for_independence();
			//};
//			top_cubby->xml_create_and_append_element(create_xml_element(L"WorkingInfo",document),document);
         xml_element *working_element = create_xml_element_shared(L"WorkingInfo",document,top_cubby);
			if (working_set_length > 0) { // || max_working_set_size != default_max_working_set_size) { // second disjunct new on 030504 to be sure max is recorded
				xml_working_set(working_element,document);            
         };
         xml_set_attribute(working_element,L"SetDownCounter",(int) set_down_counter); // shouldn't really be over 2**31
			if (drop_scroll_delta_x != 0) { // condition new on 050404
				xml_set_attribute(working_element,L"DropScrollDeltaX",drop_scroll_delta_x); // new on 040404
			};
			if (drop_scroll_delta_y != 0) {
				xml_set_attribute(working_element,L"DropScrollDeltaY",drop_scroll_delta_y); // new on 040404
			};
         xml_set_attribute(working_element,L"RecentPageNumber",recent_page_number);
//	      stream.write((char *) &home_notebook_last_page_number, sizeof(home_notebook_last_page_number));
         if (first_to_fail_for_unstable_reasons != NULL) {
				//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 -- is this needed? - can it be also on the floor or elsewhere?
				//	first_to_fail_for_unstable_reasons->check_for_independence();
				//};
            xml_append_child(create_xml_element_shared(L"FirstTeammateToFail",document,first_to_fail_for_unstable_reasons),working_element);
			};
         if (body_started_flag) {
            xml_set_attribute(working_element,L"StartedRunning",1);
         };
         if (running_body_flag) {
            xml_set_attribute(working_element,L"StillRunning",1);
         };
         if (waiting_for_release_flag) {
            xml_set_attribute(working_element,L"WaitingForDrop",1);
         };
         if (suspended_flag) {
            xml_set_attribute(working_element,L"Suspended",1);
         };
         //if (inside_sprite_flag) {
         //   xml_set_attribute(working_element,L"InsideAnother",1);
         //};
         // use_home_notebook??
         xml_append_child(working_element,element); // forgotten prior to 081002
		};
	} else if (working_set_length > 0) { // || max_working_set_size != default_max_working_set_size) { 
		// second disjunct new on 030504 to be sure max is recorded
		// shouldn't this be attached to WorkingInfo?
		xml_working_set(element,document);            
	};
//   if (!tt_dumping_to_test_equality_or_describe) { // commented out on 131202 since can't be otherwise
	// commented out on 231202 since this should be dumped when dumping special stuff (add_special_xml)
		//boolean notebooks_worth_dumping = TRUE; // added August 19 '98
		//if (first_in_line == NULL) {
		//	if (leader == NULL) notebooks_worth_dumping = FALSE;
		//} else {
		//	if (first_in_line->pointer_to_leader() == NULL) notebooks_worth_dumping = FALSE;
		//};
		//if (notebooks_worth_dumping && (contained_in(PICTURE) || contained_in(THOUGHT_BUBBLE))) { // new on 090599
		//	// contained_in(THOUGHT_BUBBLE) added on 080799 since no need to dump notebook if in thought bubble
		//	// since too many home_notebooks are being saved needlessly bloating the size of the saved encoding
		//	notebooks_worth_dumping = FALSE;
		//};
  //    if (home_notebook != NULL && notebooks_worth_dumping && being_dumped_together(this,home_notebook)) { 
  //       // added being_dumped_together on 290802 since why dump notebook unless also part of the object or city - 
  //       // is this more general than the computation for notebooks_worth_dumping??
  //       xml_append_child(create_xml_element(L"NotebookICameFromw",document,home_notebook,TRUE),element);
  //    };
  //    if (team_notebook != NULL && notebooks_worth_dumping) {
  //       xml_append_child(create_xml_element(L"NotebookThatWasInTruckWithMe",document,team_notebook,TRUE),element);
  //    };
//   };
	if (next_robot != NULL) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 -- is this needed? - can it be also on the floor or elsewhere?
		//	next_robot->check_for_independence();
		//};
      xml_append_child(create_xml_element_shared(L"Next",document,next_robot),element);
	} else if (saved_next_robot != NULL) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 -- is this needed? - can it be also on the floor or elsewhere?
		//	saved_next_robot->check_for_independence();
		//};
      xml_append_child(create_xml_element_shared(L"NextAfterRunning",document,saved_next_robot),element);
	}; 
	xml_set_attribute(element,L"Version",version);
   Sprite::xml(element,document);
};

xml_element *Robot::xml_set_special_attributes(xml_element *element, xml_document *document) {
	// new on 131202 -- went through all the state variables
	xml_element *special_element = Sprite::xml_set_special_attributes(element,document);
	if (target_index >= 0) {
		xml_set_attribute(special_element,L"TargetIndex",target_index);
	};
	if (saved_subject_index != no_subject_index) {
		xml_set_attribute(special_element,L"SavedSubjectIndex",saved_subject_index);
	};
	if (saved_character != 0) {
		xml_set_attribute(special_element,L"SavedCharacter",saved_character);
	};
	if (body_status != 0) {
		xml_set_attribute(special_element,L"BodyStatus",body_status);
	};
	if (run_again_this_cycle) {
		xml_set_attribute(special_element,L"RunAgainThisCycle",1);
	};
	if (waiting_for_release_flag) {
		xml_set_attribute(special_element,L"WaitingForRelease",1);
	};
	if (saved_select) {
		xml_set_attribute(special_element,L"SavedSelect",1);
	};
	if (abort_reason != 0) {
		xml_set_attribute(special_element,L"AbortReason",abort_reason);
	};
	if (saved_key != 0) {
		xml_set_attribute(special_element,L"SavedKey",saved_key);
	};
	if (use_home_notebook) {
		xml_set_attribute(special_element,L"UseHomeNotebook",1);
	};
	if (home_notebook_last_page_number != 0) {
		xml_set_attribute(special_element,L"HomeNotebookLastPageNumber",home_notebook_last_page_number);
	};
	if (released_on_its_stack) {
		xml_set_attribute(special_element,L"ReleasedOnItsStack",1);
	};	
	if (original_training_counter != NO_TRAINING_COUNTER) {
		xml_set_attribute(special_element,L"OriginalMaximumTrainingSteps",original_training_counter);
	};
	if (tool_width != 0) {
		xml_set_attribute(special_element,L"ToolWidth",tool_width);
	};
	if (tool_height != 0) {
		xml_set_attribute(special_element,L"ToolHeight",tool_height);
	};
	if (!name_from_serial_number) {
		xml_set_attribute(special_element,L"NameFromUser",1);
	};
	if (remove_me_from_backside_flag) {
		xml_set_attribute(special_element,L"RemoveMeFromBackside",1);
	};
	if (team_running_flag) {
		xml_set_attribute(special_element,L"TeamRunning",1);
	};
	if (last_failure_was_for_unstable_reasons_flag) {
		xml_set_attribute(special_element,L"LastFailureWasForUnstableReasons",1);
	};
	if (saved_stack != NULL) {
		xml_set_attribute(special_element,L"SavedStackTypeCode",(int) saved_stack->kind_of());
	};
	if (number_of_recent_page_numbers > 0) {
		xml_set_attribute(special_element,L"NumberOfRecentPageNumbers",number_of_recent_page_numbers);
		for (int i = 0; i < number_of_recent_page_numbers; i++) {
			xml_element *page_number_element = create_xml_element(L"Notebook",document); 
			xml_set_attribute(page_number_element,L"Index",i);
         xml_set_attribute(page_number_element,L"RecentPageNumber",recent_page_numbers[i]);
			xml_set_attribute(page_number_element,L"NotebookID",notebook_ids_with_recent_page_numbers[i]);
			xml_append_child(page_number_element,special_element); // moved here on 190804
		};
	};
	if (page_number_offsets != NULL) {
		int count = page_number_offsets->return_count();
		xml_set_attribute(special_element,L"NumberOfPageOffsets",count);
		for (int i = 0; i < count; i++) {
			Notebook *notebook = page_number_offsets->notebook(i);
			if (notebook != NULL) { // for robustness but should always be - new on 090803
				xml_element *page_offset_element = create_xml_element(L"PageOffset",document); 
				xml_set_attribute(page_offset_element,L"Index",i);
				xml_set_attribute(page_offset_element,L"Offset",page_number_offsets->offset(i));
				notebook->xml_create_and_append_element(page_offset_element,document,TRUE);
				xml_append_child(page_offset_element,special_element); // moved here on 190804
			};
		};
	};
	if (tt_suspensions->is_robot_waiting_for_programmer(this)) { // new on 120104 so can reconstruct tt_suspensions simply
		xml_set_attribute(special_element,L"WaitingForProgrammer","1");
	};
	if (saved_next_event_send_character_to_same_subject) { // new on 270704 for editing text and number pads
		xml_set_attribute(special_element,L"SavedNextEventSendsCharacterToSameSubject",1);
	};
	return(special_element);
};

boolean Robot::xml_get_special_attributes(xml_node *node) {
	target_index = xml_get_attribute_int(node,L"TargetIndex",0); // prior to 210404 was target_index
	saved_subject_index = xml_get_attribute_int(node,L"SavedSubjectIndex",no_subject_index); // prior to 210404 was saved_subject_index
	saved_character = xml_get_attribute_int(node,L"SavedCharacter",0); // prior to 210404 was saved_character
	body_status = xml_get_attribute_int(node,L"BodyStatus",0); // prior to 210404 was body_status
	run_again_this_cycle = xml_get_attribute_int(node,L"RunAgainThisCycle",0); // prior to 210404 was run_again_this_cycle 
	// above had typo prior to 120603 so time travel broke
	waiting_for_release_flag = xml_get_attribute_int(node,L"WaitingForRelease",0); // prior to 210404 was waiting_for_release_flag
	saved_select = xml_get_attribute_int(node,L"SavedSelect",0); // was missing prior to 281004
	abort_reason = xml_get_attribute_int(node,L"AbortReason",0); // prior to 210404 was abort_reason
	saved_key = xml_get_attribute_int(node,L"SavedKey",0); // prior to 210404 was saved_key
	use_home_notebook = xml_get_attribute_int(node,L"UseHomeNotebook",0); // prior to 210404 was use_home_notebook
	home_notebook_last_page_number = xml_get_attribute_int(node,L"HomeNotebookLastPageNumber",0); 
	// prior to 210404 above was home_notebook_last_page_number
	released_on_its_stack = xml_get_attribute_int(node,L"ReleasedOnItsStack",0); // prior to 210404 was released_on_its_stack
	original_training_counter = xml_get_attribute_int(node,L"OriginalMaximumTrainingSteps",NO_TRAINING_COUNTER); 
	// prior to 210404 above was original_training_counter -- restored on 051004
	if (original_training_counter == 0) { // in case was stored with this screwy value -- new on 051004
		original_training_counter = NO_TRAINING_COUNTER;
	};
	tool_width = xml_get_attribute_int(node,L"ToolWidth",0); // prior to 210404 was tool_width
	tool_height = xml_get_attribute_int(node,L"ToolHeight",0); // prior to 210404 was tool_height
	name_from_serial_number = !xml_get_attribute_int(node,L"NameFromUser",1); // prior to 210404 was !name_from_serial_number
	remove_me_from_backside_flag = xml_get_attribute_int(node,L"RemoveMeFromBackside",0); // prior to 210404 was remove_me_from_backside_flag
	team_running_flag = xml_get_attribute_int(node,L"TeamRunning",0); // prior to 210404 was team_running_flag
	last_failure_was_for_unstable_reasons_flag = xml_get_attribute_int(node,L"LastFailureWasForUnstableReasons",0); 
	// prior to 210404 was last_failure_was_for_unstable_reasons_flag
	number_of_recent_page_numbers = xml_get_attribute_int(node,L"NumberOfRecentPageNumbers",0); 
	// prior to 210404 was number_of_recent_page_numbers
	int saved_stack_type_code = xml_get_attribute_int(node,L"SavedStackTypeCode",-1);
	if (saved_stack_type_code >= 0) {
		if (tt_toolbox == NULL) {
			tt_toolbox = new Toolbox();
			tt_toolbox->initialize(); // new on 140603
		};
		saved_stack = tt_toolbox->pointer_to_stack((SpriteType) saved_stack_type_code);
	};
	if (number_of_recent_page_numbers > 0) {
		recent_page_numbers = new int[number_of_recent_page_numbers]; // removed short on 060505
		notebook_ids_with_recent_page_numbers = new int[number_of_recent_page_numbers];
	};	
	int page_offsets_count = xml_get_attribute_int(node,L"NumberOfPageOffsets",0);
	if (page_offsets_count > 0) {
		page_number_offsets = new Page_Number_Offsets(page_offsets_count);
	};
	if (number_of_recent_page_numbers > 0 || page_offsets_count > 0) { // worth checking this conditional?
		// new on 231202 to process tags that are part of the VolatileAttributes tag
		xml_entity(node,this);
	};
	if (xml_get_attribute_int(node,L"WaitingForProgrammer",0)) { // new on 120104
		insert_at_end_if_not_member(this,tt_running_robots);
		// try_clause(top_cubby,1); // doesn't work since floor==NULL but the above does the same job
	};
	saved_next_event_send_character_to_same_subject = xml_get_attribute_int(node,L"SavedNextEventSendsCharacterToSameSubject",0); // new 270704
	return(Sprite::xml_get_special_attributes(node));
};

void Robot::add_special_xml(xml_element *element, xml_document *document) {
	if (tool != NULL) {
		if (tool->pointer_to_leader() != this) { // new on 261003 since may be between :grasp_tool and :really_grasp_tool
			tool->add_xml(element,L"NextToolInHand",document);
		} else		
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 110703 
		//	tool->check_for_independence();
		//};
		tool->add_xml(element,L"CurrentToolInHand",document); // ToolInHand already taken by initial_tool
	};
	if (target != NULL) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	target->check_for_independence();
		//};
		target->add_xml(element,L"CurrentTarget",document); // Current is a nice naming convention for these kinds of things
	};
	if (vacuum != NULL) { // might hold state in its tummy
		vacuum->xml_create_and_append_element(element,document)->Release();
	};
	if (copier != NULL) { // buttons are state
		copier->xml_create_and_append_element(element,document)->Release();
	};
	if (expander != NULL) {
		expander->xml_create_and_append_element(element,document)->Release();
	};
	if (saved_subject != NULL) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	saved_subject->check_for_independence();
		//};
		saved_subject->add_xml(element,L"SavedSubject",document);
	};
	if (robot_to_put_behind != NULL) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	robot_to_put_behind->check_for_independence();
		//};
		robot_to_put_behind->add_xml(element,L"RobotToPutBehind",document);
	};
	//if (saved_stack != NULL) { // made an attribute on 181202
	//	saved_stack->add_xml(element,L"SavedStack",document);
	//};
	if (suspended_on != NULL) {
	   // just used by Marty to give more useful help
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	suspended_on->check_for_independence();
		//};
		suspended_on->add_xml(element,L"SuspendedOn",document);
	};
	// following moved here and conditions simplified on 231202
   if (home_notebook != NULL) { 
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703
		//	home_notebook->check_for_independence();
		//};
      xml_append_child(create_xml_element_shared(L"NotebookICameFrom",document,home_notebook),element);
   };
   if (team_notebook != NULL) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703
		//	team_notebook->check_for_independence();
		//};
      xml_append_child(create_xml_element_shared(L"NotebookThatWasInTruckWithMe",document,team_notebook),element);
   };
	// first_in_line recomputed on loading
	Sprite::add_special_xml(element,document);
};

void handle_working_set_element(Entity *robot, Tag tag, xml_node *node, void *extra_info) {
   ((Robot *) robot)->handle_working_set_element(tag,node);
};

boolean Robot::handle_working_set_element(Tag tag, xml_node *node) {
   int index = xml_get_attribute_int(node,L"Count",-1);
   if (index >= 0 && index < max_working_set_size && sprite_tag(tag)) {
      working_set[index] = xml_load_sprite(node,tag);
		if (working_set[index] != NULL) {
			working_set[index]->increment_ref_count(this); 
         return(TRUE);
		};
	};
   return(TRUE);
};

void handle_working_info(Entity *robot, Tag tag, xml_node *node, void *extra_info) {
   ((Robot *) robot)->handle_working_info(tag,node);
};

boolean Robot::handle_working_info(Tag tag, xml_node *node) {
   switch (tag) {
      case BOX_TAG:
//    case LINK_TAG:
			set_working_cubby((Cubby *) xml_load_sprite(node,tag,top_cubby,CUBBY));
         return(top_cubby != NULL);
      case MADE_OR_MOVED_SO_FAR_TAG: 
         working_set_length = xml_get_int(node,0);
         if (working_set_length == 0) return(TRUE); // no need to do more
			// the following is probably redundant as of 040504 since max_working_set_size is now derived from the START_EVENT
         max_working_set_size = xml_get_attribute_int(node,L"Max",max_working_set_size); // changed default to max_working_set_size on 231202 from working_set_length 
		   while (working_set_length > max_working_set_size && max_working_set_size < working_set_size_limit) { 
			   max_working_set_size = min(max(2,max_working_set_size*2),working_set_size_limit);
		   };
		   working_set = new SpritePointer[max_working_set_size]; 
		   for (int i = 0; i < max_working_set_size; i++) { 
			   working_set[i] = NULL;
		   };
         return(xml_process_children(node,::handle_working_set_element,this));
      case FIRST_TEAMMATE_TO_FAIL_TAG: 
         first_to_fail_for_unstable_reasons = (Robot *) xml_get_sprite(node,first_to_fail_for_unstable_reasons,ROBOT);
         return(first_to_fail_for_unstable_reasons != NULL); 
      case NO_MORE_TAGS:
         return(TRUE);
      default:
         return(FALSE); // warn?? save??
   };
};

boolean Robot::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {   
      case THOUGHT_BUBBLE_TAG:
			// rewritten yet again on 181202 to get the geometry right
			thought_bubble = (Thought_Bubble *) xml_load_sprite(node,tag,thought_bubble,THOUGHT_BUBBLE);
			if (thought_bubble != NULL) {
				if (thought_bubble->pointer_to_leader_offsets() != NULL) {
					add_follower(thought_bubble,FALSE,INSERT_BY_PRIORITY,TRUE);
					thought_bubble->update_display(); // experiment on 280803
					thought_bubble->set_robot(this);
				} else { 
					// new on 280103
					update_display();
					new_thought_bubble(TRUE,thought_bubble);
				};
			};
//			new_thought_bubble(TRUE,(Thought_Bubble *) xml_load_sprite(node,tag)); // rewritten on 271102 since can now have more stuff (like geometry - though that is stupid)
         //Sprite *cubby = xml_get_sprite(node);
         //new_thought_bubble(TRUE);
         //if (cubby != NULL) {
         //   if (cubby->kind_of() == CUBBY) {  
         //      thought_bubble->receive_cubby((Cubby *) cubby);
         //   } else {
         //      cubby->destroy(); // warn??
         //   };
         //};
         return(TRUE);
      case ACTIONS_TAG: 
         set_body(xml_load_events(node,TRUE,this));
         return(TRUE);
         /*int serial_number = xml_get_attribute_int(node,L"ID",0);
         int version_number = xml_get_attribute_int(node,L"Version",latest_robot_version);
         int working_set_size = xml_get_attribute_int(node,L"MadeOrMovedCount",0);
         Events *events = new Events(create_serial_number(serial_number,version_number),new Events(create_first_event(working_set_size)));
         set_body(events);
         xml_process_children(node,handle_actions_tag,this,events);
         events->last()->set_rest(new Events(create_last_event()));
         return(TRUE);*/
		case LABEL_TAG: {
         string name = xml_get_narrow_text_copy(node);
			if (name != NULL) {
#if TT_DEBUG_ON
				if (tt_debug_mode == 70900) {
					if (strnicmp(name,debug_robot_name,strlen(debug_robot_name)) == 0) {
						tt_error_file() << "Loading robot " << name << "#" << debug_counter << " on frame " << tt_frame_number << endl;
					};
				};
#endif
				set_string_name_from_user(name);
				delete [] name; // new on 160103
			};
							 };
         return(TRUE);
      case NEXT_TAG: 
//			set_next_robot((Robot *) xml_get_sprite(node,next_robot,ROBOT));
			// the above does way too much work - rewritten on 260103
			set_next_robot((Robot *) xml_get_sprite(node,next_robot,ROBOT),FALSE); 
			// restored on 120203 and added FALSE arg so first_in_line fixed
			if (next_robot != NULL && next_robot->pointer_to_leader() != NULL) { 
				// new on 061003 since can have another robot as leader
				// would it be better to fix this problem earlier upstream??
				next_robot->pointer_to_leader()->remove_follower(next_robot);
			};
			add_follower(next_robot,FALSE,INSERT_AT_END,TRUE);
         return(next_robot != NULL);
      case NEXT_AFTER_RUNNING_TAG: 
			set_saved_next_robot((Robot *) xml_get_sprite(node,saved_next_robot,ROBOT));
			if (saved_next_robot != NULL && saved_next_robot->pointer_to_background() == NULL && pointer_to_background() != NULL) { 
				// new on 291003
				// saved while separated and need to give saved_next_robot the same background as this has
				saved_next_robot->set_background(pointer_to_background(),FALSE,FALSE); 
				// added args on 031103 so this doesn't generate warnings
			};
         return(saved_next_robot != NULL);
      case TOOL_IN_HAND_TAG:
         set_initial_tool(xml_get_sprite(node));
			add_follower(initial_tool,FALSE,INSERT_AT_END,TRUE); // new on 180803
         return(TRUE);
      case NOTEBOOK_I_CAME_FROM_TAG: 
         set_home_notebook((Notebook *) xml_get_sprite(node,home_notebook,PROGRAM_PAD));
         return(home_notebook != NULL);
      case NOTEBOOK_THAT_WAS_IN_TRUCK_WITH_ME_TAG: 
			set_team_notebook((Notebook *) xml_get_sprite(node,team_notebook,PROGRAM_PAD));
         return(team_notebook != NULL);
      case MAXIMUM_TRAINING_STEPS_TAG:
         set_training_counter(xml_get_int(node,NO_TRAINING_COUNTER));
         return(TRUE);
      case WORKING_INFO_TAG:
         // rewritten on 071102
         set_down_counter = xml_get_attribute_int(node,L"SetDownCounter",set_down_counter);
			drop_scroll_delta_x = xml_get_attribute_int(node,L"DropScrollDeltaX",0); // new on 040404 -- fixed default on 050404
			drop_scroll_delta_y = xml_get_attribute_int(node,L"DropScrollDeltaY",0); // new on 040404
         recent_page_number = xml_get_attribute_int(node,L"RecentPageNumber",recent_page_number);
         body_started_flag = xml_get_attribute_int(node,L"StartedRunning",0); // prior to 210404 was body_started_flag
         running_body_flag = xml_get_attribute_int(node,L"StillRunning",0); // prior to 210404 was running_body_flag
         waiting_for_release_flag = xml_get_attribute_int(node,L"WaitingForDrop",0); // prior to 210404 was waiting_for_release_flag
         suspended_flag = xml_get_attribute_int(node,L"Suspended",0); // prior to 210404 was suspended_flag
//         inside_sprite_flag = xml_get_attribute_int(node,L"InsideAnother",inside_sprite_flag);
         return(xml_process_children(node,::handle_working_info,this,NULL));
         //Sprite *top_cubby = xml_get_sprite(node);
         //if (top_cubby != NULL && top_cubby->kind_of() == CUBBY) {
         //   set_working_cubby((Cubby *) top_cubby);
         //}; // else warn or complain??
		case MADE_OR_MOVED_SO_FAR_TAG: // like WORKING_INFO but simpler - used when saving a robot during training -- new on 140603
			return(handle_working_info(tag,node));
		case NOTEBOOK_TAG: {
			if (recent_page_numbers == NULL) return(FALSE); // something is wrong
			int index = xml_get_attribute_int(node,L"Index",-1);
			if (index < 0 || index >= number_of_recent_page_numbers) {
				return(FALSE); // something wrong
			};
//			if (number_of_recent_page_numbers > 0) { // condition implied by condition above
				recent_page_numbers[index] = xml_get_attribute_int(node,L"RecentPageNumber",0);
				notebook_ids_with_recent_page_numbers[index] = xml_get_attribute_int(node,L"NotebookID",0);
//			};
			return(TRUE);
							};
		case PAGE_OFFSET_TAG: // new on 161202 
			if (page_number_offsets != NULL) {
				page_number_offsets->set_page_offset(xml_get_attribute_int(node,L"Index",0),
																 xml_get_attribute_int(node,L"Offset",0),
																 (Notebook *) xml_get_sprite(node));
			}; // otherwise something is wrong
			return(TRUE);
		case CURRENT_TOOL_IN_HAND_TAG:
			initialize_tool(xml_get_sprite(node));
			if (tt_log_version_number >= 55) { // condition new on 191204
				propagate_changes(); // so it gets the right geometry relative to this -- new on 191004
			};
			// following was redundant since done by initialize_tool -- commented out on 191004
			//if (tool != NULL) { // new on 171202
			//	tool->increment_ref_count();
   //         add_follower(tool,FALSE,INSERT_BY_PRIORITY,TRUE);
			//};
			return(TRUE);
		case NEXT_TOOL_IN_HAND_TAG: // new on 261003 -- not holding it yet but about to
			tool = xml_get_sprite(node);
			return(TRUE);
		case CURRENT_TARGET_TAG:
			target = xml_get_sprite(node);
			return(TRUE);
		case SAVED_SUBJECT_TAG:
			saved_subject = xml_get_sprite(node);
			return(TRUE);
		case ROBOT_TO_PUT_BEHIND_TAG:
//			robot_to_put_behind = (Robot *) xml_get_sprite(node);
			set_robot_to_put_behind((Robot *) xml_get_sprite(node)); // rewrote the above on 230105
			return(TRUE);
		//case SAVED_STACK_TAG:
		//	saved_stack = xml_get_sprite(node);
		//	return(TRUE);
		case SUPSENDED_ON_TAG:
			suspended_on = xml_get_sprite(node);
			return(TRUE);
		case VACUUM_TAG:
			vacuum = (Vacuum *) xml_load_sprite(node,VACUUM_TAG,vacuum,VACUUM);
//			initialize_tool(vacuum); // commented out on 190703 since CURRENT_TOOL_IN_HAND_TAG will take care of this if appropriate 
			return(TRUE);
		case MAGIC_WAND_TAG:
			copier = (Copier *) xml_load_sprite(node,MAGIC_WAND_TAG,copier,COPIER);
//			initialize_tool(copier);
			return(TRUE);
		case BIKE_PUMP_TAG:
			expander = (Expander *) xml_load_sprite(node,BIKE_PUMP_TAG,expander,EXPANDER);
//			initialize_tool(expander);
			return(TRUE);
		//case LINK_TAG: 
		//	initialize_tool(xml_load_sprite(node,LINK_TAG));
		//	if (tool != NULL) {
		//		switch (tool->kind_of()) {
		//			case VACUUM:
		//				vacuum = (Vacuum *) tool;
		//				break;
		//			case COPIER:
		//				copier = (Copier *) tool;
		//				break;
		//			case EXPANDER:
		//				expander = (Expander *) tool;
		//				break;
		//			default: 
		//				return(FALSE); // warn too??
		//		};
		//	};
			return(TRUE);
		case NO_MORE_TAGS:
			version = xml_get_attribute_int(node,L"Version",1); // new on 120504 -- if version isn't saved then set to 1
			if (name_from_user != NULL) { // new on 120406
				int insertion_point = xml_get_attribute_int(node,L"EditInsertionPoint",-1); 
				if (insertion_point >= 0) {
					name_from_user->set_insertion_point(insertion_point);
				};
			};
			// and the following
      default:
         return(Sprite::handle_xml_tag(tag,node)); 
   };
};

void Robot::initialize_tool(Sprite *item) {
	tool = item;
	if (tool != NULL) { // new on 171202
		tool->increment_ref_count();
		if (tool->pointer_to_leader() == NULL) { // condition new on 111104
			add_follower(tool,FALSE,INSERT_BY_PRIORITY,TRUE);
		} else if (tool->pointer_to_leader() != this) {
			// new on 111104 but probably never happens
			log("Robot initialized with a tool that seems to be a part of something else.",FALSE,TRUE);
		};
	};
};

#endif

boolean Robot::equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, SpritesPointer &expected_corresponding_nests, EqualReason reason) { // new on 060601
	if (other->kind_of() != kind_of()) {
		return(FALSE);
	};
	if (other == this) return(TRUE);
	if (is_blank() != other->is_blank()) {
		return(FALSE);
	};
	Robot *other_robot = (Robot *) other;
   if (pointer_to_thought_bubble() != NULL) {
		if (other_robot->pointer_to_thought_bubble() != NULL) {
			if (!pointer_to_thought_bubble()->equal_to(other_robot->pointer_to_thought_bubble(),pending,corresponding_nests,expected_corresponding_nests,reason)) {
				return(FALSE);
			};
		} else {
			return(pointer_to_thought_bubble()->pointer_to_cubby() == NULL); // was FALSE prior to 041201 -- robot without thought bubble (e.g. in Tooly) is same as empty thought bubble
		};
	} else if (other_robot->pointer_to_thought_bubble() != NULL) {
		return(other_robot->pointer_to_thought_bubble()->pointer_to_cubby() == NULL); // was FALSE prior to 041201 -- robot without thought bubble (e.g. in Tooly) is same as empty thought bubble
	};
	// could use serial number to compare bodies except it currently can have accidentical conflicts
	if (pointer_to_body() != NULL) {
		if (other_robot->pointer_to_body() == NULL) {
			return(FALSE);
		};
		if (!pointer_to_body()->equal_to(other_robot->pointer_to_body())) {
			return(FALSE);
		};
	} else if (other_robot->pointer_to_body() != NULL) {
		return(FALSE);
	};
	//if (return_name_from_user() != NULL) {
	//	if (other_robot->return_name_from_user() != NULL) {
	//		if (!return_name_from_user()->equal_to(other_robot->return_name_from_user(),pending,corresponding_nests,expected_corresponding_nests,reason)) return(FALSE);
	//	} else if (!name_was_generated()) { // condition new on 260105 since name may have been generated by displaying
	//		return(FALSE);
	//	};
	//} else if (other_robot->return_name_from_user() != NULL && !other_robot->name_was_generated()) { 
	//	// second condition new on 260105 since name may have been generated by displaying
	//	return(FALSE);
	//};
	Text *my_name_from_user = return_name_from_user();
	Text *others_name_from_user = other_robot->return_name_from_user();
	if (my_name_from_user == NULL && body != NULL) {
		// new on 260105 to generate a name
		set_name_from_user(NULL); // uses serial number
	};
	if (others_name_from_user == NULL && other_robot->pointer_to_body() != NULL) {
		// new on 260105 to generate a name
		other_robot->set_name_from_user(NULL);
	};
	if (my_name_from_user != NULL) {
		if (others_name_from_user != NULL) {
			if (!my_name_from_user->equal_to(others_name_from_user,pending,corresponding_nests,expected_corresponding_nests,reason)) {
				return(FALSE);
			};
		} else {
			return(FALSE);
		};
	} else if (other_robot->return_name_from_user() != NULL) { 
		return(FALSE);
	};
	if (pointer_to_next_robot() != NULL) {
		if (other_robot->pointer_to_next_robot() != NULL) {
			if (!pointer_to_next_robot()->equal_to(other_robot->pointer_to_next_robot(),pending,corresponding_nests,expected_corresponding_nests,reason)) return(FALSE);
		} else {
			return(FALSE);
		};
	} else if (other_robot->pointer_to_next_robot() != NULL) {
		return(FALSE);
	};
	return(TRUE);
};

boolean describe_robot(SpriteType thought_bubble_type, int count, InputStream *pad_in, output_stream &text_out, int version_number) {
	if (stop_generating_help()) {
		text_out << S(IDS_MORE_ROBOT_SAID_ENOUGH);
		return(FALSE);
	};
	if (count > 1) {
		text_out << end_sentence() << S(IDS_ROBOT_DESCRIBE4);
      print_ordinal_definite_phrase(count,text_out,
                                    TRUE,S(IDS_ROBOT_DESCRIBE5_1),
                                    S(IDS_ROBOT_DESCRIBE5_2));
//      print_the(text_out,TRUE,NONE_GIVEN,S(IDS_ROBOT_DESCRIBE5));
//      text_out << " ";
//		print_ordinal(count,text_out);
//		text_out << " " << S(IDS_ROBOT_DESCRIBE5);
	};
	if (!describe_encoding(thought_bubble_type,pad_in,text_out,version_number,TRUE)) {
		return(FALSE); // new on 060201
	};
	text_out << end_sentence() << S(IDS_ROBOT_DESCRIBE3);
	if (!describe_body(pad_in,text_out)) {
		return(FALSE);
	};
	text_out << S(IDS_ROBOT_DESCRIBE32,"");
	if (!tt_dumping_to_test_equality_or_describe) {
		UnwindProtect<boolean> set(tt_loading_to_describe,TRUE); // new on 290300
		Sprite *ignore = load_item(pad_in);
		if (ignore != NULL) ignore->destroy();
		ignore = load_item(pad_in);
		if (ignore != NULL) ignore->destroy();
	// problem with the following is that describe_pad isn't fully functional - still?? isn't it working OK now?
	//  ignore_next_item(pad_in); // home notebook
	//  ignore_next_item(pad_in); // team notebook
		if (version_number >= first_version_to_save_cubby_of_robot) { // fixed this on 130200 - 2 weeks "late"
			unsigned char type = pad_in->get();
			if (type != NOTHING_MARKER) {
//				ignore_next_item((SpriteType) type,pad_in,version_number); // commented out this line and restored the following
				// on 270300 since the working set etc isn't dumped if no top cubby
				Sprite *working_cubby = NULL;
				if (type == OLD_SHARED_ITEM || type == EQUAL_TO_OLD_ITEM) { 
					// no need to check for EQUAL_TO_OLD_ITEM_AND_GEOMETRY for robots
					unsigned short int index; // to ingore the index
					pad_in->read((string) &index, sizeof(index));
				} else { // treated specially on 290300 since can be NULL but that is OK
					working_cubby = load_item((SpriteType) type,pad_in,version_number); // added type, on 290300
				};
				if (type == OLD_SHARED_ITEM || type == EQUAL_TO_OLD_ITEM || (working_cubby != NULL && working_cubby->kind_of() == CUBBY)) {
					Robot *dummy_robot = new Robot();
					dummy_robot->load_working_set(pad_in,version_number,tt_language,TRUE); // ignore
					dummy_robot->load_robot_flags(pad_in,version_number,tt_language,TRUE);
					if (working_cubby != NULL) working_cubby->destroy();
					dummy_robot->destroy();
				};
			};
		};
	};
	return(TRUE);
};

//void Robot::move_tool() {
//	if (!clean() && tool != NULL) {
//		tool->move_by(llx-previous_llx,lly-previous_lly);
//	};
//};

void Robot::stop_running_body() {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Stopped running body of debug target.");
	};
	if (tt_debug_mode == 2710) {
		tt_error_file() << debug_counter << " ";
		if (first_in_line != NULL) {
			tt_error_file() << first_in_line->debug_counter;
		};
		tt_error_file() << endl;
	};
#endif
//	if (body != NULL) body->decrement_ref_count(); // queue incremented it
//	top_cubby = NULL;
	no_longer_suspended();
//	if (top_cubby != NULL) { // e.g. not called by stop_team
//		top_cubby->decrement_busy_counter(); // unlock it
//	};
	running_body_flag = FALSE;
	body_started_flag = FALSE;
	set_down_counter = 0;
	set_team_running(FALSE); // new on 050400
};

void Robot::done_running_body(boolean start_again, boolean stop_team_too) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Robot debug target done running body.");
	};
#endif
	target = NULL; // new on 051004 since if retraining may interrupt the use of a tool and things becomes a dangling reference
//	running_body_flag = FALSE; // new on 210300 since I rely upon this in really_try_clause now
	Picture *picture_back = NULL; // restored on 201299 since destroying team only if on back of picture
	if (floor != NULL) picture_back = floor->pointer_to_picture();
	Background *team_background;
	if (first_in_line == NULL) {
		team_background = floor;
	} else { // this is new on 090699
		team_background = first_in_line->pointer_to_background();
	};
	if (remove_me_from_backside() || // added on 120499 so if truck drives off with cubby robot is removed later
		 (top_cubby != NULL && picture_back != NULL &&
		  (top_cubby->inside_vacuum() || // top level cubby has been sucked up
		    // added floor->pointer_to_picture() == NULL on 190299 since otherwise robot is destroyed when flipped pictures are on flipped pictures
		   (top_cubby->pointer_to_background() != team_background)))) {
		  // or sent away
		// moved here on 1/5/99 so that if a robot sucks up his cubby or sends it away
	   // and is on the back of a picture that the robot is removed as well
		if (first_in_line != NULL) {
			floor->remove_item(first_in_line,FALSE);
//			first_in_line->destroy(); // commented out on 251000
		} else {
			floor->remove_item(this,FALSE);
//			picture_back->remove_flipped_follower(this); // remove_item should do this now
//			destroy(); // commented out on 220405 -- what was this all about?? -- why didn't it cause problems sooner?
		};
		if (stop_team_too && top_cubby != NULL) { // conditions added on 250399 to stop infinite recursion
			stop_team(); // stop_team will call this again
		};
		return; // since this entire team is about to be deleted
	};
	boolean retraining = (kind_of() == ROBOT_IN_TRAINING && still_running());
	stop_running_body();
	if (retraining) { // re-did all that was trained to do
		tt_programmer->stop_retraining(this);
		return;
	};
	if (working_set != NULL) {
#if TT_DEBUG_ON
      if (tt_debug_mode == 1333) {
         log(_T("Done running body so working set is now zero."),FALSE,TRUE);
      };
		if (tt_debug_target == debug_counter) {
			log("Reseting working set item of debug target.");
		};
#endif
		release_working_set_elements(); // to release ref counts - new on 280300
		delete [] working_set;
		working_set = NULL;
		working_set_length = 0;
	};
	//if (tt_toolbox != NULL) { // new on 180101 since main notebook should act like it is back to page 1 -- commented out on 160103
	//	set_recent_page_number(1,pointer_to_toolbox_notebook());
	//};
	// restored the following on 160103 since now treats specially the main notebook
	if (start_again) {
		reset_main_notebook_page_numbers(); 
		// new branch as of 090804 since if iterating through a notebook (other than the main one) should keep going rather than restarting
	} else {
		reset_recent_page_numbers(); // arg is obsolete as of 110803 since no longer continue to flip through a notebook - arg was !start_again 
	};
	// above new on 311000 so this is reclaimed sooner -- commented out on 141200 since robot might be going thru the notebook
	// changed on 110803 so start_again should always be FAlSE
	if (tool != initial_tool) {
		if (tool != NULL) {
			tool->destroy(this); // decrement ref count new on 011199
			tool = NULL; // seems it isn't always cleaned 
		};
	};
	if (vacuum != NULL) {
// following was true so remove_item was commented -- but
// now all robot's followers are not completely inside robot
// already released it which removed it from floor
		if (floor != NULL) floor->remove_item(vacuum,FALSE,TRUE);
		if (vacuum->pointer_to_leader() == this) {
			remove_follower(vacuum);
		};
		vacuum->destroy(this);
		vacuum = NULL;
	};
	if (copier != NULL) {
		if (start_again) { 
			// branch new on 230405 -- wasteful otherwise
			copier->set_state(COPIER_NORMAL);
		} else {
			if (floor != NULL) {
				floor->remove_item(copier,FALSE,TRUE);
			};
			if (copier->pointer_to_leader() == this) {
				remove_follower(copier);
			};
			copier->destroy();
			copier = NULL;
		};
	};
	if (expander != NULL) {
		if (start_again) { 
			// branch new on 230405 -- wasteful otherwise
			expander->set_state(EXPANDER_BIGGER);
		} else {
			if (floor != NULL) floor->remove_item(expander,FALSE,TRUE);
			if (expander->pointer_to_leader() == this) {
				remove_follower(expander);
			};
			expander->destroy();
			expander = NULL;
		};
	};
//	if (deleted_sprites != NULL) {
//		if (tt_deleted_sprites == NULL) {
//			tt_deleted_sprites = deleted_sprites;
//		} else {
//			tt_deleted_sprites->last_cons()->set_rest(deleted_sprites);
//		};
////		delete deleted_sprites;
//		deleted_sprites = NULL;
//	};
//   reset_action_status(pointer_to_body_status());
	if (page_number_offsets != NULL) {
		delete page_number_offsets;
		page_number_offsets = NULL;
	};
	if (!start_again || // just called to clean up
		 top_cubby == NULL) { // added on 181000 since if vacuumed up its box I get spurious warnings otherwise
		return; 
	};
//	boolean delay_a_bit = touched_a_remote();
	if (first_in_line == NULL) {
		// delay a frame (implemented as 1 milleseond delay) if touched a remote 
		try_clause(top_cubby,!run_again_this_cycle);
//		if (delay_a_bit) {
//			tt_running_robots = new Sprites(this,tt_running_robots);
//		};
	} else {
//	   play_sound(SHUTDOWN_SOUND,1);
		first_in_line->try_clause(top_cubby,!run_again_this_cycle);
//		if (delay_a_bit) {
//			tt_running_robots = new Sprites(first_in_line,tt_running_robots);
//		};
	};
};

void Robot::connect_to_thought_bubble(Thought_Bubble *new_thought_bubble,
												  boolean bubble_moves) {
	// thought bubble is still there for collisions but
	// is updated as a follower of this robot
	if (thought_bubble == NULL) {
		thought_bubble = new_thought_bubble;
	};
	if (bubble_moves) {
		thought_bubble->move_to(llx+width/5,lly+true_height(), // height replaced with true_height on 161199
										ROBOT_ATTACHED_TO_THOUGHT_BUBBLE);
	} else {
		city_coordinate bubble_llx, bubble_lly;
		thought_bubble->lower_left_corner(bubble_llx,bubble_lly);
//   bubble_width = thought_bubble->true_width();
//   bubble_height = thought_bubble->current_height();
		move_to(bubble_llx-true_width()/2, // -true_width()/2 added 161199
				  bubble_lly-(4*true_height()/5), // also changed on 161199 (9*height)/10, // (9*good_height())/10,
				  ROBOT_ATTACHED_TO_THOUGHT_BUBBLE);
	}; 
// shouldn't be an item
//   floor->remove_item(thought_bubble,FALSE); // robot will take care of it
   thought_bubble->remove_from_floor(FALSE,FALSE);
	add_follower(thought_bubble,FALSE);
//	floor->add_item(thought_bubble,TRUE,TRUE);
	thought_bubble->set_priority(current_priority);
//	thought_bubble_attached = TRUE;
	thought_bubble->set_robot(this);
//	thought_bubble->blank_all_remotes();
};

void Robot::follower_changed(Sprite *follower) {
	switch (follower->kind_of()) {
		case THOUGHT_BUBBLE:
			city_coordinate bubble_llx, bubble_lly, bubble_width;
			follower->lower_left_corner(bubble_llx, bubble_lly);
			bubble_width = follower->current_width();
			move_to(bubble_llx+(4*bubble_width)/5-width/3,
					  bubble_lly-(4*height)/5);
			Sprite::follower_changed(follower);
			break;
	}; // could do this for next robot as well
};

//void Robot::delete_when_deleted(Sprite *sprite) { // noticed obsolete on 161202
//	// might still be in the working_set so don't delete now
//	// but now that I use ref counts does this make sense??
//#if TT_DEBUG_ON
//	if (deleted_sprites != NULL && deleted_sprites->member_cons(sprite) != NULL) {
//		log("deleting same sprite twice!");
//		return;
//	};
//#endif
//	deleted_sprites = new Sprites(sprite,deleted_sprites);
//};

void Robot::set_priority(long new_priority) {
   if (tt_log_version_number < 56 && // changed so this only runs if running old demos on 211004
		 current_parameter() != ROBOT_INERT && current_parameter() != ROBOT_MORPH_TO_INERT &&
       floor != NULL && floor->in_the_room()) {
      new_priority = min_long+1000000L; // on top of everything on the floor and bammer too but not programmer or Marty
   }
	Sprite::set_priority(new_priority);
	if (thought_bubble != NULL) {
		thought_bubble->set_priority(new_priority+1);
	};
	if (tool != NULL) {
		tool->set_priority(new_priority-1);
      if (tool->kind_of() == COPIER) {
         Sprite *attached = ((Copier *) tool)->attached_tool();
         if (attached != NULL) attached->set_priority(new_priority-2);
      };
	};
	if (next_robot != NULL) {
		next_robot->set_priority(new_priority+2);
	};
};

void Robot::set_size_and_location(city_coordinate width,
											 city_coordinate height,
											 city_coordinate llx,
											 city_coordinate lly,
											 ChangeReason reason) {
	if (reason == TEMPORARY_FOR_DUMPING && ok_to_temporarily_reset_size()) { 
		// new on 051100 since if temporarily reset for dumping no need to go deeper than this
		return;
	};
   city_coordinate two_x_pixels = 2*tt_screen->one_pixel_in_x_city_units();
   city_coordinate two_y_pixels = 2*tt_screen->one_pixel_in_y_city_units();
   if (width < two_x_pixels) width = two_x_pixels;
   if (height < two_y_pixels) height = two_y_pixels;
//  if (thought_bubble == NULL || reason == ROBOT_ATTACHED_TO_THOUGHT_BUBBLE) {
	  Sprite::set_size_and_location(width,height,llx,lly,reason);
//  } else { // leave enough room for thought bubble too
//	  unsigned char shrink_to_fit = 255; // to avoid floats
//	  city_coordinate max_height = 10L*tile_height;
//	  if (height > max_height) {
//		  shrink_to_fit = (unsigned char) ((max_height*255)/height);
//     };
//	  Sprite::set_size_and_location((width*shrink_to_fit)/255,
//											  (height*shrink_to_fit)/255,
//											  llx,lly,reason);
//  };
	  // commented out the following on 151199 since you should be able to use size of robot in teams to indicate importance and the like
   // restored 221299 since sizes of robots aren't saved and this lead to really ugly teams of robots
   if (next_robot != NULL && reason != TO_FIT_INSIDE && reason != LEADER_CHANGED && reason != TEMPORARY_FOR_DUMPING) {  
		// 4th conjunct added 051100
      next_robot->set_size(width,height);
   };
 	// new on 010999 so size of dropped things is right when getting up or sitting down while robot is running
	if (reason == TRANSLATING_FLOOR_TO_ROOM_COORDINATES) {
		if (tool_width > 0 && tool_height > 0) {
			tt_programmer->translate_floor_to_room_dimensions(tool_width,tool_height);
		};
		if (copier != NULL && copier != tool) {
			tt_programmer->translate_from_floor_to_room(copier);
		};
		if (vacuum != NULL && vacuum != tool) {
			tt_programmer->translate_from_floor_to_room(vacuum);
		};
		if (expander != NULL && expander != tool) {
			tt_programmer->translate_from_floor_to_room(expander);
		};
	} else if (reason == TRANSLATING_ROOM_TO_FLOOR_COORDINATES) {
		if (tool_width > 0 && tool_height > 0) {
			tt_programmer->translate_room_to_floor_dimensions(tool_width,tool_height);
		};
		if (copier != NULL && copier != tool) {
			tt_programmer->translate_from_room_to_floor(copier);
		};
		if (vacuum != NULL && vacuum != tool) {
			tt_programmer->translate_from_room_to_floor(vacuum);
		};
		if (expander != NULL && expander != tool) {
			tt_programmer->translate_from_room_to_floor(expander);
		};
   };
};

void Robot::signal_size_and_location() {
	Sprite::signal_size_and_location();
	if (thought_bubble != NULL && showing_on_a_screen()) {
		// give it chance to improve its geometry
		thought_bubble->signal_size_and_location();
	};
	if (body == NULL && leader != NULL && tt_log_version_number >= 64) {
		set_parameter(ROBOT_INERT); // new on 161204 so untrained robots are inert when inside something else
	};
	fit_name_on_chest(FALSE,FALSE);
};

Sprite *Robot::select(TTRegion *region, SelectionReason reason, Sprite *in_hand) {
	if (first_in_line != NULL && (reason == MIGHT_GRASP || 
											(tt_log_version_number >= 49 && (reason == MIGHT_USE_VACUUM || reason == MIGHT_USE_COPIER))) 
		 // was just MIGHT_GRASP prior to 270704 -- added tt_log_version_number >= 49 on 180804
		 && first_in_line != this) { // new on 121103 since can happen if grab a team of robots and drop on itself
		Sprite *selection = first_in_line->select(region,reason,in_hand);
		initial_selection = this;
		return(selection);
	} else {
		if (reason == MIGHT_DROP_ITEM && !contained_in(CUBBY)) {
			switch (in_hand->kind_of()) {
				case CUBBY: // could be cute and figure out if it matched...
				case ROBOT:
					if (running_body_flag && current_parameter() != ROBOT_CONFUSED) {
						// added confusion check so confused robots can be reset by dropping something on them
						return(NULL);
					} else {
						return(this);
					};
				case COPIER:
					// new on 111104 to create initial tool for robot
					if (wand_copy_ok_as_initial_tool(in_hand)) {
						return(this);
					}; // otherwise fall through to below
				default:
					return(NULL);
			};
		};
		if (reason == MIGHT_GRASP) initial_selection = this;
		return(Sprite::select(region,reason,in_hand));
	};
};

Sprite *Robot::selected(SelectionReason reason, Sprite *by) {
	initial_selection = this;
	if (infinite_stack() || reason == VACUUM_USED_TO_BLANK || 
	    floor == NULL) { // e.g. in notebook
		return(Sprite::selected(reason,by));
	};
	Sprite *result;
	// following moved here on 201299 so that stop_team is working with up-to-date info (e.g. not on back of picture anymore)
	// this even caused ToonTalk to crash sometimes when taking a robot off back of picture and dropping on floor
	if (first_in_line != NULL && reason != EXPANDER_USED) {
		if (tt_log_version_number >= 49) {
			stop_team(reason); // in case team is running while being vacuumed (or copied?) -- new on 270704
		};
		return(first_in_line->selected(reason,by));
	} else {
		result = Sprite::selected(reason,by);
	};
	if (reason == GRASPED && tt_log_version_number >= 49) { // new on 270704
		while (add_to_end_of_line(0)) {
		};
		stop_team(reason);
		morph_to_inert_all();
		line_up(floor,this);
	} else if (reason != EXPANDER_USED && reason != COPIER_USED) { 
//	if (saved_next_robot != NULL) {
       // might be the 3rd or 4th in line has been moved out
		// moved here so this happens after the above and NOT when using a tool the robot (following completed and confused running robots)
		millisecond duration;
		if (reason == VACUUM_USED && tt_log_version_number >= 49) {
			duration = 0; // new on 270704 to not get confused with both this and vacuum animation happening together
		} else {
			duration = default_duration();
		};
		add_to_end_of_line(duration); // really a good idea?? only when being watched??
		// added duration on 010503
//	};
		stop_team(reason);
	};
//	selection_status = SELECTION_OK;
	if (reason == VACUUM_USED) { // only way to break them apart
		if (next_robot != NULL) {
//       if (first_in_line == NULL) { // I was first
				next_robot->set_first_in_line(NULL);
//       };
			if (next_robot->pointer_to_leader() == this || tt_log_version_number < 49) { // condition new on 270704
				remove_follower(next_robot);
				next_robot->set_priority_fixed(FALSE);
				floor->add_item(next_robot,FALSE);
			};
			if (next_robot->pointer_to_next_robot() != NULL && first_in_line == NULL) { 
				// new on 150200 to update who is first in line
				next_robot->pointer_to_next_robot()->reset_first_in_line(next_robot);
			};
			next_robot = NULL;
		};
		if (first_in_line != NULL) { // can this still happen??
			Background *saved_floor = floor;
			first_in_line->detach(this,floor);
			saved_floor->add_item(this,FALSE);
			first_in_line = NULL;
		};
		if (saved_next_robot != NULL) {
			saved_next_robot->destroy(this); // new on 180201
			saved_next_robot = NULL;
		};
		if (leader != NULL && leader->kind_of() == PICTURE) {
			removed_item_from_flip_side();
		};
//		return(Sprite::selected(reason,by));
	};
	return(result);
};

boolean Robot::wand_copy_ok_as_initial_tool(Sprite *item) { // abstracted on 11104
	return(!(tt_toolbox != NULL && toolbox_tool(item)) &&
            initial_tool == NULL &&
            thought_bubble != NULL &&
            thought_bubble->pointer_to_cubby() == NULL);
};

/*
Sprite *Robot::appearance_before_vacuuming() {
	// could be more clever if team was broken up rather than cubby sucked from thought bubble
	return(thought_bubble->appearance_before_vacuuming());
};

void Robot::reset_appearance_before_vacuuming() {
	thought_bubble->reset_appearance_before_vacuuming();
};
*/

MatchStatus Robot::match(Robot *other_robot, int version) {
	// I wonder if next_robot should match as well? asked 140805
	if (blank || // either blank or sorta blank
		 (body == NULL && thought_bubble->pointer_to_cubby() == NULL)) { // "blank"
      return(MATCH_GOOD);
   };
   if (serial_number() == other_robot->serial_number()) {
      // matches only the "same" robot -- no testing for program equivalence here!
		return(MATCH_GOOD);
	} else {
		return(MATCH_BAD);
	};
};

void Robot::set_thought_bubble(Thought_Bubble *new_thought_bubble) {
	if (thought_bubble != NULL) {
		remove_follower(thought_bubble);
//		thought_bubble->remove_from_floor(FALSE); // commented out on 070105 since sprite::remove_follower now deals with this
	}; 
	thought_bubble = new_thought_bubble;
};

void Robot::detach(Robot *other, Background *floor) {
	if (next_robot == other) {
      next_robot->set_priority_fixed(FALSE);
		remove_follower(other);
		next_robot = NULL;
	} else if (next_robot != NULL) { // should always be
		next_robot->detach(other,floor);
	};
};

void Robot::associate_with_box(Cubby *cubby) {
   // new on 140599 to both simplify and also avoid a problem where with power off loading pictures
	// with robots on the back didn't work properly
	set_working_cubby(cubby);
	set_waiting_for_release(TRUE);
	suspended_flag = FALSE;
};

void Robot::set_working_cubby(Cubby *cubby) {
	if (cubby == top_cubby) return; // new on 170201 - seems to matter for robots that suck up their box since it is in the vacuum and working set...
	if (top_cubby != NULL) top_cubby->destroy(this); // typically just decrements ref count (new on 190100)
	top_cubby = cubby;
	if (top_cubby != NULL) top_cubby->increment_ref_count(this); // new on 190100
};

void Robot::try_clause(Cubby *cubby, millisecond delay) {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Debug target trying a clause."));
		if (tt_debug_mode == 180405 && tt_running_robots != NULL 
			 && (tt_running_robots->member_cons(this) != NULL || tt_running_robots->member_cons(pointer_to_first_in_line()) != NULL)) {
			debug_this();
		};
	};
   if (floor == NULL && !suspended()) {
      log(_T("Robot wants to run but ToonTalk lost track of where it is."));
   }; 
#endif
	if (body == NULL) return; // wouldn't do anything if it could 
	if (floor == NULL) return; // new on 090400 since more robust this way - could be about to be deleted but queue is holding on to it just now
	if (cubby != NULL) {
		set_working_cubby(cubby);
	};
	if (top_cubby == NULL) {
#if TT_DEBUG_ON
		debug_this();
#endif
		stop_team();
		return; // warn, complain, be confused?? -- isn't this normal if a robot vacuums up 'My Box'? asked 160405
	};
	// second arg changed to TRUE on 141299 since less wasteful
	if (!active()) set_active(TRUE,FALSE); 
	// added on 111299 since robots otherwise don't animate very well 
	// if they are inactive because taken from back of inactive picture
	if (tt_city->stopped()) { // && tt_log_version_number > 14) {
		// new on 190499 - this can happen if a bird delivers something while the power is off
		if (insert_at_end_if_not_member(this,tt_running_robots)) {
//			increment_ref_count(); // new on 310100
			// commented out tt_running_robot ref count stuff on 090400 since causes robots to not be deleted and to run (as orphans)
			// and when robot is prepared for deletion it removes itself from tt_running_robot
		};
		return;
	};
	millisecond new_delay = default_duration();
	if (delay > 0) {
		if (new_delay > 0 && delay != 1) { // delay of 1 means til next frame
			do_after(delay,this,TRY_CLAUSE_CALLBACK);
      } else { // delaying but off screen
#if TT_DEBUG_ON
			if (tt_queue != NULL && tt_queue->robot_already_in_queue(this)) {
				debug_this(); // if I can eliminate all occurences then can avoid the waste of checking !tt_queue->robot_already_in_queue(this)
			};
#endif
         if (my_random(100) == 0) { // new on 090699 - what about a membership check? tt_log_version_number > 14 && 
				// this is a fairer scheduler since who gets to go first alternates
				// my_random(2) added on 110699 to be more fair
				// updated to my_random(100) so it rarely gets to jump ahead in line (e.g. Ross's Space Invaders)
				if (tt_running_robots == NULL || tt_running_robots->member_cons(this) == NULL) // condition new on 241003 to avoid duplicates
				tt_running_robots = new Sprites(this,tt_running_robots);
//				increment_ref_count();
			  // commented out tt_running_robot ref count stuff on 090400 since causes robots to not be deleted and to run (as orphans)
			  // and when robot is prepared for deletion it removes itself from tt_running_robot
#if TT_DEBUG_ON
			  if (debug_counter == tt_debug_target) {
				  log(_T("Debug target body added front of running robots."));
			  };
			  if (tt_debug_mode == 51201) {
				  tt_error_file() << "Robot#" << debug_counter << " added to beginning of running robots." << endl;
			  };
#endif
			} else {
				if (insert_at_end_if_not_member(this,tt_running_robots)) {
//					increment_ref_count(); // new on 310100
					// commented out tt_running_robot ref count stuff on 090400 since causes robots to not be deleted and to run (as orphans)
					// and when robot is prepared for deletion it removes itself from tt_running_robot
				};
#if TT_DEBUG_ON
			  if (debug_counter == tt_debug_target) {
				  log(_T("Debug target body added end of running robots."));
			  };
			  if (tt_debug_mode == 51201) {
				  tt_error_file() << "Robot#" << debug_counter << " added to end of running robots." << endl;
			  };
#endif
			};
      };
		return;
	};
	if (top_cubby != NULL && top_cubby->is_busy()) {
		top_cubby->run_when_non_busy(this,TRY_CLAUSE_CALLBACK);
		// wait a sec and try again
//		do_after(1000,this,try_clause_callback);
		return;
	};
	if (new_delay != 0 && !contained_in(PICTURE)) {
		 // don't bother if not on screen or on back of picture
//		 && !full_region()->overlaps(cubby->full_region())
		// not overlapping so move to cubby and then proceed
		//city_coordinate *cubby_llx_pointer;
		//city_coordinate *cubby_lly_pointer;
		//top_cubby->pointers_to_lower_left_corner(cubby_llx_pointer,cubby_lly_pointer);
      city_coordinate cubby_llx;
		city_coordinate cubby_lly;
		top_cubby->lower_left_corner(cubby_llx,cubby_lly);
		if (floor->pointer_to_picture() == NULL) set_priority(1);
// taken care of in really_try_clause now
//		if (thought_bubble != NULL) { // is it reasonable for unconditional robots
//			thought_bubble->set_alive(TRUE);
//		};
		// head to center top of cubby
      city_coordinate cubby_width = top_cubby->current_width();
      city_coordinate cubby_height = top_cubby->current_height();
      city_coordinate max_height;
//      if (tt_log_version_number > 11) {
         max_height = 17*tile_height;  // lowered it so less likely to float off the top
//      } else {
//         max_height = 19*tile_height;
//      };
		if (last_failure_was_for_unstable_reasons()) {
			// new on 301000 so don't delay since might be waiting for sensor to change...
			really_try_clause();
			if (top_cubby != NULL) {
				move_to_waiting_position(top_cubby);
			};
		} else {
			set_parameter(ROBOT_WORKING);
			if (cubby_lly+cubby_height > max_height) { // off the top or nearly so
//          tt_log_version_number > 8) {
				animate_location_to(top_cubby, // cubby_llx_pointer,cubby_lly_pointer,
									     750,
									     this,REALLY_TRY_CLAUSE_CALLBACK,
									     cubby_width/2,-cubby_height); // a bit below the cubby so it can be seen
			} else {
				animate_location_to(top_cubby, // cubby_llx_pointer,cubby_lly_pointer,
									     750,
									     this,REALLY_TRY_CLAUSE_CALLBACK,
									     cubby_width/2,cubby_height);
			};
		};
//		postpone_new_logs(); // new on 190100
	} else {
		really_try_clause();
	};
};

void Robot::really_try_clause() {
#if TT_DEBUG_ON
	if (tt_debug_mode == 70900) {
		character robot_name[max_resource_string_length];
		name(robot_name);
		if (strnicmp(robot_name,debug_robot_name,strlen(debug_robot_name)) == 0) {
			tt_error_file() << "Trying robot" << "#" << debug_counter << " who needs debugging." << endl;
		};
	};
	if (top_cubby != NULL && top_cubby->debug_counter == tt_debug_target) {
		tt_error_file() << "Robot trying to match debug target." << endl;
//		top_cubby->describe(tt_error_file(),DEFINITE_ARTICLE); // need to debug this!!!!
		top_cubby->print(tt_error_file());
		character name_string[max_resource_string_length];
		name(name_string);
		tt_error_file() << " given to " << name_string << endl;
	};
	if (debug_counter == tt_debug_target) {
		log(_T("Debug target really trying a clause."));
	};
	if (tt_debug_mode == 101 || tt_debug_mode == 190702) {
		character robot_name[max_resource_string_length];
		name(robot_name);
		tt_error_file() << "Trying robot " << robot_name;
      if (tt_debug_mode == 190702) {
         tt_error_file() << "#" << debug_counter; // - made it hard to do file comparisons
      };
      tt_error_file() << endl; 
	};
#endif
	if (top_cubby == NULL) {
		// prior to 090203 this also added to tt_running_robots as below
		return;
	};
	if (tt_city->stopped()) {
		// tt_city->stopped() added on 150200
		if (insert_at_end_if_not_member(this,tt_running_robots)) { // copied from :try_clause on 280300
//			increment_ref_count();
			// commented out tt_running_robot ref count stuff on 090400 since causes robots to not be deleted and to run (as orphans)
			// and when robot is prepared for deletion it removes itself from tt_running_robot
		};
		return;
	};
	if (pointer_to_background() != top_cubby->pointer_to_background() &&
		 top_cubby->pointer_to_background() != NULL) { 
		// added second conjunct on 050399 since in new scheme robot's background might not be NULL 
		// when flipped and cubby dropped on it
		//leader != NULL && cubby->pointer_to_leader() != ultimate_leader()) {
		// cubby no longer on flip side of picture though robot is
		// use ultimate leader in case robot's direct leader is another robot
		stop_team();
		return;
	};
	if (suspended() ||
		 top_cubby->inside_vacuum() ||
		 current_held_status() != NOT_HELD ||
       floor == NULL) { // does this still happen when on backside of picture??
		if (tt_log_version_number < 22) { // new on 290300 since this waiting for release stuff is now obsolete
			set_waiting_for_release(TRUE);
			suspended_flag = FALSE;
		};
	  return;
	};
	if (tt_log_version_number < 22) { 
		// following is obsolete
		//	picture inside a non-picture so stop until picture released
		 if (leader != NULL && leader->kind_of() == PICTURE &&
			  leader->pointer_to_leader() != NULL &&
			  leader->pointer_to_leader()->kind_of() != PICTURE) {
			set_waiting_for_release(TRUE);
			suspended_flag = FALSE;
			return;
		 };
	} else {
		// new version of this is to return if on back of flipped picture which is contained in a cubby (290300)
		// note that it probably better to suspend these pictures when put in a box - but how to make it "sticky"
		Picture *picture = floor->pointer_to_picture();
		if (picture != NULL) { 
			if (picture->is_flipped() && picture->pointer_to_leader() != NULL && 
				 picture->pointer_to_leader()->kind_of() != PICTURE) return;
			if (!picture->active()) return; // new on 131200 to stop robots from running on inactive pictures
			// following is new on 290500 so decoration doesn't run if not visible
			Sprite *ultimate_picture = picture->ultimate_container();
			if (!ultimate_picture->held()) { 
				// condition new on 050600 since puzzles at least expect things to run while held and standing
				//House *house = ultimate_picture->pointer_to_house(); // commented out since this always computed NULL prior to 020405
				//if (house == NULL) {
					Background *ultimate_background = ultimate_picture->pointer_to_background();
					if (ultimate_background == NULL) {
						if (ultimate_picture->pointer_to_leader() == NULL) { // how could it be otherwise??
							return; 
							// commented out on 070900  -- 
							// restored on 131100 since lots of junk accumulates there that doesn't need to run
						};
					} else {
						if (ultimate_background->pointer_to_wall_decoration_sprite() == ultimate_picture) { 
							// is wall decoration so only run if Programmer is standing in this room
							if (tt_screen->pointer_to_background() != ultimate_background) {
								return;
							};
						};
					};
				//} else {
				//	if (!house->visible()) {
				//		return;
				//	};
				//};
#if TT_DEBUG_ON
				if (tt_debug_mode == 300500) {
					string name_string = return_a_robot_named(0,0);
					tt_error_file() << name_string << "#" << debug_counter << " (" << timeGetTime() << " ms)" << endl; 
					tt_error_file() << " is running on the back of a picture ultimately contained in ";
					delete [] name_string;
					tt_error_file() << endl << " ";
					ultimate_picture->describe_briefly(tt_error_file());
					tt_error_file() << "(" << ultimate_picture->debug_counter << ")" << endl;
//					if (house == NULL) {
//						Background *ultimate_background = ultimate_picture->pointer_to_background();
//						if (ultimate_background != NULL) {
							//Sprite *wall_sprite = ultimate_background->pointer_to_wall_decoration_sprite();
							//if (wall_sprite != NULL) {
							//	house = wall_sprite->pointer_to_house();
							//};
							//if (house == NULL) {
//								house = ultimate_background->pointer_to_house();
//							};
//						};
//					};
					//if (house != NULL) {
					//	Block *block = house->block_im_on();
					//	block_number block_x, block_y;
					//	block->location(block_x, block_y);
					//	tt_error_file() << " At " << (int) block_x << " and " << (int) block_y 
					//					<< " where it is " << block->lot_index(house) << " from the left side of the block." << endl;
					//};
				};
#endif
			};
		} else { // new on 160101
			if (leader != NULL && leader->kind_of() == PICTURE && leader->is_flipped()) { // but I'm on the floor
				return;
			};
		};
	};
//	top_cubby->increment_busy_counter(); // lock this from others
//	if (screen != NULL) saved_screen = screen;
	tt_suspensions->remove_if_robot_is(this);
	// following commented out since tt_suspensions no longer keeps ref counts
//	if (prepared_for_deletion()) return; // new on 090400 since can be that last reference to this guy was a suspension record
	millisecond duration = default_duration();
//	if (duration > 0 && tt_log_version_number < 15) set_parameter(ROBOT_WORKING);
	if (next_robot != NULL) {// && duration > 0) { // no need if not being watched
		// first temporarilly disconnect
		//saved_next_robot = next_robot;
		//saved_next_robot->increment_ref_count(this); // new on 180201 since decremented when reset
		set_saved_next_robot(next_robot); // rewrote the above on 230105
		if (duration > 0) { // conditional new on 280403
			next_robot->set_priority_fixed(FALSE);
			if (next_robot->pointer_to_leader() == this) { // new on 121102 to avoid spurious warnings
				remove_follower(next_robot);
			};
			floor->add_item(next_robot,TRUE,FALSE,FALSE); // don't warn if already there
//      next_robot->set_screen_updates(TRUE);
			next_robot->morph_to_inert();
			next_robot->update_display(); // might have been called by screen update
		};
		next_robot = NULL;
	};
	SpritePointerPointer suspension_cell;
	MatchStatus match_status;
	if (first_in_line == NULL) {
		// even if a robot doesn't touch remotes if those ahead of it in line
		// do then it should still be scheduled as if it touched them too
		tt_touched_a_remote = NEVER_CHANGES; // set the default
		tt_touched_a_remote_that_can_change_to_match_failure = FALSE;
	};
//	set_touched_a_remote(FALSE);
	if (thought_bubble != NULL && thought_bubble->pointer_to_cubby() != NULL) { 
		// is it reasonable for unconditional robots
//		if (duration > 0) thought_bubble->set_alive(TRUE);
		tt_cause_of_last_mismatch = NULL; // new on 020300 so if mismatch can decide whether to suspend
      Cubby *cubby = thought_bubble->pointer_to_cubby();
      if (duration > 0) { // new on 250302
         cubby->set_cause_of_match_failure(FALSE,TRUE);
      };
		match_status = cubby->match(top_cubby,suspension_cell,version); // version new on 120504
	} else {
		match_status = MATCH_GOOD;
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 190499 || (tt_debug_target == debug_counter && tt_debug_mode != 261000)) {
		debug_log(debug_counter);
		tt_error_file() << "Match status is " << (int) match_status << " for ";
		thought_bubble->pointer_to_cubby()->top_level_describe(tt_error_file());
		tt_error_file() << endl << "with ";
		top_cubby->top_level_describe(tt_error_file()); // top_level_ added on 130200 otherwise can stop at the wrong time
		tt_error_file() << " frame=" << tt_frame_number << endl;
	} else if (tt_debug_mode == 101) {
		tt_error_file() << "Match status is " << (int) match_status 
							 << " and touched a remote is " << (int) tt_touched_a_remote << endl;
	};
	//if (tt_debug_mode == 170405) {
	//	Picture *picture = floor->pointer_to_picture();
	//	if (picture != NULL && picture->debug_counter == 167) {
	//		debug_this();
	//	};
	//};
#endif
   if (first_in_line == NULL) { // I'm first
		set_first_to_fail_for_unstable_reasons(NULL); // reset this
	};
	set_last_failure_was_for_unstable_reasons(FALSE); // new on 301000 unless proven otherwise below
	switch (match_status) {
	  case MATCH_GOOD:
#if TT_DEBUG_ON
		  //if (tt_debug_mode == 170405) {
			 // Picture *picture = floor->pointer_to_picture();
			 // if (picture != NULL && picture->debug_counter == 167) {
				//  debug_this();
			 // };
		  //};
        if (tt_debug_mode == 1333) {
           log(_T("Match good so working set is now zero."),FALSE,TRUE);
        };
		  debug_log(debug_counter,"Debug target just matched.");
		  if (tt_debug_mode == 70900) {
				character robot_name[max_resource_string_length];
				name(robot_name);
				if (strnicmp(robot_name,debug_robot_name,strlen(debug_robot_name)) == 0) {
					log("Debug robot just matched.");
				};
		  };
		  if (tt_queue != NULL && tt_queue->robot_already_in_queue(this)) {
			  debug_this();
			  if (debug_counter == tt_debug_target) { 
				  log(_T("Debug target just matched but is already in the queue"));
			  };
//			  return; // to avoid problems for now - come back to this - 030603 -- 
			  // was commented out at some point since this is only while debugging
		  };
		  if (tt_running_robots != NULL && 
			   (tt_running_robots->member_cons(this) != NULL || tt_running_robots->member_cons(pointer_to_first_in_line()) != NULL)) {
			  debug_this();
		  };
#endif
		  /// moved here from above on 120499 so the robot looks like he's working only if match is good
		  if (duration > 0) set_parameter(ROBOT_WORKING); //  && tt_log_version_number > 14
		  release_working_set_elements();  // to start running body
		  if (tt_log_version_number >= 73) {
			  set_team_running(TRUE); // moved here on 190405 so TRUE while events are queued rather than before first event started
		  };
		  if (tt_queue == NULL) {
#if TT_DEBUG_ON
			  debug_log(debug_counter,"Debug target body added to empty queue.");
#endif
			  tt_queue = new Queue(body,this,tt_queue);
		  } else if (duration > 0) { // give priority to stuff being watched
			  tt_queue->insert_second(new Queue(body,this));
#if TT_DEBUG_ON
			  debug_log(debug_counter,"Debug target body added to second position in queue.");
#endif
		  } else {
			  tt_queue->insert_at_end(new Queue(body,this));
#if TT_DEBUG_ON
			  debug_log(debug_counter,"Debug target body added to end of queue.");
#endif
		  };
		  running_body_flag = TRUE;
		  run_again_this_cycle = (flag) (tt_touched_a_remote != ONLY_ON_NEW_CYCLE_IF_SUCCEEDS);
		  // was ONLY_ON_NEW_CYCLE prior to 200902
//        run_again_this_cycle = TRUE; // experiment on 061102
        if (saved_next_robot != NULL && !saved_next_robot->on_back_of_picture()) { 
			  // second condition added on 051100 since no need to move robot if on back of picture
			  saved_next_robot->animate_location(12*tile_width+my_random(tile_width),
															 tile_height+my_random(tile_height),
															 duration);
//         tt_error_file() << "saved robot moving to side " << tt_current_time << endl;
		  };
		  if (run_again_this_cycle && tt_touched_a_remote == ALWAYS_CHANGES) {
			  // new on 030105 so that random numbers are not the same value when a team iterates
			  top_cubby->note_that_team_is_about_to_run_again();
		  };
		  return;
	  case MATCH_SUSPENDED_ON_NEST: 
		  if (suspension_cell != NULL) {
			  Robot *unstable_robot = first_team_member_to_fail_for_unstable_reasons();
			  if (tt_touched_a_remote_that_can_change_to_match_failure && unstable_robot == NULL) {
				  // new on 291004 since while suspended it may fail (see issue 568)
				  unstable_robot = this;
			  };		  
			  if (unstable_robot == NULL) {
				  // moved into this branch on 180804 since no need to do this when unstable_robot is non-null
				  SpritePointer dereferenced_suspension_cell = (*suspension_cell)->dereference(); // in case nest containing nest
				  if (dereferenced_suspension_cell->kind_of() != NEST) { // somehow something arrived in the meanwhile -- new on 180804
					  // issue 542 -- I turned off an image that was receiving birds and flipped it. Then I un-flipped and TT crashed.
					  dereferenced_suspension_cell = *suspension_cell;
					  if (dereferenced_suspension_cell->kind_of() != NEST) { // and just to be paranoid
						  tt_error_file() << "Somehow a robot is waiting for a nest that isn't a nest." << endl;
						  break;
					  };
				  };
				  ((Nest *) dereferenced_suspension_cell)->when_non_empty_try(this,top_cubby); 
				  // cast should be safe now that I test above
				  suspended_on = dereferenced_suspension_cell; // for giving help
#if TT_POST3187
				  suspended_on->increment_ref_count(this); 
				  // new on 120707 with time travel on and running sentence generator demo can crash
#endif
				  move_to_waiting_position(top_cubby);
			  } else if (tt_queue == NULL || !tt_queue->robot_already_in_queue(unstable_robot)) {
				  // condition new on 291004 since don't want the same robot in the queue twice
				  unstable_robot->try_clause(top_cubby,1); // next frame
			  };
		  } else { // new on 080799
			  log("Robot suspended on nest but nest is missing.",FALSE,TRUE); // args added on 141100 since this is serious
		  };
		  break;
	  case MATCH_SUSPENDED_ON_EMPTY_CUBBY_HOLE: // move to top of cubby
     case MATCH_SUSPENDED_ON_TRUCK: // something missing from truck
		  // prior to 120399 team_leader() was "this" - but since the user can change anything the first of the team makes more sense
		  // commented out conditional on 140405 since if there isn't enough information then the team should suspend
//		  if (next_robot == NULL && saved_next_robot == NULL) { 
		  // new on 170703 since the other teammates may not care about this empty hole
		  {
			  Robot *unstable_robot = first_team_member_to_fail_for_unstable_reasons();
			  if (tt_touched_a_remote_that_can_change_to_match_failure && unstable_robot == NULL) {
				  // new on 291004 since while suspended it may fail (see issue 568)
				  unstable_robot = this;
			  };	
			  if (unstable_robot == NULL) { // conditional new on 131200
				  Robot *the_team_leader = team_leader();
				  if (tt_suspensions->remove_and_stop_if_different_robot_waiting_on_same_cell(the_team_leader,suspension_cell)) {
					  tt_suspensions->add(new Suspensions(the_team_leader,suspension_cell));
					  suspended_on = *suspension_cell; // for giving help
#if TT_POST3187
				     if (suspended_on != NULL) suspended_on->increment_ref_count(this); 
				     // new on 120707 with time travel on and running sentence generator demo can crash
#endif
					  move_to_waiting_position(top_cubby);
				  }; // otherwise already suspended on this cell				  
			  } else if (tt_queue == NULL || !tt_queue->robot_already_in_queue(unstable_robot)) {
				  // condition new on 291004 since don't want the same robot in the queue twice
				  // new on 131200
//				  last_robot_failed();
				  unstable_robot->try_clause(top_cubby,1); // next frame
			  };
			  break;
		  };
//		  }; // else fall through		  
	  case MATCH_BAD:
//		  top_cubby->decrement_busy_counter(); // unlock it
		  boolean unstable = (tt_touched_a_remote != NEVER_CHANGES && 
									 tt_touched_a_remote != ONLY_ON_NEW_CYCLE_IF_SUCCEEDS && // added CAN_CHANGE_ANYTIME test on 200902
									 tt_touched_a_remote != CAN_CHANGE_TO_MATCH_FAILURE);
		  if (unstable) {
			  if (tt_cause_of_last_mismatch != NULL && tt_cause_of_last_mismatch->change_frequency() == NEVER_CHANGES) {
				  // new on 030200 since doesn't matter if a remote was touched if mismatch didn't involve remotes
			     tt_touched_a_remote = NEVER_CHANGES;
				  tt_touched_a_remote_that_can_change_to_match_failure = FALSE;
			  };
			  if (first_team_member_to_fail_for_unstable_reasons() == NULL) {
				  set_first_team_member_to_fail_for_unstable_reasons(this);
			  };
		  };
		  set_last_failure_was_for_unstable_reasons(unstable); // new on 301000
		  if (next_robot != NULL) { // if not being watched
			  next_robot->try_clause(top_cubby);
		  } else if (saved_next_robot != NULL) {
//			  Robot *next_robot_to_run = saved_next_robot; 
			  // new on 070503 since depending upon duration next_robot will be updated immediately or soon
			  // commented out as experiment on 110704 -- 
			  // fixed a problem where when every team members fails to match the last one hangs around
			  //add_to_end_of_line(0); // experiment 110704 to do this immediately -- was duration; // duration added on 010503
//			  pretend_add_to_end_of_line_completed(); // added 110704 since the above called this
			  saved_next_robot->try_clause(top_cubby);
//			  saved_next_robot->completed(); // experiment 110704
		  } else {
			  // last one and if at least one of them touched a remote 
			  // so try again next frame
			  // could be smarter and only try with first to touch
			  // if tt_finish_all_animations is TRUE this would otherwise
			  // loop
			  last_robot_failed();
		  };
//		  thought_bubble->set_alive(FALSE);
		  if (tt_touched_a_remote == NEVER_CHANGES || tt_touched_a_remote == CAN_CHANGE_TO_MATCH_FAILURE ||
			   tt_touched_a_remote == ONLY_ON_NEW_CYCLE_IF_SUCCEEDS) { // disjunct added on 200902
			  move_to_side(); // conditional added on 120499
		  };
		  break;
	};
	if (floor != NULL && current_parameter() != robot_waiting_state && floor->pointer_to_picture() == NULL) {
      set_priority(floor->next_item_priority());
   };
};

void Robot::last_robot_failed() { // re-modularized on 131200
	if (leader != NULL && leader->kind_of() == PICTURE && (finishing_instantly() || leader->is_flipped())) { 	
		((Picture *) leader)->set_suspended(TRUE); // what else could it be?
//				  if (tt_log_version_number > 14) 
		suspend(); // until next cycle when picture::update_display will try again
		return;
	};
	//if ((tt_touched_a_remote != NEVER_CHANGES && tt_touched_a_remote != CAN_CHANGE_TO_MATCH_FAILURE 
			//&& tt_touched_a_remote != tt_touched_a_remote) || 
 //      first_team_member_to_fail_for_unstable_reasons() != NULL) {
	// the above tt_touched_a_remote != tt_touched_a_remote make no sense - besides found robot to try again twice
	Robot *robot_to_try_again = first_team_member_to_fail_for_unstable_reasons();
	if (robot_to_try_again != NULL) {
		// added && tt_touched_a_remote != tt_touched_a_remote on 200902
		// second disjunct added on 290300
//		if (tt_log_version_number > 14) { 
			// new on 120399 so when an unstable condition changes the appropriate robot tries again
//			team_leader()->pointer_to_first_to_fail_for_unstable_reasons()->try_clause(top_cubby,1); // next frame
			robot_to_try_again->try_clause(top_cubby,1); // next frame
			team_leader()->set_first_to_fail_for_unstable_reasons(NULL);
			// new on 010200 so NULL after use so dumping is more accurate
//			move_to_side(FALSE,FALSE);
			if (on_back_of_picture()) {
				add_to_end_of_line();  // added on 290399 to restore the team properly
			};
			return;
//				  } else {
//					  if (first_in_line == NULL) {
//						  try_clause(top_cubby,1); // next frame
//						  break;
//					  } else {
//						  first_in_line->try_clause(top_cubby,1);
//						  break;
//					  };
//				  };
	};
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Debug target failed to match and won't wait for anything to change.");
	};
	Cubby *saved_top_cubby = top_cubby; // for debugging since is reset below and then warning is generated
#endif
	set_working_cubby(NULL);
	if (tt_system_mode == PUZZLE) robot_failed(this);
	if (finishing_instantly()) {
      move_to_side(TRUE);  // instantly
      return;
   };
	if (!showing_on_a_screen() && team_leader()->pointer_to_first_to_fail_for_unstable_reasons() == NULL && 
		 tt_system_mode != PUZZLE) {
		// second conjunct added on 210300 to stop spurious messages
		// third conjunct added on 210101 to stop such messages when solving puzzles
		say_a_robot_named(IDS_ROBOT_HAS_NON_MATCHING_BOX,IDS_ROBOT_HAS_NON_MATCHING_BOX2,2);
   };
};

void Robot::say_a_robot_named(int before_id, int after_id, int importance) {
	// someday this should be enhanced to say where the robot is
	// which block or which picture...
	string message = return_a_robot_named(before_id,after_id);
	string alternative_spoken_message = NULL;
	if (alternative_language_spoken()) {
		UnwindProtect<boolean> set(tt_use_spoken_alternative,TRUE);
		alternative_spoken_message = return_a_robot_named(before_id,after_id);
	};
	tt_martian->say(message,importance,alternative_spoken_message);
#if TT_DEBUG_ON
	tt_error_file() << message << "  #" << debug_counter << endl;
#endif
	delete [] message;
	if (alternative_spoken_message != NULL) delete [] alternative_spoken_message;
};

string Robot::return_a_robot_named(int before_id, int after_id) {
	int max_length = 300;
	string before = S(before_id,""); // ,"" added on 081099
	string after = S(after_id,"");
	if (before != NULL) max_length += _tcslen(before);
	if (after != NULL) max_length += _tcslen(after);
	string message = new character[max_length]; 
   output_string_stream message_stream(message,max_length);
   character robot_name[max_resource_string_length];
   name(robot_name);
	if (before != NULL && before[0] != NULL) {
		message_stream << before << space() << S(IDS_A_ROBOT_NAMED);
	} else {
		message_stream << S_capitalize_first(IDS_A_ROBOT_NAMED);
	}
	message_stream << space() << robot_name;
	if (after != NULL && after[0] != NULL) message_stream << space() << after;
   message_stream.put('\0'); 
	return(message);
};

//boolean Robot::any_touched_a_remote() {
//	if (touched_a_remote()) return(TRUE);
//	if (next_robot != NULL) return(next_robot->any_touched_a_remote());
//	return(FALSE);
//};

void Robot::move_to_side(boolean instantly, boolean frustrated) {
	if (on_back_of_picture()) { //  && tt_log_version_number > 14
		add_to_end_of_line();
		return; // added on 160399 to restore the team properly
	};
	millisecond duration = default_duration();
	city_coordinate goal_x = my_random(tile_width)+drop_scroll_delta_x; // +drop_scroll_delta_x new on 050404
	city_coordinate goal_y = 2*tile_height+my_random(tile_height)+drop_scroll_delta_y;
	if (first_in_line == NULL) {
		// I'm first so move to lower right corner of screen	
      if (instantly || duration == 0) {
         move_to(goal_x,goal_y);
         if (frustrated) {
				morph_to_inert();
			};
      } else {
			AnimationCallBack callback = CALLBACK_NOTHING;
			if (frustrated) { // condition added on 290399
				set_parameter(ROBOT_FRUSTRATED); // commented out on 120499 because it looked stupid
				callback = MORPH_TO_INERT_CALLBACK;
			};
		   animate_location(goal_x,goal_y,duration,this,callback);
      };
//      tt_error_file() << "scheduling morph to inert at " << (tt_current_time+duration) << endl;
	} else {
//		finish_instantly(); // new on 110704 in case still waiting for something like splice_in_line_callback 
//	   first_in_line->add_to_end_of_line(duration); // duration added on 010503
		first_in_line->put_behind_me(this,duration); // rewritten on 110704 to work in more situations 
		if (frustrated) { // copied here on 110704
			morph_to_inert();
		};
	};
};

void Robot::move_to_waiting_position(Cubby *cubby) {
	if (on_back_of_picture()) return; //  && tt_log_version_number > 14
	millisecond duration = default_duration(500);
	set_parameter((short int) robot_waiting_state); // used to be if (duration > 0)
   city_coordinate new_llx, new_lly;
   cubby->lower_left_corner(new_llx,new_lly);
   new_llx += cubby->current_width()/2-width/3;
   new_lly += cubby->current_height()-3*height/4;
   animate_location(new_llx,new_lly,duration);
   set_priority(cubby->priority()+1); // wait under the box
	if (saved_next_robot != NULL) {
		duration = duration > 0?1000:0;
		saved_next_robot->animate_location(13*tile_width,tile_height,
													  duration,
													  saved_next_robot,MORPH_TO_INERT_CALLBACK);
	};
};

void Robot::update_priority() { 
	if (top_cubby != NULL && current_parameter() == robot_waiting_state && tt_log_version_number >= 56) {
		// new on 211004
		top_cubby->update_priority();
		set_priority(top_cubby->priority()+1); // wait under the box
	} else if (first_in_line == NULL) { // first in line will fix things for all the others
		Sprite::update_priority();
	};
};

boolean Robot::add_to_end_of_line(millisecond duration) { 
	// added duration arg on 010503 since otherwise can get next_robot still on floor after being put back in the team
	// returns boolean as of 270704 so can repeatedly call this until already in line
	if (saved_next_robot != NULL) {
	   // put_behind_me does this stuff now
//		next_robot = saved_next_robot;
//		saved_next_robot = NULL;
//		if (floor != NULL) {
//			floor->remove_item(next_robot,FALSE,FALSE);
//		};
		if (put_behind_me(saved_next_robot,duration)) { // conditional new on 080804
			if (saved_next_robot != NULL) { // condition new on 090804 since may have been "nullified" by put_behind_me
				saved_next_robot->destroy(this); // new on 180201
				saved_next_robot = NULL;
			};
		} else {
			return(FALSE); // new on 090804
		};
		return(TRUE);
	} else if (next_robot != NULL) {
		return(next_robot->add_to_end_of_line(duration));
	} else {
		completed(); // new on 050500 but seems to be needed for some teams
		return(FALSE); // no need
	};
};

void Robot::pretend_add_to_end_of_line_completed() { 
	// new on 110704 
	if (saved_next_robot != NULL) {
		saved_next_robot->pretend_add_to_end_of_line_completed();
	} else if (next_robot != NULL) {
		next_robot->pretend_add_to_end_of_line_completed();
	} else {
		completed();
	};
};

boolean Robot::any_teammate_running() { // new on 040304
	// this checks the entire team and if anyone is running returns TRUE
	return(team_leader()->next_teammate_running());
};

boolean Robot::next_teammate_running() {
	if (still_running() || animation_in_progress()) {
		return(TRUE);
	};
	Robot *next_in_line = pointer_to_next_robot();
	if (next_in_line == NULL) return(FALSE);
	return(next_in_line->next_teammate_running());
};

ReleaseStatus Robot::item_released_on_top(Sprite *item, Sprite *by, SpritePointer &recipient, int level) {
//	if (infinite_stack_flag && item->kind_of() == ROBOT &&
//		 !((Robot *) item)->fully_defined()) {
//		return(RELEASED_ON_ITS_STACK);
//	};
	if (tt_log_version_number >= 35 && any_teammate_running() && 
		 (floor == NULL || floor->pointer_to_picture() == NULL) && current_parameter() != ROBOT_CONFUSED) { 
		// new on 160204 so if a robot is running (or about to run) drops are ignored
		// prior to 040304 second condition was animation_in_progress()
		// on 060504 changed so don't ignore if dropped on robot on the back of a picture
		// made an exception for when the robot is confused on 281004
		return(RELEASE_IGNORED);
	};
   if (contained_in(CUBBY)) {
		return(Sprite::item_released_on_top(item,by,recipient,level));
   };
	switch (item->kind_of()) {
		case CUBBY:
			{
			if (infinite_stack()) {
				if (!tt_copy_top_of_stack) {
					by->action_aborted();
					return(RELEASE_IGNORED);
				};
				recipient = copy_top_of_stack(by);
				return(recipient->item_released_on_top(item,by,recipient,level));
			};
			// new policy on 6/1/98 that robots are re-trained if thought bubble is empty -- moved here on 111205
			boolean needs_to_be_trained = ((body == NULL || thought_bubble->pointer_to_cubby() == NULL) &&
													 // Following added on August 26 '98 so training when part of a team doesn't happen
													 next_robot == NULL && (first_in_line == NULL || first_in_line == this)); 
#if TT_NO_ROBOT_TRAINING
			if (!replaying() && (needs_to_be_trained || body == NULL) && tt_system_mode != PUZZLE) {
				// new on 081205 so only if replaying or giving a trained robot a box is this allowed -- 
				// as are user puzzles with robot training
				remove_call_for_marty(TRUE); // get rid of any old messages
				say(S(IDS_RUNTIME_VERSION_CANT_TRAIN_ROBOTS,
					   "Sorry, this is a version of ToonTalk only for running programs. New programs can only be created in the full version."),
					 10,FALSE);
				by->action_aborted();
				return(RELEASE_IGNORED);
			};
#endif
			if (by->kind_of() == ROBOT_IN_TRAINING && (leader == NULL || leader->kind_of() != PICTURE)) {
				// added conditions on 190902 so that can drop box on robot that is on the back of a picture while training
				// sound commented out because code may try again with what is underneath
				//	play_sound(PLOP_SOUND); // goofy mistake sound
				if (body == NULL) {
				   say(IDS_ROBOTS_CANT_TRAIN_ROBOTS,5,TRUE);
				} else {
					say(IDS_ROBOTS_CANT_GIVE_BOXES_TO_ROBOTS,5,TRUE);
				};
				by->action_aborted();
				return(RELEASE_IGNORED);
			};
			if (floor != NULL && floor->outside() && floor->pointer_to_picture() == NULL && leader == NULL) {
            by->abort_action(IDS_ROBOTS_ONLY_WORK_INSIDE_OF_HOUSES_ABORT);
            return(RELEASE_IGNORED);
         };
			if (first_in_line == NULL && leader != NULL && leader->kind_of() == TRUCK) {
				recipient = leader;
				return(leader->item_released_on_top(item,by,recipient,level-1));
			};
			if (first_in_line != NULL &&
				 first_in_line->pointer_to_leader() != NULL && 
				 first_in_line->pointer_to_leader()->kind_of() == TRUCK) {
				recipient = first_in_line->pointer_to_leader();
				// could be more than 2 levels back
				return(recipient->item_released_on_top(item,by,recipient,level-2));
			};
			if (first_in_line != NULL) {
				recipient = first_in_line;
				return(recipient->item_released_on_top(item,by,recipient,level-1));
			};
			if (contained_in(PICTURE)) {
				// don't do rest if cubby dropped on robot on back of picture
//				completed(action_status); // commented out on 190902 since will be done by caller
				if (level == 0 && tt_log_version_number >= 38) {
					// new on 010304 (after 3.53x)
					Picture *picture = NULL;
					if (floor != NULL) {
						picture = floor->pointer_to_picture();
					};
					if (picture == NULL && leader != NULL && leader->kind_of() == PICTURE) { // new on 040304
						picture = (Picture *) leader;
					};
					if (picture != NULL) {
						recipient = picture;
						return(picture->item_released_on_top(item,by,recipient,level-1)); // not clear if level is correct
					};
				};
				return(RELEASED_CUBBY_ON_ROBOT);
			};
			if (floor != NULL) item->now_on_floor(floor,this);
			if (thought_bubble != NULL) {
// already checked above by noticing that is a ROBOT_IN_TRAINING
//				if (tt_screen->inside_rule_body()) {
//					// can't define inside thought bubble -- 
//					advise("Sorry but you can't train a robot while training another robot. ");
//					return(RELEASE_IGNORED);
//				};
				if (needs_to_be_trained) { //body == NULL) {
					// so give it a copy of item
//					tt_copying_entire_thought_bubble = TRUE; // to "deactivate" copy
					// don't center cubby in thought bubble since will animate
					// moving into thought bubble
//					((Cubby *) item)->increment_busy_counter(); // new on 010904 so in case a robot is working on this box
					UnwindProtect<boolean> set(tt_copying_entire_thought_bubble,TRUE); 
					// this is a bit cleaner than what was there before 260204
				   UnwindProtect<boolean> set2(tt_dont_connect_things_while_copying,TRUE); 
					// new on 260204 - otherwise training a bird/nest pair causes problems
					UnwindProtect<boolean> set3(tt_copying_for_training,TRUE); // new on 080205
					thought_bubble->receive_cubby((Cubby *) (item->top_level_copy()),FALSE); 
					// was just copy() prior to 111200 but if circular (e.g. remote looks) then infinite recursion results
//					tt_copying_entire_thought_bubble = FALSE;
				};
			};
			if (needs_to_be_trained) {	
				user_did(ROBOT,ROBOT_GIVEN_CUBBY,by);
				return(RELEASED_CUBBY_ON_UNTRAINED_ROBOT);
			} else {
				reset_drop_scroll_deltas(); // generalized on 050404
				user_did(ROBOT,ROBOT_TRIED,by);
				if (tt_queue != NULL) { // new on 120205
					Robot *another_robot_using_item = tt_queue->robot_in_queue_with_cubby((Cubby *) item);
					if (another_robot_using_item != NULL) {
						tt_queue->remove_items_of_robot(another_robot_using_item);
						another_robot_using_item->stop_all(TRUE);
					};
				};
				try_clause((Cubby *) item);
				by->completed();
				return(RELEASED_CUBBY_ON_ROBOT);
			};
			};
		case ROBOT:
      case COPIER:
			if (receive_item(item,by,default_duration(),this,item)) {
				record_event(RELEASE_ITEM_ON,by,floor,this);
				return(RELEASE_HANDLED);
			} else {
				return(RELEASE_IGNORED);
			};
		case INTEGER:
		case PICTURE:
		case BIRD:
		case NEST:
			if (!infinite_stack() && !told_user(TOLD_ROBOTS_ARE_FUSSY)) {
				say(IDS_FUSSY_ROBOT_HELP1,2);
				say(IDS_FUSSY_ROBOT_HELP2,2);
            user_told(TOLD_ROBOTS_ARE_FUSSY);
			};
			break;
// obsolete
//		case THOUGHT_BUBBLE: {             
//			Thought_Bubble *bubble = (Thought_Bubble *) item;
//			if (body != NULL) { // e.g. thought bubble was vacuumed away
//				floor->remove_item(bubble,TRUE,FALSE); // leave it as screen layer
//				connect_to_thought_bubble(bubble,TRUE);
//				return(RELEASE_ACCEPTED);
//			} else if (bubble->pointer_to_cubby() != NULL) {
//				return(RELEASED_THOUGHT_BUBBLE_ON_ROBOT);
//			};
//			break;
//		  };	
	};
	// updated on 030805
   by->abort_action_if_robot(IDS_ROBOT_WOULD_NOT_ACCEPT,NONE_GIVEN,FALSE,3,
		                       "the robot gave a robot something it wouldn't accept");
	return(RELEASE_IGNORED);
};

boolean Robot::receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item) {
   if (inside_vacuum()) {
      by->abort_action(IDS_ROBOT_MISSING_RECIPIENT_ABORT);
      return(FALSE);
   };
	if (blank) {
		by->action_aborted(); // error message here??
		return(FALSE);
	};
	set_active(TRUE); 
	// added on 111299 since robots otherwise don't animate very well 
	// if they are inactive because taken from back of inactive picture
	switch (item->kind_of()) {
		case ROBOT: {
			Robot *robot_item = (Robot *) item;
			if (fully_defined() && robot_item->fully_defined() && !infinite_stack()) {
				if (robot_item->pointer_to_first_in_line() == this || pointer_to_first_in_line() == robot_item) {
					return(FALSE); // new on 111203 to ignore robots dropped on teammates
				};
				Background *items_floor = item->pointer_to_background();
				if (first_in_line == NULL) { // I am
					user_did(ROBOT,ROBOTS_LINED_UP,by);
				};
				if (duration > 0) { //  && tt_log_version_number > 12
					call_in_a_mouse(this,item,items_floor,by,FALSE,FALSE,duration,original_recipient,original_item);
				} else {
					set_to_future_value(item,original_recipient,by);			  
					by->completed();
				};
				return(TRUE);
			};
						};
			break;
      case COPIER:
         if (wand_copy_ok_as_initial_tool(item)) {
            add_initial_tool(item);  // was ,1000
            by->completed();
            return(TRUE); //RELEASE_ACCEPTED);
         } else { // ignore it since belongs to toolbox...
            // explain??
            by->completed();
            return(TRUE); // RELEASE_ACCEPTED);
         };
		default: // new on 030805 -- to generate an error message and stop the robot
			if (by != NULL) {
				by->abort_action_if_robot(IDS_ROBOT_WOULD_NOT_ACCEPT,NONE_GIVEN,FALSE,3,
												  "the robot gave a robot something it wouldn't accept");
			};
	};
	return(FALSE);
};

void Robot::set_to_future_value(Sprite *other, Sprite *original_recipient, Sprite *by) {
	Robot *robot_behind = (Robot *) other;
//				if (robot_behind->pointer_to_first_in_line() ==
//					 pointer_to_first_in_line()) {
					// not really released on anything but guy behind
					// is underneath me
//					return(RELEASE_IGNORED);
//				};
	tt_suspensions->remove_if_robot_is(robot_behind);
	tt_suspensions->remove_if_robot_is(this);
//				following since is not completely inside
//				tt_screen->add(robot_behind);
	put_behind_me(robot_behind,default_duration(750)); // quickly get to the end
	// added default_duration on 080705 since otherwise when a robot does this off screen it can get stuck until house is visited
	if (first_in_line == NULL) { // I am
		robot_behind->set_first_in_line(this);
	} else {
		robot_behind->set_first_in_line(first_in_line);
	};
   if (by != NULL) { // new on 180603
		by->completed(); 
	};
};

void Robot::add_initial_tool(Sprite *sprite) {
	if (sprite == NULL) return;
   set_initial_tool(sprite);
//	initial_tool->increment_ref_count(this); // so it isn't destroyed even if dropped
   add_completion(); // since add_to_chest in pick_up_and_store_tool "completes"
   pick_up_and_store_tool(sprite);
/*
   city_coordinate x_offset,y_offset,chest_width,chest_height;
   chest_size(chest_width,chest_height);
   tool->animate_width_maintain_proportionality(chest_width,duration);
//            tool->animate_size(chest_width,chest_height,duration);
   chest_offsets(x_offset,y_offset);
   tool->animate_location(llx+x_offset,lly+y_offset,duration);
   tool->set_priority(current_priority-1); // on top of robot
//	floor->add_item(tool,FALSE);
   add_completion(action_status); // since add_to_chest "completes"
   add_to_chest();
*/
};

void Robot::set_initial_tool(Sprite *tool) {
	if (initial_tool != NULL) initial_tool->destroy(); // drop reference (new 310100)
   initial_tool = tool;
	if (initial_tool != NULL) initial_tool->increment_ref_count();
};

boolean Robot::put_behind_me(Robot *robot_behind, millisecond duration) { 
	// beginning 080804 returns FALSE if a no-op
	finish_instantly(); // new on 110704 -- in case multiple team members are lining up
	if (robot_to_put_behind != NULL) { // new on 110704 -- in case multiple team members are lining up
		robot_to_put_behind->finish_instantly();
	};
	if (next_robot != NULL) { // && next_robot != robot_behind) {
		if (next_robot != robot_behind) { // condition moved here for robustness on 290704
			return(next_robot->put_behind_me(robot_behind,duration));
		} else { // else no-op
			action_completed();
			return(FALSE); // FALSE new on 080804
		};
	};
	if (saved_next_robot != NULL) { // new on 290704
		if (saved_next_robot != robot_behind) {
			return(saved_next_robot->put_behind_me(robot_behind,duration));
		} else { // else no-op
			//action_completed();
			//next_robot = saved_next_robot; // new on 090804 to restore team
//			saved_next_robot = NULL; // new on 090804 to restore team
			set_saved_next_robot(NULL); // rewrote the above on 230105 to maintain reference counts
//			return(FALSE); // FALSE new on 080804 -- commented out on 090804 so splice_in_line is called below
		}; 
	};
//	robot_to_put_behind = robot_behind; // moved earlier on 270704 so following can call splice_in_line
	set_robot_to_put_behind(robot_behind); // rewrote the above on 230105
	if (duration == 0) { // new on 280403 -e.g. on the back of a picture
//		next_robot = robot_behind;
		splice_in_line(); // updated 270704 from above
		if (next_robot->pointer_to_background() == NULL) { // new on 210104 but maybe better to fix the ultimate cause of this
			next_robot->set_background(floor,FALSE,FALSE);
		};
		action_completed();
		return(TRUE); 
	};
//	follower_to_add = robot_behind;
	city_coordinate behind_robot_llx, behind_robot_lly;
	location_behind_me(behind_robot_llx,behind_robot_lly);
	robot_behind->animate_location(behind_robot_llx,behind_robot_lly,
											 duration,this,SPLICE_IN_LINE_CALLBACK);
//	completed_after(duration,action_status,this);
   do_after(duration,this,ACTION_COMPLETED_CALLBACK); 
	// rewritten on 131102 -- removed on 171102 since old action_status was always NULL -- restored on 051202
	return(TRUE);
};

void Robot::splice_in_line() {
#if TT_DEBUG_ON
   debug_log(debug_counter,"splice_in_line of debug target.");
#endif
	if (next_robot == robot_to_put_behind) {
		return; // new on 110704
	} else if (next_robot == NULL) {
		next_robot = robot_to_put_behind; 
//		robot_to_put_behind = NULL; // new on 110704
	} else {
#if TT_DEBUG_ON
		say_error(_T("ToonTalk is mixed up! There is a robot that was supposed to be at the end of a line and it wasn't"));
#endif
		return;
	};
//	if (floor != NULL) { // typically called in got here via released on top
//		floor->remove_item(next_robot,FALSE,FALSE);
//	};
//	if (next_robot->pointer_to_background() != floor) { // somehow they are different - conditional added on 100400
		next_robot->remove_from_floor(FALSE,FALSE); 
//		if (floor != NULL) floor->add_item(next_robot); // new on 100400
//	};
	next_robot->set_priority(priority()+1); // under me too
   next_robot->set_priority_fixed(TRUE); // until broken apart
	Sprite *next_robot_leader = next_robot->pointer_to_leader();
	if (next_robot_leader != this || tt_log_version_number < 69) { 
		// condition is new on 300105 since this is both wasteful and the remove_follower can remove a robot from tt_running_robots
		// was <= 69 prior to 310105
		if (next_robot_leader != NULL) {
			// added on 090599 since sometimes the robot is a part of the back of a picture
			next_robot_leader->remove_follower(next_robot,FALSE); // FALSE added 191299 to avoid spurious warnings
		};
		add_follower(next_robot,FALSE,INSERT_BY_PRIORITY); // added INSERT_BY_PRIORITY on 260103
	};
	next_robot->morph_to_inert(); // new on 110704
};

void Robot::pointer_region(TTRegion &region) {
	true_region(region);
	city_coordinate full_width = region.max_x-region.min_x;
	city_coordinate full_height = region.max_y-region.min_y;
	region.min_x -= true_x_offset();
	region.max_x = region.min_x+full_width/10;
	region.min_y += (4*full_height)/5-true_y_offset();
	region.max_y = region.min_y+full_height/10;
//   region.min_x += full_width/4-true_x_offset();
//	region.max_x = region.min_x+full_width/10;
//	region.min_y += full_height/2-true_y_offset();
//	region.max_y = region.min_y+full_height/10;
};

void Robot::chest_size(city_coordinate &chest_width,
							  city_coordinate &chest_height) {
	// region on chest for displaying things
   if (tool == initial_tool && tool != NULL) {
   	chest_width = (3*width)/2;
	   chest_height = (3*height)/4;
   } else {
	   chest_width = width/2;
      if (chest_width < 2*tile_width) chest_width = 2*tile_width; //  && !tt_running_old_demos
	   chest_height = height/4;
   };
};	

void Robot::chest_offsets(city_coordinate &x_offset, city_coordinate &y_offset) {
	// offsets from lower left corner
   if (tool == initial_tool && tool != NULL) {
      x_offset = -width/2; // so it sticks out better
   } else {
      x_offset = 0;
   };
	y_offset = height/3;
};	

int Robot::add_to_working_set(Sprite *item, boolean might_already_be_there) {
#if TT_DEBUG_ON
   if (tt_debug_mode == 1333) {
		character name_string[256];
		name(name_string);
      tt_error_file() << "Adding #" << working_set_length << " ";
		item->print(tt_error_file());
		tt_error_file() << " to working set of " << name_string << "#" << debug_counter << endl;
   };
	debug_log(debug_counter,"Adding to working set item of debug target.");
#endif
	if (working_set == NULL) {
		working_set = new SpritePointer[max_working_set_size];
		for (int i = 0; i < max_working_set_size; i++) { // new on 280300
			working_set[i] = NULL;
		};
	} else if (might_already_be_there) {
      int i = 0;
		for (; i < working_set_length; i++) {
			if (working_set[i] == item) {
            return(i); // already there
         } else if (working_set[i] != NULL && item->kind_of() == PICTURE) { // new on 050301
				Picture *picture = (Picture *) item;
				if (picture->pointer_to_indirection() == working_set[i]) { // e.g. a text pad has been flipped over - 
					set_working_set_element(i,picture); // changed the identity of the old guy, sorta
#if TT_DEBUG_ON
					if (tt_debug_mode == 1333) {
						tt_error_file() << "Replacing item#" << i << " with picture that indirects to it." << endl;
					};
#endif
					return(i);
				};
			};
		};
      // following optimization has 2 problems:
      // breaks old code despite attempts at dealing with it
      // and introduces timing dependecies like with the body of append
      // where while training bird is hatched first while when run
      // nest is dropped in cubby first.
//#if TT_BETA
//      if (!(body_version_number < 6 && latest_version_number >= 6)) {
      // following breaks programs made in version 5
//#endif
//      i = 0;
//      for (; i < working_set_length; i++) {
//			if (working_set[i] == NULL) {
//            working_set[i] = item;
//            return(i); // replaced someone who was removed
//         };
//		};
//#if TT_BETA
//      };
//#endif
	};
#if TT_DEBUG_ON
   if (tt_debug_mode == 1333) {
      log(_T("Incrementing working set length."),FALSE,TRUE);
   };
#endif
	working_set_length++;
	if (working_set_length > max_working_set_size && kind_of() == ROBOT_IN_TRAINING) { // can grow it
		if (max_working_set_size == working_set_size_limit) { // except it already has gotten too big
			say(IDS_TOO_MANY_THINGS_FOR_ROBOT,5);
			working_set_length--; // throw away something -- better than crashing
#if TT_DEBUG_ON
         if (tt_debug_mode == 1333) {
            log(_T("Decrementing working set length since reached size limit."),FALSE,TRUE);
         };
#endif
		} else {
			int new_max_size = min(max(2,max_working_set_size*2),working_set_size_limit);
			Sprite **new_working_set = new SpritePointer[new_max_size];
			int i;
			for (i = 0; i < max_working_set_size; i++) {
				new_working_set[i] = working_set[i];
			};
			for (; i < new_max_size; i++) { // new on 280300
				new_working_set[i] = NULL;
			};
			delete [] working_set;
			working_set = new_working_set;
			max_working_set_size = new_max_size;
		};
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 99) {
		tt_error_file() << "Working set length is now " << working_set_length << endl;
	};
#endif
	if (working_set_length > max_working_set_size) {
		// this didn't used to checked for non-debugging version prior to Oct 7 '98
      if (floor != NULL && floor->pointer_to_room() != NULL &&
		    floor->pointer_to_room()->when_will_house_explode() == 0) { // not about to explode
		   say_error(IDS_ROBOT_LOST_TRACK_OF_THINGS,TRUE);
			character string_serial_number[max_resource_string_length];
			name(string_serial_number);
			tt_error_file() << S(IDS_ROBOT_PAGE) << " " << string_serial_number 
								 << " " << S(IDS_EXPECTED_TO_KEEP_TRACK) << " " 
								 << max_working_set_size << " " << S(IDS_THINGS) << "." << endl;
      };
//		working_set_length--;
//		return(-1);
		// new error recovery on 010799 is to grow the working_set
		// but this isn't likely to help much since references will be off
		int new_max_working_set_size = min(max(2,max_working_set_size*2),working_set_size_limit);
		Sprite **new_working_set = new SpritePointer[new_max_working_set_size];
		int i;
		for (i = 0; i < working_set_length-1; i++) { // -1 added on 261000 for a softer landing (e.g. no crashing)
			new_working_set[i] = working_set[i];
		};
		for (; i < new_max_working_set_size; i++) { // new on 280300
			new_working_set[i] = NULL;
		};
		delete [] working_set;
		working_set = new_working_set;
		max_working_set_size = new_max_working_set_size;
	};
	set_working_set_element(working_set_length-1,item);
//	working_set[working_set_length-1] = item;
//	item->increment_ref_count(this);
	return(working_set_length-1);
/*
	if (working_set == NULL) {
		working_set = new Sprites(item);
	} else {
		working_set->insert_at_end_if_not_member(item);
	};
*/
};

void Robot::set_max_working_set_size(int new_max_working_set_size) { // new on 300300
	release_working_set_elements(); // I think this right that it releases these even if the same size... (asked 040504)
	if (max_working_set_size == new_max_working_set_size) return;
	delete [] working_set;
	working_set = NULL; // it'll be re-created when needed
	max_working_set_size = new_max_working_set_size;
};

Sprite *Robot::tool_to_drop() {
	// added on 110699 to get what the robot would really drop
	if (tool->kind_of() == COPIER) {
		Sprite *attached = ((Copier *) tool)->attached_tool();
		if (attached == NULL) {
			return(tool); // new on 140603
		} else {
			return(attached);
		};
   } else {
		return(tool);
	};
};

void Robot::remove_tool_from_working_set(Sprite *item, Sprite *replacement) { // second arg new on 090701.	if (item == NULL) item = tool;
	if (working_set == NULL || item == NULL) return; //warn??
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Removing tool from working set item from debug target.");
	};
#endif
//	if (item->kind_of() == PROGRAM_PAD) {
//		tt_error_file() << "debug" << endl;
//	};
   if (item->kind_of() == COPIER) {
      Sprite *attached = ((Copier *) item)->attached_tool();
      if (attached != NULL) item = attached;
   };
	for (int i = 0; i < working_set_length; i++) {
		if (working_set[i] == item) {
			set_working_set_element(i,replacement);
			return;
		};
	}; // else warn that time was wasted but this called when item isn't in working set
};

void Robot::just_remove_tool_from_working_set(Sprite *item) {
	// new on 011105 as extra caution to avoid any crashes after training 
	// should be that ref_counts make this unncessary
   for (int i = 0; i < working_set_length; i++) {
		if (working_set[i] == item) {
			working_set[i] = NULL;
			return;
		};
	};
};

void Robot::release_working_set_elements() {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Releasing working set elements of debug target.");
	};
#endif
	for (int i = 0; i < working_set_length; i++) { // release old working set if haven't already
		set_working_set_element(i,NULL); // to release ref counts - new on 280300
	};
	working_set_length = 0;
};

void Robot::set_working_set_element(int i, Sprite *item) { // new on 280300 to be sure ref counting is done right
	if (working_set[i] != NULL) {
		working_set[i]->destroy(this); // release reference
		if (working_set == NULL) { // new on 121200 - as a result of the above "destroy"
			return;
		};
	};
	working_set[i] = item;
	if (item != NULL) {
		item->increment_ref_count(this);
	};
#if TT_DEBUG_ON
   if (tt_debug_mode == 1333) {
		character name_string[256];
		name(name_string);
      tt_error_file() << "Setting working set #" << i << " ";
		if (item == NULL) {
			tt_error_file() << "NULL";
		} else {
			item->print(tt_error_file());
		};
		tt_error_file() << " of " << name_string << "#" << debug_counter << endl;
   };
	if (tt_debug_target == debug_counter) {
		log("Adding to working set item of debug target.");
	};
#endif
};

void Robot::remove_last_working_set_item() {
	working_set_length--;
#if TT_DEBUG_ON
   if (tt_debug_mode == 1333) {
      log(_T("Decrementing working set length."),FALSE,TRUE);
   };
	debug_log(debug_counter,"Removing last working set item from debug target.");
#endif
	set_working_set_element(working_set_length,NULL);
//	working_set[working_set_length] = NULL;
};

// could inline this when not debuggin
Sprite *Robot::working_set_nth(int i) {
	if (i >= working_set_length) {
		// narrowed the debugging scope on 080499 so it doesn't crash if there is a bug
		tt_error_file() << "Internal ToonTalk problem. Working set index too big: " << i
							 << " when size is " <<	working_set_length << endl;
#if TT_DEBUG_ON
		say_error(_T("Robot is looking for something but messed up and forgot it. Sorry."));
#endif
		return(NULL);
	};
	return(working_set[i]); // assumes caller knows what its doing
};

Vacuum *Robot::pointer_to_vacuum(boolean &new_vacuum) {
	boolean initial_tool_ok = (initial_tool != NULL && initial_tool->kind_of() == VACUUM);
	new_vacuum = FALSE; // new on 010500 and copier and expander too
	if (!initial_tool_ok && kind_of() == ROBOT_IN_TRAINING && tt_toolbox != NULL) {
		return(tt_toolbox_vacuum);
	};
	if (vacuum == NULL) {
		if (initial_tool_ok) {
			vacuum = (Vacuum *) initial_tool;
		} else { 
		   vacuum = new Vacuum(llx,lly);
			new_vacuum = TRUE;
			// before May 19 '98 this used to happen regardless of whether it was new or not
			// scale so that width of vacuum is the width of the robot
			city_coordinate vacuum_width = vacuum->current_width();
			city_coordinate vacuum_height = vacuum->current_height();
			long factor = (1000*width)/vacuum_width;
			vacuum_width = (vacuum_width*factor)/1000;
			vacuum_height = (vacuum_height*factor)/1000;
			vacuum->set_size_and_location(vacuum_width,vacuum_height,
													llx-vacuum_width,lly+height/2);
		};
//		if (floor != NULL) floor->add_item(vacuum);
	};
	return(vacuum);
};

Copier *Robot::pointer_to_copier(boolean &new_copier) {
	boolean initial_tool_ok = (initial_tool != NULL && initial_tool->kind_of() == COPIER);
	new_copier = FALSE;
	if (!initial_tool_ok && kind_of() == ROBOT_IN_TRAINING && tt_toolbox != NULL) {
		return(tt_toolbox_copier);
	};
	if (copier == NULL) {
		if (initial_tool_ok) {
			return((Copier *) initial_tool);
		};
		copier = new Copier(llx,lly);
		new_copier = TRUE;
	};
	return(copier);
};

Expander *Robot::pointer_to_expander(boolean &new_expander) {
	boolean initial_tool_ok = (initial_tool != NULL && initial_tool->kind_of() == EXPANDER);
	new_expander = FALSE;
	if (!initial_tool_ok && kind_of() == ROBOT_IN_TRAINING && tt_toolbox != NULL) {
		return(tt_toolbox_expander);
	};
	if (expander == NULL) {
		if (initial_tool_ok) {
			return((Expander *) initial_tool);
		};
		expander = new Expander(llx,lly);
		new_expander = TRUE;
	};
	return(expander);
};

boolean Robot::grasp_copier() { 
	if (kind_of() == ROBOT_IN_TRAINING) {
		if (initial_tool != NULL && copier == initial_tool) { 
			record_event(GRASP_ITEM,this,floor,initial_tool);
		} else {
			return(FALSE); // fall back on general case
		};
	};
	boolean new_copier;
	tool = pointer_to_copier(new_copier);
   boolean showing = showing_on_a_screen();
	if (showing) tt_screen->add(tool);
	grasp_special_tool(showing || new_copier);
	return(TRUE);
};

boolean Robot::grasp_vacuum() { 
	if (kind_of() == ROBOT_IN_TRAINING) {
		if (initial_tool != NULL && vacuum == initial_tool) { 
			record_event(GRASP_ITEM,this,floor,initial_tool);
		} else {
			return(FALSE); // fall back on general case
		};
	};
	boolean showing = showing_on_a_screen();
	if (tool != NULL) { 
		// already holding something but it is special -- prior to 150303 tool == initial_tool was here but is moved down now
		// should only happen when cleaning up at the end
		if (tool->pointer_to_leader() == this) {
			remove_follower(tool);
		};
		if (showing && tool == initial_tool) {
			tt_screen->remove(tool);
		} else {
			// should this be taken care of when picking up other tools too??
			tt_screen->add(tool); // new on 150303 in case there is a bug so at least recover more gracefully
			log("Warning. A robot is picking up a vacuum while still holding something. Dropping it first.");
		};
	};
	boolean new_vacuum; // new on 050100 so size is OK
	tool = pointer_to_vacuum(new_vacuum);
	if (showing) tt_screen->add(tool);
	grasp_special_tool(showing || new_vacuum);
	return(TRUE);
};

boolean Robot::grasp_expander() { 
	if (kind_of() == ROBOT_IN_TRAINING) {
		if (initial_tool != NULL && copier == initial_tool) { 
			record_event(GRASP_ITEM,this,floor,initial_tool);
		} else {
			return(FALSE); // fall back on general case
		};
	};
	boolean new_expander;
	tool = pointer_to_expander(new_expander);
	boolean showing = showing_on_a_screen();
	if (showing) tt_screen->add(tool);
	grasp_special_tool(showing || new_expander);
	return(TRUE);
};

void Robot::grasp_special_tool(boolean resize_to_fit) { // abstracted on 161202
	// tool should be vacuum, expander, or copier
	tool->increment_ref_count(this); // new on 011199
	if (showing_on_a_screen()) { // conditional new on 280403
		if (resize_to_fit && kind_of() != ROBOT_IN_TRAINING) resize_to_fit_chest(tool);
		tool->set_priority(current_priority-1); // on top of robot
	};
	add_to_chest();
};

void Robot::location_to_apply_tool(city_coordinate target_x, city_coordinate target_y,
                                   city_coordinate &new_llx, city_coordinate &new_lly) {
	// computes where the wand tip should go
	// or the number edge to reach the center of the target
	if (tool != NULL) {
		city_coordinate current_x, current_y;
		TTRegion tool_region;
		tool->active_region(tool_region);
		tool_region.region_center(current_x,current_y);
		new_llx = llx+target_x-current_x;
		new_lly = lly+target_y-current_y;
	} else {
		new_llx = llx;
		new_lly = lly;
	};
};

/* tries to find some free space -- didn't work too well
	if (saved_screen != NULL) {
		TTRegion scratch_region(region->min_x,region->max_x,region->min_y,region->max_y);
		saved_screen->move_region_to_free_area(&scratch_region,10,this);
		if (scratch_region.overlaps(region)) {
			// gave up and didn't move far enough
			city_coordinate height_plus
			  = tile_height+scratch_region.max_x-scratch_region.min_x;
			if (scratch_region.min_x < tile_width*10) {
				scratch_region.min_x += height_plus;
				scratch_region.max_x += height_plus;
			} else {
				scratch_region.min_x -= height_plus;
				scratch_region.max_x -= height_plus;
			};
		};
		new_llx = scratch_region.min_x+(llx-region->min_x);
		new_lly = scratch_region.min_y+(lly-region->min_y);
//      new_llx = llx+(scratch_region.min_x-region->min_x);
//      new_lly = lly+(scratch_region.min_y-region->min_y);
	} else { // probably shouldn't be called
		new_llx = 10*tile_width;
		new_lly = 10*tile_height;
	};
*/

void Robot::adjust_release_coordinates(city_coordinate &subject_llx,
													city_coordinate &subject_lly) {
	// where to move to so release is over target
	if (tool == NULL) return;  // no need to adjust anything
   Sprite *item = pointer_to_ultimate_item(); // if copier has something attached then it
   city_coordinate item_llx,item_lly;
   item->lower_left_corner(item_llx,item_lly);
   subject_llx -= item_llx-llx;
   subject_lly -= item_lly-lly;
};
/*
	city_coordinate chest_x_offset, chest_y_offset;
	chest_offsets(chest_x_offset,chest_y_offset);
	subject_llx -= chest_x_offset;
	subject_lly -= chest_y_offset;
   if (tt_log_version_number > 3) { // don't want to break old demos
      if (tool->kind_of() == COPIER) {
         subject_llx += (llx-tool->current_llx());
         // undo above
         subject_llx += chest_x_offset;
//	      subject_lly += chest_y_offset;
      };
   };
*/

boolean Robot::holding(Sprite *item) {
	return(tool == item ||
          (tool == copier && copier != NULL && copier->attached_tool() == item));
};

void serial_number_to_name(unsigned short number, ascii_string name, boolean capitalize_first) {
   if (tt_robots_have_names) {
       const_ascii_string vowels = AS(IDS_VOWELS_FOR_ROBOT_NAMES);
       const_ascii_string consonants = AS(IDS_CONSONANTS_FOR_ROBOT_NAMES);
       unsigned short int vowel_count = (unsigned short int) strlen(vowels);
       unsigned short int consonant_count = (unsigned short int) strlen(consonants);
       int i = 0;
       boolean consonant_next = TRUE; // start with them
       unsigned short number_left = number;
       name[i++] = ' ';   // for the bad name test below
       // the following changes the number into alternating base-consonant_count and base-vowel_count
       // and it does the number from right to left instead
       while (number_left > 0) {
          if (consonant_next) {
             name[i++] = consonants[number_left%consonant_count];
//             boolean more_left = (number_left >= consonant_count);
             number_left /= consonant_count;
//             if (more_left && number_left == 0) name[i++] = vowels[0];
          } else {
             name[i++] = vowels[number_left%vowel_count];
//             boolean more_left = (number_left >= vowel_count);
             number_left /= vowel_count;
//             if (more_left && number_left == 0) name[i++] = consonants[0];
          };
          consonant_next = !consonant_next;
       };
       name[i++] = ' '; // for the bad name test below
       name[i] = NULL; // terminate string
       if (strstr(AS(IDS_BAD_ROBOT_NAMES),name) != NULL) {
          itoa(number,name,10); // use number instead
       } else {
          // remove spaces now
          i--;
          name[i] = NULL;
          for (int j = 0; j < i; j++) {
             name[j] = name[j+1];
          };
          if (capitalize_first) {
             char saved_second_char = name[1];
             name[1] = NULL;
             AnsiUpper(name);  // just first letter
             name[1] = saved_second_char; // restore
          };
       };
   } else {
      itoa(number,name,10);
   };
};

java_string Robot::java_name() {
#if TT_DEBUG_ON
	if (tt_debug_mode == 1411 && -tt_debug_target == serial_number()) {
		log("Debug robot java generation.");
	};
#endif
	// this works up to max_resource_string_length (400) long names
//   java_string name = (java_string) S(IDS_JAVA_ROBOT); // changes are "safe"
//   return(rest_of_java_name(name));
	return(S2(IDS_JAVA_ROBOT,java_index(this)));
};

java_string Robot::java_class_name() {
#if TT_DEBUG_ON
	if (tt_debug_mode == 1411 && -tt_debug_target == serial_number()) {
		log("Debug robot java generation.");
	};
#endif
	// this works up to max_resource_string_length (400) long names
   java_string name = (java_string) S(IDS_JAVA_ROBOT_CLASS); // changes are "safe"
	return(rest_of_java_name(name));   
};

java_string Robot::rest_of_java_name(java_string name) { 
	// re-modularized and re-written on 210700 since 
	// copies of the same robot (body) can have different names
	// so just use serial number derived name now
	int length = _tcslen(name);
   // changed from itoa to ultoa on 240702 since might be negative
	unsigned long number = serial_number();
	ultoa(number,name+length,10); // new on 140600 to avoid name conflicts with robots with the same name
   strcat(name,"_"); // to separate the serial number and the hash - new on 240702
   ultoa(body_hash(),name+_tcslen(name),10);
#if TT_DEBUG_ON
   if (tt_debug_mode == 230702) {
      character name_string[max_resource_string_length];
      Robot::name(name_string);
      tt_error_file() << "Java robot class name is " << name << " for " << name_string << endl;
//      copy_alphanumerics(name_string,name+strlen(name)); // experiment on 230702
   };
#endif
//	length = _tcslen(name);
//   name[length] = '_';
//	char ascii_name[max_resource_string_length];
//	string user_name = copy_string_name_from_user();
//	if (user_name != NULL && user_name[0] != NULL) {
//		copy_alphanumerics(u2a(user_name),ascii_name);
//		delete [] user_name;
//	} else {
//		serial_number_to_name(serial_number(),ascii_name,FALSE);
//	};
//	copy_ascii_to_unicode(ascii_name,name+length+1,max_resource_string_length-(length+1));
   return(name);
};

void Robot::name(string name) {
	string user_name = copy_string_name_from_user();
	if (user_name != NULL && user_name[0] != NULL) {
		_tcscpy(name,user_name); // name better be long enough
		delete [] user_name;
	} else {
		char ascii_name[32];
		serial_number_to_name(serial_number(),ascii_name,TRUE);
		_tcscpy(name,a2u(ascii_name));
	};
};

void Robot::display(SelectionFeedback selection_feedback,
						  boolean followers_too, TTRegion *region) {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Displaying debug target."));
	};
#endif
	if (pointer_to_top_of_stack() != NULL) { // new on 260500
		pointer_to_top_of_stack()->display(selection_feedback,followers_too,region);
		return;
	};
	if (name_from_user != NULL && name_from_user->pointer_to_leader() == NULL) { // new on 171202
		attach_name_from_user();
	};
	if (kind_of() == ROBOT_IN_TRAINING) {
		// this will include selection feedback
		tt_programmer->display_appearance(); // was programmer prior to 131202
	} else {
//		if (rescale_if_camera_above(selection_feedback,followers_too,region)) return; -- commented out on 160804
		if (name_from_user == NULL && body != NULL) {
			set_name_from_user(NULL); // uses serial number
		};
		Sprite::display(selection_feedback,followers_too,region);
/*
		if ((current_parameter() == ROBOT_INERT ||
          (current_parameter() == ROBOT_MORPH_TO_INERT && cycle_stopped())) &&
			 !blank &&
			 height > 2*tile_height)  {
			// big enough and not active (too hard to get name to show well)
			string robots_name = name_from_user;
			if (robots_name == NULL && body != NULL) {
				unsigned short number = serial_number();
				robots_name = new char[max_resource_string_length];
				serial_number_to_name(number,robots_name,TRUE);
			};
			if (robots_name != NULL) {
				city_coordinate name_llx = llx+(12*width)/100;
				city_coordinate name_lly = lly+(40*height)/100;
				if (want_feedback(selection_feedback)) {
					name_llx += selection_delta_x();
					name_lly += selection_delta_y();
				};
				int robot_name_length = strlen(robots_name);
				city_coordinate name_width, name_height;
				if (robot_name_length < 4) {
					name_width = height/12;
					name_height = (7*height)/40;
				} else if (robot_name_length < 7) {
					name_width = height/20;
					name_height = (7*height)/60;
				} else { // is this all that is needed?
					name_width = height/(3*robot_name_length);
					name_height = (7*height)/(10*robot_name_length);
				};
	BEGIN_GDI
				tt_screen->text_out(robots_name,robot_name_length,
										  name_llx,name_lly,
										  name_width,name_height,TRUE,FALSE,TRUE,tt_white);
	END_GDI
				if (name == NULL) delete [] robots_name; // just created from serial number
			};
	};
*/
   };
};

string Robot::steps_left_string() {
   if (training_counter != NO_TRAINING_COUNTER && 
       ((body == NULL && thought_bubble != NULL) || kind_of() == ROBOT_IN_TRAINING)) {
      string buffer = new character[20];
      output_string_stream stream(buffer,20);
      // leading blank so doesn't ")move" when going from 10 to 9
//      if (training_counter < 10) stream << ' ';
      if (training_counter == 1) {
         stream << (int) training_counter << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_STEP);
      } else {
         stream << (int) training_counter << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_STEPS);
      };
      stream.put((char) 0);
      return(buffer); // caller should delete it
   };
   return(NULL);
};

void Robot::abort_action_if_robot(//ActionStatus *action_status, 
											 // all callers are to "by" which is the "owner" of the action_status
                                  int reason,
											 SpriteType actor, boolean report_anyway,
                                  int importance, string default_string) {
	if (kind_of() != ROBOT_IN_TRAINING) {
		abort_action(reason,actor,importance,default_string); 
	} else {
		Sprite::abort_action_if_robot(reason,actor,report_anyway,importance,default_string);
	};
};

void Robot::abort_action(int reason, SpriteType actor, int importance, string default_string) { 
	// all callers are to "by" which is the "owner" of the action_status
	Sprite::abort_action(reason,actor,importance,default_string);
	if (kind_of() != ROBOT_IN_TRAINING) {
//		if (action_status != NULL) {
			abort_reason = reason;
//		};
		if (showing_on_a_screen()) play_sound(SHUTDOWN_SOUND); 
		stop_team();
		add_to_end_of_line(default_duration()); // new on 290403 -- duration added 010503
	};
   action_aborted(); // "moved" here from Sprite::abort_action on 171102
};

void Robot::set_in_notebook(Notebook *notebook, boolean recur) {
	// arg added on 010500 since that is the virtual declaration in Sprite
//   if (in_notebook_flag == flag) return; 
	// new on 170702 to ignore calls that already know about - 
	// commented out on 190702 since may have been set but not recursively earlier
	Sprite::set_in_notebook(notebook,recur);
	if (next_robot != NULL && recur) { // added recur conditon on 190702
		next_robot->set_in_notebook(notebook,recur);
	};
	//if (thought_bubble != NULL) { // new on 220103 since don't want thought bubble sticking out of pad
	//	thought_bubble->set_visible(notebook == NULL);
	//};
};

int Robot::page_number_offset(Notebook *notebook) {
	// since notebooks are shared robot needs to track page_offsets
	if (page_number_offsets == NULL) return(0);
	return(page_number_offsets->page_number_offset(notebook));
};

Page_Number_Offsets::~Page_Number_Offsets() {
	for (int i = 0; i < count; i++) {
		// releasing access so reset
		notebooks[i]->reset_page_number_offset();
	};
	if (notebooks != NULL) delete [] notebooks;
	if (offsets != NULL) delete [] offsets;
};

void Page_Number_Offsets::update(Notebook *notebook) {
   int i;
	for (i = 0; i < count; i++) {
		if (notebooks[i] == notebook) {
			offsets[i] = notebook->return_page_number_offset();
//			tt_error_file() << "debuggin page number offsets " << offsets[i] << " for #" << i << endl;
			return;
		};
	};
	NotebookPointer *new_notebooks = new NotebookPointer[count+1];
	int *new_offsets = new int[count+1];
	for (i = 0; i < count; i++) {
		new_notebooks[i] = notebooks[i];
		new_offsets[i] = offsets[i];
	};
	if (count > 0) {
		delete [] notebooks;
		delete [] offsets;
	};
	notebooks = new_notebooks;
	offsets = new_offsets;
	notebooks[count] = notebook;
	offsets[count] = notebook->return_page_number_offset();
//	tt_error_file() << "debuggin page number offsets " << offsets[count] << " for new #" << count << endl;
	count++;
};

#if TT_XML

Page_Number_Offsets::Page_Number_Offsets(int c) {
	count = c;
	notebooks = new NotebookPointer[count];
	offsets = new int[count];
	// values to be set by set_page_offset
};

void Page_Number_Offsets::set_page_offset(int i, int offset, Notebook *notebook) {
	notebooks[i] = notebook;
	offsets[i] = offset;
};

#endif

int Page_Number_Offsets::page_number_offset(Notebook *notebook) {
	for (int i = 0; i < count; i++) {
		if (notebooks[i] == notebook) {
			return(offsets[i]);
		};
	};
	return(0); // should it happen??
};

boolean Robot::is_action_in_progress_of_any_teammate() { // new on 180405 -- obsolete already?
	if (is_action_in_progress()) {
		return(TRUE);
	};
	if (next_robot != NULL) {
		return(next_robot->is_action_in_progress_of_any_teammate());
	};
	return(FALSE);
};

boolean Robot::is_any_teammate_still_running() {
	if (still_running()) {
		return(TRUE);
	};
	if (next_robot != NULL) {
		return(next_robot->is_any_teammate_still_running());
	};
	return(FALSE);
};

boolean Robot::any_teammate_suspended_on_something() { // new on 190405
	if (suspended_on_something()) {
		return(TRUE);
	};
	if (next_robot != NULL) {
		return(next_robot->any_teammate_suspended_on_something());
	};
	if (saved_next_robot != NULL) {
		return(saved_next_robot->any_teammate_suspended_on_something());
	};
	return(FALSE);
};

void Robot::morph_to_inert_all() {
	morph_to_inert();
	Robot *next = this;
	while ((next = next->pointer_to_next_robot()) != NULL) {
		next->morph_to_inert();
	};
};

void Robot::morph_to_life() {
	set_parameter(ROBOT_MORPH_TO_LIFE);
};

void Robot::morph_to_inert() {
	if (current_parameter() != ROBOT_INERT && current_parameter() != ROBOT_MORPH_TO_INERT 
		 && (body != NULL || tt_programmer->training() || tt_log_version_number < 64)) { // conditional new on 151199
		// body != NULL new on 141204 -- training test new on 161204
		set_parameter(ROBOT_MORPH_TO_INERT);
	};
	if (floor != NULL && floor->pointer_to_picture() == NULL) set_priority(floor->next_item_priority());
//	if (tt_log_version_number < 15) {
//		finish_instantly(FALSE); //,TRUE); // added TRUE on 010399 so that this happens even if power is off 
//	};
	//	if (next_robot != NULL) {
//		next_robot->morph_to_inert();
//	};
	if (name_from_user != NULL && tt_log_version_number >= 64) { // new on 131204
		set_cycle_stopped_callback(MAKE_VISIBLE_AND_FIT_NAME_ON_CHEST_CALLBACK,TRUE);
	};
};

boolean Robot::is_inert() {
   if (current_parameter() == ROBOT_INERT) return(TRUE);
   if (current_parameter() == ROBOT_MORPH_TO_INERT && cycle_stopped()) return(TRUE);
   return(FALSE);
};

void Robot::appear_working() {
	set_parameter(ROBOT_POINTING); //ROBOT_DROPING_ITEM); //ROBOT_WORKING);
};

//void Robot::set_inside_sprite(boolean new_flag) {
//	inside_sprite_flag = (flag) new_flag;
//	if (inside_sprite_flag && floor != NULL) {
//		floor->remove_item(this,FALSE);
//	};
//};

Sprite *Robot::pointer_to_ultimate_item() {
   if (tool == NULL) return(NULL);
   if (tool->kind_of() == COPIER) {
      Sprite *attached = ((Copier *) tool)->attached_tool();
      if (attached != NULL) return(attached);
   };
   return(tool);
};

Path *Robot::path_to(Sprite *target, boolean &ok) {
	if (target->pointer_to_leader() == this) { // why leader and not first_in_line?? or something?
		// note that this leaves OK as TRUE so path is used
		return(NULL); // prior to 130205 was new Path(target->kind_of())); // should only be a robot in the team
	} else {
		Cubby *cubby = pointer_to_thought_bubble()->pointer_to_cubby();
		if (cubby == target) {
			return(new Path(SPECIAL_PATH_MARKER,new Path(THOUGHT_BUBBLE,new Path(CUBBY))));
		} else if (cubby != NULL) {
			Path *path_from_cubby = cubby->path_to(target,ok);
			if (ok) { // toggle SPECIAL_PATH_MARKER
				return(new Path(SPECIAL_PATH_MARKER,new Path(THOUGHT_BUBBLE,new Path(CUBBY,new Path(SPECIAL_PATH_MARKER,path_from_cubby)))));
			};
		};
	};
	ok = FALSE;
	return(NULL);
};

boolean Robot::current_value(NumberValuePointer &the_value, boolean &shared, Sprite *by) {
	// so when dropped on a blank number it receives the "arity"
//	the_value.set_value(robots_in_team());
	the_value = new LongNumberValue(robots_in_team());
	shared = FALSE;
	return(TRUE);
};

void Robot::become_blank(boolean by_user, boolean controlee_too) {
	thought_bubble->become_blank(by_user,controlee_too);
	if (name_from_user != NULL) { // new on 121099
//		name_from_user->become_blank(by_user,controlee_too);
		name_from_user->set_visible(FALSE); // updated 141204
	};
	Sprite::become_blank(by_user,controlee_too);
	if (next_robot != NULL) { // new on 130405
		next_robot->become_blank(by_user,controlee_too);
	};
};

void Robot::become_non_blank(boolean controlee_too) {
	thought_bubble->become_non_blank(controlee_too);
	if (name_from_user != NULL) { // new on 121099
//		name_from_user->become_non_blank(controlee_too);
		name_from_user->set_visible(TRUE); // updated 141204
	};
	Sprite::become_non_blank(controlee_too);
	if (next_robot != NULL) { // new on 130405
		next_robot->become_non_blank(controlee_too);
	};
};

void Robot::set_string_name_from_user(string new_name) {
	if (new_name == NULL) { // || new_name[0] == NULL) { // changed on 090499 so "" is an OK name
		// back to name generated from serial number
		char ascii_name[16];
		serial_number_to_name(serial_number(),ascii_name,TRUE);
		if (ascii_name[0] != NULL) { // added on 021199 to check if no body yet not empty string
			set_name_from_user(variable_width_text_pad(a2u(ascii_name),FALSE,tt_white)); 
			name_from_serial_number = TRUE; // added on 090499
		};
	} else {
		set_name_from_user(variable_width_text_pad(new_name,FALSE,tt_white));
	};
};

void Robot::set_name_from_user(Text *new_name) {
	// new_name should be a private copy
	if (name_from_user != NULL) {
		remove_follower(name_from_user);
		tt_screen->remove(name_from_user); // just in case
		name_from_user->destroy(); // restored on 160103
	};
	name_from_user = new_name;
	if (name_from_user == NULL) {
		set_string_name_from_user(NULL); // will cause serial number name to be generated
		name_from_serial_number = TRUE; // on 270399 moved here from set_string_name_from_user
	} else {
		name_from_user->set_selectable(FALSE); // but robot is
		name_from_user->set_graspable(FALSE); // but robot is
		name_from_user->set_show_all(FALSE); // if loaded from file may have lost this
		name_from_user->set_color(tt_white);
//		if (showing_on_a_screen()) tt_screen->add(name_from_user);
		// here was equivalent of attach_name_from_user() before 171202 - now on first display does all this
		name_from_serial_number = FALSE; // on 270399 moved here from set_string_name_from_user
		if (name_from_user->pointer_to_leader_offsets() != NULL) { // new on 260103
			add_follower(name_from_user,TRUE,INSERT_AT_END,TRUE);
		} else {
			fit_name_on_chest(TRUE); // new on 290103
			add_follower(name_from_user);
		};
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 70900) {
		character robot_name[max_resource_string_length];
		name(robot_name);
		if (strnicmp(robot_name,debug_robot_name,strlen(debug_robot_name)) == 0) {
			tt_error_file() << "Naming a robot " << robot_name << "#" << debug_counter << " on frame " << tt_frame_number << endl;
		};
	};
#endif
	if (!is_inert() && tt_log_version_number >= 64) { // new on 131204
		set_cycle_stopped_callback(MAKE_VISIBLE_AND_FIT_NAME_ON_CHEST_CALLBACK,TRUE);
	};
};

void Robot::attach_name_from_user() { // abstracted on 171202
	add_follower(name_from_user);
	fit_name_on_chest(TRUE);
	set_cycle_stopped_callback(MAKE_VISIBLE_AND_FIT_NAME_ON_CHEST_CALLBACK,TRUE); // fix location at end of cycle if need be
};

const short int name_location[14][4] = {
	// for each parameter gives x,y,w, and h
{369,483,231,180},
{241,358,200,155},
{0,0,0,0}, // moving too much to even try
{575,484,231,181},
{575,484,231,181},
{380,479,200,181}, // was 0,0,0,0
{380,479,200,181},
{345,477,200,181},
{400,474,200,181},
{400,474,200,181},
{340,490,200,181},
{241,358,200,155}, // same as #1
{367,481,200,175},
{367,465,200,175}};

void Robot::fit_name_on_chest(boolean make_visible_too, boolean update_display_too) {
	if (name_from_user == NULL) return;
//	if (floor == NULL || !floor->on_the_floor()) return; // new on 021199 since in room calculates badly
	if (make_visible_too) name_from_user->set_visible(TRUE); // prior to 121099 this was in conditional
//	if (width > tile_width && height > tile_height) { 
	// conditions commented out on 161199 since robots in a team sometimes didn't have name at the right place
	if (update_display_too) { // conditional added 270704 so doesn't call update_display if called from animation called by update_display
		update_display(); // added 021199 so following is up to date
//		name_from_user->update_display(); // new on 131204 -- didn't seem to help so removed on 161204
	};
	city_coordinate new_width = (width*name_location[parameter][2])/1000;
	city_coordinate new_height = (height*name_location[parameter][3])/1000;
	name_from_user->set_size_and_location(new_width,new_height,
													  llx+(width*name_location[parameter][0])/1000-new_width/2, // location is for the center
													  lly+(height*name_location[parameter][1])/1000-new_height/2);
//	};
};

boolean Robot::set_parameter(short int new_parameter, boolean warn, boolean reset_cycle_regardless) {
	if (new_parameter == current_parameter() && !reset_cycle_regardless) return(TRUE);
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Debug target setting parameter."));
	};
#endif
	// run this first since call asking if this cycles() needs it
	boolean result = Sprite::set_parameter(new_parameter,warn,reset_cycle_regardless);
   if (name_from_user != NULL) {
//		fit_name_on_chest();
		if (!cycles() && !cycle_stopped()) { 
			// on 121099 added !cycle_stopped() so is invisible only if non-looping cycle is about to happen
			name_from_user->set_visible(FALSE);
		};
		set_cycle_stopped_callback(MAKE_VISIBLE_AND_FIT_NAME_ON_CHEST_CALLBACK,TRUE);
	};
	return(result);
};

string Robot::copy_string_name_from_user() {
	if (name_from_serial_number || name_from_user == NULL) {
		return(NULL);
	} else {
		return(name_from_user->copy_narrow_text_string());
	};
};

Robot *Robot::team_leader() {
	if (first_in_line == NULL) { // I am
		return(this);
	} else {
		return(first_in_line);
	};
};

Robot *Robot::first_team_member_to_fail_for_unstable_reasons() {
	if (first_in_line == NULL) { // I am
		return(first_to_fail_for_unstable_reasons);
	} else {
		return(first_in_line->pointer_to_first_to_fail_for_unstable_reasons());
	};
};

void Robot::set_first_team_member_to_fail_for_unstable_reasons(Robot *robot) {
	if (first_in_line == NULL) {
		set_first_to_fail_for_unstable_reasons(robot);
	} else {
		first_in_line->set_first_to_fail_for_unstable_reasons(robot);
	};
};

#if TT_DEBUG_ON
void Robot::set_waiting_for_release(boolean new_flag) {
	waiting_for_release_flag = (flag) new_flag;
	if (tt_debug_target == debug_counter) {
		if (new_flag) {
			log("Setting waiting for release to TRUE for debug target.");
		} else {
			log("Setting waiting for release to FALSE for debug target.");
		};
	};
};
#endif

boolean Robot::team_running() {
	if (first_in_line == NULL) { // I am
		return(team_running_flag);
	} else {
		return(first_in_line->team_running());
	};
};

void Robot::set_team_running(boolean new_flag) {
	if (first_in_line == NULL) { // I am
		team_running_flag = new_flag;
	} else {
		first_in_line->set_team_running(new_flag);
	};
};

void Robot::stop_all(boolean for_good, boolean suspend_robots_too) {
	// re-modularized this on 070400 so that picture::stop_all just calls this rather than doing this conditional
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log("Stopping debug target (robot)");
	};
	if (tt_debug_mode == 70900 && running_body_flag) {
		tt_error_file() << "Robot stopped at " << tt_frame_number << endl;
	};
#endif
	if (about_to_quit()) { // new on 020404
		return; // since some of the following won't work right if shutting down
	};
	if (tt_log_version_number < 22) {
		stop_team();
	} else if (for_good) {
		stop_team();
	} else if (suspend_robots_too) {
		suspend();
	}; // else do nothing
};

void Robot::set_inside_vacuum(boolean new_flag, boolean recur) { // new on 080400 - important if loading a vacuum with a robot saved with its top cubby
//   if (inside_vacuum_flag == new_flag) return; // new on 170702 to ignore calls that already know about - commented out on 190702 since might not have been called with recur earlier
	boolean changed = (!!inside_vacuum_flag != !!new_flag); // force to true boolean before comparing
   Sprite::set_inside_vacuum(new_flag,recur);
	if (new_flag && changed && tt_log_version_number >= 22) {
		stop_all();
	};
};

Robot *Robot::pointer_to_next_robot() {
	 // could be that next_robot is temporarily set to NULL - new on 080400 (re-organized on 100400)
	if (next_robot != NULL || tt_log_version_number < 22) {
		return(next_robot);
	} else {
		return(saved_next_robot);
	};
};

void Robot::set_next_robot(Robot *new_next, boolean put_behind_too) { // put_behind_too new on 120203
   // should this deal with the case where there already is a binding for next_robot??
   if (new_next != NULL) {
#if TT_DEBUG_ON
		if (new_next == this || new_next->pointer_to_next_robot() == this) {
			debug_this();
		};
#endif
      // following is new on 250802 -- (callers used to do this)
      if (first_in_line != NULL) {
         new_next->set_first_in_line(first_in_line);
      } else {
         new_next->set_first_in_line(this);
      };
		if (put_behind_too) {
			put_behind_me(new_next);
		};
   };
   next_robot = new_next;
};

void Robot::set_saved_next_robot(Robot *new_next) { // new on 110402
	if (saved_next_robot != NULL) {
		saved_next_robot->destroy(); // release old reference (new on 230105)
	};
   if (new_next != NULL) {
      // following is new on 250802 -- (callers used to do this)
      if (first_in_line != NULL) {
         new_next->set_first_in_line(first_in_line);
      } else {
         new_next->set_first_in_line(this);
      };
		new_next->increment_ref_count(this); // moved here on 290704 for robustness
   };
   saved_next_robot = new_next;
//   saved_next_robot->increment_ref_count(this);
};

void Robot::set_robot_to_put_behind(Robot *robot) { // new on 230105
	if (robot_to_put_behind != NULL) {
		robot_to_put_behind->destroy(); // release reference
	};
	robot_to_put_behind = robot;
	if (robot_to_put_behind != NULL) {
		robot_to_put_behind->increment_ref_count();
	};
};

boolean Robot::ok_to_delete_bammed() {
	// new on 131000 to maintain backwards compatibility
	return (body == NULL || kind_of() == ROBOT_IN_TRAINING || version_number() >= first_robot_version_to_delete_when_coercing);
};

int Robot::version_number() { // new on 010201
	if (kind_of() == ROBOT_IN_TRAINING) {
		return(latest_robot_version); // new on 040201
	} else if (body == NULL) {
		return(-1);
	} else {
		return(body->robot_version_number());
	};
};

void Robot::add_outside_followers_to_screen() { // new on 180101
	fit_name_on_chest();
	Sprite::add_outside_followers_to_screen();
};


void Robot::action_completed() { // new on 131102
   // rewritten on 171102 with the assumption that the action_status was always the body_status of this
	completed(); // action_status);
//	action_status = NULL;
};

void Robot::finish_instantly(boolean callbacks_only, boolean ignore_city_stopped) {
	Sprite::finish_instantly(callbacks_only,ignore_city_stopped);
	if (next_robot != NULL) {
		next_robot->finish_instantly(callbacks_only,ignore_city_stopped); // since whole team should finish right away
	};
};

void Robot::reset_drop_scroll_deltas() { // made to apply entire team on 050404
	drop_scroll_delta_x = 0;
	drop_scroll_delta_y = 0;
	if (next_robot != NULL) {
		next_robot->reset_drop_scroll_deltas();
	};
};

#if TT_DEBUG_ON
boolean Robot::does_follower_matter(Sprite *sprite) {
	return(sprite != name_from_user); // name can't change the behavior of things
};
#endif

/*

void Robot::recursively_propagate_changes() {
	if (thought_bubble != NULL) {
		thought_bubble->recursively_propagate_changes();
	};
	if (next_robot != NULL) {
		next_robot->recursively_propagate_changes();
	};
	Tool::recursively_propagate_changes();
};

void Robot::propagate_changes() {
	// too messy having both kinds of followers so 
	// thought bubble and next robot are separate
	// also easier to maintain constraints this way
	if (clean()) return;
	if (thought_bubble_attached) {
		thought_bubble->move_to(llx+true_width()/2,lly+true_height());
		thought_bubble->update_display();
	};
	if (next_robot != NULL) {
		city_coordinate behind_llx, behind_lly;
		location_behind_me(behind_llx,behind_lly);
		next_robot->move_to(behind_llx,behind_lly);
		next_robot->update_display();
	};
	Tool::propagate_changes();
};

void Robot::give_up() {
	set_priority(floor->next_item_priority());
	move_to_side();
	if (next_robot != NULL) {
		next_robot->give_up();
	};
};
*/
/*
Background *Robot::enter_thought_bubble(Thought_Bubble *bubble,
																  Screen *screen) {
	thought_bubble = bubble;
	Background *new_floor
		= new Floor(screen,floor->pointer_to_room(),
											bubble->pointer_to_cubby());
	set_floor(new_floor);
	return(new_floor);
};
*/
