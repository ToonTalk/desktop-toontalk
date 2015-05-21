// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif   
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif   
#if !defined(__TT_CLEANUP_H)   
#include "cleanup.h"
#endif   
#if !defined(__TT_USER_H)   
#include "user.h"
#endif 
#if !defined(__TT_MAIN_H)   
#include "main.h"
#endif 
#if !defined(__TT_HELP_H)   
#include "help.h"
#endif
#if !defined(__TT_SPEAK_H)   
#include "speak.h"
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
#if !defined(__TT_PUZZLE_H)
#include "puzzle.h"
#endif
#if !defined(__TT_CITY_H)
#include "city.h"
#endif
#if !defined(__TT_PRGRMMR_H)
#include "prgrmmr.h"
#endif
#if !defined(__TT_MARTIAN_H)
#include "martian.h"
#endif
#if !defined(__TT_PICTURE_H)
#include "picture.h"
#endif
#if !defined(__TT_PAD_H)
#include "pad.h"
#endif
#if !defined(__TT_TEXT_H)
#include "text.h"
#endif
#if !defined(__TT_CUBBY_H)
#include "cubby.h"
#endif
#if !defined(__TT_DIRTY_H)
#include "dirty.h"
#endif
#if !defined(__TT_LOG_H)   
#include "log.h"
#endif 
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif   

#include <string.h>	
#include <stdlib.h>
//#include <strstrea.h> commented out 210703
#include <windows.h>
#include <mmsystem.h>

#include <io.h> // to get the file length

#if TT_ENHANCED_DRAG_AND_DROP
#if !defined(__TT_DRAGDROP_H)
#include "dragdrop.h"
#endif
#endif

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* implements logging of
		  - user events (keyboard, joystick and mouse)
		  - reading the clock
        - user profile
*/

const int number_of_digits = 5; // in log files - e.g. log00099.dmo

time_t date_and_time_log_created = 0; // number of seconds between log creation and Jan 1, 1970
millisecond time_log_started = 0; // tt_current_time when this log loaded

output_file_stream log_out;
InputStream *log_in = NULL; // wasn't initialized prior to 050404
HFILE speak_file = -1;
const int speak_priority = 999;

//boolean tt_logging = FALSE; // moved to globals.cpp on 300104

int last_closed_log_segment = -2; // new on 220703 to optimize successive log segments

inline boolean running_successive_demo() { 
	return(tt_ok_to_optimize_successive_demos && last_closed_log_segment+1 == tt_current_log_segment);
};

boolean log_in_is_open() { // new on 050404
	return(log_in != NULL);
};

#if TT_ALPHA_FEATURE
void restore_time_travel_buttons();
void repair_time_travel_button_animations();
static boolean time_travel_buttons_hidden = FALSE;
#endif

ascii_string add_extension(const_ascii_string log_file_name, boolean input, const_ascii_string extension, boolean ok_to_bother_user=TRUE);

boolean recording_paused = FALSE; // new on 140204 since don't want to finalize log if paused and resumed

void begin_pointing_cursor();

//Parameters *original_parameters;

Parameters::Parameters() {
  video_mode = video_mode_index(tt_video_mode);
  programmer_head = (char) tt_programmer_head;
  if (tt_user_name != NULL) { 
//	  memcpy(user_name,tt_user_name,strlen(tt_user_name)+1);
	  strcpy(user_name,tt_user_name); // simplifed on 200703
  } else {
	  user_name[0] = NULL;
  };
#if TT_LARGE_CITIES
	city_size = (short int) tt_city_width;
#else
  city_size = (short int) tt_city_size;
#endif
  character_size_percent = (short int) tt_character_size_percent;
  help_speed_percent = (short int) tt_help_speed_percent;
  mouse_button_count = (char) tt_mouse_button_count;
  maintain_history = (char) tt_maintain_history;
  minimum_say_importance = (char) tt_minimum_say_importance;
  serial_port_to_open = (char) tt_serial_port;
  user_is_debugger = (char) tt_user_is_debugger;
  temporary_user = (char) tt_temporary_user;
  sound_option = (char) tt_sound_option;
  speed = (short int) tt_speed;
  sleep = (short int) tt_sleep;
  using_dispdib = old_dispdib_value();
  marty_talk = (char) tt_marty_talk_to_save;
  load_items_version_number = latest_version_number;
  puzzle_counter = min_short; // default -- means no puzzles
  if (tt_puzzle_file_name != NULL) {
     short int first_digit, last_digit;
     if (first_digits_of_puzzle_name(first_digit,last_digit)) {
        char saved_char = tt_puzzle_file_name[last_digit+1];
        tt_puzzle_file_name[last_digit+1] = '\0'; // terminate it for the following call
        puzzle_counter = (short int) atoi(tt_puzzle_file_name+first_digit);
		  if (tt_user_puzzles) puzzle_counter = -puzzle_counter; // on 300399 user puzzles save negative of number
        tt_puzzle_file_name[last_digit+1] = saved_char;
     };
  };
//  expansion3 = min_short; // unlikely value
  xml_version = xml_version_number;
  expansion4 = min_short;
};

#if TT_32
const int command_count = 38;
#else 
const int command_count = 39;
#endif

void Parameters::install_parameters(long last_use) {
   boolean options_changed = FALSE; // true if changed by clicking on Set Options and chosing 3rd first choice
   if (last_use > 0 && tt_time_travel == TIME_TRAVEL_OFF && !replaying()) { // last conjunct added 310100
      ascii_string options_last_changed_string = ini_entry(AC(IDC_DEFAULTS),AC(IDC_LAST_CHANGED));
      if (options_last_changed_string != NULL) {
         long options_last_changed = atol(options_last_changed_string);
         if (options_last_changed > last_use) {
				options_changed = TRUE;
			};
         delete [] options_last_changed_string;
      };
   };
	ascii_string command_line[command_count];
   int i;
	for (i = command_count-1; i >= 0; i -= 2) {
		command_line[i] = new char[256];
	};
	i = command_count-1;
   int arg_count = 0;
   if (options_changed) {
      video_mode = video_mode_index(tt_video_mode);  // use new options
   } else {
	   itoa(video_mode_number(video_mode),command_line[i--],10);
	   command_line[i--] = "-v";
      arg_count += 2;
   };
	itoa(programmer_head,command_line[i--],10);
	command_line[i--] = "-h";
   arg_count += 2;
	if (user_name != NULL && user_name[0] != NULL) { // second condition added 200703
		strcpy(command_line[i--],user_name); // not a great solution
		command_line[i--] = "-n";
      arg_count += 2;
	};
	itoa(city_size,command_line[i--],10);
	command_line[i--] = "-c";
	itoa(character_size_percent,command_line[i--],10);
	command_line[i--] = "-l";
   arg_count += 4;
   if (options_changed) {
       help_speed_percent = tt_help_speed_percent;
       mouse_button_count = tt_mouse_button_count;
   } else {
   	itoa(help_speed_percent,command_line[i--],10);
	   command_line[i--] = "-r";
	   itoa(mouse_button_count,command_line[i--],10);
	   command_line[i--] = "-b";
      arg_count += 4;
   };
	itoa(maintain_history,command_line[i--],10);
	command_line[i--] = "-u";
	itoa(minimum_say_importance,command_line[i--],10);
	command_line[i--] = "-a";
	itoa(serial_port_to_open,command_line[i--],10);
	command_line[i--] = "-p";
	itoa(sleep,command_line[i--],10);
	command_line[i--] = "-y";
	itoa(speed,command_line[i--],10);
	command_line[i--] = "-q";
	itoa(user_is_debugger,command_line[i--],10);
	command_line[i--] = "-k";
   arg_count += 12;
   if (options_changed) {
      using_dispdib = old_dispdib_value();
      sound_option = tt_sound_option;
      marty_talk = (char) tt_marty_talk;
   } else {
//   	itoa(using_dispdib,command_line[i--],10);
//	   command_line[i--] = "-f"; // in order to override this commented out on 141299
	   itoa(sound_option,command_line[i--],10);
	   command_line[i--] = "-s";
      itoa((marty_talk == MARTY_SPEAKS || marty_talk == MARTY_SPEAKS_AND_TALK_BALLOONS || marty_talk == MARTY_SPEAKS_WITH_SUBTITLES),command_line[i--],10);
      command_line[i--] = "-speak";
      itoa((marty_talk != MARTY_SPEAKS && marty_talk != MARTY_SPEAKS_WITH_SUBTITLES && marty_talk != MARTY_WITH_SUBTITLES),command_line[i--],10); // added MARTY_WITH_SUBTITLES on 110401
      command_line[i--] = "-balloon";
      arg_count += 6; // was 8 prior to 141299
   };
   if ((puzzle_counter > 0 || (puzzle_counter < 0 && puzzle_counter != min_short)) && replaying() != NO_REPLAY) { // added parens on 030300 so this applies only if tt_replay
		// allow negative values starting 300399
      itoa(puzzle_counter,command_line[i--],10);
      command_line[i--] = "-puzzle";
		arg_count += 2; // missing prior to 041199
   };
   itoa(temporary_user,command_line[i--],10);
	command_line[i] = "-t";  // i-- is a useless assignment
   arg_count += 2;
#if !TT_32
   arg_count++;  // wants to be odd
   i--;
#endif
   interpret_command_line(arg_count,command_line+i); // ,FALSE); // arg_count was command_count-i
   tt_marty_talk_to_save = (MartyTalk) marty_talk;
	for (i = command_count-1; i >= 0; i -= 2) {
		delete [] command_line[i];
	};
};

millisecond *speak_starts = NULL; // when the next narration piece starts
millisecond *m25_speak_starts = NULL; // high res
millisecond *m22_speak_starts = NULL; // low res
long *speak_sizes = NULL; // size of each speak
const short int max_speak_count_ever = 256;
short int max_speak_count = max_speak_count_ever;
short int speak_index = 0;
sound_buffer next_speak = NULL;
sound_buffer previous_speak = NULL; // new on 170300
long next_speak_size = 0;

HGLOBAL next_speak_memory_handle = NULL; // ignored by 32bit code
ascii_string speak_file_name = NULL;
boolean speak_starts_changed = FALSE;

ascii_string subtitles_file_name = NULL;

void update_subtitles_file_name(ascii_string log_file_name) {
#if TT_ALPHA_FEATURE
	if (subtitles_file_name != NULL) return; // new on 061103
#else
   if (subtitles_file_name != NULL) delete [] subtitles_file_name;
#endif
   ascii_string subtitles_extension = ini_entry(AC(IDS_FILE_EXTENSIONS),AS(IDS_SUBTITLES_EXTENSION),FALSE);
   if (subtitles_extension == NULL) {
      subtitles_extension = ASC(IDS_SUBTITLES_FILE_SUFFIX);
   };
   subtitles_file_name = add_extension(log_file_name,TRUE,subtitles_extension,FALSE);
   delete [] subtitles_extension;
};

void release_subtitles_file_name() {
	if (subtitles_file_name != NULL) delete [] subtitles_file_name;
};

void log_write(bytes buffer, int length) { // abstracted and made archive version on 250603
#if TT_DEBUG_ON
	if (tt_dumping_background != NULL) {
		log("Shouldn't be adding to the log while in the XML phase of dumping a city - log_write");
	};
#endif
	if (TT_WRITE_ZIP_FILE_INCREMENTALLY && tt_log_out_archive != NULL) {
		write_zip_file(tt_log_out_archive,buffer,length);
	} else {
	   log_out.write((string) buffer,length);
	};
};

byte buffer[1];

void log_put(byte c) { // abstracted and made archive version on 250603
#if TT_DEBUG_ON
	if (tt_dumping_background != NULL) {
 		log("Shouldn't be adding to the log while in the XML phase of dumping a city - log_put");
	};
#endif
	if (TT_WRITE_ZIP_FILE_INCREMENTALLY && tt_log_out_archive != NULL) {
		buffer[0] = c; // is there a better way?
		write_zip_file(tt_log_out_archive,buffer,1);
	} else {
	   log_out.put(c);
	};
};

void log_string(string s, long true_length=-1);

void log_string(string s, long true_length) {
#if TT_DEBUG_ON
	if (tt_dumping_background != NULL) {
		log("Shouldn't be adding to the log while in the XML phase of dumping a city - log_string");
	};
#endif
	if (!TT_WRITE_ZIP_FILE_INCREMENTALLY || tt_log_out_archive == NULL) {
		write_string(log_out,s,true_length); // old version
		return;
	};
	if (s == NULL) { // new on 110601
		true_length = 0;
	} else if (true_length < 0) {
		true_length = strlen(s)+1; // include terminating NULL
   } else {
      true_length++;
	};
	write_zip_file(tt_log_out_archive,(bytes) s,true_length);
};

//void log_in->read(bytes buffer, int length) { // abstracted and made archive version on 250603
//	if (tt_log_in_archive != NULL) {
//		read_current_file_in_unzip_archive(tt_log_out_archive,buffer,length);
//	} else {
//	   log_in.read(buffer,length);
//	};
//};
//
//byte log_in->get() {
//	if (tt_log_in_archive != NULL) {
//		read_current_file_in_unzip_archive(tt_log_out_archive,buffer,1);
//		return(buffer[0]);			
//	} else {
//		return(log_in->get()); 
//	};
//};

//string FileInputStream::read_string(string s) {
//	return(::read_string(stream,s));
//};

void close_log() {
	if (tt_log_out_archive != NULL) {
		do_postponed_include_files_xml(); // if all went well there shouldn't be any left - moved here on 180703 so it happens regardless of how the log is closed
#if TT_WRITE_ZIP_FILE_INCREMENTALLY
		close_zip_file(tt_log_out_archive);
		close_zip_archive(tt_log_out_archive); // so it can be read -- not clear if needed for all zip/unzip libraries - was for zlib one
		tt_log_out_archive = NULL;
#else
		// close it and then zip/delete it
		log_out.close();
		zip_file(tt_log_out_archive,tt_log_out_file_name,NULL,default_compression_level,TRUE);
		update_counters(); // moved here on 190703 since finished a segment (and abstracted)
		// following new on 240105 - so screen resolution is available for dragging DMO files to ToonTalk
		char counters_xml[MAX_PATH];
		strcpy(counters_xml,tt_user_directory);
		strcat(counters_xml,"counters.xml");
		zip_file(tt_log_out_archive,counters_xml);
//		tt_log_out_archive_size++;
#endif
	} else {
		log_out.close();
	};
	write_ini_entry("User","OpenDemoFileName","");
};

int reported_log_initialization_error = 0;

boolean already_noted_that_city_saving_is_taking_too_long = FALSE;

millisecond close_log_and_open_next() {
	// updated on 290903 to return the amount of time this took if time travel is enabled
	if (replaying()) return(0); // don't re-log old log
	if (tt_logging) {
		if (tt_time_between_new_logs <= 0) return(0); // just keeping one big log
		if (tt_programmer->kind_of() == PROGRAMMER_TITLES) { // added 161299 - still in titles so no need to break here
			return(0);
		};
		close_log(); // close the current one first
#if TT_DEBUG_ON
		if (tt_debug_mode == 140103) {
			tt_error_file() << "Just closed " << tt_log_out_file_name 
								 << " that was open for output. [1] at " << tt_current_time << " on frame " << tt_frame_number << endl;
		};
#endif
		delete [] tt_log_out_file_name;
	};
#if TT_NO_ROBOT_TRAINING
	return(0); // new on 091205
#endif
	if (tt_user_directory == NULL) {
		compute_tt_file_name();
	};
	tt_log_out_file_name = maintain_n_versions_youngest_last(tt_log_count_max); //,tt_current_log_segment); // new on 200799
	log_out_archive(); // make sure this is initialized - new on 190703
//	millisecond start; // replaced by a global variable so that don't count the time in the debugger (or some other dialog)
	if (tt_next_new_log_time > 0) {
		tt_log_initialization_start_time = timeGetTime(); 
		// new on 280803 so time travel intervals don't include the time to dump the city
	};
   if (tt_want_exception_breaks) {
	   log_initialization(tt_log_out_file_name);
   } else {
	   try { // exception handling added 010200
		   log_initialization(tt_log_out_file_name);
	   } catch (...) {
		   if (reported_log_initialization_error == 0) {
			   message_box(MB_ICONSTOP|MB_OK|MB_SYSTEMMODAL,
							   S(IDS_ERRORS_KEEPING_LOG_SEGMENTS,
								  "Sorry, an error occurred while saving things for time travel. Time travel might not work right for this user name."));
		   };
		   reported_log_initialization_error++;
		   if (reported_log_initialization_error > 2) { // happening too often so turn off logging
#if TT_DEBUG_ON
				debug_this();
#endif
			   tt_next_new_log_time = max_long;
			   tt_time_between_new_logs = 0;
		   };
		   close_log();
#if TT_DEBUG_ON
			if (tt_debug_mode == 140103) {
				tt_error_file() << "Just closed " << tt_log_out_file_name << " that was open for output. [2] at " 
									 << tt_current_time << " on frame " << tt_frame_number << endl;
			};
#endif
			if (tt_log_out_archive != NULL) { // conditional new on 170703
				DeleteFile(tt_log_out_file_name);
			};
		   tt_current_log_segment--;
	   };
   };
#if TT_DEBUG_ON
	if ((tt_debug_mode == 110100 || tt_debug_mode == 160299 || tt_debug_mode == 101 || 
		  tt_debug_mode == 120100 || tt_debug_mode == 230803) 
		 && tt_current_time > 0) { // and not too early for first one to use tt_error_file()
		tt_error_file() << "Starting log " << tt_log_out_file_name 
						<< " on frame " << tt_frame_number << " at " << tt_current_time << endl;
	};
#endif
	if (tt_next_new_log_time > 0 && tt_time_between_new_logs > 0) { // second conjunct added 300803
		millisecond delta = timeGetTime()-tt_log_initialization_start_time;
//#if TT_DEBUG_ON
//		if (tt_debug_mode == 280803) {
//			tt_error_file() << "Added " << delta << "ms to tt_next_new_log_time." << endl;
//		};
//#endif
		static int too_long_count = 0; // was 1 prior to 080904
		//if (!already_noted_that_city_saving_is_taking_too_long && delta > 1000 && tt_current_log_segment > 1) { 
		//	too_long_count++;
		//};
		if (delta > tt_maximum_time_travel_overhead && tt_current_log_segment > 1) { // rewritten on 080904
			too_long_count++;
		};
		if (too_long_count == 2) {
			// completely rewritten on
			too_long_count = 0; // reset so can be triggered again
			tt_time_between_new_logs *= 2;
			tt_error_file() << "Time travel is causing ToonTalk to pause for " << delta 
								 << " milliseconds and MaximumPauseDurationDueToTimeTravel is set to " 
								 << tt_maximum_time_travel_overhead << " milliseconds." << endl;
			tt_error_file() << "SecondsBetweenLogs is doubled for this session to " 
								 << tt_time_between_new_logs/1000 << " seconds." << endl;
			tt_err_file_important_but_no_error = TRUE;
		};
//		if (too_long_count > 2) { // new on 131203 that it should take too long a few times before troubling the user
			// moved here on 280803 // removed on 310803: && tt_time_travel != TIME_TRAVEL_OFF
			// should this be more general? - do I still need to test tt_time_travel != TIME_TRAVEL_OFF?
			// added  tt_current_log_segment > 1 on 040903 since can be slow at first
//#if TT_ALPHA_FEATURE
//			trouble_shoot(TROUBLE_SHOOT_TIME_TRAVEL_SLOW,NULL,NULL,TRUE);
//#else
//			log("Time travel is requiring too much time to save things. I recommend turning it off while working on this project.",FALSE,TRUE); // english.rc?
//#endif
//			already_noted_that_city_saving_is_taking_too_long = TRUE;
//		};
//		tt_next_new_log_time += delta;
		// problem with the above is that it confuses the caller when time travelling - beginning 290903 returns delta instead
//#if TT_DEBUG_ON
//		if (tt_debug_mode == 290903) {
//			tt_error_file() << "Just added " << delta << " to tt_next_new_log_time to make it " << tt_next_new_log_time << endl;
//		};
//#endif
		return(delta);
	};
	return(0);
};

void log_initialization(ascii_string log_file_name) {
	boolean first_time = !tt_logging;
	tt_logging = TRUE;
	ascii_string new_log_file_name;
	if (first_time) {
		new_log_file_name = add_demo_extension(log_file_name,FALSE);
	} else {
		new_log_file_name = log_file_name;
	};
	if (new_log_file_name == NULL) {
		tt_error_file() << S(IDS_UNABLE_TO_OPEN_LOG_FILE) << log_file_name << end_sentence();
#if TT_DEBUG_ON
		debug_this();
#endif
		tt_logging = FALSE;
		tt_time_between_new_logs = 0;
		tt_next_new_log_time = max_long;
		return;
   } else {
      tt_main_window->release_last_clipboard_string(); // new on 111102 so logs don't assume previous clipboard reading
		UnwindProtect<boolean> set(tt_user_initiated_dump,FALSE); // added on 060200
		if (TT_WRITE_ZIP_FILE_INCREMENTALLY && tt_log_count_max != 0) { // new on 250603 to keep time travel in a zip archive -- was > 0 prior to 280803
			open_zip_file(log_out_archive(),log_file_name);
//			tt_log_out_archive_size++;
		} else {
//			log_out.close(); // experiment 190703
			log_out.open(new_log_file_name,std::ios_base::out|std::ios_base::binary|std::ios_base::trunc); 
			// added trunc to delete current if it exists
			if (log_out == NULL) { // new on 290904 for error recovery
				unable_to_write_file(new_log_file_name); 
				return;
			};
		};
#if TT_DEBUG_ON
		if (tt_debug_mode == 140103 || tt_debug_mode == 101) {
			tt_error_file() << "Just opened " << new_log_file_name << " for output at " << tt_current_time 
								 << " on frame " << tt_frame_number << endl;
		};
#endif
	   // start with magic cookie and version number for ToonTalk demo files
//	   ascii_string cookie = AC(IDS_DEMO_FILE_COOKIE); // copy_string("TnTkD000");
		char cookie[24]; // rewritten on 070604 to avoid the overly costly call AC
		strcpy(cookie,"TTDMO000");
		// tt_log_version_number might not be the most current if redoing an old demo
		// revised on 070604 to use latest_log_version_number since more important that time travel work across versions than
		// supporting rebuilding an old demo
	   if (latest_log_version_number < 100) {
		   itoa(latest_log_version_number,cookie+6,10);
		} else {
         itoa(latest_log_version_number,cookie+5,10);
		};
		log_write((bytes) cookie,8);
//	  delete [] cookie;
     // version 4 started on December 2, 1996.
     // version 5 started 4/7/97
     // version 6 started for saving tools favorite size and positions
     // version 7 started 5/15/97 to improve collision detectors
     // version 8 was needed to deal with the extra synthetic pictures in the picture notebook 6/30/97
     // version 9 increases the maximum speed of the programmer in different states
     // version 10 introduces sitting on the lawn and different spots on the floor
     // version 11 all the demos were re-recorded
     // version 12 minor aesthetic improvements ...
	  // version 13 more minor aesthetic improvements (3/18/98)
	  // version 14 more minor aesthetic improvements including Bammer to join boxes and robots (5/18/98)
	  // version 15 more minor aesthetic improvements including exactly proportionate growth of thought bubble and contents when training (6/12/98)
	  // version 16 logs now save keyboard accelerator bindings, starting city, user profile, clipboard input, (200799)
	  // version 17 when sitting or standing up animations finish before the transition
	  // version 18 many changes including pixel level arithmetic better, including ini_entry, tt_current_time, and speech_has_finished entries
		// (241199)
	  // version 19 fixes a problem with saving the state of flying helicopter exactly right (171299)
	  // version 20 starts afresh due to so many changes including time travel
	  // version 21 deal correctly with puzzle logs and starting from saved cities and SpokenLanguage and Wide Characters
	  // version 22 minor changes (220300)
	  // and Marty giving help after beaming in for a different purpose
	  // version 23 objects bammed are always destroyed even if dropped on an erased object
	  // version 24 saves mouse deltas so the mouse sensor isn't influenced by the arrow keys in logs (051100)
	  // version 25 lots of changes prior to product release 2 (beta 26 or 2.34) - 170201
	  // version 26 LeftAndRightMouseButtonsDownMeansEscape and EXE name are new as of 170301
	  // version 27 Stable selection while holding Dusty or Pumpy 220901 (and recording of some new ini switches)
     // version 28 deals with absolute mouse mode as of 220402 (maybe a bit more done earlier but not recorded)
     // version 29 deals with one click per use for Dusty and Pumpy
	  // version 30 deals with some new ini_entries
	  // version 31 uses a better random number generator
	  // version 32 new on 090703 - 32-bit length for strings and support for zipped dmos
	  // version 33 new on 030204 - no scrolling toontalk ini option (and a few days late - stopped finish_instantly when scrolling)
	  // version 34 new on 140204 and uses internal hashed file names for files where pasted in 
	  // version 35 new on 160204 so if you drop something on a running robot it is ignored
	  // version 36 new on 220204 so even if a city is full robots don't abort and trucks keep trying
	  // and now supports double clicking on files and records duration of pauses where ToonTalk wasn't running
	  // version 37 has drop selection that uses the same code as the release code -- 260204
	  // version 38 puts out a zero event counter for robustness - new on 280204
	  // on 230304 tried to introduce version 39 so nothing is recorded while in titles but it is a big job - lots assumes this isn't the case
	  // then restored 39 but narrowed its meaning to no recording or reading of events_counters while in the titles
	  // version 40 new on 030404 for support of tt_match_blank_with_sensor_then_run_once_per_cycle and !tt_robot_drop_location_follows_camera
	  // version 41 new on 050404 to fix problem that set_recordable_ini_options() only was called by non-time travel demos
	  // version 42 fixes a bug with the containment_region being wrong in older demos
	  // version 43 new on 210404 deals better with a drop over 2 things (where the "first" one doesn't accept it)
	  // version 44 new on 030504 supports time travel archives with cities as separate files
	  // version 45 new on 080604 (post 3.77) supports typing partial numbers and arrow keys to text pads
	  // version 46 new on 110604 to deal not move with arrow keys while editing with arrow keys and to have label area when picture is flipped
	  // version 47 new on 010704 to deal with incorrect selection region when a tool jumps into a hand that then moves
	  // version 48 new on 080704 for reincarnation of destroyed friends of Tooly (and Tooly)
	  // version 49 new on 270704 so robots line up and go inert when picked up
	  // version 50 new on 290704 to improve the way uncollide works
	  // version 51 new on 230804 to fix a problem with the millisecond timer on the first frame of a demo 
	  //									(and 160804 changes to displaying cubbies and wand's button)
	  // version 52 new on 130904 to fix the drift of the magic wand (repeated use of space for example)
	  // version 53 new on 260904 to run animations on tt_global_picture when standing up (or switching scenes in general) 
	  //                             and deal with regrasping when a box becomes shorter while held
	  // version 54 new on 081004 and deals with magic wand drift if there is no subject
	  // version 55 new on 121004 and deals with tt_number_to_text_exact 
	  // version 56 implemented on 211004 to deal with the change of 161004 to deal with log segments with a delta of more than 1000 milliseconds
	  // version 57 new on 241004 to fix problems displaying shrinking digits with large non-shrinking parts and 
	  // to deal with geometrically large numbers growing slightly when the value changes
	  // version 58 new on 041104 to prepend 0 to .5 and the like
	  // version 59 new on 121104 to improve the way things held in middle are grasped if their size changes while held
	  // version 60 new on 251104 to deal with vacuuming off a remote looks from the touching who sensor
	  // version 61 new on 301104 fixed a problem where fresh text pads had a strange size 
	  // (depending upon whether Tooly was good size)
	  // version 62 makes sure that Pumpy doesn't make things bigger than can be displayed well
	  // version 63 new on 121204 so that when you scroll on the ground the objects and the programmer end up at the right place
	  // version 64 new on 141204 so that set_to_good_size uses tt_toolbox by default -- 
	  // and 1 letter character pads have a better size if changed
	  // version 65 new on 291204 to deal better with selection when holding a tool that jumped into the programmer's hand
	  // version 66 new on 030105 to deal with random number sensors used by the same team in the same cycle
	  // version 67 new on 110105 to deal with non-integer values for width and height; 
		// and Cubby::really_propagate_changes changes
	  // and the new updating of parts and containers
	  // version 68 new on 150105 to ignore rubout of empty text pad
	  // version 69 new on 300105 to fix a problem where robots get removed from tt_running_robots
	  // version 70 new on 020205 fixes several problems regarding size 
	  // when robots are working while you are in the room or have left the house
	  // version 71 new on 130205 to make confused robots act better
	  // version 72 new on 210205 to make text pads look better
	  // particularly if the robot uses Pumpy
	  // version 73 new on 180405 was to avoid double activation of pictures (reused for avoiding robot double activation 190405)
	  // version 74 new on 010505  -- NOT USED ANYMORE -- 
	  // was introduced so that watched robots don't wait for a truck to nearly build the house 
	  // but just wait until they drive off the floor
	  // version 75 new on 130505 deal with better way of displaying positions as integers
	  // version 76 (like 58) makes -.5 display as -0.5 and 
	  // improved placement when flipping a picture and improved drop locations -- 020605
	  // version 77 new on 140605 to make further improvements to drop locations (and undo one "improvement" made above)
	  // version 78 new on 060705 to deal better with the font size of numbers first seen when carried by a bird
	  // version 79 new on 030805 to support tt_marty_appears_only_if_called
	  // following commented out on 200799 since dumped as part of "history" now
//	  Parameters log_preface;
//	  write_user_parameters(log_out,&log_preface);
//     log_out.write((character *) tt_favorites,favorites_count*sizeof(FavoriteSizeAndLocation));
	  log_first_segment(tt_titles_ended_on_frame > tt_frame_number); // first_time); // new on 180100
	  // following is new on 200799
	  log_put((byte) tt_system_mode);
	  log_string(tt_keyboard_accelerators);
	  log_string(tt_vacuum_keyboard_accelerators);
	  log_string(tt_expander_keyboard_accelerators);
	  log_string(tt_copier_keyboard_accelerators);
	  log_string(tt_user_name);
	  log_string(tt_puzzle_file_name);
	  log_write((bytes) &tt_current_time,sizeof(tt_current_time)); // new on 241199
#if TT_DEBUG_ON
	  if (tt_debug_mode == 221103) {
		  tt_error_file() << "Current time is " << (tt_current_time/1000.0) 
			               << " and segment number is " << tt_current_log_segment << endl;
	  };
#endif
//	  time_log_started = tt_current_time; // moved to when read on 110603
	  log_write((bytes) &tt_frame_number,sizeof(tt_frame_number)); // new on 161299
	  unsigned long seed = random_number_seed();
	  log_write((bytes) &seed,sizeof(seed)); // new on 120100
	  boolean programmer_invisible = tt_programmer->invisible();
	  log_write((bytes) &programmer_invisible,sizeof(programmer_invisible)); // new on 150100
	  boolean stopped = tt_city->stopped();
	  log_write((bytes) &stopped,sizeof(stopped)); // new on 150100
#if TT_DEBUG_ON
	  long debug_counter = tt_counter_for_debugging; // return_debug_counter();
#else
	  long debug_counter = 0;
#endif
  	  log_write((bytes) &debug_counter,sizeof(debug_counter)); // new on 150100
	  dump_keep_every_data(log_out); // new on 180100
	  date_and_time_log_created = seconds_since_1970();
	  log_write((bytes) &date_and_time_log_created,sizeof(date_and_time_log_created));
#if TT_DEBUG_ON
	  if (tt_debug_mode == 290903) {
		  tt_error_file() << date_and_time_log_created << " at " << tt_current_time << " for segment " << tt_current_log_segment <<  " on frame " << tt_frame_number << endl;
	  };
#endif
//	  log_write((bytes) &tt_martian_automatically_appears,sizeof(tt_martian_automatically_appears));
	  log_put(tt_martian_automatically_appears); // experiment on 020703
	  flag wide_characters = need_wide_characters(); 
	  log_put(wide_characters); // new on 170300
	  log_put((byte) tt_alternative_spoken_language); // new on 170300
	  log_string(AC(IDC_TOONTALK_EXE_NAME)); // new on 170301
	  leave_room_for_growth(log_out,4); // new on 241199
	  // since growth_block_length is zero long nothing here just now
	  dump_history(log_out); // dumped after being loaded in winmain
	  if (tt_system_mode != PUZZLE) { // introduced conditional on 160300 since puzzle don't need this stuff
		  // commented out on 120603
		  string notebook_zip_files = NULL; // new on 171203
		  if (tt_toolbox != NULL) {
			  // branch restored on 171203 and added the following
			  notebook_zip_files = zip_current_notebook_files();
		  };
		  if (notebook_zip_files[0] != NULL) {
			  log_put(ZIP_ARCHIVE_MARKER);
			  log_string(notebook_zip_files); // not a leak since reuses same string
//		    record_notebook(pointer_to_toolbox_notebook());
		  } else {
			  log_put(NOTHING_MARKER); // new on 161299
		  };
	  };
//	  if (!first_time || tt_debug_mode != 250100) { // new on 271199 -- commented out on 240105 since is always TRUE -- don't use -d 250100 anymore
		if (tt_system_mode != PUZZLE) { // introduced conditional on 160300 since puzzle don't need this stuff
			// check the clipboard because that is what happens if first log
			// and might playback from the other than the first
			boolean new_text;
			string ignore = tt_main_window->clipboard_text(new_text,NULL,TRUE,TRUE); 
			// new on 211103 to fix a bug recording over the first segement - 
			// by the way why does this read clipboard text when it happens again in this file?		  
			// rewritten on 040803 to ignore within clipboard_text
			if (ignore != NULL && new_text) delete [] ignore;
			dump_city_to_log();
		};
		initialize_button_key_substitutions(); // new on 120100 so can run non-first
//	  };
	};
   if (first_time) delete [] new_log_file_name;
   if (speak_file_name != NULL) delete [] speak_file_name;
   speak_file_name = add_speak_extension(log_file_name,FALSE);
	if (time_travel_enabled() && tt_log_version_number >= 41) { // ordinary demos already do this -- new on 050404
		set_recordable_ini_options();
	};
//  update_subtitles_file_name();
//  delete [] log_file_name;
};

