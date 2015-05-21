// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved
#if !defined(__TT_HELP_H)

#define __TT_HELP_H

#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif   

//void read_page(int page, InputStream *pad_in, output_stream &text_out);
boolean ignore_next_item(InputStream *stream, int version_number, boolean in_thought_bubble=FALSE,
                         SpriteType container_type=NONE_GIVEN,
								 boolean is_controlee=FALSE, boolean on_nest=FALSE);
boolean ignore_next_item(SpriteType type, InputStream *stream, int version_number, boolean in_thought_bubble=FALSE,
								 SpriteType container_type=NONE_GIVEN,
								 boolean is_controlee=FALSE, boolean on_nest=FALSE);
boolean describe_item(InputStream *pad_in, output_stream &text_out, int version_number,
							 boolean in_thought_bubble=FALSE,
							 Article article=INDEFINITE_ARTICLE,
                      SpriteType container_type=NONE_GIVEN,
							 boolean is_controlee=FALSE, boolean on_nest=FALSE);
boolean describe_item(SpriteType type, InputStream *pad_in, output_stream &text_out, int version_number,
							 boolean in_thought_bubble=FALSE,
							 Article article=INDEFINITE_ARTICLE,
                      SpriteType container_type=NONE_GIVEN,
							 boolean is_controlee=FALSE, boolean on_nest=FALSE);
boolean describe_encoding(SpriteType type, InputStream *pad_in, output_stream &text_out, int version_number,
								  boolean in_thought_bubble=FALSE, 
							     Article article=INDEFINITE_ARTICLE,
                          SpriteType container_type=NONE_GIVEN,
								  boolean is_controlee=FALSE, boolean on_nest=FALSE);
boolean describe_comment_encoding(InputStream *pad_in, output_stream &text_out, int version_number, SpriteType type);
void print_ordinal_definite_phrase(int count, output_stream &text_out,
                                   boolean capitalize_first,
                                   const_string first, const_string second);
void capitalize_first_letter(const_string text, output_stream &text_out); 
void print_ordinal(int i, output_stream &text_out); // , boolean capitalize_first=FALSE
void print_article(Article article, output_stream &text_out, const_string next=NULL);
void print_it(SpriteType type, output_stream &text_out);
//void print_the(output_stream &text_out, boolean capitalize_first=FALSE, SpriteType type=NONE_GIVEN, string noun="");
boolean starts_with_preposition(const_string text, int length);
void describe_hand(output_stream &message);
void you_are(output_stream &message, boolean capitalize=TRUE); 
void you_are2(output_stream &message);
void print_type(SpriteType type, output_stream &text_out, Article article=INDEFINITE_ARTICLE, boolean capitalize_first=FALSE);
const_string indefinite_type(SpriteType type);
void print_with_article(Article article, unsigned short int index, SpriteType type, 
								output_stream &text_out, boolean capitalize_first=FALSE,
								string default_string=NULL);
void describe_appearance(SpriteCode code, output_stream &text_out, Article article);
void describe_pointing(output_stream &message, Sprite *tool_in_hand, Sprite *selection);
void enter_thought_bubble_help();
void left_thought_bubble_help();
void user_did(int history_index, int action, Sprite *by=NULL);
boolean did_user(int action, int history_index=GLOBAL_HISTORY);
void user_told(int action);
boolean told_user(int action);
boolean type_history_full(int type);
void describe_space_bar_alternatives(output_stream &message);
void describe_right_button(output_stream &message, char alternative=' '); // is this always a good default??
void describe_middle_button(output_stream &message);
void describe_left_button(output_stream &message);
void no_help_interruptions();
boolean stop_generating_help(int size_of_next_output=512);
void report_action_aborted(int reason, Sprite *by,
									SpriteType actor=NONE_GIVEN, int importance=-1, string default_string="");
//boolean martians_previous_selection(Sprite *other);
//void advise(string message, int importance=3);
void martian_says(int message_id, int importance);
void log(const_string message, boolean message_box_too=FALSE, boolean important=FALSE);
#if TT_DEBUG_ON
void debug_this(string extra=NULL);
boolean debug_log(int debug_counter, string message=NULL);
void debug_ref_count(int ref_count, xml_node *element);
#endif
void say_error(const_string message, boolean always_tell_user=FALSE);
void say_error(int message_id, boolean always_tell_user=FALSE);
void say(int message_id, int importance=3, boolean log_too=FALSE); // assume it is important;
void say(const_string message, int importance=3, boolean log_too=FALSE); // try not to use this since can't deal with alternative spoken language
void say_file_name_error(int message_id, string file_name, int importance, string extension=NULL);
void describe_key_stroke(int key, output_stream &text_out);
const_string character_name(int key, NaturalLanguage language);
void set_pre_initialization_warning_max(int new_max);

#endif
