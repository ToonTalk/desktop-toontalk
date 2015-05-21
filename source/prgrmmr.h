// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved

#if !defined(__TT_PRGRMMR_H)
#define __TT_PRGRMMR_H

#if !defined(__TT_CITY_H)
#include "city.h"
#endif   
#if !defined(__TT_THOUGHT_H)
#include "thought.h"
#endif   
#include <math.h>

// to share between different programmer files
extern char button_status;

class Room;
class Robot;
class Leaders;
class Programmer_At_Floor; // defined after being used
class Programmer_State;


enum ProgrammerStatus {
	PROGRAMMER_NORMAL, PROGRAMMER_QUIT, PROGRAMMER_RUNNING_TIMER,
	START_FLYING, FLYING, FLYING_AGAIN, LANDING, TAKING_OFF, 
	LEAVE_HELICOPTER, WALKING_AWAY_FROM_HELICOPTER,
	ENTER_HELICOPTER, CITY_WALKING, NEED_HELICOPTER,
	ENTERING_ROOM, // ROOM_WALKING,
	SITTING_DOWN, AT_FLOOR, STANDING_UP, STOOD_UP,
	LEAVING_FLOOR, LEAVE_FLOOR_WHEN_TOOLBOX_READY,
	LEAVING_ROOM, HOUSE_DESTROYED,
   ABOUT_TO_ENTER_THOUGHT_BUBBLE, READY_TO_ENTER_THOUGHT_BUBBLE,
   ENTERING_THOUGHT_BUBBLE, ENTERED_THOUGHT_BUBBLE,
	TELEPORT_MARTIAN_IN, SPINNING,DIZZY, PUZZLE_START,
   ON_GROUND, STOOD_UP_FROM_GROUND, SITTING_DOWN_OUTSIDE, LEAVING_GROUND, STANDING_UP_OUTSIDE,
   START_CITY_WALKING, START_ROOM_WALKING, START_AT_FLOOR, 
	START_LANDING, START_ON_THE_GROUND, START_IN_THOUGHT_BUBBLE,
   START_AFTER_LOADING_XML // new on 060103
};
//	NEXT_TITLE_IF_SHOWING_TITLES};

enum HandState
	 {EMPTY_HAND, ABOUT_TO_HOLD_ITEM, HOLDING_ITEM, 
	  ABOUT_TO_RELEASE_ITEM, ABOUT_TO_RELEASE_COPIER,
	  HOLDING_COPIER, USED_COPIER, // used and copy still attached
     USING_COPIER};

