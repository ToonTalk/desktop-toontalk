// Copyright (c) 1992-2003.  Ken Kahn, Animated Programs, All rights reserved.   

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif   
#if !defined(__TT_BCKGRND_H)   
#include "bckgrnd.h"
#endif   
#if !defined(__TT_FLOOR_H)   
#include "floor.h"
#endif   
#if !defined(__TT_PRGRMMR_H)   
#include "prgrmmr.h"
#endif   
#if !defined(__TT_HOUSE_H)
#include "house.h"
#endif
#if !defined(__TT_TEXT_H)
#include "text.h"
#endif
#if TT_WINDOWS
#if !defined(__TT_WINMAIN_H)
#include "winmain.h"
#endif   
#endif
#if !defined(__TT_SCREEN_H)   
#include "screen.h"
#endif   
#if !defined(__TT_ROOM_H)
#include "room.h"
#endif
#if !defined(__TT_SPRITE_H)
#include "sprite.h"
#endif
#if !defined(__TT_UTILS_H)
#include "utils.h"
#endif
#if !defined(__TT_PAD_H)
#include "pad.h"
#endif
#if !defined(__TT_PICTURE_H)
#include "picture.h"
#endif
#if !defined(__TT_ROBOT_H)   
#include "robot.h"
#endif
#if !defined(__TT_CUBBY_H)
#include "cubby.h"
#endif
#if !defined(__TT_HELP_H)
#include "help.h"
#endif
#if !defined(__TT_PUZZLE_H)
#include "puzzle.h"
#endif

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Room::Room(House *house) :
  Visible_Background(),
  house(house),
  programmer(NULL),
  floor(NULL), // created on demand
  about_to_be_destroyed(FALSE),
  explosion_time(0),
  wall_decoration(NULL),
  frame_when_last_displayed(-1) {
//  initial_cubby(NULL) {
    if (house != NULL && house->house_style() != BROKEN_ROCKET) {
       // add the door so if the wall is decorated the door is in front of the decorations...
       Sprite *door = new Picture(room_door_llx,room_door_lly,0,0,ROOM_DOOR_SPRITE);
//		 door->set_size_porportional_to_screen_size(TRUE);
       door->set_containable_by_background(FALSE);
		 door->set_move_between_floor_and_room(FALSE);
       door->set_ok_to_dump(FALSE);
		 door->set_show_all(FALSE);
		 door->update_display(); // new on 150704
		 door->move_to(room_door_llx,room_door_lly); // new on 150704
       add_item(door);
    };
};

void Room::prepare_for_deletion() {
	if (floor != NULL) {
      if (floor->ok_to_delete() || tt_shutting_down) { // decrement_ref_count() was ok_to_delete prior to 220103 -- restored 240804
//		   delete floor;
         floor->destroy();
		} else {
			floor->set_room(NULL); // forget me since I'm a goner -- new on 121003
//#if TT_DEBUG_ON
//      } else if (tt_debug_mode == 1717) {
//         tt_error_file() << "Floor not deleted. " << debug_counter << endl;
//#endif
      };
	};
//   if (wall_decoration != NULL) {
//      // they are all also items taken care of by Visible_Background
//      wall_decoration->remove_all();
//      delete wall_decoration;
//   };
   Visible_Background::prepare_for_deletion();
};

boolean Room::display_region() {
//   if (wall_decoration != NULL) {
//      tt_destination_llx = wall_x;
//      tt_destination_lly = wall_y;
//      tt_destination_urx = wall_x+wall_width;
//      tt_destination_ury = wall_y+wall_height;
//      tt_main_window->set_clip_region();
//   };
   BackgroundImage *room_background = NULL;
	switch (house->house_style()) {
		case HOUSE_A:
		  room_background = background(ROOM_A_BACKGROUND);
		  break;
		case HOUSE_B:
		  room_background = background(ROOM_B_BACKGROUND);
		  break;
		case HOUSE_C:
		  room_background = background(ROOM_C_BACKGROUND);
		  break;
      case BROKEN_ROCKET:
        room_background = background(ROCKET_INSIDES_BACKGROUND);
        break;
	};
   if (room_background != NULL) {
		if (wall_decoration != NULL) { // new on 290500
			if (frame_when_last_displayed+1 < tt_frame_number) {
				wall_decoration->activate();
			};
			frame_when_last_displayed = tt_frame_number;
		};
      room_background->display();
/*      
      if (wall_decoration != NULL) {
         boolean saved_postpone_dirty_displays = tt_postpone_dirty_displays;
         tt_postpone_dirty_displays = FALSE;
         Sprites *remaining = wall_decoration;
         while (remaining != NULL) {
            Sprite *sprite = remaining->first();
            if (sprite->kind_of() == TEXT) {
               // looks much better this way
               ((Text *) sprite)->display_text();
            } else {
               sprite->recursively_notice_on_screen_change(TRUE);
               sprite->display(NO_FEEDBACK,TRUE);
            };
            remaining = remaining->rest();
         };
         tt_postpone_dirty_displays = saved_postpone_dirty_displays;
      };
*/
   };
   return(FALSE); // won't change next frame
};

