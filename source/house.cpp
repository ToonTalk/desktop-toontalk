// Copyright (c) 1992-2006.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif 	
#if !defined(__TT_SCREEN_H)   
#include "screen.h"
#endif   
//#include "quadtree.h"
#if !defined(__TT_CITY_H)   
#include "city.h"
#endif   
#if !defined(__TT_FLOOR_H)
#include "floor.h"
#endif  
#if !defined(__TT_TRUCK_H)   
#include "truck.h"
#endif   
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif
#if !defined(__TT_PUZZLE_H)   
#include "puzzle.h"
#endif   
#if !defined(__TT_CLEANUP_H)   
#include "cleanup.h"
#endif   
#if !defined(__TT_WINMAIN_H)   
#include "winmain.h"
#endif   
#if !defined(__TT_CITY_H)   
#include "city.h"
#endif
#if !defined(__TT_PRGRMMR_H)   
#include "prgrmmr.h"
#endif
#if !defined(__TT_HELP_H)   
#include "help.h"
#endif
#if !defined(__TT_PICTURE_H)
#include "picture.h"
#endif
#if !defined(__TT_PAD_H)
#include "pad.h"
#endif
#if !defined(__TT_NUMBER_H)
#include "number.h"
#endif
//#include <fstream.h>
#include <stdlib.h> // for min, max, range
#if !defined(__TT_HOUSE_H)   
#include "house.h"
#endif   
#if !defined(__TT_ROOM_H)   
#include "room.h"
#endif 

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
   
city_coordinate lot_width, house_width, half_house_width, house_height, half_house_height;
city_coordinate ideal_lot_width, ideal_house_width, ideal_half_house_width, ideal_house_height, ideal_half_house_height;

void initialize_house_sizes() {
	// when on the ground a block should be 4 screenfuls
	tt_default_block_width = 4L*ideal_screen_width;
	tt_default_ideal_block_width = tt_default_block_width; // unchanged by large screen
	tt_block_width = tt_houses_to_a_block*ideal_screen_width;
	tt_block_height = ideal_screen_width;
//	adjust_size_to_640x480_screen(tt_default_block_width,tt_block_height); // new on 160999
	lot_width = ideal_screen_width; // block_width/houses_to_a_block,
	ideal_lot_width = lot_width;
//	adjust_width_to_640x480_screen(lot_width);
//	adjust_width_to_640x480_screen(tt_block_width);
	house_width = (6*lot_width)/10;
	ideal_house_width = house_width;
	half_house_width = (5*house_width)/10;
	ideal_half_house_width = half_house_width;
	house_height = (4*tt_block_height)/10;
	ideal_house_height = house_height;
	half_house_height = (5*house_height)/10;
	ideal_half_house_height = half_house_height;
};

city_coordinate return_house_width() {
	return(house_width);
};

city_coordinate return_house_height() {
	return(house_height);
};

long house_counter = 0;

long current_house_counter() {
   return(house_counter);
};

//void reset_house_counter() { 
// new on 220204 for time travel -- not needed since counter is maintained as houses are destroyed and loaded 230204
//	house_counter = 0;
//};

//void record_house_counter() {
//  tt_error_file() << house_counter << " houses at frame " << tt_frame_number << endl;
//};

House::House(city_coordinate center_x, city_coordinate center_y,
				 Block *block, HouseStyle style, boolean instantly) :
	center_x(center_x), // can be computed needn't be stored
	center_y(center_y),
	style(style),
	block(block),
	min_x(center_x-half_house_width),
	max_x(center_x+half_house_width),
	min_y(center_y-half_house_height),
	max_y(center_y+half_house_height),
	appearance_from_side(NULL),
	appearance_from_above(NULL),
	door_appearance(NULL),
	door_status(DOOR_CLOSED),
	room(NULL),
	door_changed_time(0),
	begin_closing_door_time(0),
	house_creation_time(tt_current_time),
	about_to_be_destroyed(FALSE),
	house_built_flag(FALSE),
	truck_builder(NULL),
   priority(0) {
//   decoration_from_above(NULL),
//   decoration_from_side(NULL) {
	house_counter++;
	if (instantly || tt_screen->inside_a_house() || !tt_screen->visible_region(min_x,max_x,min_y,max_y)) {
		// houses are built instantly if I'm inside or looking at a different
      // part of the city
		 built();
	};
};

void House::built() {
	pointer_to_floor()->house_built();
	house_built_flag = TRUE;
};

House::~House()  {
#if TT_DEBUG_ON
	if (!about_to_be_destroyed && !tt_resetting_or_destroying_city) { // condition updated on 100603
		tt_error_file() << "About to delete house at " 
							 << center_x << "," << center_y << " though it wasn't marked for destruction. " << endl;
	};
	if (tt_debug_mode == 3231) {
		tt_error_file() << "Destroying house at " << center_x << "," << center_y << ". " 
							 << (house_counter-1) << " left. Frame=" << tt_frame_number << endl;
	};
#endif
	if (appearance_from_side != NULL) {
		appearance_from_side->destroy();
//      if (decoration_from_side != NULL) { // were added as followers so just remove all and destroy empty list
//         decoration_from_side->remove_all();
//         delete decoration_from_side;
//         decoration_from_side = NULL;
//      };
   };
//   destroy_sprites(decoration_from_side); // if there are some but weren't added as followers
	if (appearance_from_above != NULL) {
		appearance_from_above->destroy();
//      if (decoration_from_above != NULL) { // were added as followers so just remove all and destroy empty list
//         decoration_from_above->remove_all();
//         delete decoration_from_above;
//         decoration_from_above = NULL;
//      };
	};
//   destroy_sprites(decoration_from_above);
	if (door_appearance != NULL) {
		door_appearance->destroy();
	};
//		delete lawn_display_layer;
//  if (above_display_layer != NULL) {
//     delete above_display_layer;
//  };
  house_counter--;
  if (house_counter == 0 && !tt_resetting_or_destroying_city && !tt_loading_a_city && !any_trucks_outside()) { 
	  // added !tt_loading_a_city on 170403
	  // tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 
	  // but can cause memory errors while resetting during time travel
	  tt_city->build_initial_houses();
	  say(IDS_ALL_HOUSES_DESTROYED);
  } else if (tt_system_mode == PUZZLE && this == pointer_to_puzzle_house() && !tt_resetting_or_destroying_city) {
	  // tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 
	  // but can cause memory errors while resetting during time travel
     House *replacement = new House(center_x,center_y,block,style);
	  block->add_house(replacement,0);
     if (room != NULL) {
        Sprite *old_decoration = room->pointer_to_wall_decoration_sprite();
        if (old_decoration != NULL) {
           Room *new_room = replacement->pointer_to_room();
           new_room->set_wall_decoration_sprite(old_decoration->copy());
        };
     };
     reload_puzzle(replacement);
  };
  if (room != NULL) {
	  room->destroy();
     //delete room;
  };
  if (truck_builder != NULL) {
	  truck_builder->reset_house();
  };
};