class Programmer : public Entity {
	public:
		Programmer();
		~Programmer();
		void initialize();
		boolean react(Screen *screen);
      void transfer_item_to_city(Sprite *picture, Background *floor);
		void translate_ground_to_city_coordinates(Sprite *item);
		void translate_ground_to_city_coordinates(city_coordinate &w, city_coordinate &h, city_coordinate &x, city_coordinate &y);
		void ground_containment_region(TTRegion &region);
      void add_stuff_on_ground_to_city();
      void move_stuff_on_floor_to_room(Room *room);
      void move_stuff_in_room_to_floor(Room *room);
		void translate_from_floor_to_room(Sprite *sprite);
		void translate_from_room_to_floor(Sprite *sprite);
//      void remove_stuff_on_ground();
      void translate_floor_to_room_coordinates(city_coordinate &x, city_coordinate &y);
      void translate_floor_to_room_dimensions(city_coordinate &w, city_coordinate &h);
      void translate_room_to_floor_coordinates(city_coordinate &x, city_coordinate &y);
      void translate_room_to_floor_dimensions(city_coordinate &w, city_coordinate &h);
		city_coordinate translate_room_to_floor_width(city_coordinate w);
		city_coordinate translate_room_to_floor_height(city_coordinate h);
		city_coordinate translate_floor_to_room_width(city_coordinate w);
		city_coordinate translate_floor_to_room_height(city_coordinate h);
      void shift_sit_corner(Direction direction,
                            city_coordinate &new_sit_llx, city_coordinate &new_sit_lly,
                            city_coordinate &new_sit_urx, city_coordinate &new_sit_ury);
      boolean set_sit_corner(city_coordinate new_sit_llx, city_coordinate new_sit_lly, Background *floor,
                             city_coordinate &delta_x, city_coordinate &delta_y);
      void just_set_sit_corner(city_coordinate x, city_coordinate y); 
		Sprite *pointer_to_appearance();
		boolean training();
      Background *pointer_to_background();
		void enter_helicopter(city_coordinate center_x,
									 city_coordinate center_y,
									 Screen *screen);
		void house_about_to_be_destroyed();
//		boolean is_programmer_on_floor() {
//			return(kind_of() == PROGRAMMER_AT_FLOOR);
//		};
		Programmer_At_Floor *programmer_at_floor();
		void set_room(Room *new_room) {
			room = new_room;
		};
		void set_timer(millisecond new_time);
		void set_next_status(ProgrammerStatus new_next_status); // new on 201000
		void set_starting_status(ProgrammerStatus status) { // new on 070103
			starting_status = status;
		};
      void forget_tool_in_hand();
		void release_items_held();
      Sprite *release_tool_in_hand();
      Sprite *pointer_to_tool_in_hand();
      Sprite *return_tool_in_hand() { // useful during state transitions
        return(tool_in_hand);
      };
      void set_tool_in_hand(Sprite *sprite, boolean tell_state_too=FALSE);
//		void set_hand_state(HandState new_state) {
//			hand = new_state;
//		};
		void display_appearance();
		boolean save_appearance_regions();
		ProgrammerState kind_of();
      city_scale return_city_scale();
		boolean part_of_selection(Sprite *item);
		//		Tool *pointer_to_current_selection();
	  void set_saved_city_location(city_coordinate x, city_coordinate y) {
		  saved_city_x = x;
		  saved_city_y = y;
	  };
	  ProgrammerStatus return_next_status() {
		  return(next_status);
	  };
     boolean floor_not_scrollable() {
        return(floor_width_sitting_ratio == 1 && floor_height_sitting_ratio == 1);
     };
	  boolean invisible() {
		  return(invisible_flag);
	  };
#if TT_XML
     xml_element *xml(xml_document *document);
//     boolean handle_xml(xml_node *node);
     boolean handle_xml_tag(Tag tag, xml_node *node);
//     void initialize_saved_deltas();
#endif
     boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
     void load(InputStream *stream, int notebook_version, NaturalLanguage language);
     void transfer_saved_gemometry_and_misc();
	  ProgrammerStatus status_when_dumped();
     void titles_over();
     //boolean are_titles_over() {
     //  return(titles_over_flag);
     //};
     void add_empty_helicopter(boolean screen_too);
	  void stop_retraining(Robot *robot);
	  void set_visibility(boolean flag);
	  void add_item_from_outside(Sprite *item, city_coordinate x, city_coordinate y);
//	  void add_items_from_outside(Sprites *items, city_coordinate x=0, city_coordinate y=0);
	  unsigned char simulated_key();
	  boolean center(city_coordinate &center_x, city_coordinate &center_y);
	  boolean lower_left_corner(city_coordinate &llx, city_coordinate &lly);
	  city_coordinate current_width();
	  city_coordinate current_height();
	  void tool_in_hand_is_ready(Sprite *tool);
     boolean compute_selection_region(TTRegion &region); // new on 170402
     void note_absolute_mode_deltas(coordinate &delta_x, coordinate &delta_y, boolean left_button_down, boolean left_button_just_went_down); // new on 030502
//	  void load_city_if_need_be();
     void load_deltas(InputStream *stream);
     void dump_saved_deltas(output_stream &stream);
     void completed(); // new on 181102
	  boolean inside_a_house(); // new on 111202
	  Room *pointer_to_room() { // new on 070103
		  return(room);
	  };
	  void activate_state();
	  city_coordinate return_grasp_delta_x() {
		  return(grasp_delta_x);
	  };
	  city_coordinate return_grasp_delta_y() {
		  return(grasp_delta_y);
	  };
	  void set_grasp_delta_x(city_coordinate new_grasp_delta_x) {
		  grasp_delta_x = new_grasp_delta_x;
	  };
	  void set_grasp_delta_y(city_coordinate new_grasp_delta_y) {
		  grasp_delta_y = new_grasp_delta_y;
	  };
	  city_coordinate return_width_at_time_of_grasp() {
		  return(width_at_time_of_grasp);
	  };
	  city_coordinate return_height_at_time_of_grasp() {
		  return(height_at_time_of_grasp);
	  };
	  void set_width_at_time_of_grasp(city_coordinate new_width_at_time_of_grasp) {
		  width_at_time_of_grasp = new_width_at_time_of_grasp;
	  };
	  void set_height_at_time_of_grasp(city_coordinate new_height_at_time_of_grasp) {
		  height_at_time_of_grasp = new_height_at_time_of_grasp;
	  };
	  void drop_on_floor();
//     void release_saved_deltas();
	private:
      boolean enable_keyboard_accelerators(char joystick_button_status, char mouse_button_status,
                                           char &button_status, unsigned char &key, boolean &extended_key);
      void disable_keyboard_accelerators();
		Programmer_State *state; // *saved_state; noticed was obsolete on 050103
//		Sprites *saved_working_set; noticed was obsolete on 050103
//      Leaders *saved_leaders;
//		City *city; // made obsolete on 050103
		// the following is for restoring things after leaving a house
		// if I use multiple screens then won't need this
		city_coordinate saved_x, saved_y, // where in room
                      sit_llx, sit_lly, // where in room adjusted for boundaries of useable area
							 city_view_x, city_view_y,
							 saved_city_x, saved_city_y; // where in the city
		city_coordinate previous_x, previous_y;
		Sprite *empty_helicopter;
//		Sprite *empty_helicopter_layer; - noticed obsolete on 050103
		city_coordinate empty_helicopter_x, empty_helicopter_y;
//		Screen *current_screen, *empty_helicopter_screen;
		Room *room;
//      Floor *floor;
		ProgrammerStatus next_status, status_when_timer_finished;
		HandState saved_hand; // renamed on 180100 since this is now only used when loading to restore the hand state (if on floor or ground)
		Sprite *tool_in_hand;
//      city_coordinate grasp_x_offset, grasp_y_offset;
		millisecond timer; // general purpose timer
		city_coordinate x_speed, y_speed; // for "auto pilot"
//		boolean just_hit_door;
      flag keyboard_accelerators_enabled;
      short int floor_width_sitting_ratio, floor_height_sitting_ratio;
      city_coordinate llx_right_room_limit, lly_top_room_limit;
      city_scale starting_scale; // set by loading a city else is default initial_scale
      flag starting_location_set;
      city_coordinate starting_x, starting_y;
//		city_coordinate starting_old_x_offset, starting_old_y_offset; - noticed obsolete on 050103
      city_coordinate starting_view_center_x, starting_view_center_y;
      ProgrammerStatus starting_status;
      Direction starting_direction;
 //     flag titles_over_flag;
		Robot *starting_robot; // added 051099 so when dumped while training can be restored
		Floor *starting_floor;
		Cubby *starting_cubby; 
		Cubby *starting_original_cubby;
//		flag city_dumped_to_log;
//		long city_load_from_log_frame;
		int file_format_version_number;
		Sprite *saved_geometry_and_misc;
      // following was at file level prior to 061102
      city_coordinate *saved_delta_xs;
      city_coordinate *saved_delta_ys;
      millisecond *saved_delta_times; // was city_coordinate before 011102 (but both were longs)
		city_coordinate person_on_ground_width, person_on_ground_height; // moved here from file level on 050103
		millisecond time_of_last_user_input; // moved here from file level on 060103
		boolean invisible_flag; // new on 090103 - replaces tt_game_switch_on
		city_coordinate grasp_delta_x; // new on 120604 so can grasp big things "in the middle" -- moved here from programmer_at_floor on 130604
		city_coordinate grasp_delta_y;
		// new on 121104 so if you grasp in the middle and then via typing the size changes significantly
		// you are still holding it at a reasonable place
		city_coordinate width_at_time_of_grasp;
		city_coordinate height_at_time_of_grasp;
};

