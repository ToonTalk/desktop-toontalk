// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)
#include "defs.h"
#endif
#if !defined(__TT_GLOBALS_H)
#include "globals.h"
#endif
#if !defined(__TT_SCREEN_H)
#include "screen.h"
#endif   
#if !defined(__TT_BLOCK_H)   
#include "block.h"
#endif   
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif
#if !defined(__TT_PAD_H)
#include "pad.h"
#endif
#if !defined(__TT_PUZZLE_H)
#include "puzzle.h"
#endif
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif
#if !defined(__TT_HELP_H)   
#include "help.h"
#endif
#if WING||TT_DIRECTX
#if !defined(__TT_WINMAIN_H)
#include "winmain.h"
#endif   
#endif
#if !defined(__TT_HOUSE_H)
#include "house.h"
#endif
#if !defined(__TT_BCKGRND_H)
#include "bckgrnd.h"
#endif
#if !defined(__TT_FLOOR_H)
#include "floor.h"
#endif
#if !defined(__TT_PICTURE_H)
#include "picture.h"
#endif
#if !defined(__TT_PRGRMMR_H)
#include "prgrmmr.h"
#endif
#if !defined(__TT_BIRD_H)
#include "bird.h"
#endif
#if !defined(__TT_CLEANUP_H)
#include "cleanup.h"
#endif
#if !defined(__TT_LOG_H)
#include "log.h"
#endif
#if !defined(__TT_CITY_H)
#include "city.h"
#endif
#if !defined(__TT_FILE_H)
#include "file.h"
#endif

//#include <strstrea.h> commented out 210703

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

string file_name_of_last_city_loaded = NULL; // new on 191201 so that demos copy the old city bytes rather than dump again (since that isn't stable when DumpCopiesCompactly=1

City::City() :
  Visible_Background(),
  changed(FALSE),
  stopped_flag(FALSE), // older tt_stop_all started as TRUE but that now makes the titles dark
#if TT_XML
  xml_after_titles(NULL),
#endif
  city_min_x(0),
  city_min_y(0) {
	tt_resetting_or_destroying_city = FALSE; // new on 100603 - just in case
   if (tt_system_mode == PUZZLE) {
#if TT_LARGE_CITIES
		tt_city_width = 3;
		tt_city_height = 3;
#else
      tt_city_size = 3; // small enough that it's hard to get lost exploring
#endif
   };
#if TT_LARGE_CITIES
   if (tt_city_width == 0) {
      tt_city_width++;
   };
   if (tt_city_height == 0) {
      tt_city_height++;
   };
#else
   if (tt_city_size == 0) {
      tt_city_size++;
   };
#endif
   initialize();
   tt_screen->initialize_background(this); // assumes only one city for now
//   if (tt_running_old_demos)
//   build_initial_houses(); // called by winmain now
};

City::~City() {
	tt_resetting_or_destroying_city = TRUE; // new on 100603
#if TT_XML
	if (xml_after_titles != NULL) {
		xml_after_titles->Release();
	};
#endif
};

void City::reset_all_blocks(boolean reset_toontalk_too) { // abstracted on 140103
//	tt_shutting_down = TRUE; // pretend for the following to be thourough
	// rewritten on 170403 so other code isn't fooled into thinking we are shutting down
#if TT_DEBUG_ON
	tt_debug_counter_at_last_reset = tt_counter_for_debugging;
#endif
	tt_resetting_or_destroying_city = TRUE; // new on 100603 -- needed for time travel to avoid spurious warnings
	if (reset_toontalk_too) reset_toontalk(); // arg and conditional new on 080703 since reset will lose some toontalk.ini options
	all_blocks.prepare_for_deletion(); // really just re-initialize
	prepare_for_deletion(); // to deal with other items (e.g. stuff on the ground) - new on 280803
#if TT_DEBUG_ON
	prepared_for_deletion = FALSE; // since re-using it - new on 280803
#endif
//	reset_house_counter(); // new on 220204
	tt_resetting_or_destroying_city = FALSE;
	if (reset_toontalk_too) {
		recreate_after_reset(); // do this after restoring tt_resetting_or_destroying_city -- should UnwindProtect be used? -- 131003
	};
//	tt_shutting_down = FALSE;
};

void City::initialize(boolean reset_toontalk_too) {
#if TT_LARGE_CITIES
	number_of_x_streets = abs(tt_city_width);
   number_of_y_streets = abs(tt_city_height);
#else
   if (tt_city_size < 0) {
      number_of_x_streets = (block_number) -tt_city_size;
      number_of_y_streets = (block_number) -tt_city_size;
   } else {
      number_of_x_streets = (block_number) tt_city_size;
      number_of_y_streets = (block_number) tt_city_size;
   };
#endif
   city_max_x = (city_coordinate) number_of_x_streets*tt_ideal_block_width;
   city_max_y = (city_coordinate) number_of_y_streets*ideal_screen_width;
	reset_all_blocks(reset_toontalk_too);
   all_blocks.set_region(0,number_of_x_streets,0,number_of_y_streets);
	// new on 080699 for containment at the city level
	set_containment();
	set_rocket(NULL); // new on 161199 since at end of demo might need to save puzzle state
	set_puzzle_house(NULL); // new on 161199
	remove_all(); // new on 120100 so when replaying things down't start duplicating on the floor
};

void City::set_containment() {
	TTRegion region(city_min_x,city_max_x,city_min_y,city_max_y);
   set_containment_region(region);
	set_containment_active(TRUE);
};

void City::build_initial_houses() {
#if TT_LARGE_CITIES
	boolean fill_with_random_houses = (tt_city_width < 0 || tt_city_height < 0);
#else
   boolean fill_with_random_houses = (tt_city_size < 0);
#endif
   int number_of_houses = 3;
   if (tt_system_mode == PUZZLE) {
      number_of_houses = 2; // 1 really but the other stands for the rocket for now
   } else if (fill_with_random_houses) { // start it out half full
      number_of_houses = number_of_x_streets*number_of_y_streets*tt_houses_to_a_block/2;
   };
   short int block_x = (short int) (number_of_x_streets/2);
   short int block_y = (short int) (number_of_y_streets/2);
   for (int i = 0; i < number_of_houses; i++) {
      if (fill_with_random_houses) {
         // construct random houses for testing
         block_x += (short int) (my_random(number_of_x_streets)+my_random(number_of_x_streets)
                     -my_random(number_of_x_streets)-my_random(number_of_x_streets));
         block_y += (short int) (my_random(number_of_y_streets)+my_random(number_of_y_streets)
                     -my_random(number_of_y_streets)-my_random(number_of_y_streets));
         if (block_x < 0 || block_x >= number_of_x_streets) {
            block_x = (short int) my_random(number_of_x_streets);
         };
         if (block_y < 0 || block_y >= number_of_y_streets) {
            block_y = (short int) my_random(number_of_y_streets);
         };
      };
      Block *block = all_blocks.block_at((block_number) block_x,(block_number) block_y,TRUE);
      city_coordinate x,y;
      short int index;
      while (!block->next_free_lot(x,y,index)) { // while full
			// new on 310399
			block_number next_x = (block_number) block_x;
			block_number next_y = (block_number) block_y;
         if (nearest_free_lot(next_x,next_y,index)) {
				block = all_blocks.block_at(next_x,next_y,TRUE);
			} else return; //
      };
         House *new_house;
         if (tt_system_mode == PUZZLE && i == 1) {
            new_house = new House(x-tt_ideal_block_width/22,y,block,BROKEN_ROCKET);
         } else {
            new_house = new House(x,y,block,(HouseStyle) (i%3));
         };
         block->add_house(new_house,index);
         if (tt_system_mode == PUZZLE) {
            if (i == 0) {
               if (load_puzzle(new_house) == NO_PUZZLE_FILE) {
                  tt_exit_failure(TROUBLE_SHOOT_GENERAL_TROUBLE,S(IDS_BAD_PUZZLE_FILE));
               };
            } else if (i == 1) {
               set_rocket(new_house);
            };
         };
   };
};

boolean City::update_display(TTRegion *new_view_region) {
   // might be better to overload operators for this?
   boolean clean = TRUE;
   if (new_view_region != NULL) {
       clean = (view_region.min_x == new_view_region->min_x &&
                view_region.max_x == new_view_region->max_x &&
                view_region.min_y == new_view_region->min_y &&
                view_region.max_y == new_view_region->max_y);
   };
   if (!clean) {
      view_region.min_x = new_view_region->min_x;
      view_region.max_x = new_view_region->max_x;
      view_region.min_y = new_view_region->min_y;
      view_region.max_y = new_view_region->max_y;
   };
//   if (screen->camera_status() == CAMERA_IN_FRONT) {
      // update houses' display layers before drawing city
      // display mean either update_display or display
      // depending upon camera status
      all_blocks.update_display(); //&view_region, screen);
//   };
   // if camera is above then the blocks will
   // be drawn when the city is displayed (part of background)
//   boolean saved_changed = changed;
//   changed = FALSE;
//   return(clean && !saved_changed);
    Visible_Background::update_display(new_view_region);
    return(FALSE); // might be houses being built, birds flying, etc... 
};

boolean City::display_region() {
   draw_streets();
   all_blocks.display(&view_region);
   return(TRUE); // will probably change next frame
};

