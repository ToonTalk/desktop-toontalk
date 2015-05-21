// Copyright (c) 1992-2006.  Ken Kahn, Animated Programs, All rights reserved.  

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif   
#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif   
#if !defined(__TT_PAD_H)   
#include "pad.h"
#endif   
#if !defined(__TT_ROBOT_H)   
#include "robot.h"
#endif  
#if !defined(__TT_LOG_H)   
#include "log.h"
#endif   
#if !defined(__TT_THOUGHT_H)   
#include "thought.h"
#endif   
#if !defined(__TT_CUBBY_H)   
#include "cubby.h"
#endif   
#if !defined(__TT_BIRD_H)   
#include "bird.h"
#endif   
#if !defined(__TT_NUMBER_H)   
#include "number.h"
#endif   
#if !defined(__TT_TEXT_H)   
#include "text.h"
#endif
#if !defined(__TT_SOUND_H)
#include "sound.h"
#endif
#if !defined(__TT_PICTURE_H)   
#include "picture.h"
#endif   
#if !defined(__TT_BOMB_H)   
#include "bomb.h"
#endif   
#if !defined(__TT_TRUCK_H)   
#include "truck.h"
#endif   
#if !defined(__TT_FLOOR_H)   
#include "floor.h"
#endif   
#if !defined(__TT_EVENT_H)   
#include "event.h"
#endif   
#if !defined(__TT_PRGRMMR_H)
#include "prgrmmr.h"
#endif
#if !defined(__TT_MARTIAN_H)
#include "martian.h"
#endif
#if !defined(__TT_PUZZLE_H)
#include "puzzle.h"
#endif
#if !defined(__TT_WINMAIN_H)
#include "winmain.h"
#endif   
//#include "screen.h"
#if !defined(__TT_HELP_H)
#include "help.h"
#endif
#if !defined(__TT_JAVA_H)
#include "java.h"
#endif
#if !defined(__TT_MOUSE_H)
#include "mouse.h"
#endif
#if TT_DIRECT_INPUT
#if !defined(__TT_INPUT_H)
#include "input.h"
#endif
#endif
#if !defined(__TT_UTILS_H)
#include "utils.h" // for sound stuff
#endif
#if TT_XML
#if !defined(__TT_XML_H)
#include "xml.h"
#endif
#endif

#include <stdlib.h> // for min, max, range
//#include <strstrea.h> commented out 210703
#include <string.h>
//#include <ctype.h> // for tolower
#include <stdlib.h>
#if !TT_32
#include <mmsystem.h> // for timeGetTime
#endif

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

string page_full_file_name(ascii_string file_name, ascii_string user_file_name, boolean check_paths, ascii_string extension);

void main_notebook_changed();
//long pad_count = 0;

// if changed update help.cpp
enum NotebookHistory {NOTEBOOK_KEYBOARD_FORWARD=0,
							 NOTEBOOK_KEYBOARD_BACKWARD,
							 NOTEBOOK_KEYBOARD_NUMBER,
							 NOTEBOOK_RIGHT_PAGE_ADDED,
							 NOTEBOOK_PAGE_REMOVED,
							 NOTEBOOK_LEFT_PAGE_ADDED,
							 NOTEBOOK_RECEIVED_PAGE_TOKEN,
							 NOTEBOOK_RECEIVED_PAGE_NUMBER,
							 NOTEBOOK_CALLED};

enum NoteBookStates {NOTEBOOK_INERT, NOTEBOOK_FLYING, 
							NOTEBOOK_BEGIN_PAGE_TURN_FORWARD,
							NOTEBOOK_END_PAGE_TURN_BACKWARD,
							NOTEBOOK_END_PAGE_TURN_FORWARD, // or begin turn backward
							NOTEBOOK_START_FLYING,
							NOTEBOOK_SETTLE_DOWN}; 

enum NoteBookPageFlipState {FLIP_TOWARD_NEXT,FLIP_TOWARD_PREVIOUS};

//void add_right_page_as_follower_callback(void *n) {
//	((Notebook *) n)->add_right_page_as_follower();
//};

#if TT_XML
inline boolean using_xml() {
   return(tt_dump_as_xml); // for now - should consider log version number (and more?)
};
#else
inline boolean using_xml() {
   return(FALSE);
};
#endif

Notebook::Notebook(city_coordinate x, city_coordinate y,
						 long initial_priority,
						 ascii_string initial_file_name, // loads this (but not if NULL and then set_file_name)
						 city_coordinate width,
						 city_coordinate height,
//						 string initial_comment,
						 Sprite *initial_comment,
						 Pages *initial_pages,
                   Picture *picture,
						 int version_number) :
	Sprite(NOTEPAD_SPRITE,x,y,PROGRAM_PAD,
		  0,ground_scale,// FALSE,
		  width,height),
	old_pages(initial_pages),
//	comment(NULL),
	page_number(-1),
	recent_page_number(1),
   next_page_number(0),
	left_page(NULL),
	right_page(NULL),
//	saved_page(NULL),
//	saved_encoding(NULL),
//	pad_number(pad_count++),
	saved_page_number(-1),
//	saved_action_status(NULL),
	page_number_offset(0),
	//cache(NULL), // noticed obsolete on 171102
	//cache_size(0),
	page_flipping(NULL),
	overlay_status(OVERLAY_OFF),
   page_number_item(NULL), // wasn't initialized prior to 181102
	running_to_hand_flag(FALSE),
	picture(picture),
	add_to_number(DONT_ADD_TO_PAGE),
	original(TRUE),
//   called_since_last_release(FALSE), // noticed obsolete on 171102
   language(tt_language), // default
   favorite_width(-1),
   built_in_notebook(USER_NOTEBOOK),
//   saved_by(NULL), 
	version_number(version_number),
	label(NULL),
	last_key_for_label(FALSE),
	changed_since_last_save(FALSE), // was TRUE prior to 161202 but that doesn't make sense and caused built-in notebooks to dump contents
	default_favorite_llx(ideal_screen_width/2),
	default_favorite_lly(tile_height),
#if TT_XML
   pages(NULL),
   pages_count(0),
   left_page_is_copy(FALSE),
   right_page_is_copy(FALSE),
//	xml_location_of_last_good_full_save(0), -- obsolete 160204
#endif
   id(tt_notebook_id_counter++),
   file_name(NULL), // initialization new on 210802
   loaded(FALSE), // variable new on 220802
	left_page_number_when_displayed(1),
#if !TT_ALPHA_FEATURE
	media_file_names(NULL), 
#endif
	page_being_defined(NULL) { 
   //left_page_active(FALSE),
   //right_page_active(FALSE)
//	picture_copy(NULL) {
//	set_priority_function_of_lly(FALSE);
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	set_priority(initial_priority);
//	file_name = copy_ascii_string(initial_file_name);
   set_file_name(initial_file_name); // rewritten on 210802 -- caller now copies if need be
	comment = initial_comment;
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	if (old_pages == NULL) {
		old_pages = new Pages(NULL,0); // initial 1 blank page
		if (file_name != NULL) {
			if (comment == NULL) {
				Text *text_comment = NULL;
				if (strcmp(file_name,"bok") == 0) {
					text_comment = new Text();
					text_comment->set_text(S(IDS_BOOK_DESCRIPTION1));
					string label_location = ini_entry("Directories","MainNotebookLabel",FALSE); 
					// too early for recording - new on 250901 - was (tt_log_version_number > 27)
					if (label_location != NULL) { // new on 010601
						string cannonical_label_location = expand_asterisk(label_location);
						FileNameStatus name_status;
						string label_file_name = existing_file_name(name_status,cannonical_label_location);
						if (label_file_name != NULL) {
							boolean aborted = FALSE; // new on 200105
							Sprite *sprite = sprite_from_file_name(label_file_name,aborted);
#if TT_NO_ROBOT_TRAINING
						   // don't show user name for run-time only version unless showing a demo -- new on 111205
							if (replaying())
#endif
							if (sprite != NULL) { // new on 120601
								Text *text = make_label(tt_user_name,tt_black,FALSE);
								text->set_size_and_location_to_those_of(sprite);
								sprite->add_follower(text);
							};
							set_label(sprite);
							delete [] label_file_name;
						};
						delete [] cannonical_label_location;
					} else {
#if TT_NO_ROBOT_TRAINING
						// don't show user name for run-time only version unless showing a demo -- new on 111205
						if (replaying())
#endif
						set_label_string(tt_user_name); // added on 110699 so user name is on the main notebook
					};
				} else if (strcmp(file_name,"2") == 0) {
					text_comment = new Text();
					text_comment->set_text(S(IDS_BOOK_DESCRIPTION2));
					set_label_string(S(IDS_MOVIES_NOTEBOOK)); // these added on 210100 since it is lost sometimes???
				} else if (strcmp(file_name,"4") == 0) {
					text_comment = new Text();
					text_comment->set_text(S(IDS_BOOK_DESCRIPTION3));
					set_label_string(S(IDS_SENSORS_NOTEBOOK));
				} else if (strcmp(file_name,"6") == 0) {
					text_comment = new Text();
					text_comment->set_text(S(IDS_BOOK_DESCRIPTION4));
					set_label_string(S(IDS_EXAMPLES_NOTEBOOK));
				} else if (strcmp(file_name,"8") == 0) { 
					text_comment = new Text();
					text_comment->set_text(S(IDS_BOOK_DESCRIPTION6));
					set_label_string(S(IDS_SOUNDS_NOTEBOOK));
				} else if (strcmp(file_name,"10") == 0) {
					text_comment = new Text();
					text_comment->set_text(S(IDS_BOOK_DESCRIPTION7));
					set_label_string(S(IDS_OPTIONS_NOTEBOOK));
				}; // worth adding new Numbers notebook like this??
				comment = text_comment;
			};
#if TT_DEBUG_ON
			add_to_log_if_interesting_error();
			millisecond start = timeGetTime();
#endif
			load(); // file_name);
#if TT_DEBUG_ON
			if (tt_debug_mode == 789) {
				tt_error_file() << "Took " << (timeGetTime()-start)
								<< "ms to load notebook " << file_name << endl;
			};
#endif
		};
	};
//	if (initial_comment != NULL) {
//		comment = copy_string(initial_comment);
//	};
	if (!tt_loading_robot_notebook) { 
		update_display(); // so it has a current_image for the following to use
		go_to_page(1);
	};
   set_show_all(FALSE); // new on 200802 - kind of obvious so not checked but helps generate better XML
#if TT_DEBUG_ON
   if (tt_debug_mode == 1717) {
      tt_error_file() << "Making pad#" << debug_counter << " ";
//      top_level_describe(tt_error_file(),INDEFINITE_ARTICLE); -- caused recursion that used up memory - 150103
      tt_error_file() << " on frame " << tt_frame_number << endl;
   };
   add_to_log_if_interesting_error();
#endif
};

void Notebook::release_all_references() { // abstracted on 050204 (i.e. moved from Notebook::prepare_for_deletion) -- removed Sprite *by on 060204
	if (!tt_resetting_or_destroying_city && !about_to_quit() && built_in_notebook == MAIN_NOTEBOOK) {
		return; // new on 090105 to protect the main notebook
	};
	if (releasing_all_references) { // new on 230305
		return;
	};
	boolean saved_releasing_all_references = releasing_all_references;
	releasing_all_references = TRUE;
	boolean am_preparing_for_deletion = preparing_for_deletion;
	preparing_for_deletion = TRUE; // in case any of the following recursively tries to delete this - new on 060204
	if (file_name != NULL) {
      delete [] file_name;
      file_name = NULL;
   };
	if (comment != NULL) {
//		delete [] comment;
		comment->destroy();
		comment = NULL; // new on 261204 since was double destroyed
	};
// following are followers so should be dealt with in sprite:prepare_for_deletion
//   if (left_page != NULL) left_page->destroy();
//   if (right_page != NULL) right_page->destroy();
//	flush_cache();
   remove_pages();
//	if (saved_page != NULL) {
//		delete saved_page;
//		delete [] saved_encoding;
//	};
// are followers so will be taken care of by Sprite
/*
	if (left_page != NULL) {
		remove_follower(left_page);
		delete left_page;
	};
	if (right_page != NULL) {
		remove_follower(right_page);
		delete right_page;
	};
*/
	if (page_flipping != NULL) {
		page_flipping->destroy();
		page_flipping = NULL; // new on 100204 just to be safe
	};
	if (!tt_resetting_or_destroying_city && picture != NULL && !picture->global()) { 
		// on 181199 re-ordered this so if shutting down won't check if global
		// about_to_quit replaced tt_shutting_down on 211200 -- replaced with tt_resetting_or_destroying_city on 100603
		// what about tt_reclaiming_memory?
		picture->remote_notebook_deleted(this);
		picture = NULL; // new on 261204
	};
//	if (picture_copy != NULL) { // new on 121199
//		picture_copy->destroy();
//	};
//   if (saved_by != NULL && !tt_shutting_down) saved_by->destroy(); // if zero ref count // commented out on 171102
#if TT_XML
   if (pages != NULL) {
		// commented out on 140104 since caused a leak
//      if (left_page != NULL && !left_page_is_copy) remove_follower(left_page,FALSE); 
		// if a copy then Sprite::prepare_for_deletion will deal fine with it 
//      if (right_page != NULL && !right_page_is_copy) remove_follower(right_page,FALSE); // added FALSE as args on 171203 - OK?
      for (int i = 0; i < pages_count; i++) {
         if (pages[i] != NULL) {
				pages[i]->destroy(); // abstracted on 260803
         };
      };
      delete [] pages;
      pages = NULL;
		pages_count = 0; // new on 090204
   };
#endif
	if (!am_preparing_for_deletion) { // restore things if set above
		preparing_for_deletion = FALSE;
	};
#if !TT_ALPHA_FEATURE
	if (media_file_names != NULL) {
		delete [] media_file_names;
		media_file_names = NULL;
	};
#endif
	// following new on 030105 since Sprite::release_all_references() will call ::release_all_references() even if ref count protects it
	if (left_page != NULL) {
		remove_follower(left_page);
		left_page->destroy();
		left_page = NULL;
	};
	if (right_page != NULL) {
		remove_follower(right_page);
		right_page->destroy();
		right_page = NULL;
	};
	releasing_all_references = saved_releasing_all_references; 
	// restore setting so the following doesn't get confused and think it is in a recursive call
	Sprite::release_all_references(); // new on 020504
};

//void Notebook::prepare_for_deletion() {
//	if (preparing_for_deletion) return;
//	preparing_for_deletion = TRUE; // new on 300104 to deal with recursive calls
//#if TT_DEBUG_ON
//   if (tt_debug_mode == 1717 || tt_debug_target == debug_counter) {
//      tt_error_file() << "Preparing to delete pad#" << debug_counter << " ";
//      describe_type(tt_error_file());
//      tt_error_file() << " on frame " << tt_frame_number << endl;
//   };
//#endif
////	release_all_references(); // removed by on 060204 -- commented out on 020504 since done by Sprite::prepare_for_deletion now
//
////	prepared_for_deletion_flag = FALSE; // restore so the following works OK -- replaced by TRUE in the following on 060204
//	Sprite::prepare_for_deletion();
//};

void Notebook::remove_pages() {
   if (old_pages != NULL) { // delete private copies
      Pages *remaining = old_pages;
      while (remaining != NULL) {
         Pages *next_remaining = remaining->rest();
         remaining->set_rest(NULL);
         delete remaining;
         remaining = next_remaining;
      };
		old_pages = NULL; // just in case (new on 170202)
//		delete pages; // for large notebooks blew the stack
	};
};

//void Notebook::flush_cache() { // noticed obsolete on 171102
//	if (cache != NULL) {
////		int size = pages->length(); // can be out of date (and even longer now)
//		for (short int i = 0; i < cache_size; i++) {
//			 if (cache[i] != NULL) {
//				 cache[i]->destroy();
////				 if (cache[i]->ok_to_delete()) {
////					 delete cache[i];
////				 };
//			 };
//		};
//		delete [] cache;
//		cache = NULL;
//      cache_size = 0;
//	};
//};

boolean Notebook::ok_to_delete() {
#if TT_DEBUG_ON
   if (tt_debug_mode == 1717) {
      tt_error_file() << "Checking whether to delete pad#" << debug_counter << " ref count is now " << ref_count << endl;
   };
	if (debug_counter == tt_debug_target) {
		log("Checking whether ok to delete notebook debug target.");
	};
#endif
	// don't destroy the main notebook (or even ref count it)
//	if ((tt_shutting_down && !tt_reclaiming_memory) || (tt_toolbox != NULL && pointer_to_toolbox_notebook() == this)) {
	if (!(tt_reclaiming_memory && about_to_quit()) && built_in_notebook == MAIN_NOTEBOOK && !inside_vacuum()) { 
		// added inside_vacuum condition on 070704 since that is handled differently now
		// && (tt_toolbox != NULL && pointer_to_toolbox_notebook() == this)) {
		// commented out part of the conditional above on 260803 since causes an infinite recursion when loading main notebook now
		// built_in_notebook == MAIN_NOTEBOOK new on 260104
		// and the main notebook is not given a max_long ref count
		// rewritten 190103 to make more sense
		return(FALSE);
	};
	boolean ok = Sprite::ok_to_delete();
   if (ok) {
		if (prepared_for_deletion()) {
			return(TRUE); // new on 280300
		} else if (!showing_on_a_screen()) { // added ok && on Sept 7 '98
//			go_to_page(-1,FALSE); // commented out on 150201
			remove_from_floor(FALSE); // since floor may be destroyed
		};
		if (picture != NULL) { // new on 241000
			if (picture->pointer_to_remote_notebook() != NULL) { // picture remembers me so
				picture->just_set_remote_notebook(NULL); // make him forget
			};
			picture = NULL; // rewritten 080203 (approximately) so this happens regardless of the above conditional
		};
	} else if (ref_count == 1 && tt_toolbox != NULL && toolbox_tool(this)) { // new on 070704
		if (tt_reincarnate_tools) {
			Notebook *notebook = new Notebook(0,0,0,copy_ascii_string("bok"));
			set_toolbox_notebook(notebook);
			initialize_toolbox_notebook(TRUE);
		} else {
			set_toolbox_notebook(NULL);
		};
   } else if (picture != NULL && inside_vacuum()) { // new on 070602
      // if being destroyed and inside a vacuum then if picture is holding on then not really inside a vacuum anymore
      set_inside_vacuum(FALSE);
	};
	return(ok); // 241000 && (picture == NULL || picture->slated_for_deletion())); // plugged a major leak by adding second disjunct on 241000
};

Sprite *Notebook::copy(boolean also_copy_offsets) {
	Notebook *copy = NULL;
	if (picture != NULL && !picture->global()) {
		copy = picture->new_remote_notebook();
		copy->go_to_page(right_page_number());
		return(copy);
	};
	Pages *pages_copy = NULL; // could rationalize this away some day
#if TT_XML
//   if (using_xml()) { // commented out on 270902
      load();
      compute_pages(); // just in case
      remove_pages(); // don't need old version anymore
//   };
#else
	// no point doing this if did the above - introduced #else on 260103
	if (old_pages != NULL) { // should be NULL if XML stuff above ran
		pages_copy = old_pages->copy();
   };
#endif
   Sprite *comment_copy = NULL;
	if (comment != NULL) comment_copy = comment->copy();
#if TT_XML
//   if (using_xml()) { // commented out on 270902
      copy = new Notebook(llx,lly,//center_x,center_y,
							     current_priority-1,
							     NULL, // don't want to load it
							     width,height,comment_copy,NULL,NULL,version_number);
      copy->initialize_pages(pages,pages_count);
      //if (file_name != NULL) { 
		// commented out on 150304 since seems to cause problems saving notebooks in notebooks (and why did it ever make sense?)
      //   copy->set_file_name(copy_string(file_name)); // forgot copy-> prior to 181102
      //};
//   };
#endif
   if (copy == NULL) { // XML above didn't run
	   copy = new Notebook(llx,lly,//center_x,center_y,
							     current_priority-1,
							     copy_string(file_name), // copy_string new on 220802
							     width,height,comment_copy,pages_copy,NULL,version_number);
   };
   copy->set_version_number(version_number);
   copy->set_language(language);
//	tt_notebooks->insert_at_end_if_not_member(copy); // member test irrelevant
	copy->go_to_page(right_page_number());
//	if (saved_width != 0) {
//		copy->set_saved_size(saved_width,saved_height);
//	};
//	if (picture != NULL && !picture->global()) {
//		picture->remote_notebook_added(copy);
//	};
//	copy->set_picture(picture);
	copy->set_original(FALSE);
//   copy->set_priority_fixed(priority_fixed_flag);
	if (label != NULL && (file_name == NULL || strcmp(file_name,"bok") != 0)) {
		// on 110699 added conditional that a copy of your main notebook isn't labeled
		copy->set_label(label->copy(TRUE)); // TRUE is new on 260103
	};
	//if (blank && !tt_copy_restores_pre_blank) { // new on 280900
	//	copy->become_blank();
	//};
	copy->set_built_in_notebook(built_in_notebook); // new on 050201
   finish_making_copy(copy,also_copy_offsets,FALSE); // FALSE added on 300103 so left and right page aren't copied as followers
#if TT_XML
//   if (using_xml()) {
      copy->switch_contents();
//   };
#endif
//#if !TT_ALPHA_FEATURE
//	if (set_media_file_names != NULL) {
//		copy->set_media_file_names(copy_string(media_file_names));
//	};
//#endif
	return(copy);
};

void Notebook::set_picture(Picture *new_picture) {
#if TT_DEBUG_ON
	if (new_picture != NULL && new_picture->pointer_to_controlee() != NULL) {
		log("Notebook shouldn't be of the remote control for the looks of something.");
	};
	if (tt_debug_target == debug_counter) {
		log("Debug target notebook getting picture.");
	};
	if (new_picture != NULL && tt_debug_target == new_picture->debug_counter) {
		log("Debug target picture getting notebook.");
	};
#endif
	picture = new_picture;
	if (new_picture != NULL && !new_picture->global()) {
		new_picture->remote_notebook_added(this);
	};
	// update current page in case it contains remotes
	if (right_page != NULL) {
		short int saved_page_number = left_page_number();
		go_to_a_blank_page();
		go_to_page(saved_page_number);
	};
};

void Notebook::new_file_name(int number) {
	if (file_name != NULL) delete [] file_name;
	file_name = new char[12];
	//output_string_stream file_name_stream(file_name,12);
	//file_name_stream << number%1000; // WHY!!??!!
	//file_name_stream.put('\0');
	itoa(number%1000,file_name,10); // rewritten on 230104
//	int count = file_name_stream.pcount();
//	file_name = file_name_stream.str();
//	file_name[count] = NULL; // terminate the string
   original = TRUE;
	changed_since_last_save = TRUE; // since was never saved - new on 280204 to deal with time travel where a segment boundary occurs
	// after this was dropped but before it was added as a page
};

void Notebook::generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection) {
	if ((!did(NOTEBOOK_KEYBOARD_FORWARD) && tt_help_counter == 0)
		 || tt_help_counter == 1) {
		tt_help_history_action = NOTEBOOK_KEYBOARD_FORWARD;
//		if (tt_help_counter == 0) {
			describe_pointing(message,tool_in_hand,selection);
//		};
		message << S(IDS_BOOK_KEY_FORWARD_HELP);
	} else if ((!did(NOTEBOOK_KEYBOARD_BACKWARD) && tt_help_counter == 0)
				  || tt_help_counter == 2) {
		tt_help_history_action = NOTEBOOK_KEYBOARD_BACKWARD;
		message << S(IDS_BOOK_KEY_BACKWARD_HELP);
		if (tt_help_counter == 0 && left_page_number() == 1) {
			message << S(IDS_BOOK_KEY_BACKWARD_HELP2);
		} else {
			message << S(IDS_BOOK_KEY_BACKWARD_HELP3);
		};
		message << S(IDS_BOOK_KEY_BACKWARD_HELP4);
	} else if ((!did(NOTEBOOK_KEYBOARD_NUMBER) && tt_help_counter == 0)
				  || tt_help_counter == 3) {
		tt_help_history_action = NOTEBOOK_KEYBOARD_NUMBER;
		message << S(IDS_BOOK_KEY_NUMBER_HELP);
	} else if ((!did(NOTEBOOK_RIGHT_PAGE_ADDED) && tt_help_counter == 0) 
				  || tt_help_counter == 4) {
		tt_help_history_action = NOTEBOOK_RIGHT_PAGE_ADDED;
		message << S(IDS_BOOK_RIGHT_PAGE_ADDED_HELP)
				  << S(IDS_BOOK_RIGHT_PAGE_ADDED_HELP2)
				  << S(IDS_BOOK_RIGHT_PAGE_ADDED_HELP3)
				  << S(IDS_BOOK_RIGHT_PAGE_ADDED_HELP4);
		if (tt_help_counter == 0 && right_page != NULL) {
			message << S(IDS_BOOK_RIGHT_PAGE_ADDED_HELP5);
		};
		} else if ((!did(NOTEBOOK_PAGE_REMOVED) && tt_help_counter == 0) 
				  || tt_help_counter == 5) {
		tt_help_history_action = NOTEBOOK_PAGE_REMOVED;
		message << S(IDS_BOOK_PAGE_REMOVED_HELP)
				  << S(IDS_BOOK_PAGE_REMOVED_HELP2)
				  << S(IDS_BOOK_PAGE_REMOVED_HELP3);
	} else if ((!did(NOTEBOOK_LEFT_PAGE_ADDED) && tt_help_counter == 0) 
				  || tt_help_counter == 6) {
		tt_help_history_action = NOTEBOOK_LEFT_PAGE_ADDED;
		message << S(IDS_BOOK_LEFT_PAGE_ADDED_HELP)
				  << S(IDS_BOOK_LEFT_PAGE_ADDED_HELP2)
				  << S(IDS_BOOK_LEFT_PAGE_ADDED_HELP3);
		if (tt_help_counter == 0 && left_page == NULL) {
			message << S(IDS_PAGE_BEGIN_SENTENCE) << left_page_number() << S(IDS_BOOK_LEFT_PAGE_ADDED_HELP4);
		};
	} else if ((!did(NOTEBOOK_RECEIVED_PAGE_TOKEN) && tt_help_counter == 0)
				  || tt_help_counter == 7) {
		tt_help_history_action = NOTEBOOK_RECEIVED_PAGE_TOKEN;
		if (tt_help_counter == 0 && left_page != NULL &&
          left_page->kind_of() == TEXT) {
			message << S(IDS_BOOK_PAGE_TOKEN_HELP) << left_page_number()
					  << S(IDS_BOOK_PAGE_TOKEN_HELP2);
         string text_string;
         long text_string_length;
         left_page->current_text(text_string,text_string_length);
         message << '"';
         if (text_string_length > 0) {
            string text_copy = copy_string(text_string,text_string_length);
            for (int i = 0; i < text_string_length; i++) {
               if (new_line(text_copy[i])) text_copy[i] = ' '; // prior to 090802 was  == '\r'
            };
            message.write(text_copy,text_string_length);
            delete [] text_copy;
         };
         message << '"';
//			left_page->describe(message,DEFINITE_ARTICLE);
			message << S(IDS_BOOK_PAGE_TOKEN_HELP3)
					  << S(IDS_BOOK_PAGE_TOKEN_HELP4);
		} else {
			message << S(IDS_BOOK_PAGE_TOKEN_HELP5);
		};
		message << S(IDS_BOOK_PAGE_TOKEN_HELP6)
				  << S(IDS_BOOK_PAGE_TOKEN_HELP7);
	} else if ((!did(NOTEBOOK_RECEIVED_PAGE_NUMBER) && tt_help_counter == 0)
				  || tt_help_counter == 8) {
		tt_help_history_action = NOTEBOOK_RECEIVED_PAGE_NUMBER;
		message << S(IDS_BOOK_PAGE_NUMBER_HELP);
	} else if ((!did(NOTEBOOK_CALLED) && tt_help_counter == 0)
				  || tt_help_counter == 9) {
		tt_help_history_action = NOTEBOOK_CALLED;
		message << S(IDS_BOOK_CALLED_HELP);
	} else if ((!did(TYPE_HELP_GIVEN) && tt_help_counter == 0)
					|| tt_help_counter == 10) {
//		tt_help_history_action = TYPE_HELP_GIVEN;
		message << S(IDS_BOOK_HELP_GIVEN_HELP)
				  << S(IDS_BOOK_HELP_GIVEN_HELP2);
		no_help_interruptions();
		user_did(PROGRAM_PAD,TYPE_HELP_GIVEN);
	};
};

void Notebook::describe(output_stream &message, Article ) {
//	if (infinite_stack() && english()) {
//		print_article(article,message,S(IDS_STACK_OF));
//		article = PLURAL_ARTICLE;
//	};
   message << S(IDS_BOOK_CONTAINING);
//	print_article(article,message,S(IDS_BOOK_CONTAINING));
	// following called pages->length() prior to 250299
	message << space() << S(IDS_CONTAINING) << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << last_non_blank_page_number() << S(IDS_PAGES);
	if (comment != NULL && !alternative_language_spoken()) {
		string comment_string;
		long comment_length;
		comment->current_text(comment_string,comment_length);
		if (comment_length > 0) {
			message << end_sentence() << comment_string;
		};
	} else if (picture != NULL) {
		message << end_sentence() << S(IDS_IT_CONTAINS_REMOTE);
		picture->describe_type(message,DEFINITE_ARTICLE);
	} else if (file_name != NULL && !infinite_stack() &&  
		// on 270999 added && !infinite_stack() so notebooks in notebooks aren't described by file name
				  built_in_notebook == USER_NOTEBOOK) { // added 280999
		message << end_sentence() << S(IDS_BOOK_DESCRIPTION5) << file_name;
	};
	if (tt_toolbox != NULL && this == tt_toolbox_notebook) { // prior to 150704 was pointer_to_toolbox_notebook()) {
		message << end_sentence() << S(IDS_MAIN_BOOK_DESCRIPTION);
	};
};

void Notebook::update_display(const UpdateStatus update_status) {
	if (overlay_status != OVERLAY_OFF) {
		set_clean_status(DIRTY); 
	};
	Sprite::update_display(update_status);
};

void Notebook::display(SelectionFeedback selection_feedback,
							  boolean followers_too, TTRegion *region) {
#if TT_DEBUG_ON
	debug_log(debug_counter,"Displaying notebook that is a debug target.");
#endif
	if (!visible()) return; // new on 191200 - earlier invisible notebooks displayed page numbers
	if (blank) followers_too = FALSE; // new on 220500
//	if (rescale_if_camera_above(selection_feedback,followers_too,region)) return; // made obsolete on 230804
	if (pointer_to_top_of_stack() != NULL) { // new on 260500
		pointer_to_top_of_stack()->display(selection_feedback,followers_too,region);
		return;
	};
	int new_page_number = (recent_page_number-1)/2;
	if (page_number != new_page_number && recent_page_number != 1 && next_page_number == 0 && page_flipping == NULL) {
		// new on 060705 since may have been page flipped offscreen by a robot -- issue 682
		// is set to 1 when not used so ignore that 
		// and ignore if flip is going on, e.g. next_page_number != 0
		page_number = new_page_number;
		recent_page_number = 1;
		switch_contents();
	};
	if (left_page_number_when_displayed > 0 && !blank) { 
		finish_switching_contents();
	};
	Sprite::display(selection_feedback,followers_too,region);
	if (blank) return; // new on 220500
#if TT_DEBUG_ON
	millisecond start_time = timeGetTime();	
#endif
	// following doesn't pay attention to region
	city_coordinate number_width = width/5;
	city_coordinate number_height = height/5;
	if (right_page_number() >= 100 && label == NULL) { // 3 digits
		number_width = (2*number_width)/3;
		number_height = (2*number_height)/3;
	};
	city_coordinate delta_x = 0;
	city_coordinate delta_y = 0;
	if (want_feedback(selection_feedback)) {
		delta_x = selection_delta_x();
		delta_y = selection_delta_y();
	};
	city_coordinate left_page_number_llx = llx+width/10+delta_x;
	city_coordinate right_page_number_llx = llx+(6*width)/10+delta_x;
	city_coordinate page_number_lly = lly+number_height+delta_y;
	if (picture != NULL && picture != tt_global_picture) { // second condition added 121104
		boolean new_label = FALSE; // new on 090102
		if (label == NULL && number_height > minimum_height && number_width > minimum_width*2) { // *2 experiment on 150704
			//  new on 080399 - second condition added 120101 -- third added 150704
			Picture *picture_copy = (Picture *) picture->shallow_copy(); // was just copy() prior to 260399
			picture_copy->unflip(TRUE);
//			label->set_size_and_location(number_height,number_height,center_x()-number_height/2,lly+number_height/10); 
			// copied here on 120101 so good size before adding as follower
			set_label(picture_copy);
			new_label = TRUE;
		};
		left_page_number_llx -= (4*width)/100; // push page number towards left hand side
		if (right_page_number() < 10 && label == NULL) {
			right_page_number_llx += width/10;
		};
		if (new_label) { // was label != NULL prior to 090102
			label->set_size_and_location(number_height,number_height,center_x()-number_height/2,lly+number_height/10,TO_FIT_INSIDE); 
			// tried adding UPDATING on 090102 but broken notebook labels
		};
	} else if (label != NULL) {
		label->set_visible(TRUE);
//		if (right_page_number() > 9) {
			number_width /= 3;
//		} else {
//			number_width /= 2;
//		};
		left_page_number_llx -= width/20; // push page number towards left hand side
		right_page_number_llx += (18*width)/100; // was 23/100
		if (right_page_number() < 10) {
			right_page_number_llx += width/15;
		};
		// leave room for 3 digits on both sides
		// label shrunk 2 number widths on 160499 so that end of Pictures notebook looks OK
		if (!clean() || !label->clean()) { // condition new on 221299
			update_textual_label_size_and_location();
		};
	};
	if ((leader == NULL || leader->kind_of() != PROGRAM_PAD) &&
		  number_height > ideal_horizontal_units(6)) { // updated on 241199 *tt_screen->one_pixel_in_x_city_units()) {
		tt_screen->place_number(left_page_number(),
										left_page_number_llx,
										page_number_lly,
										number_width,number_height);
		tt_screen->place_number(right_page_number(),
										right_page_number_llx,
										page_number_lly,
										number_width,number_height);
	};
	switch (overlay_status) {
		case OVERLAY_FINISHING:
			overlay_status = OVERLAY_OFF; // last time
			// and the following
		case OVERLAY_STARTED:
			page_flipping->update_display();
			page_flipping->set_scales(x_scale,y_scale); // same scale as notebook
			page_flipping->move_to(llx+width-page_flipping->true_width(),lly);
			page_flipping->display();
			break;
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 3) {
		tt_error_file() << (timeGetTime()-start_time) << " ms to display extra PROGRAM_PAD stuff" << endl;
	};
#endif
};

void Notebook::update_textual_label_size_and_location() {
	if (label != NULL && picture == NULL) {
		city_coordinate number_width = width/15;
		city_coordinate number_height = height/5;
		label->set_size_and_location(width-8*number_width,number_height,llx+number_width*4,lly+number_height/10,LEADER_CHANGED);
	};
};

void Notebook::set_background(Background *new_background, boolean recur, boolean warn) {
	Sprite::set_background(new_background,recur,warn);
	if (picture != NULL && label != NULL && picture != tt_global_picture) {// added picture != tt_global_picture on 260103
		set_label(NULL); // I guess this is to release the reference to the picture
	};
};