class Programmer_State : public Entity {
  // abstract class for behaviors of the programmer
  public:
    Programmer_State() :
       keyboard_accelerators_enabled(FALSE),
       absolute_mouse_x_speed(0),
       absolute_mouse_y_speed(0),
       absolute_mouse_remaining_travel_time(0),
		 appearance(NULL), // this and following weren't initialized prior to 050103
		 max_x_speed(0), 
		 max_y_speed(0),
		 current_direction(NO_DIRECTION) {
    };
	 virtual ~Programmer_State();
	 virtual ProgrammerStatus react(boolean new_user_input,
											  unsigned char key, 
											  boolean extended_key,
											  city_coordinate delta_x,
											  city_coordinate delta_y,
											  char button_status,
											  boolean button_unchanged,
											  millisecond duration,
											  boolean running_timer) = 0;
    void swap_appearance(SpriteCode code, short int initial_parameter); //Sprite *new_appearance);
	 void colliding_with(Sprite *other,
								city_coordinate &delta_x, city_coordinate &delta_y,
								boolean &changed,
								ProgrammerStatus &next_status);
    virtual void bumped_into_house(Direction direction) {
    }; // nothing by default
	 virtual void cleanup(Screen *screen, ProgrammerStatus next_status);
	 virtual void display_appearance();
	 virtual boolean save_appearance_regions();
//	 virtual Tool *pointer_to_current_selection() {
//		 return(NULL);
//	 };
//	 virtual void move_just_below_region(TTRegion *region) {
		// by default do nothing
//	 };
//	 virtual void move_back_out_of(TTRegion *region, boolean move_extra=TRUE) {
		// by default do nothing
//    };
	 void dampen_big_deltas(city_coordinate &delta_x,
									city_coordinate &delta_y);
	 virtual void position(city_coordinate &current_x, city_coordinate &current_y) {
		 appearance->lower_left_corner(current_x,current_y);
	 };
	 void set_position(city_coordinate x, city_coordinate y) {
		 appearance->move_to(x,y);
	 };
	 virtual void center(city_coordinate &center_x, city_coordinate &center_y) {
		 appearance->center(center_x,center_y);
	 };
	 void lower_left_corner(city_coordinate &llx, city_coordinate &lly) { // new on 140901
		 appearance->lower_left_corner(llx,lly);
	 };
	 virtual city_coordinate current_width() {
		 return(appearance->current_width());
	 };
	 virtual city_coordinate current_height() {
		 return(appearance->current_height());
	 };
	 Sprite *pointer_to_appearance() {
		 return(appearance);
	 };
	 virtual ProgrammerState kind_of() = 0;
//    virtual void note_colliding_with(Sprite *, SpriteType type);
	 virtual boolean part_of_selection(Sprite *item) {
		 return(FALSE);
	 };
    virtual city_scale return_city_scale() {
       return(ground_scale);
    };
    virtual boolean enable_keyboard_accelerators(char joystick_button_status, char mouse_button_status,
                                                 char &button_status, unsigned char &key, boolean &extended_key) {
       return(FALSE);
    };
    virtual ProgrammerStatus restart_status() = 0;
    Direction return_current_direction() {
       return(current_direction);
    };
    virtual boolean add_toolbox() {
      return(FALSE);
    };
    virtual void finish_initializing() {
    };
	 virtual void stop_retraining(Robot *robot) {
	 };
	 virtual void set_visibility(boolean flag) {
	 };
	 virtual unsigned char simulated_key() {
		 return(0);
	 };
	 virtual void tool_in_hand_is_ready(Sprite *tool) { // ignore by default (new on 220901)
	 };
    virtual boolean compute_selection_region(TTRegion &region) { // new on 170402
       return(FALSE);
    };
    virtual void note_absolute_mode_deltas(coordinate &delta_x, coordinate &delta_y, boolean left_button_down, boolean left_button_just_went_down);
    virtual void true_center(city_coordinate &x, city_coordinate &y); // new on 060502
    virtual void completed() { // new on 181102
    };
	 virtual void drop(ProgrammerStatus &status) {
		 // do nothing by default -- only if on floor does this work
	 };
#if TT_XML
    virtual void add_xml(xml_element *parent_element, xml_document *document);
    virtual boolean handle_xml_tag(Tag tag, xml_node *node);
	 virtual wide_string xml_tag() = 0;
	 virtual void activate();
#endif
	 virtual void set_containment_region() { // new on 090404 to do nothing
	 };
//	 virtual ProgrammerStatus next_title_if_showing_titles() {
//		return(PROGRAMMER_NORMAL); // ignore by default
//	 };
//    virtual boolean set_containment_region(TTRegion &region) {
//       region = containment_region;
//       return(TRUE);
//    };
  protected:
    city_coordinate min_x, max_x, min_y, max_y; // moved up on 090103 here since all the subclasses used this (and now XML code can deal with it just once)
	 city_coordinate max_x_speed, max_y_speed;
	 Sprite *appearance;
//	 Programmer *programmer; removed on 050103
    flag keyboard_accelerators_enabled;
    Direction current_direction;
    coordinate absolute_mouse_x_speed, absolute_mouse_y_speed; // new on 030502 so move smoothly in absolute mouse mode
    millisecond absolute_mouse_remaining_travel_time;
};

