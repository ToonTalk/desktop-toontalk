// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.  

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif   
#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif   
#if !defined(__TT_PAD_H)   
#include "pad.h"
#endif   
#if !defined(__TT_ROBOT_H)   
#include "robot.h"
#endif  
#if !defined(__TT_LOG_H)   
#include "log.h"
#endif   
#if !defined(__TT_THOUGHT_H)   
#include "thought.h"
#endif   
#if !defined(__TT_CUBBY_H)   
#include "cubby.h"
#endif   
#if !defined(__TT_BIRD_H)   
#include "bird.h"
#endif   
#if !defined(__TT_NUMBER_H)   
#include "number.h"
#endif   
#if !defined(__TT_TEXT_H)   
#include "text.h"
#endif
#if !defined(__TT_SOUND_H)
#include "sound.h"
#endif
#if !defined(__TT_PICTURE_H)   
#include "picture.h"
#endif   
#if !defined(__TT_BOMB_H)   
#include "bomb.h"
#endif   
#if !defined(__TT_TRUCK_H)   
#include "truck.h"
#endif   
#if !defined(__TT_FLOOR_H)   
#include "floor.h"
#endif   
#if !defined(__TT_EVENT_H)   
#include "event.h"
#endif   
#if !defined(__TT_PRGRMMR_H)
#include "prgrmmr.h"
#endif
#if !defined(__TT_MARTIAN_H)
#include "martian.h"
#endif
#if !defined(__TT_PUZZLE_H)
#include "puzzle.h"
#endif
#if !defined(__TT_WINMAIN_H)
#include "winmain.h"
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
#if !defined(__TT_ROOM_H)
#include "room.h"
#endif
#if !defined(__TT_SPEAK_H)
#include "speak.h"
#endif
#if TT_DIRECT_INPUT
#if !defined(__TT_INPUT_H)
#include "input.h"
#endif
#endif
#if !defined(__TT_UTILS_H)
#include "utils.h" // for sound stuff
#endif
#if !defined(__TT_DIRTY_H)
#include "dirty.h" 
#endif

void accept_contents_by_bird_callback(Sprite *sprite) {
	((Nest *) sprite)->accept_contents(TRUE); 
};

void accept_contents_by_non_bird_callback(Sprite *sprite) {
	((Nest *) sprite)->accept_contents(FALSE); 
};

void receive_item_again_callback(Sprite *bird) {
	((Bird *) bird)->receive_item_again();
};

void shrink_to_tiny_callback(Sprite *b) {
  ((Bird *) b)->shrink_to_tiny();
};

void fly_back_callback(Sprite *bird) {
	((Bird *) bird)->fly_back();
};

void returned_to_house_callback(Sprite *bird) {
	((Bird *) bird)->returned_to_house();
};

void shrink_then_returned_to_house_callback(Sprite *b) {
	((Bird *) b)->shrink_then_returned_to_house();
};

void bird_fly_up_callback(Sprite *bird) {
	((Bird *) bird)->set_parameter(BIRD_FLY_UP);
   ((Bird *) bird)->set_selectable(FALSE); //not while flying
};

void note_nest_building_finished_callback(Sprite *bird) {
	((Bird *) bird)->note_nest_building_finished();
};

//void move_to_return_x_y_callback(Sprite *bird) {
//	((Bird *) bird)->move_to_return_x_y();
//};

void bird_morph_to_inert_callback(Sprite *bird) {
	bird->morph_to_inert(); // prior to 300604 used to do a type coercion here
};

void bird_morph_to_inert_and_fix_drift_callback(Sprite *bird) {
 	bird->morph_to_inert(); // prior to 300604 used to do a type coercion here
   ((Bird *) bird)->fix_drift();
};

void bird_give_me_callback(Sprite *bird) {
	((Bird *) bird)->set_parameter(BIRD_GIVE_ME);
};
	
void build_nest_callback(Sprite *bird) {
	((Bird *) bird)->build_nest();
};

void fly_outside_now_full_size_callback(Sprite *b) {
	((Bird *) b)->fly_outside_now_full_size();
};

