// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved.
#ifndef __CUBBY_H

#define __CUBBY_H

//#if !defined(__TT_TOOLS_H)
//#include "tools.h"
//#endif
#if !defined(__TT_PICTURE_H)
#include "picture.h"
#endif

class Tool;

class Path;

class Cubby : public Sprite {
  public:
	  Cubby(city_coordinate x=0, city_coordinate y=0, 
			  long initial_priority=0,
//			  Background *floor=NULL,
			  city_coordinate width=0, city_coordinate height=0,
			  int number_of_holes=1, // 1 "hole" by default
			  Sprite **contents=NULL,
			  string *labels=NULL);
	  void release_all_references();
	  Sprite *copy(boolean also_copy_offsets=FALSE);
	  void adjust_height();
	  void good_size(city_coordinate &w, city_coordinate &h, Sprite *source=NULL, boolean for_expander=FALSE);
	  void update_piece_widths();
	  int compute_peg_count(int number);
     city_coordinate width_given_hole_count(int hole_count);
//	  void used(Tool *subject, Sprite *by,
//					boolean , Screen *screen,
//					millisecond , boolean *true_when_done);
	  Sprite *select(TTRegion *region, SelectionReason , Sprite *in_hand);
	  Sprite *selected(SelectionReason reason, Sprite *by);
     void set_priority(long new_priority);
	  void remove_selected_element(Sprite *element, SelectionReason reason, Sprite *by, boolean add_to_floor);
	  Sprite *respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected, Sprite *by,
										   boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless=FALSE);
	  void generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection);
	  void cubby_insides(Sprite *element,
								city_coordinate &insides_llx,
								city_coordinate &insides_lly,
								city_coordinate &insides_width,
								city_coordinate &insides_height);
	  void cubby_insides_of_index(int hole_index,
								city_coordinate &insides_llx,
								city_coordinate &insides_lly,
								city_coordinate &insides_width,
								city_coordinate &insides_height);
	  city_coordinate hole_center_x(int hole_index);
	  void set_size_and_location(city_coordinate new_width,
										  city_coordinate new_height,
										  city_coordinate new_llx,
										  city_coordinate new_lly,
										  ChangeReason reason=NO_REASON_GIVEN);
	  void display(SelectionFeedback selection_feedback=NO_FEEDBACK,
						boolean followers_too=TRUE, TTRegion *region=NULL);
	  void display_label(int index,
								city_coordinate character_llx,
								city_coordinate character_ury,
								city_coordinate peg_width,
								city_coordinate peg_height,
								bytes color_permutation);
	  void display_the_label(const_string label,
									 city_coordinate character_llx,
									 city_coordinate character_ury,
									 city_coordinate peg_width,
									 city_coordinate peg_height,
									 color_index text_color,
									 bytes color_permutation);
	  void display_the_label(wide_string label,
									 city_coordinate character_llx,
									 city_coordinate character_ury,
									 city_coordinate peg_width,
									 city_coordinate peg_height,
									 color_index text_color,
									 bytes color_permutation);
	  Sprite *released(Sprite *by, boolean top_level=TRUE, SpecialDrop special_drop=NOT_SPECIAL_DROP);
//     void make_contents_fit();
//     void used(Sprite *subject,
//             Background *floor_from_above,
//             Screen &screen);
//     boolean item_released(Sprite *released);
	  void suck_up();
	  void animate_suck_up();
	  ReleaseStatus item_released_on_top(Sprite *item, Sprite *by, SpritePointer &recipient, int level);
	  boolean receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item);
	  void set_to_future_value(Sprite *item, Sprite *original_recipient, Sprite *by);
	  boolean hole_receives_item(Sprite *item, int hole_index, Sprite *by,
										  millisecond duration=0,
										  boolean record_event=FALSE);
	  void initialize_hole(int hole_index, Sprite *item, boolean geometry_needs_to_set=TRUE);
	  void fit_into_hole(int index, Sprite *item, boolean initial);
	  void signal_size_and_location();
     void notice_on_screen_change(boolean on_screen);
     void really_propagate_changes(ChangeReason reason=NO_REASON_GIVEN); 
     void refit_contents();
	  void hole_region(int index, TTRegion &region);
	  boolean hole_contents_contains(int hole_index, 
												city_coordinate center_x, 
												city_coordinate center_y);
	  ReleaseStatus add_to_side(boolean right_side, Sprite *other,
										 Sprite *by, 
										 millisecond duration, Sprite *original_recipient, Sprite *original_item);
