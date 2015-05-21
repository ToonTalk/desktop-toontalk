// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved
#ifndef __TT_BLIT_H
#define __TT_BLIT_H

//#if TT_32
//#define HMEMCPY memcpy
//#else
//#define HMEMCPY hmemcpy
//#endif

// to avoid overflow problems
typedef long int integer;
typedef unsigned long int uinteger;

/* inlined now
void save_box(coordinate min_x, coordinate min_y,
				  coordinate max_x, coordinate max_y,
				  bytes buffer);

void restore_box(coordinate min_x, coordinate min_y,
					  coordinate max_x, coordinate max_y,
					  bytes buffer);
*/

void clip(long &source_width, long &source_height,
			 integer &left_offset, integer &right_offset,
			 integer &top_offset, integer &bottom_offset);

void blit(bytes source,
			 int original_source_width, int original_source_height);
//			 coordinate source_llx, coordinate source_lly);
//			 work_page destination,
//			 integer destination_width, integer destination_height,
//			 coordinate destination_llx=0, coordinate destination_lly=0);

void blit_ignore(bytes source,
					  int original_source_width, int width_with_padding, int original_source_height);
//					  coordinate source_llx, coordinate source_lly);
//					  work_page destination,
//					  integer destination_width, integer destination_height,
//					  coordinate destination_llx=0, coordinate destination_lly=0);

//void blit_switch(bytes source,
//					  int original_source_width, int original_source_height);
//					  coordinate source_llx, coordinate source_lly);
//					  work_page destination,
//					  integer destination_width, integer destination_height,
//					  coordinate destination_llx=0, coordinate destination_lly=0);

void draw_frame(color_index frame_color);
void darken_with_dark_colors();
void darken_with_color(color_index frame_color);
void clear_page(work_page page, color_index color);

void blit_scale_ignore(bytes source, 
							  uinteger pixel_width,
							  uinteger pixel_height,
							  uinteger x_scale, uinteger y_scale);
//							  coordinate llx, coordinate lly);
//							  work_page destination,
//							  integer destination_width, integer destination_height);

void blit_scale(bytes source,
					 uinteger pixel_width,
					 uinteger pixel_height,
					 uinteger x_scale, uinteger y_scale);
//					 coordinate llx, coordinate lly);
//							  work_page destination,
//							  integer destination_width, integer destination_height);

void permute_colors(work_page image, long size, bytes permutation);

void permute_pixelrun(bytes pixel_runs, const uinteger size,
							 bytes permutation);

void blit_pixelrun_scale_and_ignore(bytes pixel_runs, const uinteger size,
										uinteger pixel_width,
										uinteger pixel_height,
										uinteger x_scale, uinteger y_scale);
//										coordinate llx, coordinate lly);
//										work_page destination,
//										integer destination_width, integer destination_height);

void blit_pixelrun_ignore(bytes pixel_runs, const uinteger size,
								  uinteger width, uinteger height);
//								  coordinate llx, coordinate lly);
//								  work_page destination,
//								  integer destination_width, integer destination_height,
//								  coordinate destination_llx=0, coordinate destination_lly=0);

#if TT_DIRECTX
void copy_work_page(work_page source_page, work_page destination_page,
                    coordinate source_pitch, coordinate destination_pitch,
                    coordinate width=tt_screen_width,
                    coordinate height=tt_screen_height);
//void copy_work_page_alpha_to_black(work_page source_page, work_page destination_page,
//											  coordinate source_pitch, coordinate destination_pitch,
//											  coordinate width=tt_screen_width,
//											  coordinate height=tt_screen_height);
void mask_work_page(coordinate pixel_width, coordinate pixel_height,
						  work_page surface_bytes, int pitch, int bytes_per_pixel,
						  work_page mask_surface_bytes, int mask_pitch, int mask_bytes_per_pixel);
void copy_and_mask_work_page(work_page destination_bytes, int destination_pitch, coordinate pixel_width, coordinate pixel_height,
									  work_page surface_bytes, int pitch, int bytes_per_pixel,
									  work_page mask_surface_bytes, int mask_pitch, int mask_bytes_per_pixel);
void permute_work_page(work_page source, work_page destination,
							  coordinate source_pitch, coordinate destination_pitch,
							  coordinate width, coordinate height, bytes permutation, int transparent_color);
void recolor_work_page(work_page source_bytes, work_page destination_bytes,
							  coordinate source_pitch, coordinate destination_pitch,
							  coordinate width, coordinate height, bytes permutation, int transparent_color);
void copy_replacing_white_with_black(work_page source_bytes, work_page destination_bytes,
												 coordinate source_pitch_in_bytes, coordinate destination_pitch_in_bytes,
												 coordinate width, coordinate height);
#else
inline 
void copy_work_page(work_page source_page, work_page destination_page,
                    coordinate , coordinate ) {
	HMEMCPY(destination_page,source_page,tt_screen_bytes_size);
};
#endif

void copy_region(coordinate width, coordinate height, 
					  work_page source_page, coordinate source_pitch, boolean compute_source_offset,
                 work_page destination_page, coordinate destination_pitch, boolean compute_destination_offset);
					  
// when caching copying entire working page to cache page -- or just region?
// restore_box just uses hmemcpy to go between them

//inline 
//void save_box(//coordinate min_x, coordinate min_y,
				  //coordinate max_x, coordinate max_y,
//				  coordinate width, coordinate height,
//				  bytes buffer) {
  // maybe source box should be here
//  integer width = max_x-min_x+1;
//  integer height = max_y-min_y+1;
//  blit_switch(buffer,width,height);
//  ,min_x,min_y);
//				  tt_memory_graphics_work_page,
//				  tt_screen_width,tt_screen_height);
//};

//inline 
//void restore_box(//coordinate min_x, coordinate min_y,
					  //coordinate max_x, coordinate max_y,
//					  coordinate width, coordinate height,
//					  bytes buffer) {
//  const integer width = max_x-min_x+1;
//  const integer height = max_y-min_y+1;
//  blit(buffer,width,height);
//  ,min_x,min_y);
//		 tt_memory_graphics_work_page,
//		 tt_screen_width,tt_screen_height);
//};


#if !WING&&!TT_DIRECTX		 
void fill_box(coordinate min_x, coordinate min_y,
				  coordinate max_x, coordinate max_y,
				  color the_color);

void blit_color(color the_color, integer width, integer height);
//					 coordinate source_llx, coordinate source_lly);
//					 work_page destination,
//					 integer destination_width, integer destination_height,
//					 coordinate destination_llx=0, coordinate destination_lly=0);
#endif

#endif