void Room::initialize_wall_decoration() {
	Picture *decoration = new Picture(0,0,
			                            0,SYNTHETIC_BUT_INVISIBLE*256,
                                     SYNTHETIC_SPRITE,
                                     ideal_screen_width,ideal_screen_height);
	decoration->set_show_all(FALSE);
   set_wall_decoration_sprite(decoration);
};

void Room::set_wall_decoration_sprite(Sprite *wall_sprite) {
   if (wall_decoration != NULL) {
      remove_item(wall_decoration);
      wall_decoration->destroy();
   };
   wall_decoration = wall_sprite;
   if (wall_decoration == NULL) return;
   if (house != pointer_to_rocket()) { // clip
      wall_decoration->set_size_and_location(wall_width,wall_height,wall_x,wall_y);
   };
   wall_decoration->set_show_all(FALSE); // just the stuff on top
   wall_decoration->set_containable_by_background(FALSE);
	wall_decoration->set_move_between_floor_and_room(FALSE);
   wall_decoration->set_priority_fixed(TRUE);
   wall_decoration->set_priority(max_long); // behind everything else
   wall_decoration->activate(); // for followers that may have robots on the back
   add_item(wall_decoration);
};

void Room::set_wall_decoration(Sprites *sprites, boolean give_priority, boolean subpicture) {
   initialize_wall_decoration();
   if (sprites == NULL) {
      wall_decoration->set_visible(FALSE);
   } else {
//      wall_decoration->set_visible(TRUE);
      Sprites *remaining = sprites;
      while (remaining != NULL) {
         add_decoration(remaining->first(),give_priority,subpicture);
//         wall_decoration->add_follower(sprite,TRUE,FALSE);  // is completely inside but want insertion by priority
//         sprite->recursively_activate_pictures();
         remaining->set_first(NULL); // since about to destroy list
         remaining = remaining->rest();
      };
      delete sprites; // just the list not the contents
   };
};

void Room::add_decoration(Sprite *sprite, boolean give_priority, boolean subpicture) {
   // subpicture makes it so everything is clipped to the leader
   // but it also removes the pad from numbers and text
   if (sprite == NULL) return;
   if (wall_decoration == NULL) initialize_wall_decoration();
   wall_decoration->set_visible(TRUE);
   Picture *picture;
   if (sprite->kind_of() == PICTURE) {
      picture = (Picture *) sprite;
   } else {
      picture = make_indirect_picture(sprite,FALSE); // relies upon this really making sprite be the indirection 
		sprite->destroy(); // new on 240101 really decrement ref count increased by set_indirection called by make_indirect_picture
   };
   picture->set_subpicture(subpicture);
   if (give_priority) {
//		give_item_priority(sprite); // this might call update_priority which might set the priority to lly
		picture->set_priority(current_priority--); // was sprite->
	};
	boolean saved_show_all = picture->show_all();
   wall_decoration->add_follower(picture,TRUE,INSERT_BY_PRIORITY);
	picture->set_show_all(saved_show_all); // new on 230902 so see pads (unless changed prior to this)
   picture->set_held_status(NOT_HELD); // why??
   picture->recursively_activate_pictures(); // was activate();
   picture->set_containable_by_background(FALSE);
	picture->set_move_between_floor_and_room(FALSE);
   picture->set_priority_fixed(TRUE);
};

#if TT_XML

