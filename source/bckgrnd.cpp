// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif   
#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif   
#if !defined(__TT_SPRITES_H)   
#include "sprites.h"
#endif   
#if !defined(__TT_SCREEN_H)
#include "screen.h"
#endif
#if !defined(__TT_WINMAIN_H)
#include "winmain.h"
#endif
#if !defined(__TT_PICTURE_H)   
#include "picture.h"
#endif
#if !defined(__TT_HELP_H)   
#include "help.h"
#endif   
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif 
#if !defined(__TT_CITY_H)   
#include "city.h"
#endif 
#if !defined(__TT_TOOLS_H)   
#include "tools.h"
#endif   
#if !defined(__TT_BCKGRND_H)   
#include "bckgrnd.h"
#endif
#if !defined(__TT_MARTIAN_H)   
#include "martian.h"
#endif 

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if TT_DEBUG_ON
long background_counter = 1;
#endif

#if !defined(__TT_PRGRMMR_H)   
#include "prgrmmr.h"
#endif

//Backgrounds::Backgrounds(Background *background, Backgrounds *backgrounds) : // new on 090804
//  first(background),
//  next(backgrounds) {
//};
//
//Backgrounds::~Backgrounds() {
//	delete first;
//	if (next != NULL) {
//		delete next;
//	};
//};

Background::Background() :
	items(NULL),
	screen_count(0),
	postponed_removals(NULL),
   ref_count(1), // changed to 1 from 0 on 230103
   deletion_pending_flag(FALSE) {
#if TT_DEBUG_ON
   debug_counter = background_counter++;
	if (-tt_debug_target == debug_counter) {
		log("Creating debug target background.");
	};
	prepared_for_deletion = FALSE;
#endif
};

#if TT_DEBUG_ON
Background::~Background() {
	if (!prepared_for_deletion) {
		say_error("Deleting a background without preparing it first.");
	};
};
#endif

void Background::prepare_for_deletion() {
#if TT_DEBUG_ON
	if (debug_counter == -tt_debug_target) {
		log("Preparing background debug target for deletion.");
	};
	if (prepared_for_deletion) { 
		say_error("Destroying a destroyed background.");
	};
	prepared_for_deletion = TRUE;
	if (tt_debug_mode == 90400) {
		return;
	};
#endif
	// save in case called recursively (e.g. pictures with backgrounds...)
	UnwindProtect<Background *> set(tt_deleting_background,this); // just used to catch errors
	if (items != NULL) {
		Sprites *remaining = items;
		while (remaining != NULL) {
			Sprite *sprite = remaining->first();
			if (!tt_resetting_or_destroying_city) sprite->set_background(NULL,TRUE); 
			// moved here so that anything that is a part of this that is not ok_to_delete will stop pointing to this
			// tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 but can cause memory errors while resetting during time travel
			if (!sprite->slated_for_deletion()) { // something else will destroy it
				sprite->destroy();
//            if (!sprite->prepared_for_deletion()) { // protected -- probably by ref_count
//               sprite->recursively_set_background(NULL);
//            };
//			} else {
//            sprite->recursively_set_background(NULL);
         };
			remaining->set_first(NULL);
			remaining = remaining->rest();
		};
		delete items;
		items = NULL; // new on 280803 so this can be reset
	};
};

void Background::destroy() {
#if TT_DEBUG_ON
	//if (ref_count > 1) { // no longer anomolous as 090804 since may still be deleted by bird
	//	tt_error_file() << "Destroying background whose ref count is " << ref_count << endl;
	//};
   if (tt_debug_mode == 1717) {
      tt_error_file() << "Background deleted. " << debug_counter << endl;
   };
	if (debug_counter == -tt_debug_target) {
		log("Destroying debug background.");
	};
	if (tt_debug_mode == 90400) {
		if (prepared_for_deletion) { 
			say_error("Destroying a destroyed background.");
		};
		prepared_for_deletion = TRUE;
		return;
	};
#endif
//	if (decrement_ref_count()) { // conditional new on 090804 -- not so good because circular references are common (bird's return floor)
		tt_screen->background_being_destroyed(this); // new on 200901
		// this way prepare_for_deletion's calls to virtuals works
		prepare_for_deletion();
//		delete this; // replaced by the following on 090804
//		tt_backgrounds_to_delete_next_frame = new Backgrounds(this,tt_backgrounds_to_delete_next_frame);
		if (decrement_ref_count()) {
			delete this;
		} else {
			deletion_pending_flag = TRUE; // new on 090804 -- not clear this is useful
		};
//	};
};

boolean Background::decrement_ref_count() {
   if (ref_count == 0) {
#if TT_DEBUG_ON
		if (!tt_resetting_or_destroying_city) {
			debug_this();
		};
#endif
      return(TRUE); // warn since shouldn't really happen?
   };
   ref_count--;
	if (ref_count == 0) {
		deletion_pending_flag = TRUE; // new on 220103 since this is better than the old ok_to_delete() scheme
		return(TRUE);
	} else {
		return(FALSE);
	};
};

void Background::add_items(Sprites *items, boolean give_priority, boolean change_priority, boolean warn) {
   Sprites *remaining = items;
   while (remaining != NULL) {
      add_item(remaining->first(),give_priority,change_priority,warn);
      remaining = remaining->rest();
   };
};

void Background::reset_items(Sprites *new_items) { // abstracted on 070203
	destroy_sprites(items); // new on 070103
	items = new_items;
	Sprites *remaining = items;
	while (remaining != NULL) {
		remaining->first()->Sprite::set_background(this,TRUE); // Sprite:: new on 190903 since things like Robot::set_background just do too much
		remaining = remaining->rest();
	};
};

