// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.

#ifndef __TT_MARTIAN_H

#define __TT_MARTIAN_H

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif   

class Talk_Balloon : public Sprite {
	public:
	  Talk_Balloon(city_coordinate initial_x=0, city_coordinate initial_y=0);
//	  ~Talk_Balloon();
	  void prepare_for_deletion();
	  void new_character_size(long percent, boolean update_characters_fitting_in_full_size);
	  boolean say(Martian *martian,
                 string text, int length=-1, boolean need_to_copy=TRUE,
                 boolean add_to_old=FALSE,
					  string alternative_spoken_new_text=NULL,
					  boolean need_to_copy_alternative_spoken=TRUE, 
					  boolean add_to_old_alternative_spoken=FALSE);
     boolean just_said(string text, int length);
	  void compute_size(int length, Martian *martian);
#if TT_TEXT_TO_SPEECH
     boolean speech_has_finished(boolean not_old_topic);
#endif
	  void place_rest_of_text(); // callback version
     void remove_text(boolean speech_too=TRUE);
	  boolean dont_wait_to_say_more();
	  boolean still_talking();
     string currently_saying();
#if TT_TEXT_TO_SPEECH
     void speak_next_sentence();
	  void next_subtitle_for_closed_caption_alternative_language(int speech_line_length=0, int sentence_count=0);
#endif
	  void display(SelectionFeedback selection_feedback=NO_FEEDBACK,
						boolean followers_too=TRUE, TTRegion *region=NULL);
     boolean save_region();
     boolean displaying_dots() {
       return(displayed_dots);
     };
     long current_say_id() {
       return(say_id);
     };
	  boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
	  void load(InputStream *stream, int version_number, NaturalLanguage language);
//     void next_word_spoken(int offset);
//	  boolean full_size();
//	  boolean more_to_say() {
//		  return(waiting_to_show_more);
//	  };
	  void set_speech_should_wait_for_first_display(boolean new_flag) {
		  speech_should_wait_for_first_display = (flag) new_flag;
	  };
	  boolean ok_to_cache() { // new on 100703 - important for the new time travel scheme for saving cities
		  return(FALSE);
	  };
#if TT_XML
    wide_string xml_tag() { // new on 311102 -- currently has no "state"
       return(L"TalkBalloon");
    };
    void xml(xml_element *element, xml_document *document);
    boolean handle_xml_tag(Tag tag, xml_node *node);
#endif
	private:
	  string text;
	  string alternative_spoken_text;
	  int length;
//	  int alternative_spoken_length;
	  int text_offset, text_offset_increment;
	  flag waiting_to_show_more : 1;
     flag displayed_dots : 1;
	  int full_size_percent;  // percentage needed to say all of the text
//	  int max_characters_per_line, max_lines;
	  int characters_fitting_in_full_size;
	  city_coordinate character_width,character_height;
	  millisecond text_removal_time;
     city_coordinate old_llx, old_lly;
     int paragraph_count;
     int *paragraph_end;
     short int recent_sentence_count;
     short int oldest_recent_sentence_index;
     string *recent_sentence;
     int speech_text_offset, speech_text_offset_increment;
	  int speech_text_length;
     long say_id;
	  millisecond duration_of_last_sentence_spoken; // moved here from global on 110100
	  millisecond start_of_current_sentence_spoken;
	  int characters_in_last_sentence_spoken;
	  flag speech_should_wait_for_first_display; // new on 040500
     Martian *martian; // new on 090502
//     flag remove_text_when_speech_done;
//     int current_word_offset;
//     int spoken_word_length;
//     string spoken_word;
};

class ostrstream;

