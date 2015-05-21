// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved.
   
#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif   
#if !defined(__TT_DIRTY_H)   
#include "dirty.h"
#endif   
#if !defined(__TT_UTILS_H)
#include "utils.h"
#endif   
#if !defined(__TT_SCREEN_H)   
#include "screen.h"
#endif   
#if !defined(__TT_CLEANUP_H)   
#include "cleanup.h"
#endif 
#if !defined(__TT_LOG_H)   
#include "log.h"
#endif    
#if TT_DIRECTX
#if !defined(__TT_WINMAIN_H)   
#include "winmain.h"
#endif
#endif
#if !defined(__TT_BLIT_H)
#include "blit.h"
#endif
#if !defined(__TT_HELP_H) // for log(...)
#include "help.h"
#endif

#include <stdlib.h> // for min, max, range

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// maintains in a grid changes to the current page so it can be restored

// following should be roughly the screen aspect ratio and width
// needs to be a multiple of 8 to fit byte boundaries

const int chunk_width = 32; // was 40 (and was 32 long before that) - changed on 241099 so new screen sizes are multiples
const int chunk_height = 8; // was 20; // changed from 16 to 8 on 041199 since 800/16 was not an integer
// long to avoid overflows
const long chunk_size = chunk_width*chunk_height;

int grid_width, grid_height, grid_size, grid_top_last_index;
long grid_mid; // long to be sure to avoid truncation when computing grid_top size

long *grid_time_stamps = NULL;
//#if TT_DIRECTX
//long *grid_time_stamps2 = NULL;
//#endif

work_page cache_page = NULL;
HGLOBAL cache_page_handle = NULL;
//#if TT_DIRECTX
//work_page cache_page2 = NULL;
//#endif

void initialize_page_grid(boolean reset) {
	grid_width = tt_screen_width/chunk_width;
	grid_height = tt_screen_height/chunk_height;
	grid_mid = grid_height/2; // might be rounded
	grid_top_last_index = grid_mid*grid_width;
	grid_size = grid_width*grid_height;
	if (reset && cache_page_handle != NULL) {
		release_page_grid();
	};
   cache_page_handle = global_alloc(GMEM_FIXED|GMEM_ZEROINIT,tt_screen_bytes_size);
   cache_page = (work_page) GlobalLock(cache_page_handle);
//#if TT_DIRECTX
//   if (tt_page_flipping && tt_screen_width != 320 && tt_debug_mode == 1954) {
//      cache_page2 = new unsigned char[tt_screen_bytes_size];
//   };
//#endif
	grid_time_stamps = new long[grid_size];
//#if TT_DIRECTX
//   if (cache_page2 != NULL) grid_time_stamps2 = new long[grid_size];
//#endif
	reset_grid();
//	grid_touched = new flag[grid_size];
//	grid_saved = new flag[grid_size];
//	memset(grid_touched,FALSE,grid_size);
//	memset(grid_saved,FALSE,grid_size);
//	for (int i = 0; i < grid_size; i++) {
//#if STATIC_GRID_ALLOCATION
//		grid[i] = new unsigned char[chunk_size];
//#else	
//		if (grid != NULL) {
//			grid[i] = NULL;
//		};
//#endif
//		grid_touched[i] = 0;
//	};
//	tt_dirty_min_x = 0;
//	tt_dirty_min_y = 0;
//	tt_dirty_max_x = tt_screen_width;
//	tt_dirty_max_y = tt_screen_height;
};

//#if TT_DIRECTX
//void copy_cache_pages() {
//   if (tt_using_dispdib != 1) return;
//   if (tt_frame_number&1) {
//      memcpy(cache_page1,cache_page2,tt_screen_bytes_size);
//   } else {
//      memcpy(cache_page2,cache_page1,tt_screen_bytes_size);
//   };
//};
//#endif

void release_page_grid() {
   if (grid_time_stamps != NULL) delete [] grid_time_stamps;
//#if TT_DIRECTX
//   if (grid_time_stamps2 != NULL) delete [] grid_time_stamps2;
//#endif
   GlobalUnlock(cache_page_handle);
	GlobalFree(cache_page_handle);
//#if TT_DIRECTX
//   if (cache_page2 != NULL) delete [] cache_page2;
//#endif
};