Sprite *Notebook::released(Sprite *by, boolean top_level, SpecialDrop special_drop) {
   if (tt_toolbox != NULL && this == tt_toolbox_notebook) { // prior to 150704 was pointer_to_toolbox_notebook()) {
		// fixed on 290399 so it would not crash if a puzzle included a notebook that was released
		// commented out the following on 181000 since it is very confusing to have the notebook fly away after dropping it
//      if (called_since_last_release) {
//			const millisecond fly_duration = 2000;
//         fly_to_favorite_location(fly_duration);
//			do_after(fly_duration,this,sink_down_callback);
//         called_since_last_release = FALSE;
//      } else {
         if (by->kind_of() == PROGRAMMER) set_favorite_size_and_location();
//      };
//   if (by->kind_of() != ROBOT)
//      animate_to_favorite_size(2000);
   };
	last_key_for_label = FALSE;
	if (label != NULL) {
		label->released(by,FALSE,special_drop);
	};
   return(Sprite::released(by,top_level,special_drop));
};

void Notebook::now_on_floor(Background *floor, Sprite *by) {
   // this was a bad idea -- makes it hard to keep things small if thats what one likes
//   if (tt_log_version_number <= 7) animate_to_favorite_size(2000);
   Sprite::now_on_floor(floor,by);
};

void Notebook::move_to_hand() { 
	if (inside_vacuum()) {
		say(IDS_NOTEBOOK_IN_VACUUM);
		return;
	};
	if (floor == NULL) return; // hasn't left ToolBox yet
   //if (// !tt_running_old_demos &&  // commented out on 171102
   //    tt_system_mode != PUZZLE) {
   //   called_since_last_release = TRUE;
   //};
   if (running_to_hand() || floor == NULL) { // called twice so hurry up
		tt_finish_all_animations = TRUE;
		running_to_hand_flag = FALSE;
//		finish_instantly();
	} else {
		user_did(PROGRAM_PAD,NOTEBOOK_CALLED);
		running_to_hand_flag = TRUE;
		set_priority(min_long+1); //just under the martian and talk balloon
//		programmer = the_programmer; // commented out on 171102
		set_parameter(NOTEBOOK_FLYING);
		//city_coordinate *goal_x, *goal_y;
		//programmer->pointer_to_appearance()->pointers_to_lower_left_corner(goal_x,goal_y);
      city_coordinate goal_x, goal_y;
		tt_programmer->pointer_to_appearance()->lower_left_corner(goal_x,goal_y); // was programmer prior to 171102
		saved_page_number = left_page_number();	
		go_to_page(-1);
		// was too fast so *2 (on 200999 was /2 prior to that)
		millisecond duration = duration_delta_location(llx,lly,goal_x,goal_y)*2;
		animate_location_to(tt_programmer->pointer_to_appearance(), // goal_x,goal_y, // was programmer prior to 171102
                          duration,this,GET_GRASPED_CALLBACK);
	};
};

void Notebook::get_grasped() {	
	// why would one want to hold it
//	programmer->pick_up(this,JUMP_TO_GRASP);  // JUMP_TO_GRASP??
	running_to_hand_flag = FALSE;
	settle_down();
//	set_parameter(NOTEBOOK_SETTLE_DOWN);
//	go_to_page(saved_page_number);
};

void Notebook::stop_running() {
	Sprite::stop_running();
   running_to_hand_flag = FALSE;
	settle_down();
};

void Notebook::dramatic_entrance(millisecond duration) {
   city_coordinate goal_x = llx;
   city_coordinate goal_y = lly;
   city_coordinate goal_width = width;
   city_coordinate goal_height = height;
   // start small and somewhere off the top of the screen
   set_size_and_location(100,100,my_random(ideal_screen_width),21*tile_height);
   animate_size(goal_width,goal_height,duration);
   fly_to(goal_x,goal_y,duration,TRUE);
};  

void Notebook::fly_to_favorite_location(millisecond duration,
                                        boolean end_up_on_first_page) {
	if (tt_screen->inside_thought_bubble()) {
		fly_to(default_favorite_llx,default_favorite_lly,duration,end_up_on_first_page); // new on 011100 that in thought bubble objects are at standard default
	} else {
		fly_to(favorite_llx,favorite_lly,duration,end_up_on_first_page);
	};
};
	
void Notebook::fly_to(city_coordinate goal_x, city_coordinate goal_y,
							 millisecond duration, boolean end_up_on_first_page) {
  // better if duration were function of distance??
  set_parameter(NOTEBOOK_FLYING);
  set_priority(min_long+1);
  // fixes bug where page contents have wrong offsets
  go_to_a_blank_page();
  if (end_up_on_first_page) {
	  saved_page_number = 1;
//		go_to_page(1);
  } else {
	  saved_page_number = -1;
  };
  // moved here on 190999 in case duration is zero
  animate_location(goal_x,goal_y,duration,this,SETTLE_DOWN_CALLBACK);
};

void Notebook::settle_down() {
	set_parameter(NOTEBOOK_SETTLE_DOWN);
	set_cycle_stopped_callback(FINISHED_SETTLING_DOWN_CALLBACK);
	if (floor != NULL) set_priority(floor->next_item_priority()); // was max_long -- why??
//	tt_last_notebook = this;
}; 

void Notebook::finished_settling_down() {
	go_to_a_blank_page(); 	// fixes a bug with followers -- still an issue??
	if (saved_page_number >= 0) {
		go_to_page(saved_page_number);
	};
	update_textual_label_size_and_location();; // new on 200100 so label looks ok
};

void Notebook::go_to_a_blank_page() {
	go_to_page(-1);
};

void Notebook::go_to_recent_page(boolean show_page_turning) {
	if (recent_page_number > 0) {
		go_to_page(recent_page_number,show_page_turning);
	};
};

int Notebook::last_non_blank_page_number() {
#if TT_XML
   if (pages_count > 0) { // already converted from old format to new
      int n = pages_count;
      while (n > 0 && pages[n-1] == NULL) { // find non-blank page
         n--;
      };
      return(n);
   };
   // else do the "old" way
#endif
   if (old_pages == NULL || old_pages->no_pages()) return(0); // new on 220802
   int n = old_pages->length();
   while (n > 1 && old_pages->nth_blank((short int) (n-1)) && old_pages->nth_blank(n)) {
      n -= 2;
   };
   return(n);
};

void Notebook::go_to_page(short int n, boolean show_page_turning, boolean check_range,
								  boolean just_set_recent_page_number, Robot *robot) { // last arg was boolean by_robot prior to 301000
	// n is page number displayed -- page 1 and 2 equal
	// internal page_number is 0 to n/2-1 
	if (n < 0) {
		n = blank_page_number(); // was pages->blank_page_number() prior to 220802
   } else if (check_range) {
      if (n > page_count()) { // was pages->length() prior to 220802
		   // look for last non-blank pages
		   n = last_non_blank_page_number();
      };
		add_to_number = DONT_ADD_TO_PAGE; // new on 260900 so that if you type a page number and get to the last page you can then type over again
	};
//	boolean possibly_shared = !user_notebook(); 
	if (just_set_recent_page_number) {// && possibly_shared) { 
		// added user_notebook() test on 111203 since is not shared so can be changed directly -- 
		// removed on 160104 since broke robots that flipped through a notebook (while watched)
		set_recent_page_number(n,robot);
		return;
	};
	if (robot != NULL) { // && possibly_shared) {
		set_recent_page_number(n,robot); // added possibly_shared test on 111203 and removed it on 160104
	};
	short int new_page_number = (short int) ((n-1)/2);
	if (page_number == new_page_number) {
		// might be in a different house
//		if (left_page != NULL) left_page->record_offsets();
//		if (right_page != NULL) right_page->record_offsets();
//		set_clean_status(DIRTY);
		return; 
	};
	boolean forward = (new_page_number < page_number);
	if (show_page_turning) {
		next_page_number = new_page_number;
		turn_page(forward); // will switch contents in middle of animation
	} else {
		page_number = new_page_number;
		switch_contents();
	};
};

// following was wrong modularity -- "moved" to ::display
//void Notebook::set_leader(Sprite *the_leader, boolean temporarily) {
//	// new on 060705 so that if a robot off screen sends a notebook off it has the right page number
//	page_number = return_recent_page_number();
//	switch_contents();
//};

void Notebook::switch_contents(Sprite *by) { // arg new on 141200
#if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {
      log("Switch contents of debug target.");
   };
#endif
	// rewritten on 180103 so that this removes the old left and right page but delays generating new ones until displayed
//   tt_will_end_up_in_a_notebook = TRUE; // stuff created below
	if (left_page != NULL) {
//		if (floor != NULL) {
//			left_page->recursively_remove_leaders(floor);
//		};
		// just to be sure so that robot deletes thought bubble when it goes
		left_page->set_in_notebook(this); 
		if (left_page->pointer_to_leader() == this) { // how could it be otherwise?
			remove_follower(left_page);
		};
      left_page->remove_from_floor(FALSE); // no-op as of 070105 if follower removed above
		// on 240399 commented out the following and uncommented the lines
		// about prepare_for_deletion and prepare_for_deletion
		// the comment below is true but the reason for restoring the long delay
		// is that the bird might still refer to return_floor which might be a destroyed picture
//		if (tt_log_version_number < 15) {
//			left_page->destroy(); // commented out since causes pongact3 to crash
      // might be that a bird is still planning on returning to a cubby on page of left page
      // or something like that so wait 10 seconds (enough?) before destroying it
      // uses ref counts to protect the cubby now
////      left_page->remove_from_floor();
////      if (left_page->kind_of() == PICTURE) ((Picture *) left_page)->stop_all(TRUE);
//      left_page->prepare_for_deletion(NULL);
//      } else {
//			left_page->remove_from_floor(); // added 290399 since floor might be destroyed in the meanwhile 
			// above was commented out on 290100 since was just done a few lines above
#if !TT_XML
		left_page->set_slated_for_deletion(TRUE); // added 190799 so that before being destroyed won't be used
		tt_global_picture->do_after(default_duration(10000),left_page,destroy_callback);// was just_destroy_callback
#else // commented out on 160103 since XML doesn't leak this -- restored on 180103 since now spread out over time and is needed
		//if (left_page_is_copy) {
			left_page->destroy(); // new on 150103
		//};
#endif
		left_page = NULL;
	};
	if (right_page != NULL) {
//		if (floor != NULL) {
//			right_page->recursively_remove_leaders(floor);
//		};
		// just to be sure so that robot deletes thought bubble when it goes
		right_page->set_in_notebook(this);
		if (right_page->pointer_to_leader() == this) {
			remove_follower(right_page);
		};
      right_page->remove_from_floor(FALSE); // no-op as of 070105 if follower removed above
//		if (tt_log_version_number < 15) {
//			right_page->destroy(); // see comment for left_page above
//      right_page->prepare_for_deletion(NULL);
//      } else {
//			right_page->remove_from_floor(); // added 290399 -- commented out on 290100 since was just done a few lines above
#if !TT_XML
		right_page->set_slated_for_deletion(TRUE); // added 190799 so that before being destroyed won't be used
		tt_global_picture->do_after(default_duration(10000),right_page,destroy_callback); // was just_destroy_callback
#else // commented out on 160103 -- restored on 180103 since now spread out over time and is needed
//		if (right_page_is_copy) { // commented out the conditional on 300103 since increment_ref_count now
			right_page->destroy(); // new on 150103
//		};
#endif
		right_page = NULL;
	};
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
//	left_page_number_when_displayed = max(1,left_page_number(by)); // added max(1, on 120203 so never get a negative page to go to (Gordon had crash like this)
	left_page_number_when_displayed = left_page_number(by); // restored on 130203 since the better way to deal with this is to return when negative in finish_switching_contents
	if (showing_on_a_screen()) {
		finish_switching_contents();	
	};
};

void Notebook::finish_switching_contents() { 
#if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {
      log("Finishing switch contents of debug target.");
   };
#endif
#if TT_XML
//	finish_instantly(); // new on 090203 - this looks better than delaying until animation has finished or waiting for signal_size... 
	// commented out on 190803 since interferes with flying animation
	if (left_page_number_when_displayed < 0) {
		return; // new on 130203 
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
   left_page = page_sprite(left_page_number_when_displayed);
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
   if (left_page != NULL) {
		// I tried removing this but it meant it was very slow to flip through some notebooks (e.g. Playground) - restored on 290103
		// the idea is that pages are shared by all notebooks but the first to use doesn't have to copy
      left_page_is_copy = (left_page->pointer_to_leader() != NULL); // need to copy if already part of another notebook
      if (left_page_is_copy) {
         left_page = left_page->top_level_copy(); // made top_level_copy on 091002
		} else {
			left_page->increment_ref_count(); // new on 290103
		};
      if (picture != NULL) { // copied here on 231002 
         left_page->now_on_back_of(picture);
      };
      left_page->set_in_notebook(NULL); // so sensors work while open in the notebook      
   };
   right_page = page_sprite(left_page_number_when_displayed+1); // was right_page_number(by) prior to 180103
   if (right_page != NULL) {
      right_page_is_copy = (right_page->pointer_to_leader() != NULL); // need to copy if already part of another notebook
      if (right_page_is_copy) {
         right_page = right_page->top_level_copy(); // made top_level_copy on 091002
      } else {
			right_page->increment_ref_count(); // new on 290103
		};
      if (picture != NULL) { // copied here on 231002 
         right_page->now_on_back_of(picture);
      };
      right_page->set_in_notebook(NULL); // so sensors work while open in the notebook -- prior to 231002 this only was called if !right_page_is_copy
   };
   //} else { // do the old thing even though compiled with XML support
   //   left_page = pages->nth(left_page_number(by),version_number,this,picture);
   //   if (left_page != NULL) {
   //		left_page->set_in_notebook(this);
   //   };
	  // right_page = pages->nth(right_page_number(by),version_number,this,picture);
   //   if (right_page != NULL) {
   //		right_page->set_in_notebook(this);
   //   };
   //};
#else
	left_page = old_pages->nth(left_page_number_when_displayed,version_number,this,picture);
	right_page = old_pages->nth(left_page_number_when_displayed+1,version_number,this,picture);
#endif
	if (left_page != NULL) {
//      left_page_active = left_page->active(); // new on 150702 so doesn't start running now or too soon after being picked up
      //if (left_page_active) {
      //   left_page->set_active(FALSE);
      //   left_page->stop_all();
      //};
#if !TT_XML
		// test this out with XML notebooks (210902) -- and whether sensors are displayed up-to-date
		left_page->set_in_notebook(this);
#endif
		if (left_page->pointer_to_leader_offsets() != NULL) {
			add_follower(left_page,TRUE,INSERT_AT_END,TRUE); // new on 260103
		} else {
			fit_on_page(TRUE);
	//		left_page->recursively_propagate_changes();
			left_page->update_display();
			add_follower(left_page);
		};
		left_page->set_infinite_stack(TRUE); // moved here on 230405 so applies to both branches above
	};
	if (right_page != NULL) {
      //right_page_active = right_page->active(); // new on 150702
      //if (right_page_active) {
      //   right_page->set_active(FALSE);
      //   right_page->stop_all();
      //};
#if !TT_XML
		right_page->set_in_notebook(this);
#endif
		if (right_page->pointer_to_leader_offsets() != NULL) {
			add_follower(right_page,TRUE,INSERT_AT_END,TRUE); // new on 260103
		} else {
			fit_on_page(FALSE);
	//		right_page->recursively_propagate_changes();
	//		recursively_propagate_changes();	
			add_follower(right_page);
			right_page->update_display();
		};
		right_page->set_infinite_stack(TRUE); // moved here on 230405 so applies to both branches above
	};
	recursively_propagate_changes();	
//	move_by(1-2*(tt_frame_number&1),0); // move 1 pixel (left or right) to force propagation
	set_clean_status(DIRTY);
//   tt_will_end_up_in_a_notebook = FALSE;
	left_page_number_when_displayed = -1; // until the need arises again
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
};

//void Notebook::signal_size_and_location() { // new on 090203
//	Sprite::signal_size_and_location();
//	fit_on_page(FALSE);
//	fit_on_page(TRUE);
//};

void Notebook::fit_on_page(boolean left) { // abstracted on 090203
	Sprite *page;
	if (left) {
		page = left_page;
	} else {
		page = right_page;
	};
	if (page == NULL) return;
	city_coordinate contents_width,contents_height, contents_llx,contents_lly;
	//		right_page->recursively_propagate_changes();
	compute_contents_dimensions(left,
										 contents_width,contents_height,
										 contents_llx,contents_lly);
	// don't know why but robots are too far down and to left -- commenting out in an experiment on 090203
	if (page->kind_of() == ROBOT) { 
		if (left) { // 
			contents_llx += width/20; // restored 110203 and changed from 10 to 20
		} else { // new on 110203
//			contents_llx -= width/20;
		};
//		contents_lly += height/10;
	};
	if (page->pointer_to_followers() != NULL && !page->followers_completely_inside()) {
//		page->update_display(); // new on 071004 since otherwise old_x_offset and old_y_offset are wrong (if city saved in another resolution)
		// and these offsets are used below
		page->really_propagate_changes(); // new on 220103 since need to get geometry right if has followers and they aren't all inside
	};
	page->set_true_size_and_location(contents_width,contents_height,contents_llx,contents_lly,TO_FIT_INSIDE);
};

void Notebook::turn_page(boolean forward) {
#if !TT_XML
	if (left_page != NULL) left_page->set_slated_for_deletion(TRUE);
	if (right_page != NULL) right_page->set_slated_for_deletion(TRUE);
#endif
	play_sound(PAGE_TURNING_SOUND); 
	if (forward) {
		set_parameter(NOTEBOOK_BEGIN_PAGE_TURN_FORWARD);
	} else {
		// really begin turn backwards
		set_parameter(NOTEBOOK_END_PAGE_TURN_FORWARD);
	};
	set_cycle_stopped_callback(CONTINUE_TURN_PAGE_CALLBACK);	
};

void Notebook::continue_turn_page() {
	// first to set up overlay of the page flipping
	if (page_flipping == NULL) {
		page_flipping = new Sprite(NOTEBOOK_PAGE_TURN_SPRITE);
	};
	overlay_status = OVERLAY_STARTED;
	if (current_parameter() == NOTEBOOK_BEGIN_PAGE_TURN_FORWARD) {
		page_flipping->set_parameter(FLIP_TOWARD_NEXT);
		set_cycle_stopped_callback(FORWARD_SWITCH_CONTENTS_CALLBACK);	
	} else { 
		page_flipping->set_parameter(FLIP_TOWARD_PREVIOUS);
      page_number = next_page_number;
		next_page_number = 0; // reset -- new on 060705
		switch_contents();
		set_cycle_stopped_callback(OVERLAY_PREVIOUS_DONE_CALLBACK);	
	};
	page_flipping->reset_cycle(); // if flipping multiple times in a row
};
	
void Notebook::forward_switch_contents() {
	page_number = next_page_number;
	next_page_number = 0; // reset -- new on 060705
	overlay_status = OVERLAY_FINISHING;
	set_parameter(NOTEBOOK_END_PAGE_TURN_FORWARD);
	switch_contents();
};

void Notebook::overlay_previous_done() {
	overlay_status = OVERLAY_OFF;
	set_parameter(NOTEBOOK_END_PAGE_TURN_BACKWARD);
};

void Notebook::set_recent_page_number(short int n, Robot *robot) {
	left_page_current = (flag) (n&1);
	if (!left_page_current) n--; // maintain the left (odd) page number
	recent_page_number = n;
	if (robot != NULL) {
		robot->set_recent_page_number(n,this); // new on 301000
	};
};

short int Notebook::right_page_number(Sprite *by) {
	if (by != NULL && by->kind_of() == ROBOT) { //  && !user_notebook()) { // not by->still_running() 
		// added user notebook test on 111203 since not shared -- removed on 070104 since broke adventure game
//		if (tt_toolbox != NULL && this == pointer_to_toolbox_notebook()) { // commented out on 301000 since works for all notebooks now
		short int robots_page_number = (short int) (((Robot *) by)->return_recent_page_number(this));
//#if TT_BUG_FIXES // commented out on 111203
//		if (robots_page_number >= 0) // new on 301103 that negative value indicates robot didn't know about this notebook
//#endif
//#if TT_DEBUG_ON
//			if (robots_page_number+1 != 2*page_number+2) {
//				debug_this();
//			};
//#endif
			return(robots_page_number+1);
//		};
//		return((short int) (recent_page_number+1));
	};
	return((short int) (2*page_number+2));
};

short int Notebook::left_page_number(Sprite *by) {
	if (by != NULL && by->kind_of() == ROBOT) { // && !user_notebook()) { // not by->still_running() 
		// added user notebook test on 111203 since not shared -- removed on 070104 since broke adventure game
//		if (tt_toolbox != NULL && this == pointer_to_toolbox_notebook()) {
		short int page_number = (short int) (((Robot *) by)->return_recent_page_number(this));
#if TT_BUG_FIXES
		if (page_number >= 0)  // new on 301103 that negative value indicates robot didn't know about this notebook
#endif
			return(page_number);
//		};
//	   return(recent_page_number);
	};
	return((short int) (2*page_number+1));
};

Sprite *Notebook::used(Sprite *subject, Sprite *by, boolean , boolean record_action,
                       millisecond , boolean modified, Sprite *) {
//   if (tt_new_input_scheme) 
	modified = FALSE; // 3rd button used differently now
   add_to_number = DONT_ADD_TO_PAGE;
	Robot *robot; // updated 301000
   if (by != NULL && by->kind_of() == ROBOT) { //by->still_running());
		robot = (Robot *) by;
	} else {
		robot = NULL;
	};
  	boolean just_set_recent_page_number = (robot != NULL && !robot->showing_on_a_screen());
   if (modified) {
      go_back_a_page(by,!just_set_recent_page_number,just_set_recent_page_number,robot); // second arg added 111203
   } else {
      go_to_page((short int) (right_page_number(by)+1),!just_set_recent_page_number,FALSE, // second arg was TRUE prior to 111203
                 just_set_recent_page_number,robot);
   };
	if (record_action) record_event(APPLY_GRASPED_ITEM,by,floor,subject); // added 020299 since needs to be recorded
   ::completed(by);
   return(this);
};

Sprite *Notebook::respond_to_keyboard(unsigned char key, boolean extended, boolean selected_too, Sprite *by,
											     boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless) {
//	if (infinite_stack_flag) return; // ignore keystrokes over stack
	Robot *robot; // updated 301000
   if (by != NULL && by->kind_of() == ROBOT) { 
		robot = (Robot *) by;
	} else {
		robot = NULL;
	};
//	boolean running_robot = (by != NULL && by->still_running());
   if (robot == NULL) { // if page is turning...
		finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off 
	};
	boolean by_robot_not_showing_on_a_screen = (robot != NULL && !by->showing_on_a_screen());
	boolean just_set_recent_page_number = (by_robot_not_showing_on_a_screen
													   && robot->return_recent_page_number(this) >= 0);
														// new on 301103 since otherwise robot doesn't "know" this notebook should still change it
	// if robot is doing this and is offscreen don't do anything but reset offset
	// since robot will know what page to get things from
//	tt_last_notebook = this; // shouldn't do this if by_robot -- right?
//   if (selected_too) add_to_number = DONT_ADD_TO_PAGE;
	boolean selection_too = (add_to_number == DONT_ADD_TO_PAGE);
	if (held() && (last_key_for_label || IsCharAlpha(key) || key > 127) && 
		// third disjunct added on 220800 but maybe should only apply if need_wide_characters()
		 picture == NULL) { //  && tt_log_version_number > 14
		// held() added on 130499 so labeling doesn't interfere so much with page flipping
		// and not a picture's remote control notebook
		if (label == NULL) {
//			string name = new character[2]; 
//			name[0] = key;
//			name[1] = NULL; // terminate
//			set_label_string(name);
			wide_string name = update_string((wide_string) NULL,key,extended); // on 240800 replaced the above with this
			set_label_string("");
			if (name != NULL) { // condition added 050201 - happens if rubout typed too many times
				((Text *) label)->set_wide_text(name,wide_string_length(name),FALSE);
			};
		} else if (by->can_send_keyboard_editing_keys()) { // new on 120604
			// replace label with original_recipient on 030105 to enable recording
			label->respond_to_keyboard(key,extended,selected_too,by,record_action,original_recipient,size_constrained_regardless);
//			fit_name_on_chest(); // new on 120604
			last_key_for_label = TRUE; // missing prior to 180804
			return(this);
		} else { // if (picture == NULL) {
			Text *text_label = (Text *) label;
			wide_string text = text_label->copy_text_string();
			wide_string new_text = update_string(text,key,extended);
			boolean just_modified = (text == new_text);
			if (new_text == NULL || new_text[0] == NULL) {
				remove_follower(label);
				text_label->destroy();
				label = NULL;
			} else {
				text_label->set_wide_text(new_text,wide_string_length(new_text),just_modified);
			};
		};
		last_key_for_label = TRUE;
	} else if (key >= '0' && key <= '9') {
		// second arg to calls to go_to_page below were TRUE prior to 111203 so heard it when shouldn't
		last_key_for_label = FALSE;
		page_number_offset = 0;
		short int digit = (short int) (key-'0');
      if (robot != NULL) {
         if (selected_too) {
            go_to_page(digit,!by_robot_not_showing_on_a_screen,TRUE,just_set_recent_page_number,robot);
         } else {
				short int n = left_page_number(by);
				if (!left_page_current) n++; // right page really
		      n = (short int) (n*10+digit);
     		   go_to_page(n,!by_robot_not_showing_on_a_screen,TRUE,just_set_recent_page_number,robot);
         };
      } else {
//			last_key_for_label = FALSE; // commented out on 180804 since is redundant with the assignment before this conditional
         if (add_to_number == ADD_TO_LEFT_PAGE) {
			   go_to_page((short int) (left_page_number(by)*10+digit),!by_robot_not_showing_on_a_screen,TRUE,just_set_recent_page_number,robot);
         } else if (add_to_number == ADD_TO_RIGHT_PAGE) {
            go_to_page((short int) (right_page_number(by)*10+digit),!by_robot_not_showing_on_a_screen,TRUE,just_set_recent_page_number,robot);
         } else if (add_to_number == DONT_ADD_TO_PAGE) {
            go_to_page(digit,TRUE,TRUE,just_set_recent_page_number,robot);
            if (digit&1) {
               add_to_number = ADD_TO_LEFT_PAGE;
            } else {
				   add_to_number = ADD_TO_RIGHT_PAGE;
            };
         };
		   user_did(PROGRAM_PAD,NOTEBOOK_KEYBOARD_NUMBER,by);
      };
	} else {
		last_key_for_label = FALSE;
      if (key == '+' || key == tt_run_character) { // was ' ' prior to 070502
			if (robot == NULL) add_to_number = DONT_ADD_TO_PAGE;
			go_to_page((short int) (right_page_number(by)+1),!by_robot_not_showing_on_a_screen,FALSE,just_set_recent_page_number,robot);
			user_did(PROGRAM_PAD,NOTEBOOK_KEYBOARD_FORWARD,by);
      } else if (key == '-' || key == BACKSPACE) { // moved here on 080100 since tt_new_input_scheme is always TRUE
			if (robot == NULL) add_to_number = DONT_ADD_TO_PAGE;
			go_back_a_page(by,!by_robot_not_showing_on_a_screen,just_set_recent_page_number,robot);
/*
			case BACKSPACE:
            if (tt_new_input_scheme) { // new scheme is that to treat - and backspace the same
               if (!by_robot) add_to_number = DONT_ADD_TO_PAGE;
				   go_back_a_page(by,just_set_recent_page_number,by_robot);
				   break;
            };
            if (by_robot) {
               if (selected_too) {
                  go_back_a_page(by,just_set_recent_page_number,by_robot);
               } else {
						go_to_page((short int) (left_page_number(by)/10),TRUE,TRUE,just_set_recent_page_number,by_robot);
               };
            } else {
				   switch (add_to_number) {
					   case ADD_TO_LEFT_PAGE:
						   page_number_offset = 0;
						   go_to_page((short int) (left_page_number(by)/10),TRUE,TRUE,just_set_recent_page_number,by_robot);
						   break;
					   case ADD_TO_RIGHT_PAGE:
						   page_number_offset = 0;
						   go_to_page((short int) (right_page_number(by)/10),TRUE,TRUE,just_set_recent_page_number,by_robot);
						   break;
					   case DONT_ADD_TO_PAGE:
						   go_back_a_page(by,just_set_recent_page_number,by_robot);
//						   play_sound(PLOP_SOUND);
						   break;
				   };
				   if (next_page_number == 0) add_to_number = DONT_ADD_TO_PAGE;
            };
				break;
*/
		};
	};
   if (record_action) { // && !by_robot) {
      if (selection_too) {
		   record_event(KEYBOARD_CHARACTER_AND_SELECT,by,floor,this,TRUE,key);
	   } else {
		   record_event(KEYBOARD_CHARACTER,by,floor,this,TRUE,key);
	   };
		record_action = FALSE; // handled here
   };
//	completed(action_status);
  // commented out the above for the following
   return(Sprite::respond_to_keyboard(key,extended,selected_too,by,record_action,original_recipient,size_constrained_regardless));
};

ArrowKeyEditing Notebook::arrow_key_editing_status() { // new on 120604
	if (label != NULL) {
		return(label->arrow_key_editing_status());
	} else { 
		return(Sprite::arrow_key_editing_status());
	};
};

void Notebook::unselect() {
	add_to_number = DONT_ADD_TO_PAGE;
	if (label != NULL) { // new on 120604
		label->unselect();
	};
	Sprite::unselect(); 
};

void Notebook::go_back_a_page(Sprite *by, boolean show_page_turing, boolean just_set_recent_page_number, Robot *robot) {
	// second arg new on 111203 -- was TRUE in calls below
	short int new_page_number = (short int) (left_page_number(by)-1);
	if (new_page_number > 0) { 
		go_to_page(new_page_number,show_page_turing,TRUE,just_set_recent_page_number,robot);
	} else { // this will find last non-blank page
		go_to_page(max_short,show_page_turing,TRUE,just_set_recent_page_number,robot);
	};
	if (robot == NULL) user_did(PROGRAM_PAD,NOTEBOOK_KEYBOARD_BACKWARD,by);
};

const int max_java_notebooks = 1000;

int java_notebook_count = 0;

NotebookPointer *java_notebooks = NULL;

SpritesPointer *java_notebook_pages = NULL;

void release_java_notebook_pages() {
	if (java_notebooks != NULL) {
		delete [] java_notebooks;
		java_notebooks = NULL; // added 061200
	};
	if (java_notebook_pages != NULL) {
		for (int i = 0; i < java_notebook_count; i++) {
			Sprites *pages = java_notebook_pages[i];
			just_destroy_list(pages); // prior to 090403 this called destroy_sprites but there is sharing now using XML
		};
		delete [] java_notebook_pages;
		java_notebook_pages = NULL; // added 061200
	};
	java_notebook_count = 0;
};

Sprites *Notebook::java_pages() {
	if (java_notebook_count == 0) { // new on 071200 so 2 applets with notebooks don't collide
		increment_ini_int(AC(IDC_SWITCHES),"JavaNotebookCounter",FALSE);
	};
	for (int i = 0; i < java_notebook_count; i++) {
		if (equal(java_notebooks[i],this)) { // was == prior to 250601
			return(java_notebook_pages[i]);
		};
	};
	Sprites *all_pages = compute_java_pages(); 
	if (java_notebooks == NULL) {
		java_notebooks = new NotebookPointer[max_java_notebooks];
		java_notebook_pages = new SpritesPointer[max_java_notebooks];
	};
	java_notebooks[java_notebook_count] = this;
	java_notebook_pages[java_notebook_count] = all_pages;
	java_notebook_count++;
	if (java_notebook_count >= max_java_notebooks) {
		log("Too many notebooks for Java applet generator. Sorry.",FALSE,TRUE);
		java_notebook_count = 0; // what to do?
	};
	return(all_pages);
};

Sprites *Notebook::compute_java_pages() { // abstracted from the above on 180201
	int length = last_non_blank_page_number();
   Sprites *all_pages = NULL;
	for (int j = 1; j <= length; j++) {
      // 1-indexing
#if TT_XML
      Sprite *page = page_sprite(j);
#else
		Sprite *page = old_pages->nth(j,version_number);
#endif
		if (page != NULL) { // conditional new on 100101
//			Sprite *page_copy = page->top_level_copy(); // so it has no connections with others
//#if !TT_XML
//			page->destroy(); // new on 240601
//#endif
			insert_at_end(page,all_pages); // was page_copy but all pages are copies now - no they are not! - not copies at all with TT_XML - 080403
		} else {
			insert_at_end(NULL,all_pages); // new on 230101
		};
	};
	return(all_pages);
};

int java_notebook_index(Notebook *notebook) {
	for (int i = 0; i < java_notebook_count; i++) {
		if (equal(java_notebooks[i],notebook)) { // was == prior to 250601
			return(i);
		};
	};
	return(-1); // shouldn't happen??
};

void Notebook::java(java_stream &out) {
   switch (built_in_notebook) {
      case LOCAL_REMOTES_NOTEBOOK:
      case PICTURES_NOTEBOOK:
      case GLOBAL_REMOTES_NOTEBOOK:
      case SOUNDS_NOTEBOOK:
      case OPTIONS_NOTEBOOK:
         java_just_send(out,S(IDS_JAVA_TT_NOTEBOOK_CLASS),S(IDS_JAVA_GET_LOCAL_REMOTES_NOTEBOOK+built_in_notebook));
         break;
		case EXAMPLES_NOTEBOOK: // this is new on 240601 and isn't quite right if the ToonTalk code used the Examples - sigh...
			out << "new TTNotebook(0)"; // a notebook with nothing in it
			break;
      default:
			if (is_blank()) { // conditional new on 230101
				out << "TT.BLANK_NOTEBOOK"; // javacom.rc
			} else if (tt_toolbox_notebook == this) { // updated 150704
				out << "TT.NOTEBOOK"; // new on 310101
			} else {
//         java_new(out,S2(IDS_JAVA_TT_NOTEBOOK_CLASS,java_notebook_index(this))); // ,last_non_blank_page_number());
				int notebook_counter = ini_int(AC(IDC_SWITCHES),"JavaNotebookCounter",FALSE); // FALSE added on 050404 since this would break demos that were paused and an applet generated
				out << "new TTNotebook" << notebook_counter;
				int index = java_notebook_index(this);
				if (index >= 0) { // condition new on 062401
					out << "_" << index;
				};
				out << "()";
			};
   };
};

void Notebook::java_after(java_stream &out, const_java_string path_temp) {
	 return; // new on 041200 since this is now put into initialize method
};

boolean Notebook::is_built_in_notebook() { // abstracted on 240601
	switch (built_in_notebook) {
      case LOCAL_REMOTES_NOTEBOOK:
      case PICTURES_NOTEBOOK:
      case GLOBAL_REMOTES_NOTEBOOK:
      case SOUNDS_NOTEBOOK:
      case OPTIONS_NOTEBOOK:
      case EXAMPLES_NOTEBOOK: 
		// why isn't MATH_NOTEBOOK included -- asked 110505
         return(TRUE);
   };
	return(FALSE);
};

