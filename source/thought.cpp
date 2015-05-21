// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif   
#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif   
#if !defined(__TT_TOOLS_H)   
#include "tools.h"
#endif   
#if !defined(__TT_SCREEN_H)   
#include "screen.h"
#endif
#if !defined(__TT_WINMAIN_H)
#include "winmain.h"
#endif
#if !defined(__TT_CUBBY_H)   
#include "cubby.h"
#endif   
#if !defined(__TT_ROBOT_H)   
#include "robot.h"
#endif   
#if !defined(__TT_BCKGRND_H)   
#include "bckgrnd.h"
#endif   
#if !defined(__TT_FLOOR_H)   
#include "floor.h"
#endif   
#if !defined(__TT_HELP_H)   
#include "help.h"
#endif   
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif
#if TT_XML
#if !defined(__TT_XML_H)
#include "xml.h"
#endif
#endif
#if !defined(__TT_THOUGHT_H)
#include "thought.h"
#endif 

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// too expensive to animate this without much payoff
//enum BubbleState
//	{BUBBLE_INERT, BUBBLE_GROWS_TO_ALIVE, BUBBLE_SHRINKS, BUBBLE_ALIVE};

//void receive_cubby_again_callback(void *t) {
//	((Thought_Bubble *) t)->receive_cubby_again();
//};

//void sucking_up_callback(void *sprite);

Thought_Bubble::Thought_Bubble(city_coordinate center_x,
										 city_coordinate center_y,
										 long initial_priority,
										 city_coordinate width,
										 city_coordinate height) :
	Sprite(THOUGHT_BUBBLE_SPRITE,
		    center_x,center_y,THOUGHT_BUBBLE,
		    0,ground_scale,width,height),
	cubby(NULL),
	robot(NULL) {
//	item_being_sucked_up(NULL)
//	move_after_sucking_up(FALSE)
//	acting_like_background(FALSE)
//   sucking_up_done(FALSE) {
//	set_priority_function_of_lly(FALSE);
   set_priority_fixed(TRUE); // taken care of by the robot "owner"
	current_priority = initial_priority;
};

// delete cubby now since is follower
//Thought_Bubble::~Thought_Bubble() {
//  if (//!acting_like_background &&
//		cubby != NULL) {
//	  delete cubby;
//  };
//};

Sprite *Thought_Bubble::copy(boolean also_copy_offsets) {
	Sprite *copied = already_copied(this); // new on 210203
   if (copied != NULL) {
		return(copied);
	};
	Thought_Bubble *copy = new Thought_Bubble(llx,lly,current_priority,width,height); // moved here on 210203 so can deal with circularities better
//	just_copied(this,copy); -- moved into top_level_copy on 210203
// some reason this wasn't really centering it appropriately
// reason is that the bb is wrong since it picked up the first little bubble
//	city_coordinate center_x, center_y;
//	center_location(center_x,center_y);
	Cubby *cubby_copy = NULL;
	// setting and resetting the following flag is not very elegant
	// but since it is an atomic operation there is no problem
	// used by numbers to determine if ok to copy blank to blank
	// and blank cubbies
	//Sprite *saved_tt_top_copy = tt_top_copy; // commented out on 210203 since now uses top_level_copy
	//tt_top_copy = NULL; // no sharing inside of thought bubbles
	UnwindProtect<boolean> set(tt_copying_entire_thought_bubble,TRUE); // in case a robot is in a thought bubble
	UnwindProtect<boolean> set2(tt_dont_connect_things_while_copying,TRUE);
	if (cubby != NULL) { // && cubby->pointer_to_leader() == this) { // commented out on 250103
		cubby_copy = (Cubby *) (cubby->top_level_copy(TRUE,this,copy)); // made top_level on 100203
	};
//   Robot *robot_copy = NULL;
//   if (robot != NULL) {
//      robot_copy = (Robot *) (robot->copy());
//   };
//	if (cubby_copy != NULL) { // commented out on 250103
////		copy->update_display(); // for the receive_cubby to work right
//		copy->receive_cubby(cubby_copy);
//	};
//	copy->move_to(llx,lly);
//	if (saved_width != 0) {
//		copy->set_saved_size(saved_width,saved_height);
//	};
//	tt_copying_entire_thought_bubble = FALSE;
	// replaced the above with the following on 201299
//	tt_top_copy = saved_tt_top_copy;
//   copy->set_priority_fixed(priority_fixed_flag);
	if (cubby_copy != NULL) { // condition new on 280103 - thanks Gordon
		copy->set_cubby(cubby_copy,FALSE);
		copy->add_follower(cubby_copy,TRUE,INSERT_AT_END,TRUE);
		cubby_copy->inside_thought_bubble();
	};
	finish_making_copy(copy,also_copy_offsets); 
	return(copy);
};

