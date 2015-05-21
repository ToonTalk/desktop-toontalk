// Copyright (c) 1992-2007.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif   
#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif   
#if !defined(__TT_CUBBY_H)   
#include "cubby.h"
#endif   
#if !defined(__TT_EVENT_H)   
#include "event.h"
#endif   
#if !defined(__TT_BCKGRND_H)   
#include "bckgrnd.h"
#endif   
#if !defined(__TT_CITY_H)   
#include "city.h"
#endif   
#if !defined(__TT_TOOLS_H)   
#include "tools.h"
#endif   

coordinate tt_memory_graphics_mark_x = 0;
coordinate tt_memory_graphics_mark_y = 0;

ChangeFrequency tt_touched_a_remote;
boolean tt_touched_a_remote_that_can_change_to_match_failure = FALSE; // new on 291004

boolean tt_postpone_dirty_displays = TRUE;

//extern millisecond tt_wait_before_caching = 500; // wait 1/2 second by default
//extern millisecond tt_cache_duration = 0;
//extern millisecond tt_last_time_cache_invalidated = 0;
extern millisecond tt_cache_invalidation_cost = 500;

#if GRAPHICS_TO_MEMORY
#if TT_WINDOWS
work_page tt_memory_graphics_work_page = NULL;
HGLOBAL tt_working_page_handle = NULL;
work_page tt_background_page = NULL;
HGLOBAL tt_background_page_handle = NULL;

long tt_screen_bytes_size;
#if !TT_32
int tt_dispdib_mode;
#endif

#if TT_DEBUG_ON
bytes tt_debug_screen_bytes = NULL;
#endif

work_page tt_destination_work_page; // by default tt_memory_graphics_work_page
// better to call these tt_clipping_llx etc??
coordinate tt_destination_llx = 0;
coordinate tt_destination_lly = 0;
coordinate tt_destination_urx = 0;//tt_screen_width;	
coordinate tt_destination_ury = 0;

coordinate tt_destination_width = 0;//tt_screen_width;	
coordinate tt_destination_height = 0;//tt_screen_height;
#if WING
HDC tt_WinG_device_context = NULL;
#endif

COLORREF tt_colors[colors_count];
//COLORREF tt_dark_colors[colors_count]; // when not selected
byte tt_dark_color_indices[colors_count]; // new on 200901 is a permutation while the above are the colors themselves 
// following changed to have 7 versions so it glows red and isn't confused with other kind of object
byte tt_mismatch_color_indices[7][colors_count]; // new on 250302
//COLORREF tt_red_colors[colors_count]; // new on 200901
//byte tt_red1_color_indices[colors_count];
//byte tt_red2_color_indices[colors_count];
//byte tt_red3_color_indices[colors_count]; 
//byte tt_lighter_color_indices[4][colors_count];
byte tt_selection_color_permutations[7][colors_count];
byte tt_dark_selection_color_permutations[7][colors_count]; // new on 101201

#if !WING
unsigned char tt_color_permutation[256];
// since I treat index 0 as clar I need to know the closest color to
// black that has another index
unsigned char tt_black_index = 1;
#endif

// used by puzzles loading in state before full initialization, e.g. Integer::indirection_of_set_parameter
int tt_colors_available = 256;

/*
color BLUE;
color GREEN;
color CYAN;
color RED;
color MAGENTA;
color BROWN;
color WHITE;
color GRAY;
color DARK_GRAY;
color LIGHT_BLUE;
color LIGHT_GREEN;
color LIGHT_CYAN;
color LIGHT_RED;
color LIGHT_MAGENTA;
color LIGHT_BROWN;
color LIGHT_WHITE;
color YELLOW;
color BLACK;
*/
#else
unsigned char *tt_memory_graphics_work_page = NULL;
unsigned char *tt_memory_graphics_background_page = NULL;
#endif
#endif

//ImagePointer tt_pictures[1+LAST_PICTURE_INDEX-FIRST_PICTURE_INDEX];
#if TT_SMALL_TRIAL
char tt_programmer_head = 0;
#else
char tt_programmer_head = 2;
#endif

#if TT_WINDOWS
boolean tt_top_down_DIBs = FALSE;
char tt_mouse_button = 0;
char tt_current_mouse_button = 0;
// rationalized on 140502 to the following flags
//#if GRAPHICS_TO_MEMORY
//char tt_using_dispdib = 1;
//#else
//char tt_using_dispdib = 0;
//#endif
#endif

unsigned char tt_window_expansion_factor = 1; // new on 140502 to handle better tt_using_dispdib > 1
boolean tt_exclusive_screen_desired = TRUE; // new on 140502
boolean tt_exclusive_screen = FALSE; // have exclusive screen
boolean tt_restore_exclusive_screen = FALSE; // new on 230800 -- made global on 130502
long tt_got_exclusive_control_on_frame = -1; // new on 060602 so :set_client_size behaves differently if switching to full-screen

boolean tt_page_flipping = TRUE;

ReplayState tt_replay = NO_REPLAY;
ReplayState tt_replay_after_titles = NO_REPLAY;
long tt_titles_ended_on_frame = max_long; // new on 300104 -- changed from boolean to frame_number on 230804
boolean tt_logging = FALSE; // moved here on 300104
//boolean tt_stop_all = TRUE;  // until out of credits -- moved to tt_city on 090103
boolean tt_finish_all_animations = FALSE;