void Notebook::java_initialize(java_stream &out) {
   if (is_built_in_notebook()) return;
//   java_string path = copy_string(path_temp); // might be in string table
	Sprites *remaining = java_pages();
	int index = 1;
	int notebook_counter = ini_int(AC(IDC_SWITCHES),"JavaNotebookCounter",FALSE); 
	// FALSE added on 050404 since this would break demos that were paused and an applet generated
   while (remaining != NULL) {
      Sprite *page = remaining->first();
		java_indent(out); // new on 180201
		out << "private void notebook" << notebook_counter << "Page" << index << "() {" << endl;
		tt_java_indent += 2; // new on 180201
      if (page != NULL) {
			start_new_java_context(); // new on 180201 so that the following occurs in a fresh new context with no robots already assigned to Java variables
         page->java_before(out);
         java_send_this(out,S(IDS_JAVA_SET_PAGE),index,page); // included path prior to 041200
//         out << "  " << path << ".SetPage(" << i << ", ";
//         page->java(out);
//         out << ");" << endl;
         char page_path[256];
         output_string_stream notebook_page_path(page_path,sizeof(page_path));
//         java_just_send(notebook_page_path,S(IDS_JAVA_PAGE),index); // included was path prior to 041200
			notebook_page_path << S(IDS_JAVA_PAGE) << "(" << index << ")";
			notebook_page_path.put('\0');
			page->java_after(out,notebook_page_path.STR);
			end_new_java_context(); // new on 180201
//			if (page->kind_of() == PICTURE) { // new on 031200
//				Picture *picture = (Picture *) page;
//			   tt_java_picture_name = copy_string(picture->java_name()); // SC(IDS_JAVA_PICTURE);  // used deep inside of generating Java for remote controls
//				picture->java_followers(out,tt_java_picture_name);
//				picture->java_flipped_followers(out,tt_java_picture_name);
//			};
//         page->destroy();
      };
		tt_java_indent -= 2;
		java_indent(out); // new on 180201
		out << "}" << endl;
		remaining = remaining->rest();
		index++;
   };
//   delete [] path; 
};

void Notebook::java_flipped_followers(java_stream &out, const_java_string ) { // new on 031200
   if (is_built_in_notebook()) return;
   Sprites *remaining = java_pages();
   while (remaining != NULL) {
      Sprite *page = remaining->first();
      if (page != NULL) {
#if TT_DEBUG_ON
			if (tt_debug_mode == 31200) {
				page->print(tt_error_file());
				tt_error_file() << " was created to check for flipped followers for Java." << endl;
			};
#endif
         page->java_flipped_followers(out,NULL);
//         page->destroy();
      };
		remaining = remaining->rest();
   };
};

Sprites *Notebook::reachable_sprites_of_type(SpriteType type) { // new on 031200 - generalized 041200
	if (is_built_in_notebook()) return(NULL);
	Sprites *answer = NULL;
	Sprites *remaining = java_pages();
	while (remaining != NULL) {
		Sprite *page = remaining->first();
		if (page != NULL) {
			if (answer == NULL) {
				answer = page->reachable_sprites_of_type(type);
			} else {
				answer->last_cons()->set_rest(page->reachable_sprites_of_type(type));
			};
		};
		remaining = remaining->rest();
	};
	if (type == PROGRAM_PAD) {
		return(new Sprites(this,answer));
	} else {
		return(answer);
	};
};

Sprites *Notebook::reachable_non_equal_sprites_of_type(SpriteType type) { // new on 250601
	if (is_built_in_notebook()) return(NULL);
	Sprites *answer = NULL;
	Sprites *remaining = java_pages();
	while (remaining != NULL) {
		Sprite *page = remaining->first();
		if (page != NULL) {
			Sprites *more = page->reachable_non_equal_sprites_of_type(type);
			insert_sprites_at_end_if_not_equal_member(answer,more);
		};
		remaining = remaining->rest();
	};
	if (type == PROGRAM_PAD) {
//		return(new Sprites(this,answer));
		insert_at_end_if_not_equal_member(this,answer); // different order than == version but order shouldn't matter
	};
	return(answer);
};

void Notebook::java_class(java_stream &out) {
   switch (built_in_notebook) {
      case LOCAL_REMOTES_NOTEBOOK:
      case PICTURES_NOTEBOOK:
      case GLOBAL_REMOTES_NOTEBOOK:
      case SOUNDS_NOTEBOOK:
      case OPTIONS_NOTEBOOK:
      case EXAMPLES_NOTEBOOK:
//      case USER_NOTEBOOK:
         return;
   };
   Sprites *remaining = java_pages();
	int length;
//	if (remaining == NULL) return; // commented out on 180201 since referring to a blank notebook is fine
	if (remaining == NULL) {
		length = 0;
	} else {
		length = remaining->length();
	};
   while (remaining != NULL) {
      Sprite *page = remaining->first();
      if (page != NULL) {
			if (page->kind_of() != PROGRAM_PAD) { // condition new on 300301 since don't want to do this twice and this was already collected by reachable_sprites_of_type
				page->java_class(out);
			};
//         page->destroy();
      };
		remaining = remaining->rest();
   };
	// following is new on 041200
	if (tt_toolbox_notebook != this) { // conditional new on 310101 -- updated 150704
		int index = java_notebook_index(this);
//		if (index < 0) return; // commented out on 240601
		int notebook_counter = ini_int(AC(IDC_SWITCHES),"JavaNotebookCounter",FALSE); // FALSE added on 050404 since this would break demos that were paused and an applet generated
		java_indent(out); // new on 180201
		out << "class TTNotebook" << notebook_counter;
		if (index >= 0) { // conditional new on 240601
			out << "_" << index;
		};
		out << " extends TTNotebook {" << endl; // could add this stuff to javacom.rc
		out << "  public TTNotebook" << notebook_counter;
		if (index >= 0) { // conditional new on 240601
			out << "_" << index;
		};
		out << "() {" << endl;
		out << "    super(" << length << ");" << endl;
		out << "  }" << endl;
		out << "  protected void initialize() {" << endl;
		out << "    super.initialize();" << endl;
		for (int i = 0; i < length; i++) { // one page per method to avoid 64K limit of Java
			out << "    notebook" << notebook_counter << "Page" << (i+1) << "();" << endl;
		};
		out << "  }" << endl;
		java_initialize(out);
		java_indent(out); // new on 180201
		out << "}" << endl;
	};
};

#if TT_TRIAL_VERSION
flag trial_warning = FALSE;
#endif

void Notebook::dump() {
#if TT_TRIAL_VERSION
	if (!trial_warning && !replaying() && !tt_temporary_user) {
		trial_warning = TRUE;
	   message_box(MB_ICONINFORMATION|MB_SYSTEMMODAL,S(IDS_TRIAL_VERSION_CANT_SAVE));
	};
	return;
#endif
	if (original && file_name != NULL && !tt_temporary_user) {
#if TT_LEGO
// conditionalized on 220802
		if (strcmp("4",file_name) == 0 && tt_lego_in_use != NO_LEGO_CONTROLLER) {
			return;
		};
#endif
		output_file_stream stream;
      ascii_string full_file_name = ::new_file_name(tt_file_name,file_name);
      if (full_file_name == NULL) return;
#if TT_XML
      if (tt_dump_as_xml) {
         int original_file_name_length = strlen(full_file_name); // new on 300902
         strcat(full_file_name,".xml"); // safe because full_file_name is MAX_PATH long
         boolean saved_in_notebook_flag = in_notebook_flag;
         in_notebook_flag = FALSE; // so the following dumps its contents and not just the name -- better to remove file_name temporarily?
			UnwindProtect<DumpReason> set(tt_dump_reason,DUMPING_NOTEBOOK_CONTENTS); // new on 300103
#if !TT_ALPHA_FEATURE
//			UnwindProtect<boolean> set2(tt_saving_media,(!tt_saving_media && tt_save_media_in_sub_notebooks)); // new on 200903 to ensure that files can be transported
 			UnwindProtect<xml_element *> set3(tt_zip_description_archive_element,NULL); // new on 210903 so this gets its own media files and not the time travel or log archive 
#else
			UnwindProtect<boolean> save(tt_saving_media); // new on 270204
			UnwindProtect<xml_element *> save2(tt_zip_description_archive_element);
			if (tt_save_media_in_main_notebook) { 
				// removed && main_notebook() on 280204 since notebooks that are pages of the main should also capture media
				tt_saving_media = TRUE;
				// the following interferes with the generation of notebook versions for time travel - was it needed? 280204
//				tt_zip_description_archive_element = NULL; // so this gets its own media files and not the time travel or log archive
			};
#endif
			// above moved here on 220903 since should not apply to all calls to xml_save_document
         xml_document *document = xml_create_document();
         in_notebook_flag = saved_in_notebook_flag;
         xml_save_document(document,full_file_name,NULL);
         xml_release_document(document);
         full_file_name[original_file_name_length] = '\0'; // restore to original file name
         if (file_exists(full_file_name,FALSE)) { // if there was as old non-XML version get rid of it
//          ::DeleteFile(full_file_name);
				rename_file_to_backup(full_file_name); // new on 280103
         };
         delete [] full_file_name;
			if (main_notebook()) {
				main_notebook_changed(); // new on 171203
			};
         return;
      };
#endif
      stream.open(full_file_name,std::ios_base::out|std::ios_base::binary);
		if (stream == NULL) { // new on 290904 for error recovery
			unable_to_write_file(full_file_name); 
			return;
		};
		dump_contents(stream);
		stream.close();
      strcat(full_file_name,".xml");
      if (file_exists(full_file_name,FALSE)) { // if there was as old XML version get rid of it
         rename_file_to_backup(full_file_name);
      };
      delete [] full_file_name;
	};
};

void Notebook::xml_to_stream(output_stream &stream) { // new on 140203
	boolean saved_in_notebook_flag = in_notebook_flag;
   in_notebook_flag = FALSE; // so the following dumps its contents and not just the name -- better to remove file_name temporarily?
	UnwindProtect<DumpReason> set(tt_dump_reason,DUMPING_NOTEBOOK_CONTENTS); // new on 3001035
	//boolean saved_tt_logging = tt_logging;
	//tt_logging = FALSE; // new on 250503 since the following could convert an old notebook to a new one and cause the clipboard to be read and recorded
   xml_document *document = xml_create_document();
	//tt_logging = saved_tt_logging;
	//if (tt_frame_number <= 1) { // don't want clipboard recording to be confused if the above caused it to be read
	//	tt_main_window->release_last_clipboard_string();
	//};
   in_notebook_flag = saved_in_notebook_flag;
	::xml_to_stream(document,stream);
   xml_release_document(document);
};

boolean Notebook::dump(output_stream &stream, boolean just_prepare, boolean , boolean ) {
   if (just_prepare) return(FALSE);
   if (tt_dump_so_equals_share || (ref_count > 1 && file_name == NULL)) { // && tt_dumping_background != NULL) { // avoid copying it
		// added tt_dump_so_equals_share on 250601
      int dump_index = about_to_dump(this,stream); // rewritten on 020802
	   if (dump_index < 0) return(FALSE);
   };
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
		print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
		tt_error_file() << "About to dump ";
		print(tt_error_file());
		tt_error_file() << endl;
		tt_debug_dump_depth++;
	};
#endif
   stream.put((unsigned char) PROGRAM_PAD);
	dump_if_not_null(stream,comment);
	if (label != NULL && picture == NULL) { // don't bother if label is just picture
		label->dump(stream);
	} else {
		stream.put(NOTHING_MARKER);
	};
   stream.put((unsigned char) left_page_number());
   unsigned char file_name_length = 0;
   if (file_name != NULL && tt_dump_reason != DUMPING_TO_CLIPBOARD) {
      file_name_length = (unsigned char) strlen(file_name);
   };
   stream.put(file_name_length);
   if (file_name_length == 0) {
      dump_contents(stream);
   } else {
      stream.write((character *) file_name,file_name_length);
		if (strcmp(file_name,"rem") == 0) { 
			// condition was built_in_notebook == LOCAL_REMOTES_NOTEBOOK prior to 110200 but when loading tests file name
			// and they can get out of synch (I think)
			if (picture != NULL && being_dumped_together(picture,this)) {
				picture->dump(stream,just_prepare); // new on 5/24/98 but I don't think anything out there has saved notebook of remotes with its picture
				picture->dump_geometry(stream); // why?? - good idea??
			} else {
				stream.put(NOTHING_MARKER);
			};
		};
	};
//		output_file_stream new_stream;
//		new_stream.open(file_name,std::ios_base::out|std::ios_base::binary);
//		if (new_stream == NULL) {
//			tt_error_file() << "Warning file " << file_name << " could not be opened." << endl;
//			return;
//		};
//		dump_contents(new_stream);
//		new_stream.close();
//	};
	unsigned char flags = ((!!toolbox_tool(this))+infinite_stack()*2+blank*4); 
	// infinite_stack() added on 080400
	// blank added 220500
	stream.put(flags); // new on 050100 - was just toolbox_tool(this) prior to 080400
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

void Notebook::dump_contents(output_stream &stream) {
#if TT_XML
   // new on 280902 so that can switch back to old format if desired
   write_cookie(stream);
   if (pages == NULL) { // new on 231002
      compute_pages(TRUE);
   };
   short unsigned int page_count = (short unsigned int) last_non_blank_page_number(); // updated on 309002
   if (page_count == 0) { // empty
      page_count = 2;
		pages[0] = NULL; // new on 021202
		pages[1] = NULL;
   };
   stream.write((character *) &page_count,sizeof(page_count));
   for (int page_number = 0; page_number < page_count; page_number++) { // updated on 309002 to deal with empty notebooks -- was 1-indexing prior to 201002
      //Sprite *sprite = page_sprite(page_number,FALSE);
      //long length;
      Page *page = pages[page_number];
      //if (sprite != NULL) {
      //   string encoding = sprite->return_encoding(length,FALSE,FALSE);
      //   stream.write((char *) &length,sizeof(length));
      //   if (length > 0) { // can it be otherwise?
      //      stream.write(encoding,length);
      //      delete [] encoding;
      //   };
      if (page != NULL) {
         page->dump(stream);
      } else {
         long length = 0;
         stream.write((char *) &length,sizeof(length));
      };
   };
#else
	unsigned long saved_seed = random_number_seed(); 
   write_cookie(stream);
	short unsigned int page_count = 0;
	if (pages != NULL) {
		page_count = last_non_blank_page_number();
      if (page_count == 0) { // empty
         page_count = 2;
      };
	};
	stream.write((character *) &page_count,sizeof(page_count));
	convert_to_latest_version_number(page_count);
	old_pages->dump(stream,page_count);
	set_random_number_seed(saved_seed); // added on 181000 since this might load RANDOM_REMOTE and change the seed and break logging
#endif
};

#if TT_XML

//xml_element *Notebook::xml_create_element(xml_document *document) {
//   return(create_xml_element(xml_tag(),document));
//};

//void Notebook::set_recent_xml_location(int location) { -- obsolete 160204
//	if (location > 0 && xml_location_of_last_good_full_save == 0) {
//		xml_location_of_last_good_full_save = location;
//	};
//	Sprite::set_recent_xml_location(location);
//};

//boolean Notebook::notebook_is_only_on_default_user_directory() { // new on 290803 -- noticed was obsolete on 041003
//	// this doesn't bother with old-format files
//	if (tt_default_user_directory == NULL || file_name == NULL) return(FALSE);
//	string full_file_name = page_full_file_name(file_name,tt_file_name,FALSE,"xml");
//	if (full_file_name != NULL) {
//		delete [] full_file_name;
//		return(FALSE);
//	};
//	return(TRUE);
//};

#if TT_ALPHA_FEATURE
//string Notebook::collect_media_file_names() {
//	string result = NULL;
//	for (int i = 0; i < pages_count; i++) {
//		if (pages[i] != NULL) { 
//			string media_file_names_on_page = pages[i]->return_media_file_names();
//			if (media_file_names_on_page != NULL) {
//				add_new_media_file_names(media_file_names_on_page,result);
//			//	if (result == NULL) {
//			//		result = copy_string(media_file_names_on_page);
//			//	} else {
//			//		string both = append_strings(result,media_file_names_on_page);
//			//		delete [] result;
//			//		result = both;
//			//	};
//			};
//		};
//	};
//	return(result);
//};
#else
void Notebook::set_media_file_names(string names) { 
	if (media_file_names != NULL) {
		delete [] media_file_names;
	};
	media_file_names = names;
};

void Notebook::update_media_file_names(Sprites *sprites_with_media_to_save) {
	set_media_file_names(::media_file_names(sprites_with_media_to_save,media_file_names));
};
#endif

void Notebook::xml(xml_element *element, xml_document *document) {
   // what about "original" flag??
	if (pages == NULL) { // new on 280103 to convert if haven't already before saving
      compute_pages();
   };
	// experiment 040903
	//if (file_name != NULL &&
	//	 ((tt_dump_reason == DUMPING_NOTEBOOK_NAMES && notebook_is_only_on_default_user_directory()) || // new on 300803
	//	  (tt_saving_media && user_notebook() && tt_dump_reason != DUMPING_NOTEBOOK_CONTENTS))) { // tt_dump_reason != DUMPING_NOTEBOOK_CONTENTS stops infinite recursion
	//	UnwindProtect<DumpReason> set(tt_dump_reason,DUMPING_NOTEBOOK_CONTENTS);
	//	UnwindProtect<boolean> set2(tt_logging,FALSE); // new on 040903
	//	dump();
	//};
	boolean changed = return_changed_since_last_save(); // isn't this obsolete now? asked 090703
	//if (xml_location_of_last_good_full_save > 0) { // new on 190703 -- obsolete 160204
	//	xml_set_attribute(element,L"IncludeAndProcess",xml_location_of_last_good_full_save); 
	//	// so this uses the old XML and the page number and geometry stuff below
	//} else 
	if (file_name == NULL || tt_dump_reason == DUMPING_TO_CLIPBOARD || 
		 tt_dump_reason == DUMPING_NOTEBOOK_CONTENTS || // new on 300103 since used when adding a notebook to a notebook
		 changed) { 
		// commented out the following since moved up - to dump and then save with the name
//		 notebook_is_only_on_default_user_directory() ||	
			 // new on 290803 since if using a "virtual" copy of default user's notebook it is now time to make it non-virtual
		// following commented out on 200903 since if saving city then main notebook is treated specially anyway
//		 (tt_saving_media && main_notebook())) {
			 // new on 190803 -- added main_notebook() so its subnotebooks are checked - 
			 // included user_notebook() prior to 300803 but those are handled above now
	// commented out the following on 310803 since no need to do this extra work anymore
//				  (tt_create_new_notebooks && tt_dump_reason != DUMPING_NOTEBOOK_NAMES && !main_notebook())) { 
			 // prior to 220803 was strcmp(file_name,"bok") != 0)) { 
		// bok file contains references to the others - needn't be expanded
		// added tt_dump_reason != DUMPING_NOTEBOOK_NAMES on 140803
		// added return_changed_since_last_save() on 161202 since if contents changed need to dump
      // commented out on 041102 || (leader == NULL && !in_notebook()) || (leader != NULL && leader->kind_of() != PROGRAM_PAD)) {
      // unnamed or top-level notebooks need to dump their pages (and if going to the clipboard then everything at all levels is dumped)
      if (label != NULL && picture == NULL && !main_notebook()) { // don't bother if label is just picture
         long text_length;
         string text;
         label->current_text(text,text_length);
         // prior to 051102 this dumped the label object rather than just the text
         xml_append_child(xml_character_data_element(text,L"Label",document),element); 
			// prior to 220203 was create_xml_element(L"Label",document,text,text_length),element);
			int insertion_point = label->return_insertion_point();
			if (insertion_point >= 0) { // new on 120604
				xml_set_attribute(element,L"EditInsertionPoint",insertion_point);
			};
      };
//#if TT_DEBUG_ON
//		if (pages_count == 0) {
//			log("debug this");
//		};
//#endif
      xml_append_child(create_xml_element(L"NumberOfPages",document,pages_count),element);
//      tt_dumping_to_notebook_page = TRUE; // new on 211102
		UnwindProtect<DumpReason> save(tt_dump_reason);
		if (main_notebook()) { // experiment on 300803  && !notebook_is_only_on_default_user_directory()) {
			if (tt_dump_reason != DUMPING_TO_CLIPBOARD) { 
				// condition new on 041004 to leave tt_dump_reason alone if saving notebook without rest of city since otherwise subnotebooks refer
				// to files that aren't exported.
				tt_dump_reason = DUMPING_NOTEBOOK_NAMES; // don't recursively dump the notebooks in the main notebook
			};
		} else { 
			tt_dump_reason = DUMPING_TO_NOTEBOOK_PAGE;
		};
		// following new with ALPHA_FEATURE (271003)
		// commented out on 100205 since if saving something with multiple notebooks this loses the media files of the earlier ones
		//if (tt_extra_media_file_names != NULL) { // new on 280204 since collect up the media file names from each page below now
		//	delete [] tt_extra_media_file_names;
		//	tt_extra_media_file_names = NULL;
		//};
		//boolean need_to_deal_with_media_files = (file_name != NULL && tt_save_media_in_sub_notebooks);
		//UnwindProtectIf<Sprites *> set1(need_to_deal_with_media_files,tt_sprites_with_media_to_save,NULL); // new on 200903
		//UnwindProtectIf<boolean> set2(need_to_deal_with_media_files,tt_saving_media,TRUE); 
		// new on 200903 to ensure that files can be transported
		// rewrote the above on 220903 since need to control the unwinding so it only happens if truely set
		//if (file_name != NULL) { 
		// condition new on 210903 so that notebooks within notebooks don't hide their media from their parent
		// (unless it is the main notebook)
		//	tt_sprites_with_media_to_save = NULL;
		//	if (tt_save_media_in_sub_notebooks) {
		//		tt_saving_media = TRUE;
		//	};
		//};
		// commenting the following out an experiment on 051102
      //if (left_page != NULL) left_page->set_in_notebook(this); // left and right page made normal while saving
      //if (right_page != NULL) right_page->set_in_notebook(this);
      for (int page_number = 1; page_number <= pages_count; page_number++) {
         xml_append_child(page_xml(page_number,document),element); // rewritten on 221102 -- much much faster
			if (pages[page_number-1]) { // new on 280204 to add the pages media file names to the notebooks
				string page_media_file_names = pages[page_number-1]->return_media_file_names();
				if (page_media_file_names != NULL) {
					add_new_media_file_names(page_media_file_names,tt_extra_media_file_names);
				};
			};
         //Sprite *sprite = page_sprite(page_number,FALSE);
         //if (sprite != NULL) {
         //   xml_element *page_element = create_xml_element(L"Page",document,page_number);
         //   xml_append_child(page_element,element);
         //   sprite->xml_create_and_append_element(page_element,document);
         //};
      };
// commented out around 271003
		//if (file_name != NULL && tt_sprites_with_media_to_save != NULL) { // added file_name != NULL on 210903
		//	update_media_file_names(tt_sprites_with_media_to_save);
		//	just_destroy_list(tt_sprites_with_media_to_save);
		//};
//      tt_dumping_to_notebook_page = FALSE;
		set_changed_since_last_save(FALSE); 
		// restored on 190703 changed); // new on 161202 -- was FALSE prior to 120603 but that interfered with time travel
   } else {
      xml_append_child(create_xml_element(L"FileName",document,file_name),element); 
	};
   // following was only on the first branch of the conditional above prior to 061102
   //if (left_page != NULL) left_page->set_in_notebook(NULL); // restore
   //if (right_page != NULL) right_page->set_in_notebook(NULL); // restore
   int current_page_number = left_page_number();
   // this is after the pages are defined so it will switch_contents properly
   xml_append_child(create_xml_element(L"OpenToPage",document,current_page_number),element);
   if (toolbox_tool(this) && !dumping_an_individual()) {
		// second condition added on 220203 so if you load someone else's notebook your's isn't clobbered
//      xml_append_child(create_xml_element(L"LivesInTooly",document),element);
      xml_set_attribute(element,L"LivesInTooly",1); // new on 171102
   };
   if (id > 0 && tt_dump_reason != DUMPING_TO_CLIPBOARD && tt_dump_reason != DUMPING_TO_NOTEBOOK_PAGE && 
		 tt_saved_city_file_name_when_demo_ends == NULL) { // new on 171102
		// extra conditions added on 281004 so this only saved with a city (or unbamming)
      xml_set_attribute(element,L"ID",id);
   };
   if (built_in_notebook != USER_NOTEBOOK) { // new on 171102
      xml_set_attribute(element,L"BuiltInCode",(int) built_in_notebook);
   };
	if (built_in_notebook == LOCAL_REMOTES_NOTEBOOK && picture != NULL) {
      xml_element *controlee_element = create_xml_element(L"RemoteControlsFor",document); 
		// new on 220803 so used in both branches below
		if (being_dumped_together(picture,this)) {
			// moved here on 261102 from no file_name branch
			// needs to be before Page elements since they might need to know their controlee
			//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
			//	picture->check_for_independence();
			//};
			picture->xml_create_and_append_element(controlee_element,document,TRUE)->Release(); // rewritten on 220803
//			picture->add_xml(element,L"RemoteControlsFor",document); // simplified on 171102
		} else {
			picture->xml_create_external_link_and_append_element(controlee_element,document); 
			// new on 150703 -- changed to controlee_element on 220803
		};
		xml_append_child(controlee_element,element); // moved here on 190804
	};
	if (page_being_defined != NULL) {
		xml_append_child(create_xml_element_shared(L"PageBeingDefined",document,page_being_defined),element); // new on 171203
	};
//#if TT_ALPHA_FEATURE
//	if (media_file_names != NULL) { // new on 301103 so save these - are they maintained OK?
//		// do later
//	};
//#endif
   // didn't save changed_since_last_save since session only 
   // default_favorite_llx, default_favorite_lly -- since set when created
   // favorite_llx, favorite_lly, favorite_width, favorite_height since a property of the user not the city or whatever
   // language, version_number isn't used in load XML
   // 
   Sprite::xml(element,document);
};

xml_element *Notebook::xml_set_special_attributes(xml_element *element, xml_document *document) {
	// new on 201202 -- these were always set earlier
	xml_element *special_element = Sprite::xml_set_special_attributes(element,document);
   if (add_to_number != DONT_ADD_TO_PAGE) { // new on 171102
      xml_set_attribute(special_element,L"AddToPageCode",(int) add_to_number);
   };
   if (!original) { // new on 171102
      xml_set_attribute(special_element,L"NotOriginal",1);
   };
   if (running_to_hand_flag) { // new on 171102
      xml_set_attribute(special_element,L"RunningToHand",1);
   };
   if (left_page_current) { // new on 171102
      xml_set_attribute(special_element,L"LeftPageCurrent",1);
   };
   if (left_page_selected) { // new on 171102
      xml_set_attribute(special_element,L"LeftPageSelected",1);
   };
   if (overlay_status != OVERLAY_OFF) { // new on 171102
      xml_set_attribute(special_element,L"OverlayStatusCode",(int) overlay_status);
   };
   if (page_number_offset != 0) { // new on 181102
      xml_set_attribute(special_element,L"PageNumberOffset",page_number_offset);
   };
	if (last_key_for_label) { // new on 171102
      xml_set_attribute(special_element,L"LastKeyForLabel",1);
   };
   if (saved_page_number >= 0) { // new on 151102
      xml_set_attribute(special_element,L"SavedPageNumber",saved_page_number); // renamed to SavedPageNumber on 181102
   };
   if (next_page_number != 0) { // new on 181102
      xml_set_attribute(special_element,L"NextPageNumber",next_page_number);
   };
   if (recent_page_number != 1) { // new on 181102
      xml_set_attribute(special_element,L"RecentPageNumber",recent_page_number);
   };
	return(special_element);
};

void Notebook::add_special_xml(xml_element *element, xml_document *document) {
   if (page_flipping != NULL) { // new on 171102
      // since is L"TurningNotebookPage" no need to wrap it
		// Don't need to check for independence for time travel include segments
      page_flipping->xml_create_and_append_element(element,document)->Release();
   };
   if (page_number_item != NULL) { // new on 181102
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	page_number_item->check_for_independence();
		//};
      page_number_item->add_xml(element,L"PageNumberItem",document);
   };
	// experiment on 260803 to not bother to save selections since they can be to a notebook's left 
	// or right page which is a pain to get right
	//if (left_page != NULL) { // new on 250803 since can be current selection
	//	left_page->add_xml(element,L"LeftPage",document);
	//};
	//if (right_page != NULL) { // new on 250803 since can be current selection
	//	right_page->add_xml(element,L"RightPage",document);
	//};
	Sprite::add_special_xml(element,document);
};

boolean Notebook::xml_get_special_attributes(xml_node *node) {
	// moved here on 201202
	saved_page_number = xml_get_attribute_int(node,L"SavedPageNumber",0); // prior to 210404 was saved_page_number
   next_page_number = xml_get_attribute_int(node,L"NextPageNumber",0); // prior to 210404 was next_page_number 
   recent_page_number = xml_get_attribute_int(node,L"RecentPageNumber",1); // prior to 210404 was recent_page_number
	left_page_current = xml_get_attribute_int(node,L"LeftPageCurrent",0); // prior to 210404 was left_page_current
   left_page_selected = xml_get_attribute_int(node,L"LeftPageSelected",0); // prior to 210404 was left_page_selected
   overlay_status = (OverlayStatus) xml_get_attribute_int(node,L"OverlayStatusCode",OVERLAY_OFF); 
	// prior to 210404 was overlay_status
   page_number_offset = xml_get_attribute_int(node,L"PageNumberOffset",0); // prior to 210404 was page_number_offset
	return(Sprite::xml_get_special_attributes(node));
};

xml_element *Notebook::page_xml(int page_number, xml_document *document) {
   if (pages == NULL) compute_pages();
   if (page_number > pages_count) return(NULL);
   if (pages[page_number-1] == NULL) {
      return(NULL);
   } else {
		// rewritten on 240105 since this broke when saving a notebook for the second time -- 
		// this wasn't the problem -- it was when loading
//		return(pages[page_number-1]->top_level_xml(document,page_number,this)); 
		// some will create page_element from page number as below but not those that have XML already
      xml_element *page_element = create_xml_element(L"Page",document,page_number); 
		// rewrote the following on 200803 to once again use top_level_xml but a modified version 
      pages[page_number-1]->top_level_xml(document,page_element,this); 
//		xml_element *element = pages[page_number-1]->xml_create_and_append_element(page_element,document);
		//#if TT_DEBUG_ON
//		xml_debug_node(element,L"debug this");
//#endif
		// rewritten again on 220203 to use xml_create_and_append_element rather than top_level_xml since 
		// this avoids infinite recursions and does
		// much more useful work than the following:
		// prior to 190103 above was xml_create_element -- 
		// rewrote on 210203 since this can cause infinite recursion via for example robot's PageOffset
//		Sprite *content = pages[page_number-1]->contents();
		//if (content != NULL) {
		//	xml_append_child(content->xml_create_element(document),page_element);
		//};
		// besides pages should already be isolated from each other
//      pages[page_number-1]->xml(element,document);
 //     xml_append_child(element,page_element); // moved up on 210203
      return(page_element);
   };
};

boolean Notebook::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) { 
      case PAGE_TAG: {
         int page_number = xml_get_int(node,0);
			//if (tt_debug_mode == 240103) { // including Release version
			//	tt_error_file() << "Loading page number " << page_number << endl;
			//};
         if (page_number == 0) return(TRUE); // couldn't parse as number (or was 0 but these are one-indexed) -- warn??
//         set_page_internal(page_number,xml_load_sprite(first_node_that_is_an_element(node)));
//			xml_node *element = first_node_that_is_an_element(node);
			xml_node *element;
			if (tt_log_version_number >= 69) {
				element = first_element_with_tag_name(node,L"Page"); // rewritten on 300105
			} else {
				// restored for old demos on 310105 -- needed??
				element = first_node_that_is_an_element(node);
			};	
			if (element == NULL) {
				element = node; // new on 240105 since pages can be saved as <page>1<page ... ></page></page>
			};
			XMLPage *page = new XMLPage(xml_node_to_element(element));
			set_page_internal(page_number,page); // prior to 190703 this did a very dangerous coercion
			if (xml_has_attribute(element,L"GUID")) { // someone is pointing to this page (e.g. current selection) -- new on 250803
				page->contents()->increment_ref_count(); // this will cause the sprite to be computed and "protected" - hope this is right
			} else if (xml_get_attribute_int(element,L"PageInstantiated",0)) { // new on 170105
				// put on else branch on 240105 since no need to check if already did it above
				page->contents(); // so it is active if a sensor same during replay
			};
			// following new with ALPHA_FEATURE (271003)
			// replaced the node arg with element on 240105 for the following 2 calls
			page->set_media_file_names(xml_get_attribute_string(element,L"Media",NULL)); // node was element prior to 270204
//#if TT_DEBUG_ON
//				xml_debug_node(node);
//				xml_debug_node(element);
//#endif
         return(TRUE);
                     };
      case NUMBER_OF_PAGES_TAG:
         expand_pages(xml_get_int(node,0));
         return(TRUE);
      case OPEN_TO_PAGE_TAG:
         go_to_page(xml_get_int(node,1));
         compute_pages(); // just in case - to update it
         return(TRUE);
      //case LIVES_IN_TOOLY_TAG: // made obsolete on 171102
      //   become_main_notebook();
      //   return(TRUE);
      case FILE_NAME_TAG: 
         set_file_name(xml_get_narrow_text_copy(node));
         load(); // new on 061102 to actually use the file name
		   return(TRUE);
      case REMOTE_CONTROLS_FOR_TAG: 
			just_set_picture((Picture *) xml_get_sprite(node,picture,PICTURE));
         //Sprite *picture = xml_get_sprite(node);
         //if (picture != NULL) {
         //   if (picture->kind_of() == PICTURE) {
         //      just_set_picture((Picture *) picture);
         //   } else {
         //      picture->destroy(); // warn??
         //   };
         //};
         return(TRUE);
      case LABEL_TAG:
         set_label(make_label(xml_get_text_copy(node))); // new on 081002
         return(TRUE);
      case TURNING_NOTEBOOK_PAGE_TAG:
         page_flipping = xml_load_sprite(node,tag,page_flipping);
         return(TRUE);
      case PAGE_NUMBER_ITEM_TAG:
			if (page_number_item != NULL) { // new on 070103
				page_number_item->destroy();
			};
         page_number_item = xml_get_sprite(node);
         return(TRUE);
		// experiment on 260803 to not bother to save selections since they can be to a notebook's left or right page which is a pain to get right
		//case LEFT_PAGE_TAG: // new on 250803
		//	left_page = xml_get_sprite(node,left_page);
		//	if (left_page != NULL && left_page->pointer_to_leader() == NULL) {
		//		fit_on_page(TRUE);
		//		add_follower(left_page);
		//	};
		//	return(TRUE);
		//case RIGHT_PAGE_TAG:
		//	right_page = xml_get_sprite(node,right_page);
		//	if (right_page != NULL && right_page->pointer_to_leader() == NULL) {
		//		fit_on_page(FALSE);
		//		add_follower(right_page);
		//	};
		//	return(TRUE);
      case NO_MORE_TAGS: // new on 151002
         set_active(TRUE); // stop pretending since can never be turned off 
         id = xml_get_attribute_int(node,L"ID",id);
         last_key_for_label = xml_get_attribute_int(node,L"LastKeyForLabel",last_key_for_label);
         built_in_notebook = (BuiltinNotebooks) xml_get_attribute_int(node,L"BuiltInCode",built_in_notebook);
			if (built_in_notebook != USER_NOTEBOOK && pages == NULL) {
				load(); // new on 070803 so this works even if not dumped
			};
			// commented out on 170105 since handled more generally now
//			if (built_in_notebook == GLOBAL_REMOTES_NOTEBOOK && tt_loading_a_city) { 
			// tt_loading_a_city added on 010105 since broke intro_v2 (segment 99)
//				// new on 301204 to fix a time travel bug where when you jump the clipboard isn't read 
				// if the global_remotes notebook is open to another page
////				pages[29]->contents(); // CLIPBOARD_REMOTE needs to be instantiated		
//				if (tt_log_in_archive == NULL || tt_log_version_number >= 65) {
//					// removed || strstr(tt_log_in_archive,"pongact") == NULL) on 070105 since this broke some old demos 
			      // (besides pongact1_v2)
//					// and only fixed the second half of Lennart's u5_5
//					// new on 010105 since these demos worked before and are broken otherwise (e.g. segment 52 in pongact1_v2)
//					for (int i = 0; i < pages_count; i++) {
//						if (pages[i] != NULL) { // condition new on 110105 due to crash report from Lennart (was being sucked up)
//							pages[i]->contents(); // instantiate them all since at least clipboard and random remote need to be
//						};
//					};
//				};
//			};
         add_to_number = (AddToPageNumber) xml_get_attribute_int(node,L"AddToPageCode",DONT_ADD_TO_PAGE); 
			// prior to 210404 was add_to_number
         original = !xml_get_attribute_int(node,L"NotOriginal",0); // prior to 210404 was !original
         running_to_hand_flag = xml_get_attribute_int(node,L"RunningToHand",0); // prior to 210404 0 was running_to_hand_flag
			if (label != NULL) { // new on 120406
				int insertion_point = xml_get_attribute_int(node,L"EditInsertionPoint",-1); 
				if (insertion_point >= 0) {
					label->set_insertion_point(insertion_point);
				};
			};
         Sprite::handle_xml_tag(tag,node);
			// following commented out on 250803 since now save left and right page
//       switch_contents(); // go_to_page doesn't do this if already at that page -- moved here on 181102
         return(TRUE);
		case PAGE_BEING_DEFINED_TAG: // new on 171203
			page_being_defined = xml_get_sprite(node);
			return(page_being_defined != NULL);
		case NOTEBOOK_TAG: // shouldn't happen but 2.71 created these
//			xml_debug_node(node,L"Notebook tag");
			return(xml_entity(node,this)); // experiment on 060603 since dmo logs create these
      default:
         return(Sprite::handle_xml_tag(tag,node)); 
   };
};