void Background::just_add_items(Sprites *new_items, boolean add_to_screen_too) { // new on 240203
	if (items == NULL) {
		items = new_items;
	} else {
		Sprites *last = items->last_cons();
		last->set_rest(new_items);
	};
	Sprites *remaining = new_items;
	while (remaining != NULL) {
		Sprite *sprite = remaining->first();
		sprite->set_background(this,TRUE); // Sprite:: new on 190903 since things like Robot::set_background just do too much 
		// removed Sprite:: on 121703 since otherwise can have saved robots waiting on a nest not work as a team until reset
		if (add_to_screen_too) {
			tt_screen->add(sprite);
		};
		remaining = remaining->rest();
	};
};

void Background::add_copy_of_items(Sprites *items, boolean give_priority,
	                                boolean change_priority,
                                   boolean warn) {
   Sprites *remaining = items;
   while (remaining != NULL) {
      add_item(remaining->first()->copy(),give_priority,change_priority,warn);
      remaining = remaining->rest();
   };
};

boolean Background::add_item(Sprite *sprite, boolean , boolean , boolean warn) {
#if TT_CAREFUL
	if (sprite == NULL) {
		say_error(_T("ToonTalk confused and tried to add the NULL item to a background."));
		return(FALSE);
	};
   if (sprite->pointer_to_leader() != NULL && !about_to_quit()) {
      say_error(_T("Trying to add a part of something to a background."));
      return(FALSE);
   };
//	if (sprite->pointer_to_background() != NULL &&
//		 sprite->pointer_to_background() != this) {
//		if (tt_user_is_debugger) {
//			say("ToonTalk trying to put something on a background while it is on another.");
//		};
//		sprite->remove_from_floor(FALSE);
//	};
#endif
#if TT_DEBUG_ON
	if (prepared_for_deletion) { // typically when tt_debug_mode == 90400
		say_error("Using a destroyed background.");
	};
   if (sprite->debug_counter == tt_debug_target) {
		tt_error_file() << "Debug target being added to floor #" << debug_counter << endl;
	} else if ((sprite->prepared_for_deletion() || sprite->deleted()) && !about_to_quit()) {
		say_error("Trying to add a deleted (or prepared for deletion) item to a background",FALSE);
		return(FALSE);
	} else if (tt_debug_target == -debug_counter) {
		log("Adding item to debug target background.");
	};
#endif
	if (postponed_removals != NULL) {
		boolean postponed;
		postponed_removals = postponed_removals->remove(sprite,&postponed);
		if (postponed) {
			if (sprite->pointer_to_leader() == NULL) { 
				sprite->set_background(this,TRUE);
			};
			return(TRUE); // leave it there
		};
	};
	boolean ok;
	if (items == NULL) {
		items = new Sprites(sprite);
		ok = TRUE;
	} else {
		ok = items->insert_at_end_if_not_member(sprite);
#if TT_DEBUG_ON
		if (!ok && warn) { //  && tt_user_is_debugger
			say_error(IDS_ADD_TO_BACKGROUND_ALREADY_THERE);
		};
#endif
	};
	if (//ok && // somehow this is false but sprite doesn't know its background
		 sprite->pointer_to_leader() == NULL) {
		sprite->set_background(this,TRUE); // for some crazy reason substituted warn for TRUE on 110100 - restored on 200100
	};
	return(ok);
};

boolean Background::has_item(Sprite *sprite) {
#if TT_DEBUG_ON
	if (prepared_for_deletion) { // typically when tt_debug_mode == 90400
		say_error("Using a destroyed background.");
	};
#endif
	return(items != NULL && items->member_cons(sprite) != NULL &&
	       (postponed_removals == NULL || postponed_removals->member_cons(sprite) == NULL));
};

boolean Background::remove_item(Sprite *sprite, boolean report_problems, boolean , boolean reset_background) {
#if TT_DEBUG_ON
   if (sprite->debug_counter == tt_debug_target) {
		tt_error_file() << "Debug target being removed from floor #" << debug_counter << endl;
	};
	if (prepared_for_deletion) { // typically when tt_debug_mode == 90400
		say_error("Using a destroyed background.");
	};
#endif
	boolean found = FALSE;
	if (items != NULL) {
		items = items->remove(sprite,&found);
#if TT_DEBUG_ON
		if (report_problems && !found) { // && tt_user_is_debugger) {
			say_error(_T("Warning: ToonTalk just tried to remove something from a background it wasn't in."));
		};
	} else if (report_problems) { // && tt_user_is_debugger) {
		say_error(_T("Warning: ToonTalk just tried to remove something from an empty background.")); 
#endif
	};
	if (reset_background && //found && -- sometimes wrong...
		 sprite->pointer_to_leader() == NULL) {
		sprite->set_background(NULL,TRUE,report_problems);
	};
	return(found);
};

void Background::remove_all() {
#if TT_DEBUG_ON
	if (prepared_for_deletion) { // typically when tt_debug_mode == 90400
		say_error("Using a destroyed background.");
	};
#endif
	if (items != NULL) {
		if (tt_log_version_number >= 22) {
			Sprites *remaining = items;
			while (remaining != NULL) {
				if (!tt_resetting_or_destroying_city && remaining->first() != NULL) {
					// tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 
					// but can cause memory errors while resetting during time travel
	//				if (remaining->first()->deleted) { // conditional is a temporary hack on 070400
	//					log("debug this");
	//				} else {
						remaining->first()->set_background(NULL,TRUE);
	//				};
				};
				remaining->set_first(NULL);
				remaining = remaining->rest();
			};
			delete items;
			items = NULL;
		// replaced below with above on 060400 to set_background to NULL
		} else {
		  items->remove_all();
		  delete items;
		  items = NULL;
	  };
	};
};

