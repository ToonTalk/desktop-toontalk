// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif
#if !defined(__TT_CLEANUP_H)
#include "cleanup.h"
#endif
#if !defined(__TT_LOG_H)
#include "log.h"
#endif
#if !defined(__TT_PUZZLE_H)
#include "puzzle.h"
#endif
//#include "icache.h"
#if !defined(__TT_DIRTY_H)   
#include "dirty.h"
#endif 
#if !defined(__TT_PRGRMMR_H)   
#include "prgrmmr.h"
#endif   
#if !defined(__TT_BCKGRND_H)
#include "bckgrnd.h"
#endif
#if !defined(__TT_CITY_H)
#include "city.h"
#endif
#if !defined(__TT_MAIN_H)
#include "main.h"
#endif
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif   
#if !TT_WINDOWS
#include "sound.h"
#endif
#if !defined(__TT_HOUSE_H)   
#include "house.h"
#endif   
#if !defined(__TT_USER_H)   
#include "user.h"
#endif   
#if !defined(__TT_STRING_H)   
#include "string.h"
#endif   
#if !defined(__TT_EVENT_H)   
#include "event.h"
#endif   
#if !defined(__TT_NUMBER_H)   
#include "number.h"
#endif   
#if !defined(__TT_CUBBY_H)   
#include "cubby.h"
#endif   
#if !defined(__TT_BIRD_H)   
#include "bird.h"
#endif   
#if !defined(__TT_BOMB_H)   
#include "bomb.h"
#endif   
#if !defined(__TT_TEXT_H)   
#include "text.h"
#endif   
#if !defined(__TT_THOUGHT_H)   
#include "thought.h"
#endif   
#if !defined(__TT_ROBOT_H)   
#include "robot.h"
#endif   
#if !defined(__TT_TRUCK_H)   
#include "truck.h"
#endif   
#if !defined(__TT_PAD_H)   
#include "pad.h"
#endif   
//#if !defined(__TT_HYPERBOT_H)
//#include "hyperbot.h"
//#endif   
#if !defined(__TT_FLOOR_H)   
#include "floor.h"
#endif   
#if !defined(__TT_MARTIAN_H)
#include "martian.h"
#endif
#if !defined(__TT_REMOTE_H)
#include "remote.h"
#endif
#if TT_32
#if !defined(__TT_SPEAK_H)
#include "speak.h"
#endif
#endif  
#if TT_DIRECT_PLAY
#if !defined(__TT_NETWORK_H)
#include "network.h"
#endif
#endif
#if !defined(__TT_MOUSE_H)
#include "mouse.h"
#endif
#if !defined(__TT_ZIP_H)
#include "zip.h"
#endif
#if TT_DIRECT_INPUT
#if !defined(__TT_INPUT_H)
#include "input.h"
#endif
#endif
#if TT_XML
#if !defined(__TT_XML_H)
#include "xml.h"
#endif
#endif
#include "strings.h" 
#if FAST_GRAPH
#include <fastgraf.h>
#elif FLASH_GRAPHICS
#include <fg.h>
#elif TT_WINDOWS
#include <windows.h>
#if !defined(__TT_WINMAIN_H)   
#include "winmain.h"
#endif   
#endif

#if !defined(__TT_HASH_H)
#include "hash.h"
#endif    

#include <stdlib.h>
//#if SMART_HEAP||HEAP_AGENT
//#include <heapagnt.h>
//#endif
// by breaking this out of utils.cpp it can be used by other
// programs like disp_pr.cpp

void tt_exit(int code, string reason, string data) {
//#if TT_WINDOWS
	tt_error_file() << reason << data << " " << code <<endl;
// called in tt_exit_failure:
//	play_sound(SYSTEM_ERROR_SOUND);
   unload_string_library();
	if (tt_main_window != NULL && has_initialization_completed()) { // second conjunct added 090600
		SendMessage(tt_main_window->get_handle(),WM_DESTROY,NULL,NULL);
	} else {
		exit(EXIT_FAILURE);
	};
//#else
//	cout << reason << data << endl;
//	cleanup_for_exit();
//	copyright_notice();
//	exit(code);
//#endif
};

