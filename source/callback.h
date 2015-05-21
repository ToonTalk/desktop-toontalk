// Copyright (c) 1992-2002.  Ken Kahn, Animated Programs, All rights reserved

#ifndef __CALLBACK_H

#define __CALLBACK_H

void accept_contents_by_bird_callback(Sprite *sprite);

void accept_contents_by_non_bird_callback(Sprite *sprite);

void receive_item_again_callback(Sprite *bird);

void shrink_to_tiny_callback(Sprite *b);

void fly_back_callback(Sprite *bird);

void returned_to_house_callback(Sprite *bird);

void shrink_then_returned_to_house_callback(Sprite *b);

void bird_fly_up_callback(Sprite *bird);

void note_nest_building_finished_callback(Sprite *bird);

void bird_morph_to_inert_callback(Sprite *bird);

void bird_morph_to_inert_and_fix_drift_callback(Sprite *bird);

void bird_give_me_callback(Sprite *bird);
	
void build_nest_callback(Sprite *bird);

void fly_outside_now_full_size_callback(Sprite *b);

void fly_down_callback(Sprite *b);

void fix_t_shirt_callback(Sprite *b);

#if TT_DIRECT_PLAY
void send_pending_messages_callback(Sprite *nest);
void join_a_direct_play_session_callback(Sprite *nest);
#endif

void deliver_to_another_house_callback(Sprite *b);

void shrink_then_deliver_to_another_house_callback(Sprite *b);

void show_nest_and_return_callback(Sprite *sprite);

void nest_is_made_callback(Sprite *bird);

void hatch_bird_callback(Sprite *n);

void bird_has_hatched_callback(Sprite *nest);

void destroy_item_and_return_callback(Sprite *sprite);

void grow_by_ten_callback(Sprite *sprite);

void fly_to_nest_callback(Sprite *sprite);

void set_down_message_callback(Sprite *sprite);

void set_down_stack_callback(Sprite *sprite);

void pick_up_message_callback(Sprite *sprite);

void pick_up_stack_callback(Sprite *sprite);

void fly_to_another_house_callback(Sprite *sprite);

void release_callback(Sprite *sprite);

void put_stack_back_callback(Sprite *sprite);

void release_bad_message_callback(Sprite *sprite);

void return_to_cubby_callback(Sprite *sprite);

void return_to_cubby_on_flipside_callback(Sprite *sprite);

void destroy_bird_callback(Sprite *sprite);

void deliver_to_nowhere_callback(Sprite *b);

void explode_callback(Sprite *b);

void explosion_done_callback(Sprite *bomb);

void suck_up_callback(Sprite *sprite);

void animate_suck_up_callback(Sprite *c);

//void set_holes_callback(Sprite *sprite);

void reset_robot_priority_callback(Sprite *r);

void move_robot_out_of_the_way_callback(Sprite *robot);

void house_built_callback(Sprite *house_appearance);

void house_destroyed_callback(Sprite *house_appearance);

#if TT_DIRECT_INPUT
void restore_background_force_callback(Sprite *);
#endif

void teleport_in_done_callback(Sprite *m);

void teleport_out_done_callback(Sprite *m);

void speech_has_finished_callback(Sprite *m);

void place_rest_of_text_callback(Sprite *t);

void next_subtitle_for_closed_caption_alternative_language_callback(Sprite *t);

void teleport_in_callback(Sprite *m);

#if TT_TEXT_TO_SPEECH
void marty_speaks_callback(Sprite *);
#endif

void speak_next_sentence_callback(Sprite *tb);
  
void get_smashed_callback(Sprite *p);

void undo_get_smashed_callback(Sprite *p);

void do_operation_callback(Sprite *p);

void undo_do_operation_callback(Sprite *p);

void am_smashed_callback(Sprite *p);

void remove_mouse_callback(Sprite *p);

void done_undoing_callback(Sprite *p);