boolean City::dump_to_new_file(ascii_string file_name, boolean save_media_too, boolean check_xml_after_titles, boolean zip_file) {
	// check_xml_after_titles and zip_file new on 040504
	// as of 150105 returns FALSE if didn't dump
	tt_saving_media = save_media_too; // new on 200303
	if (check_xml_after_titles && xml_after_titles != NULL) {
		// new on 040504 for flat demos
		xml_save_document(xml_after_titles,file_name,NULL,NULL,zip_file); // don't zip it here
		tt_saving_media = FALSE;
		save_media_files_in_archive(NULL); // new on 200703
	} else if (tt_dump_as_xml) {
      // new on 311002
#if TT_XML
		UnwindProtect<xml_element *> set(tt_zip_description_archive_element,NULL); 
		// new on 180803 so logging demos doesn't interfere with saving cities with media
		UnwindProtect<Sprites *> set2(tt_sprites_with_media_to_save,NULL); 
		// new on 180803 so saving city doesn't interfere with media records for demos and time travel
		xml_document *document = xml_new_document();
		if (document == NULL) {
			return(FALSE); // new on 140105 -- user may have lost patience and aborted
		};
		tt_saving_media = FALSE;
      boolean copied_full_file_name;
      ascii_string full_file_name = compute_full_file_name(file_name,"xml.cty",copied_full_file_name);
      xml_save_document(document,full_file_name,NULL,NULL,zip_file); // don't zip it here -- FALSE new on 040504  
      xml_release_document(document);
		if (tt_current_log_segment == 1 && tt_include_media_in_time_travel_archives && !zip_file && tt_log_out_archive != NULL && 
			 tt_sprites_with_media_to_save != NULL) { 
			// new on 241004 need to be sure that any media loaded is included in the time travel archive -- 
			// only need to do this when saving the first city
			save_media_files_in_archive(tt_log_out_archive,NULL,TRUE);
		} else if (tt_extra_media_file_names != NULL) {
			// new on 110205 -- not used for each city in a time travel archive
			delete [] tt_extra_media_file_names;
			tt_extra_media_file_names = NULL;
		};
		boolean copied_old_format_city_file_name;
      ascii_string old_format_city_file_name = compute_full_file_name(file_name,"cty",copied_old_format_city_file_name); // new on 061102
      if (full_file_name != old_format_city_file_name && // added this condition if file_name already has xml.cty extension
			 file_exists(old_format_city_file_name)) {
//         DeleteFile(old_format_city_file_name);	
			 // OK to be safe but if CityFileMax is set then this isn't called since will have already been renamed
			 rename_file_to_backup(old_format_city_file_name); // replaces the above as of 280103
      };
		if (copied_full_file_name) delete [] full_file_name;
      if (copied_old_format_city_file_name) delete [] old_format_city_file_name;
#endif
   } else {
	   string temp_file_name = append_strings(tt_user_directory,"_city_being_saved_.cty"); 
		// prior to 220702 was temp.cty but user could be named temp
      if (tt_want_exception_breaks) {
	      dump_to_new_file_internal(file_name,temp_file_name);
      } else {
	      try { // added on 010200 since otherwise a broken file is left behind
		      dump_to_new_file_internal(file_name,temp_file_name);
	      } catch (...) {
		      message_box(MB_ICONSTOP|MB_OK|MB_SYSTEMMODAL,
								S(IDS_ERROR_WHILE_SAVING_A_CITY,"Sorry something went wrong while saving a city. City not saved."));
				return(FALSE);
	      };
      };
	   delete [] temp_file_name;
   };
	return(TRUE);
};

#if TT_XML

xml_document *City::xml_new_document(boolean include_explanation) { // abstracted on 110103
//#if TT_DEBUG_ON
//   // for now - conditionalize later...
//   DWORD start = timeGetTime();
//#endif
	// following updated to use UnwindProtect on 310803
   UnwindProtect<Background *> set(tt_dumping_background,this);
	UnwindProtect<DumpReason> set2(tt_dump_reason,DUMPING_CITY);
   xml_document *document = create_xml_document();
   add_processing_instruction(document);
	if (tt_titles_ended_on_frame <= tt_frame_number) { // tt_programmer->are_titles_over()) {
#if TT_LOADING_AND_SAVING_INTERRUPTABLE
		try { // try is new on 140105
			tt_sprites_saved_count = 0; // new on 150105
			top_level_xml(document,NULL);
		} catch (int ) {
			return(NULL);
		};
#else
		top_level_xml(document,NULL);
#endif		
	} else {
		// new on 120103 since if creating a log from a fresh city don't need the rest 
		// (and Programmer doesn't support saving while in Titles in XML)
		xml_element *nothing_element = create_xml_element(L"Nothing",document);
		add_xml_attributes(nothing_element,document); // new on 040805
		xml_append_child(nothing_element,document);
	};
//#if TT_DEBUG_ON
//   tt_error_file() << "Took " << (timeGetTime()-start) << "ms to create XML" << endl;
//#endif
//   tt_dumping_background = NULL;
	return(document);
};

void City::add_xml_attributes(xml_element *element, xml_document *document) {
	// abstracted on 040805
	if (stopped()) {
		xml_set_attribute(element,L"Stopped",1); // new on 090103
	};
	xml_set_attribute(element,L"RandomNumberSeed",(long) random_number_seed()); // new on 020204 (coerced to long to keep things simple)
	xml_set_attribute(element,L"NotebookIDCounter",tt_notebook_id_counter); // new on 281004
	Visible_Background::add_xml_attributes(element,document);
};

void City::add_xml(xml_element *city_element, xml_document *document) { // new on 311002
	Element_Sprite_Table *old_table = push_current_element_sprite_table(); // new on 190103
	Numbered_Sprites *old_numbered_sprites = push_current_numbered_sprites(); // new on 190103
   xml_set_attribute(city_element,L"xmlns",L"http://www.toontalk.com"); 
	// since is a top-level item (what if part of a log - if logs become XML?) 
#if TT_LARGE_CITIES
	xml_append_child(create_xml_element(L"Width",document,tt_city_width),city_element);
	xml_append_child(create_xml_element(L"Height",document,tt_city_height),city_element);
	xml_append_child(create_xml_element(L"Size",document,max(abs(tt_city_width),abs(tt_city_height))),city_element); // for backwards compatibility
#else
   xml_append_child(create_xml_element(L"Size",document,tt_city_size),city_element);
#endif
   //xml_element *time_element = create_xml_element(L"Time",document,tt_current_time); // was an attribute prior to 151102
   //xml_append_child(time_element,city_element); 
	// new on 121102 - used to pick colors, cycles, etc. -- need to process this before blocks so piggy back on Size
   xml_append_child(create_xml_element(L"MaximumHousesInABlock",document,tt_houses_to_a_block),city_element);
	// moved here on 080403 so that no information is lost when processing this and reseting tt_global_picture
//	if (tt_global_picture->animation_in_progress()) { 
	// no need otherwise -- condition commented out on 250203 since for time travel need to know to reset to nothing
	tt_global_picture->add_xml(city_element,L"GlobalPicture",document); // new on 051202
//	};
	//Background *ground = tt_programmer->pointer_to_background(); // commented out on 100103 - so L"OnTheGround" is obsolete
	//if (ground != NULL && ground->on_the_ground()) {
 //     Sprites *ground_items = ground->pointer_to_items();
 //     if (ground_items != NULL) {
	//		ground_items->add_xml(city_element,document,L"OnTheGround"); // rewritten on 100103
 //        //xml_element *ground_element = create_xml_element(L"OnTheGround",document);
 //        //xml_append_child(ground_element,city_element);
 //        //ground_items->add_xml(ground_element,document,NULL,TRUE,TRUE);
	//   };
	//};
   if (tt_queue != NULL) { // conditional new on 161102 -- otherwise loads in a queue with NULL robot and box
	   xml_append_child(tt_queue->xml(document),city_element);
	} else {
		xml_append_child(create_xml_element(L"Queue",document),city_element); // new on 250203 so is reset for time travel
   };
   xml_element *running_element = create_xml_element(L"Running",document); 
	// these were inside of condtional below prior to 250203 but then not reset when time travelling
	if (tt_running_robots != NULL) {
      tt_running_robots->add_xml(running_element,document);
	};
	xml_append_child(running_element,city_element); // moved here on 190804
   if (tt_not_city_stopped_callbacks != NULL) { // new on 181102
		tt_not_city_stopped_callbacks->add_xml_each(city_element,document); // added on 121202
   };
	all_blocks.add_xml(city_element,document); // moved here on 120703 so that these don't dump as include files if any of the above refers to them
   xml_append_child(tt_programmer->xml(document),city_element); 
	// this should be earlier but then is confused about what floor - work on this - 120703
	if (tt_toolbox != NULL) { // new on 170404 -- removed  && tt_toolbox->pointer_to_background() == NULL on 160904
		// at least Tooly's tools have state (e.g. the buttons) that needs to be recorded even if not on the floor
		tt_toolbox->xml_create_and_append_element(city_element,document)->Release();
	};
	if (tt_pictures_to_activate != NULL) { // new on 220704
		xml_element *pictures_to_activate_element = create_xml_element(L"PicturesToRunWhenNotStopped",document);
		tt_pictures_to_activate->add_xml(pictures_to_activate_element,document);
		xml_append_child(pictures_to_activate_element,city_element); // moved here on 190804
	};
	Visible_Background::add_xml(city_element,document);
//	pointer_to_toolbox_notebook()->xml_create_and_append_element(city_element,document,TRUE,FALSE); 
	// new on 300803 since if not on the ground won't be saved
   // following copied from Sprite::xml_create_document on 041102
//	do_postponed_include_file_xml(document); // new on 120703
//#if TT_DEBUG_ON
//	xml_debug_node(city_element);
//#endif
   do_postponed_nest_xml(document);
//	do_postponed_include_files_xml(); // new version as of 150703 -- this just gets the string and dumps it
	schedule_next_postponed_include_file_xml();
	// commented out on 180803 since don't use serial numbers anymore (except to load in old cities)
   //int max_serial_number = return_max_serial_number();
   //if (max_serial_number > 0) {
   //   xml_set_attribute(city_element,L"MaxSerialNumber",max_serial_number);
   //};
//	reset_serial_numbering(); // new on 150103 - important for time travel (and maybe for first sprite save after a city save) - 
	// replaced by the following on 190103
	restore_numbered_sprites(old_numbered_sprites);
	restore_current_element_sprite_table(old_table,document);
//   reset_element_sprite_table(); 
   restore_saved_nest_guids();
};

