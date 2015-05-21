// Copyright (c) 1992-2003.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)
#include "defs.h"
#endif
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif   
#if !defined(__TT_STRINGS_H)
#include "strings.h"
#endif
#if !defined(__TT_SPRITE_H)
#include "sprite.h"
#endif
#if !defined(__TT_SCREEN_H)
#include "screen.h"
#endif
#if !defined(__TT_REGION_H)   
#include "region.h"
#endif
#if !defined(__TT_ROOM_H)   
#include "room.h"
#endif
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif
#if !defined(__TT_CLEANUP_H)   
#include "cleanup.h"
#endif
#if !defined(__TT_BLIT_H)   
#include "blit.h"
#endif
#if !defined(__TT_SPRITES_H)
#include "sprites.h"
#endif   
#if !defined(__TT_TOOLS_H)
#include "tools.h"
#endif
#if !defined(__TT_ROBOT_H)   
#include "robot.h"
#endif 
#if !defined(__TT_PRGRMMR_H)   
#include "prgrmmr.h"
#endif
#if !defined(__TT_BIRD_H)   
#include "bird.h"
#endif   
#if !defined(__TT_FLOOR_H)
#include "floor.h"
#endif   
#if !defined(__TT_PAD_H)
#include "pad.h"
#endif
#if !defined(__TT_MARTIAN_H)   
#include "martian.h"
#endif
#if !defined(__TT_PICTURE_H)   
#include "picture.h"
#endif
#if !defined(__TT_NUMBER_H)   
#include "number.h"
#endif
#if !defined(__TT_HELP_H)
#include "help.h"
#endif
#if !defined(__TT_LOG_H)
#include "log.h"
#endif
#if !defined(__TT_CACHE_H)
#include "cache.h"
#endif
#if !defined(__TT_JAVA_H)
#include "java.h"
#endif
#if TT_TEXT_TO_SPEECH
#if !defined(__TT_SPEAK_H)
#include "speak.h"
#endif
#endif
#if !defined(__TT_TEXT_H)
#include "text.h"
#endif
#if !defined(__TT_FILE_H)
#include "file.h"
#endif
#if TT_XML
#if !defined(__TT_XML_H)
#include "xml.h"
#endif
#endif

#include "filestts.h" // new on 021001

#include "wingutil.h"
// Windows Header Files:
#include <windows.h>
#include <mmsystem.h>
#include <windowsx.h> // for GlobalFreePtr
   

//#include <fstream.h>
//#include <strstrea.h> commented out 210703
#include <string.h>
#if !TT_32
#include <stdlib.h> // for min, max, range and atoi
#endif
#if SMART_HEAP||HEAP_AGENT
#include <heapagnt.h>
#endif
//#include <ctype.h> // for toupper
#if !defined(__TT_WINMAIN_H)   
#include "winmain.h"
#endif

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

boolean need_to_clobber_image(int resource_index);
//void free_backgrounds();
//void free_resource(int resource_index
//#if !TT_32
//							, HGLOBAL memory_handle
//#endif
//);

work_page allocate_bytes(long size);

//short int number_of_sprite_files; // should be same as tt_appearance_count
short int number_of_image_files; // same as LAST_IMAGE??
short int number_of_sound_files = 0; // in case none required
long int *image_offsets = NULL;
//long int *compressed_image_offsets = NULL;
long int *sprite_offsets = NULL;
long int *sound_offsets = NULL;
#if TT_DEBUG_ON
long *images_retrieved = NULL;
long *sounds_retrieved = NULL;
#endif

unsigned short *sound_cache_indices = NULL;
unsigned short *sound_cache_ids = NULL;

//input_file_stream images_file;
HFILE images_file = -1;
//input_file_stream resolution_independent_file;

HFILE resolution_independent_file = -1;
int number_of_resources;

//int return_debug_counter() {
//#if TT_DEBUG_ON
//	return(counter_for_debugging);
//#else
//	return(0);
//#endif
//};
//
//void set_debug_counter(int n) {
//#if TT_DEBUG_ON
//	counter_for_debugging = n;
//#endif
//};

inline bytes color_permutation() { // new on 200901
	return(tt_city->stopped()?tt_dark_color_indices:NULL);
};

// if cycles have different heights or widths this tracks the maximums
// +1 for a slot for the synthetic sprites (e.g. rectangles)
city_coordinate *max_widths = NULL;
city_coordinate *max_heights;

flag *priority_function_of_lly_flags; // was boolean
flag *collision_region_equals_region;
short int *collision_width_factors;
short int *collision_height_factors;
short int *collision_x_offset_correction;
short int *collision_y_offset_correction;
short int *parameter_counts;

//typedef int *IdealParameters;
//IdealParameters ideal_parameters[tt_appearance_count];
typedef Image_cycle *ImageCycles;
ImageCycles *image_cycles;
TTImagePointer saved_heads[4]; // added extra level of indirection on 231001 for flexibility -- make this obsolete 170603
BOOL saved_heads_is_for_hat = FALSE; // new on 190603 -- initially it is for bald
//TTImagePointer extra_walking_hat_images[4]; // new on 180603 to save the extra 4 images used to work around the clipping problem for wearing a hat
//TTImagePointer extra_walking_bald_images[4]; // new on 190603 - pointer to original image to restore if time travelling with different appearances 
//short int appearance_count = 57;

const short int image_count = 1250; // 1161 is the count on 6/5/97 -- a bit extra for growth -- changed on 171201 since there are now 1205

//void initialize_head_images() { // abstracted on 180603 -- what good does this do??
//	if (tt_builtin_picture_directory == NULL) { // new on 231001
//		for (int i = 0; i < 4; i++) {
//			saved_heads[i] = new TTImage();
////			extra_heads[i] = new TTImage();
//		};
//	} else {
//		for (int i = 0; i < 4; i++) {
//			saved_heads[i] = new UserImage();
////			extra_heads[i] = new UserImage();
//		};
//	};
//};

//void update_saved_heads(int programmer_head) {
	// experiment to not do this since extra_heads are not saved_heads but hat_heads now... - 180603
//	if (programmer_head == 2) { // new on 180603
//		for (int i = 0; i < 4; i++) {
//			if (extra_heads[i] != NULL) {
////				delete saved_heads[i]; // new on 160103 -- commented out on 180603 since shared with extra_heads now
//				saved_heads[i] = extra_heads[i];
//			};
//		};
//	};
//};

boolean allocate_appearance_data() {
	if (max_widths != NULL) return(FALSE); // already allocated
	max_widths = new city_coordinate[tt_appearance_count];
	max_heights = new city_coordinate[tt_appearance_count];
	priority_function_of_lly_flags = new flag[tt_appearance_count];
	collision_region_equals_region = new flag[tt_appearance_count];
	collision_width_factors = new short int[tt_appearance_count];
	collision_height_factors = new short int[tt_appearance_count];
	collision_x_offset_correction = new short int[tt_appearance_count];
	collision_y_offset_correction = new short int[tt_appearance_count];
	parameter_counts = new short int[tt_appearance_count];
	image_cycles = new ImageCycles[tt_appearance_count];
	if (tt_builtin_picture_directory == NULL) { // new on 231001
		for (int i = 0; i < 4; i++) {
			saved_heads[i] = new TTImage();
		};
	} else {
		for (int i = 0; i < 4; i++) {
			saved_heads[i] = new UserImage();
		};
	};
   tt_image_file_names = new ascii_string[image_count];  // update this when adding new images
   for (int i = 0; i < image_count; i++) {
       tt_image_file_names[i] = NULL;
   };
	return(TRUE);
};

void deallocate_saved_heads() { // abstracted on 180603 -- restored on 130104 
   // commented out the same day since too hard to know how much is shared with extra_heads and programmer appearance
	if (tt_programmer_head != 2) { // haven't already swapped them
		for (int i = 0; i < 4; i++) { 
			delete saved_heads[i];
		};
	};
//   delete [] saved_heads;
};

void deallocate_appearance_data() {
	if (max_widths == NULL) return; // hasn't been allocated yet
//	MessageBox(NULL,"deallocate_appearance_data","TT",MB_OK); // just for testing!! 270105
	delete [] max_widths;
	max_widths = NULL; // new on 240105
	delete [] max_heights;
	max_heights = NULL; // new on 240105
	delete [] priority_function_of_lly_flags;
	priority_function_of_lly_flags = NULL; // new on 240105
	delete [] collision_region_equals_region;
	collision_region_equals_region = NULL; // new on 240105
	delete [] collision_width_factors;
	collision_width_factors = NULL; // new on 240105
	delete [] collision_height_factors;
	collision_height_factors = NULL; // new on 240105
	delete [] collision_x_offset_correction;
	collision_x_offset_correction = NULL; // new on 240105
	delete [] collision_y_offset_correction;
	collision_y_offset_correction = NULL; // new on 240105
	delete [] parameter_counts;
	parameter_counts = NULL; // new on 240105
	//for (int i = 0; i < tt_appearance_count; i++) { // new on 160103
	//	if (i != SYNTHETIC_SPRITE && i != USER_SPRITE) delete image_cycles[i];
	//};
//	deallocate_saved_heads();
	delete [] image_cycles;
	image_cycles = NULL; // new on 240105
   for (int i = 0; i < image_count; i++) {
		if (tt_image_file_names[i] != NULL) {
			delete [] tt_image_file_names[i];
			tt_image_file_names[i] = NULL; // new on 240105
		};
   };
   delete [] tt_image_file_names;
	tt_image_file_names = NULL; // new on 240105
};

void reallocate_appearance_data() { // new on 240105
	deallocate_appearance_data();
	_lclose(images_file);
	images_file = -1;
};

city_coordinate maximum_width(SpriteCode code) {
	return(max_widths[code]);
};

city_coordinate maximum_height(SpriteCode code) {
	return(max_heights[code]);
};

boolean still_picture(SpriteCode code) {
	return(code == SYNTHETIC_SPRITE ||
          code == USER_SPRITE || // added this on 191102
		    code < 0 || // a bad code
			 code >= tt_appearance_count || // another bad code
          (parameter_counts[code] == 1 &&
			  image_cycles[code][0].return_number_of_images() == 1));
};

boolean currently_still_picture(SpriteCode code, int parameter) {
   return(code == SYNTHETIC_SPRITE || 
          code == USER_SPRITE || // added this on 191102
		    image_cycles[code][parameter].return_number_of_images() == 1);
};

int parameter_count(SpriteCode code) {
	return(parameter_counts[code]);
};

#if TT_XML

xml_document *Entity::xml_new_document(boolean include_explanation) { // new on 040803 - abstracted and moved from Sprite
   xml_document *document = create_xml_document();
   add_processing_instruction(document);
   if (include_explanation) { // conditional new on 041002
      const int max_explanation = 2048; // English preamble is about 700 characters so this should be enough
      char explanation[max_explanation]; 
      output_string_stream stream(explanation,max_explanation);
      print_clipboard_explanation(stream,entity_type()); // was kind_of() prior to 040803
      stream.put('\0'); // terminate string
      if (explanation[0] != '\0') { // not a null comment
         wide_string wide_explanation = copy_wide_string(explanation);
         add_comment(document,wide_explanation);
         delete [] wide_explanation;
      };
   };
	return(document);
};

HGLOBAL Entity::xml_global_handle() { // moved here from Sprite on 040803
	UnwindProtect<boolean> set(tt_include_descriptions_in_xml,TRUE); // rewritten 050304 so that don't do with with 'Save to File'
   xml_document *document = xml_new_document(TRUE);
	if (document == NULL) {
		return(NULL); // new on 140105 -- not likely but why not check 
	};
	xml_element *toontalk_object = create_xml_element(L"ToonTalkObject",document); // commented out on 210105 since not used if entity
	// is a house or a city
	// restored on 240105 since it broke control-c
#if TT_LOADING_AND_SAVING_INTERRUPTABLE
	try { // try is new on 140105
		tt_sprites_saved_count = 0; // new on 150105
		top_level_xml(document,toontalk_object);
	} catch (int ) {
		return(NULL);
	};
#else
	top_level_xml(document,toontalk_object);
#endif
	xml_append_child(toontalk_object,document); // removed 210105 -- restored 230105
//   document->put_preserveWhiteSpace(VARIANT_FALSE); // experiment on 311002
   BSTR xml;
   document->get_xml(&xml);
//#if TT_DEBUG_ON
   //::MessageBoxW(NULL,xml,L"Whole Document",MB_OK);
   //VARIANT_BOOL bIsSuccessful;
   //document->loadXML(xml,&bIsSuccessful);
   //document->get_xml(&xml);
   //::MessageBoxW(NULL,xml,L"After Load",MB_OK);
   //xml_element *element;
   //document->get_documentElement(&element);
   //element->get_xml(&xml);
   //::MessageBoxW(NULL,xml,L"Document Element",MB_OK);
//#endif
   int xml_string_length = ::SysStringLen(xml); // much faster than wide_string_length
   int xml_indented_string_length;
	// following commented out on 150404 since it breaks on Win98/Me
	// and all this accomplished was making it easier to view in IE the XML with non-ASCII characters
	// can still save as Unicode and then view it/edit it
	// see http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dnxml/html/xmlencodings.asp
	//wide_string extra_processing_instructions = NULL;
	//if (tt_language != JAPANESE) { // return sometime what to do about Japanese here -- use Shift JIS?
	//	extra_processing_instructions = L" encoding=\"ISO-8859-1\"";
	//};
   wide_string xml_indented;
	if (xml_string_length > 5000000) {
		xml_indented = xml; // new on 220903 to not indent if so huge since xml_indent will allocate over 100MB to indent it - not worth it 
		xml_indented_string_length = xml_string_length;
	} else {
		xml_indented = xml_indent(xml,xml_string_length,1,80,xml_indented_string_length,NULL); // extra_processing_instructions); // 1 for each level, 80 is maximum width
		::SysFreeString(xml);
	};
   int xml_byte_length = multi_byte_length(xml_indented,xml_indented_string_length);
	HGLOBAL text_handle = global_alloc(GHND,xml_byte_length+1); // +1 for terminating NULL -- moved here from the line above on 040803 - how did things work earlier?
	if (text_handle == NULL) return(NULL); // warn??
	string text = (string) GlobalLock(text_handle);
   copy_narrow_string(xml_indented,xml_indented_string_length,text,xml_byte_length); // for Windows NT, 2000, and XP could put on clipboard as UNICODE instead
   if (xml_string_length > 5000000) {
		::SysFreeString(xml);
	} else {
		delete [] xml_indented;
	};
   document->Release();
   GlobalUnlock(text_handle);
   return(text_handle);
};

boolean Entity::handle_xml_tag(Tag tag, xml_node *node) {
   if (tag == NO_MORE_TAGS) {
      return(TRUE); 
   } else {
      return(FALSE); // extra_xml to collect? -- 051102
   };
};

#endif

//typedef bytes *image_array;
//bytes *cached_resources = NULL;
//long *cache_last_used = NULL;

Sprite::Sprite(SpriteCode appearance_code,
					city_coordinate x, city_coordinate y,
//					Background *floor,
					SpriteType col_type,
					short int initial_parameter,
					city_scale scale,
					city_coordinate given_width,
					city_coordinate given_height,
					boolean true_size) :
//					boolean function_of_lly_flag) :
	// adjusted to llx and lly once the offset of the image is known
//	width(given_width),
//	height(given_height),
	// can't win here
	// if I set these below after update_display since offsets may alter this
	// then much else breaks
	llx(x),
	lly(y), 
	floor(NULL),
	sprite_type(col_type),
//	priority_function_of_lly_flag((flag) function_of_lly_flag),
	current_priority(0),
	floor_priority(min_long), // means not yet set
//	notify_if_colliding_flag((flag) notify),
//	can_collide_flag((flag) able_to_collide),
//	motion_extends_collision_region((flag) motion_extends_flag),
//	collision_region_equals_region((flag) equal_regions_flag),
	screen_updates_flag(TRUE),
	infinite_stack_flag(FALSE),
	followers_completely_inside_flag(TRUE),
	completely_inside_leader_flag(TRUE),
	followers_absorbed_flag(FALSE),
//	leaders_removed_flag(FALSE),
//	update_display_in_progress(FALSE),
//   display_updated(FALSE),
	clean_status(DIRTY),
	cache_status(NOT_CACHED),
//	followers_cached_flag(FALSE),
//   ok_to_cache_after(tt_wait_before_caching),
	clean_run(0),
	clean_run_average(10), 
	display_cost(50), // clobbered before used anyway...
#if TT_DEBUG_ON||TT_CAREFUL
	deleted_flag(FALSE),
#endif
	changes_propagated_flag(FALSE),
	inside_vacuum_flag(FALSE),
	available_for_indexing_flag(TRUE),
	slated_for_deletion_flag(FALSE),
   is_picture_flag(FALSE),
	graspable_flag((flag) (col_type < LAST_MANIPULABLE)),
   selectable_flag((flag) (col_type < LAST_MANIPULABLE)),
//	game_element(FALSE),
//	collision_region_width(collision_width),
//	collision_region_height(collision_height),
//	collision_x_offset(col_x_offset),
//	collision_y_offset(col_y_offset),
//	visible_flag(TRUE),
	animations(NULL),
   new_animations(NULL),
//	animation_status_flag(FALSE),
//	screen_ref_count(FALSE), // for now just a single bit
//	region(NULL),
	followers(NULL),
	leader_offsets(NULL),
	leader(NULL), //	Sprite(center_x,center_y,1,1,col_type),
	current_image(NULL),
	current_scale(scale),
	current_index(0),
//	old_cycle_index(-1), // compute new one
//	cycle_index(-1), // compute new one
	old_x_offset(0),
	old_y_offset(0),
	ideal_speed(0),
	parameter(-1), // it is reset below but this causes full initialization
//	original_parameter(initial_parameter),
//	animate_parameter_time_end(0),
//	animate_parameter_time_start(0),
//	goal_delta_parameter(0),
	code(appearance_code),
//	clipping_flag(TRUE),
	true_size_flag((flag) true_size),
//	transparency_on_flag(TRUE),
	size_changed_flag(TRUE),
//	followers_on_top_flag(TRUE),
	cycle_stopped_callback(CALLBACK_NOTHING),
	x_scale(256), // 256 means 1.0
	y_scale(256),
	running_animations(FALSE),
	finish_animation_pending(FALSE),
	in_notebook_flag(FALSE),
	held_status(NOT_HELD),
	to_be_smashed_flag(FALSE),
	blank(FALSE),	 
	visible_flag(TRUE),
	show_all_flag(TRUE),
	prepared_for_deletion_flag(FALSE),
   never_cache_flag(FALSE),
   containable_by_background_flag(TRUE),
	move_between_floor_and_room_flag(TRUE),
   priority_fixed_flag(FALSE),
   ok_to_dump_flag(TRUE),
   ref_count(1), // this isn't always all references only "ones that matter"
   finishing_instantly_flag(FALSE),
	comment(NULL),
	active_flag(TRUE),
//   activatable_flag(TRUE), // new on 160702
//#if TT_DIRECT_PLAY
//	being_marshalled_flag(FALSE),
//#endif
//	adjust_height_if_camera_above_flag(FALSE), // experiment 230804 (TRUE),
	scale_for_height_of_camera_flag(TRUE),
	top_of_stack(NULL),
	is_top_of_a_stack_flag(FALSE),
   cycle_start_time(0), // not initialized prior to 051102 -- was -1 prior to 240803 but that wasn't used and confused log comparison for debugging dmo files
   looped_back_already(FALSE), // not initialized prior to 051102
#if TT_XML
   extra_xml(NULL),
   extra_xml_count(0),
//	recent_xml_location(0), // noticed is obsolete on 080506
#endif
   cause_of_match_failure_flag(FALSE),
   saved_by(NULL), // added 280302
   guid(NULL),// added 010802 - restored 140703
	preparing_for_deletion(FALSE), // new on 060204
	releasing_all_references(FALSE) { 
//	 size_porportional_to_screen_size_flag(FALSE) {
//  selected_flag(FALSE) {
//	previous_width = the_width;
// previous_height = the_height;
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
//	if (tt_debug_mode == 100) {
		tt_counter_for_debugging++;
		debug_counter = tt_counter_for_debugging;
//		if (debug_counter == debug_index) {
//			log(_T("Creating debugging target."));
//		};
      if (debug_counter == tt_debug_target) {
			debug_log(debug_counter);
         tt_error_file() << "Creating " << type_name(this) << "#" << debug_counter 
  								 << " on frame " << tt_frame_number << " at " << timeGetTime() << endl;
      } else if (tt_debug_mode == 1717) { // replicated so can put breakpoint above
         tt_error_file() << "Creating " << type_name(this) << "#" << debug_counter 
								 << " on frame " << tt_frame_number << " at " << timeGetTime() << endl;
      };
//	};
#endif
//	if (code == SYNTHETIC_SPRITE) {
		// best place for this??
//		true_size_flag = TRUE;
//	};
   if (code == USER_SPRITE) {
//      current_image = new UserImage(); // commented out 201199 since now there is an attempt to share user images
		// had been restored since some current_image is needed but rather than create and 
		// then clobber share a default starting 251000
		current_image = tt_default_user_image;
		set_parameter(0);
   } else {
	   width = compute_sprite_width(given_width,given_height,appearance_code);
	   height = compute_sprite_height(given_width,given_height,appearance_code);
//	   priority_function_of_lly_flag = (flag) priority_function_of_lly_flags[code];
		if (code != SYNTHETIC_SPRITE && initial_parameter >= parameter_counts[code]) { // added on 020299 for robustness
			tt_error_file() << "Creating something whose picture number was too big." << endl;
			initial_parameter = 0;
		};
		// added tt_dump_as_xml since the following optimization seems to break old format code
		// not clear if tt_dump_as_xml was needed - asked 130803
	   set_parameter(initial_parameter,TRUE,FALSE); // tt_loading_a_city&&tt_dump_as_xml); 
		// set_paramter now for the following to work properly -- 
		// extra args on 100803 to avoid extra work while loading - removed 250803 for time travel replay accurancy
		if (code != SYNTHETIC_SPRITE) { // conditionalized on 040899
			if (!tt_loading_a_city) { 
				// tt_loading_a_city condition added on 100803 since this is both wasted effort if volitale attributes have been saved 
				// and also update_display may change current_index and be confused as to what the current time is 
				update_display(0,0,UPDATE_INITIALIZE); // complete the initialization
				clear_changes();
			} else {
				current_image = image_cycles[code][parameter].nth_image(current_index);
				// new on 240803 instead of doing this on demand in pointer_to_current_image
				// this should make time travel more accurate
			};
		};
   };
//   current_cycle = &image_cycles[code][0]; // start with first one
//	initial_position(center_x,center_y);
//	if (!priority_fixed()) update_priority();
//  scale_to_fit(width,height);
   // following is reasonable but requires lots of re-writing
   // since some code relies on floor with floor knowing about sprite
   // also just ignoring arg and "setting" floor to NULL breaks
   // replay of logs
//   if (floor != NULL) {
//      floor->add_item(this);
//   };
//	set_visible(TRUE); // but don't clear visible flag
};

void Sprite::update_priority() {
   // only called if not clean and screen's view is from side and priority_fixed_flag is FALSE
   if (show_all() || !is_picture() || followers == NULL) {
		if (graspable() || kind_of() != PICTURE) { // second disjunct added on 300105 since only glued down pictures need this fix
			set_priority(lly);
		} else {
			set_priority(max_long); // since glued down and shouldn't obscure others -- new on 260105
		};
   } else { // lowest lly of followers
      long best_so_far = max_long;
      Sprites *remaining = followers;
      while (remaining != NULL) {
         Sprite *follower = remaining->first();
         follower->update_priority();
         if (follower->visible() && follower->priority() < best_so_far) {
            best_so_far = remaining->first()->priority();
         };
         remaining = remaining->rest();
      };
      if (followers_completely_inside()) { // this is only for pictures and their followers -- can it ever be otherwise?
         set_priority(max(best_so_far,lly)); // might be clipped off the bottom so
      } else {
         set_priority(best_so_far);
      };
   };
};

city_coordinate compute_sprite_width(city_coordinate width_specified,
												 city_coordinate height_specified,
												 SpriteCode code) {
	if (width_specified == 0) {
		if (height_specified == 0) {
			return(maximum_width(code));
		} else { // maintain original aspect ratio
			return((maximum_width(code)*height_specified)/maximum_height(code));
		};
	} else {
		return(width_specified);
	};
};

city_coordinate compute_sprite_height(city_coordinate width_specified,
												  city_coordinate height_specified,
                                      SpriteCode code) {
	if (height_specified == 0) {
      if (width_specified == 0) {
         return(maximum_height(code));
		} else { // maintain original aspect ratio
         return((maximum_height(code)*width_specified)/maximum_width(code));
		};
	} else {
		return(height_specified);
	};
};

int compute_units_per_horizontal_pixel() { // abstracted on 300301
	if (tt_screen_width <= tt_graphics_video_mode_width) {
		return(tt_ideal_units_per_horizontal_pixel);
	} else if (tt_graphics_video_mode_width == 320) {
		return(100);
	} else { // new on 170999 since graphics file is still based on 640x480
		return(50);
	};
};

int compute_units_per_vertical_pixel() {
	if (tt_screen_height <= tt_graphics_video_mode_height) {
		return(tt_ideal_units_per_vertical_pixel);
	} else if (tt_graphics_video_mode_height == 240) {
		return(100);
	} else {
		return(50);
	};
};

boolean load_sprite_file_from_stream(input_stream &file, SpriteCode code) { // made a procedure on 021001
	char temp_string[128]; // no file name is longer than 128
	file >> temp_string;
	if (strcmp(temp_string,"SpriteVersion") != 0) {
//		file.close();
		tt_error_file() << S(IDS_IMPROPER_SPRITE_FILE) << code << endl;
//		delete [] buffer; // commented out on 021001 since used by multiple callers now
		tt_exit_failure(TROUBLE_SHOOT_BAD_INSTALLATION,S(IDS_BAD_DAT_FILE),S(IDS_NEED_TO_REINSTALL));
      return(FALSE);
	};
	int version;
	file >> version;
	if (version != 1) {
//#if !TT_WINDOWS
//		file.close();
//#endif
		tt_error_file() << S(IDS_IMPROPER_SPRITE_VERSION)
							 << code << S(IDS_VERSION_IS) << version << endl;
//		delete [] buffer; // commented out on 021001 since used by multiple callers now
		tt_exit_failure(TROUBLE_SHOOT_BAD_INSTALLATION,S(IDS_BAD_DAT_FILE),S(IDS_NEED_TO_REINSTALL));
      return(FALSE);
	};
	boolean repeat_flag, function_of_distance, priority_function_of_lly;
	file >> repeat_flag
		  >> function_of_distance
		  >> priority_function_of_lly;
	priority_function_of_lly_flags[code] = (flag) priority_function_of_lly;
#if SOUND_CAPABLE		  
	file >> sound_modes[code];
	if (sound_modes[code]) {
#else
	int ignore;
	file >> ignore;
	if (ignore) {
#endif
		int sound_frequency; // ignored now (WORX doesn't use it)
		char sound_file_name[128];
		file >> sound_file_name >> sound_frequency;
#if SOUND_CAPABLE
		sound_file_names[code] = copy_string(sound_file_name);
#endif
	};
	int file_count;
	file >> file_count;
   image_id *image_ids = NULL;
   int *resource_indices = NULL;
   boolean *rectangular_mask_flags = NULL;
   if (file_count > 0) { // 0 file count is used for special sprites like USER_SPRITE
//	   strings image_file_names = new string[file_count];
	   image_ids = new image_id[file_count];
	   resource_indices = new int[file_count];
	   rectangular_mask_flags = new boolean[file_count];
	   for (int i = 0; i < file_count; i++) {
		   file >> temp_string
			     >> resource_indices[i]
			     >> rectangular_mask_flags[i];
         // following names are only used by Java translator and when BackgroundsDir or BuiltinPictureDir is set in toontalk.ini
		   if (tt_image_file_names[resource_indices[i]] == NULL) {
            tt_image_file_names[resource_indices[i]] = copy_ascii_string(temp_string);
         };
      };
	};
	file >> parameter_counts[code];
	if (parameter_counts[code] < 1) {
		tt_error_file() << S(IDS_IMPROPER_SPRITE_VERSION) << code << endl;
//		delete [] buffer; // commented out on 021001 since used by multiple callers now
		tt_exit_failure(TROUBLE_SHOOT_BAD_INSTALLATION,S(IDS_BAD_DAT_FILE),S(IDS_NEED_TO_REINSTALL));
      return(FALSE);
	};
	if (code == HIT_OR_MISS_SPRITE) {
		parameter_counts[code] = 2; // ignore the third one
	} else if (code == BALL_SPRITE || code == PADDLE_SPRITE) {
		parameter_counts[code] = 1; // new on 150704 to fix this
	};
	short int image_count, duration;
	boolean mirror_flag;
//   cycle_durations[code] = 0;
//	int cycle_duration = 0;
	max_widths[code] = 0;
	max_heights[code] = 0;
	dimension width, height, x_offset, y_offset;
	long image_offset;
	long image_size;
	short int file_name_index, compression_method;
//	int *codes_ideal_parameters = new int[parameter_counts[code]];
//	int *codes_loop_back_indices = new int[parameter_counts[code]];
//	ideal_parameters[code] = codes_ideal_parameters;
//	loop_back_indices[code] = codes_loop_back_indices;
	Image_cycle *codes_image_cycles = new Image_cycle[parameter_counts[code]];
	image_cycles[code] = codes_image_cycles;
	short int ignore_ideal_parameters;
	for (int i = 0; i < parameter_counts[code]; i++) {
		file >> ignore_ideal_parameters; //codes_ideal_parameters[i];
		file >> image_count;
//		codes_loop_back_indices[i] = repeat_flag?0:image_count-1;
		short int loop_back_index = (short int) (repeat_flag?0:image_count-1);
		if (image_count <= 0) {
			tt_error_file() << S(IDS_VERSION_IS) << code << endl;
//			delete [] buffer; // commented out on 021001 since used by multiple callers now
			tt_exit_failure(TROUBLE_SHOOT_BAD_INSTALLATION,S(IDS_BAD_DAT_FILE),S(IDS_NEED_TO_REINSTALL));
         return(FALSE);
		};
		TTImagePointer *images = new TTImagePointer[image_count]; // added extra level of indirection on 231001 as well
		if (tt_builtin_picture_directory == NULL) { // new on 231001
			for (int i = 0; i < image_count; i++) {
				images[i] = new TTImage();
			};
		} else {
			for (int i = 0; i < image_count; i++) {
				images[i] = new UserImage();
			};
		};
//		if (tt_builtin_picture_directory == NULL) { // rewritten on 231001
//			images = new TTImagePointer[image_count];
//		} else {
//			images = new UserImagePointer[image_count];
//		};
		short int *durations = new short int[image_count];
		for (int j = 0; j < image_count; j++) {
			file >> temp_string;
			if (j == 0 && strcmp(temp_string,"BMP") != 0) {
				// should be optional loop back index
				loop_back_index = (short int) atoi(temp_string);
				file >> temp_string;
			};
			if (strcmp(temp_string,"BMP") == 0) {			
				file >> mirror_flag // ignored now
					  >> duration
					  >> width >> height
					  >> x_offset >> y_offset
					  >> image_offset >> image_size >> compression_method
					  >> file_name_index;
				if (!file) {
					tt_error_file() << S(IDS_BAD_IMAGE_COUNT) << code << endl;
//					delete [] buffer; // commented out on 021001 since used by multiple callers now
					tt_exit_failure(TROUBLE_SHOOT_BAD_INSTALLATION,S(IDS_BAD_DAT_FILE),S(IDS_NEED_TO_REINSTALL));
				};
//				cycle_duration += duration;
				int units_per_horizontal_pixel = compute_units_per_horizontal_pixel();
				int units_per_vertical_pixel = compute_units_per_vertical_pixel();
				// following updated on 241199
				city_coordinate original_width = ideal_horizontal_units(width); //(city_coordinate) width*tt_ideal_units_per_horizontal_pixel;
				city_coordinate original_height = ideal_vertical_units(height); // (city_coordinate) height*tt_ideal_units_per_vertical_pixel;
				max_widths[code] = max(max_widths[code],width*units_per_horizontal_pixel);
				max_heights[code] = max(max_heights[code],height*units_per_vertical_pixel);
#if TT_DEBUG_ON
				if (tt_debug_mode == 300301 && code == 48) {
					log("check out max values");
				};
#endif
//				string image_file_name =
//				  new char[strlen(image_file_names[file_name_index])+1];
//				strcpy(image_file_name,image_file_names[file_name_index]);
				if (code == TOOLBOX_ABOVE_SPRITE || code == CUBBY_SPRITE ||
					 code == NUMBER_PAD_SPRITE || (code == WAND_BUTTON_SPRITE && tt_log_version_number > 50)) {// || code == SCALE_SPRITE) {
					// want lower left corner to be stable
					// add this to tts file later
					// added WAND_BUTTON_SPRITE on 160804
					x_offset = 0;
					y_offset = 0;
				};
				images[j]->initialize(//image_file_name,
//                               i*parameter_counts[code]+j,
//											mirror_flag,
											width,height,
											// updated the following to be more accurate
											-ideal_horizontal_units(x_offset), // -x_offset*tt_ideal_units_per_horizontal_pixel,
											-ideal_vertical_units(y_offset), // -y_offset*tt_ideal_units_per_vertical_pixel,
											original_width, original_height,
//											TRUE, // for testing
											rectangular_mask_flags[file_name_index],
											image_size,
#if USING_RC_LIBRARIES
											image_offset,
#endif
											compression_method&&(tt_builtin_picture_directory==NULL), // added second conjunct on 121001
											code,
											resource_indices[file_name_index],
#if !TT_WINDOWS // || GRAPHICS_TO_MEMORY
											image_ids[file_name_index]);
#else
											0); // ignored anyway
#endif
#if TT_DEBUG_ON
				if (tt_debug_mode == 50505 && tt_image_file_names[resource_indices[file_name_index]] != NULL) { 
					// 050505 -- for generating files for beta (and trial) versions
					if (images[j]->rectangular()) {
						tt_error_file() << "alchemy -jh100 " << tt_image_file_names[resource_indices[file_name_index]] << ".png jpgs\\" << endl;
					} else {
						tt_error_file() << "alchemy -g1 ---t 0 0 0 -8 " << tt_image_file_names[resource_indices[file_name_index]] << ".png gifs\\"
											 << endl;
					};
				};
#endif
				// special case a few hat cycles since brim would be clipped otherwise  
				if (code == PERSON_WALKING_SPRITE) {
					//if (j == 1 || j == 2) {
					//	if (i == EAST) {
					//		extra_walking_bald_images[j-1] = images[j];
					//	} else if (i == WEST) {
					//		extra_walking_bald_images[j+1] = images[j];
					//	};						
					//} else					
					if (i == 8) { // extra stuff cycle
						saved_heads_is_for_hat = (tt_programmer_head != 2);
						TTImage *image; 
						switch(j) {
							case 0: // PERSON_WITH_HAT_EAST_IMAGE2 
								image = codes_image_cycles[EAST].nth_image(1);
								break;
							case 1:
								image = codes_image_cycles[EAST].nth_image(2);
								break;
							case 2: // PERSON_WITH_HAT_WEST_IMAGE2 
								image = codes_image_cycles[WEST].nth_image(1);
								break;
							case 3:
								image = codes_image_cycles[WEST].nth_image(2);
								break;
						};
						if (tt_programmer_head == 2) {
//							delete saved_heads[j]; // new on 160103
//							saved_heads[j] = image; // was *image prior to 231001
							*(saved_heads[j]) = *image; // new on 190803 so isn't clobbered by initialization below
						} else {
							image = saved_heads[j]; // saved hat version next -- was &saved_heads[j] prior to 231001
						};
//					delete extra_heads[j]; // no need to put something there just to destroy it
//						extra_walking_hat_images[j] = *image;
//					if (tt_programmer_head == 2) {
////						delete saved_heads[j]; // new on 160103 -- commented out on 180603 since shared with extra_heads now
//						saved_heads[j] = image; // was *image prior to 231001
//					} else {
//						image = saved_heads[j]; // saved hat version next -- was &saved_heads[j] prior to 231001
//					};
						parameter_counts[PERSON_WALKING_SPRITE] = 8; //9th one a hack
						image->initialize(//image_file_name,
															width,height,
															-x_offset*units_per_horizontal_pixel,
															-y_offset*units_per_vertical_pixel,
															original_width, original_height,
															rectangular_mask_flags[file_name_index],
															image_size,
															compression_method,
															code,
															resource_indices[file_name_index],
#if !TT_WINDOWS // || GRAPHICS_TO_MEMORY
															image_ids[file_name_index]);
#else
															0); // ignored anyway
#endif
					// don't really use the "originals" here
					// this avoids sharing of the string
					// initialize takes care of this now
//					images[j].set_file_name(NULL);
					};
				};			
				durations[j] = duration;
			} else {
#if !TT_WINDOWS
				file.close();
#endif
				tt_error_file() << S(IDS_SPRITE_FILE_BAD_FORMAT)
								<< temp_string << " " << code << endl;
//				delete [] buffer; // commented out on 021001 since used by multiple callers now
				tt_exit_failure(TROUBLE_SHOOT_BAD_INSTALLATION,S(IDS_BAD_DAT_FILE),S(IDS_NEED_TO_REINSTALL));
            return(FALSE);
			};
		};
		codes_image_cycles[i].initialize(image_count,images,durations,
//													cycle_duration,
													loop_back_index,
													function_of_distance);
//		cycle_duration = 0;
	};
//	file.close();
//	delete [] full_file_name;
//	for (i = 0; i < file_count; i++) {
//		delete [] image_file_names[i];
//	};
//	delete [] image_file_names;
	if (image_ids != NULL) delete [] image_ids;
	if (resource_indices != NULL) delete [] resource_indices;
	if (rectangular_mask_flags != NULL) delete [] rectangular_mask_flags;
//	delete [] buffer; // commented out on 021001 since used by multiple callers now
//	sprite_file_names[code] = NULL; // delete old contents?
   return(TRUE); // no problems
};

boolean load_sprite_file(SpriteCode code) {
	if (tt_builtin_picture_directory != NULL) { 
		// && code < number_of_sprites) { // new on 021001 -- code < numberofsprites to deal with roomdoor and maybe others
		string tts_file_name = append_3_strings(tt_builtin_picture_directory,sprite_file_names[code],".tts");
		input_file_stream file(tts_file_name,std::ios_base::in); // |std::ios_base::nocreate);
		delete [] tts_file_name; // new on 160103
		if (file != NULL) {
			boolean result = load_sprite_file_from_stream(file,code);
			file.close();
			return(result);
		};
	};
	int size = (int) (sprite_offsets[code+1]-sprite_offsets[code]);
//	unsigned char *buffer = new unsigned char[10000];
	unsigned char buffer[10000]; // rewritten on 160103 to fix leak
//	bytes buffer = new unsigned char[size];
	_llseek(images_file,sprite_offsets[code],0);
//	images_file.seekg(sprite_offsets[code]);
	if (_lread(images_file,buffer,size) == HFILE_ERROR) {
      tt_error_file() << S(IDS_IMPROPER_SPRITE_FILE) << code << endl;
//		delete [] buffer;
		tt_exit_failure(TROUBLE_SHOOT_BAD_INSTALLATION,S(IDS_BAD_DAT_FILE),S(IDS_NEED_TO_REINSTALL));
      return(FALSE);
	};
//	images_file.read(buffer,size);
	// there probably is a better way like use a file pointer and switch here
	input_string_stream file((char *) buffer,10000);
	return(load_sprite_file_from_stream(file,code));
};

//	string file_name = sprite_file_names[code];
//	int file_name_length = strlen(tt_sprite_dir)+strlen(file_name)+5;
//	string full_file_name = new char[file_name_length];
//	strcpy(full_file_name,tt_sprite_dir);
//	strcat(full_file_name,file_name);
//	strcat(full_file_name,".tts");
//	input_file_stream file;
//	file.open(full_file_name, std::ios_base::in);
//	if (!file) {
//		tt_error_file() << "Couldn't open: " << full_file_name << endl;
//		tt_exit_failure();
//	};

// hack due to hat having different clipping region...

void swap_heads(int new_programmer_head) {
	if (image_cycles == NULL) return; // too early - added on 180603
	Image_cycle *codes_image_cycles = image_cycles[PERSON_WALKING_SPRITE];
	TTImage *temp = codes_image_cycles[EAST].nth_image(1);
	// the following works but is much more complicated than need be to work around a bug save_nth_image fixed on 190603
	string file_name = temp->return_file_name();
	if (file_name == NULL) {
		temp->load_image(TRUE);
		file_name = temp->return_file_name();
	};
	if (file_name != NULL) {
		if ((new_programmer_head == 2 && file_name[1] == 'h') || (new_programmer_head != 2 && file_name[1] != 'h')) {
			// this is kind of a hack but it avoids swapping when not needed
			return; // experiment 190603
		};
	} else {
		// shouldn't happen
		if ((new_programmer_head == 2) != saved_heads_is_for_hat) {
			return; // should be OK already -- bald and hair are equivalent here
		};
	};
	for (int i = 0; i < 4; i++) {
		temp = saved_heads[i];
		//if (new_programmer_head == 2) {
		//	temp = extra_walking_hat_images[i];
		//} else {
		//	temp = extra_walking_bald_images[i];
		//};
		temp->clear_private_data(); // caching info shouldn't be transfered
		// the following saves the current image in saved heads and uses the prior pointer to saved_heads as the new image for these walking cycles
		switch(i) {
			case 0: // PERSON_WITH_HAT_EAST_IMAGE2 
				codes_image_cycles[EAST].save_nth_image(1,saved_heads[i]);
				codes_image_cycles[EAST].set_nth_image(1,temp);
				break;
			case 1:
				codes_image_cycles[EAST].save_nth_image(2,saved_heads[i]);
				codes_image_cycles[EAST].set_nth_image(2,temp);
				break;
			case 2: // PERSON_WITH_HAT_WEST_IMAGE2 
				codes_image_cycles[WEST].save_nth_image(1,saved_heads[i]);
				codes_image_cycles[WEST].set_nth_image(1,temp);
				break;
			case 3:
				codes_image_cycles[WEST].save_nth_image(2,saved_heads[i]);
				codes_image_cycles[WEST].set_nth_image(2,temp);
				break;
		};
	};
	saved_heads_is_for_hat = !saved_heads_is_for_hat; // don't care if hair or bald
};

const int background_count = 15;
BackgroundImage *backgrounds = NULL;

void initialize_background_sprites() {
   backgrounds = new BackgroundImage[background_count];
   const int indices[background_count]
                = {FLOOR1_BACKGROUND,
                   ROOM_A_BACKGROUND,ROOM_B_BACKGROUND,ROOM_C_BACKGROUND,
                   THOUGHT_BUBBLE_BACKGROUND,PUZZLE_SINKING_BACKGROUND,
                   PUZZLE_CRASH_BACKGROUND,PUZZLE_HURT_BACKGROUND,
                   PUZZLE_RESCUE_BACKGROUND,ROCKET_INSIDES_BACKGROUND,
                   OPENING_TITLE_BACKGROUND,
                   FLOOR2_BACKGROUND,FLOOR3_BACKGROUND,
                   PUBLISHER_LOGO_BACKGROUND,
                   FLOOR4_BACKGROUND};
	int background_width, background_height, background_bytes_size;
	if (tt_video_mode > tt_graphics_video_mode_width) { // new on 150999
		// don't have high resolution background images so stretch them
		background_width = tt_graphics_video_mode_width; // updated on 140300 so this works even with m22 graphics
		background_height = tt_graphics_video_mode_height;
		background_bytes_size = background_width*background_height;
	} else {
		background_width = tt_screen_width;
		background_height = tt_screen_height;
		background_bytes_size = tt_screen_bytes_size;
	};
	string directory = process_directory_entry(ini_entry(AC(IDC_DIRECTORIES),"BackgroundsDir",FALSE)); 
	// added process_directory_entry on 021001
	if (directory == NULL && tt_builtin_picture_directory != NULL) { // new on 010302 so need only set BuiltinPictureDir
		directory = copy_string(tt_builtin_picture_directory);
	};
	for (int i = 0; i < background_count; i++) {
#if TT_NO_ROBOT_TRAINING
		if (indices[i] != PUZZLE_SINKING_BACKGROUND && 
          indices[i] != PUZZLE_CRASH_BACKGROUND &&
			 indices[i] != PUZZLE_HURT_BACKGROUND &&
          indices[i] != PUZZLE_RESCUE_BACKGROUND)
#endif
		{
		string file_name;
		// commented out the following since is waste of time unless this feature is enabled via toontalk.ini
		// this should also be resolution dependent since it looks bad otherwise
		if (directory == NULL) {
			file_name = NULL;
//			FileNameStatus status;
//			file_name = existing_file_name(status,AC(IDC_BACKGROUND_FILE_NAME0+i),"bmp","Backgrounds",FALSE,FALSE);
		} else {
//			file_name = append_3_strings(directory,AC(IDC_BACKGROUND_FILE_NAME0+i),".bmp");
//			file_name = append_strings(directory,AC(IDC_BACKGROUND_FILE_NAME0+i)); 
			// rewritten on 240901 since extension can be default extension
			// rewritten on 121001 since these are like builtins
			file_name = new char[MAX_PATH];
//			output_string_stream name(file_name,MAX_PATH); // rewritten 260104
//			name << directory << AC(IDC_BACKGROUND_FILE_NAME0+i);
			strcpy(file_name,directory);
			strcat(file_name,AC(IDC_BACKGROUND_FILE_NAME0+i));
			if (tt_missing_builtin_picture_file_extension != NULL) {
//				name << "." << tt_missing_builtin_picture_file_extension;
				strcat(file_name,".");
				strcat(file_name,tt_missing_builtin_picture_file_extension);
				if (!file_exists(file_name,FALSE)) {
					// new on 051004 to look for other picture extensions
					int extension_start = strlen(file_name)-strlen(tt_missing_builtin_picture_file_extension); 
					const int picture_extensions_count = 7;
					string picture_extenions[picture_extensions_count] = {"bmp","png","jpg","gif","dib","jpeg","ini"};
					int extension_index = 0;
					while (!file_exists(file_name,FALSE) && extension_index < picture_extensions_count) {
						strcpy(file_name+extension_start,picture_extenions[extension_index]);
						extension_index++;
					};
				};
			};
//			name.put('\0');
		};
		if (file_name == NULL || !backgrounds[i].set_file_name(file_name,FALSE)) {
			backgrounds[i].initialize(background_width,background_height,
											  0,0,
											  ideal_screen_width,ideal_screen_height,
											  TRUE, // rectangular of course
											  background_bytes_size,
											  BI_RGB, // why no compression?
											  BACKGROUND_SPRITE,
											  indices[i],
											  0);
		};
		if (file_name != NULL) {
			delete [] file_name;
		};
		};
	};
	if (directory != NULL) delete [] directory;
};

BackgroundImage *background(int index) {
   switch (index) {
      case FLOOR1_BACKGROUND:
         return(&backgrounds[0]);
      case ROOM_A_BACKGROUND:
         return(&backgrounds[1]);
      case ROOM_B_BACKGROUND:
         return(&backgrounds[2]);
      case ROOM_C_BACKGROUND:
         return(&backgrounds[3]);
      case THOUGHT_BUBBLE_BACKGROUND:
         return(&backgrounds[4]);
      case PUZZLE_SINKING_BACKGROUND:
         return(&backgrounds[5]);
      case PUZZLE_CRASH_BACKGROUND:
         return(&backgrounds[6]);
      case PUZZLE_HURT_BACKGROUND:
         return(&backgrounds[7]);
      case PUZZLE_RESCUE_BACKGROUND:
         return(&backgrounds[8]);
      case ROCKET_INSIDES_BACKGROUND:
         return(&backgrounds[9]);
      case OPENING_TITLE_BACKGROUND:
         return(&backgrounds[10]);
      case FLOOR2_BACKGROUND:
         return(&backgrounds[11]);
      case FLOOR3_BACKGROUND:
         return(&backgrounds[12]);
      case PUBLISHER_LOGO_BACKGROUND:
         return(&backgrounds[13]);
      case FLOOR4_BACKGROUND:
         return(&backgrounds[14]);
      default:
		   tt_error_file() << S(IDS_BACKGROUND_INDEX_TOO_BIG) << index << endl;
         return(&backgrounds[0]);
	};
};

/*
string sprite_file_names[tt_appearance_count]
	 = {//"chairsid","tablesid",
		"toolboxs", "toolboxa",
		"manwalk",
		"wire",
		"heliofly",
		//"heliogon",
		"frame",
		"heliolnd",
		//"heliolev",
		"mansit",
//		"houseabv", "housesid", "doorsid", "lawnsid",
		"housebt","housebs", "doorb", "doora",
		"numpad", //"digits",
		"scale",
		"vacuum", "expander", "copier",
		"tbubble", "robot",
		"nest", "bird", //"parrot",
//		"truck",
		"bomb", 
		"notebook",
		"cubby",
//		"handarm",
		"mouseham", // to avoid conflict with Window's mouse.dll
		"arm",
		"hand",
		"handwand",
		"pictures",
		"hair",
		"hat",
		"platform",
		"switch",
		"mansitht",
		"mansithr",
		"houseas",
		"houseat",
		"housecs",
		"housect",
		"doorc",
		"alien",
		"trucki",
		"trucka",
		"trucks",
		"talkbal",
		"plate",
		"textpad",
		"notepage",
		"dizzy",
		"dizhair",
		"dizhat",
		"explode",
		"alientlk",
		"hitmiss",
		"ball",
		"paddle",
		"manwalk8"};
*/

void Sprite::recursively_update_display() {
   update_display();
   Sprites *remaining = followers;
   while (remaining != NULL) {
      remaining->first()->recursively_update_display();
      remaining = remaining->rest();
   };
};

void Sprite::update_display(city_coordinate delta_x, city_coordinate delta_y,
									 const UpdateStatus update_status) {
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	if (prepared_for_deletion()) return; // about to be deleted
#if TT_DEBUG_ON
	if (tt_debug_mode == 2529) {
		if (floor != NULL && kind_of() != PROGRAMMER && !is_picture()) { 
			// && kind_of() != MARTIAN && kind_of() != TALK_BALLOON) { // Marty is treated specially (bad idea?)
			if (!floor->has_item(ultimate_leader())) {
				log(_T("Item thinks it is part of a background, but background doesn't know it."),FALSE,TRUE);
			};
		};
	};
	if (debug_counter == tt_debug_target) {
		log(_T("Updating the display of debug target."));
		tt_error_file() << delta_x << "," << delta_y << " current_index=" << current_index << "; cycle_start=" << cycle_start_time << endl;
	};
	if (deleted()) { // e.g. tt_debug_mode == 666
		debug_this("Updating display of deleted sprite.");
	};
#endif
//	update_display_in_progress = TRUE;
//	if (!running_animations && tt_log_version_number < 15) {
		// on 090699 moved this to later so that the image etc has been updated before running animations
//		run_animations(tt_finish_all_animations,FALSE);
//	};
	city_coordinate x_offset, y_offset;
	boolean image_changed;
	if (code == SYNTHETIC_SPRITE) {
		image_changed = FALSE;
   } else if (code == USER_SPRITE) { 
      if (size_changed_flag) {
         scale_to_fit(); // only if size has changed ??
         set_clean_status(DIRTY);
         size_changed_flag = FALSE; // dealt with
      };
      image_changed = FALSE; // so later code doesn't do wrong stuff
	} else {
		// sprite stores these things as 1 bit flags
		boolean looped_back_already_boolean = looped_back_already;
		boolean single_step_boolean = single_step;
		if (parameter >= parameter_counts[code] || parameter < 0) { // added 050299 for robustness
			tt_error_file() << "Picture had wrong picture number. Reset to 0." << endl;
			parameter = 0;
		};
		TTImage *new_image = image_cycles[code][parameter].update_display(((update_status != NO_UPDATE_CHANGE) && active()), 
																								// added && active() on 181199
																								// deltas for matching ideal speed
																								delta_x,delta_y,
																								ideal_speed,
																								current_index,
																								cycle_start_time,
																								cycle_duration,
																								looped_back_already_boolean,
																								single_step_boolean,
																								x_offset,y_offset);
#if TT_DEBUG_ON
		add_to_log_if_interesting_error();
#endif
		//if (!looped_back_already && looped_back_already_boolean) { // just looped back -- 050204 
		//	update_cycle_duration(image_cycles[code][parameter].return_loop_back_index());
		//} else if (looped_back_already && !looped_back_already_boolean) { // just stopped looping back
		//	update_cycle_duration();
		//};
		looped_back_already = (flag) looped_back_already_boolean; 
		single_step = (flag) single_step_boolean;
		// following shouldn't happen anymore (also 240803)
		//if (current_image == NULL) { 
		// new on 240803 -- should this also check if replaying a log or time travel or something? OK if loaded from XML?
		//	x_offset = 0; // ignore change from initial state
		//	y_offset = 0;
		//};
		image_changed = (new_image != current_image || update_status == UPDATE_TO_FIT_ON_PAGE); 
		// && current_image != NULL); // second condition added on 240803 to make time travel more accurate
		// update_status == UPDATE_TO_FIT_ON_PAGE new on 071004 to get geometry right 
		// if loading city with a different screen resolution
		current_image = new_image;
	};
//	recompute_parameters();	
	if (clean_status == DIRTY && code != SYNTHETIC_SPRITE) {
		// added on 040899 code != SYNTHETIC_SPRITE
		scale_to_fit();
	};
	if (image_changed || (size_changed_flag && code != SYNTHETIC_SPRITE && code != USER_SPRITE)) {
#if TT_DEBUG_ON
		if (debug_counter == tt_debug_target) {
			log(_T("Debug target x and y offsets being re-computed."));
			tt_error_file() << "Current index is now " << current_index << " where offsets are " << x_offset << "," << y_offset 
								 << " and where deltas are " << delta_x << "," << delta_y << endl;
		};
#endif
		set_clean_status(DIRTY); // was clean_status = DIRTY;
		if (update_status == UPDATE_INITIALIZE) { 
			return;
		};
		size_changed_flag = FALSE; // taken care of here
		// offsets from center to llx may have changed
		if (current_scale != ground_scale) {
			// x_offset and y_offset are given in ground (ideal) units
			// so adjust them for the current scale
			x_offset = (x_offset*current_scale)/ground_scale;
			y_offset = (y_offset*current_scale)/ground_scale;
		};
//		x_offset = (x_offset*x_scale)/256;
//		y_offset = (y_offset*y_scale)/256;
		delta_x += ((x_offset-old_x_offset)*x_scale)/256;
		delta_y += ((y_offset-old_y_offset)*y_scale)/256;
		set_old_x_offset(x_offset);
		set_old_y_offset(y_offset);
      // commented out the following on 090502 since the original delta_x and delta_y shouldn't be scaled - 
		// just the offset difference
//		delta_x = (delta_x*x_scale)/256; // why not >>8?
//		delta_y = (delta_y*y_scale)/256; // why not >>8?
	};
	if (update_status != NO_UPDATE_CHANGE) { 
		// experiment that didn't provide stability - removed 220901 && update_status != NO_UPDATE_MOVE) {
		move_by(delta_x,delta_y,UPDATING);
	};
	if (!running_animations) { //  && tt_log_version_number > 14
		// on 090699 moved this to later so that the image etc has been updated before running animations
		run_animations(tt_finish_all_animations,FALSE);
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	if (update_status != UPDATE_CYCLE && !running_animations) {
		check_cycle_callback(FALSE);
	};
	if (clean_status != CLEAN) {
		if (tt_screen != NULL && tt_screen->camera_status() == CAMERA_IN_FRONT && !priority_fixed() &&
			 update_status != UPDATE_INITIALIZE) {
			update_priority();
		};
		if (clean_status == FOLLOWERS_DIRTY) {
			if (clean_run != 255) clean_run++;
		} else {
			zero_clean_run();
		};
	} else {
		if (clean_run != 255) clean_run++;
	};
//	propagate_changes();
//	display_updated = TRUE;
//	update_display_in_progress = FALSE;
};

void Sprite::check_cycle_callback(boolean run_regardless) {
	if (cycle_stopped_callback != NULL && 
		 (run_regardless || cycle_stopped() || tt_finish_all_animations)) {
		// use of tt_finish_all_animations assumes that it would have eventually stopped 
		AnimationCallBackProcedure good_callback = callback_procedure(cycle_stopped_callback);
		cycle_stopped_callback = CALLBACK_NOTHING; // just once
		good_callback(this);
	};
};

void Sprite::zero_clean_run() {
	if (clean_run != 0) {
		clean_run_average = (cost) ((((long) clean_run_average)+clean_run)/2);
	};
	clean_run = 0;
};

//void Sprite::recompute_parameters() {	
//	if (clean_status == DIRTY) {
//		scale_to_fit();
//	};
	// only need to compute following when size or image changes???
//	if (x_scale != 256) {
//      x_offset += (original_width-width)/2;
		// for screen to know how big a region to protect
//		pixel_width = ((long) pixel_width*x_scale)/256;
//	};
//	if (y_scale != 256) {
//      y_offset += (original_height-height)/2;
//		pixel_height = ((long) pixel_height*y_scale)/256;
//	};
//};

void Sprite::collision_region(TTRegion &region) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("collision_region of debug target");
	};
#endif
	if (collision_region_equals_region[code]) {
		true_region(region); // was full_region
	} else {
	  // add to the offset the amount from left to center the region
	  // note that unlike true_region this doesn't pay attention to scale
	  // offsets are defined in terms of width but
     // width of region function of true_width
	   region.min_x = (city_coordinate) (llx+(collision_x_offset_correction[code]*(int64) width)/100); // int64 new on 080202
	   region.max_x = (city_coordinate) (region.min_x+(true_width()*(int64) collision_width_factors[code])/100);
	   region.min_y = (city_coordinate) (lly+(collision_y_offset_correction[code]*(int64) height)/100);
	   region.max_y = (city_coordinate) (region.min_y+(true_height()*(int64) collision_height_factors[code])/100);
	};
};

/*
double Sprite::fraction_of_animation_done() {
	millisecond time_left
		= animate_parameter_time_end-tt_current_time;
	double fraction_done = 1.0-((double) time_left)
										 /(animate_parameter_time_end
											-animate_parameter_time_start);
	if (time_left <= 0) {
		fraction_done = 1.0;
		animate_parameter_time_end = 0; // ready for the next one
	};
	return(fraction_done);
};
*/

void Sprite::increment_parameter(short int increment) {
   if (code == SYNTHETIC_SPRITE) { // only change the lower-order byte
      short int new_parameter = parameter;
      unsigned char *bytes = (unsigned char *) &new_parameter;
      bytes[0] += (unsigned char) increment;
      set_parameter(new_parameter);
   } else {
      const short int count = (short int) parameter_counts[code];
	   set_parameter((short) ((parameter+increment+count)%count));
   };
};

boolean Sprite::set_parameter(short int new_parameter, boolean warn, boolean reset_cycle_regardless) {
	// removed boolean loading on 250803 since just_set_parameter is more appropiate when loading
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		tt_error_file() << "Setting parameter of debug target to " << new_parameter << endl;
	};
//	if (kind_of() == TEXT) {
//		wide_string x;
//		unsigned short int the_text_length;
//		if (current_wide_text(x,the_text_length) && the_text_length < 65535 && the_text_length > 0) {
//			if (x != NULL && x[0] == 'X') {
//				log("debug this");
//			};
//		};
//	};
   add_to_log_if_interesting_error();
#endif
	if (tt_shutting_down) {
		parameter = new_parameter; // new on 310100 since sometimes needed to dump history while shutting down
		return(TRUE);
	};
	if (parameter != new_parameter) {
#if TT_CAREFUL
      if (code != SYNTHETIC_SPRITE) {
         short int count = parameter_counts[code];
		   if (new_parameter >= count || new_parameter < 0) {
			   if (warn) {
				   say_error(IDS_PICK_NONEXISTENT_MOVIE);
               if (reset_cycle_regardless) reset_cycle();
               return(FALSE);
			   } else { // new policy is to cycle thru
               new_parameter %= count;
               if (new_parameter < 0) new_parameter += count;
            };
         };
		};
#endif
		// following moved on 260200 here for robustness if new_parameter is out of range
		// experiment on 250803 to comment out the following
		//if (loading) { // new on 180100 -- just used to set off special behaviors (like hiding buttons on tools)
		//	parameter = new_parameter;
		//	return(TRUE);
		//};
		parameter = new_parameter;
		set_clean_status(DIRTY);
		reset_cycle();
		return(TRUE);
	};
	if (reset_cycle_regardless) reset_cycle(); // experiment on 250803 to remove  && !loading
	return(TRUE);
};

void Sprite::reset_cycle() {
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	if (code == SYNTHETIC_SPRITE || code == USER_SPRITE) return;
	current_index = 0;
	looped_back_already = FALSE;
	single_step = FALSE;
	update_cycle_duration();
	if (image_cycles[code][parameter].return_number_of_images() == 1) {
		cycle_start_time = 0; 
		// new on 240803 - since this value doesn't matter set it to 0 so isn't dumped as XML (debugging logs also works better this way)
	} else {
		cycle_start_time = tt_current_time;
	};
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log(_T("Debug target cycle reset."));
	};
   add_to_log_if_interesting_error();
#endif
};

void Sprite::update_cycle_duration() {
	cycle_duration = image_cycles[code][parameter].total_cycle_duration(looped_back_already);
};

boolean Sprite::cycle_stopped() {
	if (code == SYNTHETIC_SPRITE || code == USER_SPRITE) return(FALSE);
	return(image_cycles[code][parameter].cycle_stopped(current_index));
};

boolean Sprite::cycles() {
	// TRUE if this cycles
	if (code == SYNTHETIC_SPRITE || code == USER_SPRITE) return(FALSE);
	return(image_cycles[code][parameter].cycles());
};

int selection_x_growth() {
//	return(tt_frame_number&1?1:2);
	return(tt_frame_number%4);
};

int selection_y_growth() {
//	return(tt_frame_number&1?2:1);
  // out of phase with x growth
	return((tt_frame_number+2)%4);
};

// following causes "circular" movement
// *tt_movement_selection_feedback is new as of 220203 - nice suggestion from Leonel
city_coordinate selection_delta_x() {
	if (tt_movement_selection_feedback <= 0) return(0);
	switch ((tt_current_time%400)/100) {//tt_frame_number%4) {
	  case 0:
		  return(tt_movement_selection_feedback*tt_screen->one_pixel_in_x_city_units());		
	  case 2:
		  return(tt_movement_selection_feedback*-tt_screen->one_pixel_in_x_city_units());
	  default:
		  return(0); 
	};
};

city_coordinate selection_delta_y() {
	if (!tt_movement_selection_feedback) return(0);
   switch ((tt_current_time%400)/100) {//tt_frame_number%4) {
	  case 1:
		  return(tt_movement_selection_feedback*tt_screen->one_pixel_in_y_city_units());
	  case 3:
		  return(tt_movement_selection_feedback*-tt_screen->one_pixel_in_y_city_units());
	  default:
		  return(0); 
	};
};

city_coordinate Sprite::true_llx(boolean including_followers) {
   if (including_followers && (!followers_completely_inside() || followers_absorbed()) && followers != NULL) {
      city_coordinate min_llx = llx;
      Sprites *remaining = followers;
      while (remaining != NULL) {
         Sprite *sprite = remaining->first();
         city_coordinate follower_llx = sprite->true_llx();
         if (follower_llx < min_llx) {
            min_llx = follower_llx;
         };
         remaining = remaining->rest();
      };
      return(min_llx);
   };
   return(llx);
};

city_coordinate Sprite::true_lly(boolean including_followers) {
   if (including_followers && (!followers_completely_inside() || followers_absorbed()) && followers != NULL) {
      city_coordinate min_lly = lly;
      Sprites *remaining = followers;
      while (remaining != NULL) {
         Sprite *sprite = remaining->first();
         city_coordinate follower_lly = sprite->true_lly();
         if (follower_lly < min_lly) {
            min_lly = follower_lly;
         };
         remaining = remaining->rest();
      };
      return(min_lly);
   };
   return(lly);
};

void Sprite::true_region(TTRegion &region, boolean including_followers) {
	// might be worth being more clever about this
   city_coordinate x = true_llx(including_followers);
   city_coordinate y = true_lly(including_followers);
	region.min_x = x;
	region.min_y = y;
	// changed on 290699 so I multiply and then divide since otherwise can lose about a pixel
	region.max_x = x+(city_coordinate) ((((int64) current_scale)*true_width(including_followers))/ground_scale); // int64 new on 080202
	region.max_y = y+(city_coordinate) ((((int64) current_scale)*true_height(including_followers))/ground_scale);
};

//long Sprite::width_percentage_of_true_width(boolean including_followers) {
//	if (pointer_to_current_image() == NULL || size_is_true()) { //  removed true_size_flag ||  on 131202 since size_is_true is a better test
//		// size_true_flag should really be size_is_true() but that breaks demos
//		return(100);
//	} else if (including_followers && followers != NULL && (!followers_completely_inside() || followers_absorbed())) {
////		scale_to_fit(); // commented out on 230903
////		return((width*100)/max(true_width(TRUE),1));
//		// rewritten on 230903 to be more sensible - old scheme can cause things to shrink to zero size
//		return((width_given_scale(256)*100)/max(true_width(TRUE),1));
//   } else {
//		return(shrink_width_from_640x480_screen_size((maximum_width(appearance_code())*100)/pointer_to_current_image()->width_without_scaling()));
//	};
//};

double Sprite::width_fraction_of_true_width(boolean including_followers) {
	// rewritten to use doubles so small objects don't have huge round off problems
	if (pointer_to_current_image() == NULL || size_is_true()) { //  removed true_size_flag ||  on 131202 since size_is_true is a better test
		// size_true_flag should really be size_is_true() but that breaks demos
		return(1.0);
	} else if (including_followers && followers != NULL && (!followers_completely_inside() || followers_absorbed()) && x_scale > 0) { 
		// y_scale check new on 230903 -- changed to x_scale test on 080705
//		scale_to_fit(); // commented out on 230903
		return(((double) width)/max(true_width(TRUE),1));
   } else {
		return(((double) 
					shrink_width_from_640x480_screen_size(maximum_width(appearance_code())))/pointer_to_current_image()->width_without_scaling());
	};
};

//long Sprite::height_percentage_of_true_height(boolean including_followers) {
//	if (pointer_to_current_image() == NULL || size_is_true()) { // removed true_size_flag ||  on 131202 since size_is_true is a better test
//		return(100);
//	} else if (including_followers && followers != NULL && (!followers_completely_inside() || followers_absorbed())) {
////		return((height*100)/max(true_height(TRUE),1));
//		return((height_given_scale(256)*100)/max(true_height(TRUE),1));
//   } else {
//		return(shrink_height_from_640x480_screen_size((maximum_height(appearance_code())*100)/pointer_to_current_image()->height_without_scaling()));
//	};
//};

double Sprite::height_fraction_of_true_height(boolean including_followers) {
	if (pointer_to_current_image() == NULL || size_is_true()) { // removed true_size_flag ||  on 131202 since size_is_true is a better test
		return(1.0);
	} else if (including_followers && followers != NULL && (!followers_completely_inside() || followers_absorbed()) && y_scale > 0) { 
		// y_scale check new on 230903
		return(((double) height)/max(true_height(TRUE),1));
   } else {
		return(((double) 
					shrink_height_from_640x480_screen_size(maximum_height(appearance_code())))/pointer_to_current_image()->height_without_scaling());
	};
};

city_coordinate Sprite::true_width(boolean including_followers) {
	// removed true_size_flag on 210803 since is included in size_is_true()
	if (pointer_to_current_image() == NULL || size_is_true() || x_scale == max_unsigned_short) { // (tt_log_version_number > 14 &&
		// x_scale == max_unsigned_short new on 101204 since if too big for x_scale then width is most accurate we can do
		return(width);
   };
   city_coordinate total_width = (current_image->width_without_scaling()*x_scale)>>8;
//	if (tt_screen_width > 640) { // new on 170999
//		total_width = (tt_screen_width*total_width)/640;
//	};
   if (including_followers && (!followers_completely_inside() || followers_absorbed()) && followers != NULL) {
      city_coordinate follower_min_x = llx;
      city_coordinate follower_max_x = llx+total_width;
      Sprites *remaining = followers;
      while (remaining != NULL) {
         Sprite *sprite = remaining->first();
         city_coordinate follower_llx = sprite->true_llx(TRUE);  // was ()
         city_coordinate follower_width = sprite->true_width(TRUE);  // was ()
			// commented out the following on 080705 since causes bad sizes (e.g. a team put in a box offscreen)
			// and there shouldn't be parts that have a lower lly than the leader (or if so it'll be truncated, right?)
         //if (follower_llx < follower_min_x) {
         //   follower_min_x = follower_llx;
         //}; // was else prior to 151199
			if (follower_llx+follower_width > follower_max_x) {
            follower_max_x = follower_llx+follower_width;
         };
         remaining = remaining->rest();
      };
      return(follower_max_x-follower_min_x);
   };
	return(total_width);
};

city_coordinate Sprite::true_height(boolean including_followers) {
	// removed true_size_flag on 210803 since is included in size_is_true()
	if (pointer_to_current_image() == NULL || size_is_true() || y_scale == max_unsigned_short) { // (tt_log_version_number > 14 && 
		// y_scale == max_unsigned_short new on 101204 since if too big for y_scale then height is most accurate we can do
		return(height);
	};
   city_coordinate total_height = (current_image->height_without_scaling()*y_scale)>>8;
//	if (tt_screen_width > 480) { // new on 170999
//		total_height = (tt_screen_height*total_height)/480;
//	}
   if (including_followers && (!followers_completely_inside() || followers_absorbed()) && followers != NULL) {
      city_coordinate follower_min_y = lly;
      city_coordinate follower_max_y = lly+total_height;
      Sprites *remaining = followers;
      while (remaining != NULL) {
         Sprite *sprite = remaining->first();
         city_coordinate follower_lly = sprite->true_lly(TRUE); // was ()
         city_coordinate follower_height = sprite->true_height(TRUE);  // was ()
			// commented out the following on 080705 since causes bad sizes (e.g. a team put in a box offscreen)
			// and there shouldn't be parts that have a lower lly than the leader (or if so it'll be truncated, right?)
         //if (follower_lly < follower_min_y) {
         //   follower_min_y = follower_lly;
         //}; // else removed 151199
			if (follower_lly+follower_height > follower_max_y) {
            follower_max_y = follower_lly+follower_height;
         };
         remaining = remaining->rest();
      };
      return(follower_max_y-follower_min_y);
   };
	return(total_height);
};

void Sprite::set_true_size(city_coordinate new_width, city_coordinate new_height, ChangeReason reason) { // reason added on 220200
	if (size_is_true()) { //code == SYNTHETIC_SPRITE) {
		set_size(new_width,new_height,reason);
	} else {
		set_true_size_and_location(new_width,new_height,llx,lly,reason);
	};
};

void Sprite::set_true_size_and_location(city_coordinate new_width, city_coordinate new_height,
													 city_coordinate new_llx, city_coordinate new_lly,
													 ChangeReason reason) {
	if (size_is_true()) {
		set_size_and_location(new_width,new_height,new_llx,new_lly,reason);
	} else {
		double fraction_adjustment_x, fraction_adjustment_y;
		scale_to_fit(); // to be sure that true_height is right -- moved here from 2nd branch on 080699 so size is OK for remote looks
//		if (tt_log_version_number > 12) { 
			fraction_adjustment_x = width_fraction_of_true_width(TRUE); 
			fraction_adjustment_y = height_fraction_of_true_height(TRUE);
//		} else {
//			city_coordinate real_width = true_width(TRUE);
//			city_coordinate real_height = true_height(TRUE);
//			if (real_width == 0) real_width = 1;
//			if (real_height == 0) real_height = 1;
//			fraction_adjustment_x = (width*100)/real_width;
//			fraction_adjustment_y = (height*100)/real_height;
//		};
//#if TT_DEBUG_ON
//		// MAKE A VERSION FOR AN INDIRECTION TO EXPLOSION TOO
//	if (tt_debug_mode == 140499 && (appearance_code() == EXPLOSION_SPRITE || appearance_code() == MARTIAN_SPRITE)) {
//		tt_error_file() << "True " << new_width << "x" << new_height
//				      << "; Old " << width << "x" << height
//						<< "; f=" << tt_frame_number 
//						<< ";counter=" << debug_counter 
//						<< ";reason=" << (int) reason << endl;
//		};
//#endif
		set_size_and_location((city_coordinate) (new_width*fraction_adjustment_x),
									 (city_coordinate) (new_height*fraction_adjustment_y),
									 (city_coordinate) (new_llx+((llx-true_llx(TRUE))*fraction_adjustment_x)),
									 (city_coordinate) (new_lly+((lly-true_lly(TRUE))*fraction_adjustment_y)),
									 reason);
	};
};

void Sprite::animate_true_size_and_location(city_coordinate new_width, city_coordinate new_height,
														  city_coordinate new_llx, city_coordinate new_lly,
														  millisecond duration,
														  Sprite *notify,
														  AnimationCallBack callback,
														  boolean signal_change) {
   // new on 151199
	if (size_is_true()) {
		animate_size_and_location(new_width,new_height,new_llx,new_lly,duration,notify,callback,signal_change);
	} else {
		double fraction_adjustment_x, fraction_adjustment_y;
		scale_to_fit();
		fraction_adjustment_x = width_fraction_of_true_width(TRUE); 
		fraction_adjustment_y = height_fraction_of_true_height(TRUE);
		animate_size_and_location((city_coordinate) (new_width*fraction_adjustment_x),
										  (city_coordinate) (new_height*fraction_adjustment_y),
										  (city_coordinate) (new_llx+((llx-true_llx(TRUE))*fraction_adjustment_x)),
										  (city_coordinate) (new_lly+((lly-true_lly(TRUE))*fraction_adjustment_y)),
										  duration,notify,callback,signal_change);
	};
};

city_coordinate Sprite::true_x_offset() {
	if (true_size_flag || size_is_true()) { // (tt_log_version_number > 14 && 
		return(0);
	} else {
		return((old_x_offset*x_scale)>>8);
	};
};

city_coordinate Sprite::true_y_offset() {
	if (true_size_flag || size_is_true()) { // (tt_log_version_number > 14 && 
		return(0);
	} else {
		return((old_y_offset*y_scale)>>8);
	};
};

/*
city_coordinate Sprite::full_width() {
	if (current_image == NULL) {
		return(width);
	} else {
		return(current_image->width_without_scaling());
	};
};

city_coordinate Sprite::full_height() {
	if (current_image == NULL) {
		return(height);
	} else {
		 return(current_image->full_height());
	};
};
*/

boolean Sprite::rectangular() {
	if (code == SYNTHETIC_SPRITE) {
		return(current_parameter()/256 == SYNTHETIC_RECTANGLE);
	} else {
		// added current_image != NULL on 150103 since while loading XML this can happen (via ANCESTORS_REMOTE)
		return(pointer_to_current_image() != NULL && current_image->rectangular());
	};
};
	
int Sprite::get_size_percent() {
	// averages if not scaled proportionally
	if (tt_screen_width <= tt_graphics_video_mode_width) {
		return(((x_scale+y_scale)*50)>>8);
	} else { // new on 170999
		return((tt_graphics_video_mode_width*((x_scale+y_scale)*50)>>8)/tt_screen_width);
	};
};

int Sprite::get_width_percent() {
	return(shrink_height_from_640x480_screen_size((x_scale*100)>>8));
};

int Sprite::get_height_percent() {
	return(shrink_height_from_640x480_screen_size((y_scale*100)>>8));
};

void Sprite::set_size_maintain_proportionality(city_coordinate new_width,
									                    city_coordinate new_height) {
   // sets it to the largest dimensions that preserve its current aspect ratio
   if (width*new_height > new_width*height) { // equivalent to (width/height > new_width/new_height) so maintain width
      set_width_maintain_proportionality(new_width);
   } else {
      set_height_maintain_proportionality(new_height);
   };
};

void Sprite::set_size_percent(int percent, ChangeReason reason) {
	set_size((percent*max_widths[code])/100,(percent*max_heights[code])/100,reason);
};

void Sprite::set_size_percents(int x_percent, int y_percent) {
	set_size((x_percent*max_widths[code])/100,(y_percent*max_heights[code])/100);
};

void Sprite::animate_to_size_percent(int percent, millisecond duration) {
	animate_size((percent*max_widths[code])/100,(percent*max_heights[code])/100,
					 duration);
};

void Sprite::animate_to_size_percents(int x_percent, int y_percent,
                                      millisecond duration) {
	animate_size((x_percent*max_widths[code])/100,(y_percent*max_heights[code])/100,
					 duration);
};

int Sprite::resource_index() {
   if (pointer_to_current_image() == NULL) return(-1);
	return(current_image->return_resource_index());
};

void Sprite::set_appearance_code(SpriteCode new_code) { // better than destroying the old and recreating a new one
	// moved here from sprite.h on 031202
   code = new_code;
   set_old_x_offset(0);
   set_old_y_offset(0);
   current_index = 0;
   parameter = -1; // same reason sprite::sprite initializes it so
   size_changed_flag = TRUE;
//     leaders_removed_flag = FALSE;
   animation_pending_callbacks_only = FALSE;
   screen_updates_flag = TRUE; // why?
   ideal_speed = 0;
};

//int Sprite::image_cycle_resource_index() {
//   if (current_image == NULL) return(-1);
//	return(current_image->resource_index_of_first_cycle_image());
//};

int Sprite::image_cycle_resource_index() {
	if (code == SYNTHETIC_SPRITE) {
		return(-current_parameter());
	} else {
		return(image_cycles[code][parameter].resource_index_of_first_cycle_image());
	};
};

//void Sprite::decrement_screen_ref_count() {
//	screen_ref_count--;
//	if (screen_ref_count == 0) {
//	  ref_count_is_zero();
//	};
//};

millisecond Sprite::default_duration(millisecond the_default_duration) {
  if (tt_finish_all_animations) return(0);
  if (leader != NULL && leader->kind_of() == PICTURE) return(0); // new policy 4/10/97
  if (showing_on_a_screen()) {
     if (floor->in_the_room()) return(the_default_duration/room_speedup);
     return(the_default_duration);
  };
  return(0);
};

//boolean Sprite::showing_on_a_screen() {
//	if (screen_ref_count) return(TRUE);
//	if (leader != NULL) {
//		if (leader->return_screen_ref_count()) return(TRUE);
//		Sprite *next_leader = leader->pointer_to_leader();
//		while (next_leader != NULL) {
//			if (next_leader->return_screen_ref_count()) return(TRUE);
//			next_leader = next_leader->pointer_to_leader();
//		};
//	};
//	return(FALSE);
//};

//void Sprite::recursively_increment_screen_ref_count() {
//	increment_screen_ref_count();
//	Sprites *remaining = followers;
//	while (remaining != NULL) {
//		remaining->first()->recursively_increment_screen_ref_count();
//		remaining = remaining->rest();
//	};
//};

//void Sprite::recursively_decrement_screen_ref_count() {
//	decrement_screen_ref_count();
//	Sprites *remaining = followers;
//	while (remaining != NULL) {
//		remaining->first()->recursively_decrement_screen_ref_count();
//		remaining = remaining->rest();
//	};
//};

//void Sprite::recursively_set_screen_ref_count(boolean new_ref_count) {
//	set_screen_ref_count(new_ref_count);
//	Sprites *remaining = followers;
//	while (remaining != NULL) {
//		remaining->first()->recursively_set_screen_ref_count(new_ref_count);
//		remaining = remaining->rest();
//	};
//};

void Sprite::recursively_notice_on_screen_change(boolean on_screen) {
	notice_on_screen_change(on_screen);
	Sprites *remaining = followers;
	while (remaining != NULL) {
		remaining->first()->recursively_notice_on_screen_change(on_screen);
		remaining = remaining->rest();
	};
};

boolean Sprite::showing_on_a_screen() {
//		 return(screen_ref_count);
#if TT_DEBUG_ON
	if (floor != NULL && floor->prepared_for_deletion) {
		if (!tt_shutting_down) {
			log("Something is on a background that has been destroyed.");
		};
	};
#endif
   return(floor != NULL && floor->showing_on_a_screen());
};

void Sprite::now_released_on(Sprite *underneath, Sprite *by) {
   if (by->kind_of() == ROBOT_IN_TRAINING &&
       !(underneath->kind_of() == INTEGER && kind_of() == CUBBY) && // e.g. breaking up cubby
		 (!underneath->is_blank() || underneath->kind_of() == PICTURE)) { // or type coersion - picture isn't type coersion (added 050299)
//      ((Robot *) by)->remove_tool_from_working_set(this);
		set_available_for_indexing(FALSE); // re-written on 090701 so Bammer can be undone
   };
};

void Sprite::signal_now_on_floor() {
	if (floor != NULL) { // not clear how this happened (issue 616) -- new on 110105
		now_on_floor(floor,tt_programmer->pointer_to_appearance()); 
		// second arg was NULL prior to 090705 but caused a crash when pasting a loaded truck on the ground
	};
};

void Sprite::now_on_floor(Background *new_background, Sprite *) {
	if (tt_resetting_or_destroying_city) return;
#if TT_DEBUG_ON
	if (tt_debug_mode == 230304) {
		tt_error_file() << "Sprite::now_on_floor called on frame " << tt_frame_number << " at time " << tt_current_time << endl;
	};
#endif
	// by default give priority but don't warn
	new_background->add_item(this,TRUE,FALSE,FALSE);
//   if (!tt_stop_all || tt_log_version_number > 14) {
		// revised on 010499 since picture::activate now postpones activations if the power is off
		if (active()) { // conditional new on 081199
			// is this a good idea??
			recursively_activate_pictures(); // new policy on 3/25/97    //  || tt_log_version_number < 4
		};
//	};
	// following happens for example when a wand drops something
//	animate_to_good_size(default_duration(1000));
};

void Sprite::recursively_activate_pictures(boolean reset_animation, PictureActivateContext context) {
   Sprites *remaining = followers;
   while (remaining != NULL) {
      Sprite *sprite = remaining->first();
		sprite->recursively_activate_pictures(reset_animation,context);
      remaining = remaining->rest();
   };
};

void Sprite::remove_from_floor(boolean report_problems, boolean remove_from_screen,
                               boolean reset_background) {
	if (floor != NULL) {
      if (floor != tt_deleting_background) {
		   floor->remove_item(this,report_problems,remove_from_screen,reset_background);
      } else { // better solution than this?
         floor = NULL;
      };
	} else if (remove_from_screen) { // new on 090105 so this happens regardless of whether floor still knows this guy
		tt_screen->remove(this);
	};
};

void Sprite::abort_action_if_robot(//ActionStatus *action_status, 
                                   int reason,
											  SpriteType actor, boolean explain_anyway, int importance, string default_string) {
	if (explain_anyway) {
		abort_action(reason,actor,importance); // action_status
	} else {
		action_aborted();
	};
}

void Sprite::abort_action(int reason, SpriteType actor, int importance, string default_string) { 
	// default_string added on 120205 in case not defined in current language
//  if (action_status != NULL) { // commented out since only robot's need worry about this
//	  action_aborted();
//#if TT_DEBUG_ON
//	  if (tt_debug_mode == 411) { // maybe 411 should be used for useful but low priority information
//	     tt_error_file() << S(IDS_WARNING) << " ";
////	     describe_type(tt_error_file());
////      print_type(actor,tt_error_file(),DEFINITE_ARTICLE);
//	     tt_error_file() << S(IDS_ACTION_STOPPED_BECAUSE);
//		  if (reason != 0) {
//			  tt_error_file() << S(reason) << endl;
//		  } else {
//			   tt_error_file() << "no reason given." << endl;
//		  };
//	  };
//#endif
//	};
	if (reason >= 0) { // condition new on 130205 since sometimes Marty has already explained the problem
		report_action_aborted(reason,this,actor,importance,default_string);
	};
};

//void Sprite::initial_position(city_coordinate center_x, city_coordinate center_y) {
//	llx = center_x-width/2;
//	lly = center_y-height/2;
//	update_priority();
//};

void Sprite::signal_size_and_location() {
	set_size_and_location(width,height,llx,lly,ANIMATION_COMPLETED);
};

void Sprite::set_leader(Sprite *the_leader, boolean temporarily) {
	//commented out the folllowing on 100400 since part_of triggers bugs sometimes...
//#if TT_CAREFUL
//	if (the_leader != NULL && the_leader->part_of(this)) {
//		say_error(_T("About to make something a part of something that already has something as a part!"));
//		return;
//	};
//#endif
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Setting leader of debug target.");
	};
	if (the_leader != NULL && the_leader->has_leader(this)) {
		log("Warning: Creating a cycle of containment!!!",FALSE,TRUE);
		return;
	};
#endif
	leader = the_leader;
   if (leader != NULL) {
      // commented out on 210802 since how can something in the notebook receive followers?
      // and with new meaning of set_in_notebook this is hard to implement
      if (leader->in_notebook()) { // new on 170702
//         set_in_notebook(TRUE);
         in_notebook_flag = TRUE; // not so good - but hard to do otherwise - new on 210802 -- should really recur - right?
		} else if (leader->inside_vacuum()) { // new on 170702
         set_inside_vacuum(TRUE);
		//} else {
		//	set_recent_xml_location(0); // worth checking again since maybe it is now independent - new on 100703
      };
	} else if (leader_offsets != NULL && !temporarily) {
		// new on 260103 - this way XML can be sure when the relationship was loaded vs. left over from before
		// temporarily is new on 270103 since might be called while flipping (and swaping front and back)
		delete leader_offsets;
		leader_offsets = NULL;
	};
};

boolean Sprite::has_leader(Sprite *sprite) {
	if (leader == NULL) return(FALSE);
	if (leader == sprite) return(TRUE);
   return(leader->has_leader(sprite));
};

Sprite *Sprite::ultimate_leader() {
  if (leader == NULL) {
	  return(this);
  } else {
	  return(leader->ultimate_leader());
  };
};

Sprite *Sprite::immediate_container() { // new on 120504
#if TT_POST3186
	if (leader != NULL && leader->kind_of() == PICTURE) { 
		// new on 051206 could be on something on the back of something else
		return(leader);
	};
#endif
	Sprite *picture;
	if (floor != NULL && (picture = floor->pointer_to_picture()) != NULL) {
		return(picture);
	} else {
		return(leader);
	};
};

Sprite *Sprite::ultimate_container() {
	Sprite *picture;
	if (floor != NULL && (picture = floor->pointer_to_picture()) != NULL) {
		return(picture->ultimate_container());
	} else if (leader != NULL) {
		return(leader->ultimate_container());
//   } else if (current_held_status() == HELD_BY_PROGRAMMER) {
//      return(tt_programmer->pointer_to_appearance());
	} else {
		return(this);
	};
};

Sprite *Sprite::ultimate_top_level_container() { // new on 090205 -- stops if on a page
	Sprite *picture;
	if (floor != NULL && (picture = floor->pointer_to_picture()) != NULL) {
		return(picture->ultimate_top_level_container());
	} else if (leader != NULL && leader->kind_of() != PROGRAM_PAD) {
		return(leader->ultimate_top_level_container());
	} else {
		return(this);
	};
};

boolean Sprite::common_picture_container(Sprite *other) {
	Sprite *picture;
	if (floor != NULL && (picture = floor->pointer_to_picture()) != NULL) {
		if (other->part_of(picture)) return(TRUE);
		return(picture->common_picture_container(other));
	} else if (leader != NULL) {
		if (leader->kind_of() == PICTURE && other->part_of(leader)) return(TRUE);
		return(leader->common_picture_container(other));
	} else {
		return(FALSE);
	};
};

Sprite *Sprite::ultimate_flipside_picture() {
	Sprite *picture;
	if (floor != NULL && (picture = floor->pointer_to_picture()) != NULL) {
		return(picture->ultimate_flipside_picture());
	} else {
		return(this);
	};
};

Sprite *Sprite::ultimate_unflipped_picture() { // new on 220500
	Sprite *picture;
	if (floor != NULL && is_flipped() && (picture = floor->pointer_to_picture()) != NULL) {
		return(picture->ultimate_unflipped_picture());
	} else {
		return(this);
	};
};

boolean Sprite::member_of_background(Background *background) {
	if (floor == background) return(TRUE);
	if (floor == NULL) return(FALSE);
	Picture *picture = floor->pointer_to_picture();
	if (picture != NULL) {
		return(picture->member_of_background(background));
	};
	return(FALSE);
};

boolean Sprite::size_constrained() {
	// what if this is an indirection of a picture that is contrained??
	return((leader != NULL && 
			  // following leaders don't constrain the size
//			  leader->kind_of() != PROGRAMMER &&
			  leader->kind_of() != ROBOT_IN_TRAINING &&
			  leader->kind_of() != ROBOT &&
			  leader->kind_of() != COPIER) ||  // COPIER added on 080803
			 is_top_of_a_stack()); // new on 260500
};

//void Sprite::changed_so_update() {
//		current_priority = lly;
//};
//	if (can_collide_flag) {
//		if (collision_region_equals_region) {
//			collision_region_width = width;
//			collision_region_height = height;
//		};
//		if (collision_region == NULL) {
//			collision_region = new TTRegion;
//		};
//      if (motion_extends_collision_region) {
//			collision_region->min_x = min(llx,previous_llx)+collision_x_offset;
//         collision_region->max_x = max(llx,previous_llx)+
//                                   collision_region_width+
//                                   collision_x_offset;
//         collision_region->min_y = min(lly,previous_lly)+collision_y_offset;
//         collision_region->max_y = max(lly,previous_lly)+
//                                   collision_region_height+
//                                   collision_y_offset;
//      } else {
//			collision_region->min_x = llx+collision_x_offset;
//			collision_region->max_x = llx+
//											  collision_region_width+
//											  collision_x_offset;
//			collision_region->min_y = lly+collision_y_offset;
//			collision_region->max_y = lly+
//											  collision_region_height+
//											  collision_y_offset;
//		};
//	};

void Sprite::dirty_region(TTRegion &region) {
	boolean none_found_yet = TRUE;
	if (clean_status == FOLLOWERS_DIRTY) {
		Sprites *remaining = followers;
		while (remaining != NULL) {
			Sprite *sprite = remaining->first();
			if (sprite->completely_inside_leader() && 
				 !sprite->completely_clean()) {
				if (none_found_yet) {
					sprite->dirty_region(region);
					none_found_yet = FALSE;
				} else {
					TTRegion more;
					sprite->dirty_region(more);
					region.extend_with(more);
				};
			};
			remaining = remaining->rest();
		};
	} else {
		true_region(region);
	};
};

void Sprite::full_region(TTRegion &region) {
	region.min_x = llx;
	if (width > max_long/1000) { // risk of overflow - handled on 250202
		region.max_x = llx+current_scale*(width/ground_scale);
	} else {
		region.max_x = llx+(current_scale*width)/ground_scale;
	};
	region.min_y = lly;
	if (height > max_long/1000) {
		region.max_y = lly+current_scale*(height/ground_scale);
	} else {
		region.max_y = lly+(current_scale*height)/ground_scale;
	};
};

void Sprite::propagate_changes(ChangeReason reason) { 
	// I check for followers since can be called recursively by followers
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Debug target propagating changes."));
	};
#endif
	if (pointer_to_top_of_stack() != NULL) {
		pointer_to_top_of_stack()->set_changes_propagated(changes_propagated_flag);		
		pointer_to_top_of_stack()->propagate_changes(reason);
		return;
	};
	if (followers == NULL) return;
	if (changes_propagated()) { // on the list twice somehow
		return;
	};
	if (!clean_or_obscured()) {
		really_propagate_changes(reason);
	};
	// can't update_display as cdring thru above since that can
	// change the followers list
	if (!followers->update_display_all() && followers != NULL) {
		 // update may have removed items so check again its non-NULL
		 // problem with first element
#if TT_DEBUG_ON
		if (tt_debug_mode == 160205 && tt_deleted_lists != NULL && tt_deleted_lists->member_cons(followers) != NULL) {
			debug_this("Same list to be deleted twice.");
		};
#endif
		 tt_deleted_lists = new SpritesList(followers,tt_deleted_lists);
		 followers = followers->rest();
	};
//	followers->update_display(TRUE); // just those "inside" but broke st
	set_changes_propagated(TRUE);
};

void Sprite::really_propagate_changes(ChangeReason reason) {
//	city_coordinate center_x = llx+width/2;
//	city_coordinate center_y = lly+height/2;
//	city_coordinate x,y;
//	true_center(x,y);
	if (followers == NULL) return;
	if (width < minimum_width || height < minimum_height) { // new on 061000 -- was == prior to 110101
		return;
	};
	const city_coordinate true_x = llx-true_x_offset();
	const city_coordinate true_y = lly-true_y_offset();
	Sprites *followers_remaining = followers;
	while (followers_remaining != NULL) {
		Sprite *follower = followers_remaining->first();
		follower->follow_leader_changes(width,height,true_x,true_y,reason);
		followers_remaining = followers_remaining->rest();
	};
};

//void Sprite::really_propagate_changes_to_follower(Sprite *follower, ChangeReason reason) {
//	// variant of the above made on 030603
//	if (width < minimum_width || height < minimum_height) { 
//		return;
//	};
//	const city_coordinate true_x = llx-true_x_offset();
//	const city_coordinate true_y = lly-true_y_offset();
//	follower->follow_leader_changes(width,height,true_x,true_y,reason);
//};

void Sprite::recursively_propagate_changes(ChangeReason reason) {
   really_propagate_changes(reason);
	if (followers != NULL) {
      Sprites *remaining = followers;
      while (remaining != NULL) {
		   remaining->first()->recursively_propagate_changes(reason);
			remaining = remaining->rest();
      };
	};
};

//void Sprite::recursively_propagate_changes() {
//	if (followers != NULL) {
//		const city_coordinate true_x = llx-true_x_offset();
//		const city_coordinate true_y = lly-true_y_offset();
//		Sprites *remaining = followers;
//		while (remaining != NULL) {
//			Sprite *follower = remaining->first();
//			follower->follow_leader_changes(width,height,true_x,true_y);
//			follower->recursively_propagate_changes();
//			remaining = remaining->rest();
//		};
//	};
//};

void Sprite::recursively_set_cache_status(CacheStatus new_status) {
	cache_status = new_status;
	Sprites *remaining = followers;
	while (remaining != NULL) {
		Sprite *sprite = remaining->first();
		if (sprite->completely_inside_leader()) {
			remaining->first()->recursively_set_cache_status(new_status);
		};
		remaining = remaining->rest();
	};
};

void Sprite::recursively_set_visible(boolean new_flag) {
	set_visible(new_flag);
	Sprites *remaining = followers;
	while (remaining != NULL) {
		remaining->first()->recursively_set_visible(new_flag);
		remaining = remaining->rest();
	};
};

void Sprite::set_cached_with_followers() {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log("set_cached_with_followers()");
	};
#endif
	cache_status = CACHED_WITH_FOLLOWERS;
	Sprites *remaining = followers;
	while (remaining != NULL) {
		Sprite *sprite = remaining->first();
		if (sprite->completely_inside_leader()) {
			sprite->recursively_set_cache_status(CACHED_BY_LEADER);
		};
		remaining = remaining->rest();
	};
};

void Sprite::recursively_reset_cache_status() {
#if TT_DEBUG_ON
   if (deleted()) { // might be called from :prepare_for_deletion() so commented out following on 090699 || prepared_for_deletion_flag) {
      log(_T("Resetting the cache status of a deleted object."),FALSE,TRUE);
   };
#endif
	cache_status = NOT_CACHED;
	Sprites *remaining = followers;
	while (remaining != NULL) {
		remaining->first()->recursively_reset_cache_status();
		remaining = remaining->rest();
	};
};

void Sprite::set_inside_vacuum(boolean new_flag, boolean recur) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		tt_error_file() << "Setting inside_vacuum_flag of debug target to " << (int) new_flag << endl;
	};
#endif
//   if (inside_vacuum_flag == new_flag) return; 
	// new on 170702 to ignore calls that already know about -- commented out on 190702 since might not have been called with recur earlier
	inside_vacuum_flag = (flag) new_flag;
   if (recur) { // condition new on 160702
	   Sprites *remaining = followers;
	   while (remaining != NULL) {
		   Sprite *follower = remaining->first();
		   if (follower != NULL) {
			   follower->set_inside_vacuum(new_flag,recur);
		   };
		   remaining = remaining->rest();
	   };
   };
};

void Sprite::add_follower(Sprite *follower, boolean completely_inside, InsertionOrdering insertion_ordering, boolean dont_record_new_offsets,
								  AddFollowerReason reason) { // new arg on 210104
	if (follower == NULL) return; // new on 250203 since XML can do this if things have been edited
#if TT_DEBUG_ON
	if (debug_log(debug_counter,"Adding a follower to debug target.")) {
		follower->print(tt_error_file());
		tt_error_file() << endl;
	} else if (debug_log(follower->debug_counter,"Debug target becoming a follower.")) {
		print(tt_error_file());
		tt_error_file() << endl;
   };
//   if (pointer_to_background() != follower->pointer_to_background() &&
//       follower->pointer_to_background() != NULL &&
//       tt_user_is_debugger) {
//      say("Something being made part of something else but they aren't both on the same floor.",4);
//   };
#endif
#if TT_CAREFUL
	// restored following despite following comment on 250399 since it is a sign of a true bug
	// following seems to cause troubles for pictures with flipped pictures on the back -- don't know why
	// added dont_record_new_offsets on 101202 since XML can now load leader_offsets so this shouldn't clobber them
	if (follower->pointer_to_leader() != NULL) {
      if (follower->pointer_to_leader() != this) {
#if !TT_DEBUG_ON
			if (tt_system_mode != PUZZLE) // conditional is because rocket decoration triggers this - track it down sometime
#endif
		   say_error(IDS_PART_OF_TWO_THINGS);
#if TT_DEBUG_ON
			if (follower->pointer_to_guid() != NULL) {
				print_guid_as_hexadecimal(follower->pointer_to_guid(),tt_error_file());
				tt_error_file() << " is the follower with two leaders." << endl;
			};
#endif
//		   follower->pointer_to_leader()->remove_follower(follower); // fix it
			follower->pointer_to_leader()->remove_selected_element(follower,NO_SELECTION_REASON_GIVEN,this,FALSE); 
			// new on 070601 since might be in a box or truck so contents need to be updated as well
#if TT_DEBUG_ON
		} else {
			debug_this("Same follower added to same leader twice?");
#endif
		};
	};
   if (follower == this) {
      say_error(IDS_IS_PART_OF_ITSELF,FALSE);
      return;
   };
#endif
//	if (follower->pointer_to_background() != NULL && kind_of() == BIRD) { // bird is where the bug is
//		tt_error_file() << "Is being made a part while still on the floor." << endl;
//	};
	if (completely_inside) {
		if (!follower->followers_completely_inside()) {
			follower->absorb_followers();
		};
//		follower->set_screen_ref_count(current_screen_ref_count());
	}; 
	if (followers == NULL && floor != NULL) {
		// e.g. not building shared stacks
		floor->add_leader(this);
	};
	if (insertion_ordering == INSERT_UNORDERED || followers == NULL) {
		followers = new Sprites(follower,followers);
	} else if (insertion_ordering == INSERT_AT_END) {
		followers->insert_at_end_if_not_member(follower); // how could it already be a member??
	} else {
		followers->insert_by_priority(follower);
	};
	follower->set_leader(this);
	if (!dont_record_new_offsets || follower->pointer_to_leader_offsets() == NULL) {
		follower->record_offsets(llx-true_x_offset(),lly-true_y_offset(),width,height);
	};
	if (!completely_inside && (leader == NULL || !leader->followers_completely_inside())) { // second condition new on 080705
		follower->set_screen_updates(FALSE);
		follower->set_completely_inside_leader(FALSE);
		// floor add_item will do more work now
		set_followers_completely_inside(FALSE);
      if (insertion_ordering != INSERT_BY_PRIORITY && floor != NULL) {
         floor->give_item_priority(follower);
      };
	} else {
		// followers not completely inside handled directly by screen
		set_clean_status(FOLLOWERS_DIRTY);
	};
#if TT_CAREFUL
	// moved on 100304 to before the following since that can cause this to produce spurious warnings
	if (floor != NULL && floor->has_item(follower)) {
		say_error(IDS_PART_OF_SOMETHING_WHILE_STILL_ON_FLOOR);
		floor->remove_item(follower,TRUE,TRUE,FALSE);
	};
#endif
	if (!is_flipped() && !is_temporarily_flipped()) { 
		// conditional commented out prior to 240299 but setting flipside objects to picture's background is
		// wrong and breaks things 
		// seems to depend upon whether it is really flipped or was flipped temporarily
		// things on the back of picture have the picture's background for their background
	// yes but this gets fixed when the picture is flipped I believe
	// and not setting it breaks things
	   follower->set_background(floor,TRUE,FALSE);
	};
//	set_recent_xml_location(0); // worth checking again since maybe it is now independent - new on 100703
};

void Sprite::remove_all_followers(boolean destroy_too) {
   Sprites *remaining = followers;
   while (remaining != NULL) {
      Sprites *next_remaining = remaining->rest();
		Sprite *sprite = remaining->first();
      remove_follower(sprite,!destroy_too); // arg added on 080400 so no warnings if about to destroy it anyway
      if (destroy_too) {
			if (!sprite->destroy()) { // other references but at least update it a bit
				sprite->release_all_references(); // commented out on 080604 as an experiment - restored as experiment later
				// following was new on 090200 so removed from floor if part has high ref count and isn't destroyed
//				remove_from_floor(FALSE,FALSE); // restored on 080604
//				sprite->set_background(NULL,TRUE); // new on 280300 // restored on 080604
				// above replaces the following on 090400 since prepare_for_deletion might destroy things (like Picture's background)
//				sprite->prepare_for_deletion(NULL); // since all other references are "weak" - 
//				// this at least stops birds on the back of these followers from flying around...
//				sprite->set_background(NULL,TRUE); // new on 280300
//				// would it be better to set_slated_for_destruction??
			};
		};
      remaining = next_remaining;
   };
	followers = NULL; // just to be safe - new on 160504
};

/* experiment on 110504 to use old version above instead

void Sprite::remove_all_followers(boolean destroy_too) {
   Sprites *remaining = followers;
   while (remaining != NULL) {
      Sprites *next_remaining = remaining->rest();
		Sprite *sprite = remaining->first();
      remove_follower(sprite,!destroy_too); // arg added on 080400 so no warnings if about to destroy it anyway
      if (destroy_too) {
			// following moved here on 020504 from Sprite::prepare_for_deletion
			if (!sprite->completely_inside_leader()) { // && sprite->showing_on_a_screen()) { // new policy 5/7/97
            tt_screen->remove(sprite);
         };
			if (!sprite->destroy()) { // other references but at least update it a bit
				// was new on 090200 so removed from floor if part has high ref count and isn't destroyed
				remove_from_floor(FALSE,FALSE);
//				sprite->release_all_references(); // commented out on 030504 since can cause next_robot to be broken
				// above replaces the following on 090400 since prepare_for_deletion might destroy things (like Picture's background)
//				sprite->prepare_for_deletion(NULL); // since all other references are "weak" - 
//				// this at least stops birds on the back of these followers from flying around...
//				sprite->set_background(NULL,TRUE); // new on 280300
//				// would it be better to set_slated_for_destruction??
			};
		};
      remaining = next_remaining;
   };
};
*/

boolean Sprite::remove_follower(Sprite *follower, boolean
#if TT_DEBUG_ON
										  warn_if_not_follower
#endif
									    ) {
	boolean result; // new on 191202
	if (followers == NULL) {
		result = FALSE;
#if TT_DEBUG_ON		
		if (warn_if_not_follower) { // conditional new on 220101
			say_error(IDS_PART_OF_SOMETHING_AND_NOT);
		};
#endif
//		return;
	} else {
		if (follower->cached() && !tt_resetting_or_destroying_city) {
			// tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 but can cause memory errors while resetting during time travel
			if (cached_without_followers()) {
				tt_screen->invalidate_cache();
#if TT_DEBUG_ON
				if (tt_debug_mode == 3) {
					tt_error_file() << type_name(follower) << "#" << follower->debug_counter 
									<< S(IDS_WAS_CACHED_AND_REMOVED) << type_name(this)
									<< S(IDS_CACHED_WITHOUT_FOLLOWERS) << endl;
				};
#endif
			};
			// otherwise cache invalidated elsewhere
//			follower->recursively_reset_cache_status();
		};
#if TT_DEBUG_ON
		if (debug_log(debug_counter,"Removing a follower to debug target.")) {
		   follower->print(tt_error_file());
			tt_error_file() << endl;
		} else if (debug_log(follower->debug_counter,"Debug target removed as a follower.")) {
			print(tt_error_file());
			tt_error_file() << endl;
      };
//		if (follower->pointer_to_background() == NULL && !prepared_for_deletion() && !blank && !follower->in_notebook() && warn_if_not_follower) {
//			say_error("Warning - about to orphan an object.");
//		};
#endif
      if (!tt_resetting_or_destroying_city) { // this conditional added on 210802 -- but how far scope?? - or is the bug elsewhere? - 
			// tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 but can cause memory errors while resetting during time travel
			// expermented with commenting this out on 160103 but it caused crashes while shutting down
      // following was in conditional above but caused timing dependency
		   follower->set_clean_status(DIRTY);
		}; // changed so only the above isn't done when shutting down - need the following if debugging leaks
		if (tt_reclaiming_memory) { // commented out on 280803 && !follower->prepared_for_deletion()) { // second conjunct new on 190103
			// since can be destroyed out of order when resetting during time travel
			followers = followers->remove(follower,&result);
#if TT_DEBUG_ON
			if (!result && warn_if_not_follower) {
			   say_error(IDS_PART_OF_SOMETHING_AND_NOT);
		   };
	//		return;
#endif
		} else { 
			result = TRUE; // else if shutting down don't care what the result is
		};
	};
	follower->set_leader(NULL);
	if (followers == NULL) {
		if (floor != NULL && !tt_resetting_or_destroying_city) {// && !leaders_removed()) { 
			// updated to use tt_resetting_or_destroying_city on 100603
			floor->remove_leader(this,FALSE);
		};
		set_followers_completely_inside(TRUE);
	};
	if (!follower->completely_inside_leader() || followers_absorbed()) {
		follower->set_completely_inside_leader(TRUE);
		set_clean_status(FOLLOWERS_DIRTY);
		follower->set_screen_updates(TRUE); // on its own now
	} else if (!tt_resetting_or_destroying_city) { // if need be -- no op otherwise
		// tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 but can cause memory errors while resetting during time travel
		follower->un_absorb_followers();
	};
	if (followers_cached() && !tt_resetting_or_destroying_city) {
		// tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 but can cause memory errors while resetting during time travel
		tt_screen->invalidate_cache();
// following introduces a timing dependency
//		set_clean_status(DIRTY);
	};
//	if (!tt_shutting_down) {
//		follower->recursively_set_background(NULL); // no where just now
//	};
//	follower->set_background(NULL); // new on 070105 since :add_follower may have set this and should restore
	follower->remove_from_floor(FALSE,FALSE); // rewrote the above on 070105 since floor should know about this
	// Ylva's crash report of 060105 required at least one of the above
	return(result);
};

void Sprite::add_outside_followers_to_screen() {
	if (!followers_completely_inside()) {
		Sprites *remaining = followers;
//		long sprite_priority = priority()-1;
		while (remaining != NULL) {
			Sprite *sprite = remaining->first();
			if (!sprite->completely_inside_leader()) { //screen_updates()) {
// not clear why such parts need to be part of background but
// do need to be on screen
// also hard to delete when shutting down (order dependent)
//				add_item(remaining->first(),give_priority);
//				sprite->set_priority(sprite_priority); // commented out on 151199 since messes up robot teams priorities
				tt_screen->add(sprite);
				sprite->add_outside_followers_to_screen(); // new on 161199 for robot teams for example
			};
			remaining = remaining->rest();
		};
	};
};

void Sprite::absorb_followers() {
	if (followers_completely_inside()) return; // needed recursively
	set_followers_absorbed(TRUE);
	set_followers_completely_inside(TRUE);
	Sprites *remaining = followers;
	while (remaining != NULL) {
		Sprite *follower = remaining->first();                    
		if (!follower->completely_inside_leader()) {
			// may have been removed by a recursive call to remove_item via absorb_followers 
			if (floor != NULL) floor->remove_item(follower,FALSE);
			follower->set_screen_updates(TRUE);
			follower->absorb_followers();
		};
		remaining = remaining->rest();
	};
};

void Sprite::un_absorb_followers() {
	if (!followers_absorbed()) return; // no need
	set_followers_absorbed(FALSE);
	Sprites *remaining = followers;
	while (remaining != NULL) {
		Sprite *follower = remaining->first();
		if (!follower->completely_inside_leader()) {
//			floor->add_item(follower,FALSE); // leave priority alone
			follower->set_screen_updates(FALSE);
			follower->un_absorb_followers();
			set_followers_completely_inside(FALSE);
		};
		remaining = remaining->rest();
	};
};

void Sprite::set_background(Background *new_background, boolean recur, boolean warn) {
	if (new_background == floor) return;
#if TT_DEBUG_ON
   if (debug_counter == tt_debug_target) {
      log(_T("Setting background of debug target."));
		if (new_background != NULL) tt_error_file() << "Background's debug number is " << new_background->debug_counter << endl;
   };
	if (new_background != NULL && new_background->prepared_for_deletion) { // typically when tt_debug_mode == 90400
		say_error("Setting background to a destroyed background.");
	};
	if (warn && floor != NULL && tt_deleting_background != floor && !floor->prepared_for_deletion && floor->has_item(this)) {
		log("Background still knows about item, but item is about to forget about background.",FALSE,TRUE);
	};
#endif
#if TT_CAREFUL
   // might not be needed anymore since changes of 141002 perhaps fix this...
	if (floor != NULL && new_background != NULL &&
		 floor != tt_deleting_background && floor->pointer_to_picture() == NULL && floor->has_item(this) ) { // re-ordered for efficiency on 130703
#if TT_DEBUG_ON
		if (warn) {
			say_error(IDS_GIVEN_NEW_BACKGROUND_WHILE_ON_FLOOR);
		};
#endif
		remove_from_floor(FALSE); // new on 150400
	};
#endif
//	if (tt_debug_mode == 2529) {
//		if (new_background != NULL && !is_picture() && kind_of() != PROGRAMMER) { // && kind_of() != MARTIAN && kind_of() != TALK_BALLOON) { 
			// Marty is treated specially (bad idea?)
//			if (!new_background->has_item(ultimate_leader())) {
//				log("Item thinks it is part of a background, but background doesn't know it.");
//			};
//		};
//	};
//   if (floor != NULL) set_priority_function_of_lly(floor->priority_function_of_lly());
	if (recur) {
		Sprites *remaining = followers;
		while (remaining != NULL) {
			Sprite *follower = remaining->first();
			follower->set_background(new_background,recur,FALSE);
			remaining = remaining->rest();
		};
	};
	// re-ordered the above and below on 090105 so that leaders list stays sorted with parts before leaders
	if (followers != NULL) {
		if (floor != NULL && !tt_resetting_or_destroying_city) {
			// tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 but can cause memory errors while resetting during time travel
			floor->remove_leader(this); // added !tt_shutting_down on 300300 since floor might already be deleted
		};
		if (new_background != NULL) new_background->add_leader(this,warn);
	};
	floor = new_background;
};

void Sprite::record_offsets() {
//	city_coordinate leader_llx, leader_lly;
//	leader->lower_left_corner(leader_llx,leader_lly);
	city_coordinate x,y;
//	leader->true_center(x,y);
	leader->lower_left_corner(x,y);
	// using true_width() etc below causes drift
	record_offsets(x-leader->true_x_offset(),y-leader->true_y_offset(),
						leader->current_width(),leader->current_height());
};

#if TT_MICROSOFT
// really any compiler that supports 64 bit integers
void Sprite::record_offsets(city_coordinate city_leader_llx,
									 city_coordinate city_leader_lly,
									 city_coordinate city_leader_width,
									 city_coordinate city_leader_height) {
// convert to pixel coordinates to avoid overflows
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Debug target offsets being recorded."));
	};
#endif
//	if (city_leader_width == 0 || city_leader_height == 0) {
	// commented out the following as experiment on 270103
	//if (city_leader_width < minimum_width || city_leader_height < minimum_height || // all tests were <= prior to 110101
	//	// changed the following from <= to < on 140103 since causes spurious warnings and the meaning of minimum is that it should be OK
	//	 width < minimum_width || height < minimum_height) { // revised on 061000 -- restored <= on 180702 since vacuuming up running stuff (e.g. Pong) then spitting distorts shapes significantly
	//	return; // new on 230200
	//};
	long x_offset,y_offset; // leader_width, leader_height; 
//	if (tt_log_version_number < 14) { // only pictures need this accuracy -- makes big cubbies not look as good
//		x_offset = ideal_horizontal_pixel_length(llx-city_leader_llx);
//      y_offset = ideal_vertical_pixel_length(lly-city_leader_lly);
//	} else {
		// I divide by 10 here to reduce the chance of overflows with large objects
		// but the old pixel stuff divided by 50 or 100 and introduced too much round off error
		// commented out the following on 080200 since using int64 now
//		city_coordinate x_offset10 = llx-city_leader_llx;
//		city_coordinate y_offset10 = lly-city_leader_lly;
//		if (tt_log_version_number > 14) {
//			if (x_offset10 > 0) {
//				x_offset10 += 5; // to round up
//			} else {
//				x_offset10 -= 5; // to round up for negative numbers
//			};
//			if (y_offset10 > 0) {
//				y_offset10 += 5; // to round up
//			} else {
//				y_offset10 -= 5; // to round up for negative numbers
//			};
//		};
//	   x_offset = x_offset10/10;
//	   y_offset = y_offset10/10;
		x_offset = llx-city_leader_llx;
		y_offset = lly-city_leader_lly;
//	};
//	if (tt_log_version_number < 15) {
//		leader_width = old_ideal_horizontal_pixel_length(city_leader_width);
//		leader_height = old_ideal_vertical_pixel_length(city_leader_height);
//	} else {
//		leader_width = ideal_horizontal_pixel_length(city_leader_width);
//		leader_height = ideal_vertical_pixel_length(city_leader_height);
//	};
//	if (leader_width == 0 || leader_height == 0) {
//		tt_error_file() << "Warning tried to record offsets to a zero size leader" << endl;
//		tt_exit_failure();
//		say("Somehow something is a part of something that is zero size.");
	// commented out the following on 100200 since using real width now
//		if (leader_width == 0) leader_width = 1;
//		if (leader_height == 0) leader_height = 1;
//	};
	// to avoid overflows shrink these a bit -- huh??
	if (leader_offsets == NULL) {
		leader_offsets = new Leader_Offsets();
	};
	leader_offsets->recorded_leader_width_squared = city_leader_width; // changed these on 100200 to use city_coordinates instead of pixels
	// re-written on 020500 since leader_offsets->recorded_leader_width_squared is int64 but city_leader_width is 32 bit
	leader_offsets->recorded_leader_width_squared = leader_offsets->recorded_leader_width_squared*leader_offsets->recorded_leader_width_squared;
	leader_offsets->recorded_leader_height_squared = city_leader_height;
	leader_offsets->recorded_leader_height_squared = leader_offsets->recorded_leader_height_squared*leader_offsets->recorded_leader_height_squared;
#if TT_DEBUG_ON
	if (leader_offsets->recorded_leader_width_squared < 0 || leader_offsets->recorded_leader_height_squared < 0) {
		log("Leader dimension squared is negative!");
	};
#endif
//	if (tt_log_version_number < 15) {
//		leader_offsets->width_times_leader_width 
//			 = old_ideal_horizontal_pixel_length(width)*leader_width;
//	} else {
//		leader_offsets->width_times_leader_width = ideal_horizontal_pixel_length(width)*leader_width;
//	};
//   if (leader_offsets->width_times_leader_width == 0 && tt_log_version_number > 12 && tt_log_version_number < 15) { 
		// this was commented out after log_version 7 -- why? 
		// disabled again because it meant that rubing out a width remote control (of a follower) would not set it to zero
		// but would become 1 or 2
//       leader_offsets->width_times_leader_width = leader_width;
//	};

//	if (tt_log_version_number < 15) {
//		leader_offsets->height_times_leader_height
//			 = old_ideal_vertical_pixel_length(height)*leader_height;
//	} else {
//		leader_offsets->height_times_leader_height = ideal_vertical_pixel_length(height)*leader_height;
//	};
	// these changed from pixels to city_coordinates on 100200
	// re-wrote these on 020500 to convert to int64 (if Microsoft compiler) and then multiply
	leader_offsets->width_times_leader_width = width;
	leader_offsets->width_times_leader_width = leader_offsets->width_times_leader_width*city_leader_width; 
	leader_offsets->height_times_leader_height = height;
	leader_offsets->height_times_leader_height = leader_offsets->height_times_leader_height*city_leader_height;
#if TT_DEBUG_ON
	if (tt_debug_mode == 100200 && 
		 (leader_offsets->width_times_leader_width <= 0 || leader_offsets->height_times_leader_height <= 0)) {
		log("Recording that an object is 0% the size of its container.");
	};
#endif
//	if (leader_offsets->height_times_leader_height == 0) {
//		if (tt_log_version_number > 12 && tt_log_version_number < 15) { // && tt_log_version_number < 7)
//			leader_offsets->height_times_leader_height = leader_height;
//		};
//#if TT_DEBUG_ON
//		if (kind_of() == CUBBY) { // just for now
//			print(tt_error_file());
//			tt_error_file() << " recording a relationship with leader's height of zero. Warning." << endl;
//		};
//#endif
//	};
	// offsets were from center
	// these changed from pixels to city_coordinates on 100200
	// re-wrote these on 020500 to convert to int64 (if Microsoft compiler) and then multiply
	leader_offsets->x_offset_times_leader_width = x_offset;
	leader_offsets->x_offset_times_leader_width = leader_offsets->x_offset_times_leader_width*city_leader_width; 
//		 = (llx-(leader_llx+leader_width/2))*leader_width;
	leader_offsets->y_offset_times_leader_height = y_offset;
	leader_offsets->y_offset_times_leader_height = leader_offsets->y_offset_times_leader_height*city_leader_height;
//		 = (lly-(leader_lly+leader_height/2))*leader_height;
};
#else
void Sprite::record_offsets(city_coordinate city_leader_llx,
									 city_coordinate city_leader_lly,
									 city_coordinate city_leader_width,
									 city_coordinate city_leader_height) {
// convert to pixel coordinates to avoid overflows
	long x_offset,y_offset;
//	if (tt_log_version_number < 14) { // only pictures need this accuracy -- makes big cubbies not look as good
//		x_offset = ideal_horizontal_pixel_length(llx-city_leader_llx);
//      y_offset = ideal_vertical_pixel_length(lly-city_leader_lly);
//	} else {
		// I divide by 10 here to reduce the chance of overflows with large objects
		// but the old pixel stuff divided by 50 or 100 and introduced too much round off error
	   x_offset = (llx-city_leader_llx)/10;
	   y_offset = (lly-city_leader_lly)/10;
//	};
	long leader_width = ideal_horizontal_pixel_length(city_leader_width);
	long leader_height = ideal_vertical_pixel_length(city_leader_height);
//	if (leader_width == 0 || leader_height == 0) {
//		tt_error_file() << "Warning tried to record offsets to a zero size leader" << endl;
//		tt_exit_failure();
//		say("Somehow something is a part of something that is zero size.");
		if (leader_width == 0) leader_width = 1;
		if (leader_height == 0) leader_height = 1;
//	};
	// to avoid overflows shrink these a bit -- huh??
	if (leader_offsets == NULL) {
		leader_offsets = new Leader_Offsets();
	};
	leader_offsets->recorded_leader_width = leader_width;
	leader_offsets->recorded_leader_height = leader_height;
	leader_offsets->width_times_leader_width = ideal_horizontal_pixel_length(width)*leader_width;
   if (leader_offsets->width_times_leader_width == 0) { // this was commented out after log_version 7 -- why?  && tt_log_version_number > 12
       leader_offsets->width_times_leader_width = leader_width;
	};
	leader_offsets->height_times_leader_height = ideal_vertical_pixel_length(height)*leader_height;
   if (leader_offsets->height_times_leader_height == 0) { // && tt_log_version_number < 7)  && tt_log_version_number > 12
      leader_offsets->height_times_leader_height = leader_height;
	};
	// offsets were from center
	leader_offsets->x_offset_times_leader_width = x_offset*leader_width;
//		 = (llx-(leader_llx+leader_width/2))*leader_width;
	leader_offsets->y_offset_times_leader_height = y_offset*leader_height;
//		 = (lly-(leader_lly+leader_height/2))*leader_height;
};
/*
void Sprite::record_offsets(city_coordinate city_leader_llx,
							 city_coordinate city_leader_lly,
									 city_coordinate city_leader_width,
									 city_coordinate city_leader_height) {
// convert to pixel coordinates to avoid overflows
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Debug target offsets being recorded."));
	};
#endif
	long x_offset,y_offset; // leader_width, leader_height; 
	if (tt_log_version_number < 14) { // only pictures need this accuracy -- makes big cubbies not look as good
		x_offset = ideal_horizontal_pixel_length(llx-city_leader_llx);
      y_offset = ideal_vertical_pixel_length(lly-city_leader_lly);
	} else {
		// I divide by 10 here to reduce the chance of overflows with large objects
		// but the old pixel stuff divided by 50 or 100 and introduced too much round off error
		// commented out the following on 080200 since using int64 now
		city_coordinate x_offset10 = llx-city_leader_llx;
		city_coordinate y_offset10 = lly-city_leader_lly;
//		if (tt_log_version_number > 14) {
		if (x_offset10 > 0) {
      	x_offset10 += 5; // to round up
			} else {
				x_offset10 -= 5; // to round up for negative numbers
			};
			if (y_offset10 > 0) {
				y_offset10 += 5; // to round up
			} else {
				y_offset10 -= 5; // to round up for negative numbers
			};
//		};
	   x_offset = x_offset10/10;
	   y_offset = y_offset10/10;

	};
//	if (tt_log_version_number < 15) {
//		leader_width = old_ideal_horizontal_pixel_length(city_leader_width);
//		leader_height = old_ideal_vertical_pixel_length(city_leader_height);
//	} else {
		long leader_width = ideal_horizontal_pixel_length(city_leader_width);
		long leader_height = ideal_vertical_pixel_length(city_leader_height);
//	};
//	if (leader_width == 0 || leader_height == 0) {
//		tt_error_file() << "Warning tried to record offsets to a zero size leader" << endl;
//		tt_exit_failure();
//		say("Somehow something is a part of something that is zero size.");
	// commented out the following on 100200 since using real width now
//		if (leader_width == 0) leader_width = 1;
//		if (leader_height == 0) leader_height = 1;
//	};
	// to avoid overflows shrink these a bit -- huh??
	if (leader_offsets == NULL) {
		leader_offsets = new Leader_Offsets();
	};
	leader_offsets->recorded_leader_width = leader_width;
	leader_offsets->recorded_leader_height = leader_height;
//	if (tt_log_version_number < 15) {
//		leader_offsets->width_times_leader_width 
//			 = old_ideal_horizontal_pixel_length(width)*leader_width;
//	} else {
		leader_offsets->width_times_leader_width = ideal_horizontal_pixel_length(width)*leader_width;
//	};
//   if (leader_offsets->width_times_leader_width == 0 && tt_log_version_number > 12 && tt_log_version_number < 15) { 
		// this was commented out after log_version 7 -- why? 
		// disabled again because it meant that rubing out a width remote control (of a follower) would not set it to zero
		// but would become 1 or 2
//       leader_offsets->width_times_leader_width = leader_width;
//	};

//	if (tt_log_version_number < 15) {
//		leader_offsets->height_times_leader_height
//			 = old_ideal_vertical_pixel_length(height)*leader_height;
//	} else {
		leader_offsets->height_times_leader_height = ideal_vertical_pixel_length(height)*leader_height;
//	};
//	if (leader_offsets->height_times_leader_height == 0) {
//		if (tt_log_version_number > 12 && tt_log_version_number < 15) { // && tt_log_version_number < 7)
//			leader_offsets->height_times_leader_height = leader_height;
//		};

//	};
	// offsets were from center
	leader_offsets->x_offset_times_leader_width = x_offset*leader_width; // (llx-(leader_llx+leader_width/2))*leader_width;
	leader_offsets->y_offset_times_leader_height = y_offset*leader_height; // (lly-(leader_lly+leader_height/2))*leader_height;
};
*/
#endif

flag reported_lack_of_leader_offsets = FALSE;

void Sprite::follow_leader_changes(city_coordinate leader_width,
											  city_coordinate leader_height,
											  city_coordinate leader_x,
											  city_coordinate leader_y,
											  ChangeReason reason) {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Debug target leader changes about to be followed."));
	};
#endif
	if (leader_width == 0 || leader_height == 0) { // new on 100200
		// changed on 031000 so if either dimension of leader is zero then both dimensions of follower are zero
		set_size(0,0,reason);
		return;
	};
// following moved here from start of procedure to deal with 0 width or height leaders better
#if TT_CAREFUL
	if (leader_offsets == NULL) {
		if (leader != NULL) {
			leader->refit_contents(); // new on 150300 if cubby or bird or the like try to fix things
		};
		if (leader_offsets == NULL) { // still have a problem
			if (!reported_lack_of_leader_offsets) { // updated on 150300 so only warned once
				log("Lost track of relative size of parts and whole. Some objects might be very tiny."); // updated on 060300 since more accurate than IDS_PART_BUT_OF_WHO);
				reported_lack_of_leader_offsets = TRUE;
			};
			// following was 1,1 prior to 151000
			set_size(minimum_width,minimum_height,reason); // new on 031000 since otherwise can have garbage for size and might dump it as such
			return;
		};
	};
#endif
//#if TT_DEBUG_ON
//	// 250803
//	if (leader->kind_of() == COPIER) {
//		tt_error_file() << "Copier is at " << leader_x << "," << leader_y << " and its size is " << leader_width << "x" << leader_height << " on frame " << tt_frame_number << endl;
//		tt_error_file() << "Button is at " << llx << "," << lly << " and its size is " << width << "x" << height << endl;
//	};
//#endif
	leader_offsets->follow_leader_changes(this,
													  leader_width,leader_height,
													  leader_x,leader_y,reason);
//#if TT_DEBUG_ON
//	// 250803
//	if (leader->kind_of() == COPIER) {
//		tt_error_file() << "Button is at " << llx << "," << lly << " and its size is " << width << "x" << height << " afterwards." << endl;
//		leader_offsets->follow_leader_changes(this,
//														leader_width,leader_height,
//														leader_x,leader_y,reason);
//	   tt_error_file() << "Button is at " << llx << "," << lly << " and its size is " << width << "x" << height << " repeated." << endl;
//	};
//#endif
	// for debuggin
//	if (code == SYNTHETIC_SPRITE) {
//		tt_error_file() << "Frame=" << tt_frame_number
//						<< " leader is " << leader_width << "x" << leader_height
//						<< " rectangle is " << width << "x" << height << endl;
//	};
//   changes_propagated_flag = FALSE; // in case this in turn has followers
};

void Sprite::follow_leader_size_changes(city_coordinate leader_width, city_coordinate leader_height,
													 ChangeReason reason) {
#if TT_CAREFUL
	if (leader_offsets == NULL) {
//		say_error(IDS_PART_BUT_OF_WHO);
		log("Something is too tiny for ToonTalk to figure out its real size. Might stay too small to see.");
		return;
	};
#endif
	leader_offsets->follow_leader_size_changes(this,leader_width,leader_height,reason);
};

boolean Sprite::ok_to_temporarily_reset_size() { // moved here (and made into a procedure) on 051100
	return(!tt_dumping_to_test_equality_or_describe && leader != NULL && (!leader->is_picture() || leader->is_flipped()));
};

void Sprite::set_cause_of_match_failure(boolean new_flag, boolean recur) { // new on 250302
   cause_of_match_failure_flag = (flag) new_flag;
   if (recur && followers != NULL) {
      followers->set_cause_of_match_failure(new_flag,recur);
   };
};

House *Sprite::pointer_to_house() { // new on 020405
	if (floor == NULL) {
		return(NULL);
	} else {
		return(floor->pointer_to_house());
	};
};

void Sprite::compute_dimensions_to_fit(city_coordinate goal_width,
													city_coordinate goal_height,
													city_coordinate &compute_full_width,
													city_coordinate &compute_full_height) {
//													city_coordinate &delta_x,
//													city_coordinate &delta_y) {
	city_coordinate full_llx, full_lly;
	full_size_and_location(compute_full_width,compute_full_height,
								  full_llx,full_lly);
	if (compute_full_width == 0 || compute_full_height == 0) {
//		tt_error_file() << "Zero size! (in sprite.cpp)" << endl;
//		say("ToonTalk is having troubles because something has zero size.");
		compute_full_width = tile_width;
		compute_full_height = tile_height;
//		delta_x = 0;
//		delta_y = 0;
		return;
	};
	long x_factor = (1000*goal_width)/compute_full_width;
	long y_factor = (1000*goal_height)/compute_full_height;
//	long factor = min(x_factor,y_factor);
	compute_full_width = (width*x_factor)/1000; // future width
	compute_full_height = (height*y_factor)/1000;
//	if (factor == y_factor) {
//		delta_x = (goal_width-full_width)/2; // always zero
//		delta_y = 0;
//	} else {
//		delta_x = 0;
//		delta_y = (goal_height-full_height)/2;
//	};
};

boolean Sprite::full_region_including_outside_followers(TTRegion &region) {
	city_coordinate full_width,full_height;
	if (true_size_and_location(full_width,full_height,region.min_x,region.min_y)) {
		region.max_x = region.min_x+full_width;
		region.max_y = region.min_y+full_height;
		return(TRUE);
	} else {
		return(FALSE);
	};
};

boolean Sprite::full_size_and_location(city_coordinate &computed_full_width,
													city_coordinate &computed_full_height,
													city_coordinate &full_llx,
													city_coordinate &full_lly) {
	if (leader_offsets == NULL && leader != NULL) {
		return(FALSE); // new on 301100 - not reliable numbers and very small anyway
	};
	// if sprite has followers outside of its bounding box include them
	computed_full_width = width;
	computed_full_height = height;
	full_llx = llx;
	full_lly = lly;
	if (followers_completely_inside() && !followers_absorbed()) return(TRUE);
	city_coordinate urx = llx+computed_full_width;
	city_coordinate ury = lly+computed_full_height;
	Sprites *remaining = followers;
	while (remaining != NULL) {
		city_coordinate follower_width, follower_height,
							 follower_llx, follower_lly;
		if (remaining->first()->full_size_and_location(follower_width,follower_height,follower_llx,follower_lly)) {
			// made a conditional on 301100 since if very small might not have valid values
			if (full_llx > follower_llx) full_llx = follower_llx;
			if (full_lly > follower_lly) full_lly = follower_lly;
			city_coordinate new_urx = follower_llx+follower_width;
			city_coordinate new_ury = follower_lly+follower_height;
			if (new_urx > urx) urx = new_urx;
			if (new_ury > ury) ury = new_ury;
		};
		remaining = remaining->rest();
	};
	computed_full_width = urx-full_llx;
	computed_full_height = ury-full_lly;
	return(TRUE);
};

boolean Sprite::true_size_and_location(city_coordinate &computed_full_width,
													city_coordinate &computed_full_height,
													city_coordinate &full_llx,
													city_coordinate &full_lly) {
	if (leader_offsets == NULL && leader != NULL) {
		return(FALSE); // new on 301100
	};
	// if sprite has followers outside of its bounding box include them
	computed_full_width = true_width(); //width
	computed_full_height = true_height(); //height
	full_llx = llx;
	full_lly = lly;
	if (followers_completely_inside() && !followers_absorbed()) return(TRUE);
	city_coordinate urx = llx+computed_full_width;
	city_coordinate ury = lly+computed_full_height;
	Sprites *remaining = followers;
	while (remaining != NULL) {
		city_coordinate follower_width, follower_height,
							 follower_llx, follower_lly;
		if (remaining->first()->full_size_and_location(follower_width,follower_height,follower_llx,follower_lly)) { // conditional new on 301100
			if (full_llx > follower_llx) full_llx = follower_llx;
			if (full_lly > follower_lly) full_lly = follower_lly;
			city_coordinate new_urx = follower_llx+follower_width;
			city_coordinate new_ury = follower_lly+follower_height;
			if (new_urx > urx) urx = new_urx;
			if (new_ury > ury) ury = new_ury;
		};
		remaining = remaining->rest();
	};
	computed_full_width = urx-full_llx;
	computed_full_height = ury-full_lly;
	return(TRUE);
};

boolean Sprite::inside_vacuum() {
#if TT_DEBUG_ON
   // updated this on 160702 but left debugging version to check all is OK
   // since this flag is set recursively there is no need to recursively access it as well
	if (inside_vacuum_flag) return(TRUE);
	if (leader != NULL) {
      if (leader->inside_vacuum() && leader->kind_of() != PROGRAM_PAD) { // since notebooks don't recursively set this flag
         debug_this("Problem with inside_vacuum() - leader is in vacuum but I'm not.");
      };
	};
	return(FALSE);
#else
   return(inside_vacuum_flag); // could inline this too
#endif
};

boolean given_stack_help = FALSE;

void Sprite::generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection) {
	if (tt_help_counter == 0 && history_full()) return; // user has already done everything
	boolean pointing_described = FALSE;
	if (tt_histories[history_index()] == 0 || tt_help_counter == 1) {
		// user hasn't done anything with these or directed help
		describe_pointing(message,tool_in_hand,selection);
		pointing_described = TRUE;
	};
	if ((!did_user(USER_HAS_PICKED_UP) && tt_help_counter == 0)
		 || (tt_help_counter == 1 && tool_in_hand != this)) {
		message << S(IDS_PICK_UP_HELP);
		if (pointing_described && !infinite_stack()) { 
//			if (infinite_stack()) {
//				message << S(IDS_ONE);
//			} else {
			   message << space(SPACE_BEFORE_PRONOUN); // new on 050999
            print_it(kind_of(),message);
//			};
		} else {
			message << space();
			describe_type(message,INDEFINITE_ARTICLE);
		};
//		message << " " << S(IDS_UP);
		message << space() << S(IDS_PICK_UP_HELP2,"") << space();
		describe_left_button(message);
		message << S(IDS_PICK_UP_HELP3,""); // added on 080200
		message << end_sentence();
	};
};

boolean Sprite::history_full() {
	return(type_history_full(history_index()));
};

boolean Sprite::did(int action) {
	return(did_user(action,history_index()));
};

void Sprite::doing(int action, Sprite *by) {
   user_did(history_index(),action,by);
};

void Sprite::java_describe_as_comment(java_stream &out, const_java_string preface) {
   const int comment_length = 1024; // was 4096 prior to 081200 but long comments resulted
#if TT_NEW_IO
	output_string_stream comment_out;
#else
   char *comment = new char[comment_length];
   output_string_stream comment_out(comment,comment_length);
#endif
   if (preface != NULL) comment_out << preface;
	tt_current_output_stream = &comment_out; // only used within describe_x calls below this
   tt_current_output_stream_size = comment_length;
   top_level_describe(comment_out,english()?PLURAL_ARTICLE:INDEFINITE_ARTICLE); // was just describe prior to 081200
   comment_out << ".";
   comment_out.put('\0');
	comment[comment_length-1] = '\0'; // added on 030201 since the stream might be full so the above doesn't nothing and the following expects a null-terminated string
	print_as_comment(out,comment,comment_length); // removed comment_out.STR from many Java procedures on 010799
#if !TT_NEW_IO
   delete [] comment;
#endif
	tt_current_output_stream = NULL; // new on 021204
};

void Sprite::java_flipped_followers(java_stream &out, const_java_string ) { // new on 031200
	Sprites *remaining = pointer_to_followers();
	while (remaining != NULL) {
		remaining->first()->java_flipped_followers(out,NULL);
		remaining = remaining->rest();
	};
};

Sprites *Sprite::reachable_sprites_of_type(SpriteType type) { // new on 031200 - generalized 041200
//	if (kind_of() == type) { 
//		return(new Sprites(this)); // this can be of type and also have as reachable more of this type
//	};
	Sprites *answer = NULL;
	if (kind_of() == type) { // new on 180201
		answer = new Sprites(this);
	};
	Sprites *remaining = pointer_to_followers();
	while (remaining != NULL) {
		Sprite *follower = remaining->first();
		if (answer == NULL) {
         answer = follower->reachable_sprites_of_type(type);
      } else {
         answer->last_cons()->set_rest(follower->reachable_sprites_of_type(type));
      };
		remaining = remaining->rest();
	};
	return(answer);
};

Sprites *Sprite::reachable_non_equal_sprites_of_type(SpriteType type) { // new on 250601
	Sprites *answer = NULL;
	if (kind_of() == type) { // new on 180201
		answer = new Sprites(this);
	};
	Sprites *remaining = pointer_to_followers();
	while (remaining != NULL) {
		Sprite *follower = remaining->first();
		Sprites *more = follower->reachable_non_equal_sprites_of_type(type);
		insert_sprites_at_end_if_not_equal_member(answer,more);
		remaining = remaining->rest();
	};
	return(answer);
};

void Sprite::top_level_describe(output_stream &message, Article article) {
	// currently only called while debugging to display item's description
	UnwindProtect<output_stream*> save(tt_current_output_stream);
	// commented out the following since it broke the Java applet generator since nest_indices were broken by this
//	short int saved_tt_loads_current = tt_loads_current; // new on 050600 - important at least while debugging
//	tt_loads_current = 0;
	UnwindProtect<SpritePointer*> set(tt_sprites_being_dumped,NULL); // new on 050600 - important at least while debugging
	UnwindProtect<int> set2(tt_sprites_being_dumped_size,0);
	UnwindProtect<int> set3(tt_number_of_sprites_dumped_so_far,0);
	UnwindProtect<boolean*> set4(tt_sprites_being_dumped_ok_for_eq,NULL);
#if TT_DEBUG_ON
   if (tt_debug_mode == 150702) {
      tt_error_file() << "[2] tt_sprites_being_dumped_size=" << tt_sprites_being_dumped_size << "; tt_number_of_sprites_dumped_so_far=" << tt_number_of_sprites_dumped_so_far << endl;
   };
#endif
	// bother setting and saving tt_current_output_stream_size??
	tt_current_output_stream = &message;
	describe(message,article);
	tt_current_output_stream = NULL; // new on 021204
//	just_destroy_list(tt_sprites_being_dumped);
	if (tt_sprites_being_dumped != NULL) delete [] tt_sprites_being_dumped; // revised on 270900
	if (tt_sprites_being_dumped_ok_for_eq != NULL) { // new on 240601
		delete [] tt_sprites_being_dumped_ok_for_eq;
	};
#if TT_DEBUG_ON
   if (tt_debug_mode == 150702) {
      tt_error_file() << "[3] tt_sprites_being_dumped_size=" << tt_sprites_being_dumped_size << "; tt_number_of_sprites_dumped_so_far=" << tt_number_of_sprites_dumped_so_far << endl;
   };
#endif
};

void Sprite::describe(output_stream &message, Article article) {
	if (infinite_stack()) {
//      if (english()) {
// 		   print_article(article,message,S(IDS_STACK_OF));
//      } else { // really ok for most languages??
//         message << S(IDS_STACK_OF);
//      };
      print_with_article(article,IDS_STACK_OF,kind_of(),message);
		describe_type(message,PLURAL_ARTICLE);
		print_with_article(article,IDS_STACK_OF2,kind_of(),message,FALSE,""); // ignore if not defined
		return;
	};
//	describe_comment(message); -- will be described as part of the encoding
	string buffer = new character[max_encoding_buffer_size];
	output_string_stream encoding_out(buffer,max_encoding_buffer_size);
	UnwindProtect<boolean> set(tt_dumping_to_test_equality_or_describe,TRUE);
	dump_started();
	dump(encoding_out);
   dump_ended(); // release_sprites_being_dumped();
#if TT_NEW_IO
   long length = encoding_out.str().length();
#else
	long length = encoding_out.pcount(); // this is not reliable in VC++ (ok in BC++)
#endif
   if (length >= max_encoding_buffer_size) {
      tt_error_file() << "Too long a description of ";
      describe_type(tt_error_file(),INDEFINITE_ARTICLE);
      tt_error_file() << endl;
   };
	StringInputStream encoding_in(buffer,length);
	try { // try is new as of 030105
		describe_encoding((SpriteType) encoding_in.get(),&encoding_in,message,latest_version_number,
								contained_in(THOUGHT_BUBBLE),article);	   
	} catch (...) {
		message << " (Warning: internal error encountered in ToonTalk while generating this description. Sorry.) " << endl;
		tt_error_file() << "Error while running describe_encoding" << endl;
	};
	delete [] buffer;
};

void Sprite::describe_type(output_stream &text_out, Article article, boolean , boolean capitalize) {
	print_type(fine_kind_of(),text_out,article,capitalize); // was ordinary kind_of()
};

string Sprite::return_encoding(long &length, boolean trim, boolean include_version_number, string provided_buffer) {
	length = max_encoding_buffer_size;  // should be more than enough
	string buffer;
	if (provided_buffer != NULL) {
		buffer = provided_buffer; // new on 050500
	} else {
		buffer = new character[length];
	};
	string remaining_buffer;
	if (include_version_number) {
		remaining_buffer = buffer+2; // includes version number and language
		buffer[0] = latest_version_number;
		buffer[1] = tt_language;
	} else {
		remaining_buffer = buffer;
	};
	output_string_stream encoding(remaining_buffer,length-1);
	// commented out on 091000 so you get this warning only once per session (but log will say it all)
//	tt_warning_count = 0; // new on 061199 so that warnings about user files and the like aren't repeated too many times
	dump_started();
	// saving and reseting all this is new on 110601
//	boolean saved_tt_dump_so_equals_share = tt_dump_so_equals_share;
//	tt_dump_so_equals_share = FALSE;
//	boolean saved_tt_dump_so_equals_share_enabled = tt_dump_so_equals_share_enabled;
	UnwindProtect<SpritePointer *> set(tt_sprites_being_dumped,NULL);
	UnwindProtect<int> set2(tt_sprites_being_dumped_size,0);
//	int saved_tt_skip_first_n_dumped = tt_skip_first_n_dumped;
//	tt_skip_first_n_dumped = 0;
	UnwindProtect<int> set3(tt_number_of_sprites_dumped_so_far,0);
	UnwindProtect<boolean*> set4(tt_sprites_being_dumped_ok_for_eq,NULL);
#if TT_DEBUG_ON
   if (tt_debug_mode == 150702) {
      tt_error_file() << "[4] tt_sprites_being_dumped_size=" << tt_sprites_being_dumped_size << "; tt_number_of_sprites_dumped_so_far=" << tt_number_of_sprites_dumped_so_far << endl;
   };
#endif
	dump(encoding); // was just dump prior to 050601 -- is back to dump on 110601 since deal with context explictily here
//	tt_dump_so_equals_share = saved_tt_dump_so_equals_share;
//	tt_dump_so_equals_share_enabled = saved_tt_dump_so_equals_share_enabled;
	if (tt_sprites_being_dumped != NULL) {
		delete [] tt_sprites_being_dumped;
	};
	if (tt_sprites_being_dumped_ok_for_eq != NULL) {
		delete [] tt_sprites_being_dumped_ok_for_eq;
	}; 
#if TT_DEBUG_ON
   if (tt_debug_mode == 150702) {
      tt_error_file() << "[1] tt_sprites_being_dumped_size=" << tt_sprites_being_dumped_size << "; tt_number_of_sprites_dumped_so_far=" << tt_number_of_sprites_dumped_so_far << endl;
   };
#endif
   dump_ended(); //release_sprites_being_dumped();
#if TT_NEW_IO
	length = encoding.str().length(); -- really? there can be 0s in the string
#else
	length = encoding.tellp(); // replaced with tellp on 060601 since pcount is not reliable in VC++ (ok in BC++) -- didn't seem to matter...
#endif
   if (length >= max_encoding_buffer_size) { 
		string text = new character[256];
		output_string_stream text_message(text,256);
      describe_type(text_message,INDEFINITE_ARTICLE);
		text_message << space() << S(IDS_TOO_BIG_TO_SAVE);
		text_message.put('\0');
		Text *text_pad = new Text();
		text_pad->set_text(text);
		text_pad->set_use_variable_width_font(TRUE); // new on 150799
#if TT_WIDE_TEXT
		delete [] text;
#endif
		text_pad->set_color(tt_gray); // red would be better
		output_string_stream encoding(buffer,length);
		text_pad->dump(encoding);
#if TT_NEW_IO
		length = encoding.str().length();
#else
		length = encoding.pcount();
#endif
		text_pad->destroy();
   };
	if (!tt_dumping_to_test_equality_or_describe) { // condition is a hack to prevent debugging (via describe) from interfering with
		// generating Java applets
		release_nest_indices(); // in case built up table to keep bird/nest correspondences
	};
	if (include_version_number) {
		length += 2; // version and language
		return(buffer);
	};
	if (!trim) return(buffer);
	// following doesn't support include_version_number
	encoding.put('\0'); // added 070199
	const_string original_encoding_string = encoding.STR;
	string encoding_string = new character[length]; // just the size needed
	memcpy(encoding_string,original_encoding_string,length*sizeof(character));
	delete [] buffer; // does trim make sense together with provided_buffer??
	return(encoding_string);
};


/*
void Sprite::recursively_move_by(city_coordinate delta_x,
                                 city_coordinate delta_y) {
   move_by(delta_x,delta_y);
   Sprites *remaining = followers;
   while (remaining != NULL) {
      followers->first()->recursively_move_by(delta_x,delta_y);
      remaining = remaining->rest();
   };
};
*/

void Sprite::animate_size_and_location(city_coordinate new_width,
													city_coordinate new_height,
													city_coordinate new_llx,
													city_coordinate new_lly,
													millisecond expansion_duration,
//													boolean *status,
													Sprite *notify,
													AnimationCallBack callback,
													boolean signal_changes) {
	animate_size(new_width,new_height,expansion_duration,// status,
					 NULL,CALLBACK_NOTHING,FALSE);
	animate_location(new_llx,new_lly,expansion_duration,// status,
	                 notify,callback,signal_changes);
};

void Sprite::insert_animation(Animation *new_animation) {
	if (tt_resetting_or_destroying_city) {
		if (tt_reclaiming_memory) { // new on 180103
			delete new_animation;
		};
		return; // new on 040601 so can't get into run_animations infinite recursion when quitting (e.g. from speak_next_sentence_callback)
	};
	if (animations == NULL) {
		animations = new_animation;
	} else {
		if (running_animations) { // can't change list while using it
			if (new_animations == NULL) {
				new_animations = new_animation;
			} else { // keep in order in case no other animations left
				new_animations = new_animations->insert(new_animation);
			};
		} else {
			animations = animations->insert(new_animation);
		};
	};
};

void Sprite::remove_animations_of(Sprite *sprite) {
   if (animations != NULL) {
      animations = animations->remove_all_of(sprite);
   };
};

void Sprite::do_after(millisecond duration, Sprite *notify, AnimationCallBack callback) {
	if (duration <= 0) { // should I add || tt_finish_all_animations??
      AnimationCallBackProcedure procedure = callback_procedure(callback);
		procedure(notify);
	} else {
#if TT_DEBUG_ON
		// commented out on 161105
//		if (!showing_on_a_screen() && this != tt_global_picture && !house_sprite() && !outside_truck() && this != tt_call_for_marty 
//			 && (kind_of() == PICTURE || !is_picture()) && !is_time_travel_button(this) && kind_of() != TALK_BALLOON) {
//			debug_this("animating while off screen");
//		};
		if (tt_debug_mode == 70803) {
			tt_error_file() << "do_after callback# " << callback << " on frame " << tt_frame_number << endl;
		};
#endif
		insert_animation(new Do_After(this,tt_current_time+duration,notify,callback));
	};
};

void Sprite::animate_size(city_coordinate new_width,
								  city_coordinate new_height,
								  millisecond expansion_duration,
//								  boolean *status,
								  Sprite *notify,
								  AnimationCallBack callback,
								  boolean signal_change) {
//	set_clean_status(DIRTY);
	if (new_width < minimum_width) { // new on 201100
		new_width = minimum_width;
	};
	if (new_height < minimum_height) { // new on 201100
		new_height = minimum_height;
	};
	boolean width_change = (width != new_width);
	boolean height_change = (height != new_height);
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Debug target size animated."));
	};
#endif
	if (width_change) {
		if (height_change) {
			add_ease_in_and_out_to_goal(ANIMATING_WIDTH,new_width,expansion_duration,//status,
									          notify,CALLBACK_NOTHING); // only call back once
		} else {
			add_ease_in_and_out_to_goal(ANIMATING_WIDTH,new_width,expansion_duration,//status,
									          notify,callback);
		};
	};
	if (height_change) {
		add_ease_in_and_out_to_goal(ANIMATING_HEIGHT,new_height,expansion_duration,// status,
											 notify,callback);
	};
	if (signal_change) {
		set_size_and_location(width,height,llx,lly,ANIMATION_STARTING); // new on 110201 so object knows this is starting
		do_after(expansion_duration,this,SIGNAL_SIZE_AND_LOCATION_CALLBACK);
	};
	if (!width_change && !height_change) {
//		if (status != NULL) {
//			true_after(expansion_duration,status);
//		};
		if (callback != NULL) {
			do_after(expansion_duration,notify,callback);
		};
	};
};

void Sprite::animate_location(city_coordinate new_llx,
										city_coordinate new_lly,
										millisecond duration,
//										boolean *status,
										Sprite *notify,
										AnimationCallBack callback,
										boolean signal_change) {
	// is this called much with duration == 0? And if so worth optimizing? asked 260803
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target || (tt_debug_mode == 240804 && kind_of() == CUBBY && leader != NULL && leader->kind_of() == THOUGHT_BUBBLE)) {
		tt_error_file() << "Debug target animating to " << new_llx << "," << new_lly << " on frame=" << tt_frame_number << endl;
	};
#endif
	// compute them here since if duration is zero maybe be changed by below
	boolean x_change = new_llx != llx;
	boolean y_change = new_lly != lly;
	if (x_change) {
		if (y_change) {
			// just call backback once both are done
			add_ease_in_and_out_to_goal(ANIMATING_LLX,new_llx,duration,notify,CALLBACK_NOTHING);
		} else {
			add_ease_in_and_out_to_goal(ANIMATING_LLX,new_llx,duration,notify,callback);
		};
	};
	if (y_change) {
		add_ease_in_and_out_to_goal(ANIMATING_LLY,new_lly,duration,notify,callback);
	};
	if (signal_change) {
		set_size_and_location(width,height,llx,lly,ANIMATION_STARTING); // new on 150201 so object knows this is starting
		// if duration == 0 important to do this before 
		// other callback since this may trigger change propagation
		do_after(duration,this,SIGNAL_SIZE_AND_LOCATION_CALLBACK);
	};
	if (!x_change && !y_change) {
//		if (status != NULL) {
//			true_after(duration,status);
//		};
		if (callback != NULL) {
			do_after(duration,notify,callback);
		};
	};
	if ((x_change || y_change) && duration == 0) {
		set_clean_status(MOVED);
	};
};

const millisecond minimum_animation_duration = 33;


void Sprite::add_ease_in_and_out_to_goal(AnimatingVariable variable, long goal,
													  millisecond duration,
//													  boolean *set_when_finished,
													  Sprite *notify,
													  AnimationCallBack callback) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("About to ease in and out of goal of debug target.");
	};
#endif
	if (duration < minimum_animation_duration) {
      set_animating_variable(this,variable,goal,TRUE);
//      if (set_when_finished != NULL) {
//         *set_when_finished = TRUE;
//      };
      if (callback != NULL) {
         AnimationCallBackProcedure procedure = callback_procedure(callback);
			procedure(notify);
      };
      return;
	};
#if TT_DEBUG_ON
	// commented out the following on 161105 since seem to have found most of these anomalies
	// and it is appropriate for this to happen while removing a follower from the back of a picture
//	if (!showing_on_a_screen() && !outside_truck() && (kind_of() == PICTURE || !is_picture()) && !is_time_travel_button(this)) {
//		debug_this("animating while off screen");
//	};
	if (tt_debug_mode == 33) {
      tt_error_file() << C(IDS_ANIMATING_TO_GOAL) << goal << " at " << tt_frame_number << endl;
   };
	if (tt_debug_mode == 70803 && callback != CALLBACK_NOTHING) {
		tt_error_file() << "add_ease_in_and_out_to_goal callback# " << callback << " on frame " << tt_frame_number << endl;
	};
#endif
	insert_animation(new EaseInAndOut_To_Static_Goal(this,variable,goal,duration,notify,callback));
};

void Sprite::add_ease_in_and_out_to_goal(AnimatingVariable variable,
                                         Sprite *other_sprite,
                                         AnimatingVariable other_variable,
										           millisecond duration,
            //									  boolean *set_when_finished,
										           Sprite *notify,
										           AnimationCallBack callback,
										           long offset) {
	if (duration < minimum_animation_duration) {
//		*variable = *goal+offset;
      set_animating_variable(this,variable,animating_variable_value(other_sprite,other_variable)+offset,TRUE);
//		if (set_when_finished != NULL) {
//			*set_when_finished = TRUE;
//		};
		if (callback != NULL) {
         AnimationCallBackProcedure procedure = callback_procedure(callback);
			procedure(notify);
		};
		return;
	};
#if TT_DEBUG_ON
	if (!showing_on_a_screen() && !outside_truck()) {
		debug_this("animating while off screen");
	};
	if (tt_debug_mode == 33) {
		tt_error_file() << C(IDS_ANIMATING_TO_VARIABLE_GOAL) << tt_frame_number << endl;
	};
	if (tt_debug_mode == 70803 && callback != CALLBACK_NOTHING) {
		tt_error_file() << "add_ease_in_and_out_to_goal callback# " << callback << " on frame " << tt_frame_number << endl;
	};
#endif
	insert_animation(new EaseInAndOut_To_Variable_Goal(this,variable,other_sprite,other_variable,duration,notify,callback,offset));
};


void Sprite::animate_location_to(Sprite *other, // rewritten 131102
										   millisecond duration,
//										   boolean *status,
										   Sprite *notify,
										   AnimationCallBack callback,
										   city_coordinate x_offset,
										   city_coordinate y_offset,
										   boolean signal_change) {
// following should be subsumed by signal change stuff
//	if (duration == 0) set_clean_status(MOVED);
	add_ease_in_and_out_to_goal(ANIMATING_LLX,other,ANIMATING_LLX,duration,NULL,CALLBACK_NOTHING,x_offset);
	add_ease_in_and_out_to_goal(ANIMATING_LLY,other,ANIMATING_LLY,duration,notify,callback,y_offset);
	if (signal_change) {
		do_after(duration,this,SIGNAL_SIZE_AND_LOCATION_CALLBACK);
	};
};

millisecond Sprite::animations_stop_time() {
	if (animations == NULL) return(tt_current_time);
	return(animations->last_stop_time());
};

void Sprite::shift_viewpoint(city_coordinate delta_x, city_coordinate delta_y) { 
	// new on 020204 -- called when scrolling
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		tt_error_file() << "Debug target shift_viewpoint" << " on frame=" << tt_frame_number << endl;
	};
#endif
	move_by(delta_x,delta_y); // default is as before
	if (animations != NULL) {
		animations->shift_viewpoint(delta_x,delta_y); // so animations continue to the same floor coordinate not camera coordinate
	};
};

void Sprite::set_width_maintain_proportionality(city_coordinate new_width) {
	if (width != 0) {
		int64 height_times_new_width = ((int64) height)*new_width; // new on 010305 to deal with very large numbers
		set_size(new_width,(city_coordinate) (height_times_new_width/width));
	};
};

void Sprite::animate_width_maintain_proportionality(city_coordinate new_width, millisecond duration) {
	// could support other animation args too
	if (width != 0) {
		int64 height_times_new_width = ((int64) height)*new_width; // new on 010305 to deal with very large numbers
		animate_size(new_width,(city_coordinate) (height_times_new_width/width),duration);
	};
};

void Sprite::set_height_maintain_proportionality(city_coordinate new_height) {
	if (height != 0) {
		int64 width_times_new_height = ((int64) width)*new_height; // new on 010305 to deal with very large numbers
		set_size((city_coordinate) (width_times_new_height/height),new_height);
	};
};

void Sprite::animate_height_maintain_proportionality(city_coordinate new_height, millisecond duration) {
	// could support other animation args too
	if (height != 0) {
		int64 width_times_new_height = ((int64) width)*new_height; // new on 010305 to deal with very large numbers
		animate_size((city_coordinate) (width_times_new_height/height),new_height,duration);
	};
};

void Sprite::finish_instantly(boolean callbacks_only, boolean ignore_city_stopped) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("finish instantly of debug target.");
	};
#endif
   if (!ignore_city_stopped && tt_city->stopped()) return; // might be busy waiting for this to change  || tt_log_version_number < 15
   boolean saved_finishing_instantly_flag = finishing_instantly_flag;
   finishing_instantly_flag = TRUE;
	// finish them off instantly
	if (running_animations) { // unless recursively running animation
		finish_animation_pending = TRUE;
		set_callback_only(callbacks_only);
		set_all_at_once(TRUE);
		animation_pending_callbacks_only = (flag) callbacks_only;
	} else {
		run_animations(TRUE,callbacks_only); 
		check_cycle_callback(TRUE);
	};
   finishing_instantly_flag = saved_finishing_instantly_flag;
};

const int max_call_level = 10; // should be more than enough to finish some sequence such as a bird

void Sprite::run_animations(boolean all_at_once, boolean callback_only, int call_level) {
	// introduced call_level on 191104 since otherwise this can create a stack overflow trying to repeatedly do something that is waiting
	// e.g. delivering a long-distance message
	if (animations == NULL) return;
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("running animations of debug target.");
	};
	if (tt_debug_mode == 250804 && kind_of() == ROBOT) {
		tt_error_file() << "Stop time of robot is " << animations->stop_time() << " on frame " << tt_frame_number << endl;
	};
#endif
//	if (kind_of() == ROBOT) {
//	  tt_error_file() << "Robot at " << llx << "," << lly << " at time " << tt_current_time << endl;
//	};
//#if TT_DEBUG_ON
//	if (running_animations) {
//		say("Something's wrong inside of ToonTalk.  Trying to animate while animating.",4);
//		return;
//	};
//#endif
	running_animations = TRUE;
	if (call_level > max_call_level) { // new on 191104
		all_at_once = FALSE; // do it later
	};
	animations = animations->update(all_at_once,callback_only);
//	Animation *remaining = animations;
//	while (remaining != NULL) {
//		animations_tail = remaining;
//		remaining = remaining->rest();
//	};
	running_animations = FALSE;
	if (new_animations != NULL) { // some animations were added while running above
		if (animations == NULL) {
			animations = new_animations;
		} else {
			Animation *remaining = new_animations;
			while (remaining != NULL) {  // insert each of the new_animations into the current ones
				Animation *new_remaining = remaining->rest();
				remaining->set_rest(NULL);
				animations = animations->insert(remaining);
				remaining = new_remaining;
			};
		};
//      animations->debug();
		new_animations = NULL;
		if (all_at_once) {
			finish_animation_pending = TRUE; // do these new ones right away
     	   animation_pending_callbacks_only = (flag) callback_only;
		};
	};
	if (prepared_for_deletion()) { // new on 220704 since an animation may have destroyed this
		stop_animations();
		return;
	};
	while (finish_animation_pending) { // prior to 5/18/98 was if not while
		// while running animation someone signaled that they want to stop
		finish_animation_pending = FALSE;
		run_animations(TRUE,animation_pending_callbacks_only,call_level+1);
		check_cycle_callback(TRUE);
	};
	// moved here on 250803 so this happens after all the animations have run
//#if TT_DEBUG_ON
//	// 250803
//	if (kind_of() == COPIER) {
//		tt_error_file() << "Just did some animation on frame " << tt_frame_number << endl;
////		tt_debug_target = ((Copier *) this)->pointer_to_button()->debug_counter; // track this guy
//		tt_error_file() << "Copier's position is " << llx << "," << lly << " on frame " << tt_frame_number << endl;
//		Button *button = ((Copier *) this)->pointer_to_button();
//		tt_error_file() << " its button's position is " << button->current_llx() << "," << button->current_lly() << endl;
//	};
//#endif
	if (any_variables_changed()) {
		// might just have moved or possibly animating non-size/location
		// but to be safe:
		set_clean_status(DIRTY); 
		if (leader != NULL) {
         // this doesn't work so well -- for now I'm avoiding animating things with leaders
//		leader->set_clean_status(clean_status); // already taken care of
//         if (tt_log_version_number > 8) scale_to_fit(); // in case size has changed
			record_offsets(); // offsets from leader have changed
		};
		if (followers != NULL) {
			scale_to_fit(); // new on 250803 since propagation depends upon true_x_offset which depends upon x_scale and y_scale
			// better to move this to inside really_propagate_changes?
			really_propagate_changes(ANIMATING); // arg new on 311004
//#if TT_DEBUG_ON
//			// 250803
//			if (kind_of() == COPIER) {
//				tt_error_file() << "And then called really_propagate_changes on frame " << tt_frame_number << endl;
//				tt_error_file() << "Copier's position is " << llx << "," << lly << " on frame " << tt_frame_number << endl;
//				Button *button = ((Copier *) this)->pointer_to_button();
//				tt_error_file() << " its button's position is " << button->current_llx() << "," << button->current_lly() << endl;
//			};
//#endif
		};
	};
};

Sprite *Sprite::respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected, Sprite *by,
												boolean record_action, Sprite *original_recipient, 
                                    boolean size_constrained_regardless) {
   if (key == toggle_infinite_stack && this != tt_toolbox && (leader == NULL || leader->kind_of() != TOOLBOX)) { // new on 080400
		// added conditions on 080604 so can't turn Tooly or things in Tooly into a non-stack
		set_infinite_stack(!infinite_stack());
	};
	int encoded_key;
	if (extended) {
		encoded_key = key+256; // hack on 100604 to squeeze in extended characters into robot's training
	} else {
		encoded_key = key;
	};
	if (record_action) {
		// replace this with original_recipient on 030105 so can record labelling nests and others
		// and replaced floor with original_recipient->pointer_to_background()
		record_event(KEYBOARD_CHARACTER,by,original_recipient->pointer_to_background(),original_recipient,TRUE,encoded_key); 
		// moved here on 080400 -  means that birds and nests for example now can be labled by robots
	};
   by->completed();
//	if (action_status != NULL) *action_status += 1;
   return(this); // don't replace
};

void Sprite::set_clip_region(SelectionFeedback selection_feedback, TTRegion *region) {
//   if (leader == NULL && !followers_completely_inside()) return; -- commented out on 031002 since some followers might be inside and others not
	city_coordinate adjusted_llx, adjusted_lly;
	city_coordinate real_width, real_height;
	if (region == NULL) {
		adjusted_llx = llx;
		adjusted_lly = lly;
		// is the following right if flying over a city and scale is greater than ground_scale??
		real_width = true_width();   // was just width and height before 11/2/97
		real_height = true_height();
	} else {
		adjusted_llx = region->min_x;
		adjusted_lly = region->min_y;
		real_width = region->width();
		real_height = region->height();
	};
	if (want_feedback(selection_feedback)) {
		// only supporting movement for now
		adjusted_llx += selection_delta_x();
		adjusted_lly += selection_delta_y();
	};
	coordinate screen_coordinate = tt_screen->screen_x(adjusted_llx);
	if (screen_coordinate > tt_destination_llx) {
		tt_destination_llx = screen_coordinate;
	};
	screen_coordinate += tt_screen->horizontal_pixel_length(real_width)-1;//+1;
	if (screen_coordinate < tt_destination_urx) {
		tt_destination_urx = screen_coordinate;
	};
//	if (tt_destination_llx < 0) {
//		tt_clipped_width += tt_destination_llx;
//		tt_destination_llx = 0;
//	};
	screen_coordinate = tt_screen->screen_y(adjusted_lly);
	if (screen_coordinate > tt_destination_lly) {
		tt_destination_lly = screen_coordinate+1; // seems to work right to +1
	};
	screen_coordinate += tt_screen->vertical_pixel_length(real_height)-1;//+1;
	if (screen_coordinate < tt_destination_ury) {
		tt_destination_ury = screen_coordinate;
	};
//	if (tt_destination_lly < 0) {
//		tt_clipped_height += tt_destination_lly;
//		tt_destination_lly = 0;
//	};
	tt_main_window->set_clip_region(); // tell windows about this
};

boolean Sprite::save_region() {
#if TT_DEBUG_ON
   if (tt_debug_mode == 103) {
      tt_error_file() << type_name(this) << " ";
   };
	if (tt_debug_target == debug_counter) {
		log("save_region of debug target.");
	};
#endif
	TTRegion region;
//	full_region(region);
   true_region(region);
// only reasonable on the floor (not flying over the city)
//	if (region.min_x >= ideal_screen_width || 
//		 region.min_y >= ideal_screen_height ||
//		 region.max_x <= 0 || 
//		 region.max_y <= 0) return;
	if (tt_selection != NULL && (this == tt_selection || has_leader(tt_selection))) {
		// new to avoid thin horiztonal turds
		region.shift_by(selection_delta_x(),selection_delta_y());
	};
	return(tt_screen->save_city_region(region));
};

void Sprite::save_region_to_memory(bytes buffer) { // not used as of 290404
	TTRegion region;
   true_region(region);
	tt_screen->save_city_region(region,buffer);
};

void Sprite::copy_leader_offsets(Sprite *other) {
	Leader_Offsets *others_leader_offsets = other->pointer_to_leader_offsets();
	if (others_leader_offsets == NULL) {
		return; // new on 250103
	};
	if (leader_offsets == NULL) { // new on 171000 - rather than do nothing if no offsets better to copy them...
		leader_offsets = new Leader_Offsets();
	};
	leader_offsets->copy_leader_offsets(others_leader_offsets);
};

void Sprite::tell_programmer_am_ready() { // new on 220901
//	if (cycle_looped_back()) { // conditional new on 031201 so if Dusty or Pumpy called twice that the selection region is updated properly
		tt_programmer->tool_in_hand_is_ready(this);
};

#if TT_MICROSOFT
void Leader_Offsets::follow_leader_changes(Sprite *follower,
														 city_coordinate leader_width,
														 city_coordinate leader_height,
														 city_coordinate leader_x,
														 city_coordinate leader_y,
														 ChangeReason reason) {
	if (recorded_leader_width_squared == 0 || recorded_leader_height_squared == 0) return; // new on 100200
//	if (tt_log_version_number < 14) {
//		follower->set_size_and_location(
//			((width_times_leader_width/recorded_leader_width)*leader_width)/recorded_leader_width,
//			((height_times_leader_height/recorded_leader_height)*leader_height)/recorded_leader_height,
//			leader_x+((x_offset_times_leader_width/recorded_leader_width)*leader_width)/recorded_leader_width,
//			leader_y+((y_offset_times_leader_height/recorded_leader_height)*leader_height)/recorded_leader_height,
//			LEADER_CHANGED);
//	} else {
		// *10 as compromise between round-off errors and dealing with large objects
//		if (x_offset_times_leader_width > 1000000 || y_offset_times_leader_height > 10000000) { 
			// new on 080200 risking overflow so compute in less accurate but safer manner
//			follower->set_size_and_location(
//				((width_times_leader_width/recorded_leader_width)*leader_width)/recorded_leader_width,
//				((height_times_leader_height/recorded_leader_height)*leader_height)/recorded_leader_height,
//				leader_x+(((x_offset_times_leader_width/recorded_leader_width)*leader_width)/ideal_horizontal_units(recorded_leader_width))*10,
//				leader_y+(((y_offset_times_leader_height/recorded_leader_height)*leader_height)/ideal_vertical_units(recorded_leader_height))*10,
//				LEADER_CHANGED);
//		} else {
		// removed changed x_offset_times_leader_width*10 to x_offset_times_leader_width on 080200 since using int64 now
	if (reason == NO_REASON_GIVEN || reason == JUST_MOVED) reason = LEADER_CHANGED; 
	// new on 051100 -- was NONE_GIVEN prior to 281100 -- added JUST_MOVED on 281100
//		(city_coordinate) ((width_times_leader_width/recorded_leader_width)*leader_width)/recorded_leader_width,
//		(city_coordinate) ((height_times_leader_height/recorded_leader_height)*leader_height)/recorded_leader_height,
		// re-wrote the following on 100200 to multiply first and then divide to minimize round-off
		// and also to use squares of the numbers and add half the square to get better round-off
	city_coordinate new_width,new_height,new_llx,new_lly;
	int64 product = width_times_leader_width*leader_width;
	// following new on 250202 since even with int64s can get overflow sometimes
	// (e.g. factorial of 10000) -- consider using double floats instead??
	if (product < width_times_leader_width || product < leader_width) { // overflow
		new_width = (city_coordinate) 
						 ((((double) width_times_leader_width)*leader_width+recorded_leader_width_squared/2)
						  /recorded_leader_width_squared);
	} else {
		new_width = (city_coordinate) ((product+recorded_leader_width_squared/2)/recorded_leader_width_squared);
	};
	product = height_times_leader_height*leader_height;
	if (product < height_times_leader_height || product < leader_height) { // overflow
		new_height = (city_coordinate) 
						  ((((double) height_times_leader_height)*leader_height+recorded_leader_height_squared/2)
						   /recorded_leader_height_squared);
	} else {
		new_height = (city_coordinate) ((product+recorded_leader_height_squared/2)/recorded_leader_height_squared);
	};
	product = x_offset_times_leader_width*leader_width;
	if (product < x_offset_times_leader_width || product < leader_width) { // overflow
		new_llx = (city_coordinate) 
					  (leader_x+((((double) x_offset_times_leader_width)*leader_width+recorded_leader_width_squared/2)
					   /recorded_leader_width_squared)); 
	} else {
		new_llx = (city_coordinate) (leader_x+((product+recorded_leader_width_squared/2)/recorded_leader_width_squared)); 
		//ideal_horizontal_units(recorded_leader_width))
	};
	product = y_offset_times_leader_height*leader_height;
	if (product < y_offset_times_leader_height || product < leader_height) { // overflow
		new_lly = (city_coordinate) 
					  (leader_y+((((double) y_offset_times_leader_height)*leader_height+recorded_leader_height_squared/2)
					  /recorded_leader_height_squared));
	} else {
		new_lly = (city_coordinate) 
					  (leader_y+((product+recorded_leader_height_squared/2)/recorded_leader_height_squared)); 
		//ideal_vertical_units(recorded_leader_height)) 
	};
	follower->set_size_and_location(new_width,new_height,new_llx,new_lly,LEADER_CHANGED); 
	// experiment 250803 to replace reason with LEADER_CHANGED
};

void Leader_Offsets::follow_leader_size_changes(Sprite *follower,
															   city_coordinate leader_width,
															   city_coordinate leader_height,
																ChangeReason reason) {
	// version of the above which only deals with size changes
	if (recorded_leader_width_squared == 0 || recorded_leader_height_squared == 0) return; // new on 100200
	if (reason == NONE_GIVEN) reason = LEADER_CHANGED; // new on 051100
	follower->set_size((city_coordinate) 
							 ((width_times_leader_width*leader_width+recorded_leader_width_squared/2)/recorded_leader_width_squared),
							 (city_coordinate) 
							 ((height_times_leader_height*leader_height+recorded_leader_height_squared/2)/recorded_leader_height_squared),
							 reason);
};
#else
void Leader_Offsets::follow_leader_changes(Sprite *follower,
														 city_coordinate leader_width,
														 city_coordinate leader_height,
														 city_coordinate leader_x,
														 city_coordinate leader_y,
														 ChangeReason reason) {
		// *10 as compromise between round-off errors and dealing with large objects
//		if (x_offset_times_leader_width > 1000000 || y_offset_times_leader_height > 10000000) { 
			// new on 080200 risking overflow so compute in less accurate but safer manner
//			follower->set_size_and_location(
//				((width_times_leader_width/recorded_leader_width)*leader_width)/recorded_leader_width,
//				((height_times_leader_height/recorded_leader_height)*leader_height)/recorded_leader_height,
//				leader_x+(((x_offset_times_leader_width/recorded_leader_width)*leader_width)/
//															 ideal_horizontal_units(recorded_leader_width))*10,
//				leader_y+(((y_offset_times_leader_height/recorded_leader_height)*leader_height)/
//															 ideal_vertical_units(recorded_leader_height))*10,
//				LEADER_CHANGED);
//		} else {
		// removed changed x_offset_times_leader_width*10 to x_offset_times_leader_width on 080200 since using int64 now
		if (reason == NONE_GIVEN) reason = LEADER_CHANGED; // new on 051100
		follower->set_size_and_location(
			((width_times_leader_width/recorded_leader_width)*leader_width)/recorded_leader_width,
			((height_times_leader_height/recorded_leader_height)*leader_height)/recorded_leader_height,
			leader_x+(((x_offset_times_leader_width*10)/recorded_leader_width)*leader_width)/
			          ideal_horizontal_units(recorded_leader_width),
			leader_y+(((y_offset_times_leader_height*10)/recorded_leader_height)*leader_height)/
						 ideal_vertical_units(recorded_leader_height),
			reason);
};

void Leader_Offsets::follow_leader_size_changes(Sprite *follower,
															   city_coordinate leader_width,
															   city_coordinate leader_height,
                                                ChangeReason reason) {
	// version of the above which only deals with size changes
   if (reason == NONE_GIVEN) reason = LEADER_CHANGED; // new on 051100
		follower->set_size(
			((width_times_leader_width/recorded_leader_width)*leader_width)/recorded_leader_width,
			((height_times_leader_height/recorded_leader_height)*leader_height)/recorded_leader_height,
			reason);
};
#endif

void Leader_Offsets::copy_leader_offsets(Leader_Offsets *other) {
	if (other == NULL) return; // new on 070400 since if very small then there won't be any offsets
#if TT_MICROSOFT
	recorded_leader_width_squared = other->recorded_leader_width_squared;
	recorded_leader_height_squared = other->recorded_leader_height_squared;
#else
	recorded_leader_width = other->recorded_leader_width;
	recorded_leader_height = other->recorded_leader_height;
#endif
	width_times_leader_width = other->width_times_leader_width;
	height_times_leader_height = other->height_times_leader_height;
	x_offset_times_leader_width = other->x_offset_times_leader_width;
	y_offset_times_leader_height = other->y_offset_times_leader_height;
};

#if TT_XML
#if TT_MICROSOFT

void Leader_Offsets::xml_geometry(xml_element *element) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 240603) {
			tt_error_file() << "inside Leader_Offsets::xml_geometry" << endl;
			xml_set_attribute(element,L"IfThisAppearsThen64bitBug",1);
		};
#endif
	xml_set_attribute(element,L"RecordedLeaderWidthSquared",recorded_leader_width_squared);
	xml_set_attribute(element,L"RecordedLeaderHeightSquared",recorded_leader_height_squared);
	xml_set_attribute(element,L"WidthTimesLeaderWidth",width_times_leader_width);
	xml_set_attribute(element,L"HeightTimesLeaderHeight",height_times_leader_height);
	xml_set_attribute(element,L"XOffsetTimesLeaderWidth",x_offset_times_leader_width);
	xml_set_attribute(element,L"YOffsetTimesLeaderHeight",y_offset_times_leader_height);
};

boolean Leader_Offsets::handle_xml_tag(Tag tag, xml_node *node) {
	if (tag == NO_MORE_TAGS) {
		recorded_leader_width_squared = xml_get_attribute_int64(node,L"RecordedLeaderWidthSquared",recorded_leader_width_squared);
		recorded_leader_height_squared = xml_get_attribute_int64(node,L"RecordedLeaderHeightSquared",recorded_leader_height_squared);
		width_times_leader_width = xml_get_attribute_int64(node,L"WidthTimesLeaderWidth",width_times_leader_width);
		height_times_leader_height = xml_get_attribute_int64(node,L"HeightTimesLeaderHeight",height_times_leader_height);
		x_offset_times_leader_width = xml_get_attribute_int64(node,L"XOffsetTimesLeaderWidth",x_offset_times_leader_width);
		y_offset_times_leader_height = xml_get_attribute_int64(node,L"YOffsetTimesLeaderHeight",y_offset_times_leader_height);
		return(TRUE);
	};
	return(FALSE); // warn??
};

// could do none MICROSOFT version some day -- maybe LONGLONG is standard instead of int64
#endif
#endif

void Image_cycle::initialize(short int count, TTImagePointer *is, short int *ds, // was just TTImage prior to 231010
//									  millisecond the_cycle_duration,
									  short int the_loop_back_index,
									  boolean function_of_distance_flag) {
   // number_of_images is not number of distinct images but total used in cycle including duplicates
	number_of_images = count;
	images = is;
	durations = ds;
	loop_back_index = the_loop_back_index;
	function_of_distance = (flag) function_of_distance_flag;
//   current_index = 0;
//	cycle_duration = the_cycle_duration;
//	reset();
	always_cycle = TRUE;
	for (int i = 0; i < number_of_images; i++) {
		if (durations[i] != 0) {
			always_cycle = FALSE;
			break;
		};
	};
};

void Image_cycle::java(java_stream &out, const_java_string image_cycle_name, int parameter) {
   java_send(out,image_cycle_name,S(IDS_JAVA_INITIALIZE),parameter,number_of_images,
             loop_back_index,
             java_boolean(always_cycle),java_boolean(function_of_distance));
   for (int i = 0; i < number_of_images; i++) {
      int id = images[i]->return_resource_index();
      java_string file_name = ensure_file_for_java(id,PICTURE);
		if (file_name == NULL) {
			// new on 021204 to generate the GIF if it has been installed
			char user_java_directory[MAX_PATH]; // new on 210303
			strcpy(user_java_directory,tt_user_directory);
			strcat(user_java_directory,AC(IDC_JAVA_PATH));
			string full_file_name = images[i]->save_gif_file(user_java_directory);
			if (full_file_name == NULL) { // problems
				file_name = copy_string("unknown.gif"); // new on 071200 for robustness
			} else {
				file_name = copy_string(full_file_name+strlen(user_java_directory));
				delete [] full_file_name;
			};		
		};
		int units_per_horizontal_pixel = compute_units_per_horizontal_pixel();
		int units_per_vertical_pixel = compute_units_per_vertical_pixel();
		// changed on 060503 since the other scheme made built-in images too small and off center
      int image_width = images[i]->current_pixel_width(); //width_without_scaling()/units_per_horizontal_pixel; // updated on 230601 tt_screen->user_x(images[i].width_without_scaling());
      int image_height = images[i]->current_pixel_height(); // >height_without_scaling()/units_per_vertical_pixel; // tt_screen->user_y(images[i].height_without_scaling());
      int x_offset = images[i]->current_x_offset()/units_per_horizontal_pixel; // tt_screen->user_x(images[i].current_x_offset());
      int y_offset = images[i]->current_y_offset()/units_per_vertical_pixel; // tt_screen->user_y(images[i].current_y_offset());
      java_send_quote_arg6(out,image_cycle_name,S(IDS_JAVA_ADD_IMAGE),i,image_width,image_height,x_offset,y_offset,file_name,durations[i]);
      delete [] file_name;
   };
};

millisecond Image_cycle::total_cycle_duration(boolean looped_back_already) { // arg is new on 040204
	int cycle_duration = 0;
	int i;
	if (looped_back_already) {
		i = loop_back_index;
	} else {
		i = 0;
	};
	for (; i < number_of_images; i++) {
		cycle_duration += durations[i];
	};
	if (cycle_duration == 0) cycle_duration++;
	return(cycle_duration);
};

Image_cycle::~Image_cycle() {
	for (int i = 0; i < number_of_images; i++) { // new on 160103
		delete images[i];
	};
#if TT_DEBUG_ON
  //  if (tt_debug_mode == 66) {
	 //  for (int i = 0; i < number_of_images; i++) {
		//	images[i]->set_deleted();
		//};
	 //} else { 
//	   delete [] images;
//	 };
	 if (tt_debug_mode == 270200) {
		 tt_error_file() << "Deleting image cycle with image count of " << number_of_images << endl;
	 };
#endif
  delete [] images;
  delete [] durations;
};

// updated the following to really check the duration of the last 20 (frame_sample_size) frames on 180100
// and added dumping and loading

const int frame_sample_size = 20;
const millisecond ideal_frame_duration = 100; // 10 fps minimum
const millisecond maximum_sample_duration = frame_sample_size*ideal_frame_duration;
//millisecond average_duration = 100;

class KeepEveryData { // collected up into a structure on 030703 for easy of unzipping
	public:
		KeepEveryData() :
			sample_duration(0),
			keep_every(1),
			frames_since_last_change(0),
			recent_durations_index(0) {
				for (int i = 0; i < frame_sample_size; i++) {
					recent_durations[i] = 0;
				};
			};
		void update_keep_every();
	// following are public too
	millisecond sample_duration;
	unsigned char keep_every;  // by default use all the images
	unsigned char frames_since_last_change;
	millisecond recent_durations[frame_sample_size];
	unsigned char recent_durations_index;
};

KeepEveryData keep_every_data;

//millisecond sample_duration = 0;
//unsigned char keep_every = 1;  // by default use all the images
//unsigned char frames_since_last_change = 0;
//millisecond recent_durations[frame_sample_size] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//unsigned char recent_durations_index = 0;

void dump_keep_every_data(output_stream &stream) {
	stream.write((char *) &keep_every_data,sizeof(keep_every_data));
	// was
	//stream.write((char *) &sample_duration,sizeof(sample_duration));
	//stream.write((char *) &keep_every,sizeof(keep_every));
	//stream.write((char *) &frames_since_last_change,sizeof(frames_since_last_change));
	//stream.write((char *) &recent_durations,sizeof(recent_durations));
	//stream.write((char *) &recent_durations_index,sizeof(recent_durations_index));
};

void load_keep_every_data(InputStream *stream) {
	//if (tt_log_in_archive != NULL) { // new branch on 030703
	//	read_current_file_in_unzip_archive(tt_log_out_archive,(char *) &keep_every_data,sizeof(keep_every_data));
	//} else {
	if (tt_log_version_number > 31) { // conditional new on 170703 since single byte flags are full words in structure??
		stream->read((string) &keep_every_data,sizeof(keep_every_data));
	} else {
		stream->read((string) &keep_every_data.sample_duration,sizeof(keep_every_data.sample_duration));
		stream->read((string) &keep_every_data.keep_every,sizeof(keep_every_data.keep_every));
		stream->read((string) &keep_every_data.frames_since_last_change,sizeof(keep_every_data.frames_since_last_change));
		stream->read((string) &keep_every_data.recent_durations,sizeof(keep_every_data.recent_durations));
		stream->read((string) &keep_every_data.recent_durations_index,sizeof(keep_every_data.recent_durations_index));
	};
};

void update_keep_every() {
	keep_every_data.update_keep_every();
};

void KeepEveryData::update_keep_every() {
  // this is for machines with little memory that where the disk
  // becomes the bottle neck -- so we skip some images
   sample_duration += tt_millisecond_delta;
   sample_duration -= recent_durations[recent_durations_index]; // removed oldest duration
   recent_durations[recent_durations_index] = tt_millisecond_delta;
   recent_durations_index = (recent_durations_index+1)%frame_sample_size;
   if (frames_since_last_change > 20) {
	   if (keep_every < 4 && sample_duration > maximum_sample_duration) {
		   // falling behind
		   keep_every++;
		   frames_since_last_change = 0;
#if TT_DEBUG_ON
		   if (tt_debug_mode == 110) {
			   tt_error_file() << "Now keeping every " << keep_every 
							   << " frame since sample duration is " << sample_duration 
							   << C(IDS_FRAME_EQUAL) << tt_frame_number << ") "
							   << endl;
			};
#endif
		} else if (keep_every != 1 && sample_duration < maximum_sample_duration) {
		   // skipping but keeping up fine
		   keep_every--;
		   frames_since_last_change = 0;
#if TT_DEBUG_ON
		   if (tt_debug_mode == 110) {
			   tt_error_file() << "Now keeping every " << keep_every 
							   << " frame since sample duration is " << sample_duration 
							   << C(IDS_FRAME_EQUAL) << tt_frame_number << ") "
							   << endl;
			};
#endif
		};
   } else {
	  frames_since_last_change++;
   };
#if TT_DEBUG_ON
	if (tt_debug_mode == 241299) { // generating small footprint trial version
		if (keep_every < 2) keep_every = 2;
	};
#endif
#if TT_SMALL_TRIAL
	if (keep_every < 2) keep_every = 2;
#endif
};

/*
void update_keep_every() {
  // this is for machines with little memory that where the disk
  // becomes the bottle neck -- so we skip some images
  average_duration = (tt_millisecond_delta+average_duration)/2;
  if (frames_since_last_change > 20) {
	  if (keep_every < 4 && average_duration > ideal_frame_duration) {
		  // falling behind
		  keep_every++;
		  frames_since_last_change = 0;
#if TT_DEBUG_ON
		  if (tt_debug_mode == 110) {
			  tt_error_file() << S(IDS_NOW_KEEPING) << keep_every 
							  << S(IDS_FRAME_SINCE_AVERAGE_DURATION) << average_duration 
							  << S(IDS_FRAME_EQUAL) << tt_frame_number << ") "
							  << endl;
		  };
#endif
	  } else if (keep_every != 1 && average_duration < ideal_frame_duration) {
		  // skipping but keeping up fine
		  keep_every--;
		  frames_since_last_change = 0;
#if TT_DEBUG_ON
		  if (tt_debug_mode == 110) {
			  tt_error_file() << S(IDS_NOW_KEEPING) << keep_every 
							  << S(IDS_FRAME_SINCE_AVERAGE_DURATION) << average_duration 
							  << S(IDS_FRAME_EQUAL) << tt_frame_number << ") "
							  << endl;
		  };
#endif
	  };
  } else {
	 frames_since_last_change++;
  };
};
*/

TTImage *Image_cycle::update_display(const boolean change,
											    const city_coordinate delta_x,
											    const city_coordinate delta_y,
											    long ideal_speed,
											    // read and update the following
											    short int &current_index, 
											    millisecond &cycle_start_time,
											    millisecond &cycle_duration,
											    boolean &looped_back_already,
											    boolean &single_step,
											    city_coordinate &x_offset,
											    city_coordinate &y_offset) {
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
  if (change) {
	  millisecond cycle_value = tt_current_time-cycle_start_time;
	  short int old_index = current_index;
#if TT_DEBUG_ON
	  if (tt_debug_mode == 10) {
		  tt_error_file() << C(IDS_INDEX_WAS) << current_index
						  << C(IDS_ORIGINAL_CYCLE_VALUE_WAS) << cycle_value
						  << C(IDS_CURRENT_DURATION_IS) << durations[current_index] << endl;
	  };
#endif
	  if (always_cycle) {  // like helicopter blades -- better to have small duration?
		  current_index = (short int) ((current_index+1)%number_of_images);
	  } else if (ideal_speed > 0) {
		  city_coordinate distance = max(labs(delta_x),labs(delta_y));
		  if (distance > 0) {// && tt_millisecond_delta > 0) {
//			  // speed is in city units per second
//			  // so in a full cycle should move speed units
//			  long speed = (distance*1000L)/tt_millisecond_delta;
			  long increment1000 = (distance*1000L*number_of_images)/ideal_speed;
			  short int increment = (short int) (increment1000/1000);
			  short int maybe_one_more = (short int) (increment1000%1000);
			  if (my_random(1000) < maybe_one_more) {
				  increment++;
			  };
			  if (increment > 2) increment = 2;
			  current_index += increment;
			  current_index %= number_of_images;
		  };
	  } else if (cycle_stopped(current_index)) { // moved this before the next branch on 100803 since otherwise things can be loaded on the end of a cycle and then reset incorrectly
			// stuck on last image -- do nothing
  	  } else if (single_step) {
		  current_index = (short int) ((current_index+1)%number_of_images);
		  single_step = FALSE;
	  } else {
		  if (!looped_back_already && cycle_value >= cycle_duration) {
			  looped_back_already = TRUE;
			  if (loop_back_index != number_of_images-1) {
				  // not stuck on last image
				  if (loop_back_index != 0) { 
					  // recompute cycle_duration for just looping part
					  //cycle_start_time += cycle_duration; // since its a shorter loop
					  //cycle_duration = 0;
					  //for (int i = loop_back_index; i < number_of_images; i++) {
						 // cycle_duration += durations[i];
					  //};
					  //// like taking off -- can't be zero
					  //if (cycle_duration == 0) cycle_duration++;
					  cycle_duration = total_cycle_duration(TRUE); // rewritten on 040204 to share code
				  };
			  } else {
				  current_index = (short int) (number_of_images-1);
			  };
		  };
		  if (!(looped_back_already && loop_back_index == number_of_images-1)) {
			  //if (cycle_duration <= 0) {
				 // cycle_duration = total_cycle_duration(); // new on 120803
			  //};
			  //if (looped_back_already && !recomputed_cycle_duration && loop_back_index > 0) { // new on 040204
				 // cycle_duration = total_cycle_duration(loop_back_index); 
				 // recomputed_cycle_duration = TRUE;
			  //} else if (!looped_back_already && recomputed_cycle_duration && loop_back_index == 0) { // new on 050204
				 // // restore things if no longer looping back
				 // cycle_duration = total_cycle_duration(loop_back_index); 
				 // recomputed_cycle_duration = FALSE;
			  //};
			  // this is really only needed when the object was loaded in XML with this set
			  cycle_value %= cycle_duration;
			  if (looped_back_already) {
				  current_index = loop_back_index;
			  } else {
				  current_index = 0;
			  };
			  while (cycle_value > durations[current_index]) {
				  cycle_value -= durations[current_index];
				  current_index++;
				  // made the following part of the release on 110703 since can happen if loop_back_index is > 0 - e.g. a robot with parameter = 4
//#if TT_DEBUG_ON
				  if (current_index >= number_of_images) {
//					  log("current_index too large.");
					  current_index %= number_of_images;
				  };
//#endif
			  };
		  };
		};
		if (!always_cycle && old_index != current_index && 
			 keep_every_data.keep_every > 1 && current_index != 0 && current_index != number_of_images-1 && 
			 ideal_speed == 0) {
	  // skipping some and not first or last index
#if TT_DEBUG_ON
			if (tt_debug_mode == 110) {
				tt_error_file() << C(IDS_SKIPPED_INDEX) << current_index;
			};
#endif
			switch (keep_every_data.keep_every) {
				case 2:
					if (current_index&1) { // odd
						current_index--;
					};
					break;
				case 3: 
					// to optimize cache usage rather than every third
					// use 1/4 plus probablistically go with 1/3*1/4
					switch (current_index%4) {
						case 0:  // ok
							break;
						case 1:
							current_index--;
							break;
						case 3:
							current_index = (short int) (current_index-3);
							break;
						case 2:
							if (my_random(3) != 0) { // 2/3s of the time
								current_index = (short int) (current_index-2);
							};
							break;
					};
					break;
				case 4:
					current_index = (short int) (current_index-current_index%4);
					break;
				default:
					tt_error_file() << S(IDS_SHOULD_NOT_SKIP_SO_MANY) << endl;
			};
#if TT_DEBUG_ON
			if (tt_debug_mode == 110) {
				tt_error_file() << C(IDS_AND_USED_INSTEAD) << current_index << ". " // << this
								<< C(IDS_FRAME_EQUAL) << tt_frame_number << ") " << endl;
			};
#endif
		};
#if TT_DEBUG_ON
	  if (tt_debug_mode == 10) {
		  tt_error_file() << C(IDS_AND_HAS_BECOME) << current_index 
  						  << C(IDS_FRAME_EQUAL) << tt_frame_number << ") " << endl
						  << C(IDS_CYCLE_DURATION_IS) << cycle_duration << C(IDS_AND_CYCLE_VALUE_IS) << cycle_value << endl;
	  };
#endif
  } else {
	  single_step = TRUE; // paused so next time just go to next image
	  current_index %= number_of_images; // new on 220203 for robustness
  };
  // can the following ever be wrong?? -- seems some crashes happen but how??
#if TT_CAREFUL
  if (current_index < 0) { // new on 290204 since can happen if you land on the water -how?
#if TT_DEBUG_ON
	  debug_this();
#endif
	  current_index = 0;
  };
#endif
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
  images[current_index]->update_display(x_offset,y_offset);
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
  return(images[current_index]); // was &images ... prior to 231001
};

void AnimationCallBacks::run() {
	AnimationCallBacks *remaining = this;
	while (remaining != NULL) {
		remaining->run_first();
		remaining = remaining->rest();
	};
};

#if TT_XML

void AnimationCallBacks::add_xml_each(xml_element *parent, xml_document *document) { // new on 121202
   // based upon Animations::add_xml_each
   xml_element *animation_element = create_xml_element(L"Animation",document);
	xml_set_attribute(animation_element,L"CallbackIndex",(int) callback);
	if (callee != NULL) { // can it ever be NULL?
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	callee->check_for_independence();
		//};
		callee->xml_create_and_append_element(animation_element,document,TRUE)->Release();
	};
   if (rest() != NULL) {
      rest()->add_xml_each(animation_element,document);
   };
	xml_append_child(animation_element,parent); // moved here on 190804
};

boolean AnimationCallBacks::handle_xml_tag(Tag tag, xml_node *node) {
	if (tag == ANIMATION_TAG) {
		callback = (AnimationCallBack) xml_get_attribute_int(node,L"CallbackIndex",CALLBACK_NOTHING);
	} else if (tag != NO_MORE_TAGS) {
		callee = xml_load_sprite(node,tag); // experiment on 040503 to change from xml_get_sprite
	};
	return(TRUE);
};

#endif

TTImage::TTImage() :
#if TT_DEBUG_ON||TT_CAREFUL
	 deleted(FALSE),
#endif
//	bitmap_resource_handle(0),
	stretch_blit_cache(NULL),
	stretch_blit_cache_size(0),
	candidate_when_cached(0),
	image_cache_entry_index(0),
	image_cache_id(max_unsigned_short),
	image_size(0), // new on 081001
	ini_file(FALSE),
	resource_index(-1), // wasn't initialized prior to 291001
	transparent_color(0),
	image_hash(NULL),
	original_width(1), // wasn't initialized prior to 310303
	original_height(1) { 
};

unsigned long TTImage::candidate_speed_up_reciprocal() {
	unsigned long savings = candidate_savings_per_use*((long) candidate_used_count);
	if (savings == 0) savings++; // new on 210301 (after beta 28) to avoid divide by zero below
	const unsigned long duration = tt_current_time-candidate_when_cached;
	const unsigned long duration_savings = duration/savings;
	const unsigned long candidate_memory_size = ((long) candidate_pixel_width)*candidate_pixel_height;
	const unsigned long result = duration_savings*candidate_memory_size;
	if (result/candidate_memory_size != duration_savings) { // overflow
		return(max_unsigned_long);
	} else {
		return(result);
	};
};

void TTImage::initialize(//string file, 
//							  boolean mirror,
							  dimension the_pixel_width, dimension the_pixel_height,
							  city_coordinate x_off, city_coordinate y_off,
							  city_coordinate the_original_width,
							  city_coordinate the_original_height,
							  boolean rectangular_mask_flag,
							  long size,
#if TT_WINDOWS // &&!GRAPHICS_TO_MEMORY
#if GRAPHICS_TO_MEMORY
							  int compression_method,
#else
							  long ,
#endif
							  SpriteCode appearance_code,
							  int the_resource_index,	
							  image_id) {
#else
							  long offset,
							  int compression_method,
							  int ,	
							  image_id id) {
#endif
//	mirror_flag = mirror;
	rectangular_mask = (flag) rectangular_mask_flag;
	pixel_width = the_pixel_width;
	pixel_height = the_pixel_height;
	x_offset = x_off;
	y_offset = y_off;
	original_width = max(1,the_original_width); // new on 310303 since should never be zero
	original_height = max(1,the_original_height);
	// might be re-initializing like for guy with hat
//	if (file_name != NULL) delete [] file_name;
//	file_name = file;
	image_size = size;
	code = appearance_code;
	resource_index = the_resource_index;
//#if !TT_32
//	memory_handle = NULL;
//#if TT_DEBUG_ON
//	if (image_size > 64000L) {
//		tt_error_file() << "Warning image#" << resource_index 
//						<< " is too large for 16bit code to handle well" << endl;
//	};
//#endif
//#endif
   // using just a bit now to distinguish BI_RGB and BI_RLE8
	image_compression = (flag) (compression_method == BI_RLE8);
/*
	image_compression = BI_RGB;
	if (compression_method != image_compression) {
		tt_error_file() << "Warning -- for now no files are RLE" << endl;
	};
*/
//   image_index = index;
};

TTImage::~TTImage() {
// do I need file_name if using Windows Resources???
//	if (file_name != NULL) delete [] file_name;
	if (stretch_blit_cache != NULL) {
		for (int i = 0; i < stretch_blit_cache_size; i++) {
			delete stretch_blit_cache[i];
		};
		delete [] stretch_blit_cache;
      stretch_blit_cache = NULL;
   };
//#if TT_DEBUG_ON
//	if (tt_debug_mode == 270200) {
//		tt_error_file() << "Deleting image #" << resource_index << endl;
//	};
//#endif
};

#if TT_DEBUG_ON||TT_CAREFUL
void TTImage::set_deleted() {
  if (deleted) {
	  say_error(IDS_DESTROYING_SAME_PICTURE_TWICE);
	  return;
  };
  deleted = TRUE;
};
#endif

void TTImage::update_display(//int &the_pixel_width, int &the_pixel_height,
									city_coordinate &x_off, city_coordinate &y_off) {
//									city_coordinate &the_original_width,
//									city_coordinate &the_original_height) {
//	the_pixel_width = pixel_width;
//	the_pixel_height = pixel_height;
	x_off = x_offset;
	y_off = y_offset;
//	the_original_width = original_width;
//	the_original_height = original_height;
};

string TTImage::save_gif_file(string directory) { // new on 210303
	if (built_in()) {
		// new on 021204
		compute_full_file_name(); 
		// new on 141204 to ensure the file names have been discovered (e.g. a cycle where not every image has been displayed)
		return(save_picture_file_as_gif(return_full_file_name(),!rectangular(),directory));
	} else {
		return(save_private_media_picture_file_as_gif(return_full_file_name(),!rectangular(),directory));
		// prior to 3.28 was	return(save_picture_file_as_gif(return_full_file_name(),!rectangular(),directory));
	};
};

UserImage::UserImage() :
   TTImage(),
   file_name(NULL), // user level name - used for dumping
	full_file_name(NULL), // full path
//	saved_dib(NULL),
	ref_count(1),
	file_is_BMP(TRUE),
#if TT_WANT_IMAGE_STATISTICS
	display_count(0),
#endif
	converted_to_black_as_transparency(FALSE) {
//	treat_white_as_black(FALSE) 
//	dimensions_set_flag(FALSE) {
	// following moved here so even if can't find file these are set to reasonable values
	rectangular_mask = FALSE; // for now
	x_offset = 0;
   y_offset = 0;
   code = USER_SPRITE;
   resource_index = -2;   // was 0 but -1 indicates there is no resource index - was -1 prior to 200303
   // using just a bit now to distinguish BI_RGB and BI_RLE8
   image_compression = FALSE; //(flag) (dib->biCompression == BI_RLE8);	
};

UserImage::~UserImage() {
#if TT_WANT_IMAGE_STATISTICS
	if (full_file_name != NULL) {
		// add 100000 so easier to sort (e.g. 8 vs 80)
		tt_error_file() << display_count+1000000 << " " << full_file_name << " rectangular: " << (int) rectangular_mask << endl;
	};
#endif
   if (file_name != NULL) {
		delete [] file_name;
	};
	if (full_file_name != NULL) delete [] full_file_name;
	if (image_hash != NULL) delete [] image_hash;
//	if (saved_dib != NULL) GlobalFreePtr(saved_dib);
};

string UserImage::just_set_file_name(ascii_string true_initial_file_name, boolean warn) {
	// prior to 050399 used file_name only - this meant pictures were dumped with full path to where ToonTalk was installed
	string initial_file_name = extract_file_name_from_link(true_initial_file_name); // new on 091000
	if (initial_file_name == NULL) return(NULL);
   FileNameStatus file_status;
	// following rewritten on 020600 so that tt_missing_picture_file_extension or BMP is only default if no extension is given
	boolean has_bmp_extension = already_has_extension(initial_file_name,"bmp");
	boolean has_some_extension = (has_bmp_extension || extension_start_position(initial_file_name) > 0); 
	// added has_bmp_extension to save some work
	string default_extension;
	if (tt_missing_picture_file_extension != NULL && !has_bmp_extension) {
		default_extension = tt_missing_picture_file_extension;
	} else {
		default_extension = "bmp";
	};
	if (has_bmp_extension || !has_some_extension) {
		// last arg was TRUE prior to 060501 (and below too)
		// prior to 200601 default_extension was just "bmp"
		full_file_name = existing_file_name(file_status,initial_file_name,default_extension,"NewUsers",TRUE,FALSE,FALSE,FALSE,
														ok_to_check_paths());
		file_is_BMP = (full_file_name != NULL && (has_bmp_extension || tt_missing_picture_file_extension == NULL));
	};
//#if TT_DIRECTX_TRANSFORM
	// package up the following sometime
	if (full_file_name == NULL && has_some_extension) {
		// this rewritten on 020600 so only if explict extension is it used
//		const int other_picture_extensions_count = 4; // BMP already checked
//		string other_picture_extensions[other_picture_extensions_count] = {"png","jpg","gif","dib"};
//		full_file_name = existing_file_name(file_status,initial_file_name,other_picture_extensions,
														//other_picture_extensions_count,"NewUsers",TRUE,FALSE);
#if !TT_IMGSOURCE&&!TT_GDIPLUS
		if (directx_transform_loaded())  // conditional new on 070201
#endif
			full_file_name = existing_file_name(file_status,initial_file_name,NULL,"NewUsers",TRUE,FALSE,FALSE,FALSE,
															ok_to_check_paths());
		if (full_file_name == NULL) {
			// prior to 200601 default_extension was just "bmp"
			full_file_name = existing_file_name(file_status,initial_file_name,default_extension,"NewUsers",
															TRUE,FALSE,FALSE,FALSE,ok_to_check_paths());
			// new on 070201 to try again to find BMP instead
			file_is_BMP = (full_file_name != NULL && (has_bmp_extension || tt_missing_picture_file_extension == NULL));
		} else {
			file_is_BMP = FALSE;
		};
	};	
//#endif
#if !TT_DIRECTX_TRANSFORM&&!TT_IMGSOURCE&&!TT_GDIPLUS
	if (!file_is_BMP && full_file_name != NULL) {
		tt_error_file() << "Couldn't load the picture file: " << full_file_name << endl;
		tt_error_file() << "This version of ToonTalk can only load BMP pictures (file names who end with .bmp). Sorry. " << endl;
		delete [] full_file_name;
		full_file_name = NULL;
	};
#endif
   // following initializes image
   if (full_file_name == NULL) {
		if (tt_missing_picture_file_extension != NULL 
			 && (string_length_less_than(initial_file_name,tt_private_media_directory_length) || // updated 251004
			     strncmp(tt_private_media_directory,initial_file_name,tt_private_media_directory_length) != 0)) { 
				// new on 200601 to try again with an alternative extension
		   // added optimization on 231004 to not bother to look if in the private media folder
			int extension_start = extension_start_position(initial_file_name);
			if (extension_start == 0 || stricmp(initial_file_name+extension_start,tt_missing_picture_file_extension) != 0) {
				string new_file_name = add_or_replace_extension(initial_file_name,tt_missing_picture_file_extension); 
				// added 230601 so if file is say ring.bmp all works well
				string ok_file_name = existing_file_name(file_status,new_file_name,tt_missing_picture_file_extension,"NewUsers",
																	  TRUE,FALSE,FALSE,FALSE,ok_to_check_paths());
				if (ok_file_name != NULL) {
					string result = basic_file_name(ok_file_name,TRUE);
					delete [] new_file_name;
					delete [] ok_file_name;
					return(result);
				};
			};
		};
		if (warn) {
			say_file_name_error(IDS_NO_FILE_NAME_FOR_USER_MEDIA_FILE,true_initial_file_name,4,
									  has_some_extension?NULL:tt_missing_picture_file_extension); 
			// made the addition of BMP conditional on 090201
		};
		// following is new on 080500 so when file is missing the load can proceed with "reasonable" values for the following:
		pixel_width = 128; // was 100 prior to 250500
		pixel_height = 128;
		// new on 310303 to make sure values are never zero
		original_width = max(1,ideal_horizontal_units(pixel_width)); // pixel_width*tt_ideal_units_per_horizontal_pixel;
		original_height = max(1,ideal_vertical_units(pixel_height)); // pixel_height*tt_ideal_units_per_vertical_pixel;
		delete [] initial_file_name;
      return(NULL);
   };
	ini_file = (already_has_extension(full_file_name,"ini")); // new on 231001
	if (tt_maintaining_private_media_directory || // new disjunction on 220303
		 !is_url(initial_file_name) && strncmp(full_file_name,initial_file_name,strlen(initial_file_name)) != 0) {
		// didn't find initial_file_name so use short version since it must have been found on FileSearchPath - new on 110401
		file_name = basic_file_name(initial_file_name,TRUE);
	} else {
		string url_name = system_cached_url(true_initial_file_name); 
		if (url_name != NULL) {
			file_name = url_name; // new on 130302 so that pasting pictures from the net get stored as the URL not the cache name
		} else {
			file_name = copy_ascii_string(true_initial_file_name); 
			// updated on 091000 from copy_ascii_string(initial_file_name); // moved here on 310500
		};
	};
	delete [] initial_file_name; // new on 261000
	return(true_initial_file_name); // why??
};

boolean UserImage::set_file_name(ascii_string initial_file_name, ascii_string original_file_name, boolean warn, 
											int known_pixel_width, int known_pixel_height) {
	// why bother to check extensions here??
	// commented out on 221101 - or should this only be if tt_prefer_gdiplus?
//	if (strchr(initial_file_name,'.') != NULL) { // condition added on 210201 since without an extension BMP is assumed
//		const int picture_extensions_count = 8;
//		string picture_extenions[picture_extensions_count] = {"bmp","png","jpg","gif","dib","jpeg","lnk","ini"}; 
		// lnk added on 210201; ini added 260901
//		if (!already_has_extension(initial_file_name,picture_extenions,picture_extensions_count)) { // new on 200201
//			return(FALSE);
//		};
//	};
	string replacement_file_name = just_set_file_name(initial_file_name,warn);
   if (replacement_file_name == NULL) {
		if (original_file_name != NULL) { // new on 280403 to use original name if can't find private media file
			return(set_file_name(original_file_name,NULL,warn,known_pixel_width,known_pixel_height));
		} else {
			return(FALSE);
		};
	} else if (replacement_file_name != initial_file_name) { // need to change the name and try again
		return(set_file_name(replacement_file_name,original_file_name,warn,known_pixel_width,known_pixel_height)); 
		// added known_pixel_width,known_pixel_height on 280403
	};
   if (known_pixel_width > 0 && known_pixel_height > 0) { // new on 301002 to postpone the work of loading the image until needed
      pixel_width = known_pixel_width;
      pixel_height = known_pixel_height;
      set_dimensions();
   } else if (ok_to_set_dimensions_now()) {
		return(compute_dimensions(initial_file_name,warn)); // return is new as of 140304 so failure is propagated upward
	};
	return(TRUE);
};

void UserImage::reset_full_file_name(string new_name) { 
// new on 190303 - used to replace original name with private media file name
	if (full_file_name != NULL) {
		delete [] full_file_name;
	};
	full_file_name = new_name;
};

boolean UserImage::display(long &width, long &height, // made virtual on 081001
									boolean &just_discovered_transparency,
									bytes permutation,
									unsigned short int x_scale, unsigned short int y_scale,
									draw_surface image_surface) {
#if TT_WANT_IMAGE_STATISTICS
	display_count++;
#endif
	if (//!dimensions_set() && 
		// commented out on 290103 since if the file is different than before we'll get DirectX errors - 
		// not that costly to get the real values
		 image_surface == NULL) { // commented out on 311001 && (tt_using_directx_transform_for_BMPs || !file_is_BMP)) {
		image_surface = load_image((resource_index != -2)); // permutation,
		if (image_surface != NULL) {
			compute_dimensions(return_full_file_name(),TRUE,image_surface);
			if (transparent_color != 0 || converted_to_black_as_transparency) {
				just_discovered_transparency = TRUE; // new on 210203
			};
//			scale_to_fit();
		};
	};
	return(TTImage::display(width,height,just_discovered_transparency,permutation,x_scale,y_scale,image_surface));
};

boolean UserImage::compute_dimensions(ascii_string initial_file_name, boolean warn, draw_surface surface) {
	boolean file_read = FALSE;
	// commented out on 300500 the following and restored the old because this took too long to load a city with user images
	// and stressed the cache incorrectly
//	draw_surface surface = load_image(); // re-written this way since this ends up cached so won't do this repeatedly
//	if (surface != NULL) {
//		surface->Release(); // served its function - if cached it also did an AddRef so all is OK
//		file_read = TRUE;
//	};
//	LPBITMAPINFOHEADER dib = NULL;
#if TT_DIRECTX_TRANSFORM||TT_IMGSOURCE||TT_GDIPLUS
	// this sure is a lot of work just to get its dimensions...
//	if (tt_using_directx_transform_for_BMPs || !file_is_BMP) {
		if (surface == NULL) {
			int surface_width, surface_height;
			file_read = load_image_dimensions(full_file_name,surface_width,surface_height);
			pixel_width = surface_width;
			pixel_height = surface_height;
			// above is much more efficient than the following
//			surface = load_any_image(full_file_name,tt_bits_per_pixel,warn,TRUE); // rewritten on 291001 -- should optimize this
		};
//#if TT_DIRECTX_TRANSFORM
//		if (surface == NULL && !tt_prefer_gdiplus) {
//			surface = load_image_using_directx_transform(full_file_name,warn);
//		};
//#endif
//#if TT_GDIPLUS
//		if (surface == NULL && tt_prefer_gdiplus) {
//			surface = load_image_using_gdiplus(full_file_name,warn,tt_bits_per_pixel);
//		};
//#endif
		if (surface != NULL) {
	//		dib = surface_to_DIB(surface);
			int surface_width, surface_height, bits_per_pixel; // bits_per_pixel new on 230901
			surface_size(surface,surface_width,surface_height,bits_per_pixel);
//			surface->Release(); 
			// served its function - but can't it be cached?? -- commented out on 211102 - breaks if run with -m 0 (no cache)
			pixel_width = surface_width;
			pixel_height = surface_height;
			file_read = TRUE;
		};
//	};
#endif
	if (!file_read) {
		string new_file_name = NULL;
		boolean new_file_name_really_new = FALSE;
		if (already_has_extension(full_file_name,"lnk")) { // new on 071000
			new_file_name_really_new = TRUE;
		} else {
			new_file_name = full_file_name;
		};
		if (new_file_name != NULL && file_is_BMP) { // added file_is_BMP on 200201
			LPBITMAPINFOHEADER dib = DibReadBitmapInfoFromFileName(new_file_name); 
			// prior to 300500 was DibOpenFile but now just reads the header  
			if (dib != NULL) {
				pixel_width = dib->biWidth;
				pixel_height = dib->biHeight;
				GlobalFreePtr(dib); // new on 250500 - was a major leak earlier?
				file_read = TRUE;
			};
			if (new_file_name_really_new) {
				delete [] new_file_name;
			};
		};
	};
#if TT_DEBUG_ON
	// for debuggin
//	tt_error_file() << "From file:" << endl;
//	bytes dib_bytes = (bytes) dib;
//	for (int i = 0; i < 50; i++) {
//		for (int j = 0; j < 40; j++) {
//			tt_error_file() << (int) dib_bytes[0] << " ";
//			dib_bytes++;
//		};
//		tt_error_file() << endl;
//	};
//	tt_error_file() << endl;
	if (tt_debug_mode == -201199) {
		tt_error_file() << "Loaded Dib of " << full_file_name << " as part of :set_file_name" << endl;
	};
	if (tt_debug_text_pad != NULL && strncmp(tt_debug_text_pad,initial_file_name,strlen(tt_debug_text_pad)) == 0) {
		log("Setting user image file name to debug text.");
	};
#endif
   if (!file_read) {
      if (warn) {
			character buffer[512];
			output_string_stream message(buffer,512);
			message << S(IDS_TROUBLES_READING_PICTURE_FILE) << space() << full_file_name << end_sentence();
			message.put((char) 0); // terminate string
			//say_error(buffer,TRUE);
			say(buffer,3,TRUE); // rewritten on 070305 since may be user error
		};
      return(FALSE);
   };
//	if (file_status == FILE_ON_INTERNET_AND_DELETE_COPY) {
//		saved_dib = dib;
//	   DeleteFile(full_file_name);
//	};
   set_dimensions();
#if !TT_32
   if (dib->biBitCount != 8) {
      say_error(IDS_USER_PICTURE_NOT_256_COLORS,TRUE);
      GlobalFreePtr(dib);
      return(FALSE);
   };
   if (image_size > max_unsigned_short) {
      say_error(IDS_SORRY_USER_PICTURE_TOO_BIG,TRUE);
      GlobalFreePtr(dib);
      return(FALSE);
   };
#endif
//   if (saved_dib == NULL) GlobalFreePtr(dib);
//	set_dimensions_set(TRUE);
	return(TRUE);
};

ascii_string UserImage::copy_full_file_name() {
	// re-written on 080800 so that it adds BMP extension if need be (and returns a copy of the full file name)
	// revised on 170800 since was using file_name rather than full_file_name
	// commented out these changes since full_file_name is returned by existing_file_name and if in system cache won't have a BMP extension
//	if (file_is_BMP && !already_has_extension(full_file_name,"BMP")) {
//		return(append_strings(full_file_name,".BMP"));
//	} else {
		return(copy_string(full_file_name));
//	};
};

void TTImage::set_dimensions() { // re-modularized on 280500
	// new on 310303 to make sure values are never zero
   original_width = max(1,ideal_horizontal_units(pixel_width)); // pixel_width*tt_ideal_units_per_horizontal_pixel;
   original_height = max(1,ideal_vertical_units(pixel_height)); // pixel_height*tt_ideal_units_per_vertical_pixel;
//   if (pixel_width&3) { // needs padding
//      image_size = (pixel_width+4-pixel_width%4)* (long) pixel_height;
//   } else {
//      image_size = pixel_width* (long) pixel_height;
//   };
//	image_size *= tt_bytes_per_pixel; // new on 230901
	// rewrote the above on 240901 since padding assumption was correctly only for 8 bits per color
	int byte_width = pixel_width*tt_bytes_per_pixel;
	if (byte_width&3) { // needs padding
       image_size = (byte_width+4-byte_width%4)* (long) pixel_height;
   } else {
      image_size = byte_width* (long) pixel_height;
   };
};

//#if TT_32
//   work_page bytes = retrieve_image(TRUE);
//   if (bytes != NULL) delete [] bytes;
//#else
//   HGLOBAL memory_handle;
//   work_page bytes = retrieve_image(memory_handle,TRUE);
//   GlobalUnlock(memory_handle);
//   GlobalFree(memory_handle);
//#endif
//};

//void UserImage::initialize() {
 //      if (dib->biBitCount != 8) { // only 256 colors for now
//         say("Sorry, can't deal with this kind of picture file now.");
//         GlobalFreePtr(dib);
//         return(NULL);
//      };

boolean UserImage::compute_full_file_name() { // abstracted to a method on 171001
	if (full_file_name == NULL) { // was just file_name prior to 280500
		boolean ok = FALSE;
		if (resource_index >= 0 && tt_builtin_picture_directory != NULL && tt_image_file_names[resource_index] != NULL) {
			// rewritten on 121001
			string initial_file_name = new char[MAX_PATH];
//			output_string_stream name(initial_file_name,MAX_PATH); // rewritten 260104
//			name << tt_builtin_picture_directory << tt_image_file_names[resource_index];
			strcpy(initial_file_name,tt_builtin_picture_directory);
			strcat(initial_file_name,tt_image_file_names[resource_index]);
//			if (tt_missing_builtin_picture_file_extension != NULL) {
////				name << "." << tt_missing_builtin_picture_file_extension;
//				strcat(initial_file_name,".");
//				strcat(initial_file_name,tt_missing_builtin_picture_file_extension);
////				treat_white_as_black = (!tt_prefer_gdiplus && strcmp(tt_missing_builtin_picture_file_extension,"gif") == 0); 
			// maybe PNG too? -- only needed for directx transform
//			};
			if (tt_missing_builtin_picture_file_extension != NULL) {
				strcat(initial_file_name,".");
				strcat(initial_file_name,tt_missing_builtin_picture_file_extension);
				if (!file_exists(initial_file_name,FALSE)) {
					// new on 051004 to look for other picture extensions
					int extension_start = strlen(initial_file_name)-strlen(tt_missing_builtin_picture_file_extension); 
					const int picture_extensions_count = 7;
					string picture_extenions[picture_extensions_count] = {"bmp","png","jpg","gif","dib","jpeg","ini"};
					int extension_index = 0;
					while (!file_exists(initial_file_name,FALSE) && extension_index < picture_extensions_count) {
						strcpy(initial_file_name+extension_start,picture_extenions[extension_index]);
						extension_index++;
					};
				};
			};
//			name.put('\0');
			ok = (just_set_file_name(initial_file_name,TRUE) != NULL); // updated on 170103 was == file_name); // what if need to replace this file name?
			delete [] initial_file_name; // restored on 160103 after variable rename to avoid confusion with member
		};
      if (ok && (tt_using_directx_transform_or_gdiplus || !file_is_BMP)) {
			image_compression = FALSE; // doesn't apply to files loaded by directx transform
		};
		return(ok);
   };
	return(TRUE);
};

work_page UserImage::retrieve_image(int &transparent_color, int &bits_per_pixel, boolean &adjust_pitch, draw_surface *mask_surface_pointer) {
//   OFSTRUCT file_struct;
//   HFILE file = OpenFile(file_name, &file_struct, OF_READ);
//	boolean treat_white_as_black = FALSE; // new on 121001
   if (!compute_full_file_name()) return(NULL);
	LPBITMAPINFOHEADER dib = NULL;
#if TT_DEBUG_ON
	millisecond start_time;
	if (tt_debug_mode == 31001) {
		start_time = timeGetTime();
	};
#endif
//	draw_surface mask_surface = NULL; // new on 270901
#if TT_DIRECTX_TRANSFORM||TT_GDIPLUS
	if (tt_using_directx_transform_or_gdiplus || !file_is_BMP) {
//		if (tt_bits_per_pixel == 8) { // to get the palette right use GDI+ to get the DIB instead of the surface (new on 241001)
//#if TT_GDIPLUS
//			bits_per_pixel = 8;
//			adjust_pitch = FALSE;
//			return(load_bytes_using_gdiplus(full_file_name,TRUE)); // pass along transparent color??
//#endif
//		} else {
//		if (tt_bits_per_pixel != 8) { // new on 031001 - generalized from just 32 bits on 121001 - commented out on 241001
			// this loads the image in the right format and then copies out the bytes
//			draw_surface mask_surface; // new on 041001 since used locally and not released to caller
		bytes fill_in_hash = NULL;
		if (image_hash == NULL && !built_in() && tt_maintaining_private_media_directory) { 
			fill_in_hash = new byte[hash_byte_length];
			image_hash = fill_in_hash;
		};
		draw_surface surface = load_any_image(full_file_name,tt_bits_per_pixel,TRUE,(resource_index != -2),mask_surface_pointer,&transparent_color,fill_in_hash,this); 
//#if TT_DIRECTX_TRANSFORM
//			if (!tt_prefer_gdiplus) {
//				surface = load_image_using_directx_transform(full_file_name,TRUE,24,&mask_surface);
//			};
//#endif
//#if TT_GDIPLUS
//			if (tt_prefer_gdiplus) {
//				surface = load_image_using_gdiplus(full_file_name,TRUE,tt_bits_per_pixel,&mask_surface,NULL,&transparent_color);
//			};
//#endif
#if TT_DEBUG_ON
			if (tt_debug_mode == 31001) {
				tt_error_file() << (timeGetTime()-start_time)
								<< "ms to load_any_image " << full_file_name << endl;
				start_time = timeGetTime();
			};
#endif
			if (surface != NULL) {
//				if (!tt_prefer_gdiplus && tt_bits_per_pixel == 8) { // new on 251001 to run this old code in this case -- commented out on 311001
//					dib = surface_to_DIB(surface,transparent_color);
//					int surface_width, surface_height, surface_bits_per_pixel;
//					surface_size(surface,surface_width,surface_height,surface_bits_per_pixel);
//					pixel_width = surface_width;
//					pixel_height = surface_height;
//					bits_per_pixel = tt_bits_per_pixel; // surface_bits_per_pixel; since it is converted by display_on_page
//					set_dimensions();
//					surface->Release(); // served its function
//					surface = NULL;
//				} else {
					int surface_width, surface_height, surface_bits_per_pixel; // was DWORD prior to 041001 (since structure contains DWORDs but shouldn't matter)
					surface_size(surface,surface_width,surface_height,surface_bits_per_pixel);
					pixel_width = surface_width;
					pixel_height = surface_height;
					bits_per_pixel = tt_bits_per_pixel; // surface_bits_per_pixel; since it is converted by display_on_page
					set_dimensions();
					work_page image_bytes = new byte[image_size];
					work_page surface_bytes;
					int pitch, bits_per_pixel, width, height; // could simplify the above using this, right?
					local_lock_surface(surface,surface_bytes,pitch,bits_per_pixel,width,height);
					if (mask_surface_pointer == NULL || *mask_surface_pointer == NULL) {
//						if (treat_white_as_black) { 
//							copy_replacing_white_with_black(surface_bytes,image_bytes,-pitch,width*tt_bytes_per_pixel,width,height); // new on 121001
//						} else {
							copy_work_page(surface_bytes,image_bytes,-pitch,width*tt_bytes_per_pixel,width,height); 
//						};
#if TT_DEBUG_ON
						if (tt_debug_mode == 31001) {
							tt_error_file() << (timeGetTime()-start_time)
											<< "ms to copy_work_page " << endl;
							start_time = timeGetTime();
						};
#endif
					} else {
						// all this copied here on 041001
						bytes mask_bytes;
						int mask_pitch;
						int mask_width, mask_height, mask_bits_per_pixel;
						draw_surface mask_surface = *mask_surface_pointer;
						local_lock_surface(mask_surface,mask_bytes,mask_pitch,mask_bits_per_pixel,mask_width,mask_height);
						if (mask_width == surface_width && mask_height == surface_height) { // check if mask and main surface have same width and height
							copy_and_mask_work_page(image_bytes,width*tt_bytes_per_pixel,pixel_width,pixel_height,surface_bytes,-pitch,bits_per_pixel/8,mask_bytes,-mask_pitch,mask_bits_per_pixel/8);
#if TT_DEBUG_ON
							if (tt_debug_mode == 31001) {
								tt_error_file() << (timeGetTime()-start_time)
												<< "ms to copy_and_mask_work_page " << endl;
								start_time = timeGetTime();
							};
#endif
						} else {
							log("Surface and mask are of different sizes. Ignoring the mask.");
							string file_name = return_file_name();
							if (file_name != NULL) {
								tt_error_file() << "The file is " << file_name << endl;
							};
						};
						unlock_surface(mask_surface);
						mask_surface->Release(); // don't need it anymore
						if (mask_surface_pointer != NULL) *mask_surface_pointer = NULL; // used it already
					};
					unlock_surface(surface);
					surface->Release(); // served its function
					return(image_bytes);
//				};
			};
//		};
	};
/*
#if TT_DIRECTX_TRANSFORM
	// why??
		draw_surface surface = load_image_using_directx_transform(full_file_name,TRUE,24,mask_surface_pointer);
#if TT_DEBUG_ON
		if (tt_debug_mode == 31001) {
			tt_error_file() << (timeGetTime()-start_time)
							<< "ms to load_image_using_directx_transform " << full_file_name << endl;
			start_time = timeGetTime();
		};
#endif
		if (surface != NULL) { 
			dib = surface_to_DIB(surface,transparent_color);
			int surface_width, surface_height, surface_bits_per_pixel;
			surface_size(surface,surface_width,surface_height,surface_bits_per_pixel);
			pixel_width = surface_width;
			pixel_height = surface_height;
			bits_per_pixel = tt_bits_per_pixel; // surface_bits_per_pixel; since it is converted by display_on_page
			set_dimensions();
			surface->Release(); // served its function
#if TT_DEBUG_ON
			if (tt_debug_mode == 31001) {
				tt_error_file() << (timeGetTime()-start_time)
								<< "ms to surface_to_DIB, etc." << endl;
				start_time = timeGetTime();
			};
#endif
#endif
		};
		*/
#endif
	if (dib == NULL) {
		if (!file_is_BMP) { // error handling added on 060600
			if (full_file_name != NULL) {
				tt_error_file() << "Couldn't load the picture file: " << full_file_name << endl;
#if !TT_DIRECTX_TRANSFORM&&!TT_IMGSOURCE&&!TT_GDIPLUS
				tt_error_file() << "This version of ToonTalk can only load BMP pictures. Sorry. " << endl;
#endif
				delete [] full_file_name;
				full_file_name = NULL;
			};
			return(NULL);
		};
		dib = DibOpenFile(full_file_name);
		if (dib != NULL) { // moved here on 290500
		   long width = dib->biWidth;
		   if (width&3) { // needs padding
		      width += 4-width%4;
		   };
			pixel_width = dib->biWidth;
			pixel_height = dib->biHeight;
		   image_size = width*dib->biHeight*tt_bits_per_pixel/8; // tt_bits_per_pixel part added on 230901
			bits_per_pixel = 8; // new on 121001 - what about non 8-bit BMPs?
		};
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == -201199 || tt_debug_mode == 150500) {
		tt_error_file() << "Loaded Dib of " << full_file_name << " as part of :retrieve_image" << endl;
	};
#endif
//	};
//   if (file == -1) {
//      say("Sorry, couldn't find a file named ");
//      say(file_name);
//      return(NULL);
//   };
//   LPBITMAPINFOHEADER dib = DibReadBitmapInfo(file);
//   DibMapToPalette(dib,tt_main_window->return_palette_handle());
   // SizeImage includes padding to word boundaries
//   DWORD SizeImage;
//   long width = dib->biWidth;
//   if (width&3) { // needs padding
//      width += 4-width%4;
//   };
//   SizeImage = width*dib->biHeight;
#if TT_32
   work_page image_bytes = new unsigned char[image_size];
#else
	work_page image_bytes = allocate_bytes(image_size,memory_handle);
#endif
#if TT_DEBUG_ON
	if (tt_debug_mode == 31001) {
		tt_error_file() << (timeGetTime()-start_time)
						<< "ms to allocate_bytes " << image_size << endl;
		start_time = timeGetTime();
	};
#endif
   if (tt_want_exception_breaks) {
	   display_on_page(image_bytes,dib,image_size);
   } else {
	   try { // new on 080200
		   display_on_page(image_bytes,dib,image_size);
	   } catch (...) {
		   log("Error occurred while trying to read a BMP file.",FALSE,TRUE);
		   tt_error_file() << "File is " << full_file_name << endl;
	   };
   };
//   if (saved_dib == NULL) GlobalFreePtr(dib);
#if TT_IMGSOURCE
	GlobalFreePtr(dib);
	bits_per_pixel = tt_bits_per_pixel;
//#else if TT_GDIPLUS
//	??
#else
	if (file_is_BMP) { // conditional new on 010201 - not sure why otherwise it caused an error in system DLLs
		GlobalFreePtr(dib); // new on 311000 - commented out on 220901 to avoid errors - restored on 151001
	} else {
		delete [] dib; // new on 200201 to fix a major leak with non-bmp pictures
	};
#endif
#if TT_DEBUG_ON
	if (tt_debug_mode == 31001) {
		tt_error_file() << (timeGetTime()-start_time)
						<< "ms to display_on_page " << endl;
	};
#endif
   return(image_bytes);
};


BackgroundImage::BackgroundImage() : 
  UserImage() {
	  set_rectangular_mask(TRUE); // new on 250901
	  resource_index = -1; // was -2 prior to 200303 but needs to be easier to test for built_in()
};

work_page BackgroundImage::retrieve_image(int &transparent_color, int &bits_per_pixel, boolean &adjust_picth, draw_surface *mask_surface) {
	// still used??
	if (file_name != NULL) {
		return(UserImage::retrieve_image(transparent_color,bits_per_pixel,adjust_picth,mask_surface));
	} else {
		return(TTImage::retrieve_image(transparent_color,bits_per_pixel,adjust_picth,mask_surface));
	};
};

boolean BackgroundImage::display() {
//	if (!dimensions_set()) {
//		compute_dimensions(return_file_name(),TRUE);
//	};
   if (file_name != NULL) { // new on 210999
		tt_screen->move_mark_to(0,0);
		boolean ignore;
		return(UserImage::display(tt_screen_width,tt_screen_height,ignore,::color_permutation(),0,0,NULL));
	};
	if (tt_video_mode > tt_graphics_video_mode_width) { // new on 150999
		tt_screen->move_mark_to(0,0);
		boolean ignore;
		return(TTImage::display(tt_screen_width,tt_screen_height,ignore,::color_permutation(),0,0,NULL)); // skip over the "User" part
	};
	draw_surface surface = load_image(TRUE); // ::color_permutation(),
	if (surface == NULL) return(FALSE); // new on 140400
#if TT_DIRECTX
//   lock_back_surface();
//   copy_work_page(image_bytes,tt_destination_work_page,
//                  tt_screen_width,tt_destination_width);
//   unlock_back_surface();
   blt_full_surface(surface);
#if TT_DEBUG_ON
	ULONG count = surface->Release();
	if (tt_debug_mode == 190301) {
		tt_error_file() << "BackgroundImage::display(): ref count after release is " << count << " size is " << image_size << endl;
	};
#else
	surface->Release(); // new on 280500
#endif
#else
   // following should use copy_work_page if I want to use prefered direction
	HMEMCPY(tt_destination_work_page, //tt_memory_graphics_work_page,
			  surface,
			  tt_screen_bytes_size);
#endif
	return(TRUE);
};

flag already_reported_loading_error = FALSE; // new on 061101 so error messages don't repeat

boolean TTImage::display(long &width, long &height, 
								 boolean &just_discovered_transparency, // new on 210203
							    bytes color_permutation, // new on 200901
							    unsigned short int x_scale, unsigned short int y_scale,
								 draw_surface image_surface) { // new on 081001 to avoid loading it twice
// new on 100399 to be more accurate and to have a wider range of scales (e.g. can be more than 256X)
// added args are width and height (in pixels) -- or can be zero and will be recomputed
	// starting 011100 returns FALSE if didn't display anything (e.g. off screen or too small)
#if TT_DEBUG_ON
	millisecond start_time = timeGetTime();
#endif
	if (width == 0) {
		width = (((long) pixel_width)*x_scale)>>8;
	};
	if (width <= 0) return(FALSE); // less than part added on 100400
	if (tt_memory_graphics_mark_x+width <= 0 || // off the left side -- was just < prior to 251102
		 tt_memory_graphics_mark_x >= tt_screen_width) {// or off the right -- was just > prior to 251102
		return(FALSE); // new on 011100 as an important optimization
	};
	if (height == 0) {
		height = (((long) pixel_height)*y_scale)>>8;
	};
	if (height <= 0) return(FALSE); // less than part added on 100400
	if (tt_memory_graphics_mark_y+height <= 0 || // off the bottom -- was just < prior to 251102
		 tt_memory_graphics_mark_y+1 >= tt_screen_height) {// or off top -- was just > prior to 251102 +1 new on 121204
		 return(FALSE); // new on 011100 as an important optimization
	};
	boolean stretching_needed = (width != pixel_width || height != pixel_height);
	boolean image_surface_provided_by_caller = (image_surface != NULL);
	// if scale is close enough to 1x1 then don't scale	
	if (stretching_needed) {
		draw_surface stretched_surface = cached_stretch((unsigned short) width, (unsigned short) height); // , color_permutation);
		if (stretched_surface == NULL) { // not cached
			if (image_surface == NULL) { // conditional new on 081001
				image_surface = load_image((resource_index != -2)); // color_permutation,
			};
			if (image_surface == NULL) return(TRUE); 
			// e.g. a bad user picture file (added 060600) -- didn't display anything but region is OK
//#if !TT_USING_DAT_FILE
//			if (image_surface == NULL && resource_index >= 0 && tt_picture_directory != NULL) {
//				string file_name = append_strings(tt_picture_directory,tt_image_file_names[resource_index]);
//				set_file_name(file_name);
//				delete [] file_name;
//				image_surface = load_image();
//				if (image_surface == NULL) return; // warn??
//			};
//#endif
         // might not need to cache stretched if direct_draw fast enough
         // for now use tt_debug_mode to try this out
			if (cache_stretch_if_ok((unsigned short) width, (unsigned short) height,x_scale,y_scale,image_surface)) { 
				// ,color_permutation
			  // done it
			} else {
#if TT_DIRECTX
            RECT surface_rectangle;
            surface_rectangle.left = 0;
            surface_rectangle.top = 0;
            surface_rectangle.right = pixel_width; 
            surface_rectangle.bottom = pixel_height;
            RECT destination_rectangle;
            destination_rectangle.left = tt_memory_graphics_mark_x;
				destination_rectangle.right = destination_rectangle.left+width; // -1 caused an error
				if (destination_rectangle.left < 0) {
					// seems to fix a serious bug on some systems by not letting these get negative 251102
					surface_rectangle.left -= (destination_rectangle.left*pixel_width)/width; 
					// cut off proportional to the expansion factor
					if (surface_rectangle.left < 0) { // new on 260904
						surface_rectangle.left = 0; 
					};
					destination_rectangle.left = 0;
				};				
				destination_rectangle.top = tt_screen_height-(height+tt_memory_graphics_mark_y+1); //+1 new on 121204 (issue 587)
            destination_rectangle.bottom = destination_rectangle.top+height;
				if (destination_rectangle.top < 0) {
					// seems to fix a serious bug on some systems by not letting these get negative 251102
					surface_rectangle.top -= (destination_rectangle.top*pixel_height)/height;
					if (surface_rectangle.top < 0) { // new on 260904
						surface_rectangle.top = 0; 
					};
					destination_rectangle.top = 0;
				};
				if (destination_rectangle.right > tt_screen_width) { // new on 251102 just to be safe let's clip
					surface_rectangle.right += ((tt_screen_width-destination_rectangle.right)*pixel_width)/width;
					if (surface_rectangle.right > tt_screen_width) { // new on 260904
						surface_rectangle.right = tt_screen_width; 
					};
					destination_rectangle.right = tt_screen_width; 
				};
				if (destination_rectangle.bottom > tt_screen_height) { // new on 251102 just to be safe let's clip
					surface_rectangle.bottom += ((tt_screen_height-destination_rectangle.bottom)*pixel_height)/height;
					if (surface_rectangle.bottom-destination_rectangle.top > pixel_height) { // new on 260904
						surface_rectangle.bottom = surface_rectangle.top+pixel_height; 
					};
					destination_rectangle.bottom = tt_screen_height;
				};
//#if TT_DEBUG_ON
//				if (tt_debug_mode == 121204) {
//					tt_error_file() << "destination_rectangle.top = " << destination_rectangle.top 
//										 << "; destination_rectangle.bottom = " << destination_rectangle.bottom 
//										 << "; difference = " << destination_rectangle.top-destination_rectangle.bottom 
//										 << "; frame = " << tt_frame_number << endl;
//				};
//#endif
				if (color_permutation == NULL) { 
					stretch_blt_to_back_surface(image_surface,surface_rectangle,destination_rectangle,!rectangular_mask);
				} else { // doing this here as of 291101
					draw_surface recolored_surface = surface_recolored(image_surface,color_permutation,transparent_color);
					stretch_blt_to_back_surface(recolored_surface,surface_rectangle,destination_rectangle,!rectangular_mask);
					recolored_surface->Release();
				};          
            // see if I need the transparent flag or
            // surface could remember??
#else
            if (image_compression) {
				   blit_pixelrun_scale_and_ignore(image_surface,image_size,
														    pixel_width,pixel_height,
														    x_scale,y_scale);
			   } else {
				   if (rectangular_mask) {
					   blit_scale(image_surface,
								     pixel_width,pixel_height,
								     x_scale,y_scale);
				   } else {
					   blit_scale_ignore(image_surface,
											   pixel_width,
											   pixel_height,
											   x_scale,y_scale);
               };
				};
#endif
			};
#if TT_DEBUG_ON
			if (!image_surface_provided_by_caller) {
				ULONG count = image_surface->Release(); // new 280500
				if (tt_debug_mode == 190301) {
					tt_error_file() << "Stretching: ref count after release is " << count 
									<< " size is " << image_size << endl;
					if (count == 0) {
						log("Zero ref count");
					};
				};
			};
#else
			if (!image_surface_provided_by_caller) { // conditional new on 101001
				image_surface->Release(); // new 280500
			};
#endif
#if TT_DEBUG_ON
			if (tt_debug_mode == 8) {
				tt_error_file() << C(IDS_SCALING_AND_DRAWING) << image_size << C(IDS_BYTES_LONG_IMAGE)
								<< resource_index << endl
								<< C(IDS_SCALED_TO) << ((pixel_width*x_scale)>>8) << "x";
				tt_error_file() << ((pixel_height*y_scale)>>8) << " in "
								<< (timeGetTime()-start_time) << " " << S(IDS_MILLISECONDS) << ".";
				if (rectangular_mask) {
					tt_error_file() << S(IDS_RECTANGULAR);
				};
				tt_error_file() << endl;
			};
#endif
		} else { // use the cache instead
//			if (width&3) { // low order bits set
//				width += 4-width%4; // adjust to long boundary
//			};
#if TT_DIRECTX
         RECT surface_rectangle;
         surface_rectangle.left = 0;
         surface_rectangle.top = 0;
         surface_rectangle.right = width; // true dimensions not padded
         surface_rectangle.bottom = height;
			if (color_permutation == NULL) { 
				blt_to_back_surface(stretched_surface,surface_rectangle,!rectangular_mask);
			} else { // doing this here as of 291101
				draw_surface recolored_surface = surface_recolored(stretched_surface,color_permutation,transparent_color);
				blt_to_back_surface(recolored_surface,surface_rectangle,!rectangular_mask);
				recolored_surface->Release();
			};
#else
			if (rectangular_mask) {
				blit(stretched_surface,width,height);
			} else {
				blit_ignore(stretched_surface,width,width,height);
			};
#endif
		};
	} else { // never was a need to stretch
//      if (resource_index >= 317 && resource_index <= 324) {
//         tt_error_file() << "About to load " << resource_index << endl;
//         tt_debug_mode = 10000;
//      } else {
//         tt_debug_mode = 11;
//      };
		if (image_surface == NULL) { // conditional new on 081001
			image_surface = load_image((resource_index != -2)); // color_permutation,
		};
      if (image_surface == NULL) {
         if (!already_reported_loading_error) {
				say_error(IDS_PROBLEM_LOADING_AN_IMAGE,TRUE);
				already_reported_loading_error = TRUE;
			};
         return(TRUE);
      };
#if TT_DIRECTX
//      lock_back_surface();
      RECT surface_rectangle;
      surface_rectangle.left = 0; 
      surface_rectangle.top = 0; 
      surface_rectangle.right = width; // true dimensions not padded
      surface_rectangle.bottom = height;
		if (color_permutation == NULL) { 
			blt_to_back_surface(image_surface,surface_rectangle,!rectangular_mask);
		} else { // doing this here as of 291101
			draw_surface recolored_surface = surface_recolored(image_surface,color_permutation,transparent_color);
			blt_to_back_surface(recolored_surface,surface_rectangle,!rectangular_mask);
			recolored_surface->Release();
		};
#if TT_DEBUG_ON
		if (!image_surface_provided_by_caller) {
			ULONG count = image_surface->Release(); // new 280500
			if (tt_debug_mode == 190301) {
				tt_error_file() << "Display: ref count after release is " << count << " size is " << image_size << endl;
				if (count == 0) {
					log("Zero ref count");
				};
			};
		};
#else
		if (!image_surface_provided_by_caller) { // conditional new on 101001
			image_surface->Release(); // new 280500
		};
#endif
#else
		if (image_compression) {
			blit_pixelrun_ignore(image_surface,image_size,pixel_width,pixel_height);
		} else {
			if (rectangular_mask) {
				blit(image_surface,pixel_width,pixel_height);
			} else {
				blit_ignore(image_surface,pixel_width,pixel_width,pixel_height);
			};
		};
//#if TT_DIRECTX
//      unlock_back_surface();
#endif
	};
	return(TRUE);
};

void TTImage::display_on(coordinate x, coordinate y,
							    work_page destination, 
							    int destination_width, int destination_height,
							    bytes permutation) {
	work_page good_destination_work_page = tt_destination_work_page;
	coordinate good_destination_width = tt_destination_width;
	coordinate good_destination_height = tt_destination_height;
	coordinate good_memory_graphics_mark_x = tt_memory_graphics_mark_x;
	coordinate good_memory_graphics_mark_y = tt_memory_graphics_mark_y;
	coordinate good_destination_llx = tt_destination_llx; // this and following new on 040100 so hat stays on head even if a follower
	coordinate good_destination_lly = tt_destination_lly; 
	coordinate good_destination_urx = tt_destination_urx;
	coordinate good_destination_ury = tt_destination_ury;
	unclip(FALSE); // new on 040100
	int min_x = max(0,destination_width-pixel_width);
	destination_width *= tt_bytes_per_pixel; // new on 021001
	tt_destination_llx *= tt_bytes_per_pixel; // new on 021001
	if (destination_width&3) { // low order bits set
		destination_width += 4-destination_width%4; // adjust to long boundary
	};
	tt_destination_work_page = destination;
	tt_destination_width = destination_width;
	tt_destination_height = destination_height;
	tt_memory_graphics_mark_x = min(x,min_x)*tt_bytes_per_pixel; // was just x prior to 120201 -- *tt_bytes_per_pixel added on 021001
	tt_memory_graphics_mark_y = y;
#if TT_DIRECTX
   // ironically its easiest to just use the raw bytes for this
//	int transparent_color = 0; -- made an instance variable on 021101
	int bits_per_pixel; // new on 230901
	boolean ignore_adjust_pitch;
	draw_surface mask_surface;
   work_page image_surface = retrieve_image(transparent_color,bits_per_pixel,ignore_adjust_pitch,&mask_surface); // added mask_surface on 021101
	if (image_surface == NULL) { // new on 100600
		return; // warn??
	};
//#if !TT_USING_DAT_FILE
//	if (image_surface == NULL && resource_index >= 0 && tt_picture_directory != NULL) {
//		string file_name = append_strings(tt_picture_directory,tt_image_file_names[resource_index]);
//		set_file_name(file_name);
//		delete [] file_name;
//		image_surface = retrieve_image();
//		if (image_surface == NULL) return; // warn??
//	};
//#endif
#else
   draw_surface image_surface = load_image(permutation,TRUE);
#endif
	if (image_compression) {
		blit_pixelrun_ignore(image_surface,image_size,pixel_width,pixel_height);
	} else {
		int width_in_bytes = pixel_width*bits_per_pixel/8;
		int width_with_padding = width_in_bytes;
		if (tt_builtin_picture_directory == NULL && width_with_padding&3) { // && !tt_using_directx_transform_or_gdiplus) { 
			// restored on 311001 but only when neither GDI+ nor direct_transform is active
			width_with_padding += 4-width_with_padding%4; // adjust to long boundary
		};	
		blit_ignore(image_surface,width_in_bytes,width_with_padding,pixel_height); // *bits_per_pixel/8 new on 021001
	};
	tt_destination_work_page = good_destination_work_page;
	tt_destination_width = good_destination_width;
	tt_destination_height = good_destination_height;
	tt_memory_graphics_mark_x = good_memory_graphics_mark_x; 
	tt_memory_graphics_mark_y = good_memory_graphics_mark_y;
	tt_destination_llx = good_destination_llx; // new on 040100
	tt_destination_lly = good_destination_lly; 
	tt_destination_urx = good_destination_urx;
	tt_destination_ury = good_destination_ury;
#if TT_DIRECTX
   delete [] image_surface;
#endif
};

//#define TT_DEBUG_ON 1 // bad
//#if TT_DIRECTX
// don't bother with the optimization though for small things it might
// be worth trying it out
//boolean TTImage::cache_stretch_if_ok(unsigned short ,
//											  unsigned short ,
//											  unsigned short ,
//											  unsigned short ,
//											  draw_surface ) {
//   return(TRUE);
//};
//#else
boolean TTImage::cache_stretch_if_ok(unsigned short current_pixel_width, unsigned short current_pixel_height,
											    unsigned short x_scale, unsigned short y_scale,
											    draw_surface image_surface) {
//											  bytes color_permutation
#if TT_DEBUG_ON
	//if (tt_debug_mode == 121204) {
	//	return(FALSE);
	//};
	if (tt_debug_mode == 150500) {
		tt_error_file() << "Checking whether can cache the stretched image ";
		string name = return_file_name();
		if (name != NULL) {
			tt_error_file() << name;
		} else {
			tt_error_file() << "#" << return_resource_index();
		};
		tt_error_file() << " with dimensions " << current_pixel_width << "x" << current_pixel_height << endl;
	};
#endif
	// commented out the following on 150500
//	if (current_pixel_width > tt_screen_width || current_pixel_height > tt_screen_height) {
//#if TT_DEBUG_ON
//		if (tt_debug_mode == 150500) {
//			tt_error_file() << " Width or height is too big." << endl;
//		};
//#endif
//		return(FALSE);
//	};
	if (current_pixel_width > max_short || current_pixel_height > max_short) { // new on 020401 since causes problems on some systems
		return(FALSE);
	};
//	unsigned short padded_pixel_width;
	int padded_byte_width = current_pixel_width*tt_bytes_per_pixel; // new on 240901
	if (padded_byte_width&3) { // low order bits set
		// adjust to long boundary
		padded_byte_width += 4-padded_byte_width%4;
	};
	unsigned long size = padded_byte_width*current_pixel_height; // why both making it unsigned??
	if (size > (unsigned long) tt_screen_bytes_size) { // no need to check memory available if a screen full 
		unsigned long memory_available = compute_memory_available();
		if (size > memory_available/20) { // new on 010401 if picture is more than 5% of total available memory then don't cache it
			return(FALSE);
			// why bother to cache if not enough room
//			tt_error_file() << "Creating a picture that is stretched to be " << current_pixel_width << " by " << current_pixel_height << " would exceed 5% of available memory. Picture is being made smaller, sorry." << endl;
//			long percentage_shrinkage = (long) (((int64) memory_available*5)/((int64) size));
//			if (percentage_shrinkage < 1) {
//				percentage_shrinkage = 1; // and try again if still not enough
//			} else if (percentage_shrinkage > 99) {
//				percentage_shrinkage = 50; // cut in half and try again if need be
//			};
//			return(cache_stretch_if_ok((current_pixel_width*percentage_shrinkage)/100,
//												(current_pixel_height*percentage_shrinkage)/100,
//												0,0,image_surface));
		};
	};
	// chose not to deal with big items for now
	// on 150500 decided to go ahead and allow them (e.g. arm when running 800x600)
//	if (size > max_unsigned_short) {
//#if TT_DEBUG_ON
//		if (tt_debug_mode == 150500) {
//			tt_error_file() << " Size is too big." << endl;
//		};
//#endif
//		return(FALSE); 
//	};
	if (candidate_pixel_width != current_pixel_width ||
		 candidate_pixel_height != current_pixel_height) {
		if (tt_current_time-candidate_when_cached > 2000) { 
#if TT_DEBUG_ON
			if (tt_debug_mode == 150500) {
				tt_error_file() << " More than 2 seconds since last use so switching to new candidate." << endl;
			};
#endif
			// been more than 2 seconds since candidate first considered
			// and not re-used
			candidate_when_cached = tt_current_time;
			// candidate_savings_per_use computed later
			candidate_used_count = 1;
			candidate_pixel_width = current_pixel_width;
			candidate_pixel_height = current_pixel_height;
		} else {
			// another candidate being considered now
#if TT_DEBUG_ON
			if (tt_debug_mode == 150500) {
				tt_error_file() << " Waiting for another candidate." << endl;
			};
#endif
			return(FALSE);
		};
	};
	candidate_used_count++;
	if (candidate_when_cached == tt_current_time) {
		// just another copy of the same image
		return(FALSE);
	};
#if TT_DIRECTX
   draw_surface surface = create_surface(current_pixel_width,current_pixel_height,transparent_color); // transparent_color added on 021101
	if (surface == NULL) return(FALSE); // new on 010401
   RECT surface_rectangle;
   surface_rectangle.left = 0;
   surface_rectangle.top = 0;
   surface_rectangle.right = pixel_width; 
   surface_rectangle.bottom = pixel_height;
   RECT destination_rectangle;
   destination_rectangle.left = 0;
   destination_rectangle.top = 0;
   destination_rectangle.right = destination_rectangle.left+current_pixel_width; 
   destination_rectangle.bottom = destination_rectangle.top+current_pixel_height; 
	millisecond start_time = timeGetTime();
   stretch_blt_to_another_surface(image_surface,surface_rectangle,
                                  surface,destination_rectangle,
                                  !rectangular_mask);
//	set_transparent_color(surface,transparent_color); // not clear why this helps - 021101
   size += 100; // some overhead to surfaces -- good guess??
   // now really do the blt
   millisecond stretch_end = timeGetTime();
	millisecond stretch_duration = stretch_end-start_time;
   blt_to_back_surface(surface,destination_rectangle,!rectangular_mask);
#else
	bytes memory = new unsigned char[size];
	memset(memory,0,size);
	work_page good_destination_work_page = tt_destination_work_page;
	coordinate good_destination_width = tt_destination_width;
	coordinate good_destination_height = tt_destination_height;
	coordinate good_memory_graphics_mark_x = tt_memory_graphics_mark_x;
	coordinate good_memory_graphics_mark_y = tt_memory_graphics_mark_y;
	coordinate good_destination_llx = tt_destination_llx;
	coordinate good_destination_lly = tt_destination_lly;
	coordinate good_destination_urx = tt_destination_urx;
	coordinate good_destination_ury = tt_destination_ury;
	unclip(FALSE); // cache display of the whole thing even if clipping is on
	tt_destination_work_page = memory;
	tt_destination_width = padded_pixel_width;
   tt_destination_height = current_pixel_height;
	tt_memory_graphics_mark_x = 0;
	tt_memory_graphics_mark_y = 0;
	millisecond start_time = timeGetTime();
	// keep the following in synch with same code in TTImage::display
	if (image_compression) {
		blit_pixelrun_scale_and_ignore(image_surface,image_size,
												 pixel_width,pixel_height,
												 x_scale,y_scale);
	} else {
		if (rectangular_mask) {
			blit_scale(image_surface,
						  pixel_width,pixel_height,
						  x_scale,y_scale);
		} else {
			blit_scale_ignore(image_surface,
									pixel_width,  // padding,
									pixel_height,
									x_scale,y_scale);
		};
	};
	tt_destination_work_page = good_destination_work_page;
	tt_destination_width = good_destination_width;
	tt_destination_height = good_destination_height;
	tt_memory_graphics_mark_x = good_memory_graphics_mark_x;
	tt_memory_graphics_mark_y = good_memory_graphics_mark_y;
	tt_destination_llx = good_destination_llx;
	tt_destination_lly = good_destination_lly;
	tt_destination_urx = good_destination_urx;
	tt_destination_ury = good_destination_ury;
	millisecond stretch_end = timeGetTime();
	millisecond stretch_duration = stretch_end-start_time;
	if (rectangular_mask) {
		blit(memory,current_pixel_width,current_pixel_height);
	} else {
		blit_ignore(memory,current_pixel_width,current_pixel_width,current_pixel_height);
	};
#endif
	long savings = stretch_duration-(timeGetTime()-stretch_end);
	if (savings <= 0) {
		candidate_savings_per_use = 1;
#if TT_DEBUG_ON
      if (tt_debug_mode == -1 && savings < 0 && stretch_duration > 0) {
         // see if it ever happens other than sub millisecond sampling error
         tt_error_file() << S(IDS_NEGATIVE_SAVINGS_STRETCH_CACHE)
                     << stretch_duration << S(IDS_MS_TO_STRETCH)
                     << (timeGetTime()-stretch_end) << C(IDS_MS_TO_BLIT_RESULT) << endl;
         tt_error_file() << C(IDS_ORIGINAL_SIZE_IS) << pixel_width << "x" << pixel_height
                     << C(IDS_NEW_SIZE_IS) << current_pixel_width << "x" << current_pixel_height << endl;
      };
#endif
	} else {
		candidate_savings_per_use = (unsigned short) savings;
	};
	// add memory to cache 
	candidate_when_cached = 0;  // ready to consider a new candidate
	unsigned short cache_index;
	if (!free_cache_index(size,
								 candidate_speed_up_reciprocal(),
								 cache_index)) {
		 // some problems freeing cache (e.g. this guy is worse than those there)
#if TT_DEBUG_ON
		if (tt_debug_mode == 150500 || tt_debug_mode == 190301) {
			tt_error_file() << "Couldn't free a cache index." << endl;
		};
#endif
#if TT_DIRECTX
		if (surface != NULL) { // condition added 010401 since if low on memory can happen
#if TT_DEBUG_ON
			ULONG count = surface->Release();
			if (tt_debug_mode == 190301) {
				tt_error_file() << "TTImage::cache_stretch_if_ok: ref count after release is " << count << " size is " << size << endl;
				if (count == 0) {
					log("Zero ref count");
				};
			};
#else
			surface->Release(); 
#endif
		};
#else
		delete [] memory;
#endif
/*
#if TT_DEBUG_ON
		 if (tt_debug_mode == 3) {
			 tt_error_file() << S(IDS_FREE_CACHE_FAILED) 
							 << resource_index << S(IDS_WITH_SIZE)
							 << current_pixel_width << "x" << current_pixel_height;
			 tt_error_file() << S(IDS_WHOSE_SPEED_IS) << candidate_speed_up_reciprocal()
							 << S(IDS_AND_SAVINGS_IS) << candidate_savings_per_use
							 << endl;
		 };
#endif
*/
		return(TRUE); // at least displayed
	};
/*
#if TT_DEBUG_ON
	if (tt_debug_mode == 3) {
		tt_error_file() << S(IDS_CACHED_IMAGE) 
						<< resource_index << S(IDS_WITH_SIZE)
						<< current_pixel_width << "x" 
						<< current_pixel_height << S(IDS_AT_CACHE)
						<< cache_index;
		tt_error_file()	<< S(IDS_WHOSE_SPEED_IS) << candidate_speed_up_reciprocal()
						<< S(IDS_AND_SAVINGS_IS) << candidate_savings_per_use
						<< endl;
	};
#endif	
*/
#if TT_DEBUG_ON
   if (tt_debug_mode == 3) {
      tt_error_file() << savings << C(IDS_MS_SAVED_BY_STRETCH_CACHE)
                  << size << C(IDS_BYTES_FOR_RESULT) << cache_index << endl;
   } else if (tt_debug_mode == 150500) {
		tt_error_file() << "Assigned cache index #" << cache_index << " with savings of " << savings << endl;
	};
#endif
#if TT_DIRECTX
	add_candidate_to_cache((bytes) surface,size,cache_index,DIRECT_DRAW_SURFACE); // ,color_permutation
#else
	add_candidate_to_cache(memory,size,cache_index,color_permutation);
#endif
	return(TRUE);
};
//#endif
//#define TT_DEBUG_ON 1 ok

void TTImage::add_candidate_to_cache(bytes memory, unsigned long size, 
											  unsigned short cache_index
#if TT_DIRECTX
                                   ,DirectXObject type
#endif
                                   ) { // bytes color_permutation
#if TT_CAREFUL
	if (candidate_savings_per_use == 0 || candidate_used_count == 0) {
		tt_error_file() << S(IDS_BAD_CACHE_ENTRY)
						<< candidate_savings_per_use << " " << candidate_used_count << endl;
		candidate_savings_per_use = 1;
		candidate_used_count	= 1;
	};
#endif
	cache_entries[cache_index].initialize(candidate_when_cached,memory,size,candidate_savings_per_use,candidate_used_count);
#if TT_DIRECTX
   cache_entries[cache_index].set_directx_type(type);
#endif
	stretch_blit_cache_size++;
	StretchBlitCacheEntryPointer *new_stretch_blit_cache = new StretchBlitCacheEntryPointer[stretch_blit_cache_size];
	int i;
	for (i = 0; i < stretch_blit_cache_size-1; i++) {
		new_stretch_blit_cache[i] = stretch_blit_cache[i];
	};
	new_stretch_blit_cache[i] = new StretchBlitCacheEntry(candidate_pixel_width,
																		   candidate_pixel_height,
																		   cache_entries[cache_index].return_cache_id(),
																		   cache_index); // color_permutation
	if (stretch_blit_cache != NULL) {
		delete [] stretch_blit_cache;
	};
	stretch_blit_cache = new_stretch_blit_cache;
};

draw_surface TTImage::cached_stretch(unsigned short pixel_width, unsigned short pixel_height) { // , bytes color_permutation) {
  for (int i = 0; i < stretch_blit_cache_size; i++) {
	  if (stretch_blit_cache[i]->pixel_width == pixel_width &&
			stretch_blit_cache[i]->pixel_height == pixel_height) {
//			stretch_blit_cache[i]->color_permutation == color_permutation
		  // found one
		  unsigned short index = stretch_blit_cache[i]->cache_index;
		  if (cache_entries[index].return_cache_id() != stretch_blit_cache[i]->cache_id) {
			  // memory has been re-used cache is old -- move to end and return NULL
			  delete stretch_blit_cache[i];
           int j;
			  for (j = i; j < stretch_blit_cache_size-1; j++) {
				  stretch_blit_cache[j] = stretch_blit_cache[j+1];
			  };
			  stretch_blit_cache[j] = NULL;
			  stretch_blit_cache_size--;
			  if (stretch_blit_cache_size == 0) {
				  delete [] stretch_blit_cache;
				  stretch_blit_cache = NULL;
			  };
			  return(NULL);
		  };
		  cache_entries[index].increment_used_count();
		  return((draw_surface) cache_entries[index].return_memory());
	  };
  };
  return(NULL);
};

unsigned short TTImage::increase_x_scale(unsigned long x_scale, int pixel_growth) {
	// returns x_scale increased to be pixel_growth bigger
	// cast x_scale to long for large objects to avoid overflow
	return((unsigned short) ((x_scale*(pixel_width+pixel_growth))/pixel_width));
};

unsigned short TTImage::increase_y_scale(unsigned long y_scale, int pixel_growth) {
	// returns y_scale increased to be pixel_growth bigger
	return((unsigned short) ((y_scale*(pixel_height+pixel_growth))/pixel_height));
};  

void TTImage::how_to_scale_to(city_coordinate goal_width, city_coordinate goal_height,
									   unsigned short int &x_scale, unsigned short int &y_scale) {
	int64 int64_x_scale = (((int64) 256*goal_width)/original_width); // was long prior to 150904
	int64 int64_y_scale = (((int64) 256*goal_height)/original_height);
	boolean ok = TRUE;
	if (int64_x_scale < max_unsigned_short) { // overflow checking introduced on 280500
		x_scale = (unsigned short) int64_x_scale;
	} else {
		x_scale = max_unsigned_short;
		ok = FALSE;
	};
	if (int64_y_scale < max_unsigned_short) {
		y_scale = (unsigned short) int64_y_scale;
	} else {
		y_scale = max_unsigned_short;
		ok = FALSE;
	};
	if (ok) {
		// check for round off problems -- important for stretch caching to
		// avoid a black line here and there
		// divide by 100 to avoid overflows since not in pixel coordinates
		while (((x_scale*original_width/100)/256) < goal_width/100) {
			x_scale++; 
		};
		while (((y_scale*original_height/100)/256) < goal_height/100) {
			y_scale++; 
		};
	};
};

draw_surface TTImage::load_image_internal(boolean palette_ok) { // removed bytes color_permutation, on 291101
	if (!has_initialization_completed()) return(NULL); // new on 270901 since might be loading to get geometry right but it is too early
//	int transparent_color = 0; -- made an instance variable on 021101
	draw_surface mask_surface = NULL; // new on 270901
	work_page image_bytes = NULL;
	int image_bits_per_pixel; // new on 230901
	boolean adjust_pitch = TRUE;
#if TT_DEBUG_ON
	millisecond start_time;
	if (tt_debug_mode == 31001) {
		start_time = timeGetTime();
	};
#endif
#if TT_GDIPLUS||TT_DIRECTX_TRANSFORM // TT_DIRECTX_TRANSFORM added on 291001
	if (tt_builtin_picture_directory != NULL) {
		if (!compute_full_file_name()) {
			if (return_file_name() != NULL) {
				tt_error_file() << "Troubles finding the file " << return_file_name() << endl;
			} else {
				tt_error_file() << "Troubles finding an image file. Index is " << resource_index << endl;
			};
		};
	};
	string full_file_name = return_full_file_name();
	if (full_file_name != NULL) { // new on 011101
		draw_surface surface;
		// what about using tt_prefer_gdiplus here?? 251001
		if (ini_file) { // new on 231001
			surface = surfaces_from_ini_file(full_file_name,TRUE,palette_ok,tt_bits_per_pixel,&mask_surface); // ,color_permutation
		} else {
//			surface = load_image_using_gdiplus(return_full_file_name(),TRUE,tt_bits_per_pixel,NULL,color_permutation);
			bytes fill_in_hash = NULL;
			if (image_hash == NULL && strncmp(full_file_name,tt_private_media_directory,tt_private_media_directory_length) == 0) {
				// new on 280403 that full_file_name is private media but hash lost
				image_hash = string_to_hash(full_file_name+tt_private_media_directory_length);
			} else if (image_hash == NULL && !built_in() && tt_maintaining_private_media_directory) {
				fill_in_hash = new byte[hash_byte_length];
				image_hash = fill_in_hash;
			};
			surface = load_any_image(full_file_name,tt_bits_per_pixel,TRUE,palette_ok,NULL,&transparent_color,fill_in_hash,this); 
			// updated 291001 -- ,color_permutation
			//if (fill_in_hash != NULL) { // just generated a hash and saved this
			//	set_stored_in_private_media_directory(TRUE);
			//};
		};
#if TT_DEBUG_ON
		if (tt_debug_mode == 31001) {
			tt_error_file() << (timeGetTime()-start_time) << "ms to retrieve an image whose size is " << image_size;
			//if (color_permutation != NULL) {
			//	tt_error_file() << " and its color was changed. ";
			//};
			tt_error_file() << endl;
		};
#endif
		if (mask_surface != NULL) {
#if TT_DEBUG_ON
			if (rectangular_mask && tt_debug_mode == 141201) { // unneeded mask
				tt_error_file() << full_file_name << " needs no mask." << endl;
				rectangular_mask = FALSE; // so I don't get duplicates
			};
#endif
			bytes surface_bytes;
			int pitch, bits_per_pixel;
			int surface_width, surface_height;
			just_lock_surface(surface,surface_bytes,pitch,bits_per_pixel,surface_width,surface_height);
			process_mask_surface(mask_surface,surface_width,surface_height,pixel_width,pixel_height,surface_bytes,pitch,bits_per_pixel); 
			// this releases mask_surface
			unlock_surface(surface);
			set_transparent_color(surface,0); // new on 071101
			mask_surface = NULL; // used 
		};
		if (need_to_clobber_image(resource_index)) { // hat or hair hack
			if (surface == NULL) { // couldn't find image - new on 311001
				return(NULL);
			};
			bytes surface_bytes;
			int pitch, bits_per_pixel;
			int surface_width, surface_height;
			local_lock_surface(surface,surface_bytes,pitch,bits_per_pixel,surface_width,surface_height);
			int image_pitch = surface_width*tt_bytes_per_pixel; 
			if (image_pitch&3) image_pitch += 4-image_pitch%4;
			int size = image_pitch*surface_height;
			image_bytes = new byte[size];
			copy_work_page(surface_bytes,image_bytes,-pitch,image_pitch,surface_width,surface_height);
//			memcpy(image_bytes,surface_bytes,size);
			unlock_surface(surface);
			surface->Release(); // done with it
			image_bits_per_pixel = bits_per_pixel;
		} else {
			return(surface);
		};
	};
#endif
	if (image_bytes == NULL) {
		image_bytes = retrieve_image(transparent_color,image_bits_per_pixel,adjust_pitch,&mask_surface);
//		image_bits_per_pixel = 8;
	};
   if (image_bytes == NULL) return(NULL); // new on 140300 -- moved here on 291002 so the following warnings don't apply
	if (image_bits_per_pixel != tt_bits_per_pixel) { // something wrong - new on 230901
		if (image_bits_per_pixel < 8) {
			tt_error_file() << "Something wrong with built-in artwork. Re-installation of ToonTalk might be necessary." << endl;
		} else {
			tt_error_file() << "Artwork has different color depth than expected. " << endl;
		};
//		return(NULL);
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 31001) {
		tt_error_file() << (timeGetTime()-start_time)
						<< "ms to retrieve an image whose size is " << image_size;
		//if (color_permutation != NULL) {
		//	tt_error_file() << " and its color was changed. ";
		//};
		tt_error_file() << endl;
	};
#endif
#if TT_DIRECTX
   draw_surface surface;
#else
   draw_surface surface = image_bytes;
#endif
//	if (image == NULL) {
//		 tt_error_file() << "Problems loading image# " << resource_index << endl;
//		 tt_exit_failure();
//	};
	bytes original_image_bytes = (bytes) image_bytes;
	// the following copies and clobbers the image bytes because
	// it used to be the case that I needed to permute the color indices
	// of all images anyway.  Re-consider this now
	if (need_to_clobber_image(resource_index)) {
//		bytes other_image_bytes;
		// ok to clobber original is not a Windows resource
		image_bytes = original_image_bytes;
		int offset_from_first;
		if (resource_index >= PERSON_WALKING_WEST1_IMAGE) {
			offset_from_first = (1+PERSON_WALKING_SOUTHEAST8_IMAGE-PERSON_WALKING_EAST1_IMAGE)+(resource_index-PERSON_WALKING_WEST1_IMAGE);
		} else {
			offset_from_first = resource_index-PERSON_WALKING_EAST1_IMAGE;
		};
		TTImage *head_image;
		SpriteCode head_type;
		switch (tt_programmer_head) {
			case 1:
				head_type = HAIR_SPRITE;
				break;                                                        			
			case 2:
				head_type = HAT_SPRITE;
				break;
			// no need to handle case 0: since never called in that case
		}; // already verified programmer_head is 0, 1 or 2
		head_image = image_cycles[head_type][0].nth_image(offset_from_first);
		// here's where I blit head_image onto image_bytes
//		int image_width = image->bmiHeader.biWidth;
//		int image_height = image->bmiHeader.biHeight;
		int units_per_vertical_pixel;
//		if (tt_screen_width <= 640) {
//			units_per_horizontal_pixel = tt_ideal_units_per_horizontal_pixel;
//		} else { // new on 170999 since graphics file is still based on 640x480
//			units_per_horizontal_pixel = 50;
//		};
		if (tt_screen_height <= tt_graphics_video_mode_height) { // should this really be using tt_screen_height?? ok when -f 2 ??
			units_per_vertical_pixel = tt_ideal_units_per_vertical_pixel;
		} else if (tt_graphics_video_mode_height == 240) { // new on 140300 
			units_per_vertical_pixel = 100;
		} else {
			units_per_vertical_pixel = 50;
		};
//		int head_width = maximum_width(head_type)/units_per_horizontal_pixel;
//		if (tt_screen_width > 640) { // new on 170999
//			head_width = (640*head_width)/tt_screen_width;
//		};
		int head_height = maximum_height(head_type)/units_per_vertical_pixel;
//		if (tt_screen_height > 480) { // new on 170999
//			head_height = (480*head_height)/tt_screen_height;
//		};
		coordinate head_x_offset = ideal_horizontal_pixel_length(head_image->current_x_offset()); //head_image->current_x_offset()/tt_ideal_units_per_horizontal_pixel;
		coordinate head_y_offset = ideal_vertical_pixel_length(head_image->current_y_offset()); //head_image->current_y_offset()/tt_ideal_units_per_vertical_pixel;
//		head_y_offset = image_height-head_image->current_pixel_width()+head_y_offset;
		coordinate image_x_offset = ideal_horizontal_pixel_length(x_offset); //x_offset/tt_ideal_units_per_horizontal_pixel;
		coordinate image_y_offset = ideal_vertical_pixel_length(y_offset); // y_offset/tt_ideal_units_per_vertical_pixel;
		coordinate head_llx = head_x_offset-image_x_offset;
		coordinate head_lly = head_y_offset-image_y_offset;
//		if (head_llx+head_width > pixel_width) { // too wide
//			head_llx = pixel_width-head_width;
//		};
		if (head_lly+head_height > pixel_height) { // too high
			head_lly = pixel_height-head_height;
		};
		head_image->display_on(head_llx,head_lly,
									  image_bytes,pixel_width,pixel_height,NULL); // ,color_permutation);
//		image->bmiHeader.biYPelsPerMeter = ALREADY_ALTERED;
//		head_image->free_resources();
//		cached_resources[resource_index] = image_bytes;
	};
#if TT_DEBUG_ON
   millisecond start;
   if (tt_debug_mode == 3) start = timeGetTime();
#endif
//	if (transparent_color == 253) transparent_color = image_bytes[0]; // new on 070600
   surface = create_surface(pixel_width,pixel_height,transparent_color);
	if (surface == NULL) return(NULL); // new on 010401
   bytes surface_bytes;
   int pitch, bits_per_pixel;
	int surface_width, surface_height;
   local_lock_surface(surface,surface_bytes,pitch,bits_per_pixel,surface_width,surface_height);
   if (image_compression) {
		// this might need to be enhanced for more than 8 bits per pixel
//		if (color_permutation != NULL) { // new on 190901 -- revised on 291101
//			bytes_from_RLE8_and_permute(image_bytes,pixel_width,-pitch,surface_bytes,color_permutation);
//		} else {
			bytes_from_RLE8(image_bytes,pixel_width,-pitch,surface_bytes);
//		};
   } else {
      coordinate source_pitch = pixel_width*tt_bytes_per_pixel; // multiply here starting 240901 so padding takes into account pixel width
      if (source_pitch&3 && adjust_pitch) source_pitch += 4-source_pitch%4;
//		if (color_permutation != NULL) {
//			recolor_work_page(image_bytes,surface_bytes,source_pitch,-pitch,
//									pixel_width,pixel_height,color_permutation,transparent_color);
//		} else {
			copy_work_page(image_bytes,surface_bytes,source_pitch,-pitch,
								pixel_width,pixel_height);
//		};
   };
	if (mask_surface != NULL) {
		process_mask_surface(mask_surface,surface_width,surface_height,pixel_width,pixel_height,surface_bytes,pitch,bits_per_pixel);
	};
   unlock_surface(surface);
   delete [] image_bytes; // test 251001
#if TT_DEBUG_ON
   if (tt_debug_mode == 3) {
      tt_error_file() << (timeGetTime()-start) << C(IDS_MS_CREATED_CACHED_SURFACE);
      if (image_compression) tt_error_file() << C(IDS_PARENTHETICALLY_COMPRESSED);
      tt_error_file() << endl;
   };
#endif
	return(surface);
};

void TTImage::process_mask_surface(draw_surface mask_surface, int main_surface_width, int main_surface_height,
											  int pixel_width, int pixel_height, bytes surface_bytes, int pitch, int bits_per_pixel) {
	if (mask_surface != NULL) { // new on 270901
		bytes mask_bytes;
		int mask_pitch, mask_bits_per_pixel;
		int mask_width, mask_height;
		just_lock_surface(mask_surface,mask_bytes,mask_pitch,mask_bits_per_pixel,mask_width,mask_height);
		if (mask_width == main_surface_width && mask_height == main_surface_height) { // check if mask and main surface have same width and height
			mask_work_page(pixel_width,pixel_height,surface_bytes,pitch,bits_per_pixel/8,mask_bytes,mask_pitch,mask_bits_per_pixel/8); 
			// pitch was -pitch and mask_pitch was -mask_pitch prior to 231001
		} else {
			log("Surface and mask are of different sizes. Ignoring the mask.");
			string file_name = return_full_file_name();
			if (file_name != NULL) {
				tt_error_file() << "The file is " << file_name << endl;
			};
		};
		unlock_surface(mask_surface);
		mask_surface->Release(); // don't need it anymore
	}; 
};

#if TT_DEBUG_ON
long total_image_size = 0;
long total_memory_change = 0;
#endif

draw_surface TTImage::load_image(boolean palette_ok) { // removed bytes color_permutation, on 291101
	if (cache_entries[image_cache_entry_index].return_cache_id() == image_cache_id) {
//		 cache_entries[image_cache_entry_index].color_permutation() == color_permutation
		cache_entries[image_cache_entry_index].increment_used_count();
		draw_surface cached_surface = (draw_surface) cache_entries[image_cache_entry_index].return_memory();
#if TT_DEBUG_ON
		ULONG count = cached_surface->AddRef();
		if (tt_debug_mode == 190301) {
			tt_error_file() << "Load_image - cached: ref count after AddRef is " 
								 << " cache entry # " << cache_entries[image_cache_entry_index].return_cache_entry_debug_counter()
								 << count << " size is " << image_size << endl;
		};
#else
      if (cached_surface != NULL) { // conditional added on 211102 since a crash report had it NULL (probably due to recently fixed cache bug)
         cached_surface->AddRef(); // new on 280500
      }; // else could try to repair the cache but probably not worth the effort
#endif	
		return(cached_surface);
	};
	millisecond start_time = timeGetTime();
#if TT_DEBUG_ON
	MEMORYSTATUS memory_status;
   memory_status.dwLength = sizeof(MEMORYSTATUS);
   GlobalMemoryStatus(&memory_status);
	long memory_available_before = memory_status.dwAvailPageFile;
#endif
	draw_surface surface = load_image_internal(palette_ok); // remodularized on 230500
#if TT_DEBUG_ON
	if (tt_debug_mode == 31001) {
		GlobalMemoryStatus(&memory_status);
		long memory_available_after = memory_status.dwAvailPageFile;
		total_image_size += image_size;
		total_memory_change += (memory_available_before-memory_available_after);
		tt_error_file() << (timeGetTime()-start_time)
							 << "ms to load an image whose size is " << image_size
							 << " and available memory shrank by " << (memory_available_before-memory_available_after) 
							 <<  " to become " << memory_available_after;		
		//if (color_permutation != NULL) {
		//	tt_error_file() << " and its color was changed. ";
		//};
		tt_error_file() << endl;
		tt_error_file() << "Total images is " << total_image_size << " while total memory change is " << total_memory_change << endl;
	};
#endif
	if (surface == NULL) {
		return(NULL); // new on 280500
	};
/* -- commented out on 190301 since this was a serious memory leak on small memory machines
#if TT_DEBUG_ON
		ULONG count = surface->AddRef();
		if (tt_debug_mode == 190301) {
			tt_error_file() << "Load TTImage: ref count after AddRef is " << count << " size is " << image_size << endl;
		};
#else
		surface->AddRef(); // new on 280500
#endif
		*/
	if (free_cache_index(image_size,
								2*image_size, // optimistic initial reciprocal (50% savings)
								image_cache_entry_index)) {
		cache_entries[image_cache_entry_index].increment_cache_id();
		image_cache_id	= cache_entries[image_cache_entry_index].return_cache_id();
#if TT_DIRECTX
      cache_entries[image_cache_entry_index].set_memory((bytes) surface);
#if TT_DEBUG_ON
		ULONG count = surface->AddRef();
		if (tt_debug_mode == 190301) {
			tt_error_file() << "Load TTImage (freed cache): ref count after AddRef is " << count 
								 << " cache entry # " << cache_entries[image_cache_entry_index].return_cache_entry_debug_counter()
								 << " size is " << image_size << endl;
		};
#else
		surface->AddRef(); // new on 280500
#endif
      cache_entries[image_cache_entry_index].set_directx_type(DIRECT_DRAW_SURFACE);
#else
		cache_entries[image_cache_entry_index].memory = image_bytes;
#endif
		cache_entries[image_cache_entry_index].set_memory_size(image_size);
		cache_entries[image_cache_entry_index].set_used_count(1);
		cache_entries[image_cache_entry_index].set_savings_per_use(compute_savings_per_use(start_time));
		cache_entries[image_cache_entry_index].set_when_cached(tt_current_time);
//		cache_entries[image_cache_entry_index].set_color_permutation(color_permutation);
#if !TT_32
		cache_entries[image_cache_entry_index].memory_handle = memory_handle;
#endif
#if TT_DEBUG_ON
		if (tt_debug_mode == 3) {
			tt_error_file() << S(IDS_CACHE_IMAGE) << resource_index
								 << S(IDS_USING_CACHE) << image_cache_entry_index
								 << S(SIZE_IS) << image_size << endl;
		};
#endif
	} else {
		// commented out on 190301 since caller releases surface when done now
		if (tt_deleted_memory == NULL || !tt_deleted_memory->has_member(surface)) { // condition new on 300105
			tt_deleted_memory = new DeletedMemoryDrawSurface(surface,tt_deleted_memory); 
			// restored on 260204 to fix a serious memory leak (on small memory systems or with very large pictures)
//#if TT_DEBUG_ON
//		} else {
//			debug_this();
//#endif
		} else {
			tt_error_file() << "Avoided double memory deletion" << endl; // just for testing 300105
		};
#if TT_DEBUG_ON
		total_image_size -= image_size; // new on 260204
		if (tt_debug_mode == 190301) {
			int debug_counter = cache_entries[image_cache_entry_index].return_cache_entry_debug_counter();
			tt_error_file() << "Surface added to deleted memory. "
								 << " cache entry # " << debug_counter << " Size is " << image_size << endl;
		};
#endif
   };
	return(surface);
};

work_page TTImage::retrieve_image(int &transparent_color, int &bits_per_pixel, boolean &adjust_pitch, draw_surface *mask_surface) {
// debugin
//   if (resource_index >= 316 && resource_index <= 318) {
//      message_box(MB_OK,"Retrieving suspect image");
//      pause_toontalk();
//      tt_error_file() << "debuggin" << endl;
//      unpause_toontalk();
//   };
	long image_size = image_offsets[resource_index]-image_offsets[resource_index-1];
#if TT_DEBUG_ON
	if (tt_debug_mode == 241299 && images_retrieved[resource_index] == 0) {
		images_retrieved[resource_index] = image_size;
	};
#endif
	if (image_size <= 0 || image_size > 500000L) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 241299 && image_size == 0) {
			images_retrieved[resource_index] = -1; // indicator that it was used and is missing
		};
#endif
      say_error(IDS_IMAGE_SIZE_WRONG);
		tt_error_file() << "Resource index was " << resource_index << endl;
      return(NULL);
   };
	if (mask_surface != NULL) { // new on 270901
		*mask_surface = NULL;
	};
#if TT_32
	work_page image_bytes = new unsigned char[image_size];
//	HGLOBAL memory_handle = NULL;
#else
	work_page image_bytes = allocate_bytes(image_size,memory_handle);
#endif
	_llseek(images_file,image_offsets[resource_index-1],0);
   _hread(images_file,(bytes) image_bytes,image_size);
//	_llseek(images_file,compressed_image_offsets[index-1],0);
//	work_page image_remaining = image_bytes;
//	while (image_size > read_buffer_size) {
//	  _lread(images_file,(bytes) image_remaining,read_buffer_size);
//	  image_remaining += read_buffer_size;
//	  image_size -= read_buffer_size;
//	};
//	_lread(images_file,(bytes) image_remaining,image_size);
	bits_per_pixel = 8; // for now all the built-in images are 8-bit
   return(image_bytes);
};

boolean need_to_clobber_image(int resource_index) {
	return(tt_programmer_head > 0 && // need to add a new head to programmer 
			 ((resource_index >= PERSON_WALKING_EAST1_IMAGE &&
				resource_index <= PERSON_WALKING_SOUTHEAST8_IMAGE) ||
			  (resource_index >= PERSON_WALKING_WEST1_IMAGE &&
				resource_index <= PERSON_WALKING_SOUTHWEST8_IMAGE)) &&
			 !(tt_programmer_head == 2 &&
			  // handled specially to avoid brim from being clipped
			  (resource_index == PERSON_WALKING_EAST2_IMAGE ||
				resource_index == PERSON_WALKING_EAST3_IMAGE ||
				resource_index == PERSON_WALKING_WEST2_IMAGE ||
				resource_index == PERSON_WALKING_WEST3_IMAGE)));
};

long decibal_reduction = 0; // in hundredths of a decibal
// doesn't seem to work to use this...

void compute_decibal_level() {
	// pulled out as a procedure on 060300 so set once after loading voice (if any)
#if TT_TEXT_TO_SPEECH
   if (text_to_speech()) {
		if (tt_language == BRAZILIAN_PORTUGUESE && limited_text_to_speech()) {
			// added on 060300 since at least BRMarty3 is pretty quiet
			decibal_reduction = -2000;
		} else {
			decibal_reduction = -1000;  // -10db or 1/2 the original volume
		};
   };
#endif
	if (narrating_demo()) {
      decibal_reduction = -1000;  // -10db or 1/2 the original volume
   };
};

flag reported_sound_cache_problem = FALSE; // new on 240101

sound_buffer retrieve_sound(int sound_index, DeletedMemoryPointer &temporary_sound) {
	if (sound_index > number_of_sound_files || sound_index <= 0) { // new on 171199 to be more robust
		// was < 0 prior to 290300
#if TT_DEBUG_ON
		debug_this();
#endif
		if (tt_dumping_to_test_equality_or_describe || tt_loading_to_describe) { // conditional added on 290300
			log("Something went wrong playing a sound comment.");
		} else {
			say_error("Trying to retrieve a built-in sound with an improper index.");
		};
		return(NULL);
	};
	sound_index--; // convert from 1 indexing to 0 indexing
	if (cache_entries[sound_cache_indices[sound_index]].return_cache_id() == sound_cache_ids[sound_index]) {
#if !TT_DIRECTX
		new_sound_entry(sound_cache_indices[sound_index]);
#endif
		cache_entries[sound_cache_indices[sound_index]].increment_used_count();
#if TT_DEBUG_ON
		if (tt_debug_mode == 3) {
			tt_error_file() << S(IDS_USING_CACHE) << sound_cache_indices[sound_index]
							<< S(IDS_FOR_SOUND) << sound_index
							<< " " << cache_entries[sound_cache_indices[sound_index]].return_used_count()
							<< " " << S(IDS_TIMES) << "." << endl;
		};
#endif
#if TT_DIRECTX
      sound_buffer buffer = (sound_buffer) cache_entries[sound_cache_indices[sound_index]].return_memory();
      DWORD status;
      HRESULT result = buffer->GetStatus(&status);
      if (result != DS_OK || status&DSBSTATUS_BUFFERLOST) {
         cache_entries[sound_cache_indices[sound_index]].free_entry();
         tt_error_file() << "Retrieved a sound that lost its buffer. Getting a fresh one." << endl;
         return(retrieve_sound(sound_index,temporary_sound));
      };
		if (decibal_reduction != 0) buffer->SetVolume(decibal_reduction);
      return(buffer);
#else
      return(cache_entries[sound_entry_in_use].return_memory());
#endif
	};
	millisecond start_time = timeGetTime();
   long size = sound_offsets[sound_index+1]-sound_offsets[sound_index];
#if TT_DEBUG_ON
	if (tt_debug_mode == 241299) {
		sounds_retrieved[sound_index] = size;
	};
#endif
	if (size == 0) {
		tt_error_file() << "Missing sound #" << sound_index << endl;
#if TT_DEBUG_ON
		if (tt_debug_mode == 241299) {
			sounds_retrieved[sound_index] = -1;
		};
#endif
		return(NULL);
	};
   _llseek(resolution_independent_file,sound_offsets[sound_index],0);
#if TT_DIRECTX
//   unsigned long size;
//   sound_buffer sound = load_sound_buffer(sound_index,size);
   bytes sound_bytes = new unsigned char[size];
   _hread(resolution_independent_file,(bytes) sound_bytes,size);
   sound_buffer sound = load_sound_from_memory(sound_bytes);
   delete [] sound_bytes;
   if (sound == NULL) return(NULL); // warn??
	if (decibal_reduction != 0) {
		HRESULT result = sound->SetVolume(decibal_reduction);
		if (!SUCCEEDED(result)) {
			tt_error_file() << "Failed to SetValue. Return code = " << (int) result << endl;
		};
	};
#else
	work_page sound = read_sound(resolution_independent_file,size);
#endif
//	cache_last_used[index] = tt_frame_number;
	unsigned short index;
	if (free_cache_index(size,0,index)) { 
		// second arg above was 2*size prior to 040399
		// the problem is that if the cache is full of good stuff then
		// this sound won't get in and it will not play because deleting the
		// temporary sound stops sounds
		sound_cache_indices[sound_index] = index;
		cache_entries[index].increment_cache_id();
		sound_cache_ids[sound_index] = cache_entries[index].return_cache_id();
#if TT_DIRECTX
      cache_entries[index].set_directx_type(DIRECT_SOUND_BUFFER);
      cache_entries[index].set_memory((bytes) sound);
//		sound->AddRef(); // new on 280500 -- makes sense but caller plays sound and drops it...
#else
		cache_entries[index].set_memory(sound);
#endif
		cache_entries[index].set_memory_size(size);
		cache_entries[index].set_used_count(1);
		cache_entries[index].set_savings_per_use(compute_savings_per_use(start_time));
		cache_entries[index].set_when_cached(tt_current_time);
#if !TT_32
		cache_entries[index].memory_handle = memory_handle;
#endif
#if !TT_DIRECTX
		new_sound_entry(index);
#endif
#if TT_DEBUG_ON
		if (tt_debug_mode == 3) {
			tt_error_file() << C(IDS_CACHING_SOUND) << sound_index
							<< C(IDS_USING_CACHE_NUMBER) << index
							<< C(IDS_SIZE_IS) << size << endl;
		};
#endif
	} else {
#if TT_DIRECTX
      temporary_sound = new DeletedMemorySoundBuffer(sound,NULL);
#else
		temporary_sound = new DeletedMemoryHandle(sound,
#if !TT_32
									                     size,memory_handle,
#endif
									                     NULL);
#endif
		if (!reported_sound_cache_problem) {
			log(S(IDS_NOT_ENOUGH_MEMORY_FOR_NEXT_SOUND));
			reported_sound_cache_problem = TRUE;
		};
//		return(NULL); 
	};

//	save sounds as sound files:
//	char buffer[32];
//	output_string_stream fn(buffer,32);
//	fn << "s" << (sound_index+1) << ".wav";
//	fn.put((char)0);
//	output_file_stream sound_file(buffer,std::ios_base::binary|std::ios_base::out);
//	sound_file.write((unsigned char *) sound,size);
//	sound_file.close();

	return(sound);
};

TTImage *number_pad(PlateState plate) {
   int index = 0;
	// update soon (040102) -- isn't it updated already? (140202)
	// LEFT, MIDDLE, AND RIGHT versions added on 201201
//   if (plate == NUMBER_PAD_WITH_PLATE_ANIMATED || plate == NUMBER_PAD_WITH_PLATE_ANIMATED_LEFT 
		// || plate == NUMBER_PAD_WITH_PLATE_ANIMATED_MIDDLE || plate == NUMBER_PAD_WITH_PLATE_ANIMATED_RIGHT ||
//       plate == TEXT_PAD_WITH_PLATE_ANIMATED || plate == TEXT_PAD_WITH_PLATE_ANIMATED_LEFT 
		// || plate == TEXT_PAD_WITH_PLATE_ANIMATED_MIDDLE || plate == TEXT_PAD_WITH_PLATE_ANIMATED_RIGHT) {
	if (plate >= NUMBER_PAD_WITH_PLATE_ANIMATED_LEFT && plate <= TEXT_PAD_WITH_PLATE_ANIMATED_BOTTOM) { // new on 040102
      index = (tt_current_time%180)/60; // changes every 60ms
   };
	return(image_cycles[NUMBER_PAD_SPRITE][plate].nth_image(index));
};

city_coordinate wall_lly_value = max_long; // new on 050404 -- default is off screen

city_coordinate wall_lly() {
	return(wall_lly_value);
};

void display_wall(SpriteCode code) {
   // always full-size and full against the top of the screen
	TTImage *image = image_cycles[code][0].nth_image(0);
	// updated on 140300 to work with m22 as well
	long width = 0;
	long height = 0;
	boolean ignore;
	if (tt_video_mode <= tt_graphics_video_mode_width) { // adjustments for tt_video_mode > 640 added on 110200 
		wall_lly_value = ideal_screen_height-image->current_pixel_height()*tt_screen->one_pixel_in_y_city_units();
		tt_screen->move_mark_to(0,wall_lly_value);
		image->display(width,height,ignore,::color_permutation(),256,256); 
		// on 161199 changed to 0,0 since this has been broken for quite a while
	} else {
		wall_lly_value = ideal_screen_height-image->current_pixel_height()*50;
		tt_screen->move_mark_to(0,wall_lly_value); //tt_screen->one_pixel_in_y_city_units());
		image->display(width,height,ignore,::color_permutation(),
                     (unsigned short)((256*tt_screen_width)/tt_graphics_video_mode_width), 
							// outer parens missing prior to 250402 which gave wrong result (how did it once work?)
                     (unsigned short)((256*tt_screen_height)/tt_graphics_video_mode_height)); 
	};
};

boolean display_cubby_hole(int parameter, unsigned short int x_scale, unsigned short int y_scale, bytes color_permutation, 
									long &pixel_width, long &pixel_height) {
	// either displays a 4 walled cubby or 3 walled if not left-most
	pixel_width = 0;
	pixel_height = 0; // new on 240102 so the new value is returned
	boolean ignore;
	// added ground_scale/city_scale stuff on 160804 so holes look right from helicopter
	return(image_cycles[CUBBY_SPRITE][parameter].nth_image(0)
				->display(pixel_width,pixel_height,ignore,color_permutation,
							 (unsigned short) (((int64) (ground_scale*x_scale))/tt_screen->current_scale()),y_scale));
};

void Sprite::set_clean_status(CleanStatus status, boolean recur) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		tt_error_file() << "Changing debug target's clean status to " << (int) status << endl;
	};
#endif
   if ((int) status > (int) clean_status) { // "more dirty"
	   clean_status = status;
//	  if (status != FOLLOWERS_DIRTY) {
//		  ok_to_cache_after = tt_current_time+tt_wait_before_caching;
//	  };
   };
   if (recur && leader != NULL &&
		 (completely_inside_leader() || leader->followers_absorbed())) {
	   leader->set_clean_status(FOLLOWERS_DIRTY);
   };
   //if (status != CLEAN && recent_xml_location > 0) { // new on 080703 -- removed status != FOLLOWERS_DIRTY on 110703 since need to redump then
	  // set_recent_xml_location(0);
   //};
};

void Sprite::set_clean_status_of_outside_followers(CleanStatus new_status) {
	Sprites *remaining = followers;
	while (remaining != NULL) {
		Sprite *sprite = remaining->first();
      if (!sprite->completely_inside_leader()) {
         sprite->set_clean_status(new_status,FALSE);
         sprite->set_clean_status_of_outside_followers(new_status);
      };
		remaining = remaining->rest();
	};
};

void Sprite::reset_clean_status() {
   if (this == tt_selection) {
		return; // since providing selection feedback by wiggling
	};
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		tt_error_file() << "Resetting debug target's clean status" << endl;
	};
#endif
	clean_status = CLEAN;
	Sprites *remaining = followers;
	while (remaining != NULL) {
		remaining->first()->reset_clean_status();
		remaining = remaining->rest();
	};
};

#if TT_DEBUG_ON
//string clean_status_names[6] = {"clean","clean but on top of dirty",
//										  "clean but at least one follower dirty",
//									     "moved","dirty","dirty but completely obscured"};

const_string Sprite::current_clean_status_name() {
	return(S(IDS_CLEAN_STATUS+clean_status)); //clean_status_names[clean_status]);
};

#endif

void Sprite::set_priority(long priority) {
   if (current_priority == priority) return;
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		tt_error_file() << "Changing priority of debug target." << endl;
	};
	//if (priority < -2 && priority >= min_long+2) {
	//	log("debug this");
	//};
#endif
	if (cached() && tt_screen->is_cache_valid() &&
		 tt_screen->first_cached_to_overlap_with(this) != NULL) {
		// could be more clever and check if the priority is increasing
		// or decreasing and just check for cached guy on under and over
		// respectively
#if TT_DEBUG_ON
		if (tt_debug_mode == 3) {
			tt_error_file() << type_name(this) << "#" << debug_counter << " " << C(IDS_PRIORITY_WAS)
							<< current_priority << C(IDS_AND_NOW_IS)
							<< priority	<< C(IDS_AND_WAS_CACHED) << endl;
		};
#endif
		tt_screen->invalidate_cache();
	};
	current_priority = priority;
};

boolean Sprite::ok_to_cache() {
	if (never_cache_flag || animation_in_progress() || held() || cause_of_match_failure() || to_be_smashed()) { 
		// held() added 120201 -- cause_of_match_failure added 250302 -- to_be_smashed() added on 120703
#if TT_DEBUG_ON
	if (tt_debug_mode == 3) {
		tt_error_file() << type_name(this) << "#" << debug_counter
						<< S(IDS_NOT_OK_TO_CACHE) << endl;
   };
#endif
		return(FALSE); // just to be safe
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 3) {
		tt_error_file() << type_name(this) << "#" << debug_counter;
		if (tt_cache_invalidation_cost > ((long) display_cost)*max(clean_run_average,clean_run)) {
			tt_error_file() << " not";
		};
		tt_error_file() << C(IDS_OK_TO_CACHE) << endl
						<< C(IDS_INVALIDATION_COST) << tt_cache_invalidation_cost
						<< C(IDS_CLEAN_RUN_AVERAGE)
						<< (int) max(clean_run_average,clean_run)
						<< C(IDS_DISPLAY_COST) << (int) display_cost << endl;
	};
#endif
	return(tt_cache_invalidation_cost <= ((long) display_cost)*max(clean_run_average,clean_run));
};

//boolean Sprite::ok_to_cache() {
//	if (ok_to_cache_when_zero == 0) return(TRUE);
//	ok_to_cache_when_zero--;
//	return(FALSE);
//};

boolean Sprite::colliding(Sprite *another) {
   TTRegion my_region, other_region;
	collision_region(my_region);
	another->collision_region(other_region);
	// maybe overlaps, etc. should take &region rather than *region??	
	return(my_region.overlaps(&other_region));
};

boolean Sprite::overlaps(TTRegion *region) {
   TTRegion my_region;
	active_region(my_region);
	return(my_region.overlaps(region));
};

boolean Sprite::contained_in(SpriteType type) {
	if (leader != NULL) {
		if (leader->kind_of() == type) {
			return(TRUE);
		} else {
			return(leader->contained_in(type));
		};
	} else {
		return(FALSE);
	};
};

boolean Sprite::contained_in_robots_thought_bubble() { // new on 131003
	// contained_in(THOUGHT_BUBBLE) isn't good enough since while starting training can be part of a thought bubble
	if (leader != NULL) {
		if (leader->kind_of() == THOUGHT_BUBBLE) {
			return(leader->pointer_to_leader() != NULL); // assumes that thought bubbles can only be a part of Robots
		} else {
			return(leader->contained_in_robots_thought_bubble());
		};
	} else {
		return(FALSE);
	};
};

boolean Sprite::contained_in_thought_bubbles_cubby() { // new on 190205
	// contained_in(THOUGHT_BUBBLE) isn't good enough when creating a Java applet
	if (leader != NULL) {
		if (leader->kind_of() == THOUGHT_BUBBLE) {
			return(((Thought_Bubble *) leader)->pointer_to_cubby() == this);
		} else {
			return(leader->contained_in_thought_bubbles_cubby());
		};
	} else {
		return(FALSE);
	};
};

boolean Sprite::part_of_top_level_copy() {
	if (tt_top_copy == tt_global_picture) return(TRUE); // used to explictly control scope of top level
	if (tt_top_copy != NULL) {
		if (!tt_top_copy->is_picture() && (tt_top_copy->kind_of() == INTEGER || tt_top_copy->kind_of() == TEXT)) { // new on 210301 as an optimization
			return(FALSE); // since they have no parts
		};
		return(part_of(tt_top_copy));
	};
	return(FALSE);
};

boolean Sprite::part_of(Sprite *other) {
	// re-ordered conditional on 111100 since while moving up by "leader" rather than "picture" is slower it won't miss anything
	if (leader != NULL) {
		if (leader == other) {
			return(TRUE);
		} else {
			if (leader->part_of(other)) {
				return(TRUE);
			}; // new on 220203 that otherwise fall thru to look via picture
		};
	};
	Sprite *picture;
	if (floor != NULL && (picture = floor->pointer_to_picture()) != NULL) {
		if (picture == other) return(TRUE);
		return(picture->part_of(other));
	} else {
		return(FALSE);
	};
};

//void Sprite::recursively_save_size() {
//	save_size();
//	Sprites *remaining = followers;
//	while (remaining != NULL) {
//		remaining->first()->recursively_save_size();
//		remaining = remaining->rest();
//	};
//};

//void Sprite::save_size() {
//	if (good_width == 0) { // don't save twice
//		good_width = width;
//		good_height = height;
//	 };
//};

void Sprite::good_size(city_coordinate &w, city_coordinate &h, Sprite *source, boolean ) {
   // by default a good size has no scaling -- faster
	w = max_widths[code];
	h = max_heights[code];
	adjust_good_size(w,h,source,tt_good_sizes_adjust_to_screen_size);
};

void Sprite::adjust_good_size(city_coordinate &w, city_coordinate &h, Sprite *source, boolean adjust_to_screen_size) { // abstracted on 170603
#if TT_POST321
	if (adjust_to_screen_size) {
		w = shrink_width_from_640x480_screen_size(w);
		h = shrink_height_from_640x480_screen_size(h);
	};
	// re-orderded on 311003 so can return below since adjust_size_for_source does bad things if standing
	if (floor != NULL) {
		if (floor->in_the_room_with_programmer()) { // updated 020205
			tt_programmer->translate_floor_to_room_dimensions(w,h);
			return; // new on 311003
		};
	} else if (tt_programmer != NULL && tt_programmer->kind_of() == PROGRAMMER_ROOM_WALKING) {
		// tried != PROGRAMMER_AT_FLOOR on 040404 but that did the wrong thing while in the helicopter (starting fresh)
		// new on 311003 to deal with objects dereferenced by robot that don't have a floor 
		tt_programmer->translate_floor_to_room_dimensions(w,h);
		return; // new on 311003
	};
#else
	if (floor != NULL && floor->in_the_room_with_programmer()) { // updated 020205 
		tt_programmer->translate_floor_to_room_dimensions(w,h);
	};
	if (adjust_to_screen_size) {
		w = shrink_width_from_640x480_screen_size(w);
		h = shrink_height_from_640x480_screen_size(h);
	};
#endif
	if (source != NULL) {
		adjust_size_for_source(w,h,source);
	};
};

void Sprite::current_percent_of_good_size(long &x_percent, long &y_percent) {
   city_coordinate good_width, good_height;
   good_size(good_width,good_height);
	// dealing with overflow here on 150202 by using int64
	x_percent = (long) ((((int64) width)*100)/good_width);
   y_percent = (long) ((((int64) height)*100)/good_height);
};

void Sprite::adjust_size_for_source(city_coordinate &good_width, city_coordinate &good_height, Sprite *source) {
   if (source != NULL && source != this &&
		 !source->inside_vacuum()) { // this condition added 110200 so that if Tooly is in a vacuum every doesn't become 0 wide
		// added source != this on 040100 so Tooly for example can be restored to a good size by Pumpy
      // good size is modified by size of ultimate container
      // e.g. notebook or toolbox
      long x_percent, y_percent;
      Sprite *container = source->ultimate_leader();
      if (container->kind_of() == ROBOT || container->kind_of() == ROBOT_IN_TRAINING) {
         Sprite *tool = ((Robot *) container)->pointer_to_tool();
         if (tool != NULL) container = tool;
      };
      container->current_percent_of_good_size(x_percent,y_percent);
		if (good_width > 1000000) { // new on 150202 to have both versions to deal with both round off and overflow
			 good_width = (good_width/100)*x_percent;
			 good_height = (good_height/100)*y_percent;
		} else {
			// re-written on 220102 to avoid round off errors
			good_width = (good_width*x_percent)/100;
			good_height = (good_height*y_percent)/100;
		};
   };
};

void Sprite::set_to_good_size(Sprite *source) {
   city_coordinate good_width,good_height;
   good_size(good_width,good_height,source);
//   adjust_size_for_source(good_width,good_height,source);
   set_size(good_width,good_height,SETTING_TO_GOOD_SIZE);
};

void Sprite::animate_to_good_size(millisecond duration, Sprite *source) {
	city_coordinate good_width, good_height;
	good_size(good_width,good_height,source);
//   adjust_size_for_source(good_width,good_height,source);
//#if TT_BETA
//   if (tt_running_old_demos) {
//      animate_size(good_width,good_height,duration);
//      return;
//   };
//#endif
	animate_size(good_width,good_height,duration);
   // commented out since this can be called while flipped - but I don't think it makes sense anymore - 211002
	//set_saved_width(good_width); // new on 050601 so if use Pumpy with G and then put on clipboard it has the right size
	//set_saved_height(good_height);
	do_after(duration,this,SIGNAL_GOOD_SIZE_CALLBACK); // new on 080702
};

//void Sprite::good_size(city_coordinate &w, city_coordinate &h) {
//	if (good_width == 0 && tt_user_is_debugger) {
//		say("ToonTalk is confused and doesn't know what size something should be.");
//		tt_error_file() << "Warning retreiving saved size without saving first." << endl;
//		w = width;
//		h = height;
//	} else {
//		w = good_width;
//		h = good_height;
		// saved size used so reset for subsequent saving
//		good_width = 0;
//		good_height = 0;
//	};
//};

//void Sprite::recursively_remove_leaders(Background *) {
	// Background obsolete -- remove soon
//	if (followers != NULL && floor != NULL) {// && !leaders_removed()) {
//    leader may have other followers
//		floor->remove_leader(this);
//		Sprites *remaining = followers;
//		while (remaining != NULL) {
//			remaining->first()->recursively_remove_leaders(NULL);
//			remaining = remaining->rest();
//		};
//	};
////	set_leaders_removed(TRUE);
//};

//void Sprite::recursively_restore_leaders(Background *background) {
////	set_leaders_removed(FALSE);
//	if (followers != NULL) {
//		Sprites *remaining = followers;
//		while (remaining != NULL) {
//			remaining->first()->recursively_restore_leaders(background);
//			remaining = remaining->rest();
//		};
//		background->add_leader(this);
//	};
//};

Sprite *Sprite::select(TTRegion *region, SelectionReason reason, Sprite *in_hand) {
   if (leader != NULL && leader->kind_of() == ROBOT) {
		// can't grab things that a robot is holding
		// is this already handled by call to this in programmer?? -- not since can be a robot running on the floor
		Robot *robot = (Robot *) leader;
		if (robot->pointer_to_tool() == this) { // if I'm held - conditional new on 031202
			return(leader->select(region,reason,in_hand));
		};
	};
	if (reason == MIGHT_DROP_ITEM) {
		return(NULL); // all the cases where this is meaningful already handled
	} else {
		return(this); // by default selectable
	};
};

//#if TT_DEBUG_ON
//int repeat = 1000;
//#endif

Sprite *Sprite::top_level_copy(boolean also_copy_offsets, Sprite *original, Sprite *originals_copy) {
//#if TT_DEBUG_ON
//	for (int i = 0; i < repeat; i++) {
//		xml_document *document = create_xml_document();
//		xml_element *toontalk_object = create_xml_element(L"ToonTalkObject",document);
//		top_level_xml(document,toontalk_object);
//		xml_append_child(toontalk_object,document);
//		//xml_element *element = create_xml_element(L"ToonTalkObject",document);
////		xml_append_child(create_xml_element(L"ToonTalkObject",document),document); // experiment 190804
////		Sprite *copy = xml_get_sprite(document);
//		xml_release_document(document);
//		//int x = element->Release();
//		//while (x > 0) {
//		//	x = element->Release();
//		//};
//		//x = toontalk_object->Release();
//		//while (x > 0) {
//		//	x = element->Release();
//		//};
//		//copy->set_infinite_stack(FALSE); // since this may be a stack
//		//copy->set_size_and_location_to_those_of(this); // since saved without geometry
//		//copy->destroy();
//	};
//#endif
//#if TT_DEBUG_ON
//   millisecond start = timeGetTime();
//	const int repeats = 10;
//for (int i = 0; i < repeats; i++) {
	// first arg added on 100203
   // this replicates the same bird/nest relationship in copy as in original
   // and as of 11/17/97 also the remote control/controlee relationship
	Sprite *the_copy = already_copied(this);
	if (the_copy != NULL) { // new on 210203 to prevent infinite recursions
		return(the_copy);
	};
//#if TT_DEBUG_ON
//   millisecond start_time = timeGetTime();
//#endif
	Sprites *saved_copies = save_copied_items();
	{ // to scope tt_top_copy properly -- new on 151204
		UnwindProtect<Sprite *> set(tt_top_copy,this); // new on 191199
		UnwindProtect<Sprites *> set2(tt_nests_to_set_original_nest,NULL); // new on 180804
		if (original != NULL) { // new on 210203 so that can prevent infinite recursions
			just_copied(original,originals_copy);
		};
		the_copy = copy(also_copy_offsets);
		release_items_copied();
		restore_copied_items(saved_copies);
		if (tt_nests_to_set_original_nest != NULL) {
			process_tt_nests_to_set_original_nest();
		};
	};
	if (tt_top_copy == NULL) { // truely top level
		just_destroy_list(tt_previous_controlee_replacements); // new on 151204
	};
	return(the_copy);
};
//	the_copy->destroy();
//};
//tt_error_file() << "Copy old way took " << (timeGetTime()-start) << "ms." << endl;
//start = timeGetTime();
//for (int i = 0; i < repeats; i++) {
//	xml_document *document = create_xml_document();
//	xml_append_child(top_level_xml(document,create_xml_element(L"ToonTalkObject",document)),document);
//	Sprite *copy = xml_get_sprite(document);
//	xml_release_document(document);
//	copy->set_infinite_stack(FALSE); // since this may be a stack
//	copy->set_size_and_location_to_those_of(this); // since saved without geometry
//	copy->destroy();
//};
//tt_error_file() << "Copy new way took " << (timeGetTime()-start) << "ms." << endl;
////#endif
////#if TT_DEBUG_ON
////   if (tt_debug_mode == 300702) {
////      tt_error_file() << "Took " << (timeGetTime()-start_time) << "ms to copy ";
////      print(tt_error_file());
////      tt_error_file()  << endl;
////   };
////#endif
//	// experiment 160804
//	xml_document *document = create_xml_document();
//	xml_append_child(top_level_xml(document,create_xml_element(L"ToonTalkObject",document)),document);
//	Sprite *copy = xml_get_sprite(document);
//	xml_release_document(document);
//	copy->set_infinite_stack(FALSE); // since this may be a stack
//	copy->set_size_and_location_to_those_of(this); // since saved without geometry
//	return(copy);
//};


Sprite *Sprite::top_of_stack_or_create(Sprite *by, SelectionReason reason) {
	// new on 260500 - returns top of stack or creates it
	// args are probably a mistake but keep for now if want to get robots to record this correctly
	if (top_of_stack == NULL) {
		top_of_stack = top_level_copy();
		top_of_stack->set_is_top_of_a_stack(TRUE);
		top_of_stack->set_selectable(FALSE); // can't suck it up etc without treating its leader like a stack
//		if (floor != NULL) {
//			floor->add_item(top_of_stack);
//		};
		add_follower(top_of_stack);
#if TT_DEBUG_ON
		if (tt_debug_mode == 3) {
			tt_error_file() << "Creating top of a stack ";
			print(tt_error_file());
			tt_error_file() << endl;
		};
#endif
	};
	set_clean_status(DIRTY);
	return(top_of_stack);
};

Sprite *Sprite::remove_top_of_stack() { // if it exists
	if (top_of_stack == NULL) return(NULL);
	Sprite *top = top_of_stack;
	top->set_is_top_of_a_stack(FALSE);
	top->set_selectable(TRUE);
	remove_follower(top);
	top_of_stack = NULL;
	set_clean_status(DIRTY);
//	remove_from_floor(FALSE);
	return(top);
};

Sprite *Sprite::copy_top_of_stack(Sprite *by, SelectionReason reason) {
	Sprite *result = Sprite::selected(reason,by);
	const millisecond duration = 750;
	city_coordinate good_width, good_height;
	good_size(good_width,good_height);
	result->animate_location(llx-(good_width-width)/2,
									 lly-(good_height-height)/2,
									 duration);
//	record_event(RELEASE_ITEM_ON,by,result->pointer_to_background(),this);
	return(result);
};

Sprite *Sprite::released(Sprite *by, boolean top_level, SpecialDrop special_drop) { // ActionStatus *status, 
//	if (status != NULL) *status -= 1;
   if (top_level) ::completed(by); 
	// rewritten on 171102 -- condition added 271102 since cubby calls this recursively (and this breaks add_completion) calls
	return(this);
};

boolean Sprite::receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item) {
// by default just done already
//		 completed(action_status);
   ::completed(by); // rewritten on 171102
//	if (action_status != NULL) *action_status += 1;
	return(TRUE);
};

void Sprite::action_completed() { // new on 171102
#if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {
      log("action_completed of debug target.");
   };
#endif
   if (saved_by != NULL) {
      saved_by->completed();
		saved_by->destroy(); // new on 150103 since otherwise is a leak of robots
      saved_by = NULL; // always OK?? -- should be if finished with action
   };
};

Sprite *Sprite::selected(SelectionReason reason, Sprite *by) {
	if (to_be_smashed_flag || slated_for_deletion_flag) return(NULL);
	if (reason == VACUUM_USED_TO_BLANK && !blankable()) return(NULL);
	finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off  // good policy??
	Background *background_of_by;
	boolean by_running = (by != NULL && by->still_running());
	if (by != NULL) { // prior to 250102 this was by_running but if training then should use the background of the robot
		// this should only differ if something was selected via remote looks or the like
		background_of_by = by->pointer_to_background();
	} else {
		background_of_by = ultimate_leader()->pointer_to_background(); 
		// was floor prior to 121199 but might be NULL -- really need ultimate_leader here??
	};
   if (reason == VACUUM_USED_TO_SPIT) {
//      if (tt_log_version_number > 8)
      recursively_activate_pictures();
      return(this);
   };
	if (reason == COPIER_USED || !infinite_stack()) {
//		 || (reason == VACUUM_USED && leader != NULL && leader->kind_of() == PROGRAM_PAD)) {
		if (reason == GRASPED && !by_running) { // if running this will be recorded elsewhere
			record_event(GRASP_ITEM,by,background_of_by,this);
		};
		if (reason == GRASPED) {
			if (leader != NULL) { // contained in something so let it know
			   // vacuum handles this now || reason == VACUUM_USED)) {
				// note that VACUUM_USED_TO_BLANK does not do this
				leader->remove_selected_element(this,reason,by,TRUE);
			} else if (pointer_to_background() != NULL && pointer_to_background()->pointer_to_picture() != NULL) { 
				// following copied here from Programmer_At_Floor::use_tool
				// right modularity??
				remove_from_floor(); 
				un_absorb_followers(); // new on 050203 since otherwise can't select box in a robot's thought bubble
//				set_background(floor); // so tool knows for recording etc.
			};
		};
		return(this);
	};
	if (reason == VACUUM_USED && infinite_stack() && leader != NULL && leader->kind_of() == PROGRAM_PAD) {
		set_infinite_stack(FALSE); // vacuum up stack as ordinary item
//		set_in_notebook((Notebook *) leader,TRUE); // new on 160704 since may be shared with a copy of this notebook		
		if (return_ref_count() <= 1 || (by != NULL && by->kind_of() == ROBOT_IN_TRAINING)) { 
			// was return_ref_count() == 1 prior to 110205 but that interfered with training a robot to vacuum off a page from a notebook
			return(this);
		} else {
			Notebook *notebook = (Notebook *) leader;
			Sprite *page = notebook->copy_page(this,reason,by,background_of_by);
			leader->remove_selected_element(this,reason,by,FALSE);
			// new on 160704 since shared don't clobber the shared object
			return(page);
		};
	};
	if (leader != NULL && leader->kind_of() == PROGRAM_PAD) {
		Notebook *notebook = (Notebook *) leader;
      Sprite *page = notebook->copy_page(this,reason,by,background_of_by);
         // commented out on 150702 since this is now dealt with by activate_callback
//         if (!tt_stop_all) {  // tt_log_version_number > 9 && 
////            page->recursively_activate_pictures();
//            // if it happens immediately other things like hatching birds, building nests, growing to good size haven't happened yet
//            // I'm just guessing that 3 seconds is long enough
//            if (page->active() || tt_log_version_number <= 21) { // does this really make sense at all??
//					// conditional added on 210300
//					page->do_after(3000,page,recursively_activate_pictures_callback);
//				};
//         };
		return(page);
	};
	Sprite *the_copy = remove_top_of_stack(); // new on 260500
	if (the_copy == NULL) {
		the_copy = top_level_copy();
	};
   the_copy->set_inside_vacuum(FALSE,TRUE); // new on 221002 since robot can be running while Tooly is in a vacuum
	boolean record_select_and_grasp = (reason != ITEM_DROPPED && by != NULL && by->kind_of() != PROGRAMMER);
	if (record_select_and_grasp) {
		// only if reason is a grasp??
		record_event(SELECT_STACK_ITEM,by,background_of_by,this,TRUE);
	};
	// following is just to create an index
	record_event(NEW_ITEM,by,background_of_by,the_copy,TRUE,0,ADD_TO_WORKING_SET_REGARDLESS);
	if (record_select_and_grasp && reason == GRASPED && !by_running) {
		record_event(GRASP_ITEM,by,background_of_by,the_copy,FALSE,0,DONT_ADD_TO_WORKING_SET);
	};
	if (background_of_by != NULL) {
      background_of_by->add_item(the_copy);
		if ((reason == GRASPED || reason == VACUUM_USED_TO_BLANK) // VACUUM_USED_TO_BLANK new on 270804
			 && background_of_by->showing_on_a_screen()) { // showing test added on 090202
			if (floor == NULL && background_of_by->in_the_room() && tt_log_version_number >= 40) {
				// new on 040404 since can be standing up while robot copies something inside Tooly...
				the_copy->animate_to_good_size(background_of_by->default_duration(),tt_log_version_number >= 64?tt_toolbox:NULL); 
				// second arg new 141204
			} else {
				the_copy->animate_to_good_size(background_of_by->default_duration(),this);
			};
		};
   };
	return(the_copy);
};

//void Sprite::record_grasp_or_vacuum(SelectionReason reason, Sprite *by) {
//	if (reason == GRASPED) {
//		record_event(GRASP_ITEM,by,floor,this);
//	} else if (reason == VACUUM_USED) {
//		record_event(VACUUM_APPLY,by,floor,this);
//	};
//};

boolean Sprite::inside_a_house() {
	return(floor != NULL && floor->inside_a_house());
};

ReleaseStatus Sprite::add_to_side(boolean right_side, Sprite *other, Sprite *by, millisecond duration, Sprite *original_recipient, 
											 Sprite *original_item) {
//	completed(status); // restored on 260201 since the following was wrong (also this was in sprite.h)
//		 if (status != NULL) *status -= 1;
//	return(RELEASE_IGNORED);
	// later on 260201 decided that for all types should default to an ordinary drop
	if (receive_item(other,by,duration,original_recipient,original_item)) {
		return(RELEASE_HANDLED);
	} else {
		return(RELEASE_IGNORED);
	};
};

boolean Sprite::vacuum_if_left_on_floor(SpritePointer *, int ) {
	return(leader == NULL &&
			 !inside_vacuum() &&
			 !slated_for_deletion() &&
			 available_for_indexing() &&
			 !animation_in_progress() &&
			 floor != NULL); // added on 041202 since is already removed (e.g. smashed in a type coercion)
};

Block *Sprite::block_im_on() {
	Background *background = ultimate_container()->pointer_to_background();  
	if (background == NULL) return(NULL);
	return(background->block_im_on());
};

Sprite *Sprite::copy(boolean also_copy_offsets) {
//	city_coordinate center_x, center_y;
//   center_location(center_x, center_y);
	Sprite *copy = new Sprite(code,
									  llx,lly, //center_x,center_y, 
//									  NULL, // floor,
									  sprite_type,
									  parameter,
									  current_scale,
									  width,height);
   finish_making_copy(copy,also_copy_offsets);
	return(copy);
};

void Sprite::finish_making_copy(Sprite *copy, boolean also_copy_offsets, boolean copy_followers_too) { // abstracted  on 160702
	// added copy_followers_too on 300103 since some callers don't want to (e.g. Notebooks)
   copy->set_priority_fixed(priority_fixed_flag); // moved from Sprite::copy(boolean also_copy_offsets)
	if (comment != NULL) { // moved from Sprite::copy(boolean also_copy_offsets)
		copy->set_comment(comment->copy());
	};
   if (is_blank() && !tt_copy_restores_pre_blank) { // moved from Picture::copy(boolean also_copy_offsets)
		copy->become_blank();
	};
   copy->set_show_all(show_all()); // moved from Picture::copy(boolean also_copy_offsets)
	copy->set_visible(visible()); // moved from Picture::copy(boolean also_copy_offsets)
//   copy->set_in_notebook(in_notebook(),FALSE); // new on 160702 -- commented out on 210802 since notebooks take care of this now
//   copy->set_inside_vacuum(inside_vacuum(),FALSE); // new on 160702 -- removed on 131202 -- didn't make sense - thanks Leonel
   copy->set_active(active(),FALSE); // moved from Picture::copy(boolean also_copy_offsets) on 170702
#if TT_XML
   if (extra_xml != NULL) {
      copy->set_extra_xml(extra_xml,extra_xml_count);
   };
#endif
	if (copy_followers_too) {
		if (also_copy_offsets) { // new on 260103
			copy->copy_leader_offsets(this);
		};
		if (followers != NULL && copy->pointer_to_followers() == NULL) { // updated on 300103
			copy->set_followers_copy(followers);
		};
	};
};

void Sprite::set_followers_copy(Sprites *sprites) {
	Sprites *remaining = sprites;
	followers = new Sprites();
	Sprites *last_followers_cons = followers;
	while (remaining != NULL) {
		Sprite *sprite = remaining->first();
		Sprite *sprite_copy = sprite->copy(TRUE);
//		sprite_copy->copy_leader_offsets(sprite);
		sprite_copy->set_leader(this);
		last_followers_cons->set_first(sprite_copy);
		remaining = remaining->rest();
		if (remaining != NULL) {
			Sprites *new_last_followers_cons = new Sprites();
			last_followers_cons->set_rest(new_last_followers_cons);
			last_followers_cons = new_last_followers_cons;
		};
	};
};

void Sprite::remove_selected_element(Sprite *element, SelectionReason , Sprite *, boolean add_to_floor) {
 	remove_follower(element);
	if (!add_to_floor) return;
	if (floor != NULL) {
		// commented out on 070105 since sprite::remove_follower now deals with this
//      element->remove_from_floor(FALSE,FALSE); // new on 141002 since might be on back of a picture and grasped
		floor->add_item(element);
#if TT_DEBUG_ON
	} else {
		log(_T("Warning: removing a selected item from something that doesn't know where it is."));
#endif
	};
};

// obsolete
//void Sprite::delete_followers() {
	// this is complex since delete will otherwise call remove_follower
	// which clobbers the list of followers
//	if (followers == NULL) return;
//	if (!tt_shutting_down && floor != NULL) {// && !leaders_removed()) {
//		floor->remove_leader(this);
//	};
//	delete followers;
//};

Sprite::~Sprite() {
#if TT_DEBUG_ON
	tt_deleted_sprite_count++;
	if (tt_debug_mode == 100) {
		tt_error_file() << type_name(this) << " #" << debug_counter << "; deleted=" << (int) deleted() << endl;
	};
	if (deleted()) {
		say_error(IDS_TOONTALK_BROKEN);
		return;
	};
   if (followers != NULL) {
		debug_this();
      tt_error_file() << type_name(this) << "#" << debug_counter << " deleted but still has parts." << endl;
   };
	debug_log(debug_counter,"Deleting debug target.");
#endif
#if TT_DEBUG_ON||TT_CAREFUL
	deleted_flag = TRUE;
#endif
	if (!prepared_for_deletion_flag) {  // shouldn't happen -- right?
#if TT_DEBUG_ON
      tt_error_file() << "Deleting " << type_name(this) << "#" << debug_counter << " which wasn't prepared." << endl;
#endif
		prepare_for_deletion();  // this doesn't seem to dispatch as virtual function? -- still? (asked 060204)
	};
	if (leader_offsets != NULL) {
		delete leader_offsets;
		leader_offsets = NULL;
	};
	if (animations != NULL) {
		delete animations;
	};
};

boolean Sprite::destroy(Sprite *by, boolean save_in_vacuum) {
	// changed on 090200 so returns TRUE if really about to be destroyed
   // "by" is only used for debugging now - simplify? (asked on 171102)
#if TT_DEBUG_ON
   if (tt_debug_mode == 1717) {
      tt_error_file() << "Considering deleting " << type_name(this) << "#" << debug_counter
							 << " whose ref count is " << ref_count << " ";
		if (by != NULL) {
			by->print(tt_error_file());
		};
		tt_error_file() << endl;
   };
	if (debug_counter == tt_debug_target) {
		log(_T("Destroy() called on debug target."));
		tt_error_file() << "When ref count was " << ref_count << endl;
	};
#endif
	// added second condition on 280300 since sometimes it is good to prepare for deletion while some (unimportant) references are still out there
	if (slated_for_deletion() && ref_count == 0) { // do this check before decrementing ref_count etc.
		// prior to 140103 was slated_for_deletion() was prepared_for_deletion()
		// but now can catch complex cycles like remote control notebooks left on the thought bubble while training
#if TT_DEBUG_ON
		if (prepared_for_deletion() && !tt_resetting_or_destroying_city) {
 			say_error(IDS_DESTROYED_TWICE);
			tt_error_file() << type_name(this) << "#" << debug_counter << endl;
			// no harm done
		};
#endif
		return(TRUE); // will be??
	};
	if (!tt_resetting_or_destroying_city && !ok_to_delete()) { 
		// experimented on 190103 with using ref counts on shut down but cycles stop recovery and can't analyze leaks
		// first condition was !tt_shutting_down prior to 201200 but that isn't always set yet when running win_main_finalize
		// if about to quit then don't need to bother with ref counts
		// not to be deleted but might have been in vacuum
//		set_inside_vacuum(FALSE); // does this really make sense? - commented out on 201199 since it is wrong if ref count update
		// is reason for this
		return(FALSE);
	};
#if TT_CAREFUL
	if (current_held_status() == HELD_BY_PROGRAMMER && !tt_resetting_or_destroying_city) {
		say_error(_T("ToonTalk mixed up. It is trying to destroy something still in your hand."));
		return(FALSE);
	};
#endif
	if (save_in_vacuum && tt_toolbox != NULL) {
		if (prepared_for_deletion()) {
			return(TRUE);
		};
		// ok for now but really should also do this when ROBOT_IN_TRAINING
		// but need to upgrade event.cpp then as well
		tt_toolbox_vacuum->add_deletee(this);
	} else {
		if (!prepared_for_deletion()) { // && !slated_for_deletion()) { // second conjunct added on 280201
			set_slated_for_deletion(TRUE);
			if (by != NULL && by->kind_of() == ROBOT_IN_TRAINING) {
				// new on 011105 since Ian had a crash exiting the thought bubble
				// where something had been trashed while in working set
				((Robot *) by)->just_remove_tool_from_working_set(this);
			};
			prepare_for_deletion(); // removed by on 060204
			ref_count = 0; // new on 020301 since this was not just prepared_for_deletion but actually deleted 
			// this deals with ref_count == 0 check above
			if (prepared_for_deletion()) { // otherwise e.g. networked nests saved for time travel
//#if TT_DEBUG_ON
//				if (tt_deleted_sprites != NULL) {
//					Sprites *already_there = tt_deleted_sprites->member_cons(this);
//					if (already_there != NULL) {
//						debug_this();
//					};
//					if (already_there == NULL && tt_next_frames_deleted_sprites != NULL) {
//						already_there = tt_next_frames_deleted_sprites->member_cons(this);
//					};
//					if (already_there != NULL) {
//						debug_this();
//					};
//				};
//#endif
				tt_deleted_sprites = new Sprites(this,tt_deleted_sprites);
			};
			// rewrote below as above on 120104 since is more efficient and robust (somehow tt_deleted_sprites contained a bad cons)
			// note that the problem of duplicates shouldn't arise since check if prepared_for_deletion twice here
			//if (tt_resetting_or_destroying_city) {
			//	if (prepared_for_deletion()) { // e.g. networked nests saved for time travel
			//		// could this really just delete this here? (or at the end of this method) asked 180703 and again on 130803
			//		insert_at_end_if_not_member(this,tt_deleted_sprites); // new on 190103 to avoid duplicates
			//	};
			//} else {
			//	tt_deleted_sprites = new Sprites(this,tt_deleted_sprites);
			//};
//		} else if (tt_reclaiming_memory) { 
			// prior to 180103 was !about_to_quit()
			// was tt_shutting_down prior to 211200 // only if not shutting down delete already prepared sprites (new on 010400)
		// commented out the following on 280300 since working set elements now use ref counts
//		if (by != NULL && (by->kind_of() == ROBOT || by->kind_of() == ROBOT_IN_TRAINING)) {
//			((Robot *) by)->delete_when_deleted(this);
//		} else {
// if already prepared for deletion shouldn't get here
//#if TT_DEBUG_ON
//			if (tt_debug_mode == 1066) {
//				if ((tt_deleted_sprites != NULL &&
//					  tt_deleted_sprites->member_cons(this)) ||
//					 (tt_next_frames_deleted_sprites != NULL &&
//					  tt_next_frames_deleted_sprites->member_cons(this))) {
//					say("ToonTalk confused and is attempting to destroy the same thing twice!",4);
//					return;
//				};
//			};
//#endif
//			tt_deleted_sprites = new Sprites(this,tt_deleted_sprites); // commenting this out is experiment on 190103
		};
	};
#if TT_DEBUG_ON
	if (floor != NULL && !tt_resetting_or_destroying_city && floor != tt_deleting_background && 
		 !floor->prepared_for_deletion && // was tt_shutting_down prior to 211200
		 floor->has_item(this)) {
		say_error(IDS_TOONTALK_DESTROY_ON_FLOOR);
//		remove_from_floor();
	};
#endif
	no_background();
	return(TRUE);
};

void Sprite::prepare_for_deletion() {
  // "by" is only used for debugging now - simplify? (asked on 171102)
	// by replaced by preparing_for_deletion on 060204
	// the idea is to deal with recursive calls by setting prepared_for_deletion when starting but then need to ignore it here
  if (prepared_for_deletion()) return; // already done
  preparing_for_deletion = TRUE;
  prepared_for_deletion_flag = TRUE;
#if TT_DEBUG_ON
  if (tt_debug_mode == 1717 || tt_debug_target == debug_counter) {  
     tt_error_file() << "Preparing for deletion " << type_name(this) << "#" << debug_counter << " ";
     if (!tt_shutting_down) describe_type(tt_error_file());
     tt_error_file() << " on frame " << tt_frame_number << endl;
	  debug_log(debug_counter);
  };
#endif
  if (!tt_resetting_or_destroying_city) {
//    finish_instantly(); -- bad idea and shouldn't be necessary since not driven by background since off it
		unselect();
		if (blank && tt_toolbox != NULL && // might be before its been made
          tt_toolbox_vacuum != NULL) {
         // what if a vacuum clone did it??
			tt_toolbox_vacuum->remove_blanked_item(this);
		};
//		if (leader != NULL) {
			// might happen if a list of items is deleted and
			// a follower comes first
//			leader->remove_follower(this);
//		};
      tt_screen->remove(this); // just in case
  };
  release_all_references(); // new on 020504 -- many subclasses called this and now it calls remove_all_followers(TRUE)
  // following used to not be called when tt_shutting_down -- but that's a bad leak
  // following taken care of now (020504) by release_all_references
//  if (followers != NULL) { // why is this different from :remove_all_followers called with destroy_too flag?
//		Sprites *remaining = followers;
//		UnwindProtect<boolean> set(tt_dealing_with_follower_change,TRUE); // remove_follower below shouldn't trigger any remotes
//		while (remaining != NULL) {
//			Sprites *next = remaining->rest(); // save now in case clobbered
//			Sprite *sprite = remaining->first();
////			boolean delete_too = sprite->completely_inside_leader();
//			if (sprite != NULL) {
//            remove_follower(sprite);
//            if (!sprite->completely_inside_leader()) { // && sprite->showing_on_a_screen()) { // new policy 5/7/97
//               tt_screen->remove(sprite);
//            };
//				if (!sprite->destroy()) { 
//					// new on 090200 so removed from floor if part has high ref count and isn't destroyed
//					sprite->remove_from_floor(FALSE,FALSE);
//			   };
//         };
//			remaining = next;
//		};
//#if TT_DEBUG_ON
//		if (followers != NULL && !about_to_quit()) {
//			log("followers not null after removing them all");
//		};
//#endif
//   };
#if TT_XML
  if (extra_xml != NULL) {
     for (int i = 0; i < extra_xml_count; i++) {
        extra_xml[i]->Release();
     };
     delete [] extra_xml;
  };
#endif
// if the system was very well debugged then don't need following
// but until then makes it much more robust at a small cost
// also picture->remove_flipped_follower is needed
#if TT_CAREFUL
  if (!tt_resetting_or_destroying_city) { // replaced tt_shutting_down on 211200
		if (floor != NULL && floor != tt_deleting_background
#if TT_DEBUG_ON
			 && !floor->prepared_for_deletion // typically in progress
#endif
			) {
         Picture *picture = floor->pointer_to_picture();
         if (picture != NULL && leader != NULL) {
            picture->remove_flipped_follower(this);
         };
			if (floor != NULL) { // new on 211200 since the above may have reset the floor
				if (floor->known_leader(this)) {
					say_error(IDS_TOONTALK_USED_AFTER_DESTROYING);
					floor->remove_leader(this);
				};
				if (floor->has_item(this)) {
					say_error(IDS_TOONTALK_DESTROY_ON_FLOOR);
					floor->remove_item(this);
				};
			};
		};
		if (leader != NULL) {
			Sprites *leaders_followers = leader->pointer_to_followers();
			if (leaders_followers != NULL && leaders_followers->member_cons(this)) {
				say_error(IDS_TOONTALK_DESTROY_PART_OF_SOMETHING);
				leader->remove_follower(this);
			};
		};
   };
#endif
   stop_animations();
//	if (top_of_stack != NULL) {
//		top_of_stack->destroy();
//		top_of_stack = NULL;
//	};
	if (saved_by != NULL && tt_reclaiming_memory) { // was !about_to_quit() prior to 180103 // moved here on 191102
		saved_by->destroy(); // if zero ref count
		saved_by = NULL; // new on 081100 -- needed?? -- what is the cost??
	};
//	if (recent_xml_location > 0) { // obsolete 080506
//		remember_included_sprite(recent_xml_location,NULL); // don't need it anymore
//	};
	if (guid != NULL) { // tried moving to ~Sprite but then can be prepared (but not yet deleted) and used
		deregister_guid(guid);
		delete guid;
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 131200 && !tt_shutting_down) {
		Background *b = tt_screen->pointer_to_background();
		if (b->known_leader(this)) {
			log("Destroying something that is still a known leader.");
		};
	};
#endif
};

void Sprite::release_all_references() { // prior to 060204 was (Sprite *by=NULL) {
	if (releasing_all_references) { // new on 230305
		return;
	};
	releasing_all_references = TRUE;
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) { // || kind_of() == INTEGER) { // for now - debugging
		tt_error_file() << "About to release_all_references of debug target on frame " << tt_frame_number << endl;
	};
#endif
	remove_all_followers(TRUE); // new on 020504
	releasing_all_references = FALSE;
};

void Sprite::re_initialize() {
	stop_animations(); 
	// new on 160204 -- should this do more - this much needed for time travel since tt_global_picture and tt_martian re_initialized
};

void Sprite::snapshot(string snapshot_file_name, boolean without_followers) {
	Sprites *saved_followers = NULL;
	if (without_followers) {
		saved_followers = followers;
		followers = NULL;
	};
	create_snapshot(this,snapshot_file_name,FALSE);
	if (without_followers) {
		followers = saved_followers;
	};
};

#if TT_DEBUG_ON||TT_CAREFUL
void Sprite::set_deleted() {
#if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {
      log("Would be deleting the debug target.");
   };
#endif
   deleted_flag = TRUE;
};
#endif

void Sprite::stop_animations() {
	if (animations != NULL && !running_animations) {
		delete animations;
		animations = NULL;
	};
	if (new_animations != NULL) {
		delete new_animations;
		new_animations = NULL;
	};
   cycle_stopped_callback = CALLBACK_NOTHING;
};

void Sprite::clear_changes() {
	//	done_with_previous_values();
	// assume it won't be there next time unless this is reset
//	visible_flag = FALSE;
	if (clean_status == DIRTY) {
		scale_to_fit();
		// been changed since update so do it again
//		recompute_parameters();
	};
	if (!clean()) {
		Sprites *remaining = followers;
		while (remaining != NULL) {
			Sprite *follower = remaining->first();
			if (follower->completely_inside_leader()) { // others handled elsewhere
				follower->clear_changes();
			};
			remaining = remaining->rest();
		};
	};
	// assume it will stay "clean" unless explicitly reset
	// moved to display
//	clean_status = CLEAN;
//       changes_propagated_flag = FALSE;
};

// commented out since now initializes current_image upon creation instead
//TTImage *Sprite::pointer_to_current_image() { 
//	if (current_image == NULL && code != SYNTHETIC_SPRITE && code != USER_SPRITE) { 
//		// new on 190803 since to get cities to load correctly they don't call update_display but this is sometimes needed
//		current_image = image_cycles[code][parameter].nth_image(current_index);
//	};
//	return(current_image);
//};

void Sprite::set_home_notebook(Notebook *notebook) {
   // currently only robots care but robots might be inside
   // a truck or box ...
   Sprites *remaining = followers;
   while (remaining != NULL) {
      remaining->first()->set_home_notebook(notebook);
      remaining = remaining->rest();
   };
};

void Sprite::active_region(TTRegion &region) {
	// by default just the center
	true_center(region.min_x,region.min_y);
	region.max_x = region.min_x+1;
	region.max_y = region.min_y+1;
};

void Sprite::set_cycle_stopped_callback(AnimationCallBack callback, boolean even_if_not_showing) {
	if (cycle_stopped_callback == callback) { // new on 210104 -- fixes a possible infinite recursion
		return;
	};
	if (//tt_finish_all_animations || // noticed on 030903 that this was redundant since finishing_instantly checks it
		 finishing_instantly() || // new on 6/10/98
		 (!even_if_not_showing && !showing_on_a_screen())) {
		AnimationCallBackProcedure procedure = callback_procedure(callback);
		procedure(this);
	} else {
		cycle_stopped_callback = callback;
	};
};

void Sprite::center_location(city_coordinate &x, city_coordinate &y) {
//   y = lly-old_y_offset;
	  x = llx+width/2;
	  y = lly+height/2;
};

void Sprite::true_center(city_coordinate &x, city_coordinate &y) {
//   scale_to_fit(); // so true values are up to date
	// this still drifts a bit -- don't know why
	x = llx+true_width()/2;
	y = lly+true_height()/2;
};

int user_sprite_without_file_name_count = 1; // to recover as gracefully as possible from a User Picture without a file name

void Sprite::convert_to_synthetic_image() { // new on 080403
	log("A picture was saved while ToonTalk couldn't find its image file. Due to a bug that has since been corrected, ToonTalk doesn't know the image file name. Proceeding by turning the picture into a colored rectangle.",FALSE,TRUE);
	set_appearance_code(SYNTHETIC_SPRITE);
	set_parameter(user_sprite_without_file_name_count++);
	current_image = NULL;
};

boolean Sprite::scale_to_fit(city_coordinate new_width, city_coordinate new_height) {
	// changed on 260904 returns FALSE if limited by max unsigned short scale factors
#if TT_DEBUG_ON
	debug_log(debug_counter,"Scale to fit of debug target.");
#endif
	boolean fits_OK = TRUE;
	if (code == USER_SPRITE) { // || tt_picture_directory != NULL) { //true_size_flag) { - tt_picture_directory new on 270901
		if (current_image != NULL) { // condition new on 260900
			if (!current_image->initialized()) { // new on 270901
				draw_surface image = current_image->load_image(FALSE); // NULL,
				if (image == NULL) { // new on 080403 to recover more gracefully
					convert_to_synthetic_image();
					return(scale_to_fit(new_width,new_height));
				};
			};
			// rewrote the following on 200804 to deal better with very big objects (256 times their normal size)
			// changed from long to int64 on 260904
			int64 x_scale_as_int64 = (((int64) 256*new_width)/current_image->width_without_scaling());
			int64 y_scale_as_int64 (((int64)256*new_height)/current_image->height_without_scaling());
			if (x_scale_as_int64 < max_unsigned_short) {
				x_scale = (unsigned short) x_scale_as_int64;
			} else {
				x_scale = max_unsigned_short;
				new_width = (max_unsigned_short/256)*current_image->width_without_scaling();
				// new on 260904 -- multiplying by 256 really
				fits_OK = FALSE;
			};
			if (y_scale_as_int64 < max_unsigned_short) {
				y_scale = (unsigned short) y_scale_as_int64;
			} else {
				y_scale = max_unsigned_short;
				new_height = (max_unsigned_short/256)*current_image->height_without_scaling();
				fits_OK = FALSE;
			};
		};
//#if TT_DEBUG_ON
//		if (x_scale == 0 || y_scale == 0) {
//			tt_error_file() << "Debug this!" << endl;
//		};
//#endif
	} else {
//		x_scale = (unsigned short) ((256*new_width)/maximum_width(code));
//		y_scale = (unsigned short) ((256*new_height)/maximum_height(code));
      city_coordinate max_width = maximum_width(code);
      city_coordinate max_height = maximum_height(code);
		//x_scale = (unsigned short) (((new_width<<8)+max_width/2)/max_width); 
		// added max_width/2 on 250402 to round to nearest - 
		// since in non 640 modes some things became 255x255 scale which is much slower 
		//y_scale = (unsigned short) (((new_height<<8)+max_height/2)/max_height);
		//x_scale = (unsigned short) grow_width_to_640x480_screen_size(x_scale);
		//y_scale = (unsigned short) grow_height_to_640x480_screen_size(y_scale);
		// rewrote the above as below on 200804 to deal better with very big objects (256 times their normal size)
		// updated to use int64 on 260904
		int64 x_scale_as_int64 = ((((int64) new_width<<8)+max_width/2)/max_width); 
		// added max_width/2 on 250402 to round to nearest - 
		// since in non 640 modes some things became 255x255 scale which is much slower 
		int64 y_scale_as_int64 = ((((int64) new_height<<8)+max_height/2)/max_height); // removed (unsigned short) on 260804
		if (x_scale_as_int64 < max_unsigned_short) {
			x_scale_as_int64 = grow_width_to_640x480_screen_size((city_coordinate) x_scale_as_int64);
		};
		if (y_scale_as_int64 < max_unsigned_short) {
			y_scale_as_int64 = grow_height_to_640x480_screen_size((city_coordinate)y_scale_as_int64);
		};
		if (x_scale_as_int64 < max_unsigned_short) {
			x_scale = (unsigned short) x_scale_as_int64;
		} else {
			x_scale = max_unsigned_short;
			fits_OK = FALSE;
		};
		if (y_scale_as_int64 < max_unsigned_short) {
			y_scale = (unsigned short) y_scale_as_int64;
		} else {
			y_scale = max_unsigned_short;
			fits_OK = FALSE;
		};
	};
//#if TT_DEBUG_ON
//   if (x_scale == 12 && y_scale == 46) {
//      log("debug this");
//   };
//#endif
	return(fits_OK);
};

city_coordinate Sprite::width_given_scale(unsigned short int scale) {
	if (code == USER_SPRITE) { // || tt_picture_directory != NULL) { // tt_picture_directory new on 270901
		return((scale*pointer_to_current_image()->width_without_scaling())>>8);
	} else {
		city_coordinate result = (scale*maximum_width(code))>>8;
		return(shrink_width_from_640x480_screen_size(result));
	};
};

city_coordinate Sprite::height_given_scale(unsigned short int scale) {
	if (code == USER_SPRITE) { // || tt_picture_directory != NULL) { // tt_picture_directory new on 270901
		return((scale*pointer_to_current_image()->height_without_scaling())>>8);
	} else {
		city_coordinate result = (scale*maximum_height(code))>>8;
		return(shrink_height_from_640x480_screen_size(result));
	};
};	

void Sprite::set_full_size_and_location(city_coordinate goal_width,
													 city_coordinate goal_height,
													 city_coordinate new_llx,
													 city_coordinate new_lly) {
	city_coordinate new_width,new_height;
	compute_dimensions_to_fit(goal_width,goal_height,new_width,new_height);
	set_size_and_location(new_width,new_height,new_llx,new_lly);
};

void Sprite::set_size_stay_centered(city_coordinate new_width, city_coordinate new_height,
												ChangeReason reason) {
	set_size_and_location(new_width,new_height,
								 llx+(width-new_width)/2,lly+(height-new_height)/2,
								 reason);
};

void Sprite::set_size_and_location(city_coordinate new_width,
											  city_coordinate new_height,
											  city_coordinate new_llx,
											  city_coordinate new_lly,
											  ChangeReason reason) {
//#if TT_CAREFUL
//	if (new_width < 0 || new_height < 0) { // new on 100400
//#if TT_DEBUG_ON
//		log("An attempt to make the size be negative has been ignored.");
//#endif
//		return;
//	};
//#endif
	// commented out the above on 061000 since minimum sizes does it better
	if (reason == TEMPORARY_FOR_DUMPING && ok_to_temporarily_reset_size()) { 
		// new on 051100 since if temporarily reset for dumping no need to go deeper than this
		return;
	};
	if (new_width < 1) new_width = 1; // new on 230903
	if (new_height < 1) new_height = 1; // new on 230903
//	boolean below_minimum_size = FALSE;
	//if (new_width < minimum_width) { // experiment on 140203 - if successful remove below_minimum_size
	//	new_width = minimum_width;
	//	below_minimum_size = TRUE;
	//};
	//if (new_height < minimum_height) {
	//	new_height = minimum_height;
	//	below_minimum_size = TRUE;
	//};
	if (width != new_width || height != new_height) {
//#if TT_DEBUG_ON
//	if (tt_debug_mode == 140499 && (appearance_code() == EXPLOSION_SPRITE || appearance_code() == MARTIAN_SPRITE)) {
//		tt_error_file() << "New " << new_width << "x" << new_height
//				      << "; Old " << width << "x" << height
//						<< "; f=" << tt_frame_number 
//						<< ";counter=" << debug_counter 
//						<< ";reason=" << (int) reason << endl;
//		};
//#endif
#if TT_DEBUG_ON
		//if (new_width > 10000000 || new_height > 10000000) {
		//	log("setting size to more than ten million");
		//};
		if (tt_debug_mode == 100000 && (new_width > 100000 || new_height > 100000)) {
			tt_error_file() << "Setting width to " << new_width << " and height to " << new_height << " ";
			print(tt_error_file());
			tt_error_file() << endl;
		};
#endif
		if (code != SYNTHETIC_SPRITE && !scale_to_fit(new_width,new_height) && !displays_as_pad()) { 
			// and otherwise interferes with large numbers on the ground -- e.g. 2^50000
			// dealing with values too large new on 260904
			// added code != SYNTHETIC_SPRITE on 011004 since wasteful and may get resized below -- only these don't need scale_to_fit
			// displays_as_pad new on 011004 since none of this is appropriate for number and text pads
			city_coordinate limited_new_width, limited_new_height;
			if (tt_log_version_number >= 62 || code == USER_SPRITE) { 
				// made unconditional on 101204 since maximum_width isn't useful if current_image is much smaller than largest (e.g. bomb)
				if (current_image != NULL) {
					limited_new_width = 256*current_image->width_without_scaling();
					limited_new_height = 256*current_image->height_without_scaling();
				} else { 
					// can this happen?
					limited_new_width = new_width;
					limited_new_height = new_height;
				};
			} else {
				limited_new_width = 256*maximum_width(code);
				limited_new_height = 256*maximum_height(code);
			};
			if (limited_new_width > new_width) { // new on 300904
				city_coordinate new_limited_new_width = (city_coordinate) ((((int64) limited_new_height)*new_width)/new_height); 
				// maintain proportionally
				if (new_limited_new_width < limited_new_width) {
					limited_new_width = new_limited_new_width;
				}; // otherwise would get too big so leave it alone
			};
			if (limited_new_height > new_height) { // new on 300904
				city_coordinate new_limited_new_height = (city_coordinate) ((((int64) limited_new_width)*new_height)/new_width); 
				// maintain proportionally
				if (new_limited_new_height < limited_new_height) {
					limited_new_height = new_limited_new_height;
				}; // otherwise would get too big so leave it alone
			};
//			new_llx += (limited_new_width-new_width)/2; // stay centered -- commented out on 300904 since can get lost otherwise (e.g. 2^50000)
//			new_lly += (limited_new_height-new_height)/2;
			new_width = limited_new_width;
			new_height = limited_new_height;
		};
		size_changed_flag = TRUE;
		if (reason == LEADER_CHANGED && completely_inside_leader()) {
			clean_status = DIRTY; // just set -- don't propagate
		} else {
			set_clean_status(DIRTY);
		};
//		if (reason == FOLLOWING_INDIRECTIONS_CHANGE) set_previous_change_reason(reason); // new on 140499
#if TT_DEBUG_ON
		if (debug_counter == tt_debug_target) {
			debug_log(debug_counter,"Debug target size changed.");
			tt_error_file() << "Debug target size is now " << new_width << "x" << new_height << endl;
   //		if (new_width < 1 || new_height < 1) {
			//	print(tt_error_file());
			//	tt_error_file() << " is getting to set to non-positive width or height." << endl;
			//};
		};
#endif
	} else if (llx != new_llx || lly != new_lly) {
		if (reason == LEADER_CHANGED && completely_inside_leader()) {
			if (clean_status != DIRTY) clean_status = MOVED;
		} else {
			set_clean_status(MOVED);
		};
//		if (reason == FOLLOWING_INDIRECTIONS_CHANGE) set_previous_change_reason(reason);
#if TT_DEBUG_ON
		if (debug_counter == tt_debug_target) {
			tt_error_file() << "Debug target moved to " << new_llx << "," << new_lly << " on frame=" << tt_frame_number << endl;
		};
		//if (kind_of() == INTEGER && new_llx > ideal_screen_width) {
		//	debug_this();
		//};
#endif
	} else {
		if (reason == ANIMATION_COMPLETED) {
			scale_to_fit(new_width,new_height);
			if (followers != NULL) {
//				if (!below_minimum_size) { // conditional new on 061000 -- removed 230903
					really_propagate_changes(reason);
//				};
			};
		};
		return; // no change
	};
	llx = new_llx;
	lly = new_lly;
	width = new_width;
	height = new_height;
	if (reason != UPDATING && followers != NULL ) { // && !below_minimum_size) { 
		// third condition new on 061000 
		really_propagate_changes(reason);
	};
//   changed_so_update();
	if (leader != NULL) {
//   need the following to be able to manipulate a follower
//   without leader's involvement
		if (((reason != LEADER_CHANGED && reason != UPDATING && reason != TEMPORARY_FOR_DUMPING) || // third conjunct added 311000
			 leader_offsets == NULL)) { // second disjunct new on 171000 since might have been too small before
//			 !below_minimum_size) { // third condition new on 061000 - commented out 230903
			record_offsets(); // offsets from leader have changed
		};
	};
//	if (top_of_stack) { // new on 260500
//		top_of_stack->set_size_and_location(new_width,new_height,new_llx,new_lly,reason);
//	};
};

void Sprite::set_size_and_location_to_those_of(Sprite *other, ChangeReason reason) {
   set_size_and_location(other->current_width(),other->current_height(),
                         other->current_llx(),other->current_lly(),
                         reason);
};

void Sprite::set_true_size_and_location_to_those_of(Sprite *other, ChangeReason reason) {
   set_true_size_and_location(other->true_width(),other->true_height(), // changed to true_* on 140499
										other->true_llx(),other->true_lly(),
										reason);
};

void Sprite::set_size_and_location_to_true_values_of(Sprite *other, ChangeReason reason) {
   set_size_and_location(other->true_width(),other->true_height(),
								 other->true_llx(),other->true_lly(),
								 reason);
};

void Sprite::move_mark(SelectionFeedback selection_feedback) {
	if (want_feedback(selection_feedback)) {
		tt_screen->move_mark_to(llx+selection_delta_x(),lly+selection_delta_y());
	} else {
		tt_screen->move_mark_to(llx,lly);
	};
};

void Sprite::move_mark_in_region(SelectionFeedback selection_feedback, TTRegion *region) {
	if (want_feedback(selection_feedback)) {
		tt_screen->move_mark_to(region->min_x+selection_delta_x(),
										region->min_y+selection_delta_y());	
	} else {
		tt_screen->move_mark_to(region->min_x,region->min_y);
	};
};

void Sprite::display(SelectionFeedback selection_feedback, boolean followers_too, TTRegion *region) {
   // this used to tried to be clever (in a heursitic way)
   // -- if first follower was underneath...
   // screws up robots whose thought bubble is under and wand is over...
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Displaying debug target.");
	};
#endif
	if (!visible()) return;
//	if (selectable() && rescale_if_camera_above(selection_feedback,followers_too,region)) return; // made obsolete on 230804
	if (pointer_to_top_of_stack() != NULL) { // new on 260500
		pointer_to_top_of_stack()->display(selection_feedback,followers_too,region);
		return;
	};
//	boolean postpone_this = (!followers_completely_inside() &&
//									 followers != NULL &&
//									 followers_too &&
//									 priority() < followers->first()->priority());
//	if (!postpone_this) {
		display_without_followers(selection_feedback,region);
//	};
	if (!followers_too) return;
	// added || is_blank()) return; 
	// blank things don't display parts -- new on 030105 (trucks may be the only ones affected) but not needed
	if (followers != NULL && width > minimum_width && height > minimum_height) { // minimum size conditions new on 120101
   	if (want_feedback(selection_feedback)) {
		   // recursive calls don't show frame
		   selection_feedback = RECURSIVE_MOVEMENT_FEEDBACK;
	   };
		display_followers(selection_feedback,region);
	};
//	if (postpone_this) {
//		display_without_followers(selection_feedback,region);
//	};
};

void Sprite::display_followers(SelectionFeedback selection_feedback, TTRegion *region) {
//	if (region != NULL) {
//		display_followers_in_region(selection_feedback,region);
//		return;
//	};
#if TT_DEBUG_ON
	millisecond start_time = timeGetTime();
#endif
	Sprites *remaining = followers;
	while (remaining != NULL) {
      Sprite *sprite = remaining->first();
		sprite->display_as_a_follower(selection_feedback,region);
		remaining = remaining->rest();
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 3) {
		tt_error_file() << (timeGetTime()-start_time) << C(IDS_MS_TO_DISPLAY_FOLLOWERS);
		describe_briefly(tt_error_file());
		tt_error_file() << " (" << type_name(this) << "#" << debug_counter << ")" << endl;
	};
#endif
};

void Sprite::display_as_a_follower(SelectionFeedback selection_feedback, TTRegion *region) {
	// on 121199 added  && region == NULL since if indirecting here shouldn't be giving feedback
	if (tt_destination_llx > tt_destination_urx) {
		return; // new on 070904 since clipped away
	};
	if (tt_destination_lly > tt_destination_ury) {
		return;
		// new on 070904 since clipped away
	};
   if (this == tt_selection && region == NULL) selection_feedback = MOVEMENT_FEEDBACK;
	if (screen_updates()) { // || (selection_feedback != NO_FEEDBACK)) {
		 if (followers_completely_inside() &&
			  selection_feedback == NO_FEEDBACK_BUT_DISPLAY_OUTSIDE_FOLLOWERS) {
			 selection_feedback = NO_FEEDBACK;
		 };
		 if (region == NULL) {
			 if (clean_or_obscured()) {
				 display(selection_feedback);
			 } else if (tt_postpone_dirty_displays) {
				 if (cached() && !tt_resetting_or_destroying_city) {
					 // tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 
					 // but can cause memory errors while resetting during time travel
#if TT_DEBUG_ON
					 if (tt_debug_mode == 3) {
						 tt_error_file() << " (" << type_name(this) << "#" << debug_counter << ")" << " "
										 << current_clean_status_name()
										 << C(IDS_AND_WAS_CACHED) << endl;
					 };
#endif
					 tt_screen->invalidate_cache();
				 };
				 tt_screen->postpone_layer(this);
			} else {
				 display(selection_feedback);
			};
		} else {
			TTRegion follower_region;
			// follower_region is to region as follower is to leader
			city_coordinate leader_llx,leader_lly;
			leader->lower_left_corner(leader_llx,leader_lly);
			if (leader->is_temporarily_unflipped()) { // added on 171200 since it works - not clear what is going on
				leader_llx += leader->true_x_offset(); 
				leader_lly += leader->true_y_offset();  
			};
			city_coordinate leader_width = leader->current_width();
			if (leader_width == 0) leader_width = 1; // added on 080499 for robustness
			city_coordinate leader_height = leader->current_height();
			if (leader_height == 0) leader_height = 1; // added on 080499 for robustness
			city_coordinate region_width = region->width();
			city_coordinate region_height = region->height();
			follower_region.min_x = region->min_x+((current_llx()-leader_llx)*region_width)/leader_width; 
			follower_region.max_x = follower_region.min_x+(true_width()*region_width)/leader_width; // true added on 290399
			follower_region.min_y = region->min_y+((current_lly()-leader_lly)*region_height)/leader_height;
			follower_region.max_y = follower_region.min_y+(true_height()*region_height)/leader_height;
			//			if (sprite->clean_or_obscured()) {
			if (tt_screen->more_than_pixel_by_pixel(&follower_region)) {
				if (tt_programmer->kind_of() == PROGRAMMER_AT_FLOOR && tt_programmer->programmer_at_floor()->pointer_to_current_selection() == this) {
					// new on 171200 so both "copies" are selected
					selection_feedback = MOVEMENT_FEEDBACK;
				};
				display(selection_feedback,TRUE,&follower_region);
		   };
		};
	};
};
//			} else if (tt_postpone_dirty_displays) {
//				if (sprite->cached()) {
//#if TT_DEBUG_ON
//					if (tt_debug_mode == 3) {
//						tt_error_file() << type_name(sprite) << " "
//										<< sprite->current_clean_status_name()
//										<< " and was cached." << endl;
//					};
//#endif
//					screen->invalidate_cache();
//				};
//				screen->postpone_layer(sprite);
//			} else {
//				sprite->display(follower_region,selection_feedback);
//			};

void Sprite::display_without_followers(SelectionFeedback selection_feedback, TTRegion *region) {
	if (pointer_to_current_image() == NULL) {
		return; // new on 240303 for robustness - warn too?
	};
	if (region != NULL) {
		display_without_followers_in_region(selection_feedback,region);
		return;
	};
	// following added on 040399 to save lots of work when objects are off screen - more common now with scrolling
//	if (llx+width < tt_screen->min_x() || llx > tt_screen->max_x()) return;
//	if (lly+height < tt_screen->min_y() || lly > tt_screen->max_y()) return;
	if (tt_screen->off_screen(llx,lly,width,height)) {
		return; // re-written on 150202 since no point calling tt_screen 4 times
	};
	move_mark(selection_feedback);
#if TT_DEBUG_ON
	millisecond start_time = timeGetTime();
#endif
//	if (//tt_destination_lly > 0 &&
//		 code == EXPANDER_SPRITE) {
		// this seems to fix a mysterious problem
//		tt_memory_graphics_mark_y += 4;
//	};
	boolean discovered_transparency = FALSE;
#if TT_32
	if (true_size_flag) { // added on 100399 to improve geometry and avoid black lines at edges of some user pictures
		long pixel_width = tt_screen->horizontal_pixel_length(width);
		long pixel_height = tt_screen->vertical_pixel_length(height);
		// on 031000 "fix" pictures that are very very large
		// experiment on 210804 to turn this off
//#if TT_CAREFUL
//		if (pixel_width*pixel_height > 50000000) { // 50 million seems big enough for legitimate uses (new on 031000)
//			set_size(ideal_screen_width,ideal_screen_height);
//			tt_error_file() << "Pixel width of " << pixel_width << " and height of " << pixel_height << " is too big.  ";
//#if TT_DEBUG_ON
//			print(tt_error_file());
//#endif
//			tt_error_file() << endl;
//			tt_error_file() << "Size reset to dimensions of the screen." << endl;
//			pixel_width = tt_screen->horizontal_pixel_length(width);
//		   pixel_height = tt_screen->vertical_pixel_length(height);
//		};
//#endif
		pointer_to_current_image()->display(pixel_width,pixel_height,discovered_transparency,
			                                 color_permutation(selection_feedback),x_scale,y_scale);
	} else 
#endif
	{
		long pixel_width = 0;
		long pixel_height = 0;
		if (scale_for_height_of_camera()) {
			// new on 230804 so looks right from helicopter (e.g. box full of various non-picture objects)
			city_scale screen_scale = tt_screen->current_scale();
			unsigned short adjusted_x_scale = (unsigned short) ((x_scale*ground_scale)/screen_scale); 
			unsigned short adjusted_y_scale = (unsigned short) ((y_scale*ground_scale)/screen_scale);
			pointer_to_current_image()->display(pixel_width,pixel_height,discovered_transparency,
															color_permutation(selection_feedback),adjusted_x_scale,adjusted_y_scale);
		} else {
			//  helicopter and houses do this -- doesn't want to adjust for height
			pointer_to_current_image()->display(pixel_width,pixel_height,discovered_transparency,
				                                 color_permutation(selection_feedback),x_scale,y_scale);
		};
	};
	if (discovered_transparency) { // new on 210203
		set_show_all(FALSE);
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 3) {
		tt_error_file() << (timeGetTime()-start_time) << C(IDS_MS_TO_DISPLAY);
		describe_briefly(tt_error_file());
		tt_error_file() << " (" << type_name(this) << "#" << debug_counter << ")" << endl;
	};
#endif
};

void Sprite::display_without_followers_in_region(SelectionFeedback selection_feedback, TTRegion *region) {
	move_mark_in_region(selection_feedback,region);
#if TT_DEBUG_ON
	millisecond start_time = timeGetTime();
#endif
	unsigned short int region_x_scale 
		 = (unsigned short) ((region->width()<<8)/pointer_to_current_image()->width_without_scaling()); 
	//maximum_width(code));
	unsigned short int region_y_scale 
		 = (unsigned short) ((region->height()<<8)/pointer_to_current_image()->height_without_scaling()); 
	//maximum_height(code));
//	if (true_size_flag) {
		long region_width = max(1,tt_screen->horizontal_pixel_length(region->width())); 
		// updated 210804 so has minimum value of 1
		long region_height = max(1,tt_screen->vertical_pixel_length(region->height()));
		boolean discovered_transparency = FALSE;
		current_image->display(region_width,region_height,discovered_transparency,
									  color_permutation(selection_feedback),
									  region_x_scale,region_y_scale);
		if (discovered_transparency) { // new on 210203
			set_show_all(FALSE);
		};
//	} else { // else like Sprite::display_without_followers use x_scale and y_scale
//		current_image->display(0,0,region_x_scale,region_y_scale);
//	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 3) {
		tt_error_file() << (timeGetTime()-start_time) << C(IDS_MS_TO_DISPLAY);
		describe_briefly(tt_error_file());
		tt_error_file() << " (" << type_name(this) << "#" << debug_counter << ")" << endl;
	};
#endif
};

void Sprite::save_snapshot(string file_name, boolean white) {
	//lock_back_surface(); // ensure there is a back buffer to write on
	//unlock_back_surface();
	// first make everything black
	tt_main_window->initialize_direct_draw(TRUE); // just enough for this purpose
	 // new on 151003
BEGIN_GDI
	if (white) {
		tt_main_window->select_stock_object(WHITE_BRUSH);
		tt_main_window->select_stock_object(WHITE_PEN); // new on 130405
	} else {
		tt_main_window->select_stock_object(BLACK_BRUSH);
		tt_main_window->select_stock_object(BLACK_PEN); // new on 130405
	};
//	tt_main_window->select_stock_object(NULL_PEN); // rewritten on 130405 since the edge uses the pen
	tt_main_window->fill_box(0,0,tt_screen_width,tt_screen_height);
END_GDI
	city_coordinate saved_lly = lly;
	// not clear why but if vertically centered this works fine
	city_coordinate saved_llx = llx; // save x as well since if too big will shrink
	city_coordinate saved_width = width;
	city_coordinate saved_height = height;
	city_coordinate full_width = true_width(TRUE); // new on 0606003 to deal with things like robot's and their thought bubbles
	city_coordinate full_height = true_height(TRUE);
	if (full_width > ideal_screen_width || height > ideal_screen_height) {
		// fit to screen and stay proportional
		double excess_width = full_width/(double) ideal_screen_width;
		double excess_height = full_height/(double) ideal_screen_height;
		if (excess_width > excess_height) {
			set_size(ideal_screen_width,(city_coordinate) (full_height/excess_width));
		} else {
			set_size((city_coordinate) (full_width/excess_height),ideal_screen_height);
		};
		full_width = true_width(TRUE); // new on 040704 to recompute these for use below (so snapshot is the right size)
		full_height = true_height(TRUE);
	};
//	move_to(ideal_screen_width/2-full_width/2,ideal_screen_height/2-full_height/2); // move to center of screen
	move_to(0,0); // new on 040704 since if partly off screen then snapshot was as well
	really_propagate_changes();
	UnwindProtect<boolean> set(tt_postpone_dirty_displays,FALSE);
	//boolean saved_screen_updates = screen_updates();
	//set_screen_updates(TRUE); // new on 060603 to deal with followers not contained inside
//	display();
	recursively_display_followers_that_screen_updates();
	//set_screen_updates(saved_screen_updates);
	lock_back_surface();
	coordinate min_x = tt_screen->screen_x(true_llx(TRUE));
	coordinate min_y = tt_screen->screen_y(true_lly(TRUE));
//#if TT_POST329
//	city_coordinate one_x_pixel = tt_screen->one_pixel_in_x_city_units();
//	full_width -= (one_x_pixel-1); // new on 221103 to round down
//	city_coordinate one_y_pixel = tt_screen->one_pixel_in_y_city_units();
//#endif
   copy_screen_to_file(file_name,min_x,min_x+tt_screen->horizontal_pixel_length(full_width)-1,
							  min_y,min_y+tt_screen->vertical_pixel_length(full_height)-1);
	// rewritten with version 3.29
// copy_screen_to_file(file_name,min_x,min_x+tt_screen->horizontal_pixel_length(full_width),
							  // min_y,min_y+tt_screen->vertical_pixel_length(full_height));
	set_size_and_location(saved_width,saved_height,saved_llx,saved_lly);
	unlock_back_surface();
	tt_screen->screen_dirty(); // probably not necessary
	release_direct_draw();
};

void Sprite::recursively_display_followers_that_screen_updates() {
	display();
	Sprites *remaining = followers;
	while (remaining != NULL) { // new on 060603 to deal with followers not contained inside
		Sprite *follower = remaining->first();
		if (!follower->screen_updates()) {
			follower->recursively_display_followers_that_screen_updates();
		};
		remaining = remaining->rest();
	};
};

void Sprite::recursively_set_slated_for_deletion(boolean flag) { // new on 140904
	set_slated_for_deletion(flag);
	Sprites *remaining = followers;
	while (remaining != NULL) { // new on 060603 to deal with followers not contained inside
		Sprite *follower = remaining->first();
		follower->recursively_set_slated_for_deletion(flag);
		remaining = remaining->rest();
	};
};

boolean Sprite::outside() {
   if (floor == NULL) {
      return(FALSE); // am neither inside nor outside...
   } else {
      return(floor->outside());
   };
};

boolean Sprite::resize_if_in_room() {
   if (floor->in_the_room_with_programmer()) { // updated 020205
      city_coordinate sprite_width = width;
      city_coordinate sprite_height = height;
      tt_programmer->translate_floor_to_room_dimensions(sprite_width,sprite_height);
      set_size(sprite_width,sprite_height);
      return(TRUE);
   };
   return(FALSE);
};

void Sprite::dump_geometry(output_stream &stream, city_coordinate delta_x, city_coordinate delta_y) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
		print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
		tt_error_file() << "Dumping geometry." << endl;
	};
#endif
	dump_just_geometry(stream,delta_x,delta_y);
	dump_priority(stream);
	dump_misc(stream); // new on 130100
};

void Sprite::dump_just_geometry(output_stream &stream, city_coordinate delta_x, city_coordinate delta_y) {
	//	city_coordinate adjusted = llx-old_x_offset; // new on 130100 for better accuracy
#if TT_DEBUG_ON
	if (width > 100000 || height > 100000 || width < 0 || height < 0) {
		tt_error_file() << "Dumping geometry where width is " << width << " and height is " << height << " ";
		print(tt_error_file());
		tt_error_file() << endl;
		if (leader != NULL && (leader->current_width() == 0 || leader->current_height() == 0)) {
			tt_error_file() << "But since leader has size zero, resetting this as well." << endl;
			set_size_and_location(0,0,0,0);
		};
	};
#endif
	city_coordinate adjusted_llx = llx+delta_x;
	city_coordinate adjusted_lly = lly+delta_y;
   stream.write((string) &adjusted_llx,sizeof(adjusted_llx));
   stream.write((string) &adjusted_lly,sizeof(adjusted_lly));
   stream.write((string) &width,sizeof(width));
   stream.write((string) &height,sizeof(height));
};

void Sprite::dump_misc(output_stream &stream) { // new on 130100
	stream.write((char *) &current_index, sizeof(current_index));
	stream.write((char *) &parameter, sizeof(parameter));
	stream.write((char *) &cycle_start_time, sizeof(cycle_start_time));
	stream.write((char *) &old_x_offset, sizeof(old_x_offset));
	stream.write((char *) &old_y_offset, sizeof(old_y_offset));
   unsigned char flags = (looped_back_already | 
								  (single_step<<1) |
								  (available_for_indexing_flag<<2) |
								  (graspable_flag<<3) |
								  (selectable_flag<<4) |
								  (blank<<5) | // probably covered elsewhere
								  (containable_by_background_flag<<6) |
								  (move_between_floor_and_room_flag<<7));
	stream.put(flags);
};

void Sprite::transfer_geometry_and_misc(Sprite *other) {
	// other holds all the geometry and the like
//	set_size_and_location_to_those_of(other);
	set_parameter(other->current_parameter());
	reset_cycle(); // new on 180100
	llx = other->current_llx();
	lly = other->current_lly();
	if (appearance_code() != HAND_SPRITE) { // condition new on 110700 since if loaded city with different screen resolution this screws things up
#if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {
      log("In transfer_geometry_and_misc, setting the width and height of debug target.");
   };
#endif
      width = other->current_width();
		height = other->current_height();
	};
	current_priority = other->priority();
	current_index = other->image_cycle_index();
	cycle_start_time = other->return_cycle_start_time();
	old_x_offset = other->return_old_x_offset();
	old_y_offset = other->return_old_y_offset();
	looped_back_already = other->cycle_looped_back(); 
	single_step = other->return_single_step();
	available_for_indexing_flag = other->available_for_indexing();
	graspable_flag = other->graspable();
	selectable_flag = other->selectable();
	blank = other->is_blank();
	containable_by_background_flag = other->containable_by_background();
	move_between_floor_and_room_flag = other->containable_by_background();
	if (kind_of() != SYNTHETIC_SPRITE && kind_of() != USER_SPRITE) {
		current_image = image_cycles[code][parameter].nth_image(current_index); // new on 180100
	};
};

void Sprite::load_misc(InputStream *stream, boolean warn) { // new on 130100
	stream->read((string) &current_index, sizeof(current_index));
	short int initial_parameter; // read then set so set_parameter can trigger class-specific behavior
	stream->read((string) &initial_parameter, sizeof(initial_parameter));
	stream->read((string) &cycle_start_time, sizeof(cycle_start_time));
	stream->read((string) &old_x_offset, sizeof(old_x_offset));
	stream->read((string) &old_y_offset, sizeof(old_y_offset));
	unsigned char flags = stream->get();
	looped_back_already = !!(flags&1);
	single_step = !!(flags&(1<<1));
	available_for_indexing_flag = !!(flags&(1<<2));
	graspable_flag = !!(flags&(1<<3));
	selectable_flag = !!(flags&(1<<4));
	blank = !!(flags&(1<<5));
	containable_by_background_flag = !!(flags&(1<<6));
	move_between_floor_and_room_flag = !!(flags&(1<<7));
//	set_parameter(initial_parameter,TRUE,FALSE,TRUE); // new on 180100
	just_set_parameter(initial_parameter); // rewritten on 250803 -- if wrong make a 3rd version of set_parameter with old behavior
	if (code != SYNTHETIC_SPRITE && code != USER_SPRITE) {
#if TT_CAREFUL
		if (image_cycles[code][parameter].return_number_of_images() <= current_index) { // new on 150400
			if (!warn) return;
			tt_error_file() << "Error while loading image index of something. "; 
#if TT_DEBUG_ON
			debug_this();
			print(tt_error_file());
#endif
			tt_error_file() << endl;
         if (tt_want_exception_breaks) {
			   return;
         } else { 
			   throw(tt_global_picture);
         };
		};
#endif		
		if (parameter < 0) {
			parameter = 0; // new on 040304 for robustness
		};
		current_image = image_cycles[code][parameter].nth_image(current_index); // new on 180100
		update_cycle_duration(); // new on 310100
	};
};

void Sprite::dump_priority(output_stream &stream) { // made a separate function on 110100
	// was current_priority prior to 060699
	// and was just floor_priority prior to 211199
   if (floor != NULL && floor->on_the_floor()) {
		// if haven't stood up since this was created or priority changed then this is the more up-to-date value
		stream.write((string) &current_priority,sizeof(current_priority));
	} else {
		stream.write((string) &floor_priority,sizeof(floor_priority));
	};
};

void Sprite::load_geometry(InputStream *stream, int version_number, boolean warn) {
	load_just_geometry(stream,version_number,warn);
	load_priority(stream,warn);
	if (version_number >= first_version_to_save_number_colors) { // new on 130100
		load_misc(stream,warn);
	};
	finished_loading(); // new on 170100
};

void Sprite::load_just_geometry(InputStream *stream, int version_number, boolean warn) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
		print_spaces(tt_debug_load_depth,tt_error_file());
		tt_error_file() << "Loading geometry." << endl;
	};
#endif
//   stream.read((unsigned char *) &llx,sizeof(llx));
//   stream.read((unsigned char *) &lly,sizeof(lly));
//   stream.read((unsigned char *) &width,sizeof(width));
//   stream.read((unsigned char *) &height,sizeof(height));
//   stream.read((unsigned char *) &current_priority,sizeof(current_priority));
   city_coordinate x,y,w,h;
   stream->read((string) &x,sizeof(x));
   stream->read((string) &y,sizeof(y));
   stream->read((string) &w,sizeof(w));
   stream->read((string) &h,sizeof(h));
#if TT_DEBUG_ON
	if (w < 0 || h < 0) {
		print(tt_error_file());
		tt_error_file() << " loading negative size - something is wrong with saved file or clipping." << endl;
	} else if (tt_debug_mode == 300100 && (w == 0 || h == 0)) {
		log("Loading zero sized item.");
	};
#endif
   set_size_and_location(w,h,x,y,LOADING_GEOMETRY);
};

void Sprite::load_priority(InputStream *stream, boolean warn) {
   long p;
   stream->read((string) &p,sizeof(p));
   set_priority(p);
	just_set_floor_priority(p); // new on 060699
};

void Sprite::dump_common_flags(output_stream &stream) {
	// new on 080400
	unsigned char flags = (blank | // this is the original dumped flag
								  (infinite_stack()<<1));
	stream.put(flags);
};

void load_common_flags(InputStream *stream, boolean &blank, boolean &infinite_stack) {
	// blank is treated specially to fit in with old code
	unsigned char flags = stream->get();
	blank = !!(flags&1);
	infinite_stack = !!(flags&(1<<1));
};

boolean Sprite::top_level_dump(output_stream &stream) { // new on 060201
//	int saved_tt_skip_first_n_dumped = tt_skip_first_n_dumped;
//	tt_skip_first_n_dumped = tt_number_of_sprites_dumped_so_far; // skip all the current ones
	int starting_tt_number_of_sprites_dumped_so_far = tt_number_of_sprites_dumped_so_far;
   boolean result = dump(stream);
//	tt_skip_first_n_dumped = saved_tt_skip_first_n_dumped;
	for (int i = starting_tt_number_of_sprites_dumped_so_far; i < tt_number_of_sprites_dumped_so_far; i++) {
		tt_sprites_being_dumped_ok_for_eq[i] = FALSE;
	};
	tt_number_of_sprites_dumped_so_far = starting_tt_number_of_sprites_dumped_so_far; 
	// new on 270202 - why was this missing? - need to restore this, right?
	return(result);
};

boolean Sprite::dump(output_stream &stream, boolean just_prepare, boolean , boolean ) {
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
   stream.put((unsigned char) sprite_type);
	// what about comments?? -- callers all dump it afterwards (not Martian and Tooly - maybe others?) but it would be more modular to do it here
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

boolean Sprite::ok_to_delete() {
//#if TT_DEBUG_ON
//	if (tt_debug_mode == 220704 && code == TRUCK_SIDE_SPRITE && ref_count == 1) {
//		tt_error_file() << "ok_to_delete truck from side#" << debug_counter << endl;
////		ref_count++;
//	};
//#endif
	decrement_ref_count();
#if TT_DEBUG_ON
   if (tt_debug_mode == 7654) {
      tt_error_file() << ref_count << " is the ref count of ";
      print(tt_error_file());
      tt_error_file() << endl;
//      if (ref_count == 0) {
//          tt_error_file() << "DEBUG" << endl;
//      };
//      return(FALSE); // never really deleted this way
   };
#endif
   return(ref_count <= 0); // was == prior to 090203
};

Sprite *Sprite::first_follower_of_type(SpriteType type) {
	if (followers == NULL) {
		return(NULL);
	};
	// re-written 101100 to avoid extra work of computing reverse and so others can call last_element_of_type
	return(followers->last_element_of_type(type)); // last because followers are most recent (towards lower right corner) first
	// new on 260499 so that if there are multiple ones it finds the top one
};

/* 
	Sprites *reversed_followers = followers->reverse();
	Sprites *remaining = reversed_followers;
	while (remaining != NULL) {
		Sprite *sprite = remaining->first();
		if (sprite->kind_of() == type) {
			just_destroy_list(reversed_followers);
			return(sprite);
		};
		remaining = remaining->rest();
	};
	just_destroy_list(reversed_followers);
	return(NULL);
*/

boolean Sprite::inside_toolbox_vacuum() {
	if (inside_vacuum() && tt_toolbox != NULL) {
		Vacuum *vacuum = tt_toolbox_vacuum;
		if (vacuum != NULL) {
			Sprites *sucked_up = vacuum->vacuumed_up();
			if (sucked_up != NULL) {
				return(sucked_up->member_cons(this) != NULL);
			};
		};
	};
	return(FALSE);
};

boolean Sprite::on_back_of_picture() {
	return(floor != NULL && floor->pointer_to_picture() != NULL);
};

boolean Sprite::on_back_of_picture_or_limbo() { // new on 091100
	// added test for ultimate_leader to be in limbo as well on 081200
	return((floor == NULL && ultimate_leader()->pointer_to_background() == NULL) || on_back_of_picture());
};

void Sprite::stop_all(boolean for_good, boolean suspend_robots_too) {
//	if (tt_log_version_number < 15) return;
   // could send this to Sprites::stop_all instead
	Sprites *remaining = followers;
	while (remaining != NULL) {
		remaining->first()->stop_all(for_good,suspend_robots_too);
		remaining = remaining->rest();
	};
};

// failed experiment on 290802 since robots run much too soon
//void Sprite::activate(boolean reset_animation, PictureActivateContext context, millisecond delay) {
//   // pictures_only added 121000
//   // did nothing prior to 290802
//   Sprites *remaining = followers;
//	while (remaining != NULL) {
//		remaining->first()->activate(reset_animation,context,delay);
//		remaining = remaining->rest();
//	};
//};

void Sprite::set_active(boolean new_flag, boolean recur, boolean initialization) { // new on 191199
//   if (active_flag == new_flag) return; // new on 170702 to avoid unneeded work -- 
	// commented out on 180702 since if recur the "children" might not know
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		tt_error_file() << "Setting active flag of debug target to " << (int) new_flag << endl;
	};
#endif
	active_flag = (flag) new_flag;
	if (!recur) return;
	Sprites *remaining = followers;
	while (remaining != NULL) {
		remaining->first()->set_active(new_flag,recur,initialization); // recur arg was missing prior to 120799
		remaining = remaining->rest();
	};
};

//void Sprite::set_activatable(boolean new_flag, boolean recur) { // new on 160702
//#if TT_DEBUG_ON
//	if (tt_debug_target == debug_counter) {
//		tt_error_file() << "Setting activatable flag of debug target to " << (int) new_flag << endl;
//	};
//#endif
//	activatable_flag = (flag) new_flag;
//	if (!recur) return;
//	Sprites *remaining = followers;
//	while (remaining != NULL) {
//		remaining->first()->set_activatable(new_flag,recur); // recur arg was missing prior to 120799
//		remaining = remaining->rest();
//	};
//};

void Sprite::set_comment(Sprite *new_comment) {
	if (comment != NULL) {
		comment->destroy();
	};
	comment = new_comment;
};

void Sprite::describe_comment(output_stream &message) {
	if (comment == NULL || alternative_language_spoken()) return;
	string text;
	long text_length;
	if (comment->make_sound()) { // new on 290699
		return; // do nothing more if already made a sound
	} else if (comment->current_text(text,text_length)) {
		message << text;
	} else {
		describe_type(message,INDEFINITE_ARTICLE);
		message << space() << S(IDS_THAT_REMINDS_ME_OF) << space();
		comment->describe_briefly(message,INDEFINITE_ARTICLE);
		message << S(IDS_THAT_REMINDS_ME_OF2,"");
	};
	if (text_length > 0 &&
		 (text[text_length-1] == '.'|| text[text_length-1] == '!' || text[text_length-1] == '?' || text[text_length-1] == ',')) {
		message << space();
	} else {
		message << end_sentence();
	};
	describe_type(message,DEFINITE_ARTICLE,TRUE,TRUE); // Capitalize first letter
	message << S(IDS_IS);
};

void Sprite::now_on_top_of_nest(boolean ) {
	if (showing_on_a_screen()) {
		recursively_notice_on_screen_change(TRUE);
	};
};

void Sprite::restore_floor_priority() {
	if (floor_priority != min_long) {
		current_priority = floor_priority;
		if (floor != NULL) floor->set_current_priority_if_lower(current_priority);
	} else if (floor != NULL) {
		current_priority = floor->next_item_priority();
	};
};

// made obsolete on 230804
//boolean Sprite::rescale_if_camera_above(SelectionFeedback selection_feedback,
// 													 boolean followers_too, TTRegion *region) {
//	if (region != NULL) return(FALSE); // didn't do anything
//   if (tt_screen->camera_status() != CAMERA_ABOVE) return(FALSE);
//	if (!adjust_height_if_camera_above()) return(FALSE); // new on 210100 // made obsolete on 230804
//   city_scale scale = tt_screen->current_scale();
//   city_coordinate adjusted_height;
//    // ground decoration as seen from above
//   adjusted_height = min(tile_height,height); // since from above is just a few pixels deep
//   city_coordinate urx, ury;
//   urx = llx+(city_coordinate) (((int64) ground_scale)*width)/scale; // added int64 on 070202 but maybe ground scale should just be an int64??
//   ury = lly+(city_coordinate) (((int64) ground_scale)*adjusted_height)/scale;
//   TTRegion region_viewed_above(llx,urx,lly,ury);
//   display(selection_feedback,followers_too,&region_viewed_above);
//   return(TRUE);
//};

void Sprite::set_in_notebook(Notebook *notebook, boolean recur) {
   // prior to 210802 took a flag rather than the notebook itself
//   if (in_notebook_flag == new_flag) return; // new on 170702 to ignore calls that already know about 
	// commented out on 190702 since might not have been called with recur flag earlier
   in_notebook_flag = (notebook != NULL);
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		tt_error_file() << "Setting in_notebook_flag of debug target to " << (int) in_notebook_flag << endl;
	};
	if (deleted()) {
		log("set_in_notebook called on a deleted object");
	};
#endif
   if (recur) { // conditional added on 190702
	   Sprites *remaining = followers;
	   while (remaining != NULL) {
		   remaining->first()->set_in_notebook(notebook,recur);
		   remaining = remaining->rest();
	   };
   };
};

//#if TT_DIRECT_PLAY
//void Sprite::set_being_marshalled(boolean new_flag) {
//   being_marshalled_flag = (flag) new_flag;
//	Sprites *remaining = followers;
//	while (remaining != NULL) {
//		remaining->first()->set_being_marshalled(new_flag);
//		remaining = remaining->rest();
//	};
//};
//#endif

boolean Sprite::current_long_value(long &value, boolean best_fit) { 
	// prior to 030302 best_fit was round_off - now if too large or small then max_long or min_long is ued
	NumberValue *number_value = NULL; // initialized to NULL on 160702 for robustness
	boolean number_value_shared;
	boolean value_ok = current_value(number_value,number_value_shared);
	// prior to 020302 used to return FALSE here if !value_ok but the value is sometimes needed - e.g. when a sensor is inactive
	if (number_value == NULL) { // new on 030302
		value = -1; // not clear if this should be rationalized but is how things worked once
		return(FALSE); // doesn't really have a value but callers might not check so set value to -1
	};
	boolean result = number_value->long_value(value,best_fit); 
	if (!result & best_fit) { // new on 030302
		if (number_value->sign() < 0) {
			value = min_long;
		} else {
			value = max_long;
		};
		result = TRUE;
	};
	if (!number_value_shared) delete number_value;
	return(result && value_ok);
};

boolean Sprite::is_integer() { // new on 050803
	NumberValue *number_value = NULL; // initialized to NULL on 160702 for robustness
	boolean number_value_shared;
	boolean value_ok = current_value(number_value,number_value_shared);
	if (number_value == NULL) { // new on 030302
		return(FALSE);
	};
	long ignore_value;
	boolean result = TRUE;
	if (!number_value->long_value(ignore_value,FALSE)) { // maybe is a bignum
		bignum_pointer ignore;
		result = number_value->bignum_value(ignore); 
	};
	if (!number_value_shared) delete number_value;
	return(result && value_ok);
};

boolean Sprite::current_double_value(double &double_value, boolean best_fit) { // new on 030302
	NumberValue *number_value;
	boolean number_value_shared;
	if (!current_value(number_value,number_value_shared)) {
		if (best_fit) double_value = 0;
		return(FALSE);
	};
	boolean result = number_value->double_float_value(double_value,best_fit); 
	if (!result & best_fit) { // new on 030302
		// was wrong prior to 251004 -- ± (2-2^-52)^1023 is the correct max and min
		if (number_value->sign() < 0) {
			double_value = -1.7E308; // was min_long;
		} else {
			double_value = 1.7E308; // was max_long;
		};
		result = TRUE;
	};
	if (!number_value_shared) delete number_value;
	return(result);
};

#if TT_DEBUG_ON
// otherwise in-lined
void Sprite::increment_ref_count(Sprite *by) {
	if (tt_debug_target == debug_counter) { // || kind_of() == INTEGER) { // for now - debugging
		tt_error_file() << "About to increment ref count of debug target from " << ref_count << " on frame " << tt_frame_number << endl;
	};
   ref_count++;
   if (tt_debug_mode == 1717 || tt_debug_target == debug_counter) {
		if (ref_count > 10) {
			log("Unusually high ref count.");
		};
      tt_error_file() << "Ref count of ";
      print(tt_error_file());
      tt_error_file() << " incremented to " << return_ref_count();
      if (by != NULL) {
         tt_error_file() << " changed by ";
         by->print(tt_error_file());
         tt_error_file() << endl;
      } else {
         tt_error_file() << " changed by a suspension cell or un-reported by." << endl;
      };
   };
};

void Sprite::decrement_ref_count() {
	if (tt_debug_target == debug_counter) {
		tt_error_file() << "About to decrement ref count of debug target from " << ref_count 
							 << " on frame " << tt_frame_number << endl;
	};
   if (ref_count == 0) { 
		debug_this();
      tt_error_file() << "Ref count is negative ";
      print(tt_error_file());
      tt_error_file() << endl;
		tt_err_file_important = TRUE;
   } else {
      ref_count--;
   };
   if (tt_debug_mode == 1717 || tt_debug_target == debug_counter) {
      tt_error_file() << "Ref count of ";
      print(tt_error_file());
      tt_error_file() << " decremented to " << return_ref_count() << endl;
   };
};

void Sprite::print(output_stream &stream) {
   stream << type_name(this) << "#" << debug_counter;
};

//void Sprite::display_debug_info(output_stream &stream, int indent) {
//	if (leader != NULL && !leader->does_follower_matter(this)) return; // e.g. labels
//	for (int i = 0; i < indent; i++) stream << " ";
//	stream << "w=" << width 
//			 << "; h=" << height 
//			 << "; x=" << llx 
//			 << "; y=" << lly 
//			 << "; priority=" << priority
//			 << "; param=" << parameter 
//			 << "; index=" << current_index
//			 << "; cycle_start_time=" << cycle_start_time
//			 << "; frame=" << tt_frame_number
//			 << "; " << type_name(this) << endl;
//	if (this != tt_toolbox) { // because it has a hack of using saved_followers 
//		// (if I need to debug something in Tooly could use followers or saved_followers whichever exists)
//		Sprites *remaining = followers;
//		while (remaining != NULL) {
//			remaining->first()->display_debug_info(stream,indent+1);
//			remaining = remaining->rest();
//		};
//	};
//};

#endif

//#if !TT_32
// 16-bit compiler complained that this was re-defined multiply...
//boolean is_kind_of(Sprite *sprite, SpriteType type) {
//   return((boolean) ((sprite==NULL)?FALSE:(sprite->kind_of()==type)));
//};
//#endif

/*
	unsigned short adjusted_x_scale = x_scale,
						adjusted_y_scale = y_scale;
	switch (selection_feedback) {
		case NO_FEEDBACK:
			screen->move_mark_to(llx,lly);
			break;
		case SIZE_CHANGE_FEEDBACK:
		case RECURSIVE_SIZE_CHANGE_FEEDBACK: {
			int x_growth = selection_x_growth();
			int y_growth = selection_y_growth();
			screen->move_mark_to(llx-(x_growth*screen->one_pixel_in_x_city_units())/2,
										lly-(y_growth*screen->one_pixel_in_y_city_units())/2);
			adjusted_x_scale = current_image->increase_x_scale(x_scale,x_growth);
			adjusted_y_scale = current_image->increase_y_scale(y_scale,y_growth);
			break;
		  };
		case MOVEMENT_FEEDBACK:
		case RECURSIVE_MOVEMENT_FEEDBACK:
			screen->move_mark_to(llx+selection_delta_x(),lly+selection_delta_y());
			break;
	};
	current_image->display(adjusted_x_scale,adjusted_y_scale);
*/

/*
void Sprite::display_picture_frame(SelectionFeedback selection_feedback,
											  city_coordinate &frame_llx,
											  city_coordinate &frame_lly) {

	switch (selection_feedback) {
		case NO_FEEDBACK:
			frame_llx = llx;
		   frame_lly = lly;
			break;
		case SIZE_CHANGE_FEEDBACK: // do movement instead for now
		case MOVEMENT_FEEDBACK:
			frame_llx = llx+selection_delta_x();
			frame_lly = lly+selection_delta_y();
			break;
	};
	tt_screen->move_mark_to(frame_llx,frame_lly);
	unsigned short x_scale = (unsigned short) ((256*width)/maximum_width(FRAME_SPRITE));
	unsigned short y_scale = (unsigned short) ((256*height)/maximum_height(FRAME_SPRITE));
	image_cycles[FRAME_SPRITE][0].nth_image(0)->display(x_scale,y_scale);
};
*/

/*
string type_strings[] =
	  {"INTEGER",
		"EXPANDER", "VACUUM", "COPIER",
		"THOUGHT_BUBBLE",
		"ROBOT",
		"NEST", "BIRD",
		"TRUCK", "PROGRAM_PAD",
		"CUBBY",
		"TOOLBOX",
		"PICTURE",
		"BOMB",
		"TEXT", 
		"REMOTE_PICTURE",
		"LAST_MANIPULABLE", // after this are non-graspable items
		"PICTURE_INSIDES",
		"PROGRAMMER",
		"DOOR", "OPEN_DOOR", "HOUSE_ABOVE", "HOUSE",
		"HELICOPTER",
		"ROBOT_IN_TRAINING",
		"PROGRAMMER_ARM", "MOUSE_WITH_HAMMER",
		"TALK_BALLOON", "MARTIAN", // after this are types just for load & dump
		"BLANK_INTEGER", "ERROR_INTEGER", 
		"BLANK_CUBBY", "LABELED_CUBBY",
		"BLANK_PICTURE", 
		"BLANK_TEXT",
		"PICTURE_WITH_INSIDES",
		"REMOTE_INTEGER",
		"GLOBAL_REMOTE_INTEGER",
		"PICTURE_SCALE",
		"REMOTE_APPEARANCE",
      "PICTURE_WITH_INDIRECTION",
		"NONE_GIVEN"};
*/

const_string type_name(Sprite *sprite) {
	SpriteType type = sprite->kind_of();
	if (type >= 0 && type <= LAST_SPRITE_TYPE) { //(type >= 0 && not needed if enum compiled not as ints
		return(type_string(type)); ;
   } else if (type == NONE_GIVEN) {
      return(S(IDS_NONE_GIVEN));
   } else {
		return(S(IDS_INVALID_TYPE));
   };
};

const_string type_string(int i, boolean capitalize_first) {
   if (i >= NONE_GIVEN) {
      return("Unknown");
   } else {
	   return(S_maybe_capitalize_first(IDS_TYPE_NAME+i,capitalize_first));
   };
};

//boolean is_tool(SpriteType type) {
	// better called graspable or manipulable...
//	return(type < LAST_MANIPULABLE);
//};

boolean Sprite::check_pending_for_equals(Sprite *other, Sprites *pending) { // new on 050601
	Sprites *member = pending->member_cons(this);
	if (member != NULL && member->rest() != NULL) {
		if (member->rest()->first() == other) {
			return(TRUE); // called recursively so inductively true
		};
	};
	return(FALSE);
};

boolean Sprite::top_level_equal_to(Sprite *other, SpritesPointer &corresponding_nests, SpritesPointer &expected_corresponding_nests, 
											  EqualReason reason) { // new on 080601
// obsolete concept since can now find equals from contents of thought bubble to outside
//	boolean saved_tt_dump_so_equals_share = tt_dump_so_equals_share;
//	tt_dump_so_equals_share = FALSE;
//	boolean saved_tt_dump_so_equals_share_enabled = tt_dump_so_equals_share_enabled;
//	tt_skip_first_n_dumped = FALSE;
//	SpritePointer *saved_tt_sprites_being_dumped = tt_sprites_being_dumped;
//	tt_sprites_being_dumped = NULL;
//	int saved_tt_sprites_being_dumped_size = tt_sprites_being_dumped_size;
//	tt_sprites_being_dumped_size = 0;
//	int saved_tt_skip_first_n_dumped = tt_skip_first_n_dumped;
//	tt_skip_first_n_dumped = 0;
//	int saved_tt_skip_first_n_dumped = tt_skip_first_n_dumped; // rewritten on 110601
//	tt_skip_first_n_dumped = tt_number_of_sprites_dumped_so_far; // skip all the current ones
	boolean result = equal_to(other,NULL,corresponding_nests,expected_corresponding_nests,reason);
//	tt_skip_first_n_dumped = saved_tt_skip_first_n_dumped;
//	tt_dump_so_equals_share = saved_tt_dump_so_equals_share;
//	tt_dump_so_equals_share_enabled = saved_tt_dump_so_equals_share_enabled;
//	if (tt_sprites_being_dumped != NULL) {
//		delete [] tt_sprites_being_dumped;
//	};
//	tt_sprites_being_dumped = saved_tt_sprites_being_dumped;
//	tt_sprites_being_dumped_size = saved_tt_sprites_being_dumped_size;
//	tt_skip_first_n_dumped = saved_tt_skip_first_n_dumped;
	return(result);
};

boolean Sprite::equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, SpritesPointer &expected_corresponding_nests,
								 EqualReason reason) { // new on 050601
//	if (other == NULL) return(FALSE); -- caller should already have checked
	if (other == this) return(TRUE);
	if (pending != NULL) {
		if (check_pending_for_equals(other,pending)) return(TRUE);
	};
	return(they_dump_as_equals(this,other)); // default to old behavior -- obsolete? if not make it so
};

boolean equal(Sprite *x, Sprite *y, EqualReason reason) { // added reason as argument on 030202
	if (x == NULL) return(y == NULL);  // was FALSE
   if (y == NULL) return(FALSE);
	Sprites *corresponding_nests = NULL;
	Sprites *expected_corresponding_nests = NULL;
	boolean result = x->equal_to(y,NULL,corresponding_nests,expected_corresponding_nests,reason);
	if (result && expected_corresponding_nests != NULL) {
		if (corresponding_nests == NULL) {
			result = FALSE; // didn't find any but expect some
		} else {
			Sprites *remaining = expected_corresponding_nests;
			while (remaining != NULL) {
				Sprite *nest1 = remaining->first();
				remaining = remaining->rest();
				Sprite *nest2 = remaining->first();
				// this assumes that the nests are in the same order in both lists - bird.cpp needs to guarantee this
				if (corresponding_nests->successive_member_cons(nest1,nest2) != NULL) {
					// found an expected pair
				} else {
					result = FALSE;
#if TT_DEBUG_ON
					if (tt_debug_mode == 60601) {
						tt_error_file() << "Could NOT find the following in corresponding positions: ";
						nest1->print(tt_error_file());
						tt_error_file() << " and ";
						nest2->print(tt_error_file());
						tt_error_file() << endl;
					};
#endif
					break;
				};
				remaining = remaining->rest();
			};
		};
	};
	just_destroy_list(corresponding_nests);
	just_destroy_list(expected_corresponding_nests);
#if TT_DEBUG_ON
	if (tt_debug_mode == 80601 && x->kind_of() != NEST) {
		// compare with old method
		if (result != they_dump_as_equals(x,y)) {
			// this can happen if x and y have a remote looks to the same picture (e.g. x) and they will dump differently due to "partial context"
			log("Old and new ways of implementing equal disagree.");
		};
	};
#endif
	return(result);
};

boolean they_dump_as_equals(Sprite *x, Sprite *y) {
//	if (x == NULL) return(y == NULL);  // was FALSE -- commented out on 050601 since caller already checked this
// if (y == NULL) return(FALSE);
	SpriteType type_x = x->fine_kind_of();
	SpriteType type_y = y->fine_kind_of();
	if (type_x != type_y) return(FALSE);
	if (type_x == NEST && 
		 x->current_parameter() == NEST_INERT &&
		 y->current_parameter() == NEST_INERT) {
		// both unhatched
		return(TRUE);
	};
	// re-written on 060101 since didn't work for large objects (e.g. the adventure game)
//	string x_buffer = new character[10000];
//	string y_buffer = new character[10000];
//	output_string_stream x_encoding(x_buffer,10000);
//	output_string_stream y_encoding(y_buffer,10000);
	UnwindProtect<boolean> set(tt_dumping_to_test_equality_or_describe,TRUE);
	boolean x_infinite_stack = x->infinite_stack(); // new on 090400 to compare stacks and non-stacks
	boolean y_infinite_stack = y->infinite_stack();
	x->set_infinite_stack(FALSE);
	y->set_infinite_stack(FALSE);
//	dump_started();
//	x->dump(x_encoding);
//	dump_ended(); //release_sprites_being_dumped();
//	dump_started();
//	y->dump(y_encoding);
//   dump_ended(); //release_sprites_being_dumped();
	long x_length, y_length;
	string x_encoding = x->return_encoding(x_length,FALSE,FALSE);
	string y_encoding = y->return_encoding(y_length,FALSE,FALSE);
//#if TT_NEW_IO
//	boolean result = (x_encoding.str().length() == y_encoding.str().length());
//	if (result) {
//		result = (memcmp(x_buffer,y_buffer,x_encoding.str().length()*sizeof(character)) == 0);
//	};
//#else
//	boolean result = (x_encoding.tellp() == y_encoding.tellp()); // used pcount prior to 120201
//	if (result) {
//		result = (memcmp(x_buffer,y_buffer,x_encoding.tellp()) == 0);
//	};
//#endif
	boolean result = (x_length == y_length && memcmp(x_encoding,y_encoding,x_length) == 0); // new on 010601 -- added == 0 on 060601
#if TT_DEBUG_ON
	if (!result && tt_debug_mode == 80601) {
		x->print(tt_error_file());
		tt_error_file() << endl;
		tt_error_file().write(x_encoding,x_length);
		tt_error_file() << endl;
		y->print(tt_error_file());
		tt_error_file() << endl;
		tt_error_file().write(y_encoding,y_length);
		tt_error_file() << endl;
		int length = min(x_length,y_length);
		for (int i = 0; i < length; i++) {
			if (x_encoding[i] != y_encoding[i]) {
				tt_error_file() << "First difference at " << i << endl;
				break;
			};
		};
	};
#endif
	x->set_infinite_stack(x_infinite_stack);
	y->set_infinite_stack(y_infinite_stack);
	delete [] x_encoding;
	delete [] y_encoding;
	return(result);
};

//void Sprite::become(Sprite *other) { // new on 100901 // noticed obsolete on 140205
//	remove_all_followers(TRUE);
//	Sprites *remaining = other->pointer_to_followers();
//	while (remaining != NULL) {
//		Sprite *follower = remaining->first();
//		other->remove_follower(follower);
//		add_follower(follower);
//		remaining = remaining->rest();
//	};
//};

bytes Sprite::color_permutation(SelectionFeedback selection_feedback) { // new on 200901
	if (!active() || tt_city->stopped()) { // was tt_darken_all prior to 090103
		if (tt_color_selection_feedback && want_feedback(selection_feedback)) {
//			if (tt_current_time%200 < 100) { // every other is dark -- commented out on 101201
//				return(tt_dark_color_indices);
//			};
			int index = (tt_current_time/200)%14;
			if (index > 6) index = 13-index; // so it goes 0, 1, 2, 3, 4, 5, 6, 5, 4, 3, 2, 1, 0
			return(tt_dark_selection_color_permutations[index]); // using dark version beginning 101201
		} else {
			return(tt_dark_color_indices);
		};
   } else if (cause_of_match_failure()) { // new on 250302
		int index = (tt_current_time/200)%14; // change every 20th of a second -- new on 210902
		if (index > 6) index = 13-index; 
      return(tt_mismatch_color_indices[index]);
	} else if ((tt_color_selection_feedback && want_feedback(selection_feedback))) { 
		int index = (tt_current_time/100)%14; // change every 10th of a second 
		if (index > 6) index = 13-index; 
		return(tt_selection_color_permutations[index]);
	} else {
		return(NULL);
	};
};

void Sprite::set_saved_by(Sprite *by, boolean regardless) { // abstracted on 181102
   // beginning 191102 everyone should use this
#if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {
      log("set_saved_by of debug target.");
   };
#endif
	if (saved_by == by) return; // save some work - new on 041202
	if (saved_by != NULL) { 
		saved_by->destroy(this);
	};
	if (by != NULL && (regardless || by->kind_of() == ROBOT || by->kind_of() == ROBOT_IN_TRAINING)) { // regardless new arg on 050803
		// otherwise e.g. a hand - condition new on 170603 to not bother to store the programmer as a saved_by 
		// - also interferes with cities and time travel
		// ROBOT_IN_TRAINING condition added on 110803 - important for training robots to use remotes notebook
		saved_by = by;
		saved_by->increment_ref_count(this);
	} else {
		saved_by = NULL;
	};
};

#if TT_XML

//xml_element *Sprite::xml_create_element(xml_document *document) {
//   return(create_xml_element(xml_tag(),document));
//};

//boolean Sprite::ok_to_save_in_include_file() {
//	// orginally new on 180703 since small files slow things down
//	return(((followers != NULL && followers->rest() != NULL) || !no_flipped_followers())); 
//};

wide_string Sprite::xml_tag() {
   switch (appearance_code()) {
      case NOTEBOOK_PAGE_TURN_SPRITE:
         return(L"TurningNotebookPage"); // new on 181102
		case HAND_SPRITE:
			return(L"Hand");
		case ARM_ONLY_SPRITE:
			return(L"Arm");
		case HAND_HOLDING_WAND_SPRITE:
			return(L"HandHoldingWand");
		case TOOLBOX_SIDE_SPRITE:
			return(L"ToolboxViewedFromSide");
		case PERSON_WALKING_SPRITE:
			return(L"PersonWalking");
		case SIT_SPRITE:
			return(L"PersonSitting");
		case SIT_WITH_HAT_SPRITE:
			return(L"PersonSittingWithHat");
		case SIT_WITH_HAIR_SPRITE:
			return(L"PersonSittingWithHair");
		case DIZZY_PERSON_SPRITE:
			return(L"DizzyPerson");
		case DIZZY_PERSON_WITH_HAIR_SPRITE:
			return(L"DizzyPersonWithHair");
		case DIZZY_PERSON_WITH_HAT_SPRITE:
			return(L"DizzyPersonWithHat");
		case HELIOLND:
			return(L"LandingHelicopter");
		case HELIOFLY:
			return(L"FlyingHelicopter"); // was FlyingHelicoopter prior to 120703
		case EXPLOSION_SPRITE: // new on 030204
			return(L"Explosion");
      default:
         return(L"NotYetImplementedToonTalkObject");
   };
};

void Sprite::add_xml(xml_element *parent_element, wide_string tag, xml_document *document) { // boolean include_file_ok) { 
	// new abstraction on 011102 -- added include_file_ok on 110703
   xml_element *element = create_xml_element(tag,document);
   xml_create_and_append_element(element,document,TRUE)->Release();
	// prior to 131202 used to return the value from xml_create_and_append_element (the sprite element)
#if TT_DEBUG_ON
	if (tt_debug_mode == 2207041) {
		xml_debug_node(element);
	};
#endif
	xml_append_child(element,parent_element); // moved on 190804
//	return(element); // commented out on 200804
};

void Sprite::xml_to_stream(output_stream &stream) {
	// new abstraction on 170803
	xml_document *document = xml_create_document();
	::xml_to_stream(document,stream);
	xml_release_document(document);
};

xml_document *Sprite::xml_create_document(boolean include_explanation) {
   xml_document *document = xml_new_document(include_explanation);
	if (document == NULL) {
		return(NULL); // new on 140105 -- just in case
	};
	xml_element *toontalk_object = create_xml_element(L"ToonTalkObject",document); // OK since this is always an object - noted 210105
#if TT_LOADING_AND_SAVING_INTERRUPTABLE
	try { // try is new on 140105
		tt_sprites_saved_count = 0; // new on 150105
		top_level_xml(document,toontalk_object);
	} catch (int ) {
		return(NULL);
	};
#else
	top_level_xml(document,toontalk_object);
#endif
   xml_append_child(toontalk_object,document);
   return(document);
};

void Sprite::top_level_xml(xml_document *document, xml_element *parent) { // abstracted and improved on 190103
	// second arg prior to 200803 was boolean generate_toontalk_object but parent is more general
	// made generate_toontalk_object an arg on 110703 - default unchanged
//	xml_element *current_element = reference_to_existing_element(this,document); // new on 250203  - removed 250203
//	// to prevent loops - e.g. foreign bird's message
//	if (current_element != NULL) {
////		current_element->AddRef(); // unclear
//		return(current_element);
//	};
	UnwindProtect<Background *> set(tt_dumping_background,NULL); // new on 100703
	Element_Sprite_Table *old_table = push_current_element_sprite_table(); // new on 190103
	Numbered_Sprites *old_numbered_sprites = push_current_numbered_sprites(); // new on 190103
//	xml_element *element = NULL;
	//if (generate_toontalk_object) {
	//	element = create_xml_element(L"ToonTalkObject",document);
	//};
//	Sprite *throw_sprite = NULL;
//	boolean created_element = FALSE;
//	try { // new on 110703 since if this aborts it still needs to clean up - commented out on 150703
		boolean create_parent = (parent == NULL);
		if (create_parent) {
			parent = xml_create_and_append_element(NULL,document,FALSE,TRUE); 
			// top level with ToonTalkObject and object one level down
		} else {
			xml_create_and_append_element(parent,document,FALSE,TRUE)->Release();
		};
		//// rewritten 120703
		//if (element == NULL) {
		//	element = create_xml_element(xml_tag(),document);
		//	created_element = TRUE;
		//	xml(element,document); // this may throw if there is a dependency that matters for include files
		//	xml_element *top_element = create_xml_element(L"ToonTalkObject",document);
		//	xml_append_child(element,top_element);
		//	element = top_element;
		//} else {
		//	xml(element,document);
		//};
		do_postponed_nest_xml(document);
		//int max_serial_number = return_max_serial_number(); // obsolete as of 150703
		//if (max_serial_number > 0) {
		//	xml_set_attribute(element,L"MaxSerialNumber",max_serial_number);
		//};
		xml_set_attribute(parent,L"Language",tt_language); // not likely to change language or version number during time travel...
		xml_set_attribute(parent,L"Version",xml_version_number); // new on 180103
		if (tt_saved_city_file_name_when_demo_ends == NULL) { 
			// new on 290505 so file compare (fc) doesn't complain about this difference
			xml_set_attribute(parent,L"CreatedUsing",C(IDS_TOONTALK_VERSION_NUMBER)); // new on 050903
		};
		xml_set_attribute(parent,L"xmlns",L"http://www.toontalk.com");
		if (create_parent) {
#if TT_DEBUG_ON
			int ref_count = // for debugging XML ref counts
#endif
			parent->Release(); // new on 200804
#if TT_DEBUG_ON
			debug_ref_count(ref_count,parent);
#endif
		};
	//} catch (Sprite *sprite) {
	//	throw_sprite = sprite;
	//	if (created_element) {
	//		element->Release();
	//	};
	//};
//   reset_element_sprite_table(); // new on 120802 -- moved here on 220802
	restore_numbered_sprites(old_numbered_sprites);
	restore_current_element_sprite_table(old_table,document);
   restore_saved_nest_guids(); // new on 181002
	//if (throw_sprite) {
	//	throw(throw_sprite); // throw again now that restored things
	//};
#if TT_DEBUG_ON
   if (tt_debug_mode == 280802) {
      xml_debug_node(document,L"top_level_xml");
   };
#endif
//	return(parent); // commented out on 200804
};

boolean Sprite::might_dump_as_shared_or_equal() { // new on 070802
   return (tt_dumping_with_sharing_of_all_types || // and isn't a kind of thing that always needs to deal with sharing
      // commented out the following on 070802 since equality and describe will work differently soon
//		     !(return_ref_count() < 2 && !tt_dumping_to_test_equality_or_describe) || // new condition as of 180100 - !tt_dumping_to_test_equality_or_describe added on 120201 since equal was broken
		     kind_of() == NEST || kind_of() == PROGRAM_PAD || kind_of() == PICTURE || kind_of() == ROBOT || kind_of() == CUBBY);
   // added CUBBY disjunct on 071002 since can be pointed to by robot and by back of picture for example
};

void Sprite::xml_create_external_link_and_append_element(xml_element *parent, xml_document *document, wide_string tag) {
	xml_element *element = create_xml_element(xml_tag(),document);
	wide_character guid_string[guid_string_size]; 
	if (guid == NULL) { // new on 310803
		add_to_sprites_with_newly_minted_guids(this);
	};
	guid_as_hexadecimal(create_guid(),(wide_string) guid_string); // so even if picture didn't have a GUID this works -- shouldn't need this anymore - guid created above (310803)
	xml_set_attribute(element,L"ExternalLink",(wide_string) guid_string); // currently ExternalLink differs from Link in that it is OK to fail (someday may work if referree is loaded after this guy)
	if (tag != NULL) { // new on 050104
		xml_element *wrapped_element = create_xml_element(tag,document);
		xml_append_child(element,wrapped_element);
		element = wrapped_element;
	};
	if (parent != NULL) {
		xml_append_child(element,parent);
	};
//	return(element); // commented out on 200804
};

xml_element *Sprite::xml_create_and_append_element(xml_node *parent, xml_document *document, boolean might_be_shared, 
																	boolean top_level) { 
	// beginning 200804 it is callers responsible to ->Release() return value
	// added top_level on 280803
	// new on 070802
   xml_element *element = NULL;
	// commented out on 150703 since with new GUID scheme shouldn't be need (or right since connections lost)
	//if (tt_dump_reason == DUMPING_TO_CLIPBOARD && !might_dump_as_shared_or_equal()) {
 //     // added tt_dumping_to_clipboard on 131102 since if dumping city or floor might be sharing due to animations
 //     element = create_xml_element(xml_tag(),document);
 //     xml(element,document); // why doesn't this do reference_to_existing_element and associate_element_and_sprite?? asked 120703
	//	//if (parent != NULL) { // conditional new on 220203 - done just below
	//	//	xml_append_child(element,parent);
	//	//};
 // //    return(element);
	//} else {
		if (might_be_shared) { // condition new on 10703 since programmers arm and the like were a waste to go thru this
			element = reference_to_existing_element(this,document);
		};
		//if (!include_file_ok) { // new on 120703
		//	ultimate_leader_not_saved_as_include_file();
		//};
		// following -- obsolete 160204
//		if (element == NULL && leader == NULL && tt_dump_reason == DUMPING_CITY && might_be_shared && // restored leader == NULL on 170703
//#if TT_DEBUG_ON
//			 tt_debug_mode != 80703 &&
//#endif			
//			 time_travel_enabled() && tt_log_count_max < 0 && ok_to_save_in_include_file() && ok_to_cache()) { // restored and upgraded ok_to_save_in_include_file() on 190703
//			// new on 280803 that if tt_log_count_max is "finite" then don't generate any include files (since broken by deletions of old logs)
//			// kind of approximate - 2 followers min or some flipped followers
//			// on 150703 removed recent_xml_location >= 0  && include_file_ok
//			// note that tt_dump_reason == DUMPING_CITY prevents the followers of this guy from generating an include file
//			// new on 080703
//			// new policy on 090703 that only generate "include" files for objects without containers
//			// if already have an up-to-date file then just use a reference to it
//			// otherwise dump and use reference
//			// to enable sharing checked for references first
//			// don't create an include link if this has parts that might be pointed to (e.g. home_notebook or controlee)
//			// added might_be_shared since if it is special - e.g. the programmer's arm then it shouldn't be dumped here
//			// ok_to_save_in_include_file new on 120703 since don't want birds or remote controls to do this
//			// beginning 120703 the processing is postponed until everyone else has dumped
//			element = create_xml_element(xml_tag(),document);
//			if (parent != NULL) { // conditional new on 220203
//				xml_append_child(element,parent);
//			};
//			boolean needs_to_generate_include_file = (recent_xml_location == 0);
//			// following moved here on 180703 so the association happens before calling generate_include_file which may look up this GUID  
//			if (!needs_to_generate_include_file) {
//				// referring to an old include file - need to be prepared to update it if its parts need GUIDs
//				associate_element_and_include_location(element,recent_xml_location);
//			};
//			associate_element_and_sprite(element,this); // new on 160703 so subsequent references use the GUID (then do I need to maintain included_sprites?? - asked 160703)
//			if (needs_to_generate_include_file) {
//				generate_include_file();
//			};
//			if (recent_xml_location > 0) {
//				xml_set_attribute(element,L"Include",recent_xml_location);
//#if TT_DEBUG_ON
//				if (tt_debug_mode == 160703 && kind_of() == CUBBY) {
//					tt_error_file() << "Generating reference to include file " << recent_xml_location << " for cubby#" << debug_counter 
//								   << " where needs_to_generate_include_file is " << (int) needs_to_generate_include_file << endl;
//				};
//				//if (fine_kind_of()== TOOL_BUTTON) {
//				//	log("debug this");
//				//};
//#endif
			//} else {
				// generate it as normal - probably never happens anymore
//				associate_element_and_sprite(element,this);
//				xml(element,document); // fill it in (possibly with recursive calls to this method - OK now that element is associated with sprite)
//			}; 
//			// experiment on 120703 to not postpone since it interferes with "SameAs" links since save and loads happen in different internal order
////			postpone_include_file_xml(this,element);
//			return(element);
		//};
//	};
	if (element != NULL) {
//		if (parent != NULL) { // conditional new on 220203 -- xml_append_child now checks - new on 280803
#if TT_DEBUG_ON
		int ref_count = // for debugging XML ref counts
#endif
		element->AddRef(); // new on 200804
#if TT_DEBUG_ON
		debug_ref_count(ref_count,element);
#endif
		xml_append_child(element,parent);
		return(element); // don't do the following
	};
	VisibleNotebookPage notebook_page = is_visible_notebook_page(); 
	// new on 280803 to generate special XML for left and right page of notebooks
	if (notebook_page == NOT_NOTEBOOK_PAGE || top_level) { 
		// if top_level (e.g. a notebook generating XML for a page) then do the normal thing
		element = create_xml_element(xml_tag(),document); 
		// prior to 120703 was xml_create_element(document); // let the object generate the node
		associate_element_and_sprite(element,this);
		xml(element,document); // fill it in (possibly with recursive calls to this method - 
		// OK now that element is associated with sprite)
		if (parent != NULL) { // conditional new on 220203 -- moved on 190804
#if TT_DEBUG_ON
			int ref_count = // for debugging XML ref counts
#endif
			element->AddRef(); // new on 200804
#if TT_DEBUG_ON
			debug_ref_count(ref_count,element);
#endif
			xml_append_child(element,parent);
		};
	} else {
		if (notebook_page == IS_LEFT_PAGE) {
			element = create_xml_element(L"LeftPage",document); 
		} else { // must be IS_RIGHT_PAGE
			element = create_xml_element(L"RightPage",document);
		};
		leader->xml_create_and_append_element(element,document,TRUE)->Release();
#if TT_DEBUG_ON
		int ref_count = // for debugging XML ref counts
#endif		
		element->AddRef(); // new on 200804
#if TT_DEBUG_ON
		debug_ref_count(ref_count,element);
#endif
		xml_append_child(element,parent); // moved on 190804
	};
#if TT_DEBUG_ON
	debug_ref_count(-1,element); // should be able to be decremented -- typically directly by caller
#endif
	return(element);
};

//void Sprite::generate_include_file() { -- obsolete 160204
//	UnwindProtect<DumpReason> set(tt_dump_reason,DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL);
////	tt_part_of_city_being_dumped_for_time_travel = this;
//	xml_document *new_document = create_xml_document(); // released when postponed include is generated
//	//try { // not used beginning 150703
//	//	following throws if it finds this isn't independent
//	//	element_to_dump = top_level_xml(new_document,FALSE); // FALSE makes this the top level - not ToonTalkObject
//	// beginning 150703 no longer needs to be top-level
//	xml_element *element_to_dump = create_xml_element(xml_tag(),new_document); // in a different document
//	associate_element_and_sprite(element_to_dump,this); // so it gets its GUID set if it is needed -- moved here on 180703 in case :xml looks for this
//	xml(element_to_dump,new_document); // fill it in 
//	//} catch (Sprite *) {
//	//	set_recent_xml_location(-1); // don't bother checking again (until this is reset - e.g. Sprite::add_follower or Sprite::set_leader)
//	//};					
////	tt_part_of_city_being_dumped_for_time_travel = NULL;
////	if (recent_xml_location == 0) { // didn't abort above 
////#if TT_DEBUG_ON
////	if (tt_debug_mode == 160703) {
////		tt_error_file() << "just created XML for what will be include#" << tt_log_out_archive_size << endl;
////	};
////#endif
//	set_recent_xml_location(tt_log_out_archive_size++); // moved this after xml_create_and_append_element since that may save files too (e.g. followers)
//	xml_set_attribute(element_to_dump,L"xmlns",L"http://www.toontalk.com"); // seems to be needed to load it back in
////	dump_xml_include_file(element,recent_xml_location); 
//// postpones this until dumping the city as well (so all cross references have been computed) 150703
//	postpone_include_file_xml(recent_xml_location,element_to_dump);
//};

VisibleNotebookPage Sprite::is_visible_notebook_page() { // new on 280803 used to save XML of left_page and right_page of notebooks
	if (leader == NULL || leader->kind_of() != PROGRAM_PAD) {
		return(NOT_NOTEBOOK_PAGE);
	};
	Notebook *notebook = (Notebook *) leader;
	if (notebook->pointer_to_left_page() == this) {
		return(IS_LEFT_PAGE);
	};
		if (notebook->pointer_to_right_page() == this) {
		return(IS_RIGHT_PAGE);
	};
	return(NOT_NOTEBOOK_PAGE);
};

//void Sprite::check_for_independence() { // obsolete as of 150703
//	if (!part_of(tt_part_of_city_being_dumped_for_time_travel)) {
//		throw(this);
//	};
//	ultimate_leader_not_saved_as_include_file(); // new on 120703 so this won't become an include file
//};

Sprites *sprites_to_set_active = NULL; // new on 180103

#if TT_DEBUG_ON
boolean setting_sprites_to_active_now = FALSE;
#endif

void set_sprites_to_active_now() {
	if (tt_loading_vacuum_contents && tt_log_version_number >= 68) return; // new on 170105
	// the idea is to postpone this until everything is in place 
	// -- e.g. position sensors shouldn't be activated until everyone is where they are going to be
	if (sprites_to_set_active != NULL) {
		Sprites *remaining = sprites_to_set_active;
#if TT_DEBUG_ON
		UnwindProtect <boolean> set(setting_sprites_to_active_now,TRUE);
#endif
		while (remaining != NULL) {
			Sprite *sprite = remaining->first();
			if (sprite != NULL) { // new condition for robustness on 250503
				if (sprite->prepared_for_deletion()) { // new on 240203
					// warn? or is it just wasteful?
//#if TT_DEBUG_ON
					// might track this down someday but until then it just fills up logs
//					tt_error_file() << "Sprite #" << sprite->debug_counter 
					//<< " destroyed while loading - maybe just wasteful or a symptom of something more serious." << endl;
//#endif
				} else if (sprite->is_blank() && sprite->kind_of() == PICTURE && tt_log_version_number >= 68) { 
					// new on 170105 -- do nothing if blank -- added PICTURE test on 130205 since birds and number pads etc should be active
				} else {
					//Sprite *leader = sprite->pointer_to_leader(); // experiment on 030603
					//if (leader != NULL) {
					//	leader->really_propagate_changes(); //_to_follower(sprite); -- WHY DOESN'T THIS WORK??????
					//};
//					sprite->Sprite::set_active(TRUE,FALSE); // experiment 040603 since otherwise if a Picture this does too much work
					sprite->set_active(TRUE,FALSE,TRUE); // added new arg on 050604 so receiver knows this is from an initialization
					// and also updates the position prematurely (leader_offsets not propogated)
				};
				remaining->set_first(NULL); // commented out on 130603 since now keeps ref counts - restored as an experiment - memory leak??
			};
			remaining = remaining->rest();
		};
		delete sprites_to_set_active;
		sprites_to_set_active = NULL;
	};
};

void add_to_active_sprites(Sprite *sprite) { // abstracted as a procedure on 040603
#if TT_DEBUG_ON
	if (setting_sprites_to_active_now) {
		log("add_to_active_sprites called while setting_sprites_to_active_now");
	};
#endif
//	sprite->increment_ref_count(); // experiment on 130603
	sprites_to_set_active = new Sprites(sprite,sprites_to_set_active); // new on 180103
};

void remove_from_active_sprites(Sprite *sprite) { // new on 110603
	if (sprites_to_set_active == NULL) return;
//	sprite->destroy(); // experiment on 130603
	sprites_to_set_active = sprites_to_set_active->remove(sprite);
};

Sprites *return_sprites_to_set_active() {
	return(sprites_to_set_active);
};

void set_sprites_to_set_active(Sprites *new_sprites_to_set_active) {
	sprites_to_set_active = new_sprites_to_set_active; // caller should know what they are doing here
};

boolean Sprite::handle_xml_tag(Tag tag, xml_node *node) {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log("handle_xml_tag of debug target");
	};
#endif
   switch(tag) {
		//case INCLUDE_TAG: 
		//	handle_include_tag(node,this); // new on 080703 -- problem since this is at the wrong place - e.g. button of a wand
		//	return(TRUE);
      case GEOMETRY_TAG: {
#if TT_DEBUG_ON
			DWORD start;
			if (tt_debug_mode == 310303) {
				start = timeGetTime();
			};
#endif
         xml_load_geometry(this,node); // this could be rationalized (e.g. using xml_process_children)
#if TT_DEBUG_ON
			if (tt_debug_mode == 310303) {
				tt_error_file() << (timeGetTime()-start) << " ms to xml_load_geometry in Sprite::handle_xml_tag" << endl;
			};
#endif
         int p = xml_get_attribute_int(node,L"Z",current_priority); // new on 161102
         if (current_priority != p) {
            set_priority(p);
	         just_set_floor_priority(p);
         };
#if TT_DEBUG_ON
			if (tt_debug_mode == 310303) {
				tt_error_file() << (timeGetTime()-start) << " ms to process GEOMETRY_TAG in Sprite::handle_xml_tag" << endl;
			};
#endif
                         };
         return(TRUE);
		case GEOMETRIC_RELATION_TO_CONTAINER_TAG: 
			// new on 101202
			if (leader_offsets == NULL) {
				leader_offsets = new Leader_Offsets();
			};
			return(leader_offsets->handle_xml_tag(NO_MORE_TAGS,node));
      case INFINITE_STACK_TAG:
         set_infinite_stack(TRUE);
         return(TRUE);
      case INVISIBLE_TAG:
         set_visible(FALSE);
         return(TRUE);
      case SEE_ALL_TAG:
         set_show_all(TRUE);
         return(TRUE);
      case SEE_SOME_TAG:
         set_show_all(FALSE);
         return(TRUE);
      case ERASED_TAG:
         become_blank();
         return(TRUE);
      case ACTIVE_TAG:
			add_to_active_sprites(this);
 //        set_active(TRUE,FALSE);
         return(TRUE);
      case GLUED_DOWN_TAG:
         set_graspable(FALSE);
         return(TRUE);
      case TOP_OF_STACK_TAG: // new on 161102
			if (top_of_stack != NULL) { // new on 070103
				top_of_stack->destroy();
			};
         top_of_stack = xml_get_sprite(node);
         return(TRUE);
		case NOTIFY_WHEN_DONE_TAG:
			set_saved_by(xml_get_sprite(node)); // new on 181202
			return(TRUE);
      case TOONTALK_COMMENT_TAG:
         set_comment(xml_get_sprite(node));
         return(TRUE);
      case DO_AFTER_TAG:
      case ANIMATE_TO_STATIC_GOAL_TAG:
      case ANIMATE_TO_DYNAMIC_GOAL_TAG:
         if (xml_get_attribute_int(node,L"New",0)) {
            new_animations = xml_load_animation(tag);
				new_animations->set_owner(this); // new on 140104 since don't want to reference count notify if identical to owner
            return(new_animations->handle_xml_tag(NO_MORE_TAGS,node));
         } else {
            animations = xml_load_animation(tag);
				animations->set_owner(this); // new on 140104 since don't want to reference count notify if identical to owner
//            return(animations->handle_xml_tag(NO_MORE_TAGS,node));
            return(xml_entity(node,animations));
         };      
      case STOPPED_TAG: // obsolete - replaced with ACTIVE_TAG
         return(TRUE);
		case VOLATILE_ATTRIBUTES_TAG:
      case SPECIAL_ATTRIBUTES_TAG: // old obsolete name
#if TT_DEBUG_ON
			{
			DWORD start;
			if (tt_debug_mode == 310303) {
				start = timeGetTime();
			};
			boolean result = xml_get_special_attributes(node);
#if TT_DEBUG_ON
			if (tt_debug_mode == 310303) {
				tt_error_file() << (timeGetTime()-start) << " ms to process VOLATILE_ATTRIBUTES_TAG in Sprite::handle_xml_tag" << endl;
			};
			return(result);
			};
#endif
#else
         return(xml_get_special_attributes(node)); // new on 111102
#endif
      case NO_MORE_TAGS:
#if TT_DEBUG_ON
			DWORD start;
			if (tt_debug_mode == 310303) {
				start = timeGetTime();
			};
#endif
			if (code != SCALE_SPRITE) { // they handle parameters specially - new on 270103
				just_set_parameter(xml_get_attribute_int(node,L"Parameter",default_parameter())); 
				// prior to 181102 was set_parameter but that did too much (like reset cycle)
			};
#if TT_DEBUG_ON
			if (parameter < 0) {
				log("Loaded strange XML setting for parameter. Resetting to 0.",FALSE,TRUE);
				parameter = 0;
			};
#endif
			current_index = xml_get_attribute_int(node,L"Index",0); 
			// prior to 210404 was current_index // moved here from xml_get_special_attributes on 260103
			if (code != SYNTHETIC_SPRITE && code != USER_SPRITE && (!is_picture() || kind_of() == PICTURE)) { 
				// conditional new on 201202
				// also if is non-picture indirection then parameter may be color and not used for image cycles
				update_cycle_duration(); // new on 181202
//				if (cycle_stopped(current_index)) { // seems always a good idea - right?
				update_display(UPDATE_INITIALIZE); 
				// new on 060204 since otherwise the following may be based upon an old current_index and a new parameter
				// added arg above on 150204 since otherwise priorities get reset strangely
				current_image = image_cycles[code][parameter].nth_image(current_index); 
				// new on 030204 -- needed for Tooly to decide if he's "settled down"
//				};
				// should this be even more general? -- how bad is it to call update_display here? -- 
				//probably removing the conditional is enough 030204
			};
#if TT_DEBUG_ON
			if (tt_debug_mode == 310303) {
				tt_error_file() << (timeGetTime()-start) << " ms to process NO_MORE_TAGS in Sprite::handle_xml_tag" << endl;
			};
#endif
         return(TRUE);
		case APPEARANCE_TAG:
			return(TRUE); // should have been handled by PICTURE_OF_TAG or the like - new on 201202
      default:
         // new on 311002 to capture elements not recognized by this version of ToonTalk and maintain them
         int new_extra_xml_count = extra_xml_count+1;
         xml_node_pointer *new_extra_xml = new xml_node_pointer[new_extra_xml_count];
         for (int i = 0; i < extra_xml_count; i++) {
            new_extra_xml[i] = extra_xml[i];
         };
         new_extra_xml[extra_xml_count] = node;
         node->AddRef();
#if TT_DEBUG_ON
			static int problem_count = 0;
			problem_count++;
			if (problem_count < 4) {
				xml_debug_node(node,L"no handle_xml_tag case");
			};
#endif
//         new_extra_xml[extra_xml_count] = xml_clone_node(node); // more costly than AddRef so rewritten
         // following is no better than AddRef
         //
         //xml_node *parent;
         //node->get_parentNode(&parent);
         //xml_node *ignore;
         //parent->removeChild(node,&ignore);
         if (extra_xml != NULL) delete [] extra_xml;
         extra_xml = new_extra_xml;
         extra_xml_count = new_extra_xml_count;
         //if (extra_xml == NULL) {
         //   extra_xml = xml_get_text_copy(element);
         //} else {
         //   wide_string new_extra_xml = append_strings(extra_xml,xml_get_text(element));
         //   delete [] extra_xml;
         //   extra_xml = new_extra_xml;
         //};
         return(TRUE); 
   };
};

void Sprite::set_extra_xml(xml_node_pointer *initial_extra_xml, int initial_extra_xml_count) { // new on 311002
   // could avoid this consing if had reference counting of extra_xml itself
   extra_xml = new xml_node_pointer[initial_extra_xml_count];
   for (int i = 0; i < initial_extra_xml_count; i++) {
      extra_xml[i] = initial_extra_xml[i];
      extra_xml[i]->AddRef();
   };
   extra_xml_count = initial_extra_xml_count;
};

xml_element *Sprite::xml_geometry(xml_document *document) {
	if (leader != NULL && leader_offsets != NULL) { // leader_offsets aren't reset when becomes leaderless
		// new on 101202 to get much greater accuracy
		xml_element *geometry = create_xml_element(L"GeometricRelationToContainer",document);
#if TT_DEBUG_ON
		if (tt_debug_mode == 240603) {
			tt_error_file() << "Created GeometricRelationToContainer" << endl;
		};
#endif
		leader_offsets->xml_geometry(geometry);
#if TT_DEBUG_ON
		if (tt_debug_mode == 240603) {
			tt_error_file() << "after leader_offsets->xml_geometry" << endl;
		};
#endif
		return(geometry); // everything handled
	};
	xml_element *geometry = create_xml_element(L"Geometry",document);
	if (!held() || !dumping_an_individual()) { // condition new on 140802 since don't need position if being put on the clipboard
		// added !dumping_an_individual() since need this if dumping a city
		// commented out the conditional on 150802 because throws off geometry when followers are added
		// retstored on 101202 since followers are handled differently now
		xml_append_child(create_xml_element(L"Left",document,llx),geometry);
		xml_append_child(create_xml_element(L"Bottom",document,lly),geometry);
   };
   xml_append_child(create_xml_element(L"Width",document,width),geometry);
   xml_append_child(create_xml_element(L"Height",document,height),geometry);
   return(geometry);
};

//xml_element *Sprite::xml_flags(xml_document *document) {
//   if (!infinite_stack() && visible() && show_all() && !is_blank()) return(NULL); // all at default values - new on 050802
//   xml_element *flags = create_xml_element(L"SpecialSettings",document);
//   if (infinite_stack()) {
//      xml_append_child(create_xml_element(L"InfiniteStack",document),flags);
//   };
//   if (!visible()) {
//      xml_append_child(create_xml_element(L"Invisible",document),flags);
//   };
//   if (!show_all()) {
//      xml_append_child(create_xml_element(L"OnlyPartlyVisible",document),flags);
//   };
//   if (is_blank()) {
//      xml_append_child(create_xml_element(L"Erased",document),flags);
//   };
//   return(flags);
//};

//xml_element *Sprite::xml_index(int dump_index, xml_document *document) {
//   // move since doesn't have anything to do with Sprite??
//   if (dump_index == max_long) return(NULL); // not a real index
//   return(create_xml_element(L"SerialNumber",document,dump_index));
//};

boolean Sprite::geometry_worth_saving() {
   // updated on 161002 since compound pictures in notebooks or boxes weren't saving correctly 
	// as well as compound pictures on the back of pictures
//   return(leader != NULL || floor != NULL || kind_of() == PICTURE);
	// added completely_inside_leader on 131202 since geometry is important if for example a smash is about to happen so has leader
	// changed to TRUE on 250103 (move to sprite.h) since the alternative of figuring out how something fits 
	// doesn't work well when things are very tiny (e.g. boxes on the back of pictures)
	return(leader == NULL || !completely_inside_leader() || leader->kind_of() == THOUGHT_BUBBLE || leader->kind_of() == ROBOT);
		// updated on 260803 by changing (leader->kind_of() == THOUGHT_BUBBLE && leader->pointer_to_leader() == NULL)
		// important for exact time travel -- ROBOT as well since teams otherwise aren't reproduced exactly
	// commented out this && floor != NULL on 240103 since make thought bubbles attach badly to robots (and probably more)
	// restored on 270103 since wasteful to do this for non-pictures inside of boxes - 
	// Picture::geometry_worth_saving - updated accordingly
	// re-written on 101202 - Picture stuff moved to Picture::geometry_worth_saving()
	// added leader->kind_of() == THOUGHT_BUBBLE on 130703 since if saving while expanding thought bubble for training is needed
};

void Sprite::xml(xml_element *element, xml_document *document) {
//#if TT_DEBUG_ON
//	static int count = 0;
//	count++;
//	tt_error_file() << count << endl;
//#endif
#if TT_LOADING_AND_SAVING_INTERRUPTABLE
	if (user_wants_to_interrupt_also_process_messages_now(replaying() || time_travel_enabled() || 
		 tt_dump_reason == DUMPING_TO_ENABLE_UNBAMMING)) { 
		// new on 140105
		// don't allow interrupt of saving of state for unbamming (or as of 160105 when replaying a demo)
		ask_response_to_save_interruption();
	};
	tt_sprites_saved_count++;
#endif
	if (guid != NULL && tt_saved_city_file_name_when_demo_ends == NULL) { // new on 160703
		wide_character guid_string[guid_string_size]; 
		guid_as_hexadecimal(guid,(wide_string) guid_string);
		xml_set_attribute(element,L"GUID",(wide_string) guid_string);
	};
   if (geometry_worth_saving()) { 
      // what if this is an indirection?
      xml_element *geometry_element = xml_geometry(document);
		if (floor != NULL && floor->floor_or_rule_body()) { 
			// prior to 070103 was on_the_floor() but that did the wrong thing in a thought bubble
		   // if haven't stood up since this was created or priority changed then this is the more up-to-date value
         xml_set_attribute(geometry_element,L"Z",current_priority); // new on 161102
//#if TT_DEBUG_ON
//			if (current_priority < -4 && current_priority >= min_long+2 && kind_of() != BIRD) {
//				log("debug this");
//			};
//#endif
	   } else {
         xml_set_attribute(geometry_element,L"Z",floor_priority); // new on 161102
//#if TT_DEBUG_ON
//			if (floor_priority < -4 && floor_priority >= min_long+2 && kind_of() != BIRD) {
//				log("debug this");
//			};
//#endif
	   };
      xml_append_child(geometry_element,element); // moved here on 190804 so this is done after element is completed
	};
   if (!graspable()) {
      xml_append_child(create_xml_element(L"GluedDown",document),element);
   };
   if (active()) {
      // could try to be more clever here but its tricky since if parent is inactive it would have to spread
      // inactive when follower is added, etc.
//      xml_append_child(create_xml_element(L"Stopped",document),element);
      xml_append_child(create_xml_element(L"Active",document),element); // re-written on 081002
   };
   if (infinite_stack() && !in_notebook()) { 
		// added in_notebook test on 230802 since elements of a notebook are always infinite stacks
      xml_append_child(create_xml_element(L"InfiniteStack",document),element);
   };
   if (!visible()) {
      xml_append_child(create_xml_element(L"Invisible",document),element);
   };
   if (show_all() != show_all_default() || tt_dump_reason == DUMPING_TO_ENABLE_UNBAMMING) {
		// DUMPING_TO_ENABLE_UNBAMMING added on 111202 since may be clobbered and need to know this
		// does this really make sense? Shouldn't show_all_default really always be TRUE?
      if (show_all()) {
         xml_append_child(create_xml_element(L"SeeAll",document),element);
      } else {
         xml_append_child(create_xml_element(L"SeeSome",document),element);
      };
   };
   if (is_blank()) {
      xml_append_child(create_xml_element(L"Erased",document),element);
   };
   if (comment != NULL) {
      xml_append_child(create_xml_element(L"ToonTalkComment",document,comment),element);
   };
	if (parameter != default_parameter()) { // changed on 250103 to only do this if not default_parameter
		xml_set_attribute(element,L"Parameter",parameter); // was part of xml_set_special_attributes prior to 261102
	};
	if (current_index != 0) xml_set_attribute(element,L"Index",current_index); 
	// copied here from xml_set_special_attributes on 260103
   if (tt_dump_reason != DUMPING_TO_CLIPBOARD && tt_dump_reason != DUMPING_TO_NOTEBOOK_PAGE) { 
		// condition duplicated in picture::xml on 171004
      // if going to the clipboard don't need this stuff and it may be confusing as well
      // added tt_dumping_to_notebook_page on 211102 -- 
		// cities, rooms, houses, floors do need this stuff (but not for example notebook pages within a city)
      xml_append_child(xml_set_special_attributes(element,document),element);
		add_special_xml(element,document);
   };
   for (int i = 0; i < extra_xml_count; i++) {
      xml_append_child(extra_xml[i],element);
   };
};

void Sprite::add_special_xml(xml_element *element, xml_document *document) { // abstracted on 261102
   if (animations != NULL && !running_animations) { 
		// added && !running_animations on 240103 since shouldn't save animations that are currently running
      animations->add_xml_each(element,document);
   };
   if (new_animations != NULL)  {
      // these are animations created while animating the animations
      new_animations->add_xml_each(element,document);
      xml_set_attribute(element,L"New",1);
   };
   if (top_of_stack != NULL) { // new on 161102
      top_of_stack->add_xml(element,L"TopOfStack",document);
   };
	if (saved_by != NULL) { // new on 181202
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	saved_by->check_for_independence();
		//};
		saved_by->add_xml(element,L"NotifyWhenDone",document);
	};
};

xml_element *Sprite::xml_set_special_attributes(xml_element *element, xml_document *document) {
   // updated on 051102 so these aren't saved if already are default value
   // these are important when saving a city and not very important otherwise
   // checked that this are all the needed attributes on 161102
   // beginning 211102 this puts special attributes on its own element so no need to look for these attributes if this isn't called
   xml_element *special_element = create_xml_element(L"VolatileAttributes",document); // was SpecialAttributes prior to 181202
 //  xml_append_child(special_element,element); // done by caller now (261102)
	if (cycle_start_time > 0) { // commented out since interferes with perfect logs && !currently_still_picture(code,parameter)) {
      xml_set_attribute(special_element,L"CycleStartTime",cycle_start_time) ; // was tt_current_time-cycle_start_time); // remember it relative to "now"
   };
   if (old_x_offset != 0) xml_set_attribute(special_element,L"OldXOffset",old_x_offset);
   if (old_y_offset != 0) xml_set_attribute(special_element,L"OldYOffset",old_y_offset);
   if (looped_back_already) {
      xml_set_attribute(special_element,L"LoopedBackAlready",1);
   };
   if (single_step) {
      xml_set_attribute(special_element,L"SingleStep",1);
   };
   if (!available_for_indexing_flag) { // usually TRUE
      xml_set_attribute(special_element,L"AvailableForIndexing",0);
   };
   if (!containable_by_background_flag) {
      xml_set_attribute(special_element,L"NotContainableByBackground",1);
   };
   if (!selectable_flag) {
      xml_set_attribute(special_element,L"NotSelectable",1);
   };
   if (!move_between_floor_and_room_flag) {
      xml_set_attribute(special_element,L"NotMoveableBetweenFloorAndRoom",1);
   };
   if (cause_of_match_failure_flag) { // new on 161102
      xml_set_attribute(special_element,L"CauseOfMatchFailure",1);
   };
   if (is_top_of_a_stack_flag) { // new on 161102
      xml_set_attribute(special_element,L"IsTopOfAStack",1);
   };
   if (to_be_smashed_flag) { // new on 161102
      xml_set_attribute(special_element,L"ToBeSmashed",1);
   };
   if (cycle_stopped_callback != CALLBACK_NOTHING) { // new on 151102
      xml_set_attribute(special_element,L"CallbackIndex",(int) cycle_stopped_callback);
   };
	if (priority_fixed_flag) { // new on 100103
		xml_set_attribute(special_element,L"ZFixed",1);
   };
	if (ideal_speed > 0) { 
		// new on 200404 since walking of Tooly needs this for demos to work right (since changes number of calls to random)
		xml_set_attribute(special_element,L"IdealSpeed",ideal_speed);
	};
	return(special_element);
   // not saving adjust_height_if_camera_above_flag since currently only used for time-travel buttons
   // ref_count is special - should be OK not to dump
   // finishing_instantly_flag, animation_pending_callbacks_only, finish_animation_pending 
	// only used within a call to finish_instantly(), right?
   // running_animations only set by run_animations
   // ok_to_dump_flag should always be TRUE if it is dumped
   // never_cache_flag isn't dumped - only used for some optimizations with sensors and appearance 
	// so should be recreated anyway, right?
   // clean_run, clean_run_average, display_cost not saved since only used for performance 
	// (and city might be loaded into a different machine, etc)
   // is_picture_flag, followers_absorbed_flag, completely_inside_leader_flag, screen_updates_flag 
	// not dumped since relationships should recreate it
   // held_status, in_notebook_flag, leader_offsets, inside_vacuum_flag not dumped since should be recreated
   // x_scale, y_scale recreated
   // true_size_flag seems based upon relationships
   // changes_propagated_flag seems only to be used for error checking now -- move it to TT_DEBUG_ON??
   // I think size_changed_flag is only used to avoid extra work
   // clean_status and cache_status reset on every frame, right?
   // current_scale maintained by context, right
   // ideal_speed maintained by programmer, OK?
};

boolean Sprite::xml_get_special_attributes(xml_node *node) { // new on 111102
   cycle_start_time = xml_get_attribute_int(node,L"CycleStartTime",0); // prior to 210404 was cycle_start_time
   //if (cycle_start_time_delta != max_long) {
   //   cycle_start_time = tt_current_time-cycle_start_time_delta;
   //};
   old_x_offset = xml_get_attribute_int(node,L"OldXOffset",0); // prior to 210404 was old_x_offset
   old_y_offset = xml_get_attribute_int(node,L"OldYOffset",0); // prior to 210404 was old_y_offset
   // fixed defaults below to be no-ops on 121102
   looped_back_already = xml_get_attribute_int(node,L"LoopedBackAlready",0); // prior to 210404 was looped_back_already
   single_step = xml_get_attribute_int(node,L"SingleStep",0); // prior to 210404 was single_step
   available_for_indexing_flag = xml_get_attribute_int(node,L"AvailableForIndexing",1); 
	// prior to 210404 was available_for_indexing_flag
   containable_by_background_flag = !xml_get_attribute_int(node,L"NotContainableByBackground",0); 
	// prior to 210404 was !containable_by_background_flag // added both !s on 261202
   selectable_flag = !xml_get_attribute_int(node,L"NotSelectable",0); // prior to 210404 was !selectable_flag 
	// added both !s on 261202
   move_between_floor_and_room_flag = !xml_get_attribute_int(node,L"NotMoveableBetweenFloorAndRoom",0); 
	// prior to 210404 was !move_between_floor_and_room_flag // added both !s on 261202
   cycle_stopped_callback = (AnimationCallBack) xml_get_attribute_int(node,L"CallbackIndex",CALLBACK_NOTHING); 
	// prior to 210404 was (int) cycle_stopped_callback);
   cause_of_match_failure_flag = xml_get_attribute_int(node,L"CauseOfMatchFailure",0); 
	// prior to 210404 was cause_of_match_failure_flag // new on 161102
   is_top_of_a_stack_flag = xml_get_attribute_int(node,L"IsTopOfAStack",0); 
	// prior to 210404 was is_top_of_a_stack_flag // new on 161102
   to_be_smashed_flag = xml_get_attribute_int(node,L"ToBeSmashed",0); // prior to 210404 was to_be_smashed_flag // new on 161102
	priority_fixed_flag = xml_get_attribute_int(node,L"ZFixed",0); // prior to 210404 was priority_fixed_flag);
	ideal_speed = xml_get_attribute_int(node,L"IdealSpeed",0); // prior to 210404 was ideal_speed // new on 200404
	current_index = xml_get_attribute_int(node,L"Index",0); // prior to 210404 was current_index 
	// beginning 260103 this should be one level up but still check here for old XML savings - could use tt_xml_version_number
   return(TRUE);
};

//void Sprite::set_recent_xml_location(int location) { // new on 110703 -- obsolete 160204
//	if (recent_xml_location == location) return; // either not news or already know this is trouble...
//	// removed  || (recent_xml_location == -1 && location == 0) on 150703
//	// don't need the following
//	//if (recent_xml_location > 0) {
//	//	remember_included_sprite(recent_xml_location,NULL);
//	//};
//	recent_xml_location = location;
//};
  
//void Sprite::xml_guid(output_stream &stream) {
//   if (guid == NULL) { // initialize on need
//		guid = new GUID;
//		UuidCreate(guid);
//   };
//   stream << "<ID>";
//   print_guid(guid,stream);
//   stream << "</ID>" << endl;
//};


//void Sprite::xml_dump_geometry(output_stream &stream) {
//   stream << "<Geometry>" << endl;  
//   stream << "<Left>" << llx << "</Left>" << endl;
//   stream << "<Bottom>" << lly << "</Bottom>" << endl;
//   stream << "<Width>" << width << "</Width>" << endl;
//   stream << "<Height>" << height << "</Height>" << endl;
//   stream << "</Geometry>" << endl;
//};

//void Sprite::xml_dump_flags(output_stream &stream) {
//   if (!infinite_stack() && visible() && show_all()) return; // all at default values - new on 050802
//   stream << "<SpecialSettings>" << endl;
//   if (infinite_stack()) {
//      stream << "<InfiniteStack/>";
//   };
//   if (!visible()) {
//      stream << "<Invisible/>";
//   };
//   if (!show_all()) {
//      stream << "<OnlyPartlyVisible/>";
//   };
//   stream << "</SpecialSettings>" << endl;
//};

//void Sprite::xml_dump_comment(output_stream &stream) {
//   if (comment != NULL) {
//      stream << "<ToonTalkComment>";
//      comment->xml_dump(stream);
//      stream << "</ToonTalkComment>";
//   };
//};
//
//void Sprite::xml_dump_index(int dump_index, output_stream &stream) {
//   if (dump_index != max_long) {
//      stream << "<index>" << dump_index << "</index>" << endl;
//   };
//};

#endif // XML stuff

GUID *Sprite::create_guid() { // abstracted on 151002 -- moved from nest to Sprite on 140703
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Creating GUID for debug target.");
	};
#endif
   if (guid == NULL) {
		guid = new GUID;
		UuidCreate(guid); 
		// tried CoCreateGuid but this is lower overhead and UuidCreateSequential while 3 times faster (3 microseconds vs. 1) is not random or secure
		register_guid(guid,this);
	};
	return(guid);
};

void Sprite::set_guid(GUID *new_guid) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Setting GUID for debug target.");
	};
#endif
	if (guid != NULL) {
		if (new_guid != NULL && IsEqualGUID(*guid,*new_guid)) { // just a bit wasteful but already knew this -- new 090803
			if (new_guid != guid) delete new_guid;
			return;
		};
		deregister_guid(guid);
#if TT_DEBUG_ON
		// at least for now
		wide_character guid_string[guid_string_size];
		guid_as_hexadecimal(guid,guid_string);
		tt_error_file() << "Replacing GUID " << copy_narrow_string(guid_string) << endl;
#endif
		delete guid;
	};
	guid = new_guid;
	if (guid != NULL) {
		Sprite *already_registered = register_guid(guid,this);
		if (already_registered != NULL && already_registered != this) {
			// new on 050304 -- someone else already has this GUID (can happen by control-c control-v)
			already_registered->just_remove_guid();
		};
	};	  
};

void Sprite::just_remove_guid() {
	if (guid != NULL) {
		delete guid; 
		guid = NULL;
	};
};

void Sprite::note_that_team_is_about_to_run_again() { 
	// new on 030105 -- initially only used to update random sensors
	Sprites *remaining = followers;
	while (remaining != NULL) {
		remaining->first()->note_that_team_is_about_to_run_again();
		remaining = remaining->rest();
	};
};

//void Sprite::check_and_update_include_locations() { // new on 160703 -- obsolete 160204
//	if (recent_xml_location > 0 && recent_xml_location < tt_include_location_at_the_start_of_segment) {
//		// this is obsolete so regenerate
//		xml_element *old_element = existing_element_at_include_location(recent_xml_location);
//		generate_include_file();
//		if (old_element != NULL) {
//			xml_set_attribute(old_element,L"Include",recent_xml_location); // update with new location
//		};
//	};
//	if (leader != NULL) {
//		leader->check_and_update_include_locations();
//	};
//};

boolean Sprite::smaller_than_a_pixel() { // new on 021003
	return(width < tt_screen->one_pixel_in_x_city_units() || height < tt_screen->one_pixel_in_y_city_units());
};

void release_sprite_data() {
  for (int i = 0; i < tt_appearance_count; i++) {
//	  if (sprite_file_names[i] == NULL) {
//		  delete [] ideal_parameters[i];
//		  image_cycles[i][0].delete_images(); // since the images are shared
	  // following was commented out - restored on 270200 since led to a leak
//	  for (int j = 0; j < parameter_counts[i]; j++) {
//		  delete image_cycles[i][j];
//	  };
	  if (i != SYNTHETIC_SPRITE && i != USER_SPRITE) delete [] image_cycles[i];
#if SOUND_CAPABLE		  
	  delete [] sound_file_names[i];
#endif
  };
//#if !TT_DIRECTX
//  if (tt_sound_option&SOUND_BIT) {
//	  stop_sound();
////	  for (i = 1; i <= number_of_sound_files; i++) {
////		  if (cached_resources[number_of_image_files+i] != NULL) {
////			  if (sound_memory_handles[i-1] != NULL) {
////				  GlobalUnlock(sound_memory_handles[i-1]);
////				  GlobalFree(sound_memory_handles[i-1]);
////			  } else {
////				  delete [] cached_resources[number_of_image_files+i];
////			  };
////		  };
////	  };
//  };
//#endif
//#if !TT_DIRECTX
  _lclose(resolution_independent_file);
//#endif
  _lclose(images_file);
//  if (cached_resources != NULL) delete [] cached_resources;
//  if (cache_last_used != NULL) delete [] cache_last_used;
  if (image_offsets != NULL) delete [] image_offsets;
  if (sound_offsets != NULL) delete [] sound_offsets;
  if (sound_cache_indices != NULL) {
     delete [] sound_cache_indices;
     sound_cache_indices = NULL;
  };
  if (sound_cache_ids != NULL) delete [] sound_cache_ids;
//  if (sound_memory_handles != NULL) delete [] sound_memory_handles;
  deallocate_appearance_data();
  if (backgrounds != NULL) delete [] backgrounds;
#if TT_DEBUG_ON
  if (tt_debug_mode == 241299) {
	  int image_count = 0;
	  long byte_total = 0;
     int i;
	  tt_error_file() << "images_needed[] = {";
	  for (i = 0; i < number_of_image_files; i++) {
		  if (images_retrieved[i] != 0) {
			  image_count++;
			  if (images_retrieved[i] > 0) byte_total += images_retrieved[i];
			  tt_error_file() << i << ", ";
		  };
	  };
	  tt_error_file() << "};" << endl;
	  tt_error_file() << "images_missing[] = {";
	  for (i = 0; i < number_of_image_files; i++) {
		  if (images_retrieved[i] == -1) {
			  tt_error_file() << i << ", ";
		  };
	  };
	  tt_error_file() << "};" << endl;
	  tt_error_file() << "Total images loaded " << image_count << " for a total byte count of " << byte_total << endl;
	  delete [] images_retrieved;
	  int sound_count = 0;
	  long sound_byte_total = 0;
	  tt_error_file() << "sounds_need[] = {";
	  for (i = 0; i < number_of_sound_files; i++) {
		  if (sounds_retrieved[i] != 0) {
			  sound_count++;
			  if (sounds_retrieved[i] > 0) sound_byte_total += sounds_retrieved[i];
			  tt_error_file() << i << ", ";
		  };
	  };
	  tt_error_file() << "};" << endl;
	  tt_error_file() << "sounds_missing[] = {";
	  for (i = 0; i < number_of_sound_files; i++) {
		  if (sounds_retrieved[i] == -1) {
			  tt_error_file() << i << ", ";
		  };
	  };
	  tt_error_file() << "};" << endl;
	  tt_error_file() << "Total sounds loaded " << sound_count << " for a total byte count of " << sound_byte_total << endl;
	  delete [] sounds_retrieved;
  };
#endif
};

boolean initialize_sprite_values() {
	if (!allocate_appearance_data()) return(TRUE); // new on 270200 - already allocated - worry about change in resolution from previous call??
// called by tt_setmode when values change
//  cached_resources = new bytes[number_of_resources+1]; //LAST_IMAGE+1];
//  cache_last_used = new long[number_of_resources+1];
  int i;
//  for (i = 0; i < number_of_resources+1; i++) {
//	  cached_resources[i] = NULL;
//	  cache_last_used[i] = 0;
//  };
  for (i = 0; i < tt_appearance_count; i++) { 
	  if (i == SYNTHETIC_SPRITE) {
		  parameter_counts[i] = 256; // synthetic sprite colors
		  max_widths[i] = ideal_screen_width/6;
		  max_heights[i] = ideal_screen_height/6;
		  priority_function_of_lly_flags[i] = FALSE;
     } else if (i == USER_SPRITE) {
        parameter_counts[i] = 1;
		  max_widths[i] = ideal_screen_width/6;  // does it matter?
		  max_heights[i] = ideal_screen_height/6;
		  priority_function_of_lly_flags[i] = FALSE;
	  } else {
		  if (!load_sprite_file((SpriteCode) i)) return(FALSE);
	  };
	  collision_width_factors[i] = 100;  // 100% by default
	  collision_height_factors[i] = 100;
	  collision_x_offset_correction[i] = 0;
	  collision_y_offset_correction[i] = 0;
	  collision_region_equals_region[i] = TRUE; // by default
  };
  delete [] sprite_offsets; // done with it now
  collision_width_factors[PERSON_WALKING_SPRITE] = 67;
  collision_height_factors[PERSON_WALKING_SPRITE] = 12;
  collision_region_equals_region[PERSON_WALKING_SPRITE] = FALSE;

  collision_width_factors[MARTIAN_SPRITE] = 120;
  collision_x_offset_correction[MARTIAN_SPRITE] = -10;
  collision_region_equals_region[MARTIAN_SPRITE] = FALSE;

  collision_width_factors[HELIOLND] = 70;
  collision_x_offset_correction[HELIOLND] = 10;
  collision_height_factors[HELIOLND] = 12;
  collision_region_equals_region[HELIOLND] = FALSE;
  // can walk behind house
  collision_width_factors[HOUSE_A_SIDE_SPRITE] = 90;
  collision_x_offset_correction[HOUSE_A_SIDE_SPRITE] = 5;
  // if I change the following update house_appearance
  collision_height_factors[HOUSE_A_SIDE_SPRITE] = 33;
  collision_region_equals_region[HOUSE_A_SIDE_SPRITE] = FALSE;
  // door is a bit wider than it looks 
  collision_width_factors[DOOR_A_SPRITE] = 120;
  collision_height_factors[DOOR_A_SPRITE] = 50;
  collision_x_offset_correction[DOOR_A_SPRITE] = -10;
  collision_y_offset_correction[DOOR_A_SPRITE] = -33;
  collision_region_equals_region[DOOR_A_SPRITE] = FALSE;
  // higher up than other doors cause of steps
//  collision_width_factors[HOUSE_B_SIDE_SPRITE] = 35;
  collision_height_factors[HOUSE_B_SIDE_SPRITE] = 33;
//  collision_x_offset_correction[HOUSE_B_SIDE_SPRITE] = 33;
  collision_region_equals_region[HOUSE_B_SIDE_SPRITE] = FALSE;
  collision_width_factors[DOOR_B_SPRITE] = 100;
  collision_height_factors[DOOR_B_SPRITE] = 50;
//  collision_y_offset_correction[DOOR_B_SPRITE] = -33;
//#if TT_BETA
//  if (tt_running_old_demos) // restore old version
     collision_y_offset_correction[DOOR_B_SPRITE] = -100;
//#endif
  collision_region_equals_region[DOOR_B_SPRITE] = FALSE;
  collision_width_factors[HOUSE_C_SIDE_SPRITE] = 90;
  collision_x_offset_correction[HOUSE_C_SIDE_SPRITE] = 5;
  collision_height_factors[HOUSE_C_SIDE_SPRITE] = 33;
  collision_region_equals_region[HOUSE_C_SIDE_SPRITE] = FALSE;
  collision_width_factors[DOOR_C_SPRITE] = 120;
  collision_height_factors[DOOR_C_SPRITE] = 50;
  collision_x_offset_correction[DOOR_C_SPRITE] = -10;
  collision_y_offset_correction[DOOR_C_SPRITE] = -33;
  collision_region_equals_region[DOOR_C_SPRITE] = FALSE;
//if (!beta4_images()) {
     collision_width_factors[BROKEN_ROCKET_SPRITE] = 60;
     collision_x_offset_correction[BROKEN_ROCKET_SPRITE] = 20;
     collision_height_factors[BROKEN_ROCKET_SPRITE] = 33;
     collision_region_equals_region[BROKEN_ROCKET_SPRITE] = FALSE;
//};
  collision_width_factors[BROKEN_ROCKET_DOOR_SPRITE] = 120;
  collision_height_factors[BROKEN_ROCKET_DOOR_SPRITE] = 50;
  collision_x_offset_correction[BROKEN_ROCKET_DOOR_SPRITE] = -10;
  collision_y_offset_correction[BROKEN_ROCKET_DOOR_SPRITE] = -33;
  collision_region_equals_region[BROKEN_ROCKET_DOOR_SPRITE] = FALSE;

  // need to make only the center of the main bubble sensitive
  collision_x_offset_correction[THOUGHT_BUBBLE_SPRITE] = 12;
  collision_y_offset_correction[THOUGHT_BUBBLE_SPRITE] = 45;
  collision_width_factors[THOUGHT_BUBBLE_SPRITE] = 67;
  collision_height_factors[THOUGHT_BUBBLE_SPRITE] = 40;
  collision_region_equals_region[THOUGHT_BUBBLE_SPRITE] = FALSE;	 
  // goofy numbers that follow are to make just the hand (not the rest of
  // the arm) active for collisions
  // shift right to ignore thumb
//  collision_x_offset_correction[HANDARM] = 0.4;
  // just the hand
//  collision_y_offset_correction[HANDARM] = 0.9;
  // remainder after ignoring thumb
//  collision_width_factors[HANDARM] = tt_ideal_units_per_horizontal_pixel*0.4;
  // the collision region of the hand is not high because if no
  // collision tries a bit lower but this gives priority to the fingers
//  collision_height_factors[HANDARM] = tt_ideal_units_per_vertical_pixel*0.06;
  // just the pointing finger
  collision_width_factors[HAND_SPRITE] = 4;
  collision_height_factors[HAND_SPRITE] = 2;
  collision_x_offset_correction[HAND_SPRITE] = 52;
  collision_y_offset_correction[HAND_SPRITE] = 80;
  collision_region_equals_region[HAND_SPRITE] = FALSE;	 
//  collision_height_factors[ROBOT_SPRITE] = 80;   // ignore antenna
//  collision_region_equals_region[ROBOT_SPRITE] = FALSE;	 
  collision_x_offset_correction[BIRD_SPRITE] = 10;
  collision_width_factors[BIRD_SPRITE] = 80;
  collision_y_offset_correction[BIRD_SPRITE] = 10;
  collision_height_factors[BIRD_SPRITE] = 80;
  collision_region_equals_region[BIRD_SPRITE] = FALSE;	 
  collision_x_offset_correction[NEST_SPRITE] = 10;
  collision_width_factors[NEST_SPRITE] = 80;
  collision_y_offset_correction[NEST_SPRITE] = 10;
  collision_height_factors[NEST_SPRITE] = 80;
  collision_region_equals_region[NEST_SPRITE] = FALSE;	 		 

  collision_width_factors[NOTEPAD_SPRITE] = 90;
  collision_x_offset_correction[NOTEPAD_SPRITE] = 5;
  collision_height_factors[NOTEPAD_SPRITE] = 90;
  collision_y_offset_correction[NOTEPAD_SPRITE] = 5;
  collision_region_equals_region[NOTEPAD_SPRITE] = FALSE;	 		 
  // since it can stretch it can get very tall
  collision_height_factors[EXPANDER_SPRITE] = 100; // was 60
  collision_width_factors[EXPANDER_SPRITE] = 80; // was 60
  collision_x_offset_correction[EXPANDER_SPRITE] = 5; // was 10
  collision_region_equals_region[EXPANDER_SPRITE] = FALSE;
  collision_height_factors[VACUUM_SPRITE] = 80; // was 60
  collision_width_factors[VACUUM_SPRITE] = 80;
  collision_x_offset_correction[VACUUM_SPRITE] = 10;
  collision_y_offset_correction[VACUUM_SPRITE] = 10;
  collision_region_equals_region[VACUUM_SPRITE] = FALSE;
  collision_height_factors[COPIER_SPRITE] = 70;
  collision_y_offset_correction[COPIER_SPRITE] = 15;
  collision_region_equals_region[COPIER_SPRITE] = FALSE;
  collision_height_factors[BOMB_SPRITE] = 60;
  collision_width_factors[BOMB_SPRITE] = 80;
  collision_region_equals_region[BOMB_SPRITE] = FALSE;
  collision_height_factors[TRUCK_SIDE_SPRITE] = 20;
  collision_width_factors[TRUCK_SIDE_SPRITE] = 90;
  collision_region_equals_region[TRUCK_SIDE_SPRITE] = FALSE;
//  initialize_background_sprites(); moved on 250901 since too early to know tt_missing_picture_file_extension
  return(TRUE);
};

HFILE load_data_file(const_ascii_string file_name, long minimum_size, boolean ok_to_fail, boolean resolution_independent) {
   HFILE file = NULL;
   OFSTRUCT file_struct;
   FileNameStatus file_status;
	string full_file_name = NULL;
	if (tt_builtin_picture_directory != NULL && !resolution_independent) { 
		// new on 010302 so that look first same place that built-in pictures are stored
		string file_name_to_check = append_strings(tt_builtin_picture_directory,file_name);
		// this used to have !ok_to_fail but used below as of 120304
		full_file_name = existing_file_name(file_status,file_name_to_check,NULL,NULL,TRUE,FALSE,FALSE,FALSE,FALSE); 
		// added args on 090603 so won't check in the main ToonTalk installation directory for artwork
		delete [] file_name_to_check;
		if (full_file_name == NULL) { // new on 120304 to try again with original file name (since may be on CD-ROM or search path)
			full_file_name = existing_file_name(file_status,file_name,NULL,"Java",TRUE,!ok_to_fail,FALSE,FALSE,FALSE); 
			// looks in Java since that is where m25.us1 (small) is now
		};
	}; 
	if (full_file_name == NULL && (resolution_independent || tt_video_mode == 320)) { 
		// try default place (as of 060304 also if video mode is 320x240)
		// on 090603 added resolution_independent since number pad artwork has changed so don't want old version of m25
		full_file_name = existing_file_name(file_status,file_name,NULL,NULL,TRUE,!ok_to_fail);
	};
   if (full_file_name != NULL) {
      if (file_status == FILE_ON_CDROM) {
//       loading_wait();
         ascii_string temp_name = copy_to_temp_directory(full_file_name,file_name);
         if (temp_name != NULL) { // succeeded
            file = OpenFile(temp_name,&file_struct,OF_READ);
            delete [] temp_name;
         };
      } else {
         file = OpenFile(full_file_name,&file_struct,OF_READ);
      };
      if (file != -1 && file_status != FILE_ON_CDROM && get_file_size(file) < minimum_size && tt_builtin_picture_directory == NULL) {
         message_box(MB_ICONEXCLAMATION|MB_OK,a2u(full_file_name),AC(IDC_FILE_IS_TOO_SMALL));
      };
		delete [] full_file_name;
	} else if (ok_to_fail) { // new on 031001
		return(NULL);
	} else {
		if (strncmp(file_name,"m22",3) == 0) { // new on 130201
			message_box(MB_ICONEXCLAMATION|MB_OK,S(IDS_M22_MISSING,"This installation of ToonTalk is missing the low resolution graphics data file. Click on 'Set Options' to switch to 640x480 or higher or contact support@toontalk.com."));
		} else {
			tt_exit_failure(TROUBLE_SHOOT_BAD_INSTALLATION,S(IDS_NEED_TO_REINSTALL));
		};
	};
   return(file);
};

boolean open_images_file(int mode_number) {
	if (images_file != -1) return(TRUE); // new on 190300 since already opened
	char file_name[MAX_PATH]; // was 128 prior to 290302
//	output_string_stream file(file_name,MAX_PATH); // rewritten 240104
   ascii_string data_suffix = "us1"; // prior to 080304 was ini_entry(AC(IDS_FILE_EXTENSIONS),AC(IDS_DATA_FILE_SUFFIX),FALSE); 
#if TT_SMALL_TRIAL
//	file << "m22.jp0";
	strcpy(file_name,"m22.jp0");
	tt_graphics_video_mode_width = 320;
	tt_graphics_video_mode_height = 240;
	tt_good_sizes_adjust_to_screen_size = FALSE;
#else
	if (tt_video_mode == 320 || ini_int(AC(IDC_VERSIONS),"640x480",FALSE) == 0) { // second disjunct new on 190300
//		file << "m22."; // old old names for these video modes
		strcpy(file_name,"m22.");
		tt_graphics_video_mode_width = 320;
		tt_graphics_video_mode_height = 240;
//		if (tt_video_mode > 640) { // new on 190300
			tt_good_sizes_adjust_to_screen_size = FALSE;
//		};
	} else {
#if !TT_SMALL_TRIAL
		if (tt_video_mode > 640) { // new on 031001
//			file << "m" << tt_video_mode << ".";
			file_name[0] = 'm';
			itoa(tt_video_mode,file_name+1,10);
			strcat(file_name,".");
			tt_graphics_video_mode_width = tt_video_mode;
			tt_graphics_video_mode_height = (3*tt_graphics_video_mode_width)/4;
		} else 
#endif
//			file << "m25.";
			strcpy(file_name,"m25.");
			// don't need to set tt_graphics_video_mode_width, etc. since OK by default
	};
//	file << data_suffix; //was ".dat";
	strcat(file_name,data_suffix);
#endif
//	file.put('\0');
	boolean ok_to_fail = (tt_video_mode > 640); // these data files might not exists but can fall back on m25
#if TT_SMALL_TRIAL
   images_file = load_data_file(file_name,1000000L,FALSE,FALSE);
#else
	images_file = load_data_file(file_name,80000L,ok_to_fail,FALSE); // 2 zeroes removed on 081001 since can have short DAT files if pictures are in a directory
#endif
	if (ok_to_fail && images_file == NULL) { // new on 031001
		// since new mode isn't installed fall back on old faithful m25
//		output_string_stream file(file_name,128); // rewritten 240104
//		file << "m25." << data_suffix;
		strcpy(file_name,"m25.");
		strcat(file_name,data_suffix);
//		file.put('\0');
		images_file = load_data_file(file_name,80000L,FALSE,FALSE); // 2 zeroes removed on 081001 since can have short DAT files if pictures are in a directory
		tt_graphics_video_mode_width = 640; // even if tt_picture_directory is non-null??
		tt_graphics_video_mode_height = 480;
	};
	char cookie[2];
	int status = _lread(images_file,cookie,2);
	if (status < 0 || cookie[0] != 'T' || cookie[1] != 'T') {
		tt_error_file() << S(IDS_DAMAGED_FILE) << file_name
							 << S(IDS_STATUS_EQUALS) << status << endl;
		character *buffer = new character[256];
		output_string_stream reason(buffer,256);
		reason << S(IDS_BUT_FILE_IS) << space() << file_name << space()
				 << S(IDS_FILE_DAMAGED);
		reason.put('\0');
		tt_exit_failure(TROUBLE_SHOOT_BAD_INSTALLATION,buffer);
		return(FALSE);
	};
	read_palette(images_file);
	short int new_appearance_count;
	_lread(images_file,(unsigned char *) &new_appearance_count,
							 sizeof(new_appearance_count));
	// new_appearance_count has one extra for first and last offsets
//	if (new_appearance_count-1 != appearance_count) {
//		tt_error_file() << "Data file has different number of sprites" << endl;
//		char *buffer = new char[256];
//		output_string_stream reason(buffer,256);
//		reason << S(IDS_BUT_FILE_IS) << "  " << file_name << " "
//							  << S(IDS_FILE_DAMAGED);
//		reason.put((char) 0);
//		tt_exit_failure(buffer);
//		return(FALSE);
//	};
	tt_appearance_count = (short int) (new_appearance_count-1);
	sprite_offsets = new long[new_appearance_count];
	_lread(images_file,(unsigned char *) sprite_offsets,new_appearance_count*sizeof(long));
	_lread(images_file,(unsigned char *) &number_of_image_files,sizeof(number_of_image_files));
	image_offsets = new long[number_of_image_files];
	_lread(images_file,(unsigned char *) image_offsets,number_of_image_files*sizeof(long));
#if TT_DEBUG_ON
	if (tt_debug_mode == 241299) {
		images_retrieved = new long[number_of_image_files];
		for (int i = 0; i < number_of_image_files; i++) {
			images_retrieved[i] = 0;
		};
	};
#endif
	// temporarily
//	compressed_image_offsets = new long[number_of_image_files];
//	for (int i = 0; i < number_of_image_files; i++) {
//		_llseek(images_file,image_offsets[i],0);
// tell returns -1; and this isnt where all the time is going anyway
//		compressed_image_offsets[i] = tell(images_file);
//	};
	number_of_image_files--;
//#if TT_DIRECTX
//   allocate_brushes();
//   const short int number_of_brushes = 14;
//	for (int i = 0; i < number_of_brushes; i++) {
//       HRSRC resource;
//       HGLOBAL resource_data;
//       bytes brush;
//       if (((resource = FindResource(NULL,MAKEINTRESOURCE(i+1), "BRUSH_DATA")) != NULL) &&
//           ((resource_data = LoadResource(NULL,resource)) != NULL) &&
//           ((brush = (bytes) LockResource(resource_data)) != NULL)) {
//			  create_DIB_brush(i,brush);
//          // this is only run in Win 95 and NT which unlocks and releases for me
//       } else {
//          tt_error_file() << S(IDS_PROBLEMS_INITIALIZING_BRUSHES) << endl;
//       };
//   };
//   number_of_sound_files = 48; // with some numbers missing
//#else
//   string suffix = ini_entry("FileExtensions","DataFileSuffix");
   // now for reading resolution independent data file
   char short_file_name[16];
   strcpy(short_file_name,AC(IDS_RESIND_DOT));
#if TT_SMALL_TRIAL
	strcat(short_file_name,"jp0");
	resolution_independent_file = load_data_file(short_file_name,100000L,FALSE,TRUE);
#else
   if (data_suffix != NULL) strcat(short_file_name,data_suffix);
   resolution_independent_file = load_data_file(short_file_name,1000000L,FALSE,TRUE);
#endif
//		done_waiting_for_load(); // no-op if not message window put up
//		char cookie[2];
		status = _lread(resolution_independent_file,cookie,2);
//		if (resolution_independent_file.get() != 'T' || resolution_independent_file.get() != 'T') {
		if (status < 0 || cookie[0] != 'T' || cookie[1] != 'T') {
			tt_error_file() << S(IDS_DAMAGED_RESIND_FILE) << status << endl;
			tt_sound_option = 0;
			tt_exit_failure(TROUBLE_SHOOT_BAD_INSTALLATION,S(IDS_RESIND_DAMAGED));
			return(FALSE);
		};
//		resolution_independent_file.read((unsigned char *) &number_of_sound_files, 
//							  sizeof(number_of_sound_files));
//		status = 
		_lread(resolution_independent_file,(unsigned char *) &number_of_sound_files,sizeof(number_of_sound_files));
//		sound_memory_handles = new HGLOBAL[number_of_sound_files];
//		status =
      sound_offsets = new long[number_of_sound_files]; 
		_lread(resolution_independent_file,(unsigned char *) sound_offsets,
							 number_of_sound_files*sizeof(long));
		number_of_sound_files--; // was one extra for final offset
		short int number_of_brushes;
//		status = 
		_lread(resolution_independent_file,(unsigned char *) &number_of_brushes,sizeof(number_of_brushes)); // was sizeof(sound_offsets) prior to 290302
		if (number_of_brushes != DIB_brush_count) {
			tt_error_file() << S(IDS_BRUSH_NUMBER_MISMATCH) << endl;
		};
		// go to after sound data
		_llseek(resolution_independent_file,sound_offsets[number_of_sound_files],0);
#if TT_DEBUG_ON
		if (tt_debug_mode == 241299) {
			sounds_retrieved = new long[number_of_image_files];
			for (int i = 0; i < number_of_image_files; i++) {
				sounds_retrieved[i] = 0;
			};
		};
#endif
		allocate_brushes();
		unsigned char brush[64];
		for (int i = 0; i < number_of_brushes; i++) {
			_lread(resolution_independent_file,brush,64);
         // just to get them all
//   char buffer[32];
//	output_string_stream fn(buffer,32);
//	fn << "brush" << i << ".brh";
//	fn.put((char)0);
//	output_file_stream brush_file(buffer,std::ios_base::binary|std::ios_base::out);
//   brush_file.write((unsigned char *) brush,64);
//   brush_file.close();
			create_DIB_brush(i,brush);
		};
//#endif
      if (sound_offsets == NULL) sound_offsets = new long[number_of_sound_files];
		sound_cache_indices = new unsigned short[number_of_sound_files];
		sound_cache_ids = new unsigned short[number_of_sound_files];
		for (int j = 0; j < number_of_sound_files; j++) {
			 sound_cache_indices[j] = 0;
			 sound_cache_ids[j] = max_unsigned_short;
//			sound_memory_handles[i] = NULL;
		};
//	};
	number_of_resources = number_of_image_files+number_of_sound_files;
//   if (data_suffix != NULL) delete [] data_suffix;
   return(TRUE);
};

void release_sound_cache() {
   if (sound_cache_indices == NULL) return; // too early or late
#if !TT_DIRECTX
   new_sound_entry(-1);  // in case a sound is "playing"
#endif
   for (int i = 0; i < number_of_sound_files; i++) {
      if (//sound_cache_indices[i] != 0 &&
          cache_entries[sound_cache_indices[i]].return_cache_id() == sound_cache_ids[i]) {
         // should only be called while shutting down direct sound
//         cache_entries[sound_cache_indices[i]].protected_entry = FALSE;
#if TT_DIRECTX
         cache_entries[sound_cache_indices[i]].stop_sound();
#endif
         cache_entries[sound_cache_indices[i]].free_entry();
         sound_cache_ids[i] = max_unsigned_short;
//         tt_error_file() << "Just freed sound cache: " << sound_cache_indices[i] << " with index " << i << endl;
         sound_cache_indices[i] = 0;
      };
   };
//   sound_cache_just_freed();
};

#if TT_DIRECTX
void stop_all_sounds_in_cache() {
   if (sound_cache_indices == NULL) return; // too early or late
   for (int i = 0; i < number_of_sound_files; i++) {
      if (sound_cache_indices[i] != 0 &&
          cache_entries[sound_cache_indices[i]].return_cache_id() == sound_cache_ids[i]) {
         cache_entries[sound_cache_indices[i]].stop_sound();
      };
   };
//   new_sound_entry(-1);  // in case a sound is "playing"
};
#endif

boolean valid_image_id(int id) {
  return(id >= 0 && id <= number_of_image_files);
};

void java_image_cycle(java_stream &out, int code) {
   // first need to check whether we need to unpatch hat hack
   boolean swap_heads = (code == PERSON_WALKING_SPRITE && tt_programmer_head == 2);
   int saved_tt_programmer_head = tt_programmer_head;
	boolean saved_tt_maintain_history = tt_maintain_history;
   if (swap_heads) {
//		tt_maintain_history = FALSE; // since following isn't being done by user
      tt_global_picture->display_changed(PROGRAMMER_HEAD_REMOTE,0);
   };
   out << "  " << S(IDS_JAVA_IMAGE_CYCLE_COMMENT) << " ";
   describe_appearance((SpriteCode) code,out,PLURAL_ARTICLE);
   out << "." << endl;
   java_string image_cycle_name = java_array(S(IDS_JAVA_TT_DOT_IMAGE_CYCLES),code);
   int count = parameter_counts[code];
   java_assign_new(out,image_cycle_name,S(IDS_JAVA_TT_IMAGE_CYCLES),count,
		// fixed the following on 300301 since it caused things to get smaller if running with a high screen resolution
                   max_widths[code]/compute_units_per_horizontal_pixel(), //  tt_screen->user_x(max_widths[code]),
                   max_heights[code]/compute_units_per_vertical_pixel()); // tt_screen->user_y(max_heights[code]));
   for (int i = 0; i < count; i++) {
      image_cycles[code][i].java(out,image_cycle_name,i);
   };
   delete [] image_cycle_name;
   if (swap_heads) {
      tt_global_picture->display_changed(PROGRAMMER_HEAD_REMOTE,saved_tt_programmer_head);
		tt_maintain_history = saved_tt_maintain_history;
   };
};

//void Sprite::side_about_to_be_seen(boolean front) {
//	Sprites *remaining = followers;
//	while (remaining != NULL) {
//		remaining->first()->side_about_to_be_seen(front);
//		remaining = remaining->rest();
//	};
//};
/*
#if TT_IMGSOURCE // not using this anymore
		HISSRC file = _ISOpenFileSource(full_file_name);
		if (file == NULL) {
			UINT32 error_code = _ISGetLastError();
			tt_error_file() << "Error reading " << full_file_name << " code is " << error_code << endl;
			return(NULL);
		};
		dib = (LPBITMAPINFOHEADER) _ISReadImageToDIB(file);
		_ISCloseSource(file);
		if (dib == NULL) {
			tt_error_file() << "Troubles loading picture file " << full_file_name << endl;
			return(NULL);
		};
#if TT_DEBUG_ON
		if (tt_debug_mode == 31001) {
			tt_error_file() << (timeGetTime()-start_time)
							<< "ms to ISReadImageToDIB " << full_file_name << endl;
			start_time = timeGetTime();
		};
#endif
#endif
*/
//#if TT_DIRECTX_TRANSFORM

//draw_surface UserImage::load_image_internal() {
//	if (file_name == NULL) {
//      return(NULL);
//   };
//	draw_surface surface = load_image_using_directx_transform(full_file_name,pixel_width,pixel_height); // not clear if pixel_width,pixel_height are needed
//	if (surface == NULL) {
//		return(TTImage::load_image_internal());
//	} else {
//		return(surface);
//	};
//};

//draw_surface BackgroundImage::load_image_internal() {
//	return(TTImage::load_image_internal()); // for now don't use transforms for backgrounds
//};

//#endif

//HGLOBAL Sprite::xml_global_handle() { // maybe want boolean trim
//	HGLOBAL text_handle = global_alloc(GHND,max_encoding_buffer_size);
//	if (text_handle == NULL) return(NULL); // warn??
//	string text = (string) GlobalLock(text_handle);
//	output_string_stream stream(text,max_encoding_buffer_size);
//   stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl;
//   stream << "<!--" << endl;
//   print_clipboard_explanation(stream,kind_of());
//   stream << "-->" << endl;
//   stream << "<ToonTalkObject xmlns=\"http://www.toontalk.com\">" << endl;
//   xml_dump(stream);
//   stream << "</ToonTalkObject>" << endl;
//   stream.put('\0'); // terminate string
//   GlobalUnlock(text_handle); // added 060802
//   return(text_handle);
//};

	//output_string_stream stream(text,max_encoding_buffer_size);
 //  stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl;
 //  stream << "<!--" << endl;
 //  print_clipboard_explanation(stream,kind_of());
 //  stream << "-->" << endl;
 //  stream << "<ToonTalkObject xmlns=\"http://www.toontalk.com\">" << endl;
 //  xml_dump(stream);
 //  stream << "</ToonTalkObject>" << endl;
 //  stream.put('\0'); // terminate string

 /* noticed (or made) obsolete on 131102
void Sprite::set_after(millisecond duration, long *variable, long value) {
	// variable is any 32bit pointer and value can be any 32bit value
	if (variable == NULL) return;
	if (duration <= 0) {
		*variable = value;
	} else {
		insert_animation(new Set_After(tt_current_time+duration,variable,value));
	};
};

void Sprite::increment_after(millisecond duration, long *variable, long increment) {
	// variable is any 32bit pointer and value can be any 32bit value
	if (variable == NULL) return;
	if (duration <= 0) {
		*variable += increment;
	} else {
		insert_animation(new Increment_After(tt_current_time+duration,variable,increment));
	};
};

void Sprite::true_after(millisecond duration, flag *status) {
	if (status == NULL) return;
	if (duration <= 0) {
		*status = TRUE;
	} else {
		insert_animation(new True_When(tt_current_time+duration,status));
	};
};

void Sprite::animate_size(city_coordinate *new_width,
								  city_coordinate *new_height,
								  millisecond expansion_duration,
//								  boolean *status,
								  Sprite *notify,
								  AnimationCallBack callback,
								  boolean signal_change) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 11099 && debug_counter == tt_debug_target) {
		log(_T("Debug target size animated."));
	};
#endif
//	set_clean_status(DIRTY);
	add_ease_in_and_out_to_goal(&width,new_width,expansion_duration,NULL,CALLBACK_NOTHING);
	// only one gets real callback so it is only called once
	add_ease_in_and_out_to_goal(&height,new_height,expansion_duration,notify,callback);
	if (signal_change) {
		do_after(expansion_duration,this,SIGNAL_SIZE_AND_LOCATION_CALLBACK);
	};
};
*/
