// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved.

#ifndef __GLOBALS_H

#define __GLOBALS_H

// should really name all of these tt_<...>

//extern const street min_x_street, max_x_street, min_y_street, max_y_street;

extern unsigned int tt_city_units_per_horizontal_pixel,
						  tt_city_units_per_vertical_pixel;

extern city_coordinate tt_character_height;

extern short int tt_video_mode;
extern short int tt_video_mode_if_running_demo;
 
#if !GRAPHICS_TO_MEMORY

extern short int tt_work_page; // page being constructed but not yet visible

#endif

//extern short tt_page_count; // controls double buffering
//extern boolean tt_caching_background;
extern boolean tt_caching_screen;
// extern boolean tt_redraw_all;

extern long tt_frame_number;

//class Image;
//typedef Image *ImagePointer;
//extern ImagePointer tt_pictures[];

//extern int tt_number_of_colors;
//extern unsigned char BLACK;

extern COLORREF tt_colors[colors_count];
//extern COLORREF tt_dark_colors[colors_count];
extern byte tt_dark_color_indices[colors_count];
extern byte tt_mismatch_color_indices[7][colors_count];
//extern COLORREF tt_red_colors[colors_count];
//extern byte tt_red1_color_indices[colors_count];
//extern byte tt_red2_color_indices[colors_count];
//extern byte tt_red3_color_indices[colors_count];
//extern byte tt_selection_color_indices[7][colors_count];
extern byte tt_selection_color_permutations[7][colors_count];
extern byte tt_dark_selection_color_permutations[7][colors_count];
//extern byte tt_color_intensity_permutations[8][colors_count];

extern int tt_background_cache_page;
extern int tt_screen_cache_page; // background and stuff on top

//extern float tt_delta;   // time since last cycle in seconds
//#include <time.h> // for now
//extern clock_t tt_current_time;

//extern float tt_delta;
extern millisecond tt_millisecond_delta;
extern millisecond tt_true_millisecond_delta;

extern millisecond tt_current_time;
extern millisecond tt_current_time_at_beginning_of_first_segment;
extern millisecond tt_initial_current_time; // new on 131202 so loading cities sets the clock accurately - restored 140703

extern ReplayState tt_replay; 
extern ReplayState tt_replay_after_titles;
extern boolean tt_logging;
extern long tt_titles_ended_on_frame;
extern int tt_debug_mode;

//extern boolean tt_stop_all;
extern boolean tt_finish_all_animations;

extern char tt_programmer_head;

extern coordinate tt_memory_graphics_mark_x;
extern coordinate tt_memory_graphics_mark_y;

extern char mouse_buttons; // how many -- 0 if no mouse

extern coordinate tt_screen_width,
						tt_screen_height,
						tt_screen_half_width,
						tt_screen_half_height;

extern coordinate tt_dirty_min_x, tt_dirty_min_y, tt_dirty_max_x, tt_dirty_max_y; 


#include <windows.h>
extern HBITMAP tt_hidden_bitmap; // tt_visible_bitmap;
extern HDC tt_hidden_device_context; // tt_visible_device_context; 
class MainWindow;
extern MainWindow *tt_main_window;
//extern HINSTANCE tt_library;
//extern millisecond tt_mouse_button_up_time;
extern char tt_mouse_button; // which one is down
extern char tt_current_mouse_button; // saved copy for sensors
/*
#if GRAPHICS_TO_MEMORY
extern color BLUE;
extern color GREEN;
extern color CYAN;
extern color RED;
extern color MAGENTA;
extern color BROWN;
extern color WHITE;
extern color GRAY;
extern color DARK_GRAY;
extern color LIGHT_BLUE;
extern color LIGHT_GREEN;
extern color LIGHT_CYAN;
extern color LIGHT_RED;
extern color LIGHT_MAGENTA;
extern color LIGHT_BROWN;
extern color LIGHT_WHITE;
extern color YELLOW;
extern color BLACK;
#endif
*/

extern ChangeFrequency tt_touched_a_remote;
extern boolean tt_touched_a_remote_that_can_change_to_match_failure;
extern boolean tt_postpone_dirty_displays;

//extern millisecond tt_wait_before_caching;
//extern millisecond tt_cache_duration;
//extern millisecond tt_last_time_cache_invalidated;
extern millisecond tt_cache_invalidation_cost;