short int tt_video_mode = 320;  // default 320x240 with 256 colors
short int tt_video_mode_if_running_demo = 0; // new on 071004 so is reset to the recording resolution if needed

#if !GRAPHICS_TO_MEMORY
int short tt_work_page = 1;
#endif
//int short tt_page_count = 2; // double buffering by default

//int tt_number_of_colors = 256;

Sprites *tt_deleted_sprites = NULL;

DeletedMemory *tt_deleted_memory = NULL;

// clock_t tt_current_time = 0;
millisecond tt_current_time = 0;
millisecond tt_current_time_at_beginning_of_first_segment = 0; // new on 120024
millisecond tt_initial_current_time = -1; // restored on 140703
millisecond tt_millisecond_delta = 1; // input.cpp relies upon this being 0 on the first cycle -- 
// changed on 290802 to 1 (and it seems input.cpp doesn't care anymore) but 0 can cause divide by zero errors
millisecond tt_true_millisecond_delta = 0; // the above has a maximum value of 1000 -- this one has the truth -- even if you "come back"


char tt_sound_option = 3; // sound and music on by default

boolean tt_redraw_all = TRUE;

coordinate tt_dirty_min_x, tt_dirty_min_y, tt_dirty_max_x, tt_dirty_max_y; 

#if TT_WINDOWS
#include <windows.h>
HBITMAP tt_hidden_bitmap = 0;
//HBITMAP tt_visible_bitmap = 0;
HDC tt_hidden_device_context = 0; 
//HDC tt_visible_device_context = 0; 
//HINSTANCE tt_library = 0;
#endif

//boolean tt_game_switch_on = FALSE; // moved to tt_programmer on 090103 since should be saved there

Events *tt_events = NULL;
Queue *tt_queue = NULL;
Queue *tt_running_queue = NULL;

Suspensions *tt_suspensions = NULL;

boolean tt_copying_entire_thought_bubble = FALSE;

//boolean tt_save_pads = TRUE;

//Backgrounds *tt_backgrounds_still_animating = NULL;

// initialized by initialize_floor_stacks in floor.CPP
//Integer *tt_integer_stack;
//Number *tt_unsigned_long_int_stack;
//Cubby *tt_cubby_stack;
//Nest *tt_nest_stack;
//Thought_Bubble *tt_thought_bubble_stack;
//Robot *tt_robot_stack;
//Truck *tt_truck_stack;
//Bomb *tt_bomb_stack;
//Text *tt_text_stack;
//Notebook *tt_main_notebook;
Toolbox *tt_toolbox;
Martian *tt_martian = NULL;
Houses *tt_deleted_houses = NULL;

Picture *tt_copying_insides_for = NULL;

char tt_toontalk_crash_directory[MAX_PATH];
char tt_application_name[MAX_PATH] = "ToonTalk"; // changed to MAX_PATH on 260803 so can use 

City *tt_city;
Screen *tt_screen;

Programmer *tt_programmer;

GlobalPicture *tt_global_picture = NULL;
GlobalPicture *tt_temp_picture = NULL; // new on 011002
JoystickRemotePicture **tt_joystick_pictures = NULL;

// these were char instead but caused a very strange Borland compiler bug
short int tt_joystick_count = 0;
short int tt_force_feedback_joystick_count = 0;
short int tt_force_sensitive_mouse_count = 0; // 100801 can't deal with more than but keep it general

boolean tt_shutting_down = FALSE;

History *tt_histories = NULL;

Sprite *tt_help_target = NULL;

int tt_help_counter = 0;

int tt_help_history_index = -1; // none
int tt_last_history_index = -2; // none

int tt_help_history_action = 0;
int tt_last_history_action = 0;

long tt_frame_of_last_history = 0;

string tt_user_name = NULL;
ascii_string tt_default_file_name = NULL;

#if TT_LARGE_CITIES
// new on 020405
int tt_city_width = 10; // 10x10x4 = 400 houses max by default
int tt_city_height = 10;
#else
short int tt_city_size = 10; // 10x10x4 = 400 houses max by default
#endif

short int tt_character_size_percent = 100;
short int tt_help_speed_percent = 100;
short int tt_cache_percent = 100;

boolean tt_martian_automatically_appears = TRUE;

//Notebook *tt_last_notebook = NULL;

Sprites *tt_running_robots = NULL;

ascii_string tt_file_name = NULL;
ascii_string tt_file_name_while_time_travelling = NULL; // new on 220402
//ascii_string tt_file_name_read_only = NULL; // new on 030304
ascii_string tt_user_directory = NULL;
//int tt_user_directory_length = 0; // new on 130703 as an optimization -- bad idea since is pushed and popped
ascii_string tt_shared_files_directory = NULL;
ascii_string tt_default_user_directory = NULL; // new on 060402
boolean tt_java_files_shared = FALSE; // new on 050402 to determine whether Java files end up in Shared Folders or My Documents

boolean tt_copy_restores_pre_blank = FALSE;

char tt_mouse_button_count = 1;   // was 1 up until 9/29/97 and is again on 11/13/97 -- kids make too many mistakes