boolean exit_failure_in_progress = FALSE; // new on 280201 to avoid this being called repeatedly

void tt_exit_failure(Trouble trouble, const_string reason, const_string reason2) {
	if (exit_failure_in_progress) { // new on 280201
		tt_shutting_down = TRUE; // just in case - new on 101202
		tt_exit(EXIT_FAILURE,"Error while terminating. ",reason);
		return; // since the above might have sent a shutdown message
	};
	exit_failure_in_progress = TRUE;
   write_ini_entry("Switches",AC(IDC_TOONTALK_STARTED),"0"); // not that abnormal
	tt_trouble_shooting = TRUE; // show following dialog regardless -- really a good idea??
	if (trouble != TROUBLE_SHOOT_TRIAL_PERIOD_OVER && reason != NULL) tt_err_file_important = TRUE;
	if (tt_main_window != NULL) {
		tt_error_file() << "Abnormal termination: ";
      if (reason != NULL) {
         tt_error_file() << reason;
      };
		if (reason2 != NULL) {
         tt_error_file() << " " << reason2 << endl;
      } else {
			tt_error_file() << endl;
		};
      HDC screen_device_context = GetDC(NULL);
      SetSystemPaletteUse(screen_device_context,SYSPAL_STATIC); // restore palette
      ReleaseDC(NULL,screen_device_context);
	   pause_toontalk(FALSE,FALSE,TRUE); // TRUE added on 191199 so when in trouble doesn't trigger more bugs and crash while exiting
      if (!about_to_quit()) { // conditional added on 150200 since if problem occurred while quiting don't bother user
			if (trouble == DONT_TROUBLE_SHOOT) {
				if (reason2 == NULL) {
					MessageBox(NULL,reason,_T("ToonTalk"),MB_OK|MB_ICONSTOP|MB_SYSTEMMODAL);
				} else {
					if (reason != NULL) {
						MessageBox(NULL,reason,_T("ToonTalk"),MB_OK);
						MessageBox(NULL,reason2,_T("ToonTalk"),MB_OK|MB_ICONSTOP|MB_SYSTEMMODAL);
					};
				};
			} else {
				trouble_shoot(trouble,reason,reason2,FALSE,TRUE);
			};
		};
		if (has_initialization_completed()) {
			SendMessage(tt_main_window->get_handle(),WM_DESTROY,NULL,NULL);
		} else { // new on 090600
			tt_exit(EXIT_FAILURE,_T(""),_T(""));
		};
	} else {
      if (trouble != DONT_TROUBLE_SHOOT) trouble_shoot(trouble,reason,reason2,FALSE,TRUE);
	   if (reason != NULL) MessageBox(NULL,reason,_T("ToonTalk"),MB_OK|MB_ICONSTOP|MB_SYSTEMMODAL);
		tt_exit(EXIT_FAILURE,_T(""),_T(""));
	};
};