void Background::set_initial_items(Sprites *list) {
   if (items != NULL) {
		items->stop_all(TRUE); // moved here on 260301 so the robots are stopped first - since they might for example put working cubby on the floor
		Sprites *remaining = items;
		while (remaining != NULL) {
			Sprite *sprite = remaining->first();
//			sprite->stop_all(TRUE); // new on 231000 since might be a running robot (e.g. from a previous puzzle)
			sprite->remove_from_floor(FALSE); // new on 211200
		   sprite->set_background(NULL,TRUE,FALSE); // added FALSE on 200400
			sprite->destroy(); // new on 240101
			remaining = remaining->rest();
		};
		just_destroy_list(items); // this replaces the following as of 240101 since list of items can be altered
 //     items->destroy_all(); // destroy sprites first
 //     delete items;
 //     items = NULL;
   };
//   items = list;
   Sprites *remaining = list;
   while (remaining != NULL) {
      add_item(remaining->first(),FALSE,TRUE); // this updates priorities correctly
      remaining = remaining->rest();
   };
};

#if TT_XML

void Background::top_level_xml(xml_document *document, xml_element *parent) { // abstracted on 040803 and moved from City
	xml_element *background_element = create_xml_element(xml_tag(),document);
	add_xml(background_element,document);
	xml_append_child(background_element,document);
	release_current_element_sprite_table(); // new on 071103 so if saving stuff on the floor twice in a row no references remain between them
//	return(background_element);
};

boolean Background::handle_xml(string file_name) { // new on 041102
#if TT_DEBUG_ON
      // for now - conditionalize later...
   DWORD start = timeGetTime();
#endif
   boolean result = handle_xml(document_from_file(file_name));
#if TT_DEBUG_ON
   tt_error_file() << "Took " << (timeGetTime()-start) << "ms to process " << file_name << endl;
#endif
	return(result);
};

boolean Background::handle_xml(xml_document *document) { // new interface as of 090103
   if (document != NULL) {
//    boolean result = xml_entity(document,this);
      xml_node *background_element = first_node_that_is_an_element(document);
		if (background_element == NULL) return(FALSE); // warn??
		if (tag_token(background_element) == NOTHING_TAG) { // shouldn't some of this move up to City?
			if (current_house_counter() == 0 && !any_trucks_outside()) { 
				// condition new on 120603 -- second condition new on 130205
				tt_city->build_initial_houses();
			};
			int initial_time = xml_get_attribute_int(background_element,L"Time",min_long);
			if (initial_time == min_long) { // should be true prior to 040805 when even "Nothing" had a few attributes
				return(TRUE); // new on 120103 - nothing there so nothing to do
			};
		};
      // need to do the following before loading everything -- for cities yes but really for floors too??
//		int current_frame_number = tt_frame_number;
      tt_initial_current_time = xml_get_attribute_int(background_element,L"Time",tt_current_time); 
		// was tt_initial_current_time prior to 080103 - restored 140703
      tt_frame_number = xml_get_attribute_int(background_element,L"FrameNumber",tt_frame_number);
		// commented out on 200304 since this gave the wrong frame number on replay
//		if (tt_current_log_segment == 1) { // on 240803 changed from tt_frame_number != current_frame_number -- 
		// initial value is 0 so not clear this accomplishes anything
//			tt_frame_number--; // since will be incremented before used - new on 230803
//		} else {
			tt_current_time = tt_initial_current_time; // new on 310803
//		};
		tt_still_frame_count += tt_frame_number; 
		// commented out the following on 090403 since cities should set the language
		// restored on 170105 -- but changed tt_language to tt_xml_language_index
		// removed (NaturalLanguage) on 091205 since obsolete
		tt_xml_language_index = xml_get_attribute_int(background_element,L"Language",tt_language);
#if TT_NO_ROBOT_TRAINING
		// new on 091205 -- use the language of the saved city -- if string library exists
		if (tt_xml_language_index < LANGUAGE_COUNT) {
			char library_file_name[MAX_PATH];
			strcpy(library_file_name,country_codes[tt_xml_language_index]);
			strcat(library_file_name,AC(IDC_STRING_DLL_SUFFIX));
			FileNameStatus name_status;
			string full_file_name = existing_file_name(name_status,library_file_name);
			if (full_file_name != NULL) {
				set_language((NaturalLanguage) tt_xml_language_index); 
				set_country_code(country_codes[tt_language],FALSE);
				unload_string_library();
				load_string_library(FALSE);
				delete [] full_file_name;
			};
		};
#endif
		tt_xml_version_number = xml_get_attribute_int(background_element,L"Version",tt_xml_version_number); // new on 180103
//    boolean result = xml_entity(background_element,this);
		set_xml_after_titles(document); // background_element); // new on 070103 - updated 120103
//    document->Release();
      return(TRUE);
   } else {
      return(FALSE);
   };
};

void Background::set_xml_after_titles(xml_document *node) {
	// by default load it now
	xml_entity(node,this);
};

void Background::add_xml_attributes(xml_element *element, xml_document *document) {
	// abstracted on 040805
   xml_set_attribute(element,L"Language",(int) tt_language);
	xml_set_attribute(element,L"Version",xml_version_number); // new on 180103
   xml_set_attribute(element,L"Time",tt_current_time); // rewritten on 161102
   xml_set_attribute(element,L"FrameNumber",tt_frame_number); // new on 161102
//#if TT_DEBUG_ON commented out 290505
	if (tt_saved_city_file_name_when_demo_ends == NULL) // new on 150505 so file compare (fc) doesn't complain about this difference
//#endif
		xml_set_attribute(element,L"CreatedUsing",C(IDS_TOONTALK_VERSION_NUMBER)); // new on 050903
};