void fly_down_callback(Sprite *b) {
	((Bird *) b)->fly_down();
};

void fix_t_shirt_callback(Sprite *b) {
   ((Bird *) b)->fix_t_shirt();
};

#if TT_DIRECT_PLAY

void send_pending_messages_callback(Sprite *nest) {
	((Nest *) nest)->send_pending_messages();
};

void join_a_direct_play_session_callback(Sprite *nest) {
	((Nest *) nest)->join_a_direct_play_session();
};

#endif

// conditions added 140104 (or 130104) for robustness

void deliver_to_another_house_callback(Sprite *b) {
  if (b != NULL) ((Bird *) b)->deliver_to_another_house();
};

void shrink_then_deliver_to_another_house_callback(Sprite *b) {
  if (b != NULL) ((Bird *) b)->shrink_then_deliver_to_another_house();
};

void show_nest_and_return_callback(Sprite *sprite) {
	if (sprite != NULL) ((Bird *) sprite)->show_nest_and_return();
};

void nest_is_made_callback(Sprite *bird) {
	if (bird != NULL) ((Bird *) bird)->nest_is_made();
};

void hatch_bird_callback(Sprite *n) {
  if (n != NULL) ((Nest *) n)->hatch_bird();
};

void bird_has_hatched_callback(Sprite *nest) {
	if (nest != NULL) ((Nest *) nest)->bird_has_hatched();
};

void destroy_item_and_return_callback(Sprite *sprite) {
	if (sprite != NULL) ((Bird *) sprite)->destroy_item_and_return();
};

void grow_by_ten_callback(Sprite *sprite) {
	if (sprite != NULL) ((Bird *) sprite)->grow_by_ten();
};

void fly_to_nest_callback(Sprite *sprite) {
	if (sprite != NULL) ((Bird *) sprite)->fly_to_nest();
};

void set_down_message_callback(Sprite *sprite) {
	((Bird *) sprite)->set_down_message();
};

void set_down_stack_callback(Sprite *sprite) {
	if (sprite != NULL) ((Bird *) sprite)->set_down_stack();
};

void pick_up_message_callback(Sprite *sprite) {
	if (sprite != NULL) ((Bird *) sprite)->pick_up_message();
};

void pick_up_stack_callback(Sprite *sprite) {
	if (sprite != NULL) ((Bird *) sprite)->pick_up_stack();
};

void fly_to_another_house_callback(Sprite *sprite) {
	if (sprite != NULL) ((Bird *) sprite)->fly_to_another_house();
};

void release_callback(Sprite *sprite) {
	if (sprite != NULL) ((Bird *) sprite)->release();
};

void put_stack_back_callback(Sprite *sprite) {
	if (sprite != NULL) ((Bird *) sprite)->put_stack_back();
};

void release_bad_message_callback(Sprite *sprite) {
	if (sprite != NULL) ((Bird *) sprite)->release_bad_message();
};

void return_to_floor_callback(Sprite *sprite) {
	if (sprite != NULL) ((Bird *) sprite)->return_to_floor();
};

void return_to_cubby_callback(Sprite *sprite) {
	if (sprite != NULL) ((Bird *) sprite)->return_to_cubby();
};

void return_to_cubby_on_flipside_callback(Sprite *sprite) {
	if (sprite != NULL) ((Bird *) sprite)->return_to_cubby_on_flipside();
};

void destroy_bird_callback(Sprite *sprite) {
	if (sprite != NULL) ((Bird *) sprite)->destroy_bird();
};

void deliver_to_nowhere_callback(Sprite *b) {
   if (b != NULL) ((PrimitiveBird *) b)->deliver_to_nowhere();
};

void explode_callback(Sprite *b) {
	((Bomb *) b)->explode();
};

void explosion_done_callback(Sprite *bomb) {
	((Bomb *) bomb)->explosion_done();
};

void suck_up_callback(Sprite *sprite) {
	((Cubby *) sprite)->suck_up();
};

void animate_suck_up_callback(Sprite *c) {
	((Cubby *) c)->animate_suck_up();
};