//boolean loading_of_this_aborted = FALSE; // tried on 140105 but a partially loaded city is dangerous

boolean city_width_or_height_set = FALSE;

boolean City::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
#if TT_LARGE_CITIES // new on 020405
		case WIDTH_TAG:
			tt_city_width = xml_get_int(node,tt_city_width);
			city_width_or_height_set = TRUE;
			return(TRUE);
		case HEIGHT_TAG:
			tt_city_height = xml_get_int(node,tt_city_height);
			city_width_or_height_set = TRUE;
			return(TRUE);
		case SIZE_TAG: { // this code depends upon Width or Height set before Size
			int new_size = xml_get_int(node,tt_city_width);
			if (city_width_or_height_set) {
				initialize(TRUE);
				city_width_or_height_set = FALSE;
			} else if (new_size != tt_city_width || new_size != tt_city_height) {
				tt_city_width = new_size;
				tt_city_height = new_size;
				initialize(TRUE);
			} else {
				reset_all_blocks(TRUE);
			};
			return(TRUE);
							};
#else
		case SIZE_TAG: {
         int new_size = xml_get_int(node,tt_city_size);
			if (new_size != tt_city_size) {
				tt_city_size = new_size;
				initialize(TRUE);  // with new city size -- rewritten on 140103 so this happens only if new size
			} else {
				reset_all_blocks(TRUE);
			};
							};
         return(TRUE);
#endif
      case MAXIMUM_HOUSES_IN_A_BLOCK_TAG: {
         int houses_to_a_block = xml_get_int(node,tt_houses_to_a_block);
         set_houses_to_a_block(houses_to_a_block);
         return(TRUE);
														};
		case BLOCK_TAG: {
			//if (loading_of_this_aborted) {
			//	return(FALSE);
			//};
			//try { // try is new on 140105
				Block *block = block_at(xml_get_attribute_int(node,L"X",0),xml_get_attribute_int(node,L"Y",0));
				if (block == NULL) return(FALSE); // in case dimensions or address changed
				return(xml_entity(node,block));
			//} catch (int) {
			//	// aborted new on 140105
			//	loading_of_this_aborted = TRUE;
			//	return(FALSE);
			};
      case PROGRAMMER_TAG:
			tt_programmer->set_room(NULL); // so it can't be "stale"
         return(xml_entity(node,tt_programmer)); // restored on 070103
		case TOOLBOX_TAG: // new on 170404
			if (tt_toolbox != NULL) {
				xml_load_sprite(node,tag,tt_toolbox,TOOLBOX,FALSE);
				tt_toolbox->set_active(TRUE,FALSE,TRUE); // since the above inactivated it
				return(TRUE); // MISSING prior to 030504
			} else {
				tt_error_file() << "City recorded the state of Tooly, but Tooly isn't around to receive the information." << endl;
				return(FALSE);
			};
		case QUEUE_TAG: {
			if (tt_queue != NULL) { // new on 070103
				delete tt_queue;
			};
         tt_queue = new Queue(NULL,NULL);
//			xml_debug_node(node,L"queue");
         boolean result = xml_entity(node,tt_queue);
			if (tt_queue->empty()) {
				delete tt_queue;
				tt_queue = NULL;
			};
			return(result);
							 };
      case RUNNING_TAG: // new on 071102
			just_destroy_list(tt_running_robots); // new on 070103 -- was destroy_sprites prior to 070203 - not ref counted
         tt_running_robots = xml_load_sprites(node);
         return(TRUE);
		case GLOBAL_PICTURE_TAG: {
			if (tt_global_picture == NULL) {
				tt_global_picture = new GlobalPicture(); // new on 240105
			} else {
				tt_global_picture->re_initialize(); // new on 250203 for time travel
			};
			xml_node *first = first_node_that_is_an_element(node);
			if (first != NULL) {
				return(xml_entity(first,tt_global_picture));
			} else {
				log("Something wrong with the XML tag for GlobalPicture.",FALSE,TRUE);
				return(FALSE);
			};
										 };
		case PICTURES_TO_RUN_WHEN_NOT_STOPPED_TAG: // new on 220704
			destroy_sprites(tt_pictures_to_activate); // since ref counted use destroy_sprites rather than just_destroy_list
         tt_pictures_to_activate = xml_load_sprites(node);
			if (tt_pictures_to_activate != NULL) {
				tt_pictures_to_activate->increment_ref_count();
			};
			return(tt_pictures_to_activate != NULL);
		case ANIMATION_TAG:
			if (tt_not_city_stopped_callbacks != NULL) { // new on 070103
				delete tt_not_city_stopped_callbacks;
			};
			tt_not_city_stopped_callbacks = new AnimationCallBacks(NULL,CALLBACK_NOTHING);
			return(xml_entity(node,tt_not_city_stopped_callbacks));
		//case NOTEBOOK_TAG:
		//	// this should be the notebook that lives in Tooly - the following should work without special code - new on 300803
		//	xml_get_sprite(node,pointer_to_toolbox_notebook(),PROGRAM_PAD);
		//	return(TRUE);
		case CITY_TAG:
#if TT_LOADING_AND_SAVING_INTERRUPTABLE
			try { // new on 140105
				tt_sprites_loaded_count = 0; // new on 150105
				return(Visible_Background::handle_xml_tag(tag,node));
			} catch (string city_file_name) {
				// aborted new on 140105
				reset_all_blocks();
				xml_document *document = document_from_file(city_file_name);
				if (document != NULL) {
					xml_entity(document,this,FALSE);
					document->Release();
				};
				delete [] city_file_name;
				return(FALSE);
			};
#else
			return(Visible_Background::handle_xml_tag(tag,node));
#endif
		case NOTHING_TAG:
			tt_programmer->set_next_status(START_FLYING); // since this was clobbered -- since no city this is the right starting status
			return(Visible_Background::handle_xml_tag(tag,node)); // new on 040805
		case NO_MORE_TAGS:
//			loading_of_this_aborted = FALSE;
			set_stopped(xml_get_attribute_int(node,L"Stopped",FALSE)); // new on 090103 - default is that it is ON unless explicitly stopped
			if (stopped()) { // new on 200204 to fix a bug restoring cities that were saved with the power off
				::stop_all(TRUE);
			};
			set_random_number_seed((unsigned long) xml_get_attribute_int(node,L"RandomNumberSeed",(long) random_number_seed())); // new on 020204
			tt_notebook_id_counter = xml_get_attribute_int(node,L"NotebookIDCounter",tt_notebook_id_counter); // new on 281004
			// and the following
      default:
         return(Visible_Background::handle_xml_tag(tag,node));
   };
};

void City::set_xml_after_titles(xml_document *node) {
	if (tt_time_travel == TIME_TRAVEL_OFF) { // removed  || tt_time_travel == TIME_TRAVEL_ABOUT_TO_START on 061003
		xml_after_titles = node;
		node->AddRef();
		tt_programmer->set_starting_status(START_AFTER_LOADING_XML);
	} else { // new on 140103
		xml_entity_and_activate(node);
		if (tt_time_travel == TIME_TRAVEL_ABOUT_TO_START) { // new on 200703 -- moved here from other branch on 061003
			time_travel(TIME_TRAVEL_ON);
		};
	};
};

void City::xml_entity_and_activate(xml_document *node) { // abstracted on 270904
	xml_entity(node,this,FALSE); // FALSE new arg on 200204 since NO_MORE_TAGS will be sent from Background's call
	set_sprites_to_active_now();
	if (tt_programmer == NULL) {
		tt_programmer = new Programmer();
	};
	tt_programmer->activate_state();
};

void City::process_xml_after_titles() { 
	if (xml_after_titles != NULL) {
		xml_entity(xml_after_titles,this,FALSE); // FALSE new arg on 200204
		xml_after_titles->Release();
		xml_after_titles = NULL;
		if (!(tt_jump_to_current_log_segment || tt_jump_to_youngest_log_segment) && tt_time_between_new_logs > 0) { 
			// moved on 130103 since tt_current_time was reset when city loads
			// condition updated 290204
			tt_next_new_log_time = tt_current_time;
		};
		set_sprites_to_active_now();
	};
};

#endif

ascii_string City::compute_full_file_name(ascii_string file_name, ascii_string extension, boolean &copied) { // abstracted on 041102
   copied = !already_has_extension(file_name,"cty"); // i.e. will be copied
	ascii_string full_file_name;
	if (copied) {
		full_file_name = new_file_name(file_name,extension);
		maintain_n_versions_youngest_first(full_file_name,ini_int(AC(IDC_SWITCHES),"CityFileMax",FALSE),TRUE); // new on 180799
	} else { // this part new on 250100
		full_file_name = file_name; // already full enough if it has extension
	};
   return(full_file_name);
};

