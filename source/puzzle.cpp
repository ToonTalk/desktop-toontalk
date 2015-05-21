// Copyright (c) 1992-2007. Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)
#include "defs.h"
#endif
#if !defined(__TT_GLOBALS_H)
#include "globals.h"
#endif
#if !defined(__TT_HOUSE_H)
#include "house.h"
#endif
#if !defined(__TT_UTILS_H)
#include "utils.h"
#endif
#if !defined(__TT_HELP_H)
#include "help.h"
#endif
#if !defined(__TT_MARTIAN_H)
#include "martian.h"
#endif
#if !defined(__TT_BIRD_H)
#include "bird.h"
#endif
#if !defined(__TT_ROBOT_H)
#include "robot.h"
#endif
#if !defined(__TT_PICTURE_H)
#include "picture.h"
#endif
#if !defined(__TT_SPRITE_H)
#include "sprite.h"
#endif
#if !defined(__TT_PRGRMMR_H)
#include "prgrmmr.h"
#endif
#if !defined(__TT_CUBBY_H)
#include "cubby.h"
#endif
#if !defined(__TT_NUMBER_H)
#include "number.h"
#endif
#if !defined(__TT_TEXT_H)
#include "text.h"
#endif
#if !defined(__TT_SPRITES_H)
#include "sprites.h"
#endif
#if !defined(__TT_BCKGRND_H)
#include "bckgrnd.h"
#endif
#if !defined(__TT_FLOOR_H)
#include "floor.h"
#endif
#if !defined(__TT_ROOM_H)
#include "room.h"
#endif
#if !defined(__TT_BOMB_H)
#include "bomb.h"
#endif
#if !defined(__TT_PAD_H)
#include "pad.h"
#endif
#if !defined(__TT_STRINGS_H)
#include "strings.h"
#endif
#if !defined(__TT_WINMAIN_H)
#include "winmain.h"
#endif
#if !defined(__TT_LOG_H)
#include "log.h"
#endif
#if !defined(__TT_PUZZLE_H)
#include "puzzle.h"
#endif

//#include <fstream.h>
////#include <strstrea.h> commented out 210703
#if !TT_32
#include <stdlib.h> // for atoi
// Windows Header Files:
#include <windows.h>
#include <mmsystem.h>
#endif
#include <time.h>

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

string intro[2] = {NULL,NULL}; // both tt_use_spoken_alternative and !tt_use_spoken_alternative

Sprites *wall_sprites = NULL;

Sprite *goal_sprite = NULL;
Robot *conditions_as_robots = NULL;
Sprite *answer_sprite = NULL;
House *puzzle_house = NULL;
House *rocket_house = NULL;

PuzzleStatus puzzle_status = JUST_STARTED;

PuzzleStatus return_puzzle_status() { // new on 121102
   return(puzzle_status);
};

flag toolbox_part_of_puzzle_flag = FALSE; // unless proven otherwise
flag first_collision_with_marty = FALSE;
flag order_matters_explained = FALSE;

int empty_counter = 0;
short int puzzle_counter = -1;
short int user_puzzle_counter = 0;
short int original_puzzle_counter = -1;
short int last_puzzle_really_saved = -1;
short int first_puzzle_number = -1;
flag puzzle_loaded_fine = TRUE;
flag starting_fresh = TRUE;
flag last_puzzle_reset_old = TRUE;
flag user_did_good = FALSE;

Sprites *saved_robots = NULL;
Sprite *saved_item_in_hand = NULL;

const int max_line_length = 2048; // was 255 prior to 200400
const int max_lines_for_intro = 32;

long importance = 99; // starts off important and every time the next thing is more
int hint_counter = 0;
int hints_available[2] = {0,0}; // both tt_use_spoken_alternative and !tt_use_spoken_alternative
const int max_hint_count = 100; // per puzzle!
const int max_hint_length = 4000; // was 2000 prior to 200400

const int last_level = 5;
const short int level_start[last_level] = {1,21,31,41,61}; // puzzle counter compared to this
const short int level_count[last_level] = {17,8,9,14,14};  // slot_ids (1 to 18 here)

string *hints[2] = {NULL,NULL}; // both tt_use_spoken_alternative and !tt_use_spoken_alternative

const city_coordinate slot_width = 3800;
const city_coordinate slot_height = 1000;

long eight_seconds = 0;
long fourteen_seconds = 0; // so Marty can talk about them correctly

int house_count_at_start_of_bombs_introduced_puzzle = 2; // new on 260301

// following implements a cache of puzzle state in memory instead of disk
// problem is that state can get very large (e.g. a nest with lots of stuff on top)
// turn off cache if state gets too large
flag puzzle_state_in_memory = TRUE; // until it gets too large
bytes puzzle_state_encoding = NULL;
long puzzle_state_length = 0;
const int puzzle_state_max_length = 10000;  

enum KeywordIndices {FUNCTION_KEYS_ENABLED=0,
                     TYPING_TO_NUMBER_PADS_ENABLED,
                     TYPING_TO_TEXT_PADS_ENABLED,
                     FLIPPING_ENABLED,
                     GRASPING_NON_ESENTIALS_ENABLED,
                     RESET_ON_SOLUTION_ENABLED,
                     BRING_IN_VACUUM_WHEN_ROBOT_FAILS_ENABLED,
                     USER_PUZZLE_ENABLED,
                     ROBOTS_ARE_LOST_WHEN_SOLVED_ENABLED,
                     SAVED_ROBOTS_NOT_ADDED_ENABLED,
                     SOLUTION_POSTED_ENABLED,
                     HAND_EMPTY_AFTER_POSTING_ENABLED,
                     keyword_count};

//flag switches[keyword_count];
flag *switches = NULL;

void allocate_puzzle_data() {
   switches = new flag[keyword_count];
	for (int j = 0; j < 2; j++) {
		hints[j] = new string[max_hint_count];
		for (int i = 0; i < max_hint_count; i++) hints[j][i] = NULL;
	};
};

// following replaced by resource strings
//string keywords[keyword_count] =
//   {"FunctionKeys","NumberTyping","TextTyping","Flipping",
//    "GraspingNonEstentials","ResetOnSolution",
//    "BringInVacuumWhenRobotFails","UserPuzzle",
//    "RobotsAreLostWhenSolved","SavedRobotsNotAdded",
//    "GoodSolutionsPosted","HandEmptyAfterPosting"};

const int dusty_introduction_puzzle = 5;
const int dusty_spit_out_puzzle = 6;
const int blank_number_puzzle = 7;
const int first_robot_puzzle = 8;
const int second_robot_puzzle = 9;
const int one_billion_puzzle = 9;
const int birth_year_puzzle = 16;
const int binary_birth_year_puzzle = 17;
const int upper_case_alphabet_puzzle = 23;
const int lower_case_alphabet_puzzle = 24;
const int good_size_alphabet_puzzle = 25;
const int good_size_punctuation_puzzle = 27;
const int birth_day_puzzle = 31;
const int after_birth_day_puzzle = birth_day_puzzle+1;
const int seconds_in_hour_puzzle = 33;
const int introduce_birds_puzzle = 41;
const int stack_of_numbers_puzzle = 42;
const int sum_of_stack_puzzle = 43;
const int mystery_sensor_puzzle = 44;
const int nest_of_frozen_numbers_puzzle = 45;
const int sum_of_frozen_numbers_puzzle = 46;
const int eight_seconds_puzzle = 47;
const int fourteen_seconds_puzzle = 48;
const int bombs_introduced_puzzle = 49;
const int one_robot_for_two_puzzle = 50;
const int robot_stops_himself_puzzle = 51;
const int first_robot_team_puzzle = 53;
const int alarm_clock_team_puzzle = 54;
const int first_timer_puzzle = 61;
const int robot_on_back_puzzle = 62;
const int seconds_timer_puzzle = 63;
const int minutes_timer_puzzle = 64;
const int hours_timer_puzzle = 65;
const int modulo_seconds_timer_puzzle = 66;
const int modulo_minutes_timer_puzzle = 67;
const int modulo_hours_timer_puzzle = 68;
const int correct_time_puzzle = 69;
const int time_of_birth = 71;
const int seconds_until_millions = 72;
const int days_until_millions = 74;
const int last_puzzle = days_until_millions;

long millions_of_seconds_goal = 0;
long seconds_old = 0;

int alphabet_size() {
   return(_tcslen(S(IDS_ALPHABET_UPPER_CASE)));
};

void compute_age_in_seconds();

PuzzleState puzzle_state;

flag puzzles_resumed = FALSE;

flag explained_dots = FALSE; // this session

string Marty_was_saying[2] = {NULL,NULL}; // both tt_use_spoken_alternative and !tt_use_spoken_alternative

Sprite *previous_answer_sprite = NULL;

boolean ok_to_enter_house(House *house) {
   if (house == rocket_house) return(TRUE);
   return(puzzle_status != JUST_STARTED);
};

boolean puzzle_oks_action(Sprite *by, KeywordIndices keyword) {
   boolean ok = (tt_system_mode != PUZZLE || switches[keyword]);
   if (ok) return(TRUE);
   if (by != NULL && by->showing_on_a_screen()) play_sound(SHUTDOWN_SOUND); // removed ,5 on 070604 which caused it to repeat the sound since 200204
   if (by != NULL && by->still_running()) return(TRUE); // trained robot ok
   // add sound that says -- "Sorry, temporarily out of order."
   return(FALSE);
};

boolean puzzle_oks_function_keys(Sprite *by) {
   return(puzzle_oks_action(by,FUNCTION_KEYS_ENABLED));
};

boolean puzzle_oks_typing_to_number_pads(Sprite *by) {
   return(puzzle_oks_action(by,TYPING_TO_NUMBER_PADS_ENABLED));
};

boolean puzzle_oks_typing_to_text_pads(Sprite *by) {
   return(puzzle_oks_action(by,TYPING_TO_TEXT_PADS_ENABLED));
};

boolean puzzle_oks_flipping(Sprite *by) {
   return(puzzle_oks_action(by,FLIPPING_ENABLED));
};

Sprite *read_sprite(InputStream *file, boolean ok_for_string_to_be_text_or_number);

void interpret_switches(InputStream *file);
void respond_to_solution();
void update_loaded_puzzle();
void save_robots_on_floor(Background *floor);
void update_floor_items(SpritesPointer &items, string items_encoding, long length,
                        Background *floor, boolean reset_old);

void toolbox_is_part_of_puzzle() { // was commented out prior to 290399
   toolbox_part_of_puzzle_flag = TRUE;
};

boolean is_toolbox_part_of_puzzle() {
   return(toolbox_part_of_puzzle_flag);
};

int slot_id = 1;
int last_slot_id;
int level = 0;

Sprites *initial_rocket_items = NULL;
Sprites *saved_level_stuff = NULL;
Sprites *saved_floor_items = NULL;
Sprites *previous_saved_floor_items = NULL;
Sprites *saved_items_from_non_reset_floor = NULL;
Sprite *saved_tool_in_hand_non_reset = NULL;

void lower_left_corner_of_slot(int id, city_coordinate &slot_llx, city_coordinate &slot_lly) {
   if (id == 1 || id == 5 || id == 10 || id == 15 || id < 1 || id > 18) {
      slot_llx = 10500;
   } else if (id == 9 || id == 14) {
      slot_llx = 5400;
   } else if (id == 2 || id == 6 || id == 11 || id == 16) {
      slot_llx = 15800;
   } else if (id == 3 || id == 7 || id == 12 || id == 17) {
      slot_llx = 20800;
   } else if (id == 4 || id == 8 || id == 13 || id == 18) {
      slot_llx = 26500;
   };
   if (id < 5 || id > 18) {
      slot_lly = 10300;
   } else if (id < 9) {
      slot_lly =  9000;
   } else if (id < 14) {
      slot_lly =  6600;
   } else {
      slot_lly =  5200; // was 5000
   };
   if (id == 13 || id == 18) { // aren't quite in the same row
      slot_llx += 200;
      slot_lly -= 200;
   };
};

void initialize_rocket_insides() {
   if (rocket_house == NULL) return; // too early
   Room *room = rocket_house->pointer_to_room();
   room->set_wall_decoration(initial_rocket_items,FALSE,FALSE);   // want to see numbers with their pads
   if (initial_rocket_items != NULL) { // should I destroy them??
      initial_rocket_items = NULL;
   } else {
      city_coordinate slot_llx, slot_lly;
      for (int i = 18; i > last_slot_id; i--) {
         lower_left_corner_of_slot(i,slot_llx,slot_lly);
         room->add_decoration(new Picture(slot_llx-100,slot_lly-100,
                                          max_long-1,0,SYNTHETIC_SPRITE,
                                          slot_width+200,slot_height+200),
                              TRUE);
      };           
   };
};

void reset_level() { // made a procedure on 120700
	destroy_sprites(saved_level_stuff);
   destroy_sprites(saved_floor_items);
   destroy_sprites(previous_saved_floor_items);
   initialize_rocket_insides();
	puzzle_state_in_memory = TRUE;
   if (puzzle_state_encoding != NULL) { // level is over and no state is carried over so restore things
      delete [] puzzle_state_encoding;
      puzzle_state_encoding = NULL;
   };
   puzzle_state_length = 0;
};

void update_slot_ids(boolean initialization) {
   if (user_puzzle_counter > 0) { // user puzzles
      last_slot_id = 19;  // leave all the slots open
      if (user_puzzle_counter > 18) { // restart
         if (!initialization) {
            play_sound(LOTS_OF_SILLY_SOUNDS);
            initialize_rocket_insides();
         };
         user_puzzle_counter = 1;
         slot_id = 1;
      };
   } else {
      slot_id = 1+puzzle_counter-level_start[level];
      last_slot_id = level_count[level];
      while (slot_id > level_count[level]) {
         if (level == last_level) break;
         level++;
         slot_id = 1+puzzle_counter-level_start[level];
//       slot_id = 1;
         if (!initialization) play_sound(LOTS_OF_SILLY_SOUNDS);
         last_slot_id = level_count[level];
         if (!initialization) {
            reset_level();
         };
      };
   };
};

boolean next_puzzle_name() {
#if TT_TRIAL_LIMITS_PUZZLES
	if (puzzle_counter >= 21 && tt_expires) { // new on 090807
		launch_help("trialpuz.htm");
		set_user_wants_to_quit(TRUE);
		return FALSE;
	};
#endif
	boolean name_is_new;
	string new_name = next_file_name(tt_puzzle_file_name,name_is_new,FALSE); // FALSE added 301199 so u1.pzl etc work OK
	if (new_name == NULL) return(FALSE);
	if (name_is_new) {
		delete [] tt_puzzle_file_name;
		tt_puzzle_file_name = new_name;
	};
	if (tt_alternative_spoken_puzzle_file_name != NULL) {
		new_name = next_file_name(tt_alternative_spoken_puzzle_file_name,name_is_new,FALSE);
		if (new_name == NULL) return(FALSE);
		if (name_is_new) {
			delete [] tt_alternative_spoken_puzzle_file_name;
			tt_alternative_spoken_puzzle_file_name = new_name;
		};
	};
	if (user_puzzle_counter > 0) {
      slot_id = user_puzzle_counter;
      user_puzzle_counter++;
   };
	return(TRUE);
};

boolean first_digits_of_puzzle_name(short int &first_digit_position, short int &last_digit_position) {
   if (tt_puzzle_file_name == NULL) return(FALSE);
   short int length = (short int) strlen(tt_puzzle_file_name);
   short int i = (short int) (length-1);
   for (;; i--) { // search from end backwards
      if (i == 0) return(FALSE); // didn't find a dot
      if (tt_puzzle_file_name[i] == '.') break;
   };
   i--; // skip back over dot
   last_digit_position = i;
   for (;; i--) { // search from end backwards
      if (i == 0) return(FALSE); // didn't find a digit
      if (tt_puzzle_file_name[i] < '0' || tt_puzzle_file_name[i] > '9') break;
   };
   first_digit_position = (short int) (i+1);
	if (tt_puzzle_file_name[i] == 'p' || tt_puzzle_file_name[i] == 'P') { // rewrote on 030300 since was setting to TRUE when it shouldn't
		tt_user_puzzles = FALSE;
	} else if (tt_puzzle_file_name[i-1] != '\\') {
		tt_user_puzzles = TRUE;
	};
   return(TRUE);
};

boolean new_puzzle_name(int counter) {
   short int first_digit, ignore_last_digit;
   if (!first_digits_of_puzzle_name(first_digit,ignore_last_digit)) return(FALSE);
   output_string_stream stream(tt_puzzle_file_name+first_digit,strlen(tt_puzzle_file_name)-first_digit);
   stream << counter << ".pzl";
   stream.put('\0'); // terminate
   return(TRUE);
};

PuzzleLoadStatus reload_puzzle(House *new_puzzle_house) {
   puzzle_status = WAITING_FOR_SOLUTION;
   starting_fresh = TRUE;
   if (new_puzzle_house != NULL) {
      puzzle_house = new_puzzle_house;
		if (saved_items_from_non_reset_floor != NULL) {
			Floor *floor = puzzle_house->pointer_to_floor();
			Sprites *remaining = saved_items_from_non_reset_floor;
         while (remaining != NULL) {
				Sprite *sprite = remaining->first();
				remaining->set_first(NULL);
//				if (sprite->kind_of() != BOMB) {
					floor->add_item(sprite);
//				};
				remaining = remaining->rest();
			};
			delete saved_items_from_non_reset_floor; // just the conses
			saved_items_from_non_reset_floor = NULL;
			if (saved_tool_in_hand_non_reset != NULL) {
				tt_programmer->forget_tool_in_hand();
		      tt_programmer->set_tool_in_hand(saved_tool_in_hand_non_reset);
				saved_tool_in_hand_non_reset = NULL;
			};
		};
//   } else {
//      if (!switches[RESET_ON_BAD_ANSWER_ENABLED]) return;
   };
   release_puzzle_data(FALSE);
   return(load_puzzle(NULL,(new_puzzle_house == NULL)));
};

boolean puzzle_ok(PuzzleLoadStatus status) {
	return(status != NO_PUZZLE_FILE && status != PUZZLE_FILE_BAD);
};