//void Thought_Bubble::first_follower_is_cubby() {
//	// shouldn't be any other followers but worth checking??
//	cubby = (Cubby *) followers->first();
//	cubby->inside_thought_bubble();
//};

void Thought_Bubble::display(SelectionFeedback selection_feedback,
						           boolean followers_too, TTRegion *region) {
//	if (rescale_if_camera_above(selection_feedback,followers_too,region)) return; -- commented out on 160804
   Sprite::display(selection_feedback,(followers_too && !blank),region);
   if (robot == NULL) return;
   string steps_left = robot->steps_left_string();
   if (steps_left != NULL) {
      city_coordinate counter_llx = llx+width/8;
      city_coordinate counter_lly = lly+(7*height)/10;
      city_coordinate font_width = width/8;
      city_coordinate font_height = 2*font_width;
      if (want_feedback(selection_feedback)) {
		    counter_llx += selection_delta_x();
          counter_lly += selection_delta_y();
      };
BEGIN_GDI
	   tt_screen->text_out(steps_left,_tcslen(steps_left),
                          counter_llx,counter_lly,
							     font_width,font_height,TRUE,FALSE,TRUE);
END_GDI
      delete [] steps_left;
   };
};

//void Thought_Bubble::java(output_stream &out, string variable) {
//  if (cubby == NULL) {
//     out << "Nothing;";
//  } else {
//     cubby->java(out,variable);
//  };
//};

void Thought_Bubble::describe(output_stream &message, Article ) {
   // I don't believe this is called anymore since thought bubbles can't be separated from robots
	if (cubby == NULL) { //  || cubby->pointer_to_leader() == NULL) {
	   // there is no encoding so need to do this
//		print_article(article,message,S(IDS_EMPTY_THOUGHT_BUBBLE_DESCRIPTION));
      message << S(IDS_EMPTY_THOUGHT_BUBBLE_DESCRIPTION);
	} else {
//		print_article(article,message,S(IDS_THOUGHT_BUBBLE_DESCRIPTION));
      message << S(IDS_THOUGHT_BUBBLE_DESCRIPTION);
		cubby->describe(message);
	};
};

/*
Cubby *Thought_Bubble::cubby_copy() {
	if (cubby == NULL) { // can this happen?
		return(NULL);
	} else {
		return((Cubby *) (cubby->copy()));
	};
};
*/