void City::dump_to_new_file_internal(ascii_string file_name, ascii_string temp_file_name) {
	output_file_stream stream(temp_file_name,std::ios_base::out|std::ios_base::binary);
	if (stream == NULL) { // new on 290904 for error recovery
		unable_to_write_file(temp_file_name); 
		return;
	};
	dump_to_stream(stream); // pulled this out on 150599 so that city can be dumped to clipboard
	stream.close();
   boolean copied;
   ascii_string full_file_name = compute_full_file_name(file_name,"cty",copied);
	//if (file_exists(full_file_name,FALSE)) { // commented out on 280103 since now using MOVEFILE_REPLACE_EXISTING
	//	DeleteFile(full_file_name);
	//};
	move_file(temp_file_name,full_file_name);
	if (tt_local_ini_file_loaded_with_city != NULL) { // new on 040301
		strcpy(full_file_name+strlen(full_file_name)-3,"ini"); // re-use name to generate ini file name
		if (!CopyFile(tt_local_ini_file_loaded_with_city,full_file_name,FALSE)) {
			unable_to_write_file(full_file_name);
		};
	};
	if (copied) delete [] full_file_name;
};
//   if (on_the_floor()) {
//      stream.put((char) WHOLE_FLOOR);
//   } else if (inside_a_house()) {
//      stream.put((char) WHOLE_HOUSE);
//   } else {
//      stream.put((char) WHOLE_CITY);
//   };

void City::dump_to_stream(output_stream &stream, boolean save_media_too) { // save_media_too arg new on 200703
#if TT_XML
	tt_saving_media = save_media_too; // I think this is just to collect the names of the media files -- not to actually include them in the file
	if (xml_after_titles != NULL) {
		xml_to_stream(xml_after_titles,stream);
		tt_saving_media = FALSE;
		save_media_files_in_archive(NULL); // new on 200703
		return;
	} else if (tt_dump_as_xml) { // new on 110103
		xml_document *document = xml_new_document();
		if (document == NULL) {
			// what if doing this to for a log file??
			return; // new on 140105 -- user may have lost patience and aborted
		};
//		xml_node *node = first_node_that_is_an_element(document);
		// changed on 120103 to keep the "whole" document rather than just the City element in the file
		xml_to_stream(document,stream);
		xml_release_document(document);
		tt_saving_media = FALSE;
		save_media_files_in_archive(NULL); // new on 200703
		return;
	};
#endif
	// following was after xml dumping stuff prior to 100603 but that is very wasteful
	// moved back same day until can handle zipped files within DMO files
	if ((tt_frame_number <= 1 || tt_frame_number == tt_still_frame_count) && file_name_of_last_city_loaded != NULL) { 
		// new on 191201 to just copy the bytes
//		input_file_stream old_city_file(file_name_of_last_city_loaded,std::ios_base::in|std::ios_base::nocreate|std::ios_base::binary);
		File *old_city_file = open_file_or_url(file_name_of_last_city_loaded,FALSE);
		const int buffer_size = 8*1024; // who knows?
		char buffer[buffer_size]; // was unsigned prior to 210703
		DWORD bytes_read = buffer_size;
		old_city_file->Read(buffer,buffer_size,&bytes_read);
		while (bytes_read > 0) {
			stream.write(buffer,bytes_read);
			old_city_file->Read(buffer,buffer_size,&bytes_read);
		};
		old_city_file->Close();
		return;
	};
   tt_dumping_background = this;
   write_cookie(stream);
	dump_started();
	if (tt_programmer->kind_of() != PROGRAMMER_TITLES) { // shouldn't happen if loading a city - new on 121202
		dump(stream,TRUE); // make sure trucks, birds, etc. are not caught in an intermediate state
	};
   dump(stream);
   tt_dumping_background = NULL;
   dump_ended(); //release_sprites_being_dumped();
};

void City::dump(output_stream &stream, boolean just_prepare) {
#if TT_DEBUG_ON
	millisecond start;
	LARGE_INTEGER high_res_start;
	if (tt_debug_mode == 260900) {
		start = timeGetTime();
		QueryPerformanceCounter(&high_res_start); 
	};
#endif
   if (just_prepare) {
      all_blocks.dump(stream,just_prepare);
//      return; - commented out on 100500 since need to prepare ground items, running robots, etc. as well
   };
	if (!just_prepare) {
#if TT_LARGE_CITIES
		short int city_size = (short int) max(tt_city_width,tt_city_height); // new on 020405
		stream.write((character *) &city_size, sizeof(city_size));
#else
		short int city_size = (short int) tt_city_size; // new on 020405
		stream.write((character *) &tt_city_size, sizeof(tt_city_size));
#endif
		// added the following on 010499 to save the number of houses per block
		stream.write((character *) &tt_houses_to_a_block, sizeof(tt_houses_to_a_block));
		all_blocks.dump(stream);
		stream.put(NOTHING_MARKER);
	};
   if (items != NULL) {
      items->dump(stream,TRUE,just_prepare);
	};
	Background *ground = tt_programmer->pointer_to_background();
	if (ground != NULL && ground->on_the_ground()) {
		Sprites *ground_items = ground->pointer_to_items();
		if (ground_items != NULL) {
			ground_items->dump(stream,TRUE,just_prepare,TRUE);
		};
/* replaced the following with the above on 150400 since after loading it now translate coordinates
		Sprites *remaining = ground->pointer_to_items();
		while (remaining != NULL) {
			Sprite *sprite = remaining->first();
			// commented out toolbox part of conditional on 110100
			if (// sprite != tt_toolbox &&
             // !(tt_toolbox != NULL && toolbox_tool(sprite)) && 
			    !sprite->held()) {			
//				Sprite *sprite_copy = sprite->copy();
//				sprite_copy->save_floor_priority();
//				tt_programmer->translate_ground_to_city_coordinates(sprite_copy);
			// stopped using copies on 110100 since wasn't perfect (e.g. friend of Tooly was wrong)
				sprite->dump(stream);
				if (sprite->kind_of() == PICTURE) { // starting 120200 they dump geometry regardless on their own
					sprite->dump_priority(stream);
					sprite->dump_misc(stream);
					stream.put((char) FALSE); // just a boolean indicating no deltas - saves 7 bytes
				} else {
//						sprite->dump_geometry(stream,delta_x,delta_y);
						// replaced the above with below on 150400 to deal with coordinate translation
					city_coordinate saved_llx = sprite->current_llx();
					city_coordinate saved_lly = sprite->current_lly();
					city_coordinate saved_width = sprite->current_width();
					city_coordinate saved_height = sprite->current_height();
					tt_programmer->translate_ground_to_city_coordinates(saved_width,saved_height,saved_llx,saved_lly);
					stream.write((character *) &saved_llx,sizeof(saved_llx));
					stream.write((character *) &saved_lly,sizeof(saved_lly));
					stream.write((character *) &saved_width,sizeof(saved_width));
					stream.write((character *) &saved_height,sizeof(saved_height));
					sprite->dump_priority(stream);
					sprite->dump_misc(stream); // new on 140100
				};
//				sprite_copy->destroy(); // copied just for this purpose
			};
			remaining = remaining->rest();
		};
		*/
	};
	// both city items and ground items mixed together here - does this make sense??
   if (!just_prepare) {
		stream.put(END_OF_LIST_MARKER);
		tt_programmer->dump(stream); // dump it afterwards since needs to refer to house it might be in
		if (tt_queue == NULL) { // new on 190100
			stream.put(NOTHING_MARKER);
		} else {
#if TT_DEBUG_ON
			if (tt_debug_mode == 160299) {
				tt_error_file() << "Starting to dump tt_queue" << endl;
			};
#endif
			tt_queue->dump(stream);
#if TT_DEBUG_ON
			if (tt_debug_mode == 160299) {
				tt_error_file() << "Done dumping tt_queue" << endl;
			};
#endif
		};
	};
	if (tt_running_robots != NULL) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299) {
			tt_error_file() << "Starting to dump tt_running_robots" << endl;
		};
#endif
		tt_running_robots->dump(stream,FALSE,just_prepare); // added FALSE on 150500
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299) {
			tt_error_file() << "Done dumping tt_running_robots" << endl;
		};
#endif
	};
	if (!just_prepare) stream.put(END_OF_LIST_MARKER);
#if TT_DEBUG_ON
	if (tt_debug_mode == 260900) {
		tt_error_file() << "Took " << (timeGetTime()-start) << "ms to dump city." << endl;
		LARGE_INTEGER high_res_stop;
		LARGE_INTEGER high_res_frequency;
		QueryPerformanceCounter(&high_res_stop);
		QueryPerformanceFrequency(&high_res_frequency);
		int64 duration = (1000000*(high_res_stop.QuadPart - high_res_start.QuadPart))/high_res_frequency.QuadPart;
		tt_error_file() << (long) duration << " microseconds" << endl;
	};
#endif
};

//boolean City::load_city_from_clipboard_string(string clipboard_text) {
 // noticed obsolete on 270904
	//tt_uudecoding_a_sprite = TRUE; // prevents looping during loading NEEDED??
	//long encoding_length;
	//NaturalLanguage language;
	//int version_number;
	//string encoding = uudecode(clipboard_text,encoding_length,language,version_number);
	//if (encoding != NULL && encoding[0] == WHOLE_CITY) {
	//	StringInputStream encoding_stream(encoding+1,encoding_length);
	//	if (load_city_from_stream(&encoding_stream)) {
	//		tt_uudecoding_a_sprite = FALSE;
	//		delete [] encoding; // new on 270200
	//		return(TRUE);
	//	};
	//};
	//if (encoding != NULL) delete [] encoding; // new on 270200
	//tt_uudecoding_a_sprite = FALSE;
	//return(FALSE);