class Programmer_Titles_Flying : public Programmer_State {
  public:
	Programmer_Titles_Flying();
	~Programmer_Titles_Flying();
	ProgrammerState kind_of() {
		return(PROGRAMMER_TITLES);
	};
   ProgrammerStatus restart_status() {
      return(START_FLYING); // shouldn't really be able to save during titles ...
   };
	ProgrammerStatus react(boolean new_user_input,
								  unsigned char key,
								  boolean extended_key,
								  city_coordinate , city_coordinate ,
								  char button_status,
								  boolean ,
								  millisecond ,
								  boolean );
	 void position(city_coordinate &current_x, city_coordinate &current_y) {
		 current_x = 0;
		 current_y = 0;
	 };
	 void center(city_coordinate &center_x, city_coordinate &center_y) {
		 center_x = 0;
		 center_y = 0;
	 };
	 city_coordinate current_width() {
		 return(1);
	 };
	 city_coordinate current_height() {
		 return(1);
	 };
    ProgrammerStatus initial_programmer_status();
	 unsigned char simulated_key();
#if TT_XML
	wide_string xml_tag() {
		return(L"ProgrammerTitlesFlying");
	};
#endif
//	 ProgrammerStatus next_title_if_showing_titles();
//	void display_appearance(Screen *screen);
  private:
	Titles_Background *background;
	flag displayed;
	flag displayed_expired_message;
   flag text_to_speech_initialized;
   flag expiration_warning_given;
//	city_coordinate saved_martian_llx,saved_martian_lly; // in case Marty was saved with a city
//	Background *saved_martian_background;
	millisecond next_title_if_after; // new on 200800
	int saved_tt_help_speed_percent; // new on 110401
   Martian *martian; // restored on 090502
//   short int background_index; 
};

