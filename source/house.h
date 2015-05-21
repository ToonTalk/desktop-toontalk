// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved
#if !defined(__TT_HOUSE_H)

#define __TT_HOUSE_H

//#include "quadtree.h"
#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif


#if TT_DEBUG_ON
void record_house_counter();
#endif

enum HouseStates
	  {HOUSE_BUILT,EMPTY_LOT,HOUSE_BUILD,HOUSE_UNBUILD,HOUSE_EXPLODE};

//enum HouseTopStates
//	  {HOUSE_TOP_BUILT,EMPTY_LOT_TOP,
//		HOUSE_TOP_BUILD,HOUSE_TOP_UNBUILD,HOUSE_TOP_EXPLODE};

class Screen;
class Room;
class Floor;
class Robot;
class Cubby;
class Block;
class Truck_Outside;
class House; // used before defined
class House_Sprite; // used before defined
//class Sprite;
//class Door_Sprite;

//void initialize_house_parameters(city_coordinate block_width,
//                               city_coordinate block_height);

enum DoorStatus {DOOR_OPEN,DOOR_OPENING,DOOR_CLOSING,DOOR_CLOSED,DOOR_ABOUT_TO_EXPLODE};

class Door : public Sprite {
  public:
	  Door(city_coordinate center_x, city_coordinate center_y, House *house,
		    SpriteCode code);
	  void display(SelectionFeedback selection_feedback=NO_FEEDBACK,
						boolean followers_too=TRUE, TTRegion *region=NULL);
	  void open_door();
	  Room *pointer_to_room();
	  House *pointer_to_house() {
		  return(house);
	  };
	  void set_closed(boolean new_flag) {
		  closed = (flag) new_flag;
	  };
  private:
	House *house;
	flag closed;
};

class House : public Entity {
 public:
  House(city_coordinate x, city_coordinate y, Block *block, HouseStyle style=HOUSE_A, boolean instantly=FALSE);
  ~House();
//  void prepare_for_deletion();
  void built();
  void display(TTRegion *view_region);
  void update_display();
  void finish_instantly();
  void blow_up();
  boolean programmer_in_house();
  void destroyed();
  void remove_from_screen(Screen *screen);
  void open_door();
  void good_lower_left_corner(SpriteCode house_sprite_code,
                              city_coordinate &good_x, city_coordinate &good_y);
  void create_door();
  void initial_contents(Robot *robot, Cubby *cubby);
  boolean showing_from_above();
  Room *pointer_to_room();
  Floor *pointer_to_floor();
  Block *block_im_on() {
	 return(block);
  };
#if TT_XML
  xml_element *xml(xml_document *document);
  boolean handle_xml_tag(Tag tag, xml_node *node);
  xml_element *xml_path(xml_document *document);
  //xml_element *top_level_xml(xml_document *document, boolean generate_toontalk_object=TRUE) {
	 // return(xml(document));
  //};
  void top_level_xml(xml_document *document, xml_element *parent); // updated 300904
#endif
  	SpriteType entity_type() { // new on 040803
		return(WHOLE_HOUSE);
	};
  void dump(output_stream &stream, boolean just_prepare=FALSE);
  void load(InputStream *stream, int notebook_version, NaturalLanguage language);
  void door_collision_region(TTRegion &region) {
	  door_appearance->collision_region(region);
  };
  Picture *picture_of_appearance(boolean from_side);
  Sprites *pointer_to_decorations(boolean from_side);
  Sprite *pointer_to_appearance(boolean from_side);
  void house_center(city_coordinate &x, city_coordinate &y);
  void exit_point(city_coordinate &x, city_coordinate &y);
//  city_coordinate return_min_y() { // just need it for trucks leaving house
//	  return(min_y);
//  };
  void door_sound();
  HouseStyle house_style() {
	 return(style);
  };
  boolean is_built() {
	 return(house_built_flag);
  };
  boolean visible();
  void set_truck_builder(Truck_Outside *truck) {
	 truck_builder = truck;
  };
  void ensure_appearance_from_side();
  void ensure_appearance_from_above();
  void set_decoration(Sprites *sprites, boolean from_side, Sprite *relative_to=NULL, 
							 boolean absolute_coordinates=FALSE, city_coordinate delta_x=0, city_coordinate delta_y=0,
							 boolean dont_record_new_offsets=FALSE);
  void add_decoration(Sprite *sprite, boolean from_side, boolean absolute_coordinates=FALSE, boolean dont_record_new_offsets=FALSE);
  Sprite *pointer_to_door() {
     return(door_appearance);
  };
  boolean is_about_to_be_destroyed() { // new on 020405
	  return(about_to_be_destroyed);
  };
//  boolean is_door_closing();
//  void set_MCI_playing(boolean new_flag) {
//     MCI_playing_flag = (flag) new_flag;
//  };
//  boolean MCI_playing() {
//     return(MCI_playing_flag);
//  };
  // could make the following private and make users a friend
/*
  void set_colors(color house_color, color door) {
	  exterior_color = house_color;
	  door_color = door;
  };
*/
 private:
//  void get_house_image(bytes image, int byte_width, int byte_height);
  void front_view(TTRegion *view_region);
  void view_from_above();
  void close_door();
//  color exterior_color, door_color, roof_color;
  city_coordinate center_x, center_y, min_x, max_x, min_y, max_y;
//  city_coordinate door_width; // since it changes
  House_Sprite *appearance_from_side;
  House_Sprite *appearance_from_above;
//  Sprite *above_display_layer;
  Door *door_appearance;
//  Sprite *lawn_display_layer;
  Room *room;
//  long image_size;
//  int pixel_width, pixel_height;
//  bytes door_parameters;
  millisecond door_changed_time, begin_closing_door_time, house_creation_time;
  DoorStatus door_status;
  HouseStyle style;
  flag about_to_be_destroyed : 1;
  flag house_built_flag : 1;
//  flag MCI_playing_flag : 1;
  Block *block;
  Truck_Outside *truck_builder;
//  Sprites *decoration_from_side;
//  Sprites *decoration_from_above;
  long priority;
};

