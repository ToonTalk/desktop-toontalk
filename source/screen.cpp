// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.

/* Implements the screen
	later view ports -- like multiple virtual screens
	The screen serves two functions:
	  -- ordering of the painting (back to front)
	  -- collision detection and reporting
*/

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif   
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif 
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif   
#if !defined(__TT_CLEANUP_H)   
#include "cleanup.h"
#endif   
#if !defined(__TT_REGION_H)   
#include "region.h"
#endif   
#if !defined(__TT_CITY_H)   
#include "city.h"
#endif   
#if !defined(__TT_BCKGRND_H)   
#include "bckgrnd.h"
#endif   
#if !defined(__TT_BLIT_H)   
#include "blit.h"
#endif   
#if !defined(__TT_DIRTY_H)   
#include "dirty.h"
#endif   
//#include <strstrea.h> commented out 210703
#if HIGH_RESOLUTION_CLOCK
#include "types.h"
#include "htimer.h"
#endif
#if !defined(__TT_USER_H)   
#include "user.h"
#endif   
#if !defined(__TT_PICTURE_H)   
#include "picture.h"
#endif   
#if !defined(__TT_TOOLS_H)   
#include "tools.h"
#endif   
#if !defined(__TT_PRGRMMR_H)   
#include "prgrmmr.h"
#endif   
#if !defined(__TT_HELP_H)   
#include "help.h"
#endif
#if !defined(__TT_MARTIAN_H)   
#include "martian.h"
#endif
#if !defined(__TT_LOG_H)   
#include "log.h"
#endif   
#if TT_WINDOWS
#include <windows.h>
#include <mmsystem.h>
#if !defined(__TT_WINMAIN_H)   
#include "winmain.h"
#endif   
#endif
#if !defined(__TT_SCREEN_H)   
#include "screen.h"
#endif   
#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif   
   
// #pragma hdrstop      

#include <string.h>
//#include <fstream.h>
#if FAST_GRAPH
#include <fastgraf.h>
#endif
//#if FLASH_GRAPHICS
//#include <fg.h>
//#endif
//#include <string.h> // for strlen
//#include <stdlib.h> // for debuggin (and random, min, max, range)

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef Sprite *Layer;

const int subtitle_max_size = 500;

Screen::Screen() :
//  layers(new Layer[max_number_of_layers]),
  layer_count(0),
  // until there are multiple views the following is fine
  // subtracting 2 since there is the border around the whole thing
  width(tt_screen_width-2),
   // ideal_screen_width-tt_ideal_units_per_horizontal_pixel;
  height(tt_screen_height-2),
   // ideal_screen_height-tt_ideal_units_per_vertical_pixel;
  subtitle_character_width(tt_screen_width/max_subtitle_length()),
  subtitle_character_height(tt_screen_height/8), 
  subtitle_length(0),
//  previous_subtitle_length(0),
//  blackened_count(0),
  subtitle_stop_time(0),
  //x_offset(0), // until multiple views -- had initial value of 1 prior to 290302
  //y_offset(0), // had initial value of 1 prior to 290302
  view_status(CAMERA_ABOVE),
  background(NULL),
//  backmost_unchanged_layer_index(-1),
//  can_cache_for_last_n_frames(0),
//  background_unchanged_count(0),
//  background_cache_valid(FALSE),
//  cached(FALSE),
//  number_needing_collision_notification(0),
  postponed_count(0),
  current_selection(NULL),
  city_to_user_horizontal_units(ideal_screen_width/1000),
  city_to_user_vertical_units(ideal_screen_height/1000),
  number_of_subtitle_lines_last_time(0),
  duration_of_current_subtitle(0),
  max_number_of_layers(128), // as of 080201 these can grow - defaults left as they were
  max_number_of_postponed_layers(256) {
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "creating tt_screen 1" << endl;
	tt_error_file().flush();
#endif
	layers = new SpritePointer[max_number_of_layers];
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "creating tt_screen 2" << endl;
	tt_error_file().flush();
#endif
	postponed_layers = new SpritePointer[max_number_of_postponed_layers];
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "creating tt_screen 3" << endl;
	tt_error_file().flush();
#endif
   pixel_min_x = 0; // x_offset;
   pixel_max_x = pixel_min_x+width-1;
   pixel_min_y = 0; // y_offset;
   pixel_max_y = pixel_min_y+height-1;
//  tt_leaders = background->pointer_to_pointer_to_leaders();
   subtitle = new wide_character[subtitle_max_size];  // extra big so if running 1600x1200 there is enough room
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "creating tt_screen 4" << endl;
	tt_error_file().flush();
#endif
	if (need_wide_characters()) {
		subtitle[0] = wide_space; // add trailing blank
	} else {
		subtitle[0] = ' '; // add trailing blank
	};
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "creating tt_screen 5" << endl;
	tt_error_file().flush();
#endif
	scale = 100; // just to keep the following happy: -- new on 021204 -- caused crash on Windows Me
	set_city_coordinates_per_pixel();
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "creating tt_screen 6" << endl;
	tt_error_file().flush();
#endif
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "finished creating tt_screen" << endl;
	tt_error_file().flush();
#endif
};

void Screen::set_city_coordinates_per_pixel() { // new on 210804
	pixels_per_horizontal_city_coordinates = (100.0*tt_screen_width)/(ideal_screen_width*scale);
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "set_city_coordinates_per_pixel 1" << endl;
	tt_error_file().flush();
#endif
	pixels_per_vertical_city_coordinates = (100.0*tt_screen_height)/(ideal_screen_height*scale);
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "set_city_coordinates_per_pixel 2" << endl;
	tt_error_file().flush();
#endif
	// and their reciprocals
	horizontal_city_coordinates_per_pixel = (ideal_screen_width*scale)/(100.0*tt_screen_width);
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "set_city_coordinates_per_pixel 3" << endl;
	tt_error_file().flush();
#endif
	vertical_city_coordinates_per_pixel = (ideal_screen_height*scale)/(100.0*tt_screen_height);
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "set_city_coordinates_per_pixel 4" << endl;
	tt_error_file().flush();
#endif
};

// just for getting performance data out
long last_display_duration = 0;
long total_display_duration = 0;
long total_run_time = 0;

Screen::~Screen() {
//  for (int i = 0; i < layer_count; i++) {
//	 layers[i]->recursively_decrement_screen_ref_count();
//  };
//  delete [] layers;
#if TT_DEBUG_ON
  if (tt_debug_mode == 3 && replaying() && total_run_time > 0) {
	  tt_error_file() << "Total display time was " << total_display_duration
					  << " which was "
					  << (100*total_display_duration)/total_run_time
					  << "% of total run time.  Frame number "
					  << tt_frame_number << endl;
  };
#endif
  if (subtitle != NULL) {
	  delete [] subtitle;
	  subtitle = NULL;
  };
  delete [] layers; // new on 080201
  delete [] postponed_layers;
//  if (previous_subtitle != NULL) delete [] previous_subtitle;
};

flag said_too_many_things = FALSE; // new on 070900

boolean Screen::add(Sprite *new_layer, boolean signal_screen_change) {
#if TT_DEBUG_ON
	if (tt_debug_target == new_layer->debug_counter) {
		tt_error_file() << "Adding debug target to screen." << endl;
	};
#endif
	if (showing(new_layer)) return(FALSE); // already added
//tt_error_file() << "+ " << type_name(new_layer) << "#" << new_layer->return_debug_counter() << " " << tt_frame_number << endl;
   if (signal_screen_change) new_layer->recursively_notice_on_screen_change(TRUE);
	if (layer_count >= max_number_of_layers) {
		// new on 080201 to just grow the number of layers
		tt_error_file() << "Number of things on the screen is now greater than " << (max_number_of_layers-1) 
							 << ". If ToonTalk runs too slowly reduce the number of things on the floor or ground." << endl; // new on 090201
		int new_max_number_of_layers = max_number_of_layers*2;
		SpritePointer *new_layers = new SpritePointer[new_max_number_of_layers];
		for (int i = 0; i < max_number_of_layers; i++) {
			new_layers[i] = layers[i];
		};
		delete [] layers;
      layers = new_layers;
		max_number_of_layers = new_max_number_of_layers;
	};
	layers[layer_count++] = new_layer;
//  new_layer.set_visible(TRUE);
//  cached = FALSE;
//	new_layer->increment_screen_ref_count();
	new_layer->add_outside_followers_to_screen(); // rewritten on 080705 to use this since it does a bit more than the open-coded version below
//	if (!new_layer->followers_completely_inside()) {	
//		Sprites *remaining = new_layer->pointer_to_followers();
//		while (remaining != NULL) {
//		  Sprite *sprite = remaining->first();
//		  if (!sprite->completely_inside_leader()) {
//			  add(sprite,signal_screen_change);
////		  } else if (signal_screen_change) {
////			  sprite->recursively_notice_on_screen_change(TRUE);
//		  };
//		  remaining = remaining->rest();
//		};
//};
	if (cache_valid) {
		Sprite *on_top = first_cached_on_top_of(new_layer);
		if (on_top != NULL) {
			invalidate_cache();
#if TT_DEBUG_ON
			if (tt_debug_mode == 3) {
				new_layer->describe_briefly(tt_error_file());
				tt_error_file() << " (" << type_name(new_layer) << "#" << new_layer->debug_counter << ")"
								<< " was added and is under ";
				on_top->describe_briefly(tt_error_file());
				tt_error_file() << " (" << type_name(on_top) << "#" << on_top->debug_counter << ")"
								<< " which was cached." << endl;
			};
#endif
		};
	};
	return(TRUE);
};
/* old behavior (prior to 080201) was to signal an error if too many layers
if (new_layer->pointer_to_leader() == NULL) {
			// following moved here since now try to recover from having too many layers
			if (!said_too_many_things) { // conditional added on 150400 so warning isn't repeated - does this really work??
				say_error(IDS_TOO_MANY_THINGS_ON_SCREEN,TRUE);
				said_too_many_things = TRUE;
			};
		   // this isn't a great solution since item removed won't
		   // be on screen if moved but better than ignoring new_layer
		   TTRegion layer_region;
		   for (int i = 0; i < max_number_of_layers; i++) {
            if (layers[i]->pointer_to_leader() != NULL) { // the truck or whatever will be good enuf
               remove(layers[i]);
               return(add(new_layer,signal_screen_change));
            } else if (layers[i]->kind_of() != PROGRAMMER) {
               layers[i]->true_region(layer_region);
			      if (!layer_region.overlaps(view_region)) {
				      remove(layers[i]);
				      // since remove changes layers this is safe way to deal with this
				      return(add(new_layer,signal_screen_change));
               };
            };
			};
		};
		// moved following here from inside conditional above (on 080900)
		if (new_layer->kind_of() == PROGRAMMER || // better add him/her no matter what
			 new_layer->kind_of() == MARTIAN || // new on 070900
			 new_layer->kind_of() == TALK_BALLOON) { // new on 080900
         remove(layers[0]);
         return(add(new_layer,signal_screen_change));
      };
		return(FALSE);
		*/

void Screen::remove(Sprite *old_layer) {
  // does nothing if old_layer isn't there
  for (int i = 0; i < layer_count; i++) {
	  if (layers[i] == old_layer) { // same pointer
#if TT_DEBUG_ON
		  debug_log(old_layer->debug_counter,"Removing debug target from screen.");
#endif		  
		  if (old_layer->cached()) {
			  invalidate_cache();
#if TT_DEBUG_ON
			  if (tt_debug_mode == 3) {
				  old_layer->describe_briefly(tt_error_file());
				  tt_error_file() << " (" << type_name(old_layer) << "#" << old_layer->debug_counter << ")"
										<< " was removed"
										<< " and was cached." << endl;
			  };
#endif
//			  old_layer->set_cached(FALSE);
//	  		  old_layer->set_followers_cached(FALSE);
		  };
		  for (int j = i+1; j < layer_count; j++) {
			  layers[j-1] = layers[j];
		  };
		  layer_count--;
		  old_layer->notice_on_screen_change(FALSE);
//		  Sprite *old_leader = old_layer->pointer_to_leader();
//		  if (old_leader != NULL) {
//			  old_layer->set_screen_ref_count(old_leader->current_screen_ref_count());
//		  } else {
//			  old_layer->decrement_screen_ref_count();
//		  };
//		  layers[i]->set_clean_status(DIRTY); // in case its been cached
//		  if (!old_layer->followers_completely_inside()) {
			  Sprites *remaining = old_layer->pointer_to_followers();
			  while (remaining != NULL) {
				  Sprite *sprite = remaining->first();
				  if (!sprite->completely_inside_leader()) {
					  remove(sprite);
				  } else {
					  sprite->recursively_notice_on_screen_change(FALSE);
//				  } else if (old_leader == NULL) {
//					  sprite->recursively_decrement_screen_ref_count();
//				  } else {
//					  sprite->recursively_set_screen_ref_count(old_leader->current_screen_ref_count());
				  };
				  remaining = remaining->rest();
			  };
//		  };
		  return;
	  };
  };
};

boolean Screen::showing(Sprite *item) {
	for (int i = 0; i < layer_count; i++) {
		if (layers[i] == item) return(TRUE); 
	};
	return(FALSE);
};

#if TT_DEBUG_ON // made conditional on 100103
millisecond milliseconds_to_copypage = 0;
millisecond milliseconds_to_draw_background = 0;
long timer_start;
long display_start;
#endif

//boolean minimum_count_before_caching = 3;
boolean Screen::update_display() {
#if TT_DEBUG_ON
  display_start = timeGetTime(); // for performance measurement
#endif
  boolean clean_background = background->update_display(&view_region);
#if TT_DEBUG_ON
  if (tt_debug_mode == 3) {
	  tt_error_file() << (timeGetTime()-display_start)
							<< " ms to update display of background." << endl; 
  };
#endif
  return(clean_background);
};

void Screen::propagate_changes() {
	if (background == NULL) return; // just to be safe - new on 200703
#if TT_DEBUG_ON
	millisecond time = timeGetTime();
#endif
	background->propagate_changes();
#if TT_DEBUG_ON
	if (tt_debug_mode == 3) {
		tt_error_file() << (timeGetTime()-time)
							 << " ms to propagate changes." << endl; 
	};
#endif
};

void Screen::new_view(CameraStatus status) {
//	if (view_status == CAMERA_ABOVE) {
//		// last chance to run all this for possibly a long time
//		tt_finish_all_animations = TRUE;
//		background->finish_instantly();
//	};
	view_status = status;
};