class Programmer_City_Flying : public Programmer_State {
 public:
  Programmer_City_Flying(city_coordinate x=0, city_coordinate y=0,
								 city_scale the_scale=ground_scale,
//								 Programmer *programmer,
                         Direction starting_direction=EAST,
                         boolean starting_location_set=FALSE);
  void initialize_appearance();
  ProgrammerStatus react(boolean new_user_input,
								 unsigned char key,
								 boolean extended_key,
								 city_coordinate delta_x,
								 city_coordinate delta_y,
								 char button_status,
								 boolean button_unchanged,
								 millisecond duration,
								 boolean running_timer);
  ProgrammerState kind_of() {
	  return(PROGRAMMER_CITY_FLYING);
  };
  ProgrammerStatus restart_status() {
     return(START_FLYING);
  };
  city_scale return_city_scale() {
     return(scale);
  };
//  void set_center(city_coordinate x, city_coordinate y) {
//     center_x = x;
//     center_y = y;
//  };
  void set_visibility(boolean visible);
  void true_center(city_coordinate &x, city_coordinate &y);
#if TT_XML
	wide_string xml_tag() {
		return(L"ProgrammerCityFlying");
	};
	void add_xml(xml_element *element, xml_document *document);
	boolean handle_xml_tag(Tag tag, xml_node *node);
	void activate();
#endif
 private:
  boolean go_down();
  boolean go_up();
  city_scale scale;
  city_coordinate center_x,center_y; // removed min_x,max_x,min_y,max_y on 100103 since they are part of Programmer_State
  city_coordinate old_x_offset,old_y_offset;
  city_coordinate minimum_distance_to_reorient;
//  static int const minimum_pixel_movement_to_reorient;
//  static const int maximum_rotation_per_second;
  city_scale max_scale;
#if TT_DIRECT_INPUT
  unsigned short stuck_count;
#endif
};

class Programmer_City_Landing : public Programmer_State {
 public:
  Programmer_City_Landing(//Programmer *programmer,
								  //Screen *screen,
								  Sprite *old_appearance=NULL);
  ProgrammerStatus react(boolean new_user_input,
								 unsigned char key,
								 boolean extended_key,
								 city_coordinate delta_x,
								 city_coordinate delta_y,
								 char button_status,
								 boolean button_unchanged,
								 millisecond duration,
								 boolean running_timer);
  void cleanup(Screen *screen, ProgrammerStatus next_status);
  ProgrammerState kind_of() {
	  return(PROGRAMMER_CITY_LANDING);
  };
  ProgrammerStatus restart_status() {
     return(START_LANDING);
  };
  void set_visibility(boolean visible);
  void true_center(city_coordinate &x, city_coordinate &y);
#if TT_XML
	wide_string xml_tag() {
		return(L"ProgrammerCityLanding");
	};
	void activate();
#endif
 private:
//  int button_speed; // replaced with a constant on 100103
};

//enum leaving_mode {INSIDE,OUTSIDE,WILL_BE_INSIDE,WILL_BE_OUTSIDE}; // noticed obsolete on 100103
//
//class Programmer_Leave_Helicopter : public Programmer_State {
// public:
//  Programmer_Leave_Helicopter(boolean reversed,
//										city_coordinate old_x,
//										city_coordinate old_y);
////										Programmer *programmer);
//  ProgrammerStatus react(boolean new_user_input,
//								 unsigned char key,
//								 boolean extended_key,
//								 city_coordinate delta_x,
//								 city_coordinate delta_y,
//								 char button_status,
//								 boolean button_unchanged,
//								 millisecond duration,
//								 boolean running_timer);
//  ProgrammerState kind_of() {
//	  return(PROGRAMMER_LEAVE_HELICOPTER);
//  };
//  ProgrammerStatus restart_status() {
//     return(START_CITY_WALKING);
//  };
//#if TT_XML
//	wide_string xml_tag() {
//		return(L"ProgrammerLeavingHelicopter");
//	};
//#endif
// private:
//  leaving_mode mode;
//  unsigned long int mode_switch_time;
//  unsigned int cycle_duration;
//};

