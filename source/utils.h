// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.
#if !defined(__TT_UTILS_H)

#define __TT_UTILS_H

#if !defined(__TT_FLOOR_H)   // added here on 010202 since needed for ok_to_make_a_sound
#include "floor.h"
#endif

#if !defined(__TT_ZIP_H)   
#include "zip.h"
#endif

#include <time.h>

class UseProfile : public Entity {
  public:
	 UseProfile() :
		first_use(0),
		last_use(0),
		seconds_used(0),
		use_count(0) {
		};
	 time_t first_use, last_use, seconds_used;
	 short int use_count;
};

template <class VariableType> class UnwindProtect { // new on 080803
	// the following provides a way of setting a global variable only with the dynamic scope of the current block
	// even if there is a throw, the value is restored
	public:
		UnwindProtect(VariableType &variable) :
			variable(&variable),
			saved_value(variable) {
		};
		UnwindProtect(VariableType &variable, VariableType new_value) :
			variable(&variable),
			saved_value(variable) {
				variable = new_value;
		};
		~UnwindProtect() {
			*variable = saved_value;
		};
	protected:
		VariableType *variable;
		VariableType saved_value;
};

template <class VariableType> class UnwindProtectIf { // new variant of the above on 220903 since the alternative of using conditionals introduces a new scope
	// the following provides a way of setting a global variable only with the dynamic scope of the current block
	// even there is a throw the value is restored
	public:
		UnwindProtectIf(boolean enabled, VariableType &variable) :
			variable(&variable),
			saved_value(variable),
			enabled(enabled) {
		};
		UnwindProtectIf(boolean enabled, VariableType &variable, VariableType new_value) :
			variable(&variable),
			saved_value(variable),
			enabled(enabled) {
				if (enabled) {
					variable = new_value;
				};
		};
		~UnwindProtectIf() {
			if (enabled) {
				*variable = saved_value;
			};
		};
	protected:
		VariableType *variable;
		VariableType saved_value;
		boolean enabled;
};

boolean tt_set_screen_parameters(int video_mode);
boolean tt_setmode(int mode_number, boolean reset);
unsigned char video_mode_index(int video_mode); // changed to unsigned on 300702
int video_mode_number(int index);
void unclip(boolean tell_windows=TRUE);
//boolean clipped();
//void change_video_mode(int increment);

void initialize_history(boolean install_user_profile, boolean called_from_log=FALSE);
void read_history(InputStream *file, boolean install_user_profile, boolean called_from_log=FALSE, boolean using_default_user_profile=FALSE);
void resume_initialize_history();
long resume_initialize_history(InputStream *file, boolean called_from_log=FALSE);
void dump_history(boolean puzzle_state_changed=FALSE);
void dump_history(output_file_stream &file, boolean puzzle_state_changed=FALSE);

boolean favorites_set(FavoriteNames name);
void read_stuff_after_puzzle_state(InputStream *stream);

//void ansi_lower(string s);

//FavoriteSizeAndLocation *user_favorite_sizes_and_locations(boolean &already_set);

#if TT_WINDOWS
#if TT_DIRECTX
boolean initialize_sound();
void release_direct_sound();
LPDIRECTSOUND pointer_to_direct_sound();
//boolean create_sound_buffer(LPDIRECTSOUNDBUFFER *sound_buffer,
//                            short int channels, int samples_per_second,
//                            short int bits_per_sample, int buffer_size);
boolean write_to_sound_buffer(LPDIRECTSOUNDBUFFER sound_buffer, 
                              LPBYTE SoundData,
                              DWORD SoundBytes,
                              DWORD Offset=0);
boolean copy_file_to_sound_buffer(LPDIRECTSOUNDBUFFER sound_buffer,
                                  HFILE file, int size);
#endif

void play_sound(int id, boolean repeat=FALSE, boolean play_even_if_time_travel_paused=FALSE);// boolean play_if_tied=FALSE);
#if TT_DIRECTX
void resume_repeating_sound();
#else
void play_current_sound();
#endif
void play_sound_bytes(sound_buffer sound, 
#if !TT_32
							 long size, HGLOBAL memory_handle, 
#endif
							 int priority);
boolean play_sound_file(ascii_string file_name, int priority);

#if TT_DIRECTX
void turn_off_direct_sound();
void turn_on_direct_sound();
#endif

void turn_off_sound(boolean forever=TRUE);
void turn_on_sound(boolean forever=TRUE);

void stop_sound_id(int id);

boolean stop_sound(boolean narration_too);
//void pause_sound();
//void resume_sound();
#endif

//void tt_heap_overflow();

string copy_string(const_string source, int length=-1);
string copy_string_without_quotes(const_string source, int length=-1);

wide_string canonicalize_new_lines(wide_string source, int length=-1);
wide_string canonicalize_new_lines(string source, int length=-1);

