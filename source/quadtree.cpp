// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif   
#if !defined(__TT_SCREEN_H)   
#include "screen.h"
#endif
#if !defined(__TT_CITY_H)
#include "city.h"
#endif
#if !defined(__TT_QUADTREE_H)   
#include "quadtree.h"
#endif   
#if !defined(__TT_REGION_H)   
#include "region.h"
#endif   
#if TT_XML
#if !defined(__TT_XML_H)   
#include "xml.h"
#endif
#endif

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* implements a quad tree of blocks */

QuadTree::QuadTree(){
  blocks_in_region = 0;
//  blocks = new Block_pointer[max_per_level];
  sub_quads = NULL;
};

void QuadTree::set_region(block_number quad_min_x, block_number quad_max_x,
                          block_number quad_min_y, block_number quad_max_y) {
//  PRECONDITION((quad_min_x <= quad_max_x) && (quad_min_y <= quad_max_y));
  min_x = quad_min_x;
  max_x = quad_max_x;
  min_y = quad_min_y;
  max_y = quad_max_y;
  mid_x = (block_number) (((int) min_x+(int) max_x)/2);
  mid_y = (block_number) (((int) min_y+(int) max_y)/2);
};

QuadTree::~QuadTree() {
#if TT_DEBUG_ON
	if (tt_debug_mode == 3231) {
		tt_error_file() << "Deleting quad tree. Frame=" << tt_frame_number << endl;
	};
#endif
	prepare_for_deletion();  
};

void QuadTree::prepare_for_deletion() {
	if (sub_quads != NULL) {
	  delete [] sub_quads;
	  sub_quads = NULL;
	} else {	
		for (int i = 0; i < blocks_in_region; i++) {
			delete blocks[i];
		};
	};
	// the following used to be only in the else branch (i.e. when sub_quads == NULL) prior to 090803
	blocks_in_region = 0;
};

Block *QuadTree::block_at_address(city_coordinate x, city_coordinate y) {
	block_number block_x = (block_number) ((x/tt_block_width)%tt_city->return_number_of_x_streets()); // mod is new on 290903 - needed at least in a 5x5 city near the upper left corner
	block_number block_y = (block_number) ((y/tt_block_height)%tt_city->return_number_of_y_streets());
	return(block_at(block_x,block_y,TRUE));
};

void block_address_at(city_coordinate x, city_coordinate y,
                      block_number &block_x, block_number &block_y) {
	block_x = (block_number) (x/tt_block_width);
	block_y = (block_number) (y/tt_block_height);
};

Block *QuadTree::block_at(block_number x, block_number y, boolean create) {
  // either finds an existing block at that location or returns NULL
#if TT_DEBUG_ON
  if (tt_debug_mode == 7024) {
	  tt_error_file() << (int) x << "," << (int) y << endl;
  };
#endif
  if (sub_quads == NULL) {
     for (int i = 0; i < blocks_in_region; i++) {
			if (blocks[i]->at(x,y)) {
            return(blocks[i]);
         };
     };
     if (create) {
        Block *block = new Block(x,y);
        add_block(block);
        return(block);
     } else {
        return(NULL);
     };
  } else {
     Block *block = sub_quads[quad_index(x,y)].block_at(x,y,FALSE);
     if (create && block == NULL) {
        block = new Block(x,y);
        add_block(block);
     };
     return(block);
  };
};

void QuadTree::add_block(Block *new_block) {
   // this assumes that someone has already checked that this location is free
	if (sub_quads == NULL) {
		if (blocks_in_region < max_blocks_per_level) {
			blocks[blocks_in_region] = new_block;
		} else { // full so distribute the blocks to each of the sub quads
			sub_quads = new QuadTree[4];
			sub_quads[0].set_region(min_x, mid_x, min_y, mid_y);
			sub_quads[1].set_region(min_x, mid_x, (block_number) (mid_y+1), max_y);
			sub_quads[2].set_region((block_number) (mid_x+1), max_x, min_y, mid_y);
			sub_quads[3].set_region((block_number) (mid_x+1), max_x, (block_number) (mid_y+1), max_y);
			block_number x,y;
			for (int i = 0; i < blocks_in_region; i++) {
				 blocks[i]->location(x,y);
				 sub_quads[quad_index(x,y)].add_block(blocks[i]);
			};
//			delete [] blocks;
			blocks_in_region = 0;
		};
	};
	// I don't use an else here because this may have become true just now
	if (sub_quads != NULL) {
		block_number x,y;
		new_block->location(x,y);
		sub_quads[quad_index(x, y)].add_block(new_block);
	};
	blocks_in_region++; // maintain this even if it has sub quads
};

