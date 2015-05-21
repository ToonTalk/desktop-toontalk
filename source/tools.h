// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_TOOLS_H)

#define __TT_TOOLS_H
#if !defined(__TT_REGION_H)   
#include "region.h"
#endif
#if !defined(__TT_SPRITES_H)   
#include "sprites.h"
#endif   
#if !defined(__TT_BCKGRND_H)
#include "bckgrnd.h"
#endif

class Sprite;
//class Programmer_At_Floor;
class Notebook;
//class Integer;
class Number; // new on 010202
class Text;
class Cubby;
class Nest;
class Bomb;
class Picture;
//class Thought_Bubble;
class Robot;
class Truck_Inside;

class Tool : public Sprite {
  public:
	  Tool(SpriteCode appearance_code,
			 city_coordinate center_x,
			 city_coordinate center_y,
//			 Background *background,
			 SpriteType col_type=NONE_GIVEN,
			 short int initial_parameter=0,
			 city_scale current_scale=ground_scale,
			 city_coordinate width=0,
			 city_coordinate height=0);   
//	  void active_region(TTRegion &region);
//     void prepare_for_deletion();
//	  void generate_help(output_stream &message, Sprite *tool_in_hand);
	  virtual boolean get_grasped();
//	  virtual void held_help(output_stream &message, Sprite *current_selection) {
		  // shouldn't really be called
//        generate_help(message,current_selection);
//	  };
     Sprite *released(Sprite *by, boolean top_level=TRUE, SpecialDrop special_drop=NOT_SPECIAL_DROP);
	  boolean is_tool() {
		  return(TRUE);
	  };
     void toggle_on(Sprite *by);
     void turn_on(Sprite *by);
     void turn_off(Sprite *by);
     void update_button();
  	  Sprite *select(TTRegion *region, SelectionReason reason, Sprite *in_hand);
     void button_pushed(Button *button, Sprite *by);
     virtual char button_label() {
       return('?');
     };
     virtual void good_button_size_and_location(city_coordinate &button_width, city_coordinate &button_height,
																city_coordinate &button_llx, city_coordinate &button_lly) {
     }; // shouldnt be called 
//     void update_button_size_and_location();                                                
     virtual void set_state(int new_state, Sprite *by=NULL) { // by added on 240902
        state = (char) new_state;
     };
     void stop_repeating_sound();
     virtual void repeating_sound() {
     };
     void current_favorite(FavoriteSizeAndLocation &f) {
         f.favorite_width = favorite_width;
         f.favorite_height = favorite_height;
         f.favorite_llx = favorite_llx;
         f.favorite_lly = favorite_lly;
     };
     void set_favorite(FavoriteSizeAndLocation &f) {
         favorite_width = f.favorite_width;
         favorite_height = f.favorite_height;
         favorite_llx = f.favorite_llx;
         favorite_lly = f.favorite_lly;
     };
     void set_default_favorite_size_and_location(city_coordinate width,
                                                 city_coordinate height,
                                                 city_coordinate x,
                                                 city_coordinate y);
     void favorite_size(city_coordinate &width, city_coordinate &height);
     void favorite_location(city_coordinate &x, city_coordinate &y) {
        x = favorite_llx;
        y = favorite_lly;
     };
     void set_favorite_size_and_location();
//     void locomote_to_favorite_location_and_size();
     boolean is_on() {
        return(on_flag);
     };
	  // and wands refuse to be copied (by default)
//	  Sprite *copy(Background *) {
//			return(NULL);
//	  };
	  boolean being_used() {
		  return(being_used_flag);
	  };
	  void set_being_used(boolean new_flag) {
		  being_used_flag = (flag) new_flag;
	  };
     boolean used_since_pick_up() {
       return(used_since_pick_up_flag);
     };
     void reset_used_since_pick_up() {
        used_since_pick_up_flag = FALSE;
     };
     void save_state() {
        saved_state = state;
        set_state(0); // 0 had better be the default
     };
     void restore_state() {
        set_state(saved_state);
     };
	  void stop_running();
	  void set_called_since_last_release(boolean new_flag) {
		  called_since_last_release = (flag) new_flag;
	  };
	  void recursively_activate_pictures(boolean reset_animation=TRUE, 
		                                  PictureActivateContext context=PICTURE_ACTIVATE_CONTEXT_NOT_INTIALIZED) {
		  // new on 210300 to do nothing so when a tool is dropped its followers aren't activated
		  // e.g. something being sucked up by Dusty.
	  }; 
	  boolean need_subject_to_work() {
		 return(TRUE); // unless overridden
	  };
     virtual boolean used_once_per_click() { // new on 310702
        return(TRUE);
     };
	  void update_button_position();
	  void set_button(Sprite *new_button);
#if TT_DEBUG_ON
	  // only needed while debugging
	  Button *pointer_to_button() {
		  return(button);
	  };
#endif
#if TT_XML
	  void xml(xml_element *element, xml_document *document);
     boolean handle_xml_tag(Tag tag, xml_node *node);
	 // boolean ok_to_save_in_include_file() {
		// return(FALSE); // new on 190703 since too small to save in its own file 
	 //};
#endif
//	  boolean stable_while_in_hand() { // new on 210901
//		  return(TRUE); // need stability for accurate selection
//	  };
	  boolean running_to_hand() { // new on 221105
		  return(running_to_hand_flag);
	  };
	protected:
//	  Programmer_At_Floor *programmer; -- made obsolete on 171102
     Button *button;
	  flag running_to_hand_flag : 1;
	  flag being_used_flag : 1;
     flag on_flag : 1;
     flag used_since_pick_up_flag : 1;
     flag called_since_last_release : 1;
     city_coordinate favorite_llx, favorite_lly, favorite_width, favorite_height;
	  city_coordinate default_favorite_llx, default_favorite_lly;
     short int sound_id;
     char state, saved_state;
};