class Programmer_Walking : public Programmer_State {
	public:
		Programmer_Walking(Visible_Background *background);
		~Programmer_Walking();
//		void move_just_below_region(TTRegion *region);
		boolean handle_collisions(city_coordinate &delta_x,
										  city_coordinate &delta_y,
										  ProgrammerStatus &status);
      void finished_standing_up(Background *background);
		void cleanup(Screen *screen, ProgrammerStatus next_status);
		boolean add_toolbox();
		void update_toolbox(Direction previous_direction,
								  city_coordinate delta_x,
								  city_coordinate delta_y,
								  boolean moved,
								  boolean stay_in_region,
								  boolean scale_with_y); 
		void step_sound();
		virtual void make_step_sound();
#if TT_DIRECTX
      virtual void not_colliding() {
      };
#endif
		void set_spinning(boolean new_value) {
			spinning = (flag) new_value;
		};
		void set_visibility(boolean visible);
#if TT_XML
		void add_xml(xml_element *element, xml_document *document);
		boolean handle_xml_tag(Tag tag, xml_node *node);
		void activate();
#endif
	protected:
		void toolbox_center(city_coordinate &new_toolbox_x,
								  city_coordinate &new_toolbox_y,
								  boolean stay_in_region=FALSE);
		city_coordinate delta_x, delta_y;
		city_coordinate minimum_distance_to_reorient;
//		static city_coordinate const minimum_movement_to_reorient;
//		static const int maximum_rotation_per_second;
		city_coordinate toolbox_x, toolbox_y;
		Sprite *toolbox;
//		TTImage *previous_appearance_image;
		// replaced the above with below on 090103 since better for saving things
		int previous_appearance_image_index;
		unsigned char image_change_count;
		flag spinning;
		Visible_Background *background;
};

class Programmer_City_Walking : public Programmer_Walking {
 public:
  Programmer_City_Walking(city_coordinate x=0, city_coordinate y=0,
								  Direction initial_direction=EAST, // prior to 140103 default was NO_DIRECTION but that isn't a good parameter (to be updated later by more XML)
//								  Programmer *programmer,
								  boolean dizzy_first=FALSE);
  void finish_initializing();
  ProgrammerStatus react(boolean new_user_input,
								 unsigned char key,
								 boolean extended_key,
								 city_coordinate delta_x,
								 city_coordinate delta_y,
								 char button_status,
								 boolean button_unchanged,
								 millisecond duration,
								 boolean running_timer);
  void bumped_into_house(Direction direction);
#if TT_DIRECT_INPUT
  void not_colliding();
#endif
  ProgrammerState kind_of() {
	  return(PROGRAMMER_CITY_WALKING);
  };
  ProgrammerStatus restart_status() {
     return(START_CITY_WALKING);
  };
#if TT_XML
	wide_string xml_tag() {
		return(L"ProgrammerCityWalking");
	};
	void add_xml(xml_element *element, xml_document *document);
	boolean handle_xml_tag(Tag tag, xml_node *node);
//	void activate();
#endif
 private:
  city_coordinate city_min_x,city_max_x,city_min_y,city_max_y;
};

class Programmer_Room_Walking : public Programmer_Walking {
 public:
	Programmer_Room_Walking(Room *the_room,
									city_coordinate x=0,
									city_coordinate y=0,
//									Programmer *programmer,
									boolean entering=FALSE,
                           Direction starting_direction=EAST); // prior to 140103 default was NO_DIRECTION but that isn't a good parameter (to be updated later by more XML)
   void finish_initializing();
	ProgrammerStatus react(boolean new_user_input,
								  unsigned char key,
								  boolean extended_key,
								  city_coordinate delta_x,
								  city_coordinate delta_y,
								  char button_status,
								  boolean button_unchanged,
								  millisecond duration,
								  boolean running_timer);
	ProgrammerState kind_of() {
	  return(PROGRAMMER_ROOM_WALKING);
	};
   ProgrammerStatus restart_status() {
     return(START_ROOM_WALKING);
   };
	void make_step_sound();
//	Room *pointer_to_room();
#if TT_XML
	wide_string xml_tag() {
		return(L"ProgrammerRoomWalking");
	};
//	void activate();
#endif
 private:
//	Room *room; -- subsumed by background in Programmer_Walking as of 090103
};

typedef Robot *RobotPointer;

const int number_of_deltas = 16; // was 8 prior to 101199 but a longer sample is more reliable (was 32 prior to 150100)

