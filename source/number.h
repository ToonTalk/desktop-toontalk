// Copyright (c) 1992-2006.  Ken Kahn, Animated Programs, All rights reserved.
#ifndef __NUMBER_H

#define __NUMBER_H

#if !defined(__TT_CUBBY_H)   
#include "cubby.h"
#endif   
#if !defined(__TT_TOOLS_H)   
#include "tools.h"
#endif   
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif   

enum ArithmeticException 
	 {NO_EXCEPTION,INTEGER_OVERFLOW,INTEGER_UNDERFLOW,DIVIDE_BY_ZERO,SOME_ARITHMETIC_EXCEPTION,FLOATING_POINT_NOT_A_NUMBER,
	  SOME_ARITHMETIC_EXCEPTION_ALREADY_REPORTED}; // new on 070305 so Marty doesn't repeat himself

enum RationalNumberFlag // new on 250202 -- much better style - should use this everywhere flags are dumped and loaded
	{INTEGER_AND_FRACTION_FORMAT=1,
	 DECIMAL_NOTATION_IF_EXACT_FORMAT=2,
	 PUNCTUATE_EVERY_THREE_DIGITS_FORMAT=4, // obsolete - can be recyled
	 INEXACT_NUMBER=8,
	 USING_DEFAULT_COLOR_FORMAT=16,
	 OPERATION_WITH_NO_VALUE=32};

class Floor;
class Mouse;