boolean tt_win31j = FALSE;

char tt_minimum_say_importance = 2; // by default say everything 2 and above
boolean tt_user_is_debugger = FALSE;
boolean tt_recording_off = FALSE;

Background *tt_deleting_background = NULL;

#if TT_NO_ROBOT_TRAINING
boolean tt_temporary_user = TRUE;
boolean tt_maintain_history = FALSE;
#else
boolean tt_temporary_user = FALSE;
boolean tt_maintain_history = TRUE;
#endif

// too large and might blow a stack
// can construct deeper structures just not one level at a time
#if TT_32
int tt_max_containment_levels = 50; // have a much bigger stack
#else
int tt_max_containment_levels = 12;
#endif

SpritesList *tt_deleted_lists = NULL;

Sprite *tt_top_copy = NULL;

// If I change this change magic cookie in pad.cpp
//int tt_notebook_version = 3; 

boolean tt_copy_top_of_stack = FALSE; // by default if something dropped on top ignored

#if TT_LEGO
// conditionalized on 220802
LegoStatus tt_lego_in_use = NO_LEGO_CONTROLLER;
int tt_lego_ports = 0; // assumes same number of input and output ports
#endif

int tt_serial_port = 0; // obsolete but a bit of work to remove ...

//millisecond tt_narration_overhead = 0; // obsolete 20204

ascii_string tt_variable_font_name = NULL;
ascii_string tt_fixed_font_name = NULL;

Sprites *tt_next_frames_deleted_sprites = NULL;

//boolean tt_button_unchanged = FALSE; -- noticed this was obsolete on 270902
boolean tt_mouse_up_simulated = FALSE;

long tt_last_key_frame_number = 0;
short int tt_last_key_sensed = 0;

Parameters *tt_user_parameters = NULL;

unsigned short int tt_speed = 100; // 100% of the real clock
unsigned short int tt_sleep = 0; // amount to sleep on each frame

ascii_string *tt_temp_files = NULL;
unsigned char tt_temp_files_count = 0;

//boolean tt_dumping_to_clipboard = FALSE;
//boolean tt_dumping_to_notebook_page = FALSE; // new on 211102
boolean tt_dumping_to_test_equality_or_describe = FALSE; // rationalize this some day as well

DumpReason tt_dump_reason = NOT_DUMPING; // rationalized on 111202

short int tt_appearance_count = 0; // should be initialized by reading DAT file

color_index tt_white = 255; // good guesses -- fixed in winmain.cpp
color_index tt_gray = 1;
color_index tt_black = 0;
color_index tt_nearest_black = 1; // will be recomputed (new on 071101)

color_index tt_nearest_text_background_color = 2; // will be recomputed (new on 040102)
color_index tt_nearest_number_background_color = 3;
color_index tt_nearest_sound_background_color = 4;
color_index tt_nearest_force_background_color = 5;
color_index tt_nearest_operation_background_color = 6; // new on 130202

// these became global when cache.cpp split off from sprite.cpp
CacheEntry *cache_entries = NULL;
#if !TT_DIRECTX
int sound_entry_in_use = -1;
#endif

unsigned char tt_log_version_number = latest_log_version_number; 

#if TT_NEW_IO
output_string_stream *tt_current_output_stream;
#else
output_stream *tt_current_output_stream;
#endif
int tt_current_output_stream_size = 256;

boolean tt_show_birds_return = TRUE;

NaturalLanguage tt_language = NO_LANGUAGE_SPECIFIED;

Background *tt_dumping_background = NULL;

#if TT_SMALL_TRIAL
SystemMode tt_system_mode = PUZZLE;
#else
SystemMode tt_system_mode = FREE_PLAY;
// this could be merged with tt_replay and ReplayState
#endif

ascii_string tt_puzzle_file_name = NULL;
ascii_string tt_alternative_spoken_puzzle_file_name = NULL;
//FileNameStatus tt_puzzle_file_name_status = FILE_ON_HARD_DRIVE;
//FileNameStatus tt_alternative_spoken_puzzle_file_name_status = FILE_ON_HARD_DRIVE;

//boolean tt_new_input_scheme = TRUE;

FavoriteSizeAndLocation *tt_favorites = NULL;

//boolean tt_favorites_set = FALSE; -- made obsolete on 111602 since some entries can be set and others not

ascii_string tt_log_out_file_name = NULL;
ascii_string tt_log_out_file_name_specified = NULL; // new on 170204 -- this is explicitly given (i.e. -o on command line)
//boolean tt_log_out_file_name_generated = TRUE;

//boolean tt_bomb_to_reset_puzzle = TRUE;

AutoDemo tt_autodemo = NO_AUTODEMO;

unsigned short int tt_subtitles_speed = 0;

MartyTalk tt_marty_talk_to_save = MARTY_SPEAKS_AND_TALK_BALLOONS; // so 16 bit visit doesn't change defaults

#if TT_TEXT_TO_SPEECH && !TT_SMALL_TRIAL
MartyTalk tt_marty_talk = MARTY_SPEAKS_AND_TALK_BALLOONS;
#else
MartyTalk tt_marty_talk = MARTY_TALK_BALLOONS;
#endif

