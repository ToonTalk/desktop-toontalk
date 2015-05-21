// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved.
#ifndef __TEXT_H

#define __TEXT_H

#if !defined(__TT_TOOLS_H)
#include "tools.h"
#endif

// if changed update help.cpp
enum TextHistory {TEXT_CONCATENATION=0,
						TEXT_LETTERS_ADDED,
						TEXT_LETTERS_REMOVED,
						TEXT_MADE_MULTIPLE_LINES,
						TEXT_NUMBER_CONCATENATION,
						TEXT_SOUNDED,
						TEXT_FLIPPED,                   // added on 9/24
						TEXT_BLANK,
						TEXT_CONVERSION,
						TEXT_BLANKED_INSIDE_THOUGHT_BUBBLE,
						TEXT_FORCE_EFFECT_FELT};

class Floor;
class Mouse;

class Text : public Sprite {
  public:
	 Text(city_coordinate llx=0, city_coordinate lly=0,
			wide_string text=NULL, long text_length=-1,
			long priority=0,
			city_coordinate width=maximum_width(NUMBER_PAD_SPRITE),
			city_coordinate height=0,
//			boolean ok_to_reuse_initial_text=FALSE, // bad idea??
         color_index text_color=tt_black);
//	 Text(city_coordinate llx, city_coordinate lly,
//			string text, long text_length=-1,
//			long priority=0,
//			city_coordinate width=maximum_width(NUMBER_PAD_SPRITE),
//			city_coordinate height=0,
//         color text_color=tt_black);
    void release_all_references();
	 void display(SelectionFeedback selection_feedback=NO_FEEDBACK,
					  boolean followers_too=TRUE, TTRegion *region=NULL);
    void update_size(boolean regardless=FALSE);
    void update_size_internal(TTRegion *region, city_coordinate adjusted_width, city_coordinate adjusted_height,
                              city_coordinate &edge_size,
                              city_coordinate &adjusted_text_width, city_coordinate &adjusted_text_height,
                              city_coordinate &adjusted_character_width, city_coordinate &adjusted_character_height,
                              boolean regardless);
    void display_text(city_coordinate start_x, city_coordinate start_y, city_coordinate adjusted_character_width, city_coordinate adjusted_character_height,
							 city_coordinate adjusted_width, city_coordinate adjusted_height, bytes color_permutation, boolean just_show_text, color background_color);
    boolean size_and_location_of_characters(wide_character character,
					city_coordinate adjusted_width, city_coordinate adjusted_height,
               city_coordinate &box_width, city_coordinate &box_height,
               city_coordinate &box_llx, city_coordinate &box_lly);
    boolean compute_display_text_character_size(city_coordinate adjusted_width, city_coordinate adjusted_height);
	 void set_size_and_location(city_coordinate new_width,
										  city_coordinate new_height,
										  city_coordinate new_llx,
										  city_coordinate new_lly,
										  ChangeReason reason=NO_REASON_GIVEN);
	 Sprite *copy(boolean also_copy_offsets=FALSE);
	 void finish_making_copy(Text *copy, boolean also_copy_offsets, boolean copy_followers_too=TRUE);
  	 Sprite *respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected, Sprite *by,
									     boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless=FALSE);
	 void unselect();
	 Sprite *released(Sprite *by, boolean top_level=TRUE, SpecialDrop special_drop=NOT_SPECIAL_DROP);
	 void signal_good_size();
    void generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection);
	 void describe_briefly(output_stream &message, Article article=INDEFINITE_ARTICLE, 
								  boolean capitalize_first=FALSE);
	 void adjust_grasp(city_coordinate &adjustment_x,
							 city_coordinate &adjustment_y);
	 Sprite *select(TTRegion *region, SelectionReason reason, Sprite *in_hand);