class Programmer_At_Floor : public Programmer_State {
 public:
	Programmer_At_Floor(Floor *the_floor_from_above=NULL, // default values added on 060103
//							  Programmer *programmer,
							  city_coordinate x=0, // =10*tile_width
							  city_coordinate y=0, // =-9*tile_height
                       HandState hand_state=EMPTY_HAND,
                       Sprite *tool_in_hand=NULL);
   void finish_initializing();
	~Programmer_At_Floor();
	void cleanup(Screen *screen, ProgrammerStatus next_status);
	ProgrammerStatus react(boolean new_user_input,
								  unsigned char key,
								  boolean extended_key,
								  city_coordinate delta_x,
								  city_coordinate delta_y,
								  char button_status,
								  boolean button_unchanged,
								  millisecond duration,
								  boolean running_timer);
   ProgrammerStatus get_up();
   void drop(ProgrammerStatus &status);
//   void hand_visibility(boolean hidden);
	void display_appearance();
	void turn_off_selection_feedback_for(millisecond duration);
   void set_selection_feedback(boolean new_flag) { // new on 131102
      selection_feedback_flag = (flag) new_flag;
   };
	boolean save_appearance_regions();
	void adjust_tool_in_hand();
//   void note_colliding_with(Sprite *sprite_of_other, SpriteType type);
	millisecond expand_thought_bubble();
	void expand_thought_bubble2();
	void enter_thought_bubble(Room *room, Floor *starting_floor=NULL);
   Robot *enter_thought_bubble2();
	void fix_before_leaving_thought(Screen *screen);
	void release_items_held();
   void turn_off_item_held();
	void vacuum_items_unused();
//	void body_done();
	void returning_from_body(Floor *floor);
//   void grasp_object(Tool *item, boolean record=TRUE);
	Sprite *hand_state(HandState &hand_state);
//	void set_maximum_y();
   boolean ok_to_scroll();
	//boolean ok_to_fix_containment_active() {
	//	return(ok_to_scroll());
	//};
	void robot_in_control(boolean flag);
	void set_appearance(Sprite *new_appearance);
	Sprite *drop_recipient(Sprite *released, int min_priority);
	void release_copier();
	void restore_copier();
   void delete_released();
	void use_tool(boolean via_mouse_buttons=TRUE, city_coordinate starting_x=max_long, city_coordinate starting_y=max_long);
   void new_tool_in_hand(Sprite *replacement);
	boolean pick_up(Sprite *item, SelectionReason reason=GRASPED);
	void initialize_tool_in_hand(Sprite *item);
//	void hand_holding_copier(); // noticed obsolete on 060103
	void hand_release_cycle();
	void release_on_floor(Sprite *released);
	ProgrammerStatus release_object(boolean ignore_underneath=FALSE,
											  boolean hand_release=FALSE,
                                   boolean treat_copier_special=TRUE);
	void set_release_time(millisecond new_release_time) { // typically now
		release_time = new_release_time;
	};
	boolean bomb_set() {
	  return(bombed);
	};
	void set_bombed() {
		bombed = TRUE;
	};
	void compute_recent_speed();
	void recent_speed(city_coordinate &x_speed, city_coordinate &y_speed);
   boolean part_of_selection(Sprite *item);
	Sprite *pointer_to_current_selection() {
	  return(current_selection);
	};
	ProgrammerState kind_of() {
	  return(PROGRAMMER_AT_FLOOR);
   };
   ProgrammerStatus restart_status();
  void set_tool_in_hand(Sprite *new_tool);
  void set_hand_state(HandState new_state) {
     hand = new_state;
  };
  HandState return_hand_state() {
	  return(hand);
  };
  Sprite *pointer_to_tool_in_hand() {
     return(tool_in_hand);
  };
  boolean enable_keyboard_accelerators(char joystick_button_status, char mouse_button_status,
                                       char &button_status, unsigned char &key, boolean &extended_key);
  boolean shift_view(Direction direction, city_coordinate &delta_x, city_coordinate &delta_y);
  void set_containment_region(); 
//  boolean set_containment_region(TTRegion &region);
  void called_for(Sprite *tool);
  void stop_retraining(Robot *robot);
  void update_tool_in_hand(Robot *robot);
  void set_visibility(boolean visible);
  void tool_called_for(Sprite *tool, int sound_id, unsigned char key);
  void set_thought_bubble_robot(Robot *robot) {
	  thought_bubble_robot = robot;
  };
  void set_original_thought_bubble(Thought_Bubble *bubble) {
	  original_thought_bubble = bubble;
  };
  Thought_Bubble *pointer_to_original_thought_bubble() {
	  return(original_thought_bubble);
  };
  void add_item_from_outside(Sprite *item, city_coordinate x, city_coordinate y);
//  void add_items_from_outside(Sprites *items, city_coordinate x, city_coordinate y);
#if TT_XML
  wide_string xml_tag() {
	  return(L"ProgrammerAtFloor");
  };
  void add_xml(xml_element *parent_element, xml_document *document);
  boolean handle_xml_tag(Tag tag, xml_node *node);
  void activate();
#endif
  void dump_deltas(output_stream &stream);
//  void initialize_with_saved_deltas();
  void tool_in_hand_is_ready(Sprite *tool);
  boolean compute_selection_region(TTRegion &region);
  void note_absolute_mode_deltas(coordinate &delta_x, coordinate &delta_y, boolean left_button_down, boolean left_button_just_went_down);
  void completed(); // new on 181102
  private:
//	void compute_grasp_region(TTRegion &region);
	Sprite *selection(boolean even_if_ungraspable=FALSE);
	ProgrammerStatus release_on_top(Sprite *underneath, Sprite *released, boolean ok_to_try_again=TRUE);
   void move_appearance(boolean moved, city_coordinate delta_x, city_coordinate delta_y);
   void move_arm();
	Floor *floor;
	HandState hand;
	flag using_arm;
	flag bombed; // so don't vacuum up stuff after bombing
//	boolean game_element_in_hand_added;
	Sprite *tool_in_hand;
	Thought_Bubble *thought_bubble_entered;
	flag thought_bubble_added_to_floor;
	Thought_Bubble *original_thought_bubble;
	city_coordinate thought_bubble_llx, thought_bubble_lly;
	Robot *thought_bubble_robot;
	Sprite *hand_appearance;
	Sprite *hand_with_wand;
	Sprite *arm;
//	Sprite *colliding_sprite; // saved for call backs
	flag saved_button_unchanged; // was boolean prior to 050103
	Sprite *current_selection;
	Sprite *saved_selection;
	flag saved_modified;
	SelectionReason saved_reason;
	int delta_index;
//	city_coordinate selection_delta_x,selection_delta_y;
	city_coordinate delta_xs[number_of_deltas], delta_ys[number_of_deltas];
	millisecond delta_times[number_of_deltas];
	millisecond release_time;
	flag selection_feedback_flag;
	TTRegion current_region; // current region used to select
	flag moved;
	Sprite *last_released;
   Cubby *cubby_that_started_training;
	Sprite *waiting_for; // called for (e.g. by accelerator key)
	long retraining_stopped_on_frame;
	flag ignore_underneath_on_next_drop;
	flag give_toss_on_next_drop; // new on 100200
	TTRegion selection_region; // new on 220901
	flag use_selection_region;
   int move_count; // new on 130502 for absolute mouse mode
   millisecond time_of_next_scroll_with_absolute_mouse; // moved here on 070602
	city_coordinate current_delta_x, current_delta_y; // new on 010704
//   city_coordinate distance_dragged; // new on 070502
//   flag tool_in_hand_used;
//   city_coordinate grasp_x_offset, grasp_y_offset;
};