//};

boolean City::load_city(ascii_string city_name, boolean directly_loadable) {
	// directly_loadable is new on 040504 since in logs there is no extra stuff about the size of the file
	// new on 150599 that check the clipboard first to see if city is there
	// commented out on 010903 since didn't make much sense and makes it trickier to generate logs due to call to clipboard_text
	//boolean new_text;
	//string clipboard_text = tt_main_window->clipboard_text(new_text);
	//if (clipboard_text != NULL) {
	//	if (load_city_from_clipboard_string(clipboard_text)) {
	//		if (new_text) delete [] clipboard_text;
	//		return(TRUE);
	//	} else {
	//		if (new_text) delete [] clipboard_text;
	//	};
	//};
   if (city_name == NULL) return(FALSE);
   FileNameStatus file_status;
	// added args on 180200 so this doesn't use path to find the city - OK?? - otherwise it looks for the users CTY file ...
   ascii_string full_name = existing_file_name(file_status,city_name,"cty","NewUsers",TRUE,FALSE,FALSE,FALSE,FALSE);
   boolean xml_format = FALSE; // unless proven otherwise
#if TT_XML
   if (full_name == NULL) { // new on 041102
      full_name = existing_file_name(file_status,city_name,"xml.cty","NewUsers",TRUE,FALSE,FALSE,FALSE,FALSE);
 //     if (full_name != NULL) xml_format = TRUE;
	//} else if (already_has_extension(full_name,"xml.cty")) { // new on 140103
	//	// wouldn't it be better to determine if xml format looking at the first few bytes?
	//	xml_format = TRUE;
	};
	boolean delete_cty_file_after_loading = FALSE;
	string time_travel_log_in_archive = log_in_archive(TRUE); // new on 131003
	if (full_name == NULL && 
		time_travel_log_in_archive != NULL &&
		log_in_archive() != NULL) { // still can't find it so check time travel archive new on 190703
		// removed && time_travel_enabled() on 150903 since if have an archive use it even if time travel turned off now
		string short_log_file_name = short_file_name(city_name,TRUE); // kind of awkward - new on 010703
		// would be a bit nicer if this was extracted to the temp file directory and used there - noted on 290204
		delete_cty_file_after_loading = extract_archive_to_directory(time_travel_log_in_archive,short_log_file_name,tt_user_directory);
//		delete [] time_travel_log_in_archive; // moved to below on 140104 to fix a minor leak
//		delete_cty_file_after_loading = extract_archive_to_directory(tt_log_in_archive,short_log_file_name,tt_user_directory);
		// since is just a copy of what is in the archive - new on 180703
		delete [] short_log_file_name;
		if (delete_cty_file_after_loading) {
			full_name = copy_string(city_name);
			xml_format = TRUE; // no other format in these archives
// moved out of this conditional on 021003
		//} else if (strcmp(city_name,tt_file_name) != 0) { // new on 240803
		//	return(load_city(copy_string(tt_file_name))); // try again with usual user's city file
		} else { // new on 290204 to deal differently with archives missing the current city
			tt_jump_to_youngest_log_segment = TRUE; 
			return(FALSE);
		};
	};
	if (tt_log_in_archive == NULL) {
		delete [] time_travel_log_in_archive; // moved here 140104
	};
#endif
	if (full_name == NULL) {
		//if (tt_file_name_read_only != NULL && strcmp(city_name,tt_file_name_read_only) != 0) { // new on 030304
		//	char fuller_file_name[MAX_PATH]; // doesn't include extension
		//	strcpy(fuller_file_name,tt_default_user_directory);
		//	strcat(fuller_file_name,tt_file_name_read_only);
		//	boolean loaded = load_city(fuller_file_name);
		//	if (loaded) return(TRUE); // else try again with tt_file_name below
		//};
		if (strcmp(city_name,tt_file_name) != 0) { // new on 240803 -- moved on 021003
			return(load_city(tt_file_name)); // try again with usual user's city file -- on 130104 removed copy_string() to stop a little leak
		};
	};
   if (full_name == NULL) {
		if (tt_default_file_name != NULL && 
			 (strcmp(city_name,tt_file_name) == 0 || strstr(city_name,"current.xml.cty") != NULL)) {
			// user doesn't have a city but default user name provided - updated 170903 so that if there is no current city in time travel archive then still use default user's city
			UnwindProtect<string> set(tt_user_directory,tt_default_user_directory);
         full_name = existing_file_name(file_status,tt_default_file_name,"cty","NewUsers",TRUE,FALSE,FALSE,FALSE,FALSE);
#if TT_XML
         if (full_name == NULL) {
            full_name = existing_file_name(file_status,tt_default_file_name,"xml.cty","NewUsers",TRUE,FALSE,FALSE,FALSE,FALSE);
            if (full_name != NULL) xml_format = TRUE;
         };
#endif
		};
		if (full_name == NULL) { // still can't find one
			return(FALSE);
		};
	};
//	// new on 011003
//	FileInputStream stream;
//	stream.open(full_name,std::ios_base::in|std::ios_base::binary); // |std::ios_base::nocreate);
	if (tt_local_ini_file == NULL && !tt_ini_file_only_from_tt_ini_file) { // new on 040301
		compute_local_ini_file_name(full_name);
		if (tt_local_ini_file != NULL) { 
			tt_local_ini_file_loaded_with_city = copy_string(tt_local_ini_file);
		};
	};
   boolean result;
   //if (xml_format) {
   //   tt_loading_a_city = TRUE; 
	// still needed?? - asked 041102 -- used now (170403) to reset city without extra consequences
	// (like no houses so building new ones)
   //   result = handle_xml(full_name);
   //   tt_loading_a_city = FALSE;
   //} else {
	if (!directly_loadable) {
		FileInputStream stream;
		stream.open(full_name,std::ios_base::in|std::ios_base::binary); // |std::ios_base::nocreate);
		try {
			result = load_city_from_stream(&stream);
			stream.close();
		} catch (City *) { // new on 011003 to handle PK zipped files
			stream.close();
			directly_loadable = TRUE;
		};
	};
	if (directly_loadable) { // moved here from catch above on 040504
//		tt_loading_a_city = TRUE; 
		UnwindProtect<boolean> set(tt_loading_a_city,TRUE); 
		// was ordinary assignment prior to 140105 but can be called recursively now
		UnwindProtect<boolean> save(tt_loading_is_ok_to_abort); // new on 200105
		if (!time_travel_enabled() && !replaying()) {
			tt_loading_is_ok_to_abort = TRUE;
		};
		result = handle_xml(full_name);
//		tt_loading_a_city = FALSE;
	};
	if (delete_cty_file_after_loading) { // was just unzipped here
		DeleteFile(full_name);
	};
//	if (file_status == FILE_ON_INTERNET_AND_DELETE_COPY) DeleteFile(full_name); // obsolete so commented out on 191201
//   delete [] full_name; // restored on 180103 -- commented out on 100603 since used by file_name_of_last_city_loaded
	if (file_name_of_last_city_loaded != NULL) { // new on 191201
		delete [] file_name_of_last_city_loaded;
	};
	file_name_of_last_city_loaded = full_name;
   return(result);
};

