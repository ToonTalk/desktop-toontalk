// Copyright (c) 1992-2003.  Ken Kahn, Animated Programs, All rights reserved.

//#include "defs.h"
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif   
#if !defined(__TT_BOMB_H)   
#include "bomb.h"
#endif   
#if !defined(__TT_FLOOR_H)   
#include "floor.h"
#endif   
#if !defined(__TT_ROBOT_H)   
#include "robot.h"
#endif   
#if !defined(__TT_HELP_H)   
#include "help.h"
#endif   
#if !defined(__TT_MARTIAN_H)   
#include "martian.h"
#endif   
#if !defined(__TT_PRGRMMR_H)   
#include "prgrmmr.h"
#endif   
#if !defined(__TT_PICTURE_H)   
#include "picture.h"
#endif   
#if !defined(__TT_WINMAIN_H)   
#include "winmain.h"
#endif   
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif 

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum BombStates {BOMB_INERT,BOMB_FUSE_BURNING};

// if changed update help.cpp
enum BombHistory {BOMB_USED_HOUSE=0,
						BOMB_USED_THOUGHT_BUBBLE};

boolean bomb_ever_used() {
  return(did_user(BOMB_USED_HOUSE,BOMB));
};

Bomb::Bomb(city_coordinate x, city_coordinate y,
			  long initial_priority,
			  city_coordinate width,
			  city_coordinate height) :
   Sprite(BOMB_SPRITE,x,y,BOMB,BOMB_INERT,ground_scale,width,height) {
//  action_status(NULL),
//  saved_by(NULL) {
//	set_priority_function_of_lly(FALSE);
	set_priority(initial_priority);
};

//void Bomb::prepare_for_deletion(Sprite *by) { // commented out since Sprite::prepare_for_deletion does it all now
//   if (saved_by != NULL && !tt_shutting_down) { // added second disjunct to avoid infinite loop while shutting down if robot is working with a bomb
//      saved_by->destroy(); // if zero ref count
//      saved_by = NULL;
//   };
//   Sprite::prepare_for_deletion(by);
//};

Sprite *Bomb::copy(boolean also_copy_offsets) {
//	city_coordinate center_x, center_y;
//	center_location(center_x,center_y);
	Bomb *copy = new Bomb(llx,lly,current_priority-1,width,height);
//	if (saved_width != 0) {
//		copy->set_saved_size(saved_width,saved_height);
//	};
 //  copy->set_priority_fixed(priority_fixed_flag);
	//if (comment != NULL) {
	//	copy->set_comment(comment->copy());
	//};
   finish_making_copy(copy,also_copy_offsets);
	return(copy);
};

Sprite *Bomb::respond_to_keyboard(unsigned char , boolean extended, boolean robot_selected,
                                  Sprite *by, boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless) {
   // don't care which key it is
	if (extended) { //  new on 180305 since otherwise F1 or arrow keys can set off the bomb
		return(this);
	};
	return(used(NULL,by,FALSE,record_action,0,FALSE,NULL));
};

