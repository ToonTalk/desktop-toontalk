// Copyright (c) 1992-2007.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif   
#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif   
#if !defined(__TT_NUMBER_H)   
#include "number.h"
#endif   
#if !defined(__TT_TOOLS_H)   
#include "tools.h"
#endif   
#if !defined(__TT_CUBBY_H)   
#include "cubby.h"
#endif   
#if !defined(__TT_TOOLS_H)   
#include "tools.h"
#endif   
#if !defined(__TT_THOUGHT_H)   
#include "thought.h"
#endif   
#if !defined(__TT_BIRD_H)   
#include "bird.h"
#endif   
#if !defined(__TT_BOMB_H)   
#include "bomb.h"
#endif   
#if !defined(__TT_TRUCK_H)   
#include "truck.h"
#endif   
#if !defined(__TT_TEXT_H)   
#include "text.h"
#endif   
#if !defined(__TT_ROOM_H)   
#include "room.h"
#endif   
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif   
#if !defined(__TT_HOUSE_H)   
#include "house.h"
#endif   
#if !defined(__TT_PAD_H)   
#include "pad.h"
#endif   
#if !defined(__TT_PRGRMMR_H)   
#include "prgrmmr.h"
#endif   
//#include "primitiv.h"
#if !defined(__TT_CLEANUP_H)   
#include "cleanup.h"
#endif   
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif   
#if TT_WINDOWS
#if !defined(__TT_WINMAIN_H)   
#include "winmain.h"
#endif   
#endif
#if !defined(__TT_ROBOT_H)   
#include "robot.h"
#endif   
#if !defined(__TT_EVENT_H)   
#include "event.h"
#endif   
#if !defined(__TT_HELP_H)
#include "help.h"
#endif
#if !defined(__TT_MARTIAN_H)
#include "martian.h"
#endif
#if !defined(__TT_PUZZLE_H)
#include "puzzle.h"
#endif
#if !defined(__TT_FLOOR_H)   
#include "floor.h"
#endif 
   
//#include <strstrea.h> commented out 210703

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Floor::Floor(short int floor_id, Room *room, Cubby *thought_bubble_cubby, Robot *initial_robot) :
  Visible_Background(),
  room(room),
//  current_tool_priority(max_long/2),
//  saved_items(NULL),
  body_cubby(NULL),
  alternative_body_cubbies(NULL),
  floor_id(floor_id),
  at_left_wall(FALSE),
  at_right_wall(FALSE),
  at_front_wall(FALSE),
  initial_robot(initial_robot),
  toolbox_status(TOOLBOX_CACHE_STATUS_UNINITIALIZED), // wasn't initialized prior to 070103
  previous_mouse(NULL) {
//  previous_undoable_event(NULL) {
//  ok_for_toolbox_entry_flag(FALSE)  // FALSE becuase only in PUZZLE mode is this used
//  vacuum(NULL),
//  copier(NULL),
//  expander(NULL),
//  notebook(NULL) 
//  deleted_sprites(NULL),
//  working_set(NULL),
//  working_set_length(0),
//  max_working_set_size(8) { // is extended if needed
	if (thought_bubble_cubby == NULL) {
		body_cubby = NULL;
	} else {
		body_cubby = thought_bubble_cubby;
		body_cubby->increment_ref_count(); // new on 080203
		body_cubby->set_main_cubby_in_body(TRUE);
      add_item(body_cubby);
//		body_cubby->set_priority(current_priority--);
//		body_cubby->recursively_set_background(this);
//		items = new Sprites(body_cubby,items);
	};
	if (initial_robot != NULL) {
		initial_robot->increment_ref_count(); // new on 090203
	};
};

void Floor::prepare_for_deletion() { // new on 080701
	if (previous_mouse != NULL && tt_reclaiming_memory) { // added shutting_down condition on 080202 - updtaed on 180103
		previous_mouse->destroy();
		previous_mouse = NULL;
	};
	if (initial_robot != NULL && tt_reclaiming_memory) { // new on 090203
		initial_robot->destroy();
		initial_robot = NULL;
	};
	if (body_cubby != NULL) {
		body_cubby->set_main_cubby_in_body(FALSE); // new on 151204
		body_cubby->destroy();
		body_cubby = NULL;
	};
	destroy_sprites(alternative_body_cubbies); // is ref counted -- new on 151204
	Visible_Background::prepare_for_deletion();
};


//Floor::~Floor() {
//	remove_toolbox();
//	if (items != NULL) {
//		items = items->remove(vacuum,FALSE); 
//		if (items != NULL) {
//			items = items->remove(expander,FALSE); 
//			if (items != NULL) {
//				items = items->remove(copier,FALSE); 
//				if (items != NULL) {
//					items = items->remove(notebook,FALSE);  
// covered in bckgrnd.cpp
//					if (items != NULL) {
//						delete items;
//					};
//				};
//			};
//		};
//	};
//	if (saved_items != NULL) {
//		delete saved_items;
//	};
//	if (vacuum != NULL) remove_item(vacuum,FALSE);
//	if (expander != NULL) remove_item(expander,FALSE);
//	if (copier != NULL) remove_item(copier,FALSE);
//	if (notebook != NULL) remove_item(notebook,FALSE);
//	if (working_set != NULL) {
		// items themselves handled elsewhere -- e.g. background::~background
//		working_set->remove_all();
//		delete working_set;
//		delete [] working_set;
//	};
//	if (deleted_sprites != NULL) {
//		delete deleted_sprites;
//	};
//};

//void Floor::destroy(Sprite *sprite) {
//	if (inside_rule_body()) {
//		// might still be in the working_set
//		sprite->set_slated_for_deletion(TRUE); // so it'll be ignored
//		deleted_sprites = new Sprites(sprite,deleted_sprites);
//	} else { // delete during next frame
//		Visible_Background::destroy(sprite);
//	};
//};
	
/*
void Floor::in_thought_bubble(Cubby *thought_bubble_cubby) {
	saved_items = items;
	original_cubby = thought_bubble_cubby;
	body_cubby = (Cubby *) thought_bubble_cubby->copy(this);
	body_cubby->set_priority(current_priority--);
	body_cubby->set_main_cubby_in_body(TRUE);
	items = new Sprites(body_cubby,NULL);
};

void Floor::leaving_thought_bubble() {
	delete items;
	items = saved_items;
	saved_items = NULL;
};
*/