class Number : public Sprite { // changed from Integer to Number on 280102
  public:
	  Number(city_coordinate center_x, city_coordinate center_y, 
            city_coordinate width, city_coordinate height, // these were missing prior to 220702        
            long priority,
				NumberOperation operation=NO_NUMBER_OPERATION,
  				int min_text_length=0,
				int text_color=-1, // was color_index but need to distinguished between default and black - new on 130202
				SpriteType type=INTEGER,
				NumberFormat number_format=TT_LONG,
				boolean integer_and_fraction_format=TRUE,
				boolean decimal_notation_if_denominator_is_power_of_ten=TRUE,
				int base=tt_number_output_base);
//	 void prepare_for_deletion();
	 void release_all_references();
//  virtual wide_string text_value() = 0; // made virtual on 280102 (and abstract)
//	 virtual const_string narrow_text_value() {
//		 return("should never need narrow_text_value"); // made abstract on 280102
//	 };
	 void display(SelectionFeedback selection_feedback=NO_FEEDBACK,
					  boolean followers_too=TRUE, TTRegion *region=NULL);
	 void display_shrinking_decimal(wide_string wide_text, int wide_text_length,
											  city_coordinate ulx, city_coordinate uly,
											  city_coordinate character_width, city_coordinate character_height,
											  bytes color_permutation,
											  city_coordinate total_width,
											  city_coordinate height_or_region_height);
#if TT_SHRINKING_AND_GROWING
	 void display_shrinking_growing_integer(wide_string wide_text1, int wide_text_length,
														 //wide_character separator,
														 int separator_index,
														 //wide_string wide_text2, int wide_text_length2,
														 city_coordinate start_x, city_coordinate start_y,
														 city_coordinate adjusted_character_width, city_coordinate adjusted_character_height,
														 double part1_shrinkage, double part2_shrinkage,
														 bytes color_permutation,
														 //city_coordinate total_width1, city_coordinate total_width2,
														 city_coordinate total_width,
														 city_coordinate height_or_region_height);
	 void display_shrinking_growing_integer_part(wide_string wide_text1, int wide_text_length1,
															   city_coordinate ulx, city_coordinate uly,
																city_coordinate character_width, city_coordinate character_height,
																bytes color_permutation,
																city_coordinate total_width,
																city_coordinate height_or_region_height,
															   double shrinkage, double digit_width_to_character_width,
																double digit_height_to_character_height,
																double &ulx_as_double, double &character_width_as_double,
																double &character_height_as_double);
	 int text_out_dots(string dot, int dots_remaining, double &start_x_as_double, city_coordinate start_y, double max_start_x,
							 city_coordinate dot_character_width, city_coordinate dot_character_height, bytes color_permutation,
							 double dot_true_width);
 	 void display_old(SelectionFeedback selection_feedback=NO_FEEDBACK,
						   boolean followers_too=TRUE, TTRegion *region=NULL);
	 void display_shrinking_decimal_old(wide_string wide_text, int wide_text_length,
													city_coordinate ulx, city_coordinate uly,
													city_coordinate character_width, city_coordinate character_height,
													bytes color_permutation,
													city_coordinate total_width,
													city_coordinate height_or_region_height);
#endif
//	 int digits_before_decimal_point(wide_string wide_text, int wide_text_length);
	 void compute_shrinking_decimal_places();
	 void compute_shrinking_decimal_places_unconstrained();
	 int non_shrinking_equivalent(int character_count);
#if TT_SHRINKING_AND_GROWING
	 int non_shrinking_or_integer_part_and_fraction_equivalent(wide_string wide_text, int character_count);
	 double non_shrinking_growing_equivalent(int character_count, double shrinkage);
	 void set_part1_shrinkage(double shrinkage) {
		 part1_shrinkage = shrinkage;
	 };
	 void set_part2_shrinkage(double shrinkage) {
		 part2_shrinkage = shrinkage;
	 };
#endif
	 Sprite *respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected, Sprite *by, boolean record_action, 
										  Sprite *original_recipient, boolean size_constrained_regardless=FALSE);
	 virtual boolean respond_to_keyboard_body(unsigned char key, boolean extended, boolean robot_selected,
		                                       Sprite *by, boolean record_action, 
															Sprite *original_recipient, boolean size_constrained_regardless=FALSE) {
		 return(FALSE);
	 };
	 Sprite *select(TTRegion *region, SelectionReason reason, Sprite *in_hand);
 	 void adjust_grasp(city_coordinate &adjustment_x, city_coordinate &adjustment_y);
	 void good_size(city_coordinate &good_width, city_coordinate &good_height, Sprite *source=NULL, boolean for_expander=FALSE);
	 boolean equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, 
		               SpritesPointer &expected_corresponding_nests,
							EqualReason reason=NORMAL_EQUAL);
 	 ReleaseStatus item_released_on_top(Sprite *item, Sprite *by, SpritePointer &recipient, int level);
	 boolean receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item);
	 virtual ArithmeticException accept_result(NumberValue *result,
															 Sprite *item, Sprite *by, millisecond duration, 
															 Sprite *original_recipient, Sprite *original_item,
															 boolean delete_item=TRUE);
	 ArithmeticException result_of_operation(Sprite *other, 
														  NumberValue *my_number_value, NumberValuePointer &result,
														  Sprite *by=NULL, boolean performing_operations=FALSE);
	 ArithmeticException result_of_operation_once(Sprite *other, 
															    NumberValue *my_number_value, NumberValuePointer &result,
															    Sprite *by);
	 virtual void set_value(NumberValue *new_value,
									boolean ignore_change=FALSE,
									boolean sizes_constrained=FALSE,
									boolean update=TRUE) = 0;
	 void set_long_value(long new_value, // was virtual and abstract prior to 020302 and called set_value
							   boolean ignore_change=FALSE,
							   boolean sizes_constrained=FALSE,
							   boolean update=TRUE);
	 void set_double_value(double new_value, 
								 boolean ignore_change=FALSE, 
								 boolean sizes_constrained=FALSE,
								 boolean update=TRUE);
  	 boolean set_value_as_string(string text, int text_length=-1, NumberFormat number_format=TT_UNDETERMINED_NUMBER_FORMAT, 
										  boolean size_constrained=FALSE, boolean ignore_operation=FALSE);
	 virtual NumberValue *pointer_to_value() { // maybe value should be moved up from Rational instead - new on 050803
		 return(NULL);
	 };
	 MatchStatus match(Number *number, int version);
	 void set_to_future_value(Sprite *other, Sprite *original_recipient, Sprite *by);
	 Sprite *used(Sprite *subject, Sprite *by,
					  boolean button_unchanged, boolean record_action,
					  millisecond duration, 
					  boolean modified, Sprite *original_subject);
	 Sprite *selected(SelectionReason reason, Sprite *by);
	 void java(java_stream &out);
    boolean to_windows(Pointer *data, short int &data_offset,
                       ascii_string types, short int &types_offset,
                       BirdPointer *bird, short int &bird_count);
	 void set_size_and_location(city_coordinate new_width,
										 city_coordinate new_height,
										 city_coordinate new_llx,
										 city_coordinate new_lly,
										 ChangeReason reason=NO_REASON_GIVEN);
	 void describe_briefly(output_stream &message, Article article=INDEFINITE_ARTICLE, 
								  boolean capitalize_first=FALSE);
	 void generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection);
	 boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
	 virtual void dump_value(output_stream &stream) = 0; // new on 310102
	 virtual void negate() = 0; // new on 090202 -- commented out on 030302 since only Rational::negate is needed
	 virtual int sign() = 0; // new on 090202 -- updated 100202 to return -1, 0, or 1
	 virtual NumberValue *perform_operations(long start_value) {
		 return(new LongNumberValue(start_value));
	 };
	 virtual unsigned char number_dump_type(boolean &dump_language);
	 void update_narrow_text(long long_value, string narrow_text, int narrow_text_length);
	 virtual boolean textual() { 
		 return(FALSE);
	 };
	 boolean can_be_a_number() {
		  return(TRUE);
	 };
	 boolean can_be_text() {
		  return(TRUE);
	 };
	 virtual boolean is_remote_picture() {
		 return(FALSE);
	 };
	 city_coordinate return_character_width() { // new on 170201
		 return(character_width);
	 };
	 void set_character_width(city_coordinate new_width) {
 #if TT_DEBUG_ON
		 if (debug_counter == tt_debug_target) {
			 tt_error_file() << "setting character width of debug target." << endl;
		 };
#endif
		 character_width = new_width;
//		 decimal_places = 0; // new on 150803
	 };
	 city_coordinate return_character_height() { // new on 170201
		 return(height);
	 };
 	 void set_character_height(city_coordinate new_height) {
//#if TT_DEBUG_ON
//		 debug_log(debug_counter,"setting character height of debug target.");
//		 //if (new_height > height) {
//			// tt_error_file() << "can't have character height greater than total height" << endl;
//		 //};
//#endif
		 character_height = new_height;
//		 decimal_places = 0; // new on 150803
	 };
	 boolean ok_to_dump_as_text() {
		 // prior to 100202 was (operation == '+' || operation == '-');
		 return(operation == NO_NUMBER_OPERATION && exact() && decimal_places == 0 &&
				  integer_and_fraction_format && decimal_notation_if_denominator_is_power_of_ten); // new on 010704
	 };
	 color appropriate_background_color() {
		 return((operation == NO_NUMBER_OPERATION)?number_background_color:operation_background_color); 
	 };
	 color_index appropriate_background_color_index() {
		 return((operation == NO_NUMBER_OPERATION)?tt_nearest_number_background_color:tt_nearest_operation_background_color);
	 };
	 boolean change_size_to_fit_extent() { // new on 180102
		 return(change_size_to_fit_extent_flag);
	 };
	 void set_change_size_to_fit_extent(boolean new_flag) {
		 change_size_to_fit_extent_flag = (flag) new_flag;
		 if (new_flag) decimal_places = 0; // force recomputation - new on 080803
	 };
	 boolean change_font_size_to_fit_extent() { // new on 180102
		 return(change_font_size_to_fit_extent_flag);
	 };
	 void set_change_font_size_to_fit_extent(boolean new_flag) {
		 change_font_size_to_fit_extent_flag = (flag) new_flag;
	 };
	 //void unselect() { // noticed obsolete on 080205
		// first_key_since_selection = TRUE;
	 //};
	 boolean blankable() {
		 return(!blank);
	 };
	 PlateState plate_state() {
       return((operation==NO_NUMBER_OPERATION)?NUMBER_PAD_WITH_PLATE:OPERATION_PAD_WITH_PLATE);
    };
	 virtual boolean read_only() {
		 return(FALSE); // not even a remote
	 };
	 NumberOperation true_operation() {
		return(operation);
	 };
	 void set_true_operation(NumberOperation new_operation) { // new on 110604
		 operation = new_operation;
	 };