Sprite *Notebook::page_sprite(int page_number, boolean expand_if_notebook_defined_by_file) {
   // 1-indexing
   if (pages == NULL) compute_pages();
   if (page_number > pages_count) return(NULL);
   Sprite *sprite;
   if (pages[page_number-1] == NULL) {
      sprite = NULL;
   } else {
      sprite = pages[page_number-1]->contents();
   };
   if (expand_if_notebook_defined_by_file && sprite != NULL && sprite->kind_of() == PROGRAM_PAD) {
      Notebook *notebook = (Notebook *) sprite;
      notebook->load();
      notebook->switch_contents(); // to initialize it properly
   };
	//if (sprite != NULL && sprite->pointer_to_background() != pointer_to_background()) {
	//	sprite->set_background(pointer_to_background(),TRUE,FALSE); // new on 120804 so that decoration sensors work
	//};
   return(sprite);
};         

void Notebook::set_page_internal(int page_number, Page *page) { // updated 201020 was Sprite *sprite
   // 1-indexing
   if (page_number > pages_count) { // prior to 280103 was >= but expand_pages returns immediately if they are equal
      expand_pages(page_number);
   } else if (pages == NULL) {
      compute_pages();
   };
   int index = page_number-1; // convert to zero indexing
	// following restored on 260803 -- I think this was a serious memory leak
   if (pages[index] != NULL) { // seems caller has already taken care of this -- worry about this in new scheme 181002
 //     delete pages[index];
		pages[index]->destroy(); // rewritten on 240204 to respect ref counts
   };
   //if (sprite == NULL) {
   //   pages[index] = NULL;
   //} else {
   //   pages[index] = new SpritePage(sprite);
   //};
   //if (sprite != NULL) sprite->set_in_notebook(this); // new on 210802
   pages[index] = page;
	//if (page == NULL && tt_debug_mode == 240103) { // including Release version
	//	tt_error_file() << "Page is empty" << endl;
	//};
	if (page != NULL) {
		page->increment_ref_count(); // new on 240204
		page->set_in_notebook(this); // new on 210802
	};
};

void Notebook::compute_pages(boolean make_empty_notebook_if_need_be) {
   if (pages != NULL) return; // already done
   if (pages_count == 0) { // haven't converted yet (or is an empty notebook)
      pages_count = last_non_blank_page_number();
      if (pages_count == 0) {
         if (make_empty_notebook_if_need_be) {
            pages_count = 2; // blank notebook - new on 231002
            pages = new PagePointer[pages_count];
            pages[0] = NULL;
            pages[1] = NULL;
         };
         return;
      };
      if (pages_count&1) pages_count++; // if odd then include facing page
   };
   unsigned long saved_seed = random_number_seed(); // so it doesn't change if this updates by recreating and encoding
//   sprite_pages = new SpritePointer[pages_count];
   pages = new PagePointer[pages_count];
   for (int i = 1; i <= pages_count; i++) {
      pages[i-1] = old_pages->nth_old_format_page(i,version_number);
		if (pages[i-1] != NULL) {
			pages[i-1]->increment_ref_count(); // new on 240204
		};
      //Sprite *sprite = old_pages->nth(i,version_number,this,picture);
      //if (sprite != NULL) {
      //   pages[i-1] = new SpritePage(sprite);
      //} else {
      //   pages[i-1] = NULL;
      //};
      //if (sprite != NULL) sprite->set_in_notebook(this); // so it won't run
   };
	set_random_number_seed(saved_seed); // above might load RANDOM_REMOTE and change the seed and break logging
   //// no longer need the pages
   //remove_pages(); // too soon
};

void Notebook::expand_pages(int new_pages_count) {
	if (new_pages_count <= pages_count) {
		return; // no need
	};
//   SpritePointer *new_sprite_pages = new SpritePointer[new_pages_count];
   PagePointer *new_pages = new PagePointer[new_pages_count];
   int i = 0;
   if (pages == NULL) { // compute them
      unsigned long saved_seed = random_number_seed(); // so it doesn't change if this updates by recreating and encoding
      for (; i < pages_count; i++) {
         new_pages[i-1] = old_pages->nth_old_format_page(i,version_number); // this was pages[] prior to 160103
         //Sprite *sprite = old_pages->nth(i+1,version_number,this,picture);
         //if (sprite != NULL) {
         //   new_pages[i] = new SpritePage(sprite);
         //} else {
         //   new_pages[i] = NULL;
         //};
         //if (sprite != NULL) sprite->set_in_notebook(this); // so it won't run
      };
      set_random_number_seed(saved_seed); 
   } else {
      for (; i < pages_count; i++) {
         new_pages[i] = pages[i];
      };
   };
   for (; i < new_pages_count; i++) {
      new_pages[i] = NULL;
   };
   if (pages != NULL) delete [] pages;
   pages = new_pages;
   pages_count = new_pages_count;
};

void Notebook::initialize_pages(PagePointer *shared_pages, int new_pages_count) {
   if (pages != NULL) {
      delete [] pages; // should both be NULL (empty notebook) -- new on 231002
   };
   pages = new PagePointer[new_pages_count];
   for (int i = 0; i < new_pages_count; i++) {
      pages[i] = shared_pages[i];
      if (shared_pages[i] != NULL) shared_pages[i]->increment_ref_count();
   };
   pages_count = new_pages_count;
   if (pages_count > 0) loaded = TRUE; // if and when it gets file_name
};

boolean Notebook::pages_instatiated() { // new on 170105
	for (int i = 0; i < pages_count; i++) {
		if (pages[i] != NULL && !pages[i]->instantiated()) {
			return(FALSE);
		};
	};
	return(TRUE);
};

void Notebook::instantiate_pages() { // new on 170105
	for (int i = 0; i < pages_count; i++) {
		if (pages[i] != NULL) {
			pages[i]->contents();
		};
	};
};

//PagePointer *Notebook::transfer_pages(int &count) { // made obsolete on 161202
//   count = pages_count;
//   PagePointer *result = pages;
//   pages = NULL; // finished with transfer
//   pages_count = 0;
//   if (left_page != NULL) remove_follower(left_page);
//   if (right_page != NULL) remove_follower(right_page);
//   return(result);
//};

#endif

void Notebook::convert_to_latest_version_number(short unsigned int page_count) {
	// moved here on 250299 since old and new version now can co-exist
	// but not in the same notebook
	if (version_number < latest_version_number) { // tt_debug_mode == 50100 && // added last on 050100 so by default don't convert - was != prior to 060201
//		 version_number+2 < first_version_to_save_thought_bubbles_disconnected) { // commented out on 020702 since interferes with updating to new numbers
		// new on 191200 since these changes don't require conversion
		 // don't need to convert since changes won't show up in notebooks
//		 !(version_number == 11 && latest_version_number == first_version_to_save_city_items)) { // commented out on 050202 since need to convert to new number scheme
		if (page_count <= 0) {
			page_count = 0;
			if (pages != NULL) {
				page_count = last_non_blank_page_number();
				if (page_count == 0) { // empty
					page_count = 2;
				};
			};
		};
		for (int page = 1; page <= page_count; page++) {
			Sprite *sprite = old_pages->nth(page,version_number);
			if (sprite != NULL) {
//				if (sprite->kind_of() != PROGRAM_PAD) {
					long encoding_length;
					// encoding should be "top level"
					// commented this out on 030702 since is already done by return_encoding and it does a better job
					// i.e. it includes tt_sprites_being_dumped_ok_for_eq
//					SpritePointer *saved_tt_sprites_being_dumped = tt_sprites_being_dumped;
//					tt_sprites_being_dumped = NULL;
//					int saved_tt_sprites_being_dumped_size = tt_sprites_being_dumped_size;
//					tt_sprites_being_dumped_size = 0;
//					int saved_tt_number_of_sprites_dumped_so_far = tt_number_of_sprites_dumped_so_far;
//					tt_number_of_sprites_dumped_so_far = 0;
					string encoding_string = sprite->return_encoding(encoding_length,TRUE,FALSE);
//					release_sprites_being_dumped(); // new on 080699 -- commented out on 190702 since return_encoding deals with this better and this messes up saving cities with old notebooks in them
//					tt_sprites_being_dumped = saved_tt_sprites_being_dumped;
//					tt_sprites_being_dumped_size = saved_tt_sprites_being_dumped_size;
//					tt_number_of_sprites_dumped_so_far = saved_tt_number_of_sprites_dumped_so_far; // new on 110601
					old_pages->set_page(page,encoding_string,encoding_length);
#if TT_DEBUG_ON
					tt_error_file() << "Converted page " << page 
									<< " from file format version " << (int) version_number
									<< " to version " << latest_version_number 
									<< "(encoding length is " << encoding_length << ")" << endl;
#endif
//				}; // else program pad is just the file name
				sprite->destroy(); 
			};
		};
		version_number = latest_version_number;
      dump(); // new on 220702
	};
};

Notebook *load_notebook(InputStream *pad_in,
								int notebook_version, NaturalLanguage language, boolean go_to_page_number,
								Sprite *container, Picture *controlee, long index, boolean descendants_inactive) {
	UnwindProtect<boolean> set(tt_loading_a_notebook,TRUE);
	Sprite *comment = NULL;
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299) {
		tt_error_file() << "About to load a notebook - version is " << notebook_version
						<< " - file position is " << (int) pad_in->tellg() << endl;
	};
#endif
	if (notebook_version >= first_version_to_save_copied_nests) {
		comment = load_item(pad_in,notebook_version,language);
	};
	Text *label = NULL;
	if (notebook_version >= first_version_to_label_notebooks) {
		label = (Text *) load_item(pad_in,notebook_version,language);
	};
	unsigned char page_number = (unsigned char) pad_in->get();
	unsigned char file_name_size = (unsigned char) pad_in->get();
	Notebook *notebook = NULL;
	if (file_name_size == 0) {
		notebook = new Notebook(0,0,0,NULL);
      if (container != NULL) {  // new on 160702
         if (container->kind_of() == PROGRAM_PAD) {
            notebook->set_in_notebook((Notebook *) container,FALSE); // no point recuring
         } else if (container->kind_of() == VACUUM) {
            notebook->set_inside_vacuum(TRUE,FALSE);
         };
      }; 
      associate_shared_item(index,notebook);
		notebook->load_notebook(pad_in);
	} else {
		char file_name[MAX_PATH];// = new char[file_name_size+1];
		pad_in->read((string) file_name,file_name_size);
		file_name[file_name_size] = 0; // terminate the string
		if (container != NULL && container->kind_of() == PROGRAM_PAD) {
			ascii_string container_file_name = ((Notebook *) container)->pointer_to_file_name();
			if (container_file_name != NULL && strcmp(container_file_name,"bok") != 0) {
				// something is wrong - user notebook's shouldn't have subnotebooks with file names
				log("A notebook inside a notebook wasn't stored correctly. Sorry, it is lost.",FALSE,TRUE);
				notebook = new Notebook(0,0,0,NULL); // ignoring file name
			};
		};
		if (notebook == NULL) { // not just created above
			notebook = new Notebook(0,0,0,copy_ascii_string(file_name)); // beginning 210802 the caller copies the file name
		};
      associate_shared_item(index,notebook);
		if (strcmp(file_name,"rem") == 0) {
			Sprite *sprite = load_item(pad_in,notebook_version,language);
			if (sprite != NULL) { // might not be one
				if (sprite->kind_of() == PICTURE) {
					Picture *picture = (Picture *) sprite;
					notebook->set_picture(picture);
					picture->load_geometry(pad_in,notebook_version); // dumped just in case needed
					associate_shared_item(index+1,picture);
				} else {
					say_error(IDS_COULDNT_RECOGNIZE_NOTEBOOK_FILE,TRUE);
				};
			};
		};
	};
	if (!tt_loading_robot_notebook && go_to_page_number) {
		notebook->go_to_page(page_number);
	}; // otherwise this can be a big waste of time and if called recursively can cause big problems
	notebook->set_comment(comment);
	if (label != NULL &&
		 // commented out the following so user notebooks can replace built-in ones without label change
		 // restored on 110101 for the main notebook only (so double clicking on a CTY file shows the right name)
		 // changed user_notebook to main_notebook on 120101
		 !notebook->main_notebook()) { // && notebook->pointer_to_label() == NULL)) { 
		// conditions added on 140300 so built in notebooks like "4" don't lose their label when constructed
		notebook->set_label(label);
	};
	if (notebook_version >= first_version_to_save_programmer_and_remotes_accurately) { // new on 050100
		unsigned char flags = pad_in->get();
		boolean friend_of_tooly = (flags&1);
		if (friend_of_tooly && !tt_loading_to_ignore) { // added !tt_loading_to_ignore on 181202
         notebook->become_main_notebook();
		};
		notebook->set_infinite_stack(!!(flags&(1<<1)));
		if ((flags&(1<<2))) { // new on
			notebook->become_blank();
		};
	};
	return(notebook);
};

void Notebook::become_main_notebook(){
   // abstracted out on 200802
	if (tt_toolbox == NULL) {
		tt_toolbox = new Toolbox();
		tt_toolbox->initialize();
	}; 
	// new on 251000 
	Notebook *old_notebook = pointer_to_toolbox_notebook(); // re-written on 261000 so it works whether tt_toolbox is new or not
	set_toolbox_notebook(this);
	if (old_notebook != NULL && old_notebook->pointer_to_leader() == NULL && old_notebook->pointer_to_background() == NULL) {
		old_notebook->destroy(); // do this after the above so isn't protected by tt_toolbox
	};
	update_textual_label_size_and_location(); // added 060100 so label is updated
};

void Notebook::which_side(TTRegion *region, boolean &left_item_selected, boolean &right_item_selected) {
	TTRegion left_region;
	if (left_page != NULL) {
		left_page->true_region(left_region); // was full_region
	} else {
		page_region(TRUE,left_region);
	};
	left_item_selected = region->overlaps(left_region);
	TTRegion right_region;
	if (right_page != NULL) {
		right_page->true_region(right_region);
	} else {
		page_region(FALSE,right_region);
	};
	right_item_selected = region->overlaps(right_region);
	if (left_item_selected && right_item_selected) {
		// pointing to both so count it as neither
		left_item_selected = FALSE;
		right_item_selected = FALSE;
	};
};

ReleaseStatus Notebook::item_released_on_top(Sprite *item, Sprite *by, SpritePointer &recipient, int level) {
//	if (leader != NULL) { // e.g. flipped side of picture or another notebook
//		return(RELEASE_IGNORED);
//	};
#if TT_DEBUG_ON
	if (item->debug_counter == tt_debug_target) {
		tt_error_file() << "Notebook#" << debug_counter << " receiving debug target." << endl;
	};
#endif
	// consider if I can share more here with receive_item (when non-blank)
	if (blank) { // new 220500 -- removed conditional on 220900 so robots can be trained to drop new items on notebooks
		if (receive_item(item,by,default_duration(),this,item)) {
			record_event(RELEASE_ITEM_ON,by,floor,this); // new on 131200
			return(RELEASE_HANDLED); // was RELEASE_ACCEPTED prior to 131200
		} else { 
			return(RELEASE_IGNORED);
		};
	};
	if (contained_in(CUBBY)) { 
		return(Sprite::item_released_on_top(item,by,recipient,level));
   };
//	tt_last_notebook = this;
	TTRegion region;
	item->active_region(region);
	boolean left_item_selected,right_item_selected;
	which_side(&region,left_item_selected,right_item_selected);	
	left_page_selected = (flag) closer_to_left_page(&region);
	if (!left_item_selected && !right_item_selected) { // missed both
		item->full_region(region);
		which_side(&region,left_item_selected,right_item_selected);
	};
	Sprite *current_page = NULL;
	if (left_item_selected) current_page = left_page;
	if (right_item_selected) current_page = right_page;
	boolean nothing_selected = (left_page == NULL || right_page == NULL);
   if (nothing_selected) {
      if (left_page == NULL && right_page == NULL) {
         left_page_selected = !right_item_selected;
      } else {
         left_page_selected = (left_page == NULL);
      };
   };
	switch(item->kind_of()) {
	  case TOOLBOX:
	  case VACUUM:
	  case EXPANDER:
	  case COPIER:
		 return(RELEASE_IGNORED);
//	  case PROGRAM_PAD:
//		 if (nothing_selected &&
//			  ((Notebook *) item)->pointer_to_picture() != NULL) {
//			 say("Sorry, you can't add a notebook of remote controls for a picture to another notebook.");
//			 by->abort_action_if_robot(action_status,"");
//			 return(RELEASE_IGNORED);
//		 };
	};
//	if (infinite_stack() && // notebook is itself in a notebook
//		 item->kind_of() == PROGRAM_PAD) {
//		string my_name = pointer_to_file_name();
//		if (my_name != NULL) {
//			string other_name = ((Notebook *) item)->pointer_to_file_name();
//			if (other_name != NULL && strcmp(my_name,other_name) == 0) {
//				return(RELEASED_ON_ITS_STACK); // just returning the notebook
//			};
//		};
//	};
   boolean equal_to_left_page = equal(left_page,item);
   if (equal_to_left_page || equal(right_page,item)) {
      if (equal_to_left_page) {
         recipient = left_page;
			//if (left_page_is_copy) { // not sure if this should be restored - asked 290103
			//	recipient->increment_ref_count(); // new on 150103 since left and right page are destroyed when page flipping now
			//};
      } else {
         recipient = right_page;
			//if (right_page_is_copy) {
			//	recipient->increment_ref_count(); // new on 150103 since left and right page are destroyed when page flipping now
			//};
      };
 		by->completed();
		return(RELEASED_ON_ITS_STACK);
   };
	if (left_page != NULL && right_page != NULL && item->kind_of() == PICTURE && (item->pointer_to_followers() != NULL || !item->no_flipped_followers() || item->is_flipped())) { // new on 251000
		// moved here on 111200 to stop loss of pictures with stuff on the back
		// added page NULL tests on 121200
		by->abort_action_if_robot(IDS_NOTEBOOK_PAGE_DIDNT_ACCEPT);
		return(RELEASE_IGNORED);
	};
	if (nothing_selected) {
      if (built_in_notebook == LOCAL_REMOTES_NOTEBOOK && tt_debug_mode != 8642) { // this is a notebook of remote controls
			// test was picture != NULL prior to 050201 but that allowed you do change all remote notebooks 
			// if you get one without a picture (not sure you can anymore...)
         if (!told_user(TOLD_REMOTE_NOTEBOOK_READONLY)) {
            say(IDS_CANT_ADD_TO_REMOTE_NOTEBOOK);
            user_told(TOLD_REMOTE_NOTEBOOK_READONLY);
         };
		   by->abort_action_if_robot(-1);
		   return(RELEASE_IGNORED);
      };
		if (floor->inside_rule_body() && tt_toolbox != NULL && this == tt_toolbox_notebook) { // prior to 150704 was pointer_to_toolbox_notebook() == this) {
			// added extra conditions on 190900 so that robots can be trained to work on notebooks other than the "main" one
			say(IDS_CANT_ADD_TO_NOTEBOOK_IN_BUBBLE);
			by->abort_action_if_robot(-1);
			return(RELEASE_IGNORED);
		};
		// just add it but fix it up first if need be
//		if (tt_log_version_number > 14) {
			item->stop_all(FALSE); // new on on 160299 - TRUE changed to FALSE on 180299 so that saving flipped pictures doesn't break connections
//		};
		switch (item->kind_of()) {
			case ROBOT: // why doesn't deletion do this??
				tt_suspensions->remove_if_robot_is((Robot *) item);
//				flush_cache();
				robot_saved();
				break;
				// commented out since now handled for all types(changed on 160299) - was restored and commented out again on 111200
//			case PICTURE:
////				((Picture *) item)->stop_all();
//				if (item->pointer_to_followers() != NULL || !item->no_flipped_followers() || item->is_flipped()) { // new on 251000
//					break;
//				}; // else look up matching picture
			case PROGRAM_PAD: 
				if (tt_toolbox != NULL && tt_toolbox_notebook == this) { // prior to 150704 was pointer_to_toolbox_notebook() == this) {
					// re-organized on 210900 - only semantic change is that temporarly users can save in notebooks other than the main one now 
					if (tt_temporary_user) {
						say(IDS_TEMPORARY_USER_CANT_SAVE,6);
						by->abort_action_if_robot(-1);
						return(RELEASE_IGNORED);
					} else { 
						Notebook *new_page = (Notebook *) item;
						if (left_page_selected) {
							new_page->new_file_name(left_page_number());
						} else {
							new_page->new_file_name(right_page_number());
						};
//						flush_cache();
					};
				};
				break;   
		};
		set_page(item,left_page_selected,by);
		EventCode code;
		if (left_page_selected) {
			code = RELEASE_ITEM_LEFT_OF;
		} else {
			code = RELEASE_ITEM_RIGHT_OF;
		};
		record_event(code,by,floor,this); // new on 220900
//		record_event(RELEASE_ITEM_ON,by,floor,this); // new on 190900
		by->completed();
		return(RELEASED_ON_NOTEBOOK);
	};
	// item dropped should indicate page number
	// but ignore it if bigger than the screen
	// this is really just a special case of ignoring ambiguous drops
	short int indicated_page_number = -1;
	if (item->current_width() < ideal_screen_width ||
		 item->current_height() < ideal_screen_height) {
		indicated_page_number = page_number_of_item(item->dereference()); // dereference added 120500 so pictures of text and numbers works fine as do sensors
	};
	if (indicated_page_number < 0) { // didn't find a page
		if (current_page != NULL && equal(current_page,item)) {
         ::completed(by);
			return(RELEASED_ON_ITS_STACK);
		};
		by->abort_action_if_robot(IDS_NOTEBOOK_PAGE_DIDNT_ACCEPT);
		return(RELEASE_IGNORED);
	};
	// re-wrote the following to the above on 120500 so pictures of text or numbers work fine
/*	short int indicated_page_number;
	switch (item->kind_of()) {	 
		case TEXT: 
//		case PICTURE:  // too often happens accidently and confusing
		case INTEGER:
			// item dropped should indicate page number
			// but ignore it if bigger than the screen
			// this is really just a special case of ignoring ambiguous drops
			if (item->current_width() < ideal_screen_width ||
				 item->current_height() < ideal_screen_height) {
				indicated_page_number = page_number_of_item(item);
				break;
			};
		default:
			if (current_page != NULL && equal(current_page,item)) {
				completed(action_status);
				return(RELEASED_ON_ITS_STACK);
			};
//			return(current_page->item_released_on_top(item,by,recipient,level+1,action_status));
//			} else { // still possible???
				by->abort_action_if_robot(action_status,IDS_NOTEBOOK_PAGE_DIDNT_ACCEPT);
				return(RELEASE_IGNORED);
//			};
	};
*/
	if (indicated_page_number > 0) {// && indicated_page_number <= last_non_blank_page_number()+2) {
		boolean training = (by->kind_of() == ROBOT_IN_TRAINING);
      if (training) {
         set_available_for_indexing(FALSE); // so a path to it is used
      };
		record_event(RELEASE_ITEM_ON,by,floor,this);
      if (training) {
         set_available_for_indexing(TRUE); 
      };
      item->now_released_on(this,by);
		receive_item(item,by,default_duration(750),this,item);
		if (item->kind_of() == INTEGER) {
			user_did(PROGRAM_PAD,NOTEBOOK_RECEIVED_PAGE_NUMBER,by);
		} else {
			user_did(PROGRAM_PAD,NOTEBOOK_RECEIVED_PAGE_TOKEN,by);
		};
		return(RELEASED_ON_NOTEBOOK);
	} else {
		play_sound(PLOP_SOUND);
		by->abort_action_if_robot(IDS_NOTEBOOK_DIDNT_ACCEPT);
		return(RELEASE_IGNORED);
	};
	// following canonicalizes the appearance of page items
//	int saved_page_number = right_page_number();
//	go_to_a_blank_page();
//	go_to_page(saved_page_number);
};

boolean member_sprite_array(Sprite *item, SpritePointer *array, int count) {
	for (int i = 0; i < count; i++) {
		if (array[i] == item) return(TRUE);
	};
	return(FALSE);
};

boolean Notebook::vacuum_if_left_on_floor(SpritePointer *others_being_vaccuumed, int others_count) {
	// don't vacuum remote control notebooks
	return((picture == NULL || member_sprite_array(picture,others_being_vaccuumed,others_count)) && 
		    Sprite::vacuum_if_left_on_floor(others_being_vaccuumed,others_count));
};

short int opposite_page(short int page_number) {
  if (page_number&1) { // odd
	  return((short int) (page_number+1));
  } else {
	  return((short int) (page_number-1));
  };
};

ReleaseStatus Notebook::add_to_side(boolean right_side, Sprite *item,
												Sprite *by, 
												millisecond duration, Sprite *original_recipient, Sprite *original_item) {
	if (built_in_notebook == LOCAL_REMOTES_NOTEBOOK) { // copied from item_released_on_top and updated on 050201 
		by->abort_action_if_robot(IDS_CANT_ADD_TO_REMOTE_NOTEBOOK);
		return(RELEASE_IGNORED);
   };
   // new on 220900 so the item is added if page is blank
//	long encoding_length = 0;
//	string ignore_encoding;
//	if (pages != NULL) {
//		 pages->encoding_and_length(page_number, 
//										    ignore_encoding, 
//										    encoding_length);
//	};
//	if (encoding_length == 0) { // nothing there
//		int page_number;
//		go_to_page(page_number,FALSE,FALSE,FALSE,TRUE);
	if ((right_side && right_page == NULL) || (!right_side && left_page == NULL)) {
		millisecond duration = set_page(item,!right_side,by);
//		completed_after(duration,action_status,this);
#if TT_BUG_FIXES
		set_saved_by(by); // new on 301103 since otherwise robots wait for ever for this to complete
#endif
      do_after(duration,this,ACTION_COMPLETED_CALLBACK); // rewritten on 131102
		return(right_side?RELEASED_ITEM_RIGHT_OF_ITEM:RELEASED_ITEM_LEFT_OF_ITEM);
	};
	if (receive_item(item,by,duration,original_recipient,original_item)) {
		return(right_side?RELEASED_ITEM_RIGHT_OF_ITEM:RELEASED_ITEM_LEFT_OF_ITEM);
	} else { // prior to 061000 just ignored the release
		if (blank) { 
			log("Warning: only boxes can be dropped on an erased notebook.",FALSE,TRUE); // english.rc ??
#if TT_DEBUG_ON
			debug_this();
#endif
		};
		by->action_aborted(); 
		return(RELEASE_IGNORED);
	};
};

boolean Notebook::receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item) {
	// for now the only thing you can train a robot to do with a notebook 
	// is to find the right page and get something from it
	// or to return something to the notebook
   if (inside_vacuum()) {
      by->abort_action(IDS_ROBOT_MISSING_RECIPIENT_ABORT);
      return(FALSE);
   };
	if (blank) { // new on 220500
		switch(item->kind_of()) {
			case CUBBY:
				call_in_a_mouse(this,item,item->pointer_to_background(),by,FALSE,::ok_to_delete_bammed(by), // this arg was FALSE prior to 131000
									duration,original_recipient,original_item);
				return(TRUE);
			case TOOLBOX:
			case VACUUM:
			case EXPANDER:
			case COPIER:
				return(FALSE); // new on 270204 to avoid spurious warning below
			// prior to 061000 this just ignored the release by returning FALSE
			default: // missing prior to 270804 so following was ignored
				log("Warning: only boxes can be dropped on an erased notebook.",FALSE,TRUE); // english.rc ??
	//			by->abort_action(action_status); // commented out on 131200 since better to ignore drop
				return(FALSE);
		};
	};
	Robot *robot; // new on 301000
	if (by != NULL && by->kind_of() == ROBOT) {
		robot = (Robot *) by;
	} else {
		robot = NULL;
	};
   if (by->showing_on_a_screen()) {
		boolean equal_to_left_page = equal(item,left_page);
		if (equal_to_left_page || equal(item,right_page)) {	
         if (equal_to_left_page) {
				set_recent_page_number(left_page_number(),robot);
         } else {
            set_recent_page_number(right_page_number(),robot);
         };
			if (item->pointer_to_leader() == this) remove_follower(item);
			millisecond duration = by->default_duration(750);
			if (duration > 0) {
				Sprite *target;
				if (equal_to_left_page) {
					target = left_page;
				} else {
					target = right_page;
				};
				item->animate_size_and_location(target->current_width(),
														  target->current_height(),
														  target->current_llx(),
														  target->current_lly(),
														  duration,item,REMOVE_FROM_FLOOR_AND_DESTROY_CALLBACK);
				item->now_on_floor(floor,by); // but not for long
				item->set_slated_for_deletion(TRUE); // so can't be used
			} else {
            item->remove_from_floor(FALSE,FALSE);
				item->destroy();
			};
//			completed_after(duration,action_status,this);
			set_saved_by(by); // added 180103 - robots got stuck without this
         do_after(duration,this,ACTION_COMPLETED_CALLBACK); // rewritten on 131102
			return(TRUE);
// commented out because too hard to know which recent_page number makes sense and its costly
//		} else if (by->kind_of() == ROBOT) {  // what else could it be?
//			// off screen so use recent_page_number
//			short int recent_page_number = ((Robot *) by)->return_recent_page_number();
//			if (recent_page_number > 0) {
//				short int recent_length;
//				bytes recent_encoding;
//				pages->encoding_and_length(recent_page_number,recent_encoding,recent_length);
//				short int item_length;
//				bytes item_encoding = item->return_encoding(item_length,FALSE);
//				boolean same = (item_length == recent_length);
//				if (same) same = (memcmp(item_encoding,recent_encoding,item_length) == 0);
//				if (!same) { // try opposite page of recent page
//					pages->encoding_and_length(opposite_page(recent_page_number),recent_encoding,recent_length);
//					same = (item_length == recent_length);
//					if (same) same = (memcmp(item_encoding,recent_encoding,item_length) == 0);
//				};
//				delete [] item_encoding;
//				if (same) {
//					completed(action_status);
//					remove_follower(item);
//					item->destroy();
//					return(TRUE);
//				};
//			};
		};
	};
	int new_page_number = page_number_of_item(item->dereference()); // dereference() new on 120500
#if TT_DEBUG_ON
	if (tt_debug_mode == 120201 || tt_debug_target == debug_counter) {
		print(tt_error_file());
		tt_error_file() << " received ";
		item->print(tt_error_file());
		long length;
		string text;
		if (item->current_text(text,length)) {
			tt_error_file() << " (" << text << ")";
		};
		tt_error_file() << " and switched to page " << new_page_number << endl;
	};
#endif
	// -1 since page 1 is the default from which we now have a new base
	if (new_page_number >= 0) {
		if (new_page_number&1) { // odd so number is on left side
			if (left_page == NULL || !equal(left_page,item)) {
				page_number_offset = (short int) (new_page_number-1);
			};  // otherwise a no-op
		} else { // even
			if (right_page == NULL || !equal(right_page,item)) {
				page_number_offset = (short int) (new_page_number-1);
			};
		};
//		if (by->kind_of() == ROBOT) set_recent_page_number(saved_page_number);
	} else {
		return(FALSE);
	};
	set_recent_page_number(new_page_number,robot);
	if (duration == 0) {
		by->completed();
		item->remove_from_floor(FALSE);
		item->destroy(by);
	} else {
		page_number_item = item;
      saved_page_number = new_page_number;
//		saved_action_status = action_status;
      set_saved_by(by); // replaces the above on 171102
		city_coordinate delta_x;
		if (saved_page_number&1) { // odd so number is on left side
			delta_x = width/8;
		} else { 
			delta_x = (5*width)/8;
		};
      //if (saved_by != by) { // commented out on 171102
      //   if (saved_by != NULL) saved_by->destroy(); // if zero ref count
	     // saved_by = by;
      //   if (saved_by != NULL) saved_by->increment_ref_count(this);
      //};
		item->animate_size_and_location(3*width/8,height/4,
												  llx+delta_x,lly,
												  duration,
												  this,GO_TO_ITEMS_PAGE_CALLBACK);
	};
	return(TRUE);
};

void Notebook::set_to_future_value(Sprite *other, Sprite *original_recipient, Sprite *by) {
   // this doesn't set by's "action status" so will this work if a robot does it? -- asked on 171102
	if (other->kind_of() != CUBBY) return;
	remove_all_pages();
	Cubby *cubby = (Cubby *) other;
	int size = cubby->current_number_of_holes();
	for (int i = 0; i < size; i++) {
		Sprite *item = cubby->component(i);
		if (item != NULL) {
			add_page(i+1,item->top_level_copy()); // top_level new on 270804 (experiment)
		};
		if (size < 3) go_to_page(3); // so the pages are created and displayed - is there a better way?? (new on 131200)
	};	
	become_non_blank();
	go_to_page(1);
};

MatchStatus Notebook::match(Notebook *, int version) {
	if (old_pages == NULL || old_pages->all_pages_blank() || blank) { // "blank" -- blank itself added 220500
      return(MATCH_GOOD);
   };
   // good enough -- says that all notebooks are unique
	return(MATCH_BAD);
};

//short int Notebook::page_number_of_subnotebook_with_item(Sprite *item) {
//	return(pages->first_page_with_notebook_matching(item));
//};