//	 Sprite *selected(SelectionReason reason, Sprite *by);
	 void become_blank(boolean by_user=FALSE, boolean controlee_too=TRUE);
	 void become_non_blank(boolean controlee_too=TRUE);
	 boolean blankable() {
		 return(!blank);
	 };
    PlateState plate_state() {
       return(default_plate_state);
    };
    void set_plate_state(PlateState new_state);
	 ReleaseStatus item_released_on_top(Sprite *item, Sprite *by, SpritePointer &recipient, int level);
	 boolean receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item);
	 ReleaseStatus add_to_side(boolean right_side, Sprite *other,
										Sprite *by, 
										millisecond duration, Sprite *original_recipient, Sprite *original_item);
	 boolean on_right_side(Sprite *item);
	 boolean current_text(string &the_text, long &the_text_length);
	 boolean current_wide_text(wide_string &the_text, long &the_text_length);
	 boolean can_be_text() {
		 return(!blank);
	 };
	 wide_string copy_text_string();
	 string copy_narrow_text_string();
	 string return_narrow_text();
	 boolean current_value(NumberValuePointer &the_value, boolean &shared, Sprite *by=NULL);
	 NumberOperation true_operation();
//	 void set_text_after(millisecond duration,
//								string new_text, int new_text_length);
	 wide_string compute_new_text(Sprite *other, long &new_text_length, Sprite *by); // new on 201202
	 void set_to_future_value(Sprite *other, Sprite *original_recipient, Sprite *by);
	 void set_text(string new_text, int new_text_length=-1,
						boolean text_was_updated=FALSE,
						boolean size_constrained_regardless=FALSE);
	 virtual void set_wide_text(wide_string new_text, int new_text_length=-1,
										 boolean text_was_updated=FALSE,
										 boolean size_constrained_regardless=FALSE);
	 void update_sizes(boolean sizes_constrained, boolean character_size_fixed);
//	 void update_sizes_unconstrained();
	 void good_size(city_coordinate &good_width, city_coordinate &good_height, Sprite *source=NULL, boolean for_expander=FALSE);
	 MatchStatus match(Text *number, int version);
    SpriteType dump_type();
#if TT_XML
//    xml_element *xml_create_element(xml_document *document);
	 void xml(xml_element *element, xml_document *document);
    virtual xml_character_data *xml_create_character_data(xml_element *element, xml_document *document);
    boolean handle_xml_tag(Tag tag, xml_node *node);
    wide_string xml_tag() {
       return(L"TextPad");
    };
    xml_element *xml_set_special_attributes(xml_element *element, xml_document *document); // did nothing prior to 201202
	 boolean xml_get_special_attributes(xml_node *node);
	 boolean geometry_worth_saving() { // new on 170103 since not worth saving if is indirection
		 return(!is_picture_flag && Sprite::geometry_worth_saving());
	 };
	 short int default_parameter() { // new on 250103 - only used for dumping XML now
	    return(2);
	 };
#endif
	 boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
    void java(java_stream &out);
    java_string java_text_value();
    boolean to_windows(Pointer *data, short int &data_offset,
                       ascii_string types, short int &types_offset,
                       BirdPointer *bird, short int &bird_count);
	 Sprite *used(Sprite *subject, Sprite *by, boolean button_unchanged,
				     boolean record_action, millisecond , boolean modified, Sprite *original_subject);
//	 boolean to_be_smashed() {
//		 return(to_be_smashed_flag);
//	 };
//	 void set_to_be_smashed(boolean new_flag) {
//		 to_be_smashed_flag = (flag) new_flag;
//	 };
    boolean rectangular();
    void indirection_of_set_parameter(short int new_parameter, boolean warn=TRUE,
                                      boolean reset_cycle_regardless=FALSE);
    void set_color(color_index new_color) {
       text_color = new_color;
    };
	 color_index return_text_color() {
		 return(text_color);
	 };
//    boolean display_just_nice_text() {
//		 // beginning 050102 the meaning of this flag changed to simply mean use_variable_width_font - hence the new version below
//      return(display_just_nice_text_flag);
//    };
//    void set_display_just_nice_text(boolean new_flag) {
//       display_just_nice_text_flag = (flag) new_flag;
//    };
	 boolean use_variable_width_font() {
      return(use_variable_width_font_flag);
    };
    void set_use_variable_width_font(boolean new_flag) {
       use_variable_width_font_flag = (flag) new_flag;
    };
    virtual boolean built_in_sound() {
       return(FALSE);
    };
	 virtual string return_dump_text() { // new on 311099 - added so wave sounds can dump full path but display short name
		 return(copy_narrow_text_string()); // changed on 170200 since some need to cons up an answer - was return_narrow_text());
	 };
 #if TT_DEBUG_ON
	 void set_character_width(city_coordinate new_width);
	 void set_character_height(city_coordinate new_height);
