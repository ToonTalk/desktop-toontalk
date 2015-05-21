// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved
  
#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif   
#if !defined(__TT_GLOBALS_H)
#include "globals.h"
#endif
#if !defined(__TT_UTILS_H)
#include "utils.h"
#endif
#if !defined(__TT_BLIT_H)   
#include "blit.h"
#endif   


#include <windows.h>
#if !TT_32
#include <mem.h>
#include <dos.h> // for FP_SEG
#include <mmsystem.h> // for timeGetTime
#endif

inline
long offset(integer width, coordinate llx, coordinate lly) {
	if (llx < 0) llx = 0;
	if (lly < 0) lly = 0;
	return(((long) lly)*((long) width)+llx);
};

// should clip be involved with tt_memory_graphics_mark_x and tt_memory_graphics_mark_y ??
#if !TT_DEBUG_ON
inline
#endif
void clip(long &source_width, long &source_height,
//			 coordinate &tt_memory_graphics_mark_x, coordinate &tt_memory_graphics_mark_y,
//			 int destination_width, int destination_height,
//			 coordinate destination_llx, coordinate destination_lly,
			 integer &left_offset, integer &right_offset,
			 integer &top_offset, integer &bottom_offset) {
	coordinate source_urx = (coordinate) (tt_memory_graphics_mark_x+source_width-tt_bytes_per_pixel); // prior to 021001 tt_bytes_per_pixel was 1
	coordinate source_ury = (coordinate) (tt_memory_graphics_mark_y+source_height-tt_bytes_per_pixel);
//	coordinate destination_urx 
//		= min(tt_destination_urx,tt_destination_width-1);
//	coordinate destination_ury 
//		= min(tt_destination_ury,tt_destination_height-1);
	if (tt_memory_graphics_mark_x > tt_destination_urx ||
		 tt_memory_graphics_mark_y > tt_destination_ury ||
		 source_urx < tt_destination_llx ||
		 source_ury < tt_destination_lly ||
		 source_urx < 0 ||
		 source_ury < 0  || // completely offscreen
		 tt_destination_urx <= tt_destination_llx || // clipping off screen
		 tt_destination_ury <= tt_destination_lly) {
		source_width = 0;
		source_height = 0;
		return;
	};
	left_offset = 0;
	right_offset = 0;
	top_offset = 0;
	bottom_offset = 0;
	if (tt_destination_llx < 0) {
		tt_destination_llx = 0;
	};
	if (tt_destination_lly < 0) {
		tt_destination_lly = 0;
	};
	if (tt_memory_graphics_mark_x < tt_destination_llx) {
		left_offset = tt_destination_llx-tt_memory_graphics_mark_x;
		source_width -= left_offset;
		tt_memory_graphics_mark_x = tt_destination_llx;
	};
	if (source_urx > tt_destination_urx) {
		right_offset = source_urx-tt_destination_urx;
		source_width -= right_offset;
	};
	if (tt_memory_graphics_mark_y < tt_destination_lly) {
		bottom_offset = tt_destination_lly-tt_memory_graphics_mark_y;
		source_height -= bottom_offset;
		tt_memory_graphics_mark_y = tt_destination_lly;
	};
	if (source_ury > tt_destination_ury) {
		top_offset = source_ury-tt_destination_ury;
		source_height -= top_offset;
	};
};

void copy_region(coordinate width, coordinate height,
					  work_page source_page, coordinate source_pitch, boolean compute_source_offset,
                 work_page destination_page, coordinate destination_pitch, boolean compute_destination_offset) {
  // copies region between full screen work pages -- added compute_offset args so can be used with non-full-page sources or destinations
	source_pitch *= tt_bytes_per_pixel; // new on 240901
	destination_pitch *= tt_bytes_per_pixel; 
	work_page destination;
	if (compute_destination_offset) {
		destination = destination_page+offset(destination_pitch,tt_memory_graphics_mark_x*tt_bytes_per_pixel,tt_memory_graphics_mark_y);
	} else {
		destination = destination_page;
	};
	work_page source;
	if (compute_source_offset) {
		source = source_page+offset(source_pitch,tt_memory_graphics_mark_x*tt_bytes_per_pixel,tt_memory_graphics_mark_y);
	} else {
		source = source_page;
	};
/* for debugging
   long d = offset(destination_pitch,tt_memory_graphics_mark_x*tt_bytes_per_pixel,tt_memory_graphics_mark_y);
   long s = offset(source_pitch,tt_memory_graphics_mark_x*tt_bytes_per_pixel,tt_memory_graphics_mark_y);
#if TT_DEBUG_ON
   if (tt_debug_mode == 250901) {
      tt_error_file() << "Size is " << width << "x" << height;
      tt_error_file() << " ;source is " << s << " ;dest is " << d;
		tt_error_file() << " ;mark_x=" << tt_memory_graphics_mark_x << " ;mark_y=" << tt_memory_graphics_mark_y << endl;
   };
#endif
	*/
	int width_in_bytes = width*tt_bytes_per_pixel;
	for (int y = 0; y < height-1; y++) {
		memcpy(destination,source,width_in_bytes); // width); updated on 240901
		destination += destination_pitch;
		source += source_pitch;
/*
		d += destination_pitch;
      s += source_pitch;
#if TT_DEBUG_ON
      if (tt_debug_mode == 250901) {
         tt_error_file() << " Source is " << s << " ;dest is " << d << endl;
      };
#endif
		*/
	};
   // do last one afterwards so debugger doesn't complain about
   // computing bad addresses on last iteration above
	memcpy(destination,source,width_in_bytes); // width); updated on 240901
};

#if TT_DIRECTX
void copy_work_page(work_page source, work_page destination,
                    coordinate source_pitch, coordinate destination_pitch,
                    coordinate width, coordinate height) {
	int width_in_bytes = width*tt_bytes_per_pixel; // new on 230901
	for (int y = 0; y < height-1; y++) {
		memcpy(destination,source,width_in_bytes);
#if TT_DEBUG_ON
		if (tt_debug_mode == 121001 && tt_bits_per_pixel == 32) {
			for (int i = 0; i < width; i++) {
				tt_error_file() << (int) source[i*4+3] << " ";
			};
			tt_error_file() << endl;
		};
#endif
		destination += destination_pitch;
		source += source_pitch;
	};
   // do last one afterwards so debugger doesn't complain about
   // computing bad addresses on last iteration above
	memcpy(destination,source,width_in_bytes);
};