void House::display(TTRegion *view_region) {
  if (!view_region->partly_inside(min_x,max_x,min_y,max_y)) return;
  switch (tt_screen->camera_status()) {
	 case CAMERA_IN_FRONT:
		front_view(view_region);
//      if (decoration_from_side != NULL) decoration_from_side->display();
		break;
	 case CAMERA_ABOVE:
		view_from_above();
//      if (decoration_from_above != NULL) decoration_from_above->display();
		break;
	 default:
		say_error(IDS_BAD_HOUSE_VIEW);
		tt_error_file() << "Unrecognized camera status to display House. " << endl;
//		tt_exit_failure();
  };
};

void House::set_decoration(Sprites *sprites, boolean from_side, Sprite *relative_to, 
									boolean absolute_coordinates, city_coordinate delta_x, city_coordinate delta_y,
									boolean dont_record_new_offsets) { // new arg on 240203 for loading XML
   if (from_side) {
		if (appearance_from_side != NULL) {
			appearance_from_side->remove_all_followers(TRUE);
		};
   } else {
		if (appearance_from_above != NULL) {
			appearance_from_above->remove_all_followers(TRUE);
		};
   };
	if (sprites != NULL) {
		Sprites *remaining = sprites;
		while (remaining != NULL) {
			Sprite *sprite = remaining->first();
			if (relative_to != NULL) {
				position_and_add_to_house(relative_to,sprite,this,from_side);
			} else {
				sprite->move_by(delta_x,delta_y); // new on 070200 so can paste houses and decoration ends up at the right place
				add_decoration(sprite,from_side,absolute_coordinates,dont_record_new_offsets);
			};
			remaining = remaining->rest();
		};
		if (dont_record_new_offsets) { // new on 240203
			if (from_side) {
				appearance_from_side->really_propagate_changes();
			} else {
				appearance_from_above->really_propagate_changes();
			};
		};
	};
};

void House::add_decoration(Sprite *addition, boolean from_side, boolean absolute_coordinates, boolean dont_record_new_offsets) {
   // addition is given in relative coordinates -- update it first
   // 1000 units is full width or height of house appearance
   Sprite *appearance;
   if (from_side) {
      ensure_appearance_from_side();
      appearance = appearance_from_side;
   } else {
      ensure_appearance_from_above();
      appearance = appearance_from_above;
   };
   if (!absolute_coordinates) {
		// replaced true_ with current_ on 121204
      city_coordinate appearance_width = appearance->current_width();
      city_coordinate appearance_height = appearance->current_height();
      addition->set_size_and_location((addition->current_width()*appearance_width)/1000,
                                      (addition->current_height()*appearance_height)/1000,
                                      (addition->current_llx()*appearance_width)/1000+appearance->current_llx(),
                                      (addition->current_lly()*appearance_height)/1000+appearance->current_lly());
   };
//   if (from_side) {
//      appearance->add_follower(addition,FALSE,FALSE);  // decoration might be outside
//   } else {  // doesn't clip ...
      appearance->add_follower(addition,TRUE,INSERT_AT_END,dont_record_new_offsets);
//   };
   addition->set_subpicture(TRUE);
   addition->set_priority(priority--);
   addition->recursively_activate_pictures();
//   if (!from_side) {
//      addition->set_screen_updates(TRUE); // but taken care of by the screen when seen from above
//   };
//   appearance->set_screen_updates(TRUE);
/*
   // and save below to recreate if appearance is destroyed
   if (from_side) {
      if (decoration_from_side == NULL) {
         decoration_from_side = new Sprites(addition);
      } else {
         decoration_from_side->insert_by_priority(addition);
      };
   } else {
      if (decoration_from_above == NULL) {
         decoration_from_above = new Sprites(addition);
      } else {
         decoration_from_above->insert_by_priority(addition);
      };
   };
*/
};

boolean House::visible() {
   return(tt_screen->viewing_region()->partly_inside(min_x,max_x,min_y,max_y));
};

// for now just one house image shared by everyone
// can have a small set with different appearances

//Sprite *appearance_from_above = NULL;
// Sprite *appearance_from_side = NULL;

//void release_house_sprites() {
//   if (appearance_from_above != NULL) {
//      delete appearance_from_above;
//   };
//   appearance_from_above = NULL;
//   if (appearance_from_side!= NULL) {
//      delete appearance_from_side;
//   };
//   appearance_from_side = NULL;
//};

const int collision_height_ratio = 8;

void House::ensure_appearance_from_side() {
   if (appearance_from_side != NULL) {
      if (door_appearance == NULL) create_door(); // but be sure that door does
      return; // already exists
   };
   SpriteCode house_sprite_code = house_from_side_sprite(style);
   city_coordinate good_x, good_y;
   good_lower_left_corner(house_sprite_code,good_x,good_y);
   short initial_parameter = 0;
   if (!is_built() && house_sprite_code != BROKEN_ROCKET_SPRITE && tt_current_time > tt_open_log_time) {
		// added tt_current_time > tt_open_log_time on 161299 so if starting new log doesn't show building
		// added is_built() on 090103
		initial_parameter = HOUSE_BUILD;
	};
   appearance_from_side = new House_Sprite(good_x,good_y,
														 this,house_creation_time,
														 house_sprite_code,initial_parameter,HOUSE);
//	appearance_from_side->set_size_porportional_to_screen_size(TRUE);
   appearance_from_side->update_display(); // to cycle forward
   priority = appearance_from_side->priority();
   if (door_appearance == NULL) create_door();
//	door_appearance->set_size_porportional_to_screen_size(TRUE);
//   Sprites *remaining = decoration_from_side;
//   while (remaining != NULL) {
//      appearance_from_side->add_follower(remaining->first(),FALSE,TRUE);  // might be outside and priority counts
//      remaining = remaining->rest();
//   };
};