void log_first_segment(boolean flag) {
	log_put((byte) flag); // TRUE if the first time (new on 180100)
};

void dump_city_to_log() {
#if TT_DEBUG_ON
	if (tt_debug_mode == 202) {
		log("Dumping city");
	};
#endif
	// commented out on 120603
	//Notebook *main_notebook = NULL;
	//if (tt_toolbox != NULL) { // new on 210100
	//	main_notebook = pointer_to_toolbox_notebook();
	//	if (main_notebook != NULL) {
	//		main_notebook->set_ok_to_dump(FALSE); 
	// since handled specially by record_notebook - for both space efficiency and so it is immune to later changes
	//	};
	//};
	wait_cursor(); // new on 150103
//	UnwindProtect<boolean> set(tt_dump_as_xml,TRUE); // cities in log must be XML - new on 170903
	// on 040504 decided that this was always a good idea and more robust -- only downside is these demos can't be run by older versions
	//if (tt_current_log_segment == 0 || tt_log_out_archive == NULL) { // non time-travel log
	//   tt_city->dump_to_stream(log_out,(tt_log_out_archive != NULL));
	//} else {
		// beginning 030504 making this a separate file since is often not loaded (when successive demos on replay)
		log_put(FILE_NAME_MARKER);
		char city_file_name[MAX_PATH]; // more than enough
		if (tt_log_out_file_name_specified != NULL) {
			// new on 110704 to always use version 1 for named demos -- e.g. those created with -o name on the command line
			strcpy(city_file_name,"city00001.cty");
		} else {
			file_name_renumbered("city00000.cty",tt_current_log_segment,city_file_name);
		};
		log_string(city_file_name);
		char full_city_file_name[MAX_PATH];
		strcpy(full_city_file_name,tt_extracted_file_temp_directory);
		strcat(full_city_file_name,city_file_name);
		if (tt_titles_ended_on_frame > tt_frame_number) {
			// new on 050805 since city is saved incorrectly during the titles
			tt_dump_city_to_file_name_when_titles_over = copy_string(full_city_file_name);
		} else if (tt_city->dump_to_new_file(full_city_file_name,TRUE,TRUE,FALSE)) { // don't zip it here since zipped below
			// made conditional on 150105
			zip_file(tt_log_out_archive,full_city_file_name); // this could be done later when "there is time"
		};
//	};
	restore_cursor();
	//if (main_notebook != NULL) {
	//	main_notebook->set_ok_to_dump(TRUE); 
	//};
};

void load_city_from_log() {
	if (tt_log_version_number >= 44) { // on 040504 commented out && tt_current_log_segment != 0
		// new on 030504
		if (log_in->get() == FILE_NAME_MARKER) {
			string city_file_name = log_in->read_string(); // could optimize this some day to ignore when running_successive_demo
			if (city_file_name != NULL) {
				if (!running_successive_demo()) {
					// otherwise just ignore the file name
					string full_city_file_name = extract_file_from_archive(city_file_name,tt_log_in_archive);
					if (full_city_file_name != NULL) {
						tt_city->load_city(full_city_file_name,TRUE);
						delete [] full_city_file_name;
					} else {
						tt_error_file() << "Was not able to extract " << city_file_name << " from " << (tt_log_in_archive||"Null") 
											 // updated 080804 since tt_log_in_archive can be NULL if someone clicks on a lognnnnn.dmo file 
											 << ". Segment " << tt_current_log_segment << " skipped." << endl;
						throw(UNABLE_TO_EXTRACT_CITY_FILE);
					};
				};
				if (city_file_name != NULL) { // conditional new on 080804
					delete [] city_file_name;
				};
				return;
			};
			tt_error_file() << "Time travel archive should have included the name of its city file. Segment skipped." << endl;
			throw(LOG_FILE_MISSING_CITY_FILE_NAME);
		};
	};
	wait_cursor(); // new on 150103 -- moved here on 030504
	if (!tt_city->load_city_from_stream(log_in,running_successive_demo())) {
		log("Problems loading a city inside of a demo file.");
	}; // moved here on 030504
	restore_cursor();
#if TT_DEBUG_ON
	if (tt_debug_mode == 101) {
		log("Loaded city from log.");
	};
#endif
};

void wave_name(int i, ascii_string wave_file_name) {
	wave_file_name[0] = 's'; // named s01 ... sn
   if (i < 9) {
    	wave_file_name[1] = '0';
	   itoa(i+1,wave_file_name+2,10); // 1-indexing
   } else {
 	   itoa(i+1,wave_file_name+1,10); // 1-indexing
   };
	strcat(wave_file_name,AS(IDS_DOT_WAV));
};

void new_speak() {
	// marking time for next narration
	if (speak_index >= max_speak_count_ever) {
		play_sound(PLOP_SOUND);
		return;
	};
	millisecond start_time = timeGetTime();
	if (!speak_starts_changed && speak_file != -1) {
		// updating an existing speak file
		millisecond *temp = new millisecond[max_speak_count_ever];
      int i;
		for (i = 0; i < speak_index; i++) {
			temp[i] = speak_starts[i];
		};
		for (;i < max_speak_count_ever; i++) {
			temp[i] = 0;
		};
		delete [] speak_starts;
		speak_starts = temp;
	} else if (speak_starts == NULL) {
		initialize_speak_starts();
	};
	if (!play_next_sound_file()) { // file is missing
		new_speak();
	};
	speak_starts_changed = TRUE;
//   tt_narration_overhead = timeGetTime()-start_time;
};

boolean play_next_sound_file() {
	char wave_file_name[32];
	wave_name(speak_index,wave_file_name);
	speak_starts[speak_index++] = tt_current_time;
	return(play_sound_file(wave_file_name,99)); // high priority
};

boolean narrating_demo() {
  return(speak_file != -1);
};

void speak_now() {
	if (speak_file == -1) return;
	if (next_speak == NULL) { // not pre_fetched
		fetch_next_speak();
	};
	if (next_speak_size > 0) {
#if TT_DIRECTX
      if (next_speak != NULL) play_sound_buffer(next_speak);
		release_previous_speak();
#else
		// stop any other sound and give this very high prioriy
		play_sound_bytes(next_speak,
#if !TT_32
							  next_speak_size,next_speak_memory_handle,
#endif
							  speak_priority);
		next_speak = NULL;
#if !TT_32
      // otherwise release_next_speak may try to double delete this
		next_speak_memory_handle = NULL;
#endif
#endif
	};
	speak_index++;
};

short int speak_index_fetched = -1;

void fetch_next_speak() {
	// retrieving next narration
	if (speak_file == -1) return;
	if (speak_index >= max_speak_count) return;
	if (speak_index_fetched == speak_index) return; // already got it
	if (!(tt_sound_option&SOUND_BIT)) {
	   // sound turned off after log opened
		_lclose(speak_file);
		speak_file = -1;
		return;
	};
	// the following caused a problem because the next narration is fetched 1 second before it is needed
	// on 170300 fixed this leak by keeping previous fetch and releasing it when speaking the current one
//	if (next_speak != NULL) {
//		log("Trying to prefetch when still have old narration.");
//      release_next_speak(); // lost narration this way -- don't understand
//	};
	previous_speak = next_speak;
	speak_index_fetched = speak_index;
	next_speak_size = speak_sizes[speak_index];
	if (next_speak_size == 0) { // skipping this one
		speak_index++;
		fetch_next_speak();
	} else {
#if TT_DIRECTX
      bytes buffer = new unsigned char[next_speak_size];
      _hread(speak_file,buffer,next_speak_size);
      next_speak = load_sound_from_memory(buffer);
      delete [] buffer;
#else
		next_speak = read_sound(speak_file,next_speak_size
#if !TT_32
										,next_speak_memory_handle
#endif
										);
#endif
	};
   if (tt_print_narration_start_times || tt_debug_mode == 3210) { // print out speak starts - 3210 kept for compatibility with old documentation
      for (int i = 0; i < max_speak_count; i++) {
         tt_error_file() << speak_starts[i] << endl;
      };
//      tt_debug_mode = 19;
		tt_exit_at_end_of_log = FALSE; // replaced the above on 070201
   };
};

void log_time(millisecond millisecond_delta) { 
	// changed arg from short unsigned int on 130204 since can have long pauses if paused ToonTalk and resumed - important for CLOCK_REMOTE
#if TT_DEBUG_ON
	if (tt_debug_mode == 40803 || tt_debug_mode == 202 || tt_debug_mode == 140103 || tt_debug_mode == 230304) {
		tt_error_file() << "Logging millisecond delta=" << millisecond_delta << " on frame " << tt_frame_number << endl; 
		// " at file location " << (int) log_in->tellg() << endl;
	};
#endif
	if (millisecond_delta < 0) { // just to be safe
		millisecond_delta = 1;
	};
	short unsigned int short_millisecond_delta = max_unsigned_short;
	while (millisecond_delta >= max_unsigned_short) { // >= so that if the delta is an exact multiple of max_unsigned_short the 0 is also written
		log_write((bytes) &short_millisecond_delta,sizeof(short_millisecond_delta));
		millisecond_delta -= max_unsigned_short;
	};
	short_millisecond_delta = (short unsigned int) millisecond_delta;		
   log_write((bytes) &short_millisecond_delta,sizeof(short_millisecond_delta));
};

// event counter might be replayed differently while I'm debugging so
// lower to risk by adding a token to the log
const unsigned char EVENT_COUNTER_TOKEN = 142;

void log_events_counter(unsigned short int counter) {
//	log_put(EVENT_COUNTER_TOKEN); // obsolete as of 290204 since always output it
	log_write((bytes) &counter,sizeof(counter));
#if TT_DEBUG_ON
   if (tt_debug_mode == 202 || tt_debug_mode == 101 || tt_debug_mode == 230304) {
      tt_error_file() << "Recording event counter = " << counter << " on frame " << tt_frame_number << endl;
   };
#endif
};

byte encode_event_flags(boolean event) {
   byte result = 0;
   if (event) result |= INPUT_EVENT_FLAG;
   if (tt_hand_needs_to_move_for_tool_use) result |= TT_HAND_NEEDS_TO_MOVE_FOR_TOOL_USE_FLAG;
   if (tt_hand_needs_to_move_to_cursor) result |= TT_HAND_NEEDS_TO_MOVE_TO_CURSOR_FLAG;
   if (tt_hand_needs_to_move_relatively) result |= TT_HAND_NEEDS_TO_MOVE_RELATIVELY_FLAG;
   if (tt_delta_x_and_delta_y_due_solely_to_arrow_keys) result |= TT_DELTA_X_AND_Y_DUE_SOLELY_TO_ARROW_KEYS_FLAG;
   if (tt_dragging_enabled) result |= TT_DRAGGING_ENABLED_FLAG;
//#if TT_DEBUG_ON
//   if (result > 0) {
//      tt_error_file() << "Encoding event" << (int) event << endl;
//   };
//#endif
   return(result);
};

void log_user(unsigned char key,
				  boolean extended_key,
				  boolean event,
				  city_coordinate delta_x, city_coordinate delta_y, // total (arrow keys and mouse)
				  city_coordinate mouse_delta_x, city_coordinate mouse_delta_y, // new on 051100 (just mouse)
				  char mouse_button_status, char joystick_button_status, millisecond duration, boolean more,
				  unsigned char second_byte) { 
  log_put(encode_event_flags(event)); // was just even prior to 220402
  if (event) { // user really did something
//	 EventRecord event_record(button_status,delta_x,delta_y,
//									  duration,key,extended_key,more);
// following doesn't work cross 16bit and 32bit versions of same compiler
//	 log_out.write((unsigned char *) &event_record, sizeof(EventRecord));
#if TT_DEBUG_ON
	  if (event != 0 && event != 1) {
		  say_error(_T("ToonTalk confused about how to create log for replay."));
		  log(_T("Event not 0 or 1"));
//		  event = !!event;
	  };
	  if (extended_key != 0 && extended_key != 1) {
		  say_error(_T("ToonTalk confused about how to create log for replay."));
		  log(_T("Extended key not 0 or 1"));
		  extended_key = !!extended_key;
	  };
	  if (more != 0 && more != 1) {
		  say_error(_T("ToonTalk confused about how to create log for replay."));
		  log(_T("More not 0 or 1"));
		  more = !!more;
	  };
#endif
     boolean joystick_event = (joystick_button_status != 0); // better to save both??
	  // added shift, control, and second_byte flags on 220900
	  log_put((unsigned char) (((second_byte&&1)<<6) | (tt_control_down<<5) | (tt_shift_down<<4) | 
										 (joystick_event<<3) | (more<<2) | (extended_key<<1)));
	  if (joystick_event) {
        log_put(joystick_button_status);
     } else {
        log_put(mouse_button_status);
     };
	  log_put(key);
	  if (second_byte != 0) {
		  log_put(second_byte); // new on 220900
	  };
	  short int temp = (short int) delta_x;
	  log_write((bytes) &temp, sizeof(temp));
	  temp = (short int) delta_y;
	  log_write((bytes) &temp, sizeof(temp));
	  temp = (short int) mouse_delta_x; // new on 051100
	  log_write((bytes) &temp, sizeof(temp));
	  temp = (short int) mouse_delta_y;
	  log_write((bytes) &temp, sizeof(temp));
	  log_write((bytes) &duration, sizeof(duration));
	  if (tt_log_out_archive == NULL) {
		  log_out.flush();
	  };
  };
#if TT_DEBUG_ON
	if (tt_debug_mode == 202) { //  || tt_debug_mode == 2308031
		tt_error_file() << "Logging user keyboard event " << (int) key 
						<< " extended= " << (int) extended_key
						<< " event= " << (int) event 
						<< " delta_x= " << delta_x
						<< " delta_y= " << delta_y
						<< " mouse_delta_x= " << mouse_delta_x
						<< " mouse_delta_y= " << mouse_delta_y
						<< " on frame " << tt_frame_number << endl;
//		tt_error_file() << "At file position " << (int) log_out.tellg() << endl;
	};
#endif
};

void log_finalization() {
  if (tt_logging) {
	  close_log();
#if TT_DEBUG_ON
		if (tt_debug_mode == 140103) {
			tt_error_file() << "Just closed " << tt_log_out_file_name << " that was open for output. [3] at " << tt_current_time 
								 << " on frame " << tt_frame_number << endl;
		};
#endif
	  save_speak_starts(TRUE);
	  if (speak_file_name != NULL) {
		  delete [] speak_file_name;
		  speak_file_name = NULL;
	  };
     if (subtitles_file_name != NULL) {
        delete [] subtitles_file_name;
        subtitles_file_name = NULL;
     };
	  tt_logging = FALSE;
  };
};

ascii_string replay_log_file_name = NULL;

// already taken care of starting ascii_string log_file_name = replay_file_name(); in cleanup.cpp
//void release_replay_log_file_name() { // new on 261000
//	if (replay_log_file_name != NULL) delete [] replay_log_file_name;
//};

void stop_replay() {
	set_replay(NO_REPLAY);
	if (!tt_subtitles_and_narration_even_without_demo) { // tt_debug_mode != 1785) {
      speak_file = -1;
      tt_subtitles_speed = 0; // should this release some resources too?
   };
	if (tt_time_travel == TIME_TRAVEL_ON) {
//		time_travel(TIME_TRAVEL_RECORD); // new on 250100
		if (!unnamed_user() || !tt_exit_at_end_of_log) {
			// condition new 211004 so if running someone else's demo and toontalk.ini option ExitWhenDemoEnds was set then exit rather than pause
			// updated condition on 281004 from tt_log_out_archive != NULL to !unnamed_user() since can be replaying last time travel segment 
			// that ended abnormally on the previous sessions and if it doesn't play correctly don't exit now
			tt_time_travel_after_display_updated = TRUE; 
			// new on 150204 so at the end of a demo rather than begin recording you get the time travel buttons
		} else {
			set_user_wants_to_quit(tt_exit_at_end_of_log); // new on 211004 -- arg was TRUE prior to 160205
			if (tt_saved_city_file_name_when_demo_ends != NULL) {
				tt_city->dump_to_new_file(tt_saved_city_file_name_when_demo_ends,FALSE,FALSE,FALSE);
				delete [] tt_saved_city_file_name_when_demo_ends;
				tt_saved_city_file_name_when_demo_ends = NULL;
			};
		};
	};
};

millisecond next_subtitle_time = 0;
flag next_subtitle_time_is_real = FALSE;
//#if TT_UNICODE
input_file_stream subtitles_file;
//#else
//input_file_stream subtitles_file;
//#endif
const int max_subtitle_line = 80;
string subtitle_line = NULL;
flag subtitles_initialized = FALSE;
flag next_line_new_set = TRUE;
const millisecond default_subtitle_duration = 3000;

#if TT_ALPHA_FEATURE
xml_document *subtitle_xml = NULL;

xml_document *read_and_process_rest_of_xml(string read_so_far, int length, int max_size, input_stream &file) { // new on 281003
	// keep reading until finding an empty line
	int size_remaining = max_size-length;
	const int chunk_growth = 4096; // who knows what is best?
	const int minimum_chunk_size = 1024;
	while (file != NULL) {
		if (file.peek() == '\n') { // found an empty line 
			xml_document *document = document_from_string(read_so_far,length);
			delete [] read_so_far;
			return(document);
		};
		if (size_remaining < minimum_chunk_size) {
			max_size += chunk_growth;
			string new_read_so_far = new char[max_size];
			memcpy(new_read_so_far,read_so_far,length);
			delete [] read_so_far;
			read_so_far = new_read_so_far;
			size_remaining += chunk_growth;
		};
		//if (read_so_far[length-1] == '\n') { // last time just barely made it to the end
		//	debug_this();
		//};
		int length_just_read = non_failing_getline(subtitles_file,read_so_far+length,size_remaining);
		if (length_just_read == -1) return(NULL); // reached end of file
//		file.getline(read_so_far+length,size_remaining);
//		int length_just_read = file.gcount();
		length += length_just_read;
	};
	return(NULL); // warn??
};

#endif

void convert_to_real_time(millisecond &time) {
	if (tt_speed == 0) return; // leave it alone - OK??
   time = (((time-tt_current_time)*100)/tt_speed)+timeGetTime();
};

boolean really_get_next_subtitle_line(short int &length) {
	length = non_failing_getline(subtitles_file,subtitle_line,max_subtitle_line);
	if (length == -1) return(FALSE); // reached end of file
	// updated with TT_ALPHA_FEATURE (11/03 and 12/04)
   //subtitles_file.getline(subtitle_line,max_subtitle_line);
   //if (subtitles_file == NULL) return(FALSE);
   //length = (short int) (subtitles_file.gcount()-1); // -1 to remove linefeed
   if (length == 0) {
      if (next_line_new_set) {
         next_line_new_set = FALSE;
         return(really_get_next_subtitle_line(length));
      } else {
         return(FALSE);
      };
   };
	// updated with TT_ALPHA_FEATURE (11/03 and 12/04)
	if (length > 5 && _strnicmp(subtitle_line,"<?xml",5) == 0) { // is a new XML extension
		// copy_string below so always can delete the string whether grown or passed in
		subtitle_xml = read_and_process_rest_of_xml(copy_string(subtitle_line,length),length,max_subtitle_line,subtitles_file);
//		next_line_new_set = TRUE; // always OK?
	} else {
	   next_line_new_set = FALSE;
	};
	return(TRUE);
};

millisecond subtitle_now(boolean ignore_real_time) { // abstracted on 271003
   // the issue here is that on slow machines you still want the subtitles to start
	// when the demo and narration are at the same recorded time but for multi-line
	// subtitles you want subsequent lines to use the real clock so they are visible
	// ignore_real_time added on 221103
	// for the right length of time
   if (next_subtitle_time_is_real && !ignore_real_time) {
      return(timeGetTime());
		// following was wrong modularity so dealing with tt_current_time_at_beginning_of_first_segment happens elsewhere as of 150204
//	} else if (tt_log_version_number >= 34) { // new on 150204
//		return(tt_current_time+tt_millisecond_delta-tt_current_time_at_beginning_of_first_segment); // rewritten on 120204 since this is more accurate
//#if TT_ALPHA_FEATURE
//		// not clear this is needed anymore (as of 150204)
//	} else if (tt_log_in_archive != NULL && tt_time_travel != TIME_TRAVEL_OFF) { 
//		// new on 271003 note that this assumes all segments are the same size - 
//		// what about those created by recording in the middle of another segment?
//		// second conjunct added on 211103 since if recording an archive with subtitles it gets confused here
////#if TT_DEBUG_ON
////		if (tt_current_time-time_log_started > tt_time_between_new_logs) {
////			debug_this();
////		};
////#endif
//		// above test does happen - what to do? ask 271003
//		return(tt_current_time+tt_millisecond_delta-tt_current_time_at_beginning_of_first_segment); // rewritten on 120204 since this is more accurate
////		return(((tt_current_log_segment-1)*tt_time_between_new_logs)+(tt_current_time-time_log_started)+tt_millisecond_delta);
//#endif
   } else {
      return(tt_current_time-tt_current_time_at_beginning_of_first_segment); // removed +tt_millisecond_delta on 150204 since can be large since haven't yet read special events to subtract pause time
   };
};

#if TT_ALPHA_FEATURE

int max_number_of_titles = 32; // can grow if need be 
int subtitle_count = 0;
int current_index = -1;
short int *subtitle_lengths = NULL;
string *subtitle_lines = NULL;
millisecond *next_subtitle_times = NULL;
xml_document_pointer *subtitle_xmls = NULL;

void reset_next_subtitle_time(int log_number) {
	if (subtitle_count == 0) return;
	next_subtitle_time_is_real = FALSE;
	current_index = -1;
	next_subtitle_time = (log_number-1)*tt_time_between_new_logs+tt_current_time_at_beginning_of_first_segment;
	// +tt_current_time_at_beginning_of_first_segment new on 080404 since otherwise can start with the previous segment
	for (current_index = 0; current_index < subtitle_count; current_index++) {
		if (next_subtitle_times[current_index] >= next_subtitle_time) {
			next_subtitle_time = next_subtitle_times[current_index];
//			if (current_index != 0) 
			current_index--;
			break;
		};
	};
};