millisecond time_display_started;
#if TT_DIRECTX
long copy_cache_back_on_frame = 0;
#endif
boolean displayed_something = FALSE; // maybe only direct_draw cares

//#if TT_DEBUG_ON
//
//boolean repeat_next_frame = FALSE;
//
//void Screen::display_debug_info(millisecond current_delta) {
//	background->display_debug_info();
//	if (repeat_next_frame) {
//		// on replay these appear twice (end of old log and beginning of new log)
//		background->display_debug_info();
//	};
//	repeat_next_frame = (tt_debug_mode == 2308031 && tt_current_time+current_delta >= tt_next_new_log_time && !replaying());
//};
//#endif

void Screen::save_floor_priorities() { // new as of 300803
	// this could forward to the background instead
	for (int i = 0; i < layer_count; i++) {
		if (layers[i] != NULL) {
			// can it be otherwise?
			layers[i]->save_floor_priority();
		};
	};
};

void Screen::display(boolean clean_background) {
//tt_error_file() << "Screen display f=" << tt_frame_number << " ";
//view_region.print(tt_error_file());
//tt_error_file() << endl;
#if TT_DEBUG_ON
  if (tt_debug_mode == 3) {
	  tt_error_file() << "Display total was " << last_display_duration
							<< " which was "
							<< (100*last_display_duration)/tt_millisecond_delta
							<< "% of total frame time.  Frame number "
							<< tt_frame_number << endl;
	  total_display_duration += last_display_duration;
	  total_run_time += tt_millisecond_delta;
  };
  //if (tt_debug_mode == 121204) {
	 // clean_background = FALSE; // so caching doesn't confuse things
  //};
  add_to_log_if_interesting_error();
  millisecond time = timeGetTime();
#endif
  if (!clean_background) {
	  reset_grid();
	  screen_dirty();
#if TT_DEBUG_ON
	  if (tt_debug_mode == 3) {
		  tt_error_file() << "Background changed so cache invalidated." << endl;
	  };
#endif
  };
  if (tt_time_travel != TIME_TRAVEL_OFF) { // new on 240100
#if TT_DEBUG_ON
	  millisecond update_time;
	  if (tt_debug_mode == 3) {
		  update_time = timeGetTime();
	  };
#endif
	  update_time_travel_buttons(FALSE);
#if TT_DEBUG_ON
	  if (tt_debug_mode == 3) {
		  tt_error_file() << (timeGetTime()-update_time)
								<< " ms to update time travel buttons." << endl; 
	  };
#endif
  };
  update_layers();
#if TT_DEBUG_ON
  if (tt_debug_mode == 3) {
	  tt_error_file() << (timeGetTime()-time)
						   << " ms to update layers." << endl; 
  };
//  time = current_time(performance_timer);
#endif
  if (!cache_valid)  { // flush what is there
	  time_display_started = timeGetTime();
	  if (clean_background) { // haven't reset grid yet
		  reset_grid();
		  screen_dirty();
	  };
  };
#if TT_DEBUG_ON
  if (tt_debug_mode == 3) {
	  millisecond time = timeGetTime();
	  tt_error_file() << "Screen pre-display ms total elapsed was "
							<< (time-display_start) << endl;
	  display_start = time;
  };
  timer_start = timeGetTime();
#endif
  boolean display_background = !cache_valid;
  if (display_background && background != NULL) { // second condition added for robustness - interruption dialog can trigger this
//#if TT_DIRECTX
//     if (!background->display_region()) { // next frame won't change
//        if (tt_page_flipping && tt_screen_width != 320) {
//           copy_cache_back_on_frame = tt_frame_number+1;
//           copy_back_surface_to_cache_page();
//        };
//     };
//#else
     background->display_region();
//#endif
//#if TT_DIRECTX
//     copy_after_next_flip(); // wasteful if background will be displayed next frame too
//#endif
#if TT_DEBUG_ON
	  milliseconds_to_draw_background = timeGetTime()-timer_start;
	  if (tt_debug_mode == 3) {
		  tt_error_file() << milliseconds_to_draw_background
								<< " ms to draw background." << endl;
	  };
	  if (tt_debug_mode == 250404) {
		  tt_error_file() << "Displayed background on frame " << tt_frame_number << endl;
	  };
#endif
//#if TT_DIRECTX
//  } else if (copy_cache_back_on_frame == tt_frame_number &&
//             tt_page_flipping && tt_screen_width != 320) {
//    copy_cache_page_to_back_surface();
//     copy_cache_back_on_frame = 0; // shouldn't need to bother
//#endif
  };
//  if (tt_caching_background && clean_background) {
//	  cache_background();
//  };
#if TT_DEBUG_ON
  timer_start = timeGetTime();
#endif
  if (cache_valid) restore_regions(); // restore this page to how it was last cycle
#if TT_DEBUG_ON
  if (tt_debug_mode == 3) {
	  tt_error_file() << (timeGetTime()-timer_start)
							<< " ms to restore picture." << endl;
  };
#endif
  if (!cache_valid) tt_postpone_dirty_displays = FALSE;
//  Sprite *selection = current_selection;
//  long selection_priority;
//  if (selection != NULL) selection_priority = selection->priority();
  sort(); // do this as late as possible in case new items added by background
  int i;
  displayed_something = FALSE;
  if (tt_selection != NULL && tt_toolbox != NULL && tt_selection->pointer_to_leader() == tt_toolbox) {
     // a compartment of toolbox is selected
     tt_selection->display(MOVEMENT_FEEDBACK);
     TTRegion selection_region, layer_region;
     tt_selection->true_region(selection_region);
     for (i = 0; i < layer_count; i++) {
        layers[i]->true_region(layer_region);
        if (layer_region.overlaps(selection_region)) layers[i]->just_set_clean_status(DIRTY);
     };
  };
//#if TT_DEBUG_ON
//  	int house_count = 0;
//	int truck_count = 0;
//	int other_count = 0;
//#endif
  boolean toolbox_cached_background = FALSE;
  if (tt_toolbox != NULL) {
	  toolbox_cached_background = tt_toolbox->is_background_cached();
  };
  for (i = 0; i < layer_count; i++) {
#if TT_DEBUG_ON
	  if (tt_debug_target == layers[i]->debug_counter) {
		  tt_error_file() << "About to display debug target." << endl;
	  };
	  //if (tt_debug_mode == 141003) {
		 // switch (layers[i]->kind_of()) {
			//  case TRUCK:
			//	  truck_count++;
			//	  break;
			//  case HOUSE:
			//	  house_count++;
			//	  break;
			//  default:
			//	  other_count++;
		 // };
	  //};
#endif
	  layers[i]->clear_changes();
// Following tried to provide feedback at the level the item now is at
// It failed since caching assumes lower down things don't change
//	  if (selection != NULL && layers[i]->priority() < selection_priority) {
//		  selection->display(this,MOVEMENT_FEEDBACK,TRUE);
//		  selection = NULL;
//	  };
	  //if (toolbox_cached_background) {
		 // if (layers[i] == tt_toolbox) {
			//  toolbox_cached_background = FALSE;
		 // };
	  //};
	  //if (toolbox_cached_background) {
		 // // new on 111204 -- no need to display these since they are under Tooly and already cached
	  //} else 
	  if (layers[i]->clean_or_obscured()) {
		  if (cache_valid && layers[i]->cached()) {
			  // already cached so skip this guy
			  layers[i]->set_changes_propagated(FALSE); // reset leader flag
			  if (layers[i]->current_clean_status() == DIRTY_PART_OBSCURED) {
#if TT_DEBUG_ON
				  if (tt_debug_mode == 3) {
					  tt_error_file() << "Skipping dirty parts obscured: ";
					  layers[i]->describe_briefly(tt_error_file());
					  tt_error_file() << " (" << type_name(layers[i]) << "#" << layers[i]->debug_counter << ")" << endl;
				  };
#endif
// need to do this even if followers are clean since the leader
// may have been cached but the followers just turned clean
//			  if (layers[i]->current_clean_status() == FOLLOWERS_DIRTY) {
			  } else if (!layers[i]->followers_cached() &&
					       (layers[i]->pointer_to_followers() != NULL) &&  //  || tt_log_version_number <= 9
//                      layers[i]->visible() && -- experiment on 171202 -- just because leader is invisible doesn't mean followers are
							 !layers[i]->is_blank() && // new on 191200
							 layers[i]->kind_of() != CUBBY) { // new on 191200
				  cache_followers(layers[i],i);
			  };
		  } else if (layers[i]->current_clean_status() == DIRTY_BUT_OBSCURED ||
						 first_to_obscure(i) != NULL) {
#if TT_DEBUG_ON
			  if (tt_debug_mode == 3) {
				  tt_error_file() << "Skipping obscured: ";
				  layers[i]->describe_briefly(tt_error_file());
				  tt_error_file() << " (" << type_name(layers[i]) << "#" << layers[i]->debug_counter << ")" << endl;
			  };
#endif
			  layers[i]->set_changes_propagated(FALSE);
		  } else if (clean_background && cache_valid &&
                   layers[i]->ok_to_cache() &&
						 postponed_layer_underneath(layers[i]) == NULL) {
			  // can't cache this if someone underneath postponed			 
			  if (layers[i]->current_clean_status() == FOLLOWERS_DIRTY) {
//					layers[i]->kind_of() != PICTURE) { // since Pictures clip
				  if (layers[i]->ok_to_cache_without_followers()) {
#if TT_DEBUG_ON
					  if (tt_debug_mode == 3) {
						  tt_error_file() << "Caching without followers: ";
					  };
#endif
					  display_layer(layers[i],FALSE); // not followers
//               tt_error_file() << tt_frame_number << " " << i << " " << type_name(layers[i]) << " Caching 1" << endl;
					  layers[i]->set_cached_without_followers();
//					  if (layers[i]->visible()) { -- experiment on 171202 -- just because leader is invisible doesn't mean followers are
						  cache_followers(layers[i],i);
//					  };
				  } else {
					  postpone_layer(layers[i]);
				  };
			  } else {
#if TT_DEBUG_ON
				  if (tt_debug_mode == 3) {
					  tt_error_file() << "Caching: ";
				  };
				  if (tt_debug_target == layers[i]->debug_counter) {
					  log("Caching with following the debug target.");
				  };
#endif
              boolean saved_postpone_dirty_displays = tt_postpone_dirty_displays;
              tt_postpone_dirty_displays = FALSE;
				  display_layer(layers[i]);
//            tt_error_file() << tt_frame_number << " " << i << " " << type_name(layers[i]) << " Caching 2" << endl;
              tt_postpone_dirty_displays = saved_postpone_dirty_displays;
				  layers[i]->set_cached_with_followers();
			  };
		  } else { // clean but not ready to cache
//			  layers[i]->set_cached(FALSE);
//			  layers[i]->set_followers_cached(FALSE);
			  postpone_layer(layers[i]);
		  };
	  } else if (first_to_obscure(i) == NULL) { // condition new on 261004 -- prevents flickering as tools settle down under big things
//		  layers[i]->set_cached(FALSE);
//		  layers[i]->set_followers_cached(FALSE);
		  postpone_layer(layers[i]);
	  };
	  layers[i]->reset_clean_status();
  };
  // displayed all that is ok to cache
  if (displayed_something) {
     if (cache_valid) {
//#if TT_DIRECTX
//        reset_grid(FALSE);
//        if (tt_page_flipping && tt_screen_width != 320) {
//           copy_cache_back_on_frame = tt_frame_number+1;
//           copy_back_surface_to_cache_page();
//        };
//#else
        reset_grid();  // just reset it
//#endif
      };
  };
#if TT_DEBUG_ON
  add_to_log_if_interesting_error();
#endif
  if (!cache_valid) {
	  cache_valid = TRUE;
	  // average weighted 50% for last item; 25% for one before that, etc.
//	  tt_cache_duration
//		  = (tt_cache_duration+(tt_current_time-tt_last_time_cache_invalidated))/2;
	  tt_cache_invalidation_cost = (tt_cache_invalidation_cost+(timeGetTime()-time_display_started))/2;
//	  tt_last_time_cache_invalidated = tt_current_time;
  };
  tt_postpone_dirty_displays = FALSE;
  for (i = 0; i < postponed_count; i++) {
	  if (//postponed_layers[i]->clean_or_obscured() ||
			postponed_layers[i]->save_region()) {
		  display_layer(postponed_layers[i],TRUE,TRUE);
#if TT_DEBUG_ON
		  add_to_log_if_interesting_error();
#endif
//      tt_error_file() << tt_frame_number << " " << i << " " << type_name(postponed_layers[i]) << endl;
	  }; // otherwise dirty and offscreen
  };
#if TT_FUNCTION_KEY_HELP
  if (tt_function_key_help != NULL) { // new on 010305
	  update_function_key_help();
	  display_layer(tt_function_key_help);
  };
#endif
#if TT_CALL_FOR_MARTY
  if (tt_call_for_marty != NULL) { // new on 010305
	  update_call_for_marty();
	  if (tt_call_for_marty != NULL) { // above may have gotten rid of it
		  display_layer(tt_call_for_marty);
	  };
  };
#endif
  tt_postpone_dirty_displays = TRUE;
  postponed_count = 0;
  if (subtitle_stop_time > 0 && tt_current_time > subtitle_stop_time) { 
	  // prior to 101199 was (millisecond) timeGetTime() > subtitle_stop_time) {
     subtitle_stop_time = 0;
	  reset_grid();
     screen_dirty();
	  if (tt_toolbox != NULL) { // new on 250105
		  tt_toolbox->set_background_cached(FALSE);
	  };
#if TT_DEBUG_ON
     if (tt_debug_mode == 3) {
	     tt_error_file() << "Screen dirty since subtitle removed." << endl;
	  };
#endif
  } else { // else new on 250105 -- following was unconditional
	  display_subtitles();
  };
#if TT_DEBUG_ON
  add_to_log_if_interesting_error();
#endif
  if (tt_time_travel != TIME_TRAVEL_OFF) {
	  display_time_travel_buttons();
  };
#if TT_DEBUG_ON
  add_to_log_if_interesting_error();
  if (tt_debug_mode == 3) {
	  last_display_duration = (timeGetTime()-display_start);
  };
#endif
};