PuzzleLoadStatus next_puzzle(boolean reset_old, boolean save_robots) {
   if (next_puzzle_name()) {
      last_puzzle_reset_old = reset_old;
      release_puzzle_data(FALSE);
      empty_counter = 0;
      hint_counter = 0;
      if (puzzle_counter != 0) {
         puzzle_counter++;
//         update_slot_ids();
      };
      puzzle_status = JUST_STARTED;
      first_collision_with_marty = FALSE; // so you don't get a hint after solving a puzzle
      PuzzleLoadStatus load_status = load_puzzle(puzzle_house,reset_old,save_robots);
      if (load_status == NO_PUZZLE_FILE) {
         if (original_puzzle_counter != puzzle_counter) {
            puzzle_status = ALL_PUZZLES_SOLVED;
				play_sound(LOTS_OF_SILLY_SOUNDS); // new on 120700
				// commented out on 081200 since that interferred with getting the "present"
//				puzzle_counter = 1; // new on 120700 so that if the same user tries puzzles again they start over
            return(load_status);
         }; // else give warning about missing file??
      };
      return(load_status);
   } else {
      puzzle_status = ALL_PUZZLES_SOLVED;
		play_sound(LOTS_OF_SILLY_SOUNDS); // new on 120700
//		puzzle_counter = 1; // new on 120700 so that if the same user tries puzzles again they start over
      return(NO_PUZZLE_FILE);
   };
};

void next_puzzle_default() {
   next_puzzle(switches[RESET_ON_SOLUTION_ENABLED],
               !switches[ROBOTS_ARE_LOST_WHEN_SOLVED_ENABLED]);
};
//
//void previous_puzzle_default() { // new on 141104
//
//};

Sprites *floor_items(string floor_encoding, long length, boolean , Picture *controlee=NULL) {
   StringInputStream floor_stream(floor_encoding,length);
   if (floor_stream.get() != WHOLE_FLOOR) { // something is wrong
      log(S(IDS_PUZZLE_FOUND_WRONG_TOKEN),FALSE,TRUE);
      return(NULL);
   };
   NaturalLanguage language;
   character cookie_version_number;
   if (!read_cookie(&floor_stream,cookie_version_number,language)) return(NULL);
   if (cookie_version_number >= 8) { // floor token is also dumped now
      floor_stream.get(); // ignore token
   };
   release_nest_indices(); // just in case
   Sprites *items = load_items(&floor_stream,cookie_version_number,tt_language,controlee,FALSE);
   if (items == NULL && puzzle_counter != bombs_introduced_puzzle) return(NULL); // not even bomb since saved...
   if (last_puzzle_reset_old) { // && tt_bomb_to_reset_puzzle) {
      items = new Sprites(new Bomb(15*tile_width,0,max_long),items);
   };// indirects to number
   return(items);
};

flag first_puzzle = TRUE;

PuzzleLoadStatus process_puzzle_file(string file_name, boolean reset_old, boolean save_robots, 
												 House *house, boolean skip_robots,
												 SpritesPointer &items, 
												 string &floor_encoding, long &floor_encoding_length) {
	if (file_name == NULL) return(NO_PUZZLE_FILE);
	FileNameStatus file_status;	
	string new_file_name = existing_file_name(file_status,file_name,"pzl","puzzles",TRUE,FALSE
#if TT_POST3187
		// added final TRUE arg to the following for language specific default on 090807
		,TRUE
#endif
		); // caches if URL - last 2 args added 100700 to prevent warning to user of missing files
	if (new_file_name == NULL) return(NO_PUZZLE_FILE);
	FileInputStream file;
	file.open(new_file_name,std::ios_base::in|std::ios_base::binary); // |std::ios_base::nocreate
	// added std::ios_base::binary so that tellg/seekg work properly
	delete [] new_file_name;
   if (file.empty()) {
      return(NO_PUZZLE_FILE);
   };
   long length;
	delete [] intro[tt_use_spoken_alternative];
   intro[tt_use_spoken_alternative] = read_lines(&file,max_line_length*max_lines_for_intro,length,line_terminator());
   if (intro[tt_use_spoken_alternative] == NULL) return(EMPTY_PUZZLE_FILE); // zero-length file - updated on 080402
   if (_tcsncmp(intro[tt_use_spoken_alternative],_T("Skip this puzzle."),strlen("Skip this puzzle.")) == 0) { // was strncmpi
      return(next_puzzle(reset_old,save_robots));
   };
	if (!tt_use_spoken_alternative && // new on 200400
		 already_has_extension(intro[tt_use_spoken_alternative],"htm") || // new on 030400
		 already_has_extension(intro[tt_use_spoken_alternative],"html")) {
		// if first line of the puzzle ends with .htm or .html then end puzzle game and display file
//		ascii_string main_dir = ini_entry(AC(IDC_DIRECTORIES),AC(IDS_MAIN_DIR),FALSE);
//		ShellExecuteG(NULL,"open",intro[tt_use_spoken_alternative],"",main_dir,SW_SHOW); // main_dir new on 200400
//		if (main_dir != NULL)  delete [] main_dir;
//		set_user_wants_to_quit(TRUE); // well maybe not but has no choice
		// re-wrote the above as follows on 080500
		tt_puzzles_over_html_file = copy_string(intro[tt_use_spoken_alternative]);
		return(NO_PUZZLE_FILE);
	};
//   if (puzzle_counter > 10) {  // feeling better
//      martian_just_injured(); // not dizzy
//   } else {
//      martian_just_injured_and_dizzy();
//   };
//   wall_preface = new char[max_line_length];
//   file.getline(wall_preface,max_line_length);
   Floor *floor = house->pointer_to_floor();
//   boolean need_to_restore_room = FALSE;
   Room *room = floor->pointer_to_room();
   if (tt_screen->pointer_to_background() != room && !tt_use_spoken_alternative) { 
		// if not in standing in the room move stuff if possible to floor
      tt_programmer->move_stuff_in_room_to_floor(room);
//      need_to_restore_room = TRUE;
   };
   Sprite *first_decoration = read_sprite(&file,TRUE);
   if (first_decoration == NULL) return(PUZZLE_FILE_BAD);
//   // make it an indirect picture to be sure it is a "subpicture"
//   wall_sprites = new Sprites(make_indirect_picture(first_decoration,FALSE),wall_sprites);
   wall_sprites = new Sprites(first_decoration,wall_sprites);
//   house->pointer_to_room()->set_wall_decoration(first_decoration);
//   wall_postfix = new char[max_line_length];
//   file.getline(wall_postfix,max_line_length);
   string floor_uuencoding = read_lines(&file,32000,length,'\r');
   if (floor_uuencoding == NULL) return(PUZZLE_FILE_BAD);
	string xml_string = strstr(floor_uuencoding,"<?xml");
	if (xml_string != NULL) { // new on 180803
		items = xml_load_sprites_from_floor(xml_string);
		delete [] floor_uuencoding;
	} else {
		NaturalLanguage ignore_language;
		int floor_encoding_version_number;   // ignored now
		floor_encoding_length;
		floor_encoding = uudecode(floor_uuencoding,floor_encoding_length,ignore_language,floor_encoding_version_number);
		if (floor_encoding == NULL) return(PUZZLE_FILE_BAD);
		delete [] floor_uuencoding;
		items = floor_items(floor_encoding,floor_encoding_length,reset_old);
	};		
   if (items == NULL && last_puzzle_reset_old) {
      if (saved_floor_items == NULL) {
         items = previous_saved_floor_items;
         previous_saved_floor_items = NULL; // new on 221002 since not needed anymore (and could be deleted)
      } else {
         items = saved_floor_items; // loaded in from saved state
         destroy_sprites(previous_saved_floor_items);
         previous_saved_floor_items = saved_floor_items->top_level_copy();
         saved_floor_items = NULL;
      };
   } else if (saved_floor_items != NULL) { // won't need them after all
      destroy_sprites(previous_saved_floor_items);
      previous_saved_floor_items = saved_floor_items; //->copy(floor);
      saved_floor_items = NULL;
   };
   if (!tt_use_spoken_alternative && tt_programmer->pointer_to_tool_in_hand() == NULL && saved_item_in_hand != NULL) { // was starting_fresh  &&
//      Sprite *tool_in_hand = saved_item_in_hand->top_level_copy();
      if (saved_item_in_hand->current_width() <= 0 || saved_item_in_hand->current_height() <= 0) {
         saved_item_in_hand->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL); // arg new on 141204 (and elsewhere in this file)
			// new on 180302 -- best modularity to fix the problem that numbers were very small if left in hand
      };
      tt_programmer->set_tool_in_hand(saved_item_in_hand);
      saved_item_in_hand->recursively_activate_pictures();
      saved_item_in_hand = saved_item_in_hand->top_level_copy(); // in case of reset;
   };
   if (puzzle_counter == dusty_introduction_puzzle && !tt_use_spoken_alternative) { 
		// stuff "glued" to floor
      Sprites *remaining = items;
      while (remaining != NULL) {
         remaining->first()->set_priority_fixed(TRUE);
         remaining = remaining->rest();
      };
   };
	if (!tt_use_spoken_alternative) {
		starting_fresh = FALSE;
		destroy_sprites(saved_items_from_non_reset_floor);
		if (saved_tool_in_hand_non_reset != NULL) {
			saved_tool_in_hand_non_reset->destroy();
			saved_tool_in_hand_non_reset = NULL;
		};
		if (reset_old) {
			if (save_robots) {
				save_robots_on_floor(floor);
	// following removed since copy of robot is added so old robot restored
	//      } else if (saved_robots != NULL &&
	//                 !switches[SAVED_ROBOTS_NOT_ADDED_ENABLED]) {
				// maybe restarting this puzzle
				// if user brought robot back to Marty might be in trouble
	//         Sprites *remaining = saved_robots;
	//         while (remaining != NULL) {
	//            remaining->first()->remove_from_floor();
	//            remaining = remaining->rest();
	//         };
			};
			room->set_wall_decoration_sprite(NULL);
			room->set_initial_items(NULL);
			floor->set_initial_items(items);
//			just_destroy_list(items); // new on 071100 - - commented out since done by update_floor_items
		} else { 
			// save stuff in house in case need to restore after bomb used
			saved_items_from_non_reset_floor = floor->pointer_to_items();
			if (saved_items_from_non_reset_floor != NULL) {
				UnwindProtect<Sprite*> set(tt_top_copy,tt_global_picture); // to set up the scope of the copy
				Sprite *tool_in_hand = tt_programmer->pointer_to_tool_in_hand();
				if (tool_in_hand != NULL && puzzle_counter != bombs_introduced_puzzle) { 
					// for bomb puzzle tool in hand not reset so houses can be blown up but shouldn't have thing in hand still
					// copy them as unit preserving relationships
					saved_tool_in_hand_non_reset = tool_in_hand->copy();
				};
				saved_items_from_non_reset_floor = saved_items_from_non_reset_floor->copy();
				release_items_copied();
			};
			// and add to old ones
			floor->add_items(items);
//			just_destroy_list(items); // new on 071100 - - commented out since done by update_floor_items
		};
		release_nest_indices(); // just in case
	};
	if (goal_sprite != NULL) {
		goal_sprite->destroy(); // new on 240101
	};
   goal_sprite = read_sprite(&file,TRUE);
   if (goal_sprite == NULL) {
		tt_error_file() << S(IDS_ENCODING_MISSING_IN_PUZZLE) << endl;
		return(PUZZLE_FILE_BAD);
	};
   goal_sprite->inside_thought_bubble(); // since that's how it acts
// Microsoft seems to have a bug in the implementation of tellg()
	std::streampos file_position = file.tellg();
	// added on 050499
	if (!skip_robots) {
		Sprite *robots = read_sprite(&file,FALSE);
		if (robots == NULL) {
			file.seekg(file_position); // restore its position
//			// rewritten on 130499 to re-read the entire file rather than trust tellg/seekg
//			goal_sprite->destroy();
//			floor->set_initial_items(NULL);
//			return(load_puzzle(house,reset_old,save_robots,TRUE)); // try again skipping robots
		} else if (robots->kind_of() != ROBOT) {
			robots->destroy();
//			goal_sprite->destroy();
//			floor->set_initial_items(NULL);
			tt_error_file() << S(IDS_PUZZLE_CONTAINED_NON_ROBOT_AFTER_GOAL) << endl;
//			return(load_puzzle(house,reset_old,save_robots,TRUE)); // try again skipping robots
		} else {
			conditions_as_robots = (Robot *) robots;
		};
	};
   interpret_switches(&file);
   if (!switches[SAVED_ROBOTS_NOT_ADDED_ENABLED] && !tt_use_spoken_alternative) {
		// re-written so that copies end up both added to floor and to items
//      floor->add_copy_of_items(saved_robots);
		Sprites *remaining = saved_robots;
      while (remaining != NULL) {
			Sprite *copy = remaining->first()->copy();
         floor->add_item(copy);
			items = new Sprites(copy,items);
         remaining = remaining->rest();
		};
      release_items_copied();
//      release_nests_being_copied(); // just in case copying bird/nest pairs
//      items = floor->pointer_to_items();
   };
   if (!switches[GRASPING_NON_ESENTIALS_ENABLED] && !tt_use_spoken_alternative) {
      Sprites *remaining = items;
      while (remaining != NULL) {
         Sprite *sprite = remaining->first();
         if (!sprite->is_tool() && sprite->kind_of() != BOMB &&
             match(goal_sprite,sprite,default_robot_version) != MATCH_GOOD) {
            sprite->set_graspable(FALSE);
         };
         remaining = remaining->rest();
      };
   };
	hints_available[tt_use_spoken_alternative] = 0;
   while (!file.empty() && 
		    (hints[tt_use_spoken_alternative][hints_available[tt_use_spoken_alternative]] = 
			  read_lines(&file,max_hint_length,length,line_terminator())) != NULL) {
      if (hints_available[tt_use_spoken_alternative] >= max_hint_count) {
         delete [] hints[tt_use_spoken_alternative][hints_available[tt_use_spoken_alternative]];
         break; // too many -- something probably wrong
      };
      if (hints[tt_use_spoken_alternative][hints_available[tt_use_spoken_alternative]][0] == NULL) { // empty string
         delete [] hints[tt_use_spoken_alternative][hints_available[tt_use_spoken_alternative]];
//          hints_available--;
         break;
      };
      hints_available[tt_use_spoken_alternative]++;
   };
   file.close();
	return(PUZZLE_LOADED_FINE);
};

boolean ok_to_start_puzzles_again() { // abstracted on 120401
	if (message_box(MB_YESNO|MB_SYSTEMMODAL,S(IDS_FINISHED_ALL_PUZZLES_START_AGAIN)) == IDYES) { 
      puzzle_state.counter = 1;
		puzzle_counter = 1;
      update_slot_ids(FALSE);
		puzzles_resumed = FALSE; // new on 120401
		resume_puzzles();
//		return(PUZZLE_LOADED_FINE); // most likely
		return(TRUE);
   } else {
      PostQuitMessage(0);
		return(FALSE);     
   };
};