xml_element *Room::xml(xml_document *document) { // new on 011102
   xml_element *room_element = create_xml_element(L"Room",document);
   if (wall_decoration != NULL) {
      xml_element *decoration_element = create_xml_element(L"OnWall",document);
      wall_decoration->xml_create_and_append_element(decoration_element,document)->Release();
		xml_append_child(decoration_element,room_element); // moved here on 190804
      // don't need to restore this since it'll be dumped here but not as an item below
		wall_decoration->set_ok_to_dump(FALSE); // so not dumped as an item as well
      // i.e. not when items are dumped below - note this really means don't dump if part of a list - will still be dumped as decoration from here
	};
   if (items != NULL && !(items->rest() == NULL && !items->first()->ok_to_dump())) {
      xml_element *inside_element = create_xml_element(L"Inside",document);    
      items->add_xml(inside_element,document,NULL,TRUE);
		xml_append_child(inside_element,room_element); // moved here on 190804
   };
	if (explosion_time != 0) { // new on 030204
		xml_set_attribute(room_element,L"ExplosionTime",explosion_time);
	};
   return(room_element);
};

boolean Room::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
      case INSIDE_TAG:
			// following called reset_items prior to 240203
			just_add_items(xml_load_sprites(node)); // rewritten on 070203 - below should not have activated since that is redundant
        // Sprites *room_items = xml_load_sprites(node);
        // if (room_items != NULL) {
		      //add_items(room_items);
		      //room_items->recursively_propagate_changes();
        //    room_items->activate();
        //    room_items->remove_all();
        //    delete room_items;
        // };
         return(TRUE);
      case ON_WALL_TAG: {
         Sprite *saved_wall_decoration = xml_get_sprite(node);
         if (saved_wall_decoration != NULL) {
		      saved_wall_decoration->recursively_propagate_changes(); // new on 160601 to get offsets right
		      set_wall_decoration_sprite(saved_wall_decoration);
         };;
         return(TRUE);
                        };
      case NO_MORE_TAGS:
			explosion_time = xml_get_attribute_int(node,L"ExplosionTime",0); // prior to 210404 was explosion_time // new on 030204
         return(TRUE); 
      default:
         return(FALSE); // extra_xml?? 051102
   };
};

xml_element *Room::xml_path(xml_document *document) { // new on 050103
	if (block_im_on() == NULL) {
		return(NULL); 
	};
	return(pointer_to_house()->xml_path(document));
};

#endif

void Room::dump(output_stream &stream, boolean just_prepare) {
   if (just_prepare) {
      if (wall_decoration != NULL) wall_decoration->dump(stream,just_prepare);
      if (items != NULL) {
			items->dump(stream,FALSE,just_prepare);
		};
      return;
   };
   stream.put(ROOM_MARKER);
   if (wall_decoration != NULL) {
//		wall_decoration->propagate_changes(); // new on 160601 to get offsets right
      wall_decoration->dump(stream);
		// rewrote the above on 090400 to get the geometry to save correctly
//		Sprite *wall_decoration_copy = wall_decoration->top_level_copy();
//		Sprites *remaining = wall_decoration_copy->pointer_to_followers();
//		while (remaining != NULL) {
//			Sprite *sprite = remaining->first();
//			sprite->move_by(-sprite->return_old_x_offset(),-sprite->return_old_y_offset());
//			remaining = remaining->rest();
//		};
//		wall_decoration_copy->dump(stream);
//		wall_decoration_copy->destroy();
		wall_decoration->set_ok_to_dump(FALSE); // i.e. not when items are dumped below - note this really means don't dump if part of a list - will still be dumped as decoration from here
   } else {
      stream.put(NOTHING_MARKER);
   };
//   Sprites *sorted_items = sort_items(items); // commented out on 210100 since robots now save links to working cubby
   if (items != NULL) {
		items->dump(stream,TRUE);
   };
//   if (sorted_items != NULL) {
//      sorted_items->dump(stream,TRUE);
//      sorted_items->remove_all();
//      delete sorted_items;
//   };
   stream.put(END_OF_LIST_MARKER);
};

void Room::load(InputStream *stream, int notebook_version, NaturalLanguage language) {
   if (stream->get() != ROOM_MARKER) {
		log(S(IDS_BAD_ROOM_IN_CITY_FILE),FALSE,TRUE);
      return;
   };
   Sprite *saved_wall_decoration = load_item(stream,notebook_version,language);
   if (saved_wall_decoration != NULL) {
		saved_wall_decoration->recursively_propagate_changes(); // new on 160601 to get offsets right
		set_wall_decoration_sprite(saved_wall_decoration);
//		Sprites *remaining = saved_wall_decoration->pointer_to_followers();
//		while (remaining != NULL) {
//			Sprite *sprite = remaining->first();
//			position_and_add_to_wall(Sprite *wall_remote, Sprite *addition, Room *room)
	};
   Sprites *floor_items = load_items(stream,notebook_version,language);
   if (floor_items != NULL) {
      add_items(floor_items);
		floor_items->recursively_propagate_changes();
      floor_items->activate();
      floor_items->remove_all();
      delete floor_items;
   };
};