void get_all_subtitles() { // new on 271003 - read all the entire file and store it so can jump in time
	subtitle_lengths = new short int[max_number_of_titles];
	subtitle_lines = new string[max_number_of_titles];
	next_subtitle_times = new millisecond[max_number_of_titles+1]; // contains an extra time at the end
	subtitle_xmls = new xml_document_pointer[max_number_of_titles];
	short int length;
	while (subtitles_file != NULL) {
		while (really_get_next_subtitle_line(length)) {
			if (subtitle_count == max_number_of_titles) { // grow the tables
				int new_max_number_of_titles = max_number_of_titles*2;
				short int *new_subtitle_lengths = new short int[new_max_number_of_titles];
				string *new_subtitle_lines = new string[new_max_number_of_titles];
				millisecond *new_next_subtitle_times = new millisecond[new_max_number_of_titles+1]; // contains an extra time at the end
				xml_document_pointer *new_subtitle_xmls = new xml_document_pointer[new_max_number_of_titles];
				for (int i = 0; i < max_number_of_titles; i++) {
					new_subtitle_lengths[i] = subtitle_lengths[i];
					new_subtitle_lines[i] = subtitle_lines[i];
					new_next_subtitle_times[i] = next_subtitle_times[i];
					new_subtitle_xmls[i] = subtitle_xmls[i];
				};
				delete [] subtitle_lengths;
				delete [] subtitle_lines;
				delete [] next_subtitle_times;
				delete [] subtitle_xmls;
				max_number_of_titles = new_max_number_of_titles;
				subtitle_lengths = new_subtitle_lengths;
				subtitle_lines = new_subtitle_lines;
				next_subtitle_times = new_next_subtitle_times;
				subtitle_xmls = new_subtitle_xmls;
			};
			next_subtitle_times[subtitle_count] = next_subtitle_time; // already read during initialization or at the end of the last cycle
			if (subtitle_xml != NULL) { // found an XML instruction
				subtitle_xmls[subtitle_count] = subtitle_xml;
				subtitle_xml = NULL;
				subtitle_lengths[subtitle_count] = 0;
				subtitle_lines[subtitle_count] = NULL;
			} else {
				subtitle_lengths[subtitle_count] = length;
				subtitle_lines[subtitle_count] = copy_string(subtitle_line,length);
				subtitle_xmls[subtitle_count] = NULL;
			};
			subtitle_count++;
		};
		subtitles_file >> next_subtitle_time; // get the next time	
		next_line_new_set = TRUE; // ready for next set of lines
	};
	next_subtitle_times[subtitle_count] = next_subtitle_times[subtitle_count-1]+default_subtitle_duration; // so the last one notices it is different from the next one
	subtitles_file.close();
	next_subtitle_time = next_subtitle_times[0];
};

#endif

boolean initialize_subtitles() {
   if (subtitles_file_name == NULL) return(FALSE);
   subtitles_file.open(subtitles_file_name,std::ios_base::in); //|std::ios_base::nocreate);
//   delete [] subtitles_file_name;
   if (subtitles_file == NULL) return(FALSE);
   subtitle_line = new character[max_subtitle_line];
   subtitles_initialized = TRUE;
   subtitles_file >> next_subtitle_time;
//   convert_to_real_time(next_subtitle_time);
   next_subtitle_time_is_real = FALSE;
   next_line_new_set = TRUE;
	if (tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES) { // new on 130300
		// they interfere so
		tt_marty_talk = MARTY_SPEAKS_AND_TALK_BALLOONS;
		tt_closed_caption_alternative_language = FALSE;
		tt_closed_caption = FALSE;
	} else if (tt_marty_talk == MARTY_WITH_SUBTITLES) { // new on 110401
		// they interfere so
		tt_marty_talk = MARTY_TALK_BALLOONS;
		tt_closed_caption_alternative_language = FALSE;
		tt_closed_caption = FALSE;
	};
#if TT_ALPHA_FEATURE
	if (tt_creating_narration_file == NARRATION_CREATION_USING_SUBTITLES) { // branch new on 100204 since generating old style SPK file
		subtitle_lengths = new short int[max_number_of_titles];
		subtitle_lines = new string[max_number_of_titles];
		next_subtitle_times = new millisecond[max_number_of_titles+1]; // contains an extra time at the end
		subtitle_xmls = new xml_document_pointer[max_number_of_titles];
	} else {
		get_all_subtitles();
	};
#endif
   return(TRUE);
};

void release_subtitles() {
   if (subtitles_initialized) {
#if TT_ALPHA_FEATURE
		for (int i = 0; i < subtitle_count; i++) {
			if (subtitle_lines[i] != NULL) {
				delete [] subtitle_lines[i];
			} else if (subtitle_xmls[i] != NULL) {
				xml_release_document(subtitle_xmls[i]);
			};
		};
		subtitle_count = 0; // in case called more than once
		current_index = -1;
		delete [] subtitle_lengths;
		delete [] subtitle_lines; 
		delete [] next_subtitle_times;
		delete [] subtitle_xmls;
#else
		subtitles_file.close();
#endif
      delete [] subtitle_line;
   };
};

void process_demo_xml(xml_document *document) {
	xml_node *node = first_node_that_is_an_element(document);
	Tag tag = tag_token(node);
	switch (tag) {
		case SOUND_FILE_TAG: 
			if (tt_log_in_archive != NULL && tt_play_demo_sound_files) { 
				// conditional new on 061103 - can be NULL if demo is over and tt_subtitles_and_narration_even_without_demo
				// tt_play_demo_sound_files added on 210704
				string file_name = xml_get_narrow_text_copy(node);
				if (file_name != NULL) {
					string full_file_name = extract_file_from_archive(file_name,tt_log_in_archive);
					if (full_file_name != NULL) {
						play_sound_file(full_file_name,99);
						delete [] full_file_name;
					};
					delete [] file_name;
				};
			};
			break;
		case TEXT_TO_SPEECH_TAG: 
			if (tt_speak_demo_text) { // conditional new on 210704
				BSTR text = xml_get_text(node);
				if (text != NULL) {
					speak(text);
				};
			};
			break;
		default:
#if TT_DEBUG_ON
			xml_debug_node(node);
#endif
			tt_error_file() << "Unrecoginized tags in demo subtitle script." << endl;
	};
};

boolean get_next_subtitle_line(short int &length) {
	int saved_current_index = current_index;
	int next_index = current_index+1;
	if (current_index >= 0 && next_index < subtitle_count && next_subtitle_times[current_index] == next_subtitle_times[next_index]) {
		// next line with same start time
		current_index = next_index;
	} else {
		millisecond now = subtitle_now(TRUE); // arg added 221103 -tt_current_time_at_beginning_of_first_segment added 150204 -- moved on 160204 to subtitle_now
		int index_of_first_line_in_last_group = -1;
		int start_time_of_last_group = min_long;
		current_index++; // ready for next - or if first time then current_index should be -1
		for (; current_index <= subtitle_count; current_index++) { // <= to go an extra to deal with last one (next_subtitle_times has max_long final value)
			if (next_subtitle_times[current_index] >= now && index_of_first_line_in_last_group >= 0) {
				current_index = index_of_first_line_in_last_group; // we want the one just before the current time
				break;
			} else if (start_time_of_last_group != next_subtitle_times[current_index]) {
				start_time_of_last_group = next_subtitle_times[current_index];
				index_of_first_line_in_last_group = current_index;
			};
		};
		//if (current_index == subtitle_count) { // didn't find any
		//	current_index = index_of_first_line_in_last_group; // got to the end so use this - should be the last time
		//};
	};
	if (current_index < subtitle_count) {
		next_line_new_set = FALSE; // I think this simulates the old linear behavior
		length = subtitle_lengths[current_index];
		if (subtitle_xmls[current_index] != NULL) { // need to do something special
			process_demo_xml(subtitle_xmls[current_index]);
			return(get_next_subtitle_line(length));
		};
		subtitle_line = copy_string(subtitle_lines[current_index]);
		if (current_index+1 < subtitle_count) {
			next_subtitle_time = next_subtitle_times[current_index+1]; // new on 221103 to set it up to wait for the next subtitle
		};
		return(TRUE);
	} else { // no more
		if (next_subtitle_time_is_real) {
			current_index = saved_current_index; // try again later
		};
		return(FALSE);
	};
};

void get_next_subtitle_time() {
//	if (next_subtitle_time_is_real) return; // is already fine
	if (current_index < subtitle_count) {
		next_subtitle_time = next_subtitle_times[current_index+1];
	} else {
		next_subtitle_time = max_long;
	};
};

//boolean get_next_subtitle_line(short int &length) {
//   // broken into 2 interfaces on 271003 for use by time travel
//   return(really_get_next_subtitle_line(length));
//};

void synchronize_subtitles() {
   if (!subtitles_initialized) {
      if (!initialize_subtitles()) { // can't initialize
         tt_subtitles_speed = 0;
         return;
      };      
   };
//	millisecond now = subtitle_now(FALSE); // obsolete as of 150204 replaced by tt_current_time
#if TT_DEBUG_ON 
	if (tt_debug_mode == 221103) {
		tt_error_file() << "Now is " << (tt_current_time/1000.0) << " and next_subtitle_time is " << (next_subtitle_time/1000.0) << " and real clock is " << (timeGetTime()/1000.0) << endl;
	};
#endif
   if (tt_current_time-tt_current_time_at_beginning_of_first_segment >= next_subtitle_time) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 290204) {
			tt_error_file() << "Ready for next subtitle since " << endl
								 << "tt_current_time-tt_current_time_at_beginning_of_first_segment >= next_subtitle_time" << endl
								 << tt_current_time << "-" << tt_current_time_at_beginning_of_first_segment << " >= " << next_subtitle_time << endl
								 << "At real time " << timeGetTime() << endl;
		};
#endif
		// subtract tt_current_time_at_beginning_of_first_segment so subtitle file can be constructed with fiction that time is 0 at the start
      millisecond subtitle_duration = (default_subtitle_duration/tt_subtitles_speed)*100;
      // ToonTalk speed should not affect subtitle duration
      subtitle_duration = (subtitle_duration*tt_speed)/100;
		if (subtitle_duration == 0) { // new on 210300
			subtitle_duration = default_subtitle_duration; 
		};
      short int length;
      if (get_next_subtitle_line(length)) {
         // short lines are quicker
			int effective_length;
         if (length < 20) {//  // prior to Oct 10 '98 was != -1 which was wrong
				// minimum time now not constrained even if matching narration
            effective_length = 20;
         } else {
				effective_length = length;
				int i;
			   for (i = 0; i < length; i++) {
				   if (subtitle_line[i] == '.' || subtitle_line[i] == '?' || subtitle_line[i] == '!') {
			         // new policy 3/30/98
                  effective_length += 6; // add a bit for natural pauses at the end of sentences
					   break; // once is enough
					};
            };
				for (i = 0; i < length; i++) { // new on 6/11/98 -- numbers take longer to say
					if (subtitle_line[i] >= '1' && subtitle_line[i] <= '9') { 
						effective_length += 4;
					} else if(subtitle_line[i] == '0') { // zero may not take much longer
						effective_length += 2;
					};
				};
			};
			subtitle_duration = (subtitle_duration*effective_length)/45; // 45 is empirically about line length
			// maybe this should be scaled by tt_speed - noted on 271003
// commented out on Oct 11 '98 since the next subtitle is more likely to clobber
// a slow subtitle
//         if (speak_file == -1) { // just subtitles so slow them down
//            subtitle_duration = (subtitle_duration*133)/100; // add 33%
//         } else 
			if (tt_language == AMERICAN) { // tend to fall behind
            subtitle_duration = (subtitle_duration*90)/100; // take off 10%
         };
			if (tt_display_demo_subtitles) { // conditional new on 210704
				tt_screen->set_subtitle(subtitle_line,length,subtitle_duration);
			};
//#if TT_TEXT_TO_SPEECH
//         if (speak_file == -1 && text_to_speech()) {
//            string line = new char[length+2];
//            memcpy(line,subtitle_line,length);
//            line[length] = ' '; // to force the engine to say it now
//            line[length+1] = NULL; // terminate it
//            speak(line);
//            delete [] line;
//         };
//#endif 
#if TT_ALPHA_FEATURE	
			if (next_subtitle_time <= tt_current_time-tt_current_time_at_beginning_of_first_segment) { // should happen when multi-line subtitle -- new on 221103
				next_subtitle_time = tt_current_time+subtitle_duration-tt_current_time_at_beginning_of_first_segment;
				// -tt_current_time_at_beginning_of_first_segment new on 150204 since will be subtracted from tt_current_time
			};
			// commented out on 221103 since now computes next time when finding the current subtitle
			//next_subtitle_time = timeGetTime()+subtitle_duration; // just add duration to the real time clock
			//next_subtitle_time_is_real = TRUE; // whether it was or not
#else
			next_subtitle_time = tt_current_time+subtitle_duration ; // was prior to Oct 10 '98 += subtitle_duration;
			// changed since if behind schedule this gives more time
			if (!next_subtitle_time_is_real) {
				convert_to_real_time(next_subtitle_time);
				next_subtitle_time_is_real = TRUE;
			};
#endif
      } else { // ready for next set
			next_subtitle_time_is_real = FALSE; // moved here on 271003 so this is set before the call to get_next_subtitle_time
			get_next_subtitle_time();
			// rewritten with TT_ALPHA_FEATURE
//       subtitles_file >> next_subtitle_time;
         // check if time is 0 (i.e. problem)
//       convert_to_real_time(next_subtitle_time);
         next_line_new_set = TRUE;
      };
   };
};

void synchronize_narration(millisecond millisecond_delta) {
	millisecond now = tt_current_time+millisecond_delta;
	millisecond start = speak_starts[speak_index];
	if (start == 0) { // skip this
	} else if (now >= start) { // was just >
//#if TT_DIRECTX
      if (!sound_buffer_playing()) {
//#else
//		if (stop_sound(speak_priority-1)) { // try to stop a lower priority sound
//#endif
			speak_now();
		} else { // else wait half a second and try again
			start += 500;
		};
	};
	if (start != 0 && now > start-1000) {
		// 1 second before it is needed
		fetch_next_speak();
	};
};

void release_next_speak() {
#if TT_DIRECTX
   release_sound_buffer(next_speak);
#elif TT_32
	delete [] next_speak;
#else
	if (next_speak_memory_handle != NULL) {      
		GlobalUnlock(next_speak_memory_handle);
		GlobalFree(next_speak_memory_handle);
		next_speak_memory_handle = NULL;
	};
#endif
   next_speak = NULL;
};

void release_previous_speak() { // proceduralized on 130700
   if (previous_speak != NULL) {
		release_sound_buffer(previous_speak); // new on 170300
		previous_speak = NULL;
	};
};

void reopen_log_in(long restart_position) { // new on 050600
//	if (log_in_archive() != NULL) {
//		close_current_file_in_unzip_archive(tt_log_in_archive);
//		open_named_file_in_unzip_archive(tt_log_in_archive,replay_log_file_name);
//		if (zipped_log_in->empty()) {
//			zipped_log_in = new zipped_input_file_stream(tt_log_in_archive);
//		};
////		zipped_log_in->set_archive(tt_log_in_archive);
//	} else {
	// THIS PROBABLY NEEDS TO BE REWRITTEN -- like the other calls to log_in->open
		log_in->close(); // just in case
		log_in->open(replay_log_file_name,std::ios_base::in|std::ios_base::binary); // |std::ios_base::nocreate);
		log_in->seekg(restart_position); // is this a problem for archives???? - asked 250603
//	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 140103) {
		tt_error_file() << "Just re-opened " << replay_log_file_name << " for input at " << tt_current_time << endl;
	};
#endif
};

unsigned long ignore_log_growth(int number_of_bytes) {
	unsigned long extra;
	if (number_of_bytes == 1) {
		//if (tt_log_in_archive != NULL) { // new branch on 030703
		//	unsigned char buffer[1];
		//	read_current_file_in_unzip_archive(tt_log_out_archive,buffer,1);
		//	extra = buffer[0];
		//} else {
			extra = log_in->get();
//		};
	} else if (number_of_bytes == 2) {
		unsigned short short_extra;
		//if (tt_log_in_archive != NULL) { // new branch on 030703
		//	read_current_file_in_unzip_archive(tt_log_out_archive,(char *) &short_extra,sizeof(short_extra));
		//} else {
			log_in->read((string) &short_extra,sizeof(short_extra));
//		};
		extra = short_extra;
	} else { // check first??
		//if (tt_log_in_archive != NULL) { // new branch on 030703
		//	read_current_file_in_unzip_archive(tt_log_out_archive,(char *) &extra,sizeof(extra));
		//} else {
			log_in->read((string) &extra,sizeof(extra));
//		};
	};
	if (extra > 0) {
		//if (tt_log_in_archive != NULL) {
		//	bytes buffer = new byte[extra];
		//	read_current_file_in_unzip_archive(tt_log_out_archive,buffer,extra);
		//	delete [] buffer;
		//} else {
		// could use .ignore(extra) instead...
		log_in->ignore(extra); // faster than the following sometimes - updated 070703
		//for (unsigned long i = 0; i < extra; i++) {
		//	log_in->get();
		//};
//		};
	};
	return(extra);
};

string demo_created_by_exe_name = NULL; // new on 170301 to generate warning

boolean already_noted_that_city_loading_is_taking_too_long = FALSE; // for issuing warnings