PuzzleLoadStatus load_puzzle(House *house, boolean reset_old, boolean save_robots, boolean skip_robots) {
   if (first_puzzle && puzzle_state.counter >= level_start[last_level-1]+level_count[last_level-1] && tt_puzzle_file_name == NULL) { 
		if (!ok_to_start_puzzles_again()) {
			return(PUZZLES_ALL_DONE); 
		};
   };
	boolean original_first_puzzle = first_puzzle; // new on 130401
   first_puzzle = FALSE;
   if (house == NULL) {
      house = puzzle_house;
   } else {
      puzzle_house = house;
   };
   if (house == NULL || tt_puzzle_file_name == NULL) return(NO_PUZZLE_FILE);
	SpritesPointer items = NULL;
	string floor_encoding = NULL;
   long floor_encoding_length;
	PuzzleLoadStatus status;
	PuzzleLoadStatus alternative_language_status = PUZZLE_LOADED_FINE; // unless proven otherwise
	if (alternative_language_spoken() && tt_alternative_spoken_puzzle_file_name != NULL) {
		tt_use_spoken_alternative = TRUE;
		alternative_language_status = process_puzzle_file(tt_alternative_spoken_puzzle_file_name,reset_old,save_robots,
											   						  house,skip_robots,
																		  items,floor_encoding,floor_encoding_length);
		tt_use_spoken_alternative = FALSE;
		destroy_sprites(items); // just a copy with the "wrong" language on it
		if (floor_encoding != NULL) delete [] floor_encoding;
	};
	status = process_puzzle_file(tt_puzzle_file_name,reset_old,save_robots,
										  house,skip_robots,
										  items,floor_encoding,floor_encoding_length);
   if (status == EMPTY_PUZZLE_FILE) { // new on 080402 to treat empty puzzle files as "next puzzle"
      // note that while this worked it didn't initialize the rocket with "old" items - so check for zero size earlier now
      resume_puzzles(); // will recompute tt_puzzle_file_name
      return(load_puzzle(house,reset_old,save_robots,skip_robots));
   };
	if (!puzzle_ok(status)) {
		if (tt_puzzles_over_html_file != NULL && original_first_puzzle) { // new on 120401 original_first_puzzle added 130401
			if (!ok_to_start_puzzles_again()) {
				return(PUZZLES_ALL_DONE);
			} else {
				delete [] tt_puzzles_over_html_file;
				tt_puzzles_over_html_file = NULL;
				return(load_puzzle(house,reset_old,save_robots,skip_robots)); // ok_to_start_puzzles_again has reset numbers back to puzzle #1
			};
		};
		return(status);
	};
	if (!puzzle_ok(alternative_language_status)) { // moved here on 161199 so if last puzzle isn't called
		// turn off second language
		tt_alternative_spoken_language = NO_LANGUAGE_SPECIFIED;
		tt_error_file() << "Something wrong with " << tt_alternative_spoken_puzzle_file_name
						<< " so second language ignored now." << endl;
		tt_err_file_important = TRUE;
	};
   if (puzzle_counter < 0 || !puzzles_resumed) { // need to initialize it
      puzzles_resumed = TRUE; // if started by -puzzle then initialize once
      int i = strlen(tt_puzzle_file_name);
      for (; i > 0; i--) {
         if (tt_puzzle_file_name[i] == '.') {
            i--;
            break;
         };
      };
      if (i == 0) i = strlen(tt_puzzle_file_name);
      int j = i;
//      AnsiUpper(tt_puzzle_file_name); // to test for "p<n>"
		// above commented out since maybe a URL that is case sensitive
      for (; j >= 0; j--) {
         if (tt_puzzle_file_name[j] < '0' ||
             tt_puzzle_file_name[j] > '9') break;
      };
      if (j == i || (tt_puzzle_file_name[j] != 'P' && tt_puzzle_file_name[j] != 'p')) {
         // no digits or not a "p" file
         puzzle_counter = 0;
         user_puzzle_counter = 1;
         slot_id = 1;
      } else {
         char number_string[32];
         memcpy(number_string,tt_puzzle_file_name+j+1,i-j);
         number_string[i-j] = NULL;
         puzzle_counter = (short int) atoi(number_string);
//         if (puzzle_counter == 0) puzzle_counter++;
      };
      for (; level < last_level; level++) {   // was last_level-1 -- why??
        if (puzzle_counter < level_start[level]) {
           level--;
           break;
        };
      };
      if (level == last_level) {
         level--;
      };
      last_slot_id = level_count[level];
      original_puzzle_counter = puzzle_counter;
   };
	Floor *floor = house->pointer_to_floor();
   if (!switches[USER_PUZZLE_ENABLED]) {
      int saved_puzzle_counter = puzzle_counter;
      update_floor_items(items,floor_encoding,floor_encoding_length,floor,reset_old);
      if (saved_puzzle_counter != puzzle_counter) {
         if (new_puzzle_name(puzzle_counter)) {
				just_destroy_list(items);
            return(next_puzzle(TRUE,FALSE));
         };
      };
   };
   update_loaded_puzzle();
   if (items != NULL) items->recursively_activate_pictures();
   if (wall_sprites != NULL) {
      Room *room = puzzle_house->pointer_to_room();
      Sprite *first_decoration = wall_sprites->first(); // may have been updated by update_loaded_puzzle
      Text *label = NULL;
      if (first_decoration->kind_of() == TEXT) {
         label = (Text *) first_decoration;
         label->set_use_variable_width_font(TRUE);
         // was 2/3 by 1/2
         label->set_size((43*ideal_screen_width)/100,ideal_screen_height/3);
      };
      city_coordinate decoration_width = first_decoration->current_width();
      city_coordinate decoration_height = first_decoration->current_height();
      city_coordinate wall_bottom = 8*tile_height;
      city_coordinate first_llx = tile_width+(ideal_screen_width-decoration_width)/2;
      city_coordinate first_lly = wall_bottom+((ideal_screen_height-wall_bottom)-decoration_height)/2;
      first_decoration->move_to(first_llx,first_lly);
      Picture *frame = new Picture(tile_width*4,wall_bottom+tile_height,max_long,0,FRAME_SPRITE);
      frame->set_size((8*decoration_width)/5,(3*decoration_height)/2);
      room->set_wall_decoration(wall_sprites,TRUE);
      room->add_decoration(frame);
      if (label != NULL) {
         city_coordinate goal_width,goal_height,goal_llx,goal_lly;
         if (label->size_and_location_of_characters(
#if TT_WIDE_TEXT
																	 L'#',
#else
																	 '#',
#endif
																	  label->current_width(),label->current_height(),
																	  goal_width,goal_height,goal_llx,goal_lly)) {
//				// due to round off added the following on 290699
//				const city_coordinate x_pixel = tt_screen->one_pixel_in_x_city_units();
//				const city_coordinate y_pixel = tt_screen->one_pixel_in_y_city_units();
//				goal_width += x_pixel;
//				goal_height += y_pixel;
//				goal_llx -= x_pixel;
//				goal_lly -= y_pixel;
            goal_sprite->set_true_size_and_location(goal_width,goal_height,goal_llx,goal_lly); // true added on 290399
				goal_sprite->update_display(); // new on 070705 -- not clear this helps at all - could it hurt? ask 110705
				Sprite *goal_sprite_copy;
				if (!goal_sprite->rectangular()) {
					// new on 290399 so if goal isn't rectangular it is placed on a white rectangle
					goal_sprite_copy = new Picture(0,0,0,0,SYNTHETIC_SPRITE);
					goal_sprite_copy->set_size_and_location(goal_width,goal_height,goal_llx,goal_lly);
					goal_sprite_copy->set_parameter(255);
					goal_sprite_copy->add_follower(goal_sprite->top_level_copy());
				} else {
					goal_sprite_copy = goal_sprite->top_level_copy();
				};
//				goal_sprite_copy->refit_contents(); // new on 060705 since can look bad otherwise -- didn't help
//            wall_sprites->last_cons()->set_rest(new Sprites(goal_sprite->copy(NULL)));
				boolean saved_show_all = goal_sprite_copy->show_all(); // saved it since following triggers show some via become_part_of_picture
            room->add_decoration(goal_sprite_copy,TRUE,(goal_sprite->kind_of()==CUBBY));
				goal_sprite_copy->set_show_all(saved_show_all); // new on 230902 since numbers and the like should show the pad
         };
      };
//      wall_sprites->recursively_activate_pictures();   // taken care of by add_decoration now
      wall_sprites = NULL;
   };
//   if (need_to_restore_room) tt_programmer->move_stuff_on_floor_to_room(room);
   delete [] floor_encoding;
   just_destroy_list(items);
	if (first_puzzle_number == -1) { // new on 140300 to fix bug where it said you solved puzzle 4 to 4 when it should be 1 to 4
		first_puzzle_number = puzzle_counter;
	};
   return(PUZZLE_LOADED_FINE);
};

void player_has_sat() {
   previous_answer_sprite = NULL; // might get changed
   Sprites *remaining = puzzle_house->pointer_to_floor()->pointer_to_items();
   while (remaining != NULL) {
       remaining->first()->dramatic_entrance(1500); // 1.5 seconds
       remaining = remaining->rest();
   };
};

void player_leaving_room(Room *room) {
	int start, stop;
	if (alternative_language_spoken()) {
		start = 0;
		stop = 1;
	} else {
		start = tt_use_spoken_alternative;
		stop = tt_use_spoken_alternative;
	};
	UnwindProtect<boolean> save(tt_use_spoken_alternative);
	for (int i = start; TRUE; i++) {
		if (alternative_language_spoken()) { 
			// condition new on 141204 so if BRITISH and AMERICAN are alternatives that there isn't a spurious missing puzzle file warning message box
			tt_use_spoken_alternative = i;
		};
		if (Marty_was_saying[i] != NULL) { // private copy
			delete [] Marty_was_saying[i];
			Marty_was_saying[i] = NULL;
		};
		if (tt_martian->displaying_dots()) {
			Marty_was_saying[i] = copy_string(tt_martian->currently_saying());
		};
		if (i == stop) break;
	};
   if (room == rocket_house->pointer_to_room() && puzzle_status == PUZZLE_JUST_LOADED) {
      puzzle_status = WAITING_FOR_SOLUTION;
   };
	tt_martian->remove_text(); // new on 290301 since if left while talking some state is preserved that confuses next interaction
};

void martian_stopped_talking() {
   if (user_did_good) {  // feeling better
      martian_just_injured_and_pointing(); // not dizzy
   } else {
      martian_just_injured_and_dizzy();
   };
};

void save_robots_on_floor(Background *floor) {
   destroy_sprites(saved_robots);
   Sprites *remaining = floor->pointer_to_items();
   while (remaining != NULL) {
      if (remaining->first()->kind_of() == ROBOT) {
//         floor->remove_item(remaining->first());
         saved_robots = new Sprites(remaining->first()->copy(),saved_robots);
      };
      remaining = remaining->rest();
   };
   release_items_copied();
//   release_nests_being_copied(); // just in case copying bird/nest pairs
};

Vacuum *dusty = NULL;

void robot_failed(Robot *robot) {
   if (!switches[BRING_IN_VACUUM_WHEN_ROBOT_FAILS_ENABLED]) return;
	boolean on_the_floor = (tt_programmer->kind_of() == PROGRAMMER_AT_FLOOR);
   if (dusty == NULL) {
		if (on_the_floor) {
			dusty = new Vacuum(-10*tile_width,8*tile_height);
		} else {
			city_coordinate dusty_llx = 4*tile_width;
			city_coordinate dusty_lly = tile_height;
			dusty = new Vacuum(dusty_llx,dusty_lly);
			city_coordinate dusty_width = dusty->current_width();
         city_coordinate dusty_height = dusty->current_height();
         tt_programmer->translate_floor_to_room_coordinates(dusty_llx,dusty_lly);
         tt_programmer->translate_floor_to_room_dimensions(dusty_width,dusty_height);
         dusty->set_size_and_location(dusty_width,dusty_height,dusty_llx,dusty_lly);
		};
   };
   robot->pointer_to_background()->add_item(dusty,TRUE,FALSE,FALSE); // don't warn if already there
	if (!on_the_floor) {
		return;
	};
	Programmer_At_Floor *state = tt_programmer->programmer_at_floor();
   dusty->move_to_hand(); // state);
	state->called_for(dusty);
};

//void entering_house(House *house) { // nothing just now
//};

Sprite *read_sprite(InputStream *file, boolean ok_for_string_to_be_text_or_number) {
   long length;
   string uuencoding = read_lines(file,32000,length,'\r');
   if (uuencoding == NULL) return(NULL);
   Sprite *sprite = uudecode_sprite(uuencoding,FALSE,ok_for_string_to_be_text_or_number);
   delete [] uuencoding;
   return(sprite);
};

void read_line(InputStream *file, string destination, long max_length, long &characters_read) {
	// like getline but considers both \n and \r as delimiters
	characters_read = 0;
	while (!file->empty() && characters_read < max_length) {
		char character = file->get();
		if (character == -1) {
			break; // added 031199 since the end of puzzles sometimes included a couple -1s
		};
		if (new_line(character)) {
			destination[characters_read] = '\0'; // terminate it
			char next_character = file->peek();
			if (next_character != character && new_line(next_character)) {
				file->get(); // ignore second terminator
			};
#if TT_DEBUG_ON
			if (characters_read > 255) { 
				tt_error_file() << "A line of a puzzle didn't read properly prior to 20/04/00 because its length is " << characters_read << endl;
				tt_error_file().write(destination,characters_read);
				tt_error_file() << endl;
			};
#endif
			return;
		} else {
			destination[characters_read] = character;
			characters_read++;
		};
	};
	destination[characters_read] = '\0';
#if TT_DEBUG_ON
	if (characters_read > 255) { 
		tt_error_file() << "A line of a puzzle didn't read properly prior to 20/04/00 because its length is " << characters_read << endl;
		tt_error_file().write(destination,characters_read);
		tt_error_file() << endl;
	};
#endif
};

string read_lines(InputStream *file, long max_length, long &length, char line_terminator) {
   string result = new character[max_length];
   length = 0;
   int line_count = 0;
   while (TRUE) {
      if (file->empty()) {
			if (line_count > 0 && length > 3) {
				return(result);// changed from NULL to result on 161099 since files that don't end with a blank line are OK
			} else {
				delete [] result;
				return(NULL); 
			};
      };
//      file.getline(result+length,max_line_length);
		// the above doesn't behave the same when the file is binary
		// and yet it needs to be binary to fix bugs in tellg
		// changed on 280499
		long characters_read = 0;
		read_line(file,result+length,max_line_length,characters_read);
      line_count++;
//      int characters_read = file.gcount();
      if (characters_read <= 1) { // was <= 2 prior to 280499
         // ignore leading empty lines
         if (line_count > 1) {
				return(result); // line feed might be read
			} else {
				line_count--; // ignore this one
			};
      } else if (line_terminator == 0) { // e.g. Japanese
			length += characters_read; 
		} else {
         if (length > 0) {
            result[length-1] = line_terminator; // keep the lines seperated
         };
         length += characters_read+1; // +1 added on 280499 to work with read_line
         while (result[length-2] == line_terminator) { // really removes extra spaces at the end of lines
            length--; // remove redundant terminator
         };
      };
      if (length+max_line_length > max_length) {
         log(S(IDS_PUZZLE_PART_TOO_LONG),FALSE,TRUE);
         delete [] result;
         return(NULL);
      };
   };
};

#define GOALBOX_STRING _T("Goal box:")

void interpret_switches(InputStream *file) {
  for (int i = 0; i < keyword_count; i++) switches[i] = TRUE; // default
   // at the end are optionally a sequence of
   // feature=on or feature=off
   // one per line
   string line = new character[256];
   while (!file->empty()) {
//      file.getline(line,256);
		long line_length;
		read_line(file,line,max_line_length,line_length); // revised on 280499 - max_line_length was 256 prior to 200400
      while (line[0] == ' ') line++; // remove initial blanks.
      if (line[0] == NULL || (new_line(line[0]) && line[1] == NULL)) break; // was  == '\r' prior to 090802
      string goal_box = _tcsstr(line,GOALBOX_STRING);
      if (goal_box != NULL) {
         int prefix_length = _tcslen(GOALBOX_STRING);
         input_string_stream box(line+prefix_length,_tcslen(line)-prefix_length);
         short int llx=0, lly=0, width=0, height=0; // percent
         box >> llx >> lly >> width >> height;
         if (height != 0) {
            goal_sprite->set_size_and_location(width*(ideal_screen_width/100),
                                               height*(ideal_screen_height/100),
                                               llx*(ideal_screen_width/100),
                                               lly*(ideal_screen_height/100));
            insert_at_end_if_not_member(goal_sprite->top_level_copy(),wall_sprites);
         };
      } else {
         for (int i = 0; i < keyword_count; i++) {
            if (_tcsstr(line,S(IDS_PUZZLE_SWITCH+i)) != NULL) {      // keywords[i]
               for (int j = 0; j < 256; j++) {
                  if (line[j] == 0) {
                     log(S(IDS_NO_EQUAL_SIGN_IN_PUZZLE),FALSE,TRUE);
                     break; // terminated without finding
                  };
                  if (line[j] == '=') {
                     if (_tcsstr(line+j,_T("on")) != NULL) {
                        switches[i] = TRUE;
                        break;
                     } else if (_tcsstr(line+j,_T("off")) != NULL) {
                        switches[i] = FALSE;
                        break;
                     };
                  };
               };
            };
         };
      };
   };
   delete [] line;
};

#if TT_TEXT_TO_SPEECH
flag said_marty_talks_funny = FALSE;
#endif

string return_intro() {
   string greeting = new character[1024];
   output_string_stream stream(greeting,1024);
   if (puzzle_counter == original_puzzle_counter) {
      if (puzzle_counter == 1) {
         stream << S(IDS_FIRST_PUZZLE_GREETING1);
         if (!limited_text_to_speech()) { // can't pronounce every name
            stream << S(IDS_FIRST_PUZZLE_GREETING2) << space() << tt_user_name << end_sentence();
         };
#if TT_TEXT_TO_SPEECH
        if (text_to_speech() && return_use_count() == 1 && !said_marty_talks_funny && !alternative_language_spoken()) { // first use
           stream << S(IDS_MARTY_TALKS_FUNNY);
//           if (!tt_use_spoken_alternative) said_marty_talks_funny = TRUE;
        };
#endif
         stream << S(IDS_FIRST_PUZZLE_GREETING3) << space();
      } else if (puzzle_counter > 1) { // starting after p1
         tt_martian->say_hi(stream);
         stream << S(IDS_RESTART_PUZZLE_GREETING1+my_random_unless_tt_use_spoken_alternative(3));
			// was timeGetTime()%3 to be more random but interferes with logging (and time travel)
         stream << space(); // was space() - don't know why
       };
   } else {
      stream << S(IDS_CONGRATULATIONS+my_random_unless_tt_use_spoken_alternative(6));
      if (!limited_text_to_speech()) { // can't pronounce every name
         if (my_random_unless_tt_use_spoken_alternative(3) == 0) stream << space() << tt_user_name;
      };
      if (my_random_unless_tt_use_spoken_alternative(3) == 0) {
         stream << end_sentence('!');
      } else {
         stream << end_sentence();
      };
   };
   if (intro[tt_use_spoken_alternative] != NULL) stream << intro[tt_use_spoken_alternative];
   stream.put('\0');
   return(greeting);
};

void say_intro() {
	string alternative_spoken_greeting = NULL;
	if (alternative_language_spoken()) {
		tt_use_spoken_alternative = TRUE;
		alternative_spoken_greeting = return_intro();
		tt_use_spoken_alternative = FALSE;
	};
	string greeting = return_intro();
	tt_martian->say(greeting,importance++,alternative_spoken_greeting);
   delete [] greeting;
	if (alternative_spoken_greeting != NULL) delete [] alternative_spoken_greeting;
};

void set_rocket(House *house) {
   rocket_house = house;
   update_slot_ids(TRUE);
   initialize_rocket_insides();
};

House *pointer_to_rocket() {
   return(rocket_house);
};

House *pointer_to_puzzle_house() {
   return(puzzle_house);
};

void set_puzzle_house(House *house) {
	puzzle_house = house;
};

boolean inside_rocket() {
   // just a house for now
   if (tt_system_mode != PUZZLE) return(FALSE);
   Sprite *appearance = tt_programmer->pointer_to_appearance();
   if (appearance == NULL) return(FALSE);
   Background *background = appearance->pointer_to_background();
   if (background == NULL) return(FALSE);
   return(background->pointer_to_house() == rocket_house);
};

const_string try_again() {
  return(S(IDS_TRY_AGAIN1+my_random_unless_tt_use_spoken_alternative(2)));
};

string return_thanks() {
   string thanks = new character[1024];
   output_string_stream stream(thanks,1024);
   stream << S(IDS_SOLVED_ALL_PUZZLES); // used to be say(...) to add to current
#if !TT_SMALL_TRIAL
   if (tt_puzzles_over_html_file == NULL) { // conditional new on 080500
		stream << S(IDS_SOLVED_ALL_PUZZLES2);
	};
#endif
   stream << S(IDS_CHECK_FOR_MORE_PUZZLES);
	stream.put('\0');
	return(thanks);
};

void say_thanks() {
	string alternative_spoken_thanks = NULL;
	if (alternative_language_spoken()) {
		tt_use_spoken_alternative = TRUE;
		alternative_spoken_thanks = return_thanks();
		tt_use_spoken_alternative = FALSE;
	};
	string thanks = return_thanks();
	tt_martian->say(thanks,importance++,alternative_spoken_thanks);
   delete [] thanks;
	if (alternative_spoken_thanks != NULL) delete [] alternative_spoken_thanks;
};

