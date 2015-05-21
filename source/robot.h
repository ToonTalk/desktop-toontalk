// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.
#ifndef __TT_ROBOT_H

#define __TT_ROBOT_H

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_TOOLS_H)
#include "tools.h"
#endif   
#if !defined(__TT_PAD_H)   
#include "pad.h"
#endif   
#if !defined(__TT_THOUGHT_H)   
#include "thought.h"
#endif
#if !defined(__TT_PRGRMMR_H)
#include "prgrmmr.h"
#endif

class Sprite;
class Screen;
class Events;
class Background;
class Cubby;
class Sprite;
class Bird;
//class Programmer;
class Truck_Inside;

const unsigned char NO_TRAINING_COUNTER = 255; // 0 would have been a much better value -- too late now (noticed 051004)

class Page_Number_Offsets {
  public:
	 Page_Number_Offsets() :
		count(0),
		notebooks(NULL),
		offsets(NULL) {};
	 ~Page_Number_Offsets();
	 void update(Notebook *notebook);
	 int page_number_offset(Notebook *notebook);
#if TT_XML
	 // following new on 161202
	 int return_count() { 
		 return(count);
	 };
	 Notebook *notebook(int i) { 
		 return(notebooks[i]); // callers should be careful
	 };
	 int offset(int i) { 
		 return(offsets[i]);
	 };
	 Page_Number_Offsets(int c);
	 void set_page_offset(int i, int offset, Notebook *notebook);
#endif
	private:
	  int count;
	  NotebookPointer *notebooks; // arrays of notebooks
	  int *offsets;
};

class Robot : public Sprite {
   public:
     Robot(city_coordinate x=0, city_coordinate y=0,
           long initial_priority=0,
//         Background *floor,
			  city_coordinate width=0, city_coordinate height=0,
//			  Thought_Bubble *thought_bubble=NULL,
			  Events *body=NULL, 
			  int max_working_set_size=default_max_working_set_size); // should only be defaulted for fresh robots -- can grow
//			  Robot *next_robot=NULL);
	  void prepare_for_deletion();
	  void release_all_references();
	  Sprite *copy(boolean also_copy_offsets=FALSE);
//	  boolean ok_to_delete();
     void no_longer_suspended();
     void remove_suspension() { // called when nest is being destroyed
		  if (suspended_on != NULL) suspended_on->destroy(this); // new on 120707 to maintain ref counts
        suspended_on = NULL;
     };
	  boolean suspended_on_something() { // new on 190405
		  return(suspended_on != NULL);
	  };
	  boolean any_teammate_suspended_on_something(); // new on 190405
     MatchStatus match(Robot *other_robot, int version);
	  void generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection);
     Sprites *reachable_sprites_of_type(SpriteType type);
	  Sprites *reachable_non_equal_sprites_of_type(SpriteType type);
     string java_start(Cubby *cubby=NULL, Text *text=NULL, Notebook *notebook=NULL, Picture *picture=NULL,
                       Sprites *more_robots_for_picture=NULL, Sprites *more_notebooks=NULL, Sprite *display_this=NULL, 
							  java_string extra_text=NULL);
     void java_class(java_stream &out);
     void java_after(java_stream &out, const_java_string robot_name);
     void java_team(java_stream &out, Cubby *cubby);
	  Cubby *restored_cubby(Cubby *cubby);
     void java_before(java_stream &out);
     void java(java_stream &out);
     Robot *previous_robot(Robot *me);
//   boolean is_on_back_of(Picture *picture);
     void describe(output_stream &message, Article article);
	  void really_grasp_tool();
	  void really_apply_tool();
	  void keep_applying_tool();
	  void remove_from_cubby();
	  void really_select_from_stack();
	  void really_release_tool();
	  void send_keyboard_character(millisecond duration,
											 int character, // was char prior to 100604
											 Sprite *subject,
											 boolean label_character,
											 boolean select,
                                  int label_index,
											 boolean next_event_send_character_to_same_subject);
	  void really_send_keyboard_character();
	  void stop_team(SelectionReason reason=NO_SELECTION_REASON_GIVEN);
	  void move_to_side_of_box(millisecond duration);
	  void forget_tool();
	  void stop_all(boolean for_good=FALSE, boolean suspend_robots_too=TRUE); 
	  void stop_running_body();
	  void give_up();
