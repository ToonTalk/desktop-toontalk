// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved.
#if !defined(__TT_BACKGROUND_H)

#define __TT_BACKGROUND_H

class TTRegion;
class Screen;
class Sprite;
class Sprites;
class Leaders;
class Vacuum;
class Copier;
class Expander;
class Notebook;
class Room;

class Path;
class Cubby;

class Picture;
class House;
class Block;
class Mouse;

#if !defined(__TT_EVENT_H)   
#include "event.h"
#endif 

//class Backgrounds : public Entity { // new on 090804
//public:
//	Backgrounds(Background *background, Backgrounds *backgrounds=NULL);
//	~Backgrounds();
//protected:
//  Background *first;
//  Backgrounds *next;
//};

class Background : public Entity {
  public:
	Background();
#if TT_DEBUG_ON
	virtual ~Background();
#endif
	virtual boolean display_region() {
       return(TRUE);
   };
   virtual void finish_instantly(); // was no-op at this level of the class hierarchy prior to 030903
	virtual boolean update_display(TTRegion *view_region) {
		return(TRUE);
	};
	virtual void now_on_screen() {
		// hook for say add items to screen
	};
#if TT_XML
   virtual void add_xml(xml_element *element, xml_document *document);
	virtual void add_xml_attributes(xml_element *element, xml_document *document);
   boolean handle_xml(string file_name);
	boolean handle_xml(xml_document *document);
   boolean handle_xml_tag(Tag tag, xml_node *node);
	virtual xml_element *xml_path(xml_document *document) {
		return(NULL);
	};
	virtual void set_xml_after_titles(xml_document *node); // was xml_node prior to 040504
	virtual wide_string xml_tag() {
		return(L"NoXMLTag");
	};
	void top_level_xml(xml_document *document, xml_element *parent); // boolean generate_toontalk_object);
#endif
   void dump_to_clipboard(); //bytes buffer=NULL, long buffer_size=0);
   virtual void dump(output_stream &stream, boolean just_prepare=FALSE);
	void increment_screen_count() {
		screen_count++;
	};
	void decrement_screen_count() {
		screen_count--;
	};
	virtual void propagate_changes() {};
   void add_items(Sprites *items, boolean give_priority=TRUE, boolean change_priority=FALSE, boolean warn=TRUE);
	void reset_items(Sprites *new_items);
	void just_add_items(Sprites *new_items, boolean add_to_screen_too=FALSE);
   void add_copy_of_items(Sprites *items, boolean give_priority=TRUE,
	                       boolean change_priority=FALSE,
                          boolean warn=TRUE);
	virtual boolean add_item(Sprite *item, boolean give_priority=TRUE,
	                         boolean change_priority=FALSE,
                            boolean warn=TRUE);
	boolean has_item(Sprite *sprite);
	virtual boolean remove_item(Sprite *sprite, boolean report_problems=TRUE,
										 boolean remove_from_screen=TRUE,
										 boolean reset_background=TRUE);
	void remove_all();
//	virtual void eventually_delete(Sprite *sprite);
	virtual void recursively_add_leader(Sprite *item, boolean warn=TRUE) {
	};
	virtual void add_leader(Sprite *leader, boolean warn=TRUE) {
	  // nothing by default
	};
	virtual void remove_leader(Sprite *leader, boolean warn=FALSE) {
	};
	virtual void recursively_remove_leader(Sprite *item, boolean warn=FALSE) {
	};
	virtual boolean inside_rule_body() {
		return(FALSE);
	};
	virtual long next_tool_priority() {
		return(0); // shouldn't matter
	};
	virtual long next_item_priority() {
		return(0); 
	};
	virtual void give_item_priority(Sprite *sprite) {};
	virtual millisecond default_duration(millisecond default_duration=1000) {
		return(0);
	};
	virtual boolean showing_on_a_screen() {
		return(FALSE);
	};
	virtual boolean on_the_floor() {
		return(FALSE);
	};
	virtual boolean floor_or_rule_body() { // new on 070103
		return(FALSE);
	};
	boolean in_the_room_with_programmer(); // new on 020205
   virtual boolean in_the_room() { // but not on the floor
      return(FALSE);
   };
	virtual boolean inside_a_house() {
		return(TRUE); // most backgrounds are
	};
	virtual boolean is_titles_background() {
		return(FALSE);
	};
	virtual boolean directed_help_ok() {
		return(FALSE);
	};
	virtual Block *block_im_on() {
		return(NULL);
	};
	virtual House *pointer_to_house() {
		return(NULL);
	};
	virtual void house_will_explode(millisecond duration) {};
	virtual void destroy_house() {};
	virtual void add_tools(Vacuum *new_vacuum,
								  Copier *new_copier,
								  Expander *new_expander,
								  Notebook *new_notebook) {};
	virtual void remove_tools() {};
	virtual Room *pointer_to_room() {
		return(NULL);
	};
	virtual boolean slated_for_destruction() {
	  return(FALSE);
	};
   virtual void prepare_for_deletion();
   void destroy();
	virtual Cubby *pointer_to_body_cubby() {
		// or signal an error??
		return(NULL);
	};
   virtual void set_body_cubby(Cubby *new_cubby, Sprite *by) {};
	virtual void add_to_working_set(Sprite *item) {};
	virtual void remove_last_n_events(int count) {};
	virtual boolean known_leader(Sprite *item) {
		return(FALSE);
	};
	virtual boolean inside(House *some_house=NULL) {
	  	return(FALSE);
	};
	virtual Picture *pointer_to_picture() {
	  return(NULL);
	};
   // following moved here from floor.cpp on 191102
   boolean decrement_ref_count(); 
   void increment_ref_count() {
      ref_count++;
   };
   boolean deletion_pending() {
     return(deletion_pending_flag);
   };
	boolean ok_to_delete() { // restored on 240804
		// only used by Room::prepare_for_deletion() -- not clear this is useful anymore
//      deletion_pending_flag = TRUE;
      return(ref_count <= 1); // 1 is OK since destroy will decrement the final count
   };
   //boolean ok_to_delete() { // commenting out was a test on 220103
   //   deletion_pending_flag = TRUE;
   //   return(ref_count == 0);
   //};
   //virtual boolean decrement_ref_count() {
   //   return(TRUE);
   //};
   //virtual void increment_ref_count() {
   //};
   //virtual boolean deletion_pending() {
   //  return(FALSE);
   //};
//	virtual Path *compute_path(Sprite *subject, EventCode code) {
//		return(NULL);
//	};
	Sprites *pointer_to_items() {
		return(items);
	};
   void set_initial_items(Sprites *list);
   virtual short int return_background_index() {
      return(0);
    };
   virtual void set_dirty() {
   };
   virtual boolean outside() {
     return(FALSE);
   };
   virtual boolean on_the_ground() {
		return(FALSE);
   };
   virtual boolean inside_thought_bubble() {
      return(FALSE);
   };
   void shift_all(city_coordinate delta_x, city_coordinate delta_y);
   virtual void up_against_the_wall(boolean at_left_wall, boolean at_right_wall, boolean at_front_wall) {
   };
   virtual Floor *pointer_to_floor() {
     return(NULL);
   };
	virtual void set_current_priority_if_lower(long new_priority) {
	};
	virtual Sprite *pointer_to_wall_decoration_sprite() { // new on 290500
      return(NULL);
   };
	virtual void set_previous_mouse(Mouse *mouse) {
		// warn?
	};
	virtual void forget_previous_mouse() {
		// warn?
	};
	//virtual void undo_previous_event() { // noticed obsolete on 070103
	//	// warn??
	//};
   virtual void restore_toolbox(boolean move_toolbox_offscreen_first=TRUE) {
      // new on 131102 to deal better with animation callbacks
   };
	virtual void destroy_previous_mouse_if_involving(Sprite *sprite) {
	};
	virtual city_coordinate wall_lly() {
		return(max_long);
	};
	virtual void add_alternative_body_cubby(Cubby *cubby) {
		// only floor cares about this
	};
	virtual void need_new_main_cubby() {
		// only floor deals with this
	};
	//virtual  boolean ok_to_fix_containment_active() { // new on 080404
	//	return(FALSE);
	//};
#if TT_DEBUG_ON
	void display_debug_info();
#endif
//   virtual void set_ok_for_toolbox_entry(boolean new_flag) {
//   }; // error??
//   virtual boolean ok_for_toolbox_entry() {
//      return(TRUE);
//   };
#if TT_DEBUG_ON
   long debug_counter;
	flag prepared_for_deletion;
#endif
  protected:
	Sprites *items;
	int screen_count;
	Sprites *postponed_removals;
   // following moved here on 191102 since birds might return to backside of a picture (and then what about rooms?)
   unsigned int ref_count; // at least for birds so they don't return to destroyed floors 
   flag deletion_pending_flag : 1;
};