#if TT_WIDE_TEXT
//wide_string copy_wide_string(wide_string source, int length=-1); moved on 310502

wide_string copy_wide_string(string source, int length=-1);
void copy_wide_string(string source, int source_length, wide_string destination);

string copy_narrow_string(wide_string source, int length=-1);
void copy_narrow_string(wide_string source, int source_length, string destination, int destination_length);
#else
inline string copy_wide_string(string source, int length=-1) {
	return(copy_string(source,length));
};
inline string copy_narrow_string(string source, int length=-1) {
	return(copy_string(source,length));
};
#endif

ascii_string copy_ascii_string(const_ascii_string source, int length=-1);
string append_strings(const_string a, const_string b);
string append_3_strings(const_string a, const_string b, const_string c);
ascii_string append_3_ascii_strings(const_ascii_string a, const_ascii_string b, const_ascii_string c);
ascii_string append_ascii_strings(const_ascii_string a, const_ascii_string b);

wide_string append_strings(wide_string a, wide_string b); // new on 311002

void sleep(millisecond duration);

#if FAST_GRAPH
#include <fastgraf.h>
inline void generic_move(int new_x, int new_y) {
	fg_move(new_x,new_y);
};

inline void generic_moverel(int new_x, int new_y) {
   fg_moverel(new_x,new_y);
};

inline void generic_drwimage(string image, int width, int height) {
   fg_drwimage(image,width,height);
};
#endif

#if GRAPHICS_TO_MEMORY
//extern int flash_graphics_mark_x;
//extern int flash_graphics_mark_y;

void generic_move(int new_x, int new_y);
void generic_moverel(int new_x, int new_y);
//void generic_drwimage(string image, int width, int height);
#endif

long grow_value(long old_value, millisecond delta, millisecond double_every=1000);
long shrink_value(long old_value, millisecond delta, millisecond double_every=1000);
Direction direction(city_coordinate delta_x, city_coordinate delta_y);
Direction dampen_turn(Direction new_direction, Direction current_direction);

boolean rubout(char key);

#if !TT_STRINGS_STATIC
HINSTANCE load_library(ascii_string library_name, boolean warn=TRUE);
void load_string_library(boolean alternate=FALSE);
#endif
void unload_string_library();
HINSTANCE resource_source(boolean alternate=FALSE);
const_string S_maybe_capitalize_first(int id, boolean capialize, string default_string=NULL);
const_string S2_capitalize_first(int id, int index=-1);
const_string S_capitalize_first(int id, string default_string=NULL);
string SC(int id, int index=-1);
const_string S2(int id, int index);
const_ascii_string S2(int id, const_string suffix);
const_string S(int id, string default_string=NULL, boolean alternate=FALSE);
const_string C(int id);
const_string C2(int id, int index);
#if TT_UNICODE
const_ascii_string AS(int id, boolean null_ok=FALSE);
const_ascii_string AC(int id);
const_ascii_string AS2(int id, const_string suffix);
const_ascii_string AS2(int id, int index);
ascii_string ASC(int id, int index=-1);
#else
inline const_ascii_string AS(int id, string default_string=NULL) {
	return(S(id,default_string));
};
inline const_ascii_string AC(int id) {
	return(C(id));
};
inline const_ascii_string AS2(int id, ascii_string suffix) {
	return(S2(id,suffix));
};
inline const_ascii_string AS2(int id, int index) {
	return(S2(id,index));
};
inline ascii_string ASC(int id, int index=-1) {
	return(SC(id,index));
};
#endif
NaturalLanguage current_language();
NaturalLanguage spoken_language();
boolean english();
boolean alternative_language_spoken();
void set_country_code(ascii_string name, boolean alternate); // exported on 131203

//void by_completed(Sprite *by);
void completed(Sprite *robot);
void add_completion(Robot *robot);
void action_aborted(Sprite *robot);
void reset_action_status(Robot *robot);
boolean is_completed(Robot *robot);
boolean is_action_aborted(Sprite *robot);
boolean is_action_in_progress(Robot *robot);

//void completed(ActionStatus *action_status);
//void add_completion(ActionStatus *action_status);
//void action_aborted(ActionStatus *action_status);
////void completed_after(millisecond duration, ActionStatus *action_status, Sprite *sprite);
//void reset_action_status(ActionStatus *action_status);
//boolean is_completed(ActionStatus *action_status);
//boolean is_action_aborted(ActionStatus *action_status);
//boolean is_action_in_progress(ActionStatus *action_status);

int my_random(int n);
int my_random2(int n);
int my_random_unless_tt_use_spoken_alternative(int n);
unsigned long random_number_seed();
void set_random_number_seed(unsigned long new_seed);

sound_buffer read_sound(HFILE file, long size
#if !TT_32
	, HGLOBAL &memory_handle
#endif
	);