int Notebook::page_number_of_item(Sprite *item) {
	// odd page with item or -1 if no match
   // seems to be OK that it sometimes returns even pages as well
	switch (item->kind_of()) {	 
		case TEXT:
      case SOUND:
#if TT_XML
         if (pages != NULL) {
            wide_string text;
			   long text_length;
			   if (item->current_wide_text(text,text_length)) {
               wide_string others_text;
               long others_text_length;
               for (int i = 0; i < pages_count; i++) {
                  if (pages[i] != NULL) {
                     // could be more clever than this (turning page into a Sprite)
                     Sprite *sprite = pages[i]->contents();
                     if (sprite != NULL && sprite->current_wide_text(others_text,others_text_length) && 
                         others_text_length >= text_length && 
								 text != NULL && others_text != NULL && // shouldn't be necessary but got a crash so to be safe - new on 241104
                         wcsnicmp(text,others_text,text_length) == 0) {
                        return(i+1);
                     };
                  };
               };
            };
            return(-1);
         };
#endif
         if (old_pages == NULL) return(-1);
			string text;
			long text_length;
			if (!item->current_text(text,text_length)) {
				return(-1);
			} else {
				return(old_pages->first_page_matching_text(text,text_length,version_number));
			};        
		case PICTURE: {
			short int index;
			SpriteCode sprite_code;
			Picture *picture = (Picture *) item;
			if (picture->pointer_to_followers() != NULL ||
				 !picture->no_flipped_followers() ||
				 picture->is_flipped() || // new on 251000
				 !picture->current_image_index(index,sprite_code)) {
				return(-1);
         };
#if TT_XML
         if (pages != NULL) {
            string picture_file_name = picture->return_file_name();
            for (int i = 0; i < pages_count; i++) {
               if (pages[i] != NULL) {
                  Sprite *sprite = pages[i]->contents(); // could be more clever sometime
                  if (sprite->kind_of() == PICTURE) {
                     Picture *other_picture = (Picture *) sprite;
                     short int other_index;
	                  SpriteCode other_sprite_code;
	                  if (other_picture->current_image_index(other_index,other_sprite_code) && sprite_code == other_sprite_code) {
                        if (picture_file_name == NULL) {
                           if (index == other_index) {
                              return(i+1);
                           };
                        } else {
                           string other_picture_file_name = other_picture->return_file_name();
                           if (other_picture_file_name != NULL && strcmp(picture_file_name,other_picture_file_name) == 0) {
                              return(i+1);
                           };
                        };
                     };
                  };
               };
            };
            return(-1);
         }; 
#endif
         if (old_pages == NULL) return(-1);
			if (sprite_code == USER_SPRITE) { // new on 170900
				return(old_pages->first_page_with_picture(index,sprite_code,version_number,picture->return_file_name()));
			} else {
				return(old_pages->first_page_with_picture(index,sprite_code,version_number));
			};           
                    };
		case INTEGER: 
			if (item->true_operation() == NO_NUMBER_OPERATION) { // condition new on 110803 since dropping /5 and the like shouldn't do anything
				// though it could operate on the current page number ;-)
				NumberValue *number_value; // re-written on 300102
				boolean number_value_shared;
				if (!item->current_value(number_value,number_value_shared)) {
					return(-1);
				} else {
					long value;
					if (number_value->long_value(value) && value <= max_long) { // was max_short prior to 280802
						if (!number_value_shared) delete number_value;
						return(value);
					} else {
						if (!number_value_shared) delete number_value;
						return(-1);
					};
				};
         };	
		default:
			return(-1);
	};
};

//void Notebook::add_right_page_as_follower() {
//	right_page->set_infinite_stack(TRUE);
//	add_follower(right_page);
//	floor->remove_item(right_page);
//};

//boolean Notebook::left_side(Sprite *item) {
//	city_coordinate item_center_x, item_center_y;
//	item->center_location(item_center_x,item_center_y);
//	return(item_center_x < llx+width/2);
//};