boolean open_log(ascii_string log_file_name, output_stream &stream, 
					  boolean ok_if_doesnt_exist, boolean warn_if_doesnt_exist, boolean first_time, 
					  boolean may_include_opening_credits) {
#if TT_DEBUG_ON
	millisecond start_time = timeGetTime();
#endif
	ascii_string new_log_file_name;
	boolean file_name_provided_elsewhere = FALSE;
	if (log_file_name == NULL) { // set elsewhere
		log_file_name = replay_log_file_name;
		new_log_file_name = copy_string(log_file_name);
		if (log_file_name == NULL) {
			return(FALSE);
		};
		file_name_provided_elsewhere = TRUE;
	} else if (!first_time) { // file name should be OK already
		new_log_file_name = copy_string(log_file_name);
	} else {
	   new_log_file_name = add_demo_extension(log_file_name,TRUE);
	};
	if (new_log_file_name == NULL) {
		// tt_error_file() isn't opened yet (by tt_initialize)
		if (warn_if_doesnt_exist) {
			stream << S(IDS_UNABLE_TO_OPEN_FILE) << space() << log_file_name << end_sentence() << endl;
		};                                                                       
//		delete [] log_file_name;
		stop_replay();
		return(ok_if_doesnt_exist);
	} else {
		boolean opened_ok = FALSE; // given a default value on 201003
		if (!file_name_provided_elsewhere && log_in_archive() != NULL && file_exists(tt_log_in_archive)) { 
			// moved file_exists here on 180703
#if TT_DYNA_ZIP
			string short_log_file_name = short_file_name(new_log_file_name,TRUE); // kind of awkward - new on 010703
//#if TT_DEBUG_ON
//			millisecond start;
//			if (tt_debug_mode == 281003) {
//				start = timeGetTime();
//			};
//#endif
			static boolean already_looked_for_local_ini_file = FALSE;
			if (!already_looked_for_local_ini_file) { // new on 301003 -- this should be done with all TT zip archives
				// removed tt_local_ini_file == NULL && on 030304 -- rewritten to not use tt_local_ini_file at all here
				already_looked_for_local_ini_file = TRUE; // so for next segment won't bother to try this again
				string short_name = short_file_name(tt_log_in_archive);
				string local_ini = append_strings(short_name,".ini");
				string local_ini_file = extract_file_from_archive(local_ini,tt_log_in_archive); 
				// was tt_local_ini_file = prior to 030304
				tt_local_ini_file = copy_string(local_ini_file); // restored as experiment on 100904
//				string exe_name = NULL;
				char exe_name[MAX_PATH];
				int exe_named = 0;
				if (local_ini_file != NULL) {
					//UnwindProtect<string> set(tt_ini_file,tt_local_ini_file);
					//UnwindProtect<string> set2(tt_local_ini_file,NULL); // new on 021103 to only use the new tt_local_ini_file
//					exe_name = ini_entry("Executables","DemoToonTalk32",FALSE); // new on 021103
					exe_named = GetPrivateProfileStringG("Executables","DemoToonTalk32","",exe_name,MAX_PATH,local_ini_file);
				};
				if (exe_named && stricmp(exe_name,AC(IDC_TOONTALK_EXE_NAME)) != 0) { 
					// should be running a different EXE - new on 311003
					FileNameStatus name_status;
					tt_exe_to_run_upon_shutdown = existing_file_name(name_status,exe_name,"exe",NULL,FALSE,FALSE,FALSE);
					if (tt_exe_to_run_upon_shutdown == NULL) {
						// new on 030304 to ask if it is OK to do this
						string message = S(IDC_DMO_CONTAINS_EXE_WARNING,"This demo needs a different version of ToonTalk to run correctly. This version isn't installed but is contained in the demo file. You should ONLY proceed if you trust the person who supplied this file. Do you want to continue?");
						if (message_box(MB_YESNO|MB_SYSTEMMODAL,message,NULL,TRUE) == IDYES) {
							string exe_file_name = append_strings(exe_name,".exe");
							tt_exe_to_run_upon_shutdown = extract_file_from_archive(exe_file_name,tt_log_in_archive);
							delete [] exe_file_name;
						};
					};
					if (tt_exe_to_run_upon_shutdown != NULL) {
//						tt_command_line_for_exe_to_run_upon_shutdown 
							// = append_strings(tt_command_line_for_exe_to_run_upon_shutdown," -allow_multiple_toontalks 1");
						SetWindowText(tt_main_window->get_handle(),"Switching to another version of ToonTalk"); 
						// this prevents FindWindow from treating this as a running ToonTalk
						standard_exit(FALSE);
						exit(FALSE);
					};					
				};
				delete [] short_name;
				delete [] local_ini;
				delete [] local_ini_file;
			};
			// moved here on 100904
			tt_display_demo_subtitles = ini_int("Switches","DisplayAvailableSubtitlesInDemos",FALSE,tt_display_demo_subtitles);
			tt_play_demo_sound_files = ini_int("Switches","PlayAvailableDemoSoundFiles",FALSE,tt_play_demo_sound_files);
			tt_speak_demo_text = ini_int("Switches","SpeakAvailableDemoText",FALSE,tt_speak_demo_text);
			tt_exit_at_end_of_log = ini_int(AC(IDC_SWITCHES),"ExitWhenDemoEnds",FALSE,tt_exit_at_end_of_log); 
			// new on 070201 -- moved here from tt_initialize on 211004
		   tt_run_demos_despite_anomalies = ini_int(AC(IDC_SWITCHES),"KeepRunningDemoDespiteProblems",FALSE,
																  tt_run_demos_despite_anomalies); 
			// new on 311204
			tt_remove_unneeded_things_from_thought_bubble = 
				ini_int(AC(IDC_SWITCHES),"RemoveUnneededThingsFromThoughtBubbles",FALSE,
						  tt_remove_unneeded_things_from_thought_bubble); 
				// new on 110105
			// prior to 200105 the above was RemoveUnneededThingsFromThoughBubbles (no t in Thought)
			tt_old_nest_merging_behavior = ini_int(AC(IDC_SWITCHES),"OldNestMergingScheme",FALSE,tt_old_nest_merging_behavior);
			string temp_file_name = extract_file_from_archive(short_log_file_name,tt_log_in_archive); // new on 291003
			if (temp_file_name == NULL) { // new on 031103
				// try the user's directory (in case a log wasn't copied to the archive)
				temp_file_name = append_strings(tt_user_directory,short_log_file_name);
				if (file_exists(temp_file_name)) {
					// put it in the archive
					zip_file(tt_log_in_archive,temp_file_name,NULL,default_compression_level,TRUE);
				} else {
					delete [] temp_file_name;
					temp_file_name = NULL;
				};
			};
			if (temp_file_name != NULL) {
				opened_ok = TRUE;
				// tt_delete_replay_file_after_reading is obsolete now
				if (new_log_file_name != NULL) delete [] new_log_file_name; 
				// except for the first log this is equal to temp_file_name so a bit wasteful
				new_log_file_name = temp_file_name;
			};
			// following replaced by the above on 301003
//			opened_ok = extract_archive_to_directory(tt_log_in_archive,short_log_file_name,tt_user_directory);
//			tt_delete_replay_file_after_reading = opened_ok; 
			//  && (strncmp(tt_user_directory,replay_log_file_name,strlen(tt_user_directory)) == 0));
			// the above took between 34 and 73ms for an archive with about 50 logs - measured 281003 on SU/KTH laptop
//#if TT_DEBUG_ON
//			if (tt_debug_mode == 281003) {
//				tt_error_file() << "Took " << (timeGetTime()-start) << "ms to extract " << short_log_file_name << endl;
//			};
//#endif
			// since is just a copy of what is in the archive - new on 180703
			// added second condition on 100803 since shouldn't delete files it didn't extract
			// rewrote the follow as the above since MUCH faster
//			open_named_file_in_unzip_archive(tt_log_in_archive,short_log_file_name); // use this - 260603
			delete [] short_log_file_name;
//			opened_ok = TRUE; // above doesn't really check - just sets thing up to read... was -- (file != NULL); - updated 300603
			if (log_in != NULL) {
				delete log_in;
				log_in = NULL;
			};
//			log_in->open(tt_log_in_archive);
////			zipped_log_in.set_archive(tt_log_in_archive);
#else
			opened_ok = (open_named_file_in_unzip_archive(tt_log_in_archive,new_log_file_name) == UNZ_OK);
#endif
		}; 
		if (log_in == NULL || log_in_archive() == NULL) {
			if (log_in == NULL) {
				log_in = new FileInputStream();
			};
			log_in->open(new_log_file_name,std::ios_base::in|std::ios_base::binary); // std::ios_base::nocreate);
			if (log_in->empty()) {
				log_in->close(); // new on 140103 - not sure why it's needed but otherwise file is empty the first time around
				log_in->open(new_log_file_name,std::ios_base::in|std::ios_base::binary); // std::ios_base::nocreate);
				opened_ok = !log_in->empty(); // (log_in != NULL);
			} else {
				opened_ok = TRUE;
			};
		};
		if (!opened_ok) { // didn't help to re-open
			if (warn_if_doesnt_exist || tt_debug_mode == 140103) {
				stream << S(IDS_UNABLE_TO_OPEN_FILE) << space() << log_file_name 
					    << " error number is " << GetLastError() << end_sentence() << endl;
				tt_err_file_important = TRUE; 
			};
			// new on 031103 that if log is missing to just skip it if it isn't the last one
			if (tt_current_log_segment >= tt_youngest_log_segment) { 
				stop_replay();
			} else {
				// try the next segment
				tt_error_file() << "Couldn't find the file " << log_file_name << " so skipping this segment." << endl;
//				return(jump_to_log_segment(tt_current_log_segment+1));
#if TT_DEBUG_ON
				debug_this();
#endif
				throw(LOG_FILE_MISSING);
			};
//				int x = GetLastError();
//          LocalFile *file = new LocalFile(new_log_file_name);
//          long s = file->Size();
//				tt_error_file() << s << endl;
			delete [] new_log_file_name; // new on 070799
			return(FALSE);
		};
#if TT_DEBUG_ON
		if (tt_debug_mode == 140103) {
			tt_error_file() << "Just opened " << new_log_file_name 
								 << " for input at " << tt_current_time  << " on frame " << tt_frame_number << endl;
		};
#endif
		char cookie[9];
		log_in->read((string) cookie,8);
		cookie[8] = '\0'; // terminate the string
		if (is_zip_cookie((string) cookie)) { 
			// updated 281204 cookie[0] == 'P' && cookie[1] == 'K') { // PK zip format -- new on 190703
			if (tt_log_in_archive != NULL) {
				delete [] tt_log_in_archive;
			};
			delete log_in; // new on 050404
			log_in = NULL; // new on 050404
			tt_log_in_archive = new_log_file_name;
			update_private_media_directory(new_log_file_name); // extract any media files included therein
			if (open_named_file_in_unzip_archive(tt_log_in_archive,"0.xml") != NULL) { 
				// condition new on 110104 since the following only makes sense if archive is a time-travel archive
				close_current_file_in_unzip_archive(new_log_file_name); // opened it just to see if it existed
				tt_jump_to_current_log_segment = TRUE;
				set_replay(NO_REPLAY); // until we jump later
#if TT_DEBUG_ON
				if (tt_debug_mode == 150404) {
					log("Found 0.xml and will jump to current segment.");
				};
#endif
			} else { // new on 110104
				string demo_file_name = open_named_file_in_unzip_archive(tt_log_in_archive,"log*.dmo"); 
				// added log since can have generated dmo files from clipboard in archive -- 220704 (after 3.92)
				if (demo_file_name == NULL) {
					tt_error_file() << "Couldn't find a DMO file inside of " << tt_log_in_archive << endl;
					// following new on 200804
					tt_err_file_important = TRUE; 
					trouble_shoot(TROUBLE_SHOOT_BAD_DEMO,S(IDS_LOG_REPLAY_ERROR));
					set_user_wants_to_quit(tt_exit_at_end_of_log); // arg was TRUE prior to 160205
					return(FALSE);
				} else if (extract_archive_to_directory(tt_log_in_archive,demo_file_name,tt_extracted_file_temp_directory)) {
					char new_file_name[MAX_PATH]; 
					// if dynamic is a leak -- new_file_name was 
					// append_strings(tt_extracted_file_temp_directory,demo_file_name) prior to 130104
					strcpy(new_file_name,tt_extracted_file_temp_directory);
					strcat(new_file_name,demo_file_name);
#if TT_DEBUG_ON
					if (tt_debug_mode == 150404) {
						tt_error_file() << "Trying again with " << demo_file_name << " in " << tt_log_in_archive << endl;
					};
#endif
					return(open_log(new_file_name,stream,ok_if_doesnt_exist,warn_if_doesnt_exist,first_time,
										 may_include_opening_credits));
				} else {
					tt_error_file() << "Couuldn't extract " << demo_file_name << " from " << tt_log_in_archive 
										 << " to " << tt_extracted_file_temp_directory << endl;
					tt_err_file_important = TRUE; // new on 200804
					set_replay(NO_REPLAY); // new on 200804
					return(FALSE);
				};
			};
			return(TRUE); // not an error but unusual...
		};
		if (strncmp(cookie,AC(IDS_DEMO_FILE_COOKIE),5) != 0 &&
			 strncmp(cookie,AC(IDS_OLD_DEMO_FILE_COOKIE),5) != 0) {
			stream << S(IDS_SORRY_BUT_FILE) << space()
					 << log_file_name << space() << S(IDS_NOT_A_GOOD_DEMO_FILE) << endl; 
			stop_replay();
			return(FALSE); // abnormal
		};
		tt_log_version_number = (unsigned char) atoi(cookie+5);
		if (tt_log_version_number > latest_log_version_number) {
			say_error(IDS_DEMO_FILE_NEED_NEWER_VERSION_OF_TOONTALK),TRUE;
			// next line new on 230305 -- commented out on 230305 since too early to know demo_created_by_exe_name
			//tt_error_file() << "Created by " << demo_created_by_exe_name << " and now running " << C(IDC_TOONTALK_EXE_NAME) << endl;
			stop_replay();
			return(FALSE);
		} else if (tt_log_version_number < 20) { // new on 070300
			say_error(S(IDS_DEMO_FILE_NEED_OLDER_VERSION_OF_TOONTALK,
							"Sorry, this version of ToonTalk can't run this demo. Try versions earlier than 2.0."),TRUE);
  			stop_replay();
			return(FALSE);
		} else if (tt_log_version_number < 33) {
			// new on 050204 since need to reset these to their default values before introducing them
			tt_right_to_left_text = FALSE;
			if (!tt_scrolling_ok) { // new on 040204 - just turned scrolling back on so re-compute limits of programmer
				tt_scrolling_ok = TRUE;
				tt_programmer->initialize();
			};
		};
//			if (tt_log_version_number < 9)
//      tt_new_input_scheme = FALSE;
		if (replay_log_file_name != NULL && replay_log_file_name != log_file_name) { 
			// was log_file_name prior to 170103 - restored on 300103 since replay_log_file_name is shared 
			delete [] replay_log_file_name; // new on 261000
		};
		replay_log_file_name = new_log_file_name;
//		if (version_number == 1) {
//			Version1Parameters log_preface;
//			log_in.read((unsigned char *) &log_preface,sizeof(log_preface));
//			log_preface.reset_unused_options();
//			log_preface.install_parameters();
//		} else {
/*
			if (tt_log_version_number < 16) {
				Parameters log_preface;
				read_user_parameters(log_in,log_preface);
	//			log_in.read((unsigned char *) &log_preface,sizeof(log_preface));
				log_preface.install_parameters(0);
	//         if (tt_log_version_number >= 3) {// && !tt_temporary_user) {
	//            if (tt_log_version_number >= 6) { // fixed a bug in saving and restoring favorites
						log_in.read((character *) tt_favorites,
										favorites_count*sizeof(FavoriteSizeAndLocation));
	//            } else {
	//               log_in.read((unsigned char *) tt_favorites, sizeof(tt_favorites));
	//            };
					tt_favorites_set = (tt_favorites[TOOLBOX_FAVORITE].favorite_width != 0);
					// added following on 220999 to restore ability to run old demos
					tt_histories = new History[history_count];
					for (int i = 0; i < history_count; i++) {
					   tt_histories[i] = 0;
					};
			// following new on 200799
			} else {
*/
				boolean first_segment = (boolean) log_in->get(); 
				// new on 180100 that log leaves a flag as to whether it is the first in a sequence
				// added may_include_opening_credits on 110603 to work around problem with time travel visiting most recent city
				if (!first_time && first_segment && may_include_opening_credits) {
					return(close_input_log(TRUE,TRUE)); 
					// skip over first segment since it contains all sorts of inappropriate stuff to define user etc
				};
				if (first_segment) {
					last_closed_log_segment = -2; // so not treated as successive
				};
				tt_system_mode = (SystemMode) log_in->get();
				release_keyboard_accelerators();
				tt_keyboard_accelerators = log_in->read_string();
				tt_vacuum_keyboard_accelerators = log_in->read_string();
				tt_expander_keyboard_accelerators = log_in->read_string();
				tt_copier_keyboard_accelerators = log_in->read_string();
				initialize_keyboard_accelerators(); // no-op if they are non-NULL
				boolean new_name = set_tt_user_name(log_in->read_string());
				if (tt_puzzle_file_name != NULL) delete [] tt_puzzle_file_name;
				tt_puzzle_file_name = log_in->read_string(); // added 230799
				long debug_counter = 0;
//				if (tt_log_version_number > 17) {
					millisecond previous_current_time = tt_current_time; // new with ALPHA_FEATURE 041103?
					millisecond current_time; 
					// new on 160904 so when running_successive_demo neither tt_current_time nor tt_frame_number is clobbered
					long frame_number;
					log_in->read((string) &current_time,sizeof(current_time)); // new on 241199 -- was tt_current_time prior to 160904
					if (tt_prevous_log_segment+1 == tt_current_log_segment && previous_current_time > current_time) { // new on 041103
						// time decreased yet is sucessive segment - probably created by a session where the current time wasn't saved
						repair_time_travel_button_animations();
					};
					time_log_started = tt_current_time;
					log_in->read((string) &frame_number,sizeof(frame_number)); // new on 161299 - prior to 160904 was tt_frame_number
//					if (tt_frame_number > 1) tt_frame_number++; // new on 130100
					tt_open_log_time = current_time;
					tt_log_started_frame = frame_number; // new on 130100
					unsigned long seed;
					log_in->read((string) &seed,sizeof(seed)); // new on 120100
					if (!running_successive_demo()) {
						tt_frame_number = frame_number;
						set_random_number_seed(seed);
					};
					tt_current_time = current_time; // from 160904 to 290904 this was in the conditional above
					boolean programmer_invisible;
					log_in->read((string) &programmer_invisible,sizeof(programmer_invisible)); // new on 150100
					if (tt_programmer != NULL) tt_programmer->set_visibility(!programmer_invisible); // new on 090103
					boolean stopped;
					log_in->read((string) &stopped,sizeof(stopped)); // new on 150100
					if (tt_city != NULL) tt_city->set_stopped(stopped);
					log_in->read((string) &debug_counter,sizeof(debug_counter)); // new on 170100
					load_keep_every_data(log_in); // new on 180100
					log_in->read((string) &date_and_time_log_created,sizeof(date_and_time_log_created));
					tt_martian_automatically_appears = (boolean) log_in->get(); // experiment on 020703
//					log_in->read((string) &tt_martian_automatically_appears,sizeof(tt_martian_automatically_appears));
					if (tt_log_version_number > 20) { 
						tt_log_used_wide_characters = (flag) log_in->get(); // new on 170300
						tt_alternative_spoken_language = (NaturalLanguage) log_in->get(); // new on 170300
						if (tt_alternative_spoken_language != NO_LANGUAGE_SPECIFIED) { 
							// new on 131203 since can cause Ver22.DLL missing warnings
							set_country_code(country_codes[tt_alternative_spoken_language],TRUE);
						};
					} else {
						tt_log_used_wide_characters = FALSE;
						tt_alternative_spoken_language = NO_LANGUAGE_SPECIFIED;
					};
					if (tt_log_version_number > 25) { // new on 170301
						demo_created_by_exe_name = log_in->read_string(); // used to generate warnings late					
					};
					ignore_log_growth(4);
//					unsigned long growth_block_length = 0;
					//  for growth without breaking old 
//					log_in.read((bytes) &growth_block_length,sizeof(growth_block_length)); // new on 241199
//					if (growth_block_length > 0) {
//						log("Something wrong with log.",TRUE,TRUE);
//					};
//				};
#if TT_DEBUG_ON
			  if (tt_debug_mode == 160299) {
				  int tell = log_in->tellg();
				  tt_error_file() << "Read log settings - file position is " << tell << endl;
				  tt_error_file() << "Date and time log was created is " << date_and_time_log_created << endl;
			  };
#endif
				// following moved up earlier so customizations such as video mode happen before
				// initializing objects
//#if TT_DEBUG_ON
//			  if (tt_debug_mode == 160299) {
//				  tt_error_file() << "File position before reading history is " << (int) log_in->tellg() << endl;
//			  };
//#endif
			  // on 200703 added tt_log_in_archive != NULL since don't want to change user name etc if reading a time travel log
			  read_history(log_in,TRUE,TRUE,(tt_log_in_archive != NULL)); // this includes user parameters so no need to dump twice
//#if TT_DEBUG_ON
//			  if (tt_debug_mode == 160299) {
//				  tt_error_file() << "File position after reading history is " << (int) log_in->tellg() << endl;
//			  };
//#endif
			  if (!running_successive_demo()) { // condition new on 220703
			     if (new_name) compute_tt_file_name();
				  if (!tt_initialize()) {
//					  premature_exit = TRUE;
					  return(0); // failure
				  };
				  tt_initialize_objects();
				  if (tt_screen != NULL) { // new on 261003 - just in case time travelling and subtitles are up
					  tt_screen->remove_current_subtitle();
				  };
//#if TT_DEBUG_ON
//			     if (tt_debug_mode == 160299) {
//				     tt_error_file() << "File position before resuming initializing history is " << (int) log_in->tellg() << endl;
//				  };
//#endif
			  };
			  long restart = resume_initialize_history(log_in,TRUE);
			  if (restart > 0) {
				  reopen_log_in(restart);
				  resume_initialize_history(log_in,TRUE);
			  };
//#if TT_DEBUG_ON
//			  if (tt_debug_mode == 160299) {
//				  tt_error_file() << "File position after resuming initializing history is " << (int) log_in->tellg() << endl;
//			  };
//#endif
			  if (tt_system_mode == PUZZLE) {
					if (tt_puzzle_file_name == NULL) resume_puzzles();
				} else {
					Notebook *main_notebook = replay_notebook();
					if (!running_successive_demo() && (tt_time_travel == TIME_TRAVEL_OFF)) { 
						// commented out  || !tt_dump_as_xml on 150903
						// conditional new on 130103 since no need -- added running_successive_demo on 220703
						if (tt_toolbox == NULL) { // updated 080704
							tt_toolbox = new Toolbox(); //main_notebook); // added here since recorded before city now
						}; // else {
						set_toolbox_notebook(main_notebook);
//						};
						tt_toolbox->initialize(); // new on 161202
					};
//#if TT_DEBUG_ON
//				   if (tt_debug_mode == 160299) {
//					   tt_error_file() << "File position before reading clipboard is " << (int) log_in->tellg() << endl;
//				   };
//#endif
					boolean new_text; // this is because load_city checks the clipboard for a city on the clipboard
					string clipboard_text = tt_main_window->clipboard_text(new_text,NULL,TRUE,TRUE); 
					// rewritten on 040803 to ignore within clipboard_text
//#if TT_DEBUG_ON
//				   if (tt_debug_mode == 160299) {
//					   tt_error_file() << "File position after reading clipboard is " << (int) log_in->tellg() << endl;
//				   };
//#endif
					if (clipboard_text != NULL) {
						// ignore clipboard since city is dumped as well in normal manner
//						if (first_time) { // since only if first log might the clipboard have a city on it
//							tt_city->load_city_from_clipboard_string(clipboard_text);
//						};
						if (new_text) delete [] clipboard_text;
					};
					// on 160300 following moved into conditional since if log of a puzzle no need to save city in it
//#if TT_DEBUG_ON
//				   if (tt_debug_mode == 160299) {
//					   tt_error_file() << "File position before loading city from log is " << (int) log_in->tellg() << endl;
//				   };
//#endif
					if (tt_log_in_archive != NULL) { // conditional new on 080804
						DWORD start = timeGetTime();
						try {
							load_city_from_log();
						} catch (CycleInterruptionReason reason) { // new on 080804
							tt_error_file() << "Unable to load city from log file. Error code is " << (int) reason << endl;
						};
					};
//#if TT_DEBUG_ON
//				  if (tt_debug_mode == 160299) {
//					  tt_error_file() << "File position before loading city from log is " << (int) log_in->tellg() << endl;
//				  };
//#endif
				};
				if (!first_time) { // added on 120100 so can start log at any point - should I add || first_segment??
					initialize_button_key_substitutions(); 
					//tt_current_time += replay_clock(FALSE); // experiment on 230803
					//replay_special_events(); // experiment on 230803
				};
				if (time_travel_enabled() && tt_log_version_number >= 41) { // ordinary demos already do this -- new on 050404
					set_recordable_ini_options();
				} else if (tt_log_version_number < 41) {
					// set these to the values they had before these features were invented
					if (tt_log_version_number < 27) {
						tt_vacuum_maximum_item_count = -1; // prior to version 27 this was "infinite" - negative numbers act that way
					};
					if (tt_log_version_number < 29) {
						tt_vacuum_used_once_per_click = FALSE;
						tt_expander_usage_maximum = -1; // no maximum
					};
					if (tt_log_version_number < 30) {
						tt_speak_button_name = FALSE;
						tt_parts_change_when_bammed = FALSE;
						tt_good_sizes_adjust_to_screen_size = FALSE; // I think
					};
					if (tt_log_version_number < 32) {
						tt_number_shrinkage = .95;
					};
					if (tt_log_version_number < 34) {
						tt_right_to_left_text = FALSE;
						tt_scrolling_ok = TRUE;
					};
					if (tt_log_version_number < 41) {
						tt_match_blank_with_sensor_then_run_once_per_cycle = FALSE; // old behavior
						tt_robot_drop_location_follows_camera = TRUE; // new on 0504004
					};
				};
		tt_paragraph_id_just_finished = -1; 
		// new on 050204 so time travel segments don't have any lingering signals that cut off talking prematurely
		if (tt_sound_option&SOUND_BIT && tt_creating_narration_file == NARRATION_CREATION_OFF) { // second conjunct added 070201
			if (speak_file_name == NULL) {
				speak_file_name = add_speak_extension(replay_log_file_name,TRUE); // was log_file_name prior to 190103
			};
			OFSTRUCT file_struct;
			// check for existence to avoid having to ignore an API failure...
//			speak_file = OpenFile(speak_file_name,&file_struct,OF_EXIST);
//			if (speak_file != -1) {
			if (speak_file_name != NULL && tt_debug_mode != 1540 && tt_debug_mode != 141299) { // last != was == prior to 040300
				 // otherwise is stuff set by reading start.log or subtitles
				speak_file = OpenFile(speak_file_name,&file_struct,OF_READ);
			};
		};
		if (speak_file != -1) {
			char cookie[9];
			_lread(speak_file,cookie,8);
			cookie[8] = 0; // terminate string
			int version_number = atoi(cookie+5);
			if (strncmp(cookie,"TnTkS",5) != 0 || version_number == 0) {
				stream << "Sorry, but I think that the file "
						 << speak_file_name 
						 << " is not a good ToonTalk demo narration file. Narration turned off." << endl; 
				speak_file = -1; // as if error in opening
			} else {
			   _lread(speak_file,(unsigned char *) &max_speak_count,sizeof(max_speak_count));
			   m25_speak_starts = new millisecond[max_speak_count];
			   _lread(speak_file,(unsigned char *) m25_speak_starts,sizeof(millisecond)*max_speak_count);
            m22_speak_starts = new millisecond[max_speak_count];
			   _lread(speak_file,(unsigned char *) m22_speak_starts,sizeof(millisecond)*max_speak_count);
			   speak_sizes = new long[max_speak_count];
			   _lread(speak_file,(unsigned char *) speak_sizes,sizeof(long)*max_speak_count);
			  	if (tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES || tt_marty_talk == MARTY_SPEAKS_AND_TALK_BALLOONS) { // new on 130300
					tt_marty_talk = MARTY_TALK_BALLOONS; // they interfere so
					tt_closed_caption_alternative_language = FALSE;
					tt_closed_caption = FALSE;
				};
#if TT_DEBUG_ON
			  if (tt_debug_mode == 123456) { // want to regenerate the s*.wav files
				  char file_name[64];
#if !TT_32
				  HGLOBAL speak_memory_handle;
#endif
				  for (int i = 0; i < max_speak_count; i++) {
					  long speak_size = speak_sizes[i];
					  if (speak_size != 0) { // not skipping this one
#if TT_DIRECTX
                    bytes original_speak = new unsigned char[speak_size];
                    _hread(speak_file,original_speak,speak_size);
                    bytes speak = original_speak;
#else
						  sound_buffer original_speak
							  = read_sound(speak_file,speak_size
#if !TT_32
												,speak_memory_handle
#endif
												 );
                    sound_buffer speak = original_speak;
#endif
						  output_string_stream name(file_name,64);
                    if (i >= 9) {
						     name << "s" << (i+1) << ".wav";
                    } else {
  						     name << "s0" << (i+1) << ".wav";
                    };
						  name.put('\0');
						  OFSTRUCT of_struct;
						  HFILE file = OpenFile(file_name,&of_struct,OF_CREATE);
						  while (speak_size > 32000L) {
						     _lwrite(file,(ascii_string) speak,32000L);
						     speak_size -= 32000L;
						     speak += 32000L;
						  };
                	  _lwrite(file,(ascii_string) speak,speak_size);
						  _lclose(file);
#if TT_32
						  delete [] original_speak;
#else               
						  GlobalUnlock(speak_memory_handle);
						  GlobalFree(speak_memory_handle);
#endif
					  };
				  };
				  _lclose(speak_file); // too hard to reposition it
              tt_exit_failure(DONT_TROUBLE_SHOOT,_T("Wave files saved -- restart"));
				  return(FALSE);
			  };
#endif
			};
		};
		if (!(tt_sound_option&SOUND_BIT) && 
			 (tt_marty_talk == MARTY_SPEAKS_WITH_SUBTITLES || tt_marty_talk == MARTY_SPEAKS_AND_TALK_BALLOONS)) { 
			// new on 281101
			tt_marty_talk = MARTY_TALK_BALLOONS; // so demos work right if there is no sound card
			tt_closed_caption_alternative_language = FALSE;
			tt_closed_caption = FALSE;
		};
		// following moved here on 141299 since subtitles_file_name used now in second conditional below
#if TT_ALPHA_FEATURE
		if (subtitles_file_name == NULL && tt_log_in_archive != NULL) {
			update_subtitles_file_name(tt_log_in_archive);
		} else // otherwise leave it alone
#endif
		update_subtitles_file_name(log_file_name); // was log_file_name prior to 190103 - and still is???
		if (tt_debug_mode == 1540) { // read start times from ascii file named start.log (not a documented feature)
			m25_speak_starts = new millisecond[max_speak_count_ever];
         m22_speak_starts = new millisecond[max_speak_count_ever];
		   speak_sizes = new long[max_speak_count_ever];
         input_file_stream start_times;
         start_times.open("start.log",std::ios_base::in); // std::ios_base::nocreate); 
         for (short int i = 0; i < max_speak_count; i++) {
            if (start_times != NULL) {
               start_times >> m25_speak_starts[i];
            } else {
               max_speak_count = i;  // will stop the iteration as well
               m25_speak_starts[i] = 0;
				};
         };
         speak_starts_changed = TRUE;
      } else if (tt_creating_narration_file == NARRATION_CREATION_USING_SUBTITLES || tt_debug_mode == 141299) {
			// first disjunct is the documented way of doing tis
			m25_speak_starts = new millisecond[max_speak_count_ever];
         m22_speak_starts = new millisecond[max_speak_count_ever];
		   speak_sizes = new long[max_speak_count_ever];
			if (initialize_subtitles()) {
				int i = 0;
				m25_speak_starts[i++] = next_subtitle_time;
				short ignore_length;
				while (subtitles_file != NULL) {
					while (get_next_subtitle_line(ignore_length)) {}; 
					// skip over subtitles -- changed to really_ on 170204 - removed as experiment
//					if (subtitles_file != NULL) { // conditional new on 170204
						subtitles_file >> m25_speak_starts[i++];
						next_line_new_set = TRUE;
//					};
				};
            max_speak_count = i-1;  
            m25_speak_starts[max_speak_count] = 0;
				speak_starts = m25_speak_starts;
		      speak_starts_changed = TRUE;
				release_subtitles();
				save_speak_starts(TRUE);
				exit(0);
			};
		};
		initialize_speak_starts();
      if (speak_file != -1 && !tt_tts_uses_direct_sound) { // || tt_log_version_number < 15 
			// log version added on Oct 11 '98 since Marty doesn't speak well during most demos
			// probably due to mixture of real clock and replay clock
			tt_marty_talk = MARTY_TALK_BALLOONS; // can't have both unless mixing is on
		};
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 1313) {
		char delta[20];
		MessageBoxA(NULL,itoa((timeGetTime()- start_time),delta,10),"Log open time",MB_OK);
	//} else if (tt_debug_mode == 110100 || tt_debug_mode == 160299) {
	//	tt_error_file() << "Finished opening log and tellg() = " << (int) log_in->tellg() << endl;
	};
#endif
//	if (tt_time_travel != TIME_TRAVEL_OFF) {
//		add_time_travel_buttons();
//	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 210105) {
		int tell = log_in->tellg();
		tt_error_file() << "TellG after open_log is " << tell << endl;
	};
#endif
	return(TRUE);
};

void initialize_speak_starts() {
   if (m25_speak_starts == NULL) {
		m25_speak_starts = new millisecond[max_speak_count];
   };
   if (m22_speak_starts == NULL) {
	   m22_speak_starts = new millisecond[max_speak_count];
   };
//   if (tt_video_mode == 25) { // not using this feature just yet
	   speak_starts = m25_speak_starts;
//   } else {
//	   speak_starts = m22_speak_starts;
//   };
};

ascii_string replay_file_name() {
	return(replay_log_file_name);
};

boolean set_replay_file_name(ascii_string file_name) {
	// simplifed this on 200703 - commented out redundant or dead parts
	if (replay_log_file_name != NULL) {
		delete [] replay_log_file_name; // new on 261000
//		replay_log_file_name = NULL;
	};
	if (file_name[0] == '"') { // new on 260601 to remove quotes
		int length = strlen(file_name);
		if (file_name[length-1] == '"') {
			file_name[length-1] = '\0';
			replay_log_file_name = add_demo_extension(file_name+1,TRUE);
			file_name[length-1] = '"';
			return(replay_log_file_name != NULL); // added on 290204 since conditional below removed (when? why?)
		};
	};
//	if (replay_log_file_name == NULL) { // not set above
		replay_log_file_name = add_demo_extension(file_name,TRUE);
	//};
	return(replay_log_file_name != NULL);
};

millisecond accumulation = 0;

millisecond replay_clock(boolean accumulate) {
	if (log_in == NULL) { // has happened during time travel - not clear how (maybe hitting play after pausing?) - new on 220903
		throw(END_OF_LOG_FILE);
	};
	short unsigned int short_millisecond_delta;
	log_in->read((string) &short_millisecond_delta,sizeof(short_millisecond_delta));
#if TT_DEBUG_ON
	if (short_millisecond_delta == 0) {
		int tell = log_in->tellg();
		tt_error_file() << "TellG=" << tell << endl;
		static boolean reported = FALSE;
		if (!reported) {
			debug_this();
			reported = TRUE;
		};
	};
#endif
	millisecond millisecond_delta = short_millisecond_delta; // new on 130204
	while (short_millisecond_delta == max_unsigned_short && !log_in->empty()) {
		log_in->read((string) &short_millisecond_delta,sizeof(short_millisecond_delta));
		millisecond_delta += short_millisecond_delta;
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 110100 || tt_debug_mode == 160299 || tt_debug_mode == 40803 || 
		 tt_debug_mode == 140103 || tt_debug_mode == 230304 || tt_debug_mode == 301204) {
		int tell = log_in->tellg();
		tt_error_file() << millisecond_delta << " millisecond delta read. TellG=" << tell  
							 << " frame=" << tt_frame_number << "; segment=" << tt_current_log_segment << endl;
	};
	if (tt_debug_mode == 120204) {
		tt_error_file() << (((tt_current_log_segment-1)*tt_time_between_new_logs)+(tt_current_time-time_log_started))*.001 
							 << " is the calculated time. ";
		if (log_in->empty()) { 
			tt_error_file() << "Segment ended. Current time is " << tt_current_time*.001 << endl;
		} else {
			tt_error_file() << millisecond_delta << " millisecond delta read." << endl;
		};
		static long first_current_time = tt_current_time;
		tt_error_file() << (tt_current_time-first_current_time)*.001  << " is the current time. Segment number is " 
							 << tt_current_log_segment << endl;
		static long start_time = timeGetTime();
		tt_error_file() << (timeGetTime()-start_time)*.001 << " is the real time." << endl;
	};
#endif
	if (log_in->empty()) { // reached end of log
		millisecond_delta = 0; // new on 120204 -- important since a big value can trigger extra_time via saving previous_delta
		if (tt_time_travel != TIME_TRAVEL_OFF) { // new on 180703
			throw(END_OF_LOG_FILE);
		};
		if (close_input_log(TRUE,FALSE)) { 
			// second arg was TRUE prior to 210300 and prior to 180703 was (tt_time_travel != TIME_TRAVEL_OFF)
			play_sound(SPIRAL_OUT_SOUND);
			millisecond_delta = 200; // in case we want to continue
		} else {
			return(replay_clock(accumulate));
		};
   } else if (accumulate) {
      if (millisecond_delta == 0) {
         accumulation++;
      } else {
         accumulation += millisecond_delta;
      };
	} else if (millisecond_delta != 0) { // should be zero only in titles
	//	millisecond_delta = 1; // commented out since this makes titles stay up to long while replaying a log - new on 170903
	//} else {
      if (speak_sizes != NULL && speak_index < max_speak_count && tt_sound_option != 0 && 
			 tt_titles_ended_on_frame <= tt_frame_number) { 
		   // narration to take care of -- added tt_titles_ended_on_frame <= tt_frame_number on 100204 -- updated 230804
		   synchronize_narration(millisecond_delta);
      };
//      if (tt_subtitles_speed > 0) {
//         synchronize_subtitles(millisecond_delta);
//      };
	};
   if (!accumulate && accumulation != 0) {
      millisecond result = accumulation;
      accumulation = 0;
      return(result);
   };
   if (accumulate) return(accumulation);
	return(millisecond_delta);
};