void Floor::restore_toolbox(boolean move_toolbox_offscreen_first) {
/*
	Sprites *remaining = tt_notebooks;
	while (remaining != NULL) {
		Notebook *pad = (Notebook *) remaining->first();
		add_item(pad);
		pad->set_background(this);
		recursively_restore_leaders(pad,this);
		remaining = remaining->rest();
	};
*/
   if (tt_system_mode == PUZZLE &&
       (!is_toolbox_part_of_puzzle() || inside_rocket())) return;
   if (tt_toolbox == NULL || tt_toolbox->gone()) return; // simplified this on 030300
	if (tt_shutting_down) return;
	if (tt_toolbox->pointer_to_background() == NULL) {
		add_item(tt_toolbox);
	};
	tt_screen->add(tt_toolbox);
//	tt_toolbox->set_background(this);
	tt_toolbox->enter(move_toolbox_offscreen_first);
};

#if TT_XML

//boolean first_to_be_dumped(Sprite *sprite) {
//	return(sprite->appearance_code() == MOUSE_SPRITE || (sprite->kind_of() == ROBOT && ((Robot *) sprite)->body_started()));
//};
//
//boolean not_first_to_be_dumped(Sprite *sprite) {
//	return(!(sprite->appearance_code() == MOUSE_SPRITE || (sprite->kind_of() == ROBOT && ((Robot *) sprite)->body_started())));
//};

xml_element *Floor::xml(xml_document *document) { // new on 011102
   xml_element *floor_element = create_xml_element(xml_tag(),document);
	if (previous_mouse != NULL && tt_dumping_background != NULL) { // new on 131202
		previous_mouse->add_xml(floor_element,L"PreviousMouse",document);
	};
	// folllowing new on 070103 - checked through all state variables
	if (body_cubby != NULL) {
		body_cubby->add_xml(floor_element,L"MainBox",document);
		if (room != NULL) { // only needed if in thought bubble
			xml_append_child(room->xml_path(document),floor_element);
		};
	};
	if (alternative_body_cubbies != NULL) { // new on 151204
		xml_element *alternative_body_cubbies_element = create_xml_element(L"AlternativesForMyBox",document);	
		alternative_body_cubbies->add_xml(alternative_body_cubbies_element,document);
		xml_append_child(alternative_body_cubbies_element,floor_element); // new on 161204
	};		
	if (initial_robot != NULL) {
		initial_robot->add_xml(floor_element,L"InitialRobot",document);
	};
	if (toolbox_status != TOOLBOX_CACHE_STATUS_UNINITIALIZED) {
		xml_set_attribute(floor_element,L"ToolBoxStatusCode",(int) toolbox_status);
	};
	if (items != NULL) { // moved here so these don't generate include files if the above refers to them (or their parts)
      xml_element *on_floor_element = create_xml_element(L"OnFloor",document);
      xml_set_attribute(on_floor_element,L"Z",current_priority); // only need this if there are some items on the floor
#if TT_DEBUG_ON
		if (tt_debug_mode == 160703) {
			tt_error_file() << "About to dump floor items." << endl;
		};
#endif
		items->add_xml(on_floor_element,document,NULL,TRUE);
		xml_append_child(on_floor_element,floor_element); // moved here on 190804
#if TT_DEBUG_ON
		if (tt_debug_mode == 160703) {
			tt_error_file() << "Done dumping floor items." << endl;
		};
#endif
		// following not needed as of 160703 so restored above
		// new on 120703 so that mice who might cause others to not dump as an include file are processed first
		// what about running robots?? -- ok since tt_running_robots is processed first - right?
  //    items->add_xml(on_floor_element,document,NULL,TRUE,FALSE,first_to_be_dumped); 
		//items->add_xml(on_floor_element,document,NULL,TRUE,FALSE,not_first_to_be_dumped);
   };
	xml_set_attribute(floor_element,L"FloorID",floor_id);
	if (at_left_wall) {
		xml_set_attribute(floor_element,L"AtLeftWall",1);
	};
	if (at_right_wall) {
		xml_set_attribute(floor_element,L"AtRightWall",1);
	};
	if (at_front_wall) {
		xml_set_attribute(floor_element,L"AtFrontWall",1);
	};
//	add_xml(floor_element,document); // new on 090404 -- this wasn't needed since all this containment stuff is now computed from sit_llx etc on load
   return(floor_element);
};

boolean Floor::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
      case ON_FLOOR_TAG: 
		case INSIDE_TAG: // new on 040803
			// following called reset_items prior to 240203
			just_add_items(xml_load_sprites(node),xml_get_attribute_int(node,L"TopLevel",0)); // new on 110803 to add to screen if pasting in the floor by itself		
		   // rewritten on 171202 to just do above rather than below
         // Sprites *floor_items = xml_load_sprites(node);
         // if (floor_items != NULL) {
		      //Sprites *remaining = floor_items;
		      //while (remaining != NULL) {
			     // // new on 101099 if priority of item is fixed then don't reset it
			     // // so Marty, for example, loads in with correct priority
			     // add_item(remaining->first(),FALSE); 
			     // // FALSE replaced !remaining->first()->priority_fixed() above on 211199 since priority is now saved correctly
			     // remaining = remaining->rest();
		      //};
		      //floor_items->recursively_propagate_changes();
		      //floor_items->activate();
         //    floor_items->remove_all();
         //    delete floor_items;
         // };
			current_priority = xml_get_attribute_int(node,L"Z",max_long);
         return(TRUE);
		case PREVIOUS_MOUSE_TAG:
			set_previous_mouse((Mouse *) xml_get_sprite(node,previous_mouse,MOUSE_WITH_HAMMER)); // args added on 070103
			return(TRUE);
		case MAIN_BOX_TAG: 
			body_cubby = (Cubby *) xml_get_sprite(node,body_cubby,CUBBY);
			if (body_cubby != NULL) { // new on 090203 - not clear if better to use set_body_cubby instead but it does much more
				body_cubby->increment_ref_count(); 
			};
			return(body_cubby != NULL);
		case ALTERNATIVES_FOR_MY_BOX_TAG:
			alternative_body_cubbies = xml_load_sprites(node);
			return(alternative_body_cubbies != NULL);
		case INITIAL_ROBOT_TAG: 
			set_initial_robot((Robot *) xml_get_sprite(node,initial_robot,ROBOT));
			return(initial_robot != NULL);
		case HOUSE_LOCATION_TAG: {
			House *house = house_at_location(node);
			if (house == NULL) return(FALSE);
			room = house->pointer_to_room();
			return(room != NULL);
									  };
      case NO_MORE_TAGS:
			// commented out on 041103 since this causes problem when jumping between segments in time travel and is only an optimization anyway