void House::front_view(TTRegion *view_region) {
  if (appearance_from_side != NULL) { // already visible
     if (door_appearance == NULL) create_door();
	  if (begin_closing_door_time != 0 &&
			tt_current_time > begin_closing_door_time) {
		  close_door(); // if open otherwise no-op
	  };
//	  appearance_from_side->update_display();
	  if (door_appearance != NULL) {
		  door_appearance->update_display();
		  tt_screen->add(door_appearance);
	  };
	  // no-op if already there -- BUT A WASTE OF TIME
	  if (tt_screen->add(appearance_from_side) && appearance_from_side->pointer_to_followers() != NULL) {
        // it is new, so be sure background knows about it
//        appearance_from_side->set_background(tt_programmer->pointer_to_background());
//        tt_screen->pointer_to_background()->add_leader(appearance_from_side);
		  appearance_from_side->set_background(tt_screen->pointer_to_background()); // rewritten on 260405
		  // the above will do the add_leader if needed but also maintain things so it is removed if need be
		  // why isn't there a similar piece of code for appearance_from_above? asked 260405
		  // and why doesn't this use background::add_item instead? -- except that isn't right either
     };
  } else { // don't bother if off screen
	  SpriteCode house_sprite_code = house_from_side_sprite(style);
	  city_coordinate half_width = maximum_width(house_sprite_code)/2;
	  city_coordinate half_height = maximum_height(house_sprite_code)/2;
	  if (!view_region->partly_inside(center_x-half_width,center_x+half_width,
												 center_y-half_height,center_y+half_height)) {
		  return;
	  };
//	  if (appearance_from_above != NULL) {
//		  appearance_from_above->finish_instantly();
//		  appearance_from_above->destroy();
//		  appearance_from_above = NULL;
//	  };
	  if (about_to_be_destroyed) return;
     ensure_appearance_from_side();
	  if (house_creation_time == tt_current_time && !is_built()) { // added !is_built() on 090103
		  play_sound(LOTS_OF_SILLY_SOUNDS);
	  };
     if (appearance_from_above != NULL) {
			appearance_from_above->finish_instantly();
//			AnimationCallBack callback
//				  = appearance_from_above->pointer_to_cycle_stopped_callback();
//			if (callback != NULL) { // swap them
//				appearance_from_side->set_cycle_stopped_callback(callback,TRUE);
//				appearance_from_above->reset_cycle_stopped_callback();
//			};
	  };
/*
	  city_coordinate house_llx, house_lly, door_llx, door_lly;
	  appearance_from_side->lower_left_corner(house_llx,house_lly);
	  door_appearance->lower_left_corner(door_llx,door_lly);
	  city_coordinate gap = door_lly-house_lly;
	  if (gap > 0) { // need door to be lower to be sure it is hit first
		  door_appearance->move_by(0,-(gap+1));
	  };
*/
     if (tt_screen->add(appearance_from_side) && appearance_from_side->pointer_to_followers() != NULL) {
        // and if it is new set_background in case it has animating followers
        appearance_from_side->set_background(tt_programmer->pointer_to_background()); // not clear why this is needed given the above -- asked 260405
     };
	  // to be on top
	  tt_screen->add(door_appearance);
  };
};

void House::ensure_appearance_from_above() {
   if (appearance_from_above == NULL) {
		// prior to 141299 it was > 0 (and this was outside the conditional)
      // commented out the following on 221102 since tt_loading_a_city makes more sense
//		boolean new_house = (house_creation_time == tt_current_time && tt_current_time > tt_open_log_time); // not during initialize of a city
		SpriteCode house_sprite_code = house_from_above_sprite(style);
//		city_coordinate good_x,good_y;
//		good_lower_left_corner(house_sprite_code,good_x,good_y);
		appearance_from_above = 
				  new House_Sprite(min_x-ideal_house_width/8,
										 min_y-ideal_house_height/2, //good_x,good_y,
										 this,house_creation_time,
										 house_sprite_code,
										 (short) (house_built_flag?HOUSE_BUILT:HOUSE_BUILD), 
										 // updated on 221102 // updated again 251102 -used to use tt_loading_a_city
                               HOUSE_ABOVE);
      appearance_from_above->update_display(); // added on 12/12/97 to fix problems with roof remotes
		// don't remember why the following works but it improves house placement
		// is it because sprites take lower left corner not center now?
//		appearance_from_above->move_to(min_x-house_width/8,
//												 min_y-house_height/8);
// problem with this is that built call back gets clobbered
//				AnimationCallBack callback
//				  = appearance_from_side->pointer_to_cycle_stopped_callback();
//				if (callback != NULL) { // swap them
//					appearance_from_above->set_cycle_stopped_callback(callback,TRUE);
//					appearance_from_side->reset_cycle_stopped_callback();
//				};
//			};
//		   appearance_from_above->set_size(house_width,house_height);
		if (!is_built()) { // updated 251102
			// was !tt_loading_a_city && !tt_skip_titles) { // if skipping titles then reloading a new demo so no funny sounds
			play_sound(LOTS_OF_SILLY_SOUNDS);
		};
//      Sprites *remaining = decoration_from_above;
//      while (remaining != NULL) {
//         appearance_from_above->add_follower(remaining->first(),FALSE,TRUE);  // might be outside and priority counts
//         remaining = remaining->rest();
//      };
	};
};

void House::view_from_above() {
	city_scale scale = tt_screen->current_scale();
/* following was an optimization but not too good when houses are changing
	if (scale > maximum_scale_for_house_image) {
		// for now the height of the house is also its depth
		// (i.e. width from above)
#if WING
		tt_main_window->select_brush(roof_brush(style));
		screen->draw_rectangle(min_x,max_x,min_y,max_y);
#else
		screen->clip_rectangle(RED,min_x,max_x,min_y,max_y);
#endif
	} else {
*/
	if (appearance_from_side != NULL) {
		appearance_from_side->finish_instantly();
		if (door_appearance != NULL) {
			tt_screen->remove(door_appearance);
			door_appearance->destroy();
			door_appearance = NULL;
		};
//		appearance_from_side->destroy();
//		appearance_from_side = NULL;
	};
	if (about_to_be_destroyed) return;
   ensure_appearance_from_above();
//	appearance_from_above->update_display(); // if cycling or animating
   city_coordinate width = appearance_from_above->current_width();
   city_coordinate height = appearance_from_above->current_height();
	city_coordinate new_width = (ideal_house_width*ground_scale)/scale;
	city_coordinate new_height = ((height*ground_scale/(scale)*ideal_house_width))/width; // maintain proportionality
//	adjust_size_from_640x480_screen(new_width,new_height);
	appearance_from_above->scale_to_fit(new_width,new_height);
	appearance_from_above->display();
};