#if GRAPHICS_TO_MEMORY
#if TT_WINDOWS
extern boolean tt_top_down_DIBs;
extern work_page tt_memory_graphics_work_page;
//extern dib tt_work_dib;
//extern long tt_dib_size;
extern HGLOBAL tt_working_page_handle;
extern work_page tt_background_page;
extern HGLOBAL tt_background_page_handle;

//extern boolean tt_cache_page_valid;
extern long tt_screen_bytes_size;
#if !TT_32
extern int tt_dispdib_mode;
#endif

#if TT_DEBUG_ON
extern bytes tt_debug_screen_bytes;
#endif

extern work_page tt_destination_work_page;
extern coordinate tt_destination_llx;
extern coordinate tt_destination_lly;
extern coordinate tt_destination_urx;
extern coordinate tt_destination_ury;
extern coordinate tt_destination_width;
extern coordinate tt_destination_height;

#if WING
extern HDC tt_WinG_device_context;
#endif
#if !WING
extern unsigned char tt_color_permutation[256];
extern unsigned char tt_black_index;
#endif
#else
extern unsigned char *tt_memory_graphics_work_page;
extern unsigned char *tt_memory_graphics_background_page;
#endif
#endif

extern int tt_colors_available;

//extern char tt_using_dispdib;

extern unsigned char tt_window_expansion_factor;
extern boolean tt_exclusive_screen_desired;
extern boolean tt_exclusive_screen;
extern boolean tt_restore_exclusive_screen;
extern long tt_got_exclusive_control_on_frame;

extern boolean tt_page_flipping;

extern int tt_ideal_units_per_horizontal_pixel,
           tt_ideal_units_per_vertical_pixel;
//			  tt_half_ideal_units_per_horizontal_pixel,
//           tt_half_ideal_units_per_vertical_pixel;

//extern boolean tt_game_switch_on;

//class Leaders;
//extern Leaders **tt_leaders;

class Sprites;
extern Sprites *tt_deleted_sprites;

class DeletedMemory;
extern DeletedMemory *tt_deleted_memory;

extern char tt_sound_option;

//class Cubby;
//extern Cubby *tt_body_cubby;

class Events;
extern Events *tt_events;

class Queue;
extern Queue *tt_queue;
extern Queue *tt_running_queue;

class Suspensions;
extern Suspensions *tt_suspensions;

extern boolean tt_copying_entire_thought_bubble;

//extern boolean tt_save_pads;

//class Backgrounds;
//extern Backgrounds *tt_backgrounds_still_animating;

//class Integer;
//extern Integer *tt_integer_stack;
//extern Number *tt_unsigned_long_int_stack;
//class Text;
//extern Text *tt_text_stack;
//class Cubby;
//extern Cubby *tt_cubby_stack;
//class Nest;
//extern Nest *tt_nest_stack;
//class Bomb;
//extern Bomb *tt_bomb_stack;
//class Thought_Bubble;
//extern Thought_Bubble *tt_thought_bubble_stack;
//class Robot;
//extern Robot *tt_robot_stack;
//class Truck;
//extern Truck *tt_truck_stack;

class Picture;
class GlobalPicture;
class JoystickRemotePicture;
extern Picture *tt_copying_insides_for;
extern GlobalPicture *tt_global_picture;
extern GlobalPicture *tt_temp_picture;
extern JoystickRemotePicture **tt_joystick_pictures;
extern short int tt_joystick_count;
extern short int tt_force_feedback_joystick_count;
extern short int tt_force_sensitive_mouse_count;

class City;
extern City *tt_city;
class Screen;
extern Screen *tt_screen;
class Programmer;
extern Programmer *tt_programmer;

//extern Notebook *tt_main_notebook;

class Toolbox;
extern Toolbox *tt_toolbox;

class Martian;
extern Martian *tt_martian;

class Houses;
extern Houses *tt_deleted_houses;

extern boolean tt_shutting_down;

extern History *tt_histories;

extern Sprite *tt_help_target;

extern int tt_help_counter;

extern int tt_help_history_index;
extern int tt_last_history_index;

extern int tt_help_history_action;
extern int tt_last_history_action;

extern long tt_frame_of_last_history;

extern string tt_user_name;
extern ascii_string tt_default_file_name;

// needs to be able to be negative to indicate needs random filling
#if TT_LARGE_CITIES
extern int tt_city_width;
extern int tt_city_height;
#else
extern short int tt_city_size;
#endif