java_string tt_java_picture_name = NULL;

unsigned char tt_java_indent = 0;

ascii_strings tt_image_file_names = NULL;

char tt_country_code[3];

boolean tt_tts_uses_direct_sound = TRUE; // changed on Oct 11 '98 since with SAPI 4.0 works fine

boolean tt_robots_have_names = TRUE;

boolean tt_main_notebook_referenced = FALSE;

unsigned short int tt_mouse_sensitivity = 100; // 100% is normal
unsigned short int tt_joystick_sensitivity = 100;

#if TT_DIRECT_INPUT
boolean tt_using_direct_input = TRUE;
#else
boolean tt_using_direct_input = FALSE;
#endif

#if !TT_32
HyperBotGlobalPicture *tt_hyperbot_picture = NULL;
#else
GlobalPicture *tt_hyperbot_picture = NULL;
#endif

boolean tt_dealing_with_follower_change = FALSE;

//string tt_java_subdirectory = NULL;

RemoteIdentifier location_and_size[saved_remotes_count] =
		 {WIDTH_REMOTE,HEIGHT_REMOTE,
		  // these first since X_REMOTE, etc. are given in terms of center_x
		  // which depends upon having the right width and height
		  X_REMOTE,Y_REMOTE,
		  X_SPEED_REMOTE,Y_SPEED_REMOTE};
//		  PARAMETER_REMOTE}; // same as image_index

ascii_string tt_city_name = NULL;

//boolean tt_darken_when_stopped = FALSE; //  // until out of credits
boolean tt_darken_all = FALSE; // rationalized on 200901

//Sprites *tt_sprites_being_dumped = NULL;
SpritePointer *tt_sprites_being_dumped = NULL; // revised on 270900
int tt_sprites_being_dumped_size = 0;

Sprite *tt_selection = NULL;

long tt_still_frame_count = 0; // those frames where the screen didn't change

boolean tt_trouble_shooting = TRUE;

#if !TT_NEW_IO
bytes tt_temporary_buffer = NULL;
short int tt_temporary_buffer_size = 512;
#endif

int tt_debug_mode = 0;
long tt_frame_number = 1;

#if TT_DIRECT_INPUT
boolean tt_will_end_up_in_a_notebook = FALSE;
#endif

HINSTANCE tt_main_instance = NULL;

long tt_max_number_of_holes = max_unsigned_short; // prior to 150202 was 2048; // slows down pretty bad as it gets bigger

Trouble tt_current_trouble = DONT_TROUBLE_SHOOT;
const_string tt_current_extra_message = NULL;
const_string tt_current_extra_message2 = NULL;

Sprite *tt_just_vacuumed = NULL; // used for generating paths
Sprite *tt_just_vacuumed_remote = NULL; // used for generating paths - new on 091000 - if remote control vacuumed this is it while tt_just_vacuumed might be its appearance

//char end_sentence[3] = ". ";

short int tt_loads_current = 0;

boolean tt_err_file_important = FALSE;
boolean tt_err_file_important_but_no_error = FALSE; // new on 031104

boolean tt_loading_robot_notebook = FALSE;

#if TT_DEBUG_ON
long tt_debug_frame_number = -1;
long tt_debug_target = min_long;
long tt_animation_counter = 0;
wide_string tt_debug_guid = NULL;
#endif

boolean tt_user_puzzles = FALSE;

short int tt_houses_to_a_block = 4;

city_coordinate tt_block_width = 4L*ideal_screen_width;

Sprites *tt_pictures_to_activate = NULL;

boolean tt_uudecoding_a_sprite = FALSE;

boolean tt_avenues_print_as_numbers = FALSE;

long tt_shift_frame_number = 0;

#if TT_32
// one for language and other for alternative spoken language
UINT tt_code_page[2] = {CP_ACP,CP_ACP}; // used to translate multibyte to wide string
#endif

boolean tt_unicode = FALSE;

string tt_keyboard_accelerators = NULL;
string tt_vacuum_keyboard_accelerators = NULL;
string tt_expander_keyboard_accelerators = NULL;
string tt_copier_keyboard_accelerators = NULL;

NaturalLanguage tt_alternative_spoken_language = NO_LANGUAGE_SPECIFIED;

char tt_alternative_spoken_country_code[3];
boolean tt_use_spoken_alternative = FALSE;

#if TT_MICROSOFT
HINTERNET tt_internet_connection = NULL;
#endif

boolean tt_skip_titles = FALSE;

city_coordinate tt_default_block_width = 4L*ideal_screen_width;
city_coordinate tt_block_height = ideal_screen_width;

city_scale tt_min_flying_scale = ground_scale;
city_coordinate tt_ideal_block_width = tt_default_block_width;
city_coordinate tt_default_ideal_block_width = tt_default_block_width;

boolean tt_good_sizes_adjust_to_screen_size = FALSE; // default typically overridden by update_defaults()

boolean tt_closed_caption = FALSE;
boolean tt_closed_caption_alternative_language = FALSE; 

boolean tt_limited_text_to_speech = FALSE;

boolean tt_dumping_with_sharing_of_all_types = FALSE;

boolean tt_mouse_acquired = TRUE;

int tt_warning_count = 0;