Sprite *Bomb::used(Sprite *, Sprite *by,
						 boolean , boolean record_action,
						 millisecond , boolean , Sprite *) {
   if (!held() && by->kind_of() != ROBOT) { // if not held then ignore keyboard
		// added conjunct on 120499 so robots on the back of pictures can use bombs without bothering to "hold" them
      by->completed();
		return(this);
	};
	// don't repeat while mouse is down
	if (!by->still_running()) simulate_mouse_up();
   if (floor->outside() && floor->pointer_to_picture() == NULL) { // aren't in a house so nothing to blow up
      by->abort_action(IDS_BOMBS_ONLY_WORK_INSIDE_OF_HOUSES_ABORT);
      return(this);
   };
#if TT_DEBUG_ON
	if (by->still_running() && tt_debug_mode == 7777) {
		by->completed();
		return(this);
	};
	//if (by != NULL && by->kind_of() == ROBOT) {
	//	((Robot *) by)->print(tt_error_file());
	//	tt_error_file() << " used bomb at " << tt_frame_number << endl;
	//};
#endif
//	action_status = the_action_status;
	set_saved_by(by);
	Picture *picture;
	boolean showing = showing_on_a_screen();
	boolean used_by_leader = (by == leader); // bound here now (110803) since explosion_done makes this untrue
	if (showing) {
		set_parameter(BOMB_FUSE_BURNING);
		if (record_action) record_event(APPLY_GRASPED_ITEM,by,floor);
		set_cycle_stopped_callback(EXPLODE_CALLBACK);
		floor->house_will_explode(total_cycle_duration()+750); // plus estimate of explosion time (500ms should be OK)
	} else if ((picture = floor->pointer_to_picture()) != NULL) { // on back of picture
		// added following on 120499 since the bomb should blow up the whole picture
		picture = (Picture *) picture->ultimate_unflipped_picture(); // if starting with a picture will always end up with one
		// above was ultimate_flipside_picture prior to 220500 - but pictures in boxes for example should explode on the back and not the container
		Sprite *picture_leader;
		if ((picture_leader = picture->pointer_to_leader()) != NULL) {
         picture_leader->remove_selected_element(picture,NO_SELECTION_REASON_GIVEN,this,FALSE); // FALSE new on 191199 so isn't added to floor
		};
		if (picture->held()) {
			if (picture->current_held_status() == HELD_BY_ROBOT) { // new on 060300
				// when training didn't know it would blow so don't now while held
				// still useful if this was a remote looks and the other guy blew up too
				// and this will get vacuumed away anyway
//				((Robot *) picture_leader)->delete_when_deleted(picture);
//				completed(the_action_status);
            ::completed(by);
				return(NULL); // or this??
			};
			tt_programmer->release_items_held(); 
		};
		Background *picture_background = picture->pointer_to_background();
		if (picture_background != NULL) picture_background->remove_item(picture,FALSE,TRUE); // test was background != NULL prior to 191199
		if (!picture->destroy()) {
			picture->remove_from_floor(FALSE); // new on 070400
			picture->set_slated_for_deletion(TRUE); // new on 280300 - important since otherwise can still be collided with etc
		};
		return(NULL); // floor is destroyed now...
//		tt_deleted_sprites = new Sprites(picture,tt_deleted_sprites);
	} else { // bomb going off inside a house but not seen
		// delay in case programmer got in while this was going on
		Background *programmers_background = tt_programmer->pointer_to_appearance()->pointer_to_background();
		if (programmers_background != NULL) {
			Room *programmers_room = programmers_background->pointer_to_room();
			if (programmers_room != NULL && programmers_room == floor->pointer_to_room()) {
				// if bomb is going of in same room as programmer get him out of there
				tt_programmer->house_about_to_be_destroyed();
				if (tt_toolbox != NULL) tt_toolbox->finish_instantly();
			};
		};
		floor->house_will_explode(1); // in case programmer sees this
//		floor->destroy_house();
//		do_after(500,this,explosion_done_callback);
		explosion_done();
#if TT_DEBUG_ON
		if (tt_debug_mode == 3231) {
			by->print(tt_error_file());
			tt_error_file() << " is about to set off a bomb on frame " << tt_frame_number << endl;
		};
#endif
	};
	if (used_by_leader && by->still_running()) { // used by trained robot
		Robot *robot = (Robot *) by;
		robot->stop_all(FALSE); // tried to add TRUE arg but then retrained robot forgot how many elements needed for working set
		// and have problem releasing working set while bomb is still working
		if (robot->kind_of() == ROBOT_IN_TRAINING) { // condition is new on 110803
			tt_programmer->stop_retraining(robot); // new on 160801
			robot->set_suspended(FALSE); // new on 160801
		};
	} else {
      if (floor != NULL && floor->inside_rule_body()) {
		   user_did(BOMB,BOMB_USED_THOUGHT_BUBBLE,by);
	   } else {
		   user_did(BOMB,BOMB_USED_HOUSE,by);
      };
//      if (by == leader && by->kind_of() == ROBOT_IN_TRAINING) {
//         tt_programmer->programmer_at_floor()->release_object(TRUE); // at least to index properly
//      };
	};
	if (floor != NULL && !floor->inside_rule_body() && 
       showing && tt_toolbox != NULL && !tt_toolbox->gone()) { // last condition added 030300
		// in case something valuable is about to be blown up
		// let vacuum have it 
		tt_finish_all_animations = TRUE;
      boolean vacuumed_stuff = FALSE;
		Sprites *remaining = NULL;
		if (tt_system_mode != PUZZLE) {
			// added on 270499 since when running puzzles shouldn't be vacuuming up old stuff
			remaining = floor->pointer_to_items();
		};
		Vacuum *vacuum = tt_toolbox_vacuum;
		while (remaining != NULL) {
			Sprite *sprite = remaining->first();
			Sprites *next_remaining = remaining->rest(); // save now before clobbering
			if (sprite != tt_toolbox && sprite != this && sprite != tt_martian &&  
				 !toolbox_tool(sprite) && !sprite->infinite_stack()) {
				switch (sprite->kind_of()) {
					case ROBOT:
						((Robot *) sprite)->stop_all(TRUE); // TRUE added on 121000 - maybe since 070400 this meant the bomb wasn't dropped
						break;
					case PICTURE:
						((Picture *) sprite)->stop_all();
						break;
					default:
						sprite->finish_instantly();
						break;
				};
				if (sprite->pointer_to_leader() == NULL &&
					sprite->kind_of() != MOUSE_WITH_HAMMER) {
					vacuum->add_deletee(sprite); // this will remove it from floor and screen as well
               vacuumed_stuff = TRUE;
				} else if (floor != NULL) {  // e.g. part of a team of robots
					// condition added on 090705 for robustness (issue 690)
					floor->remove_item(sprite,FALSE,TRUE);
				};
			};
			remaining = next_remaining;
		};
		tt_finish_all_animations = FALSE;
      if (vacuumed_stuff) make_sucking_sound(); // added 12/12 to give a hint that the stuff is still around
	} else if (!showing && floor != NULL) {
		floor->remove_item(this,FALSE); // robot may have dropped it in stop_all()
	};
	return(NULL);
};