// noticed this was obsolete on 121202
//void set_holes_callback(Sprite *sprite) {
//	((Cubby *) sprite)->set_to_planned_number_of_holes();
//};

void reset_robot_priority_callback(Sprite *r) {
	((Robot *) r)->set_priority(default_robot_priority);
//   tt_error_file() << "debugging -- reset robot priority callback " << tt_current_time << endl;
};

void move_robot_out_of_the_way_callback(Sprite *robot) {
	((Robot *) robot)->animate_delta_location(0,4*tile_height,500);
};

void house_built_callback(Sprite *house_appearance) {
	((House_Sprite *) house_appearance)->house_built();
};

void house_destroyed_callback(Sprite *house_appearance) {
  ((House_Sprite *) house_appearance)->house_destroyed();
};

#if TT_DIRECT_INPUT

void restore_background_force_callback(Sprite *) {
   if (return_background_force_feedback_effect() != NULL) play_force_feedback_effect(return_background_force_feedback_effect());
};

#endif

void teleport_in_done_callback(Sprite *m) {
	((Martian *) m)->teleport_in_done();
};

void teleport_out_done_callback(Sprite *m) {
	((Martian *) m)->teleport_out_done();
};

void speech_has_finished_callback(Sprite *m) { // new on 110401
	((Martian *) m)->speech_has_finished_by_callback();
};

void place_rest_of_text_callback(Sprite *t) {
	if (t != NULL) { // new for robustness on 061103
		((Talk_Balloon *) t)->place_rest_of_text();
	};
};

void next_subtitle_for_closed_caption_alternative_language_callback(Sprite *t) {
   if (t != NULL) { // new for robustness on 061103
		((Talk_Balloon *) t)->next_subtitle_for_closed_caption_alternative_language();
	};
};

void teleport_in_callback(Sprite *m) {
	((Martian *) m)->teleport_in();
};

#if TT_TEXT_TO_SPEECH

void marty_speaks_callback(Sprite *) {
   tt_martian->new_talk_mode(tt_martian->return_saved_tt_marty_talk());
};
#endif

void speak_next_sentence_callback(Sprite *tb) {
	((Talk_Balloon *) tb)->speak_next_sentence();
};
  
void get_smashed_callback(Sprite *p) {
	((Mouse *) p)->get_smashed();
};

void undo_get_smashed_callback(Sprite *p) {
	((Mouse *) p)->undo_get_smashed();
};

void do_operation_callback(Sprite *p) {
	((Mouse *) p)->do_operation();
};

void undo_do_operation_callback(Sprite *p) {
	((Mouse *) p)->undo_do_operation();
};

void am_smashed_callback(Sprite *p) {
	((Mouse *) p)->am_smashed();
};

void remove_mouse_callback(Sprite *p) {
	((Mouse *) p)->remove_mouse();
};

void done_undoing_callback(Sprite *p) {
	((Mouse *) p)->done_undoing();
};

void run_delayed_call_in_a_mouse_callback(Sprite *x) {
   delayed_call_in_a_mouse *d = (delayed_call_in_a_mouse *) x;
   d->run();
   delete d;
};

void define_page_callback(Sprite *sprite) {
	((Notebook *) sprite)->define_page();
};

void go_to_items_page_callback(Sprite *pad) {
	((Notebook *) pad)->go_to_items_page();
};

void go_to_recent_page_callback(Sprite *pad) {
	((Notebook *) pad)->go_to_recent_page(TRUE);
};

void settle_down_callback(Sprite *pad) {
	((Notebook *) pad)->settle_down();
};

void finished_settling_down_callback(Sprite *pad){
	((Notebook *) pad)->finished_settling_down();
};

void continue_turn_page_callback(Sprite *pad) {
	((Notebook *) pad)->continue_turn_page();
};

void forward_switch_contents_callback(Sprite *pad) {
	((Notebook *) pad)->forward_switch_contents();
};

void overlay_previous_done_callback(Sprite *pad) {
	((Notebook *) pad)->overlay_previous_done();
};

void get_grasped_callback(Sprite *pad) {
	((Notebook *) pad)->get_grasped();
};