#if TT_DEBUG_ON
int tt_debug_dump_depth = 0;
int tt_debug_load_depth = 0;
string tt_debug_text_pad = NULL;
#endif

boolean tt_last_picture_loaded_knew_if_geometry_was_dumped = FALSE;
boolean tt_last_picture_dumped_geometry = FALSE;

long tt_paragraph_id_just_finished = -1;
long tt_log_count_max = 0;
millisecond tt_time_between_new_logs = 0;
millisecond tt_maximum_time_travel_overhead = 500; // new on 080904
millisecond tt_next_new_log_time = max_long; // don't save logs unless this value is changed
//millisecond tt_next_new_log_time_saved = -1; // if positive is a copy of tt_next_new_log_time while it has been reset because bad time to dump - 
boolean tt_time_travel_enabled = TRUE; // new on 290903 -- better than using tt_time_between_new_logs > 0 
// since need to deal with command line -time_travel_enabled before reading toontalk.ini
millisecond tt_open_log_time = 0;
millisecond tt_next_postponed_include_file_xml_time = max_long; // new on 170703

boolean tt_dont_connect_things_while_copying = FALSE;
boolean tt_copying_for_training = FALSE; // new on 080205

long tt_log_started_frame = 0;
TimeTravelState tt_time_travel = TIME_TRAVEL_OFF;
boolean tt_exit_at_end_of_log = TRUE;

boolean tt_keep_center_when_size_changes = TRUE;
boolean tt_user_initiated_dump = TRUE;

string *tt_file_search_path = NULL;
unsigned short int tt_file_search_path_length = 0;

coordinate tt_screen_width = 640; // initialized on 270200 since if zero won't initialize
coordinate tt_screen_height = 480;
coordinate tt_screen_half_width, tt_screen_half_height;

Sprite *tt_cause_of_last_mismatch = NULL;

boolean tt_logs_contain_virgin_notebook = FALSE;

short int tt_graphics_video_mode_width = 640; // if m25 is being used
short int tt_graphics_video_mode_height = 480;

boolean tt_log_used_wide_characters = FALSE;

string tt_file_name_to_clipboard = NULL; // only used when double clicked on a TT file and too soon to make the sprite - done after initialization now
Sprite *tt_add_sprite_when_on_floor = NULL; // this replaces the above on 220303

boolean tt_loading_to_describe = FALSE;
boolean tt_loading_to_ignore = FALSE; // new boolean on 181202 - e.g. when loading puzzle state in Free Play

Sprites *tt_items_loaded_so_far = NULL;

#if TT_DIRECT_PLAY
Sprites *tt_networked_nests = NULL;
Sprites *tt_nest_proxies = NULL; // used to increase sharing if multiple birds go to the same foreign nest
// and starting 050500 to receive system message

int tt_max_direct_play_queue_size = 20; // defaults - can be overridden by toontalk.ini
int tt_max_pending_messages_sent_at_once = 20;
int tt_max_pending_message_count = 50;
boolean tt_debug_direct_play = FALSE;
boolean tt_running_direct_play = TRUE; // new on 140600 so is easier to turn if not wanted
#endif

string tt_puzzles_over_html_file = NULL;

//boolean tt_using_directx_transform_for_BMPs = FALSE;
boolean tt_using_directx_transform_or_gdiplus = TRUE; // revised on 311001

ascii_string tt_URL_cache_directory = NULL;
ascii_string tt_cdrom_directory = NULL; // moved here and renamed tt_ on 300500
ascii_string tt_main_directory = NULL;
ascii_string tt_clipping_directory = NULL;
ascii_string tt_temp_directory = NULL;
ascii_string tt_user_provided_user_directory = NULL;
ascii_string tt_picture_directory = NULL;
ascii_string tt_builtin_picture_directory = NULL; // new on 121001

ascii_string tt_extracted_file_temp_directory = NULL;
boolean tt_file_temp_directory_is_up_to_date = FALSE; // new on 291003 - so know if can trust tt_extracted_file_temp_directory to be up-to-date
string tt_exe_to_run_upon_shutdown = NULL;
string tt_command_line_for_exe_to_run_upon_shutdown = NULL;
millisecond tt_duration_to_trigger_auto_hide = 3000; 
// default is 3 seconds before hiding time travel buttons -- new on 031103 - need to be set by toontalk.ini


boolean tt_direct_input_mouse_exclusive_ok = FALSE;

int tt_delay_between_titles = 5; // new on 200800 - 10 second default delay between titles -- 
// changed to 5 seconds on 170903 - and reset to 1 for demos now

unsigned char tt_second_byte_of_double_byte_input = 0;

boolean tt_save_nests_as_local = FALSE;
//boolean tt_dump_nests_as_networked = FALSE; // new on 041002 -- for implementing F11 as undo properly --
// noticed on 290103 that the above was redundant with tt_dump_reason == DUMPING_TO_ENABLE_UNBAMMING

boolean tt_shift_down = FALSE; // new on 220900 since logging of shift and control was broken
boolean tt_control_down = FALSE;

boolean tt_loading_a_notebook = FALSE;

UserImage *tt_default_user_image = NULL;

boolean tt_loading_a_city = FALSE;