class House_Sprite : public Sprite {
  public:
	  House_Sprite(city_coordinate center_x, city_coordinate center_y,
						House *house, millisecond house_creation_time,
                  SpriteCode code, short parameter,
                  SpriteType type);
	  boolean house_sprite() {
		  return(TRUE); // new on 080705
	  };
	  void door_collision_region(TTRegion &region) {
		  house->door_collision_region(region);
	  };
	  House *pointer_to_house() {
		  return(house);
	  };
	  void house_built();
	  void house_destroyed();
     void set_priority(long new_priority);
	  boolean like_a_picture() {
		  return(TRUE);
	  };
	  void display(SelectionFeedback selection_feedback=NO_FEEDBACK, // new on 290500
						boolean followers_too=TRUE, TTRegion *region=NULL);
  private:
	House *house;
	long frame_when_last_displayed; // new on 290500
};

typedef House *House_Pointer;

//void release_house_sprites();

class Houses {
  public:
    Houses(House *house, Houses *next=NULL) :
      house(house),
      next(next) {};
    ~Houses();
    House *first() {
      return(house);
    };
    void set_first(House *new_house) {
       house = new_house;
    };
    Houses *rest() {
      return(next);
    };
    void set_rest(Houses *new_next) {
       next = new_next;
    };
  private:
    House *house;
    Houses *next;
};

HouseStyle house_style(SpriteCode code);
SpriteCode house_from_side_sprite(HouseStyle style);
SpriteCode house_from_above_sprite(HouseStyle style);
SpriteCode door_sprite(HouseStyle style);
BrushId roof_brush(HouseStyle style);
long current_house_counter();
//void reset_house_counter();
House *load_house(InputStream *stream, int notebook_version, NaturalLanguage language, city_coordinate x, city_coordinate y, Block *block);
void initialize_house_sizes();
city_coordinate return_house_width();
city_coordinate return_house_height();

#if TT_XML
House *house_at_location(xml_node *node);
#endif

/*

class House_Sprite : public Sprite {
  public:
     House_Sprite(color exterior_color,
                      color roof_color,
                      city_coordinate house_width,
                      city_coordinate house_height,
                      city_coordinate llx,
							 city_coordinate lly,
                      city_coordinate collision_width,
                      city_coordinate collision_height,
                      House *house,
                      Screen *screen);
	  void display(Screen *screen);
     void ref_count_is_zero();
   private:
     dimension pixel_width, pixel_height;
     color exterior_color, roof_color;
};

class Door_Sprite : public Sprite {
   public:
     Door_Sprite(color the_color,
                     city_coordinate door_width,
                     city_coordinate door_height,
                     city_coordinate llx,
                     city_coordinate lly,
                     city_coordinate collision_width,
							city_coordinate collision_height,
                     city_coordinate col_x_offset,
                     city_coordinate col_y_offset,
                     House *house,
                     Screen *screen);
     void display(Screen *screen);
     void ref_count_is_zero();
     int percent_closed() {
        return(the_percent_closed);
     };
     void set_percent_closed(int percent);
   private:
     int the_percent_closed;
     dimension pixel_width, pixel_height;
     color door_color;
};
*/

#endif