void cleanup_for_exit() {
	if (!tt_exclusive_screen) {
	  wait_cursor(); // new on 091004 since can take some time to zip up archives, save city if time travelling, and restore video mode
	};
#if TT_DEBUG_ON
	tt_error_file() << (tt_counter_for_debugging-tt_deleted_sprite_count) << " unclaimed sprites before cleanup." << endl;
	tt_error_file() << (tt_number_value_count-tt_number_value_destroyed_count) << " unclaimed number values before cleanup." << endl;
#endif
	tt_resetting_or_destroying_city = TRUE; // new on 020504 to prevent spurious warnings
	if (tt_puzzles_over_html_file != NULL) { // new on 180700 so if you quit after last puzzle you see HTML file
		// moved here on 050401 from Programmer_Walking::~Programmer_Walking() so this happens before too much has been clobbered
		show_puzzles_over_html();
	};
	if (!tt_debugging_leaks) { // new on 180103
		tt_reclaiming_memory = FALSE; // no need to slow down the process of quiting unless debugging leaks
	};
#if TT_TEXT_TO_SPEECH
   stop_speaking();
#endif
  stop_sound(TRUE);
#if TT_DIRECT_INPUT
  stop_force_feedback_effect(); // new on 110602
#endif
  tt_shutting_down = TRUE; // moved here on 190103
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
  if (tt_reclaiming_memory) { // conditional new on 21104 since it is possible for the following to take a long time
	  release_postponed_collision_checks(); // moved here on 261000 so this happens before setting tt_shutting_down -- probably doesn't matter anymore (180103)
  };
  log_finalization();
  dump_history(); // includes tt_parameter values...
//  if (replaying()) // commented this out on 061103 since should close any log regardless 
  close_input_log(TRUE,FALSE);
#if TT_DIRECTX
  if (tt_exclusive_screen) {
	  restore_display_mode();
	  wait_cursor(); // new on 091004 since can take some time to zip up archives, save city if time travelling, and restore video mode
	  Sleep(250); // sleeping a bit helps problem with exiting true colors -- maybe takes time to restore display and need to do that before releasing direct draw.
  };
#endif
  tt_suspensions->delete_all();
//  if (tt_backgrounds_still_animating != NULL) {
//	  delete tt_backgrounds_still_animating;
//  };
  if (tt_deleted_houses != NULL) {
	  delete tt_deleted_houses;
  };
//  delete tt_integer_stack;
//  delete tt_text_stack;
//  delete tt_cubby_stack;
//  delete tt_nest_stack;
//  delete tt_bomb_stack;
//  delete tt_thought_bubble_stack;
//  delete tt_robot_stack;
//  delete tt_truck_stack;
  if (tt_martian != NULL && tt_reclaiming_memory) { // added tt_reclaiming_memory on 230105
//	  tt_martian->remove_from_floor(FALSE); // commented out on 230105 since possible for floor to be trash and cause crash on exit
//	  Background *background = tt_martian->pointer_to_background();
//	  if (background != NULL) {// && background->has_item(tt_martian)) {
//		  background->remove_item(tt_martian,FALSE);
//	  };
	  tt_martian->destroy();
	  tt_martian = NULL;
  };
  tt_global_picture->destroy();
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
  if (tt_temp_picture != NULL) tt_temp_picture->destroy();
//#if TT_DIRECT_INPUT // commented out on 210901 since allocated even if not running with TT_DIRECT_INPUT (for compatibility with those versions that do)
  if (tt_joystick_pictures != NULL) {
     for (int i = 0; i < max_number_of_joysticks; i++) { // fixed leak on 270200 since was tt_joystick_count instead of max_number_of_joysticks
         tt_joystick_pictures[i]->destroy();
     };
     delete [] tt_joystick_pictures;
  };
//#endif
//  if (tt_system_mode == PUZZLE)  // can allocate by reading and saving puzzle state
  release_puzzle_data(TRUE);
  release_subtitles();
  release_time_travel_buttons();
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
//  release_previous_mouse(); // new on 060701
/*
  Sprites *remaining = tt_notebooks;
  Floor *floor;
  while (remaining) {
	  Notebook *pad = (Notebook *) remaining->first();
	  floor = pad->pointer_to_floor();
	  if (floor != NULL) floor->remove_item(pad,FALSE);
	  remaining = remaining->rest();
  };
  delete tt_notebooks;
  tt_notebooks = NULL;  // since floors refer to this when shutting down
*/
  //if (tt_events != NULL) { // moved on 151203
	 // delete tt_events;
  //};
//  if (tt_body_working_set != NULL) {
	  // items themselves handled elsewhere
//     tt_body_working_set->remove_all();
//     delete tt_body_working_set;
//  };
#if SOUND_CAPABLE
  close_sound();
#endif
#if FAST_GRAPH
  fg_kbinit(0);
  fg_freepage(tt_background_cache_page);
  fg_freepage(tt_screen_cache_page);
  fg_mousefin();
  fg_setmode(old_mode);
  fg_reset();
  fg_setnum(tt_numlock_original);
#endif
#if FLASH_GRAPHICS
  fg_term();
#endif
#if GRAPHICS_TO_MEMORY&&TT_WINDOWS
#if !TT_32
	close_serial_port();
#endif
#if !WING&&!TT_DIRECTX
	GlobalUnlock(tt_working_page_handle);
	GlobalFree(tt_working_page_handle);
#endif
	GlobalUnlock(tt_background_page_handle);
	GlobalFree(tt_background_page_handle);
// since it is statically allocated now
//  if (tt_work_dib != NULL) {
//	  delete tt_work_dib;
//  };
#else 
 #if !WING
  if (tt_memory_graphics_work_page != NULL) {
	  delete [] tt_memory_graphics_work_page;
  };
 #endif
#endif
#if TT_DEBUG_ON
  add_to_log_if_interesting_error();
#endif
  flush_input_events();
  release_page_grid();
  release_sound_cache(); // added on 041199
  release_sprite_data();
  release_keyboard_buffer();
  if (tt_user_parameters != NULL) {
	  delete tt_user_parameters;
  };
  if (tt_histories != NULL) delete [] tt_histories;
//  release_house_sprites();
// re-ordered the following so floor still exists
// when tools are deleted
// was commented out the following since should go with floor
#if TT_DIRECT_INPUT
  release_force_parameter_controlees(); // do this early since will delete controlees while iforce lib still loaded
#endif
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
  if (tt_reclaiming_memory) { // new on 040301 to speed up shut down since this memory is reclaimed anyway
     // much of this moved here on 091002
      if (tt_toolbox != NULL && !tt_toolbox->gone()) { 
	      // not so early that no room entered or vacuumed
      //	  Background *floor = tt_toolbox->pointer_to_background();
      //	  if (floor != NULL) floor->remove_item(tt_toolbox,FALSE,FALSE);
	      if (tt_toolbox->pointer_to_leader() != NULL) { 
		      // e.g. expanding thought bubble
		      tt_toolbox->pointer_to_leader()->remove_follower(tt_toolbox);
	      };
	      tt_toolbox->remove_from_floor(FALSE,FALSE); // as of 070105 no-op if removed follower above
	      tt_toolbox->destroy();
	      tt_toolbox = NULL;
			release_toolbox_tools();
      };
      tt_city->prepare_for_deletion();
      delete tt_city;
      delete tt_screen;
      delete tt_programmer;
      if (tt_next_frames_deleted_sprites != NULL) {
			if (tt_deleted_sprites == NULL) {
				tt_deleted_sprites = tt_next_frames_deleted_sprites;
			} else {
				tt_deleted_sprites->last_cons()->set_rest(tt_next_frames_deleted_sprites);
			};
//	  tt_next_frames_deleted_sprites->delete_all();
//	  delete tt_next_frames_deleted_sprites;
		};
		// do these last since some of the above may add to it
		while (tt_deleted_sprites != NULL) {
			// deleting might add new ones
			Sprites *temp = tt_deleted_sprites;
			tt_deleted_sprites = NULL;
			if (temp != NULL) {
#if TT_DEBUG_ON
				DWORD start;
				int count;
				if (tt_debug_mode == 40301) {
					start = timeGetTime();
					count = temp->length();
				};
#endif
     			temp->delete_all();
	  			delete temp;
#if TT_DEBUG_ON
				if (tt_debug_mode == 40301) {
					tt_error_file() << (timeGetTime()-start) << "ms to delete " << count << " items." << endl;
				};
#endif
			};
		};
		if (tt_deleted_lists != NULL) {
#if TT_DEBUG_ON
			DWORD start;
			if (tt_debug_mode == 40301) {
				start = timeGetTime();
			};
#endif
			tt_deleted_lists->delete_all();
#if TT_DEBUG_ON
			if (tt_debug_mode == 40301) {
				tt_error_file() << (timeGetTime()-start) << "ms to delete lists." << endl;
			};
#endif
//	   delete tt_deleted_lists;
			tt_deleted_lists = NULL;
		};
  };
  release_queue_and_robots(); // prior to 210901 was part of conditional above (presumably by mistake)
#if TT_DIRECT_PLAY
  just_destroy_list(tt_networked_nests); // since are destroyed ignoring ref count if shutting down
#endif
#if TT_DEBUG_ON
  add_to_log_if_interesting_error();
#endif
  release_next_speak();
//  release_previous_speak(); // new on 130700 -- commented out on 190301 since triggers an error I couldn't track down
  delete [] tt_user_name;
  if (tt_log_out_file_name != NULL) delete [] tt_log_out_file_name;
//  flush_input_events(); // commented out on 160103 since called twice
#if !TT_32
  free_lego_state();
#endif
//  string StartTTName = ini_entry("Executables","StartToonTalk");
//  if (StartTTName != NULL) {
     if (tt_temp_files != NULL && FindWindowG("StartTT",NULL) == NULL) { // otherwise StartTT will take care of it
	     for (int i = 0; i < tt_temp_files_count; i++) {
		     OFSTRUCT file_struct;
		     OpenFile(tt_temp_files[i],&file_struct,OF_DELETE);
        };
	  };
//     delete [] StartTTName;
//  };
  if (tt_temp_files != NULL) delete [] tt_temp_files;
  ascii_string log_file_name = replay_file_name();
  if (log_file_name != NULL) {
	  delete [] log_file_name;
  };
  if (tt_favorites != NULL) delete [] tt_favorites;
  release_user_image_table();
  release_user_sound_table();
  release_speak_starts(); // new on 170103
  release_sprites_being_dumped(); // new on 170103
  release_sprite_table(); // new on 140703
//#if !TT_TEXT_TO_SPEECH
  // Win95 recovers space anyways and this seems to trigger some odd bug
//  if (tt_debug_mode == 6666) { // condition added on 270301
	  free_memory_cache(); // moved here on 260301
//  };
//#endif
#if TT_DIRECTX
  release_direct_draw();
#if TT_DIRECT_PLAY
  release_direct_play();
#endif
#if TT_TEXT_TO_SPEECH
  release_text_to_speech(); // this releases OLE which direct_play relies upon so moved here on 270400
#endif
  release_direct_sound(); // starting 050500 release this after text-to-speech to avoid problem with some engines
#endif
  release_alphabet();
  if (tt_file_name_while_time_travelling != NULL) delete [] tt_file_name_while_time_travelling;
  if (tt_default_file_name != NULL) delete [] tt_default_file_name;
//  if (tt_file_name_read_only != NULL) delete [] tt_file_name_read_only;
  if (tt_shared_files_directory != NULL) delete [] tt_shared_files_directory; // new on 030402
  if (tt_default_user_directory != NULL) delete [] tt_default_user_directory; // new on 060402
  if (tt_not_city_stopped_callbacks != NULL) {
     delete tt_not_city_stopped_callbacks;
     tt_not_city_stopped_callbacks = NULL;
  };
#if TT_DEBUG_ON
  add_to_log_if_interesting_error();
#endif
  release_directory_names();
  if (tt_puzzle_file_name != NULL) delete [] tt_puzzle_file_name;
  release_shared_load_items();
//  release_saved_deltas(); // new on 251000 // commented out on 061102 since part of Programmer now
//  release_current_notebook_file_name(); // new on 251000
  release_subtitles_file_name(); // new on 271000
#if !TT_STRINGS_STATIC
  unload_string_library();
#endif
  if (tt_fixed_font_name != NULL) delete [] tt_fixed_font_name;
  if (tt_variable_font_name != NULL) delete [] tt_variable_font_name;
  release_keyboard_accelerators();
  release_file_search_path();
  if (tt_default_user_image != NULL) {
	  delete tt_default_user_image;
  };
  if (tt_local_ini_file_uncached_name != NULL) {
	  delete [] tt_local_ini_file_uncached_name;
  };
  if (tt_local_ini_file_loaded_with_city != NULL) {
	  delete [] tt_local_ini_file_loaded_with_city;
  };
  if (tt_trouble_shoot_java_applet_file_name != NULL) { // new on 240601
	  delete [] tt_trouble_shoot_java_applet_file_name;
  };
  	if (tt_missing_picture_file_extension != NULL) { // this was part of release_queue_and_robots (why??) prior to 130703
		delete [] tt_missing_picture_file_extension;
		tt_missing_picture_file_extension = NULL; // new on 170403
	};
  // following new on 160103
  if (tt_ok_to_generate_mini_dump_message != NULL) delete [] tt_ok_to_generate_mini_dump_message;
  if (tt_mini_dump_email != NULL) delete [] tt_mini_dump_email;
  if (tt_missing_builtin_picture_file_extension != NULL) delete [] tt_missing_builtin_picture_file_extension;
  release_non_networked_nests_with_guid(); // new on 170103 -- restored on 140104 (even though it is also called during reset)
#if TT_XML
  release_xml_tag_table();
  release_numbered_sprites(); // new and replaces the following
//  reset_serial_numbering();
  release_saved_nest_guids();
  release_postponed_nest_xml();
  release_postponed_include_file_xml();
  //  reset_element_sprite_table(); -- replaced by the following
  release_current_element_sprite_table(); // new on 190103
#endif
  release_hash(); // new on 170303
  release_included_sprites();
  release_included_elements();
  release_zip_description_archive_element();
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
  tt_main_window->shut_down();
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
  deallocate_saved_heads(); // new on 130104
  release_last_yes_no(); // new on 130104
  release_file_name_of_last_city_loaded(); // new on 140104
  	if (tt_file_name_clipboard_message != NULL) { // new on 130204
		delete [] tt_file_name_clipboard_message;
	};
	if (tt_function_key_help != NULL) { // new on 060305
		tt_function_key_help->destroy();
		tt_function_key_help = NULL;
	};
#if TT_CALL_FOR_MARTY
	remove_call_for_marty(TRUE);
#endif
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	release_log_file_names();
	release_rational_number_for_string_testing();
	release_screen_patch_buffer();
	release_error_string_buffer();
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
};

