// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved.
#if !defined(__TT_ROOM_H)

#define __TT_ROOM_H
#if !defined(__TT_BCKGRND_H)
#include "bckgrnd.h"
#endif   

class TTRegion;
class Screen;
class Robot;
class Cubby;
class House;
class Block;
class Sprite;
//class Programmer;

class Room : public Visible_Background {
  public:
	 Room(House *house);
//	 ~Room();
    void prepare_for_deletion();
//	 boolean update_display(TTRegion *);
	 boolean display_region();
//    void move_chair(boolean pull_out, millisecond duration);
//    void add_all(Screen *screen);
	 void enter();
//	 void initial_contents(Robot *robot, Cubby *cubby);
	 void house_will_explode(millisecond duration);
	 millisecond when_will_house_explode() {
		 return(explosion_time);
	 };
	 void forget_explosion_time() {
		 explosion_time = 0;
	 };
	 void destroy_house();
//    void house_built();
//    void add_floor(Floor *bench);
	 Floor *pointer_to_floor();
	 House *pointer_to_house() {
		return(house);
	 };
    Floor *create_floor(Cubby *cubby=NULL);
	 Room *pointer_to_room() {
		return(this);
	 };
	 Block *block_im_on();
#if TT_XML
    xml_element *xml(xml_document *document);
    boolean handle_xml_tag(Tag tag, xml_node *node);
	 xml_element *xml_path(xml_document *document);
#endif
    void dump(output_stream &stream, boolean just_prepare=FALSE);
    void load(InputStream *stream, int notebook_version, NaturalLanguage language);
	 void set_programmer(Programmer *new_programmer) {
		programmer = new_programmer;
	 };
	 boolean slated_for_destruction() {
		 return(about_to_be_destroyed);
	 };
	 boolean inside(House *some_house) {
		 return(house == some_house || some_house == NULL);
	 };
    void set_wall_decoration(Sprites *sprites, boolean give_priority=FALSE, boolean subpicture=TRUE);
    void set_wall_decoration_sprite(Sprite *wall_sprite);
//    void just_set_wall_decoration(Sprites *sprites);
    void add_decoration(Sprite *sprite, boolean give_priority=FALSE, boolean subpicture=TRUE);
    Sprite *pointer_to_wall_decoration_sprite() {
       return(wall_decoration);
    };
//    Sprites *pointer_to_wall_decoration() {
//       return(wall_decoration==NULL?NULL:wall_decoration->pointer_to_followers());
//    };
    boolean in_the_room() { // but not on the floor
      return(TRUE);
    };
  private:
    void initialize_wall_decoration();
	 Floor *floor;
	 House *house;
	 Programmer *programmer;
	 flag about_to_be_destroyed;
	 millisecond explosion_time;
    Sprite *wall_decoration;
//    Sprite *door; // noticed obsolete on 150704
	 long frame_when_last_displayed;
};

#endif
