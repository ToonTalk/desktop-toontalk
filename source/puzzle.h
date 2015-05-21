// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved
#ifndef __TT_PUZZLE_H

#define __TT_PUZZLE_H

class House;
class Room;
class Robot;
class Programmer_At_Floor;

class PuzzleState : public Entity {
   public:
     PuzzleState() :
       counter(0),
       birth_year(1985),
       birth_month(1),
       birth_day(29),
       birth_hour(19),
       birth_minute(51),
       birth_second(4) {};
     short int counter;
     short int birth_year;
     char birth_month;
     char birth_day;
     char birth_hour;
     char birth_minute;
     char birth_second;
};


enum PuzzleLoadStatus {NO_PUZZLE_FILE,PUZZLE_LOADED_FINE,PUZZLE_FILE_BAD,PUZZLES_ALL_DONE,EMPTY_PUZZLE_FILE}; // EMPTY_PUZZLE_FILE added on 080402 to indicate next puzzle

PuzzleLoadStatus reload_puzzle(House *new_puzzle_house=NULL);
PuzzleLoadStatus load_puzzle(House *house=NULL,
                             boolean reset_old=TRUE, boolean save_robots=FALSE,
									  boolean skip_robots=FALSE);
void toolbox_is_part_of_puzzle();
boolean starting_with_first_tutorial_puzzle();
//short int current_puzzle_counter();
boolean is_toolbox_part_of_puzzle();
boolean ok_to_enter_house(House *house);
boolean puzzle_oks_function_keys(Sprite *by);
boolean puzzle_oks_typing_to_number_pads(Sprite *by);
boolean puzzle_oks_typing_to_text_pads(Sprite *by);
boolean puzzle_oks_flipping(Sprite *by);
void set_rocket(House *house);
boolean inside_rocket();
void puzzle_talk();
void say_intro();
void robot_failed(Robot *robot);
//void entering_house(House *house);
//void respond_to_solution(Sprite *answer);
void release_puzzle_data(boolean for_good);
void quiting_puzzles();
House *pointer_to_rocket();
House *pointer_to_puzzle_house();
void set_rocket(House *house);
void set_puzzle_house(House *house);
boolean new_puzzle_name(int counter);
void resume_puzzles();
void save_puzzle_state(output_stream &history_file, boolean puzzle_state_changed=FALSE);
void read_puzzle_state(InputStream *history_file, boolean called_from_log=FALSE);
long resume_read_puzzle_state(InputStream *history_file, int version_number, boolean called_from_log, int xml_version);
void resume_read_puzzle_state2(InputStream *stream, int version_number, boolean called_from_log=FALSE);
void resume_read_puzzle_state_xml(InputStream *stream, int xml_version, boolean called_from_log);
boolean ok_to_save_puzzle_state();
void function_key_pressed(char key); // Programmer_At_Floor *the_programmer);
void colliding_with_marty();
void player_has_sat();
void player_leaving_room(Room *room);
void martian_stopped_talking();
boolean ok_to_create_remote_notebook();
string read_lines(InputStream *file, long max_length, long &length,
                  char line_terminator);
Picture *players_age_in_seconds();

void allocate_puzzle_data();

boolean first_digits_of_puzzle_name(short int &first_digit_position, short int &last_digit_position);

void update_slot_ids(boolean initialization);

enum PuzzleStatus {JUST_STARTED,WAITING_FOR_SOLUTION,RESPONDED,ALL_PUZZLES_SOLVED,
                   WAITING_TO_VERIFY_SOLUTION,WAITED_TOO_LONG_TO_VERIFY_SOLUTION,PUZZLE_JUST_LOADED}; // moved here on 121102

PuzzleStatus return_puzzle_status();

void next_puzzle_default(); // exported on 141104
boolean 
next_puzzle_name(); // exported 151104

#endif