extern short int tt_character_size_percent;
extern short int tt_help_speed_percent;
extern short int tt_cache_percent;

extern boolean tt_martian_automatically_appears;

//class Notebook;
//extern Notebook *tt_last_notebook;

extern Sprites *tt_running_robots;

extern ascii_string tt_file_name;
extern ascii_string tt_file_name_while_time_travelling;
//extern ascii_string tt_file_name_read_only;
extern ascii_string tt_user_directory;
extern ascii_string tt_shared_files_directory; // new on 030402
extern ascii_string tt_default_user_directory;
extern boolean tt_java_files_shared;

extern boolean tt_copy_restores_pre_blank;
extern char tt_mouse_button_count;
extern boolean tt_win31j;
extern boolean tt_maintain_history;
//extern boolean tt_give_advice;
extern char tt_minimum_say_importance;
extern boolean tt_user_is_debugger;
extern boolean tt_recording_off;

class Background;
extern Background *tt_deleting_background;
extern Background *tt_dumping_background;

extern boolean tt_temporary_user;
extern int tt_max_containment_levels;

class SpritesList;
extern SpritesList *tt_deleted_lists;

extern Sprite *tt_top_copy;

//extern int tt_notebook_version;
extern boolean tt_copy_top_of_stack;

#if TT_LEGO
// conditionalized on 220802
extern LegoStatus tt_lego_in_use;
extern int tt_lego_ports;
#endif

extern int tt_serial_port;

//extern millisecond tt_narration_overhead;

extern ascii_string tt_variable_font_name;
extern ascii_string tt_fixed_font_name;

extern Sprites *tt_next_frames_deleted_sprites;

//extern boolean tt_button_unchanged;
extern boolean tt_mouse_up_simulated;

extern long tt_last_key_frame_number;
extern short int tt_last_key_sensed;

class Parameters;
extern Parameters *tt_user_parameters;

extern unsigned short int tt_speed;
extern unsigned short int tt_sleep; // amount to sleep on each frame

extern ascii_string *tt_temp_files;
extern unsigned char tt_temp_files_count;

//extern boolean tt_dumping_to_clipboard;
//extern boolean tt_dumping_to_notebook_page;
extern DumpReason tt_dump_reason;

extern short int tt_appearance_count;

extern color_index tt_white, tt_gray, tt_black, tt_nearest_black;

extern color_index tt_nearest_text_background_color, tt_nearest_number_background_color, tt_nearest_sound_background_color, tt_nearest_force_background_color, tt_nearest_operation_background_color;

class CacheEntry;
extern CacheEntry *cache_entries;
#if !TT_DIRECTX
extern int sound_entry_in_use;
#endif

//#if TT_BETA
//extern boolean tt_running_old_demos;
//#endif

extern unsigned char tt_log_version_number;

class ostrstream;
#if TT_NEW_IO
extern output_string_stream *tt_current_output_stream;
#else
extern output_stream *tt_current_output_stream;
#endif
extern int tt_current_output_stream_size;

extern boolean tt_show_birds_return;

extern NaturalLanguage tt_language;

extern SystemMode tt_system_mode;
extern ascii_string tt_puzzle_file_name;
extern ascii_string tt_alternative_spoken_puzzle_file_name;
//extern FileNameStatus tt_puzzle_file_name_status;
//extern FileNameStatus tt_alternative_spoken_puzzle_file_name_status;

//extern unsigned short tt_robot_counter;

//extern boolean tt_new_input_scheme;

extern FavoriteSizeAndLocation *tt_favorites;

//extern boolean tt_favorites_set;

extern ascii_string tt_log_out_file_name;
extern ascii_string tt_log_out_file_name_specified;
//extern boolean tt_log_out_file_name_generated; // new on 030300

//extern boolean tt_bomb_to_reset_puzzle;

extern AutoDemo tt_autodemo;

//extern string tt_original_command_line;

extern unsigned short int tt_subtitles_speed;

extern MartyTalk tt_marty_talk;
extern MartyTalk tt_marty_talk_to_save;

#if TT_DEBUG_ON
extern long tt_debug_target;
extern long tt_debug_frame_number;
extern long tt_animation_counter;
extern wide_string tt_debug_guid;
#endif

extern java_string tt_java_picture_name;
extern unsigned char tt_java_indent;

extern ascii_strings tt_image_file_names;