millisecond Notebook::set_page(Sprite *item, boolean left, Sprite *by) {
	// didn't return duration prior to 220900
	left_page_current = (flag) left;
	if (left) {
		left_page = item;
		user_did(PROGRAM_PAD,NOTEBOOK_LEFT_PAGE_ADDED,by);
	} else {
		right_page = item;
		user_did(PROGRAM_PAD,NOTEBOOK_RIGHT_PAGE_ADDED,by);
	};
	city_coordinate tool_width,tool_height,tool_llx,tool_lly;
	compute_contents_dimensions(left,tool_width,tool_height,tool_llx,tool_lly);
#if TT_XML
   Page *page;
   if (item == NULL) {
      page = NULL;
   } else {
      page = new SpritePage(item);
   };
   set_page_internal(left?left_page_number(by):right_page_number(by),page);
#else
	if (!save_encoding()) { 
		// save it now before changing size etc... -- moved up here from just before animate_size_and_location on 240200
	// since following signals destruction of controlee
		if (left) {
			left_page = NULL;
		} else {
			right_page = NULL;
		};
		return(0); // new on 240200 to not destroy item if error while encoding
	};
	item->set_slated_for_deletion(TRUE);
#endif
	millisecond duration = default_duration(); // moved here on 090100
	if (duration > 0) { // conditional new on 090100
//		if (!floor->has_item(item)) {
			floor->add_item(item,FALSE,FALSE,FALSE); // so it'll be updated
//		};
		tt_screen->add(item);
		item->animate_size_and_location(tool_width,tool_height,
												  tool_llx,tool_lly,
												  duration);
		// changed on 240103 so following happens after animation is finished since it now triggers dumping if item is a notebook
		page_being_defined = item; // new on 171203
		item->do_after(duration+1,this,DEFINE_PAGE_CALLBACK); 
	} else {
#if TT_DEBUG_ON
		if (tt_debug_mode == 120201) {
			print(tt_error_file());
			tt_error_file() << " setting page " << (left_page_number()+(left?0:1)) << " to ";
			item->print(tt_error_file());
			tt_error_file() << endl;
		};
#endif
		define_page(FALSE,TRUE); // new on 090100
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error(1008,"Start MainWindow::WndProc");
#endif
	return(duration);
};

Path *Notebook::path_to(Sprite *target, boolean &ok) { // new on 141200
	if (target != left_page && target != right_page) {
		ok = FALSE;
		return(NULL);
	} else {
		ok = TRUE;
		return(new Path(!left_page_selected)); // made ! on 180204 since 0, 1 are the increments
	};
};

void Notebook::define_page(boolean save, boolean switch_contents_too) {
#if TT_DEBUG_ON
	add_to_log_if_interesting_error(1008,"Start MainWindow::WndProc");
#endif
	Sprite *current_page = left_page_current?left_page:right_page;
	if (current_page == NULL) { // new on 090100
		if (page_being_defined != NULL) {
			set_page(page_being_defined,left_page_current); // new on 171203
			page_being_defined->finish_instantly();
			page_being_defined = NULL; // used above (if at all) - new on 171203
			return;
		} else {
			// warn??
			return;
		};
	};
	page_being_defined = NULL; // copied here on 231203
//	current_page->finish_instantly(); 
	// new on 231203 since no longer save pages that are still animating (e.g. using page_being_defined)
	current_page->remove_from_floor(FALSE);
	if (current_page->kind_of() == PROGRAM_PAD) { // moved here on 240103 since don't want to dump it while it is still animating
//		current_page->finish_instantly(); // just in case - new on 240103
#if TT_DEBUG_ON
	   add_to_log_if_interesting_error();
#endif
		((Notebook *) current_page)->dump();
#if TT_DEBUG_ON
	   add_to_log_if_interesting_error();
#endif
	};
//#if TT_DEBUG_ON
//	if (tt_create_new_notebooks) return; 
	// commented out on 080703 since caused new additions to notebook to be invisible until flip back
//#endif
	if (switch_contents_too) switch_contents(); // conditional new on 060499 to avoid extra work when creating notebooks
// commented out the following on 140300 since don't want to lose file name (like "4")
//	if (file_name != NULL && strncmp(file_name,"bok",3) != 0) {
      // other than top notebook flush name forcing recompute
//		delete [] file_name;
//		file_name = NULL;
//	};
	set_changed_since_last_save(TRUE); // prior to 161202 this was after call to dump
// commented out around 271003
	//if (tt_sprites_with_media_to_save != NULL) { // new on 200903
	//	update_media_file_names(tt_sprites_with_media_to_save);
	//};
	if (save) {
#if TT_DEBUG_ON
	   add_to_log_if_interesting_error();
#endif
		dump(); 
		// second conjunct ( && !tt_create_new_notebooks) was the intent of code above commented out on 080703 
		// and once again on 100803
#if TT_DEBUG_ON
	   add_to_log_if_interesting_error();
#endif
	};
	// this canonicalized current_page by encoding and decoding
	set_clean_status(DIRTY);
};

boolean Notebook::save_encoding() {
   if (tt_want_exception_breaks) {
	   save_encoding_internal();
	   return(TRUE);
   } else {
	   try { // new on 240200 so if error while saving then recover gracefully
		   save_encoding_internal();
		   return(TRUE);
	   } catch (...) {
		   if (!handle_internal_error(S(IDS_INTERNAL_ERROR,"Sorry but an internal error has occurred. Strange things may happen."),
											   TRUE)) {
			   throw(tt_global_picture); // user wants to quit
         };
		   return(FALSE);
	   };
   };
};

void Notebook::save_encoding_internal() {
	convert_to_latest_version_number(); // in case rest of notebook is in an older format
	Sprite *current_page = left_page_current?left_page:right_page;
	long encoding_length;
	// encoding should be "top level"
	UnwindProtect<SpritePointer*> set(tt_sprites_being_dumped,NULL);
	UnwindProtect<int> set2(tt_sprites_being_dumped_size,0);
	UnwindProtect<int> set3(tt_number_of_sprites_dumped_so_far,0);
	UnwindProtect<boolean*> set4(tt_sprites_being_dumped_ok_for_eq,NULL); // new on 150701
	UnwindProtect<DumpReason> set5(tt_dump_reason);
#if TT_DEBUG_ON
   if (tt_debug_mode == 150702) {
      tt_error_file() << "[8] tt_sprites_being_dumped_size=" << tt_sprites_being_dumped_size << "; tt_number_of_sprites_dumped_so_far=" << tt_number_of_sprites_dumped_so_far << endl;
   };
#endif
//	boolean saved_tt_dumping_to_clipboard = tt_dumping_to_clipboard;
	if (current_page->kind_of() == PROGRAM_PAD && tt_toolbox != NULL && this != tt_toolbox_notebook) { // prior to 150704 was pointer_to_toolbox_notebook()) { 
		// new on 100200
		// dump notebook without using file names since is now part of a user notebook
		// tt_dumping_to_clipboard = TRUE; // not quite true - maybe should introduce a new variable for this concept
		tt_dump_reason = DUMPING_TO_CLIPBOARD;
	};
	string encoding_string = current_page->return_encoding(encoding_length,TRUE,FALSE);
//	tt_dumping_to_clipboard = saved_tt_dumping_to_clipboard;
	release_sprites_being_dumped(); // new on 080699
	short int true_page_number = left_page_current?left_page_number():right_page_number();
#if TT_XML
	set_page_internal(true_page_number,new OldFormatPage(encoding_string,encoding_length,version_number,language)); // new on 230103
#else
	old_pages->set_page(true_page_number,encoding_string,encoding_length);
#endif
#if TT_DEBUG_ON
	if (tt_debug_mode == 545) {
		tt_error_file() << endl << "Saving page " << true_page_number
						<< " encoding's length is " << encoding_length << endl;
		current_page->top_level_describe(tt_error_file());
	};
#endif
};

void Notebook::go_to_items_page() {
	go_to_page(saved_page_number,TRUE,TRUE);
	// remove and eventually delete integer
//	floor->remove_item(page_number_item,TRUE);
	// the above doesn't work if run off screen by a robot
	page_number_item->remove_from_floor(TRUE,TRUE);
//	completed_after(total_cycle_duration(),saved_action_status,this); // after page flips
   do_after(total_cycle_duration(),this,ACTION_COMPLETED_CALLBACK); // rewritten on 131102
	page_number_item->destroy(); // (saved_by) // commented out on 171102
	page_number_item = NULL; // new on 231202 for XML dumping of cities
   //if (saved_by != NULL) { // commented out on 171102
   //   saved_by->destroy(); // if ref count is zero
   //   saved_by = NULL; // finished with it
   //};
};

void Notebook::compute_contents_dimensions(boolean left,
														 city_coordinate &contents_width,
														 city_coordinate &contents_height,
														 city_coordinate &contents_llx,
														 city_coordinate &contents_lly) {
//	city_coordinate delta_x, delta_y;
//	city_coordinate page_width, page_height;
//	page_size(page_width,page_height);
	Sprite *current_page = left?left_page:right_page;
   // commented out the following because now uses set_true_size ...
//	current_page->compute_dimensions_to_fit(page_width(),page_height(),
//														 contents_width,contents_height);
   contents_width = page_width();
   contents_height = page_height();
	page_corner(left,contents_llx,contents_lly);
	//if (current_page->kind_of() == ROBOT) { // experiment on 090203
	//	// not too sure why but robots appear to far up and to the right
	//	// maybe room is left for the flame underneath, etc.
	//	contents_llx -= true_width()/20;
	//	contents_lly -= true_height()/10;
	//};
};

void Notebook::page_region(boolean left, TTRegion &region) {
	page_corner(left,region.min_x,region.min_y);
	region.max_x = region.min_x+page_width();
	region.max_y = region.min_y+page_height();
};

void Notebook::page_corner(boolean left, 
									city_coordinate &page_llx, 
									city_coordinate &page_lly) {
	if (left) {
		page_llx = llx+true_width()/40; // was 20 prior to 090203
	} else {
		page_llx = llx+(23*true_width())/40; // was 11/20 prior to 090203
	};
	page_lly = lly+true_height()/5; // was /4 prior to 090203
};

//void Notebook::initialize_file_name(ascii_string name) {
//	file_name = copy_ascii_string(name);
//};

city_coordinate Notebook::page_width() {
	return((4*current_width())/10); // was true_width prior to 090699 but when animation finishing quickly that was too small   
};

city_coordinate Notebook::page_height() {
	return((7*current_height())/10);
};

boolean Notebook::closer_to_left_page(TTRegion *region) {
  return((region->min_x+region->max_x)/2 < llx+width/2);
};

Sprite *Notebook::select(TTRegion *region, SelectionReason reason, Sprite *in_hand) {
	if (reason == MIGHT_DROP_ITEM && in_hand != NULL && tt_log_version_number >= 38) {
		// new on 290204 -- if page is empty or if text, numbers or pictures (to select page)
		// or if returning??
		if (blank) { // new on 111104
			if (in_hand->kind_of() == CUBBY) {
				return(this);
			} else {
				return(NULL);
			};
		};
		if (left_page == NULL || right_page == NULL) {
			return(this);
		};
		SpriteType in_hand_type = in_hand->kind_of();
		if (in_hand_type == INTEGER || in_hand_type == TEXT || in_hand_type == PICTURE) {
			return(this);
		};
		if (equal(left_page,in_hand) || equal(right_page,in_hand)) {
			return(this);
		};
		return(NULL);
	};
   if (reason == MIGHT_DROP_ITEM ||
		 reason == MIGHT_USE_EXPANDER || // reason == MIGHT_USE_NUMBER
		 reason == JUMP_TO_GRASP) {
      return(this);
   };
   if (contained_in(CUBBY) || contained_in(TRUCK)) {
		return(this);
   };
	if (blank) return(this); // new on 220500
	// following is only meaningful if neither item is selected
	left_page_selected = (flag) closer_to_left_page(region);
	boolean left_item_selected,right_item_selected;
	which_side(region,left_item_selected,right_item_selected);
	Sprite *current_page = NULL;
	if (left_item_selected) {
		current_page = left_page;
		if (current_page != NULL && left_page_is_copy) { // && left_page_is_copy had been commented out prior to 270104
			current_page->increment_ref_count(); // new on 150103 since these are now destroyed on page flip
		};
//		recent_page_number = left_page_number();
	};
	if (right_item_selected) {
		current_page = right_page;
		if (current_page != NULL && right_page_is_copy) {
			current_page->increment_ref_count(); // new on 150103 since these are now destroyed on page flip
		};
//		recent_page_number = right_page_number();
	};
//	boolean pad_selected = (!left_item_selected && !right_item_selected);
//	if (reason == MIGHT_DROP_ITEM) {
//		switch (in_hand->kind_of()) {
//			case TEXT:
//			case INTEGER:
//			case PICTURE:
//			case CUBBY:
//			case ROBOT:
//			case BIRD:
//			case NEST:
//			case PROGRAM_PAD:
//			case TRUCK:
//				if (current_page == NULL) {
//					return(this);
//				} else {
//					return(current_page); // was NULL
//				};
//			default:
//				return(NULL);
//		};
//	};
	if (current_page == NULL) {
//		 reason == MIGHT_USE_COPIER || // copier could copy just a page but why?
//		 (reason == MIGHT_GRASP && left_page_selected)) {
		return(this);
	} else {
		return(current_page);
	};
};

Sprite *Notebook::selected(SelectionReason reason, Sprite *by) {
//	if (leader != NULL && leader->kind_of() == PICTURE && reason != EXPANDER_USED) {
		// notebook of a picture
		// do I need to record this??
		// don't want this to be an infinite stack except just now
//		set_infinite_stack(TRUE);
//		Sprite *a_copy = Sprite::selected(reason,by);
//		if (a_copy->kind_of() == PROGRAM_PAD) {
//			((Notebook *) a_copy)->go_to_page(1);
//		};
//		set_infinite_stack(FALSE);
//		picture_notebook_copied(by);
//		tt_last_notebook = (Notebook *) a_copy;
//		return(a_copy); //->selected(reason,by));
//	};
	if (animation_in_progress()) { // was in ::select
		finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
	};
//	if (left_page_selected) {
//		tt_last_notebook = this;
		return(Sprite::selected(reason,by)); // whole thing
//	};
}; 

void Notebook::set_inside_vacuum(boolean new_flag, boolean recur) {
	// new on 270204 - so that shared pages aren't affected by this
	Sprite::set_inside_vacuum(new_flag,FALSE);
};

void Notebook::picture_deleted(Picture *picture) {
#if TT_DEBUG_ON
	if (deleted()) {
		tt_error_file() << "Notebook learning that its picture has been deleted but notebook is already deleted." << endl;
	};
#endif
	// this is remote control notebook for a picture that is being deleted
//	if (floor != NULL) {
//		floor->remove_item(this,FALSE); // might be on flip side
//	};
	// rewrote the above as the following on 290300 since the floor might have been destroyed by now
	Background *picture_background = picture->pointer_to_background();
	if (picture_background != NULL && picture_background->pointer_to_picture() == picture) {
		// second conjunct added on 311000 since might be on the floor or elsewhere
		picture_background->remove_item(this,FALSE); // might be on flip side
	};
//	};
	destroy(); // commented out on 301000 since broke second use of portal in adventure game - OK or leak?? -- restored on 121200 
//	set_slated_for_deletion(TRUE);
//	tt_deleted_sprites = new Sprites(this,tt_deleted_sprites);
};


/*
	if (reason == VACUUM_USED) {
		Sprite *current_page = left_page_selected?left_page:right_page;
		if (current_page == NULL) {
			return(Sprite::selected(reason,by));
		} else {
			remove_selected_element(NULL,reason);
			user_did(PROGRAM_PAD,NOTEBOOK_PAGE_REMOVED);
			return(current_page);
		};
	};

	if (right_page == NULL ||
		 reason == COPIER_USED || reason == EXPANDER_USED ||
		 reason == JUMP_TO_GRASP) {
		// really want the pad
		return(Sprite::selected(reason,by));
	} else {
		Background *floor_of_by;
		if (by != NULL && by->kind_of() == ROBOT) {
			floor_of_by = ((Robot *) by)->pointer_to_background();
		} else {
			floor_of_by = floor;
		};
		Picture *old_tt_copying_insides_for = tt_copying_insides_for;
		tt_copying_insides_for = picture;
		Sprite *page_copy = right_page->copy(floor_of_by);
		if (right_page->kind_of() == PROGRAM_PAD) {
			// e.g. sub notebooks of a picture notebook 
			tt_last_notebook = (Notebook *) page_copy;
			if (picture != NULL) {
				tt_last_notebook
				  ->set_picture(picture->which_sub_picture_on_page(right_page_number()));
			};
		};
//		city_coordinate page_width,page_height;
//		page_size(page_width,page_height);
//		page_copy->set_size(page_width,page_height);
		tt_copying_insides_for = old_tt_copying_insides_for;
		if (floor_of_by != NULL) floor_of_by->add_item(page_copy);
		record_event(GRASP_ITEM,by,this,floor_of_by,0,FALSE,TRUE);
		// the pad is grasped but a copy is obtained so switch availability
		set_available_for_indexing(TRUE);
		page_copy->set_available_for_indexing(FALSE); // until RELEASEd
		record_event(NEW_ITEM,by,page_copy,floor_of_by);
		return(page_copy); //->selected(reason,by));
	};
*/

void Notebook::remove_selected_element(Sprite *item, SelectionReason , Sprite *by, boolean add_to_floor) {
	user_did(PROGRAM_PAD,NOTEBOOK_PAGE_REMOVED,by);
	if (by != NULL && by->kind_of() == ROBOT) { // new on 141200 - when robot uses vacuum on notebook
		left_page_selected = (left_page == item);
#if TT_DEBUG_ON
		if (tt_debug_mode == 120201 || tt_debug_target == debug_counter) {
			print(tt_error_file());
			tt_error_file() << " removing ";
			item->print(tt_error_file());
			long length;
			string text;
			if (item->current_text(text,length)) {
				tt_error_file() << " (" << text << ")";
			};
			tt_error_file() << " from page " << (left_page_selected?left_page_number():right_page_number()) << endl;
		};
#endif
	};
	Sprite *current_page = left_page_selected?left_page:right_page;
	int true_page_number = left_page_selected?left_page_number():right_page_number(); // was short int prior to 200802
	if (left_page_selected) {
		left_page = NULL;
	} else {
		right_page = NULL;
	};
	if (current_page != NULL) { // condtional new on 130103
		remove_follower(current_page);
		if (add_to_floor) {
			floor->add_item(current_page,FALSE);
		};
		current_page->set_infinite_stack(FALSE);
	} else {
		return; // new on 130103 - warn as well??
	};
//	current_page->un_absorb_followers();
//	Tool *saved_current_page = current_page;
/*
	if (saved_page_number == page_number) {
		// removing a page that replaced a "saved page"
		current_page = saved_page;		
		add_follower(current_page,floor);
		saved_page_number = -1;
		saved_page = NULL;
		pages->set_page(page_number,saved_encoding,saved_encoding_length);
	} else {
*/
#if TT_XML
      set_page_internal(true_page_number,NULL);
#else
		pages->set_page(true_page_number,NULL,0);
#endif
//	};
	current_page->set_in_notebook(NULL);
	if (main_notebook()) { // conditional new on 010301 since vacuuming off a page shouldn't be permanent unless saved somewhere or is main notebook
		dump();
	};
	if (file_name != NULL && strncmp(file_name,"bok",3) != 0) {
		delete [] file_name;
		file_name = NULL;
	};
//	flush_cache();
};	

void Notebook::remove_page(short int page_number) {
	short int current_page_number = left_page_number();
	go_to_page(page_number);
	Sprite *current_page = page_number&1?left_page:right_page;
	remove_follower(current_page);
	current_page->set_infinite_stack(FALSE);
	if (page_number&1) {
		left_page = NULL;
	} else {
		right_page = NULL;
	};
	old_pages->set_page(page_number,NULL,0);
	current_page->set_in_notebook(NULL);
	go_to_page(current_page_number);
	dump();
//	flush_cache();
};	

void Notebook::remove_all_pages() { // new on 220500
	if (left_page != NULL) {
		remove_follower(left_page);
//#if TT_XML
//      if (left_page_is_copy)
//#endif
		left_page->destroy();
	};
	if (right_page != NULL) {
		remove_follower(right_page);
//#if TT_XML
//      if (right_page_is_copy)
//#endif
		right_page->destroy();
	};
	if (old_pages != NULL) { // delete private copies
      Pages *remaining = old_pages;
      while (remaining != NULL) {
         Pages *next_remaining = remaining->rest();
         remaining->set_rest(NULL);
         delete remaining;
         remaining = next_remaining;
      };
//		delete pages; // for large notebooks blew the stack
	};
	version_number = latest_version_number;
	old_pages = new Pages(NULL,0); // initial 1 blank page
//	flush_cache();
#if TT_XML
   if (pages != NULL) {
      delete [] pages;
      pages = NULL;
      pages_count = 0;
   };
#endif
};

string page_full_file_name(ascii_string file_name, ascii_string user_file_name, boolean check_paths, ascii_string extension) {
   // abstracted to a procedure on 220802 - and added extension for use by XML
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
   char short_name[MAX_PATH];
 //  output_string_stream file_name_stream(short_name,MAX_PATH); // rewritten 230104
 	if (user_file_name != NULL) {
 //     file_name_stream << user_file_name << "."; // user name truncated
		strcpy(short_name,user_file_name);
		strcat(short_name,".");
	} else {
		short_name[0] = '\0';
	};
	//file_name_stream << file_name;
	strcat(short_name,file_name);
	//file_name_stream.put('\0');
   FileNameStatus file_status;
	// added args so won't search path list if not found
//   string saved_tt_user_directory = NULL;
	UnwindProtect<string> save(tt_user_directory);
	if (tt_default_user_directory != NULL) {
		// rewritten on 120203 to deal with .bok and the like caused by XML
		string very_short_name = short_file_name(short_name); // used to just use short_name below
		if (strcmp(very_short_name,tt_default_file_name) == 0) { // need to use directory of default user
			tt_user_directory = copy_string(tt_default_user_directory); // added copy_string on 111205 since is usually consed
		};
		delete [] very_short_name;
	};
	if (tt_user_directory != tt_default_user_directory && (tt_time_travel != TIME_TRAVEL_OFF || replaying())) { 
		// new on 171203 - aren't working on the default directory but are replaying a log or time travel
		tt_user_directory = copy_string(tt_extracted_file_temp_directory); // added copy_string on 111205 since is usually consed
	};		 
   ascii_string full_file_name = existing_file_name(file_status,short_name,extension,"NewUsers",
																	 TRUE,FALSE,FALSE,FALSE,check_paths); // last arg was FALSE prior to 110100
   //if (saved_tt_user_directory != NULL) { // restore things
   //   tt_user_directory = saved_tt_user_directory;
   //};
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
   return(full_file_name);
};

FileInputStream *open_page_file(ascii_string file_name, ascii_string user_file_name, boolean check_paths) {
	// rewritten on 070703 to return new FileInputStream
	if (tt_logs_contain_virgin_notebook) return(FALSE); // new on 060300 since virgin means - don't read files
#if TT_LEGO
// conditionalized on 220802
	if (strcmp("4",file_name) == 0 && tt_lego_in_use != NO_LEGO_CONTROLLER) {
		return(NULL); // don't use cached page since modified to include Lego sensors etc
	};
#endif
   ascii_string full_file_name = page_full_file_name(file_name,user_file_name,check_paths,NULL);
	if (full_file_name != NULL) {
		FileInputStream *pad_in = new FileInputStream();
      pad_in->open(full_file_name,std::ios_base::in|std::ios_base::binary); //std::ios_base::nocreate);
      delete [] full_file_name;
      return(pad_in);
   };
   return(NULL);
};

#if TT_DIRECT_INPUT
Notebook *joystick_notebook(int joystick_number) {
   const int count = 6;
   RemoteIdentifier identifiers[count]
					= {JOYSTICK_SPEED_X_REMOTE,JOYSTICK_SPEED_Y_REMOTE,JOYSTICK_SPEED_Z_REMOTE,
                  JOYSTICK_AXIS_ROTATION_X_REMOTE,JOYSTICK_AXIS_ROTATION_Y_REMOTE,JOYSTICK_AXIS_ROTATION_Z_REMOTE};
   Notebook *notebook = new Notebook();
   short int page = notebook->add_remotes(identifiers,0,count,tt_joystick_pictures[joystick_number]);
	Cubby *cubby = new Cubby();
   cubby->set_number_of_holes(joystick_button_count);
	Text *text = variable_width_text_pad(S(IDS_JOYSTICK_BUTTONS_LONG_LABEL),text_pad_see_all);
   notebook->add_page(page++,text);
	Remote_Integer *remote_integer;
	int i;
   for (i = 0; i < joystick_button_count; i++) {
       remote_integer = new Remote_Picture(tt_joystick_pictures[joystick_number],(RemoteIdentifier) (JOYSTICK_BUTTON1_REMOTE+i));
       cubby->initialize_hole(i,remote_integer);
       ascii_string label = new char[3];
       itoa(i+1,label,10);
       cubby->set_label(i,a2u(label));
   };
   notebook->add_page(page++,cubby);
   cubby = new Cubby();
   cubby->set_number_of_holes(joystick_button_count);
	text = variable_width_text_pad(S(IDS_JOYSTICK_BUTTONS_DOWN_LONG_LABEL),text_pad_see_all);
   notebook->add_page(page++,text);
   for (i = 0; i < joystick_button_count; i++) {
       remote_integer = new Remote_Picture(tt_joystick_pictures[joystick_number],(RemoteIdentifier) (JOYSTICK_BUTTON1_DOWN_REMOTE+i));
       cubby->initialize_hole(i,remote_integer);
       ascii_string label = new char[3];
       itoa(i+1,label,10);
       cubby->set_label(i,a2u(label));
   };
   notebook->add_page(page++,cubby);
   notebook->go_to_page(1);
   return(notebook);
};

Notebook *force_feedback_joystick_notebook(int joystick_number) {
   int force_count = return_force_count();
   ascii_string *force_names = return_force_names();
   Notebook *notebook = new Notebook();
   short int page = 1;
   boolean ignore_old;
   tt_will_end_up_in_a_notebook = TRUE;
   for (int i = 0; i < force_count; i++) {
      ForceRemoteControlee *parameter_controlee = force_parameter_controlee(i,joystick_number,ignore_old);
	   ForceFeedback *force = new ForceFeedback(joystick_number,parameter_controlee);
		force->set_text(force_names[i]); 
      notebook->add_page(page++,force);
      Cubby *cubby = new Cubby();
      const int force_parameter_count = 3;
      // trigger button seems like a bad idea and was causing errors
      // left in but skipped over for now -- maybe will re-use its "slot"
      cubby->set_number_of_holes(force_parameter_count);
      for (int i = 0; i < force_parameter_count; i++) {
          Remote_Integer *remote_integer = new Force_Remote_Integer(parameter_controlee,(RemoteIdentifier) (FORCE_FEEDBACK_DURATION_REMOTE+i));
          cubby->initialize_hole(i,remote_integer);
          cubby->set_label(i,SC(IDS_FORCE_FEEDBACK_REMOTE_SHORT_LABEL+i));
      };
      notebook->add_page(page++,cubby);
   };
   notebook->go_to_page(1);
   tt_will_end_up_in_a_notebook = FALSE;
   return(notebook);
};

#endif

void Notebook::load() { // ascii_string ignore_file_name, boolean add_prefix) { // rationalized on 210802
	// ignoring file_name on 090200 since notebook should already know 
   if (file_name == NULL) return; 
	boolean worth_checking_paths_for_default_user = (built_in_notebook != LOCAL_REMOTES_NOTEBOOK); // moved here on 190803 rather than do it twice
#if TT_XML
// if (using_xml()) { // commented out on 270902 so can switch on and off XML and still load old stuff
   if (loaded) return;
   string full_file_name;
	string name;
	if (tt_time_travel == TIME_TRAVEL_OFF) {
		//if (tt_file_name_read_only != NULL) { // new on 030304
		//	name = tt_file_name_read_only;
		//} else {
			name = tt_file_name;
//		};
	} else {
		if (tt_file_name_while_time_travelling != NULL) {
			name = tt_file_name_while_time_travelling;
		} else {
			name = tt_file_name; // new on 280204 since notebook versions were broken (unless running a DMO time travel archive)
		};
	};
	// following moved here so used for all branches above on 030304
	full_file_name = page_full_file_name(file_name,name,FALSE,"xml"); // new on 220402 so the right file is loaded from a notebook
   if (full_file_name == NULL) {
      full_file_name = page_full_file_name(file_name,tt_default_file_name,worth_checking_paths_for_default_user,"xml");
   };
   if (full_file_name != NULL) {	
		loaded = TRUE; // moved here on 161202 to prevent infinite recursions due to creating tt_toolbox and BOK notebook
		UnwindProtect<boolean> set(tt_loading_a_notebook,TRUE); // new on 120504 -- fixes problems with long-distance birds in a notebook
//		UnwindProtect<boolean> set2(tt_loading_is_not_to_be_aborted,TRUE); // new on 160105
		boolean aborted = FALSE; // new on 140105
		xml_load_sprite_from_file(full_file_name,aborted,this);
		// added arg on 161202 so this clobbers this existing one rather than create new and transfer
		//if (aborted) { // commented out on 150105 since best to just leave one page empty, right?
		//	throw(0); // caught too low
		//};
		set_infinite_stack(FALSE);
		// new on 241003 since if needs to be infinite will be set but otherwise copies of built-in notebooks will become stacks on the floor
//    Sprite *replacement = xml_load_sprite_from_file(full_file_name);
      delete [] full_file_name;
      //if (replacement != NULL) {
      //   if (replacement->kind_of() == PROGRAM_PAD) {
      //      Notebook *replacement_notebook = (Notebook *) replacement;
      //      pages = replacement_notebook->transfer_pages(pages_count);
      //   };
      //   replacement->destroy();
      //}; // else warn??
      return;
   };
   //int file_name_length = strlen(file_name);
   //if (file_name_length > 4) {
   //   int dot_location = file_name_length-4; // if there is one
   //   if (strnicmp(file_name+dot_location-i,".xml",4) == 0) { // has XML extension
#endif
   int original_page_number = page_number;
//	input_file_stream pad_in;
//#if TT_DEBUG_ON // commented out on 050203 since why only if debugging?
	string file_name_copy;
	if (tt_create_new_notebooks) { 
		file_name_copy = copy_ascii_string(file_name);
	};
//#endif
	// above new on 090200 since define_page nulls file_name and this interferes with dumping when tt_debug_mode == 2468 (tt_create_new_notebooks)
	// following was pulled out of a conditional branch on 160301 below since it should apply in all cases
   // commented out on 210802 since now taken care of by :set_file_name
	//if (strcmp(file_name,"bok") == 0) {
 //     built_in_notebook = MAIN_NOTEBOOK;
	//} else if (strcmp(file_name,"rem") == 0) {
 //     built_in_notebook = LOCAL_REMOTES_NOTEBOOK;
	//};
	FileInputStream *pad_in = open_page_file(file_name,name,FALSE); // name was tt_file_name prior to 030304
	if (tt_default_file_name != NULL && (pad_in == NULL || pad_in->empty())) {
		pad_in = open_page_file(file_name,tt_default_file_name,worth_checking_paths_for_default_user);
	};
	if (pad_in != NULL && !pad_in->empty()) {
		load_notebook(pad_in);
		pad_in->close();
	} else {
		if (strcmp(file_name,"bok") == 0) {
//#if TT_DEBUG_ON 
			if (tt_create_new_notebooks) {
				short int page = 1;
				Notebook *notebook;
				// no pad file so fill with defaults -- made them "See some" on 120200
				add_page(page++,variable_width_text_pad(S(IDS_MOVIES_NOTEBOOK),text_pad_see_all));
				notebook = new Notebook(0,0,0,copy_ascii_string("2")); // beginning 210802 the caller copies the file name
				notebook->set_label_string(S(IDS_MOVIES_NOTEBOOK)); // seems to have already been set - noticed on 260803
				add_page(page++,notebook);
				add_page(page++,variable_width_text_pad(S(IDS_SENSORS_NOTEBOOK),text_pad_see_all));
				notebook = new Notebook(0,0,0,copy_ascii_string("4")); // beginning 210802 the caller copies the file name
				notebook->set_label_string(S(IDS_SENSORS_NOTEBOOK));
				add_page(page++,notebook);
				add_page(page++,variable_width_text_pad(S(IDS_EXAMPLES_NOTEBOOK),text_pad_see_all));
				notebook = new Notebook(0,0,0,copy_ascii_string("6")); // beginning 210802 the caller copies the file name
				notebook->set_label_string(S(IDS_EXAMPLES_NOTEBOOK));
				add_page(page++,notebook);
				add_page(page++,variable_width_text_pad(S(IDS_SOUNDS_NOTEBOOK),text_pad_see_all));
				notebook = new Notebook(0,0,0,copy_ascii_string("8")); // beginning 210802 the caller copies the file name
				notebook->set_label_string(S(IDS_SOUNDS_NOTEBOOK));
				add_page(page++,notebook);
				add_page(page++,variable_width_text_pad(S(IDS_OPTIONS_NOTEBOOK),text_pad_see_all));
				notebook = new Notebook(0,0,0,copy_ascii_string("10")); // beginning 210802 the caller copies the file name
				notebook->set_label_string(S(IDS_OPTIONS_NOTEBOOK));
				add_page(page++,notebook);
				add_page(page++,variable_width_text_pad(S(IDS_NUMBERS_AND_FUNCTIONS,"Numbers and\rFunctions"),text_pad_see_all));
				notebook = new Notebook(0,0,0,copy_ascii_string("12")); // beginning 210802 the caller copies the file name
				notebook->set_label_string("Numbers");
				add_page(page++,notebook);
				add_page(page++,variable_width_text_pad(S(IDS_BLANK_NOTEBOOK),text_pad_see_all));
				add_page(page++,new Notebook());
				go_to_page(page++); // final blank page
				if (file_name != NULL) delete [] file_name; // new on 270200
				file_name = file_name_copy; // new on 090200
//				dump(); // so next time it'll load faster -- commented out on 050803 since not faster and just creates files in user's directory
			} else { 
				load_page_resource("NOTEBOOK_MAIN");
			};
//#else
//			load_page_resource("NOTEBOOK_MAIN");
//#endif
		} else if (strcmp(file_name,"rem") == 0) {
         built_in_notebook = LOCAL_REMOTES_NOTEBOOK;
			delete [] file_name_copy; // new on 260104 - why do this at all?
//			if (!tt_create_new_notebooks) load_page_resource("NOTEBOOK_LOCAL_REMOTES"); // commented out on 090603 since fixes bug with speeds not initialized properly
		} else if (strcmp(file_name,"2") == 0) {
         built_in_notebook = PICTURES_NOTEBOOK;
//#if TT_DEBUG_ON 
			if (tt_create_new_notebooks) {
				short int page = 1;
				const int good_picture_count = 56;
				const SpriteCode good_pictures[good_picture_count] =
				  {SYNTHETIC_SPRITE,
					BALL_SPRITE,PADDLE_SPRITE,
					PERSON_WALKING_SPRITE, //MANWALK_NORMAL_SPRITE,
					DIZZY_PERSON_SPRITE,
					DIZZY_PERSON_WITH_HAIR_SPRITE,
					DIZZY_PERSON_WITH_HAT_SPRITE,
					SIT_SPRITE,SIT_WITH_HAT_SPRITE,SIT_WITH_HAIR_SPRITE,
					TOOLBOX_SIDE_SPRITE,
					HELIOFLY,HELIOLND,
					EXPLOSION_SPRITE,BOMB_SPRITE, 
					HOUSE_A_SIDE_SPRITE,
					HOUSE_A_TOP_SPRITE,
					HOUSE_B_SIDE_SPRITE,
					HOUSE_B_TOP_SPRITE,
					HOUSE_C_SIDE_SPRITE,
					HOUSE_C_TOP_SPRITE,
					ROBOT_SPRITE,
					NEST_SPRITE,BIRD_SPRITE,
					MOUSE_SPRITE,
					VACUUM_SPRITE, EXPANDER_SPRITE, COPIER_SPRITE,
					MARTIAN_SPRITE, BROKEN_ROCKET_SPRITE, // ALIEN_TALK_SPRITE,
					TRUCK_INSIDE_SPRITE,
					TRUCK_ABOVE_SPRITE,
					TRUCK_SIDE_SPRITE,
					THOUGHT_BUBBLE_SPRITE,TALK_BALLOON_SPRITE,
					BIKEPUMP_BUTTON_SPRITE, //CHARACTER_PLATE_SPRITE,
					NUMBER_PAD_SPRITE, // TEXT_PAD_SPRITE,
					NOTEPAD_SPRITE,
					FRAME_SPRITE,
					ARM_ONLY_SPRITE,HAND_SPRITE,HAND_HOLDING_WAND_SPRITE,
					SWITCH_SPRITE,WIRE_SPRITE,
					FLOWER_SPRITE,
					TREE_SPRITE,
					TRAFFIC_LIGHT_SPRITE,
               // and now new synthetic shapes
               SYNTHETIC_SPRITE, SYNTHETIC_SPRITE, SYNTHETIC_SPRITE, SYNTHETIC_SPRITE, SYNTHETIC_SPRITE,
               SYNTHETIC_SPRITE, SYNTHETIC_SPRITE, SYNTHETIC_SPRITE, SYNTHETIC_SPRITE};
            int synthetic_count = 0;
				for (int i = 0; i < good_picture_count; i++) {
					SpriteCode code = good_pictures[i];
					add_page(page++,variable_width_text_pad(S(IDS_RECTANGLE_PAGE+i),text_pad_see_all));
               Picture *picture = new Picture(0,0,0,0,code);
					picture->set_show_all(FALSE); // background transparent by default
               if (code == SYNTHETIC_SPRITE) {
                  picture->set_parameter((short int) (synthetic_count*256));
                  synthetic_count++;
               };
					add_page(page++,picture);
				};
				go_to_page(page++); // final blank page
				if (file_name != NULL) delete [] file_name; // new on 130104 to fix a minor leak
				file_name = file_name_copy; // new on 090200
//				dump(); // so next time it'll load faster -- commented out on 050803 since not faster and just creates files in user's directory
			} else {
//#endif
//				if (tt_log_version_number <= 7) {
//               load_page_resource("NOTEBOOK_PICTURES_OLD");
//            } else {
               load_page_resource("NOTEBOOK_PICTURES");
         };
		} else if (strcmp(file_name,"4") == 0) {
			if (file_name_copy != NULL) delete [] file_name_copy; // new on 130104 to fix a tiny leak 
         built_in_notebook = GLOBAL_REMOTES_NOTEBOOK;
			// following is new to see if there is a cached notebook to load in -- note this fixes a bug in making dmo files for brand new users
			// since creating the clipboard_remote messes up the log
			char cached_file_name[MAX_PATH];
			strcpy(cached_file_name,tt_main_directory);
			//switch (tt_language) {
			//	case SWEDISH:
			//		strcat(cached_file_name,"se.4.xml");
			//		break;
			//	case BRITISH:
			//		strcat(cached_file_name,"uk.4.xml");
			//		break;
			//	case PORTUGAL_PORTUGUESE:
			//		strcat(cached_file_name,"pt.4.xml");
			//		break;
			//	default:
			//		strcat(cached_file_name,"us.4.xml");
			//		break;
			//};
			// rewrote the above to be more general
			strcat(cached_file_name,country_codes[tt_language]);
			strcat(cached_file_name,".4.xml");
			if (file_exists(cached_file_name)) { // new on 050603
				boolean aborted = FALSE; // new on 140105
//				UnwindProtect<boolean> set(tt_loading_is_not_to_be_aborted,TRUE); // new on 150105
				UnwindProtect<boolean> set(tt_loading_is_ok_to_abort,FALSE); // new on 130405
				xml_load_sprite_from_file(cached_file_name,aborted,this);		
			} else if (tt_create_new_notebooks) { // || tt_lego_in_use != NO_LEGO_CONTROLLER) { 
				const int count = 25;
				RemoteIdentifier identifiers[count]
					= {MOUSE_SPEED_X_REMOTE,MOUSE_SPEED_Y_REMOTE,
						MOUSE_LEFT_BUTTON_REMOTE,
						MOUSE_MIDDLE_BUTTON_REMOTE,
						MOUSE_RIGHT_BUTTON_REMOTE,
						MOUSE_LEFT_BUTTON_DOWN_REMOTE,
						MOUSE_MIDDLE_BUTTON_DOWN_REMOTE,
						MOUSE_RIGHT_BUTTON_DOWN_REMOTE,
						KEYBOARD_BUTTON_REMOTE,
						LAST_KEYSTROKE_REMOTE,
						SHIFT_DOWN_REMOTE,
						CONTROL_DOWN_REMOTE,
						CLOCK_REMOTE,RANDOM_REMOTE,
						CLIPBOARD_REMOTE,
						HAND_VISIBLE_REMOTE,
                  FILE_TO_PICTURE_REMOTE,
                  FILE_TO_SOUND_REMOTE,
                  TEXT_TO_MCI_REMOTE,
                  TEXT_TO_SPEECH_REMOTE, // 20th in the list
                  ROOM_DECORATION_REMOTE,
                  HOUSE_DECORATION_REMOTE,
                  ROOF_DECORATION_REMOTE,
                  STREET_REMOTE, // really both  -- primes.tt expects it to be at the end but that is a bad policy - page numbers should be stable
						LANGUAGE_REMOTE};
#if TT_SPEAK_ALL_NUMBERS 
				// TT_SPEAK_ALL_NUMBERS inspired by this feature to simplify access to a text-to-speech pad
				// new on 121104 to replace the text pad with a text-to-speech sound for the text-to-speech remote
				short int page = add_remotes(identifiers,0,19,tt_global_picture);
				Sprite *text_to_speech = new SpeechSound(0,0,copy_wide_string(S(IDS_X_LONG_LABEL+TEXT_TO_SPEECH_REMOTE)));
				add_page(page++,text_to_speech);
				Cubby *cubby = new Cubby();
				cubby->initialize_hole(0,new Remote_Picture(tt_global_picture,TEXT_TO_SPEECH_REMOTE));
			   cubby->set_label(0,copy_string(S(IDS_X_SHORT_LABEL+TEXT_TO_SPEECH_REMOTE)));
		      add_page(page++,cubby);
				page = add_remotes(identifiers,page-1,count,tt_global_picture);
#else
				short int page = add_remotes(identifiers,0,count,tt_global_picture);
#endif
				add_page(page++,variable_width_text_pad(S(IDS_PRIMITIVE_BIRD_LABEL),text_pad_see_all));
		      Bird *bird = new PrimitiveBird(receive_extension_name);
            // following is done by PrimitiveBird::PrimitiveBird as of 140802
//				Picture *t_shirt = t_shirt_label(_T("?")); // is this really a good label?
//		      bird->update_display(); 
//          bird->add_t_shirt(t_shirt);
            add_page(page++,bird);				
			  go_to_page(page++); // final blank page
			  set_picture(tt_global_picture);
			  dump(); // so next time it'll load faster -- had been commented out - restored on 090203 
			} else { 
           load_page_resource("NOTEBOOK_GLOBAL_REMOTES");
           int page = last_non_blank_page_number()+1;     //pages->length()
#if TT_DIRECT_INPUT
		     int i;
           for (i = 0; i < tt_joystick_count; i++) {
				  string label;
              if (tt_joystick_count == 1) { // if only 1 then no need for numbers on the label
					  label = copy_string(S(IDS_JOYSTICK_LABEL));
                 add_page(page++,variable_width_text_pad(label,text_pad_see_all));
              } else {
					  label = copy_string(S2(IDS_JOYSTICK_LABEL,i+1));
                 add_page(page++,variable_width_text_pad(label,text_pad_see_all));
              };
				  Notebook *notebook = joystick_notebook(i); 
				  notebook->set_label_string(label); // new on 160801
				  delete [] label;
              add_page(page++,notebook);
           };
			  if (tt_force_sensitive_mouse_count > 0) { // only expect 1 for now
				  for (i = 0; i < tt_force_sensitive_mouse_count; i++) {
					  string label = copy_string(S(IDS_TOUCH_SENSTIVE_MOUSE_LABEL,S(IDS_FORCE_FEEDBACK_JOYSTICK_LABEL)));
					  add_page(page++,variable_width_text_pad(label,text_pad_see_all)); // if translation is missing this then use joystick label
					  Notebook *notebook = force_feedback_joystick_notebook(i);
					  notebook->set_label_string(label); // new on 160801
					  delete [] label;
					  add_page(page++,notebook);
				  };
			  };
           for (i = 0; i < tt_force_feedback_joystick_count; i++) {
				  string label;
              if (tt_force_feedback_joystick_count+tt_force_sensitive_mouse_count == 1) {
					  label = SC(IDS_FORCE_FEEDBACK_JOYSTICK_LABEL);
                 add_page(page++,variable_width_text_pad(label,text_pad_see_all));
              } else {
					  label = copy_string(S2(IDS_FORCE_FEEDBACK_JOYSTICK_LABEL,i+1));
                 add_page(page++,variable_width_text_pad(label,text_pad_see_all)); // updated 150801
              };
				  Notebook *notebook = force_feedback_joystick_notebook(i+tt_force_sensitive_mouse_count);
				  notebook->set_label_string(label); // new on 160801
				  delete [] label;
				  add_page(page++,notebook);
           };
#endif
           Picture *age_in_seconds = players_age_in_seconds();
           if (age_in_seconds != NULL) { // have "earned" it
              string label = append_strings(S(IDS_AGE_OF_LABEL),tt_user_name);
//				  Text *text = new Text(0,0,label,-1,0,0,0,TRUE);  // re-uses string
				  Text *text = new Text();
				  text->set_text(label);
#if TT_WIDE_TEXT
				  delete [] label;
#endif
				  text->set_use_variable_width_font(TRUE);
              add_page(page++,text);
              add_page(page,age_in_seconds);
           };
			  if (!replaying()) { // condition added 080403 - I think it is right
				  dump(); // moved here on 050203 since the above should happen regardless -- really??
			  };
			};
		} else if (strcmp(file_name,"6") == 0) {
         built_in_notebook = EXAMPLES_NOTEBOOK;
			string full_file_name;
			char localized_examples_file_name[MAX_PATH];
			strcpy(localized_examples_file_name,tt_main_directory);
			strcat(localized_examples_file_name,country_codes[tt_language]);
			strcat(localized_examples_file_name,".6.xml");
			if (file_exists(localized_examples_file_name)) {
				// new as of 230305 to check first for localized versions of the Examples notebook
				full_file_name = copy_string(localized_examples_file_name);
			} else {
				FileNameStatus ignore_name_status;
				full_file_name = existing_file_name(ignore_name_status,"Examples","xml.tt",NULL,FALSE,FALSE,FALSE,FALSE,FALSE);
			};
			if (full_file_name != NULL) {
				boolean aborted = FALSE; // new on 140105
//				UnwindProtect<boolean> set(tt_loading_is_not_to_be_aborted,TRUE); // new on 150105
				UnwindProtect<boolean> set(tt_loading_is_ok_to_abort,FALSE); // new on 130405
				xml_load_sprite_from_file(full_file_name,aborted,this);
				delete [] full_file_name;
			} else {
				load_page_resource("NOTEBOOK_EXAMPLES");
			};
			if (file_name != NULL) delete [] file_name; // new on 130104
			file_name = file_name_copy; // new on 090200 -- uncommented on 130104
//			dump(); // new on 090200 in case needs to be converted to newer format version
		} else if (strcmp(file_name,"8") == 0) {
         built_in_notebook = SOUNDS_NOTEBOOK;
//#if TT_DEBUG_ON 
			if (tt_create_new_notebooks) {
				Text *text;
				short int page = 1;
//				char sound_label[128];
				for (int i = 0; i < LAST_SOUND; i++) {
               //	not one of the one's skipped
               if (!skipped_sound_index(i)) {
						const_string sound_name = S(IDS_FIRST_SOUND+i);
//						output_string_stream sound_label_stream(sound_label,128);
//						sound_label_stream << sound_name << "\r" << S(IDS_SOUND);
//						sound_label_stream.put((char) 0);
//						text = new Text(0,0,sound_label);
//						add_page(page++,text);
						BuiltInSound *sound = new BuiltInSound();
						sound->set_text(sound_name);
                  sound->set_index((unsigned short)(i+1));
                  text = sound;
						add_page(page++,text);
					};
				};
				if (file_name != NULL) delete [] file_name; // new on 130104 to fix a minor leak
				file_name = file_name_copy; // new on 090200  
//				dump(); // so user gets a private copy to edit -- -- commented out on 050803 since not faster and just creates files in user's directory - can dump when and if edited
			} else {
				load_page_resource("NOTEBOOK_SOUNDS");
			};
//#else
//			load_page_resource("NOTEBOOK_SOUNDS");
//#endif
		} else if (strcmp(file_name,"10") == 0) {
         built_in_notebook = OPTIONS_NOTEBOOK;
//#if TT_DEBUG_ON 
			if (tt_create_new_notebooks) {
				const int count = 11;
				RemoteIdentifier identifiers[count] 
					= {PROGRAMMER_HEAD_REMOTE,
						SOUND_ON_REMOTE,
						CITY_SIZE_REMOTE,
						MOUSE_BUTTONS_REMOTE,
						SCREEN_SIZE_REMOTE,
						SYSTEM_SPEED_REMOTE,
						SYSTEM_SLEEP_REMOTE,
						LETTER_SIZE_REMOTE,
						READING_SPEED_REMOTE,
						MARTIAN_ADVICE_LEVEL_REMOTE,
                  MARTIAN_SPEAK_REMOTE};
	//					MARTIAN_FONT_REMOTE, // too much work for not very important feature
//						LEGO_SERIAL_PORT_REMOTE}; // been obsolete for a long time - removed on 100202
				add_remotes(identifiers,0,count,tt_global_picture);
				if (file_name != NULL) delete [] file_name; // new on 130104 to fix a minor leak
				file_name = file_name_copy; // new on 090200
//				dump(); // new on 090200 -- commented out on 050803 since not faster and just creates files in user's directory
			} else {
				load_page_resource("NOTEBOOK_OPTIONS");
			};
//#else
//			load_page_resource("NOTEBOOK_OPTIONS");
//#endif
		} else if (strcmp(file_name,"12") == 0) { // new on 130202
//         built_in_notebook = OPTIONS_NOTEBOOK;
//#if TT_DEBUG_ON
//			if (tt_create_new_notebooks) { // commented out on 120203
			built_in_notebook = MATH_NOTEBOOK; // new on 140104
			int page = 1;
			Rational *number = new Rational();
			string string_value = copy_string("3/2"); 
			// think about whether this default causes more confusion and should be 0 and labels have examples instead? noted 091104
			number->set_value_as_string(string_value,3,TT_UNDETERMINED_NUMBER_FORMAT,TRUE); 
			// TT_UNDETERMINED_NUMBER_FORMAT,TRUE added on 091104 TT_BIGNUM_WITH_FRACTION makes more sense but doesn't work
			delete [] string_value;
			add_page(page++,variable_width_text_pad(S(IDS_FRACTION_NUMBER_FORMAT,"Fraction")));
			number->set_current_style(FALSE,FALSE,FALSE);
			add_page(page++,number);
			
			number = (Rational *) number->copy();
			add_page(page++,variable_width_text_pad(S(IDS_INTEGER_AND_PROPER_FRACTION_NUMBER_FORMAT,"Integer and\rProper Fraction")));
			number->set_current_style(TRUE,FALSE,FALSE);
			add_page(page++,number);

			number = (Rational *) number->copy();
			add_page(page++,variable_width_text_pad(S(IDS_EXACT_DECIMAL_OR_FRACTION_NUMBER_FORMAT,"Exact Decimal\ror Fraction")));
			number->set_current_style(FALSE,TRUE,FALSE);
			add_page(page++,number);

			number = (Rational *) number->copy();
			add_page(page++,variable_width_text_pad(S(IDS_EXACT_DECIMAL_OR_INTEGER_AND_PROPER_FRACTION_NUMBER_FORMAT,"Exact Decimal\ror Integer and\rProper Fraction")));
			number->set_current_style(TRUE,TRUE,FALSE);
			add_page(page++,number);
//			number->set_long_value(0); // doesn't really matter -- commented out since clobbers last example number (3/2)
			const int operation_count = 12;
			NumberOperation operations[operation_count] = {INTEGER_PART,FRACTION_PART,NUMERATOR,DENOMINATOR,SINE,COSINE,TANGENT,ARCSINE,ARCCOSINE,ARCTANGENT,NATURAL_LOG,LOG10};
			for (int i = 0; i < operation_count; i++) {
				number = new Rational(); // was (Rational *) number->copy(); - but caused the Integer Part1.5 display problem
				number->set_operation(operations[i]);
//				number->set_is_an_operation_with_no_value(TRUE); // new on 040803 -- otherwise you see Integer Part1.5 - removed on 050803 since changes behavior of operation
				add_page(page++,number);
			};
//#if TT_DEBUG_ON	
//			if (tt_create_new_notebooks) { // commented out on 120203
//			if (file_name != NULL) delete [] file_name; // new on 130104 to fix a tiny leak -- commented out on 210104 since used below - but why is it copied?
			if (file_name_copy != NULL) delete [] file_name_copy; // new on 130104
//			file_name = copy_ascii_string(file_name); // was file_name_copy prior to 120203 -- not clear why I'm doing this // new on 090200 -- commented out on 230104
//				dump(); // new on 090200 // commented out as an experiment on 060603
//			};
//#endif
			//} else { // this is the wrong notebook! commented out on 120203
			//	load_page_resource("NOTEBOOK_OPTIONS");
			//};
//#else
//			load_page_resource("NOTEBOOK_OPTIONS");
//#endif
      };
	};
   go_to_page(original_page_number); // new on 220802
   // followng needs to wait until XML coverage is complete
//#if TT_XML
//   if (tt_dump_as_xml) { // save convertion to XML format 
//      dump();
//   };
//#endif
   loaded = TRUE;
};

boolean skipped_sound_index(int i) {
  return(i == 17 || i == 18 || i == 19 ||
         i == 21 || i == 26 || i == 27 ||
         i == 33 || i == 34 || i == 35 ||
         i == 39 || i == 40 || i == 41 || i == 46);
};

short int Notebook::add_remotes(RemoteIdentifier *identifiers, int start, int stop, // stop was misnamed count prior to 121104
										  Picture *controlee) {
	short int page = start+1;
	Cubby *cubby;
	Text *text;
	Remote_Integer *remote_integer;
	for (int i = start/2; i < stop; i++) {
		RemoteIdentifier identifier = identifiers[i];
		text = variable_width_text_pad(S(IDS_X_LONG_LABEL+identifier),text_pad_see_all);
		add_page(page++,text);
		cubby = new Cubby();
		if (remote_address(identifier)) { // put both in the same box
			cubby->set_number_of_holes(2);
			remote_integer = new Remote_Address(tt_global_picture,AVENUE_REMOTE);
			cubby->initialize_hole(0,remote_integer);
			cubby->set_label(0,copy_string(S(IDS_X_SHORT_LABEL+AVENUE_REMOTE)));
			remote_integer = new Remote_Address(tt_global_picture,STREET_REMOTE);
			cubby->initialize_hole(1,remote_integer);
			cubby->set_label(1,copy_string(S(IDS_X_SHORT_LABEL+STREET_REMOTE)));
		} else {
         if (remote_picture(identifier)) {
			   remote_integer = new Remote_Picture(controlee,identifier);
		   } else {
			   remote_integer = new Remote_Integer(controlee,identifier);
		   };
//		   remote_integer->set_read_only(TRUE);
		   cubby->initialize_hole(0,remote_integer);
		   cubby->set_label(0,copy_string(S(IDS_X_SHORT_LABEL+identifier)));
      };
      add_page(page++,cubby);
	};
	return(page);
};

void Notebook::load_page_resource(ascii_string resource_name) {
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	long size;
	HGLOBAL resource = tt_main_window->get_resource(resource_name,size);
   if (resource == NULL) return; // something wrong -- warn??
	character *page = (character *) LockResource(resource);
	StringInputStream pad_in(page,size);
	load_notebook(&pad_in);
	UnlockResource(resource);
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	FreeResource(resource);
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
};

int Notebook::load_notebook(InputStream *pad_in) {
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	short int page = 1;
//   NaturalLanguage language;
	character cookie_version_number;
	if (!read_cookie(pad_in,cookie_version_number,language,file_name)) {
		return(-1);
	};
// following commented out on 260299 so can run with old and new versions simultaenously
//   if (tt_toolbox == NULL || // must be the main one then
//       pointer_to_toolbox_notebook() == this) {
//      version_number = latest_version_number; // why??
//   } else {
      version_number = cookie_version_number;
//   };
//	int saved_tt_notebook_version = tt_notebook_version;
//	tt_notebook_version = version_number;
	if (cookie_version_number > latest_version_number) {
		character buffer[512];
		output_string_stream out(buffer,512);
		out << S(IDS_NOTEBOOK_FUTURE_VERSION) << space()
			 << S(IDS_THIS_VERSION_CANT) << space() << tt_file_name << "." << file_name 
			 << S(IDS_GET_NEW_VERSION);
		out.put('\0');
		say_error(buffer,TRUE);
		return(cookie_version_number);
	};
	unsigned int short page_count;
	pad_in->read((string) &page_count,sizeof(page_count));
   if (page_count == 0) page_count = 2; // fixed in version 6  && cookie_version_number < 6
	long encoding_size;
	string encoding;
	for (; page <= page_count; page++) {
		if (pad_in->empty()) { // new on 280201
			log("Notebook's encoding ended early.",FALSE,TRUE);
			tt_error_file() << "Stopping on page " << page << " and expected to find " << page_count << " pages." << endl;
			page_count = page-1; 
			encoding = NULL;
			encoding_size = 0;
			old_pages->set_page(page-1,encoding,encoding_size); // previous one probably broken
			old_pages->set_page(page,encoding,encoding_size);
			break;
		};
		if (cookie_version_number >= first_version_with_large_pages) {
			pad_in->read((string) &encoding_size,sizeof(encoding_size));
		} else {
			short unsigned int short_size;
			pad_in->read((string) &short_size,sizeof(short_size));
			encoding_size = short_size;
		};
#if TT_DEBUG_ON
      if (encoding_size > 2000000) {
         log("Should this really be more than two million bytes long?");
      };
#endif
		if (encoding_size > 0) {
			encoding = new character[encoding_size];
			pad_in->read((string) encoding,encoding_size); // );
		} else {
			encoding = NULL;
		};
		old_pages->set_page(page,encoding,encoding_size);
//      pages->set_version_number(version_number);
      old_pages->set_language(language);
#if TT_DEBUG_ON
		if (tt_debug_mode == 545) {
			tt_error_file() << endl << "Loading page " << page 
							<< " whose encoding's length is " << encoding_size << endl;
			if (tt_toolbox != NULL) { // not too early
				Sprite *temp = pointer_to_page(page,FALSE);
				if (temp != NULL) {
					temp->top_level_describe(tt_error_file());
					delete temp;
				} else {
					tt_error_file() << "NULL";
				};
			};
		};
#endif
	};
	if (!tt_loading_robot_notebook) {
		if (encoding != NULL) { 
			// last one isn't blank so
			go_to_page((short int) (page_count+1));
		} else { 
			go_to_page(page_count);
		};
		if (page_count <= 2) { // added on 150799 since otherwise not updated properly
			switch_contents();
		};
	};
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	return(cookie_version_number);
//	tt_notebook_version = saved_tt_notebook_version;
};

boolean describe_pad(InputStream *stream, output_stream &text_out, int version_number, Article article) {
	print_type(PROGRAM_PAD,text_out,article); 
	// on 110700 removed the whole attempt to describe notebooks inside of things since got very hairy
	Notebook *ignore;
	UnwindProtect<boolean> set(tt_loading_to_describe,TRUE); // new on 160801
   // note this won't produce a mini-dump
	try {
		ignore = load_notebook(stream,version_number,tt_language,FALSE,NULL,NULL,0,FALSE);
	} catch (Sprite *) { // new on 160801 to catch trouble loading
		ignore = NULL;
	};
	if (ignore != NULL) {
		ignore->destroy();
		return(TRUE);
	};
	return(FALSE);
};

/*
	if (version_number >= first_version_to_save_copied_nests) {
		text_out << end_sentence(); // new on 110700
//		boolean comment_added = // isn't meaningful
		describe_comment_encoding(stream,text_out,version_number,PROGRAM_PAD);
		if (version_number >= first_version_to_label_notebooks) {
			boolean saved_tt_loading_to_describe = tt_loading_to_describe;
			tt_loading_to_describe = TRUE;
			Text *label = (Text *) load_item(stream,version_number);
			tt_loading_to_describe = saved_tt_loading_to_describe;
			if (label != NULL) { // added on 303099 so Marty can describe labelled pads
//				if (!comment_added) {
					text_out << space() << S(IDS_LABELED_BY) << " \""; 
//				label->describe(text_out);
					string text_label;
					unsigned short int text_label_length;
					label->current_text(text_label,text_label_length);
					text_out.write(text_label,text_label_length);
					text_out << "\"";
					text_out << S(IDS_LABELED_BY2,"");
				};
				label->destroy();
//			};
		};
	};
	stream.get(); // ignore left page number
	unsigned char file_name_length = (unsigned char) stream.get();
	if (file_name_length == 0) {
//		char cookie[9];
//		stream.read((character *) cookie,8);
		character cookie_version_number;
		NaturalLanguage ignore_language;
		read_cookie(stream,cookie_version_number,ignore_language);
		unsigned int short page_count;
		stream.read((character *) &page_count,sizeof(page_count));
//		print_article(article,text_out,S(IDS_BOOK_CONTAINING));
//      text_out << S(IDS_BOOK_CONTAINING); // commented out on 300699
		text_out << space() << S(IDS_CONTAINING) << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << page_count << S(IDS_PAGES);
		// added the following on 130200 so notebooks can be described when in a box or whatever
		long encoding_size;
		for (int page = 1; page <= page_count; page++) {
			if (cookie_version_number >= first_version_with_large_pages) {
				stream.read((character *) &encoding_size,sizeof(encoding_size));
			} else {
				short unsigned int short_size;
				stream.read((character *) &short_size,sizeof(short_size));
				encoding_size = short_size;
			};
			stream.ignore(encoding_size);
		};
	} else {
//		tt_error_file() << "Shouldn't be describing a notebook defined by a file." << endl;
//		print_article(article,text_out,S(IDS_BOOK_CONTAINING));
		text_out << end_sentence(); // commented out on 300699
		// following is new on 300399 so Marty can talk about pads in box for example
		ascii_string file_name = new char[file_name_length+1];
		stream.read((character *) file_name,file_name_length);
		delete [] file_name; // could try to "talk" about it?
      text_out << S(IDS_BOOK_CONTAINING);
	};
	if (version_number >= first_version_to_save_programmer_and_remotes_accurately) { // new on 050100
		stream.get(); // ignore for now whether this is a "friend" of Tooly
	};
	return(TRUE);
};
*/

void Notebook::add_page(int page_number, Sprite *item) { // was a short int prior to 200802 - no reason to restrict it like that
   go_to_page(page_number,FALSE);
#if TT_XML
   Page *page;
   if (item == NULL) {
      page = NULL;
   } else {
      page = new SpritePage(item);
   };
   set_page_internal(page_number,page);
#else
	left_page_current = (flag) (page_number&1); // odd
	if (left_page_current) {
		left_page = item;
	} else {
		right_page = item;
	};
	if (save_encoding()) { // conditional use new on 240200
		define_page(FALSE,FALSE);
	};
#endif
};

Sprite *Notebook::pointer_to_page_i_am_on(Sprite *sprite) {
	if (sprite == left_page) {
		return(pointer_to_page(left_page_number())); //,sprite->pointer_to_background()));
	} else if (sprite == right_page) {
		return(pointer_to_page(right_page_number())); //,sprite->pointer_to_background()));
	} else {
		say_error(IDS_TOONTALK_MIXED_UP);
		return(NULL);
	};
};

Sprite *Notebook::pointer_to_page(short int n, Sprite *by, boolean copy_page) {
	// on 180204 added copy_page arg and removed obsolete second arg (use_cache)
#if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {
      log("Getting pointer to page for debug target.");
   };
#endif
	Sprite *result = NULL;
/* problem with cache is that a "sub" notebook can be used by different robots simultaneously
	if (use_cache) { // && n < cache_size) {
		if (cache != NULL) result = cache[n];
		if (result == NULL) {
			Picture *old_tt_copying_insides_for = tt_copying_insides_for;
			tt_copying_insides_for = picture;
			result = pages->nth(n,floor,this,picture);
			tt_copying_insides_for = old_tt_copying_insides_for;
			if (result == NULL) return(result);
			switch (result->kind_of()) {
				case PROGRAM_PAD:
					if (cache == NULL) {
						cache_size = pages->length();
						cache = new NotebookPointer[cache_size];
						for (int i=0; i < cache_size; i++) {
							cache[i] = NULL;
						};
					};
					cache[n] = (Notebook *) result;
					cache[n]->increment_ref_count();
//					break;
//				case PICTURE:
//					((Picture *) result)->activate();
//					break;
			};
		} else { // was cached
			result->set_size_percent(100);
//			result->save_size();
			result->set_graspable(TRUE); // due to sharing may have been reset
			cache[n]->increment_ref_count();
//			((Notebook *) result)->go_to_page(1);
		};
		result->set_size_and_location_to_region(region);
		return(result);
	};
*/
//	Picture *old_tt_copying_insides_for = tt_copying_insides_for;
//	tt_copying_insides_for = picture;
#if TT_XML
//   if (using_xml()) { // commented out on 270902
      result = page_sprite(n);
      if (result != NULL) {
			if (copy_page) { // condition new on 180204
				result = result->top_level_copy(); //  top_level new on 270804 since otherwise remote looks can get confused 
				// (at least when converting an off-screen page -- e.g. notebook on erased box)
			};
         // moved to switch_contents on 231002 -- 
			// restored the following on 281002 since switch contents doesn't get called by robots working off-screen
         if (picture != NULL) { // new on 221002 -- old code pass the controlee down - should this too??
            result->now_on_back_of(picture);
         };
      } else if (pages == NULL) { // condition new on 270902
         return(NULL);
      };
//   };
#endif
   if (result == NULL) {
	   result = old_pages->nth(n,version_number,this,picture);
   };
	if (result != NULL) {
      if (result->in_notebook()) { // condition new on 221002 so don't do all this work if already knew it
         result->set_in_notebook(NULL); // new on 190702 since isn't there anymore
      };
      if (showing_on_a_screen()) { // || tt_log_version_number <= 9) {
   	   TTRegion region;
	      page_region((boolean) (n&1),region);
		   result->set_size_and_location_to_region(region);
      } else if (by != NULL && by->still_running()) { // then default to robot's chest
         ((Robot *) by)->resize_to_fit_chest(result); // why bother??
      }; // else don't care what size -- right?
	};
//	tt_copying_insides_for = old_tt_copying_insides_for;
	return(result);
};