//	 NumberOperation current_operation(); // don't need both versions anymore (100202)
	 virtual void set_operation(NumberOperation new_operation, boolean sizes_constrained=FALSE);
	 // was virtual prior to 030302 and restored on 011105
//	 void notice_on_screen_change(boolean on_screen);
//	 void update_text_and_widths(boolean sizes_constrained);
//	 virtual void update_text_string() {}; //int min_text_length=0);
	 virtual void add_operation(Number *operation) { 
	 };
	 void become_blank(boolean by_user=FALSE, boolean controlee_too=TRUE);
    virtual NaturalLanguage natural_language();
	 virtual void set_language(NaturalLanguage l) {
    }; // nothing by default
    virtual short int return_joystick_number() {
       return(-1); // has none by default
    };
    virtual boolean hyperbot_remote() {
       return(FALSE);
    };
    virtual boolean user_media() {
       return(FALSE);
    };
    void indirection_of_set_parameter(short int new_parameter, boolean warn=TRUE, boolean reset_cycle_regardless=FALSE);
    void set_color(color_index new_color, boolean updating_default=FALSE); // updating_default added 130202
	 color_index return_text_color() {
		 return(text_color);
	 };
	 //virtual boolean return_punctuate_every_three_digits() {
		// return(FALSE); // new default as of 050202
	 //};
	 void need_to_recompute_text(boolean sizes_constrained=FALSE);
	 virtual void print_value(output_stream &message); // maded virtual on 281004
	 void set_following_operation(Number *operation);
	 void just_set_following_operation(Number *operation) {
		 following_operation = operation;
	 };
	 Number *pointer_to_following_operation() {
		 return(following_operation);
	 };
	 int add_operations_to_string(string text);
	 int operation_string_length();
	 int following_operations_string_length();
	 virtual void set_exact(boolean new_flag) {}; // only Rationals deal with this - old Integers ignore it
	 virtual void and_exact(boolean new_flag) {};
	 virtual boolean exact() {
		return(TRUE); // e.g. sensors are assumed to be exact (OK?)
	 };
	 boolean is_an_operation_with_no_value() {
		 return(is_an_operation_with_no_value_flag);
	 };
	 void set_is_an_operation_with_no_value(boolean new_flag) {
		 is_an_operation_with_no_value_flag = (flag) new_flag;
	 };
	 // commented out the following when moving this up to Number
	 //virtual int return_base() {
		// return(10); // by default
	 //};
	 //virtual void set_base(int new_base) {
		// // warn??
	 //};
	 //virtual NumberFormat return_number_format() {
		// return(TT_UNDETERMINED_NUMBER_FORMAT); // shouldn't really be called
	 //};
    boolean show_all_default() { // new on 160802
        return(TRUE);
    };
#if TT_XML
	 void xml(xml_element *element, xml_document *document);
	 xml_element *xml_set_special_attributes(xml_element *element, xml_document *document); 
	 // prior to 191202 did nothing (and didn't call Sprite version)
	 boolean xml_get_special_attributes(xml_node *node);
	 boolean handle_xml_tag(Tag tag, xml_node *node);
	 boolean geometry_worth_saving() { // new on 170103 since not worth saving if is indirection
		 return(!is_picture_flag && Sprite::geometry_worth_saving());
	 };
#endif
	 virtual java_string java_value_string() {
		 return("0"); // should be handled by Rational
	 };
	 void set_shrinking_decimal_notation(boolean new_value) {
		 if (shrinking_decimal_notation_flag != new_value) {
			 shrinking_decimal_notation_flag = (flag) new_value;
			 text_length = -1; // new on 111004
		 };
	 };
	 boolean shrinking_decimal_notation() {
		 return(shrinking_decimal_notation_flag);
	 };
	 void set_base(int new_base) {
		 base = (unsigned char) new_base; // could check it is between 2 and 36
	 };
	 int return_base() {
		 return(base);
	 };
	 NumberFormat return_number_format() {
		 return(number_format);
	 };
	 virtual void canonicalize() {
	 };
	 ArrowKeyEditing arrow_key_editing_status() {
		 return(insertion_point >= 0?ARROW_KEY_EDITING_IN_PROGRESS:ARROW_KEY_EDITING_POSSIBLE_BUT_NOT_IN_PROGRESS);
	 };
	 int return_insertion_point() { // new on 120604
		 return(insertion_point);
	 };
	 boolean displays_as_pad() {
		 return(!(show_all() && text_length == 0));
	 };
#if TT_CORRECT_IMPLEMENTATION_OF_DISPLAY_X_AND_Y_AS_INTEGERS
	 virtual boolean display_as_integer() {
		 return(FALSE); // new on 130505
	 };
#endif
//	 void set_is_picture(boolean new_flag); // new on 121203
  protected:
//	 NumberValue *future_value; // moved here and made a NumberValue on 310102 -- made obsolete on 140202
//	 boolean *when_smashed; // noticed it was obsolete on 191202
//	 Mouse *mouse;
//	 char operation;
	 NumberOperation operation; // changed on 100202
	 Number *following_operation; // if operation is dropped on an operation - new on 100202
	 city_coordinate character_width, character_height; // added character_height on 150102
	 int text_length; 
	 int min_text_length;
    color_index text_color;
	 int decimal_places; // new on 230603
//	 flag first_key_since_selection : 1;
	 flag change_size_to_fit_extent_flag : 1; // new on 110102
	 flag change_font_size_to_fit_extent_flag : 1;
//	 flag exact_flag : 1; // new on 100202 -- inexact if the result of trig or square root or the like -- 
	 // is now a property of the value
	 flag using_default_text_color : 1; // new on 130202
	 flag is_an_operation_with_no_value_flag : 1; // new on 250202 for "numbers" like "*"
	 flag shrinking_decimal_notation_flag : 1; // new on 230603 -- this is whether to fall back on shrinking decimals
//	 flag display_as_shrinking_decimal : 1; // this one is whether it is needed - i.e. no exact decimal possible
	 // following moved up here from Rational
	 flag integer_and_fraction_format : 1;
	 flag decimal_notation_if_denominator_is_power_of_ten : 1;
	 flag edited : 1; // new on 080604
#if TT_SHRINKING_AND_GROWING
	 flag display_with_size_unconstrained : 1; // new on 061104 to deal with shrinking decimal better
#endif
//	 flag punctuate_every_three_digits : 1; // e.g. 1,000,000 -- made obsolete on 191202
	 char base; // must be between 2 and 36 (true of mpz as well as itoa)
	 NumberFormat number_format;
    // new on 080803 since if in box or the like has unique value
	 // renamed from number_shrinkage on 261004 since is more general 
	 // and may need different values for integer part and decimal part or
	 // numerator and denominator
#if TT_SHRINKING_AND_GROWING
	 double part1_shrinkage; 
	 double part2_shrinkage;
#else
	 double number_shrinkage;
#endif
	 int insertion_point; 
	 // new on 090804 to support arrow key editing -- 
	 // consider optimizing this so only waste space if needed - note that robots can do this simultaneously
};

class Rational : public Number {
	public:
		Rational(city_coordinate center_x=0, city_coordinate center_y=0,
				   long priority=0,
				   city_coordinate width=maximum_width(NUMBER_PAD_SPRITE),
				   city_coordinate height=0,
				   long initial_value=0,
				   NumberOperation operation=NO_NUMBER_OPERATION,
  				   int min_text_length=0,
				   int text_color=-1); // color_index text_color=tt_black);
//		Rational(city_coordinate center_x, city_coordinate center_y, // new on 010202 - used by *load_number
//					long initial_priority,
//					city_coordinate width,
//					city_coordinate height,
//					string text, int text_length, NumberFormat number_format, byte format_flags, byte base,
//					NumberOperation operation,
//					int min_text_length=0,
//               int text_color=-1); // color_index text_color=tt_black);
		Rational(city_coordinate center_x, city_coordinate center_y, // new on 010202 - used by *load_number
					long initial_priority,
					city_coordinate width,
					city_coordinate height,
					NumberValue *value, 
					NumberFormat number_format, byte format_flags, byte base,
					NumberOperation operation,
					int min_text_length=0,
               int text_color=-1); // color_index text_color=tt_black);
		Rational(city_coordinate center_x, city_coordinate center_y,
				   long priority,
				   city_coordinate width,
				   city_coordinate height,
					NumberValue *initial_value,
					NumberFormat number_format, // added 020702
				   NumberOperation operation=NO_NUMBER_OPERATION,
  				   int min_text_length=0,
				   int text_color=-1); // color_index text_color=tt_black);
//		Rational(double initial_value);
	 void prepare_for_deletion();
	 void release_all_references();
	 Sprite *copy(boolean also_copy_offsets=FALSE);
	 boolean respond_to_keyboard_body(unsigned char key, boolean extended, boolean robot_selected, Sprite *by, 
												 boolean record_action, Sprite *original_recipient, 
												 boolean size_constrained_regardless=FALSE);
	 void canonicalize();
	 Sprite *released(Sprite *by, boolean top_level=TRUE, SpecialDrop special_drop=NOT_SPECIAL_DROP);
//	 void become(Sprite *other);
	 boolean current_value(NumberValuePointer &the_value, boolean &shared, Sprite *by=NULL);
	 boolean current_text(const_string &the_text, long &the_text_length);
	 boolean current_wide_text(wide_string &the_text, long &the_text_length);
	 void set_value(NumberValue *new_value,
						 boolean ignore_change=FALSE,
						 boolean sizes_constrained=FALSE,
						 boolean update=TRUE);
//	 void update_text_string() {}; // do nothing
	 void dump_value(output_stream &stream);
	 NumberValue *pointer_to_value() { // caller shouldn't abuse this
		 return(value);
	 };
	 void need_text_of_length(int needed_text_length, boolean copy_old=FALSE);
	 void current_style(flag &the_integer_and_fraction_format, flag &the_decimal_notation_if_denominator_is_power_of_ten, 
							  flag &the_shrinking_decimal_notation) {
		 // flag &the_shrinking_decimal_notation was missing prior to 071104
		 the_integer_and_fraction_format = integer_and_fraction_format;
		 the_decimal_notation_if_denominator_is_power_of_ten = decimal_notation_if_denominator_is_power_of_ten;
		 the_shrinking_decimal_notation = shrinking_decimal_notation_flag;
//		 the_punctuate_every_three_digits = punctuate_every_three_digits;
	 };
	 void set_current_style(flag new_integer_and_fraction_format, flag new_decimal_notation_if_denominator_is_power_of_ten, 
									flag shrinking_decimal_notation) {
		 integer_and_fraction_format = new_integer_and_fraction_format;
		 decimal_notation_if_denominator_is_power_of_ten = new_decimal_notation_if_denominator_is_power_of_ten;
		 shrinking_decimal_notation_flag = shrinking_decimal_notation;
//		 punctuate_every_three_digits = new_punctuate_every_three_digits;
	 };
	 virtual boolean add_to_keyboard_buffer(char key, boolean extended); // made virtual on 270704
	 void unselect();
	 void send_keyboard_buffer();
	 boolean keyboard_buffer_empty() {
		 return(keyboard_session_start < 0);
	 };
	 //boolean return_punctuate_every_three_digits() {
		// return(punctuate_every_three_digits);
	 //};
	 void negate();
	 int sign();
	 void set_exact(boolean new_flag) { // obsolete??
		 value->set_exact(new_flag);
	 };
	 void and_exact(boolean new_flag) {
		 value->and_exact(new_flag);
	 };
	 boolean exact() {
		 return(value->exact());
	 };
#if TT_XML
//    xml_element *xml_create_element(xml_document *document);
	 void xml(xml_element *element, xml_document *document);
	 xml_element *xml_set_special_attributes(xml_element *element, xml_document *document);
	 boolean xml_get_special_attributes(xml_node *node);
    boolean handle_xml_tag(Tag tag, xml_node *node);
	 wide_string xml_tag() { // new on 090703
		 return(value->xml_tag());
	 };
	 void just_set_narrow_text(wide_string text, int text_length);
#endif
	 java_string java_value_string();
	 void print_value(output_stream &message);
	 void set_edited(boolean new_value);
	protected:
		NumberValue *value; // changed to NumberValue on 300102 to be flexible about internal representations
		wide_string wide_text;
		string narrow_text; // new on 010202 - acts like a cache
		int text_max_length;
//		int slash_location, space_location, decimal_location; // so can split up wide_text when needed
		// following new on 030202
//		string keyboard_buffer; // new on 050202 to deal with intermediate incorrect format states - e.g. typing ".0001"
		int keyboard_session_start; 
		// beginning 080202 I use the wide and narrow text so you can see the buffer and then remove it if need be
};