extern char tt_country_code[3];

extern boolean tt_tts_uses_direct_sound;

extern boolean tt_robots_have_names;

extern boolean tt_main_notebook_referenced;

//extern string tt_java_subdirectory;

extern unsigned short int tt_mouse_sensitivity;
extern unsigned short int tt_joystick_sensitivity;

extern boolean tt_using_direct_input;

#if !TT_32
class HyperBotGlobalPicture;
extern HyperBotGlobalPicture *tt_hyperbot_picture;
#else
extern GlobalPicture *tt_hyperbot_picture; // should be NULL
#endif

extern boolean tt_dealing_with_follower_change;

extern RemoteIdentifier location_and_size[saved_remotes_count];

extern ascii_string tt_city_name;

extern boolean tt_darken_all;

//extern Sprites *tt_sprites_being_dumped;
extern SpritePointer *tt_sprites_being_dumped;
extern int tt_sprites_being_dumped_size;


extern boolean tt_dumping_to_test_equality_or_describe;

extern Sprite *tt_selection;

extern long tt_still_frame_count;

extern boolean tt_trouble_shooting;

//#if !TT_NEW_IO
//extern bytes tt_temporary_buffer;
//#endif
//extern short int tt_temporary_buffer_size;

#if TT_DIRECT_INPUT
extern boolean tt_will_end_up_in_a_notebook;
#endif

extern HINSTANCE tt_main_instance;

extern long tt_max_number_of_holes;

extern Trouble tt_current_trouble;
extern const_string tt_current_extra_message;
extern const_string tt_current_extra_message2;

//extern char end_sentence[3];

extern Sprite *tt_just_vacuumed;
extern Sprite *tt_just_vacuumed_remote;

extern short int tt_loads_current;

extern boolean tt_err_file_important;
extern boolean tt_err_file_important_but_no_error;

extern boolean tt_loading_robot_notebook;

extern boolean tt_user_puzzles;

extern short int tt_houses_to_a_block;

extern city_coordinate tt_block_width;
extern city_coordinate tt_block_height;

extern Sprites *tt_pictures_to_activate;

extern boolean tt_uudecoding_a_sprite;

extern boolean tt_avenues_print_as_numbers;

extern long tt_shift_frame_number;

#if TT_32
extern UINT tt_code_page[2]; // used to translate multibyte to wide string
// one for language and other for alternative spoken language
#endif

extern boolean tt_unicode;

extern string tt_keyboard_accelerators;
extern string tt_vacuum_keyboard_accelerators;
extern string tt_expander_keyboard_accelerators;
extern string tt_copier_keyboard_accelerators;

extern NaturalLanguage tt_alternative_spoken_language;
extern char tt_alternative_spoken_country_code[3];
extern boolean tt_use_spoken_alternative;

#if TT_MICROSOFT
// my version of Borland C++ doesn't seem to include it and the Microsoft one has pragma's that Borland doesn't grok
#include <wininet.h> // Internet API
extern HINTERNET tt_internet_connection;
#endif

extern boolean tt_skip_titles;

// should really restore these to constants now that greater than 640x480 screens is implemented without changing these
extern city_coordinate tt_default_block_width; // prior to 160999 was a constant
extern city_scale tt_min_flying_scale; // prior to 160999 was a constant
extern city_coordinate tt_ideal_block_width;
extern city_coordinate tt_default_ideal_block_width; 

extern boolean tt_good_sizes_adjust_to_screen_size;
extern boolean tt_closed_caption;
extern boolean tt_closed_caption_alternative_language;

extern boolean tt_limited_text_to_speech;

extern boolean tt_dumping_with_sharing_of_all_types;

extern boolean tt_mouse_acquired;

extern int tt_warning_count;

#if TT_DEBUG_ON
extern int tt_debug_dump_depth;
extern int tt_debug_load_depth;
extern string tt_debug_text_pad;
#endif

extern boolean tt_last_picture_loaded_knew_if_geometry_was_dumped;
extern boolean tt_last_picture_dumped_geometry;

extern long tt_paragraph_id_just_finished;
extern long tt_log_count_max;
extern millisecond tt_time_between_new_logs;
extern millisecond tt_maximum_time_travel_overhead;
extern millisecond tt_next_new_log_time;
extern boolean tt_time_travel_enabled;
//extern millisecond tt_next_new_log_time_saved; noticed on 290903 this is obsolete
extern millisecond tt_open_log_time;
extern millisecond tt_next_postponed_include_file_xml_time;