void Bomb::explode() { 
	tt_screen->remove(this); // bomb no longer visible
	play_sound(EXPLOSION_SOUND);
	if (tt_toolbox != NULL && !tt_toolbox->gone()) { // second condition added 030300
      tt_toolbox->finish_instantly(FALSE,TRUE); // finish off anything still in progress
      tt_toolbox->remove_from_floor(); // somehow it isn't always when a bomb is going off
   };
	Sprite *explosion = new Sprite(EXPLOSION_SPRITE,llx,lly);
	floor->add_item(explosion);
	millisecond duration;
   if (finishing_instantly()) {
      duration = 0;
   } else {
      duration = explosion->total_cycle_duration();
   };
	explosion->set_size_percent(200); // animated version looked bad
   tt_global_picture->do_after(duration,this,EXPLOSION_DONE_CALLBACK); // added tt_global_picture-> on 031002 so can't sneak out before explosion is finished and house remains until you go back inside
};

void Bomb::explosion_done() {
	action_completed();
	if (floor != NULL) { // new on 101204 due to crash (issue 577)
		if (floor->inside_rule_body()) {
			record_event(DESTROY_HOUSE,saved_by,floor);
		} else {
			floor->destroy_house();
		};
		floor->remove_item(this,FALSE); // only if it was dropped after use
	};
	if (leader != NULL) leader->remove_follower(this);
	destroy(saved_by);
};