Picture *House::picture_of_appearance(boolean from_side) {
   Picture *copy;
//   Sprite *appearance;
   if (from_side) {
      copy = new Picture(0,0,priority--,HOUSE_BUILT,house_from_side_sprite(style));
//      appearance = appearance_from_side;
   } else {
      copy = new Picture(0,0,priority--,HOUSE_BUILT,house_from_above_sprite(style));
//      appearance = appearance_from_above;
   };
	copy->set_show_all(FALSE);
/*
// new policy is that followers are of remote_picture not its appearance
   Sprites *remaining = NULL;
   if (appearance != NULL) remaining = appearance->pointer_to_followers();
   while (remaining != NULL) {
      Sprite *sprite = remaining->first();
      Sprite *sprite_copy = sprite->copy();
      set_relative_size_and_location(sprite,appearance,sprite_copy,copy);
      copy->add_follower(sprite_copy);
      remaining = remaining->rest();
   };
*/
   return(copy);
};

Sprite *House::pointer_to_appearance(boolean from_side) {
   if (from_side) {
      return(appearance_from_side);
   } else {
      return(appearance_from_above);
   };
};
Sprites *House::pointer_to_decorations(boolean from_side) {
   Sprite *appearance;
   if (from_side) {
      appearance = appearance_from_side;
   } else {
      appearance = appearance_from_above;
   };
   if (appearance == NULL) return(NULL);
   return(appearance->pointer_to_followers());
};

void House::good_lower_left_corner(SpriteCode house_sprite_code,
                                   city_coordinate &good_x,
											  city_coordinate &good_y) {
	city_coordinate half_width = maximum_width(house_sprite_code)/2;
	city_coordinate half_height = maximum_height(house_sprite_code)/2;
//	adjust_size_to_640x480_screen(half_width,half_height);
	good_x = center_x-half_width; // looks nicer
	good_y = center_y-half_height; // looks nicer
	if (style == HOUSE_C) { // not very tall so move it down
		good_y -= half_height/2;
	};
};

//const
city_coordinate broken_rocket_x_offset = -4500; // 9000;
city_coordinate broken_rocket_y_offset = -4400;

void House::create_door() {
   SpriteCode door_style = door_sprite(style);
   city_coordinate door_x, door_y;
   good_lower_left_corner(house_from_side_sprite(style),door_x,door_y);
   if (door_style == BROKEN_ROCKET_DOOR_SPRITE) {
      appearance_from_side->lower_left_corner(door_x,door_y);
      door_x += broken_rocket_x_offset;
      door_y += broken_rocket_y_offset;
   };
	door_appearance = new Door(door_x,door_y,this,door_style);
//	door_appearance->set_priority_function_of_lly(FALSE);
   door_appearance->set_priority_fixed(TRUE);
   door_appearance->update_display(); // otherwise takes a frame for its big offsets to be noticed
   // how should this work?  should sprite::sprite be changed??
	if (appearance_from_side != NULL) { // might be a truck leaving a newly built house ...
		door_appearance->set_priority(appearance_from_side->priority()-1);
	};
};

void House::update_display() {
   if (tt_screen->camera_status() == CAMERA_IN_FRONT) {
      if (appearance_from_side != NULL) {
		   appearance_from_side->recursively_update_display();
	   };
   } else {
      if (appearance_from_above != NULL) {
		   appearance_from_above->recursively_update_display();
//         if (appearance_from_above->pointer_to_followers() != NULL) {
//            appearance_from_above->propagate_changes(); // isn't an item in background like usual case
//            appearance_from_above->set_changes_propagated(FALSE); // for next time
//         };
      };
	};
};

void House::finish_instantly() {
	if (appearance_from_above != NULL) {
		appearance_from_above->finish_instantly();
	};
   if (appearance_from_side != NULL) {
		appearance_from_side->finish_instantly();
	};
   if (door_appearance != NULL) {
      door_appearance->finish_instantly();
   };
};

#if TT_XML

void House::top_level_xml(xml_document *document, xml_element *parent) { // new on 300904
	//if (parent != NULL) {
	//	parent->Release(); // not used -- new on 240105
	//};
	xml_element *house_element = xml(document);
	xml_append_child(house_element,document);
};

xml_element *House::xml(xml_document *document) { // new on 011102
   xml_element *house_element = create_xml_element(L"House",document);
   xml_set_attribute(house_element,L"StyleCode",(int) style);
	// following were missing prior to 180603
	if (door_changed_time > 0 && tt_dump_reason != DUMPING_TO_CLIPBOARD) { // second condition new on 040803
		xml_set_attribute(house_element,L"DoorChangedTime",door_changed_time);
	};
	if (begin_closing_door_time > 0 && tt_dump_reason != DUMPING_TO_CLIPBOARD) { // second condition new on 040803
		xml_set_attribute(house_element,L"BeginClosingDoorTime",begin_closing_door_time);
	};
	if (house_creation_time > 0 && tt_dump_reason != DUMPING_TO_CLIPBOARD) { // second condition new on 040803
		xml_set_attribute(house_element,L"HouseCreationTime",house_creation_time);
	};
   xml_append_child(pointer_to_room()->xml(document),house_element);
   xml_append_child(pointer_to_floor()->xml(document),house_element);
   Sprites *decorations = pointer_to_decorations(FALSE);
   if (decorations != NULL) {
		// dumping coordinates relative to house center
      xml_element *roof_element = create_xml_element(L"OnRoof",document);
      decorations->add_xml(roof_element,document,NULL,TRUE,FALSE,NULL,NULL,-center_x,-center_y);
		xml_append_child(roof_element,house_element); // moved here on 190804
   };
   decorations = pointer_to_decorations(TRUE);
   if (decorations != NULL) {
		// dumping coordinates relative to house center
      xml_element *wall_element = create_xml_element(L"OnFrontWall",document);
      decorations->add_xml(wall_element,document,NULL,TRUE,FALSE,NULL,NULL,-center_x,-center_y);
		xml_append_child(wall_element,house_element); // moved here on 190804
   };
	Houses *remaining = tt_deleted_houses;
	while (remaining != NULL) { // new on 140205 since may save city while a house is just a frame away from being removed
		if (remaining->first() == this) {
			xml_set_attribute(house_element,L"AboutToBeDestroyed",1);
			break;
		};
		remaining = remaining->rest();
	};
   return(house_element);
};