ReleaseStatus Thought_Bubble::item_released_on_top(Sprite *item, Sprite *by, 
																	SpritePointer &recipient, int level) {
//  if (acting_like_background) {
//	  return(RELEASE_IGNORED);	  
//  };
//  if (infinite_stack_flag) {
//	  switch (item->kind_of()) {
//		  case THOUGHT_BUBBLE:
//			  if (((Thought_Bubble *) item)->pointer_to_cubby() == NULL) {
//				  return(RELEASED_ON_ITS_STACK);
//			  };
//			  break;
//		  case CUBBY:
//			  Background *floor = ((Cubby *) item)->pointer_to_background();
//			  Thought_Bubble *copy = (Thought_Bubble *) this->copy(floor);
//			  if (floor != NULL) {
//				  floor->add_item(copy,FALSE);
//				  copy->set_priority(current_priority-1);
//				  item->set_priority(current_priority-2);
////				  copy->set_move_after_sucking_up(TRUE);
////            floor->new_item(copy); // put back when meta-programming
//			  };
////         copy->move_by(4*tile_width,0);
////         copy->changed_so_update(); // to update collision region
//			  return(copy->item_released_on_top(item,by,screen));
//	  };
//	  return(RELEASE_IGNORED);
//  };
//  if (cubby != NULL) { // already has something in it
	  // think about generalizing this for defining robots unconditionally
	  // by dropping them on empty thought bubbles
//	  if (item->kind_of() == ROBOT) {
//		 if (robot == NULL) {
//			 robot = ((Robot *) item);
//			 if (robot->pointer_to_thought_bubble() != NULL) {
//				 // already defined
//				 robot = NULL;
//				 return(RELEASE_IGNORED);
//			 };
//			 return(RELEASED_ROBOT_ON_THOUGHT_BUBBLE);
//		 } else { // probably trying to make a line of robots
//			 return(robot->item_released_on_top(item,by,screen));
//		 };
//	  };
// commented this out because it happened too much by accident
// to edit thought bubble need to remove cubby, edit and then return
//  if (cubby != NULL) { // already has something in it
//	  TTRegion cubby_collision_region, item_region;
//	  cubby->collision_region(cubby_collision_region);
//	  item->collision_region(item_region);
//	  if (cubby_collision_region.overlaps(&item_region)) {
//		  item->inside_thought_bubble();	
//		  int hole_count = cubby->current_number_of_holes();
//		  recipient = cubby;
//		  ReleaseStatus status = cubby->item_released_on_top(item,by,recipient);
//		  if (hole_count != cubby->current_number_of_holes()) { // changed
//			  city_coordinate new_width, new_height, new_llx, new_lly;
//			  cubby_region(cubby,new_width,new_height,new_llx,new_lly);
//			  cubby->set_size_and_location(new_width,new_height,new_llx,new_lly);
//			  cubby->record_offsets();
//		  };
//		  if (status == RELEASE_IGNORED) {
//			  recipient = robot;
//			  return(robot->item_released_on_top(item,by,recipient));
//		  };
//		  return(status);
//	  };
//	} else 
	if (cubby == NULL && item->kind_of() == CUBBY && robot->pointer_to_body() != NULL) { 
		// was (cubby == NULL || cubby->pointer_to_leader() == NULL)
		// empty thought bubble but robot has been trained so accept cubby
		if (receive_item(item,by,default_duration(),this,item)) {
//			record_event(RELEASE_ITEM_ON,by,floor,this);
			return(RELEASE_HANDLED);
		} else {
			return(RELEASE_IGNORED);
		};
	};
	recipient = robot;
	return(robot->item_released_on_top(item,by,recipient,level-1));
};
	//	  return(RELEASE_IGNORED);
//	};
//	if (item_being_sucked_up != NULL ||
//		 item->kind_of() != CUBBY) {
//		return(RELEASE_IGNORED); // already has a cubby or about to or non-cubby on top
//	};
//	receive_cubby((Cubby *) item,accept_item_duration);
//	return(RELEASED_CUBBY_ON_THOUGHT_BUBBLE);

//void Thought_Bubble::receive_cubby_again() {
//	receive_cubby(cubby);
//};

boolean Thought_Bubble::receive_item(Sprite *item, Sprite *by, millisecond , Sprite *original_recipient, Sprite *original_item) {
   if (inside_vacuum()) {
      by->abort_action(IDS_ROBOT_MISSING_RECIPIENT_ABORT);
      return(FALSE);
   };
//	cubby_vacuumed = FALSE;
	by->completed();
	if (item->kind_of() != CUBBY) { // error check for robustness new on 111104
		by->abort_action(IDS_THOUGHT_BUBBLE_RECEIVED_SOMETHING_OTHER_THAN_A_BOX,NONE_GIVEN,2,"a thought bubble received something other than a box"); // updated on 130205
//		log("Thought bubble received something other than a box.",FALSE,TRUE);
		return(FALSE);
	};
	Cubby *new_cubby = (Cubby *) item;
	new_cubby->now_inside_thought_bubble(FALSE); // was (by != NULL && by->kind_of() == ROBOT_IN_TRAINING));
   // the following calls inside_thought_bubble -- combine the 2 protocols??
	receive_cubby(new_cubby,(boolean) accept_item_duration);
	record_event(RELEASE_ITEM_ON,by,floor,item);
	return(TRUE); //RELEASED_CUBBY_ON_THOUGHT_BUBBLE);
};

