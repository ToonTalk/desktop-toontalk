// Copyright (c) 1992-2007.  Ken Kahn, Animated Programs, All rights reserved.
#if !defined(__TT_FLOOR_H)

#define __TT_FLOOR_H

#if !defined(__TT_BCKGRND_H)   
#include "bckgrnd.h"
#endif   
#if !defined(__TT_SCREEN_H)   
#include "screen.h"
#endif   
#if !defined(__TT_REGION_H)   
#include "region.h"
#endif   
#if !defined(__TT_TOOLS_H)   
#include "tools.h"
#endif   
#if !defined(__TT_SPRITE_H)
#include "sprite.h"
#endif
#if !defined(__TT_EVENT_H)   
#include "event.h"
#endif
#if !defined(__TT_MOUSE_H)   
#include "mouse.h"
#endif   

//class Vacuum;
//class Expander;
//class Copier;
//class Sprite;
class Cubby;
class Number;
class Nest;
class Truck_Inside;
class Room;
class Notebook;
class Path;


class Floor : public Visible_Background {
  public:
	Floor(short int floor_id=NULL, Room *room=NULL, Cubby *thought_bubble_cubby=NULL, Robot *initial_robot=NULL); // added optional args on 070103
	void Floor::prepare_for_deletion();
//	~Floor();
//	void eventually_delete(Sprite *sprite);
//   boolean on_copier(Sprite *sprite);
//	void in_thought_bubble(Cubby *thought_bubble_cubby);
//	void leaving_thought_bubble();
	SpriteType entity_type() { // new on 040803
		return(WHOLE_FLOOR);
	};
	boolean display_region();
   void display_walls();
	city_coordinate wall_lly();
	boolean update_display(TTRegion *);
	void set_initial_robot(Robot *robot);
	void initial_contents(Robot *robot, Cubby *cubby);
   void house_built();
//	boolean animate_though_offscreen();
	void restore_toolbox(boolean move_toolbox_offscreen_first=TRUE);
//	void remove_toolbox();
//   void about_to_redisplay() {
//      contents_displayed = FALSE; // but will be soon
//   };
//   void increment_screen_count();
	void remove_last_n_events(int count);
	void cubby_is_busy();
	void cubby_is_not_busy();
	void house_will_explode(millisecond duration);
	void destroy_house();
	Block *block_im_on();
	House *pointer_to_house();
//   void body_defined();
	boolean inside_rule_body() {
		// does this ever return something different from inside_thought_bubble ?
		return(body_cubby != NULL);
	};
	boolean on_the_floor() {
		return(body_cubby == NULL);
	};
	boolean floor_or_rule_body() { // new on 070103
		return(TRUE);
	};
	boolean directed_help_ok() {
		return(TRUE);
	};
	Room *pointer_to_room() {
		return(room);
	};
	void set_room(Room *new_room) {
		room = new_room;
	};
	boolean slated_for_destruction();
//	Sprites *pointer_to_items() {
//		return(items);
//	};
	Cubby *pointer_to_body_cubby() {
	  return(body_cubby);
	};
	void set_body_cubby(Cubby *new_cubby, Sprite *by);
//	void add_tools(Vacuum *new_vacuum,
//						Copier *new_copier,
//						Expander *new_expander,
//						Notebook *new_notebook);
//	void remove_tools();
	Path *compute_path(Sprite *subject, EventCode code, Robot *robot,
							 boolean notebook_page_really_selected=FALSE
#if TT_POST3187
						    ,Sprite *original_subject=NULL
#endif							 
							 );
	boolean inside(House *some_house=NULL);
   void up_against_the_wall(boolean at_left_wall, boolean at_right_wall, boolean at_front_wall);
   Floor *pointer_to_floor() {
      return(this);
   };
#if TT_XML
	virtual wide_string xml_tag() {
		return(L"Floor");
	};
   xml_element *xml(xml_document *document);
   boolean handle_xml_tag(Tag tag, xml_node *node);
	xml_element *xml_path(xml_document *document);
	Floor *interpret_path(xml_node *node);
#endif
   void dump(output_stream &stream, boolean just_prepare=FALSE);
   void load(InputStream *stream, int notebook_version, NaturalLanguage language);
   boolean inside_thought_bubble() {
      return(floor_id == THOUGHT_BUBBLE_BACKGROUND);
   };
	void set_previous_mouse(Mouse *mouse);
	void forget_previous_mouse() { // new on 171202
		previous_mouse = NULL; // don't do all the stuff that set_previous_mouse does
	};
	void undo_previous_mouse();
	void destroy_previous_mouse_if_involving(Sprite *sprite);
	void add_alternative_body_cubby(Cubby *cubby);
	void need_new_main_cubby();
//	void undo_previous_event();
//   void set_ok_for_toolbox_entry(boolean new_flag) {
//      ok_for_toolbox_entry_flag = (flag) new_flag;
//   };
//   boolean ok_for_toolbox_entry() {
//      return(ok_for_toolbox_entry_flag);
//   };
//	Path *compute_path(int index);
  private:
//    Sprites *saved_items; // items before entering thought bubble
	 Cubby *body_cubby;
	 Sprites *alternative_body_cubbies; // new on 151204
	 Room *room;
	 Robot *initial_robot;
	 Mouse *previous_mouse; // new on 080701
    short int floor_id;
	 ToolboxCacheStatus toolbox_status;
//	 Cubby *initial_cubby;
    boolean at_left_wall : 1;
    boolean at_right_wall : 1;
    boolean at_front_wall : 1;
	 
//	 Events *previous_undoable_event;
//    flag ok_for_toolbox_entry_flag : 1;
//	 Sprites *deleted_sprites;
};

class Ground : public Floor {
  public:
    Ground() :
      Floor(FLOOR4_BACKGROUND,NULL,NULL) {
    };
    boolean inside(House *some_house=NULL) {
      return(FALSE);
    };
    boolean outside() {
      return(TRUE);
    };
    boolean inside_rule_body() {
      return(FALSE);
    };
    boolean on_the_floor() {
		return(FALSE);
    };
    boolean on_the_ground() {
		return(TRUE);
    };
    Room *pointer_to_room() {
		return(NULL);
	 };
    boolean slated_for_destruction() {
      return(FALSE);
    };
#if TT_XML
	 wide_string xml_tag() {
		 return(L"Ground");
	 };
#endif
};

void record_event(EventCode code, Sprite *by, Background *floor=NULL, 
						Sprite *subject=NULL, boolean type_of_subject_fixed=FALSE,
						int key=0, // was char prior to 100604
						AddToWorkingSet add_to_working_set=ADD_TO_WORKING_SET_IF_NEW,
						boolean notebook_page_really_selected=FALSE,
						int label_index=-1
#if TT_POST3187
						,Sprite *original_subject=NULL
#endif
						);

#if TT_XML
boolean handle_return_floor(Entity *bird, Tag tag, xml_node *node, void *extra_info);
#endif

#endif