void reset_grid(
//#if TT_DIRECTX
//                boolean both_pages
//#endif
                ) {
//#if TT_DIRECTX
//   if (!both_pages && grid_time_stamps2 != NULL) { //tt_page_flipping && tt_screen_width != 320) {
      // just reset "current grid"
//      if (tt_frame_number&1) {
//         for (int i = 0; i < grid_size; i++) grid_time_stamps1[i] = 0;
//#if TT_DEBUG_ON
//	      if (tt_debug_mode == 103) {
//           tt_error_file() << "Grid1 only reset" << endl;
//	      };
//#endif
//      } else {
//         for (int i = 0; i < grid_size; i++) grid_time_stamps2[i] = 0;
//#if TT_DEBUG_ON
//	      if (tt_debug_mode == 103) {
//            tt_error_file() << "Grid2 only reset" << endl;
//	      };
//#endif
//      };
//      return;
//   };
//#endif
   for (int i = 0; i < grid_size; i++) {
      grid_time_stamps[i] = 0;
//#if TT_DIRECTX
//      if (grid_time_stamps2 != NULL) grid_time_stamps2[i] = 0;
//#endif
   };
#if TT_DEBUG_ON
	if (tt_debug_mode == 103 || tt_debug_mode == 250404) {
		tt_error_file() << "Grid reset" << endl;
	};
#endif
//	if (tt_time_travel) {
//		add_time_travel_buttons();
//	};
};

boolean save_region(coordinate min_x, coordinate max_x, coordinate min_y, coordinate max_y) {
	// frame_number added 250404 for time travel cursor and removed soon afterwards
//	if (frame_number < 0) { // removed as arg on 111104 since wasn't being used
	int frame_number = tt_frame_number; // normal behavior
//	};
	// extend region by a pixel on all side to avoid round-off causing turds
	if (min_x > 0) { // new on 11104 to avoid negative numbers here
		min_x--;
	};
	max_x++;
	if (min_y > 0) { // new on 11104 to avoid negative numbers here
		min_y--;
	};
	max_y++;
	if (min_x >= max_x || min_y >= max_y) {
		return(FALSE);
	};
	if (tt_dirty_min_x > min_x) tt_dirty_min_x = min_x;
	if (tt_dirty_min_y > min_y) tt_dirty_min_y = min_y;
	if (tt_dirty_max_x < max_x) tt_dirty_max_x = max_x;
	if (tt_dirty_max_y < max_y) tt_dirty_max_y = max_y;
	int min_grid_x = min_x/chunk_width;
// need another grid box if just fits
	int max_grid_x = max_x/chunk_width+2;
   if (max_grid_x > grid_width) max_grid_x = grid_width;
	int min_grid_y = min_y/chunk_height;
	int max_grid_y = max_y/chunk_height+2;
   if (max_grid_y > grid_height) max_grid_y = grid_height;
	int grid_index = min_grid_x+min_grid_y*grid_width;
	const int unused_width = grid_width-(max_grid_x-min_grid_x);
#if TT_DEBUG_ON
	if (tt_debug_mode == 103) {
		tt_error_file() << "Touching region "
							 << min_grid_x << "," << min_grid_y << " to "
							 << (max_grid_x-1) << "," << (max_grid_y-1) << endl;
	};
#endif
//   long *grid_time_stamps;
#if TT_DIRECTX
//   work_page cache_page;
//   if (!tt_page_flipping || tt_frame_number&1 || cache_page2 == NULL) { // last was tt_screen_width == 320
//      cache_page = cache_page1;
//      grid_time_stamps = grid_time_stamps1;
//      log("Save region using page1");
//   } else {
//      cache_page = cache_page2;
//      grid_time_stamps = grid_time_stamps2;
//      log("Save region using page2");
//   };
   lock_back_surface();
//#else
//   work_page cache_page = cache_page1;
//   grid_time_stamps = grid_time_stamps1;
#endif
	for (int grid_y = min_grid_y; grid_y < max_grid_y; grid_y++) {
		for (int grid_x = min_grid_x; grid_x < max_grid_x; grid_x++) {
			if (grid_time_stamps[grid_index] == 0) { // not already saved
#if TT_DEBUG_ON
				if (tt_debug_mode == 103 || tt_debug_mode == 250404) {
					tt_error_file() << "Saving region "
										 << grid_x << "," << grid_y
										 << " first grid#=" << grid_index;
				};
#endif
				tt_memory_graphics_mark_x = grid_x*chunk_width;
				tt_memory_graphics_mark_y = grid_y*chunk_height;
				int width = chunk_width;
            grid_time_stamps[grid_index] = frame_number;
				for (grid_x++; grid_x < max_grid_x; grid_x++) {
					grid_index++;
					if (grid_time_stamps[grid_index] != 0) {
						grid_index--;
						grid_x--;
						break;
					};
					width += chunk_width;
					grid_time_stamps[grid_index] = frame_number;
				};
				copy_region(width,chunk_height,
								tt_destination_work_page,tt_destination_width,TRUE,
                        cache_page,tt_screen_width,TRUE);
#if TT_DEBUG_ON
				if (tt_debug_mode == 103 || tt_debug_mode == 250404) {
					tt_error_file() << " width=" << width
										 << " last_grid#=" << grid_index
										 << " frame=" << frame_number <<  endl;
				};
#endif
			};
		   grid_time_stamps[grid_index] = frame_number;
			grid_index++;
		};
		grid_index += unused_width;
	};
#if TT_DIRECTX
   unlock_back_surface();
#endif
	return(TRUE);
};