boolean House::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
      case ROOM_TAG:
         return(xml_entity(node,pointer_to_room()));
      case FLOOR_TAG:
         return(xml_entity(node,pointer_to_floor()));
      case ON_ROOF_TAG: {
	      ensure_appearance_from_above(); 
			// so that load_items knows its container - otherwise won't restore picture right -- still needed? 051102
         Sprites *decorations = xml_load_sprites(node);
         if (decorations != NULL) {
				// not clear if the center_x, center_y stuff is needed anymore but doesn't seem to hurt either - noted 240203
		      set_decoration(decorations,FALSE,NULL,TRUE,center_x,center_y,TRUE); // last TRUE added 240203
		      just_destroy_list(decorations); // new on 261000
	      };
         return(TRUE);
                        };
      case ON_FRONT_WALL_TAG: {
         ensure_appearance_from_side(); 
			// so that load_items knows its container - otherwise won't restore picture right -- still needed? 051102
         Sprites *decorations = xml_load_sprites(node);
         if (decorations != NULL) {
		      set_decoration(decorations,TRUE,NULL,TRUE,center_x,center_y,TRUE); // last TRUE added 240203
		      just_destroy_list(decorations); // new on 261000
	      };
         return(TRUE);
                              };
      case NO_MORE_TAGS:
			// following new on 180603
			door_changed_time = xml_get_attribute_int(node,L"DoorChangedTime",0); // prior to 210404 door_changed_time
			begin_closing_door_time = xml_get_attribute_int(node,L"BeginClosingDoorTime",0); 
			// prior to 210404 begin_closing_door_time
			house_creation_time = xml_get_attribute_int(node,L"HouseCreationTime",0); // prior to 210404 house_creation_time
			if (xml_get_attribute_int(node,L"AboutToBeDestroyed",0)) { // new on 140205
				tt_deleted_houses = new Houses(this,tt_deleted_houses);
			};
         return(TRUE);
      default:
         // collect extra_xml?? - 051102
         return(FALSE); 
   };
};

xml_element *House::xml_path(xml_document *document) { // abstracted on 201202
	xml_element *path_element = create_xml_element(L"HouseLocation",document);
	block_number block_x, block_y;
	block->location(block_x,block_y);
	int house_number = block->lot_index(this);
	xml_set_attribute(path_element,L"BlockX",block_x);
	xml_set_attribute(path_element,L"BlockY",block_y);
	xml_set_attribute(path_element,L"LotNumber",house_number+1); // switch from 0-indexing to 1
	return(path_element);
};

House *house_at_location(xml_node *node) { // abstracted on 201202
	int block_x = xml_get_attribute_int(node,L"BlockX",-1);
	if (block_x < 0) return(NULL);
	int block_y = xml_get_attribute_int(node,L"BlockY",-1);
	if (block_y < 0) return(NULL);
	Block *block = tt_city->block_at(block_x,block_y,FALSE);
	if (block == NULL) return(NULL);
	int lot_number = xml_get_attribute_int(node,L"LotNumber",0)-1; // convert from 1-indexing to 0-indexing
//	if (lot_number < 0 || lot_number >= tt_houses_to_a_block) return(NULL); // house_at now does this check
	return(block->house_at(lot_number));
};

#endif

void House::dump(output_stream &stream, boolean just_prepare) {
   Sprites *decorations = pointer_to_decorations(FALSE);
   if (just_prepare) {
      finish_instantly();
      pointer_to_room()->dump(stream,just_prepare);
		pointer_to_floor()->dump(stream,just_prepare); 
		// this was missing prior to 250399 so birds, etc didn't finish before city was saved
      if (decorations != NULL) decorations->dump(stream,FALSE,just_prepare);
      decorations = pointer_to_decorations(TRUE);
      if (decorations != NULL) decorations->dump(stream,FALSE,just_prepare);
      return;
   };
   stream.put(HOUSE_MARKER);
   stream.put((char) style);
   pointer_to_room()->dump(stream,just_prepare);
   pointer_to_floor()->dump(stream,just_prepare);
   if (decorations != NULL) {
		// starting 070200 dumping coordinates relative to house center
      decorations->dump(stream,TRUE,FALSE,FALSE,NULL,NULL,-center_x,-center_y);
   };
   stream.put(END_OF_LIST_MARKER);
   decorations = pointer_to_decorations(TRUE);
   if (decorations != NULL) {
		// starting 070200 dumping these coordinates relative to house center 
		// (since appearance while being built changes - could have used true_llx())
      decorations->dump(stream,TRUE,FALSE,FALSE,NULL,NULL,-center_x,-center_y);
   };
   stream.put(END_OF_LIST_MARKER);
};

House *load_house(InputStream *stream, int notebook_version, NaturalLanguage language, 
						city_coordinate x, city_coordinate y, Block *block) {
   House *house = new House(x,y,block,(HouseStyle) stream->get());
   house->load(stream,notebook_version,language);
   return(house);
};

void House::load(InputStream *stream, int notebook_version, NaturalLanguage language) {
#if TT_DEBUG_ON
	millisecond start;
	if (tt_debug_mode == 260900) {
		start = timeGetTime();
	};
#endif
   pointer_to_room()->load(stream,notebook_version,language);
   pointer_to_floor()->load(stream,notebook_version,language);
	ensure_appearance_from_side(); 
	// added this on 210699 so that load_items knows its container - otherwise won't restore picture right
	ensure_appearance_from_above();
   Sprites *decorations = load_items(stream,notebook_version,language,NULL,TRUE);
	city_coordinate delta_x, delta_y;
	if (notebook_version >= first_version_with_large_pages) {
		delta_x = center_x;
		delta_y = center_y;
	} else {
		delta_x = 0;
		delta_y = 0;
	};
   if (decorations != NULL) {
		set_decoration(decorations,FALSE,NULL,TRUE,delta_x,delta_y);
		just_destroy_list(decorations); // new on 261000
	};
   decorations = load_items(stream,notebook_version,language,NULL,TRUE);
   if (decorations != NULL) {
		set_decoration(decorations,TRUE,NULL,TRUE,delta_x,delta_y);
		just_destroy_list(decorations); // new on 261000
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 260900) {
		tt_error_file() << "Took " << (timeGetTime()-start) << "ms to load house; x=" << center_x << "; y=" << center_y << endl;
	};
#endif
};

boolean House::showing_from_above() {
	if (appearance_from_above == NULL) return(FALSE);
	if (tt_screen->camera_status() != CAMERA_ABOVE) return(FALSE);
	return(tt_screen->viewing_region()->partly_inside(min_x,max_x,min_y,max_y));
};

void House::house_center(city_coordinate &x, city_coordinate &y) {
	  x = center_x;
	  y = center_y;
};

void House::exit_point(city_coordinate &x, city_coordinate &y) {
	if (door_appearance != NULL) {
		TTRegion door_region;
		door_collision_region(door_region);
		x = (door_region.min_x+door_region.max_x)/2;
		y = door_region.max_y;
	} else {
		Sprite *house_sprite = appearance_from_above;
		if (house_sprite == NULL) { // maybe just in transition between above and side views
			house_sprite = appearance_from_side;
		};
		if (house_sprite == NULL) { // not very accurate for good enuf...
			x = center_x;
			y = min_y;
		} else {
			house_sprite->center_location(x,y);
			y = house_sprite->current_lly();
		};
	};
};