void puzzle_talk() {
//#if TT_DEBUG_ON
//	if (tt_debug_mode == 44444 && tt_debug_target == 44444) { // just for now
//		say_thanks();
//		return;
//	};
//#endif
   do
     switch (puzzle_status) {
        case WAITING_FOR_SOLUTION:
           first_collision_with_marty = TRUE; // just entered room
        case WAITING_TO_VERIFY_SOLUTION:
           respond_to_solution();
           break;
        case JUST_STARTED:
           first_collision_with_marty = FALSE;
           if (puzzle_loaded_fine) {
              say_intro();
              puzzle_status = PUZZLE_JUST_LOADED;
//              puzzle_status = WAITING_FOR_SOLUTION; // set now when leaving rocket
           } else {
              say(IDS_BAD_PUZZLE_FILE,importance+99);
              log(a2u(tt_puzzle_file_name));
              puzzle_status = ALL_PUZZLES_SOLVED;
           };
           break;
        case RESPONDED:
           break; // nothing for now  -- teleport out??
        case ALL_PUZZLES_SOLVED:
           break;
     } while (puzzle_status == JUST_STARTED); // or restarted
	  if (puzzle_status == ALL_PUZZLES_SOLVED && !tt_user_puzzles) { // !tt_user_puzzles mistakening removed from 161199 to 281199
		  say_thanks();
        Floor *floor = puzzle_house->pointer_to_floor();
        Room *room = floor->pointer_to_room();
        room->set_wall_decoration_sprite(NULL);
        wide_string thanks = copy_wide_string(S(IDS_THANKS));
        Text *text = new Text(tile_width*5,12*tile_height,thanks,wide_string_length(thanks),max_long,10*tile_width,tile_height*5);
#if TT_WIDE_TEXT
		  delete [] thanks; // copied above
#endif
		  room->add_decoration(text,TRUE);
        floor->set_initial_items(NULL);
	  };       
};

void give_hint(output_stream &stream) {
	if (hint_counter < hints_available[tt_use_spoken_alternative]) { // was <=
      if (!tt_use_spoken_alternative) first_collision_with_marty = FALSE; // only one hint per visit
      if (hints[tt_use_spoken_alternative][hint_counter] != NULL) {
		   stream << hints[tt_use_spoken_alternative][hint_counter] << space(); // space not always needed
		};
	   if (!tt_use_spoken_alternative) hint_counter++;
	} else if (!bomb_ever_used() && hint_counter == hints_available[tt_use_spoken_alternative]) { // removed +1 here on 180201
      stream << S(IDS_BOMB_TO_RESTART);
      if (!tt_use_spoken_alternative) hint_counter++;
   } else if (hints_available[tt_use_spoken_alternative] > 0) {
      // there are some hints so but all given so start repeating last hint
      stream << S(IDS_ALL_HINTS_GIVEN1+my_random_unless_tt_use_spoken_alternative(3));
      if (hints[tt_use_spoken_alternative][hints_available[tt_use_spoken_alternative]-1] != NULL) {
		   stream << hints[tt_use_spoken_alternative][hints_available[tt_use_spoken_alternative]-1];
		};
   } else {
      stream << S(IDS_NO_HINTS) << space() << try_again(); // space added on 120700
   };
};

void colliding_with_marty() {
   if (puzzle_status != PUZZLE_JUST_LOADED && 
		 puzzle_status != WAITING_TO_VERIFY_SOLUTION && // new on 101199 so don't get a hint while waiting for the bird
		 puzzle_status != ALL_PUZZLES_SOLVED && // new on 120700
		 ((first_collision_with_marty && answer_sprite == NULL) || !tt_martian->still_talking())) {
		// added answer_sprite == NULL on 080301 so if you come back with the wrong thing you don't get a hint unless Marty is finished talking
      first_collision_with_marty = FALSE;
      string buffer = new character[1024];
      output_string_stream stream(buffer,1024);
		string alternative_spoken_buffer = NULL;
		if (alternative_language_spoken()) {
			alternative_spoken_buffer = new character[1024];
			output_string_stream alternative_spoken_stream(alternative_spoken_buffer,1024);
			tt_use_spoken_alternative = TRUE;
			give_hint(alternative_spoken_stream);
			tt_use_spoken_alternative = FALSE;
			alternative_spoken_stream.put('\0');
		};
	   give_hint(stream);
      stream.put('\0');
      tt_martian->say(buffer,importance-1,alternative_spoken_buffer); // add to current
      delete [] buffer;
      user_did(GLOBAL_HISTORY,USER_HAS_ASKED_FOR_PUZZLE_HINT);
   };
};

Robot *find_robot_on_flip_side(Sprite *sprite) {
   if (sprite->kind_of() != PICTURE) return(NULL); // warn??
	Picture *picture = (Picture *) sprite;
	Picture *controlee = picture->pointer_to_controlee();
	if (controlee != NULL) {
		return(find_robot_on_flip_side(controlee));
	};
   Sprites *remaining = picture->flipped_followers();
   while (remaining != NULL) {
      if (remaining->first()->kind_of() == ROBOT) {
         return((Robot *) (remaining->first()));
      };
      remaining = remaining->rest();
   };
   return(NULL);
};

void post_solution(boolean good, boolean keep_solution, boolean dump) {
   if (!good) { // tt_bomb_to_reset_puzzle && 
      if (tt_programmer->pointer_to_tool_in_hand() != NULL) {
         play_sound(SHUTDOWN_SOUND);
      };
      return; // just hold on to it
   };
	if (puzzle_counter-1 == bombs_introduced_puzzle) {
		// no need to post anything -- leave slot empty
		update_slot_ids(FALSE);
	   return; 
	};
   Sprite *programmer_appearance = tt_programmer->pointer_to_appearance();
   Room *room = programmer_appearance->pointer_to_background()->pointer_to_room();
   Sprite *solution;
   // consider the idea that HAND_EMPTY_AFTER_POSTING_ENABLED is a bad idea and should be equivalent to
   // SOLUTION_POSTED_ENABLED
   if (good && keep_solution) {
      solution = new Picture(0,0, 
                             max_long,0,SYNTHETIC_SPRITE,
                             slot_width/4,slot_height/4);
      solution->set_parameter(2);  // non-black
      saved_item_in_hand = tt_programmer->pointer_to_tool_in_hand();
      if (saved_item_in_hand != NULL) {
         saved_item_in_hand = saved_item_in_hand->top_level_copy(); // in case of reset
      };
   } else if (good && !switches[HAND_EMPTY_AFTER_POSTING_ENABLED]) {
      solution = tt_programmer->pointer_to_tool_in_hand();
      if (solution != NULL) {
         solution = solution->top_level_copy(); // was just copy prior to 060600 - does it really matter?
         if (saved_item_in_hand != NULL) {
            saved_item_in_hand->destroy();
         };
         saved_item_in_hand = solution->top_level_copy(); // in case of reset
         play_sound(MAGIC_SOUND);
      };
   } else {
      if (saved_item_in_hand != NULL) {
         saved_item_in_hand->destroy();
         saved_item_in_hand = NULL;
      };
      solution = tt_programmer->release_tool_in_hand();
      if (solution == NULL) return;
   };
   city_coordinate x,y;
   programmer_appearance->center_location(x,y);
   solution->move_to(x,y);
   room->add_item(solution);
   if (good) {
      city_coordinate slot_llx,slot_lly;
      lower_left_corner_of_slot(slot_id,slot_llx,slot_lly); 
      // was slot_id-1 for previous slot
//   slot_id++;
      room->give_item_priority(solution);
//      solution->set_priority(room->current_priority()); // behind everything except stuff on wall already
   // make the box a bit bigger than the slot to cover off by 1 errors
		// don't play the sound if the level finished sound is happening as well
      if (slot_id != 1 || puzzle_counter == original_puzzle_counter+1) play_sound(CALCULATING_SOUND);
      solution->animate_size_and_location(slot_width+200,slot_height+200,
                                          slot_llx-100,slot_lly-100,2000,
                                          solution,ADD_DECORATION_CALLBACK);
      if (dump) dump_history(TRUE); // in case of a crash knows which puzzles done... - conditional added 041199
   } else { // should allow user to trash bad answer -- change when I have reset button
		// is this obsolete now?
      tt_martian->center_location(x,y);
      solution->animate_size_and_location(tile_width/2,tile_height/2,x,y,
                                          1500,solution,DESTROY_CALLBACK);
      play_sound(SHUTDOWN_SOUND);
   };
};

//long birth_year = 1985;  // in case one starts with this puzzle

boolean save_birth_year(Sprite *answer, output_stream &stream) {
   long birth_year;
   answer->current_long_value(birth_year);
   if (birth_year < 1000) birth_year += 1900; // ok to say 1985 or 85 -- was < 100 prior to 210800
   puzzle_state.birth_year = (short int) birth_year;
// following only works for 32 bit windows
//   SYSTEMTIME time;
//   GetLocalTime(&time);
//   long age = time.wYear-birth_year;
   time_t timer;
   struct tm *tblock;
   timer = seconds_since_1970();
   tblock = localtime(&timer); // converts date/time to a structure
   long age = 1900+tblock->tm_year-birth_year;
   if (age < 0) {
      stream << S(IDS_NEGATIVE_AGE) << try_again();
      return(FALSE);
   } else if (age < 5) {
      stream << S(IDS_VERY_YOUNG1) << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << age 
			    << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_VERY_YOUNG2);
      return(TRUE); // but let it go
   } else if (age > 95) {
      stream << S(IDS_VERY_OLD1)<< space(SPACE_BEFORE_OR_AFTER_NUMBERS) << age
             << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_VERY_OLD2) << try_again();
      return(FALSE);
   };
   return(TRUE);
};

boolean save_birth_time(Sprite *answer, output_stream &stream) {
   long birth_hour, birth_minute, birth_second;
   Cubby *cubby = (Cubby *) answer;
   Sprite *hour = cubby->component(0);
   Sprite *minute = cubby->component(1);
   Sprite *second = cubby->component(2);
   hour->current_long_value(birth_hour);
   minute->current_long_value(birth_minute);
   second->current_long_value(birth_second);
   if (birth_hour < 0 || birth_hour > 23) {
      stream << S(IDS_BAD_TIME) << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
      print_integer_with_commas(birth_hour,stream);
      stream << end_sentence();
      return(FALSE);
   } else if (birth_minute < 0 || birth_minute > 59) {
      stream << S(IDS_BAD_TIME) << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
      print_integer_with_commas(birth_minute,stream);
      stream << end_sentence();
      return(FALSE);
   } else if (birth_second < 0 || birth_second > 59) {
      stream << S(IDS_BAD_TIME) << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
      print_integer_with_commas(birth_second,stream);
      stream << end_sentence();
      return(FALSE);
   };
   puzzle_state.birth_hour = (char) birth_hour;
   puzzle_state.birth_minute = (char) birth_minute;
   puzzle_state.birth_second = (char) birth_second;
   return(TRUE);
};

void add_robot_to_saved_level_stuff() {
	if (tt_use_spoken_alternative) return; // new on 021199 so don't get 2 copies if running bilingually
   Background *room = puzzle_house->pointer_to_room();
   Sprites *remaining = room->pointer_to_items();
   while (remaining != NULL) {
      if (remaining->first()->kind_of() == ROBOT) {
         Robot *robot = (Robot *) (remaining->first()->copy());
         robot->morph_to_inert();
         robot->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL);
         insert_at_end_if_not_member(robot,saved_level_stuff);   // NULL was floor
         break; // just save the only robot at the end of the list
      };
      remaining = remaining->rest();
   };
   release_items_copied();
//   release_nests_being_copied(); // just in case copying bird/nest pairs
};

millisecond time_nest_delivered = 0;

Sprite *saved_answer = NULL;

boolean wait_for_delivery(Sprite *answer, output_stream &stream) {
   if (answer == NULL) {
      if (saved_answer == NULL) return(FALSE);
		// conditional added 091199 and elsewhere in this procedure
		answer = saved_answer;
   };
   if (answer->kind_of() == NEST) {
      if (!tt_use_spoken_alternative) saved_answer = answer;
      Nest *nest = (Nest *) answer;
      if (nest->stack_empty()) {  // answer should be an empty nest initially
         if (time_nest_delivered == 0) { // first time
            if (!tt_use_spoken_alternative) time_nest_delivered = tt_current_time;
            if (!tt_use_spoken_alternative) post_solution(TRUE,FALSE,FALSE);
            stream << S(IDS_WAIT_FOR_BIRD_TO_BRING_MESSAGE);
            if (!tt_use_spoken_alternative) puzzle_status = WAITING_TO_VERIFY_SOLUTION;
            tt_martian->give_help_after(1000); // check back in a second
            return(TRUE);
         } else if (tt_current_time > time_nest_delivered+25000) {
            stream << S(IDS_GIVING_UP_ON_BIRD) << try_again();
//            tt_programmer->set_tool_in_hand(answer);
            if (!tt_use_spoken_alternative) saved_answer = NULL;
            if (!tt_use_spoken_alternative) time_nest_delivered = 0;
            if (!tt_use_spoken_alternative) puzzle_status = WAITED_TOO_LONG_TO_VERIFY_SOLUTION;
            return(TRUE);
         } else {
            if (!tt_use_spoken_alternative) tt_martian->give_help_after(1000);
            return(TRUE); // just wait some more
         };
      } else if (time_nest_delivered != 0) {
         Sprite *top = nest->dereference();
         if (top->kind_of() == TEXT) {
				if (!tt_use_spoken_alternative) {
					dump_history(TRUE); // new on 041199 since otherwise nest is slated for deletion and bird won't work
					puzzle_counter++;
					update_slot_ids(FALSE);
					dump_history(TRUE);
					puzzle_counter--; // am confused why this fixes the bug of skipping over puzzle #61
					next_puzzle_default();
				};
            return(TRUE);
          };
       } else {
          stream << S(IDS_NEST_SHOULD_BE_EMPTY) << try_again();
          return(TRUE);
       };
   };
   return(FALSE);
};

boolean respond_to_days_until_millions(Sprite *answer, output_stream &stream) {
   long number;
   answer = answer->dereference();
   answer->current_long_value(number);
   compute_age_in_seconds();
   long seconds = millions_of_seconds_goal-seconds_old;
   long hours = seconds/3600;
   long days = hours/24;
   hours = hours%24;
   if (number >= 0 && number < 13) {
      stream << S(IDS_MAYBE_YOU_SHOULD_DO_SOMETHING_SPECIAL) << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
      if (days > 1) {
         stream << days << space(SPACE_BEFORE_OR_AFTER_NUMBERS) 
				    << S(IDS_DAYS) << space() << S(IDS_AND) << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
      } else if (days == 1) {
         stream << days << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_DAY) 
				    << space() << S(IDS_AND) << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
      }; // else nothing
      if (hours > 1) {
         stream << hours << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_HOURS);
      } else if (hours == 1) {
         stream << hours << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_HOUR);
      } else {
         stream << seconds/60 << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_MINUTES);
      };
      stream << space() << S(IDS_TO_CELEBRATE_BEING) << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
      print_integer_with_commas(millions_of_seconds_goal/1000000L,stream);
      stream << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_MILLIONS_OF_SECONDS_OLD);
      return(TRUE);
   } else {
//      stream << S(IDS_ANSWER_MUST_BE_LESS_THAN_TWELVE);
      return(FALSE);
   };
};

boolean integer_is_between(Sprite *answer, output_stream &stream, long minimum, long maximum=max_long, 
									const_string special_message=NULL) {
   long number;
   answer = answer->dereference();
   answer->current_long_value(number);
   if (number >= minimum && number <= maximum) return(TRUE);
   if (answer_sprite != previous_answer_sprite) {
      if (special_message == NULL) {
         stream << S(IDS_WE_NEED1) << space();
			if (tt_language == BRAZILIAN_PORTUGUESE) {
				stream << "de "; // added on 160600 to handle the special "de" particle for "need" in Portuguese
			};
         print_type(INTEGER,stream,INDEFINITE_ARTICLE);
         if (number < minimum) {
            stream << space() << S(IDS_BIGGER_THAN) << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
				print_integer_with_commas(minimum-1,stream);
         } else {
            stream << space() << S(IDS_LESS_THAN) << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
				print_integer_with_commas(maximum+1,stream);
         };
         stream << end_sentence();
      } else {
         stream << special_message;
      };
   };
   return(FALSE);
};

boolean all_3_non_zero(Sprite *answer, output_stream &stream) {
   Cubby *box = (Cubby *) answer;
   for (int i = 0; i < 3; i++) {
       Sprite *element = box->component(i);
       if (element->remote_status() != NOT_REMOTE) {
          stream << S(IDS_NO_CHANGING_NUMBERS);
          return(FALSE);
       };
      long n;
      element->current_long_value(n);
      if (n == 0) {
         stream << S(IDS_ALL_NUMBERS_MORE_THAN_ZERO);
         return(FALSE);
      };
   };
   return(TRUE);
};      