wide_character segment_rerecorded_attribute[] = L"SegmentRerecorded_0000000000";

boolean rerecorded_segment(int segment_number) { // new on 200704
	if (tt_zip_description_archive_element == NULL) {
		return(TRUE); // new on 180105 since had crash running demo test -- not clear how -- maybe called while exiting?
	};
	_itow(segment_number,segment_rerecorded_attribute+18,10); // clobber the zeroes
	return(xml_get_attribute_int(tt_zip_description_archive_element,segment_rerecorded_attribute,0));
};

void set_rerecorded_segment(int segment_number) { // new on 200704
	_itow(segment_number,segment_rerecorded_attribute+18,10); // clobber the zeroes
	xml_set_attribute(tt_zip_description_archive_element,segment_rerecorded_attribute,L"1");
};

void remove_rerecorded_segment(int segment_number) { // new on 240904
	if (tt_zip_description_archive_element == NULL) return; // too early to do anything
	_itow(segment_number,segment_rerecorded_attribute+18,10); 
	xml_remove_attribute(tt_zip_description_archive_element,segment_rerecorded_attribute);
};

boolean jump_to_log_segment(int version_number, boolean possibly_successive) { 
	// possibly_successive new on 100204 and maybe is really is successive
	if (tt_logging) log_finalization();
//#if TT_DEBUG_ON
//	if (tt_debug_mode == 170105 && version_number == 18) {
//		tt_debug_frame_number = tt_frame_number+1;
//	};
//#endif
	recording_paused = FALSE; // new on 140204 -- if recording was paused and then jumped it no longer is just "paused"
	tt_real_time_pause_began = 0; // new on 140204 since again not recording that was paused
	if (version_number != tt_current_log_segment+1) { // added condition that this isn't the next log on 030204
		stop_speaking(); // so Marty doesn't try to finish his sentence in the new time
	};
 	if (tt_time_travel == TIME_TRAVEL_ON) {
		if (tt_replay != REPLAY_REPRODUCE_TIMING_ON_NEXT_FRAME) {
			set_replay(REPLAY_REPRODUCE_TIMING);
		};
#if TT_DEBUG_ON
		if (log_in == NULL) {
			debug_this();
		};
#endif
	} else if (tt_time_travel == TIME_TRAVEL_PAUSED) {
//		tt_screen->invalidate_cache();
		tt_time_travel = TIME_TRAVEL_JUST_PAUSED;
//		update_time_travel_buttons(); // commented out since next log could change everything anyway
	};
#if TT_ALPHA_FEATURE
	if (tt_time_travel != TIME_TRAVEL_ON || version_number != tt_current_log_segment+1) { // didn't just jump here
		reset_next_subtitle_time(version_number); // new on 271003 to find the right subtitle when jumping in time
	};
#endif
	if (version_number == 0) version_number++; 
	// skip the titles segment - new on 180703 so that if running segment will begin to run as well - changed 1 to 0 on 190703
	boolean increasing = (version_number >= tt_current_log_segment);
	if (possibly_successive && rerecorded_segment(version_number) && tt_debug_mode != 141004) { // for debugging can ignore this
		// re-recorded (i.e. record button hit while replaying) so not quite successive (but maybe could be made to be so -- with effort)
		possibly_successive = FALSE;
	};
	try {
		return(close_input_log(TRUE,TRUE,version_number,possibly_successive)); 
		// replaced FALSE with possibly_successive on 100204
		// FALSE added on 040803 so not confused into thinking there are successive demos when jumping
	} catch (CycleInterruptionReason reason) {
#if TT_DEBUG_ON
		debug_this();
#endif
		if (reason == LOG_FILE_MISSING && version_number < tt_youngest_log_segment) { // new on 031103
			if (increasing) {
				return(jump_to_log_segment(version_number+1));
			} else {
				return(jump_to_log_segment(version_number-1));
			};
		} else {
			return(FALSE);
		};
	};
//	return(close_input_log(TRUE,TRUE,version_number,FALSE));
};

long extract_version_number(string demo_file_name) {
	char number_string[number_of_digits+1];
	int length = strlen(demo_file_name);
	memcpy(number_string,demo_file_name+length-(4+number_of_digits),number_of_digits);
	number_string[number_of_digits] = '\0';
	return(atol(number_string));
};

boolean reported_different_exe_warning = FALSE; // new on 190703 since with time travel this can happen several times

boolean close_input_log(boolean normal, boolean run_next_dmo, long version_number, boolean possibly_successive) {
	if (demo_created_by_exe_name != NULL && !reported_different_exe_warning) { // new on 170301
		if (stricmp(demo_created_by_exe_name,AC(IDC_TOONTALK_EXE_NAME)) != 0) {
			log("Warning: running a log created with a different version of ToonTalk.");
			tt_error_file() << "If this demo didn't replay correctly, try running it with " << demo_created_by_exe_name << endl;
			reported_different_exe_warning = TRUE;
		};
		delete [] demo_created_by_exe_name;
		demo_created_by_exe_name = NULL; // new on 110103
	};
	int position = -1;
	if (log_in != NULL && !log_in->empty()) position = log_in->tellg(); // conditional new on 210300
	if (replay_log_file_name != NULL) {
		if (log_in != NULL) { // condition new on 140104
			log_in->close();
			if (!run_next_dmo) { // new on 130104 to fix a tiny leak
				delete log_in;
				log_in = NULL;
			};
#if TT_DEBUG_ON
			if (tt_debug_mode == 140103 || tt_debug_mode == 110100) {
				tt_error_file() << "Just closed " << replay_log_file_name << " that was open for input at " << tt_current_time  
									 << " on frame " << tt_frame_number << endl;
			};
#endif
		};
		if (possibly_successive) { // conditional new on 040803
			last_closed_log_segment = tt_current_log_segment;
		} else {
			last_closed_log_segment = -2; // new on 040803
		};
		if (tt_delete_replay_file_after_reading) { // then delete the extracted file - new on 100703
			DeleteFile(replay_log_file_name);
			tt_delete_replay_file_after_reading = FALSE; // until reset again
		};
	};
	if (!normal && run_next_dmo) return(TRUE); // new on 120100 to postpone opening next log until right context
	if (run_next_dmo && (replay_log_file_name != NULL || version_number > 0)) { // NULL check added on 071299 for robustness
		boolean new_name = FALSE;
		string next_demo_file_name;
		if (version_number > 0) {
			if (replay_log_file_name == NULL) {
				replay_log_file_name = append_strings(tt_user_directory,"log00000.dmo");
			};
			int length = strlen(replay_log_file_name);
			string digits = replay_log_file_name+length-(4+number_of_digits);
			int version_number_copy = version_number; // new on 110603 since use version_number later
			for (int i = number_of_digits-1; i >= 0 ; i--) {
				digits[i] = (char) ('0'+(version_number_copy%10));
				version_number_copy /= 10;
			};
			next_demo_file_name = replay_log_file_name;
		} else {
			next_demo_file_name = next_file_name(replay_log_file_name,new_name,FALSE);
		};
		if (next_demo_file_name != NULL) { // new on 130999 so goes on to next demo automatically
			if (new_name) {
				delete [] replay_log_file_name;
				replay_log_file_name = next_demo_file_name;
			};
			if (tt_time_travel != TIME_TRAVEL_OFF || tt_current_log_segment > 0 || file_exists(replay_log_file_name)) {
				// added tt_time_travel != TIME_TRAVEL_OFF on 130103
				// no need to check if it exists if time traveling (besides is checked within open_log)
				// only reason to check at all is to avoid re-initializing below
				// added tt_current_log_segment > 0 on 040204 to help -segment command line work
#if TT_DEBUG_ON
				if (tt_debug_mode == 110100 || tt_debug_mode == 160299) {
					tt_error_file() << "Starting log " << replay_log_file_name << " on frame " << tt_frame_number << endl;
				};
#endif
				if (tt_time_travel == TIME_TRAVEL_OFF) { // commented out the following on 150903 || !tt_dump_as_xml) { 
					// conditions new on 110103 -- experimented with turning these off for time travel on 100603
					delete tt_programmer;
					tt_programmer = new Programmer();
					stop_sound(FALSE); 
					// in case a repeating sound is going on -- note helicopter sound is restarted by programmer::load if need be
					tt_screen->initialize_background(tt_city); // just in case loading cares...
					// following replaced by reset_toontalk() on 140603
					//release_queue_and_robots(); // added on 281199 
					//tt_global_picture->destroy(); // new on 051199
					//tt_global_picture = new GlobalPicture(); // new on 051199
					//tt_global_picture->become_blank(); // copied here on 011002 since used as well as typical blank picture
					// not clear with XML if the following is needed anymore since these objects will be reset
//					// following removed on 131003 since reset_toontalk will just undo this work so very wasteful
//					if (tt_martian != NULL) { // new on 171299
//						tt_martian->remove_from_floor(FALSE); // new on 170403
//						tt_martian->destroy(); // create a fresh one
//						tt_martian = new Martian();
//					};
//					if (tt_toolbox != NULL) { // new on 171299
////						Notebook *notebook = pointer_to_toolbox_notebook();
//						// commented out the following on 200100 since when city is initialized this stuff is destroyed 
						// (even if ref_count is > 0) (uses tt_shutting_down)
////						set_toolbox_notebook(NULL); // stole it...
////						tt_toolbox->destroy(); // create a fresh one
//						tt_toolbox = new Toolbox(); // notebook); // arg new on 130100
//						tt_toolbox->initialize(); // new on 100603
//					};
					reset_toontalk();
					recreate_after_reset(); // do this after reset - new on 131003
				//} else {
				//	tt_city->initialize(); // new on 120603 - does the other branch still make sense?
				};
				tt_prevous_log_segment = tt_current_log_segment; // new on 041103
				tt_current_log_segment = extract_version_number(replay_log_file_name); 
//				tt_include_location_at_the_start_of_segment = tt_log_out_archive_size; 
				// a very bad idea to use zip index numbers - noted 150204
				// don't really need to maintain tt_log_out_archive_size anymore once the above is rationalized or removed
				//above used to be just after openning but that confused debugging messages while loading 
				// since it didn't know the segment changed
#if TT_DEBUG_ON
				if (tt_debug_mode == 100703) {
					tt_error_file() << "About to open log segment " << tt_current_log_segment << endl;
				};
#endif
				if (open_log(replay_log_file_name,tt_error_file(),FALSE,FALSE,FALSE,(version_number == 1))) {			
					if (tt_system_mode == PUZZLE) {
						release_puzzle_data(TRUE);
						allocate_puzzle_data();
						tt_city->initialize(); 
						delete tt_programmer;
						tt_programmer = new Programmer();
						tt_screen->initialize_background(tt_city); 
						tt_martian = new Martian(); // may have been clobbered when city was initialized
						tt_city->build_initial_houses();
					}; // otherwise already initialized in the log file
//					tt_skip_titles = TRUE; // gobble up the titles part of the log
					// following moved here on 180703 since if moved to next segment 
					// (whether by jumping or because previous one finished) need to initialize this
					if (tt_time_travel != TIME_TRAVEL_ON) { 
						// conditional new on 200703 - 
						// not needed if TIME_TRAVEL_ON and breaks if running time travel archive as demo 
						// (since tt_screen has no background yet)
						tt_stop_all_pictures = TRUE; 
						// new on 171004 since the following can call update_display and pictures with speeds will move
						tt_screen->propagate_changes(); 
						// needed so that tool buttons are in the right place (experimented on 151004 to discover this)
						tt_stop_all_pictures = FALSE; // new on 171004
//						tt_screen->update_display(); 
						// commented out on 141004 since this can cause pictures to move if they have a speed
						// commented out the following on 081004 -- 
						// seems OK and avoids mismatched hands and arms when jumping in time travel
						//if (tt_programmer != NULL && 
						//	 tt_programmer->pointer_to_appearance() != NULL && 
						// new on 150903 since some crash reports indicated it can be NULL
						//	 tt_programmer->pointer_to_appearance()->appearance_code() == HAND_HOLDING_WAND_SPRITE) { 
						// new condition on 081004
      //               tt_programmer->pointer_to_appearance()->update_display(); 
						// new on 180603 - otherwise hand holding wand is broken
						//};
					};
					return(FALSE); // opened a new log file 
				} else if (tt_time_travel == TIME_TRAVEL_ON) { // reached end of last log so take over and stop time travelling
					time_travel(TIME_TRAVEL_JUST_PAUSED);
//					if (tt_time_between_new_logs > 0) {
//						tt_next_new_log_time = tt_current_time+tt_time_between_new_logs;
//					};
				};
			};
		};
	};
	stop_replay();
	if (tt_city_document_to_load_after_demo_ends != NULL) { // new on 240105
		//tt_city->handle_xml(tt_city_document_to_load_after_demo_ends);
		//tt_city_document_to_load_after_demo_ends->Release();
		tt_city_document_to_load = tt_city_document_to_load_after_demo_ends;
		tt_city_document_to_load_after_demo_ends = NULL;
	};
#if !TT_NO_ROBOT_TRAINING
	if (tt_creating_narration_file != NARRATION_CREATION_OFF) { // new on 070201
		save_speak_starts(normal);
	};
	if (tt_log_out_file_name != NULL && normal && !time_travel_enabled() && position > 0) { // new on 030300 
		// added !time_travel_enabled() on 150103 and position > 0 on 140103
		// if -i and -o are used then -o is just the additional stuff - need to use copy /b x+y z
		string new_log_file_name = add_demo_extension(tt_log_out_file_name,FALSE);
		if (tt_log_count_max != 0) { // new on 250603 to keep time travel in a zip archive -- was > 0 prior to 280803
			open_zip_file(log_out_archive(),new_log_file_name);
//			tt_log_out_archive_size++;
		} else {
			log_out.open(new_log_file_name,std::ios_base::out | std::ios_base::binary | std::ios_base::trunc ); 
			// added trunc to delete current if it exists
			if (log_out == NULL) { // new on 290904 for error recovery
				unable_to_write_file(new_log_file_name); 
				return(FALSE);
			};
		};
#if TT_DEBUG_ON
		if (tt_debug_mode == 140103) {
			tt_error_file() << "Just opened[2] " << new_log_file_name << " for output at " << tt_current_time 
								 << " on frame " << tt_frame_number << endl;
		};
#endif
		delete [] tt_log_out_file_name;
		tt_log_out_file_name = new_log_file_name;
		tt_logging = TRUE;
		tt_exit_at_end_of_log = FALSE;
		truncate(replay_log_file_name,position); // in case control was taken of the demo to be extended
		return(TRUE); // return now to keep going with subtitles and narration
	};
#endif
//	if (!tt_logging) { // still producing an output log -- huh?? - commented out on 070201 since done above now
//		if (tt_debug_mode != 1785) {
		  // otherwise want to keep SPK and update DMO
//			save_speak_starts(normal);
//		};
//	};
  if (speak_file_name != NULL) {
	  delete [] speak_file_name;
	  speak_file_name = NULL;
  };
  if (subtitles_file_name != NULL && (!tt_subtitles_and_narration_even_without_demo)) { 
	  //	  || !tt_logging removed with TT_ALPHA_FEATURE (271003)
     delete [] subtitles_file_name;
     subtitles_file_name = NULL;
  };
  if (!normal && tt_exit_at_end_of_log && tt_debug_mode != 19) {
//     tt_exit_failure(TROUBLE_SHOOT_BAD_DEMO,S(IDS_LOG_REPLAY_ERROR));
     trouble_shoot(TROUBLE_SHOOT_BAD_DEMO,S(IDS_LOG_REPLAY_ERROR));
     set_user_wants_to_quit(TRUE);
//  } else {
//     tt_new_input_scheme = TRUE; // should really be what it was before log started but good enough
  };
  return(TRUE);
};

boolean speak_starts_saved = FALSE;

void save_speak_starts(boolean normal) {
	if (speak_starts_saved) return;
	speak_starts_saved = TRUE;
//   if (tt_video_mode == 25) {
		m25_speak_starts = speak_starts;
//	} else {
//		m22_speak_starts = speak_starts;
//	};
	boolean special_mode = (tt_creating_narration_file != NARRATION_CREATION_OFF || 
		// added on 070201 since this replaces the older tt_debug_mode == 1540 and tt_debug_mode == 141299
									tt_debug_mode == 5871 || tt_debug_mode == 1540 || tt_debug_mode == 141299);
	string base_file_name = tt_log_out_file_name;
	if (base_file_name == NULL) { // new on 070201 - earlier just used tt_log_out_file_name
		base_file_name = replay_log_file_name;
	};
	if (base_file_name != NULL && ((normal && speak_starts_changed && speak_index > 0) || special_mode)) {
		// check for tt_log_out_file_name != NULL added 070201
		if (!special_mode) { // conditional commented out on 070201 since if there are more Sxx.WAV files than times no point including them
			// conditional restored on 100204 to generate new narrations for Lennart
			max_speak_count = speak_index;
		};
//		output_file_stream speak_file(speak_file_name,std::ios_base::out|std::ios_base::binary);             
		OFSTRUCT file_struct;
      if (speak_file_name == NULL) {
			speak_file_name = add_speak_extension(base_file_name,FALSE);
		};
		speak_file = OpenFile(speak_file_name,&file_struct,OF_WRITE|OF_CREATE);
		// start with magic cookie and version number for ToonTalk Speak files
		_lwrite(speak_file,AS(IDS_NARRATION_FILE_COOKIE),8);
		// should (char *) really be (unsigned char *) ??
		_lwrite(speak_file,(char *) &max_speak_count,sizeof(max_speak_count));
		_lwrite(speak_file,(char *) m25_speak_starts,sizeof(millisecond)*max_speak_count);
		_lwrite(speak_file,(char *) m22_speak_starts,sizeof(millisecond)*max_speak_count);
		char wave_file_name[24];
//		OFSTRUCT file_struct;
		speak_sizes = new long[max_speak_count];
      int i;
		for (i = 0; i < max_speak_count; i++) {
			wave_name(i,wave_file_name);
			int file = open(wave_file_name,0);
//			if (file == ENOENT) {
//				speak_sizes[i] = 0;
//			} else {
         if (file < 0) { 
  				speak_sizes[i] = 0;
         } else {
				speak_sizes[i] = filelength(file);
//				if (speak_sizes[i] == -1) speak_sizes[i] = 0;
				close(file);
			};
		};
		_lwrite(speak_file,(char *) speak_sizes,sizeof(long)*max_speak_count);
		for (i = 0; i < max_speak_count; i++) {
			long size = speak_sizes[i];
			const int chunk_size = 10000;
			char *buffer = new char[chunk_size]; // would unsigned char be more appropriate?
			if (size > 0) {
				wave_name(i,wave_file_name);
				HFILE file = OpenFile(wave_file_name,&file_struct,OF_READ);
				while (size > 0) {
					if (size < chunk_size) {
						_lread(file,buffer,size);
						_lwrite(speak_file,buffer,size);
						size = 0;
					} else {
						_lread(file,buffer,chunk_size);
						_lwrite(speak_file,buffer,chunk_size);
						size -= chunk_size;
					};
				};
				_lclose(file);
			};
			delete [] buffer;
		};
//		speak_file.close();
//		close(speak_file);
		_lclose(speak_file);
	};
	if (speak_sizes != NULL) {
		delete [] speak_sizes;
		speak_sizes = NULL; // new on 220803
	};
	release_speak_starts();
};

void release_speak_starts() { // abstracted on 170103
	if (m25_speak_starts != NULL) {
		delete [] m25_speak_starts;
		m25_speak_starts = NULL;
	};
	if (m22_speak_starts != NULL) {
		delete [] m22_speak_starts;
		m22_speak_starts = NULL;
	};
};

void replay_events_counter(unsigned short int &counter) {
	if (tt_log_version_number < 38) { // this token isn't needed anymore as of 290204
		unsigned char token = log_in->get();
		if (token != EVENT_COUNTER_TOKEN) {
			counter = 1;
			log(S(IDS_EVENT_TOKEN_MISSING),FALSE,(tt_debug_mode != 19)); // last arg was TRUE prior to 160702
			tt_error_file() << "Read " << (int) token << " at " << (int) log_in->tellg() << " in " << replay_log_file_name << endl;
			tt_error_file() << "Following bytes are: "; // look at the next 10 bytes
			for (int i = 0; i < 10; i++) { 
				tt_error_file() << (int) log_in->get() << " ";
			};
			tt_error_file() << endl;
			if (tt_time_travel != TIME_TRAVEL_OFF) { // new on 180703
#if TT_DEBUG_ON
				debug_this();
#endif
				throw(EVENTS_COUNTER_MISSING);
			};
			close_input_log(FALSE,FALSE); // second arg was TRUE prior to 210300 - prior to 180703 was (tt_time_travel != TIME_TRAVEL_OFF)
		};
	};
	log_in->read((string) &counter,sizeof(counter));
#if TT_DEBUG_ON
   if (tt_debug_mode == 1021 || tt_debug_mode == 110100 || tt_debug_mode == 160299 || tt_debug_mode == 202 || tt_debug_mode == 230304) {
      tt_error_file() << "Reading event counter = " << counter << " on frame " << tt_frame_number << endl;
   };
#endif
};

boolean interpret_event_flags(byte event_encoding) {
   if (event_encoding&TT_HAND_NEEDS_TO_MOVE_FOR_TOOL_USE_FLAG) {
      tt_hand_needs_to_move_for_tool_use = TRUE;
   };
   if (event_encoding&TT_HAND_NEEDS_TO_MOVE_TO_CURSOR_FLAG) {
      tt_hand_needs_to_move_to_cursor = TRUE;
   };
   if (event_encoding&TT_HAND_NEEDS_TO_MOVE_RELATIVELY_FLAG) {
      tt_hand_needs_to_move_relatively = TRUE;
   };
   if (event_encoding&TT_DELTA_X_AND_Y_DUE_SOLELY_TO_ARROW_KEYS_FLAG) {
      tt_delta_x_and_delta_y_due_solely_to_arrow_keys = TRUE;
   };
   if (event_encoding&TT_DRAGGING_ENABLED_FLAG) {
      tt_dragging_enabled = TRUE;
   };
   return(event_encoding&INPUT_EVENT_FLAG);
};

boolean replay_user(boolean &event,
						  unsigned char &key,
						  boolean &extended_key,
						  city_coordinate &delta_x,
						  city_coordinate &delta_y,
						  city_coordinate &mouse_delta_x, // new on 051100
						  city_coordinate &mouse_delta_y,
						  char &mouse_button_status,
                    char &joystick_button_status,
						  long &duration,
						  unsigned char &second_byte) {
  //  tt_programmer->load_city_if_need_be(); // new on 210799
  // for now joystick button status is recorded combined with the mouse button status
  // might mess up logs that use remote sensors for mouse buttons if joystick was used while logging
  //  event = (boolean) log_in->get();
//  if (tt_titles_ended_on_frame <= tt_frame_number || tt_log_version_number < 39) { 
		// condition new on 230304 since now no events are recorded during titles
	  event = interpret_event_flags(log_in->get()); // rewritten on 220402 to have additional flags
  //} else {
	 // event = 0; // new on 230304
  //};
//  if (event > 1) {
//  	  play_sound(SPIRAL_IN_SOUND,5);
//#if TT_DEBUG_ON
//     if (tt_debug_mode != 5254) {
//	     say_error(IDS_BAD_DEMO_LOG);
//#endif
//	     log(S(IDS_BOOLEAN_EVENT_TOKEN_EXPECTED));
////	  tt_replay = FALSE;
//        close_input_log(FALSE,(tt_time_travel != TIME_TRAVEL_OFF)); // second arg was TRUE prior to 210300
//	     return(FALSE);
//#if TT_DEBUG_ON
//     } else {
//        log(_T("Ignoring bad event in log."));
//        event = FALSE;
//     };
//#endif
//  };
  boolean more = FALSE;
//  int location = log_in->tellg()-1;
  if (event && !log_in->empty()) {
//	  EventRecord event_record;
//	  log_in.read((unsigned char *) &event_record, sizeof(EventRecord));
//	  button_status = event_record.button_status;
//	  more = event_record.more;
//	  delta_x = event_record.delta_x;
//     delta_y = event_record.delta_y;
//	  duration = event_record.duration;
//	  key = event_record.key;
//	  extended_key = event_record.extended_key;
	  unsigned char status = (unsigned char) log_in->get();
	  more = !!((0x1<<2)&status);
	  extended_key = !!((0x1<<1)&status);
     if (!!((0x1<<3)&status)) { // was a joystick event
	     joystick_button_status = (unsigned char) log_in->get();
        mouse_button_status = 0;
     } else {
	     mouse_button_status = (unsigned char) log_in->get();
        joystick_button_status = 0;
     };
	  // added !! to the following so that they get a value of 0 or 1 for comparison with last frame's value
	  tt_shift_down = !!((1<<4)&status); // new on 220900
	  tt_control_down = !!((1<<5)&status); // new on 220900
	  key = (unsigned char) log_in->get();
	  if ((1<<6)&status) { // new on 220900
		  second_byte = (unsigned char) log_in->get();
	  } else {
		  second_byte = 0;
	  };
#if TT_DEBUG_ON
	  if (tt_debug_mode == 10400 && key > 0) {
		  int location = log_in->tellg()-1;
		  char hex[32];
		  itoa(location,hex,16);
		  tt_error_file() << key << " (" << (int) key << ") " << hex << " " << location << endl;
	  };
#endif
	  if (replaying() && key > 0) { // added 201199 since demos didn't work with keyboard sensors
		  add_key_to_buffer(key+extended_key*256);
	  };
	  short int temp;
	  log_in->read((string) &temp, sizeof(temp));
	  delta_x = temp;
	  log_in->read((string) &temp, sizeof(temp));
	  delta_y = temp;
	  if (tt_log_version_number > 23) {
		  log_in->read((string) &temp, sizeof(temp));
		  mouse_delta_x = temp;
		  log_in->read((string) &temp, sizeof(temp));
		  mouse_delta_y = temp;
	  };
	  log_in->read((string) &duration, sizeof(duration));
#if TT_DEBUG_ON
	  if (tt_debug_mode == 110100 || tt_debug_mode == 160299 || tt_debug_mode == 180402) { //  || tt_debug_mode == 2308031
		  tt_error_file() << "delta_x=" << delta_x << "; delta_y=" << delta_y << "; duration=" << duration
								<< "; key=" << (int) key << "; more=" << (int) more << "; extended_key=" << (int) extended_key
								<< "; joystick_status=" << (int) joystick_button_status
								<< "; mouse_status=" << (int) mouse_button_status 
								<< " TellG is " << log_in->tellg() << endl;
		};
#endif
  } else {
	  delta_x = 0;
	  delta_y = 0;
	  mouse_delta_x = 0;
	  mouse_delta_y = 0;
	  mouse_button_status = 0;
     joystick_button_status = 0;
	  duration = 0;
	  key = 0;
	  extended_key = FALSE;
	  second_byte = 0;
	  tt_shift_down = FALSE;
	  tt_control_down = FALSE;
  };
  return(more);
};

ascii_string add_demo_extension(const_ascii_string log_file_name, boolean input) {
	if (already_has_extension(log_file_name,AS(IDS_DEMO_EXTENSION)) && strchr(log_file_name,':') != NULL && file_exists(log_file_name)) {
		// changed == to != on 290204 since copy_string is only OK if does contain a : -- also added file_exists check (not clear I need both)
		// new on 100803 to check if both has extension and now also if full path (is checking for : enough)
		return(copy_string(log_file_name)); // new on 250100
	};
   return(add_extension(log_file_name,input,AS(IDS_DEMO_EXTENSION)));
};