//	  void propagate_changes();
//	  void update_display();
	  void splice_in_line();
//	  void add_saved_follower();
//	  Background *enter_thought_bubble(Thought_Bubble *bubble,
//																Screen *screen);
	  void connect_to_thought_bubble(Thought_Bubble *new_thought_bubble,
												boolean move_bubble=TRUE);
     void add_initial_tool(Sprite *sprite); //, millisecond duration=0);
	  void follower_changed(Sprite *follower);
	  boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
	  void set_priority(long new_priority);
     boolean save_region();
	  void set_size_and_location(city_coordinate width,
										  city_coordinate height,
										  city_coordinate llx,
										  city_coordinate lly,
										  ChangeReason reason=NO_REASON_GIVEN);
     void signal_size_and_location();
	  Thought_Bubble *pointer_to_thought_bubble() {
		 return(thought_bubble);
	  };
	  void set_body(Events *events);
	  SpritePointer *working_set_and_size(int &working_set_length_ref) {
		  working_set_length_ref = working_set_length;
		  return(working_set);
	  };
	  void set_max_working_set_size(int new_max_working_set_size);
//	  void remove_last_n_working_set_items(int count) {
//		  working_set_length -= count; // do I need to NULL them?
//	  };
	  Events *pointer_to_body() {
		  return(body);
	  };
	  boolean fully_defined() {
		  return(thought_bubble != NULL && body != NULL);
	  };
	  void try_clause(Cubby *cubby=NULL, millisecond delay=0);
	  void really_try_clause();
	  void last_robot_failed();
	  void associate_with_box(Cubby *cubby);
	  void set_working_cubby(Cubby *cubby);
//	  boolean any_touched_a_remote();
	  void move_to_side(boolean instantly=FALSE, boolean frustrated=TRUE);
	  void move_to_waiting_position(Cubby *cubby);
	  void update_priority();
	  boolean put_behind_me(Robot *robot_behind, millisecond duration=0);
	  boolean add_to_end_of_line(millisecond duration=0);
	  void pretend_add_to_end_of_line_completed();
	  boolean any_teammate_running();
	  boolean next_teammate_running();
	  void grasp_tool(Sprite *new_tool, Sprite *target, int target_index, millisecond after);
	  void pick_up_and_store_tool(Sprite *tool);
	  void regrasp_tool();
	  void add_to_chest();
	  void resize_to_fit_chest(Sprite *sprite);
	  void begin_drop_action();
	  void drop_tool();
     void swap_tool(Sprite *new_tool);
	  void apply_tool(Sprite *subject,
							Sprite *old_target, int target_index,
							millisecond not_before,
							char key);
	  void select_from_stack(Sprite *stack, millisecond not_before);
	  void release_tool(millisecond not_before,
							  Sprite *target=NULL, int target_index=0,
							  boolean new_released_on_its_stack=FALSE);
	  //void release_tool(millisecond not_before, Bird *bird) {
		 // receiving_bird = bird;
		 // release_tool(not_before,NULL,0,FALSE); // removed new_action_status,
	  //};
	  //void release_tool(millisecond not_before, Truck_Inside *truck) {
		 // receiving_truck = truck;
		 // release_tool(not_before,NULL,0,FALSE); 
	  //};
   //  void release_tool(millisecond not_before) {
		 // release_tool(not_before,NULL,0,FALSE); 
	  //};
	  Sprite *select(TTRegion *region, SelectionReason reason, Sprite *in_hand);
	  Sprite *selected(SelectionReason reason, Sprite *by);
	  boolean wand_copy_ok_as_initial_tool(Sprite *item);