const int min_remote_integer_text_length = 4;

class Remote_Integer : public Rational {
  public:
	 Remote_Integer(Picture *controlee,
						 RemoteIdentifier identifier,
						 city_coordinate center_x=0, city_coordinate center_y=0,
						 long priority=0,
						 city_coordinate width=maximum_width(NUMBER_PAD_SPRITE)*min_remote_integer_text_length,
						 city_coordinate height=maximum_height(NUMBER_PAD_SPRITE),
						 NumberValue *initial_value=NULL,
						 boolean active=TRUE,
						 NumberOperation operation=NO_NUMBER_OPERATION,
						 int min_text_length=min_remote_integer_text_length);
	 void release_all_references();
	 Sprite *copy(boolean also_copy_offsets=FALSE);
	 virtual Sprite *copy_for(Picture *controlee, boolean also_copy_offsets);
    void java(java_stream &out);
    virtual java_string java_remote_class_name();
    void java_before(java_stream &out);
	 boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
	 void dump_value(output_stream &stream);
	 void generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection);
	 void set_value(NumberValue *new_value,// int min_text_length=0,
						 boolean ignore_change=FALSE,
						 boolean sizes_constrained=FALSE,
						 boolean update=TRUE);
	 void set_operation(NumberOperation new_operation, 
							  boolean sizes_constrained=FALSE);
    boolean current_value(NumberValuePointer &the_value, boolean &shared, Sprite *by=NULL);
//#if !TT_SHRINKING_AND_GROWING
	 boolean current_wide_text(wide_string &the_text, long &the_text_length);
//#endif
	 void add_operation(Number *operation);
	 NumberValue *perform_operations(long start_value);
//	 void set_background(Background *new_floor);
	 void changed_floor(Background *controlee_floor);
	 void changed_vacuum_status(boolean controlee_in_vacuum);
	 void become_blank(boolean by_user=FALSE, boolean controlee_too=TRUE);
	 void become_non_blank(boolean controlee_too=TRUE);
//	 boolean blankable();
	 Sprite *used(Sprite *subject, Sprite *by,
					  boolean button_unchanged, boolean record_action,
					  millisecond duration, 
					  boolean modified, Sprite *original_subject);
	 void controlee_deleted();
	 void inside_thought_bubble(); // bad name should be set_inside...
//	 boolean inside_sprite(); // noticed obsolete on 131202
	 void now_on_back_of(Picture *picture);
//    Sprite *selected(SelectionReason reason, Sprite *by);
	 Sprite *now_inside_thought_bubble(boolean in_training);
	 Sprite *respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected, Sprite *by,
									     boolean record_action,  Sprite *original_recipient,
                                boolean size_constrained_regardless=FALSE);
	 RemoteStatus remote_status();
	 ChangeFrequency change_frequency();
	 void note_that_team_is_about_to_run_again();
//    void update_text_string();
	 void set_inside_vacuum(boolean new_flag, boolean recur=TRUE);
    PlateState plate_state();
	 RemoteIdentifier return_identifier() {
		return(identifier);
	 };
	 void set_controlee(Picture *new_controlee);
	 Picture *pointer_to_controlee() { // new on 060601
		 return(controlee); 
	 };
	 //void set_different_floors_blanked(boolean new_flag) {
		// different_floors_blanked = (flag) new_flag;
	 //};
	 void set_controlee_vacuum_blanked(boolean new_flag) {
		 controlee_vacuum_blanked = (flag) new_flag;
	 };
	 void set_controlee_deleted_blanked(boolean new_flag) {
		 controlee_deleted_blanked = (flag) new_flag;
	 };
	 boolean read_only() {
		 return(read_only_flag);
	 };