//void copy_line_alpha_to_black(work_page destination, work_page source, int width_in_bytes) { // new on 190303
//	// if alpha is 0 then completely transparent so replace with black which is default transparent color
//	// if any pixels are black already replace with nearly black
//	switch (tt_bits_per_pixel) {
//		case 32:
//		default: // do others later 
//			for (int i = 0; i < width_in_bytes; i += 4) {
//				if (source[i+3] == 0) { // transparent
//					destination[i+3] = 255;
//					destination[i] = 0; 
//					destination[i+1] = 0; 
//					destination[i+2] = 0; 
//				} else if (source[i] == 0 && source[i+1] == 0 && source[i+2] == 0) { // black
//					destination[i+3] = 255; // make nearly black
//					destination[i+1] = 1; 
//					destination[i+2] = 1; 
//					destination[i+3] = 1; 
//				} else {
//					destination[i] = source[i];
//					destination[i+1] = source[i+1];
//					destination[i+2] = source[i+2];
//					destination[i+3] = source[i+3];
//				};
//			};
//			break;
//	};
//};
//
//void copy_work_page_alpha_to_black(work_page source, work_page destination,
//											  coordinate source_pitch, coordinate destination_pitch,
//											  coordinate width, coordinate height) {
//	// new on 190303
//	int width_in_bytes = width*tt_bytes_per_pixel;
//	for (int y = 0; y < height-1; y++) {
//		copy_line_alpha_to_black(destination,source,width_in_bytes);
//		destination += destination_pitch;
//		source += source_pitch;
//	};
//   // do last one afterwards so debugger doesn't complain about
//   // computing bad addresses on last iteration above
//	copy_line_alpha_to_black(destination,source,width_in_bytes);
//};

void mask_work_page(coordinate pixel_width, coordinate pixel_height,
						  work_page surface_bytes, int pitch, int bytes_per_pixel,
						  work_page mask_surface_bytes, int mask_pitch, int mask_bytes_per_pixel) {
	// rewritten on 021101 since this is just when source and destination are the same
	copy_and_mask_work_page(surface_bytes,pitch,pixel_width,pixel_height,
									surface_bytes,pitch,bytes_per_pixel,
									mask_surface_bytes,mask_pitch,mask_bytes_per_pixel);
};
/*
#if TT_DEBUG_ON
	work_page original_surface_bytes = surface_bytes;
	if (tt_debug_mode == 270901) {
		tt_error_file() << "Surface bytes: " << endl;
		int i;
		for (i = 0; i < pixel_width; i++) {
			tt_error_file() << i << ": ";
			for (int j = 0; j < bytes_per_pixel; j++) {
				tt_error_file() << (int) surface_bytes[i*bytes_per_pixel+j] << " ";
			};
			tt_error_file() << "   ";
		};
		tt_error_file() << endl;
		tt_error_file() << "Mask bytes: " << endl;
		for (i = 0; i < pixel_width; i++) {
			tt_error_file() << i << ": ";
			for (int j = 0; j < mask_bytes_per_pixel; j++) {
				tt_error_file() << (int) mask_surface_bytes[i*mask_bytes_per_pixel+j] << " ";
			};
			tt_error_file() << "   ";
		};
		tt_error_file() << endl;
		tt_error_file() << endl;
	};
#endif
	if (tt_bits_per_pixel == 32) { // new on 041001
		for (int y = 0; y < pixel_height; y++) {
			int *source = (int *) surface_bytes;
//			int *mask = (int *) mask_surface_bytes;
			for (int i = 0; i < pixel_width; i++) {
				if (mask_surface_bytes[i*mask_bytes_per_pixel] == 0) { 
					source[i] = 0;
				};
			};
			if (y < pixel_height-1) {
				surface_bytes += pitch;
				mask_surface_bytes += mask_pitch;
			};
		};
	} else {
		for (int y = 0; y < pixel_height; y++) {
			for (int i = 0; i < pixel_width; i++) {
				if (mask_surface_bytes[i*mask_bytes_per_pixel] == 0) { // should really check all 3 bytes
					for (int j = 0; j < bytes_per_pixel; j++) {
						surface_bytes[i*bytes_per_pixel+j] = 0; // black is transparent
					};
				};
			};
			if (y < pixel_height-1) {
				surface_bytes += pitch;
				mask_surface_bytes += mask_pitch;
			};
		};
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 270901) {
		tt_error_file() << "Surface bytes after masking: " << endl;
		int i;
		for (i = 0; i < pixel_width; i++) {
			tt_error_file() << i << ": ";
			for (int j = 0; j < bytes_per_pixel; j++) {
				tt_error_file() << (int) original_surface_bytes[i*bytes_per_pixel+j] << " ";
			};
			tt_error_file() << "   ";
		};
		tt_error_file() << endl;
	};
#endif
};
*/ 

//#include <iomanip.h> - for debugging to print in hex