enum ExpanderState {EXPANDER_BIGGER,EXPANDER_TALLER,EXPANDER_WIDER,
                    EXPANDER_SMALLER,EXPANDER_SHORTER,EXPANDER_NARROWER,
                    EXPANDER_GOOD_SIZE,EXPANDER_LAST_STATE};

class Expander : public Tool {
  public:
	  Expander(city_coordinate x=0, city_coordinate y=0); // given default values on 011102
	  boolean ok_to_delete();
	  Sprite *copy(boolean also_copy_offsets=FALSE);
	  Sprite *used(Sprite *subject,
					Sprite *by,
					boolean button_unchanged,
					boolean record_action,
					millisecond duration,
					boolean modified, Sprite *original_subject);
	  void grow(Sprite *selection, Screen *screen, boolean modified);
	  void shrink(Sprite *selection, Screen *screen);
#if TT_XML
//     xml_element *xml_create_element(xml_document *document);
     void xml(xml_element *element, xml_document *document);
     boolean handle_xml_tag(Tag tag, xml_node *node);
	  short int default_parameter() { // new on 250103 - only used for dumping XML now
	     return(2);
	  };
	  wide_string xml_tag() { // new on 090703
		  return(L"BikePump"); 
	  };
#endif
     boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
//	  void held_help(output_stream &message, Sprite *current_selection);
	  void generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection);
     void describe(output_stream &message, Article article=INDEFINITE_ARTICLE);
	  Sprite *selected(SelectionReason reason, Sprite *by);
	  Sprite *released(Sprite *by, boolean top_level=TRUE, SpecialDrop special_drop=NOT_SPECIAL_DROP);
     void turn_on(Sprite *by);
     void turn_off(Sprite *by);
	  void adjust_grasp(city_coordinate &adjustment_x,
							  city_coordinate &adjustment_y);
     void move_to_hand(); // Programmer_At_Floor *the_programmer);
//     void locomote_to_favorite_location_and_size();
     void dramatic_entrance(millisecond duration);
     void jump_to_favorite_location();
	  void jump_to(city_coordinate x, city_coordinate y);
	  void jump_to_goal();
	  void really_jump();
	  void really_jump_to();
	  boolean get_grasped();
	  void stop_running();
	  void morph_to_inert();
	  void active_region(TTRegion &region);
     Sprite *respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected,
                                 Sprite *by, boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless=FALSE);
     boolean set_parameter(short int new_parameter, boolean warn=TRUE,
                           boolean reset_cycle_regardless=FALSE); // boolean loading=FALSE);
     void one_cycle_done();
     void repeating_sound();
     char button_label();
     void good_button_size_and_location(city_coordinate &button_width,
                                        city_coordinate &button_height,
                                        city_coordinate &button_llx,
                                        city_coordinate &button_lly);
     void set_state(int new_state, Sprite *by=NULL);
     boolean used_once_per_click();
     boolean is_still_on();
     boolean used_enough();