const char month_length[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

void print_date(long birth_month, long birth_day, long year, output_stream &stream) {
   // add year if not 0
   switch (current_language()) {
       case AMERICAN:
//       case BRITISH:
          stream << S(IDS_JANUARY+birth_month) << space() << birth_day;
          break;
       default:
          stream << birth_day << space() << S(IDS_JANUARY+birth_month);
   };
   if (year != 0) {
      stream << comma() << space() << year;
   };
};

boolean check_birth_day(Sprite *answer, output_stream &stream) {
	if (alternative_language_spoken() && // new on 011199 to deal with dual language use
		 ((tt_language == JAPANESE && !tt_use_spoken_alternative) || // if Japanese only check spoken language
		  (tt_language != JAPANESE && tt_use_spoken_alternative))) { // else only check textual language
		 return(TRUE); 
	};
   long birth_day;
   Cubby *box = (Cubby *) answer;
	short int month_location, day_location;
	if (tt_language == AMERICAN) {
		month_location = 0;
		day_location = 1;
	} else {
		month_location = 1;
		day_location = 0;
	};
   Sprite *month = box->component(month_location);
   Sprite *day = box->component(day_location);
   day->current_long_value(birth_day);
   string month_string;
   long month_string_length;
   month->current_text(month_string,month_string_length);
//   month_string[3] = NULL; // truncate it
   int birth_month = 0;
   boolean month_found = FALSE;
   if (month_string_length >= 3) { // just first 3 letters count
      for (; birth_month < 12; birth_month++) {
			month_found = (_tcsnicmp(S(IDS_JANUARY+birth_month),month_string,3) == 0); // January is 0, etc.
			if (month_found) {
				break;
			};
      };
   };
   if (!month_found) {
      stream << S(IDS_NO_SUCH_MONTH) << space();
      stream.write(month_string,month_string_length);
      stream << end_sentence();
      return(FALSE);
   };
   if (birth_day < 1 || 
      (birth_day > month_length[birth_month] || // leap day
       (birth_day == 29 && birth_month == 2 && puzzle_state.birth_year%4 == 0))) {
      stream << S(IDS_BAD_DATE) << space();
      print_date(birth_month,birth_day,0,stream);
      stream << end_sentence();
      return(FALSE);
   };      
   puzzle_state.birth_month = (char) birth_month;
   puzzle_state.birth_day = (char) birth_day;
   return(TRUE);
};

boolean good_size_box(Sprite *answer, output_stream &stream) {
   city_coordinate width = answer->current_width();
   if (width > 21*tile_width) {  
      stream << S(IDS_TOO_WIDE_TO_SEE) << space();
      return(FALSE);
   };
   if ((width < 12*tile_width && 
        puzzle_counter == good_size_alphabet_puzzle) ||
       (width < 3*tile_width &&
        puzzle_counter == good_size_punctuation_puzzle)) {
      stream << S(IDS_TOO_NARROW_TO_SEE) << space();
      return(FALSE);
   };
   return(TRUE);
};

void find_and_call_pumpy() { // Programmer_At_Floor *the_programmer) {
   Floor *floor = puzzle_house->pointer_to_floor();
   Sprites *remaining = floor->pointer_to_items();
   while (remaining != NULL) {
      Sprite *sprite = remaining->first();
      if (sprite->kind_of() == EXPANDER) {
         sprite->move_to_hand(); // the_programmer);
         return;
      };
      remaining = remaining->rest();
   };
   // something wrong if got here
};

void find_and_call_dusty() { // Programmer_At_Floor *the_programmer) {
   Floor *floor = puzzle_house->pointer_to_floor();
   Sprites *remaining = floor->pointer_to_items();
   while (remaining != NULL) {
      Sprite *sprite = remaining->first();
      if (sprite->kind_of() == VACUUM) {
         sprite->move_to_hand(); // the_programmer);
         return;
      };
      remaining = remaining->rest();
   };
   // something wrong if got here
};

void find_and_call_wand() { // Programmer_At_Floor *the_programmer) {
   Floor *floor = puzzle_house->pointer_to_floor();
   Sprites *remaining = floor->pointer_to_items();
   while (remaining != NULL) {
      Sprite *sprite = remaining->first();
      if (sprite->kind_of() == COPIER) {
         sprite->move_to_hand(); // the_programmer);
         return;
      };
      remaining = remaining->rest();
   };
   // something wrong if got here
};

void function_key_pressed(char key) { // Programmer_At_Floor *the_programmer) {
   // already checked that function keys enabled and sound already made
   switch (key) {
       case VK_F2:
          find_and_call_dusty(); // the_programmer);
          break;
       case VK_F3:
          find_and_call_pumpy(); // the_programmer);
          break;
       case VK_F5:
          find_and_call_wand(); // the_programmer);
          break;
   };
};

void reset_robots_wands_counter(int new_counter, Sprites *items) {
   Sprites *remaining = items;
   while (remaining != NULL) {
      if (remaining->first()->kind_of() == ROBOT) {
         Sprites *followers = remaining->first()->pointer_to_followers();
         while (followers != NULL) {
            if (followers->first()->kind_of() == COPIER) {
               ((Copier *) followers->first())->set_counter(new_counter);
               return;
            };
            followers = followers->rest();
         };
      };
      remaining = remaining->rest();
   };
};

void reset_to_start_of_level() {
   update_slot_ids(TRUE);
   int difference = puzzle_counter-level_start[level];
   if (difference > 0) {
      character message[256];
      output_string_stream stream(message,256);
      stream << S(IDS_GO_BACK_TO_START_OF_PUZZLE_LEVEL) << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << difference
             << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_GO_BACK_TO_START_OF_PUZZLE_LEVEL2);
      stream.put((char) 0);
      if (message_box(MB_ICONQUESTION|MB_YESNO,message) == IDYES) {
         puzzle_counter = level_start[level];
         puzzle_counter--; // since will go to "next" puzzle and increment it
      };
   };
};

void connect_number_and_remotes(SpritesPointer &items,
                                string items_encoding, long length,
                                Background *floor, boolean reset_old,
//                                int version_number, // of items_encoding
                                Picture *picture=NULL, boolean picture_in_hand=FALSE) {
//   Picture *picture = NULL;
   if (picture == NULL) {
      Sprites *remaining = items;
      while (remaining != NULL) {
         if (remaining->first()->kind_of() == PICTURE) { // indirects to number
            picture = (Picture *) (remaining->first());
            break;
         };
         remaining = remaining->rest();
      };
   };
//   boolean picture_in_hand = FALSE;
   if (picture == NULL) {
      if (tt_programmer->pointer_to_tool_in_hand() != NULL &&
          tt_programmer->pointer_to_tool_in_hand()->kind_of() == PICTURE) {
         picture = (Picture *) (tt_programmer->pointer_to_tool_in_hand());
         picture_in_hand = TRUE;
      } else {
         log(S(IDS_CANT_FIND_PICTURE_FOR_PUZZLE),FALSE,TRUE);
         return;
      };
   };
   items = floor_items(items_encoding,length,reset_old,picture);
   if (!picture_in_hand) {
      Sprites *remaining = items;
      while (remaining != NULL) {
         if (remaining->first()->kind_of() == PICTURE) {
            // replace with the old one
            remaining->first()->destroy();
            remaining->set_first(picture);
            break;
         };
         remaining = remaining->rest();
      };
      floor->remove_item(picture); // so it isn't clobbered by resetting
   };
   // following should reclaim the old ones properly
   floor->set_initial_items(items);
//	just_destroy_list(items); // new on 071100 - commented out since done by update_floor_items
   // the copy has remotes of the picture on the floor
//   Background *background = items->first()->pointer_to_background();
//   background->set_initial_items(items->copy(background));
//   tt_copying_insides_for = NULL; // restore
};

void update_floor_items(SpritesPointer &items, string items_encoding, long length,
                        Background *floor, boolean reset_old) {
   switch (puzzle_counter) {
      case dusty_spit_out_puzzle: // need to save vacuum in case user saves or blows up bomb
        {
         Sprites *floor_items = floor->pointer_to_items();
         if (saved_level_stuff == NULL) { // first time so save it
            if (floor_items != NULL) {
               saved_level_stuff = floor_items->top_level_copy();
//               Sprites *remaining = room->pointer_to_items();
//               while (remaining != NULL) {
//                  Sprite *sprite = remaining->first();
//                  if (sprite->containable_by_background()) {
//                     saved_level_stuff = new Sprites(sprite->copy(),saved_level_stuff);
//                  };
//                  remaining = remaining->rest();
//               };
            }; // else warn??
         } else if (floor_items == NULL) {
            floor->add_copy_of_items(saved_level_stuff); // on next puzzle could delete saved_level_stuff ...
         };
        };
        break;
      case upper_case_alphabet_puzzle: // first use of robot reset its wand (6 done already)
         reset_robots_wands_counter(2*(alphabet_size()-6),items);
         break;
      case lower_case_alphabet_puzzle: // second use of robot reset its wand
         reset_robots_wands_counter(2*alphabet_size(),items);
         break;
      case seconds_in_hour_puzzle:
         reset_robots_wands_counter(60,items);
         break;
      case first_robot_team_puzzle:
         reset_robots_wands_counter(-1,items); // unlimited magic now
         break;
      case sum_of_frozen_numbers_puzzle:
         if (saved_level_stuff != NULL) {
//            Background *floor = puzzle_house->pointer_to_floor();
				Sprite *robot = saved_level_stuff->nth(0)->copy();
				robot->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL); // new on 030404
            floor->add_item(robot);
         } else {
            reset_to_start_of_level();
         };
         break;
      case eight_seconds_puzzle:
      case fourteen_seconds_puzzle:
         if (saved_level_stuff != NULL) {
//            Background *floor = puzzle_house->pointer_to_floor();
				Sprite *robot = saved_level_stuff->nth(0)->copy();
				robot->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL); // new on 030404
            floor->add_item(robot);
            if (saved_level_stuff->rest() != NULL) {
					Sprite *robot2 = saved_level_stuff->nth(1)->copy();
					robot2->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL); // new on 030404
               floor->add_item(robot2);
            } else {
               reset_to_start_of_level();
            };
         } else {
            reset_to_start_of_level();
         };
         break;
      case alarm_clock_team_puzzle:
         if (saved_level_stuff != NULL) { // &&
//             saved_level_stuff->rest() != NULL && saved_level_stuff->rest()->rest() != NULL) {
//            Background *floor = puzzle_house->pointer_to_floor();
				Sprite *robot = saved_level_stuff->nth(0)->copy();
				robot->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL); // new on 020404
            floor->add_item(robot);  // was nth(2)
         } else {
            reset_to_start_of_level();
         };
         break;
//      case first_timer_puzzle: // commented out on 030200 since floor was dumped with connectons intact
      case seconds_timer_puzzle:
         connect_number_and_remotes(items,items_encoding,length,floor,reset_old);
         break;
      case minutes_timer_puzzle:
          // new version saves the connection
//         connect_number_and_remotes(items,items_encoding,length,floor,reset_old);
         if (saved_level_stuff != NULL) {
            Robot *seconds_robot = (Robot *) find_robot_on_flip_side(saved_level_stuff->nth(0));
            if (seconds_robot == NULL) {
               reset_to_start_of_level();
               break;
            };
            seconds_robot = (Robot *) (seconds_robot->top_level_copy());
            seconds_robot->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL);
            seconds_robot->move_to(ideal_screen_width/2,tile_height);
            items = new Sprites(seconds_robot,items);
            floor->add_item(seconds_robot);
            if (seconds_robot->pointer_to_initial_tool() != NULL) {
                seconds_robot->pointer_to_initial_tool()->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL);
            };
         } else {
            reset_to_start_of_level();
         };
         break;
      case hours_timer_puzzle:
         connect_number_and_remotes(items,items_encoding,length,floor,reset_old);
         if (saved_level_stuff != NULL && saved_level_stuff->rest() != NULL) {
            Sprite *seconds_timer = saved_level_stuff->nth(0)->top_level_copy();
            Sprite *minutes_timer = saved_level_stuff->nth(1)->top_level_copy();
            Sprite *minutes_robot = find_robot_on_flip_side(minutes_timer);
            minutes_robot = minutes_robot->top_level_copy();
            minutes_robot->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL);
            minutes_robot->move_to(ideal_screen_width/2,tile_height);
            items = new Sprites(minutes_robot,items);
            floor->add_item(minutes_robot);
            seconds_timer->set_size_and_location(tile_height,2*tile_width,ideal_screen_width/3,ideal_screen_height/2);
            items = new Sprites(seconds_timer,items);
            floor->add_item(seconds_timer);
            minutes_timer->set_size_and_location(tile_height,2*tile_width,(2*ideal_screen_width)/3,ideal_screen_height/2);
            items = new Sprites(minutes_timer,items);
            floor->add_item(minutes_timer);
         } else {
            reset_to_start_of_level();
         };
         break;
      case modulo_seconds_timer_puzzle:
      case modulo_minutes_timer_puzzle:
      case modulo_hours_timer_puzzle:
         {
          Sprite *in_hand = tt_programmer->pointer_to_tool_in_hand();
          Sprite *in_first_hole = NULL;
          int index;
          if (puzzle_counter == modulo_seconds_timer_puzzle) {
             index = 2;
          } else if (puzzle_counter == modulo_minutes_timer_puzzle) {
             index = 1;
          } else if (puzzle_counter == modulo_hours_timer_puzzle) {
             index = 0;
          };
          if (in_hand != NULL && in_hand->kind_of() == CUBBY) in_first_hole = ((Cubby *) in_hand)->component(index);
          if (in_first_hole != NULL && in_first_hole->kind_of() == PICTURE) {
             connect_number_and_remotes(items,items_encoding,length,floor,reset_old,(Picture *) in_first_hole,TRUE);
          } else {
             reset_to_start_of_level();
          };
         };
         break;
      case seconds_until_millions:
         if (saved_level_stuff != NULL) {
            compute_age_in_seconds();
            Picture *seconds_timer = (Picture *) (saved_level_stuff->nth(0)->top_level_copy());
            Number *number = (Number *) (seconds_timer->pointer_to_indirection());
            number->set_long_value(seconds_old);
            seconds_timer->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL);
            seconds_timer->set_width(tile_width*8); // too narrow otherwise
            seconds_timer->move_to(ideal_screen_width/2,tile_height);
            items = new Sprites(seconds_timer,items);
            floor->add_item(seconds_timer);
            Number *goal = new NUMBER(ideal_screen_width/2,tile_height*5);
				goal->set_long_value(millions_of_seconds_goal);
            goal->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL);
            items = new Sprites(goal,items);
            floor->add_item(goal);
         } else {
            reset_to_start_of_level();
         };
         break;
   };
};

boolean check_nest_for_days_of_the_week(Sprite *answer, output_stream &stream) {
   // answer must be a cubby -- check that it contains a nest with 7 things
   Cubby *cubby = (Cubby *) answer;
   Sprite *contents = cubby->component(0);
   if (contents->kind_of() != NEST) {
      stream << S(IDS_PUT_NEST_IN_BOX);
      return(FALSE);
   };
   Nest *nest = (Nest *) contents;
   long length = nest->stack_size();
   if (length == 7) return(TRUE);
   stream << S(IDS_NEED_ALL_7_DAYS) << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << length << end_sentence();
   return(FALSE);
};

boolean check_nest_for_numbers(Sprite *answer, output_stream &stream, boolean must_be_different) {
   Cubby *cubby = (Cubby *) answer;
   Sprite *contents = cubby->component(0);
   if (contents->kind_of() != NEST) {
      stream << S(IDS_PUT_NEST_IN_BOX);
      return(FALSE);
   };
   Nest *nest = (Nest *) contents;
   long length = nest->stack_size();
   if (length < 5) {
      stream << S(IDS_NEED_NUMBERS_UNDERNEATH) << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << length << end_sentence();
      return(FALSE);
   };
	if (must_be_different) { // conditional new on 090103 - broke puzzle 45 - thanks Gordon
		Sprites *remaining = nest->pointer_to_contents_stack();
		int previous_value = -1;
		while (remaining != NULL) { // new on 071201 to enforce that they are different since got a bug report on 300901 that someone put 2,2,2,2,1 on the nest
			Sprite *sprite = remaining->first();
			long value;
			if (sprite->current_long_value(value)) {
				if (value == previous_value) {
					stream << S(IDS_NEED_DIFFERENT_NUMBERS_UNDERNEATH,"Sorry, but we need 2, 3, 4, 5, 6, and so on on the nest. ");
					return(FALSE);
				} else {
					previous_value = value;
				};
			} else {
				stream << S(IDS_NEED_DIFFERENT_NUMBERS_UNDERNEATH,"Sorry, but we need 2, 3, 4, 5, 6, and so on on the nest. ");
				return(FALSE);
			};
			remaining = remaining->rest();
		};
	};
   return(TRUE);
};

boolean fourteen_seconds_and_new_house(Sprite *answer, output_stream &stream) {
   if (current_house_counter() == 2) { 
      stream << S(IDS_DIDNT_BUILD_A_NEW_HOUSE);
      return(FALSE);
   } else {
      answer->current_long_value(fourteen_seconds);
      return(fourteen_seconds >= 12000); // can be off by a second or two
   };
};

int count_robots_on_flip_side(Picture *picture) {
	Picture *controlee = picture->pointer_to_controlee();
	if (controlee != NULL) {
		return(count_robots_on_flip_side(controlee));
	};
   int count = 0;
   Sprites *remaining = picture->flipped_followers();
   while (remaining != NULL) {
      if (remaining->first()->kind_of() == ROBOT) {
         count++;
      };
      remaining = remaining->rest();
   };
   return(count);
};
  
boolean robots_on_back_ok(Sprite *answer, int expected_number, output_stream &stream) {
   if (answer->kind_of() != PICTURE) return(FALSE);
   int robot_count = count_robots_on_flip_side((Picture *) answer);
   if (robot_count == expected_number) return(TRUE);
   stream << S(IDS_THE_NUMBER_OF_ROBOTS_ON_THE_BACK_SHOULD_BE) << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << expected_number << end_sentence();
   return(FALSE);
//   Robot *robot = find_robot_on_flip_side(answer);
   // could explain why if this fails.
//   return(robot != NULL); // need to re-evaluate following: && (robot->waiting_for_release() || robot->suspended()));
};

boolean is_really_blank(Sprite *answer, output_stream &) {
   return(answer->is_blank());
};

boolean special_matching(Sprite *answer, output_stream &stream) {
   // differs from good_answer in that haven't try to match
   // and this must take care of everything
   switch (puzzle_counter) {
      case alarm_clock_team_puzzle:
         return(wait_for_delivery(answer,stream));
      default:
         return(FALSE);
   };
};

long previous_value = 0;
long expected_rate = 0;
millisecond previous_integer_time = 0;
Number *previous_integer = NULL;