//	 void set_read_only(boolean new_flag) {
//		 read_only_flag = (flag) new_flag;
//	 };
	 void set_operations(Sprites *new_operations) { 
		 // should only be called to initialize when operations is NULL
		 operations = new_operations;
	 };
	 boolean global();
	 void changed_by_keyboard();
	 void set_active(boolean new_flag, boolean recur=TRUE, boolean initialization=FALSE);
	 virtual void update_value(); // made virtual on 210803
#if TT_CORRECT_IMPLEMENTATION_OF_DISPLAY_X_AND_Y_AS_INTEGERS
	 boolean display_as_integer();
#endif
    boolean hyperbot_remote() {
#if TT_32
        return(FALSE);
#else
        if (controlee != NULL) return(controlee->hyperbot_remote());
        return(FALSE);
#endif
    };
    short int return_joystick_number();
	 boolean textual();
	 //boolean receive_item(Sprite *, Sprite *by, millisecond ,
  //                       Sprite *original_recipient, Sprite *original_item);
	 virtual void update_picture(boolean regardless=FALSE);
	 virtual void set_appearance(Sprite *new_appearance) {
	 };
	 void display(SelectionFeedback selection_feedback=NO_FEEDBACK,
					  boolean followers_too=TRUE, TTRegion *region=NULL);
	 boolean equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, SpritesPointer &expected_corresponding_nests,
							EqualReason reason=NORMAL_EQUAL);
//	 long dump_value();
	 Sprites *pointer_to_operations() {
		 return(operations);
	 };
	 void restore_active(boolean new_flag);
	 boolean ok_to_dump_as_text() {
		 return(FALSE);
	 };
//	 void set_global(boolean new_flag) {
//		 global_flag = (flag) new_flag;
//	 };
#if TT_XML
//    xml_element *xml_create_element(xml_document *document);
	 wide_string xml_tag() {
		 return(L"RemoteControl");
	 };
    void xml(xml_element *element, xml_document *document);
    boolean handle_xml_tag(Tag tag, xml_node *node);
	xml_element *xml_set_special_attributes(xml_element *element, xml_document *document);
	boolean xml_get_special_attributes(xml_node *node);
	//boolean ok_to_save_in_include_file() {
	//	return(FALSE);
	//};
#endif
//    void set_temporarily_inactive(boolean become_inactive);
    void set_in_notebook(Notebook *notebook, boolean recur=TRUE); // new on 200802
	 void set_version(int new_version) {
		 version = new_version;
	 };
	 boolean size_constrained() {
		 return(TRUE); // new on 180804 so sensors don't change size when on the floor
	 };
  protected:
	 Picture *controlee;
//    Picture *saved_controlee; // noticed it was obsolete on 201202
	 RemoteIdentifier identifier;
	 Sprites *operations;
//	 Sprite *saved_by;
//	 flag different_floors_blanked : 1; // blanked due to "distance" - noticed obsolete on 201202
	 flag controlee_vacuum_blanked : 1; 
	 flag controlee_deleted_blanked : 1;
	 flag read_only_flag : 1; // could make this a subclass instead
	 flag display_as_double : 1; // new on 161202 -- name should really be display_as_rational -- noted 130505
//    flag temporarily_not_active_flag : 1; 
	 // new on 200802 should be TRUE only if should be active but turned off for now (e.g. in the notebook)
//	 flag global_flag : 1; // e.g. mouse_speed_x -- could compute from identifier
	 int version; 
	 // new on 120504 so ANCESTOR_REMOTE can have a new behaviour without breaking old code -- 
	 // could be moved to Remote_Integer if needed
};

class Remote_Picture : public Remote_Integer {
  public:
	 Remote_Picture(Picture *controlee,
						 RemoteIdentifier identifier,
						 city_coordinate center_x=0, city_coordinate center_y=0,
						 long priority=0,
						 city_coordinate width=5*tile_width,
						 city_coordinate height=5*tile_height,
#if TT_XML
                   long initial_value=0,
#else
						 long initial_value=-1, // no picture
#endif
						 NaturalLanguage natural_language=tt_language,
						 boolean active=TRUE,
						 boolean viewing_front_side=TRUE);
	 virtual Remote_Picture *just_copy(Picture *controlee);
	 void release_all_references();
	 Sprite *copy_for(Picture *controlee, boolean also_copy_offsets);
    java_string java_remote_class_name();
    boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
#if TT_XML
//    xml_element *xml_create_element(xml_document *document);
    void xml(xml_element *element, xml_document *document);
	 void add_special_xml(xml_element *element, xml_document *document);
    boolean handle_xml_tag(Tag tag, xml_node *node);
	 boolean geometry_worth_saving() { 
		 // new on 170103 since numbers are not worth saving if is indirection but this needs to skip over that
		 return(Sprite::geometry_worth_saving());
	 };
#endif
	 boolean numeric_value_meaningful();
	 void set_value(NumberValue *new_value, //int min_text_length=0,
						 boolean ignore_change=FALSE,
						 boolean sizes_constrained=FALSE,
						 boolean update=TRUE);
//	 void update_text_and_widths(boolean sizes_constrained);
//	 void update_widths(boolean sizes_constrained) {}; // nothing
	 ReleaseStatus item_released_on_top(Sprite *item, Sprite *by, 
													SpritePointer &recipient, int level);
	 boolean receive_item(Sprite *, Sprite *by, millisecond , Sprite *original_recipient, Sprite *original_item);
    Sprite *used(Sprite *subject, Sprite *by,
					  boolean button_unchanged, boolean record_action,
					  millisecond duration, 
					  boolean modified, Sprite *original_subject);
	 Sprite *respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected, Sprite *by,
									     boolean record_action,  Sprite *original_recipient,
                                boolean size_constrained_regardless=FALSE);
	 boolean add_to_keyboard_buffer(char key, boolean extended) {
		 return(FALSE); // do nothing -- new on 270704 -- updated 270804
	 };
    void set_size_and_location(city_coordinate new_width,
										 city_coordinate new_height,
										 city_coordinate new_llx,
										 city_coordinate new_lly,
										 ChangeReason reason=NO_REASON_GIVEN);
	 void update_display(const UpdateStatus update_status=UPDATE_CHANGE);
	 void display(SelectionFeedback selection_feedback=NO_FEEDBACK,
					  boolean followers_too=TRUE, TTRegion *region=NULL);
	 void now_on_top_of_nest(boolean just_arrived);
	 void update_value();
	 void update_picture(boolean regardless=FALSE);
	 virtual string narrow_text_value(NaturalLanguage language); // moved virtual down here on 280102 -- added arg on 130702
    NaturalLanguage natural_language();
	 virtual boolean text_value_need_updating() {
		return(TRUE);
	 };
    void add_follower(Sprite *follower, boolean completely_inside=TRUE, InsertionOrdering insertion_ordering=INSERT_AT_END, 
							 boolean dont_record_new_offsets=FALSE, AddFollowerReason reason=NO_ADD_FOLLOWER_REASON_GIVEN);
	 boolean remove_follower(Sprite *sprite, boolean warn_if_not_follower=TRUE);
	 void good_size(city_coordinate &good_width, city_coordinate &good_height, Sprite *source=NULL, boolean for_expander=FALSE);
	 void generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection);
	 void java_before(java_stream &out);
	 void java(java_stream &out);
	 Sprite *now_inside_thought_bubble(boolean in_training);
    void become_blank(boolean by_user=FALSE, boolean controlee_too=TRUE);
	 void become_non_blank(boolean controlee_too=TRUE);
	 MatchStatus match(Number *number, int version); // was Integer prior to 020302
	 Sprite *dereference();
	 boolean can_be_text();
	 boolean current_text(const_string &the_text, long &the_text_length);
	 boolean current_wide_text(wide_string &the_text, long &the_text_length);
	 void set_background(Background *new_floor, boolean recur=FALSE, boolean warn=TRUE);
    Sprite *select(TTRegion *selection_region, SelectionReason reason, Sprite *in_hand);