//     void flip_mode();
  private:
	  city_coordinate goal_x, goal_y;
	  flag normal_flag : 1;
	  flag grow_just_width_flag : 1;
     flag modified_previously : 1;
//	  flag still_working : 1;
	  Sprite *last_subject;
     millisecond use_start;
//     ActionStatus *action_status; // replace with saved_by?? -- 171102
//     ExpanderState state;
};

enum VacuumState {VACUUM_SUCK,VACUUM_SPIT,VACUUM_BLANK,VACUUM_LAST_STATE};

class Vacuum : public Tool {
  public:
	  Vacuum(city_coordinate x=0, city_coordinate y=0);
	  boolean ok_to_delete();
	  void release_all_references();
	  Sprite *copy(boolean also_copy_offsets=FALSE);
#if TT_XML
//     xml_element *xml_create_element(xml_document *document);
     void xml(xml_element *element, xml_document *document);
     boolean handle_xml_tag(Tag tag, xml_node *node);
	  wide_string xml_tag() { // new on 090703
		  return(L"Vacuum"); 
	  };
#endif
     boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
//	  void update_display(const UpdateStatus update_status=UPDATE_CHANGE); // added 210901
//	  void update_display();
//     void selected();
	  Sprite *used(Sprite *subject,
					Sprite *by,
					boolean button_unchanged,
					boolean record_action,
					millisecond duration,
					boolean modified, Sprite *original_subject);
     void vacuum_only_if_first(Sprite *to_protect);
	  boolean suck_to_make_blank(Sprite *item, Sprite *by);
	  void sucking_up_finished();
//     void turn_off_unless_mouse_still_down();
	  void remove_blanked_item(Sprite *item);
	  void restore_last_item(millisecond duration,
									 Sprite *by, boolean record_action);
	  void item_sucked_up(Sprite *item);
	  void add_deletee(Sprite *item);
	  void item_restored();
     void turn_on(Sprite *by);
     void turn_off(Sprite *by);
     boolean is_still_on();
//	  void held_help(output_stream &message, Sprite *current_selection);
	  void generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection);
	  void describe(output_stream &message, Article article=INDEFINITE_ARTICLE);
	  void adjust_grasp(city_coordinate &adjustment_x,
							  city_coordinate &adjustment_y);
     void dramatic_entrance(millisecond duration);
	  void move_to_hand(); // Programmer_At_Floor *the_programmer);
	  void walk_to_location();
//     void locomote_to_favorite_location_and_size();
     void walk_to_favorite_location(millisecond new_duration=0);
	  void walk_to_location(city_coordinate goal_x, city_coordinate goal_y,
									millisecond duration=0);
	  void walk_to_goal();
	  boolean get_grasped();
	  void stop_running();
	  void morph_to_inert();
	  Sprite *last_sucked_up() {
		  return(sucked_up->first());
	  };
	  Sprite *selected(SelectionReason reason, Sprite *by);
	  Sprite *released(Sprite *by, boolean top_level=TRUE, SpecialDrop special_drop=NOT_SPECIAL_DROP);
	  void active_region(TTRegion &region);
     void is_inert();
     boolean set_parameter(short int new_parameter, boolean warn=TRUE,
                           boolean reset_cycle_regardless=FALSE); // boolean loading=FALSE);
 	  Sprites *vacuumed_up() {
		 return(sucked_up);
	  };
//	  Screen *pointer_to_saved_screen() {
//		 return(saved_screen);
//	  };
	  void save_contents();
	  void restore_contents(Sprite *by);
     Sprite *respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected,
                                 Sprite *by, boolean record_action, Sprite *original_recipient,
                                 boolean size_constrained_regardless=FALSE);
     void repeating_sound();
     void initialize_sucked_up(Sprites *list);
     void initialize_blanked(Sprites *list);
     char button_label();
     void good_button_size_and_location(city_coordinate &button_width,
                                        city_coordinate &button_height,
                                        city_coordinate &button_llx,
                                        city_coordinate &button_lly);
      void set_state(int new_state, Sprite *by=NULL);
