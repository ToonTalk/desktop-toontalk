// Copyright (c) 1992-2006. Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif   
#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif   
#if !defined(__TT_FLOOR_H)   
#include "floor.h"
#endif   
#if !defined(__TT_PAD_H)   
#include "pad.h"
#endif
#if !defined(__TT_EVENT_H)   
#include "event.h"
#endif   
#if !defined(__TT_ROBOT_H)   
#include "robot.h"
#endif   
#if !defined(__TT_TEXT_H)   
#include "text.h"
#endif
#if !defined(__TT_SOUND_H)
#include "sound.h"
#endif
#if !defined(__TT_MOUSE_H)   
#include "mouse.h"
#endif   
#if !defined(__TT_PICTURE_H)   
#include "picture.h"
#endif 
#if !defined(__TT_BLOCK_H)   
#include "block.h"
#endif 
#if !defined(__TT_CITY_H)   
#include "city.h"
#endif  
#if !defined(__TT_ROOM_H)
#include "room.h"
#endif
#if !defined(__TT_PUZZLE_H)
#include "puzzle.h"
#endif
#if !defined(__TT_SPEAK_H)
#include "speak.h"
#endif
#if !defined(__TT_WINMAIN_H)   
#include "winmain.h"
#endif   
#if !TT_WINDOWS
#include "icache.h"
#endif
#if !defined(__TT_UTILS_H)   
#include "utils.h" 
#endif   
#if !defined(__TT_COMMON_H)
#include "common.h" 
#endif
#if !defined(__TT_NUMBER_H)
#include "number.h"
#endif
#if !defined(__TT_JAVA_H)
#include "java.h"
#endif
#if !defined(__TT_PRGRMMR_H)
#include "prgrmmr.h"
#endif
#if !defined(__TT_HELP_H)
#include "help.h"
#endif
#if !defined(__TT_BIRD_H)
#include "bird.h"
#endif
#if !defined(__TT_REMOTE_H)
#include "remote.h"
#endif
#if TT_XML
#if !defined(__TT_XML_H)
#include "xml.h"
#endif
#endif
#if !TT_32
#include <mmsystem.h> // for timeGetTime
#endif
#include <stdlib.h>   
//#include <strstrea.h> commented out 210703
#include <string.h>
//#include <iomanip.h>

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef Sprite *SpritePointer;

#if TT_DEBUG_ON
long integer_counter = 0;
#endif

const int extra_space = 32; // each bignum and rational number has 32 extra bytes (narrow and wide) so don't need to reallocate very often

#if TT_SHRINKING_AND_GROWING
const double fraction_of_width_between_digits = .8; 
// made a const on 291004 -- used for shrinking and growing digits to increment horizontal position
#endif

wide_string operation_to_wide_string(NumberOperation operation);

// if changed update help.cpp
enum IntegerHistory {INTEGERS_ADDITION=0,
							INTEGERS_TYPED,  
							INTEGERS_DIGITS_REMOVED,
							INTEGERS_MULTIPLICATION,
							INTEGERS_DIVISION,
							INTEGERS_MODULO,
							INTEGERS_ASSIGNMENT,
							INTEGERS_BITWISE_OPERATIONS,
							INTEGERS_MADE_NEGATIVE,
							INTEGERS_FLIPPED,
							INTEGERS_BLANK,
							INTEGERS_CONVERSION,
							INTEGERS_BLANKED_INSIDE_THOUGHT_BUBBLE};

boolean decoration_identifier(RemoteIdentifier identifier) {
   switch (identifier) {
      case ROOM_DECORATION_REMOTE:
      case HOUSE_DECORATION_REMOTE:
      case ROOF_DECORATION_REMOTE:
         return(TRUE);
   };
   return(FALSE);
};

inline boolean read_only_identifier(RemoteIdentifier identifier) {
  // read only number like mouse speed
  return((identifier >= MOUSE_SPEED_X_REMOTE &&
			 identifier <= RANDOM_REMOTE) ||
			(identifier >= END_OF_ANIMATION_REMOTE &&
			 identifier <= RELEASED_REMOTE) ||
         (identifier >= JOYSTICK_SPEED_X_REMOTE &&
          identifier <= JOYSTICK_BUTTON32_DOWN_REMOTE) ||
         decoration_identifier(identifier) ||
         (identifier >= LEGO_SENSOR1_REMOTE &&
			 identifier <= LEGO_SENSOR_COUNT8_REMOTE) ||
			 (identifier == LANGUAGE_REMOTE));
};

inline boolean double_float_identifier(RemoteIdentifier identifier) {
	// add x and y remote since they are slowly changed by speed -- still make sense??
   return(identifier == X_REMOTE || identifier == Y_REMOTE);
};

inline boolean speed_identifier(RemoteIdentifier identifier) {
	return(identifier == X_SPEED_REMOTE || identifier == Y_SPEED_REMOTE);
};

Number::Number(city_coordinate center_x, city_coordinate center_y,
               city_coordinate width, city_coordinate height, // new on 220702
					long initial_priority,
					NumberOperation initial_operation,
  				   int min_text_length,
				   int initial_text_color,
					SpriteType type,
					NumberFormat number_format,
					boolean integer_and_fraction_format,
					boolean decimal_notation_if_denominator_is_power_of_ten,
					int base) : 
	Sprite(NUMBER_PAD_SPRITE,center_x,center_y,type,
			 0,ground_scale,//FALSE, // should default these
			 width,height),
	operation(initial_operation),
//	first_key_since_selection(TRUE),
//	need_to_recompute_text(TRUE),
   min_text_length(min_text_length),
//   text_color(text_color),
	change_size_to_fit_extent_flag(FALSE), // new on 110102 to decide whether to change the font size or the pad size
	change_font_size_to_fit_extent_flag(TRUE),
	character_width(tile_height), // initial values were 300x500 prior to 190202
	character_height(tile_width), // given initial values on 220102
//	future_value(NULL),
	text_length(-1), // new on 010202 to indicated have yet to compute textual equivalent
	following_operation(NULL),
	is_an_operation_with_no_value_flag(FALSE),
	shrinking_decimal_notation_flag(TRUE),
	decimal_places(0),
	// following moved here from Rational on 050803
	number_format(number_format),
	integer_and_fraction_format(integer_and_fraction_format),
	decimal_notation_if_denominator_is_power_of_ten(decimal_notation_if_denominator_is_power_of_ten),
	edited(FALSE), // new on 080604 -- text string hasn't been edited yet
 	base(base),
#if TT_SHRINKING_AND_GROWING
	display_with_size_unconstrained(FALSE), // until discovered to be otherwise -- was TRUE prior to 071104
	part1_shrinkage(0.0),
	// following should also be initialized with 0.0
	part2_shrinkage(0.0), // prior to 051104 was tt_number_shrinkage 
#else
	number_shrinkage(tt_number_shrinkage), // overridden when shrinking_decimal_notation and not used otherwise
#endif
	insertion_point(min_long) { // new on 090604 -- is here since used in displaying any number
//	display_as_shrinking_decimal(FALSE) {
		current_priority = initial_priority;
		set_size_is_true(TRUE); // width and height are its true size
//		character_width = width; // commented out on 190202
		using_default_text_color = (initial_text_color < 0);
		if (using_default_text_color) {
			text_color = tt_black; // but will be set to gray if inexact
		} else {
			text_color = initial_text_color;
		};
};

//void Number::prepare_for_deletion() { // new on 080202
//	if (preparing_for_deletion) return; // already done
////	preparing_for_deletion = TRUE; // new on 060204
////	if (future_value != NULL) {
////		delete future_value;
////	};
////	release_all_references(); // commented out on 020504 since the following calls this now
//	Sprite::prepare_for_deletion();
//};

void Number::release_all_references() { // new on 300404
	if (releasing_all_references) { // new on 230305
		return;
	};
	// moved here from Number::prepare_for_deletion
	if (following_operation != NULL) { // new on 100202
		following_operation->destroy();
		following_operation = NULL; // new on 300404
	};
	operation = NO_NUMBER_OPERATION; // so won't show +2 if was just 2 prior to unbamming -- new on 020504
	Sprite::release_all_references(); // new on 020504
};

//char Number::current_operation() {
//	if (operation == '-') return('+'); // to be consistent with current_value
//	return(operation);
//};

//void Number::notice_on_screen_change(boolean on_screen) {
   // going on screen and clean then stay same size
//	if (on_screen) {
//		if (!clean()) { // commented out on 110102
//			update_widths(TRUE);
//		};
//		update_text_and_widths(TRUE);
//	};
//};

//void Number::update_text_and_widths(//int min_text_length,
//												 boolean sizes_constrained) {
//	update_text_string();
//	if (!size_constrained()) { // commented out on 110102
		// commented out on 221299 since now indirection if leaderless updates size && (!is_picture() || tt_log_version_number < 15)) {
		// !is_picture() added on 170299 since without it sucking and spitting made indirections 0 wide
////		width = min(character_width*text_length,max_sprite_width);
//		width = character_width*text_length;
//	};
//	update_widths(sizes_constrained); // commented out on 110102
//	set_clean_status(DIRTY); 
// still make sense?? -- commented out on 020302 since this is called during display and it is too late (e.g. leader already displayed)
//};

void Number::become_blank(boolean by_user, boolean controlee_too) {
	Sprite::become_blank(by_user,controlee_too);
	if (by_user) {
		user_did(INTEGER,INTEGERS_BLANK);
		if (contained_in(THOUGHT_BUBBLE)) {
			user_did(INTEGER,INTEGERS_BLANKED_INSIDE_THOUGHT_BUBBLE);
		};
	};
};

NaturalLanguage Number::natural_language() {
   return(tt_language);
};

void Number::indirection_of_set_parameter(short int new_parameter, boolean , boolean ) {
   text_color = (color_index) (new_parameter%tt_colors_available);
};

void Number::set_operation(NumberOperation new_operation, boolean sizes_constrained) {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Number::set_operation of debug target."));
	};
#endif
	if (operation == new_operation) return;
	operation = new_operation;
	if (operation != NO_NUMBER_OPERATION) {
		set_shrinking_decimal_notation(FALSE); // since can't display anything afterwards -- new on 120305
	};
//	if (showing_on_a_screen()) { // make sense??
//		update_text_and_widths(sizes_constrained);
//	};
	need_to_recompute_text(sizes_constrained); // arg new on 281002
};

#if TT_SHRINKING_AND_GROWING
double compute_shrinkage(double distance, int iterations) {
	if (distance <= 1.0 || iterations < 2 || (distance > max_unsigned_short && iterations > max_unsigned_short)) {
		// added distance > max_unsigned_short on 301104 since if very long then it takes forever to compute shrinking digits
		return(0.0); // don't shrink
	};
	double shrinkage = (distance-1.0)/distance; // subtract 1.0 since start with a full size digit
	double error = pow(shrinkage,iterations); // not an infinite sequence so don't count the part truncated at the end
	if (error < 1.0) { // was .5 prior to 131104 // if it is that large must be very narrow or something
		distance /= 1.0-error;
		return((distance-1.0)/distance);
	} else {
		return(shrinkage);
	};	
};

const double decimal_point_width_relative_to_a_digit = 0.18; // may depend upon the font -- empirically derived

void shrinking_decimal_parts(int integer_part_length, city_coordinate total_width, city_coordinate character_width, 
									  double &fraction_per_character, double &part1_fraction, double &part2_fraction) { 
	// new on 051104
	fraction_per_character = (double) character_width/total_width;
	part1_fraction = min(0.5,integer_part_length*fraction_per_character)
		                      -fraction_per_character*decimal_point_width_relative_to_a_digit;
	// gives up decimal_point_width_relative_to_a_digit (.18) character for the decimal point itself 
	// since that is currently how much room it takes
	part2_fraction = 1.0-part1_fraction;
};

void multiple_lines_with_integer_part_lengths(int space_index, int separator_index, int text_length, city_coordinate total_width,
															 int &numerator_length, int &denominator_length, int &max_fraction_length,
															 city_coordinate &max_integer_part) {
	numerator_length = separator_index-(space_index+1);
	denominator_length = text_length-(separator_index+1);
	max_fraction_length = max(2,max(numerator_length,denominator_length)); // need a little extra if small for line and space
	city_coordinate quarter_width = total_width/4;
	// +2 added on 091104 since need room for space -- changed to +1 on 131104 -- max_fraction_length+1 has +1 to round up
	// /2 added on 131104 since integer part is a double sized font so needs more room
	max_integer_part = min(max((space_index*total_width)/((max_fraction_length+1)/2+space_index+1),quarter_width),3*quarter_width); 
};

void Number::display(SelectionFeedback selection_feedback, boolean followers_too, TTRegion *region) {
	if (!tt_shrink_and_grow_numbers) { // new on 141104 to run old code if this flag if off (or old demos)
		display_old(selection_feedback,followers_too,region);
		return;
	};
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log("Displaying debug target.");
	};
#endif
	if (!visible()) return;
//	if (rescale_if_camera_above(selection_feedback,followers_too,region)) return; // copied here on 050102
	if (pointer_to_top_of_stack() != NULL) { // new on 260500
		pointer_to_top_of_stack()->display(selection_feedback,followers_too,region);
		return;
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	wide_string wide_text = NULL; // moved here on 050803
	long wide_text_length; // was unsigned short int prior to 090202
	if (text_length < 0 && visible() && !is_blank()) { 
		// new on 050803 so that this recomputes the text (and possibly the size) before all the rest
		// changed show_all to visible on 150803
		current_wide_text(wide_text,wide_text_length);
		//if (display_with_size_unconstrained) { // new on 081104 -- removed on 301104 since it typically looked worse than before
		//	set_to_good_size();
		//	set_change_font_size_to_fit_extent(TRUE);
		//};
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
	if (tt_debug_mode == 60504) {
		string narrow_text = copy_narrow_string(wide_text);
		tt_error_file() << "Displaying ";
		if (narrow_text == NULL) {
			tt_error_file() << "Number#" << debug_counter;
		} else {
			tt_error_file()	<< narrow_text;
		}
		tt_error_file() << " at " << llx << "," << lly << " with size " << width << "x" << height 
			             << " on frame " << tt_frame_number << endl;
	};
#endif
	if (want_feedback(selection_feedback)) {
		selection_feedback = MOVEMENT_FEEDBACK; // don't support growth
	};
//   if (text_length < 0) { // not computed yet
//      update_text_and_widths(TRUE);
//   };
	if (show_all() && text_length == 0) { // how can this happen with numbers? -- it does now (090604) while editing while holding
		Sprite::display(selection_feedback,followers_too,region); // only one plate
	} else {
#if TT_DEBUG_ON
		millisecond start_time = timeGetTime();	
#endif
		city_coordinate adjusted_llx, adjusted_lly;
		city_coordinate adjusted_width, adjusted_height;
		if (region == NULL) {
			adjusted_llx = llx;
			adjusted_lly = lly;
			adjusted_width = width;
			adjusted_height = height;
		} else {
			adjusted_llx = region->min_x;
			adjusted_lly = region->min_y;
			adjusted_width = region->width();
			adjusted_height = region->height();
		};
		city_coordinate one_x_pixel = tt_screen->one_pixel_in_x_city_units();
//		if (!change_font_size_to_fit_extent_flag && character_width < one_x_pixel) return; 
		// new on 220102 -- commented out since still need to see frame even if text is too small
      if (adjusted_width < one_x_pixel) return; // added _adjusted on 190503
		city_coordinate one_y_pixel = tt_screen->one_pixel_in_y_city_units();
		if (adjusted_height < one_y_pixel) return; // added _adjusted on 190503
		if (region == NULL && // added this condition on 070202 since if flying over a city the following test doesn't work
			// (and if part of something should already have checked if off screen??)
			 tt_screen->off_screen(adjusted_llx,adjusted_lly,adjusted_width,adjusted_height) &&
          !change_size_to_fit_extent_flag) return; // new on 160102 -- moved earlier on 220102 
		    // added !change_size_to_fit_extent_flag on 180302
#if TT_DEBUG_ON
		if (tt_debug_mode == 220102) {
			tt_error_file() << "Printed in " << adjusted_width << "x" << adjusted_height 
								 << " region with font size of " << character_width << "x" << character_height << endl;
		};
#endif
		city_coordinate adjusted_text_width = adjusted_width;
		city_coordinate adjusted_text_height = adjusted_height;
		city_coordinate edge_size = compute_edge_size(adjusted_width,adjusted_height); // is the same horizontally and vertically
		//		if (show_all()) { // new on 040102 - conditional commented out on 180102
		adjusted_text_width -= 2*edge_size;
		adjusted_text_height -= 2*edge_size;
//		};
//		character_width = adjusted_text_width/text_length; // commented out on 110102
		// following re-written on 241199 to be more accurate - does it really accomplish much??
//		character_width = ideal_horizontal_units(tt_screen->horizontal_pixel_length(character_width));  // commented out on 110102
		city_coordinate adjusted_character_width = character_width; 
		if (want_feedback(selection_feedback)) {
//		switch (selection_feedback) {
//			case NO_FEEDBACK:
//				break;
//			case MOVEMENT_FEEDBACK:
//			case RECURSIVE_MOVEMENT_FEEDBACK:
				adjusted_llx += selection_delta_x();  
				adjusted_lly += selection_delta_y();  
//				break;
		};		
//		if (adjusted_character_width == 0) { // commented out on 220102 since dealt with differently now
//			adjusted_character_width = one_x_pixel;
//		};
//		display_plated_text(text,text_length,
//								  adjusted_llx,adjusted_lly,
//								  adjusted_character_width,adjusted_height,
//								  plate_state(),show_all(),!blank,text_color,color_permutation(selection_feedback));
		bytes permutation = color_permutation(selection_feedback);
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		if (is_blank()) {
			// new on 050102
			display_plate(adjusted_llx,adjusted_lly,adjusted_width,adjusted_height,plate_state(),TRUE,permutation);
			return;
		} else {			
			color background_color;
			if (permutation != NULL) {
				if (tt_bits_per_pixel == 8) {
					// new on 080202
					background_color = compute_color(appropriate_background_color_index(),permutation);
				} else {
					background_color = alter_color24(appropriate_background_color(),(int *) permutation); // 24-bit color
				};
			} else {
				background_color = appropriate_background_color();
			};
			if (show_all() && !is_blank()) { // new on 040102
BEGIN_GDI
				tt_main_window->select_stock_object(NULL_PEN);
				HGDIOBJ old_selection = tt_main_window->select_object(tt_main_window->create_solid_brush(background_color));
				tt_screen->draw_rectangle(adjusted_llx,adjusted_llx+adjusted_width, // new on 170799
												  adjusted_lly,adjusted_lly+adjusted_height);
				if (old_selection != NULL) DeleteObject(tt_main_window->select_object(old_selection));
END_GDI
			};
#if TT_DEBUG_ON
			add_to_log_if_interesting_error();
#endif
			city_coordinate adjusted_character_width = character_width; // copied here from text::display_text on 040102
			city_coordinate adjusted_character_height = character_height;
			city_coordinate old_adjusted_text_width = adjusted_text_width;
			city_coordinate old_adjusted_text_height = adjusted_text_height;
			if (wide_text == NULL) { // not computed above
				current_wide_text(wide_text,wide_text_length);
			};
#if TT_DEBUG_ON
			add_to_log_if_interesting_error();
#endif
//			wide_string output_text; // new on 090604 -- made obsolete on 100604
//			int text_length;
//			boolean reclaim_output_text
			// = current_wide_text_with_insertion_character(insertion_point,wide_text,wide_text_length,output_text,text_length);
//			city_coordinate alternative_width, alternative_height;
//			tt_screen->correct_font_size(wide_text,wide_text_length,1,
//												  adjusted_character_width,adjusted_height,TRUE,FALSE,
//												  adjusted_text_width,adjusted_text_height,
//												  adjusted_character_width,adjusted_character_height,
//												  alternative_width, alternative_height);
			boolean need_to_change_size = FALSE;
			int ignore_longest_line; // moved here on 251004 so can be shared by multiple callers
			long ignore_new_text_height; // new on 270202 so that height isn't changed by this
			if (change_size_to_fit_extent_flag || insertion_point >= 0) { // copied here from text::display_text on 110102
				// && !change_font_size_to_fit_extent_flag) { 
				// new policy on 050202 so if change_font_size_to_fit_extent_flag then do it first
				// (so character_width etc are up-to-date for changing size)
				// undid this on 140202 since can cause numbers to be ... before being changed (e.g. float on a blank rational)
				if (size_constrained() || animation_in_progress() || region != NULL) { 
               // relationship have changed or things are changing (added region != NULL test on 081102 
					// since shouldn't change size if in a region
					// replaced leader != NULL with size_constrained() on 101204 
					// since if attached to copier and was fraction being edited looked bad
					if (leader != NULL) set_change_size_to_fit_extent(FALSE); 
					// conditional added on 050202 so that size change will happen when animation is over
//					if (!tt_shrink_and_grow_numbers) {
						set_change_font_size_to_fit_extent(TRUE);
//					};
				} else if (character_width > 0 && character_height > 0) { 
					// conditional is to avoid work when dealing with numbers too long to see the digits
#if TT_DEBUG_ON
					if (tt_debug_mode == 110302) {
						tt_error_file() << "Before get_extent_size "
											 << non_shrinking_or_integer_part_and_fraction_equivalent(wide_text,text_length) 
											 // needs wide_text arg as of 131104
//											 << non_shrinking_equivalent(text_length)
											 << " length with character width " 
											 << character_width << "X" << character_height
											 << " and adjusted_text_width is " << adjusted_text_width << endl;
					};
#endif
					tt_screen->get_extent_size(wide_text,
														non_shrinking_or_integer_part_and_fraction_equivalent(wide_text,text_length), 
														// needs wide_text arg as of 131104
//														non_shrinking_equivalent(text_length),
														1, 
														// decimal_places only non-zero if displaying shrinking decimal
														character_width,character_height,
														ignore_longest_line,adjusted_text_width,ignore_new_text_height,
														TRUE,TRUE,FALSE);
					if (adjusted_text_height <= character_height && !edited &&
						 following_operation == NULL && operation == NO_NUMBER_OPERATION && // otherwise these aren't displayed as 2D
						 tt_log_version_number >= 76) {
						// new on 300505 to display fractions better
						if (find_separator_location(wide_text,wide_text_length,L"/") > 0) {						
							character_height /= 2;
						};
					};
#if TT_DEBUG_ON
					if (tt_debug_mode == 110302) {
						tt_error_file() << "With character_width of " << character_width 
											 << " and character_height of " << character_height << " and " << text_length 
											 << " characters computed a new width of " << adjusted_text_width << " and height of " 
											 << ignore_new_text_height 
											 << " on frame " << tt_frame_number << endl;
					};
#endif
					need_to_change_size = TRUE;
//					if (!animation_in_progress()) { // if animating shouldn't be changing size to fit, right?
						set_change_size_to_fit_extent(FALSE);
//					};
//					change_font_size_to_fit_extent_flag = TRUE; // new on 160102 -- restored on 140202 (e.g. float on a blank rational)
				};
			};
#if TT_DEBUG_ON
			add_to_log_if_interesting_error();
#endif
			if (change_font_size_to_fit_extent_flag || 
				 region != NULL ||
				 // commented out on 051104 special handling of shrinking_decimals here
//				 decimal_places > 0 || // && !tt_shrink_and_grow_numbers) || // (!tt_shrink_and_grow_numbers || need_to_change_size)
				 // above updated 110104 so only happens after size change if shrinking decimals (and tt_shrink_and_grow_numbers)
				 animation_in_progress()) { 
				// region check added on 210102 since with remote looks 
				// and the like same thing can be displayed with different dimensions
				// added decimal_places > 0 || animation_in_progress() on 170803 since otherwise doesn't use a good font size
				int separator_index = find_separator_location(wide_text,wide_text_length,L"./");
				if (separator_index == wide_text_length-1) { // if separator is at the very end then ignore this - new on 031104
					separator_index = -1;
				};
				int text_length_of_longest_line; // new on 301004
				boolean multiple_lines = (separator_index > 0 && wide_text[separator_index] == '/' && 
												  following_operation == NULL && operation == NO_NUMBER_OPERATION); 
				// removed tt_shrink_and_grow_numbers && since this code is never run when !tt_shrink_and_grow_numbers 
				// -- following_operations test new on 011204
				boolean multiple_lines_with_integer_part = FALSE;
				int integer_part_length, fraction_part_length;
				// if !tt_shrink_and_grow_numbers then don't display multiple lines
				city_coordinate available_text_height, max_integer_part;
				if (multiple_lines) { // new on 301004 to support multi-line fractions
					int space_index = find_separator_location(wide_text,separator_index,L" ");
					//if (space_index < 0 && operation != NO_NUMBER_OPERATION) {
					//	// new on 011204 to deal with operations like /3/2 better
					//	space_index = 0; // right after operation -- always correct??
					//};
					multiple_lines_with_integer_part = (space_index >= 0);
					if (multiple_lines_with_integer_part) {
						integer_part_length = space_index;
						int numerator_length, denominator_length;
						multiple_lines_with_integer_part_lengths(space_index,separator_index,wide_text_length,adjusted_text_width,
																			  numerator_length,denominator_length,fraction_part_length,
																			  max_integer_part);
						text_length_of_longest_line = space_index+(fraction_part_length+1)/2+1; 
						// /2 new on 131104 since fraction part is half-sized font and +1 for space and +1 to round up
					} else {
						text_length_of_longest_line = max(separator_index,(text_length-(separator_index+1)));
					};
					available_text_height = adjusted_text_height/2; // if multiple lines then only half the height is available
					// and if there is an integer part it'll double this
				} else {
					text_length_of_longest_line = text_length; // updated 151104 non_shrinking_equivalent(text_length);
					available_text_height = adjusted_text_height;
				};
				// commented out on 211104 since doesn't do right thing for integers
//				text_length_of_longest_line 
				// = (int) (non_shrinking_growing_equivalent(text_length_of_longest_line,part1_shrinkage)+0.5); 
				// new on 151104
				city_coordinate correct_character_width, correct_character_height;
				tt_screen->correct_font_size(wide_text,text_length_of_longest_line,1,
													  adjusted_text_width,available_text_height,TRUE,TRUE,FALSE,
													  correct_character_width,correct_character_height);
				//if (correct_character_width == 0) {
				//	// new on 021205 since large numbers operated upon in boxes can have 0 width
				//	correct_character_width = (9*correct_character_height)/20;
				//};
				if (//tt_shrink_and_grow_numbers // not needed as of 141104 && // !animation_in_progress() && 
					// added !animation_in_progress on 051104 
					 (multiple_lines || decimal_places > 0 || 
					  (correct_character_height > correct_character_width*3 && size_constrained())) 
					  && text_length > 2 && !IsCharAlphaW(wide_text[0])) {
					// added size_constrained() on 081105 since if large number on the ground shouldn't do this
				   // on 021205 moved size_constrained() to only apply if the digits are getting too narrow
				   // new on 261004 that only do this if font gets too narrow and it isn't an operation (first character is a letter)
					// added decimal_places disjunct on 051104 since 
				   // otherwise has the shrinkage factor computed without knowing how much room
					// the integer part will take
					adjusted_character_height = correct_character_height;
					adjusted_character_width = (9*adjusted_character_height)/20; 
					// 3/5 (good font aspect ratio) x 3/4 (screen unit aspect ratio)
					//tt_screen->get_extent_size(wide_text,non_shrinking_equivalent(wide_text,text_length),1, 
					//									// decimal_places only non-zero if displaying shrinking decimal
					//									character_width,character_height,
					//									ignore_longest_line,adjusted_text_width,ignore_new_text_height,
					//									TRUE,TRUE,FALSE);
//					adjusted_text_width = adjusted_character_width*text_length;
					// why is there text_length and wide_text_length? -- asked 301004
					if (//(adjusted_character_width*text_length_of_longest_line > adjusted_width) ||
						// updated on 301104 since breaks down for very large numbers (over a million digits)
						 ((int64) adjusted_character_width*text_length_of_longest_line > adjusted_width) ||
						// needed_width is greater than available_width
//						 multiple_lines_with_integer_part || // maybe any multiple_lines fraction? -- experiment on 131104 to comment out
						 decimal_places > 0) { // added 061104
						// won't fit so time to shrink and grow
						// first find out how big a digit really is
						city_coordinate digit_width, digit_height;
						tt_screen->get_extent_size(L"0",1,1, 
															adjusted_character_width,adjusted_character_height,
															ignore_longest_line,digit_width,digit_height,
															TRUE,TRUE,FALSE);
//						adjusted_character_height 
						// = (city_coordinate) (adjusted_character_height *((double) adjusted_character_height/digit_height); 
						// so its extent matches the desired size
						city_coordinate available_width;
						if (decimal_places > 0) { // shrinks away to nothing so need to worry about extra at the end
							available_width = adjusted_text_width;
						} else {
							available_width = adjusted_text_width-2*digit_width;
						};
						double first_half = ((double) available_width)/(2*digit_width);
						if (first_half < 1.5) { // new on 050805 -- modified on 060805 to check text_length to see if this makes sense
							double reduction_needed_to_show_all = ((double) adjusted_text_width)/(text_length*digit_width);
							double reduction_to_make_first_half_at_least_1_5 = first_half*.66;
							double reduction = max(reduction_needed_to_show_all,reduction_to_make_first_half_at_least_1_5); 
							digit_width = (city_coordinate) (digit_width*reduction);
							adjusted_character_width = (city_coordinate) (adjusted_character_width*reduction);
//							adjusted_character_height = 20*adjusted_character_width/9;
							if (reduction_to_make_first_half_at_least_1_5 > reduction_needed_to_show_all) {
								first_half = ((double) available_width)/(2*digit_width);
							};
						};
						// -2*edge_size to give a little space at the ends -- changed to adjusted_text_width which already did that
						// -3*digit_width on 131104 to leave a digit and a half space on both ends
						// tried to leave room for 10 pixels since there seems to be a round off problem 
						// greater for small pads -10*tt_screen->one_pixel_in_x_city_units()
						// experiment on 091104 removing the following:
						//if (first_half < 1.5 && text_length_of_longest_line > 4) { 
						// too little room (only enough for 1.5 full size characters in shrinking part)
						//	double font_shrinkage = first_half/1.5;
						//	adjusted_character_width = (city_coordinate) (adjusted_character_width*font_shrinkage);
						//	adjusted_character_height = (city_coordinate) (adjusted_character_height*font_shrinkage);
						//	first_half = 1.5;
						//};
						if (separator_index <= 0 || (multiple_lines && !multiple_lines_with_integer_part)) {
							// changed from < to <= on 091104 since operator shouldn't count
							// there is no separator or just dealing with numerator or denominator (the bigger one) 
							// without an integer part
							if (multiple_lines) {
								set_part1_shrinkage(compute_shrinkage(first_half,text_length_of_longest_line/4)); 
								// /4 new on 131104 since is half size font
							} else {
								set_part1_shrinkage(compute_shrinkage(first_half,text_length_of_longest_line/2)); // new on 211104
							};
							if (part1_shrinkage <= 0.0 || part1_shrinkage > .99) { // just use narrow text since can't shrink and grow
								// added part1_shrinkage > .99 on 301104 since no point shrinking one part in 100 
								// each time since won't be perceptable
								set_part1_shrinkage(0.0); // simplifies things to canonicalize this
								adjusted_character_width = correct_character_width;
								adjusted_character_height = correct_character_height;
							};
						} else if (decimal_places > 0) {
							double part1_fraction,part2_fraction,fraction_per_character;
							// on 061104 replaced digit_width with adjusted_character_width
							shrinking_decimal_parts(separator_index,adjusted_text_width,adjusted_character_width,fraction_per_character,
															part1_fraction,part2_fraction);
//							part1_shrinkage = 1.0-fraction_per_character/part1_fraction;
//							part1_shrinkage = compute_shrinkage(part1_fraction/fraction_per_character,separator_index/2);
							if (!display_with_size_unconstrained) { // condition new on 061104
								set_part1_shrinkage(compute_shrinkage(part1_fraction*first_half,separator_index/2));
								set_part2_shrinkage(1.0-fraction_per_character/part2_fraction);
							};
						} else {
							int part1_text_length, part2_text_length;
							double fraction_before_separator;
							if (multiple_lines_with_integer_part) {
								part1_text_length = integer_part_length; // *2; // *2 since double size font -- new on 131104 - 
								// removed 131104
								part2_text_length = fraction_part_length/2; 
								fraction_before_separator = (double) max_integer_part/adjusted_text_width;
							} else {
								part1_text_length = separator_index;
								part2_text_length = wide_text_length-(part1_text_length+1);
								fraction_before_separator = (double) part1_text_length/text_length_of_longest_line; 
								// updated 051104 was (wide_text_length-1);
								if (part2_shrinkage <= 0.0) {
									// new on 081104
									part2_shrinkage = compute_shrinkage((1-fraction_before_separator)*first_half,part2_text_length);
								};
								part2_text_length = (city_coordinate) non_shrinking_growing_equivalent(part2_text_length,part2_shrinkage); 
								// prior to 081104 was non_shrinking_equivalent						
								if (fraction_before_separator > .5 && part1_shrinkage > 0.0 && part1_shrinkage < 1.0 && 
									 part2_shrinkage != 0.0) { 
								   // added part2_shrinkage != 0.0 on 021205 since no need to share if second part so tiny
									// only gets half
									fraction_before_separator = .5;
								};
							};					
//							city_coordinate adjusted_text_width_of_part = adjusted_character_width*part_text_length;
//							double adjusted_width_of_part = adjusted_width*fraction_before_separator;
//							double first_half = (part_text_length*adjusted_width_of_part)/(2*adjusted_text_width_of_part);
							double first_half_part1 = first_half*fraction_before_separator;
							// now for other half			
							double fraction_after_separator;
							if (multiple_lines_with_integer_part) {
								fraction_after_separator = 1.0-(fraction_before_separator+2.0/text_length_of_longest_line); 
								// updated 131104 to use 2.0
								first_half_part1 *= 0.5; // since is made with double size characters
							} else {
								fraction_after_separator = 1.0-(fraction_before_separator+1.0/text_length_of_longest_line);
								// updated 051104 was wide_text_length);
							}
							// was (double) part2_text_length/(wide_text_length-1); but above is better way to deal with splitting 
							// (and giving a share to the decimal point)
							//if (multiple_lines) {
							//	fraction_after_separator -= 0.1; // new on 091104 since fractions exceed right edge
							//};
							double first_half_part2 = first_half*fraction_after_separator;
							//if (decimal_places > 0) { // shrinking
							//	first_half_part2 *= 2; // use the whole thing since no growing afterwards
							//};
							// use text_length_of_longest_line if multi-line?? 011104
							// commented out the following correction as experiment on 041204
							//boolean need_to_shrink_fonts_for_part1 = (first_half_part1 < 1.5 && !multiple_lines_with_integer_part);
							//boolean need_to_shrink_fonts_for_part2 = (first_half_part2 < 1.5 && !multiple_lines_with_integer_part);
							//if (need_to_shrink_fonts_for_part1 && 
							//    (!need_to_shrink_fonts_for_part2 || first_half_part1 < first_half_part2)) {
							//	// only one needing correction or needs it more seriously
							//	if (part1_text_length > 1) {
							//		double font_shrinkage = first_half_part1/1.5;
							//		adjusted_character_width = (city_coordinate) (adjusted_character_width*font_shrinkage);
							//		adjusted_character_height = (city_coordinate) (adjusted_character_height*font_shrinkage);
							//		first_half_part1 = 1.5;	
							//		first_half_part2 /= font_shrinkage; // both use the same font
							//	// commented out on 091104 since messes up numbers like 1.2345678901234
							//	//} else {
							//	//	// new on 071104 (condition used to be part of need_to_shrink_fonts_for_part1)
							//	//	adjusted_character_width = correct_character_width;
							//	//	adjusted_character_height = correct_character_height;
							//	};
							//} else if (need_to_shrink_fonts_for_part2) { 
							//	if (part2_text_length > 1) {
							//		double font_shrinkage = first_half_part2/1.5;
							//		adjusted_character_width = (city_coordinate) (adjusted_character_width*font_shrinkage);
							//		adjusted_character_height = (city_coordinate) (adjusted_character_height*font_shrinkage);
							//		first_half_part2 = 1.5;	
							//		first_half_part1 /= font_shrinkage; // both use the same font
							//	//} else {
							//	//	// new on 071104 (condition used to be part of need_to_shrink_fonts_for_part2)
							//	//	adjusted_character_width = correct_character_width;
							//	//	adjusted_character_height = correct_character_height;
							//	};
							//};
							//if (decimal_places > 0) { // shrinking
							//	double distance = first_half_part2*2; // use the whole thing since no growing afterwards
							//	if (distance < 1.5) { // too short so shrink font
							//		double font_shrinkage = distance/1.5;
							//		adjusted_character_width = (city_coordinate) (adjusted_character_width*font_shrinkage);
							//		adjusted_character_height = (city_coordinate) (adjusted_character_height*font_shrinkage);
							//		distance = 1.5;	
							//		first_half_part1 /= font_shrinkage; // both use the same font
							//	};
							//	part2_shrinkage = (distance-1.0)/distance;
							//} else {
							if (decimal_places > 0) { // shrinking
								// handled elsewhere (correctly) if shrinking decimal
								//double distance = first_half_part2*2; 
								//part2_shrinkage = (distance-1.0)/distance;
//								part2_shrinkage = 1.0-(0.25/first_half_part2); // rewritten on 041104 -- first_half_part2 is just half
							} else {
								set_part2_shrinkage(compute_shrinkage(first_half_part2,part2_text_length/2));
							};
							if (!display_with_size_unconstrained) { // condition new on 061104
								set_part1_shrinkage(compute_shrinkage(first_half_part1,part1_text_length/2));
							};
						};
						if (part1_shrinkage <= 0.0 && part2_shrinkage <= 0.0) { 
							// new on 211104 so if can't shrink use best possible font
							adjusted_character_width = correct_character_width;
							adjusted_character_height = correct_character_height;
						};
					} else {
						if (multiple_lines) {
							set_part1_shrinkage(-1.0); // meaning no shrinkage but use display_shrinking_growing_integer -- really??
						} else {
							set_part1_shrinkage(0.0); // meaning no shrinkage
						};
						set_part2_shrinkage(0.0);
//						if (region == NULL) { 
						// condition new on 220405 since if indirection of picture this is wrong -- wasn't the bug 220405
							adjusted_character_width = correct_character_width;
							adjusted_character_height = correct_character_height;
//							adjusted_text_width = adjusted_character_width*text_length;
//						};
					};
				} else {
					//if (multiple_lines) {
					//	adjusted_character_width = correct_character_width/2;
					//	adjusted_character_height = correct_character_height/2;
					//} else {
						adjusted_character_width = correct_character_width;
						adjusted_character_height = correct_character_height;
//					};
					if (decimal_places == 0) { // conditional new on 011104
						set_part1_shrinkage(0.0); // meaning no shrinkage
						set_part2_shrinkage(0.0);
					};
				};
				if (part1_shrinkage == 0.0 && part2_shrinkage == 0.0) { // too little room to shrink
					// so use the best font size possible
					adjusted_character_width = correct_character_width;
					adjusted_character_height = correct_character_height;
				};
//				adjusted_character_height = adjusted_text_height; 
				// don't care what the vertical extent is - leave the height alone - new on 270202
//tt_error_file() << "With width of " << adjusted_text_width << " and height of " << adjusted_text_height 
				//    << " computed character_width of " << adjusted_character_width 
				// << " and character_height of " << adjusted_character_height << endl;
#if TT_DEBUG_ON
            if (debug_counter == tt_debug_target) {
               log("changing font size to fit extent of debug target.");
            };
				if (tt_debug_mode == 110302) {
					tt_error_file() << "After correct_font_size adjusted character width and height " 
										 << adjusted_character_width << "X" << adjusted_character_height
										 << " and adjusted_text_width is " << adjusted_text_width 
										 <<  " on frame " << tt_frame_number << endl;
				};
#endif
//				if (region == NULL) { // experiment on 101202 ) { // removed && region == NULL on 281002 -- 
				// condition removed again on 220405 since otherwise indirections to multi-line fractions look terrible
					// first test was change_font_size_to_fit_extent_flag prior to 081102 
					// but don't remember new font size if am say displaying the remote looks
               // animation check added on 250102 and region check since 
				   // could be "seen" through remote looks and then shouldn't save font sizes
					// no point caching it if displayed within a region
					set_character_width(max(1,adjusted_character_width)); 
					// for very long bignums width can become zero - needs a minimum
					set_character_height(max(1,adjusted_character_height));
					if (!animation_in_progress()) { // conditions new when?
						set_change_font_size_to_fit_extent(FALSE);
					};
//				};
			};
#if TT_DEBUG_ON
			add_to_log_if_interesting_error();
#endif
			if (edge_size > 0) { // need to compute width that would produce adjusted_text_width after subtracting edge size
				if (old_adjusted_text_width != adjusted_text_width) {
//					adjusted_text_width = (adjusted_text_width*edge_size_proportion)/(edge_size_proportion-2);
					adjusted_text_width += 2*edge_size; // new version on 160102
				};
				if (old_adjusted_text_height != adjusted_text_height) { 
					adjusted_text_height += 2*edge_size; // new version on 170102
				};
			};
			if (need_to_change_size && old_adjusted_text_width != adjusted_text_width) {  
				//if (leader == NULL && !held() &&  
				// experiment 121202 -- removing this helps keep numbers that are changing number of digits on the floor stationary
				//	 llx >= tt_screen->min_x() && llx+adjusted_text_width <= tt_screen->max_x() && 
				// new on 260202 - package this up as fully_on_screen
				//	 lly >= tt_screen->min_y() && llx+adjusted_text_height <= tt_screen->max_y()) {
				//	move_by((old_adjusted_text_width-adjusted_text_width)/2,0); 
				// old_adjusted_text_width was width prior to 250202 -- for very long numbers matters
				//};
				city_coordinate urx = llx+width; // new on 251004 -- compute before changing width
				Sprite::set_size(adjusted_text_width,height,NEW_TEXT_EXTENT); // don't want to trigger changes by text::set_size...
				if (tt_log_version_number >= 57 && urx >= 0 && leader == NULL && llx+width < 0) { // new on 251004
					// wasn't off screen before but is now so fix it
					move_to(min(tt_screen->max_x(),urx)/2,lly); // center of screen or center of portion showing
				} else if (tt_log_version_number == 58 && held()) { // improved in version 59
					// new on 091104 since otherwise can end up off screen while held after just adding an operator
					tt_programmer->set_grasp_delta_x(tt_programmer->return_grasp_delta_x()-character_width+
																(adjusted_text_width-old_adjusted_text_width));
				};
				set_clean_status(DIRTY);
				// new on 310104 since can be DIRTY_BUT_OBSCURED and grow (but can't this be handled elsewhere more generally?)
				// not clear why the following works better than set_size
//				width = adjusted_text_width;
				tt_screen->invalidate_cache(); // new on 170102 since dirty region is confused by this size change while displaying
				if (tt_log_version_number < 57) {
					set_change_font_size_to_fit_extent(TRUE); 
					// moved here on 270202 so it happens next time around -- 
					// removed as experiment on 241004 since if changing size why change font size too?
				};
			};
			city_coordinate start_x = adjusted_llx+edge_size;
			if (tt_right_to_left_text) { // new on 081203 for Hebrew
				tt_main_window->set_text_align(TA_LEFT); // restore the default for displaying numbers - 
				// in particular important for shrinking decimals and things like /7 (which otherwise are displayed as 7/)
//				start_x += adjusted_text_width;
			};
			UnwindProtect <boolean> set(tt_right_to_left_text,FALSE); // so numbers aren't affected
#if TT_DEBUG_ON
			add_to_log_if_interesting_error();
#endif
			if (adjusted_character_width >= 3*one_x_pixel) { 
				// conditional new on 060202 -- made it 3 pixels on 130202 -- changed to >= on 110302
				int separator_index = find_separator_location(wide_text,wide_text_length,L"./");
				if (separator_index == wide_text_length-1) { // if separator is at the very end then ignore this - new on 031104
					separator_index = -1;
				};
				if (decimal_places > 0 || 
					 (//tt_shrink_and_grow_numbers && // not needed as of 141104
					  (part1_shrinkage > 0.0 || part2_shrinkage > 0.0 || 
					   (separator_index > 0 && !edited && wide_text[separator_index] == L'/')))) {
					// even if no need to shrink if need multiple lines then do this branch as well (new on 031104)
					// added part2_shrinkage > 0.0 on 091104 to deal with exact decimals with short integer parts
					city_coordinate height_or_region_height; // new with TT_ALPHA
					if (region == NULL) {
						height_or_region_height = height;
					} else {
						height_or_region_height = region->height();
					};
#if TT_DEBUG_ON
					DWORD start_time;
					if (tt_debug_mode == 100804) {
						start_time = timeGetTime();
					};
#endif
					// commented out on 141104 since never called with !tt_shrink_and_grow_numbers
//					if (decimal_places > 0 && !tt_shrink_and_grow_numbers) {
//						display_shrinking_decimal(wide_text,wide_text_length,start_x,adjusted_lly+adjusted_height-edge_size,
//														  adjusted_character_width,adjusted_character_height,permutation,
//					                             adjusted_width-2*edge_size,
//														  height_or_region_height); // arg new with TT_ALPHA
//#if TT_DEBUG_ON
//						if (tt_debug_mode == 100804) {
//							tt_error_file() << "Displaying shrinking digits of number#" << debug_counter 
												// << " took " << (timeGetTime()-start_time) << "ms" << endl;
//						};
//#endif
//					} else {
						display_shrinking_growing_integer(wide_text,wide_text_length,
																	 separator_index,
																	 //wide_text2,wide_text_length2,
																	 start_x,adjusted_lly+edge_size,
																	 adjusted_character_width,adjusted_character_height,
																	 part1_shrinkage,part2_shrinkage,permutation,
																	 //(city_coordinate) total_width1,(city_coordinate) total_width2,
																	 adjusted_text_width,					  
																	 height_or_region_height); // new on 251004
//					};
				} else {
					tt_screen->text_out((string) wide_text,wide_text_length,start_x,adjusted_lly+adjusted_height-edge_size,
											  adjusted_character_width,adjusted_character_height,
											  TRUE,TRUE,TRUE,text_color,TRUE,permutation,WHITE,adjusted_text_width,FALSE,insertion_point);
					// replaced adjusted_width with adjusted_text_width on 031104 
					// since otherwise clipped long integers stick out a tiny bit
#if TT_DEBUG_ON
					add_to_log_if_interesting_error();
#endif
				};
			} else if (adjusted_text_width > tile_width && wide_text_length > 12) { 
				// new on 060202 to show something when text is too much
				// was > 3 prior to 020703
				// changed from ... to first 9 digits followed by ... but much smaller font on 020703 to look more like tiny text
				wide_character wide_text9 = wide_text[9];
				wide_character wide_text10 = wide_text[10];
				wide_character wide_text11 = wide_text[11];
				wide_text[9] = '.';
				wide_text[10] = '.';
				wide_text[11] = '.';
				// changed lly in the following to center better on 080803
				tt_screen->text_out((string)wide_text,12,start_x,adjusted_lly+(3*adjusted_height)/4-edge_size,
										  (96*adjusted_text_width)/(3*72*4),adjusted_character_height/4,TRUE,TRUE,TRUE,text_color,TRUE,
										  permutation,WHITE,adjusted_width); // /3 makes more sense but 96/3*72 looks better
				wide_text[9] = wide_text9;
				wide_text[10] = wide_text10;
				wide_text[11] = wide_text11;
#if TT_DEBUG_ON
				add_to_log_if_interesting_error();
#endif
			};
			if (show_all()) { // new on 040102
				display_plate_borders(adjusted_llx,adjusted_lly,adjusted_width,adjusted_height,plate_state(),permutation); 
			};
//			if (wide_text != NULL) delete [] wide_text; // commented out on 170102 since is no longer a copy
			//if (reclaim_output_text) {
			//	delete [] output_text;
			//};
#if TT_DEBUG_ON
			if (tt_debug_mode == 3) {
				tt_error_file() << (timeGetTime()-start_time) << " ms to display -- INTEGER of length " 
								<< text_length << endl;
			};
#endif
		};
	};
	if (tt_right_to_left_text) { // need to restore the setting
		// experimented by adding TA_RIGHT^ on 270104
		tt_main_window->set_text_align(TA_RTLREADING); 
		// so if a Hebrew or Arabic font is selected then text will be right-to-left - new on 081203
	};
};

void Number::display_shrinking_decimal(wide_string wide_text, int wide_text_length,
													city_coordinate ulx, city_coordinate uly,
													city_coordinate character_width,	city_coordinate character_height,
													bytes color_permutation,
													city_coordinate total_width, 
													// re-used on 080803 to mean area for digits not including edges
													city_coordinate height_or_region_height) {
	// major rewrite on 150804 to be much more accurate 
	// rather than multiply by part2_shrinkage and then round and then find nearest font size
	// this version uses a double to keep track of the "truth"
	if (!tt_shrink_and_grow_numbers) { // new on 141104
		display_shrinking_decimal_old(wide_text,wide_text_length,ulx,uly,character_width,character_height,color_permutation,
			                           total_width,height_or_region_height);
		return;
	};
#if TT_DEBUG_ON
	debug_log(debug_counter,"Displaying shrinking digits of debug target.");
#endif
	if (part2_shrinkage == 0.0) { // shouldn't be so recompute it now -- new on 061104
		compute_shrinking_decimal_places(); // what if unconstrained?
		long new_wide_text_length;
		current_wide_text(wide_text,new_wide_text_length); // new on 091104 to make this up-to-date
		wide_text_length = new_wide_text_length;
	};
	city_coordinate true_height = height_or_region_height; // new on 041003 so remote looks work correctly
//	int full_size_part = digits_before_decimal_point(wide_text,wide_text_length)+2; 
	// decimal point and first digit afterwards are full-size
	int full_size_part = wide_text_length-decimal_places; // no longer +1 to include first digit as of 231004
	if (tt_log_version_number < 57) {
		full_size_part++; // to treat first digit after decimal point specially
	};
	// below was rewritten as above on 231004
	//int full_size_part = wide_text_length;
	//for (int i = 0; i < wide_text_length; i++) {
	//	if (wide_text[i] == '.') {
	//		full_size_part = i+2; // decimal point and first digit afterwards are full-size - was +1 prior to 080803 
	// but then first digit after decimal point not treated special
	//		break;
	//	};
	//};
//	city_coordinate original_lly = uly-character_height; // new on 110803
	city_coordinate original_ulx = ulx;
	city_coordinate original_uly = uly;
	city_coordinate original_character_height = character_height;
	int ignore_longest_line;
	city_coordinate width_of_next, height_of_next; 
	city_coordinate width_of_full_size_part,ignore_height;
	// moved here on 231004 from after text_out but need to know this earlier
	if (tt_log_version_number >= 57) {
		uly = original_uly; // new on 231004
		if (full_size_part <= 0) {
			width_of_full_size_part = 0; // new on 011104
		} else {
			tt_screen->get_extent_size(wide_text,full_size_part,1,character_width,character_height, 
				                        // updated 231004 width_of_next,height_of_next, 
												ignore_longest_line,width_of_full_size_part,ignore_height,
												TRUE,TRUE,FALSE);
			tt_screen->text_out((string) wide_text,full_size_part,ulx,uly,
			// replaced character_width,character_height with width_of_next,height_of_next on 160804 -- restored on 231004
			// also width_of_full_size_part new on 231004 below 
									  character_width,character_height,TRUE,TRUE,TRUE,text_color,TRUE,color_permutation,WHITE,
									  width_of_full_size_part,FALSE);
		};
		width_of_next = character_width;
		height_of_next = character_height;
	} else {
		tt_screen->get_extent_size(wide_text,1,1,character_width,character_height, // new on 160804
											ignore_longest_line,width_of_next,height_of_next,
											TRUE,TRUE,FALSE);
		tt_screen->get_extent_size(wide_text,full_size_part,1,width_of_next,height_of_next,
											ignore_longest_line,width_of_full_size_part,ignore_height,
											TRUE,TRUE,FALSE);
		uly = original_uly-(true_height-height_of_next)/2; // in case character has been shrunk - restored on 120803 -- 
		// prior to 160804 height_of_next was character_height
		tt_screen->text_out((string) wide_text,full_size_part,ulx,uly,
								  width_of_next,height_of_next,TRUE,TRUE,TRUE,text_color,TRUE,color_permutation,WHITE,width_of_next,FALSE);
	};
	//	uly = original_lly+character_height; // in case character has been shrunk -- removed on 120803
//	city_coordinate full_size_part_width = (total_width*full_size_part)/wide_text_length;
	// FALSE added as last arg on 100804 since can't contain tabs
	int remaining = wide_text_length-full_size_part;
	city_coordinate max_ulx = ulx+total_width;
	wide_string text_remaining = wide_text+full_size_part;
//	ulx += width_of_next;
	double ulx_as_double = ulx+width_of_full_size_part; // updated on 160804
	double character_width_as_double;
	double character_height_as_double = height_of_next; // character_height;
	if (tt_log_version_number >= 57) {
		// back to 100%  on 231004
		character_width_as_double = width_of_next; // width_of_next*.8; // character_width; -- 80% seems to make it look better -- 
		// width_of_full_size_part new on 231004 above = character_height; // restored 231004 height_of_next; // character_height;
	} else {
		character_width_as_double = width_of_next*.8;
	};
	city_coordinate region_min_x = tt_screen->viewing_region()->min_x; 
	// so if flying a helicopter the digits in the water are displayed
	if (tt_log_version_number >= 57) { // moved out here on 041104
		uly = (city_coordinate) (uly-character_height*.6);
	};
	for (int i = 0; i < remaining; i++) {
//		character_width = (city_coordinate) character_width_as_double;
		if (tt_log_version_number < 57) {
			uly = (city_coordinate) (original_uly-(true_height-character_height_as_double)/2); 
			// height was original_character_height prior to 120803
		};
//		uly = original_lly; // +new_character_height;
		if (ulx_as_double+character_width_as_double > region_min_x) { // condition new on 100804 -- was > 0 prior to 160804
//			city_coordinate character_height = (city_coordinate) character_height_as_double; -- noticed obsolete on 251004
			if (!tt_screen->text_out((string) text_remaining,1,
				                      (city_coordinate) ulx_as_double,(city_coordinate) (uly+character_height_as_double*.6),
											 (city_coordinate) character_width_as_double,(city_coordinate) character_height_as_double,
											 TRUE,TRUE,TRUE,text_color,TRUE,color_permutation,WHITE,character_width,FALSE)) {
				break; // new on 160804 to stop when too small or off screen
			};
			// FALSE added as last arg on 100804 since can't contain tabs
		//} else {
		//	width_of_next = character_width; // since is fixed width font
		};
//		ulx_as_double += width_of_next;
		if (tt_log_version_number >= 57) {
			ulx_as_double += character_width_as_double*fraction_of_width_between_digits; // was .6 for a while -- restored on 061104
			// fraction_of_width_between_digits here does look nicer but not in text_out as if was prior to 231004
		} else {
			ulx_as_double += character_width_as_double;
		};
		if (ulx_as_double > max_ulx) {
			return; // new on 080803
		};
//		uly -= (character_height-new_character_height)/2; // to keep the bottom fixed
//		character_height = new_character_height;
		character_width_as_double *= part2_shrinkage; // moved here on 041104 so first digit isn't shrunk
		character_height_as_double *= part2_shrinkage;
		text_remaining++;
	};
	// following an attempt to improve the appearance on 080803 but sometimes goes too far -- restored and updated on 160804
	ulx = (city_coordinate) ulx_as_double;
	city_coordinate dot_character_width = minimum_character_width();
	city_coordinate dot_character_height = minimum_character_height();
	city_coordinate start_uly = (city_coordinate) (uly+0.6*dot_character_height);
	while (total_width-(ulx-original_ulx) > 0 && ulx < max_ulx) {
		// +0.6*dot_character_height new on 061104 so this lines up better with the previous stuff
		tt_screen->text_out(".",1,ulx,start_uly,dot_character_width,dot_character_height,TRUE,TRUE,TRUE,text_color,FALSE,
			                 color_permutation,WHITE,dot_character_width,FALSE);
		ulx += dot_character_width;
	};
};

void Number::display_shrinking_growing_integer(wide_string wide_text, int wide_text_length,
															  //wide_character separator,
															  int separator_index,
															  //wide_string wide_text2, int wide_text_length2,
															  city_coordinate start_x, city_coordinate start_y, // was ulx and uly prior to 011104
															  city_coordinate adjusted_character_width, city_coordinate adjusted_character_height,
															  double part1_shrinkage, double part2_shrinkage,
															  bytes color_permutation,
//															  city_coordinate total_width1,
//															  city_coordinate total_width2,
															  city_coordinate total_width,
															  city_coordinate height_or_region_height) { 
	if (tt_screen->horizontal_pixel_length(total_width) < 5) {
		return; // new on 040705 -- less than 5 pixels wide so don't bother -- can even get a negative width when so narrow
		// e.g. when vacuuming up a shrinking digit
	};
	city_coordinate digit_width, digit_height;
	int ignore_longest_line;
	tt_screen->get_extent_size(L"0",1,1, 
										character_width,character_height,
										ignore_longest_line,digit_width,digit_height,
										TRUE,TRUE,FALSE);
	double digit_width_to_character_width = (double) digit_width/character_width;
	double digit_height_to_character_height = (double) digit_height/character_height;
	// following moved here on 051104 from caller
	wide_string wide_text1 = wide_text; // really just the first part
	wide_string wide_text2;
	int wide_text_length1, wide_text_length2;
	wide_character separator;
//	city_coordinate total_width = adjusted_text_width; // adjusted_width-2*edge_size;
	double total_width1, total_width2;
	int space_index = -1; // unless overridden below
//	city_coordinate start_y = adjusted_lly+edge_size; // adjusted_character_height-edge_size;
	if (separator_index <= 0) { // changed for < on 091104 since if operator then ignore it here
		wide_text2 = NULL;
		wide_text_length1 = wide_text_length;
		wide_text_length2 = 0;
		separator = NULL;
		total_width1 = total_width;
		total_width2 = 0;
		start_y += (height_or_region_height-adjusted_character_height)/4; // vertically center text
	} else {
		space_index = find_separator_location(wide_text1,separator_index,L" ");
		//if (space_index < 0 && operation != NO_NUMBER_OPERATION && following_operation == NULL) {
		//	// new on 0112043
		//	space_index = 0;
		//};
		wide_text2 = wide_text+separator_index+1;
		wide_text_length1 = separator_index;
		wide_text_length2 = wide_text_length-(wide_text_length1+1); // 1 for the separator
		separator = wide_text[separator_index];
		if (decimal_places > 0) { 
			double part1_fraction,part2_fraction,fraction_per_character;
			// following as of 051104 uses digit_width rather than adjusted_character_width -- switched back on 061104
			shrinking_decimal_parts(separator_index,total_width,adjusted_character_width,fraction_per_character,
				                     part1_fraction,part2_fraction);
			total_width1 = total_width*part1_fraction+.5;
			total_width2 = total_width*part2_fraction+.5;
//			if (!display_with_size_unconstrained) { // condition new on 061104 -- removed as experiment on 141104
				set_part2_shrinkage(1.0-fraction_per_character/part2_fraction); // since font may have changed since last computed
//			};
		} else if (space_index < 0 || following_operation != NULL || operation != NO_NUMBER_OPERATION) { 
			// otherwise this is just wasted work -- 
			// added second and third disjunct on 011204 since not displayed multi-lined if an operation 
			double part1_fraction;
			double part1_equivalent_width = non_shrinking_growing_equivalent(wide_text_length1,part1_shrinkage);
			double part2_equivalent_width;
			double total_width_without_separator;
			if (separator == '/') { // separator doesn't take up space when multi-line fraction
				total_width_without_separator = (double) total_width;
				part2_equivalent_width = non_shrinking_growing_equivalent(wide_text_length2,part1_shrinkage); // use same shrinkage
			} else {
				total_width_without_separator = (double) total_width-adjusted_character_width;
				part2_equivalent_width = non_shrinking_growing_equivalent(wide_text_length2,part2_shrinkage);
			};
			double both_parts_equivalent_width = part1_equivalent_width+part2_equivalent_width;
			part1_fraction = part1_equivalent_width/both_parts_equivalent_width;
			double part2_fraction = 1.0-part1_fraction;
			total_width1 = total_width_without_separator*part1_fraction;
			total_width2 = total_width_without_separator*part2_fraction;
		};
		if (separator != '/') { // not multiple lines
			start_y += (height_or_region_height-adjusted_character_height)/4; // vertically center text
		};
	};
	boolean multiple_lines = (separator == '/' && following_operation == NULL && operation == NO_NUMBER_OPERATION); 
	// added following_operations test above on 011204 since sequence of fractions looks bad as well as /3/2
	double start_x_as_double = (double) start_x;
	double character_width_as_double = (double) character_width;
	double character_height_as_double = (double) character_height;
	if (multiple_lines) { // new on 301004 to convert to vertical format for fractions
//		separator = NULL; // there is none if multiple lines
//		city_coordinate total_width = (city_coordinate) max(total_width1,total_width2);
		double start_x_as_double = (double) start_x;
		city_coordinate full_size_width,full_size_height; // if no shrinking or growing -- using get_extent
		if (space_index > 0) {
			// need to display the integer part of this at full size
			int numerator_length, denominator_length, max_fraction_length;
			city_coordinate max_integer_part;
			multiple_lines_with_integer_part_lengths(space_index,separator_index,wide_text_length,total_width,
																  numerator_length,denominator_length,max_fraction_length,max_integer_part);
			// split proportionately but give at least 1/4 and most 3/4 to the integer part
//			double integer_part_width = 2*character_width*space_index*0.6; // digit_width_to_character_width;
			city_coordinate full_size_character_width = 2*character_width;
			city_coordinate full_size_character_height = 2*character_height;
			tt_screen->get_extent_size(wide_text1,space_index,1, 
												full_size_character_width,full_size_character_height,
												ignore_longest_line,full_size_width,full_size_height,
												TRUE,TRUE,FALSE);
			city_coordinate extra_after_integer_part; // new on 131104
			if (full_size_width <= max_integer_part) {
				full_size_character_width = (city_coordinate) (((double) max_integer_part/full_size_width)*full_size_character_width);
				tt_screen->text_out((string) wide_text1,space_index, 
										  (city_coordinate) start_x_as_double,
										  (city_coordinate) (start_y+digit_height_to_character_height*full_size_character_height), 
										  // was .6*
										  full_size_character_width,full_size_character_height,
										  TRUE,TRUE,TRUE,text_color,TRUE,color_permutation,WHITE,full_size_width,FALSE);
				start_x_as_double += max_integer_part; // full_size_width;
				total_width -= max_integer_part; // full_size_width;
				extra_after_integer_part = max_integer_part-full_size_width;
			} else {
//				double shrinkage = (double) max_integer_part/full_size_width;
//				double shrunken_character_width_as_double = shrinkage*full_size_character_width;
//				city_coordinate shrunken_character_width = (city_coordinate) shrunken_character_width_as_double;
				double old_start_x_as_double = start_x_as_double;
				// was multiplying by digit_width_to_character_width rather than 0.6 prior to 091104 -- 
				// tried 2*digit_width but that was bad
//				double shrinkage = compute_shrinkage(max_integer_part/(0.6*full_size_character_height),space_index/2);
				display_shrinking_growing_integer_part(wide_text1,space_index,
																   start_x,
																	(city_coordinate) (start_y+full_size_character_height*0.2),
																	full_size_character_width,full_size_character_height,
																	color_permutation,max_integer_part,full_size_character_height,
																	part1_shrinkage, // was just shrinkage prior to 131104
																	digit_width_to_character_width,digit_height_to_character_height,
																	start_x_as_double,character_width_as_double,character_height_as_double);
				//tt_screen->text_out((string) wide_text1,space_index,
				//						  (city_coordinate) start_x_as_double,
				//						  (city_coordinate) (start_y+shrunken_character_height*digit_height_to_character_height), 
										  // digit_height_to_character_height*(character_height+shrunken_character_height)/2), 
				//						  shrunken_character_width,shrunken_character_height,
				//						  TRUE,TRUE,TRUE,text_color,TRUE,color_permutation,WHITE,max_integer_part,FALSE);
//				start_x_as_double = start_x+max_integer_part; 
				// is updated by display_shrinking_growing_integer_part above but this looks better
//				total_width -= max_integer_part;
				city_coordinate delta_x = (city_coordinate) (start_x_as_double-old_start_x_as_double);
				total_width -= delta_x;
				extra_after_integer_part = max_integer_part-delta_x;
				//total_width1 -= delta_x;
				//total_width2 -= delta_x;
			};
			//start_x = (city_coordinate) start_x_as_double;
			//wide_text1 += space_index+1; // skip integer part and space
			//wide_text_length1 -= space_index+1;
			int skip = space_index+1;
			city_coordinate spacing = (city_coordinate) (character_width*0.6);
			if (extra_after_integer_part < 0) { // integer part went too far
				spacing = max(0,spacing+extra_after_integer_part);
			} else { // already left some space
				spacing = max(0,spacing-extra_after_integer_part);
			};
			start_x_as_double += spacing; // for space between integer part and fraction -- new on 131104 -- half size font is OK here
			total_width -= spacing; // new on 131104
			display_shrinking_growing_integer(wide_text+skip,wide_text_length-skip,
														 separator_index-skip,
														 (city_coordinate) start_x_as_double,start_y,
														 adjusted_character_width,adjusted_character_height,
														 part2_shrinkage,0.0,
														 color_permutation,
														 total_width,height_or_region_height);
			return;
		};
		city_coordinate center_x_offset1, center_x_offset2;
		if (total_width1 > total_width2) { // moved here 051104
			double growth = total_width/total_width1;
			center_x_offset1 = 0;
			center_x_offset2 = (city_coordinate) ((total_width1-total_width2)*0.5*growth);
			total_width2 *= growth; // grow proportionately
			total_width1 = total_width;
//			set_character_width((city_coordinate) (character_width*growth)); // new on 081104
		} else {
			double growth = total_width/total_width2;
			center_x_offset2 = 0;
			center_x_offset1 = (city_coordinate) ((total_width2-total_width1)*0.5*growth);
			total_width1 *= growth;
			total_width2 = total_width;
//			set_character_width((city_coordinate) (character_width*growth)); // new on 081104
		};
		city_coordinate true_height = height_or_region_height/2; // maybe need extra for the line itself
		start_y += character_height; 
		//(city_coordinate) (character_height*digit_height_to_character_height); // since start_y was computed assuming just one line
//		original_start_y = start_y;
		tt_screen->get_extent_size(wide_text1,wide_text_length1,1, 
											character_width,character_height,
											ignore_longest_line,full_size_width,full_size_height,
											TRUE,TRUE,FALSE);
		if (full_size_width <= total_width || !size_constrained()) { // size_constrained new on 081105
			// || part1_shrinkage <= 0.0) { // negative shrinkage means no shrinkage
			tt_screen->text_out((string) wide_text1,wide_text_length1,
                             //following updated on 081105
									  start_x+center_x_offset1, // max(0,(total_width-full_size_width)/2), // center this line
									  (city_coordinate) (start_y+0.6*character_height), 
									  // 0.6 was digit_height_to_character_height prior to 051104
									  character_width,character_height,
									  // on 081105 replaced full_size_width with total_width1
									  // so multi-line fractions line up when "cropped"
									  TRUE,TRUE,TRUE,text_color,TRUE,color_permutation,WHITE,(city_coordinate) total_width1,FALSE); 
			// full_size_width was (city_coordinate) total_width1 prior to 081104
		} else {
			// made the following "local" on 051104 since it should not change start_x_as_double for others
			double local_start_x_as_double = start_x_as_double+center_x_offset1; 
			// max(0,(total_width-total_width1)/2-character_width); // center this line but one character
			display_shrinking_growing_integer_part(wide_text1,wide_text_length1, 
																(city_coordinate) local_start_x_as_double,
																start_y,
																character_width,character_height,
																color_permutation,(city_coordinate) total_width1,true_height,
																part1_shrinkage,digit_width_to_character_width,digit_height_to_character_height,
																local_start_x_as_double,character_width_as_double,character_height_as_double);
		};
		//// following if revived should have text_color as arg
		//city_coordinate line_y = original_start_y+character_height/4;
		//city_coordinate total_width = max(total_width1,total_width2);
		//city_coordinate margin = total_width/40;
		//tt_screen->draw_line(original_start_x+margin,line_y,original_start_x+total_width-2*margin,line_y);
		city_coordinate dash_width, dash_height;
		tt_screen->get_extent_size(L"_",1,1, 
											character_width,character_height,
											ignore_longest_line,dash_width,dash_height,
											FALSE,TRUE,FALSE); // not bold
		city_coordinate margins = total_width/40;
		int dash_count = max(1,(total_width-margins)/dash_width);
		//if (following_operation != NULL) { 
		// new on 011204 since fractions in a sequence of operations looked very bad -- so did this
		//	int operation_boundary_index = find_separator_location(wide_text,wide_text_length,L";");
		//	if (operation_boundary_index > 0) { 
		//		dash_count = operation_boundary_index;
		//	};
		//};
		string dashes = new char[dash_count];
		for (int i = 0; i < dash_count; i++) {
			dashes[i] = '_';
		};
//		start_y -= (city_coordinate) (character_height*0.3); // new on 051104 commented out on 081104
		tt_screen->text_out(dashes,dash_count,
								  (city_coordinate) start_x_as_double, 
								  // on 081105 commented out +max(0,(total_width-full_size_width)/2),
								  (city_coordinate) (start_y+0.6*character_height), 
								  // was digit_height_to_character_height* prior to 081104
								  character_width,character_height,
								  FALSE,TRUE,TRUE,text_color,FALSE,color_permutation,WHITE,total_width,FALSE); // not bold
		start_y -= character_height; // prior to 081104 was (city_coordinate) (character_height*0.7); // updated on 051104
//		start_y -= (3*character_height)/4; // not sure why but 3/4 looks better -- maybe the true extent
//		if (wide_text2[wide_text_length2-1] == L'/') 
		// could try to support multiple line fractions followed by operator but lots of work (noted 031104)
		tt_screen->get_extent_size(wide_text2,wide_text_length2,1, 
											character_width,character_height,
											ignore_longest_line,full_size_width,full_size_height,
											TRUE,TRUE,FALSE);
		// on 081105 changed the following to <= from <
		if (full_size_width <= total_width || !size_constrained()) { // size_constrained new on 081105
			//  || part1_shrinkage <= 0.0) { // both use the same shrinkage factor
			tt_screen->text_out((string) wide_text2,wide_text_length2,
									  (city_coordinate) (start_x_as_double+center_x_offset2), 
									  // updated on 081105 center_x_offset2 was max(0,(total_width-full_size_width)/2))
									  (city_coordinate) (start_y+digit_height_to_character_height*character_height), 
									  character_width,character_height,
									  TRUE,TRUE,TRUE,text_color,TRUE,color_permutation,WHITE,(city_coordinate) total_width2,FALSE); 
			// shouldn't this pass full_size_width -- asked 061104
		} else {
//			city_coordinate start_x_for_second_line;
//			if (total_width2 < total_width1) { // center this line
//				start_x_for_second_line = (city_coordinate) (start_x+max(0,(total_width-total_width2)/2)); 
//			} else {
//				start_x_for_second_line = start_x; 
//			};
			display_shrinking_growing_integer_part(wide_text2,wide_text_length2, 
																(city_coordinate) (start_x+center_x_offset2), 
																// max(0,(total_width-total_width2)/2)),
																(city_coordinate) (start_y+0.25*character_height), 
																character_width,character_height,
																color_permutation,(city_coordinate) total_width2,true_height,
																part1_shrinkage, // re-use the same shrinkage factor
																digit_width_to_character_width,digit_height_to_character_height,
																start_x_as_double,character_width_as_double,character_height_as_double);
		};
		return;
	};
	start_y = (city_coordinate) (start_y+height_or_region_height/5); // looks good but not sure why -- moved here on 031104
	display_shrinking_growing_integer_part(wide_text1,wide_text_length1, 
														start_x,start_y, 
														// -(3*character_height)/4, // not sure why but 3/4 looks better -- maybe the true extent
														character_width,character_height,
														color_permutation,(city_coordinate) total_width1,height_or_region_height, // true_height,
														part1_shrinkage,digit_width_to_character_width,digit_height_to_character_height,
														start_x_as_double,character_width_as_double,character_height_as_double);
#if TT_DEBUG_ON
	if (start_x_as_double-start_x > total_width1) {
		double excess = (start_x_as_double-start_x)-total_width1;
		tt_error_file() << "Part 1 of a number took " << excess << " units more space than planned." << endl;
	};
#endif
	if (separator != NULL) { // do the rest
		set_character_width((city_coordinate) character_width_as_double); 
		// new on 061104 since looks bad if integer part has different font size
		set_character_height((city_coordinate) character_height_as_double); // probably a no-op
		wide_character separator_string[2];
		separator_string[0] = separator;
		separator_string[1] = NULL;
		//city_coordinate separator_width, separator_height;
		//city_coordinate separator_character_height = (city_coordinate) character_height_as_double; 
		//tt_screen->get_extent_size(separator_string,1,1, 
		//								   (city_coordinate) character_width_as_double,separator_character_height,
		//								   ignore_longest_line,separator_width,separator_height,
		//								   TRUE,TRUE,FALSE);
		// split the difference in width by backing up half of it
		if (wide_text_length1 > 0) { // actually showed something earlier
			// if changed then update shrinking_decimal_parts
			start_x_as_double -= character_width*(1.0-digit_width_to_character_width);
//			start_x_as_double -= separator_width*(1.0-digit_width_to_character_width); 
			// experiment 110104 (character_width_as_double-separator_width); // on each side of the separator
		};
		tt_screen->text_out((string) separator_string,1,
								  (city_coordinate) start_x_as_double,
								  (city_coordinate) (start_y+(.6*character_height)), 
								  //separator_character_height)), // (start_y+separator_character_height-character_height_as_double/4), 
								  character_width,character_height, 
								  // separator_width,separator_character_height, // current size (full or nearly)
								  TRUE,TRUE,TRUE,text_color,TRUE,color_permutation,WHITE,character_width,FALSE);
		start_x_as_double += character_width/2; 
		// separator_width*.5; // if changed then update shrinking_decimal_parts // digit_width_to_character_width;
		if (decimal_places > 0) { // shrinking decimal at the end
			display_shrinking_decimal(wide_text2,wide_text_length2,
											  (city_coordinate) start_x_as_double,
											  (city_coordinate) (start_y+.6*character_height_as_double),
											  character_width,character_height, 
											  // using the original font size even if integer part changed it -- 051104
//											  (city_coordinate) character_width_as_double,(city_coordinate) character_height_as_double,
											  color_permutation,(total_width-(city_coordinate) (start_x_as_double-start_x)),
											  height_or_region_height);
		} else {
			display_shrinking_growing_integer_part(wide_text2,wide_text_length2,
																(city_coordinate) start_x_as_double,start_y,
																(city_coordinate) character_width_as_double,
																(city_coordinate) character_height_as_double,
																color_permutation,
																(city_coordinate) total_width2, 
																// (city_coordinate) (total_width2-separator_width*digit_width_to_character_width),
																height_or_region_height,
																part2_shrinkage,digit_width_to_character_width,digit_height_to_character_height,
																start_x_as_double,character_width_as_double,character_height_as_double);
		};
	};
};

void Number::display_shrinking_growing_integer_part(wide_string wide_text, int wide_text_length,
															   	 city_coordinate start_x, city_coordinate start_y,
																	 city_coordinate character_width, city_coordinate character_height,
																	 bytes color_permutation,
																	 city_coordinate total_width,
																    city_coordinate height_or_region_height,
															       double shrinkage,
																	 double digit_width_to_character_width,
																	 double digit_height_to_character_height,
																	 // outputs:
																	 double &start_x_as_double, 
																	 double &character_width_as_double, double &character_height_as_double) { 
	// new on 251004
	if (wide_text_length == 0) return;
#if TT_DEBUG_ON
	if (tt_debug_mode == 311004) {
		tt_error_file() << endl << "digit_width_to_character_width = " << digit_width_to_character_width << endl;
	};
#endif
	start_x_as_double = start_x;
	double max_start_x = start_x+total_width;
	city_coordinate dot_character_width = minimum_character_width();
	city_coordinate dot_character_height = minimum_character_height();
	double dot_true_width = dot_character_width*digit_width_to_character_width; // could check that this is really the ratio for . as well
	if (shrinkage <= 0.0) { // no shrinkage
		city_coordinate new_font_width, new_font_height;
		int ignore_longest_line;
		city_coordinate width_needed,height_needed;
		tt_screen->get_extent_size(wide_text,wide_text_length,1,
										   character_width,character_height,
										   ignore_longest_line,width_needed,height_needed,
										   TRUE,TRUE,FALSE);
		if (width_needed <= total_width) {
			new_font_width = character_width;
			new_font_height = character_height;
		} else {
			tt_screen->correct_font_size(wide_text,wide_text_length,1,
												  total_width,character_height,
//												  total_width1+total_width2-(current_start_x-start_x),(city_coordinate) character_height_as_double, 
													// replaced original_start_x with start_x on 011104
												  TRUE,TRUE,FALSE,
												  new_font_width,new_font_height);
		};
		if (new_font_width < dot_character_width*2 || new_font_height < dot_character_height*2) {
			// less than twice as big as a small dot so just draw some small dots
			start_y += (city_coordinate) (0.3*character_height); // move up even though just showing dots
			text_out_dots(".",wide_text_length, 
							  start_x_as_double,start_y,max_start_x, 
							  dot_character_width,dot_character_height,color_permutation,
							  dot_character_width);
		   character_width_as_double = character_width; // what to do with this?
			character_height_as_double = character_height;
			return;
		};
		tt_screen->get_extent_size(wide_text,wide_text_length,1,
											new_font_width,character_height,
											ignore_longest_line,width_needed,height_needed,
											TRUE,TRUE,FALSE);
		tt_screen->text_out((string) wide_text,wide_text_length,
								  start_x,
								  (city_coordinate) (start_y+0.6*character_height),
								  new_font_width,character_height,
								  TRUE,TRUE,TRUE,text_color,TRUE,color_permutation,WHITE,width_needed,FALSE); // was total_width prior to 061104
		start_x_as_double += width_needed; // new_font_width*wide_text_length*digit_width_to_character_width; 
		// do I need to multiply by digit_width_to_character_width -- 011104
		character_width_as_double = new_font_width; 
		character_height_as_double = character_height;
		return;
	};
//	start_x -= (character_width-digit_width)/2; // split the difference with the other end of the number
	city_coordinate true_height = height_or_region_height; // new on 041003 so remote looks work correctly
	city_coordinate original_start_x = start_x;
	city_coordinate original_start_y = start_y;
	city_coordinate original_character_height = character_height;
	int first_half = (wide_text_length+1)/2; // first half shrinks including extra one if odd
	wide_string text_remaining = wide_text;
	character_width_as_double = character_width; 
	character_height_as_double = character_height;
	city_coordinate region_min_x = tt_screen->viewing_region()->min_x; 
	// can't assume region_min_x is 0 since if flying a helicopter the digits in the water are then not displayed
	int dot_count = 0;
	if (character_width < dot_character_width*2 || character_height < dot_character_height*2) {
		// less than twice as big as a small dot so just draw some small dots
		dot_count = text_out_dots(".",first_half, 
										  start_x_as_double,(city_coordinate) (start_y+digit_height_to_character_height*dot_character_height),max_start_x, 
										  dot_character_width,dot_character_height,color_permutation,
										  dot_true_width);
		return;
	};
	int shrinking_digits_displayed = first_half; // unless break out of the following because it is too small
	if (first_half != dot_count) {
		city_coordinate minimum_font_width_in_city_coordinates = minimum_font_width*tt_screen->one_pixel_in_x_city_units();
//		start_y = (city_coordinate) (original_start_y+character_height_as_double*.5*digit_height_to_character_height); 
		// (character_height-character_height_as_double)/2); // height_or_region_height was true_height prior to 011104
		for (int i = 0; i < first_half-dot_count; i++) {
			if (start_x_as_double+character_width_as_double > region_min_x) {
#if TT_DEBUG_ON
				if (tt_debug_mode == 311004) {
					tt_error_file() << "About to display " << (char) text_remaining[0] 
										 << " with size " << character_width_as_double << "x" << character_height_as_double 
										 << " at " << start_x_as_double << "," << start_y << endl;
				};
#endif
				boolean displayed_ok = tt_screen->text_out((string) text_remaining,1,
																		 (city_coordinate) start_x_as_double,
																		 (city_coordinate) (start_y+character_height_as_double*.6),
																		 (city_coordinate) character_width_as_double,(city_coordinate) character_height_as_double,
																		 TRUE,TRUE,TRUE,text_color,TRUE,color_permutation,WHITE,
																		 (city_coordinate) character_width_as_double,FALSE);
				if (!displayed_ok) {
					shrinking_digits_displayed = i;
					//if (i != 0) { // restore to previous values so growing is symmetric
					//	character_width_as_double /= shrinkage;
					//	character_height_as_double /= shrinkage;
					//};
					break; // stop when too small or off screen
				};
			} else if (character_width_as_double < minimum_font_width_in_city_coordinates) {
				// new on 301104 to display partly off screen
				shrinking_digits_displayed = i;
				break;
			};
			// following moved out here on 291004 since if part of the number is off the left still need to compute the size
			//tt_screen->get_extent_size(text_remaining,1,1, 
			//									(city_coordinate) character_width_as_double,(city_coordinate) character_height_as_double,
			//									ignore_longest_line,digit_width,digit_height,
			//									TRUE,TRUE,FALSE);
			start_x_as_double += character_width_as_double*digit_width_to_character_width; 
			// tried digit_width above computed above on 301004 but looked worse since it doesn't shrink as character_width_as_double does
			if (!(i+1 == first_half && (wide_text_length&1))) { // don't shrink if doing the middle one of an odd text length
				character_width_as_double *= shrinkage;
				character_height_as_double *= shrinkage;
			};
			if (start_x_as_double > max_start_x) {
				return;
			};
			text_remaining++;
		};
	} else {
		shrinking_digits_displayed = 0; // just dots
	};
	double shrinking_half = total_width/2.0;
	boolean display_all_dots_needed = TRUE; // unless overridden below
	while (shrinking_digits_displayed+dot_count < first_half) { 
		// didn't display them all and still left of the center -- but display at least 3 dots if there are some remaining digits that are too small
		tt_screen->text_out(".",1,(city_coordinate) start_x_as_double,(city_coordinate) (start_y+digit_height_to_character_height*dot_character_height),
								  dot_character_width,dot_character_height,TRUE,TRUE,TRUE,text_color,FALSE,color_permutation,WHITE,dot_character_width,FALSE);
		start_x_as_double += dot_true_width;
		dot_count++;
//		if (shrinking_half+dot_true_width < start_x_as_double-original_start_x) { // at mid point so no point displaying more dots
		if (start_x_as_double-original_start_x > shrinking_half-dot_true_width) { // updated on 131104 to be clearer and more accurate 
			// -dot_true_width rather than +dot_true_width
			display_all_dots_needed = FALSE;
			break;
		};
	};
	double distance_after_shrinking_digits = start_x_as_double-original_start_x;
#if TT_DEBUG_ON
	if (tt_debug_mode == 311004) {
		tt_error_file() << "About to grow." << endl;
		tt_error_file() << "shrinking_half = " << shrinking_half << endl;
		tt_error_file() << "distance_after_shrinking_digits = " << distance_after_shrinking_digits << endl;
		tt_error_file() << "start_x_as_double = " << start_x_as_double << endl;
		tt_error_file() << "dot_count = " << dot_count << endl;
	};
#endif
	// now show the same number of dots on the growing side unless already too far over
	double max_start_x_for_dots = original_start_x+shrinking_half+dot_count*dot_character_width;
	int first_half_dot_count = dot_count;
	for (int i = 0; i < first_half_dot_count; i++) {
		if (start_x_as_double >= max_start_x_for_dots) {
//			shrinking_digits_displayed = wide_text_length; // so nothing more is printed
			break;
		};
		tt_screen->text_out(".",1,(city_coordinate) start_x_as_double,(city_coordinate) (start_y+digit_height_to_character_height*dot_character_height),
								  dot_character_width,dot_character_height,TRUE,TRUE,TRUE,text_color,FALSE,color_permutation,WHITE,dot_character_width,FALSE);
		start_x_as_double += dot_true_width;
		dot_count++;
	};
	//if (start_x_as_double < original_start_x+shrinking_half) { // jump to very center if left of it
	//	start_x_as_double = original_start_x+shrinking_half;
	//};
	// now to grow
	int first_growing_digit = wide_text_length-shrinking_digits_displayed; 
	// shrinking_digits_displayed+dot_count; // first_half+first_half_dot_count; // shrinking_digits_displayed+dot_count; // 
	if (wide_text_length&1 && dot_count == 0) {
		first_growing_digit++; // shrinking phase already showed the extra one
	};
	//	if (wide_text_length&1 && dot_count == 0) { // odd
////		first_growing_digit--; // so nothing is lost
//		// but this should shrink a bit since has one extra digit compared to the shrinking phase so
//		double correction_shrinkage = first_half/(first_half+1.0);
//		character_width_as_double = character_width_as_double*correction_shrinkage;
//		character_height_as_double = character_height_as_double*correction_shrinkage;
//	};
	text_remaining = wide_text+first_growing_digit;
	//int second_half = text_length-first_half;
	//for (int i = first_half-shrinking_digits_displayed; i < second_half; i++) {
	for (int i = first_growing_digit; i < wide_text_length; i++) {
		//if (start_x_as_double > max_start_x) { // corrected below
		//	return; // shouldn't really happen
		//};
//		if (i > first_growing_digit || (wide_text_length&1) || dot_count > 0) { // if even length don't resize first one
			character_width_as_double /= shrinkage;
			character_height_as_double /= shrinkage;
//		};
//		start_y = (city_coordinate) (original_start_y+(character_height-character_height_as_double)/2); 
		// height_or_region_height was true_height prior to 011104
		if (start_x_as_double+character_width_as_double > region_min_x) {
#if TT_DEBUG_ON
			if (tt_debug_mode == 311004) {
				tt_error_file() << "About to display " << (char) text_remaining[0] 
									 << " with size " << character_width_as_double << "x" << character_height_as_double 
									 << " at " << start_x_as_double << "," << start_y << endl;
			};
#endif
			tt_screen->text_out((string) text_remaining,1,(city_coordinate) start_x_as_double,(city_coordinate) (start_y+character_height_as_double*0.6),
									  (city_coordinate) character_width_as_double,(city_coordinate) character_height_as_double,
									  TRUE,TRUE,TRUE,text_color,TRUE,color_permutation,WHITE,(city_coordinate) character_width_as_double,FALSE);
		};
		//tt_screen->get_extent_size(text_remaining,1,1, 
		//									(city_coordinate) character_width_as_double,(city_coordinate) character_height_as_double,
		//									ignore_longest_line,digit_width,digit_height,
		//									TRUE,TRUE,FALSE);
		start_x_as_double += character_width_as_double*digit_width_to_character_width; // digit_width tried on 301004 but costs more and looked worse
		text_remaining++;
	};
};

int Number::text_out_dots(string dot, int dots_remaining, double &start_x_as_double, city_coordinate start_y, double max_start_x,
								  city_coordinate dot_character_width, city_coordinate dot_character_height, bytes color_permutation,
								  double dot_true_width) { 
	int dot_count = 0;
	while (dots_remaining > 0) { 
		// didn't display them all and still left of the center
		tt_screen->text_out(dot,1,(city_coordinate) start_x_as_double,start_y,
								  dot_character_width,dot_character_height,TRUE,TRUE,TRUE,text_color,FALSE,color_permutation,WHITE,dot_character_width,FALSE);
		start_x_as_double += dot_true_width;
		if (start_x_as_double > max_start_x) {
			// pretend all the dots were shown even though there wasn't enough room
			dot_count += dots_remaining;
			break;
		};
		dots_remaining--;
		dot_count++;
	};
	return(dot_count);
};

#endif

#include "c:/tt/number_old_display.cpp"

//	double shrinking_end_as_fraction_of_middle = (wide_text_length1*distance_after_shrinking_digits)/(total_width1*first_half);
//	// above takes into account odd text_length
//	double fraction_of_total_width_used = (ulx_as_double-original_ulx)/total_width1;
//	double fraction;		
//	if (fraction_of_total_width_used > 1.0 || fraction_of_total_width_used < .9) {
//		// this is the more serious problem so adjust for that
//		fraction = fraction_of_total_width_used;
//	} else if (shrinking_end_as_fraction_of_middle > 1.0) {
//		fraction = 1.0; // don't adjust since they disagree about how to fix things
//	} else {
//		//double middle_error = 1.0-shrinking_end_as_fraction_of_middle);
//		//double full_length_error = 1.0-fraction_of_total_width_used;
//		//if (middle_error > full_length_error) {
//		fraction = max(shrinking_end_as_fraction_of_middle,fraction_of_total_width_used);
//		//} else {
////				fraction = fraction_of_total_width_used;
////			};
//	};
	//if (fraction < .98 || fraction > 1.02) {
	//	// at least 2% short or too long so try to fix it for next time
	//	shrinkage = 1.0/(((1.0/shrinkage-1.0)*fraction)+1.0); // algebra
	//};

//int Number::digits_before_decimal_point(wide_string wide_text, int wide_text_length) { // abstracted on 231004 and made obsolete hours later
//	for (int i = 0; i < wide_text_length; i++) {
//		if (wide_text[i] == '.') {
//			return(i);
//			break;
//		};
//	};
//	return(0); // warn??
//};

//const double number_shrinkage = 0.95;
//double number_shrinkage_log = 0.0; 
// a "cache" - eliminated on 080803 since isn't global anymore - each number could cache it if this was worthwhile

//void Number::display_shrinking_decimal(wide_string wide_text, int wide_text_length,
//													city_coordinate ulx, city_coordinate uly,
//													city_coordinate character_width,	city_coordinate character_height,
//													bytes color_permutation,
//													city_coordinate total_width // re-used on 080803 to mean area for digits not including edges
//#if TT_ALPHA
//													,city_coordinate height_or_region_height
//#endif
//													) {
//#if TT_DEBUG_ON
//	if (debug_counter == tt_debug_target) {
//		log(_T("Displaying shrinking digits of debug target."));
//	};
//#endif
//#if TT_ALPHA
//	city_coordinate true_height = height_or_region_height; // new on 041003 so remote looks work correctly
//#else
//	city_coordinate true_height = height;
//#endif
//	int full_size_part = wide_text_length;
//	for (int i = 0; i < wide_text_length; i++) {
//		if (wide_text[i] == '.') {
//			full_size_part = i+2; 
// decimal point and first digit afterwards are full-size - was +1 prior to 080803 but then first digit after decimal point not treated special
//			break;
//		};
//	};
////	city_coordinate original_lly = uly-character_height; // new on 110803
//	city_coordinate original_uly = uly;
//	city_coordinate original_character_height = character_height;
//	int ignore_longest_line;
//	city_coordinate width_of_next, height_of_next;
//	tt_screen->get_extent_size(wide_text,full_size_part,1,character_width,character_height,
//									   ignore_longest_line,width_of_next,height_of_next,
//				                  TRUE,TRUE,FALSE);
//	uly = original_uly-(true_height-character_height)/2; // in case character has been shrunk - restored on 120803
////	uly = original_lly+character_height; // in case character has been shrunk
////	city_coordinate full_size_part_width = (total_width*full_size_part)/wide_text_length;
//	tt_screen->text_out((string) wide_text,full_size_part,ulx,uly, // adjusted_text_height+edge_size, 
//								character_width,character_height,TRUE,TRUE,TRUE,text_color,TRUE,color_permutation,WHITE,width_of_next,FALSE);
//	// FALSE added as last arg on 100804 since can't contain tabs
//	int remaining = wide_text_length-full_size_part;
//	city_coordinate max_ulx = ulx+total_width;
//	wide_string text_remaining = wide_text+full_size_part;
//	ulx += width_of_next;
//	for (int i = 0; i < remaining; i++) {
//		character_width = (city_coordinate) (character_width*number_shrinkage);
//		city_coordinate new_character_height = (city_coordinate) (character_height*number_shrinkage);
//		uly = original_uly-(true_height-new_character_height)/2; // height was original_character_height prior to 120803
////		uly = original_lly; // +new_character_height;
////#if TT_DEBUG_ON
////		if (i == 173800) {
////			debug_this();
////		};
////#endif
//		if (ulx+character_width > 0) { // condition new on 100804
//			tt_screen->text_out((string) text_remaining,1,ulx,uly, // adjusted_text_height+edge_size, 
//										character_width,new_character_height,TRUE,TRUE,TRUE,text_color,TRUE,color_permutation,WHITE,character_width,FALSE);
//			// FALSE added as last arg on 100804 since can't contain tabs
//			tt_screen->get_extent_size(text_remaining,1,1,character_width,new_character_height,
//												ignore_longest_line,width_of_next,height_of_next,
//												TRUE,TRUE,FALSE);
//		} else {
////			width_of_next = character_width; // since is fixed width font
//			// new on 150804 to skip over all of these that are left of the edge
//			// note that unlike the old code this doesn't round down on each iteration
//			// not fully debugged (clips some digits)
//			double character_width_as_double = character_width;
//			double character_height_as_double = new_character_height;
//			double ulx_as_double = ulx+width_of_next; // width_of_next since will add it soon after
//			while (i < remaining && ulx_as_double < 0) {
//				character_width_as_double *= number_shrinkage;
//				ulx_as_double += character_width_as_double;
//				character_height_as_double *= number_shrinkage;
//				i++;
//				text_remaining++;
//			};
//			width_of_next = (city_coordinate) character_width_as_double;
//			new_character_height = (city_coordinate) character_height_as_double;
//			ulx = (city_coordinate) ulx_as_double;
//		};
//		ulx += width_of_next;
//		if (ulx > max_ulx) return; // new on 080803
////		uly -= (character_height-new_character_height)/2; // to keep the bottom fixed
//		character_height = new_character_height;
//		text_remaining++;
//	};
//	// following an attempt to improve the appearance on 080803 but sometimes goes too far
////	tt_screen->text_out("...",1,ulx,uly,character_width,character_height,TRUE,TRUE,TRUE,text_color,FALSE,color_permutation,WHITE,character_width);
//};

#if TT_SHRINKING_AND_GROWING

void Number::compute_shrinking_decimal_places() {
	// changed from function to method on 080803 since number_shrinkage is no longer global
	// when pumping a number on the floor the first branch is taken not the second -- why? asked 150804
	city_coordinate one_x_pixel = tt_screen->one_pixel_in_y_city_units();
	city_coordinate one_y_pixel = tt_screen->one_pixel_in_y_city_units();
	// another failed 150803 experiment is to make this unconditonal
	if (!display_with_size_unconstrained && change_font_size_to_fit_extent_flag && !change_size_to_fit_extent_flag) { 
		// on 150803 replaced size_constrained() by !change_size_to_fit_extent_flag
		// added !display_with_size_unconstrained on 061104
		// search for a good value for number_shrinkage given the following character size and the width
		city_coordinate original_character_width = character_width;
		city_coordinate original_character_height = character_height;
//		city_coordinate edge_size = compute_edge_size(width,height); // noticed wasn't used anymore on 311004
		set_character_height(height); // was -2*edge_size prior to 150803
		// changed from 3/5 to 9/20 since 100x100 city coordinates is 75/100 pixel ratio 
		set_character_width((9*character_height)/20); 
		// typical for many fonts - looks pretty good -- was height rather than character height prior to 150803
//		set_change_font_size_to_fit_extent(FALSE); // taken care of here -- not completely - commented out on 051104
		double double_value;
		current_double_value(double_value,TRUE); 
		// what if out of range?? asked 041104 -- will give 308 as digits before decimal point which is OK here
		// since only used to split the space and with so much it'll just get half anyway
		int before_decimal_point = (int) log10(abs(double_value)); // abs new on 251004 since doesn't matter if negative
		if (before_decimal_point < 0) { // was <= prior to 150804 but single digit numbers need one digit before decimal point
			before_decimal_point = 1; // changed from 0 to 1 on 051104 since we now produce 0.5 rather than .5
		} else {
			before_decimal_point++;
		};
		// following commented out on 051104 since don't know how much space the integer part will take so compute enough digits for the whole thing
		// first check out current number_shrinkage
		// note that other code will re-compute part2_shrinkage more accurately
		//double total_width_available = width; // experiment 150803 -2.0*edge_size;
		//// complete rewrite on 120803 using a little math this time
		//// the width of the text is character_width*(1/(1-number_shrinkage))
		//if (tt_shrink_and_grow_numbers) {
		//	// give it up to half the total if it needs it
		//	if (character_width*before_decimal_point < total_width_available/2) { 
		//		// doesn't need to shrink
		//		part1_shrinkage = 0.0;
		//	} else {
		//		part1_shrinkage = compute_shrinkage(total_width_available/(2*character_width),before_decimal_point);
		//	};
		//};
		//double width_available;
		//if (part1_shrinkage > 0.0 && part1_shrinkage < 1.0) {
		//	width_available = total_width_available/2; // new on 011104 -- took half of it
		//} else {
		//	width_available = total_width_available-before_decimal_point*character_width;
		//};
		//if (width_available <= 0.0) { // must use smaller font
		//	part2_shrinkage = 0.0; // must be re-computed below
		//} else {
		// following changed to use width rather than width_available on 051104
		double part1_fraction,part2_fraction,fraction_per_character;
		shrinking_decimal_parts(before_decimal_point,width,character_width,fraction_per_character,part1_fraction,part2_fraction);
		set_part2_shrinkage(1.0-fraction_per_character/part2_fraction);
//		part2_shrinkage = 1.0-((double) character_width/width); // could this ever compute 1.0 and cause problems? asked 251004
//		};
//		const double min_number_shrinkage = .75;
//		if (part2_shrinkage < min_number_shrinkage) { // shrinks so fast won't see many digits
//			const double one_minus_min_number_shrinkage = 1.0-min_number_shrinkage;
//			double new_character_width = (one_minus_min_number_shrinkage*width_available)/(1.0+before_decimal_point*one_minus_min_number_shrinkage);
//			set_character_height((city_coordinate) (character_height*new_character_width)/character_width);
//			set_character_width((city_coordinate) new_character_width);
////			width_available = total_width_available-before_decimal_point*character_width; // commented out on 051104
//			// following changed to use width rather than width_available on 051104
//			part2_shrinkage = 1.0-(character_width/width); // will be very close to min_number_shrinkage
//		};
		double minimun_font_width_in_city_coordinates_as_double = (double) (minimum_font_width*one_x_pixel);
		double x_ratio = minimun_font_width_in_city_coordinates_as_double/character_width;
		double decimal_places_as_double = log(x_ratio)/log(part2_shrinkage);
		if (decimal_places_as_double < 1) { 
			decimal_places = 1; // must be very small but the old (prior to 151003) way of setting the decimal_places to 0 
			// makes the number display wrong even when expanded (so long as it stays constrained)
		//if (decimal_places_as_double < 3) { // won't see enough shrinkage so try a smaller font
		//	decimal_places = 0; // give up with shrinking digits - just not enough room -- 1 is an experiment 151003 - was 0
		} else {
			decimal_places = (int) (decimal_places_as_double+.5); // added .5 on 051104 to round up
		};
	} else { // unconstrained - probably on the floor
		compute_shrinking_decimal_places_unconstrained();		
	};
};

void Number::compute_shrinking_decimal_places_unconstrained() { // abstracted on 080803
	display_with_size_unconstrained = TRUE;
	city_coordinate one_x_pixel = tt_screen->one_pixel_in_y_city_units();
	city_coordinate one_y_pixel = tt_screen->one_pixel_in_y_city_units();
	if (one_x_pixel > character_width || one_y_pixel > character_height) {
		decimal_places = 1; // can't recover from 0 without being taken out of a box so updated on 151003
//		decimal_places = 0;
		return;
	};
	double x_ratio = ((double) minimum_font_width*one_x_pixel)/character_width; 
	// currently doesn't display fonts smaller than minimum_font_width pixels
	double y_ratio = ((double) minimum_font_height*one_y_pixel)/character_height;
	double ratio_log = log(max(x_ratio,y_ratio));
	//if (number_shrinkage_log == 0.0) {
	//	number_shrinkage_log = log(tt_number_shrinkage);
	//};
	set_part1_shrinkage(0.0); // shouldn't shrink it if unconstrained -- new on 051104
	set_part2_shrinkage(tt_number_shrinkage);
	decimal_places = max(1,((int) (ratio_log/log(part2_shrinkage))));
};

#else // old scheme:

void Number::compute_shrinking_decimal_places() {
	// changed from function to method on 080803 since number_shrinkage is no longer global
	// when pumping a number on the floor the first branch is taken not the second -- why? asked 150804
	city_coordinate one_x_pixel = tt_screen->one_pixel_in_y_city_units();
	city_coordinate one_y_pixel = tt_screen->one_pixel_in_y_city_units();
	// another failed 150803 experiment is to make this unconditonal
	if (change_font_size_to_fit_extent_flag && !change_size_to_fit_extent_flag) { 
		// experiment 150803 - replaced size_constrained() by !change_size_to_fit_extent_flag
		// search for a good value for number_shrinkage given the following character size and the width 
		city_coordinate original_character_width = character_width;
		city_coordinate original_character_height = character_height;
		city_coordinate edge_size = compute_edge_size(width,height);
		set_character_height(height); // was -2*edge_size prior to 150803
		// changed from 3/5 to 9/20 since 100x100 city coordinates is 75/100 pixel ratio 
		set_character_width((9*character_height)/20); 
		// typical for many fonts - looks pretty good -- was height rather than character height prior to 150803
		set_change_font_size_to_fit_extent(FALSE); // taken care of here
		// first check out current number_shrinkage
		double minimun_font_width_in_city_coordinates_as_double = (double) (minimum_font_width*one_x_pixel);
		double total_width_available = width; // experiment 150803 -2.0*edge_size;
		double double_value;
		current_double_value(double_value,TRUE);
		int before_decimal_point = (int) log10(double_value);
		if (before_decimal_point < 0) { // was <= prior to 150804 but single digit numbers need one digit before decimal point
			before_decimal_point = 0;
		} else {
			before_decimal_point++;
		};
		// complete rewrite on 120803 using a little math this time
		// the width of the text is character_width*(1/(1-number_shrinkage))
		double width_available = total_width_available-before_decimal_point*character_width;
		if (width_available <= 0.0) { // must use smaller font
			number_shrinkage = 0.0; // must be re-computed below
		} else {		
			number_shrinkage = 1.0-(character_width/width_available);
		};
		const double min_number_shrinkage = .75;
		if (number_shrinkage < min_number_shrinkage) { // shrinks so fast won't see many digits
			const double one_minus_min_number_shrinkage = 1.0-min_number_shrinkage;
			double new_character_width 
						= (one_minus_min_number_shrinkage*total_width_available)/(1.0+before_decimal_point*one_minus_min_number_shrinkage);
			set_character_height((city_coordinate) (character_height*new_character_width)/character_width);
			set_character_width( (city_coordinate) new_character_width);
			width_available = total_width_available-before_decimal_point*character_width;
			number_shrinkage = 1.0-(character_width/width_available); // will be very close to min_number_shrinkage
		};
		double x_ratio = minimun_font_width_in_city_coordinates_as_double/character_width;
		double decimal_places_as_double = log(x_ratio)/log(number_shrinkage);
		if (decimal_places_as_double < 1) { 
			decimal_places = 1; // must be very small but the old (prior to 151003) way of setting the decimal_places to 0 
			// makes the number display wrong even when expanded (so long as it stays constrained)
		//if (decimal_places_as_double < 3) { // won't see enough shrinkage so try a smaller font
		//	decimal_places = 0; // give up with shrinking digits - just not enough room -- 1 is an experiment 151003 - was 0
		} else {
			decimal_places = (int) decimal_places_as_double;
		};
	} else { // unconstrained - probably on the floor
		compute_shrinking_decimal_places_unconstrained();		
	};
};

void Number::compute_shrinking_decimal_places_unconstrained() { // abstracted on 080803
	city_coordinate one_x_pixel = tt_screen->one_pixel_in_y_city_units();
	city_coordinate one_y_pixel = tt_screen->one_pixel_in_y_city_units();
	if (one_x_pixel > character_width || one_y_pixel > character_height) {
		decimal_places = 1; // can't recover from 0 without being taken out of a box so updated on 151003
//		decimal_places = 0;
		return;
	};
	double x_ratio = ((double) minimum_font_width*one_x_pixel)/character_width; 
	// currently doesn't display fonts smaller than minimum_font_width pixels
	double y_ratio = ((double) minimum_font_height*one_y_pixel)/character_height;
	double ratio_log = log(max(x_ratio,y_ratio));
	//if (number_shrinkage_log == 0.0) {
	//	number_shrinkage_log = log(tt_number_shrinkage);
	//};
	number_shrinkage = tt_number_shrinkage;
	decimal_places = max(1,((int) (ratio_log/log(number_shrinkage))));
};

#endif


//
//void Number::compute_shrinking_decimal_places() {
//	// changed from function to method on 080803 since number_shrinkage is no longer global
//	city_coordinate one_x_pixel = tt_screen->one_pixel_in_y_city_units();
//	city_coordinate one_y_pixel = tt_screen->one_pixel_in_y_city_units();
//	if (change_font_size_to_fit_extent_flag && size_constrained()) {
//		// search for a good value for number_shrinkage given the following character size and the width
//		city_coordinate original_character_width = character_width;
//		city_coordinate original_character_height = character_height;
//		city_coordinate edge_size = compute_edge_size(width,height);
//		character_height = height-2*edge_size;
//		character_width = (3*height)/5; // typical for many fonts - looks pretty good
//		change_font_size_to_fit_extent_flag = FALSE; // taken care of here
//		// first check out current number_shrinkage
//		double minimun_font_width_in_city_coordinates_as_double = (double) (minimum_font_width*one_x_pixel);
//		const int max_improvements = 10; // for now
//		const double minimum_shrinkage = .25; // this was dumb - values less than 1/2 result in less than twice the original character width
//		long last_decimal_places = -1;
//		double width_available = width-2.0*edge_size;
//		double double_value;
//		current_double_value(double_value,TRUE);
//		int before_decimal_point = (int) log10(double_value);
//		if (before_decimal_point <= 0) {
//			before_decimal_point = 0;
//		} else {
//			before_decimal_point++;
//		};
//		for (int j = 0; j < max_improvements; j++) {
//			double x_ratio = minimun_font_width_in_city_coordinates_as_double/character_width;
//			decimal_places = (int) (log(x_ratio)/log(number_shrinkage));
//			if (decimal_places == last_decimal_places && decimal_places >= 3+before_decimal_point) {
//				return; // no change
//			};
//			last_decimal_places = decimal_places;
//			if (decimal_places < 3+before_decimal_point) { 
//				if (number_shrinkage <= minimum_shrinkage) { // already shrinking by more than a factor of 4 so try smaller font instead
//					character_width /= 2;
//					if (character_width <= minimun_font_width_in_city_coordinates_as_double) {
//						// give up trying to look nice - restore things and act unconstrained
//						character_width = original_character_width;
//						character_height = original_character_height;
//						change_font_size_to_fit_extent_flag = TRUE;
//						compute_shrinking_decimal_places_unconstrained(); 
//						return;
//					};
//					character_height /= 2;
//					number_shrinkage = tt_number_shrinkage;
//					j = 0; // like starting over
//				} else { // need to shrink faster
//					number_shrinkage *= 0.5;
//				};
//			} else {
//				double current_character_width = (double) character_width;
//				double width_with_current_settings = current_character_width;
//				boolean changed_number_shrinkage = FALSE;
//				for (int i = 1; i < decimal_places; i++) {
//					if (i >= before_decimal_point) {
//						current_character_width *= number_shrinkage;
//					};
//					width_with_current_settings += current_character_width;
//					if (width_with_current_settings > width_available) {
////						if (i == decimal_places-1) return; // so close call it done
//						// need to shrink faster since ran out of room
//						number_shrinkage = max(minimum_shrinkage,number_shrinkage-((1.0-number_shrinkage)*(decimal_places-i))/decimal_places);
//						changed_number_shrinkage = TRUE;
//						break;
//					};
//				};
//				if (!changed_number_shrinkage) {
//					if (width_available-width_with_current_settings < minimun_font_width_in_city_coordinates_as_double) {
//						return; // close enough
//					};
//					// shrinking too fast - reduce gap from 1 by ratio that we're off
//					number_shrinkage = number_shrinkage+((1.0-number_shrinkage)*(1.0-width_with_current_settings/width_available))*.5; 
//					// *.5 since over shooting
//				};
//			};
//		};
//		if (decimal_places < 3) {
//			decimal_places = 0; // don't use shrinking digits if so little room
//		};
//	} else { // unconstrained - probably on the floor
//		compute_shrinking_decimal_places_unconstrained();		
//	};
//};

Sprite *Number::respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected,
											   Sprite *by, boolean record_action, Sprite *original_recipient, 
												boolean size_constrained_regardless) {
//	if (infinite_stack()) return(this); // ignore keystrokes over stack - handled more generally now (080400)
   if (!puzzle_oks_typing_to_number_pads(by)) return(this);
	if (unary_operation(true_operation())) { // new on 050803 since if Unary operation shouldn't type to it (e.g. Integer Part)
		return(this); // beep too??
	};
	if (animation_in_progress()) {
		finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
	};
   if (!extended && keyboard_accelerator(key,FLIP_THING_HELD)) { // !extended added on 090804
      if (held()) return(used(NULL,by,FALSE,record_action,0,FALSE,NULL));
//      return(this); // commented out on 220505 since in base 16 to 36 need to type f
   };
	if (!by->can_send_keyboard_editing_keys() || reachable_from_remote_looks(original_recipient,this)) { 
		// condition new on 090604 -- removed !original_recipient->held() ||  on 100604
		// original_recipient test added on 300604
		if (rubout(key) && true_operation() != NO_NUMBER_OPERATION && sign() == 0) { 
			// rubbing out zero makes a "naked" operation - new on 250202
			set_is_an_operation_with_no_value(TRUE);
			need_to_recompute_text();
			return(this);
		};
		//if (key == VK_DELETE) {
		//   set_value(0); // new on 260902 -- thanks to Yishay for the suggestion
		//   return(this);
		//};
		if (key == '#') { // new on 270202
			long long_value;
			if (current_long_value(long_value,FALSE)) {
				if (long_value >= 2 && long_value <= 36) {
					set_base(long_value);
					set_long_value(0);
					return(this);
				};
			};
			// revert to decimal
			set_base(10);
			need_to_recompute_text();
			return(this);
		};
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	if (respond_to_keyboard_body(key,extended,robot_selected,by,record_action,original_recipient,size_constrained_regardless)) {
//		if (leader == NULL && !is_picture()) { // new on 050202
//			change_size_to_fit_extent_flag = TRUE;
//		};
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		return(Sprite::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,
			                                size_constrained_regardless));
	} else {
		if (by != NULL) {
			by->completed(); // new on 010904
		};
		return(this);
	};
};

Sprite *Number::select(TTRegion *, SelectionReason reason, Sprite *in_hand) {
	if (reason == MIGHT_DROP_ITEM && in_hand != NULL) {
		// was missing in_hand-> prior to 290204
		if (in_hand->looks_like_is_of_type(INTEGER) || 
			 (blank && (in_hand->looks_like_is_of_type(TEXT)) || 
						   in_hand->kind_of() == CUBBY ||
							in_hand->kind_of() == PROGRAM_PAD || // new on 010205
							in_hand->kind_of() == ROBOT || // new on 010205
							(in_hand->kind_of() == REMOTE_PICTURE && in_hand->can_be_a_number()))) {
			// added in_hand->kind_of() == REMOTE_PICTURE on 111104 but not all remotes makes sense so check if can_be_a_number
			// cubby test new on 280404
			return(this);
		} else {
			return(NULL);
		};
		// rewritten on 140403 to deal with indirections
		//switch (in_hand->kind_of()) {
		//	case TEXT:
		//		if (!blank) return(NULL);
  //          return(this);
		//	case INTEGER:
		//		return(this);
		//	default:
		//	  return(NULL);
		//};
	};
	return(this); 
};

void Number::adjust_grasp(city_coordinate &adjustment_x, city_coordinate &adjustment_y) {
	adjustment_x = tile_width/-2; // so the rightmost digit is visible
	adjustment_y = 0;
};

void Number::good_size(city_coordinate &good_width, city_coordinate &good_height, Sprite *source, boolean for_expander) {
	good_height = (4*default_talk_balloon_character_height()/3);
#if TT_SHRINKING_AND_GROWING 
	if (tt_log_version_number >= 59) {
		good_width = (good_height*14)/20; // new on 131104 to be consistent with new style -- trying 14 rather than 9 on 141104
	} else 
#endif
	good_width = (8*default_talk_balloon_character_width())/3; // 4 was 8 prior to 170102 and is again on 220102
	if (is_blank()) { // new on 140202
		good_width = (4*good_height)/3; // new on 140202
	} else {
//		if (text_length < 0) { // not computed yet
//			update_text_and_widths(TRUE);
//		};
	//	good_width *= text_length;
		// following copied from text::good_size on 150102
		// 8/3 and 4/3 in the following since set_font now uses character size not cell size (110102)
		wide_string wide_text;
		long wide_text_length;
//		if (!display_with_size_unconstrained) { // condition new on 131104 -- condition commented out on 141104
			text_length = -1; // force the following to recompute
//		};
#if TT_SHRINKING_AND_GROWING
		boolean saved_flag = change_font_size_to_fit_extent_flag;
		// new on 060705 since otherwise birds carrying numbers can have the wrong size due to set_change_font_size_to_fit_extent(FALSE) below
#endif
		set_character_width(good_width); // new on 150803
		set_character_height(good_height); // so the following computes shrinking digits correctly new on 140803
		current_wide_text(wide_text,wide_text_length); // if is a remote_picture this could be a leak since they cons the wide_text -- sigh...
#if TT_SHRINKING_AND_GROWING
		if (tt_log_version_number >= 78) {
			set_change_font_size_to_fit_extent(saved_flag);
		} else if (tt_log_version_number >= 59) {
			set_change_font_size_to_fit_extent(FALSE); // above may have reset this -- new on 131104
		};
#endif
		int ignore_longest_line;
		int equivalent_length;
#if TT_SHRINKING_AND_GROWING
//		boolean height_needs_to_be_doubled = FALSE; // unless is a multi-line fraction
		if (tt_log_version_number >= 58 && tt_shrink_and_grow_numbers) {
			// added tt_shrink_and_grow_numbers on 101204
			// new on 061104
			if (decimal_places == 0) { // added decimal_points == 0 on 081104 since taken care of below then
				int slash_index = find_separator_location(wide_text,wide_text_length,L"/");
//				double length_as_double;
				if (slash_index > 0 && !edited && wide_text[slash_index-1] != L' ') { 
					// added space test on 091104 in case is part of a sequence of operations
					int space_index = find_separator_location(wide_text,slash_index,L" ");
					//if (space_index < 0 && operation != NO_NUMBER_OPERATION && following_operation == NULL) {
					//	// new on 011204 to deal with operations like /3/2 better
					//	space_index = 0; // right after operation -- always correct??
					//};
					if (space_index > 0) {
						equivalent_length = space_index+(1+max(slash_index-(space_index+1),(wide_text_length-(slash_index+1))))/2;
						// 1+ for extra space (e.g. between integer part and fraction) -- 
						// should compute same value as non_shrinking_or_integer_part_and_fraction_equivalent
						//length_as_double = non_shrinking_growing_equivalent(space_index,part1_shrinkage)+
						//						 max(non_shrinking_growing_equivalent(separator_index-(space_index+1),part2_shrinkage),
						//							  non_shrinking_growing_equivalent(wide_text_length-(separator_index+1),part2_shrinkage))+
						//						 1.0; // for extra space (e.g. between integer part and fraction)
//						height_needs_to_be_doubled = TRUE; // since have full size and half size fonts -- experiment 131104 to remove this
					} else { // just a 2D fraction
						equivalent_length = max(slash_index,(wide_text_length-(slash_index+1)));
						// rewritten on 081104 since there is no shrinking_decimals to deal with
						//length_as_double = max(non_shrinking_growing_equivalent(separator_index,part1_shrinkage),
						//							  non_shrinking_growing_equivalent(wide_text_length-(separator_index+1),part1_shrinkage)); 
						// share same shrinkage
					};
				} else { // e.g. decimal point but not shrinking decimals
					equivalent_length = wide_text_length;
					// prior to 081104 was the following
					//length_as_double = non_shrinking_growing_equivalent(separator_index,part1_shrinkage)+
					//						 non_shrinking_growing_equivalent(wide_text_length-(separator_index+1),part2_shrinkage)+
					//						 decimal_point_width_relative_to_a_digit; // for decimal point
				};
			} else {
				// no separator
//				length_as_double = non_shrinking_growing_equivalent(wide_text_length,part1_shrinkage);
				equivalent_length = non_shrinking_equivalent(wide_text_length); // prior to 131104 was just wide_text_length; 
				// the whole thing -- updated 081104
			};
//			equivalent_length = (int) (length_as_double+0.5); // round up
		} else { // if running old demos or without new number display code then do the old thing:
			equivalent_length = non_shrinking_equivalent(wide_text_length);
		};
//		good_width = equivalent_length*good_width; // experiment on 141104
#else
		equivalent_length = non_shrinking_equivalent(wide_text_length);
#endif
		tt_screen->get_extent_size(wide_text,equivalent_length,1,									  
											good_width,good_height,
											ignore_longest_line,good_width,good_height,
											TRUE,TRUE,FALSE);
//#if TT_SHRINKING_AND_GROWING
//		// new on 061104
//		if (height_needs_to_be_doubled) {
////			good_height *= 2;
//			good_width /= 2; // updated on 131104 so the integer part is the normal height and the fraction is half size font
//		};
//#endif
		city_coordinate edge_size = compute_edge_size_given_text_size(good_width,good_height);
		good_width += edge_size*2; // new on 270202
		good_height += edge_size*2;
	};
	if (tt_log_version_number >= 40) {
		// the following does the right thing if standing in room while robot is working - new on 040404
		adjust_good_size(good_width,good_height,source);
	} else {
		adjust_size_for_source(good_width,good_height,source);
	};
};

boolean Number::equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, SpritesPointer &expected_corresponding_nests,
								 EqualReason reason) { // new on 060601
	if (other == this) return(TRUE);
	if (other->kind_of() != kind_of()) return(FALSE);
	if (is_blank() != other->is_blank()) return(FALSE);
	Number *other_integer = (Number *) other;
	if (return_text_color() != other_integer->return_text_color()) return(FALSE);
	if (true_operation() != other_integer->true_operation()) return(FALSE); // was current_operation prior to 100202
	boolean need_to_restore_blankness = FALSE;
	if (is_blank()) { // and so must the other one be - new on 030202
		if (reason == DUMPING_EQUALS_TOGETHER) {
			// then shouldn't collapse two blank numbers with different "original" values
			just_set_blank(FALSE); // so current_value works fine
			other->just_set_blank(FALSE);
			need_to_restore_blankness = TRUE;
		} else {
			return(TRUE); // both blank so equal
		};
	};
	NumberValue *my_value, *other_value;
	boolean my_value_shared, other_value_shared;
	current_value(my_value,my_value_shared);
	other_integer->current_value(other_value,other_value_shared);
	if (need_to_restore_blankness) {
		just_set_blank(TRUE); 
		other->just_set_blank(TRUE);
	};
	boolean result;
	if (my_value == NULL) { // new on 060302 since remote integers can have NULL for 0 here
		if (other_value == NULL) {
			result = TRUE;
		} else {
			result = (other_value->sign() == 0);
		};
	} else if (other_value == NULL) {
		result = (my_value->sign() == 0);
	} else {
		result = my_value->equals(other_value);
	};
	if (!my_value_shared) delete my_value;
	if (!other_value_shared) delete other_value;
	return(result);
};

ReleaseStatus Number::item_released_on_top(Sprite *item, Sprite *by, 
														 SpritePointer &recipient, int level) {
	if (infinite_stack() && !tt_copy_top_of_stack && !by->still_running()) {
		return(RELEASE_IGNORED);
	};
	switch (item->kind_of()) {
		case INTEGER:
			if (infinite_stack()) { // RELEASED_ON_ITS_STACK handled by caller now
				if (!tt_copy_top_of_stack && !by->still_running()) { // copied this here and commented out the following on 290102
					by->action_aborted();
					return(RELEASE_IGNORED);
				};
//				long others_value;
//				if (item->current_value(others_value,by)) {
////					if (value == others_value) {
//						// not bothering to check subtype just now
////						return(RELEASED_ON_ITS_STACK);
////					} else {
//						recipient = copy_top_of_stack(by);
//						boolean saved_tt_recording_off = tt_recording_off;
//						tt_recording_off = TRUE;
//						recipient->item_released_on_top(item,by,recipient,level,action_status);
//						tt_recording_off = saved_tt_recording_off;
//						record_event(RELEASE_ITEM_ON,by,floor,this);
//                  item->now_released_on(this,by);
//						return(RELEASE_HANDLED);
//					};
//				} else {
//					by->abort_action_if_robot(action_status,IDS_NUMBER_STACK_WONT_ACCEPT);
//					return(RELEASE_IGNORED);
//				};
			}; // and the following
		case TEXT: 
		  if (receive_item(item,by,default_duration(),this,item)) {
			  record_event(RELEASE_ITEM_ON,by,floor,this);
           item->now_released_on(this,by);
			  switch (item->true_operation()) {
				  // updated on 161204 since wasn't noticing what operations were being done
//				  case '+':
				  case INCREASE_BY:
				  case NO_NUMBER_OPERATION:
					  user_did(INTEGER,INTEGERS_ADDITION,by);
					  break;
				  case SUBTRACT_FROM: // '-':  // rename this since subtraction is now the semantics
					  user_did(INTEGER,INTEGERS_MADE_NEGATIVE,by);
					  break;
				  //case '*':
				  //case 'x':
				  //case 'X':
				  case MULTIPLY_BY:
					  user_did(INTEGER,INTEGERS_MULTIPLICATION,by);
					  break;
				  case DIVIDE_BY: // '/':
					  user_did(INTEGER,INTEGERS_DIVISION,by);
					  break;
				  case MODULUS_BY: // '%':
					  user_did(INTEGER,INTEGERS_MODULO,by);
					  break;
				  case MAKE_EQUAL: // '=':
					  user_did(INTEGER,INTEGERS_ASSIGNMENT,by);
					  break;
				  //case '^':
				  //case '|':
				  //case '&':
				  case BITWISE_EXCLUSIVE_OR:
				  case BITWISE_AND:
				  case BITWISE_OR:
					  user_did(INTEGER,INTEGERS_BITWISE_OPERATIONS,by);
					  break;
			  };		  
			  return(RELEASED_ON_NUMBER);
		  }; 
        return(RELEASE_IGNORED);
		case PROGRAM_PAD: // new on 151199
		case ROBOT:
			if (!blank) break; // compute size of team if blank or number of pages in a notebook
		case PICTURE: // may be forwarding to text or number
		case REMOTE_PICTURE: // or might be an address sensor or the like
			{ 
//			  ActionStatus temp_status;
//			  if (action_status == NULL) action_status = &temp_status;
//			  receive_item(item,by,default_duration(),action_status,this);
//			  if (!is_action_aborted(action_status)) {
				// rewrote the above as the following on 230499 since that is the intent of
				// receive_item returning a boolean and also the old version didn't work since
				// temp_status wasn't initialized
			  if (receive_item(item,by,default_duration(),this,item)) {
				  record_event(RELEASE_ITEM_ON,by,floor,this);
				  item->now_released_on(this,by);
				  return(RELEASED_ON_NUMBER);
			  } else {
				  return(RELEASE_IGNORED);
			  };
		  };
		case CUBBY: 
		  if ((leader == NULL || is_blank()) && tt_system_mode != PUZZLE) { 
			  // breaking cubby in two (or as updated 070205 is dropped on an erased number)
           // same day puzzles may want this enabled but not the first 60 puzzles
//           item->now_released_on(by);  // cubby is still around after this
			  if (receive_item(item,by,default_duration(),this,item)) {
				  	record_event(RELEASE_ITEM_ON,by,floor,this,TRUE); 
					// this was before receive_item prior to 230499
//			  if (!is_action_aborted(action_status)) {
				  // recorded above to get the order of events right
	//			  record_event(RELEASE_ITEM_ON,by,floor,this,TRUE);
				  return(RELEASED_ON_NUMBER);
			  } else { // might be in trouble here since recorded release above...
				  return(RELEASE_IGNORED);
			  };
		  } else break;
	};
	by->abort_action_if_robot(IDS_NUMBER_WONT_ACCEPT);
	return(RELEASE_IGNORED);
};

boolean Number::receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item) {
   if (inside_vacuum()) {
      by->abort_action(IDS_ROBOT_MISSING_RECIPIENT_ABORT);
      return(FALSE);
   };
   if (to_be_smashed()) {
		by->abort_action_if_robot(IDS_ANOTHER_MOUSE_SMASHING_IT,MOUSE_WITH_HAMMER);
		return(FALSE);
   };
	boolean handled = FALSE; // moved up here on 120604
	Number *other = NULL; // moved up here on 120604
	if (blank) {
		if (item->kind_of() == REMOTE_PICTURE) {
			// the fix of 120604 below prevented coercing remote pictures to numbers -- e.g. addresses -- updated 111104
			long items_value;
			if (item->current_long_value(items_value)) {
				set_long_value(items_value);
				handled = TRUE;
			} else {
				return(FALSE);
			};
		} else {
			// new on 120604 since the following was limited to longs
			long item_text_length;
			string item_text;
			if (item->current_text(item_text,item_text_length)) {
				if (set_value_as_string(item_text,item_text_length)) {
					handled = TRUE;
					user_did(INTEGER,INTEGERS_CONVERSION);
					//by->completed();
					//return(TRUE);
				} else {
	//				by->action_aborted();
					return(FALSE);
				};
			};
		};
	};
	if (!handled) {
		long long_value = 0;
		NumberValuePointer item_value;
		boolean item_value_shared;
		boolean item_has_value = item->current_value(item_value,item_value_shared,by);
		if (blank && item_has_value) { // not clear anymore if this needs special handling here
			NumberValue *item_value_copy; // new on 160702 in case shared
			if (item_value_shared) {
				item_value_copy = item_value->copy();
			} else { 
				item_value_copy = item_value; // has already been copied
			};
			boolean delete_bammed = ::ok_to_delete_bammed(by);
			accept_result(item_value_copy,item,by,duration,original_recipient,original_item,delete_bammed); 
			// last arg was FALSE prior to 131000
	//		if (!item_value_shared) delete item_value; 
			// commented out on 200202 since has become the new value of this
			if (operation != NO_NUMBER_OPERATION) { 
				// condition new on 070604 -- since I'm an erased operation then copy the item's operation
				operation = item->true_operation(); // this had been commented out ages ago (and used current_operation())
			};
			if (item->kind_of() == CUBBY) {
				user_obtained_arity();
				// recorded as release_on_top but also like release_on
				// needs to be recorded in case item is "new"
				if (!delete_bammed) { // condition new so this only applies to old code
					record_event(NEW_ITEM,by,floor,item,FALSE); // restored on 200205 since broke Sentence Generator example
				};
				// commented out the above on 060205 after realizing it does nothing useful and did confuse Java generator
				// changed from TRUE to FALSE on 060205 since type isn't really fixed 
				// and this confuses the Java generator if COPY_APPLY was previous event
			} else if (item->kind_of() != INTEGER) {
				user_did(INTEGER,INTEGERS_CONVERSION);
			};
			return(TRUE);
		};
		switch (item->kind_of()) {
			case REMOTE_PICTURE: // new on 310399 so that you can drop addresses on numbers, etc.
				if (!item->can_be_a_number()) {
					handled = FALSE;
					break;
				}; // else fall thru to the following
			case INTEGER:
				other = (Number *) item;
				handled = TRUE;
				break;
			case PICTURE:
				other = (Number *) ((Picture *) item)->forwarding_to(FALSE);
				if (other != NULL && other->kind_of() == INTEGER && other != this) { // ok
					if (tt_copy_top_of_stack && infinite_stack()) {
						return(copy_top_of_stack(by)->receive_item(item,by,duration,original_recipient,original_item));
					};
					handled = TRUE;
					break; 
				};
				if (((Picture *) item)->is_flipped()) { // ignore but don't warn
					by->action_aborted();
					return(FALSE);
				};
				handled = FALSE;
				break;
			case CUBBY: {
				// recorded as release_on_top but also like release_on
				// needs to be recorded in case item is "new"
				record_event(NEW_ITEM,by,floor,item,TRUE);
				Cubby *cubby = (Cubby *) item;
				if (floor != NULL) { // conditional new on 021000 (Thanks to Micke Kindborg)
					// can be NULL if this is running on the back of a picture
					cubby->now_on_floor(floor,by);
				};
				NumberValuePointer my_result;
				boolean my_result_shared;
				current_value(my_result,my_result_shared,by); // check if failure??
				if (my_result->long_value(long_value) && long_value >= 0 && long_value <= cubby->current_number_of_holes()) {
					if (!my_result_shared) delete my_result;
					// number is used to break the cubby in two
					Cubby *excess_on_right = cubby->set_number_of_holes(long_value,TRUE,TRUE); 
					// coerced to (cubby_index) prior to 070203
					if (excess_on_right != NULL) {
						if (floor != NULL) { // conditional new on 021000 (Thanks to Micke Kindborg)
							floor->add_item(excess_on_right);
						};
						if (by->showing_on_a_screen()) {	// play crack sound
							play_sound(SWITCH_SOUND);
							if (cubby->current_llx()+cubby->current_width() < 0) { 
								// new on 220102 so that you can see both parts when broken
								cubby->set_llx(tile_width-cubby->current_width());
							};
						};
						record_event(NEW_ITEM,by,floor,excess_on_right,TRUE,0,ADD_TO_WORKING_SET_REGARDLESS);
						if (!by->still_running()) user_broke_cubby();
					}; // else what about NEW_ITEM and indices??
					by->completed();
				} else {
					if (!my_result_shared) delete my_result;
					if (long_value < 0) {
						by->abort_action(IDS_NUMBER_UNDERNEATH_IS_NEGATIVE);
					} else {
						by->abort_action(IDS_NUMBER_UNDERNEATH_TOO_BIG);
					};
					return(FALSE);
				};
				return(TRUE);
				};
			default: 
				handled = FALSE;
		};
		if (!handled) {
			if (infinite_stack()) {
				by->action_aborted();
			} else { 
				by->abort_action_if_robot(IDS_ONLY_NUMBERS_ON_NUMBERS,
												  MOUSE_WITH_HAMMER,
												  !told_user(TOLD_NUMBERS_ON_NUMBERS));
				if (by->kind_of() == ROBOT) { // new on 260200
					tt_error_file() << "The following was dropped on the number " << long_value;
#if TT_DEBUG_ON
					tt_error_file() << " by ";
					by->print(tt_error_file());
#endif
					tt_error_file() << endl;
					item->top_level_describe(tt_error_file());
					tt_error_file() << endl;
					// is this important enough to set importance flag??
				} else { // following was unconditional
					user_told(TOLD_NUMBERS_ON_NUMBERS);
				};
			};
			return(FALSE);
		};
		if (other->is_blank()) {
			if (infinite_stack()) {
				by->action_aborted();
			} else {
				by->abort_action(IDS_TOP_NUMBER_IS_BLANK,MOUSE_WITH_HAMMER,4);
			};
			return(FALSE);
		};
	};
	// following copied and revised on 140202	
	if (duration > 0 && 
		 (// !delete_item || -- commented out on 140202 
		  !item->to_be_smashed())) {
//		 && // not another mouse in progress
//		 mouse == NULL) {
		// if just concatenating operations then true_operation test will be FALSE 
		// and other won't be deleted (since it is incorporated into the other and just removed from floor)
		// replaced item with other since needed if is a picture forwarding to a number 130504 -- restored as experiment 130504
		call_in_a_mouse(this,item,item->pointer_to_background(),by,FALSE,
			             (true_operation() == NO_NUMBER_OPERATION || is_an_operation_with_no_value()), 
							 // added is_an_operation_with_no_value on 250202 since other should be deleted 
							 // if giving a value to this who has none 
							 duration,original_recipient,original_item);
	} else {
		item->set_to_be_smashed(TRUE); // so it acts like call_in_a_mouse above -- new on 031105
		if (other == NULL) {
			set_to_future_value(item,original_recipient,by); // new on 120604
		} else {
			set_to_future_value(other,original_recipient,by);
		};
		item->remove_from_floor(FALSE); // new on 170202
		item->destroy(by); // new on 170202
		by->completed();
	};
	return(TRUE); // I guess
};

//#if !TT_MULTIPLE_PRECISION // not clear how to deal with this 300102 - commented out on 310102 since :set_value takes care of this 
//	if (operation == '-') {
//		future_value = -future_value; // what about value??
//		operation = '+';
//	};
//#endif
	/* commented out all of this on 140202 since it is expensive (and when dealing with remote_integers can cause operations to apply twice)
	// I think all this accomplished was giving you an error message at the appropriate point - only divide by zero (and some roots are a problem now)
	if (true_operation() != NO_NUMBER_OPERATION && item->kind_of() == INTEGER) { 
	// new policy as of 100202 that operations combine to form a sequence of operations
		accept_result(NULL,item,by,duration,action_status,original_recipient);
		return(TRUE);
	};
	NumberValue *my_value;
	boolean my_value_shared;
	current_value(my_value,my_value_shared,by);
	NumberValuePointer result_value;
	ArithmeticException exception = result_of_operation(other,my_value,result_value,by); 
	if (!my_value_shared) delete my_value;
	if (exception == NO_EXCEPTION && result_value != NULL) {
//		if (result < 0 && operation == '-') { // commented out on 300102 since this functionality is part of result_of_operation
//			 result = -result;
//			 operation = '+';
//		};
      // moved this here since call_in_a_mouse removes index
//	   record_event(RELEASE_ITEM_ON,by,floor,this);
		exception = accept_result(result_value,item,by,duration,action_status,original_recipient); 
		// might be new exceptions (e.g. an integer not accepting a bignum)
		if (exception == NO_EXCEPTION) {
			add_operation(other);
			if (duration > 0) { // didn't do it yet so need to save result_value
				if (future_value != NULL) {
					delete future_value;
				};
				future_value = result_value;
			};
		};
	};
	if (exception != NO_EXCEPTION) { // else overflow or underflow so don't smash them
		if (showing_on_a_screen()) play_sound(PLOP_SOUND,5);
		other->set_priority(ultimate_leader()->priority()-1); // on top of me (was ultimate_leader)
		switch (exception) {
			case INTEGER_OVERFLOW:
				by->abort_action(action_status,IDS_ABORT_NUMBER_TOO_LARGE,MOUSE_WITH_HAMMER,4);
				break;
			case INTEGER_UNDERFLOW:
				by->abort_action(action_status,IDS_ABORT_NUMBER_TOO_SMALL,MOUSE_WITH_HAMMER,4);
				break;
			case DIVIDE_BY_ZERO:
				by->abort_action(action_status,IDS_CANT_DIVIDE_BY_ZERO,MOUSE_WITH_HAMMER,4);
				break;
			default:
				by->abort_action(action_status,IDS_ABORT_NUMBER_TOO_SMALL_OR_BIG,MOUSE_WITH_HAMMER,4);
		};
		return(FALSE);
	};
	*/

ArithmeticException Number::result_of_operation(Sprite *other, NumberValue *my_number_value, 
																NumberValuePointer &number_value_result, 
																Sprite *by, boolean performing_operations) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("result_of_operation of debug target");
	};
#endif
	// re-written on 100202 so all the operations are performed if it is a concatenated sequence
	try { // new on 130202 -- GNU MP will raise divide by zero exceptions when it isn't really (e.g. square root of -2)
		UnwindProtect<boolean> set(tt_ok_to_process_windows_messages,TRUE); 
		// new on 050305 so if any Windows message arise here they are handled
		ArithmeticException exception = result_of_operation_once(other,my_number_value,number_value_result,by);
		if (number_value_result == NULL) { // new on 130202
			return(exception);
		};
		if (exception == NO_EXCEPTION && other->kind_of() == INTEGER) {
			Number *other_number = (Number *) other;
			boolean other_exact = other_number->exact();
			switch (other_number->true_operation()) {
				case MAKE_EQUAL: // then only the other's exactness counts -- new on 170202
					number_value_result->set_exact(other_exact);
					break;
				case INTEGER_PART:
				case DENOMINATOR:
				case NUMERATOR:
					// new on 050803 - these are always exact (but shouldn't need to set)
					break;
				default:
					if (number_value_result->and_exact(other_exact)) { // new on 130202 to propogate inexactness
						number_value_result->and_exact(my_number_value->exact()); // no point if already FALSE
					};
			};
			Number *others_following_operation = other_number->pointer_to_following_operation();
			if (others_following_operation != NULL) {
				return(result_of_operation(others_following_operation,number_value_result,number_value_result,by));
			};
			if (!performing_operations) { // condition (and arg) new on 080403
				add_operation(other_number); 
				// restored on 080403 after going away when I switched to bignums and rationals - 
				// only matters if this is a remote_integer and read_only()
			};
		};
		return(exception);
	} catch (string string_exception) {
		if (string_exception != NULL) {
//			tt_error_file() << string_exception << endl;
			// replaced the above on 050305 since this is typically useful information
			string problem = append_3_strings(S(IDS_PROGRAMMER_ABORT),string_exception,S(IDS_PROGRAMMER_ABORT2,". "));
			say(problem,3,TRUE);
			delete [] problem;
			return(SOME_ARITHMETIC_EXCEPTION_ALREADY_REPORTED); // new on 070305
		};
		return(SOME_ARITHMETIC_EXCEPTION);
	} catch (...) {
		return(SOME_ARITHMETIC_EXCEPTION);
	};
};

ArithmeticException Number::result_of_operation_once(Sprite *other, NumberValue *my_number_value, 
																	  NumberValuePointer &number_value_result, 
																	  Sprite *by) {
	// re-written on 010202 to work with different kinds of NumberValue results
	// only remaining exception is divide by zero (others are computed by caller)
	// if no exceptions then number_value_result is freshly created here
	NumberValue *others_number_value;
	boolean others_number_value_shared;
	if (other->current_value(others_number_value,others_number_value_shared,by)) {
//		boolean need_to_negate_others_number_value = FALSE;
		NumberOperation others_operation = other->true_operation();
		if (others_operation == NO_NUMBER_OPERATION) {
			others_operation = INCREASE_BY; // interpret it as addition - rewritten on 030302
		};			
		switch (others_operation) {
			case DIVIDE_BY:
			case MODULUS_BY:
				if (others_number_value->sign() == 0) {
					if (!others_number_value_shared) delete others_number_value;
					return(DIVIDE_BY_ZERO);
				};
//				number_value_result = my_number_value->do_operation(DIVIDE_BY,others_number_value);
//				if (!others_number_value_shared) delete others_number_value;
//				return(NO_EXCEPTION); // can it go wrong with integers?
				// beginning 030302 these all share the same following code
			case SUBTRACT_FROM:
//				others_number_value->negate(); // not good if using threads...
//				need_to_negate_others_number_value = TRUE;
//				// and the following				
			case INCREASE_BY:
			case MULTIPLY_BY:
//			case NO_NUMBER_OPERATION: // other guy is a plain number -- REVISIT THIS
//			  result = my_value+others_value;
				number_value_result = my_number_value->do_operation(others_operation,others_number_value);
//			  if (others_value > 0 && my_value > 0 && result < 0) {
//				  return(INTEGER_OVERFLOW);
//			  } else if	(others_value < 0 && my_value < 0 && result > 0) {
//				  return(INTEGER_UNDERFLOW);
//			  } else {
//				if (operation == '-' && number_value_result->sign() < 0) {
////					result = -result;
//					number_value_result->negate();
//					operation = '+';
//				};
				if (!others_number_value_shared) {
					delete others_number_value;
//				} else if (need_to_negate_others_number_value) {
//					others_number_value->negate();
				};
				return(NO_EXCEPTION);
//			case MULTIPLY_BY:
//				result = my_value*others_value;
//				number_value_result = my_number_value->do_operation(MULTIPLY_BY,others_number_value);
//				if (others_value != 0 && result/others_value != my_value) {
//					if ((others_value < 0 && my_value < 0) ||
//						 (others_value > 0 && my_value > 0)) {
//						return(INTEGER_OVERFLOW);
//					} else {
//						return(INTEGER_UNDERFLOW);
//					};
//				} else {
//				if (!others_number_value_shared) delete others_number_value;
//				return(NO_EXCEPTION);
			case MAKE_EQUAL:
				if (others_number_value_shared) {
					number_value_result = others_number_value->copy();
				} else {
					number_value_result = others_number_value;
				};
				return(NO_EXCEPTION);
//			case MODULUS_BY:
//				if (others_number_value->sign() == 0) {
//					if (!others_number_value_shared) delete others_number_value;
//					return(DIVIDE_BY_ZERO);
//				};
//				number_value_result = my_number_value->do_operation(MODULUS_BY,others_number_value);
//				if (!others_number_value_shared) delete others_number_value;
//				return(NO_EXCEPTION);	
//			case BITWISE_EXCLUSIVE_OR:
//			case BITWISE_AND:
//			case BITWISE_OR:
			default:
				number_value_result = my_number_value->do_operation(others_operation,others_number_value);
				if (!others_number_value_shared) delete others_number_value;
				return(NO_EXCEPTION);				
		};
	}; // else is blank? -- shouldn't happen
	number_value_result = NULL; // new on 100202
	return(NO_EXCEPTION); 
	// was SOME_ARITHMETIC_EXCEPTION prior to 091100 but wrong error message for blanks or inoperative remote controls...
};

ArithmeticException Number::accept_result(NumberValue *result,
														Sprite *item, Sprite *by, millisecond duration, 
														Sprite *original_recipient,
														Sprite *original_item,
														boolean delete_item) {
	// obsolete as of 140202 except for blank branch???
	// didn't overflow 
	Background *items_floor = item->pointer_to_background();
// following is wasteful -- tried duration instead but didn't work
	if (duration > 0 && //default_duration() != 0 && 
		 (!delete_item || !item->to_be_smashed())) {
//		 && // not another mouse in progress
//		 mouse == NULL) {
//		mouse = 
		if (result == NULL) delete_item = FALSE; // since "just" concatenating operations
		call_in_a_mouse(this,item,items_floor,by,FALSE,delete_item,duration,original_recipient,original_item);
//		future_value = result; 
	} else if (result == NULL) { // new on 100202 - am concatenating operations (could check that this has an operation)
		set_following_operation((Number *) item); // caller checked type so cast is OK
	} else {
		blank = FALSE; // If I'm blank become the other
		set_value(result);
		if (delete_item) {
			if (items_floor != NULL) items_floor->remove_item(item,FALSE);
			if (!item->to_be_smashed()) {
				item->destroy(by);
			};
		};
//		if (original_recipient != this) { // probably forwarded by a picture -- remote appearances might be involved so re-compute
//		   update_text_and_widths(FALSE);
//		};
		by->completed();
	};
	return(NO_EXCEPTION);
};

MatchStatus Number::match(Number *other_integer, int version) {
	if (tt_match_blank_with_sensor_then_run_once_per_cycle) { // new on 030404 to do this before matching
		if (tt_touched_a_remote != ONLY_ON_NEW_CYCLE) {
			set_touched_a_remote(other_integer->change_frequency());
		};
	};
	if (blank) {
		return(MATCH_GOOD);
	};
	if (!tt_match_blank_with_sensor_then_run_once_per_cycle) { // new on 030404
		if (tt_touched_a_remote != ONLY_ON_NEW_CYCLE) {
			set_touched_a_remote(other_integer->change_frequency());
		};
	};
//	if (error) return(MATCH_BAD);
	if (true_operation() != other_integer->true_operation()) {
		return(MATCH_BAD);
	};
	NumberValue *my_value,*other_value;
	boolean my_value_shared, other_value_shared;
//   boolean other_positive;
	if (!other_integer->current_value(other_value,other_value_shared)) {
		return(MATCH_BAD);
	};
	current_value(my_value,my_value_shared);
//#if TT_DEBUG_ON
//	if (tt_debug_target == 98) { // just debuggin
//		tt_error_file() << "My value is " << my_value << " and other value is " << other_value << endl;
//	};
//#endif
//	if (my_value == other_value) {
	boolean equal = my_value->equals(other_value);
	if (!other_value_shared) delete other_value;
	if (!my_value_shared) delete my_value;
	if (equal) {
		return(MATCH_GOOD);
	} else {
		return(MATCH_BAD);
	};
};

unsigned char Number::number_dump_type(boolean &dump_language) { // renamed on 240601 to avoid confusion with Sprite::dump_type()
	short int joystick_number = return_joystick_number();
   if (kind_of() == REMOTE_PICTURE && joystick_number < 0) { // added joystick condition on 090699 so they get the right controlee
		// textual() restored on 220499 since this broke saving of some "hit who?" sensors
		// moved up here on 150499 so that language is dumped properly
      if (textual()) { // was text_value() == NULL) {
			dump_language = TRUE;
		   return((unsigned char) REMOTE_TEXT_PICTURE); // added 12/26/96
      } else {
		   return((unsigned char) REMOTE_PICTURE);
      };
	} else if (remote_status() == GLOBAL_REMOTE) { // even if picture
      if (joystick_number < 0) {
          if (user_media()) {
             return((unsigned char) GLOBAL_USER_REMOTE);
          } else if (hyperbot_remote()) {
             return((unsigned char) GLOBAL_HYPERBOT_REMOTE_INTEGER);
          } else {
		       return((unsigned char) GLOBAL_REMOTE_INTEGER);
          };
      } else {
          dump_language = TRUE; // for most of them -- just wastes a byte for others
          return((unsigned char) (GLOBAL_JOYSTICK0_REMOTE_INTEGER+joystick_number));
      };
	} else if (remote_status() != NOT_REMOTE) {// && !contained_in(THOUGHT_BUBBLE)) {
		return((unsigned char) REMOTE_INTEGER);
//	} else if (blank) {
//		stream.put((unsigned char) BLANK_INTEGER);
	};
   return((unsigned char) RATIONAL_NUMBER); // INTEGER); updated on 080202
};

void Number::set_to_future_value(Sprite *other, Sprite *original_recipient, Sprite *by) {
	// recompute if called by mouse since value may have changed if sensor
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("set_to_future_value of debug target (Rational)");
	};
	if (tt_debug_mode == 10399) {
		log("set_to_future_value");
	};
#endif
	if (other != NULL) {
		if (!is_blank()) {
			if (true_operation() != NO_NUMBER_OPERATION && !is_an_operation_with_no_value()) { 
				// new on 250202 since if have no value then this will give it one
				// is_an_operation_with_no_value new on 100202 
				if (other->kind_of() == INTEGER) {
					set_following_operation((Number *) other);
					return;
				} else { // new 091202 since can be indirection to number
					Sprite *number = other->dereference();
					if (number != NULL && number->kind_of() == INTEGER) {
						set_following_operation((Number *) number);
						return;
					};
				};				
			};
			if (other->is_an_operation_with_no_value()) {
				if (true_operation() == NO_NUMBER_OPERATION) {
					set_operation(other->true_operation());
				} else { // both are operations without values -- abort OK??
					by->abort_action(IDS_NUMBER_WONT_ACCEPT); 
				};
				return;
			};
		};
//		NumberValue *future_value = NULL; 
		// should maybe be renamed result_value -- the old instance variable future_value is now obsolete
//		boolean future_value_shared = FALSE;
		if (other->kind_of() == TEXT) { // new on 050803 to deal with more complex strings than just integers
			long others_text_length;
			string others_text_value;
			if (other->current_text(others_text_value,others_text_length)) {
				blank = FALSE;
				set_value_as_string(others_text_value,others_text_length);
//				delete [] others_text_value; 
				// commented out on 010505 since this has no business doing so since may be cached by "other"
				return;
			};
		};
		NumberValue *other_value;
		boolean other_value_shared;
		if (!other->current_value(other_value,other_value_shared)) {
			// if other is blank then abort -- new on 250202
			by->abort_action(IDS_NUMBER_WONT_ACCEPT); 
			return;
		};
		if (is_an_operation_with_no_value()) {
			if (other_value_shared) {
				// all of these calls updated on 231203 to make sure that original_recipient isn't NULL first
				set_value(other_value->copy(),FALSE,original_recipient?original_recipient->size_constrained():FALSE); 
				// last arg new on 031203
			} else {
				set_value(other_value,FALSE,original_recipient?original_recipient->size_constrained():FALSE); 
				// last arg new on 031203
			};
		} else if (blank) {
			blank = FALSE; // If I was blank I've become the other or picked up info from the other (e.g. cubby on blank integer)
//			if (!value->long_value(future_value)) { // deal with overflow -- 300102??
//				log("overflow");
//			};
			double double_float_value;
			if (other_value->double_float_value(double_float_value,FALSE)) {
				if (exact()) { // and I'm not an erased float
					if (!other_value_shared) delete other_value; // forgot the ! - fixed on 200202
					set_value(new RationalNumberValue(double_float_value),FALSE,
						                               original_recipient?original_recipient->size_constrained():FALSE); 
					// last arg new on 031203
					return;
//					future_value_shared = FALSE;
				};
			};
//			if (future_value == NULL) { // updated on 140202
//				future_value = other_value;
//				future_value_shared = other_value_shared;
//			};
			if (other_value_shared) {
				set_value(other_value->copy(),FALSE,original_recipient?original_recipient->size_constrained():FALSE); 
				// last arg new on 031203
			} else {
				set_value(other_value,FALSE,original_recipient?original_recipient->size_constrained():FALSE); 
				// last arg new on 031203
			};
		} else { // I think this branch is obsolete as of 140202 -- except for remote_integer instances and indirections to numbers
//			long my_value;
//			current_value(my_value); // doesn't need by arg since already checked earlier
//			if (!value.long_value(my_value)) { // deal with overflow -- 300102??
//				log("overflow");
//			};
			// bother with arithmetic exceptions?
//			NumberValue *result_value;
			NumberValue *my_value;
			NumberValue *new_value = NULL;
			boolean my_value_shared;
			// note that if the following as by as its third arg then robots that update sensors 
			// (but don't match them) will run once per cycle - noted 091203
			if (!current_value(my_value,my_value_shared)) { 
				// condition new on 280202 -- e.g. when this is a remote_integer can happen
				if (!other_value_shared) delete other_value;
				return; // warn??
			};
			ArithmeticException exception = result_of_operation(other,my_value,new_value); 
			// might this return value which is shared -- I don't think so
//			future_value_shared = FALSE; // since computed here
			if (!other_value_shared) delete other_value;
			if (!my_value_shared) delete my_value;
			if (exception == NO_EXCEPTION) {
				if (new_value != NULL) {
					set_value(new_value,FALSE,original_recipient?original_recipient->size_constrained():FALSE); 
					// last arg new on 031203
				}; // else warn? - can it happen??
			} else if (by != NULL) { // can happen when loading a Remote_Picture but then there shouldn't be exceptions
				switch (exception) {
//					case INTEGER_OVERFLOW:
//						say(IDS_ABORT_NUMBER_TOO_LARGE);
//						break;
//					case INTEGER_UNDERFLOW:
//						say(IDS_ABORT_NUMBER_TOO_SMALL);
//						break;
					case DIVIDE_BY_ZERO:
						by->abort_action(IDS_CANT_DIVIDE_BY_ZERO);
						break;
					case SOME_ARITHMETIC_EXCEPTION_ALREADY_REPORTED:
						// new on 070305
						by->abort_action(0,NONE_GIVEN,0); // 0 importance message
						break;
					default:
						by->abort_action(IDS_ABORT_UNKNOWN_ARITHMETIC_EXCEPTION); 
						// prior to 140202 was IDS_ABORT_NUMBER_TOO_SMALL_OR_BIG					
				}; // left value alone 
			};
		};
	}; // warn??
};
		// following was outside of the conditional prior to 130202
//		if (future_value == NULL) {
//			return; // and warn?? - new on 310102
//		};
//		if (future_value < 0 && operation == '-') { // commented out on 310102 since wrong modularity :set_value does this now
//			future_value = -future_value;
//			operation = '+';
//		};
//		if (future_value_shared) {
//			future_value = future_value->copy();
//		};
//		set_value(future_value); 
//	};
//	delete future_value; 
//	future_value = NULL; // new on 310201
//};

Sprite *Number::used(Sprite *subject, Sprite *by,
							boolean button_unchanged, boolean record_action,
							millisecond duration, boolean modified, Sprite *original_subject) {
   if (!puzzle_oks_flipping(by)) return(this);
//   if (modified && !tt_new_input_scheme) {
//      if (!puzzle_oks_typing_to_number_pads(by)) return(this);
//      // is this really a good idea??
//      set_value(value+1); // what about recording this???
//      completed(action_status);
//      return(this);
//   };
	if (edited) { // new on 100604
		canonicalize();
	};
	Picture *replacement = make_indirect_picture(this);
//	replacement->set_stopped(FALSE); // added on 101199 so flipped numbers can be tossed, etc.
   // this won't be recorded right will it??
	replacement->used(subject,by,button_unchanged,record_action,duration,modified,original_subject);
   user_did(INTEGER,INTEGERS_FLIPPED);
	return(replacement);
};

Sprite *Number::selected(SelectionReason reason, Sprite *by) {
	if (edited) { // new on 291004
		canonicalize();
	};
	return(Sprite::selected(reason,by));
};

void Number::java(java_stream &out) {
   if (blank) {
      out << S(IDS_JAVA_TT_BLANK_NUMBER);
   } else {
//		long long_value; // new on 310102 -- updated 030503
//		if (current_long_value(long_value)) {
			COLORREF color = tt_colors[text_color]; // new on 230700
			// added operation_to_string on 060302
			//string operation_string = operation_to_string(operation);
			//// for now use first letter - works for =, *, etc. but not sine, etc. -- fix soon
			//char operation_character;
			//if (operation_string == NULL || operation_string[0] == '\0') { // disjunction added 230702
			//	operation_character = '+'; // default
   //      } else if (operation_string[1] != NULL) { 
   //         tt_error_file() << "Java applet generator cannot yet handle the arithmetic operation: " << operation_string << ". Ignored." << endl;
   //         tt_err_file_important = TRUE;
   //         operation_character = '+';
			//} else {
			//	operation_character = operation_string[0];
			//};
			java_string value_string = NULL;
			long long_value; 
			if (!current_long_value(long_value,FALSE)) {
				value_string = java_value_string();
				if (value_string == NULL) {
					// shouldn't really happen
					value_string = copy_string("0");
				};
			};
			// stopped using IDS_JAVA_TT_INTEGER below and use "TTNumber" instead - 030503
			if (value_string == NULL) {
				java_new(out,"TTNumber",long_value,java_operation(operation),GetRValue(color),GetGValue(color),GetBValue(color));
			} else {
				java_new(out,"TTNumber",value_string,java_operation(operation),(int) GetRValue(color),(int) GetGValue(color),(int) GetBValue(color));
			};
			delete [] value_string;
//		}; // else warn?? - or try using bigger Java numbers - or at least 64-bit ones??
//      out << "new TTInteger(" << value;
//      if (operation != '+') { // not the default
//         out << ", '" << operation << "'";
//      };
//      out << ")";
   };
};

boolean Number::to_windows(Pointer *data, short int &data_offset,
                           ascii_string types, short int &types_offset,
                           BirdPointer *, short int &) {
   // updates data and types
	long long_value; // new on 310102
	if (current_long_value(long_value)) {
		types[types_offset] = 'L'; // token for long
		types_offset++;
		data[data_offset] = (Pointer) long_value; // lie so value is immediate
		data_offset++;
		return(TRUE);
	}; // else warn ??
	return(FALSE);
};

void Number::set_size_and_location(city_coordinate new_width, city_coordinate new_height,
											  city_coordinate new_llx, city_coordinate new_lly,
											  ChangeReason reason) {
	boolean size_changed = (width != new_width || height != new_height);
	Sprite::set_size_and_location(new_width,new_height,new_llx,new_lly,reason);
//	if (size_changed || reason == ANIMATION_COMPLETED) { // commented out on 110102
//		scale_to_fit();
//		if (showing_on_a_screen()) {
//			update_widths(TRUE);
//		};
//	};
	if ((size_changed && reason != LOADING_GEOMETRY && reason != NEW_TEXT_EXTENT) || // removed  && reason != TO_FIT_ON_A_NEST on 180903
		  reason == ANIMATION_STARTING || reason == ANIMATION_COMPLETED || reason == VACUUM_RESTORING) { // last disjunct new on 310702
		if (leader == NULL && !is_picture() && reason == NO_REASON_GIVEN) { // simplified on 180102 
//			 reason != LEADER_CHANGED && // leader might not yet be set...
//			 reason != TO_FIT_INSIDE && reason != SETTING_TO_GOOD_SIZE && 
//			 reason != UPDATING && reason != ANIMATION_STARTING && reason != ANIMATION_COMPLETED && 
//			 reason != EXPANDER_CHANGED_IT && reason != TRANSLATING_FLOOR_TO_ROOM_COORDINATES && reason != TRANSLATING_ROOM_TO_FLOOR_COORDINATES) { 
			// these added on 160102 
			set_change_size_to_fit_extent(TRUE);
		} else if (size_changed || reason == ANIMATION_COMPLETED) { // conditional new on 140803 - why do this work if size didn't change?
			// added reason == ANIMATION_COMPLETED on 150803 since sometimes needed
			set_change_font_size_to_fit_extent(TRUE);
			if ((decimal_places > 0 || number_format == TT_BIGNUM_DECIMAL) 
				 // second disjunct added on 040705 since, for example, could have been so small that only 0. fit and now there's room (issue 680)
#if TT_SHRINKING_AND_GROWING
				 && !display_with_size_unconstrained
#endif
				 ) { // might need a different number now
				// not if just running with display_with_size_unconstrained -- condition added 131104
				text_length = -1; // need to recompute wide_text and narrow_text
//				decimal_places = 0; // recompute these as well as of 080803 -- not needed
			};
#if TT_SHRINKING_AND_GROWING
			if ((size_changed && reason != ANIMATION_COMPLETED && reason != SETTING_TO_GOOD_SIZE) || (leader != NULL && completely_inside_leader())) {
				// reason != SETTING_TO_GOOD_SIZE added on 131104
				display_with_size_unconstrained = FALSE;
			};
#endif
		};
	};
};

void Number::describe_briefly(output_stream &message, Article article, boolean capitalize_first) {
	Sprite::describe_briefly(message,article,capitalize_first);
	if (!blank) {
		message << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
//		if (operation != '+') message << operation;
//		message << value;
		// re-written as follows to be more general
		print_value(message);
	};
};

void Number::print_value(output_stream &message) { // abstracted on 090202
	string text;
	long text_length;
	current_text(text,text_length);
	if (stop_generating_help(text_length)) {
		// new on 251104 to deal with numbers that are larger than Marty's maximum utterance
		if (stop_generating_help(20)) { // not even enough room to describe how many digits
			message << S(IDS_SAID_ENOUGH);
//			return(FALSE);
		} else {
			message << text_length << space() << S(IDS_DIGITS,"digits");
		};
	} else {
		message.write(text,text_length);
	};
};

void Number::generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection) {
//	History history = tt_histories[INTEGER]; // not kind_of() since inherited
	if (edited) {
		canonicalize(); // new on 281004 since otherwise refers to previous value
	};
	long long_value;
	if (blank) {
		// could say more if done this before
		if ((contained_in(THOUGHT_BUBBLE) && 
		     !did_user(INTEGERS_BLANKED_INSIDE_THOUGHT_BUBBLE,INTEGER)) ||
			 !did_user(INTEGERS_BLANK,INTEGER) ||
			 tt_help_counter == 1) {
//			 if (tt_help_counter == 0) {
				 describe_pointing(message,tool_in_hand,selection);
//			 };
		};
		if (tt_help_counter == 1) {
			message << S(IDS_INTS_BLANK_HELP);
		} else if ((!did_user(INTEGERS_CONVERSION,INTEGER) && tt_help_counter == 0) 
					  || tt_help_counter == 2) {
			tt_help_history_action = INTEGERS_CONVERSION;
			message << S(IDS_CONVERT_TO_NUMBER_HELP);
//		} else if (contained_in(THOUGHT_BUBBLE) &&
//					  !did_user(INTEGERS_BLANKED_INSIDE_THOUGHT_BUBBLE,INTEGER)) {
//			tt_help_history_action = INTEGERS_BLANKED_INSIDE_THOUGHT_BUBBLE;
//			message << S(IDS_INTS_BLANKED_TB_HELP);
//			user_did(INTEGER,INTEGERS_BLANKED_INSIDE_THOUGHT_BUBBLE);
		} else if (!did_user(INTEGERS_BLANK,INTEGER)) {
			tt_help_history_action = INTEGERS_BLANK;
			message << S(IDS_INTS_BLANK_HELP2);
		};
	} else if (operation != NO_NUMBER_OPERATION) { // new on 110202 to not give full help for operations since much of it doesn't apply
		// could add additional help at some point
		describe_pointing(message,tool_in_hand,selection);
	} else if ((!did_user(INTEGERS_ADDITION,INTEGER) && tt_help_counter == 0) 
				  || tt_help_counter == 1) {
		tt_help_history_action = INTEGERS_ADDITION;
//		if (tt_help_counter == 0) {
			describe_pointing(message,tool_in_hand,selection);
//		};
		message << S(IDS_INTS_ADDITION_HELP);
	} else if ((tool_in_hand == NULL || tool_in_hand == this) &&
				  ((!did_user(INTEGERS_TYPED,INTEGER) && tt_help_counter == 0)
					|| tt_help_counter == 2)) {
		tt_help_history_action = INTEGERS_TYPED;
		message << S(IDS_INTS_DIGITS_ADDED_HELP);
//		print_integer_with_commas(value,message);
//		message << S(IDS_INTS_DIGITS_ADDED_HELP2);
		if (tool_in_hand == NULL) {
			message << S(IDS_INTS_DIGITS_ADDED_HELP3);
//			you_are(message,FALSE);
//			message << S(IDS_INTS_DIGITS_ADDED_HELP4);
		};
	} else if (decimal_places == 0 && ((!did_user(INTEGERS_DIGITS_REMOVED,INTEGER) && tt_help_counter == 0) || tt_help_counter == 3)) {
		// can't edit shrinking digits so don't mention it -- new on 281004
		tt_help_history_action = INTEGERS_DIGITS_REMOVED;
		message << S(IDS_INTS_DIGITS_REMOVED_HELP)
		        << S(IDS_INTS_DIGITS_REMOVED_HELP2);
	} else if (sign() > 0 && ((!did_user(INTEGERS_MADE_NEGATIVE,INTEGER) && tt_help_counter == 0)|| tt_help_counter == 4)) {
//	            && value != 0) // commented out on 310102 since even if zero can give an example of making things negative
		// added sign() test on 281004 since it is silly to talk about dropping 2 to subtract by -2 and the like
//		long long_value;
//		if (!current_long_value(long_value)) {
//			long_value = 5; // if too big use a different example
//		};
		tt_help_history_action = INTEGERS_MADE_NEGATIVE;
		message << S(IDS_INTS_MADE_NEGATIVE_HELP);
//      if (long_value == 0) long_value = 5; // so the following makes sense...
//		print_integer_with_commas(long_value,message); 
		print_value(message); // replaces the above beginning 090202
		message << S(IDS_INTO);
//		print_integer_with_commas(-long_value,message);
		Number *temporary_copy = (Number *) copy(); 
		// new on 180305 so don't side effect it while describing it since that can change its font or width
		//negate();
		//print_value(message);
		// rewritten on 180305
		temporary_copy->negate();
		temporary_copy->print_value(message);
		message << S(IDS_INTS_MADE_NEGATIVE_HELP2)
				  << S(IDS_INTS_MADE_NEGATIVE_HELP3);
//		print_integer_with_commas(-long_value,message);
//		describe_briefly(message); 
		temporary_copy->print_value(message);
		message << S(IDS_INTS_MADE_NEGATIVE_HELP4,"");
//		negate(); //restore
		temporary_copy->destroy(); // rather than restore get rid of temporary copy -- 180305
//		print_integer_with_commas(long_value,message);
		print_value(message); 
		message << S(IDS_INTS_MADE_NEGATIVE_HELP5,""); // new on 080200
		message << end_sentence();
	} else if ((!did_user(INTEGERS_MULTIPLICATION,INTEGER) && tt_help_counter == 0)
				  || tt_help_counter == 5) {
//		long long_value;
//		if (!current_long_value(long_value)) {
//			long_value = 5; // if too big use a different example
//	};
		tt_help_history_action = INTEGERS_MULTIPLICATION;
		message << S(IDS_INTS_MULTIPLICATION_HELP);
//		print_integer_with_commas(long_value,message);
		print_value(message); 
		message << S(IDS_INTS_MULTIPLICATION_HELP2)
				  << S(IDS_INTS_MULTIPLICATION_HELP3);
//		print_integer_with_commas(long_value,message);
		print_value(message);
		message << S(IDS_INTS_MULTIPLICATION_HELP3_2,""); // new on 080200
		message << end_sentence() << S(IDS_INTS_MULTIPLICATION_HELP4);
	} else if ((sign() != 0 && (!did_user(INTEGERS_DIVISION,INTEGER) && tt_help_counter == 0) || tt_help_counter == 6)) {
//				  && value != 0) 	// commented out on 310102 since even if zero can give a different example	
		// restored updated version of zero test (sign() != 0) on 281004
//		long long_value;
//		if (!current_long_value(long_value)) {
//			long_value = 0; 
//		};
		tt_help_history_action = INTEGERS_DIVISION;
		message << S(IDS_INTS_DIVISION_HELP);
//		print_integer_with_commas(long_value,message);
		print_value(message);
		message << S(IDS_INTS_DIVISION_HELP2)
				  << S(IDS_INTS_DIVISION_HELP3);
#if !TT_MULTIPLE_PRECISION
		message << S(IDS_INTS_DIVISION_HELP4); // removed since no longer ignore remainder
#endif
		if (sign() == 0) {
			message << S(IDS_INTS_DIVISION_HELP5);
		};
		if (!did_user(INTEGERS_MODULO,INTEGER) && tt_help_counter == 0) {
			message << S(IDS_INTS_MODULO_HELP);
		};	
	} else if ((!did_user(INTEGERS_MODULO,INTEGER) && tt_help_counter == 0)
				  || tt_help_counter == 7) {	
//		long long_value;
//		if (!current_long_value(long_value)) {
//			long_value = 0; 
//		};
		if (is_integer() && sign() != 0) { // condition new since the following is just confusing with holding a non-integer or zero
			tt_help_history_action = INTEGERS_MODULO;
			message << S(IDS_INTS_MODULO_HELP3);
//			print_integer_with_commas(long_value,message);
			print_value(message);
			message << S(IDS_INTS_MODULO_HELP4)
					<< S(IDS_INTS_MODULO_HELP5);
			if (sign() == 0) { // long_value == 0) { // and is really zero?? - not just too big?? -- that's why is_zero was introduced
				message << S(IDS_INTS_DIVISION_HELP5);
			};
		};
	} else if ((!did_user(INTEGERS_ASSIGNMENT,INTEGER) && tt_help_counter == 0)
				  || tt_help_counter == 8) {
//		long long_value;
//		if (!current_long_value(long_value)) {
//			long_value = 2; 
//		};
		tt_help_history_action = INTEGERS_ASSIGNMENT;
		message << S(IDS_EQUAL_OPERATOR_ON_NUMBERS) << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
//		print_integer_with_commas(long_value,message);
		print_value(message);
		message << S(IDS_EQUAL_OPERATOR_ON_NUMBERS2,"") << end_sentence();
	} else if (((!did_user(INTEGERS_BITWISE_OPERATIONS,INTEGER) && tt_help_counter == 0)
					|| tt_help_counter == 9) && 
					current_long_value(long_value,FALSE)) { // added this check on 090202 so don't talk about binary if number doesn't have a long value
		tt_help_history_action = INTEGERS_BITWISE_OPERATIONS;
		message << S(IDS_INTS_BITWISE_HELP)
				  << S(IDS_INTS_BITWISE_HELP2)
				  << S(IDS_INTS_BITWISE_HELP3);
      print_binary_number(message,long_value,8);
		// too much trouble to print in binary with nice spacing if bignum (and what does it mean if rational?)
//		int saved_base = base;
//		base = 2;
//		print_value(message);
//		base = saved_base;
		message << end_sentence() 
				  << S(IDS_INTS_BITWISE_HELP4)
				  << S(IDS_INTS_BITWISE_HELP5) << space() // space() added on 150202 since IDS_INTS_BITWISE_HELP5 was updated
				  << S(IDS_INTS_BITWISE_HELP56);
	 } else if ((!did_user(INTEGERS_BLANK,INTEGER) && tt_help_counter == 0)
					|| tt_help_counter == 10) {
		tt_help_history_action = INTEGERS_BLANK;	
		message << S(IDS_VACUUM_TO_BLANK_NUMBER);
		// say more??
	 } else if ((!did_user(INTEGERS_FLIPPED,INTEGER) && tt_help_counter == 0)
					|| tt_help_counter == 11) {
		tt_help_history_action = INTEGERS_FLIPPED;
		message << S(IDS_IF_YOU_PRESS) << space();
		describe_right_button(message,tt_keyboard_accelerators[FLIP_THING_HELD]);
		message << space() << S(IDS_WHILE_HOLDING_A_NUMBER);
      message << S(IDS_PRESS_AGAIN_TO_FLIP_BACK);
	 } else if ((!did_user(TYPE_HELP_GIVEN,INTEGER) && tt_help_counter == 0)
					|| tt_help_counter == 12) {
//		tt_help_history_action = TYPE_HELP_GIVEN;
		message << S(IDS_NUMBER_HELP_GIVEN_HELP)
				  << S(IDS_IDS_NUMBER_HELP_GIVEN_HELP2);
		no_help_interruptions();
		user_did(INTEGER,TYPE_HELP_GIVEN);
	 };
};

boolean Number::dump(output_stream &stream, boolean just_prepare, boolean , boolean ) {
   if (just_prepare) return(FALSE);
   int dump_index = about_to_dump(this,stream); // rewritten on 020802
	if (dump_index < 0) return(FALSE); // new on 051099
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
		print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
		tt_error_file() << "About to dump ";
		print(tt_error_file());
		tt_error_file() << endl;
		tt_debug_dump_depth++;
	};
#endif
   boolean dump_language = FALSE;
	unsigned char dump_type = number_dump_type(dump_language);
   stream.put(dump_type);
	dump_if_not_null(stream,comment);
	dump_value(stream); // default if blank
	stream.put((unsigned char) operation); // never more than 256 operations??
//	stream.put((unsigned char) blank);
	dump_common_flags(stream); // on 080400 this replaces the above
	stream.put((unsigned char) text_color); // new on 100100
   if (dump_language) {
		stream.put((unsigned char) natural_language());
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
		print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
		tt_error_file() << "Finished dumping ";
		print(tt_error_file());
		tt_error_file() << endl;
		tt_debug_dump_depth--;
	};
#endif
	return(TRUE);
};

void Number::update_narrow_text(long long_value, string narrow_text, int narrow_text_length) { // abstracted on 010202
//	output_string_stream text_stream(narrow_text,narrow_text_length); // was 14 prior to 110102 -- not good for operator with negative billion
	// rewritten on 260104
	int written_so_far = 0;
	if (operation != NO_NUMBER_OPERATION) { // was '+' prior to 100202
//		text_stream << operation_to_string(operation);
		string operation_string = operation_to_string(operation);
		written_so_far += strlen(operation_string);
		memcpy(narrow_text,operation_string,written_so_far+1); // +1 to include terminator
		if (unary_operation(operation)) {
//			text_length = text_stream.LENGTH;
			text_length = written_so_far;
			return;
		};
	};
	//if (return_punctuate_every_three_digits()) { // if not obsolete then needs to deal with non decimal radix -- removed on 191202
	//	print_integer_with_commas(long_value,text_stream);
	//} else { 
//		text_stream << long_value; // new on 050202 so longs are consistent with bignums
		char long_value_string[35]; // binary needs 32 bits and radix indicator -- re-writen on 270202 to deal with radix
		int offset = 0;
		int base = return_base();
		if (base != 10) {
			itoa(base,long_value_string,10);
			offset = strlen(long_value_string);
			long_value_string[offset++] = '#'; // radix indicator
		};
		if (long_value < 0) { 
			long_value_string[offset++] = '-';
			long_value = -long_value;
		};
		itoa(long_value,long_value_string+offset,base);
//		text_stream << long_value_string;
		int long_value_string_length = strlen(long_value_string);
		memcpy(narrow_text+written_so_far,long_value_string,long_value_string_length+1); // +1 to copy terminator too
		written_so_far += long_value_string_length;
//	};
	int old_text_length = text_length;
//	text_length = text_stream.LENGTH;
	text_length = written_so_far;
	if (text_length < min_text_length && decimal_places == 0) { // second condition added on 110803 so if shrinking digits don't do this
		int gap = min_text_length-text_length;
      int i;
		for (i = text_length-1; i >= 0; i--) {
			narrow_text[i+gap] = narrow_text[i];
		};
		for (i = 0; i < gap; i++) {
			narrow_text[i] = ' '; // prepend blanks
		};
		text_length += gap;
		narrow_text[min_text_length] = '\0'; // terminate string
	} else {
//		text_stream.put('\0'); // terminate it
		if (old_text_length != text_length && (!change_size_to_fit_extent() || tt_log_version_number < 57))  { // new on 110102
			// added !change_size_to_fit_extent() on 241004 since if going to change size don't change font size too
			// commented out on 070302 so that only font size not number pad size changes here
//			if (size_constrained() || is_picture()) { // is_picture added on 160102
//			if (!tt_shrink_and_grow_numbers) {
				set_change_font_size_to_fit_extent(TRUE);
//			};
//			} else {
//				change_size_to_fit_extent_flag = TRUE;
//			};
		};
	};
};

void Number::need_to_recompute_text(boolean sizes_constrained) {
//	if (regardless || tt_log_version_number < 45) { // new policy on 080604 to allow intermediate results be non-numeric while held
		text_length = -1; // need to recompute wide_text and narrow_text
//	} else {
//		edited = TRUE;
//	};
   if (sizes_constrained || size_constrained()) { // removed on 281002 || is_picture()) { // moved here on 060202 from current_wide_text
//		if (!tt_shrink_and_grow_numbers) {
			set_change_font_size_to_fit_extent(TRUE);
//		};
	} else {
		set_change_size_to_fit_extent(TRUE);
#if TT_SHRINKING_AND_GROWING
		display_with_size_unconstrained = TRUE; // new on 091104
#endif
	};
};

void Number::set_following_operation(Number *operation) {
	if (following_operation == NULL) {
		if (operation->true_operation() == NO_NUMBER_OPERATION) { // implicitly is addition or subtraction
			if (operation->sign() < 0) {
				operation->set_operation(SUBTRACT_FROM);
				operation->negate();
			} else {
				operation->set_operation(INCREASE_BY);
			};
		};
		operation->remove_from_floor(FALSE);
		following_operation = operation;
		operation->increment_ref_count(); // new on 091202
	} else {
		following_operation->set_following_operation(operation);
	};
	need_to_recompute_text();
};

int Number::add_operations_to_string(string text) {
	string operation_string = operation_to_string(operation);
	if (operation_string != NULL && operation_string[0] != '\0') { // second conjunct added 230702
		int operation_string_length = strlen(operation_string);
		memcpy(text,operation_string,operation_string_length);
		return(operation_string_length);
	} else {
		return(0);
	};
};

int Number::operation_string_length() {
	string operation_string = operation_to_string(operation);
	if (operation_string != NULL) {
		return(strlen(operation_string));
	} else {
		return(0);
	};
};

int Number::following_operations_string_length() {
	if (following_operation == NULL) return(0);
	string ignore_text;
	long following_length;
	if (following_operation->current_text(ignore_text,following_length)) {
		return(following_length);
	} else {
		return(0);
	};
};

void Number::set_color(color_index new_color, boolean updating_default) { // updating_default added 130202
   text_color = new_color;
	if (!updating_default) using_default_text_color = FALSE;
};

Remote_Integer *new_global_remote(Picture *controlee, RemoteIdentifier identifier, long value, 
											 NumberOperation operation, NaturalLanguage natural_language,
											 city_coordinate initial_width, city_coordinate initial_height) {
	// this is a confusing name since it seems to work fine with non-global remotes as well
   if (remote_address(identifier)) {
		return(new Remote_Address(controlee,identifier,0,0,0,initial_width,initial_height,value));
   } else if (remote_picture(identifier)) {
		return(new Remote_Picture(controlee,identifier,0,0,0,initial_width,initial_height,value,natural_language));
   } else {
		return(new Remote_Integer(controlee,identifier,0,0,0,initial_width,initial_height,(value==min_long)?NULL:new LongNumberValue(value),TRUE,
										  operation));
   };
};

boolean text_identifier(RemoteIdentifier identifier) {
   return(identifier != FILE_TO_SOUND_REMOTE &&
          identifier != FILE_TO_PICTURE_REMOTE &&
          identifier != CLIPBOARD_REMOTE &&
          identifier != TEXT_TO_MCI_REMOTE &&
          identifier != TEXT_TO_SPEECH_REMOTE &&
          identifier != ROOM_DECORATION_REMOTE &&
          identifier != HOUSE_DECORATION_REMOTE &&
          identifier != ROOF_DECORATION_REMOTE &&
			 identifier != COLLIDEE_REMOTE &&
			 identifier != APPEARANCE_REMOTE &&
			 identifier != CHILDREN_REMOTE &&
			 identifier != ANCESTORS_REMOTE &&
			 identifier != HIT_OR_MISS_REMOTE && // last 3 were missing and added on 140599
			 identifier != X_HIT_OR_MISS_REMOTE &&
			 identifier != Y_HIT_OR_MISS_REMOTE);
};

boolean remote_picture(RemoteIdentifier identifier) {
	switch (identifier) {
		case HIT_OR_MISS_REMOTE:
		case X_HIT_OR_MISS_REMOTE:
		case Y_HIT_OR_MISS_REMOTE:
		case COLLIDEE_REMOTE:
		case CHILDREN_REMOTE:
		case ANCESTORS_REMOTE:
		case VISIBLE_REMOTE:
		case END_OF_ANIMATION_REMOTE:
		case GRASPED_REMOTE:
		case SELECTED_REMOTE:		
		case RELEASED_REMOTE:
		case MOUSE_LEFT_BUTTON_REMOTE:
		case MOUSE_MIDDLE_BUTTON_REMOTE:
		case MOUSE_RIGHT_BUTTON_REMOTE:
		case MOUSE_LEFT_BUTTON_DOWN_REMOTE:
		case MOUSE_MIDDLE_BUTTON_DOWN_REMOTE:
		case MOUSE_RIGHT_BUTTON_DOWN_REMOTE:
		case LAST_KEYSTROKE_REMOTE: 
		case KEYBOARD_BUTTON_REMOTE:
		case SHIFT_DOWN_REMOTE:
		case CONTROL_DOWN_REMOTE:
		case HAND_VISIBLE_REMOTE:
		case SOUND_ON_REMOTE:
		case PROGRAMMER_HEAD_REMOTE:
		case MOUSE_BUTTONS_REMOTE:
		case SCREEN_SIZE_REMOTE:
		case CLIPBOARD_REMOTE:
      case FILE_TO_PICTURE_REMOTE:
      case FILE_TO_SOUND_REMOTE:
      case TEXT_TO_MCI_REMOTE:
      case TEXT_TO_SPEECH_REMOTE:
      case MARTIAN_SPEAK_REMOTE:
      case ROOM_DECORATION_REMOTE:
      case HOUSE_DECORATION_REMOTE:
      case ROOF_DECORATION_REMOTE:
		case LANGUAGE_REMOTE:
			return(TRUE);
		default:
			return(identifier >= JOYSTICK_BUTTON1_REMOTE && identifier <= JOYSTICK_BUTTON32_DOWN_REMOTE);
	};
};

boolean inactive_after_changed(RemoteIdentifier identifier) {
	switch (identifier) {
		// essentially these are ones that aren't bi-directional
		case GRASPED_REMOTE:
		case SELECTED_REMOTE:		
		case RELEASED_REMOTE:
		case MOUSE_LEFT_BUTTON_REMOTE:
		case MOUSE_MIDDLE_BUTTON_REMOTE:
		case MOUSE_RIGHT_BUTTON_REMOTE:
		case MOUSE_LEFT_BUTTON_DOWN_REMOTE:
		case MOUSE_MIDDLE_BUTTON_DOWN_REMOTE:
		case MOUSE_RIGHT_BUTTON_DOWN_REMOTE:
		case LAST_KEYSTROKE_REMOTE: // make sense to change this??
		case KEYBOARD_BUTTON_REMOTE: // make sense to change this??
		case SHIFT_DOWN_REMOTE:
		case CONTROL_DOWN_REMOTE:
//		case SCREEN_SIZE_REMOTE: // commented out on 100602
		case STREET_REMOTE:
		case AVENUE_REMOTE:
		case END_OF_ANIMATION_REMOTE:
//		case CITY_SIZE_REMOTE: // commented out on 100602
			return(TRUE);
		default:
			// can't change joystick buttons either
			return(identifier >= JOYSTICK_BUTTON1_REMOTE && identifier <= JOYSTICK_BUTTON32_DOWN_REMOTE);
	};
};

boolean global_remote_picture(RemoteIdentifier identifier) {
	switch (identifier) {
		case MOUSE_LEFT_BUTTON_REMOTE:
		case MOUSE_MIDDLE_BUTTON_REMOTE:
		case MOUSE_RIGHT_BUTTON_REMOTE:
		case MOUSE_LEFT_BUTTON_DOWN_REMOTE:
		case MOUSE_MIDDLE_BUTTON_DOWN_REMOTE:
		case MOUSE_RIGHT_BUTTON_DOWN_REMOTE:
		case LAST_KEYSTROKE_REMOTE: 
		case KEYBOARD_BUTTON_REMOTE:
		case SHIFT_DOWN_REMOTE:
		case CONTROL_DOWN_REMOTE:
		case HAND_VISIBLE_REMOTE:
		case SOUND_ON_REMOTE:
		case PROGRAMMER_HEAD_REMOTE:
		case MOUSE_BUTTONS_REMOTE:
		case SCREEN_SIZE_REMOTE:
		case CLIPBOARD_REMOTE:
      case FILE_TO_PICTURE_REMOTE:
      case FILE_TO_SOUND_REMOTE:
      case TEXT_TO_MCI_REMOTE:
      case TEXT_TO_SPEECH_REMOTE:
      case MARTIAN_SPEAK_REMOTE:
      case ROOM_DECORATION_REMOTE:
      case HOUSE_DECORATION_REMOTE:
      case ROOF_DECORATION_REMOTE:
		case AVENUE_REMOTE:
		case STREET_REMOTE:
		case LANGUAGE_REMOTE:
			return(TRUE);
		default:
			return(identifier >= JOYSTICK_BUTTON1_REMOTE && identifier <= JOYSTICK_BUTTON32_DOWN_REMOTE);
	};
};

boolean remote_address(RemoteIdentifier identifier) {
	return(identifier == STREET_REMOTE || identifier == AVENUE_REMOTE);
};

boolean remote_integer_whose_value_should_not_be_dumped(RemoteIdentifier identifier) {
	switch (identifier) {
		case STREET_REMOTE:
		case AVENUE_REMOTE:
		case HIT_OR_MISS_REMOTE:
		case X_HIT_OR_MISS_REMOTE:
		case Y_HIT_OR_MISS_REMOTE:
		case COLLIDEE_REMOTE:
		case CHILDREN_REMOTE:
		case ANCESTORS_REMOTE:
//		case VISIBLE_REMOTE: -- 
		// commented out on 221101 since this meant that dropping the visibility remote on the back didn't change the picture
		case END_OF_ANIMATION_REMOTE:
		case GRASPED_REMOTE:
		case SELECTED_REMOTE:		
		case RELEASED_REMOTE:
		case MOUSE_LEFT_BUTTON_REMOTE:
		case MOUSE_MIDDLE_BUTTON_REMOTE:
		case MOUSE_RIGHT_BUTTON_REMOTE:
		case MOUSE_LEFT_BUTTON_DOWN_REMOTE:
		case MOUSE_MIDDLE_BUTTON_DOWN_REMOTE:
		case MOUSE_RIGHT_BUTTON_DOWN_REMOTE:
		case LAST_KEYSTROKE_REMOTE: 
		case KEYBOARD_BUTTON_REMOTE:
		case SHIFT_DOWN_REMOTE:
		case CONTROL_DOWN_REMOTE:
			// following added on 090200 since they are just default values that should not override 'Set Options' for example
		case SOUND_ON_REMOTE:
		case LETTER_SIZE_REMOTE:
		case READING_SPEED_REMOTE:
		case MARTIAN_ADVICE_LEVEL_REMOTE:
		case PROGRAMMER_HEAD_REMOTE:
		case SCREEN_SIZE_REMOTE:
		case SYSTEM_SPEED_REMOTE:
		case CITY_SIZE_REMOTE:
		case SYSTEM_SLEEP_REMOTE:
		case MOUSE_BUTTONS_REMOTE:
		case LEGO_SERIAL_PORT_REMOTE:
		case MARTIAN_FONT_REMOTE:
		case MARTIAN_SPEAK_REMOTE:
		case LANGUAGE_REMOTE:
			return(TRUE);
		default:
			return(FALSE);
	};
};

void active_and_identifier(unsigned char c, boolean &active, RemoteIdentifier &identifier) {
	active = !(c>>7);
	identifier = (RemoteIdentifier) (c&~(1<<7));
};

int nested_comments = 0;

Number *load_number(SpriteType type, InputStream *pad_in,
						  int notebook_version, NaturalLanguage language,
                    Sprite *container, Picture *controlee, long index, boolean descendants_inactive) {
	Sprite *comment = NULL;
	if (notebook_version >= first_version_to_save_copied_nests) {
		if (nested_comments > 3) { // new on 140200
			// to avoid overflow when end of stream reached since INTEGER is code 0
 			log("ToonTalk had problems re-creating a saved object.");
//			tt_error_file() << "TellG is " << (int) pad_in->tellg() << endl;
			nested_comments = 0;
			throw(new Rational(0L));
		};
		nested_comments++; 
		comment = load_item(pad_in,notebook_version,language);
		nested_comments--;
	};
	long long_value = -1; // -1 is a convention used by some to mean re-compute value
//	double double_float_value;
	Sprite *following_operation = NULL;
//	string text_value = NULL;
//	long text_length;
	NumberValue *number_value = NULL; // new on 050302
	int base = 10;
	byte format_flags = 0;
	long dumped_value = 0; // moved here on 020302
	NumberFormat number_format = TT_LONG;
	if (notebook_version >= first_version_to_save_rational_numbers) { 
		// type == RATIONAL_NUMBER) { // old INTEGER and REMOTE_INTEGER and the like don't do this
		// updated on 020302 since everything works this way now
		number_format = (NumberFormat) pad_in->get();
#if TT_DEBUG_ON
		if (number_format >= TT_UNDETERMINED_NUMBER_FORMAT) {
			log("Loading a number that wasn't stored correctly.");
			throw(tt_global_picture); // new on 130103 for a softer lander (Leonel's students encountered this)
		};
#endif
	};
	if (number_format == TT_LONG) {
		pad_in->read((string) &long_value, sizeof(long_value));
		dumped_value = long_value; // rewritten o 020302
//	} else if (number_format == TT_DOUBLE_FLOAT) { // done generically below as of 050302
//		pad_in->read((string) &double_float_value, sizeof(double_float_value)); 
//		following_operation = load_item(pad_in,notebook_version,language,NULL,controlee,index);
	} else if (number_format == TT_COMPUTE_VALUE_WHEN_LOADED) { // new on 020302
		// do nothing here
	} else {	
//		pad_in->read((string) &text_length, sizeof(text_length));
//		text_value = new char[text_length+1];
//		pad_in->read(text_value,text_length+1); // include terminator
		number_value = load_number_value(pad_in,notebook_version);
		if (number_format != TT_DOUBLE_FLOAT) {
			format_flags = pad_in->get();
			base = pad_in->get();
			if (base == 0) { // probably reached end of encoding
				throw(tt_global_picture); // new on 130103 for a softer lander (Leonel's students encountered this)
			};
		};
		following_operation = load_item(pad_in,notebook_version,language,NULL,controlee,index);
	};
	// should distinguish short and long numbers -- huh???
	Number *number = NULL;
	Remote_Integer *remote_number = NULL;
	unsigned char operation_key = pad_in->get();
	NumberOperation operation;
	if (notebook_version >= first_version_to_save_rational_numbers) { // was type == RATIONAL_NUMBER prior to 020302
		operation = (NumberOperation) operation_key;
	} else {
		operation = operation_from_key(operation_key);
	};
	boolean blank = FALSE;
	boolean infinite_stack = FALSE; // new on 080400
//	if (notebook_version > 2) { // anything that old is before first release so don't bother.
//		blank = (boolean) pad_in->get();
		// replaced above with below on 080400
		load_common_flags(pad_in,blank,infinite_stack);
//	};
	color_index color = tt_black;
	if (notebook_version >= first_version_to_save_number_colors) { // new on 100100
		color = pad_in->get();
	};
	RemoteIdentifier identifier;
	boolean active = TRUE; // by default
	switch (type) {
      case GLOBAL_USER_REMOTE:
		case GLOBAL_REMOTE_INTEGER:
			// depending upon context make this be alive or not
			// e.g. in notebook not alive -- on flip side of picture is
//			identifier = (RemoteIdentifier) pad_in->get();
			active_and_identifier(pad_in->get(),active,identifier);
         if (active && descendants_inactive) { // new on 220702 to enforce the idea that if the parent is inactive then so must I be
            active = FALSE;
         };
         if (notebook_version <= version_number_just_before_joysticks+1) {  // +1 fixes things like p48.pzl -- unclear what went wrong
            // globals are not contiguous so some moved around
            switch (identifier) {
               case OLD_MOUSE_SPEED_X_REMOTE:
                  identifier = MOUSE_SPEED_X_REMOTE;
                  break;
               case OLD_MOUSE_SPEED_Y_REMOTE:
                  identifier = MOUSE_SPEED_Y_REMOTE;
                  break;
               case OLD_CLOCK_REMOTE:
                  identifier = CLOCK_REMOTE;
                  break;
               case OLD_RANDOM_REMOTE:
                  identifier = RANDOM_REMOTE;
                  break;
            };
         };
		   controlee = tt_global_picture;
         if (!remote_address(identifier)) {
				long_value = controlee->current_user_value(identifier);
			};
         remote_number = new_global_remote(controlee,identifier,long_value,operation,language,0,0);
         if (type == GLOBAL_USER_REMOTE) { // then a text pad is following this
            Sprite *text_pad = load_item(pad_in,notebook_version,language,controlee);
            if (text_pad != NULL) remote_number->set_to_future_value(text_pad,NULL,NULL); // second NULL is for "by" - so be careful
         };
			break;
      case FORCE_REMOTE_INTEGER:
//			active_and_identifier(pad_in->get(),active,identifier); // commented out on 140799
			// since they aren't made inactive and they need the 8th bit
         identifier = (RemoteIdentifier) pad_in->get();
			active = TRUE; // why?
         controlee = (Picture *) load_item(pad_in,notebook_version,language,NULL,NULL,-1,TRUE); 
			// why load a controlee for forces?? -- args added on 030302 so it knows this is a controlee
			if (controlee != NULL && controlee->kind_of() != PICTURE) { // new on 141200 for robustness
				log("Warning: something wrong with things saved in a file, the clipboard, a notebook, or a message given to a long-distance bird. Sorry.",FALSE,TRUE);
				throw(tt_global_picture);
			};
         if (controlee != NULL) { // && number_format == TT_COMPUTE_VALUE_WHEN_LOADED ??
				long_value = controlee->current_user_value(identifier);
			};
			remote_number = new Force_Remote_Integer(controlee,identifier,
														        0,0,0,0,0, 
																  //  TRUE was missing on 060302
																  new LongNumberValue(dumped_value),TRUE,operation); 
			// replaced long_value with dumped_value on 060302 since that's what works
			break;
		case REMOTE_INTEGER:
			active_and_identifier(pad_in->get(),active,identifier);
         if (active && descendants_inactive) { // new on 220702 to enforce the idea that if the parent is inactive then so must I be
            active = FALSE;
         };
//			identifier = (RemoteIdentifier) pad_in->get();
			if (identifier >= JOYSTICK_SPEED_X_REMOTE && identifier <= JOYSTICK_AXIS_ROTATION_Z_REMOTE) { 
				// new on 060200 to fix badly saved joystick button remotes
				controlee = tt_joystick_pictures[0];
			} 
         if (controlee != NULL) { // && number_format == TT_COMPUTE_VALUE_WHEN_LOADED ??
				if (speed_identifier(identifier)) { // new on 050302
					remote_number = new Remote_Integer(controlee,identifier,
																  0,0,0,0,0,
																  controlee->current_speed(identifier,TRUE),active,operation);
				} else if (double_float_identifier(identifier)) { // no longer longs as of 030302 
					remote_number = new Remote_Integer(controlee,identifier,
																  0,0,0,0,0,
																  new RationalNumberValue(controlee->current_user_value_double(identifier)),active,operation); 
					// replaced TRUE by active on 081299
				} else {
					long_value = controlee->current_user_value(identifier);
					remote_number = new Remote_Integer(controlee,identifier,
																  0,0,0,0,0,
																  new LongNumberValue(long_value),active,operation);
				};
				delete number_value;
			} else {
            if (number_value == NULL) { // new on 180702 since if saved inactive or blank then the dumped value matters
               number_value = new LongNumberValue(dumped_value);
            };
				remote_number = new Remote_Integer(controlee,identifier,
															  0,0,0,0,0,
															  number_value,active,operation);
			};
			break;
      case REMOTE_TEXT_PICTURE:
         language = (NaturalLanguage) pad_in->get();
         // and the following
		case REMOTE_PICTURE:
			active_and_identifier(pad_in->get(),active,identifier);
         if (active && descendants_inactive) { 
				// new on 220702 to enforce the idea that if the parent is inactive then so must I be
            active = FALSE;
         };
//			identifier = (RemoteIdentifier) pad_in->get();
         if (notebook_version <= version_number_just_before_joysticks+1) { 
				// +1 fixes things like p48.pzl -- unclear what went wrong
            // globals are not contiguous so some moved around
            switch (identifier) {
               case OLD_END_OF_ANIMATION_REMOTE:
                  identifier = END_OF_ANIMATION_REMOTE;
                  break;
               case OLD_GRASPED_REMOTE:
                  identifier = GRASPED_REMOTE;
                  break;
               case OLD_SELECTED_REMOTE:
                  identifier = SELECTED_REMOTE;
                  break;
               case OLD_RELEASED_REMOTE:
                  identifier = RELEASED_REMOTE;
                  break;
            };
         };
			if (identifier >= JOYSTICK_BUTTON1_REMOTE && identifier <= JOYSTICK_BUTTON32_DOWN_REMOTE) { 
				// new on 020200 to fix badly saved joystick button remotes
				controlee = tt_joystick_pictures[0];
            if (controlee != NULL) { // might be fewer joysticks on this system
               long_value = controlee->current_user_value(identifier);
            } else { // new on 300799 since can't happen anymore
					log("Problem loading a joystick remote control");
				};
            remote_number = new_global_remote(controlee,identifier,long_value,operation,language,0,0);
				log("Troubles loading a joystick button remote control - problem hopefully is repaired.");
				break;
			};
			if (//type == REMOTE_TEXT_PICTURE && // commented out on 260499 so things like decoration remotes load right
				 global_remote_picture(identifier)) {
				// added on 150499 so that global yes/no sensors save with language correctly
				controlee = tt_global_picture;
			};
         if (controlee != NULL && identifier != STREET_REMOTE && identifier != AVENUE_REMOTE && 
				 number_format == TT_COMPUTE_VALUE_WHEN_LOADED) {
				// is number_format == TT_COMPUTE_VALUE_WHEN_LOADED OK ??
				// added 190499 remote address conditions
			   // so it is calulated at the appropriate moment
				long_value = controlee->current_user_value(identifier);
            dumped_value = long_value; // new on 010802 so options (like how Marty speaks) don't use dumped value
			};
			// following added on 160499
			remote_number = new_global_remote(controlee,identifier,long_value,operation,language,4*tile_width,0);
			// added following on 130599 since sometimes is reset by initialization but not appropriate if dumped with a value
			if (dumped_value < 0) { // changed on 090699 so that negative values are interpreted as compute new value
				remote_number->set_long_value(long_value);
			} else {
				remote_number->set_long_value(dumped_value);
			};
			break;
      case GLOBAL_HYPERBOT_REMOTE_INTEGER: // obsolete
			active_and_identifier(pad_in->get(),active,identifier);
         if (active && descendants_inactive) { 
				// new on 220702 to enforce the idea that if the parent is inactive then so must I be
            active = FALSE;
         };
//         identifier = (RemoteIdentifier) pad_in->get();
//#if !TT_32
			controlee = tt_hyperbot_picture;
//#endif
         if (controlee != NULL) {
            long_value = controlee->current_user_value(identifier);
         };
         remote_number = new_global_remote(controlee,identifier,long_value,operation,language,0,0);
			break;
		default:
         if (type >= GLOBAL_JOYSTICK0_REMOTE_INTEGER && type <= GLOBAL_JOYSTICK31_REMOTE_INTEGER) {
            language = (NaturalLanguage) pad_in->get();
				if (notebook_version <= first_version_to_save_rational_numbers) { 
					// made conditional on 270801 -- was <= 21 prior to 010302 -- fix this with next file format
					active_and_identifier(pad_in->get(),active,identifier); 
					// restored on 100400 since only FORCE joysticks need ignore active flag
					// commented out the above and restored below on 150801 since joystick_button_down (27 to 32) 
					// go over 128 so were broken
               if (active && descendants_inactive) { 
						// new on 220702 to enforce the idea that if the parent is inactive then so must I be
                  active = FALSE;
               };
            } else {
					identifier = (RemoteIdentifier) pad_in->get();
					active = TRUE; // new on 140799 (see comment above)
				};
				if (pad_in->empty()) { // new on 290803 since happens on page 16 of the notebook on page 4 of Playground
					throw(tt_global_picture);
				};
//#if TT_DIRECT_INPUT // commented out on 300799 so can still load and save joystick remotes
//				if (tt_joystick_pictures != NULL) { // commented out on 300799
					// probably no joystick connected now but loading something that relied upon joysticks so
				if (type-GLOBAL_JOYSTICK0_REMOTE_INTEGER < max_number_of_joysticks) { 
					// on 310799 changed tt_joystick_count to max_number_of_joysticks
					controlee = tt_joystick_pictures[type-GLOBAL_JOYSTICK0_REMOTE_INTEGER];
				};
//#endif
            if (controlee != NULL) { // might be fewer joysticks on this system
               long_value = controlee->current_user_value(identifier);
            } else { // new on 300799 since can't happen anymore
					log("Problem loading a joystick remote control");
				};
            remote_number = new_global_remote(controlee,identifier,long_value,operation,language,0,0);
			} else {
				if (number_format == TT_LONG) {
					if (notebook_version < first_version_to_save_rational_numbers && type == INTEGER && operation == SUBTRACT_FROM) {
						// beginning with first_version_to_save_rational_numbers not saved this way unless really mean it 
						operation = NO_NUMBER_OPERATION; // new on 200202 since old negative numbers are not subtract_from operations
						long_value = -long_value;
					};
					number = new Rational(0,0,0,3*tile_width,0,new LongNumberValue(long_value),number_format,operation); 
					// added number_format on 020702
				} else {
					if (number_format == TT_DOUBLE_FLOAT) {
						number = new Rational(0,0,0,3*tile_width,0,number_value,number_format,operation); 
						// added number_format on 020702
					} else {
						number = new Rational(0,0,0,3*tile_width,0,number_value,number_format,format_flags,base,operation);
					};
					if (following_operation != NULL && following_operation->kind_of() == INTEGER) { // shouldn't be anything else
						number->set_following_operation((Number *) following_operation);
					};
				};
         };
	};
	if (remote_number != NULL) {
		number = remote_number; // moved here on 050499 so comments on remote numbers work OK
		// new on 220499 so inactivity is saved correctly
		if (remote_address(identifier)) {
			remote_number->set_active(dumped_value < 0); // is this obsolete?? - doesn't it get clobbered below??
		} else { // commented out on 230499 since this makes valid remotes inactive
			remote_number->set_active(dumped_value == min_long); 
		};
		remote_number->set_active(active);
	};
	if (number == NULL) return(NULL);
	associate_shared_item(index,number);
   if (container != NULL) {  // new on 160702
      if (container->kind_of() == PROGRAM_PAD) {
         number->set_in_notebook((Notebook *) container,FALSE); // no point recuring
      } else if (container->kind_of() == VACUUM) {
         number->set_inside_vacuum(TRUE,FALSE);
      };
   }; 
	if (comment != NULL) number->set_comment(comment);
	if (type == REMOTE_TEXT_PICTURE && controlee != NULL) {
		// starting 150499 ignoring the saved controlee if it is global
		// it is hard to avoid this but now the ignored controlee is just the NOTHING_MARKER when global
		if (!ignore_next_item(pad_in,notebook_version,language)) {
			return(FALSE); // new on 120101
		};
   } else if ((type == REMOTE_TEXT_PICTURE || type == REMOTE_INTEGER || type == REMOTE_PICTURE) &&
				  notebook_version >= first_version_to_save_controlees) {
      // have read all the Integer stuff, so now the controlee
      Sprite *new_controlee = load_item(pad_in,notebook_version,language,NULL,NULL,-1,TRUE); 
		// TRUE added on 290699 since pictures load differently if they are a controlee
      if (new_controlee != NULL && new_controlee->kind_of() == PICTURE &&
			// following added on 060200 to avoid reseting the controlee if joystick remote
			 (identifier < JOYSTICK_SPEED_X_REMOTE || identifier > JOYSTICK_BUTTON32_DOWN_REMOTE)) {
			controlee = (Picture *) new_controlee;
			remote_number->set_controlee(controlee);
			if (speed_identifier(identifier)) { // new on 050302
				remote_number->set_value(controlee->current_speed(identifier,TRUE));
			} else if (double_float_identifier(identifier)) { 
				remote_number->set_double_value(controlee->current_user_value_double(identifier));
			} else {
				remote_number->set_long_value(controlee->current_user_value(identifier)); // added 241099
			};
			remote_number->update_picture(TRUE); // added on 300699 so it is up to date
		}; // else could produce a warning
   };
	if (remote_number != NULL) {
      remote_number->set_language(language);
		if (remote_number->read_only()) { // || notebook_version == 2) {
			remote_number->set_operations(load_operations(pad_in,notebook_version,language,controlee));
		};
//      remote_number->update_widths(FALSE); // commented out on 110102
	};
//   if (type != REMOTE_PICTURE && // maybe need a new type GLOBAL_REMOTE_PICTURE
//       text_identifier(identifier)) {
//	   number->update_text_and_widths(TRUE);
//   };
	if (blank) { // || (notebook_version <= 2 && type == BLANK_INTEGER)) {
		number->become_blank();
	};
	number->set_color(color,(type == RATIONAL_NUMBER)); // new on 100100 -- is second arg OK? Should sensors be gray if inexact???
	number->set_infinite_stack(infinite_stack); // new on 080400
	if (number->is_remote_picture() && notebook_version >= first_version_to_save_programmer_and_remotes_accurately) { 
		// starting 020200 (type == REMOTE_PICTURE || type == REMOTE_TEXT_PICTURE) was replaced by 
		// number->is_remote_picture()  - e.g. joystick buttons are different
		// new on 050100
		// type == REMOTE_TEXT_PICTURE added on 110100
		Sprite *appearance = load_item(pad_in,notebook_version,language);
		if (appearance != NULL) { // conditional added 010200
			if (dumped_value < 0) { // new on 160200 - e.g. LANGUAGE_REMOTE
				appearance->destroy(); // shouldn't have dumped it
			} else {
			   ((Remote_Picture *) number)->set_appearance(appearance);
				if (active && appearance->kind_of() == TEXT) { // new on 150801 so it animates
					((Text *) appearance)->set_plate_state(TEXT_PAD_WITH_PLATE_ANIMATED);
					// appearance->set_never_cache(TRUE); 
					// new on 020302 since always changing - only if active?? -- remodularized on 130405
				};
			};
		};
	};
	return(number);
};

Sprites *load_operations(InputStream *pad_in, 
                         int notebook_version, NaturalLanguage language,
                         Picture *controlee) {
	Sprites *operations = NULL;
	if (notebook_version > 1) { // added operations
		unsigned short int count;
		pad_in->read((string) &count, sizeof(count));
		while (count > 0) {
			Sprite *new_item = load_item(pad_in,notebook_version,language,controlee);
			if (operations == NULL) {
				operations = new Sprites(new_item);
			} else {
				operations->insert_at_end(new_item);
			};
			count--;
		};
	};
	return(operations);
};

boolean describe_operations(InputStream *pad_in, output_stream &text_out, int version_number,
								    boolean in_thought_bubble) {
	unsigned short int count;
	pad_in->read((string) &count, sizeof(count));
	if (count > 0) {
		text_out << space() << S(IDS_COMBINED_WITH) << space();
		while (count > 0) {
			if (!describe_item((SpriteType) pad_in->get(), // was describe_number prior to 190201
									 pad_in,text_out,version_number,in_thought_bubble,INDEFINITE_ARTICLE)) {
				return(FALSE);
			};
			count--;
			if (count != 0) text_out << space() << S(IDS_AND) << space();
		};
		text_out << S(IDS_COMBINED_WITH2,"");
	};
	return(TRUE);
};

boolean describe_joystick_button_remote(int joystick_number, RemoteIdentifier identifier, output_stream &text_out, boolean whole_sentence) {
   if (identifier >= JOYSTICK_BUTTON1_REMOTE && identifier <= JOYSTICK_BUTTON32_REMOTE) {
      if (whole_sentence) {
         text_out << S(IDS_THIS_IS) << space();
      };
      text_out << S(IDS_JOYSTICK_BUTTONS_HELP_START1) << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << (int) (1+identifier-JOYSTICK_BUTTON1_REMOTE)
               << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_OF_JOYSTICK_NUMBER) << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << joystick_number
               << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_JOYSTICK_BUTTONS_HELP_START2);
      if (whole_sentence) {
         text_out << end_sentence();
      };
      return(TRUE);
   } else if (identifier >= JOYSTICK_BUTTON1_DOWN_REMOTE && identifier <= JOYSTICK_BUTTON32_DOWN_REMOTE) {
      if (whole_sentence) {
         text_out << S(IDS_THIS_IS) << space();
      };
      text_out << S(IDS_JOYSTICK_BUTTONS_DOWN_HELP_START1) << space(SPACE_BEFORE_OR_AFTER_NUMBERS) 
					<< (int) (1+identifier-JOYSTICK_BUTTON1_DOWN_REMOTE)
               << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_OF_JOYSTICK_NUMBER) << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << joystick_number
               << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_JOYSTICK_BUTTONS_DOWN_HELP_START2);
      if (whole_sentence) {
         text_out << end_sentence();
      };
      return(TRUE);
   };
   return(FALSE);
};

//void print_rational(NumberFormat number_format, long value, double double_float_value, NumberValue *number_value, output_stream &text_out) {
//	if (number_format == TT_LONG) {
//		print_integer_with_commas(value,text_out);
//	} else if (number_format == TT_DOUBLE_FLOAT) {
//		text_out << setprecision(DBL_DIG) << setiosflags(std::ios_base::fixed) << double_float_value;
//	} else if (number_value != NULL) {
//		number_value->print(text_out);
//		// rewritten on 050302
//		if ((text[0] >= '0' && text[0] <= '9') || text[0] == '.') { // removed || text[0] == '-' since the operation should be "subtract"
//			text_out.write(text,text_length); // for now OK - don't worry about commas etc.
//		} else { // worry about longer than 1 character operations later...
//			text_out.write(text+1,text_length-1); 
//		};
//	};
//};

boolean describe_operation(NumberOperation operation, NumberFormat number_format, NumberValue *number_value, boolean operation_with_no_value,
									output_stream &text_out) {
	switch (operation) {
		case NO_NUMBER_OPERATION:
			return(FALSE); // shouldn't happen
		case INCREASE_BY:
			text_out << S(IDS_OPERATION_ADD,"add") << space();
			if (operation_with_no_value) {
				text_out << S(IDS_TWO_NUMBERS,"two numbers") << S(IDS_OPERATION_ADD2,""); // new on 030302
			} else {
            if (number_value == NULL) return(FALSE); // for robustness (new on 150702)
				number_value->print(text_out);
			};
			return(TRUE);
		case SUBTRACT_FROM:
			text_out << S(IDS_OPERATION_SUBTRACT,"subtract") << space();
			if (operation_with_no_value) {
				text_out << S(IDS_TWO_NUMBERS,"two numbers") << S(IDS_OPERATION_SUBTRACT2,""); // new on 030302
			} else {
            if (number_value == NULL) return(FALSE); // for robustness (new on 150702)
				number_value->print(text_out);
			};
			return(TRUE);
		case MULTIPLY_BY:
			if (operation_with_no_value) {
				text_out << S(IDS_OPERATION_MULTIPLY,"multiply") << space() << S(IDS_TWO_NUMBERS,"two numbers") << S(IDS_OPERATION_MULTIPLY2,"");
			} else {
				text_out << S(IDS_OPERATION_MULTIPLY_BY,"multiply by") << space();
            if (number_value == NULL) return(FALSE); // for robustness (new on 150702)
				number_value->print(text_out);
			};
//			print_integer_with_commas(value,text_out); -- most of these worked this way prior to 110202
			return(TRUE);
		case DIVIDE_BY:
			if (operation_with_no_value) {
				text_out << S(IDS_OPERATION_DIVIDE,"divide") << space() << S(IDS_TWO_NUMBERS,"two numbers") << S(IDS_OPERATION_DIVIDE2,"");
			} else {
				text_out << S(IDS_OPERATION_DIVIDE_BY,"divide by") << space();
            if (number_value == NULL) return(FALSE); // for robustness (new on 150702)
				number_value->print(text_out);
			};
			return(TRUE);
		case MODULUS_BY:
			if (operation_with_no_value) {
				text_out << S(IDS_OPERATION_DIVIDE,"divide") << space() << S(IDS_TWO_NUMBERS,"two numbers") << S(IDS_OPERATION_DIVIDE2,"");
			} else {
				text_out << S(IDS_OPERATION_DIVIDE_BY,"divide by") << space();
            if (number_value == NULL) return(FALSE); // for robustness (new on 150702)
				number_value->print(text_out);
			};
			text_out << S(IDS_AND_KEEP_REMAINDER); 
			return(TRUE);
		case MAKE_EQUAL:
			if (operation_with_no_value) {
				text_out << S(IDS_OPERATION_CHANGE_NUMBER_NO_VALUE,"make one number equal to another");
			} else {
				text_out << S(IDS_OPERATION_CHANGE_NUMBER,"change the number it is dropped on to") << space(); // added space on 290303
            if (number_value == NULL) return(FALSE); // for robustness (new on 150702)
				number_value->print(text_out);
			};
			return(TRUE);
		case BITWISE_OR:
			if (operation_with_no_value) {
				text_out << S(IDS_OPERATION_BITWISE_OR_NO_VALUE,"OR the bits of") << space() << S(IDS_TWO_NUMBERS,"two numbers") 
							<< S(IDS_OPERATION_BITWISE_OR_NO_VALUE2,"");
			} else {
				text_out << S(IDS_OPERATION_BITWISE_OR,"OR the bits of the number underneath with") << space(); // added space on 290303
				number_value->print(text_out);
			};
			return(TRUE); 
		case BITWISE_AND:
			if (operation_with_no_value) {
				text_out << S(IDS_OPERATION_BITWISE_AND_NO_VALUE,"AND the bits of") << space() << S(IDS_TWO_NUMBERS,"two numbers") 
							<< S(IDS_OPERATION_BITWISE_AND_NO_VALUE2,"");
			} else {
				text_out << S(IDS_OPERATION_BITWISE_AND,"AND the bits of the number underneath with") << space(); // added space on 290303
            if (number_value == NULL) return(FALSE); // for robustness (new on 150702)
				number_value->print(text_out);
			};
			return(TRUE); 
		case BITWISE_EXCLUSIVE_OR:
			if (operation_with_no_value) {
				text_out << S(IDS_OPERATION_BITWISE_XOR_NO_VALUE,"EXCLUSIVE OR the bits of") << space() << S(IDS_TWO_NUMBERS,"two numbers") 
							<< S(IDS_OPERATION_BITWISE_XOR_NO_VALUE2,"");
			} else {
				text_out << S(IDS_OPERATION_BITWISE_XOR,"will EXCLUSIVE OR the bits of the number underneath with") << space(); // added space on 290303
            if (number_value == NULL) return(FALSE); // for robustness (new on 150702)
				number_value->print(text_out);
			};
			return(TRUE); 
		case TO_THE_POWER:
			if (operation_with_no_value) {
				text_out << S(IDS_OPERATION_COMPUTE_THE_POWER_NO_VALUE,"raise one number to the power of another");
			} else {
				text_out << S(IDS_OPERATION_COMPUTE_THE_POWER,"raise what it is dropped on to the power of") << space(); // added space on 290303
				// could special case when value is 2 or 1/2 or -1
            if (number_value == NULL) return(FALSE); // for robustness (new on 150702)
				number_value->print(text_out);
			};
			return(TRUE); 
		case INTEGER_PART:
			text_out << S(IDS_OPERATION_INTEGER_PART,"leave the integer part");
			return(TRUE); 
		case FRACTION_PART:
			text_out << S(IDS_OPERATION_FRACTION_PART,"leave the fractional part");
			return(TRUE); 
		case NUMERATOR:
			text_out << S(IDS_OPERATION_NUMERATOR,"leave the numerator");
			return(TRUE); 
		case DENOMINATOR:
			text_out << S(IDS_OPERATION_DENOMINATOR,"leave the denominator");
			return(TRUE);
		case SINE:
			text_out << S(IDS_OPERATION_SINE,"take the sine");
			return(TRUE);
		case COSINE:
			text_out << S(IDS_OPERATION_COSINE,"take the cosine");
			return(TRUE);
		case TANGENT:
			text_out << S(IDS_OPERATION_TANGENT,"take the tangent");
			return(TRUE);
		case ARCSINE:
			text_out << S(IDS_OPERATION_ARCSINE,"take the arc sine");
			return(TRUE);
		case ARCCOSINE:
			text_out << S(IDS_OPERATION_ARCCOSINE,"take the arc cosine");
			return(TRUE);
		case ARCTANGENT:
			text_out << S(IDS_OPERATION_ARCTANGENT,"take the arc tangent");
			return(TRUE);
		case NATURAL_LOG:
			text_out << S(IDS_OPERATION_NATURAL_LOG,"compute the natural log");
			return(TRUE);
		case LOG10:
			text_out << S(IDS_OPERATION_LOG10,"compute the log in base 10");
			return(TRUE);
		case COMPLEMENT:
			text_out << S(IDS_OPERATION_BITWISE_COMPLEMENT,"complement the bits");
			return(TRUE);
		default:
			text_out << S(IDS_THE_OPERATION) << operation_to_string(operation) << space(); // added space on 290303
         if (number_value == NULL) return(FALSE); // for robustness (new on 150702)
			number_value->print(text_out);
			return(TRUE);
	};
};

boolean describe_operations(NumberOperation operation, NumberFormat number_format, NumberValue *number_value, Number *following_operation, boolean operation_with_no_value,
									 output_stream &text_out) {
	// major rewrite on 050302
	if (describe_operation(operation,number_format,number_value,operation_with_no_value,text_out)) {
		if (following_operation != NULL) {
			text_out << space() << S(IDS_THEN,"then") << space();
			NumberValue *new_value = NULL;
			boolean new_value_shared = TRUE;
			if (following_operation->current_value(new_value,new_value_shared)) {
				boolean result = describe_operations(following_operation->true_operation(),
																 following_operation->return_number_format(),new_value,following_operation->pointer_to_following_operation(),FALSE,text_out);
				if (!new_value_shared) delete new_value;
				return(result);
			};
		};
		return(TRUE);
	} else {
		return(FALSE); // something went wrong describing the operation
	};
};

/* old version:
	// value not really used here...
	string remaining_operations = NULL;
	if (text != NULL) { // added the NULL test on 150202
		remaining_operations = strchr(text,';');
	};
	if (remaining_operations == NULL) { // is only one
		return(describe_operation(operation,number_format,value,double_float_value,number_value,operation_with_no_value,text_out));
	} else {
		remaining_operations[0] = '\0'; // temporarily clobber text where semicolon is
		int text_length_of_first_operation = strlen(text);
		boolean result 
		// = describe_operation(operation,number_format,value,double_float_value,number_value_of_first_operation,operation_with_no_value,text_out);
		remaining_operations[0] = ';'; // restore
		if (!result) {
			return(result); // something went wrong
		};
		text_out << space() << S(IDS_THEN,"then") << space();
		return(describe_operations(operation_from_string(text+text_length_of_first_operation+2),
											number_format,value,double_float_value,text+text_length_of_first_operation+2,
											text_length-(text_length_of_first_operation+2),operation_with_no_value,text_out));
	};
*/

boolean describe_number(SpriteType type, InputStream *pad_in, output_stream &text_out, int version_number,
							   boolean in_thought_bubble, Article ) {
	if (version_number >= first_version_to_save_copied_nests) {
		describe_comment_encoding(pad_in,text_out,version_number,type);
	};
	// updated on 090202 to deal with rationals
	NumberFormat number_format = TT_LONG;
	if (version_number >= first_version_to_save_rational_numbers) { // was type == RATIONAL_NUMBER prior to 020302 
		// no need for version_number >= first_version_to_save_rational_numbers && 
		number_format = (NumberFormat) pad_in->get();
#if TT_DEBUG_ON
		if (number_format >= TT_UNDETERMINED_NUMBER_FORMAT) {
			log("Loading a number that wasn't stored correctly.");
			throw(tt_global_picture); // new on 130103 for a softer lander (Leonel's students encountered this)
		};
#endif
	};
	long value = 0;
//	double double_float_value;
//	string text = NULL;
//	long text_length = 0;
	NumberValue *number_value = NULL;
	Number *following_operation = NULL;
	int base = 10;
	byte format_flags = 0;
//	Sprite *following_operation;
	if (number_format == TT_LONG) {
		pad_in->read((string) &value, sizeof(value));
		number_value = new LongNumberValue(value);
	} else if (number_format == TT_COMPUTE_VALUE_WHEN_LOADED) { // new on 020302
		// do nothing here
	} else {
//		if (number_format == TT_DOUBLE_FLOAT) {
//			pad_in->read((string) &double_float_value, sizeof(double_float_value)); 
//		} else {	
//			pad_in->read((string) &text_length, sizeof(text_length));
//			text = new char[text_length+1];
//			pad_in->read(text,text_length+1); // include terminator
		number_value = load_number_value(pad_in,version_number);
		if (number_format != TT_DOUBLE_FLOAT) {	
			format_flags = pad_in->get();
			base = pad_in->get();
		};
		following_operation = (Number *) load_item(pad_in,version_number); // could check that it really is a Number
	};
	char operation_key = (char) pad_in->get();
	NumberOperation operation;
	if (version_number >= first_version_to_save_rational_numbers) { // was type == RATIONAL_NUMBER prior to 020302
		operation = (NumberOperation) operation_key;
	} else {
		operation = operation_from_key(operation_key);
	};
	boolean blank = (boolean) pad_in->get();
	if (version_number >= first_version_to_save_number_colors) pad_in->get(); // ignore color - new on 110100
	RemoteIdentifier identifier;
   if (type == REMOTE_TEXT_PICTURE ||
       (type >= GLOBAL_JOYSTICK0_REMOTE_INTEGER && type <= GLOBAL_JOYSTICK31_REMOTE_INTEGER)) {
       pad_in->get(); // ignore language indicator...
   };
   if (type == REMOTE_TEXT_PICTURE || type == GLOBAL_REMOTE_INTEGER || type ==  GLOBAL_USER_REMOTE ||
       type == REMOTE_INTEGER || type == FORCE_REMOTE_INTEGER || type == REMOTE_PICTURE || type == GLOBAL_HYPERBOT_REMOTE_INTEGER ||
       (type >= GLOBAL_JOYSTICK0_REMOTE_INTEGER && type <= GLOBAL_JOYSTICK31_REMOTE_INTEGER)) {
		if (blank) {
         if (in_thought_bubble) {
				if (type == REMOTE_PICTURE) {
					text_out << S(IDS_ANY_PICTURE); // new on 090999
				} else {
					text_out << S(IDS_ANY_NUMBER);
				};
				text_out << " (" << S(IDS_FOR_EXAMPLE) << comma() << space();
         } else {
//				print_article(article,text_out,S(IDS_BLANK_NUMBER_PAD));
            text_out << S(IDS_BLANK_NUMBER_PAD);
            text_out << space() << S(IDS_WHICH_ORIGINALLY_WAS) << space();
         };
      };
		if ((type >= GLOBAL_JOYSTICK0_REMOTE_INTEGER && type <= GLOBAL_JOYSTICK31_REMOTE_INTEGER && version_number <= 21) || type == FORCE_REMOTE_INTEGER) { 
			// new on 150801 to restore ignoring active flag for joystick remotes
			// on 270801 made to only be for older version
         // prior to 230702 was version_number > 21 above but these flags are dumped
			identifier = (RemoteIdentifier) pad_in->get();
		} else {
			boolean active; // ignored for now
			active_and_identifier(pad_in->get(),active,identifier); // new on 090999
		};
      if (type == GLOBAL_USER_REMOTE) { // followed by text item
         text_out << S(IDS_A_REMOTE_CONTROL_FOR) << S(IDS_X_HELP+identifier) << end_sentence();
         text_out << space() << S(IDS_ON_TOP_OF_IT_IS) << space();
         if (!describe_item(pad_in,text_out,version_number)) return(FALSE);
         text_out << end_sentence();
      } else if (type == FORCE_REMOTE_INTEGER) {
         ForceRemoteControlee *controlee;
			UnwindProtect<boolean> set(tt_loading_to_describe,TRUE); // new on 160801
         // note this won't produce a mini-dump
			try {
				controlee = (ForceRemoteControlee *) load_item(pad_in); // check type first in case really broken?
			} catch (Sprite *) { // new on 160801 since might be buggy or EQUAL_TO_OLD_ITEM
				controlee = NULL;
			};
         // these are kept in special arrays so don't worry about a memory leak here
         text_out << S(IDS_A_REMOTE_CONTROL_FOR)
                  << S(IDS_FORCE_FEEDBACK_DURATION_REMOTE_HELP+identifier-FORCE_FEEDBACK_DURATION_REMOTE) << space()
                  << S(IDS_FORCE_FEEDBACK_EFFECT_HELP);
			if (controlee != NULL) { // condition added 171199 for robustness
				text_out << space() << controlee->return_force_name() << end_sentence();
			}; // else warn??
         // could also mention which joystick but let's keep it simple
      } else if (!describe_joystick_button_remote((int) (1+type-GLOBAL_JOYSTICK0_REMOTE_INTEGER),identifier,text_out,FALSE)) {
         // even if the article is definite there may be more than one remote
         text_out << S(IDS_A_REMOTE_CONTROL_FOR) << S(IDS_X_HELP+identifier);
      };
//			if (identifier == RANDOM_REMOTE) {
//				print_article(INDEFINITE_ARTICLE,text_out,
//								  S(IDS_X_HELP+identifier));
//			} else {
//				print_article(DEFINITE_ARTICLE,text_out,
//								  S(IDS_X_HELP+identifier));
//			};
      if (type == REMOTE_TEXT_PICTURE || type == REMOTE_INTEGER || type == REMOTE_PICTURE) {
			// last 3 args added on 300699 to correctly ignore controlee
         if (!ignore_next_item(pad_in,version_number,in_thought_bubble,NONE_GIVEN,TRUE)) {
				return(FALSE); // new on 120101
			};
      };
      if (read_only_identifier(identifier)) { //type != REMOTE_PICTURE) {
         if (!describe_operations(pad_in,text_out,version_number,in_thought_bubble)) return(FALSE);
      };
      if (blank && in_thought_bubble) text_out << ")"; // end "for example"
		if ((remote_address(identifier) || remote_picture(identifier)) && 
			 version_number >= first_version_to_save_programmer_and_remotes_accurately) {
			// on 170300 changed last arg from TRUE since isn't a controlee
			if (!ignore_next_item(pad_in,version_number,in_thought_bubble,NONE_GIVEN,FALSE)) { // new on 070200 to ignore the appearance
				return(FALSE); // new on 120101
			};
		};
      return(TRUE);
	};
	if (blank) {
		if (in_thought_bubble) {
			text_out << S(IDS_ANY_NUMBER);
		} else {
//			print_article(article,text_out,S(IDS_BLANK_NUMBER_PAD));
         text_out << S(IDS_BLANK_NUMBER_PAD);
		};
	} else if (operation == NO_NUMBER_OPERATION) {
//		text_out << S(IDS_THE_NUMBER); // commented out on 251104 since TRUE arg added below now does this better
//		print_integer_with_commas(value,text_out);
		number_value->print(text_out,TRUE);
		return(TRUE);
	} else {
		text_out << S(IDS_SOMETHING_WHICH_WILL,"something which will") << space();
		boolean result = describe_operations(operation,number_format,number_value,following_operation,
														 format_flags&OPERATION_WITH_NO_VALUE,text_out);
		text_out << S(IDS_SOMETHING_WHICH_WILL2,""); // was "too " // restored on 030302 to be grammatical for other langauges
		if (following_operation != NULL) {
			following_operation->destroy(); // for now (100202)
		};
	};
	return(TRUE);
};

void Number::set_long_value(long new_value, boolean ignore_change, boolean sizes_constrained, boolean update) {
	if (ignore_change) { // new on 030302 to ignore this if long_values are the same
		long long_value;
		if (current_long_value(long_value)) {
			if (long_value == new_value) return;
		};
	};
	set_value(new LongNumberValue(new_value),ignore_change,sizes_constrained,update);
};

void Number::set_double_value(double new_value, boolean ignore_change, boolean sizes_constrained, boolean update) {
	if (ignore_change) { // new on 030302 to ignore this if long_values are the same
		double double_value;
		if (current_double_value(double_value)) {
			if (double_value == new_value) return;
		};
	};
	set_value(new RationalNumberValue(new_value),ignore_change,sizes_constrained,update);
};

#if TT_XML

void Number::xml(xml_element *element, xml_document *document) {
	if (!using_default_text_color) {
		xml_set_attribute(element,L"NotUsingDefaultTextColor",1);
	};
	if (is_an_operation_with_no_value_flag) {
		xml_set_attribute(element,L"IsAnOperationWithNoValue",1);
	};
	if (min_text_length != 0) {
		xml_set_attribute(element,L"MinimumTextLength",min_text_length);
	};
	if (following_operation != NULL) {
		following_operation->add_xml(element,L"NextOperation",document);
	};
	if (operation != NO_NUMBER_OPERATION) { // moved here from Rational on 191202
		// prior to 250204 called operation_to_string but that broke other language versions copy and paste
      xml_append_child(create_xml_element(L"Operation",document,operation_to_wide_string(operation)),element); 
		// should this be called "Operator"?
   };
   if (text_color != tt_black) { // if black don't bother // moved here from Rational on 191202
      xml_append_child(create_xml_element(L"ColorIndex",document,text_color),element);
   };
	if (!shrinking_decimal_notation()) { // new on 230603 
		xml_set_attribute(element,L"ShrinkingDecimal",0);
	};
	if (edited) { // new on 090604
		if (text_length == 0) {
			xml_set_attribute(element,L"EditedAndValueNotUpdated",2); // new on 230704 to deal with rubbed out numbers
		} else {
			xml_set_attribute(element,L"EditedAndValueNotUpdated",1);
		};
	};
	if (insertion_point >= 0) { // new on 100604
		xml_set_attribute(element,L"EditInsertionPoint",insertion_point);
	};
#if TT_SHRINKING_AND_GROWING
	if (display_with_size_unconstrained) { // new on 071104
		xml_set_attribute(element,L"DisplayWithAGoodSize",display_with_size_unconstrained);
	};
#endif
	Sprite::xml(element,document);
};

xml_element *Number::xml_set_special_attributes(xml_element *element, xml_document *document) {
	// new on 191202 -- went through all the state variables
	xml_element *special_element = Sprite::xml_set_special_attributes(element,document);
	//if (!first_key_since_selection) { // noticed obsolete on 080205
	//	xml_set_attribute(special_element,L"NotFirstKeySinceSelection",1);
	//};
	//if (change_size_to_fit_extent_flag) {
	//	xml_set_attribute(special_element,L"ChangeSizeToFitExtent",1);
	//};
	//if (!change_font_size_to_fit_extent_flag) {
	//	xml_set_attribute(special_element,L"DontChangeFontSizeToFitExtent",1);
	//};
	xml_set_attribute(special_element,L"CharacterWidth",character_width); // maybe could be computed rather than saved
	xml_set_attribute(special_element,L"CharacterHeight",character_height); 
	// text_length can be computed at load time
	return(special_element);
};

boolean Number::xml_get_special_attributes(xml_node *node) {
	//first_key_since_selection = !xml_get_attribute_int(node,L"NotFirstKeySinceSelection",0); 
	// prior to 210404 !first_key_since_selection
	//change_size_to_fit_extent_flag = xml_get_attribute_int(node,L"ChangeSizeToFitExtent",change_size_to_fit_extent_flag);
	//change_font_size_to_fit_extent_flag = 
	// !xml_get_attribute_int(node,L"DontChangeFontSizeToFitExtent",!change_font_size_to_fit_extent_flag);
	set_character_width(xml_get_attribute_int(node,L"CharacterWidth",character_width));
	set_character_height(xml_get_attribute_int(node,L"CharacterHeight",character_height));
	return(Sprite::xml_get_special_attributes(node));
};

boolean Number::handle_xml_tag(Tag tag, xml_node *node) {
#if TT_DEBUG_ON
	DWORD start;
	if (tt_debug_mode == 310303) {
		start = timeGetTime();
	};
#endif
	switch (tag) {
      case COLOR_INDEX_TAG: // moved here from Rational on 191202
         set_color(xml_get_int(node,tt_black));
         return(TRUE);
      case OPERATION_TAG: {
         // was xml_get_narrow_text_copy prior to 140104 but that was a leak
         set_operation(operation_from_string(xml_get_text(node)));
         return(TRUE);
                          };
		case NEXT_OPERATION_TAG:
			following_operation = (Number *) xml_get_sprite(node,following_operation,INTEGER);
			return(TRUE);
		case NO_MORE_TAGS:
			using_default_text_color = !xml_get_attribute_int(node,L"NotUsingDefaultTextColor",0); // prior to 210404 !using_default_text_color
			is_an_operation_with_no_value_flag = xml_get_attribute_int(node,L"IsAnOperationWithNoValue",0); 
			// prior to 210404 is_an_operation_with_no_value_flag
			min_text_length = xml_get_attribute_int(node,L"MinimumTextLength",0); // prior to 210404 min_text_length
			set_shrinking_decimal_notation(xml_get_attribute_int(node,L"ShrinkingDecimal",1));
//			edited = xml_get_attribute_int(node,L"EditedAndValueNotUpdated",0); // new on 09060 -- handled in xml.cpp	
			if (edited) { // no need to bother otherwise
				insertion_point = xml_get_attribute_int(node,L"EditInsertionPoint",insertion_point);
			};
#if TT_SHRINKING_AND_GROWING
			display_with_size_unconstrained = xml_get_attribute_int(node,L"DisplayWithAGoodSize",FALSE);
#endif
#if TT_DEBUG_ON
			if (tt_debug_mode == 310303) {
				tt_error_file() << (timeGetTime()-start) << " ms to process NO_MORE_TAGS in Number::handle_xml_tag" << endl;
			};
#endif
			// and the following
		default:
			return(Sprite::handle_xml_tag(tag,node));
	};
};

void Rational::just_set_narrow_text(wide_string text, int length) { // new on 090604
	narrow_text = copy_narrow_string(text,length);
	text_length = length;
	text_max_length = length;
	wide_text = copy_wide_string(text,length);
	set_edited(TRUE);
};

#endif

// prior to 050803 this was for Rational only
boolean Number::set_value_as_string(string text, int text_length, NumberFormat number_format, boolean size_constrained, 
												boolean ignore_operation) { 
	// assumes text is terminated
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Running set_value of debug target.");
	};
#endif
	if (text == NULL) return(FALSE); // new on 120604 for robustness
	set_clean_status(DIRTY); // new on 110703
	boolean found_operation_already = FALSE;
	NumberOperation original_operation = operation; // new on 211104
	while (text[0] == ' ') { // new on 120604
		text++;
		text_length--;
	};
	string next_operation = strchr(text,';');
	if (!ignore_operation && text[0] != '-' && valid_operation_key(text[0])) { 
		// re-written on 150202 -- added check for negative numbers on 260302
		set_operation(operation_from_key(text[0]),(next_operation != NULL)); // prior to 110604 next_operation != NULL was TRUE
		found_operation_already = TRUE;
		text++;
		text_length--;
	};
	while (!ignore_operation && valid_operation_key(text[text_length-1])) { 
		// ends with an operation so accept it as well -- new on 110604
		// changed from if to while on 051104 to deal with multiple operators at the end -- e.g. 2-- or more sensibly 2-*
		NumberOperation operation = operation_from_key(text[text_length-1]);
		if (operation == SUBTRACT_FROM) {
			if (text[0] == '-') { // both in the beginning and the end
				text[0] = '0'; // remove it (and the one at the end)
			} else {
				for (int i = text_length-2; i >= 0; i--) {
					text[i+1] = text[i];
				};
				text[0] = '-'; // prepend the minus sign
				text_length++; // to be decremented below
			};
		} else if (!found_operation_already) {
			// condition new on 101204 to maintain old behavior of multiple operations becoming the last one -- e.g. %x
			set_operation(operation,(next_operation != NULL)); // prior to 110604 next_operation != NULL was TRUE
			found_operation_already = TRUE;
		};
		text_length--;
		text[text_length] = '\0'; // just in case changed above
	};
	if (found_operation_already && text_length == 0) { // operator without a value -- new on 270202
		number_format = TT_LONG;
		set_long_value(0);
		return(TRUE);
	};
	if (next_operation != NULL && next_operation[1] == ' ') { 
		// added test on 050203 that next character be a space - clipboard may have foo = 1; on it
		next_operation[0] = '\0'; // terminate it there for the rest of this code
		text_length = strlen(text); // ignore the rest
		next_operation += 2; // skip over the semicolon and space
		Rational *following_operation = new Rational();
		following_operation->set_value_as_string(next_operation,strlen(next_operation));
		if (following_operation->true_operation() == NO_NUMBER_OPERATION) { // new on 110604
			if (following_operation->sign() >= 0) {
				following_operation->set_operation(INCREASE_BY);
			} else {
				following_operation->set_operation(SUBTRACT_FROM);
				following_operation->negate();
			};
		};
		just_set_following_operation(following_operation);
	};
	if (IsCharAlpha(text[0])) { // new on 150202
		NumberOperation operation = operation_from_string(text);
		if (operation != NO_NUMBER_OPERATION) {
			set_operation(operation);
//			found_operation_already = TRUE;
			if (unary_operation(operation)) {
				set_long_value(0); // doesn't matter but needs a value
				return(TRUE); // dealt with ; already
			};
		} else { // new on 100604
			set_operation(NO_NUMBER_OPERATION);
		};
	} else if (!found_operation_already) { // new on 100604
		set_operation(NO_NUMBER_OPERATION);
	};
	if (text_length < 0) { // re-ordered with the following on 270202 since -1 doesn't imply it is a long
		text_length = strlen(text);
	};
	while (text[text_length-1] == ' ') {
		text_length--; // remove trailing blanks since they confuse decimal processing -- e.g "1.5 "
		text[text_length] = NULL;
	};
	// following converted 10/ to /10 but that is weird
	//if (text_length > 1) { // and isn't unary negation (not to be mistaken for subtraction)
	//	// could deal with white space and function names -- e.g. sine
	//	NumberOperation operation = operation_from_string(text+text_length-1);
	//	if (operation == SUBTRACT_FROM) {
	//		text_length--;
	//		text[text_length] = '\0';
	//		// need to shift over to put - in front
	//		int first = 0;
	//		if (found_operation_already) first++;
	//		for (int i = text_length-1; i >= first; i--) {
	//			text[i+1] = text[i];
	//		};
	//		text[0] = '-';
	//	} else if (operation != NO_NUMBER_OPERATION) {
	//		set_operation(operation);
	//		text_length--; // ignore the operation in the following
	//		text[text_length] = '\0'; // terminate it
	//	};
	//};
   int the_base;
	boolean initial_minus_sign = (text[0] == '-');
	if (initial_minus_sign) { // new on 220505
		text++;
	};
   int offset_to_skip_base = extract_base(text,the_base);
   base = (unsigned char) the_base;
	if (initial_minus_sign) {
		if (offset_to_skip_base > 0) { // new on 220505 -- second condition new on 280505
			offset_to_skip_base--;
			// following moved into conditional on 280505
			text += offset_to_skip_base;
			if (initial_minus_sign) { // new on 220505
				text[0] = '-'; // turns # to -
			};
		} else { // restore text
			text--;
		};
	} else {
		text += offset_to_skip_base; // copied here on 020605
	};
   text_length -= offset_to_skip_base;
   // following abstracted as extract_base on 160802
	//if (text[1] == '#') { // new on 270202
	//	base = text[0]-'0';
	//	if (base < 2 || base > 36) {
	//		base = 10; // warn??
	//	};
	//	text += 2;
	//	text_length -= 2;
	//} else if (text_length > 3 && text[2] == '#') {
	//	base = (text[0]-'0')*10+(text[1]-'0');
	//	if (base < 2 || base > 36) {
	//		base = 10;
	//	};
	//	text += 3;
	//	text_length -= 3;
	//};
	if (number_format == TT_UNDETERMINED_NUMBER_FORMAT) {
		if (strchr(text,'.') != NULL) {
			number_format = TT_BIGNUM_DECIMAL;
		} else {
			string beginning_with_space = strchr(text,' ');
			if (beginning_with_space != NULL && (beginning_with_space[1] >= '0' && beginning_with_space[1] <= '9')) { 
				// new on 170202 to check that following the space is a digit -- broke puzzle loading otherwise
				number_format = TT_BIGNUM_WITH_FRACTION;
			} else {
				string beginning_with_slash = strchr(text,'/');
				if (beginning_with_slash != NULL) {
					number_format = TT_RATIONAL;
				} else {
					number_format = TT_BIGNUM;
				};
			};
		};
	};
	if (number_format == TT_BIGNUM && text_length < 10 && base <= 10) { 
		// slight performance penalty for those between one billion and max_long
		// added base test on 100604 a penality for base over 10 that fits in 32-bits... -- 
		// this was a bug for copy and paste of numbers (alone) as well
		number_format = TT_LONG;
	};
	switch (number_format) {
		case TT_LONG: {
//			long long_value = atoi(text);
			long long_value = strtol(text,NULL,base); // rewritten on 270202 to deal with different base
			if (long_value == 0 && text[0] != '0' && !all_characters_are(' ',text) && text_length > 0) {
				if (found_operation_already) { // new on 211104 to undo operation in case of failure
					set_operation(original_operation);
				};
				return(FALSE);
			};
			// not a good number string - all spaces OK (e.g. some sensors with minimum character length) - added on 030302
			// && text_length > 0 added 080604
			set_value(new LongNumberValue(long_value),FALSE,size_constrained);
			return(TRUE);
						  };
		case TT_BIGNUM: {
//			set_value(new BigNumberValue(),FALSE,size_constrained);
			BigNumberValue *new_value = new BigNumberValue();
			bignum_pointer bignum_value;
			new_value->bignum_value(bignum_value);
			set_bignum_value_to_string(bignum_value,text,base);
			if (mpz_sgn(bignum_value) == 0 && (text[0] != '0' || text[1] != '\0')) {
				// second condition added 010205 since 16#0, etc. is fine
				if (found_operation_already) { // new on 211104 to undo operation in case of failure
					set_operation(original_operation);
				};
				return(FALSE); // not a good number string
			};
			set_value(new_value,FALSE,size_constrained);
			return(TRUE);
							 };
		case TT_RATIONAL: {
			int slash_location = strcspn(text,"/");
			if (slash_location == 0) {
				if (found_operation_already) { // new on 211104 to undo operation in case of failure
					set_operation(original_operation);
				};
				return(FALSE);
			};
			// set_value(new RationalNumberValue(),FALSE,size_constrained);
			// rewritten on 101202 since might be a remote control and this value is goofy
			RationalNumberValue *new_value = new RationalNumberValue();
			rational_pointer rational_value;
			new_value->rational_value(rational_value);
			text[slash_location] = '\0'; // clobber the /
			set_bignum_value_to_string(mpq_numref(rational_value),text,base);
			if (mpz_sgn(mpq_numref(rational_value)) == 0 && text[0] != '0') {
				text[slash_location] = '/'; // restore the slash -- new on 050704
				if (found_operation_already) { // new on 211104 to undo operation in case of failure
					set_operation(original_operation);
				};
				return(FALSE); // not a good number string
			};
			set_bignum_value_to_string(mpq_denref(rational_value),text+slash_location+1,base);
			if (mpz_sgn(mpq_denref(rational_value)) == 0) {
				if (found_operation_already) { // new on 211104 to undo operation in case of failure
					set_operation(original_operation);
				};
				return(FALSE); // not a good number string
			};
			// shouldn't need to canonicalize since was OK before it was dumped
			// but is safest and besides this is used by more than load_number now
			mpq_canonicalize(rational_value);
			text[slash_location] = '/'; // restore (even though most likely about to be deleted) --
			// was slanted the wrong way prior to 270202
			set_value(new_value,FALSE,size_constrained);
			return(TRUE);
							};
		case TT_BIGNUM_DECIMAL: {
			int decimal_location = strcspn(text,"."); // unless in another language version !! 040202
			if (decimal_location == 0 && text[0] != '.') {
				if (found_operation_already) { // new on 211104 to undo operation in case of failure
					set_operation(original_operation);
				};
				return(FALSE);
			};
//			set_value(new RationalNumberValue(),FALSE,size_constrained);
			// rewritten on 101202 since might be a remote control and this value is goofy
			RationalNumberValue *new_value = new RationalNumberValue();
			rational_pointer rational_value;
			new_value->rational_value(rational_value);
			//int i = decimal_location;
			//for (; i < text_length; i++) { 
			//	text[i] = text[i+1];
			//};
			//text[i-1] = '\0';
			// rewritten on 050803 to fix bugs introduced on 230603
			string new_text = new char[text_length+1]; 
			// new on 230603 to be safer than the above (since it causes some memory problems)
			for (int i = 0; i < decimal_location; i++) { 
				new_text[i] = text[i];
			};
			for (int i = decimal_location; i < text_length; i++) { // could use memcpy instead here
				new_text[i] = text[i+1];
			};
			set_bignum_value_to_string(mpq_numref(rational_value),new_text,10);
			delete [] new_text;
			// restore text (even though often it is about to be deleted) -- 
			// moved here so that if returns FALSE below that the string is restored - 
			// new on 130302 - broke pasting text with '.' in it
			//for (i = text_length-1; i >= decimal_location; i--) { 
			//	text[i+1] = text[i];
			//};
			//text[decimal_location] = '.';	
			if (mpz_sgn(mpq_numref(rational_value)) == 0 && text[0] != '0') {
				if (found_operation_already) { // new on 211104 to undo operation in case of failure
					set_operation(original_operation);
				};
				return(FALSE); // not a good number string
			};
			mpz_ui_pow_ui(mpq_denref(rational_value),10,(text_length-decimal_location)-1);
         mpq_canonicalize(rational_value);
			set_value(new_value,FALSE,size_constrained);
			return(TRUE);
										};
		case TT_BIGNUM_WITH_FRACTION: { // rewritten on 160802 since strcspn returns length if not there - not zero
         // so error handling was poor previously
//			int space_location = strcspn(text," ");
//			if (space_location == 0) return(FALSE);
//			int slash_location = strcspn(text+space_location,"/");
         string space_and_rest = strchr(text,' ');
         string text_to_use;
			if (space_and_rest == NULL) {
				if (found_operation_already) { // new on 211104 to undo operation in case of failure
					set_operation(original_operation);
				};
				return(FALSE); // not an bignum followed by a fraction
			};
         text_to_use = space_and_rest+1; // skip over the space
         text_length = strlen(text_to_use);
         string slash_and_denominator = strchr(text_to_use,'/');
			if (slash_and_denominator == NULL || slash_and_denominator[1] == NULL) {
				if (found_operation_already) { // new on 211104 to undo operation in case of failure
					set_operation(original_operation);
				};
				return(FALSE);
			};
			// added second condition in case have 2/ so there is no denominator
			NumberOperation saved_operation = operation; 
			// new on 051104 since the following clobbers the operation (anything else?)
         if (set_value_as_string(text_to_use,text_length)) {
//			if (set_value(text+space_location+1,text_length-(space_location+1))) {
//				text[space_location] = '\0'; // terminate temporarily
            space_and_rest[0] = '\0'; // so we can process the first part (the bignum)
				rational integer_part; // rational just since going to add it later
				mpq_init(integer_part);
				set_bignum_value_to_string(mpq_numref(integer_part),text,10);
				rational_pointer rational_value;
				NumberValue *value = pointer_to_value();
				value->rational_value(rational_value);
				mpq_add(rational_value,integer_part,rational_value);
				mpq_clear(integer_part);
            space_and_rest[0] = ' '; // restore to be tidy
				set_operation(saved_operation);
				return(TRUE);
			};
			if (found_operation_already) { // new on 211104 to undo operation in case of failure
				set_operation(original_operation);
			};
			return(FALSE); 											
												};
		case TT_DOUBLE_FLOAT: {
			double double_float_value = atof(text);
			if (double_float_value == 0.0 && text[0] != '0') {
				if (found_operation_already) { // new on 211104 to undo operation in case of failure
					set_operation(original_operation);
				};
				return(FALSE); // not a good number string -- or will it be .0 ??
			};
			set_value(new DoubleFloatNumberValue(double_float_value),FALSE,size_constrained);
			return(TRUE);
									 };
	};
	if (found_operation_already) { // new on 211104 to undo operation in case of failure
		set_operation(original_operation);
	};
	return(FALSE);
};
//
//void Number::set_is_picture(boolean new_flag) {
//	if (new_flag) {
//		change_font_size_to_fit_extent_flag = FALSE;
// new on 121203 so it doesn't remember the font when displayed within the region of the Picture
//	};
//	Sprite::set_is_picture(new_flag);
//};


Rational::Rational(city_coordinate center_x, city_coordinate center_y,
					    long initial_priority,
					    city_coordinate width,
					    city_coordinate height,
					    long long_value,
					    NumberOperation operation,
					    int min_text_length,
                   int text_color) :
	Number(center_x,center_y,width,height,initial_priority,operation,min_text_length,text_color,INTEGER,TT_LONG,TRUE,TRUE),
	wide_text(NULL),
	narrow_text(NULL),
	text_max_length(0),
//	punctuate_every_three_digits(FALSE), // might not ever have TRUE versions of this...
	keyboard_session_start(-1) { // new on 020702 - seems to matter only for big nums though
		value = new LongNumberValue(long_value);
};

//Rational::Rational(double initial_value) {
//	Number(0,0,0,NO_NUMBER_OPERATION,0,-1,INTEGER),
//	wide_text(NULL),
//	narrow_text(NULL),
//	text_max_length(0),
//	integer_and_fraction_format(TRUE),
//	decimal_notation_if_denominator_is_power_of_ten(TRUE),
//	punctuate_every_three_digits(FALSE), // might not ever have TRUE versions of this...
//	base(10),
//	keyboard_session_start(-1) {
//		value = new RationalNumberValue(initial_value);
//};
/* obsolete?
Rational::Rational(city_coordinate center_x, city_coordinate center_y,
					    long initial_priority,
					    city_coordinate width,
					    city_coordinate height,
						 string text, int text_length, NumberFormat number_format, byte format_flags, byte base,
					    NumberOperation operation,
					    int min_text_length,
                   int text_color) :
	Number(center_x,center_y,initial_priority,operation,min_text_length,text_color,INTEGER),
	wide_text(NULL),
	narrow_text(NULL),
	text_max_length(0), 
	base(base),
	keyboard_session_start(-1),
	value(NULL) {
	integer_and_fraction_format = !!(format_flags&INTEGER_AND_FRACTION_FORMAT);
	decimal_notation_if_denominator_is_power_of_ten = !!(format_flags&DECIMAL_NOTATION_IF_EXACT_FORMAT);
	punctuate_every_three_digits = !!(format_flags&PUNCTUATE_EVERY_THREE_DIGITS_FORMAT);
	using_default_text_color = !!(format_flags&USING_DEFAULT_COLOR_FORMAT);
	text[text_length] = '\0'; // make sure it is terminated
	set_value(text,text_length,number_format,TRUE); // TRUE added on 060202 so fresh numbers don't change size
	if (format_flags&OPERATION_WITH_NO_VALUE) {
		set_is_an_operation_with_no_value(TRUE);
	} else if (format_flags&INEXACT_NUMBER) { 
		value->set_exact(FALSE);
	};
};
*/
Rational::Rational(city_coordinate center_x, city_coordinate center_y,
					    long initial_priority,
					    city_coordinate width,
					    city_coordinate height,
						 NumberValue *value, 
						 NumberFormat number_format, byte format_flags, byte base,
					    NumberOperation operation,
					    int min_text_length,
                   int text_color) :
	Number(center_x,center_y,width,height,initial_priority,operation,min_text_length,text_color,INTEGER,
			 number_format,!!(format_flags&INTEGER_AND_FRACTION_FORMAT),!!(format_flags&DECIMAL_NOTATION_IF_EXACT_FORMAT),base),
	wide_text(NULL),
	narrow_text(NULL),
	text_max_length(0), 
	keyboard_session_start(-1),
	value(value) { 
//	integer_and_fraction_format = !!(format_flags&INTEGER_AND_FRACTION_FORMAT);
//	decimal_notation_if_denominator_is_power_of_ten = !!(format_flags&DECIMAL_NOTATION_IF_EXACT_FORMAT);
//	punctuate_every_three_digits = !!(format_flags&PUNCTUATE_EVERY_THREE_DIGITS_FORMAT);
	using_default_text_color = !!(format_flags&USING_DEFAULT_COLOR_FORMAT);
//	text[text_length] = '\0'; // make sure it is terminated
//	set_value(text,text_length,number_format,TRUE); // TRUE added on 060202 so fresh numbers don't change size
	if (format_flags&OPERATION_WITH_NO_VALUE) {
		set_is_an_operation_with_no_value(TRUE);
	} else if (format_flags&INEXACT_NUMBER) { 
		if (value != NULL) { // condition added for robustness on 040702
			value->set_exact(FALSE);
		};
	};
};

Rational::Rational(city_coordinate center_x, city_coordinate center_y,
					    long initial_priority,
					    city_coordinate width,
					    city_coordinate height,
						 NumberValue *initial_value, // should be fresh (e.g. a copy)
						 NumberFormat number_format,
					    NumberOperation operation,
					    int min_text_length,
                   int initial_text_color) :
	Number(center_x,center_y,width,height,initial_priority,operation,min_text_length,initial_text_color,INTEGER,number_format,TRUE,TRUE),
	value(initial_value),
//	future_value(initial_value),
	wide_text(NULL),
	narrow_text (NULL), // why have both narrow and wide versions? - asked 130504
	text_max_length(0),
//	punctuate_every_three_digits(FALSE),
	keyboard_session_start(-1) { 
	if (using_default_text_color && initial_value != NULL && !initial_value->exact()) {
		text_color = tt_gray;
	};
};

void Rational::prepare_for_deletion() { // new on 080202
	if (preparing_for_deletion) return; // already done
	if (value != NULL) { // moved here on 140904
		delete value;
		value = NULL; // new on 130504
	};
	Number::prepare_for_deletion();
};

void Rational::release_all_references() { // new on 130504
	if (releasing_all_references) { // new on 230305
		return;
	};
	// moved the following on 140904 since may be saved by time travel after being bammed
	//if (value != NULL) {
	//	delete value;
	//	value = NULL; // new on 130504
	//};
	if (wide_text != NULL) {
//#if TT_DEBUG_ON
//      if (tt_debug_mode == 10802) {
//         if (wide_text[0] == 'Z') {
//            log("debug this");
//         } else {
//            wide_text[0] = 'Z';
//         };
//      } else {
//         delete [] wide_text;
//      };
//#else
		delete [] wide_text;
		wide_text = NULL; // new on 130504
//#endif
	};
	if (narrow_text != NULL) {
//#if TT_DEBUG_ON
//      if (tt_debug_mode == 10802) {
//         if (narrow_text[0] == 'Y') {
//            log("debug this");
//         } else {
//            narrow_text[0] = 'Y';
//         };
//      } else {
//         delete [] narrow_text;
//      };
//#else
		delete [] narrow_text;
		narrow_text = NULL; // new on 130504
		text_max_length = 0; // new on 070604 to fix a bug unbamming a number dropped on a text pad
//#endif
	};
	Number::release_all_references();
};

Sprite *Rational::copy(boolean also_copy_offsets) {
	Rational *copy = new Rational(llx,lly, 
										   current_priority-1,
										   width,height,
											value->copy(),
											number_format, // new on 020702
//											(future_value==NULL)?value->copy():future_value->copy(),
										   operation,
                                 min_text_length,using_default_text_color?-1:text_color);
	copy->set_size(width,height,COPY_INITIALIZATION); // not always right -- added COPY_INITIALIZATION on 080202
	//if (blank && !tt_copy_restores_pre_blank) {
	//	// only allow blanks if in fully defined robot
	//	copy->become_blank();
	//};
 //  copy->set_priority_fixed(priority_fixed_flag);
	//if (comment != NULL) {
	//	copy->set_comment(comment->copy());
	//};
   finish_making_copy(copy,also_copy_offsets);
	copy->set_current_style(integer_and_fraction_format,decimal_notation_if_denominator_is_power_of_ten,
		                     shrinking_decimal_notation()); // new on 050202
	if (following_operation != NULL) {
		copy->just_set_following_operation((Number *) following_operation->copy());
	};
	copy->set_base(base); // new on 250202
	copy->set_is_an_operation_with_no_value(is_an_operation_with_no_value());
	return(copy);
};

boolean Rational::respond_to_keyboard_body(unsigned char key, boolean extended, boolean robot_selected,
											          Sprite *by, boolean record_action, Sprite *original_recipient, 
														 boolean size_constrained_regardless) {
//	boolean saved_first_key_since_selection = first_key_since_selection;
	if (text_length < 0) { // need to initialize the text form
		long ignore_length;
		string ignore_text;
		current_text(ignore_text,ignore_length);
	};
	long digit_value = digit_to_number(key,base);
	if (key == BACKSPACE || (extended && key == VK_DELETE)) { // delete key new on 100604
		if ((decimal_places > 0 && tt_log_version_number >= 45) || text_length == 0) { // new on 090604 -- empty test new on 110604
			play_sound(PLOP_SOUND);
			return(FALSE); // ignore it since there are an infinite number of them -- thanks Gordon
		};
		user_did(INTEGER,INTEGERS_DIGITS_REMOVED,by);
#if TT_SHRINKING_AND_GROWING
		if (!edited && tt_shrink_and_grow_numbers && narrow_text != NULL && strchr(narrow_text,'/') != NULL) {
			// new on 101204 to deal with multi-line fractions becoming one-dimensional
			// changed first_key_since_selection to !edited on 080205
//			set_change_size_to_fit_extent(TRUE);
			set_edited(TRUE); // so the following works correctly
			set_to_good_size(tt_toolbox);
		};
#endif
//      if (!by->still_running()) first_key_since_selection = FALSE; // noticed obsolete on 080205
		if (insertion_point >= 0) { 
			if ((insertion_point == 0 && key == BACKSPACE) || // if 0 then can't rubout but can Delete -- new on 100604
				 (insertion_point == text_length && key == VK_DELETE)) { // can't delete if at the end
				play_sound(PLOP_SOUND);
				return(FALSE);
			};
			int delete_position;
			if (key == BACKSPACE) {
				delete_position = insertion_point-1;
				insertion_point--; // new on 100604
			} else if (key == VK_DELETE) {
				delete_position = insertion_point;
			};
			if (delete_position >= 0) {
				for (int i = delete_position; i < text_length; i++) {
					narrow_text[i] = narrow_text[i+1];
				};
			};
			text_length--;
		} else if (!keyboard_buffer_empty()) { // new on 100202
			text_length--;
			if (keyboard_session_start == text_length) {
				keyboard_session_start = -1; // erased entire buffer
			};
			if (tt_log_version_number >= 59 && leader == NULL && text_length > 0) { // leader test new on 151204
//				need_to_recompute_text(); // new on 131104 since otherwise rubout doesn't narrow the pad
				// added text_length > 0 on 040105 since otherwise if typing fast can get an invisible number
				set_width(width-character_width); // better version of the above -- 131104
			};
//		} else if (value->rubout(number_format,base)) { 
			// new on 030202 -- removed on 030302 since this doesn't work well with remotes since set_value isn't called
//			need_to_recompute_text(); 
		} else if (narrow_text != NULL) { // conditional added on 010802 - occurred with rubout of some textual sensors - fixed now
			// couldn't rubout this number_format so fall back on string manipulation
			narrow_text[text_length-1] = '\0'; // remove last character
//			if (number_format == TT_BIGNUM_WITH_FRACTION) {
//				log("to do"); // check for final slash
//			};
			if (!by->can_send_keyboard_editing_keys() || reachable_from_remote_looks(original_recipient,this) 
#if TT_SHRINKING_AND_GROWING
				|| part1_shrinkage > 0.0 // was != prior to 191104 but can be -1.0
#endif
				) { 
				// condition updated on 110604
				// new on 080604 -- removed !original_recipient->held() ||  on 100604
				// added original_recipient test on 300604
				// added part1_shrinkage test on 291004
				if (text_length < 2 || narrow_text[text_length-2] == '-' || (text_length == 2 && narrow_text[0] == '.')) { 
					// rubbed out til there is just the - sign left - new on 030302
					// added rub out of . so that .5 becomes zero when rubbed out - new on 200903
					set_long_value(0);
				} else {
					// size_constrained_regardless was FALSE prior to 200204 - so indirection in a box behaved oddly
					set_value_as_string(narrow_text,text_length-1,number_format,size_constrained_regardless,
						                 (operation == NO_NUMBER_OPERATION)); 
					// extra args added on 030302
				};
			} else {
				set_edited(TRUE);
				text_length--;
			};
		};
	} else if (by->can_send_keyboard_editing_keys() && decimal_places <= 0 && 
		        !reachable_from_remote_looks(original_recipient,this) && !blank) { 
		// can't edit shrinking digits (as of 110604)
		// new on 080604 -- removed !original_recipient->held() ||  on 100604
		// added original_recipient test on 300604
		// added blank test on 070804 since handled correctly below
		if (add_to_keyboard_buffer(key,extended)) { // condition new on 270804
			if (key == '-') {
				user_did(INTEGER,INTEGERS_MADE_NEGATIVE,by); // new on 161204
			} else if (key >= '0' && key <= '9') {
				user_did(INTEGER,INTEGERS_TYPED,by); // copied here on 161204
			};
			set_edited(TRUE); 
		} else { // new on 270804 so ignored keys aren't recorded
			record_action = FALSE; 
			return(FALSE);
		};
	} else if (extended) { // new on 110604
		record_action = FALSE;
		return(FALSE);
	} else if (digit_value >= 0 && decimal_places <= 0) { // can't edit shrinking digits (as of 110604)
		// a digit -- updated on 270202 to work in any base
		user_did(INTEGER,INTEGERS_TYPED,by);
		if (blank) {
			set_long_value(digit_value);
			become_non_blank();
//			first_key_since_selection = FALSE; // noticed obsolete on 080205
		} else if (robot_selected) { // or was when robot was trained
			set_long_value(digit_value); // believe this is obsolete -- only notebooks use robot_selected now (noticed 270704)
		} else {
			if ((number_format == TT_BIGNUM_DECIMAL && key == '0') || // zero will be lost so buffer it
				 !keyboard_buffer_empty()) { // already buffering
				add_to_keyboard_buffer(key,extended);
				if (key != '0') { // done buffering since is a digit but is not zero
					send_keyboard_buffer();
				};
			} else if (number_format == TT_LONG || number_format == TT_BIGNUM) {
							// && (!held() || tt_log_version_number < 45) // extra conditions new on 080604
				LongNumberValue radix(base);
				set_value(value->do_operation(MULTIPLY_BY,&radix));
//				LongNumberValue keystroke((value >= 0)?key-'0':'0'-key);
				LongNumberValue keystroke((value >= 0)?digit_value:base-digit_value);
				if (sign() < 0) {
					set_value(value->do_operation(SUBTRACT_FROM,&keystroke)); // new on 100202
				} else {
					set_value(value->do_operation(INCREASE_BY,&keystroke));
				};
			} else {
				if (text_length+1 > text_max_length) {
					need_text_of_length(text_length+extra_space,TRUE);
				};
				narrow_text[text_length] = key;
				narrow_text[text_length+1] = '\0';
				//if (!held() || tt_log_version_number < 45) { // new on 080604
					set_value_as_string(narrow_text,text_length+1,number_format);
				//} else {
				//	text_length++;
				//};
			};
		};
	} else if (key == '-') {
		if (is_an_operation_with_no_value()) { // changing the operation -- new on 280202
			set_operation(SUBTRACT_FROM);
		} else {
			negate();
		};
	} else if (key == '~') {
		NumberValue *new_value = value->copy(); // rewritten on 030302 to use set_value
		if (new_value->complement()) { // made a conditional on 130202 since doesn't always make sense
			set_value(new_value);
		} else { // else warn or beep or return FALSE?
			delete new_value;
		};
//		if (value->complement()) {
//			need_to_recompute_text();
//		};
	} else if (key == '+') { // new on 130202 - at least this is how things used to work where + removes operations
		if (is_an_operation_with_no_value()) { // changing the operation -- new on 280202
			set_operation(INCREASE_BY);
		} else {
			set_operation(NO_NUMBER_OPERATION);
		};
	} else if (valid_operation_key(key)) {
		set_operation(operation_from_key(key));
	} else if (key == '.' && (number_format == TT_LONG || number_format == TT_BIGNUM) && keyboard_buffer_empty()) {  
		// or turning integer into a decimal
		if (!add_to_keyboard_buffer(key,extended)) {
			record_action = FALSE; // new on 270804
		};
	} else { // new on 190201 to ignore invalid keystrokes to numbers
		// make noise too??
		record_action = FALSE;
	};
	if ((!by->can_send_keyboard_editing_keys() || reachable_from_remote_looks(original_recipient,this)) && decimal_places <= 0) { 
		// !original_recipient->held() || removed on 100604 
		// added decimal places test on 110604 -- added original_recipient test on 300604
		need_to_recompute_text(size_constrained_regardless); // new on 281002
	};
	return(TRUE);
};

void Rational::set_edited(boolean new_value) { // new on 071104
#if TT_SHRINKING_AND_GROWING
	if (!edited && new_value) { // just started editing
		// new on 011204
		// *2 misses some due to pixel round off
		if (character_height*1.8 <= height) { // was displaying as a multiple lines
			set_character_height(height);
		};
	};
#endif
	edited = (flag) new_value;
	if (edited && !size_constrained()) { // added check if size constrained on 011204
		change_font_size_to_fit_extent_flag = FALSE; // don't do this while editing
	};
};

void Rational::canonicalize() { // abstracted 100604
	// added default_number_format() on 091104 otherwise floating point numbers turn into strange rationals and misbehave
	NumberFormat format;
	if (value != NULL && value->default_number_format() == TT_DOUBLE_FLOAT) {
		// only this one since can edit one format to another except this
		format = TT_DOUBLE_FLOAT;
	} else {
		format = TT_UNDETERMINED_NUMBER_FORMAT;
	};
#if TT_SHRINKING_AND_GROWING
	// compute this here in case narrow_text changes below
	boolean need_to_restore_font_height = 
		           (following_operation == NULL && operation == NO_NUMBER_OPERATION && strchr(narrow_text,'/') != NULL);
#endif
	if (!set_value_as_string(narrow_text,text_length,format) && showing_on_a_screen()) {
		play_sound(PLOP_SOUND);
	} else if (operation == NO_NUMBER_OPERATION && following_operation != NULL) { 
		// new on 110604 -- if part of an operation sequence then be explicit about addition and subtraction
		// updated on 211104 so this happens only if the set_value_as_string succeeded
		if (sign() >= 0) {
			operation = INCREASE_BY;
		} else {
			operation = SUBTRACT_FROM;
			negate();
		};
	} else if (operation == COMPLEMENT) { // new on 040205 -- handled this way since is a unary operation
		value->complement();
		operation = NO_NUMBER_OPERATION;
	}; 
#if TT_SHRINKING_AND_GROWING
	if (need_to_restore_font_height) {
		// new on 011204 to restore font height if multiple lines
		if (leader == NULL) {
//			set_character_height(character_height/2);
			set_to_good_size(tt_system_mode==PUZZLE?NULL:tt_toolbox); // just as if Pumpy used
			set_change_size_to_fit_extent(FALSE);
		//} else {
		//	set_change_font_size_to_fit_extent(TRUE);
		};
		need_to_recompute_text(TRUE); // size OK already
	} else
#endif
	need_to_recompute_text(FALSE);
	set_edited(FALSE);
	keyboard_session_start = -1; // reset this 090804
	insertion_point = min_long; // reset this 090804
};

Sprite *Rational::released(Sprite *by, boolean top_level, SpecialDrop special_drop) { // new on 080604
	// not clear about Number vs. Rational but only here do we have access to the narrow text
	if (edited) {
		canonicalize();
	};
	return(Number::released(by,top_level,special_drop));
};

void Rational::negate() {
//	value->negate(); 
//	need_to_recompute_text();
	// re-written on 030302 so that set_value is called (and cleaner)
	LongNumberValue zero(0);
	set_value(zero.do_operation(SUBTRACT_FROM,value));
};

int Rational::sign() {
	return(value->sign());
};

boolean Rational::add_to_keyboard_buffer(char key, boolean extended) {
	// beginning 270804 this returns FALSE if key is ignored
	// commented out on 270704 since this misbehaved when number is rubbed out completely (0 length) 
	//if (text_length < 1) {
	//	long ignore_text_length; // re-organize the code so this isn't necessary??
	//	wide_string ignore_text;
	//	current_wide_text(ignore_text,ignore_text_length);
	//	if (wide_text == NULL) { // new on 150604 (Joe999's bug) to skip this - e.g. typing to a textual sensor
	//		return;
	//	};
	//};
	if (extended) {
		if (arrow_key(key)) {
			if (insertion_point < 0) { // not yet initialized
				insertion_point = text_length;
			};
			switch (key) { 
				case VK_LEFT:
					insertion_point = max(0,insertion_point-1);
					return(TRUE);
				case VK_RIGHT:
					insertion_point = min(text_length,insertion_point+1);
					return(TRUE);
				case VK_UP:
				case VK_DOWN:
					play_sound(PLOP_SOUND);
					break;
			};
		};
		// rewritten on 110604 so returns for any extended key - not just arrow keys here
		return(FALSE);
	};
	if (text_length+1 >= text_max_length) { 
		// changed from > to >= on 130904 in case user types lots of digits (or junk) before dropping
		need_text_of_length(text_length+extra_space,TRUE);
	};
	if (keyboard_session_start < 0) {
		keyboard_session_start = text_length;
	};
	int new_character_index;
	if (insertion_point >= 0) {
		for (int i = text_length-1; i >= insertion_point; i--) { // shift everything over
			narrow_text[i+1] = narrow_text[i];
			wide_text[i+1] = wide_text[i];
		};
		new_character_index = insertion_point;
		insertion_point++;
	} else {
		new_character_index = text_length;
	};
	int new_text_length = text_length+1;
	need_text_of_length(new_text_length+1); 
	// new as of 070804 since this happens if you vacuum out a numeric sensor from a robot's thought bubble -- 
	// yes but now blanks are handled differently so this maybe didn't help
	// and then type to it
	narrow_text[new_character_index] = key;
	wide_text[new_character_index] = key;
	narrow_text[new_text_length] = '\0';
	wide_text[new_text_length] = '\0';
	need_to_recompute_text(); // need to change size or font size 
	text_length = new_text_length; // no need (or desire to compute the true value just now)
	return(TRUE);
};


	/* old way
	if (keyboard_buffer == NULL) {
		keyboard_buffer = new char[2];
		keyboard_buffer[0] = key;
		keyboard_buffer[1] = '\0'; // terminate
	} else {
		int old_length = strlen(keyboard_buffer);
		string new_keyboard_buffer = new char[old_length+2]; // 1 for new key and 1 for terminator
		for (int i = 0; i < old_length; i++) {
			new_keyboard_buffer[i] = keyboard_buffer[i];
		};
		new_keyboard_buffer[old_length] = key;
		new_keyboard_buffer[old_length+1] = '\0';
		delete [] keyboard_buffer;
		keyboard_buffer = new_keyboard_buffer;
	};
*/

void Rational::unselect() {
//	if (keyboard_buffer != NULL) {
//		delete [] keyboard_buffer;
//		keyboard_buffer = NULL;
//	};
	if (edited) { // new on 100604
		canonicalize(); // how does this interact with the following?
	};
	if (keyboard_session_start >= 0) {
		int new_text_length = keyboard_session_start;
		narrow_text[new_text_length] = '\0';
		wide_text[new_text_length] = '\0';
		need_to_recompute_text(); // need to change size or font size
		text_length = new_text_length; // no need (or desire to compute the true value just now)
		keyboard_session_start = -1;
	};
	Number::unselect();	
};

void Rational::send_keyboard_buffer() {
	if (keyboard_session_start >= 0) {
		keyboard_session_start = -1;
		set_value_as_string(narrow_text,text_length);
	};
};
/*
	if (keyboard_buffer != NULL) { // could ignore keybaord_buffer if all 0s
		int buffer_length = strlen(keyboard_buffer);
		if (text_length+buffer_length > text_max_length) {
			need_text_of_length(text_length+buffer_length+extra_space,TRUE);
		};
		memcpy(narrow_text+text_length,keyboard_buffer,buffer_length);
		set_value(narrow_text,text_length+buffer_length,(keyboard_buffer[0]=='.')?TT_BIGNUM_DECIMAL:number_format);
		delete [] keyboard_buffer;
		keyboard_buffer = NULL;
	};
*/

boolean Rational::current_value(NumberValuePointer &the_value, boolean &shared, Sprite *by) {
	the_value = value;
	shared = TRUE;
	return(!is_blank()); // used to return FALSE with blank without setting values - not good for current_long_value
};

//void Rational::become(Sprite *other) { // noticed obsolete on 140205
//	if (other == NULL || other->kind_of() != INTEGER) return; 
//	if (other->is_blank()) {
//		become_blank();
//	} else {
//		set_value(((Rational *) other)->pointer_to_value()->copy());
//	};
//};

void Rational::need_text_of_length(int needed_text_length, boolean copy_old_narrow_text) {
	if (needed_text_length > text_max_length) { 
#if TT_DEBUG_ON
		if (tt_debug_target == debug_counter) {
			log("Running need_text_of_length of debug target.");
		};
#endif
		text_max_length = needed_text_length;
		string new_narrow_text = new char[text_max_length*2]; // *2 an experiment on 040503
		wide_string new_wide_text = new wide_character[text_max_length*2]; // *2 an experiment on 040503
		if (copy_old_narrow_text && text_length > 0) {
			memcpy(new_narrow_text,narrow_text,text_length+1);
//			memcpy((string) new_wide_text, (string) wide_text, (text_length+1)*sizeof(wide_character)); // no need for it
		};
		if (narrow_text != NULL) delete [] narrow_text;
		if (wide_text != NULL) delete [] wide_text;
		narrow_text = new_narrow_text;
		wide_text = new_wide_text;
	};
};

int insert_decimal_point(string destination, int offset, int total_digits, int decimal_places) {
	int shift = decimal_places-total_digits; // removed offset+ on 130202
	if (shift > 1) { // need some leading zeros after the decimal point
		for (int i = offset+total_digits; i >= 0; i--) {
			destination[i+shift-1] = destination[i];
		};									
		for (int j = 1; j < shift; j++) {
			destination[j+offset] = '0';
		};
	};
	destination[offset] = '.'; // moved to after shift on 250203 to deal with operations like *.025
	if (shift < 0) shift = 0; // not clear why this happens - if it still does??
//	numerator_text = mpz_get_str(narrow_text+offset+shift,10,temp);
//	int true_total_digits = strlen(numerator_text);
	return(offset+shift+total_digits);
};

static boolean gave_big_number_slow_warning = FALSE; // new on 040904

boolean Rational::current_wide_text(wide_string &the_text, long &the_text_length) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Running current_wide_text of debug target.");
		if (text_length < 0) {
			log("about to recompute wide text of debug target");
		};
	};
#endif
	if (blank) return(FALSE);
	if (value == NULL) { // new on 060505
      value = new LongNumberValue(0);
   };
	if (text_length < 0 || narrow_text == NULL) { // needs to be computed
		// added narrow_text == NULL test for robustness on 051004 since happened during time travel
		set_clean_status(DIRTY); // new on 121202
		decimal_places = 0; // just in case it used to be... -- moved here on 080803 so applies more generally
		long long_value;
		bignum_pointer bignum_value;
		double double_float_value;
		if (is_an_operation_with_no_value()) { // new on 250202
			need_text_of_length(maximum_operator_text_size); // new on 270202
			text_length = add_operations_to_string(narrow_text);
		} else if (value->long_value(long_value,FALSE)) { 
			int total_text_size = maximum_integer_text_size+following_operations_string_length(); 
			if (total_text_size > text_max_length) {
				need_text_of_length(total_text_size);
			};
			update_narrow_text(long_value,narrow_text,maximum_integer_text_size);
			number_format = TT_LONG;
		} else { // restructured a bit on 130505
			boolean have_a_bignum_value = FALSE;
#if TT_CORRECT_IMPLEMENTATION_OF_DISPLAY_X_AND_Y_AS_INTEGERS
			bignum my_value_as_bignum;
			boolean need_to_free_my_value_as_bignum = FALSE;
#endif
			if (!value->bignum_value(bignum_value)) {
#if TT_CORRECT_IMPLEMENTATION_OF_DISPLAY_X_AND_Y_AS_INTEGERS
				// new on 130505
				if (display_as_integer()) {
					mpz_init(my_value_as_bignum);
					if (value->round_to_nearest_bignum_value(my_value_as_bignum)) {
						bignum_value = (bignum_pointer) &my_value_as_bignum;
						need_to_free_my_value_as_bignum = TRUE;
						have_a_bignum_value = TRUE;
					};
				};
#endif
			} else {
				have_a_bignum_value = TRUE;
			};
			if (have_a_bignum_value) {
				int total_text_size = mpz_sizeinbase(bignum_value,base)+1+operation_string_length()+
					                   following_operations_string_length(); 
				// and extra in case negative
				if (total_text_size > text_max_length) { 
					need_text_of_length(total_text_size+extra_space); // while allocating get some extra space
				};
				int offset = 0;
				if (operation != NO_NUMBER_OPERATION) {
					offset = add_operations_to_string(narrow_text);
				};
				if (base != 10) {
					itoa(base,narrow_text+offset,10);
					offset += strlen(narrow_text+offset);
					narrow_text[offset++] = '#'; // radix indicator
				};
				if (total_text_size > 5000 && base != 2 && base != 4 && base != 8 && base != 16 && base != 32 && 
					 !gave_big_number_slow_warning) { 
					// && !did_user(TROUBLE_SHOOT_BIG_NUMBERS_SLOW,TROUBLE_SHOOT_HISTORY)) { // new on 270202
					// bases that are powers of 2 are converted to strings very quickly 
					// added last condition on 010904 so don't repeatedly try to produce this warning
					UnwindProtect<boolean> save(tt_err_file_important); // new on 081104 so this doesn't update tt_err_file_important
					trouble_shoot(TROUBLE_SHOOT_BIG_NUMBERS_SLOW,NULL,NULL,TRUE);
					tt_error_file() << "The number has " << total_text_size << " digits!" << endl; // new on 081104
					gave_big_number_slow_warning = TRUE;
					tt_err_file_important_but_no_error = !replaying(); // was TRUE prior to 150505
				};
				bignum_to_string(narrow_text+offset,base,bignum_value,total_text_size);
				text_length = strlen(narrow_text); // there are no fractions 
				number_format = TT_BIGNUM;
#if TT_CORRECT_IMPLEMENTATION_OF_DISPLAY_X_AND_Y_AS_INTEGERS
				if (need_to_free_my_value_as_bignum) {
					mpz_clear(my_value_as_bignum);
				};
#endif
			} else if (value->double_float_value(double_float_value,FALSE)) { // new on 130202
				int total_text_size = maximum_double_flat_text_size+following_operations_string_length(); 
				if (total_text_size > text_max_length) {
					need_text_of_length(total_text_size);
				};
				// rewritten on 050305 since didn't handle properly numbers like 1.234 10^-100
				if (operation != NO_NUMBER_OPERATION) { // was '+' prior to 100202
					strcpy(narrow_text,operation_to_string(operation));
				} else {
					narrow_text[0] = '\0';
				};
				int decimal, sign;
				string buffer = _ecvt(double_float_value, 17, &decimal, &sign ); 
				// 17 digits seems to be minimum http://www.cs.indiana.edu/~burger/FP-Printing-PLDI96.pdf
				if (sign != 0) {
					strcat(narrow_text,"-");
				};
				if (decimal < 0) { // need some leading zeros
					strcat(narrow_text,"0.");
					int offset = strlen(narrow_text);
					int count = -decimal;
					for (int i = 0; i < count; i++) {
						narrow_text[i+offset] = '0';
					};
					narrow_text[count+offset] = '\0';
					strcat(narrow_text,buffer);
				} else {
					int buffer_length = strlen(buffer);
					if (decimal > buffer_length) {
						strcat(narrow_text,buffer);
						int offset = strlen(narrow_text);
						int count = decimal-buffer_length;
						for (int i = 0; i < count; i++) {
							narrow_text[i+offset] = '0';
						};
						narrow_text[count+offset] = '\0';
						strcat(narrow_text,".0"); // so ends with .0 to hint at type
					} else {
						int offset = strlen(narrow_text);
						memcpy(narrow_text+offset,buffer,decimal);
						narrow_text[offset+decimal] = '.';
						strcpy(narrow_text+offset+decimal+1,buffer+decimal);
					};
				};
				text_length = strlen(narrow_text);
				//output_string_stream text_stream(narrow_text,total_text_size);
				// was 14 prior to 110102 -- not good for operator with negative billion
				//if (operation != NO_NUMBER_OPERATION) { // was '+' prior to 100202
				//	text_stream << operation_to_string(operation);
				//};
				//text_stream << std::setprecision(DBL_DIG) << std::setiosflags(std::ios_base::fixed) << double_float_value; 
				//text_length = text_stream.LENGTH;
				number_format = TT_DOUBLE_FLOAT;			
			} else {
				rational_pointer rational_value;
				if (value->rational_value(rational_value)) { // should not fail 
					int numerator_text_size = mpz_sizeinbase(mpq_numref(rational_value),base); // base 10 
					int estimated_denominator_text_size = mpz_sizeinbase(mpq_denref(rational_value),base);
					// following was +6 prior to 111104 but now that we are producing 0.5 best to add one just to be sure
					int total_text_size 
						= numerator_text_size+estimated_denominator_text_size+7+operation_string_length()+
						  following_operations_string_length();
					//  extra for space, decimal point, and slash (though can't have all 3) and terminator
					if (total_text_size < min_text_length) { // new on 060803
						total_text_size = min_text_length;
					};
					if (total_text_size > 5000 && base != 2 && base != 4 && base != 8 && base != 16 && base != 32) { 
						// new on 060302
						// bases that are powers of 2 are converted to strings very quickly
						UnwindProtect<boolean> save(tt_err_file_important); 
						// new on 081104 so this doesn't update tt_err_file_important
						trouble_shoot(TROUBLE_SHOOT_BIG_NUMBERS_SLOW,NULL,NULL,TRUE);
						gave_big_number_slow_warning = TRUE; // was missing prior to 081104
						tt_error_file() << "The number has " << total_text_size << " digits!" << endl; // new on 081104
						tt_err_file_important_but_no_error = !replaying(); // new on 081104 // was TRUE prior to 150505
					};
					if (total_text_size > text_max_length) {
						need_text_of_length(total_text_size+extra_space); // while allocating get extra_space
					};
					int offset = 0;
					if (operation != NO_NUMBER_OPERATION) {
						offset = add_operations_to_string(narrow_text);
					};
					if (base != 10) {
						itoa(base,narrow_text+offset,10);
						offset += strlen(narrow_text+offset);
						narrow_text[offset++] = '#'; // radix indicator
					};
					string numerator_text = bignum_to_string(narrow_text+offset,base,mpq_numref(rational_value),numerator_text_size);
					int numerator_text_length = strlen(numerator_text); // why was it +offset ?
					if (mpz_sgn(mpq_numref(rational_value)) < 0) { // negative - new on 040202
						// so treat the sign that mpz_get_str generated as if it were an operator and not part of the string
						offset++;
						numerator_text_length--; // don't count the minus sign
						numerator_text++; // hop over the minus sign
					};
					narrow_text[numerator_text_length+offset] = '/';
					string denominator_text = bignum_to_string(narrow_text+numerator_text_length+offset+1,base,
						                                        mpq_denref(rational_value),
																			 estimated_denominator_text_size);
					int denominator_text_size = strlen(denominator_text);
					string denominator_text_copy = NULL;
					if (integer_and_fraction_format) {
						denominator_text_copy = copy_string(denominator_text,denominator_text_size); 
						// since it shares memory with narrow_text it would be clobbered otherwise
					};
					text_length = offset+numerator_text_length+denominator_text_size+1;
					boolean computed_alternate_format = FALSE;
					boolean greater_than_one;
					boolean know_if_greater_than_one = FALSE; // this is so only gets computed if needed and then only once
					if ((decimal_notation_if_denominator_is_power_of_ten || shrinking_decimal_notation()) && base == 10) {
						// check if the rest is all zeros
						boolean all_zeros = TRUE; // unless shown otherwise below
						for (int i = 1; i < denominator_text_size; i++) {
							if (denominator_text[i] != '0') {
								all_zeros = FALSE;
								break;
							};
						};
						if (all_zeros && denominator_text[0] == '1') { // shift number to right
							int decimal_places_needed = denominator_text_size; 
							// e.g. 100 has 3 characters and has 2 decimal places plus 1 for decimal point
							greater_than_one = (mpz_cmpabs(mpq_numref(rational_value),mpq_denref(rational_value)) > 0);
							know_if_greater_than_one = TRUE;
							if (!greater_than_one) {
								int i = numerator_text_length-1;
								int shift = decimal_places_needed-numerator_text_length;
								//int shift = decimal_places-i; // rewritten on 230603 so i+shift can never be negative
								for (; i >= 0; i--) { // shift to right -- was >= offset
									numerator_text[i+shift] = numerator_text[i]; // there always should be enough room for this
								};
								i = 0; // was offset prior to 130202 but dealing with numerator_text not narrow_text ...
								numerator_text[i++] = '.'; // what about languages where it is a comma??
								for (; i < shift; i++) { // was shift+offset
									numerator_text[i] = '0'; // leading zeros
								};
								text_length = offset+decimal_places_needed;
							} else { // number is greater than 1
								int i = numerator_text_length-1;
								for (; i > 0; i--) { // was > offset
									if (decimal_places_needed > 1) {
										numerator_text[i+1] = numerator_text[i];
										decimal_places_needed--;
									} else {
										break;
									};
								};
								numerator_text[i+1] = '.'; // what about languages where it is a comma??
								text_length = offset+numerator_text_length+1;
							};
							number_format = TT_BIGNUM_DECIMAL;
							computed_alternate_format = TRUE;
						} else { // see if denominator factors only into 2s and 5s
							bignum two,five,temp;
							mpz_init(temp);
							mpz_init(five);
							mpz_init(two);
							mpz_set_ui(two,2);
							mpz_set_ui(five,5);
							unsigned long number_of_fives = mpz_remove(temp,mpq_denref(rational_value),five);
							mpz_clear(five);
							unsigned long number_of_twos = mpz_remove(temp,temp,two);
							mpz_clear(two);
							if (mpz_cmpabs_ui(temp,1) == 0) { // didn't have any other factors
								if (number_of_fives > number_of_twos) {
									mpz_ui_pow_ui(temp,2,number_of_fives-number_of_twos);
								} else { // shouldn't be equal since that should have been dealt with above
									mpz_ui_pow_ui(temp,5,number_of_twos-number_of_fives);
								};
								// temp is now the factor to multiply the numerator by
	//							int extra_digits = mpz_sizeinbase(temp,10);
								mpz_mul(temp,mpq_numref(rational_value),temp);
								mpz_abs(temp,temp); // new on 040202 since offset already has the '-' sign
								// temp is now the exact decimal expansion 
	//							int decimal_places = extra_digits+estimated_denominator_text_size-1;
								int decimal_places = max(number_of_fives,number_of_twos)+1; 
								// since made equal this counts the number of 10s -- +1 to include decimal point
	//							int estimated_total_digits = mpz_sizeinbase(temp,10); // might be one to large
								if (!know_if_greater_than_one) {
									greater_than_one = (mpz_cmpabs(mpq_numref(rational_value),mpq_denref(rational_value)) > 0);
									know_if_greater_than_one = TRUE;
								};
								if (greater_than_one) {
									int text_size_estimate = mpz_sizeinbase(temp,base)+offset+1; // new on 050702
									if (text_size_estimate > text_max_length) {
										string prefix = NULL;
										if (offset > 0) {
											prefix = new char[offset]; // save prefix
											memcpy(prefix,narrow_text,offset);
										};
										// extra_space isn't just a nice policy to avoid recomputation but seems to sometimes be needed
										// not clear why - happens in Space Invaders game
										need_text_of_length(text_size_estimate+extra_space);
										if (offset > 0) {
											memcpy(narrow_text,prefix,offset);
											delete [] prefix;
										};
									};
									numerator_text = bignum_to_string(narrow_text+offset,base,temp,text_size_estimate);
									int numerator_text_length = strlen(numerator_text);
									int i = numerator_text_length-1;
									for (; i > 0; i--) {
										if (decimal_places > 1) {
											numerator_text[i+1] = numerator_text[i]; 
											decimal_places--;
										} else {
											break;
										};
									};
	//								if (numerator_text_length < estimated_total_digits) { 
									// estimate was off by one -- commented out on 100202
	//									numerator_text[1] = numerator_text[0];
	//									numerator_text[0] = '.';
	//								} else {
										numerator_text[i+1] = '.'; // what about languages where it is a comma??
	//								};
									text_length = offset+numerator_text_length+1;
								} else { // less than 1
									int text_size_estimate = mpz_sizeinbase(temp,base)+offset+1; // new on 050702
									if (text_size_estimate > text_max_length) {
										string prefix = NULL;
										if (offset > 0) {
											prefix = new char[offset]; // save prefix
											memcpy(prefix,narrow_text,offset);
										};
										need_text_of_length(text_size_estimate+extra_space);
										if (offset > 0) {
											memcpy(narrow_text,prefix,offset);
											delete [] prefix;
										};
									};
									numerator_text = bignum_to_string(narrow_text+offset+1,base,temp,text_size_estimate); 
									// might be wrong place but will get exact length (and will be lucky if greater than .1)
									text_length = insert_decimal_point(narrow_text,offset,strlen(numerator_text),decimal_places);
	//								if (true_total_digits < estimated_total_digits) { 
									// should only be off by 1 according documentation of mpz_sizeinbase
	//									// so need to shift one extra
	//									int i = shift+true_total_digits-1; // was offset+
	//									for (; i >= 0; i--) {
	//										numerator_text[i+1] = numerator_text[i]; 
	//										true_total_digits--;
	//										if (true_total_digits == 0) break;
	//									};
	//									numerator_text[i] = '0'; // insert an extra zero
	//								};
								};
								number_format = TT_BIGNUM_DECIMAL;
								computed_alternate_format = TRUE;
							};
							mpz_clear(temp);
						};
						if (shrinking_decimal_notation() && !computed_alternate_format) { // new on 230603
							// decimal_places indicates how much room there is after shrinking the digits 
							// so compute the expansion to that many places
							// multiply by 10**decimal_places and then insert the decimal point at the right place
							bignum multiplier,numerator_times_multiplier,value_times_multiplier;
							mpz_init(multiplier);
							mpz_init(numerator_times_multiplier);
							mpz_init(value_times_multiplier);
	//						decimal_places = max(1,shrinking_decimal_places(character_width,character_height)); // at least 1
							compute_shrinking_decimal_places(); // moved here so text has been computed as well as updated on 080803
							mpz_ui_pow_ui(multiplier,base,decimal_places);
							mpz_mul(numerator_times_multiplier,mpq_numref(rational_value),multiplier);
							mpz_clear(multiplier);
							mpz_tdiv_q(value_times_multiplier,numerator_times_multiplier,mpq_denref(rational_value));
							mpz_clear(numerator_times_multiplier);
							mpz_abs(value_times_multiplier,value_times_multiplier); 
							// sign already dealt with earlier - could not bother if positive...
							total_text_size = offset+1+mpz_sizeinbase(value_times_multiplier,base);
							if (total_text_size >= text_max_length) { // was just > prior to 251104 but 10^25000/3333 caused problems
								narrow_text[offset] = '\0'; // terminate it before copying it to the new longer
								need_text_of_length(total_text_size+extra_space,TRUE); // while allocating get extra_space
							};
							numerator_text = bignum_to_string(narrow_text+offset+1,base,value_times_multiplier,total_text_size); 
							// +1 to offset to leave room for decimal point
							mpz_clear(value_times_multiplier);
							// now we need to insert the decimal point
							int total_length = strlen(numerator_text);
							int before_decimal_point = total_length-decimal_places;
							if (before_decimal_point < 0) {
								text_length = insert_decimal_point(narrow_text,offset,total_length,decimal_places+1); 
								// not sure why +1 works but it does
							} else {					
								if (before_decimal_point > 0) { // no need to bother otherwise
									for (int i = 0; i < before_decimal_point; i++) {
										narrow_text[i+offset] = narrow_text[i+offset+1];
									};
								};
								narrow_text[before_decimal_point+offset] = '.';
								text_length = offset+total_length+1;
							};
							number_format = TT_BIGNUM_DECIMAL;
							computed_alternate_format = TRUE;
	//						display_as_shrinking_decimal = TRUE;
						};
					};
					if (!computed_alternate_format && integer_and_fraction_format) {
						if (!know_if_greater_than_one) {
							greater_than_one = (mpz_cmpabs(mpq_numref(rational_value),mpq_denref(rational_value)) > 0);
							know_if_greater_than_one = TRUE;
						};
						if (greater_than_one) { // is greater than one
							// so turn numbers like 8/3 intor 2 2/3
							bignum quotient, remainder;
							mpz_init(quotient);
							mpz_init(remainder);
							mpz_abs(quotient,mpq_numref(rational_value)); // new on 080202 - just reusing quotient for a sec
							mpz_tdiv_qr(quotient,remainder,quotient,mpq_denref(rational_value));
							int total_length = mpz_sizeinbase(quotient,base)+mpz_sizeinbase(remainder,base)
													 +mpz_sizeinbase(mpq_denref(rational_value),base)+3;
							if (total_length > text_max_length) {
								need_text_of_length(total_length+extra_space);
							};
							string quotient_string = bignum_to_string(narrow_text+offset,base,quotient,total_length);
							int quotient_length = strlen(quotient_string);
							narrow_text[offset+quotient_length] = ' ';
							string remainder_string = bignum_to_string(narrow_text+offset+quotient_length+1,base,remainder,
								                                        quotient_length);
							int reminder_length = strlen(remainder_string);
							narrow_text[offset+quotient_length+1+reminder_length] = '/';
							memcpy(narrow_text+offset+quotient_length+1+reminder_length+1,denominator_text_copy,denominator_text_size);
							text_length = offset+quotient_length+1+reminder_length+1+denominator_text_size;
							// what if text_length is longer than the one allocated for above? do same trick as 050702? Or is too late?
							mpz_clear(quotient);
							mpz_clear(remainder);
							number_format = TT_BIGNUM_WITH_FRACTION;
							computed_alternate_format = TRUE;
						};			
					};
					if (denominator_text_copy != NULL) delete [] denominator_text_copy;
					if (!computed_alternate_format) {
						number_format = TT_RATIONAL;
					};
				};
			};
		};
#if TT_SHRINKING_AND_GROWING
		if (tt_log_version_number >= 58 && narrow_text[0] == '.') {
			// insert a 0 to follow the world-wide convention that .5 is shown as 0.5
			for (int i = text_length-1; i >= 0; i--) {
				narrow_text[i+1] = narrow_text[i];
			};
			narrow_text[0] = '0';
			text_length++;
		} else if (tt_log_version_number >= 76 && narrow_text[0] == '-' && narrow_text[1] == '.') {
			// insert a 0 to follow the world-wide convention that -.5 is shown as -0.5 (new on 280505)
			for (int i = text_length-1; i >= 1; i--) {
				narrow_text[i+1] = narrow_text[i];
			};
			narrow_text[1] = '0';
			text_length++;
		};
#endif
		if (following_operation != NULL) { // new on 100202
			string following_operation_text;
			long following_operation_text_length;
			if (following_operation->current_text(following_operation_text,following_operation_text_length)) {
				narrow_text[text_length++] = ';'; // space and semi colon between operations
				narrow_text[text_length++] = ' ';
				memcpy(narrow_text+text_length,following_operation_text,following_operation_text_length);
				text_length += following_operation_text_length;
//				narrow_text[text_length] = '\0'; // terminate the string
			};
		};
	};
	narrow_text[text_length] = '\0'; // terminate the string -- moved here on 040702
	// following new on 060803 so that everything respects minimun text length
	int gap = 0;
	if (decimal_places == 0) { // and not shrinking (new on 110803)
		gap = max(0,min_text_length-text_length); // should be negative
	};
	if (gap > 0) { 
		for (int i = 0; i < gap; i++) {
			wide_text[i] = ' '; // prepend spaces
		};
	};
	copy_wide_string(narrow_text,text_length,wide_text+gap);
	text_length += gap; // include the spaces added
	if (gap > 0) { // new on 120803 to update narrow text as well with preceeding spaces (to left justify)
		copy_narrow_string(wide_text,text_length,narrow_text,text_length);
	};
	the_text = wide_text;
//#if TT_DEBUG_ON
//	tt_error_file() << narrow_text << endl;
//#endif
	the_text_length = text_length;
//	insertion_point = text_length; // new on 090804
	return(the_text != NULL); // was TRUE prior to 241104 but this is safer since caller won't assume it has some text
};

#if TT_SHRINKING_AND_GROWING
int Number::non_shrinking_or_integer_part_and_fraction_equivalent(wide_string wide_text, int character_count) {
	if (decimal_places <= 0) {
		if (edited) { // new on 011204 to treat as ordinary string while being edited
			return(character_count);
		};
		// new on 131104 to figure out equivalent for integer part followed by fraction
		int separator_index = find_separator_location(wide_text,character_count,L"./");
		if (separator_index > 0) { 
			int space_index = find_separator_location(wide_text,separator_index,L" ");
			//if (space_index < 0 && operation != NO_NUMBER_OPERATION && following_operation == NULL) {
			//	// new on 011204 to deal with operations like /3/2 better
			//	space_index = 0; // right after operation -- always correct??
			//};
			if (space_index > 0) {
				int numerator_length = separator_index-(space_index+1);
				int denominator_length = character_count-(separator_index+1);
				int fraction_length = max(numerator_length,denominator_length);
				return(space_index+(1+fraction_length)/2); // +1 for space
			} else if (wide_text[separator_index] == '/' && tt_log_version_number >= 76) { 
				// new on 300505 to display 2D fractions better when created (if unconstrained)
				// condition narrowed on 030605
				int numerator_length = separator_index;
				int denominator_length = character_count-(separator_index+1);
				return(max(numerator_length,denominator_length));
			};
		};
		return(character_count);
	};
	return(non_shrinking_equivalent(character_count));
};

int Number::non_shrinking_equivalent(int character_count) {
	// could cache this
	if (decimal_places <= 0) {
		return(character_count);
	};
	double decimal_characters;
	if (part2_shrinkage < 1.0) { // && tt_shrink_and_grow_numbers) { // updated 061104 since why run the old expensive code?
		decimal_characters = 1.0/((1.0/part2_shrinkage)-1.0); 
		// assumes number_shrinkage is less than 1.0 -- new on 251004
		// and that the infinite sum good enough -- otherwise could subtract number_shrinkage^decimal_places or the like
	} else {
		// shouldn't really happen
		decimal_characters = part2_shrinkage;
	};
	return(max(1,(character_count-decimal_places)+(int) (decimal_characters+.5))); 
	// rounded to nearest integer
};

double Number::non_shrinking_growing_equivalent(int character_count, double shrinkage) {
	if (shrinkage <= 0.0 || shrinkage >= 1.0) {
		return(character_count);
	} else if (decimal_places <= 0) {
		// correct for fact that this isn't an infinite shrinkage but stops and grows (dots can complicate this) -- new on 051104
		return(2.0/((1.0/shrinkage)-1.0)+2-pow(shrinkage,character_count/2)*2.0); // since shrink and grow +2 since starts and ends full size
	} else {
		// was 0.6 for a while since that is the spacing now when shrinking decimals -- restored 061104
		return(1.0/((1.0/shrinkage)-1.0)+1); // +1 since starts full size
	};
};

#else

int Number::non_shrinking_equivalent(int character_count) {
	// could cache this
	if (decimal_places <= 0) return(character_count);
	double decimal_characters = number_shrinkage;
	double current_shrinkage = number_shrinkage;
	for (int i = 1; i < decimal_places; i++) {
		current_shrinkage *= number_shrinkage;
		decimal_characters += current_shrinkage;
	};
	return(max(1,(character_count-decimal_places)+(int) (decimal_characters+.5))); // round to nearest integer
};

#endif

boolean Rational::current_text(const_string &the_text, long &the_text_length) {
	if (blank) return(FALSE);
	if (text_length < 0) { // need to recompute it
		wide_string wide_text;
		long wide_text_length;
		current_wide_text(wide_text,wide_text_length);
	};
	the_text = narrow_text;
	the_text_length = text_length;
	return(TRUE);
};

void Rational::set_value(NumberValue *new_number_value,
							    boolean ignore_change,
							    boolean sizes_constrained,
							    boolean update) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Setting value of debug target (Rational)");
	};
	if (tt_debug_mode == 10106 && new_number_value != NULL && debug_counter == 254) {
		tt_error_file() << "At time " << tt_current_time << " setting value to ";
		tt_current_output_stream = NULL;
		new_number_value->print(tt_error_file());
		tt_error_file() << endl;
	};
//	tt_error_file() << "Setting value of #" << debug_counter << endl; // for now 290303
#endif
	set_clean_status(DIRTY); // new on 110703
	if (value != NULL) {
		delete value; 
	};
	value = new_number_value;
	if (!new_number_value->exact()) {
		decimal_places = 0; // new on 050803 since shrinking digit format inappropriate once inexact
	};
	need_to_recompute_text(sizes_constrained);
	keyboard_session_start = -1; // new on 200204 since if typing to a number that is being changed via its remote looks
	if (using_default_text_color) {
		if (text_color != tt_gray && !new_number_value->exact()) { 
			// inexact and color never explicitly set so is still "default_color" -- new on 130202
			set_color(tt_gray,TRUE);
		} else if (text_color != tt_black && new_number_value->exact()) { // new on 170202
			set_color(tt_black,TRUE);
		};
	};
	set_is_an_operation_with_no_value(FALSE); // new on 250202 -- seems better to have caller do this since rarely is needed
};

void Rational::dump_value(output_stream &stream) {
	boolean saved_blank = blank;
	blank = FALSE; // new on 030302 so it dumps default value properly
	long long_value;
	double double_float_value;
	boolean requires_special_handling = (is_an_operation_with_no_value() || return_base() != 10); // new on 270202
	if (value == NULL) { // new on 210203
		stream.put(NOTHING_MARKER);
	} else if (following_operation == NULL && value->long_value(long_value,FALSE) && !requires_special_handling) {
		stream.put((char) TT_LONG); // flag to indicate an integer
		stream.write((character *) &long_value, sizeof(long_value)); 
	} else if (value->double_float_value(double_float_value,FALSE) && !requires_special_handling) { // new on 130202
		// note that if a float is turned into an operation with no value then it becomes an ordinary value
		stream.put((char) TT_DOUBLE_FLOAT); // flag to indicate an integer
		stream.write((character *) &double_float_value, sizeof(double_float_value)); 
		if (following_operation == NULL) {
			stream.put(NOTHING_MARKER);
		} else {
			following_operation->dump(stream);
		};
	} else {
		// major rewrite on 050302 so that this is much faster and more compact (and shares code with dumping a picture's speed)
//		long the_text_length;
//		string text;
//		current_text(text,the_text_length);
		if (number_format == TT_LONG) { // is really an operation or else it would have been dumped above
			stream.put((char) TT_NEW_LONG); 
		} else {
#if TT_DEBUG_ON
			if (number_format >= TT_UNDETERMINED_NUMBER_FORMAT) {
				log("Dumping an incorrect number format.");
			};
#endif
			stream.put((char) number_format);
		};
		value->dump(stream);
//		stream.write((character *) &the_text_length, sizeof(the_text_length));
//		stream.write(text,the_text_length+1); // +1 for terminator
		byte flags = 0; // re-written on 250202 for clarity and to add OPERATION_WITH_NO_VALUE
		if (integer_and_fraction_format) flags |= INTEGER_AND_FRACTION_FORMAT;
		if (decimal_notation_if_denominator_is_power_of_ten) flags |= DECIMAL_NOTATION_IF_EXACT_FORMAT;
//		if (punctuate_every_three_digits) flags |= PUNCTUATE_EVERY_THREE_DIGITS_FORMAT; // not used -- could recycle this
		if (!exact()) flags |= INEXACT_NUMBER;
		if (using_default_text_color) flags |= USING_DEFAULT_COLOR_FORMAT;
		if (is_an_operation_with_no_value()) flags |= OPERATION_WITH_NO_VALUE;
		stream.put(flags);
//		stream.put((byte) ((integer_and_fraction_format?1:0)+(decimal_notation_if_denominator_is_power_of_ten?2:0)+
		// (punctuate_every_three_digits?4:0)+(!exact()?8:0)+(using_default_text_color?16:0))); // added exact and using_default_text_color on 130202
		stream.put((byte) base);
		if (following_operation == NULL) {
			stream.put(NOTHING_MARKER);
		} else {
			following_operation->dump(stream);
		};
	};
	blank = saved_blank;
};

#if TT_XML

// commented out on 120703 - xml_tags now does the equivalent
//xml_element *Rational::xml_create_element(xml_document *document) {
//   return(value->xml_create_element(document));
//};

void Rational::xml(xml_element *element, xml_document *document) {
	if (edited) {
		// new on 090604
		wide_string temp_wide_text = copy_wide_string(narrow_text,text_length);
		xml_text *text = create_xml_text(temp_wide_text,document);
		if (temp_wide_text != NULL) delete [] temp_wide_text;
		xml_append_child(text,element);
	} else {
		value->xml(element,document);
	};
	if (!integer_and_fraction_format) {
		xml_set_attribute(element,L"NotIntegerAndFractionFormat",1); // should this be a tag instead??
	};
	if (!decimal_notation_if_denominator_is_power_of_ten) {
		xml_set_attribute(element,L"NoDecimalNotation",1); // should this be a tag instead??
	};
	if (base != 10) {
		xml_set_attribute(element,L"Base",base); // should this be a tag instead?? -- must be between 2 and 36
	};
	if (number_format != TT_LONG) {
		xml_set_attribute(element,L"NumberFormatCode",(int) number_format);
	};
	// wide_text, narrow_text, text_max_length computed when needed
	Number::xml(element,document);
};

xml_element *Rational::xml_set_special_attributes(xml_element *element, xml_document *document) {
	// new on 191202 -- went through all the state variables
	xml_element *special_element = Number::xml_set_special_attributes(element,document);
	if (keyboard_session_start >= 0) {
		xml_set_attribute(special_element,L"KeyboardSessionStart",keyboard_session_start);
	};
	return(special_element);
};

boolean Rational::xml_get_special_attributes(xml_node *node) {
	keyboard_session_start = xml_get_attribute_int(node,L"KeyboardSessionStart",-1); // prior to 210404 keyboard_session_start
	return(Number::xml_get_special_attributes(node));
};

boolean Rational::handle_xml_tag(Tag tag, xml_node *node) {
#if TT_DEBUG_ON
	DWORD start;
	if (tt_debug_mode == 310303) {
		start = timeGetTime();
	};
#endif
   switch (tag) {  
		case NO_MORE_TAGS:
			integer_and_fraction_format = !xml_get_attribute_int(node,L"NotIntegerAndFractionFormat",0); 
			// prior to 210404 !integer_and_fraction_format
			decimal_notation_if_denominator_is_power_of_ten = !xml_get_attribute_int(node,L"NoDecimalNotation",0); 
			// prior to 210404 !decimal_notation_if_denominator_is_power_of_ten
			base = xml_get_attribute_int(node,L"Base",10); // prior to 210404 base
			number_format = (NumberFormat) xml_get_attribute_int(node,L"NumberFormatCode",TT_LONG); 
			// prior to 210404 (int) number_format
			// and the following
#if TT_DEBUG_ON
			if (tt_debug_mode == 310303) {
				tt_error_file() << (timeGetTime()-start) << " ms to process NO_MORE_TAGS in Rational::handle_xml_tag" << endl;
			};
#endif
      default:
         return(Number::handle_xml_tag(tag,node));
   };
};

#endif


string Rational::java_value_string() {
	if (value != NULL) {
		return(value->java_value_string());
	} else { // shouldn't happen - warn??
		return(NULL);
	}
};

void Rational::print_value(output_stream &message) { 
	// new on 281004 since this does a better job than below (e.g. 1/3 instead of .3333333)
	if (value != NULL) { // can it ever be otherwise?
		value->print(message);
	} else {
		Number::print_value(message);
	};
};

Remote_Integer::Remote_Integer(Picture *controlee,
										 RemoteIdentifier identifier,
										 city_coordinate x, city_coordinate y,
										 long priority,
										 city_coordinate width, city_coordinate height,
										 NumberValue *initial_value,
										 boolean active,
										 NumberOperation operation,
										 int min_text_length) :
	Rational(x,y,priority,width,height,initial_value,TT_LONG,operation,min_text_length),
	controlee(controlee),
	identifier(identifier),
	operations(NULL),
	controlee_deleted_blanked(FALSE),
	controlee_vacuum_blanked(FALSE),
//   different_floors_blanked(FALSE),
	display_as_double(!tt_x_and_y_display_as_integers),
	// version 2 new on 120504 so that ANCESTOR_REMOTE can work with both flipped and unflipped ancestors
	version(2) {
   //saved_by(NULL) {
//   temporarily_not_active_flag(FALSE) {
	set_active(active);
//	global_flag(FALSE), // by default not global like mouse speed
	if (controlee != NULL) {
		if (!controlee->add_remote(identifier,this)) {
			// new on 010899 to deal with legacy problems with joystick controls
			Picture *joystick_controllee = tt_joystick_pictures[0];
			if (identifier >= joystick_controllee->return_first_identifier() &&
				 identifier <= joystick_controllee->return_last_identifier()) {
				if (joystick_controllee->add_remote(identifier,this)) {
					controlee = joystick_controllee;
					tt_error_file() << "Fixed problem with bad remote identifier " << (int) identifier << endl; // new on 280202
				}; // else give up??
			};
		};
		// why was this conditionalized?
//		if (!global()) {//controlee != tt_dummy_picture) {
			// initialize value
		if (initial_value == NULL) { // none given
			if (speed_identifier(identifier)) { // new on 050302
				set_value(controlee->current_speed(identifier,TRUE));
#if !TT_CORRECT_IMPLEMENTATION_OF_DISPLAY_X_AND_Y_AS_INTEGERS
			} else if (display_as_double && double_float_identifier(identifier)) { // display_as_double new on 241104
				set_double_value(controlee->current_user_value_double(identifier));
#endif
			} else {
				set_long_value(controlee->current_user_value(identifier),TRUE);
			};
		};
	};
	read_only_flag = (flag) read_only_identifier(identifier);
	if (using_default_text_color) { // new on 040302
		text_color = tt_black;
		using_default_text_color = FALSE; // since sensors are always exact since "reality" matches its inexactness
	};
	if (value != NULL) {
		number_format = value->default_number_format(); // new on 020702
	};
};

//void Remote_Integer::prepare_for_deletion() {
//#if TT_DEBUG_ON
//   if (tt_debug_target == debug_counter) {
//      log("prepare_for_deletion of debug target (Remote_Integer)");
//   };
//#endif
//   if (preparing_for_deletion) return; // already done
//	preparing_for_deletion = TRUE; // new on 060204
//
//	Rational::prepare_for_deletion();
//};

void Remote_Integer::release_all_references() {
	if (releasing_all_references) { // new on 230305
		return;
	};
	if (!tt_resetting_or_destroying_city && controlee != NULL) { 
		// tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 but caused memory errors while resetting during time travel
		controlee->remove_remote(identifier,this);
		controlee = NULL; // new on 130504
	};
	if (operations != NULL) {
		delete operations; // items are private copies
		operations = NULL; // new on 130504
	};
	Rational::release_all_references();
};

Sprite *Remote_Integer::copy(boolean also_copy_offsets) {
	if (global()) { 
		return(copy_for(controlee,also_copy_offsets)); 
	};
   if (controlee != NULL && controlee->part_of_top_level_copy()) {
		UnwindProtect<boolean> set(tt_copying_entire_thought_bubble,FALSE); 
		// new on 120504 since otherwise the following coercion may cause errors
      return(copy_for((Picture *) controlee->copy(),also_copy_offsets)); // sharing is enabled so should work fine
   } else if (tt_copying_insides_for != NULL) {
		return(copy_for(tt_copying_insides_for,also_copy_offsets));
	} else {
		return(copy_for(controlee,also_copy_offsets));
	};
};

Sprite *Remote_Integer::copy_for(Picture *controlee, boolean also_copy_offsets) {
//	city_coordinate center_x, center_y;
//	center_location(center_x,center_y);
	Remote_Integer *copy;
   boolean ignore;
   if (number_dump_type(ignore) == FORCE_REMOTE_INTEGER) {
      copy = new Force_Remote_Integer(controlee,identifier,
									           llx,lly, //center_x,center_y,
									           current_priority-1,
									           width,height,
												  (value==NULL)?NULL:value->copy(), // copy added on 060302
												  active(), // updated from active_flag on 160702
                                      operation,min_remote_integer_text_length);
   } else {
		copy = new Remote_Integer(controlee,identifier,
									     llx,lly, //center_x,center_y,
									     current_priority-1,
									     width,height,
									     (value==NULL)?NULL:value->copy(),
										  active(), // updated from active_flag on 160702
                                operation,min_remote_integer_text_length);
   };
//	copy->set_value(value,4,TRUE,TRUE); // don't let this change the size
//	copy->set_operation(operation);
	if (blank && !tt_copy_restores_pre_blank) {
//	  if (tt_copying_entire_thought_bubble || 
//			different_floors_blanked ||
//			controlee_vacuum_blanked ||
//			controlee_deleted_blanked) {
		  // only allow blanks if in fully defined robot
		  copy->become_blank();
		  //if (different_floors_blanked) {
			 // copy->set_different_floors_blanked(TRUE);
		  //};
		  if (controlee_vacuum_blanked) {
			  copy->set_controlee_vacuum_blanked(TRUE);
		  };
		  if (controlee_deleted_blanked) {
			  copy->set_controlee_deleted_blanked(TRUE);
		  };
//	  };
	};
//	if (error) {
//		copy->become_error();
//	};
	if (operations != NULL) {
		copy->set_operations(operations->copy());
	};
//	if (saved_width != 0) {
//		copy->set_saved_size(saved_width,saved_height);
//	};
// Remote_Integer::Remote_Integer takes care of this now
//	if (controlee != NULL) {
//		controlee->add_remote(identifier,copy);
//	};
	if (identifier == RANDOM_REMOTE && !blank && active()) { // active test added on 161204
		// every copy should be random even if created on the same frame
//		value = my_random(1001);
      copy->set_value(copy->perform_operations(my_random(1000))); // was 1001 prior to 200203
	};
	copy->set_version(version); // new on 120504
 //  copy->set_priority_fixed(priority_fixed_flag);
	//if (comment != NULL) {
	//	copy->set_comment(comment->copy());
	//};
   finish_making_copy(copy,also_copy_offsets);
	return(copy);
};

boolean Remote_Integer::global() {
   return(controlee != NULL && controlee->global());
};

short int Remote_Integer::return_joystick_number() {
   if (controlee != NULL) return(controlee->return_joystick_number());
	// added following on 020200 since this what broke things when loading since didn't think it was a joystick remote
	// this seems to happen inside of thought bubbles
	if (identifier >= JOYSTICK_BUTTON1_REMOTE && identifier <= JOYSTICK_BUTTON32_DOWN_REMOTE) {
		return(0);
	};
   return(-1); // is none 
};

unsigned short int operations_counter = 1;

void Remote_Integer::java_before(java_stream &out) {
   if (blank) return;
   if (operations == NULL) return;
	if (contained_in(THOUGHT_BUBBLE)) { // new on 210700 
		Rational::java_before(out);
		return;
	};
   int count = operations->length();
//   java_begin_block(out);
   java_string operations_name = SC(IDS_JAVA_OPERATIONS, operations_counter++);
   java_declare_new_array(out,"TTNumber",operations_name,count); // was S(IDS_JAVA_TT_INTEGER)
   for (int i = 0; i < count; i++) {
      java_assign_array(out,operations_name,i,operations->nth(i));
   };
   delete [] operations_name;
};

//void Remote_Integer::java_after(output_stream &out, string) {
//   if (blank) return;
//   if (operations != NULL) {
//      java_end_block(out);
//   };
//};

flag reported_java_sensor_limitations = FALSE; // new on 120101

void Remote_Integer::java(java_stream &out) {
   if (blank) {
      out << S(IDS_JAVA_TT_BLANK_NUMBER);
      return;
   };
	if (contained_in(THOUGHT_BUBBLE)) { // new on 210700 
		Rational::java(out);
		return;
	};
   const_string op_arg;
   if (operations == NULL) {
      op_arg = S(IDS_JAVA_NULL);
   } else {
      op_arg = S2(IDS_JAVA_OPERATIONS,operations_counter-1);
   };
//   switch(identifier) {
//      case RANDOM_REMOTE:  // remove this special case soon
		   // every copy should be random even if created on the same frame
//         java_new(out,S(IDS_JAVA_TT_RANDOM_INTEGER),op_arg,operation);
//         break;
//      default:
//         {
			NumberValue *number_value;
			boolean number_value_shared;
         current_value(number_value,number_value_shared);
			long value;
			number_value->long_value(value); // deal with bignums and rationals sometime (new on 300102)
			if (!number_value_shared) delete number_value;
         const_java_string controlee_string;
			if (controlee == NULL) {
				controlee_string = S(IDS_JAVA_NULL); // new on 171100
         } else if (global()) {
            controlee_string = S(IDS_JAVA_TT_GLOBAL_REMOTE);
         } else if (tt_java_picture_name != NULL) {
            controlee_string = tt_java_picture_name;
         } else {
				controlee_string = "container"; // updated again on 141003 to be the extra arg to PicturePart -- 
				// was controlee->java_name(); // new on 141003 but probably not in scope anymore
//          controlee_string = S(IDS_JAVA_PICTURE);
         };
         const_java_string identifier_string;
         if (identifier >= X_REMOTE && identifier <= RANDOM_REMOTE) {
            identifier_string = S(IDS_JAVA_X_REMOTE_IDENTIFIER+identifier);
			} else { // new on 151100 to update things (could have added to javacom.rc instead)
				switch (identifier) {
					case TEXT_TO_MCI_REMOTE:
						identifier_string = "TT.TEXT_TO_MCI_REMOTE"; 
						break;
					case TEXT_TO_SPEECH_REMOTE:
						identifier_string = "TT.TEXT_TO_SPEECH_REMOTE"; 
						break;
					case ROOM_DECORATION_REMOTE:
						identifier_string = "TT.ROOM_DECORATION_REMOTE"; 
						break;
					case HOUSE_DECORATION_REMOTE:
						identifier_string = "TT.HOUSE_DECORATION_REMOTE"; 
						break;
					case ROOF_DECORATION_REMOTE:
						identifier_string = "TT.ROOF_DECORATION_REMOTE"; 
						break;
               case LANGUAGE_REMOTE:
						identifier_string = "TT.LANGUAGE_REMOTE"; 
						break;
					default:  // joystick isn't going to work unless I use DirectX in Java anyway
						identifier_string = S(IDS_JAVA_NO_IDENTIFIER_GIVEN_IDENTIFIER);
						{
							char buffer[max_buffer_size];
							output_string_stream message(buffer,max_buffer_size);
							message << S(IDS_JAVA_SENSOR_LIMITATIONS,
											 "Sorry. Some sensors just don't work in Java. The following sensor will be inactive in Java: ");
							top_level_describe(message);
							message.put('\0'); // terminate string
							log(buffer,!reported_java_sensor_limitations,FALSE); // last arg was 5 prior to 210205
							reported_java_sensor_limitations = TRUE;
						};
				};
         };
         //string operation_string = operation_to_string(operation); // new on 100502 to handle new representation of operations
         //char operation_character;
         //if (operation_string == NULL || operation_string[0] == '\0') {
         //   operation_character = '+';
         //} else if (operation_string[1] != NULL) { 
         //   tt_error_file() << "Java applet generator cannot yet handle the arithmetic operation: " << operation_string << ". Ignored." << endl;
         //   tt_err_file_important = TRUE;
         //   operation_character = '+';
         //} else {
         //   operation_character = operation_string[0];
         //};
			if (identifier == HIT_OR_MISS_REMOTE || identifier == X_HIT_OR_MISS_REMOTE || identifier == Y_HIT_OR_MISS_REMOTE) { 
				// new on 050503 to canonicalize
				if (value == 5301) {
					value = 0; // not hitting
				} else {
					value = 1; // hitting
				};
			};
	      java_new(out,java_remote_class_name(),value,op_arg,java_operation(operation),identifier_string,controlee_string,
						java_language(natural_language())); // natural language added 141100
//         };
//         break;
//	};
};

java_string Remote_Integer::java_remote_class_name() {
   return((java_string) S(IDS_JAVA_TT_REMOTE_INTEGER));
};

void Remote_Integer::dump_value(output_stream &stream) { // // new on 020302
	if (value == NULL) {
		if (controlee != NULL) {
			if (speed_identifier(identifier)) { // new on 050302
				set_value(controlee->current_speed(identifier,TRUE));
#if !TT_CORRECT_IMPLEMENTATION_OF_DISPLAY_X_AND_Y_AS_INTEGERS
			} else if (display_as_double && double_float_identifier(identifier)) { // display_as_double new on 241104
				set_double_value(controlee->current_user_value_double(identifier)); // needed??
#endif
			} else {
				set_long_value(controlee->current_user_value(identifier));
			};
		} else {
			set_long_value(0); // OK?
		};
	};
	if (active() && remote_integer_whose_value_should_not_be_dumped(identifier) && !contained_in(THOUGHT_BUBBLE)) { 
		// needs to be recomputed upon load
		stream.put((unsigned char) TT_COMPUTE_VALUE_WHEN_LOADED);
	} else {
		Rational::dump_value(stream);
	};
};

boolean Remote_Integer::dump(output_stream &stream, boolean just_prepare, boolean is_controlee, boolean on_nest) {
   if (just_prepare) return(FALSE);
//	long  saved_value = value;
//	if (active() && remote_integer_whose_value_should_not_be_dumped(identifier) && 
	// then dump with min_long as value so it is recomputed when reloaded
//	 	 !contained_in(THOUGHT_BUBBLE)) { // 
		// moved here from remote_address::dump on 220499 and changed value to min_long from -1
		// added remote_address check on 230499
		// remote_integer_whose_value_should_not_be_dumped replaced remote_address 121099
		// !contained_in(THOUGHT_BUBBLE) added 241099
//		value = min_long; 
//	};
	boolean already_dumped = !Rational::dump(stream,just_prepare,is_controlee,on_nest);
//	value = saved_value;
	if (already_dumped) return(FALSE); // prior to 221299 this was before restoring value above
//	if (!contained_in(THOUGHT_BUBBLE)) {
		// dump as ordinary integer then
		unsigned char active_and_identifier = (((!active())<<7)|identifier); // added active on leftmost bit on 250699 since wasn't being dumped
		stream.put((unsigned char) active_and_identifier);
      boolean ignore;
      unsigned char type = number_dump_type(ignore);
      if (type == REMOTE_TEXT_PICTURE || type == REMOTE_INTEGER || type == REMOTE_PICTURE) {
         if (controlee != NULL && controlee != tt_global_picture &&
				 being_dumped_together(controlee,this)) { // added on 120599 to save s
				// added controlee != tt_global_picture on 190499 so that not dumped - load should know which
				// ones need to be connected to tt_global_picture
            controlee->dump(stream,just_prepare,TRUE); // TRUE added on 140699
         } else {
            stream.put(NOTHING_MARKER);
         };
      }; // otherwise global so no need to dump controlee or even the NOTHING_MARKER
      if (read_only()) {
			unsigned short int operations_length = 0;
			if (operations != NULL) operations_length = (unsigned short int) operations->length();
			stream.write((character *) &operations_length,sizeof(operations_length));
			Sprites *remaining = operations;
			while (remaining != NULL) {
				if (remaining->first() != NULL) remaining->first()->dump(stream);
				remaining = remaining->rest();
			};
		};
//	};
	return(TRUE);
};

#if TT_XML

boolean force_feedback_effect(RemoteIdentifier identifier) {
   switch (identifier) { 
      case FORCE_FEEDBACK_DURATION_REMOTE:
      case FORCE_FEEDBACK_GAIN_REMOTE:
      case FORCE_FEEDBACK_DIRECTION_REMOTE:
         return(TRUE);
      default:
         return(FALSE);
   };
};

//xml_element *Remote_Integer::xml_create_element(xml_document *document) {
//   return(create_xml_element(L"RemoteControl",document));
//};

void Remote_Integer::xml(xml_element *element, xml_document *document) {
   xml_set_attribute(element,L"Code",return_identifier());
	if (tt_include_descriptions_in_xml) { //tt_dump_reason == DUMPING_TO_CLIPBOARD) { 
		// tt_dumping_to_clipboard) { // no need to waste time and space otherwise -- conditional new on 261102
		string description = NULL;
		if (force_feedback_effect(identifier)) {
			description = S(IDS_FORCE_FEEDBACK_REMOTE_SHORT_LABEL+identifier-FORCE_FEEDBACK_DURATION_REMOTE,"");
		} else {
			description = S(IDS_X_LONG_LABEL+identifier,"");
		};
		if (description[0] != NULL) { // not the null string
			xml_set_attribute(element,L"Description",description);
		};
	};
   if (controlee != NULL) { // aren't there more situations where this isn't needed?
      xml_element *controlee_element = NULL;
      if (controlee->global()) {
         if (controlee == tt_global_picture) {
            controlee_element = create_xml_element(L"GlobalSensor",document);
         } else if (force_feedback_effect(identifier)) {
            controlee_element = create_xml_element(L"ForceFeedbackSensor",document);
            xml_set_attribute(element,L"JoystickNumber",controlee->return_joystick_number());
            xml_set_attribute(element,L"ForceCode",controlee->return_force_index());
         } else {
            for (int i = 0; i < max_number_of_joysticks; i++) {
               if (controlee == tt_joystick_pictures[i]) {
                  controlee_element = create_xml_element(L"JoystickSensor",document,i+1);
                  break;
               };
            };
         };
      } else if (controlee_element == NULL) { 
			// rewritten on 090205 since if dropping something and its controlee into a notebook page 
			// if they are dumped together they should link up
			// beginning 220803 also check if contained in notebook since in_notebook_flag doesn't seem reliable
			// removed && being_dumped_together(controlee,this) on 150703 since with new GUID scheme not needed
			if (being_dumped_together(controlee,this)) {
				controlee_element = create_xml_element(L"For",document);
				controlee->xml_create_and_append_element(controlee_element,document,TRUE)->Release();
			} else if (!(in_notebook() || contained_in(PROGRAM_PAD))) {
				controlee_element = create_xml_element(L"For",document);
				controlee->xml_create_external_link_and_append_element(controlee_element,document); // new on 150703
			};
      };
   //   } else if (controlee_element == NULL && !(in_notebook() || contained_in(PROGRAM_PAD))) {
			//// beginning 220803 also check if contained in notebook since in_notebook_flag doesn't seem reliable
			//// removed && being_dumped_together(controlee,this) on 150703 since with new GUID scheme not needed
   //      controlee_element = create_xml_element(L"For",document);
			//if (being_dumped_together(controlee,this)) { 
			//	controlee->xml_create_and_append_element(controlee_element,document,TRUE)->Release();
			//} else {
			//	controlee->xml_create_external_link_and_append_element(controlee_element,document); // new on 150703
			//};
   //   };
      if (controlee_element != NULL) xml_append_child(controlee_element,element);
   }; // otherwise global so no need to include controlee 
   if (operations != NULL && (read_only() || !active())) {
		// added !active on 071105 so inactive sensors with operatons are saved correctly
      xml_element *operations_element = create_xml_element(L"OperationsAppliedToValue",document);
      operations->add_xml(operations_element,document);
		xml_append_child(operations_element,element); // moved here on 190804
   };
   if ((!active() || !remote_integer_whose_value_should_not_be_dumped(identifier)) && value != NULL) { // not always needed
      // || was && prior to 230802
      // contained_in(THOUGHT_BUBBLE) // can it be active and in a thought bubble?
		// added value != NULL test on 260103 since at least when generating a fresh remote control notebook it is needed
      xml_element *value_element = create_xml_element(L"Value",document);
//      xml_append_child(value->xml_create_element(document),value_element); 
		xml_append_child(create_xml_element(xml_tag(),document,value),value_element); // rewritten on 120703 -- added value on 050803
      xml_append_child(value_element,element);
		// rewritten on 050903 since the above two lines was redundant and produced extra XML with RemoteControl
		// reverted on 050903 but it should be possible to improve this 
//		xml_append_child(create_xml_element(xml_tag(),document,value),element);
   };
	if (operation != NO_NUMBER_OPERATION) { // copied here from Number on 071105
      xml_append_child(create_xml_element(L"Operation",document,operation_to_wide_string(operation)),element); 
   };
	// only ANCESTORS_REMOTE cares about version numbers so far
	xml_set_attribute(element,L"Version",version); // new on 120504 but this way all the sensors can be updated in later version
	// and be loaded into 3.75 and saved again without loss of version information
	// why doesn't the following call Number::xml? - asked 120703
   Sprite::xml(element,document); // moved here on 171002 since setting active needs to happen after value is set
};

xml_element *Remote_Integer::xml_set_special_attributes(xml_element *element, xml_document *document) {
	// new on 201202 -- went through all the state variables
	xml_element *special_element = Rational::xml_set_special_attributes(element,document);
	if (controlee_vacuum_blanked) { 
		xml_set_attribute(special_element,L"RemoteControlBlankDueToVacuuming",1);
	};
	if (controlee_deleted_blanked) { 
		// new on 201202 - not really volatile but seems to only matter if remote control is "out there" as well
		xml_set_attribute(special_element,L"RemoteControlBlankDueToRemoval",1);
	};
	if (display_as_double) {
		xml_set_attribute(special_element,L"DisplayAsDoubleFloatingPointNumber",1); 
		// keeping this inaccurate name for backwards compatiblity since beginning 130505 is consistently a rational number value
	};
	// read_only_flag recomputed on load
	return(special_element);
};

boolean Remote_Integer::xml_get_special_attributes(xml_node *node) {
	controlee_vacuum_blanked = xml_get_attribute_int(node,L"RemoteControlBlankDueToVacuuming",0); 
	// prior to 210404 controlee_vacuum_blanked
	controlee_deleted_blanked = xml_get_attribute_int(node,L"RemoteControlBlankDueToRemoval",0); 
	// prior to 210404 controlee_deleted_blanked
	display_as_double = xml_get_attribute_int(node,L"DisplayAsDoubleFloatingPointNumber",0)&&!tt_x_and_y_display_as_integers; 
	// prior to 210404 display_as_double 
	// added !tt_x_and_y_display_as_integers on 250903 so even if saved as non-integer is truncated if integer values are in effect
	// is that desirable?? why have bother to save it then??
	return(Rational::xml_get_special_attributes(node));
};

boolean Remote_Integer::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {   
      case FOR_TAG:
			set_controlee((Picture *) xml_get_sprite(node,controlee,PICTURE));
         //Sprite *controlee = xml_get_sprite(node);
         //if (controlee != NULL) {
         //   if (controlee->kind_of() == PICTURE) {
         //      set_controlee((Picture *) controlee);
         //   } else {
         //      controlee->destroy(); // warn??
         //   };
         //};
         return(TRUE);
      case GLOBAL_SENSOR_TAG:
         set_controlee(tt_global_picture);
         return(TRUE);
      case JOYSTICK_SENSOR_TAG: {
         int joystick_number = xml_get_int(node);
         if (joystick_number > 0 && joystick_number <= max_number_of_joysticks) {
            set_controlee(tt_joystick_pictures[joystick_number-1]);
         }; // else warn??
         return(TRUE);     
                                };
      case FORCE_FEEDBACK_SENSOR_TAG: {
         boolean ignore_old;
         set_controlee(force_parameter_controlee(xml_get_attribute_int(node,L"ForceCode",0),
				                                     xml_get_attribute_int(node,L"JoystickNumber",0),
							  ignore_old));
         return(TRUE);
                                      };
      case VALUE_TAG: {
         NumberValue *number_value = xml_get_number_value(node); // node was first_node_that_is_an_element(node) prior to 050803
         if (number_value != NULL) { // else warn??
            set_value(number_value,FALSE,TRUE); // args added so assumes size is constrained (until "proven" otherwise) -- 211003
//				set_value(number_value);
         };
         return(TRUE);
                      };
      case OPERATIONS_APPLIED_TO_VALUE_TAG:  // added on 221102
         set_operations(xml_load_sprites(node));
         return(TRUE);
	  case OPERATION_TAG: {
         // copied here on 071105 from Number
         set_operation(operation_from_string(xml_get_text(node)));
         return(TRUE);
                          };
      default:
         return(Sprite::handle_xml_tag(tag,node)); // not rational since wasn't involved in creating this
   };
};

#endif

RemoteStatus Remote_Integer::remote_status() {
	if (global()) return(GLOBAL_REMOTE);
	return(PICTURE_REMOTE);
};

ChangeFrequency Remote_Integer::change_frequency() {
	if (!active()) { // added with 3.26
		return(NEVER_CHANGES); // so the scheduler doesn't penalize robots that use inactive sensors
	};
  switch (identifier) {
     // on March 6 '98 discovered that street_remote and random_remote were switched here
	  case STREET_REMOTE:
	  case AVENUE_REMOTE: // really CAN change at any time -- typically don't
        return(NEVER_CHANGES); // perhaps I need a new category
     case RANDOM_REMOTE:
		  return(ALWAYS_CHANGES); 
	  default:
		  return(ONLY_ON_NEW_CYCLE);
  };
};

void Remote_Integer::note_that_team_is_about_to_run_again() { 
	// new on 030105 -- so RANDOM_REMOTE sensors are updated
	if (identifier == RANDOM_REMOTE && tt_log_version_number >= 66) {
		set_value(perform_operations(my_random(1000)));
	};
};

boolean Remote_Integer::current_value(NumberValuePointer &the_value, boolean &shared, Sprite *by) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("current_value of debug target (remote integer)");
	};
#endif
   if (value == NULL) { // new on 070302 to treat as zero
		set_long_value(0);
	} else if (to_be_smashed()) {
		// new on 011105 since if it appears to be an integer then it should contribute a value as such
		string text_value;
		long text_length;
		current_text(text_value,text_length);
		set_active(FALSE);
		// above new on 071105 so the following doesn't effect other instances
		// following rewritten since just setting set_value_as_string(text_value,text_length) of this
		// doesn't work since active is removed when it has no operation
		Rational *temp_copy = new Rational();
		temp_copy->set_value_as_string(text_value,text_length);
		NumberValuePointer temp_value;
		boolean temp_shared;
      temp_copy->current_value(temp_value,temp_shared,NULL);
      set_value(temp_value->copy(),TRUE,FALSE,FALSE);
		temp_copy->destroy();
	};
	boolean result = Rational::current_value(the_value,shared,by); 
	// prior to 310102 was called first but why bother if returning FALSE?
	// commented out the following since the sensor can be on the back of a picture that is in a box that a robot is matching
	//if (controlee != NULL && !controlee->global() && controlee->on_back_of_picture_or_limbo()) { 
	//	// placed after the above on 020302 since might need value even if inactive (e.g. Remote_Integer::set_value)
	//	return(FALSE); // new on 091100 so sensors don't work if measuring something on back
	//};
	if (by != NULL && by->still_running() &&
		 change_frequency() == ONLY_ON_NEW_CYCLE) {
		((Robot *) by)->dont_run_again_this_cycle();
	};
	return(result);
};

void Remote_Integer::set_value(NumberValue *new_value, boolean ignore_change, boolean sizes_constrained, boolean update) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Setting value of debug target (remote integer)");
	};
#endif
//	if (blank) return;
	// should really have something like black rather blank for 
	// currently inoperative remotes...
	if (controlee_vacuum_blanked ||
//		 different_floors_blanked ||
		 controlee_deleted_blanked) return;
//	long long_value; // for now 020302
//	current_long_value(long_value);
	boolean changed = (!ignore_change && value != NULL && !value->equals(new_value)); //(long_value != new_value));
	// always is at least min_remote_integer_text_length digits wide
	Rational::set_value(new_value,ignore_change,sizes_constrained,update);
	if (changed && !read_only() && controlee != NULL && active()) {
		// active() added 311000 since loading inactive sensors shouldn't reset these values
		// on 111202 moved active() test up so it applies to broadcast stuff below
		if (speed_identifier(identifier)) { // beginning 030302 deal with these as NumberValues
			controlee->speed_display_changed(identifier,new_value->copy());
		} else if (tt_log_version_number >= 67 && 
					  (identifier == WIDTH_REMOTE || identifier == HEIGHT_REMOTE || identifier == X_REMOTE || identifier == Y_REMOTE)) {
			// new on 110105 since round off was causing major (but unreported until now) problems -- e.g. -0.1 from 100 -> 99
			double new_double_value;
			current_double_value(new_double_value,TRUE);
			double revised_value = controlee->display_changed(identifier,new_double_value);
			if (revised_value != new_double_value) {
				// rounding better??
				Rational::set_long_value((long) revised_value,ignore_change,sizes_constrained,update);
			};
		} else {
			long new_long_value;
			current_long_value(new_long_value);
			long revised_value = controlee->display_changed(identifier,new_long_value);
			if (revised_value != new_long_value) {
				Rational::set_long_value(revised_value,ignore_change,sizes_constrained,update);
			};
		};
		if (identifier == X_REMOTE || identifier == Y_REMOTE) {
			// new on 161202
			long ignore_value;
			boolean is_long = value->long_value(ignore_value,FALSE);
			if (display_as_double) {
				if (is_long) {
					display_as_double = FALSE; // is now an integer
				};
			} else {
				if (!is_long) {
					display_as_double = TRUE;
				};
			};
		};
		// new on 140301 so copies are updated as well
		// prior to 030302 the following happened only if wasn't active
		Remote *remote = controlee->pointer_to_remote();
		if (remote != NULL) { // can it be otherwise?
//			long long_value;
//			current_long_value(long_value);
//			remote->changed(identifier,long_value); // was new_value before 160301
			remote->broadcast_new_value(identifier,value,this);
		};
	};
	set_clean_status(DIRTY); // new on 020302
};

//#if !TT_SHRINKING_AND_GROWING
// while the following did solve the random*1000 problem it generally made things look much worse -- removed on 141104
// restored on 301104 but does nothing special unless !tt_shrink_and_grow_numbers
boolean Remote_Integer::current_wide_text(wide_string &the_text, long &the_text_length) {
	// new on 060803 to deal with things like random*1000.
	boolean new_value = (text_length < 0);
	boolean result = Rational::current_wide_text(the_text,the_text_length);
	if (!tt_shrink_and_grow_numbers && new_value && the_text_length > min_text_length && decimal_places == 0) { 
		// added decimal_places == 0 on 080803 since shrinking digits shouldn't change this
		min_text_length = the_text_length;
	};
	return(result);
};
//#endif

void Remote_Integer::add_operation(Number *operation) {
	if (read_only()) {// && !floor->inside_rule_body()) { 
		// for non-read-onlys this doesn't make sense
		// also changes made during training shouldn't stick
		// need to do it from now on
		Sprite *operation_copy = operation->copy();
		operation_copy->set_active(FALSE); // new on 180803 in case someone is combining sensors then the ones on top act like ordinary numbers
		if (operations == NULL) {
			operations = new Sprites(operation_copy);
		} else {
			operations->insert_at_end(operation_copy);
		};
	};
};

NumberValue *Remote_Integer::perform_operations(long start_value) {
	// rewritten on 080503 to use NumberValues rather than longs
//	long answer; // = start_value;
	NumberValue *previous_answer_value = new LongNumberValue(start_value); // new on 300102
	Sprites *remaining = operations;
	Sprite *trouble_maker = NULL;
	while (remaining != NULL) {
		Number *op = (Number *) remaining->first();
		NumberValuePointer new_answer_value;
		ArithmeticException exception = Rational::result_of_operation(op,previous_answer_value,new_answer_value,NULL,TRUE);
		if (exception != NO_EXCEPTION || new_answer_value == NULL) {
         // aren't these dealt with elsewhere in this file??
			say(IDS_SENSOR_ARITHMETIC_PROBLEM);
			switch (exception) {
//				case INTEGER_OVERFLOW:
//					say(IDS_ABORT_NUMBER_TOO_LARGE);
//					break;
//				case INTEGER_UNDERFLOW:
//					say(IDS_ABORT_NUMBER_TOO_SMALL);
//					break;
				case DIVIDE_BY_ZERO:
					say(IDS_CANT_DIVIDE_BY_ZERO);
					break;
				default:
//					say(IDS_ABORT_NUMBER_TOO_SMALL_OR_BIG); // still make sense??
					say(S(IDS_ABORT_UNKNOWN_ARITHMETIC_EXCEPTION,"couldn't do the arithmetic")); // new on 140202
			};
			say(IDS_TROUBLE_NUMBER_REMOVED);
			trouble_maker = op;
		} else {
			// put in this branch on 110803 since if an exception occurred above need to skip the operation
			delete previous_answer_value;
			previous_answer_value = new_answer_value;
		};
		remaining = remaining->rest();
	};
	if (trouble_maker != NULL) {
		operations = operations->remove(trouble_maker);
	};
	return(previous_answer_value);
	//if (previous_answer_value->long_value(answer)) {
	//	delete previous_answer_value;
	//	return(answer);
	//} else {
	//	if (previous_answer_value->sign() > 0) {
	//		say(IDS_ABORT_NUMBER_TOO_LARGE);
	//	} else {
	//		say(IDS_ABORT_NUMBER_TOO_SMALL);
	//	};
	//	return(0);
	//};
};

//boolean Remote_Integer::inside_sprite() {
//	boolean inside = FALSE;
//	if (leader != NULL) {
//		Sprite *sprite = ultimate_leader();
//		inside = (sprite->kind_of() == PICTURE && !((Picture *) sprite)->is_flipped());
//// inside_sprite_flag not maintain any long
////					  ||(sprite->kind_of() == CUBBY && ((Cubby *) sprite)->inside_sprite()));
//	};
//	return(inside);
//};

void Remote_Integer::now_on_back_of(Picture *picture) {
	if (controlee == picture) {
		update_picture(TRUE); // new on 120504 -- at least parts and container sensors seem to need this
		return; // already knew about this
	};
   // following was post 3.21
	if (tt_top_copy != NULL && controlee != NULL) return; 
	// while copying already got a controlee so picture may be a temporary controlee - e.g. will be a flipped picture on the back of controlee
	if (!global() && picture != NULL &&
		 // following added on 060200 so when joystick remote not reset like this
		 (identifier < JOYSTICK_SPEED_X_REMOTE || identifier > JOYSTICK_BUTTON32_DOWN_REMOTE)) { 
		// picture != NULL since if removing a control from back of picture it should still be connected
		set_controlee(picture); // added on 191299 since subsumes the following
//		if (controlee != NULL) {
//			controlee->remove_remote(identifier,this);
//		};
		// stopped using saved_controlee on 111099 since gets the wrong connection
		// e.g. taking a flipped picture A off of flipped picture B and then taking A's box it gets connected to B
//		if (picture != NULL) {
//			controlee = picture;
//		};
//		if (controlee != NULL) {
//			controlee->add_remote(identifier,this);
//		};
	};
};

//Sprite *Remote_Integer::selected(SelectionReason reason, Sprite *by) {
//	if (controlee != tt_dummy_picture && saved_controlee != NULL &&
//		 (reason == GRASPED || reason == VACUUM_USED)) {
//		if (controlee != NULL) {
//			controlee->remove_remote(identifier,this);
//		};
//		controlee = saved_controlee;
//		if (controlee != NULL) {
//			controlee->add_remote(identifier,this);
//		};
//	};
//	return(selected(reason,by));
//};

void Remote_Integer::changed_vacuum_status(boolean controlee_in_vacuum) {
	if (!controlee_vacuum_blanked && controlee_in_vacuum &&
		 controlee->remote_status() == NOT_REMOTE) {
		become_blank();
		controlee_vacuum_blanked = TRUE;
	} else if (controlee_vacuum_blanked && !controlee_in_vacuum) {
		// was blank but no longer
		controlee_vacuum_blanked = FALSE;		
		become_non_blank();
	};
};

Sprite *Remote_Integer::now_inside_thought_bubble(boolean in_training) {
   if (in_training) return(this);
	if (identifier == PARAMETER_REMOTE // used as a particular value typically
#if TT_LEGO
// conditionalized on 220802
		  || (identifier >= LEGO_MOTOR1_REMOTE && 
		  identifier <= LEGO_SENSOR_COUNT8_REMOTE
#if TT_SICS_ULTRASOUND_CONTROLLER
        && tt_lego_in_use != SICS_ULTRASOUND_CONTROLLER
#endif
         )
#endif
       ) {
		set_active(FALSE); // new on 220499
		return(this);
		/*
		// return ordinary number
		Integer *replacement = new Integer();
		replacement->set_value(value);
		leader->add_follower(replacement);
		leader->remove_follower(this);
		replacement->set_size_and_location(width,height,llx,lly);
		destroy();
		return(replacement);
		*/
	} else {
		if (active() || tt_log_version_number < 64) { // condition new on 161204 -- version check updated 191204
			become_blank();
		};
		return(this);
	};
};

Sprite *Remote_Integer::used(Sprite *, Sprite *by,
							        boolean , boolean ,
							        millisecond , boolean modified, Sprite *) {
//   if (modified) {
////      if (!tt_new_input_scheme) set_value(value-1);
//   } else {
		// still make sense?
//      set_value(value+1);
//   };
	// rewritten on 030302 so that it does nothing except prevent the general behavior of flipping a number over from happening
   by->completed();
   return(this);
};

Sprite *Remote_Integer::respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected,
													     Sprite *by, boolean record_action, Sprite *original_recipient, 
														  boolean size_constrained_regardless) {
	if (!extended && // extended test new on 010704 since Delete caused pad to be stopped (treated like '.')
		 !original_recipient->held() || !by->can_send_keyboard_editing_keys() || read_only()) { 
// read_only added on 150604 to deal with textual sensors
		// -- removed !original_recipient->held() || 
	   // on 100604 -- restored 110604 since turning on and off does depend upon whether held or not
		NumberValue *previous_value = value;
		if (key == tt_stop_character) { // new on 190799
			set_active(FALSE);
			// following new on 120604 so don't also add this to the number etc
			return(Sprite::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,
                                            size_constrained_regardless));
		} else if (key == tt_run_character) {// && tt_log_version_number > 14) {
			set_active(TRUE);
			// following new on 120604 so don't also add this to the number etc
			return(Sprite::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,
				                                size_constrained_regardless));
	// following commented out on 030302 since taken care of fine by Remote_picture::respond_to_keyboard
//	} else if (enumeration_type()) {
		// e.g. text values like Yes and No
//		if (key == ' ' || key == '+') { // ' ' probably overridden since is now tt_stop_character
//			set_value(value+1);
//		} else if (key == '-') {
//			set_value(value-1);
//		} else {
//			Sprite *result = Rational::respond_to_keyboard(key,extended,robot_selected,by,record_action,action_status);
//			if (value != previous_value) {
////				set_active(FALSE); // new on 130300 - e.g. so addresses become inactive when changed manually
//				changed_by_keyboard(); // new on 160300
//			};
//			return(result);
//		};
		} else {
			if (identifier == PARAMETER_REMOTE && controlee != NULL) {
				// respond to keyboard as if value was just what is displayed
				Rational::set_long_value(controlee->current_user_value(identifier));
			};
			Sprite *result 
				= Rational::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,
				                                size_constrained_regardless);
			if (value != previous_value) { // assumes any change called :set_value
//			set_active(FALSE); // new on 130300 - e.g. so addresses become inactive when changed manually
				changed_by_keyboard(); // new on 160300 since only some kinds of remotes should become inactive - e.g. addresses
			};
			if (edited) { // new on 120604 so changes affect things immediately
				canonicalize();
			};
			return(result);
		};
	};
	// commented out on 080400 since Sprite::respond_to_keyboard does it fine
//	if (record_action) record_event(KEYBOARD_CHARACTER,by,floor,this,TRUE,key);
//	completed(action_status);
   Sprite *result = Rational::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,
		                                            size_constrained_regardless);
	if (edited && active()) { // new on 120604 so changes affect things immediately
		// added active() test on 011105 since if remote isn't active you should be able to edit it as normal
		canonicalize();
	};
	return(result);
};

void Remote_Integer::become_blank(boolean by_user, boolean controlee_too) {
	// commented out on 051006 since can cause memory corruption
	if (tt_log_version_number < 79 && controlee != NULL && !controlee->inside_vacuum() && !controlee->slated_for_deletion()) { 
		// no need to be notified if it is blank but if controlee is in a vacuum then it may be spit out again later
		// added && !controlee->slated_for_deletion() on 080604 since no need and can cause problems (in adventure game)
		controlee->remove_remote(identifier,this);
	};
	Rational::become_blank(by_user,controlee_too);
};

void Remote_Integer::become_non_blank(boolean controlee_too) {
	if (!controlee_vacuum_blanked &&
//		 !different_floors_blanked &&
		 !controlee_deleted_blanked) {
		Rational::become_non_blank(controlee_too);
	};
	// commented out following on 051006 since :become_blank no longer removes remote
	// re-ordered on 110105 to catch any calls to add_remote of an erased remote
	if (tt_log_version_number < 79 && controlee != NULL && !controlee->inside_vacuum()) {
		// added && !controlee->inside_vacuum() on 010200
		controlee->add_remote(identifier,this);
	};
};

// commented out on 180702 since why is this any different from numbers? Are there remote_pictures that shouldn't be erased?
//boolean Remote_Integer::blankable() {
//   return(contained_in(THOUGHT_BUBBLE) || identifier == CHILDREN_REMOTE); 
// added parts sensor on 160101 since that is now how to remove all behaviors
//};

void Remote_Integer::controlee_deleted() {
	become_blank();
	controlee = NULL;
	controlee_deleted_blanked = TRUE;
};
	
void Remote_Integer::inside_thought_bubble() {
	// stop tracking -- how to restore if taken out of thought bubble??
	if (controlee != NULL && !global()) {
		controlee->remove_remote(identifier,this);
		controlee = NULL;
	};
   // why not become blank??
	Rational::inside_thought_bubble();
};

void Remote_Integer::generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection) {
	if (controlee == NULL) {
		Rational::generate_help(message,tool_in_hand,selection);
		return;
	};
	History history = tt_histories[kind_of()];
	if ((!((history>>identifier)&1) && tt_help_counter == 0) ||
		 tt_help_counter == 1) {
		if (tt_help_counter == 0) {
			// haven't explained this kind of remote yet
			no_help_interruptions();
			user_did(kind_of(),1L<<identifier);
		};
		if (!read_only_identifier(identifier)) {
			if (!global()) {
				message << S(IDS_REMOTE_DESCRIPTION);
				controlee->describe_type(message,DEFINITE_ARTICLE);
//          following provides too much detail and mentions flip side
//				controlee->describe(message,DEFINITE_ARTICLE);
				message << S(IDS_REMOTE_DESCRIPTION2,"") << end_sentence();
			};
			if (identifier == PARAMETER_REMOTE) {
				if (controlee->is_synthetic() ||
                (controlee->pointer_to_indirection() != NULL &&
                 (controlee->pointer_to_indirection()->kind_of() == TEXT ||
                  controlee->pointer_to_indirection()->kind_of() == INTEGER))) {
					message << S(IDS_REMOTE_COLOR_DESCRIPTION);
				} else {
					message << S(IDS_REMOTE_PARAMETER_DESCRIPTION);
				};
			} else if (global()) {
//				message << S(IDS_THIS_NUMBER) << " ";
//				if (read_only()) {
//					message << S(IDS_A_SENSOR);
//				} else {
//					message << S(IDS_A_REMOTE_CONTROL);
//				};
//				message << " " << S(IDS_FOR) << " " << S(IDS_X_HELP+identifier) << end_sentence();
            long next_id;
				if (read_only()) {
            	message << S(IDS_REMOTE_NUMBER_DESCRIPTION6);
					next_id = IDS_REMOTE_NUMBER_DESCRIPTION62;
            } else {
               message << S(IDS_REMOTE_DESCRIPTION);
					next_id = IDS_REMOTE_DESCRIPTION2;
            };
            boolean ignore;
            if (number_dump_type(ignore) == FORCE_REMOTE_INTEGER) {
               message << S(IDS_FORCE_FEEDBACK_DURATION_REMOTE_HELP+identifier-FORCE_FEEDBACK_DURATION_REMOTE) << space()
                       << S(IDS_FORCE_FEEDBACK_EFFECT_HELP) << space()
                       << ((ForceRemoteControlee *) controlee)->return_force_name() << end_sentence();
            } else {
               message << S(IDS_X_HELP+identifier);
            };
				message << S(next_id,"");
			} else {
				message << S(IDS_REMOTE_NUMBER_DESCRIPTION2) << space()
						  << S(IDS_X_HELP+identifier) << space()
                    << S(IDS_OF_THE_PICTURE) << end_sentence();
			};
			if (!global()) {
				message << S(IDS_REMOTE_NUMBER_DESCRIPTION3);
				controlee->describe_type(message,DEFINITE_ARTICLE,FALSE);
			   message << S(IDS_REMOTE_NUMBER_DESCRIPTION4);
			};
// generated redundant stuff
//			if (!type_history_full(INTEGER)) {
//				message << S(IDS_REMOTE_NUMBER_DESCRIPTION5);
//				Integer::generate_help(message,tool_in_hand);
//			};
			if (controlee->is_flipped()) {
				message << S(IDS_SINCE);
            if (current_language() != SWEDISH) {
				   controlee->describe_type(message,DEFINITE_ARTICLE);
            };
				message << S(IDS_REMOTE_NUMBER_DESCRIPTION9);
			};
	  } else {
		  message << S(IDS_REMOTE_NUMBER_DESCRIPTION6)
					 << S(IDS_X_HELP+identifier) << end_sentence();
        if (operations != NULL) {
			  message << S(IDS_AND_COMBINED_WITH) << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
			  Sprites *remaining = operations;
			  while (remaining != NULL) {
				  remaining->first()->describe(message);
				  remaining = remaining->rest();
				  if (remaining != NULL) {
					  message << space(SPACE_BEFORE_OR_AFTER_NUMBERS) << S(IDS_AND) << space(SPACE_BEFORE_OR_AFTER_NUMBERS);
				  };
			  };
           message << S(IDS_REMOTE_NUMBER_DESCRIPTION62,"") << end_sentence();
		  };
        message << S(IDS_REMOTE_NUMBER_DESCRIPTION7);
		  message << S(IDS_REMOTE_NUMBER_DESCRIPTION8);
	  };
//				if ((identifier == MOUSE_SPEED_X_REMOTE ||
//					  identifier == MOUSE_SPEED_Y_REMOTE) &&
//					 !tt_screen->inside_rule_body()) {
//					message << S(IDS_HIDE_HAND_HELP1)
//							  << S(IDS_HIDE_HAND_HELP2)
//							  << S(IDS_HIDE_HAND_HELP3);
//				};
	};
};

void Remote_Integer::set_inside_vacuum(boolean new_flag, boolean recur) {
//   if (inside_vacuum_flag == new_flag) return; 
	// new on 170702 to ignore calls that already know about - commented out on 190702 since might not have been called with recur earlier
	boolean old_inside_vacuum_flag = inside_vacuum_flag;
   Rational::set_inside_vacuum(new_flag,recur);
	if (!new_flag && old_inside_vacuum_flag && controlee != NULL) { 
      // new on 190700 since might have changed while inside of vacuum (second conjunct added 260700)
		// added condition on 190702 that this be "news"
      // was !old_inside_vacuum_flag prior to 290802 but that didn't make sense and caused these to be called when copying (finish_making_copy)
      if (speed_identifier(identifier)) { // new on 050302
			set_value(controlee->current_speed(identifier,TRUE));
#if !TT_CORRECT_IMPLEMENTATION_OF_DISPLAY_X_AND_Y_AS_INTEGERS
		} else if (display_as_double && double_float_identifier(identifier)) { // display_as_double new on 241104
			set_double_value(controlee->current_user_value_double(identifier));
#endif
		} else {
			user_coordinate new_value = controlee->current_user_value(identifier);
//			user_coordinate newest_value = perform_operations(new_value); // no operations on speeds or double_values??
//			set_long_value(newest_value,TRUE); 
			set_value(perform_operations(new_value),TRUE); // rewrote the above on 080503
		};
	};
};

//void Remote_Integer::update_text_string() {
//   if (identifier == PARAMETER_REMOTE && controlee != NULL && controlee->is_synthetic()) {
//      long saved_value = value;
//      value = value%256;
//      Rational::update_text_string();
//      value = saved_value;
//   } else {
//      Rational::update_text_string();
//   };
//};

boolean Remote_Integer::textual() {
	return(text_identifier(identifier));
};

void Remote_Integer::set_controlee(Picture *new_controlee) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		tt_error_file() << "Setting controlee of debug target" << endl;
	};
	//if (new_controlee != NULL && new_controlee->pointer_to_leader() == NULL && new_controlee->showing_on_a_screen()) {
	//	debug_this();
	//};
#endif
	if (controlee == new_controlee) return;
   if (controlee != NULL) {
		if (controlee_vacuum_blanked) { // new on 120105
			controlee_vacuum_blanked = FALSE;
			become_non_blank();
		};
		controlee->remove_remote(identifier,this);
	};
	if (controlee_deleted_blanked && new_controlee != NULL) {
		controlee_vacuum_blanked = FALSE; // just in case this is how it was deleted
		controlee_deleted_blanked = FALSE;
		become_non_blank();
	};
	controlee = new_controlee;
	if (active()) { // conditional added on 111202 since if not active doesn't matter about the controlee (just now)
		if (controlee != NULL) { // moved into this conditional on 170105 since wasteful (and maybe breaks some demos) to add this when inactive
			controlee->add_remote(identifier,this);
		};
		set_appearance(NULL); // new on 280300 since appearance is likely to be out of date
		update_picture(); // new on 070600 since otherwise text and picture based sensor display black (but probably worked)
		// following is strange - check it out sometime
		Remote_Integer::update_picture(); // new on 041201 so that dropping sensors on the back get updated properly (e.g. the visibility sensor)
	};
};

void Remote_Integer::update_picture(boolean regardless) { // new on 110401 to ensure that these get the right value when given a new controlee
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		tt_error_file() << "Updating picture of sensor that is debug target" << endl;
	};
#endif	
   if (!regardless && !ok_to_activate()) return; // was just active() prior to 200802
   if (is_blank()) { // new on 100502 -- doesn't make sense to update if erased
      return;
   };
	if (controlee == NULL) return;
	if (speed_identifier(identifier)) { // new on 050302
		NumberValue *current_speed = controlee->current_speed(identifier,FALSE);
		if (current_speed == NULL) { 
			// new on 070302 - but wouldn't be better to fix this earlier? -- 
			// if I didn't call current_speed with FALSE then would be OK but a bit wasteful
			return; 
		};
		NumberValue *old_speed;
		boolean old_speed_shared = TRUE;
		if (!current_value(old_speed,old_speed_shared) || !old_speed->equals(current_speed)) {
			set_value(current_speed->copy());
		};
		if (!old_speed_shared) delete old_speed;
#if !TT_CORRECT_IMPLEMENTATION_OF_DISPLAY_X_AND_Y_AS_INTEGERS
	} else if (display_as_double && double_float_identifier(identifier)) {
		// updated on 161202 so that displays as a double only if its history warrants it (e.g. a non-integer was dropped on top)
		double new_value = controlee->current_user_value_double(identifier);
		double old_value;
		current_double_value(old_value);
		if (new_value != old_value) { 
			set_double_value(new_value);
		};
#endif
	} else {
		long new_value = controlee->current_user_value(identifier);
		long old_value;
		current_long_value(old_value);
		if (new_value != old_value) { // condition new on 221101
			set_long_value(new_value);
		};
	};
};

void Remote_Integer::set_in_notebook(Notebook *notebook, boolean recur) { // new on 200802 so updated properly in notebooks
//   boolean changed = ((notebook != NULL) != in_notebook_flag);
   Number::set_in_notebook(notebook,recur);
   if (notebook != NULL) {
      Picture *picture = notebook->pointer_to_picture();
      if (picture != NULL) {
			set_controlee(picture);
		} else {
			update_picture(active()); // new on 210902 so that sensors display updates when page flips -- changed from TRUE to active on 231002
		};
   } else {
      update_picture(active()); // TRUE added on 210902  -- changed from TRUE to active on 231002
   };
};

// commented out on 101202 since does nothing useful
//boolean Remote_Integer::receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item) {
//	// commented out on 150699 since this breaks useful things like operations on random numbers
////	if (read_only()) { // added on 090699 to protect read-only sensors
////		by->abort_action_if_robot(action_status,S(IDS_SENSOR_UNDERNEATH),
////										  MOUSE_WITH_HAMMER,
////										 !told_user(TOLD_STUFF_ON_SENSORS));
////		return(FALSE);
////	} else {
//		return(Rational::receive_item(item,by,duration,original_recipient,original_item));
////	};
//};

void Remote_Integer::changed_by_keyboard() {
	if (inactive_after_changed(identifier)) { // new on 160300
		set_active(FALSE);
	};
};

void Remote_Integer::set_active(boolean new_flag, boolean recur, boolean initialization) { // new on 301000
	boolean change = (new_flag != active());
	Sprite::set_active(new_flag,recur,initialization);
	if (change) { // moved here on 170105
		if (controlee != NULL) { 
			if (active()) {
				controlee->add_remote(identifier,this);
			} else {
		      // no need to be notified if it is inactive but if controlee is in a vacuum then it may be spit out again later
				controlee->remove_remote(identifier,this);
			};
		};
		if (new_flag && identifier != RANDOM_REMOTE) { 
			// identifier != RANDOM_REMOTE new on 020204 since this is called when loading and activating and throws off replayability
			update_value();
		};
	};
	set_never_cache(new_flag); // new on 020302 -- not clear this does any good 
	// but may save some work if animated so it changes all the time
};

void Remote_Integer::update_value() { // abstracted on 180803 and added TRUE args so this doesn't trigger further changes
	if (controlee == NULL) return;
	if (speed_identifier(identifier)) { // new on 050302
		set_value(controlee->current_speed(identifier,TRUE),TRUE);
#if !TT_CORRECT_IMPLEMENTATION_OF_DISPLAY_X_AND_Y_AS_INTEGERS
	} else if (display_as_double && double_float_identifier(identifier)) { // display_as_double new on 180103
		set_double_value(controlee->current_user_value_double(identifier),TRUE);
#endif
	} else {
		set_long_value(controlee->current_user_value(identifier),TRUE);
	};
};

void Remote_Integer::display(SelectionFeedback selection_feedback, boolean followers_too, TTRegion *region) {	
	if (controlee != NULL && !controlee->global() && controlee->on_back_of_picture_or_limbo() && !blank) { 
		// new on 091100 so sensors don't work if measuring something on back
		// rewritten on 030302 to display as if blank when controlee is not really around 
		// (tried using active_flag instead but then you would expect arithimetic to work and doesn't)
		blank = TRUE;
		Rational::display(selection_feedback,followers_too,region);
		blank = FALSE;
//		Sprite::display(selection_feedback,FALSE,region); 
	} else {
      if (value == NULL) {
         // not clear how this happens - new on 100502
         value = new LongNumberValue(0); // or recompute??
      };
		Rational::display(selection_feedback,followers_too,region);
	};
};

#if TT_CORRECT_IMPLEMENTATION_OF_DISPLAY_X_AND_Y_AS_INTEGERS
boolean Remote_Integer::display_as_integer() { // new on 130505
	return((tt_x_and_y_display_as_integers && (identifier == X_REMOTE || identifier == Y_REMOTE)) || tt_log_version_number < 75);
};
#endif

boolean Remote_Integer::equal_to(Sprite *other, Sprites *pending, 
											SpritesPointer &corresponding_nests, SpritesPointer &expected_corresponding_nests, 
											EqualReason reason) { // new on 050601
	if (this == other) return(TRUE);
	if (kind_of() != other->kind_of()) {
		return(FALSE);
	};
	if (remote_status() != other->remote_status()) {
		return(FALSE); // new on 110601
	};
	if (is_blank() != other->is_blank()) {
		return(FALSE);
	};
	if (active() != other->active()) {
		return(FALSE);
	};
	Remote_Integer *other_remote = (Remote_Integer *) other;
	if (return_identifier() != other_remote->return_identifier()) {
		return(FALSE);
	};
//	if (dump_value() != other_remote->dump_value()) return(FALSE); // new on 080601
   if ((controlee == NULL || !active()) && !Rational::equal_to(other,pending,corresponding_nests,expected_corresponding_nests,reason)) {
      // added more conditions on 051102 since if their controlees are equal it doesn't matter if they have different value (e.g. X remote)
      return(FALSE); // this replaced above on 030302
   };
	Sprites *remaining = pointer_to_operations(); // new on 080601
	Sprites *other_remaining = other_remote->pointer_to_operations();
	while (remaining != NULL && other_remaining != NULL) {
		if (!remaining->first()->equal_to(other_remaining->first(),pending,corresponding_nests,expected_corresponding_nests,reason)) {
			return(FALSE);
		};
		remaining = remaining->rest();
		other_remaining = other_remaining->rest();
	};
	if (remaining != NULL || other_remaining != NULL) {
		return(FALSE);
	};
	if (controlee != NULL) {
		if (other_remote->pointer_to_controlee() == NULL) {
			return(FALSE); // does this happen?
		};
		return(controlee->equal_to(other_remote->pointer_to_controlee(),pending,corresponding_nests,expected_corresponding_nests,reason));
	} else {
		return(other_remote->pointer_to_controlee() == NULL);
	};
	return(TRUE);
};

void Remote_Integer::restore_active(boolean new_flag) { // new 100901
	if (new_flag) {
		// make this active and the "truth"
		NumberValue *saved_value = value->copy(); // updated on 030302
		set_active(new_flag);
		set_value(saved_value);
	} else {
		Sprite::restore_active(new_flag);
	};
};

//void Remote_Integer::set_temporarily_inactive(boolean become_inactive) { // new on 200802
//   if (active_flag && become_inactive) {
//      active_flag = FALSE;
//      temporarily_not_active_flag = TRUE;
//   } else if (temporarily_not_active_flag && !become_inactive) {
//      temporarily_not_active_flag = FALSE;
//      restore_active(TRUE);
//   };
//};

PlateState Remote_Integer::plate_state() {
	if (active()) { // conditional new on 100901
       return(NUMBER_PAD_WITH_PLATE_ANIMATED);
	} else {
		return(NUMBER_PAD_WITH_PLATE);
   };
};

void Remote_Integer::set_operation(NumberOperation new_operation, boolean sizes_constrained) {
	// new on 011105 to turn on and off a remote depending upon whether it has an operation
	set_active(new_operation == NO_NUMBER_OPERATION);
	Rational::set_operation(new_operation,sizes_constrained);
};

Remote_Picture::Remote_Picture(Picture *controlee,
										 RemoteIdentifier identifier,
										 city_coordinate center_x, city_coordinate center_y,
										 long priority,
										 city_coordinate initial_width,
										 city_coordinate initial_height,
										 long value,
										 // added following arg on 150499 so these are initialized
										 // correctly is dumped language is different from current one
										 NaturalLanguage natural_language,
										 boolean active,
										 boolean viewing_front_side) :
	 Remote_Integer(controlee,identifier,center_x,center_y,priority,
						 initial_width,initial_height,new LongNumberValue(value),active),
	 appearance(NULL),
//    extra_pointer(NULL),
	 sensor_applied_to_house(NULL), // new on 201202
	 saved_controlees_followers(NULL), // new on 201202
	 original_of_appearance(NULL),
	 viewing_front_side_flag(viewing_front_side) {		 
   set_language(natural_language);
	set_sprite_type(REMOTE_PICTURE);
   set_is_picture(TRUE);
//	unsave_size(); // screwed up by inherited stuff from Remote_Integer
/* commented out on 140599 since it sets things to not colliding even when copying due to training
	switch (identifier) {
		case HIT_OR_MISS_REMOTE:
		case X_HIT_OR_MISS_REMOTE:
		case Y_HIT_OR_MISS_REMOTE:
			if (controlee != NULL) { 
				// conditional added on 130599 so value not reset if no controlee
				set_value(compute_picture_id(HIT_OR_MISS_SPRITE,NOT_COLLIDING),TRUE,FALSE,FALSE);
			};
			break;
//		case LAST_KEYSTROKE_REMOTE:
//		case KEYBOARD_BUTTON_REMOTE:
//		case SHIFT_DOWN_REMOTE:
//		case CONTROL_DOWN_REMOTE:
//      case PROGRAMMER_HEAD_REMOTE:
//			set_value(0); // start off non-negative since negative is used for "dead"
//			break;
	};
	*/
	if (identifier != CHILDREN_REMOTE && identifier != ANCESTORS_REMOTE) {
		update_picture(); // initialize stuff even if not displayed
	};
};

//void Remote_Picture::prepare_for_deletion() {
//   if (preparing_for_deletion) return; // already done
//	preparing_for_deletion = TRUE; // new on 060204
//
//	Remote_Integer::prepare_for_deletion();
//};

void Remote_Picture::release_all_references() { // new on 130504
	if (releasing_all_references) { // new on 230305
		return;
	};
	if (appearance != NULL) {
      close_current_sound();
		appearance->destroy(); // removed by on 060204
      appearance = NULL;
	};
	//if (extra_pointer != NULL && identifier == CHILDREN_REMOTE) {
	//	Sprites *controlees_followers = (Sprites *) extra_pointer;
	//	just_destroy_list(controlees_followers);
	//	extra_pointer = NULL;
	//};
	if (saved_controlees_followers != NULL) {
		just_destroy_list(saved_controlees_followers);
	};
	Remote_Integer::release_all_references();
};

Remote_Picture *Remote_Picture::just_copy(Picture *controlee) {
	long long_value;
	current_long_value(long_value);
	return(new Remote_Picture(controlee,identifier,
									  llx,lly, //center_x,center_y,
									  current_priority-1,
									  width,height,
									  long_value,
									  language,
									  active(), // updated from active_flag on 160702
									  viewing_front_side_flag));
};

Sprite *Remote_Picture::copy_for(Picture *controlee, boolean also_copy_offsets) {
//	city_coordinate center_x, center_y;
//	center_location(center_x,center_y);
	if (appearance != NULL && tt_copying_entire_thought_bubble && !tt_copy_restores_pre_blank &&
		 tt_remove_unneeded_things_from_thought_bubble
		 && !tt_copying_for_training) { 
			 // this added on 080205 since for training need live remotes -- e.g. to vacuum off touching who
		// new on 110105
		// shouldn't have remote controls in the thought bubble
		Sprite *appearance_copy;
		if (is_blank()) {
			if (appearance->kind_of() == PICTURE) {
				appearance_copy = appearance->shallow_copy();
			} else if (appearance->kind_of() == CUBBY) {
				appearance_copy = new Cubby();
			} else {
				appearance_copy = appearance->copy();
			};
			appearance_copy->become_blank();
			return(appearance_copy);
		} else {
			return(appearance->copy());
		};
	};
	if (!infinite_stack()) { // new on 221299 since there can be cycles via things like CHILDREN_REMOTE
      Sprite *copied = already_copied(this);
      if (copied != NULL) {
//			copied->increment_ref_count(this); // commented out on 311000
			return((Remote_Integer *) copied);
		};
   };
   //if (appearance != NULL && tt_copying_entire_thought_bubble) { // new on 231002
   //   // this is a new policy -- means that wand with O(riginal) 
	//   // will not quite behave as before but everything else is much simpler
   //   // should remote looks do this too??
   //   return(appearance->copy());
   //};
	Remote_Picture *copy = just_copy(controlee);
	if (!infinite_stack()) { 
		just_copied(this,copy);
	};
	if (appearance != NULL) {
		// && !decoration_identifier(identifier)) {
		// second condition added 120804 since may be a different house (e.g. due to sharing of notebook pages)
		//if (identifier == CHILDREN_REMOTE || identifier == ANCESTORS_REMOTE) {
		//	// new on 120504 since these remotes are hard to keep up-to-date
		// a nice idea but can cause the appearance to be NULL
		//	update_picture(TRUE);
		//};
//		UnwindProtect<Picture *> set(tt_copying_insides_for,NULL); // experiment 160504
		copy->set_appearance(appearance->top_level_copy(FALSE,this,copy));
		// above replaces the following on 240203 - works for all sensors and treats them uniformly
//		if (identifier == FILE_TO_SOUND_REMOTE ||
//          identifier == CLIPBOARD_REMOTE ||
//          identifier == TEXT_TO_MCI_REMOTE ||
//          identifier == TEXT_TO_SPEECH_REMOTE ||
//		    // tried to comment out the following due to following but it didn't help much
//		    // commented out these on 091100 since they can be very very expensive 
			 // (e.g. firing bullet in space invaders with ancestor appearance being whole game)
//			 identifier == COLLIDEE_REMOTE) {
//			// removed top_level_ as an experiment but I think it should be there
//			copy->set_appearance(appearance->copy()); 
		// restored top_level_copy for these sensors on 130701 since otherwise can get improper sharing of leader
//		} else if (identifier == CHILDREN_REMOTE ||
//					  identifier == ANCESTORS_REMOTE) {
////           identifier == ROOM_DECORATION_REMOTE ||   // force re-computation
////           identifier == HOUSE_DECORATION_REMOTE ||
////           identifier == ROOF_DECORATION_REMOTE)) {
//			//Picture *saved_tt_copying_insides_for = tt_copying_insides_for;
//			//tt_copying_insides_for = NULL; // new on 210203 since otherwise this can copy and use the wrong controlee
//			// restored on 210203 since can now pass this and copy to trap circularities
//			copy->set_appearance(appearance->top_level_copy(FALSE,this,copy)); 
		// removed top_level_ on 221299 to deal with circularities (e.g. CHILDREN_REMOTE on the back of a picture)
////			tt_copying_insides_for = saved_tt_copying_insides_for;
//		};
	};
// reset value since general initialization messes it up
	long long_value;
	current_long_value(long_value);
	copy->set_long_value(long_value,TRUE,TRUE,FALSE); // don't let this change the size
//	copy->set_operation(operation);
	if (blank && !tt_copy_restores_pre_blank) {
//	  if (tt_copying_entire_thought_bubble ||
//			different_floors_blanked ||
//			controlee_vacuum_blanked ||
//			controlee_deleted_blanked) {
		  // only allow blanks if in fully defined robot
		  copy->become_blank(); 
		  //if (different_floors_blanked) {
			 // copy->set_different_floors_blanked(TRUE);
		  //};
		  if (controlee_vacuum_blanked) {
			  copy->set_controlee_vacuum_blanked(TRUE);
		  };
		  if (controlee_deleted_blanked) {
			  copy->set_controlee_deleted_blanked(TRUE);
		  };
//	  };
	};
//	if (error) {
//		copy->become_error();
//	};
//	if (saved_width != 0) {
//		copy->set_saved_size(saved_width,saved_height);
//	};
//	if (controlee != NULL) {
//		controlee->add_remote(identifier,copy);
//	};
	copy->set_size(width,height); // might have been recomputed so reset
   copy->set_language(language);
	copy->set_version(version); // new on 120504
 //  copy->set_priority_fixed(priority_fixed_flag);
	//if (comment != NULL) {
	//	copy->set_comment(comment->copy());
	//};
	if (identifier == SELECTED_REMOTE) {
		// added on 300106 since doesn't matter if thing copied was selected (others too? -- e.g. GRASPED_REMOTE)
		copy->update_value();
	};
   finish_making_copy(copy,also_copy_offsets);
	return(copy);
};

NaturalLanguage Remote_Picture::natural_language() {
   return(language);
};

void Remote_Picture::set_language(NaturalLanguage lang) {
   if (lang == BRITISH) {
      language = AMERICAN; // same with respect to sensors
   } else {
      language = lang;
   };
#if TT_DEBUG_ON
   if (language < 0) {
      log("Invalid language code.");
      language = AMERICAN;
   };
#endif
};

java_string Remote_Picture::java_remote_class_name() {
	if (identifier == HIT_OR_MISS_REMOTE || identifier == X_HIT_OR_MISS_REMOTE || identifier == Y_HIT_OR_MISS_REMOTE) {
		// new on 201100 since these sensors act like numbers (booleans 0 or 1)
		return((java_string) S(IDS_JAVA_TT_REMOTE_INTEGER));
	} else {
		return((java_string) S(IDS_JAVA_TT_REMOTE_PICTURE));
	};
};

boolean Remote_Picture::user_media() {
   return(appearance != NULL &&
          (// identifier == CLIPBOARD_REMOTE ||
           identifier == FILE_TO_SOUND_REMOTE ||
           identifier == TEXT_TO_MCI_REMOTE ||
           identifier == TEXT_TO_SPEECH_REMOTE));
};

boolean Remote_Picture::dump(output_stream &stream, boolean just_prepare, boolean is_controlee, boolean on_nest) {
   if (just_prepare) {
      if (appearance != NULL) appearance->dump(stream,just_prepare,TRUE);
      return(FALSE);
   };
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log("Dumping debug target - remote picture.");
	};
#endif
//	if (about_to_dump(this,stream)) return(FALSE); 
	// new on 051099 - commented out since following takes care of this just fine - and doing it twice is very bad
	// following moved here on 050100 so that all remote pictures dump as integer and then dump appearance or NOTHING_MARKER now
	if (!Remote_Integer::dump(stream,just_prepare,is_controlee,on_nest)) {
		return(FALSE);
	};
	if (appearance != NULL && !tt_dumping_to_test_equality_or_describe && !is_blank()) { 
		// how much sense does the following make when appearance is computed text from value??
		// second conjunct added 261000
		// !is_blank() added 170701 since if for example in thought bubble no need to keep the appearance - best to recompute it when needed
		appearance->dump(stream,just_prepare,FALSE,TRUE); // not really on nest but pretty close to the same idea
	} else {
		stream.put(NOTHING_MARKER);
	};
	return(TRUE);
};

/*
   if (appearance != NULL) {  
		switch (identifier) {
			case FILE_TO_PICTURE_REMOTE:
				// commented out on 120999 the following so treated just like sound remotes
				// appearance dumped afterwards
//				appearance->dump(stream,just_prepare);
//				return;
			case FILE_TO_SOUND_REMOTE:
         case TEXT_TO_MCI_REMOTE:
         case TEXT_TO_SPEECH_REMOTE:
				if (!Remote_Integer::dump(stream,just_prepare,is_controlee,on_nest)) return(FALSE); 
				// commented out the following on 120999 since I don't want to create a new file format
				// just before big Playground show
				// implemented on 050100
				appearance->dump(stream,just_prepare,FALSE,TRUE); // not really on nest but pretty close to the same idea
				return(TRUE);
			case COLLIDEE_REMOTE: // others too?
				if (!active() && !blank) { // e.g. in the thought bubble
#if TT_DEBUG_ON
					if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
						print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
						tt_error_file() << "About to dump appearance of ";
						print(tt_error_file());
						tt_error_file() << endl;
						tt_debug_dump_depth++;
					};
#endif
					appearance->dump(stream,just_prepare);
#if TT_DEBUG_ON
					if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
						print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
						tt_error_file() << "Finished dumping appearance of ";
						print(tt_error_file());
						tt_error_file() << endl;
						tt_debug_dump_depth--;
					};
#endif
					return(TRUE);
				} else break; // otherise fall thru
      };
   }; // not dumping a NOTHING_MARKER for appearance since handled specially above
   return(Remote_Integer::dump(stream,just_prepare,is_controlee));
*/

#if TT_XML

// I assume this is fine for Remote_Address and that Force_Remote_Integer doesn't need special handling --- check!!!!

//xml_element *Remote_Picture::xml_create_element(xml_document *document) {
//   //if (tt_copying_entire_thought_bubble && appearance != NULL && !is_blank()) { // new on 231002
//   //   return(appearance->xml_create_element(document));
//   //};
//   return(Remote_Integer::xml_create_element(document));
//}

void Remote_Picture::xml(xml_element *element, xml_document *document) {
   //if (tt_copying_entire_thought_bubble && appearance != NULL && !is_blank()) { // new on 231002
   //   appearance->xml(element,document);
   //   return;
   //};
//#if TT_POST327
//	if (textual()) {
//		int language_index = xml_get_attribute_int(xml_get_document_element(document),L"Language",-1); // new on 131103
//		if (language_index == -1 || natural_language() != (NaturalLanguage) language_index) 
//#else
	if (textual() && (tt_dump_reason == DUMPING_TO_NOTEBOOK_PAGE || natural_language() != tt_language)) {
		// updated 250204 to check tt_dump_reason and dump language 
		// if dumping a page since you might have a different natural language than the page this is on
		// and if you take the page and save it in another language version confusion results
//#endif
      xml_set_attribute(element,L"Language",natural_language()); 
      // shouldn't use number here??
   };
	// commented out on 210803 since this seems to bring back the container bug (where shooting is at the wrong level)
	//if (appearance == NULL && !active() && controlee != NULL) { 
	//	// new on 190803 to be sure "legacy" apps save and load correctly 
	//	// e.g. Playground2001 city the reset robot on the touch and appear picture on the side of the house with appearing and disappearing gadgets
	//	update_picture(TRUE);
	//};
	if (appearance != NULL && !tt_dumping_to_test_equality_or_describe) { // && !is_blank()) { 
		// !is_blank() since if for example in thought bubble no need to keep the appearance - best to recompute it when needed
		// removed !is_blank() test on 070503 since that breaks when when in thought bubble (and copy with 'O' gets broken)
      // but shouldn't be active and in thought bubble, right?? -- NO -- removed  && !active() on 231002 since these do occur in thought bubbles
      // also removed  && !remote_integer_whose_value_should_not_be_dumped(identifier) on 231002
      xml_element *appearance_element = create_xml_element(L"Appearance",document); // new on 081002 to wrap this in an Appearance tag
		appearance->xml_create_and_append_element(appearance_element,document)->Release();
      xml_append_child(appearance_element,element);
		if (followers != NULL) { // new on 240203
			followers->add_xml(element,document,L"OnFront",TRUE,TRUE);
		};
   };
	if (!viewing_front_side_flag) { // new on 201202
		xml_set_attribute(element,L"ViewingBackSide",1);
	};
	// no need to save original_of_appearance since recomputed as need
   Remote_Integer::xml(element,document);
};

void Remote_Picture::add_special_xml(xml_element *element, xml_document *document) {
	if (saved_controlees_followers != NULL) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	saved_controlees_followers->check_for_independence();
		//};
		saved_controlees_followers->add_xml(element,L"SavedParts",document); 
	};
	if (sensor_applied_to_house != NULL) {
		xml_append_child(sensor_applied_to_house->xml_path(document),element); // wrap with L"AppliedToHouse" -- is Bird OK?
	};
	Remote_Integer::add_special_xml(element,document);
};

boolean Remote_Picture::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) { 
      case REMOTE_CONTROL_TAG: {
         int language_index = xml_get_attribute_int(node,L"Language",-1);
         if (language_index == -1) {
				language_index = tt_xml_language_index; 
				// new on 131103 - this is read from the document or the PAGE (which is why this change was necessary)
//          language_index = xml_get_attribute_int(xml_get_document_element(xml_get_document(node)),L"Language",-1);
         };
			if (language_index > 0) { // 0 is bad news - updated 131103
//       if (language_index >= 0) {
            set_language((NaturalLanguage) language_index);
         };
         return(TRUE);
                               };
      //case VALUE_TAG: // just inherit or does this need to do extra???
      //   set_value(xml_get_number_value(node));
      //   return(TRUE);
      case APPEARANCE_TAG:
         // new on 081002
         // prior to 231002 was set_appearance but that does too much work here and messes up the size 
			// (assumes this rather than appearance "knows" the size)
			if (appearance != NULL) { // new on 070103
				appearance->destroy();
			};
			appearance = xml_get_sprite(node); // added first_node_that_is_an_element on 231002 - rewrote on 071102 for clarity
         if (appearance != NULL) {
            appearance->save_size(); // in case sensor has no leader so good size is what it was before
				if (appearance->kind_of() == PICTURE && tt_log_version_number >= 56) { 
					// new on 141004 so that for example hit or miss sensors don't forget their state
					// added tt_log_version_number check on 181204
					set_long_value(compute_picture_id(appearance->appearance_code(),appearance->current_parameter()));
				};
         };
         return(TRUE);
		case SAVED_PARTS_TAG:
			destroy_sprites(saved_controlees_followers); // new on 070103
			saved_controlees_followers = xml_load_sprites(node);
			return(TRUE);
		case HOUSE_LOCATION_TAG: 
			sensor_applied_to_house = house_at_location(node);
			return(TRUE);
		case ON_FRONT_TAG: // new on 240203
			// if "reloading" shouldn't followers list be reset too? -- 
			// yes caller should call remove_all_followers and release_all_rerferences
         add_follower(xml_get_sprite(node),TRUE,INSERT_AT_END,TRUE); 
			// worry about something going wrong and having a NULL follower? - asked 240203
         return(TRUE);
      case NO_MORE_TAGS:
			viewing_front_side_flag = !xml_get_attribute_int(node,L"ViewingBackSide",0); // new on 120504
			version = xml_get_attribute_int(node,L"Version",1); // new on 120504 -- if version isn't saved then set to 1
         if (active()) { // conditional new on 231002
            update_picture(TRUE); // new on 091002 since value set while inactive
         };
         // and do the following (new on 121102)
      default:
         return(Remote_Integer::handle_xml_tag(tag,node)); 
   };
};

#endif

boolean Remote_Picture::numeric_value_meaningful() { // new on 140504
	switch (identifier) {
		case ANCESTORS_REMOTE:
		case CHILDREN_REMOTE:
		case COLLIDEE_REMOTE:
		case TEXT_TO_MCI_REMOTE:
		case TEXT_TO_SPEECH_REMOTE:
		case ROOM_DECORATION_REMOTE:
		case HOUSE_DECORATION_REMOTE:
		case ROOF_DECORATION_REMOTE:
		case CLIPBOARD_REMOTE:
			return(FALSE);
		default:
			return(TRUE);
	};
};

void Remote_Picture::set_value(NumberValue *new_value, // int min_text_length,
										 boolean ignore_change,
										 boolean sizes_constrained,
										 boolean update) {
	if (!numeric_value_meaningful()) {
		Rational::set_value(new_value,ignore_change,sizes_constrained,FALSE); 
		// but set it anyway in case needs to have 0 or something - 
		// could rationalize this someday
		return; // new on 140504 since value doesn't matter for these sensors -- and can cause infinite recursion
	};
	long old_value;
	if (update) { // condition new on 140504 since don't need this otherwise
		current_long_value(old_value);
	};
	Remote_Integer::set_value(new_value, //min_text_length,
									  ignore_change,sizes_constrained,update); // update was FALSE prior to 020302
	long new_long_value;
	current_long_value(new_long_value);
	if (update && ok_to_activate() && (old_value != new_long_value || appearance == NULL)) { // active() conjunct added on 311000
      // ok_to_activate was active prior to 210802 but don't want this to happen in the notebook unless the pages its open to
		// added update to conditions on 040603 to stop infinite recursion
		update_picture(TRUE); // TRUE added on 260900 so addresses are updated when changed by the keyboard
	};
};

//void Remote_Picture::update_text_and_widths(//int min_text_length,
//														  boolean sizes_constrained) {
//	if (appearance == NULL && text_identifier(identifier)) { 
//		Remote_Integer::update_text_and_widths(sizes_constrained);
//	}; // otherwise do nothing
//};

ReleaseStatus Remote_Picture::item_released_on_top(Sprite *item, Sprite *by,
																	SpritePointer &recipient, int level) {
   boolean ok_to_receive_item = FALSE;
   boolean abort = FALSE;
	switch (identifier) {
      case CLIPBOARD_REMOTE:
         if (!item->is_tool() && item->kind_of() != TOOLBOX) {
            ok_to_receive_item = TRUE;
         } else {
            abort = TRUE;
         };
         break;
      case FILE_TO_SOUND_REMOTE:
      case FILE_TO_PICTURE_REMOTE:
      case TEXT_TO_MCI_REMOTE:
      case TEXT_TO_SPEECH_REMOTE:
         if (appearance == NULL) {
            string file_name;
            long file_name_length;
            if (item->current_text(file_name,file_name_length)) {
               // for the first 2 I could check if file exists here and abort if not
               ok_to_receive_item = TRUE;
            } else {
               abort = TRUE;
            };
         } else {
            abort = TRUE;
         };
         break;
		case CHILDREN_REMOTE:
		case ANCESTORS_REMOTE:
			if (appearance == NULL) {
				update_picture();
			};
			if (appearance != NULL) {
				appearance->set_size_and_location_to_those_of(this); // added on 090599 to be sure the following works OK
				return(appearance->item_released_on_top(item,by,recipient,level));
			};
			break;
		case STREET_REMOTE:
		case AVENUE_REMOTE:
			ok_to_receive_item = TRUE; // like numbers
			break;		
	};
   if (abort) {
      by->action_aborted();
      return(RELEASE_IGNORED);
   };
   if (ok_to_receive_item) {
      if (receive_item(item,by,default_duration(),this,item)) { // checked for failure on 010799
			record_event(RELEASE_ITEM_ON,by,floor,this);
			item->now_released_on(this,by);
			if (by->is_action_aborted()) {
				return(RELEASE_IGNORED);
			} else {
				return(RELEASED_ON_NUMBER); //ok?
			};
		} else {
			return(RELEASE_IGNORED);
		};
   };
	return(Remote_Integer::item_released_on_top(item,by,recipient,level));
};

boolean Remote_Picture::receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, 
												 Sprite *original_item) {
   if (inside_vacuum()) {
      by->abort_action(IDS_ROBOT_MISSING_RECIPIENT_ABORT);
      return(FALSE);
   };
	set_saved_by(by);
   switch (identifier) {
		case CLIPBOARD_REMOTE:
      case FILE_TO_PICTURE_REMOTE:
      case FILE_TO_SOUND_REMOTE:
      case TEXT_TO_MCI_REMOTE:
      case TEXT_TO_SPEECH_REMOTE:
			if (appearance == NULL) {// || identifier != CLIPBOARD_REMOTE) {
				Background *items_floor = item->pointer_to_background();
				if (duration > 0) {
					call_in_a_mouse(this,item,items_floor,by,FALSE,TRUE,duration,original_recipient,original_item);
				} else {
					set_to_future_value(item,original_recipient,by);
					item->remove_from_floor(FALSE);
					if (!item->to_be_smashed()) {
						item->destroy();
					};
					action_completed(); // OK since saved_by is same as by
				};
				return(TRUE);
			} else {
            // something already on clipboard or picture import -- remove it first
            ::action_aborted(saved_by);
				return(FALSE);
			};
		case AVENUE_REMOTE:
		case STREET_REMOTE:
//			{boolean result = Remote_Integer::receive_item(item,by,duration,action_status,original_recipient);
//			 set_active(FALSE); // new on 160300
//			 return(result);
//			};
			set_active(FALSE); // re-written on 290301 so that it doesn't recompute its address before becoming inactive
			return(Remote_Integer::receive_item(item,by,duration,original_recipient,original_item));
      case ROOM_DECORATION_REMOTE:
      case HOUSE_DECORATION_REMOTE:
      case ROOF_DECORATION_REMOTE:
			if (!floor->outside()) {
				switch (item->kind_of()) {
					case INTEGER:
					case TEXT:
						if (by->kind_of() != PROGRAMMER) { // robots have no geometry control -- added 270199
							//if (!showing_on_a_screen()) { 
							//// new on 030405 so geometry works out as if done on floor -- not needed (030405)
							//	ultimate_container()->set_to_good_size(tt_toolbox); // source just as if on floor
							//};
							item->set_size_and_location_to_those_of(this);
						};
						call_in_a_mouse(this,make_indirect_picture(item),item->pointer_to_background(),by,FALSE,FALSE,duration,
											 original_recipient,original_item);
						return(TRUE);
					case PICTURE:
						if (item->is_flipped()) return(FALSE); // new on 210601
						if (by->kind_of() != PROGRAMMER) { // robots have no geometry control -- added 270199
							//if (!showing_on_a_screen()) { // new on 030405 so geometry works out as if done on floor
							//	ultimate_container()->set_to_good_size(tt_toolbox);
							//};
							item->set_size_and_location_to_those_of(this);
						};
						call_in_a_mouse(this,item,item->pointer_to_background(),by,FALSE,FALSE,duration,
							             original_recipient,original_item);
						return(TRUE);
					default:
						return(FALSE);
				};
			};
		case CHILDREN_REMOTE:
		case ANCESTORS_REMOTE:
			if (appearance == NULL) {
				update_picture();
			};
			if (appearance != NULL) {
				return(appearance->receive_item(item,by,duration,original_recipient,original_item));
			};
			break;
	};
	if (item->kind_of() == INTEGER) {
		return(Remote_Integer::receive_item(item,by,duration,original_recipient,original_item));
	};
	by->abort_action_if_robot(IDS_SENSOR_UNDERNEATH,
									  MOUSE_WITH_HAMMER,
									  !told_user(TOLD_STUFF_ON_SENSORS));
	user_told(TOLD_STUFF_ON_SENSORS);
	return(FALSE);
};

boolean Remote_Picture::ok_for_mouse_to_temporarily_add_follower() {
   return(identifier != ROOM_DECORATION_REMOTE && identifier != HOUSE_DECORATION_REMOTE && identifier != ROOF_DECORATION_REMOTE);
};

void position_and_add_to_wall(Sprite *wall_remote, Sprite *addition, Room *room) {
//	addition->update_display(); // new on 160601 so that geometry doesn't drift due to offsets
   city_coordinate addition_width = addition->current_width();
   city_coordinate addition_height = addition->current_height();
   city_coordinate wall_remote_width = wall_remote->current_width();
   city_coordinate wall_remote_height = wall_remote->current_height();
   city_coordinate addition_delta_x = addition->current_llx()-wall_remote->current_llx();
   city_coordinate addition_delta_y = addition->current_lly()-wall_remote->current_lly();
   addition->set_size_and_location((addition_width*wall_width)/wall_remote_width,
                                   (addition_height*wall_height)/wall_remote_height,
                                   wall_x+(addition_delta_x*wall_width)/wall_remote_width,
                                   wall_y+(addition_delta_y*wall_height)/wall_remote_height);
   room->add_decoration(addition);
};

void position_and_add_to_house(Sprite *decoration_remote, Sprite *addition, House *house, boolean from_side) {
	// updated from true_ to current_ on 121204
   city_coordinate addition_width = addition->current_width();
   city_coordinate addition_height = addition->current_height();
   city_coordinate decoration_remote_width = decoration_remote->current_width();
   city_coordinate decoration_remote_height = decoration_remote->current_height();
   city_coordinate addition_delta_x = addition->current_llx()-decoration_remote->current_llx();
   city_coordinate addition_delta_y = addition->current_lly()-decoration_remote->current_lly();
   // set size and position as relative coordinates where 1000 is full width or height of house view
   addition->set_size_and_location((addition_width*1000)/decoration_remote_width,
                                   (addition_height*1000)/decoration_remote_height,
                                   (addition_delta_x*1000)/decoration_remote_width,
                                   (addition_delta_y*1000)/decoration_remote_height);
   house->add_decoration(addition,from_side);
};

void Remote_Picture::set_to_future_value(Sprite *other, Sprite *original_recipient, Sprite *by) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 12399 && other->debug_counter == tt_debug_target) {
		tt_error_file() << "Remote picture set to value of debug target." << endl;
	};
#endif
   switch (identifier) {
      case CLIPBOARD_REMOTE:
         put_on_clipboard(other);
         break;
      case ROOM_DECORATION_REMOTE:
      case HOUSE_DECORATION_REMOTE:
      case ROOF_DECORATION_REMOTE:
         {
          Sprite *container = ultimate_container();
          if (container == NULL) break;
          Background *floor = container->pointer_to_background();
          if (floor == NULL || floor->outside()) break;
          other->remove_from_floor();
          // add decoration to room, house, or floor positioned proportionately to how item is relative to appearance
//          appearance->receive_item(other,NULL,0,NULL);
          //switch (identifier) { // commented out on 021203 since redundant
          //   case ROOM_DECORATION_REMOTE:
          //   case ROOF_DECORATION_REMOTE:
          //   case HOUSE_DECORATION_REMOTE:
		 	 other->set_subpicture(TRUE); 
			 // moved here from after the following conditional on 280204 since this will change text to show_some
			 // and then copies will be show_some as well
          if (saved_by != NULL && saved_by->kind_of() == ROBOT_IN_TRAINING) {
				 UnwindProtect<boolean> set(tt_dealing_with_follower_change,TRUE);
				 // just add follower but don't trigger changes to other remotes or the house -- 220299
				 add_follower(other);
			 } else { // added on 220299 since training a robot should not effect the roof, etc.
				 // prior to 150603 the following were calls to top_level_copy
				 // but that interferred with robots changing the decoration
				 if (identifier == ROOM_DECORATION_REMOTE) {
					 position_and_add_to_wall(this,other->copy(),floor->pointer_to_room());
				 } else {
					 position_and_add_to_house(this,other->copy(),floor->pointer_to_house(),(identifier == HOUSE_DECORATION_REMOTE));
				 };
				 add_follower(other); // ,FALSE);  // don't clip
			 };
          //      break;
          //};
//
// following provided only partial confusing updates if say standing in room and robots are working on the floor
//          tt_global_picture->update_room_decoration();
         };
         break;
		default:
			// following moved here since not appropriate for identifiers above
			wide_string text_dropped;
			long text_length;
			boolean is_text = other->current_wide_text(text_dropped,text_length);
			if (!is_text || text_length == 0) { // not really a text pad
				// conditional new on 141299 but might not happen any more now that picture::current_wide_text is defined
//				log("Text pad didn't have any text when dropped on sensor that wanted some text.",FALSE,TRUE); 
				// -- moved below on 140301 so only is logged when appropriate
				text_dropped = NULL;
				text_length = 0;
			};
			// commented out on 220599 due to new policy where strings in Text are always null terminated
//			if (is_text && text_dropped != NULL) text_dropped[text_length] = '\0'; // terminate
			switch (identifier) {
				case FILE_TO_PICTURE_REMOTE: 
					if (text_dropped != NULL) { // new on 141299
						string file_name = copy_narrow_string(text_dropped); // pulled out here on 200201
						boolean ok;
						Picture *picture = new UserPicture(file_name,ok,FALSE); // FALSE added 200201 so there are no warnings if this 
						if (ok) {
							picture->save_size();
							picture->set_true_size_and_location(width,height,llx,lly);
							set_appearance(picture); // new on 200201 to use set_appearance to maintain ref counts
						} else {
							picture->destroy();	
							Sprite *sprite = NULL;
							if (!already_has_extension(file_name,"wav")) { // sounds on the File to Picture sensor is just too weird
								boolean aborted = FALSE; // new on 200105
								sprite = sprite_from_file_name(file_name,aborted); 
								// new on 200201 so you can drop URL's of TT or TXT or HTML or whatever here
							};
//							if (sprite != NULL && sprite->kind_of() != PICTURE) { // might be a text pad for example
//								picture = make_indirect_picture(sprite,FALSE);
//							} else {
//								picture = (Picture *) sprite;
//							};
							delete [] file_name; // new on 200201 - otherwise string is part of UserPicture's image
							set_appearance(sprite); // new on 210201 since things like trucks don't work well as an indirect_picture
						};					
					};
					break;
				case FILE_TO_SOUND_REMOTE:
				case TEXT_TO_MCI_REMOTE:
				  {
					if (appearance != NULL) {
						close_current_sound();
						appearance->destroy();
						appearance = NULL;
					};
					ascii_string command = NULL;
					ascii_string sound_file_name = NULL;
					ascii_string narrow_text_dropped = copy_narrow_string(text_dropped);
					FileNameStatus file_status = FILE_ON_HARD_DRIVE; // default
					if (identifier == FILE_TO_SOUND_REMOTE) {
						sound_file_name = existing_file_name(file_status,narrow_text_dropped,"wav","NewUsers",TRUE,FALSE);
						if (sound_file_name == NULL) {
							say_file_name_error(IDS_NO_FILE_NAME_FOR_USER_MEDIA_FILE,narrow_text_dropped,4,"wav");
							break;
						} else {
							command = append_ascii_strings("play ",sound_file_name); // why? why not narrow_text_dropped??
						};
					} else if (identifier == TEXT_TO_MCI_REMOTE) {
						command = copy_narrow_string(text_dropped); 
					};
#if TT_DIRECTX
					if (identifier == FILE_TO_SOUND_REMOTE) { // && stricmp(sound_file_name+strlen(sound_file_name)-4,".wav") == 0) {
						// commented out the WAV extension test on 100401 since the system cached file name won't have the WAV on it still
						// and the following behavior MCI doesn't makes sense anyway
						set_appearance_to_wave_sound(text_dropped); // play removed on 210599
		/* commented out on 241199 since
		 // shouldn't make the sound until you turn it on - since might be on the back of a picture, etc.
 						int index;
						sound_buffer sound = load_sound_from_file(sound_file_name,index);
						if (sound != NULL) {
							if (ok_to_make_a_sound(floor)) play_sound_buffer(sound);
		//               set_appearance_to_wave_sound(append_strings("play ",text_dropped));
							set_appearance_to_wave_sound(text_dropped); // play removed on 210599
		//               appearance = new Text(llx,lly,text_dropped);
		//               if (leader == NULL) {
		//                  set_size_and_location(other->current_width(),
		//                                        other->current_height(),
		//                                        other->current_llx()+tile_width/10,
		//                                        other->current_lly()+tile_height/10);
		//               };
							extra_pointer = sound;
						} else {
							say_file_name_error(IDS_TROUBLES_READING_SOUND_FILE,narrow_text_dropped,4,"wav");
						};
						*/
						break;
					};
#endif
					if (!ok_to_make_a_sound(floor) || tt_main_window->MCI_send_string(command)) {
						set_appearance_to_MCI_sound(copy_wide_string(command));
		//            appearance = new Text(llx,lly,text_dropped);
		//            if (leader == NULL) {
		//               set_size_and_location(other->current_width(),
		//                                     other->current_height(),
		//                                     other->current_llx()+tile_width/10,
		//                                     other->current_lly()+tile_height/10);
		//            };
					};
					if (command != NULL) delete [] command;
//					if (file_status == FILE_ON_INTERNET_AND_DELETE_COPY) DeleteFile(sound_file_name); // obsolete so commented out on 191201
					if (sound_file_name != NULL) delete [] sound_file_name;
					delete [] narrow_text_dropped;
				 };
				  break;
				case TEXT_TO_SPEECH_REMOTE:
					if (appearance != NULL) {
						close_current_sound();
						appearance->destroy();
						appearance = NULL;
					};
					set_appearance_to_speech_sound(text_dropped);
		//           appearance = new Text(llx,lly,text_dropped);
		//           if (leader == NULL) {
		//               set_size_and_location(other->current_width(),
		//                                     other->current_height(),
		//                                     other->current_llx()+tile_width/10,
		//                                     other->current_lly()+tile_height/10);
		//           };
#if TT_TEXT_TO_SPEECH
					if (ok_to_make_a_sound(floor)) {
						if (text_dropped != NULL) { // was just speak prior to 100800
							speak_after_preprocessing(text_dropped,wide_string_length(text_dropped));
						};
					};
#endif
					break;
			  default:
				 if (other->kind_of() == INTEGER) {
					 Remote_Integer::set_to_future_value(other,original_recipient,by);
//					 set_active(FALSE); // new on 130300 since changed remote
					 changed_by_keyboard(); // new on 160300
					 return; // new on 140301
				 };
			};
			if (!is_text || text_length == 0) { // not really a text pad
				// conditional new on 141299 but might not happen any more now that picture::current_wide_text is defined
				log("Text pad didn't have any text when dropped on sensor that wanted some text.",FALSE,TRUE); // moved here on 140301
			};	
   };
};

// commented out on 260499 since these remotes are rectangular and know their size
// this broke too many other things - restored right away
city_coordinate Remote_Picture::true_width(boolean including_followers) {
   if (appearance != NULL) {
      return(appearance->true_width(including_followers));
   } else {
      return(Remote_Integer::true_width(including_followers));
   };
};

city_coordinate Remote_Picture::true_height(boolean including_followers) {
   if (appearance != NULL) {
      return(appearance->true_height(including_followers));
   } else {
      return(Remote_Integer::true_height(including_followers));
   };
};

void Remote_Picture::close_current_sound() {
   switch (identifier) {
      case FILE_TO_SOUND_REMOTE:
//      case TEXT_TO_MCI_REMOTE:
//#if TT_DIRECTX // commented out on 201202 since is obsolete
//         if (extra_pointer != NULL) {  // must close the sound even if not showing_on_a_screen
//            stop_sound((sound_buffer) extra_pointer);
////            release_sound_buffer((sound_buffer) extra_pointer); // commented out on 201199 since buffers are now shared
//            extra_pointer = NULL;
//            return;
//         };
//#endif
         if (showing_on_a_screen()) {   // otherwise is a probably a robot on the other side so don't close the sound
            string old_file_name;
            long old_file_name_length;
            if (appearance->current_text(old_file_name,old_file_name_length)) {
               old_file_name[old_file_name_length] = 0; // terminate string in case
               ascii_string command = append_ascii_strings("close ",u2a(old_file_name));
               tt_main_window->MCI_send_string(command,FALSE);
               delete [] command;
            };
         };
         break;
      case TEXT_TO_SPEECH_REMOTE:
#if TT_TEXT_TO_SPEECH
			if (!tt_shutting_down && showing_on_a_screen()) {
				stop_speaking();  // this stops all speaking not just this one...
			};
			// added !tt_shutting_down on 300500
#endif
         break;
   };
};

Sprite *Remote_Picture::select(TTRegion *selection_region, SelectionReason reason, Sprite *in_hand) {
   if (appearance != NULL) {
      if (decoration_identifier(identifier)) {
         switch (reason) {
            case MIGHT_USE_VACUUM:
               {
               Sprites *remaining = followers;
		         while (remaining != NULL) {
			         TTRegion subpicture_region;
			         Sprite *sub_picture = remaining->first();
			         sub_picture->collision_region(subpicture_region);
			         if (subpicture_region.overlaps(selection_region)) {
				         return(sub_picture->select(selection_region,reason,in_hand));
			         };
			         remaining = remaining->rest();
		         };
//             Sprite *selection = appearance->select(selection_region,reason,in_hand);
//             if (selection == appearance) return(this); // can't remove the appearance from the remote
//             return(selection);
               };
               break;
            case MIGHT_DROP_ITEM:
               switch (in_hand->kind_of()) {
                  case PICTURE:
                  case TEXT:
                  case INTEGER:
                     return(this);
                  default:
                     return(NULL);
               };
         };
      };
	} else if (reason == MIGHT_DROP_ITEM && tt_log_version_number >= 38) {
		// new on 290204
		switch (identifier) {
			case TEXT_TO_SPEECH_REMOTE:
			case TEXT_TO_MCI_REMOTE:
			case FILE_TO_PICTURE_REMOTE:
			case FILE_TO_SOUND_REMOTE:
			case FILE_TO_MOVIE_REMOTE:
				if (in_hand->looks_like_is_of_type(TEXT)) {
					return(this);
				} else {
					return(NULL);
				};
			case CLIPBOARD_REMOTE:
				return(this); // is there anything you can't drop on the clipboard (shouldn't be called with tool in hand)
			default:
				return(Remote_Integer::select(selection_region,reason,in_hand)); // is this right for all the types
		};
   };
   return(Remote_Integer::select(selection_region,reason,in_hand)); // was Sprite:: prior to 290204
};

boolean update_before_vacuuming(RemoteIdentifier identifier) { // new on 080301
	switch (identifier) {
		case CHILDREN_REMOTE:
		case ANCESTORS_REMOTE:
			return(TRUE); // not clear if there are others
		default:
			return(FALSE);
	};
};

Sprite *Remote_Picture::selected(SelectionReason reason, Sprite *by) {
	if (reason == VACUUM_USED && appearance != NULL) {
      switch (identifier) {
          case FILE_TO_SOUND_REMOTE:
             close_current_sound(); // and the following
		    case CLIPBOARD_REMOTE:
          case FILE_TO_PICTURE_REMOTE:
          case TEXT_TO_MCI_REMOTE:
          case TEXT_TO_SPEECH_REMOTE:
			 case COLLIDEE_REMOTE:
			 case CHILDREN_REMOTE:
			 case ANCESTORS_REMOTE:
				 {
			    Sprite *result;
				 if (identifier == COLLIDEE_REMOTE) { // didn't copy the stuff on the back until needed which is now
//					 if (reason == VACUUM_USED) { // commented out on 300301 since already checked this above
					 result = appearance; // ->copy(); // copy() added on 020300
					 Sprite *controlee = ((Picture *) appearance)->pointer_to_controlee();
					 if (controlee != NULL) {
						 appearance->set_size(controlee->current_width(),controlee->current_height(),FOLLOWING_CONTROLEES_CHANGE);
						 appearance->save_size();
					 };
					 tt_just_vacuumed = result; // just used to create paths when training a robot to do this
					 tt_just_vacuumed_remote = this; // new on 091000
//					 } else {
//						 result = this; // whole thing
//					 };
//					 Picture *collidee = (Picture *) ((Picture *) appearance)->extra_pointer_while_inaccessible();
//  					 if (collidee != NULL) {
////						 Sprite *new_appearance = collidee->copy();
//						 // changed on 210499 so that collidee is the remote looks of the collidee
//						 Picture *new_appearance = new Picture();
// 						 new_appearance->set_controlee(collidee);
//					    new_appearance->set_size_and_location_to_those_of(appearance);
//						 new_appearance->set_ignore_controlee_when_size_changes(TRUE);
//					    set_appearance(new_appearance);
//					 };
				 } else {
					 if (update_before_vacuuming(identifier)) { // conditional new on 080301
						 update_picture(floor != NULL && !floor->inside_rule_body()); // new on 070301 -- was just TRUE prior to 060103
					 };
					 result = appearance; // ->copy(); // copy added 280200 since appearance stays put but you suck off a copy
					 // this problem with the following is that robots need this to vacuum up parts or container sensor value
					 // moved following to vacuum spit
//					 appearance->set_is_picture(FALSE); // new on 060400 since this flag is used when dereferencing robot's path
					 // this is a change of policy for some of the remotes
					 tt_just_vacuumed = appearance; // just used to create paths when training a robot to do this
					 tt_just_vacuumed_remote = this; // new on 091000
				 };
				 // commented out on 020300 since 
//		       set_appearance(NULL); // was appearance = NULL; prior to 280200
				 appearance = NULL; // restored on 020300 since released above - is there a ref count risk here??
             if (identifier == CLIPBOARD_REMOTE) {
					 set_appearance(NULL); // moved here on 020300 - but isn't always already NULL??
        	       tt_main_window->clear_clipboard();
             };
		       if (floor != NULL && result != NULL) { // result != NULL new on 220803
			       floor->add_item(result);
		       };
             if (leader == NULL && identifier != COLLIDEE_REMOTE) { // added 210499
                city_coordinate good_width, good_height;
                good_size(good_width,good_height);
                set_size_and_location(good_width,good_height,
                                      llx+(width-good_width)/2,
                                      lly+(height-good_height)/2);
             };
				 if (identifier == COLLIDEE_REMOTE && !tt_screen->inside_thought_bubble() && controlee != NULL) {
					 controlee->check_collisions(TRUE);
				 };
				 update_picture(); // added on 070499 to ensure that for example CHILDREN_REMOTE is updated after vacuuming
		       return(result);
             };
//          case ROOM_DECORATION_REMOTE:
//             return(appearance->selected(reason,by));
      };
	};
	return(Remote_Integer::selected(reason,by));
};

void Remote_Picture::set_inside_vacuum(boolean inside, boolean recur) {
//   if (inside_vacuum_flag == inside) return; // new on 170702 to ignore calls that already know about - 
	// commented out on 190702 since might not have been called with recur
   boolean old_inside_vacuum_flag = inside_vacuum_flag;
   Remote_Integer::set_inside_vacuum(inside,recur); // was missing prior to 160702
   if (inside && !old_inside_vacuum_flag && appearance != NULL) {
      // added !old_inside_vacuum_flag on 190702
      close_current_sound(); // this is no-op if this remote isn't responsible for a sound
   };
};

Sprite *Remote_Picture::used(Sprite *subject, Sprite *by,
							        boolean button_unchanged, boolean record_action,
							        millisecond duration, boolean modified, Sprite *original_subject) {
	if (original_subject == NULL) { // new on 140305 since other code assumes this is always bound
		original_subject = subject;
		if (original_subject == NULL) {
			original_subject = this;
		};
	};
	// what about recording this properly????
	if (enumeration_type()) {
//      if (tt_new_input_scheme) {
         return(respond_to_keyboard('+',FALSE,FALSE,by,record_action,original_subject));
//      } else {
//         return(respond_to_keyboard(modified?'-':'+',FALSE,by,record_action,action_status));
//      };
   } else {
		switch (identifier) {
			case FILE_TO_PICTURE_REMOTE:
				if (appearance != NULL) {
		//          return(appearance->used(subject,by,button_unchanged,
		//                                  screen,duration,action_status,modified));
					by->abort_action(IDS_CANT_FLIP_USED_FILE_TO_PICTURE_REMOTE);
				} else {
					by->abort_action(IDS_CANT_FLIP_UNUSED_FILE_TO_PICTURE_REMOTE);
				};
				break;
         case TEXT_TO_SPEECH_REMOTE: 
			case FILE_TO_SOUND_REMOTE:
			case TEXT_TO_MCI_REMOTE:
				// recorded a bit oddly but should work fine
				respond_to_keyboard(tt_run_character,FALSE,FALSE,by,record_action,original_subject);
				break;
			case COLLIDEE_REMOTE:
            by->abort_action(IDS_CANT_FLIP_REMOTES); 
				// added on 181002 to give a good error message and so doesn't hang since didn't complete action_status
//            if (appearance != NULL) {
//					appearance->set_background(floor); // still needed??
//					return(appearance->used(subject,by,button_unchanged,record_action,duration,action_status,modified,original_subject));
//				} else 
				break;
			case CHILDREN_REMOTE:
			case ANCESTORS_REMOTE:
				// new on 160101
				if (!blank) {
					viewing_front_side_flag = !viewing_front_side_flag;
					update_picture(TRUE); 
					// why regardless (TRUE)? -- is this due to assignment rather than UnwindProtect of updating_remote_control_for_controlee
					if (record_action) { // new on 230101
						record_event(APPLY_GRASPED_ITEM,by,floor);
					};
				}; // else warn or beep??
				by->completed();
				break;
			default:
				if (modified) {
//					if (!tt_new_input_scheme) {
//						set_value(value-1);
//						completed(action_status);
//					};
				} else {
					if (record_action) { // new on 230101
						record_event(APPLY_GRASPED_ITEM,by,floor);
					};
					long long_value;
					if (current_long_value(long_value)) {
						set_long_value(long_value+1);
					};
				};
				by->completed(); // moved out of the conditional on 160101
		};
	};
   return(this);
};

Sprite *Remote_Picture::respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected,
													     Sprite *by, boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless) {
//	if (read_only()) {
//		by->abort_action(action_status,IDS_CANT_CHANGE_SENSOR);
//		return(this);
//	};
	if (appearance == NULL || read_only()) { // new on 061000 that if read_only pass this up to super (so can turn it on and off)
		return(Remote_Integer::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,size_constrained_regardless));
   };
   switch (identifier) {
      case TEXT_TO_SPEECH_REMOTE:
      case FILE_TO_SOUND_REMOTE:
      case TEXT_TO_MCI_REMOTE:
			if (appearance != NULL) {
				appearance->respond_to_keyboard(key,extended,robot_selected,by,FALSE,original_recipient,size_constrained_regardless);
			};
         break;
      case ROOM_DECORATION_REMOTE:
      case HOUSE_DECORATION_REMOTE:
      case ROOF_DECORATION_REMOTE:
        // could switch between styles A, B, and C...
        return(this); 
		  // why not make the decoration sensors above also turn on and off as below??
		case COLLIDEE_REMOTE: 
		case ANCESTORS_REMOTE:
		case CHILDREN_REMOTE:
		case FILE_TO_PICTURE_REMOTE: 
			// new on 050605 since if typing 'f' to this sensor should get the warning generated by the following
			// new on 191000 since can only turn these on and off
			return(Remote_Integer::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,
																	 size_constrained_regardless));
      default:
			if (appearance->kind_of() != PICTURE && !extended) { 
				// extended test new on 010704 since Delete caused pad to be stopped (treated like '.')
				Sprite *result; 
				long long_value;
				current_long_value(long_value); // new on 030302
				if (key == '+') { // added first 2 branches on 250699 so you can cycle thru Yes/No
					// removed key == ' ' || on 231099 to be consistent with other uses
					set_long_value(long_value+1);
//					set_active(FALSE); // added on 250699 since over-ridden now
					changed_by_keyboard(); // added 160300
					update_picture(TRUE); // added on 250699 so that is updated even if inactive
//					return(this);
					result = Sprite::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,size_constrained_regardless); 
					// this replaces the above on 140301 
				} else if (key == '-' || rubout(key)) { // new on 010802 to treat rubout like -1
					set_long_value(long_value-1);
//					set_active(FALSE); // added on 250699 since over-ridden now
					changed_by_keyboard(); // added 160300
					update_picture(TRUE); // added on 250699 so that is updated even if inactive
//					return(this);
					result = Sprite::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,size_constrained_regardless); 
					// this replaces the above on 140301 
				} else {
					boolean previous_active = active();
					result = Remote_Integer::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,
																				size_constrained_regardless);
					if (previous_active != active()) { // if changed re-compute picture
						update_picture(TRUE); // added on 130300 
					};
				};
				return(result);
			};
			if (!extended) { // condition new on 020904 so confusion from using the keyboard's number pad is avoid
				SpriteCode picture_code;
				short int picture_index, new_picture_index;
				long long_value;
				current_long_value(long_value);
				decode_picture_id((short) long_value,picture_index,picture_code);
				if (key == '+') {
					// removed key == ' ' || on 231099 to be consistent with other uses
					new_picture_index = (short int) (picture_index+1); // set_value(value+1);
				} else if (key == '-') {
					new_picture_index = (short int) (picture_index-1); // set_value(value-1);
				} else if (key == tt_stop_character) { // new on 231099
					set_active(FALSE);
				} else if (key == tt_run_character) { // && tt_log_version_number > 14) {
					set_active(TRUE);
				} else { // and what about non-digits??
					new_picture_index = (short int) key; // how about 2 digit indices??
	//			   Remote_Integer::respond_to_keyboard(key,extended,robot_selected,by,record_action,action_status);
	//			   return;
				};
				short int picture_count = (short int) parameter_count(picture_code);
				new_picture_index %= picture_count;
				if (new_picture_index < 0) new_picture_index += picture_count;
				if (new_picture_index != picture_index) {
					set_long_value(compute_picture_id(picture_code,new_picture_index));
				};
			};
   };
//	if (record_action) record_event(KEYBOARD_CHARACTER,by,floor,this,TRUE,key);
//	completed(action_status);
	// commented out the above for the following -- why not Remote_Integer::respond_to_keyboard??
   return(Sprite::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,size_constrained_regardless));
};

void Remote_Picture::set_size_and_location(city_coordinate new_width,
												       city_coordinate new_height,
												       city_coordinate new_llx,
												       city_coordinate new_lly,
												       ChangeReason reason) {
   Sprite::set_size_and_location(new_width,new_height,new_llx,new_lly,reason); // Integer:: not appropriate
   if (appearance != NULL) {
      appearance->set_size_and_location(width,height,llx,lly,reason); // removed Sprite:: on 090102
   };
};

void Remote_Picture::update_display(const UpdateStatus update_status) {
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	Remote_Integer::update_display(update_status);
	if (appearance != NULL) {
		appearance->update_display(update_status); // new on 281101
	   if (active()) {
			if (!clean()) {
//				appearance->Sprite::set_size_and_location(width,height,llx,lly);
				appearance->set_size_and_location(width,height,llx,lly,UPDATING); // re-written on 090102
			};
			if (leader == NULL || ultimate_leader()->kind_of() != PICTURE) { // was == CUBBY prior to 020302 -- 
				// but that meant that notebooks and other things didn't always display right when containing live sensors
				// if on flip side of picture don't cycle since whole
				// picture gets redrawn then
				appearance->update_display(UPDATE_CHANGE_FROM_REMOTE); //update_status);
	//			if (appearance->kind_of() != PICTURE) { // condition new on 170301
	//				set_clean_status(appearance->current_clean_status()); // if animated text or number pad this will take care of things just fine
	//			} else if (!appearance->rectangular()) { // new on 170301 - has an animated background so it is dirty
	//				set_clean_status(DIRTY);
	//			};
				// rewritten on 180901 since things like hit or miss sensor stopped animating
				if (appearance->kind_of() != PICTURE || appearance->rectangular()) { 
					set_clean_status(appearance->current_clean_status()); // if animated text or number pad this will take care of things just fine
				} else { // new on 170301 - has an animated background so it is dirty
					set_clean_status(DIRTY);
				};
			};
		};
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
};

void Remote_Picture::display(SelectionFeedback selection_feedback, boolean followers_too, TTRegion *region) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Displaying remote picture that is a debug target.");
	};
#endif
//	if (rescale_if_camera_above(selection_feedback,followers_too,region)) return;  // made obsolete on 230804
	if // (blank && textual() && appearance != NULL) { // && tt_log_version_number > 14) { // added 220499
		// rewritten on 161202 since erased parts and containers looked bad - why check that it is not a picture??
		(blank && appearance != NULL) { // && appearance->kind_of() != PICTURE) {
//		appearance->become_blank(); // just in case -- kind of wasteful - commented out on 161202
		appearance->display(selection_feedback,followers_too,region);
		return;
	};
	if (pointer_to_top_of_stack() != NULL) { // new on 260500
		pointer_to_top_of_stack()->display(selection_feedback,followers_too,region);
		return;
	};
	city_coordinate adjusted_llx, adjusted_lly;
	city_coordinate adjusted_width, adjusted_height;
   city_coordinate real_width, real_height;
	if (region == NULL) {
		adjusted_llx = llx;
		adjusted_lly = lly;
		adjusted_width = width;
		adjusted_height = height;
      real_width = true_width();
      real_height = true_height();
	} else {
		adjusted_llx = region->min_x;
		adjusted_lly = region->min_y;
		adjusted_width = region->width();
		adjusted_height = region->height();
      real_width = adjusted_width;
      real_height = adjusted_height;
	};
	if (want_feedback(selection_feedback)) {
		adjusted_llx += selection_delta_x();
		adjusted_lly += selection_delta_y();
		// only supporting movement for now
	};
//	if (appearance != NULL) {
//		switch (appearance->kind_of()) {
//			case PICTURE:
//			  if (!((Picture *) appearance)->rectangular() || blank) {
				  // wouldn't appearance->display do this if non-blank??
//				  tt_main_window->select_stock_object(WHITE_BRUSH);
//				  tt_main_window->select_stock_object(BLACK_PEN);
//				  tt_screen->draw_rectangle(adjusted_llx,adjusted_llx+adjusted_width,
//													 adjusted_lly,adjusted_lly+adjusted_height);
//				  tt_main_window->select_stock_object(NULL_PEN);
//			  };
//			  break;
//		};
//		if (!blank) {
//			appearance->display(selection_feedback,followers_too,region);
//			return;
//		};
//	};
	boolean controlee_out_of_range = (controlee != NULL && !controlee->global() && controlee->on_back_of_picture_or_limbo()); 
	// new on 091100
   if (!blank && !controlee_out_of_range && active() && // added active() test on 111202
       (appearance != NULL || decoration_identifier(identifier))) { // if decoration without appearance will get one
		update_picture();
   };
	if (blank || appearance == NULL || identifier == CLIPBOARD_REMOTE || controlee_out_of_range) {
      // if identifier == FILE_TO_PICTURE_REMOTE and show_some then
      // this should happen too
		// clipboard remote has background and object
BEGIN_GDI
		HGDIOBJ old_selection = NULL;
		if (blank) {
			tt_main_window->select_stock_object(BLACK_PEN);
			bytes permutation = color_permutation(selection_feedback);
			if (permutation != NULL) {
				// new on 200901
				old_selection = tt_main_window->select_object(tt_main_window->create_solid_brush(compute_color(tt_white,
																																			  permutation)));
			} else {
				tt_main_window->select_stock_object(WHITE_BRUSH);
			};
		} else {
			tt_main_window->select_stock_object(WHITE_PEN);
			tt_main_window->select_stock_object(BLACK_BRUSH);
		};
		tt_screen->draw_rectangle(adjusted_llx,adjusted_llx+adjusted_width,
										  adjusted_lly,adjusted_lly+adjusted_height);
		tt_main_window->select_stock_object(NULL_PEN);
		if (old_selection != NULL) DeleteObject(tt_main_window->select_object(old_selection));
END_GDI
		if (controlee_out_of_range && identifier != ANCESTORS_REMOTE) return; 
		// added ANCESTORS_REMOTE test on 120504 so they don't go black
	};
	if (!blank && appearance != NULL) {
//      if (leader == NULL && !appearance->followers_completely_inside()) unclip(); // for the following
      if (!appearance->rectangular() && active() && !textual() && kind_of() != INTEGER) { 
			// draw backing for appearance -- added active() condition on 191000
			// on 160301 added last two conjuncts added since no need to do this 
			// if already have animated marquee for text and number pads
         if (want_feedback(selection_feedback)) {
            // recursive calls don't show frame
            selection_feedback = RECURSIVE_MOVEMENT_FEEDBACK;
         };
//         appearance->just_set_blank(TRUE);
//         appearance->display(selection_feedback,FALSE,region);
//         appearance->just_set_blank(FALSE);
BEGIN_GDI	
			if (cycle_background_color()) {
				tt_main_window->select_stock_object(NULL_PEN); 
				// was WHITE_PEN prior to 181000 since looks bad if drawn over by appearance of remote
				tt_screen->draw_rectangle(adjusted_llx,adjusted_llx+real_width,
												  adjusted_lly,adjusted_lly+real_height);
			};
//         tt_main_window->select_stock_object(NULL_PEN); // commented out on 181000
END_GDI
      };
      appearance->display(selection_feedback,followers_too,region);
      if (followers_too && followers != NULL) {
         coordinate saved_destination_llx = tt_destination_llx,
                    saved_destination_lly = tt_destination_lly,
                    saved_destination_urx = tt_destination_urx,
                    saved_destination_ury = tt_destination_ury;
         set_clip_region(selection_feedback,region);
         if (want_feedback(selection_feedback)) {
            // in case wasn't done above do it now for followers
            selection_feedback = RECURSIVE_MOVEMENT_FEEDBACK;
         };
         Sprite::display_followers(selection_feedback,region);
         tt_destination_llx = saved_destination_llx;
         tt_destination_lly = saved_destination_lly;
         tt_destination_urx = saved_destination_urx;
         tt_destination_ury = saved_destination_ury;
         tt_main_window->set_clip_region();
      };
	};
};

void Remote_Picture::now_on_top_of_nest(boolean ) {
	// new on 190499 so that things like address are updated when birds deliver them to nests
	update_picture();
};

void Remote_Picture::set_cause_of_match_failure(boolean new_flag, boolean recur) {
	// new on 291002 so glow red appropriately if sensor with an appearance
	Sprite::set_cause_of_match_failure(new_flag,recur);
	if (appearance != NULL) {
		appearance->set_cause_of_match_failure(new_flag,recur);
	};
};

boolean Remote_Picture::size_is_true() {
	if (appearance != NULL) {
		return(appearance->size_is_true());
	} else {
		return(Remote_Integer::size_is_true());
	};
};

void Remote_Picture::set_true_size_and_location(city_coordinate new_width, city_coordinate new_height,
																city_coordinate new_llx, city_coordinate new_lly,
																ChangeReason reason) { // new on 131202
	if (appearance != NULL && appearance->kind_of() == PICTURE && !decoration_identifier(identifier)) {
		// added !decoration_identifier(identifier) on 280204 otherwise the sensors don't always fit in the box right (too small)
		appearance->set_true_size_and_location(new_width,new_height,new_llx,new_lly,reason);
		set_size_and_location_to_those_of(appearance);
	} else {
		Remote_Integer::set_true_size_and_location(new_width,new_height,new_llx,new_lly,reason);
	};
};

//flag uudecoding = FALSE; -- noticed was obsolete on 291002

void Remote_Picture::update_value() { // new on 210803
	if (controlee == NULL) return;
	if (tt_log_version_number >= 69) { // condition new on 300105
		Remote_Integer::update_value(); // new on 270105 to initialize options properly (e.g. head)
	};
	update_picture(appearance == NULL); 
	// new on 210803 -- arg was TRUE prior to 310803 but then saved appearances are throw away (and breaks clipboard sensor)
};

Sprite *updating_remote_control_for_controlee = NULL; // to prevent looping

void Remote_Picture::update_picture(boolean regardless) {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log("update_picture of debug target remote picture.");
	};
#endif
	if ((!regardless && !ok_to_activate() && appearance != NULL) || tt_recording_notebook_for_log)  { 
		//  was active prior to 200802 but want to stop this if in notebook or vacuum
		// tt_recording_notebook_for_log added on 250503 to deal with loading old notebooks while making a dmo log
//		if (text_identifier(identifier)) {
			// new on 130300 
//			appearance->destroy();
//			appearance = NULL;
//			update_text_and_widths(FALSE); 
//		};
		return;
	};
   // can controlee be NULL - worth checking??
	long long_value;
	current_long_value(long_value);
	if (long_value < 0 || regardless) { // added || regardless on 300699 so remotes like REMOTE_ANCESTORS load properly
		if (appearance != NULL) {
//			remove_follower(picture);
#if TT_DEBUG_ON
			if (debug_counter == tt_debug_target) {
				log("About to set appearance of debug target remote picture to NULL.");
			};
#endif
			appearance->destroy();
			appearance = NULL;
		};
		if (long_value < 0) {
			return;
		};
	};
	switch (identifier) {
		case COLLIDEE_REMOTE:
			if (long_value == 0) {
//				if (appearance != NULL) {
//					appearance->destroy();
//					appearance = NULL;
//				};
				return;
			} else if (blank) {
				become_non_blank();
			} else return;
		case HIT_OR_MISS_REMOTE:
		case X_HIT_OR_MISS_REMOTE:
		case Y_HIT_OR_MISS_REMOTE:	{
			if (long_value == 0) {
				//return; // new on 290802 so these remotes when loaded without controlee don't show Tooly
				long_value = compute_picture_id(HIT_OR_MISS_SPRITE,NOT_COLLIDING); // new on 091004 to initialize these
				set_long_value(long_value,TRUE); // new on 101004
			};
			SpriteCode picture_code;
			short int picture_index;
			decode_picture_id((short) long_value,picture_index,picture_code);
			if (appearance != NULL) {
				SpriteCode old_picture_code;
				short int old_picture_index;
				Picture *picture = (Picture *) appearance;
				picture->current_image_index(old_picture_index,old_picture_code);
				if (old_picture_code != picture_code) {
					// easier to get rid of it and make a new one
		//			remove_follower(picture);
//					picture->destroy();
               set_appearance(NULL); 
					// new on 210802 since the above can lead to double destroy (under circumstances where an invalid code was used)
				} else {
					picture->set_image_index(picture_index);
		//		   if (picture->pointer_to_leader() != NULL) {
						// won't get correct current_image otherwise
		//				picture->Tool::update_display();
		//			};
					return;
				};
			}; //  commented out on 120599 since just destroyed appearance above so need to create anew else {
			city_coordinate new_width,new_height;
			if (leader == NULL) { // free to have natural dimensions of picture
				new_width = 0; // added on 241099
				new_height = 0; // computed below
			} else {
				new_width = width; // added on 241099
				new_height = height;
			};
			set_appearance(new Picture(llx,lly,0,picture_index,picture_code,new_width,new_height));
											};
			break;
		case CLIPBOARD_REMOTE:
			if (!tt_uudecoding_a_sprite && has_initialization_completed()) { // added has_initialization_completed() on 021002
				if (appearance == NULL) { 
					Sprite *from_clipboard = read_from_clipboard(TRUE,FALSE); //was FALSE prior to 010503 but a box of each is fine
					// second arg added 280404
					if (from_clipboard != NULL) {
						set_appearance(from_clipboard);
					};
				};
			};
			break;
      case FILE_TO_SOUND_REMOTE:
      case FILE_TO_PICTURE_REMOTE:
      case TEXT_TO_MCI_REMOTE:
      case TEXT_TO_SPEECH_REMOTE:
         break;
      case ROOM_DECORATION_REMOTE: {
			Background *background = ultimate_container()->pointer_to_background();
			if (background == NULL) {
				// new on 180105 to check if background is OK (can be a sensor of a picture in limbo -- issue 618)
				remove_all_followers(TRUE); // destroy its followers since they should just be copies -- copied here on 260105
			} else {
				if (appearance != NULL) { 
					// new on 120804 to check if still is the right house		
					House *house = background->pointer_to_house();
					if (house != NULL) { // can it be otherwise?
						if (sensor_applied_to_house != house) {
							set_appearance(NULL); // force recomputation
							sensor_applied_to_house = NULL;
						};
					};
				};	
				if (appearance == NULL && !background->outside()) {
					set_appearance(new Picture(llx,lly,0,0,WALL_SPRITE,width,height)); // was appearance = prior to 200201
					background->pointer_to_house();
					sensor_applied_to_house = background->pointer_to_house();
					if (background != NULL) {
						Room *room = background->pointer_to_room();
						if (room != NULL) {
							Sprite *wall_decoration = room->pointer_to_wall_decoration_sprite();
							if (wall_decoration != NULL) {
								UnwindProtect<boolean> set(tt_dealing_with_follower_change,TRUE); // add_follower below shouldn't trigger any remotes
								remove_all_followers(TRUE);
								Sprite *wall_decoration_copy = wall_decoration->top_level_copy();
								wall_decoration_copy->set_size_and_location(width,height,llx,lly); // important to get the sizes and locations right
	//                     add_follower(wall_decoration_copy);
								// rewrote this on 260499 so that dumped sensors can be used to remove things
								Sprites *remaining = wall_decoration_copy->pointer_to_followers();
								while (remaining != NULL) {
									Sprite *sprite = remaining->first();
									boolean see_all = sprite->show_all(); // remove_follower might reset this
									wall_decoration_copy->remove_follower(sprite);
									sprite->set_show_all(see_all);
									add_follower(sprite);
									remaining = remaining->rest();
								};
								wall_decoration_copy->destroy();
							} else {
								remove_all_followers(TRUE); // new on 120804
							};
						};
					};
				};
			};
				};
         break;
      case HOUSE_DECORATION_REMOTE:
      case ROOF_DECORATION_REMOTE:
         if (appearance == NULL) {
            Background *background = ultimate_container()->pointer_to_background();
				if (background == NULL) { 
					remove_all_followers(TRUE); // destroy its followers since they should just be copies
					// new on 280204 since if moved or in limbo then old followers are obsolete
				} else {
               House *house = background->pointer_to_house();
               if (house != NULL) { // can it be otherwise?
						if (sensor_applied_to_house != house && sensor_applied_to_house != NULL) { // new on 280204 since now in another house
							// second condition added on 121204 since can be triggered by copying sensors and taking them out of the sensor notebook
							remove_all_followers(TRUE);
//							sensor_applied_to_house = NULL; // new on 120804 to keep things honest -- commented out on 111204 since silly since set below
						};
                  sensor_applied_to_house = house; 
						// remember it to know when to reset because of house change -- was extra_pointer prior to 201202
						boolean from_side = (identifier == HOUSE_DECORATION_REMOTE); 
						Sprite *house_appearance = house->picture_of_appearance(from_side);
                  set_appearance(house_appearance); // was appearance = prior to 200201
//                Sprites *house_decorations = house->pointer_to_decorations(identifier == HOUSE_DECORATION_REMOTE);
						// above rewrritten on 280204 since better to do house_appearance->pointer_to_followers() below
//						Sprites *house_decorations = house_appearance->pointer_to_followers();
						// rewritten on 111204 since was broken -- didn't show any decoration if you got up and sat down
						Sprites *house_decorations = house->pointer_to_decorations(from_side);
//                Sprite *house_appearance = NULL;
                  if (house_decorations != NULL) {
//                   house_appearance = house->pointer_to_appearance(identifier == HOUSE_DECORATION_REMOTE);
							house_appearance->update_display(); // added 270499 so sensors work even if you haven't been outside
							// following was outside of the conditional prior to 141003 so crashed when house_appearance was NULL
							if (held()) {  // not constrained so look nice -- why held()? - asked 280204
								set_size(house_appearance->true_width(),house_appearance->true_height());
							};
                  };
                  appearance->set_size_and_location(width,height,llx,lly); 
                  if (house_appearance != NULL && house_decorations != NULL) {
                     boolean saved_tt_dealing_with_follower_change = tt_dealing_with_follower_change;
                     tt_dealing_with_follower_change = TRUE; // add_follower below shouldn't trigger any remotes
//                     remove_all_followers(TRUE); // some may have been added by ROBOT_IN_TRAINING - added on 220299
							Sprites *remaining = house_decorations;
                     while (remaining != NULL) {
                        Sprite *sprite = remaining->first();
                        Sprite *sprite_copy = sprite->top_level_copy();
//                        set_relative_size_and_location(sprite,house_appearance,sprite_copy,this); // restored on 111204
//								set_relative_size_and_location(sprite,house_appearance,sprite_copy,appearance); // rewritten on 280204
								set_relative_size_and_location(sprite,house->pointer_to_appearance(from_side),sprite_copy,appearance); // rewritten on 
                        sprite_copy->set_subpicture(TRUE);
								// added appearance-> to the following on 280204 to stop duplicates 
								// and the followers of the sensor itself are there already
                        add_follower(sprite_copy); // removed appearance-> on 111204 since are removed when you stand up for example
                        remaining = remaining->rest();
                     };
							tt_dealing_with_follower_change = saved_tt_dealing_with_follower_change; 
                  };
               };
            };
         };
         break;
		case CHILDREN_REMOTE:
			if (appearance == NULL && active() && // added on 310303 since it was needed for ANCESTORS_REMOTE and seems a good idea
				 // on 210803 tred to remove active test and use it to generate 0-hole box but that broke Playground
				 // following condition added 081199 so parts sensor can be placed on the back
				 updating_remote_control_for_controlee != controlee && // pulled out of disjunction below on 010205
				 (regardless || controlee == NULL || !controlee->is_flipped())) { 
				 // commented out on 160101 since always FALSE || is_temporarily_flipped())) { 
				// regardless was added on 040301
				//updating_remote_control_for_controlee = controlee;
			   UnwindProtect <Sprite *> set(updating_remote_control_for_controlee,controlee); // rewrote the above on 010205
				Cubby *cubby = new Cubby();
				Sprites *controlees_followers = NULL;
				if (controlee != NULL) {
					if (viewing_front_side()) {
						controlees_followers = controlee->unflipped_followers(); // was pointer_to_followers prior to 040301
					} else {
						controlees_followers = controlee->flipped_followers();
					};
				};
				if (controlees_followers == NULL) {
					cubby->set_number_of_holes(0);
				} else {
//					unsigned short length = (unsigned short) controlees_followers->length();
					int length = controlees_followers->length(); // removed 64K limit on 240104
					cubby->set_number_of_holes(length);
					length = cubby->current_number_of_holes(); // added on 160499 in case truncated due to tt_max_number_of_holes
					for (unsigned short i = 0; i < length; i++) {
						Sprite *sprite = controlees_followers->first();
						if (sprite->kind_of() == PICTURE) {
							Picture *picture_remote_looks = new Picture();
							picture_remote_looks->set_controlee((Picture *) sprite);
							cubby->initialize_hole(i,picture_remote_looks);
						} else {
							cubby->initialize_hole(i,sprite->copy());
						};
						controlees_followers = controlees_followers->rest();
					};
				};
				cubby->update_display();
				cubby->set_background(floor,FALSE); // was TRUE but that caused infinite recursion when testing user sensors in ToonTalk on 130504
				set_appearance(cubby);
			};
			break;
		case ANCESTORS_REMOTE: // update_picture
			if (appearance == NULL && active()) {
				// on 210803 tred to remove active test and use it to generate 0-hole box but that broke Playground
				// added active() condition on 310303 - this fixes a bug where wrong ancestor was used
				Cubby *cubby = new Cubby();
				Sprite *next_level = NULL;
				if (controlee != NULL) {
					if (version == 1) {
						next_level = controlee->pointer_to_leader();
					} else {
						next_level = controlee->immediate_container(); // new on 120504
					};
#if TT_DEBUG_ON
					if (controlee->debug_counter == tt_debug_target) {
						log("Computing containers remote for debug target.");
					};
#endif
				};
				while (next_level != NULL && next_level->kind_of() != PICTURE) { 
					if (version == 1) {
						next_level = next_level->pointer_to_leader();
					} else {
						next_level = next_level->immediate_container(); // new on 120504
					};
				};
				if (next_level == NULL) { // || next_level->kind_of() != PICTURE) { // check for non-picture added 300699 - removed 120504
					cubby->set_number_of_holes(0);
				} else {
					Sprites *ancestors = NULL;
					int levels = 0; // prior to 120504 was unsigned short - not that it is likely to matter
					while (next_level != NULL) { // && next_level->kind_of() == PICTURE) { 
						// check for non-picture added 300699 and moved below on 120504 to show picture ancestors but skip over boxes, etc.
						if (next_level->kind_of() == PICTURE) {
							insert_at_end(next_level,ancestors);
							levels++;
						};
						if (version == 1) {
							next_level = next_level->pointer_to_leader();
						} else {
							next_level = next_level->immediate_container(); // new on 120504
						};
					};
					cubby->set_number_of_holes(levels);
					levels = cubby->current_number_of_holes(); // added on 160499 in case truncated due to tt_max_number_of_holes
					Sprites *remaining = ancestors;
					for (int i = 0; i < levels; i++) { // was unsigned short
						Sprite *sprite = remaining->first();
//						if (sprite->kind_of() == PICTURE) { // noticed on 120504 that this was redundant
							Picture *picture_remote_looks = new Picture();
							picture_remote_looks->set_controlee((Picture *) sprite);
							cubby->initialize_hole(i,picture_remote_looks);
						//} else {
						//	cubby->initialize_hole(i,sprite->copy());
						//};
						remaining->set_first(NULL); // new on 261000
						remaining = remaining->rest();
					};
					if (ancestors != NULL) { // new on 261000
						delete ancestors;
					};
				};
				cubby->update_display();
			   cubby->set_background(floor,TRUE);
				set_appearance(cubby);
			};
			break;
			// commented out the following on 061000 since narrow_text_value does it better and thing like an animated appearance work correctly
//		case LANGUAGE_REMOTE:
//			if (appearance == NULL) {
//				Text *text = new Text();
//				text->set_text(S(IDS_NATURAL_LANGUAGE));
//				set_appearance(text);
//			};
//			break;
		default:
//		case END_OF_ANIMATION_REMOTE:
//		case VISIBLE_REMOTE:
//		case MOUSE_LEFT_BUTTON_REMOTE:
//		case MOUSE_MIDDLE_BUTTON_REMOTE:
//		case MOUSE_RIGHT_BUTTON_REMOTE:
//		case MOUSE_LEFT_BUTTON_DOWN_REMOTE:
//		case MOUSE_MIDDLE_BUTTON_DOWN_REMOTE:
//		case MOUSE_RIGHT_BUTTON_DOWN_REMOTE:
//		case LAST_KEYSTROKE_REMOTE:
//		case KEYBOARD_BUTTON_REMOTE:
//		case SHIFT_DOWN_REMOTE:
//		case CONTROL_DOWN_REMOTE:
//		case GRASPED_REMOTE:
//		case SELECTED_REMOTE:
//		case RELEASED_REMOTE:
//		case HAND_VISIBLE_REMOTE:
//		case SOUND_ON_REMOTE:
//		case MOUSE_BUTTONS_REMOTE:
//		case PROGRAMMER_HEAD_REMOTE:
//		case SCREEN_SIZE_REMOTE:
//		case AVENUE_REMOTE:
//		case STREET_REMOTE:
//      case MARTIAN_SPEAK_REMOTE:
// and joystick buttons
// and anything that should have been caught earlier
			if (appearance == NULL) {
				if (!regardless) {
					update_value(); //  new on 180803 to initialize it properly 
				};
				wide_string initial_text = text_value();
            set_appearance_to_animated_text(initial_text);
				if (initial_text != NULL) delete [] initial_text; // new on 251000
			} else if (text_value_need_updating()) {
				// it is this guy's leader that determines size constraints
				// not the appearance
				wide_string new_state_name = text_value();
				wide_string old_state_name;
				long old_length;
				appearance->current_wide_text(old_state_name,old_length); // prior to 170102 did ((Text *) appearance)
				long new_length; // was unsigned short int prior to 090202
				if (new_state_name != NULL) {
					new_length = wide_string_length(new_state_name);
				} else {
					set_appearance(NULL); // new on 160499
					return;
				};
				if (old_length != new_length ||
					 (old_length != 0 && new_length != 0 &&
					  memcmp(old_state_name,new_state_name,old_length*sizeof(wide_character)) != 0)) {
					((Text *) appearance)->set_wide_text(copy_wide_string(new_state_name),new_length,FALSE,TRUE);
					delete [] new_state_name;
					if (!size_constrained()) { // what if is_picture() - asked 290103
						set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL);
					};
					set_clean_status(DIRTY);
					return; // don't set_height again
				};
				delete [] new_state_name;
			};
	};
   // free to change size and appearance wasn't just created
//	if (leader == NULL && appearance != NULL) set_height(appearance->current_height());
//   if (!size_constrained()) {
//      set_to_good_size();
//   };
//	if (identifier == VISIBLE_REMOTE) { // conditional added on 041201 since this breaks remote controls like keyboard sensor
		// not clear which identifiers should be here (or if this is the right place to deal with the problem)
//		Remote_Integer::update_picture(regardless); // new on 221101 so the picture is updated appropriately 
//	};
};

void Remote_Picture::add_follower(Sprite *follower, boolean completely_inside, InsertionOrdering insertion_ordering, 
											 boolean dont_record_new_offsets,
											 AddFollowerReason reason) { // new arg on 210104
	Remote_Integer::add_follower(follower,completely_inside,insertion_ordering,dont_record_new_offsets,reason);
   tt_global_picture->follower_added(this,follower);
};

boolean Remote_Picture::remove_follower(Sprite *follower, boolean warn_if_not_follower) {
	if (Remote_Integer::remove_follower(follower,warn_if_not_follower)) {
		tt_global_picture->follower_removed(this,follower);
		return(TRUE);
	} else {
		return(FALSE);
	};
};

wide_string Remote_Picture::text_value() { // new on 170102 so it behaves as it did before the introduction of wide_text to Integer
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Getting text_value of debug target.");
	};
#endif
	string narrow_result = narrow_text_value(natural_language());
	wide_string wide_result = copy_wide_string(narrow_result);
//	delete [] narrow_result;
	return(wide_result); 
};

void Remote_Picture::set_text_appearance_size(Sprite *text) {
   if (leader != NULL) {
      text->set_size(width,height);
   } else { // otherwise free to have good dimensions of the text
      text->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL);
      set_size(text->current_width(),text->current_height());
   };
};

void Remote_Picture::set_appearance_to_animated_text(wide_string text_string) {
   if (text_string == NULL || text_string[0] == NULL) return; 
   Text *text = new Text(llx,lly,text_string);
   if (floor != NULL && !blank) { // conditional new on 300702 to avoid extra work when just copying sensors on the back of pictures
      set_text_appearance_size(text);
   };
   if (active()) { // conditional added on 231099 since was animated when turned off
		text->set_plate_state(TEXT_PAD_WITH_PLATE_ANIMATED);
		// text->set_never_cache(TRUE); // new on 020302 -- remodularized on 130405
	};
   set_appearance(text); // was appearance = prior to 241199
	if (blank) { // new on 170799
		appearance->become_blank();
	};
// #if TT_WIDE_TEXT // commented out on 290699 since caller may not want this
//	delete [] text_string; // copied above
//#endif
};

void Remote_Picture::set_appearance_to_MCI_sound(wide_string text_string) {
   if (text_string == NULL || text_string[0] == NULL) return; // complain??
   set_appearance(new MCISound(llx,lly,text_string)); // was appearance = prior to 241199
   set_text_appearance_size(appearance);
//#if TT_WIDE_TEXT // commented out on 290699 since caller may not want this
//	delete [] text_string; // copied above
//#endif
};

void Remote_Picture::set_appearance_to_speech_sound(wide_string text_string) {
   if (text_string == NULL || text_string[0] == NULL) return; // complain??
   set_appearance(new SpeechSound(llx,lly,text_string)); // was appearance = prior to 241199
   set_text_appearance_size(appearance);
//#if TT_WIDE_TEXT // commented out on 290699 since caller may not want this
//	delete [] text_string; // copied above
//#endif
};

void Remote_Picture::set_appearance_to_wave_sound(wide_string text_string) {
   if (text_string == NULL || text_string[0] == NULL) return; // complain??
   set_appearance(new WaveSound(llx,lly,text_string)); // was appearance = prior to 241199
   set_text_appearance_size(appearance);
//#if TT_WIDE_TEXT
//	delete [] text_string; // copied above
//#endif
};

boolean Remote_Picture::current_wide_text(wide_string &the_text, long &the_text_length) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Running current_wide_text of debug target.");
	};
#endif
	the_text = text_value();
	if (the_text == NULL) return(FALSE);
	the_text_length = wide_string_length(the_text);
	return(TRUE);
};

boolean Remote_Picture::current_text(const_string &the_text, long &the_text_length) {
	the_text = narrow_text_value(natural_language());
	if (the_text == NULL) return(FALSE);
	the_text_length = strlen(the_text);
	return(TRUE);
};

void Remote_Picture::set_background(Background *new_floor, boolean recur, boolean warn) {
   if (new_floor != floor) {
		boolean old_floor_null = (floor == NULL); // new on 240203
	   Remote_Integer::set_background(new_floor,recur,warn);
      if (decoration_identifier(identifier) && !old_floor_null) { // removed  && new_floor != NULL on 120804
			if (tt_next_background != NULL && new_floor == NULL && tt_next_background->pointer_to_house() == sensor_applied_to_house) {
				// don't do all this if about to be added to a background within the same house -- new on 121204
				return; // don't need to do more -- already really set the background
			};
			House *old_house = sensor_applied_to_house;
			if (new_floor == NULL) {
				sensor_applied_to_house = NULL;
			} else {
				sensor_applied_to_house = new_floor->pointer_to_house();
			};
         if (appearance != NULL && (sensor_applied_to_house != old_house || (followers != NULL && sensor_applied_to_house == NULL))) { 
				// different house now force recomputation
				// updated on 120804 so that this also recomputes if background is now NULL and this has followers
            appearance->destroy();
            appearance = NULL;
            if (followers != NULL) {
               UnwindProtect<boolean> set(tt_dealing_with_follower_change,TRUE); // remove_follower below shouldn't trigger any remotes
               remove_all_followers();
            };
//          update_picture(); // will happen soon enough but this might be in transition (e.g. ultimate container's background not yet updated)
         };
      };
		if (appearance == NULL && (identifier == CHILDREN_REMOTE || identifier == ANCESTORS_REMOTE)) {
			update_picture(TRUE); // added on 060499 in case it hasn't been initialized properly
			// Should this be called for other identifiers?
			// though note comment above
		};
	   if (appearance != NULL) {
		   appearance->set_background(new_floor,recur);
	   };
   } else if (identifier == ANCESTORS_REMOTE && floor != NULL && !floor->inside_rule_body()) { 
		// second conjunct added 050400 since otherwise gets reset to 0-hole box when training a robot to work with container 
		// (third conjunct added 060103 to be sure) - thanks Gordon
		// new on 180200 since containers remote will be showing flipped picture
		// this forces an update when flipped (and maybe a bit more often so a bit wasteful)
		update_picture(TRUE);
		if (appearance != NULL) {
		   appearance->set_background(new_floor,recur,warn);
	   };
	};
};

void Remote_Picture::good_size(city_coordinate &good_width, city_coordinate &good_height, Sprite *source, boolean for_expander) {
	if (appearance != NULL || tt_log_version_number >= 79) {
		// updated on 050805 so that computes good size even if appearance == NULL but careful to not break old demos
		if (appearance != NULL && // check again since since 050805 might be NULL
			 (identifier == APPEARANCE_REMOTE ||
			  identifier == COLLIDEE_REMOTE ||
           appearance->kind_of() == TEXT ||
           identifier == FILE_TO_PICTURE_REMOTE)) {
			appearance->good_size(good_width,good_height,source,for_expander);
       } else if (identifier == ROOF_DECORATION_REMOTE) {
         good_width = return_house_width();
         good_height = return_house_height();
			if (tt_log_version_number >= 40) {
				// the following does the right thing if standing in room while robot is working - new on 040404
				adjust_good_size(good_width,good_height,source);
			} else {
				adjust_size_for_source(good_width,good_height,source);
			};
      } else if (identifier == ROOM_DECORATION_REMOTE ||
                 identifier == HOUSE_DECORATION_REMOTE) {
         Sprite *house_appearance = NULL;
         Background *background = ultimate_container()->pointer_to_background();
         if (background != NULL) {
            House *house = background->pointer_to_house();
            if (house != NULL) {
               house->ensure_appearance_from_side(); // was ensure_appearance_from_above() prior to 050805
               house_appearance = house->pointer_to_appearance(identifier == HOUSE_DECORATION_REMOTE);
            };
         };
         if (house_appearance != NULL) {
            good_width = house_appearance->current_width(); 
            good_height = house_appearance->current_height(); 
				if (tt_log_version_number >= 40) {
					// the following does the right thing if standing in room while robot is working - new on 040404
					adjust_good_size(good_width,good_height,source);
				} else {
					adjust_size_for_source(good_width,good_height,source);
				};
         } else if (appearance != NULL) { // condition needed as of 050805
            appearance->Sprite::good_size(good_width,good_height,source,for_expander); // good size of original artwork -- not picture's saved size
         } else {
				Sprite::good_size(good_width,good_height,source,for_expander);
			};
		} else { // just a random remote which has no special size
			good_width = 4*tile_width;
			good_height = 4*tile_height;
			if (tt_log_version_number >= 40) {
				// the following does the right thing if standing in room while robot is working - new on 040404
				adjust_good_size(good_width,good_height,source);
			} else {
				if (floor != NULL && floor->in_the_room_with_programmer()) { // updated 020205 
					tt_programmer->translate_floor_to_room_dimensions(good_width,good_height);
				};
				if (tt_good_sizes_adjust_to_screen_size) {
					good_width = shrink_width_from_640x480_screen_size(good_width);
					good_height = shrink_height_from_640x480_screen_size(good_height);
				};
				adjust_size_for_source(good_width,good_height,source);
			};
		};
	} else {
		Sprite::good_size(good_width,good_height,source,for_expander);
	};
};

boolean Remote_Picture::can_be_text() {
	return(text_identifier(identifier));
};

NaturalLanguage last_language = NO_LANGUAGE_SPECIFIED; 
// new on 300702 to cache this since S is costly (250ms on 733MHz) for such a common operation
string last_yes = NULL; 
string last_no = NULL;

void release_last_yes_no() { // new on 130104 to fix a tiny leak
	if (last_yes != NULL) delete [] last_yes;
   if (last_no != NULL) delete [] last_no;
};

const_string Remote_Picture::narrow_text_value(NaturalLanguage language) {
	long long_value;
	current_long_value(long_value);
	switch (identifier) {
		case VISIBLE_REMOTE:
			return(S(IDS_VISIBLE_REMOTE_STATE1+(3*(language-AMERICAN))+abs(long_value)%3));
		case LAST_KEYSTROKE_REMOTE:
		case KEYBOARD_BUTTON_REMOTE:
			return(character_name(long_value,language));
		case PROGRAMMER_HEAD_REMOTE:
			switch (long_value) {
				case 0:
					return(S(IDS_BALD));
				case 1:
					return(S(IDS_LONG_HAIR));
				default: // shouldn't be any others
					return(S(IDS_HAT));
			};
		case MOUSE_BUTTONS_REMOTE:
			switch (long_value) {
				case 1:
					 return(S(IDS_NO_MOUSE_SHORTCUTS));
				case 2:
					 return(S(IDS_RIGHT_BUTTON_SHORTCUT));
				case 3:
					 return(S(IDS_RIGHT_AND_MIDDLE_SHORTCUTS));
			};
			break;
		case SCREEN_SIZE_REMOTE:
			switch (long_value) {
				case 0:
					return(_T("1X"));
				case 1:
					return(S(IDS_FULL_SCREEN));
				case 2:
					return(_T("2X"));
				case 3:
					return(_T("3X"));
				case 4:
					return(_T("4X"));
			};
         break;
      case MARTIAN_SPEAK_REMOTE:
			long_value = tt_marty_talk; // new on 150903
			switch (long_value) {
				case MARTY_TALK_BALLOONS:
					return(S(IDS_MARTIAN_BALLOONS));
				case MARTY_SPEAKS:
					return(S(IDS_MARTIAN_SPEAK));
				case MARTY_SPEAKS_AND_TALK_BALLOONS: 
					return(S(IDS_MARTIAN_SPEAK_AND_BALLOONS));
				case MARTY_SPEAKS_WITH_SUBTITLES:
					return(S(IDS_MARTAIN_SPEAK_WITH_SUBTITLES,"speak and subtitles")); // added 101199 - arg just until all translation catch up
				case MARTY_WITH_SUBTITLES:
					return(S(IDS_MARTAIN_WITH_SUBTITLES,"subtitles"));
				default:
					return("???"); // shouldn't happen
			};
		case LANGUAGE_REMOTE: // moved here on 061000 -- computed here
			return(S(IDS_NATURAL_LANGUAGE));
		case STREET_REMOTE:
		case AVENUE_REMOTE:
		case ANCESTORS_REMOTE:
		case CHILDREN_REMOTE: // are there others?
		case COLLIDEE_REMOTE: // new on 270201
		//case HIT_OR_MISS_REMOTE: // new on 181203 - to fix Java generation bug
		//case X_HIT_OR_MISS_REMOTE: // new on 181203 - to fix Java generation bug
		//case Y_HIT_OR_MISS_REMOTE: // new on 181203 - to fix Java generation bug
			return(NULL); // if called while Remote_Picture::Remote_Picture is running
	};
//    case END_OF_ANIMATION_REMOTE:
//		case GRASPED_REMOTE:
//		case SELECTED_REMOTE:
//		case RELEASED_REMOTE:
//		case MOUSE_LEFT_BUTTON_REMOTE:
//		case MOUSE_MIDDLE_BUTTON_REMOTE:
//		case MOUSE_RIGHT_BUTTON_REMOTE:
//		case MOUSE_LEFT_BUTTON_DOWN_REMOTE:
//		case MOUSE_MIDDLE_BUTTON_DOWN_REMOTE:
//		case MOUSE_RIGHT_BUTTON_DOWN_REMOTE:
//		case SHIFT_DOWN_REMOTE:
//		case CONTROL_DOWN_REMOTE:
//		case HAND_VISIBLE_REMOTE:
//		case SOUND_ON_REMOTE:
// and the joystick buttons
	// tried Yes but matching failed and kids are more like to stay lower
   if (last_language != language) { // update the "cache" - rewritten on 010802
      last_language = language;
      if (last_yes != NULL) delete [] last_yes;
      last_yes = SC(IDS_YES+(2*(language-AMERICAN))); // copied now on 300702
      if (last_no != NULL) delete [] last_no;
      last_no = SC(IDS_NO+(2*(language-AMERICAN)));
   };
	if (long_value&1) {
      return(last_yes);
   } else {
      return(last_no);
   };
};

Sprite *Remote_Picture::now_inside_thought_bubble(boolean in_training) {
   if (in_training) return(this);
	// new policy on 171199 regarding the following remotes - they aren't touched but their appearance is blanked if need be
	if (identifier == CLIPBOARD_REMOTE || // but blank clipboard only match empty clipboards
       identifier == FILE_TO_PICTURE_REMOTE ||
       identifier == FILE_TO_SOUND_REMOTE ||
       identifier == TEXT_TO_MCI_REMOTE ||
       identifier == TEXT_TO_SPEECH_REMOTE) {
//			become_blank(); // commented out on 171199 since the remote shouldn't be blank - just anything on top
		if (appearance != NULL) {
			appearance->become_blank(FALSE,FALSE); 
			// second FALSE so that controlee doesn't become blank due to remote looks being vacuumed up
		};
		return(this);
	} else if (appearance == NULL) { // not clear what else to do
		return(Rational::now_inside_thought_bubble(in_training));
	} else { // if (tt_log_version_number > 14) { 
		// new on 220499 both here and below -- new policy is that it just becomes inactive
		// while inside a thought bubble
		set_active(FALSE); // won't be updated anymore since is part of thought bubble
		return(this);
/*
	} else if (identifier == COLLIDEE_REMOTE || 
				  identifier == APPEARANCE_REMOTE || 
				  identifier == CHILDREN_REMOTE ||
				  identifier == ANCESTORS_REMOTE) {
		// this is a bit of a problem since might have trained a robot to vacuum off the collidee from the remote
		// ideally there should still be a remote but a "dead" one that shows the last collidee
		Sprite *replacement = appearance->copy();
		leader->add_follower(replacement);
		leader->remove_follower(this);
		destroy();
		return(replacement);
	} else {   // or would it be better to become blank???
		// replace self with picture
      update_picture();
		leader->add_follower(appearance);
		leader->remove_follower(this);
		appearance->set_true_size_and_location(width,height,llx,lly);
      if (appearance->kind_of() == TEXT) {
         ((Text *) appearance)->set_plate_state(TEXT_PAD_WITH_PLATE);
      };
//		tt_deleted_sprites = new Sprites(this,tt_deleted_sprites);
		Sprite *saved_appearance = appearance;
		appearance = NULL;
		destroy();
		return(saved_appearance);
*/
	};
};

MatchStatus Remote_Picture::match(Remote_Picture *other_remote_picture, int version) {
	// new on 111099
	if (tt_match_blank_with_sensor_then_run_once_per_cycle) { // new on 030404 to do this before matching
		if (tt_touched_a_remote != ONLY_ON_NEW_CYCLE) {
			set_touched_a_remote(other_remote_picture->change_frequency());
		};
	};
	if (blank) {
		return(MATCH_GOOD);
	};
	if (controlee != NULL && !controlee->global() && controlee->on_back_of_picture_or_limbo()) { // new on 091100
		return(MATCH_BAD);
	};
	if (!tt_match_blank_with_sensor_then_run_once_per_cycle) { // new on 030404 to do this before matching
		if (tt_touched_a_remote != ONLY_ON_NEW_CYCLE) {
			set_touched_a_remote(other_remote_picture->change_frequency());
		};
	};
	Sprite *others_appearance = other_remote_picture->pointer_to_appearance();
	if (appearance == NULL && others_appearance == NULL) return(MATCH_GOOD);
	if (appearance == NULL || others_appearance == NULL) {
		return(MATCH_BAD); // only one of them has a NULL appearance
	};
	return(::match(appearance,others_appearance,version));
};

MatchStatus Remote_Picture::match(Number *other, int version) {
	if (identifier == CLIPBOARD_REMOTE ||
       identifier == FILE_TO_PICTURE_REMOTE ||
       identifier == FILE_TO_SOUND_REMOTE ||
       identifier == TEXT_TO_MCI_REMOTE ||
       identifier == TEXT_TO_SPEECH_REMOTE) {
		// matches only if other is too -- clipboard is empty since otherwise dereferenced
      set_touched_a_remote(change_frequency());
		if (other->kind_of() == REMOTE_PICTURE) {
			if (((Remote_Picture *) other)->return_identifier() == identifier) {
            // what about which appearance???
				return(MATCH_GOOD);
			};
		};
		return(MATCH_BAD);
	};
	return(Remote_Integer::match(other,version));
};

Sprite *Remote_Picture::dereference() {
	if (active()) { // conditional added 301000 !blank condition added 170101
		tt_touched_a_remote = ONLY_ON_NEW_CYCLE; // will always be true? -- should this be set_touched_a_remote?
		if (appearance == NULL) { 
			// new on 020405 since if active but has no appearance maybe can compute one (e.g. decoration sensors in fresh house)
			update_picture(TRUE);
		};
	// following didn't help -- blank conditional below was moved (incorrectly) as well on 210803
	//} else if (appearance == NULL) { 
	//	// is inactive and has no appearance and is involved in a match or the like
	//	// new on 210803 that it gets its current value and becomes inactive again
	//	set_active(TRUE);
	//	Sprite *result = dereference();
	//	set_active(FALSE);
	//	return(result);
	};
	if (appearance != NULL) return(appearance->dereference()); // added dereference on 230499
	if (blank) { 
		return(tt_global_picture); 
		// why not use it as a sterotypical blank picture as well? (moved before the following on 160101 and restored it on 170101)
	};
	return(this);
};

boolean Remote_Picture::copy_set_appearance(Sprite *new_appearance) {
	if (new_appearance != original_of_appearance || appearance == NULL) {
		original_of_appearance = new_appearance;
		if (identifier == COLLIDEE_REMOTE) { // && tt_log_version_number > 14) { // new on 220499
			Picture *collidee_remote_looks = new Picture();
			collidee_remote_looks->set_controlee((Picture *) new_appearance);
//			collidee_remote_looks->set_ignore_controlee_when_size_changes(TRUE);
//			collidee_remote_looks->set_size(new_appearance->current_width(),new_appearance->current_height());
			collidee_remote_looks->set_size_and_location_to_those_of(this);
			// args added below on 061102 since no need to do all that work (I hope)
			set_long_value(0,TRUE,FALSE,FALSE); 
			// added on 080699 so that this info isn't lost if remote was dumped with a negative value -- prior to 030302 this was just value = 0
         set_appearance(collidee_remote_looks);
		} else {
			set_appearance(new_appearance->copy());
		};
		return(TRUE); // made a copy
	}; // same so leave it alone
	return(FALSE);
};

void Remote_Picture::set_appearance(Sprite *new_appearance) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 230499 || debug_counter == tt_debug_target) {
		tt_error_file() << "Remote_Picture#" << debug_counter << " new appearance is ";
		if (new_appearance != NULL) {
			new_appearance->print(tt_error_file());
			tt_error_file() << endl;
		} else {
			tt_error_file() << "NULL." << endl;
		};
	};
	if (new_appearance != NULL && new_appearance->kind_of() == CUBBY) {
		Cubby *cubby = (Cubby *) new_appearance;
		if (cubby->current_number_of_holes() > 0) {
			Sprite *first_hole = cubby->component(0);
			if (first_hole != NULL) {
				Picture *picture = (Picture *) (first_hole->pointer_to_controlee());
				if (picture != NULL) {
					string file_name = picture->return_file_name();
					if (file_name != NULL && strncmp(file_name,"touch",5) == 0) { 
						debug_this();
					};
				};
			};
		};
	};
#endif
	if (blank &&
       (identifier == CLIPBOARD_REMOTE ||
        identifier == FILE_TO_PICTURE_REMOTE ||
        identifier == FILE_TO_SOUND_REMOTE ||
        identifier == TEXT_TO_MCI_REMOTE ||
        identifier == TEXT_TO_SPEECH_REMOTE)) { // ignore it
      return;
	};
	if (appearance != NULL) { // moved after above on 280300 so isn't destroyed if ignored
		appearance->destroy();
	};
	appearance = new_appearance;
   if (appearance != NULL) {
		if (blank) { 
			// new on 170799 and moved into conditional on 310799 since appearance may be NULL - 
			// e.g. this is COLLIDEE_REMOTE and no longer colliding 
			appearance->become_blank(FALSE,FALSE); // args new on 030899
		};
//		if (tt_log_version_number > 14) {
			appearance->set_is_picture(TRUE); // added on 240499 to be more "honest"
			appearance->set_background(floor); // makes it recompute size if is a number, for example -- 
			// but who keeps this correct as sensor is moved around?
//      };
		appearance->set_true_size(width,height);
		appearance->move_to(llx,lly); // new on 221199 since often not initialized properly -- e.g. remote looks when copy is flipped
   };
	set_clean_status(DIRTY);
};

void Remote_Picture::java_before(java_stream &out) { // new on 171100
	if (appearance != NULL && (contained_in(THOUGHT_BUBBLE) || controlee == NULL)) { 
		appearance->java_before(out);
	} else {
		Remote_Integer::java_before(out);
	};
};

void Remote_Picture::java(java_stream &out) { // new on 210700 
   if (contained_in(THOUGHT_BUBBLE) || (controlee == NULL && appearance != NULL)) {
		if (blank) { // new on 220700 act like a blank picture or text pad
			if (identifier == ANCESTORS_REMOTE || identifier == CHILDREN_REMOTE) { // new on 061200
				out << S(IDS_JAVA_TT_BLANK_BOX);
			} else if ((narrow_text_value(natural_language()) != NULL || can_be_a_number()) && 
				        (appearance == NULL || appearance->looks_like_is_of_type(TEXT))) { // was just can_be_a_number() prior to 270201
				// so displays as text - // new on 301100 - condition updated on 011200 added narrow_text_value test on 270201
				// added appearance == NULL etc so that generates the right code for hit_or_miss_remote - 181203
				out << S(IDS_JAVA_TT_BLANK_TEXT);
			} else { 
				out << S(IDS_JAVA_TT_BLANK_PICTURE);
			};
		} else if (appearance == NULL) {
			// commented out on 011200 since it doesn't match as if it were text
//			if (narrow_text_value() == NULL) {
//				out << S(IDS_JAVA_TT_REMOTE_PICTURE_NO_APPEARANCE); // new on 230700
//			} else { // new on 301100
				out << "TT.REMOTE_PICTURE_NO_TEXTUAL_APPEARANCE"; // could add to javacom.rc -- would IDS_JAVA_TT_BLANK_TEXT do the right thing??
//			};
		} else {
         if (natural_language() == JAPANESE && appearance->kind_of() == TEXT) { 
				// new on 130702 to program around a problem with Japanese string literals in some versions of Java (not J++ or 2.0)
            string english_text = narrow_text_value(AMERICAN);
            Text *text = new Text();
            text->set_text(english_text);
            text->java(out);
            text->destroy();
         } else {
			   appearance->java(out);
         };
		};
#if TT_POST322
	} else if (blank) {
		out << S(IDS_JAVA_TT_BLANK_PICTURE); // new on 041103
#endif
	} else {
		Remote_Integer::java(out);
	};
};

flag told_need_32bit_version = FALSE;

void Remote_Picture::generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection) {
	History history = tt_histories[kind_of()];
	if ((!((history>>identifier)&1) && tt_help_counter == 0) ||
		 tt_help_counter == 1) {
		// haven't explained this kind of remote yet
		no_help_interruptions();
		user_did(kind_of(),1L<<identifier);
		switch (identifier) {
			case HIT_OR_MISS_REMOTE:
				message << S(IDS_REMOTE_PICTURE_DESCRIPTION);
            if (controlee != NULL) {
				   controlee->describe_briefly(message,DEFINITE_ARTICLE);
            } else {
               print_type(PICTURE,message,INDEFINITE_ARTICLE);
            };
				message << S(IDS_REMOTE_PICTURE_DESCRIPTION2);
				break;
			case COLLIDEE_REMOTE:
				message << S(IDS_REMOTE_PICTURE_DESCRIPTION3);
            if (controlee != NULL) {
				   controlee->describe_briefly(message,DEFINITE_ARTICLE);
            } else {
               print_type(PICTURE,message,INDEFINITE_ARTICLE);
            };
				message << S(IDS_REMOTE_PICTURE_DESCRIPTION4);
				break;
			case X_HIT_OR_MISS_REMOTE: 
				message << S(IDS_REMOTE_PICTURE_DESCRIPTION);
            if (controlee != NULL) {
				   controlee->describe_briefly(message,DEFINITE_ARTICLE);
            } else {
               print_type(PICTURE,message,INDEFINITE_ARTICLE);
            };
				message << space() << S(IDS_COLLIDING_SIDE_OR_NOT);
				break;
			case Y_HIT_OR_MISS_REMOTE:
				message << S(IDS_REMOTE_PICTURE_DESCRIPTION);
            if (controlee != NULL) {
				   controlee->describe_briefly(message,DEFINITE_ARTICLE);
            } else {
               print_type(PICTURE,message,INDEFINITE_ARTICLE);
            };
				message << space() << S(IDS_COLLIDING_TOP_OR_NOT);
				break;
			case APPEARANCE_REMOTE:
				message << S(IDS_REMOTE_PICTURE_DESCRIPTION);
				message << S(IDS_IF_YOU_CHANGE_APPEARANCE);
            if (controlee != NULL) {
				   controlee->describe_briefly(message,DEFINITE_ARTICLE);
            } else {
               print_type(PICTURE,message,INDEFINITE_ARTICLE);
            };
				message << space() << S(IDS_CHANGE_SAME_WAY);
				break;
			case VISIBLE_REMOTE:
				message << S(IDS_CONTROLLING_WHETHER) << space();
            if (controlee != NULL) {
				   controlee->describe_briefly(message,DEFINITE_ARTICLE);
            } else {
               print_type(PICTURE,message,INDEFINITE_ARTICLE);
            };
				message << space() << S(IDS_VISIBLE_PART_OR_NOT);
				message << S(IDS_SPACE_WILL_CHANGE_THEM);
				break;
			case END_OF_ANIMATION_REMOTE:
				message << S(IDS_ANSWER_QUESTION_ANIMATION) << space();
            if (controlee != NULL) {
				   controlee->describe_briefly(message,DEFINITE_ARTICLE);
            } else {
               print_type(PICTURE,message,INDEFINITE_ARTICLE);
            };
				message << space() << S(IDS_IS_FINISHED);
				break;
			case CHILDREN_REMOTE:
				message << S(IDS_ANSWER_CHILDREN_QUESTION) << space();
            if (controlee != NULL) {
				   controlee->describe_briefly(message,DEFINITE_ARTICLE);
            } else {
               print_type(PICTURE,message,INDEFINITE_ARTICLE);
            };
				message << end_sentence();
				break;
		   case ANCESTORS_REMOTE:
				message << S(IDS_ANSWER_ANCESTORS_QUESTION) << space();
            if (controlee != NULL) {
				   controlee->describe_briefly(message,DEFINITE_ARTICLE);
            } else {
               print_type(PICTURE,message,INDEFINITE_ARTICLE);
            };
				message << space() << S(IDS_ANSWER_ANCESTORS_QUESTION2) << end_sentence();
				break;
			case HAND_VISIBLE_REMOTE:
				message << S(IDS_WHETHER_HAND_VISIBLE)
						  << S(IDS_SPACE_WILL_SWITCH_IT);
				break;
			case SOUND_ON_REMOTE:
				message << S(IDS_WHETHER_SOUND_ON)
						  << S(IDS_WHETHER_SOUND_ON);
				break;
			case MOUSE_BUTTONS_REMOTE:
				message << S(IDS_MIDDLE_AND_RIGHT_SHORTCUTS_ON)
						  << S(IDS_MAKE_RIGHT_BUTTON_SHORTCUT)
						  << S(IDS_AND_RIGHT_BUTTON_SHORTCUT);
				break;
			case AVENUE_REMOTE:
				message << S(IDS_AVENUE_HOUSE_IS_ON)
						  << S(IDS_YOU_CHANGE_IT_BY)
						  << S(IDS_GOOD_FOR_TELLING_TRUCK_WHICH_AVENUE_TO_BUILD);
				break;
			case STREET_REMOTE:
				message << S(IDS_STREET_HOUSE_IS_ON)
						  << S(IDS_YOU_CHANGE_IT_BY)
						  << S(IDS_GOOD_FOR_TELLING_TRUCK_WHICH_STREET_TO_BUILD);
				break;
         case FILE_TO_SOUND_REMOTE:
            message << S(IDS_FILE_TO_SOUND_FULL_HELP);
            if (appearance == NULL) {
               message << S(IDS_FILE_TO_SOUND_FULL_HELP2);
            } else {
               message << S(IDS_ON_TOP_OF_IT_IS) << space();
               appearance->describe(message);
            };
            return;
         case FILE_TO_PICTURE_REMOTE:
            message << S(IDS_FILE_TO_PICTURE_FULL_HELP);
            if (appearance == NULL) {
               message << S(IDS_FILE_TO_PICTURE_FULL_HELP2);
            } else {
               message << S(IDS_ON_TOP_OF_IT_IS) << space();
               appearance->describe(message);
            };
            return;
         case CLIPBOARD_REMOTE:
            message << S(IDS_THIS_IS) << space() << S(IDS_A_REMOTE_CONTROL)
                    << space() << S(IDS_FOR) << space() << S(IDS_X_HELP+identifier) << end_sentence();
            message << S(IDS_CLIPBOARD_REMOTE_FULL_HELP1) << S(IDS_CLIPBOARD_REMOTE_FULL_HELP2);
            if (appearance != NULL) {
               message << S(IDS_ON_TOP_OF_IT_IS) << space();
               appearance->describe(message);
            };
            return;
         case TEXT_TO_MCI_REMOTE:
            message << S(IDS_TEXT_TO_MCI_FULL_HELP);
            if (appearance == NULL) {
               message << S(IDS_TEXT_TO_MCI_FULL_HELP2);
            } else {
               message << S(IDS_ON_TOP_OF_IT_IS) << space();
               appearance->describe(message);
            };
            return;
         case TEXT_TO_SPEECH_REMOTE:
            message << S(IDS_TEXT_TO_SPEECH_FULL_HELP);
            if (appearance == NULL) {
#if TT_TEXT_TO_SPEECH
               message << S(IDS_TEXT_TO_SPEECH_FULL_HELP3);
#else
               if (!told_need_32bit_version) {
                  message << S(IDS_TEXT_TO_SPEECH_FULL_HELP2);
                  told_need_32bit_version = TRUE;
               };
#endif
            } else {
               message << S(IDS_ON_TOP_OF_IT_IS) << space();
               appearance->describe(message);
            };
            return;
			default:
            if (describe_joystick_button_remote(return_joystick_number(),identifier,message,TRUE)) {
               // already taken care of
				} else if (appearance == NULL || appearance->kind_of() == INTEGER) {
					Remote_Integer::generate_help(message,tool_in_hand,selection);
				} else {
					long next_id;
					if (appearance->kind_of() == TEXT) {
						message << S(IDS_THIS_TEXT_PAD) << space();
						next_id = IDS_THIS_TEXT_PAD2;
					} else {
						message << S(IDS_THIS_IS) << space();
						next_id = IDS_THIS_IS2;
					};
					if (read_only()) {
						message << S(IDS_A_SENSOR);
					} else {
						message << S(IDS_A_REMOTE_CONTROL);
					};
					message << space() << S(IDS_FOR) << space() << S(IDS_X_HELP+identifier) 
						     << S(next_id,"") << end_sentence();
				};
		};
		switch (identifier) { // and say...
			case HIT_OR_MISS_REMOTE:
			case X_HIT_OR_MISS_REMOTE:
			case Y_HIT_OR_MISS_REMOTE:
				message << S(IDS_TURN_OFF_COLLISION_WITH_SPACE);
				break;
		};
	};
   if (tt_help_counter == 2 && // new on 091004 to prevent the following from being repeated in response to F1
		 !read_only() &&
       identifier != FILE_TO_SOUND_REMOTE &&
       identifier != FILE_TO_PICTURE_REMOTE &&
       identifier != CLIPBOARD_REMOTE &&
       identifier != TEXT_TO_MCI_REMOTE &&
       identifier != TEXT_TO_SPEECH_REMOTE &&
		 identifier != COLLIDEE_REMOTE &&
		 identifier != CHILDREN_REMOTE &&
		 identifier != ANCESTORS_REMOTE) {
		message << space(); // new on 250204 since otherwise can get ".. last key pressedIf you pick ..."
      if (held()) {
         message << S(IDS_IF_YOU_PRESS) << space();
      } else {
         message << S(IDS_IF_YOU_PICK_UP_AND_PRESS) << space();
      };
      describe_right_button(message,0);
      if (enumeration_type()) {
         message << space() << S(IDS_JUST_THE_SPACE_BUTTON);
      };
      message << space() << S(IDS_ITLL_SWITCH_TO_THE_NEXT_VALUE);
      message << space() << S(IDS_IF_YOU_PRESS) << space();
//      describe_middle_button(message);
//      if (enumeration_type()) {
//          message << " " << S(IDS_OR_JUST) << " " << "'-'";
//      };
      message << "'-'"; // replaces the above since 3rd button is now under user control
		if (text_to_speech()) {
			message << space() << "(" << S(IDS_MINUS,"minus") << ")"; // new on 091004 for speaking about the - key
		};
      message << space() << S(IDS_ITLL_SWITCH_TO_THE_PREVIOUS_VALUE);
   };
};

Path *Remote_Picture::path_to(Sprite *target, boolean &ok) {
//	if (tt_just_vacuumed == target) { // just vacuumed it off a remote -- was tt_just_vacuumed == this prior to 290704
	// rewritten on 171204 since the fix of 290704 was OK if not in a box but otherwise the old code was correct
	if (leader==NULL?(tt_just_vacuumed == target):(tt_just_vacuumed == this)) {
//		 target->reset_appearance_before_vacuuming();
		ok = TRUE; // new on 290704
		// added kind_of information on 290704
		return(new Path(SPECIAL_PATH_MARKER,new Path(target->kind_of()))); // ,new Path(SPECIAL_PATH_MARKER)
	};
	if (appearance != NULL && appearance->kind_of() == CUBBY) { // still possible? asked 290704
		return(appearance->path_to(target,ok));
	};
	ok = FALSE;
	return(NULL);
};

boolean has_yes_or_no_values(RemoteIdentifier identifier) {
	// new on 210301 by subsetting can_be_a_number()
	switch (identifier) { 
		case END_OF_ANIMATION_REMOTE:
		case GRASPED_REMOTE:
		case SELECTED_REMOTE:
		case RELEASED_REMOTE:
		case MOUSE_LEFT_BUTTON_REMOTE:
		case MOUSE_MIDDLE_BUTTON_REMOTE:
		case MOUSE_RIGHT_BUTTON_REMOTE:
		case MOUSE_LEFT_BUTTON_DOWN_REMOTE:
		case MOUSE_MIDDLE_BUTTON_DOWN_REMOTE:
		case MOUSE_RIGHT_BUTTON_DOWN_REMOTE:
		case SHIFT_DOWN_REMOTE:
		case CONTROL_DOWN_REMOTE:
		case HAND_VISIBLE_REMOTE:
			return(TRUE);
		default:
			return(FALSE);
	};
};

boolean Remote_Picture::can_be_a_number() {
	// new on 310399
	// is true if it makes sense to coerce this to a number
	if (has_yes_or_no_values(identifier)) {
		return(TRUE);
	};
	switch (identifier) { 
		case HIT_OR_MISS_REMOTE:
		case X_HIT_OR_MISS_REMOTE:
		case Y_HIT_OR_MISS_REMOTE:
		case VISIBLE_REMOTE:	
		case AVENUE_REMOTE:
		case STREET_REMOTE:
		case MARTIAN_ADVICE_LEVEL_REMOTE:
			return(TRUE);
		default:
			if (appearance != NULL) { // new on 221299
				if (appearance->can_be_a_number()) {
					return(TRUE);
				};
			};
			return(identifier >= JOYSTICK_BUTTON1_REMOTE && identifier <= JOYSTICK_BUTTON32_DOWN_REMOTE);
	};
};

void Remote_Picture::set_active(boolean new_flag, boolean recur, boolean initialization) {
	//if (tt_recording_notebook_for_log) {
	//	return; // new on 250503
	//};
	boolean change = (new_flag != active());
	Remote_Integer::set_active(new_flag,recur,initialization);
	//if (change) { // moved to Remote_Integer::set_active on 170105
	//	if (controlee != NULL) { 
	//		if (active()) {
	//			controlee->add_remote(identifier,this);
	//		} else {
	//	      // no need to be notified if it is inactive but if controlee is in a vacuum then it may be spit out again later
	//			controlee->remove_remote(identifier,this);
	//		};
	//	};
	//};
	if (appearance != NULL) {
		if (textual()) {
			if (active()) { // updated from active_flag on 160702
				((Text *) appearance)->set_plate_state(TEXT_PAD_WITH_PLATE_ANIMATED);
				// appearance->set_never_cache(TRUE); // new on 020302 -- remodularized on 130405
				if (controlee != NULL) { // new on 241099 since when remote control was activated it didn't recompute new value
					Remote *remote = controlee->pointer_to_remote();
					if (remote != NULL) {
						switch (identifier) {
							case MOUSE_RIGHT_BUTTON_REMOTE:
							case MOUSE_MIDDLE_BUTTON_REMOTE:
							case MOUSE_LEFT_BUTTON_REMOTE:
								remote->changed(identifier,0); // not just down if activated manually
								break;
							case MOUSE_LEFT_BUTTON_DOWN_REMOTE:
								// shows current state
								// went "virtual" on 051199
								remote->changed(identifier,virtual_left_button(tt_current_mouse_button)); // tt_current_mouse_button&LEFT_BUTTON);
								break;
							case MOUSE_MIDDLE_BUTTON_DOWN_REMOTE:
								// shows current state
								remote->changed(identifier,virtual_middle_button(tt_current_mouse_button)); // tt_current_mouse_button&MIDDLE_BUTTON);
								break;
							case MOUSE_RIGHT_BUTTON_DOWN_REMOTE:
								// shows current state
								remote->changed(identifier,virtual_right_button(tt_current_mouse_button)); //tt_current_mouse_button&RIGHT_BUTTON);
								break;
							case SHIFT_DOWN_REMOTE:
								remote->changed(identifier,tt_shift_down);
								break;
							case CONTROL_DOWN_REMOTE:
								remote->changed(identifier,tt_control_down);
								break;
							case LANGUAGE_REMOTE: // new on 121000 so when activated if saved inactive works fine
//								set_appearance(NULL);
								update_picture(TRUE);
								break;
						};
					};
				};
			} else {
				((Text *) appearance)->set_plate_state(TEXT_PAD_WITH_PLATE);
				// appearance->set_never_cache(FALSE); // new on 020302 remodularized on 130405
			};
		} else if (active() && tt_log_version_number >= 67) {
			// new on 110105 -- not clear why this wasn't done before
			// and not clear if other sensors should respond here too
			switch (identifier) {
				case ANCESTORS_REMOTE:
				case CHILDREN_REMOTE:
					update_picture(TRUE); // why regardless (TRUE)? 
					// -- is this due to assignment rather than UnwindProtect of updating_remote_control_for_controlee
					break;
					// others ??
			};
		};
	};
};

void Remote_Picture::become_blank(boolean by_user, boolean controlee_too) { // new on 170799
	if (identifier == CHILDREN_REMOTE
		&& tt_top_copy == NULL) { // new on 221003 since otherwise this can break things if copying a picture or robot with an erased parts sensor
		// what about loading XML of erased blank parts sensor -- OK?
		// new on 160101
		Sprites *controlees_followers;
		if (controlee == NULL) { // new on 180101
			controlees_followers = NULL;
		} else {
#if TT_POST318
			if (!controlee->slated_for_deletion())
#endif
			{
				if (viewing_front_side()) {
					controlees_followers = controlee->pointer_to_followers();
				} else {
					controlees_followers = controlee->flipped_followers();
				};
				if (controlees_followers != NULL) {
					saved_controlees_followers = controlees_followers->just_copy_list();
					if (viewing_front_side()) { // moved into conditional on 170101 (so only happens if there are followers)
						controlee->remove_all_followers(FALSE);
					} else {
						controlee->remove_all_flipped_followers(FALSE);
					};
				};
			};
		};
	};
	Remote_Integer::become_blank(by_user,controlee_too);
	if (appearance != NULL) {
		appearance->become_blank(by_user,FALSE); 
		// FALSE added on 030899 so that controlee doesn't become blank due to remote looks being vacuumed up
	};
};

void Remote_Picture::become_non_blank(boolean controlee_too) { // new on 170799
	Remote_Integer::become_non_blank(controlee_too);
	if (appearance != NULL) {
		appearance->become_non_blank(FALSE); 
		// FALSE added on 030899 so that controlee doesn't become blank due to remote looks being vacuumed up
	};
	if (identifier == CHILDREN_REMOTE && saved_controlees_followers != NULL && controlee != NULL) { 
		// new on 160101 - added controlee != NULL on 180101 for robustness
		Sprites *remaining = saved_controlees_followers;
		if (viewing_front_side()) {
			while (remaining != NULL) {
				Sprite *follower = remaining->first();
				controlee->add_follower(follower);
				remaining = remaining->rest();
			};
		} else {
			Sprite *previous_follower = NULL;
			while (remaining != NULL) {
				Sprite *follower = remaining->first();
				add_flipped_follower(follower,previous_follower,controlee);
				previous_follower = follower;
				remaining = remaining->rest();
			};
			// commented out on 060201 since now use placed_new_item()
//			if (controlee->showing_on_a_screen() && controlee->is_flipped()) {
//				controlee->place_new_item(); // to tile the flipped followers nicely
//			};
		};
//		Sprites *controlees_followers = (Sprites *) extra_pointer;
		just_destroy_list(saved_controlees_followers);
		saved_controlees_followers = NULL;
//		update_picture();
	};
};

boolean Remote_Picture::enumeration_type() {
	// was of Remote_Integer prior to 030302 but that makes no sense given the list below
	switch (identifier) {
		// PARAMETER_REMOTE is enumerated but 256 values...
		case VISIBLE_REMOTE:
		case HAND_VISIBLE_REMOTE:
		case SOUND_ON_REMOTE:
		case PROGRAMMER_HEAD_REMOTE:
		case SCREEN_SIZE_REMOTE:
		case MOUSE_BUTTONS_REMOTE:
//		case LEGO_SERIAL_PORT_REMOTE:
      case MARTIAN_SPEAK_REMOTE:
			return(TRUE);
		default:
			return(FALSE);
	};
};

boolean Remote_Picture::decoration_remote() { // new on 170106
	return(decoration_identifier(identifier));
};

void Remote_Picture::follower_used_in_decoration_changed(Sprite *follower) { // new on 170106
	if (floor == NULL) return;
	tt_global_picture->follower_removed(this,follower); // this should force the recomputation of the decoration
};

/* commented out since one remote might not have run yet but they mean the same if they have same controlee and identifier -- 
// what if both are inactive??
boolean Remote_Picture::equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, 
// SpritesPointer &expected_corresponding_nests, EqualReason reason) { // new on 050601
	if (!Remote_Integer::equal_to(other,pending,corresponding_nests,expected_corresponding_nests,reason)) return(FALSE);
	Remote_Picture *other_remote = (Remote_Picture *) other;
	if (pointer_to_appearance() != NULL) {
		if (other_remote->pointer_to_appearance() == NULL) return(FALSE); // does this happen?
		return(pointer_to_appearance()->equal_to(other_remote->pointer_to_appearance(),pending,corresponding_nests,
		// expected_corresponding_nests,reason));
	} else {
		return(other_remote->pointer_to_appearance() == NULL);
	};
	return(TRUE);
};
*/

boolean Force_Remote_Integer::dump(output_stream &stream, boolean just_prepare, boolean is_controlee, boolean on_nest) {
	if (!Remote_Integer::dump(stream,just_prepare,is_controlee,on_nest)) return(FALSE);
   if (controlee == NULL) {
		stream.put(NOTHING_MARKER); // added on 190999 for robustness
	} else {
		controlee->dump(stream,just_prepare,TRUE); // TRUE added on 140699
	};
	return(TRUE);
};

Remote_Address::Remote_Address(Picture *controlee, // ignored?
										 RemoteIdentifier identifier, // either AVENUE_REMOTE or STREET_REMOTE
										 city_coordinate center_x, city_coordinate center_y,
										 long priority,
										 city_coordinate width, city_coordinate height,
										 long initial_value,
										 NaturalLanguage natural_language,
										 boolean active) :
	Remote_Picture(controlee,identifier,
						center_x,center_y,
						priority,
						width,height,
						initial_value,
						natural_language,
						active),
	current_block(NULL),
	text_value_up_to_date(FALSE) {
		// isn't the following obsolete now that active flag is saved??
		if (initial_value < 0) set_active(TRUE); // notice that this overrides the active_flag
		update_picture(); // initialize properly
  // could change SpriteType from REMOTE_PICTURE to REMOTE_ADDRESS or something
};

Remote_Address::Remote_Address(RemoteIdentifier identifier) : // either AVENUE_REMOTE or STREET_REMOTE
	Remote_Picture(NULL,identifier,0,0,0,0,0,0,tt_language,FALSE),
	current_block(NULL),
	text_value_up_to_date(FALSE) {
	// used when loading XML - does much less work and doesn't prematurely run update_picture() - new on 111202
};

Remote_Picture *Remote_Address::just_copy(Picture *controlee) {
	long long_value;
	current_long_value(long_value);
	return(new Remote_Address(controlee,identifier,
									  llx,lly,
									  current_priority-1,
									  width,height,
									  long_value,
									  language,
									  active())); // updated from active_flag on 160702
};

//void Remote_Address::set_value_tracks_location(flag new_flag) {
//	value_tracks_location = new_flag;
//	if (appearance != NULL) {
//	   if (value_tracks_location) {
//			((Text *) appearance)->set_plate_state(TEXT_PAD_WITH_PLATE_ANIMATED);
//		} else {
//			((Text *) appearance)->set_plate_state(TEXT_PAD_WITH_PLATE);
//		};
//	};
//};

void Remote_Address::set_background(Background *new_floor, boolean recur, boolean warn) {
	if (new_floor != floor && active() && !slated_for_deletion() && !tt_moving_stuff_from_room_to_floor && !tt_moving_stuff_from_floor_to_room) {
		// !tt_moving_stuff_from_room_to_floor && !tt_moving_stuff_from_floor_to_room new on 050603 -- 
		// turns out the bug was elsewhere but this is a good optimization anyway (I think)
		if (new_floor == NULL && !to_be_smashed()) { 
			// in transit so go blank -- second condition added on 111104 otherwise adding 2 addresses leads to a blank result
			become_blank();
		} else {
			if (floor == NULL) {
//				if (!contained_in(THOUGHT_BUBBLE)) { // new on 170799 but not sure I need it
					become_non_blank();
//				};
			};
			update_picture();
		};
	};
	Remote_Picture::set_background(new_floor,recur,warn);
};

void Remote_Address::update_picture(boolean regardless) {
	if (!active()) {
		Remote_Picture::update_picture(regardless);
		// following restored on 050603 as an experiment
		set_active(active()); // to set other things like whether pad is animated -- 
		// commented out on 111202 as an experiment since doesn't change the value
		return;
	};
	Block *block = block_im_on();
	if (block != NULL && (block != current_block || !text_value_up_to_date)) { // added text_value_up_to_date on 130300
		current_block = block;
		block_number block_x, block_y;
		block->location(block_x,block_y);
		if (identifier == AVENUE_REMOTE) {
			set_long_value(block_x,FALSE,FALSE,FALSE);
		} else { //if (identifier == STREET_REMOTE) {
			set_long_value(block_y,FALSE,FALSE,FALSE);
		};
//		set_active(TRUE); // in case set_value above reset active_flag
	};
	Remote_Picture::update_picture(regardless);
};

void Remote_Address::set_value(NumberValue *new_value,
										 boolean ignore_change, boolean sizes_constrained,
										 boolean update) {
	// ensures that new_value ranges from 0 to city size-1
	block_number number_of_streets;
	if (identifier == AVENUE_REMOTE) {
		number_of_streets	= tt_city->return_number_of_x_streets();
	} else {
		number_of_streets	= tt_city->return_number_of_y_streets();
	};
	long new_long_value;
	boolean ok = new_value->long_value(new_long_value,TRUE);
	delete new_value; // fixed leak on 111202
	if (!ok) { // warn??
		return;
	};
	new_long_value %= number_of_streets;
	if (new_long_value < 0) new_long_value += number_of_streets;
	// commented out on 261199 since this caused the inert sensor to pick up its real value
	// and then go inert again
//	if (update) {
//		set_active(TRUE,FALSE); // so following does update it
//	};
	Remote_Picture::set_value(new LongNumberValue(new_long_value),ignore_change,sizes_constrained,update);
	if (update && !active()) { // new on 290301 since Remote_Picture::set_value won't do this if inactive
		// removed  && showing_on_a_screen() on 050603 since a robot can be updating it off screen
		update_picture(TRUE);
	};
	text_value_up_to_date = FALSE;
//	if (update) {
//		set_active(FALSE,FALSE); // if updated explictly it is no longer active
//	};
};

void Remote_Address::set_to_future_value(Sprite *other, Sprite *original_recipient, Sprite *by) {
	if (appearance != NULL) { // new on 220499 so the new display gets computed
		appearance->destroy();
		appearance = NULL;
	};
	Rational::set_to_future_value(other,original_recipient,by); // not like the other remotes
};

void Remote_Address::set_active(boolean new_flag, boolean recur, boolean initialization) { // new on 130300
	if (new_flag != active()) text_value_up_to_date = FALSE;
	Remote_Picture::set_active(new_flag,recur,initialization);
};

const_string Remote_Address::narrow_text_value(NaturalLanguage language) {
	// IS THE FOLLOWING A GOOD IDEA??? 
	static character result[32]; // need to last long enough for caller to use and copy
//	string result = new character[32];
	output_string_stream name(result,32);
	string number_first_string = S(IDS_DISPLAY_NUMBER_BEFORE_STREET,"1");
	boolean number_first = (number_first_string[0] == '1');
	long long_value;
	current_long_value(long_value);
	if (identifier == AVENUE_REMOTE) {
		if (number_first) {
			if (tt_avenues_print_as_numbers || long_value < 1) { // disjunct added on 110700 since seems to happen sometimes that it is zero
				name << (int) long_value;
			} else {
				print_ordinal(long_value,name);
			};
			name << space();
		};
      name << S(IDS_AVENUE);
		if (!number_first) {
			name << space();
			if (tt_avenues_print_as_numbers) {
				name << (int) long_value;
			} else {
				print_ordinal(long_value,name);
			};
		};
		name.put('\0');
	} else if (identifier == STREET_REMOTE) {
		if (number_first) {
			name << (int) long_value << space() << S(IDS_STREET);
		} else {
			name << S(IDS_STREET) << space() << (int) long_value;
		};
		name.put('\0');
	};
	text_value_up_to_date = TRUE;
//	string result = (string) name.STR; // safe cast?
   character first[2];
   first[0] = result[0];
   first[1] = NULL;
	upper_case(first);
   result[0] = first[0]; // capitalize if you can
	return(result);
};

long Remote_Address::return_avenue_number() {
	long long_value;
	current_long_value(long_value);
	if (identifier == AVENUE_REMOTE) {
		return(long_value);
	} else {
		return(-1);
	};
};

long Remote_Address::return_street_number() {
	long long_value;
	current_long_value(long_value);
	if (identifier == STREET_REMOTE) {
		return(long_value);
	} else {
		return(-1);
	};
};

long Remote_Address::return_address_number() { 
	// added on 220499 - doesn't distinguish between streets and avenues
//	 return(value);
	// rewritten on 020302
	long long_value;
	current_long_value(long_value);
	return(long_value);
};

/* remote_integer::dump now takes care of this
void Remote_Address::dump(output_stream &stream, boolean just_prepare) {
	if (active()) { // then dump with -1 as value so it is recomputed when reloaded
		long saved_value = value;
		value = min_long;
		Remote_Picture::dump(stream,just_prepare);
		value = saved_value;
	} else {
		Remote_Picture::dump(stream,just_prepare);
	};
};
*/

boolean valid_operation_key(char key) {
  return(key == '-' || key == 'X' || key == 'x' || key == '*' || key == '+' || 
			key == '/' || key == '%' || key == '^' || key == '&' ||
			key == '|' || key == '=' ||
			key == '~'); // new on 040205
};

NumberOperation operation_from_key(char key, boolean plus_is_increase_by) { // new on 100202
	switch (key) {
		case '+':
			if (plus_is_increase_by) {
				return(INCREASE_BY);
			} else {
				return(NO_NUMBER_OPERATION);
			};
		case '-':
			return(SUBTRACT_FROM);
		case '*':
		case 'x':
		case 'X':
			return(MULTIPLY_BY);
		case '/':
			return(DIVIDE_BY);
		case '%':
			return(MODULUS_BY); // or is it really remainder that I'm thinking of??
		case '=':
			return(MAKE_EQUAL);
		case '^': // new incompatible change
			return(TO_THE_POWER);
//		case '^':
//			return(BITWISE_EXCLUSIVE_OR); // just because C does this doesn't mean it is a good idea 
		case '&': // still should work this way??
			return(BITWISE_AND);
		case '|':
			return(BITWISE_OR);
		case '~': // new on 040205
			return(COMPLEMENT);
	};
	return(NO_NUMBER_OPERATION);
};

NumberOperation operation_from_string(string operation_string) {
	NumberOperation result = operation_from_key(operation_string[0],TRUE); // added TRUE on 270202
	if (result != NO_NUMBER_OPERATION) return(result);
	// think about this -- hard to make language independent
	const int number_of_operations = 12;
	NumberOperation operations[number_of_operations] = {INTEGER_PART,FRACTION_PART,NUMERATOR,DENOMINATOR,
																		 SINE,COSINE,TANGENT,ARCSINE,ARCCOSINE,ARCTANGENT,NATURAL_LOG,LOG10};
	for (int i = 0; i < number_of_operations; i++) {
		string candidate_operation_string = operation_to_string(operations[i]);
		if (strncmp(operation_string,candidate_operation_string,strlen(candidate_operation_string)) == 0) { // rewritten on 150202 to work correctly
			return(operations[i]);
		};
	};
	return(result); // warn??
};

string operation_to_string(NumberOperation operation) {
	// text for use within operations themselves
	switch (operation) {
		case SUBTRACT_FROM:
			return("-");
		case MULTIPLY_BY:
			return("*");
		case INCREASE_BY:
			return("+");
		case DIVIDE_BY:
			return("/");
		case MODULUS_BY:
			return("%"); // or is it really remainder that I"m thinking of??
		case MAKE_EQUAL:
			return("=");
		case TO_THE_POWER: // new incompatible change
			return("^");
		case BITWISE_EXCLUSIVE_OR:
			return(S(IDS_BITWISE_XOR,"Bitwise XOR")); // no longer following C conventions
		case BITWISE_AND:
			return("&"); // changed back on 270202 since is confusing when typing & -- was S(IDS_BITWISE_AND,"Bitwise AND"));
		case BITWISE_OR:
			return("|"); // changed back on 270202 -- was S(IDS_BITWISE_OR,"Bitwise OR"));
		case COMPLEMENT:
			return("~"); // good idea?
		case INTEGER_PART:
			return(S(IDS_INTEGER_PART,"Integer Part"));
		case FRACTION_PART:
			return(S(IDS_FRACTION_PART,"Fraction Part"));
		case NUMERATOR:
			return(S(IDS_NUMERATOR,"Numerator"));
		case DENOMINATOR:
			return(S(IDS_DENOMINATOR,"Denominator"));
		case SINE:
			return(S(IDS_SINE,"Sine"));
		case COSINE:
			return(S(IDS_COSINE,"Cosine"));
		case TANGENT:
			return(S(IDS_TANGENT,"Tangent"));
		case ARCSINE:
			return(S(IDS_ARCSINE,"Arc Sine"));
		case ARCCOSINE:
			return(S(IDS_ARCCOSINE,"Arc Cosine"));
		case ARCTANGENT:
			return(S(IDS_ARCTANGENT,"Arc Tangent"));
		case NATURAL_LOG:
			return(S(IDS_NATURAL_LOG,"Natural Log"));
		case LOG10:
			return(S(IDS_LOG10,"Log Base 10"));
	};
	return(""); // was NULL prior to 220702 but that causes problems with ostream
};

wide_string operation_to_wide_string(NumberOperation operation) { // note this is for English so that when creating XML the English name is saved
	// text for use within operations themselves
	switch (operation) {
		case SUBTRACT_FROM:
			return(L"-");
		case MULTIPLY_BY:
			return(L"*");
		case INCREASE_BY:
			return(L"+");
		case DIVIDE_BY:
			return(L"/");
		case MODULUS_BY:
			return(L"%"); // or is it really remainder that I"m thinking of??
		case MAKE_EQUAL:
			return(L"=");
		case TO_THE_POWER: // new incompatible change
			return(L"^");
		case BITWISE_EXCLUSIVE_OR:
			return(L"Bitwise XOR"); // no longer following C conventions
		case BITWISE_AND:
			return(L"&"); // changed back on 270202 since is confusing when typing & -- was S(IDS_BITWISE_AND,"Bitwise AND"));
		case BITWISE_OR:
			return(L"|"); // changed back on 270202 -- was S(IDS_BITWISE_OR,"Bitwise OR"));
		case COMPLEMENT:
			return(L"~"); // good idea?
		case INTEGER_PART:
			return(L"Integer Part");
		case FRACTION_PART:
			return(L"Fraction Part");
		case NUMERATOR:
			return(L"Numerator");
		case DENOMINATOR:
			return(L"Denominator");
		case SINE:
			return(L"Sine");
		case COSINE:
			return(L"Cosine");
		case TANGENT:
			return(L"Tangent");
		case ARCSINE:
			return(L"Arc Sine");
		case ARCCOSINE:
			return(L"Arc Cosine");
		case ARCTANGENT:
			return(L"Arc Tangent");
		case NATURAL_LOG:
			return(L"Natural Log");
		case LOG10:
			return(L"Log Base 10");
	};
	return(L""); // was NULL prior to 220702 but that causes problems with ostream
};

// following are wide_string versions of the above - new on 140104 to simplify loading XML of operations (and speed up)
NumberOperation operation_from_string(wide_string operation_string) {
	NumberOperation result = NO_NUMBER_OPERATION;
	if (operation_string[0] < 256) {
		result = operation_from_key((char) operation_string[0],TRUE); // added TRUE on 270202
	};
	if (result != NO_NUMBER_OPERATION) return(result);
	// think about this -- hard to make language independent
	const int number_of_operations = 12;
	NumberOperation operations[number_of_operations] = {INTEGER_PART,FRACTION_PART,NUMERATOR,DENOMINATOR,
																		 SINE,COSINE,TANGENT,ARCSINE,ARCCOSINE,ARCTANGENT,NATURAL_LOG,LOG10};
	for (int i = 0; i < number_of_operations; i++) {
		wide_string candidate_operation_string = operation_to_wide_string(operations[i]);
		if (wcsncmp(operation_string,candidate_operation_string,wide_string_length(candidate_operation_string)) == 0) { 
			// rewritten on 150202 to work correctly
			return(operations[i]);
		};
	};
	return(result); // warn??
};

/*
	// the following are functions of one arg - should they really work the way they do?? - 
	// immediate is mysterious (and hard to undo) 
	case 'i': // good idea? what if someday I want complex numbers?
	case 'I':
		return(INTEGER_PART);
	case 'f':
	case 'F':
		return(FRACTION_PART);
		// and the trig functions
*/