void save_region_to_memory(coordinate min_x, coordinate max_x, coordinate min_y, coordinate max_y, bytes memory) { // new on 250404
	coordinate width = 1+max_x-min_x;
	if (width <= 0) return;
	coordinate height = 1+max_y-min_y;
	if (height <= 0) return;
	coordinate padded_width = width;
	if (padded_width&3) { // low order bits set
		// adjust to long boundary
		padded_width += 4-padded_width%4;
	};
	if (lock_back_surface()) { // conditionalized on 290404
		tt_memory_graphics_mark_x = min_x;
		tt_memory_graphics_mark_y = min_y;
		copy_region(width,height,
						tt_destination_work_page,tt_destination_width,TRUE,
						memory,padded_width,FALSE);
#if TT_DEBUG_ON
		if (tt_debug_mode == 2904041) {
			tt_error_file() << "   Saving region that is " << width << "x" << height << " lower left corner is " << min_x << "," << min_y << " on frame " << tt_frame_number << endl;
		};
#endif
		unlock_back_surface();
	};
};

void restore_region_from_memory(coordinate min_x, coordinate max_x, coordinate min_y, coordinate max_y, bytes memory) { // new on 250404
	coordinate width = 1+max_x-min_x;
	if (width <= 0) return;
	coordinate height = 1+max_y-min_y;
	if (height <= 0) return;
	coordinate padded_width = width;
	if (padded_width&3) { // low order bits set
		// adjust to long boundary
		padded_width += 4-padded_width%4;
	};
	if (lock_back_surface()) { // made conditional on 290404
		tt_memory_graphics_mark_x = min_x;
		tt_memory_graphics_mark_y = min_y;
		copy_region(width,height,
						memory,padded_width,FALSE,
						tt_destination_work_page,tt_destination_width,TRUE);
#if TT_DEBUG_ON
		if (tt_debug_mode == 2904041) {
			tt_error_file() << "Restoring region that is " << width << "x" << height << " lower left corner is " << min_x << "," << min_y << " on frame " << tt_frame_number << endl;
		};
#endif
		unlock_back_surface();
	};
};