void Background::add_xml(xml_element *element, xml_document *document) { // new on 171102
	add_xml_attributes(element,document);
	Sprites *all_items = items; // new on 030805
#if TT_CALL_FOR_MARTY
	if (tt_call_for_marty != NULL && tt_martian != NULL) { // new on 030805
		all_items = new Sprites(tt_martian,all_items);
	};
#endif
   if (all_items != NULL) { // what about postponed_removals??
      xml_element *inside_element = create_xml_element(L"Inside",document);
		if (tt_dump_reason == DUMPING_TO_CLIPBOARD) {
			xml_set_attribute(inside_element,L"TopLevel",1); // new on 110803
		};
      all_items->add_xml(inside_element,document,NULL,TRUE); // want geometry
		xml_append_child(inside_element,element); // moved here on 190804
		if (all_items != items) { // tt_martian added
			all_items->set_first(NULL);
			all_items->set_rest(NULL);
			delete all_items;
		};
	};
};

boolean Background::handle_xml_tag(Tag tag, xml_node *node) { // new on 161102
   switch (tag) {
		case INSIDE_TAG: {
			Sprites *sprites = xml_load_sprites(node);
#if TT_CALL_FOR_MARTY
			if (sprites != NULL && sprites->first() == tt_martian) {
				Sprites *remaining = sprites->rest();
				sprites->set_first(NULL);
				sprites->set_rest(NULL);
				delete sprites;
				sprites = remaining;
			};
#endif
			just_add_items(sprites); // rewritten on 100803 since this also sets background pointer of each sprite
			//if (items == NULL) {
			//	items = xml_load_sprites(node);
			//} else {
			//	Sprites *last = items->last_cons();
			//	last->set_rest(xml_load_sprites(node));
			//};
			return(TRUE); // this was missing prior to 100803!
							  };
			// on 240203 rewrote below as above so that items are combined rather than destroyed 
			// (e.g. a room gets a door and decoration and then more items)
   //   case INSIDE_TAG: {
			//destroy_sprites(items); // new on 070103
   //      items = xml_load_sprites(node);
   //      return(TRUE);
   //                    };
		case CITY_TAG:
			// added NOTHING_TAG on 040805
		case NOTHING_TAG:{ // e.g. from entire document being processed in process_xml_after_titles - new on 120103
			allocate_serial_numbers(node); // new on 190103
			UnwindProtect<boolean> set(tt_loading_a_city,TRUE); 
			// new on 170403 -- was ordinary assignment prior to 140105 but can be called recursively now
			UnwindProtect<boolean> save(tt_loading_is_ok_to_abort); // new on 200105
			if (!time_travel_enabled() && !replaying()) {
				tt_loading_is_ok_to_abort = TRUE;
			};
			boolean result = xml_entity(node,this);
//			tt_loading_a_city = FALSE;
			return(result);
							};
		case NO_MORE_TAGS:
			return(TRUE); 
      default:
			log("Unrecognized XML tag loading a background (e.g. a city)",FALSE,TRUE);
			tt_error_file() << (int) tag << " is the tag's value." << endl;
         return(TRUE);
   };
};

#endif

void Background::dump_to_clipboard() { // bytes buffer, long buffer_size) {
	if (tt_dump_as_xml) { // new on 040803 to dump floors, houses, and cities to the clipboard
		UnwindProtect<DumpReason> set(tt_dump_reason,DUMPING_TO_CLIPBOARD);
		if (on_the_floor() && !on_the_ground()) {
			tt_main_window->add_to_clipboard(xml_global_handle());
		} else {
			if (in_the_room()) {
				House *house = pointer_to_house();
				if (house == NULL) { // what??
					log("Room has no house!");
				} else {
					tt_main_window->add_to_clipboard(house->xml_global_handle());
				};
			} else {
				tt_main_window->add_to_clipboard(tt_city->xml_global_handle());
			};
		};
		return;
	};
   string suffix; // = "";
	SpriteType type;
#if TT_NEW_IO
	output_string_stream stream;
#else
	long buffer_size = max_encoding_buffer_size; // was max_background_dump_size prior to 170101
   string buffer = new char[buffer_size];
   output_string_stream stream(buffer,buffer_size);
#endif
   // even though dump of background takes care of background type token now
   // do this again for backwards compatibility starting with version 8
   if (on_the_floor() && !on_the_ground()) {
		suffix = "_floor";
      stream.put((char) WHOLE_FLOOR);
		type = WHOLE_FLOOR;
//   } else if (inside_a_house()) {
//      stream.put((char) WHOLE_HOUSE);
//		   if (top_level)
         write_cookie(stream);
			if (tt_dumping_background == NULL) tt_dumping_background = this; // doesn't :dump below do this fine now??
			dump_started();
			dump(stream,TRUE); // added on 150499
			dump(stream);
			dump_ended();
			tt_dumping_background = NULL;
	} else if (in_the_room()) { // new on 030200
		suffix = "_house";
		stream.put((char) WHOLE_HOUSE); // maybe should be called whole_house
		type = WHOLE_HOUSE;
		House *house = pointer_to_house();
		if (house == NULL) { // what??
			log("Room has no house!");
		} else {
			if (tt_dumping_background == NULL) tt_dumping_background = this; // doesn't :dump below do this fine now??
			dump_started();
			house->dump(stream,TRUE); // new on 170101 to prepare things first
			house->dump(stream,FALSE);
			dump_ended();
			tt_dumping_background = NULL;
		};
   } else { // on the ground
		suffix = "_city";
      stream.put((char) WHOLE_CITY);
		type = WHOLE_CITY;
		tt_city->dump_to_stream(stream); // not clear if this works since could be binary/ascii confusion - noted on 100603
   };
//   if (top_level) release_sprites_being_dumped();

//   if (top_level) {
//#if TT_32
//		HGLOBAL encoding_handle = uuencode(stream.STR,stream.LENGTH,TRUE);
//#else
		buffer_size = stream.pcount();
      HGLOBAL encoding_handle = uuencode(buffer,buffer_size,type);
      delete [] buffer;
//#endif
		tt_main_window->add_to_clipboard(encoding_handle,suffix);
//   };
};