extern boolean tt_dont_connect_things_while_copying;
extern boolean tt_copying_for_training;

extern long tt_log_started_frame;
extern TimeTravelState tt_time_travel;
extern boolean tt_exit_at_end_of_log;
extern boolean tt_keep_center_when_size_changes;
extern boolean tt_user_initiated_dump;

extern string *tt_file_search_path;
extern unsigned short int tt_file_search_path_length;

extern Sprite *tt_cause_of_last_mismatch;

extern boolean tt_logs_contain_virgin_notebook;

extern short int tt_graphics_video_mode_width;
extern short int tt_graphics_video_mode_height;

extern boolean tt_log_used_wide_characters;

extern string tt_file_name_to_clipboard;
extern Sprite *tt_add_sprite_when_on_floor;

extern boolean tt_loading_to_describe; // so don't report errors in the same way
extern boolean tt_loading_to_ignore;

extern Sprites *tt_items_loaded_so_far;

#if TT_DIRECT_PLAY
extern Sprites *tt_networked_nests;
extern Sprites *tt_nest_proxies;
extern int tt_max_direct_play_queue_size;
extern int tt_max_pending_messages_sent_at_once;
extern int tt_max_pending_message_count;
extern boolean tt_debug_direct_play;
extern boolean tt_running_direct_play;
#endif

extern string tt_puzzles_over_html_file;

extern boolean tt_using_directx_transform_or_gdiplus;

extern ascii_string tt_URL_cache_directory;
extern ascii_string tt_cdrom_directory;
extern ascii_string tt_main_directory;
extern ascii_string tt_clipping_directory;
extern ascii_string tt_temp_directory;
extern ascii_string tt_user_provided_user_directory;
extern ascii_string tt_picture_directory;
extern ascii_string tt_builtin_picture_directory;
//#if TT_ALPHA_FEATURE
extern ascii_string tt_extracted_file_temp_directory; // new on 281003
extern boolean tt_file_temp_directory_is_up_to_date;
extern string tt_exe_to_run_upon_shutdown;
extern string tt_command_line_for_exe_to_run_upon_shutdown;
extern millisecond tt_duration_to_trigger_auto_hide;
//#endif

extern boolean tt_direct_input_mouse_exclusive_ok;

extern int tt_delay_between_titles;

extern unsigned char tt_second_byte_of_double_byte_input;

extern boolean tt_save_nests_as_local;
//extern boolean tt_dump_nests_as_networked;

extern boolean tt_shift_down;
extern boolean tt_control_down;

extern boolean tt_loading_a_notebook;

class UserImage;
extern UserImage *tt_default_user_image;

extern boolean tt_loading_a_city;

extern boolean tt_create_subnotebooks_of_remote_notebook;

extern string tt_local_ini_file;
extern string tt_local_ini_file_uncached_name;
extern string tt_local_ini_file_loaded_with_city;
extern string tt_ini_file;
extern boolean tt_ini_file_only_from_tt_ini_file;
extern boolean tt_ini_file_from_tt_ini_file;

extern string tt_java_class_name; // new on 310101

//extern int tt_skip_first_n_dumped;
extern int tt_number_of_sprites_dumped_so_far;
extern boolean *tt_sprites_being_dumped_ok_for_eq;

extern NarrationCreationMode tt_creating_narration_file;
extern boolean tt_print_narration_start_times;

extern boolean tt_creating_java_for_an_event; // new on 180201

extern string tt_trouble_shoot_java_applet_file_name;

class PrimitiveBird;
extern PrimitiveBird *tt_most_recent_primitive_bird; // new on 280201
extern boolean tt_ask_if_ok_to_load_dll;

extern boolean tt_2_mouse_buttons_is_escape;

//extern unsigned long tt_memory_available_at_start; // new on 010401
extern boolean tt_coinitialized;

extern int tt_vacuum_maximum_item_count;

extern boolean tt_dump_so_equals_share;
//extern boolean tt_dump_so_equals_share_enabled;

extern string tt_missing_picture_file_extension;
extern string tt_missing_builtin_picture_file_extension;

#if TT_ABSOLUTE_MOUSE
extern MouseMode tt_mouse_mode;
extern MouseMode tt_mouse_mode_on_floor;
extern MouseMode tt_mouse_mode_not_on_floor;