//      boolean ready_to_spit() {
//         return(state == VACUUM_SPIT);
//      };
		SelectionReason selection_reason();
		boolean need_subject_to_work() {
			return(state != VACUUM_SPIT);
		};
      boolean used_once_per_click();
  private:
	  Sprites *sucked_up;
	  Sprites *blanked;
	  Sprite *restoring;
	  Sprites *sucking_up;
	  Sprites *deleted;
	  Sprites *saved_sucked_up;
	  Sprites *saved_blanked;
//	  boolean restoring_done;
//	  Screen *saved_screen;
	  flag normal_flag : 1;
//	  ActionStatus *action_status;
//     millisecond flip_end_time;
//	  city_coordinate *pointer_to_llx;
//	  city_coordinate *pointer_to_lly;
	  city_coordinate item_width;
//	  , llx_minus_item_width;
	  city_coordinate last_sucked_llx, last_sucked_lly;
	  city_coordinate goal_x, goal_y;
	  millisecond duration;
     Sprites *only_first_timers;
	  flag restoring_contents : 1;
//     VacuumState state; // : 2 caused some problems
//     city_coordinate vacuum_x, vacuum_y;
//     city_coordinate vacuum_height;
};

enum CopierState {COPIER_NORMAL,COPIER_ORIGINAL,COPIER_SELF,COPIER_LAST_STATE};

class Copier : public Tool {
  public:
	  Copier(city_coordinate x=0, city_coordinate y=0);
	  boolean ok_to_delete();
//	  ~Copier();
//	  void prepare_for_deletion();
	  Sprite *copy(boolean also_copy_offsets=FALSE);
//     void update_display();
//     void selected();
	  Sprite *used(Sprite *subject,
					   Sprite *by,
					   boolean button_unchanged,
					   boolean record_action,
					   millisecond duration,
					   boolean modified, Sprite *original_subject);
//	  void held_help(output_stream &message, Sprite *current_selection);
	  void generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection);
//     TTRegion *active_region();
	  Sprite *respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected, Sprite *by,
										   boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless=FALSE);
     ReleaseStatus item_released_on_top(Sprite *item, Sprite *by, SpritePointer &recipient, int level);
     void display(SelectionFeedback selection_feedback,
						boolean followers_too, TTRegion *region);
     void display_counter(city_coordinate counter_llx,
                          city_coordinate counter_lly);
     void java(java_stream &stream);
#if TT_XML
//     xml_element *xml_create_element(xml_document *document);
     void xml(xml_element *element, xml_document *document);
     boolean handle_xml_tag(Tag tag, xml_node *node);
	  wide_string xml_tag() {
		  return(L"MagicWand");
	  };
#endif
     boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
     void dramatic_entrance(millisecond duration);
	  void move_to_hand(); // Programmer_At_Floor *the_programmer);
	  Sprite *if_released();
	  Sprite *released(Sprite *by, boolean top_level=TRUE, SpecialDrop special_drop=NOT_SPECIAL_DROP);
     void active_region(TTRegion &region);
//     void add_attached_as_follower();
	  Sprite *attached_tool() {
		  return(attached);
	  };
	  void set_attached(Sprite *item, boolean just_set);
	  void finished_loading();
     void set_priority(long new_priority);
     void set_counter(long new_counter) {
        counter = new_counter;
     };
     long return_counter() {
        return(counter);
     };
     void restore_counter() {
        counter = saved_counter;
     };
     void save_counter() {
        saved_counter = counter;
     };
     boolean need_subject_to_work() {
        return(state != COPIER_SELF);
     };
     char button_label();
     void good_button_size_and_location(city_coordinate &button_width,
                                        city_coordinate &button_height,
                                        city_coordinate &button_llx,
                                        city_coordinate &button_lly);
     void set_state(int new_state, Sprite *by=NULL);
	  void travel_to_favorite_location(millisecond duration);
	  void remove_selected_element(Sprite *element, SelectionReason reason, Sprite *by, boolean add_to_floor);
  private:
	  Sprite *attached; // what it just copied
//     CopierState state;
     long counter;
     long saved_counter;
};