#if TT_DIRECTX
sound_buffer previously_loaded_sound(int index);
sound_buffer load_sound_buffer(int sound_index, unsigned long &size);
sound_buffer load_sound_from_file(ascii_string file_name, int &index);
void release_user_sound_table();
sound_buffer load_sound_from_memory(bytes sound_bytes);
void play_sound_buffer(sound_buffer sound);
boolean sound_buffer_playing();
void stop_sound_buffer(sound_buffer sound);
void release_sound_buffer(sound_buffer sound);
#endif

short unsigned int next_serial_number();

int increment_ini_int(const_ascii_string category, const_ascii_string id, boolean record);
ascii_string ini_entry(const_ascii_string category, const_ascii_string id, boolean record=TRUE);
int ini_int(const_ascii_string category, const_ascii_string id, boolean record=TRUE, int default_value=0);
void write_ini_entry(const_ascii_string category, const_ascii_string id, const_ascii_string value);
void write_ini_entry(const_ascii_string category, const_ascii_string id, int value);

ascii_string new_file_name(const_ascii_string part1, const_ascii_string extension=NULL);
string extract_file_name_from_link(string file_name);
boolean already_has_extension(string file_name, string extension, boolean check_links_too=TRUE);
boolean already_has_extension(string file_name, string *extensions, int extension_count);
ascii_string existing_file_name(FileNameStatus &file_status, const_ascii_string short_file_name, 
										  string extension=NULL, const_ascii_string subdirectory=NULL,
										  boolean cache_if_url=TRUE, boolean ok_to_bother_user=TRUE,
										  boolean language_specific_subdirectory=FALSE, boolean just_narration=FALSE,
										  boolean check_paths=TRUE, int call_depth=1);
ascii_string existing_file_name(FileNameStatus &name_status, const_ascii_string original_file_name, 
										  string *extensions, int extension_count, // this version checks multiple extensions
										  const_ascii_string subdirectory=NULL,
										  boolean cache_if_url=TRUE, boolean ok_to_bother_user=TRUE,
										  boolean language_specific_subdirectory=FALSE, boolean just_narration=FALSE,
										  boolean check_paths=TRUE, int call_depth=1);
boolean copy_file_to_toontalk_directory(const_ascii_string full_file_name, const_ascii_string sub_directory, const_ascii_string short_name, const_ascii_string extension=NULL);
ascii_string copy_to_temp_directory(const_ascii_string source_name, const_ascii_string original_name);
boolean copy_system_file_to_directory(const_ascii_string short_file_name, const_ascii_string sub_directory, const_ascii_string destination_directory);
boolean launch_help(const_ascii_string short_htm_file_name);
int message_box(UINT style, const_string message1, const_string message2=NULL, boolean restore_toontalk_afterwards=TRUE, string trouble_shooting_ini_key=NULL);
void set_touched_a_remote(ChangeFrequency new_frequency);
void print_binary_number(output_stream &message, long value, int seperator_every=8, char seperator=' ', boolean leading_zeros=TRUE);
boolean white_space_only_at_ends(const_string text, int length); // exported as of 010704
Sprite *string_to_sprite(const_string text);
boolean can_be_rational_number(string text, int length=-1);
void release_rational_number_for_string_testing();
HGLOBAL uuencode(const_string encoding, long encoding_length, SpriteType type=NONE_GIVEN);
Sprite *uudecode_sprite(const_string encoding_string, boolean original_language=TRUE, 
								boolean ok_for_string_to_be_text_or_number=TRUE, boolean floors_or_houses_ok=TRUE);
string uudecode(const_string encoding_string, long &length, NaturalLanguage &language, int &version_number);
Sprite *encoding_to_sprite(bytes encoding, int encoding_size, int version_number, NaturalLanguage language);
void print_clipboard_explanation(output_stream &stream, SpriteType type, string description=NULL, int description_length=0);
HGLOBAL global_alloc(UINT fuAlloc, DWORD cbAlloc);
BOOL out_of_heap(size_t );
city_coordinate compute_edge_size(city_coordinate plate_width, city_coordinate plate_height);
city_coordinate compute_edge_size_given_text_size(city_coordinate text_width, city_coordinate text_height);
void display_plate_borders(city_coordinate llx, city_coordinate lly,
								   city_coordinate plate_width, city_coordinate plate_height,
								   PlateState plate_state, bytes color_permutation);
void display_two_plate_borders(city_coordinate llx, city_coordinate lly,
										 city_coordinate plate_width, city_coordinate plate_height,
										 PlateState plate_state, bytes color_permutation);