void Background::dump(output_stream &stream, boolean just_prepare) {
   if (items != NULL) {
//      Sprites *sorted_items = sort_items(items); // commented out on 210100 since robots now save links to working cubby
      //flag saved_dumping_to_clipboard = tt_dumping_to_clipboard;
      //tt_dumping_to_clipboard = TRUE;
		UnwindProtect<DumpReason> set(tt_dump_reason,DUMPING_TO_CLIPBOARD); // restoration looks like it was missing prior to 080803
		UnwindProtect<Background *> save(tt_dumping_background);// is this useful?? won't it be NULL or unchanged
      if (tt_dumping_background == NULL) tt_dumping_background = this; // if dumping entire city don't reset this
		dump_started();
		if (tt_dumping_background == this) { // not part of a bigger dump (e.g. city)
			items->dump(stream,TRUE,just_prepare,FALSE,not_friend_of_tooly); // new on 010200
		} else {
			items->dump(stream,TRUE,just_prepare);
		};
      stream.put((unsigned char) END_OF_LIST_MARKER);
//      tt_dumping_to_clipboard = saved_dumping_to_clipboard;
//      sorted_items->remove_all();
//      delete sorted_items;
		dump_ended();
//      if (saved_tt_dumping_background == NULL) release_sprites_being_dumped(); // was a top level dump
   };
};

void Background::shift_all(city_coordinate delta_x, city_coordinate delta_y) {
   if (items != NULL) items->shift_all(delta_x,delta_y);
};

//#if TT_DEBUG_ON
//void Background::display_debug_info() {
//	tt_error_file() << "Visible_Background::update_display on frame " << tt_frame_number << " at time " << tt_current_time << endl;
//	Sprites *remaining = items;
//	while (remaining != NULL) {
//		remaining->first()->display_debug_info(tt_error_file(),1);
//		remaining = remaining->rest();
//	};
//};
//#endif

void Background::finish_instantly() { // new on 030903
	if (items != NULL) {
		tt_finish_all_animations = TRUE;
		items->finish_instantly();
	};
};

boolean Background::in_the_room_with_programmer() { // new on 020205
	return(in_the_room() && (tt_programmer->pointer_to_room() == pointer_to_room() || tt_log_version_number < 70));
};

//void Background::destroy(Sprite *sprite) {
//	// by default delete it next frame
//	tt_deleted_sprites = new Sprites(sprite,tt_deleted_sprites);
//};

Visible_Background::Visible_Background() :
	Background(),
//	screen(screen),
	leaders(NULL),
	updating_items(FALSE),
   clean(FALSE),
   current_priority(max_long-100), // to avoid conflict with stacks
   containment_active(FALSE) {
};

void Visible_Background::prepare_for_deletion() {
#if TT_DEBUG_ON
	if (-tt_debug_target == debug_counter) {
		log("Preparing to delete target background.");
	};
#endif
   if (postponed_removals != NULL) remove_postponed_removals();
//	Background *saved_tt_deleting_background = tt_deleting_background;
//	tt_deleting_background = this; // just used to catch errors
	if (leaders != NULL) {
		// why is this conditional???
		if (!tt_resetting_or_destroying_city) {
			// tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 but can cause memory errors while resetting during time travel
//			Sprites *remaining = leaders;
//			while (remaining != NULL) {
//				Sprites *saved_remaining = remaining->rest();
//				remaining->first()->no_floor(); // was set_background(NULL) but that triggered stuff
//				remaining = saved_remaining;
//			};
		} else {
			leaders->remove_all();
			delete leaders;
      };
		leaders = NULL; // new on 280803 so this can be reset
	};
   Background::prepare_for_deletion();
//   tt_deleting_background = saved_tt_deleting_background;
};

//  Background takes care of this now
//  if (items != NULL) {
//	  delete items;
//  };
//  if (leaders != NULL && tt_shutting_down) {
//	  leaders->remove_all();
//	  delete leaders;
//  };
//  if (tt_backgrounds_still_animating != NULL) {
//	  tt_backgrounds_still_animating->remove(this); // if there
//  };

void Visible_Background::now_on_screen() {
	if (items != NULL) {
		items->add_all_to_screen();
	};
};

void Visible_Background::give_item_priority(Sprite *sprite) {
	// changing priority unncessarily invalidates cache so check first
   if (tt_screen->camera_status() == CAMERA_IN_FRONT && !sprite->priority_fixed()) {
      sprite->update_priority();
	} else if (current_priority+1 != sprite->priority()) {
		sprite->set_priority(current_priority--);
	};
   if (current_priority < 1) current_priority = max_long-100; // so nothing gets on top of hand etc
};

void Visible_Background::set_current_priority_if_lower(long new_priority) {
	if (new_priority < 10000) {
		return; // new on 021199 since otherwise things float above the hand
	};
	if (new_priority < current_priority) {
		current_priority = new_priority;
	};
};

void Visible_Background::bring_to_top(Sprite *sprite) {
	Sprites *followers = sprite->pointer_to_followers();
	while (followers != NULL) {
		followers->first()->set_priority(current_priority--);
		followers = followers->rest();
	};
	sprite->set_priority(current_priority--);
   if (current_priority < 1) current_priority = max_long-100; // so nothing gets on top of hand etc
};