void run_delayed_call_in_a_mouse_callback(Sprite *x);

void define_page_callback(Sprite *sprite);

void go_to_items_page_callback(Sprite *pad);

void go_to_recent_page_callback(Sprite *pad);

void settle_down_callback(Sprite *pad);

void finished_settling_down_callback(Sprite *pad);

void continue_turn_page_callback(Sprite *pad);

void forward_switch_contents_callback(Sprite *pad);

void overlay_previous_done_callback(Sprite *pad);

void get_grasped_callback(Sprite *pad);

void activate_callback(Sprite *s);

void flip_to_back_callback(Sprite *p);

void expand_back_callback(Sprite *p);

void start_flip_to_front_callback(Sprite *p);

void finish_flip_to_front_callback(Sprite *p);

void flip_to_front_finished_callback(Sprite *p);

void action_completed_callback(Sprite *p);

void add_pages_to_remote_notebook_callback(Sprite *p);

void now_flipped_callback(Sprite *p);

void update_text_and_widths_of_controlee_callback(Sprite *p);

void reset_programmer_timer_callback(Sprite *p);

void set_active_true_callback(Sprite *martian);

void restore_toolbox_callback(Sprite *p);

void use_tool_callback(Sprite *p);

void restore_copier_callback(Sprite *p);

void expand_thought_bubble2_callback(Sprite *p);

void delete_released_callback(Sprite *p);

void tell_programmer_am_ready_callback(Sprite *t);

void add_decoration_callback(Sprite *s);

void appear_working_callback(Sprite *r);

void really_send_keyboard_character_callback(Sprite *r);

void make_visible_and_fit_name_on_chest_callback(Sprite *r);

void keep_applying_tool_callback(Sprite *r);

void really_apply_tool_callback(Sprite *sprite);

void really_select_from_stack_callback(Sprite *sprite);

void really_release_tool_callback(Sprite *sprite);

void really_grasp_tool_callback(Sprite *sprite);

void add_to_chest_callback(Sprite *p);

void try_clause_callback(Sprite *robot);

void really_try_clause_callback(Sprite *robot);

void splice_in_line_callback(Sprite *p);

void morph_to_inert_callback(Sprite *p);

void speak_callback(Sprite *);

void turn_on_direct_sound_callback(Sprite *);

void turn_on_sound_and_speech_off_callback(Sprite *);

void signal_size_and_location_callback(Sprite *s);

void signal_good_size_callback(Sprite *s);

void animate_to_good_size_callback(Sprite *s);

void destroy_callback(Sprite *s);

void remove_from_floor_and_destroy_callback(Sprite *s);

void just_destroy_callback(Sprite *s);

void sink_down_callback(Sprite *s);

void remove_from_floor_callback(Sprite *s);

void recursively_activate_pictures_callback(Sprite *s);

void vacuum_morph_to_inert_callback(Sprite *vacuum);

void item_restored_callback(Sprite *vacuum);

//void turn_off_unless_mouse_still_down_callback(Sprite *vacuum);

void set_being_used_false_callback(Sprite *t);

void set_favorite_size_and_location_callback(Sprite *tool);

void turn_off_callback(Sprite *t);

void wait_callback(Sprite *expander);

void really_jump_callback(Sprite *sprite);

void jump_to_goal_callback(Sprite *sprite);

void really_jump_to_callback(Sprite *expander);

void expander_morph_inert_callback(Sprite *expander);

void vacuum_is_inert_callback(Sprite *v);

void make_spitting_sound_callback(Sprite *);

void sucking_up_finished_callback(Sprite *vacuum);

void walk_to_goal_callback(Sprite *sprite);

void walk_to_location_callback(Sprite *sprite);
	
void vacuum_get_grasped_callback(Sprite *sprite);

void copier_get_grasped_callback(Sprite *sprite);

void toolbox_turn_southeast_callback(Sprite *p);

