// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved
#ifndef __TT_TRUCK_H

#define __TT_TRUCK_H

#if !defined(__TT_TOOLS_H)   
#include "tools.h"
#endif   

enum CargoType 
	{TRUCK_ROBOT=0,TRUCK_CUBBY,
    TRUCK_HOUSE,TRUCK_AVENUE,TRUCK_STREET,
    TRUCK_TEXT,TRUCK_NOTEBOOK,TRUCK_UNKNOWN=254};

const int cargo_type_count = TRUCK_NOTEBOOK+1; // new on 020204

//0 is transition to still; 1 is moving west;
//2 is facing north; 3 is moving east again; 4 is facing south;
//5 is facing east; // just to parallel truck above where it is moving east
enum TruckOutsideState // both are same structure now
  {TRUCK_OUTSIDE_WEST=0,TRUCK_OUTSIDE_NORTH,
	TRUCK_OUTSIDE_EAST,TRUCK_OUTSIDE_SOUTH,
	TRUCK_OUTSIDE_EMPTY_WEST,TRUCK_OUTSIDE_EMPTY_EAST};

enum TruckOutsideActivity
  {TRUCK_OUTSIDE_DOING_NOTHING,TRUCK_OUTSIDE_MOVING_TO_STREET,
	TRUCK_OUTSIDE_MOVING_TO_CORNER,TRUCK_OUTSIDE_MOVING_TO_LOT,
	TRUCK_OUTSIDE_MOVING_TO_GOAL,TRUCK_OUTSIDE_BUILDING_HOUSE,
	TRUCK_OUTSIDE_LEAVING,TRUCK_OUTSIDE_WAITING_TO_MOVE_TO_GOAL};

class Robot;
class Cubby;
class Remote_Address;

class Truck : public Sprite {
	public:
	  Truck(SpriteCode code,
	        city_coordinate x, city_coordinate y,
//			  Background *floor,
			  city_coordinate width=0,
			  city_coordinate height=0,
			  Robot *robot=NULL,
			  Cubby *cubby=NULL,
           Text *text=NULL,
           Notebook *notebook=NULL,
           Sprite *saved_by=NULL);
//			  ActionStatus *status=NULL);
//     void prepare_for_deletion();
	  virtual boolean set_cargo(CargoType type, Sprite *item);
     void fill_house(House *new_house);
	  virtual boolean is_outside() = 0; // new on 130205
#if TT_XML
     void xml(xml_element *element, xml_document *document);
     boolean handle_xml_tag(Tag tag, xml_node *node);
#endif
	protected:
	  Robot *robot; 
	  Cubby *cubby;
     Text *text;  // for comments and naming for Java
     Notebook *notebook; // notebook the team should use
//	  ActionStatus *saved_action_status;
//     Sprite *saved_by; // moved up here from Truck_Inside to replace saved_action_status on 171102 then moved to Sprite
};

class Truck_Inside : public Truck {
	public:
	  Truck_Inside(city_coordinate x=0, city_coordinate y=0,
						long initial_priority=0,
						city_coordinate width=0,
						city_coordinate height=0,
						Robot *robot=NULL,
						Cubby *cubby=NULL,
                  Text *text=NULL,
                  Notebook *notebook=NULL,
                  Remote_Address *avenue=NULL,
                  Remote_Address *street=NULL,
                  Picture *house_picture=NULL);
//	  void prepare_for_deletion();
     Sprite *copy(boolean also_copy_offsets=FALSE);
     boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
     void java_before(java_stream &out);
	  void java_after(java_stream &out, const_java_string variable_temp);
     void java(java_stream &out);
     void java_class(java_stream &out);
     string java_start(Cubby *cubby=NULL, Text *text=NULL, Notebook *notebook=NULL,
                       Picture *picture=NULL,
                       Sprites *more_robots_for_picture=NULL, Sprites *more_notebooks=NULL, Sprite *display_this=NULL, java_string extra_text=NULL);
	  ReleaseStatus item_released_on_top(Sprite *item, Sprite *by, SpritePointer &recipient, int level);
     MatchStatus match(Truck_Inside *other_truck, SpritePointerPointer suspension_cell, int version);
     void generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection);
	  void receive_item0();
	  boolean receive_item(Sprite *released, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item);
	  Sprite *select(TTRegion *region, SelectionReason reason, Sprite *in_hand);
	  void remove_selected_element(Sprite *element, SelectionReason reason, Sprite *by, boolean add_to_floor);
	  void start_adding(Sprite *item, millisecond duration);
	  void remove_item(Sprite *item, SelectionReason reason, boolean add_to_floor);
	  void add_robot();
	  void add_cubby();
	  void add_adding();
	  void animate_robot();
	  void animate_cubby();
	  void animate_adding();
	  void set_priority(long new_priority);
	  void replace_truck_and_build();
     void now_on_floor(Background *floor, Sprite *by);
	  void drive_away_and_build();
	  void drive_outside_to(House *house,
									block_number street_number, block_number avenue_number,
									HouseStyle new_house_style);
	  Sprite *respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected, Sprite *by,
									      boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless=FALSE);
	  void correct_cargo_size_and_location(Sprite *cargo);
	  void size_and_location_of(Sprite *cargo,
										 city_coordinate &cargo_width,city_coordinate &cargo_height,
									    city_coordinate &cargo_llx, city_coordinate &cargo_lly);
	  CargoType cargo_type(Sprite *item);
	  Sprite *cargo(CargoType type);
     boolean set_cargo(CargoType type, Sprite *item);
	  boolean acceptable(Sprite *item);
	  Path *path_to(Sprite *target, boolean &ok);
	  boolean equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, SpritesPointer &expected_corresponding_nests, EqualReason reason=NORMAL_EQUAL);
	  void shift_viewpoint(city_coordinate delta_x, city_coordinate delta_y);
	  boolean blankable() {
		return(TRUE); // new on 030105
	 };
	 void become_blank(boolean by_user=FALSE, boolean controlee_too=TRUE); // new on 030105
	 void become_non_blank(boolean controlee_too=TRUE); // new on 030105