//	  void set_number_of_holes_after(millisecond duration, long number);
//	  void set_to_planned_number_of_holes();
	  Cubby *set_number_of_holes(long number, 
										  boolean excess_on_right=TRUE,
										  boolean save_excess=FALSE,
                                boolean initializing=FALSE);
//	  void adjust_grasp(city_coordinate &adjustment_x,
//							  city_coordinate &adjustment_y);
	  void update_labels(char key, boolean extended, Sprite *by); 
	  void update_label(char key, boolean extended, long index, Sprite *by); 
	  string label(int index, boolean remove=FALSE);
	  void set_label(int index, string new_label);
	  void is_labeled();
	  boolean labeled() {
		  return(labels != NULL);
	  };
	  void update_parameter();
	  Sprite *now_inside_thought_bubble(boolean in_training);
	  Path *path_to(Sprite *target, boolean &ok);
	  MatchStatus match(Cubby *cubby, SpritePointerPointer &suspension_cell, int version);
//     Sprite *dereference_component(cubby_index index);
	  Sprite *component(int index);
	  SpritePointerPointer pointer_to_component(int index);
	  long my_index(Sprite *item);
     void remove_me(Sprite *item, boolean will_return, SelectionReason reason=NO_SELECTION_REASON_GIVEN, 
						  boolean add_to_floor=TRUE);
     void remove_component(int hole_index, SelectionReason reason, millisecond duration, boolean add_to_floor);
	  Sprite *remove_component_directly(int hole_index);
#if !TT_32
	  boolean vectorize(int arity, int offset, SpriteType *types, long *vector);
#endif
	  boolean contains(Sprite *item);
	  boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
#if TT_XML
 //    xml_element *xml_create_element(xml_document *document);
     void xml(xml_element *element, xml_document *document);
     boolean handle_xml_hole_tag(Tag tag, xml_node *node, int hole_number);
     boolean handle_xml_tag(Tag tag, xml_node *node);
     xml_element *xml_set_special_attributes(xml_element *element, xml_document *document); 
	  // did nothing prior to 121202 (i.e. blocked Sprite::xml_set_special_attributes)
	  boolean xml_get_special_attributes(xml_node *node);
	  void add_special_xml(xml_element *element, xml_document *document);
	  short int default_parameter() { // new on 250103 - only used for dumping XML now
	     return(1);
	  };
	  wide_string xml_tag() {
		  return(L"Box");
	  };
#endif
	  void inside_thought_bubble();
	  void now_on_back_of(Picture *picture);
//     Sprites *reachable_robots();
//     void java_flipped_followers(java_stream &out, const_java_string picture_name);
     void java_before(java_stream &out);
     void java(java_stream &out);
     void java_after(java_stream &out, const_java_string variable);
     boolean to_windows(Pointer *data, short int &data_offset,
                        ascii_string types, short int &types_offset,
                        BirdPointer *bird, short int &bird_count);
//	  void set_move_after_sucking_up(boolean new_flag) {
//		  move_after_sucking_up = (flag) new_flag;
//	  };
	  boolean current_value(NumberValuePointer &the_value, boolean &shared, Sprite *by=NULL);  // used to get the arity