//			toolbox_status = (ToolboxCacheStatus) xml_get_attribute_int(node,L"ToolBoxStatusCode",TOOLBOX_CACHE_STATUS_UNINITIALIZED);
			floor_id = xml_get_attribute_int(node,L"FloorID",floor_id);
			at_left_wall = xml_get_attribute_int(node,L"AtLeftWall",0); // prior to 210404 at_left_wall
			at_right_wall = xml_get_attribute_int(node,L"AtRightWall",0); // prior to 210404 at_right_wall
			at_front_wall = xml_get_attribute_int(node,L"AtFrontWall",0); // prior to 210404 at_front_wall
         return(TRUE); // had been commented out on 090404 to fall through to call to super -- trying to fix time travel bug with containment region
			// restored 090404 since Floor is "self contained"
      default:
 //        return(FALSE); // extra_xml?? 051102
			return(Visible_Background::handle_xml_tag(tag,node)); // new on 090404 - though super doesn't do much better...
   };
};

xml_element *Floor::xml_path(xml_document *document) { // new on 111202
	if (block_im_on() == NULL) {
		return(NULL); // was create_xml_element(L"ReturnGround",document)); -- // birds can't start from the ground currently
	};
	return(pointer_to_house()->xml_path(document));
};

#endif

void Floor::dump(output_stream &stream, boolean just_prepare) {
   if (just_prepare) {
      if (items != NULL) items->dump(stream,FALSE,just_prepare);
      return;
   };
   stream.put(FLOOR_MARKER);
   if (items != NULL) {
		items->dump(stream,TRUE);
//      Sprites *sorted_items = sort_items(items); // commented out on 210100 since robots now save links to working cubby
		// commented out the following on 050100 since now if tt_toolbox is responsible it is dumped specially
//      if (tt_toolbox != NULL) {
//         Sprites *non_toolbox_items = NULL;
//         Sprites *remaining = items;
//         while (remaining != NULL) {
//            Sprite *sprite = remaining->first();
//            if (!toolbox_tool(sprite)) {
//              insert_at_end(sprite,non_toolbox_items);
//            };
//            remaining = remaining->rest();
//        };
//         sorted_items = sort_items(non_toolbox_items);
//         non_toolbox_items->remove_all();
//         delete non_toolbox_items;
//      } else {
//         sorted_items = sort_items(items);
//      };
//      if (sorted_items != NULL) {
//         sorted_items->dump(stream,TRUE);
//         sorted_items->remove_all();
//         delete sorted_items;
//      };
   };
   stream.put(END_OF_LIST_MARKER);
	stream.write((char *) &current_priority, sizeof(current_priority)); // new on 150100
};

void Floor::load(InputStream *stream, int notebook_version, NaturalLanguage language) {
   if (stream->get() != FLOOR_MARKER) {
		say_error(IDS_BAD_FLOOR_IN_CITY_FILE,TRUE);
      return;
   };
   Sprites *floor_items = load_items(stream,notebook_version,language);
   if (floor_items != NULL) {
//      add_items(floor_items);
		Sprites *remaining = floor_items;
		while (remaining != NULL) {
			// new on 101099 if priority of item is fixed then don't reset it
			// so Marty, for example, loads in with correct priority
			add_item(remaining->first(),FALSE); 
			// FALSE replaced !remaining->first()->priority_fixed() above on 211199 since priority is now saved correctly
			remaining = remaining->rest();
		};
		floor_items->recursively_propagate_changes();
		floor_items->activate();
      floor_items->remove_all();
      delete floor_items;
   };
	if (notebook_version >= first_version_to_save_number_colors) {
		stream->read((string) &current_priority, sizeof(current_priority)); // new on 150100
	};
};

void Floor::set_initial_robot(Robot *robot) { // new on 090203 for ref counting initial_robot
	if (initial_robot != NULL) {
		initial_robot->destroy();
	};
	initial_robot = robot;
	if (initial_robot != NULL) {
		initial_robot->increment_ref_count();
	};
};

void Floor::initial_contents(Robot *robot, Cubby *cubby) {
//	initial_robot = robot;
	set_initial_robot(robot); // rewritten on 090203
//	initial_cubby = cubby;
//	robot->recursively_set_background(this);
//	cubby->recursively_set_background(this);
//	robot->recursively_restore_leaders(this);
//	cubby->recursively_restore_leaders(this);
	add_item(robot);
   add_item(cubby);
	robot->recursively_propagate_changes(); // seems to help geometry...
	if (robot->pointer_to_body() != NULL) { // conditional new on 070800 so when training an untrained robot that has ridden in the truck it behaves normally
		robot->set_working_cubby(cubby); // or it will be once this runs
	};
	//   robot->suspend();
//   robot->try_clause(cubby);
};

void Floor::house_built() {
	cubby_is_not_busy();
	if (initial_robot != NULL && initial_robot->fully_defined()) {
		// all this delay stuff not so important now -- is it?
		// problem is to approximate fairness between on screen
		// and off screen computations
//		if (!initial_robot->showing_on_a_screen()) {
			// so this can be delayed it is added to list
//			tt_running_robots = new Sprites(initial_robot,tt_running_robots);
//		};
//		initial_robot->try_clause(initial_cubby,1); // delay 1 millisecond
//		initial_robot->increment_ref_count(); // commented out on 080203
      if (insert_at_end_if_not_member(initial_robot,tt_running_robots)) {
			// commented out tt_running_robot ref count stuff on 090400 since causes robots to not be deleted and to run (as orphans)
		   // and when robot is prepared for deletion it removes itself from tt_running_robot
//			initial_robot->increment_ref_count(); // new on 310100
		};
	};
//	initial_robot = NULL; // new on 090203 since done with it -- ref counted now so this interferes (also 090203)
};

/*
void Floor::remove_toolbox() {
	Sprites *remaining = tt_notebooks;
	while (remaining != NULL) {
		Notebook *pad = (Notebook *) remaining->first();
		remove_item(pad,FALSE); // don't complain if pad not there
		pad->set_background(NULL);
		recursively_remove_leaders(pad,this);
		remaining = remaining->rest();
	};
//	tt_screen->remove(tt_toolbox);
	tt_toolbox->set_background(NULL);
};
*/

/*
boolean Floor::add_item(Sprite *sprite, boolean give_priority,
                        boolean change_priority, boolean warn) {
	boolean result = Visible_Background::add_item(sprite,give_priority,change_priority,warn);
	if (give_priority) { // give priority even if already added
		sprite->set_priority(current_priority--);
	} else if (change_priority && current_priority >= sprite->priority()) {
      current_priority = sprite->priority()-1;
   };
   return(result);
};
*/

void Floor::up_against_the_wall(boolean now_at_left_wall, boolean now_at_right_wall, boolean now_at_front_wall) {
   at_left_wall = now_at_left_wall;
   at_right_wall = now_at_right_wall;
   at_front_wall = now_at_front_wall;
};

void Floor::display_walls() {
   if (tt_programmer->floor_not_scrollable()) return;
   if (at_front_wall) {
      if (at_left_wall) {
         display_wall(LEFT_CORNER_SPRITE);
      } else if (at_right_wall) {
         display_wall(RIGHT_CORNER_SPRITE);
      } else {
         display_wall(FRONT_WALL_SPRITE);
      };
   };
};