boolean tt_create_subnotebooks_of_remote_notebook = FALSE; // new policy on 211200 but can be overridden by toontalk.ini

string tt_local_ini_file = NULL;
string tt_local_ini_file_uncached_name = NULL;
string tt_local_ini_file_loaded_with_city = NULL;
string tt_ini_file = ""; // new on 260601 -- was "toontalk.ini" prior to 290302
boolean tt_ini_file_only_from_tt_ini_file = TRUE; 
// new on 030304 - this is both useful and also no chance of a nasty TT or DMO file that gets
// around foreign bird safeties by overridding the toontalk.ini options
boolean tt_ini_file_from_tt_ini_file = TRUE; // new on 060304 -- reset if found locally associated with a DMO, CTY, etc. file

string tt_java_class_name = NULL;

//int tt_skip_first_n_dumped = 0;
int tt_number_of_sprites_dumped_so_far = 0;
boolean *tt_sprites_being_dumped_ok_for_eq = NULL;

NarrationCreationMode tt_creating_narration_file = NARRATION_CREATION_OFF; // new on 070201
boolean tt_print_narration_start_times = FALSE;

boolean tt_creating_java_for_an_event = FALSE;

string tt_trouble_shoot_java_applet_file_name = NULL;

PrimitiveBird *tt_most_recent_primitive_bird = NULL;
boolean tt_ask_if_ok_to_load_dll = TRUE;

boolean tt_2_mouse_buttons_is_escape = FALSE; // new on 160301

//unsigned long tt_memory_available_at_start = 0;

boolean tt_coinitialized = FALSE;

int tt_vacuum_maximum_item_count = 1000; // new on 190401

boolean tt_dump_so_equals_share = TRUE; // new on 060401
//boolean tt_dump_so_equals_share_enabled = FALSE; // new on 060401 - until completely debugged

string tt_missing_picture_file_extension = NULL; // new on 200601
string tt_missing_builtin_picture_file_extension = NULL; // new on 121001

#if TT_ABSOLUTE_MOUSE
MouseMode tt_mouse_mode = RELATIVE_MOUSE_MODE; // new on 290801
MouseMode tt_mouse_mode_on_floor = RELATIVE_MOUSE_MODE; // new on 180402
MouseMode tt_mouse_mode_not_on_floor = RELATIVE_MOUSE_MODE;

long tt_directional_pad_center_x = 92; // new on 180402 - 92% of the way to the right (since radius in terms of height this should be flush against right side)
long tt_directional_pad_center_y = 10; // 10% up from bottom
long tt_directional_pad_center_radius = 10; // circle's radius is 10% of height

boolean tt_hand_needs_to_move_for_tool_use = FALSE;
boolean tt_hand_needs_to_move_to_cursor = FALSE; // new on 220402
boolean tt_hand_needs_to_move_relatively = FALSE;

#if TT_DEBUG_ON
boolean tt_show_mouse_cursor = TRUE;
#else
boolean tt_show_mouse_cursor = FALSE;
#endif
#endif

int tt_movement_selection_feedback = 2; // was a boolean prior to 220203 -- changed to 2 on 020304 since is easier to see
boolean tt_color_selection_feedback = TRUE;

int tt_bits_per_pixel = 8; // new on 220901
int tt_bytes_per_pixel = 1; // convenient variant of the above
int tt_desired_bits_per_pixel = 0; // by default use current Windows setting (new on 191001)
int tt_red_bits_per_pixel = 8;
int tt_green_bits_per_pixel = 8;
int tt_blue_bits_per_pixel = 8;

#if TT_GDIPLUS
// new on 251001
boolean tt_prefer_gdiplus = TRUE;
#else
boolean tt_prefer_gdiplus = FALSE;
#endif

boolean tt_dither_to_8bits = TRUE; // new on 201101

//boolean tt_new_pad_look = TRUE; // new on 171201

int tt_number_output_base = 10; // new on 250202 -- currently can only range from 2 to 36

boolean tt_x_and_y_display_as_integers = TRUE; // too many digits otherwise

boolean tt_allow_multiple_toontalks = FALSE; // new on 250402 since kids get mixed up by this too easily

boolean tt_delta_x_and_delta_y_due_solely_to_arrow_keys = FALSE; // useful on some tablets - new on 060502

char tt_stop_character = '.';
char tt_run_character = ' ';

boolean tt_dragging_permitted = FALSE;
boolean tt_dragging_enabled = FALSE;

boolean tt_vacuum_used_once_per_click = FALSE; // new on 310702 to introduce a customized alternative to staying on
millisecond tt_expander_usage_maximum = -1; // negative means no maximum - default maintains old behavior

boolean tt_dump_as_xml = TRUE; // default changed on 110103 (important for time travel since saves city so early)

boolean tt_speak_button_name = TRUE; // new on 240902

//Sprites *tt_nests_temporarily_dumping_as_proxies = NULL; // new on 171002

boolean tt_want_exception_breaks = TRUE; // new on 291002
//boolean tt_want_all_exception_breaks = FALSE; // new on 291002 this includes ones that should be safe to recover from