void Screen::display_subtitles() { // abtracted on 250105
	if (subtitle_stop_time <= 0 || subtitle_length == 0 || tt_current_time > subtitle_stop_time) return; 
	// second disjunct added 010505 -- third disjunct added 080705 since needed to undo fix of call to subtitle_need_not_wait_for_speech by Martian
	int number_of_lines = tt_main_window->show_subtitle(subtitle,subtitle_length,subtitle_character_width,subtitle_character_height);
//#if TT_DEBUG_ON
//		if (tt_debug_mode == 111203) {
//			string narrow = copy_narrow_string(subtitle,subtitle_length);
//			tt_error_file() << "Showing subtitle " << narrow << " at " << timeGetTime() << endl;
//			delete [] narrow;
//		};
//#endif
   //tt_dirty_min_x = 0;
	//tt_dirty_min_y = min(tt_dirty_min_y,tt_screen_height-subtitle_character_height); // 0; 
	// prior to 111104 was min(tt_dirty_min_y,tt_screen_height-subtitle_character_height)
	//tt_dirty_max_x = tt_screen_width-1;
	//tt_dirty_max_y = tt_screen_height-1; // subtitle_character_height*number_of_lines; // tt_screen_height-1;
	// rewrote the above on 111104 as
	TTRegion dirty_region(min_x(),max_x(),min_y(),min_y()+(subtitle_character_height*number_of_lines*scale)/ground_scale);		
	save_city_region(dirty_region);
};

void Screen::remove_current_subtitle() {
	subtitle_stop_time = 1; // changed from 0 on 250105 since when displayed this will trigger the right behaviour 
	// don't need the following either if this works OK
//		 invalidate_cache();
	screen_dirty(); // replaced above on 111104 since this does more to be sure nothing remains from earlier
};

void Screen::redisplay_above(Sprite *layer, TTRegion &region) {
	while (layer->completely_inside_leader() && layer->pointer_to_leader() != NULL) {
		layer = layer->pointer_to_leader();
	};
   int i;
	for (i = 0; i < layer_count; i++) {
		if (layers[i] == layer) break;
	};
	i++; // skip layer
//	TTRegion region;
//	layer->full_region(region);
   long layer_priority = layer->priority();
	for (; i < layer_count; i++) {
		if (layers[i]->priority() <= layer_priority && // should be sorted but just in case
          layers[i]->kind_of() != PROGRAMMER &&
			 layers[i]->kind_of() != MARTIAN &&
          layers[i]->kind_of() != TALK_BALLOON &&
			 layers[i]->kind_of() != ROBOT_IN_TRAINING &&
			 layers[i]->current_held_status() != HELD_BY_PROGRAMMER &&
			 (!layers[i]->part_of(layer) || layers[i]->to_be_smashed())) {
			TTRegion other_region;
			layers[i]->full_region(other_region);
			if (region.overlaps(other_region)) {
				layers[i]->display(NO_FEEDBACK,TRUE);
			};
		};
	};
};

//void Screen::display_layers_below(Sprite *sprite); // new on 111204
//	for (i = 0; i < layer_count; i++) {
//		if (layers[i] == sprite) return;
//		sprite->display(NO_FEEDBACK,TRUE);
//	};
//};

void Screen::invalidate_cache() {
	if (cache_valid) {
		cache_valid = FALSE;
		for (int i = 0; i < layer_count; i++) {
			layers[i]->recursively_reset_cache_status();
		};
	};
};

void Screen::cache_followers(Sprite *leader, int index) {
	Sprites *remaining = leader->pointer_to_followers();
	while (remaining != NULL) {
		Sprite *sprite = remaining->first();
		if (!sprite->screen_updates()) { // ignore since is a layer already
		} else if (// (!sprite->clean() && tt_log_version_number < 15) ||
		           !sprite->completely_clean()) { //  && tt_log_version_number >= 15)) {
			if (sprite->cached()) {
				invalidate_cache();
#if TT_DEBUG_ON
				if (tt_debug_mode == 3) {
					tt_error_file() << "Follower ";
					sprite->describe_briefly(tt_error_file());
					tt_error_file() << " (" << type_name(sprite) << "#" << sprite->debug_counter << ")"
						         << " "
									<< sprite->current_clean_status_name()
									<< " and was cached." << endl;
				};
#endif			
			};
//			sprite->set_cached(FALSE);
//			sprite->set_followers_cached(FALSE);
			postpone_layer(sprite);
			// new on 290399 - once I postpone a follower I must postpone all that follow
		   remaining = remaining->rest();
		   while (remaining != NULL) {
			   postpone_layer(remaining->first());
			   remaining = remaining->rest();
			};
		   return;
//		} else if (sprite->cached()) { // commented out on 261100 since redundant with the above
//			sprite->set_changes_propagated(FALSE);
//			if (!sprite->followers_cached()) {
//				cache_followers(sprite,index);
//			}; 
		} else if (first_to_obscure(index,sprite) != NULL) {	  
#if TT_DEBUG_ON
			if (tt_debug_mode == 3) {
				tt_error_file() << "Skipping follower obscured: ";
				sprite->describe_briefly(tt_error_file());
				tt_error_file() << " (" << type_name(sprite) << "#" << sprite->debug_counter << ")" << endl;
			};
#endif
		} else if (sprite->ok_to_cache()) {
//					  postponed_layer_underneath(sprite) == NULL) { // new on 6/14/98 otherwise z-ordering breaks for multiply compound pictures
#if TT_DEBUG_ON
		  if (tt_debug_mode == 3) {
			  tt_error_file() << "Caching follower: ";
		  }; 
#endif
		  if ((sprite->completely_clean() || sprite->is_blank() || sprite->kind_of() == CUBBY) && sprite->has_followers()) { // cubby and blank tests added 191200
//				sprite->pointer_to_followers() != NULL // this was replaced on 110200 with the above which does the right thing for remote looks
			  display_layer(sprite,TRUE,TRUE);
//         tt_error_file() << tt_frame_number << " " << -1 << " " << type_name(sprite) << " Caching 3" << endl;
			  sprite->set_cached_with_followers();
		  } else {
			  display_layer(sprite,FALSE,TRUE);
//         tt_error_file() << tt_frame_number << " " << -1 << " " << type_name(sprite) << " Caching 4" << endl;
			  cache_followers(sprite,index);
		  };
		  sprite->set_cached_without_followers();
	  } else { 
//		  sprite->set_cached(FALSE);
//		  sprite->set_followers_cached(FALSE);                       
		  postpone_layer(sprite);
		  // new on 290399 - once I postpone a follower I must postpone all that follow
		  remaining = remaining->rest();
		  while (remaining != NULL) {
			  postpone_layer(remaining->first());
			  remaining = remaining->rest();
		  };
		  return;
	  };
	  remaining = remaining->rest();
	};
};

void Screen::display_layer(Sprite *sprite, boolean followers_too, boolean out_of_order) {
#if TT_DEBUG_ON
	if (sprite->debug_counter == tt_debug_target) {
		log("display_layer of debug target");
	};
	if (tt_debug_mode == 250404) {
		tt_error_file() << "Displayed ";
		sprite->print(tt_error_file());
		tt_error_file() << " on frame " << tt_frame_number << endl;
	};
#endif
	boolean clipped = FALSE;
   TTRegion saved_clip_region;
//	if (sprite->current_width() < scale || sprite->current_height() < scale) return;
	if (out_of_order && sprite->kind_of() == PICTURE) {
		Sprite *leader = sprite->pointer_to_leader();
		if (leader != NULL &&
			 ((Picture *) sprite)->subpicture() && // could be follower before mouse smashes it
			 leader->kind_of() == PICTURE) {
			// pictures clip their followers
			// postpone breaks this -- this fixes it
         // but first save clip region to restore it
         tt_main_window->current_clip_region(saved_clip_region);
			leader->set_clip_region();
			clipped = TRUE;
		};
	};
	millisecond start = timeGetTime();
   SelectionFeedback feedback;
   if (tt_selection != NULL && (sprite == tt_selection || sprite->has_leader(tt_selection))) {
      feedback = MOVEMENT_FEEDBACK;
   } else {
      feedback = NO_FEEDBACK;
   };
	sprite->display(feedback,followers_too);
   displayed_something = TRUE;
	if (clipped) {
      //unclip();
      tt_main_window->restore_clip_region(saved_clip_region);
   };
	millisecond duration = timeGetTime()-start;
	if (duration > 255) {
		sprite->set_display_cost(255);
	} else if (duration == 0) { // so it'll get cached eventually
		sprite->set_display_cost(1);
	} else {
		sprite->set_display_cost((cost) duration);
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 1) {
		TTRegion region;
		sprite->full_collision_region(region);
#if WING||TT_DIRECTX
BEGIN_GDI
		tt_main_window->select_stock_object(NULL_BRUSH);
		tt_main_window->select_stock_object(WHITE_PEN);
		draw_rectangle(region.min_x,region.max_x,region.min_y,region.max_y);
		tt_main_window->select_stock_object(NULL_PEN); // default
END_GDI
#else
		draw_rectangle(RED,region.min_x,region.max_x,region.min_y,region.max_y);
#endif
	};
#if WING||TT_DIRECTX
	if (tt_debug_mode == 111) {
	  if (sprite->graspable()) {
		  TTRegion region;
		  sprite->active_region(region);
BEGIN_GDI
		  tt_main_window->select_stock_object(NULL_BRUSH);
		  tt_main_window->select_stock_object(WHITE_PEN);
		  draw_rectangle(region.min_x,region.max_x,region.min_y,region.max_y);
		  tt_main_window->select_stock_object(NULL_PEN); // default
END_GDI
		};
	};
#endif
#endif
};

//void Screen::remove_subtitle() { // new on 070401 
//	subtitle_stop_time = 0; // if travelling backward in time this will work while tt_current_time won't -- 261003
////	subtitle_stop_time = tt_current_time;
//};

void Screen::subtitle_need_not_wait_for_speech() {
	duration_of_current_subtitle = 0; // new on 281004 since -1 means waiting for text to speech to finish
};

void Screen::set_subtitle(string line, short int length, millisecond duration) {
	for (int i = 0; i < length; i++) { // new on 100204 to permit blank (invisible) lines
		if (line[i] != ' ') break;
		if (i+1 == length) { // all white space so ignore
			return;
		};
	};
   millisecond now = tt_current_time; // was timeGetTime() prior to 101199
#if TT_DEBUG_ON
	if (tt_debug_mode == 44444 || tt_debug_mode == 221103 || tt_debug_mode == 290204) {
		tt_error_file() << "Starting to display the following subtitle at " << timeGetTime() << "ms." << endl;
		tt_error_file().write(line,length);
		tt_error_file() << " for " << duration << "ms" << endl;
	};
#endif // moved here on 130205 -- temporarilly????
	boolean schedule_problem = FALSE;
	if (duration > 0) { // otherwise is put up until explictly removed
		schedule_problem = (subtitle_stop_time > now+100); // starting a new subtitle with more than .1 seconds left to the current one
		if (schedule_problem) {
			string narrow_subtitle = copy_narrow_string(subtitle);
			tt_error_file() << "Subtitle: " << narrow_subtitle << " was scheduled to stop at " << subtitle_stop_time << endl;
			delete [] narrow_subtitle;
		};
	};
//#endif
//   if (subtitle_length > 0) {
      // save the old one
//      strcpy(previous_subtitle,subtitle);
//      previous_subtitle_length = subtitle_length;
//      blackened_count = 0;
//  };
	int max_length = max_fat_characters_per_subtitle_line*2;
	if (tt_good_sizes_adjust_to_screen_size) {
		max_length = grow_width_to_640x480_screen_size(max_length);
	};
	wide_string wide_line = copy_wide_string(line,length);
	length = wide_string_length(wide_line);
	/* commented out on 110401 since confusing when describing a picture from a file since the name is a parenthetical remark
	if (length > max_length) {
		wide_string wide_line_without_parenthetical_remark = remove_parenthetical_remark(wide_line);
		if (wide_line_without_parenthetical_remark != NULL) {
			delete [] wide_line;
			wide_line = wide_line_without_parenthetical_remark;
			length = wide_string_length(wide_line);
		};
//		if (length > max_length) {
//			length = last_space_or_punctuation(wide_line,max_length-2);
//			if (length < 0) { // didn't find one
//				length = max_length-2;
//			} else {
//				length++; // zero-indexing
//			};
//		};
	};
	*/
	// following new on 040503 since 500 is the max size of the subtitle - replace with constant
	if (length-3 >= subtitle_max_size) { // prior to 110505 was -2 but then clobbered one byte too many below
		length = subtitle_max_size-3; // prior to 110505 was -2
	};
   memcpy(subtitle+1,wide_line,length*sizeof(wide_character)); // leading blank left alone
	delete [] wide_line;
	if (need_wide_characters()) {
		subtitle[length+1] = wide_space; // add trailing blank
	} else {
		subtitle[length+1] = ' '; // add trailing blank
	};
   subtitle[length+2] = NULL;
   subtitle_length = (short int) (length+2); // space padding
//   if (subtitle_stop_time != 0) {
      // replacing old one
//      reset_grid();
//      tt_screen->screen_dirty(); 
//#if TT_DEBUG_ON
//	  if (tt_debug_mode == 3) {
//		  tt_error_file() << "Cache invalidated since replacing old subtitle." << endl;
//	  };
//#endif
//   };
//   if (subtitle_stop_time == 0) { // not currently showing subtitle
      // save the entire subtitle region
//      save_region(pixel_min_x,pixel_max_x,
//                  pixel_max_y-subtitle_character_height,pixel_max_y);
//   };
	if (length == 0) {
		subtitle_stop_time = now;
//		screen_dirty();
	} else if (duration > 0) {
		subtitle_stop_time = now+duration; // was tt_current_time+duration;
		duration_of_current_subtitle = duration;
	} else {
		subtitle_stop_time = now+100000; 
		// get rid of after 100 seconds if something goes wrong - was 10 seconds prior to 070401 but some sentences 
		// (especially with numbers) took too long - subtitle went away and next sentence got confused
		duration_of_current_subtitle = duration;
	};
//#if TT_DEBUG_ON
	if (schedule_problem) {
		tt_error_file() << "But subtitle " << line << " set at " << now << endl;
	};
#if TT_DEBUG_ON	 // moved here on 130205 -- temporarilly????
   if (tt_debug_mode == 5050) {
      tt_error_file() << timeGetTime() << "+" << duration << " " << line << " frame=" << tt_frame_number << endl;
   };
//	if (tt_debug_mode == 5151) {
//		wide_string wide_line = copy_wide_string(line);
//      tt_error_file() << timeGetTime() << "+" << duration << " ";
//		tt_error_file().write(wide_line,wide_string_length(wide_line));
//		tt_error_file() << " frame=" << tt_frame_number << endl;
//		delete [] wide_line;
//	};
#endif
};