boolean Visible_Background::add_item(Sprite *sprite, boolean give_priority,
												 boolean change_priority, boolean warn) {
   if (!on_the_floor() && tt_screen->camera_status() == CAMERA_IN_FRONT && !sprite->priority_fixed()) {
		// first conjunct added on 150600 since puzzles are loaded while you are outside
		// but items belong on the floor
      sprite->update_priority();
   } else if (give_priority) { // give priority even if already added
		if (!sprite->graspable() || (sprite->infinite_stack() && sprite->pointer_to_leader() == NULL)) { 
			// new on 111204 so glued things are below even Tooly -- on 121204 added infinite stacks on the floor as well
			sprite->set_priority(max_long);
		} else {
			sprite->set_priority(current_priority--);
			if (current_priority < 1) current_priority = max_long-100; // so nothing gets on top of hand etc
		};
	} else if (change_priority && current_priority >= sprite->priority()) {
      current_priority = sprite->priority()-1;
      if (current_priority < 1) current_priority = max_long-100; // so nothing gets on top of hand etc
   };
	if (Background::add_item(sprite,give_priority,change_priority,warn)) {
// set_background should do the following
//		if (sprite->pointer_to_followers() != NULL) {
//			add_leader(sprite);
//		};
		if (showing_on_a_screen()) { // screen != NULL) {
			tt_screen->add(sprite);
//			sprite->add_outside_followers_to_screen(); // not needed since screen::add takes care of this
		};
      if (sprite->cached()) sprite->recursively_reset_cache_status();
//#if TT_DEBUG_ON
//      if (!showing_on_a_screen() && tt_screen->showing(sprite)) {
//         say("Somehow ToonTalk has something on the screen but it is on a background that isn't.",4);
//      };
//#endif
		return(TRUE);
	};
	return(FALSE);
};

boolean Visible_Background::remove_item(Sprite *sprite,
													 boolean report_problems,
													 boolean remove_from_screen,
													 boolean reset_background) {
	boolean removed;
	if (updating_items) { // postpone removal
		if (has_item(sprite)) {
#if TT_DEBUG_ON
			if (sprite->debug_counter == tt_debug_target) {
				log(_T("Postponing of remove_item of debug target."));
			};
#endif
			postponed_removals = new Sprites(sprite,postponed_removals);
			removed = TRUE;
		} else {
			removed = FALSE; // not there
		};
		// on 310399 moved the following from "has_item" branch above
		// can cause crashes when floor is destroyed before an item that was there (e.g. top pages of notebook)
		if (reset_background) {
			sprite->set_background(NULL,TRUE,report_problems);
		};
	} else {
		removed = Background::remove_item(sprite,report_problems,
		                                  remove_from_screen,reset_background);
	};
	if (remove_from_screen && showing_on_a_screen()) { // screen != NULL && 
		tt_screen->remove(sprite);
		if (!sprite->followers_completely_inside()) {
			Sprites *remaining = sprite->pointer_to_followers();
			while (remaining != NULL) {
				if (!remaining->first()->screen_updates()) {
					tt_screen->remove(remaining->first());
				};
				remaining = remaining->rest();
			}; 
		};
	};
//	if (removed && sprite->pointer_to_followers() != NULL) {
//		recursively_remove_leader(sprite);
//	};
	return(removed);
};

boolean Visible_Background::display_region() {
   return(FALSE);
}; // do nothing by default

void Visible_Background::remove_postponed_removals() {
   Sprites *saved_postponed_removals = postponed_removals;
	postponed_removals = NULL;
	Sprites *remaining = postponed_removals;
	while (remaining != NULL) { 
		Background::remove_item(remaining->first(),TRUE,FALSE,FALSE);
		remaining->set_first(NULL); // done with it
		remaining = remaining->rest();
	};
	delete saved_postponed_removals;
};

boolean Visible_Background::update_display(TTRegion *) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 50) {
		int item_count = 0;
		if (items != NULL) item_count = items->length();
		tt_error_file() << "Visible_Background has " << item_count << " items." << endl;
	};
#endif
  if (items != NULL) { // all may be in vacuum
	  if (postponed_removals != NULL) remove_postponed_removals();
	  updating_items = TRUE;
	  if (!items->update_display(TRUE,containment_region,containment_active)) {
		  // problem with first element
//		  items->first()->destroy(); // commented out since can be damaged so crashes deleting it
#if TT_DEBUG_ON
		  say_error(_T("Update display found a bad item and spliced it out."));
#endif
		  items = items->rest();
	  };
	  updating_items = FALSE;
	  if (postponed_removals != NULL) {
		  Sprites *remaining = postponed_removals;
		  Sprites *saved_postponed_removals = postponed_removals;
		  postponed_removals = NULL;
		  while (remaining != NULL) {
			  Background::remove_item(remaining->first(),FALSE,FALSE,FALSE); // maybe first arg should be TRUE and track down warnings
			  remaining->set_first(NULL); // done with it
			  remaining = remaining->rest();
		  };
		  delete saved_postponed_removals;
	  };
  };
  if (!clean) {
     clean = TRUE;
     return(FALSE);
  };
  return(TRUE);
};

void Visible_Background::remove_all() {
	if (showing_on_a_screen()) { // screen != NULL) { 
		Sprites *remaining = items;
		while (remaining != NULL) {
			tt_screen->remove(remaining->first());
//			remaining->set_first(NULL);
			remaining = remaining->rest();
		};
	};
	Background::remove_all();
//	delete items;
//	items = NULL;
	leaders = NULL;
};

void Visible_Background::recursively_add_leader(Sprite *item, boolean warn) {
	add_leader(item,warn);
	Sprites *remaining = item->pointer_to_followers();
	while (remaining != NULL) {
		Sprite *sprite = remaining->first();
		if (sprite->pointer_to_followers() != NULL) {
			recursively_add_leader(sprite,warn);
		};	
		remaining = remaining->rest();
	};
};

void Visible_Background::add_leader(Sprite *leader, boolean warn) {
#if TT_DEBUG_ON
	if (tt_debug_target == leader->debug_counter) {
		log(_T("Adding debug target as leader."));
	};
	if (warn && leader->pointer_to_leader() != NULL && leaders != NULL && leaders->member_cons(leader->pointer_to_leader()) == NULL) {
		log("Didn't know that the leader of a leader is a leader.",FALSE,TRUE);
		debug_this();
	};
#endif
	if (leaders == NULL) {
		leaders = new Leaders(leader,leaders);
	} else { // put at end because this might
				// be called while propagating changes
		leaders->insert_at_end_if_not_member(leader);
	};
};