boolean changing_number(Sprite *answer, output_stream &stream, long rate) {
  // true if changing so that in it changes by 1 unit every rate milliseconds
  // unless the rate is zero which just means any change is OK
	if (answer == NULL) { // something wrong
		return(TRUE); // let it pass -- my fault
	};
	Number *integer_answer = (Number *) answer;
	if (puzzle_status != WAITING_TO_VERIFY_SOLUTION) { 
		if (tt_use_spoken_alternative) return(TRUE); // new 111199
      puzzle_status = WAITING_TO_VERIFY_SOLUTION;
//      answer = answer->dereference();    // commented out on 010600 since not used after this - but should it deal with integer_answer instead??
      previous_integer = integer_answer;
      previous_integer->current_long_value(previous_value);
      expected_rate = rate;
      previous_integer_time = tt_current_time;
//#if TT_DEBUG_ON
//		tt_error_file() << "Recording previous value = " << previous_value << " at " << tt_current_time << endl;
//#endif
      if (tt_martian->importance_of_last_thing_spoken() <= 4) { // condition added on 120500 since don't eliminate it if important
			tt_martian->remove_balloon();
		};
      tt_martian->give_help_after(1500); // was 1000 prior to 150600 but the second timer (puzzle #63) might not change in exactly 1000 ms.
      return(TRUE);
   };
	if (!tt_use_spoken_alternative) {
		puzzle_status = WAITING_FOR_SOLUTION;
	};
   long value;
   integer_answer->current_long_value(value);
   if (expected_rate == 0) { // 0 means some change but don't care about the rate
      if (value == previous_value) {
         stream << S(IDS_NOT_CHANGING_AT_ALL) << try_again();
      };
      return(value != previous_value);
   };
   long expected_value = previous_value+(tt_current_time-previous_integer_time)/expected_rate;
//#if TT_DEBUG_ON
//	tt_error_file() << "New value = " << value << " at " << tt_current_time << endl;
//#endif
   if (expected_value == value || expected_value+1 == value) { // added second disjunct on 150600 to deal with round off (either rounding down or up is OK)
		// by the way for the minute timer this accepts it despite only 1.5 seconds - the alternative is too boring...
		// if expected_value is also == previous_value then haven't waited long enuf (in theory)
      return(TRUE);
   };
   if (value == previous_value) {
      stream << S(IDS_NOT_CHANGING_AT_ALL) << try_again();
   } else if (value < expected_value) {
      stream << S(IDS_NOT_CHANGING_FAST_ENOUGH) << try_again();
   } else {
      stream << S(IDS_NOT_CHANGING_SLOW_ENOUGH) << try_again();
   };
   return(FALSE);
};

boolean good_answer(Sprite *answer, output_stream &stream) {
   switch (puzzle_counter) {
      case blank_number_puzzle:
         return(is_really_blank(answer,stream));
      case one_billion_puzzle:
         return(integer_is_between(answer,stream,1000000001L));
      case birth_year_puzzle:
         return(save_birth_year(answer,stream));
      case good_size_alphabet_puzzle:
      case good_size_punctuation_puzzle:
         return(good_size_box(answer,stream));
      case birth_day_puzzle:
         return(check_birth_day(answer,stream));
      case introduce_birds_puzzle:
         return(check_nest_for_days_of_the_week(answer,stream));
      case nest_of_frozen_numbers_puzzle:
      case stack_of_numbers_puzzle:
         if (check_nest_for_numbers(answer,stream,(puzzle_counter == stack_of_numbers_puzzle))) { // last arg new on 090103
            if (puzzle_counter == nest_of_frozen_numbers_puzzle) {
               add_robot_to_saved_level_stuff();
            };
            return(TRUE);
         };
         return(FALSE);
      case sum_of_stack_puzzle:
         // at least the first 5 numbers
         if (integer_is_between(answer,stream,2+3+4+5+5)) { // should be ...5+6 but some users managed to get a 5 there...
            add_robot_to_saved_level_stuff();
            return(TRUE);
         };
         return(FALSE);
      case sum_of_frozen_numbers_puzzle:
         return(integer_is_between(answer,stream,5)); // at least 5
      case mystery_sensor_puzzle:
         return(all_3_non_zero(answer,stream));
      case eight_seconds_puzzle:
         if (integer_is_between(answer,stream,7001)) { // off by a second is OK
            answer->current_long_value(eight_seconds);
            return(TRUE);
         } else {
            return(FALSE);
         };
      case fourteen_seconds_puzzle:
         return(fourteen_seconds_and_new_house(answer,stream));
      case one_robot_for_two_puzzle:
         return(integer_is_between(answer,stream,10001));
      case robot_stops_himself_puzzle:
         {   // problem with relying upon matching for a good answer is
             // that the scale might be "equal" (and the number is 3000)
         Sprite *number = ((Cubby *) answer)->component(1);
         if (integer_is_between(number,stream,3000)) {
            if (!tt_use_spoken_alternative) { // conditional added 021199
					destroy_sprites(saved_level_stuff); // only this robot is needed
				};
            add_robot_to_saved_level_stuff();
            return(TRUE);
         };
         };
         return(FALSE);
      case robot_on_back_puzzle:
         return(robots_on_back_ok(answer,1,stream) &&
                integer_is_between(answer,stream,1001,max_long,S(IDS_NUMBER_SHOULD_HAVE_GROWN_OVER_A_THOUSAND)) &&
                changing_number(answer,stream,0));
      case first_timer_puzzle:
         if (answer->kind_of() != PICTURE || ((Picture *) answer)->pointer_to_indirection() == NULL) {
            stream << S(IDS_WE_NEED_THE_NUMBER_THAT_WAS_ON_THE_FLOOR);
            return(FALSE);
         };
         // be sure robot's thought bubble was "abstracted" so it keeps running
         return(changing_number(answer,stream,0) &&
                integer_is_between(answer,stream,1001,max_long,S(IDS_NUMBER_SHOULD_HAVE_GROWN_OVER_A_THOUSAND))); // must take at least a second to get to rocket
      case seconds_timer_puzzle:
      case minutes_timer_puzzle:
         if (robots_on_back_ok(answer,1,stream) &&
             changing_number(answer,stream,puzzle_counter==seconds_timer_puzzle?1000:60000L)) { 
            if (puzzle_status != WAITING_TO_VERIFY_SOLUTION && !tt_use_spoken_alternative) {
					insert_at_end(answer->top_level_copy(),saved_level_stuff);
				};
            return(TRUE);
         } else {
            return(FALSE);
         };
       case hours_timer_puzzle:  // isn't a picture but a cubby with 3 "pictures"
         {
         Cubby *cubby = (Cubby *) answer;
         Sprite *seconds_timer = cubby->component(2);
         return(seconds_timer->kind_of() == PICTURE &&
                robots_on_back_ok(seconds_timer,1,stream) &&
                changing_number(seconds_timer,stream,1000));
          };
       case modulo_seconds_timer_puzzle:
         {
         Cubby *cubby = (Cubby *) answer;
         Sprite *seconds_timer = cubby->component(2);
         return(robots_on_back_ok(seconds_timer,2,stream) &&
                integer_is_between(seconds_timer,stream,0,59) &&
                changing_number(seconds_timer,stream,1000));
         };
       case modulo_minutes_timer_puzzle:
         {
         Cubby *cubby = (Cubby *) answer;
         Sprite *minutes_timer = cubby->component(1);
         return(robots_on_back_ok(minutes_timer,2,stream) &&
                integer_is_between(minutes_timer,stream,0,59) &&
                changing_number(minutes_timer,stream,60000L));
         };
       case modulo_hours_timer_puzzle:
         {
         Cubby *cubby = (Cubby *) answer;
         Sprite *hours_timer = cubby->component(0);
         return(robots_on_back_ok(hours_timer,2,stream) &&
                integer_is_between(hours_timer,stream,0,23) &&
                changing_number(hours_timer,stream,1000L*60*60));
         };
       case time_of_birth:
         return(save_birth_time(answer,stream));
       case seconds_until_millions:
          return(integer_is_between(answer,stream,
                                    (millions_of_seconds_goal-seconds_old)-3600, // might take an hour to compute this one
                                    millions_of_seconds_goal-seconds_old));
       case days_until_millions:
          return(respond_to_days_until_millions(answer,stream));
   };
   return(TRUE);
};

void relate_answer_to_time() {
   if (rocket_house == NULL) return; // restarting with this puzzle
   string new_intro = new character[1024];
   output_string_stream stream(new_intro,1024);
//   long eight_seconds;
//   Sprites *slots = rocket_house->pointer_to_room()->pointer_to_wall_decoration_sprite()->pointer_to_followers();
   // problem here is that the latest answer hasn't been posted yet
   // saved specially as fourteen_seconds
//   slots->nth(0)->current_value(eight_seconds);
//   slots->nth(1)->current_value(eight_seconds);
   if (eight_seconds > 0 && eight_seconds < 12000 && fourteen_seconds > 0 && fourteen_seconds < 20000) {
      // did what I expected
      stream << S(IDS_LAST_NUMBER_WAS) << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
      print_integer_with_commas(eight_seconds,stream);
      stream << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_AFTER_WAITING_8_SECONDS) << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
      print_integer_with_commas(fourteen_seconds,stream);
      stream << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_AFTER_WAITING_14_SECONDS);
      stream << intro[tt_use_spoken_alternative];
      stream.put('\0');
      delete [] intro[tt_use_spoken_alternative];
      intro[tt_use_spoken_alternative] = new_intro;
   } else { // don't say anything but something is probably wrong...
      delete [] new_intro;
   };
};

void binary_birth_year() {
	if (intro[tt_use_spoken_alternative] == NULL) return; // shouldn't really happen but better than crashing
   string new_intro = new character[1024];
   output_string_stream stream(new_intro,1024);
   int birth_year = puzzle_state.birth_year-1900; // was %100 prior to 210800 but that didn't deal with birth years of 2000 or greater
   stream << intro[tt_use_spoken_alternative] << space() << S(IDS_LIKE_INSIDE_A_COMPUTER)
          << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << birth_year << S(IDS_IS);
//	const_string separator_string = S(IDS_THOUSANDS_COMMA);
	char separator = '\0'; // changed on 060902 since commas in numbers are a mess -- separator_string[0];
	int separator_count = 3;
	if (separator == '\0' || separator == ' ') separator_count = max_short; // spaces confuses string_to_sprite
   print_binary_number(stream,birth_year,separator_count,separator,FALSE);
   stream << end_sentence() << S(IDS_SEE_IF_YOU_CAN_MAKE_THAT_NUMBER);
   stream.put('\0');
   delete [] intro[tt_use_spoken_alternative];
   intro[tt_use_spoken_alternative] = new_intro;
   wall_sprites->first()->destroy();
   string wall_string = new character[256];
   output_string_stream wall_stream(wall_string,256);
   wall_stream << S(IDS_WE_NEED1) << "\r";
   print_binary_number(wall_stream,birth_year,separator_count,separator,FALSE);
   wall_stream.put('\0');
   wall_sprites->set_first(string_to_sprite(wall_string));
   delete [] wall_string;
   if (goal_sprite != NULL) goal_sprite->destroy();
   string goal_string = new character[33];
   output_string_stream goal_stream(goal_string,33);
   print_binary_number(goal_stream,birth_year,separator_count,separator,FALSE);
   goal_stream.put('\0');
   goal_sprite = string_to_sprite(goal_string);
   delete [] goal_string;
};

long difference_in_days(int year1, int year2,
                        int month1, int month2,
                        int day1, int day2,
                        int &leap_days,
                        boolean &leap_day_this_year) {
  // come back and do leap years etc. later
  long days = (year1-year2)*365;
  if (month1 > month2) {
     for (int i = month2; i < month1; i++) {
        days += month_length[i];
     };
  } else if (month1 < month2) {
     for (int i = month1; i < month2; i++) {
        days -= month_length[i];
     };
  };
  days += day1-day2;
  leap_days = 0;
  for (int i = year2; i < year1; i++) { 
     // pretty stupid way to do this but...
     if (i%4 == 0 && i != 1900) leap_days++;
  };
  leap_day_this_year = (year2%4 == 0 && month2 > 2); // after February
  if (leap_day_this_year) leap_days++;
  return(days);
};

void inform_player_exact_age() {
   time_t timer;
   struct tm *tblock;
   timer = seconds_since_1970();
   tblock = localtime(&timer); // converts date/time to a structure
   string new_intro = new character[1024];
   output_string_stream stream(new_intro,1024);
   stream << S(IDS_THANKS) << end_sentence() << S(IDS_TODAY_IS) << space();
   print_date(tblock->tm_mon,tblock->tm_mday,(1900+tblock->tm_year),stream);
   stream << space() << S(IDS_AND_YOU_WERE_BORN) << space();
   print_date(puzzle_state.birth_month,puzzle_state.birth_day,puzzle_state.birth_year,stream);
   stream << space() << S(IDS_SO_YOU_ARE) << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
   int leap_days;
   boolean leap_day_this_year;
   long days = difference_in_days(1900+tblock->tm_year,puzzle_state.birth_year,
                                  tblock->tm_mon,puzzle_state.birth_month,
                                  tblock->tm_mday,puzzle_state.birth_day,
                                  leap_days,leap_day_this_year);
   stream << (days/365) << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_YEARS_AND)
          << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << ((days%365)+(leap_day_this_year?1:0));
   stream << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_DAYS_OLD) << space() << S(IDS_TODAY) << end_sentence() 
		    << S(IDS_OR_SENTENCE_START) << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
//   stream << days << " " << S(IDS_DAYS_OLD) << end_sentence();
	print_integer_with_commas(days,stream); // revised to generate commas on 280499
	stream << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_DAYS_OLD) << end_sentence();
   if (days%365 == 0) {
      stream << S(IDS_ITS_YOUR_BIRTHDAY);
   } else if (days%365 < 4) { // less than days after birthday
      stream << S(IDS_HAD_BIRTHDAY_RECENTLY);
   } else if (days%365 > 358) { // less than 7 days until
      stream << S(IDS_BIRTHDAY_SOON);
   };
	stream << S(IDS_SO_YOU_ARE2,"");
   stream << intro[tt_use_spoken_alternative];
   stream.put('\0');
   delete [] intro[tt_use_spoken_alternative];
   intro[tt_use_spoken_alternative] = new_intro;
};

void tell_seconds_old() {
   string new_intro = new character[1024];
   output_string_stream stream(new_intro,1024);
   // I see you are 0 seconds old.  I mean 1 seconds old.  No, it's 2 seconds old.
   // I wonder how many seconds until you are 3 seconds old. Could you please go next
   // door and figure it out for me?
   stream << S(IDS_AGE_IN_SECONDS1) << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
   print_integer_with_commas(seconds_old,stream);
   stream << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_AGE_IN_SECONDS2) << space() 
		    << S(IDS_AGE_IN_SECONDS3) << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
   seconds_old += 3;
   print_integer_with_commas(seconds_old,stream);
   stream << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_AGE_IN_SECONDS2) 
		    << space() << S(IDS_AGE_IN_SECONDS4) << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
   seconds_old += 5;
   print_integer_with_commas(seconds_old,stream);
   stream << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_AGE_IN_SECONDS2) << space() 
		    << S(IDS_AGE_IN_SECONDS5) << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
   print_integer_with_commas(millions_of_seconds_goal,stream);
   stream << end_sentence('?') << S(IDS_AGE_IN_SECONDS6);
   stream.put('\0');
   delete [] intro[tt_use_spoken_alternative];
   intro[tt_use_spoken_alternative] = new_intro;
};

Picture *players_age_in_seconds() {
   if (puzzle_state.counter < last_puzzle) return(NULL); // haven't earned it yet
   compute_age_in_seconds();
	const int size_of_encoding_string = 9*72+1; // just enough room
   character encoding_string[size_of_encoding_string];
   output_string_stream encoding(encoding_string,size_of_encoding_string);
   for (int i = 0; i < 9; i++) {
      encoding << S(IDS_SECONDS_TIMER0+i);
   };
   encoding.put((char) 0);
   Picture *seconds_timer = (Picture *) uudecode_sprite(encoding_string);
   Number *number = (Number *) (seconds_timer->pointer_to_indirection());
   number->set_long_value(seconds_old);
   seconds_timer->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL);
   seconds_timer->set_width(tile_width*8); // too narrow otherwise
   return(seconds_timer);
};

void compute_age_in_seconds() {
   time_t timer;
   struct tm *tblock;
   timer = seconds_since_1970();
   tblock = localtime(&timer); // converts date/time to a structure
   int leap_days;
   boolean leap_day_this_year;
   long days = difference_in_days(1900+tblock->tm_year,puzzle_state.birth_year,
                                  tblock->tm_mon,puzzle_state.birth_month,
                                  tblock->tm_mday,puzzle_state.birth_day,
                                  leap_days,leap_day_this_year);
   while (days >= 24844) { // after about 68 years old we run into overflow problems -- could revisit this now that there are bignums...
      days -= 1000000000L/seconds_in_a_day; // remove a billion
      tt_error_file() << "Player's age in seconds is too big for 32 bit integers.  Removing a billion seconds from the age." << endl;
   };
   seconds_old = days*seconds_in_a_day;
   seconds_old += (tblock->tm_hour-puzzle_state.birth_hour)*60*60;
   seconds_old += (tblock->tm_min-puzzle_state.birth_minute)*60;
   seconds_old += (tblock->tm_sec-puzzle_state.birth_second);
   millions_of_seconds_goal = ((seconds_old/1000000L)+1)*1000000L;
   if (millions_of_seconds_goal-seconds_old < 3600) { // so soon that it may go negative in less than an hour
      millions_of_seconds_goal += 1000000L;
   };
};

void update_loaded_puzzle1() {
   switch (puzzle_counter) {
      case binary_birth_year_puzzle:
         binary_birth_year();
         break;
      case after_birth_day_puzzle:
         inform_player_exact_age();
         break;
      case bombs_introduced_puzzle:
         relate_answer_to_time();
			house_count_at_start_of_bombs_introduced_puzzle = current_house_counter(); // new on 260301
         break;
      case seconds_until_millions:
         tell_seconds_old();
         break;
   };
};

void update_loaded_puzzle() { // fixed to work twice on 041199
	if (alternative_language_spoken()) {
		tt_use_spoken_alternative = TRUE;
		update_loaded_puzzle1();
		tt_use_spoken_alternative = FALSE;
	};
	update_loaded_puzzle1();
};


boolean starting_with_first_tutorial_puzzle() {
   return(puzzle_counter == 1);
};