ascii_string add_extension(const_ascii_string original_log_file_name, boolean input, const_ascii_string extension, boolean ok_to_bother_user) {
	ascii_string log_file_name = new char[MAX_PATH];
	strcpy(log_file_name,original_log_file_name); // but this will be reused - but this is OK to clobber
   ascii_string old_extension = strchr(log_file_name,'.');
	if (old_extension != NULL) { // already has an extension
		ascii_string last_old_extension = old_extension;
		while ((last_old_extension = strchr(last_old_extension+1,'.')) != NULL) { // added on 180599 in case there is more than 1 . in name 
			old_extension = last_old_extension;
		};
      log_file_name[strlen(log_file_name)-strlen(old_extension)] = NULL; // terminate at "."
   };
//      string copy = copy_string(log_file_name);
//      strcpy(copy+strlen(copy)-strlen(old_extension)+1,extension);
//		return(copy);
//	};
#if !TT_95
	if (strlen(log_file_name) > 8 && old_extension == NULL && strchr(log_file_name,'\\') == NULL) {
		log_file_name[8] = NULL; // truncate after 8 for non Win95 version
	};
#endif
	ascii_string file_name = new char[MAX_PATH];
//	output_string_stream stream(file_name,MAX_PATH); // rewritten 240104
//   char demo_dir[32] = "Demos";
//   if (stricmp(extension,AS(IDS_DEMO_EXTENSION)) == 0) { // two versions to deal with different resolutions
		// commented out on 210300 since too much trouble to support 2 resolutions of demos
//      if (tt_video_mode == 320) { // should this really be a test of graphics resolution - not screen now??
//         strcat(demo_dir,AC(IDS_LOW_RES_DEMO_DIRECTORY));
//      } else {
//         strcat(demo_dir,AC(IDS_HIGH_RES_DEMO_DIRECTORY));
//      };
//   };
	if (is_relative_path(log_file_name)) { // was strchr(log_file_name,'\\') == NULL prior to 020304 but that breaks input such as UK\intro_v2 
      if (input) {
//         stream << tt_main_directory << "Demos\\"; 
			// restored on 310502 since tt_user_directory may not be bound since user name not set (e.g. -I demo)
			strcpy(file_name,tt_main_directory);
		} else {
 //        stream << tt_user_directory << "Demos\\"; // re-written on 030402 to use user directory
			strcpy(file_name,tt_user_directory);
      };
		strcat(file_name,"Demos\\");
	} else {
		file_name[0] = '\0';
	};
   int demo_subdirectory_ends = strlen(file_name); // stream.pcount();
//   stream << log_file_name;
	strcat(file_name,log_file_name);
//   stream << "." << extension;
	strcat(file_name,".");
	strcat(file_name,extension);
//	stream.put('\0');
//   delete [] log_file_name; // delete the original
//	string file_name = stream.str();
	if (!input) {
		if (demo_subdirectory_ends > 0) { // condition new on 100803 to be safe
			file_name[demo_subdirectory_ends-1] = '\0'; // temporarily terminate string (new on 030402)
			::CreateDirectory(file_name,NULL);
			file_name[demo_subdirectory_ends-1] = '\\'; // restore it
		};
		delete [] log_file_name; // local copy
      return(file_name); //(ascii_string) stream.STR);
   };
	if (!file_exists(file_name)) { 
#if TT_POST3187
		string url_file_name = append_strings(file_name,".url"); // new on 300507
		if (file_exists(url_file_name)) {
			string true_file_name = extract_file_name_from_link(url_file_name);
			if (true_file_name != NULL) {
				FileNameStatus name_status;
				string local_cache = existing_file_name(name_status,true_file_name,extension,NULL,TRUE);
				delete [] true_file_name;
				if (local_cache != NULL) {
					delete [] file_name;
					delete [] log_file_name; // local copy
					return(local_cache);
				} else {
					message_box(MB_ICONSTOP|MB_OK|MB_SYSTEMMODAL,
						         "You asked for a feature that requires the ability to read files over the web. If you connect to the Internet, ToonTalk will be able to copy these files to your local disk and will be able to run without a net connection afterwards. Sorry.");	
				};
			};
		};
#endif
      if (strchr(log_file_name,'\\') != NULL) { // fully specified path given
         delete [] file_name;
//         return(NULL); 
			if (strchr(log_file_name,'\\') != NULL) { // fully specified path given
				// before giving up try again with just the simple file name
				int length = strlen(log_file_name);
				char short_name[MAX_PATH];
				int i = MAX_PATH-1;
				short_name[i--] = '\0'; // terminate and write string backwards
				for (;length >= 0; length--) {
					if (log_file_name[length] == '\\') break;
					short_name[i--] = log_file_name[length];
				};
				string result = add_extension(short_name+i+1,input,extension,FALSE); // don't bother user since might just have the same name
#if TT_ALPHA_FEATURE
				if (result == NULL && original_log_file_name == tt_log_in_archive) {
					// didn't find it but maybe it is in the archive -- new on 281003
					// think about having these in a regular (non time-travel) demo as well
					string short_name_with_extension = append_3_strings(short_name+i+1,".",extension);
					result = extract_file_from_archive(short_name_with_extension,tt_log_in_archive);
					//if (extract_archive_to_directory(tt_log_in_archive,short_name_with_extension,tt_extracted_file_temp_directory)) {
					//	// need to DeleteFile log_file_name after use
					//	result = append_strings(tt_extracted_file_temp_directory,short_name_with_extension);
					//};
					delete [] short_name_with_extension;
				};
#endif
				delete [] log_file_name; // new on 281003 to plug a minor leak
				return(result);
			};
      };
      FileNameStatus file_status;
      ascii_string result = existing_file_name(file_status,log_file_name,extension,"Demos",TRUE,ok_to_bother_user);
		delete [] file_name;
		delete [] log_file_name; // local copy
		return(result);
	} else {
		delete [] log_file_name; // local copy
		return(file_name); //(ascii_string) stream.STR);
	};
};

ascii_string add_speak_extension(const_ascii_string log_file_name, boolean input) {
   int name_length;
	for (name_length = 0; log_file_name[name_length] != 0; name_length++) {
		if (log_file_name[name_length] == '.') {
//			name_length--; // back up to before the .
			break;
		};
	};
	ascii_string file_name = new char[MAX_PATH]; // was 256 prior to 261000
//	output_string_stream stream(file_name,MAX_PATH); // rewritten 260104
	if (strchr(log_file_name,'\\') == NULL) {
//      stream << tt_main_directory << AC(IDS_DEMO_DIRECTORY) << "\\";
		strcpy(file_name,tt_main_directory);
		strcat(file_name,AC(IDS_DEMO_DIRECTORY));
		strcat(file_name,"\\");
	} else {
		file_name[0] = '\0';
	};
//	stream.write(log_file_name,name_length);
	int file_name_length = strlen(file_name);
	memcpy(file_name+file_name_length,log_file_name,name_length);
	file_name[file_name_length+name_length] = '\0'; // new on 300104 to terminate for future concat
   ascii_string speak_postfix = ini_entry(AC(IDS_FILE_EXTENSIONS),AC(IDS_NARRATION_FILE_SUFFIX_KEY),FALSE);
   if (speak_postfix == NULL) { // ini file explictly over-rode defaults
      speak_postfix = ASC(IDS_NARRATION_FILE_SUFFIX); // default is language dependent
   };
	if (speak_postfix != NULL) {
//		stream << "." << speak_postfix;
		strcat(file_name,".");
		strcat(file_name,speak_postfix);
	};
//	stream.put('\0');
//   delete [] log_file_name; // delete the original
//	ascii_string file_name = (ascii_string) stream.STR;
	if (!input) {
      if (speak_postfix != NULL) delete [] speak_postfix;
      return(file_name);
   };
	if (!file_exists(file_name,TRUE)) { 
      if (strchr(log_file_name,'\\') != NULL) { // fully specified path given
			// before giving up try again with just the simple file name
			int length = strlen(log_file_name);
			char short_name[MAX_PATH];
			int i = MAX_PATH-1;
			short_name[i--] = '\0'; // terminate and write string backwards
			for (;length >= 0; length--) {
				if (log_file_name[length] == '\\') break;
				short_name[i--] = log_file_name[length];
			};
			if (speak_postfix != NULL) delete [] speak_postfix; // new on 271000
			delete [] file_name; // new on 261000
			return(add_speak_extension(short_name+i+1,input)); 
		};
		string short_name = short_file_name(log_file_name); 
		// new on 230300 since has .dmo extension and that interferes with finding narration on CD-ROM
      FileNameStatus file_status;
      ascii_string result = existing_file_name(file_status,short_name,speak_postfix,AC(IDS_DEMO_DIRECTORY),TRUE,TRUE,FALSE,TRUE);
		delete [] short_name;
		delete [] file_name;
		file_name = result;
	};
   if (speak_postfix != NULL) delete [] speak_postfix;
   return(file_name);
};

void record_no_clipboard_change() {
#if TT_DEBUG_ON
	if (tt_debug_mode == 110100 || tt_debug_mode == 160299) {
		log("No change in clipboard");
	};
#endif
	log_put((byte) NO_CLIPBOARD_CHANGE);
};

void record_clipboard(string s, long size) {
	if (s == NULL) {
#if TT_DEBUG_ON
	   if (tt_debug_mode == 202) {
		   log("Clipboard is empty.");
	   };
#endif
		log_put((byte) CLIPBOARD_EMPTY);
	} else {
		log_put((byte) CLIPBOARD_STRING_FOLLOWS);
#if TT_DEBUG_ON
		//if (size > max_unsigned_short) { // new on 110601 -- removed on 111102
		//	// should really fixed write_string sometime
		//	tt_error_file() << "Didn't record clipboard string since its size is over 64K: " << size << endl;
		//	log_string(0,NULL); // new on 110601
		//	return;
		//};
		if (tt_debug_mode == 110100 || tt_debug_mode == 160299 || tt_debug_mode == 40803) {
			tt_error_file() << "Clipboard contains \"" << s << "\" on frame " << tt_frame_number << " at " << log_out.tellp() << endl;
		};
#endif
		log_string(s,size);
	};
};

void record_clipboard_files(unsigned int file_count) { // new on 010503
	// files names themselves to follow
	log_put((byte) CLIPBOARD_FILE_NAMES_FOLLOW);
	log_write((bytes) &file_count,sizeof(file_count));
};

Sprite *replay_clipboard_file_names() {
	unsigned int file_count;
	log_in->read((string) &file_count,sizeof(file_count));
	char file_name[MAX_PATH];
	char hashed_file_name[MAX_PATH]; // new on 140204 -- this is the file name that should be in the tt_log_in_archive
	Cubby *box = NULL;
	if (file_count > 1) {
		box = new Cubby();
      box->set_number_of_holes(file_count);
		box->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL); // new on 280204 -- since this was called when recording (since 200903)
	};
	boolean aborted = FALSE; // new on 200105
	for (unsigned int i = 0; i < file_count; i++) {
		log_in->read_string(file_name);
		Sprite *sprite;
		if (tt_log_version_number >= 34) { // uses the hash name in the archive
			log_in->read_string(hashed_file_name);
			string destination_directory; // new on 180204 to use tt_extracted_file_temp_directory or tt_private_media_directory
			// dealing with pictures is tricky here since need to load the image to convert to png to get this to work - do another time 180204
			if (//has_picture_extension(hashed_file_name) || 
				 has_sound_extension(hashed_file_name)) {
				destination_directory = tt_private_media_directory;
			} else {
				destination_directory = tt_extracted_file_temp_directory;
			};
			string real_file_name = extract_file_from_archive(hashed_file_name,tt_log_in_archive,destination_directory,TRUE);
			sprite = sprite_from_file_name(real_file_name,aborted,file_name);
			delete [] real_file_name;
		} else {
			sprite = sprite_from_file_name(file_name,aborted);
		};
		if (file_count == 1) return(sprite);
		if (sprite != NULL) {
			box->initialize_hole(i,sprite);
		};
		wide_string wide_file_name = copy_wide_string(file_name);
		wide_string wide_short_file_name = short_file_name(wide_file_name);
		box->set_label(file_count-1,copy_narrow_string(wide_short_file_name));
		delete [] wide_file_name;
		delete [] wide_short_file_name;
	};
	return(box);
};

void ignore_replay_clipboard_file_names() {
	unsigned int file_count;
	log_in->read((string) &file_count,sizeof(file_count));
	char file_name[MAX_PATH];
	for (unsigned int i = 0; i < file_count; i++) {
		log_in->read_string(file_name);
		if (tt_log_version_number >= 34) { // uses the hash name in the archive
			log_in->read_string(file_name);
		};
	};
};

ClipboardTokens replay_clipboard_token() {
	// following is like a band-aid that doesn't fix the real problem which can occur later downstream even worse
	//int next_byte = log_in->peek(); // new on 310803 since sometimes the order of dealing with the clipboard sensor is hard to get right
	//if (next_byte < NO_CLIPBOARD_CHANGE || next_byte > CLIPBOARD_FILE_NAMES_FOLLOW) {
	//	return(NO_CLIPBOARD_CHANGE);
	//};
	if (log_in == NULL) { // new on 050504 for robustness  
#if TT_DEBUG_ON
		debug_this();
#endif
		return(NO_CLIPBOARD_CHANGE); // throw(END_OF_LOG_FILE);
	};
	ClipboardTokens token = (ClipboardTokens) log_in->get();
#if TT_DEBUG_ON
	if (tt_debug_mode == 101 || tt_debug_mode == 110100 || tt_debug_mode == 160299 || tt_debug_mode == 40803) {
		tt_error_file() << "Clipboard token is " << (int) token << " on frame " << tt_frame_number << " TellG = " << log_in->tellg() << endl;
	};
#endif
   return(token);
};

string replay_clipboard_string() {
	string s = log_in->read_string();
#if TT_DEBUG_ON
	if (tt_debug_mode == 101 || tt_debug_mode == 110100 || tt_debug_mode == 160299 || tt_debug_mode == 40803) {
		if (s == NULL) {
			tt_error_file() << "Clipboard contains NULL on frame " <<  tt_frame_number << endl;
		} else {
			tt_error_file() << "Clipboard contains \"" << s << "\" on frame " << tt_frame_number << endl;
		};
	};
#endif
	return(s);
};

void record_ini_entry(string entry) {
	log_string(entry);
};

void record_string(string s) { // currently the same as the above
	log_string(s);
};

void record_ini_int(unsigned int entry) {
	log_write((bytes) &entry, sizeof(entry));
};

string replay_ini_entry() {
#if TT_DEBUG_ON
	if (tt_debug_mode == 110100 || tt_debug_mode == 160299) {
		int location = (int) log_in->tellg();
		tt_error_file() << "tellg() = " << location << endl;
	};
#endif
	if (log_in == NULL || log_in->empty()) {
		// added log_in == NULL test since can happen with pasting while paused during time travel
		return(NULL); // new on 070201 -- warn??
	};
	return(log_in->read_string());
};

unsigned int replay_ini_int() {
#if TT_DEBUG_ON
	if (tt_debug_mode == 110100 || tt_debug_mode == 160299) {
		int location = log_in->tellg();
		tt_error_file() << "tellg() = " << location << endl;
	};
#endif
	if (log_in->empty()) {
		return(0); // new on 070201 -- warn??
	};
	unsigned int answer;
	log_in->read((string) &answer, sizeof(answer));
	return(answer);
};

enum SpecialEvents {SPEAKING_ENDED=0, TIME_SPENT_PAUSED, FILE_DOUBLE_CLICKED, FILES_DRAGGED};

void record_special_events() {
	byte event_types = (tt_paragraph_id_just_finished >= 0) |
		                ((tt_time_spent_paused > 0)<<TIME_SPENT_PAUSED) |
							 ((tt_file_name_clipboard_message != NULL)<<FILE_DOUBLE_CLICKED) |
							 ((tt_drop_files_count > 0)<<FILES_DRAGGED);
	// new on 130204
	log_put(event_types); // room for 7 other events here as well -- since different special events can happen simultaneously
	if (tt_paragraph_id_just_finished >= 0) {
		log_write((bytes) &tt_paragraph_id_just_finished, sizeof(tt_paragraph_id_just_finished));
#if TT_DEBUG_ON
		if (tt_debug_mode == 110100 || tt_debug_mode == 160299 || tt_debug_mode == 140103) {
			tt_error_file() << "Special event tt_paragraph_id_just_finished " << tt_paragraph_id_just_finished 
								 << " on frame " << tt_frame_number << endl;
		};
#endif
	};
	if (tt_time_spent_paused > 0) { // new on 130204
		log_write((bytes) &tt_time_spent_paused, sizeof(tt_time_spent_paused));
#if TT_DEBUG_ON
		if (tt_debug_mode == 110100 || tt_debug_mode == 160299 || tt_debug_mode == 140103) {
			tt_error_file() << "Special event tt_time_spent_paused " 
								 << tt_time_spent_paused << " on frame " << tt_frame_number << endl;
		};
#endif
	};
	if (tt_file_name_clipboard_message != NULL) {
		// new on 140204 so that file is copied and 
		// renamed to its hash name so DMO won't depend upon existence or constancy of file
		string new_name = log_file_name(tt_file_name_clipboard_message);
		log_string(new_name);
#if TT_DEBUG_ON
		if (tt_debug_mode == 110100 || tt_debug_mode == 160299 || tt_debug_mode == 140103 || tt_debug_mode == 121205) {
			tt_error_file() << "Special event tt_file_name_clipboard_message  " 
								 << new_name << " derived from " << tt_file_name_clipboard_message 
								 << " on frame " << tt_frame_number << endl; // << (int) log_out.pcount() << endl;
		};
#endif
		if (new_name != NULL) delete [] new_name; // conditional new on 200204 for robustness
		delete [] tt_file_name_clipboard_message;
		tt_file_name_clipboard_message = NULL;
	};
	if (tt_drop_files_count > 0) {
		log_write((bytes) &tt_drop_files_count, sizeof(tt_drop_files_count));
		log_write((bytes) &tt_drop_files_x, sizeof(tt_drop_files_x));
		log_write((bytes) &tt_drop_files_y, sizeof(tt_drop_files_y));
		for (int i = 0; i < tt_drop_files_count; i++) {
			log_string(tt_drop_file_names[i]);
			log_string(tt_drop_file_hash_names[i]);
		};
		::PostMessage(tt_main_window->get_handle(),WM_USER+DROP_FILES_MESSAGE_ID,0,0);
	};
};

void replay_special_events() {
	unsigned char events = log_in->get();
	if (events == 0) return; // this is so common may as well optimize for this case here - 200204
	if (events > 15) { // new on 140204
		tt_err_file_important = TRUE;
		if (tt_log_version_number > 35) { 
			// conditional new on 200204 so you get a different error message and don't throw out of here
			tt_error_file() << "This demo or time travel archive was created with a newer version of ToonTalk. This version may not be able to recreate special events correctly. Or else there is a ToonTalk bug." << endl;
		} else {
			tt_error_file() << "Either the demo file is corrupted or ToonTalk has a bug. Please report this and send the time_travel.dmo file." 
								 << endl;
			if (tt_time_travel != TIME_TRAVEL_OFF) {
#if TT_DEBUG_ON
				debug_this();
#endif
				throw(UNRECOGNIZED_SPECIAL_EVENT_TOKEN);
			};
		};
	};
	if (events&1) { // speech finished event
		log_in->read((string) &tt_paragraph_id_just_finished, sizeof(tt_paragraph_id_just_finished));
#if TT_DEBUG_ON
		if (tt_debug_mode == 110100 || tt_debug_mode == 160299 || tt_debug_mode == 140103) {
			tt_error_file() << "Special event tt_paragraph_id_just_finished " << tt_paragraph_id_just_finished << " on frame " 
								 << tt_frame_number << endl;
		};
#endif
	};
	if (events&(1<<TIME_SPENT_PAUSED)) { // spent time paused
		log_in->read((string) &tt_time_spent_paused, sizeof(tt_time_spent_paused));
#if TT_DEBUG_ON
		if (tt_debug_mode == 110100 || tt_debug_mode == 160299 || tt_debug_mode == 140103) {
			tt_error_file() << "Special event tt_time_spent_paused " << tt_time_spent_paused << " on frame " << tt_frame_number << endl;
		};
#endif
	};
	if (events&(1<<FILE_DOUBLE_CLICKED)) {
		tt_file_name_clipboard_message = log_in->read_string();
#if TT_DEBUG_ON
		if (tt_debug_mode == 110100 || tt_debug_mode == 160299 || tt_debug_mode == 140103) {
			tt_error_file() << "Special event tt_file_name_clipboard_message ";
			if (tt_file_name_clipboard_message == NULL) {
				tt_error_file() << "NULL";
			} else {
				tt_error_file() << tt_file_name_clipboard_message;
			};
			tt_error_file() << " on frame " << tt_frame_number << " TellG is " << (int) log_in->tellg() << endl;
		};
#endif
		if (tt_file_name_clipboard_message != NULL) {
//			WritePrivateProfileString("User","TTFileLastDoubleClicked",tt_file_name_clipboard_message,tt_ini_file);
			string destination_directory; // new on 180204 to use tt_extracted_file_temp_directory or tt_private_media_directory
			if (//has_picture_extension(tt_file_name_clipboard_message) || 
				 has_sound_extension(tt_file_name_clipboard_message)) {
				destination_directory = tt_private_media_directory;
			} else {
				destination_directory = tt_extracted_file_temp_directory;
			};
			string real_file_name = extract_file_from_archive(tt_file_name_clipboard_message,tt_log_in_archive,
																			  destination_directory);
			delete [] tt_file_name_clipboard_message;
			tt_file_name_clipboard_message = real_file_name;
			::PostMessage(tt_main_window->get_handle(),WM_USER+FILE_FOR_CLIPBOARD_MESSAGE_ID,0,0);
		} else { // else warn -- new on 220304
			tt_error_file() << "Demo or time travel recorded a file was double clicked upon but somehow the file name wasn't recorded. Demo may not replay correctly." << endl;
		};
	};
	if (events&(1<<FILES_DRAGGED)) {
		log_in->read((string) &tt_drop_files_count, sizeof(tt_drop_files_count));
		if (tt_drop_files_count <= 0 || tt_drop_files_count > max_unsigned_short) { 
			// just to be safe no negative or greater than 64K collection of files dropped
#if TT_DEBUG_ON
			debug_this();
			tt_error_file() << "TellG is " << log_in->tellg() << endl;
#endif
			throw(LOG_FILE_CORRUPTED);
		};
		log_in->read((string) &tt_drop_files_x, sizeof(tt_drop_files_x));
		log_in->read((string) &tt_drop_files_y, sizeof(tt_drop_files_y));
		tt_drop_file_names = new string[tt_drop_files_count];
		tt_drop_file_hash_names = new string[tt_drop_files_count];
		for (int i = 0; i < tt_drop_files_count; i++) {
			tt_drop_file_names[i] = log_in->read_string();
			tt_drop_file_hash_names[i] = log_in->read_string();
		};
		::PostMessage(tt_main_window->get_handle(),WM_USER+DROP_FILES_MESSAGE_ID,0,0);
	};
};

//string last_log_file_name_in = NULL; // new on 050506 to avoid repeatedly doing this while it is on the clipboard
//string last_log_file_name_out = NULL;

string log_file_name(string file_name, string extension_including_dot_if_missing) { 
	// new on 140204 -- returns short hash file name and archives it
	//if (file_name == last_log_file_name_in) {
	//	return(last_log_file_name_out);
	//};
	//last_log_file_name_in = file_name;
	int extension_start = extension_start_position(file_name);
	string extension;
	if (extension_start > 0) {
		extension = file_name+extension_start-1; // -1 to include the dot
	} else {
		extension = extension_including_dot_if_missing; // was "" prior to 181204
	};
	byte hash[hash_byte_length];
	string destination_directory; // new on 180204 to use tt_extracted_file_temp_directory or tt_private_media_directory
	//if (has_picture_extension(file_name)) {
	//	destination_directory = tt_private_media_directory;
	//	extension = ".png";
	//} else 
	if (has_sound_extension(file_name)) {
		destination_directory = tt_private_media_directory;
	} else {
		destination_directory = tt_extracted_file_temp_directory;
	};
	string new_file_name = hash_and_copy_file(file_name,extension,hash,FALSE,destination_directory,strlen(destination_directory));
	if (new_file_name != NULL) { // conditional new on 200204 for robustness (Gordon had a crash)
	//	int basic_name_start = max(0,start_of_basic_file_name(new_file_name));
	//	log_string(new_file_name+basic_name_start);
		string zip_destination = NULL;
		if (destination_directory == tt_private_media_directory) {
			// new on 020504 (just sounds for now) -- puts them in the Media folder in the zip archive
			int file_name_length = strlen(new_file_name);
			boolean found_slash = FALSE;
			for (int i = file_name_length-1; i > 0; i--) {
				if (new_file_name[i] == '\\') {
					if (found_slash) { // this one is the second one so found the zip destination (e.g. Media\xxx.wav)
						zip_destination = new_file_name+i+1;
						break;
					} else {
						found_slash = TRUE;
					};
				};
			};
		};
		if (zip_destination != NULL) {
			save_media_files_in_archive(tt_log_out_archive,zip_destination,TRUE); 
			// new on 050504 -- third arg works but is misnamed?
		} else if (tt_log_out_archive != NULL) { 
			// conditional new on 080804 since if no logging going on this can signal an error
			zip_file(tt_log_out_archive,new_file_name,zip_destination);
		};
		string name = short_file_name(new_file_name,TRUE);
		delete [] new_file_name;
//		last_log_file_name_out = name;
		return(name);
	} else {
		return(NULL);
	};
};

//string current_notebook_file_name = NULL;

// commented out the following on 070703 since 120603 just dump NOTHING instead
//void record_notebook(Notebook *notebook) { // changed to use XML when appropriate on 140203
//	if (!tt_log_out_file_name_generated) { // new on 030300 so named logs have notebook embedded
//		//boolean saved_tt_dumping_to_clipboard = tt_dumping_to_clipboard;
//		//tt_dumping_to_clipboard = !tt_logs_contain_virgin_notebook; // sometimes a white lie to save the entire notebook
//		DumpReason saved_tt_dump_reason = tt_dump_reason;
//		if (!tt_logs_contain_virgin_notebook) { // could this become another DumpReason instead?? - asked 111202
//			tt_dump_reason = DUMPING_TO_CLIPBOARD;
//		};
////		log_out.put((char) PROGRAM_PAD);
////		notebook->dump_contents(log_out);
//		if (tt_dump_as_xml) {
//			// new on 140203
//			log_put('X'); // to know it is XML and not the old format
////#if TT_DEBUG_ON
////			int loc = log_out.tellp();
////			tt_error_file() << "debug this" << loc << endl;
////#endif
////			tt_recording_notebook_for_log = TRUE; // this fixes demos but causes sensors in the notebook to be dead for brand new users - come back to this !!! - 250503
//			notebook->xml_to_stream(log_out);
//			tt_recording_notebook_for_log = FALSE;
//		} else {
//			notebook->top_level_dump(log_out); 
// changed on 060300 so may or may not dump contents rather than refer to file names -- 
// changed on 270202 to be top_level so don't leave record for subsequent sharing
////		tt_dumping_to_clipboard = saved_tt_dumping_to_clipboard;
//			notebook->set_changed_since_last_save(FALSE); // since just saved
//			notebook->dump_geometry(log_out); // added 210100 -- moved into each branch on 130103 since shouldn't be there always
//		};
//		tt_dump_reason = saved_tt_dump_reason;
//		return;
//	} else if (current_notebook_file_name == NULL || notebook->return_changed_since_last_save()) {
//		if (current_notebook_file_name == NULL) {
//			current_notebook_file_name = append_strings(tt_user_directory,"nb00000.bok");
//		} else {
//			boolean name_is_new;
//			string new_file_name = next_file_name(current_notebook_file_name,name_is_new,FALSE);
//			if (name_is_new) {
//				delete [] current_notebook_file_name;
//				current_notebook_file_name = new_file_name;
//			};
//		};
//		output_file_stream stream(current_notebook_file_name,std::ios_base::out|std::ios_base::binary);
//		//boolean saved_tt_dumping_to_clipboard = tt_dumping_to_clipboard;
//		//tt_dumping_to_clipboard = TRUE; // a white lie but this saves the entire notebook
//		DumpReason saved_tt_dump_reason = tt_dump_reason;
//		tt_dump_reason = DUMPING_TO_CLIPBOARD;
//		notebook->dump_contents(stream);
//		stream.close();
////		tt_dumping_to_clipboard = saved_tt_dumping_to_clipboard;
//		tt_dump_reason = saved_tt_dump_reason;
//		notebook->set_changed_since_last_save(FALSE); // since just saved
//	};
//	// the following now (120603) happens regardless of the conditional above
//	log_put(FILE_NAME_MARKER);
//	log_string(current_notebook_file_name);
//	short int page_number = notebook->left_page_number();
//	log_write((bytes) &page_number,sizeof(page_number));
//	notebook->dump_geometry(log_out); // added 210100 -- moved into each branch on 130103 since shouldn't be there always
//};