boolean Screen::displaying_subtitle(boolean only_if_unscheduled) {
	// flag is so that subtitled demos don't interfere
	if (subtitle_stop_time == 0) {
		return(FALSE); // new on 220405
	} else if (tt_current_time > subtitle_stop_time) {
		return(only_if_unscheduled && duration_of_current_subtitle < 0); 
		// was FALSE prior to 281004 and so sometimes Marty left while still speaking
	} else if (only_if_unscheduled) {
		return(duration_of_current_subtitle < 0);
	} else {
		return(TRUE);
	};
};

boolean Screen::save_city_region(TTRegion &region) {
//	save_city_region(region.min_x,region.min_y,
//						  horizontal_pixel_length(region.max_x-region.min_x),
//						  vertical_pixel_length(region.max_y-region.min_y));
//};

//void Screen::save_city_region(city_coordinate layer_llx,
//										city_coordinate layer_lly,
//										int layer_width, int layer_height) {
//#if TT_DEBUG_ON
//  timer_start = current_time(performance_timer);
//#endif
  dimension region_min_x = screen_x(region.min_x);
  dimension region_max_x = screen_x(region.max_x);
  dimension region_min_y = screen_y(region.min_y);
  dimension region_max_y = screen_y(region.max_y); 
  // clip to screen bounding box
  return(save_region(max(region_min_x,pixel_min_x), min(region_max_x,pixel_max_x),
							max(region_min_y,pixel_min_y), min(region_max_y,pixel_max_y)));
//#if TT_DEBUG_ON
//  if (tt_debug_mode == 3) {
//	  long duration = current_time(performance_timer)-timer_start;
//	  if (duration > 0) {
//		  tt_error_file() << duration
//						  << " ms to save region "
//						  << max(region_min_x,pixel_min_x) << "," 
//						  << max(region_min_y,pixel_min_y) << " "
//						  << min(region_max_x,pixel_max_x) << "," 
//						  << min(region_max_y,pixel_max_y) << " of ";
//	  };
//  };
//#endif
};

void Screen::save_city_region(TTRegion &region, bytes memory) {
  dimension region_min_x = screen_x(region.min_x);
  dimension region_max_x = screen_x(region.max_x);
  dimension region_min_y = screen_y(region.min_y);
  dimension region_max_y = screen_y(region.max_y); 
  // clip to screen bounding box
  save_region_to_memory(max(region_min_x,pixel_min_x), min(region_max_x,pixel_max_x),
								max(region_min_y,pixel_min_y), min(region_max_y,pixel_max_y),
								memory);
};

void Screen::restore_city_region(TTRegion &region, bytes memory) {
  dimension region_min_x = screen_x(region.min_x);
  dimension region_max_x = screen_x(region.max_x);
  dimension region_min_y = screen_y(region.min_y);
  dimension region_max_y = screen_y(region.max_y); 
  // clip to screen bounding box
  restore_region_from_memory(max(region_min_x,pixel_min_x), min(region_max_x,pixel_max_x),
									  max(region_min_y,pixel_min_y), min(region_max_y,pixel_max_y),
									  memory);
};

long Screen::city_region_pixel_size(TTRegion &region) { // new on 250404
   // just finding out how big memory should be for a buffer
	dimension region_min_x = screen_x(region.min_x);
   dimension region_max_x = screen_x(region.max_x);
   dimension region_min_y = screen_y(region.min_y);
   dimension region_max_y = screen_y(region.max_y);
	coordinate width = 1+region_max_x-region_min_x;
	coordinate padded_width = width;
	if (padded_width&3) { // low order bits set
		// adjust to long boundary
		padded_width += 4-padded_width%4;
	};
   return(padded_width*(1+region_max_y-region_min_y)*tt_bytes_per_pixel);
};

void Screen::set_cursor_position(city_coordinate x, city_coordinate y) { // new on 290404
	tt_main_window->set_cursor_position(screen_x(x),screen_y(y));
};

boolean Screen::region_is_free(TTRegion *region, Sprite *exception) { // abstracted and improved on 050404
	if (region->min_y >= background->wall_lly()) return(FALSE); // in the wall of the room
//	return(first_tool_to_collide_with(region,exception) == NULL); // rewritten on 050404 since this isn't quite the right functionality
	// e.g. handling of selectable and non-contained followers
	TTRegion layer_region;
	for (int i = 0; i < layer_count; i++) {
      Sprite *candidate = layers[i];
      if (candidate != NULL && candidate != exception && candidate->kind_of() != PROGRAMMER) {
		   candidate->full_region(layer_region);
		   if (layer_region.overlaps(region)) {
            return(FALSE); // was layers[i] prior to 010399
         };
		};
	};
	return(TRUE); // none found
};

Sprite *Screen::first_to_contain(city_coordinate x, city_coordinate y) {
	TTRegion layer_region;
	for (int i = layer_count-1; i>= 0; i--) { // from front to back
		layers[i]->full_region(layer_region); // was collision_region
		if (layer_region.inside(x,y)) {
			return(layers[i]);
		};
	};
	return(NULL); // none found
};

Sprites *Screen::all_to_collide_with(TTRegion *region) {
   // this reports even invisible sprites because programmer needs to deal with them
	TTRegion layer_region;
	Sprites *so_far = NULL;
	for (int i = 0; i < layer_count; i++) {
//      if (tt_log_version_number >= 7) {
		   layers[i]->full_collision_region(layer_region);
//      } else {
//         layers[i]->collision_region(layer_region);
//      };
		if (layer_region.overlaps(region)) {
			so_far = new Sprites(layers[i],so_far);
		};
	};
	return(so_far);
};

Sprite *Screen::first_to_collide_with(Sprite *layer, Sprite *exception,
												  PredicateCallBack predicate, void *predicate_arg) {
	int i;
	for (i = layer_count-1; i>0; i--) { // from front to back
		if (layers[i] == layer) break; // found the layer
	};
	for (; i>=0; i--) { // continue to find someone colliding except exception
		if (layers[i] != exception && layer->colliding(layers[i]) &&
			 (predicate == NULL || predicate(layers[i],predicate_arg))) {
			return(layers[i]);
		};
	};
	return(NULL); // none found
};

Sprite *Screen::first_to_collide_with(Sprite *layer, long min_priority,
												  PredicateCallBack predicate, void *predicate_arg) {
	for (int i = layer_count-1; i>=0; i--) { // from front to back
		if (layers[i] != layer && layers[i]->priority() > min_priority &&
			 layer->colliding(layers[i]) && !layers[i]->part_of(layer) &&
			 (predicate == NULL || predicate(layers[i],predicate_arg))) {
			return(layers[i]);
		};
	};
	return(NULL); // none found
};

Sprite *Screen::next_to_collide_with(Sprite *layer, Sprite *below_me,
												 PredicateCallBack predicate, void *predicate_arg) {
   int i;
	for (i = layer_count-1; i >= 0; i--) {
		if (layers[i] == below_me) {
			i--; // continue below that
			break;
		};
	};
	for (; i >= 0; i--) { // from front to back
		if (layers[i] != layer && layer->colliding(layers[i]) &&
		    (predicate == NULL || predicate(layers[i],predicate_arg))) {
			return(layers[i]);
		};
	};
	return(NULL); // none found
};

Sprite *Screen::next_tool_to_collide_with(TTRegion *region, Sprite *exception,
														Sprite *start_after, PredicateCallBack predicate, void *predicate_arg) {
	int i = layer_count-1;
	if (start_after != NULL) {
		// first find out where to start 
		for (; i >= 0; i--) { // from front to back
			if (layers[i] == start_after) {
				i--;
				break;
			};
		};
	};
	TTRegion layer_region;
	for (; i >= 0; i--) {
      Sprite *candidate = layers[i];
		while (candidate != NULL && !candidate->selectable()) {
			if (candidate->visible()) { 
				// new policy on 1/31/98 that can grasp leader of a non-selectable item
				// added visible() test on 300704 so if Marty's talk balloon is hidden doesn't count
				candidate = candidate->pointer_to_leader();
			} else {
				candidate = NULL;
			};
      };
      if (candidate != NULL && candidate != exception) {
		   layers[i]->collision_region(layer_region); // why isn't this candidate-> -- asked 060704 -
			// because used if dealing with grabbing a tool of a robot and getting the robot (300704)
		   if (layer_region.overlaps(region) && //is_tool(layers[i]->kind_of()) &&
             (predicate == NULL || predicate(candidate,predicate_arg))) {
            return(candidate); // was layers[i] prior to 010399
         };
		};
	};
	return(NULL); // none found
};

Sprite *Screen::next_tool_true_region_to_collide_with(TTRegion *region, Sprite *exception,
																		Sprite *start_after, PredicateCallBack predicate, void *predicate_arg) {
	int i = layer_count-1;
	if (start_after != NULL) {
		// first find out where to start 
		for (; i >= 0; i--) { // from front to back
			if (layers[i] == start_after) {
				i--;
				break;
			};
		};
	};
	TTRegion layer_region;
	for (; i >= 0; i--) {
		layers[i]->true_region(layer_region);
		if (layer_region.overlaps(region) && //is_tool(layers[i]->kind_of()) &&
			 layers[i]->selectable() &&
          layers[i] != exception &&
			 (predicate == NULL || predicate(layers[i],predicate_arg))) {
			return(layers[i]);
		};
	};
	return(NULL); // none found
};

Sprite *Screen::next_tool_full_collision_region_to_collide_with(TTRegion *region, Sprite *exception,
																		          Sprite *start_after, PredicateCallBack predicate, void *predicate_arg,
																					 boolean convert_to_picture_if_appropriate,
																					 boolean loop_around_if_not_found) {
	// convert_to_picture_if_appropriate new on 250103 so that can collide with text and number pads and not care if they once had been flipped
	// loop_around_if_not_found added on 290105 since appropriate for detecting collisions but not uncolliding
	int i = layer_count-1;
	if (start_after != NULL) {
		// first find out where to start 
		for (; i >= 0; i--) { // from front to back
			if (layers[i] == start_after) {
				i--;
				break;
			};
		};
	};
	TTRegion layer_region;
//	for (; i >= 0; i--) {
	// updated 251104 to cycle around if nothing found
	boolean first_round = TRUE;
	int starting_i = i;
	while (TRUE) {
		if (i < 0) {
			if (first_round && loop_around_if_not_found) {
				if (tt_log_version_number <= 60) break; // old demos didn't keep trying
				first_round = FALSE;
				i = layer_count-1;
			} else {
				break; // new on 310105
			};
		};
		if (i == starting_i && !first_round) { // commented out the following on 310105 || !loop_around_if_not_found)) {
			break; // give up
		};
		if (layers[i]->selectable() &&
          layers[i]->visible() && // added 10/16/97
//          (layers[i]->show_all() || layers[i]->pointer_to_followers() == NULL) &&
          layers[i] != exception &&
			 !layers[i]->held() && // new on 181004 since otherwise this creates a ghost and confusion if converted to a picture
			 (predicate == NULL || predicate(layers[i],predicate_arg))) {
			// moved the following test on 080900 here since it is more costly than the conditions above
			layers[i]->full_collision_region(layer_region);
			if (layer_region.overlaps(region)) { //is_tool(layers[i]->kind_of()) &&
				if (convert_to_picture_if_appropriate && (layers[i]->kind_of() == TEXT || layers[i]->kind_of() == INTEGER)) {
					make_indirect_picture(layers[i],TRUE);
				};
				return(layers[i]);
			};
		};
		i--;
	};
	// following is new on 080900 so that Hit Who sensor shows collisions with what is in your hand
	Sprite *tool_in_hand = tt_programmer->pointer_to_tool_in_hand();
	if (tool_in_hand != NULL && tool_in_hand != exception && 
		 tool_in_hand != start_after && !tool_in_hand->is_tool() && 
		 // added above conditions on 190201 since can cause a loop otherwise and tools shouldn't matter
       tool_in_hand->visible() && (predicate == NULL || predicate(tool_in_hand,predicate_arg))) {
		tool_in_hand->full_collision_region(layer_region);
		if (layer_region.overlaps(region)) {
			if (convert_to_picture_if_appropriate && (tool_in_hand->kind_of() == TEXT || tool_in_hand->kind_of() == INTEGER)) { 
				// && tool_in_hand->remote_status() == NOT_REMOTE) {
				// added on 120105 check for remote controls since it is too confusing to make them indirections -- 
				// but too hard to avoid -- e.g. dropping them on a picture
				Picture *picture = make_indirect_picture(tool_in_hand,TRUE);
				tt_programmer->set_tool_in_hand(picture,TRUE);
			};
			return(tool_in_hand);
		};
	};
	return(NULL); // none found
};

Sprite *Screen::first_to_collide_with(TTRegion *region, Sprite *underneath, PredicateCallBack predicate, void *predicate_arg) {
	int i = layer_count-1;
	if (underneath != NULL) {
		// first find out where to start (under underneath)
		for (; i > 0; i--) { // from front to back
			if (layers[i] == underneath) {
				i--;
				break;
			};
		};
	};
	TTRegion layer_region;
	for (; i >= 0; i--) {
		layers[i]->collision_region(layer_region);
		if (layer_region.overlaps(region) && 
			 (predicate == NULL || predicate(layers[i],predicate_arg))) {
			return(layers[i]);
		};
	};
	return(NULL); // none found
};

Sprite *Screen::first_of_type_to_collide_with(TTRegion *region, SpriteType type,
												          PredicateCallBack predicate, void *predicate_arg) {
	TTRegion layer_region;
	int i = layer_count-1;
	for (; i >= 0; i--) {
		if (layers[i]->kind_of() == type) {
			layers[i]->collision_region(layer_region);
			if (layer_region.overlaps(region) && 
				 (predicate == NULL || predicate(layers[i],predicate_arg))) {
				return(layers[i]);
			};
		};
	};
	return(NULL); // none found
};