//long display_plated_text(string text, long text_length, 
//								 city_coordinate llx, city_coordinate lly,
//								 city_coordinate plate_width, city_coordinate plate_height,
////								 city_coordinate width, city_coordinate height,
//								 PlateState plate_state,
//								 boolean plate_visible,
//								 boolean text_visible,
//                         color_index text_color,
//								 bytes color_permutation);
//long display_plated_wide_text(wide_string text, long text_length, 
//								 city_coordinate llx, city_coordinate lly,
//								 city_coordinate plate_width, city_coordinate plate_height,
////								 city_coordinate width, city_coordinate height,
//								 PlateState plate_state,
//								 boolean plate_visible,
//								 boolean text_visible,
//                         color_index text_color,
//								 bytes color_permutation);
void display_plate(city_coordinate llx, city_coordinate lly,
						 city_coordinate plate_width, city_coordinate plate_height,
						 PlateState plate_state,
						 boolean blank,
						 bytes color_permutation); // new on 200901

MatchStatus match(Sprite *answer_sprite, Sprite *goal_sprite, int version);
MatchStatus same_type_match(SpriteType type, Sprite *item, Sprite *other,
                            SpritePointerPointer &suspension_cell, int version, int index=-1); // -1 is default that is overridden when cubby is the leader

void allocate_resource_strings();
void deallocate_resource_strings();
boolean being_dumped_together(Sprite *x, Sprite *y);

void do_when_not_city_stopped(Sprite *sprite, AnimationCallBack callback);
void stop_all(boolean on);
//void release_not_tt_stop_all_callbacks();
#if TT_POST326
void remove_from_tt_pictures_to_activate(Picture *picture);
#endif

void print_as_comment(java_stream &out, const_string comment, int max_length);
void java_indent(java_stream &out);

boolean ok_to_make_a_sound(Background *floor);

void initialize_alphabet();
void release_alphabet();
char next_in_alphabet(char letter, long change=1);
#if TT_WIDE_TEXT
wide_character next_in_alphabet(wide_character letter, long change=1);
#endif
NaturalLanguage string_to_language(ascii_string name, boolean alternate);
void write_cookie(output_stream &stream);
boolean valid_cookie(string cookie);
boolean read_cookie(InputStream *stream, 
                    character &cookie_version_number, 
                    NaturalLanguage &language,
                    ascii_string file_name=NULL);
boolean process_cookie(string cookie,
							  character &cookie_version_number, NaturalLanguage &language,
							  ascii_string file_name=NULL);
boolean text_to_speech();
boolean limited_text_to_speech();
short int return_use_count();

java_string ensure_file_for_java(int id, SpriteType type);
java_string ensure_file_for_java(ascii_string very_short_name, SpriteType type, boolean check_cdrom); // added declaration on 120201
java_string gif_file_for_java(const_ascii_string bmp_file_name);
java_string au_file_for_java(const_java_string wave_file_name);
boolean is_relative_path(string name);
ascii_string basic_file_name(const_ascii_string full_name, boolean include_extension=FALSE);
int start_of_basic_file_name(const_ascii_string full_name);
ascii_string file_name_without_extension(const_ascii_string full_name);
ascii_string add_or_replace_extension(const_ascii_string full_name, const_ascii_string new_extension);
int extension_start_position(const_ascii_string full_name);
int first_extension_start_position(const_ascii_string full_name);
boolean execute(string program, string command_line=NULL, boolean quote_command_line=TRUE);

string quote_file_name(string file_name, string quoted_file_name=NULL);

//long copy_lz_file(const_ascii_string source_name, const_ascii_string destination_name);

long get_file_size(HFILE file);

Picture *make_indirect_picture(Sprite *item, boolean add_too=TRUE);
void set_relative_size_and_location(Sprite *other_follower, Sprite *other_leader, Sprite *follower, Sprite *leader);
Sprites *sort_items(Sprites *followers);

void trouble_shoot(Trouble trouble, const_string extra_message=NULL, const_string extra_message2=NULL, 
						 boolean restore_toontalk_afterwards=FALSE, boolean now=FALSE, boolean trouble_shoot_regardless=FALSE);

int about_to_dump(Sprite *sprite, output_stream &stream);
void release_sprites_being_dumped();

void just_copied(Sprite *original, Sprite *copy);
Sprite *already_copied(Sprite *original);
boolean is_a_current_copy(Sprite *copy);
boolean copied_bird_has_as_original_nest(Sprite *nest);
void release_items_copied();
Sprites *save_copied_items();
void restore_copied_items(Sprites *items);

#include <sys\timeb.h>

inline time_t seconds_since_1970() {
	// re-written on 160301 since StartTT no longer corrects for time zones and 16-bit version isn't supported
	return(time(NULL));
  // for some reason the 16 bit and 32 bit versions believe they are in different time zones
  // this corrects for the problem
//   timeb time_struct;
//   ftime(&time_struct);
//   return(time_struct.time-time_struct.timezone*60);
};

boolean trouble_shooting();

void write_user_parameters(output_stream &file, Parameters *parameters);
void read_user_parameters(InputStream *file, Parameters &parameters, boolean using_default_user_profile=FALSE);


void leave_room_for_growth(output_stream &stream, int number_of_bytes);
unsigned long ignore_growth(InputStream *stream, int number_of_bytes);