//void release_current_notebook_file_name() { // new on 251000
//	if (current_notebook_file_name != NULL) delete [] current_notebook_file_name;
//};

Notebook *replay_notebook() {
	// any logs created after 120603 will just find NOTHING_MARKER and return NULL
	unsigned char first_character;
	//if (tt_log_in_archive != NULL) {
	//	first_character = log_in->get();
	//} else {
		first_character = log_in->peek(); // changed to unsigned on 300303 since interfered with == tests below
//	};
	if (first_character == 'X') { // so long as it doesn't conflict with the old possibilities - new on 140203
//		if (tt_log_in_archive == NULL) {
			log_in->get(); // gobble up marker
//		}; // otherwise didn't peek (into archive)
		xml_document *document = xml_from_stream(log_in);
		Notebook *notebook = new Notebook();
		xml_entity(document,notebook);
		document->Release();
		return(notebook);
	};
	SpriteType type = (SpriteType) first_character;
//	if (type == PROGRAM_PAD) {
//		return((Notebook *) load_item(log_in));
	if (first_character == ZIP_ARCHIVE_MARKER) { // new on 171203
		log_in->get(); // gobble up marker
		//if (tt_current_notebook_zip_archive != NULL) { // rewritten on 280204 to deal better when it is the same archive
		//	delete [] tt_current_notebook_zip_archive;
		//};
//		tt_current_notebook_zip_archive = log_in->read_string(); 
		// was a local variable prior to 160204 but need to know this if record is pressed in the past
		string new_notebook_zip_archive = log_in->read_string();
		if (new_notebook_zip_archive == NULL) {
			return(NULL); // warn??
		};
		if (tt_log_in_archive == NULL) { // new on 100403 for robustness
			tt_error_file() << "Can't extract " << new_notebook_zip_archive << "from the NULL zip archive." << endl;
			return(NULL);
		};
		if (tt_current_notebook_zip_archive != NULL && strcmp(new_notebook_zip_archive,tt_current_notebook_zip_archive) == 0) {
			// no change so no need to do the following
			return(NULL); // ok
		} else {
			if (tt_current_notebook_zip_archive != NULL) {
				delete [] tt_current_notebook_zip_archive;
			};
			tt_current_notebook_zip_archive = new_notebook_zip_archive;
		};
		string notebook_zip_archive = extract_file_from_archive(tt_current_notebook_zip_archive,tt_log_in_archive);
		if (notebook_zip_archive == NULL) {
#if TT_DEBUG_ON
			debug_this();
#endif
			tt_error_file() << "Troubles extracting " << tt_current_notebook_zip_archive << " from " << tt_log_in_archive << endl;
			return(NULL); 
		};
		if (extract_archive_to_directory(notebook_zip_archive,"*.*",tt_extracted_file_temp_directory)) {
//			tt_load_new_main_notebook_files = TRUE; // new on 050204 -- made obsolete on 100204
		} else {
#if TT_DEBUG_ON
			debug_this();
#endif
			tt_error_file() << "Troubles extracting files from " << notebook_zip_archive << endl;
		};
		return(NULL); // all is fine but this didn't create the notebook just arranged the files it'll need when loaded
	};
	if (first_character == FILE_NAME_MARKER) {
		int version_number;
		Notebook *notebook;
//		if (tt_log_in_archive == NULL) {
			log_in->get(); // gobble up marker
//		};
//		if (type == FILE_NAME_MARKER) { -- rationalized away on 303030
		string name = log_in->read_string();
		if (name == NULL) return(NULL); // warn??
		notebook = new Notebook(0,0,0,NULL);
		FileNameStatus name_status;
		string full_name = existing_file_name(name_status,name,"bok",tt_user_directory); // new on 130701 
		if (full_name != NULL) {
			FileInputStream stream;
			stream.open(full_name,std::ios_base::in|std::ios_base::binary);
			version_number = notebook->load_notebook(&stream);
			stream.close();
			delete [] full_name;
		} else {
			tt_error_file() << "Can't find the file named " << name << endl;
			return(NULL); // new on 200103 
		};
		//} else { // rationalized away on 300303
		//	notebook = new Notebook(0,0,0,NULL);
		//	version_number = notebook->load_notebook(log_in);
		//};
		short int page_number;
		log_in->read((string) &page_number,sizeof(page_number));
		notebook->go_to_page(page_number);
		// following is saved on the "outside" rather than "inside" of notebook so added manually here
		Text *text_comment = new Text();
		text_comment->set_text(S(IDS_BOOK_DESCRIPTION1));
		notebook->set_comment(text_comment);
		notebook->set_label_string(tt_user_name);
		notebook->load_geometry(log_in,version_number);
		notebook->update_textual_label_size_and_location();
#if TT_DEBUG_ON
		if ((tt_debug_mode == 110100 || tt_debug_mode == 160299) && name != NULL) {
			tt_error_file() << "Read a notebook defined by " << name << endl;
		};
#endif
		if (name != NULL) delete [] name;
		return(notebook);
	} else if (first_character == NOTHING_MARKER) {
//		if (tt_log_in_archive == NULL) {
			log_in->get(); // gobble up marker
//		};
		return(NULL);
	} else if (type == PROGRAM_PAD || type == NEW_SHARED_ITEM) { // second disjunct added on 090701 since compaction now generates this
		if (tt_log_in_archive != NULL) {
			log("Reading old format saved objects in an archive file.",FALSE,TRUE);
			return(NULL);
		};
		UnwindProtect<boolean> set(tt_logs_contain_virgin_notebook,TRUE); // if files are refered to use virgin version (in DLL)
		Notebook *notebook = (Notebook *) load_item(log_in); // new on 060300
		// changed the following on 130300 since even if the language DLL has an old notebook the geometry
		// was dumped by this log
		// if geometry is changed then it should be correlated with log version numbers
		notebook->load_geometry(log_in,latest_version_number); //notebook->return_version_number());
		return(notebook);
	} else {
	   if (tt_log_in_archive != NULL) {
			log("Problem loading saved notebook in an archive file.",FALSE,TRUE);
	   };
		return(NULL); // no need to warn since have peeked to see if notebook is here
	};
};

const int number_of_buttons_displayed = 6; // new on 041103

#if TT_ALPHA_FEATURE

// play and pause handled differently as of 041103
const int number_of_buttons = 7; // treating pause and play differently as of 041103
enum TimeTravelButton {TO_OLDEST_SEGMENT,TO_PREVIOUS_SEGMENT,PLAY_TIME_TRAVEL, // reordered on 100603 and again on 160603
							  TO_NEXT_SEGMENT,TO_NEWEST_SEGMENT,QUIT_TIME_TRAVEL,PAUSE_TIME_TRAVEL};
const string button_file_names[number_of_buttons] = {"back_n.gif","back_1.gif","play.gif","fd_1.gif","fd_n.gif","record.gif","pause.gif"};

#else

const int number_of_buttons = 6;
enum TimeTravelButton {TO_OLDEST_SEGMENT,TO_PREVIOUS_SEGMENT,PAUSE_OR_PLAY_TIME_TRAVEL, // reordered on 100603 and again on 160603
							  TO_NEXT_SEGMENT,TO_NEWEST_SEGMENT,QUIT_TIME_TRAVEL};
const string button_file_names[number_of_buttons] = {"back_n.gif","back_1.gif","play.gif","fd_1.gif","fd_n.gif","record.gif"};

#endif

// new on 230404 since ShowCursor doesn't work in full screen mode on Win 98/Me
const string pointing_cursor_file_name = "cursorpt.gif";
//const string waiting_cursor_file_name = "cursorwt.gif";
Sprite *pointing_cursor = NULL;
//Sprite *waiting_cursor = NULL;

Sprite **buttons = NULL;
Sprite *fresh_button = NULL; // used for calculating size
//Text **labels = NULL;
Sprite *time_label = NULL; // was Text prior to 110603
//Sprite *play_appearance = NULL; // obsolete as of 041103
//Sprite *pause_appearance = NULL;

boolean displaying_pause_button() { // abstracted on 310803 -- moved here on 031103
	// rewritten on 041103 so that both versions co-exist and move together (for autohiding) but only one is visible at a time
	if (buttons == NULL) {
		prepare_to_time_travel(); // new on 240204
	};
	return(buttons[PAUSE_TIME_TRAVEL]->visible());
};
//inline boolean displaying_pause_button() { // abstracted on 310803 -- moved here on 031103
//	return(buttons[PAUSE_OR_PLAY_TIME_TRAVEL] == pause_appearance);
//};

void use_play_appearance() { // abstracted on 200703 -- rewritten 041103
	if (buttons == NULL) {
		prepare_to_time_travel(); // new on 240204 
	};
	buttons[PLAY_TIME_TRAVEL]->set_visible(TRUE);
	buttons[PAUSE_TIME_TRAVEL]->set_visible(FALSE);
};

void use_pause_appearance() { // abstracted on 200703
	if (buttons == NULL) {
		prepare_to_time_travel(); // new on 240204 -- can happen (but maybe only buggy circumstances)
	};
	buttons[PLAY_TIME_TRAVEL]->set_visible(FALSE);
	buttons[PAUSE_TIME_TRAVEL]->set_visible(TRUE);
};

//void use_play_appearance() { // abstracted on 200703
//#if TT_ALPHA_FEATURE
//	restore_time_travel_buttons();
//#endif
//	buttons[PAUSE_OR_PLAY_TIME_TRAVEL] = play_appearance;
//};
//
//void use_pause_appearance() { // abstracted on 200703
//#if TT_ALPHA_FEATURE
//	restore_time_travel_buttons();
//#endif
//	buttons[PAUSE_OR_PLAY_TIME_TRAVEL] = pause_appearance;
//};

void time_travel(TimeTravelState state) {
	if (tt_time_travel == state) return; // no news
	TimeTravelState previous_state = tt_time_travel;
	tt_time_travel = state;
	if (tt_time_travel == TIME_TRAVEL_ON && log_in == NULL) { // new on 220903 - e.g. hit pause and then play while running/recording
		tt_time_travel = TIME_TRAVEL_RECORD;
	};
	boolean begin_recording = (tt_time_travel == TIME_TRAVEL_RECORD); // new on 180703
	if (begin_recording) {
#if TT_ALPHA_FEATURE
		tt_log_started_frame = tt_frame_number; // new on 221103 so don't jump due to first mouse deltas
#endif
		tt_time_travel = TIME_TRAVEL_OFF;
		set_rerecorded_segment(tt_current_log_segment);
	};
	tt_mouse_acquired = (tt_time_travel == TIME_TRAVEL_OFF);
	if (pointing_cursor == NULL) { // condition new 290404
		show_cursor(tt_time_travel != TIME_TRAVEL_OFF
#if TT_ALPHA_FEATURE
						&& !time_travel_buttons_hidden
#endif		
						,TRUE); // added TRUE as second arg on 130103 so this happens regardless
	};
	if (tt_time_travel == TIME_TRAVEL_ABOUT_TO_START) { // new on 200703 for replaying time travel archives as demos
		set_replay(REPLAY_REPRODUCE_TIMING_ON_NEXT_FRAME);
		prepare_to_time_travel(); // to initialize things
		use_pause_appearance(); // since it starts off running you need to be able to pause it
		// couild push_title here too (with different text)
		return; // following not appropriate if just starting 
	};
	if (tt_time_travel != TIME_TRAVEL_OFF) {
		if (previous_state == TIME_TRAVEL_OFF || previous_state == TIME_TRAVEL_ON) {
			// second disjunct new on 211003 to deal better with pause pressed during playback
			tt_main_window->push_title("Ready to Time Travel"); // english.rc - 200703
#if TT_TEXT_TO_SPEECH
			stop_speaking();
#endif
			stop_sound(TRUE);
			if (tt_log_out_archive != NULL) { // condition new on 211004 since shouldn't reset this if playing someone else's demo
				tt_exit_at_end_of_log = FALSE; // so can easily "jump" to the "present"
			};
			//if (tt_next_new_log_time_saved > 0) { 
			// condition added so only if time travel segments enabled does it do this - noticed on 290903 this is obsolete
			//	save_city_since_end_of_logging();
			//};
			recording_paused = tt_logging; // replaced the following on 140204 -- 
			// above was = TRUE prior to 190304 -- fixed a bug re-recording over the end of the time travel logs -- i.e. extending the archive
//			log_finalization(); // new on 190703 since need to finish the current (partial) segment
			if (previous_state == TIME_TRAVEL_ON) {  // new on 211003 to deal better with pause pressed during playback
				use_play_appearance();
			};
//			paused_frame_number = tt_frame_number; // new on 250404
//			close_log(); // this didn't turn off tt_replay and the like
		};
		if (previous_state == TIME_TRAVEL_PAUSED) {
//			tt_screen->remove(time_label);
#if TT_ENHANCED_DRAG_AND_DROP
			release_drag_drop(tt_main_window->get_handle()); // new on 301204 since no longer able to receive drags
#endif
		} else if (tt_time_travel == TIME_TRAVEL_JUST_PAUSED) {
#if TT_ENHANCED_DRAG_AND_DROP
			register_drag_drop(tt_main_window->get_handle()); 
			// new on 301204 -- didn't register earlier since drag and drop don't make sense while replaying but while pausing it does
#endif
			update_time_travel_buttons(TRUE);
			if (previous_state == TIME_TRAVEL_OFF) {
				time_log_started = tt_current_time; // new on 241003 so time display is correct
//			}; // noticed on 250404 that these have the same condition so were combined
//			if (previous_state == TIME_TRAVEL_OFF) { // only record pause time if was recording previously - also 140204
				// was != TIME_TRAVELLING_BEFORE_DIALOG) { // condition new on 140204 so if double Paused don't reset clock
				tt_real_time_pause_began = timeGetTime(); // new on 130204 to deal better with timing and pauses
			};
			begin_pointing_cursor();
			// following is new on 290404 and happens regardless of whether cursor is being emmulated
			city_coordinate play_button_center_x, play_button_center_y;
			buttons[PAUSE_TIME_TRAVEL]->center_location(play_button_center_x,play_button_center_y);
#if TT_DEBUG_ON
			if (tt_debug_mode == 300404 || tt_debug_mode == 290404) {
				tt_error_file() << "About to set_cursor_position to " << play_button_center_x << "," << play_button_center_y << endl;
			};
#endif
			tt_screen->set_cursor_position(play_button_center_x,play_button_center_y);
#if TT_DEBUG_ON
			if (tt_debug_mode == 300404 || tt_debug_mode == 290404) {
				tt_error_file() << "set_cursor_position done on frame " << tt_frame_number << endl;
			};
#endif
		};
		if (tt_time_travel == TIME_TRAVEL_ON) {
			if (tt_replay != REPLAY_REPRODUCE_TIMING_ON_NEXT_FRAME) {
				set_replay(REPLAY_REPRODUCE_TIMING);
				tt_real_time_pause_ended = timeGetTime(); // new on 200204
				tt_current_time_pause_ended = tt_current_time; // new on 220204
			};
#if TT_DEBUG_ON
			if (log_in == NULL) {
				debug_this();
			};
#endif
		};
	} else if (!about_to_quit()) { // condition new on 100703
		close_input_log(TRUE,FALSE); // new on 190703 
		tt_main_window->restore_title();
		update_clock();
		resume_repeating_sound();
		tt_screen->invalidate_cache();
		if (begin_recording && tt_current_log_segment != tt_youngest_log_segment) { 
			// condition new on 180603 so if continuing with the last segment not new so close and start a new one
			//char default_file_name[MAX_PATH];
			//strcpy(default_file_name,tt_user_directory);
			//strcat(default_file_name,"log00000.dmo");
			show_cursor(TRUE,TRUE); // new on 290904 since the following can take a long time and be mistaken for a crash
			wait_cursor();
			boolean running_anothers_archive = (tt_log_out_archive == NULL); // new on 200703
			if (running_anothers_archive) {
				string short_name = short_file_name(tt_log_in_archive,TRUE);
				string full_name = append_3_strings(tt_user_directory,"Demos\\",short_name);
				tt_log_out_archive = open_archive_to_zip(full_name);
				delete [] short_name;
				delete [] full_name;
			};
			tt_create_new_time_line = TRUE; // new on 010304
			tt_youngest_log_segment = tt_current_log_segment; 
			// new on 290204 but how did this ever work without this? -- this was confused - maybe oldest is what I meant?
			save_oldest_and_current_version_numbers(tt_current_log_segment); // new on 170603
			tt_log_in_archive = open_archive_to_unzip(tt_log_out_archive);
			// should really truncate the current segment if in the middle of playing a segment
			//close_log_and_open_next(); // begin writing the next segment
			//tt_next_new_log_time = tt_current_time+tt_time_between_new_logs;
			tt_next_new_log_time = tt_current_time; // rewrote the above on 150903 so recording starts at the right place in a cycle
#if TT_DEBUG_ON
			if (tt_debug_mode == 290903) {
				tt_error_file() << "Just set tt_next_new_log_time to tt_current_time to make it " << tt_next_new_log_time << endl;
			};
#endif
			restore_cursor(); // new on 290904
			show_cursor(FALSE,TRUE);
		} else if (recording_paused) { // conditional new on 140204 so don't start a new log if just pause/resume
			recording_paused = FALSE;
			if (tt_jump_to_youngest_log_segment) { // new on 290204
				tt_next_new_log_time = tt_current_time+tt_time_between_new_logs;
				tt_jump_to_youngest_log_segment = FALSE;
			};
		} else {
			start_time_travelling(); // new on 130204 otherwise starts a frame late and is slightly confused about millisecond_delta
		};
//		if (buttons != NULL) {
//			for (int i = 0; i < number_of_buttons; i++) {
//				tt_screen->remove(buttons[i]);
//				tt_screen->remove(labels[i]);
//			};
//			tt_screen->remove(time_label);
//		};
	} else if (recording_paused) { // new on 140204 - quitting so do it now
		log_finalization();
	};
};

// rewrote this on 160603 to use default locale's date format
void day_and_time_string(string temp_string, int max_string_length) {
	int i = 0;
	if (tt_log_out_archive == NULL) { // don't want the dates - just segment number good enough - reformat more??
		for (int j = 0; j < 10; j++) { // pad enough before -- was 15 prior to 220703
			temp_string[i++] = ' ';
		};
	};
	temp_string[i++] = '#';
	itoa(tt_current_log_segment,temp_string+i,10);
	i = strlen(temp_string);
	temp_string[i++] = ' ';
	temp_string[i++] = '=';
	temp_string[i++] = ' ';
	if (tt_log_out_archive == NULL) {
		// used to calculate relative time assuming all segments are tt_time_between_new_logs millseconds long 
		// (should almost always be true - could reset and then add to log - also interrupted segments may be shorter)
//		int seconds = ((tt_current_log_segment-1)*tt_time_between_new_logs)/1000;
//		seconds += (tt_current_time-time_log_started)/1000; // prior to 271003 to only show multiples of tt_time_between_new_logs/1000
		// Why isn't tt_current_time/1000 right? asked 021203 -- it is OK if initial_time is taken into account - rewritten 130204
		int seconds = (tt_current_time-tt_current_time_at_beginning_of_first_segment)/1000;
		int minutes = seconds/60;
		seconds = seconds%60;
		int hours = minutes/60;
		minutes = minutes%60;
		if (hours < 10) {
			temp_string[i++] = '0'; // so it shows 05 not 5
		};
		itoa(hours,temp_string+i,10);
		i = strlen(temp_string);
		temp_string[i++] = ':';
		if (minutes < 10) {
			temp_string[i++] = '0'; // so it shows 05 not 5
		};
		itoa(minutes,temp_string+i,10);
		i = strlen(temp_string);
		temp_string[i++] = ':';
		if (seconds < 10) {
			temp_string[i++] = '0'; // so it shows 05 not 5
		};
		itoa(seconds,temp_string+i,10);
		i = strlen(temp_string);
		for (int j = 0; j < 10; j++) { // pad enough after
			temp_string[i++] = ' ';
		};
		temp_string[i] = '\0'; // terminate it
		return; // don't show original recording time if replaying someone else's log
	};
	time_t log_time = date_and_time_log_created+(tt_current_time-time_log_started)/1000; 
	// prior to 140103 was +(tt_current_time-time_log_started)/1000; // restored 110603
	TIME_ZONE_INFORMATION time_zone;
	GetTimeZoneInformation(&time_zone);
	FILETIME file_time; 
	// following copied from Microsoft documentation:
   LONGLONG ll = Int32x32To64(log_time, 10000000) + 116444736000000000;
   file_time.dwLowDateTime = (DWORD) ll;
   file_time.dwHighDateTime = (DWORD) (ll >>32); // cast to avoid warning
	SYSTEMTIME universal_time, local_time;
	if (FileTimeToSystemTime(&file_time,&universal_time)) {
		if (SystemTimeToTzSpecificLocalTime(&time_zone,&universal_time,&local_time)) {
			int date_length = GetDateFormat(LOCALE_USER_DEFAULT,DATE_LONGDATE,&local_time,NULL,temp_string+i,max_string_length-i);
			i += date_length-1;
			temp_string[i++] = ' ';
			int time_length = GetTimeFormat(LOCALE_USER_DEFAULT,0,&local_time,NULL,temp_string+i,max_string_length-i);
		} else { 
			// maybe on Windows 98 or something else that doesn't support SystemTimeToTzSpecificLocalTime - new on 240603
			int date_length = GetDateFormat(LOCALE_USER_DEFAULT,DATE_LONGDATE,&universal_time,NULL,temp_string+i,max_string_length-i);
			i += date_length-1;
			temp_string[i++] = ' ';
			int time_length = GetTimeFormat(LOCALE_USER_DEFAULT,0,&universal_time,NULL,temp_string+i,max_string_length-i);
			strcat(temp_string," GMT");
		};
	} else {
		tt_error_file() << "FileTimeToSystemTime failed with error code = " << GetLastError() << endl;
	};
};

//void day_and_time_string(string temp_string) {
//	int i = 0;
//	temp_string[i++] = '#';
//	itoa(current_log_segment_number,temp_string+i,10);
//	i = strlen(temp_string);
//	temp_string[i++] = ' ';
//	temp_string[i++] = '=';
//	temp_string[i++] = ' ';
//	time_t log_time = date_and_time_log_created+(tt_current_time-time_log_started)/1000; 
// prior to 140103 was +(tt_current_time-time_log_started)/1000; // restored 110603
//	struct tm *time = gmtime(&log_time);
//	itoa(time->tm_year+1900,temp_string+i,10);
//	i = strlen(temp_string);
//	temp_string[i++] = ' ';
//	if (time->tm_mon+1 < 10) {
//		temp_string[i++] = '0';
//	};
//	itoa(time->tm_mon+1,temp_string+i,10);
//	i = strlen(temp_string);
//	temp_string[i++] = ' ';
//	if (time->tm_mday < 10) {
//		temp_string[i++] = '0';
//	};
//	itoa(time->tm_mday,temp_string+i,10);
//	i = strlen(temp_string);
//	temp_string[i++] = ' ';
//	if (time->tm_hour < 10) {
//		temp_string[i++] = '0';
//	};
//	itoa(time->tm_hour,temp_string+i,10);
//	i = strlen(temp_string);
//	temp_string[i++] = ' ';
//	if (time->tm_min < 10) {
//		temp_string[i++] = '0';
//	};
//	itoa(time->tm_min,temp_string+i,10);
//	i = strlen(temp_string);
//	temp_string[i++] = ' ';
//	if (time->tm_sec < 10) {
//		temp_string[i++] = '0';
//	};
//	itoa(time->tm_sec,temp_string+i,10);
//	i = strlen(temp_string);
//	temp_string[i] = '\0';
//};

boolean prepare_to_time_travel() {
	if (tt_titles_ended_on_frame > tt_frame_number && !tt_titles_just_ended) return(FALSE); // too hard to get time travel working while in titles
	if (tt_time_between_new_logs <= 0) return(FALSE); // new on 020300 since time travel without segments is kind of stupid
//	tt_real_time_pause_began = timeGetTime(); // new on 130204 to deal better with timing -- too soon since one more cycle to go - 130204
//	string version_count_file_name = append_strings(tt_user_directory,"log00000.dmo.n");
	oldest_and_yongest_versions(); // tt_oldest_log_segment,tt_youngest_log_segment);
//	delete [] version_count_file_name;
	if (tt_oldest_log_segment < 0) return(FALSE); // there is no record to time travel
	if (tt_youngest_log_segment < tt_current_log_segment && !tt_jump_to_youngest_log_segment) { // new on 170603
		// second condition added 290204
		tt_youngest_log_segment = tt_current_log_segment;
	};
	// following is new on 100603 - note the new order
	if (buttons == NULL) {
		buttons = new SpritePointer[number_of_buttons];
//		labels = new TextPointer[number_of_buttons];
//		SpriteCode code;
//		int text_color;
//		string text_string;
//		char temp_string[32];
		for (int i = 0; i < number_of_buttons; i++) {
//			buttons[i] = new Picture(0,0,min_long,0,BIKEPUMP_BUTTON_SPRITE);
			boolean ok;
			buttons[i] = new UserPicture(button_file_names[i],ok);
			buttons[i]->set_show_all(FALSE);
//			buttons[i]->set_adjust_height_if_camera_above(FALSE); // made obsolete on 230804
			buttons[i]->set_selectable(FALSE);
#if !TT_ALPHA_FEATURE
			// pause button is just another button now
			if (i == PAUSE_OR_PLAY_TIME_TRAVEL) { // new on 160603
				boolean ok;
				pause_appearance = new UserPicture("pause.gif",ok);
				pause_appearance->set_show_all(FALSE);
//				pause_appearance->set_adjust_height_if_camera_above(FALSE); // made obsolete on 230804
				play_appearance = buttons[i];
			};
#endif
//			city_coordinate width = buttons[i]->current_width();
//			city_coordinate height = buttons[0]->current_height(); // all have same height
//			string text_label;
//			if (i == REPLAY_SEGMENT) {
//				text_label = S(IDS_TIME_TRAVEL_RUN_BUTTON_LABEL,"GO");
//			} else {
//				text_label = button_strings[i];
//			};
//			labels[i] = variable_width_text_pad(text_label,FALSE,tt_white);
//			labels[i]->set_size((4*width)/5,(4*height)/5); 
////			labels[i]->set_adjust_height_if_camera_above(FALSE); // commented out on 140103
//			labels[i]->set_priority(min_long);
//			labels[i]->set_selectable(FALSE);
////			buttons[i]->add_follower(labels[i]);
		};
		fresh_button = buttons[0]->copy();
		if (time_label == NULL) {
//			time_label = variable_width_text_pad("",FALSE,tt_black); // is variable width a good idea? asked 110603
			boolean ok;
			time_label = new UserPicture("time.gif",ok);
			time_label->set_show_all(FALSE);
//			time_label->set_priority(min_long);
//			time_label->set_adjust_height_if_camera_above(FALSE); // commented out on 140103 - restored 110603 // made obsolete on 230804
		};
	} else {
		use_play_appearance(); // new on 170603
	};
	if (date_and_time_log_created == 0) { // haven't read any logs so current time
		date_and_time_log_created = seconds_since_1970()-tt_current_time/1000;
	};
	if (
#if !TT_DEBUG_ON
		 tt_exclusive_screen && // so while debugging can emulate the cursor in window mode
#endif
		 tt_emulate_mouse_for_full_screen_time_travel) {
		// condition new on 290404 since no need to emulate the mouse if not in full-screen mode
//		pointing_cursor = new Picture(0,0,0,0,HAND_SPRITE);
//		pointing_cursor->set_size_percent(20);
		boolean ok;
		pointing_cursor = new UserPicture(pointing_cursor_file_name,ok); // new on 290404
		if (!ok) { // new on 300404 for error recovery
			tt_error_file() << "Unable to load the cursorpt.gif file to eumulate the mouse cursor in full-screen mode." << endl;
			pointing_cursor->destroy();
			pointing_cursor = NULL;
		} else {
			pointing_cursor->set_show_all(FALSE);
//			pointing_cursor->set_adjust_height_if_camera_above(FALSE); // made obsolete on 230804
			pointing_cursor->set_selectable(FALSE);
			pointing_cursor->set_priority(max_long);
			pointing_cursor->update_display();
			begin_pointing_cursor(); // new on 270205 since otherwise crashes if cursor displayed "too early"
#if TT_DEBUG_ON
			if (tt_debug_mode == 300404 || tt_debug_mode == 290404) {
				tt_error_file() << "Created " << pointing_cursor_file_name << " on frame " << tt_frame_number << endl;
				if (pointing_cursor->pointer_to_current_image() != NULL) {
					tt_error_file() << "Full file name is " << pointing_cursor->pointer_to_current_image()->return_full_file_name() << endl;
				};
			};
#endif
		};
	};
	update_time_travel_buttons(TRUE);
//	tt_screen->add(pointing_cursor);
	//waiting_cursor = new UserPicture(waiting_cursor_file_name,ok);
	//waiting_cursor->set_show_all(FALSE);
	//waiting_cursor->set_adjust_height_if_camera_above(FALSE);
	//waiting_cursor->set_selectable(FALSE);
//	add_time_travel_buttons();
	return(TRUE);
};