//		  the_value = number_of_holes;
//		  return(!blank);
//	  };
	  NumberOperation true_operation() {
		  return(MAKE_EQUAL); //  for arity
     };
	  int current_number_of_holes() {
		 return(number_of_holes);
	  };
	  void set_main_cubby_in_body(boolean new_flag) {
		  main_cubby_in_body = (flag) new_flag;
	  };
	  boolean is_main_cubby_in_body() {
		  return(main_cubby_in_body);
	  };
	  void become_blank(boolean by_user=FALSE, boolean controlee_too=TRUE);
	  void become_non_blank(boolean controlee_too=TRUE);
	  boolean is_busy() {
		  return(busy_counter != 0);
	  };
	  void increment_busy_counter() {
		  busy_counter++;
	  };
	  void decrement_busy_counter();
	  void run_when_non_busy(Sprite *sprite, AnimationCallBack callback);
	  //boolean inside_sprite() { // noticed obsolete on 121202
		 //return(inside_sprite_flag);
	  //};
	  //void set_inside_sprite(boolean new_flag);
	  boolean blankable();
	  boolean all_holes_empty();
	  void initialize_labels(string *new_labels);
     boolean suck_up_in_progress() {
        return(item_being_sucked_up != NULL);
     };
	  boolean ok_to_cache_without_followers() {
		  return(FALSE); // new on 191200
	  };
	  boolean equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, SpritesPointer &expected_corresponding_nests, EqualReason reason=NORMAL_EQUAL);
//	  void side_about_to_be_seen(boolean front) { 
		  // new on 050600 to do nothing since if inside a box about whether it is on the front or back - still runs
//	  };
#if TT_DEBUG_ON
	  void check_integrity();
#endif
//	  city_coordinate full_width() {
//		  return(width);
//	  };
//	  city_coordinate full_height() {
//		  return(height);
//	  };
	  string *pointer_to_labels() { // new on 120601 - used for equal_to
		  return(labels);
	  };
//	  void become(Sprite *other);
     boolean show_all_default() { // new on 160802
        return(TRUE);
     };
	  void active_region(TTRegion &region);
	  boolean vacuum_if_left_on_floor(SpritePointer *others_being_vaccuumed, int others_count);
  protected:
//	 void notify_leader(boolean smaller);
//	 void record_grasp_or_vacuum(SelectionReason reason, Sprite *selection, Sprite *by);
	 void initialize_contents(int old_size, int new_size,
									  boolean excess_on_right=TRUE, int excess_holes=0);
	 long closest_hole(city_coordinate x,city_coordinate y);
	 void set_contents(Sprite **new_contents);
	 Sprite **contents; // parameter is the number of "holes"
	 Sprite *item_being_sucked_up;
//    boolean sucking_up_done;
//	 ActionStatus *action_status; // made obsolete on 171102
	 short unsigned int busy_counter; // e.g. bird is building nest and will return -- or parallel robots using this
//	 flag move_after_sucking_up : 1;
	 flag main_cubby_in_body : 1;
	 int item_being_sucked_up_index; // prior to 070203 was cubby_index (short int)
//    millisecond set_number_of_holes_after_this;
	 int number_of_holes; // prior to 070203 was cubby_index (short int)
//    long planned_number_of_holes; // noticed this was obsolete on 121202
	 city_coordinate peg_width;
//	 flag inside_sprite_flag : 1; // noticed obsolete on 121202
//	 Sprite *saved_by;
	 string *labels;
	 city_coordinate previous_width, previous_height;
	 wide_string future_text; // better way than having this here??
//	 long future_text_length; // was unsigned short int prior to 090202 -- obsolete as of 121202 since penalizes all for a minor speedup
	 AnimationCallBacks *non_busy_callbacks;
	 long last_drop_index;
	 flag saved_right_side : 1;
	 Sprites *returning_items;
};

Cubby *load_cubby(SpriteType type, InputStream *pad_in, 
						int notebook_version, NaturalLanguage language,
                  Sprite *container, Picture *controlee, long index, boolean descendants_inactive);
boolean describe_cubby(SpriteType type, InputStream *pad_in, output_stream &text_out, int version_number,
						     boolean in_thought_bubble=FALSE,
						     Article article=INDEFINITE_ARTICLE);
boolean describe_cubby2(SpriteType type, InputStream *pad_in, output_stream &text_out, int version_number,
						      boolean in_thought_bubble, cubby_index size);
boolean cubby_contents_type(SpriteType contents_type);
void user_broke_cubby();
void user_obtained_arity();
void release_boxes_already_declared();

#endif