//	  Sprite *appearance_before_vacuuming();
//	  void reset_appearance_before_vacuuming();
	  Sprite *released(Sprite *by, boolean top_level=TRUE, SpecialDrop special_drop=NOT_SPECIAL_DROP);
	  void unselect();
	  void now_on_floor(Background *floor, Sprite *by);
	  void line_up(Background *floor, Sprite *by,  millisecond duration=0);
     Sprite *respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected,
                                 Sprite *by, boolean record_action, Sprite *original_recipient,
                                 boolean size_constrained_regardless=FALSE);
	  ArrowKeyEditing arrow_key_editing_status();
	  void abort_action_if_robot(int reason, 
                                SpriteType actor=NONE_GIVEN,
										  boolean explain_anyway=FALSE,
                                int importance=-1,
										  string default_string="");
	  void abort_action(int reason,
     					     SpriteType actor=NONE_GIVEN,
                       int importance=-1, string default_string="");
	  void detach(Robot *other, Background *floor);
	  void done_running_body(boolean keep_going, boolean stop_team_too=TRUE);
	  ReleaseStatus item_released_on_top(Sprite *item, Sprite *by, SpritePointer &recipient, int level); 
	  boolean receive_item(Sprite *item, Sprite *by, millisecond duration,
                          Sprite *original_recipient, Sprite *original_item);
	  void set_to_future_value(Sprite *other, Sprite *original_recipient, Sprite *by); 
//	  void insert_at_end(Robot *another_robot);
//	  void move_tool(); // for running off screen in event.cpp
	  void add_attached_to_working_set();
	  int add_to_working_set(Sprite *item, boolean might_already_be_there);
	  void remove_tool_from_working_set(Sprite *item=NULL, Sprite *replacement=NULL);
	  void just_remove_tool_from_working_set(Sprite *item);
     void remove_last_working_set_item();
//     void update_watched_status() {
//      watched_at_start_flag = showing_on_a_screen();
//     };
//     boolean watched_at_start() {
//      return(watched_at_start_flag);
//     };
//     boolean watched_at_start_but_not_now() {
//      return(watched_at_start_flag && !showing_on_a_screen());
//     };
//     void absorb_thought_bubble();
	  Path *path_to(Sprite *target, boolean &ok);
	  boolean holding(Sprite *item);
//     boolean watched() {
//       return(watched_flag);
//     };
	  boolean holding_tool() {
		 return(tool != NULL);
	  };
//     void record_tool_release(Sprite *tool, Screen *screen);
	  // noticed on 131202 that the following are obsolete
	  //void save_tool_coordinates(city_coordinate tool_llx, city_coordinate tool_lly) {
		 //saved_tool_llx = tool_llx;
		 //saved_tool_lly = tool_lly;
	  //};
	  //void saved_tool_coordinates(city_coordinate &tool_llx, city_coordinate &tool_lly) {
		 //tool_llx = saved_tool_llx;
		 //tool_lly = saved_tool_lly;
	  //};
	  void delta_for_robot_behind_me(city_coordinate &delta_x, city_coordinate &delta_y);
	  void location_behind_me(city_coordinate &x, city_coordinate &y);
	  void pointer_region(TTRegion &region);
	  void chest_size(city_coordinate &chest_width,
							city_coordinate &chest_height);
	  void chest_offsets(city_coordinate &delta_x, city_coordinate &delta_y);
	  void reset_first_in_line(Robot *first); 
	  void set_first_in_line(Robot *first);
	  Robot *pointer_to_first_in_line() {
		  return(first_in_line);
	  };
	  Robot *team_leader();
	  Robot *pointer_to_next_robot();
	  void set_next_robot(Robot *new_next, boolean put_behind_too=TRUE);
	  void just_set_next_robot(Robot *new_next) {
		  next_robot = new_next;
	  };
     void set_saved_next_robot(Robot *next);
	  void set_robot_to_put_behind(Robot *robot);
	  //void set_robot_to_put_behind(Robot *robot) { // new on 051204 -- updated on 230105 to deal with reference counts
		 // robot_to_put_behind = robot;
	  //};
//	  Sprites *pointer_to_working_set() {
//		  return(working_set);
//	  };
	  boolean grasp_copier(); 
	  boolean grasp_vacuum(); 
  	  boolean grasp_expander(); 
//	  void release_vacuum_or_copier();
	  Vacuum *pointer_to_vacuum(boolean &new_vacuum);
	  Copier *pointer_to_copier(boolean &new_copier);
     Expander *pointer_to_expander(boolean &new_expander);
	  void grasp_special_tool(boolean resize_to_fit);
	  Sprite *pointer_to_tool() {
		  return(tool);
	  };