void copy_and_mask_work_page(work_page destination_bytes, int destination_pitch,
									  coordinate pixel_width, coordinate pixel_height,
									  work_page source_bytes, int pitch, 
									  int bytes_per_pixel, // of both source and destination
									  work_page mask_surface_bytes, int mask_pitch, int mask_bytes_per_pixel) {
	// new on 041001 
	if (bytes_per_pixel == 4) { 
		for (int y = 0; y < pixel_height; y++) {
			unsigned int *source = (unsigned int *) source_bytes; // changed to unsigned for clarity on 071201
			unsigned int *destination = (unsigned int *) destination_bytes;
			for (int i = 0; i < pixel_width; i++) {
//#if TT_DEBUG_ON
//				tt_error_file() << (int) mask_surface_bytes[i] << " "; 
//#endif
				if (mask_surface_bytes[i*mask_bytes_per_pixel] == 0) { 
					destination[i] = 0;
				} else if (source[i] == 0xFF000000) { // source[i]&0xFFFFFF == 0) { // mask away the alpha channel - updated on 071201
					// above is a better test but I'm confused why source[i]&0xFFFFFF == 0 caused trouble
					destination[i] = RGB(1,1,1); // nearly black but not transparent
//#if TT_DEBUG_ON
//					if (source[i]&0x00FFFFFF == 0) {
//						tt_error_file() << "debug this" << endl;
//					};
//#endif
				} else {
					destination[i] = source[i];
//#if TT_DEBUG_ON
//					tt_error_file() << setiosflags(std::ios_base::hex) << source[i] << setiosflags(std::ios_base::dec) << " " << i << "," << y << endl;
//#endif
				};
			};
			if (y < pixel_height-1) {
				source_bytes += pitch;
				mask_surface_bytes += mask_pitch;
				destination_bytes += destination_pitch;
			};
		};
	} else if (bytes_per_pixel == 3) { // rewritten on 021101
		for (int y = 0; y < pixel_height; y++) {
			for (int i = 0; i < pixel_width; i++) {
				if (mask_surface_bytes[i*mask_bytes_per_pixel] == 0) { // should really check all 3 bytes but cheaper to just require the mask to be pure black and white
					for (int j = 0; j < 3; j++) {
						destination_bytes[i*3+j] = 0; // black is transparent
					};
				} else if (source_bytes[i*3] == 0 && source_bytes[i*3+1] == 0 && source_bytes[i*3+2] == 0) {
					// black converted to near black 
					for (int j = 0; j < 3; j++) {
						destination_bytes[i*3+j] = 1; 
					};
				} else {
					for (int j = 0; j < 3; j++) {
						destination_bytes[i*3+j] = source_bytes[i*3+j]; 
					};
				};
			};
			if (y < pixel_height-1) {
				source_bytes += pitch;
				mask_surface_bytes += mask_pitch;
				destination_bytes += destination_pitch;
			};
		};
	} else if (bytes_per_pixel == 2) { // new on 071101
		for (int y = 0; y < pixel_height; y++) {
			unsigned short int *source = (unsigned short int *) source_bytes;
			unsigned short int *destination = (unsigned short int *) destination_bytes;
			for (int i = 0; i < pixel_width; i++) {
				if (mask_surface_bytes[i*mask_bytes_per_pixel] == 0) { 
					destination[i] = 0;
				} else if (source[i] == 0) { // should mask away the alpha channel if there is one, right?
					destination[i] = near_black16(); // nearly black but not transparent
				} else {
					destination[i] = source[i];
				};
			};
			if (y < pixel_height-1) {
				source_bytes += pitch;
				mask_surface_bytes += mask_pitch;
				destination_bytes += destination_pitch;
			};
		};
	} else { // must be 1 byte, right?
		for (int y = 0; y < pixel_height; y++) {
			for (int i = 0; i < pixel_width; i++) {
				if (mask_surface_bytes[i] == 0) { 
					destination_bytes[i] = 0; // black is transparent
				} else if (source_bytes[i] == 0) { // convert to near black (new on 071201)
					destination_bytes[i] = tt_nearest_black;
				} else {
					destination_bytes[i] = source_bytes[i]; 
				};
			};
			if (y < pixel_height-1) {
				source_bytes += pitch;
				mask_surface_bytes += mask_pitch;
				destination_bytes += destination_pitch;
			};
		};
	};
};

void permute_work_page(work_page source, work_page destination,
							  coordinate source_pitch, coordinate destination_pitch,
							  coordinate width, coordinate height, bytes permutation, int transparent_color) { // new on 190901
  // only called when running with 8-bit color
  // copies full screen work pages while permuting the color
	byte saved_entry;
	if (transparent_color >= 0) {
		saved_entry = permutation[transparent_color];
		permutation[transparent_color] = transparent_color; // identity so it remains transparent
	};
	for (int y = 0; y < height; y++) {
		for (int i = 0; i < width; i++) {
			destination[i] = permutation[source[i]];
		};
		if (y < height-1) {
			destination += destination_pitch;
			source += source_pitch;
		};
	};
	if (transparent_color >= 0) {
		permutation[transparent_color] = saved_entry; // restored
	};
};