void bring_out_vacuum_callback(Sprite *toolbox);

void bring_out_copier_callback(Sprite *toolbox);

void bring_out_expander_callback(Sprite *toolbox);

void bring_out_notebook_callback(Sprite *toolbox);

void toolbox_opened_callback(Sprite *sprite);

void toolbox_turn_vertical_callback(Sprite *sprite);

void remove_tools_callback(Sprite *sprite);

void toolbox_offstage_callback(Sprite *sprite);

void add_cubby_callback(Sprite *sprite);
void add_robot_callback(Sprite *sprite);
void add_adding_callback(Sprite *sprite);

void replace_truck_and_build_callback(Sprite *sprite);

void animate_robot_callback(Sprite *t);

void animate_cubby_callback(Sprite *t);

void animate_adding_callback(Sprite *t);

void drive_away_and_build_callback(Sprite *t);

void drive_to_corner_activity_callback(Sprite *t);

void drive_to_goal_activity_callback(Sprite *t);

void build_house_callback(Sprite *t);

void leave_since_house_built_callback(Sprite *t); 

void destroy_truck_callback(Sprite *t);

void now_on_floor_callback(Sprite *s);

void initialize_sound_callback(Sprite *);

void stop_postponing_new_logs_callback(Sprite *); // obsolete

void set_focus_callback(Sprite *);

void expander_get_grasped_callback(Sprite *sprite);

void toolbox_exit_stage_right_callback(Sprite *sprite);

extern AnimationCallBackProcedure tt_callback[];

