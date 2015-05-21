// old (prior to TT_SHRINKING_AND_GROWING) version kept
// so WebLabs members have a choice of this enhancement or not

// renamed if compiling with new number display code since it can call this if !tt_shrink_and_grow_numbers -- 141104
#if TT_SHRINKING_AND_GROWING
void Number::display_old(SelectionFeedback selection_feedback, boolean followers_too, TTRegion *region) {
#else
void Number::display(SelectionFeedback selection_feedback, boolean followers_too, TTRegion *region) {
#endif
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Displaying debug target."));
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
	if (text_length < 0 && visible() && !is_blank()) { // new on 050803 so that this recomputes the text (and possibly the size) before all the rest
		// changed show_all to visible on 150803
		current_wide_text(wide_text,wide_text_length);
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
		tt_error_file() << " at " << llx << "," << lly << " with size " << width << "x" << height << " on frame " << tt_frame_number << endl;
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
		if (region == NULL && 
			// added this condition on 070202 since if flying over a city the following test doesn't work 
			// (and if part of something should already have checked if off screen??)
			 tt_screen->off_screen(adjusted_llx,adjusted_lly,adjusted_width,adjusted_height) &&
          !change_size_to_fit_extent_flag) return; // new on 160102 -- moved earlier on 220102 -- added !change_size_to_fit_extent_flag on 180302
#if TT_DEBUG_ON
		if (tt_debug_mode == 220102) {
			tt_error_file() << "Printed in " << adjusted_width << "x" << adjusted_height << " region with font size of " 
								 << character_width << "x" << character_height << endl;
		};
#endif
		city_coordinate adjusted_text_width = adjusted_width;
		city_coordinate adjusted_text_height = adjusted_height;
		city_coordinate edge_size = compute_edge_size(adjusted_width,adjusted_height);
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
			if (change_size_to_fit_extent_flag || insertion_point >= 0) { // copied here from text::display_text on 110102
				// && !change_font_size_to_fit_extent_flag) { 
				// new policy on 050202 so if change_font_size_to_fit_extent_flag then do it first 
				// (so character_width etc are up-to-date for changing size)
				// undid this on 140202 since can cause numbers to be ... before being changed (e.g. float on a blank rational)
				if (leader != NULL || animation_in_progress() || region != NULL) { 
               // relationship have changed or things are changing (added region != NULL test on 081102 since shouldn't change size if in a region
					if (leader != NULL) set_change_size_to_fit_extent(FALSE); 
					// conditional added on 050202 so that size change will happen when animation is over
					set_change_font_size_to_fit_extent(TRUE);
				} else if (character_width > 0 && character_height > 0) { 
					// conditional is to avoid work when dealing with numbers too long to see the digits
					int ignore_longest_line;
					long ignore_new_text_height; // new on 270202 so that height isn't changed by this
#if TT_DEBUG_ON
					if (tt_debug_mode == 110302) {
						tt_error_file() << "Before get_extent_size " << non_shrinking_equivalent(text_length) << " length with character width " 
										<< character_width << "X" << character_height
										<< " and adjusted_text_width is " << adjusted_text_width << endl;
					};
#endif
					tt_screen->get_extent_size(wide_text,non_shrinking_equivalent(text_length),1, 
														// decimal_places only non-zero if displaying shrinking decimal
														character_width,character_height,
														ignore_longest_line,adjusted_text_width,ignore_new_text_height,
														TRUE,TRUE,FALSE);
#if TT_DEBUG_ON
					if (tt_debug_mode == 110302) {
						tt_error_file() << "With character_width of " << character_width << " and character_height of " << character_height 
							 				 << " and " << text_length 
											 << " characters computed a new width of " << adjusted_text_width << " and height of " << ignore_new_text_height 
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
			if (change_font_size_to_fit_extent_flag || region != NULL || decimal_places > 0 || animation_in_progress()) { 
				// region check added on 210102 since with remote looks and the like same thing can be displayed with different dimensions
				// added decimal_places > 0 || animation_in_progress() on 170803 since otherwise doesn't use a good font size
				tt_screen->correct_font_size(wide_text,non_shrinking_equivalent(text_length),1,
													  adjusted_text_width,adjusted_text_height,TRUE,TRUE,FALSE,
													  adjusted_character_width,adjusted_character_height);
//				adjusted_character_height = adjusted_text_height; // don't care what the vertical extent is - leave the height alone - new on 270202
//tt_error_file() << "With width of " << adjusted_text_width << " and height of " << adjusted_text_height << " computed character_width of " << adjusted_character_width << " and character_height of " << adjusted_character_height << endl;
#if TT_DEBUG_ON
            if (debug_counter == tt_debug_target) {
               log("changing font size to fit extent of debug target.");
            };
				if (tt_debug_mode == 110302) {
					tt_error_file() << "After correct_font_size adjusted character width and height " 
									<< adjusted_character_width << "X" << adjusted_character_height
									<< " and adjusted_text_width is " << adjusted_text_width <<  " on frame " << tt_frame_number << endl;
				};
#endif
				if (region == NULL) { // experiment on 101202 ) { // removed && region == NULL on 281002
					// first test was change_font_size_to_fit_extent_flag prior to 081102 but don't remember new font size if am say displaying the remote looks
               // animation check added on 250102 and region check since could be "seen" through remote looks and then shouldn't save font sizes
					// no point caching it if displayed within a region
					set_character_width(max(1,adjusted_character_width)); // for very long bignums width can become zero - needs a minimum
					set_character_height(max(1,adjusted_character_height));
					if (!animation_in_progress()) { // conditions 
						set_change_font_size_to_fit_extent(FALSE);
					};
				};
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
				//if (leader == NULL && !held() &&  // experiment 121202 -- removing this helps keep numbers that are changing number of digits on the floor stationary
				//	 llx >= tt_screen->min_x() && llx+adjusted_text_width <= tt_screen->max_x() && // new on 260202 - package this up as fully_on_screen
				//	 lly >= tt_screen->min_y() && llx+adjusted_text_height <= tt_screen->max_y()) {
				//	move_by((old_adjusted_text_width-adjusted_text_width)/2,0); // old_adjusted_text_width was width prior to 250202 -- for very long numbers matters
				//};
				Sprite::set_size(adjusted_text_width,height,NEW_TEXT_EXTENT); // don't want to trigger changes by text::set_size...
				set_clean_status(DIRTY); // new on 310104 since can be DIRTY_BUT_OBSCURED and grow (but can't this be handled elsewhere more generally?)
				// not clear why the following works better than set_size
//				width = adjusted_text_width;
				tt_screen->invalidate_cache(); // new on 170102 since dirty region is confused by this size change while displaying
				set_change_font_size_to_fit_extent(TRUE); // moved here on 270202 so it happens next time around
			};
			city_coordinate start_x = adjusted_llx+edge_size;
#if TT_ALPHA_FEATURE
			if (tt_right_to_left_text) { // new on 081203 for Hebrew
				tt_main_window->set_text_align(TA_LEFT); // restore the default for displaying numbers - in particular important for shrinking decimals and things like /7 (which otherwise are displayed as 7/)
//				start_x += adjusted_text_width;
			};
			UnwindProtect <boolean> set(tt_right_to_left_text,FALSE); // so numbers aren't affected
#endif
#if TT_DEBUG_ON
			add_to_log_if_interesting_error();
#endif
			if (adjusted_character_width >= 3*one_x_pixel) { // conditional new on 060202 -- made it 3 pixels on 130202 -- changed to >= on 110302
				if (decimal_places > 0) {
#if TT_ALPHA
					city_coordinate height_or_region_height;
					if (region == NULL) {
						height_or_region_height = height;
					} else {
						height_or_region_height = region->height();
					};
#endif
#if TT_DEBUG_ON
					DWORD start_time;
					if (tt_debug_mode == 100804) {
						start_time = timeGetTime();
					};
#endif
					display_shrinking_decimal(wide_text,wide_text_length,start_x,adjusted_lly+adjusted_height-edge_size, // adjusted_text_height+edge_size, 
													  adjusted_character_width,adjusted_character_height,permutation,adjusted_width-2*edge_size
#if TT_ALPHA													  
													 ,height_or_region_height
#endif
													  );
#if TT_DEBUG_ON
					if (tt_debug_mode == 100804) {
						tt_error_file() << "Displaying shrinking digits of number#" << debug_counter << " took " << (timeGetTime()-start_time) << "ms" << endl;
					};
#endif
				} else {
					tt_screen->text_out((string) wide_text,wide_text_length,start_x,adjusted_lly+adjusted_height-edge_size, // adjusted_text_height+edge_size, 
											  adjusted_character_width,adjusted_character_height,
											  TRUE,TRUE,TRUE,text_color,TRUE,permutation,WHITE,adjusted_text_width,FALSE,insertion_point);
					// replaced adjusted_width with adjusted_text_width on 031104 since otherwise clipped long integers stick out a tiny bit
					// but recreated this edit only on 081104
#if TT_DEBUG_ON
					add_to_log_if_interesting_error();
#endif
				};
			} else if (adjusted_text_width > tile_width && wide_text_length > 12) { // new on 060202 to show something when text is too much
				// was > 3 prior to 020703
				// changed from ... to first 9 digits followed by ... but much smaller font on 020703 to look more like tiny text
				wide_character wide_text9 = wide_text[9];
				wide_character wide_text10 = wide_text[10];
				wide_character wide_text11 = wide_text[11];
				wide_text[9] = '.';
				wide_text[10] = '.';
				wide_text[11] = '.';
				// changed lly in the following to center better on 080803
				tt_screen->text_out((string)wide_text,12,start_x,adjusted_lly+(3*adjusted_height)/4-edge_size, // adjusted_text_height+edge_size, 
										  (96*adjusted_text_width)/(3*72*4),adjusted_character_height/4,TRUE,TRUE,TRUE,text_color,TRUE,permutation,WHITE,adjusted_width); // /3 makes more sense but 96/3*72 looks better
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
#if TT_ALPHA_FEATURE
	if (tt_right_to_left_text) { // need to restore the setting
		// experimented by adding TA_RIGHT^ on 270104
		tt_main_window->set_text_align(TA_RTLREADING); // so if a Hebrew or Arabic font is selected then text will be right-to-left - new on 081203
	};
#endif
};

#if TT_SHRINKING_AND_GROWING
void Number::display_shrinking_decimal_old(wide_string wide_text, int wide_text_length,
														 city_coordinate ulx, city_coordinate uly,
														 city_coordinate character_width, city_coordinate character_height,
														 bytes color_permutation,
														 city_coordinate total_width, // re-used on 080803 to mean area for digits not including edges
														 city_coordinate height_or_region_height) {
// renamed if compiling with new number display code since it can call this if !tt_shrink_and_grow_numbers -- 141104
#else
void Number::display_shrinking_decimal(wide_string wide_text, int wide_text_length,
													city_coordinate ulx, city_coordinate uly,
													city_coordinate character_width,	city_coordinate character_height,
													bytes color_permutation,
													city_coordinate total_width, // re-used on 080803 to mean area for digits not including edges
													city_coordinate height_or_region_height) {
#endif
	// major rewrite on 150804 to be much more accurate 
	// rather than multiply by number_shrinkage and then round and then find nearest font size
	// this version uses a double to keep track of the "thruth"
#if TT_DEBUG_ON
	debug_log(debug_counter,"Displaying shrinking digits of debug target.");
#endif
	city_coordinate true_height = height_or_region_height; // new on 041003 so remote looks work correctly
	int full_size_part = wide_text_length;
	for (int i = 0; i < wide_text_length; i++) {
		if (wide_text[i] == '.') {
			full_size_part = i+2; // decimal point and first digit afterwards are full-size - was +1 prior to 080803 but then first digit after decimal point not treated special
			break;
		};
	};
//	city_coordinate original_lly = uly-character_height; // new on 110803
	city_coordinate original_ulx = ulx;
	city_coordinate original_uly = uly;
	city_coordinate original_character_height = character_height;
	int ignore_longest_line;
	city_coordinate width_of_next, height_of_next;
	tt_screen->get_extent_size(wide_text,1,1,character_width,character_height, // new on 160804
									   ignore_longest_line,width_of_next,height_of_next,
				                  TRUE,TRUE,FALSE);
	uly = original_uly-(true_height-height_of_next)/2; // in case character has been shrunk - restored on 120803 -- prior to 160804 height_of_next was character_height
//	uly = original_lly+character_height; // in case character has been shrunk
//	city_coordinate full_size_part_width = (total_width*full_size_part)/wide_text_length;
	tt_screen->text_out((string) wide_text,full_size_part,ulx,uly, // adjusted_text_height+edge_size, 
		// replaced character_width,character_height with width_of_next,height_of_next on 160804
								width_of_next,height_of_next,TRUE,TRUE,TRUE,text_color,TRUE,color_permutation,WHITE,width_of_next,FALSE);
	// FALSE added as last arg on 100804 since can't contain tabs
	int remaining = wide_text_length-full_size_part;
	city_coordinate max_ulx = ulx+total_width;
	wide_string text_remaining = wide_text+full_size_part;
//	ulx += width_of_next;
	city_coordinate width_of_full_size_part,ignore_height;
	tt_screen->get_extent_size(wide_text,full_size_part,1,width_of_next,height_of_next, 
									   ignore_longest_line,width_of_full_size_part,ignore_height,
				                  TRUE,TRUE,FALSE);
	double ulx_as_double = ulx+width_of_full_size_part; // updated on 160804
	double character_width_as_double = width_of_next*.8; // character_width; -- 80% seems to make it look better
	double character_height_as_double = height_of_next; // character_height;
	city_coordinate region_min_x = tt_screen->viewing_region()->min_x; // so if flying a helicopter the digits in the water are displayed
	for (int i = 0; i < remaining; i++) {
#if TT_SHRINKING_AND_GROWING
		character_width_as_double *= part2_shrinkage;
		character_height_as_double *= part2_shrinkage;
#else
		character_width_as_double *= number_shrinkage;
		character_height_as_double *= number_shrinkage;
#endif
		character_width = (city_coordinate) character_width_as_double;
		uly = (city_coordinate) (original_uly-(true_height-character_height_as_double)/2); // height was original_character_height prior to 120803
//		uly = original_lly; // +new_character_height;
		if (ulx_as_double+character_width_as_double > region_min_x) { // condition new on 100804 -- was > 0 prior to 160804
			city_coordinate character_height = (city_coordinate) character_height_as_double;
			if (!tt_screen->text_out((string) text_remaining,1,(city_coordinate) ulx_as_double,uly, // adjusted_text_height+edge_size, 
											 character_width,(city_coordinate) character_height_as_double,
											 TRUE,TRUE,TRUE,text_color,TRUE,color_permutation,WHITE,character_width,FALSE)) {
				break; // new on 160804 to stop when too small or off screen
			};
			// FALSE added as last arg on 100804 since can't contain tabs
		//} else {
		//	width_of_next = character_width; // since is fixed width font
		};
//		ulx_as_double += width_of_next;
		ulx_as_double += character_width_as_double;
		if (ulx_as_double > max_ulx) {
			return; // new on 080803
		};
//		uly -= (character_height-new_character_height)/2; // to keep the bottom fixed
//		character_height = new_character_height;
		text_remaining++;
	};
	// following an attempt to improve the appearance on 080803 but sometimes goes too far -- restored and updated on 160804
	ulx = (city_coordinate) ulx_as_double;
	city_coordinate dot_character_width = minimum_character_width();
	city_coordinate dot_character_height = minimum_character_height();
	while (total_width-(ulx-original_ulx) > 0 && ulx < max_ulx) { 
		tt_screen->text_out(".",1,ulx,uly,dot_character_width,dot_character_height,TRUE,TRUE,TRUE,text_color,FALSE,color_permutation,WHITE,dot_character_width,FALSE);
		ulx += dot_character_width;
	};
};