void recolor_work_page(work_page source_bytes, work_page destination_bytes,
							  coordinate source_pitch_in_bytes, coordinate destination_pitch_in_bytes,
							  coordinate width, coordinate height, bytes permutation, int transparent_color) { // new on 021001
   // copies full screen work pages while altering the color
	// only called when more than 8-bit color
	int y;
	int *color_changes = (int *) permutation;
   // change now has 4 ints: change to all, change to red, change to green, and change to blue
	int change_all = color_changes[all_index]; 
   int red_change = color_changes[red_index]+change_all;
	int green_change = color_changes[green_index]+change_all;
	int blue_change = color_changes[blue_index]+change_all; 
 	switch (tt_bits_per_pixel) {
		case 32:
			{
			int *source = (int *) source_bytes;
			int *destination = (int *) destination_bytes;
			int source_pitch = source_pitch_in_bytes/tt_bytes_per_pixel;
			int destination_pitch = destination_pitch_in_bytes/tt_bytes_per_pixel;
 			for (y = 0; y < height; y++) {
				for (int i = 0; i < width; i++) {
					// could the following test suffer from the same problems that copy_and_mask_work_page did and fixed on 071201??
					if ((source[i]&0xFFFFFF) == transparent_color) { // mask out the alpha channel new on 311001 - was source[i]<<8 == 0) { // i.e. is black (shifted away the alpha channel)
						destination[i] = transparent_color; // was 0 prior to 021101
					} else {
//						destination[i] = alter_color32(source[i],color_changes);
                  // above rewritten on 130602 for performance (almost twice the old speed)
                  int red = (byte) (source[i]>>16);
	               int green = (byte) (source[i]>>8);
	               int blue = (byte) (source[i]);
                  if (red < 2 && green < 2 && blue < 2) { // treat black (and near black) specially
                     // this is very general -- doesn't do the right thing for mismatch feedback
		               if (change_all != 0) {
			               red = 64;
			               green = 64;
			               blue = 64;
		               } else {
			               if (red_change != 0) {
				               red = 64;
			               };
			               if (green_change != 0) {
				               green = 64;
			               };
			               if (blue_change != 0) {
				               blue = 64;
			               };
		               };
                     // can't be the value is too big or small so no need for min or max
                     destination[i] = Color::MakeARGB(0xFF,red,green,blue);
	               } else {
                     red += red_change;
                     green += green_change;
                     blue += blue_change;
                     // byte 0 is alpha - leave alone or would it be neat to use it?
                     // 1 rather than 0 since don't want transparent
                     destination[i] = Color::MakeARGB(0xFF,max(1,min(255,red)),max(1,min(255,green)),max(1,min(255,blue)));
                  };
               };
				};
				if (y < height-1) {
					destination += destination_pitch; 
					source += source_pitch;
				};
			};
			};
			break;
		case 24:
         { // open-coded alter_color24 on 130602
         int red_transparent = GetRValue(transparent_color);
         int green_transparent = GetGValue(transparent_color);
         int blue_transparent = GetBValue(transparent_color);
			for (y = 0; y < height; y++) {
				for (int i = 0; i < width*3; i += 3) {
					int red = source_bytes[i+2]; // this was i and i was i+2 prior to 250302
					int green = source_bytes[i+1];
					int blue = source_bytes[i];
//					COLORREF source_color = RGB(red,green,blue);
               //int red = GetRValue(color);
	            //int green = GetGValue(color);
	            //int blue = GetBValue(color);
//#if TT_DEBUG_ON
//               if (tt_debug_mode == 90503) {
//                  tt_error_file() << "Color in:  " << red << " " << green << " " << blue << endl;
//               };
//#endif
//					if ((int) source_color == transparent_color) { // prior to 311001 was 0 // i.e. is black 
               if (red == red_transparent && green == green_transparent && blue == blue_transparent) {
						destination_bytes[i] = 0;
						destination_bytes[i+1] = 0;
						destination_bytes[i+2] = 0;
					} else {
//						COLORREF new_color = alter_color24(source_color,color_changes);
//						destination_bytes[i] = GetBValue(new_color); // this was GetRValue and GetRValue was GetBValue prior to 250302
//						destination_bytes[i+1] = GetGValue(new_color);
//						destination_bytes[i+2] = GetRValue(new_color);
        //	      return(RGB(max(1,min(255,red)),max(1,min(255,green)),max(1,min(255,blue)))); // 1 rather than 0 since don't want transparent
                  if (red < 2 && green < 2 && blue < 2) { // treat black (and near black) specially
		               if (change_all != 0) {
			               destination_bytes[i+2] = 64;
			               destination_bytes[i+1] = 64;
			               destination_bytes[i] = 64;
		               } else {
			               if (color_changes[red_index] != 0) {
				               destination_bytes[i+2] = 64;
			               };
			               if (color_changes[green_index] != 0) {
				               destination_bytes[i+1] = 64;
			               };
			               if (color_changes[blue_index] != 0) {
				               destination_bytes[i] = 64;
			               };
		               };
	               } else {
                     destination_bytes[i+2] = max(1,min(255,red+red_change));
                     destination_bytes[i+1] = max(1,min(255,green+green_change));
                     destination_bytes[i] = max(1,min(255,blue+blue_change));
	               };
					};
				};
				if (y < height-1) {
					destination_bytes += destination_pitch_in_bytes; 
					source_bytes += source_pitch_in_bytes;
				};
			};
         };
			break;
		case 16: // new on 061101
			{
			short unsigned int *source = (short unsigned int *) source_bytes;
			short unsigned int *destination = (short unsigned int *) destination_bytes;
			int source_pitch = source_pitch_in_bytes/tt_bytes_per_pixel;
			int destination_pitch = destination_pitch_in_bytes/tt_bytes_per_pixel;
         // open coded alter_color16 on 130602
         int red_transparent = get_red_from_color16(transparent_color);
         int green_transparent = get_green_from_color16(transparent_color);
         int blue_transparent = get_blue_from_color16(transparent_color);
         if (tt_red_bits_per_pixel == 5 && tt_green_bits_per_pixel == 6 && tt_blue_bits_per_pixel == 5) {
            // special case a common pattern but this only improved things by 1% - worth it??
            for (y = 0; y < height; y++) {
				   for (int i = 0; i < width; i++) {
                  int red = get_red_from_color565(source[i]);
	               int green = get_green_from_color565(source[i]);
	               int blue = get_blue_from_color565(source[i]);
                  if (red == red_transparent && green == green_transparent && blue == blue_transparent) {
						   destination[i] = transparent_color; 
					   } else {
                     if (red < 2 && green < 2 && blue < 2) { // treat black (and near black) specially
		                  if (change_all != 0) {
			                  red = 64;
			                  green = 64;
			                  blue = 64;
		                  } else {
			                  if (red_change != 0) {
				                  red = 64;
			                  };
			                  if (green_change != 0) {
				                  green = 64;
			                  };
			                  if (blue_change != 0) {
				                  blue = 64;
			                  };
		                  };
                        destination[i] = make_color565(red,green,blue);
	                  } else {
	                     destination[i] = make_color565(max(8,min(255,red+red_change)),max(8,min(255,green+green_change)),max(8,min(255,blue+blue_change))); // 8 rather than 0 since don't want transparent (was 1 prior to 291101)
                     };
					   };
				   };
				   if (y < height-1) {
					   destination += destination_pitch; 
					   source += source_pitch;
				   };
			   };
         } else {
			   for (y = 0; y < height; y++) {
				   for (int i = 0; i < width; i++) {
                  int red = get_red_from_color16(source[i]);
	               int green = get_green_from_color16(source[i]);
	               int blue = get_blue_from_color16(source[i]);
   //					if ((source[i]) == transparent_color) { 
                  if (red == red_transparent && green == green_transparent && blue == blue_transparent) {
						   destination[i] = transparent_color; 
   //					} else if (tt_desired_bits_per_pixel == 15) {
   //						destination[i] = alter_color15(source[i],color_changes);
					   } else {
   //						destination[i] = alter_color16(source[i],color_changes);
                     // open coded below
                     if (red < 2 && green < 2 && blue < 2) { // treat black (and near black) specially
		                  if (change_all != 0) {
			                  red = 64;
			                  green = 64;
			                  blue = 64;
		                  } else {
			                  if (red_change != 0) {
				                  red = 64;
			                  };
			                  if (green_change != 0) {
				                  green = 64;
			                  };
			                  if (blue_change != 0) {
				                  blue = 64;
			                  };
		                  };
                        destination[i] = make_color16(red,green,blue);
	                  } else {
	                     destination[i] = make_color16(max(8,min(255,red+red_change)),max(8,min(255,green+green_change)),max(8,min(255,blue+blue_change))); // 8 rather than 0 since don't want transparent (was 1 prior to 291101)
                     };
					   };
				   };
				   if (y < height-1) {
					   destination += destination_pitch; 
					   source += source_pitch;
				   };
			   };
			};
         };
			break;
		case 8: // remodularized this on 251001
			permute_work_page(source_bytes,destination_bytes,
								   source_pitch_in_bytes,destination_pitch_in_bytes,
								   width,height,permutation,transparent_color);
			break;
		default:
			// shouldn't happen, right?
			copy_work_page(source_bytes,destination_bytes,source_pitch_in_bytes,destination_pitch_in_bytes,width,height);
	};
};

void copy_replacing_white_with_black(work_page source_bytes, work_page destination_bytes,
												 coordinate source_pitch_in_bytes, coordinate destination_pitch_in_bytes,
												 coordinate width, coordinate height) { // new on 121001
   // copies bytes while replacing white with black
	// only called when more than 8-bit color
	if (tt_bits_per_pixel == 32) {
		int *source = (int *) source_bytes;
		int *destination = (int *) destination_bytes;
		int source_pitch = source_pitch_in_bytes/tt_bytes_per_pixel;
		int destination_pitch = destination_pitch_in_bytes/tt_bytes_per_pixel;
		for (int y = 0; y < height; y++) {
			for (int i = 0; i < width; i++) {
				if (source[i] == max_unsigned_long) { // i.e. is black (shifted away the alpha channel)
					destination[i] = 0;
				} else {
					destination[i] = source[i];
				};
			};
			if (y < height-1) {
				destination += destination_pitch; 
				source += source_pitch;
			};
		};
	} else { // for now
		copy_work_page(source_bytes,destination_bytes,source_pitch_in_bytes,destination_pitch_in_bytes,width,height);
	};
};