void restore_regions(long last_frame) { // arg new on 250404 -- used to make cursor during time travel
	if (last_frame < 0) {
		last_frame = tt_frame_number-1; // normal behavior
	};
	// compute new ones below
	tt_dirty_min_x = max_long; 
	tt_dirty_min_y = max_long; 
	tt_dirty_max_x = 0;
	tt_dirty_max_y = 0; 
//	screen_dirty();
	int grid_index = 0;
//   long *grid_time_stamps;
//#if TT_DIRECTX
 //  work_page cache_page;
//   const long last_frame // cache_page2 != NULL was tt_page_flipping && tt_screen_width != 320
////      = (cache_page2 != NULL)?tt_frame_number-2:
//	   = tt_frame_number-1;
//   if (!(tt_frame_number&1) && cache_page2 != NULL) {
//      cache_page = cache_page2;
//      grid_time_stamps = grid_time_stamps2;
//           log("restore region using page2");
//   } else {
//      cache_page = cache_page1;
//      grid_time_stamps = grid_time_stamps1;
//           log("restore region using page1");
//   };
   lock_back_surface();
//#else
////   work_page cache_page = cache_page1;
//   const long last_frame = tt_frame_number-1;
////   grid_time_stamps = grid_time_stamps1; 
//#endif
	for (int grid_y = 0; grid_y < grid_height; grid_y++) {
		for (int grid_x = 0; grid_x < grid_width; grid_x++) {
			if (grid_time_stamps[grid_index] == last_frame
//#if TT_DIRECTX // this assumes just one flipping page
//             || (tt_using_dispdib == 1 &&
//                 grid_time_stamps[grid_index] == last_frame-1)
//#endif
             ) {
				// touched last frame so restore
#if TT_DEBUG_ON
				if (tt_debug_mode == 103 || tt_debug_mode == 250404) {
					tt_error_file() << "Restoring box " << grid_x << "," << grid_y
                           << " first grid#=" << grid_index;
				};
#endif
				tt_memory_graphics_mark_x = grid_x*chunk_width;
				tt_memory_graphics_mark_y = grid_y*chunk_height;
#if TT_WINDOWS
				int width = chunk_width;
				for (grid_x++; grid_x < grid_width; grid_x++) {
					grid_index++;
					if (grid_time_stamps[grid_index] != last_frame
//#if TT_DIRECTX
//                   && (grid_time_stamps[grid_index] != last_frame-1 ||
//                       tt_using_dispdib != 1)
//#endif
                   ) {
					   grid_index--;
						grid_x--;
						break;
					};
					width += chunk_width;
//					grid_time_stamps[grid_index-1] = tt_frame_number; // to be reset next round
				};
#if TT_DEBUG_ON
				if (tt_debug_mode == 103 || tt_debug_mode == 250404) {
					tt_error_file() << " width=" << width
                           << " last grid#=" << grid_index
									<< " last_frame=" << last_frame <<  endl;
				};
#endif
//				restore_box(width,chunk_height);//grid[grid_index]);
				copy_region(width,chunk_height,
								cache_page,tt_screen_width,TRUE,
                        tt_destination_work_page,tt_destination_width,TRUE);
//#if TT_32
//								grid_page_top+grid_index*chunk_size);
//#else
//								top?grid_page_top+grid_index*chunk_size:
//									 grid_page_bottom+(grid_index-grid_top_last_index)*chunk_size);
//#endif
#else			  
				int max_x = min_x+chunk_width-1;
				int max_y = min_y+chunk_height-1;
				restore_image(grid_index,min_x,max_x,min_y,max_y);
#endif
				// could optimize the following
				if (tt_dirty_min_x > tt_memory_graphics_mark_x)
					tt_dirty_min_x = tt_memory_graphics_mark_x;
				if (tt_dirty_min_y > tt_memory_graphics_mark_y) 
					tt_dirty_min_y = tt_memory_graphics_mark_y;
				if (tt_dirty_max_x < tt_memory_graphics_mark_x+width-1) 
					tt_dirty_max_x = tt_memory_graphics_mark_x+width-1;
				if (tt_dirty_max_y < tt_memory_graphics_mark_y+chunk_height-1) 
					tt_dirty_max_y = tt_memory_graphics_mark_y+chunk_height-1;		  
			};
//			grid_time_stamps[grid_index] = tt_frame_number; // to be reset next round
			grid_index++;
		};
	};
#if TT_DIRECTX
   unlock_back_surface();
#endif
};