//	 Sprite *appearance_before_vacuuming();
//	 void reset_appearance_before_vacuuming();
	 Sprite *selected(SelectionReason reason, Sprite *by);
	 Path *path_to(Sprite *target, boolean &ok);
    void close_current_sound();
    void set_inside_vacuum(boolean inside, boolean recur=TRUE);
	 void set_to_future_value(Sprite *other, Sprite *original_recipient, Sprite *by);
    void set_text_appearance_size(Sprite *text);
    void set_appearance_to_animated_text(wide_string text_string);
    void set_appearance_to_MCI_sound(wide_string text_string);
    void set_appearance_to_speech_sound(wide_string text_string);
    void set_appearance_to_wave_sound(wide_string text_string);
    boolean user_media();
//    void set_clip_region(SelectionFeedback selection_feedback=NO_FEEDBACK,
//								 TTRegion *region=NULL);
	 boolean has_appearance() {
		 return(appearance != NULL);
	 };
	 boolean copy_set_appearance(Sprite *new_appearance);
	 void set_appearance(Sprite *new_appearance);
    Sprite *pointer_to_appearance() {
       return(appearance);
    };
	 virtual boolean is_remote_address() {
		 return(FALSE);
	 };
	 virtual long return_avenue_number() {
		 return(-1); // isn't
	 };
	 virtual long return_street_number() {
		 return(-1); // isn't
	 };
    void set_language(NaturalLanguage language);
    city_coordinate true_width(boolean including_followers=FALSE);
    city_coordinate true_height(boolean including_followers=FALSE);
    boolean ok_for_mouse_to_temporarily_add_follower();
	 boolean can_be_a_number();
	 MatchStatus match(Remote_Picture *number, int version);
	 void set_active(boolean new_flag, boolean recur=TRUE, boolean initialization=FALSE);
	 boolean is_remote_picture() {
		 return(TRUE);
	 };
	 boolean viewing_front_side() {
		 return(viewing_front_side_flag);
	 };
	 wide_string text_value();
    boolean enumeration_type(); // has only a few values
    void set_cause_of_match_failure(boolean new_flag, boolean recur);
//	 boolean equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, 
	                  // SpritesPointer &expected_corresponding_nests,
							// EqualReason reason=NORMAL_EQUAL);
	 boolean size_is_true(); // new on 131202
	 void set_true_size_and_location(city_coordinate new_width, city_coordinate new_height,
												city_coordinate new_llx, city_coordinate new_lly,
												ChangeReason reason=NO_REASON_GIVEN); // new on 131202
	 boolean decoration_remote();
	 void follower_used_in_decoration_changed(Sprite *follower);
  protected:
//	 Picture *picture;
	 Sprite *appearance;
//    void *extra_pointer; // for sound_buffer for example -- sound buffer no longer saved
	 // following is much cleaner and easier to save in XML but it is 2 pointers rather than 1 (sigh)
	 House *sensor_applied_to_house; // replaces extra_pointer on 201202
	 Sprites *saved_controlees_followers; // replaces extra_pointer on 201202
    NaturalLanguage language; // how many bits??
	 Sprite *original_of_appearance; // just to avoid redundant work ...
	 flag viewing_front_side_flag; // new on 160101
};