Sprite *Notebook::copy_page(Sprite *page, SelectionReason reason, Sprite *by, Background *background_of_by) {
	Picture *old_tt_copying_insides_for = tt_copying_insides_for;
//	Notebook *containing_notebook = (Notebook *) leader;
//	Picture *picture = containing_notebook->pointer_to_picture();
	tt_copying_insides_for = picture;
	Sprite *the_copy = page->top_level_copy();
	switch (the_copy->kind_of()) {
		case PROGRAM_PAD:
			// e.g. sub notebooks of a picture notebook
//			tt_last_notebook = (Notebook *) the_copy;
			if (picture != NULL) {
				Picture *sub_picture = picture->which_sub_picture_on_page(right_page_number());
//				sub_picture->increment_ref_count(this); 
				// new on 041299 but not needed since problem was subpicture was of copy_of_me and not the "real" thing
				((Notebook *) the_copy)->set_picture(sub_picture);
			};
			if (right_page_number() == 10) {
				user_did(GLOBAL_HISTORY,USER_LOOKED_AT_OPTIONS_NOTEBOOK);
			};
			break;
	};
   if (tt_toolbox != NULL && this != tt_toolbox_notebook) { 
		// prior to 150704 was pointer_to_toolbox_notebook()) { // tt_toolbox != NULL && added on 010499 for puzzles
      the_copy->set_home_notebook(this);
   };
	tt_copying_insides_for = old_tt_copying_insides_for;
	if (background_of_by != NULL) background_of_by->add_item(the_copy);
	// pass the notebook itself and let its current page number
	// be recorded
	if (reason != ITEM_DROPPED && reason != VACUUM_USED) { // only if reason is a grasp?? (probably) -- second condition added 110205
		record_event(GRASP_ITEM,by,background_of_by,this,FALSE,0,DONT_ADD_TO_WORKING_SET,TRUE);	
	};
   the_copy->set_in_notebook(NULL); // new on 160702 -- why???
	if (reason == GRASPED || reason == VACUUM_USED_TO_BLANK) { // added reason == VACUUM_USED_TO_BLANK on 180804
      millisecond duration = default_duration();
		the_copy->animate_to_good_size(duration,this);
      // following new on 150702
      if (page->active()) { // not ok_to_activate since is still in the notebook and can't be in the vacuum (new on 170702)
//         the_copy->set_active(TRUE); // as it was before
         the_copy->stop_all(FALSE,FALSE); 
			// new on 170702 since otherwise things move using their speed -- args added 290802 so doesn't suspend robots
         the_copy->do_after(duration+1,the_copy,ACTIVATE_CALLBACK); // +1 on 160702 to be sure this happens after the expansion
      };
   } else if (page->ok_to_activate()) { // (page == left_page && left_page_active) || (page == right_page && right_page_active)) {
      // immediate version of the above (new on 150702)
//      the_copy->set_active(TRUE);
      the_copy->activate();
   };
	if (reason != VACUUM_USED) { // condition new on 110205
		record_event(NEW_ITEM,by,background_of_by,the_copy,TRUE,0,ADD_TO_WORKING_SET_REGARDLESS);
	};
// the pad is grasped but a copy is obtained so switch availability
//		   set_available_for_indexing(TRUE);
//			the_copy->set_available_for_indexing(FALSE); // until released
	return(the_copy);
};

void Notebook::set_home_notebook(Notebook *) { // don't recursively descend as is the default
};

void Notebook::set_default_favorite_size_and_location(city_coordinate width,
                                                      city_coordinate height,
                                                      city_coordinate x,
                                                      city_coordinate y) {
  if (favorite_width <= 0) {
     favorite_width = width;
     favorite_height = height;
     favorite_llx = x;
     favorite_lly = y;
  };
};

void Notebook::set_favorite_size_and_location() {
	if (floor != NULL && floor->inside_thought_bubble()) return; // just the robot's imagination so don't make these changes stick
	favorite_width = width;
   favorite_height = height;
   if (llx+width < 1) { // off the left side
      favorite_llx = tile_width/4-width; // a bit on on screen
   } else if (llx >= ideal_screen_width) {
      favorite_llx = ideal_screen_width-tile_width/4;
   } else {
      favorite_llx = llx;
   };
   if (lly+height < 1) { // off the bottom
      favorite_lly = tile_height-height;
   } else if (lly >= ideal_screen_height) {
      favorite_lly = ideal_screen_height-tile_height/4;
   } else {
      favorite_lly = lly;
   };
};

void Notebook::favorite_size(city_coordinate &width, city_coordinate &height) {
   if (favorite_width <= 0) {
      good_size(width,height);
   } else {
      width = favorite_width;
      height = favorite_height;
   };
};

int Notebook::page_count() {
#if TT_XML
//   if (using_xml()) { // commented out on 270902
      return(pages_count);
//   };
#else // new on 270902
	return(old_pages->length());
#endif
};

int Notebook::blank_page_number() {
#if TT_XML
//   if (using_xml()) {
      compute_pages(); // so is converted before doing this if need be
      for (int i = 0; i < pages_count-1; i += 2) {
         if (pages[i] == NULL && pages[i+1] == NULL) return(i+2);
      };
      // else add two empty pages
      expand_pages(pages_count+2);
      return(pages_count+2);
 //  };
#else
   return(old_pages->blank_page_number());
#endif
};

//Picture *Notebook::return_picture_copy() { // new on 121199
//	reset_picture_copy(NULL); // just a test 201299
//	if (picture_copy == NULL && picture != NULL) {
//		picture_copy = (Picture *) picture->top_level_copy(); // changed to top_level_copy on 201299
//	};
//	return(picture_copy);
//};

//void Notebook::reset_picture_copy(Picture *new_copy) {
//	if (picture_copy != NULL) picture_copy->destroy();
//	picture_copy = new_copy;
//};

boolean Notebook::current_value(NumberValuePointer &the_value, boolean &shared, Sprite *by) { // new on 151199
//	the_value.set_value(page_count());
	the_value = new LongNumberValue(page_count());
	shared = FALSE;
	return(TRUE);
};

void Notebook::set_file_name(string new_name) {
	if (file_name != NULL) {
		delete [] file_name;
	};
	file_name = new_name;
   if (file_name == NULL) return;
	if (strcmp(file_name,"bok") == 0) {
      built_in_notebook = MAIN_NOTEBOOK;
	} else if (strcmp(file_name,"rem") == 0) {
      built_in_notebook = LOCAL_REMOTES_NOTEBOOK;
	};
};

boolean Notebook::equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, SpritesPointer &expected_corresponding_nests, EqualReason reason) { // new on 060601
	if (other == this) return(TRUE);
	if (other->kind_of() != kind_of()) return(FALSE);
	if (is_blank() != other->is_blank()) return(FALSE);
	Notebook *other_notebook = (Notebook *) other;
	if (page_count() != other_notebook->page_count()) return(FALSE);
	if (left_page_number() != other_notebook->left_page_number()) return(FALSE);
	if (pointer_to_label() != NULL) {
		if (other_notebook->pointer_to_label() == NULL) return(FALSE);
		if (!pointer_to_label()->equal_to(other_notebook->pointer_to_label(),pending,corresponding_nests,expected_corresponding_nests,reason)) return(FALSE);
	} else if (other_notebook->pointer_to_label() != NULL) return(FALSE);
	if (pages == NULL && tt_log_version_number >= 38) return(TRUE); // both are empty notebooks - new on 290204
#if TT_XML
   if (pages != NULL) {
      int other_pages_count;
      PagePointer *other_pages = other_notebook->pointer_to_pages(other_pages_count);
      // can be different length if different number of blank pages at the end
      int last_non_blank_page = last_non_blank_page_number();
      int other_last_non_blank_page = other_notebook->last_non_blank_page_number();
      if (last_non_blank_page != other_last_non_blank_page) return(FALSE);
      for (int i = 0; i < last_non_blank_page; i++) {
         if (pages[i] == NULL) {
            if (other_pages[i] != NULL) return(FALSE);
         } else {
            if (other_pages[i] == NULL) return(FALSE);
            // both non-NULL
            // Can I do better than turn both into Sprites and compare? -- e.g. compare XML??
            Sprite *sprite = pages[i]->contents();
            if (sprite == NULL) return(FALSE); // error - how better to handled? (new on 211002)
            Sprite *other_sprite = other_pages[i]->contents();
            if (other_sprite == NULL) return(FALSE);
            if (!sprite->equal_to(other_sprite,pending,corresponding_nests,expected_corresponding_nests,reason)) return(FALSE);
         };
      };
      return(TRUE);
   };
#endif
   if (pointer_to_old_pages() == NULL) return(other_notebook->pointer_to_old_pages() == NULL);
	return(pointer_to_old_pages()->equal_to(other_notebook->pointer_to_old_pages()));
};

void Notebook::set_label_string(const_string text) {
	set_label(make_label(text,tt_black,FALSE));
};

void Notebook::set_label(Sprite *new_label) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("setting label of notebook debug target.");
	};
#endif
	if (label != NULL) {
		remove_follower(label);
		label->destroy();
	};
	label = new_label;
	if (label != NULL) {
		if (label->pointer_to_leader_offsets() == NULL) { // condition new n 260103
			update_textual_label_size_and_location(); // new on 280100 since pasting notebooks have wrong size labels initially
			add_follower(label);
		} else {
			add_follower(label,TRUE,INSERT_AT_END,TRUE);
		};
		// commented out on 190100 since does the wrong thing for notebooks saved in cities and the like
		// this is just appropriate for main notebook if coming out of Tooly
//		if (label->kind_of() == TEXT) { // conditional added 201099
//			label->set_visible(FALSE); // moved here on 170999 so invisible until displayed which will fix its geometry
//		};
	};
};

char notebook_zip_file_name[MAX_PATH]; // new on 171203
int notebook_zip_file_name_revision_number = -1;
int main_notebook_revision_number = -1;

void main_notebook_changed() { // new on 171203
	if (tt_zip_description_archive_element == NULL) {
		return; // new on 240204 for robustness
	};
	if (main_notebook_revision_number < 0) { // not yet initialized
		main_notebook_revision_number = xml_get_attribute_int(tt_zip_description_archive_element,L"MainNotebookRevisionNumber",0);
	};
	main_notebook_revision_number++;
	xml_set_attribute(tt_zip_description_archive_element,L"MainNotebookRevisionNumber",main_notebook_revision_number);
};

boolean new_notebook_version(int new_notebook_version) {
	if (main_notebook_revision_number == new_notebook_version) return(FALSE); // already was known
	main_notebook_revision_number = new_notebook_version;
	xml_set_attribute(tt_zip_description_archive_element,L"MainNotebookRevisionNumber",main_notebook_revision_number);
	return(TRUE);
};

string zip_current_notebook_files() {
	char default_user_notebook_file[MAX_PATH];
	boolean use_default_user_notebook_file = FALSE;
	if (main_notebook_revision_number < 0) { // not yet initialized
		main_notebook_revision_number = xml_get_attribute_int(tt_zip_description_archive_element,L"MainNotebookRevisionNumber",0);
		Notebook *notebook = tt_toolbox_notebook; 
		if (main_notebook_revision_number == 0 && notebook != NULL) { // can notebook be NULL?
			if (tt_default_user_directory != NULL) { // don't copy default user's notebook to new user just because of this
				// new on 230804
				strcpy(default_user_notebook_file,tt_default_user_directory);
				strcat(default_user_notebook_file,tt_default_file_name);
				strcat(default_user_notebook_file,".bok.xml");
				if (file_exists(default_user_notebook_file,FALSE)) {
					use_default_user_notebook_file = TRUE; // don't create one but include this one in the demo
				} else {
					notebook->dump();
				};
			} else {
				notebook->dump(); // new on 150204 to make sure there is a saved version of the initial notebook
			};
		};
	};
	// zips up the current main notebook and subnotebooks if revised since last time
	if (main_notebook_revision_number != notebook_zip_file_name_revision_number) {
		notebook_zip_file_name_revision_number = main_notebook_revision_number;
		strcpy(notebook_zip_file_name,"notebook.");
		itoa(main_notebook_revision_number,notebook_zip_file_name+strlen(notebook_zip_file_name),10);
		strcat(notebook_zip_file_name,".zip");
		char source[MAX_PATH];
		strcpy(source,tt_user_directory);
		strcat(source,tt_file_name);
		strcat(source,".*.xml");
		char archive[MAX_PATH];
		strcpy(archive,tt_extracted_file_temp_directory);
		strcat(archive,notebook_zip_file_name);
		DeleteFile(archive); // in case tt_extracted_file_temp_directory has an old version don't want to add to it but create a new one - new on 280204
		boolean something_zipped = zip_file(archive,source);
		if (use_default_user_notebook_file) {
			something_zipped = zip_file(archive,default_user_notebook_file)||something_zipped;
		};
		if (something_zipped) {
			zip_file(tt_log_out_archive,archive);
		} else {
			tt_error_file() << "Failed to add the file " << source << " to the zip archive " << archive // error message new on 020504
								 << ". If this time travel archive depends upon changes to the main notebook, then it may not replay correctly." << endl;
			notebook_zip_file_name[0] = '\0';
		};
	};
	return(notebook_zip_file_name);
};

Sprites *load_items(InputStream *pad_in, int notebook_version, NaturalLanguage language,
                    Picture *controlee, boolean associate_robots_and_cubbies, Sprite *container, boolean geometry_too, boolean descendants_inactive) {
//   Sprites *items = NULL;
	UnwindProtect<Sprites*> set(tt_items_loaded_so_far,NULL); // replaced above to handle errors better
   Sprite *previous_item = NULL;
	load_started();
#if TT_DEBUG_ON
	long item_count = 0;
#endif
   while (!pad_in->empty()) {
      unsigned char type = (unsigned char) pad_in->get();
      if (type == END_OF_LIST_MARKER) break;
		if (type == NULL_MARKER) {
			insert_at_end(NULL,tt_items_loaded_so_far); // new on 180100
		} else {
			Sprite *new_item = load_item((SpriteType) type,pad_in,notebook_version,language,container,controlee,-1,FALSE,FALSE,descendants_inactive);
#if TT_DEBUG_ON
			item_count++;
#endif
			if (new_item != NULL) {
				if (geometry_too && !tt_dumping_to_test_equality_or_describe) { // conjunct added on 150300 
					if (new_item->kind_of() == PICTURE && notebook_version >= first_version_to_save_absolute_picture_geometry) {
						// new on 120200
						new_item->load_priority(pad_in);
						new_item->load_misc(pad_in);
						boolean deltas_too = (boolean) pad_in->get();
						if (deltas_too) {
							city_coordinate delta_x,delta_y;
							pad_in->read((string) &delta_x,sizeof(delta_x));
							pad_in->read((string) &delta_y,sizeof(delta_y));
							new_item->move_by(delta_x,delta_y);
						};
					} else {
						new_item->load_geometry(pad_in,notebook_version);
					};
				};
				if (tt_toolbox != NULL && toolbox_tool(new_item)) { // new on 050100
					new_item->set_favorite_size_and_location();
				}; // else { // start 110100 do both above and below when adding a friend of Tooly
				insert_at_end(new_item,tt_items_loaded_so_far);
				if (associate_robots_and_cubbies && notebook_version < first_version_to_save_cubby_of_robot &&
					 new_item->kind_of() == ROBOT && previous_item != NULL && previous_item->kind_of() == CUBBY) {
					// associate them but don't run just yet
//					((Robot *) new_item)->suspend();
//					((Robot *) new_item)->try_clause(((Cubby *) previous_item));
					((Robot *) new_item)->associate_with_box((Cubby *) previous_item);
					new_item->set_priority(previous_item->priority());
				};
				previous_item = new_item;
			} else { // geometry was dumped even if this is to be ignored so
				tt_global_picture->load_geometry(pad_in,notebook_version); // doesn't matter what its geometry is
			};
		};
   };
	load_ended();
	Sprites *result = tt_items_loaded_so_far;
   return(result);
};

Sprite *load_item(InputStream *pad_in,
                  int notebook_version, NaturalLanguage language,
						Sprite *container, Picture *controlee, long index, boolean is_controlee, boolean on_nest, boolean descendants_inactive) {
	if (pad_in->empty()) return(NULL); // added 150699 for robustness
	SpriteType type = (SpriteType) pad_in->get();
	Sprite *result = load_item(type,pad_in,
									   notebook_version,language,
										container,controlee,index,is_controlee,on_nest,descendants_inactive);
	return(result);
};

SpritePointer *shared_items = NULL;
long shared_items_count = 0;
const int extra_for_growth = 1024; 
// was 16 prior to 270900 - changing it cut overhead for Playground city load from 728ms to 22ms. 

#if TT_DEBUG_ON
LARGE_INTEGER start; 
LARGE_INTEGER stop;
long total_time_associating_shared_times = 0;
#endif

void associate_shared_item(long index, Sprite *item) {
   if (index < 0) return;
#if TT_CAREFUL
	if (index > 10000000) { // new on 261000 for a softer landing...
		log("Warning bad index to associate_shared_item");
		return;
	};
#endif
#if TT_DEBUG_ON
	if (tt_debug_mode == 270900) {
		QueryPerformanceCounter(&start);
	};
   if (tt_debug_mode == 1616) {
      tt_error_file() << "Associating ";
      item->describe_type(tt_error_file());
      tt_error_file() << " with index " << index << endl;
   };
#endif
   if (shared_items == NULL) {
      shared_items_count = index+extra_for_growth; 
      shared_items = new SpritePointer[shared_items_count];
      for (long i = 0; i < shared_items_count; i++) shared_items[i] = NULL;
   };
   if (index >= shared_items_count) {
      long new_shared_items_count = index+extra_for_growth;
      SpritePointer *new_shared_items = new SpritePointer[new_shared_items_count];
	   long i;
      for (i = 0; i < shared_items_count; i++) {
         new_shared_items[i] = shared_items[i];
      };
      for (i = shared_items_count; i < new_shared_items_count; i++) new_shared_items[i] = NULL;
      delete [] shared_items;
      shared_items = new_shared_items;
      shared_items_count = new_shared_items_count;
   };
   shared_items[index] = item;
#if TT_DEBUG_ON
	if (tt_debug_mode == 270900) {
		QueryPerformanceCounter(&stop);
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		total_time_associating_shared_times += (long) ((1000000*(stop.QuadPart-start.QuadPart))/frequency.QuadPart);
		tt_error_file() << "Total microseconds to associate shared items: " << total_time_associating_shared_times << endl;
	};
#endif
};

Sprite *shared_item(unsigned short int index) {
   if (shared_items == NULL) {
      // If I dump a picture with something on the back that has the picture as a controlee
      // then haven't finished loading to provide the value for the index
      // this kind of case works anyway since the controlee will be the picture it is on the
      // the back of.  But worry about the general case.
		// this can also occur sometimes when a robot saves his "home notebook" which he is part of
		// Maybe robot's shouldn't do this
#if TT_DEBUG_ON
		if (!tt_dumping_to_test_equality_or_describe) {
			tt_error_file() << "Requesting shared item #" << index << " before it is defined." << endl;
		};
#endif
      return(NULL);
   };
	if (index >= shared_items_count) {
		log(_T("Loading a shared item whose index is too large."));
 		tt_error_file() << index << endl;
		return(NULL); 
	};
#if TT_DEBUG_ON
   if (tt_debug_mode == 1616) {
      Sprite *item = shared_items[index];
      tt_error_file() << "Retrieved item ";
      if (item != NULL) {
			item->describe_type(tt_error_file());
		} else {
			tt_error_file() << "NULL";
		};
      tt_error_file() << " with index " << index << endl;
   };
#endif
   return(shared_items[index]);
};

void release_shared_load_items(boolean regardless) {
	if (!regardless && tt_loads_current > 0 && !tt_resetting_or_destroying_city) {
		// tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 but can cause memory errors while resetting during time travel
		return;
	};
   if (shared_items != NULL) {
      delete [] shared_items;
      shared_items = NULL;
      shared_items_count = 0;
   };
};

Sprite *load_item_internal(SpriteType type, InputStream *pad_in,
								   int notebook_version, NaturalLanguage language,
									Sprite *container, Picture *controlee, long index, boolean is_controlee, boolean on_nest, boolean descendants_inactive) {
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	switch (type) {
      case NEW_SHARED_ITEM:
         {
         unsigned short int index; // unsigned added 260200
         pad_in->read((string) &index, sizeof(index));
#if TT_DEBUG_ON
			if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
				tt_error_file() << index << endl;
				if (tt_debug_target == -index && index > 0) { // negative so no conflict with debug_counter
					print_spaces(tt_debug_load_depth,tt_error_file());
					tt_error_file() << "Loading index target." << endl;
				};
			};
#endif
         Sprite *item = load_item(pad_in,notebook_version,language,container,controlee,index,is_controlee,on_nest,descendants_inactive);
//         if (item != NULL) {
//            associate_shared_item(index,item);
//         };
#if TT_DEBUG_ON
			if (item != NULL && item->debug_counter == tt_debug_target) {
				log(_T("Creating shared debug target"));
			};
#endif
         return(item);
         };
		case EQUAL_TO_OLD_ITEM:
		case EQUAL_TO_OLD_ITEM_AND_GEOMETRY: // new on 120601
      case OLD_SHARED_ITEM:
         {
         unsigned short int index;
         pad_in->read((string) &index, sizeof(index));
#if TT_DEBUG_ON
			if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
				tt_error_file() << index << endl;
			};
#endif
         Sprite *item = shared_item(index);
			// obsoleted out on 060201 - except for old dumps - also wasn't the following a memory leak??
			if (tt_copying_entire_thought_bubble && item != NULL && notebook_version < first_version_to_save_thought_bubbles_disconnected) { 
				// really loading a thought bubble
				// this added on 280199 since a thought bubble should have no sharing
				item = item->top_level_copy(); // made top_level on 060201 since could have circularities in it
			};
         // ref count is incremented by things like set_home_notebook
//         if (item != NULL) item->increment_ref_count();
#if TT_DEBUG_ON
			if (item != NULL) {
				if (item->debug_counter == tt_debug_target) {
					log(_T("Referencing shared debug target"));
				} else if (tt_debug_mode == 280199) {
					tt_error_file() << "Referencing shared item ";
					item->top_level_describe(tt_error_file(),DEFINITE_ARTICLE);
					tt_error_file() << endl;
				};
			   if (is_controlee && item->kind_of() != PICTURE) {
				   log("Controlee not a picture.");
				};
			} else {
				if (!tt_dumping_to_test_equality_or_describe && !tt_loading_to_describe && !is_controlee) { // condition added 290300 -- !is_controlee added on 030302 since NULL is OK and can be legitimate
					// if loading a FORCE_REMOTE_INTEGER and no force feedback joystick is connected then this seems to be OK
					log("NULL old shared item.");
				};
			};
#endif
			if (type == EQUAL_TO_OLD_ITEM) {
				if (item != NULL) { 
					UnwindProtect<boolean> set(tt_copying_entire_thought_bubble,FALSE); // new on 051201 since special inside thought bubble behavior is inappropriate here
               // remove all this on 150702 since it caused parts of a game to become inactive (so stop and start were needed to fix things)
//					boolean is_active = item->active(); // new on 040702 so the top_level_copy doesn't activate this
//					item->Sprite::set_active(FALSE,FALSE);
					Sprite *result = item->top_level_copy();
//					item->Sprite::set_active(is_active,FALSE);
////				result->set_active(is_active,FALSE); // OK?
					return(result);
				} else {
					say_error("Couldn't find old item to copy while loading.");
					tt_error_file() << "Index is " << index << endl;
					return(NULL);
				};
			} else if (type == EQUAL_TO_OLD_ITEM_AND_GEOMETRY) {
				if (item != NULL) {
//					boolean is_active = item->active(); // new on 040702 so the top_level_copy doesn't activate this
//					item->Sprite::set_active(FALSE,FALSE);
					Picture *result = (Picture *) item->top_level_copy();
//					item->Sprite::set_active(is_active,FALSE);
////				result->set_active(is_active,FALSE); // OK?
					result->load_geometry(pad_in,notebook_version);
					return(result);
				} else {
					say_error("Couldn't find old item to copy while loading.");
					tt_error_file() << "Index is " << index << endl;
					return(NULL);
				};
			} else {
//#if TT_DEBUG_ON
//            if (item != NULL && item->kind_of() != PICTURE && item->kind_of() != BIRD && item->kind_of() != CUBBY && item->kind_of() != ROBOT && item->kind_of() != NEST && item->kind_of() != PROGRAM_PAD) {
//               log("debug this");
////               item = item->top_level_copy(); // experiment on 190702
//            };
//#endif
				return(item);
			};
         };
		case CUBBY:
		case BLANK_CUBBY:
		case LABELED_CUBBY:
			return(load_cubby(type,pad_in,notebook_version,language,container,controlee,index,descendants_inactive));
		case INTEGER:
		case RATIONAL_NUMBER:
		case BLANK_INTEGER:
//		case ERROR_INTEGER:
		case REMOTE_INTEGER:     
		case GLOBAL_REMOTE_INTEGER:
		case REMOTE_PICTURE:
      case REMOTE_TEXT_PICTURE:
      case GLOBAL_HYPERBOT_REMOTE_INTEGER:
      case GLOBAL_USER_REMOTE:
      case FORCE_REMOTE_INTEGER:
			return(load_number(type,pad_in,notebook_version,language,container,controlee,index,descendants_inactive));
		case TEXT:
		case BLANK_TEXT:
      case VARIABLE_WIDTH_TEXT:
         return(load_text(type,pad_in,notebook_version,language,index,container,descendants_inactive));
      case SOUND:
      case BLANK_SOUND:
      case BUILT_IN_SOUND:
      case USER_SOUND:
      case SPEECH_SOUND:
      case MCI_SOUND:
      case BLANK_BUILT_IN_SOUND:
      case BLANK_USER_SOUND:
      case BLANK_SPEECH_SOUND:
      case BLANK_MCI_SOUND:
      case BLANK_FORCE_FEEDBACK:
      case FORCE_FEEDBACK:
         return(load_sound(type,pad_in,notebook_version,language,index,container,descendants_inactive));
		case PICTURE:
		case BLANK_PICTURE:
		case REMOTE_APPEARANCE:
		case PICTURE_WITH_INDIRECTION:
      case USER_PICTURE:
		case BLANK_USER_PICTURE: // added 021199
//		case PICTURE_WITH_INSIDES:
			return(load_picture(type,pad_in,notebook_version,language,container,controlee,index,is_controlee,on_nest,descendants_inactive));
		case BIRD:
      case PRIMITIVE_BIRD:
			return(load_bird(type,pad_in,notebook_version,language,index,container,descendants_inactive));
		case NEST:
			return(load_nest(pad_in,notebook_version,language,container,controlee,index,descendants_inactive));
		case PROGRAM_PAD: 
			return(::load_notebook(pad_in,notebook_version,language,TRUE,container,controlee,index,descendants_inactive));
		case ROBOT:
		case BLANK_ROBOT:
//         return(load_robot_line(pad_in,notebook_version,language,type));
      case ROBOT_WITH_TOOL:
		case BLANK_ROBOT_WITH_TOOL:
			return(load_robot_line(pad_in,notebook_version,language,type,index,container,descendants_inactive));
      case ROBOT_WITH_TRAINING_COUNTER:
         return(load_robot_with_training_counter(pad_in,notebook_version,language,index,container,descendants_inactive));
		case UNDEFINED_ROBOT_MARKER:
         return(undefined_robot(pad_in,notebook_version,language,index,container,descendants_inactive));
		case EMPTY_PAGE_MARKER: // same as NOTHING_MARKER
			return(NULL);
		case BOMB: {
			Bomb *bomb = new Bomb(0,0,0);
			associate_shared_item(index,bomb);
			if (notebook_version >= first_version_to_save_copied_nests) {
				bomb->set_comment(load_item(pad_in,notebook_version,language));
			};
			return(bomb);
			};
		case TRUCK:
         return(load_truck(pad_in,notebook_version,language,container,controlee,index,descendants_inactive));
      case TOOLBOX:
//         floor->set_ok_for_toolbox_entry(TRUE);
			// revised on 280499 so that this only happens with the new versions which
			// support Tooly in puzzles
         if (notebook_version > first_version_to_label_notebooks) {
				toolbox_is_part_of_puzzle();
			} else if (tt_system_mode == PUZZLE) {
				return(NULL); // new on 020600 since old versions of puzzles never saved Tooly
			};
			if (tt_loading_to_ignore) { // new on 181202
				return(new Toolbox());
			};
			if (tt_toolbox == NULL) { // new on 120100
				tt_toolbox = new Toolbox();
				tt_toolbox->initialize();
			}; // else if already part of a background then added again - noted on 091202
         return(tt_toolbox); // was NULL prior to 110100
      case COPIER:
         return(load_copier(pad_in,notebook_version,language,index,descendants_inactive));
      case VACUUM:
         return(load_vacuum(pad_in,notebook_version,language,index));
      case EXPANDER:
         return(load_bike_pump(pad_in,notebook_version,language,index));
      case PROGRAMMER:
      case PROGRAMMER_ARM:
      case MOUSE_WITH_HAMMER:
			return(NULL); // warn??
      case TALK_BALLOON:
         return(load_talk_balloon(pad_in,notebook_version,language)); // new on 250100
	   case MARTIAN: // new on 220999
			return(load_martian(pad_in,notebook_version,language));
      case WHOLE_FLOOR: {
			Sprite *programmer_appearance = tt_programmer->pointer_to_appearance();
			if (programmer_appearance == NULL) { // new on 160999
				log("Loading a floor while programmer has no appearance.");
				return(NULL);
			};
         Background *background = programmer_appearance->pointer_to_background();
			if (background->pointer_to_floor() != NULL) { // check first now (220203) - also simplified - earlier could crash if pasted outside
				background = background->pointer_to_floor();
         };
         NaturalLanguage language;
         character cookie_version_number;
         if (!read_cookie(pad_in,cookie_version_number,language)) return(NULL);
         if (cookie_version_number >= 8) { // floor token is also dumped now
            pad_in->get(); // ignore token
         };
         Sprites *list = load_items(pad_in,cookie_version_number,language); // was latest_version_number
         if (list != NULL) {
            background->add_items(list);
            list->activate(); // in case some items are suspended
            list->remove_all();
            delete list;
         };
//         Sprites *remaining = list;
//         while (remaining != NULL) {
//            background->add_item(remaining->first());
//            remaining->set_first(NULL);
//            remaining = remaining->rest();
//         };
//         delete list;
         return(NULL);
								};
		case WHOLE_HOUSE: // new on 030200
			{
				unsigned char house_token = pad_in->get();
				if (house_token == HOUSE_MARKER) {
					city_coordinate x,y;
					tt_programmer->pointer_to_appearance()->lower_left_corner(x,y);
					block_number block_x,block_y;
					block_address_at(x,y,block_x,block_y);
					Block *block = tt_city->block_at(block_x,block_y,TRUE); // (block_number) (x/tt_ideal_block_width),(block_number) (y/ideal_screen_height),TRUE);
					short int index;
					boolean lot_is_free = block->next_free_lot(x,y,index); // resets x and y to good values
					House *house = load_house(pad_in,notebook_version,language,x,y,block);	
					if (lot_is_free) {
						block->add_house(house,index);
					} else {
						delete house;
						play_sound(PLOP_SOUND);
					};
				};
			return(NULL); // side effect of creating the house was the point
			};
      case HELICOPTER:
        return(new Sprite(HELIOLND,0,0,HELICOPTER));
      case FORCE_REMOTE_CONTROLEE:
        return(load_force_remote_controlee(pad_in,notebook_version,language,index)); // what about descendants_inactive?
		default:
         if (type >= GLOBAL_JOYSTICK0_REMOTE_INTEGER && type <= GLOBAL_JOYSTICK31_REMOTE_INTEGER) {
            return(load_number(type,pad_in,notebook_version,language,container,controlee,index,descendants_inactive));
			} else if (type == -1) { // end of file reached -- warn?? (or can this happen when loading "rem" notebook?
				throw(tt_global_picture); // prior to 140200 was return(NULL)
         } else {
				if (tt_dumping_to_test_equality_or_describe || tt_loading_to_describe) { // conditional added on 290300
					log("Something went wrong trying to describe something.");
					// could throw something different to distinguish this case
				} else {
					say_error(IDS_COULDNT_RECOGNIZE_NOTEBOOK_FILE,TRUE);
				};
			   throw(tt_global_picture); // prior to 140200 was return(NULL)
         };
	};
};

Sprite *load_item(SpriteType type, InputStream *pad_in,
						int notebook_version, NaturalLanguage language,
                  Sprite *container, Picture *controlee, long index, boolean is_controlee, boolean on_nest, boolean descendants_inactive) {
	load_started();
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
		if (type != 255) print_spaces(tt_debug_load_depth,tt_error_file());
		if (type == NEW_SHARED_ITEM) {
			tt_error_file() << "About to load a NEW shared item #"; // number printed in load_item_internal
		} else if (type == OLD_SHARED_ITEM) {
			tt_error_file() << "About to load a OLD shared item #";
		} else if (type == EQUAL_TO_OLD_ITEM) {
			tt_error_file() << "About to load a copy of an OLD shared item #";
		} else if (type == EQUAL_TO_OLD_ITEM_AND_GEOMETRY) {
			tt_error_file() << "About to load geometry and a copy of an OLD shared item #";
		} else if (type != 255 && type != -1) {
			tt_error_file() << "About to load an item of type " << type_string(type) << " " << (int) type 
							<< " file position is " << (int) pad_in->tellg() << endl;
		};
		tt_debug_load_depth++;
	};
#endif
	Sprite *result;
//	UnwindProtect<unsigned char> set(tt_log_version_number,31); // new on 040304 since read_string has changed
   if (tt_want_exception_breaks) {	
	   try {
		   result = load_item_internal(type,pad_in,notebook_version,language,container,controlee,
											    index,is_controlee,on_nest,descendants_inactive);
	   } catch (Sprite *) { // new on 260200 to catch trouble loading
		   result = NULL;
	   };
   } else {
	   try { // new on 161299
		   result = load_item_internal(type,pad_in,notebook_version,language,container,controlee,
											    index,is_controlee,on_nest,descendants_inactive);
	   } catch (...) {
		   if (!handle_internal_error(S(IDS_INTERNAL_ERROR_LOADING_ITEM,"Sorry but an internal error occurred while loading something. It is probably lost."),TRUE)) {
			   throw(tt_global_picture); // user wants to quit
		   };
		   result = NULL;
	   };
   };
   load_ended();
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
		tt_debug_load_depth--;
		if (result == NULL) {
			if (type != 255 && type != -1) {
				print_spaces(tt_debug_load_depth,tt_error_file());
				tt_error_file() << "Loaded NULL object of type " << (int) type << endl;
			};
		} else {
			print_spaces(tt_debug_load_depth,tt_error_file());
			result->print(tt_error_file());
			tt_error_file() << " just loaded. File position is " << (int) pad_in->tellg() << endl;
//			result->java_describe_as_comment(tt_error_file()); // commented out since can cause pcount() VC++ bug
//			tt_error_file() << endl;
		};
	};
#endif
	return(result);
};


Page::~Page() {
#if TT_ALPHA_FEATURE
	if (media_file_names != NULL) {
		delete [] media_file_names;
	};
#endif
};


void Page::destroy() { // abstracted on 260803
   decrement_ref_count();
   if (is_ref_count_zero()) { // condition new on 201002
      delete this; // updated 181002
   };
};