void dump_if_not_null(output_stream &stream, Sprite *sprite);
void dump_string(output_stream &stream, string text, long max_size);
void load_string(InputStream *stream, string text, long max_size); 

void load_started();
void load_ended();

void dump_started();
void dump_ended();
#if TT_WIDE_TEXT
wide_string update_string(wide_string text, unsigned_character key, boolean extended, long max_length=255);
#endif
string update_string(string text, unsigned_character key, boolean extended, long max_length=255);

void copy_alphanumerics(ascii_string from, ascii_string to, long max_size=max_long);
void keep_only_alphanumerics(ascii_string text);
void set_houses_to_a_block(int houses_to_a_block);
void put_on_clipboard(Sprite *sprite, Background *floor=NULL, Sprite *appearance=NULL, char key=0); 
Sprite *read_from_clipboard(boolean more_than_one_ok=TRUE, boolean check_files_too=TRUE);
void add_item_from_clipboard(Background *floor, Sprite *appearance, char key);
//void add_items_from_outside(Sprites *items, Background *floor, Sprite *appearance, city_coordinate x=0, city_coordinate y=0);
void add_item_from_outside(Sprite *item, Background *floor, Sprite *appearance, city_coordinate x=0, city_coordinate y=0, boolean change_to_good_size=TRUE);
Sprite *sprites_from_drop_handle(HDROP drop_handle, boolean more_than_one_ok);
Sprite *sprites_from_drop_globals();
boolean has_picture_extension(string full_file_name);
boolean has_sound_extension(string full_file_name);
Sprite *sprite_from_file_name(string full_file_name, boolean &aborted, string original_file_name=NULL, string *local_file_name_if_different=NULL);
Sprite *sprite_from_file_name1(string full_file_name, boolean &aborted, string original_file_name=NULL, 
															 string *local_file_name_if_different=NULL);
void run_queue();
void save_clipping_to_file(HGLOBAL clipping, string suffix);
void save_clipping_to_file_name(HGLOBAL clipping, string file_name);
#if TT_DYNA_ZIP
boolean update_private_media_directory(string archive);
void save_media_files_in_archive(string archive, string extra_media_file_names=NULL, boolean for_a_document=FALSE);
#else
boolean update_private_media_directory(unzFile archive);
void save_media_files_in_archive(zipFile archive);
#endif
void include_media_files_in_zip_description_archive_element(string archive);
string media_file_names(Sprites *sprites_with_media_to_save, string old_relative_media_file_names=NULL);

#if TT_UNICODE
ascii_string u2a(const_string s);
string a2u(const_ascii_string s);
ascii_string copy_unicode_to_ascii(const_string s, int length=-1);
string copy_ascii_to_unicode(const_ascii_string s, int length=-1);
void copy_ascii_to_unicode(const_ascii_string s, string result, int string_size);
#else
// following don't allocate while above does...
inline ascii_string u2a(const_string s) {
	return(s);
};
inline string a2u(const_ascii_string s) {
	return(s);
};
inline ascii_string copy_unicode_to_ascii(const_string s, int length=-1) {
	return(copy_string(s,length));
};
inline string copy_ascii_to_unicode(const_ascii_string s, int length=-1) {
	return(copy_string(s));
};
inline void copy_ascii_to_unicode(const_ascii_string s, string result, int string_size) {
	strcpy(result,s);
};
#endif
#if TT_32
ascii_string last_error();
void message_box_last_error();
#endif
//inline boolean unicode_version(int version) {
//	return(version >= first_version_with_unicode);
//};
//string read_old_string(InputStream *pad_in, int length);
//#if TT_WIDE_TEXT
//wide_character widen(character c);
//#else
//inline wide_character widen(character c) {
//	return(c);
//};
//#endif
boolean need_wide_characters(NaturalLanguage language=tt_language);
city_coordinate default_talk_balloon_character_width();
city_coordinate default_talk_balloon_character_height();

string maintain_n_versions_youngest_last(int max);
void maintain_n_versions_youngest_first(ascii_string file_name, int max, boolean should_contain_dash);
string file_name_renumbered(string file_name, long number, string file_name_to_use=NULL);
string next_file_name(string file_name, boolean &name_is_new, boolean should_contain_dash=TRUE);
void save_oldest_and_current_version_numbers(int current_version, int oldest_version=-1, int youngest_version=-1);

void initialize_keyboard_accelerators();
void release_keyboard_accelerators();
boolean keyboard_accelerator(char key, KeyboardAccelerator accelerator);
character upper_case_character(character c);
character lower_case_character(character c);
string space(SpaceBetweenWords context=DEFAULT_SPACE_BETWEEN_WORDS);
string comma();
string end_sentence(character terminator='.');

void write_string(output_stream &log_out, string s, long len=-1);
string read_string(input_stream &log_out, string s=NULL);
string read_string_old(input_stream &log_out, string s=NULL);
boolean is_url(string name);