void House::door_sound() {
   if (door_appearance == NULL) return;
   if (door_appearance->appearance_code() == BROKEN_ROCKET_DOOR_SPRITE) {
      play_sound(DOOR_SOUND); // open vs close??
   } else {
      play_sound(DOOR_CREAK_SOUND);
   };
}; 

void House::blow_up() {
	if (door_appearance != NULL) { // "close" door
		door_appearance->set_closed(TRUE);
	};
	door_status = DOOR_ABOUT_TO_EXPLODE;
	House_Sprite *appearance = NULL;
	if (showing_from_above()) {
		appearance = appearance_from_above;
	} else if (appearance_from_side != NULL && 
				  (tt_screen->showing(appearance_from_side) || programmer_in_house())) {
		appearance = appearance_from_side;
	};
	if (appearance == NULL) { // off screen
		destroyed();
	} else {
      appearance->remove_all_followers(TRUE); 
		// new on 031002 to fix bug blowing up decorated houses -- done here so you don't see the followers on the explosion image
      tt_city->remove_leader(appearance); // new on 201102 since was added earlier by House
		appearance->set_parameter(HOUSE_EXPLODE);
		millisecond explosion_duration = appearance->total_cycle_duration();
		appearance->set_cycle_stopped_callback(HOUSE_DESTROYED_CALLBACK,TRUE);
//		appearance_from_side->do_after(explosion_duration*2,appearance_from_side,house_destroyed_callback);
		appearance->do_after(explosion_duration/3,appearance,PLAY_EXPLOSION_CALLBACK);
	};
};

boolean House::programmer_in_house() {
	Background *programmers_background = tt_programmer->pointer_to_appearance()->pointer_to_background();
	if (programmers_background != NULL) {
		return(programmers_background->inside(this));
	};
	return(FALSE);
};

void House::destroyed() {
#if TT_DEBUG_ON
	if (about_to_be_destroyed) {
		tt_error_file() << "About to delete a house at " << center_x << "," << center_y << " twice!" << endl;
		return;
	};
	if (tt_debug_mode == 3231) {
		tt_error_file() << "Adding house at " << center_x << "," << center_y << " to deleted list. " 
							 << (house_counter-1) << " left. Frame=" << tt_frame_number << endl;
	};
#endif
	tt_deleted_houses = new Houses(this,tt_deleted_houses);
	about_to_be_destroyed = TRUE;
};

void House::remove_from_screen(Screen *screen) {
	screen->remove(appearance_from_side);
	if (door_appearance != NULL) {
		screen->remove(door_appearance);
	};
//   screen->remove(lawn_display_layer);
};

House_Sprite::House_Sprite(city_coordinate x, city_coordinate y,
									House *house, millisecond house_creation_time,
									SpriteCode code, 
									short parameter, SpriteType type) :
  Sprite(code,x,y,type,parameter),
  house(house),
  frame_when_last_displayed(-1) {
//    set_size(house_width,house_height);
    set_sprite_type(type);
    set_is_picture(TRUE); // since can have followers with robots on the back...
	 // even lower priority so lawn isn't included
//	 current_priority += height/4;
    set_priority(max_long); // so decorations are on top
	 set_cycle_start_time(house_creation_time);
	 if (!house->is_built()) {
		 set_cycle_stopped_callback(HOUSE_BUILT_CALLBACK,TRUE);
		 house->pointer_to_floor()->cubby_is_busy(); // until house is built
		 update_display(); // given new time above
	 };
	 set_scale_for_height_of_camera(FALSE); // new on 230804
};

void House_Sprite::house_built() {
	house->built();
};

void House_Sprite::house_destroyed() {
	house->destroyed();
};

void House_Sprite::set_priority(long new_priority) {
   unsigned long delta; // on 121099 delta is now an unsigned long
	boolean delta_is_positive;
	if (new_priority > priority()) {
		delta_is_positive = TRUE;
		delta = new_priority-priority(); 
	} else {
		delta_is_positive = FALSE;
		delta = priority()-new_priority;
	};
   if (delta == 0) return;
   Sprite::set_priority(new_priority);
   Sprites *remaining = followers;
   while (remaining != NULL) {
      Sprite *sprite = remaining->first();
		if (delta_is_positive) {
			sprite->set_priority(sprite->priority()+delta);
		} else {
			sprite->set_priority(sprite->priority()-delta);
		};
      remaining = remaining->rest();
   };
   Sprite *door = house->pointer_to_door();
   if (door != NULL) {
		door->set_priority(new_priority-1); // new on 121099 so even if starting out in a house from saved city this looks right
	};
};

void House_Sprite::display(SelectionFeedback selection_feedback, boolean followers_too, TTRegion *region) {
	// new on 290500 so decorations run only if being watched
	Sprite::display(selection_feedback,followers_too,region);
	if (frame_when_last_displayed+1 < tt_frame_number) { // first display in a while
		recursively_activate_pictures();
	};
	frame_when_last_displayed = tt_frame_number;
};

SpriteCode house_from_side_sprite(HouseStyle style) {
	switch (style) {
		case HOUSE_A:
			return(HOUSE_A_SIDE_SPRITE);
		case HOUSE_B:
			return(HOUSE_B_SIDE_SPRITE);
      case HOUSE_C:
	      return(HOUSE_C_SIDE_SPRITE);
      case BROKEN_ROCKET:
         return(BROKEN_ROCKET_SPRITE);
   };
   return(HOUSE_C_SIDE_SPRITE); // warn??
};

SpriteCode house_from_above_sprite(HouseStyle style) {
	switch (style) {
		case HOUSE_A:
			return(HOUSE_A_TOP_SPRITE);
		case HOUSE_B:
			return(HOUSE_B_TOP_SPRITE);
      case HOUSE_C:
         return(HOUSE_C_TOP_SPRITE);
      case BROKEN_ROCKET:
         return(BROKEN_ROCKET_SPRITE); // don't have rocket from above now
   };
	return(HOUSE_C_TOP_SPRITE);
};

SpriteCode door_sprite(HouseStyle style) {
	switch (style) {
		case HOUSE_A:
			return(DOOR_A_SPRITE);
		case HOUSE_B:
			return(DOOR_B_SPRITE);
      case BROKEN_ROCKET:
         return(BROKEN_ROCKET_DOOR_SPRITE);
	};
	return(DOOR_C_SPRITE);
};