city_coordinate Floor::wall_lly() {
   if (tt_programmer->floor_not_scrollable()) return(0);
   if (at_front_wall) {
		return(::wall_lly());
	} else {
		return(max_long);
   };
};

boolean Floor::display_region() {
#if TT_DEBUG_ON
	if (tt_debug_mode == 250404) {
		tt_error_file() << "Displaying floor on frame " << tt_frame_number << endl;
	};
#endif
   boolean result = FALSE;
	int background_index;
	if (body_cubby == NULL) {
		background_index = floor_id;
	} else {
		background_index = THOUGHT_BUBBLE_BACKGROUND;
	};
   if (tt_toolbox == NULL || tt_toolbox->gone()) { // gone test new on 120704
      background(background_index)->display();
   } else {
	   switch(toolbox_status) {
		   case TOOLBOX_CACHE_INVALID:
//			   play_sound(POP_SOUND,8);
//			   load_background(background_index);
			   if (tt_toolbox != NULL) tt_toolbox->set_background_cached(FALSE);
			   // and do the following
		   case TOOLBOX_NOT_READY_TO_CACHE:
         case TOOLBOX_CACHE_STATUS_UNINITIALIZED:
			   background(background_index)->display();
//			   screen->display_background(background_index);
			   result = TRUE; // background will only last one frame
            break;
		   case TOOLBOX_CACHE_VALID:
			   display_background_cache();
            return(result);
		   case TOOLBOX_READY_TO_CACHE:
//			   play_sound(PLOP_SOUND,8);
			   background(background_index)->display();
            display_walls(); // moved here on 250402 since otherwise this can obscure part of Tooly
//			   screen->display_background(background_index);
            tt_postpone_dirty_displays = FALSE;
			   if (!tt_toolbox->gone()) {
//#if TT_DEBUG_ON
//					if (tt_toolbox->is_background_cached()) {
//						log("debug this");
//					};
//#endif
					tt_toolbox->just_set_background_cached(FALSE); // new on 061003 since time travel can confuse this caching
//					tt_screen->display_layers_below(tt_toolbox); // new on 111204
				   tt_toolbox->display();
			   };
			   cache_background();
			   tt_toolbox->set_background_cached(TRUE);
            return(result);
      };
	};
   if (background_index == THOUGHT_BUBBLE_BACKGROUND) {
      string steps_left = initial_robot->steps_left_string();
      if (steps_left != NULL) {
BEGIN_GDI
	      tt_screen->text_out(steps_left,_tcslen(steps_left),
                             steps_left_counter_llx,steps_left_counter_lly,
							        steps_left_counter_font_width,steps_left_counter_font_height,TRUE,FALSE,TRUE);
END_GDI
         delete [] steps_left;
      };
   };
   display_walls();
   return(result);
};

boolean Floor::update_display(TTRegion *region) {
   boolean saved_clean = clean; // since following clobbers this
	Visible_Background::update_display(region);
   if (tt_toolbox == NULL) return(saved_clean);
	if (!tt_toolbox->gone() &&
       (tt_system_mode != PUZZLE  ||
        (is_toolbox_part_of_puzzle() && !inside_rocket()))) {
		tt_toolbox->update_display();
	} else {
		tt_toolbox->reset_clean_status();
	};
	// ok to cache this (if not changing by caching toolbox)
	toolbox_status = tt_toolbox->cache_status();
	if (toolbox_status == TOOLBOX_READY_TO_CACHE) return(FALSE);
   if (!saved_clean) return(FALSE);
	return(tt_toolbox->clean() && tt_toolbox->settled_down());
//	switch (tt_toolbox->cache_status()) {
//		case TOOLBOX_READY_TO_CACHE:
//		case TOOLBOX_CACHE_INVALID:
//			return(FALSE);
//		case TOOLBOX_CACHE_VALID:
//			return(TRUE);
//		default:
//			return(tt_toolbox->completely_clean());
//		default:
//			return(FALSE);
//	};
};