class Backside_Background : public Background {
  public:
	Backside_Background(Picture *picture) :
	  Background(),
	  picture(picture) {};
   boolean remove_item(Sprite *sprite, boolean report_problems=TRUE,
										 boolean remove_from_screen=TRUE,
										 boolean reset_background=TRUE);
	Picture *pointer_to_picture() {
	  return(picture);
	};
	Room *pointer_to_room();
  private:
	Picture *picture;
};

class Visible_Background : public Background {
  public:
	Visible_Background();
//	~Visible_Background();
   void prepare_for_deletion();
	boolean display_region();
   void remove_postponed_removals();
	boolean update_display(TTRegion *view_region);
	void propagate_changes();
 	void now_on_screen();
//	virtual void record_event(EventCode code, Sprite *by, 
//									  Tool *subject, char key=0) {
//	};
	virtual boolean inside_rule_body() {
	  return(FALSE); // by default
	};
	virtual Sprites *pointer_to_working_set() {
	  return(NULL);
	};
//	virtual void add_to_working_set(Tool *tool) {};
	boolean add_item(Sprite *item, boolean give_priority=TRUE,
                    boolean change_priority=FALSE, boolean warn=TRUE);
	boolean remove_item(Sprite *sprite, boolean report_problems=TRUE,
							  boolean remove_from_screen=TRUE,
							  boolean reset_background=TRUE);
	void remove_all(); 
	void remove_all_leaders() {
		leaders = NULL;
	};
	void recursively_add_leader(Sprite *item, boolean warn=TRUE);
	void add_leader(Sprite *leader, boolean warn=TRUE);
	void remove_leader(Sprite *leader, boolean warn=FALSE);
	void recursively_remove_leader(Sprite *item, boolean warn=FALSE);
	boolean known_leader(Sprite *item);
	boolean showing_on_a_screen() {
		return(screen_count > 0);
	};
	millisecond default_duration(millisecond default_duration=1000);
   void set_dirty() {
      clean = FALSE;
   };
  	void give_item_priority(Sprite *sprite);
	long next_item_priority() {
	  return(current_priority--);
	};
	long next_tool_priority() {
		// obsolete: was so tools can float about items...
		return(current_priority--);
	};
	void set_current_priority_if_lower(long new_priority);
 	void bring_to_top(Sprite *sprite);
   void set_containment_region(TTRegion &region) {
      containment_region = region;
      containment_active = TRUE;
   };
   //boolean current_containment_region(TTRegion &region) { // noticed obsolete 190404
   //   region = containment_region;
   //   return(containment_active);
   //};
   void set_containment_active(boolean flag) {
      containment_active = flag;
   };
#if TT_XML
//	void add_xml(xml_element *element, xml_document *document);
	void add_xml_attributes(xml_element *element, xml_document *document);
	boolean handle_xml_tag(Tag tag, xml_node *node);
#endif
  protected:
	Leaders *leaders;
   long current_priority; //, current_tool_priority;
	flag updating_items : 1; // not clear if this should be an instance variable (and whether when set best to use UnwindProtect) - asked 031103
   flag clean : 1;
   flag containment_active : 1;
   TTRegion containment_region;
//	Screen *screen;  // should really be screens
};