city_coordinate grow_width_to_640x480_screen_size(city_coordinate w);
city_coordinate grow_height_to_640x480_screen_size(city_coordinate h);
city_coordinate shrink_width_from_640x480_screen_size(city_coordinate w);
city_coordinate shrink_height_from_640x480_screen_size(city_coordinate h);
int shrink_percent_from_640x480_screen_size(int percent);

int last_space_or_punctuation(wide_string text, int length=-1, boolean or_ascii_character=FALSE);
int last_space_or_punctuation(ascii_string text, int length=-1, boolean or_ascii_character=FALSE); // last arg ignored
boolean space_or_punctuation(wide_character *characters, int index, int length);
boolean phrase_ending_punctuation(character *characters, int index, int length, boolean commas_too=TRUE, boolean close_parens_too=TRUE);
boolean phrase_ending_punctuation(wide_character *characters, int index, int length, boolean commas_too=TRUE, boolean close_parens_too=TRUE);
wide_string remove_spaces_if_non_ascii_neighbors(wide_string text, int old_length, int &new_length);
int max_subtitle_length();
int count_sentences(string text, int length);
int end_of_nth_sentence(int sentence_count, wide_string wide_text, int length);

city_coordinate minimum_character_width();
city_coordinate minimum_character_height();

wide_string remove_parenthetical_remark(wide_string text);
void remove_extra_spaces(string text);
boolean file_exists(string name, boolean might_be_url=TRUE);
int multi_byte_length(wide_string wide_text, int length=-1);
boolean contains_non_ascii(wide_string wide_text, int wide_length=-1);
boolean control_down();
wide_string short_file_name(wide_string path, boolean leave_extension=FALSE);
ascii_string short_file_name(ascii_string path, boolean leave_extension=FALSE);
int short_file_name_start(ascii_string path);
char line_terminator();
boolean virtual_left_button(char button_status, boolean for_a_sensor=TRUE);
boolean virtual_middle_button(char button_status, boolean for_a_sensor=TRUE);
boolean virtual_right_button(char button_status, boolean for_a_sensor=TRUE);

void set_spoken_language_from_string(string language_name);
void set_spoken_language(NaturalLanguage language);
void set_language(NaturalLanguage language);

void replace_character(string s, char search, char replace);
void print_spaces(int stop, output_stream &stream);
boolean handle_internal_error(string message, boolean can_recover);
boolean toolbox_tool(Sprite *sprite);

//void postpone_new_logs();
//void stop_postponing_new_logs();
void oldest_and_yongest_versions(); // int &oldest, int &youngest);
time_t return_last_use();

zipFile log_out_archive();

#if TT_ALPHA2
unzFile log_in_archive(boolean time_travel_archive_regardless=FALSE);
#else
unzFile log_in_archive();
#endif

void initial_zip_description_archive_element();
void release_zip_description_archive_element();
boolean read_counters_from_archive();

void print_int64(int64 i, output_stream &stream);
void truncate(string file_name, int length);
boolean rename_file_to_backup(string file_name);

void compute_tt_file_name(); 
boolean set_tt_user_name(string new_name);
string new_title_for_window(string name);

void delete_strings(string *strings, int count);

void output_last_error(int error_code, output_stream &stream);
void get_my_documents_folder(string path); 

void release_directory_names();
void set_directory_names();
string process_directory_entry(string entry);

void set_ini_file(string ini_file);

boolean ok_to_delete_bammed(Sprite *by);
void replace_carriage_returns_with_spaces(string text, int length);
boolean cycle_background_color();

void compute_local_ini_file_name(string file_name);
void initialize_tt_file_search_path();
void release_file_search_path();

boolean interesting_error(int last_error);
#if TT_DEBUG_ON
boolean add_to_log_if_interesting_error(int ignore_number=min_long, string message=NULL, int ignore_error_also=min_long);
void set_previous_last_error(int n);
#endif

DWORD compute_memory_available(boolean just_virtual_memory=TRUE);
void show_puzzles_over_html();

string canonicalize_path(string path);
string expand_asterisk(string directory);

color compute_color(int index, bytes permutation);
//color alter_color(color color, int *changes);
color alter_color24(color color, int *changes);
//color alter_color32(color color, int *changes);
//color16 alter_color15(color16 color, int *changes);
color16 alter_color16(color16 color, int *changes);
color16 make_color16(byte red, byte green, byte blue);
color16 color_to_color16(color color);
color16 near_black16();
string picture_file_name_of_ini_file(ascii_string ini_file_name, string entry, string full_entry);

void print_integer_with_commas(long value, output_stream &text_out);

boolean off_screen(city_coordinate llx, city_coordinate lly, city_coordinate adjusted_width, city_coordinate adjusted_height);

int digit_to_number(unsigned char key, int base);

boolean all_characters_are(char character, string text);