#endif

void blit(bytes source, int original_source_width, int original_source_height) {
//			 coordinate tt_memory_graphics_mark_x, coordinate tt_memory_graphics_mark_y) {
//			 work_page destination,
//			 int destination_width, int destination_height,
//			 coordinate destination_llx, coordinate destination_lly) {
//	work_page destination = tt_destination_work_page;
#define blit_mode 0
#include "blitbody.cpp"
#undef blit_mode
};   

void blit_ignore(bytes source, int original_source_width, int width_with_padding, int original_source_height) { // width_with_padding new on 011101
	// ignore 0s
//#define blit_mode 1
//#include "blitbody.cpp"
//#undef blit_mode
	long left_offset,right_offset,top_offset,bottom_offset;
	// don't really use top_offset
	long source_width = original_source_width;
	long source_height = original_source_height;
//	long si,di; // for debuggin
	clip(source_width,source_height,
	//tt_memory_graphics_mark_x,tt_memory_graphics_mark_y,
//		  destination_width,destination_height,destination_llx,destination_lly,
		  left_offset,right_offset,top_offset,bottom_offset);
	if (source_width == 0 || source_height == 0) return;
	work_page destination = tt_destination_work_page;
	destination += offset(tt_destination_width,tt_memory_graphics_mark_x,tt_memory_graphics_mark_y);
//	di = offset(tt_destination_width,tt_memory_graphics_mark_x,tt_memory_graphics_mark_y);
//	int width_with_padding = original_source_width;
//	if (original_source_width&3) { // && !tt_using_directx_transform_or_gdiplus) { // restored on 311001 but only when neither GDI+ nor direct_transform is active
//		width_with_padding += 4-original_source_width%4; // adjust to long boundary
//	};	
//	source += offset(width_with_padding,tt_destination_llx,tt_destination_lly);
//	si = offset(width_with_padding,tt_destination_llx,tt_destination_lly);
	source += ((long) bottom_offset)*width_with_padding; // possibly clip off bottom of source
//	si += ((long) bottom_offset)*width_with_padding; // possibly clip off bottom of source
	source += left_offset; // possibly clip off left side of source
//	si += left_offset; // possibly clip off left side of source
#if TT_DEBUG_ON
	if (tt_debug_mode == 301001) {
		tt_error_file() << "original_source_width=" << original_source_width
						<< " ;width_with_padding=" << width_with_padding
						<< " ;bottom_offset=" << bottom_offset
						<< " ;original_source_height=" << original_source_height << endl;
	};
#endif
	if (tt_bytes_per_pixel == 4) { // new on 031001 
		int source_width_in_pixels = source_width/tt_bytes_per_pixel;
//		int destination_width_in_pixels = tt_destination_width/tt_bytes_per_pixel;
		for (int i = 0; ;) {
			int *source_in_words = (int *) source;
			int *destination_in_words = (int *) destination;
			for (int j = 0; j < source_width_in_pixels; j++) {
				if (source_in_words[j] != 0) {
					destination_in_words[j] = source_in_words[j];
				};
			};
			// odd control structure is so I don't trigger code guard by 
			// harmlessly going past the end of source and destination
			// but also this does a tiny bit less work
			i++;
			if (i == source_height) break;
			source += width_with_padding; // move up a row
			destination += tt_destination_width;
//			source_in_words += source_width_in_pixels; // move up a row
//			destination_in_words += destination_width_in_pixels;
		};
	} else if (tt_bytes_per_pixel == 3) { // new on 141201
		for (int i = 0; ;) {
			for (int j = 0; j < source_width; j += 3) {
				if (source[j] != 0 || source[j+1] != 0 || source[j+2] != 0) { // not all 3 bytes 0
					destination[j] = source[j];
					destination[j+1] = source[j+1];
					destination[j+2] = source[j+2];
				};
			};
			// odd control structure is so I don't trigger code guard by 
			// harmlessly going past the end of source and destination
			// but also this does a tiny bit less work
			i++;
			if (i == source_height) break;
			source += width_with_padding; // move up a row
	//		si += width_with_padding; // move up a row
			destination += tt_destination_width;
	//		di += tt_destination_width;
		};
	} else if (tt_bytes_per_pixel == 2) { // new on 141201 -- just short int version of 4 bytes per pixel
		int source_width_in_pixels = source_width/tt_bytes_per_pixel;
		for (int i = 0; ;) {
			short int *source_in_words = (short int *) source;
			short int *destination_in_words = (short int *) destination;
			for (int j = 0; j < source_width_in_pixels; j++) {
				if (source_in_words[j] != 0) {
					destination_in_words[j] = source_in_words[j];
				};
			};
			// odd control structure is so I don't trigger code guard by 
			// harmlessly going past the end of source and destination
			// but also this does a tiny bit less work
			i++;
			if (i == source_height) break;
			source += width_with_padding; // move up a row
			destination += tt_destination_width;
//			source_in_words += source_width_in_pixels; // move up a row
//			destination_in_words += destination_width_in_pixels;
		};
	} else {
		for (int i = 0; ;) {
			for (int j = 0; j < source_width; j++) {
				if (source[j] != 0) {
					destination[j] = source[j];
				};
			};
			// odd control structure is so I don't trigger code guard by 
			// harmlessly going past the end of source and destination
			// but also this does a tiny bit less work
			i++;
			if (i == source_height) break;
			source += width_with_padding; // move up a row
	//		si += width_with_padding; // move up a row
			destination += tt_destination_width;
	//		di += tt_destination_width;
		};
	};
};

/* noticed on 021001 that this was obsolete
void blit_switch(bytes source, int original_source_width, int original_source_height) {
//					  coordinate tt_memory_graphics_mark_x, coordinate tt_memory_graphics_mark_y) {
//					  work_page destination,
//					  int destination_width, int destination_height,
//					  coordinate destination_llx, coordinate destination_lly) {
	// move from destination to source
#define blit_mode 2
#include "blitbody.cpp"
#undef blit_mode
};   
*/
#if !WING&&!TT_DIRECTX
void fill_box(coordinate min_x, coordinate min_y,
				  coordinate max_x, coordinate max_y,
				  color the_color) {
  const integer width = max_x-min_x;
  const integer height = max_y-min_y;
  blit_color(the_color,
				 width,height,min_x,min_y,
				 tt_memory_graphics_work_page,
				 tt_screen_width,tt_screen_height);
};