extern long tt_directional_pad_center_x;
extern long tt_directional_pad_center_y;
extern long tt_directional_pad_center_radius;

extern boolean tt_hand_needs_to_move_for_tool_use;
extern boolean tt_hand_needs_to_move_to_cursor;
extern boolean tt_hand_needs_to_move_relatively;

extern boolean tt_show_mouse_cursor;

#endif

extern int tt_movement_selection_feedback;
extern boolean tt_color_selection_feedback;

extern int tt_bits_per_pixel;
extern int tt_bytes_per_pixel;
extern int tt_desired_bits_per_pixel;
extern int tt_red_bits_per_pixel;
extern int tt_green_bits_per_pixel;
extern int tt_blue_bits_per_pixel;

extern boolean tt_prefer_gdiplus;

extern boolean tt_dither_to_8bits;

extern int tt_number_output_base;

extern boolean tt_x_and_y_display_as_integers;

extern boolean tt_allow_multiple_toontalks;

extern boolean tt_delta_x_and_delta_y_due_solely_to_arrow_keys;

extern char tt_stop_character;
extern char tt_run_character;

extern boolean tt_dragging_permitted;
extern boolean tt_dragging_enabled;

extern boolean tt_vacuum_used_once_per_click;
extern millisecond tt_expander_usage_maximum;

extern boolean tt_dump_as_xml; // new on 080102

extern boolean tt_speak_button_name;

//extern Sprites *tt_nests_temporarily_dumping_as_proxies;

extern char tt_toontalk_crash_directory[MAX_PATH]; // new on 241002 for mini dumps
extern char tt_application_name[];

extern boolean tt_want_exception_breaks;
//extern boolean tt_want_all_exception_breaks;

extern string tt_ok_to_generate_mini_dump_message;
extern string tt_mini_dump_email;
extern BOOL tt_show_mini_dump_directory;
extern BOOL tt_full_memory_dump;

class AnimationCallBacks;
extern AnimationCallBacks *tt_not_city_stopped_callbacks;

#if TT_DEBUG_ON
extern int tt_number_value_count;
extern int tt_number_value_destroyed_count;
#endif

extern boolean tt_debugging_leaks;
extern boolean tt_reclaiming_memory;

extern int tt_xml_version_number;
#if TT_POST327
extern int tt_xml_language_index;
#endif

extern boolean tt_create_new_notebooks;

extern boolean tt_parts_change_when_bammed;

extern boolean tt_zip_files;

extern string tt_private_media_directory;
extern int tt_private_media_directory_length;
extern boolean tt_maintaining_private_media_directory;
extern boolean tt_saving_media;
extern Sprites *tt_sprites_with_media_to_save;
//extern boolean tt_save_media_in_sub_notebooks;
extern boolean tt_save_media_in_main_notebook;

#if TT_ALPHA_FEATURE
extern string tt_extra_media_file_names;
#endif

extern char shared_buffer[shared_buffer_size]; 

#if TT_DEBUG_ON
extern unsigned int tt_counter_for_debugging;
extern unsigned int tt_debug_counter_at_last_reset;
extern unsigned int tt_deleted_sprite_count;
extern int tt_sprites_counter;
#endif

extern string tt_commands_to_convert_sounds_to_au;

extern int tt_digits_after_decimal_point_in_java;

extern boolean tt_recording_notebook_for_log;

extern boolean tt_moving_stuff_from_room_to_floor;
extern boolean tt_moving_stuff_from_floor_to_room;

extern boolean tt_resetting_or_destroying_city;

#if TT_DYNA_ZIP
extern string tt_log_out_archive; // new on 260603
extern string tt_log_in_archive; // new on 260603
#else
extern zipFile tt_log_out_archive; // new on 250603
extern unzFile tt_log_in_archive; // new on 250603
#endif

//extern unsigned int tt_log_out_archive_size;

extern boolean tt_delete_replay_file_after_reading;

//extern Sprite *tt_part_of_city_being_dumped_for_time_travel;

extern double tt_number_shrinkage;

// extern int tt_include_location_at_the_start_of_segment;

extern int tt_oldest_log_segment; // renamed and moved here on 180703
extern int tt_youngest_log_segment;
extern int tt_current_log_segment; // starts reading version 1 by default
extern int tt_starting_log_segment; // new on 050204
#if TT_ALPHA_FEATURE
extern int tt_prevous_log_segment;
#endif
extern boolean tt_create_new_time_line;