Sprite *Screen::first_cached_to_overlap_with(Sprite *sprite) {
	TTRegion region,layer_region;
	sprite->true_region(region);
	for (int i = layer_count-1; i >= 0; i--) {
		if (layers[i] != sprite) {
			layers[i]->true_region(layer_region);
			if (layer_region.overlaps(region)) {
				return(layers[i]);
			};
		};
	};
	return(NULL); // none found
};

Sprite *Screen::first_cached_on_top_of(Sprite *sprite) {
	TTRegion region,layer_region;
	sprite->true_region(region);
	long sprite_priority = sprite->priority();
	for (int i = layer_count-1; i >= 0; i--) {
		if (layers[i] != sprite && layers[i]->cached() && 
			 // need to check since this is called before update_layers
			 layers[i]->priority() <= sprite_priority) {
			layers[i]->true_region(layer_region);
			if (layer_region.overlaps(region)) {
				return(layers[i]);
			};
		};
	};
	return(NULL); // none found
};

Sprite *Screen::postponed_layer_underneath(Sprite *layer) {
	// check if its even worth computing region first
	if (postponed_count == 0) return(NULL);
	TTRegion region,postponed_region;
	layer->true_region(region);
	for (int i = 0; i < postponed_count; i++) {
	  postponed_layers[i]->true_region(postponed_region);
	  if (region.overlaps(postponed_region)) {
		  return(postponed_layers[i]);
	  };
	};
	return(NULL);
};

Sprite *Screen::first_to_obscure(int index, Sprite *follower) {
	TTRegion region,layer_region;
	if (index >= 0) {
		if (follower == NULL) {
			layers[index]->true_region(region);
		} else {
			follower->true_region(region);
		};
		// clip region to screen's view region
		if (region.min_x < view_region.min_x) region.min_x = view_region.min_x;
		if (region.max_x > view_region.max_x) region.max_x = view_region.max_x;
		if (region.min_y < view_region.min_y) region.min_y = view_region.min_y;
		if (region.max_y > view_region.max_y) region.max_y = view_region.max_y;
	} else { // whole screen
		region = view_region;
      index = 0;
	};
	for (int i = layer_count-1; i > index; i--) {
		if (layers[i]->rectangular()) {
			layers[i]->true_region(layer_region); // was full_region prior to 260499 but that incorrectly claimed that decoration sensors obscured things
			if (layer_region.contains(region)) {
				return(layers[i]);
			};
		};
	};
	return(NULL); // none found
};

/*
Sprite *Screen::first_game_element_to_collide_with(TTRegion *region, 
																	Sprite *exception) {
  TTRegion layer_region;
  for (int i = layer_count-1; i > 0; i--) {
		if (layers[i]->is_game_element() && layers[i] != exception) {
			layers[i]->collision_region(layer_region);
			if (layer_region.overlaps(region)) {
				return(layers[i]);
			};
		};
	};
	return(NULL); // none found
};
*/

void Screen::update_layers() {
  // check validity of cache
  for (int i = 0; i < layer_count; i++) {
	  if (!layers[i]->completely_clean()) {
		  // first see if this dirty layer can be ignored because it is obscured
		  TTRegion dirty_region,other_region;
		  layers[i]->dirty_region(dirty_region);
		  for (int j = i+1; j < layer_count; j++) {
			  layers[j]->true_region(other_region);
			  if (layers[j]->rectangular() &&
					layers[j]->pointer_to_leader() == NULL) { // top level item
				  // later should support inner box of non-rectangular guys
				  if (other_region.contains(dirty_region)) {
					  layers[i]->set_clean_status(DIRTY_PART_OBSCURED,FALSE);
					  break;
				  };
				  layers[i]->true_region(dirty_region);
				  if (other_region.contains(dirty_region)) {
					  layers[i]->set_clean_status(DIRTY_BUT_OBSCURED,FALSE);
					  break;
				  };
			  };
			  if (layers[j]->clean()) {
				  if (other_region.overlaps(dirty_region)) {
					  // partially obscures a dirty guy below	
					  layers[j]->set_clean_status(BELOW_DIRTY,FALSE);
					  // otherwise will keep repeating this "mistake"
					  layers[j]->zero_clean_run();
					  if (cache_valid && layers[j]->cached()) {
#if TT_DEBUG_ON
						  if (tt_debug_mode == 3) {
							   layers[j]->describe_briefly(tt_error_file());
								tt_error_file() << " (" << type_name(layers[i]) << "#" << layers[i]->debug_counter << ")"
									         << " "
												<< layers[j]->current_clean_status_name()
												<< " and was cached." << endl;
						  };
#endif
						  invalidate_cache();
					  };
				  };
				  // leave it CLEAN
//			     layers[j]->set_clean_status(BELOW_DIRTY_BUT_OBSCURED,FALSE);
			  };
		  };
		  if (cache_valid) {
			  switch (layers[i]->current_clean_status()) {
				  case FOLLOWERS_DIRTY:
					  check_out_followers(layers[i]);
					  break;
				  case DIRTY_BUT_OBSCURED:
				  case DIRTY_PART_OBSCURED:
					  break; // nothing to do
				  default:
					  if (layers[i]->cached()) {
						  // still dirty but had been cached	
#if TT_DEBUG_ON
						  if (tt_debug_mode == 3) {
							  layers[i]->describe_briefly(tt_error_file());
							  tt_error_file() << " (" << type_name(layers[i]) << "#" << layers[i]->debug_counter << ")"
								           << " "
											  << layers[i]->current_clean_status_name()
											  << " and was cached." << endl;
						  };
#endif
						  invalidate_cache();
					  };
			  };
		  };
	  };
  };
};

void Screen::sort() { // bubble sort
  for (int i = 0; i < layer_count; i++) {
	  if (i > 0 && layers[i-1]->priority() < layers[i]->priority()) {
		  // out of order so swap
		  Sprite *temp;
		  temp = layers[i-1];
		  layers[i-1] = layers[i];
		  layers[i] = temp;
		  if (i > 1) i -= 2; // might bubble further back
	  };
  };
};

void Screen::check_out_followers(Sprite *leader) {
	if (leader->followers_cached() && !leader->completely_clean()) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 3) {
			tt_error_file() << "Leader ";
			leader->describe_briefly(tt_error_file());
			tt_error_file() << " (" << type_name(leader) << "#" << leader->debug_counter << ")"
				         << " "
							<< leader->current_clean_status_name()
							<< " and, along with followers, was cached." << endl;
		};
#endif
		invalidate_cache();		
		return;
	};
	// could be more clever if some but not all the followers
	// are covered to discover it now
	Sprites *remaining = leader->pointer_to_followers();
	while (remaining != NULL) {
		Sprite *sprite = remaining->first();
		if (sprite->cached() && sprite->screen_updates()) {
			switch (sprite->current_clean_status()) {
				case CLEAN:
				case DIRTY_BUT_OBSCURED:
				case DIRTY_PART_OBSCURED:
				  break; // do nothing
				case FOLLOWERS_DIRTY:
					check_out_followers(sprite);
					break;
				default:
//					sprite->set_cached(FALSE);
//					sprite->set_followers_cached(FALSE);
#if TT_DEBUG_ON
					if (tt_debug_mode == 3) {
						tt_error_file() << "Follower ";
						sprite->describe_briefly(tt_error_file());
						tt_error_file() << " (" << type_name(sprite) << "#" << sprite->debug_counter << ")" 
							         << " "
										<< sprite->current_clean_status_name()
										<< " and was cached." << endl;
					};
#endif
					invalidate_cache();
					break;
			};
		};
		remaining = remaining->rest();	  
	};
};

void Screen::postpone_layer(Sprite *layer) {
#if TT_DEBUG_ON
	if (layer->debug_counter == tt_debug_target) {
		log(_T("Postponing layer debug target."));
	};
#endif
	if (layer->visible() || layer->pointer_to_followers() != NULL || layer == tt_selection || layer->is_flipped()) { 
		// conditions new on 171202 except is_flipped which was added on 060805 since can be invisible and flipped
		// added layer == tt_selection so that invisible things can be selected
		if (postponed_count == max_number_of_postponed_layers) {
			// used to signal an error - it grows now
	//		say_error(IDS_DISPLAY_LOST_TRACK_OF_STUFF,TRUE);   
	//		return; // some stuff won't be displayed
			// new on 080201 to just grow the number of layers
			int new_max_number_of_postponed_layers = max_number_of_postponed_layers*2;
			SpritePointer *new_layers = new SpritePointer[new_max_number_of_postponed_layers];
			for (int i = 0; i < max_number_of_postponed_layers; i++) {
				new_layers[i] = postponed_layers[i];
			};
			delete [] postponed_layers;
			postponed_layers = new_layers;
			max_number_of_postponed_layers = new_max_number_of_postponed_layers;
		};
		postponed_layers[postponed_count++] = layer;
	};
};

void Screen::move_by(city_coordinate delta_x, city_coordinate delta_y) {
   update_viewing_region(center_x+delta_x, center_y+delta_y, scale);
};

city_coordinate Screen::from_user_x(user_coordinate x) { // were in screen.h (inlined) prior to 030302
//   return(x*(ideal_screen_width/1000));
	if (x > max_long/city_to_user_horizontal_units) { // to deal with overflow
		return(max_long);
	} else if (x < min_long/city_to_user_horizontal_units) {
		return(min_long);
	} else {
		return(x*city_to_user_horizontal_units);
	};
};

 city_coordinate Screen::from_user_y(user_coordinate y) {
//   return(y*(ideal_screen_height/1000));
	 // noticed on 040603 that the overflow code was using city_to_user_horizontal_units instead of city_to_user_vertical_units
	 if (y > max_long/city_to_user_vertical_units) { // to deal with overflow
		return(max_long);
	} else if (y < min_long/city_to_user_vertical_units) {
		return(min_long);
	} else {
		return(y*city_to_user_vertical_units);
	};
};

 city_coordinate Screen::from_user_x(double x) { // new on 110105 to deal with doubles
	if (x > max_long/city_to_user_horizontal_units) { // to deal with overflow
		return(max_long);
	} else if (x < min_long/city_to_user_horizontal_units) {
		return(min_long);
	} else if (x >= 0) {
		return((city_coordinate) (x*city_to_user_horizontal_units+0.5)); // round up
	} else {
		return((city_coordinate) (x*city_to_user_horizontal_units-0.5)); // round up
	};
};

 city_coordinate Screen::from_user_y(double y) { // new on 110105 to deal with doubles
	 if (y > max_long/city_to_user_vertical_units) { // to deal with overflow
		return(max_long);
	} else if (y < min_long/city_to_user_vertical_units) {
		return(min_long);
	} else if (y >= 0) {
		return((city_coordinate) (y*city_to_user_vertical_units+0.5)); // round up
	} else {
		return((city_coordinate) (y*city_to_user_vertical_units-0.5)); // round up
	};
};

#if TT_XML

xml_element *Screen::xml(xml_document *document) { // new on 100103
	xml_element *element = create_xml_element(L"Screen",document);
	xml_set_attribute(element,L"CenterX",center_x);
	xml_set_attribute(element,L"CenterY",center_y);
	if (scale != ground_scale) {
		xml_set_attribute(element,L"Scale",scale);
	};
	if (view_status != CAMERA_MOSTLY_ABOVE) { // most common since on floor
		xml_set_attribute(element,L"ViewStatusCode",(int) view_status);
	};
	return(element);
};

boolean Screen::handle_xml_tag(Tag tag, xml_node *node) {
	if (tag == NO_MORE_TAGS) {
		city_coordinate region_center_x = xml_get_attribute_int(node,L"CenterX",center_x);
		city_coordinate region_center_y = xml_get_attribute_int(node,L"CenterY",center_y);
      city_scale region_scale = xml_get_attribute_int(node,L"Scale",ground_scale);
		update_viewing_region(region_center_x,region_center_y,region_scale);
		view_status = (CameraStatus) xml_get_attribute_int(node,L"ViewStatusCode",(int) CAMERA_MOSTLY_ABOVE);
		return(TRUE);
	} else {
		return(FALSE); // warn?
	};
};

#endif

void Screen::update_viewing_region(city_coordinate region_center_x,
                                   city_coordinate region_center_y,
                                   city_scale region_scale) {
   center_x = region_center_x;
   center_y = region_center_y;
 //  scale = region_scale;
	set_scale(region_scale); // updated 210804
   // computes the size of the region given the center and
   // the scaling factor a scale of 100 means one ideal screen unit
	// per city coordinate unit
//   x_city_units_per_pixel = (long) tt_ideal_units_per_horizontal_pixel*scale/100;
//   changed following on 170999 since is more accurate "equivalent" of above
	x_city_units_per_pixel = (long) (ideal_screen_width*scale)/(tt_screen_width*100);
//	x_city_units_per_pixel_as_double = (double) (ideal_screen_width*scale)/(tt_screen_width*100); // updated on 210804
//	y_city_units_per_pixel = (long) tt_ideal_units_per_vertical_pixel*scale/100;
	y_city_units_per_pixel = (long) (ideal_screen_height*scale)/(tt_screen_height*100);
//	y_city_units_per_pixel_as_double = (double) (ideal_screen_height*scale)/(tt_screen_height*100);
//	if (tt_log_version_number > 14) {
//		x_half_city_units_per_pixel = x_city_units_per_pixel/2;
//		y_half_city_units_per_pixel = y_city_units_per_pixel/2;
//	} else { // old version didn't round to nearest
//		x_half_city_units_per_pixel = 0;
//		y_half_city_units_per_pixel = 0;
//	};
   min_horizontal_feature = x_city_units_per_pixel/2;
   min_vertical_feature = y_city_units_per_pixel/2;
   min_rect_width = x_city_units_per_pixel*2;
   min_rect_height = y_city_units_per_pixel*2;
   city_coordinate half_width = (ideal_screen_width/200)*scale;
   city_coordinate half_height = (ideal_screen_height/200)*scale;
   view_region.min_x = region_center_x-half_width;
   // subtract off 1 pixel since it goes from say 0 to 399 not 400
   view_region.max_x = region_center_x+half_width-x_city_units_per_pixel;
   // add pixel since min_y translate to FG max_y
   view_region.min_y = region_center_y-half_height; // prior to 250402 +y_city_units_per_pixel but the top and bottom weren't overwritten properly 
   view_region.max_y = region_center_y+half_height-y_city_units_per_pixel; // added -y_city_units_per_pixel on 250402
};

coordinate Screen::screen_x(city_coordinate x) {
	return((coordinate) ((x-view_region.min_x)*pixels_per_horizontal_city_coordinates));
};

