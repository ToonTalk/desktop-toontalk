// Copyright (c) 1992-2007.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif
#if !defined(__TT_ZIP_H)   
#include "zip.h"
#endif
#if !defined(__TT_UTILS_H)
#include "utils.h"
#endif   
#if !defined(__TT_CLEANUP_H)   
#include "cleanup.h"
#endif
#if !defined(__TT_MAIN_H)
#include "main.h"
#endif   
#if !defined(__TT_DIRTY_H)   
#include "dirty.h"
#endif
#if !defined(__TT_SCREEN_H)   
#include "screen.h"
#endif	
#if !defined(__TT_LOG_H)   
#include "log.h"
#endif
#if !defined(__TT_HOUSE_H)   
#include "house.h"
#endif
#if !defined(__TT_CITY_H)
#include "city.h"
#endif
#if !defined(__TT_PAD_H)
#include "pad.h"
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
#if !defined(__TT_ROBOT_H)
#include "robot.h"
#endif
#if !defined(__TT_TRUCK_H)
#include "truck.h"
#endif
#if !defined(__TT_HELP_H)
#include "help.h"
#endif
#if !defined(__TT_STRING_H)   
#include "string.h"
#endif
#if !defined(__TT_PUZZLE_H)
#include "puzzle.h"
#endif
#if !defined(__TT_BIRD_H)
#include "bird.h"
#endif
#if !defined(__TT_MARTIAN_H)
#include "martian.h"
#endif
#if TT_WINDOWS
#ifndef _INC_WINDOWS
#include <windows.h>
#endif
#if !defined(__TT_WINMAIN_H)   
#include "winmain.h"
#endif
#if !defined(__TT_FLOOR_H)   
#include "floor.h"
#endif
#if !defined(__TT_FILE_H)   
#include "file.h"
#endif
#if TT_XML
#if !defined(__TT_XML_H)
#include "xml.h"
#endif
#endif
#if !defined(__TT_COMMON_H)
#include "common.h"
#endif
#if !defined(__TT_CLEANUP_H)
#include "cleanup.h"
#endif
#if GRAPHICS_TO_MEMORY
#if !TT_32
#include "dispdib.h"
#endif   
#endif
#else
#include "sound.h"
#include <dos.h>
#endif 

#if TT_ENHANCED_DRAG_AND_DROP
#if !defined(__TT_DRAGDROP_H)
#include "dragdrop.h"
#endif
#endif

#if HIGH_RESOLUTION_CLOCK
#include "types.h"
#include "htimer.h"
#else
#include <time.h>
#endif
//#include <fstream.h>
////#include <strstrea.h> commented out 210703 // commented out on 210703
#if FAST_GRAPH
#include <fastgraf.h>
#endif
#if FLASH_GRAPHICS
#include <fg.h>
#endif
#include <mmsystem.h>   
#include <stdlib.h>
#include <stdio.h>
//#include <io.h> // to load file into a string (should find out modern way)
#if SMART_HEAP||HEAP_AGENT
#include <heapagnt.h>
#endif

#if !TT_32
#include <shellapi.h>
#include <ctype.h>  // for isspace
#endif

#if TT_DIRECTX
//#include <dsound.h>
//#include "d:\dxsdk\include\dsound.h"
#include "dsutil.h" 
//#include "wave.h" -- commented out on 291002 since not used anymore
#endif

#include <shlobj.h>
//#include <exdisp.h>
//#include <shlguid.h>
//#include <comdef.h>

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// new on August 19 '98
#define TT_BASE_64_ENCODING 1

int const number_of_video_modes = 7; 
//int const video_modes[number_of_video_modes] = {20,21,22,23,25,28,36}; // old names
int const screen_width[number_of_video_modes] =  {320,640,800,1024,1152,1280,1600};
int const screen_height[number_of_video_modes] = {240,480,600, 768, 864,1024,1200};

//coordinate tt_screen_width, tt_screen_height,
//           tt_screen_half_width, tt_screen_half_height;

int tt_ideal_units_per_horizontal_pixel,
    tt_ideal_units_per_vertical_pixel;
//	 tt_half_ideal_units_per_horizontal_pixel,
//    tt_half_ideal_units_per_vertical_pixel;

city_coordinate tt_character_height;

int tt_background_cache_page = 2;
int tt_screen_cache_page = 3; // background and stuff on top

flag sound_flag = TRUE;

int current_sound_priority = 0;
int id_of_sound_to_play = 0;
boolean repeating_current_sound = FALSE;
#if TT_DIRECTX
sound_buffer current_sound_buffer = NULL;
#endif

string ZippedInputStream::read_string(string s) {
	long length;
	read_current_file_in_unzip_archive(archive,(bytes) &length, sizeof(length));
	if (length == 0) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299) {
			tt_error_file() << "Just read a string of zero length" << endl; // - file position is " << (int) in.tellg() << endl;
		};
#endif
		return(NULL); // new on 290999
	};
#if TT_DEBUG_ON
	if (length > 1000000 || length < 0) {
#if TT_DEBUG_ON
		debug_this();
#endif
		tt_error_file() << "String is more than 1,000,000 bytes long: " << length << endl;
	};
#endif
	if (s == NULL) {
		s = new char[length];
	};
	read_current_file_in_unzip_archive(archive,s,length);
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299) {
		tt_error_file() << "Just read a string '" << s << endl; // "' - file position is " << (int) in.tellg() << endl;
	};
#endif
	return(s);
};


//#if TT_DIRECTX
//DeletedMemorySoundBuffer *sound_to_delete = NULL;
//#else
DeletedMemory *sound_to_delete = NULL;
//#endif

unsigned char video_mode_index(int video_mode) {
	for (char i = 0; i < number_of_video_modes; i++) { 
      // not commented out on 300702 as obsolete so that demos created here can run in somewhat older versions
	  if (screen_width[i] == video_mode) {
		  return((char) i);
	  };
   };
	//return(-1); // error
   return(video_mode/32); // new on 300702 to map mode to a byte (for legacy reasons). Accomplished by dividing by 32.
   // note that resolutions greater than or equal to 8192 should still work but they won't log correctly or remember correctly for that user.
};

int video_mode_number(int index) {
   // commented out on 300702 - might break some really old demos or user profiles??
//	if (index == 22) return(320); // obsolete video mode name
//	if (index == 25) return(640);
   if (index > 7) return(index*32); // new on 300702
	return(screen_width[index]);
};

boolean tt_set_screen_parameters(int video_mode) {
//   int index = video_mode_index(video_mode);
//   if (index >= 0) {
//		tt_screen_height = screen_height[index];
//		tt_screen_width = video_mode;
////      tt_number_of_colors = mode_number_of_colors[index];
//		return(TRUE);
//   } else { 
	// new on 271299 so new formats can be run via the command line
	   tt_screen_width = video_mode;
		tt_screen_height = (3*tt_screen_width)/4;
		if (tt_screen_height == 960) {
			// new on 141204 since 1280x1024 is common and 1280x960 isn't -- what about aspect ratio?
			tt_screen_height = 1024;
		};
		return(TRUE);
//	};
//   cout << video_mode << AC(IDC_IS_AN_INVALID_VIDEO_MODE) << endl;
//   tt_exit_failure(DONT_TROUBLE_SHOOT,S(IDS_VIDEO_MODE_BAD));
//   return(FALSE);
};

// should figure out if being reset or initialized and pass boolean to
// initialize_sprite_values

flag mode_set = FALSE;

boolean tt_setmode(int video_mode, boolean reset) { // reset added 190300
//#if TT_SMALL_TRIAL
   //  added on 120700 since values greater than 320 causes problems with talk balloons
//	if (video_mode > 320) {
//		video_mode = 320;
//		tt_video_mode = 320;
//	};
//#endif
  boolean need_to_reallocate = (video_mode != tt_screen_width);
  if (!tt_set_screen_parameters(video_mode)) return(FALSE);  
  if (tt_screen_width == 320 && tt_builtin_picture_directory != NULL) { 
	  // new on 200502 so use old m22.us1 and the like when running at this resolution
     delete [] tt_builtin_picture_directory;
     tt_builtin_picture_directory = NULL;
     tt_desired_bits_per_pixel = 8;
  };
//  if (tt_memory_graphics_work_page != NULL) { // changing modes?
//              delete [] tt_memory_graphics_work_page;
//  };
  if (mode_set && !reset) return(TRUE); // already done (new on 270200)
  if (need_to_reallocate) { // moved here on 270105
//	  MessageBox(NULL,"about to reallocate_appearance_data","TT",MB_OK); // just for testing!! 270105
	  reallocate_appearance_data(); // since changing resolution -- new on 240105
  };
  if (!open_images_file(video_mode)) return(FALSE);
  mode_set = TRUE;
#if TT_WINDOWS&&GRAPHICS_TO_MEMORY
  tt_screen_bytes_size = ((long) tt_screen_width)*tt_screen_height*tt_bytes_per_pixel;
//  tt_dib_size = tt_screen_bytes_size+dib_leader_size;
//  tt_work_dib = (dib) working_page_memory;
//  tt_memory_graphics_work_page = working_page_memory+dib_leader_size;
#if !WING&&!TT_DIRECTX
  tt_working_page_handle = global_alloc(GMEM_FIXED|GMEM_ZEROINIT,tt_screen_bytes_size);
  tt_memory_graphics_work_page = (work_page) GlobalLock(tt_working_page_handle);
#endif
  if (reset && tt_background_page_handle != NULL) {
		GlobalUnlock(tt_background_page_handle);
		GlobalFree(tt_background_page_handle);
  };
  tt_background_page_handle = global_alloc(GMEM_FIXED|GMEM_ZEROINIT,tt_screen_bytes_size);
  tt_background_page = (work_page) GlobalLock(tt_background_page_handle);
#endif
  tt_screen_half_width = tt_screen_width/2;
  tt_screen_half_height = tt_screen_height/2;
  tt_ideal_units_per_horizontal_pixel = (ideal_screen_width+tt_screen_width-1)/tt_screen_width; //round up unless exact
  tt_ideal_units_per_vertical_pixel = (ideal_screen_height+tt_screen_height-1)/tt_screen_height;
//  if (tt_log_version_number > 14) {
//     tt_half_ideal_units_per_horizontal_pixel 
//	      = tt_ideal_units_per_horizontal_pixel/2;
//	  tt_half_ideal_units_per_vertical_pixel
//			= tt_ideal_units_per_vertical_pixel/2;
//  } else { // didn't round to nearest before
//	  tt_half_ideal_units_per_horizontal_pixel = 0;
//	  tt_half_ideal_units_per_vertical_pixel = 0;
//  };
//  strcpy(tt_sprite_dir,sprite_dir_prefix);
//  itoa(video_mode,tt_sprite_dir+strlen(sprite_dir_prefix),10); // update in place (after m)
//#if TT_32
//  strcat(tt_sprite_dir,"_32\\");
//#else
//  strcat(tt_sprite_dir,"\\");
//#endif
  if (!initialize_sprite_values()) return(FALSE);
  initialize_page_grid(reset);
  if (tt_screen != NULL) tt_screen->screen_dirty();
  // set default blit parameters
  tt_destination_work_page = tt_memory_graphics_work_page;
  tt_destination_width = tt_screen_width;
  tt_destination_height = tt_screen_height;
  unclip(FALSE);
  sound_flag = (flag) (tt_sound_option&SOUND_BIT); // why is this here?
  initialize_house_sizes();
  tt_min_flying_scale = ground_scale;
//  adjust_width_to_640x480_screen(tt_min_flying_scale); // in case screen is more than 640x480
  return(TRUE);
};

UseProfile use_profile;

void read_use_profile(InputStream *history_file) {
//  following worked but was compiler dependent due to byte boundaries (14 bytes vs 16)
//    history_file.read((string) &use_profile, sizeof(use_profile));
    history_file->read((string) &use_profile.first_use, sizeof(time_t));
    history_file->read((string) &use_profile.last_use, sizeof(time_t));
    history_file->read((string) &use_profile.seconds_used, sizeof(time_t));
	 history_file->read((string) &use_profile.use_count, sizeof(short int));
};

void write_use_profile(output_stream &history_file) {
//  following worked but was compiler dependent due to byte boundaries (14 bytes vs 16)
//    history_file.write((char *) &use_profile, sizeof(use_profile));
    history_file.write((string) &use_profile.first_use, sizeof(time_t));
    history_file.write((string) &use_profile.last_use, sizeof(time_t));
    history_file.write((string) &use_profile.seconds_used, sizeof(time_t));
	 history_file.write((string) &use_profile.use_count, sizeof(short int));
};

time_t return_last_use() { // obsolete??
	return(use_profile.last_use);
};

void read_user_parameters(InputStream *file, Parameters &parameters, boolean using_default_user_profile) {
   parameters.video_mode = file->get();
   parameters.programmer_head = file->get();
	if (using_default_user_profile) { 
		// new on 160301 - profile created with right name - just don't clobber it here
//		unsigned char buffer[256];
//		file->read(buffer,256);
		file->ignore(256); // rewritten on 200703
	} else {
		file->read((string) &parameters.user_name,256);
		// saw a corrupted USR file where this was nonsense so added this on 161105
		if (file->empty()) {
         parameters.user_name[0] = '\0';
			tt_error_file() << "USR file is corrupt." << endl;
			return;
		};
	};
   file->read((string) &parameters.city_size, sizeof(short int));
   file->read((string) &parameters.character_size_percent, sizeof(short int));
   file->read((string) &parameters.help_speed_percent, sizeof(short int));
   parameters.mouse_button_count = file->get();
   parameters.maintain_history = file->get();
   parameters.minimum_say_importance = file->get();
   parameters.serial_port_to_open = file->get();  // obsolete -- OK to recycle
   parameters.user_is_debugger = file->get();	// doesn't belong here either
   parameters.temporary_user = file->get();
   parameters.sound_option = file->get();
   file->read((string) &parameters.sleep, sizeof(unsigned short int));
   file->read((string) &parameters.speed, sizeof(unsigned short int));
   parameters.using_dispdib = file->get(); // obsolete
   parameters.marty_talk = file->get();
   parameters.load_items_version_number = file->get();
   file->read((string) &parameters.puzzle_counter, sizeof(short int));
   file->read((string) &parameters.xml_version, sizeof(short int));
   file->read((string) &parameters.expansion4, sizeof(short int));
};

void write_user_parameters(output_stream &file, Parameters *parameters) {
	file.put(parameters->video_mode);
//   file.put(parameters->video_mode);
	file.put(parameters->programmer_head);
   file.write((string) &parameters->user_name, 256);
   file.write((string) &parameters->city_size, sizeof(short int));
   file.write((string) &parameters->character_size_percent, sizeof(short int));
   file.write((string) &parameters->help_speed_percent, sizeof(short int));
   file.put(parameters->mouse_button_count);
   file.put(parameters->maintain_history);
   file.put(parameters->minimum_say_importance);
   file.put(parameters->serial_port_to_open);  // obsolete -- OK to recycle
   file.put(parameters->user_is_debugger);  // doesn't belong here either
   file.put(parameters->temporary_user);
   file.put(parameters->sound_option);
   file.write((string) &parameters->sleep, sizeof(unsigned short int));
   file.write((string) &parameters->speed, sizeof(unsigned short int));
   file.put(parameters->using_dispdib);
   file.put(parameters->marty_talk);
   file.put(parameters->load_items_version_number);
   file.write((string) &parameters->puzzle_counter, sizeof(short int));
   file.write((string) &parameters->xml_version, sizeof(short int));
   file.write((string) &parameters->expansion4, sizeof(short int));
};

const int new_history_count = 1; // 1 added on 11/6/97 -- put at end so as not to break old profiles

void dump_history(boolean puzzle_state_changed) {
   if (!tt_maintain_history || tt_user_parameters == NULL) return; 
	// second disjunct added 311099 since might be trouble shooting too early to deal with this
   if (tt_system_mode == PUZZLE && !puzzle_state_changed && !ok_to_save_puzzle_state()) return;
	if (replaying()) return; // new on 151204 -- no point maintaining USR when replaying someone's time travel file or demo
   ascii_string full_file_name = new_file_name(tt_file_name,AC(IDS_USR));
   output_file_stream history_file(full_file_name,std::ios_base::binary|std::ios_base::out);
	if (history_file == NULL) { // new on 290904 for error recovery
		unable_to_write_file(full_file_name); 
		return;
	};
	delete [] full_file_name;
   dump_history(history_file,puzzle_state_changed);
   history_file.close();
};

void dump_history(output_file_stream &file, boolean puzzle_state_changed) {
	if (tt_histories == NULL) return; // added on 140999 to enable reading of old DMO files
   time_t now = seconds_since_1970();
   use_profile.seconds_used += now-use_profile.last_use;
   use_profile.last_use = now;
//  history_file.write((string) &use_profile, sizeof(use_profile));
   write_use_profile(file);
   int i;
   for (i = 0; i < history_count-new_history_count; i++) {
	   History history = tt_histories[i];
	   file.write((string) &history,sizeof(history));
   };
   write_user_parameters(file,tt_user_parameters);
//  history_file.write((string) tt_user_parameters,sizeof(Parameters));
   save_puzzle_state(file,puzzle_state_changed);
   if (tt_toolbox != NULL && !tt_toolbox->gone() && tt_system_mode != PUZZLE) {
	   // second condition added 030300
	   // third conjunct added 170300 since puzzles shouldn't change favorites
      tt_toolbox->update_tt_favorites();
   };
   file.write((string) tt_favorites,favorites_count*sizeof(FavoriteSizeAndLocation));
   for (i = history_count-new_history_count; i < history_count; i++) { // dump new ones now
	   History history = tt_histories[i];
	   file.write((string) &history,sizeof(history));
   };
};

InputStream *history_file = NULL; // out here so it can be resumed
//flag history_file_already_opened = FALSE; // new on 290301 since NULL test on the above is not reliable -- was true when it was an input_file_stream
flag install_user_profile;
Parameters user_parameters;

void initialize_history(boolean install_user_profile_flag, boolean called_from_log) {
	// updated on 061103 since may have been called with NoName and now with real name -- OK?
	if (history_file != NULL) { // new on 290301 since can be called recursively via compute_tt_file_name via interpret_command_line
		history_file->close();
		delete history_file; // new on 130104 to fix a minor leak
		history_file = NULL;
//		return; // commented out 130104
	};
   boolean using_default_user_profile = FALSE;
   if (!tt_temporary_user || called_from_log) {
      ascii_string full_file_name = new_file_name(tt_file_name,AC(IDS_USR));
	   if (!file_exists(full_file_name,TRUE) && tt_default_file_name != NULL) { // re-written on 160301
//		   delete [] full_file_name; // rewritten on 170603
//		   full_file_name = new_file_name(tt_default_file_name,AC(IDS_USR));
         // rewritten to use existing_file_name on 060402
			UnwindProtect<string> set(tt_user_directory,tt_default_user_directory);
         FileNameStatus status;
         string alternative_full_file_name = existing_file_name(status,tt_default_file_name,AC(IDS_USR),"NewUsers",TRUE,FALSE,FALSE,FALSE,TRUE);
		   using_default_user_profile = TRUE;
			if (alternative_full_file_name != NULL) {
				delete [] full_file_name;
				full_file_name = alternative_full_file_name;
			}; // used to return and then no tt_history etc.
		};
		history_file = new FileInputStream();
//		history_file_already_opened = TRUE;
		history_file->open(full_file_name,std::ios_base::binary|std::ios_base::in); // |std::ios_base::nocreate -- obsolete? 210703
      delete [] full_file_name;
//     if (history_file == NULL && tt_default_file_name != NULL) { // try default user
//		    history_file.close(); // new on 160301
//        full_file_name = new_file_name(AC(IDS_USERS_DIRECTORY),tt_default_file_name,AC(IDS_USR));
//        history_file.open(full_file_name,std::ios_base::binary|std::ios_base::in|std::ios_base::nocreate);
//        delete [] full_file_name;
//     };
   };
   read_history(history_file,install_user_profile_flag,called_from_log,using_default_user_profile);
};

void read_history(InputStream *file, boolean install_user_profile_flag, boolean called_from_log, 
						boolean using_default_user_profile) {
  install_user_profile = install_user_profile_flag;
  boolean history_already_initialized = (tt_histories != NULL);
  if (!history_already_initialized) { // conditional new on 120201 - otherwise Feb 12 and Feb12 interfere sometimes
	  tt_histories = new History[history_count];
  };
  if ((!tt_temporary_user || called_from_log) && !file->empty()) { // (tt_log_in_archive != NULL || file != NULL)) {
	  read_use_profile(file);
	  for (int i = 0; i < history_count-new_history_count; i++) {
			History history;
			file->read((string) &history,sizeof(History));
			tt_histories[i] = history;
	  };
     if (tt_system_mode != PUZZLE && tt_time_travel == TIME_TRAVEL_OFF && !replaying()) {
		  // commented out the following on 011204 since it was goofy and occassionally you got asked twice
    //    int days_to_forget_how_to_use_toontalk;
    //    if (use_profile.use_count < 14) {
    //        days_to_forget_how_to_use_toontalk = 14;
    //    } else { // if you have used ToonTalk 50 times but it has been 50 days since last use offer to reset history
    //       days_to_forget_how_to_use_toontalk = use_profile.use_count;
    //    };
    //    if (use_profile.last_use > 0 && !using_default_user_profile && // !using_default_user_profile added on 190601
				//(seconds_since_1970()-use_profile.last_use)/seconds_in_a_day > days_to_forget_how_to_use_toontalk) {
           if (unnamed_user()) {			    
 //             ||  message_box(MB_YESNO,S(IDS_OK_IF_MARTY_SUGGESTS_STUFF_YOU_ONCE_DID)) == IDYES) {
              for (int i = 0; i < history_count; i++) {
		           tt_histories[i] = 0;
	           };
           };
//        };
     };
  } else if (!history_already_initialized) { // conditional new on 120201 
	  for (int i = 0; i < history_count; i++) {
		 tt_histories[i] = 0;
	  };
	  use_profile.first_use = seconds_since_1970();
	  use_profile.use_count = 0;
	  use_profile.seconds_used = 0;
  };
  use_profile.use_count++;
  long last_use = use_profile.last_use;
  use_profile.last_use = seconds_since_1970();
  if (!tt_temporary_user || called_from_log) { // disjunct new on 040300
     if (install_user_profile && !file->empty()) { // older version didn't have more
//       history_file.read((string) &user_parameters,sizeof(user_parameters));
        read_user_parameters(file,user_parameters,using_default_user_profile);
     };
     if (!file->empty()) {
        read_puzzle_state(file,called_from_log);
     };
	  if (install_user_profile && !file->empty()) { 
		  // moved here on 160300 since don't install parameter until having read puzzle state
	  	  user_parameters.install_parameters(last_use);
	  };
  };
//#if TT_DEBUG_ON
//  if (tt_debug_mode == 160299) {
//	  tt_error_file() << "Read history - file position is " << (int) file.tellg() << endl;
//  };
//#endif
};

short int return_use_count() {
  return(use_profile.use_count);
};

void resume_initialize_history() {
	if (history_file == NULL) return; // new on 290204 for robustness
	resume_initialize_history(history_file);
	history_file->close();
	delete history_file; // new on 130104 to fix a minor leak
	history_file = NULL; // new on 130104
};

long resume_initialize_history(InputStream *file, boolean called_from_log) {
   // do this later since things aren't initialized enuf now to create sprites
#if TT_DEBUG_ON
   if (tt_debug_mode == 160299) {
	   tt_error_file() << "About to resume intialize history - file position is " << (int) file->tellg() << endl;
   };
#endif
   if (file == NULL) {
	   return(0); // for robustness - new on 130104
   };
	if (!file->empty() && (!tt_temporary_user || called_from_log)) { 
		// added called_from_log on 040300 so logs of temporary uses work ok 
      int version_number = user_parameters.load_items_version_number;
      if (version_number < 6) version_number = 5; // 5 is the implicit version number of old items
      long restart = resume_read_puzzle_state(file,version_number,called_from_log,user_parameters.xml_version);
	   if (called_from_log && restart > 0) { // new on 050600
		   return(restart);
	   };
   };
#if TT_DEBUG_ON
   if (tt_debug_mode == 160299) {
	   tt_error_file() << "About to read_stuff_after_puzzle_state - file position is " << (int) file->tellg() << endl;
   };
#endif
   read_stuff_after_puzzle_state(file);
#if TT_DEBUG_ON
   if (tt_debug_mode == 160299) {
	   tt_error_file() << "Resumed intialize history - file position is " << (int) file->tellg() << endl;
   };
#endif
   return(0);
};

void read_tt_favorites(InputStream *stream) {
   // skip this if favorites already set
   for (int i = 0; i < favorites_count; i++) {
      stream->read((string) &tt_favorites[i],sizeof(FavoriteSizeAndLocation));
      if (stream->empty()) { // an old or corrupt history file so ignore favorites
//         tt_favorites_set = FALSE;
         return;
      };
   };
//   tt_favorites_set = (tt_favorites[TOOLBOX_FAVORITE].favorite_width > 0); 
	// if zero didn't read anything (was != 0 prior to 170300)
};

boolean favorites_set(FavoriteNames name) {
   return(tt_favorites[name].favorite_width > 0); // new on 161102
};

void read_stuff_after_puzzle_state(InputStream *stream) {
  if (!stream->empty()) {
     read_tt_favorites(stream);
  };
  if (!stream->empty()) {
  	  for (int i = history_count-new_history_count; i < history_count; i++) {
		  History history;
		  stream->read((string) &history,sizeof(History));
		  tt_histories[i] = history;
	  };
  };
};

//FavoriteSizeAndLocation *user_favorite_sizes_and_locations(boolean &already_set) {
//  if (!favorites_set) return(NULL);
//   already_set = favorites_set;
//   return(favorites);
//};

void unclip(boolean tell_windows) {
  tt_destination_llx = 0;
  tt_destination_lly = 0;
  tt_destination_urx = tt_screen_width; // experiment on 180702 -1;
  tt_destination_ury = tt_screen_height; // ditto -1;
  if (tell_windows) {
     tt_main_window->set_clip_region();
  };
};

//boolean clipped() { // obsolete??
//  return(tt_destination_llx != 0 ||
//         tt_destination_lly != 0 ||
//         tt_destination_urx != tt_screen_width-1 ||
//         tt_destination_ury != tt_screen_height-1);
//};

#if TT_DIRECTX

LPDIRECTSOUND direct_sound = NULL;

//flag direct_sound_cooperative_level_set = FALSE; // do I still need this? Isn't it FALSE when direct_sound is NULL? // commented out on 251102

boolean initialize_sound() {
	if (tt_sound_option == 0 || about_to_quit()) {
		return(FALSE);
	};
   HRESULT status;
   if (direct_sound != NULL) { // already created
      status = DS_OK;
   } else {
      status = DirectSoundCreate(NULL, &direct_sound, NULL);  // had troubles on P100 so ran with DSERR_ALLOCATED instead
   };
   if (DS_OK == status) {
      status = direct_sound->SetCooperativeLevel(tt_main_window->get_handle(),DSSCL_NORMAL);
		// change cooperative level to DSSCL_PRIORITY if I need to set sample rate or the like
      if (DS_OK == status) {
//         direct_sound_cooperative_level_set = TRUE;
         return(TRUE);
      };
      tt_error_file() << "Problem setting cooperative level of Direct Sound." << endl;
//      direct_sound_cooperative_level_set = FALSE;
   } else {
      if (status != DSERR_ALLOCATED) log(_T("Problems initializing Direct Sound."),FALSE,TRUE);
      direct_sound = NULL; // just in case
   };
   switch (status) {
      case DSERR_ALLOCATED:
#if TT_DEBUG_ON
         tt_error_file() << "DirectSound's SetCooperativeLevel failed because resources (such as a priority level) were already in use by another caller. Will try again in a second." << endl;
#endif
         // try again in a second -- might be just busy with speech not yet turned off
         if (!tt_shutting_down) tt_global_picture->do_after(1000,NULL,INITIALIZE_SOUND_CALLBACK);
			return(TRUE); // trying again so don't consider this a failure
      case DSERR_INVALIDPARAM:
         tt_error_file() << "An invalid parameter was passed to the returning function." << endl;
         break;
      case DSERR_NOAGGREGATION:
         tt_error_file() << "This object does not support aggregation." << endl;
         break;
      case DSERR_NODRIVER:
         tt_error_file() << "No sound driver is available for use." << endl;
         break;
      case DSERR_OUTOFMEMORY:
         tt_error_file() << "The DirectSound subsystem couldn't allocate sufficient memory to complete the caller's request." << endl;
         break;
      default:
         tt_error_file() << "Unknown error status from DirectSoundCreate is " << status << endl;
   };
   return(FALSE);
};

void release_direct_sound() {
   if (direct_sound != NULL) {
      if (!tt_shutting_down) {
         release_sound_cache();
      };
      direct_sound->Release();
//		id_of_sound_to_play = 0; // new on 100700
      direct_sound = NULL;
		current_sound_buffer = NULL; // new on 130700 since released by toggle_pause and then restarted
		release_next_speak(); // new on 130700 for the same reason
		release_previous_speak(); // ditto
   };
};

LPDIRECTSOUND pointer_to_direct_sound() {
   return(direct_sound);
};

/*
boolean create_sound_buffer(//LPDIRECTSOUND lpDirectSound,
                            LPDIRECTSOUNDBUFFER *lplpDsb) {
//                            short int channels, int samples_per_second,
//                            short int bits_per_sample, int buffer_size) {
    PCMWAVEFORMAT pcmwf;
    DSBUFFERDESC dsbdesc;
    HRESULT hr;
    // Set up wave format structure.
    memset(&pcmwf, 0, sizeof(PCMWAVEFORMAT));
    pcmwf.wf.wFormatTag = WAVE_FORMAT_PCM;
    pcmwf.wf.nChannels = channels;
    pcmwf.wf.nSamplesPerSec = samples_per_second;
    pcmwf.wf.nBlockAlign = channels*bits_per_sample/8;
    pcmwf.wf.nAvgBytesPerSec =
        pcmwf.wf.nSamplesPerSec * pcmwf.wf.nBlockAlign;
    pcmwf.wBitsPerSample = bits_per_sample;
    // Set up DSBUFFERDESC structure.
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); // Zero it out.
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_CTRLDEFAULT;//DSBCAPS_CTRLVOLUME; // just volume ok for now
    // could |DSBCAPS_STATIC	above for using hardware buffers
    dsbdesc.dwBufferBytes = //buffer_size;
     3 * pcmwf.wf.nAvgBytesPerSec; // 3 second buffer.
// another approach

   PCMWAVEFORMAT pcmwf;
    DSBUFFERDESC dsbdesc;
    HRESULT hr;
    // Set up wave format structure.
    memset(&pcmwf, 0, sizeof(PCMWAVEFORMAT));
    pcmwf.wf.wFormatTag = WAVE_FORMAT_PCM;
    pcmwf.wf.nChannels = 2;
    pcmwf.wf.nSamplesPerSec = 22050;
    pcmwf.wf.nBlockAlign = 4;
    pcmwf.wf.nAvgBytesPerSec =
        pcmwf.wf.nSamplesPerSec * pcmwf.wf.nBlockAlign;
    pcmwf.wBitsPerSample = 16;
    // Set up DSBUFFERDESC structure.
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); // Zero it out.
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_CTRLDEFAULT; // Need default controls
    dsbdesc.dwBufferBytes = 3 * pcmwf.wf.nAvgBytesPerSec; // 3 second

    dsbdesc.lpwfxFormat = (LPWAVEFORMATEX)&pcmwf;
    // Create buffer.
    hr = direct_sound->CreateSoundBuffer(&dsbdesc, lplpDsb, NULL);
    if (DS_OK == hr) {
        // Succeeded! Valid interface is in *sound_buffer.
        return(TRUE);
    } else {
        // Failed!
        *lplpDsb = NULL;
        return (FALSE);
    }
}

boolean write_to_sound_buffer(LPDIRECTSOUNDBUFFER sound_buffer, // was lpDsb
                              LPBYTE SoundData,
                              DWORD SoundBytes,
                              DWORD Offset) {
    LPVOID lpvPtr1;
    DWORD dwBytes1;
    LPVOID lpvPtr2;
    DWORD dwBytes2;
    DWORD dwAudio1; // was missing from example in help file
    DWORD dwAudio2;
    HRESULT hr;
    // Obtain write pointer.
    hr = sound_buffer->Lock(Offset, SoundBytes, &lpvPtr1,
                            &dwBytes1, &lpvPtr2, &dwBytes2, 0);
    // If we got DSERR_BUFFERLOST, restore and retry lock.
    if (DSERR_BUFFERLOST == hr) {
       sound_buffer->Restore();
       hr = sound_buffer->Lock(Offset, SoundBytes,
                               &lpvPtr1, &dwAudio1, &lpvPtr2, &dwAudio2, 0);
    };
    if (DS_OK == hr) {
        // Write to pointers.
        CopyMemory(lpvPtr1, SoundData, dwBytes1);
        if (NULL != lpvPtr2) {
            CopyMemory(lpvPtr2, SoundData+dwBytes1, dwBytes2);
        };
        // Release the data back to DirectSound.
        hr = sound_buffer->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);
        if (DS_OK == hr) {
           // Success!
           return(TRUE);
        }
    }
    // If we got here, then we failed Lock, Unlock, or Restore.
    return(FALSE);
};

boolean copy_file_to_sound_buffer(LPDIRECTSOUNDBUFFER sound_buffer,
                                  HFILE file, int size) {
    bytes data;
    DWORD dwBytes1;
    LPVOID lpvPtr2;
    DWORD dwBytes2;
    DWORD dwAudio1; // was missing from example in help file
    DWORD dwAudio2;
    HRESULT hr;
    // Obtain write pointer.

    hr = sound_buffer->Lock(0, size, &data,
                            &dwBytes1, &lpvPtr2, &dwBytes2, 0);
    // If we got DSERR_BUFFERLOST, restore and retry lock.
    if (DSERR_BUFFERLOST == hr) {
       sound_buffer->Restore();
       hr = sound_buffer->Lock(0, size, &data,
                               &dwAudio1, &lpvPtr2, &dwAudio2, 0);
    };
    if (DS_OK == hr) {
       while (size > read_buffer_size) {
	       _lread(file,data,read_buffer_size);
	       data += read_buffer_size;
	       size -= read_buffer_size;
	    };
	    _lread(file,data,size);
       // Release the data back to DirectSound.
       hr = sound_buffer->Unlock(data, dwBytes1, lpvPtr2, dwBytes2);
       if (DS_OK == hr) {
          // Success!
          return(TRUE);
       };
    };
    // If we got here, then we failed Lock, Unlock, or Restore.
    return(FALSE);
};
*/
#endif

//void temporary_sound(DeletedMemory *sound) {
//      if (sound_to_delete != NULL) delete sound_to_delete;
//      sound_to_delete = sound;
//};

//#if TT_DIRECTX

void play_sound(int id, boolean repeat, boolean play_even_if_time_travel_paused) { // boolean play_if_tied
	// second arg was obsolete (priority) and was eliminated on 200204
	if (!sound_flag || direct_sound == NULL || about_to_quit()) return; 
	// direct_sound == NULL was !direct_sound_cooperative_level_set prior to 251102
	if (tt_time_travel == TIME_TRAVEL_PAUSED && !play_even_if_time_travel_paused) return; 
	// play_even_if_time_travel_paused new on 230903
#if TT_DEBUG_ON
   if (tt_debug_mode == 3334) {
      tt_error_file() << "Sound id=" << id
                  << "; frame=" << tt_frame_number << endl;
   };
#endif
	if (repeat && id_of_sound_to_play == id) return; 
	// new on 120100 -- good idea?? or this why I lose helicopter sounds while time travelling??
	if (sound_to_delete != NULL) {
      delete sound_to_delete;
      sound_to_delete = NULL;
   };
//   if (direct_sound == NULL) return; // turned off just now -- commented out on 251102
	sound_buffer sound = retrieve_sound(id,sound_to_delete);
	if (sound == NULL) return; // not enough memory or some problem
//	HRESULT result = sound->SetVolume(-5000); // this seems to have no effect??
//	if (result != DS_OK) { 
//		log("debug this");
//	};
	if (repeat) {
		id_of_sound_to_play = id;
		sound->Play(0,0,DSBPLAY_LOOPING);
	} else {
      id_of_sound_to_play = 0;
      sound->Play(0,0,0);
	};
};
//#else // if not TT_DIRECTX
//void play_sound(int id, int priority, boolean repeat) { // boolean play_if_tied) {
//	if (!sound_flag) return;
//#if TT_DEBUG_ON
//   if (tt_debug_mode == 3334) {
//      tt_error_file() << "Sound id=" << id << "; priority=" << priority
//                  << "; frame=" << tt_frame_number << endl;
//   };
//#endif
//   boolean dont_play_if_playing = (priority <= current_sound_priority);
////         (priority < current_sound_priority || 
////          (!play_if_tied && priority == current_sound_priority));
//	if (dont_play_if_playing && 
//		 (id_of_sound_to_play > 0 || !sndPlaySound(NULL,SND_NOSTOP))) {
//      // am playing a sound with higher priority
//      return;
//   };
//	if (sound_to_delete != NULL) {
//	   sndPlaySound(NULL,NULL); // just in case
//      delete sound_to_delete;
//      sound_to_delete = NULL;
//   };
//	// until I have mixing just let highest priority one started during a frame win
//	id_of_sound_to_play = id;
//	current_sound_priority = priority;
//	repeating_current_sound = repeat;
//};
//#endif

#if TT_DIRECTX
void resume_repeating_sound() {
   if (id_of_sound_to_play != 0 && direct_sound != NULL) {
		// second condition added 100700 so if called (e.g. by getting focus) before direct_sound object is restored doesn't do the following
		int id = id_of_sound_to_play;
		id_of_sound_to_play = 0; // new on 100700 so when resumed check for mulitple playing of same sound doesn't stop this
      play_sound(id,TRUE);
   };
};
#endif

#if !TT_DIRECTX
void play_current_sound() {
	if (id_of_sound_to_play == 0) return;
//      if (repeating_current_sound && !sndPlaySound(NULL,SND_NOSTOP)) return;
//   if (tt_debug_mode == 9999) {
//		char buffer[32];
//		output_string_stream file_name(buffer,32);
//		file_name << "s" << id_of_sound_to_play << ".wav";
//		file_name.put((char)0);
//		if (!sndPlaySound(buffer,(UINT) SND_ASYNC)) {
//			// play sound failed so turn off that option
//			turn_off_sound();
//		};
//		id_of_sound_to_play = 0;
//		return;
//	};
	work_page sound = retrieve_sound(id_of_sound_to_play,sound_to_delete);
	id_of_sound_to_play = 0;
	if (sound == NULL) return; // not enough memory or some problem
	UINT flags = SND_ASYNC;
	// following is not quite right but good enough
	// the problem is that I would really like to conditionally set
	// the priority and then restore the old value when it finishes playing
	if (repeating_current_sound) {
#if TT_32
	  // looping from memory doesn't work in win32s
//		sndPlaySound("helicopt.wav",flags|SND_LOOP);
#else
		if (!sndPlaySound((char *) sound,(UINT) (flags|SND_MEMORY|SND_LOOP))) {
			// play sound failed so turn off that option
			turn_off_sound();
		};
#endif
	} else {
		if (!sndPlaySound((char *) sound,(UINT) (flags|SND_MEMORY))) {
			turn_off_sound();
		};
	};
};
#endif

void play_sound_bytes(sound_buffer sound, 
#if !TT_32
							 long size, HGLOBAL memory_handle,
#endif
							 int priority) {
	if (!sound_flag) return;
	if (sound == NULL) return;
	stop_sound(FALSE);
	id_of_sound_to_play = 0;
	if (!sndPlaySoundG((char *) sound,(UINT) (SND_ASYNC|SND_MEMORY))) {
      turn_off_sound();    // was commented out -- why????
      sndPlaySound(NULL,NULL); // just in case
		tt_error_file() << S(IDS_SOMETHING_WENT_WRONG_PLAYING_A_SOUND) << endl;
#if TT_32
//		delete [] sound;
#else
//		tt_error_file() << "Its size is " << size << endl;
//		if (memory_handle != NULL) {
//			GlobalUnlock(memory_handle);
//			GlobalFree(memory_handle);
//		 } else {
//			delete [] sound;
//		 };
#endif
	} else {
		current_sound_priority = priority;
   };
#if TT_DIRECTX
   sound_to_delete = new DeletedMemorySoundBuffer(sound,NULL);
#else
	sound_to_delete = new DeletedMemoryHandle(sound,
#if !TT_32
														   size,memory_handle,
#endif
														   NULL);
#endif
};

boolean play_sound_file(ascii_string file_name, int priority) {
	current_sound_priority = priority;
	return((boolean) sndPlaySoundG(file_name,SND_ASYNC));
};

#if TT_DIRECTX
void turn_off_direct_sound() {
//	if (sound_to_delete != NULL) {
//      delete sound_to_delete; // should turn it off by releasing it
//      sound_to_delete = NULL;
//   };
//   stop_all_sounds_in_cache();
//   turn_off_sound(FALSE);
   release_direct_sound();
};

void turn_on_direct_sound() {
   if (initialize_sound()) { 
		turn_on_sound(FALSE);
	} else {
      tt_sound_option = 0;
	};
};
#endif

void turn_off_sound(boolean forever) {
   sound_flag = FALSE;
#if TT_DEBUG_ON
	if (tt_debug_mode == 1600) return; // leave sound on
#endif
	if (forever) tt_sound_option = (char) (tt_sound_option&~SOUND_BIT);
};

void turn_on_sound(boolean forever) {
   sound_flag = TRUE;
	if (forever) tt_sound_option = (char) (tt_sound_option|SOUND_BIT); // was & prior to 220802 !!
};

//#if TT_DIRECTX
void stop_sound_id(int id) {
   if (id <= 0) return; // <= as of 210404
   if (sound_to_delete != NULL) {
      delete sound_to_delete;
      sound_to_delete = NULL;
   };
	sound_buffer sound = retrieve_sound(id,sound_to_delete);
	if (sound == NULL) return; // not enough memory or some problem
   if (sound_to_delete == NULL) sound->Stop(); // is really a pre-existing one from the cache
   id_of_sound_to_play = 0;
};
//#else
//void stop_sound_id(int id) {
//   if (id < 0) return;
//   if (!repeating_current_sound) return;
////   if (id != id_of_sound_to_play) return;
//   stop_sound();
//};
//#endif

//#if TT_DIRECTX
boolean stop_sound(boolean narration_too) { // added narration_too (and removed an obsolete priority number) on 170204
   stop_all_sounds_in_cache();
	if (narration_too) {
		sndPlaySound(NULL,SND_SYNC); // might be playing narration for example made conditional on 170204 
	};
//	id_of_sound_to_play = 0;
   return(TRUE);
};
//#else
//boolean stop_sound(int if_priority_less_than) {
//	// returns true of no sound is playing or it stopped it
//	if (current_sound_priority >= if_priority_less_than) {
//		return((boolean) sndPlaySound(NULL,SND_NOSTOP)); // returns FALSE if sound is playing
//	};
//	current_sound_priority = 0;
//#if TT_DEBUG_ON
//		if (tt_debug_mode == 999) {
//			tt_error_file() << "Sound stopped at " << tt_frame_number << endl;
//		};
//#endif
//	sndPlaySound(NULL,SND_SYNC);
//	new_sound_entry(-1);
//	if (sound_to_delete != NULL) {
//		delete sound_to_delete;
//		sound_to_delete = NULL;
//	};
//	return(TRUE);
//};
//#endif

string append_strings(const_string a, const_string b) {
//  if (a == NULL) return(b);
//  if (b == NULL) return(a);
  int a_length = _tcslen(a);
  string result = new character[a_length+_tcslen(b)+1];
  _tcscpy(result,a);
  _tcscpy(result+a_length,b);
  return(result);
};

wide_string append_strings(wide_string a, wide_string b) { // new on 311002
  int a_length = wide_string_length(a);
  wide_string result = new wide_character[a_length+wide_string_length(b)+1];
  wcscpy(result,a);
  wcscpy(result+a_length,b);
  return(result);
};

ascii_string append_ascii_strings(const_ascii_string a, const_ascii_string b) {
//  if (a == NULL) return(b);
//  if (b == NULL) return(a);
  int a_length = strlen(a);
  int b_length = strlen(b);
  ascii_string result = new char[a_length+b_length+1];
  strcpy(result,a);
  memcpy(result+a_length,b,b_length+1); // including terminator
  return(result);
};

string append_3_strings(const_string a, const_string b, const_string c) {
  int a_length = _tcslen(a);
  int b_length = _tcslen(b);
  int c_length = _tcslen(c);
  ascii_string result = new character[a_length+b_length+c_length+1];
  _tcscpy(result,a);
  memcpy(result+a_length,b,b_length*sizeof(character));
  memcpy(result+a_length+b_length,c,(c_length+1)*sizeof(character));
  return(result);
};

ascii_string append_3_ascii_strings(const_ascii_string a, const_ascii_string b, const_ascii_string c) {
//  if (a == NULL) return(b);
//  if (b == NULL) return(a);
  int a_length = strlen(a);
  int b_length = strlen(b);
  int c_length = strlen(c);
  ascii_string result = new char[a_length+b_length+c_length+1];
  strcpy(result,a);
  memcpy(result+a_length,b,b_length);
  memcpy(result+a_length+b_length,c,c_length+1);
  return(result);
};

string copy_string(const_string source, int length) {
	// simplified on 040803
   if (source == NULL) return(NULL);
//	if (source[0] == NULL) return(source); // new on 211200 -- removed 120101 since can be for example a label and get deleted
   if (length < 0) length = strlen(source);
   string destination = new char[length+1];
   memcpy(destination,source,length);
   destination[length] = NULL; // terminate the string
   return(destination);
};

string copy_string_without_quotes(const_string source, int length) { // new on 260601
	if (source == NULL) return(NULL);
	if (source[0] == '"') {
		if (length < 0) length = _tcslen(source);
		while (source[length-1] == ' ') { // new on 270601 to remove trailing space
			length--; 
		};
		if (source[length-1] == '"') { // closing quote found
			return(copy_string(source+1,length-2));
		};
	};
	return(copy_string(source,length));
};

//#if TT_WIDE_TEXT
// moved wide_string copy_wide_string(wide_string source, int length) to common.cpp on 310502
// and wide_string copy_wide_string(string source, int source_length)
// and void copy_wide_string(string source, int source_length, wide_string destination)
//#endif

//useless
//wide_character widen(character c) {
//	character source[1];
//	source[0] = c;
//	wide_character destination[1];
//   MultiByteToWideChar(tt_code_page,0,source,1,destination,1);
//	return(destination[0]);
//};
#if TT_WIDE_TEXT
string copy_narrow_string(wide_string source, int source_length) { // source_length is length of string
   if (source == NULL) return(NULL);
	if (source_length == 0) {
		return(copy_string("",0));
		// new on 050401 since the following doesn't seem to work quite right when length is zero (works but sets GetLastError)
	};
//   if (source_length < 0) source_length = wide_string_length(source);
//   if (length == 1) return(NULL); // good enough for NULL string?
	long destination_length = WideCharToMultiByte(tt_code_page[tt_use_spoken_alternative],0,source,source_length,NULL,0,NULL,NULL);
   string destination = new char[destination_length+1];
	WideCharToMultiByte(tt_code_page[tt_use_spoken_alternative],0,source,source_length,destination,destination_length,NULL,NULL);
   destination[destination_length] = NULL; // tried to comment out on 300803 but wasn't terminated then
   return(destination);
};
void copy_narrow_string(wide_string source, int source_length, string destination, int destination_length) { // new on 060802
	// lengths should be string lengths (not including the extra character for terminating NULL
   if (source == NULL || source_length == 0) {
      destination[0] = NULL;
      return;
   };
	WideCharToMultiByte(tt_code_page[tt_use_spoken_alternative],0,source,source_length,destination,destination_length,NULL,NULL);
	// note that callers should sent in the true destination_length not just the size of the data structure
   destination[destination_length] = NULL; // had commented out on 300803 
	// since the documentation for WideCharToMultiByte seems to indicate not needed (and the length is always the right length - can be more)
};
#endif
ascii_string copy_ascii_string(const_ascii_string source, int length) {
   if (source == NULL) return(NULL);
   if (length < 0) length = strlen(source);
//   if (length == 1) return(NULL); // good enough for NULL string?
   ascii_string destination = new char[length+1];
   memcpy(destination,source,length);
   destination[length] = NULL; 
   return(destination);
};

//#if HIGH_RESOLUTION_CLOCK
//millisecond current_time(HTimer *high_resolution_clock) {
//	return(high_resolution_clock->getElapsed()/1000);
//};
//#elif TT_WINDOWS
// if I move to utils.h and inline
// then callers need to include <windows.h>
//millisecond current_time(void *) {
//  return(timeGetTime());
//};
//#else
//const long milliseconds_per_tick = 1000L/CLK_TCK;
//
//millisecond current_time(void *) {
//  return(clock()*milliseconds_per_tick);
//};
//#endif

void sleep(millisecond duration) {
	if (tt_skip_titles) return;
#if TT_DEBUG_ON 
	if (duration < 0) {
		log(_T("Trying to sleep a negative amount."),FALSE,TRUE);
		return;
	};
	if (tt_debug_mode == 1995) {
		tt_error_file() << "Sleeping " << duration << " ms; frame = " << tt_frame_number << "; time = " << timeGetTime() << endl;
	};
#endif
	Sleep(duration);
#if TT_DEBUG_ON
	if (tt_debug_mode == 1995) {
		tt_error_file() << "Slept. Time is now " << timeGetTime() << endl;
	};
#endif
};

// have graphics to memory simulate FastGraph a bit
#if GRAPHICS_TO_MEMORY

void generic_move(int new_x, int new_y) {
   tt_memory_graphics_mark_x = new_x;
   // they disagree which way is up
   tt_memory_graphics_mark_y = tt_screen_height-new_y;
};

void generic_moverel(int new_x, int new_y) {
   tt_memory_graphics_mark_x += new_x;
   // they disagree which way is up
   tt_memory_graphics_mark_y -= new_y;
};
#endif

// 1000x the growth in increments of 1/50 seconds   
short growth[] =   // was long
  {1013, 1028, 1042, 1057, 1071, 1086, 1101, 1117, 1132, 1148, 1164, 1180,
	1197, 1214, 1231, 1248, 1265, 1283, 1301, 1319, 1337, 1356, 1375, 1394,
	1414, 1433, 1453, 1474, 1494, 1515, 1536, 1558, 1580, 1602, 1624, 1647,
	1670, 1693, 1717, 1741, 1765, 1790, 1815, 1840, 1866, 1892, 1918, 1945,
	1972, 2000, 2000};

long grow_value(long old_value, millisecond delta, millisecond double_every) {
	if (delta < 0) {
		return(old_value); // new on 250503 just in case - e.g. delta was encoded incorrectly in a log file
	};
	long result;
	if (delta <= double_every) {
		result = (long) ((((int64) old_value)*growth[(50L*delta)/double_every])/1000); 
		// added (int64) on 060202 to deal with very large objects (e.g. bignums)
	} else {
		result = 2*grow_value(old_value,delta-double_every,double_every);
	};
	if (old_value == result) { // new on 040301 can happen for example if down to a pixel in size
		return(old_value*2); // should I check that it really is small enough for this to be sensible??
	} else if (old_value > result && tt_log_version_number >= 51) { // new on 160804 to handle overflows
		tt_error_file() << "Pumpy can't expand something that is already " << old_value << " units wide or tall." << endl;
		return(old_value);
	} else {
		return(result);
	};
};

long shrink_value(long old_value, millisecond delta, millisecond double_every) {
	if (delta <= double_every) {
		return((long) ((((int64) old_value)*1000)/growth[(50L*delta)/1000])); 
		// added (int64) on 060202 to deal with very large objects (e.g. bignums)
		// on 050805 added another level of paratheses since otherwise can get 32bit overflow and negative value
	} else {
		return(shrink_value(old_value,delta-1000,double_every)/2);
	};
};

Direction direction(city_coordinate delta_x, city_coordinate delta_y) {
	// 0 is EAST, 1 is SOUTH EAST, etc.
	if (delta_x == 0) {
		if (delta_y > 0) {
			return(NORTH);
		} else {
			return(SOUTH);
		};
	};
	long ratio1000 = (delta_y*1000)/delta_x;
	if (ratio1000 < 0) ratio1000 = -ratio1000;
	if (ratio1000 > 2414) { // more than 67.5 degrees
		if (delta_y > 0) {
			return(NORTH);
		} else {
			return(SOUTH);
		};
	} else if (ratio1000 > 414) { // between 22.5 and 67.5 degrees
		if (delta_y > 0) {
			if (delta_x > 0) {
				return(NORTHEAST);
			} else {
				return(NORTHWEST);
			};
		} else {
			if (delta_x > 0) {
				return(SOUTHEAST);
			} else {
				return(SOUTHWEST);
			};
		};
	} else { // less than 22.5
		if (delta_x > 0) {
			return(EAST);
		} else {
			return(WEST);
		};
	};
};

Direction dampen_turn(Direction new_direction, Direction current_direction) {
	if (current_direction < new_direction) {
		if (new_direction-current_direction < 4) {
			// just change by 45 degrees at a time
			current_direction = (Direction) (((int) current_direction)+1);
		} else {
			current_direction = (Direction) (((int) current_direction)-1);
		};
	} else if (current_direction > new_direction) {
		if (current_direction-new_direction < 4) {
			current_direction = (Direction) (((int) current_direction)-1);
		} else {
			current_direction = (Direction) (((int) current_direction)+1);
		};
	}; 
	return((Direction) ((((int) current_direction)+8)%8));
};

boolean rubout(char key) {
	return(key == BACKSPACE); // || key == DELETE_KEY); // || key == CONTROL_BS);
};

#if !TT_STRINGS_STATIC
HINSTANCE string_library = NULL;
HINSTANCE alternative_spoken_string_library = NULL;

void load_string_library(boolean alternate) {
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
   if (alternate) {
		if (alternative_spoken_string_library != NULL) return;// already loaded
		if (tt_help_speed_percent > 0) { // added on 200800 so that it waits for PageDown button
			tt_help_speed_percent = 250; // since they are decoupled but should be roughly the same speed
		};
	} else {
		if (string_library != NULL) return; // already loaded
	};
#if TT_32
	if (tt_language == JAPANESE) {
		tt_code_page[alternate] = 932; // for conversion from SHIFT-JIS to Unicode
	};
#endif
	// commented out the following on 090799 since a better way is to make it language dependent
//	ascii_string user_provided_name =
//#if TT_32
	// new on 290499 so old and new versions can co-exist
//		ini_entry(AC(IDC_EXECUTABLES),AC(IDC_STRING_LIBRARY_DLL_32));
//#else
//		ini_entry(AC(IDC_EXECUTABLES),AC(IDC_STRING_LIBRARY_DLL_16));
//#endif
//	if (user_provided_name != NULL) {
//		string_library = load_library(user_provided_name);
//		delete [] user_provided_name;
//		if (string_library != NULL) return; // done
//	};
   char library_file_name[MAX_PATH];
//   output_string_stream file_name(library_file_name,MAX_PATH); // commented out on 041002
   boolean us_version_used;
	if (alternate) {
      strcpy(library_file_name,tt_alternative_spoken_country_code); // rewritten on 041002
      us_version_used = (stricmp(tt_alternative_spoken_country_code,"us") == 0);
//		file_name << tt_alternative_spoken_country_code; // replaces following:
	} else {
      strcpy(library_file_name,tt_country_code);
      us_version_used = (stricmp(tt_country_code,"us") == 0);
//		file_name << tt_country_code; // replaces following:
	};
/*
   switch (tt_language) {
      case SWEDISH:
         file_name << "SE";
         break;
      case BRAZILIAN_PORTUGUESE:
         file_name << "BR";
         break;
      case GERMAN:
         file_name << "DE";
         break;
      case BRITISH:
         file_name << "UK";
         break;
      default:
         if (tt_new_country_code[0] != 0) {
            file_name << tt_new_country_code;
         } else {
            file_name << "US";
         };
         break;
   };
*/
//   file_name.put('\0'); // terminate string
//	string new_library_file_name;
	// commented out the following on 010302 since want to be able to have multiple installations co-existing 
	// where each EXE knows what DLLs it should use
//	ascii_string user_provided_suffix =
//#if TT_32
//		ini_entry(AC(IDC_EXECUTABLES),"NewStringLibraryDll",FALSE); // new on 190201
//		if (user_provided_suffix == NULL) { // if not set use old setting
//			// new on 290499 so old and new versions can co-exist
//			// reinterpreted on 090799 so that this is just the suffix and language code is prefixed
//			user_provided_suffix = ini_entry(AC(IDC_EXECUTABLES),AC(IDC_STRING_LIBRARY_DLL_32),FALSE);
//		};
//#else
//		ini_entry(AC(IDC_EXECUTABLES),AC(IDC_STRING_LIBRARY_DLL_16),FALSE);
//#endif
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
//	if (user_provided_suffix != NULL) {
//		new_library_file_name = append_strings(library_file_name,AC(IDC_STRING_DLL_SUFFIX)); 
	// was user_provided_suffix prior to 010302
      strcat(library_file_name,AC(IDC_STRING_DLL_SUFFIX));
		if (alternate) {
			alternative_spoken_string_library = load_library(library_file_name,FALSE); // was new_...
         if (alternative_spoken_string_library == NULL) { // condition missing prior to 081002 (must have broken it on 041002)
            // if failed then live without it 
            tt_alternative_spoken_language = NO_LANGUAGE_SPECIFIED;
            tt_use_spoken_alternative = FALSE;
         };
		} else {
			string_library = load_library(library_file_name,FALSE);
         if (string_library == NULL && !us_version_used) {
            // try again with US version (new on 041002)
            strcpy(library_file_name,"US");
            strcat(library_file_name,AC(IDC_STRING_DLL_SUFFIX));
            string_library = load_library(library_file_name,TRUE);
         };
		};
//		delete [] new_library_file_name;
//		delete [] user_provided_suffix; // new on 270200
		//if ((!alternate && string_library != NULL) ||
		//	  (alternate && alternative_spoken_string_library != NULL)) return; // using new DLLs
//	};
	// try again with the old names -- no longer makes sense (010302)
//#if TT_32
//	string suffix = AC(IDC_TT32_DLL);
//#else
//	string suffix = AC(IDC_TT16_DLL);
//#endif
//	new_library_file_name= append_strings(library_file_name,suffix);
//	if (alternate) {
//		alternative_spoken_string_library = load_library(new_library_file_name);
//	} else {
//		string_library = load_library(new_library_file_name);
//	};
//	delete [] new_library_file_name;
};

HINSTANCE load_library(ascii_string library_name, boolean warn) {
	/* new on 160301 to only use the full existing file name to load
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	HINSTANCE library = LoadLibraryG(library_name);
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
#if TT_32
//	string_library = LoadLibraryEx(library_name,NULL,DONT_RESOLVE_DLL_REFERENCES);
	if (library != NULL) return(library);
#if TT_DEBUG_ON
	if (tt_debug_mode == 260599) message_box_last_error();
#endif
//   tt_error_file() << "Can't load dll libary " << library_name << endl
//					<< " error code = " << GetLastError() << endl;
#else
	if ((UINT) library > 32) return(library);
//		tt_error_file() << "Can't load string libary " << library_name << endl
//						<< " error code = " << (UINT) library << endl;
#endif
   // try CDROM before giving up
*/
	HINSTANCE library = NULL;
   FileNameStatus file_status;
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	MessageBox(NULL,library_name,"String Library File",MB_OK);
#endif
   ascii_string new_library_name = existing_file_name(file_status,library_name);
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
   if (new_library_name != NULL) {
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
		MessageBox(NULL,new_library_name,"String Library Full File",MB_OK);
#endif
      library = LoadLibraryG(new_library_name);
      delete [] new_library_name;
   };
#if TT_DEBUG_ON
   add_to_log_if_interesting_error(6); // above signals error number 6 when running Japanese and spoken_language=English - not clear why
#endif
#if TT_32
	if (library != NULL) return(library);
#else
	if ((UINT) library > 32) return(library);
#endif
   if (warn) { // conditional new on 041002
	   tt_exit_failure(TROUBLE_SHOOT_BAD_INSTALLATION,C(ICD_TROUBLE_LOADING_DLL));
   };
   return(NULL);
};

/*
   if (resource_library == NULL) {
       string library_file_name;
#if TT_32
       switch (tt_language) {
           case SWEDISH:
              library_file_name = "Swed32.dll";
              break;
           default:
              library_file_name = "Amer32.dll";
              break;
       };
#else
       switch (tt_language) {
           case SWEDISH:
              library_file_name = "Swed16.dll";
              break;
           default:
              library_file_name = "Amer16.dll";
              break;
       };
#endif
	    resource_library = load_library(library_file_name); 
    };
*/

#endif

void unload_string_library() {
#if !TT_STRINGS_STATIC
	if (string_library != NULL) {
      FreeLibrary(string_library);
//      if (tt_debug_mode == 4008) tt_error_file() << "Freeing string library " << (UINT) string_library << endl;
      string_library = NULL;
   };
	if (alternative_spoken_string_library != NULL) {
      FreeLibrary(alternative_spoken_string_library);
      alternative_spoken_string_library = NULL;
   };
#endif
};

HINSTANCE resource_source(boolean alternate) {
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
#if TT_STRINGS_STATIC
   return(main_instance());
#else
	if (alternate || tt_use_spoken_alternative) {
		if (alternative_spoken_string_library == NULL) { // added on 170300 since not always set so early now 
			set_spoken_language(tt_alternative_spoken_language); // no op if no language set
		};
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		return(alternative_spoken_string_library);
	} else {
		return(string_library); // using it for dialogs and the other resources too
	};
#endif
};

const int max_simultaneous_resource_strings = 16; // was 8 prior to 150500 but with space() and the like there are now occurances of more than 8

string *string_buffer = NULL;
unsigned int buffer_index = 0;
ascii_string *ascii_string_buffer = NULL;
unsigned int ascii_buffer_index = 0;

void allocate_resource_strings() {
	string_buffer = new string[max_simultaneous_resource_strings];
	int i;
	for (i = 0; i < max_simultaneous_resource_strings; i++) {
		string_buffer[i] = new character[max_resource_string_length];
	};
	ascii_string_buffer = new ascii_string[max_simultaneous_resource_strings];
	for (i = 0; i < max_simultaneous_resource_strings; i++) {
		ascii_string_buffer[i] = new char[max_resource_string_length];
	};
};

void deallocate_resource_strings() {
	int i;
	for (i = 0; i < max_simultaneous_resource_strings; i++) {
		delete [] string_buffer[i];
	};
	delete [] string_buffer;
	for (i = 0; i < max_simultaneous_resource_strings; i++) {
		delete [] ascii_string_buffer[i];
	};
	delete [] ascii_string_buffer;
};

const_string S_maybe_capitalize_first(int id, boolean capitalize, string default_string) {
	if (capitalize) {
		return(S_capitalize_first(id,default_string));
	} else {
		return(S(id,default_string));
	};
};

const_string S2_capitalize_first(int id, int index) {
  string result = (string) S2(id,index);
  if (!need_wide_characters(current_language()) && _tcslen(result) > 1) { // !need_wide_characters() added 101199
     result[0] = upper_case_character(result[0]);
  };
  return((const_string) result); // cast is OK since really using shared buffers 
};

const_string S_capitalize_first(int id, string default_string) {
  string result = (string) S(id,default_string);
  if (!need_wide_characters(current_language()) && _tcslen(result) > 1) { // !need_wide_characters() added 101199
     result[0] = upper_case_character(result[0]);
  };
  return((const_string) result); // cast is OK since really using shared buffers 
};

character upper_case_character(character c) {
	character first_letter[2];
   first_letter[0] = c;
   first_letter[1] = NULL;
   upper_case(first_letter);
	return(first_letter[0]);
};

character lower_case_character(character c) {
	character first_letter[2];
   first_letter[0] = c;
   first_letter[1] = NULL;
   lower_case(first_letter);
	return(first_letter[0]);
};

string SC(int id, int index) {
   return(copy_string(S2(id,index)));
};

const_string S2(int id, int index) {
   // string buffers are 400 bytes long so should be safe adding stuff here
   string result = (string) S(id);
   if (index >= 0) _itot(index,result+_tcslen(result),10);
   return((const_string) result);
};

const_string S2(int id, const_string suffix) {
   string result = (string) S(id);
   _tcscat(result,suffix);
   return((const_string) result);
};

#if TT_DEBUG_ON
int debugging_id = -1; 
#endif

#if TT_UNICODE
const_ascii_string AS(int id, boolean null_ok) {
#if TT_DEBUG_ON
	if (id == debugging_id) {
		log(_T("Found debugging string index."),FALSE,TRUE);
	};
#endif
	int index = ascii_buffer_index++%max_simultaneous_resource_strings;
   HINSTANCE library = resource_source();
   if (library == NULL) return(NULL); // typically this is just if it called too early
	int status = LoadStringG(library,id,ascii_string_buffer[index],max_resource_string_length);
	if (status == 0) {
		if (null_ok) return(NULL);
#if TT_DEBUG_ON
      if (tt_debug_mode == 9999) return(NULL);
#endif
	// can't put in english.dll since might recursively not find that
      switch (current_language()) {
         case SWEDISH:
				return(AC(IDC_NO_SUCH_STRING_SWEDISH));
         case GERMAN:
				return(AC(IDC_NO_SUCH_STRING_GERMAN));
			case BRAZILIAN_PORTUGUESE: // added on 310899
				return(AC(IDC_NO_SUCH_STRING_BRAZILIAN_PORTUGUESE));
			case PORTUGAL_PORTUGUESE:
				return(AC(IDC_NO_SUCH_STRING_PORTUGAL_PORTUGUESE));
			case JAPANESE:
				return("ごめん、言おうとしたことを忘れてしまった．．．");
         default:
				return(AC(IDC_NO_SUCH_STRING));
      };
      tt_error_file() << C(IDC_COULDNT_FIND_STRING) << id << "." << endl;
	} else if (status == 1 && ascii_string_buffer[index][0] == ' ') {
      // can't have an empty string so use " " instead
      return("");
   };
#if TT_DEBUG_ON
   if (tt_debug_mode == 4444 && has_initialization_completed()) {
      tt_error_file() << id << "="; // don't know why this was commented out before 220603
		tt_error_file().write(ascii_string_buffer[index],_tcslen(ascii_string_buffer[index])); // works for Unicode as well
//		tt_error_file() << endl;
   };
#endif   
	return(ascii_string_buffer[index]);
};
const_ascii_string AS2(int id, ascii_string suffix) {
   ascii_string result = (ascii_string) AS(id);
   strcat(result,suffix);
   return(result);
};
const_ascii_string AS2(int id, int index) {
   // string buffers are 400 bytes long so should be safe adding stuff here
   ascii_string result = (ascii_string) AS(id);
   if (index >= 0) itoa(index,result+strlen(result),10);
   return(result);
};
ascii_string ASC(int id, int index) {
   return(copy_ascii_string(AS2(id,index)));
};
#else
const_ascii_string S(int id, string default_string, boolean alternate) {
	// prior to 060999 default_string was a boolean indicating that "" was desired
#if TT_DEBUG_ON
	if (id == debugging_id) {
		log(_T("Found debugging string index."),FALSE,TRUE);
	};
   add_to_log_if_interesting_error();
#endif
//#if TT_SPEAK_ALL_NUMBERS
//	if (default_string != NULL && 
//		 (alternate || tt_use_spoken_alternative)?
//		  (tt_alternative_spoken_language == BRITISH || tt_alternative_spoken_language == AMERICAN):
//	     (tt_language == BRITISH || tt_language == AMERICAN)) {
//		// should be much faster -- assumes the caller doesn't clobber the result
//		// calling wordify on a number with 301030 digits was about 1.3 times faster -- not worth the risk
//		return(default_string);
//	};
//#endif
	int index = ascii_buffer_index++%max_simultaneous_resource_strings;
   HINSTANCE library = resource_source(alternate);
   if (library == NULL) return(default_string); // typically this is just if it called too early -- was "" prior to 060501
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
//	int status = 0;
//#if TT_FOR_FAR_EAST_WINDOWS
//	if (need_wide_characters()) {
//		if (SetThreadLocale(MAKELANGID(LANG_JAPANESE,SUBLANG_DEFAULT))) {
//			tt_error_file() << "Thread locale set to Japanese" << endl;
//		};
//		char id_string[10];
//		id_string[0] = '#';
//		itoa(id,id_string+1,10);
//		HRSRC resource_handle =	FindResourceEx(library,RT_STRING,id_string,MAKELANGID(LANG_JAPANESE,SUBLANG_DEFAULT));
//		if (resource_handle != NULL) {
//			HGLOBAL global_handle = LoadResource(library,resource_handle);
//			if (global_handle != NULL) {
//				string global_string = (string) LockResource(global_handle);
//				if (global_string != NULL) {
//					status = strlen(global_string);
//					memcpy(ascii_string_buffer[index],global_string,status);
//				};
//			};
//		};
//	};
//	int error_id = GetLastError();
//	if (status == 0)  // try again
//#endif
	int status = LoadStringG(library,id,ascii_string_buffer[index],max_resource_string_length);
#if TT_DEBUG_ON
   add_to_log_if_interesting_error(1814);
#endif
	// the following was just an experiment to deal with problems with LoadString on Win95/98 (non Japanese version) for Japanese
//	HRSRC resource = FindResourceG(library,(const char *) id,RT_STRING);
//	HGLOBAL resource_handle = LoadResource(library,resource);
//	string foo = (string) LockResource(resource_handle);
//	int status = 0; // resource != NULL...
	if (status == 0) {
		if (default_string != NULL) {
			// return(default_string);
			strcpy(ascii_string_buffer[index],default_string); // new on 050305 so can clobber the result safely
			return(ascii_string_buffer[index]);
		};
#if TT_DEBUG_ON
      if (tt_debug_mode == 9999) return("");
#endif
	// can't put in english.dll since might recursively not find that
      tt_error_file() << C(IDC_COULDNT_FIND_STRING) << id << "." << endl;
      switch (current_language()) {
         case SWEDISH:
				return(AC(IDC_NO_SUCH_STRING_SWEDISH));
         case GERMAN:
				return(AC(IDC_NO_SUCH_STRING_GERMAN));
			case BRAZILIAN_PORTUGUESE:
				return(AC(IDC_NO_SUCH_STRING_BRAZILIAN_PORTUGUESE));
			case PORTUGAL_PORTUGUESE:
				return(AC(IDC_NO_SUCH_STRING_PORTUGAL_PORTUGUESE));
			case JAPANESE:
            if (!alternate) {
               HINSTANCE library = resource_source(TRUE); // experiment on 110702
               if (library != NULL) {
                  return(S(id,default_string,TRUE)); // use alternate - e.g. SpokenLanguage
               };
            };
				return("ごめん、言おうとしたことを忘れてしまった．．．");
         default:
				return(AC(IDC_NO_SUCH_STRING));
       };
	} else if (status == 1 && ascii_string_buffer[index][0] == ' ') {
      if (default_string != NULL) return(default_string); 
      return(""); // can't have an empty string so use " " instead
   };
//#if TT_DEBUG_ON // removed this so translators can debug Marty's output more easily
   if (tt_debug_mode == 4444 && has_initialization_completed()) {
      tt_error_file() << id << "=";
		tt_error_file().write(ascii_string_buffer[index],_tcslen(ascii_string_buffer[index])); // works for Unicode as well
		tt_error_file() << endl;
   };
//#endif   
	return(ascii_string_buffer[index]);
};

const_ascii_string C(int id) {
	// like S(id) but uses statically linked language independent strings
#if TT_DEBUG_ON
	if (id == debugging_id) {
		log(_T("Found debugging static string index."),FALSE,TRUE);
	};
#endif
	int index = ascii_buffer_index++%max_simultaneous_resource_strings;
   HINSTANCE library = main_instance();
	int status = LoadStringG(library,id,ascii_string_buffer[index],max_resource_string_length);
	if (status == 0) {
#if TT_DEBUG_ON
		message_box_last_error();
#endif
		return("");
	};
	return(ascii_string_buffer[index]);
};

const_string C2(int id, int index) {
   // string buffers are 400 bytes long so should be safe adding stuff here
   string result = (string) C(id);
   if (index >= 0) _itot(index,result+_tcslen(result),10);
   return((const_string) result);
};


#endif

#if TT_UNICODE
const_ascii_string AC(int id) {
	// like S(id) but uses statically linked language independent strings
#if TT_DEBUG_ON
	if (id == debugging_id) {
		log(_T("Found debugging static string index."),FALSE,TRUE);
	};
#endif
	int index = ascii_buffer_index++%max_simultaneous_resource_strings;
   HINSTANCE library = main_instance();
	int status = LoadStringG(library,id,ascii_string_buffer[index],max_resource_string_length);
	if (status == 0) {
#if TT_DEBUG_ON
		message_box_last_error();
#endif
		return("");
	};
	return(ascii_string_buffer[index]);
};

const_string C(int id) {
 // returns the ascii version
	const_ascii_string entry = AC(id);
	if (entry == NULL || entry[0] == NULL) return("");
//	int length = strlen(entry);
	int index = buffer_index++%max_simultaneous_resource_strings; // use next index
   string result = string_buffer[index];
   MultiByteToWideChar(tt_code_page[tt_use_spoken_alternative],0,entry,-1,result,max_resource_string_length);
	return((const_string) result);
};
// #else handled in utils.h
#endif

NaturalLanguage current_language() {
	if (tt_use_spoken_alternative) {
		return(tt_alternative_spoken_language);
	} else {
		return(tt_language);
	};
};

NaturalLanguage spoken_language() {
	if (alternative_language_spoken()) {
		return(tt_alternative_spoken_language);
	} else {
		return(tt_language);
	};
};

boolean english_language(NaturalLanguage language) { // new on 141204
	return(language == AMERICAN || language == BRITISH);
};

boolean english() {
	return(current_language() == AMERICAN || current_language() == BRITISH);
};

boolean alternative_language_spoken() {
	// second conjunct added on 170300 since may still be initializing
	return(tt_alternative_spoken_language != NO_LANGUAGE_SPECIFIED && tt_alternative_spoken_language != tt_language 
		    && !(english_language(tt_language) && english_language(tt_alternative_spoken_language))); 
	// both english check new on 141204
};

void set_country_code(ascii_string name, boolean alternate) {
   if (alternate) {
		tt_alternative_spoken_country_code[0] = name[0];
		tt_alternative_spoken_country_code[1] = name[1];
	} else {
		tt_country_code[0] = name[0];
		tt_country_code[1] = name[1];
	};
};

NaturalLanguage string_to_language(ascii_string name, boolean alternate) {
   if (strnicmp(name,AC(IDC_AMERICAN),8) == 0) {
		set_country_code("US",alternate);
      return(AMERICAN);
   };
   if (strnicmp(name,AC(IDC_ENGLISH),3) == 0) { // checked all 7 letters prior to 170400
      if (ini_int(AC(IDC_VERSIONS),AC(IDC_ENGLISH_IS_AMERICAN),FALSE,0)) {
			set_country_code("US",alternate);
         return(AMERICAN);
      } else {
			set_country_code("UK",alternate);
         return(BRITISH);
      };
   };
   if (strnicmp(name,AC(IDC_BRITISH),7) == 0) {
		set_country_code("UK",alternate);
      return(BRITISH);
   };
   if (strnicmp(name,AC(IDC_SWEDISH),3) == 0 || strnicmp(name,"Sve",3) == 0) { // 3 and Sve new on 170400
		set_country_code("SE",alternate);
      return(SWEDISH);
   };
   if (strnicmp(name,AC(IDC_PORTUGUESE),4) == 0) { // new on 170400 to just check first 4 letters
		if (ini_int(AC(IDC_VERSIONS),AC(IDC_PORTUGUESE_IS_BRAZILIAN),FALSE,0)) {
			set_country_code("BR",alternate);
			return(BRAZILIAN_PORTUGUESE);
      } else {
			set_country_code("PT",alternate);
         return(PORTUGAL_PORTUGUESE);
      };
   };
   if (strnicmp(name,AC(IDC_GERMAN),6) == 0) {
		set_country_code("DE",alternate);
      return(GERMAN);
   };
   if (strnicmp(AC(IDC_TURKISH),name,7) == 0) {
		set_country_code("TR",alternate);
      return(TURKISH);
   };
   if (strnicmp(AC(IDC_JAPANESE),name,8) == 0) {
		set_country_code("JP",alternate);
      return(JAPANESE);
   };
   // now the for country code alternatives
	set_country_code(name,alternate);
   if (strnicmp("US",name,2) == 0) {
      return(AMERICAN);
   } else if (strnicmp("UK",name,2) == 0) {
      return(BRITISH);
   } else if (strnicmp("SE",name,2) == 0) {
      return(SWEDISH);
   } else if (strnicmp("BR",name,2) == 0) {
      return(BRAZILIAN_PORTUGUESE);
   } else if (strnicmp("DE",name,2) == 0) {
      return(GERMAN);
   } else if (strnicmp("FR",name,2) == 0) {
      return(FRENCH);
   } else if (strnicmp("TR",name,2) == 0) {
      return(TURKISH);
	} else if (strnicmp("JP",name,2) == 0) {
      return(JAPANESE);
	} else if (strnicmp("PT",name,2) == 0) {
      return(PORTUGAL_PORTUGUESE);
	} else if (strnicmp("IS",name,2) == 0) { // new on 131203
      return(HEBREW);
	} else if (strnicmp("ES",name,2) == 0) { // new on 020506
      return(SPANISH);
   };
   // so new countries can come after product is out there
//   tt_new_country_code[0] = name[0];
//   tt_new_country_code[1] = name[1];
   return(UNKNOWN_LANGUAGE);
};

void completed(Sprite *robot) {
   if (robot != NULL) {
      robot->completed();
   };
};

//void by_completed(Sprite *by) {
//   if (by != NULL && by->kind_of() == ROBOT) { 
//      ((Robot *) by)->completed());
//   };
//};

void add_completion(Robot *robot) {
	if (robot != NULL) {
		robot->add_completion();
	};
};

void action_aborted(Sprite *robot) {
	if (robot != NULL) {
		robot->action_aborted();
	};
};

void reset_action_status(Robot *robot) {
	if (robot != NULL) {
		robot->reset_action_status();
	};
};

boolean is_completed(Robot *robot) {
	return(robot != NULL && robot->is_completed());
};

boolean is_action_aborted(Sprite *robot) {
	return(robot != NULL && robot->is_action_aborted());
};

boolean is_action_in_progress(Robot *robot) {
	return(robot != NULL && robot->is_action_in_progress());
};

//void completed(ActionStatus *action_status) {
//	if (action_status != NULL) *action_status -= 1;
//};
//
//void add_completion(ActionStatus *action_status) {
//	if (action_status != NULL) {
//		*action_status += 1;
//	};
//};
//
//void action_aborted(ActionStatus *action_status) {
//	if (action_status != NULL) {
//		*action_status = -1;
//	};
//};
//
//void reset_action_status(ActionStatus *action_status) {
//	if (action_status != NULL) {
//		*action_status = 1;
//	};
//};
//
//boolean is_completed(ActionStatus *action_status) {
//	return(action_status != NULL && *action_status == 0);
//};
//
//boolean is_action_aborted(ActionStatus *action_status) {
//	return(action_status != NULL && *action_status < 0);
//};
//
//boolean is_action_in_progress(ActionStatus *action_status) {
//	return(action_status != NULL && *action_status > 0);
//};

//void completed_after(millisecond duration, ActionStatus *action_status, Sprite *sprite) { // made obsolete on 131102
//	sprite->increment_after(duration,action_status,-1);
//};


/*
const short int random_count = 100;
unsigned short random_index = 0;
unsigned short random_index2 = 0;

short int randoms[random_count] = {
346,130,10982,1090,11656,7117,17595,6415,22948,31126,9004,
14558,3571,22879,18492,1360,5412,26721,22463,25047,27119,31441,
7190,13985,31214,27509,30252,26571,14779,19816,21681,19651,
17995,23593,3734,13310,3979,21995,15561,16092,18489,11288,28466,8664,
5892,13863,22766,5364,17639,21151,20427,100,25795,8812,15108,12666,
12347,19042,19774,9169,5589,26383,9666,10941,13390,7878,13565,1779,
16190,32233,53,13429,2285,2422,8333,31937,11636,13268,6460,6458,
6936,8160,24842,29142,29667,24115,15116,17418,1156,4279,15008,
15859,19561,8297,3755,22981,21275,29040,28690,1401};
*/

unsigned long next_random_number = 1;

unsigned long random_number_seed() { // new on 120100 so that logs can record this
	return(next_random_number);
};

void set_random_number_seed(unsigned long new_seed) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 10399 || tt_debug_mode == 101) {
		tt_error_file() << "Setting random number seed to " << new_seed << " f=" << tt_frame_number << endl;
	};
#endif
	next_random_number = new_seed;
};

int my_random_old(int n) {
//   int temp = random(n);
//   tt_error_file() << temp << endl;
//   return(temp);
//	return(random(n)); // ok for now
#if TT_DEBUG_ON
	if (tt_debug_mode == 10399 || tt_debug_mode == 101) {
		tt_error_file() << "Next was " << next_random_number << " f=" << tt_frame_number << endl;
	};
#endif
	return ((next_random_number = next_random_number*1103515245L+12345)%n);
};

//+ * Compute x = (7^5 * x) mod (2^31 - 1)
//+ * wihout overflowing 31 bits:
//+ *      (2^31 - 1) = 127773 * (7^5) + 2836
//+ * From "Random number generators: good ones are hard to find",
//+ * Park and Miller, Communications of the ACM, vol. 31, no. 10,
//+ * October 1988, p. 1195.
//+ */

int my_random(int n) {
	// rewritten on 090403
	if (tt_log_version_number < 31) {
		return(my_random_old(n));
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 10399 || tt_debug_mode == 101) {
		tt_error_file() << "Next was " << next_random_number << " f=" << tt_frame_number << endl;
	};
#endif
// from http://mail.gnu.org/archive/html/avr-libc-dev/2002-09/txt00003.txt

	long hi, lo, x;

	hi = next_random_number / 127773L;
	lo = next_random_number % 127773L;
	x = 16807L * lo - 2836L * hi;
	if (x <= 0)
		x += 0x7fffffffL;
	return ((next_random_number = x) % n);
};

unsigned long next2 = 1;

int my_random2(int n) {
	// this one should be called by non-semantic callers (e.g. performance tuning)
	// need two so that replay is exact even on very different speed machines
//tt_error_file() << "Next was " << next << " f=" << tt_frame_number << " my_random2" << endl;
	return ((next2 = next2*1103515245L+12345)%n);
//	random_index2++; 
//	return(randoms[random_index2%random_count]%n);
};

int my_random_unless_tt_use_spoken_alternative(int n) {
	// new on 221199 so that when running bilingually the random number generator's behavior isn't changed
	if (tt_use_spoken_alternative) {
		return(tt_current_time%n);
	} else {
		return(my_random(n));
	};
};

void java_indent(java_stream &out) {
   for (int i = 0; i < tt_java_indent; i++) out << " ";
};

void print_as_comment(java_stream &out, const_string comment, int max_length) { // added nax_length on 260700
   const int max_comment_line_length = 75;
   int comment_remaining = _tcslen(comment);
	boolean too_long = (comment_remaining > max_length);
	if (too_long) {
	   comment_remaining = max_length;
	};
   while (comment_remaining > 0) {
      int break_at = max_comment_line_length;
      if (comment_remaining > max_comment_line_length) {
         for (int i = max_comment_line_length-1; i > 0; i--) {
            if (comment[i] == ' ' || comment[i] == '.' ||
                comment[i] == ',' || comment[i] == '?') {
               break_at = i+1;
               break;
            };
         };
      } else {
         break_at = comment_remaining;
      };
      java_indent(out);
      out << "// ";
      out.write(comment,break_at);
		if (too_long && comment_remaining == break_at) {
			out << " ...";
		};
      out << endl;
      comment += break_at;
      comment_remaining -= break_at;
   };
};

//#if TT_DEBUG_ON
//	if (tt_debug_mode == 259) {
//		unsigned long r = rand(); //random(n);
//		tt_error_file() << r << " (frame=" << tt_frame_number << ")" << endl;
//		return(r%n);
//	};
//#endif
//	return(rand()%n); //random(n));

#if TT_DIRECTX

const int first_wave_id = 1;

sound_buffer load_sound_buffer(int sound_index, unsigned long &size) {
  if (direct_sound == NULL) return(NULL);
  return(DSLoadSoundBuffer(direct_sound,
                           MAKEINTRESOURCE(first_wave_id+sound_index),
                           (DWORD *) &size));
//                           tt_main_window->get_handle()));
};


int number_of_user_sounds =0;
int max_number_of_user_sounds = 0;
string *user_sound_file_names = NULL ;
sound_buffer *user_sounds = NULL ;
const int user_sound_table_increment = 50;
int minimum_sound_index = 0; // new on 050504 so when reset there are no stale references

void grow_user_sound_table() {
	int old_max_number_of_user_sounds = max_number_of_user_sounds;
	max_number_of_user_sounds += user_sound_table_increment;
	sound_buffer *new_user_sounds = new sound_buffer[max_number_of_user_sounds];
	string *new_user_sound_file_names = new string[max_number_of_user_sounds];
	int i = 0;
	for (;i < old_max_number_of_user_sounds; i++) {
		new_user_sounds[i] = user_sounds[i];
		new_user_sound_file_names[i] = user_sound_file_names[i];
	};
	if (user_sounds != NULL) {
		delete [] user_sounds;
	};
	user_sounds = new_user_sounds;
	if (user_sound_file_names != NULL) {
		delete [] user_sound_file_names;
	};
	user_sound_file_names = new_user_sound_file_names;
};

void release_user_sound_table() {
	if (user_sounds != NULL) {
		for (int i = 0; i < number_of_user_sounds; i++) {
			release_sound_buffer(user_sounds[i]);
			user_sounds[i] = NULL; // new on 260105 -- just to be careful since one of Leonel's students had a crash
		};
		delete [] user_sounds;
		user_sounds = NULL;
	};
	if (user_sound_file_names != NULL) {
		for (int i = 0; i < number_of_user_sounds; i++) {
			delete [] user_sound_file_names[i];
			user_sound_file_names[i] = NULL; // new on 260105 -- just to be careful since one of Leonel's students had a crash
		};
		delete [] user_sound_file_names;
		user_sound_file_names = NULL;
	};
	minimum_sound_index += number_of_user_sounds;
	number_of_user_sounds = 0;
	max_number_of_user_sounds = 0;
};

sound_buffer previously_loaded_sound(int index) { // this was separated from load_sound_from_file on 210300 since stop_sound needs it
	int true_index = index-minimum_sound_index; // new on 050504
	if (true_index >= 0 && true_index < number_of_user_sounds && user_sounds[true_index] != NULL) { // already stored
		return(user_sounds[true_index]);
	};
	return(NULL);
};

// this is very much like find_image_for_file - could they share code??

sound_buffer load_sound_from_file(ascii_string file_name, int &index) {
	// changed on 201199 so buffers are shared
	// changed on 241199 so references are by index (unless not yet initialized)
	sound_buffer buffer = previously_loaded_sound(index);
	if (buffer != NULL) return(buffer);
   if (direct_sound == NULL) return(NULL);
	for (int i = 0; i < number_of_user_sounds; i++) { // this is still needed if user makes two sounds with same name
		if (user_sound_file_names[i] != NULL && strcmp(file_name,user_sound_file_names[i]) == 0) {
			index = i+minimum_sound_index; // +minimum_sound_index new on 050504
			return(user_sounds[i]);
		};
	};
	HRESULT result;
   buffer = DSLoadSoundFromFile(direct_sound,file_name,&result);
	if (buffer == NULL) {
		dserr(result,"Loading a sound");
		index = -1;
	} else {
		if (number_of_user_sounds >= max_number_of_user_sounds) {
			grow_user_sound_table();
		};
		user_sound_file_names[number_of_user_sounds] = copy_string(file_name);
		user_sounds[number_of_user_sounds] = buffer;
		index = number_of_user_sounds;
		number_of_user_sounds++;
	};
	return(buffer);
};

sound_buffer load_sound_from_memory(bytes sound_bytes) {
   if (direct_sound == NULL) return(NULL);
   return(DSLoadSoundFromMemory(direct_sound,sound_bytes));
};

void play_sound_buffer(sound_buffer sound) {
   if (sound != NULL) {
		try {
			HRESULT status = sound->Play(0,0,0);
			if (status != DS_OK) {
				tt_error_file() << "Problem playing a sound. Error status code is " << (int) status << endl;
			};
			current_sound_buffer = sound;
		} catch (...) {
			// new on 260105 since Leonel reported a crash here
			log("Internal error while playing a sound buffer.",FALSE,TRUE);
		};      
   };
};

boolean sound_buffer_playing() {
   if (current_sound_buffer == NULL) return(FALSE);
   DWORD play_cursor, write_cursor;
   current_sound_buffer->GetCurrentPosition(&play_cursor,&write_cursor);
   return(play_cursor > 0);
};

void stop_sound_buffer(sound_buffer sound) {
   if (sound != NULL && !tt_shutting_down) sound->Stop();
};

void release_sound_buffer(sound_buffer sound) {
   if (sound != NULL && direct_sound != NULL) { // abnormal termination seems call this with direct_sound closed
      sound->Release();
      if (sound == current_sound_buffer) {
         current_sound_buffer = NULL;
      };
   };
};

#endif

sound_buffer read_sound(HFILE file, long size
#if !TT_32
	, HGLOBAL &memory_handle
#endif
	 ) {
#if TT_32
	sound_buffer sound = (sound_buffer) new unsigned char[size];
#else
	memory_handle = global_alloc(GMEM_MOVEABLE,size);
	sound_buffer sound = (sound_buffer) GlobalLock(memory_handle);
#endif
   _hread(file,(bytes) sound,size);
//	sound_buffer sound_remaining = sound;
//	while (size > read_buffer_size) {
//	  _lread(file,(bytes) sound_remaining,read_buffer_size);
//	  sound_remaining += read_buffer_size;
//	  size -= read_buffer_size;
//	};
//	_lread(file,(bytes) sound_remaining,size);
	return(sound);
};
//#endif

int ini_int(const_ascii_string category, const_ascii_string id, boolean record, int default_value) {
	// updated 241199 to be "log" aware
	int answer = min_long; // was unsigned short prior to 110101
	if (record && log_in_is_open()) { // (tt_log_version_number >= 41?log_in_is_open():replaying())) { // prior to 050404 was replaying()
		// was prior to 060903 but time travel can be paused and there is no replay -- (tt_replay || tt_time_travel != TIME_TRAVEL_OFF)) { 
		//  && tt_log_version_number > 17
		answer = replay_ini_int();
	} else {
		if (tt_local_ini_file != NULL) { // new on 110101
			answer = GetPrivateProfileIntG(category,id,min_long,tt_local_ini_file);
#if TT_SPECIAL_DEBUGGING
			if (answer != min_long) {
				tt_special_debugging_file << "Read from " << tt_local_ini_file << "[" << category << "]" << id << "=" << (int) answer << endl;
			};
#endif
		};
		if (answer == min_long) {
			answer = GetPrivateProfileIntG(category,id,min_long,tt_ini_file); // default_value was 0 prior to 030500 -- changed to min_long on 020205
			if (answer == min_long) { // new on 020205 to write the default values to toontalk.ini so new 'Set Options' dialog knows about them
				answer = default_value;
				char default_value_as_string[16];
				itoa(default_value,default_value_as_string,10);
				WritePrivateProfileStringG(category,id,default_value_as_string,tt_ini_file);
			};
#if TT_SPECIAL_DEBUGGING
			if (answer != min_long) {
				tt_special_debugging_file << "Read from " << tt_ini_file << "[" << category << "]" << id << "=" << (int) answer << endl;
			};
#endif
		};
	};
	if (record && tt_logging) {
		record_ini_int(answer);
	};
#if TT_DEBUG_ON
	if (record && (tt_debug_mode == 110100 || tt_debug_mode == 160299)) {
		tt_error_file() << "ini entry for [" << category << "] " << id << "=" << answer << endl;
	};
#endif
	return(answer);
};

ascii_string ini_entry(const_ascii_string category, const_ascii_string id, boolean record) {
	// updated 241199 to be "log" aware
	ascii_string answer;
	if (record && log_in_is_open()) {
		// prior to 050404 was (tt_replay || tt_time_travel != TIME_TRAVEL_OFF)  
		answer = replay_ini_entry();
	} else {
		int max_size = 256;
		answer = new char[max_size];
		int size = 0;
		boolean from_local_ini_file = TRUE; // new on 020301
		if (tt_local_ini_file != NULL) { // new on 110101
			size = GetPrivateProfileStringG(category,id,"",answer,max_size,tt_local_ini_file);
		};
		if (size == 0) { // didn't find it in the local ini file
			from_local_ini_file = FALSE;
 			size = GetPrivateProfileStringG(category,id,"",answer,max_size,tt_ini_file);
		};
		if (size == 0) {
			delete [] answer;
	//		tt_error_file() << "Couldn't find the " << category << " " << id << " entry in TOONTALK.INI" << endl;
			answer = NULL;
		} else if (size == 1 && answer[0] == ' ') { // a space is used for "non entry"
			delete [] answer;
			answer = NULL;
		} else {
			while (size >= max_size-1) { // new on 081200 to deal with long entries
				// updated on 020301 to keep doubling
				delete [] answer;
				max_size *= 2; // new on 020301
				answer = new char[max_size];
				if (from_local_ini_file) { // was tt_local_ini_file != NULL) { // new on 110101
					size = GetPrivateProfileStringG(category,id,"",answer,max_size,tt_local_ini_file);
				} else {
					size = GetPrivateProfileStringG(category,id,"",answer,max_size,tt_ini_file);
				};
			};
		};
	};
	if (record && tt_logging) {
		record_ini_entry(answer);
	};
#if TT_DEBUG_ON
	if (record && (tt_debug_mode == 110100 || tt_debug_mode == 160299)) {
		tt_error_file() << "Replaying ini entry for [" << category << "] " << id;
		if (answer != NULL) {
		   tt_error_file() << "=" << answer << endl;
		} else {
			tt_error_file() << "=NULL" << endl;
		};
	};
#endif
	return(answer);
};

short unsigned int temp_counter = 1;

short unsigned int next_serial_number() {
   if (tt_temporary_user) {
      return(temp_counter++);
   };
	return((unsigned short) increment_ini_int(AC(IDC_SWITCHES),AC(IDS_ROBOT_COUNTER),TRUE));
};

int increment_ini_int(const_ascii_string category, const_ascii_string id, boolean record) { // proceduralized on 261100
	// since this reads and writes it ignores the tt_local_ini_file
	// added record arg so that robots get the same name when replayed - 050204
	int count;
	if (record && (replaying() || tt_time_travel != TIME_TRAVEL_OFF) && tt_log_version_number >= 33) {
		// replaying() was tt_replay prior to 300604
		ascii_string answer = replay_ini_entry();
		if (answer == NULL) return(0); // more robust that breaking -- new on 300604 (and for NoNameCounter is OK -- 140605)
		count = atoi(answer)-1; // since was incremented for saving 
		if (record && tt_logging) {
			record_ini_entry(answer);
		};
	} else {
		count = GetPrivateProfileIntG(category,id,1,tt_ini_file); // default value was 0 prior to 261100
  		char count_string[16];
		itoa(count+1,count_string,10);
  		WritePrivateProfileStringG(category,id,count_string,tt_ini_file);
		if (record && tt_logging && tt_log_version_number >= 33) {
			record_ini_entry(count_string);
		};
	};
   return(count);
};

void write_ini_entry(const_ascii_string category, const_ascii_string id, const_ascii_string value) { // new on 110903
	WritePrivateProfileStringG(category,id,value,tt_ini_file);
#if TT_SPECIAL_DEBUGGING
	tt_special_debugging_file << "Wrote in " << tt_ini_file << "[" << category << "]" << id << "=" << value << endl;
#endif
};

void write_ini_entry(const_ascii_string category, const_ascii_string id, int value) { // new on 110903
	char value_string[14]; // 32-bit integer as string
	itoa(value,value_string,10);
	WritePrivateProfileStringG(category,id,value_string,tt_ini_file);
#if TT_SPECIAL_DEBUGGING
	tt_special_debugging_file << "Wrote in " << tt_ini_file << "[" << category << "]" << id << "=" << value_string << endl;
#endif
};

//flag already_searched_for_cdrom = FALSE;
//flag already_told_user_about_cdrom = FALSE;
//const int cdrom_path_size = 13;
//char cdrom_path[cdrom_path_size] = "";

ascii_string new_file_name(const_ascii_string short_name, const_ascii_string extension) { 
	// rewritten on 040402 to be compatible with Windows XP guidelines
   ascii_string name_buffer = new char[MAX_PATH];
//   output_string_stream name(name_buffer,MAX_PATH); // rewritten 230104
//   if (tt_default_user_directory != NULL && strcmp(name,tt_file_name) == 0) { 
	// new on 060402 to deal with default user under the new scheme
	if (tt_user_directory != NULL) { // condition new on 031202
//		name << tt_user_directory;
		strcpy(name_buffer,tt_user_directory);
	} else {
		name_buffer[0] = '\0';
	};
//   } else {
//      name << tt_default_user_directory; // always right? - doesn't it depend upon whether going to read or write?
//   };
//   name << short_name;
	strcat(name_buffer,short_name);
	if (extension != NULL) {
		// name << "." << extension;
		strcat(name_buffer,".");
		strcat(name_buffer,extension);
	};
//   name.put('\0');
   return(name_buffer);
};
/*
ascii_string new_file_name(const_ascii_string subdirectory, const_ascii_string short_name, const_ascii_string extension) {
   ascii_string name_buffer = new char[MAX_PATH];
   output_string_stream name(name_buffer,MAX_PATH);
	if (tt_user_provided_user_directory != NULL && subdirectory != NULL && stricmp(subdirectory,AC(IDS_USERS_DIRECTORY)) == 0) {
		// new on 300500
		name << tt_user_provided_user_directory;
	} else {
		if (tt_main_directory != NULL) name << tt_main_directory;
      // what about a default for Users subdirectory?
		if (subdirectory != NULL) name << subdirectory << "\\";
	};
   name << short_name;
   if (extension != NULL) name << "." << extension;
   name.put('\0');
   return(name_buffer);
};
*/
boolean isCDROMDemoFile(const_ascii_string short_file_name, const_ascii_string extension) {
	if (extension == NULL) {
       return((boolean) GetPrivateProfileIntG(AC(IDC_CDROM_DEMO_FILES),short_file_name,0,tt_ini_file));
	};
   char name[MAX_PATH]; // should be short but might not be
   output_string_stream stream(name,MAX_PATH);
   stream << short_file_name << "." << extension << '\0';
   stream.put('\0');
   return((boolean) GetPrivateProfileIntG(AC(IDC_CDROM_DEMO_FILES),name,0,tt_ini_file));
};

const_ascii_string language_subdirectory(const_ascii_string subdirectory) {
	if (english() && stricmp(subdirectory,AC(IDC_DOC_DIRECTORY)) == 0) {
		return(AC(IDC_ENGLISH));
	} else {
		return(AS(IDS_PUZZLE_SUBDIRECTORY));
	};
};

boolean already_has_extension(string file_name, string *extensions, int extension_count) {
	string new_file_name = extract_file_name_from_link(file_name);
	if (new_file_name == NULL) {
		return(FALSE);
	};
	for (int i = 0; i < extension_count; i++) {
		if (already_has_extension(new_file_name,extensions[i],FALSE)) {
			delete [] new_file_name;
			return(TRUE);
		};
	};
	delete [] new_file_name;
	return(FALSE);
};

boolean already_has_extension(string file_name, string extension, boolean check_links_too) {
	if (extension == NULL || file_name == NULL) return(FALSE);
	int length = strlen(file_name);
   int extension_length = strlen(extension);
   if (length > extension_length+1) { // check if already has extension
		if (file_name[length-(extension_length+1)] == '.') {
			boolean has_extension = (stricmp(file_name+length-extension_length,extension) == 0);
			if (has_extension) return(TRUE);
			if (check_links_too) { // re-written on 091000 to deal with both LNK and URL extensions
				string new_file_name = extract_file_name_from_link(file_name);
				if (new_file_name == NULL) {
					return(FALSE);
				} else {
					boolean answer = already_has_extension(new_file_name,extension,FALSE);
					delete [] new_file_name;
					return(answer);
				};
			};
			return(has_extension);
		};
	};
	return(FALSE);
};

string exists_file_name_or_links(FileNameStatus &name_status, string file_name, 
										   const_ascii_string extension, const_ascii_string subdirectory, 
										   boolean cache_if_url, boolean ok_to_bother_user,
										   boolean language_specific_subdirectory, boolean just_narration, 
											boolean check_paths, int call_depth) {
//	OFSTRUCT file_struct;
//   if (OpenFile(file_name,&file_struct,OF_EXIST) != -1) {
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	boolean file_name_is_url = is_url(file_name);
	if (file_exists(file_name,file_name_is_url)) { // rewritten on 020301
      return(file_name);
   };
	// added attempt to find LNK or URL version of file on 091000
	string lnk_file_name = append_strings(file_name,".lnk");
//	if (OpenFile(lnk_file_name,&file_struct,OF_EXIST) != -1) {
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	if (file_exists(lnk_file_name,file_name_is_url)) { // rewritten on 020301
//		delete [] file_name; // commented out on 151000
		string true_file_name = extract_file_name_from_link(lnk_file_name);
		delete [] lnk_file_name;
		// calling existing_file_name added 121000
      return(existing_file_name(name_status,true_file_name,extension,subdirectory, 
										  cache_if_url,ok_to_bother_user,language_specific_subdirectory,just_narration,
										  check_paths,call_depth+1)); 
   };
	delete [] lnk_file_name;
	string url_file_name = append_strings(file_name,".url");
//	if (OpenFile(url_file_name,&file_struct,OF_EXIST) != -1) {
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	if (file_exists(url_file_name,file_name_is_url)) { // rewritten on 020301
//		delete [] file_name; // commented out on 151000
		string true_file_name = extract_file_name_from_link(url_file_name);
		delete [] url_file_name;
      // calling existing_file_name added 121000
		return(existing_file_name(name_status,true_file_name,extension,subdirectory, 
										  cache_if_url,ok_to_bother_user,language_specific_subdirectory,just_narration,
										  check_paths,call_depth+1)); 
   };
	delete [] url_file_name;
	int extension_start = extension_start_position(file_name);
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	if (extension_start > 0) { // new on 101000
		// file can be foo.lnk or foo.url
		strcpy(file_name+extension_start,"lnk");
//		if (OpenFile(file_name,&file_struct,OF_EXIST) != -1) {
		if (file_exists(file_name,file_name_is_url)) { // rewritten on 020301
			string true_file_name = extract_file_name_from_link(file_name);
//			delete [] file_name; // commented out on 151000
			// calling existing_file_name added 121000
			return(existing_file_name(name_status,true_file_name,extension,subdirectory, 
										     cache_if_url,ok_to_bother_user,language_specific_subdirectory,just_narration,
											  check_paths,call_depth+1)); 
		};
		strcpy(file_name+extension_start,"url");
//		if (OpenFile(file_name,&file_struct,OF_EXIST) != -1) {
		if (file_exists(file_name,file_name_is_url)) { // rewritten on 020301
			string true_file_name = extract_file_name_from_link(file_name);
//			delete [] file_name; // commented out on 151000
			// calling existing_file_name added 121000
			return(existing_file_name(name_status,true_file_name,extension,subdirectory, 
										     cache_if_url,ok_to_bother_user,language_specific_subdirectory,just_narration,
											  check_paths,call_depth+1)); 
		};
   };
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	return(NULL);
};

#if TT_MICROSOFT
string url_file_name(string url_name, boolean cache_if_url, FileNameStatus &name_status) {
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	if (tt_internet_connection != NULL) {
// commented out the following since best to check cache first if cache_if_url is TRUE
//					HINTERNET url = InternetOpenUrl(tt_internet_connection,name_buffer,NULL,0,INTERNET_FLAG_EXISTING_CONNECT,0);
//					if (url != NULL) {
//						InternetCloseHandle(url); // could optimize this since this closes connection and caller will re-open it
		if (cache_if_url) { // new on 170200
			name_status = FILE_ON_INTERNET; // _AND_DELETE_COPY; -- used anymore??
			return(cached_file_name(url_name)); 
		} else { // as of 060401 no code should go down this path
#if TT_DEBUG_ON
			add_to_log_if_interesting_error();
#endif
			HINTERNET url = InternetOpenUrl(tt_internet_connection,url_name,NULL,0,INTERNET_FLAG_EXISTING_CONNECT,0);
#if TT_DEBUG_ON
			add_to_log_if_interesting_error();
#endif
			if (url != NULL) {
				// following was 1000 prior to 050401
				const int buffer_size = 8000; // who knows what's good? - was 255 prior to 170301 but that is too small to deal well with 404 errors
				char buffer[buffer_size];
				DWORD bytes_read;
				if (InternetReadFile(url,buffer,buffer_size,&bytes_read)) { // made into a conditional on 060401
#if TT_DEBUG_ON
					add_to_log_if_interesting_error();
#endif
					// following new on 060401 - for reasons I don't understand I need to get to the end of file for this to work reliably
					char ignore_buffer[buffer_size];
					DWORD ignore_bytes_read;
					do {
						InternetReadFile(url,ignore_buffer,buffer_size,&ignore_bytes_read);
					} while (ignore_bytes_read > 0);
					InternetCloseHandle(url); // could optimize this since this closes connection and caller will typically re-open it
#if TT_DEBUG_ON
					add_to_log_if_interesting_error(6); // not clear why this generates error # 6
#endif
					if (bytes_read > 0 && !is_404_error(buffer,url_name)) {
						name_status = FILE_ON_INTERNET;
						return(url_name);
					};
				};
			};
		};
	};
	return(NULL);
};
#endif

int number_of_times_user_asked_to_insert_cdrom = 0; // new on 020301 so don't ask too many times

boolean toontalk_cdrom_available(int message_id) {
	char test_file[MAX_PATH]; // test_file should exist on all ToonTalk CD-ROMs - new on 020301
	output_string_stream file_name(test_file,sizeof(test_file));
	file_name << tt_cdrom_directory << AC(IDC_JAVA_PATH) << "UNKNOWN.GIF"; 
	file_name.put('\0');
	if (file_exists(test_file,FALSE)) {
		return(TRUE);
	} else {
		number_of_times_user_asked_to_insert_cdrom++;
		// added default value to S on 060501 so if language DLL is missing this asks for the CD-ROM
		while (message_box(MB_RETRYCANCEL|MB_ICONQUESTION,
								 S(message_id,"If you insert the ToonTalk CD-ROM now, ToonTalk will be able to continue. ")) == IDRETRY) {
			if (file_exists(test_file,FALSE)) {
				return(TRUE);
			};
		};
	};
	return(FALSE);
};

boolean is_relative_path(string name) {
	return(name[0] != '\\' && strchr(name,':') == NULL);
};

ascii_string existing_file_name_internal(FileNameStatus &name_status, const_ascii_string original_file_name_maybe_link, 
													  const_ascii_string extension, const_ascii_string subdirectory, 
													  boolean cache_if_url, boolean ok_to_bother_user,
													  boolean language_specific_subdirectory, boolean just_narration,
													  boolean check_paths, int call_depth) {
	if (original_file_name_maybe_link == NULL) return(NULL); // new on 111200 (happens with bad LNK files)
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	string new_file_name = NULL;
	boolean delete_original_file_name = FALSE;
	if (strnicmp(original_file_name_maybe_link,"file://",7) == 0) { 
		// moved here on 151000 since should happen before the following
		original_file_name_maybe_link += 7; // skip over file:// stuff -- what about localfile:// and others??
	};
	string original_file_name = extract_file_name_from_link(original_file_name_maybe_link);
	if (original_file_name == NULL) {
		return(NULL);
	};
	// if file_name is a URL and cache_if_url is FALSE then the caller must use generic file stuff in file.cpp
   name_status = FILE_ON_HARD_DRIVE; // until proven otherwise
	boolean name_includes_extension = already_has_extension(original_file_name,extension);
//	if (extension != NULL && !name_includes_extension && extension_start_position(original_file_name) > 0) { // new on 020600
		// already has an extension but it is different so ignore this attempt
//		return(NULL);
//	};
	boolean is_relative = is_relative_path(original_file_name);
	boolean name_is_url = is_url(original_file_name);
	ascii_string name_buffer = new char[MAX_PATH]; // moved out of conditional on 151000
	if (!is_relative && !name_is_url) { // name_includes_extension && strchr(original_file_name,'\\') != NULL) {
		// revised on 160200 so that this works with qualified path that is missing the extension
		// local file name - 
//		output_string_stream name(name_buffer,MAX_PATH); // rewritten on 140104
//		name << original_file_name;
		strcpy(name_buffer,original_file_name);
		if (!name_includes_extension && extension != NULL) {
			strcat(name_buffer,".");
			strcat(name_buffer,extension);
			// prior to 140104 was name << "." << extension;
		};
//		name.put('\0');
		if (file_exists(name_buffer,FALSE)) { // condition added 250100
			delete [] original_file_name;
	      return(name_buffer);
		} else if (check_paths) { // condition added on 080402 since shouldn't do this for example when looking for an ini file
//			delete [] name_buffer; // commented out on 151000
//			return(NULL);
			// new on 090500
			string short_original_file_name = short_file_name(original_file_name,(extension == NULL)); 
			// second arg added on 290103 so keeps the extension if there is no default
			delete [] original_file_name;
			original_file_name = short_original_file_name;
			is_relative = TRUE; // is now so fall thru
			if (extension != NULL) name_includes_extension = FALSE; // condition new on 290103
		};
   };
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	if (name_is_url) {
#if TT_MICROSOFT
		string name_with_extension = NULL;
		boolean delete_name_with_extension;
		// re-wrote the following on 111000 to be more modular
		if (extension != NULL && !name_includes_extension) {
			name_with_extension = append_3_strings(original_file_name,".",extension);
			delete_name_with_extension = TRUE;
		} else {
			name_with_extension = original_file_name;
			delete_name_with_extension = FALSE;
		};
		string result = url_file_name(name_with_extension,cache_if_url,name_status);
		if (result != NULL) {
			if (delete_name_with_extension && result != name_with_extension) delete [] name_with_extension;
			delete [] original_file_name;
			return(result);
		};
#endif
	};
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
   boolean using_user_directory = (tt_user_directory != NULL && subdirectory != NULL && strcmp(subdirectory,"NewUsers") == 0); 
	// new on 060402 so files can be found on old installations (prior to beta 35 or 2.50)
	// added tt_user_directory test above on 310502 for playing demos without knowing the user's name
   boolean using_old_user_directory = (!using_user_directory && subdirectory != NULL && 
													stricmp(subdirectory,AC(IDS_USERS_DIRECTORY)) == 0);
	// commented out the following on 061103 since tt_user_directory is tt_user_provided_user_directory followed by user name
//   boolean using_user_provided_user_directory = (tt_user_provided_user_directory != NULL && using_user_directory);
   char relative_path[MAX_PATH]; // everything after main or cdrom directory
//   output_string_stream path(relative_path,MAX_PATH); 
	// commented out on 230104 since either this is a leak or interferes with debugging leaks
	if (is_relative) { // new on 160200 - is not an absolute path
		if (subdirectory != NULL) {
//			!using_user_provided_user_directory && // removed 061103 
         if (using_user_directory) { // new on 060402
//            path << tt_user_directory;
				strcpy(relative_path,tt_user_directory);
         } else {
//			   path << subdirectory;
				strcpy(relative_path,subdirectory);
			   if (subdirectory[strlen(subdirectory)-1] != '\\') { // check that it doesn't already end in a slash added on 070201
//				   path << "\\";
					strcat(relative_path,"\\");
			   };
         };
		} else {
			relative_path[0] = '\0'; // new on 230104 to be sure when calling strcat below
		};
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		if (language_specific_subdirectory) {
			string directory = language_subdirectory(subdirectory); // worth renaming??
#if TT_DEBUG_ON
			add_to_log_if_interesting_error();
#endif
			if (directory != NULL) { // for robustness in case DLL doesn't indicate subdirectory - new on 161203
//				path << directory;
				strcat(relative_path,directory);
				if (directory[strlen(directory)-1] != '\\') { // condition new on 070201
//					path << "\\";
					strcat(relative_path,"\\");
				};
			};
		};
	} else {
		relative_path[0] = '\0'; // new on 230104 to be sure when calling strcat below
	};
//   path << original_file_name;
	strcat(relative_path,original_file_name);
	if (!name_includes_extension && extension != NULL) {
		// path << "." << extension;
		strcat(relative_path,".");
		strcat(relative_path,extension);
	};
//   path.put('\0');
//   ascii_string name_buffer = new char[MAX_PATH];
//   output_string_stream name(name_buffer,MAX_PATH);
	// rewritten on 140104 to simplify and also output_string_stream conses up 
	// std::_Mutex which makes it harder to track down memory leaks
	if (is_relative_path(relative_path)) { // new on 130701 because subdirectory might be full path
		//if (using_user_provided_user_directory) {
		//	name << tt_user_provided_user_directory; // new on 300500
		//} else 
		if (tt_main_directory != NULL) {
//			name << tt_main_directory;
			strcpy(name_buffer,tt_main_directory); // rewritten from above on 140104
		} else {
			// new on 140104
			name_buffer[0] = '\0';
		};
//         name << AC(IDS_USERS_DIRECTORY) << "\\";
		// new on 170302 -- this makes sense but some calls don't expect this 
		// (e.g. loading string DLL) -- so callers need to pass Users in instead
	} else {
		name_buffer[0] = '\0';
	};
//	const_ascii_string relative_path = path.STR;
//	name << relative_path;
// name.put('\0');
	strcat(name_buffer,relative_path); // re-written 140104
//	ascii_string name_buffer = (ascii_string) name.STR;
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	string answer = exists_file_name_or_links(name_status,name_buffer,extension,subdirectory, 
															cache_if_url,ok_to_bother_user,language_specific_subdirectory,just_narration,
															check_paths,call_depth); 
	if (answer != NULL) {
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		delete [] original_file_name;
		if (answer != name_buffer) delete [] name_buffer; // new on 151000
		return(answer);
	};
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
   if (tt_cdrom_directory != NULL) {
      // try cdrom now
      if (subdirectory == NULL ||
          // and not on Users or Demos (unless installation recorded it as such)
          (stricmp(subdirectory,AC(IDS_DEMO_DIRECTORY)) != 0 &&
//           stricmp(subdirectory,AC(IDC_HIGH_RES_DEMO_PATH)) != 0 &&
//           stricmp(subdirectory,AC(IDC_LOW_RES_DEMO_PATH)) != 0 && // commented out on 210300
           stricmp(subdirectory,AC(IDS_USERS_DIRECTORY)) != 0) || isCDROMDemoFile(original_file_name,extension)) {
          // might be on CDROM so check it out
 //         output_string_stream cd_name(name_buffer,MAX_PATH);
//          cd_name << tt_cdrom_directory << relative_path;
				  // rewritten 140104
			 strcpy(name_buffer,tt_cdrom_directory);
			 boolean new_name_to_check = FALSE; // new on 260304 since otherwise check if the "file" named tt_cdrom_directory exists
			 if (is_relative_path(relative_path)) { // conditional new on 080304
				 strcat(name_buffer,relative_path);
				 new_name_to_check = TRUE;
			 } else if (is_relative_path(original_file_name)) { // new on 080304
				 strcat(name_buffer,original_file_name);
				 if (extension != NULL) {
					 strcat(name_buffer,".");
					 strcat(name_buffer,extension);
				 };
				 new_name_to_check = TRUE;
			 };
//          cd_name.put('\0');
			 if (new_name_to_check) { // conditional new on 260304
				 name_status = FILE_ON_CDROM;
				 if (file_exists(name_buffer,FALSE)) {
					 delete [] original_file_name;
					 return(name_buffer);
				 };
			 };
          if (ok_to_bother_user && number_of_times_user_asked_to_insert_cdrom < 4) {  // but don't bother if on Demos but not on CDROM
				 if (toontalk_cdrom_available(just_narration?IDS_NEED_CDROM_FOR_NARRATION:IDS_NEED_CDROM_TO_CONTINUE)) {
                if (file_exists(name_buffer,FALSE)) {
						 delete [] original_file_name;
                   return(name_buffer);
                } else { // new on 020301
						 tt_error_file() << "ToonTalk expected to find the file " << name_buffer 
											  << " on the ToonTalk CD-ROM and didn't. Perhaps you have the wrong CD-ROM." << endl;
					 };
             };
				 // commented out on 180200 since taken care of after optionally check path
//          } else {
//             delete [] name_buffer;
//             return(NULL);
          };
       };
   };
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	if (check_paths) {
		for (int i = 0; i < tt_file_search_path_length; i++) { // new on 160200
//			output_string_stream path(name_buffer,MAX_PATH);
//			path << tt_file_search_path[i];
//			path << original_file_name;
//			if (!name_includes_extension && extension != NULL) path << "." << extension;
//			path.put('\0');
			// rewritten on 140104
			strcpy(name_buffer,tt_file_search_path[i]);
			strcat(name_buffer,original_file_name);
			if (!name_includes_extension && extension != NULL) {
				strcat(name_buffer,".");
				strcat(name_buffer,extension);
			};
#if TT_DEBUG_ON
			add_to_log_if_interesting_error();
#endif
			if (is_url(name_buffer)) {
#if TT_MICROSOFT
				string result = url_file_name(name_buffer,cache_if_url,name_status);
#if TT_DEBUG_ON
				add_to_log_if_interesting_error();
#endif
				if (result != NULL) {
					delete [] original_file_name;
					if (name_buffer != result) delete [] name_buffer;
					return(result);
				};
#endif
			} else {
//				OFSTRUCT file_struct;
//				if (OpenFile(name_buffer,&file_struct,OF_EXIST) != -1) {
//					delete [] original_file_name;
//					return(name_buffer);
//				};
				// this deletes name_buffer if appropriate
#if TT_DEBUG_ON
				add_to_log_if_interesting_error();
#endif
				string answer = exists_file_name_or_links(name_status,name_buffer,extension,subdirectory, 
																	   cache_if_url,ok_to_bother_user,language_specific_subdirectory,
																		just_narration,check_paths,call_depth);
#if TT_DEBUG_ON
				add_to_log_if_interesting_error();
#endif
				if (answer != NULL) {
					if (answer != original_file_name) delete [] original_file_name;
					if (is_url(answer)) {
#if TT_MICROSOFT
						string result = url_file_name(answer,cache_if_url,name_status);
#if TT_DEBUG_ON
						add_to_log_if_interesting_error();
#endif
						if (result != NULL) {
							if (answer != result) {
								delete [] answer;
								return(result);
							};
						};
#endif
					};
					return(answer);
				};
			};
		};
	};
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
   delete [] original_file_name;
   if (using_user_directory) { // new on 060402
		delete [] name_buffer; // new on 160301
      return(existing_file_name_internal(name_status,original_file_name_maybe_link, 
													  extension,AC(IDS_USERS_DIRECTORY), // use old location
													  cache_if_url,ok_to_bother_user,
													  language_specific_subdirectory,just_narration,
													  FALSE,call_depth)); // already checked paths
   };
   if (ok_to_bother_user && !just_narration) {
		// could imagine 3 states for ok_to_bother_user where new one just logs the following message...
      character message_buffer[512];
      output_string_stream message(message_buffer,512);
      const_string warning_file = S(IDS_WARNING_FILE);
      if (warning_file == NULL || warning_file[0] == NULL) { // disjunct added on 170300 so if haven't finished initializing can still proceed
         warning_file = C(IDC_COULD_NOT_OPEN_FILE);
      };
      const_string couldnt_open = S(IDS_COULDNT_OPEN);
      message << warning_file << space() << original_file_name_maybe_link; // was name_buffer prior to 090600 but that might have stuff from FileSearchPath
		if (extension != NULL && !already_has_extension(original_file_name_maybe_link,extension)) {// prior to 300103 was !name_includes_extension 
		   message << "." << extension; // new on 031100
		};
		if (couldnt_open != NULL) message << space() << couldnt_open; // might be too early to get strings
      message.put('\0');
		message_box(MB_ICONINFORMATION,message_buffer);
   };
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	delete [] name_buffer;
   return(NULL);
};

ascii_string existing_file_name(FileNameStatus &name_status, string original_file_name, 
										  const_ascii_string extension, const_ascii_string subdirectory, 
										  boolean cache_if_url, boolean ok_to_bother_user,
										  boolean language_specific_subdirectory, boolean just_narration, boolean check_paths, 
										  int call_depth) {
	if (call_depth > 4) return(NULL); // recurring too deep with links so give up - new on 121000
   if (tt_want_exception_breaks) {
	   return(existing_file_name_internal(name_status,original_file_name, 
												     extension,subdirectory, 
												     cache_if_url,ok_to_bother_user,
												     language_specific_subdirectory,just_narration,check_paths,call_depth));
   } else {
	   try {
		   return(existing_file_name_internal(name_status,original_file_name, 
													     extension,subdirectory, 
													     cache_if_url,ok_to_bother_user,
													     language_specific_subdirectory,just_narration,check_paths,call_depth));
	   } catch (...) {
		   tt_error_file() << "Had some difficulties finding a file: " << original_file_name;
		   if (extension != NULL) {
			   tt_error_file() << " with extension " << extension;
		   };
		   tt_error_file() << endl;
		   return(NULL); // new on 221299
	   };
   };
};

ascii_string existing_file_name(FileNameStatus &name_status, const_ascii_string original_file_name, 
										  string extensions[], int extension_count, const_ascii_string subdirectory, 
										  boolean cache_if_url, boolean ok_to_bother_user,
										  boolean language_specific_subdirectory, boolean just_narration, boolean check_paths, 
										  int call_depth) {
	if (already_has_extension(original_file_name,extensions,extension_count)) {
		return(existing_file_name(name_status,original_file_name,NULL,
										  subdirectory,cache_if_url,ok_to_bother_user,language_specific_subdirectory,just_narration,
										  check_paths));
	} else {
		for (int i = 0; i < extension_count; i++) {
			string full_file_name = existing_file_name(name_status,original_file_name,extensions[i],
																	 subdirectory,cache_if_url,ok_to_bother_user,
																	 language_specific_subdirectory,just_narration,check_paths,call_depth);
			if (full_file_name != NULL) { // found one
				return(full_file_name);
			};
		};
	};
	return(NULL);
};

string expand_question_mark(string directory) { // new on 191204 -- so UserFiles can reference MainDir
	int length = strlen(directory);
	char main_directory[MAX_PATH];
	if (tt_main_directory == NULL) {
		// if not (maybe am setting it right now) then ? means folder where EXE lives
		::GetModuleFileName(NULL,main_directory,MAX_PATH);
		int directory_end = short_file_name_start(main_directory);
		if (directory_end > 0) {
			main_directory[directory_end] = '\0';
		} else {
			return(directory); // pretty helpless -- warn??
		};
	} else {
		 strcpy(main_directory,tt_main_directory);
	};
	int main_dir_length = strlen(main_directory);
	int new_length = length+1+main_dir_length;
	int user_directory_length = 0;
	string new_directory = new char[new_length];
	strcpy(new_directory,main_directory);
	int skip = 1; // to skip the ? itself
	if (directory[1] == '\\') {
		skip++; // and to skip \ (since tt_main_directory has a slash)
	};
	strcat(new_directory,directory+skip);
//	delete [] directory; // caller will do this
	return(new_directory);
};

string add_final_slash_if_needed(string directory) {
	if (directory == NULL) return(directory); 
	if (directory[strlen(directory)-1] != '\\') { // needs to end in a slash
		string temp = append_strings(directory,"\\");
//		delete [] directory; // commented out on 160103
		directory = temp;
	};
	if (directory[0] == '?') { // new on 191204 to expand the MainDir
		return(expand_question_mark(directory));
	};
	return(expand_asterisk(directory));
};

string expand_asterisk(string directory) { // made into a procedure on 010601
	if (strchr(directory,'*') != NULL && tt_file_name != NULL) { 
		// new on 110100 to allow * (meaning replace with tt_file_name) - second conjunct added on 010301
		int length = strlen(directory);
		int tt_file_name_length = strlen(tt_file_name);
		int new_length = length+1+tt_file_name_length;
		int user_directory_length = 0;
		boolean relative = (strchr(directory,':') == NULL && directory[0] != '\\'); // relative if no colon and doesn't start with a slash
		if (relative) { 
			user_directory_length = strlen(tt_user_directory)-(1+tt_file_name_length); // +1 to remove final slash
			new_length += user_directory_length;
		};
		string new_directory = new char[new_length];
		int j = 0;
		for (int i = 0; i < length; i++) {
			if (directory[i] == '*') {
				if (relative) {
					strncpy(new_directory+j,tt_user_directory,user_directory_length);
					j += user_directory_length;
				};
				strncpy(new_directory+j,tt_file_name,tt_file_name_length);
				j += tt_file_name_length;
			} else {
				new_directory[j++] = directory[i];
			};
		};
		new_directory[j] = '\0';
		delete [] directory;
		directory = new_directory;
	};
	return(directory);
};

boolean copy_file_to_toontalk_directory(const_ascii_string full_file_name, const_ascii_string sub_directory, 
													 const_ascii_string short_name, const_ascii_string extension) {
	// new on 260201
	char new_file_name[MAX_PATH];
	output_string_stream path(new_file_name,MAX_PATH);
	path << tt_main_directory;
	if (sub_directory != NULL) {
		path << sub_directory << "\\";
	};
	path << short_name;
	if (extension != NULL) {
		path << "." << extension;
	};
	path.put('\0');
	boolean ignore;
	return(copy_file_or_url_to_file(full_file_name,new_file_name,ignore));
};

boolean copy_system_file_to_directory(const_ascii_string short_file_name, const_ascii_string sub_directory, 
												  const_ascii_string destination_directory) {
	// new on 060402
   FileNameStatus status;
   string full_file_name = existing_file_name(status,short_file_name,NULL,sub_directory,FALSE,TRUE,FALSE,FALSE,FALSE);
   if (full_file_name == NULL) {
      return(FALSE); // warn?
   };
	char new_file_name[MAX_PATH];
	output_string_stream path(new_file_name,MAX_PATH);
	path << destination_directory << short_file_name;
	path.put('\0');
	boolean ignore;
	boolean result = copy_file_or_url_to_file(full_file_name,new_file_name,ignore);
   delete [] full_file_name;
   return(result);
};

string canonicalize_path(string path) { // new on 050301 to remove spaces from the ends and to simplify paths with "\.." in them (what about /..?)
	int path_length = strlen(path);
	string new_path = new char[MAX_PATH]; // prior to 020401 was [path_length+1]; // can only get shorter or stay the same
	boolean start = TRUE;
	int j = 0;
	for (int i = 0; i < path_length; i++) {
		if (path[i] == ' ') {
			if (start) { 
				// do nothing
			} else {
				boolean all_spaces = TRUE;
				for (int k = i+1; k < path_length; k++) {
					if (path[k] != ' ') {
						all_spaces = FALSE;
						break;
					};
				};
				if (all_spaces) { // done
					break;
				};
				new_path[j++] = path[i]; // new on 221101
			};
		} else {
			start = FALSE; // new on 221101 to deal with spaces in directory names
			boolean dealt_with_slash_dot_dot = FALSE;
			if (path[i] == '\\') {
				if (i+2 < path_length && path[i+1] == '.' && path[i+2] == '.') { // remove previous subdirectory
					for (int k = j; k > 0; k--) {
						if (new_path[k] == '\\') {
							j = k;
							new_path[k] = '\0'; // so we don't find this again if there are two \..s
							i += 2;
							dealt_with_slash_dot_dot = TRUE;
							break;
						};
					};
				};
			};
			if (!dealt_with_slash_dot_dot) {
				new_path[j++] = path[i];
			};
		};
	};
	new_path[j] = '\0';
	return(new_path);
};

string process_directory_entry(string entry) { // new on 050301
	if (entry == NULL) return(entry);
	string stage1 = add_final_slash_if_needed(entry);
	string stage2 = canonicalize_path(stage1);
	if (stage1 != entry) { // conditional new on 160103
		delete [] stage1;
	};
	delete [] entry; // new on 160103 - make sure all callers are sending in a fresh string!
	return(stage2);
};

void get_my_documents_folder(string path) { // abstracted on 050504
	if (!get_special_folder(CSIDL_PERSONAL,path)) {
//#if TT_DEBUG_ON // make for release as well on 050504
		// new on 310104 -- should this be for Release version as well?
		tt_error_file() << "Failed to get_special_folder of CSIDL_PERSONAL. This probably means this machine has no folder for 'My Documents'" << endl;
		tt_error_file() << "Creating a My Documents folder under the main ToonTalk directory instead." << endl;
		if (tt_main_directory != NULL) {
			strcpy(path,tt_main_directory);
			strcat(path,"My Documents");
		} else {
			strcpy(path,"C:"); // need something
		};
//#endif
	};
};

void output_last_error(int error_code, output_stream &stream) { // abstracted on 101004
	stream << S(IDS_WINDOWS_ERROR_CODE,"The Windows error code number is") << space() << error_code;
	// following copied from Microsoft documentation of FormatMessage
	LPVOID lpMsgBuf;
	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
							FORMAT_MESSAGE_FROM_SYSTEM | 
							FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL,
							error_code,
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
							(LPTSTR) &lpMsgBuf,
							0,
							NULL)) {
		stream << space() << S(IDS_DESCRIBED_BY_WINDOWS_AS_FOLLOWS,"and is described by Windows as follows:") <<  space() << endl;
		stream << (string) lpMsgBuf; // seems to include empty lines at end already
	};
	// Free the buffer.
	LocalFree( lpMsgBuf );
};

void set_directory_names() {
	release_directory_names(); // new on 110101 since might be re-computed
	if (tt_main_directory != NULL) {
		delete [] tt_main_directory; // new on 130104 to fix a minor leak
		tt_main_directory = NULL; // since now support ? expansion need to know if this is set or not
	};
#if TT_DEBUG_ON
    add_to_log_if_interesting_error(0,"beginning of set_directory_names");
#endif
   tt_main_directory = process_directory_entry(ini_entry(AC(IDC_DIRECTORIES),AC(IDS_MAIN_DIR),FALSE));
#if TT_DEBUG_ON
    add_to_log_if_interesting_error(0,"set_directory_names after tt_main_directory");
#endif
   tt_cdrom_directory = process_directory_entry(ini_entry(AC(IDC_DIRECTORIES),AC(IDS_CDROM_DIR),FALSE));
	tt_URL_cache_directory = process_directory_entry(ini_entry(AC(IDC_DIRECTORIES),"URLCacheDir",FALSE));
	tt_clipping_directory = process_directory_entry(ini_entry(AC(IDC_DIRECTORIES),AC(IDC_CLIPPING_DIR),FALSE)); // added 110699
	tt_temp_directory = process_directory_entry(ini_entry(AC(IDC_DIRECTORIES),AC(IDS_TEMP_DIR),FALSE));
	tt_user_provided_user_directory = process_directory_entry(ini_entry(AC(IDC_DIRECTORIES),"UserFiles",FALSE));
	if (tt_user_provided_user_directory != NULL) { // new on 011004 so Crash Reports end up in this directory
		strcpy(tt_toontalk_crash_directory,tt_user_provided_user_directory);
		strcat(tt_toontalk_crash_directory,"Crash Reports\\");
		::CreateDirectory(tt_toontalk_crash_directory,NULL);
	} else {
		::CreateDirectory(tt_toontalk_crash_directory,NULL); // new on 191204 since don't do it earlier anymore
	};
	tt_picture_directory = process_directory_entry(ini_entry(AC(IDC_DIRECTORIES),"PictureDir",FALSE)); 
	tt_builtin_picture_directory = process_directory_entry(ini_entry(AC(IDC_DIRECTORIES),"BuiltinPictureDir",FALSE));
	if (tt_builtin_picture_directory == NULL) { 
		// new on 080304 to have default be the Java dir so if ini file is "defective" all is well
		tt_builtin_picture_directory = append_strings(tt_main_directory,"Java\\");
	};
	// moved here so this is available for use if TT_NO_ROBOT_TRAINING
//#if TT_ALPHA_FEATURE -- commented out on 280104 since needed for new DMO zip scheme
	if (tt_extracted_file_temp_directory == NULL) { // condition new on 130104 to fix a minor leak
		tt_extracted_file_temp_directory = process_directory_entry(ini_entry(AC(IDC_DIRECTORIES),"ExtractedFileTempDir",FALSE)); 
		// new on 281003
		if (tt_extracted_file_temp_directory == NULL) {
			tt_extracted_file_temp_directory = new char[MAX_PATH];
			if (tt_user_provided_user_directory != NULL) {
				// new on 011004 to use tt_user_provided_user_directory if it is set
				strcpy(tt_extracted_file_temp_directory,tt_user_provided_user_directory);
				strcat(tt_extracted_file_temp_directory,"Temporary File Cache\\");
			} else {
#if TT_DEBUG_ON
				add_to_log_if_interesting_error(0,"set_directory_names before get_special_folder");
#endif
				get_my_documents_folder(tt_extracted_file_temp_directory); // abstracted on 050504
				if (!get_special_folder(CSIDL_PERSONAL,tt_extracted_file_temp_directory)) {
//#if TT_DEBUG_ON // make for release as well on 050504
					// new on 310104 -- should this be for Release version as well?
					tt_error_file() << "Failed to get_special_folder of CSIDL_PERSONAL. This probably means this machine has no folder for 'My Documents'" 
										 << endl;
					tt_error_file() << "Creating a My Documents folder under the main ToonTalk directory instead." << endl;
					strcpy(tt_extracted_file_temp_directory,tt_main_directory);
					strcat(tt_extracted_file_temp_directory,"My Documents");
//#endif
				};
#if TT_DEBUG_ON
				add_to_log_if_interesting_error(1008,"set_directory_names after get_special_folder");
				// not clear why the above has an error code of 1008
#endif
				strcat(tt_extracted_file_temp_directory,"\\ToonTalk\\Temporary File Cache\\");
			};
			::CreateDirectory(tt_extracted_file_temp_directory,NULL);
		};
	};
//#endif
	tt_private_media_directory = process_directory_entry(ini_entry(AC(IDC_DIRECTORIES),"MediaDir",FALSE)); // new on 170303
#if TT_DEBUG_ON
   add_to_log_if_interesting_error(0,"set_directory_names after tt_private_media_directory");
#endif
	if (tt_private_media_directory == NULL) {
#if TT_NO_ROBOT_TRAINING
		tt_private_media_directory = copy_string(tt_extracted_file_temp_directory);
#else
		tt_private_media_directory = new char[MAX_PATH];
		// set directory under shared documents
		// prior to 190408 following was CSIDL_COMMON_DOCUMENTS but
		// that doesn't work for limited users (without write access to All Users)
		if (!get_special_folder(CSIDL_PERSONAL|CSIDL_FLAG_CREATE,tt_private_media_directory)) { 
			get_my_documents_folder(tt_private_media_directory); // updated 050504
			//if (!get_special_folder(CSIDL_PERSONAL,tt_private_media_directory)) { // new on 261003
			//	// removed \\ from after c: on 241003 since the next instruction adds the backslash
			//	strcpy(tt_private_media_directory,"C:"); // shouldn't happen but if so need somewhere to put these files
			//};
      };
		strcat(tt_private_media_directory,"\\ToonTalk\\"); 
		if (!::CreateDirectory(tt_private_media_directory,NULL)) {
			// error handling new on 101004
			int error_code = GetLastError();
			if (error_code != 0 && error_code != 183) { // OK if already exists
				tt_error_file() << "Unable to create a directory named " << tt_private_media_directory 
									 << " to store pictures and sounds." << endl;
				output_last_error(error_code,tt_error_file());
//				tt_err_file_important = TRUE; // commented out on 091104
				// new as of 091104 to give up on Shared Documents and use My Documents instead
				if (tt_user_provided_user_directory != NULL) {
					strcpy(tt_private_media_directory,tt_user_provided_user_directory);
				} else {
					get_my_documents_folder(tt_private_media_directory);
				};
				strcat(tt_private_media_directory,"\\ToonTalk\\Shared Documents\\");
				if (!::CreateDirectory(tt_private_media_directory,NULL)) {
					int error_code = GetLastError();
					if (error_code != 0 && error_code != 183) { // OK if already exists
						tt_error_file() << "Unable to create a directory named " << tt_private_media_directory 
											 << " to store pictures and sounds." << endl;
						output_last_error(error_code,tt_error_file());
						tt_err_file_important = TRUE;
					};
				};
			};
		};
#if TT_DEBUG_ON
		add_to_log_if_interesting_error(0,"set_directory_names after CreateDirectory tt_private_media_directory");
#endif
#endif
	};
#if !TT_NO_ROBOT_TRAINING
	strcat(tt_private_media_directory,media_directory); 
	// these moved out of the conditonal above so that even user specified directories get a Media subdirectory
	if (!::CreateDirectory(tt_private_media_directory,NULL)) {
		// error handling new on 101004
		int error_code = GetLastError();
		if (error_code != 0 && error_code != 183) {
			tt_error_file() << "Unable to create a directory named " << tt_private_media_directory 
								 << " to store pictures and sounds." << endl;
			output_last_error(error_code,tt_error_file());
			tt_err_file_important = TRUE;
		};
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error(0,"set_directory_names after CreateDirectory tt_private_media_directory 2");
#endif
#endif
	tt_private_media_directory_length = strlen(tt_private_media_directory);
	const int max_media_directory_length = MAX_PATH-(hash_string_length+4); // +4 for the extension
	if (tt_private_media_directory_length > max_media_directory_length) { 
		tt_error_file() << "MediaDir is too long. Maximum length is " << max_media_directory_length << ". Truncated. " << endl;
		tt_private_media_directory[max_media_directory_length-1] = '\0';
		tt_private_media_directory_length = max_media_directory_length;
	};
};

void release_directory_names() {
#if TT_DEBUG_ON
    add_to_log_if_interesting_error(0,"beginning of release_directory_names");
#endif
   if (tt_cdrom_directory != NULL) delete [] tt_cdrom_directory;
//#if !TT_ALPHA_FEATURE // moved on 021103 -- commented out on 280104 since is now part of set_directory_names
//   if (tt_main_directory != NULL) delete [] tt_main_directory;
//#endif
	if (tt_URL_cache_directory != NULL) delete [] tt_URL_cache_directory;
	if (tt_clipping_directory != NULL) delete [] tt_clipping_directory;
	if (tt_temp_directory != NULL) delete [] tt_temp_directory;
	if (tt_user_provided_user_directory != NULL) delete [] tt_user_provided_user_directory; // moved here on 300500 from cleanup
	if (tt_picture_directory != NULL) delete [] tt_picture_directory;
	if (tt_builtin_picture_directory != NULL) delete [] tt_builtin_picture_directory;
	if (tt_private_media_directory != NULL) delete [] tt_private_media_directory;
};

void set_ini_file(string ini_file) { // new on 030304
	tt_ini_file = ini_file;
	// new on 030304 to find local ini file in toontalk.ini
	//if (tt_local_ini_file != NULL) { // commented out on on 060404 to support -local_ini_file command line option
	//	delete [] tt_local_ini_file;
	//};
	if (tt_local_ini_file == NULL) { // conditional new on 060404 so not overridden here
		tt_local_ini_file = ini_entry("Executables","UseThisIniFileFirst",FALSE);
	};
	tt_local_ini_file_uncached_name = copy_string(tt_local_ini_file); // needed in any case
	// FALSE added on 050404 since this would break demos that were paused and an applet generated (and default was in wrong position)
	tt_ini_file_only_from_tt_ini_file = !ini_int("Switches","UseLocalIniFilesWhenLoading",FALSE,!tt_ini_file_only_from_tt_ini_file); 
	// so old behavior is still available
};

ascii_string copy_to_temp_directory(const_ascii_string source_name, const_ascii_string original_name) {
	if (tt_temp_files_count >= tt_temp_files_max) {
		tt_error_file() << AC(IDC_TOO_MANY_TEMPORARY_FILES) << endl;
		return(copy_ascii_string(source_name)); // shouldn't really happen
	};
	if (tt_temp_directory == NULL) { // this means we'll read off the CDROM
//		tt_error_file() << "Can't find temporary file directory." << endl;
		return(copy_ascii_string(source_name));  // don't copy file
	};
	ascii_string temp_name = new char[MAX_PATH];
	strcpy(temp_name,tt_temp_directory);
	strcat(temp_name,original_name);
	if (tt_temp_files != NULL) tt_temp_files[tt_temp_files_count++] = temp_name;
	if (file_exists(temp_name,FALSE)) {
		// already copied -- maybe left over from an aborted previous execution
//      delete [] temp_directory;
		return(temp_name);
	};
   loading_wait(TRUE);
	/* haven't used this for ages
	int last_char_location = strlen(source_name)-1;
	char last_char = source_name[last_char_location];
	source_name[last_char_location] = '_'; // used for compression
	OFSTRUCT source_struct;
	if (OpenFile(source_name,&source_struct,OF_EXIST) == -1) { // oh well
		source_name[last_char_location] = last_char; // restored
	};
	*/
	boolean ignore;
   boolean status = copy_file_or_url_to_file(source_name,temp_name,ignore);
   done_waiting_for_load();
	if (status) { // went ok
//      delete [] temp_directory;
		return(temp_name);
	} else {
		if (GetPrivateProfileIntG(AC(IDC_SWITCHES),AC(IDC_NO_TEMP_DIR_WARNING_GIVEN),1,tt_ini_file)) {
			string message = new character[512];
			_tcscpy(message,S(IDS_TROUBLES_COPYING_FROM_CDROM));
			_tcscat(message,a2u(tt_temp_directory));
			_tcscat(message,S(IDS_TROUBLES_COPYING_FROM_CDROM2));
			_tcscat(message,S(IDS_TROUBLES_COPYING_FROM_CDROM3));
			message_box(MB_ICONINFORMATION|MB_OK,message);
			delete [] message;
			write_ini_entry(AC(IDC_SWITCHES),AC(IDC_NO_TEMP_DIR_WARNING_GIVEN),"0");
		};
//#if TT_DEBUG_ON
//      tt_error_file() << "CopyLZFile from " << source_name << " to " << "destination; error code = " << status << endl;
//#endif
//		source_name[last_char_location] = last_char; // restored
		strcpy(temp_name,source_name);
//      delete [] temp_directory;
		return(temp_name);
	};
};

#if !TT_32
enum FilePosition {FILE_BEGIN,FILE_CURRENT,FILE_END};
#endif

long get_file_size(HFILE file) {
//   long position = _llseek(file,0,1);  // save position
   long answer = _llseek(file,0,FILE_END); // go to end
   _llseek(file,0,FILE_BEGIN);
   return(answer);
};

/*
long copy_lz_file(const_ascii_string source_name, const_ascii_string destination_name) {
   OFSTRUCT source_struct;
   OFSTRUCT destination_struct;
	HFILE source = LZOpenFile((ascii_string) source_name,&source_struct,OF_READ);
	HFILE destination = LZOpenFile((ascii_string) destination_name,&destination_struct,OF_CREATE);
	LZStart();
	long status = CopyLZFile(source,destination); // will work whether compressed or not
	LZClose(source);
	LZClose(destination);
	LZDone();
   return status;
};
*/

boolean launch_help(const_ascii_string short_htm_file_name) {
//   if (tt_shutting_down) return(FALSE);
   FileNameStatus file_status;
	// don't cache if URL since browser will deal with it
	string htm;
	if (tt_language == BRAZILIAN_PORTUGUESE) { // new on 230300
		htm = "html";
	} else {
		htm = AC(IDC_HTM);
	};
   ascii_string htm_file_name = existing_file_name(file_status,short_htm_file_name,htm,AC(IDC_DOC_DIRECTORY),FALSE,FALSE,TRUE); 
	// last FALSE was TRUE prior to 200201
   if (htm_file_name != NULL) {
      if (!about_to_quit()) { // was tt_shutting_down prior to 280700 but that caused problems on Win 2000
   	   pause_toontalk(FALSE,FALSE);
         CloseWindow(tt_main_window->get_handle());
      };
      int status = (int) ShellExecuteG(NULL,AC(IDC_OPEN),htm_file_name,"","",SW_SHOWNORMAL); // was SW_SHOW prior to 130601
      delete [] htm_file_name;
      if (status <= 32) { // had some problems
         message_box(MB_ICONINFORMATION,S(IDS_TROUBLES_LAUNCHING_BROWSER));
      };
      return(TRUE);
	} else { // new on 200201 to check www.toontalk.com
		char htm_file_name[MAX_PATH];
		output_string_stream path(htm_file_name,MAX_PATH);
		string web_site_url = ini_entry("Directories","HTMLFiles",FALSE);
		if (web_site_url == NULL) {
			path << "http://www.toontalk.com/";
			string directory = language_subdirectory(AC(IDC_DOC_DIRECTORY)); 
			if (directory != NULL) {
				int length = strlen(directory);
				if (directory[length-1] == '\\') { // how could this happen?
					directory[length-1] = '\0';
				};
				path << directory << "/";
			};
		} else {
			path << web_site_url;
			if (web_site_url[strlen(web_site_url)-1] != '/') {
				path << "/";
			};
			delete [] web_site_url;
		};
		path << short_htm_file_name << "." << htm;
		path.put('\0');
		int status = (int) ShellExecuteG(NULL,AC(IDC_OPEN),htm_file_name,"","",SW_SHOW);
      if (status <= 32) { // had some problems
         message_box(MB_ICONINFORMATION,S(IDS_TROUBLES_LAUNCHING_BROWSER));
      };
      return(TRUE);
   };
   return(FALSE);
};

int HTML_message_box(string message, UINT style, string trouble_shooting_ini_key) {
   // this covers the current usage of message_box (as of 030602)
   char style_string[1024];
   style_string[0] = '\0';
	byte style_byte = (byte) (style&0x7); // new on 300304 since old test mistook MB_RETRYCANCEL for MB_YESNO
	if (style_byte == MB_YESNO) { // was style&MB_YESNO
      strcat(style_string,"YESNO;");
	} else if (style_byte == MB_RETRYCANCEL) { 
      strcat(style_string,"RETRYCANCEL;");
   } else {
      strcat(style_string,"OK;");
   };
   if (style&MB_ICONSTOP) {
      strcat(style_string,"ICONSTOP;");
   } else if (style&MB_ICONEXCLAMATION) {
      strcat(style_string,"ICONEXCL;");
   } else if (style&MB_ICONINFORMATION) {
      strcat(style_string,"ICONINFO;");
   } else if (style&MB_ICONQUESTION) {
      strcat(style_string,"ICONQUES;");
   } else {
      strcat(style_string,"NO ICON;");
   };
   if (message != NULL) { // do this last in case it has semicolons inside
      // can't use strcat because need to turn newlines into <br>
      int message_length = strlen(message); // what about Japanese?
      int style_string_index = strlen(style_string);
      for (int i = 0; i < message_length; i++) {
         if (!new_line(message[i])) { // was  != '\r' prior to 090802
            style_string[style_string_index++] = message[i];
         } else { // two <br>
            style_string[style_string_index++] = '<';
            style_string[style_string_index++] = 'b';
            style_string[style_string_index++] = 'r';
            style_string[style_string_index++] = '>';
            style_string[style_string_index++] = '<';
            style_string[style_string_index++] = 'b';
            style_string[style_string_index++] = 'r';
            style_string[style_string_index++] = '>';
         };
      };
      style_string[style_string_index] = '\0'; // terminate string
   };
   string result = NULL;
	if (trouble_shooting_ini_key != NULL) { // new on 131203
		result = show_html_dialog_named_in_ini_file("HTMLMessageBoxCheckBox",style_string);
	};
	if (result != NULL) {
		string check_box = strchr(result,',');
		if (check_box != NULL) {
			if (check_box[1] == 't') { // never again checked
				write_ini_entry("NeverShowTheseDialogsAgain",trouble_shooting_ini_key,1);
			};
			check_box[0] = '\0'; // terminate for the following
		};
	} else { // else if no never_again_check_box or if the dialog isn't installed or available
		result = show_html_dialog_named_in_ini_file("HTMLMessageBox",style_string);
	};
   if (result == NULL || dialog_closed(result)) return(-1);
   if (strcmp(result,"OK") == 0) return(IDOK);
   if (strcmp(result,"YES") == 0) return(IDYES);
   if (strcmp(result,"NO") == 0) return(IDNO);
   if (strcmp(result,"RETRY") == 0) return(IDRETRY);
   if (strcmp(result,"CANCEL") == 0) return(IDCANCEL);
   return(-1);
};

int message_box(UINT style, const_string message1, const_string message2, boolean restore_toontalk_afterwards, string trouble_shooting_ini_key) {
	if (message1[0] == '\0') { // new on 060501 so never see question without text
		return(0); // I guess this is always OK
	};
#if TT_DEBUG_ON
   if (tt_debug_mode == 1954 || tt_debug_mode == 101 || tt_debug_mode == 130502) {
      tt_error_file() << "Message box: " << message1;
		if (message2 != NULL) tt_error_file() << message2;
		tt_error_file() << endl;
   };
#endif
	string message;
	if (message2 == NULL) {
		message = (string) message1;
	} else {
		message = new character[strlen(message1)+strlen(message2)+1]; // was 1024 prior to 110702
		_tcscpy(message,message1);
		_tcscat(message,message2);
	};
	boolean already_paused = currently_paused(); // new on 080500 since a TT file can be clicked on while minimized and then triggers a message box
	boolean too_early_to_restore;
   if (exclusive_screen_desired()) { // tt_using_dispdib == 1) { // old behavior fine for full-screen mode
		too_early_to_restore = !has_initialization_completed();
	} else {
		too_early_to_restore = !has_directx_been_initialized();
		//  has_directx_been_initialized() was has_initialization_completed() prior to 130401 but the palette sometimes was messed up
	};
#if TT_DEBUG_ON
   if (tt_debug_mode == 130502) {
      tt_error_file() << "In message_box already_paused is " << (int) already_paused 
                  << " and too_early_to_restore is " << (int) too_early_to_restore
                  << " and restore_toontalk_afterwards is " << (int) restore_toontalk_afterwards << endl;
   };
#endif
	if (!already_paused && !too_early_to_restore && restore_toontalk_afterwards) { 
      pause_toontalk(FALSE,FALSE);
      //   tt_main_window->close_window(); // somehow message box was iconized too
   } else {
      restore_toontalk_afterwards = FALSE; // since it is too early anyway
		// but need to be sure this is dealt with
		style |= MB_SYSTEMMODAL; // new on 080600
   };
	if (!tt_shutting_down && tt_main_window != NULL && !too_early_to_restore) { // second condition added on 070900 in case not yet initialized
		// third condition added on 280201
		flip_to_GDI_surface(); // new on 110704 so that when in full-screen mode and not restoring afterwards that the dialog 
		// (e.g. to see the log) isn't under the full screen
		CloseWindow(tt_main_window->get_handle()); // moved out of the conditional above
	};
	/* Don't know how to get the right font to be used by MessageBox ...
#if TT_UNICODE
  coordinate width = tile_width/3;
  coordinate height = (3*tile_height)/2;
  const flag fixed_width = FALSE;
  const flag bold = FALSE;
  // get Windows not DirectX device context to set font of message box
  HDC main_device_context = tt_main_window->get_windows_device_context(); 
   LOGFONT logical_font;
   memset((bytes) &logical_font,0,sizeof(logical_font));
   if (width == 0) width = 1;
   if (height == 0) height = 1;
   if (fixed_width) { // seems to fit better...
      logical_font.lfHeight = height;
      logical_font.lfWidth = width;
   } else {	// 72 points to an inch but 96 pixels
      logical_font.lfHeight = (72*height)/96;
      logical_font.lfWidth = (72*width)/96;
   };
   if (bold) {
      logical_font.lfWeight = FW_BOLD;
   } else {
      logical_font.lfWeight = FW_NORMAL;
   };
   if (fixed_width) {
		logical_font.lfPitchAndFamily = FIXED_PITCH|FF_MODERN;
   } else {
		logical_font.lfPitchAndFamily = VARIABLE_PITCH|FF_SWISS;
   };
   if (fixed_width) {
		strcpy(logical_font.lfFaceName,tt_fixed_font_name);
   } else {
      strcpy(logical_font.lfFaceName,tt_variable_font_name);
	};
   logical_font.lfOutPrecision = OUT_DEFAULT_PRECIS;
   logical_font.lfClipPrecision = CLIP_DEFAULT_PRECIS;
   logical_font.lfQuality = DEFAULT_QUALITY;
   logical_font.lfCharSet = DEFAULT_CHARSET; // might try ANSI_CHARSET
	SelectObject(main_device_context,CreateFontIndirect(&logical_font));
  int result = MessageBoxW(NULL,message,L"ToonTalk",style);
  tt_main_window->deselect_font(); 
#else
  */
	boolean showed_cursor = show_cursor(TRUE,TRUE); // new on 171099 -- added second arg on 110702 so always get cursor back
	int result = HTML_message_box(message,style,trouble_shooting_ini_key);
   if (result < 0) { // didn't work so fall back on old technology
      result = MessageBox(NULL,message,C(IDC_MESSAGE_BOX_TITLE),style|MB_SETFOREGROUND); // MB_SETFOREGROUND  new on 300599
   };
   if (message2 != NULL) delete [] message; 
	// moved here on 280201 -- only if needed to append strings was message created by this procedure
	if (too_early_to_restore) return(result); // new on 280201
	if (showed_cursor) {
		show_cursor(FALSE);
	};
//#endif
	if (restore_toontalk_afterwards && !tt_shutting_down) {
      unpause_toontalk();
   };
	if (!about_to_quit() && restore_toontalk_afterwards) { // was tt_shutting_down prior to 080600
		// second condition added on 220900 since can get DirectX errors after trouble shooting dialog
		OpenIcon(tt_main_window->get_handle()); 
		// was in the above conditional prior to 080600 but messages during initialization were handled poorly
	};
	set_back_and_front_surface_palette(); // new on 050600 - not clear this helps any...
//   tt_main_window->open_icon();
	return(result);
};

void set_touched_a_remote(ChangeFrequency new_frequency) {
	if (tt_touched_a_remote < new_frequency) {
		tt_touched_a_remote = new_frequency;
	};
	if (new_frequency == CAN_CHANGE_TO_MATCH_FAILURE) {
		tt_touched_a_remote_that_can_change_to_match_failure = TRUE;
	};
};

int const line_length = 72;
int const line_length_half = line_length/2;

//static const char	b64chars[] =
//    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; 
// on 150200 replaced final / with - to avoid file:// mailer problems
/*   0000000000000000111111111111111122222222222222223333333333333333	*/
/*   0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF	*/

static const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-"; 
// made const (rather than static RC) on 160703

HGLOBAL uuencode(const_string encoding, long encoding_length, SpriteType type) {
   const int max_description = 10000;
   int description_length = 0;
   const_string description = NULL;
	boolean whole_background = (type == WHOLE_FLOOR || type == WHOLE_HOUSE || type == WHOLE_CITY);
	if (!whole_background) {
//	   input_string_stream encoding_in(encoding,encoding_length);
		StringInputStream encoding_in(encoding,encoding_length);
#if TT_NEW_IO
		output_string_stream description_stream;
#else
	   description = new char[max_description];
	   output_string_stream description_stream(description,max_description);
#endif
      tt_current_output_stream = &description_stream; // only used within calls below this
      tt_current_output_stream_size = max_description;
  	   describe_encoding((SpriteType) encoding_in.get(),&encoding_in,description_stream,latest_version_number);
		tt_current_output_stream = NULL; // new on 021204
#if TT_NEW_IO
		description = description_stream.STR;
#endif
		description_length = description_stream.LENGTH;
#if TT_DEBUG_ON
	   if (tt_debug_mode == 9876) {
		   for (int bug = 0; bug < encoding_length; bug++) {
			   tt_error_file() << (int) encoding[bug] << endl;
		   };
		   tt_error_file() << "Encoding done." << endl;
      };
#endif
	};
	long total_length = 2*encoding_length+5000+(10*description_length)/9;
	// add a little bit for extra stuff and description line break overhead
	HGLOBAL text_handle = global_alloc(GHND,total_length);
	if (text_handle == NULL) return(NULL); // warn??
	string clipboard_text = (string) GlobalLock(text_handle);
	output_string_stream stream(clipboard_text,total_length);
   if (description != NULL) {
      print_clipboard_explanation(stream,type,description,description_length);
#if !TT_NEW_IO
	   delete [] description;
#endif
   };
	const_string start_token = S(IDS_BASE64_START_TOKEN);
   stream.put((unsigned char) ('1'+latest_version_number-8)); // starts off version 1 
   stream.put((unsigned char) ('A'+tt_language)); // prior to Aug 19 was '@'
	stream << start_token << endl; // new on August 19 '98
#if TT_BASE_64_ENCODING
	long temp = 0;
	long bits = 0;
	long  linelen = 0;
//	string b64chars = AC(IDC_BASE64_CHARACTERS); -- replaced by a const on 160703
	for (long i = 0; i < encoding_length; i++) {
		// use MIME BASE 64 standard now (Aug 19 '98)
		// following was based on a post by 
      // Reece R. Pollack reece@eco.twg.com 
      // Date: 1998/08/14 on comp.os.vms
		 long octet = encoding[i];
		 temp <<= 8;
		 temp |= 0xff & octet;	
		 bits += 8;	
		 while( bits >= 6 ) {	    
			 bits -= 6;
			 stream.put(b64chars[ 0x3f & (temp >> bits)]);
			 if( ++linelen >= line_length ) {		
				 stream.put( '\n');		
				 linelen = 0;
			 };
		 };
	};
	if( bits > 0 ) {	
		temp <<= 6 - bits;
		stream.put( b64chars[ 0x3f & temp ]);	
		if( bits == 4 ) {
			stream.put('=');
		} else if( bits == 2 ) {	    
			stream.put('=');
			stream.put('=');	
		};	
		stream.put('\n');
	};
	stream.put('.'); // anything after this will be ignored when decoding
#else
	long column = strlen(start_token);
	for (long i = 0; i < encoding_length; i++) {
		 // the following used characters that some mail readers mishandle
		unsigned char byte = encoding[i];
		boolean extra_bit = (boolean) (byte>>7);
		if (extra_bit) {
			byte = (unsigned char) (byte&127); // mask off that bit
		};
		boolean control_character = FALSE;
		boolean space = FALSE;
		if (byte < 33) { // 32 (space) is moved as well
			control_character = TRUE;
			byte += (unsigned char) 33;
		} else if (byte == 127) {
//			control_character = TRUE;
//			byte = 32; // space
			// this was changed on Aug 14 '98 since spaces are not respected by all email readers (e.g. Outlook Express)
			space = TRUE;
			byte = 'E'; // can be any ordinary ASCII character
		};
		stream.put(byte);
		stream.put((char) ('A'+extra_bit+2*control_character+4*space));
		column += 2;
		if (i == encoding_length-1) { // very end
			stream.put((char) '.');
			stream.put((char) '.');
			column += 2;
		};
		if (column >= line_length-1) {
			stream << endl;
			column = 0;
		};
	};
#endif
//	for (int j = 0; j < line_length_half-(i%line_length_half); j++) {
//		stream.put((char) 33);
//		stream.put((char) 33);
//	};
	stream << endl;
   if (!(type == WHOLE_FLOOR || type == WHOLE_HOUSE || type == WHOLE_CITY)) {
      stream << S(IDS_UUENCODING_FOOTER) << endl;
   };
	GlobalUnlock(text_handle);
	return(text_handle);
};

void print_clipboard_explanation(output_stream &stream, SpriteType type, string description, int description_length) {
	if (type == WHOLE_FLOOR) {
      stream << S(IDS_RESULT_OF_SAVING_ROOM) << endl;
	} else if (type == WHOLE_HOUSE) {
		stream << S(IDS_RESULT_OF_SAVING_HOUSE,"This is the result of saving a house.") << endl;
	} else if (type == WHOLE_CITY) {
		stream << S(IDS_RESULT_OF_SAVING_CITY,"This is the result of saving a city.") << endl;
   } else {
		if (type != BIRD && type != NEST) { // condition new on 050600 so that can paste network birds into instant messages.
			stream << S(IDS_UUENCODING_HEADER) << endl;
			stream << S(IDS_UUENCODING_HEADER2) << endl;
			stream << S(IDS_UUENCODING_HEADER3) << endl;
			stream << S(IDS_UUENCODING_HEADER4) << endl;
			stream << S(IDS_UUENCODING_HEADER5) << endl;
			stream << S(IDS_UUENCODING_HEADER6) << endl;
			stream << S(IDS_UUENCODING_HEADER7) << endl;
			stream << S(IDS_UUENCODING_HEADER8) << endl;
		}; 
		// could report guid if bird or nest has one here...
//	stream << S(IDS_FOLLOWING_IS) << S(IDS_IN_CODE);
//	stream << S(IDS_IT_TAKES) << (encoding_length+line_length_half-1)/line_length_half;
      if (description_length > 0) { // new on 010802
	      const_string preamble = S(IDS_FOLLOWING_IS);
	      stream << "   " << preamble;
	      int j = line_length-_tcslen(preamble);
	      long i;
//	int lines_left = 4; // maximum description
// following looped when given a cubby label longer than line_length
// fixed on 140599 by check that j>i etc.
	      for (i = 0; i < description_length-line_length;) {
		      if (i != 0) j = i+line_length;
		      while (description[j] != ' ' && j > i) j--; // move back to first space
			   if (j == i) j = i+line_length; // give up and break it after line_length characters
//			   stream << " "; // new on 150200 since some characters on 
		      stream.write(description+i,j-i);
//		      tt_error_file() << "Lines left " << lines_left << " size is " << j-i << endl;
		      stream << endl;
		      i = j+1;
//		      lines_left--;
//		      if (lines_left == 0) break;
	      };
// following version doesn't break long lines so they fit within line length
//		   for (i = 0; i < description_length-line_length;) {
//		      stream.write(description+i,line_length);
//			   i += line_length;
//		   };
//	   if (description_length-i < line_length) {  // just one left
	      if (i < description_length) {  // just one left
		      stream.write(description+i,description_length-i);
//	   } else {
//		   int j = i+line_length-3;
//		   while (description[j] != ' ') j--; // move back to first space
//		   stream.write(description+i,j-i);
//		   stream << "..";
	      };
	      stream << "." << endl;
      };
  	};
   if (type != WHOLE_FLOOR && type != WHOLE_HOUSE && type != WHOLE_CITY && type != BIRD && type != NEST) { 
      // bird and nest conditons added 090600
      stream << S(IDS_WHAT_FOLLOWS_IS_CODE) << endl
             << C(IDS_TOONTALK_VERSION_NUMBER) << ". " 
				 << S(IDS_TOONTALK_WEB_PAGE) << endl;
   };
};

void print_binary_number(output_stream &message, long value, int seperator_every, char seperator,
                         boolean leading_zeros) {
	for (int i = 31; i >= 0; i--) {
		long bit = value>>i;
      if (bit&1) {
			message << "1";
         leading_zeros = TRUE; // not really "leading"
      } else {
			if (leading_zeros) message << "0";
      };
      if (leading_zeros && i != 0 && i%seperator_every == 0) message << seperator;
   };
};

boolean white_space_only_at_ends(const_string text, int length) { // new on 010903
	boolean non_white_found = FALSE;
	boolean non_white_space_ok = TRUE;
	for (int i = 0; i < length; i++) {
		boolean white_space = (text[i] == ' ' || text[i] == '\t' || text[i] == '\n' || text[i] == '\r');
		if (!white_space) {
			if (non_white_space_ok) {
				non_white_found = TRUE;
			} else {
				return(FALSE);
			};
		} else {
			if (non_white_found) {
				non_white_space_ok = FALSE;
			};
		};
	};
	return(TRUE);
};

Sprite *string_to_sprite(const_string text) {
   if (text[0] == NULL) return(NULL); // new on 030402 since can be an empty tt file (to indicate launch 'Free Play')
   Number *result = NULL;
   int length = strlen(text); // was _tcslen prior to 270103
   if (text[0] == '0' && text[1] == NULL) {
      result = new NUMBER();
      result->set_long_value(0); // was just set_value prior to 050803
   } else if (white_space_only_at_ends(text,length)) { // condition new on 010903
//    long number = atoi(text);
#if TT_MULTIPLE_PRECISION
      Rational *rational = new Rational();
		if (rational->set_value_as_string(text,length)) {
			result = rational;
		} else {
			rational->destroy();
      };
#else
      long number = 0;
      int j = 0;
      string text_copy = new character[length+1];
      // in Swedish for example just copy
      const_string separator_string = S(IDS_THOUSANDS_COMMA);
      character separator = separator_string[0];
      // will be ' ' for Swedish -- ok??
      for (int i = 0; i < length; i++) {
         if (text[i] != separator && 
				 (text[i] != ',' || tt_language != BRITISH)) { // comma is not separator for UK but could occur
				text_copy[j++] = text[i];
			};
      };
      text_copy[j] = NULL; // terminate the string
      input_string_stream stream(text_copy);
      stream >> number;
      boolean non_number = (stream == NULL); // didn't read a number
      // if there is some non-white space characters left it's not a number
      while (TRUE) {
         int ch = stream.get();
         if (ch == EOF || stream == NULL) break;
         if (ch != ' ' && ch != '\r' && ch != '\t' && ch != '\n') {
            non_number = TRUE;
            break;
         };
      };
      if (!non_number) {
         result = new NUMBER();
         result->set_value(number);
      };
      delete [] text_copy;
#endif
   };
   if (result == NULL) { // not a number either
		Text *result = new Text();
		result->set_use_variable_width_font(TRUE);
//		result->set_text(text);
      result->set_wide_text(canonicalize_new_lines(text,length));
		result->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL); // new on 161103
		return(result);
   } else {
//      result->update_text_string();
		result->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL); // new on 161103
      return(result);
   };
};

Rational *rational_for_testing_string_as_number = NULL; 

boolean can_be_rational_number(string text, int length) { // new on 010704
	if (text == NULL) return(FALSE);
	if (length < 0) {
		length = strlen(text);
	};
	if (rational_for_testing_string_as_number == NULL) {
		rational_for_testing_string_as_number = new Rational();
	};
	return(rational_for_testing_string_as_number->set_value_as_string(text,length));
};

void release_rational_number_for_string_testing() { // new on 010704
	if (rational_for_testing_string_as_number != NULL) {
		rational_for_testing_string_as_number->destroy();
	};
};

Sprite *uudecode_sprite(const_string encoding_string, boolean original_language, 
								boolean ok_for_string_to_be_text_or_number, boolean floors_or_houses_ok) {
   if (encoding_string == NULL) return(NULL);
#if TT_XML
	boolean warn; // new on 181204
   string xml_string = strstr(encoding_string,"<?xml"); // should this be wcsstr and use wide_string??
   if (xml_string != NULL) {
		// rewritten on 091203 to deal with double pastings and extra stuff better
		const char end_tag[] = "ToonTalkObject"; // so this matches either <ToonTalkObject ...> or </ToonTalkObject>
		const int length_of_end_tag = sizeof(end_tag);
		string remaining = xml_string;
		int nesting_depth = 0;
		while ((remaining = strstr(remaining,end_tag)) != NULL) { 
			if (remaining[-1] == '<') { // opening tag
				nesting_depth++;
			} else if (remaining[-1] == '/') { // closing tag
				nesting_depth--;
				if (nesting_depth == 0) { // done
					remaining[length_of_end_tag] = '\0'; // terminate it after the closing tag
					break;
				} else if (nesting_depth < 0) { // found a closing tag without finding an opening 
					break;
				};
			};
			remaining += length_of_end_tag;
		};
		warn = TRUE; // if there are parse errors
		//// if nesting_depth is non zero then try whole thing - maybe am in a CDATA section	
  //    const char end_tag[] = "</ToonTalkObject>";
		//const int length_of_end_tag = sizeof(end_tag);
  //    string end_string = xml_string; // strstr(xml_string,end_tag);
		//string next_end_string; // = end_string+length_of_end_tag;
		//while ((next_end_string = strstr(end_string,end_tag)) != NULL) { // keep looking for the last one - new on 220103
		//	end_string = next_end_string+length_of_end_tag;	
		//};
  //    if (end_string != NULL && next_end_string != NULL) {
		// second condition added on 040803 so if there is no ToonTalkObject it may still work   
  //       end_string[0] = '\0'; // terminate it to remove anything afterwards
  //    };
   } else {
      xml_string = encoding_string; // try it anyway - user may have selected a piece
		warn = FALSE; // new on 181204 so parse errors aren't generated here
   };
	boolean aborted = FALSE; // new on 140105
   Sprite *sprite = xml_load_sprite(xml_string,aborted,NULL,floors_or_houses_ok,warn); 
	// floors_or_houses_ok new arg as of 110803
//	release_non_networked_nests_with_guid(); 
	// new on 091202 - so no accidental sharing of nests pasted at different times - needed elsewhere too?
   if (sprite != NULL && sprite->kind_of() == PICTURE) {
      Picture *picture = (Picture *) sprite;
      if (!picture->is_flipped()) {
         picture->save_size(); // Geometry is identical to GeometryWhenFreeAndUnflipped
      };
   };
   if (sprite != NULL || aborted) {
      return(sprite);
   };
#endif
   long encoding_length;
   NaturalLanguage language;
   int version_number;
   string encoding = uudecode(encoding_string,encoding_length,language,version_number);
   if (encoding == NULL || 
		 (!floors_or_houses_ok && (encoding[0] == WHOLE_FLOOR || encoding[0] == WHOLE_HOUSE || encoding[0] == WHOLE_CITY))) {
		if (ok_for_string_to_be_text_or_number) {
			return(string_to_sprite(encoding_string));
		} else {
			return(NULL);
		};
   };
	// following moved up on 280404 so that the clipboard sensor doesn't repeatedly try to decode this
	//if (!floors_or_houses_ok && (encoding[0] == WHOLE_FLOOR || encoding[0] == WHOLE_HOUSE || encoding[0] == WHOLE_CITY)) { 
	//	// new on 080200
	//	return(NULL);
	//};
   if (!original_language) language = tt_language;
 //  input_string_stream encoding_stream(encoding,encoding_length);
	StringInputStream encoding_stream(encoding,encoding_length);
   Sprite *item = load_item(&encoding_stream,version_number,language);
   delete [] encoding;
   return(item);
};

Sprite *encoding_to_sprite(bytes encoding, int encoding_size, int version_number, NaturalLanguage language) {
//	input_string_stream encoding_stream((char *) encoding,encoding_size);
	StringInputStream encoding_stream((string) encoding,encoding_size);
   return(load_item(&encoding_stream,version_number,language));
};

string uudecode(const_string encoding_string, long &length, NaturalLanguage &language, int &version_number) {
	if (encoding_string == NULL || strlen(encoding_string) < 3) return(NULL); // new on 240101
	const_string search = S(IDS_BASE64_START_TOKEN);
	string start = _tcsstr(encoding_string+2,search);
	boolean base_64_encoding = (start != NULL);
   if (!base_64_encoding) {
		search = S(IDS_UUENCODING_START_TOKEN)+1; // try old scheme
		start = _tcsstr(encoding_string+1,search);
		if (start == NULL) return(NULL);
	};
 	int search_length = _tcslen(search);
	if (base_64_encoding) {
		version_number = start[-2]-'1'+8; // safe to use negative index since started search +2 from start 
		// 1 means release 1 which is notebook version 8
		language = (NaturalLanguage) (start[-1]-'A'); // changed to avoid "dangerous" characters
	} else {
      version_number = start[-1]-'0'; 
	};
   // forgot to update token when moving from 4 to 5 so can't distinguish between them...
	start += search_length;
	if (!base_64_encoding) {
		language = (NaturalLanguage) (start[0]-'@'); // @ used for backwards compatibility
	};
	if (language == NO_LANGUAGE_SPECIFIED || language == UNKNOWN_LANGUAGE) {
      language = tt_language;
   } else if (language < 0 || language >= LANGUAGE_COUNT) { // warn??
      language = tt_language;
   };
   start++;
//	while (start[0] != '\n') start++; // skip to next line
//	start++;
	long size = _tcslen(start);
	string encoding = new character[size]; // size is more than enough
	length = 0;
   if (base_64_encoding) {
		// following was based on a post by 
		// Reece R. Pollack reece@eco.twg.com 
		// Date: 1998/08/14 on comp.os.vms
		long flushing = 0;
		long temp = 0;
		long bits = 0;
		for (long i = 0; i < size; i++) {
			long ch = start[i];
			if( !isspace(ch) ) {
				if (ch == '.') {
				  return(encoding); // ignore the rest
				} else if( flushing ) {
   			  if( ch != '=' ) {
#if TT_DEBUG_ON
						log(_T("Clipboard: Illegal character in BASE-64 encoding of EOF padding." ));
#endif
						say_error(IDS_COULDNT_RECOGNIZE_NOTEBOOK_FILE,TRUE);
						delete [] encoding;
						return(NULL);
				};
			  } else if( ch == '=' ) {
				  flushing = 1;		
			  } else {		    
				  temp <<= 6;
				  int sextet;
				  if( ('A' <= ch) && (ch <= 'Z') )	sextet = ch - 'A';
				  else if( ('a' <= ch) && (ch <= 'z') )	sextet = (ch - 'a') + 26;
				  else if( ('0' <= ch) && (ch <= '9') )	sextet = (ch - '0') + 52;
				  else if( ch == '+' )	sextet = 62;  
				  // added ch == '-' on 150200 since // turns into file:// on many mailers
				  else if( ch == '/' || ch == '-')	sextet = 63;
				  else {
#if TT_DEBUG_ON
					  log(_T("Clipboard: Illegal character in BASE-64 encoding."));
#endif
					  tt_error_file() << AC(IDS_BAD_CLIPBOARD_CHARACTER_IS) << (char) ch 
									  << " which is character number " << i << " in the encoding." << endl;
					  say_error(IDS_COULDNT_RECOGNIZE_NOTEBOOK_FILE,TRUE);
					  delete [] encoding;
					  return(NULL);
				  };
				  temp |= sextet;		    
				  bits += 6;		    
				  if( bits >= 8 ) {
					  bits -= 8;			
					  encoding[length++] = (unsigned char) (0xff & (temp >> bits));    
				  };
			  };
			};
		};
		return(encoding);
	} else {
		long j = 0;
		for (long i = 0; i < size; i += 2) {
  			if (start[i] == 13) i++; // skip over carriage return -- Netscape at least inserts these
			if (start[i] == '\n') i++; // skip over new lines
			unsigned char byte = start[i];
			char code = start[i+1];
			switch (code) {
				case 'A':
					break; // no change
				case 'B':
					byte |= 128;
					break;
				case 'C':
					if (byte == 32) {
						byte = 127;
					} else {
						byte -= (unsigned char) 33;
					};
					break;
				case 'D':
					if (byte == 32) { // old scheme (prior to Aug 14 '98)
						byte = 127;
					} else {
						byte -= (unsigned char) 33;
					};
					byte |= 128;
					break;
				case 'E': // added on Aug 14 '98
					byte = 127;
					break;
				case 'F': // added on Aug 14 '98
					byte = 255;
					break;
				case '.': // done
	//				length = i/2-1;
					return(encoding);
				default:
					say_error(IDS_COULDNT_RECOGNIZE_NOTEBOOK_FILE,TRUE);
			};
			encoding[j++] = byte;
			length++;
		};
		delete [] encoding;
		return(NULL);
	};
};

boolean being_dumped_together(Sprite *x, Sprite *y) {
   if (tt_dumping_background == NULL) {
      return(x->ultimate_top_level_container() == y->ultimate_top_level_container()); // prior to 090205 called ultimate_container
   } else {
		if (tt_dumping_background == tt_city) {
			return(TRUE); // everything is being dumped
		// re-ordered these 2 clauses on 220803 since if dumping city doesn't matter who is in a vacuum, right?
		} else if (x->inside_toolbox_vacuum() || y->inside_toolbox_vacuum()) {
			return(FALSE);
		} else {
			// changed on 150499 so that if on flipside of a picture on the front
			// and also on the flipside of the same picture then this counts
			return(x->member_of_background(tt_dumping_background) &&
				    y->member_of_background(tt_dumping_background));
//			return(x->pointer_to_background() == y->pointer_to_background() &&
//					 x->pointer_to_background() == tt_dumping_background);
		};
	};
};

HGLOBAL global_alloc(UINT fuAlloc, DWORD cbAlloc) {
	HGLOBAL result = GlobalAlloc(fuAlloc,cbAlloc);
	if (result == NULL) {
		if (out_of_heap(0)) { // freed some memory
			return(global_alloc(fuAlloc,cbAlloc)); // try again
		};
	};
	return(result);
};

flag exit_in_progress = FALSE;

BOOL out_of_heap(size_t ) {
  if (tt_cache_percent == 0) {
     if (exit_in_progress) return(FALSE);
     exit_in_progress = TRUE;
//	  say("Out of memory!");
	  message_box(MB_OK|MB_ICONSTOP,S(IDS_OUT_OF_MEMORY));
	  standard_exit(TRUE);
	  abort();
	  return(FALSE); // abort makes this academic
  } else {
	  if (tt_cache_percent > 20) {
		  tt_cache_percent -= (short int) 20; // take away 20 percentage point
		  // say_error(IDS_MEMORY_LOW,TRUE);
		  say(IDS_MEMORY_LOW,3,TRUE); // rewritten on 070305 since may be a user error
	  } else {
		  tt_error_file() << S(IDS_MEMORY_LOW_1) << tt_cache_percent
								<< S(IDS_MEMORY_LOW_2) << endl;
		  //say_error(IDS_MEMORY_LOW2,TRUE);
		  say(IDS_MEMORY_LOW2,99,TRUE); // rewritten on 070305 since may be a user error
		  tt_cache_percent = 0;
	  };
	  free_all_cache_entries();
//	  free_memory_cache(); // flush the whole thing
//	  initialize_memory_cache();
  };
//  memory_released = flush_image_cache();
  return(TRUE);
};

PlateState border_plate(PlateState plate_state, LeftRightTopBottom side) {
	switch (plate_state) {
		case TEXT_PAD_WITH_PLATE:
			switch (side) {
				case TOP_SIDE:
				case BOTTOM_SIDE:
					return(TEXT_PAD_HORIZONTAL);
				default:
					return(TEXT_PAD_VERTICAL);
			};
		case TEXT_PAD_WITH_PLATE_ANIMATED:
			switch (side) {
				case TOP_SIDE:
					return(TEXT_PAD_WITH_PLATE_ANIMATED_TOP);
				case BOTTOM_SIDE:
					return(TEXT_PAD_WITH_PLATE_ANIMATED_BOTTOM);
				case LEFT_SIDE:
					return(TEXT_PAD_WITH_PLATE_ANIMATED_LEFT);
				default:
					return(TEXT_PAD_WITH_PLATE_ANIMATED_RIGHT);
			};
		case NUMBER_PAD_WITH_PLATE:
			switch (side) {
				case TOP_SIDE:
				case BOTTOM_SIDE:
					return(NUMBER_PAD_HORIZONTAL);
				default:
					return(NUMBER_PAD_VERTICAL);
			};
		case NUMBER_PAD_WITH_PLATE_ANIMATED:
			switch (side) {
				case TOP_SIDE:
					return(NUMBER_PAD_WITH_PLATE_ANIMATED_TOP);
				case BOTTOM_SIDE:
					return(NUMBER_PAD_WITH_PLATE_ANIMATED_BOTTOM);
				case LEFT_SIDE:
					return(NUMBER_PAD_WITH_PLATE_ANIMATED_LEFT);
				default:
					return(NUMBER_PAD_WITH_PLATE_ANIMATED_RIGHT);
			};
		case SOUND_PAD_WITH_PLATE:
			switch (side) {
				case TOP_SIDE:
				case BOTTOM_SIDE:
					return(SOUND_PAD_HORIZONTAL);
				default:
					return(SOUND_PAD_VERTICAL);
			};
		case FORCE_PAD_WITH_PLATE:
			switch (side) {
				case TOP_SIDE:
				case BOTTOM_SIDE:
					return(FORCE_PAD_HORIZONTAL);
				default:
					return(FORCE_PAD_VERTICAL);
			};
		case OPERATION_PAD_WITH_PLATE: // added 130202
			switch (side) {
				case TOP_SIDE:
				case BOTTOM_SIDE:
					return(OPERATION_PAD_HORIZONTAL);
				default:
					return(OPERATION_PAD_VERTICAL);
			};
	};
	return(NUMBER_PAD_VERTICAL); // for now
};

city_coordinate compute_edge_size(city_coordinate plate_width, city_coordinate plate_height) {
	return(min(plate_height/edge_size_proportion,plate_width/edge_size_proportion));
};

city_coordinate compute_edge_size_given_text_size(city_coordinate text_width, city_coordinate text_height) {
	// new on 270202
	if (text_width > text_height) {
		return((edge_size_proportion*text_height)/(edge_size_proportion-2)-text_height);
	} else {
		return((edge_size_proportion*text_width)/(edge_size_proportion-2)-text_width);
	};
};

void display_plate_borders(city_coordinate llx, city_coordinate lly,
								   city_coordinate plate_width, city_coordinate plate_height,
								   PlateState plate_state, bytes color_permutation) {
//	plate_width = min(plate_width,ideal_screen_width); // new on 050202 to deal with very large numbers (or text wide text pads)
//	plate_height = min(plate_height,ideal_screen_height); // no need to be larger than the screen
	city_coordinate edge_size = compute_edge_size(plate_width,plate_height);
	city_coordinate horizontal_height = edge_size; // do this to make it easier to go back to different widths...
	city_coordinate vertical_width = edge_size;
	city_coordinate one_x_pixel = tt_screen->one_pixel_in_x_city_units();
	city_coordinate one_y_pixel = tt_screen->one_pixel_in_y_city_units();
	city_coordinate plate_height_without_borders = plate_height-2*horizontal_height+2*one_y_pixel;
	city_coordinate screen_width = tt_screen->viewing_width();
	city_coordinate twice_screen_width = screen_width*2;
	boolean display_left_side = TRUE; // new on 100804 since if both are off screen don't display
	boolean display_right_side = TRUE;
	if (plate_width > twice_screen_width) { // is wider than twice the screen and is off both edges - new on 060202
		if (llx < tt_screen->min_x()) { // off left edge of screen
			display_left_side = FALSE;
			if (llx+plate_width > tt_screen->max_x()) { // off right edge of screen
				display_right_side = FALSE;
				// use twice screen width sometimes so that pattern on plate edge moves as number is moved
				plate_width = twice_screen_width;
				llx = tt_screen->min_x()-abs(llx%screen_width);
			} else {
				llx = llx+plate_width-twice_screen_width; // llx is computed from right edge
				plate_width = twice_screen_width;
			};
		} else {
			plate_width = twice_screen_width;
		};
	};
	// do same for height?
	display_plate(llx,lly,plate_width,horizontal_height,border_plate(plate_state,BOTTOM_SIDE),FALSE,color_permutation);
	display_plate(llx,lly+plate_height-horizontal_height,plate_width,horizontal_height,border_plate(plate_state,TOP_SIDE),FALSE,color_permutation);
	if (display_left_side) {
		display_plate(llx,lly+horizontal_height-one_y_pixel,vertical_width,plate_height_without_borders,border_plate(plate_state,LEFT_SIDE),FALSE,
						  color_permutation);
	};
	if (display_right_side) {
		display_plate(llx+plate_width-vertical_width,lly+horizontal_height-one_y_pixel,vertical_width,plate_height_without_borders,border_plate
						  (plate_state,RIGHT_SIDE),FALSE,color_permutation);
	};
};

void display_two_plate_borders(city_coordinate llx, city_coordinate lly,
										 city_coordinate plate_width, city_coordinate plate_height,
										 PlateState plate_state, bytes color_permutation) {
	// new on 050102
//	plate_width = min(plate_width,ideal_screen_width); // new on 050202 to deal with very large numbers (or text wide text pads)
//	plate_height = min(plate_height,ideal_screen_height); // no need to be larger than the screen
	city_coordinate one_x_pixel = tt_screen->one_pixel_in_x_city_units(); // to deal with roundoff
	display_plate(llx,lly,plate_width/2+one_x_pixel,plate_height,border_plate(plate_state,LEFT_SIDE),FALSE,color_permutation);
	display_plate(llx+plate_width/2,lly,plate_width/2,plate_height,border_plate(plate_state,RIGHT_SIDE),FALSE,color_permutation);
};

/*
long display_plated_text(string text, long text_length, 
								 city_coordinate llx, city_coordinate lly,
								 city_coordinate plate_width, city_coordinate plate_height,
//								 city_coordinate width, city_coordinate height,
								 PlateState plate_state,
								 boolean plate_visible, boolean text_visible,
                         color_index text_color,
								 bytes color_permutation) {
// prints until a line feed; returns number of characters printed
	if (text_length == 0) return(0);
	if (plate_width == 0 || plate_height == 0) return(text_length);
//   if (tt_log_version_number > 11) { // plates overlap now
//	if (tt_new_pad_look && text_length > 1) { // new on 171201
//		plate_width = (plate_width*5*text_length)/(4*2+3*(text_length-2));
//	} else {
      plate_width = (plate_width*5*text_length)/(5+4*(text_length-1));
//   };
	city_coordinate character_width; 
	if (need_wide_characters()) { 
		character_width = (6*plate_width)/10; // new on 150201 so numbers look nicer - not clear why it was needed
	} else {
		character_width = (7*plate_width)/10; // was 4/5
	};
	city_coordinate character_height = (4*plate_height)/5;
	city_coordinate character_x_space = (3*plate_width)/20;
	city_coordinate character_y_space = plate_height/10;
	city_coordinate character_llx = llx; //+width-plate_width;
	city_coordinate character_lly = lly; //+(height-plate_height)/2;
	// to avoid one pixel wide gaps due to round off
	city_coordinate widened_plate_width, delta_width;
//   if (tt_log_version_number > 11) { // new policy on 2/1/98 is that plates overlap much like cubbies
   widened_plate_width = plate_width;
//	if (tt_new_pad_look && text_length > 1) { // new on 171201
//		delta_width = plate_width; // no need to overlap (3*plate_width)/5;
//	} else {
		delta_width = (4*plate_width)/5-tt_screen->one_pixel_in_x_city_units(); 
		// new on 201201 to substract a pixel to avoid gaps due to round off errors
//	};
//   } else {
//		widened_plate_width = plate_width+tt_screen->one_pixel_in_x_city_units();
//      delta_width = plate_width;
//   };
	long i = 0;
	while (character_llx < -plate_width) {
		// can't be seen since off the left side of screen
		if (text[i] == '\r') { // new line
			return(i+1);
		};
		i++;
      if (i == text_length) return(text_length);
		character_llx += delta_width;
	};
	if (character_llx > tt_screen->max_x()) { // off the right
		while (text[i] != '\r' && i < text_length) {
			i++;
         if (i == text_length) return(text_length);
		};
		return(i);
	};
   long first_i = i;
   city_coordinate start_llx = character_llx;
	PlateState original_plate_state = plate_state;
   if (plate_visible) {
	   for (; i < text_length; i++) {
         if (text[i] == 10 && i+1 < text_length) {
				i++; // skip carriage returns
			};
			if (text[i] == '\r') { // new line
				if (!text_visible) text_length = i+1; // otherwise it'll take care of it
				if (i == 0) { // empty line
					display_plate(character_llx,character_lly,plate_width,plate_height,
									  plate_state,TRUE,color_permutation);
				};
				text_length = i+1; // new on 211201 - how did this work earlier? - and shouldn't the narrow version do this too?
				break;
			};
			if (text[i] == 10) { // && i+1 < text_length) {
				i++; // skip carriage returns
			} else {
				// new on 171201 - should also check that running with new m25.us1 as well
				if (tt_new_pad_look && text_length > 1) {
					LeftMiddleOrRight position;
					if (i == 0) { // first one
						if (i == text_length-1) { // conditional new on 020102
							position = WHOLE_THING;
						} else {
							position = LEFT_SIDE;
						};
					} else if (i == text_length-1) { 
						position = RIGHT_SIDE;						
					} else {
						position = MIDDLE_REGION;
					};
					switch (original_plate_state) {
						case NUMBER_PAD_WITH_PLATE:
							if (position == LEFT_SIDE) { // first one
								plate_state = NUMBER_PAD_WITH_PLATE_LEFT;
							} else if (position = RIGHT_SIDE) { // last one
								plate_state = NUMBER_PAD_WITH_PLATE_RIGHT;
							} else if (position == MIDDLE_REGION) {
								plate_state = NUMBER_PAD_WITH_PLATE_MIDDLE;
							};
							break;
						case NUMBER_PAD_WITH_PLATE_ANIMATED: // added 201201
							if (position == LEFT_SIDE) { // first one
								plate_state = NUMBER_PAD_WITH_PLATE_ANIMATED_LEFT;
							} else if (position = RIGHT_SIDE) { // last one
								plate_state = NUMBER_PAD_WITH_PLATE_ANIMATED_RIGHT;
							} else if (position == MIDDLE_REGION) {
								plate_state = NUMBER_PAD_WITH_PLATE_ANIMATED_MIDDLE;
							};
							break;
					};
				};
			   display_plate(character_llx,character_lly,
							     widened_plate_width,plate_height,
							     plate_state,!text_visible,color_permutation);
			};
			character_llx += delta_width;
		};
		if (!text_visible) character_lly += character_y_space;
	};
   if (text_visible) { // best to seperate text from plates
//      character_llx += character_x_space;
      // too hard to work with -- e.g. width of object and text...
//      if (!plate_visible) { // looks much nicer if I don't space to plates
//         int to_print = 0;
//         for (i = first_i; i < text_length; i++) {
//            if (text[i] == 10 && i+1 < text_length) {
//               i++; // skip carriage returns
//            };
//            if (text[i] == '\r') { // new line
//               text_length = i+1;
//               break;
//			   };
//            to_print++;
//         };
//         tt_screen->text_out(text,to_print,
//						           character_llx,character_lly+character_height,
//						           character_width,character_height);
//      } else {
         character_llx = start_llx+character_x_space;
         character_lly += character_y_space;
//BEGIN_GDI
//	      tt_screen->set_font(character_width,character_height,TRUE,TRUE);
//			if (color_permutation != NULL) { // commented out on 011001
//				text_color = color_permutation[text_color]; // new on 200901
//			};
			color color = compute_color(text_color,color_permutation);
         for (i = first_i; i < text_length; i++) {
            if (text[i] == 10 && i+1 < text_length) i++; // skip carriage returns
            if (text[i] == '\r') { // new line
                text_length = i+1;
                break;
				} else if (text[i] == '\n') {
				    i++; // skip carriage returns
				} else {
					tt_screen->place_character(text[i],
														character_llx,character_lly,
														character_width,character_height,
														color);
					character_llx += delta_width;
				};
         };
//         tt_main_window->deselect_font();
//END_GDI
//      };
	};
	return(text_length);
};

// this should be identical except for dealing with wide characters
long display_plated_wide_text(wide_string text, long text_length, 
										city_coordinate llx, city_coordinate lly,
										city_coordinate plate_width, city_coordinate plate_height,
//										city_coordinate width, city_coordinate height,
										PlateState plate_state,
										boolean plate_visible, boolean text_visible,
										color_index text_color,
										bytes color_permutation) {
// prints until a line feed; returns number of characters printed
	if (text_length == 0) return(0);
	if (plate_width == 0 || plate_height == 0) return(text_length);
//   if (tt_log_version_number > 11) { // plates overlap now
       plate_width = (plate_width*5*text_length)/(5+4*(text_length-1));
//   };
	city_coordinate character_width = (7*plate_width)/10; // was 4/5
	city_coordinate character_height = (4*plate_height)/5;
	city_coordinate character_x_space = (3*plate_width)/20;
	city_coordinate character_y_space = plate_height/10; // huh??
	city_coordinate character_llx = llx;//+width-plate_width;
	city_coordinate character_lly = lly;//+(height-plate_height)/2;
	// to avoid one pixel wide gaps due to round off
	city_coordinate widened_plate_width, delta_width;
//   if (tt_log_version_number > 11) { // new policy on 2/1/98 is that plates overlap much like cubbies
      widened_plate_width = plate_width;
      delta_width = (4*plate_width)/5-tt_screen->one_pixel_in_x_city_units(); // new on 201201 to subtract 1 pixel do avoid gaps from round off errors
//   } else {
//		widened_plate_width = plate_width+tt_screen->one_pixel_in_x_city_units();
//      delta_width = plate_width;
//   };
	long i = 0;
	if (text[i] == 10) { // new on 020102 since otherwise the wrong kind of text plate is displayed
		i++; // skip carriage returns
	};
	while (character_llx < -plate_width) {
		// can't be seen since off the left side of screen
		if (text[i] == '\r') { // new line
			return(i+1);
		};
		i++;
      if (i == text_length) return(text_length);
		character_llx += delta_width;
	};
	if (character_llx > tt_screen->max_x()) { // off the right
		while (text[i] != '\r' && i < text_length) {
			i++;
         if (i == text_length) return(text_length);
		};
		return(i);
	};
   long first_i = i;
   city_coordinate start_llx = character_llx;
	PlateState original_plate_state = plate_state;
   if (plate_visible) {
	   for (; i < text_length; i++) {
         if (text[i] == 10 && i+1 < text_length) {
				i++; // skip carriage returns
			};
			if (text[i] == '\r') { // new line
				if (!text_visible) {
					text_length = i+1; // otherwise it'll take care of it
				};
				if (i == 0) { // empty line
					PlateState new_plate_state;
					switch (plate_state) { // new on 020102
						case TEXT_PAD_WITH_PLATE:
							new_plate_state = TEXT_PAD_WITHOUT_PLATE;
							break;
						case SOUND_PAD_WITH_PLATE:
							new_plate_state = SOUND_PAD_WITHOUT_PLATE;
							break;
						case FORCE_PAD_WITH_PLATE:
							new_plate_state = FORCE_PAD_WITHOUT_PLATE;
							break;
						default:
							new_plate_state = plate_state;
					};
					display_plate(character_llx,character_lly,plate_width/6,plate_height, // added /6 on 020102 so it looks like just a border
									  new_plate_state,TRUE,color_permutation);
				};
				text_length = i; //+1; // new on 211201 -- commented +1 out on 020102
				break;
			};
			if (text[i] == 10) { // && i+1 < text_length) {
				i++; // skip carriage returns
			} else {
				if (tt_new_pad_look && text_length > 1) { // new on 171201
					LeftMiddleOrRight position;
					if (i == text_length-1 || (text[i+1] == '\r')) { // end of line of end of all
						if (i == first_i) { // conditional new on 020102 to handle single characters in multi-line text pads
							position = WHOLE_THING;
						} else {
							position = RIGHT_SIDE;
						};
					} else if (i == first_i) { // first one -- was == 0 prior to 020102
						position = LEFT_SIDE;
					} else {
						position = MIDDLE_REGION;
					};
					switch (original_plate_state) {
						case TEXT_PAD_WITH_PLATE:
							if (position == LEFT_SIDE) { 
								plate_state = TEXT_PAD_WITH_PLATE_LEFT;
							} else if (position == RIGHT_SIDE) { 
								plate_state = TEXT_PAD_WITH_PLATE_RIGHT;
							} else if (position == MIDDLE_REGION) {
								plate_state = TEXT_PAD_WITH_PLATE_MIDDLE;
							};
							break;
						case TEXT_PAD_WITH_PLATE_ANIMATED: // added 201201
							if (position == LEFT_SIDE) { 
								plate_state = TEXT_PAD_WITH_PLATE_ANIMATED_LEFT;
							} else if (position = RIGHT_SIDE) { 
								plate_state = TEXT_PAD_WITH_PLATE_ANIMATED_RIGHT;
							} else { 
								plate_state = TEXT_PAD_WITH_PLATE_ANIMATED_MIDDLE;
							};
							break;
						case SOUND_PAD_WITH_PLATE:
							if (position == LEFT_SIDE) { 
								plate_state = SOUND_PAD_WITH_PLATE_LEFT;
							} else if (position == RIGHT_SIDE) { 
								plate_state = SOUND_PAD_WITH_PLATE_RIGHT;
							} else if (position == MIDDLE_REGION) {
								plate_state = SOUND_PAD_WITH_PLATE_MIDDLE;
							};
							break;
						case FORCE_PAD_WITH_PLATE:
							if (position == LEFT_SIDE) { 
								plate_state = FORCE_PAD_WITH_PLATE_LEFT;
							} else if (position == RIGHT_SIDE) { 
								plate_state = FORCE_PAD_WITH_PLATE_RIGHT;
							} else if (position == MIDDLE_REGION) {
								plate_state = FORCE_PAD_WITH_PLATE_MIDDLE;
							};
							break;
					};
				};
			   display_plate(character_llx,character_lly,
							     widened_plate_width,plate_height,
							     plate_state,!text_visible,color_permutation);
			};
			character_llx += delta_width;
		};
		if (!text_visible) character_lly += character_y_space;
	};
   if (text_visible && !tt_new_pad_look) { // best to separate text from plates
//      character_llx += character_x_space;
      // too hard to work with -- e.g. width of object and text...
//      if (!plate_visible) { // looks much nicer if I don't space to plates
//         int to_print = 0;
//         for (i = first_i; i < text_length; i++) {
//            if (text[i] == 10 && i+1 < text_length) {
//               i++; // skip carriage returns
//            };
//            if (text[i] == '\r') { // new line
//               text_length = i+1;
//               break;
//			   };
//            to_print++;
//         };
//         tt_screen->text_out(text,to_print,
//						           character_llx,character_lly+character_height,
//						           character_width,character_height);
//      } else {
         character_llx = start_llx+character_x_space;
         character_lly += character_y_space;
//BEGIN_GDI
//	      tt_screen->set_font(character_width,character_height,TRUE,TRUE);
//			if (color_permutation != NULL) { // commented out on 011001
//				text_color = color_permutation[text_color]; // new on 200901
//			};
         for (i = first_i; i < text_length; i++) {
            if (text[i] == 10 && i+1 < text_length) i++; // skip carriage returns
            if (text[i] == '\r') { // new line
                text_length = i+1;
					 if (text[i+1] == 10) text_length++; // new on 080999 so multi-line text pads partly off the left display right
                break;
				} else if (text[i] == 10) {
				    i++; // skip carriage returns
				} else {
					tt_screen->place_wide_character(text[i],
															  character_llx,character_lly,
															  character_width,character_height,
															  text_color);
					character_llx += delta_width;
				};
         };
//         tt_main_window->deselect_font();
//END_GDI
//      };
	};
	return(text_length);
};
*/

void display_plate(city_coordinate llx, city_coordinate lly,
						 city_coordinate plate_width, city_coordinate plate_height,
						 PlateState plate_state, boolean blank, bytes color_permutation) {
//	if (llx+plate_width < 0 || llx > ideal_screen_width) return;
//	if (lly > ideal_screen_height || lly+plate_height < 0) return;
	if (llx+plate_width < tt_screen->min_x() || llx > tt_screen->max_x()) return;
	if (lly+plate_height < tt_screen->min_y() || lly > tt_screen->max_y()) return;
	if (blank) {
      switch (plate_state) {
         case NUMBER_PAD_WITH_PLATE:
         case NUMBER_PAD_WITH_PLATE_ANIMATED:
		      plate_state = NUMBER_PAD_WITHOUT_PLATE;
            break;
         case TEXT_PAD_WITH_PLATE:
         case TEXT_PAD_WITH_PLATE_ANIMATED:
		      plate_state = TEXT_PAD_WITHOUT_PLATE;
            break;
         case SOUND_PAD_WITH_PLATE:
            plate_state = SOUND_PAD_WITHOUT_PLATE;
            break;
			case FORCE_PAD_WITH_PLATE: // added 050102
            plate_state = FORCE_PAD_WITHOUT_PLATE;
            break;
			case OPERATION_PAD_WITH_PLATE: // added 130202
            plate_state = OPERATION_PAD_WITHOUT_PLATE;
            break;
      };
   };
	TTImage *image = number_pad(plate_state);
	unsigned short int x_scale, y_scale;
	image->how_to_scale_to(plate_width,plate_height,x_scale,y_scale);
	long width_in_pixels = tt_screen->horizontal_pixel_length(plate_width);
	if (width_in_pixels == 0) return; // too small -- new on 011004 -- otherwise may display junk
	long height_in_pixels = tt_screen->vertical_pixel_length(plate_height);
	if (height_in_pixels == 0) return; // too small -- new on 011004
	city_coordinate x = llx;
	city_coordinate y = lly; // new on 111104
	if (width_in_pixels > max_unsigned_short) { // new on 130904 to avoid bad source rectangles when blt_to_back_surface
#if TT_DEBUG_ON
		// new on 111104 that this warning is not in the release version
		static boolean gave_too_wide_warning = FALSE;
		if (!gave_too_wide_warning) {
			tt_error_file() << "Number pad is too wide to display correctly. It is " << width_in_pixels 
								 << " pixels wide and is displayed as if it were " << max_unsigned_short << " pixels wide." << endl;
			gave_too_wide_warning = TRUE;
		};
#endif
		x += tt_screen->horizontal_city_coordinate_length(width_in_pixels-max_unsigned_short)/2; // stay centered -- new on 111104
		width_in_pixels = max_unsigned_short;
	};
	if (height_in_pixels > max_unsigned_short) { // new on 130904 to avoid bad source rectangles when blt_to_back_surface
#if TT_DEBUG_ON
		// new on 111104 that this warning is not in the release version
		static boolean gave_too_high_warning = FALSE;
		if (!gave_too_high_warning) {
			tt_error_file() << "Number pad is too tall to display correctly. It is " << height_in_pixels 
								 << " pixels high and is displayed as if it were " << max_unsigned_short << " pixels wide." << endl;
			gave_too_high_warning = TRUE;
		};
#endif
		y += tt_screen->vertical_city_coordinate_length(height_in_pixels-max_unsigned_short)/2; // stay centered -- new on 111104
		height_in_pixels = max_unsigned_short;
	};
	tt_screen->move_mark_to(x,y); // llx,lly); // moved here on 111104 
	boolean ignore;
	image->display(width_in_pixels,height_in_pixels,ignore,color_permutation,x_scale,y_scale);
};

MatchStatus match(Sprite *in_thought_bubble, Sprite *other, int version) {
   in_thought_bubble = in_thought_bubble->dereference();
   // what if other is a remote for an integer?
   SpriteType thought_bubble_type = in_thought_bubble->kind_of();
   SpriteType other_type = other->kind_of();
   if (thought_bubble_type != other_type) return(MATCH_BAD);
   SpritePointerPointer suspension_cell;
   return(same_type_match(thought_bubble_type,in_thought_bubble,other,suspension_cell,version));
};

MatchStatus same_type_match(SpriteType type, Sprite *item, Sprite *other, SpritePointerPointer &suspension_cell, 
									 int version, int index) { 
   switch (type) {
      case CUBBY:
         return(((Cubby *) item)->match((Cubby *) other,suspension_cell,version));
      case INTEGER:
         return(((Number *) item)->match((Number *) other,version));
		case REMOTE_PICTURE:
			// prior to 111099 this was handled above as if it was an Integer -- and what about REMOTE_INTEGERs?
			return(((Remote_Picture *) item)->match((Remote_Picture *) other,version));
      case TEXT:
		case FORCE_FEEDBACK: // new on 100400
	   case SOUND: // moved here on 100400 since following wasn't quite right but did the same thing
//         return(((BuiltInSound *) item)->match((BuiltInSound *) other));
         return(((Text *) item)->match((Text *) other,version));
      case PICTURE:
          return(((Picture *) item)->match((Picture *) other,suspension_cell,version,index));
      case NEST: // an empty nest matches any empty nest
//			tt_touched_a_remote = CAN_CHANGE_TO_MATCH_FAILURE; 
			// updated on 250302 -- was CAN_CHANGE_ANYTIME; // not exactly a remote but this is a "non stable" property
			// thanks to Stefan for providing an example of this
			set_touched_a_remote(CAN_CHANGE_TO_MATCH_FAILURE); 
			// rewritten on 060103 so that if this is the only problem won't run again this cycle if remotes are involved elsewhere
			// and fall through
      case BIRD: // a bird variable matches any bird
      case BOMB:
			return(MATCH_GOOD);
      case ROBOT:
         return(((Robot *) item)->match((Robot *) other,version));
      case TRUCK:
         return(((Truck_Inside *) item)->match((Truck_Inside *) other,suspension_cell,version));
      case PROGRAM_PAD:
         return(((Notebook *) item)->match((Notebook *) other,version));
      default:
			return(MATCH_BAD);
   };
};

void do_when_not_city_stopped(Sprite *sprite, AnimationCallBack callback) {
   if (tt_city->stopped()) {
      tt_not_city_stopped_callbacks = new AnimationCallBacks(sprite,callback,tt_not_city_stopped_callbacks); 
		// was missing tt_not_stop_all_callbacks = prior to 181102!
   } else {
      AnimationCallBackProcedure procedure = callback_procedure(callback);
      procedure(sprite);
   };
};

// don't forget to XML dump not_tt_stop_all_callbacks from city -- 181102

void stop_all(boolean on) {
   if (!on && tt_not_city_stopped_callbacks != NULL && tt_titles_ended_on_frame != max_long) {
		// second condition new on 170205 so this isn't called when loading a city that is turned off and has trucks and the like waiting to go
      tt_not_city_stopped_callbacks->run();
      delete tt_not_city_stopped_callbacks;
      tt_not_city_stopped_callbacks = NULL;
   };
//if (tt_debug_mode == 1110) { // for testing
//   if (tt_stop_all && !on) { // turning on
//      tt_main_window->restore_original_palette();
//   } else if (!tt_stop_all && on) { // turning off
//       tt_main_window->use_palette_with_color_deltas(x,x,x);
//   };
//};
   tt_city->set_stopped(on);
//	tt_darken_all = (on && tt_programmer->are_titles_over());
	// following new on 200901 so all images are recomputed so color permutation is correct
	free_all_cache_entries(); // should only free images in the cache - noticed on 251102
//	tt_screen->invalidate_cache();
//	reset_grid();
	tt_screen->screen_dirty();
	if (tt_toolbox != NULL) {
		tt_toolbox->set_clean_status(DIRTY,TRUE); // to invalidate floor image cache
	};
	if (!tt_city->stopped() && tt_pictures_to_activate != NULL) {
		// new on 010499 so that pictures are activated again after power is resumed if they were activated while power was off
		Sprites *remaining = tt_pictures_to_activate;
		while (remaining != NULL) {
			Sprite *sprite = remaining->first();
			if (sprite->return_ref_count() > 1) { // I'm not the only reference so it is not trash
				sprite->activate();
//				remaining->set_first(NULL); // not needed since ref count will protec it
			};
			remaining = remaining->rest();
		};
		destroy_sprites(tt_pictures_to_activate); // re-written on 241000 since this also frees the memory of the list itself
//		tt_pictures_to_activate->destroy_all();
//		tt_pictures_to_activate = NULL;
	};
	if (tt_martian != NULL) tt_martian->power_toggled();
};

void remove_from_tt_pictures_to_activate(Picture *picture) { // new on 121103
	if (tt_pictures_to_activate != NULL) {
		boolean found;
		tt_pictures_to_activate = tt_pictures_to_activate->remove(picture,&found);
		if (found) {
			picture->destroy(); // remove reference
		};
	};
};

boolean ok_to_make_a_sound(Background *floor) {
   return((tt_sound_option&SOUND_BIT) &&
          (floor == NULL || // floor could be NULL since this might be indirecting via FILE_TO_SOUND_REMOTE
           tt_screen->pointer_to_background()->pointer_to_room() == floor->pointer_to_room()));
};

int alphabet_size; // = 128-' '; // skip the first 31 (control characters)

string alphabet = NULL;
#if TT_WIDE_TEXT
wide_string wide_alphabet = NULL;
#endif

void initialize_alphabet() {
   const_ascii_string upper = AS(IDS_ALPHABET_UPPER_CASE);
   int j = strlen(upper);
   alphabet = new char[256]; // surely large enough
   memcpy(alphabet,upper,j);
   memcpy(alphabet+j,S(IDS_ALPHABET_LOWER_CASE),j);
   j += j;
   int i = j;
   for (j = ' '; j < 'A'; j++) alphabet[i++] = (char) j;
   for (j = 'z'+1; j < 128; j++) alphabet[i++] = (char) j;
   alphabet_size = i-1;
	alphabet[alphabet_size] = NULL; // new on 010799
#if TT_WIDE_TEXT
	wide_alphabet = copy_wide_string(alphabet);
#endif
};

//#if TT_SWEDISH
//   alphabet_size += 3*2; // 3 extra characters
//#endif  
//   alphabet = new char[alphabet_size];
//   int i = 0;
//   for (; i < 26; i++) {
//      alphabet[i] = (char) ('A'+i);
//   }; 
//#if TT_SWEDISH
//   alphabet[i++] = 'ﾅ';
//   alphabet[i++] = 'ﾄ';
//   alphabet[i++] = 'ﾖ';
//#endif
//   for (int j = 0; j < 26; j++) {
//      alphabet[i++] = (char) ('a'+j);
//   };
//#if TT_SWEDISH
//   alphabet[i++] = '・;
//   alphabet[i++] = '・;
//   alphabet[i++] = '・;
//#endif

//};

// following sorted the entire character set
// problem is that there were for example 5 kinds of I with various
// accents etc.
//   for (int i = 0; i < alphabet_size; i++) alphabet[i] = (char) i;
   // not sort using locale sensitive primitive
//   char letter1[2];
//   letter1[1] = 0; // teriminate string
//   char letter2[2];
//   letter2[1] = 0; // teriminate string
//   for (int i = 0; i < alphabet_size; i++) {
//      for (int j = i; j < alphabet_size; j++) {
//         letter1[0] = alphabet[i];
//         letter2[0] = alphabet[j];
//         if (lstrcmp(letter1,letter2) > 0) { // out of order so swap
//            char temp = alphabet[i];
//            alphabet[i] = alphabet[j];
//            alphabet[j] = temp;
//         };
//      };
//   };

void release_alphabet() {
   if (alphabet != NULL) delete [] alphabet;
#if TT_WIDE_TEXT
	if (wide_alphabet != NULL) delete [] wide_alphabet;
#endif
};

character next_in_alphabet(character letter, long change) {
   for (int i = 0; i < alphabet_size; i++) {
      if (alphabet[i] == letter) {
//         tt_error_file() << change << " " << ((i+change%alphabet_size+alphabet_size)%alphabet_size) << endl;
         return(alphabet[(i+change%alphabet_size+alphabet_size)%alphabet_size]);
      };
   };
   return('?'); // shouldn't happen
};
#if TT_WIDE_TEXT
wide_character next_in_alphabet(wide_character letter, long change) {
   for (int i = 0; i < alphabet_size; i++) {
      if (wide_alphabet[i] == letter) {
         return(wide_alphabet[(i+change%alphabet_size+alphabet_size)%alphabet_size]);
      };
   };
   return((wide_character) (letter+change)); 
};
#endif

void write_cookie(output_stream &stream) {
//   stream.write("TnTlk005",8); // magic cookie and version number
	stream.write(_T("TnTl"),4);
   // for backwards compatibility the old means tt_language = NO_LANGUAGE_SPECIFIED
   stream.put((character) ('k'+tt_language)); // language
//   stream.write(_T("021"),3); //version number
	char version_string[4]; // was 3 prior to 210901 but ttbug32 caught an overrun in itoa below
	int version_number;
	if (!tt_dump_so_equals_share) { // this can't be a good long-term solution if DumpCopiesCompactly is documented 
		version_number = 20;
	} else {
		version_number = latest_version_number;
	};
	if (version_number < 100) {
		version_string[0] = '0';
		itoa(version_number,version_string+1,10);
	} else {
		itoa(version_number,version_string,10);
	};
	stream.write(version_string,3); 
};

boolean read_cookie(InputStream *stream,
                    character &cookie_version_number, NaturalLanguage &language,
                    ascii_string file_name) {
   char cookie[9];
	stream->read((string) cookie,8);
	cookie[8] = '\0'; // terminate string - not sure why I need to
	// broke this into 2 pieces so XML code can check cookie as well
	boolean ok = process_cookie(cookie,cookie_version_number,language,file_name);
	if (!ok) {
		tt_error_file() << "File position is " << (int) stream->tellg() << endl;
	};
	return(ok);
};

boolean valid_cookie(string cookie) {
	return(strncmp(cookie,"TnTl",4) == 0);
};

boolean process_cookie(string cookie,
							  character &cookie_version_number, NaturalLanguage &language,
							  ascii_string file_name) {
   // changed from TnTlk to TnTl<k+language byte> on 12/23/96
	cookie_version_number = atoi(cookie+5);
	if (strncmp(cookie,"TnTl",4) != 0 ||
       (cookie_version_number == 0 && strncmp(cookie,"TnTlk000",8) != 0)) {
		character buffer[1024];
		output_string_stream out(buffer,1024);
		if (file_name == NULL) {
			out << S(IDS_SORRY_BUT_FILE) << space()
				 << tt_file_name << space() << S(IDS_NOT_NOTEBOOK_FILE);
		} else {
			out << S(IDS_SORRY_BUT_FILE) << space()
				 << tt_file_name << "." << file_name << space() << S(IDS_NOT_NOTEBOOK_FILE);
		};
		out.put('\0');
		say_error(buffer,TRUE);
      return(FALSE);
   };
   language = (NaturalLanguage) (cookie[4]-'k'); // could do error checking first
   if (language == NO_LANGUAGE_SPECIFIED || language == UNKNOWN_LANGUAGE) {
      language = tt_language;
   } else if (language < 0 || language >= LANGUAGE_COUNT) {
#if TT_DEBUG_ON
      say_error(_T("Something wrong with language indicator of a file."));
#endif
      language = tt_language;
   };
   return(TRUE);
};

boolean text_to_speech() {
  return(tt_marty_talk == MARTY_SPEAKS || tt_marty_talk == MARTY_SPEAKS_AND_TALK_BALLOONS || tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES);
};

boolean limited_text_to_speech() { // new on 050499
	// if this switch is set in toontalk.ini then we assume Marty can say anything
	return(tt_limited_text_to_speech && text_to_speech());
};

//boolean ensure_failed = FALSE;
boolean ensure_picture_failed = FALSE; // split into 2 flags on 101201 since might have only one set of media
boolean ensure_sound_failed = FALSE;

string directory_with_java_directory() { // abstracted on 110702 so used throughout
   if (tt_java_files_shared) {
      return(tt_shared_files_directory); //  was tt_main_directory prior to 050402
   } else {
      return(tt_user_directory);
   };
};

java_string ensure_file_for_java(ascii_string media_file_name, SpriteType type, boolean check_cdrom) {
	// extension added as an arg so this can be called to ensure ZIP or CAB archive files
	// why doesn't this use GDI+ if necessary to generated the GIF file?
   if (media_file_name == NULL) {
      return(NULL);
   };
   ascii_string extension;
   ascii_string subdirectory; // the default place to find the files
	ascii_string very_short_name = basic_file_name(media_file_name);
   if (type == PICTURE) {
      extension = "gif";
      if (tt_video_mode == 320) {
         subdirectory = AC(IDC_JAVA_PICTURE_LOW_RES_PATH);
      } else {
         subdirectory = AC(IDC_JAVA_PICTURE_HIGH_RES_PATH);
      };
   } else if (type == SOUND) {
      extension = "au";
      subdirectory = AC(IDC_JAVA_SOUND_PATH);
   } else {
		if (very_short_name != NULL) delete [] very_short_name; // new on 240601
      return(NULL);
   };
   char existing_file[MAX_PATH];
   output_string_stream file_name(existing_file,sizeof(existing_file));
   file_name << directory_with_java_directory() << AC(IDC_JAVA_PATH) << very_short_name << "." << extension;   
   file_name.put('\0');
//   OFSTRUCT file_struct;
//   boolean file_already_exists = (OpenFile(existing_file,&file_struct,OF_EXIST) != -1); // if not then copy it from source
	boolean file_already_exists = file_exists(existing_file,TRUE); // nicer abstraction (240201)
//    if (!check_cdrom) { // commented out on 031100 so FileSearchPath is used
//			return(NULL);
//		};
	boolean ignore;
#if TT_PATCH
	if (!file_already_exists) { // new on 090401 to try subdirectory (since that's where it'll be if an older version of ToonTalk was patched)
		char old_location[MAX_PATH];
		output_string_stream file_name(old_location,sizeof(existing_file));
      // was directory_with_java_directory() prior to 051002
		file_name << tt_main_directory << subdirectory << "\\" << very_short_name << "." << extension; // updated 110702
		file_name.put('\0');
		if (file_exists(old_location,TRUE)) {
			file_already_exists = copy_file_or_url_to_file(old_location,existing_file,ignore);
		};
	};
#endif
	if (check_cdrom && tt_cdrom_directory != NULL && !file_already_exists && number_of_times_user_asked_to_insert_cdrom < 4) {
		// new on 240201 to try CDROM
		// didn't include isCDROMDemoFile(very_short_name,extension) 
		// since there are more than a 1000 of built-in pictures and caller is careful to set check_cdrom
		char cdrom_file[MAX_PATH];
		output_string_stream file_name(cdrom_file,sizeof(cdrom_file));
		file_name << tt_cdrom_directory << AC(IDC_JAVA_PATH) << very_short_name << "." << extension; 
		file_name.put('\0');
//		if (toontalk_cdrom_available(IDS_NEED_CDROM_TO_CONTINUE)) {	// commented out on 300304 since no need to bother user since can generate GIF now
			if (!file_exists(cdrom_file,FALSE)) { // CD-ROM must be an older version
				output_string_stream file_name(cdrom_file,sizeof(cdrom_file));
				file_name << tt_cdrom_directory << subdirectory << "\\" << very_short_name << "." << extension; 
				file_name.put('\0');
			};
//		};
		if (file_exists(cdrom_file,FALSE)) { // condition new on 020301
			file_already_exists = copy_file_or_url_to_file(cdrom_file,existing_file,ignore);
		};
	};
	if (!file_already_exists && type == PICTURE) { // new on 130201 to check for jpg
		extension = "jpg";
		output_string_stream file_name(existing_file,sizeof(existing_file));
		file_name << directory_with_java_directory() << AC(IDC_JAVA_PATH) << very_short_name << "." << extension; // updated 110702
		file_name.put('\0');
		file_already_exists = file_exists(existing_file,TRUE);
		if (!file_already_exists) { // new on 130201 to check for jpeg
			extension = "jpeg";
			output_string_stream file_name(existing_file,sizeof(existing_file));
			file_name << directory_with_java_directory() << AC(IDC_JAVA_PATH) << very_short_name << "." << extension; // updated 110702
			file_name.put('\0');
		};
		if (!file_already_exists) { // new on 130201 since this looks for GIF, JPG, and JPEG
			string result = gif_file_for_java(very_short_name);
			delete [] very_short_name; // new on 240601
			return(result);
		};
	};
	if (!file_already_exists) {
		ascii_string source_file_name;
		FileNameStatus file_status;
		{ // so unwind protect has correct scope
			UnwindProtect<string> set(tt_URL_cache_directory,append_strings(tt_main_directory,AC(IDC_JAVA_PATH))); // new on 060401
			source_file_name = existing_file_name(file_status,very_short_name,extension,subdirectory,TRUE,FALSE); 
			// TRUE,FALSE added 031100 so as not to bother the user
			delete [] tt_URL_cache_directory;
		};
		if (source_file_name == NULL) {
			// new on 021204 to try to generate the au file from the wav file
			// problem is that this won't find the wav file since it is embedded in the resind.* file - sigh
			//string wave_file_name = existing_file_name(file_status,very_short_name,"wav",AC(IDC_JAVA_PATH),FALSE,FALSE);
			//if (wave_file_name != NULL) {
			//	string au_file_name = au_file_for_java(wave_file_name);
			//	delete [] wave_file_name;
			//	if (au_file_name != NULL) {
			//		return(au_file_name);
			//	};
			//};
         log(S(IDS_JAVA_SOUNDS_AND_PICTURES_MISSING),FALSE,TRUE); // was message_box prior to 031100 -- args added on 170101 since it is important
         tt_error_file() << S(IDS_PROBLEM_FIND_JAVA_MEDIA_FILE) << very_short_name << "." << extension << endl;
         if (check_cdrom) { // conditional added on 031100
				if (type == PICTURE) {
					ensure_picture_failed = TRUE;
				} else if (type == SOUND) {
					ensure_sound_failed = TRUE;
				};
			};
			delete [] very_short_name; // new on 240601
         return(NULL);
      };
		if (file_status != FILE_ON_INTERNET) {	// conditional new on 060401 since it is now cached in the right directory if a URL
			boolean ignore;
			if (!copy_file_or_url_to_file(source_file_name,existing_file,ignore)) { 
				tt_error_file() << S(IDS_PROBLEM_COPYING_JAVA_MEDIA_FILE) << very_short_name << "." << extension
								<< AC(IDC_FROM) << source_file_name << endl;
				return(NULL);
			};
		};
   };
   string short_file_name = new character[MAX_PATH]; // was 32 prior to 130201
   output_string_stream short_name(short_file_name,MAX_PATH);
   short_name << very_short_name << "." << extension;
   short_name.put('\0');
	tt_error_file() << "The java applet needs the following file: " << short_file_name << endl; // new on 180201
	delete [] very_short_name; // new on 240601
   return(short_file_name);
};

java_string ensure_file_for_java(int id, SpriteType type) {
   if ((ensure_sound_failed && type == SOUND) || (ensure_picture_failed && type == PICTURE) || id < 0) return(NULL); // added id < 0 on 101201
   ascii_string very_short_name;
   if (type == PICTURE) {
      very_short_name = tt_image_file_names[id];
   } else if (type == SOUND) {
      very_short_name = (ascii_string) AS(IDS_SOUND_FILE_NAME+id-1); //sound_files[id-1];
   } else {
      tt_error_file() << S(IDS_PROBLEM_FIND_NAME_OF_JAVA_FILE) << id << endl;
      return(NULL);
   };
   return(ensure_file_for_java(very_short_name,type,TRUE));
};

int extension_start_position(const_ascii_string full_name) { // new on 020600
   int length = strlen(full_name);
   for (int i = length-1; i > 0; i--) {
      if (full_name[i] == '.') {
         return(i+1);
      } else if (full_name[i] == '\\' || full_name[i] == '/' || full_name[i] == ':') { // colon test added on 080401
			return(-1);
		};
   };
   return(-1); // was no extension
};

int first_extension_start_position(const_ascii_string full_name) { // new on 220303 - the one above returns the last extension
   int length = strlen(full_name);
	for (int i = 0; i < length; i++) {
      if (full_name[i] == '.') {
         return(i+1);
		};
   };
   return(-1); // was no extension
};

string quote_file_name(string file_name, string quoted_file_name) {
	if (quoted_file_name == NULL) {
		quoted_file_name = new char[MAX_PATH+2];
	};
	quoted_file_name[0] = '"';
	int length = strlen(file_name);
	memcpy(quoted_file_name+1,file_name,length);
	quoted_file_name[length+1] = '"';
	quoted_file_name[length+2] = '\0';
	return(quoted_file_name);
};

boolean execute(string program, string command_line, boolean quote_command_line) {
	int result; // rewritten on 021103 to get error information out of this
	if (quote_command_line && command_line != NULL) {
		// if command_line is a file name with spaces this is necessary
		char quoted_command_line[MAX_PATH+2];
		quote_file_name(command_line,quoted_command_line);		
		result = (int) ShellExecuteG(NULL,"open",program,quoted_command_line,tt_main_directory,SW_SHOW); // tt_main_directory was "" prior to 140604
	} else {
		// experimented with sometimes using "edit" but got back SE_ERR_NOASSOC -221103
		result = (int) ShellExecuteG(NULL,"open",program,command_line,tt_main_directory,SW_SHOW); // tt_main_directory was "" prior to 140604
	};
#if TT_DEBUG_ON
	if (result <= 32) {
		if (result == ERROR_FILE_NOT_FOUND) {
			debug_this("file not found");
		} else {
			debug_this(); // tt_error_file() may be closed when this is run
		};
	};
#endif
	return ((int) result > 32);
};

ascii_string file_name_without_extension(const_ascii_string full_name) {
   int length = strlen(full_name);
   // skip the extension and .
   int i = length-1;
	for (; i > 0; i--) {
		if (full_name[i] == '.') {
			return(copy_string(full_name,i));
		} else if (full_name[i] == '\\' || full_name[i] == ':' || full_name[i] == '/') { // new on 080401
			break;
		};
	};
	return(copy_string(full_name,length));
};

ascii_string add_or_replace_extension(const_ascii_string full_name, const_ascii_string new_extension) { // new on 200601
	int length = strlen(full_name);
   // skip the extension and .
   int i = length-1;
	for (; i > 0; i--) {
		if (full_name[i] == '.') {
			int extension_length = strlen(new_extension);
			string result = new char[i+extension_length+2];
			memcpy(result,full_name,i+1);
			memcpy(result+i+1,new_extension,extension_length+1);
			return(result);
		} else if (full_name[i] == '\\' || full_name[i] == ':' || full_name[i] == '/') { // new on 080401
			break;
		};
	};
	return(append_3_strings(full_name,".",new_extension)); // had no extension
};	

ascii_string basic_file_name(const_ascii_string full_name, boolean include_extension) {
   int length = strlen(full_name);
   // first skip the extension and .
   int i = length-1;
	if (!include_extension) { // condition new on 020600
		for (; i > 0; i--) {
			if (full_name[i] == '.') {
				i--;
				break;
			} else if (full_name[i] == '\\' || full_name[i] == ':' || full_name[i] == '/') { // new on 080401
				i = length-1; 
				break;
			};
		};
	};
   if (i == 0) { // was no extension
      i = length-1;
   };
   int last = i;
//	if (!is_url(full_name)) {
		for (; i > 0; i--) { // search back for : or backslash
			if (full_name[i] == '\\' || full_name[i] == ':'|| full_name[i] == '/') {
				i++;
				break;
			};
		};
//	};
   int short_name_length = 1+last-i;
   ascii_string short_name = new char[short_name_length+1];
   memcpy(short_name,full_name+i,short_name_length);
   short_name[short_name_length] = '\0'; // terminate string
   return(short_name);
};

int start_of_basic_file_name(const_ascii_string full_name) {
   int length = strlen(full_name);
   // first skip the extension and .
   int i = length-1;
   int last = i;
//	if (!is_url(full_name)) {
		for (; i > 0; i--) { // search back for : or backslash
			if (full_name[i] == '\\' || full_name[i] == ':'|| full_name[i] == '/') {
				return(i+1);
			};
		};
//	};
   return(-1);
};

java_string gif_file_for_java(const_ascii_string picture_file_name) {
	// completely re-writen on 070201 since JPG and JPEG is OK as well 
	// (too bad the Java class is called TTGIFPicture when it works fine with JPEGs as well)
	// also uses existing_file now since the files can be anywhere on FileSearchPath
	FileNameStatus name_status;
	string full_file_name;
	string extensions[] = {"gif","jpg","jpeg"};
	{ // so unwind protect has correct scope
		UnwindProtect<string> set(tt_URL_cache_directory,append_strings(tt_main_directory,AC(IDC_JAVA_PATH))); // new on 060401
		// first TRUE was FALSE below prior to 060401
		full_file_name = existing_file_name(name_status,picture_file_name,extensions,3,NULL,TRUE,FALSE,FALSE,FALSE,TRUE); 
		// NULL was AC(IDC_JAVA_PATH) prior to 120201
		delete [] tt_URL_cache_directory;
	};
	if (full_file_name != NULL) {
		ascii_string basic_name = basic_file_name(picture_file_name);
		ascii_string extension;
		int extension_start = extension_start_position(full_file_name);
		if (extension_start < 0) { // didn't find out
			extension = "gif"; // default
		} else {
			extension = full_file_name+extension_start;
		};
		if (name_status != FILE_ON_INTERNET) {	// conditional new on 060401 since it is now cached in the right directory if a URL
			char file_on_java_directory[MAX_PATH]; 
			output_string_stream file_name(file_on_java_directory,MAX_PATH);
			file_name << directory_with_java_directory() << AC(IDC_JAVA_PATH) << basic_name << "." << extension; 
			// was just gif prior to 130201 - updated 110702
			file_name.put('\0');
			boolean ignore;
			if (!copy_file_or_url_to_file(full_file_name,file_on_java_directory,ignore)) { 
				tt_error_file() << S(IDS_PROBLEM_COPYING_JAVA_MEDIA_FILE) << basic_name
									 << AC(IDC_FROM) << full_file_name << endl;
				tt_err_file_important = TRUE;
				return(NULL);
			};
		};
		string result = append_3_strings(basic_name,".",extension); // updated on 130201
		delete [] basic_name;
		delete [] full_file_name;
		tt_error_file() << "The java applet needs the following file: " << result << endl; // new on 180201
		return(result);
	};
	return(NULL);
};
										  
/* old version:
java_string gif_file_for_java(const_ascii_string picture_file_name) {
	ascii_string basic_name = basic_file_name(picture_file_name);
	char file_on_java_directory[MAX_PATH];
	output_string_stream file_name(file_on_java_directory,MAX_PATH);
	file_name << tt_main_directory << AC(IDC_JAVA_PATH) << basic_name << ".gif"; 
	file_name.put('\0');
	OFSTRUCT file_struct;
	if (OpenFile(file_on_java_directory,&file_struct,OF_EXIST) != -1) { // already there
		string result = append_strings(basic_name,".gif");
		delete [] basic_name;
		return(result);
	};
	string gif_file;
	if (already_has_extension(picture_file_name,"gif")) { 
		gif_file = copy_string(picture_file_name);
	} else { // else check if the gif is in the same directory as the BMP
		string name_without_extension = file_name_without_extension(picture_file_name); 
		FileNameStatus file_status;
		gif_file = existing_file_name(file_status,name_without_extension,"gif",NULL,TRUE,FALSE); // TRUE added 080900 to prevent spurious warnings
		delete [] name_without_extension;
	};
	if (gif_file != NULL) {
		boolean ignore;
		if (!copy_file_or_url_to_file(gif_file,file_on_java_directory,ignore)) { 
			tt_error_file() << S(IDS_PROBLEM_COPYING_JAVA_MEDIA_FILE) << basic_name << ".gif "
							<< AC(IDC_FROM) << gif_file << endl;
			tt_err_file_important = TRUE;
			return(NULL);
		};
		string result = append_strings(basic_name,".gif");
		delete [] basic_name;
		delete [] gif_file;
		return(result);
	};
	// all of the above is new on 180800
   if (basic_name != NULL) {
      java_string result = ensure_file_for_java(basic_name,PICTURE,FALSE);
      delete [] basic_name;
      return(result);
   };
   return(NULL);
};
*/

java_string au_file_for_java(const_java_string wave_file_name) {
   ascii_string basic_name = basic_file_name(wave_file_name);
   if (basic_name != NULL) {
//      java_string result = ensure_file_for_java(basic_name,SOUND,FALSE);
		// revised on 240303
		string au_file_name = append_strings(basic_name,".au");
		if (tt_commands_to_convert_sounds_to_au == NULL || strstr(au_file_name,tt_commands_to_convert_sounds_to_au) == NULL) {
			// new one
			const int line_max = MAX_PATH*3+64;
			char new_line_buffer[line_max];
			output_string_stream line(new_line_buffer,line_max);
			//char short_wave_file_name[MAX_PATH];
			//GetShortPathName(wave_file_name,short_wave_file_name,MAX_PATH);
			// quote wave_file_name and au_file_name in case it contains spaces
			line << tt_main_directory << "Sox\\sox.exe \"" << wave_file_name << "\" -u \"" << au_file_name << "\"" << endl;
			line.put('\0');
			if (tt_commands_to_convert_sounds_to_au == NULL) {
				tt_commands_to_convert_sounds_to_au = copy_string(new_line_buffer);
			} else {
				string new_tt_commands_to_convert_sounds_to_au = append_strings(new_line_buffer,tt_commands_to_convert_sounds_to_au);
				delete [] tt_commands_to_convert_sounds_to_au;
				tt_commands_to_convert_sounds_to_au = new_tt_commands_to_convert_sounds_to_au;
			};
		};		 
      delete [] basic_name;
      return(au_file_name);
   };
   return(NULL);
};

Picture *make_indirect_picture(Sprite *item, boolean add_too) {
	if (item == NULL) { // new on 220203 for robustness
		return(NULL);
	};
	// caller shouldn't also hold on to item
	city_coordinate item_llx,item_lly;
	item->lower_left_corner(item_llx,item_lly);
	Background *floor = item->pointer_to_background();
	Picture *picture = new Picture(item_llx,item_lly,
											 item->priority(),
											 0,item->kind_of()==PICTURE?item->appearance_code():SYNTHETIC_SPRITE,
											 item->current_width(),
											 item->current_height());
	picture->set_indirection(item,TRUE);
   picture->set_show_all(item->show_all());
   picture->set_visible(item->visible());
	// following commented out on 150299 since it now triggers warnings
//   picture->set_parameter(256); // don't want it to be a rectangle since they are solid for walking programmer
   // a convenient lie since it won't be seen again
	// following commented out on 201202 since that isn't what slated_for_deletion means and 
	// interferes with the ability to abort Bammer when something is destroyed
	//if (item->kind_of() != NEST) { // conditional added 051199 since nests on walls should still receive things
	//	item->set_slated_for_deletion(TRUE);
	//};
//	item->set_is_picture(TRUE); // set by set_indirection now
//	item->set_screen_ref_count(picture->current_screen_ref_count());
	item->set_priority_fixed(TRUE); // doesn't make sense to update this afterwards
   if (!add_too) return(picture);
	if (floor != NULL) {
		floor->add_item(picture,FALSE);
		item->set_background(floor);
		floor->remove_item(item,FALSE);
	} else {
		tt_screen->remove(item);
		tt_screen->add(picture);
	};
	return(picture);
};

void set_relative_size_and_location(Sprite *other_follower, Sprite *other_leader, Sprite *item, Sprite *leader) {
   // sets size and locaton of item so that its relationship to leader is the same as other_follower's to other_leader
	// changed the following from true_ to current_ on 121204 -- e.g. dropping a picture of a house on a house decoration
	city_coordinate other_follower_width = other_follower->current_width();
	city_coordinate leader_width = leader->current_width();
	city_coordinate other_leader_width = other_leader->current_width();
	city_coordinate other_follower_height = other_follower->current_height();
	city_coordinate leader_height = leader->current_height();
	city_coordinate other_leader_height = other_leader->current_height();
	city_coordinate leader_llx = leader->current_llx();
	city_coordinate other_follower_llx = other_follower->current_llx();
	city_coordinate other_leader_llx = other_leader->current_llx();
	city_coordinate leader_lly = leader->current_lly();
	city_coordinate other_follower_lly = other_follower->current_lly();
	city_coordinate other_leader_lly = other_leader->current_lly();
   item->set_size_and_location((other_follower_width*leader_width)/other_leader_width,
                               (other_follower_height*leader_height)/other_leader_height,
                               leader_llx+((other_follower_llx-other_leader_llx)*leader_width)/other_leader_width,
                               leader_lly+((other_follower_lly-other_leader_lly)*leader_height)/other_leader_height);
};

Sprites *sort_items(Sprites *items) {
	// the items are put in the following order:
	// robots without cubbies and non-robots and non-cubbies
	// each cubby followed by its robot(s)
	// cubbies without robots
	// so a robot whose previous item was a cubby "owns" it
	Sprites *remaining = items;
	if (remaining == NULL) return(NULL);
// following commented out on 240399 since it might be a robot whose top_cubby isn't a follower just now but is added to this list
//   if (remaining->rest() == NULL) return(new Sprites(remaining->first())); // don't waste time with below
//   int count = 0;
	Sprites *robots_remaining = NULL;
	Sprites *cubbies_remaining = NULL;
	Sprites *new_items = NULL;
	while (remaining != NULL) {
		Sprite *item = remaining->first();
		if (item->kind_of() == CUBBY) {
			insert_at_end(item,cubbies_remaining);
//         count++;
//			cubbies_remaining = new Sprites(item,cubbies_remaining);
		} else if (item->kind_of() == ROBOT) {
			if (((Robot *) item)->pointer_to_working_cubby() == NULL) {
				// orphan robot (or robots) 
				insert_at_end(item,new_items);
//            count++;
//				if (new_items == NULL) {
//					new_items = new Sprites(item);
//				} else {
//					new_items->insert_at_end(item);
//				};
			} else {
				insert_at_end(item,robots_remaining);
//				robots_remaining = new Sprites(item,robots_remaining);
			};
		} else { // non-robot and non-cubby
//         count++;
			insert_by_priority(item,new_items);
//			if (new_items == NULL) {
//				new_items = new Sprites(item);
//			} else {
//				new_items->insert_at_end(item);
//			};
		};
		remaining = remaining->rest();
	};
	remaining = robots_remaining;
	while (remaining != NULL) { // these have cubbies
		Robot *robot = (Robot *) (remaining->first());
		Sprite *cubby = robot->pointer_to_working_cubby();
#if TT_DEBUG_ON
      if (cubby->pointer_to_background() != robot->pointer_to_background() || cubby->pointer_to_leader() != robot->pointer_to_leader()) {
         log("robot and cubby differ regarding backgrounds or leaders");
      };
#endif
		insert_at_end(cubby,new_items);
		new_items->insert_at_end(robot);
		if (cubbies_remaining != NULL) { // if a robot's cubby isn't an item of the floor for example -- is this OK?
         cubbies_remaining = cubbies_remaining->remove(cubby);
      };
		remaining = remaining->rest();
	};
	if (new_items == NULL) {
		new_items = cubbies_remaining;
	} else {
		new_items->last_cons()->set_rest(cubbies_remaining);
	};
	if (robots_remaining != NULL) {
		robots_remaining->remove_all();
		delete robots_remaining;
	};
//	set_flipped_followers(new_items);
   return(new_items);
};

flag trouble_shooting_flag = FALSE; // used to prevent recursive trouble shooting

boolean trouble_shooting() {
  return(trouble_shooting_flag);
};

void trouble_shoot(Trouble trouble, const_string extra_message, const_string extra_message2, 
						 boolean restore_toontalk_afterwards, boolean now, boolean trouble_shoot_regardless) {
	if (tt_puzzles_over_html_file != NULL && trouble != TROUBLE_SHOOT_GENERAL_TROUBLE) return; 
	// new on 050401 so nothing interferes with the ToonTalk "ad" - unless things are really broken (added 120401)
	// added trouble_shoot_regardless on 100600
   trouble_shooting_flag = TRUE;
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
   write_ini_entry(AC(IDC_SWITCHES),AC(IDC_TOONTALK_STARTED),"0"); // shouldn't trigger trouble shooting next time around
	if (has_initialization_completed() && restore_toontalk_afterwards && (tt_trouble_shooting || trouble_shoot_regardless) &&
		 (extra_message != NULL || extra_message2 != NULL || !did_user(trouble,TROUBLE_SHOOT_HISTORY))) {
      // iconize ToonTalk since at least one message box is coming up
		if (now) {
			// moved later so this happens even if !restore_toontalk_afterwards
//#if TT_DEBUG_ON
//			add_to_log_if_interesting_error();
//#endif
//			pause_toontalk(FALSE,FALSE,tt_shutting_down);
//#if TT_DEBUG_ON
//			add_to_log_if_interesting_error();
//#endif
//			if (!tt_shutting_down) CloseWindow(tt_main_window->get_handle()); // commented out on 070401 since seems to be an invalid handle (how??)
//#if TT_DEBUG_ON
//			add_to_log_if_interesting_error();
//#endif
		} else { // this will get finished on the next cycle
		   tt_current_trouble = trouble;
		   tt_current_extra_message = extra_message;
		   tt_current_extra_message2 = extra_message2;
         return;
		};
   } else {
      restore_toontalk_afterwards = FALSE; // since there are no message boxes coming or its too early
   };
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	int show_dialog_option = ini_int("NeverShowTheseDialogsAgain",trouble_shooting_ini_key[trouble],FALSE,0);
	if (show_dialog_option == 1) { // new on 131203
		// notice that this no longer pays attention to trouble_shoot_regardless or tt_trouble_shooting - 
		// or should it at least for museum mode??
//	if (!tt_trouble_shooting && !trouble_shoot_regardless) {
      if (!did_user(trouble,TROUBLE_SHOOT_HISTORY) && // new conjunct on 081105
			 trouble != TROUBLE_SHOOT_GENERAL_TROUBLE && trouble != TROUBLE_SHOOT_BAD_INSTALLATION &&
			 !(tt_last_history_index == TROUBLE_SHOOT_HISTORY && tt_last_history_action == trouble)) { 
			// last condition new on 121004 to avoid duplicate warnings in log
         log(S(IDS_FIRST_TROUBLE_SHOOT+trouble));
			if (extra_message != NULL) log(extra_message); // new on 010904
			if (extra_message2 != NULL) log(extra_message2);  
			if (!tt_shutting_down) user_did(TROUBLE_SHOOT_HISTORY,trouble); 
			// if they read the log - but no point doing this again - new on 130204
			// following new on 050805
			tt_error_file() << "The above trouble shooting advice was not offered when the problem occurred since the key "
								 << trouble_shooting_ini_key[trouble] 
								 << " in the NeverShowTheseDialogsAgain section of toontalk.ini was set to 1." << endl;
         // following added so warning doesn't appear many times in log
			if (!tt_shutting_down) user_did(TROUBLE_SHOOT_HISTORY,trouble);
		};
		restore_toontalk_afterwards = FALSE; // new on 010904 since didn't display a dialog
   } else if (!did_user(trouble,TROUBLE_SHOOT_HISTORY) || trouble == TROUBLE_SHOOT_TRIAL_PERIOD_OVER) { 
		// haven't already told user about this problem
		if (extra_message != NULL) message_box(MB_ICONINFORMATION|MB_SYSTEMMODAL,extra_message,NULL,FALSE); // moved here on 010904
		if (extra_message2 != NULL) message_box(MB_ICONINFORMATION|MB_SYSTEMMODAL,extra_message2,NULL,FALSE); 
      if (!tt_shutting_down) user_did(TROUBLE_SHOOT_HISTORY,trouble); // if shutting down might not be able to save history
      const_string help;
      if (trouble != TROUBLE_SHOOT_GENERAL_TROUBLE && trouble != TROUBLE_SHOOT_BAD_INSTALLATION) {
         help = S(IDS_FIRST_TROUBLE_SHOOT+trouble);
			// following doesn't use string library because things may be so broken that it is not available
		} else if (tt_language == SWEDISH) {
			help = C(IDC_TROUBLE_SHOOTING_HELP_SWEDISH);
      } else {
         help = C(IDC_TROUBLE_SHOOTING_HELP);
      };
		if (help == NULL) { //  new on 101202 since crash report after control-alt-delete shows help == NULL possible
			//  e.g. no string library??
			tt_trouble_shooting = FALSE;
			tt_shutting_down = TRUE;
			log("Trouble while trouble shooting. Exiting immediately.");
			return;
//			tt_exit(EXIT_FAILURE,_T("Trouble while trouble shooting. Exiting immediately."),_T(""));
		};
		log(help);
		string dialog_check_mark_name = NULL;
#if TT_ALPHA_FEATURE
		if (show_dialog_option == 0) {
			dialog_check_mark_name = trouble_shooting_ini_key[trouble];
		};
#endif
		// following moved here on 290204
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		pause_toontalk(FALSE,FALSE,tt_shutting_down);
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		if (message_box(MB_YESNO|MB_SYSTEMMODAL,help,NULL,FALSE,dialog_check_mark_name) == IDYES) { // last arg new on 131203
			launch_help(AS(IDS_FIRST_TROUBLE_SHOOT_HELP_FILE+trouble));
			trouble_shooting_flag = FALSE;
			return;
		};
   };
   if (restore_toontalk_afterwards && !tt_shutting_down) {
      unpause_toontalk();
      OpenIcon(tt_main_window->get_handle());
   };
   trouble_shooting_flag = FALSE;
};

#if TT_DEBUG_ON
LARGE_INTEGER start_dump; 
LARGE_INTEGER stop_dump;
long total_time_dumping_shared_times = 0;
#endif

int about_to_dump(Sprite *sprite, output_stream &stream) {
   // beginning 020802 returns index if new, max_long if no index needed, and otherwise a negative int to indicate taken care of herein
	if (!tt_dumping_with_sharing_of_all_types && // and isn't a kind of thing that always needs to deal with sharing
		 (sprite->return_ref_count() < 2 && !tt_dumping_to_test_equality_or_describe) && 
		 // new condition as of 180100 - !tt_dumping_to_test_equality_or_describe added on 120201 since equal was broken
		 sprite->kind_of() != NEST && sprite->kind_of() != PROGRAM_PAD && sprite->kind_of() != PICTURE &&
		 sprite->kind_of() != ROBOT) { // added on 271000 since keeps pointer to things like first_team_member_to_fail_for_unstable_reasons
		return(max_long);
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 270900) {
		QueryPerformanceCounter(&start_dump);
	};
#endif
   long index = -1;
	boolean found = FALSE;
	boolean only_equal = FALSE; // new on 040601 - i.e. not just eq
	/*
   if (tt_sprites_being_dumped != NULL) index = tt_sprites_being_dumped->index_of(sprite,found);
   if (!found) { // new one
      insert_at_end(sprite,tt_sprites_being_dumped);
      index = tt_sprites_being_dumped->length()-1;
      if (index > max_unsigned_short) {
         tt_error_file() << S(IDS_TOO_MANY_THINGS_TO_DUMP) << endl;
         return(FALSE); // dump without sharing -- OK??
      };
	*/
	// replaced the above with the following on 270900 for better performance
	for (int i = 0; i < tt_number_of_sprites_dumped_so_far; i++) { // rewritten on 110601
		if (tt_sprites_being_dumped[i] == sprite && tt_sprites_being_dumped_ok_for_eq[i]) { 
			// already there and not one that is left over from a thought bubble dump
			index = i;
			found = TRUE;
			break;
		};
	};
	if (!found && tt_dump_so_equals_share && !tt_dumping_to_test_equality_or_describe && sprite->kind_of() != NEST && sprite->kind_of() != BIRD) { 
		// new on 040601 - first check if each -- 
		// added bird and nest conditions on 120601 since equal sharing of them doesn't make sense (does it make sense for numbers?)
		for (int i = 0; i < tt_number_of_sprites_dumped_so_far; i++) { 
			if (equal(tt_sprites_being_dumped[i],sprite,DUMPING_EQUALS_TOGETHER)) { 
				index = i; 
				// rewritten on 110601 - was j-tt_skip_first_n_dumped; // -tt_skip_first_n_dumped new on 080601 so top_level_dump has right indices
				found = TRUE;
				only_equal = TRUE;
				break;
			};
		};	
	};
	if (!found) { 
		if (tt_number_of_sprites_dumped_so_far == tt_sprites_being_dumped_size) { // need to grow
#if TT_DEBUG_ON
         if (tt_debug_mode == 150702) {
            tt_error_file() << "[7] tt_sprites_being_dumped_size=" << tt_sprites_being_dumped_size 
									 << "; tt_number_of_sprites_dumped_so_far=" << tt_number_of_sprites_dumped_so_far << endl;
         };
#endif
			int new_size = tt_sprites_being_dumped_size*2; // double it
			if (new_size < 32) new_size = 32; // first time allocate a reasonable amount 
			SpritePointer *new_sprites_being_dumped = new SpritePointer[new_size];
			flag *new_sprites_being_dumped_ok_for_eq = new flag[new_size];
			long i;
			for (i = 0; i < tt_sprites_being_dumped_size; i++) {
				new_sprites_being_dumped[i] = tt_sprites_being_dumped[i];
				new_sprites_being_dumped_ok_for_eq[i] = tt_sprites_being_dumped_ok_for_eq[i];
			};
			// removed i = tt_sprites_being_dumped_size from first part of for loop below on 120803
			for (; i < new_size; i++) { // was tt_sprites_being_dumped_size+1 prior to 150202
				new_sprites_being_dumped[i] = NULL;
				new_sprites_being_dumped_ok_for_eq[i] = TRUE;
			};
			if (tt_sprites_being_dumped != NULL) {
				delete [] tt_sprites_being_dumped;
			};
			if (tt_sprites_being_dumped_ok_for_eq != NULL) { // removed from conditional above on 240601 to be sure it is deleted (doubt it matters)
				delete [] tt_sprites_being_dumped_ok_for_eq;
			};
			tt_sprites_being_dumped = new_sprites_being_dumped;
			tt_sprites_being_dumped_ok_for_eq = new_sprites_being_dumped_ok_for_eq;
			tt_sprites_being_dumped_size = new_size;
#if TT_DEBUG_ON
         if (tt_debug_mode == 150702) {
            tt_error_file() << "[5] tt_sprites_being_dumped_size=" << tt_sprites_being_dumped_size
									 << "; tt_number_of_sprites_dumped_so_far=" << tt_number_of_sprites_dumped_so_far << endl;
         };
#endif
		};
		index = tt_number_of_sprites_dumped_so_far;
		tt_number_of_sprites_dumped_so_far++;
		tt_sprites_being_dumped[index] = sprite;
		tt_sprites_being_dumped_ok_for_eq[index] = TRUE;
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
			print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
			tt_error_file() << "About to dump NEW item #" << index << endl;
			if (tt_debug_target == -index && index != 0) { // negative so no conflict with debug_counter
				print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
				tt_error_file() << "Dumping index target." << endl;
			};
		};
      if (tt_debug_mode == 270900) {
			QueryPerformanceCounter(&stop_dump);
			LARGE_INTEGER frequency;
			QueryPerformanceFrequency(&frequency);
			total_time_dumping_shared_times += (long) ((1000000*(stop_dump.QuadPart-start_dump.QuadPart))/frequency.QuadPart);
			tt_error_file() << "Total microseconds to dump shared items: " << total_time_dumping_shared_times << " (new item)" << endl;
		};
#endif
//      if (!tt_dump_as_xml) { // condition new on 020802 
		   stream.put(NEW_SHARED_ITEM);
         unsigned short int dump_index = (unsigned short int) index;
         stream.write((string) &dump_index, sizeof(dump_index));
//      };
      return(index); // dump as normal - // new on 020802 return index so caller can include it in its description
   };
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
		print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
		tt_error_file() << "About to dump OLD item #" << index << endl;
		if (index == -tt_debug_target && index != 0) {
			log("debug old item index");
		};
	};
#endif
	boolean dump_geometry_too = FALSE;
//   string xml_close_tag = NULL;
	if (only_equal) {
		if (sprite->kind_of() == PICTURE && 
			(sprite->pointer_to_leader() == NULL || 
			 (sprite->pointer_to_leader()->kind_of() == PICTURE && !((Picture *) sprite->pointer_to_leader())->is_flipped()) ||
			 (sprite->pointer_to_leader()->like_a_picture()))) { 
		   // only pictures without a leader or whose leader is an unflipped picture need to save geometry
         //if (tt_dump_as_xml) {
         //   stream << "<IsCopyOfWithNewGeometry><IsCopyOf>" << endl;
         //   xml_close_tag = "</IsCopyOf>";
         //} else {
			   stream.put(EQUAL_TO_OLD_ITEM_AND_GEOMETRY);
			   dump_geometry_too = TRUE;
//         };
		} else {
         //if (tt_dump_as_xml) { // revisit this soon
         //   stream << "<IsCopyOf>" << endl;
         //   xml_close_tag = "</IsCopyOf>";
         //} else {
			   stream.put(EQUAL_TO_OLD_ITEM);
//         };
		};
	} else {
//#if TT_DEBUG_ON
//      if (sprite->kind_of() != PICTURE && sprite->kind_of() != BIRD && sprite->kind_of() != CUBBY && sprite->kind_of() != ROBOT && 
				//sprite->kind_of() != NEST && sprite->kind_of() != PROGRAM_PAD) {
//         log("debug this");
//      };
//#endif
      //if (tt_dump_as_xml) {
      //   stream << "<RefersTo>" << endl;
      //   xml_close_tag = "</RefersTo>";
      //} else {
		   stream.put(OLD_SHARED_ITEM);
//      };
	};
//   if (tt_dump_as_xml) {
//      stream << index << xml_close_tag;
//      if (dump_geometry_too) {
//         sprite->xml_dump_geometry(stream);
//         stream << endl << "</IsCopyOfWithNewGeometry>" << endl;
//      };
//   } else {
      unsigned short int dump_index = (unsigned short int) index;
      stream.write((string) &dump_index, sizeof(dump_index));
	   if (dump_geometry_too) {
		   ((Picture *) sprite)->dump_geometry(stream);
	   };
//   };
#if TT_DEBUG_ON
	if (tt_debug_mode == 270900) {
		QueryPerformanceCounter(&stop_dump);
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		total_time_dumping_shared_times += (long) ((1000000*(stop_dump.QuadPart-start_dump.QuadPart))/frequency.QuadPart);
		tt_error_file() << "Total microseconds to dumping shared items: " << total_time_dumping_shared_times << " (old item)" << endl;
	};
#endif
   return(-1); // that's all -- caller need do no more
};

void release_sprites_being_dumped() {
   if (tt_sprites_being_dumped != NULL) {
//      tt_sprites_being_dumped->remove_all();
//      delete tt_sprites_being_dumped;
		// commented out the following on 120803 - don't care about the leak since this is only if SaveInXML is off
		//for (int i = 0; i < tt_sprites_being_dumped_size; i++) {
		//	tt_sprites_being_dumped[i] = NULL; // new on 120803 despite only applies if SaveInXML is off
		//};
		//delete [] tt_sprites_being_dumped; // revised 270900
      tt_sprites_being_dumped = NULL;
		tt_sprites_being_dumped_size = 0;
		tt_number_of_sprites_dumped_so_far = 0;
   };
	if (tt_sprites_being_dumped_ok_for_eq != NULL) {
		// commented out the following on 120803 - don't care about the leak since this is only if SaveInXML is off
//		delete [] tt_sprites_being_dumped_ok_for_eq;
 		tt_sprites_being_dumped_ok_for_eq = NULL;
	};
#if TT_DEBUG_ON
   if (tt_debug_mode == 150702) {
      tt_error_file() << "[6] tt_sprites_being_dumped_size=" << tt_sprites_being_dumped_size 
							 << "; tt_number_of_sprites_dumped_so_far=" << tt_number_of_sprites_dumped_so_far << endl;
   };
#endif
	release_nest_indices();
};

Sprites *copied_items = NULL; // maintained as (original1 copy1 original2 copy2 ...)

void just_copied(Sprite *original, Sprite *copy) {
#if TT_DEBUG_ON
	if (original->debug_counter == tt_debug_target) {
		log("Just copied debug target.");
	};
#endif
	if (tt_top_copy != NULL) {
		copied_items = new Sprites(original,new Sprites(copy,copied_items));
	};
};

Sprite *already_copied(Sprite *original) {
#if TT_DEBUG_ON
	if (original->debug_counter == tt_debug_target) {
		log("Checking if already copied debug target.");
	};
   //millisecond start_time = timeGetTime();
#endif
	if (tt_top_copy == NULL) return(NULL);
   Sprites *remaining = copied_items;
   while (remaining != NULL) {
      if (remaining->first() == original) {
//#if TT_DEBUG_ON
//         if (tt_debug_mode == 300702) {
//            tt_error_file() << "Already copied. Took " << (timeGetTime()-start_time) << "ms." << endl;
//         };
//#endif
         return(remaining->rest()->first());
      };
      remaining = remaining->rest()->rest();
   };
//#if TT_DEBUG_ON
//   if (tt_debug_mode == 300702) {
//      tt_error_file() << "Not already copied. Took " << (timeGetTime()-start_time) << "ms." << endl;
//   };
//#endif
   return(NULL);
};

boolean is_a_current_copy(Sprite *copy) {
	if (copied_items == NULL) return(FALSE);
	Sprites *remaining = copied_items;
   while (remaining != NULL) {
		remaining = remaining->rest(); // is a "p-list" so skip odd elements
      if (remaining->first() == copy) {
         return(TRUE);
      };
      remaining = remaining->rest();
   };
   return(FALSE);
};

boolean copied_bird_has_as_original_nest(Sprite *nest) {
	if (copied_items == NULL) return(FALSE);
	Sprites *remaining = copied_items;
   while (remaining != NULL) {
		remaining = remaining->rest(); // is a "p-list" so skip odd elements
      if (remaining->first()->kind_of() == BIRD) {
			Bird *bird = (Bird *) remaining->first();
			if (bird->pointer_to_nest() == nest) {
				return(TRUE);
			};
      };
      remaining = remaining->rest();
   };
   return(FALSE);
};

Sprites *save_copied_items() {
	Sprites *result = copied_items;
	copied_items = NULL;
	return(result);
};

void restore_copied_items(Sprites *items) {
	copied_items = items;
};

void release_items_copied() {
   if (copied_items != NULL) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 27100 || tt_debug_mode == 133) {
			tt_error_file() << "Releasing " << (copied_items->length()) << " copied items." << endl;
		};
#endif
      copied_items->remove_all();
      delete copied_items;
      copied_items = NULL;
   };
};

void leave_room_for_growth(output_stream &stream, int number_of_bytes) {
	// for growth without breaking old format since when not zero is how much room the new stuff takes
   // number_of_bytes should be 1, 2, or 4 depending how much growth is plausible
	for (int i = 0; i < number_of_bytes; i++) {
		stream.put('\0');
	};
	// here's the more "logical" equivalent
// unsigned long growth_block_length = 0;
//	log_out.write((char *) &growth_block_length,sizeof(growth_block_length)); 
};

unsigned long ignore_growth(InputStream *stream, int number_of_bytes) {
	unsigned long extra;
	if (number_of_bytes == 1) {
		//if (tt_log_in_archive != NULL) { // new branch on 030703
		//	unsigned char buffer[1];
		//	read_current_file_in_unzip_archive(tt_log_out_archive,buffer,1);
		//	extra = buffer[0];
		//} else {
			extra = stream->get();
//		};
	} else if (number_of_bytes == 2) {
		unsigned short short_extra;
		//if (tt_log_in_archive != NULL) { // new branch on 030703
		//	read_current_file_in_unzip_archive(tt_log_out_archive,(char *) &short_extra,sizeof(short_extra));
		//} else {
			stream->read((string) &short_extra,sizeof(short_extra));
//		};
		extra = short_extra;
	} else { // check first??
		//if (tt_log_in_archive != NULL) { // new branch on 030703
		//	read_current_file_in_unzip_archive(tt_log_out_archive,(char *) &extra,sizeof(extra));
		//} else {
			stream->read((string) &extra,sizeof(extra));
//		};
	};
	if (extra > 0) {
		//if (tt_log_in_archive != NULL) {
		//	bytes buffer = new byte[extra];
		//	read_current_file_in_unzip_archive(tt_log_out_archive,buffer,extra);
		//	delete [] buffer;
		//} else {
		stream->ignore(extra); // rewritten on 070703
			//for (unsigned long i = 0; i < extra; i++) {
			//	stream->get();
			//};
//		};
	};
	return(extra);
};

void dump_if_not_null(output_stream &stream, Sprite *sprite) {
   if (sprite != NULL) {
      sprite->dump(stream);
   } else {
      stream.put(NOTHING_MARKER);
   };
};

void dump_string(output_stream &stream, string text, long max_size) {
	long length = 0;
	if (text != NULL) length = _tcslen(text);
	if (max_size <= 255) {
		if (length > 255) length = 255;
		stream.put((unsigned char) length);
	} else if (max_size <= max_unsigned_short) {
		if (length > max_unsigned_short) length = max_unsigned_short;
		unsigned short int len = (unsigned short int) length;
		stream.write((string) &len, sizeof(len));
	} else {
		unsigned long int len = (unsigned long int) length;
		stream.write((string) &len, sizeof(len));
	};
	stream.write(text,length);
};

void load_string(InputStream *stream, string text, long max_size) {
	long length;
	if (max_size <= 255) {
		length = stream->get();
	} else if (max_size <= max_unsigned_short) {
		unsigned short int len;
		stream->read((string) &len, sizeof(len));
		length = len;
	} else {
		unsigned long int len;
		stream->read((string) &len, sizeof(len));
		length = len;
	};
	stream->read((string) text,length);
	text[length] = '\0'; // terminate string
};

void load_started() {
	tt_loads_current++;
};

void load_ended() {
	tt_loads_current--;
	if (tt_loads_current == 0) { // not a nested load
		release_nest_indices(); // in case built up table to keep bird/nest corespondences
		release_shared_load_items();
	};
};

void dump_started() {
	tt_loads_current++; // since a load and dump shouldn't happen simultaenously re-use the same variable
};

void dump_ended() {
	tt_loads_current--;
	if (tt_loads_current == 0) { // not a nested dump
		release_sprites_being_dumped();
	};
};

#if TT_WIDE_TEXT
wide_string update_string(wide_string text, unsigned_character key, boolean extended, long max_length) {
	// typing key to add or rubout
	if (extended) { // for now - new on 100604
		return(text);
	};
	int text_size = 0;
	if (text != NULL) text_size = wide_string_length(text);
	if (rubout(key)) {
		if (text_size == 0) { // rubing out an empty string
			return(text);
		};
		text[text_size-1] = NULL; // terminate one character earlier
		return(text);
	} else if (text_size < max_length) {
		wide_string new_text = new wide_character[text_size+2];
		// changed key to unsigned_character on 310899 so that extra characters like Swedish ・work OK
		int byte_count; // re-written this way on 250800 so that single byte Japanese characters work OK
		char double_byte[2];
		if (tt_second_byte_of_double_byte_input != 0) { // new on 220800
			byte_count = 2;
			double_byte[1] = key;
			double_byte[0] = tt_second_byte_of_double_byte_input;
		} else {
//			new_text[text_size] = key;
			byte_count = 1;
			double_byte[0] = key;
		};
		wide_string unicode_character = copy_wide_string(double_byte,byte_count);
		new_text[text_size] = unicode_character[0];
#if TT_DEBUG_ON
		if (tt_debug_mode == 240800) {
			tt_error_file() << (byte) key << " and " << (byte) tt_second_byte_of_double_byte_input << " become " << (int) unicode_character[0] << endl;
		};
#endif
		delete [] unicode_character;
		new_text[text_size+1] = NULL;
		for (int i = 0; i < text_size; i++) {
			new_text[i] = text[i];
		};
		if (text != NULL) delete [] text;
		return(new_text);
	} else { // beep or warn that name is too long
		return(text);
	};
};
#endif

string update_string(string text, unsigned_character key, boolean extended, long max_length) {
	// typing key to add or rubout
	int text_size = 0;
	if (text != NULL) text_size = _tcslen(text);
	if (rubout(key)) {
		if (text_size == 0) { // rubing out an empty string
			return(text);
		};
		text[text_size-1] = NULL; // terminate one character earlier
		return(text);
	} else if (text_size < max_length) {
		string new_text = new character[text_size+3]; // was +2 prior to 220800 but might be double byte character
		if (tt_second_byte_of_double_byte_input != 0) { // new on 220800
			new_text[text_size] = tt_second_byte_of_double_byte_input;
			new_text[text_size+1] = key;
			new_text[text_size+2] = NULL;
#if TT_DEBUG_ON
			if (tt_debug_mode == 240800) {
				tt_error_file() << (byte) key << " and " << (byte) tt_second_byte_of_double_byte_input << " added to string." << endl;
			};
#endif
		} else {
			new_text[text_size] = key;
			new_text[text_size+1] = NULL;
		};
		for (int i = 0; i < text_size; i++) {
			new_text[i] = text[i];
		};
		if (text != NULL) delete [] text;
		return(new_text);
	} else { // beep or warn that name is too long
		return(text);
	};
};

void copy_alphanumerics(ascii_string from, ascii_string to, long max_size) {
	long j = 0;
   long i;
	long length = strlen(from);
	for (i = 0; i < length; i++) {
		if (IsCharAlphaNumericG(from[i])) { // was isalnum -- remove non-alphanumerics
			to[j++] = from[i];
		};
		if (j >= max_size) break;
	};
	to[j] = NULL;    // terminate string
};

void keep_only_alphanumerics(ascii_string text) {
	long length = strlen(text);
   long j = 0;
	for (int i = 0; i < length; i++) {
		if (IsCharAlphaNumericG(text[i])) {
			text[j] = text[i];
			j++;
		};
	};
	text[j] = NULL; // terminate it
};

void set_houses_to_a_block(int houses_to_a_block) {
	if (houses_to_a_block < 1) return; 
	// second disjunct added on 120603 -- moved on conjunct below on 080703 since still need to reset variables below
	boolean changed = (tt_houses_to_a_block != houses_to_a_block); // new on 180803 to compute this before setting (clobbering)
	tt_houses_to_a_block = houses_to_a_block;
	tt_block_width = ideal_screen_width*houses_to_a_block;
	tt_ideal_block_width = tt_block_width;
	if (tt_city != NULL && changed) {
		tt_city->initialize(); // new on 110603
	};
//	adjust_width_to_640x480_screen(tt_block_width);
};

void put_on_clipboard(Sprite *sprite, Background *floor, Sprite *appearance, char key) {
//#if TT_DEBUG_ON&&TT_XML
////    Sprite *test = xml_load_sprite("c:\\temp\\temp.xml");
//   Cubby *box = new Cubby();
//   Text *text = new Text();
//   text->set_text("A test");
//   box->set_number_of_holes(3);
//	box->initialize_hole(0,text->copy());
//   box->initialize_hole(1,text);
//   box->initialize_hole(2,text);
//   sprite = box;
//#endif
	if (sprite == NULL) { // new policy on 150599 is to copy the floor (if on it) or house (if in it) if hand is empty
		if (floor != NULL) {
			record_event(KEYBOARD_CHARACTER,appearance,floor,NULL,TRUE,key);
		   floor->dump_to_clipboard();
		}; // else warn??
		return;
	};
	string text_dropped;
   long text_length;
   boolean is_text = FALSE;
	// added sprite->remote_status() == NOT_REMOTE on 151299 so text is put on clipboard as text
	// and removed !sprite->active() since an inactive sensor shouldn't be saved as text
//	if (sprite->remote_status() == NOT_REMOTE && sprite->kind_of() != PICTURE) {
#if TT_DEBUG_ON
	DWORD start;
	if (tt_debug_mode == 120105) {
		start = timeGetTime();
	};
#endif
	if (sprite->ok_to_dump_as_text() && sprite->no_flipped_followers()) { // re-written on 041201 since numbers like 'x1' saved wrong
		// new on 300699 so that remote controls aren't saved as text
		// added on 060899 sprite->kind_of() != PICTURE since will lose color and the like
		is_text = sprite->current_text(text_dropped,text_length);
   };
#if TT_DEBUG_ON
	if (tt_debug_mode == 120105) {
		tt_error_file() << "Took " << (timeGetTime()-start) << "ms to determine that the sprite's ok to dump as text is " << (int) is_text << endl;
	};
#endif
   if (is_text && text_dropped != NULL) text_dropped[text_length] = '\0'; // terminate
	if (is_text) {
		//&& sprite->kind_of() != SOUND && sprite->kind_of() != FORCE_FEEDBACK && // force feedback added on 160801 since otherwise becomes plain text
      // !(sprite->kind_of() == PICTURE && !sprite->no_flipped_followers())) {
      // unless the text is a picture with stuff on the back dump as text
		tt_main_window->add_to_clipboard(text_dropped,text_length);
   } else {
  //    flag saved_dumping_to_clipboard = tt_dumping_to_clipboard;
		//tt_dumping_to_clipboard = TRUE;
		UnwindProtect<DumpReason> set(tt_dump_reason,DUMPING_TO_CLIPBOARD);
      if (tt_dump_as_xml) { // not sure if the order of this and is_text is OK?
#if TT_XML
         // new on 010802
		   tt_main_window->add_to_clipboard(sprite->xml_global_handle());
#else
         tt_error_file() << "SaveInXML is set to 1 in toontalk.ini but this version of ToonTalk does not support XML." << endl;
         tt_dump_as_xml = FALSE;
#endif
	   } else { // else put encoding string there
		   long encoding_length;
		   string encoding = sprite->return_encoding(encoding_length,FALSE,FALSE);
		   HGLOBAL encoding_handle = uuencode(encoding,encoding_length,sprite->kind_of()); 
			// last arg new on 090600 since birds and nests don't want long preamble
		   tt_main_window->add_to_clipboard(encoding_handle);
		   delete [] encoding;
	   };
//      tt_dumping_to_clipboard = saved_dumping_to_clipboard;
   };
	if (floor != NULL) record_event(KEYBOARD_CHARACTER,appearance,floor,NULL,TRUE,key);
};

Sprite *read_from_clipboard(boolean more_than_one_ok, boolean check_files_too) { //  check_files_too added 280404
	// following commented out on 010503 since now can paste multiple files and they are recorded correctly
   //if (!replaying()) { // new on 111102
	  // HDROP drop_handle = tt_main_window->get_drop_handle();
	  // if (drop_handle != NULL) {
   //      if (!tt_logging || DragQueryFile(drop_handle,0xFFFFFFFF,NULL,0) != 1) { 
	// conditions new on 111102 since otherwise wouldn't make it into the log
		 //     // if logging and many files dragged onto to ToonTalk then log won't work...
   //         return(sprites_from_drop_handle(drop_handle,FALSE)); // second arg should really be more_than_one_ok but Windows doesn't allow it
   //      };
   //   };
   //};
	Sprite *result = NULL;
	boolean new_text;
	string clipboard_text = tt_main_window->clipboard_text(new_text,&result,FALSE,FALSE,check_files_too);
	if (clipboard_text != NULL) {
		tt_uudecoding_a_sprite = TRUE; // prevents looping during loading 
		// not sure if this looping can happen with XML but 
		// beginning 070803 used to indicate you want a copy of a sprite already registered with that GUID
		result = uudecode_sprite(clipboard_text,TRUE,TRUE,more_than_one_ok); // args added on 080200
		tt_uudecoding_a_sprite = FALSE;
		if (new_text) delete [] clipboard_text; // new on 281199
	};
	return(result);
};

// noticed obsolete on 020105
//void add_items_from_outside(Sprites *items, Background *floor, Sprite *appearance,
//									 city_coordinate x, city_coordinate y) {
//	Sprites *remaining = items;
//	while (remaining != NULL) {
//		add_item_from_outside(remaining->first(),floor,appearance,x,y);
//		remaining = remaining->rest();
//	};
//};

void add_item_from_outside(Sprite *item, Background *floor, Sprite *appearance, 
									city_coordinate x, city_coordinate y, boolean change_to_good_size) {
	if (floor == NULL) return; // new on 170101 (e.g. control-v when city is on clipboard)
	if (item == tt_global_picture) { 
		// prior to 180803 was item->global() // new on 040803 since this is what is returned when a non-sprite (e.g. a house or floor) is loaded
		return;
	};
	if (item != NULL) {
		if (floor->showing_on_a_screen()) play_sound(MAGIC_SOUND); // moved here on 260904 so this happens even if following is true
		if (item->held()) { // e.g. control-c of a tool (friend of Tooly) and then control-v -- new on 260904
			return; // nothing more to do
		};
		floor->add_item(item);
		switch (my_random(4)) {
			case 0:
				item->move_to(-2*ideal_screen_width,-2*ideal_screen_height);
				break;
			case 1:
				item->move_to(2*ideal_screen_width,-2*ideal_screen_height);
				break;
			case 2:
				item->move_to(2*ideal_screen_width,2*ideal_screen_height);
				break;
			case 3:
				item->move_to(-2*ideal_screen_width,2*ideal_screen_height);
				break;
		};
		city_coordinate good_width, good_height;
		if (change_to_good_size) { // conditional and arg new on 220803
			item->good_size(good_width,good_height);
		} else {
			good_width = item->current_width();
			good_height = item->current_height();
		};
		if (appearance == NULL) {
			// treat x and y as center location
			item->animate_location(x-good_width/2,y-good_height/2,clipboard_paste_duration);
		} else {
			item->animate_location(appearance->current_llx()+tile_width,appearance->current_lly()+tile_height,clipboard_paste_duration);
		};
//		item->set_size(1,1); // commented out on 041299 since sometimes it messes up the geometry
		if (change_to_good_size) {
			item->animate_size(good_width,good_height,clipboard_paste_duration); // new on 150799
		};
		item->do_after(clipboard_paste_duration,item,NOW_ON_FLOOR_CALLBACK); 
		// restored NOW_ON_FLOOR_CALLBACK on 260103 - was RELEASED_CALLBACK starting 170103 - NOW_ON_FLOOR_CALLBACK now obsolete // new on 121099
	};
};

void add_item_from_clipboard(Background *floor, Sprite *appearance, char key) { // new on 171099
	Sprite *from_clipboard = read_from_clipboard(TRUE);
	if (from_clipboard == tt_global_picture) { // a hack to indicate that this has been taken care of and don't want to see the text pad equivalent
		return; // new on 270904
	};
	if (from_clipboard == NULL) {
		return; // warn too??
	};
	add_item_from_outside(from_clipboard,floor,appearance,0,0,FALSE);
	record_event(KEYBOARD_CHARACTER,appearance,floor,NULL,TRUE,key);
	record_event(NEW_ITEM,appearance,floor,from_clipboard,FALSE,key);	
};

HRESULT ResolveIt(HWND hwnd, LPCSTR lpszLinkFile, LPSTR lpszPath) 
{ // copied from MS documentation
    HRESULT hres; 
    IShellLink* psl; 
    char szGotPath[MAX_PATH]; 
    char szDescription[MAX_PATH]; 
    WIN32_FIND_DATA wfd; 
 
    *lpszPath = 0; // assume failure 
 
    // Get a pointer to the IShellLink interface. 
    hres = CoCreateInstance(CLSID_ShellLink, NULL, // was &CLSID_ShellLink
            CLSCTX_INPROC_SERVER, IID_IShellLink, (void **) &psl); // was &IID_IShellLink
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
    if (SUCCEEDED(hres)) { 
        IPersistFile* ppf; 
 
        // Get a pointer to the IPersistFile interface. 
 //       hres = psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile,&ppf); 
		  hres = psl->QueryInterface(IID_IPersistFile, (void **) &ppf);
#if TT_DEBUG_ON
		  add_to_log_if_interesting_error();
#endif             
        if (SUCCEEDED(hres)) { 
            WCHAR wsz[MAX_PATH]; // prior to 161105 was WORD
 
            // Ensure that the string is Unicode. 
            MultiByteToWideChar(CP_ACP, 0, lpszLinkFile, -1,
#if !TT_MICROSOFT
 // Borland C++ seems to insist on this...
            (wchar_t *) 
#endif
            wsz,MAX_PATH);

            // Load the shortcut. 
            hres = ppf->Load(
#if !TT_MICROSOFT
// Borland C++ seems to insist on this...
            						(wchar_t *) 
#endif
            						wsz,STGM_READ);
#if TT_DEBUG_ON
				add_to_log_if_interesting_error();
#endif
            if (SUCCEEDED(hres)) { 
 
                // Resolve the link. 
//                hres = psl->Resolve(hwnd, SLR_ANY_MATCH); 
					 // replaced above with following on 091000
					 hres = psl->Resolve(hwnd, SLR_NO_UI);
#if TT_DEBUG_ON
					add_to_log_if_interesting_error();
#endif
                if (SUCCEEDED(hres)) { 
 
                    // Get the path to the link target. 
                    hres = psl->GetPath(szGotPath, 
                        MAX_PATH, (WIN32_FIND_DATA *)&wfd, 
                        SLGP_SHORTPATH ); 
#if TT_DEBUG_ON
						  add_to_log_if_interesting_error();
#endif
                    if (!SUCCEEDED(hres)) 
                        return(hres);
 
                    // Get the description of the target. 
                    hres = psl->GetDescription(szDescription, MAX_PATH); 
                    if (!SUCCEEDED(hres)) 
                        return(hres); 
                    lstrcpy(lpszPath, szGotPath); 
                } 
            } 
        // Release the pointer to the IPersistFile interface. 
        ppf->Release(); 
        } 
    // Release the pointer to the IShellLink interface. 
    psl->Release(); 
    } 
#if TT_DEBUG_ON
	 add_to_log_if_interesting_error();
#endif
    return hres; 
};

string extract_file_name_from_link(string file_name) { // new on 071000
	if (file_name == NULL || file_name[0] == NULL) return(NULL); // second condtion added 111200 to improve robustness with bad links
	if (already_has_extension(file_name,"lnk",FALSE)) { // conditional moved here on 091000
		string new_file_name = new char[MAX_PATH];
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		if (SUCCEEDED(ResolveIt(NULL,file_name,new_file_name))) { 
			// NULL was tt_main_window->get_handle() prior to 091000 but don't want dialog boxes
//			tt_error_file() << "Resolved " << file_name << " to " << new_file_name << endl;
#if TT_DEBUG_ON
			add_to_log_if_interesting_error();
#endif
			return(new_file_name);
		} else {
			tt_error_file() << "Warning: couldn't find file linked to from " << file_name << endl; // moved here on 091000
			tt_err_file_important = TRUE;
			return(NULL);
		};
	} else if (already_has_extension(file_name,"url",FALSE)) { // new on 091000
		string new_file_name = new char[MAX_PATH];
		int count = GetPrivateProfileString("InternetShortcut","URL","",new_file_name,MAX_PATH,file_name);
		if (count == 0) {
			delete [] new_file_name;
			return(NULL); 
		};
//		tt_error_file() << "Resolved " << file_name << " to " << new_file_name << endl;
		return(new_file_name);
	};
	return(copy_string(file_name));
};

boolean has_picture_extension(string full_file_name) { // abstracted on 180204
	const int picture_extensions_count = 7;
	// is ini a bad idea?? -- maybe should be an archive? - asked 180204
	string picture_extenions[picture_extensions_count] = {"bmp","png","jpg","gif","dib","jpeg","ini"}; 
	// jpeg added on 200201; ini added 260901
	return(already_has_extension(full_file_name,picture_extenions,picture_extensions_count));
};

boolean has_sound_extension(string full_file_name) { // abstracted on 180204
	return(already_has_extension(full_file_name,"wav"));
};

Sprite *sprite_from_file_name1(string full_file_name, boolean &aborted, string original_file_name, 
										 string *local_file_name_if_different) {
	// original_file_name new on 180204 -- used to give names (e.g. to sounds)
	// aborted added on 200105
	if (has_picture_extension(full_file_name)) { // was just BMP prior to 300500
		boolean ok;
      Picture *picture = new UserPicture(copy_string(full_file_name),ok,FALSE); // don't warn if missing or share image
		if (ok) {
			if (original_file_name != NULL) { // new on 180204
				string name = short_file_name(original_file_name,FALSE);
				picture->just_set_original_file_name(name); 
			};
			return(picture);
		} else { // new on 210201 so don't have "junk" left on File to Picture sensor or the like
			picture->destroy();
			return(NULL);
		};
	} else if (has_sound_extension(full_file_name)) {
		WaveSound *sound = new WaveSound(0,0,copy_wide_string(full_file_name)); // new on 161103
		if (original_file_name != NULL) { // new on 180204
			string name = short_file_name(original_file_name,FALSE);
			sound->Text::set_text(name); // pretty sure I don't need to delete [] name now
		};
		// prior to 020504 the following was before resetting text above 
		// so the size was for the internal hash name (36 characters long)
		sound->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL);
		return(sound);
//		return(new WaveSound(0,0,copy_wide_string(full_file_name)));
	} else if (already_has_extension(full_file_name,"dmo")) { // new on 210105 to enable dragging of demos into ToonTalk
		if (set_replay_file_name(full_file_name)) {
			tt_replay_after_titles = REPLAY_REPRODUCE_TIMING;
			if (tt_city_document_to_load_after_demo_ends != NULL) {
				tt_city_document_to_load_after_demo_ends->Release();
			};
			close_input_log(TRUE,FALSE); // new on 111205
			tt_time_travel_enabled = FALSE; // new on 111205 so at least demos load correctly but maybe need to do more?
			if (!time_travel_enabled() && tt_city_document_to_load_after_demo_ends == NULL) { 
				// at least for now -- pretty hard to get this to work with time travel
				// second conjunct added on 111205 in case multiple demos are run
				tt_city_document_to_load_after_demo_ends = tt_city->xml_new_document(); // new on 240105
			};
			return(tt_global_picture);
		};
		return(NULL);
	} else { // either uuencoding of ToonTalk object or a text string
		// what if a "new" kind of picture that GDI+ groks?
		Sprite *result = NULL;
//		if (already_has_extension(full_file_name,"tt")) { 
		// new on 040301 -- commented out on 210105 since xml_load_sprite can handle CTY files
			// rewritten on 171204 to accept ULRs as well
			string local_file_name = NULL;
			//boolean local_file_name_shared; // to know whether to delete it or not
			if (is_url(full_file_name)) {
				FileNameStatus file_name_status;
				local_file_name = url_file_name(full_file_name,TRUE,file_name_status);
				if (local_file_name_if_different != NULL) {
					*local_file_name_if_different = local_file_name;
#if TT_DEBUG_ON
				} else {
					debug_this();
#endif
				};
//				local_file_name_shared = FALSE;
			} else {
				local_file_name = full_file_name;
			//	local_file_name_shared = TRUE;
			};
			if (local_file_name != NULL) {
				compute_local_ini_file_name(local_file_name);
				UnwindProtect<boolean> save(tt_loading_is_ok_to_abort); // new on 200105
				if (!replaying() && !tt_dont_set_tt_loading_is_ok_to_abort) { 
					// removed !time_travel_enabled() on 200105 since OK to abort an object load while time traveling (I think)
					tt_loading_is_ok_to_abort = TRUE;
				};
				result = xml_load_sprite_from_file(local_file_name,aborted,NULL); // new on 180303
				//if (!local_file_name_shared) {
				//	delete [] local_file_name;
				//};
				if (result != NULL || aborted) {
					return(result);
				};
			};
		// commented out on 210105 since xml_load_sprite already deals with this
		//} else if (already_has_extension(full_file_name,"cty")) { // new on 281204
		//	tt_city->load_city(full_file_name,TRUE); // added TRUE on 200105
		//	return(NULL);
		//};
		long string_length;
		string file_contents = copy_file_or_url_to_string(full_file_name,string_length);
		if (string_length > 0 && !(string_length == 1 && file_contents[0] == ' ')) { 
			// added 250402 to ignore one byte files containing white space - used to launch TT in various modes
			result = uudecode_sprite(file_contents,TRUE,TRUE,FALSE); 
			// last arg new on 280404 so that if, for example, a puzzle file is on the clipboard
			// its contents aren't loaded
		};
		delete [] file_contents;
		return(result);
	};
};

Sprite *sprite_from_file_name(string full_file_name, boolean &aborted, string original_file_name, 
										string *local_file_name_if_different) {
#if TT_LOADING_AND_SAVING_INTERRUPTABLE
	// new on 140105
	try {
		if (!tt_loading_a_city) { // condition new on 130405 since if loading a city don't reset this counter
			tt_sprites_loaded_count = 0;
		};
		loading_wait(TRUE); // new on 190105
		return(sprite_from_file_name1(full_file_name,aborted,original_file_name,local_file_name_if_different));
	} catch (int ) {
		// for now all signals are abort
		return(NULL);
	};
#else
	return(sprite_from_file_name1(full_file_name,aborted,original_file_name,local_file_name_if_different));
#endif
};

Sprite *sprites_from_drop_handle(HDROP drop_handle, boolean more_than_one_ok) {
	UINT file_count = DragQueryFile(drop_handle,0xFFFFFFFF,NULL,0);
	Cubby *box;
	if (more_than_one_ok) {
		box = new Cubby();
		box->set_number_of_holes(file_count);
		box->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL); // new on 200903
	};
	boolean aborted = FALSE; // new on 200105
	while (file_count > 0) {
		char file_name[MAX_PATH];
		DragQueryFile(drop_handle,file_count-1,file_name,MAX_PATH);
		Sprite *sprite = sprite_from_file_name(file_name,aborted);
		if (!more_than_one_ok) return(sprite);
		if (sprite != NULL) { // conditional added 091000
			box->initialize_hole(file_count-1,sprite);
		};
		wide_string wide_file_name = copy_wide_string(file_name);
		wide_string wide_short_file_name = short_file_name(wide_file_name);
		box->set_label(file_count-1,copy_narrow_string(wide_short_file_name));
		delete [] wide_file_name;
		delete [] wide_short_file_name;
		file_count--;
	};
	box->update_parameter(); // new on 230800 so displays right label stuff
	return(box);
};

Sprite *sprites_from_drop_globals() { // new on 140204 -- much like the above but using globals that may come from playback of demos
	// this assumes more_than_one_ok
	if (tt_drop_files_count == 0) return(NULL); // warn instead??
	Cubby *box;
#if TT_ENHANCED_DRAG_AND_DROP
	if (tt_drop_files_count == 1) {
		box = NULL; // new on 301204 so don't get back if dragging just one file in
	} else 
#endif
		{
		box = new Cubby();
		box->set_number_of_holes(tt_drop_files_count);
		box->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL); // new on 200903
	};
	for (int i = 0; i < tt_drop_files_count; i++) {
		string destination_directory; // new on 180204 to use tt_extracted_file_temp_directory or tt_private_media_directory
		if (//has_picture_extension(tt_drop_file_hash_names[i]) || 
			 has_sound_extension(tt_drop_file_hash_names[i])) {
			destination_directory = tt_private_media_directory;
		} else {
			destination_directory = tt_extracted_file_temp_directory;
		};
		string real_file_name = extract_file_from_archive(tt_drop_file_hash_names[i],tt_log_in_archive,destination_directory,TRUE);
		if (real_file_name == NULL) { // new on 240204 for robustness
			break; // warn??
		};
		boolean aborted = FALSE; // new on 200105
		Sprite *sprite = sprite_from_file_name(real_file_name,aborted,tt_drop_file_names[i]); // second arg new on 180204
		delete [] real_file_name;
		if (sprite == tt_global_picture) { // new on 210105 -- loading a city (or maybe a demo)
			return(NULL);
		};
		if (sprite != NULL && box != NULL) { // conditional added 091000 -- second condition new on 301204
			box->initialize_hole(i,sprite);
		};
		wide_string wide_file_name = copy_wide_string(tt_drop_file_names[i]);
		wide_string wide_short_file_name = short_file_name(wide_file_name);
		if (box != NULL) {
			box->set_label(i,copy_narrow_string(wide_short_file_name));
		};
		delete [] wide_file_name;
		delete [] wide_short_file_name;
		if (box == NULL) { // new on 301204 -- just one thing so don't put it in a box
			return(sprite);
		};
	};
	if (box != NULL) {
		box->update_parameter(); // new on 230800 so displays right label stuff
	};
	return(box);
};

void run_queue() {
//	if (tt_queue == NULL) return; // commented out on 210104 since all the callers check this first
	Queue *remaining_queue;
//		  tt_error_file() << tt_queue->length() << " items in queue frame = " << tt_frame_number << endl;
	Queue *to_run = tt_queue;
	tt_queue = NULL;
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	to_run->begin_run(remaining_queue);
	if (remaining_queue != NULL) {
		if (tt_queue == NULL) {
			tt_queue = remaining_queue;
		} else {
			combine_queues(tt_queue,remaining_queue);
//				  tt_queue->insert_at_end(remaining_queue);
		};
	};
//		  if (tt_queue == NULL) {
//			  tt_error_file() << "Queue now empty." << endl;
//		  } else {
//			  tt_error_file() << tt_queue->length() << " items remaining in queue." << endl;
//		  };
};

void save_clipping_to_file(HGLOBAL clipping, string suffix) {
	if (replaying()) return; // added 241199 since running demos shouldn't add stuff to clipping directory
	if (tt_clipping_directory == NULL) return;
	char file_name[MAX_PATH];
	output_string_stream stream(file_name,MAX_PATH);
	stream << tt_clipping_directory << tt_file_name << suffix << ".tt"; 
	// changed on 250300 since TT now is associated with ToonTalk - was AC(IDC_DOT_TXT);
	stream.put('\0');
	save_clipping_to_file_name(clipping,file_name);
};

void save_clipping_to_file_name(HGLOBAL clipping, string file_name) {
	// restructured on 240902
	string text = (string) GlobalLock(clipping);
	if (text == NULL) {
		log("Clipboard empty for saving a ToonTalk object to file. Something wrong with ToonTalk, sorry.",FALSE,TRUE);
		return;
	};
	if (tt_zip_files) { // new on 180303 to package everything up and zip the XML
		zipFile archive = open_archive_to_zip(file_name);
		if (archive != NULL) {
			open_zip_file(archive,main_xml_file_name); // good enough name for now
			write_zip_file(archive,(bytes) text,strlen(text));
			close_zip_file(archive);
			save_media_files_in_archive(archive);			
		} else {
			log("Something went wrong trying to create a zip archive for a ToonTalk object. Sorry.",FALSE,TRUE);
		};
	} else {
		output_file_stream copy(file_name,std::ios_base::out);
		if (copy == NULL) { // new on 290904 for error recovery
			unable_to_write_file(file_name); 
			return;
		};
		copy.write(text,strlen(text));
		copy.close();
	};
	GlobalUnlock(clipping);
};

string media_file_names(Sprites *sprites_with_media_to_save, string old_relative_media_file_names) {
	// support for old_relative_media_file_names added 210903
	if (sprites_with_media_to_save == NULL && old_relative_media_file_names == NULL) return(NULL);
	Sprites *remaining = sprites_with_media_to_save;
	const int relative_media_file_name_length = media_directory_length+39; 
	// 32 for hash name, 4 for extension including ., 1 for space and a little slop just in case
	int number_of_sprites_with_media_to_save;
	if (sprites_with_media_to_save == NULL) {
		number_of_sprites_with_media_to_save = 0;
	} else {
		number_of_sprites_with_media_to_save = sprites_with_media_to_save->length();		
	};
	int relative_media_file_index;
	if (old_relative_media_file_names == NULL) {
		relative_media_file_index = 0;
	} else {
		relative_media_file_index = strlen(old_relative_media_file_names);
	};
	string relative_media_file_names = new char[relative_media_file_name_length*number_of_sprites_with_media_to_save+relative_media_file_index+1]; 
	// was MAX_PATH*tt_sprites_with_media_to_save->length() prior to 200903
	if (relative_media_file_index > 0) {
		memcpy(relative_media_file_names,old_relative_media_file_names,relative_media_file_index+1); // +1 added 210903 to include the terminating NULL
	};
	boolean ignore_worth_compressing;
	while (remaining != NULL) {
		string short_media_file_name = remaining->first()->private_media_file_name(ignore_worth_compressing,FALSE); 
		// changed on 210703 to use short path here
		if (short_media_file_name != NULL) {
			if (relative_media_file_index == 0 || strstr(relative_media_file_names,short_media_file_name) == NULL) { 
				// new on 200903 to check for duplicates here
//				string short_media_file_name = media_file_name+tt_private_media_directory_length;
//				char relative_media_file_name[MAX_PATH];
				memcpy(relative_media_file_names+relative_media_file_index,media_directory,media_directory_length);
				relative_media_file_index += media_directory_length;
				int short_media_file_name_length = strlen(short_media_file_name);
				memcpy(relative_media_file_names+relative_media_file_index,short_media_file_name,short_media_file_name_length);
				relative_media_file_index += short_media_file_name_length;
				relative_media_file_names[relative_media_file_index++] = ' '; // spaces separate the names
				relative_media_file_names[relative_media_file_index] = '\0'; // so strstr above can eliminate duplicates
			};
//			copy_file_or_url_to_zip_file(media_file_name,short_media_file_name,archive,worth_compressing?default_compression_level:0);
			delete [] short_media_file_name;
		};
		remaining = remaining->rest();
	};
//	relative_media_file_names[relative_media_file_index] = '\0'; // done above now (200903)
	return(relative_media_file_names);
};

void save_media_files_in_archive(zipFile archive, string extra_media_file_names, boolean for_a_document) {
	// added for_a_document (e.g. a notebook, as opposed to for time travel) -- 280204
	// since setting tt_zip_description_archive_element to NULL caused other problems
	if (tt_extra_media_file_names != NULL) { // new on 081203
		add_new_media_file_names(tt_extra_media_file_names,extra_media_file_names);
		delete [] tt_extra_media_file_names;
		tt_extra_media_file_names = NULL;
	};
	if (tt_sprites_with_media_to_save == NULL && extra_media_file_names == NULL) return;
	if (for_a_document && archive != NULL) {
		// rewritten on 210803 to give all the file names to DynaZip at once
		// for_a_document was tt_zip_description_archive_element == NULL prior to 280204
		string relative_media_file_names = media_file_names(tt_sprites_with_media_to_save,extra_media_file_names);
		if (relative_media_file_names != NULL) {
			boolean worth_compressing = (strstr(relative_media_file_names,"wav") != NULL); 
			// the PNGs are already compressed - rewritten 200903 -
			// compress them if there is at least one wav file among them -- 
			// could alternatively separate the files names into pictures and sounds
			// previously worth_compressing was always FALSE since used to do this on a file-by-file basis
			char media_path_name[MAX_PATH]; // with *.* at the end so included in archive with only part of path
			memcpy(media_path_name,tt_private_media_directory,tt_private_media_directory_length-media_directory_length);
			strcpy(media_path_name+tt_private_media_directory_length-media_directory_length,"*.*");
			zip_file(archive,media_path_name,relative_media_file_names,worth_compressing?default_compression_level:0,FALSE);
			delete [] relative_media_file_names;
		};
	} else { // new on 200703
		xml_element *element = first_element_with_tag_name(tt_zip_description_archive_element,L"Media");
		wide_character hash_string[hash_byte_length*2+1]; // 1 extra for terminator - double since 4-bit encodings of 8-bit items
		if (element != NULL) {
//			xml_document *document = xml_get_document(tt_zip_description_archive_element);
			Sprites *remaining = tt_sprites_with_media_to_save;
			while (remaining != NULL) {
				Sprite *sprite = remaining->first();
				hash media_hash = sprite->return_hash();
				// rather than trying to use worth_compressing could set up the archive 
				// to not compress certain file extensions such as JPG
				if (media_hash != NULL) {
					hash_to_wide_string(media_hash,hash_string);
					// add if not already there...
					xml_set_attribute(element,hash_string,sprite->private_media_file_extension());
				};
				remaining = remaining->rest();
			};
		};
	};
	just_destroy_list(tt_sprites_with_media_to_save); // could do some of this work above...
};

//void include_media_file_of_attribute(BSTR attribute, void *extra) {
//	// a typical attribute will look like dgnjdcfkeginjpndmcbjfibfebgfnfmd="png"
//	string archive = (string) extra;
//	char media_file_name[MAX_PATH];
//	char full_short_media_file_name[MAX_PATH]; // won't need this much but how much less?
//	strcpy(full_short_media_file_name,media_directory);
//	string short_media_file_name = full_short_media_file_name+media_directory_length; 
//	int length = ::SysStringLen(attribute);
//	for (int i = length-1; i >= 0; i--) {
//		if (attribute[i] == '=') {
//			copy_narrow_string(attribute,i,short_media_file_name,i);
//			short_media_file_name[i++] = '.';
//			// following needs to skip over the quote mark
//			copy_narrow_string(attribute+i+1,i+2,short_media_file_name+i,3); 
//			// not so good that this assumes all extensions are 3 letters - OK for now
//			short_media_file_name[i+4] = '\0'; // terminate it
//			//memcpy(media_file_name,tt_private_media_directory,tt_private_media_directory_length);
//			//strcpy(media_file_name+tt_private_media_directory_length,short_media_file_name);
//			// as of 210703 don't pass the full path if there is a destination but the place to search so get partial path in archive
//			memcpy(media_file_name,tt_private_media_directory,tt_private_media_directory_length-media_directory_length);
//			strcpy(media_file_name+tt_private_media_directory_length-media_directory_length,"*.*");
//			zip_file(archive,media_file_name,full_short_media_file_name);
//			return;
//		};
//	};
//};

void include_media_file_of_attribute(BSTR attribute, void *extra) {
	// a typical attribute will look like dgnjdcfkeginjpndmcbjfibfebgfnfmd="png"
	string file_names = (string) extra;
	int file_names_offset = strlen(file_names);
	int length = ::SysStringLen(attribute);
	for (int i = length-1; i >= 0; i--) {
		if (attribute[i] == '=') {
			memcpy(file_names+file_names_offset,media_directory,media_directory_length);
			file_names_offset += media_directory_length;
			copy_narrow_string(attribute,i,file_names+file_names_offset,i);
			file_names_offset += i;
			file_names[file_names_offset++] = '.';
			// following needs to skip over the quote mark
			copy_narrow_string(attribute+i+2,i+2,file_names+file_names_offset,3); 
// not so good that this assumes all extensions are 3 letters - OK for now
			file_names_offset += 3;
			// add a space to separate files and terminate it
			file_names[file_names_offset++] = ' ';
			file_names[file_names_offset] = '\0'; 
			return;
		};
	};
};

void include_media_files_in_zip_description_archive_element(string archive) {
	xml_element *element = first_element_with_tag_name(tt_zip_description_archive_element,L"Media");
	if (element != NULL) {
		// updated on 220803 so the following only generates the list of files to archive
//		xml_process_attributes(element,include_media_file_of_attribute,archive);
		xml_node_map *map = xml_get_xml_node_map(element);
		if (map == NULL) return;
		int count = xml_attribute_length(map);
		if (count == 0) {
			return;
		};
		string file_names = new char[count*MAX_PATH];
		file_names[0] = '\0'; // will grow after finding the current length - not the most efficient but good enough
		xml_process_attributes(map,include_media_file_of_attribute,file_names);
		char media_file_name[MAX_PATH];
		memcpy(media_file_name,tt_private_media_directory,tt_private_media_directory_length-media_directory_length);
		strcpy(media_file_name+tt_private_media_directory_length-media_directory_length,"*.*");
		zip_file(archive,media_file_name,file_names,FALSE);
		delete [] file_names;
	};
};

#if TT_DYNA_ZIP
boolean update_private_media_directory(unzFile archive) {
	return(extract_archive_to_directory(archive,"Media\\*.*",tt_private_media_directory
												  ,FALSE 
   // new on 281003 -- don't overwrite old files since no need - was old policy before 281003 as well
			));
};
#else
boolean update_private_media_file(string file_name, unzFile archive, unz_file_info &file_info) {
	if (strncmp(file_name,media_directory,media_directory_length) == 0) { // move to constant.h
		char full_file_name[MAX_PATH];
		memcpy(full_file_name,tt_private_media_directory,tt_private_media_directory_length);
		strcpy(full_file_name+tt_private_media_directory_length,file_name+media_directory_length);
		if (!file_exists(full_file_name,FALSE)) {
			return(extract_current_file_in_archive(full_file_name,archive,file_info));
		};
	};
	return(TRUE); // all went well since nothing to do
};

boolean update_private_media_directory(unzFile archive) {
	int result = go_to_first_file_in_unzip_archive(archive);
	char file_name[MAX_PATH];
	unz_file_info file_info;
	result = get_info_from_current_file_in_archive(archive,file_name,sizeof(file_name),&file_info);
	update_private_media_file(file_name,archive,file_info);
	while (TRUE) {
		result = go_to_next_file_in_unzip_archive(archive);
		if (result == UNZ_END_OF_LIST_OF_FILE) break;
		result = get_info_from_current_file_in_archive(archive,file_name,sizeof(file_name),&file_info);
		update_private_media_file(file_name,archive,file_info);
	};
	return(TRUE); // all OK
};
#endif

#if TT_UNICODE
string a2u(const_ascii_string source) {
	// memory leak when Unicode is on... maybe it could use buffers used by S?
	if (source == NULL) return(NULL);
	long destination_length = MultiByteToWideChar(tt_code_page[tt_use_spoken_alternative],0,source,-1,NULL,0);
   wide_string destination = new wide_character[destination_length+1];
   MultiByteToWideChar(tt_code_page[tt_use_spoken_alternative],0,source,-1,destination,destination_length);
	destination[destination_length] = NULL;
	return(destination);
};

ascii_string u2a(const_string source) {
	if (source == NULL) return(NULL);
	long destination_length = WideCharToMultiByte(tt_code_page[tt_use_spoken_alternative],0,-1,NULL,0,NULL,NULL);
   string destination = new char[destination_length+1];
	WideCharToMultiByte(tt_code_page[tt_use_spoken_alternative],0,source,-1,destination,destination_length,NULL,NULL);
   destination[destination_length] = NULL;
	return(destination);
};

ascii_string copy_unicode_to_ascii(const_string source, int source_length) {
	// besides length arg this one is guaranteed to make a new string
	// u2a could be changed to use buffers and return const_string
	long destination_length = WideCharToMultiByte(tt_code_page[tt_use_spoken_alternative],0,source_length,NULL,0,NULL,NULL);
   string destination = new char[destination_length+1];
	WideCharToMultiByte(tt_code_page[tt_use_spoken_alternative],0,source,source_length,destination,destination_length,NULL,NULL);
   destination[destination_length] = NULL;
	return(destination);
};
string copy_ascii_to_unicode(const_ascii_string source, int source_length) {
	// this is guaranteed to make a copy - unlike a2u
	if (source == NULL) return(NULL);
	long destination_length = MultiByteToWideChar(tt_code_page[tt_use_spoken_alternative],0,source,source_length,NULL,0);
   wide_string destination = new wide_character[destination_length+1];
   MultiByteToWideChar(tt_code_page[tt_use_spoken_alternative],0,source,source_length,destination,destination_length);
	destination[destination_length] = NULL;
	return(destination);
};
void copy_ascii_to_unicode(const_ascii_string s, string result, int string_size) {
	// this is guaranteed to make a copy - unlike a2u
	if (s == NULL) return;
	int length = strlen(s);
   MultiByteToWideChar(tt_code_page[tt_use_spoken_alternative],0,s,length+1,result,string_size);
};
#endif

#if TT_32
ascii_string last_error() {
	ascii_string error_message_buffer = NULL;
	DWORD error_code = GetLastError();
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
					  NULL,error_code,
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					  error_message_buffer,0,NULL);
	if (error_message_buffer == NULL) return("Unknown Error");
	if (error_message_buffer[0] == 0) {
		itoa(error_code,error_message_buffer,10);
	};
   return(error_message_buffer);
};

void message_box_last_error() {
	message_box(MB_ICONSTOP|MB_OK|MB_SYSTEMMODAL,last_error());
};
#endif

city_coordinate default_talk_balloon_character_width() {
	city_coordinate default_width = (3*tile_width)/5; // 30 characters accross screen 
	if (tt_good_sizes_adjust_to_screen_size) { 
		default_width = shrink_width_from_640x480_screen_size(default_width);
	};
	return(default_width);
};

city_coordinate default_talk_balloon_character_height() {
	city_coordinate default_height = 2*tile_height; // (8*tile_height)/3; // 11 lines -- was 2x prior to 050102 - restored on 110102
	if (tt_good_sizes_adjust_to_screen_size) {
		default_height = shrink_height_from_640x480_screen_size(default_height);
	};
	return(default_height);
};

void maintain_n_versions_youngest_first(ascii_string file_name, int max, boolean should_contain_dash) {
	// takes a file name like ken.cty (maybe with full path)
	// and renames it to ken1.cty
	// and if ken-1.cty already existed renames it to ken-2.cty
	// and so at max a "max" number of times
	// Should this be careful if original file_name is very close to MAX_PATH in length? - asked 280103
	if (max <= 0) return;
	if (!file_exists(file_name,FALSE)) return; // doesn't exist so all is fine
	boolean name_is_new;
	char file_name_copy[MAX_PATH]; // rewritten on 130703 to not use the heap
	strcpy(file_name_copy,file_name);
//	string file_name_copy = copy_string(file_name);
	string new_file_name = next_file_name(file_name_copy,name_is_new,should_contain_dash);
	maintain_n_versions_youngest_first(new_file_name,max-1,should_contain_dash); // recur now so youngest moved first
//	tt_error_file() << file_name << " renamed to " << new_file_name << endl;
//	DeleteFile(new_file_name); // commented out on 280103 since now using MOVEFILE_REPLACE_EXISTING
	if (!move_file(file_name,new_file_name)) { 
		return; // warn?
	};
//	delete [] file_name_copy;
	if (name_is_new) {
		delete [] new_file_name;
	};
};

string file_name_renumbered(string file_name, long number, string file_name_to_use) {
	int file_name_length = strlen(file_name);
	int i;
	for (i = file_name_length-1;; i--) { // search from end backwards
		if (i == 0 || file_name[i] == '\\' || file_name[i] == '/' || file_name[i] == ':') { // new on 080401 - dots don't count left of these characters
			// didn't find a dot
			i = file_name_length-1;
			break;
		};
		if (file_name[i] == '.') break;
	};
	for (int j = i-1; j >= 0; j--) { // new on 280803
		if (file_name[j] > '0' && file_name[j] <= '9') { // need to zero all the non-zero digits
			file_name[j] = '0'; // so for example log00012 becomes log00000 before being clobbered
		} else if (file_name[j] == '\\' || file_name[j] == '\\' ||file_name[j] == ':') break;  // starting with the path	
	};
	string new_file_name;
	if (file_name_to_use == NULL) {
		new_file_name = new char[file_name_length+1];
	} else { // new on 150204
		new_file_name = file_name_to_use;
	};
//	string number_string = new char[12];
	char number_string[12]; // re-written on 251000 to prevent a minor leak
	ltoa(number,number_string,10);
	int number_string_length = strlen(number_string);
	int number_start = i-number_string_length;
	memcpy(new_file_name,file_name,number_start);
	memcpy(new_file_name+number_start,number_string,number_string_length);
	memcpy(new_file_name+number_start+number_string_length,file_name+i,(file_name_length-i)+1);
	return(new_file_name);
};

zipFile log_out_archive_without_time_travel(string file_name) { // new on 110104
	tt_log_out_archive = file_name; // open_archive_to_zip(file_name);
//	tt_log_out_archive_size = max(1,count_all_zip_members(tt_log_out_archive)); // if new then include counters -- obsolete 160204
	write_ini_entry("User","OpenDemoFileName",tt_log_out_archive); // new on 140904 for recovery of DMOs from crashes
	return(tt_log_out_archive);
};

zipFile log_out_archive() { // to be called when unsure if archive has been opened yet
	if (tt_log_out_archive == NULL) {
		if (!time_travel_enabled()) { // new on 110104
			return(NULL);
		};
		char zip_archive_file_name[MAX_PATH];
		strcpy(zip_archive_file_name,tt_user_directory);
//		if (time_travel_enabled()) {
			strcat(zip_archive_file_name,"time_travel.dmo");
		//} else { // commented out on 110104
		//	strcat(zip_archive_file_name,"replay_logs.zip");
//		};
		tt_log_out_archive = open_archive_to_zip(zip_archive_file_name);
//		tt_log_out_archive_size = max(1,count_all_zip_members(tt_log_out_archive)); 
		// if new then include counters -- obsolete 160204
		write_ini_entry("User","OpenDemoFileName",tt_log_out_archive); // new on 140904 for recovery of DMOs from crashes
	};
	return(tt_log_out_archive);
};

//boolean log_in_archive_doesnt_exist = FALSE; 
// until proven otherwise -- as of 180703 just those callers that care check if it exists

// editted on 131003 to have an argument - time_travel_archive_regardless
unzFile log_in_archive(boolean time_travel_archive_regardless) { // to be called when unsure if archive has been opened yet
	if (tt_log_in_archive == NULL || time_travel_archive_regardless) { // && !log_in_archive_doesnt_exist) {
		char zip_archive_file_name[MAX_PATH];
		strcpy(zip_archive_file_name,tt_user_directory);
		if (time_travel_archive_regardless || time_travel_enabled()) {
			strcat(zip_archive_file_name,"time_travel.dmo");
			if (time_travel_archive_regardless) { // branch new on 201003
				return(open_archive_to_unzip(zip_archive_file_name));
			} else 
				tt_log_in_archive = open_archive_to_unzip(zip_archive_file_name);
//		} else {
			//strcat(zip_archive_file_name,"replay_logs.zip"); // commented out on 110104
			//tt_log_in_archive = open_archive_to_unzip(zip_archive_file_name); // commented out on 110104
			// not needed since there is time_travel_archive_regardless now (131003)
			//if (tt_log_in_archive == NULL) { // new on 150903 to try again to see if there is a time_travel archive
			//	strcpy(zip_archive_file_name,tt_user_directory);
			//	strcat(zip_archive_file_name,"time_travel.dmo");
			//	tt_log_in_archive = open_archive_to_unzip(zip_archive_file_name);
			//};
		};
		//if (file_exists(zip_archive_file_name)) {
		//	tt_log_in_archive = open_archive_to_unzip(zip_archive_file_name);
		//} else {
		//	log_in_archive_doesnt_exist = TRUE;
		//};
	};
	return(tt_log_in_archive);
};

flag using_archive = TRUE; // until proven otherwise - new on 250603

string update_versions(int max, string file_name) { // int &current_version, int oldest_version, int youngest_version) {
	// this doesn't really name file_name and "log00000.dmo" could be a constant - 250603
	if (tt_youngest_log_segment < tt_current_log_segment) {
		tt_youngest_log_segment = tt_current_log_segment;
	};
	if (tt_oldest_log_segment < 1) tt_oldest_log_segment = 1; 
	if (tt_current_log_segment < 1) tt_current_log_segment = 1;
	if (max > 0 && tt_log_out_archive != NULL) { // condition new on 280803 -- second condition added 290903
		while (tt_current_log_segment-tt_oldest_log_segment >= max) { // delete oldest one
			string oldest_file_name = file_name_renumbered(file_name,tt_oldest_log_segment);
//			if (using_archive) {
			if (!tt_keep_all_time_travel_segments) // new on 061003 - still pretend that it is gone
				delete_zip_file(tt_log_out_archive,oldest_file_name); // this was tt_log_in_archive prior to 280903
//			tt_log_out_archive_size--; // problem is that maybe things got renumbered
			//} else {
			//	  DeleteFile(oldest_file_name);
			//};
			delete [] oldest_file_name;
			tt_oldest_log_segment++;
		};
	};
	save_oldest_and_current_version_numbers(tt_current_log_segment,tt_oldest_log_segment,tt_youngest_log_segment);
	if (tt_log_out_file_name_specified != NULL) {
		// new on 110704 to deal with -o name
		return(file_name);		
	} else {
		string result = file_name_renumbered(file_name,tt_current_log_segment); // sometimes redundant work
		delete [] file_name;
		return(result);
	};
};

string maintain_n_versions_youngest_last_pre_archive_version(int max, int &current_version) {
	// else handle old pre-archive version
	string file_name = append_strings(tt_user_directory,"log00000.dmo");
	string count_file_name = append_strings(file_name,".n");
	HANDLE count_file = CreateFile(count_file_name,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	long oldest_version, youngest_version;
	DWORD bytes_read;
	if (count_file == INVALID_HANDLE_VALUE) { // first time updated on 151299 was == NULL earlier
		current_version = 1;
		oldest_version = 1;
		youngest_version = 1;
	} else if (ReadFile(count_file,(char *) &current_version,sizeof(current_version),&bytes_read,NULL)) {
		current_version++;
		ReadFile(count_file,(char *) &oldest_version,sizeof(oldest_version),&bytes_read,NULL);
		if (!ReadFile(count_file,(char *) &youngest_version,sizeof(youngest_version),&bytes_read,NULL)) {
			youngest_version = current_version;
		}; 
		CloseHandle(count_file);
	} else {
		current_version = 1;
		oldest_version = 1;
		youngest_version = 1;
		CloseHandle(count_file);
	};
	delete [] count_file_name; // added 251000
	// reverted back on 010703
	//string short_name = short_file_name(file_name,TRUE); // beginning 250603 don't want the full path
	//delete [] file_name;
	return(update_versions(max,file_name)); //current_version,oldest_version,youngest_version));
};

void initialize_zip_description_archive_element() {
	xml_document *document = create_xml_document();
	tt_zip_description_archive_element = create_xml_element(L"TimeTravelData",document);
	xml_set_attribute(tt_zip_description_archive_element,L"xmlns",L"http://www.toontalk.com");	
	xml_set_attribute(tt_zip_description_archive_element,L"ChangeThisToZeroIfYouEditThis",1); // to deal with user edits
	xml_set_attribute(tt_zip_description_archive_element,L"VersionNumber",1);
	xml_set_attribute(tt_zip_description_archive_element,L"SecondsBetweenLogs",tt_time_between_new_logs/1000);
	xml_set_attribute(tt_zip_description_archive_element,L"MaximumLogs",tt_log_count_max); // new on 280803
	xml_append_child(create_xml_element(L"Media",document),tt_zip_description_archive_element); // new on 200703
	xml_set_attribute(tt_zip_description_archive_element,L"InitialTime",tt_current_time); // new on 120204
};

void release_zip_description_archive_element() {
	if (tt_zip_description_archive_element == NULL) return;
	xml_document *document = xml_get_document(tt_zip_description_archive_element);
	tt_zip_description_archive_element->Release(); // new on 200804
	document->Release();
	tt_zip_description_archive_element = NULL;
};

string contents_of_counters_file() { // abstracted and given a different behavior on 110104 when logging but no time travel
	if (log_in_archive() != NULL) {
		return(narrow_contents_of_current_file_in_archive(tt_log_in_archive,"0.xml"));
	} else if (time_travel_enabled()) { // new on 120104
		return(NULL);
	} else {
		string archive_name = replay_file_name(); // new on 210105 to check if demo has current.xml in it
		if (archive_name != NULL) {
			string from_demo_archive = narrow_contents_of_current_file_in_archive(archive_name,"counters.xml");
			if (from_demo_archive != NULL) {
				return(from_demo_archive);
			};
		};
		long string_length;
		char file_name[MAX_PATH];
		strcpy(file_name,tt_user_directory);
		strcat(file_name,"counters.xml");
		return(copy_file_or_url_to_string(file_name,string_length));
	};
};

boolean read_counters_from_archive() { // int &current_version, int &oldest_version, int &youngest_version) {
	// returns FALSE if it encouters problems - new on 190703
	if (tt_youngest_log_segment >= 0) { 
		// changed for > 0 to >= 0 since can be 0 before being incremented 
		// but the archive isn't yet up to date (010304)
		return(TRUE); // new on 180703 to not bother reading if already know
	};
#if TT_DYNA_ZIP
	// new on 300603
//	if (log_in_archive() != NULL) {
		// rewritten on 190703
		// should always be index == 0 so don't bother to use the name
//		open_named_file_in_unzip_archive(tt_log_in_archive,"0.xml"); // should appear first in an alphabetic listing
//		string time_travel_data_string = narrow_contents_of_current_file_in_archive(tt_log_in_archive,"0.xml"); 
	// updated to be more robust on 031003
	string time_travel_data_string = contents_of_counters_file();
	if (time_travel_data_string == NULL) {
			//tt_current_log_segment = 0;
			//tt_oldest_log_segment = 0;
			//tt_youngest_log_segment = 0;
#if TT_DEBUG_ON
		if (tt_debug_mode == 150404) {
			log("time_travel_data_string is NULL");
		};
#endif
		return(FALSE); // no need to warn since this normally happens with brand new archives
	};
	xml_document *document = document_from_string(time_travel_data_string);
	if (document == NULL) { // new on 150404 for robustness and error reporting
		tt_error_file() << "Could not create a document from the following XML:" << endl;
		tt_error_file() << time_travel_data_string << endl;
		return(FALSE);
	};
	xml_element *element = xml_get_document_element(document);
	if (element != NULL) {
		release_zip_description_archive_element();
		tt_zip_description_archive_element = element;
		tt_prevous_log_segment = tt_current_log_segment; // new on 041103
		// rewritten on 071004 otherwise can have "reminants" of previous resolution -- e.g. wrong wall size
		tt_video_mode_if_running_demo = xml_get_attribute_int(element,L"ScreenWidth",tt_video_mode); // new on 260304
		tt_current_log_segment = xml_get_attribute_int(element,L"CurrentSegment",tt_current_log_segment);
		tt_oldest_log_segment = xml_get_attribute_int(element,L"OldestSegment",tt_oldest_log_segment);
		tt_youngest_log_segment = xml_get_attribute_int(element,L"YoungestSegment",tt_youngest_log_segment);
		if (tt_current_log_segment == tt_oldest_log_segment) { // condition new on 200304
			tt_current_time_at_beginning_of_first_segment = xml_get_attribute_int(element,L"InitialTime",tt_current_time); 
			// new on 120204
		};
		if (tt_time_travel_enabled) { // added tt_time_travel_enabled on 021003
//				tt_time_between_new_logs == 0 && // removed on 021203 since for good playback need to have the same value as recording 
				// and confusion results if the same time travel archive has different length segments
			// hasn't been set so use setting in archive - e.g. playback of someone else's archive
			tt_time_between_new_logs = xml_get_attribute_int(element,L"SecondsBetweenLogs",tt_time_between_new_logs)*1000;
#if TT_SPECIAL_DEBUGGING
			tt_special_debugging_file 
				<< "Turning on time travel since set in 0.xml file in time_travel.dmo. tt_time_between_new_logs is " 
				<< tt_time_between_new_logs << endl;
#endif
		};
		tt_log_count_max = xml_get_attribute_int(element,L"MaximumLogs",tt_log_count_max); // new on 280803
		if (tt_time_between_new_logs > 0) {
			tt_dump_as_xml = TRUE; // time travel just won't work otherwise - new on 170903
		};
	} else {
		tt_error_file() << "Had troubles finding the expected XML in the 0.xml file of " << tt_log_in_archive << endl;
		return(FALSE);
	};
	delete [] time_travel_data_string;
		//open_named_file_in_unzip_archive(tt_log_in_archive,"counters");
		//counters c;
		//if (read_current_file_in_unzip_archive(tt_log_in_archive,(char *) &c,sizeof(c)) == sizeof(c)) {
		//	tt_current_log_segment = c.current_version;
		//	tt_oldest_log_segment = c.oldest_version;
		//	tt_youngest_log_segment = c.youngest_version;
		//	return;
		//};
#else
	if (log_in_archive() != NULL && 
		 open_named_file_in_unzip_archive(tt_log_in_archive,"counters") == UNZ_OK) {
		if (read_current_file_in_unzip_archive(tt_log_in_archive,
															(char *) &current_version,sizeof(current_version)) == sizeof(current_version)) {
			// no problems finding and reading beginning of old file	
			read_current_file_in_unzip_archive(tt_log_in_archive,(char *) &oldest_version,sizeof(oldest_version));
			read_current_file_in_unzip_archive(tt_log_in_archive,(char *) &youngest_version,sizeof(youngest_version));
			// more stuff to come for time travel
			close_current_file_in_unzip_archive(tt_log_in_archive);
			close_unzip_archive(tt_log_in_archive);
			tt_log_in_archive = NULL;
			return(TRUE);
		};
	};
#endif
	return(TRUE);
};

boolean write_counters_in_archive(int current_version, int oldest_version, int youngest_version) {
#if TT_DYNA_ZIP
//	if (log_out_archive() != NULL) { // commented out on 110104
		// rewritten 190703
	if (tt_zip_description_archive_element == NULL) {
		initialize_zip_description_archive_element();
	};
	xml_set_attribute(tt_zip_description_archive_element,L"ScreenWidth",tt_video_mode); // new on 260304
	xml_set_attribute(tt_zip_description_archive_element,L"CurrentSegment",tt_current_log_segment);
	xml_set_attribute(tt_zip_description_archive_element,L"OldestSegment",tt_oldest_log_segment);
	xml_set_attribute(tt_zip_description_archive_element,L"YoungestSegment",tt_youngest_log_segment);
	if (time_travel_enabled()) { //was (log_out_archive() != NULL) prior to 110104
		dump_xml_to_zip_file(tt_zip_description_archive_element,"0.xml");
	} else {
		// new on 110104
		xml_document *document = create_xml_document();
		add_processing_instruction(document);
		// could add commentary/explanation to the document
		tt_zip_description_archive_element->AddRef(); // new on 190804 since xml_append_child now releases
		xml_append_child(tt_zip_description_archive_element,document);
		char counters_file[MAX_PATH];
		strcpy(counters_file,tt_user_directory);
		strcat(counters_file,"counters.xml");
		xml_save_document(document,counters_file,NULL,NULL,FALSE); // don't zip it
		xml_release_document(document);
	};
	//open_zip_file(tt_log_out_archive,"counters");
	//counters c(current_version, oldest_version, youngest_version);
	//write_zip_file(tt_log_out_archive,(bytes) &c,sizeof(c),0); // added 0 for no compression on 190703
	return(TRUE);
//	};
#else
	if (log_out_archive() != NULL && open_zip_file(tt_log_out_archive,"counters") == ZIP_OK) {
		write_zip_file(tt_log_out_archive,(bytes) &current_version,sizeof(current_version));
		write_zip_file(tt_log_out_archive,(bytes) &oldest_version,sizeof(oldest_version));
		write_zip_file(tt_log_out_archive,(bytes) &youngest_version,sizeof(youngest_version));
		result = close_zip_file(tt_log_out_archive);
		result = close_zip_archive(tt_log_out_archive); // so can be read without problems
		tt_log_out_archive = NULL;
		return(TRUE);
	};
#endif
	return(FALSE);
};

void oldest_and_yongest_versions() { // int &oldest, int &youngest) {
#if TT_DYNA_ZIP
	// new on 010703
	read_counters_from_archive(); // ignore_current_version,oldest,youngest)) {
#else
	string file_name = append_strings(tt_user_directory,"log00000.dmo.n");
	HANDLE count_file = CreateFile(file_name,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
											 FILE_ATTRIBUTE_NORMAL,NULL);
	DWORD bytes_read;
	if (count_file != INVALID_HANDLE_VALUE) {
		long current_version;
		ReadFile(count_file,(char *) &current_version,sizeof(current_version),&bytes_read,NULL);
		ReadFile(count_file,(char *) &oldest,sizeof(oldest),&bytes_read,NULL);
		if (!ReadFile(count_file,(char *) &youngest,sizeof(youngest),&bytes_read,NULL)) { // new on 170603
			youngest = current_version; // old file format with only 2 values
		} else if (youngest < current_version) {
			youngest = current_version; // current one can't be younger
		};
		CloseHandle(count_file);
		return;
	};
#endif
	//tt_oldest_log_segment = -1; // commented out on 180703
	//tt_youngest_log_segment = -1;
};

string maintain_n_versions_youngest_last(int max) { // , int &current_version) {
	// takes a file name like log00001.dmo (maybe with full path)
	// and returns log<n+1>.dmo where n is the previous youngest file
	// if max files is exceeded then lowest numbered file is deleted
	// relies upon a dmo.n file that contains the current count and the oldest count
	string directory; // new on 200804 since if ToonTalk crashes these DMO files aren't replayable and should be cleaned up
	if (tt_extracted_file_temp_directory != NULL) {
		directory = tt_extracted_file_temp_directory;
	} else { // shouldn't happen
		directory = tt_user_directory;
	};
	if (max == 0) { // was <= 0 prior to 280803
		return(append_strings(directory,"log00000.dmo")); // shouldn't happen
	};
//	int oldest_version, youngest_version; // prior to 250603 used to be long but current_version is int
	read_counters_from_archive(); // current_version,oldest_version,youngest_version)) {
	// no longer condtional	
	tt_current_log_segment++;
	if (tt_log_started_frame+1 != tt_frame_number) {
		remove_rerecorded_segment(tt_current_log_segment); 
		// new on 240904 in case have now moved further back in time and recording over old rerecordings
	};
	string short_file_name;
	if (tt_log_out_file_name_specified != NULL) {
		// new on 110704 to deal with -o name
		short_file_name = copy_string("log00001.dmo");		
	} else {
		short_file_name = file_name_renumbered("log00000.dmo",tt_current_log_segment);
	};
#if TT_WRITE_ZIP_FILE_INCREMENTALLY
	string result = update_versions(max,short_file_name); // current_version,oldest_version,youngest_version);
		//	no need to delete short_file_name since update_versions will take care of this
#else
	string result = update_versions(max,append_strings(directory,short_file_name)); //,current_version,oldest_version,youngest_version);
	if (log_out_archive() == NULL) { // new on 110104
		char short_archive_name[MAX_PATH]; // more than enough - prior to 170204 was 20
		if (tt_log_out_file_name_specified) {
			strcpy(short_archive_name,tt_log_out_file_name_specified);
		} else {
			const int demo_length = strlen("demo");
			memcpy(short_archive_name,"demo",demo_length);
			itoa(tt_current_log_segment,short_archive_name+demo_length,10);
		};
		strcat(short_archive_name,".dmo");
		// can't use the same name -
		// the 0000 hack was annoying but isn't necessary for XP and beyond 
		// (with other systems the directory when sorted by name will not look right)
		tt_log_out_archive = log_out_archive_without_time_travel(append_3_strings(tt_user_directory,"Demos\\",short_archive_name)); 
		// added demos on 170204
		using_archive = TRUE;
	};
	delete [] short_file_name; 		
#endif
	return(result);
};
	// following obsolete as of 180703
	//} else {
	//	using_archive = FALSE;
	//	return(maintain_n_versions_youngest_last_pre_archive_version(max,tt_current_log_segment));

void save_oldest_and_current_version_numbers(int current_version, int oldest_version, int youngest_version) { // abstracted on 170603
	if (using_archive) {
//		int old_current_version,old_oldest_version,old_youngest_version;
		read_counters_from_archive(); // old_current_version,old_oldest_version,old_youngest_version)) { - no longer conditional
		if (oldest_version < 0) {
			oldest_version = tt_oldest_log_segment;
			if (youngest_version < 0) {
				youngest_version = tt_youngest_log_segment;
			};
		};
		//} else {
		//	oldest_version = 1;
		//	youngest_version = current_version;
		//};
		if (tt_create_new_time_line && tt_log_out_archive != NULL) { // changing the past so copy the old time line first - new on 130703
			// first condition was current_version < youngest_version prior to 010304
			tt_create_new_time_line = FALSE; // since doing it here
			tt_log_version_number = latest_log_version_number; 
			// new on 080404 so toontalk.ini options are updated to current defaults (not what was in time travel archive)
			set_recordable_ini_options(); // new on 080404
			char temp_file[MAX_PATH];
			strcpy(temp_file,tt_user_directory);
			strcat(temp_file,"temp_time_travel.dmo");
			current_version--; // since we want to override old the current version -- commented out 010304
//			string old_current_dmo_file = file_name_renumbered("log00000.dmo",current_version); // removed 150204
//			tt_log_out_archive_size = max(1,copy_zipped_files_before_file(old_current_dmo_file,tt_log_in_archive,temp_file)); 
			// was tt_log_out_archive prior to 200703
			wait_cursor(); // new on 200204 since this can take quite a while
			copy_zipped_files_except_logs_after(current_version,tt_log_in_archive,temp_file); 
			restore_cursor();
			// obsolete 160204
//			tt_log_out_archive_size = max(1,count_all_zip_members(tt_log_out_archive)); 
			// rewritten 150204 (despite the fact that tt_log_out_archive_size should be obsolete) 
//			delete [] old_current_dmo_file;
			maintain_n_versions_youngest_first(tt_log_out_archive,tt_max_number_of_time_lines,TRUE);
			MoveFile(temp_file,tt_log_out_archive);
			if (tt_current_notebook_zip_archive != NULL) {
				// new on 160204 to clean up "newer" notebook versions
				int length = strlen(tt_current_notebook_zip_archive);
				tt_current_notebook_zip_archive[length-4] = '\0'; // remove the .zip part
				const string before_version_number = "notebook.";
				const int before_version_number_length = 9;
				int current_notebook_version = atoi(tt_current_notebook_zip_archive+before_version_number_length);
				tt_current_notebook_zip_archive[length-4] = '.'; // restore
				if (current_notebook_version > 0) { // no problems above
					if (new_notebook_version(current_notebook_version)) { // otherwise this is the current version so need to do all this
						char full_file_name_notebook_zip_archive[MAX_PATH];
						int temp_directory_length = strlen(tt_extracted_file_temp_directory);
						memcpy(full_file_name_notebook_zip_archive,tt_extracted_file_temp_directory,temp_directory_length); 
						// will it always be there? I think so
						strcpy(full_file_name_notebook_zip_archive+temp_directory_length,tt_current_notebook_zip_archive);
						extract_archive_to_directory(full_file_name_notebook_zip_archive,"*.*",tt_user_directory); 
						// new on 160204 so this is the current notebook
						char notebook_file_name[32];
						memcpy(notebook_file_name,before_version_number,before_version_number_length);
						current_notebook_version++;
						itoa(current_notebook_version,notebook_file_name+before_version_number_length,10);
						strcat(notebook_file_name,".zip");
						while (delete_zip_file(tt_log_out_archive,notebook_file_name)) {
							char temporary_file_copy[MAX_PATH]; // new on 160204 to delete the temporary files as well
							memcpy(temporary_file_copy,tt_extracted_file_temp_directory,temp_directory_length);
							strcpy(temporary_file_copy+temp_directory_length,notebook_file_name);
							::DeleteFile(temporary_file_copy);
							current_notebook_version++;
							itoa(current_notebook_version,notebook_file_name+before_version_number_length,10);
							strcat(notebook_file_name,".zip");
						};
					};
				};
			};
			youngest_version = current_version; // forget about those now
			tt_youngest_log_segment = youngest_version; // new on 190703
			tt_current_log_segment = youngest_version; // will be incremented soon
			// commented out the following on 160204 since those files are now deleted
//#if TT_ALPHA_FEATURE
//			tt_file_temp_directory_is_up_to_date = FALSE; // new on 031103 since may record new segments with the same name as those in old time line
//#endif
		};
	} else {
	// else handle old pre-archive version
		string count_file_name = append_strings(tt_user_directory,"log00000.dmo.n"); // rationalized on 250603
//	string count_file_name = append_strings(file_name,".n");
		HANDLE count_file = CreateFile(count_file_name,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		int ignore_current_version;
		DWORD bytes_read; //,bytes_written;
		if (count_file == INVALID_HANDLE_VALUE) { // first time updated on 151299 was == NULL earlier
			oldest_version = 1;
		} else if (ReadFile(count_file,(char *) &ignore_current_version,sizeof(ignore_current_version),&bytes_read,NULL)) {
			if (oldest_version < 0) {
				ReadFile(count_file,(char *) &oldest_version,sizeof(oldest_version),&bytes_read,NULL);
				// new on 170603 to keep 3 numbers: current, oldest, youngest
				if (youngest_version < 0) {
					if (!ReadFile(count_file,(char *) &youngest_version,sizeof(youngest_version),&bytes_read,NULL)) {
						youngest_version = current_version;
					};
				};		
			};
			CloseHandle(count_file);
		} else {
			oldest_version = 1;
			CloseHandle(count_file);
		};
		delete [] count_file_name;
	};
	if (current_version == 1) { // no point otherwise - need to create archive for the rest to work
		write_counters_in_archive(current_version,oldest_version,youngest_version); // copied on 180703 until segment is finished being written
	};
	using_archive = TRUE; 
	// restored on 120104 since no longer showed relative time when double clicking a time travel archive - was (log_out_archive() != NULL); 
	// was TRUE prior to 110104
	//count_file = CreateFile(count_file_name,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	//WriteFile(count_file,(char *) &current_version,sizeof(current_version),&bytes_written,NULL);
	//WriteFile(count_file,(char *) &oldest_version,sizeof(oldest_version),&bytes_written,NULL);
	//WriteFile(count_file,(char *) &youngest_version,sizeof(youngest_version),&bytes_written,NULL);
	//CloseHandle(count_file);
};

string next_file_name(string file_name, boolean &name_is_new, boolean should_contain_dash) {
	if (file_name == NULL) return(NULL); // added 071299 for robustness
   // looks at last character before the .
   // if a digit then increments it
   // if necessary will carry or insert a new character
   int length = strlen(file_name);
   int i;
   for (i = length-1;; i--) { // search from end backwards
      if (i == 0 || file_name[i] == '\\' || file_name[i] == '/' || file_name[i] == ':') { 
			// new on 080401 to test file_name[i] - dots don't count left of these characters
			return(FALSE); // didn't find a dot
		};
      if (file_name[i] == '.') break;
   };
   i--;
   while (file_name[i] == '9' && i > 0) {
      file_name[i] = '0';
      i--;
   };
	int j = i-1;
	while (j > 0 && file_name[j] >= '0' && file_name[j] <= '9') j--; // finding the first non-digit heading to the left
	boolean contains_dash = (!should_contain_dash || (file_name[j] == '-' || file_name[j] == '_'));
   if (file_name[i] >= '0' && file_name[i] < '9' && contains_dash) {
		file_name[i]++;
		name_is_new = FALSE;
		return(file_name);
	};
	name_is_new = TRUE;
   ascii_string new_name = new char[length+5]; // enough extra
   memcpy(new_name,file_name,length);
	int end = i+2;
	if (!contains_dash) {
		new_name[i+1] = '-';
		new_name[i+2] = '1';
		end++;
	} else {
		new_name[i+1] = '1';
	};
	memcpy(new_name+end,file_name+i+1,length-i);
	return(new_name);
};

void initialize_keyboard_accelerators() {
	if (tt_keyboard_accelerators == NULL) {
		tt_keyboard_accelerators = copy_ascii_string(AS(IDS_KEYBOARD_ACCELERATORS,""));
	};
	if (tt_keyboard_accelerators == NULL || tt_keyboard_accelerators[0] == NULL) {
		// if missing then use default from static resource table
		tt_keyboard_accelerators = copy_ascii_string(AC(IDC_DEFAULT_KEYBOARD_ACCELERATORS));
	};
	if (tt_vacuum_keyboard_accelerators == NULL) {
		tt_vacuum_keyboard_accelerators = SC(IDS_VACUUM_KEY_COMMANDS);
	};
	if (tt_expander_keyboard_accelerators == NULL) {
		tt_expander_keyboard_accelerators = SC(IDS_EXPANDER_KEY_COMMANDS);
	};
	if (tt_copier_keyboard_accelerators == NULL) {
		tt_copier_keyboard_accelerators = SC(IDS_COPIER_KEY_COMMANDS);
	};
};

void release_keyboard_accelerators() {
	if (tt_keyboard_accelerators != NULL) delete [] tt_keyboard_accelerators;
	if (tt_vacuum_keyboard_accelerators != NULL) delete [] tt_vacuum_keyboard_accelerators;
	if (tt_expander_keyboard_accelerators != NULL) delete [] tt_expander_keyboard_accelerators;
	if (tt_copier_keyboard_accelerators != NULL) delete [] tt_copier_keyboard_accelerators;
};

boolean keyboard_accelerator(char key, KeyboardAccelerator accelerator) {
	key = lower_case_character(key);
	return(key == tt_keyboard_accelerators[accelerator]);
};

string space(SpaceBetweenWords context) { // new on 200799
	string result;
	switch (context) {
		case SPACE_BEFORE_PRONOUN:
			result = S(IDS_SPACE_BEFORE_PRONOUNS," "); // if no entry so assume ordinary space
			break;
		case SPACE_BEFORE_OR_AFTER_NUMBERS:
			result = S(IDS_SPACE_BEFORE_OR_AFTER_NUMBERS," ");
			break;
		default:
			result = S(IDS_DEFAULT_SPACE_BETWEEN_WORDS," ");
	};
	if (strcmp(result,"NOTHING") == 0) return(""); // special token meaning no space
	return(result);
};

string comma() {
	return(S(IDS_COMMA,","));
};

string end_sentence(character terminator) {
	switch (terminator) {
		case '!':
			return(S(IDS_END_SENTENCE_EXCLAMATION,"! "));
		case '?':
			return(S(IDS_END_QUESTION,"? "));
		default:
			return(S(IDS_END_SENTENCE,". "));
	};
};

void write_string(output_stream &out, string s, long true_length) {
   // re-written on 111102 to deal better with strings longer than unsigned short
	if (s == NULL) { // new on 110601
		true_length = 0;
	} else if (true_length < 0) {
		true_length = strlen(s)+1; // include terminating NULL
   } else {
      true_length++;
	};
#if TT_DYNA_ZIP
	// while switching to zip archives may as well rationalize this
	if (s == NULL) {
		// don't know why VS 8 complains but rewrote this on 161105
		out.write("\x0\x0\x0\x0",sizeof(true_length)); // in 3.180 first arg was "0000" rather than binary 
	} else {
	  out.write((string) &true_length,sizeof(true_length));
	  out.write(s,true_length);
	};
#else
   unsigned short short_length = 0;
	while (TRUE) { // keep writing 64K blocks
      if (true_length > max_unsigned_short) {
         short_length = max_unsigned_short;
         true_length -= max_unsigned_short;
      } else {
         short_length = (unsigned short) true_length;
         true_length = 0;
      };
	   out.write((string) &short_length, sizeof(short_length));
	   out.write(s,short_length);
      if (short_length != max_unsigned_short) break;
      if (true_length > 0) {
         s += max_unsigned_short;
      };
	};
#endif
};

string FileInputStream::read_string(string s) {
	return(::read_string(stream,s));
};

string FileInputStream::read_string_old(string s) {
	return(::read_string_old(stream,s));
};

string StringInputStream::read_string(string s) {
	return(::read_string(*stream,s));
};

string StringInputStream::read_string_old(string s) {
	return(::read_string_old(*stream,s));
};

string read_string(input_stream &in, string s) {
	if (in == NULL) return(NULL); // new on 050600
	if (tt_log_version_number > 31) { // new on 090703
		long length;
		in.read((string) &length, sizeof(length));
		if (length == 0) {
#if TT_DEBUG_ON
			if (tt_debug_mode == 160299) {
				tt_error_file() << "Just read a string of zero length" << endl; // - file position is " << (int) in.tellg() << endl;
			};
#endif
			return(NULL); // new on 290999
		};
#if TT_DEBUG_ON
		if (length > 1000000 || length < 0) {
			tt_error_file() << "String is more than 1,000,000 bytes long: " << length << endl;
		};
#endif
		if (length >= tt_cache_memory_max_size || length < 0) { 
			if (length == 808464432) { // this was a bug only 3.180 -- where NULL was saved with this length -- fixed 221105
				return(NULL);
			};
			// no point risking running out of memory here - no strings can be that large - new on 061003
			tt_error_file() << "Reading a string from a file that claims to be " 
				             << length 
								 // following mistyped ignoring prior to 121205
								 << " btyes long. Ignoring it but perhaps the file has been corrupted." << endl;
			if (length > 0) in.ignore(length);
			return(NULL);
		};
		if (s == NULL) {
			s = new char[length];
		};
		in.read(s,length);
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299) {
			tt_error_file() << "Just read a string '" << s << endl; // "' - file position is " << (int) in.tellg() << endl;
		};
#endif
		if (in == NULL) { // new on 180903 to deal with broken files (or code)
			delete [] s;
			tt_error_file() << "Failed to read a string of length " << length 
								 << ". Either a file was corrupted or there is a ToonTalk bug." << endl;
			return(NULL);
		};
		return(s);
	};
	unsigned short length;
	in.read((string) &length, sizeof(length));
	if (length == 0) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299) {
			tt_error_file() << "Just read a string of zero length - file position is " << (int) in.tellg() << endl;
		};
#endif
		return(NULL); // new on 290999
	};
//	string s = new char[length];
	if (s == NULL) {
		s = new char[length];
	};
   long total_length = length;
	in.read(s,length);
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299) {
		tt_error_file() << "Just read a string '" << s << "' - file position is " << (int) in.tellg() << endl;
	};
#endif
   while (length == max_unsigned_short) { // new on 111102
	   in.read((string) &length, sizeof(length));
      long already_read = total_length;
      total_length += length;
      string new_s = new char[total_length];
      memcpy(new_s,s,already_read);
      delete [] s;
      s = new_s;
      in.read(s+already_read,length);
   };
	return(s);
};

string read_string_old(input_stream &in, string s) {
	if (in == NULL) return(NULL); // new on 050600
	unsigned short length;
	in.read((string) &length, sizeof(length));
	if (length == 0) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 160299) {
			tt_error_file() << "Just read a string of zero length - file position is " << (int) in.tellg() << endl;
		};
#endif
		return(NULL); // new on 290999
	};
	if (s == NULL) {
		s = new char[length];
	};
   long total_length = length;
	in.read(s,length);
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299) {
		tt_error_file() << "Just read a string '" << s << "' - file position is " << (int) in.tellg() << endl;
	};
#endif
   while (length == max_unsigned_short) { // new on 111102
	   in.read((string) &length, sizeof(length));
      long already_read = total_length;
      total_length += length;
      string new_s = new char[total_length];
      memcpy(new_s,s,already_read);
      delete [] s;
      s = new_s;
      in.read(s+already_read,length);
   };
	return(s);
};

//void write_string(output_stream &out, string s, long len) {
//	unsigned short length;
//	if (s == NULL) { // new on 110601
//		length = 0;
//	} else if (len < 0) {
//		length = (unsigned short) (strlen(s)+1); // include terminating NULL
//	} else {
//		length = (unsigned short) len+1;
//	};
//	out.write((string) &length, sizeof(length));
//	out.write(s,length);
//	if (len > length) { 
//		log("Warning: a string was recorded that was over 64K long so only part was saved.");
//	};
//};

// the following accepts c:\foo.bar as a URL - not what I mean by a URL
//boolean is_url(string name) {
//	wide_string unicode_name = copy_wide_string(name);
//	boolean answer = (IsValidURL(NULL,unicode_name,0) == S_OK);
//	delete [] unicode_name;
//	return(answer);
//};

boolean is_url(string name) {
	return(strnicmp(name,"http:",5) == 0 ||
			 strnicmp(name,"ftp:",4) == 0 ||
			 strnicmp(name,"https:",5) == 0 ||
			 strnicmp(name,"gopher:",7) == 0);
};

// these were updated on 140300 to work even with m22 - so maybe the name should change??
city_coordinate grow_width_to_640x480_screen_size(city_coordinate w) {
	if (tt_screen_width <= tt_graphics_video_mode_width) return(w); // already fine
	if (w > max_long/3000) { // new on 150202 to deal with overflow
		return(tt_screen_width*(w/tt_graphics_video_mode_width));
	} else {
		return((w*tt_screen_width)/tt_graphics_video_mode_width);
	};
};

city_coordinate grow_height_to_640x480_screen_size(city_coordinate h) {
	if (tt_screen_height <= tt_graphics_video_mode_height) return(h); // already fine
	if (h > max_long/3000) {
		return(tt_screen_height*(h/tt_graphics_video_mode_height));
	} else {
		return((h*tt_screen_height)/tt_graphics_video_mode_height);
	};
};

city_coordinate shrink_width_from_640x480_screen_size(city_coordinate w) {
	if (tt_screen_width <= tt_graphics_video_mode_width) return(w); // already fine
	if (w > max_long/3000) { // new on 150202 to deal with overflow
		return(tt_graphics_video_mode_width*(w/tt_screen_width));
	} else {
		return((w*tt_graphics_video_mode_width)/tt_screen_width);
	};
};

city_coordinate shrink_height_from_640x480_screen_size(city_coordinate h) {
	if (tt_screen_height <= tt_graphics_video_mode_height) return(h); // already fine
	if (h > max_long/3000) {
		return(tt_graphics_video_mode_height*(h/tt_screen_height));
	} else {
		return((h*tt_graphics_video_mode_height)/tt_screen_height);
	};
};

int shrink_percent_from_640x480_screen_size(int percent) {
	if (tt_screen_width <= tt_graphics_video_mode_width) return(percent); // already fine
	if (tt_good_sizes_adjust_to_screen_size) {
		return((percent*tt_graphics_video_mode_width)/tt_screen_width);
	};
	return(percent);
};

//#if TT_MICROSOFT
//flag get_string_type_ok = TRUE; // seems Win95/98 doesn't support this
//#else
// had some problems with Borland C++ here
//const flag get_string_type_ok = FALSE;
//#endif

int last_space_or_punctuation(wide_string text, int length, boolean or_ascii_character) {
	// rewritten 221199 so space and ascii are of equal status
	if (text == NULL) return(-1);
	if (length < 0) length = wide_string_length(text);
	int index = length-1; // -1 was missing prior to 171099
	while (index >= 0) {
		// I really don't want any punctuation - e.g. not ' in don't or " in "foo"
		if (text[index] == ' ' || text[index] == wide_space ||
			 text[index] == wide_period || text[index] == wide_question || 
			 text[index] == wide_exclamation || text[index] == wide_comma ||
			 text[index] == wide_close_paren) { // added 031199
			return(index);
		};
		if (or_ascii_character && text[index] < 256 && text[index] != ' ') { // new on 161099
			return(index); 
		};
		index--;
	};
	return(-1); // didn't find one
};

int count_sentences(string text, int length) { // new on 100102
	int count = 0;
	boolean inside_parens = FALSE;
	boolean inside_single_quote = FALSE;
//	boolean inside_double_quote = FALSE;
	for (int i = 0; i < length; i++) {
		if (!inside_parens && !inside_single_quote && phrase_ending_punctuation(text,i,length,FALSE,FALSE)) { // && !inside_double_quote
			if (i+1 == length || text[i+1] == ' ') { // last one or followed by space
				count++;
			};
		} else if (text[i] == '(') {
			inside_parens = TRUE;
		} else if (text[i] == ')') {
			inside_parens = FALSE;
//		} else if (text[i] == '"') { -- seems that breaking sentences doesn't deal with this either
//			inside_double_quote = !inside_double_quote;
		} else if (text[i] == '\'') {
			inside_single_quote = !inside_single_quote;
		};
	};
	return(count);
};

int end_of_nth_sentence(int sentence_count, wide_string text, int length) { // new on 100102
	// returns 0 or end of the nth sentence
	if (sentence_count <= 0) return(0);
	boolean inside_parens = FALSE;
	boolean inside_single_quote = FALSE;
	boolean inside_double_quote = FALSE;
	for (int i = 0; i < length; i++) {
		if (!inside_parens && !inside_single_quote && !inside_double_quote && phrase_ending_punctuation(text,i,length,FALSE,FALSE)) {
			if (i+1 == length || (text[i] != '.' || text[i+1] == ' ')) { // last one or not a period or followed by space (to exclude 3.14159)
				sentence_count--;
			};
			if (sentence_count == 0) {
				while (i < length && text[i] == ' ') {
					i++;
				};
//				if (i+1 == length) {
//					return(i);
//				} else {
					return(i+1); // skip over final punctuation
//				};
			};
		} else if (text[i] == '(') {
			inside_parens = TRUE;
		} else if (text[i] == ')') {
			inside_parens = FALSE;
		} else if (text[i] == '"') {
			inside_double_quote = !inside_double_quote;
		} else if (text[i] == '\'') {
			inside_single_quote = !inside_single_quote;
		};
	};
	return(0);
};

/* no longer want to give "precedence" to real punctuation over space and ascii
int last_space_or_punctuation(wide_string text, int length, boolean or_ascii_character) {
	if (text == NULL) return(-1);
	if (length < 0) length = wide_string_length(text);
	int index = length-1; // -1 was missing prior to 171099
	while (index >= 0) {
		// I really don't want any punctuation - e.g. not ' in don't or " in "foo"
		if (text[index] == wide_period || text[index] == wide_question || 
			 text[index] == wide_exclamation || text[index] == wide_comma ||
			 text[index] == wide_close_paren) { // added 031199
			return(index);
		};
//		if (or_ascii_character && text[index] < 256 && text[index] != ' ') { // new on 161099
//			return(index); 
//		};
		index--;
	};
	index = length;
	while (index >= 0) {
		if (text[index] == ' ' || text[index] == wide_space) {
			return(index);
		};
		if (or_ascii_character && text[index] < 256) { // new on 161099 -- moved here on 041199
			return(index); 
		};
		index--;
	};
	return(-1); // didn't find one
};
*/

// following only works right in NT so can't use it
/*
#if TT_MICROSOFT
	wide_string types = new wide_character[length];
	if (!GetStringTypeW(CT_CTYPE1,text,length,types)) {
		get_string_type_ok = FALSE;
		delete [] types;
		return(non_nt_last_space_or_punctuation(text,length));
	};
	int index = length;
	while (index >= 0) {
		if (//types[index] == C1_SPACE || 
			 types[index] == C1_PUNCT) {
			delete [] types;
			return(index);
		};
		index--;
	};
	// no punctuation so look for spaces
	index = length;
	while (index >= 0) {
		if (types[index] == C1_SPACE) {
			delete [] types;
			return(index);
		};
		index--;
	};
	delete [] types;
   return(-1); // didn't find one
#endif
	*/

int last_space_or_punctuation(ascii_string text, int length, boolean ) {
	if (text == NULL) return(-1);
	if (length < 0) length = strlen(text);
	int index = length-1;
	while (index >= 0) {
		// I really don't want any punctuation - e.g. not ' in don't or " in "foo"
		if (text[index] == '.' 
//		      && (tt_language != BRAZILIAN_PORTUGUESE || // new on 150600 since numbers are 1. 234. 567
//		      index+2 >= length || text[index+1] != ' ' ||
//			   text[index+2] < '0' || text[index+2] > '9')) 
				||  
			 text[index] == '?' || text[index] == '!' || text[index] == ',') {
			if (index+1 == length || text[index+1] == ' ') { // new on 100700 so numbers like 1.234 for example aren't broken
				return(index);
			};
		};
		index--;
	};
	index = length;
	while (index >= 0) {
		// I really don't want any punctuation - e.g. not ' in don't or " in "foo"
		if (text[index] == ' '
//			  && (tt_language != BRAZILIAN_PORTUGUESE || // new on 150600 since numbers are 1. 234. 567
//		     index+1 >= length || index == 0 || text[index-1] != '.' || 
//			  text[index+1] < '0' || text[index+1] > '9')
			) {
			return(index);
		};
		index--;
	};
	return(-1); // didn't find one
};

boolean phrase_ending_punctuation(character *characters, int index, int length, boolean commas_too, boolean close_parens_too) {
	// semicolon added 221199
//	if (index+1 < length && characters[index+1] != ' ') return(FALSE); // new on 100700 so numbers like 1.234 for example aren't broken
	if (index+1 < length && characters[index+1] >= '0' && characters[index+1] <= '9') return(FALSE);
	// re-written on 280700 to ignore punctuation followed by a digit
	character c = characters[index];
	return(c == '.'
//		      &&  (tt_language != BRAZILIAN_PORTUGUESE || // new on 150600 since numbers are 1. 234. 567
//		      index+2 >= length || characters[index+1] != ' ' || 
//			   characters[index+2] < '0' || characters[index+2] > '9'))
			 || 
		    (c == ',' && commas_too) || c == '!' || c == '?' || (close_parens_too && c == ')') || c == ';');
};

boolean phrase_ending_punctuation(wide_character *characters, int index, int length, boolean commas_too, boolean close_parens_too) {
	// close_parens_too added 100102
	wide_character c = characters[index];
	return((c == wide_period || c == '.') ||
//			  && (tt_language != BRAZILIAN_PORTUGUESE || // new on 150600 since numbers are 1. 234. 567
//					index+2 >= length || characters[index+1] != ' ' || 
//					characters[index+2] < '0' || characters[index+2] > '9'))
			 (c == wide_comma && commas_too) || c == wide_exclamation || c == wide_question || 
			 (close_parens_too && (c == wide_close_paren ||  c == ')')) ||	// close paren check added 031199
			 (c == ',' && commas_too) || c == '!' || c == '?' || c == ';'); // added semi-colon on 100102
};

boolean space_or_punctuation(wide_character *characters, int index, int length) {
	return(phrase_ending_punctuation(characters,index,length) || 
			 characters[index] == wide_space || characters[index] == ' ' );
};

boolean contains_non_ascii(wide_string wide_text, int wide_length) {
	if (wide_length < 0) wide_length = wide_string_length(wide_text);
	for (int i = 0; i < wide_length; i++) {
		if (wide_text[i] > 255) {
			return(TRUE);
		};
	};
	return(FALSE);
};

wide_string remove_spaces_if_non_ascii_neighbors(wide_string text, int old_length, int &new_length) {
	if (old_length < 0) { // new on 210101 for robustness
		new_length = 0;
		return(NULL);
	};
   // copies text unless it is a space with Unicode neighbors
	wide_string result = new wide_character[old_length+1]; // might be a bit longer than need be
	new_length = 0;
	for (int i = 0; i < old_length; i++) {
		if (text[i] != ' ' || (i > 0 && i+1 < old_length && text[i-1] <= 255 && text[i+1] <= 255)) { 
			// skip spaces if at beginning or end or if either neighbor is non-ascii
			result[new_length] = text[i];
			new_length++;
		};
	};
	result[new_length] = 0; // terminate just in case
	return(result);
};

/* Only works in NT
#if TT_MICROSOFT
	wide_character text[1];
	text[0] = c;
   wide_character types[1];
	if (GetStringTypeW(CT_CTYPE1,text,1,types)) {
		return(types[0] == C1_SPACE || types[0] == C1_PUNCT);
	} else { // seems Win95/98 doesn't support this
		get_string_type_ok = FALSE;
		return(non_nt_space_or_punctuation(c));
	};
#endif
*/

int max_subtitle_length() {
	int max_length = max_fat_characters_per_subtitle_line; //+max_fat_characters_per_subtitle_line/2; // ok to shrink them a bit
	if (tt_good_sizes_adjust_to_screen_size) { 
		max_length = grow_width_to_640x480_screen_size(max_length);
	};
	return(max_length);
};

city_coordinate minimum_character_width() {
	return(minimum_character_pixel_width*tt_screen->one_pixel_in_x_city_units()); // prior to 240999 was *100
};

city_coordinate minimum_character_height() {
	return(minimum_character_pixel_height*tt_screen->one_pixel_in_x_city_units());
};

wide_string remove_parenthetical_remark(wide_string text) {
	int length = wide_string_length(text);
	wide_string result = NULL;
	int result_index;
	for (int i = 0; i < length; i++) {
      if (text[i] == '(') { // found one
			result = new wide_character[length];
			memcpy(result,text,i*sizeof(wide_character));
			result_index = i;
         while (i < length) {
            if (text[i] == ')') {
               break; // while loop
            };
            i++;
         };
      } else if (result != NULL) {
			result[result_index++] = text[i];
		};
	};
	if (result == NULL) return(NULL);
	result[result_index] = NULL; // terminate string
	return(result);
};

void remove_extra_spaces(string text) {
	int length = strlen(text);
	for (int i = 0; i < length; i++) {
		if (text[i] == ' ' && (i == 0 || text[i-1] == ' ')) { // either 2 spaces or starts with a space
			for (int j = i; j < length; j++) {
				text[j] = text[j+1];
			};
//			text[length-1] = NULL; // terminate earlier
			length--; // since shifted everything to the left
		};
	};
	if (text[length-1] == ' ') { // trailing space
		text[length-1] = NULL; // terminate sooner
	};
};

boolean file_exists(string name, boolean might_be_url) {
#if TT_MICROSOFT
	// added might_be_url on 020301
	if (might_be_url && is_url(name)) {
		URLFile test(name);
		return(!test.Empty());
	} else {
#endif
//		OFSTRUCT file_struct;
//		return(OpenFile(name,&file_struct,OF_EXIST) != -1); // is HFILE_ERROR better?
		// re-written since above only works for file with 128 byte long file names or less
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
		MessageBox(NULL,name,"file_exists",MB_OK);
#endif
		HANDLE handle = CreateFile(name,0,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if (handle == INVALID_HANDLE_VALUE) {
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
			MessageBox(NULL,"INVALID","file_exists",MB_OK);
#endif
			OFSTRUCT file_struct;
			boolean ok = (OpenFile(name,&file_struct,OF_EXIST) != -1);
			if (ok) {
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
				MessageBox(NULL,"OpenFile Valid","file_exists",MB_OK);
#endif
			};
#if TT_DEBUG_ON
			if (tt_debug_mode == 70501) {
				tt_error_file() << name << " is missing." << endl;
			};
			if (add_to_log_if_interesting_error(3,"CreateFile",87)) {
				tt_error_file() << "Could not verify that the file " << name << " exists due to error above." << endl;
			};
#endif
			return(FALSE);
		} else {
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
			MessageBox(NULL,"VALID","file_exists",MB_OK);
#endif
			CloseHandle(handle);
			return(TRUE);
		};  
#if TT_MICROSOFT
	};
#endif
};

int multi_byte_length(wide_string wide_text, int length) {
   return(WideCharToMultiByte(tt_code_page[tt_use_spoken_alternative],0,wide_text,length,NULL,0,NULL,NULL));
};

boolean control_down() {
	return(tt_control_down); // was GetAsyncKeyState(VK_CONTROL) < 0) prior to 220900
};

// the following (and several others) could be replaced by calls to _splitpath (noted on 270904)

wide_string short_file_name(wide_string path, boolean leave_extension) { // new on 311099
	int name_ends = wide_string_length(path)-1; // end of string if no dot otherwise just prior to dot
	int i = name_ends;
	if (!leave_extension) { // conditional new on 260201
		while (i > 0) {
			if (path[i] == '.') {
				name_ends = i-1;
				break;
			} else if (path[i] == '\\' || path[i] == ':' || path[i] == '/') { // new on 080401
				break;
			};
			i--;
		};
	};
	int name_begins = 0;
	i = name_ends;
	while (i > 0) {
		if (path[i] == '\\' || path[i] == ':' || path[i] == '/') {
			name_begins = i+1;
			break;						
		} else if (path[i] == '.') { // two dots - new on 091000 ignore second one too (e.g. drip.wav.url or bomb.gif.lnk)
			name_ends = i-1;
		};
		i--;
	};
	return(copy_wide_string(path+name_begins,1+name_ends-name_begins));
};

ascii_string short_file_name(ascii_string path, boolean leave_extension) { // copied the above for ascii strings on 160200
	int name_ends = strlen(path)-1; // end of string if no dot otherwise just prior to dot
	int i = name_ends;
	if (!leave_extension) { // conditional new on 260201
		while (i > 0) {
			if (path[i] == '.') {
				name_ends = i-1;
				break;
			} else if (path[i] == '\\' || path[i] == ':' || path[i] == '/') { 
				// new on 070401 since don't want to be throw off by dots in directory names
				break;
			};
			i--;
		};
	};
	int name_begins = 0;
	i = name_ends;
	while (i > 0) {
		if (path[i] == '\\' || path[i] == ':' || path[i] == '/') {
			name_begins = i+1;
			break;						
		};
		i--;
	};
	return(copy_string(path+name_begins,1+name_ends-name_begins));
};

int short_file_name_start(ascii_string path) { // new on 180303
	int i = strlen(path)-1; // end of string if no dot otherwise just prior to dot
	while (i > 0) {
		if (path[i] == '\\' || path[i] == ':' || path[i] == '/') {
			return(i+1);
					
		};
		i--;
	};
	return(0);
};

char line_terminator() {
	if (need_wide_characters(current_language())) {
		return(0); // no terminator -- new on 041199
	} else {
		return(' '); // connect separate lines by space
	};
};

boolean need_wide_characters(NaturalLanguage language) {
	return(language == JAPANESE || tt_unicode);
};

boolean virtual_right_button(char button_status, boolean for_a_sensor) {
	if (button_status&SHIFT_KEY_DOWN) { // || button_status&CONTROL_KEY_DOWN) { // commented out on 041199
	   return(button_status&RIGHT_BUTTON || button_status&LEFT_BUTTON ||
				 button_status&MIDDLE_BUTTON);
   };
   switch (tt_mouse_button_count) {
     case 0:
	  case 1:
		  if (!for_a_sensor) { // conditional new on 100600
			  return(FALSE); // only TRUE if above returned
		  };
	  case 2:
		  return(button_status&RIGHT_BUTTON);
	  case 3:
		  return(button_status&RIGHT_BUTTON); //||button_status&MIDDLE_BUTTON);
   };
   return(FALSE); // shouldn't happen
};

boolean virtual_middle_button(char button_status, boolean for_a_sensor) { // new on 041199	if (button_status&CONTROL_KEY_DOWN) {
  if (button_status&CONTROL_KEY_DOWN) {
	  return(button_status&RIGHT_BUTTON || button_status&LEFT_BUTTON ||
				button_status&MIDDLE_BUTTON);
  };
  switch (tt_mouse_button_count) {
     case 0:
	  case 1:
	  case 2:
		  if (!for_a_sensor) { // conditional added 061000
			  return(FALSE); // only TRUE if above returned
		  };
	  case 3:
		  return(button_status&MIDDLE_BUTTON);
  };
  return(FALSE); // shouldn't happen
};

//boolean left_button_when_no_longer_down = FALSE; // new on 070502 for absolute mouse mode to implement dragging
long left_button_went_up_on_frame = 0; // so multiple calls to virtual_left_button return the same thing during the same frame

void note_mouse_state_for_dragging() {
   if (tt_mouse_button&LEFT_BUTTON) { // just went down or is still down
//      left_button_when_no_longer_down = TRUE;
      tt_hand_needs_to_move_to_cursor = TRUE; // I guess true so long as mouse button is held down
      tt_dragging_enabled = TRUE;
#if TT_DEBUG_ON
      if (tt_debug_mode == 70502) {
         tt_error_file() << "Should follow cursor on frame " << tt_frame_number << endl;
      };
#endif
   } else if (tt_dragging_enabled) { // left_button_when_no_longer_down) { // was down but is no longer
//      left_button_when_no_longer_down = FALSE;
      left_button_went_up_on_frame = tt_frame_number;
      tt_hand_needs_to_move_to_cursor = TRUE;
      tt_dragging_enabled = FALSE;
#if TT_DEBUG_ON
      if (tt_debug_mode == 70502) {
         tt_error_file() << "Noticed mouse up on frame " << tt_frame_number << endl;
         tt_error_file() << "Should follow cursor." << endl;
      };
#endif
   };
};

boolean virtual_left_button(char button_status, boolean for_a_sensor) {
   if (button_status&SHIFT_KEY_DOWN || button_status&CONTROL_KEY_DOWN) {
	   return(FALSE);
   };
   if (tt_mouse_mode == ABSOLUTE_MOUSE_MODE && !for_a_sensor) { // or should this be a new flag and one that is logged?? -- new on 070502
      if (left_button_went_up_on_frame == tt_frame_number) {
#if TT_DEBUG_ON
         if (tt_debug_mode == 70502) {
            tt_error_file() << "Reacted to mouse up on frame " << tt_frame_number << endl;
         };
#endif
         return(TRUE);   
      };
      if (tt_mouse_button&LEFT_BUTTON) {
#if TT_DEBUG_ON
         if (tt_debug_mode == 70502) {
            tt_error_file() << "Ignoring mouse down on frame " << tt_frame_number << endl;
         };
#endif
         return(FALSE); // ignore mouse down until it comes up
      };
   };
   switch (tt_mouse_button_count) {
      case 0:
	   case 1: 
		   if (!for_a_sensor) { // conditional added 061000
//		 boolean a_button_down
			   return(button_status&RIGHT_BUTTON || button_status&LEFT_BUTTON || button_status&MIDDLE_BUTTON);
//		 if (modifier_keys_down && !new_modifier_keys_down &&
//			  button_status&BUTTON_STILL_DOWN) { 
			 // modifier buttons have been down and still holding mouse button
//			 return(FALSE);
//		 };
//		 if (a_button_down && !(button_status&BUTTON_STILL_DOWN)) {  // just went down
//			 modifier_keys_down = new_modifier_keys_down;
//		 };
//		 return(a_button_down && !new_modifier_keys_down);
		 };
	   case 2:
	   case 3:
		   return(button_status&LEFT_BUTTON);
   };
   return(FALSE); // shouldn't happen
};

NaturalLanguage specified_spoken_language = NO_LANGUAGE_SPECIFIED;

void set_spoken_language_from_string(string language_name) {
	set_spoken_language(string_to_language(language_name,TRUE));
};

void set_spoken_language(NaturalLanguage language) {
	tt_alternative_spoken_language = language;
	if (tt_alternative_spoken_language == NO_LANGUAGE_SPECIFIED) return;
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	if (tt_alternative_spoken_language == BRITISH && tt_language == AMERICAN) { // new on 101204
		// this is just silly and confusing and caused a crash
		tt_alternative_spoken_language = AMERICAN;
	} else if (tt_alternative_spoken_language == AMERICAN && tt_language == BRITISH) { // new on 101204
		tt_alternative_spoken_language = BRITISH;
	};
	if (tt_alternative_spoken_language == tt_language) { // not really an alternative
		specified_spoken_language = tt_alternative_spoken_language;
		tt_alternative_spoken_language = NO_LANGUAGE_SPECIFIED;
		if (tt_closed_caption_alternative_language){
			tt_closed_caption_alternative_language = FALSE;
			tt_closed_caption = TRUE;
		};
	} else if (alternative_language_spoken()) {
		load_string_library(TRUE);
		if (tt_closed_caption) {
			tt_closed_caption_alternative_language = TRUE;
			tt_closed_caption = FALSE;
		};
	};
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
};

void set_language(NaturalLanguage language) { // new on 121199
	boolean first_time = (tt_language == NO_LANGUAGE_SPECIFIED);
	tt_language = language;
	if (first_time) {
//		set_spoken_language(tt_alternative_spoken_language); // commented out since now happens after loading the log
	} else if (tt_alternative_spoken_language == NO_LANGUAGE_SPECIFIED && specified_spoken_language != NO_LANGUAGE_SPECIFIED) {
		// try to set it again
		set_spoken_language(specified_spoken_language);
	};
	if (tt_language == PORTUGAL_PORTUGUESE) { // new on 210400
		tt_avenues_print_as_numbers = TRUE; // Tiago asked for this
	};
//	if (tt_language == JAPANESE) { // new on 220800
//		if (SetThreadLocale(MAKELANGID(LANG_JAPANESE,SUBLANG_DEFAULT))) {
//			tt_error_file() << "Set thread locale to Japanese." << endl;
//		};
//	};
};

void replace_character(string s, char search, char replace) {
   for (int i = 0; TRUE; i++) {
		if (s[i] == 0) return; // end of string
		if (s[i] == search) {
			s[i] = replace; 
		};
   };
};

void print_spaces(int stop, output_stream &stream) {
	for (int i = 0; i < stop; i++) {
		stream << " ";
	};
};

boolean ignore_subsequent_errors = FALSE;

boolean handle_internal_error(string message, boolean can_recover) {
	// returns TRUE if user wants to keep trying
	if (about_to_quit()) {
		log(message,FALSE,TRUE); // added 240200
		return(FALSE); // new on 140200 to deal with nested errors and a user that wants to quit
	};
	if (!can_recover) {
		if (trouble_shooting()) {
			message_box(MB_OK|MB_SYSTEMMODAL,message);
		} else {
			log(message,FALSE,TRUE); // added 240200
		};
		ShellExecuteG(NULL,"open",tt_err_file_name,"","",SW_SHOW); // new on 011105
		exit(0);
	} else if (ignore_subsequent_errors ||
				  message_box(MB_YESNO|MB_SYSTEMMODAL,
								  message,S(IDS_KEEP_GOING_DESPITE_INTERNAL_ERROR," Do you want ToonTalk to try to keep going anyway?"))
				  == IDYES) {
		if (ignore_subsequent_errors) {
			log(message,FALSE,TRUE); // added 240200
		};
		ignore_subsequent_errors = TRUE;
		set_pre_initialization_warning_max(0); // added on 150400 to ignore these errors as well
		return(TRUE);
	} else {
//		if (message_box(MB_YESNO|MB_OK|MB_SYSTEMMODAL,
//							 "Do you want ToonTalk to try to save your city before exiting?") 
//			 == IDYES) {
		if (has_initialization_completed()) {
			toggle_pause(FALSE,FALSE,TRUE);
		} else {
			exit(0);
		};
	};
   return(!about_to_quit());
};
//boolean toolbox_tool(Sprite *sprite) { // new on 050100 -- obsolete as of 080704
//	if (tt_toolbox == NULL) return(FALSE);
//	return(toolbox_tool(sprite));
//};

// commented out on 250603 since obsolete
//int postponed_count = 0; 
//
//void postpone_new_logs() {
//	if (!tt_logging || tt_dump_as_xml) return; // return if not logging or next log is not scheduled -- 
//	// added tt_dump_as_xml disjunt on 130103 since no need to postpone since the XML cities are "perfect" 
//	postponed_count++;
////	log("postpone_new_logs");
//	if (tt_next_new_log_time_saved < 0 && tt_next_new_log_time < max_long) { // postpone until settled down
//#if TT_DEBUG_ON
//		if (tt_debug_mode == 120100) {
//			tt_error_file() << "Started postponing new logs at " << tt_current_time 
//				         << " where next log was scheduled for " << tt_next_new_log_time << endl;
//		};
//#endif
//		tt_next_new_log_time_saved = tt_next_new_log_time;
//		tt_next_new_log_time = max_long;
//	};
//};
//
//void stop_postponing_new_logs() {
//	if (!tt_logging || tt_dump_as_xml) return; // return if not logging or next log is not scheduled -- 
////	log("stop_postponing_new_logs");
////	tt_error_file() << postponed_count << endl;
//	if (tt_next_new_log_time_saved > 0) {
//		postponed_count--;
//		if (postponed_count > 0) {
//#if TT_DEBUG_ON
//			if (tt_debug_mode == 120100) {
//				tt_error_file() << "Postponed count just reduced to " << postponed_count << " at " << tt_current_time << endl;
//			};
//#endif
//			return; // still more reasons to keep this postponed
//		};
//		if (tt_next_new_log_time_saved <= tt_current_time) {
//			tt_next_new_log_time = tt_current_time; // next cycle
//		} else { // didn't need to postpone so restore things
//			tt_next_new_log_time = tt_next_new_log_time_saved;
//		};
//		tt_next_new_log_time_saved = -1; // no longer postponing
//#if TT_DEBUG_ON
//		if (tt_debug_mode == 120100) {
//			tt_error_file() << "Stopped postponing new logs at " << tt_current_time << endl;
//		};
//#endif
//	};
//};

// still can't get this to work
//ostream& operator<< ( ostream& stream, int64 i) { // new on 080202
//   char buffer[20];
//  sprintf(buffer,"%I64d", i);
//   stream << buffer;
//   return stream;
//};

void print_int64(int64 i, output_stream &stream) {
	char buffer[20];
   sprintf(buffer,"%I64d", i);
   stream << buffer;
};

int max_entries = 2; // will double as needed - moved here on 040301

void add_directory_to_file_search_path(string original_entry) { // made a procedure on 040301
	string entry = canonicalize_path(original_entry); // new on 050301
	delete [] original_entry;
	for (int i = 0; i < tt_file_search_path_length; i++) { // new on 040301 to check first if already there (since comes from different sources)
		if (stricmp(entry,tt_file_search_path[i]) == 0) {
			delete [] entry; // new on 160103
			return; // already there (this doesn't check for equivalent ways of expressing the same directory...)
		};
	};
#if TT_DEBUG_ON
	tt_error_file() << "Adding " << entry << " to search path." << endl;
#endif
	tt_file_search_path_length++; 
	if (tt_file_search_path_length > max_entries) { // need to make room for it
		int old_max_entries = max_entries;
		max_entries += max_entries; // double it
		string *new_tt_file_search_path = new string[max_entries];
		for (int i = 0; i < old_max_entries; i++) {
			new_tt_file_search_path[i] = tt_file_search_path[i];
		};
		delete [] tt_file_search_path;
		tt_file_search_path = new_tt_file_search_path;
	};
	tt_file_search_path[tt_file_search_path_length-1] = entry;
};

void release_file_search_path() {
	if (tt_file_search_path_length > 0) {
	   // the following should be right but for some reason triggered an exception??
	   // restored on 251000
	   for (int i = 0; i < tt_file_search_path_length; i++) {
		   if (tt_file_search_path[i] != NULL) delete [] tt_file_search_path[i];
		};
	   delete [] tt_file_search_path;
	   tt_file_search_path_length = 0;
	   tt_file_search_path = NULL;
	};
};

void initialize_tt_file_search_path() { // new on 160200
//	if (tt_file_search_path_length > 0) {
//		return; // already initialized;
//	};
	release_file_search_path(); // new on 050301 since can get reset now
	if (tt_file_name == NULL) { // new on 050301
		return; // too early - should be called again when name is set
	};
	tt_file_search_path = new string[max_entries];
	if (tt_URL_cache_directory != NULL) { // new on 040301
		add_directory_to_file_search_path(copy_string(tt_URL_cache_directory));
	};
	string path = ini_entry(AC(IDC_DIRECTORIES),"FileSearchPath",FALSE);
	string secondary_path = ini_entry(AC(IDC_DIRECTORIES),"SecondaryFileSearchPath",FALSE);
	if (path == NULL) {
		if (secondary_path == NULL) return;
		path = secondary_path;
	} else if (secondary_path != NULL) { // both paths
		string all = append_3_strings(path,";",secondary_path);
		delete [] path;
		delete [] secondary_path;
		path = all;
	};
// tt_file_search_path_length;
	int i = 0;
	boolean contains_colon = FALSE;
	int asterisk_location = -1;
	int entry_start = 0;
	int user_name_length = strlen(tt_file_name);
	int path_to_user_subdirectory_length = strlen(tt_user_directory)-(1+user_name_length); // +1 to remove final slash
	while (TRUE) {
		if ((path[i] == ';' || path[i] == 0) && i > 0) { // got one semi-colon or end of string
			int length = i-entry_start;
			boolean missing_final_slash;
			if (path[i] == 0) { // end of string
				missing_final_slash = (path[i] != '/' && path[i] != '\\');
			} else {
				missing_final_slash = (path[i-1] != '/' && path[i-1] != '\\');
			};
			if (missing_final_slash) length++;
			if (asterisk_location >= 0) {
				length += user_name_length; // asterisk is replaced with tt_file_name
			};
			boolean relative = (!contains_colon && path[entry_start] != '\\' && path[entry_start] != '?'); 
			// relative if no colon and doesn't start with a slash -- added ? test since that expands to MainDir as of 191204
			if (relative) { 
				length += path_to_user_subdirectory_length;
			};
			string entry = new char[length+1]; // added +1 on 060400
			int k = 0;
			if (relative) { // prepend path to Users directory
				for (int j = 0; j < path_to_user_subdirectory_length; j++) {
					entry[k++] = tt_user_directory[j];
				};
			};
			if (asterisk_location >= 0) {
				int j;
				for (j = entry_start; j < asterisk_location; j++) { // copy everything before asterisk
					entry[k++] = path[j];
				};
				for (j = 0; j < user_name_length; j++) { // copy user name
					entry[k++] = tt_file_name[j];
				};	
				for (j = asterisk_location+1; j < i; j++) { // copy everything after asterisk
					entry[k++] = path[j];
				};
			} else {
				for (int j = entry_start; j < i; j++) { // copy everything
					entry[k++] = path[j];
				};
			};
			entry[k] = '\0'; // terminate entry
			if (missing_final_slash) {
				if (!relative && is_url(entry)) {
					entry[k++] = '/';
				} else {
					entry[k++] = '\\';
				};
				entry[k] = '\0'; // terminate entry again
			};
			// got a new one
			if (entry[0] == '?') { 
				// new on 191204
				add_directory_to_file_search_path(expand_question_mark(entry));
			} else {
				add_directory_to_file_search_path(entry);
			};
			if (path[i] == 0) break; // end of string
			entry_start = i+1;
			if (path[entry_start] == '\0') { // new on 050301 since could end with a semi-colon
				break;
			};
			contains_colon = FALSE;
			asterisk_location = -1;
		} else {
			if (path[i] == ':') {
				contains_colon = TRUE; 
			} else if (path[i] == '*') {
				asterisk_location = i;
			};
		}
		i++;
	};
	delete [] path;
	if (tt_private_media_directory != NULL) { // should be
		add_directory_to_file_search_path(copy_string(tt_private_media_directory)); 
		// new on 080403 - though only needed for those things made by versions 2.83 thru 2.87
	};
};

string new_title_for_window(string name) { // abstracted on 221103
	const_string title = S(IDS_TOONTALK_WINDOW_TITLE);
	if (title == NULL) { // new on 050203 to recover better from inability to load string DLL
		return(NULL);
	};
	string unicode_window_title = new character[_tcslen(title)+_tcslen(name)+6];
	_tcscpy(unicode_window_title,title);
	_tcscat(unicode_window_title,_T("  -  "));
	_tcscat(unicode_window_title,name);
	return(u2a(unicode_window_title));
};

boolean set_tt_user_name(string new_name) { // new on 260300 since resetting to the same name was clobbering the user history
	if (new_name == NULL) {
		return(FALSE); // new on 081003 -- for robustness
	};
	if ((tt_user_name != NULL && stricmp(new_name,tt_user_name) == 0) // not new news
//		|| tt_log_in_archive != NULL
		) { 
		// or as of 200703 ignore this if reading a time travel archive
		// added i to stricmp on 280300 since using a different casing 
		// caused ToonTalk to think it was a name change and clobbered to user history
		delete [] new_name;
		return(FALSE); // didn't set the name
	} else {
		if (tt_user_name != NULL) {
			tt_main_window->restore_title(new_title_for_window(new_name)); // new on 241103 (plus or minus a day)
			delete [] tt_user_name;
		};
		tt_user_name = new_name;
		return(TRUE);
	};
};

void set_tt_default_file_name(string default_user, boolean even_if_already_set) { // abstracted on 200203
	if (default_user == NULL) return;
	// added even_if_already_set on 141003
	if (!even_if_already_set && tt_default_file_name != NULL) return; // new on 141003
	if (tt_default_file_name != NULL) {
		delete [] tt_default_file_name;
	};
	tt_default_file_name = default_user;
   // following moved here on 200203 so used regardless of how it was set
	if (tt_default_user_directory == NULL) { // conditional new on 200203
      tt_default_user_directory = new char[MAX_PATH];
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	char personal_documents[MAX_PATH];
//	get_special_folder(CSIDL_PERSONAL,personal_documents); // copied here on 200203
	get_my_documents_folder(personal_documents); // updated 050504
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
   output_string_stream default_user_directory_name(tt_default_user_directory,MAX_PATH);
   default_user_directory_name << personal_documents << "\\ToonTalk\\" << tt_default_file_name << "\\";
   default_user_directory_name.put('\0');
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	// new on 270904 to then check ToonTalk installation directory
	if (::GetFileAttributes(tt_default_user_directory) != FILE_ATTRIBUTE_DIRECTORY) {
		output_string_stream default_user_directory_name(tt_default_user_directory,MAX_PATH);
		default_user_directory_name << tt_main_directory << "Users\\" << tt_default_file_name << "\\";
		default_user_directory_name.put('\0');
//		if (!file_exists(tt_default_user_directory)) {
		if (::GetFileAttributes(tt_default_user_directory) != FILE_ATTRIBUTE_DIRECTORY) {
			tt_error_file() << "Could not find the files for DefaultUser named " << default_user << endl;
		};
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
};

void compute_tt_default_file_name() { // prior to 050301 was in-line below
	if (!replaying()) {
		// removed as experiment on 240603 && tt_time_travel == TIME_TRAVEL_OFF) { 
		// otherwise this breaks demos - removed tt_default_file_name == NULL && on 050301
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		string entry = ini_entry(AC(IDC_USER),AC(IDC_DEFAULT_USER));
		// rewritten on 240105 to deal with spaces and other punctuation
		if (entry != NULL) {
			int length = strlen(entry);
			string clean_entry = new char[length+1];
			copy_alphanumerics(entry,clean_entry,length+1);
			set_tt_default_file_name(clean_entry,FALSE);	// FALSE added 141003 so this won't reset if already set - change from prior behavior
			delete [] entry;
		};
   };
};

boolean create_personal_toontalk_directory(string personal_documents) {
   string directory = append_strings(personal_documents,"\\ToonTalk\\"); 
	// decided on 030402 to remove \\Animated Programs since breaks Java applets and even Microsoft breaks this rule
	if (!::CreateDirectory(directory,NULL)) {
      delete [] directory;
      return(FALSE);
   };
   delete [] directory;
   return(TRUE);
};

boolean new_name_has_been_generated = FALSE;
boolean changing_names = FALSE; // easiest way for the 2 pieces to communicate now (030304)

void just_compute_tt_file_name() {
	if (tt_time_travel == TIME_TRAVEL_OFF) { // new on 221103 to leave default file name alone if time travelling
		compute_tt_default_file_name();
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	if (tt_language == BRAZILIAN_PORTUGUESE && replaying()) {
		if (strcmp(tt_user_name,"Sally&Nicky") == 0) { // new on 110500
			delete [] tt_user_name;
			tt_user_name = copy_string("Kica&Dudu");
		} else if (strcmp(tt_user_name,"Pat") == 0) { // new on 120500
			delete [] tt_user_name; // missing prior to 030304
			tt_user_name = copy_string("Beth");
		};
	};
	if (tt_file_name != NULL) { // new on 170300 since logs do rename user now -- was this a good idea? - asked 200703
		if (tt_time_travel != TIME_TRAVEL_OFF) {
			// following is new on 220402 so when double clicking on an archive the right files are loaded for notebooks
			if (tt_file_name_while_time_travelling == NULL) {
				tt_file_name_while_time_travelling = new char[MAX_PATH];
			};
			if (need_wide_characters()) { 
				strcpy(tt_file_name_while_time_travelling,tt_user_name); 
			} else {
				copy_alphanumerics(tt_user_name,tt_file_name_while_time_travelling);
			};
			return; // don't change tt_file_name for time travel archives - new on 200703
		};
		delete [] tt_file_name;
		changing_names = TRUE;
	};
	const int max_user_file_name = 100; // new on 080404 to avoid overflows 
#if TT_NO_ROBOT_TRAINING
	if (!replaying()) {
		tt_file_name = copy_string(tt_user_name,max_user_file_name); // new on 091205
		tt_user_directory = copy_string(tt_extracted_file_temp_directory);
		return;
	};
#endif
	if (need_wide_characters()) {
		tt_file_name = copy_string(tt_user_name,max_user_file_name); // new on 290800
	} else {
		tt_file_name = new char[max_user_file_name]; 
		// was [MAX_PATH] -- updated on 080404 // moved here on 010301 since was wasteful to allocate it before this conditional
		copy_alphanumerics(tt_user_name,tt_file_name,max_user_file_name);
	};
	if (tt_user_directory != NULL) delete [] tt_user_directory; // added on 270200
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
   // following is new on 271199
  	tt_user_directory = new char[MAX_PATH];
//	output_string_stream user_directory_name(tt_user_directory,MAX_PATH); // rewritten on 230104
   char personal_documents[MAX_PATH];
	if (tt_user_provided_user_directory == NULL) {
//		if (tt_main_directory != NULL) stream << tt_main_directory; 
//		stream << AC(IDS_USERS_DIRECTORY) << "\\";
      //re-written on 290302
//      if (!SHGetSpecialFolderPath(NULL,personal_documents,CSIDL_PERSONAL,TRUE)) {
      // replaced above with the following to work on all systems
		get_my_documents_folder(tt_user_directory);
//      if (!get_special_folder(CSIDL_PERSONAL,personal_documents)) {
//#if TT_DEBUG_ON
//	      tt_error_file() << "Failed to find CSIDL_PERSONAL." << endl;
//#endif
//      };
 //     user_directory_name << personal_documents << "\\ToonTalk\\";
//		strcpy(tt_user_directory,personal_documents); // commented out 050504 since was redundant
		strcat(tt_user_directory,"\\ToonTalk\\");
	} else { // new on 300500 so can have this set to a: and use floppies for local files
//		user_directory_name << tt_user_provided_user_directory;
		strcpy(tt_user_directory,tt_user_provided_user_directory);
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
//	user_directory_name << tt_file_name << "\\"; // new on 251199
	strcat(tt_user_directory,tt_file_name);
	strcat(tt_user_directory,"\\");
//	user_directory_name.put('\0');
	if (!::CreateDirectory(tt_user_directory,NULL)) {
      // create directories along path first
      int last_error = GetLastError(); // code 183 is fine - means directory already existed
		if (tt_user_name_should_be_new && last_error == 183) { // new on 030304
			//GUID guid;
			//UuidCreate(&guid);
			//char guid_string[guid_string_size];
			//guid_as_narrow_hexadecimal(&guid,guid_string);
			string new_name;
			int old_user_name_length = strlen(tt_user_name);
			char last_character = tt_user_name[old_user_name_length-1];
			if (last_character >= '0' && last_character < '9') {
				tt_user_name[old_user_name_length-1]++;
			} else {
				if (last_character == '9') {
					// this will go 1, 2, 3 ... 19, 100, 101, ... -- good enuf
					new_name = append_strings(tt_user_name,"0");
				} else {
					new_name = append_strings(tt_user_name,"_1");
				};
				delete [] tt_user_name;
				tt_user_name = new_name;
			};
			delete [] tt_user_directory;
			tt_user_directory = NULL;
			delete [] tt_file_name;
			tt_file_name = NULL;
			just_compute_tt_file_name(); // try again with new name
			new_name_has_been_generated = TRUE;
			return;
		};
		if (last_error == 3) { // new on 010402 // no such path - 
			if (tt_user_provided_user_directory != NULL) {
				// added this branch on 300904 since
				// Zip library will create it very soon typically anyway and this is more consistent
				::CreateDirectory(tt_user_provided_user_directory,NULL);
			} else {
				create_personal_toontalk_directory(personal_documents);
			};
			if (!::CreateDirectory(tt_user_directory,NULL)) {
#if TT_DEBUG_ON
            last_error = GetLastError();
            if (last_error == 3) {
	            tt_error_file() << "Failed to create directory " << tt_user_directory << endl;
            };
#endif
         };
      };
   };
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
};

void use_computed_tt_file_name() {
   // following moved down here on 050402 since it now uses tt_user_directory
   if (!replaying() && tt_replay != REPLAY_REPRODUCE_TIMING_ON_NEXT_FRAME) {
		// second condition new on 020404 to fix but introduced 200304 when replaying() was changed
		initialize_history(TRUE);
#if !TT_NO_ROBOT_TRAINING
		write_ini_entry(AC(IDC_USER),AC(IDC_PREVIOUS_NAME),u2a(tt_user_name));
#endif
	};
	set_directory_names(); // copied here on 010301 since there may be * in the names
	// re-ordered above and below on 040301 since URLCacheDir now is part of file_search_path
	initialize_tt_file_search_path();
	if (changing_names) {
		open_tt_error_file(); // new on 170300 so demos have right log file name
	};
};

void compute_tt_file_name() { // split into 2 on 030304
	if (tt_user_name == NULL) return;
	if (new_name_has_been_generated) return;
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	just_compute_tt_file_name();
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	use_computed_tt_file_name();
};


void truncate(string file_name, int length) { // new on 040300
//	if (length <= 0) return; 
	if (length < 0) { // new on 210300 - means copy the entire file
		//// why is this useful??? asked 130103 and decided to comment it out
		//CopyFile(file_name,temp_file,FALSE);
		//delete [] temp_file;
		return;
	};
	string temp_file = append_strings(file_name,".tmp");
//	DeleteFile(temp_file);
	move_file(file_name,temp_file); // updated 280103
	input_file_stream file(temp_file,std::ios_base::in|std::ios_base::binary);
	char *buffer = new char[length];
	file.read(buffer,length);
	file.close();
	output_file_stream truncated_file(file_name,std::ios_base::out|std::ios_base::binary);
	if (truncated_file == NULL) { // new on 290904 for error recovery
		unable_to_write_file(file_name); 
		return;
	};
	truncated_file.write(buffer,length);
	truncated_file.close();
	delete [] temp_file;
};

boolean rename_file_to_backup(string file_name) { // new on 280103
	// this assumes that file_name has a 3 character extension...
	char new_file_name[MAX_PATH];
	int length = strlen(file_name);
	if (length >= MAX_PATH-2) length -= 2; // in case already full length
	memcpy(new_file_name,file_name,length-3);
	new_file_name[length-3] = 'o'; // for "old"
	memcpy(new_file_name+length-2,file_name+length-3,4); // copy extension and terminator
	return(move_file(file_name,new_file_name));
};

void delete_strings(string *strings, int count) {
	if (count == 0) return;
	for (int i = 0; i < count; i++) {
		if (strings[i] != NULL) delete [] strings[i];
	};
	delete [] strings;
};

boolean ok_to_delete_bammed(Sprite *by) {
	return(tt_log_version_number > 22 && 
			 (by == NULL || by->ok_to_delete_bammed()));
};

void replace_carriage_returns_with_spaces(string text, int length) { // proceduralized on 261100
	for (int i = 0; i < length; i++) {
      if (new_line(text[i])) {
         text[i] = ' '; // replace carriage returns with spaces
      };
      // rewrote on 090802 for clarity
		//if (text[i] == '\r') { // line feed
		//	text[i] = ' '; // replace carriage returns with spaces
		//} else if (text[i] == 10) { // carriage return
		//	text[i] = ' ';
		//};
	};
};

boolean cycle_background_color() {
//	int color_id = (tt_frame_number/10)%10;
	int color_id = (tt_current_time/50)%100; // updated on 191000 to look nicer (and not be dependent upon frame rate) - 1 second cycle now
	// used to divided by 100 then %10 but that was too fast and ugly and perhaps could trigger a fit? - 270103
//	if (color_id > 4) { 
//		color_id = 10-color_id;
//	};	
	if (color_id < 5) { // Sarah Lowe's suggestion so there is a story about why this matches just the appearance on nothing
		int brush_color;
		switch (color_id) { // change color - choose among 5 shades of gray
			case 0:
				brush_color = BLACK_BRUSH;
				break;
			case 1:
				brush_color = DKGRAY_BRUSH;
				break;
			case 2:
				brush_color = GRAY_BRUSH;
				break;
			case 3:
				brush_color = LTGRAY_BRUSH;
				break;
			case 4:
				brush_color = WHITE_BRUSH;
				break;
		};
		tt_main_window->select_stock_object(brush_color); 
		// was BLACK_BRUSH prior to 181000 but background of the sensor in now color animated - different from showing black
		return(TRUE);
	} else {
		return(FALSE);
	};
};

void remove_extension_from_file_name(string file_name) {
	int length = strlen(file_name);
	for (int i = length-1; i > 1; i--) {
		if (file_name[i] == '.') { // remove extension
			file_name[i] = '\0';
			break;
		} else if (file_name[i] == '\\' || file_name[i] == ':' || file_name[i] == '/') { // new on 080401
			return;
		};
	};
};

void compute_local_ini_file_name(string full_file_name) {
	if (tt_ini_file_only_from_tt_ini_file) return; // new on 030304
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	string file_name = copy_string(full_file_name);
	remove_extension_from_file_name(file_name);
	FileNameStatus file_status;
	string ini_file = existing_file_name(file_status,file_name,"ini","NewUsers",TRUE,FALSE,FALSE,FALSE,FALSE); 
	// AC(IDS_USERS_DIRECTORY) was NULL prior to 170302
	if (ini_file != NULL) {
		tt_ini_file_from_tt_ini_file = FALSE; // since is from associated file instead - new on 060304
		if (tt_local_ini_file != NULL) { // new on 040301 so it is reset and storage reclaimed if a new one is found
			delete [] tt_local_ini_file;
			tt_local_ini_file = NULL;
		};
		tt_local_ini_file = ini_file;
//		if (has_initialization_completed()) {
			tt_error_file() << "ToonTalk is now running with a local INI file: " << ini_file << endl;
//		};
		set_directory_names(); // may have changed -- new on 040301
		// may have changed file search path so recompute it
//		if (tt_file_name != NULL) { // commented out on 050301
			release_file_search_path(); 
			initialize_tt_file_search_path();
			compute_tt_default_file_name(); // new on 050301
			// could imagine doing things like changing a voice etc but where to stop???
//		};
	};
	delete [] file_name; // new on 070201 - finished with local copy
#if TT_MICROSOFT
	// system_cached_url not yet implemented for other compilers
	if (tt_local_ini_file == NULL) {
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		string url = system_cached_url(full_file_name);
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		if (url != NULL) {
			compute_local_ini_file_name(url);
			if (tt_local_ini_file != NULL) {
				remove_extension_from_file_name(url); // about to be clobbered anyway
				tt_local_ini_file_uncached_name = append_strings(url,".ini"); // new on 020301
			};
			delete [] url;
		};
	} else {
		tt_local_ini_file_uncached_name = copy_string(tt_local_ini_file);
	};
#endif
#if TT_DEBUG_ON
	if (tt_debug_mode == 110101) {
		output_file_stream temp;
		temp.open("c:\\pre_init.txt",std::ios_base::out); // used to be prior to 070502 \\temp but doesn't always exist
		temp << "Loading " << full_file_name;
		if (tt_local_ini_file == NULL) {
			temp << " but can't find a local ini file." << endl;
		} else {
			temp << " with local ini file: " << tt_local_ini_file << endl;
		};
		temp.close();
	};
#endif
};

boolean interesting_error(int last_error) { // made a procedure on 140301
	// not file not found and not overlapped I/O in progress and not can't create existing file and not timeout
	// so worth reporting in log file
	boolean interesting = (last_error != 0 && last_error != 2 && last_error != 997 && last_error != 183 && last_error != 1460  
								  && last_error != 18 && last_error != 53
								  && last_error != 1008); // 1008 is bad token but SHGetFolderPath triggers with NULL token despite documentation
	if (interesting && about_to_quit()) {
		return(last_error != 1813 && last_error != 1400); // and more to be added
	};
	return(interesting);
};

int previous_last_error = -1;

#if TT_DEBUG_ON
boolean add_to_log_if_interesting_error(int ignore_error, string message, int ignore_error_also) {
   //if (tt_debug_mode == 130502 && message != NULL) {
   //   tt_error_file() << message << " at time " << timeGetTime() << " and frame " << tt_frame_number << endl;
   //};
	int last_error = GetLastError();
	if (last_error == previous_last_error) return(FALSE);
	if (currently_paused()) return(FALSE); // new on 080202
	boolean result = FALSE; // rewritten 141004 so returns TRUE only if really interesting
	if (interesting_error(last_error)) {
		if (last_error != ignore_error && last_error != ignore_error_also) {
			// re-arranged on 100904 so if ignore_error then still set previous_last_error
			//if (tt_exclusive_screen && tt_debug_mode == 130502) { // with this can put breakpoint after this
			//   toggle_pause(TRUE,FALSE,FALSE);
			//};
			debug_this();
			tt_error_file() << "Last error number=" << last_error;
			if (message != NULL) {
				tt_error_file() << " " << message;
			};
			tt_error_file() << " at time " << timeGetTime() << " and frame " << tt_frame_number << endl;
			result = TRUE;
		};
		previous_last_error = last_error;
	};
	//if (last_error != 0) {
	//	previous_last_error = last_error;
	//};
	return(result);
};

void set_previous_last_error(int n) {
	previous_last_error = n;
};

#endif

DWORD compute_memory_available(boolean just_virtual_memory) { // abstracted on 010401
	MEMORYSTATUS memory_status;
   memory_status.dwLength = sizeof(MEMORYSTATUS);
   GlobalMemoryStatus(&memory_status);
	// prior to 010401 this used dwAvailVirtual rather than dwAvailPageFile
#if TT_DEBUG_ON
   if (tt_debug_mode == 8765) {
      tt_error_file() << "Physical memory = " << memory_status.dwTotalPhys << endl;
      tt_error_file() << "Physical memory available = " << memory_status.dwAvailPhys << endl;
      tt_error_file() << "Virtual memory = " << memory_status.dwTotalVirtual << endl;
      tt_error_file() << "Virtual memory available = " << memory_status.dwAvailPageFile << endl;
      tt_error_file() << "GetFreeSpace(0) = " << GetFreeSpace(0) << endl;
   };
#endif
	if (just_virtual_memory) {
		return(memory_status.dwAvailPageFile);
	} else {
		// should be more clever but physical memory is better basis since this is used to determine cache size
		return(min(memory_status.dwTotalPhys,memory_status.dwAvailPageFile));
	};
};

void show_puzzles_over_html() { // modularized on 180700
	// moved here on 050401 from Programmer_Walking::
#if TT_DEBUG_ON
	tt_error_file() << "About to show " << tt_puzzles_over_html_file << endl;
#endif
	set_user_wants_to_quit(TRUE); // well maybe not but has no choice -- moved here from end of this method on 050401 so it shuts down more cleanly
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	pause_toontalk(FALSE,FALSE,TRUE); // new on 050401
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
//		ascii_string main_dir = ini_entry(AC(IDC_DIRECTORIES),AC(IDS_MAIN_DIR),FALSE);
	// added get_handle() on 050401
	HINSTANCE result = ShellExecuteG(tt_main_window->get_handle(),"open",tt_puzzles_over_html_file,"",tt_main_directory,SW_SHOW); 
	// main_dir new on 200400 - renamed to tt_main_directory on 300500
#if TT_DEBUG_ON
	if ((int) result <= 32) {
		tt_error_file() << "ShellExecute returned code = " << (int) result << endl;
	};
	add_to_log_if_interesting_error(126); // don't know why 126 is triggered
#endif
//	if (main_dir != NULL)  delete [] main_dir;
  	delete [] tt_puzzles_over_html_file;
	tt_puzzles_over_html_file = NULL;
	tt_trouble_shooting = FALSE; // new on 050401
};

color compute_color(int index, bytes permutation) { // new on 011001
	if (tt_bits_per_pixel == 8) {
		if (permutation == NULL) {
			return(tt_colors[index]);
		} else {
			return(tt_colors[permutation[index]]);
		};
	} else {
		color color = tt_colors[index];
		if (permutation == NULL) {
			return(color);
		} else {
			return(alter_color24(color,(int *) permutation));	// added 24 on 100502 since color is always a COLORREF here		
		};
	};
};

// made obsolete on 100502
//color alter_color(color color, int *changes) {
//   switch (tt_bits_per_pixel) {
//       case 24:
//         return(alter_color24(color,changes));
//      case 32:
//         return(alter_color32(color,changes));
//      case 16: // added on 110402 - why was it missing?
//         return(alter_color16(color_to_color16(color),changes));
//      default:
//         log("Internal problem. Shouldn't be calling alter_color when color depth is other than 16, 24, or 32.");
//         return(NULL);
//   };
//};

//void compute_new_rgb(color color, int *changes, int &red, int &green, int &blue) { // restructured on 260302
//};

color alter_color24(color color, int *changes) { // new version on 021001 
	// change now has 4 ints: change to all, change to red, change to green, and change to blue
   int red = GetRValue(color);
	int green = GetGValue(color);
	int blue = GetBValue(color);
#if TT_DEBUG_ON
   if (tt_debug_mode == 90503) {
      tt_error_file() << "Color in:  " << red << " " << green << " " << blue << endl;
   };
#endif
	int change_all = changes[all_index]; 
	if (red < 2 && green < 2 && blue < 2) { // treat black (and near black) specially
		if (change_all != 0) {
			red = 64;
			green = 64;
			blue = 64;
		} else {
			if (changes[red_index] != 0) {
				red = 64;
			};
			if (changes[green_index] != 0) {
				green = 64;
			};
			if (changes[blue_index] != 0) {
				blue = 64;
			};
		};
	} else {
		red += changes[red_index]+change_all;
		green += changes[green_index]+change_all;
		blue += changes[blue_index]+change_all;
	};
#if TT_DEBUG_ON
   if (tt_debug_mode == 90503) {
      tt_error_file() << "Color out: " << red << " " << green << " " << blue << endl;
   };
#endif
	return(RGB(max(1,min(255,red)),max(1,min(255,green)),max(1,min(255,blue)))); // 1 rather than 0 since don't want transparent
};

/* commented out on 310702 since not called
// confusing to use the color datatype here since is a 32-bit ARGB encoding
color alter_color32(color color, int *changes) { // new on 260302 -- earlier 24-bit and 32-bit versions shared the same code (wrong for 32-bit)
	// change now has 4 ints: change to all, change to red, change to green, and change to blue
   // color in and out is ARGB
   int red = (byte) (color>>16);
	int green = (byte) (color>>8);
	int blue = (byte) (color);
	int change_all = changes[all_index]; 
	if (red < 2 && green < 2 && blue < 2) { // treat black (and near black) specially
      // this is very general -- doesn't do the right thing for mismatch feedback
		if (change_all != 0) {
			red = 64;
			green = 64;
			blue = 64;
		} else {
			if (changes[red_index] != 0) {
				red = 64;
			};
			if (changes[green_index] != 0) {
				green = 64;
			};
			if (changes[blue_index] != 0) {
				blue = 64;
			};
		};
	} else {
		red += changes[red_index]+change_all;
		green += changes[green_index]+change_all;
		blue += changes[blue_index]+change_all;
	};
   // byte 0 is alpha - leave alone or would it be neat to use it?
   // 1 rather than 0 since don't want transparent
   return(Color::MakeARGB(0xFF,max(1,min(255,red)),max(1,min(255,green)),max(1,min(255,blue))));
};
*/
void set_absolute_mouse_mode(int mode) {
   switch (mode) {
      case 1: // new meaning on 030502
         tt_mouse_mode_on_floor = ABSOLUTE_MOUSE_MODE;
         tt_mouse_mode_not_on_floor = ABSOLUTE_MOUSE_MODE;
         break;
      case 2:
         tt_mouse_mode_on_floor = ABSOLUTE_MOUSE_MODE;
         tt_mouse_mode_not_on_floor = RELATIVE_MOUSE_MODE;
         break;
      case 3:
         tt_mouse_mode_on_floor = SIMULATED_DIRECTIONAL_PAD_MOUSE_MODE;
         tt_mouse_mode_not_on_floor = SIMULATED_DIRECTIONAL_PAD_MOUSE_MODE;
         break;
      case 4: // old meaning of 1 (prior to 030502)
         tt_mouse_mode_on_floor = ABSOLUTE_MOUSE_MODE;
         tt_mouse_mode_not_on_floor = SIMULATED_DIRECTIONAL_PAD_MOUSE_MODE;
         break;
 //     case 0:
      default:             
         tt_mouse_mode_on_floor = RELATIVE_MOUSE_MODE;
         tt_mouse_mode_not_on_floor = RELATIVE_MOUSE_MODE;
         break;
   };
   tt_mouse_mode = tt_mouse_mode_not_on_floor; // initialize it so set even in opening tiles - new on 160502
};

unsigned char name_to_character(string encoding, boolean &extended_key) { // abstracted on 070502 
   // encoding can be things like <Escape> or <F11>
   if (encoding == NULL) return(0);
   extended_key = FALSE; // unless found otherwise below
   int length = strlen(encoding);
   if (length == 0) return(0);
   if (encoding[0] == '<' && length > 3) {
      if (encoding[1] == 'f' || encoding[1] == 'F') { // function key
         int j = 2;
         while (encoding[j] != 0) {
            if (encoding[j] < '0' || encoding[j] > '9') {
               encoding[j] = 0; // terminate it
               break;
            };
            j++;
         };
         int f_number = atoi(encoding+2)-1;
         extended_key = TRUE;
         return((unsigned char) (VK_F1+f_number));
      } else if (stricmp(encoding,"<Escape>") == 0) {
         // not extended key?
         return((unsigned char) VK_ESCAPE);
      // Won't work since has no affect here -- must be caught earlier in WinMain
      //} else if (stricmp(substitution,"<Pause>") == 0) {
         //   key_substitute[i] = (unsigned char) VK_PAUSE;
         //   key_substitute_extended_key |= (1<<i);
      } else if (stricmp(encoding,"<space>") == 0) {
         return(' ');
      } else if (stricmp(encoding,"<return>") == 0) { // new on 070502
         return('\r'); // or should it be \n?? asked on 090802
      } else if (stricmp(encoding,"<tab>") == 0) { // new on 070502
         return('\t');
      } else { // any more??
         return(0); // warn?
      }; 
   } else { // ordinary character
      return(encoding[0]);
   };
};

/*
int get_red_from_color15(color16 color) {
	return(((color>>10))<<3);
};

int get_green_from_color15(color16 color) {
	return(((color&0x3E0)>>5)<<2);
};

int get_blue_from_color15(color16 color) {
	return((color&0x1F)<<3);
};

color16 make_color15(int red, int green, int blue) {
	// args range from 0 to 255
	return((((red>>3)&0x1F)<<10)|
		    (((green>>3)&0x1F)<<5)|
			 ((blue>>3)&0x1F));
};

color16 alter_color15(color16 color, int *changes) { // new 061101 to support 16-bit color
	// change now has 4 ints: change to all, change to red, change to green, and change to blue
	// assumes PixelFormat16bppRGB555 or PixelFormat16bppARGB1555
	int red = get_red_from_color15(color);
	int green = get_green_from_color15(color);
	int blue = get_blue_from_color15(color);
	int change_all = changes[all_index]; 
	if (red < 2 && green < 2 && blue < 2) { // treat black (and near black) specially
		if (change_all != 0) {
			red = 64;
			green = 64;
			blue = 64;
		} else {
			if (changes[red_index] != 0) {
				red = 64;
			};
			if (changes[green_index] != 0) {
				green = 64;
			};
			if (changes[blue_index] != 0) {
				blue = 64;
			};
		};
	} else {
		red += changes[red_index]+change_all;
		green += changes[green_index]+change_all;
		blue += changes[blue_index]+change_all;
	};
	return(make_color15(max(1,min(255,red)),max(1,min(255,green)),max(1,min(255,blue)))); // 1 rather than 0 since don't want transparent
};
*/

color16 color_to_color16(color color) {
#if TT_DEBUG_ON
   if (tt_debug_mode == 90503) {
      tt_error_file() << "Colorto16: " << GetRValue(color) << " " << GetGValue(color) << " " << GetBValue(color) << endl;
   };
#endif
   return(make_color16(GetRValue(color),GetGValue(color),GetBValue(color)));
};

color16 near_black16() {
	if (tt_green_bits_per_pixel == 6) {
		return(0x821); // should be 0000100000100001
	} else {
		return(0x421); // should be 000010000100001
	}
};

color16 alter_color16(color16 color, int *changes) { // new 061101 to support 16-bit color
	// change now has 4 ints: change to all, change to red, change to green, and change to blue
	// assumes PixelFormat16bppRGB565
	int red = get_red_from_color16(color);
	int green = get_green_from_color16(color);
	int blue = get_blue_from_color16(color);
#if TT_DEBUG_ON
   if (tt_debug_mode == 90503) {
      tt_error_file() << "Color in:  " << red << " " << green << " " << blue << endl;
   };
#endif
	int change_all = changes[all_index]; 
	if (red < 2 && green < 2 && blue < 2) { // treat black (and near black) specially
		if (change_all != 0) {
			red = 64;
			green = 64;
			blue = 64;
		} else {
			if (changes[red_index] != 0) {
				red = 64;
			};
			if (changes[green_index] != 0) {
				green = 64;
			};
			if (changes[blue_index] != 0) {
				blue = 64;
			};
		};
	} else {
		red += changes[red_index]+change_all;
		green += changes[green_index]+change_all;
		blue += changes[blue_index]+change_all;
	};
#if TT_DEBUG_ON
   if (tt_debug_mode == 90503) {
      tt_error_file() << "Color out: " << red << " " << green << " " << blue << endl;
   };
#endif
	return(make_color16(max(8,min(255,red)),max(8,min(255,green)),max(8,min(255,blue)))); 
	// 8 rather than 0 since don't want transparent (was 1 prior to 291101)
};

string picture_file_name_of_ini_file(ascii_string ini_file_name, string entry, string full_entry) { // re-modularized on 071201
	if (GetPrivateProfileString("Files","Picture","",entry,MAX_PATH,ini_file_name) != 0) {
		if (is_relative_path(entry)) { // new on 270901 - use path from full_file_name
			int start_of_basic_name = start_of_basic_file_name(ini_file_name);
			if (start_of_basic_name < 0) { // not clear how this can happen
				tt_error_file() << "Couldn't separate the directory and basic file name parts of " << ini_file_name << endl;
				return(NULL);
			};
			memcpy(full_entry,ini_file_name,start_of_basic_name);
			strcpy(full_entry+start_of_basic_name,entry);
			return(full_entry);
		} else {
			return(entry);
		};
	};
	return(NULL);
};

void print_integer_with_commas(long value, output_stream &text_out) { // moved here from number.cpp on 110102
	character integer_string[maximum_integer_text_size]; // was 15 prior to 030202
	output_string_stream temp_out(integer_string,maximum_integer_text_size);
	temp_out << value;
	int length = temp_out.LENGTH;
	if (length < 4) {  // no comma at all
		text_out.write((string) integer_string,length);
		return;
	};
   const_string comma;
   if (current_language() == SWEDISH || current_language() == BRITISH) {
      comma = _T("");
   } else {
      comma = S(IDS_THOUSANDS_COMMA,""); // "" added on 050999
   };
	int next_comma = length%3;
	if (next_comma == 0) next_comma += 3;
	if (value < 0 && next_comma == 1) next_comma += 3;
	for (int i = 0; i < length; i++) {
		if (i == next_comma) {
			text_out << comma;
			next_comma += 3;
		};
		text_out.put(integer_string[i]);
	};
};

int digit_to_number(unsigned char key, int base) { // new on 270202
	if (key >= '0' && key <= '0'+min(9,base-1)) {
		return(key-'0');
	} else if (base > 10 && key >= 'a' && key < 'a'+base-10) {
		return(key-'a'+10);
	} else if (base > 10 && key >= 'A' && key < 'A'+base-10) {
		return(key-'A'+10);
	};
	return(-1); // isn't a valid digit
};

boolean all_characters_are(char character, string text) {
	int length = strlen(text);
	for (int i = 0; i < length; i++) {
		if (text[i] != character) return(FALSE);
	};
	return(TRUE);
};

unsigned char old_dispdib_value() { // new on 140502 to simulate the old tt_using_dispdib flag
   if (tt_window_expansion_factor > 1) {
      return(tt_window_expansion_factor);
   } else {
      return(exclusive_screen_desired());
   };
};

unsigned char current_dispdib_value() { 
   // new on 100602 to simulate the old tt_using_dispdib flag 
   // differs from the above in that it reflects current state
   // e.g. full-screen alt-tabbed is 0
   if (tt_window_expansion_factor > 1) {
      return(tt_window_expansion_factor);
   } else {
      return(tt_exclusive_screen);
   };
};

void set_old_dispdib_value(unsigned char value) {
   if (value > 1) {
      tt_window_expansion_factor = value;
      tt_exclusive_screen_desired = FALSE;
   } else {
      tt_exclusive_screen_desired = value;
   };
};

void interpret_command_string(string command_string) {
   ascii_string commands[32];
   int command_count;
   process_command_line(command_string,command_count,commands);
   interpret_command_line(command_count,commands);
   for (int i = 0; i < command_count; i++) {
      delete [] commands[i];
   };
};

ascii_string copy_arg(ascii_string arg1, ascii_string arg2) {
   while (arg1[0] == ' ') arg1++; // trim preceeding spaces
   if (arg1[0] == '"') {
      int i = 1;
      int j = 0;
      while (arg1[i] != '"' && arg1[i] != 0) {
         // this doesn't do the "right thing" if there are \" inside
         arg2[j++] = arg1[i++];
      };
      arg2[j] = NULL; // terminate it
      if (arg1[i] == '"') i++;
      return(arg1+i);
   } else {
      strcpy(arg2,arg1);
      return(NULL);
   };
};

ascii_string next_command(string command_line_remaining, boolean first_command) {
	// new on 060902 since the old code used strstr to find '-' which might be in a file name
	int size = strlen(command_line_remaining);
	boolean inside_a_quoted_string = FALSE;
	for (int i = 0; i < size-1; i++) {
		if (inside_a_quoted_string) {
			if (command_line_remaining[i] == '"') {
				inside_a_quoted_string = FALSE;
			};
		} else if (command_line_remaining[i] == '"') {
			inside_a_quoted_string = TRUE;
		} else if (command_line_remaining[i] == ' ' && command_line_remaining[i+1] == '-') {
			return(command_line_remaining+i+1);
		} else if (first_command && command_line_remaining[i] == '-' && i == 0) { // added  && i == 0 on 031202
			// if first command can be white space followed by -
			return(command_line_remaining+i);
		};
	};
	return(NULL);
};

NaturalLanguage process_command_line(string command_line, int &command_line_count, ascii_string commands[]) { 
   // abstracted into a procedure on 300502
   // returns NaturalLanguage since some callers care and others don't so easiest way to communicate
  	 int command_line_size = strlen(command_line);
//	 int command_line_count_start = 0;
    command_line_count = 0; // command_line_count_start;
	 char temp[MAX_PATH];
    boolean command_next = TRUE;
	 NaturalLanguage language_to_set = NO_LANGUAGE_SPECIFIED;
	 if (command_line_size > 0) {
// following had the problem that it broke apart Win95 file names with spaces in them
//		 istrstream command_line = istrstream(lpszCmdLine);
       ascii_string command_line_left = command_line;
       boolean language_next = FALSE;
		 boolean ini_file_next = FALSE; // new on 260601
		 boolean local_ini_file_next = FALSE; // new on 050404
		 boolean first_command = TRUE;
		 while (command_line_left != NULL && command_line_left[0] != NULL) {
//			 command_line >> temp;
          if (command_next) {
             ascii_string next_command_line_left = next_command(command_line_left,first_command); 
				 // was strstr(command_line_left,"-") prior to 060902
             first_command = FALSE;
				 if (next_command_line_left == NULL) { // whole thing should be an arg
                command_line_left = copy_arg(command_line_left,temp);
             } else if (next_command_line_left[0] == '"') { // quoted
                command_line_left = copy_arg(command_line_left,temp);
             } else {
                int i = 0;
                int j = 0;
                while (next_command_line_left[i] != ' ' && next_command_line_left[i] != NULL) { // second condition added 031202
                   temp[j++] = next_command_line_left[i++];
                };
                temp[j] = NULL; // terminate it
                command_line_left = next_command_line_left+j+1;
             };
             command_next = FALSE;
          } else {
             ascii_string next_command_line_left = strstr(command_line_left," -");
             if (next_command_line_left == NULL) { // last pair
                strcpy(temp,command_line_left);
                command_line_left = NULL;
             } else {
                int front_length = strlen(command_line_left)-strlen(next_command_line_left);
                memcpy(temp,command_line_left,front_length);
                temp[front_length] = NULL;
             };
             command_next = TRUE;
          };
			 int temp_length = strlen(temp);
//			 if (temp_length == 0) {
//				 break;
//			 };
          // need to check here so strings are loaded now
          if (language_next) {
				 language_to_set = string_to_language(temp,FALSE);
//		       set_language(language_to_set); // done later now that toontalk.ini might not be set
//             load_string_library();
             language_next = FALSE;
          };
			 if (ini_file_next) {
				 set_ini_file(copy_string_without_quotes(temp));
				 ini_file_next = FALSE;
			 };
			 if (local_ini_file_next) { // new on 050404
				 // note that this must come after -ini_file if both are used (why would one?)
				 tt_local_ini_file = copy_string_without_quotes(temp);
				 boolean relative = (strchr(temp,':') == NULL && temp[0] != '\\'); // new on 060404
				 if (relative) {
					 // append the directory of this EXE
					 string full_path = new char[MAX_PATH];
					 full_path[0] = '\0'; // just in case the following fails
					 GetModuleFileName(NULL,full_path,MAX_PATH);
					 int end_of_directory_path = short_file_name_start(full_path);
					 strcpy(full_path+end_of_directory_path,tt_local_ini_file);
					 delete [] tt_local_ini_file;
					 tt_local_ini_file = full_path;
				 };	
				 ini_file_next = FALSE;
			 };
          if (stricmp(temp,"-language") == 0) language_next = TRUE;
			 if (stricmp(temp,"-ini_file") == 0) ini_file_next = TRUE; // new on 260601
			 if (stricmp(temp,"-local_ini_file") == 0) local_ini_file_next = TRUE;
			 commands[command_line_count] = new char[temp_length+1];
//			 strcpy(commands[command_line_count],temp);
          boolean start = TRUE;
          int j = 0;
          for (int i = 0; i < temp_length; i++) {
             if (!start || temp[i] != ' ') {  // trim preceeding spaces
                commands[command_line_count][j++] = temp[i];
                start = FALSE;
             };
          };
          commands[command_line_count][j] = NULL; // terminate string
          while (commands[command_line_count][j-1] == ' ') { // trim trailing spaces
             commands[command_line_count][j-1] = NULL;
             j--;
          };
			 command_line_count++;
			 if (command_line_count == 32) break;
		 };
	 };
    return(language_to_set);
};

void compute_tt_alternative_spoken_puzzle_file_name(string short_name) { // moved here on 011100 to be shared
	if (alternative_language_spoken()) {
		tt_use_spoken_alternative = TRUE;
		FileNameStatus file_status;
		tt_alternative_spoken_puzzle_file_name = existing_file_name(file_status,short_name,"pzl","Puzzles",TRUE,TRUE,TRUE);
		tt_use_spoken_alternative = FALSE;
	};
};

void interpret_command_line(int argc, char *argv[]) { // , boolean ) { -- moved here on 300502 from main.cpp
  boolean just_one, asked_name_already; 
  boolean problem_with_command_line = FALSE;
#if TT_SMALL_TRIAL
	boolean next_puzzle = TRUE;
#else
  boolean next_puzzle = FALSE;
#endif
//  boolean speaking_set = FALSE;
#if TT_32
  if (argc%2 == 1) { 
	  just_one = (argc == 1);
#else
//  just_one = FALSE;
  if (argc%2 == 0) {
	  just_one = (argc == 2);
#endif
	  if (just_one) {
		  ascii_string file_or_name = argv[argc-1];
		  int length = strlen(file_or_name);
		  argc = 0; // skip while loop
		  if (length > 5 &&  // see if it ends in .USR because user clicked on profile file
				strcmpi(file_or_name+length-4,".usr") == 0) {
			  // is really a user name file so extract it from full path
			  ascii_string name = new char[MAX_PATH];
			  int name_index = MAX_PATH-1;
			  name[name_index--] = 0; // terminate it and build it from back
			  for (int i = length-5; i >= 0; i--) {
				  if (file_or_name[i] == '\\' || file_or_name[i] == ':') {
					  set_tt_user_name(copy_string(a2u(name+name_index+1)));
					  break;
				  } else {
					  name[name_index--] = file_or_name[i];
				  };
			  };
			  delete [] name;
        } else if (length > 5 &&  // see if it ends in .PZL because user clicked on puzzle file
				       strcmpi(file_or_name+length-4,".pzl") == 0) {
			  compute_local_ini_file_name(file_or_name); // new on 110101
			  tt_puzzle_file_name = copy_ascii_string(file_or_name);
			  string short_name = short_file_name(file_or_name);
			  compute_tt_alternative_spoken_puzzle_file_name(short_name); // new on 011100
			  delete [] short_name;
           tt_system_mode = PUZZLE;
        } else if (length > 5 &&  // see if it ends in .CTY because user clicked on city file
				       strcmpi(file_or_name+length-4,".cty") == 0) {
			  if (!tt_ini_file_only_from_tt_ini_file) {
					 compute_local_ini_file_name(file_or_name); // new on 110101
			       tt_local_ini_file_loaded_with_city = copy_string(tt_local_ini_file); // new on 040301
			  };
			  tt_city_name = copy_ascii_string(file_or_name);
		  } else if (length > 5 &&  // see if it ends in .DMO because user clicked on demo file
				       strcmpi(file_or_name+length-4,".dmo") == 0) { // new on 271199 - was default before
			  compute_local_ini_file_name(file_or_name); // new on 110101
			  set_replay(REPLAY_REPRODUCE_TIMING);
 			  set_replay_file_name(file_or_name); // new on 271199 to delay this
			  asked_name_already = TRUE; 
			  // a lie added on 070201 but this prevents name from being asked for demos when it shouldn't
//			  if (open_log(file_or_name,pre_tt_err_file,TRUE)) {
		  } else if (length > 5 &&  // see if it ends in .TT because user clicked on a "ToonTalk" file
				       strcmpi(file_or_name+length-3,".tt") == 0) {
			  compute_local_ini_file_name(file_or_name); // new on 110101
			  tt_file_name_to_clipboard = copy_ascii_string(file_or_name); 
			  tt_file_name_clipboard_message = copy_string(tt_file_name_to_clipboard); 
			  // new on 220304 so demos that start with a double click work
			  // rewritten on 181203 so time travel will work if started by double clicking on a TT file
			  //::WritePrivateProfileString("User","TTFileLastDoubleClicked",file_or_name,tt_ini_file);
			  //::PostMessage(tt_main_window->get_handle(),WM_USER+FILE_FOR_CLIPBOARD_MESSAGE_ID,0,0);
			} else { // assume it was the user's name
//					if (!replaying()) // commented out on 221103 so Marty doesn't greet one as NoName when replaying demos		 
#if TT_UNICODE
				  set_tt_user_name(file_or_name);
#else
				  set_tt_user_name(copy_string(file_or_name));
#endif
//					  name_given = TRUE;
		  };
	  } else {
		 // ignore but warn (includes filename as first one)
		 tt_error_file() << S(IDS_COMMAND_LINE_ODD_ARGS1) 
			 // was pre_tt_err_file prior to 300502 but that is in main.cpp and this is called during initialization and later
						 << (argc-1) << S(IDS_COMMAND_LINE_ODD_ARGS2) << endl;
		 argc = 0; // to skip while loop below
		 problem_with_command_line = TRUE;
	  };
  };
#if TT_32
  int i = 0;
#else
  int i = 1;
#endif
  if (tt_user_name == NULL) { // check if name will be specified
     boolean name_will_be_given = FALSE;  // until proven otherwise
     while (i < argc) {
       if (stricmp(argv[i],"-n") == 0 ||
//			  stricmp(argv[i],"-i") == 0 || // commented out on 050204 since the following is case insensitive
			  stricmp(argv[i],"-I") == 0) {
          name_will_be_given = TRUE;
			 asked_name_already = TRUE; // a lie added on 040300 but this prevents name from being asked for demos when it shouldn't
          break;
       } else if ((stricmp(argv[i],"-next_puzzle") == 0 && i < argc && 
			 // for robustness - e.g. -next_puzzle is end of the command line
						atoi(argv[i+1]) != 0) || stricmp(argv[i],"-puzzle") == 0) {
			 tt_system_mode = PUZZLE; // new on 250903 so ask_name knows how to behave
		 } else if (stricmp(argv[i],"-time_travel_enabled") == 0 && i < argc && atoi(argv[i+1]) == 0) { // new on 011003
			 tt_time_travel_enabled = FALSE;
		 } else if (stricmp(argv[i],"-must_be_new_user") == 0 && i < argc) { 
			 // new on 030304 -- need it now for the compute_tt_file_name() call below
			 tt_user_name_should_be_new = atoi(argv[i+1]);
		 };
       i += 2;
     };
     if (!name_will_be_given && !replaying()) { // ask now so subsequent commands can override
		  // tt_replay added on 171299 so name isn't asked if double click on demo
        ask_what_name();
        asked_name_already = TRUE;
        compute_tt_file_name();
     };
  };
#if TT_32
  i = 0;
#else
  i = 1;
#endif
  int specified_video_mode = 0;
  boolean tt_subtitles_set = FALSE;
//  boolean speak_option_set = FALSE;
//  boolean balloon_option_set = FALSE;
  while (i < argc) {
	  ascii_string option = argv[i];
	  ascii_string arg = argv[i+1];
	  if (stricmp(option,"-v") == 0 || stricmp(option,"-screen_width") == 0) { // second disjunct new on 060802
        tt_video_mode = atoi(arg);
		  switch (tt_video_mode) {
			  case 22: // old name
				  tt_video_mode = 320;
				  break;
			  case 25:
				  tt_video_mode = 640;
              break;
              // commented the following on 300702 so can run at any screen resolution
			  //case 320: // added on 050401 since when running the same guy twice the resolution shot up to 640
			  //case 640:
			  //case 800:
			  //case 1024:
			  //case 1152:
			  //case 1280:
			  //case 1600:
				 // break;
//			  default: 
				  //tt_error_file() << S(IDS_BAD_VIDEO_MODE1) << space() << tt_video_mode 
				  // was pre_tt_err_file prior to 300502 but that is in main.cpp and this is called during initialization and later
						//		  << space() << S(IDS_BAD_VIDEO_MODE2);
				  //tt_video_mode = 640; // was 320 pror to 060301
				  //problem_with_command_line = TRUE;
		  };
        specified_video_mode = tt_video_mode;
	  } else if (stricmp(option, "-d") == 0) {
		  tt_debug_mode = atoi(arg);
		  tt_create_new_notebooks |= (tt_debug_mode == 2468 || tt_debug_mode == 2469); // changed to |= so this doesn't turn it off
#if TT_DEBUG_ON
	  } else if (stricmp(option, "-debug_target") == 0) {
		  tt_debug_target = atoi(arg);
//		  tt_debug_mode = 10000;
	  } else if (stricmp(option, "-debug_frame") == 0) {
		  tt_debug_frame_number = atoi(arg);
	  } else if (stricmp(option, "-debug_guid") == 0) { 
		  tt_debug_guid = copy_wide_string(arg);
	  } else if (stricmp(option, "-debug_text") == 0) {
		  tt_debug_text_pad = copy_string(arg);
	  } else if (stricmp(option,"-debug_counter") == 0) {
		  tt_counter_for_debugging = atoi(arg);
#endif
#if TT_DIRECT_PLAY
	  } else if (stricmp(option,"-debug_network") == 0) { // new on 030500
		  tt_debug_direct_play = atoi(arg); 
	  } else if (stricmp(option,"-long_distance_birds") == 0) { // new on 030500
		  tt_running_direct_play = atoi(arg); 
#endif
	  } else if (stricmp(option,"-use_directx_transform") == 0) { // new on 260500
		  tt_prefer_gdiplus = !(atoi(arg));
//		  tt_using_directx_transform_for_BMPs = atoi(arg); 
	  } else if (stricmp(option, "-virgin_notebook") == 0) {
		  tt_logs_contain_virgin_notebook = (flag) atoi(arg);
	  } else if (stricmp(option, "-s") == 0) {
		  tt_sound_option = (char) atoi(arg);
	  } else if (stricmp(option, "-f") == 0) {
//		  tt_using_dispdib = (char) atoi(arg);
        set_old_dispdib_value(atoi(arg));
#if !TT_SMALL_TRIAL
	  } else if (stricmp(option, "-h") == 0) { 
		  // 0 is nothing; 1 is hair; 2 is hat
//		  tt_programmer_head = (char) atoi(arg);
		  set_programmer_head(atoi(arg)); // rewritten on 180603
		  if (tt_programmer_head < 0 || tt_programmer_head > 2) {
			  tt_error_file() << S(IDS_COMMAND_LINE_H_WRONG1) << tt_programmer_head 
				  // was pre_tt_err_file prior to 300502 but that is in main.cpp and this is called during initialization and later
									<< S(IDS_COMMAND_LINE_H_WRONG2); 
			  tt_programmer_head = 0;
			  problem_with_command_line = TRUE;
		  };
#endif
	  } else if (stricmp(option, "-n") == 0 && arg[0] != NULL) { // second conjunct added on 160300
		  // note that this can be a bit odd when NoName is the user name and the noname.usr contains the last demo user name
#if TT_UNICODE
		  boolean new_name = set_tt_user_name(a2u(arg));
#else
		  boolean new_name = set_tt_user_name(copy_string_without_quotes(arg)); // was just copy_string prior to 260601
#endif
//		  name_given = TRUE;
		  if (new_name) compute_tt_file_name();
	  } else if (stricmp(option, "-c") == 0) {
#if TT_LARGE_CITIES
		  string second_part = strchr(arg,'x');
		  if (second_part != NULL) {
			  second_part[0] = '\0';
			  tt_city_width = atoi(arg);
			  tt_city_height = atoi(second_part+1);
			  second_part[0] = 'x'; // restore just in case might be needed
		  } else {
			  tt_city_width = atoi(arg);
			  tt_city_height = tt_city_width;
		  };
#else
		  tt_city_size = (short int) atoi(arg); // number of streets (10 is default)
        if (tt_city_size < -255) {
           tt_city_size = -255;
        } else if (tt_city_size > 255) {
           tt_city_size = 255;
        };
#endif
	  } else if (stricmp(option, "-houses") == 0) {
		  set_houses_to_a_block(atoi(arg));
	  } else if (stricmp(option, "-l") == 0) { 
		  // bigger for bigger letters
		  tt_character_size_percent = (unsigned short int) atoi(arg);
	  } else if (stricmp(option, "-font_variable") == 0) { // font name
		  tt_variable_font_name = copy_string_without_quotes(arg); // was just copy_string prior to 260601
		  replace_character(tt_variable_font_name,'_',' '); // replace underbars with spaces
	  } else if (stricmp(option, "-font_fixed") == 0) { // font name
		  tt_fixed_font_name = copy_string_without_quotes(arg); // was just copy_string prior to 260601
		  replace_character(tt_fixed_font_name,'_',' '); // replace underbars with spaces
	  } else if (stricmp(option, "-r") == 0) { 
		  tt_help_speed_percent = (unsigned short int) atoi(arg);
	  } else if (stricmp(option, "-m") == 0) {
		  tt_cache_percent = (unsigned short int) atoi(arg);
	  } else if (stricmp(option, "-p") == 0) { 	  
		  tt_serial_port = (char) atoi(arg);
	  } else if (stricmp(option, "-b") == 0) {
		  int count = atoi(arg);
		  if (count < 0 || count > 3) { // was < 1
			  tt_error_file() <<  "-b " << count << S(IDS_COMMAND_LINE_B_WRONG); 
			  // was pre_tt_err_file prior to 300502 but that is in main.cpp and this is called during initialization and later
		  } else {
			  tt_mouse_button_count	= (char) count;
		  };
	  } else if (stricmp(option, "-j") == 0) {
		  tt_win31j = (flag) atoi(arg);
	  } else if (stricmp(option, "-u") == 0) {
		  tt_maintain_history = (flag) atoi(arg);
	  } else if (stricmp(option, "-q") == 0) {
		  tt_speed = (unsigned short int) atoi(arg);
		  if (tt_speed == 0) tt_speed = 100; // warn??
	  } else if (stricmp(option, "-y") == 0) {
		  tt_sleep = (unsigned short int) atoi(arg);
	  } else if (stricmp(option, "-a") == 0) {
		  tt_minimum_say_importance = (char) atoi(arg);
		  if (replay_file_name() == NULL) { 
			  // condition new on 310100 so that when reading a log will leave this alone (since saved in logs now)
			  tt_martian_automatically_appears = (tt_minimum_say_importance <= 2);
		  };
	  } else if (stricmp(option, "-k") == 0) {
		  tt_user_is_debugger = (flag) atoi(arg);
	  } else if (stricmp(option, "-z") == 0) { // just for experimentation
		  tt_copy_top_of_stack = (flag) atoi(arg);
	  } else if (stricmp(option, "-t") == 0) {
#if !TT_NO_ROBOT_TRAINING
		  tt_temporary_user = (flag) atoi(arg);
		  if (tt_temporary_user) {
			  tt_maintain_history = FALSE;
		  };
		  // otherwise leave it TRUE
#endif
	  // these are at the end because logs record parameters
#if !TT_SMALL_TRIAL || TT_DEBUG_ON
	  } else if (strcmp(option,"-i") == 0) { 
		  if (set_replay_file_name(arg)) { // new on 271199 to delay this
			  // following moved into this conditional so doesn't crash if file is missing
			  set_replay(REPLAY_IGNORE_TIMING);
			  compute_local_ini_file_name(arg);  // copied here on 070201
		  } else { // new on 140404
			  tt_error_file() << "Could not open the demo named " << arg << endl;
			  // following new on 230804
			  ask_what_name();
			  asked_name_already = TRUE;
			  compute_tt_file_name();
		  };
//		  if (open_log(arg,pre_tt_err_file)) {
//		  } else {
//			  problem_with_command_line = TRUE;
//		  };			  		  
	  } else if (strcmp(option,"-I") == 0) {
		  if (set_replay_file_name(arg)) { // new on 271199 to delay this
			  set_replay(REPLAY_REPRODUCE_TIMING);
			  compute_local_ini_file_name(arg);  // copied here on 070201
		  } else { // new on 140404
			  tt_error_file() << "Could not open the demo named " << arg << endl;
			  // following new on 230804
			  ask_what_name();
			  asked_name_already = TRUE;
			  compute_tt_file_name();
		  };
//		  if (!open_log(arg,pre_tt_err_file)) problem_with_command_line = TRUE;
//	  } else if (strcmp(option,"-Segment") == 0) { 
		  // still buggy -- 040204 -- removed on 220204 since don't need -Segment and -segment anymore
////		  tt_replay_after_titles = REPLAY_REPRODUCE_TIMING; // commented out on 220204
////		  set_replay_file_name(file_name_renumbered("log00000.dmo",atoi(arg))); // updated on 040204 but still broken
//		  tt_starting_log_segment = atoi(arg); // updated 040204
////		  asked_name_already = TRUE; // a lie added on 050204
#endif
	  } else if (stricmp(option,"-segment") == 0) { 
		  // moved out of TT_DEBUG_ON on 220204 since seems to work fine but yet to document (if ever)
//		  tt_replay_after_titles = REPLAY_IGNORE_TIMING;  // commented out on 220204
//		  set_replay_file_name(file_name_renumbered("log00000.dmo",atoi(arg)));
		  tt_starting_log_segment = atoi(arg); // updated 040204
//		  asked_name_already = TRUE; // a lie added on 050204
	  } else if (stricmp(option,"-o") == 0) {
        tt_log_out_file_name_specified = copy_string_without_quotes(arg); 
		  // updated 260601 -- changed on 170204 to be a different variable
		  if (tt_time_travel_enabled) {
			  // new on 100704 since if explictly creating an old demo don't want time travel on
			  tt_time_travel_enabled = FALSE; 
			  tt_error_file() << "Time travel turned off since the -o " << tt_log_out_file_name_specified 
									<< " command line option was used." << endl;
		  };
//		  tt_log_out_file_name_generated = FALSE; // commented out on 170204
     } else if (stricmp(option,"-bird") == 0) {
		  tt_show_birds_return = (flag) atoi(arg);
//     } else if (stricmp(option,"-bomb") == 0) { // commented out on 270100 since is obsolete
//		  tt_bomb_to_reset_puzzle = (flag) atoi(arg);
//     } else if (stricmp(option,"-new_input") == 0) { // commented out on 080100 since is obsolete
//		  tt_new_input_scheme = (flag) atoi(arg);
// need to catch this earlier to load the right library earlier        
     } else if (stricmp(option,"-language") == 0) {
//		  tt_language = string_to_language(arg); 
	  } else if (stricmp(option,"-spoken_language") == 0) { // new on 020999
		  set_spoken_language_from_string(arg);
     } else if (stricmp(option,"-puzzle") == 0) {
		  tt_system_mode = PUZZLE;
		  next_puzzle = FALSE; // since specified below
        int puzzle_number = atoi(arg);
        if (puzzle_number == 0) { // just the name
//           tt_puzzle_file_name = copy_ascii_string(arg);
			  FileNameStatus file_status; // new on 120999
           string file_name = copy_string_without_quotes(arg); // new on 220702 to remove surrounding quotes
			  // if URL don't cache now but just before reading file
           tt_puzzle_file_name = existing_file_name(file_status,file_name,"pzl","Puzzles",FALSE,TRUE,TRUE);
			  tt_user_puzzles = TRUE; // added on 300399
			  // should I check that it isn't an explicit mention of a system puzzle?
           delete [] file_name;
        } else if (puzzle_number > 0) {          
           char short_name[10] = "p";
           strcat(short_name,arg);
			  FileNameStatus file_status;
			  // if URL don't cache now but just before reading file
           tt_puzzle_file_name = existing_file_name(file_status,short_name,"pzl","Puzzles",FALSE,TRUE,TRUE);
			  compute_tt_alternative_spoken_puzzle_file_name(short_name);
        } else if (tt_puzzle_file_name != NULL) { // new on 300399
			  // update it to be abs of puzzle_number
			  short int first_digit_position,last_digit_position;
			  if (first_digits_of_puzzle_name(first_digit_position,last_digit_position)) {
				  ascii_string new_name = new char[strlen(tt_puzzle_file_name)+12]; // extra in case number is big
				  memcpy(new_name,tt_puzzle_file_name,first_digit_position);
				  itoa(-puzzle_number,new_name+first_digit_position,10);
				  strcat(new_name,tt_puzzle_file_name+last_digit_position+1);
				  tt_puzzle_file_name = new_name;
				  tt_user_puzzles = TRUE; // added on 300399
			  };
		  };
     } else if (stricmp(option,"-next_puzzle") == 0) {
        next_puzzle = (flag) atoi(arg);
		  // added conditional on 290499 so that -next_puzzle 0 works OK
        if (next_puzzle) tt_system_mode = PUZZLE;
     } else if (stricmp(option,"-autodemo") == 0) { 
        tt_autodemo = (AutoDemo) atoi(arg);
     } else if (stricmp(option,"-subtitles") == 0) {
        tt_subtitles_speed = (unsigned short int) atoi(arg);
        tt_subtitles_set = TRUE;
     } else if (stricmp(option,"-speak") == 0) {
        boolean speaking = (flag) atoi(arg);
//        if (!from_user && tt_log_version_number < 5) speaking = FALSE; // old demos doesn't expect text-to-speech
//        speaking_set = TRUE;
        switch (tt_marty_talk) {
           case MARTY_NO_TALK:
              if (speaking) {
					  if (tt_closed_caption || tt_closed_caption_alternative_language) {
						  tt_marty_talk = MARTY_SPEAKS_WITH_SUBTITLES;
					  } else {
						  tt_marty_talk = MARTY_SPEAKS;
					  };
				  };
              break;
           case MARTY_TALK_BALLOONS:
              if (speaking) {
					  tt_marty_talk = MARTY_SPEAKS_AND_TALK_BALLOONS;
				  };
              break;
           case MARTY_SPEAKS:
				  if (tt_closed_caption || tt_closed_caption_alternative_language) {
					  tt_marty_talk = MARTY_SPEAKS_WITH_SUBTITLES;
				  };
              break;
          case MARTY_SPEAKS_AND_TALK_BALLOONS:
              if (!speaking) tt_marty_talk = MARTY_TALK_BALLOONS;
              break;
        };
//        tt_marty_talk_to_save = tt_marty_talk;  // explictly changed by user
// this gives spurious warnings since user parameters generate command lines...
//#if !TT_TEXT_TO_SPEECH
//        if (tt_marty_talk == MARTY_SPEAKS || tt_marty_talk == MARTY_SPEAKS_AND_TALK_BALLOONS) {
//           tt_marty_talk = MARTY_TALK_BALLOONS;
//           if (speaking_set) {
//              pre_tt_err_file << S(IDS_MARTIAN_SPEAKS_ONLY_IN_32BITS) << endl;
//           };
//        };
//#endif
     } else if (stricmp(option,"-balloon") == 0) {
        boolean balloons = (flag) atoi(arg);
//        balloon_option_set = TRUE;
        switch (tt_marty_talk) {
           case MARTY_NO_TALK:
              if (balloons) tt_marty_talk = MARTY_TALK_BALLOONS;
              break;
           case MARTY_TALK_BALLOONS:
//              if (!balloons) tt_marty_talk = MARTY_NO_TALK; // bad idea?
              break;
           case MARTY_SPEAKS:
              if (balloons) {
					  tt_marty_talk = MARTY_SPEAKS_AND_TALK_BALLOONS;
				  };
              break;
           case MARTY_SPEAKS_AND_TALK_BALLOONS:
				  if (!balloons) {
					  if (tt_closed_caption || tt_closed_caption_alternative_language) {
						  tt_marty_talk = MARTY_SPEAKS_WITH_SUBTITLES;
					  } else {
						  tt_marty_talk = MARTY_SPEAKS;
					  };
				  };
              break;
        };
     } else if (stricmp(option,"-tts_uses_ds") == 0) { // not clear if this should be documented
        tt_tts_uses_direct_sound = (flag) atoi(arg);
     } else if (stricmp(option,"-mouse_sensitivity") == 0) {  // an experiment
        tt_mouse_sensitivity = (unsigned short int) atoi(arg);
        if (tt_mouse_sensitivity == 0) tt_mouse_sensitivity = 100;
     } else if (stricmp(option,"-direct_input") == 0) {
        tt_using_direct_input = (flag) atoi(arg);
     } else if (stricmp(option,"-city") == 0) {
        tt_city_name = copy_string_without_quotes(arg); // was copy string prior to 260601
		  if (!tt_ini_file_only_from_tt_ini_file) {
			  compute_local_ini_file_name(tt_city_name);  // copied here on 120201 -- was arg prior to 170302
			  if (tt_local_ini_file != NULL) { // new on 040301
					tt_local_ini_file_loaded_with_city = copy_string(tt_local_ini_file);
			  };
		  };
    //} else if (stricmp(option,"-copy_user") == 0) { // new on 030304
		  // this didn't quite interact well with time travel -- replaced by the following also on 030304
    //    tt_file_name_read_only = copy_string_without_quotes(arg);
		  //tt_city_name = copy_string(tt_file_name_read_only);
		  //set_tt_default_file_name(copy_string(tt_file_name_read_only),TRUE);
     } else if (stricmp(option,"-save_city_when_demo_ends") == 0) {
        tt_saved_city_file_name_when_demo_ends = copy_string_without_quotes(arg);	  
	  } else if (stricmp(option,"-resume") == 0) {
		  if (atoi(arg)) { // resume where left off last time
			  tt_city_name = most_recent_city_file_name();
		  };
     } else if (stricmp(option,"-trouble_shoot") == 0) {
        tt_trouble_shooting = (flag) atoi(arg);
	  } else if (stricmp(option,"-avenues_as_numbers") == 0) {
        tt_avenues_print_as_numbers = (flag) atoi(arg);
	  } else if (stricmp(option,"-adjustable_sizes") == 0) {
        tt_good_sizes_adjust_to_screen_size = (flag) atoi(arg);
	  } else if (stricmp(option,"-closed_caption") == 0) {
		  tt_closed_caption = (flag) atoi(arg);
	  } else if (stricmp(option,"-subtitle_marty") == 0) {
        tt_closed_caption_alternative_language = (flag) atoi(arg);
	  } else if (stricmp(option,"-create_narration") == 0) { // new on 070201 - should be 0, 1, 2
        tt_creating_narration_file = (NarrationCreationMode) atoi(arg);
	  } else if (stricmp(option,"-print_narration_start_times") == 0) { // new on 070201 
        tt_print_narration_start_times = (flag) atoi(arg);
	  } else if (stricmp(option,"-ini_file") == 0) { // new on 260601
//		  tt_ini_file = copy_string(arg); // caught earlier since needed before loading files
	  } else if (stricmp(option,"-local_ini_file") == 0) { // new on 050404
		  // handled earlier too
	  } else if (stricmp(option,"-integer_positions") == 0) { // new on 040302
		  tt_x_and_y_display_as_integers = (flag) atoi(arg);
	  } else if (stricmp(option,"-default_user") == 0) { // new on 200203
		  set_tt_default_file_name(copy_string(arg),TRUE); // TRUE added 141003
	  } else if (stricmp(option,"-seconds_between_logs") == 0) { // new on 300803 to selectively turn off time travel
		  int new_value = atoi(arg);
		  if (new_value > 0) {
			  tt_time_between_new_logs = new_value;
			  tt_dump_as_xml = TRUE; // time travel just won't work otherwise - new on 170903
		  } else { // zero (and non-numbers) and negative values
			  tt_time_between_new_logs = -1; // like zero but not to be overriden by toontalk.ini
		  };
	  } else if (stricmp(option,"-time_travel_enabled") == 0) { // new on 100903 to selectively turn off time 
		  tt_time_travel_enabled = (boolean) atoi(arg); // rewritten on 290903
		  if (tt_time_travel_enabled) {
			  tt_dump_as_xml = TRUE; // time travel just won't work otherwise - new on 170903		  
		  };
#if TT_SPECIAL_DEBUGGING
		  tt_special_debugging_file << "Time travel enabled set to " << (int) tt_time_travel_enabled << endl;
#endif
		  write_ini_entry("Switches","TimeTravelEnabledDefault",tt_time_travel_enabled); 
		  // rewritten on 290903 - controls if check mark is on by default
#if TT_ABSOLUTE_MOUSE
	  } else if (stricmp(option,"-absolute_mouse") == 0) { // new on 290801
        set_absolute_mouse_mode(atoi(arg));
#endif
	  } else if (stricmp(option,"-unicode_code_page") == 0) {
		  tt_code_page[FALSE] = atoi(arg);
		  tt_unicode = TRUE;
     } else if (stricmp(option,"-32") == 0 || stricmp(option,"-free_play") == 0) { // -free_play added 200203
       // ignore it -- startTT uses them
//#if TT_BETA
//	  } else if (stricmp(option,"-w") == 0) {
//		  tt_running_old_demos = (flag) atoi(arg);
//#endif
	  //} else if (stricmp(option,"-allow_multiple_toontalks") == 0) { 
		  // added soon after 3.21 -- commented out on 260904 since this is too late to set this flag
		 // tt_allow_multiple_toontalks = atoi(arg);
	  } else if (stricmp(option,"-must_be_new_user") == 0) { 
		  // new on 070504 to do nothing since handled elsewhere but shouldn't produce a warning
	  } else if (stricmp(option,"-exit_at_end_of_demo") == 0) { // new on 211004
		  tt_exit_at_end_of_log = atoi(arg);
	  } else if (stricmp(option,"-keep_running_demo_despite_problems") == 0) { // new on 311204
		  tt_run_demos_despite_anomalies = atoi(arg);
	  } else {
		  // note that this warning is well hidden as ToonTalk_pre_init.txt on a temp directory -- fixed on 100704
		  tt_error_file() << S(IDS_COMMAND_LINE_UNKNOWN_OPTION) << option << space() << arg << endl;  // added endl on 111205
		  // was pre_tt_err_file prior to 300502 but that is in main.cpp and this is called during initialization and later
		  problem_with_command_line = TRUE;
	  };
//	  argc -= 2;
	  i += 2; 
  };
//  if (tt_language == NO_LANGUAGE_SPECIFIED) {
     // could put up a dialog -- could warn -- could ...
//     tt_language = AMERICAN;
//  };
  if (!tt_subtitles_set) {
//     string entry = ini_entry("Executables","SubtitlesSuffix");
     tt_subtitles_speed = (unsigned short int) ini_int(AC(IDC_SWITCHES),AC(IDC_SUBTITLES_SPEED),FALSE,100); // re-written 110101
//	  GetPrivateProfileIntG(AC(IDC_SWITCHES),AC(IDC_SUBTITLES_SPEED),100,AC(IDC_TT_INI));
//        delete [] entry;
  };
//  if (tt_language == NO_LANGUAGE_SPECIFIED && resource_source() != NULL) {
//     tt_language = string_to_language(S(IDS_NATURAL_LANGUAGE));
//     if (tt_language == NO_LANGUAGE_SPECIFIED) {
//        pre_tt_err_file << "Confused about which language to speak!";
//     };
//  };
//#if TT_TEXT_TO_SPEECH
//  if (tt_system_mode == PUZZLE && !balloon_option_set && !speak_option_set) {
     // default for puzzles is both text and speech since there isn't a
     // problem with room
//     tt_marty_talk = MARTY_SPEAKS_AND_TALK_BALLOONS;
//  };
//#endif
  boolean need_new_name = (tt_user_name == NULL);
  if (need_new_name) {
	  if (!problem_with_command_line && !asked_name_already) {
		  ask_what_name(); 
	  };
	  if (tt_user_name == NULL) { // didn't get a name
		  set_tt_user_name(copy_string(S(IDS_NO_NAME_IS_YOUR_NAME2)));
		  if (!problem_with_command_line && !replaying()) {
			  // added replaying test on 100704 since warning doesn't make sense if running a demo
			  tt_error_file() << S(IDS_NO_NAME_IS_YOUR_NAME) << space() 
			  // was pre_tt_err_file prior to 300502 but that is in main.cpp and this is called during initialization and later
									<< S(IDS_NO_NAME_IS_YOUR_NAME2) << space()
									<< S(IDS_NO_NAME_IS_YOUR_NAME3);
		  };
	  };
  } else {
	  // this fixes a palette problem when the display has more than 256 colors
	  SetFocus(tt_main_window->get_handle());
  };
  if (need_new_name && !about_to_quit()) { // added !about_to_quit() since if the ask name dialog is closed this happens
	  compute_tt_file_name();
  };
  if (specified_video_mode != 0) {
	  tt_video_mode = specified_video_mode;
	  // commented out on 270200 since it causes things to load too soon (e.g. hat vs. hair)
//	  tt_setmode(tt_video_mode); // new on 150999 - does this mean demos will run at recorded resolution only?
  };
  if (tt_marty_talk == MARTY_SPEAKS_AND_TALK_BALLOONS && 
	   (tt_closed_caption || tt_closed_caption_alternative_language)) {
	  // for now no talk balloons if subtitled - besides it is not that useful
     tt_marty_talk = MARTY_SPEAKS_WITH_SUBTITLES;
  };
  if (tt_user_parameters != NULL) {
	  delete tt_user_parameters;
  };
  set_desired_head();
  tt_user_parameters = new Parameters();
  if (tt_puzzle_file_name != NULL) { // new on 080402
     File *file = open_file_or_url(tt_puzzle_file_name,TRUE);
     if (!file->Empty()) { // was != NULL prior to 070703
        if (file->Size() <= 1) { // was zero prior to 160402 but Netscape doesn't like zero byte files
           delete [] tt_puzzle_file_name;
           tt_puzzle_file_name = NULL;
           next_puzzle = TRUE;
        };
        file->Close();
     };
  };
  if (next_puzzle) resume_puzzles();
  if (tt_additional_command_line != NULL) { // new on 011003
	  string additional_command_line = tt_additional_command_line;
	  tt_additional_command_line = NULL; // do this before recurring so no infinite recursion
	  interpret_command_string(additional_command_line);
     delete [] additional_command_line;
  };
};

string show_html_dialog_named_in_ini_file(string key, string to_dialog) { // new on 300502
   string dialog_file = ini_entry("Executables",key,FALSE);
   if (dialog_file == NULL) {
		// if new dialog then use default -- new on 240303
		if (strcmp(key,"HTMLGeneralPauseHoldingItem") == 0) {  
			dialog_file = copy_string("hndpause.htm");
		} else if (strcmp(key,"HTMLJustSavedPause") == 0) { 
			dialog_file = copy_string("savpause.htm");
		} else if (strcmp(key,"HTMLNoSavePause") == 0) { // new on 141104
			dialog_file = copy_string("nopause.htm");
#if TT_ENHANCED_PUZZLE_DIALOG
#if TT_POST3187
		} else if (strcmp(key,"HTMLPuzzlePauseHoldingItem") == 0) { // new on 130607
			FileNameStatus name_status;
			string full_file_name = existing_file_name(name_status,"pzhpause.htm","htm","doc",FALSE,FALSE,TRUE);
			if (full_file_name != NULL) { // check that this enhancement has been installed
				dialog_file = copy_string("pzhpause.htm");
				delete [] full_file_name;
			} else {
				full_file_name = existing_file_name(name_status,"puzpause.htm","htm","doc",FALSE,FALSE,TRUE);
				if (full_file_name != NULL) { // check that this enhancement has been installed
					dialog_file = copy_string("puzpause.htm");
					delete [] full_file_name;
				} else { // fall back on the old
					dialog_file = copy_string("nopause.htm");
				};
			};
#endif
		} else if (strcmp(key,"HTMLPuzzlePause") == 0) { // new on 141104
			FileNameStatus name_status;
			string full_file_name = existing_file_name(name_status,"puzpause.htm","htm","doc",FALSE,FALSE,TRUE);
			if (full_file_name != NULL) { // check that this enhancement has been installed
				dialog_file = copy_string("puzpause.htm");
				delete [] full_file_name;
			} else { // fall back on the old
				dialog_file = copy_string("nopause.htm");
			};
#endif
#if TT_LOADING_AND_SAVING_INTERRUPTABLE
		} else if (strcmp(key,"HTMLCityLoadPause") == 0) { // new on 150105
			dialog_file = copy_string("ctypause.htm");
		} else if (strcmp(key,"HTMLObjectLoadPause") == 0) { // new on 150105
			dialog_file = copy_string("loadpaus.htm");
		} else if (strcmp(key,"HTMLCitySavePause") == 0) { // new on 150105
			dialog_file = copy_string("savctypa.htm");
		} else if (strcmp(key,"HTMLObjectSavePause") == 0) { // new on 150105
			dialog_file = copy_string("savobjpa.htm");
		} else if (strcmp(key,"HTMLReadURLPause") == 0) { // new on 150105
			dialog_file = copy_string("cpyurlpa.htm");
#endif
		} else {
#if TT_DEBUG_ON
			tt_error_file() << "Couldn't find " << key << " in Executables section of toontalk.ini - old dialogs used instead." << endl;
#endif
			return(NULL);
		};
	};
   // checking ini file was done prior to 310502 inside of show_html_dialog but that interfered with sharing with starttt.cpp (and clickme.cpp)
   int dialog_width = ini_int("Switches","DialogWidth",FALSE,0);
   int dialog_height = ini_int("Switches","DialogHeight",FALSE,0);
   int dialog_left = ini_int("Switches","DialogLeft",FALSE,0); // 0 was -1 prior to 240602
   int dialog_top = ini_int("Switches","DialogTop",FALSE,0);
   HWND window_handle = NULL;
	if (tt_main_window != NULL) { // rewritten on 080702 so that can get dialogs while initializing (e.g. missing files)
		window_handle = tt_main_window->get_handle();
	};
//   if (IsIconic(window_handle)) window_handle = NULL; // new on 250602
   FileNameStatus name_status;
   string full_file_name = existing_file_name(name_status,dialog_file,"htm","doc",FALSE,FALSE,TRUE); // new on 161002
	string result = NULL;
   // note this won't produce a mini-dump
	try {
      if (full_file_name != NULL) {
         result = show_html_dialog(full_file_name,window_handle,to_dialog,dialog_width,dialog_height,dialog_left,dialog_top);
         delete [] full_file_name;
      };
	} catch (...) { // new on 230902 to handle errors (like recursive calls due to missing DLLs) -- hard to test..
      tt_error_file() << "Failed to show HTML dialog for " << full_file_name << endl; // new on 291002
	};
   delete [] dialog_file;
   if (tt_main_window != NULL) { // condition new on 080702
		SetFocus(tt_main_window->get_handle()); // new on 070602 since dialog might interfere with focus
	};
	if (result != NULL) { // new on 010205
		string browse_command = strstr(result,"-browse ");
		if (browse_command != NULL) {
			string file_name = browse_command+strlen("-browse ");
			string first_space = strchr(file_name,' ');
			if (first_space != NULL) {
				first_space[0] = '\0'; // restored in a second
			};
			launch_help(file_name);
			set_currently_paused(TRUE); // wait to be maximized or closed
			MSG msg;
			while (currently_paused() && GetMessage(&msg, NULL, 0, 0) != -1) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			};
			// maximized so let's proceed
			if (first_space != NULL) {
				first_space[0] = ' '; // restored 
			};
			return(show_html_dialog_named_in_ini_file(key,to_dialog)); // show original dialog again
		};
	};
   return(result);
};

wide_string canonicalize_new_lines(wide_string source, int length) { // new on 090802
   // canonicalizes text to meet the XML standard (use of \n and no use of \r)
   if (source == NULL ) return(NULL);
   if (length < 0) length = wide_string_length(source);
   wide_string destination = new wide_character[length+1];
   int destination_index = 0;
   for (int i = 0; i < length; i++) {
      if (source[i] == '\r') {
         destination[destination_index] = '\n';
         if (i+1 < length && source[i+1] == '\n') {
            i++; // skip the '\n'
         };        
      } else {
         destination[destination_index] = source[i];
      };
      destination_index++;
   };
   destination[destination_index] = NULL; // terminate the string
   return(destination);
};

wide_string canonicalize_new_lines(string source, int length) { // new on 090802 -- narrow source version
   // canonicalizes text to meet the XML standard (use of \n and no use of \r)
   if (source == NULL ) return(NULL);
	wide_string wide_source = copy_wide_string(source,length);
	wide_string result = canonicalize_new_lines(wide_source);
	delete [] wide_source;
	return(result);
};
	/* changed on 220902 since this doesn't deal with characters over 127 (e.g. Swedish vowels)
   if (length < 0) length = strlen(source);
   wide_string destination = new wide_character[length+1];
   int destination_index = 0;
   for (int i = 0; i < length; i++) {
      if (source[i] == '\r') {
         destination[destination_index] = '\n';
         if (i+1 < length && source[i+1] == '\n') {
            i++; // skip the '\n'
         };        
      } else {
         destination[destination_index] = source[i];
      };
      destination_index++;
   };
   destination[destination_index] = NULL; // terminate the string
   return(destination);
*/

wide_string copy_wide_string(string source, int length) {
   return(copy_wide_string_with_code_page(source,length,tt_code_page[tt_use_spoken_alternative]));
};

void copy_wide_string(string source, int source_length, wide_string destination) {
   copy_wide_string_with_code_page(source,source_length,destination,tt_code_page[tt_use_spoken_alternative]);
};

int extract_base(string text, int &base) {
   // returns offset for text where number itself starts
   // typical text is 32#<some number>
	// this assumes the caller has deal with a preceeding minus sign
   if (text == NULL || text[0] == '\0') {
      base = 10;
      return(0);
   };
	if (text[1] == '#') { 
		base = text[0]-'0';
		if (base < 2 || base > 36) {
			base = 10; // warn??
		};
		return(2);
	} else if (text[1] != '\0' && text[2] == '#') { // checked that it isn't just a one character long string
		base = (text[0]-'0')*10+(text[1]-'0');
		if (base < 2 || base > 36) {
			base = 10;
		};
		return(3);
	};
   base = 10; // default
   return(0);
};   

int extract_base(wide_string text, int &base) {
   // wide_string version of the above (just a copy)
   if (text == NULL || text[0] == NULL) {
      base = 10;
      return(0);
   };
	if (text[1] == '#') { 
		base = text[0]-'0';
		if (base < 2 || base > 36) {
			base = 10; // warn??
		};
		return(2);
	} else if (text[1] != '\0' && text[2] == '#') { // checked that it isn't just a one character long string
		base = (text[0]-'0')*10+(text[1]-'0');
		if (base < 2 || base > 36) {
			base = 10;
		};
		return(3);
	};
   base = 10; // default
   return(0);
};

string hash_to_string(hash hash_bits) {
	string hash_string = new char[hash_string_length+1];
	hash_to_string(hash_bits,hash_string);
	return(hash_string);
};

//void hash_to_string(hash hash_bits, string hash_string) { // new on 180303
//	unsigned __int64 chunk; 
//	memcpy(&chunk,hash_bits,sizeof(chunk));
//   _ui64toa(chunk,hash_string,36); // base 36 wastes as little as this can waste
//	int length = strlen(hash_string);
//	for (int i = length; i < hash_string_length/2; i++) {
//		// pad if needed so can pull apart parts easily
//		hash_string[i] = ' ';
//	};
//	memcpy(&chunk,hash_bits+sizeof(chunk),sizeof(chunk));
//	_ui64toa(chunk,hash_string+hash_string_length/2,36);
//};

static const char	b16chars[] = "abcdefghijklmnop";

void hash_to_string(hash hash_bits, string hash_string) { // new on 180303
	// above was tricky and harder to compute hash from string - hexadecimal is fine (32 bytes vs 26)
	int index = 0;
	for (int i = 0; i < hash_byte_length; i++) {
		hash_string[index++] = b16chars[hash_bits[i]>>4]; // just first 4 bits
		hash_string[index++] = b16chars[hash_bits[i]&0xF]; // mask off first 4 bits
	};
	hash_string[index] = '\0'; // terminate string
};

static const wide_character b16wide_characters[] = L"abcdefghijklmnop";

void hash_to_wide_string(hash hash_bits, wide_string hash_string) { // new on 200303
	// variant of the above that constructs the wide string from the hash
	int index = 0;
	for (int i = 0; i < hash_byte_length; i++) {
		hash_string[index++] = b16wide_characters[hash_bits[i]>>4]; // just first 4 bits
		hash_string[index++] = b16wide_characters[hash_bits[i]&0xF]; // mask off first 4 bits
	};
	hash_string[index] = NULL; // terminate string
};

hash string_to_hash(string hash_string) {
	hash hash_bits = new byte[hash_byte_length];
	int index = 0;
	for (int i = 0; i < hash_byte_length; i++) {
		byte first_half = (hash_string[index++]-'a')<<4;
		byte second_half = hash_string[index++]-'a';
		hash_bits[i] = first_half|second_half;
	};
	return(hash_bits);
};

void media_file_name(hash hash_bits, string extension, string file_name, string directory, int directory_length) {
	if (directory != NULL) {
		memcpy(file_name,directory,directory_length);
	};
	// convert the 128-bit hash to a string with characters that are OK as file names
	//string guid_string;
	//string_of_guid((LPGUID) hash_bits,guid_string);
	//strcat(file_name,guid_string);
	//release_guid_string(guid_string);
	hash_to_string(hash_bits,file_name+directory_length);
	if (extension != NULL) { // can it be otherwise?
		strcat(file_name,extension);
	};
};

void media_file_name(string hash_string, string extension, string file_name, string directory, int directory_length) {
	if (directory != NULL) {
		memcpy(file_name,directory,directory_length);
	};
	// convert the 128-bit hash to a string with characters that are OK as file names
	//string guid_string;
	//string_of_guid((LPGUID) hash_bits,guid_string);
	//strcat(file_name,guid_string);
	//release_guid_string(guid_string);
	strcpy(file_name+directory_length,hash_string);
	if (extension != NULL) { // can it be otherwise?
		strcat(file_name,extension);
	};
};

//#if TT_GMP_VERSION_4_1 -- doesn't seem to be needed with the DLL from the Perl folks
//void mpq_canonicalize_safely(rational x) {
//	// since this sometimes causes an error try to get by without it
//	try {
//		mpq_canonicalize(x);
//	} catch (...) {
//		return;
//	};
//};
//#endif

// following new on 110703 so that sprites loaded via the L"include" attribute properly share and also this avoids redundant work
SpritePointer *included_sprites = NULL;
int included_sprites_count = 0;
const int included_sprites_count_growth = 32;

Sprite *included_sprite(int index) {
	if (index >= included_sprites_count) {
		return(NULL);
	};
	return(included_sprites[index]);
};

void remember_included_sprite(int index, Sprite *sprite) {
	// could optimize this if the first n elements are NULL to have an index for first_element...
	if (index >= included_sprites_count) {
		if (sprite == NULL) {
			return; // implicitly NULL already
		};
		// grow it
		int new_included_sprites_count = index+included_sprites_count_growth;
		SpritePointer *new_included_sprites = new SpritePointer[new_included_sprites_count];
		int i = 0;
		for (; i < included_sprites_count; i++) {
			new_included_sprites[i] = included_sprites[i];
		};
		for (; i < new_included_sprites_count; i++) {
			new_included_sprites[i] = NULL;
		};
		if (included_sprites != NULL) {
			delete [] included_sprites;
		};
		included_sprites = new_included_sprites;
		included_sprites_count = new_included_sprites_count;
	};
	included_sprites[index] = sprite;
};

void release_included_sprites() {
	delete [] included_sprites;
	included_sprites = 0;
	included_sprites_count = NULL;
};

void reset_included_sprites() {
	// more efficient than release_included_sprites() when just moving between segments
	for (int i = 0; i < included_sprites_count; i++) {
		included_sprites[i] = NULL;
	};
};

// following new on 160703 so include elements are shared and also updated if needed
xml_element_pointer *included_elements = NULL;
int included_elements_count = 0;
const int included_elements_count_growth = 32;

void associate_element_and_include_location(xml_element *element, int location) {
	if (location >= included_elements_count) {
		int new_included_elements_count = location+included_elements_count_growth;
		xml_element_pointer *new_included_elements = new xml_element_pointer[new_included_elements_count];
		int i = 0;
		for (; i < included_elements_count; i++) {
			new_included_elements[i] = included_elements[i];
		};
		for (; i < new_included_elements_count; i++) {
			new_included_elements[i] = NULL;
		};
		if (included_elements != NULL) {
			delete [] included_elements;
		};
		included_elements = new_included_elements;
		included_elements_count = new_included_elements_count;
	};
	included_elements[location] = element;
};

xml_element *existing_element_at_include_location(int location) {
	if (location >= included_elements_count) {
		return(NULL);
	};
	return(included_elements[location]);
};

void release_included_elements() {
	delete [] included_elements;
	included_elements = 0;
	included_elements_count = NULL;
};

void reset_included_elements() {
	for (int i = 0; i < included_elements_count; i++) {
		included_elements[i] = NULL;
	};
};


PostponedIncludeFileXML *postponed_include_file_xml = NULL; 
// new on 120703 to deal with include_files that might be saved before knowing if anyone connects to them or their followers (recursively)
int postponed_include_file_xml_count = 0;
int max_postponed_include_file_xml_count = 0;

void postpone_include_file_xml(int location, xml_element *element) {
   if (postponed_include_file_xml_count >= max_postponed_include_file_xml_count) {
      int new_max_postponed_include_file_xml_count = max(32,max_postponed_include_file_xml_count*2);
      PostponedIncludeFileXML *new_postponed_include_file_xml = new PostponedIncludeFileXML[new_max_postponed_include_file_xml_count];
      for (int i = 0; i < postponed_include_file_xml_count; i++) { // copy over old ones
         new_postponed_include_file_xml[i].location = postponed_include_file_xml[i].location;
         new_postponed_include_file_xml[i].element = postponed_include_file_xml[i].element;
      };
      delete [] postponed_include_file_xml;
      postponed_include_file_xml = new_postponed_include_file_xml;
      max_postponed_include_file_xml_count = new_max_postponed_include_file_xml_count;
   };
   postponed_include_file_xml[postponed_include_file_xml_count].location = location;
   postponed_include_file_xml[postponed_include_file_xml_count].element = element;
   postponed_include_file_xml_count++;
};

void release_postponed_include_file_xml() {
   if (postponed_include_file_xml != NULL) {
      delete [] postponed_include_file_xml;
      postponed_include_file_xml = NULL;
      max_postponed_include_file_xml_count = 0;
   };
};

millisecond time_between_postponed_include_files = 0;

void schedule_next_postponed_include_file_xml() { // new on 170703
	// rather than bunch up all these saves to disk spread them out during the current log segment
	// note that new postponements shouldn't happen during this period
	if (postponed_include_file_xml_count == 0) return;
	if (time_between_postponed_include_files == 0) {
		time_between_postponed_include_files = tt_time_between_new_logs/(postponed_include_file_xml_count+1); 
		// don't want to finish just when time goes into starting the next segment
	};
	tt_next_postponed_include_file_xml_time = tt_current_time+time_between_postponed_include_files;
};

int current_postponed_include_file_index = 0;

void do_next_postponed_include_file_xml(int index) {
	if (postponed_include_file_xml_count == 0) {
//		write_counters_in_archive(tt_current_log_segment,tt_oldest_log_segment,tt_youngest_log_segment); // moved on 190703
		// moved here on 180703 since tt_current_log_segment is now completely written
		return;
	};
	if (index < 0) {
		//postponed_include_file_xml_count--;
		//index = postponed_include_file_xml_count;
		// rewrote the above since the order of saving is important to get the location to match the zip archive's index
		index = current_postponed_include_file_index;
		current_postponed_include_file_index++;
		if (postponed_include_file_xml_count <= current_postponed_include_file_index) { // last one < for robustness
			// should it just turn off time travel if negative since broken - cause couldn't dump all in time_between_postponed_include_files
			time_between_postponed_include_files = 0; // reset
			current_postponed_include_file_index = 0;
			tt_next_postponed_include_file_xml_time = max_long;
//			write_counters_in_archive(tt_current_log_segment,tt_oldest_log_segment,tt_youngest_log_segment); // moved on 190703
		} else { // schedule next one
			tt_next_postponed_include_file_xml_time = tt_current_time+time_between_postponed_include_files;
		};
	};
	dump_xml_include_file(postponed_include_file_xml[index].element,postponed_include_file_xml[index].location);
	xml_document *document = xml_get_document(postponed_include_file_xml[index].element);
	document->Release();
};

void update_counters() {
	write_counters_in_archive(tt_current_log_segment,tt_oldest_log_segment,tt_youngest_log_segment);
};

void do_postponed_include_files_xml() {
   for (int i = 0; i < postponed_include_file_xml_count; i++) {
		do_next_postponed_include_file_xml(i);
   };
   postponed_include_file_xml_count = 0;
};

// following is new as of 140703
const int sprite_table_size = max_unsigned_short+1;
SpritesPointer sprite_table[sprite_table_size]; // hashing to 16 bits (at least for now)

unsigned short int hash16_guid(GUID *guid) {
	unsigned int *guid_as_ints = (unsigned int *) guid;
	unsigned int hash32 = guid_as_ints[0]^guid_as_ints[1]^guid_as_ints[2]^guid_as_ints[3];
	unsigned short int *hash_as_short_ints = (unsigned short int *) &hash32;
	return(hash_as_short_ints[0]^hash_as_short_ints[1]);
};

void initialize_sprite_table() {
	for (int i = 0; i < sprite_table_size; i++) {
		sprite_table[i] = NULL;
	};
};

void release_sprite_table() {
	for (int i = 0; i < sprite_table_size; i++) {
		just_destroy_list(sprite_table[i]);
	};
};

void print_guid_as_hexadecimal(GUID *guid, output_stream &stream) { // new on 250903 initially for use for debugging
	char guid_string[guid_string_size];
	guid_as_narrow_hexadecimal(guid,guid_string);
	stream.write(guid_string,guid_string_size);
};

Sprite *register_guid(GUID *guid, Sprite *sprite) {
	// changed on 050304 to return old registrant if another is already registered
#if TT_DEBUG_ON
	if (tt_debug_mode == 190703) {
		char guid_string[guid_string_size];
		guid_as_narrow_hexadecimal(guid,guid_string);
		tt_error_file() << "Registering GUID    " << guid_string << " with ";
		sprite->print(tt_error_file());
		tt_error_file() << " on frame " << tt_frame_number << " and segment number " << tt_current_log_segment << endl;
	};
	if (tt_debug_guid != NULL) {
		wide_character guid_string[guid_string_size];
		guid_as_hexadecimal(guid,guid_string);
		if (tt_debug_guid != NULL && compare_wide_strings(tt_debug_guid,guid_string) == 0) {
			log("Registering debug guid.");
		};
	};
#endif
	unsigned short int hash = hash16_guid(guid);
	Sprites *bucket = sprite_table[hash];
	if (bucket == NULL) {
		sprite_table[hash] = new Sprites(sprite);
		return(NULL);
	} else {
		return(bucket->insert_at_end_or_replace_same_guid(sprite,guid));
//		return(bucket->insert_at_end_if_unique(sprite,guid));
	};
};

void deregister_guid(GUID *guid) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 190703) {
		char guid_string[guid_string_size];
		guid_as_narrow_hexadecimal(guid,guid_string);
		tt_error_file() << "Un-registering GUID " << guid_string;
		tt_error_file() << " on frame " << tt_frame_number << " and segment number " << tt_current_log_segment << endl;
	};
	if (tt_debug_guid != NULL) {
		wide_character guid_string[guid_string_size];
		guid_as_hexadecimal(guid,guid_string);
		if (tt_debug_guid != NULL && compare_wide_strings(tt_debug_guid,guid_string) == 0) {
			log("Registering debug guid.");
		};
	};
#endif
	unsigned short int hash = hash16_guid(guid);
	Sprites *sprites = sprite_table[hash];
	if (sprites == NULL) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 190703) {
			tt_error_file() << "But no old entry found." << endl;
		};
#endif
		return; // warn since no need to deregister
	};
	sprite_table[hash] = sprite_table[hash]->remove_guid(guid);
	// warn if not found??
};

Sprite *find_sprite(GUID *guid) {
	Sprites *remaining = sprite_table[hash16_guid(guid)];
	while (remaining != NULL) {
		if (remaining->first()->has_guid(guid)) {
			return(remaining->first());
		};
		remaining = remaining->rest();
	};
	return(NULL);
};

void guid_as_hexadecimal(GUID *guid, wide_string guid_string) { // wide string since that is what XML provides
	// new on 150703
	byte *guid_as_bytes = (byte *) guid;
	int string_index = 0;
	for (int i = 0; i < 16; i++) {
		guid_string[string_index++] = (guid_as_bytes[i]>>4)+'a';
		guid_string[string_index++] = (guid_as_bytes[i]&0xF)+'a';
#if TT_DEBUG_ON
		if (tt_debug_mode == 1507032) {
			tt_error_file() << (int) guid_as_bytes[i] << " GUID byte becomes " << guid_string[string_index-2] 
								 << " and " << guid_string[string_index-1] << endl;
		};
#endif
	};
	guid_string[string_index] = NULL; // terminate string
};

void guid_as_narrow_hexadecimal(GUID *guid, string guid_string) { // narrow string for debugging and the like
	// new on 150703
	byte *guid_as_bytes = (byte *) guid;
	int string_index = 0;
	for (int i = 0; i < 16; i++) {
		guid_string[string_index++] = (guid_as_bytes[i]>>4)+'a';
		guid_string[string_index++] = (guid_as_bytes[i]&0xF)+'a';
	};
	guid_string[string_index] = NULL; // terminate string
};

void hexidecimal_as_guid(wide_string guid_string, GUID *guid) { // wide string since that is what XML provides
	byte *guid_as_bytes = (byte *) guid;
	int string_index = 0;
	for (int i = 0; i < 16; i++) {
		guid_as_bytes[i] = ((guid_string[string_index]-'a')<<4)|(guid_string[string_index+1]-'a');
		string_index += 2;
#if TT_DEBUG_ON
		if (tt_debug_mode == 1507032) {
			tt_error_file() << (int) guid_as_bytes[i] << " GUID byte computed from " << guid_string[string_index-2] 
								 << " and " << guid_string[string_index-1] << endl;
		};
#endif
	};
#if TT_DEBUG_ON
	if (tt_debug_guid != NULL && compare_wide_strings(tt_debug_guid,guid_string) == 0) {
		log("Processing debug guid.");
	};
#endif
};

void set_replay(ReplayState new_state) {
	if (tt_replay == new_state) return; // new on 200204 since not news
	tt_replay = new_state;
	if (tt_replay != NO_REPLAY) {
		 tt_dump_as_xml = TRUE; // regardless of toontalk.ini value (document this!) - new on 150903
#if TT_ENHANCED_DRAG_AND_DROP
	} else if (tt_main_window != NULL) { // && tt_log_in_archive == NULL) {
		// if time travelling maybe need to enable this when Paused
		register_drag_drop(tt_main_window->get_handle()); // new on 291204 -- didn't register earlier since drag and drop don't make sense
#endif
	};
};

int non_failing_getline(input_stream &stream, string buffer, int max_characters_to_read) {
	// new on 281003 since C++'s getline treats the file as empty if max_characters_to_read or more characters are on a line
	for (int i = 0; i < max_characters_to_read; i++) {
		char c = stream.get();
		if (stream == NULL) return(-1);
		if (c == '\n') { // reached new line
			return(i);
		} else {
			buffer[i] = c;
		};
	};
	if (stream.peek() == '\n') {
		stream.get(); // if ended after max characters then gobble up the new line
	};
	return(max_characters_to_read);
};

//#if TT_ALPHA_FEATURE // beginning 171203 used as well to fix notebook update time travel bug

string extract_file_from_archive(string relative_file_name, string archive_file_name, string destination_directory, 
											boolean relative_file_name_is_unique) { // new on 281003
	// added relative_file_name_is_unique on 260204 since for those it doesn't matter if tt_file_temp_directory_is_up_to_date
	// destination_directory new on 180204 so can be used for private media as well
	//if (destination_directory == NULL) {
	//	destination_directory = tt_extracted_file_temp_directory;
	//};
	if (relative_file_name == NULL) return(NULL); // new on 300304 for robustness
	string temp_file_name = append_strings(destination_directory,relative_file_name);
	if (destination_directory != tt_extracted_file_temp_directory ||
		 ((tt_file_temp_directory_is_up_to_date || relative_file_name_is_unique) && file_exists(temp_file_name,FALSE))) { 
		// can trust tt_extracted_file_temp_directory to be up-to-date (or else the media directory)
		return(temp_file_name);
	};
	// can check first if file has already been extracted so long as tt_extracted_file_temp_directory is reset each session -- right?
	if (extract_archive_to_directory(archive_file_name,relative_file_name,tt_extracted_file_temp_directory,TRUE,TRUE)) {
		return(temp_file_name);
	};
	delete [] temp_file_name;
	return(NULL);
};

boolean delete_file_or_directory(WIN32_FIND_DATA &FindFileData, string directory) {
	if (FindFileData.cFileName == NULL || FindFileData.cFileName[0] == '.') return(TRUE); // ignore these - not clear if NULL is possible
	if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_SYSTEM) {
		tt_error_file() << "Attempted to delete a system file or directory: " << FindFileData.cFileName << endl;
		return(FALSE);
	} else if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) {
		char full_directory_path[MAX_PATH];
		strcpy(full_directory_path,directory);
		strcat(full_directory_path,FindFileData.cFileName);
		strcat(full_directory_path,"\\");
		if (delete_all_files_in_directory(full_directory_path)) {
			return(RemoveDirectory(full_directory_path));
		} else {
			return(FALSE);
		};
	} else {
		char full_file_path[MAX_PATH];
		strcpy(full_file_path,directory);
		strcat(full_file_path,FindFileData.cFileName);
		if (DeleteFile(full_file_path) == 0) {
			if (tt_exe_to_run_upon_shutdown == NULL || stricmp(tt_exe_to_run_upon_shutdown,full_file_path) != 0) {
				// added conditions on 021103 since it is normal if an EXE was in a DMO file to be unable to delete it
				int error_code = GetLastError();
				tt_error_file() << "Something went wrong deleting the file " << full_file_path << ". Error code = " << error_code << endl;
			};
			return(FALSE);
		} else {
			return(TRUE);
		};
	};
};

boolean delete_all_files_in_directory(string directory) { // new on 291003 -- assumes that directory ends with a \
	// based upon http://msdn.microsoft.com/library/default.asp?url=/library/en-us/fileio/base/deleting_files.asp
	WIN32_FIND_DATA FindFileData;
   char directory_wild_cards[MAX_PATH];
	strcpy(directory_wild_cards,directory);
	strcat(directory_wild_cards,"*");
   HANDLE hFind = FindFirstFile(directory_wild_cards, &FindFileData);
   if (hFind == INVALID_HANDLE_VALUE) {
		int error_code = GetLastError();
		tt_error_file() << "Something went wrong deleting the files in " << directory << ". Error code = " << error_code << endl;
		return(FALSE);
   } else {
      delete_file_or_directory(FindFileData,directory);
      while (FindNextFile(hFind, &FindFileData) != 0) {
			if (!delete_file_or_directory(FindFileData,directory)) {
				FindClose(hFind);
				return(FALSE);
			};
      };
   };
	int error_code = GetLastError();
   if (error_code == ERROR_NO_MORE_FILES) {
      FindClose(hFind);
		return(TRUE);
   } else {
      tt_error_file() << "Something went wrong deleting the files in " << directory << ". Error code = " << error_code << endl;
		return(FALSE);
   };
};

void add_new_media_file_names(string new_names, string &old_names) { // new on 081203 - deals with duplicates
	if (old_names == NULL) {
		old_names = append_strings(new_names," "); // prior to 110205 was copy_string(new_names);
	} else {
		int new_names_length = strlen(new_names);
		int old_names_length = strlen(old_names);
		if (new_names_length == old_names_length && strcmp(old_names,new_names) == 0) { 
			// new on 100205 since otherwise adding 1 to 1 makes 2 names even if identical names
			return; // nothing to do
		};
		int new_length = new_names_length+old_names_length+3; 
		// might be too long if there are duplicates // was +2 prior to 110205 but may introduce an extra space now
		// was +1 prior to 160304 -- if doesn't end in space may need an extra character
		string result = new char[new_length+1];
		memcpy(result,old_names,old_names_length);
		int result_end = old_names_length;
		int start = 0;
		int last_space = 0; // added on 110204 since isn't always terminated by a space
		for (int i = 0; i < new_names_length; i++) {
			if (new_names[i] == ' ') { // space separates them and terminates the entire list
				last_space = i;
				new_names[i] = '\0'; // temporarily terminate it
				if (strstr(old_names,new_names+start) == NULL) { // new one
					int new_name_length = 1+i-start;
					new_names[i] = ' '; // restore
					memcpy(result+result_end,new_names+start,new_name_length);
//					result[result_end++] = ' '; // space seperator -- missing prior to 110204
					result_end += new_name_length;
					result[result_end] = ' '; // space seperator -- missing prior to 110204 -- moved here on 240204
				} else {
					new_names[i] = ' '; // restore
				};
				start = i+1;
			};
		};
		if (last_space+1 != new_names_length) { // didn't end with a space so treat the last file name now -- 110204
			int length_of_last_name = new_names_length-last_space;
			result[result_end++] = ' '; // space seperator
			memcpy(result+result_end,new_names+last_space,length_of_last_name);
			result_end += length_of_last_name;
			result[result_end++] = ' '; // space seperator -- new on 110205 since otherwise can get two names stuck together 
			// (if one came from sprites)
		};
		delete [] old_names;
		old_names = result;
		old_names[result_end] = '\0'; //terminate it
	};
};

void set_recordable_ini_options() { // abstracted on 050404
	 // (tt_log_version_number >= 26) was FALSE prior to 170301 below but this needs to be recorded in the log for the right interpretation
	 tt_2_mouse_buttons_is_escape = ini_int(AC(IDC_SWITCHES),"LeftAndRightMouseButtonsDownMeansEscape",
														 (tt_log_version_number >= 26),tt_2_mouse_buttons_is_escape); // new on 160301
	 if (tt_log_version_number >= 27 && tt_system_mode != PUZZLE) { // added second condition on 140901
		 tt_vacuum_maximum_item_count = ini_int(AC(IDC_SWITCHES),"MaximumItemsInDusty",TRUE,tt_vacuum_maximum_item_count); // new on 190401
	 } else {
		 tt_vacuum_maximum_item_count = -1; // prior to version 27 this was "infinite" - negative numbers act that way
	 };
    boolean extended_character; // new on 070502 - here because need logging to be working
    unsigned char character_entry = name_to_character(ini_entry("User","RunCharacter",TRUE),extended_character);
    if (character_entry != 0) {
       tt_run_character = character_entry;
    };
    character_entry = name_to_character(ini_entry("User","StopCharacter",TRUE),extended_character);
    if (character_entry != 0) {
       tt_stop_character = character_entry;
    };
    tt_vacuum_used_once_per_click = ini_int(AC(IDC_SWITCHES),"DustyUsedOncePerClick",(tt_log_version_number >= 29),tt_vacuum_used_once_per_click);
	 tt_expander_usage_maximum = ini_int(AC(IDC_SWITCHES),"MillisecondsPumpyUsedPerClick",(tt_log_version_number >= 29),tt_expander_usage_maximum);
#if TT_XML
    tt_dump_as_xml = (ini_int(AC(IDC_SWITCHES),"SaveInXML",FALSE,tt_dump_as_xml) || tt_time_between_new_logs > 0); 
	 // new on 170903 that if time travel is enabled ignore this
#endif
	 tt_speak_button_name = ini_int(AC(IDC_SWITCHES),"SpeakToolButtons",FALSE,tt_speak_button_name); // new on 240902
	 // new on 230203 - prior to version 30 sensors updated when dropped
	 tt_parts_change_when_bammed = !ini_int("Switches","SensorsChangeAtDropNotWhenBammed",(tt_log_version_number >= 30),
														 (tt_log_version_number >= 30)?!tt_parts_change_when_bammed:FALSE); 
	 // 120303 removed a space from the end of GoodSizesAreAFixedPercentageOfScreen
    tt_good_sizes_adjust_to_screen_size = !ini_int(AC(IDC_SWITCHES),"GoodSizesAreAFixedPercentageOfScreen",(tt_log_version_number >= 30),
																	!tt_good_sizes_adjust_to_screen_size); // new on 181002
    // moved up to before log_initialization so that used when city is dumped
	 string number_shrinkage_string = ini_entry("Switches","NumberShrinkage",(tt_log_version_number >= 32));
	 if (number_shrinkage_string != NULL) {
		 double number_shrinkage = atof(number_shrinkage_string);
		 if (number_shrinkage > 0.0 && number_shrinkage < 1.0) {
			 tt_number_shrinkage = number_shrinkage;
		 };
		 delete [] number_shrinkage_string;
	 };
	 if (tt_log_version_number >= 33) {
		 // made into a conditional so this isn't read from toontalk.ini if running an old demo
		 tt_right_to_left_text = ini_int("Switches","DisplayTextRightToLeft",TRUE,tt_right_to_left_text); 
		 // new on 081203 -- moved out of TT_ALPHA_FEATURE on 040204
		 // default was negated below prior to 110204
		 tt_scrolling_ok = !ini_int("Switches","DontScrollOnFloor",TRUE,!tt_scrolling_ok); 
		 // new on 030204 -- moved out of TT_ALPHA_FEATURE on 040204
		 if (!tt_scrolling_ok) { // new on 040204 - just turned off scrolling to re-compute limits of programmer
			 tt_programmer->initialize();
		 };
	 };
	 if (tt_log_version_number >= 40) {
		 tt_match_blank_with_sensor_then_run_once_per_cycle = ini_int("Switches","RunOncePerCycleIfMatchBlankWithSensor",TRUE,
																						  tt_match_blank_with_sensor_then_run_once_per_cycle); // new on 030404
		 tt_robot_drop_location_follows_camera = ini_int("Switches","RobotDropLocationFollowsCamera",TRUE,tt_robot_drop_location_follows_camera); 
		 // new on 040404
	 } else {
	    tt_match_blank_with_sensor_then_run_once_per_cycle = FALSE; // old behavior
		 tt_robot_drop_location_follows_camera = TRUE; // new on 0504004
	 };
	 // tt_dump_so_equals_share = ini_int(AC(IDC_SWITCHES),"DumpCopiesCompactly",(tt_log_version_number > 27),tt_dump_so_equals_share); 
	 // new on 040601
	 if (tt_log_version_number >= 48) {
		 tt_reincarnate_tools = ini_int("Switches","ReincarnateToolsIfDestroyed",TRUE,tt_reincarnate_tools); // new on 070704 -- moved here 080704
	 };
	 if (tt_log_version_number >= 55) {
		 tt_number_to_text_exact = ini_int("Switches","NumberToTextConversionIsExact",TRUE,tt_number_to_text_exact); // new on 121004
	 } else {
		 tt_number_to_text_exact = FALSE; // for backwards compatibility
	 };
	 if (tt_log_version_number >= 57) {
		 tt_shrink_and_grow_numbers = ini_int("Switches","OKToDisplayNumbersShrinkingAndGrowing",TRUE,tt_shrink_and_grow_numbers); // new on 251004
	 } else {
		 tt_shrink_and_grow_numbers = FALSE;
	 };
	 if (tt_log_version_number >= 75) { // new on 130505
		 tt_x_and_y_display_as_integers = ini_int("Switches","DisplayPositionsAsIntegers",TRUE,tt_x_and_y_display_as_integers); 
	 } else {
		 tt_x_and_y_display_as_integers = TRUE; // new on 050805 to restore old behaviour for old demos (though it was inconsistent for a while)
	 };
#if TT_CALL_FOR_MARTY
	 if (tt_log_version_number >= 79) { // new on 030805
		 tt_marty_appears_only_if_called = ini_int("Switches","MartyAppearsOnlyIfCalled",TRUE,TRUE); // default is TRUE after 030805
		 tt_call_for_marty_wait_duration // new on 050805 -- specified in seconds -- stored in milliseconds
			 = ini_int("Switches","MartyHasSomethingToSayDisplayDuration",TRUE,tt_call_for_marty_wait_duration/1000)*1000; 
	 } else {
		 tt_marty_appears_only_if_called = FALSE; // old default for old demos
	 };
#endif
};

boolean need_to_display_insertion_caret(int insertion_point) {
	return(insertion_point >= 0 && (tt_current_time/500)%2 == 0);
};

boolean reachable_from_remote_looks(Sprite *possible_remote_looks, Sprite *sprite) {
	// new on 010704 to figure out if forwarded keyboard commands should support keyboard editing
	Picture *others_controlee = possible_remote_looks->pointer_to_controlee();
	if (others_controlee == NULL) return(FALSE);
	if (others_controlee == sprite) return(TRUE);
	Sprite *indirection = others_controlee->pointer_to_indirection();
	while (indirection != NULL) {
		if (indirection == sprite) return(TRUE);
		indirection = indirection->pointer_to_indirection();
	};
	return(FALSE);
};

void set_toolbox_notebook(Notebook *new_notebook) {
	Notebook *old_notebook = tt_toolbox_notebook;
	tt_toolbox_notebook = new_notebook;
	if (old_notebook != NULL) { // moved here on 251000 since otherwise is protected by tt_toolbox
		old_notebook->destroy();
	};
	if (tt_toolbox_notebook != NULL) tt_toolbox_notebook->increment_ref_count(); // removed 'this' arg on 080704
};

void set_toolbox_expander(Expander *tool) {
   Expander *old_expander = tt_toolbox_expander;
	tt_toolbox_expander = tool;
	if (old_expander != NULL) { // moved here on 251000 since otherwise is protected by tt_toolbox
		old_expander->remove_from_floor(FALSE);
		old_expander->destroy();
	};
	if (tt_toolbox_expander != NULL) tt_toolbox_expander->increment_ref_count(); // new on 261000
};

void set_toolbox_vacuum(Vacuum *tool) {
	Vacuum *old_vacuum = tt_toolbox_vacuum;
	tt_toolbox_vacuum = tool;
	if (old_vacuum != NULL) { // moved here on 251000 since otherwise is protected by tt_toolbox
		old_vacuum->remove_from_floor(FALSE);
		old_vacuum->destroy();
	};
	if (tt_toolbox_vacuum != NULL) tt_toolbox_vacuum->increment_ref_count(); // new on 261000
};

void set_toolbox_copier(Copier *tool) {
	Copier *old_copier = tt_toolbox_copier;
	tt_toolbox_copier = tool;
	if (old_copier != NULL) {
		old_copier->remove_from_floor(FALSE); // moved here on 251000 since otherwise is protected by tt_toolbox
		old_copier->destroy();
	};
	if (tt_toolbox_copier != NULL) tt_toolbox_copier->increment_ref_count(); // new on 261000
};

// following moved from Toolbox:: and updated on 080704
void initialize_toolbox_expander(boolean set_size_to_favorite) { // abstracted on 060704
	city_coordinate good_width, good_height;
	tt_toolbox_expander->good_size(good_width,good_height);
   tt_toolbox_expander->set_default_favorite_size_and_location(good_width,good_height,8*tile_width,tile_height/2);
	if (set_size_to_favorite) {
		tt_toolbox_expander->set_to_favorite_size();
	};
};

void initialize_toolbox_vacuum(boolean set_size_to_favorite) { // abstracted on 060704
   city_coordinate good_width, good_height;
	tt_toolbox_vacuum->good_size(good_width,good_height);
   tt_toolbox_vacuum->set_default_favorite_size_and_location(good_width,good_height,tile_width,tile_height/2);
	if (set_size_to_favorite) {
		tt_toolbox_vacuum->set_to_favorite_size();
	};
};

void initialize_toolbox_copier(boolean set_size_to_favorite) { // abstracted on 060704
   city_coordinate good_width, good_height;
	tt_toolbox_copier->good_size(good_width,good_height);
   tt_toolbox_copier->set_default_favorite_size_and_location(good_width,good_height,
                                                  tile_width/4,tile_height*5);
	if (set_size_to_favorite) {
		tt_toolbox_copier->set_to_favorite_size();
	};
};

void initialize_toolbox_notebook(boolean set_size_to_favorite) { // abstracted on 070704
	city_coordinate good_width, good_height;
	tt_toolbox_notebook->good_size(good_width,good_height);
	tt_toolbox_notebook->set_default_favorite_size_and_location(good_width,good_height,11*tile_width,0);
	if (favorites_set(NOTEBOOK_FAVORITE)) { // was commented out prior to 130100 -- rewritten on 161102
		tt_toolbox_notebook->set_favorite(tt_favorites[NOTEBOOK_FAVORITE]);
	};
	if (set_size_to_favorite) {
		tt_toolbox_notebook->set_to_favorite_size();
	};
};

Notebook *pointer_to_toolbox_notebook() { // moved from Toolbox on 080704
	if (tt_toolbox_notebook != NULL) {
      if (favorites_set(NOTEBOOK_FAVORITE)) { // rewritten on 161102
         tt_toolbox_notebook->set_favorite(tt_favorites[NOTEBOOK_FAVORITE]);
      };
		return(tt_toolbox_notebook);
	};
	if (!tt_reincarnate_tools) {
		return(NULL); // correct?
	};
	// no longer using the following: - commented out on 210100
//	if (tt_replay && tt_log_version_number > 17) {
//		tt_replay = FALSE; // added 130100 to avoid recursive invocations of the following
//		set_notebook(replay_notebook());
//		tt_replay = TRUE;
//		if (notebook != NULL) {
//			notebook->set_file_name(copy_string("bok"));
//		};
//	};
//	if (tt_toolbox_notebook == NULL) {
		tt_toolbox_notebook = new Notebook(0,0,0,copy_ascii_string("bok")); // beginning 210802 the caller copies the file name
//	};
	city_coordinate good_width, good_height;
	tt_toolbox_notebook->good_size(good_width,good_height);
	tt_toolbox_notebook->set_default_favorite_size_and_location(good_width,good_height,11*tile_width,0);
	if (favorites_set(NOTEBOOK_FAVORITE)) { // rewritten on 161102
      tt_toolbox_notebook->set_favorite(tt_favorites[NOTEBOOK_FAVORITE]);
   };
	tt_toolbox_notebook->increment_ref_count(); // toolbox and typically floor -- removed this arg on 080704
//	if (tt_logging) {
//		record_notebook(notebook);
//	};
	return(tt_toolbox_notebook);
};

boolean toolbox_tool(Sprite *sprite) { // updated on 080704
	return(sprite == tt_toolbox_copier || sprite == tt_toolbox_expander || 
			 sprite == tt_toolbox_notebook || sprite == tt_toolbox_vacuum || sprite == tt_toolbox);
};

boolean not_friend_of_tooly(Sprite *sprite, void *predicate_arg) { // used as a PredicateCallBack
	//if (tt_toolbox == NULL) return(FALSE);
	//if (tt_toolbox == sprite) return(TRUE); // Tooly is a friend of himself
	//return(!toolbox_tool(sprite));
	return(!toolbox_tool(sprite));
};

void release_toolbox_tools() {
	set_toolbox_vacuum(NULL);
	set_toolbox_copier(NULL);
	set_toolbox_expander(NULL);
	set_toolbox_notebook(NULL);
};

string bignum_to_string(string narrow_text, int base, bignum_pointer bignum_value, int total_text_size) { // new on 040904
	try {
		return(mpz_get_str(narrow_text,base,bignum_value));
	} catch (...) {
		// new on 040904 for more graceful recovery when number is too big
		static boolean gave_number_display_warning = FALSE;
		if (!gave_number_display_warning) {
			log("Failed to convert very big number to string because it was too big."); // removed on 081104 ,FALSE,TRUE);
			tt_err_file_important_but_no_error = !replaying(); // was TRUE prior to 150505 // new on 081104 since isn't a ToonTalk problem
		};
		if (total_text_size > 0) {
			// updated 251104
			string unable1 = S(IDS_UNABLE_TO_DISPLAY1,"Unable to display");
			string unable2 = S(IDS_UNABLE_TO_DISPLAY2,"digits.");
			string unable3 = S(IDS_UNABLE_TO_DISPLAY3,"ToonTalk can display it in a base that is a power of 2.");
			strcpy(narrow_text,unable1);
			strcat(narrow_text," ");
			// was strlen of unable1 prior to 150105 but that clobbered the space
			itoa(total_text_size-1,narrow_text+strlen(narrow_text),10); 
			// -1 since total_text_size includes room for the sign if negative -- new on 251104
			strcat(narrow_text," ");
			strcat(narrow_text,unable2);
			strcat(narrow_text," ");
			strcat(narrow_text,unable3);
			if (!gave_number_display_warning) {
				tt_error_file() << "The number has " << (total_text_size-1) << " digits. " << endl;
			};
		} else {
			// added "this" below on 151104
			strcpy(narrow_text,S(IDS_UNABLE_TO_DISPLAY_THIS_NUMBER,"Unable to display this number because it has too many digits."));
			strcat(narrow_text," ");
			strcat(narrow_text,S(IDS_UNABLE_TO_DISPLAY3,"ToonTalk can display it in a base that is a power of 2."));
		};
		gave_number_display_warning = TRUE;
		return(narrow_text);
	};
};

boolean set_bignum_value_to_string(bignum_pointer bignum_value, string text, int base) { 
	// new on 291104 to catch stack overflows from very large numbers (e.g. 10^10000000 base 32)
	try {
		mpz_set_str(bignum_value,text,base);
		return(TRUE);
	} catch (...) {
		tt_error_file() << "Failed to load the value of a number that has " << strlen(text) << " digits in base " << base << endl;
		tt_error_file() << "Value is lost. Sorry. Number is set to 1. " << endl;
		mpz_set_str(bignum_value,"1",base);
		tt_err_file_important_but_no_error = TRUE;
		return(FALSE);
	};
};

boolean gave_unable_to_write_file_warning = FALSE;

void unable_to_write_file(string file_name) { // new on 290904
	if (gave_unable_to_write_file_warning) return;
	gave_unable_to_write_file_warning = TRUE;
	if (ini_entry("Switches","DoNotWarnAboutInabilityToWriteUserFiles",0)) { // new on 300904
		return;
	};
	int error_code = GetLastError();
	const int max_message_size = MAX_PATH+2000;
	char message[max_message_size];
	output_string_stream stream(message,max_message_size);
	stream << S(IDS_UNABLE_TO_WRITE_FILE,"ToonTalk is unable to write to the file") << space() << '"' << file_name << '"' << endl;
	stream << S(IDS_EXPLAIN_WHY_UNABLE_TO_WRITE_FILE,
					"Perhaps the folder is full and you need to delete some files or maybe you don't have permission to write to that file or folder.") 
			 << endl;
	if (error_code != 0 && error_code != 80) { 
		// added test for file already exists (80) on 071004 since that is often intentional (e.g. hash file names)
		output_last_error(error_code,stream);
	};
	if (strstr(file_name,tt_private_media_directory) != NULL) { // error_code == 5 && 
		// new on 101004 -- quotes for the directory added on 131004
		stream << S(IDS_UNABLE_TO_WRITE_TO_FOLDER,"ToonTalk cannot write to the folder") 
				 << space() << '"'<< tt_private_media_directory << '"' << endl;
		stream << S(IDS_SOME_MEDIA_MAY_BE_LOST,"Some user provided pictures and sounds may be lost.") << endl;
		stream << S(IDS_CAN_SET_MEDIADIR,
						"You can provide a different directory by setting MediaDir in the Directories section of the toontalk.ini file.")
						<< endl;
	} else {
		stream << S(IDS_TOONTALK_WILL_CONTINUE_BUT_UNABLE_TO_SAVE,
					   "ToonTalk will attempt to continue but it probably will not be able to save your work.") << endl;
	};
	stream.put('\0');
	log(message,FALSE,TRUE);
	message_box(MB_ICONINFORMATION|MB_OK,message);
	//} else {
	//	stream << "ToonTalk will stop now, sorry." << endl;
	//	stream.put('\0');
	//	message_box(MB_ICONSTOP|MB_OK|MB_SYSTEMMODAL,message,NULL,FALSE);
	//};
};

boolean gave_unable_to_read_file_warning = FALSE;

void unable_to_read_file(string file_name, string extra_message) { // new on 101004
	if (gave_unable_to_read_file_warning) return;
	gave_unable_to_read_file_warning = TRUE;
	int error_code = GetLastError();
	const int max_message_size = MAX_PATH+2000;
	char message[max_message_size];
	output_string_stream stream(message,max_message_size);
	stream << S(IDS_UNABLE_TO_READ_FILE,"ToonTalk is unable to read the file") 
			 << space() << '"' << file_name << '"' << endl; // quotes added 131004
	if (extra_message != NULL) {
		stream << extra_message << endl;
	};
	if (error_code != 0) {
		output_last_error(error_code,stream);
	};
	stream << S(IDS_WILL_CONTINUE_BUT_MAYBE_MEDIA_WILL_BE_MISSING,
					"ToonTalk will attempt to continue but some things (pictures or sounds) may be missing.") << endl;
	stream.put('\0');
	log(message,FALSE,TRUE);
	message_box(MB_ICONINFORMATION|MB_OK,message);
	//} else {
	//	stream << "ToonTalk will stop now, sorry." << endl;
	//	stream.put('\0');
	//	message_box(MB_ICONSTOP|MB_OK|MB_SYSTEMMODAL,message,NULL,FALSE);
	//};
};

string error_string_buffer = NULL;

output_stream & tt_error_file() { // new on 300904
	if (tt_err_file == NULL) {
		const int max_error_string_buffer = 10000; // should be more than enough since reset on each call to this function
		if (error_string_buffer == NULL) {
			error_string_buffer = new char[max_error_string_buffer]; 
		};
		static output_string_stream tt_err_stream(error_string_buffer,max_error_string_buffer);
		tt_err_stream.seekp(0); // go to beginning each time
		return(tt_err_stream);
	} else {
		return(tt_err_file);
	};
};

void release_error_string_buffer() { // new on 300904
	if (error_string_buffer != NULL) {
		delete [] error_string_buffer;
		error_string_buffer = NULL;
	};
};

boolean string_length_less_than(string s, int length) { // new on 251004
	if (s == NULL) return(TRUE);
	int index = 0;
	while (s[index] != '\0') {
		index++;
		if (index == length) return(FALSE);
	};
	return(TRUE);
};

int find_separator_location(wide_string wide_text, int wide_text_length, wide_string separators) {
	int separators_length = wide_string_length(separators);
	for (int i = 0; i < wide_text_length; i++) {
		for (int j = 0; j < separators_length; j++) {
			if (wide_text[i] == separators[j] && i > 0 && wide_text[i-1] != L' ') {
				// added space test since isn't really a separator if proceeded by a space (e.g. in a sequence of operations)
				return(i);
			};
		};
	};
	return(-1);
};

boolean unnamed_user() { // abstracted on 281004
	// first condition added on 250105 for robustness
	return(tt_file_name != NULL && strcmp(tt_file_name,AS(IDS_NO_NAME_IS_YOUR_NAME2)) == 0);
};

boolean is_digit(wide_character c) {
	return(c >= L'0' && c <= L'9');
};

boolean is_zip_cookie(string cookie) {
	// abstracted and added TT on 281204 and removed TT on 281204 since unzip fails with "wrong" cookie
	return(cookie[0] == 'P' && cookie[1] == 'K'); // || (cookie[0] == 'T' && cookie[1] == 'T'));
};

#if TT_LOADING_AND_SAVING_INTERRUPTABLE

void ask_response_to_load_interruption() { // new on 150105
//	MessageBox(NULL,"calling ask_response_to_load_interruption","TT",MB_OK); // just for testing!! 270105
	char parameters[100]; 
	strcpy(parameters,"count;");
	itoa(tt_sprites_loaded_count,parameters+strlen(parameters),10);
	if (tt_loading_a_city) {
		char previous_city_file_name[MAX_PATH];
		strcpy(previous_city_file_name,tt_user_directory);
		strcat(previous_city_file_name,tt_file_name); // what if loading DefaultUser's city??
		strcat(previous_city_file_name,".xml-1.cty"); // kind of dumb that the -1 ends up after the .xml but too much work to change everything
		if (!file_exists(previous_city_file_name)) {
			strcat(parameters,";previous_city;0");
		} else {
			strcat(parameters,";previous_city;1");
		};
		pause_toontalk(FALSE,FALSE,FALSE);
		string from_dialog = show_html_dialog_named_in_ini_file("HTMLCityLoadPause",parameters);
		unpause_toontalk();
		tt_screen->display(TRUE);
		tt_main_window->Paint();
		if (from_dialog == NULL || strcmp(from_dialog,"***CLOSED***") == 0 || strcmp(from_dialog,"1") == 0) { // continue
			// do nothing
		} else {
			string city_file_name = new char[MAX_PATH];
			if (strcmp(from_dialog,"2") == 0) {
				// previous city
				strcpy(city_file_name,previous_city_file_name);
			} else if (strcmp(from_dialog,"3") == 0) {
				strcpy(city_file_name,tt_main_directory);
				strcat(city_file_name,"default.xml.cty");
			};	
			reset_current_queue(); // new on 130405 since may have interrupted while loading the queue
			throw(city_file_name);
		};
	} else { // loading an object
		pause_toontalk(FALSE,FALSE,FALSE);
		string from_dialog = show_html_dialog_named_in_ini_file("HTMLObjectLoadPause",parameters);
		unpause_toontalk();
		tt_screen->display(TRUE);
		tt_main_window->Paint();
		if (from_dialog != NULL && strcmp(from_dialog,"3") == 0) {
			throw(0); // could signal different things someday...
		};
	};
};

void ask_response_to_save_interruption() { // new on 150105
//	MessageBox(NULL,"calling ask_response_to_save_interruption","TT",MB_OK); // just for testing!! 270105
	char parameters[100]; 
	strcpy(parameters,"count;");
	itoa(tt_sprites_saved_count,parameters+strlen(parameters),10);
	pause_toontalk(FALSE,FALSE,FALSE);
	string from_dialog;
	if (tt_dump_reason == DUMPING_CITY) {
		from_dialog = show_html_dialog_named_in_ini_file("HTMLCitySavePause",parameters);
	} else {
		from_dialog = show_html_dialog_named_in_ini_file("HTMLObjectSavePause",parameters);
	};
	unpause_toontalk();
	tt_screen->display(TRUE);
	tt_main_window->Paint();
	if (from_dialog != NULL && strcmp(from_dialog,"3") == 0) {
		throw(0); // could signal different things someday...
	};
};
#if TT_POST3187
int bytes_read_previously = -1;
#endif

boolean ask_response_to_copy_url_interruption(int bytes_read) {
#if TT_POST3187
	if (bytes_read == bytes_read_previously) {
		return(TRUE); // new on 240707 to avoid repeatedly asking the same question
	};
#endif
	bytes_read_previously = bytes_read;
	char parameters[100]; 
	strcpy(parameters,"count;");
	itoa(bytes_read,parameters+strlen(parameters),10);
	pause_toontalk(FALSE,FALSE,FALSE);
	string from_dialog;
	from_dialog = show_html_dialog_named_in_ini_file("HTMLReadURLPause",parameters);
	unpause_toontalk();
	tt_screen->display(TRUE);
	tt_main_window->Paint();
	boolean result = (from_dialog == NULL || strcmp(from_dialog,"3") != 0);
	if (from_dialog != NULL) delete [] from_dialog; // fixed this tiny leak on 040205
	return(result);
};

#endif

boolean copy_string_to_file(string value, long string_length, string file_name) { // copied from StartTT.cpp on 290105
   HANDLE file_handle = CreateFile(file_name,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if (file_handle == INVALID_HANDLE_VALUE) {
#if TT_DEBUG_ON
		int error = GetLastError();
		tt_error_file() << "Could not open " << file_name << " to write. Error code = " << error << endl;
#endif
		return(FALSE);
	};
   DWORD bytes_written;
   ::WriteFile(file_handle,value,string_length,&bytes_written,NULL);
   CloseHandle(file_handle); 
   return(bytes_written == string_length);
};

boolean is_truck_outside(Sprite *sprite, void *ignore_arg) {
	// makes sure that the truck isn't just an inside truck left on the grass
	return(sprite->kind_of() == TRUCK && ((Truck *) sprite)->is_outside());
};

boolean any_trucks_outside() { // new on 130205
	return(tt_city->predicate_true_of_any_extras(is_truck_outside,NULL));
};

#if TT_FUNCTION_KEY_HELP
void update_function_key_help() { // new on 280205
	if (tt_function_key_help != NULL) {
//		city_scale scale = tt_screen->current_scale();
//		double non_ground_adjustment = (double) scale/ground_scale;
		// make it as wide as the screen and on the top
		//city_coordinate good_width,good_height;
		//tt_function_key_help->good_size(good_width,good_height);
//		tt_function_key_help->set_width_maintain_proportionality((city_coordinate) (tt_screen->viewing_width()*non_ground_adjustment));
		TTRegion *screen_region = tt_screen->viewing_region();
		tt_function_key_help->set_width_maintain_proportionality(tt_screen->viewing_width());
		tt_function_key_help->move_to(screen_region->min_x,screen_region->max_y-tt_function_key_help->current_height()); 
		// (city_coordinate) ((tt_screen->viewing_height()-tt_function_key_help->current_height())*non_ground_adjustment));
		tt_function_key_help->set_graspable(FALSE);
		tt_function_key_help->set_selectable(FALSE);
//		tt_function_key_help->set_priority(min_long);
	};
};
#endif

//boolean current_wide_text_with_insertion_character(int insertion_point, wide_string wide_text, int text_length, 
//																	wide_string &output_text, int &output_text_length) { // new on 090604
//	if (insertion_point >= 0) {
//		output_text_length = text_length+1; // need one more for insertion character
//		output_text = new wide_character[output_text_length+1];
//		for (int i = 0; i < insertion_point; i++) {
//			output_text[i] = wide_text[i]; // copy first part
//		};
//		if ((tt_current_time/500)%2 == 0) { // every half second
//			output_text[insertion_point] = L'|';
//		} else {
//			output_text[insertion_point] = L' ';
//		};
//		for (int i = insertion_point; i < output_text_length; i++) {
//			output_text[i+1] = wide_text[i]; // copy rest
//		};
//		output_text[output_text_length] = NULL; // terminate string
//		return(TRUE);
//	} else {
//		output_text = wide_text; // new on 090604
//		output_text_length = text_length;
//		return(FALSE);
//	};
//};


//#endif

//void delete_all_files_in_directory(string directory) { // new on 291003
//	// tried to be safe and to the recycle bin rather than really deleted but can't access the recycle bin that fashion - 
// SHGetFolderPath returns error 183
////	char recycle_bin_path[MAX_PATH];
//	char directory_wild_cards[MAX_PATH];
//	strcpy(directory_wild_cards,directory);
//	strcat(directory_wild_cards,"*.*");
////	if (!get_special_folder(CSIDL_BITBUCKET,recycle_bin_path)) {
////#if TT_DEBUG_ON
////		int error_code = GetLastError();
////		tt_error_file() << "Failed to get the path to the Recycle Bin. Error code = " << error_code << endl;
////#endif
////	};
//	SHFILEOPSTRUCT file_operation;
//	ZeroMemory(&file_operation,sizeof(file_operation));
//	file_operation.hwnd = tt_main_window->get_handle();
////	file_operation.wFunc = FO_MOVE;
//	file_operation.wFunc = FO_DELETE;
//   file_operation.pFrom = directory_wild_cards;
//   file_operation.pTo = NULL; // recycle_bin_path;
//   file_operation.fFlags = FOF_ALLOWUNDO|FOF_NOCONFIRMATION|FOF_NOCONFIRMMKDIR|FOF_NOERRORUI;
//   //file_operation.fAnyOperationsAborted;
//   //file_operation.hNameMappings;
//   //file_operation.lpszProgressTitle;
//	if (SHFileOperation(&file_operation) != 0) {
//		int error_code = GetLastError();
//		tt_error_file() << "Something went wrong moving " << directory << " to the recycle bin. Error code = " << error_code << endl;
//	};
//}; 

// commented out on 060202 since replaced with a more general version Screen::off_screen
//boolean off_screen(city_coordinate llx, city_coordinate lly, city_coordinate adjusted_width, city_coordinate adjusted_height) {
//	// inline??
//	return (llx > ideal_screen_width || lly > ideal_screen_height ||
//		     llx+adjusted_width < 0 || lly+adjusted_height < 0);
//};

/*
color alter_color(color color, int *changes) {
	int red = GetRValue(color)+changes[red_index];
	int green = GetGValue(color)+changes[green_index];
	int blue = GetBValue(color)+changes[blue_index];
	if (red < 0) {
		green -= red;
		blue -= red;
		red = 0;
	} else if (red > 255) {
		int difference = red-255;
		green -= difference;
		blue -= difference;
		red = 255;
	};
	if (green < 0) {
		red -= green;
		blue -= green;
		green = 0;
	} else if (green > 255) {
		int difference = green-255;
		red -= difference;
		blue -= difference;
		green = 255;
	};
	if (blue < 0) {
		red -= blue;
		green -= blue;
		blue = 0;
	} else if (blue > 255) {
		int difference = blue-255;
		red -= difference;
		green -= difference;
		blue = 255;
	};
	return(RGB(red,green,blue));
};
*/
/* better to use methods since this doesn't maintain leaders list and the like
boolean assign_sprite(SpritePointer x, SpritePointer y) { // new on 100901
	// makes x have the same bindings as y
	if (x == NULL || y == NULL || x->kind_of() != y->kind_of()) return(FALSE);
	switch (x->kind_of()) { 
		case CUBBY:
			{
			Cubby *cubby = (Cubby *) x;
			Cubby *other_cubby = (Cubby *) y;
			*cubby = *other_cubby; 
//			x = cubby;
			};
			break;
		case PICTURE:
			{
			Picture *picture = (Picture *) x;
			Picture *other_picture = (Picture *) y;
			*picture = *other_picture; 
			};
			break;
		default:
			*x = *y; // as Sprites
			};
	return(TRUE);
};
*/
// following found in Microsoft documentation to work around problem of printing 64-bit integers
// but I still get compiler warnings trying to use it
/*
#include<iostream>
using namespace std;

std::ostream& operator<<(std::ostream& os, __int64 i )
   {
       char buf[20];
       sprintf(buf,"%I64d", i );
       os << buf;
       return os;
   }
*/

//wide_string read_old_string(InputStream *pad_in, int length) {
//	ascii_string ascii_contents = new char[length+1];
//	for (int i = 0; i < length; i++) {
//		ascii_contents[i] = pad_in->get();
//	};
//	ascii_contents[length] = '\0';
//	wide_string contents = copy_wide_string(ascii_contents);
//	delete [] ascii_contents;
//	return(contents);
//};

/*
   string short_file_name = new char[32];
   output_string_stream short_name(short_file_name,32);
   short_name << very_short_name << "." << extension;
   short_name.put((char) 0);
   string java_dir_file_name = new_file_name("Java",very_short_name,extension);
   string media_dir = ini_entry("Directories",directory_key);
   if (media_dir == NULL) {
      tt_error_file() << "Can't find an entry in ToonTalk.ini for " << directory_key << " in Directories." << endl;
      return(NULL);
   };
   char media_dir_file_name[260];
   OFSTRUCT file_struct;
   if (OpenFile(java_dir_file_name,&file_struct,OF_EXIST) == -1) { // see if it exists on GIF dir
      output_string_stream media_file_name(media_dir_file_name,sizeof(media_dir_file_name));
      media_file_name << media_dir << short_file_name;
      media_file_name.put((char) 0);
      if (OpenFile(media_dir_file_name,&file_struct,OF_EXIST) == -1) {
         tt_error_file() << "Can't find file " << media_dir_file_name << " for Java." << endl;
         return(NULL);
      };
      copy_lz_file(media_dir_file_name,java_dir_file_name);  // works whether compressed or not and both 16 and 32 bit windows
   };
#if TT_INSTALL_SHIELD2
   if (java_dir != NULL) delete [] java_dir;
#else
   delete [] java_dir_file_name;
#endif
   if (media_dir != NULL) delete [] media_dir;
   return(short_file_name);
};
*/
/*
// put these elsewhere or look for pre-defined constants
float const PI = 3.14159;
float const RADIAN = 57.296;

int arc_tan(city_coordinate delta_x, delta_y) {
	// return(180+atan2(delta_y, -delta_x)*RADIAN)
	// orientation should be between 0 and 360
*/

/*
void generic_drwimage(string image, int width, int height) {
   fg_box_t bounding_box;
   fg_make_box(bounding_box,
          tt_memory_graphics_mark_x+1,tt_memory_graphics_mark_y-1,
			 tt_memory_graphics_mark_x+width,tt_memory_graphics_mark_y+height-2);
   fg_writeboxi(bounding_box,image,FG_BLACK);
//   fg_writebox(bounding_box,image);
};
*/

/*
void put_int(int i, output_file_stream &stream) {
// Writing a two byte integer
	stream.put(i>>8);
   stream.put(i&0xff);
};

void put_long(long i, output_file_stream &stream) {
// Writing a four byte integer
	stream.put(i>>24);
	stream.put(i>>16&0xff);
   stream.put(i>>8&0xffff);
   stream.put(i&0xffffffL);
};

int get_int(input_file_stream &stream) {
// Reading a two byte integer
   register int temp;
	unsigned char ch;
   stream.get(ch);
   temp=ch<<8;
   stream.get(ch);
   return(ch | temp);
};

long get_long(input_file_stream &stream) {
// Reading a four byte integer
   register long temp;
	unsigned char ch;
	stream.get(ch); // most significant digit first
	temp = ch<<24;
   stream.get(ch);
   temp = temp|(ch<<16);
   stream.get(ch);
   temp = temp|(ch<<8);
   stream.get(ch);
   return(ch | temp);
};

*/

/*
void pause_sound() {
#if TT_DEBUG_ON
   if (tt_debug_mode == 999) {
      tt_error_file() << "Sound paused at " << tt_frame_number << endl;
   };
#endif
   sndPlaySound(NULL,NULL);
};

void resume_sound() {
   if (current_repeating_sound != 0) {
#if TT_DEBUG_ON
      if (tt_debug_mode == 999) {
         tt_error_file() << "Sound " << current_repeating_sound << " resumed at " << tt_frame_number << endl;
      };
#endif
      play_sound(current_repeating_sound,current_sound_priority,TRUE);
   };
};
*/

//void change_video_mode(int increment) {
//   int new_mode =  // increment may be negative so add num in case
//      (number_of_video_modes+video_mode_index+increment)%number_of_video_modes;
//   tt_setmode(video_modes[new_mode]);
//};

//void check_fail(string conditional, string file, int line_number) {
//  tt_error_file() << "Check failed: " << conditional << " "
//              << "in file: " << file << " on line: " << line_number << endl;
//  tt_exit_failure();
//};

//void tt_heap_overflow() {
//  tt_error_file() << "Heap overflow. (Not caught by new_handler.)" << endl;
//  tt_exit_failure();
//};

//#if TT_XML
//void print_string_as_xml_string(string text, output_stream &stream) {
//   // new on 020802 to deal with ToonTalk strings containing angle brackets and the like
//   if (text == NULL) return;
//   int text_length = strlen(text);
//   for (int i = 0; i < text_length; i++) {
//      switch (text[i]) {
//         case '<':
//            stream << "&lt;";
//            break;
//         case '>':
//            stream << "&gt;";
//            break;
//         case '&':
//            stream << "&amp;";
//            break;
//         default:
//            stream.put(text[i]);
//      };
//   };
//};
//#endif