//	  void delete_when_deleted(Sprite *sprite);
	  void destroy_house();
//	  void full_size_and_location(city_coordinate &full_width,
//										  city_coordinate &full_height,
//										  city_coordinate &full_llx,
//										  city_coordinate &full_lly,
//										  boolean ignore_followers_completely_inside_flag=FALSE);
	  void adjust_release_coordinates(city_coordinate &subject_llx, city_coordinate &subject_lly);
	  void shift_viewpoint(city_coordinate delta_x, city_coordinate delta_y);
	  void next_set_down_location(city_coordinate &subject_llx,	city_coordinate &subject_lly);
	  void location_to_apply_tool(city_coordinate target_x, city_coordinate target_y,
											city_coordinate &new_llx, city_coordinate &new_lly);
	  void display(SelectionFeedback selection_feedback=NO_FEEDBACK,
						boolean followers_too=TRUE, TTRegion *region=NULL);
     string steps_left_string();
	  Notebook *notebook_to_use(short int page_number);
     void starting_event();
     void last_event();
//	  void recursively_propagate_changes();
//     boolean *pointer_to_try_clause_when() {
//       return(&true_when_ready_to_try_clause);
//     };
//     void update_display();
	//flag *pointer_to_body_started() { // made obsolete on 171102
	//  return(&body_started);
	//};
   boolean body_started() {
      return(body_started_flag);
   };
   void set_body_started(boolean new_flag) {
      body_started_flag = (flag) new_flag;
   };
	void set_in_notebook(Notebook *notebook, boolean recur=TRUE);
	int page_number_offset(Notebook *notebook);
//	flag *pointer_to_true_when_body_finished() {
//		return(&true_when_body_finished);
//	};
	//ActionStatus *pointer_to_body_status() { // made obsolete on 171102
	//	return(&body_status);
	//};
   void completed() {
      body_status--;
   };
   void add_completion() { // something extra to complete before whole is completed
      body_status++;
   };
   void reset_action_status() {
      body_status = 1;
   };
   void action_aborted() {
      body_status = min_long; // was -1 but I now allow extra calls to completed() to be no-ops
   };
   boolean is_completed() {
      return(body_status <= 0 && body_status != min_long);
   };
   boolean is_action_aborted() {
      return(body_status == min_long);
   };
   boolean is_action_in_progress() {
      return(body_status > 0);
   };
	boolean is_action_in_progress_of_any_teammate(); // new on 180405
	boolean is_any_teammate_still_running(); // new on 180405
//	void colliding_with(Sprite *other, Screen *screen);
	//void set_programmer(Programmer *new_programmer) { // made obsolete on 131202
	//	programmer = new_programmer;
	//};
	void morph_to_life();
   void morph_to_inert_all();
	void morph_to_inert();
   boolean is_inert();
	void appear_working();
	boolean still_running() {
	  return(running_body_flag);
	};
	void set_running_body_entire_team(boolean flag);
	void set_running_body(boolean new_flag) {
		running_body_flag = (flag) new_flag;
	};
	Sprite *working_set_nth(int i); 
	void set_thought_bubble(Thought_Bubble *new_thought_bubble);
	//boolean inside_sprite() {
	//	 return(inside_sprite_flag);
	//};
//	void set_inside_sprite(boolean new_flag);
//	boolean encountered_remotes() {
//		return(encountered_remotes_flag);
//	};
//	void set_encountered_remotes(boolean new_flag) {
//		encountered_remotes_flag = (flag) new_flag;
//	};
//	boolean touched_a_remote() {
//		return(touched_a_remote_flag);
//	};
//	void set_touched_a_remote(boolean new_flag) {
//		touched_a_remote_flag = (flag) new_flag;
//	};
	boolean waiting_for_release() {
		// on 040500 restored this since this flag is still used (though with work probably could be rationalized)
		return(waiting_for_release_flag); // && tt_log_version_number < 22); // on 290300 made waiting_for_release() obsolete
	};
	void set_waiting_for_release(boolean new_flag)