void record_event(EventCode code, Sprite *by, Background *floor,
						Sprite *subject, boolean type_of_subject_fixed,
						int key, AddToWorkingSet add_to_working_set,
						boolean notebook_page_really_selected,
						int label_index
#if TT_POST3187
						,Sprite *original_subject
#endif		
						) {
#if TT_DEBUG_ON
 	if (tt_debug_mode == 5050) {
		tt_error_file() << timeGetTime() << " ";
      Event *event = new Event(code,NULL);
		event->debug(NULL);
		delete event;
	};
#endif
	// until I generate instructions programmer actions aren't recorded
	if (tt_log_version_number >= 22) {
		if (by == NULL || tt_recording_off || (by->kind_of() != ROBOT && by->kind_of() != ROBOT_IN_TRAINING) || tt_shutting_down) return;
	} else {
		if (by == NULL || tt_recording_off || by->kind_of() == PROGRAMMER || tt_shutting_down) return;
	};
	// third condition prior to 050400 was by->kind_of() == PROGRAMMER but by might be a bird for example
	boolean body_cubby;
	if (by->kind_of() == ROBOT) {
		body_cubby = (subject == ((Robot *) by)->pointer_to_working_cubby());
	} else if (floor != NULL || tt_log_version_number < 22) { // condition added 050400
		body_cubby = (subject == floor->pointer_to_body_cubby());
	} else {
		body_cubby = FALSE;
	};
	// doesn't count that its main cubby if put somewhere
//	if (body_cubby) {
//		body_cubby = (subject->pointer_to_leader() == NULL ||
//						  subject->pointer_to_leader()->kind_of() == ROBOT_IN_TRAINING);
//	};
	const boolean notebook = (tt_toolbox != NULL && subject == tt_toolbox_notebook);
// following screws up if main cubby put into something and then taken out
// also bad to have just 1 optimization like this
// restored for notebook since it is shared and shouldn't be moved by robots
	if (code == RELEASE_ITEM && notebook) {
//		 ((body_cubby && subject->pointer_to_leader() == NULL) || notebook)) {
		// could be more clever and if anything is released right
		// after being picked up it is ignored
		// picked up and released main cubby or notebook
		// (or vacuumed up and restored)
		// robot shouldn't be doing this so ok to assume floor
		if (tt_events != NULL) {
			Event *last_event = tt_events->last()->first();
			if (last_event->event_code() == GRASP_ITEM || last_event->event_code() == VACUUM_APPLY) { 
				// test new on 120305 since can be keyboard event, for example
				floor->remove_last_n_events(1); // must have been a grasp_item (or apply_vacuum)
				return;
			};
		}; // else should be OK since there are no events to remove, right?
	};
	if (code == RELEASE_ITEM_ON && by->kind_of() == ROBOT_IN_TRAINING) {
      if (subject->kind_of() == NEST && subject->infinite_stack()) {
		   // returning a nest with egg back to stack but already
		   // generated a hatch_bird message
		   floor->remove_last_n_events(1);
		   ((Robot *) by)->remove_last_working_set_item();
// taken care of by :now_part_of_somthing()
//      } else { // Happens in event.cpp so needs to happen here as well
//         ((Robot *) by)->remove_tool_from_working_set(subject);
      };
	};
	Path *path = NULL;
	Event *new_item_event = NULL;
	if (body_cubby || notebook) {
		add_to_working_set = DONT_ADD_TO_WORKING_SET;
	};
	if (code == NEW_ITEM || code == HATCH_BIRD || 
		 code == RELEASE_ITEM || code == VACUUM_APPLY_RESTORE) {
		if (subject != NULL && !body_cubby &&
          !(tt_toolbox != NULL && toolbox_tool(subject))) {
			// either defining a body so add to this
			// or by is a robot so add to its
			// or by is programmer so ignore
			switch (by->kind_of()) {
				case ROBOT_IN_TRAINING:
//					if (!by->still_running()) {
						if (add_to_working_set != DONT_ADD_TO_WORKING_SET) {
							int index = ((Robot *) by)->add_to_working_set(subject,(add_to_working_set == ADD_TO_WORKING_SET_IF_NEW));
							if (type_of_subject_fixed) {
								path = new Path(index,new Path(subject->fine_kind_of()));
							} else if (code == VACUUM_APPLY_RESTORE) {
								path = new Path(index,new Path(NONE_GIVEN));
								subject = NULL;
							};
	//						} else if (code == HATCH_BIRD) {
	//							path = new Path(index,new Path(BIRD));
	//						} else {
	//							path = new Path(index,new Path(NONE_GIVEN));
						};
						break;
//					}; // otherwise fall thru to the following
				case ROBOT:
					if (add_to_working_set != DONT_ADD_TO_WORKING_SET) {
						((Robot *) by)->add_to_working_set(subject,(add_to_working_set == ADD_TO_WORKING_SET_IF_NEW));
					};
					break;
			};
//			floor->add_to_some_working_set(subject,by);
			if (code != GRASP_ITEM && code != GRASP_NEST && code != VACUUM_APPLY_RESTORE &&
				 notebook_page_really_selected) {
				subject->set_available_for_indexing(TRUE);
			};
		};
// following commented out so path is recorded for generating Java properly
//		if (code == VACUUM_APPLY_RESTORE) subject = NULL;
		if (path != NULL) {
			new_item_event = new Event(NEW_ITEM,path,0);
			if (code != VACUUM_APPLY_RESTORE) {
				if (tt_events == NULL) {
					tt_events = new Events(new_item_event,tt_events);
				} else {
					tt_events->insert_at_end(new_item_event);
				};
				new_item_event = NULL;
			}; // else add it after this event
		};
		// following just are used to update working set
		// can combine them to one?? except for descriptions?
		if (code == NEW_ITEM || code == HATCH_BIRD) return;
	};
	if (by->kind_of() != ROBOT_IN_TRAINING) {
		if (code == NEW_MAIN_CUBBY) {
			floor->set_body_cubby((Cubby *) subject, by);
		};
		return; // already knows the path
	};
	if (subject != NULL && code == GRASP_ITEM) {
		if (subject->kind_of() == NEST) {
			code = GRASP_NEST; // the whole thing not the top of its stack
		} else if (tt_toolbox != NULL) {
			// used to do switch (subject->kind_of()) but copies of tools break
			if (subject == tt_toolbox_copier) {
				code = GRASP_COPIER;
			} else if (subject == tt_toolbox_vacuum) {
				code = GRASP_VACUUM;
			} else if (subject == tt_toolbox_expander) {
				code = GRASP_EXPANDER;
			} else if (subject == tt_toolbox) {  
				return; // no semantic meaning
			};
		} else { // e.g. puzzle game
			switch (subject->kind_of()) {
				case COPIER:
					code = GRASP_COPIER;
					break;
				case VACUUM:
					code = GRASP_VACUUM;
					break;
				case EXPANDER:
					code = GRASP_EXPANDER;
					break;
			};
		};
	};
	if (subject != NULL && subject->kind_of() == NEST) {
		// don't wait for subject to receieve something to apply tool
		switch (code) {
			case COPIER_APPLY:
				code = COPIER_APPLY_NEST; 
				break;
			case VACUUM_APPLY:
				code = VACUUM_APPLY_NEST; 
				break;
			case APPLY_GRASPED_ITEM:
				code = APPLY_GRASPED_NEST;
				break;
		};
	};
	if (subject != NULL && code == RELEASE_ITEM) {
		subject->set_available_for_indexing(TRUE);
//		switch (subject->kind_of()) {
      if (tt_toolbox != NULL) {
		   if (subject == tt_toolbox_copier) {
			   code = RELEASE_COPIER;
		   } else if (subject == tt_toolbox_vacuum) {
			   code = RELEASE_VACUUM;
         } else if (subject == tt_toolbox_expander) {
			   code = RELEASE_EXPANDER;
		   } else if (subject == tt_toolbox) { // was subject == tt_toolbox_expander || 
			   return;
		   };
      } else { // puzzle game
			switch (subject->kind_of()) {
				case COPIER:
					code = RELEASE_COPIER;
					break;
				case VACUUM:
					code = RELEASE_VACUUM;
					break;
				case EXPANDER:
					code = RELEASE_EXPANDER;
					break;
			};
		};
	};
	// safe cast?
	path = ((Floor *) floor)->compute_path(subject,code,(Robot *) by,notebook_page_really_selected
#if TT_POST3187
								 ,original_subject
#endif		
		     );
#if TT_CAREFUL
	if (path == NULL && subject != tt_toolbox) { // ignore actions on Toolbox...
#if TT_DEBUG_ON
#if TT_NEW_IO
		output_string_stream err_message;
#else
		string buffer = new char[10000];
		output_string_stream err_message(buffer,10000);
#endif
		err_message << "Warning: Robot is confused and couldn't find ";
		subject->describe(err_message,INDEFINITE_ARTICLE);
		err_message << ".";
		err_message.put('\0');
		say_error(err_message.STR,TRUE);
#if !TT_NEW_IO
		delete [] buffer;
#endif
		log(event_string(code));
#endif
		return;
//      working_set->insert_at_end_if_not_member(subject);
//      path = compute_path(subject);
	};
#endif
//	if (code == RELEASE_ITEM_ON && by->kind_of() == ROBOT_IN_TRAINING &&
//		 subject->kind_of() != PROGRAM_PAD && subject->is_container()) {  // notebooks use this as well as cubbies
////		 && subject->pointer_to_leader() != NULL) {
//      // is now part of something and should be accessed via that container
//		((Robot *) by)->remove_tool_from_working_set(subject);
//	};
   //on 5/27 re-wrote the above to match the similar situation in event.cpp
   // handled by now_released_on
//   if (code == RELEASE_ITEM_ON && by->kind_of() == ROBOT_IN_TRAINING &&
//       // except when dropping a cubby on a number since both are around afterwards
//       !(item_underneath != NULL && item_underneath->kind_of() == INTEGER &&
//         subject->kind_of() == CUBBY)) {
//      ((Robot *) by)->remove_tool_from_working_set(subject);
//   };
	if (code == LABEL_CHARACTER) {
		path->add_to_end(new Path(label_index));
	};
	if (code == NEW_MAIN_CUBBY) { // do this after computing the path
		floor->set_body_cubby((Cubby *) subject, by);
	};
	if ((code == GRASP_ITEM || code == GRASP_NEST) && !notebook_page_really_selected) {
		subject->set_available_for_indexing(FALSE); // does this ever get restored??
	};
	Event *event = new Event(code,path,key);
	if (tt_events == NULL) {
		tt_events = new Events(event,tt_events);
	} else {
		tt_events->insert_at_end(event);
	};
	if (new_item_event != NULL) {
		tt_events->insert_at_end(new_item_event);
	};
   if (by->kind_of() == ROBOT_IN_TRAINING &&
       code != SELECT_STACK_ITEM) {
       if (subject != ((Robot *) by)->pointer_to_initial_tool()) {
          ((Robot *) by)->decrement_training_counter();
       };
   };
};