extern xml_element *tt_zip_description_archive_element;

extern boolean tt_jump_to_current_log_segment;
extern boolean tt_jump_to_youngest_log_segment;

extern boolean tt_include_media_in_time_travel_archives;
//extern boolean tt_might_include_media_in_time_travel_archives;

extern boolean tt_ok_to_optimize_successive_demos;

extern int tt_max_number_of_time_lines;

extern boolean tt_titles_just_ended;

extern millisecond tt_log_initialization_start_time;

extern string tt_additional_command_line;

extern ascii_string tt_err_file_name;

extern long tt_cache_memory_max_size;

extern boolean tt_keep_all_time_travel_segments;

extern boolean tt_loading_to_undo_bammer;

extern boolean tt_time_travel_after_display_updated;

extern boolean tt_subtitles_and_narration_even_without_demo;

#if TT_ALPHA_FEATURE
extern boolean tt_right_to_left_text; // e.g. Hebrew
extern boolean tt_unicode_clipboard;
extern boolean tt_remove_playground_active_documentation;
extern boolean tt_scrolling_ok;
#endif

extern boolean tt_record_clock;

extern DWORD tt_real_time_pause_began;
extern millisecond tt_time_spent_paused;
extern DWORD tt_real_time_pause_ended;
extern millisecond tt_current_time_pause_ended;

extern string tt_file_name_clipboard_message;
extern boolean tt_add_sprite_when_on_floor_but_wait_a_frame;

//extern boolean tt_load_new_main_notebook_files;

extern city_coordinate tt_drop_files_x;
extern city_coordinate tt_drop_files_y;
extern int tt_drop_files_count;
extern string *tt_drop_file_names;
extern string *tt_drop_file_hash_names;

extern string tt_current_notebook_zip_archive;

extern int tt_make_current_exe_default;
//extern int tt_install_current_exe;

extern boolean tt_user_name_should_be_new;

extern boolean tt_include_descriptions_in_xml;

extern boolean tt_match_blank_with_sensor_then_run_once_per_cycle;

extern boolean tt_robot_drop_location_follows_camera;

extern boolean tt_emulate_mouse_for_full_screen_time_travel;

extern string tt_file_name_of_tt_object_just_saved;

extern boolean tt_reincarnate_tools;

class Vacuum;
extern Vacuum *tt_toolbox_vacuum;
class Copier;
extern Copier *tt_toolbox_copier;
class Expander;
extern Expander *tt_toolbox_expander;
class Notebook;
extern Notebook *tt_toolbox_notebook;

extern boolean tt_display_demo_subtitles;
extern boolean tt_play_demo_sound_files;
extern boolean tt_speak_demo_text;

//class Backgrounds;
//extern Backgrounds *tt_backgrounds_to_delete_next_frame;

extern Sprites *tt_nests_to_set_original_nest;

extern xml_document *tt_city_document_to_load;

extern boolean tt_number_to_text_exact;

extern boolean tt_stop_all_pictures;

extern boolean tt_shrink_and_grow_numbers;

extern int tt_notebook_id_counter;

extern string tt_saved_city_file_name_when_demo_ends;

extern Background *tt_next_background;

extern Sprites *tt_previous_controlee_replacements;

extern boolean tt_run_demos_despite_anomalies;

extern boolean tt_dragging;

extern boolean tt_ok_to_process_windows_messages;

extern boolean tt_remove_unneeded_things_from_thought_bubble;

extern boolean tt_loading_is_ok_to_abort;

extern int tt_sprites_loaded_count;
extern int tt_sprites_saved_count;
extern int tt_bytes_read_from_URL;

extern boolean tt_loading_vacuum_contents;

extern xml_document *tt_city_document_to_load_after_demo_ends;

extern boolean tt_standard_exit_started;

extern boolean tt_old_nest_merging_behavior;

extern boolean tt_describing_a_comment;

extern Picture *tt_function_key_help;

extern Sprite *tt_call_for_marty;
extern boolean tt_marty_appears_only_if_called;
extern boolean tt_dont_set_tt_loading_is_ok_to_abort;
extern millisecond tt_call_for_marty_wait_duration;

extern string tt_dump_city_to_file_name_when_titles_over;

extern boolean tt_expires;

#endif