inline boolean absolute_mouse_mode() {
   return(tt_mouse_mode == ABSOLUTE_MOUSE_MODE && !tt_hand_needs_to_move_relatively);
};

inline boolean relative_mouse_mode() { // not negation of above since can also be direction pad mode
   return(tt_mouse_mode == RELATIVE_MOUSE_MODE || tt_hand_needs_to_move_relatively);
};

void set_absolute_mouse_mode(int mode);

unsigned char name_to_character(string encoding, boolean &extended_key);

void note_mouse_state_for_dragging();

unsigned char old_dispdib_value();
unsigned char current_dispdib_value();
void set_old_dispdib_value(unsigned char value);
inline boolean exclusive_screen_desired() {
   return(tt_exclusive_screen_desired && !tt_restore_exclusive_screen);
};

void interpret_command_line(int argc, char *argv[]); //, boolean from_user);
NaturalLanguage process_command_line(string command_line, int &command_line_count, ascii_string commands[]);
void interpret_command_string(string command_string); // new on 300502

string show_html_dialog_named_in_ini_file(string key, string to_dialog=NULL);

inline int get_red_from_color16(color16 color) { // inlined (and moved here) on 130602
	return((byte) ((color>>(tt_green_bits_per_pixel+tt_blue_bits_per_pixel))<<(8-tt_red_bits_per_pixel)));
};

inline int get_green_from_color16(color16 color) {
	return((byte) (((color<<tt_red_bits_per_pixel)>>(tt_red_bits_per_pixel+tt_blue_bits_per_pixel))<<(8-tt_green_bits_per_pixel)));
};

inline int get_blue_from_color16(color16 color) {
	return((byte) ((color<<(16-tt_blue_bits_per_pixel)>>(16-tt_blue_bits_per_pixel))<<(8-tt_blue_bits_per_pixel)));
};

inline color16 make_color16(byte red, byte green, byte blue) {
	// args range from 0 to 255
//#if TT_DEBUG_ON
//	int new_red = (red>>(8-tt_red_bits_per_pixel));
//	new_red = new_red<<(tt_green_bits_per_pixel+tt_blue_bits_per_pixel);
//#endif
	return(((red>>(8-tt_red_bits_per_pixel)))<<(tt_green_bits_per_pixel+tt_blue_bits_per_pixel))|
		    ((green>>(8-tt_green_bits_per_pixel))<<tt_blue_bits_per_pixel)|
			 (blue>>(8-tt_blue_bits_per_pixel));
};

// and now special versions for 565 red is 5 bits, green 6, blue 5

inline int get_red_from_color565(color16 color) { // inlined (and moved here) on 130602
	return((byte) ((color>>11)<<3));
};

inline int get_green_from_color565(color16 color) {
	return((byte) (((color<<5)>>10)<<2));
};

inline int get_blue_from_color565(color16 color) {
	return((byte) (((color<<11)>>11)<<3));
};

inline color16 make_color565(byte red, byte green, byte blue) {
	return(((red>>3)<<11)|
		    ((green>>2)<<5)|
			 (blue>>3));
};

//#if TT_XML
//void print_string_as_xml_string(string text, output_stream &stream);
//#endif

inline boolean new_line(char c) { // new on 080802 since if coming from XML '\n' will be there but not if typed to a text pad
   return(c == '\r' || c == '\n');
};

inline boolean new_line_wide(wide_character c) { // new on 080802 since if coming from XML '\n' will be there but not if typed to a text pad
   return(c == '\r' || c == '\n');
};

int extract_base(string text, int &base);
int extract_base(wide_string text, int &base);

inline boolean dumping_an_individual() { // abstracted on 180103
	return(tt_dump_reason < DUMPING_INDIVIDUAL);
};

string hash_to_string(hash hash_bits);
void hash_to_string(hash hash_bits, string hash_string);
hash string_to_hash(string hash_string);
void hash_to_wide_string(hash hash_bits, wide_string hash_string);

void media_file_name(hash hash_bits, string extension, string file_name, string directory=NULL, int directory_length=0);
void media_file_name(string hash_string, string extension, string file_name, string directory=NULL, int directory_length=0);

//#if TT_GMP_VERSION_4_1
//void mpq_canonicalize_safely(rational x);
//#else
//inline void mpq_canonicalize_safely(rational x) {
//	// not needed in 3.1
//	mpq_canonicalize(x);
//};
//#endif

inline boolean time_travel_enabled() { // new on 150103
	// note if GenerateLogs is 0 then the following will be as well
	return(tt_time_between_new_logs > 0);
};

#if TT_DYNA_ZIP
class counters {
public:
	counters(int current_version=-1, int oldest_version=-1, int youngest_version=-1) :
		current_version(current_version),
		oldest_version(oldest_version),
		youngest_version(youngest_version) {
	};
	// following are public
	int current_version;
	int oldest_version;
	int youngest_version;
};
#endif