void Floor::remove_last_n_events(int count) {
	if (body_cubby == NULL) return;
	for (int i = 0; i < count; i++) {
	  if (tt_events != NULL) {
		  tt_events = tt_events->remove_last();
	  } else {
		  return; // warn??
	  };
	};
};

void Floor::set_body_cubby(Cubby *new_cubby, Sprite *by) {
	body_cubby->set_main_cubby_in_body(FALSE);
	body_cubby->set_available_for_indexing(TRUE);
	if (by->kind_of() == ROBOT || by->kind_of() == ROBOT_IN_TRAINING) {
		((Robot *) by)->add_to_working_set(body_cubby,TRUE);
	};
	new_cubby->set_main_cubby_in_body(TRUE);
	new_cubby->set_available_for_indexing(FALSE);
	body_cubby = new_cubby;
	body_cubby->increment_ref_count(); // new on 090203
};

void Floor::cubby_is_busy() {
	if (body_cubby != NULL) {
		body_cubby->increment_busy_counter();
	};
};

void Floor::cubby_is_not_busy() {
	if (body_cubby != NULL) {
		 body_cubby->decrement_busy_counter();
	};
};

void Floor::house_will_explode(millisecond duration) {
	room->house_will_explode(duration);
};

void Floor::destroy_house() {
	room->destroy_house();
};

boolean Floor::slated_for_destruction() {
	return(room != NULL && room->slated_for_destruction());
};

boolean Floor::inside(House *some_house) {
	if (some_house == NULL) return(TRUE); // am inside of course
	return(room->pointer_to_house() == some_house);
};