ReleaseStatus Bomb::item_released_on_top(Sprite *, Sprite *by, SpritePointer &, int) {
// following handled by caller now
//	if (infinite_stack_flag && item->kind_of() == BOMB) {
//		completed(action_status);
//		return(RELEASED_ON_ITS_STACK);
//	};
	if (infinite_stack()) {
		by->action_aborted();
	} else if (!contained_in(CUBBY)) {
		by->abort_action_if_robot(IDS_BOMB_WONT_ACCEPT,
										  BOMB,
										  !told_user(TOLD_BOMBS_WONT_ACCEPT));
		user_told(TOLD_BOMBS_WONT_ACCEPT);
	};
	return(RELEASE_IGNORED);
};

void Bomb::generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection) {
	if ((!did(BOMB_USED_HOUSE) && 
		  !tt_screen->inside_rule_body() &&
		  tt_help_counter == 0)
		 || tt_help_counter == 1) {
		describe_pointing(message,tool_in_hand,selection);
//		Sprite::generate_help(message,tool_in_hand);
		tt_help_history_action = BOMB_USED_HOUSE;
		if (tt_help_counter == 0) {
			message << S(IDS_BOMB_USED_HOUSE_HELP,"");
		} else {
			message << S(IDS_BOMB_USED_HOUSE_HELP2,"");
		};
		describe_right_button(message);
		if (tt_help_counter == 0) {
			message << S(IDS_BOMB_USED_HOUSE_HELP_2,"");
		} else {
			message << S(IDS_BOMB_USED_HOUSE_HELP2_2,"");
		};
      message << end_sentence();
//      if (!tt_new_input_scheme) message << S(IDS_TURN_BOMB_ON_HELP);
		message << S(IDS_BOMB_USED_HOUSE_HELP3)
				  << S(IDS_BOMB_USED_HOUSE_HELP4);
      describe_left_button(message);
		message << S(IDS_BOMB_USED_HOUSE_HELP5,""); // added 080200
      message << end_sentence();
		return;
	} else if ((!did(BOMB_USED_THOUGHT_BUBBLE) && 
					tt_screen->inside_rule_body()
					&& tt_help_counter == 0)
				  || tt_help_counter == 2) {
		tt_help_history_action = BOMB_USED_THOUGHT_BUBBLE;
		if (tt_help_counter == 0) {
			message << S(IDS_BOMB_USED_TB_HELP);
		} else {
			message << S(IDS_BOMB_USED_TB_HELP2);
		};
		describe_right_button(message);
		if (tt_help_counter == 0) { // new on 080200
			message << S(IDS_BOMB_USED_TB_HELP_2,"");
		} else {
			message << S(IDS_BOMB_USED_TB_HELP2_2,"");
		};
      message << end_sentence();
//      if (!tt_new_input_scheme) message << S(IDS_TURN_BOMB_ON_HELP);
		message << S(IDS_BOMB_USED_TB_HELP3)
				  << S(IDS_BOMB_USED_TB_HELP4);
		return;
	};
	if ((!did(TYPE_HELP_GIVEN) && tt_help_counter == 0)
		 || tt_help_counter == 3) {
		message << S(IDS_BOMB_HELP_GIVEN_HELP)
				  << S(IDS_BOMB_HELP_GIVEN_HELP2);
		no_help_interruptions();
		user_did(BOMB,TYPE_HELP_GIVEN);
	};
};

boolean Bomb::dump(output_stream &stream, boolean just_prepare, boolean is_controlee, boolean on_nest) {
   if (just_prepare) {
      finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off);
      return(FALSE);
   };
	if (!Sprite::dump(stream,just_prepare,is_controlee,on_nest)) return(FALSE);
	dump_if_not_null(stream,comment);
	return(TRUE);
};

void Bomb::java(java_stream &out) { // new on 281200 since can occur in a box
	out << "TT.BOMB"; // could be added to javacom.rc - IDS_JAVA_BOMB_STACK is there
};

boolean describe_bomb(InputStream *pad_in, output_stream &text_out, int version_number, Article article) {
	if (version_number >= first_version_to_save_copied_nests) {
		describe_comment_encoding(pad_in,text_out,version_number,BOMB);
	};
	print_type(BOMB,text_out,article);
	return(TRUE);
};