enum AnimationCallBack
  {CALLBACK_NOTHING=0,ACCEPT_CONTENTS_BY_BIRD_CALLBACK, ACCEPT_CONTENTS_BY_NON_BIRD_CALLBACK, RECEIVE_ITEM_AGAIN_CALLBACK, SHRINK_TO_TINY_CALLBACK, FLY_BACK_CALLBACK, RETURNED_TO_HOUSE_CALLBACK, 
   SHRINK_THEN_RETURNED_TO_HOUSE_CALLBACK, BIRD_FLY_UP_CALLBACK, NOTE_NEST_BUILDING_FINISHED_CALLBACK, BIRD_MORPH_TO_INERT_CALLBACK, BIRD_MORPH_TO_INERT_AND_FIX_DRIFT_CALLBACK, 
   BIRD_GIVE_ME_CALLBACK, BUILD_NEST_CALLBACK, FLY_OUTSIDE_NOW_FULL_SIZE_CALLBACK, FLY_DOWN_CALLBACK, FIX_T_SHIRT_CALLBACK, SEND_PENDING_MESSAGES_CALLBACK, 
   JOIN_A_DIRECT_PLAY_SESSION_CALLBACK, DELIVER_TO_ANOTHER_HOUSE_CALLBACK, SHRINK_THEN_DELIVER_TO_ANOTHER_HOUSE_CALLBACK, SHOW_NEST_AND_RETURN_CALLBACK, NEST_IS_MADE_CALLBACK, 
   HATCH_BIRD_CALLBACK, BIRD_HAS_HATCHED_CALLBACK, DESTROY_ITEM_AND_RETURN_CALLBACK, GROW_BY_TEN_CALLBACK, FLY_TO_NEST_CALLBACK, SET_DOWN_MESSAGE_CALLBACK, 
   SET_DOWN_STACK_CALLBACK, PICK_UP_MESSAGE_CALLBACK, PICK_UP_STACK_CALLBACK, FLY_TO_ANOTHER_HOUSE_CALLBACK, RELEASE_CALLBACK, PUT_STACK_BACK_CALLBACK, 
   RELEASE_BAD_MESSAGE_CALLBACK, RETURN_TO_CUBBY_CALLBACK, RETURN_TO_CUBBY_ON_FLIPSIDE_CALLBACK, DESTROY_BIRD_CALLBACK, DELIVER_TO_NOWHERE_CALLBACK, EXPLODE_CALLBACK, 
   EXPLOSION_DONE_CALLBACK, SUCK_UP_CALLBACK, ANIMATE_SUCK_UP_CALLBACK, RESET_ROBOT_PRIORITY_CALLBACK, MOVE_ROBOT_OUT_OF_THE_WAY_CALLBACK, 
   HOUSE_BUILT_CALLBACK, HOUSE_DESTROYED_CALLBACK, RESTORE_BACKGROUND_FORCE_CALLBACK, TELEPORT_IN_DONE_CALLBACK, TELEPORT_OUT_DONE_CALLBACK, SPEECH_HAS_FINISHED_CALLBACK, 
   PLACE_REST_OF_TEXT_CALLBACK, NEXT_SUBTITLE_FOR_CLOSED_CAPTION_ALTERNATIVE_LANGUAGE_CALLBACK, TELEPORT_IN_CALLBACK, MARTY_SPEAKS_CALLBACK, SPEAK_NEXT_SENTENCE_CALLBACK, GET_SMASHED_CALLBACK, 
   UNDO_GET_SMASHED_CALLBACK, DO_OPERATION_CALLBACK, UNDO_DO_OPERATION_CALLBACK, AM_SMASHED_CALLBACK, REMOVE_MOUSE_CALLBACK, DONE_UNDOING_CALLBACK, 
   RUN_DELAYED_CALL_IN_A_MOUSE_CALLBACK, DEFINE_PAGE_CALLBACK, GO_TO_ITEMS_PAGE_CALLBACK, GO_TO_RECENT_PAGE_CALLBACK, SETTLE_DOWN_CALLBACK, FINISHED_SETTLING_DOWN_CALLBACK, 
   CONTINUE_TURN_PAGE_CALLBACK, FORWARD_SWITCH_CONTENTS_CALLBACK, OVERLAY_PREVIOUS_DONE_CALLBACK, GET_GRASPED_CALLBACK, ACTIVATE_CALLBACK, FLIP_TO_BACK_CALLBACK, 
   EXPAND_BACK_CALLBACK, START_FLIP_TO_FRONT_CALLBACK, FINISH_FLIP_TO_FRONT_CALLBACK, FLIP_TO_FRONT_FINISHED_CALLBACK, ACTION_COMPLETED_CALLBACK, ADD_PAGES_TO_REMOTE_NOTEBOOK_CALLBACK, 
   NOW_FLIPPED_CALLBACK, UPDATE_TEXT_AND_WIDTHS_OF_CONTROLEE_CALLBACK, RESET_PROGRAMMER_TIMER_CALLBACK, SET_ACTIVE_TRUE_CALLBACK, RESTORE_TOOLBOX_CALLBACK, USE_TOOL_CALLBACK, 
   RESTORE_COPIER_CALLBACK, EXPAND_THOUGHT_BUBBLE2_CALLBACK, DELETE_RELEASED_CALLBACK, TELL_PROGRAMMER_AM_READY_CALLBACK, ADD_DECORATION_CALLBACK, APPEAR_WORKING_CALLBACK, 
   REALLY_SEND_KEYBOARD_CHARACTER_CALLBACK, MAKE_VISIBLE_AND_FIT_NAME_ON_CHEST_CALLBACK, KEEP_APPLYING_TOOL_CALLBACK, REALLY_APPLY_TOOL_CALLBACK, REALLY_SELECT_FROM_STACK_CALLBACK, REALLY_RELEASE_TOOL_CALLBACK, 
   REALLY_GRASP_TOOL_CALLBACK, ADD_TO_CHEST_CALLBACK, TRY_CLAUSE_CALLBACK, REALLY_TRY_CLAUSE_CALLBACK, SPLICE_IN_LINE_CALLBACK, MORPH_TO_INERT_CALLBACK, 
   SPEAK_CALLBACK, TURN_ON_DIRECT_SOUND_CALLBACK, TURN_ON_SOUND_AND_SPEECH_OFF_CALLBACK, SIGNAL_SIZE_AND_LOCATION_CALLBACK, SIGNAL_GOOD_SIZE_CALLBACK, ANIMATE_TO_GOOD_SIZE_CALLBACK, 
   DESTROY_CALLBACK, REMOVE_FROM_FLOOR_AND_DESTROY_CALLBACK, JUST_DESTROY_CALLBACK, SINK_DOWN_CALLBACK, REMOVE_FROM_FLOOR_CALLBACK, RECURSIVELY_ACTIVATE_PICTURES_CALLBACK, 
   VACUUM_MORPH_TO_INERT_CALLBACK, ITEM_RESTORED_CALLBACK, SET_BEING_USED_FALSE_CALLBACK, SET_FAVORITE_SIZE_AND_LOCATION_CALLBACK, TURN_OFF_CALLBACK, 
   WAIT_CALLBACK, REALLY_JUMP_CALLBACK, JUMP_TO_GOAL_CALLBACK, REALLY_JUMP_TO_CALLBACK, EXPANDER_MORPH_INERT_CALLBACK, VACUUM_IS_INERT_CALLBACK, 
   MAKE_SPITTING_SOUND_CALLBACK, SUCKING_UP_FINISHED_CALLBACK, WALK_TO_GOAL_CALLBACK, WALK_TO_LOCATION_CALLBACK, VACUUM_GET_GRASPED_CALLBACK, COPIER_GET_GRASPED_CALLBACK, 
   TOOLBOX_TURN_SOUTHEAST_CALLBACK, BRING_OUT_VACUUM_CALLBACK, BRING_OUT_COPIER_CALLBACK, BRING_OUT_EXPANDER_CALLBACK, BRING_OUT_NOTEBOOK_CALLBACK, TOOLBOX_OPENED_CALLBACK, 
   TOOLBOX_TURN_VERTICAL_CALLBACK, REMOVE_TOOLS_CALLBACK, TOOLBOX_OFFSTAGE_CALLBACK, ADD_CUBBY_CALLBACK, ADD_ROBOT_CALLBACK, ADD_ADDING_CALLBACK, 
   REPLACE_TRUCK_AND_BUILD_CALLBACK, ANIMATE_ROBOT_CALLBACK, ANIMATE_CUBBY_CALLBACK, ANIMATE_ADDING_CALLBACK, DRIVE_AWAY_AND_BUILD_CALLBACK, DRIVE_TO_CORNER_ACTIVITY_CALLBACK, 
   DRIVE_TO_GOAL_ACTIVITY_CALLBACK, BUILD_HOUSE_CALLBACK, LEAVE_SINCE_HOUSE_BUILT_CALLBACK, DESTROY_TRUCK_CALLBACK, NOW_ON_FLOOR_CALLBACK, INITIALIZE_SOUND_CALLBACK, 
   STOP_POSTPONING_NEW_LOGS_CALLBACK, // obsolete
	SET_FOCUS_CALLBACK, EXPANDER_GET_GRASPED_CALLBACK, TOOLBOX_EXIT_STAGE_RIGHT_CALLBACK, PLAY_EXPLOSION_CALLBACK,
   SET_SELECTION_FEEDBACK_CALLBACK, RELEASED_CALLBACK, STOP_SPEAKING_CALLBACK, RESPOND_TO_STOP_CHARACTER_CALLBACK,
	RETURN_TO_FLOOR_CALLBACK, FORGET_SAVED_SUBJECT_CALLBACK, SCREEN_DIRTY_CALLBACK, REMOVE_CALL_FOR_MARTY_CALLBACK,
	CUBBY_REFIT_CONTENTS_CALLBACK
   };

AnimationCallBackProcedure callback_procedure(AnimationCallBack index);

#endif