boolean City::load(InputStream *stream, boolean ignore_city) {
	// starting 090103 read cookie first to see if XML
   char cookie[9];
	stream->read((string) cookie,4); // only read first 4 bytes since that is either the length or the cookie
	if (is_zip_cookie((string) cookie)) { // new on 011003
		throw this; // kind of a hack to throw the city but why not?
	};
	if (cookie[0] == '<' && cookie[1] == '?' && (cookie[2] == 'x' || cookie[2] == 'X') && (cookie[3] == 'm' || cookie[3] == 'M')) {
		throw this; // new on 040504 in case trying to load an uncompressed city (e.g. extracted from a demo)
	};
	if (!valid_cookie(cookie)) { // then try XML
		long length; // ugly but OK
		memcpy((char *) &length,cookie,4);
		int max_length = tt_cache_memory_max_size; // typically 1/4 of phyiscal memory -- new on 061003
//		int max_length = 100000000; // 100M
		if (length < max_length && length >= 0) { // new that no city can be loaded this way that is "too big"
			if (ignore_city) {
				stream->ignore(length*sizeof(wide_character)); // new on 220703
			} else {
				xml_document *city_document = xml_from_stream(stream,length);
				handle_xml(city_document);
			};
			return(TRUE);
#if TT_DEBUG_ON
		} else {
#if TT_DEBUG_ON
			debug_this();
#endif
			tt_error_file() << "Not likely that a city in a log file has a length of " 
								 << length << " and TellG is " << stream->tellg() << endl;
#endif
			return(FALSE); // better to return here than try to load the city with a bad cookie -- new on 061103
		};
	};
	stream->read((string) cookie+4,4); // read rest of cookie
	cookie[8] = '\0'; // terminate string
#if TT_DEBUG_ON
	millisecond start;
	LARGE_INTEGER high_res_start;
	if (tt_debug_mode == 260900) {
//		loading_wait();
//BEGIN_GDI
//		UpdateWindow(tt_main_window->get_handle());
//		SendMessage(tt_main_window->get_handle(),WM_PAINT,0,0);
//END_GDI
		start = timeGetTime();
		QueryPerformanceCounter(&high_res_start); 
	};
#endif
//	UnwindProtect<unsigned char> set(tt_log_version_number,31); // new on 040304 since read_string has changed
   NaturalLanguage language;
   character cookie_version_number;
   if (!process_cookie(cookie,cookie_version_number,language)) return(FALSE);
#if TT_LARGE_CITIES
	short int city_size; // new on 020405
   stream->read((string) &city_size, sizeof(city_size));
	tt_city_width = city_size;
	tt_city_height = city_size;
#else
	stream->read((string) &tt_city_size, sizeof(tt_city_size));
#endif
	if (cookie_version_number >= first_version_with_houses_per_block) {
		stream->read((string) &tt_houses_to_a_block, sizeof(tt_houses_to_a_block));
		set_houses_to_a_block(tt_houses_to_a_block); // added on 120499 so cities load with correct block widths
	};
   initialize();  // with new city size
	unsigned char type = (unsigned char) stream->get();
	load_started();
	tt_loading_a_city = TRUE; // new on 181200 - moved on 191200
   while (type == BLOCK_MARKER) {
      load_block(stream,cookie_version_number,language,this);
      type = (unsigned char) stream->get();
   };
	boolean internal_error_occurred = FALSE;
	Sprites *city_items = NULL;
	if (cookie_version_number >= first_version_to_save_city_items) {
#if TT_DEBUG_ON
		long start = stream->tellg();
#endif
      if (tt_want_exception_breaks) {
		   city_items = load_items(stream,cookie_version_number,language);
   //		tt_loading_a_city = FALSE; // commented out on 260201 since this should be reset after loading programmer (since might be holding a tool)
      } else {
		   try {
			   city_items = load_items(stream,cookie_version_number,language);
   //			tt_loading_a_city = FALSE; // commented out on 260201 since this should be reset after loading programmer (since might be holding a tool)
		   } catch (...) {
			   tt_loading_a_city = FALSE;
			   if (handle_internal_error(S(IDS_INTERNAL_ERROR_LOADING_CITY,"Sorry but an internal error occurred while loading a city. Strange things may happen."),TRUE)) {
				   city_items = tt_items_loaded_so_far; // those loaded so far are put here for this purpose starting 120400
				   tt_items_loaded_so_far = NULL;
				   load_ended(); // added 150400
				   internal_error_occurred = TRUE;
			   } else {
				   throw(tt_global_picture);
			   };
		   };
      };
	   if (city_items != NULL) {
#if TT_DEBUG_ON
		   if (tt_debug_mode == 141100) {
			   tt_error_file() << (stream->tellg()-start) << " bytes in encoding of stuff outside and not associated with a block" << endl;
		   };
#endif
		   add_items(city_items);
		   city_items->recursively_propagate_changes();
		   city_items->activate();
	   };
   };
	if (!internal_error_occurred) {
#if TT_DEBUG_ON
		long start = stream->tellg();
#endif
		if (stream != NULL) tt_programmer->load(stream,cookie_version_number,language);
		if (cookie_version_number >= first_version_to_save_cubby_of_robot) {
			tt_queue = load_queue(stream,cookie_version_number,language);
#if TT_DEBUG_ON
			if (tt_debug_mode == 141100 && tt_queue != NULL) {
				tt_error_file() << (stream->tellg()-start) << " bytes in encoding of queue of " << tt_queue->length() << " waiting events." << endl;
				start = stream->tellg();
			};
#endif
			tt_running_robots = load_items(stream,cookie_version_number,language,NULL,FALSE,NULL,FALSE);
			// commented out tt_running_robot ref count stuff on 090400 since causes robots to not be deleted and to run (as orphans)
			// and when robot is prepared for deletion it removes itself from tt_running_robot
	//		if (tt_running_robots != NULL) {
	//			tt_running_robots->increment_ref_count(); // new on 280100
	//		};
#if TT_DEBUG_ON
			if (tt_debug_mode == 141100 && tt_running_robots != NULL) {
				tt_error_file() << (stream->tellg()-start) << " bytes in encoding of " << tt_running_robots->length() << " running or waiting robots." << endl;
			};
#endif
		};
		if (tt_programmer->status_when_dumped() == START_ON_THE_GROUND) {
			Sprites *remaining = city_items;
			while (remaining != NULL) {
				Sprite *sprite = remaining->first();
				city_coordinate llx = sprite->current_llx();
				city_coordinate lly = sprite->current_lly();
				city_coordinate width = sprite->current_width();
				city_coordinate height = sprite->current_height();
				tt_programmer->translate_ground_to_city_coordinates(width,height,llx,lly);
				sprite->set_size_and_location(width,height,llx,lly,LOADING_GEOMETRY); // added LOADING_GEOMETRY on 080202 so numbers and text pads don't recalculate size
				remaining = remaining->rest();
			};
		};
	};
	if (city_items != NULL) {
		city_items->remove_all();
		delete city_items;
	};
	tt_loading_a_city = FALSE; // moved here on 260201
	load_ended();
#if TT_DEBUG_ON
	if (tt_debug_mode == 260900) {
		tt_error_file() << "Took " << (timeGetTime()-start) << "ms to load city." << endl;
		LARGE_INTEGER high_res_stop;
		LARGE_INTEGER high_res_frequency;
		QueryPerformanceCounter(&high_res_stop);
		QueryPerformanceFrequency(&high_res_frequency);
		int64 duration = (1000000*(high_res_stop.QuadPart - high_res_start.QuadPart))/high_res_frequency.QuadPart;
		tt_error_file() << (long) duration << " microseconds" << endl;
//		done_waiting_for_load();
	};
#endif
	return(TRUE);
};

boolean City::load_city_from_stream(InputStream *stream, boolean ignore_city) {
   if (tt_want_exception_breaks) {
	   return(load(stream,ignore_city));
   } else {
	   try { // new on 151299
		   return(load(stream,ignore_city));
	   } catch (...) {
		   handle_internal_error(S(IDS_INTERNAL_ERROR_LOADING_CITY,"Sorry but an internal error occurred while loading a city. Strange things may happen."),TRUE);
		   return(FALSE);
	   };
   }; 
};

void City::finish_instantly() {
   all_blocks.finish_instantly();
	Visible_Background::finish_instantly(); // new on 030903
};

void City::screen_flushed() {
   all_blocks.screen_flushed();
};