void activate_callback(Sprite *s) {
   ((Sprite *) s)->activate();
};

void flip_to_back_callback(Sprite *p) {
	((Picture *) p)->flip_to_back();
};

void expand_back_callback(Sprite *p) {
	((Picture *) p)->expand_back();
};

void start_flip_to_front_callback(Sprite *p) {
	((Picture *) p)->start_flip_to_front();
};

void finish_flip_to_front_callback(Sprite *p) {
	((Picture *) p)->finish_flip_to_front();
};

void flip_to_front_finished_callback(Sprite *p) {
   ((Picture *) p)->flip_to_front_finished();
};

void action_completed_callback(Sprite *p) {
  ((Sprite *) p)->action_completed(); // was Picture prior to 171102
};

void add_pages_to_remote_notebook_callback(Sprite *p) {
   ((Picture *) p)->add_pages_to_remote_notebook();
};

void now_flipped_callback(Sprite *p) {
   ((Picture *)p)->now_flipped();
};

void update_text_and_widths_of_controlee_callback(Sprite *p) {
	((Picture *) p)->update_text_and_widths_of_controlee();
};

void reset_programmer_timer_callback(Sprite *p) {
	((Programmer_Sprite *) p)->reset_programmer_timer();
};

void set_active_true_callback(Sprite *martian) {
	((Martian *) martian)->set_active(TRUE);
};

void restore_toolbox_callback(Sprite *s) {
//	((Floor *) p)->restore_toolbox();
   Background *floor = tt_programmer->pointer_to_room()->pointer_to_floor(); // was ((Sprite *) s)->pointer_to_background(); prior to 070103
   floor->restore_toolbox();
};

void use_tool_callback(Sprite *) {
//	stop_postponing_new_logs(); // though use_tool might postpone them again
//	((Programmer_At_Floor *) p)->use_tool();
   Programmer_At_Floor *p = tt_programmer->programmer_at_floor();
   if (p != NULL) {
      p->use_tool();
   }; // else warn??
};

void restore_copier_callback(Sprite *) {
//	((Programmer_At_Floor *) p)->restore_copier();
   Programmer_At_Floor *p = tt_programmer->programmer_at_floor();
   if (p != NULL) {
      p->restore_copier();
   }; // else warn??
};

void expand_thought_bubble2_callback(Sprite *) {
//	((Programmer_At_Floor *) p)->expand_thought_bubble2();
   Programmer_At_Floor *p = tt_programmer->programmer_at_floor();
   if (p != NULL) {
      p->expand_thought_bubble2();
   }; // else warn??
};

void delete_released_callback(Sprite *) {
//  ((Programmer_At_Floor *) p)->delete_released();
   Programmer_At_Floor *p = tt_programmer->programmer_at_floor();
   if (p != NULL) {
      p->delete_released();
   }; // else warn??
};

void tell_programmer_am_ready_callback(Sprite *t) { // new on 220901
	((Sprite *) t)->tell_programmer_am_ready();
};

void add_decoration_callback(Sprite *s) {
   Sprite *sprite = (Sprite *) s;
   Room *room = (Room *) sprite->pointer_to_background();
   sprite->remove_from_floor();
   room->add_decoration(sprite,TRUE,FALSE); // FALSE to see text and number pads on rocket wall
   update_slot_ids(FALSE);
   if (return_puzzle_status() != WAITING_TO_VERIFY_SOLUTION) { // conditional added on 051199 since might be waiting on nest and dump will destroy it (and leave a copy)
		dump_history(TRUE); // in case of a crash then rocket wall shows all prior solutions
	};
};

void appear_working_callback(Sprite *r) {
   ((Robot *) r)->appear_working();
};

void really_send_keyboard_character_callback(Sprite *r) {
	((Robot *) r)->really_send_keyboard_character();
};

void make_visible_and_fit_name_on_chest_callback(Sprite *r) {
	((Robot *) r)->fit_name_on_chest(TRUE,FALSE);
};

void keep_applying_tool_callback(Sprite *r) {
	((Robot *) r)->keep_applying_tool();
};