string tt_ok_to_generate_mini_dump_message = "Sorry, something is wrong with ToonTalk. Is it OK to generate a diagnostic file to help fix the problem?"; // set here in case error occurs before reset by toontalk.ini
string tt_mini_dump_email = "mailto:support@toontalk.com?subject=ToonTalk Crash Report&body=Please add any information that might be useful. And don't forget the attachment."; 
BOOL tt_show_mini_dump_directory = TRUE;
BOOL tt_full_memory_dump = FALSE;

AnimationCallBacks *tt_not_city_stopped_callbacks = NULL; // was in utils.cpp prior to 181102 - renamed on 090103

#if TT_DEBUG_ON
int tt_number_value_count = 0;
int tt_number_value_destroyed_count = 0;
#endif

boolean tt_debugging_leaks = FALSE; // new on 180103
boolean tt_reclaiming_memory = TRUE;

int tt_xml_version_number = 4; // new on 180103 - reset when loading city or clipping
// default changed to version 4 on 210506 for the changes to comparing text of different cases

int tt_xml_language_index = 0;

boolean tt_create_new_notebooks = TRUE; 
// until fully debugged - new on 090203 -- restored on 230603 as experiment - set back to FALSE on 190703 - TRUE on 250703 since Math notebook missing and some sensors are black initially

boolean tt_parts_change_when_bammed = TRUE; // new on 230203

boolean tt_zip_files = TRUE; // new on 230203

string tt_private_media_directory = NULL;
int tt_private_media_directory_length = 0;
boolean tt_maintaining_private_media_directory = TRUE;
boolean tt_saving_media = FALSE; 
Sprites *tt_sprites_with_media_to_save = NULL;
//boolean tt_save_media_in_sub_notebooks = FALSE; // new on 200903
boolean tt_save_media_in_main_notebook = FALSE; // new on 270204
string tt_extra_media_file_names = NULL; // new on 081203 -- used to collect the media files used by notebook pages

char shared_buffer[shared_buffer_size]; // by placing this here it should run faster but is not thread safe - moved here 180303

#if TT_DEBUG_ON
unsigned int tt_counter_for_debugging = 0; // moved here on 240103
unsigned int tt_debug_counter_at_last_reset = 0; // new on 250903
unsigned int tt_deleted_sprite_count = 0; // new on 240103
int tt_sprites_counter = 0;
#endif

string tt_commands_to_convert_sounds_to_au = NULL;

int tt_digits_after_decimal_point_in_java = 1000; // good default - new 040503

boolean tt_recording_notebook_for_log = FALSE; 
// new on 250503 to fix bugs generating demos for new users due to conversion to XML of sensor for the clipboard and more

boolean tt_moving_stuff_from_room_to_floor = FALSE; // new on 050603
boolean tt_moving_stuff_from_floor_to_room = FALSE; // new on 050603

boolean tt_resetting_or_destroying_city = FALSE; // new on 100603

#if TT_DYNA_ZIP
string tt_log_out_archive = NULL; // new on 260603
string tt_log_in_archive = NULL; // new on 260603
#else
zipFile tt_log_out_archive = NULL; // new on 250603
unzFile tt_log_in_archive = NULL; // new on 250603
#endif

// unsigned int tt_log_out_archive_size = 0; -- obsolete 160204

boolean tt_delete_replay_file_after_reading = FALSE;

//Sprite *tt_part_of_city_being_dumped_for_time_travel = NULL; // obsolete as of 150703

double tt_number_shrinkage = 0.95; // was a constant prior to 140703

// int tt_include_location_at_the_start_of_segment = 0; -- obsolete 160204

int tt_oldest_log_segment = -1; // renamed and moved here on 180703
int tt_youngest_log_segment = -1;
int tt_current_log_segment = 0; // if brand new archive this will be incremented first
int tt_starting_log_segment = -1; // meaning not set
#if TT_ALPHA_FEATURE
int tt_prevous_log_segment = -1; // new on 041103
#endif
boolean tt_create_new_time_line = FALSE; // new on 010304

xml_element *tt_zip_description_archive_element = NULL; // new on 190703 to hold information about the archive (like oldest and youngest segments)

boolean tt_jump_to_current_log_segment = FALSE; // new on 190703
boolean tt_jump_to_youngest_log_segment = FALSE; // new on 290204

boolean tt_include_media_in_time_travel_archives = FALSE; // new on 210703
//boolean tt_might_include_media_in_time_travel_archives = TRUE; // this doesn't cost too much - noticed obsolete on 021003

boolean tt_ok_to_optimize_successive_demos = TRUE; // new on 220703

int tt_max_number_of_time_lines = 5; // moved here on 240703

boolean tt_titles_just_ended = FALSE; // new on 230803

millisecond tt_log_initialization_start_time = 0; // new on 310803

#if TT_ALPHA
string tt_additional_command_line = NULL;
#endif

ascii_string tt_err_file_name = NULL; // moved here on 041003

long tt_cache_memory_max_size = 100000000; // moved here and added tt_ on 061003 -- was unsigned earlier -- given an initial value since can come up while initializing

boolean tt_keep_all_time_travel_segments = FALSE; // new on 061003 - mostly good for researchers and teachers since extra logs are "hidden"

boolean tt_loading_to_undo_bammer = FALSE; // new with ALPHA5

