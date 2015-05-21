// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.
#if !defined(__TT_LOG_H)

#define __TT_LOG_H

class EventRecord {
  public:
	  EventRecord() {};
	  EventRecord(char button_status, coordinate delta_x, coordinate delta_y,
					  millisecond duration, unsigned char key, boolean extended_key,
					  boolean more) :
		 button_status((flag) button_status),
		 more((flag) more), // could try to pack this with the above
		 delta_x((short int) delta_x), // to save room
		 delta_y((short int) delta_y),
		 duration(duration),
		 key(key),
		 extended_key((flag) extended_key) {};
	 char button_status;
	 flag more;
	 short int delta_x, delta_y;
	 millisecond duration;
	 unsigned char key;
	 flag extended_key;
};

class Parameters : public Entity {
  public:
	 Parameters();
	 void install_parameters(long last_use);
//  private: // public for more general use
	 char video_mode;                    // -v
	 char programmer_head;               // -h
	 character user_name[256];           // -n
	 short int city_size;                // -c
	 short int character_size_percent;   // -l
	 short int help_speed_percent;       // -r
	 char mouse_button_count;            // -b
	 char maintain_history;              // -u
	 char minimum_say_importance;        // -a
	 char serial_port_to_open;           // -p
	 char user_is_debugger;              // -k
	 char temporary_user;                // -t
	 char sound_option;                  // -s
	 unsigned short int sleep;           // -y
	 unsigned short int speed;           // -q   
	 char using_dispdib;                 // -f
    char marty_talk;                    // -speak and -balloon
	 char load_items_version_number;  
    short int puzzle_counter;           // like -puzzle <n> -- used for logs to know which puzzle to start from
//	 short int expansion2;
	 short int xml_version;		 // new on 170803 to indicate use of XML for saving puzzle state
//	 short int expansion3;
	 short int expansion4;
    // If I ever run out of space I could shorten the 256 characters devoted to the user_name
};

boolean log_in_is_open();
void log_initialization(ascii_string log_file);
void dump_city_to_log();
void load_city_from_log();
void log_time(millisecond millisecond_delta); // removed short unsigned int on 130204
void log_events_counter(unsigned short int counter);
void log_user(unsigned char key, boolean extended_key,
				  boolean event,
				  city_coordinate delta_x, city_coordinate delta_y,
				  city_coordinate mouse_delta_x, city_coordinate mouse_delta_y,
				  char mouse_button_status, char joystick_button_status, long duration, boolean more, unsigned char second_byte);
void log_finalization();
void reopen_log_in(long start);
boolean open_log(ascii_string log_file, output_stream &stream, boolean ok_if_doesnt_exist=FALSE, 
					  boolean warn_if_doesnt_exist=TRUE, boolean first_time=TRUE, boolean may_include_opening_credits=FALSE);
//boolean replayed();
boolean narrating_demo();
ascii_string replay_file_name();
boolean set_replay_file_name(ascii_string file_name);
millisecond replay_clock(boolean accumulate=FALSE);
void synchronize_narration(millisecond millisecond_delta);
void replay_events_counter(unsigned short int &counter);
boolean replay_user(boolean &event,
						  unsigned char &key,
						  boolean &extended_char,
						  city_coordinate &delta_x,
						  city_coordinate &delta_y,
						  city_coordinate &mouse_delta_x, 
						  city_coordinate &mouse_delta_y,
						  char &mouse_button_status, char &joystick_button_status,
                    millisecond &duration,
						  unsigned char &second_byte);
void synchronize_subtitles();

void remove_rerecorded_segment(int segment_number);

boolean jump_to_log_segment(int version_number, boolean possibly_successive=FALSE);
boolean close_input_log(boolean normal, boolean run_next_demo, long version_number=-1, boolean possibly_successive=TRUE);
ascii_string add_demo_extension(const_ascii_string log_file_name, boolean input);
ascii_string add_speak_extension(const_ascii_string log_file_name, boolean input);
void new_speak();
void speak_now();
void fetch_next_speak();
boolean play_next_sound_file();
void release_next_speak();
void release_previous_speak();
void initialize_speak_starts();
void save_speak_starts(boolean normal);
void stop_replay();

void release_subtitles();

void record_no_clipboard_change();
void record_clipboard(string s, long size);
ClipboardTokens replay_clipboard_token();
string replay_clipboard_string();
void record_clipboard_files(unsigned int file_count);
Sprite *replay_clipboard_file_names();
void ignore_replay_clipboard_file_names();

void record_ini_entry(string entry);
void record_string(string s);
void record_ini_int(unsigned int entry);
string replay_ini_entry();
unsigned int replay_ini_int();
void record_special_events();
void replay_special_events();
string log_file_name(string file_name, string extension_including_dot_if_missing="");

class Notebook;
//void record_notebook(Notebook *notebook); // noticed obsolete 280204
Notebook *replay_notebook();
millisecond close_log_and_open_next();
void log_first_segment(boolean flag);

void time_travel(TimeTravelState state);
boolean prepare_to_time_travel();
void time_travel_react_to_mouse_down(city_coordinate x, city_coordinate y);
void release_time_travel_buttons();
//void add_time_travel_buttons();
void display_time_travel_buttons();
boolean display_time_travel_cursor();
void undisplay_time_travel_cursor();
void release_screen_patch_buffer();
void update_time_travel_buttons(boolean regardless);
void save_city_since_end_of_logging();
string most_recent_city_file_name();
//void release_current_notebook_file_name();
void release_subtitles_file_name();
void release_speak_starts();

boolean rerecorded_segment(int segment_number); // used for debugging experiment 010105

#if TT_DEBUG_ON
boolean is_time_travel_button(Sprite *sprite);
#endif

enum EventFlags {INPUT_EVENT_FLAG=1,TT_HAND_NEEDS_TO_MOVE_FOR_TOOL_USE_FLAG=2,
                 TT_HAND_NEEDS_TO_MOVE_TO_CURSOR_FLAG=4,TT_HAND_NEEDS_TO_MOVE_RELATIVELY_FLAG=8,
                 TT_DELTA_X_AND_Y_DUE_SOLELY_TO_ARROW_KEYS_FLAG=16,TT_DRAGGING_ENABLED_FLAG=32};

#endif