void really_apply_tool_callback(Sprite *sprite) {
	((Robot *) sprite)->really_apply_tool();
};

void really_select_from_stack_callback(Sprite *sprite) {
	((Robot *) sprite)->really_select_from_stack();
};

void really_release_tool_callback(Sprite *sprite) {
	((Robot *) sprite)->really_release_tool();
};

void really_grasp_tool_callback(Sprite *sprite) {
	((Robot *) sprite)->really_grasp_tool(); // could inline really_grasp_tool
};

void add_to_chest_callback(Sprite *p) {
	((Robot *) p)->add_to_chest();
};

void try_clause_callback(Sprite *robot) {
	((Robot *) robot)->try_clause();
};

void really_try_clause_callback(Sprite *robot) {
//	stop_postponing_new_logs();
	((Robot *) robot)->really_try_clause();
};

void splice_in_line_callback(Sprite *p) {
	((Robot *) p)->splice_in_line();
};

void morph_to_inert_callback(Sprite *p) {
	p->morph_to_inert(); // prior to 300604 used to do a type coercion here
//   tt_error_file() << "morph to inert callback at " << tt_current_time << endl;
};

void speak_callback(Sprite *) {
//	if (tt_debug_mode == 60998) tt_error_file() << "Speak Callback: " << endl;
   speak(return_saved_to_say(),return_saved_id());
};

void turn_on_direct_sound_callback(Sprite *) {
   if (!return_speech_on()) {
      turn_on_direct_sound();
      set_sound_on(TRUE);
   };
};

void turn_on_sound_and_speech_off_callback(Sprite *) {
	turn_on_sound_and_speech_off(0);
};

void signal_size_and_location_callback(Sprite *s) {
	if (s != NULL) { // added for robustness on 151003
		((Sprite *) s)->signal_size_and_location();
#if TT_DEBUG_ON
	} else {
		log("signal_size_and_location_callback on NULL sprite");
#endif
	};
};

void signal_good_size_callback(Sprite *s) { // new on 080702
	((Sprite *) s)->signal_good_size();
};

void animate_to_good_size_callback(Sprite *s) {
	((Sprite *) s)->animate_to_good_size(((Sprite *) s)->default_duration(),tt_log_version_number >= 64?tt_toolbox:NULL);
};

void destroy_callback(Sprite *s) {
	((Sprite *) s)->destroy();
};

void remove_from_floor_and_destroy_callback(Sprite *s) {
	((Sprite *) s)->remove_from_floor();
	((Sprite *) s)->destroy();
};

void just_destroy_callback(Sprite *s) {
	delete (Sprite *) s;  // was already prepared
};

void sink_down_callback(Sprite *s) {
	Sprite *sprite = (Sprite *) s;
	if (sprite->held()) return; // new on 050603 since grabbed before settling down (e.g. F5 F5)
	sprite->set_priority(max_long-2); // toolbox has max_long-1 (-1 as of 111204)
//	((Sprite *) s)->really_propagate_changes(); // new on 021199 so initialization of things like wand button looks right
	sprite->set_clean_status(DIRTY);
};

void remove_from_floor_callback(Sprite *s) {
	((Sprite *) s)->remove_from_floor();
};

void recursively_activate_pictures_callback(Sprite *s) {
   ((Sprite *) s)->recursively_activate_pictures();
};

void vacuum_morph_to_inert_callback(Sprite *vacuum) {
	vacuum->morph_to_inert(); // prior to 300604 used to do a type coercion here
};

void item_restored_callback(Sprite *vacuum) {
	((Vacuum *) vacuum)->item_restored();
};

//void turn_off_unless_mouse_still_down_callback(Sprite *vacuum) {
//   ((Vacuum *) vacuum)->turn_off_unless_mouse_still_down();
//};

void set_being_used_false_callback(Sprite *t) {
//tt_error_file() << tt_current_time << endl; // debuggin
   ((Tool *) t)->set_being_used(FALSE);
};

void set_favorite_size_and_location_callback(Sprite *tool) {
   ((Tool *) tool)->set_favorite_size_and_location();
};