boolean tt_time_travel_after_display_updated = FALSE; // was time_travel_after_display_updated in windows.cpp prior to 031103

boolean tt_subtitles_and_narration_even_without_demo = FALSE; // new on 061103 -- replaces tt_debug_mode == 1785

boolean tt_right_to_left_text = FALSE; // e.g. Hebrew - new on 081203
boolean tt_unicode_clipboard = FALSE; // new on 101203
boolean tt_remove_playground_active_documentation = FALSE; // new on 181203
boolean tt_scrolling_ok = TRUE; // new on 030204

boolean tt_record_clock = TRUE;

DWORD tt_real_time_pause_began = 0; // new on 130204
millisecond tt_time_spent_paused = 0; // while this can be computed from the above it requires a call to timeGetTime() at the right time
DWORD tt_real_time_pause_ended = 0; // new on 190204
millisecond tt_current_time_pause_ended = 0;

string tt_file_name_clipboard_message = NULL; // new on 130204
boolean tt_add_sprite_when_on_floor_but_wait_a_frame = FALSE; // new on 140204

//boolean tt_load_new_main_notebook_files = FALSE;

city_coordinate tt_drop_files_x = 0; // new on 140204
city_coordinate tt_drop_files_y = 0;
int tt_drop_files_count = 0;
string *tt_drop_file_names = NULL;
string *tt_drop_file_hash_names = NULL;

string tt_current_notebook_zip_archive = NULL; // new on 160204

int tt_make_current_exe_default = 1; // new on 020304 - like a boolean but if not 0 or 1 then ask
//int tt_install_current_exe = 1; // new on 020304 - like a boolean but if not 0 or 1 then ask

boolean tt_user_name_should_be_new = FALSE; // new on 030304

boolean tt_include_descriptions_in_xml = FALSE; // new on 050304

boolean tt_match_blank_with_sensor_then_run_once_per_cycle = TRUE; // new on 030404

boolean tt_robot_drop_location_follows_camera = FALSE; // new on 040404

boolean tt_emulate_mouse_for_full_screen_time_travel = TRUE; // new on 030504 -- default changed to TRUE on 010305

string tt_file_name_of_tt_object_just_saved = NULL; // new on 140604

boolean tt_reincarnate_tools = TRUE; // new on 070704

Vacuum *tt_toolbox_vacuum = NULL; // new on 080704
Copier *tt_toolbox_copier = NULL;
Expander *tt_toolbox_expander = NULL;
Notebook *tt_toolbox_notebook = NULL;

boolean tt_display_demo_subtitles = TRUE; // new on 210704
boolean tt_play_demo_sound_files = TRUE; // new on 210704
boolean tt_speak_demo_text = TRUE; // new on 210704

//Backgrounds *tt_backgrounds_to_delete_next_frame = NULL;

Sprites *tt_nests_to_set_original_nest = NULL; // new on 180804

xml_document *tt_city_document_to_load = NULL; // new on 300904

boolean tt_number_to_text_exact = TRUE; // new on 121004

boolean tt_stop_all_pictures = FALSE; // new on 161004

boolean tt_shrink_and_grow_numbers = TRUE; // new on 251004

int tt_notebook_id_counter = 10000; // made global on 281004 -- starts off with a number that should be larger than any old saved city

string tt_saved_city_file_name_when_demo_ends = NULL; // new on 281004 but maybe only useful internally for testing

Background *tt_next_background = NULL; 
// new on 121204 to support decoration sensors and not have them do unneeded and incorrect work when standing

Sprites *tt_previous_controlee_replacements = NULL; // new on 151204

boolean tt_run_demos_despite_anomalies = FALSE; // new on 311204

boolean tt_dragging = FALSE; // new on 020105 for drag and drop

boolean tt_ok_to_process_windows_messages = TRUE; // except inside one_tt_cycle

boolean tt_remove_unneeded_things_from_thought_bubble = TRUE; // new on 110105

boolean tt_loading_is_ok_to_abort = FALSE; // new on 140105 -- revised 200105

int tt_sprites_loaded_count = 0; // new on 150105
int tt_sprites_saved_count = 0;  // new on 150105
int tt_bytes_read_from_URL = 0; // new on 290105

boolean tt_loading_vacuum_contents = FALSE; // new on 170105

xml_document *tt_city_document_to_load_after_demo_ends = NULL;

boolean tt_standard_exit_started = FALSE; // new on 150105 -- made global on 270105

boolean tt_old_nest_merging_behavior = FALSE; // new on 140205 in case anyone really wants/needs it

boolean tt_describing_a_comment = FALSE; 
// should really redo this whole thing but this helps in describing things (e.g. comments) -- new on 210205

Picture *tt_function_key_help = NULL; // new on 280205

Sprite *tt_call_for_marty = NULL; // new on 030805
boolean tt_marty_appears_only_if_called = FALSE; // new on 030805
boolean tt_dont_set_tt_loading_is_ok_to_abort = FALSE; // new on 030805
millisecond tt_call_for_marty_wait_duration = 15000; // new on 040805 - changed from 30000 on 081105

string tt_dump_city_to_file_name_when_titles_over = NULL; // new on 050805

boolean tt_expires = FALSE; // new on 090807 will be set to TRUE if a trial or beta version