// prior to 210804 was:
//#if TT_MICROSOFT
//	// new on 011100 for better accuracy (important for large boxes for example)
//	return((coordinate)((((((int64) x-view_region.min_x)*100)/scale)*tt_screen_width)/ideal_screen_width+x_offset));
//#else
//	// new version on 241199 is both more accurate and doesn't suffer from bad round off problems
//	// when tt_screen_width doesn't divide well into 32000 (e.g. 1152)
//	return(((((x-view_region.min_x)*100)/scale)*tt_screen_width)/ideal_screen_width+x_offset); // was tt_screen_width
//#endif
////	if (x >= view_region.min_x) { // which way to round off
////		return((coordinate) ((x_half_city_units_per_pixel+x-view_region.min_x)/x_city_units_per_pixel+x_offset));
////	} else {
////		return((coordinate) ((-x_half_city_units_per_pixel+x-view_region.min_x)/x_city_units_per_pixel+x_offset));
////	};

coordinate Screen::screen_y(city_coordinate y) {
	return((coordinate) ((y-view_region.min_y)*pixels_per_vertical_city_coordinates));
	// updated to work like screen_x on 301004 (should have been done on 210804 when screen_x was updated)
//#if TT_MICROSOFT
//	// new on 011100 for better accuracy (important for large boxes for example)
//	return((coordinate)((((((int64) y-view_region.min_y)*100)/scale)*tt_screen_height)/ideal_screen_height)); // +y_offset
//#else
//	return(((((y-view_region.min_y)*100)/scale)*tt_screen_height)/ideal_screen_height); // +y_offset
//#endif
};
	 // above rounds to nearest unlike below
//	 coordinate old_screen_x(city_coordinate x) {
//		 return((coordinate) ((x-view_region.min_x)/x_city_units_per_pixel+x_offset));
//	 };
//	 coordinate old_screen_y(city_coordinate y) {
//		 // In FastGraph and Windows screen y decreases as one goes up
//#if FAST_GRAPH||(TT_WINDOWS&&!GRAPHICS_TO_MEMORY)
//		 return((view_region.max_y-y)/y_city_units_per_pixel-y_offset);
//#else
//		 return((coordinate) ((y-view_region.min_y)/y_city_units_per_pixel+y_offset));
//#endif
//	 };

void Screen::viewing_region(city_coordinate &region_center_x,
									 city_coordinate &region_center_y,
									 city_scale &region_scale) {
	region_center_x = center_x;
	region_center_y = center_y;
	region_scale = scale;
};

void Screen::clear(color_index background_color) {
	// is PatBlt better???
	HBRUSH brush = CreateSolidBrush(background_color);
	HBRUSH old_brush = (HBRUSH) SelectObject(tt_hidden_device_context,brush);
	Rectangle(tt_hidden_device_context,
				 0,0,tt_screen_width,tt_screen_height);
	SelectObject(tt_hidden_device_context,old_brush);	
	DeleteObject(brush);
};

boolean Screen::visible_region(city_coordinate rect_min_x,
										 city_coordinate rect_max_x,
										 city_coordinate rect_min_y,
										 city_coordinate rect_max_y) {
	if (rect_min_x >= view_region.max_x) return(FALSE);
	if (rect_min_y >= view_region.max_y) return(FALSE);
	if (rect_max_x <= view_region.min_x) return(FALSE);
	if (rect_max_y <= view_region.min_y) return(FALSE);
	return(TRUE);
};

#if WING||TT_DIRECTX
//void Screen::clip_rectangle(city_coordinate rect_min_x,
//									 city_coordinate rect_max_x,
//									 city_coordinate rect_min_y,
//									 city_coordinate rect_max_y) {
//	// consider using Windows to do this clipping...
//	// when I tried to do the following save_region missed the top line
//	// I add 1 screen_y(rect_max_y) so that the top line is drawn
//	// Windows has a different convention than my code does
//	coordinate min_x = max(screen_x(rect_min_x),tt_destination_llx);
//	coordinate max_x = min(screen_x(rect_max_x),tt_destination_urx);
//	coordinate min_y 
//		= max((coordinate) (tt_screen_height-screen_y(rect_max_y)),
//				(coordinate) (tt_screen_height-tt_destination_ury));
//	coordinate max_y 
//		= min((coordinate) (tt_screen_height-screen_y(rect_min_y)),
//				(coordinate) (tt_screen_height-tt_destination_lly));
//	if (min_x >= max_x || min_y >= max_y) return;
//	tt_main_window->fill_box(min_x,min_y,max_x,max_y);
//};

void Screen::draw_rectangle(city_coordinate rect_min_x,
									 city_coordinate rect_max_x,
									 city_coordinate rect_min_y,
									 city_coordinate rect_max_y) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 170999) {
		tt_error_file() << "Drawing rectangle " << rect_min_x << "," << rect_min_y 
						<< " to " << rect_max_x << "," << rect_max_y 
						<< " frame=" << tt_frame_number << endl;
	};
#endif
	tt_main_window->fill_box(screen_x(rect_min_x),
									 tt_screen_height-screen_y(rect_max_y),
									 screen_x(rect_max_x),
									 tt_screen_height-screen_y(rect_min_y));
};

void Screen::draw_synthetic_shape(SyntheticShape shape,
											 city_coordinate rect_min_x,
									       city_coordinate rect_max_x,
									       city_coordinate rect_min_y,
									       city_coordinate rect_max_y) {
	tt_main_window->draw_synthetic_shape(shape,
   												 screen_x(rect_min_x),
									 			    tt_screen_height-screen_y(rect_max_y),
									 				 screen_x(rect_max_x),
									 				 tt_screen_height-screen_y(rect_min_y));
};

void Screen::set_brush_origin(city_coordinate x, city_coordinate y) {
	// so street and lawn pattern stay put
	tt_main_window->set_brush_origin(horizontal_pixel_length(x)%8,
												vertical_pixel_length(y)%8);
};

#else
void Screen::draw_rectangle(color_index color,
									 city_coordinate rect_min_x,
									 city_coordinate rect_max_x,
									 city_coordinate rect_min_y,
									 city_coordinate rect_max_y) {
  if (rect_min_x > rect_max_x || rect_min_y > rect_max_y) return;
  coordinate pixel_min_x = screen_x(rect_min_x);
  coordinate pixel_max_x = screen_x(rect_max_x)-1;
  // city coordinates have up as more while screen has it as less
#if GRAPHICS_TO_MEMORY
  coordinate pixel_min_y = screen_y(rect_min_y);
  coordinate pixel_max_y = screen_y(rect_max_y);
#elif FAST_GRAPH||TT_WINDOWS
  coordinate pixel_min_y = screen_y(rect_max_y)+1; // deal with round-off problems
  coordinate pixel_max_y = screen_y(rect_min_y)-1; // deal with round-off problems
#endif
	
#if TT_DEBUG_ON && !TT_WINDOWS // since Windows does clipping later
  if (pixel_min_x < 0) {
	  tt_error_file() << "Whoops bad rectangle not clipped; min_x is "
					  << pixel_min_x << endl;
	  pixel_min_x = 0;
  };
  if (pixel_max_x >= tt_screen_width) {
	  tt_error_file() << "Whoops bad rectangle not clipped; max_x is "
					  << pixel_max_x << endl;
	  pixel_max_x = tt_screen_width-1;
  };
  if (pixel_min_y < 0) {
	  tt_error_file() << "Whoops bad rectangle not clipped; min_y is"
					  << pixel_min_y << endl;
	  pixel_min_y = 0;
  };
  if (pixel_max_y >= tt_screen_height) {
	  tt_error_file() << "Whoops bad rectangle not clipped; max_y is"
					  << pixel_max_y << endl;
	  pixel_max_y = tt_screen_height-1;
  };
#endif
  city_coordinate rect_width = rect_max_x-rect_min_x;
  if (rect_width < min_horizontal_feature) {
	  return; // too small to see
  };
  city_coordinate rect_height = rect_max_y-rect_min_y;
  if (rect_height < min_vertical_feature) {
	  return;
  };
#if FAST_GRAPH
  fg_setcolor(color);
  if (rect_width < min_rect_width) {
	  if (rect_height < min_rect_height) { // use a point
		  fg_point(pixel_min_x,pixel_min_y);
		  return;
	  } else { // use a line instead
		  fg_move(pixel_min_x, pixel_min_y);
		  fg_draw(pixel_min_x, pixel_max_y);
		  return;
	  };
  };
  if (rect_height < min_rect_height) { // use a line instead
	  fg_move(pixel_min_x, pixel_min_y);
	  fg_draw(pixel_max_x, pixel_min_y);
	  return;
  };
//  save_region(pixel_min_x, pixel_max_x, pixel_min_y, pixel_max_y);
  fg_rect(pixel_min_x,pixel_max_x,pixel_min_y,pixel_max_y);
#elif GRAPHICS_TO_MEMORY
//   fg_box_t box;
//   fg_make_box(box,pixel_min_x,pixel_min_y,pixel_max_x,pixel_max_y);
//   fg_fillbox(color,FG_MODE_SET,~0,box);
//	if (!tt_using_dispdib) {
//		color = tt_color_permutation[color];
//	};
//#if WING||TT_DIRECTX
//	tt_main_window->select_brush(brush_id);
//	tt_main_window->fill_box(pixel_min_x,pixel_min_y,pixel_max_x,pixel_max_y);
//#else
	fill_box(pixel_min_x,pixel_min_y,pixel_max_x,pixel_max_y,color);
//#endif
#elif TT_WINDOWS
	// probably worth optimizing this for case when color is same as last time
	// or use PatBlt???
	HBRUSH brush = CreateSolidBrush(color);
	HBRUSH old_brush = (HBRUSH) SelectObject(tt_hidden_device_context,brush);
	Rectangle(tt_hidden_device_context,
				 pixel_min_x,pixel_min_y,pixel_max_x,pixel_max_y);
	SelectObject(tt_hidden_device_context,old_brush);	
	DeleteObject(brush);
#endif
};

#endif


void Screen::place_character(character character, 
									  city_coordinate ulx, city_coordinate uly,
									  city_coordinate width, city_coordinate height,
                             color text_color) {
	if (ulx+width < min_x() || ulx > max_x()) return;
	if (uly > max_y() || uly+height < min_y()) return;
	tt_main_window->place_character(character,
											  screen_x(ulx),screen_y(uly),
											  horizontal_pixel_length_as_double(width),
											  vertical_pixel_length_as_double(height),
                                   text_color);
};

void Screen::place_wide_character(wide_character character, // only difference
											 city_coordinate ulx, city_coordinate uly,
											 city_coordinate width, city_coordinate height,
											 color text_color) {
	if (ulx+width < min_x() || ulx > max_x()) return;
	if (uly > max_y() || uly+height < min_y()) return;
	tt_main_window->place_wide_character(character,
													 screen_x(ulx),screen_y(uly),
													 horizontal_pixel_length_as_double(width), 
													 vertical_pixel_length_as_double(height),
													 text_color);
};

void Screen::place_number(long number,
								  city_coordinate ulx, city_coordinate uly,
								  city_coordinate character_width,
								  city_coordinate character_height) {
	if (uly > max_y() || uly+character_height < min_y() || ulx > max_x()) return;
	character text[16];
//	output_string_stream text_stream(text,16); // rewritten on 260104
//	text_stream << number;
	itoa(number,text,10);
//#if TT_NEW_IO
//	int text_length = text_stream.str().length();
//#else
//	int text_length = text_stream.pcount();
//#endif
	int text_length = strlen(text);
	if (ulx+character_width*text_length < min_x()) return;
	tt_main_window->text_out(text,text_length,
									 screen_x(ulx),screen_y(uly),
									 // add 1/2 pixel so it rounds better
									 horizontal_pixel_length(character_width+one_pixel_in_x_city_units()/2),
									 vertical_pixel_length(character_height+one_pixel_in_y_city_units()/2));
};

int Screen::place_text(const_string text, int length,
							  city_coordinate ulx, city_coordinate uly,
							  city_coordinate max_width, city_coordinate max_height,
							  city_coordinate character_width,
							  city_coordinate character_height,
							  WhenToBreakSentences break_sentences,
							  boolean vertically_center,
                       boolean add_dots_if_more,
							  city_coordinate extra_width_at_center,
							  city_coordinate blank_width,
                       color text_color,
							  boolean first_call,
							  long max_length) {
	return(tt_main_window->place_text(text,length,
												 screen_x(ulx),screen_y(uly),
												 horizontal_pixel_length(max_width),
												 vertical_pixel_length(max_height),
												 // add 1/2 pixel so it rounds better
												 horizontal_pixel_length(character_width+one_pixel_in_x_city_units()/2),
												 vertical_pixel_length(character_height+one_pixel_in_y_city_units()/2),
												 break_sentences,
												 vertically_center,
                                     add_dots_if_more,
												 horizontal_pixel_length(extra_width_at_center),
												 horizontal_pixel_length(blank_width),
                                     text_color,
												 first_call,
												 max_length));
};

city_coordinate Screen::get_extent(const_string text, int length,
                                   city_coordinate width, city_coordinate height,
							              boolean bold, boolean fixed_width,
											  boolean might_contain_tabs) { // new on 070903
	return(tt_main_window->get_extent(text,length,
                                     horizontal_pixel_length_as_double(width),
                                     vertical_pixel_length_as_double(height),
                                     bold,fixed_width,might_contain_tabs)
          *one_pixel_in_x_city_units());
};
#if TT_WIDE_TEXT
city_coordinate Screen::get_extent(wide_string text, int length,
                                   city_coordinate width, city_coordinate height,
							              boolean bold, boolean fixed_width,
											  boolean might_contain_tabs) { // new on 070903
	return(tt_main_window->get_extent(text,length,
                                     horizontal_pixel_length(width),
                                     vertical_pixel_length(height),
                                     bold,fixed_width,might_contain_tabs)
          *one_pixel_in_x_city_units());
};
#endif
city_coordinate Screen::get_vertical_extent(const_string text, int length,
														  city_coordinate width, city_coordinate height,
														  boolean bold, boolean fixed_width) {
	return(tt_main_window->get_vertical_extent(text,length,
                                     horizontal_pixel_length(width),
                                     vertical_pixel_length(height),
                                     bold,fixed_width)
          *one_pixel_in_y_city_units());
};
#if TT_WIDE_TEXT
city_coordinate Screen::get_vertical_extent(wide_string text, int length,
                                   city_coordinate width, city_coordinate height,
							              boolean bold, boolean fixed_width) {
	return(tt_main_window->get_vertical_extent(text,length,
                                     horizontal_pixel_length(width),
                                     vertical_pixel_length(height),
                                     bold,fixed_width)
          *one_pixel_in_y_city_units());
};
#endif