void QuadTree::remove_house(House *house, block_number x, block_number y) {
	Block *block = block_at(x,y,FALSE);
#if TT_DEBUG_ON
	if (block == NULL) {
		tt_error_file() << "Couldn't find block at " << (int) x << " and " << (int) y << endl;
		return;
	};
#endif
	block->remove_house(house);
};

//	if (sub_quads == NULL) {
//      for (int i = 0; i < blocks_in_region; i++) {
//			blocks[i]->remove_house(house);
//      };
//   } else {
//      sub_quads[quad_index(x,y)].remove_house(house,x,y);
//   };


void QuadTree::display(TTRegion *view_region) {
	if (sub_quads == NULL) {
		for (int i = 0; i < blocks_in_region; i++) {
			 blocks[i]->display(view_region);
		};
	} else {
		unsigned char quads = which_quads(view_region);
		for (int i = 0; i<4; i++) {
			 if ((quads>>i)&1) {
				 sub_quads[i].display(view_region);
			 };
		};
	};
};

void QuadTree::update_display() { // doesn't care about viewing region
	if (sub_quads == NULL) {
		for (int i = 0; i < blocks_in_region; i++) {
			 blocks[i]->update_display();
		};
	} else {
		for (int i = 0; i<4; i++) {
			 sub_quads[i].update_display();
		};
	};
};

void QuadTree::finish_instantly() {
	if (sub_quads == NULL) {
		for (int i = 0; i < blocks_in_region; i++) {
			 blocks[i]->finish_instantly();
		};
	} else {
		for (int i = 0; i<4; i++) {
			 sub_quads[i].finish_instantly();
		};
	};
};

void QuadTree::screen_flushed() {
	if (sub_quads == NULL) {
		for (int i = 0; i < blocks_in_region; i++) {
			 blocks[i]->screen_flushed();
		};
	} else {
		for (int i = 0; i<4; i++) {
			 sub_quads[i].screen_flushed();
		};
	};
};

boolean QuadTree::predicate_true_of_any_extras(PredicateCallBack predicate, void *arg) { // new on 130205
	if (sub_quads == NULL) {
		for (int i = 0; i < blocks_in_region; i++) {
			if (blocks[i]->predicate_true_of_any_extras(predicate,arg)) {
				return(TRUE);
			};
		};
	} else {
		for (int i = 0; i<4; i++) {
			 if (sub_quads[i].predicate_true_of_any_extras(predicate,arg)) {
				 return(TRUE);
			 };
		};
	};
	return(FALSE);
};

void QuadTree::dump(output_stream &stream, boolean just_prepare) {
   if (sub_quads == NULL) {
		for (int i = 0; i < blocks_in_region; i++) {
			 blocks[i]->dump(stream,just_prepare);
		};
	} else {
		for (int i = 0; i<4; i++) {
			 sub_quads[i].dump(stream,just_prepare);
		};
	};
};

#if TT_XML

void QuadTree::add_xml(xml_element *city_element, xml_document *document) { // new on 311002
   if (sub_quads == NULL) {
		for (int i = 0; i < blocks_in_region; i++) {
		   xml_append_child(blocks[i]->xml(document),city_element);
		};
	} else {
		for (int i = 0; i<4; i++) {
		   sub_quads[i].add_xml(city_element,document);
		};
	};
};

#endif

unsigned char QuadTree::which_quads(TTRegion *view_region) {
	// first compute quadrant locations for opposite corners of view_region
	// think about how to avoid the division by block dimensions here
	boolean const corner1_quad_x = (view_region->min_x/tt_block_width > mid_x),
					  corner1_quad_y = (view_region->min_y/tt_block_height > mid_y),
					  corner2_quad_x = (view_region->max_x/tt_block_width+1 > mid_x),
					  corner2_quad_y = (view_region->max_y/tt_block_height+1 > mid_y);
	if (corner1_quad_x == corner2_quad_x) {
		if (corner1_quad_y == corner2_quad_y) { // same quadrant
			return((unsigned char) (1<<(2*corner1_quad_x+corner1_quad_y)));
		} else { // equal in x but not in y so display 2 quads
			return((unsigned char) ((1<<(2*corner1_quad_x))|(1<<(2*corner1_quad_x+1))));
		};
   } else { // x coordinates differ
      if (corner1_quad_y == corner2_quad_y) { // but y is the same
			return((unsigned char) ((1<<corner1_quad_y)|(1<<(2+corner1_quad_y))));
     } else { // region spans all for quadrants
         return(15); // all ones
     };
   };
};