void blit_color(color the_color, integer width, integer height);
//					 coordinate tt_memory_graphics_mark_x, coordinate tt_memory_graphics_mark_y) {
//					 work_page destination,
//					 int destination_width, int destination_height,
//					 coordinate destination_llx, coordinate destination_lly) {
	integer left_offset,right_offset,top_offset,bottom_offset;
	// don't really use top_offset
	long source_width = width;
	long source_height = height;
	clip(source_width,source_height,
	//tt_memory_graphics_mark_x,tt_memory_graphics_mark_y,
//		  destination_width,destination_height,destination_llx,destination_lly,
		  left_offset,right_offset,top_offset,bottom_offset);
	if (source_width <= 0 || source_height <= 0) return;
	work_page destination = tt_destination_work_page;
	long index = offset(tt_destination_width,tt_memory_graphics_mark_x,tt_memory_graphics_mark_y);
	destination += index;
	for (integer i = 0; i < source_height; i++) {
#if TT_32
		memset(destination,the_color,source_width);
#else
		work_page final_destination = destination+source_width;
		if (FP_SEG(destination) == FP_SEG(final_destination)) {
			memset(destination,the_color,source_width);
		} else {
			for (integer j = 0; j < source_width; j++) {
				destination[j] = the_color;
			};
		};
#endif
		destination += tt_destination_width;
	};
};
#endif

void draw_frame(color_index frame_color) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 51001) {
		tt_error_file() << "draw_frame called. tt_destination_work_page is " << (int) tt_destination_work_page << endl;
		tt_error_file() << "tt_screen_width is " << tt_screen_width << "; tt_destination_width is " << tt_destination_width << "; tt_bytes_per_pixel is " << (int) tt_bytes_per_pixel << endl;
		tt_error_file() << "tt_screen_height is " << tt_screen_height << "; tt_destination_height is " << tt_destination_height << endl;
//		if (tt_bits_per_pixel == 32 && tt_exclusive_screen) return; // for now
	};
#endif
	// updated 250901 to work for different color depths
	work_page page = tt_destination_work_page; //tt_memory_graphics_work_page;
	// called just before blitting to screen so don't bother if "clean"
   if (tt_dirty_min_y == 0) {
		memset(page,frame_color,tt_screen_width*tt_bytes_per_pixel);
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 51001) {
		tt_error_file() << "draw_frame top done." << endl; // or is it bottom?
	};
#endif
   page += tt_destination_width*tt_bytes_per_pixel;
	if (tt_dirty_min_x == 0 || tt_dirty_max_x == tt_screen_width-1) {
		for (long y = 0; y < tt_screen_height-2; y++) {
			for (int i = 0; i < tt_bytes_per_pixel; i++) {
				page[i] = frame_color; // left border
				page[tt_screen_width*tt_bytes_per_pixel-(i+1)] = frame_color; // right border
			};
         page += tt_destination_width*tt_bytes_per_pixel;
		};
	} else {
      page += tt_destination_width*(tt_screen_height-2)*tt_bytes_per_pixel;
   };
#if TT_DEBUG_ON
	if (tt_debug_mode == 51001) {
		tt_error_file() << "draw_frame sides done." << endl; 
	};
#endif
   // not clear what was wrong with this test but the frame came and went
//	if (tt_dirty_max_y == tt_screen_height-1) {
		memset(page,frame_color,tt_screen_width*tt_bytes_per_pixel);
//	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 51001) {
		tt_error_file() << "draw_frame done." << endl; 
	};
#endif
};

/*
void darken_with_dark_colors() { // obsolete beginning 200901
#if TT_DEBUG_ON
   millisecond start_time = timeGetTime();
#endif
//if (tt_debug_mode == 1110) return; // for testing
//   unsigned long next_random = 1;
   work_page page = tt_destination_work_page; //tt_memory_graphics_work_page;
   long index = 0;
   long line_padding = tt_destination_width-tt_screen_width;
   for (long y = 0; y < tt_screen_height; y++) {
//		for (long x = y&1; x < tt_screen_width; x += 2) {
//      for (long x = y%every; x < tt_screen_width; x += every) {
//      for (long x = 0; x < tt_screen_width; x += next_random%every) {
//			page[y*tt_destination_width+x] = frame_color;
//         next_random = next_random*1103515245+12345;
         for (long x = 0; x < tt_screen_width; x++) {
//            long index = y*tt_destination_width+x;
            COLORREF darker_replacement = tt_dark_colors[page[index]];
            // 255 indicates identity permutation
            if (darker_replacement != 255) page[index] = (byte) darker_replacement;
            index++;
		   };
         index += line_padding;
	};
#if TT_DEBUG_ON
   if (tt_debug_mode == 3) {
      tt_error_file() << (timeGetTime()-start_time) << "ms to darken screen." << endl;
   };
#endif
};
*/

void darken_with_color(color_index frame_color) {
#if TT_DEBUG_ON
   millisecond start_time = timeGetTime();
#endif
//if (tt_debug_mode == 1110) return; // for testing
//   unsigned long next_random = 1;
	work_page page = tt_destination_work_page; //tt_memory_graphics_work_page;
	// called just before blitting to screen so don't bother if "clean"
   const int darken_every = 4;
	for (long y = tt_dirty_min_y; y < tt_dirty_max_y; y++) {
      int offset;
      switch (y%darken_every) {
         case 0:
            offset = 0;
            break;
         case 1:
            offset = 2;
            break;
         case 2:
            offset = 3;
            break;
         case 3:
            offset = 1;
            break;
         default:
            offset = y%darken_every;
      };
		for (long x = tt_dirty_min_x-(tt_dirty_min_x&(darken_every-1))+offset; x < tt_dirty_max_x; x += darken_every) {
			page[y*tt_destination_width+x] = frame_color;
		};
	};
#if TT_DEBUG_ON
   if (tt_debug_mode == 3) {
      tt_error_file() << (timeGetTime()-start_time) << "ms to darken screen." << endl;
   };
#endif
};

void clear_page(work_page page, color_index color) {
   // not updated for DIRECTX could use blt fill but isn't needed
#if TT_32
	memset(page,color,tt_screen_bytes_size);
#else
	long page_remaining = tt_screen_bytes_size;
	while (page_remaining > 0) {
		integer pointer_offset = FP_OFF(page);
		long remaining_in_segment = max_unsigned_short-pointer_offset;
		if (remaining_in_segment >= page_remaining) {
			remaining_in_segment = page_remaining-1;
		};
		memset(page,color,remaining_in_segment);
		page_remaining -= remaining_in_segment+1;
		page += remaining_in_segment;
		page[0] = color; // fill in the last one
		page++;
	};
#endif
};