BrushId roof_brush(HouseStyle style) {
	// could do arithmetic on BrushID and HouseStyle instead
	switch (style) {
		case HOUSE_A:
			return(ROOF_A_BRUSH);
		case HOUSE_B:
			return(ROOF_B_BRUSH);
      // what about BROKEN_ROCKET?? can't fly above it now...
	};
	return(ROOF_C_BRUSH);
};
	
void House::open_door() {
	if (door_status == DOOR_ABOUT_TO_EXPLODE) { // ignore this
		return;
	};
	if (door_appearance == NULL) {
		create_door();
		tt_screen->add(door_appearance);
	};
	door_appearance->set_closed(FALSE);
	// close in 5 seconds if programmer stops colliding with door region
	begin_closing_door_time = tt_current_time+5000;
	switch (door_status) {
		case DOOR_OPEN:
         door_appearance->set_sprite_type(OPEN_DOOR); // in case something went wrong -- set it again
			break;
		case DOOR_OPENING:
			if (door_changed_time != 0 && tt_current_time > door_changed_time) {
				door_changed_time = 0;
				door_status = DOOR_OPEN;
				door_appearance->set_sprite_type(OPEN_DOOR);
//			} else {
//				door_appearance->set_sprite_type(OPENING_DOOR);
			};
			break;
		case DOOR_CLOSED:
			door_status = DOOR_OPENING;
			door_appearance->set_parameter((short) door_status);
			door_changed_time
				// let the programmer go thru 1/4 second early
			  = tt_current_time+door_appearance->total_cycle_duration()-250;
			break;
		case DOOR_CLOSING:
			door_status = DOOR_OPEN;
			door_appearance->set_sprite_type(OPEN_DOOR);
			door_appearance->set_parameter((short) door_status);
			open_door(); // try again
			break;
	};
};

void House::initial_contents(Robot *robot, Cubby *cubby) {
//  if (room == NULL) { // first time so create it
//	  room = new Room(this);
//  };
  pointer_to_floor()->initial_contents(robot,cubby);
  if (is_built()) built();
};

Room *House::pointer_to_room() {
  if (room == NULL) { // first time so create it
	  room = new Room(this);
  };
  return(room);
};

Floor *House::pointer_to_floor() {
  if (room == NULL) { // first time so create it
	  room = new Room(this);
  };
  return(room->pointer_to_floor());
};

void House::close_door() {
	switch (door_status) {
		case DOOR_CLOSED:
		case DOOR_CLOSING:
			if (door_changed_time != 0 && tt_current_time > door_changed_time) {
				door_changed_time = 0;
				begin_closing_door_time = 0;
				door_status = DOOR_CLOSED;
				door_appearance->set_closed(TRUE);
				door_appearance->set_sprite_type(DOOR);
				tt_screen->remove(door_appearance); 
			};
			return;
		case DOOR_OPEN:
		case DOOR_OPENING: 
			door_status = DOOR_CLOSING;
			door_appearance->set_parameter((short) door_status);
			door_changed_time
			  = tt_current_time+door_appearance->total_cycle_duration();
			break;
//		case DOOR_OPENING: // a bit of problem so just close it instantly
//			door_status = DOOR_CLOSED;
//			door_appearance->set_parameter(door_status);
//			door_appearance->set_sprite_type(DOOR);
	};
};

Door::Door(city_coordinate center_x, city_coordinate center_y, House *house,
		  SpriteCode code) :
	  Sprite(code,center_x,center_y,DOOR,DOOR_OPEN),
	  closed(TRUE), // so DOOR_OPEN is ignored
	  house(house) {
//	set_size_porportional_to_screen_size(TRUE); // stretch if running greater than 640x480
};

void Door::open_door() {
	house->open_door();
};

//boolean House::is_door_closing() {
//   return(door_status == DOOR_CLOSING);
//};

Room *Door::pointer_to_room() {
	return(house->pointer_to_room());
};
	  
void Door::display(SelectionFeedback selection_feedback, 
						 boolean followers_too, TTRegion *region) {
	if (!closed) {  // could make door invisible when closed instead
		Sprite::display(selection_feedback,followers_too,region);
	};
};

Houses::~Houses() {
	if (house != NULL) {
		house->remove_from_screen(tt_screen); // in case it is being viewed
		tt_city->remove_house(house);
		delete house;
	};
	if (next != NULL) delete next;
};

HouseStyle house_style(SpriteCode code) {
	switch (code) {
		case HOUSE_A_TOP_SPRITE:
		case HOUSE_A_SIDE_SPRITE:
		  return(HOUSE_A);
		case HOUSE_B_TOP_SPRITE:
		case HOUSE_B_SIDE_SPRITE:
		  return(HOUSE_B);
		case HOUSE_C_TOP_SPRITE:
		case HOUSE_C_SIDE_SPRITE:
		  return(HOUSE_C);
      case BROKEN_ROCKET_SPRITE:
        return(BROKEN_ROCKET);
		default:
		  return(NOT_HOUSE);
	};
};

/*
  door_closed_time = 0;
  int percent_door_closed = door_appearance->percent_closed();
  if (percent_door_closed > 0) {
	  int new_percent_closed =  // 1 second to open
			 percent_door_closed-tt_millisecond_delta/10;
	  if (new_percent_closed < 0) new_percent_closed = 0;
	  door_appearance->set_percent_closed(new_percent_closed);
	  if (new_percent_closed < 25) { // at least 3/4 open
		  door_appearance->set_sprite_type(OPEN_DOOR);
*/

/*
	door_appearance->set_visible(TRUE);
	int new_percent_closed = door_appearance->percent_closed();
	if (new_percent_closed == 100) return; // already closed
	if (door_closed_time != 0) {
		// take time left to closing in milliseconds and divide by 100
		// so it takes 10 seconds to close
		new_percent_closed = 100-(door_closed_time-tt_current_time)/100;
		if (new_percent_closed > 100) {
			new_percent_closed = 100;
		};
		if (new_percent_closed > 25) {
         door_appearance->set_sprite_type(DOOR);
      };
      door_appearance->set_percent_closed(new_percent_closed);
   } else { // first time
		door_closed_time = tt_current_time+10000; // 10 seconds from now
	};
*/

