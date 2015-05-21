// Copyright (c) 1992-2003.  Ken Kahn, Animated Programs, All rights reserved

#if !defined(__TT_REGION_H)

#define __TT_REGION_H

class TTRegion {
  public:
    TTRegion() {};
    TTRegion(city_coordinate region_min_x, city_coordinate region_max_x,
           city_coordinate region_min_y, city_coordinate region_max_y) {
      min_x = region_min_x;
      max_x = region_max_x;
      min_y = region_min_y;
      max_y = region_max_y;
    };
    ~TTRegion() {};
    boolean inside(city_coordinate x, city_coordinate y) {
      return(x >= min_x && x <= max_x && y >= min_y && y <= max_y);
    };
    void region_center(city_coordinate &center_x,
                       city_coordinate &center_y) {
       center_x = (min_x+max_x)/2;
       center_y = (min_y+max_y)/2;
    };
    void region_size(city_coordinate &width, city_coordinate &height) {
       width = max_x-min_x;
       height = max_y-min_y;
    };
    boolean partly_inside(city_coordinate r_min_x, city_coordinate r_max_x,
                          city_coordinate r_min_y, city_coordinate r_max_y) {
      return(r_min_x < max_x && r_max_x > min_x &&
             r_min_y < max_y && r_max_y > min_y);
    };
    boolean contains(TTRegion *another) {
      return(min_x <= another->min_x &&
             max_x >= another->max_x &&
             min_y <= another->min_y &&
             max_y >= another->max_y);
	 };
	 boolean contains(TTRegion &another) {
		return(min_x <= another.min_x &&
				 max_x >= another.max_x &&
				 min_y <= another.min_y &&
				 max_y >= another.max_y);
	 };
    boolean overlaps(TTRegion *another) {
      return(!(another->min_x > max_x ||
               another->max_x < min_x ||
               min_x > another->max_x ||
               max_x < another->min_x ||
               another->min_y > max_y ||
               another->max_y < min_y ||
               min_y > another->max_y ||
               max_y < another->min_y));
	 };
	 boolean overlaps(TTRegion &another) {
		return(!(another.min_x > max_x ||
					another.max_x < min_x ||
					min_x > another.max_x ||
					max_x < another.min_x ||
					another.min_y > max_y ||
					another.max_y < min_y ||
					min_y > another.max_y ||
					max_y < another.min_y));
	 };
	 void extend_with(TTRegion &another);
    boolean mostly_contains(TTRegion *another);
    city_coordinate width() {
		 return(max_x-min_x);
	 };
	 city_coordinate height() {
		 return(max_y-min_y);
	 };
	 void intersect_with(TTRegion &other_region, TTRegion &result);
	 void move_inside(TTRegion &container_region,
							city_coordinate &delta_x, city_coordinate &delta_y);
	 void print(output_stream &stream);
	 void grow_by(int x_factor, int y_factor);
	 void shift_by(city_coordinate delta_x, city_coordinate delta_y);
 #if TT_ALPHA
	 city_coordinate random_x();
	 city_coordinate random_y();
#endif
	 // yes, the following is public...	 
	 city_coordinate min_x, max_x, min_y, max_y;
};

#endif