void Visible_Background::recursively_remove_leader(Sprite *item, boolean warn) {
	remove_leader(item,warn);
	Sprites *remaining = item->pointer_to_followers();
	while (remaining != NULL) {
		Sprite *sprite = remaining->first();
		if (sprite->pointer_to_followers() != NULL) {
			recursively_remove_leader(sprite,warn);
		};	
		remaining = remaining->rest();
	};
};

void Visible_Background::remove_leader(Sprite *leader, boolean warn) {
#if TT_DEBUG_ON
	if (tt_debug_target == leader->debug_counter) {
		tt_error_file() << "Removing debug target as leader." << endl;
	};
#endif
	if (leaders != NULL) {
#if TT_DEBUG_ON
		boolean found;
		leaders = leaders->remove(leader,&found);
		if (warn && !found) {
//			tt_error_file() << "Removing leader from floor that didn't know it was" << endl;
			say_error(IDS_REMOVE_PART_BUT_NO_PARTS);   
		};
#else
		leaders = leaders->remove(leader);
#endif
	} else if (warn) { // && tt_user_is_debugger) {
//		tt_error_file() << "Removing leader from floor that didn't know it was" << endl;
		say_error(IDS_REMOVE_PART_BUT_NO_PARTS);
	};
};

void Visible_Background::propagate_changes() {
//	if (leaders != NULL) {
//		leaders->propagate_changes();
//	};
// re-written so no risk of stack overflow
	Leaders *remaining = leaders;
	while (remaining != NULL) {
      // save it in case following changes things
		Leaders *next_remaining = (Leaders *) remaining->rest();
		if (remaining->first()->pointer_to_background() == this) { 
			// condition new on 090105 since Logotron bug error 19.2 had an example 
			// where an earlier update removed this as well (time_travel2 segment 28)
			remaining->propagate_changes();
		};
		remaining = next_remaining;
	};
	remaining = leaders;
	while (remaining != NULL) {
		Sprite *sprite = remaining->first();
		// conditional added below on 070200 for robustness
		if (sprite != NULL) sprite->set_changes_propagated(FALSE); // until next round
		remaining = (Leaders *) remaining->rest();
	};
};

boolean Visible_Background::known_leader(Sprite *item) {
	if (leaders == NULL) return(FALSE);
	return(leaders->member_cons(item) != NULL);
};

millisecond Visible_Background::default_duration(millisecond the_default_duration) {
  if (tt_finish_all_animations) return(0);
  if (showing_on_a_screen()) {
     if (in_the_room()) return(the_default_duration/room_speedup);
     return(the_default_duration);
  };
  return(0);
};

#if TT_XML

void Visible_Background::add_xml_attributes(xml_element *element, xml_document *document) {
	// abstracted on 040805
	// current_priority?? -- asked 181102 -- and finally answered on 031103 - and containment_active needs to be saved too
//	xml_set_attribute(element,L"Z",current_priority); // makes more sense here but floor::xml takes care of this
//   flag clean : 1; // only used within a cycle
	if (!containment_active) { // normally is TRUE
		xml_set_attribute(element,L"ContainmentInactive",1);
	};
   xml_set_attribute(element,L"ContainmentRegionMinX",containment_region.min_x);
	xml_set_attribute(element,L"ContainmentRegionMaxX",containment_region.max_x);
	xml_set_attribute(element,L"ContainmentRegionMinY",containment_region.min_y);
	xml_set_attribute(element,L"ContainmentRegionMaxY",containment_region.max_y);
	Background::add_xml_attributes(element,document);
};

// commented out on 040805 since nothing left to do and the above does what it used to do
//void Visible_Background::add_xml(xml_element *element, xml_document *document) {
////	if (entity_type() != WHOLE_FLOOR) { // conditional new on 090404 since floor now calls this but doesn't want the following
//		Background::add_xml(element,document); // missing from 3.22 - 031103
////	};
//};

boolean Visible_Background::handle_xml_tag(Tag tag, xml_node *node) {
	if (tag == NO_MORE_TAGS) {
		containment_active = !xml_get_attribute_int(node,L"ContainmentInactive",0); // prior to 210404 was !containment_active
		//if (tt_log_version_number < 42 && !containment_active && ok_to_fix_containment_active()) { // shouldn't have been inactivated
		//	containment_active = TRUE; // new on 080404
		//};
		containment_region.min_x = xml_get_attribute_int(node,L"ContainmentRegionMinX",containment_region.min_x);
		containment_region.max_x = xml_get_attribute_int(node,L"ContainmentRegionMaxX",containment_region.max_x);
		containment_region.min_y = xml_get_attribute_int(node,L"ContainmentRegionMinY",containment_region.min_y);
		containment_region.max_y = xml_get_attribute_int(node,L"ContainmentRegionMaxY",containment_region.max_y);
	};
	return(Background::handle_xml_tag(tag,node));
};

#endif

//Room *Under_Sprite_Background::pointer_to_room() {
//	return(sprite_on_top->pointer_to_background()->pointer_to_room());
//};

//boolean Backside_Background::remove_item(Sprite *sprite, boolean report_problems,
//										           boolean remove_from_screen,
//										 			  boolean reset_background) {
//   if (picture != NULL) {
//      picture->remove_flipped_follower(sprite);
//   };
//   return (Background::remove_item(sprite,report_problems,remove_from_screen,reset_background));
//};

Room *Backside_Background::pointer_to_room() {
	Background *picture_background = picture->pointer_to_background();
	if (picture_background == NULL) { // e.g. picture might be in vacuum
		return(NULL);
	} else {
		return(picture_background->pointer_to_room());
	};
};

