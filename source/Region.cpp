// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif 
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif
#if !defined(__TT_REGION_H)   
#include "region.h"
#endif   
   

boolean TTRegion::mostly_contains(TTRegion *another) {
   // true if at least half of another's region fits inside of me
   city_coordinate half_width = (another->max_x-another->min_x)/2,
                   half_height = (another->max_y-another->min_y)/2;
   if (another->min_x < min_x) {
      if (min_x-another->min_x > half_width) {
         return(FALSE);
      };
   };
   if (another->max_x > max_x) {
      if (another->max_x-max_x > half_width) {
         return(FALSE);
      };
   };
   if (another->min_y < min_y) {
      if (min_y-another->min_y > half_height) {
         return(FALSE);
      };
   };
   if (another->max_y > max_y) {
      if (another->max_y-max_y > half_height) {
			return(FALSE);
		};
	};
	return(TRUE);
};

void TTRegion::move_inside(TTRegion &container_region,
								 city_coordinate &delta_x, city_coordinate &delta_y) {
	// if any part of region is outside of container move it in
	delta_x = container_region.min_x-min_x;
	if (delta_x < 0) { // not left of container
		delta_x = container_region.max_x-max_x;
		if (delta_x > 0) { // not right either
			delta_x = 0;
		};
	};
	delta_y = container_region.min_y-min_y;
	if (delta_y < 0) {
		delta_y = container_region.max_y-max_y;
		if (delta_y > 0) {
			delta_y = 0;
		};
	};	
};

void TTRegion::extend_with(TTRegion &another) {
	if (another.min_x < min_x) min_x = another.min_x;
	if (another.max_x > max_x) max_x = another.max_x;
	if (another.min_y < min_y) min_y = another.min_y;
	if (another.max_y > max_y) max_y = another.max_y;
};

void TTRegion::intersect_with(TTRegion &other, TTRegion &result) {
	// still assumes non empty intersection 
	result.min_x = max(min_x,other.min_x);
	result.min_y = max(min_y,other.min_y);
	result.max_x = min(max_x,other.max_x);
	result.max_y = min(max_y,other.max_y);
	// following new on 290404 to deal with non-empty intersections
	//if (result.min_x > result.max_x) {
	//	result.min_x = result.max_x;
	//};
	//if (result.min_y > result.max_y) {
	//	result.min_y = result.max_y;
	//};
};

void TTRegion::grow_by(int x_factor, int y_factor) {
	// stay centered
	city_coordinate x_half_growth = (width()*(x_factor-1))/2;
	city_coordinate y_half_growth = (height()*(y_factor-1))/2;
	min_x -= x_half_growth;
	max_x += x_half_growth;
	min_y -= y_half_growth;
	max_y += y_half_growth;
};

void TTRegion::print(output_stream &stream) {
	stream << min_x << "," << min_y << " to " << max_x << "," << max_y;
};

void TTRegion::shift_by(city_coordinate delta_x, city_coordinate delta_y) { // new on 171200
	min_x += delta_x;
	max_x += delta_x;
	min_y += delta_y;
	max_y += delta_y;
};

#if TT_ALPHA
city_coordinate TTRegion::random_x() {
	return(min_x+my_random(max_x-min_x));
};
city_coordinate TTRegion::random_y() {
	return(min_y+my_random(max_y-min_y));
};
#endif
	