class Martian : public Sprite {
	public:
	  Martian();
	  void re_initialize();
	  void prepare_for_deletion();
	  void teleport_in_after(millisecond delay);
	  void teleport_in(TeleportInReason reason=MARTY_CALLED_BY_TIMER);
	  void teleport_in_done();
	  void teleport_out(boolean called_by_user=FALSE);
	  void teleport_out_done();
#if TT_TEXT_TO_SPEECH
     void new_talk_mode(MartyTalk new_mode);
#endif
	  void teleport_toggle(Sprite *current_selection, Sprite *tool_in_hand);
	  void change_background(Background *new_background);
//	  void add_balloon();
	  void new_character_size(unsigned short int percent);
	  void remove_balloon();
     void hide_balloon();
     boolean show_balloon();
	  void initialize_balloon(Talk_Balloon *new_balloon=NULL, boolean loading=FALSE); 
	  // optional arg new on 211103 // second arg 040204
     void repeat_or_speed_up();
     void repeat_last_thing_said();
	  string repeat_last_thing_said_string();
	  void say(int text_id, int importance=0);
	  void say(const_string text, int importance=0, const_string alternative_spoken_text=NULL);
	  void react(boolean &new_user_input,
					 unsigned char &key,
					 boolean &extended_key,
					 city_coordinate &delta_x,
					 city_coordinate &delta_y,  
					 char &button_status,
					 millisecond );
	  boolean dont_wait_to_say_more();
     void dont_wait_to_give_help(boolean override_old=TRUE);
     void started_talking();
	  void just_talk_balloons_for(millisecond duration);
	  void move_to_favorite_spot();
	  void animate_to_favorite_spot_and_size(millisecond duration);
	  void set_priority(long new_priority);
//	  boolean active() {
//		  return(active_flag);
//	  };
//	  void set_active(boolean new_flag) {
//		  active_flag = (flag) new_flag;
//	  };
	  boolean still_talking();
     string currently_saying();
     boolean displaying_dots() {
        return(balloon != NULL && balloon->displaying_dots());
     };
	  Sprite *pointer_to_potential_help_target() {
		  return(potential_help_target);
	  };
	  void set_potential_help_target(Sprite *sprite);
	  void clear_selection() {
		  set_potential_help_target(NULL);
	  };
//	  void update_display();
	  void update_balloon();
//	  void display(Screen *, SelectionFeedback selection_feedback=NO_FEEDBACK,
//						boolean followers_too=TRUE);
//	  boolean save_region(Screen *screen);
	  void programmer_at_floor(Sprite *programmer_appearance,
										Sprite *current_selection);
     void say_pending_message();
	  void keep_out_of_the_way(TTRegion &region);
	  void action_aborted(int reason, Sprite *by, SpriteType actor=NONE_GIVEN, int importance = -1, string default_string="");
	  void action_aborted2(output_stream &message,
								  int reason, Sprite *by, SpriteType actor, int &importance, string default_string="");
	  void give_help(Sprite *current_selection, Sprite *tool_in_hand);
     void give_help_after(millisecond delay);
	  void give_help2(Sprite *current_selection, Sprite *tool_in_hand,
						   output_stream &message, boolean alternate, int &importance, Sprite *target); 
	  void preface_full_help(output_stream &message, Sprite *subject);
	  void give_greeting(output_stream &message, boolean alternate);
	  void say_hi(output_stream &message);
	  void give_name(output_stream &message);
	  boolean stop_generating_help();
	  void say_completed();
	  void remove_text(boolean pending_message_too=TRUE);
	  void no_interruptions();
	  boolean called_by_user() {
		  return(called_by_user_flag);
	  };
     void speech_has_finished_by_callback();
     void speech_has_finished(long id);
	  // commented out on 190103
    // boolean ok_to_delete() {
		  //// updated on 160103 so martian that is local to titles is recycled
    //    return(tt_martian != this); // even if stuck in some house -- cleanup will clean this up
    // };
	  void balloon_is_now_invisible() {
		  ready_to_move_off_screen_time = 1; // move off screen next
	  };
	  boolean is_message_pending() {
		  return(pending_message != NULL);
	  };
	  ReleaseStatus item_released_on_top(Sprite *item, Sprite *by, SpritePointer &recipient, int level);
	  void power_toggled();
	  void remove_selected_element(Sprite *element, SelectionReason reason, Sprite *by, boolean add_to_floor);
	  Sprite *select(TTRegion *region, SelectionReason reason, Sprite *in_hand);
	  boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
	  void load(InputStream *stream, int version_number, NaturalLanguage language);
	  Sprite *copy(boolean also_copy_offsets=FALSE);
	  MartyTalk return_saved_tt_marty_talk() {
		  return(saved_tt_marty_talk);
	  };
	  boolean return_ok_to_repeat() {
		  return(ok_to_repeat);
	  };
	  string return_last_thing_said(boolean alternative) {
		  return(last_thing_said[alternative]);
	  };
	  void set_last_thing_said(boolean alternative, string new_string) {
		  if (last_thing_said[alternative] != NULL) delete [] last_thing_said[alternative];
		  last_thing_said[alternative] = new_string;
	  };
	  void set_teleport_in_selection(Sprite *sprite);
	  void set_teleport_in_tool_in_hand(Sprite *sprite);
	  void set_previous_target(Sprite *sprite);
	  void set_subject(Sprite *sprite);
	  boolean staying_around() {
		  return(!teleport_out_when_nothing_to_say);
	  };
	  int importance_of_last_thing_spoken() {
		  return(previous_importance); // new on 120500
	  };
	  long current_say_id() {
		  return(balloon->current_say_id());
	  };
	  void set_speech_should_wait_for_first_display(boolean new_flag) {
		  if (balloon != NULL) {
			  balloon->set_speech_should_wait_for_first_display(new_flag);
		  };
	  };
	  void set_size_and_location(city_coordinate width, city_coordinate height,
										  city_coordinate llx, city_coordinate lly,
										  ChangeReason reason=NO_REASON_GIVEN); // new on 150202
     void set_saved_say_id(int new_say_id) {
        saved_say_id = new_say_id;
     };
#if TT_XML
     wide_string xml_tag() { // new on 311102 
       return(L"Martian");
     };
     void xml(xml_element *element, xml_document *document);
     boolean handle_xml_tag(Tag tag, xml_node *node);
#endif
	  boolean ok_to_cache() { // new on 100703 - important for the new time travel scheme for saving cities
		  return(FALSE);
	  };
	  boolean ok_to_dump() { // new on 040803
        return(ok_to_dump_flag && tt_dump_reason != DUMPING_TO_CLIPBOARD);
     };
//     void next_word_spoken(int offset);
//	  boolean teleport_in_when_sitting() {
//		  return(automatically_appear_when_sitting);
//	  };
	private:
	  Talk_Balloon *balloon;
//	  Background *background;
//	  flag active_flag : 1;
	  flag on_floor : 1;
	  flag keep_ignoring_button : 1;
	  flag dont_interrupt : 1;
	  flag greeting_needed : 1; 
	  flag next_selection_is_target : 1;
//	  flag automatically_appear_when_sitting : 1;
	  flag first_time : 1;
	  flag timer_trigger_help : 1;
	  flag saying_greeting : 1;
	  flag teleport_out_when_nothing_to_say : 1;
	  flag pending_message_dont_interrupt : 1;
	  flag called_by_user_flag : 1;
     flag teleporting_out : 1;
	  millisecond time_of_last_help,time_between_greetings,
					  ready_to_move_off_screen_time;
	  city_coordinate favorite_floor_x, favorite_floor_y,
							favorite_room_x, favorite_room_y;
//	  int	called_count; // how many times he's been called upon
	  Sprite *potential_help_target; // waiting to talk about this
//	  Sprite *previous_tool_in_hand; // last thing in hand talked about
	  Sprite *teleport_in_selection;
	  Sprite *teleport_in_tool_in_hand;
	  string pending_message;
	  string alternative_spoken_pending_message;
	  int previous_importance;
	  int pending_message_importance;
	  int center_counter;
     millisecond start_of_current_selection;
     flag give_help_without_waiting;
     Sprite *previous_target;
     millisecond started_generating_help;
     unsigned short int last_greeting_help;
	  Sprite *subject; // for associating comments
	  // moved here on 110100 - were global previously
	  string last_thing_said[2]; // normal and alternative spoken languages
	  boolean ok_to_repeat;
	  MartyTalk saved_tt_marty_talk;
     int saved_say_id; // new on 151102 -- was file-level variable earlier
//     city_coordinate saved_llx, saved_lly, saved_width, saved_height; // new on 090502
};

boolean martian_active();
void martian_just_injured_and_pointing();
void martian_just_injured_and_dizzy();

#if TT_TEXT_TO_SPEECH
string process_for_text_to_speech(string to_say, int line_length);
#if TT_SPEAK_ALL_NUMBERS
void process_for_text_to_speech(wide_string input, int length, wide_string output);
#endif
#endif

Martian *load_martian(InputStream *stream, int version_number, NaturalLanguage language);
Talk_Balloon *load_talk_balloon(InputStream *stream, int version_number, NaturalLanguage language);

#if TT_CALL_FOR_MARTY
void remove_call_for_marty(boolean remove_text);
void update_call_for_marty();
#endif

#endif