Titles_Background::Titles_Background(short int image_index) :
  Visible_Background(),
  text(NULL),
  text2(NULL),
  clean(TRUE),
  image_index(image_index),
  y_offset_percent(0),
  text_color(tt_black) {
};

void Titles_Background::prepare_for_deletion() {
	if (text != NULL) delete [] text;
   if (text2 != NULL) delete [] text2;
   Visible_Background::prepare_for_deletion();
};

boolean Titles_Background::display_region() {
	background(image_index)->display();
   if ((tt_win31j && image_index == THOUGHT_BUBBLE_BACKGROUND) || text == NULL) return(FALSE); // seems to fix a loading problem (WinG/Win3.1J)
	clean = TRUE; // until text is changed
   int text_length = _tcslen(text);
   city_coordinate character_width,character_height,ulx,uly;
   if (image_index == OPENING_TITLE_BACKGROUND) {
	   character_width = tile_width/2; // was (3*tile_width)/5;
	   character_height = (3*tile_height)/2;
	   uly = ideal_screen_height-tile_height/2;   // was 18*tile_height
		if (tt_language == BRITISH) { // new on 151204
			uly -= 17*tile_height/4; // just below the second title (intelligent modelling ...)
		};
      uly -= (y_offset_percent*ideal_screen_height)/100;
BEGIN_GDI
	   city_coordinate extent = tt_screen->get_extent(text,text_length,
                                                     character_width,character_height,TRUE,FALSE,FALSE);
      ulx = (ideal_screen_width-extent)/2;
      tt_screen->text_out(text,text_length,
							     ulx,uly,
							     character_width,character_height,TRUE);
      if (text2 != NULL) {
         text_length = _tcslen(text2);
         extent = tt_screen->get_extent(text2,text_length,
                                        character_width,character_height,TRUE,FALSE,FALSE);
         ulx = (ideal_screen_width-extent)/2;
         uly -= character_height;
         tt_screen->text_out(text2,text_length,
							        ulx,uly,
							        character_width,character_height,TRUE);
      };
END_GDI
   } else {
      character_width = (3*tile_width)/5; // was (2*tile_width)/3;
	   character_height = tile_height*2;
	   ulx = tile_width;
	   uly = 18*tile_height-(y_offset_percent*ideal_screen_height)/100;
	   city_coordinate text_width = 18*tile_width;
	   city_coordinate text_height = 16*tile_height;
BEGIN_GDI
      tt_screen->place_text(text,text_length,
								    ulx,uly,
								    text_width,text_height,
								    character_width,character_height,
								    OK_TO_BREAK_SENTENCES,FALSE,FALSE,0,0,tt_colors[text_color]);
END_GDI
   };
   return(FALSE);
};

/*
	for (int i = 0;;i++) {
		// assumes that it ends with a . or ! followed by 0 string terminator
		if (((text[i] == '.' || text[i] == '!') &&
			  (text[i+1] == ' ' || text[i+1] == 0 )) ||
          text[i] == '\r') {
			int length = i-offset+1;
         int length_to_print = length;
         if (text[i] == '\r') {
            length_to_print--;
         };
			tt_screen->place_text(text+offset,length_to_print,
										 ulx,uly,
										 text_width,text_height,
										 character_width,character_height,
									    TRUE,FALSE,0,0,text_color);
			i++;
			if (text[i] == 0) return(FALSE);
			if (text[i] == ' ') i++; // skip over blanks
			if (text[i] == ' ') i++;
			offset = i;
         // I can't make sense of the following
//			length = (3*length)/4; // most characters aren't full width
//			uly -= (1+((length*character_width)/text_width))*character_height;
         uly -= character_height;
		};
	};
*/

void Titles_Background::set_text(const_string new_text) {
	if (text != NULL) delete [] text;
	text = copy_string(new_text);
	clean = FALSE;
};

void Titles_Background::set_text2(const_string new_text) {
	if (text2 != NULL) delete [] text2;
	text2 = copy_string(new_text);
	clean = FALSE;
};

boolean Titles_Background::update_display(TTRegion *region) {
   if (!clean) {
      clean = TRUE;
      Visible_Background::update_display(region);
      return(FALSE);
   };
	return(Visible_Background::update_display(region));
};

boolean Backside_Background::remove_item(Sprite *sprite, boolean report_problems,
										           boolean remove_from_screen,
											        boolean reset_background) {
	if (picture != NULL && 
		(sprite->pointer_to_leader() == NULL ||
		 sprite->pointer_to_leader() == picture)) { // added possibility that sprite consider the picture as a leader on 090699
		// top level item on back of picture
		picture->remove_flipped_follower(sprite);
		report_problems = FALSE; // new on 190902 -- better to just return??
	};
//	// added on 1/9/99
//	Background *picture_background = sprite->pointer_to_background();
//	if (picture_background != NULL) {
//		Picture *subpicture = pointer_to_picture();
//		if (subpicture != NULL) subpicture->remove_flipped_follower(sprite);
//	};
	return(Background::remove_item(sprite,report_problems,remove_from_screen,reset_background));
};

/*
Visible_Backgrounds::~Visible_Backgrounds() {
  // only deletes the list not the members
  if (next != NULL) delete next;
};

void Visible_Backgrounds::remove(Visible_Background *removing) {
	if (background == removing) {
		background = NULL;
	} else if (next != NULL) {
		next->remove(removing);
	};
};
*/
//obsolete
/*
Visible_Backgrounds *Visible_Backgrounds::animate_though_offscreen() {
	if (background != NULL && background->animate_though_offscreen()) {
		if (next != NULL) {
			next = next->animate_though_offscreen();
		};
		return(this);
	} else { // splice this guy out since nothing left to animate
		Visible_Backgrounds *saved_next = next;
		next = NULL;
		delete this;
		if (saved_next == NULL) {
			return(NULL);
		} else {
			return(saved_next->animate_though_offscreen());
		};
	};
};
*/