void Screen::set_font(city_coordinate new_font_width, city_coordinate new_font_height,
							 boolean bold, boolean fixed_width) {
	tt_main_window->set_font(horizontal_pixel_length_as_double(new_font_width),
									 vertical_pixel_length_as_double(new_font_height),
									 bold,fixed_width);
};

boolean Screen::text_out(const_string text, int length, 
								 city_coordinate ulx, city_coordinate uly,
								 city_coordinate character_width, city_coordinate character_height,
								 boolean bold, boolean fixed_width,
								 boolean background_transparent,
								 color_index text_color,
								 boolean text_is_wide,
								 bytes color_permutation,
								 color background_color,
								 city_coordinate total_width, // new on 060202
								 boolean might_contain_tabs,
								 int caret_location) {
//							 city_coordinate urx, city_coordinate ury) { // new on 090702 but probably a mistake - remove??
   // updated on 160804 to return FALSE if nothing displayed (i.e. TextOut not called - it may not show anything)
	if (character_width == 0 || text == NULL || length == 0) return(FALSE); // new on 060202
	if (tt_log_version_number >= 59 && !fixed_width && length > max_variable_width_text_length) {
		fixed_width = TRUE; // too expensive and error prone to deal with very long lines -- new on 121104
	};
	if (fixed_width && total_width > ideal_screen_width && ulx < view_region.min_x) { 
		// for long text round off gets in the way - new on 060202
		// was < 0 prior to 070202 but that doesn't work outdoors
		// should check if length is prior to a new line or carriage return
//		int longest_line;
//		city_coordinate new_width, new_height;
//		get_extent_size((wide_string) text,length,1, // 1 is number of lines - OK?? 
//                      character_width,character_height, 
//							 longest_line,new_width,new_height,
//				          bold,fixed_width);
		double true_character_width = (double) total_width/length; 
		// was int prior to 250202 but round off did havok with very very long numbers
//		int characters_to_skip = -ulx/true_character_width;
		int characters_to_skip = (int) ((((int64) view_region.min_x-ulx)*(int64) length)/(int64) total_width); 
		// view_region.min_x was implictily 0 prior to 070202
		if (characters_to_skip >= length) return(FALSE);
//		ulx %= true_character_width;'
		ulx += (city_coordinate) (characters_to_skip*true_character_width); 
		// above rewriten on 070202 since the above assumes you are on the floor
		length -= characters_to_skip;
		if (text_is_wide) {
			text += 2*characters_to_skip; 
		} else {
			text += characters_to_skip;
		};
		if (caret_location >= 0) { // condition new on 160904
			caret_location -= characters_to_skip; // new on 040704 since clipping the text so need to adjust the caret location
		};
	};
	int max_characters;
//	if (inside_a_house()) {
	if (fixed_width) { 
		// added conditional on 130302 since too hard to calculate when variable width and this truncated the display sometimes
		max_characters = 2*viewing_width()/character_width; // double to deal with font size oddities - new on 060202
	} else {
		max_characters = max_long;
	};
//	} else { // doesn't work otherwise - track this down...
//		max_characters = length;
//	};
	coordinate ulx_in_pixels = screen_x(ulx);
	coordinate uly_in_pixels = screen_y(uly);
//	if (!background_transparent) { // new on 090702 since using TextOut leaves some space on the right
//BEGIN_GDI	
//		tt_main_window->select_stock_object(NULL_PEN);
//		HGDIOBJ old_selection = tt_main_window->select_object(tt_main_window->create_solid_brush(background_color));
//		draw_rectangle(ulx,urx,uly,ury);
//		if (old_selection != NULL) DeleteObject(tt_main_window->select_object(old_selection));
//		background_transparent = TRUE; // since dealt with it here
//END_GDI
//	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	return(tt_main_window->text_out(text,min(length,max_characters),
											  ulx_in_pixels,uly_in_pixels,
											  horizontal_pixel_length_as_double(character_width), 
											  // switched to double from integer on 080803
											  vertical_pixel_length_as_double(character_height), //+1, // stretched by a pixel on 080702
											  bold,fixed_width,
											  background_transparent,text_color,text_is_wide,color_permutation,background_color,
											  might_contain_tabs,caret_location));
};

//boolean Screen::create_caret(city_coordinate height) {
//	return(tt_main_window->create_caret(vertical_pixel_length(height)));
//};

void Screen::draw_line(city_coordinate start_x, city_coordinate start_y, city_coordinate end_x, city_coordinate end_y) {
	// added pixel_max_y on 301004 since this didn't compensate for Windows treating y in the other direction 
	// wasn't being used by anything so is a safe change
	tt_main_window->draw_line(screen_x(start_x),pixel_max_y-screen_y(start_y),screen_x(end_x),pixel_max_y-screen_y(end_y));
};

void Screen::correct_font_size(wide_string text, int length, int number_of_lines, 
										 city_coordinate width, 
										 city_coordinate height,
										 boolean bold, boolean fixed_width, // added fixed_width on 050102
										 boolean might_contain_tabs, // new on 070903
										 city_coordinate &new_font_width, 
										 city_coordinate &new_font_height) {
//	coordinate coordinate_font_width,coordinate_font_height;
	double font_width_pixels_as_double,font_height_pixels_as_double;
	tt_main_window->correct_font_size(text,length,number_of_lines, 
												 horizontal_pixel_length_as_double(width), 
												 vertical_pixel_length_as_double(height),
												 bold,fixed_width,might_contain_tabs,
												 font_width_pixels_as_double,font_height_pixels_as_double);
	city_coordinate one_x_pixel = one_pixel_in_x_city_units();
	city_coordinate one_y_pixel = one_pixel_in_y_city_units();
	new_font_width = (city_coordinate) font_width_pixels_as_double*one_x_pixel;
	new_font_height = (city_coordinate) font_height_pixels_as_double*one_y_pixel;
};

void Screen::get_extent_size(wide_string text, int length, int number_of_lines,
                             city_coordinate font_width, city_coordinate font_height, 
									  int &longest_line, city_coordinate &new_width, city_coordinate &new_height,
				                 boolean bold, boolean fixed_width,
									  boolean might_contain_tabs) { // new on 070903
	if (text == NULL || length == 0) { // new on 140202 -- second disjunct added 020903
		longest_line = 1;
		new_width = font_width/4; // was tile_width; prior to 020903
		new_height = font_height; // was tile_height;
		return;
	};
	double new_width_in_pixels, new_height_in_pixels;
	// integer round-off not a problem beginning 080803
	double old_width_in_pixels = horizontal_pixel_length_as_double(font_width); 
	// +min_horizontal_feature); // added 1/2 a pixel here to deal with round off (new on 110302)
	double old_height_in_pixels = vertical_pixel_length_as_double(font_height); // +min_vertical_feature);
	tt_main_window->get_extent_size(text,length,number_of_lines, 
											  old_width_in_pixels,old_height_in_pixels,
											  longest_line,
											  new_width_in_pixels,new_height_in_pixels,
											  bold,fixed_width,might_contain_tabs);
	// this all works but new_width_in_pixels is in logical units not pixels
	if (old_width_in_pixels == new_width_in_pixels) {
		new_width = font_width; // no change - rewritten on 110302 to avoid round off errors here
	} else {
		double one_x_pixel = one_pixel_in_x_city_units_as_double();
		new_width = (city_coordinate) (new_width_in_pixels*one_x_pixel);
		if ((new_width < new_width_in_pixels && one_x_pixel > 1) || // overflow
			 (longest_line > new_width && font_width > 1)) { // new on 260202
			// prior to 131004 the test above was length > new_width but for multi-lined text that doesn't make sense
//			new_width = max_long; // best we can do now - new on 250202
//			new_width = (city_coordinate) (max_unsigned_short*one_x_pixel); 
			// above rewritten on 131004 since the above seems to become negative and then we get a zero scale
			// may be it became negative since edge size and the like is sometimes added to the extent so half max should be pretty safe
			new_width = max_long/2; // new on 091104 since the above does badly on million digit numbers and text pads
		};
	};
	if (old_height_in_pixels == new_height_in_pixels) {
		new_height = font_height; // no change - rewritten on 110302 to avoid round off errors here
	} else {
		double one_y_pixel = one_pixel_in_y_city_units_as_double();
		new_height = (city_coordinate) (new_height_in_pixels*one_y_pixel);
		if (new_height < new_height_in_pixels && one_y_pixel > 1) { // overflow
			new_height = max_long/2; // best we can do now - new on 250202 -- divide by 2 on 091104 since otherwise can overflow again
		};
	};
};

/*

city_coordinate Screen::correct_font_width(wide_string text, int length, 
														 city_coordinate new_font_width, 
														 city_coordinate new_font_height,
														 boolean bold) {
	return(tt_main_window->correct_font_width(text,length, 
														   horizontal_pixel_length(new_font_width), 
														   vertical_pixel_length(new_font_height),
														   bold)
			 *one_pixel_in_x_city_units());
};

city_coordinate Screen::correct_font_height(wide_string text, int length, int number_of_lines,
														  city_coordinate new_font_width, 
														  city_coordinate new_font_height,
														  boolean bold) {
	return(tt_main_window->correct_font_height(text,length,number_of_lines,
														    horizontal_pixel_length(new_font_width), 
														    vertical_pixel_length(new_font_height),
														    bold)
			 *one_pixel_in_y_city_units());
};
*/

void Screen::move_mark_to(city_coordinate x, city_coordinate y) {
	tt_memory_graphics_mark_x = screen_x(x);
	tt_memory_graphics_mark_y = screen_y(y);
};


/*
const long min_that_will_trigger_overflow = max_long/100+1; // new on 061103 since 820,000 for x went down the inaccurate path below and returned 0
//const long min_that_will_trigger_overflow = max_long/3000;

long Screen::horizontal_pixel_length(city_coordinate x) {
	if (x > min_that_will_trigger_overflow) { // new on 050202 to avoid overflow
		return((((x/ideal_screen_width)*100)/scale)*tt_screen_width);
	} else {
		return((((x*100)/scale)*tt_screen_width)/ideal_screen_width);
//		 return((x+x_half_city_units_per_pixel)/x_city_units_per_pixel);
//		 return((x)/x_city_units_per_pixel);
	};
};

long Screen::vertical_pixel_length(city_coordinate y) {
	if (y > min_that_will_trigger_overflow) { // new on 050202 to avoid overflow
		return((((y/ideal_screen_height)*100)/scale)*tt_screen_height);
	} else {
		return((((y*100)/scale)*tt_screen_height)/ideal_screen_height);
//       return((y+y_half_city_units_per_pixel)/y_city_units_per_pixel);
//		 return((y)/y_city_units_per_pixel);
   };
};

double Screen::horizontal_pixel_length_as_double(city_coordinate x) { // new on 080803 - initial for better quality set_font
	return((((x*100.0)/scale)*tt_screen_width)/ideal_screen_width);
};

double Screen::vertical_pixel_length_as_double(city_coordinate y) {
	return((((y*100.0)/scale)*tt_screen_height)/ideal_screen_height);
};
*/

boolean Screen::more_than_pixel_by_pixel(TTRegion *region) {
	return(region->width() > x_city_units_per_pixel &&
			 region->height() > y_city_units_per_pixel);
};

void Screen::switch_to(Background *new_background,
							  boolean keep_animating, boolean old_background_ok,
							  boolean new_background_finish_instantly) {
	if (old_background_ok) {
		if (keep_animating) {
//			tt_finish_all_animations = TRUE; // new way of doing this
//         tt_backgrounds_still_animating
//           = new Backgrounds(background,tt_backgrounds_still_animating);
			background->finish_instantly(); // new on 030903 since just setting tt_finish_all_animations isn't good enugh
			if (tt_global_picture != NULL && tt_log_version_number >= 53) { // new on 260904 -- e.g. bird may be moving stack on nest while standing
				tt_global_picture->finish_instantly();
			};
			background->update_display(NULL);
			propagate_changes();
//			string replay_file = replay_file_name();
//			if (replay_file == NULL) {//  ||
//			    strncmp(replay_file,"intro.",6) != 0) {
//				tt_finish_all_animations = FALSE;
//			};
		};
//		background->set_screen(NULL);
		background->decrement_screen_count();
		flush_all();
	} else {
		layer_count = 0;
//		if (!tt_shutting_down && tt_time_travel != TIME_TRAVEL_OFF) {
//			add_time_travel_buttons();
//		};
	};
	reset_grid();
	screen_dirty();
#if TT_DEBUG_ON
	if (tt_debug_mode == 3) {
		tt_error_file() << "Switch to new background so cache invalidated." << endl;
	};
#endif	
//	new_background->set_screen(this);
   new_background->now_on_screen();
	new_background->increment_screen_count();
	background = new_background; // install new background
	// background_cache_valid = FALSE;
//   backmost_unchanged_layer_index = -1;
//   background_unchanged_count = -1; // force both pages to notice
//   background->about_to_redisplay();
//	background->update_display(&view_region); -- commented out on 191202 since will be called again as soon we start to display this frame
//	if (!tt_titles_just_ended) { // condition new on 071003 since otherwise first frame on playback can be wrong
		background->display_region();
//	};
	if (new_background_finish_instantly) {
		background->finish_instantly();
	};
//	if (tt_caching_background) {
//		cache_background();
//	};
};

void Screen::screen_dirty() {
	tt_dirty_min_x = 0;
	tt_dirty_max_x = tt_screen_width-1;
	tt_dirty_min_y = 0;
	tt_dirty_max_y = tt_screen_height-1;
	invalidate_cache();
//	if (background != NULL) { // new on 200901
//		background->set_dirty();
//	};
};

void Screen::background_being_destroyed(Background *background_being_destroy) { // new on 200901
	if (background_being_destroy == background) {
		background = NULL;
	};
};

void Screen::flush_all() {
//	for (int i = 0; i < layer_count; i++) { //
//		layers[i]->recursively_decrement_screen_ref_count();
//	};
//   tt_redraw_all = TRUE;
	for (int i = 0; i < layer_count; i++) {
		layers[i]->recursively_reset_cache_status();
		layers[i]->notice_on_screen_change(FALSE);
	};
	layer_count = 0;
	if (background == tt_city) {
		tt_city->screen_flushed();
	};
//	if (!tt_shutting_down && tt_time_travel != TIME_TRAVEL_OFF) {
//		add_time_travel_buttons();
//	};
};