void Thought_Bubble::receive_cubby(Cubby *a_cubby, boolean center_cubby) {
//	item_being_sucked_up = a_cubby;
	cubby = a_cubby;
//	if (cubby->is_busy()) {
//		cubby->run_when_non_busy(this,receive_cubby_again_callback);
//		return;
//	};
//	item_being_sucked_up
	cubby->inside_thought_bubble();	
//	item_being_sucked_up
	scale_to_fit(); // see if this fixes geometry sloppiness
	if (center_cubby) center_and_resize_cubby();
	cubby->remove_from_floor(FALSE);
	add_follower(cubby);
//	cubby // item_being_sucked_up
//		->animate_size_and_location(new_width,new_height,new_llx,new_lly,
//											 duration,
//											 this,sucking_up_callback);
};

void Thought_Bubble::center_and_resize_cubby() {
//	cubby->save_size();
	TTRegion region;
	collision_region(region);
	cubby->set_size_and_location_to_region(region);
   cubby->update_piece_widths();
	cubby->refit_contents();
};

void Thought_Bubble::signal_size_and_location() {
	Sprite::signal_size_and_location();
	if (cubby != NULL) { // && cubby->pointer_to_leader() == this) {
		// give it chance to improve its geometry
		if (width > tile_width && width < ideal_screen_width) { 
		   // to avoid approximation errors recompute now
			center_and_resize_cubby();
		};
	};
};
	
//void Thought_Bubble::sucking_up() {
//   sucking_up_done = FALSE;
//	cubby->remove_from_floor(FALSE);
//	add_follower(cubby);
	// cubby might be brand new if called by copying a thought bubble
//	if (floor != NULL) floor->remove_item(cubby,FALSE);
//	set_clean_status(DIRTY);
//	item_being_sucked_up = NULL;
//	if (move_after_sucking_up) {
//		animate_location(llx+tile_width,lly+tile_height,
//							  showing_on_a_screen()?500:0);
//		move_after_sucking_up = FALSE;
//	};
//};

//void sucking_up_callback(void *sprite) {
//	((Thought_Bubble *) sprite)->sucking_up();
//};

Sprite *Thought_Bubble::select(TTRegion *region, SelectionReason reason, Sprite *in_hand) {
//	if (reason == MIGHT_USE_EXPANDER) {
		// so thought bubble can be expanded without robot
//		return(this);
//	};
	if (robot != NULL && (reason == MIGHT_GRASP || reason == MIGHT_USE_EXPANDER)) {
		// was commented out -- why??
		return(robot->select(region,reason,in_hand));
	};
	// if vacuum is used and robot is non null should only be able to
	// blank numbers
	// if there is something in the thought bubble see if region is inside
	if (cubby != NULL) { // && cubby->pointer_to_leader() == this) {
		TTRegion cubbys_region;
		cubby->collision_region(cubbys_region);
 #if TT_DEBUG_ON
		if (tt_debug_mode == 220803) {
			tt_error_file() << "Thought bubble's cubby region is ";
			cubbys_region.print(tt_error_file());
			tt_error_file() << endl;
		};
#endif
		if (!blank && // added 121199 since if erased shouldn't be able to select parts
			 ((reason == MIGHT_USE_VACUUM && cubbys_region.contains(region)) ||
			  (reason == MIGHT_USE_COPIER && cubbys_region.overlaps(region)))) {
			Sprite *selection = cubby->select(region,reason,in_hand);
//		if (robot != NULL && reason == MIGHT_USE_VACUUM) {
//			return(NULL);
//		};
			return(selection);
		};
	} else if (in_hand != NULL && in_hand->kind_of() == CUBBY && cubby == NULL && robot->pointer_to_body() != NULL && tt_log_version_number >= 38) { 
		// added cubby == NULL on 010304 (after 3.53x)
		return(this); // on 290204 to show bubble wiggling if about to replace thought bubble cubby
	};
//	if (robot != NULL && reason == VACUUM_USED) {
//		return(NULL);
//	};
   if (robot != NULL) return(robot); // why was this missing??
	return(this);
};