/*
#if TT_DIRECTX
void copy_back_surface_to_cache_page() {
   // only called when in full screen mode (with flipping)
   if (cache_page2 == NULL) return;
#if TT_DEBUG_ON
   millisecond start_time;
   if (tt_debug_mode == 3) {
	   start_time = timeGetTime();
   };
#endif
   // update the other cache page with the current back surface
   long *grid_time_stamps;
   work_page cache_page;
   if (tt_frame_number&1) {
      // opposite the usual
      cache_page = cache_page2;
      grid_time_stamps = grid_time_stamps2;
//      log("copying back surface to cache page2");
   } else {
      cache_page = cache_page1;
      grid_time_stamps = grid_time_stamps1;
//      log("copying back surface to cache page1");
   };
   lock_back_surface();
   copy_work_page(tt_destination_work_page,cache_page,
                  tt_destination_width,tt_screen_width);
   unlock_back_surface();
   // may as well update the time stamps too
   for (int i = 0; i < grid_size; i++) {
      grid_time_stamps[i] = tt_frame_number;
   };
#if TT_DEBUG_ON
	if (tt_debug_mode == 103) {
		tt_error_file() << "Setting all grid time stamps to " << tt_frame_number << endl;
	};
   if (tt_debug_mode == 3) {
	   tt_error_file() << (timeGetTime()-start_time)
                  << " ms to copy back surface to cache page." << endl;
   };
#endif
};

void copy_cache_page_to_back_surface() {
   if (cache_page2 == NULL) return;
 // this is usually called when cache page has the entire state
 // of previous frame and next surface needs updating
#if TT_DEBUG_ON
   millisecond start_time;
   if (tt_debug_mode == 3) {
	   start_time = timeGetTime();
   };
#endif
   work_page cache_page;
   if (!(tt_frame_number&1)) {
      // opposite the usual
      cache_page = cache_page2;
//      log("copying cache page2 to back surface");
   } else {
      cache_page = cache_page1;
//      log("copying cache page1 to back surface");
   };
   lock_back_surface();
   copy_work_page(cache_page,tt_destination_work_page,
                  tt_screen_width,tt_destination_width);
   unlock_back_surface();
#if TT_DEBUG_ON
  if (tt_debug_mode == 3) {
	  tt_error_file() << (timeGetTime()-start_time)
                 << " ms to copy cache page to back surface." << endl;
  };
#endif
};
#endif


#if !TT_WINDOWS //|| GRAPHICS_TO_MEMORY
void save_image_to_memory(int grid_index,
								  int min_x, int max_x, int min_y, int max_y) {
//#if !STATIC_GRID_ALLOCATION
//	grid[grid_index] = new unsigned char[chunk_size];
//#endif
#if FAST_GRAPH
	fg_getblock(grid[grid_index],min_x,max_x,min_y,max_y);
#elif GRAPHICS_TO_MEMORY
	save_box(min_x,min_y,max_x,max_y,grid[grid_index]);
//   fg_box_t bounding_box;
//   fg_make_box(bounding_box,min_x,min_y,max_x,max_y);
//   fg_readbox(bounding_box,min_x,min_y,max_x,max_y);
#endif
};
#endif

#if FAST_GRAPH||(TT_WINDOWS&&!GRAPHICS_TO_MEMORY)
void save_image_to_page(int ,
								int min_x, int max_x, int min_y, int max_y) {
#if TT_WINDOWS
	 tt_main_window->save_region(min_x,min_y,max_x,max_y);
#else
	fg_sethpage(tt_screen_cache_page);
	fg_save(min_x,max_x,min_y,max_y);
#endif
//   fg_box_t bounding_box;
//   fg_make_box(bounding_box,min_x,min_y,max_x,max_y);
//   fg_blit(bounding_box,min_x,min_y,fg.activepage,tt_screen_cache_page);
};
#endif

#if !TT_WINDOWS //|| GRAPHICS_TO_MEMORY
void restore_image_from_memory(int grid_index,
										 int min_x, int max_x, int min_y, int max_y) {
#if TT_DEBUG_ON
	if (grid != NULL && grid[grid_index] == NULL) {
		tt_error_file() << grid_index << " has no saved image despite its time stamp of "
						<< grid_touched[grid_index] << endl;
		return;
	};
#endif
#if FAST_GRAPH
	fg_putblock(grid[grid_index],min_x,max_x,min_y,max_y);
#elif GRAPHICS_TO_MEMORY
	restore_box(min_x,min_y,max_x,max_y,grid[grid_index]);
//   fg_box_t bounding_box;
//   fg_make_box(bounding_box,min_x,min_y,max_x,max_y);
//   fg_writebox(bounding_box,grid[grid_index]);
#endif
};
#endif

#if FAST_GRAPH||(TT_WINDOWS&&!GRAPHICS_TO_MEMORY)
void restore_image_from_page(int,
									  int min_x, int max_x, int min_y, int max_y) {
#if TT_WINDOWS
	 tt_main_window->restore_region(min_x,min_y,max_x,max_y);
#else
	fg_sethpage(tt_screen_cache_page);
	fg_restore(min_x,min_y,max_x,max_y);
#endif
//#if FLASH_GRAPHICS
//   fg_box_t bounding_box;
//   fg_make_box(bounding_box,min_x,min_y,max_x,max_y);
//   fg_blit(bounding_box,min_x,min_y,tt_screen_cache_page,fg.activepage);
//#endif
};
#endif


			if (!grid_touched[grid_index]&SAVED_BIT) { // not already saved
				tt_memory_graphics_mark_x = grid_x*chunk_width;
				tt_memory_graphics_mark_y = grid_y*chunk_height;
				int width = chunk_width;
				for (grid_x++; grid_x < max_grid_x; grid_x++) {
					grid_index++;
					if (grid_touched[grid_index]&SAVED_BIT) {
						 grid_index--;
						 grid_x--;
						 break;
					};
					width += chunk_width;
					grid_touched[grid_index-1] = SAVED_BIT;
				};
				grid_touched[grid_index] = SAVED_BIT;
				copy_region(width,chunk_height,
								tt_destination_work_page,cache_page);
#if TT_DEBUG_ON
				if (tt_debug_mode == 103) {
					tt_error_file() << "Saving region " << grid_x << "," << grid_y
									<< " width=" << width 
									<< " frame=" << tt_frame_number <<  endl;
				};
#endif
			};
			grid_touched[grid_index] |= TOUCHED_BIT;
//			 if (grid_index != grid_x+grid_y*grid_width) {
//				 tt_error_file() << "oops";
//			 };
			grid_index++;
		};
		grid_index += grid_untouched_width;
	};
*/
//			if (grid_touched[grid_index] == 0) { // not already saved
//				tt_memory_graphics_mark_x = grid_x*chunk_width;
//				tt_memory_graphics_mark_y = grid_y*chunk_height;
//#if TT_32
//							grid_page_top+grid_index*chunk_size);
//#else
//							top?grid_page_top+grid_index*chunk_size:
//								 grid_page_bottom+(grid_index-grid_top_last_index)*chunk_size);
//#endif
//				int pixel_min_x = grid_x*chunk_width;
//				int pixel_min_y = grid_y*chunk_height;
//				save_image(grid_index,
//							  pixel_min_x,pixel_min_x+chunk_width-1,
//							  pixel_min_y,pixel_min_y+chunk_height-1);
//          poll_input_devices(FALSE);
//			};