class Force_Remote_Integer: public Remote_Integer {
  public:
	 Force_Remote_Integer(Picture *controlee,
						       RemoteIdentifier identifier,
						       city_coordinate center_x=0, city_coordinate center_y=0,
						       long priority=0,
						       city_coordinate width=maximum_width(NUMBER_PAD_SPRITE)*min_remote_integer_text_length,
                         city_coordinate height=maximum_height(NUMBER_PAD_SPRITE),
						       NumberValue *initial_value=NULL,
								 boolean active=TRUE,
						       NumberOperation operation=NO_NUMBER_OPERATION,
						       int min_text_length=min_remote_integer_text_length) :
    Remote_Integer(controlee,
						 identifier,
						 center_x,center_y,
						 priority,
						 width,height,
						 initial_value,
						 active,
                   operation,
						 min_text_length) {
    };
    unsigned char number_dump_type(boolean &dump_language) {
       return(FORCE_REMOTE_INTEGER);
    };
    boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
	 void set_active(boolean new_flag, boolean recur=TRUE, boolean initialization=FALSE) { 
		 // ignores this since can't be turned off - and dumping identifier and not active flag
	 };
	 void just_set_inactive() {
		 // ignore -- new on 140605
	 };
//#if TT_XML
//    wide_string xml_tag() {
//       return(L"ForceRemoteControl");
//    };
//#endif
};

class Remote_Address : public Remote_Picture {
  public:
	 Remote_Address(Picture *controlee, // ignored?
						 RemoteIdentifier identifier, // either return_avenue_number or STREET_REMOTE
						 city_coordinate center_x=0, city_coordinate center_y=0,
						 long priority=0,
						 city_coordinate width=5*tile_width,
						 city_coordinate height=5*tile_height,
#if TT_XML
                   long initial_value=0,
#else
						 long initial_value=-1, // no picture
#endif
						 NaturalLanguage language=tt_language,
						 boolean active=TRUE);
	 Remote_Address(RemoteIdentifier identifier);
	 Remote_Picture *just_copy(Picture *controlee);
	 void set_background(Background *new_floor, boolean recur=FALSE, boolean warn=TRUE);
	 void update_picture(boolean regardless=FALSE);
	 void set_value(NumberValue *new_value,
						 boolean ignore_change=FALSE, boolean sizes_constrained=FALSE,
						 boolean update=TRUE);
	 void set_to_future_value(Sprite *other, Sprite *original_recipient, Sprite *by);
	 void set_active(boolean new_flag, boolean recur=TRUE, boolean initialization=FALSE);
	 const_string narrow_text_value(NaturalLanguage language);
	 boolean text_value_need_updating() {
		 return(!text_value_up_to_date);
	 };
	 boolean is_remote_address() {
		 return(TRUE);
	 };
	 long return_avenue_number();
	 long return_street_number();
	 long return_address_number();
	 void changed_by_keyboard() {
		 set_active(FALSE);
	 };
//	 void dump(output_stream &stream, boolean just_prepare=FALSE);
//	 void set_value_tracks_location(flag new_flag);
	private:
	 Block *current_block; // no need to save this since used to determine whether to recompute
	 flag text_value_up_to_date : 1;
//	 flag value_tracks_location : 1; // subsumed by active_flag
};

Number *load_number(SpriteType type, InputStream *pad_in,
						  int notebook_version, NaturalLanguage language, Sprite *container,
                    Picture *controlee, long index, boolean descendants_inactive);
Sprites *load_operations(InputStream *pad_in, 
								 int notebook_version, NaturalLanguage language,
                         Picture *controlee);
boolean remote_picture(RemoteIdentifier identifier);
boolean remote_address(RemoteIdentifier identifier);

boolean describe_number(SpriteType type, InputStream *pad_in, output_stream &text_out, int version_number,
							   boolean in_thought_bubble, Article article);
boolean describe_operations(InputStream *pad_in, output_stream &text_out, int version_number, boolean in_thought_bubble); 
// older sense of operations (applied to read-only remotes)
boolean valid_operation_key(char op);
NumberOperation operation_from_key(char key, boolean plus_is_increase_by=FALSE);
string operation_to_string(NumberOperation operation);
NumberOperation operation_from_string(string operation_string);
NumberOperation operation_from_string(wide_string operation_string);

void position_and_add_to_house(Sprite *decoration_remote, Sprite *addition, House *house, boolean from_side);
void position_and_add_to_wall(Sprite *wall_remote, Sprite *addition, Room *room);

boolean text_identifier(RemoteIdentifier identifier);
boolean has_yes_or_no_values(RemoteIdentifier identifier);

#if TT_XML
boolean force_feedback_effect(RemoteIdentifier identifier);
#endif

void release_last_yes_no();

//void initialize_digits();
//void display_integer(Screen *screen,
//							long value,
//							city_coordinate width, city_coordinate height,
//							city_coordinate llx, city_coordinate lly,
//							char operation='+',
//							boolean show_plate=FALSE,
//							boolean error_number=FALSE);

/*
class Unsigned_Short_Number : public Number {
  public:
	Unsigned_Short_Number(city_coordinate center_x, city_coordinate center_y,
								 long priority,
								 Background *floor,
								 city_coordinate width=maximum_width(NUMBERPAD)/2,
//                       city_coordinate height=maximum_height(NUMBERPAD)/2,
								 Sprite **digits=NULL, int digits_in_use=0) :
	 Number(center_x,center_y,priority,floor,
			  width,digits,digits_in_use,
			  5,0,max_unsigned_short) {
	};
};

class Unsigned_Long_Number : public Number {
  public:
	Unsigned_Long_Number(city_coordinate center_x, city_coordinate center_y,
					long priority,
					Background *floor,
					city_coordinate width=maximum_width(NUMBERPAD)/2,
//             city_coordinate height=maximum_height(NUMBERPAD)/2,
					Sprite **digits=NULL, int digits_in_use=0) :
	 Number(center_x,center_y,priority,floor,
			  width,digits,digits_in_use,
			  10,0,max_unsigned_long) {
	 };
};
*/

//void display_error_number(Screen *screen,
//                          city_coordinate width, city_coordinate height,
//                          city_coordinate llx, city_coordinate lly);
#endif
