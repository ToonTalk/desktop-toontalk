// Copyright (c) 1992,1996.  Ken Kahn, Animated Programs, All rights reserved

#ifdef compute_dimensions_and_clip
	// don't really need to compute these here -- could pass them from
	// Image::display
	long width = (((long) pixel_width)*x_scale)>>8;
	long height = (((long) pixel_height)*y_scale)>>8;
	integer pixel_left_offset,pixel_right_offset,pixel_top_offset,pixel_bottom_offset;
	clip(width,height,//llx,lly,
//		  destination_width,destination_height,0,0,
		  pixel_left_offset,pixel_right_offset,pixel_top_offset,pixel_bottom_offset);
	if (width == 0 || height == 0) return;
	work_page destination = tt_destination_work_page;
//	integer left_offset = (pixel_left_offset*256L)/x_scale;
//	if ((left_offset&255) != 0) left_offset++;   // why?
//	integer right_offset = (pixel_right_offset*256L)/x_scale;
//	if ((right_offset&255) != 0) right_offset++;
//	integer top_offset = (pixel_top_offset*256L)/y_scale;
//	if ((top_offset&255) != 0) top_offset++;
//	integer bottom_offset = (pixel_bottom_offset*256L)/y_scale;
//	if (bottom_offset&255 != 0) bottom_offset++;

#endif
#ifdef blit_mode

	integer left_offset,right_offset,top_offset,bottom_offset;
	// don't really use top_offset
	long source_width = original_source_width;
	long source_height = original_source_height;
	clip(source_width,source_height,
//tt_memory_graphics_mark_x,tt_memory_graphics_mark_y,
//		  destination_width,destination_height,destination_llx,destination_lly,
		  left_offset,right_offset,top_offset,bottom_offset);
	if (source_width == 0 || source_height == 0) return;
	work_page destination = tt_destination_work_page;
	destination += offset(tt_destination_width,tt_memory_graphics_mark_x,tt_memory_graphics_mark_y);
	integer width_with_padding = original_source_width;
	if (original_source_width&3) { // low order bits set
		width_with_padding += 4-original_source_width&3; // adjust to long boundary
	};
//	source += offset(width_with_padding,tt_destination_llx,tt_destination_lly);
	source += ((long) bottom_offset)*width_with_padding; // possibly clip off bottom of source
	source += left_offset; // possibly clip off left side of source
	for (integer i = 0; ;) {
#if blit_mode == 0 // straight blit
	  memcpy(destination,source,source_width);
#elif blit_mode == 2 // switch blit
	  memcpy(source,destination,source_width);
#elif blit_mode == 1 // color 0 is transparent
		for (integer j = 0; j < source_width; j++) {
			if (source[j] != 0) {
				destination[j] = source[j];
			};
		};
#endif
      i++;
      if (i == source_height) break;
		source += width_with_padding; // move up a row
		destination += tt_destination_width;
	};

#endif

#ifdef blit_scale_mode
	integer pixel_width_padded = pixel_width;
	if (pixel_width&3) { // low order bits set
		pixel_width_padded += 4-pixel_width&3; // adjust to long boundary
	};
	integer left_offset = (pixel_left_offset*256L)/x_scale;
	// following seems to prevent overwriting destination
//	if ((left_offset&255) != 0) left_offset++; 
	integer right_offset = (pixel_right_offset*256L)/x_scale;
   int round_off_error 
      = ((pixel_left_offset*256L)%x_scale+(pixel_right_offset*256L)%x_scale+(x_scale-1))/x_scale;
   if (round_off_error < 2) {
      left_offset += round_off_error;
   } else { // share it
     right_offset += round_off_error-1;
     left_offset++;
   };
//	if ((right_offset&255) != 0) right_offset++; 
	pixel_width -= left_offset+right_offset;
//	pixel_height -= bottom_offset+top_offset;
	destination += offset(tt_destination_width,tt_memory_graphics_mark_x,tt_memory_graphics_mark_y);
	integer bottom_offset = (pixel_bottom_offset*256L)/y_scale; // what about roundoff
	source += ((long) bottom_offset)*pixel_width_padded; // possibly clip off bottom of source
	source += left_offset; // possibly clip off left side of source
	long image_index;
	integer lines_left;
	if (y_scale <= 256) {
		lines_left = height;
	} else { // subtract one otherwise sometimes writes beyond destination
      // but often leaves a black line at the top so removing this
		lines_left = height; //-1;  // do lines 0 thru height-1
	};
	integer x_left_over = 0, y_left_over = 0;
	integer vertical_copies;
	integer lines_to_skip;
	for (;;) { // line count is who stops this
		image_index = 0; //left_offset;
		y_left_over += y_scale;
		// consider shifting
		vertical_copies = y_left_over>>8;
		if (vertical_copies > lines_left) {
			vertical_copies = lines_left;
		} else {
			y_left_over = y_left_over&255;
		};
		if (vertical_copies > 0) {
#if blit_scale_mode == 0 // straight blit
//         long starting_image_index = image_index;
         unsigned char next_color;
			for (integer x = 0; x < (integer) pixel_width; x++) {
				x_left_over += x_scale;
				if (x_left_over > 255) {
					next_color = source[x];
					while (x_left_over > 255) {
						destination[image_index] = next_color;
						image_index++;
						x_left_over -= 256;
					};
				};
			};
//         long remaining_pixels = width-(image_index-starting_image_index);
//         while (remaining_pixels > 0) {
//            destination[image_index++] = next_color;
//            remaining_pixels--;
//         };
			// did one line copy the rest
			long last_line_offset = 0;
			long next_line_offset = tt_destination_width;
//         long line_width = image_index-starting_image_index;
			for (integer c = 1; c < vertical_copies; c++) {
				memcpy(destination+next_line_offset,
						  destination+last_line_offset,
						  width); 
				last_line_offset = next_line_offset;
				next_line_offset += tt_destination_width;
			};
#elif blit_scale_mode == 1 // ignore 0s blit
	      long copies_index;
			for (integer x = 0; x < (integer) pixel_width; x++) {
				x_left_over += x_scale;
				if (x_left_over > 255) {
					unsigned char next_color = source[x];
					while (x_left_over > 255) {
						if (next_color != 0) {
							copies_index = image_index;
							for (integer c = 0; c < vertical_copies; c++) {
								destination[copies_index] = next_color;
								copies_index += tt_destination_width;
							};
						};
						image_index++;
						x_left_over -= 256;
					};
				};
			};
#endif
		};
		lines_left -= vertical_copies;
		if (lines_left == 0) return;
		lines_to_skip = (255-y_left_over)/y_scale;
		if (lines_to_skip > 0) {
			source += pixel_width_padded*lines_to_skip;
			y_left_over += lines_to_skip*y_scale;
		};
		destination += tt_destination_width*vertical_copies;
		source += pixel_width_padded;
		x_left_over = 0; // don't accumulate extra stuff
	};
#endif

#ifdef pixelrun_skip_lines
				  for (; lines_to_skip > 0;) {
					  i += 2;
					  if (i >= size) return;
					  run = pixel_runs[i];
					  color = pixel_runs[i+1];
					  if (run == 0) {
						  if (color == 0) { // new line
							  lines_to_skip--;
						  } else if (color == 1) {
							  i = size; // done
						  } else { // skip non run pixels
							  i += color;
							  if (color&1) i++; // skip padding as well
						  };
					  };
					};
#endif