//#if FAST_GRAPH
//			+2; // when to expire
//#else // does this make sense??
//			+1; // fewer pages
//#endif
//		};
//	};
//};

//          poll_input_devices(FALSE);
//#if TT_DEBUG_ON
//		if (tt_debug_mode == 3) {
//			bug_min_x = min(bug_min_x,min_x);
//			bug_max_x = max(bug_max_x,min_x+chunk_width-1);
//			bug_min_y = min(bug_min_y,min_y);
//			bug_max_y = max(bug_max_y,min_y+chunk_height-1);
//		};
//#endif
/*
				fg_setvpage(tt_work_page);
				fg_setcolor(YELLOW);
				fg_box(pixel_min_x,pixel_min_x+chunk_width-1,
								pixel_min_y,pixel_min_y+chunk_height-1);
				fg_waitfor(1);
				fg_setvpage(1-tt_work_page);
*/
//			if (grid_touched[i] == tt_frame_number) {
				// hasn't been written to in n frames
//#if !STATIC_GRID_ALLOCATION
//				if (grid != NULL) {
//#if TT_DEBUG_ON				
//					if (grid[i] == NULL) {
//						tt_error_file() << "Dirty region structures broken!" << endl;
//						tt_exit_failure();
//					};
//#endif
//					delete [] grid[i];
//					grid[i] = NULL;
//				};
//#endif
//				grid_touched[i] = 0;
//			};
//             if (tt_debug_mode == 3) {
//                tt_error_file() << "Releasing " << i << endl;
//             };
//		};
//			i++;
//		};
//	};
//#if TT_DEBUG_ON
//	if (tt_debug_mode == 3) {
//		tt_error_file() << "Restoring "
//						<< bug_min_x << " " << bug_max_x << " "
//						<< bug_min_y << " " << bug_max_y << endl;
//	};
//#endif
//};

/*
void dirty_region(int &min_x, int &min_y, int &max_x, int &max_y) {
	min_x = tt_dirty_min_x;
	min_y = tt_dirty_min_y;
	max_x = tt_dirty_max_x;
	max_y = tt_dirty_max_y;
};
*/