#if !TT_DEBUG_ON
		{
			waiting_for_release_flag = (flag) new_flag;
			}
#endif
			;
	void suspend();
   void activate(boolean reset_animation=TRUE, PictureActivateContext context=PICTURE_ACTIVATE_CONTEXT_NOT_INTIALIZED);
	boolean suspended() {
		return(suspended_flag);
	};
	void set_suspended(boolean new_flag) {
#if TT_DEBUG_ON
		if (debug_counter == tt_debug_target) {
			tt_error_file() << "Setting suspension of debug target (robot) to " << (int) new_flag << endl;
		};
#endif
		suspended_flag = (flag) new_flag;
	};
	void set_suspended_entire_team(boolean flag);
	Cubby *pointer_to_working_cubby() {
		return(top_cubby);
	};
//	void set_working_cubby(Cubby *new_cubby) {
//		top_cubby = new_cubby;
//	};
	void dont_run_again_this_cycle() {
		run_again_this_cycle = FALSE;
	};
	short int return_recent_page_number(Notebook *notebook);
	void set_recent_page_number(short int page_number, Notebook *notebook);
	void reset_recent_page_numbers(); // boolean for_good);
	void reset_main_notebook_page_numbers();
	void notebook_just_flew_out(Notebook *notebook);
	void set_home_notebook(Notebook *notebook);
	Notebook *pointer_to_home_notebook() {
		return(home_notebook);
	};
   void set_team_notebook(Notebook *notebook);
//   unsigned char return_training_counter() {
//     return(training_counter);
//   };
   boolean exhausted_training_counter() {
     return(training_counter == 0);
   };
   boolean decrement_training_counter();
   void increment_training_counter();
   void set_training_counter(unsigned char counter) {
     training_counter = counter;
	  original_training_counter = counter;
   };
	void restore_original_training_counter() {
		training_counter = original_training_counter;
	};
   java_string java_class_name();
   java_string java_name();
	java_string rest_of_java_name(java_string name);
	void comment_java_name(java_stream &out);
   void name(string name);
   unsigned short serial_number();
   unsigned long body_hash();
//     return(serial_number);
//   };
//   void set_serial_number(unsigned short number) {
//      serial_number = number;
//   };
   int robots_in_team();
   int place_in_team(); 
   Sprite *pointer_to_initial_tool() {
      return(initial_tool);
   };
   void set_initial_tool(Sprite *tool);
   void forget_tool_dimensions() {
      tool_width = 0;
      tool_height = 0;
   };
   Sprite *pointer_to_ultimate_item();
	boolean blankable() {
		return(TRUE); 
	};
	boolean current_value(NumberValuePointer &the_value, boolean &shared, Sprite *by=NULL);
	void become_blank(boolean by_user=FALSE, boolean controlee_too=TRUE);
	void become_non_blank(boolean controlee_too=TRUE);
	void say_a_robot_named(int before, int after, int importance=3);
	string return_a_robot_named(int before_id, int after_id);
	void set_string_name_from_user(string new_name);
	void set_name_from_user(Text *new_name);
	void attach_name_from_user();
	Text *return_name_from_user() { // new on 060601
		return(name_from_user);
	};
	string copy_string_name_from_user();
	void fit_name_on_chest(boolean make_visible_too=FALSE, boolean update_display_too=TRUE);
	boolean set_parameter(short int new_parameter, boolean warn=TRUE,
                         boolean reset_cycle_regardless=FALSE); // boolean loading=FALSE);
	Robot *first_team_member_to_fail_for_unstable_reasons();
	void set_first_team_member_to_fail_for_unstable_reasons(Robot *robot);
	Robot *pointer_to_first_to_fail_for_unstable_reasons() {
		return(first_to_fail_for_unstable_reasons);
	};
	void set_first_to_fail_for_unstable_reasons(Robot *robot);
	boolean last_failure_was_for_unstable_reasons() {
		return(last_failure_was_for_unstable_reasons_flag);
	};
	void set_last_failure_was_for_unstable_reasons(boolean new_flag) {
		last_failure_was_for_unstable_reasons_flag = (flag) new_flag;
	};
	void set_background(Background *new_background, boolean recur=FALSE, boolean warn=TRUE);
	void set_remove_me_from_backside(boolean new_flag) {
		remove_me_from_backside_flag = (flag) new_flag;
	};
	boolean remove_me_from_backside() {
		return(remove_me_from_backside_flag);
	};
	void set_team_running(boolean new_flag);
	boolean team_running();
	Sprite *tool_to_drop();
	void dump_working_set(output_stream &stream);
	void load_working_set(InputStream *pad_in, int version_number, NaturalLanguage language, boolean ignore=FALSE);
	void dump_robot_flags(output_stream &stream);
	void load_robot_flags(InputStream *stream, int version_number, NaturalLanguage language, boolean ignore=FALSE);
	void set_working_set_element(int i, Sprite *item);
	void release_working_set_elements();
	void set_inside_vacuum(boolean new_flag, boolean recur=TRUE);
	boolean ok_to_delete_bammed();
	int version_number();
	void add_outside_followers_to_screen();
	boolean equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, 
		              SpritesPointer &expected_corresponding_nests, 
						  EqualReason reason=NORMAL_EQUAL);