void blit_scale_ignore(bytes source,
							  uinteger pixel_width,
							  uinteger pixel_height,
							  uinteger x_scale, uinteger y_scale) {
//							  coordinate llx, coordinate lly) {
//							  work_page destination,
//							  int destination_width, int destination_height) {
  // scales a bit map ignoring 0s
#define compute_dimensions_and_clip
#include "blitbody.cpp"
#undef compute_dimensions_and_clip
#define blit_scale_mode 1
#include "blitbody.cpp"
#undef blit_scale_mode
};

void blit_scale(bytes source,
					 uinteger pixel_width,
					 uinteger pixel_height,
					 uinteger x_scale, uinteger y_scale) {
//					 coordinate llx, coordinate lly) {
//					 work_page destination,
//					 int destination_width, int destination_height) {
  // scales a bit map 
#define compute_dimensions_and_clip
#include "blitbody.cpp"
#undef compute_dimensions_and_clip
#define blit_scale_mode 0
#include "blitbody.cpp"
#undef blit_scale_mode
};

void blit_pixelrun_ignore(bytes pixel_runs, const uinteger size,
								  uinteger width,
								  uinteger height) {
//								  coordinate llx, coordinate lly) {
//								  work_page destination,
//								  int destination_width, int destination_height,
//								  coordinate destination_llx, coordinate destination_lly) {
	integer left_offset,right_offset,top_offset,bottom_offset;
	long source_width = width;
	long source_height = height;
	// following fixes a clipping bug -- couldn't figure out a better way
	// somehow Pumpy needs to subtract -2 and -3 respectively below...
	if (tt_destination_lly > 0) {
		tt_destination_lly--;
		tt_memory_graphics_mark_y--;
	};
	clip(source_width,source_height,//llx,lly,
//		  destination_width,destination_height,destination_llx,destination_lly,
		  left_offset,right_offset,top_offset,bottom_offset);
	if (source_width == 0 || source_height == 0) return;
	work_page destination = tt_destination_work_page;
	destination += offset(tt_destination_width,
								 tt_memory_graphics_mark_x,tt_memory_graphics_mark_y);
	unsigned char color,run;
//	integer initial_image_index = -left_offset;
	long image_index = 0; //initial_image_index;
	integer new_run;
	integer left_margin_remaining = left_offset;
//	integer right_margin = tt_destination_width-tt_memory_graphics_mark_x-1;
	integer right_margin = source_width-1; // right margin in relative coordinates
	integer pixels_to_skip;
	uinteger i = 0;
	integer lines_left = source_height;
	integer lines_to_skip = bottom_offset; // skip bottom lines;
#define pixelrun_skip_lines
#include "blitbody.cpp"
#undef pixelrun_skip_lines				  
	for (; ; i += 2) { // was i < size but now returns when run=0 & color=1
//	  int j;
	  run = pixel_runs[i];
	  color = pixel_runs[i+1];
	  if (run == 0) {
		  if (color == 0) { 
			  // finish off this line
			  lines_left--;
			  if (lines_left == 0) {
				  return;
			  };
			  destination += tt_destination_width;
			  image_index = 0; //initial_image_index;
			  // and now prepare for next line
			  left_margin_remaining = left_offset;
		  } else { // color is the number of pixels to copy over
			  if (color == 1) { // end of file
				  return;
			  };
			  i += 2;
/* following speeds this up by 1/3 but treats embedded 0s wrong
			  memcpy(destination+image_index,pixel_runs+i,color);
			  image_index += color;
			  i += color;
*/
			  new_run = color;
			  if (left_margin_remaining > 0) {
				  if (new_run > left_margin_remaining) {
					  i += left_margin_remaining;
					  new_run -= left_margin_remaining;
					  left_margin_remaining = 0;
				  } else {
					  left_margin_remaining -= new_run;
					  i += new_run;
					  new_run = 0;
				  };
			  }; 
			  if (image_index+new_run > right_margin) {
				  // clip on the right
				  pixels_to_skip = image_index+new_run-right_margin;
				  new_run -= pixels_to_skip;
				  // can the following happen without scaling??
				  if (new_run < 0) {
					  pixels_to_skip += new_run; // don't skip more than original
					  new_run = 0;
					  image_index = right_margin+1;
				  };
			  } else {
				  pixels_to_skip = 0;
			  };
			  uinteger stop = i+new_run; // was long
			  for (;i < stop; i++) {
				  unsigned char next_color = pixel_runs[i];
				  if (next_color != 0) {
					  destination[image_index] = next_color;
				  };
				  image_index++;
			  };
			  i += pixels_to_skip-2;
			  if (color&1) { // if odd since byte aligned skip over the next
				  i++;
			  };
		  };
	  } else {
		  if (left_margin_remaining > 0) {
			  if (run > left_margin_remaining) {
				  run -= (unsigned char) left_margin_remaining;
				  left_margin_remaining = 0;
			  } else {
				  left_margin_remaining -= run;
				  run = 0;
				  color = 0; // to skip the memset code
			  };
		  }; 
		  if (image_index+run > right_margin) {
			  // clip on the right
			  integer excess = image_index+run-right_margin;
			  if (excess > run) {
				  run = 0;
				  image_index = right_margin+1;
			  } else {
				  run -= (unsigned char) excess;
			  };
		  };
		  if (color != 0) { 
			  work_page real_destination = destination+image_index;
#if TT_32
			  memset(real_destination,color,run);
#else
			  work_page final_destination = real_destination+run;
			  if (FP_SEG(real_destination) == FP_SEG(final_destination)) {
				  memset(real_destination,color,run);
			  } else {
				  for (integer j = 0; j < run; j++) {
					  real_destination[j] = color;
				  };
			  };
#endif
		  };
		  image_index += run;
	  };
	};
};