/*
void Room::add_decoration(Sprite *sprite, boolean give_priority) {
   wall_decoration = new Sprites(sprite,wall_decoration);
   sprite->set_containable_by_background(FALSE);
   sprite->set_priority_fixed(TRUE); // not influenced by Y as are things on the floor
   add_item(sprite,give_priority);
   sprite->set_held_status(NOT_HELD);
   sprite->recursively_activate_pictures(); // was activate();
};
*/

/*
   if (wall_decoration != NULL) {
      Sprites *remaining = wall_decoration;
      while (remaining != NULL) {
         Sprite *old_sprite = remaining->first();
         if (old_sprite != NULL) {
            old_sprite->remove_from_floor();
            tt_screen->remove(old_sprite);
         };
         remaining = remaining->rest();
      };
      wall_decoration->destroy_all();
      delete wall_decoration;
   };
   wall_decoration = sprites;
   Sprites *remaining = wall_decoration;
   while (remaining != NULL) {
      Sprite *sprite = remaining->first();
      sprite->set_containable_by_background(FALSE); // put it where I say!
      sprite->set_priority_fixed(TRUE); // not like stuff on the floor
      add_item(sprite);
//      give_item_priority(sprite); // since the above may be confused by current state of the "camera"
      remaining = remaining->rest();
   };
*/

/*
void Room::set_wall_decoration(Sprites *sprites) {
   just_set_wall_decoration(sprites);
   Sprite *first_decoration = sprites->first();
   if (first_decoration->kind_of() == TEXT) {
      first_decoration->set_show_all(FALSE);
      first_decoration->set_size((2*ideal_screen_width)/3,ideal_screen_height/2);
      ((Text *) first_decoration)->set_display_just_nice_text(TRUE);
   };
   city_coordinate decoration_width = first_decoration->current_width();
   city_coordinate decoration_height = first_decoration->current_height();
   city_coordinate wall_bottom = 8*tile_height;
   first_decoration->move_to(tile_width+(ideal_screen_width-decoration_width)/2,
                             wall_bottom+((ideal_screen_height-wall_bottom)-decoration_height)/2);
//   wall_decoration->really_propagate_changes();
};
*/

void Room::enter() {
//	screen->add(chair);
//   screen->add(floor_from_side);
//   tt_global_picture->update_room_decoration();
};

//void Room::initial_contents(Robot *robot, Cubby *cubby) {
	// could put cubby on the wall later
//	if (floor == NULL) create_floor();
//	floor->initial_contents(robot,cubby);
//};

Floor *Room::create_floor(Cubby *cubby) {
   short int floor_id;
   switch (house->house_style()) {
      case HOUSE_A:
         floor_id = FLOOR1_BACKGROUND;
         break;
      case HOUSE_B:
         floor_id = FLOOR2_BACKGROUND;
         break;
      default: // really case HOUSE_C:
         floor_id = FLOOR3_BACKGROUND;
         break;
   };
   floor = new Floor(floor_id,this,cubby);
//	floor->increment_ref_count(); // new on 220103 - experiment to comment out on 090203 since is a cycle (floor and room refer to each other)
   return(floor);
};

void Room::house_will_explode(millisecond duration) {
	explosion_time = tt_current_time+duration;
};

void Room::destroy_house() {
	about_to_be_destroyed = TRUE;
	house->blow_up();
//	if (programmer != NULL) {
//		programmer->house_about_to_be_destroyed();
//	};
};

//void Room::add_floor(Floor *bench) {
//   floor = bench;
//};

Floor *Room::pointer_to_floor() {
	if (floor == NULL) create_floor();
//   floor->about_to_redisplay();
	return(floor);
};

Block *Room::block_im_on() {
  return(house->block_im_on());
};

/*
void Room::move_chair(boolean pull_out, millisecond duration) {
	int direction = pull_out ? -1 : 1;
	chair->animate_delta_location(direction*tile_width/2,
											direction*2*tile_height,
											duration,NULL);
};
*/