#if TT_XML
//    xml_element *xml_create_element(xml_document *document);
    void xml(xml_element *element, xml_document *document);
	 void xml_working_set(xml_element *parent, xml_document *document);
	 xml_element *xml_set_special_attributes(xml_element *element, xml_document *document);
	 boolean xml_get_special_attributes(xml_node *node);
	 void add_special_xml(xml_element *element, xml_document *document);
    boolean handle_xml_tag(Tag tag, xml_node *node);
    Events *xml_events(xml_node *node);
    boolean handle_working_info(Tag tag, xml_node *node);
    boolean handle_working_set_element(Tag tag, xml_node *node);
	 void initialize_tool(Sprite *item);
	 short int default_parameter() { // new on 250103 - only used for dumping XML now
	    return(11);
	 };
	 wide_string xml_tag() { // new on 090703
		 return(L"Robot"); 
	 };
	 //boolean ok_to_save_in_include_file() {
		// return(!body_started_flag); // not if running -- since changing too frequently
	 //};
#endif
    Thought_Bubble *new_thought_bubble(boolean connect_now, Thought_Bubble *thought_bubble=NULL);
    void action_completed();
	 void finish_instantly(boolean callbacks_only=FALSE, boolean ignore_city_stopped=FALSE); // new on 260803
	 void reset_drop_scroll_deltas();
	 void set_version(int new_version) {
		 version = new_version;
	 };
	 int return_version() { // new on 110604
		 return(version);
	 };
	 boolean can_send_keyboard_editing_keys() { // new on 110604
		 return(version > 2);
	 };
	 void forget_saved_subject();
	 boolean name_was_generated() { // new on 260105
		 return(name_from_serial_number);
	 };
#if TT_DEBUG_ON
	  boolean does_follower_matter(Sprite *sprite);
#endif
   //#if TT_DEBUG_ON
//   void set_body_version_number(unsigned char version) {
//      body_version_number = version;
//   };
//	unsigned char return_body_version_number() {
//		return(body_version_number);
//	};
//#endif
  private:
	 Thought_Bubble *thought_bubble;
	 Events *body;
	 Sprite *tool;
    Sprite *initial_tool;
	 Sprite *target;
//	 Bird *receiving_bird; -- made obsolete on 151202
//	 Truck_Inside *receiving_truck; -- made obsolete on 151202
	 int target_index;
	 Sprite **working_set;
	 int working_set_length, max_working_set_size;
	 Vacuum *vacuum;
	 Copier *copier;
    Expander *expander;
	 Sprite *saved_subject;
	 unsigned char saved_subject_index;
	 Cubby *top_cubby;
	 Robot *next_robot;
	 Robot *saved_next_robot;
	 Robot *first_in_line;
	 Robot *robot_to_put_behind;
	 Sprite *saved_stack;
	 int saved_character; // was char prior to 100604
//	 city_coordinate saved_tool_llx, saved_tool_lly;
//    city_coordinate behind_robot_llx, behind_robot_lly;
	 // need whole byte since pointer to it is released 