#if TT_XML
//     xml_element *xml_create_element(xml_document *document);
	  wide_string xml_tag() { // simpler than the above
		  return(L"Truck");
	  };
     void xml(xml_element *element, xml_document *document);
	  void add_special_xml(xml_element *element, xml_document *document);
     boolean handle_xml_tag(Tag tag, xml_node *node);
#endif
	  boolean is_outside() {
		  return(FALSE);
	  };
//	  boolean empty() {
//		 return(robot == NULL && cubby == NULL);
//	  };
//	  void set_move_after_receiving_item(boolean new_flag) {
//		  move_after_receiving_item = (flag) new_flag;
//	  };
//	  Cubby *pointer_to_cubby() {
//		  return(cubby);
//	  };
//	  Robot *pointer_to_robot() {
//		  return(robot);
//	  };
	private:
	  Remote_Address *street;
	  Remote_Address *avenue;
	  Picture *house_picture;
	  Sprite *adding;
//	  flag move_after_receiving_item : 1; // noticed it was obsolete on 231202
};

class Truck_Outside : public Truck {
	public:
	  Truck_Outside(city_coordinate x=0, city_coordinate y=0,
						 boolean from_above=TRUE,
						 city_coordinate width=0, city_coordinate height=0,
						 Robot *robot=NULL, Cubby *cubby=NULL, Text *text=NULL, Notebook *notebook=NULL,
						 block_number goal_street=0, block_number goal_avenue=0,
						 HouseStyle house_style=HOUSE_A,
						 Block *block=NULL,
//						 ActionStatus *action_status=NULL,
                   Sprite *saved_by=NULL,
						 short int lot_number=-1);
//						 House *house=NULL);
     void release_all_references();
	  void display(SelectionFeedback selection_feedback=NO_FEEDBACK,
						boolean followers_too=TRUE, TTRegion *region=NULL);
     void start_activity(TruckOutsideActivity new_activity);
	  millisecond drive_horizontally_to(city_coordinate new_llx, AnimationCallBack callback=CALLBACK_NOTHING);
     millisecond delay_before_leaving();
	  void truck_sound(city_coordinate new_llx, city_coordinate new_lly);
	  void drive_to_next_block();
	  void turn_to(Direction direction);
	  void switch_to(block_number avenue, block_number street);
	  void drive_to_free_lot();
     boolean update_goal();
	  void build_house(boolean have_checked_lot_free);
	  void leave_since_house_built();
	  void destroy_truck(boolean fill_house_too=TRUE);
	  millisecond driving_duration(city_coordinate distance);
	  boolean side_view() {
		 return(appearance_code() == TRUCK_SIDE_SPRITE);
	  };
	  void reset_house() {
//		  house = NULL;
		  lot_number = -1; // new on 261202 to indicate no house (has blown up)
	  };
     boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
	  boolean outside_truck() {
		  return(TRUE);
	  };
     long current_size_percent() { // new on 131102
        return(size_percent);
     };
     void just_set_size_percent(long new_percent) { // new on 131102
        size_percent = new_percent;
     };
#if TT_XML
	  wide_string xml_tag();
	  xml_element *xml_set_special_attributes(xml_element *element, xml_document *document);
	  boolean xml_get_special_attributes(xml_node *node);
	  void set_current_block(Block *block);
	  boolean geometry_worth_saving() { // new on 261202
		  return(TRUE);
	  };
#endif
	  boolean is_outside() {
		  return(TRUE);
	  };
	private:
//	  Robot *robot;
//	  Cubby *cubby;
	  block_number goal_avenue, goal_street;
	  HouseStyle house_style;
	  Block *current_block;
	  short int lot_number;
	  flag east_west_next : 1;
	  flag size_is_100_percent : 1;
	  TruckOutsideActivity activity; // only need 3 bits but afraid of compiler bug
	  long size_percent; // since animated must be long for now
//     House *house; // recomputed as needed beginning 261202
//     city_coordinate immediate_destination_x, immediate_destination_y;
};

boolean robot_loaded_in_truck();

Truck *load_truck(InputStream *pad_in, 
                  int notebook_version, NaturalLanguage language,
                  Sprite *container, Picture *controlee, long index, boolean descendants_inactive);
boolean describe_truck(InputStream *stream, output_stream &text_out, int version_number, Article article);

#endif