Path *Floor::compute_path(Sprite *subject, EventCode code, Robot *by,
								  boolean notebook_page_really_selected
#if TT_POST3187
								 ,Sprite *original_subject
#endif
								 ) {
	if (subject == body_cubby && 
		 (subject->pointer_to_leader() == NULL ||
		  subject->pointer_to_leader() == by)) {
		return(new Path(MAIN_CUBBY));
	};
	if (subject == NULL) { // e.g. APPLY_VACUUM_RESTORE
		return(new Path(NO_ARG));
	};
	if (subject == by->pointer_to_tool()) {
		return(new Path(ITEM_HELD));
	};
   Path *path;
   int backup_path_index = 0;
   int working_set_length;
	SpritePointer *working_set = by->working_set_and_size(working_set_length);
	for (int index = 0; index < working_set_length; index++) {
		Sprite *item = working_set[index];
      if (item == subject && backup_path_index == 0 && !item->available_for_indexing()) {
         backup_path_index = -(index+1);
      };
		if (item != NULL &&
			 (item->available_for_indexing() || // ok to be grasping it and typing
			  code == KEYBOARD_CHARACTER || code == KEYBOARD_CHARACTER_AND_SELECT)) {
			boolean notebook_page = (notebook_page_really_selected && item->kind_of() == PROGRAM_PAD);
#if TT_POST3187
			// this fixes Jack's bug "Wrong book" -- new on 280507 
  		   for (int i = index+1; i < working_set_length; i++) {
				if (working_set[i] == original_subject) { // another vacuum really did this
					notebook_page = FALSE;
					break;
				};
			};
#endif										 );
			if (item == subject || item == tt_just_vacuumed_remote || (code == VACUUM_APPLY && notebook_page)) { 
				// second disjunct added 091000 -- 
				// third on 110205 -- removed on 160205 || code == VACUUM_APPLY -- added back on 170205 with added notebook_page condition
				if (notebook_page && (item != subject || code != VACUUM_APPLY)) { // rewritten on 240205
					// added second condition on 170205 in case vacuuming up entire notebook
					Notebook *notebook = (Notebook *) item;
//					int page_number = notebook->return_recent_page_number()
//										  -notebook->return_page_number_offset();
					if (code == VACUUM_APPLY) {
						// new on 110205 -- not clear why the other branch uses ! but that's worked for a long time...
						return(new Path(-(index+1),new Path(notebook->is_left_page_selected())));
					} else {
						return(new Path(-(index+1),new Path(!notebook->is_left_page_selected()))); // page_number)));
					};
				} else if (tt_just_vacuumed == subject) { // new on 290704
					return(new Path(-(index+1),new Path(SPECIAL_PATH_MARKER,new Path(subject->kind_of()))));
				} else {
					return(new Path(-(index+1)));
				};
			} else if (tt_just_vacuumed == item) { // item was subject prior to 191199 so sometimes things not recorded right when vacuum used
				// not clear when this branch applies -- asked 290704
//				item->reset_appearance_before_vacuuming();
				return(new Path(-(index+1),new Path(SPECIAL_PATH_MARKER,new Path(item->kind_of())))); // ,new Path(SPECIAL_PATH_MARKER)
			};
			boolean ok = TRUE; // path is ok unless found otherwise
			path = item->path_to(subject,ok);
			if (ok) {
//				if (item->kind_of() == CUBBY) {
					return(new Path(-(index+1),path));
//				} else {
//					return(new Path(-(index+1),new Path(SPECIAL_PATH_MARKER,path)));
//				};
			};
/*
			switch (item->kind_of()) {
				case CUBBY:
				  path = ((Cubby *) item)->path_to(subject);
				  if (path != NULL) {
					  return(new Path(-(index+1),path));
				  };
				  break;
				case NEST: {
				  Nest *nest = (Nest *) item;
				  Sprite *nest_top = nest->pointer_to_contents();
				  if (nest_top == NULL) break;
				  if (nest_top == subject) {
					  return(new Path(-(index+1),NULL));
				  } else if (nest_top->kind_of() == CUBBY) {
					  Path *cubby_path = ((Cubby *) nest_top)->path_to(subject);
					  if (cubby_path != NULL) {
						  return(new Path(-(index+1),cubby_path));
					  };
				  };
				  break;
				};
				case TRUCK: {
//				  Cubby *trucks_cubby = ((Truck *) item)->pointer_to_cubby();
//				  if (trucks_cubby != NULL) {
//					  path = trucks_cubby->path_to(subject);
//				  };
				  CargoType type = ((Truck_Inside *) item)->cargo_type(subject);
				  if (type != TRUCK_UNKNOWN) {
					  return(new Path(-(index+1),new Path(type)));
				  };
				  };
				  break;
				case PICTURE:
					if (subject->pointer_to_leader() == item) {
						return(new Path(-(index+1),new Path(subject->kind_of())));
					};
					break;
				case REMOTE_PICTURE:
//				case ROBOT: // for returning cubby to empty thought bubble
					if (subject->appearance_before_vacuuming() == item) { // just vacuumed it off a remote
						subject->reset_appearance_before_vacuuming();
						return(new Path(-(index+1),new Path(0)));
					};
					break;
			};
*/
		};
	};
	if ((tt_toolbox != NULL && subject == tt_toolbox_copier) ||
	    (tt_toolbox == NULL && subject->kind_of() == COPIER)) {
      //(subject->kind_of() == COPIER) {
		return(new Path(A_COPIER));
	};
   if ((tt_toolbox != NULL && subject == tt_toolbox_expander) ||
		 (tt_toolbox == NULL && subject->kind_of() == EXPANDER)) {
      //(subject->kind_of() == EXPANDER) {
      return(new Path(AN_EXPANDER));
   };
	if ((tt_toolbox != NULL && subject == tt_toolbox_vacuum) ||
		 (tt_toolbox == NULL && subject->kind_of() == VACUUM)) {
		return(new Path(A_VACUUM));
	};
	if (subject->infinite_stack() && tt_toolbox != NULL) {
		if (subject == tt_toolbox->pointer_to_integer_stack()) {
			return(new Path(INTEGER_STACK));
		};
		if (subject == tt_toolbox->pointer_to_text_stack()) {
			return(new Path(TEXT_STACK));
		};
//   if (subject == tt_unsigned_long_int_stack) {
//      return(new Path(UNSIGNED_LONG_INT_STACK));
//   };
		if (subject == tt_toolbox->pointer_to_cubby_stack()) {
			if (code == RELEASE_ITEM_ON) {
				return(new Path(CUBBY_STACK,new Path(0)));
			} else {
				return(new Path(CUBBY_STACK));
			};
		};
		if (subject == tt_toolbox->pointer_to_nest_stack()) {
			return(new Path(NEST_STACK));
		};
		if (subject == tt_toolbox->pointer_to_bomb_stack()) {
			return(new Path(BOMB_STACK));
		};
		if (subject == tt_toolbox->pointer_to_truck_stack()) {
			return(new Path(TRUCK_STACK));
		};
		if (subject == tt_toolbox->pointer_to_robot_stack()) {
			return(new Path(ROBOT_STACK));
		};
		if (subject == tt_toolbox->pointer_to_scale_stack()) {
			return(new Path(SCALE_STACK));
		};
	};
	if (tt_toolbox != NULL) {
//		Notebook *main_notebook = pointer_to_toolbox_notebook(); // simplified on 150704
		if (subject == tt_toolbox_notebook) {
			if (code == VACUUM_APPLY) {
				return(NULL); // can't destroy main notebook
			} else if (notebook_page_really_selected) {
				return(new Path(PROGRAM_PADS,
									new Path(!tt_toolbox_notebook->is_left_page_selected()))); 
//									new Path(notebook->return_recent_page_number()-notebook->return_page_number_offset())));
			} else {
				return(new Path(PROGRAM_PADS));
			};
		} else if (subject->pointer_to_leader() == tt_toolbox_notebook) { // updated 150704 pointer_to_toolbox_notebook()) {
			// new on 180204 - can happen if use Magic wand on the page of the main notebook (what about other notebooks)
			return(new Path(PROGRAM_PADS,new Path(!tt_toolbox_notebook->is_left_page_selected())));
		};
	};
//	if (subject->kind_of() == ROBOT) {
//		Sprites *remaining = tt_notebooks;
//		int pad_number = 0;
//		int page_number;
//		while (remaining != NULL) {
//			Notebook *pad = (Notebook *) remaining->first();
//			if (pad->pointer_to_body() == ((Robot *) subject)->pointer_to_body()) {
//				page_number = pad->current_page_number();
//				return(new Path(PROGRAM_PADS,
//							  new Path(pad_number,new Path(page_number))));
//			};
//			pad_number++;
//			remaining = remaining->rest();
//		};
//	};
	boolean ok = TRUE; 
	if (body_cubby != NULL) { // conditional added on 250102 for robustness
		path = body_cubby->path_to(subject,ok);
	} else {
		ok = FALSE;
	};
	if (ok) {
		return(new Path(MAIN_CUBBY,(Path *) path));
	};
   // working set stuff was here -- moved earlier for new initial_tool stuff
/*
	Sprites *vacuum_contents = vacuum->contents();
	if (vacuum_contents == NULL) return(NULL);
	boolean found_in_vacuum;
	index = vacuum_contents->index_of(subject,found_in_vacuum);
	if (found_in_vacuum) {
		// since vacuum contents are like a stack index from bottom
		// to distinguish vacuum contents from working set make it negative
		return(new Path(-(vacuum_contents->length()-index)));
	};
*/
   if (backup_path_index != 0) return(new Path(backup_path_index));
	return(NULL);
};

Block *Floor::block_im_on() {
	if (room == NULL) return(NULL); //  e.g. on the ground?
	return(room->block_im_on());
};

House *Floor::pointer_to_house() {
	if (room == NULL || deletion_pending_flag) return(NULL); // e.g. on the ground
	return(room->pointer_to_house());
};