//	 flag true_when_body_finished;
	 ActionStatus body_status;
//	 ActionStatus *action_status; // made obsolete on 171102
//    boolean true_when_ready_to_try_clause;
//    boolean watched_at_start_flag : 1;
	 flag running_body_flag : 1;
//	 flag vacuum_grasped : 1;
//	 flag copier_grasped : 1;
//    flag expander_grasped : 1;
//	 flag encountered_remotes_flag : 1;
	 flag run_again_this_cycle : 1;
	 flag waiting_for_release_flag : 1;
	 flag suspended_flag : 1;
	 flag saved_select : 1;
	 flag saved_next_event_send_character_to_same_subject : 1; // new on 270704
	 // following regrouped on 270704 (perhaps saves memory -- shouldn't matter otherwise)
	 flag body_started_flag : 1; // updated on 171102
	 flag use_home_notebook : 1;
	 flag released_on_its_stack : 1;
	 flag name_from_serial_number : 1;
	 flag remove_me_from_backside_flag : 1;
	 flag team_running_flag : 1;
	 flag last_failure_was_for_unstable_reasons_flag : 1;
//  millisecond add_follower_after;
//	 Sprite *follower_to_add; // noticed obsolete on 161202
//	 Programmer *programmer; // made obsolete on 131202
	 unsigned int set_down_counter;
//	 flag inside_sprite_flag : 1;
//	 flag nothing_grasped_flag : 1;
//	 Sprites *deleted_sprites; // noticed obsolete on 161202
	 Sprite *suspended_on; // just used by Marty to give more useful help
	 int abort_reason; // worth the cost? - only used by Marty to give more useful help
	 char saved_key; // only used as a kludge to encode duration of use of Pumpy -- 
	 // bad idea now that Pumpy can run a fixed number of milliseconds? - asked 131202
	 Page_Number_Offsets *page_number_offsets;
	 short int recent_page_number;
    Notebook *team_notebook;
	 Notebook *home_notebook; 
    short int home_notebook_last_page_number; 
//    flag notebook_not_yet_used : 1; // noticed on 301000 that this was obsolete
    unsigned char training_counter, original_training_counter;
    city_coordinate tool_width, tool_height;
	 city_coordinate drop_scroll_delta_x, drop_scroll_delta_y; // new on 040404 to keep track of how much robot has scrolled 
	 // since it received something
	 Text *name_from_user;
	 Robot *first_to_fail_for_unstable_reasons; // is really a team property so is just kept for first_in_line
	 int *notebook_ids_with_recent_page_numbers;
	 int *recent_page_numbers; // removed short on 060505 -- but are there other places that need to be changed too??
	 int number_of_recent_page_numbers; // removed short on 060505
	 int version; // new on 120504 -- initially to change the matching of a blank picture with a flipped picture
//#if TT_DEBUG_ON
//    unsigned char body_version_number; 
//#endif    
};

Robot *load_robot_line(InputStream *pad_in,
                       int notebook_version, NaturalLanguage language,
                       unsigned char type, long index, Sprite *container, boolean descendants_inactive);
//Robot *load_robot(SpriteType type, InputStream *pad_in,
//                  int notebook_version, NaturalLanguage language, long index);
Robot *load_robot_with_training_counter(InputStream *pad_in,
                                        int notebook_version, NaturalLanguage language, long index, Sprite *container, boolean descendants_inactive);
Robot *undefined_robot(InputStream *pad_in,
                       int notebook_version, NaturalLanguage language, long index, Sprite *container, boolean descendants_inactive);
boolean describe_robot_line(InputStream *pad_in, output_stream &text_out, int version_number,
									 Article article=INDEFINITE_ARTICLE,
									 boolean in_thought_bubble=FALSE);
boolean describe_robot(SpriteType type, int count,
							  InputStream *pad_in, output_stream &text_out, int version_number);

void robot_saved();

void robot_abstracted();

void ensure_image_cycles(int code);
void start_new_java_context();
void end_new_java_context();

void serial_number_to_name(unsigned short number, ascii_string name, boolean capitalize_first=FALSE);

#endif
