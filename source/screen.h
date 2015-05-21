// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.
#if !defined(__TT_SCREEN_H)

#define __TT_SCREEN_H

#if !defined(__TT_GLOBALS_H)   
#include "globals.h" // e.g. for tt_redraw_all
#endif   
#if !defined(__TT_REGION_H)   
#include "region.h"
#endif   
#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif   
//#include "event.h"
#if !defined(__TT_BCKGRND_H)   
#include "bckgrnd.h"
#endif   
//class Programmer_State;
//#include "prgrmmr.h"
//class Background;
#if TT_WINDOWS&&!GRAPHICS_TO_MEMORY
#if !defined(__TT_WINMAIN_H)   
#include "winmain.h"
#endif   
#endif

#if HIGH_RESOLUTION_CLOCK
class HTimer;
#endif
//typedef Sprite **Layers; // array of pointers to display instances
class Screen {
  public:
	 Screen();
	 ~Screen();
	 boolean add(Sprite *display, boolean signal_screen_change=TRUE);
//	 void add_to_background(Sprite *display) {
//		 background->add_item(display);
//	 };
	 void remove(Sprite *display);
//	 void remove_from_background(Sprite *display) {
//		 background->remove_item(display);
//	 };
//	 void replace(Sprite *old_display, Sprite *new_display);
	 boolean showing(Sprite *item);
	 boolean update_display();
	 void propagate_changes();
	 void save_floor_priorities();
	 void display(boolean clean_background);
	 void display_subtitles();
    void redisplay_above(Sprite *layer, TTRegion &region);
	 void display_layer(Sprite *sprite, boolean followers_too=TRUE, boolean out_of_order=FALSE);
	 void cache_followers(Sprite *sprite, int index);
	 void check_out_followers(Sprite *sprite);
//	 void cache_background();
	 void invalidate_cache();
    void screen_dirty();
	 void background_being_destroyed(Background *background_being_destroy);
	 boolean save_city_region(TTRegion &region);
//    void save_city_region(city_coordinate layer_llx,
//                          city_coordinate layer_lly,
//                          int layer_width,
//                          int layer_height);
#if TT_XML
	 xml_element *xml(xml_document *document);
	 boolean handle_xml_tag(Tag tag, xml_node *node);
#endif
    void update_viewing_region(city_coordinate region_center_x,
                               city_coordinate region_center_y,
                               city_scale scale);
    void move_by(city_coordinate delta_x, city_coordinate delta_y);
    void viewing_region(city_coordinate &region_center_x,
                        city_coordinate &region_center_y,
                        city_scale &region_scale);
	 void clear(color_index background);
    boolean visible_region(city_coordinate rect_min_x,
                           city_coordinate rect_max_x,
                           city_coordinate rect_min_y,
									city_coordinate rect_max_y);
#if !WING&&!TT_DIRECTX
	 void clip_rectangle(color color,
								city_coordinate rect_min_x,
								city_coordinate rect_max_x,
								city_coordinate rect_min_y,
								city_coordinate rect_max_y);
#endif
#if WING||TT_DIRECTX
//	 void clip_rectangle(city_coordinate rect_min_x,
//								city_coordinate rect_max_x,
//								city_coordinate rect_min_y,
//								city_coordinate rect_max_y);
	 void draw_rectangle(city_coordinate rect_min_x,
								city_coordinate rect_max_x,
								city_coordinate rect_min_y,
								city_coordinate rect_max_y);
    void draw_synthetic_shape(SyntheticShape shape,
										city_coordinate rect_min_x,
									   city_coordinate rect_max_x,
									   city_coordinate rect_min_y,
									   city_coordinate rect_max_y);
	 void set_brush_origin(city_coordinate x, city_coordinate y);
#else
	 void draw_rectangle(color color,
								city_coordinate rect_min_x,
								city_coordinate rect_max_x,
								city_coordinate rect_min_y,
								city_coordinate rect_max_y);
#endif
//	 void display_background(int index);
	 void place_character(character character, 
								 city_coordinate ulx, city_coordinate uly,
						       city_coordinate width, city_coordinate height,
                         color text_color=BLACK);
	 void place_wide_character(wide_character character, 
								 city_coordinate ulx, city_coordinate uly,
						       city_coordinate width, city_coordinate height,
                         color text_color=BLACK);
	 void place_number(long number,
							 city_coordinate ulx, city_coordinate uly,
							 city_coordinate character_width,
							 city_coordinate character_height);
	 city_coordinate get_extent(const_string text, int length,
                               city_coordinate new_font_width, city_coordinate new_font_height,
						             boolean bold=FALSE, boolean fixed_width=FALSE, boolean might_contain_tabs=TRUE);
	 void get_extent_size(wide_string text, int length, int number_of_lines, 
                         city_coordinate width, city_coordinate height, int &longest_line, city_coordinate &new_width, city_coordinate &new_height,
				             boolean bold=FALSE, boolean fixed_width=FALSE, boolean might_contain_tabs=TRUE);
#if TT_WIDE_TEXT
	 city_coordinate get_extent(wide_string text, int length,
                               city_coordinate new_font_width, city_coordinate new_font_height,
						             boolean bold=FALSE, boolean fixed_width=FALSE, boolean might_contain_tabs=TRUE);
#endif
	 city_coordinate get_vertical_extent(const_string text, int length,
                               city_coordinate new_font_width, city_coordinate new_font_height,
						             boolean bold=FALSE, boolean fixed_width=FALSE);
#if TT_WIDE_TEXT
	 city_coordinate get_vertical_extent(wide_string text, int length,
                               city_coordinate new_font_width, city_coordinate new_font_height,
						             boolean bold=FALSE, boolean fixed_width=FALSE);
#endif
	 void set_font(city_coordinate new_font_width, city_coordinate new_font_height,
						boolean bold=FALSE, boolean fixed_width=FALSE);
	 boolean text_out(const_string text, int length, 
							city_coordinate ulx, city_coordinate uly,
							city_coordinate character_width,
							city_coordinate character_height,
							boolean bold=FALSE, boolean fixed_width=FALSE,
							boolean background_transparent=TRUE,
							color_index text_color=tt_black,
							boolean text_is_wide=FALSE,
							bytes color_permutation=NULL,
							color background_color=WHITE,
							city_coordinate total_width=0,
							boolean might_contain_tabs=TRUE,
							int caret_location=-1);
//						city_coordinate urx=0, city_coordinate ury=0); // only used if !background_transparent
//	 boolean create_caret(city_coordinate height);
	 void draw_line(city_coordinate start_x, city_coordinate start_y, city_coordinate end_x, city_coordinate end_y);
	 void correct_font_size(wide_string text, int length, int number_of_lines,
									city_coordinate width, city_coordinate height,
									boolean bold, boolean fixed_width, // added fixed_width on 050102
									boolean might_contain_tabs,
//									city_coordinate max_total_width,	city_coordinate max_total_height, // removed on 180102
									city_coordinate &new_font_width, city_coordinate &new_font_height);
	 int place_text(const_string text, int length, 
						 city_coordinate ulx, city_coordinate uly,
						 city_coordinate max_width, city_coordinate max_height,
						 city_coordinate character_width,
						 city_coordinate character_height,
                   WhenToBreakSentences break_sentences=BREAK_SENTENCES_TO_NOT_WASTE_HALF_BALLOON,
						 boolean vertically_center=FALSE,
                   boolean dots_if_more=FALSE,
						 city_coordinate extra_width_at_center=0,
						 city_coordinate blank_width=0,
                   color text_color=BLACK,
						 boolean first_call=TRUE,
						 long max_length=max_long);
//	 boolean confirm_quit(); // commented out on 050202 since apparently obsolete
	 void move_mark_to(city_coordinate x, city_coordinate y);
	 //long horizontal_pixel_length(city_coordinate x);
	 //long vertical_pixel_length(city_coordinate y);
	 //double horizontal_pixel_length_as_double(city_coordinate x);
	 //double vertical_pixel_length_as_double(city_coordinate y);
	 // following rewritten on 210804 to use pre-computed doubles rather than worry about overflow etc.
	 // moved here to be open-coded on 111104
	long horizontal_pixel_length(city_coordinate x) {
		return((long) (x*pixels_per_horizontal_city_coordinates));
	};
	long vertical_pixel_length(city_coordinate y) {
		return((long) (y*pixels_per_vertical_city_coordinates));
	};
	double horizontal_pixel_length_as_double(city_coordinate x) { // new on 080803 - initial for better quality set_font
		return(x*pixels_per_horizontal_city_coordinates);
	};
	double vertical_pixel_length_as_double(city_coordinate y) {
		return(y*pixels_per_vertical_city_coordinates);
	};
	long horizontal_city_coordinate_length(coordinate x) { // new on 111104
		return((long) (x/pixels_per_horizontal_city_coordinates));
	};
	long vertical_city_coordinate_length(coordinate y) { // new on 111104
		return((long) (y/pixels_per_vertical_city_coordinates));
	};
//	 long old_horizontal_pixel_length(city_coordinate x) {
//		 return(x/x_city_units_per_pixel);
//	 };
//	 long old_vertical_pixel_length(city_coordinate y) {
//       return(y/y_city_units_per_pixel);
//    };
    // ideal_horizontal_pixel_length are not dependent upon scale
    // and so are just in line functions
    city_coordinate one_pixel_in_x_city_units() {
      return(x_city_units_per_pixel);
    };
	 city_coordinate one_pixel_in_y_city_units() {
		return(y_city_units_per_pixel);
	 };
	 double one_pixel_in_x_city_units_as_double() {
      return(horizontal_city_coordinates_per_pixel);
    };
	 double one_pixel_in_y_city_units_as_double() {
		return(vertical_city_coordinates_per_pixel);
	 };
	 boolean more_than_pixel_by_pixel(TTRegion *region);
	 TTRegion *viewing_region() {
      return(&view_region);
    };
	 void save_city_region(TTRegion &region, bytes memory);
	 void restore_city_region(TTRegion &region, bytes memory);
	 long city_region_pixel_size(TTRegion &region);
	 void set_cursor_position(city_coordinate x, city_coordinate y);
	 boolean region_is_free(TTRegion *region, Sprite *exception=NULL);
    CameraStatus camera_status() {
      return(view_status);
	 };
    void new_view(CameraStatus status);
//    void increment_number_needing_collision_notification(int change) {
//       number_needing_collision_notification += change;
//    };
	 boolean inside_rule_body() {
      return(background != NULL && background->inside_rule_body());
	 };
	 boolean inside_a_house() {
		return(background != NULL && background->inside_a_house());
	 };
	 boolean in_the_room() {
		 return(background != NULL && background->in_the_room());
	 };
	 void initialize_background(Background *new_background) {
		 background = new_background;
	 };
	 void switch_to(Background *new_background,
						 boolean keep_animating=TRUE, 
						 boolean old_background_ok=TRUE,
						 boolean new_background_finish_instantly=FALSE);
	 void flush_all();
//	 void remove_subtitle(); // was redundant with remove_current_subtitle
	 void subtitle_need_not_wait_for_speech();
    void set_subtitle(string line, short int length, millisecond duration);
	 coordinate screen_x(city_coordinate x);
	 coordinate screen_y(city_coordinate y);
	 city_scale current_scale() {
		 return(scale);
	 };
	 void set_scale(city_scale new_scale) {
		 scale = max(1,new_scale); // updated on 200103 so is never 0 or negative - can happen with broken log files flying too high
		 set_city_coordinates_per_pixel(); 
	 };
	 city_coordinate min_x() {
		 return(view_region.min_x);
	 };
	 city_coordinate min_y() {
		 return(view_region.min_y);
	 };
	 city_coordinate max_x() {
		 return(view_region.max_x);
	 };
	 city_coordinate max_y() {
		 return(view_region.max_y);
	 };
	 city_coordinate viewing_width() {
		 return(view_region.width());
	 };
	 city_coordinate viewing_height() {
		 return(view_region.height());
	 };
	 boolean off_screen(city_coordinate llx, city_coordinate lly, city_coordinate width, city_coordinate height) {
		return (llx > view_region.max_x || lly > view_region.max_y ||
				  llx+width < view_region.min_x || lly+height < view_region.min_y);
	 };
	 Sprite *first_to_contain(city_coordinate x, city_coordinate y);
	 Sprites *all_to_collide_with(TTRegion *region);
	 Sprite *first_to_collide_with(Sprite *layer, Sprite *exception, PredicateCallBack predicate=NULL, void *predicate_arg=NULL);
	 Sprite *first_to_collide_with(Sprite *layer, long min_priority, PredicateCallBack predicate=NULL, void *predicate_arg=NULL);
	 Sprite *next_tool_true_region_to_collide_with(TTRegion *region, Sprite *underneath=NULL,
												              Sprite *start_after=NULL, PredicateCallBack predicate=NULL, void *predicate_arg=NULL);
    Sprite *next_tool_full_collision_region_to_collide_with(TTRegion *region, Sprite *underneath=NULL,
												                        Sprite *start_after=NULL, PredicateCallBack predicate=NULL, void *predicate_arg=NULL,
																				boolean convert_to_picture_if_appropriate=FALSE,
																				boolean loop_around_if_not_found=TRUE);
	 Sprite *next_tool_to_collide_with(TTRegion *region, Sprite *underneath=NULL,
												  Sprite *start_after=NULL, PredicateCallBack predicate=NULL, void *predicate_arg=NULL);
	 Sprite *first_tool_to_collide_with(TTRegion *region, Sprite *underneath=NULL, PredicateCallBack predicate=NULL, void *predicate_arg=NULL) {
		 return(next_tool_to_collide_with(region,underneath,NULL,predicate,predicate_arg));
	 };
	 Sprite *first_to_collide_with(TTRegion *region, Sprite *underneath=NULL, PredicateCallBack predicate=NULL, void *predicate_arg=NULL);
	 Sprite *first_of_type_to_collide_with(TTRegion *region, SpriteType type, PredicateCallBack predicate=NULL, void *predicate_arg=NULL);
	 Sprite *first_cached_to_overlap_with(Sprite *sprite);
	 Sprite *first_cached_on_top_of(Sprite *sprite);
	 Sprite *postponed_layer_underneath(Sprite *layer);
	 Sprite *first_to_obscure(int index, Sprite *follower=NULL);
//	 Sprite *first_game_element_to_collide_with(TTRegion *region, 
//															  Sprite *exception);
	 Sprite *next_to_collide_with(Sprite *layer, Sprite *below_me,
	                              PredicateCallBack predicate=NULL, void *predicate_arg=NULL);
//    boolean move_region_to_free_area(TTRegion *region, int number_of_tries,
//												 Sprite *exception);
	 void postpone_layer(Sprite *layer);
    user_coordinate user_x(city_coordinate x) {
       // easier for kids to deal with units of 0 to 1000 to cross the screen
	    // for now just works inside
       return(x/city_to_user_horizontal_units);
//	    return(x/(ideal_screen_width/1000));
    };
    user_coordinate user_y(city_coordinate y) {
       return(y/city_to_user_vertical_units);
//       return(y/(ideal_screen_height/1000));
    };
	 double user_x_double_float(double x) {
       return(x/city_to_user_horizontal_units);
    };
    double user_y_double_float(double y) {
       return(y/city_to_user_vertical_units);
    };
    city_coordinate from_user_x(user_coordinate x);
    city_coordinate from_user_y(user_coordinate y);
	 city_coordinate from_user_x(double x);
	 city_coordinate from_user_y(double y);
    void set_city_to_user_units(long x, long y) {
       city_to_user_horizontal_units = x;
       city_to_user_vertical_units = y;
    };
    Background *pointer_to_background() {
		 return(background);
	 };
	 void set_selection(Sprite *selection) {
		 current_selection = selection;
	 };
    boolean is_cache_valid() {
       return(cache_valid);
    };
    boolean subtitles_showing() {
       return(subtitle_stop_time > 0);
    };
	 void remove_current_subtitle();
    boolean inside_thought_bubble() {
       return(background != NULL && background->inside_thought_bubble());
    };
	 void set_number_of_subtitle_lines_last_time(int value) {
		 number_of_subtitle_lines_last_time = value;
	 };
	 int return_number_of_subtitle_lines_last_time() {
		 return(number_of_subtitle_lines_last_time);
	 };
	 boolean displaying_subtitle(boolean only_if_unscheduled);
	 city_coordinate x_pixel_to_screen(coordinate x) { // new on 240100
		 return(x*x_city_units_per_pixel+view_region.min_x);
	 };
	 city_coordinate y_pixel_to_screen(coordinate y) {
		 return(view_region.max_y-y*y_city_units_per_pixel);
	 };
	 coordinate return_subtitle_character_width() { // new on 150101
		 return(subtitle_character_width);
	 };
	 coordinate return_subtitle_character_height() {
		 return(subtitle_character_height);
	 };
	 boolean first_layer_is(Sprite *sprite) { // new on 111204 to know if OK to cache Tooly
		 return(layers[0] == sprite); 
	 };
//#if TT_DEBUG_ON
//	 void display_debug_info(millisecond current_delta);
//#endif
//    Sprite *first_to_collide_with_except(TTRegion *region, Sprite *exception);
/*
	 void add_leader(Sprite *sprite) {
		 leaders.insert(sprite);
	 };
	 void remove_leader(Sprite *sprite) {
		 leaders.remove(sprite);
	 };
*/
  private: // protected maybe makes more sense but not subclassed so doesn't matter
	 void update_layers();
    void sort();
	 void set_city_coordinates_per_pixel();
//    void detect_collisions();
	 SpritePointer *layers; // max_number_of_layers];
	 SpritePointer *postponed_layers; // max_number_of_postponed_layers];
	 int max_number_of_layers;
	 int max_number_of_postponed_layers;
	 int postponed_count;
	 TTRegion view_region;
	 Background *background;
//    static const int max_number_of_layers;
    int layer_count;
//    long background_unchanged_count;
	 int width, height; // in pixels
//    int x_offset, y_offset;  // pixels (for multiple "screens") -- commented out on 301004 since not used and many changes will be needed to support multiple screens
    coordinate pixel_min_x, pixel_max_x, pixel_min_y, pixel_max_y;
//    long backmost_unchanged_layer_index, can_cache_for_last_n_frames;
//    boolean background_cache_valid; // cached;
    CameraStatus view_status;
    city_coordinate min_horizontal_feature, min_vertical_feature,
                    min_rect_width, min_rect_height;
    city_coordinate center_x, center_y;
    city_scale scale;
	 double pixels_per_horizontal_city_coordinates, pixels_per_vertical_city_coordinates; // new on 210804 for accuracy and avoiding overflow problems
	 double horizontal_city_coordinates_per_pixel, vertical_city_coordinates_per_pixel;
	 // renamed below as above on 210804 to be more consistent
//	 double x_city_units_per_pixel_as_double, y_city_units_per_pixel_as_double; // new on 110302 since the round off error can be quite noticable (e.g. typing lots of digits to a number)
	 long x_city_units_per_pixel, y_city_units_per_pixel;
	 //	 long x_half_city_units_per_pixel, y_half_city_units_per_pixel;
    Sprite *current_selection;
    flag cache_valid;
//    int number_needing_collision_notification;
//#if HIGH_RESOLUTION_CLOCK
//	 HTimer *performance_timer;
//#endif
//    Sprites leaders;
    short int subtitle_length; //, previous_subtitle_length, blackened_count;
    millisecond subtitle_stop_time;
    wide_string subtitle; //, previous_subtitle;
    coordinate subtitle_character_width, subtitle_character_height;
    long city_to_user_horizontal_units, city_to_user_vertical_units;
	 int number_of_subtitle_lines_last_time;
	 millisecond duration_of_current_subtitle;
};

// increased the accurracy of these on 170299 by rounding to nearest (assuming positive value for x)
//inline
long ideal_horizontal_pixel_length(city_coordinate x);

//inline 
long ideal_vertical_pixel_length(city_coordinate y);

//double ideal_horizontal_pixel_length_as_double(city_coordinate x);
//double ideal_vertical_pixel_length_as_double(city_coordinate y);

//inline long ideal_horizontal_pixel_length(city_coordinate x) {
//	return(x/tt_ideal_units_per_horizontal_pixel);
//};

//inline long ideal_vertical_pixel_length(city_coordinate y) {
//   return(y/tt_ideal_units_per_vertical_pixel);
//};

//inline 
city_coordinate ideal_horizontal_units(coordinate x);

//inline 
city_coordinate ideal_vertical_units(coordinate y) ;

#endif