//class Under_Sprite_Background : public Background {
//  public:
//	Under_Sprite_Background(Picture *sprite_on_top) :
//	  Background(),
//	  sprite_on_top(sprite_on_top) {};
//	  Room *pointer_to_room();
//  protected:
//	  Picture *sprite_on_top;
//};

class Titles_Background : public Visible_Background {
  public:
	 Titles_Background(short int image_index);
//	 ~Titles_Background();
    void prepare_for_deletion();
	 boolean update_display(TTRegion *view_region);
	 void set_text(const_string new_text);
    void set_text2(const_string new_text);
	 boolean display_region();
    void set_index(short int index) {
       image_index = index;
       clean = FALSE;
    };
    void set_y_offset_percent(short int offset) {
       y_offset_percent = offset;
    };
    short int return_background_index() {
      return(image_index);
    };
    void set_text_color(color_index new_color) {
       text_color = new_color;
    };
	 boolean is_titles_background() { // new on 160801
		 return(TRUE);
	 };
  private:
	 string text;
    string text2;
	 flag clean;
    short int image_index;
    short int y_offset_percent;
    color_index text_color;
};
	 

/*
class Backgrounds {
  public:
	 Backgrounds(Background *background, Backgrounds *next) :
		background(background),
		next(next) {};
	 ~Backgrounds();
	 void remove(Background *removing);
//	 Background *first() {
//		return(background);
//	 };
//	 Backgrounds *rest() {
//		return(next);
//	 };
  private:
	 Background *background;
	 Backgrounds *next;
};
*/

#endif