#if TT_ALPHA_FEATURE

const millisecond auto_hide_duration = 2000;
const millisecond auto_return_duration = 500;
static city_coordinate original_button_lly = 0;
static city_coordinate original_time_label_lly = 0;

void hide_time_travel_buttons() {
	//if (time_travel_buttons_hidden && !buttons[0]->animation_in_progress() && buttons[0]->visible()) { // new on 061103
	//	// if flying the helicopter then what was off screen may no longer be so
	//	for (int i = 0; i < number_of_buttons; i++) {
	//		buttons[i]->set_visible(FALSE);
	//	};
	//	time_label->set_visible(FALSE);
	//};
	if (time_travel_buttons_hidden || buttons[0]->animation_in_progress()) return; // second disjunct added 041103
	time_travel_buttons_hidden = TRUE;
	city_coordinate delta_y = -buttons[0]->current_height()*2; // was ideal_screen_height-time_label->current_lly(); // made negative on 051103
//	city_coordinate delta_y = buttons[0]->current_height()*2; // was ideal_screen_height-time_label->current_lly();
	//if (original_button_lly == 0) { // commented out on 061103 since update_time_travel_buttons does this (and it adjusts for city scale)
	//	original_button_lly = buttons[0]->current_lly();
	//};
	//if (original_time_label_lly == 0) {
	//	original_time_label_lly = time_label->current_lly();
	//};
	for (int i = 0; i < number_of_buttons; i++) {
		buttons[i]->animate_delta_location(0,delta_y,auto_hide_duration);
	};
	//if (displaying_pause_button()) {
	//	play_appearance->move_to(pause_appearance->current_llx(),pause_appearance->current_lly());
	//	play_appearance->animate_delta_location(0,delta_y,auto_hide_duration);
	//} else {
	//	pause_appearance->move_to(play_appearance->current_llx(),play_appearance->current_lly());
	//	pause_appearance->animate_delta_location(0,delta_y,auto_hide_duration);
	//};
	time_label->animate_delta_location(0,delta_y,auto_hide_duration);
	{
		UnwindProtect<boolean> set(tt_mouse_acquired,TRUE); // so it hides the cursor - new on 041103
//		tt_screen->remove(pointing_cursor); // new on 240404
		show_cursor(FALSE,TRUE); // added 031103
	};
};

void restore_time_travel_buttons() {
	if (pointing_cursor == NULL) { // condition new on 290404
		show_cursor(TRUE,TRUE); // added 031103 - moved 041103 so this happens regardless of the following
	};
	begin_pointing_cursor(); // new on 290404
	if (!time_travel_buttons_hidden && !buttons[0]->animation_in_progress() && buttons[0]->visible()) return;
	time_travel_buttons_hidden = FALSE;
	for (int i = 0; i < number_of_buttons; i++) {
		buttons[i]->stop_animations();
		buttons[i]->animate_location(buttons[i]->current_llx(),original_button_lly,auto_return_duration);
//		buttons[i]->set_visible(TRUE); // new on 061103
	};
	//if (displaying_pause_button()) {
	//	play_appearance->stop_animations();
	//	play_appearance->animate_location(play_appearance->current_llx(),original_button_lly,auto_return_duration);
	//} else {
	//	pause_appearance->stop_animations();
	//	pause_appearance->animate_location(pause_appearance->current_llx(),original_button_lly,auto_return_duration);
	//};
	time_label->stop_animations();
	time_label->animate_location(time_label->current_llx(),original_time_label_lly,auto_return_duration);
//	time_label->set_visible(TRUE); // new on 061103
};

void repair_time_travel_button_animations() { // new on 041103
	if (buttons == NULL) return; // too early - new on 040204
	// only way I know this is needed where segments were added but current.cty not saved so that
	// subsequent log segments have an earlier time than earlier ones
	for (int i = 0; i < number_of_buttons; i++) {
		buttons[i]->finish_instantly();
	};
	time_label->finish_instantly();
};

#endif

void display_time_travel_buttons() {
#if TT_DEBUG_ON
	if (tt_debug_mode == 230803 || tt_debug_mode == 2308031) return; // since interferes with hashing or shows up as extra pictures
#endif
//	tt_main_window->set_focus(); // seems to be needed sometimes when running full screen - new on 051103
	static millisecond time_of_last_mouse_movement = timeGetTime()-2000; // -2000 new on 221103 so when clicking on a DMO file it hides quicker
	if (tt_time_travel == TIME_TRAVEL_ON) { // new on 041103 since this should only happen while in playback
		if (mouse_moved()) {
			time_of_last_mouse_movement = timeGetTime();
			if (time_travel_buttons_hidden) {
				restore_time_travel_buttons();
			};
		} else if ((millisecond) (timeGetTime()-time_of_last_mouse_movement) > tt_duration_to_trigger_auto_hide) {
			hide_time_travel_buttons();
		};
	} else {
		if (time_travel_buttons_hidden) {
			restore_time_travel_buttons();
		};
		time_of_last_mouse_movement = timeGetTime(); // so it is ignored unless in playback
	};
	for (int i = 0; i < number_of_buttons; i++) {
		buttons[i]->update_display(); // new on 041103
		if (buttons[i]->visible()) { // new on 041103
			buttons[i]->save_region(); // so the background under is displayed fine as this moves
		};
		buttons[i]->display();
		TTRegion button_region;
//		buttons[i]->full_region(button_region);
//		labels[i]->display(NO_FEEDBACK,FALSE,&button_region); // rewritten to use regions on 140103 -- commented out on 100603
	};
	if (tt_time_travel != TIME_TRAVEL_ON && tt_titles_ended_on_frame <= tt_frame_number) {
		time_label->update_display(); // new on 041103
		time_label->save_region();
		time_label->display();
		const int date_and_time_length = 64;
		char date_and_time[date_and_time_length];
		day_and_time_string(date_and_time,date_and_time_length);
		int length = strlen(date_and_time);
		city_coordinate llx = time_label->current_llx();
		city_coordinate lly = time_label->current_lly();
		city_coordinate width = time_label->current_width();
		city_coordinate height = time_label->current_height();
		city_coordinate character_width = width/length;
		city_coordinate character_height = (3*height)/4;
		city_coordinate x_offset = width/8; // half of the 1/4 left above
		tt_screen->text_out(date_and_time,length,llx+x_offset,lly+character_height,character_width,character_height,TRUE,TRUE);
	};
//	buttons_just_added = TRUE;
};

//long paused_frame_number = 0; // new on 250404 to use dirty regions for cursor movement
//long frame_number_pointing_cursor_began = 0;
//boolean pointing_cursor_moved_since_began = FALSE;

bytes screen_patch_buffer = NULL;

void begin_pointing_cursor() {
	//frame_number_pointing_cursor_began = tt_frame_number;
	//paused_frame_number = tt_frame_number;
	//pointing_cursor_moved_since_began = FALSE;
	if (screen_patch_buffer == NULL && pointing_cursor != NULL) {
		TTRegion region;
		pointing_cursor->true_region(region);
		// updated on 300404 to compute the buffer size more acuratately
		int size;
		TTImage *image = pointing_cursor->pointer_to_current_image();
		if (image != NULL) {
			size = image->width_without_scaling()*image->height_without_scaling()*tt_bytes_per_pixel;
		} else {
			size = 33*33*4; // good default but shouldn't happen -- changed from 32*32 to 33*33 on 270205 (but shouldn't be called anyway)
		};
		screen_patch_buffer = new byte[size]; // 2*tt_screen->city_region_pixel_size(region)]; 
#if TT_DEBUG_ON
		if (tt_debug_mode == 300404 || tt_debug_mode == 290404) {
			tt_error_file() << "Created a buffer for moving the emulated mouse cursor of size " << size << endl;
		};
#endif
	};
};

void release_screen_patch_buffer() { // new on 020704
	if (screen_patch_buffer != NULL) {
		delete [] screen_patch_buffer;
		screen_patch_buffer = NULL;
	};
};

TTRegion last_cursor_region; // static so undisplay uses same region

void undisplay_time_travel_cursor() { // new on 250404
#if TT_DEBUG_ON
	if (tt_debug_mode == 300404 || tt_debug_mode == 290404 || tt_debug_mode == 2304041) {
		tt_error_file() << "undisplay_time_travel_cursor on frame " << tt_frame_number 
							 << " with screen_patch_buffer != NULL " << (int) (screen_patch_buffer != NULL)
							 << " with pointing_cursor != NULL " << (int) (pointing_cursor != NULL) << endl;
	};
#endif
	if (screen_patch_buffer != NULL && pointing_cursor != NULL) {
//		TTRegion region;
//		pointing_cursor->true_region(region);
		tt_screen->restore_city_region(last_cursor_region,screen_patch_buffer);
	};
};

boolean display_time_travel_cursor() {
	if (pointing_cursor == NULL || time_travel_buttons_hidden) return(FALSE); // new on 290404
#if TT_DEBUG_ON
	if (tt_debug_mode == 300404 || tt_debug_mode == 290404) {
		tt_error_file() << "display_time_travel_cursor" << endl;
	};
#endif
	city_coordinate mouse_x, mouse_y;
	tt_main_window->current_mouse_location(mouse_x,mouse_y);
#if TT_DEBUG_ON
	if (tt_debug_mode == 2304041 || tt_debug_mode == 300404 || tt_debug_mode == 290404) {
		tt_error_file() << "display_time_travel_cursor's llx and ury are " << mouse_x << " and " << mouse_y 
							 << " on frame " << tt_frame_number << endl;
	};
#endif
	//city_coordinate old_llx, old_lly;
	//pointing_cursor->lower_left_corner(old_llx,old_lly);
//	pointing_cursor->move_to(mouse_x-(2*pointing_cursor->current_width())/5,mouse_y-(4*pointing_cursor->current_height())/5); 
	// upper left corner should follow the cursor
	pointing_cursor->move_to(mouse_x-pointing_cursor->current_width()/4,mouse_y-pointing_cursor->current_height()); // rewritten 290404
	// subtracting pointing_cursor->current_width()/4 as of 050305 to emulate the mouse more accurately
	//	return(!pointing_cursor->clean()); // rewritten on 240404 so that this is just on the screen and displayed accordingly -- didn't work well
	//city_coordinate delta_mouse_x,delta_mouse_y;
	//last_mouse_deltas(delta_mouse_x,delta_mouse_y);
	//pointing_cursor->move_by(delta_mouse_x,delta_mouse_y);
	//city_coordinate new_llx, new_lly;
	//pointing_cursor->lower_left_corner(new_llx,new_lly);
//	if (old_llx != new_llx || old_lly != new_lly) {
	if (pointing_cursor->clean()) {
		return(FALSE); // not displayed since didn't change position
	} else {
//		pointing_cursor->update_display();
//		pointing_cursor->save_region();
		//if (frame_number_pointing_cursor_began != paused_frame_number) {
		//	if (pointing_cursor_moved_since_began) { // don't restore until cursor has moved
		//		restore_regions(paused_frame_number); // remove cursor from last frame
		//	};
		//	paused_frame_number++;
		//};
		pointing_cursor->set_scale(tt_screen->current_scale()); // new on 260404 so this works when flying
		pointing_cursor->true_region(last_cursor_region);
		//TTRegion *screen_region = tt_screen->viewing_region();
		//screen_region->intersect_with(last_cursor_region,last_cursor_region); // commented out since redundant with the following
		tt_screen->save_city_region(last_cursor_region,screen_patch_buffer);
//		pointing_cursor->save_region_to_memory(screen_patch_buffer); // open coded the above on 290404 so can share last_cursor_region
//		if (pointing_cursor->save_region(paused_frame_number)) {
			pointing_cursor->display(NO_FEEDBACK,FALSE,&last_cursor_region); // updated 010305 so works while flying
//#if TT_DEBUG_ON
//			if (tt_debug_mode == 250404) {
//				tt_error_file() << "Displayed pointing cursor on frame " << tt_frame_number 
									//<< " and paused_frame_number is " << paused_frame_number << endl;
//			};
//#endif
//		};
		//if (!pointing_cursor_moved_since_began) {
		//	pointing_cursor_moved_since_began = TRUE;
		//	paused_frame_number++;
		//};
//		tt_screen->display_layer(pointing_cursor);
#if TT_DEBUG_ON
		if (tt_debug_mode == 2304041) {
			tt_error_file() << "display_time_travel_cursor dirty on frame " << tt_frame_number << endl;
		};
#endif
		return(TRUE);	
	};
};

void update_time_travel_buttons(boolean regardless) { // regardless new arg on 041103 -- ignored by non TT_ALPHA_FEATURE code
	//static boolean first_update = TRUE;
	//if (tt_time_travel == TIME_TRAVEL_ON && !first_update) {
	//	return;
	//};
	//first_update = FALSE;
	static city_scale last_screen_scale = 0; // new on 061103 to respond sensibly to flying up and down while buttons are animating
	if (regardless) {
		for (int i = 0; i < number_of_buttons; i++) {
			buttons[i]->stop_animations();
		};
		time_label->stop_animations();
		time_travel_buttons_hidden = FALSE; // new on 061103
	} else if (last_screen_scale != tt_screen->current_scale()) { // fly up or down (or maybe stood up) -- new on 061103
		for (int i = 0; i < number_of_buttons; i++) {
			buttons[i]->finish_instantly();
		};
		time_label->finish_instantly();
		last_screen_scale = tt_screen->current_scale();
	};
	if (buttons[0]->animation_in_progress()) { // || time_travel_buttons_hidden) {
		// rewritten on 061103 so even if time_travel_buttons_hidden buttons are updated (since may be flying up and down)
		return; // rewritten on 041103 so this doesn't interfere with auto hide but otherwise runs as before (which may be a bit more than needed)
	};
	if (!time_travel_buttons_hidden && pointing_cursor == NULL) { // second condition added 290404
#if TT_DEBUG_ON
			if (tt_debug_mode == 230404) {
				tt_error_file() << "update_time_travel_buttons calling show_cursor" << endl;
			};
#endif
		show_cursor(TRUE,TRUE); // added 041103 so don't get confused by no cursor
	};
	if (!displaying_pause_button()) { // condition new on 310803
		stop_sound(TRUE); // new on 110603 - especially if flying helicopter good to turn it off when paused like this
	};
	city_coordinate min_x = tt_screen->min_x();
	city_coordinate max_x = tt_screen->max_x();
	city_coordinate min_y = tt_screen->min_y();
	city_coordinate max_y = tt_screen->max_y();
	city_scale scale = tt_screen->current_scale();
	// TO_OLDEST_SEGMENT is a typical button -- beginning 100603 using fresh_button instead
	city_coordinate button_width = (fresh_button->current_width()*scale)/ground_scale;
	city_coordinate button_height = (fresh_button->current_height()*scale)/ground_scale; // all same height
	city_coordinate time_label_height = button_height/2; // abstracted on 051103
	city_coordinate total_width = 0;
	int i;
	//for (i = 0; i < number_of_buttons; i++) {
	//	total_width += (buttons[i]->current_width()*scale)/ground_scale;
	//};
	city_coordinate llx = min_x+((max_x-min_x)-number_of_buttons_displayed*button_width)/2; 
	// number_of_buttons*button_width was total_width prior to 100603
	city_coordinate lly = min_y; 
	// moved to the bottom on 051103 since at the top I don't get messages from Windows when on the the "title bar" was +time_label_height 
	// when time_display was underneath
	original_button_lly = lly; // new on 061103
	original_time_label_lly = lly+button_height; // new on 061103
	if (time_travel_buttons_hidden) { // new on 061103 -- keep them off screen but city scale may have changed so need to do this
		lly -= 2*button_height;
	};
//	city_coordinate lly = max_y-button_height;
	for (i = 0; i < number_of_buttons_displayed; i++) {
//		city_coordinate button_width = (buttons[i]->current_width()*scale)/ground_scale;
//		if (i == TIME_LABEL) { // add an extra width for the time label
//			llx += button_width;
//		};
//		buttons[i]->move_to(llx,lly);
		if (i == PLAY_TIME_TRAVEL) {
			buttons[PAUSE_TIME_TRAVEL]->set_size_and_location(button_width,button_height,llx,lly);
			if (buttons[PLAY_TIME_TRAVEL]->visible()) { // really only care about the first time - maybe this is more robust - new on 041103
				buttons[PAUSE_TIME_TRAVEL]->set_visible(FALSE);
//				displaying_play_button = TRUE; // new on 061103 since make buttons invisible when off screen (in case flying up and down)
			};
		};
		buttons[i]->set_size_and_location(button_width,button_height,llx,lly); // rewritten on 100603
		if (i == 1) { // since this is 5 buttons wide
			time_label->set_size_and_location(button_width*5,time_label_height,llx-button_width/2,lly+button_height); 
			// was +time_label_height when time label was above
//			time_label->set_size_and_location(button_width*5,time_label_height,llx-button_width/2,lly-time_label_height);
		};
//		if (i == TIME_LABEL) {
//			labels[TIME_LABEL]->set_size_and_location_to_those_of(buttons[i]);
//			if (!labels[TIME_LABEL]->fonts_fixed()) {
//				labels[TIME_LABEL]->fix_font_size(); // otherwise the size changes too much
//			};
//		} else {
//			labels[i]->move_to(llx+button_width/5,lly+button_height/10);
		   // rewritten on 140103 so that labels are the right size when flying the helicopter
		   // commented out on 140103 since better to display labels in the region of their buttons
//			labels[i]->set_size_and_location((button_width*scale)/ground_scale,(button_height*scale)/ground_scale,
														//llx+button_width/5,lly+button_height/10);
//		};
		llx += button_width; // next one
//		buttons[i]->set_scale(scale); // commented out on 100603
//		buttons[i]->update_display(); // to update clean run etc
//		labels[i]->update_display();
/*
		if (i != TIME_LABEL && (buttons_just_added || !buttons[i]->clean())) {
			if (buttons[i]->save_region()) {
				buttons[i]->display();
			};
			buttons[i]->reset_clean_status();
		};
		if (buttons_just_added || !labels[i]->clean()) {
			if (labels[i]->save_region()) {
				labels[i]->display();
			};
			labels[i]->reset_clean_status();
		};
*/
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 250404) {
		tt_error_file() << "About to reset grid from update_time_travel_buttons" << endl;
	};
#endif
//	reset_grid(); // new on 250404 -- commented out on 290404 - OK?
//#if TT_ALPHA_FEATURE
//	pause_appearance->move_to(play_appearance->current_llx(),play_appearance->current_lly()); 
	// initialize here since update_display isn't called as often as of 031103
//#endif
//	if (tt_time_travel != TIME_TRAVEL_ON) {
//		char temp_string[32];
//		day_and_time_string(temp_string);
////		time_label->set_text(temp_string,-1,FALSE,TRUE);
//		city_coordinate width = 10*tile_width; // tile_width*8; shortened a little bit on 140103 to look nicer // changed from 7 to 10 on 100603
//		city_coordinate llx = min_x+(tile_width*5*scale)/(ground_scale); // 5 was 13/2 prior to 100603
////		width = (width*scale)/ground_scale;
//		city_coordinate height = tile_height; // (max_y-min_y)/15;
//		city_coordinate lly = max_y-(3*height*scale)/(ground_scale); // was 7/3 prior to 100603
////		height = (height*scale)/ground_scale;
////		llx += tile_width/2; // add a bit since width is really maximum width since font size is defined as pixels
//		time_label->set_size_and_location((width*scale)/ground_scale,(height*scale)/ground_scale,llx,lly); // -- adjusted size for scale on 140103
////		city_coordinate good_width,good_height;
////		time_label->good_size(good_width,good_height);
////		time_label->set_size((good_width*60)/100,(good_height*60)/100);
////		time_label->update_display(); // commenting it out as an experiment on 110603
////		tt_screen->add(time_label); // shouldn't really be needed
////		if (!time_label->fonts_fixed()) {
////			time_label->fix_font_size(); // otherwise the size changes too much
////		};
//	};
//	buttons_just_added = FALSE;
};

void release_time_travel_buttons() {
	if (buttons != NULL) {
		//if (displaying_pause_button()) {
		//	play_appearance->destroy();
		//} else {
		//	pause_appearance->destroy();
		//};
		for (int i = 0; i < number_of_buttons; i++) {
//			tt_screen->remove(buttons[i]);
//			tt_screen->remove(labels[i]);
//			buttons[i]->remove_follower(labels[i]);
			buttons[i]->destroy();
//			labels[i]->destroy();
		};
		delete [] buttons;
//		delete [] labels;
		buttons = NULL;
//		labels = NULL;
	};
	if (time_label != NULL) {
//		tt_screen->remove(time_label);
		time_label->destroy();
		time_label = NULL;
	};
	if (pointing_cursor != NULL) {
		pointing_cursor->destroy();
		pointing_cursor = NULL;
	};
	//if (waiting_cursor != NULL) {
	//	waiting_cursor->destroy();
	//	waiting_cursor = NULL;
	//};
};

void time_travel_react(TimeTravelButton button) {
	play_sound(POP_SOUND,FALSE,TRUE);
//	time_travel_buttons_hidden = FALSE; // new on 041103 so buttons are updated and displayed as normal
	restore_time_travel_buttons(); // replaced the above on 041103
	for (int i = 0; i < number_of_buttons; i++) {
		buttons[i]->stop_animations();
	};
	if (button != QUIT_TIME_TRAVEL && button != PLAY_TIME_TRAVEL && displaying_pause_button()) {
		// new on 041103 - if replaying and any button other than record is pushed then pause
		use_play_appearance();
		time_travel(TIME_TRAVEL_PAUSED); // prior to 210204 was TIME_TRAVEL_JUST_PAUSED
	}
	switch (button) {
		// code simplified on 110603
		case TO_OLDEST_SEGMENT:
			jump_to_log_segment(tt_oldest_log_segment);
//			buttons[PAUSE_OR_PLAY_TIME_TRAVEL] = play_appearance; // new on 190603
			break;
		case TO_NEWEST_SEGMENT:
			jump_to_log_segment(tt_youngest_log_segment);
			break;
		case TO_PREVIOUS_SEGMENT:
			if (tt_current_log_segment > tt_oldest_log_segment && tt_current_log_segment > 1) {
				jump_to_log_segment(tt_current_log_segment-1);
//				buttons[PAUSE_OR_PLAY_TIME_TRAVEL] = play_appearance; // new on 190603
			} else {
				play_sound(PLOP_SOUND,FALSE,TRUE); // new on 230903 to indicate can't go back further in time
			};
			break;
		case TO_NEXT_SEGMENT:
			if (tt_current_log_segment < tt_youngest_log_segment) {
				jump_to_log_segment(tt_current_log_segment+1); 
//				buttons[PAUSE_OR_PLAY_TIME_TRAVEL] = play_appearance; // new on 190603
			} else {
				play_sound(PLOP_SOUND,FALSE,TRUE); // new on 230903 to indicate can't go forward further in time
			};
			break;
#if TT_ALPHA_FEATURE
		case PLAY_TIME_TRAVEL: // renamed 041103
#else
		case PAUSE_OR_PLAY_TIME_TRAVEL:
#endif
			if (displaying_pause_button()) {
				use_play_appearance();
				time_travel(TIME_TRAVEL_JUST_PAUSED);
			} else {
				use_pause_appearance();
				reset_grid(); // new on 290304 to remove "turds" from the display
				tt_screen->screen_dirty(); // new on 290304 to remove "turds" from the display
				time_travel(TIME_TRAVEL_ON);
			};		
			break;
		case QUIT_TIME_TRAVEL: // maybe this should be renamed...
			time_travel(TIME_TRAVEL_RECORD); // start recording
			break;
		//case REPLAY_SEGMENT:
		//	time_travel(TIME_TRAVEL_ON);
//			if (time_log_started == tt_current_time) {
//				jump_to_log_segment(current_log_segment_number);
//			};
//			break;
	};
};

void time_travel_react_to_mouse_down(city_coordinate x, city_coordinate y) {
	if (buttons == NULL) return; // something wrong
	TTRegion region;
	for (int i = 0; i < number_of_buttons_displayed; i++) { // since pause button shouldn't be considered here - new on 041103
		buttons[i]->full_region(region);
//		region.print(tt_error_file());
//		tt_error_file() << " point = " << x << "," << y << endl;
		if (region.inside(x,y)) {
			time_travel_react((TimeTravelButton) i);
			return;
		};
	};
	// new on 031103 that if a button down and buttons are hidden they are restored
	tt_time_travel_after_display_updated = TRUE;
};

void save_city_since_end_of_logging() {
	if (tt_log_out_archive == NULL) return; // e.g. playing someone else's time travel archive
//	UnwindProtect<boolean> set(tt_dump_as_xml,TRUE); // cities in log must be XML - new on 170903
	string file_name = most_recent_city_file_name();
//   if (tt_want_all_exception_breaks) {
//      tt_city->dump_to_new_file(file_name);
//   } else 
   if (tt_want_exception_breaks) {	
      try {
		   tt_city->dump_to_new_file(file_name,FALSE); // "saves" in tt_zip_description_archive_element -- changed from TRUE to FALSE on 220803
	   } catch (Sprite *) {
		   return;
      };
   } else {
      try {
		   tt_city->dump_to_new_file(file_name,FALSE);
	   } catch (...) { // not always during shut down but should be OK anyway
		   handle_internal_error(S(IDS_INTERNAL_ERROR_EXITING,"An internal error occurred while shutting down."),FALSE); 
	   };
   };
	do_postponed_include_files_xml(); // new on 190703 since time travel in archive and uses include files now
	zip_file(tt_log_out_archive,file_name,NULL,0,TRUE); // no compression since should already be a zip archive
	delete [] file_name;
	if (tt_include_media_in_time_travel_archives) {
		include_media_files_in_zip_description_archive_element(tt_log_out_archive); 
		// test on 200703 - should be conditional and have a nice interface
	};
};

string most_recent_city_file_name() {
	if (tt_user_directory == NULL) return(NULL); // new on 050300 -- not good if this is called before user directory is set
	return(append_strings(tt_user_directory,"current.xml.cty")); // added .xml on 140103 since this is used by time travel and it relies upon XML
};

#if TT_DEBUG_ON
boolean is_time_travel_button(Sprite *sprite) {
	if (buttons == NULL) return(FALSE);
	for (int i = 0; i < number_of_buttons; i++) {
		if (buttons[i] == sprite) return(TRUE);
	};
	return(sprite == time_label);
};
#endif




