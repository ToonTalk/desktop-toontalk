// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved
#ifndef __DIRTY_H

#define __DIRTY_H

void initialize_page_grid(boolean reset);
void release_page_grid();
void reset_grid();
boolean save_region(coordinate min_x, coordinate max_x, coordinate min_y, coordinate max_y);
void restore_regions(long last_frame=-1);

void save_region_to_memory(coordinate min_x, coordinate max_x, coordinate min_y, coordinate max_y, bytes memory);
void restore_region_from_memory(coordinate min_x, coordinate max_x, coordinate min_y, coordinate max_y, bytes memory);

//#if TT_DIRECTX
//void copy_back_surface_to_cache_page();
//void copy_cache_page_to_back_surface();
//void copy_cache_pages();
//#endif

//void dirty_region(int &min_x, int &min_y, int &max_x, int &max_y);
#endif
