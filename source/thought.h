// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.
#ifndef __TT_THOUGHT_H

#define __TT_THOUGHT_H

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_TOOLS_H)   
#include "tools.h"
#endif   

//class Sprite;
class Screen;
//class Cubby;
class Robot;

class Thought_Bubble : public Sprite {
   public:
     Thought_Bubble(city_coordinate center_x=0, city_coordinate center_y=0, // defaults added 140101
                    long initial_priority=0,
//                    Background *floor,
						  city_coordinate width=0, city_coordinate height=0
//                    Cubby *cubby=NULL
						  );
     Sprite *copy(boolean also_copy_offsets=FALSE);
//	  void first_follower_is_cubby();
//     ~Thought_Bubble();
//     void generate_help(output_stream &message, Sprite *tool_in_hand);
     void display(SelectionFeedback selection_feedback=NO_FEEDBACK,
						boolean followers_too=TRUE, TTRegion *region=NULL);
     void describe(output_stream &message, Article article=INDEFINITE_ARTICLE);
	  Sprite *select(TTRegion *region, SelectionReason reason, Sprite *in_hand);
	  Sprite *selected(SelectionReason reason, Sprite *by);
	  void remove_selected_element(Sprite *element, SelectionReason reason, Sprite *by, boolean add_to_floor);
//	  Sprite *appearance_before_vacuuming();
//	  void reset_appearance_before_vacuuming();
//     void update_display();
//	  void sucking_up();
	  ReleaseStatus item_released_on_top(Sprite *item, Sprite *by, SpritePointer &recipient, int level);
//	  void receive_cubby_again();
	  boolean receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item);
	  void receive_cubby(Cubby *cubby, boolean center_cubby=TRUE);
	  void center_and_resize_cubby();
	  void signal_size_and_location();
	  Cubby *pointer_to_cubby() {
		  return(cubby);
	  };
	  void set_cubby(Cubby *new_cubby, boolean also_add_follower=TRUE);
	  Robot *pointer_to_robot() {
		  return(robot);
	  };
	  Sprite *now_inside_thought_bubble(boolean in_training);
	  void set_robot(Robot *new_robot) {
		  robot = new_robot;
	  };
	  void adjust_max_size(city_coordinate &max_width,
								  city_coordinate &max_height) {
		  max_width *= 2;
		  max_height *= 2;
	  };	  
//     void adjust_grasp(city_coordinate &adjustment_x,
//                       city_coordinate &adjustment_y);
     boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
	  void set_active(boolean new_flag, boolean recur=TRUE, boolean initialization=FALSE) { // new on 211299 so don't recur inside thought bubbles
        active_flag = TRUE; // can never become inactive -- new on 151002
	  };
	  void just_set_inactive() {
		 // ignore -- new on 140605
	  };
	  boolean equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, SpritesPointer &expected_corresponding_nests, 
							 EqualReason reason=NORMAL_EQUAL);
#if TT_XML
//     xml_element *xml_create_element(xml_document *document);
	  wide_string xml_tag() {
		  return(L"ThoughtBubble");
	  };
	  void xml(xml_element *element, xml_document *document);
     boolean handle_xml_tag(Tag tag, xml_node *node);
#endif
	  void recursively_activate_pictures(boolean reset_animation=TRUE, PictureActivateContext context=PICTURE_ACTIVATE_CONTEXT_NOT_INTIALIZED) {
		// new on 011204 since nothing inside a robot's thought bubble should be activated
	  };
     //     void display(Screen *screen);
//	  void set_move_after_sucking_up(boolean new_flag) {
//        move_after_sucking_up = (flag) new_flag;
//	  };
//  void set_alive(boolean flag);
/*
	  void fully_expanded() {
		  acting_like_background = TRUE;
	  };
	  void not_fully_expanded() {
		  acting_like_background = FALSE;
	  };
*/
	private:
	 Cubby *cubby;
	 Robot *robot;
//	 flag cubby_vacuumed : 1;
//	 Cubby *item_being_sucked_up;
//	 flag move_after_sucking_up : 1;
// acting_like_background : 1;
//    boolean sucking_up_done;
//    Floor *floor;
};
#endif