void turn_off_callback(Sprite *t) {
   ((Tool *) t)->turn_off(NULL);
};

void wait_callback(Sprite *expander) {
   ((Expander *) expander)->one_cycle_done();
};

void really_jump_callback(Sprite *sprite) {
	((Expander *) sprite)->really_jump();
};

void jump_to_goal_callback(Sprite *sprite) {
	((Expander *) sprite)->jump_to_goal();
};

void really_jump_to_callback(Sprite *expander) {
  ((Expander *) expander)->really_jump_to();
};

void expander_morph_inert_callback(Sprite *expander) {
	expander->morph_to_inert(); // prior to 300604 used to do a type coercion here
};

void vacuum_is_inert_callback(Sprite *v) {
   ((Vacuum *) v)->is_inert();
};

void make_spitting_sound_callback(Sprite *) {
	play_sound(SPIT_SOUND);
};

void sucking_up_finished_callback(Sprite *vacuum) {
	((Vacuum *) vacuum)->sucking_up_finished();
};

void walk_to_goal_callback(Sprite *sprite) {
	((Vacuum *) sprite)->walk_to_goal();
};

void walk_to_location_callback(Sprite *sprite) {
	((Vacuum *) sprite)->walk_to_location();
};
	
void vacuum_get_grasped_callback(Sprite *sprite) {
	((Vacuum *) sprite)->get_grasped();
};

void copier_get_grasped_callback(Sprite *sprite) {
	((Copier *) sprite)->get_grasped();
};

void toolbox_turn_southeast_callback(Sprite *p) {
	((Toolbox *) p)->set_parameter(SOUTHEAST);
};

void bring_out_vacuum_callback(Sprite *toolbox) {
	((Toolbox *) toolbox)->bring_out_vacuum();
};

void bring_out_copier_callback(Sprite *toolbox) {
	((Toolbox *) toolbox)->bring_out_copier();
};

void bring_out_expander_callback(Sprite *toolbox) {
	((Toolbox *) toolbox)->bring_out_expander();
};

void bring_out_notebook_callback(Sprite *toolbox) {
	((Toolbox *) toolbox)->bring_out_notebook();
};

void toolbox_opened_callback(Sprite *sprite) {
	((Toolbox *) sprite)->opened();
};

void toolbox_turn_vertical_callback(Sprite *sprite) {
	((Toolbox *) sprite)->turn_vertical();
};

void remove_tools_callback(Sprite *sprite) {
	((Toolbox *) sprite)->remove_tools();
};

void toolbox_offstage_callback(Sprite *sprite) {
	((Toolbox *) sprite)->offstage();
};

void add_cubby_callback(Sprite *sprite) {
	((Truck_Inside *) sprite)->add_cubby();
};
void add_robot_callback(Sprite *sprite) {
	((Truck_Inside *) sprite)->add_robot();
};
void add_adding_callback(Sprite *sprite) {
	((Truck_Inside *) sprite)->add_adding();
};

void replace_truck_and_build_callback(Sprite *sprite) {
	((Truck_Inside *) sprite)->replace_truck_and_build();
};

void animate_robot_callback(Sprite *t) {
	((Truck_Inside *) t)->animate_robot();
};

void animate_cubby_callback(Sprite *t) {
	((Truck_Inside *) t)->animate_cubby();
};

void animate_adding_callback(Sprite *t) {
	((Truck_Inside *) t)->animate_adding();
};

void drive_away_and_build_callback(Sprite *t) {
	((Truck_Inside *) t)->drive_away_and_build();
};

void drive_to_corner_activity_callback(Sprite *t) {
  ((Truck_Outside *) t)->start_activity(TRUCK_OUTSIDE_MOVING_TO_CORNER);
};

void drive_to_goal_activity_callback(Sprite *t) {
  ((Truck_Outside *) t)->start_activity(TRUCK_OUTSIDE_MOVING_TO_GOAL);
};

void build_house_callback(Sprite *t) {
	((Truck_Outside *) t)->build_house(FALSE);
};