class Toolbox : public Sprite {
  public:
	  Toolbox(); // updated 080704 Notebook *notebook=NULL); // arg new on 130100
	  boolean ok_to_delete();
	  void release_all_references();
//	  void toontalk_reset();
	  void initialize();
	  //void initialize_expander(boolean set_size_to_favorite=FALSE);
	  //void initialize_vacuum(boolean set_size_to_favorite=FALSE);
	  //void initialize_copier(boolean set_size_to_favorite=FALSE);
	  //void initialize_notebook(boolean set_size_to_favorite=FALSE);
     void update_tt_favorites();
	  Sprite *copy(boolean also_copy_offsets=FALSE);
#if TT_XML
//     xml_element *xml_create_element(xml_document *document);
     wide_string xml_tag() { // new on 311102 -- currently has no "state" -- had been commented out - restored on 090703
        return(L"Toolbox");
     };
	  void xml(xml_element *toolbox_element, xml_document *document);
     boolean handle_xml_tag(Tag tag, xml_node *node);
	  short int default_parameter() { // new on 250103 - only used for dumping XML now
	     return(12);
	  };
#endif
	  void set_priority(long );
	  void move_to_hand(); // Programmer_At_Floor *the_programmer);
	  void enter(boolean move_toolbox_offscreen_first=TRUE);
	  void turn_vertical();
	  millisecond leave(boolean floor_too=FALSE, Sprite *by=NULL);
//     void locomote_to_favorite_location_and_size();
	  millisecond go_to(city_coordinate goal_x, city_coordinate goal_y,
							  int percentage_size_goal=100, Sprite *by=NULL);
	  void exit_stage_right();
	  void opened();
	  void bring_out_notebook();
	  void bring_out_expander();
	  void bring_out_copier();
	  void bring_out_vacuum();
	  boolean settled_down() {
		  return(resource_index() == TOOL_BOX_MORPH16_IMAGE);
	  };
     void become_inert();
     void set_inside_vacuum(boolean new_flag, boolean recur=TRUE);
	  void offstage();
     void remove_tools();
	  Sprite *compartment_contents(int row, int column);
     void refit_contents();
     Sprite *selected(SelectionReason reason, Sprite *by);
//     Sprite *stack_of_type(SpriteType type);
	  void compartment_size_and_location(int row, int column,
													 city_coordinate &compartment_width, city_coordinate &compartment_height,
													 city_coordinate &compartment_llx, city_coordinate &compartment_lly);
	  Sprite *select(TTRegion *region, SelectionReason reason, Sprite *in_hand);
	  Sprite *released(Sprite *by, boolean top_level=TRUE, SpecialDrop special_drop=NOT_SPECIAL_DROP);
     ReleaseStatus item_released_on_top(Sprite *item, Sprite *by, SpritePointer &recipient, int level);
	  void display(SelectionFeedback selection_feedback=NO_FEEDBACK,
						boolean followers_too=TRUE, TTRegion *region=NULL);
	  boolean save_region();
//	  TTRegion *active_region();
//	boolean responsible_for(Sprite *tool);
	void add_self_and_friends(Background *background);
	// commented out the following on 080704 since these are now global variables
	//Vacuum *pointer_to_vacuum() {
	//  return(vacuum);
	//};
	//void set_vacuum(Vacuum *tool);
	//Expander *pointer_to_expander() {
	//  return(expander);
	//};
	//void set_expander(Expander *tool);
	//Copier *pointer_to_copier() {
	//	return(copier);
	//};
	//void set_copier(Copier *tool);
	//Notebook *pointer_to_notebook();
	//void set_notebook(Notebook *new_notebook);
	Sprite *pointer_to_stack(SpriteType type); // new on 181202 - for saving running robots in XML
	Sprite *pointer_to_integer_stack() {
		return(stacks[TOOLBOX_INTEGER_STACK]);
	};
	Sprite *pointer_to_cubby_stack() {
		return(stacks[TOOLBOX_CUBBY_STACK]);
	};
	Sprite *pointer_to_nest_stack() {
		return(stacks[TOOLBOX_NEST_STACK]);
	};
	Sprite *pointer_to_scale_stack() {
		return(stacks[TOOLBOX_SCALE_STACK]);
	};
	Sprite *pointer_to_robot_stack() {
		return(stacks[TOOLBOX_ROBOT_STACK]);
	};
	Sprite *pointer_to_truck_stack() {
		return(stacks[TOOLBOX_TRUCK_STACK]);
	};
	Sprite *pointer_to_bomb_stack() {
		return(stacks[TOOLBOX_BOMB_STACK]);
	};
	Sprite *pointer_to_text_stack() {
		return(stacks[TOOLBOX_TEXT_STACK]);
	};
	ToolboxCacheStatus cache_status();
	void set_background_cached(boolean flag);
	boolean is_background_cached() { // new on 061003 - initially for debugging
		return(background_cached);
	};
	void just_set_background_cached(boolean flag) { // new on 061003
		background_cached = flag;
	};
	void restore_followers();
   void shifted(city_coordinate delta_x, city_coordinate delta_y);
	// commented out on 190103 since shouldn't need this
	//boolean ok_to_delete() {
	//	return(tt_shutting_down); // never until shutting down -- was FALSE prior to 160103
	//};
	void reset_last_contents_selection() {
		last_contents_selection = NULL;
	};
	boolean now_offstage() {
		return(offstage_flag);
	};
   void current_favorite(FavoriteSizeAndLocation &f) {
      f.favorite_width = favorite_width;
      f.favorite_height = favorite_height;
      f.favorite_llx = favorite_llx;
      f.favorite_lly = favorite_lly;
     };
   void set_favorite(FavoriteSizeAndLocation &f);
   void set_favorite_size_and_location();
   void favorite_size(city_coordinate &width, city_coordinate &height);
	void check_tt_file_name_to_clipboard();
	void add_sprite(Sprite *sprite);
	void set_clean_status(CleanStatus new_status, boolean recur=TRUE); // was missing the =TRUE prior to 060305
	boolean gone_for_good() {
		return(gone_for_good_flag);
	};
	boolean gone() {
		return(gone_for_good_flag || inside_vacuum());
	};
	//boolean ok_to_save_in_include_file() {
	//	return(FALSE); // updated 190703 since too small to save in its own file -- was (code != TOOLBOX_SIDE_SPRITE); 
	// since tool box running around is changing all the time
	//};
  private:
	 //Vacuum *vacuum;
	 //Copier *copier;
	 //Expander *expander;
	 //Notebook *notebook;
	 //Number *integer_stack;
	 //Cubby *cubby_stack;
	 //Nest *nest_stack;
	 //Picture *scale_stack;
	 //Robot *robot_stack;
	 //Truck_Inside *truck_stack;
	 //Bomb *bomb_stack;
	 //Text *text_stack;
	 SpritePointer stacks[TOOLBOX_STACK_COUNT];
	 int percentage_size_goal;
	 city_coordinate goal_x,goal_y;
	 city_coordinate cached_llx, cached_lly, cached_width, cached_height;
	 boolean	background_cached;
	 Sprites *saved_followers;
	 Sprite *last_contents_selection;
	 flag offstage_flag;
    flag leave_floor_when_offstage;
    city_coordinate favorite_llx, favorite_lly, favorite_width, favorite_height;
	 city_coordinate default_favorite_llx, default_favorite_lly; // new on 011100
    flag called_since_last_release : 1;
	 flag running_to_hand_flag : 1;
	 flag gone_for_good_flag : 1; // new on 120704 -- deleted but used for robot stacks and a few other things
	 int frame_to_check_tt_file_name_to_clipboard; // new on 12103 - XML can ignore this since just used to delay pasting by a frame
};

//boolean is_wand(SpriteType type);
Copier *load_copier(InputStream *stream, int notebook_version, NaturalLanguage langauge, long index, boolean descendants_inactive);
boolean describe_copier(InputStream *pad_in, output_stream &text_out, int version_number, Article article);
Vacuum *load_vacuum(InputStream *stream, int notebook_version, NaturalLanguage langauge, long index);
boolean describe_vacuum(InputStream *pad_in, output_stream &text_out, int version_number, Article article);
Expander *load_bike_pump(InputStream *stream, int notebook_version, NaturalLanguage langauge, long index);
boolean describe_bike_pump(InputStream *pad_in, output_stream &text_out, int version_number, Article article);
void make_sucking_sound();
millisecond decode_duration(char encoding);
char encode_duration(millisecond duration);
boolean not_friend_of_tooly(Sprite *sprite, void *predicate_arg=NULL);

#endif