Sprite *Thought_Bubble::selected(SelectionReason reason, Sprite *by) {
   // new policy doesn't allow selection of thought bubble without attached robot
//	if (reason == VACUUM_USED) cubby_vacuumed = TRUE;
	return(robot->selected(reason,by));
//	if (infinite_stack_flag) {
//		return(Tool::selected(reason));
//	};
/*
	if (robot != NULL) {// && reason == VACUUM_USED) {
		if (reason == GRASPED) {
			play_sound(WHIT_SOUND);
		};
		robot->set_thought_bubble(NULL);
		robot = NULL;
	};
	return(Sprite::selected(reason,by));
*/
};

Sprite *Thought_Bubble::now_inside_thought_bubble(boolean in_training) {
	if (cubby != NULL) { // && cubby->pointer_to_leader() == this) {
		cubby->now_inside_thought_bubble(in_training);
	};
	return(this);
};

void Thought_Bubble::set_cubby(Cubby *new_cubby, boolean also_add_follower) { // rewritten on 080205 to maintain follower links
	if (cubby != NULL && cubby->pointer_to_leader() == this) {
		remove_follower(cubby);
	};
	cubby = new_cubby;
	if (new_cubby != NULL && also_add_follower && new_cubby->pointer_to_leader() != this) {
		add_follower(new_cubby);
	};
};

void Thought_Bubble::remove_selected_element(Sprite *element,
															SelectionReason reason, Sprite *, boolean add_to_floor) {
	if (element != cubby) {
#if TT_DEBUG
		say_error("Somehow a non-box is being removed from a thought bubble.");
#endif
//		tt_error_file() << "Removing non-cubby from thought bubble!" << endl;
		return;
	};
	if (reason == GRASPED || reason == VACUUM_USED) { // how can it be grasped???
		remove_follower(cubby);
		if (add_to_floor) {
			floor->add_item(cubby);
			if (reason == GRASPED) {
	//			city_coordinate saved_width, saved_height;
	//			cubby->saved_size(saved_width,saved_height);
	//			cubby->animate_size(saved_width,saved_height,1000);
				cubby->animate_to_good_size(1000,tt_log_version_number >= 64?tt_toolbox:NULL);
			};
		};
      cubby->set_cause_of_match_failure(FALSE,TRUE); // new on 260302 so that reset indication of what last failed to match
 		tt_just_vacuumed = cubby;
		cubby = NULL; 
		set_clean_status(DIRTY);
	};
};

//Sprite *Thought_Bubble::appearance_before_vacuuming() {
//	return(cubby);
//};

//void Thought_Bubble::reset_appearance_before_vacuuming() {
//	if (cubby_vacuumed) cubby = NULL;
//	cubby_vacuumed = FALSE;
//};

// this was cute animation but very very resource consuming for little payoff
//void Thought_Bubble::set_alive(boolean flag) {
//  if (flag) {
//	  set_parameter(BUBBLE_ALIVE);
//  } else {
//	  set_parameter(BUBBLE_INERT);
//  };
//};

/*
void Thought_Bubble::update_display() {
	Tool::update_display();
	if (sucking_up != NULL) {
		if (sucking_up_done) {
			sucking_up_done = FALSE;
			cubby = (Cubby *) sucking_up;
			add_follower(sucking_up,floor);
			floor->remove_item(sucking_up);
			set_clean_status(DIRTY);
			sucking_up = NULL;
		};
	};
};
*/

boolean Thought_Bubble::dump(output_stream &stream, boolean just_prepare, boolean , boolean ) {
   if (just_prepare) return(FALSE);
   int dump_index = about_to_dump(this,stream); // rewritten on 020802
	if (dump_index < 0) return(FALSE); // new on 051099
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
		print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
		tt_error_file() << "About to dump ";
		print(tt_error_file());
		tt_error_file() << endl;
		tt_debug_dump_depth++;
	};