void repeat_last_thing(output_stream &stream) {
   if (_tcsncmp(S(IDS_REPEAT1),Marty_was_saying[tt_use_spoken_alternative],_tcslen(S(IDS_REPEAT1))) != 0 &&
       _tcsncmp(S(IDS_REPEAT2),Marty_was_saying[tt_use_spoken_alternative],_tcslen(S(IDS_REPEAT2))) != 0 &&
       _tcsncmp(S(IDS_REPEAT3),Marty_was_saying[tt_use_spoken_alternative],_tcslen(S(IDS_REPEAT3))) != 0) {
     // not a user inititiated repeat that had "..." last time 
      const_string repeat_intro = S(IDS_AS_I_WAS_SAYING);
      if (_tcsncmp(repeat_intro,Marty_was_saying[tt_use_spoken_alternative],_tcslen(repeat_intro)) == 0) {
         // already repeated
         stream << Marty_was_saying[tt_use_spoken_alternative];
      } else {
         if (!explained_dots && tt_marty_talk != MARTY_SPEAKS && tt_marty_talk != MARTY_SPEAKS_WITH_SUBTITLES) { 
				// without talk balloons
            stream << S(IDS_WHAT_DOTS_MEAN) << space();
            explained_dots = TRUE;
         };
         stream << repeat_intro << Marty_was_saying[tt_use_spoken_alternative];  // << Marty_was_saying was commented out -- why???
         if (_tcsstr(Marty_was_saying[tt_use_spoken_alternative],S(IDS_THATS_WHAT_I_WANTED_SAY)) == NULL) { // not already saying this
            stream << space() << S(IDS_THATS_WHAT_I_WANTED_SAY);
         };
      };
   };
   delete [] Marty_was_saying[tt_use_spoken_alternative];
   Marty_was_saying[tt_use_spoken_alternative] = NULL;
};

MatchStatus check_conditions(output_stream &stream, boolean &say_more) {
	Cubby *message = new Cubby();
	message->set_number_of_holes(2);
	message->initialize_hole(0,answer_sprite->copy());
	Bird *bird;
	Nest *nest;
	create_bird_nest_pair(bird,nest);
	message->initialize_hole(1,bird);
//	Picture *temp = new Picture();
//	Background *temp_background = temp->pointer_to_backside();
//	temp->flip();
//	add_flipped_follower(message,NULL,temp);
//	add_flipped_follower(conditions_as_robots->copy(),message,temp);
//	temp->unflip();
//	temp->activate();
	Background *background = new Background();
	Robot *conditions_as_robots_copy = (Robot *) conditions_as_robots->copy();
	background->add_item(conditions_as_robots_copy);
	background->add_item(message);
//	background->add_item(bird); // already in message
	background->add_item(nest);
	UnwindProtect<Queue*> set(tt_queue,NULL);
	conditions_as_robots_copy->try_clause(message);
	if (tt_queue == NULL) {
		// robot didn't match
		background->destroy();
		return(MATCH_BAD); // can I / should I distinguish with SUSPENSION?
	};
	boolean toolbox_is_new = FALSE;
	if (tt_queue != NULL && tt_toolbox == NULL) {
		// needed to run some kinds of robots
		tt_system_mode = FREE_PLAY;
		tt_toolbox = new Toolbox();
		tt_toolbox->initialize();
		toolbox_is_new = TRUE;
   };
	// does the following really work right? And doesn't the use of timeGetTime interfere with logging? (and time travel)
	unsigned long stop_time = timeGetTime()+3000; // 3 seconds max
	while (tt_queue != NULL && timeGetTime() < stop_time) {
		run_queue();
	};
	tt_system_mode = PUZZLE; // restore in case reset above to get "normal" behavior
	Sprite *answer = nest->dereference();
	MatchStatus result = MATCH_BAD; // default
	if (answer->kind_of() == CUBBY) {
		Cubby *answer_cubby = (Cubby *) answer;
		if (answer_cubby->current_number_of_holes() == 2) {
			Sprite *first = answer_cubby->component(0);
			// do I need to dereference this?
			if (first != NULL && first->kind_of() == INTEGER) {
				long value;
				if (first->current_long_value(value)) {
					if (value != 0) {
						result = MATCH_GOOD;
					};
				};
			};
			Sprite *second = answer_cubby->component(1);
			if (second != NULL && second->kind_of() == TEXT) {
				Text *text = (Text *) second;
				string text_string;
				long length;
				if (text->current_text(text_string,length)) {
					if (length > 0) {
						text_string[length] = '\0';
						stream << text_string << space();
						say_more = FALSE;
					};
				};
			};
		};
	};
	background->destroy();
	if (toolbox_is_new) {
		tt_toolbox->destroy();
		tt_toolbox = NULL;
		release_toolbox_tools();
	};
	return(result);
};

flag bomb_explained = FALSE;
flag extra_puzzle_1_help_given = FALSE;
flag how_to_get_hints_explained = FALSE; // new on 270100

string return_response_to_solution() {
	answer_sprite = tt_programmer->pointer_to_tool_in_hand();
	boolean answer_same_as_previous = (answer_sprite == previous_answer_sprite); // moved here on 270100 since it becomes TRUE later
   string buffer = new character[2048];
   output_string_stream stream(buffer,2048);
   boolean reset_puzzle = FALSE;
   boolean hint_given = FALSE;
   // save following since may get reset by reloading next puzzle
   boolean post_solution_enabled = switches[SOLUTION_POSTED_ENABLED];
   boolean special_handling = special_matching(answer_sprite,stream);
   if (// !special_handling && answer_sprite == NULL &&
       puzzle_counter == bombs_introduced_puzzle) {
      // supposed to come back empty handed but why worry about it
      if (current_house_counter() == house_count_at_start_of_bombs_introduced_puzzle-1) { // rocket and puzzle house (and as of 260301 any houses built and then puzzle restarted)
         post_solution_enabled = FALSE;
         if (!tt_use_spoken_alternative) next_puzzle_default();
      } else {
         if (!answer_same_as_previous) {
            stream << S(IDS_SOMETHING_WRONG) << try_again();
         };
         if (!tt_use_spoken_alternative) {
				previous_answer_sprite = answer_sprite;
			};
			reset_puzzle = TRUE;
      };
   } else if (!special_handling && answer_sprite == NULL) {
      if (!tt_use_spoken_alternative) empty_counter++;
      if (Marty_was_saying[tt_use_spoken_alternative] != NULL) repeat_last_thing(stream);
      if (puzzle_counter == 1) { // first one
         if (!extra_puzzle_1_help_given) {
            stream << S(IDS_SIT_DOWN_AND_GET) << space();
            goal_sprite->describe_type(stream,DEFINITE_ARTICLE);
            stream << space() << S(IDS_WE_NEED2) << end_sentence();
         };
         if (empty_counter > 1) {
            stream << S(IDS_BASIC_HINT1);
            stream << S(IDS_BASIC_HINT2);
         };
         if (!extra_puzzle_1_help_given) {
            if (!tt_use_spoken_alternative) extra_puzzle_1_help_given = TRUE;
            stream << S(IDS_WHILE_HOLDING) << space();
            goal_sprite->describe_type(stream,DEFINITE_ARTICLE);
            stream << space() << S(IDS_PRESS_ESC_AND_COME_BACK);
            hint_given = TRUE;
         };
      };
      reset_puzzle = TRUE;
   } else if (!special_handling && answer_sprite != NULL && goal_sprite != NULL) {
//      Sprite *answer_sprite = answer_sprite->dereference();
      SpriteType answer_type = answer_sprite->dereference()->kind_of();
      SpriteType goal_type = goal_sprite->kind_of();
      if (answer_type != goal_type) {
         if (!answer_same_as_previous) {
            // didn't just walk back with wrong thing
            stream << S(IDS_WE_DONT_NEED,"") << space();
				const int max_buffer_size = 2000;
				string buffer = new char[max_buffer_size]; // new on 160600
				output_string_stream out1(buffer,max_buffer_size);
            answer_sprite->describe_type(out1,INDEFINITE_ARTICLE);
				out1.put('\0'); // terminate it
				if (tt_language == BRAZILIAN_PORTUGUESE) { // new on 160600
					if (buffer[0] == 'o' || buffer[0] == 'a') {
						stream << "d" << buffer;
					} else {
						stream << "de " << buffer;
					};
				} else {
					stream << buffer;
				};
            stream << end_sentence() << S(IDS_WE_NEED1) << space();
				output_string_stream out2(buffer,max_buffer_size);
            if (goal_sprite->is_blank() &&
                puzzle_counter != blank_number_puzzle) {
               // probably other constraints on it
               // don't want a blank number though...
               goal_sprite->describe_type(out2,INDEFINITE_ARTICLE);
            } else {
               goal_sprite->describe(out2,INDEFINITE_ARTICLE);
            };
				out2.put('\0'); // terminate it
				if (tt_language == BRAZILIAN_PORTUGUESE) { // new on 160600
					if (buffer[0] == 'o' || buffer[0] == 'a') {
						stream << "d" << buffer;
					} else {
						stream << "de " << buffer;
					};
				} else {
					stream << buffer;
				};
				delete [] buffer;
				stream << S(IDS_WE_DONT_NEED2,"");
            stream << end_sentence();
            if (!tt_use_spoken_alternative) previous_answer_sprite = answer_sprite;
         };
         reset_puzzle = TRUE;
      } else {
         MatchStatus match_status;
			boolean say_more = TRUE;
			if (conditions_as_robots != NULL) {
				match_status = check_conditions(stream,say_more);
			} else {
				match_status = match(goal_sprite,answer_sprite->dereference(),default_robot_version); // dereference added 3/31 for p61
         };
			if (match_status == MATCH_GOOD &&
             !switches[USER_PUZZLE_ENABLED] && // system puzzle
             !good_answer(answer_sprite,stream)) {
            if (Marty_was_saying[tt_use_spoken_alternative] != NULL) repeat_last_thing(stream);
            if (!tt_use_spoken_alternative) previous_answer_sprite = answer_sprite;
            reset_puzzle = TRUE;
         } else {
            switch (match_status) {
               case MATCH_GOOD:
						if (puzzle_status != WAITING_TO_VERIFY_SOLUTION && !tt_use_spoken_alternative) next_puzzle_default();
						break;
               case MATCH_BAD:
						if (say_more) {
							if (Marty_was_saying[tt_use_spoken_alternative] != NULL) repeat_last_thing(stream);
							if (!answer_same_as_previous) {
								stream << S(IDS_NOT_IT1+my_random_unless_tt_use_spoken_alternative(3)) << space();
								if (answer_type == CUBBY) {
									if (!order_matters_explained &&
										 ((Cubby *) answer_sprite)->current_number_of_holes() ==
										 ((Cubby *) goal_sprite)->current_number_of_holes() &&
                               ((Cubby *) answer_sprite)->current_number_of_holes() > 1) { // no point saying this if there is just one hole - new on 091102
										stream << S(IDS_ORDER_OF_THINGS_MATTER);
										if (!tt_use_spoken_alternative) order_matters_explained = TRUE; // for this session
									};
								};
							};
                     if (!tt_use_spoken_alternative) previous_answer_sprite = answer_sprite;
                  };
                  reset_puzzle = TRUE;
                  break;
               case MATCH_SUSPENDED_ON_EMPTY_CUBBY_HOLE:
                  if (Marty_was_saying[tt_use_spoken_alternative] != NULL) repeat_last_thing(stream);
                  if (answer_sprite != previous_answer_sprite) {
                     stream << S(IDS_SOMETHING_IS_MISSING); // << space();
//                     goal_sprite->describe_type(stream,DEFINITE_ARTICLE);
                     stream << S(IDS_SOMETHING_IS_MISSING2) << try_again();
                     if (!tt_use_spoken_alternative) previous_answer_sprite = answer_sprite;
                  };
                  reset_puzzle = TRUE;
                  break;
               default:
                  stream << S(IDS_SOMETHING_WRONG) << try_again();
                  reset_puzzle = TRUE;
            };
         };
      };
   };
   if (puzzle_status == WAITED_TOO_LONG_TO_VERIFY_SOLUTION) {
		// conditional below added on 161099
      if (!tt_use_spoken_alternative) puzzle_status = WAITING_FOR_SOLUTION; // become normal again but don't do the following
   } else if (puzzle_status != WAITING_TO_VERIFY_SOLUTION) {
      if (!tt_use_spoken_alternative) post_solution(!reset_puzzle,!post_solution_enabled,TRUE);
      if (!tt_use_spoken_alternative) user_did_good = !reset_puzzle; // conditional added 161099
      if (reset_puzzle) {
         if (!hint_given) {
            boolean bomb_hint_needs_to_given = (!bomb_explained && !bomb_ever_used());
            if (answer_sprite != NULL && answer_sprite->kind_of() == BOMB && puzzle_counter != bombs_introduced_puzzle) {
               stream << S(IDS_BOMB_TO_RESTART);
               if (!tt_use_spoken_alternative) bomb_explained = TRUE;
            } else if (// !tt_bomb_to_reset_puzzle ||
                       answer_sprite == NULL ||
                       (answer_same_as_previous && !bomb_hint_needs_to_given)) {
               // returned empty handed or no reset bombs available
               // or came back again without sitting down
               // if second time with same thing offer bomb hint now if needed
               give_hint(stream);
            } else if (!did_user(USER_HAS_ASKED_FOR_PUZZLE_HINT) || !how_to_get_hints_explained) {
               stream << S(IDS_COME_HERE_FOR_HINTS);
					if (!tt_use_spoken_alternative) how_to_get_hints_explained = TRUE;
            } else if (bomb_hint_needs_to_given) {
               stream << S(IDS_BOMB_TO_RESTART);
               if (!tt_use_spoken_alternative) bomb_explained = TRUE;
            };
         };
//         if (!tt_bomb_to_reset_puzzle && !tt_use_spoken_alternative) reload_puzzle();
      };
   }
   stream.put('\0');
#if TT_UNICODE
	if (stream.str().length() > 1 ||
#else
   if (stream.pcount() > 1 || 
#endif
		puzzle_status != WAITING_TO_VERIFY_SOLUTION) {
      if (!tt_use_spoken_alternative && tt_martian->importance_of_last_thing_spoken() <= 4) { // second condition added 120400
			tt_martian->remove_balloon();  // ok to repeat himself
		};
   };
   return(buffer);
};

void respond_to_solution() {
	string alternative_spoken_buffer = NULL;
	if (alternative_language_spoken()) {
		tt_use_spoken_alternative = TRUE;
		alternative_spoken_buffer = return_response_to_solution();
		tt_use_spoken_alternative = FALSE;
	};
	string buffer = return_response_to_solution();
	tt_martian->say(buffer,importance,alternative_spoken_buffer); // was importance++
	delete [] buffer;
	if (alternative_spoken_buffer != NULL) delete [] alternative_spoken_buffer;
};

void release_puzzle_data(boolean for_good) {
	for (int i = 0; i < 2; i++) {
		if (intro[i] != NULL) {
			delete [] intro[i];
			intro[i] = NULL;
		};
	};
//   following isn't needed since house will destroy decoration when it goes
//   if (things_in_house_too && wall_sprites != NULL) {
//      destroy_sprites(wall_sprites);
//   };
   if (goal_sprite != NULL) {
      goal_sprite->destroy();
      goal_sprite = NULL;
   };
	if (conditions_as_robots != NULL) {
		conditions_as_robots->destroy();
		conditions_as_robots = NULL;
	};
	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < hints_available[j]; i++) {
			delete [] hints[j][i];
			hints[j][i] = NULL;
		};
		hints_available[j] = 0;
	};
   dusty = NULL; // should be destroyed by floor
   destroy_sprites(initial_rocket_items);
//   if (wall_postfix != NULL) delete [] wall_postfix;
//   if (congrats != NULL) {
//      delete [] congrats;
//      congrats = NULL;
//   };
   if (for_good) {
      destroy_sprites(saved_level_stuff);
      destroy_sprites(saved_floor_items);
//      destroy_sprites(previous_saved_floor_items); // commented out on 040201 since caused a crash and leaks on exiting aren't important
		destroy_sprites(saved_items_from_non_reset_floor);
		if (saved_tool_in_hand_non_reset != NULL) saved_tool_in_hand_non_reset->destroy();
		if (saved_item_in_hand != NULL) saved_item_in_hand->destroy(); // new on 071100
      if (puzzle_state_encoding != NULL) {
         delete [] puzzle_state_encoding;
			puzzle_state_encoding = NULL;
      };
      if (switches != NULL) {
			delete [] switches;
			switches = NULL;
		};
		for (int j = 0; j < 2; j++) {
			if (hints[j] != NULL) delete [] hints[j];
		};
		if (tt_alternative_spoken_puzzle_file_name != NULL) {
			delete [] tt_alternative_spoken_puzzle_file_name;
			tt_alternative_spoken_puzzle_file_name = NULL;
		};
   };
	if (Marty_was_saying[0] != NULL) { // private copy - prior to 240101 was just checking and deleting tt_use_spoken_alternative
		delete [] Marty_was_saying[0];
		Marty_was_saying[0] = NULL;
	};
	if (Marty_was_saying[1] != NULL) { // private copy - prior to 240101 was just checking and deleting tt_use_spoken_alternative
		delete [] Marty_was_saying[1];
		Marty_was_saying[1] = NULL;
	};
};

void quiting_puzzles() {
	// this was pulled out of release_puzzle_data on 111199 so that it happens before tt_error_file() is closed
	// replaced the following with a new arg to trouble_shoot
//	boolean saved_tt_trouble_shooting = tt_trouble_shooting; // still has check for showing log
   if (tt_system_mode == PUZZLE && replay_file_name() == NULL) { // second condition new on 071100 (too late for tt_replay since replay is over)
      tt_error_file() << "Started with puzzle #" << first_puzzle_number << " and ended on puzzle #" << puzzle_counter << endl;
      if (tt_puzzles_over_html_file == NULL && // condition added 231000 so don't get offer of hints if puzzles terminated by HTML file
			 (puzzle_counter == 1 && first_puzzle_number < 2)  // second conjunct added on 200800 so that once you finish the puzzles and then quit you don't get an offer of hints
			  // quitting without solving even the first puzzle
			  || (hint_counter < 2 && original_puzzle_counter == 1 && puzzle_counter < 17 && first_puzzle_number < 17)) {
			// added second conjunct on 210999 so get explanation of hints if got less than 2 after starting at the beginning
//          tt_trouble_shooting = TRUE; // since this is for "novices"
          pause_toontalk(FALSE,FALSE,TRUE); // since trouble_shoot won't do it since restore_toontalk_afterwards is false
          trouble_shoot(TROUBLE_SHOOT_QUIT_ON_FIRST_PUZZLE,NULL,NULL,FALSE,TRUE,TRUE);
       } else if (puzzle_counter == first_robot_puzzle || puzzle_counter == second_robot_puzzle) {
//          tt_trouble_shooting = TRUE; // since this is for "novices"
          pause_toontalk(FALSE,FALSE,TRUE);
          trouble_shoot(TROUBLE_SHOOT_QUIT_ON_EARLY_ROBOT_PUZZLE,NULL,NULL,FALSE,TRUE,TRUE);
       };
   };
//	tt_trouble_shooting = saved_tt_trouble_shooting;
};