// haven't bothered to make a blit_pixelrun_scale
void blit_pixelrun_scale_and_ignore(bytes pixel_runs, const uinteger size,
												uinteger pixel_width,
												uinteger pixel_height,
												uinteger x_scale, uinteger y_scale) {
//												coordinate llx, coordinate lly) {
//												work_page destination,
//												int destination_width, int destination_height) {
	// following fixes a clipping bug -- couldn't figure out a better way
	// somehow Pumpy needs to subtract -2 and -3 respectively below...
	if (tt_destination_lly > 0) {
		tt_destination_lly--;
		tt_memory_graphics_mark_y--;
	};
#define compute_dimensions_and_clip
#include "blitbody.cpp"
#undef compute_dimensions_and_clip
	integer lines_left = height;
	integer right_margin;
	integer initial_image_index = -pixel_left_offset;//0;
//	if (left_offset > 0) {
		// will skip left stuff explictly
//		destination += offset(tt_destination_width,0,tt_memory_graphics_mark_y);
//		destination_offset += offset(destination_width,0,lly);
//		initial_image_index = -pixel_left_offset;
//		right_margin = tt_destination_width;
//	} else {
		destination += offset(tt_destination_width,tt_memory_graphics_mark_x,tt_memory_graphics_mark_y);
//		destination_offset += offset(destination_width,llx,lly);
//		right_margin 
//			= tt_destination_width-(tt_memory_graphics_mark_x+pixel_right_offset);  // -1?
		right_margin = width-1; // right margin in relative coordinates
//	};
	unsigned char color,run;
	long image_index = initial_image_index;
	integer x_left_over = 0;
	unsigned long run_times_scale;
//	uinteger new_run; // caused some inexplicable (Borland bug??) bug in 32bits
	long new_run;
	integer y_left_over = y_scale;
	integer vertical_copies = y_left_over>>8;
	if (vertical_copies > lines_left) {
		vertical_copies = lines_left;
	} else {
		y_left_over = y_left_over&255;
	};
	uinteger i = 0;
	integer bottom_offset = (pixel_bottom_offset*256L)/y_scale;
	integer lines_to_skip = bottom_offset; // skip bottom lines 
#define pixelrun_skip_lines
#include "blitbody.cpp"
#undef pixelrun_skip_lines				  
	for (; i < size; i += 2) {
//	  integer j;
	  run = pixel_runs[i];
	  color = pixel_runs[i+1];
	  if (run == 0) {
		  if (color == 0) { // finish off this line
			  lines_left -= vertical_copies;
			  if (lines_left == 0) {
				  return;
			  };
			  destination += tt_destination_width*vertical_copies;
//			  destination_offset += destination_width*vertical_copies;
//			  if (destination_offset >= destination_size) {
//				  tt_error_file() << "blit bug" << endl;
//			  };
			  image_index = initial_image_index;
			  x_left_over = 0; // don't accumulate extra stuff
			  // and now prepare for next line
			  y_left_over += y_scale;
			  vertical_copies = y_left_over>>8;   // shift?
			  if (vertical_copies > lines_left) {
				  vertical_copies = lines_left;
			  } else {
				  y_left_over = y_left_over&255;   // mask? (cast to unsigned char?)
			  };
			  if (vertical_copies == 0) {
				  integer lines_to_skip = (255-y_left_over)/y_scale;
				  y_left_over += lines_to_skip*y_scale;
#define pixelrun_skip_lines
#include "blitbody.cpp"
#undef pixelrun_skip_lines
           };			
		  } else { // color is the number of pixels to copy over
			  i += 2;
			  uinteger stop = i+color; // was long 
			  for (;i < stop; i++) {
				  x_left_over += x_scale;
				  if (x_left_over > 255) {
					  unsigned char next_color = pixel_runs[i];
					  if (next_color != 0) {
						  while (x_left_over > 255) {
							  if (image_index >= 0 && image_index <= right_margin) { // was just <
								  long copy_index = image_index;
								  for (integer c = 0; c < vertical_copies; c++) {
									  destination[copy_index] = next_color;
									  copy_index += tt_destination_width;
								  };
							  };
							  image_index++;
							  x_left_over -= 256;
						  };
					  } else {
						  image_index += x_left_over>>8;
						  x_left_over = x_left_over&255;
					  };
				  };
			  };
			  i -= 2;
			  if (color&1) { // if odd since byte aligned skip over the next
				  i++;
			  };
		  };
	  } else {
		  run_times_scale = ((unsigned long) run)*x_scale;
//		  new_run = run_times_scale/256;
		  new_run = run_times_scale>>8;
//		  x_left_over += run_times_scale&0xFF; // just the last byte
		  x_left_over += run_times_scale&255;
		  if (x_left_over > 255) {
			  new_run++;
			  x_left_over -= 256;
		  };
		  if (color != 0) { // for now ignore version
			  boolean setmem = TRUE;
			  if (image_index < 0) {
				  integer gap_to_margin = -image_index;
				  if (gap_to_margin < new_run) {
					  new_run -= gap_to_margin;
					  image_index = 0;
				  } else {
					  image_index += new_run;
					  new_run = 0;
					  setmem = FALSE;
//					  vertical_copies = 0; // to avoid doing the memset
				  };
			  };
			  if (image_index+new_run > right_margin) {
				  if (image_index >= right_margin) { // already at margin
					  new_run = 0;
					  setmem = FALSE;
//					  vertical_copies = 0;
				  } else {
					  new_run = right_margin-image_index;
				  };
			  };
			  if (setmem) {
				  for (integer c = 0; c < vertical_copies; c++) {
					  work_page real_destination 
						  = destination+image_index+c*tt_destination_width;
//						if (destination_offset+image_index+c*destination_width >= destination_size) {
//				  tt_error_file() << "blit bug" << endl;
//			  };
#if TT_32
					  memset(real_destination,color,new_run);
#else
					  work_page final_destination = real_destination+new_run;
					  if (FP_SEG(real_destination) == FP_SEG(final_destination)) {
						  memset(real_destination,color,new_run);
					  } else {
						  for (integer j = 0; j < new_run; j++) {
							  real_destination[j] = color;
						  };
					  };
#endif
				  };
			  };
		  };
		  image_index += new_run;
	  };
	};
};

#if !WING
void permute_colors(work_page image, long size, bytes permutation) {
	// can be for a background > 64K
	for (long i = 0; i < size; i++) {
		unsigned char color = image[i];
		image[i] = permutation[color];
		// maintain 0 for clear
		if (color != 0 && image[i] == 0) { // wasn't clear but is now
			image[i] = tt_black_index;
		};
	};
};

void permute_pixelrun(bytes pixel_runs, const uinteger size,
							 bytes permutation) {
	unsigned char color, run;
	for (uinteger i = 0; i < size; i += 2) { // why not long??
	  run = pixel_runs[i];
	  color = pixel_runs[i+1];
	  if (run == 0) {
		  if (color != 0) {
			  // color is the number of pixels to copy over
			  if (color == 1) { // end of file
				  return;
			  };
			  i += 2;
			  uinteger stop = i+color;
			  for (;i < stop; i++) {
				  unsigned char color = pixel_runs[i];
				  pixel_runs[i] = permutation[color];
				  if (color != 0 && pixel_runs[i] == 0) { 
					  // wasn't clear but is now
					  pixel_runs[i] = tt_black_index;
				  };
			  };
			  i += -2;
			  if (color&1) { // if odd since byte aligned skip over the next
				  i++;
			  };
		  };
	  } else {
		  pixel_runs[i+1] = permutation[color];
		  if (color != 0 && pixel_runs[i+1] == 0) { 
			  // wasn't clear but is now
			  pixel_runs[i+1] = tt_black_index;
		  };
	  };
	};
};
#endif
