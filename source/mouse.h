// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved
#ifndef __MOUSE_H

#define __MOUSE_H

#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif   
#if !defined(__TT_TOOLS_H)   
#include "tools.h"
#endif   

class Floor;

enum MouseState {MOUSE_RUNNING_NORTHEAST,MOUSE_RUNNING_SOUTHEAST,
					  MOUSE_USING_HAMMER_TO_SMASH,MOUSE_AFTER_SMASH};

class Mouse : public Sprite {
	// doesn't need to be a tool since just runs on its own and can't be grabbed
  public:
	Mouse(city_coordinate initial_x=0, city_coordinate initial_y=0,
			Sprite *integer_to_smash=NULL, Sprite *integer_underneath=NULL,
			Sprite *by=NULL, 
			city_coordinate smashee_center_x=0, city_coordinate smashee_center_y=0,
			boolean save_copy=FALSE, boolean delete_smashee=FALSE,
			Sprite *original_recipient=NULL, Sprite *original_item=NULL);
	 void prepare_for_deletion();
	 void get_smashed();
	 void remove_mouse();
	 void am_smashed();
	 void do_operation();
    void recompute_mouse_priority();
//	 void undo_run_out();
	 void undo_get_smashed();
	 void done_undoing();
	 void undo_am_smashed(Floor *current_floor);
	 void undo_do_operation();
	 boolean undo_information_involves(Sprite *sprite);
	 void release_undo_information();
	 void set_original_smashee(Sprite *the_original_smashee);
	 void set_previous_over_item(Sprite *the_previous_over_item);
	 void set_item_to_smash(Sprite *the_item_to_smash);
	 void set_item_underneath(Sprite *the_item_underneath);
	 void abort();
	 boolean undo_add_follower();
#if TT_XML
     wide_string xml_tag() { // new on 311102 
       return(L"Mouse");
     };
     void xml(xml_element *element, xml_document *document);
     boolean handle_xml_tag(Tag tag, xml_node *node);
#endif
	  boolean ok_to_cache() { // new on 100703 - important for the new time travel scheme for saving cities
		  return(FALSE);
	  };
  private:
	 Sprite *item_to_smash;
	 Sprite *item_underneath;
//	 ActionStatus *action_status;
//	 Background *floor;
//	 Sprite *by; -- replaced by Sprite::saved_by
	 city_coordinate smashee_center_x,smashee_center_y;
	 flag save_copy;
	 flag delete_smashee;
	 Sprite *original_smashee; // noticed this was obsolete on 041002 -- but it isn't!! - 121202
//	 Sprite *smashee_copy;
    Sprite *previous_over_item;
//	 Sprite *previous_under_item; // new on 050701
//	 Sprite *previous_over_item_copy; // new on 100901
//	 flag previous_under_item_active,previous_over_item_copy_active;
    // above replaced by following on 100402
    xml_document *previous_under_item_xml;
    xml_document *previous_over_item_xml;
	 city_coordinate previous_initial_x, previous_initial_y;
	 city_coordinate previous_smashee_center_x,previous_smashee_center_y;
};

class delayed_call_in_a_mouse { // moved here on 121102
  public:
   delayed_call_in_a_mouse(Sprite *under_item, Sprite *over_item,
							      Background *floor, Sprite *by,
							      boolean save_copy,
							      boolean delete_smashee,
                           Sprite *original_recipient,
									Sprite *original_over_item);
  void run();
  private:
    Sprite *under_item;
    Sprite *over_item;
    Background *floor;
    Sprite *saved_by;
//    ActionStatus *action_status;
    boolean save_copy;
    boolean delete_smashee;
    Sprite *original_recipient;
	 Sprite *original_over_item;
};


void call_in_a_mouse(Sprite *under_item, Sprite *over_item,
							Background *floor, Sprite *by,
							boolean save_copy,
							boolean delete_smashee,
                     millisecond delay,
                     Sprite *original_recipient, Sprite *original_over_item);

millisecond mouse_change_value_duration();

#endif