void City::draw_streets() {
BEGIN_GDI
#if TT_DIRECTX // default in place otherwise
   tt_main_window->select_stock_object(NULL_PEN);
//   SelectObject(main_device_context,GetStockObject(NULL_PEN));
#endif
   // could in principle just paint the whole city and let clipping take
   // care of things but instead I just paint the region viewed
   // which I first clip by the boundaries of the city
   city_coordinate min_x = view_region.min_x;
   city_coordinate min_y = view_region.min_y;
   city_coordinate max_x = view_region.max_x;
   city_coordinate max_y = view_region.max_y;
   boolean view_region_completely_inside_city = (min_x > city_min_x && max_x < city_max_x && min_y > city_min_y && max_y < city_max_y);
   if (!view_region_completely_inside_city) { // might need to clip to city
      if (min_x < city_min_x) {
         min_x = city_min_x;
      };
      if (max_x > city_max_x) {
         max_x = city_max_x;
      };
      if (min_y < city_min_y) {
         min_y = city_min_y;
      };
      if (max_y > city_max_y) {
         max_y = city_max_y;
      };
   };
#if !WING&&!TT_DIRECTX
   color lawn_color = GREEN; // 121 was too dark
#endif
   if (!view_region_completely_inside_city) {
      tt_main_window->clear(water_brush_id());
#if WING||TT_DIRECTX
      tt_main_window->select_brush(lawn_brush_id());
      // could be an elipse!!
      tt_screen->draw_rectangle(min_x,max_x,min_y,max_y);
      tt_main_window->deselect_brush();
#else
      tt_screen->clear(BLUE); // water around rectangular city?
      tt_screen->draw_rectangle(lawn_color,min_x,max_x,min_y,max_y);
#endif
    } else {
#if WING||TT_DIRECTX
// following didn't seem to fix the problem of pattern not moving while scrolling
// trying again
      tt_screen->set_brush_origin(min_x,min_y);
      tt_main_window->clear(lawn_brush_id());
#else
      tt_screen->clear(lawn_color);
#endif
#if TT_WINDOWS // going to clip anyway so make sure leftmost and rightmost
               // pixels are included
      min_x -= tt_screen->one_pixel_in_x_city_units();
      max_x += tt_screen->one_pixel_in_x_city_units();
#endif
   };
   long street = min_y/ideal_screen_width; // was block_number prior to 010499
   // the bottom of horizontal streets is a multiple of ideal_screen_width
   city_coordinate bottom_y, y;
   // huh??? what is going on here?
   do {bottom_y = street*ideal_screen_width;
       y = bottom_y+x_street_width(street);
       street++;}
    while (y < min_y);
   tt_main_window->select_brush(street_brush_id());
#if WING||TT_DIRECTX
   tt_screen->draw_rectangle(min_x,max_x,bottom_y,y);
#else
   tt_screen->clip_rectangle(x_street_color,min_x,max_x,bottom_y,y);
#endif
   y += ideal_screen_width; // start with next street
   for (; y < max_y; y += ideal_screen_width) {
#if WING||TT_DIRECTX
      tt_screen->draw_rectangle(min_x,max_x,y-x_street_width(street),y);
#else
      tt_screen->draw_rectangle(x_street_color,min_x,max_x,
                                y-x_street_width(street),y);                            
#endif
      street++;
   };
#if WING||TT_DIRECTX
   tt_screen->draw_rectangle(min_x,max_x,y-x_street_width(street),max_y);
#else
   tt_screen->clip_rectangle(x_street_color,min_x,max_x,
                             y-x_street_width(street),max_y);
#endif
   // now for the y_streets
   street = min_x/tt_ideal_block_width;
   city_coordinate left_x, x;
   do {left_x = street*tt_ideal_block_width;
       x = left_x+y_street_width(street);
       street++;}
    while (x < min_x);
#if TT_WINDOWS&&!GRAPHICS_TO_MEMORY
   const color y_street_color = PALETTERGB(64,64,64);
#elif WING||TT_DIRECTX
//      BrushId brush_id = street_brush_id();
   left_x -= tt_screen->one_pixel_in_x_city_units(); // seems to fix a problem with 1 pixel wide water on easternmost street
   if (left_x < min_x) {
		left_x = min_x;
	};
   if (x >= view_region.min_x && left_x < x && left_x <= view_region.max_x) {
      tt_screen->draw_rectangle(left_x,x,min_y,max_y);
#if TT_DEBUG_ON
		if (tt_debug_mode == 310399) {
         tt_error_file() << left_x << " " << (int) tt_screen->screen_x(left_x) << " "
								 << x << " " << (int) tt_screen->screen_x(x) << " "
								 << min_y << " " << max_y
								 << " frame = " << tt_frame_number << " first." << endl;
		};
#endif
   };
#else
   const color y_street_color = DARK_GRAY;
   tt_screen->clip_rectangle(y_street_color,left_x,x,min_y,max_y);
#endif
	x += tt_ideal_block_width; // start with next street
	max_x += y_street_width(street); // since that is subtracted
   for (; x < max_x; x += tt_ideal_block_width) {
#if WING||TT_DIRECTX
      tt_screen->draw_rectangle(x-y_street_width(street),x,min_y,max_y);
#if TT_DEBUG_ON
		if (tt_debug_mode == 310399) {
			tt_error_file() << (x-y_street_width(street)) << " " << (int) tt_screen->screen_x(x-y_street_width(street)) << " "
								 << x << " " << (int) tt_screen->screen_x(x) << " "
								 << min_y << " " << max_y
								 << " frame = " << tt_frame_number << " others." << endl;
		};
#endif
#else
      tt_screen->draw_rectangle(y_street_color,x-y_street_width(street),x,
                                min_y,max_y);
#endif
      street++;
   };
#if WING||TT_DIRECTX
   x -= y_street_width(street);
   if (x >= view_region.min_x && max_x <= view_region.max_x && max_x > x) {
      tt_screen->draw_rectangle(max_x-y_street_width(street),max_x,min_y,max_y); // max_x-y_street_width(street) was x prior to 150301
//         tt_error_file() << x << " " << (int) tt_screen->screen_x(x) << " "
//                  << max_x << " " << (int) tt_screen->screen_x(max_x) << " "
//                  << min_y << " " << max_y
//                  << " frame = " << tt_frame_number << " last." << endl;
   };
#else
   tt_screen->clip_rectangle(y_street_color,x-y_street_width(street),max_x,
              min_y,max_y);
#endif
   tt_main_window->deselect_brush();
END_GDI
};

BrushId City::street_brush_id() {
   // following should be STREET_SIDE_BRUSH but it looked ugly
   if (tt_screen->camera_status() == CAMERA_IN_FRONT)      return(STREET4_BRUSH);
   city_scale scale = tt_screen->current_scale();
   if (scale < ground_scale*3) return(STREET1_BRUSH);
   if (scale < ground_scale*6) return(STREET2_BRUSH);
   return(STREET4_BRUSH);
};

BrushId City::lawn_brush_id() {
   if (tt_screen->camera_status() == CAMERA_IN_FRONT)      return(LAWN4_BRUSH);
   city_scale scale = tt_screen->current_scale();
//      if (scale == ground_scale) return(LAWN4_BRUSH);//LAWN_SIDE_BRUSH);
   if (scale < ground_scale*3) return(LAWN1_BRUSH);
   if (scale < ground_scale*6) return(LAWN2_BRUSH);
   return(LAWN4_BRUSH);
};

BrushId City::water_brush_id() {
   city_scale scale = tt_screen->current_scale();
//      if (scale == ground_scale) return(LAWN_SIDE_BRUSH);
   if (scale < ground_scale*3) return(WATER1_BRUSH);
   if (scale < ground_scale*6) return(WATER2_BRUSH);
   return(WATER4_BRUSH);
};

void City::add_extra(Sprite *extra,
                     city_coordinate x, city_coordinate y) {
   Block *block = all_blocks.block_at_address(x,y);
   block->add_extra(extra);
   if (extra->pointer_to_followers() != NULL) {
      add_leader(extra);
   };
//   extra->set_priority_function_of_lly(TRUE);
};

void City::remove_extra(Sprite *extra,
                        city_coordinate x, city_coordinate y) {
   Block *block = all_blocks.block_at_address(x,y);
   block->remove_extra(extra);
   if (extra->pointer_to_followers() != NULL) {
      remove_leader(extra);
   };
//   extra->set_priority_function_of_lly(FALSE);
};

void City::remove_house(House *house) {
	city_coordinate x, y; 
   house->house_center(x,y);
   block_number block_x, block_y;
   block_address_at(x,y,block_x,block_y);
   all_blocks.remove_house(house,block_x,block_y);
   changed = TRUE;
};

void give_ground_coordinates(Sprite *sprite, TTRegion &region) {
   // re-compute coordinates to be relative to region where region is 0,0 to ideal_screen_width,ideal_screen_height
   int64 sprite_width = sprite->current_width(); // was city_coordinate prior to 070202
   int64 sprite_height = sprite->current_height();
   city_coordinate region_width = region.width();
   city_coordinate region_height = region.height();
   // re-compute coordinates to be relative to region where region is 0,0 to ideal_screen_width,ideal_screen_height
   sprite->set_size_and_location((city_coordinate) ((sprite_width*ideal_screen_width)/region_width),
                                 (city_coordinate) ((sprite_height*ideal_screen_height)/region_height),
                                 (city_coordinate) (((sprite->current_llx()-region.min_x)*(int64) ideal_screen_width)/region_width), // int64 new on 070202
                                 (city_coordinate) (((sprite->current_lly()-region.min_y)*(int64) ideal_screen_height)/region_height),
											TRANSLATING_CITY_TO_GROUND_COORDINATES);
};

void City::transfer_pictures(Floor *floor, TTRegion &region) {
	// changed on 190901 to ignore region since objects outside of region might be moving (or after sitting programmer might change some remote controls)
   Sprites *remaining = items;
//   TTRegion sprite_region;
	// new on 060699
//	TTRegion big_region = region;
//	big_region.grow_by(20,20); // too wasteful to transfer everything but a wide area around sprite is good
   while (remaining != NULL) {
      Sprite *sprite = remaining->first();
//      if (sprite->kind_of() == PICTURE) { // conditional removed on 060699
//         sprite->full_region(sprite_region);
//         if (sprite_region.overlaps(big_region)) {
//           Sprite *leader = sprite->pointer_to_leader();
//           if (leader != NULL) leader->remove_follower(sprite); // remove from old coordinate holder
				remove_item(sprite);
            give_ground_coordinates(sprite,region);
//            sprite->set_priority_function_of_lly(FALSE);
            floor->add_item(sprite);
				sprite->restore_floor_priority();
//         };
//      };
      remaining = remaining->rest();
   };
   city_coordinate x,y;
   region.region_center(x,y);
   Block *block = all_blocks.block_at_address(x,y);
   if (block == NULL) return; // possible?
   Sprites *extras = block->extras_in_region(region);
   if (extras != NULL) {
      remaining = extras;
      while (remaining != NULL) {
         Sprite *sprite = remaining->first();
//         Sprite *leader = sprite->pointer_to_leader();
//         if (leader != NULL) leader->remove_follower(sprite); // remove from old coordinate holder
         if (sprite->kind_of() == PICTURE) {
            give_ground_coordinates(sprite,region);
//            sprite->set_priority_function_of_lly(FALSE);
            floor->add_item(sprite);
            remove_extra(sprite,x,y);
         };
         remaining->set_first(NULL); // to release list memory
         remaining = remaining->rest();
      };
      delete extras; // just a list of NULLs
   };
};

// rename to nearest_avenue?
city_coordinate City::nearest_x_street(city_coordinate view_y) {
   block_number street = (block_number) (view_y/ideal_screen_width);
   return(street*ideal_screen_width+x_street_width(street)/2);
};

city_coordinate City::nearest_y_street(city_coordinate view_x) {
   block_number street = (block_number) (view_x/tt_ideal_block_width);
   return(street*tt_ideal_block_width+y_street_width(street)/2);
};