void release_queue_and_robots() {
#if TT_DEBUG_ON
	DWORD start;
	int count;
	if (tt_debug_mode == 40301) {
		start = timeGetTime();
		if (tt_running_robots == NULL) {
			count = 0;
		} else {
			count = tt_running_robots->length();
		};
	};
#endif
   if (tt_queue != NULL) {
	   delete tt_queue;
		tt_queue = NULL;
   };
//	destroy_sprites(tt_running_robots); // ref counted so typically won't be destroyed here - new on 190100
	// commented out above on 090400 since causes robots to not be deleted and to run (as orphans)
	// and when robot is prepared for deletion it removes itself from tt_running_robot
	// and restored following
   if (tt_running_robots != NULL) {
	   tt_running_robots->remove_all();
	   delete tt_running_robots;
		tt_running_robots = NULL;
   };
#if TT_DEBUG_ON
	if (tt_debug_mode == 40301) {
		tt_error_file() << (timeGetTime()-start) << "ms to delete " << count << " running robots and the queue." << endl;
	};
#endif
};

void reset_toontalk() { // abstracted on 140603
	// much of the following should be abstracted and shared with similar code in log.cpp
	release_postponed_collision_checks(); // new on 110603
	release_non_networked_nests_with_guid(); // new on 110603
//	just_destroy_list(tt_networked_nests); // new on 110603 - is this necessary??
	release_queue_and_robots(); // moved here on 110603
	if (tt_martian != NULL) { // new on 160603 - beginning 110603 just set it to NULL here
		tt_martian->destroy();
//		tt_martian = new Martian();
	};
	if (tt_global_picture != NULL) {
//		tt_global_picture->destroy(); // new on 051199
		// replaced the above with the following on 171203 since there can be dangling references (e.g. saved puzzle state)
		tt_global_picture->re_initialize();
//		tt_global_picture = new GlobalPicture(); // new on 051199
//		tt_global_picture->become_blank(); // copied here on 011002 since used as well as typical blank picture
	};
	reset_events_counters(); // new on 120603
//	tt_programmer = NULL; // new on 110603
	if (tt_suspensions != NULL) { // new on 140603
		tt_suspensions->delete_all();
//		tt_suspensions = new Suspensions();
	};
	if (tt_toolbox != NULL) { 
		// replaced the code that destroyed the toolbox and recreated a new one with the following on 050204 - and undid this on 100204
		// was causing mysterious bugs and this only happens if not a successive log so the cost isn't an issue
//		tt_toolbox->toontalk_reset();
		tt_toolbox->destroy();
		tt_toolbox = NULL; // new on 100204
		// tried to comment this out again and restored on 050204 since Toolbox doesn't save its state completely -- e.g. offstage_flag
		// offstage_flag is now saved as of 060504 but maybe there are other variables that aren't saved
//		tt_toolbox = new Toolbox(); // commented out on 100204
		// commented out again on 10204 since too early to initialize while other things are being reset
//		tt_toolbox->initialize(); // new on 150603 -- on 240803 tried to comment this out but then need to fix other things so Tooly's friends are connected correctly
	};
	release_toolbox_tools(); // new on 080704 -- moved out of conditional on 200105 since loading a city can now be aborted
	reset_included_sprites(); // new on 110703 - this is kind of wasteful since most of it is useful
	reset_included_elements(); // new on 160703
	if (tt_programmer->return_next_status() != START_AFTER_LOADING_XML) { // not if initializing after titles
		delete tt_programmer; // new on 120703
		tt_programmer = NULL;
//		tt_programmer = new Programmer();
	};
	if (tt_events != NULL) { // copied here on 151203 -- since otherwise going back in time to train a robot can get confused
	   delete tt_events;
		tt_events = NULL; // new on 151203
   };
	if (tt_deleted_houses != NULL) {
	   delete_deleted_houses(); // new on 290204 so time travel jumping works with houses being destroyed
   };
	destroy_sprites(tt_pictures_to_activate); // new on 220704
//	release_sprite_table(); // this will happen as sprites are destroyed but networked nests may refuse to die so connections aren't recreated during time travel
//	deallocate_saved_heads(); // new on 180603 -- experiment - maybe a leak
//	initialize_head_images();
	// more??
};

void recreate_after_reset() { // new on 131003 - reconstructs stuff deleted above
	// prior to 131003 this was interleaved but caused problems, e.g. remotes of remote controls include trash
	UnwindProtect<boolean> set(tt_loading_is_ok_to_abort,FALSE); // since not enough is re-initialized yet -- new on 240105
	tt_martian = new Martian();
//	tt_global_picture = new GlobalPicture(); // commented out on 171203 since re_initialized now
//	tt_global_picture->become_blank(); // copied here on 011002 since used as well as typical blank picture
	tt_suspensions = new Suspensions();
	if (tt_toolbox == NULL) { // condition new on 040204 -- no point clobbering it otherwise
		tt_toolbox = new Toolbox();
		tt_toolbox->initialize(); // new on 150603 -- on 240803 tried to comment this out but then need to fix other things so Tooly's friends are connected correctly
	};
	if (tt_programmer == NULL) {
		tt_programmer = new Programmer();
	};
};



  