Direction move_back_out_of(TTRegion &others_region, TTRegion &my_region,
							      city_coordinate &delta_x, city_coordinate &delta_y);

boolean horizontal_shape(SpriteType type);

class Programmer_Sprite : public Sprite {
  public:
	Programmer_Sprite(SpriteCode code,
							city_coordinate x=0, city_coordinate y=0,
//							Programmer *programmer=NULL,
                     Visible_Background *background=NULL,
							short int initial_parameter=0,
							city_scale current_scale=ground_scale);
//	  void adjust_tool_in_hand() {
//		  programmer->programmer_at_floor()->adjust_tool_in_hand();
//	  };
	  //Programmer *pointer_to_programmer() {
		 // return(programmer);
	  //};
	  boolean is_programmer_appearance() { // new on 201202 to replace use of pointer_to_programmer()
		  return(TRUE);
	  };
	  void recent_speed(city_coordinate &x_speed, city_coordinate &y_speed);
	  void reset_programmer_timer() {
		  tt_programmer->set_timer(0);
	  };
	  void display(SelectionFeedback selection_feedback=NO_FEEDBACK,
						boolean followers_too=TRUE, TTRegion *region=NULL);
	  boolean save_region();
	  void pointer_region(TTRegion &region) {
		  collision_region(region);
	  };
     void completed() { // new on 181102
        tt_programmer->completed();
     };
#if TT_XML
//	  xml_element *xml_create_element(xml_document *document);
	  void xml(xml_element *element, xml_document *document);
#endif
	private:
//	  Programmer *programmer; // made obsolete on 201202
};

SpriteCode sit_sprite();
//boolean programmer_speed_available();
boolean control_button_down();
boolean shift_button_down();
void initialize_button_key_substitutions();
//void release_saved_deltas();

/*
int compute_maximum_orientation_delta(city_coordinate total_cursor_movement,
												  int maximum_rotation_per_second,
												  city_coordinate minimum_movement_to_reorient,
												  millisecond duration);

int compute_new_orientation(int orientation,
									 city_coordinate delta_x,
									 city_coordinate delta_y,
									 int maximum_orientation_delta);
*/

#endif