void City::nearest_lane(city_coordinate &x, city_coordinate &y, Direction direction) {
  // for two way traffic 
  city_coordinate intersection_x = nearest_y_street(x);
  city_coordinate intersection_y = nearest_x_street(y);
  city_coordinate x_distance = labs(intersection_x-x);
  city_coordinate y_distance = labs(intersection_y-y);
  if (x_distance < y_distance) { // fix x dimension since lesser change
     x = intersection_x+y_street_width(0)/4;
//	  if (tt_log_version_number > 14) x += y_street_width(0)/4; 
  } else {
     y = intersection_y+y_street_width(0)/4;
//	  if (tt_log_version_number > 14) y += y_street_width(0)/4;
  };
//  if (tt_log_version_number > 14) {
	  x = min(max(x,min_x()),max_x());
	  y = min(max(y,min_y()),max_y());
//  } else {
//	  switch (direction) {
//		  case SOUTH:
//			  x -= tile_width;
//			  break;
//		  case WEST:
//			  y -= tile_height;
//			  break;
//	  };
//  };
};

Block *City::block_at(block_number x, block_number y, boolean create) {
#if TT_DEBUG_ON
  if (x >= number_of_x_streets) {
     say_error(_T("Trying to build a house on a block outside the city."));
     x %= number_of_x_streets;
  };
  if (y >= number_of_y_streets) {
     say_error(_T("Trying to build a house on a block outside the city."));
     y %= number_of_y_streets;
  };
#endif
  return(all_blocks.block_at(x,y,create));
};

House *City::new_house_nearest(block_number x, block_number y, HouseStyle style) {
   changed = TRUE;
   Block *block = all_blocks.block_at(x,y,TRUE);
   House *house = block->build_house(style);
   if (house != NULL) return(house);
   short int lot_number; // not used here
   if (nearest_free_lot(x,y,lot_number)) {
      block = all_blocks.block_at(x,y,TRUE); // typically a different block
      return(block->build_house(style));
   };
   return(NULL);
};

boolean City::nearest_free_lot(block_number &x, block_number &y, short int &lot) {
   Block *block;
   short int block_x, block_y; // can range negative or outside city  
   for (short int i = 2; i <= max(number_of_x_streets,number_of_y_streets)*2; i += (short) 2) { // search square 3x3 then 5x5 ...
      // start in the block north of this one
      block_x = x;
      block_y = (short int) (y+i/2); 
      int k;
      for (k = 0; k < i/2; k++) { // right half of top row 
         if (valid_address(block_x,block_y)) {
            block = all_blocks.block_at((block_number) block_x,(block_number) block_y,TRUE);
            if (block->next_free_lot(lot)) {
               x = (block_number) block_x;
               y = (block_number) block_y;
               return(TRUE);
            };
         };
         block_x++;
      };
      for (k = 0; k < i; k++) { // right column
         if (valid_address(block_x,block_y)) {
            block = all_blocks.block_at((block_number) block_x,(block_number) block_y,TRUE);
            if (block->next_free_lot(lot)) {
               x = (block_number) block_x;
               y = (block_number) block_y;
               return(TRUE);
            };
         };
         block_y--;
      };
      for (k = 0; k < i; k++) { // bottom row
         if (valid_address(block_x,block_y)) {
            block = all_blocks.block_at((block_number) block_x,(block_number) block_y,TRUE);
            if (block->next_free_lot(lot)) {
               x = (block_number) block_x;
               y = (block_number) block_y;
               return(TRUE);
            };
         };
         block_x--;
      };
      for (k = 0; k < i; k++) { // left column 
         if (valid_address(block_x,block_y)) {
            block = all_blocks.block_at((block_number) block_x,(block_number) block_y,TRUE);
            if (block->next_free_lot(lot)) {
               x = (block_number) block_x;
               y = (block_number) block_y;
               return(TRUE);
            };
         };
         block_y++;
      };
      for (k = 0; k < i/2; k++) { // left half of top row 
         if (valid_address(block_x,block_y)) {
            block = all_blocks.block_at((block_number) block_x,(block_number) block_y,TRUE);
            if (block->next_free_lot(lot)) {
               x = (block_number) block_x;
               y = (block_number) block_y;
               return(TRUE);
            };
            block_x++;
         };
      };
   };
   return(FALSE); // town full
};

boolean City::valid_address(int block_x, int block_y) {
   return(block_x < number_of_x_streets && block_x >= 0 &&
          block_y < number_of_y_streets && block_y >= 0);
};

void City::center(city_coordinate &x, city_coordinate &y) {
  x = (city_coordinate) number_of_x_streets*tt_ideal_block_width/2;
  y = (city_coordinate) number_of_y_streets*ideal_screen_width/2;
};

city_coordinate City::x_street_width(long n) { // long was block_number prior to 010499
  if (n%3) {
     return(ideal_screen_width/4); 
  } else {
     return(ideal_screen_width/3); 
  };
};

city_coordinate City::y_street_width(long n) {
  if (n%4) {
     return(min(tt_ideal_block_width,tt_default_block_width)/20); // prior to 310399 this was ideal_screen_width/5
  } else {
     return(min(tt_ideal_block_width,tt_default_block_width)/12); // prior to 310399 this was ideal_screen_width/3
  };
};

void release_file_name_of_last_city_loaded() { // new on 140104 to fix a small leak
	if (file_name_of_last_city_loaded != NULL) delete [] file_name_of_last_city_loaded;
};

/*
void City::y_street_color(int street_number) {
   fg_setcolor(y_street_colors[street_number%number_of_y_street_colors]);
};

void x_street_color (int street_number) {
   fg_setcolor(x_street_colors[street_number%number_of_x_street_colors]);
};
*/


/*

	tt_loading_a_city = TRUE; // new on 181200 - moved on 191200
   while (type == BLOCK_MARKER) {
      load_block(stream,cookie_version_number,language,this);
      type = (unsigned char) stream.get();
   };
	boolean internal_error_occurred = FALSE;
	Sprites *city_items = NULL;
	if (cookie_version_number >= first_version_to_save_city_items) {
#if TT_DEBUG_ON
		long start = stream->tellg();
#endif
      if (tt_want_exception_breaks) {
		   city_items = load_items(stream,cookie_version_number,language);
   //		tt_loading_a_city = FALSE; // commented out on 260201 since this should be reset after loading programmer (since might be holding a tool)
      } else {
		   try {
			   city_items = load_items(stream,cookie_version_number,language);
   //			tt_loading_a_city = FALSE; // commented out on 260201 since this should be reset after loading programmer (since might be holding a tool)
		   } catch (...) {
			   tt_loading_a_city = FALSE;
			   if (handle_internal_error(S(IDS_INTERNAL_ERROR_LOADING_CITY,"Sorry but an internal error occurred while loading a city. Strange things may happen."),TRUE)) {
				   city_items = tt_items_loaded_so_far; // those loaded so far are put here for this purpose starting 120400
				   tt_items_loaded_so_far = NULL;
				   load_ended(); // added 150400
				   internal_error_occurred = TRUE;
			   } else {
				   throw(tt_global_picture);
			   };
		   };
      };
	   if (city_items != NULL) {
#if TT_DEBUG_ON
		   if (tt_debug_mode == 141100) {
			   tt_error_file() << (stream->tellg()-start) << " bytes in encoding of stuff outside and not associated with a block" << endl;
		   };
#endif
		   add_items(city_items);
		   city_items->recursively_propagate_changes();
		   city_items->activate();
	   };
   };
	if (!internal_error_occurred) {
#if TT_DEBUG_ON
		long start = stream->tellg();
#endif
		if (stream != NULL) tt_programmer->load(stream,cookie_version_number,language);
		if (cookie_version_number >= first_version_to_save_cubby_of_robot) {
			tt_queue = load_queue(stream,cookie_version_number,language);
#if TT_DEBUG_ON
			if (tt_debug_mode == 141100 && tt_queue != NULL) {
				tt_error_file() << (stream->tellg()-start) << " bytes in encoding of queue of " << tt_queue->length() << " waiting events." << endl;
				start = stream->tellg();
			};
#endif
#if TT_DEBUG_ON
			if (tt_debug_mode == 141100 && tt_running_robots != NULL) {
				tt_error_file() << (stream->tellg()-start) << " bytes in encoding of " << tt_running_robots->length() << " running or waiting robots." << endl;
			};
#endif
		};
		if (tt_programmer->status_when_dumped() == START_ON_THE_GROUND) {
			Sprites *remaining = city_items;
			while (remaining != NULL) {
				Sprite *sprite = remaining->first();
				city_coordinate llx = sprite->current_llx();
				city_coordinate lly = sprite->current_lly();
				city_coordinate width = sprite->current_width();
				city_coordinate height = sprite->current_height();
				tt_programmer->translate_ground_to_city_coordinates(width,height,llx,lly);
				sprite->set_size_and_location(width,height,llx,lly,LOADING_GEOMETRY); // added LOADING_GEOMETRY on 080202 so numbers and text pads don't recalculate size
				remaining = remaining->rest();
			};
		};
	};
	if (city_items != NULL) {
		city_items->remove_all();
		delete city_items;
	};
	tt_loading_a_city = FALSE; // moved here on 260201
	load_ended();
#if TT_DEBUG_ON
	if (tt_debug_mode == 260900) {
		tt_error_file() << "Took " << (timeGetTime()-start) << "ms to load city." << endl;
		LARGE_INTEGER high_res_stop;
		LARGE_INTEGER high_res_frequency;
		QueryPerformanceCounter(&high_res_stop);
		QueryPerformanceFrequency(&high_res_frequency);
		int64 duration = (1000000*(high_res_stop.QuadPart - high_res_start.QuadPart))/high_res_frequency.QuadPart;
		tt_error_file() << (long) duration << " microseconds" << endl;
//		done_waiting_for_load();
	};
#endif
	return(TRUE);
*/