boolean SpritePage::dump(output_stream &stream, boolean just_prepare, boolean is_controlee, boolean on_nest) {
//   return(sprite->dump(stream,just_prepare,is_controlee,on_nest));
   // re-written on 291002
   long encoding_length; 
   string encoding = sprite->return_encoding(encoding_length,FALSE,FALSE);
   stream.write((character *) &encoding_length,sizeof(encoding_length));
	if (encoding_length > 0) {
		stream.write(encoding,encoding_length); 
      delete [] encoding;
      return(TRUE);
   } else {
      return(FALSE); // something is wrong
   };   
};

#if TT_XML

// new on 201002

SpritePage::~SpritePage() {
	if (sprite->pointer_to_leader()) { // new on 300104
		sprite->pointer_to_leader()->remove_follower(sprite);
	};
//	if (!sprite->destroy()) {
////		sprite->destroy(); // new on 130104 since can have a ref count of 2 initially (why?) -- commented out as experiment 300104
//	};
	if (XML != NULL) {
#if TT_DEBUG_ON
		int ref_count = // for debugging XML ref counts
#endif
		XML->Release();
#if TT_DEBUG_ON
		debug_ref_count(ref_count,XML);
#endif
	};
	sprite->destroy();
};

Sprite *SpritePage::contents() {
	return(sprite);
};

//xml_element *SpritePage::top_level_xml(xml_document *document) {
//	// rewritten on 240103 to store the XML
//	if (XML == NULL) {
//		XML = sprite->top_level_xml(document);
//	};
//	XML->AddRef();
//	return(XML);
//};

xml_element *SpritePage::top_level_xml(xml_document *document, xml_element *parent, Notebook *notebook) { // new on 220203
	// rewritten on 240103 to store the XML
	boolean dumping_main_notebook_to_clipboard = (notebook->main_notebook() && tt_dump_reason == DUMPING_TO_CLIPBOARD);
	if (XML == NULL || dumping_main_notebook_to_clipboard) {
		// added tt_dump_reason == DUMPING_TO_CLIPBOARD on 041004 so subnotebooks aren't saved
		// as references to file names
		// commenented out on 200903 || (tt_saving_media || (tt_might_include_media_in_time_travel_archives && !xml_generated_this_session))) {
		// combined the two branches on 280803 so both use top_level_xml etc.
//		xml_generated_this_session = TRUE; // new on 210703 -- made obsolete on 200903
		if (sprite == notebook->pointer_to_page_being_defined()) { // prior to 231203 was sprite->animation_in_progress()) {
			return(NULL); // sprite->xml(NULL,document)); // new on 171203 since if in process of being added to notebook should be shared with live object
		};
//		xml_element *parent = create_xml_element(L"Page",document,page_number); // new on 240105
		// moved here on 081203
//		UnwindProtect<boolean> set2(tt_saving_media,(!tt_saving_media && tt_save_media_in_sub_notebooks)); // new on 200903 to ensure that files can be transported
 		boolean really_saving_media = tt_saving_media;
		UnwindProtect<boolean> set2(tt_saving_media,TRUE); // always maintain the list of media files
		UnwindProtect<xml_element *> set3(tt_zip_description_archive_element,NULL); // new on 210903 so this gets its own media files and not the time travel or log archive 
		string old_tt_extra_media_file_names = tt_extra_media_file_names;
		tt_extra_media_file_names = NULL;
		Sprites *old_tt_sprites_with_media_to_save = tt_sprites_with_media_to_save; // new on 160304 -- otherwise successive pages refer to previous pages' media
		tt_sprites_with_media_to_save = NULL;
		if (!dumping_main_notebook_to_clipboard) { // condition new on 041004
			XML = parent;
#if TT_DEBUG_ON
			int ref_count = // for debugging XML ref counts
#endif
			XML->AddRef(); // new on 200804 since is held by this
#if TT_DEBUG_ON
			debug_ref_count(ref_count,XML);
#endif
		}; 
		sprite->top_level_xml(document,parent);
		// rewritten on 200804 to make it easier to deal with reference counting
//		XML = sprite->top_level_xml(document,parent);
		// following introduced with TT_ALPHA_FEATURE
		if (media_file_names != NULL) {
			delete [] media_file_names;
			media_file_names = NULL;
		};
		if (tt_sprites_with_media_to_save != NULL || tt_extra_media_file_names != NULL) { // new on 081203
			media_file_names = ::media_file_names(tt_sprites_with_media_to_save,tt_extra_media_file_names);
			if (media_file_names != NULL) {
				xml_set_attribute(XML,L"Media",media_file_names);
//				if (really_saving_media) { // commented out on 280204 since should maintain these file names regardless
					tt_extra_media_file_names = old_tt_extra_media_file_names;
					add_new_media_file_names(media_file_names,tt_extra_media_file_names);
//				};
			};
		} else {
			tt_extra_media_file_names = old_tt_extra_media_file_names;
		};
		tt_sprites_with_media_to_save = old_tt_sprites_with_media_to_save;
		return(XML);  // new on 150603 since just added it to parent - no need to clone until used again
	//} else if (tt_saving_media || (tt_might_include_media_in_time_travel_archives && !xml_generated_this_session)) { // new on 230303
	//	xml_generated_this_session = TRUE; // new on 210703
	//	return(sprite->xml_create_and_append_element(parent,document));
	};
	// prior to 190703 this did a very dangerous coercion
	xml_element *XML_copy = xml_node_to_element(xml_clone_node(XML)); // was clone prior to 140603 - restored clone on 150603 since need element specific version
//	XML->Release(); // new on 200804
	if (parent != NULL) {
#if TT_DEBUG_ON
		int ref_count = // for debugging XML ref counts
#endif
		XML_copy->AddRef(); // new on 200804 since caller is responsible for releasing it and xml_append_child does release it
#if TT_DEBUG_ON
		debug_ref_count(ref_count,XML_copy);
		if (tt_debug_mode == 41004) {
			xml_debug_node(XML_copy);
		};
#endif
		xml_append_child(XML_copy,parent); // missing prior to 150303 
		// doesn't this do it twice if added above (due to no XML initially?) - asked 230303
	};
	if (media_file_names != NULL) { // new on 270204 -- removed tt_saving_media && on 280204
		add_new_media_file_names(media_file_names,tt_extra_media_file_names);
	};
	return(XML_copy); 
};


XMLPage::XMLPage(xml_element *xml) :
   XML(xml),
	sprite(NULL) {
//	xml_generated_this_session(FALSE) { // made obsolete on 200903
#if TT_DEBUG_ON
	int ref_count = // for debugging XML ref counts
#endif
   XML->AddRef();
#if TT_DEBUG_ON
	debug_ref_count(ref_count,XML);
#endif
};

XMLPage::~XMLPage() {
   if (sprite != NULL) {
      sprite->destroy();
   };
#if TT_DEBUG_ON
	int ref_count = // for debugging XML ref counts
#endif
   XML->Release();
#if TT_DEBUG_ON
	debug_ref_count(ref_count,XML);
#endif
};

Sprite *XMLPage::contents() {
   if (sprite == NULL) {
      sprite = xml_load_sprite(XML,tag_token(XML));
		set_sprites_to_active_now(); // new on 080403 since otherwise pages are inactive
		if (sprite != NULL) {
			sprite->update_display(UPDATE_TO_FIT_ON_PAGE); // new on 071004 to fix bug with sizes of robots (and others with parts outside) 
			// if city saved in a different resolution
		};
   }; // what if error and sprite is NULL? Caller should be prepared
#if TT_DEBUG_ON
	if (tt_debug_mode == 270804) {
		xml_debug_node(XML);
	};
#endif
	return(sprite);
};

xml_element *XMLPage::top_level_xml(xml_document *document, xml_element *parent, Notebook *notebook) {
//   XML->AddRef(); -- commented out on 150303 since same node can't be added to two parents - don't know why -- SEXPs can
//	xml_element *XML_copy = (xml_element *) xml_shallow_copy(XML); // didn't work right so copy the whole thing deeply
	// commented out on 200903
//	if (tt_saving_media || (tt_might_include_media_in_time_travel_archives && !xml_generated_this_session)) { 
//		// new on 230303 
//		xml_generated_this_session = TRUE; // new on 210703
//		// new on 230303 - unfortunately have to create the sprite and save them just to get the media files
//		// maybe should at least save a bit saying whether there is any embedded media??
////		Sprite *sprite = contents(); // rewritten on 140803 to compute it and to save it (OK?)
//		sprite = contents();
//		return(sprite->top_level_xml(document,parent));
//	};
	// prior to 190703 this did a very dangerous coercion
	// commented out on 200804 since TT_ALPHA_FEATURE is 1 and don't want to maintain it 
//#if TT_POST329&&!TT_ALPHA_FEATURE
//	if (tt_save_media_in_sub_notebooks) { // wasteful but does the job - new on 261103
//		Sprite *sprite = contents();
//		if (sprite != NULL) { // else warn?
//			xml_element *element = sprite->top_level_xml(document,NULL); // parent);
////			sprite->destroy(); // can cause problems and I can live with a leak if this switch isn't normally set
//			element->Release();
//		};
//	};
//#endif
	if (notebook->main_notebook() && tt_dump_reason == DUMPING_TO_CLIPBOARD) { 
		// new on 041004 to deal with saving the main notebook to the clipboard 
		sprite = contents();
//		xml_element *parent = create_xml_element(L"Page",document,page_number); // new on 240105
		sprite->top_level_xml(document,parent);
		return(parent);
	};
	xml_element *XML_copy = xml_node_to_element(xml_clone_node(XML)); // was clone prior to 140603
//	XML->Release(); // new on 200804
	if (parent != NULL) {
#if TT_DEBUG_ON
		int ref_count = // for debugging XML ref counts
#endif
		XML_copy->AddRef(); // new on 200804 since caller is responsible for releasing it and xml_append_child does release it
#if TT_DEBUG_ON
		debug_ref_count(ref_count,XML_copy);
#endif
		xml_append_child(XML_copy,parent);
		if (sprite != NULL) {
			xml_set_attribute(parent,L"PageInstantiated",1); // new on 170105 for demos and time travel (since random and clipboard sensor might act differently otherwise)
		};
	};
		if (sprite != NULL && !dumping_an_individual()) { // rewritten on 240105 -- added individual test on 240105 since wasteful otherwise
			xml_set_attribute(XML_copy,L"PageInstantiated",1); // new on 170105 for demos and time travel (since random and clipboard sensor might act differently otherwise)
		};
	// following new with ALPHA_FEATURE (271003)
	if (media_file_names != NULL) { // commented out && tt_saving_media on 280204 
		// since need to maintain these lists in any case - e.g. notebook within notebook
		add_new_media_file_names(media_file_names,tt_extra_media_file_names);
	};
#if TT_DEBUG_ON
   if (tt_debug_mode == 280802) {
      xml_debug_node(XML_copy,L"XMLPage::xml_create_and_append_element");
   };
#endif
   return(XML_copy);
};

boolean XMLPage::dump(output_stream &stream, boolean just_prepare, boolean is_controlee, boolean on_nest) {
   Sprite *sprite = contents();
   if (sprite == NULL) return(FALSE);
//   return(sprite->dump(stream,just_prepare,is_controlee,on_nest));
   // fixed on 301002 -- above didn't store encoding length
   long encoding_length; 
   string encoding = sprite->return_encoding(encoding_length,FALSE,FALSE);
   stream.write((character *) &encoding_length,sizeof(encoding_length));
	if (encoding_length > 0) {
		stream.write(encoding,encoding_length); 
      delete [] encoding;
      return(TRUE);
   } else {
      return(FALSE); // something is wrong
   };
};

#endif

// new on 201002

OldFormatPage::OldFormatPage(string encoding, int encoding_length, int version_number, NaturalLanguage language) :
   encoding(encoding),
   encoding_length(encoding_length),
   version_number(version_number),
   language(language),
   sprite(NULL) {
#if TT_DEBUG_ON
   if (encoding_length > 2000000) {
      log("Should this really be more than 2 million bytes long?");
   };
#endif
};

OldFormatPage::~OldFormatPage() {
   if (sprite != NULL) {
      sprite->destroy();
   };
   if (encoding != NULL) {
      delete [] encoding;
   };
};

Sprite *OldFormatPage::contents() {
   if (sprite == NULL) {
#if TT_DEBUG_ON
		sprite = sprite_from_encoding(encoding,encoding_length,version_number,language);
#else
		// following is new on 030903 to recover more gracefully -- don't use tt_want_exception_breaks since this is easy to localize
		try {
			sprite = sprite_from_encoding(encoding,encoding_length,version_number,language);
		} catch (...) {
			sprite = variable_width_text_pad("Sorry, something went wrong loading a notebook page saved in the old format.\nPlease report this to support@toontalk.com");
		};
#endif
   };
	return(sprite);
};

#if TT_XML

xml_element *OldFormatPage::top_level_xml(xml_document *document, xml_element *parent, Notebook *notebook) { // new on 220203
	if (sprite == NULL) {
		sprite = contents();
		if (sprite == NULL) {
			return(NULL);
		};
	};
//	xml_element *parent = create_xml_element(L"Page",document,page_number); // new on 240105
	// copied here on 081203
	boolean really_saving_media = tt_saving_media;
	UnwindProtect<boolean> set2(tt_saving_media,TRUE); // always maintain the list of media files
	UnwindProtect<xml_element *> set3(tt_zip_description_archive_element,NULL); // new on 210903 so this gets its own media files and not the time travel or log archive 
	string old_tt_extra_media_file_names = tt_extra_media_file_names;
	tt_extra_media_file_names = NULL;
//   xml_element *element = sprite->top_level_xml(document,parent);
	xml_element *element = parent;
	sprite->top_level_xml(document,parent); // rewritten 200804
	if (media_file_names != NULL) {
		delete [] media_file_names;
		media_file_names = NULL;
	};
	if (tt_sprites_with_media_to_save != NULL || tt_extra_media_file_names != NULL) { // new on 081203
		media_file_names = ::media_file_names(tt_sprites_with_media_to_save,tt_extra_media_file_names);
		if (media_file_names != NULL) {
			xml_set_attribute(element,L"Media",media_file_names);
//			if (really_saving_media) { // commented out on 280204
				tt_extra_media_file_names = old_tt_extra_media_file_names;
				add_new_media_file_names(media_file_names,tt_extra_media_file_names);
//			};
		};
	};
	return(element);
};

//void OldFormatPage::xml(xml_element *element, xml_document *document) {
//   Sprite *sprite = contents();
//   if (sprite == NULL) return;
//   sprite->xml(element,document); // adds XML elements and attributes to element
//};

#endif

boolean OldFormatPage::dump(output_stream &stream, boolean just_prepare, boolean is_controlee, boolean on_nest) {
   stream.write((char *) &encoding_length,sizeof(encoding_length));
   stream.write(encoding,encoding_length);
   return(TRUE);
};


Pages::~Pages() {
	if (encoding != NULL) delete [] encoding;
	if (next_page != NULL) delete next_page;
};

Pages *Pages::copy(boolean also_copy_offsets) {
   // risk of blowing the stack -- should re-write iteratively
	Pages *next_page_copy = NULL;
	if (next_page != NULL) {
		next_page_copy = next_page->copy();
	};
	string encoding_copy = new character[encoding_length];
	memcpy(encoding_copy,encoding,encoding_length);
	return(new Pages(encoding_copy,encoding_length,next_page_copy,language));
};

short int Pages::length() {
	short int answer = 1;
//	if (next_page == NULL) return(answer);
//  return(1+next_page->length());
	Pages *remaining = next_page;
	while (remaining != NULL) {
		answer++;
		remaining = remaining->rest();
	};
	return(answer);
};

Sprite *Pages::nth(short int index, int version_number, Sprite *container, Picture *controlee) {
	// check for maximum page number??
	Pages *remaining = this;
	int i = index;
	while (i > 1) { // 1-indexing since that's what the page numbers seem (was != prior to 131200 but not too robust for bad calls)
		i--;
		remaining = remaining->rest_or_add_blank();
	};
	string nth_encoding;
	long nth_encoding_length;
	remaining->encoding_and_length(nth_encoding,nth_encoding_length);
	if (nth_encoding_length == 0) return(NULL);
   return(sprite_from_encoding(nth_encoding,nth_encoding_length,version_number,language,remaining));
};

Sprite *sprite_from_encoding(string encoding, int encoding_length, int notebook_version, NaturalLanguage language, Pages *remaining) { 
   // abstracted on 201002
	StringInputStream encoding_stream(encoding,encoding_length);
//#if TT_DEBUG_ON
//	if (tt_debug_mode == 111099) {
//		tt_error_file() << "Encoding for page " << index << endl;
//		tt_error_file().write((character *) encoding,encoding_length);
//	};
//#endif
//	istrstream encoding_stream((char *) encoding,encoding_length);
// following was commented out -- why? -- necessary for remote appearances to work
	Picture *old_tt_copying_insides_for = tt_copying_insides_for;
	tt_copying_insides_for = NULL; // was controlee prior to 201002
	// on 280399 added following so that when a notebook is loading and calls this that there is no sharing
	// with the outer context of loading
	SpritePointer *saved_shared_items = shared_items;
	int saved_shared_items_count = shared_items_count;
	shared_items = NULL;
#if TT_DEBUG_ON
	if (tt_debug_mode == 1616) {
		tt_error_file() << "Saved " << saved_shared_items_count << " shared load items." << endl;
	};
   //if (index == 14) {
   //   log("debug this");
   //};
#endif
	// not top_level since part of this notebook
	Sprite *page = NULL;
   if (tt_want_exception_breaks) {	
      try {
         page = load_item(&encoding_stream,notebook_version,language,NULL,NULL); // was container,controlee); prior to 201002
      } catch (Sprite *) { // new on 140200 to catch my throws
         // do nothing
      };
   } else {
	   try { 
		   page = load_item(&encoding_stream,notebook_version,language,NULL,NULL); // was container,controlee); prior to 201002
      } catch (Sprite *) { // new on 140200 to catch my throws
         // do nothing
   	} catch (...) { // catch general problems (added 151299)
		   tt_error_file() << "Internal ToonTalk error re-creating page " << endl; // was index << endl;
         if (remaining != NULL) {
		      remaining->replace_encoding_and_length("",0); // so don't revist this problem
         };
		   if (!handle_internal_error(S(IDS_INTERNAL_ERROR_LOADING_PAGE,"Sorry but an internal error occurred while loading a page of a notebook. It is probably lost."),TRUE)) {
			   // user wants to quit
			   // there will be someone to catch this if breaks are off
			   throw(tt_global_picture); // new on 140200
         };
      };
	};
	release_shared_load_items(TRUE);
#if TT_DEBUG_ON
	if (tt_debug_mode == 1616) {
		tt_error_file() << "Released shared load items. Restoring " << saved_shared_items_count << " items." << endl;
	};
#endif
	shared_items = saved_shared_items;
	shared_items_count = saved_shared_items_count; // new on 300100
	tt_copying_insides_for = old_tt_copying_insides_for;
	return(page);
};

Page *Pages::nth_old_format_page(short int index, int version_number) { // new on 201002
	Pages *remaining = this;
	int i = index;
	while (i > 1) { // 1-indexing since that's what the page numbers seem (was != prior to 131200 but not too robust for bad calls)
		i--;
		remaining = remaining->rest_or_add_blank();
	};
	string nth_encoding;
	long nth_encoding_length;
	remaining->encoding_and_length(nth_encoding,nth_encoding_length);
#if TT_DEBUG_ON
   if (nth_encoding_length > 2000000) {
      log("Should this really be more than 2 million bytes long?");
	} else if (nth_encoding_length < 0) {
		log("Encoding length shouldn't be negative.",FALSE,TRUE);
   };
#endif
	if (nth_encoding_length <= 0) return(NULL); // changed from == to <= for robustness on 130603
   return(new OldFormatPage(copy_string(nth_encoding,nth_encoding_length),nth_encoding_length,version_number,language));
};

boolean Pages::nth_blank(short int index) {
	Pages *remaining = this;
	while (index != 1) {
		index--;
		remaining = remaining->rest_or_add_blank();
	};
	return(remaining->is_blank());
};

//short int Pages::first_page_with_notebook_matching(Sprite *item) {
//	Pages *remaining = this;
//	short int page_number = 1;
//	short int page_encoding_length;
//	bytes encoding_remaining;
//	while (remaining != NULL) {
//		remaining->encoding_and_length(encoding_remaining,page_encoding_length);
//		if (page_encoding_length > 0 && 
//			 ((SpriteType) encoding_remaining[0]) == PROGRAM_PAD) {
//			// optimize this another time
//			Notebook *subnotebook = (Notebook *) nth(page_number);
//			if (subnotebook->page_number_of_item(item) != -1) {
//				subnotebook->destroy();
//				return(page_number);
//			};
//			subnotebook->destroy(); // kind of wasteful...
//		};
		// just left pages
//		remaining = remaining->rest();
//		page_number++;
//	};
//	return(-1); // nothing found
//};

boolean check_page_match(unsigned char type, InputStream *page_encoding, string match_text, long match_text_length, int version_number) {
	if (type == PICTURE_WITH_INDIRECTION) { // ready to read indirection now
		if (version_number >= first_version_to_save_copied_nests) { // ignore picture's comment
			if (!ignore_next_item(page_encoding,version_number)) {
				return(FALSE); // new on 120101
			};
		};
		type = page_encoding->get();
		return(check_page_match(type,page_encoding,match_text,match_text_length,version_number));
	} else if (type == NEW_SHARED_ITEM) {
		unsigned short int ignore_index; 
      page_encoding->read((string) &ignore_index, sizeof(ignore_index));
		type = page_encoding->get();
		return(check_page_match(type,page_encoding,match_text,match_text_length,version_number));
	} else if (type == TEXT || type == VARIABLE_WIDTH_TEXT) {
		if (version_number >= first_version_to_save_copied_nests) {
			if (!ignore_next_item(page_encoding,version_number)) { // comment
				return(FALSE); // new on 120101
			};
		};
		unsigned short int text_length;
		page_encoding->read((string) &text_length, sizeof(text_length));
      char char1[2];
      char char2[2];
      char1[1] = NULL; // terminate
      char2[1] = NULL;
		int i = 0;
		int j = 0;
		string encoding = new character[text_length+1];
		page_encoding->read((string) encoding,text_length);
		for (; i < text_length && j < match_text_length;) {
//			while (encoding_remaining[i] == ' ' || encoding_remaining[i] == 10 || encoding_remaining[i] == '\r') { // skip over spaces, line feeds and carriage returns
			while (encoding[i] == ' ' || encoding[i] == 10 || new_line(encoding[i])) { // skip over spaces, line feeds and carriage returns (was encoding[i] == '\r' prior to 090802)
            i++;
			};
			while (match_text[j] == ' ' || match_text[j] == 10 || new_line(match_text[j])) { // skip over spaces, line feeds and carriage returns -- prior to 090802 was  == '\r'
            j++;
			};
			char1[0] = encoding[i];
         AnsiLower(char1);
         char2[0] = match_text[j];
         AnsiLower(char2);
			if (char1[0] != char2[0]) break;
			i++;
			j++;
//			if (!((char1[0] == char2[0]) ||
//					(encoding_remaining[i] == ' ' && match_text[i] == '\r') ||
//					(encoding_remaining[i] == '\r' && match_text[i] == ' '))) break;
		};
		delete [] encoding;
		if (j == match_text_length) { // all matched
			return(TRUE);
		};
	};
	return(FALSE);
};

short int Pages::first_page_matching_text(string match_text, long match_text_length, int version_number) {
	Pages *remaining = this;
	short int page_number = 1;
	long page_encoding_length;
	string encoding_remaining;
	while (remaining != NULL) {
		remaining->encoding_and_length(encoding_remaining,page_encoding_length);
		if (page_encoding_length > 0) {
			// re-wrote the following on 120500 to deal with indirections of pictures
			unsigned char type = encoding_remaining[0];
			if (type == PICTURE_WITH_INDIRECTION || type == TEXT || type == VARIABLE_WIDTH_TEXT || type == NEW_SHARED_ITEM) {
				StringInputStream page_encoding(encoding_remaining,page_encoding_length);
				page_encoding.get(); // skip over type "properly"
				if (check_page_match(type,&page_encoding,match_text,match_text_length,version_number)) {
					return(page_number);
				};
			};
		};
		// just left pages
		remaining = remaining->rest();
		page_number++;
	};
	return(-1); // nothing found
};

short int Pages::first_page_with_picture(int match_index, SpriteCode match_code, int version_number, string file_name) {
	// added file_name on 170900 - is NULL for TT pictures
	Pages *remaining = this;
	short int page_number = 1;
	long page_encoding_length;
	string encoding_remaining;
	while (remaining != NULL) {
		remaining->encoding_and_length(encoding_remaining,page_encoding_length);
//		if (page_encoding_length > 0 &&
//			 ((SpriteType) encoding_remaining[0]) == PICTURE) {
		// revised the above on 170900
		if (page_encoding_length > 0) { 
			unsigned char type = (SpriteType) encoding_remaining[0];
			if (type == NEW_SHARED_ITEM) {
				encoding_remaining += 1+sizeof(unsigned short int); // skip type and index
				type = (SpriteType) encoding_remaining[0];
			};
			if ((type == PICTURE && file_name == NULL) || (type == USER_PICTURE && file_name != NULL)) {
				encoding_remaining++; // skip over type
				StringInputStream page_encoding(encoding_remaining,page_encoding_length);
				if (version_number >= first_version_to_save_copied_nests) {
					if (!ignore_next_item(&page_encoding,version_number)) { // comment
						return(FALSE); // new on 120101
					};
				};
				short int image_index = 0;
				if (type == PICTURE) {
					page_encoding.read((string) &image_index,sizeof(image_index));
					if (image_index == match_index &&
						 page_encoding.get() == match_code) {
						return(page_number);
					};
				} else if (type == USER_PICTURE) {
					unsigned char file_name_length = (unsigned char) page_encoding.get();
					if (file_name_length == strlen(file_name)) {
						char buffer[MAX_PATH]; // was 256 prior to 280802
						page_encoding.read((string) buffer,file_name_length);
						if (strncmp(buffer,file_name,file_name_length) == 0) {
							return(page_number);
						};
					};
				};
			};
		};
		remaining = remaining->rest();
		page_number++;
	};
	return(0); // nothing found
};

Pages *Pages::skip_first_n(short int n) {
	Pages *remaining = this;
	while (n != 0 && remaining != NULL) {
		n--;
		remaining = remaining->rest();
	};
	return(remaining);
};

Pages *Pages::rest_or_add_blank() {
	if (next_page == NULL) {
		next_page = new Pages(NULL,0);
	};
	return(next_page);
};

int Pages::blank_page_number() {
	// if last 2 pages are not blank adds some
	Pages *remaining = this;
	Pages *next;
	int page_number = 1;
	while (TRUE) {
		next = remaining->rest();
		if (next == NULL || next->rest() == NULL) {
			if (!remaining->is_blank()) {
				remaining->rest_or_add_blank()->rest_or_add_blank();
				page_number += 2;
			};
//			if (page_number%2 == 0) page_number++;
			return(page_number+2);
		} else {
			if (next->is_blank() && remaining->is_blank()) {
				return(page_number);
			};
			remaining = next->rest();
			page_number += 2;
		};
	};
};

boolean Pages::all_pages_blank() {
   Pages *remaining = this;
   while (remaining != NULL) {
      if (!remaining->is_blank()) return(FALSE);
		remaining = remaining->rest();
	};
   return(TRUE);
};

void Pages::encoding_and_length(short int index, 
										  string &result_encoding, 
										  long &result_encoding_length) {
	Pages *remaining = this;
	while (index != 1) {
		index--;
		remaining = remaining->rest_or_add_blank();
	};
	remaining->encoding_and_length(result_encoding,result_encoding_length);
};
		
void Pages::set_page(short int index, string new_encoding, long new_encoding_length) {
	Pages *remaining = this;
	while (index != 1) {
		index--;
		remaining = remaining->rest_or_add_blank();
	};
	remaining->replace_encoding_and_length(new_encoding,new_encoding_length);
};

void Pages::replace_encoding_and_length(string new_encoding, long new_encoding_length) {
	if (encoding != NULL) delete [] encoding;
	encoding = new_encoding;
	encoding_length = new_encoding_length;
};

void Pages::dump(output_stream &stream, int max) {
	Pages *remaining = this;
	while (remaining != NULL) {
      if (max <= 0) break; // done
		remaining->dump_encoding_and_length(stream);
		remaining = remaining->rest();
      max--;
	};
};

void Pages::dump_encoding_and_length(output_stream &stream) {
//	unsigned short int short_encoding_length;
	string encoding_to_dump;
	long true_encoding_length; // different if notebook saved to file
	boolean delete_encoding = FALSE;
	if (tt_dump_reason == DUMPING_TO_CLIPBOARD && encoding_length > 0 &&
       encoding[0] == PROGRAM_PAD && encoding[2] != 0) { // is defined by a file name so dump that file's contents
		Sprite *notebook = nth(1); // this page
      if (notebook == NULL) return; // warn??
//		SpritePointer *saved_shared_items = shared_items; // new on 040702
//		int saved_shared_items_count = shared_items_count;
//		shared_items = NULL;
//#if TT_DEBUG_ON
//		if (tt_debug_mode == 1616) {
//			tt_error_file() << "Saved " << saved_shared_items_count << " shared load items." << endl;
//		};
//#endif
		encoding_to_dump = notebook->return_encoding(true_encoding_length,FALSE,FALSE);
		notebook->destroy(); // added 270200
//		short_encoding_length = (unsigned short int) encoding_length; 
		delete_encoding = TRUE;
//		release_shared_load_items(TRUE);
//#if TT_DEBUG_ON
//		if (tt_debug_mode == 1616) {
//			tt_error_file() << "Released shared load items. Restoring " << saved_shared_items_count << " items." << endl;
//		};
//#endif
//		shared_items = saved_shared_items;
//		shared_items_count = saved_shared_items_count;
	} else {
//		short_encoding_length = (unsigned short int) encoding_length;
		true_encoding_length = encoding_length;
		encoding_to_dump = encoding;
	};
	stream.write((character *) &true_encoding_length,sizeof(true_encoding_length));
	if (true_encoding_length > 0) {
		stream.write(encoding_to_dump,true_encoding_length); // 
	};
	if (delete_encoding) delete [] encoding_to_dump;
};

boolean Pages::equal_to(Pages *other) { // new on 060601
	string my_encoding, other_encoding;
	long my_length, other_length;
	encoding_and_length(my_encoding,my_length);
	other->encoding_and_length(other_encoding,other_length);
	if (my_length != other_length) return(FALSE);
	if (memcmp(my_encoding,other_encoding,my_length) != 0) return(FALSE);
	if (rest() != NULL) {
		if (other->rest() == NULL) return(FALSE);
		return(rest()->equal_to(other->rest()));
	} else if (other->rest() != NULL) {
		return(FALSE);
	};
	return(TRUE);
};

/*
Events *Notebook::pointer_to_body() {
	if (current_page == NULL) {
		return(NULL);
	} else {
		return(current_page->pointer_to_body());
	};
};
*/
/*
	boolean left = left_side(item);
	if (!left && right_page != NULL && 
		 right_page->kind_of() == PROGRAM_PAD && item->kind_of() == PROGRAM_PAD) {
		 string right_page_name = ((Notebook *) right_page)->pointer_to_file_name();
		 Notebook *new_right_page = (Notebook *) item;
		 string new_right_page_name = new_right_page->pointer_to_file_name();
		 if (right_page_name != NULL && new_right_page_name != NULL && 
			  strcmp(right_page_name,new_right_page_name) == 0) {
			 // returning a notebook back to container notebook
			 city_coordinate page_llx,page_lly;
			 right_page->lower_left_corner(page_llx,page_lly);
			 city_coordinate page_width = right_page->current_width();
			 city_coordinate page_height = right_page->current_height();
			 new_right_page->set_saved_size(return_saved_width(),return_saved_height());
			 remove_follower(right_page);
			 delete right_page;
			 right_page = new_right_page;
			 right_page->animate_size_and_location(page_width,page_height,page_llx,page_lly,500,
																this,add_right_page_as_follower_callback);
			 new_right_page->set_original(TRUE);
			 new_right_page->dump();
			 left_page_current = FALSE; // so right page is saved below
			 save_encoding(); // to save page number
			 return(RELEASED_ON_NOTEBOOK);
		 } else {
			 play_sound(PLOP_SOUND,5);
			 return(RELEASE_IGNORED);
		 };
	};
	if (!left && right_page != NULL && 
		 item->kind_of() != INTEGER && item->kind_of() != TEXT &&
		 (item->kind_of() != PICTURE ||
		  // or is a picture but has followers
		  item->pointer_to_followers() != NULL)) {
		 // not just page number or text string or picture to match left page
		return(RELEASE_IGNORED); // too easy to accidently redefine a page
	};
	if (left && left_page == NULL) {
		if (floor->inside_rule_body()) return(RELEASE_IGNORED); // for now
		switch (item->kind_of()) {
			case PICTURE:
				if (item->pointer_to_followers() != NULL) {
					return(RELEASE_IGNORED);
				}; // otherwise do the following
			case TEXT:
				set_page(item,TRUE,by);
				return(RELEASED_ON_NOTEBOOK);				
			default:
			  return(RELEASE_IGNORED); // only text or pictures on the left
		};
	};
	if ((left_page != NULL && left) || (right_page != NULL && !left)) {
		// item may indicate page to go to
		// page must be non-blank
		switch (item->kind_of()) {	 
			case TEXT: 
			case PICTURE: 
			case INTEGER:
				saved_page_number = page_number_of_item(item);
				break;
			default:
				return(RELEASE_IGNORED);
		};
		if (saved_page_number > 0 && saved_page_number <= pages->length()+2) {
			millisecond duration = default_duration(750);
			page_number_item = item;
			city_coordinate delta_x;
			if (saved_page_number&1) { // odd so number is on left side
				delta_x = width/8;
			} else { 
				delta_x = (5*width)/8;
			};
			item->animate_size_and_location(3*width/8,height/4,
													  llx+delta_x,lly,
													  duration,this,go_to_items_page_callback);
			if (item->kind_of() == INTEGER) {
				user_did(PROGRAM_PAD,NOTEBOOK_RECEIVED_PAGE_NUMBER,by);
			} else {
				user_did(PROGRAM_PAD,NOTEBOOK_RECEIVED_PAGE_TOKEN,by);
			};
			return(RELEASED_ON_NOTEBOOK);
		} else {
			play_sound(PLOP_SOUND);
			return(RELEASE_IGNORED);
		};
	};
	if (floor->inside_rule_body()) return(RELEASE_IGNORED); // for now
	switch (item->kind_of()) {
		case ROBOT:
			tt_suspensions->remove_if_robot_is((Robot *) item);
			flush_cache();
			robot_saved();
			break;
		case PROGRAM_PAD: {
			if (pointer_to_toolbox_notebook() == this) { //strcmp(file_name,"notebook.tt") == 0) {
				// no sub-directory support yet
				((Notebook *) item)->new_file_name(right_page_number());
				flush_cache();
			} else {
				play_sound(PLOP_SOUND);
				return(RELEASE_IGNORED);
			};
			};
			break;   
	};
	set_page(item,FALSE,by);
	if (right_page->kind_of() == PROGRAM_PAD) {
		((Notebook *) right_page)->dump();
	};
	// following canonicalizes the appearance of page items
//	int saved_page_number = right_page_number();
//	go_to_a_blank_page();
//	go_to_page(saved_page_number);
	return(RELEASED_ON_NOTEBOOK);
};
*/