#else
	 void set_character_width(city_coordinate new_width) { // new on 240100 for debugging
		 character_width = new_width;
	 };
	void set_character_height(city_coordinate new_height) { // new on 240100 for debugging
		character_height = new_height;
	};
#endif
		city_coordinate return_character_width() {
		 return(character_width);
	 };
	 city_coordinate return_character_height() {
		 return(character_height);
	 };
//	 void fix_font_size();
//	 boolean fonts_fixed() {
//		 return(fixed_width > 0);
//	 };
	 boolean equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, SpritesPointer &expected_corresponding_nests, EqualReason reason=NORMAL_EQUAL);
	 boolean ok_to_dump_as_text();
	 color appropriate_background_color() {
		 return(text_background_color);
	 };
	 color_index appropriate_background_color_index() {
		 return(tt_nearest_text_background_color);
	 };
//	 void set_show_all(boolean new_flag);
	 boolean change_size_to_fit_extent() { // new on 180102
		 return(change_size_to_fit_extent_flag);
	 };
	 void set_change_size_to_fit_extent(boolean new_flag) {
		 change_size_to_fit_extent_flag = (flag) new_flag;
	 };
	 boolean change_font_size_to_fit_extent() { // new on 180102
		 return(change_font_size_to_fit_extent_flag); // && text_length > 0); // second condition added as experiment on 240904
	 };
	 void set_change_font_size_to_fit_extent(boolean new_flag) {
		 change_font_size_to_fit_extent_flag = (flag) new_flag;
	 };
    boolean show_all_default() { // new on 160802
       return(TRUE);
    };
	 ArrowKeyEditing arrow_key_editing_status() {
		 return(insertion_point >= 0?ARROW_KEY_EDITING_IN_PROGRESS:ARROW_KEY_EDITING_POSSIBLE_BUT_NOT_IN_PROGRESS);
	 };
	 int return_insertion_point() { // new on 120604
		 return(insertion_point);
	 };
	 void set_insertion_point(int new_insertion_point) { // called when loading XML
		 insertion_point = new_insertion_point;
	 };
	 boolean displays_as_pad() {
		 return(TRUE);
	 };
	 void now_on_floor(Background *floor, Sprite *by);
//	 void set_is_picture(boolean new_flag); // new on 121203
  protected:
	 void compute_number_of_lines(); // boolean sizes_constrained);
//	 string text;
	 wide_string wide_text, future_text; 
#if TT_WIDE_TEXT
	 string narrow_text; // used as a cache and to help with memory management
#endif
	 // prior to 090202 was unsigned short since used when dumping to file - now is converted before dumping - soon a new file format will accept longs
	 long text_length, number_of_lines, longest_line; // removed future_text_length on 121202 since better to recompute it than penalize all text pads
	 city_coordinate character_width, character_height;
//	 boolean *when_smashed; // noticed it was obsolete on 191202
//	 Mouse *mouse;
    color_index text_color;
    PlateState default_plate_state;
    flag use_variable_width_font_flag : 1;
	 flag change_size_to_fit_extent_flag : 1;
	 flag change_font_size_to_fit_extent_flag : 1;
	 flag saved_right_side : 1; // new on 201202 since no longer precomputing future_text
	 int insertion_point; // new on 100604
//	 coordinate fixed_width, fixed_height; // noticed this was obsolete on 211201
};

Text *load_text(SpriteType type, InputStream *pad_in,
                int notebook_version, NaturalLanguage language, long index, Sprite *container, boolean descendants_inactive);
boolean describe_text(SpriteType type,
                      InputStream *pad_in, output_stream &text_out, int version_number,
                      boolean in_thought_bubble=FALSE, Article article=INDEFINITE_ARTICLE);
Text *variable_width_text_pad(const_string s, boolean see_all=TRUE, color_index text_color=tt_black);
Text *make_label(const_string s, color_index text_color=tt_black, boolean show_all=FALSE);
Text *make_label(wide_string s, color_index text_color=tt_black, boolean show_all=FALSE);
#endif