void Floor::set_previous_mouse(Mouse *mouse) {
	if (previous_mouse != NULL) {
		if (previous_mouse->pointer_to_background() != NULL) { // new on 120701
			// must be a second mouse has been called before first one finished
			previous_mouse->release_undo_information();
		} else {
			previous_mouse->destroy();
		};
	};
	previous_mouse = mouse;
	if (previous_mouse != NULL) { // added for robustness on 071003
		previous_mouse->increment_ref_count(); // new on 080202
	};
	//if (tt_events != NULL) {
	//	previous_undoable_event = tt_events->last(); // new on 090701
	//};
};

void Floor::undo_previous_mouse() {
	if (previous_mouse != NULL) {
		previous_mouse->finish_instantly();
		Mouse *saved_previous_mouse = previous_mouse;
		previous_mouse = NULL; // in case F11 is pressed twice
		saved_previous_mouse->undo_am_smashed(this);
	} else {
		play_sound(SHUTDOWN_SOUND); // new on 080701
	};
};

void Floor::destroy_previous_mouse_if_involving(Sprite *sprite) { // new on 071003
	if (previous_mouse != NULL && previous_mouse->undo_information_involves(sprite)) {
		previous_mouse->destroy();
		previous_mouse = NULL;
	};
};

void Floor::add_alternative_body_cubby(Cubby *cubby) {
	cubby->increment_ref_count();
	alternative_body_cubbies = new Sprites(cubby,alternative_body_cubbies);
};

void Floor::need_new_main_cubby() {
	Sprites *remaining = alternative_body_cubbies;
	while (remaining != NULL) {
		Cubby *cubby = (Cubby *) (remaining->first());
		if (cubby->vacuum_if_left_on_floor(NULL,0)) { // args are ignored
			record_event(NEW_MAIN_CUBBY,tt_programmer->pointer_to_appearance(),this,cubby);
		};
		remaining = remaining->rest();
	};
};

//void Floor::undo_previous_event() {
//	if (previous_undoable_event != NULL) {
//		Events *next = previous_undoable_event->rest();
//		if (next == NULL) { // nothing has been added since 
////			remove_last_n_events(1);
//			log("Having problems undoing Bammer's hammer during the training of a robot.");
//		} else { 
//			Event *event = next->first();
//			event->set_event_code(RELEASE_ITEM);
////			Events *next_next = next->rest();
////			previous_undoable_event->set_rest(next_next); // skip over the undone one
////			next->set_rest(NULL);
////			delete next;
//		};
//		previous_undoable_event = NULL;
//	};
//};

/*
Path *extra_path(Relation subject_relation) {
	switch (subject_relation) {
		case DIRECTLY_ON_TOP:
		  return(NULL);
		case TO_THE_LEFT:
		case TO_THE_RIGHT:
		  return(new Path(- (int) subject_relation));
	};
	tt_error_file() << "Warning extra_path called with unknown relation" << endl;
	return(NULL);
};
*/
/*  programmer appearance isn't in items
	Sprites *remaining = items;
	while (remaining != NULL) {
		Sprite *sprite = remaining->first();
		if (sprite->kind_of() == PROGRAMMER) {
			((Programmer_Sprite *) sprite)->house_about_to_be_destroyed();
			return;
		};
		remaining = remaining->rest();
	};
*/

/*
void Floor::decrement_truck_count() {
	truck_count--;
	if (truck_count == 0) {
      Truck *truck = new Truck(18*tile_width,4*tile_height,current_priority--,
                               this,tile_width/10);
      add_item(truck);
      truck->animate_size_and_location(tile_width*4,
                                       truck->height_given_width(tile_width*4),
                                       16*tile_width,
                                       3*tile_height,
                                       showing_on_a_screen()?1000:0);
   };
};
*/
//void Floor::replace_truck() {
//   remove_item(truck);




//Path *Floor::compute_path(int index) {
//   return(new Path(index));
//};


/*
// not treated as ordinary items because toolbox is responsible for them
void Floor::add_tools(Vacuum *new_vacuum,
							 Copier *new_copier,
							 Expander *new_expander,
							 Notebook *new_notebook) {
	vacuum = new_vacuum;
	copier = new_copier;
	expander = new_expander;
	notebook = new_notebook;
	vacuum->set_priority(current_priority--);
	copier->set_priority(current_priority--);
	expander->set_priority(current_priority--);
	notebook->set_priority(current_priority--);
	add_item(vacuum);
	add_item(copier);
	add_item(expander);
	add_item(notebook);
};	

void Floor::remove_tools() {
	if (vacuum == NULL) {
		tt_error_file() << "Removed tools twice." << endl;
		return;
	};
	remove_item(vacuum,FALSE);
	remove_item(copier,FALSE);
	remove_item(expander,FALSE);
	remove_item(notebook,FALSE);
	vacuum->stop_animation();
	copier->stop_animation();
	expander->stop_animation();
	notebook->stop_animation();
	vacuum = NULL;
	copier = NULL;
	expander = NULL;
	notebook = NULL;	
};

void Floor::add_to_working_set(Tool *item) {
	if (working_set == NULL) {
		working_set = new ToolPointer[max_working_set_size];
	} else {	// can I be more clever and never try to add same thing twice?
		for (int i = 0; i < working_set_length; i++) {
			if (working_set[i] == item) return; // already there
		};
	};
	working_set_length++;
	if (working_set_length > max_working_set_size) {
		Tool **new_working_set = new ToolPointer[max_working_set_size*2];
		for (int i = 0; i < max_working_set_size; i++) {
			new_working_set[i] = working_set[i];
		};
		delete [] working_set;
		working_set = new_working_set;
		max_working_set_size *= 2;
	};
	working_set[working_set_length-1] = item;
};
*/
/*
void Floor::remove_from_working_set(Sprite *subject) {
	if (working_set != NULL) { // shouldn't be NULL
		working_set = working_set->remove(subject);
	};
};
*/
//void Floor::remove_last_n_working_set_items(int count) {
//  working_set_length -= count;
/*
	for (int i = 0; i < count; i++) {
		if (working_set != NULL) {
			working_set = working_set->remove_last();
		} else {
			return; // warn??
		};
	};
*/
//};

/*
void Floor::body_defined() {
	if (working_set != NULL) {
		working_set->remove_all();
		delete working_set;
		working_set = NULL;
	};
};
*/


/*
void Floor::now_on_screen() {
//	Visible_Background::set_screen(new_screen);
	if (items != NULL) {// && screen != NULL) {
		items->add_all_to_screen(tt_screen);
//      tt_notebooks->add_all_to_screen(new_screen);
//      screen->add(tt_unsigned_short_int_stack);
//      screen->add(tt_unsigned_long_int_stack);
//		screen->add(tt_integer_stack);
//		screen->add(tt_cubby_stack);
//		screen->add(tt_nest_stack);
//		screen->add(tt_truck_stack);
//		if (body_cubby == NULL) {
//			screen->add(tt_thought_bubble_stack);
//			screen->add(tt_robot_stack);
//		};
	};
};
*/