void leave_since_house_built_callback(Sprite *t) {
  ((Truck_Outside *) t)->leave_since_house_built();
}; 

void destroy_truck_callback(Sprite *t) {
  ((Truck_Outside *) t)->destroy_truck();
};

void now_on_floor_callback(Sprite *s) {
//	((Sprite *) s)->released(NULL);
	((Sprite *) s)->signal_now_on_floor();
};

void initialize_sound_callback(Sprite *) {
   if (initialize_sound()) {
      tt_sound_option = 3; // restore this
   } else {
		tt_sound_option = 0;
	};
};

void stop_postponing_new_logs_callback(Sprite *) {  // obsolete
//	stop_postponing_new_logs();
};

void set_focus_callback(Sprite *) {
	SetFocus(tt_main_window->get_handle());
	UpdateWindow(tt_main_window->get_handle()); // this whole thing doesn't seem to work??
};

void expander_get_grasped_callback(Sprite *sprite) {
	((Expander *) sprite)->get_grasped();
};

void toolbox_exit_stage_right_callback(Sprite *sprite) {
	((Toolbox *) sprite)->exit_stage_right();
};

void play_explosion_callback(Sprite *) {
	// used to be that even if explosion in progress begin anew
	// but sounded bad
	play_sound(EXPLOSION_SOUND); //,FALSE,TRUE); 
};

void set_selection_feedback_callback(Sprite *) {
//   ((Programmer_At_Floor *) p)->set_selection_feedback(TRUE);
   Programmer_At_Floor *p = tt_programmer->programmer_at_floor();
   if (p != NULL) {
      p->set_selection_feedback(TRUE);
   }; // else warn??
};

void remove_call_for_marty_callback(Sprite *) { // new on 040805
	remove_call_for_marty(TRUE);
};

void released_callback(Sprite *s) { // new on 170103
	((Sprite *) s)->released(NULL);
};

void stop_speaking_callback(Sprite *) { // new on 060803
	stop_speaking();
};

void respond_to_stop_character_callback(Sprite *s) {
	((Sprite *) s)->respond_to_stop_character();
};

void forget_saved_subject_callback(Sprite *r) { // new on 280704
	((Robot *) r)->forget_saved_subject();
};

void screen_dirty_callback(Sprite *toolbox) { // new on 250105
	reset_grid();
	if (tt_screen != NULL) {
		tt_screen->screen_dirty();
		tt_screen->remove_current_subtitle();
	};	
};

void cubby_refit_contents_callback(Sprite *toolbox) {
	((Toolbox *) toolbox)->refit_contents();
};