long ideal_horizontal_pixel_length(city_coordinate x) {
	// new version on 241199 which is more accurate especially at screen resolutions that don't divide evenly into ideal_screen_width
	// unlike horizontal_pixel_length this doesn't consider the current scale (e.g. when flying)
	// worth replacing this with a version that uses a double that precomputes tt_screen_width/ideal_screen_width -- asked 210804
	if (x > max_long/3000) { // 3000 is largest future screen width prior to overflow problems -- new on 050202
		return((x/ideal_screen_width)*tt_screen_width);
	} else {
		return((x*tt_screen_width)/ideal_screen_width);
	};
//	return((x+tt_half_ideal_units_per_horizontal_pixel)/tt_ideal_units_per_horizontal_pixel);
};

long ideal_vertical_pixel_length(city_coordinate y) {
	if (y > max_long/3000) { // 3000 is largest future screen height prior to overflow problems -- new on 050202
		return((y/ideal_screen_height)*tt_screen_height);
	} else {
		return((y*tt_screen_height)/ideal_screen_height);
	};
//   return((y+tt_half_ideal_units_per_vertical_pixel)/tt_ideal_units_per_vertical_pixel);
};

// following new on 080803
//double ideal_horizontal_pixel_length_as_double(city_coordinate x) {
//	return((x*(double) tt_screen_width)/ideal_screen_width);
//};
//
//double ideal_vertical_pixel_length_as_double(city_coordinate y) {
//	return((y*(double) tt_screen_height)/ideal_screen_height);
//};

city_coordinate ideal_horizontal_units(coordinate x) {
	if (x > max_long/ideal_screen_width) { // new on 050202 to prevent overflows
		return((x/tt_screen_width)*ideal_screen_width);
	} else {
		return((x*ideal_screen_width)/tt_screen_width);
	};
//	return(x*tt_ideal_units_per_horizontal_pixel);
};

city_coordinate ideal_vertical_units(coordinate y) {
	if (y > max_long/ideal_screen_height) { // new on 050202 to prevent overflows
		return((y/tt_screen_height)*ideal_screen_height);
	} else {
		return((y*ideal_screen_height)/tt_screen_height);
	};
//   return(y*tt_ideal_units_per_vertical_pixel);
};

/*
boolean Screen::move_region_to_free_area(TTRegion *region, int number_of_tries,
													  Sprite *exception) {
	city_coordinate region_width = 1+region->max_x-region->min_x;
	city_coordinate region_height = 1+region->max_y-region->min_y;
//	city_coordinate screen_min_x = center_x-ideal_screen_width/2;
//	city_coordinate screen_max_x = center_x+ideal_screen_width/2;
//	city_coordinate screen_min_y = center_y-ideal_screen_height/2;
//	city_coordinate screen_max_y = center_y+ideal_screen_height/2;
//   city_coordinate region_center_x, region_center_y;
	for (int i = 0; i < number_of_tries; i++) {
		for (int j = 0; j < layer_count; j++) {
			TTRegion *other_region = layers[j]->pointer_to_collision_region();
			if (other_region != NULL && layers[j] != exception &&
				 region->overlaps(other_region)) break; // no good
		};
		if (j == layer_count) { // no overlaps found
//       tt_error_file() << "Free " << region->min_x << " " << region->max_x << endl;
			return(TRUE);
		};
//      region->region_center(region_center_x,region_center_y);
		boolean move_horizontally = random(2);
		if (move_horizontally) {
			if (region->max_x > ideal_screen_width-region_width) {
//          tt_error_file() << "x " << -region_width << endl;
				region->min_x -= region_width;
				region->max_x -= region_width;
			} else if (region->min_x < region_width) {
//          tt_error_file() << "x " << region_width << endl;
				region->min_x += region_width;
				region->max_x += region_width;
			} else { // can move either direction
				if (random(2)) {
//             tt_error_file() << "random x " << -region_width << endl;
					region->min_x -= region_width;
					region->max_x -= region_width;
				} else {
//             tt_error_file() << "random x " << region_width << endl;
					region->min_x += region_width;
					region->max_x += region_width;
				};
			};
		} else {
			if (region->max_y > ideal_screen_height-region_height) {
				region->min_y -= region_height;
				region->max_y -= region_height;
			} else if (region->min_y < region_height) {
				region->min_y += region_height;
				region->max_y += region_height;
			} else { // can move either direction
				if (random(2)) {
					region->min_y -= region_height;
					region->max_y -= region_height;
				} else {
					region->min_y += region_height;
					region->max_y += region_height;
				};
			};
		};
#if FAST_GRAPH&&TT_DEBUG_ON
		if (tt_debug_mode == 21) {
			fg_setpage(1-tt_work_page);
			draw_box(RED,region->min_x,region->max_x,region->min_y,region->max_y);
			fg_setpage(tt_work_page);
		};
#endif
//#if FLASH_GRAPHICS
//       fg_setactivepage(1-tt_work_page);
//       fg_box_t box;
//       fg_make_box(box,region->min_x,region->min_y,region->max_x,region->max_y);
//       fg_drawbox(FG_RED,FG_MODE_SET,~0,FG_LINE_SOLID,box,fg.displaybox);
//       fg_setactivepage(tt_work_page);
//#endif
	};
//   tt_error_file() << "Gave up " << region->min_x << " " << region->max_x << endl;
	return(FALSE);
};
*/
/*

#if !TT_WINDOWS
void Screen::draw_box(color color,
							 city_coordinate rect_min_x,
							 city_coordinate rect_max_x,
							 city_coordinate rect_min_y,
							 city_coordinate rect_max_y) {
	coordinate pixel_min_x = screen_x(rect_min_x);
	coordinate pixel_max_x = screen_x(rect_max_x);
#if FAST_GRAPH
	coordinate pixel_min_y = screen_y(rect_max_y);
	coordinate pixel_max_y = screen_y(rect_min_y);
	fg_setcolor(color);
	fg_box(pixel_min_x,pixel_max_x,pixel_min_y,pixel_max_y);
#endif
//#if FLASH_GRAPHICS
//   coordinate pixel_min_y = screen_y(rect_min_y);
//   coordinate pixel_max_y = screen_y(rect_max_y);
//   fg_box_t box;
//   fg_make_box(box,pixel_min_x,pixel_min_y,pixel_max_x,pixel_max_y);
//   fg_drawbox(color,FG_MODE_SET,~0,FG_LINE_SOLID,box,fg.displaybox);
//#endif
};

void Screen::draw_dotted_box(color color,
							 city_coordinate rect_min_x,
							 city_coordinate rect_max_x,
							 city_coordinate rect_min_y,
							 city_coordinate rect_max_y) {
	const coordinate min_x = screen_x(rect_min_x);
	const coordinate max_x = screen_x(rect_max_x);
// does the following work for FAST_GRAPH?
	const coordinate min_y = screen_y(rect_min_y);
	const coordinate max_y = screen_y(rect_max_y);
#if FAST_GRAPH
	const short int dash_pattern = 0x3333; // 2 pixels on -- 2 off
	fg_setcolor(color);
	fg_move(min_x,min_y);
	fg_dash(min_x,max_y,dash_pattern);
	fg_dash(max_x,max_y,dash_pattern);
	fg_dash(max_x,min_y,dash_pattern);
	fg_dash(min_x,min_y,dash_pattern);
#endif
//#if FLASH_GRAPHICS
//   fg_box_t box;
//   fg_make_box(box,min_x,min_y,max_x,max_y);
//   fg_drawbox(color,FG_MODE_SET,~0,FG_LINE_MEDIUM_DASHED,box,fg.displaybox);
//#endif
};

void Screen::draw_line(color line_color,
							  city_coordinate end_x, city_coordinate end_y) {
	dimension pixel_x = screen_x(end_x);
	dimension pixel_y = screen_y(end_y);
#if FAST_GRAPH
	fg_setcolor(line_color);
	fg_draw(pixel_x,pixel_y);
#endif
//#if FLASH_GRAPHICS
//   fg_line_t line;
//   fg_make_line(line,
//              tt_flash_graphics_mark_x,tt_flash_graphics_mark_y,
//              pixel_x,pixel_y);
//   fg_drawline(line_color,FG_MODE_SET,~0,FG_LINE_SOLID,line);
//#endif
};
#endif


*/


//void Screen::cache_background() {
//	if (milliseconds_to_draw_background > milliseconds_to_copypage) {
		 // could figure in how long a cache is good for
//#if TT_DEBUG_ON
//		timer_start = current_time(performance_timer);
//#endif
//#if FAST_GRAPH
//		fg_copypage(tt_work_page,tt_background_cache_page);
//#endif
//#if !TT_WINDOWS&&GRAPHICS_TO_MEMORY
//	 memcpy(tt_memory_graphics_background_page,tt_memory_graphics_work_page,
//			  64000L);
//#endif
//#if TT_DEBUG_ON
//		if (tt_debug_mode == 3) {
//			tt_error_file() << (current_time(performance_timer)-timer_start)
//							<< " ms to cache background cache." << endl;
//		};
//#endif
//	};
//};
/*
void Screen::place_arrow(boolean up_flag,
								 city_coordinate length, color arrow_color,
								 city_coordinate center_x, city_coordinate center_y) {
	city_coordinate shaft_y, head_y, arrow_y;
	const city_coordinate left_arrow_x = center_x-length/4;
	const city_coordinate right_arrow_x = center_x+length/4;
	if (up_flag) {
		shaft_y = center_y-length/2;
		head_y =  center_y+length/2;
		arrow_y = center_y+length/4;
	} else {
		shaft_y = center_y+length/2;
		head_y =  center_y-length/2;
		arrow_y = center_y-length/4;
   };

   move_mark_to(center_x,shaft_y);
   draw_line(arrow_color,center_x,head_y);
   draw_line(arrow_color,left_arrow_x,arrow_y);
   move_mark_to(center_x,head_y);
   draw_line(arrow_color,right_arrow_x,arrow_y);
};
*/

#if !WING&&!TT_DIRECTX
void Screen::clip_rectangle(color color,
									 city_coordinate rect_min_x,
									 city_coordinate rect_max_x,
									 city_coordinate rect_min_y,
									 city_coordinate rect_max_y) {
#if !TT_WINDOWS // both GDI and GRAPHICS_TO_MEMORY clip by default anyway
	// first check if outside of the view completely
	if (!visible_region(rect_min_x,rect_max_x,rect_min_y,rect_max_y)) return;
	if (rect_min_x < view_region.min_x) rect_min_x = view_region.min_x;
	if (rect_min_y < view_region.min_y) rect_min_y = view_region.min_y;
	if (rect_max_x > view_region.max_x) rect_max_x = view_region.max_x;
	if (rect_max_y > view_region.max_y) rect_max_y = view_region.max_y;
#endif
	draw_rectangle(color,rect_min_x,rect_max_x,rect_min_y,rect_max_y);
};
#endif

/*
void Screen::display() {
  HTimer clock, cycle_clock;
  boolean clean_background = background->update_display(&view_region,this);
  detect_collisions();
  // collisions may cause movement which will change priorities
  if (tt_leaders != NULL) {
	  tt_leaders->propagate_changes();
  };
  int new_backmost_unchanged_layer_index = update_layers();
  if (new_backmost_unchanged_layer_index < backmost_unchanged_layer_index) {
	  // part of cached background has moved so flush cache
	  tt_redraw_all = TRUE;
  };
  backmost_unchanged_layer_index = new_backmost_unchanged_layer_index;
  if (tt_debug_mode == 3) {
	  tt_error_file() << "Display total was " << last_display_duration
					  << " which was "
					  << (int) (0.1*last_display_duration/tt_delta)
					  << "% of total frame time." << endl;
	  total_display_duration += last_display_duration;
	  total_run_time += tt_millisecond_delta;
	  cycle_clock.timerOn();
  };
  if (tt_redraw_all || !clean_background) {
	  clock.timerOn();
	  background->display_region(this);
	  if (tt_debug_mode == 3) {
		  tt_error_file() << clock.timerOff()
						  << " ms to draw background." << endl;
	  };
  };
  for (int i = 0; i < layer_count; i++) {
	  if (!tt_redraw_all && i <= backmost_unchanged_layer_index) {
		  layers[i]->done_with_previous_values();
		  layers[i]->set_changes_propagated(FALSE); // reset leader flag
	  } else {
		  clock.timerOn();
		  layers[i]->display(this);
		  layers[i]->clear_changes();
		  if (tt_debug_mode == 3) {
			  tt_error_file() << clock.timerOff()
							  << " ms to display -- "
							  << colliders[layers[i]->kind_of()] << endl;
		  };
		  if (tt_debug_mode == 1) {
			  TTRegion *collision_region = layers[i]->pointer_to_collision_region();
			  if (collision_region != NULL) {
				  draw_box(RED,
							  collision_region->min_x, collision_region->max_x,
							  collision_region->min_y, collision_region->max_y);
			  };
		  };
	  };
  };
  tt_redraw_all = FALSE;
  fg_setcolor(WHITE);
  fg_box(x_offset,x_offset+width,y_offset,y_offset+height);
  if (tt_debug_mode == 3) {
	  last_display_duration = cycle_clock.timerOff();
  };
};

void Screen::detect_collisions() {
	boolean clean_so_far = TRUE;
	for (int i = layer_count-1; i>= 0; i--) { 
		// from front to back (so programmer hits door before house)
		clean_so_far = clean_so_far && layers[i]->clean();
		if (!clean_so_far && layers[i]->notify_if_colliding()) {
			for (int j = 0; j < layer_count; j++) {
				if (i != j && layers[i]->colliding(layers[j])) {
					layers[i]->colliding_with(layers[j],this);
				};
			};
		};
	};
};
// not in use anymore
Sprite *Screen::first_to_collide_with_except(TTRegion *region,
															Sprite *exception) {
	for (int i = layer_count-1; i >= 0; i--) { // from front to back
		if (layers[i] != exception) {
			TTRegion *other_region = layers[i]->pointer_to_collision_region();
			if (other_region != NULL && region->overlaps(other_region)) {
				return(layers[i]);
			};
		};
	};
	return(NULL); // none found
};
*/