Sprite *included_sprite(int index);
void remember_included_sprite(int index, Sprite *sprite);
void release_included_sprites();
void reset_included_sprites();

void associate_element_and_include_location(xml_element *element, int location);
xml_element *existing_element_at_include_location(int location);
void release_included_elements();
void reset_included_elements();

class PostponedIncludeFileXML {
   public:
      PostponedIncludeFileXML() { // to be filled in before use...
      };
      int location;
      xml_element *element;
};

void postpone_include_file_xml(int location, xml_element *element);
void release_postponed_include_file_xml();
void do_postponed_include_files_xml();
void schedule_next_postponed_include_file_xml();
void do_next_postponed_include_file_xml(int index=-1);
void update_counters();

void print_guid_as_hexadecimal(GUID *guid, output_stream &stream);
Sprite *register_guid(GUID *guid, Sprite *sprite);
void deregister_guid(GUID *guid);
Sprite *find_sprite(GUID *guid);
void initialize_sprite_table();
void release_sprite_table();
void guid_as_hexadecimal(GUID *guid, wide_string guid_string);
void guid_as_narrow_hexadecimal(GUID *guid, string guid_string);
void hexidecimal_as_guid(wide_string guid_string, GUID *guid);

inline boolean replaying() {
//	return(tt_replay != NO_REPLAY && tt_replay != REPLAY_REPRODUCE_TIMING_ON_NEXT_FRAME);
	return(tt_replay == REPLAY_IGNORE_TIMING || tt_replay == REPLAY_REPRODUCE_TIMING);
	// || tt_replay == REPLAY_REPRODUCE_TIMING_ON_NEXT_FRAME); 
	// rewritten on 300104 and then on 200304 added tt_replay == REPLAY_REPRODUCE_TIMING_ON_NEXT_FRAME since otherwise during replay can use wrong user name
	// and removed again on  020404 since broke some demos and is the wrong modularity
};

void set_replay(ReplayState new_state);

int non_failing_getline(input_stream &stream, string buffer, int max_characters_to_read);

//#if TT_ALPHA_FEATURE
string extract_file_from_archive(string relative_file_name, string archive_file_name, string destination_directory=tt_extracted_file_temp_directory, boolean relative_file_name_is_unique=FALSE);

boolean delete_all_files_in_directory(string directory);

void add_new_media_file_names(string new_names, string &old_names);

void set_recordable_ini_options();

boolean need_to_display_insertion_caret(int insertion_point);
boolean reachable_from_remote_looks(Sprite *possible_remote_looks, Sprite *sprite);

void set_toolbox_notebook(Notebook *new_notebook);
void set_toolbox_expander(Expander *tool);
void set_toolbox_vacuum(Vacuum *tool);
void set_toolbox_copier(Copier *tool);
void initialize_toolbox_expander(boolean set_size_to_favorite=FALSE);
void initialize_toolbox_vacuum(boolean set_size_to_favorite=FALSE);
void initialize_toolbox_copier(boolean set_size_to_favorite=FALSE);
void initialize_toolbox_notebook(boolean set_size_to_favorite=FALSE);
Notebook *pointer_to_toolbox_notebook();
boolean toolbox_tool(Sprite *sprite);
boolean not_friend_of_tooly(Sprite *sprite);
void release_toolbox_tools();
string bignum_to_string(string narrow_text, int base, bignum_pointer bignum_value, int total_text_size=-1);
boolean set_bignum_value_to_string(bignum_pointer bignum_value, string text, int base);
void unable_to_write_file(string file_name);
void unable_to_read_file(string file_name, string extra_message=NULL);
//output_stream & tt_error_file(); -- moved to defs.h since needed earlier
void release_error_string_buffer();
boolean string_length_less_than(string s, int length);
int find_separator_location(wide_string wide_text, int length, wide_string separators);
boolean unnamed_user();
boolean is_digit(wide_character c);
boolean is_zip_cookie(string cookie);

#if TT_LOADING_AND_SAVING_INTERRUPTABLE
void ask_response_to_load_interruption();
void ask_response_to_save_interruption();
boolean ask_response_to_copy_url_interruption(int bytes_read);
#endif

boolean copy_string_to_file(string value, long string_length, string file_name);

boolean any_trucks_outside();
#if TT_FUNCTION_KEY_HELP
void update_function_key_help();
#endif

//boolean current_wide_text_with_insertion_character(int insertion_point, wide_string wide_text, int text_length, 
//																	  wide_string &output_text, int &output_text_length);

//#endif

//boolean assign_sprite(SpritePointer x, SpritePointer y);

//#if TT_WATCOM
// seems to be missing
//inline int random(int n) {
//  return(rand()%n);
//};
//#endif

/*
int get_int(ifstream &stream);
long get_long(ifstream &stream);
void put_int(int, output_file_stream &stream);
void put_long(long, output_file_stream &stream);
*/
#endif