boolean ok_to_create_remote_notebook() {
  return(FALSE); // for now
};

void resume_puzzles() {
   if (puzzles_resumed) return; // new on 210799 since can be called twice now by log and command line
	if (puzzle_state.counter < 1) { //  || puzzle_state.counter > last_puzzle // second condition added on 081200 so you can start over again -- better to ask via messge box
		puzzle_state.counter = 1; 
	};
   tt_system_mode = PUZZLE;
   if (puzzle_state.counter >= level_start[last_level-1]+level_count[last_level-1]) return; // all puzzles done, deal with this later
   char short_name[32];
   output_string_stream name(short_name,32);
   name << 'p' << puzzle_state.counter;
   name.put('\0');
	if (tt_puzzle_file_name != NULL) delete [] tt_puzzle_file_name;
   FileNameStatus file_status;
	// if URL don't cache now but just before reading file
   tt_puzzle_file_name = existing_file_name(file_status,short_name,"pzl","Puzzles",FALSE,TRUE,TRUE);
//	tt_built_in_puzzles = TRUE; // new on 120401 so know when restarting is OK -- wasn't needed removed the same day
	if (alternative_language_spoken()) {
		tt_use_spoken_alternative = TRUE;
		tt_alternative_spoken_puzzle_file_name = existing_file_name(file_status,short_name,"pzl","Puzzles",TRUE,TRUE,TRUE);
		tt_use_spoken_alternative = FALSE;
	};
   puzzles_resumed = TRUE;
};

void read_puzzle_state(InputStream *file, PuzzleState &puzzle_state) {
   file->read((string) &puzzle_state.counter, sizeof(short int));
   file->read((string) &puzzle_state.birth_year, sizeof(short int));
   file->read((string) &puzzle_state.birth_month, 1);
   file->read((string) &puzzle_state.birth_day, 1);
   file->read((string) &puzzle_state.birth_hour, 1);
   file->read((string) &puzzle_state.birth_minute, 1);
   file->read((string) &puzzle_state.birth_second, 1);
};

void write_puzzle_state(output_stream &file, PuzzleState &puzzle_state) {
   file.write((character *) &puzzle_state.counter, sizeof(short int));
   file.write((character *) &puzzle_state.birth_year, sizeof(short int));
   file.write((character *) &puzzle_state.birth_month, 1);
   file.write((character *) &puzzle_state.birth_day, 1);
   file.write((character *) &puzzle_state.birth_hour, 1);
   file.write((character *) &puzzle_state.birth_minute, 1);
   file.write((character *) &puzzle_state.birth_second, 1);
};

void really_save_puzzle_state(output_stream &history_file) {
  last_puzzle_really_saved = puzzle_counter;
  Room *room = NULL;
  Sprites *decoration_to_save;
  boolean decoration_to_save_copied = FALSE;
  if (rocket_house != NULL) {
     room = rocket_house->pointer_to_room();
     Sprite *wall_sprite = room->pointer_to_wall_decoration_sprite();
     if (wall_sprite == NULL || wall_sprite->pointer_to_followers() == NULL) {
        decoration_to_save = NULL;
     } else {  // need to make a copy because the followers dump relative to their leader...
        decoration_to_save = wall_sprite->pointer_to_followers()->top_level_copy_each(); // was just top_level_copy prior to 060600 - does this matter?
        decoration_to_save_copied = TRUE; // so memory is reclaimed
     };
  } else {
     decoration_to_save = initial_rocket_items;
  };
//  saved_floor_items makes this obsolete now
/*
  switch (puzzle_counter) {
   // is this just the list of those with either
   // ResetOnSolution" and "GoodSolutionsPosted"
      case lower_case_alphabet_puzzle:
      case dusty_spit_out_puzzle: // should this be the one before
      case seconds_in_hour_puzzle:
      case stack_of_numbers_puzzle:
      case sum_of_frozen_numbers_puzzle:
//      case fourteen_seconds_puzzle:
      case fourteen_seconds_puzzle:
         puzzle_state.counter = (short int) (puzzle_counter-1);
         if (decoration_to_save != NULL && rocket_house != NULL) {
            // relies on the order of add_decoration being most recent first
            decoration_to_save = decoration_to_save->rest();
         };
         break;
      default:
*/
         puzzle_state.counter = (short int) puzzle_counter;
         if (puzzle_counter == bombs_introduced_puzzle) puzzle_state.counter--; // can't save here since extra house isn't saved
//  };
  write_puzzle_state(history_file, puzzle_state);
//  history_file.write((unsigned char *) &puzzle_state,sizeof(puzzle_state));
  if (decoration_to_save != NULL) {
     tt_dumping_background = room;
	  if (tt_dump_as_xml) {
		  decoration_to_save->xml_to_stream(history_file,L"RocketWallDecoration");
	  } else {
		  dump_started();
		  decoration_to_save->dump(history_file,TRUE);
		  dump_ended(); 
	  };
     tt_dumping_background = NULL;
     if (decoration_to_save_copied) {
		  destroy_sprites(decoration_to_save);
	  };
  } else if (tt_dump_as_xml) {
	  xml_to_stream(NULL,history_file);
  };
  if (!tt_dump_as_xml) {
	  history_file.put((unsigned char) END_OF_LIST_MARKER);
  };
  if (saved_level_stuff != NULL) {
     tt_dumping_background = room;
	  if (tt_dump_as_xml) {
		  saved_level_stuff->xml_to_stream(history_file,L"LevelItems");
	  } else {
		  dump_started();
		  saved_level_stuff->dump(history_file,TRUE);
		  dump_ended(); 
	  };
     tt_dumping_background = NULL;
  };
  if (tt_dump_as_xml) {
	  if (saved_level_stuff == NULL) {
		  xml_to_stream(NULL,history_file);
	  };
  } else {
	  history_file.put((unsigned char) END_OF_LIST_MARKER);
  };
  boolean floor_items_saved = FALSE;
  if (puzzle_house != NULL) { // save floor items as saved_floor_items
     Background *floor = puzzle_house->pointer_to_floor();  // what if they are in the room but not the floor???
     if (floor != NULL) {
        Sprites *floor_items = floor->pointer_to_items();
        if (floor_items != NULL) {
           tt_dumping_background = floor;
			  if (tt_dump_as_xml) {
			     floor_items->xml_to_stream(history_file,L"FloorItems");
			  } else {
				  dump_started();
				  floor_items->dump(history_file,TRUE);
				  dump_ended(); 
			  };
			  floor_items_saved = TRUE;
           tt_dumping_background = NULL;
        };
     };
  } else if (saved_floor_items != NULL) {
  	  if (tt_dump_as_xml) {
		  saved_floor_items->xml_to_stream(history_file,L"FloorItems");
	  } else {
		  dump_started();
		  saved_floor_items->dump(history_file,TRUE);
		  dump_ended(); 
	  };
	  floor_items_saved = TRUE;
  };
  if (tt_dump_as_xml) {
	  if (!floor_items_saved) {
		  xml_to_stream(NULL,history_file);
	  };
  } else {
	  history_file.put((unsigned char) END_OF_LIST_MARKER);
  };
  if (saved_item_in_hand != NULL) {
	  if (tt_dump_as_xml) {
		  saved_item_in_hand->xml_to_stream(history_file); 
	  } else {
		  dump_started(); // new on 071100
		  saved_item_in_hand->dump(history_file);
		  dump_ended(); // new on 071100
	  };
  } else if (tt_system_mode == PUZZLE && tt_programmer != NULL && // added NULL check on 071299 since can dump before objects initialized
             tt_programmer->pointer_to_tool_in_hand() != NULL) {
 	  if (tt_dump_as_xml) {
		  tt_programmer->pointer_to_tool_in_hand()->xml_to_stream(history_file); // prior to 250903 used saved_item_in_hand but that can be NULL
	  } else {
		  dump_started();
		  tt_programmer->pointer_to_tool_in_hand()->dump(history_file);
		  dump_ended(); //release_sprites_being_dumped();
	  };
  } else if (tt_dump_as_xml) {
	  xml_to_stream(NULL,history_file);
  } else {
     history_file.put((unsigned char) EMPTY_PAGE_MARKER);
  };
  if (saved_robots != NULL) {
	  if (tt_dump_as_xml) {
		  saved_robots->xml_to_stream(history_file,L"Robots");
	  } else {
		  dump_started();
		  saved_robots->dump(history_file,TRUE);
		  dump_ended(); 
	  };
  };
  if (tt_dump_as_xml) {
	  if (saved_robots == NULL) {
		  xml_to_stream(NULL,history_file);
	  };
  } else {
	  history_file.put((unsigned char) END_OF_LIST_MARKER);
  };
};

void save_puzzle_state(output_stream &history_file, boolean puzzle_state_changed) {
  if (puzzle_status == ALL_PUZZLES_SOLVED) { // new on 120700
	  reset_level();
  };
  if (!last_puzzle_reset_old && puzzle_counter != dusty_spit_out_puzzle) {  // since dusty_spit_out_puzzle saves state OK
     puzzle_state_changed = FALSE; // depends upon previous state
     if (puzzle_state_encoding == NULL && last_puzzle_really_saved < puzzle_counter-1 &&
		   original_puzzle_counter == puzzle_counter) { // added 161199 since sometimes this is triggered after first puzzle
		  // didn't save previous state though
        reset_to_start_of_level();
     };
  };
  if (!puzzle_state_changed && puzzle_state_encoding != NULL) {
     history_file.write((character *) puzzle_state_encoding,puzzle_state_length);
     return;
  };
  if (!puzzle_state_in_memory) {
     really_save_puzzle_state(history_file);
     return;
  };
  if (puzzle_state_encoding == NULL) {
     puzzle_state_encoding = new unsigned char[puzzle_state_max_length];
  };
  output_string_stream puzzle_state_stream((character *) puzzle_state_encoding,
														 puzzle_state_max_length);
  really_save_puzzle_state(puzzle_state_stream);
//#if TT_32
  puzzle_state_length = puzzle_state_stream.LENGTH;
//#else
// puzzle_state_length = puzzle_state_stream.pcount();
//#endif
  if (puzzle_state_length >= puzzle_state_max_length) { // turn off caching
     puzzle_state_in_memory = FALSE;
     delete [] puzzle_state_encoding;
     puzzle_state_encoding = NULL;
     really_save_puzzle_state(history_file);
     return;
  };
  history_file.write((character *) puzzle_state_encoding,puzzle_state_length);
};

boolean ok_to_save_puzzle_state() { // only called when puzzle state unchanged
   return(puzzle_state_in_memory);
};

int size_of_puzzle_state = -1; // sizeof is compiler dependent ...

void read_puzzle_state(InputStream *history_file, boolean called_from_log) {
   // USR's created by version 4C have only 2 bytes -- the puzzle count here
//   history_file.read((unsigned char *) &puzzle_state.counter,sizeof(puzzle_state.counter));
//   if (history_file != NULL) {
//      history_file.read((sizeof(puzzle_state.counter)+(unsigned char *) &puzzle_state),
//                        sizeof(puzzle_state)-sizeof(puzzle_state.counter));
//   };
//	if (tt_system_mode != PUZZLE) // commented out the condition on 080402 since first time it should be FALSE and this was called before knowing if running puzzles are not (typically)
   puzzle_state_in_memory = FALSE; // new on 210799 so short demos aren't broken
   if (puzzle_state_encoding == NULL && puzzle_state_in_memory) {
      puzzle_state_encoding = new unsigned char[puzzle_state_max_length];
   };
   if (tt_temporary_user && !puzzle_state_in_memory && !called_from_log) return; //  && !called_from_log added 040300
//   puzzle_state_length = sizeof(PuzzleState);
//   history_file.read((unsigned char *) &puzzle_state,puzzle_state_length);
   read_puzzle_state(history_file,puzzle_state);
   if (puzzle_state_in_memory) {
//      memcpy(puzzle_state_encoding,(unsigned char *) &puzzle_state,puzzle_state_length);
	   output_string_stream stream((string) puzzle_state_encoding,puzzle_state_max_length);
	   write_puzzle_state(stream,puzzle_state);
#if TT_UNICODE
	   puzzle_state_length = stream.str().length();
#else
	   puzzle_state_length = stream.pcount();
#endif
	   size_of_puzzle_state = puzzle_state_length;
   };
   puzzle_counter = puzzle_state.counter;
   original_puzzle_counter = puzzle_counter;
};

long resume_read_puzzle_state(InputStream *history_file, int version_number, boolean called_from_log, int xml_version) {
   if (tt_temporary_user && !puzzle_state_in_memory && !called_from_log) return(0); //  && !called_from_log added 040300
   long start;
   if (puzzle_state_in_memory) {
      // first read entire state and then interpret it
      start = history_file->tellg(); // WARNING - VC++ 6.0 seems to have a tellg() bug
      while (!history_file->empty() && puzzle_state_length < puzzle_state_max_length) {
         puzzle_state_encoding[puzzle_state_length++] = (unsigned char) history_file->get();
      };
      if (puzzle_state_length >= puzzle_state_max_length || history_file == NULL) {
			// second disjunct added 050600 to deal with demos shorter than puzzle_state_max_length
         puzzle_state_in_memory = FALSE;
         delete [] puzzle_state_encoding;
         puzzle_state_encoding = NULL;
			if (history_file->empty()) { // new on 050600
				return(start);
			};
         history_file->seekg(start);
      };
   };
	// commented out the following on 080500 since breaks reading of favorites and history to skip the following
//   if (!puzzles_resumed && !replaying() && tt_system_mode == PUZZLE) return; // since user jumped to some puzzle
   if (puzzle_state_in_memory) {
      StringInputStream state_stream((string) puzzle_state_encoding+size_of_puzzle_state,(puzzle_state_length-size_of_puzzle_state));
		if (xml_version >= 0) { // is really set
			resume_read_puzzle_state_xml(&state_stream,xml_version,called_from_log);
		} else {
			resume_read_puzzle_state2(&state_stream,version_number,called_from_log);
		};
//      history_file.seekg(-favorites_count*sizeof(FavoriteSizeAndLocation),std::ios_base::end); //start+state_stream.tellg()); // restore read pointer to where it would be if other branch followed
      if (!state_stream.empty()) {
//         state_stream.read((unsigned char *) &puzzle_state_length);
         puzzle_state_length = ((int) state_stream.tellg())+size_of_puzzle_state;
         read_stuff_after_puzzle_state(&state_stream);
//         puzzle_state_length -= favorites_count*sizeof(FavoriteSizeAndLocation);
//         puzzle_state_length -= 2; // 2 extra bytes read
      };
   } else if (xml_version >= 0) { // is really set
		resume_read_puzzle_state_xml(history_file,xml_version,called_from_log);
	} else {
      resume_read_puzzle_state2(history_file,version_number,called_from_log);
   };
   if (version_number < latest_version_number) { // force a fresh dump -- what about xml_version? asked 170803
      puzzle_state_in_memory = FALSE;
      if (puzzle_state_encoding != NULL) {
         delete [] puzzle_state_encoding;
         puzzle_state_encoding = NULL;
      };
   };
	return(0);
};

void resume_read_puzzle_state2(InputStream *stream, int version_number, boolean called_from_log) {
   if (tt_temporary_user && !puzzle_state_in_memory && !called_from_log) return; //  && !called_from_log added on 040300
	tt_loading_to_ignore = (tt_system_mode != PUZZLE || (!puzzles_resumed && !replaying())); // new on 181202 to set this switch
	// was equivalent to !puzzles_resumed && !replaying() && tt_system_mode == PUZZLE prior to 181202 but if Free Play after playing a puzzle then 
	// ignore all the following
   if (!stream->empty()) {
      // this happens after everything is initialized
      initial_rocket_items = load_items(stream,version_number,tt_language);
   };
   if (!stream->empty()) {
      saved_level_stuff = load_items(stream,version_number,tt_language);
   };
   if (!stream->empty()) {
      saved_floor_items = load_items(stream,version_number,tt_language);
   };  
   if (!stream->empty()) {
      saved_item_in_hand = load_item(stream,version_number,tt_language);
   };
   if (!stream->empty()) {
      saved_robots = load_items(stream,version_number,tt_language);
   };
	if (tt_loading_to_ignore) {
		// since user jumped to some puzzle - new on 080500
		destroy_sprites(initial_rocket_items);
		destroy_sprites(saved_level_stuff);
		destroy_sprites(saved_floor_items);
		if (saved_item_in_hand != NULL) {
			saved_item_in_hand->destroy();
			saved_item_in_hand = NULL;
		};
		destroy_sprites(saved_robots);
		tt_loading_to_ignore = FALSE;
	};
};

void resume_read_puzzle_state_xml(InputStream *stream, int xml_version, boolean called_from_log) { // new on 170803
   if (tt_temporary_user && !puzzle_state_in_memory && !called_from_log) return;
	// commented out the following on 180803 since interferes with puzzle, then free play, then resume puzzle game
	//if (tt_system_mode != PUZZLE || (!puzzles_resumed && !replaying())) {
	//	ignore_xml_from_stream(stream); // initial_rocket_items
	//	ignore_xml_from_stream(stream); // saved_level_stuff
	//	ignore_xml_from_stream(stream); // saved_floor_items
	//	ignore_xml_from_stream(stream); // saved_item_in_hand
	//	ignore_xml_from_stream(stream); // saved_robots
	//} else {
		if (!stream->empty()) {
			// this happens after everything is initialized
			initial_rocket_items = xml_load_sprites(stream);
		};
		if (!stream->empty()) {
			saved_level_stuff = xml_load_sprites(stream);
		};
		if (!stream->empty()) {
			saved_floor_items = xml_load_sprites(stream);
		};  
		if (!stream->empty()) {
			boolean aborted = FALSE; // new on 140105 -- could introduce a variable to turn off the possibility of aborting here
//			UnwindProtect<boolean> set(tt_loading_is_not_to_be_aborted,TRUE); // new on 150105
			UnwindProtect<boolean> set(tt_loading_is_ok_to_abort,FALSE); // new on 130405
			saved_item_in_hand = xml_load_sprite(stream,aborted);
		};
		if (!stream->empty()) {
			saved_robots = xml_load_sprites(stream);
		};
//	};
};