#endif
   if (cubby != NULL) { // && cubby->pointer_to_leader() == this) {
		// changed to top_level_dump on 060201 since there should be no connections between stuff in the thought bubble and outside 
      cubby->top_level_dump(stream);
	} else if (robot->pointer_to_body() != NULL) { // a bit of a hack to distinguish trained robots with empty thought bubbles from untrained ones
		stream.put(NOTHING_MARKER);
	} else {
		stream.put((char) NONE_GIVEN);
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
		print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
		tt_error_file() << "Finished dumping ";
		print(tt_error_file());
		tt_error_file() << endl;
		tt_debug_dump_depth--;
	};
#endif
	return(TRUE);
};

#if TT_XML

void Thought_Bubble::xml(xml_element *element, xml_document *document) {
//#if TT_DEBUG_ON
//	if (!geometry_worth_saving()) {
//		log("debug this");
//	};
//#endif
	// taken care of below now -- uncommented out on 280803 since if saving a city just after giving an untrained robot a box then cubby isn't yet a follower
	if (cubby != NULL && followers == NULL) {
		cubby->xml_create_and_append_element(element,document)->Release();
	};
	// what about robot?? -- apparently not needed since is only a back pointer and robot will do the work
	Sprites *remaining = followers; // new on 130703 -- needed if saving while expanding thought bubble to enter training
	while (remaining != NULL) {
		Sprite *sprite = remaining->first();
//		if (sprite != cubby) { // if not already dealt with above
			sprite->xml_create_and_append_element(element,document)->Release(); // shouldn't be any more cubbies - right? otherwise confusion
//		};
		remaining = remaining->rest();
	};
	Sprite::xml(element,document);
//   return(create_xml_element(L"ThoughtBubble",document,cubby,TRUE)); // last arg was FALSE prior to 120603 but can be shared if is current selection when city is saved - everything is like this
};

boolean Thought_Bubble::handle_xml_tag(Tag tag, xml_node *node) { // new on 151002 
   switch (tag) {
		case BOX_TAG: // new on 021202
			cubby = (Cubby *) xml_load_sprite(node,tag,cubby,CUBBY);
			if (cubby->pointer_to_leader_offsets() == NULL) {
				// restored on 050903
				receive_cubby(cubby,TRUE); // experiment 280803 for dealing with thought bubbles that are about to be entered to start training
			} else {
				add_follower(cubby,TRUE,INSERT_AT_END,TRUE);
			};
//			receive_cubby((Cubby *) xml_load_sprite(node,tag,cubby,CUBBY));
			return(TRUE);
      case NO_MORE_TAGS: // new on 151002
         set_active(TRUE); // stop pretending since can never be turned off -
         // and do the following (new on 121102)
      default:
			if (sprite_tag(tag)) { // new on 130703
				add_follower(xml_load_sprite(node,tag),TRUE,INSERT_AT_END,TRUE);
				return(TRUE);
			} else {
				return(Sprite::handle_xml_tag(tag,node));
			};
   };
};

#endif

boolean Thought_Bubble::equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, SpritesPointer &expected_corresponding_nests, EqualReason reason) { // new on 060601
	if (other->kind_of() != kind_of()) return(FALSE);  
	if (other == this) return(TRUE);
	Thought_Bubble *other_bubble = (Thought_Bubble *) other;
	if (pointer_to_cubby() != NULL) {
		if (other_bubble->pointer_to_cubby() != NULL) {
			// started using top_level_equal_to on 08061 since no sharing should exist between thought bubble cubby and the rest
			return(pointer_to_cubby()->top_level_equal_to(other_bubble->pointer_to_cubby(),corresponding_nests,expected_corresponding_nests,reason));
		} else {
			return(FALSE);
		};
	} else {
		return(other_bubble->pointer_to_cubby() == NULL);
	};
};

//void Thought_Bubble::adjust_grasp(city_coordinate &adjustment_x,
//                                  city_coordinate &adjustment_y) {
//   adjustment_x = 0;
//   adjustment_y = 0;
//};