/*
House_Sprite::House_Sprite(color the_exterior_color,
                                   color the_roof_color,
                                   city_coordinate house_width,
                                   city_coordinate house_height,
                                   city_coordinate x,
                                   city_coordinate y,
                                   city_coordinate collision_width,
                                   city_coordinate collision_height,
                                   House *house, // due to "this"
                                   Screen *screen) :
   Sprite(x,y,house_width,house_height,HOUSE,house,
          collision_width,collision_height),
   exterior_color(the_exterior_color),
   roof_color(the_roof_color) {
   pixel_width = house_width/screen->one_pixel_in_x_city_units();
   pixel_height = house_height/screen->one_pixel_in_y_city_units();
//   collision_region_width = collision_width;
//   collision_region_height = collision_height;
//   collider = (Collider *) house; // since it is a const
};

void House_Sprite::display(Screen *screen) {
   // should probably redo this in city coordinates some day
   screen->move_mark_to(llx,lly);
   int x_offset = fg_getxpos();
   int y_offset = fg_getypos();
   int center_x = x_offset+pixel_width/2;
   int center_y = y_offset-pixel_height/2;
   int min_x = x_offset;
//   if (min_x < 0) min_x = 0;
   int max_x = x_offset+pixel_width-1;
//   if (max_x >= tt_screen_width) {
//      max_x = tt_screen_width-1;
//   };
   int min_y = y_offset-pixel_height+1;
//   if (min_y < 0) min_y = 0;
	int max_y = y_offset;
//   if (max_y >= tt_screen_height) {
//      max_y = tt_screen_height-1;
//   };
   int roof_height = pixel_height/4;
   int roof_bottom_y = min_y+roof_height;
//   if (roof_bottom_y > max_y) {
//      roof_bottom_y = max_y;
//   };

   fg_setcolor(exterior_color);
   fg_clprect(min_x,max_x,roof_bottom_y,max_y);

   // multiply by 100 to avoid floats
   long int roof_left_x100 = 100L*x_offset;  // need to clip
   long int roof_right_x100 = 100L*(pixel_width+x_offset-1);
   int slope100 = pixel_width*50/roof_height;
   fg_setcolor(roof_color);
   int y = roof_bottom_y-1;
// /*
	if (y > tt_screen_height) { // just the top of the roof visible
      int gap = tt_screen_height-y-1;
		y = tt_screen_height-1;
      roof_left_x100 += gap*slope100;
      roof_right_x100 -= gap*slope100;
   };
// * /
   for (; y > min_y; y--) {
      if (y < 0) break; // no point
      roof_left_x100 += slope100;
      roof_right_x100 -= slope100;
      int roof_left_x = roof_left_x100/100;
      int roof_right_x = roof_right_x100/100;
      if (roof_left_x < 0) {
         roof_left_x = 0;
      };
      if (roof_right_x >= tt_screen_width) {
         roof_right_x = tt_screen_width-1;
      };
      if (roof_left_x < roof_right_x && y < tt_screen_height) {
			fg_move(roof_left_x,y);
         fg_draw(roof_right_x,y);
		};
   };
   int window_center_x = center_x-pixel_width/4,
       window_center_y = center_y+pixel_height/4;
   int half_window_width = pixel_width/12;
   int half_window_height = pixel_height/10;
   fg_setcolor(BRIGHT_WHITE); // window's color should depend upon state
   fg_clprect(window_center_x-half_window_width,
              window_center_x+half_window_width,
              window_center_y-half_window_height,
              window_center_y+half_window_height);
   window_center_x += pixel_width/2;
   fg_clprect(window_center_x-half_window_width,
              window_center_x+half_window_width,
              window_center_y-half_window_height,
              window_center_y+half_window_height);
};

void House_Sprite::ref_count_is_zero() {
  // should this do something?
};

Door_Sprite::Door_Sprite(color the_color,
                                 city_coordinate door_width,
                                 city_coordinate door_height,
                                 city_coordinate x,
                                 city_coordinate y,
                                 city_coordinate collision_width,
                                 city_coordinate collision_height,
                                 city_coordinate col_x_offset,
                                 city_coordinate col_y_offset,
                                 House *house,
                                 Screen *screen) :
   Sprite(x,y,house_width,house_height,DOOR,house,
          collision_width,collision_height,
          col_x_offset, col_y_offset),
   door_color(the_color),
   the_percent_closed(100) {
	pixel_width = door_width/screen->one_pixel_in_x_city_units();
   pixel_height = door_height/screen->one_pixel_in_y_city_units();
};

void Door_Sprite::display(Screen *screen) {
      screen->move_mark_to(llx,lly); // forget what this accomplishes
      int width_open = ((100-the_percent_closed)*pixel_width)/100;
      int width_closed = pixel_width-width_open;
      // grab lower left corner
      int door_way_min_x = fg_getxpos();
      int min_x = door_way_min_x+width_open;
      int max_y = fg_getypos();
      int door_way_max_x = door_way_min_x+pixel_width;
      int max_x = min_x+width_closed;
      int min_y = max_y-pixel_height;
      if (the_percent_closed != 100) { // then draw the doorway
         fg_setcolor(CLEAR);
         fg_clprect(door_way_min_x,door_way_max_x,min_y,max_y);
      };
      if (the_percent_closed == 0) return; // nothing more to do - fully open
		fg_setcolor(door_color);
      fg_clprect(min_x,max_x,min_y,max_y);

      const int knob_center_x = min_x+2*pixel_width/3;
      const int knob_center_y = max_y-pixel_height/3; // 1/3 up
      int door_knob_radius = pixel_width/6;
      fg_setcolor(BROWN);
      const int door_min_x = max(min_x,knob_center_x-door_knob_radius);
      const int door_max_x = min(max_x,knob_center_x+door_knob_radius);
      const int door_min_y = max(min_y,knob_center_y-door_knob_radius);
      const int door_max_y = min(max_y,knob_center_y+door_knob_radius);
      if (door_max_x > door_min_x && door_max_y > door_min_y) {
         fg_clprect(door_min_x, door_max_x, door_min_y, door_max_y);
      };
// /*
     if (knob_center_x > door_knob_radius &&
         knob_center_x < tt_screen_width-door_knob_radius &&
         knob_center_y > door_knob_radius &&
         knob_center_y < tt_screen_height-door_knob_radius &&
         knob_center_x+door_knob_radius < min_x+width_closed) {
		// will fit --- don't have an easy way to clip a circle
        fg_setcolor(WHITE);
		  fg_move(knob_center_x, knob_center_y);
        fg_circle(door_knob_radius);
        fg_setcolor(BLACK);
      fg_paint(knob_center_x, knob_center_y);
     };
// * /
};

void Door_Sprite::set_percent_closed(int percent) {
   if (percent != the_percent_closed) {
      the_percent_closed = percent;
      set_clean_status(DIRTY);
   };
};

void Door_Sprite::ref_count_is_zero() {
  // should this do something?
};
*/