AnimationCallBackProcedure callback_table[] = 
  {NULL,accept_contents_by_bird_callback, accept_contents_by_non_bird_callback, receive_item_again_callback, shrink_to_tiny_callback, fly_back_callback, returned_to_house_callback, 
   shrink_then_returned_to_house_callback, bird_fly_up_callback, note_nest_building_finished_callback, bird_morph_to_inert_callback, bird_morph_to_inert_and_fix_drift_callback, 
   bird_give_me_callback, build_nest_callback, fly_outside_now_full_size_callback, fly_down_callback, fix_t_shirt_callback, send_pending_messages_callback, 
   join_a_direct_play_session_callback, deliver_to_another_house_callback, shrink_then_deliver_to_another_house_callback, show_nest_and_return_callback, nest_is_made_callback,  
   hatch_bird_callback, bird_has_hatched_callback, destroy_item_and_return_callback, grow_by_ten_callback, fly_to_nest_callback, set_down_message_callback, 
   set_down_stack_callback, pick_up_message_callback, pick_up_stack_callback, fly_to_another_house_callback, release_callback, put_stack_back_callback, 
   release_bad_message_callback, return_to_cubby_callback, return_to_cubby_on_flipside_callback, destroy_bird_callback, deliver_to_nowhere_callback, explode_callback, 
   explosion_done_callback, suck_up_callback, animate_suck_up_callback, reset_robot_priority_callback, move_robot_out_of_the_way_callback, 
   house_built_callback, house_destroyed_callback, restore_background_force_callback, teleport_in_done_callback, teleport_out_done_callback, speech_has_finished_callback, 
   place_rest_of_text_callback, next_subtitle_for_closed_caption_alternative_language_callback, teleport_in_callback, marty_speaks_callback, speak_next_sentence_callback, get_smashed_callback, 
   undo_get_smashed_callback, do_operation_callback, undo_do_operation_callback, am_smashed_callback, remove_mouse_callback, done_undoing_callback, 
   run_delayed_call_in_a_mouse_callback, define_page_callback, go_to_items_page_callback, go_to_recent_page_callback, settle_down_callback, finished_settling_down_callback, 
   continue_turn_page_callback, forward_switch_contents_callback, overlay_previous_done_callback, get_grasped_callback, activate_callback, flip_to_back_callback, 
   expand_back_callback, start_flip_to_front_callback, finish_flip_to_front_callback, flip_to_front_finished_callback, action_completed_callback, add_pages_to_remote_notebook_callback, 
   now_flipped_callback, update_text_and_widths_of_controlee_callback, reset_programmer_timer_callback, set_active_true_callback, restore_toolbox_callback, use_tool_callback, 
   restore_copier_callback, expand_thought_bubble2_callback, delete_released_callback, tell_programmer_am_ready_callback, add_decoration_callback, appear_working_callback, 
   really_send_keyboard_character_callback, make_visible_and_fit_name_on_chest_callback, keep_applying_tool_callback, really_apply_tool_callback, really_select_from_stack_callback, really_release_tool_callback, 
   really_grasp_tool_callback, add_to_chest_callback, try_clause_callback, really_try_clause_callback, splice_in_line_callback, morph_to_inert_callback, 
   speak_callback, turn_on_direct_sound_callback, turn_on_sound_and_speech_off_callback, signal_size_and_location_callback, signal_good_size_callback, animate_to_good_size_callback, 
   destroy_callback, remove_from_floor_and_destroy_callback, just_destroy_callback, sink_down_callback, remove_from_floor_callback, recursively_activate_pictures_callback, 
   vacuum_morph_to_inert_callback, item_restored_callback, set_being_used_false_callback, set_favorite_size_and_location_callback, turn_off_callback, 
   wait_callback, really_jump_callback, jump_to_goal_callback, really_jump_to_callback, expander_morph_inert_callback, vacuum_is_inert_callback, 
   make_spitting_sound_callback, sucking_up_finished_callback, walk_to_goal_callback, walk_to_location_callback, vacuum_get_grasped_callback, copier_get_grasped_callback, 
   toolbox_turn_southeast_callback, bring_out_vacuum_callback, bring_out_copier_callback, bring_out_expander_callback, bring_out_notebook_callback, toolbox_opened_callback, 
   toolbox_turn_vertical_callback, remove_tools_callback, toolbox_offstage_callback, add_cubby_callback, add_robot_callback, add_adding_callback, 
   replace_truck_and_build_callback, animate_robot_callback, animate_cubby_callback, animate_adding_callback, drive_away_and_build_callback, drive_to_corner_activity_callback, 
   drive_to_goal_activity_callback, build_house_callback, leave_since_house_built_callback, destroy_truck_callback, now_on_floor_callback, initialize_sound_callback, 
   stop_postponing_new_logs_callback, // obsolete
	set_focus_callback, expander_get_grasped_callback, toolbox_exit_stage_right_callback, play_explosion_callback,
   set_selection_feedback_callback,released_callback,stop_speaking_callback,respond_to_stop_character_callback,
   return_to_floor_callback,forget_saved_subject_callback,screen_dirty_callback,remove_call_for_marty_callback,
   cubby_refit_contents_callback};

AnimationCallBackProcedure callback_procedure(AnimationCallBack index) {
//   if (index == CALLBACK_NOTHING) return(NULL);
#if TT_DEBUG_ON
	if (tt_debug_mode == 70803) {
		tt_error_file() << "Retreiving callback# " << index << " on frame " << tt_frame_number << endl;
	};
#endif
   return(callback_table[index]);
};
