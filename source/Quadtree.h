// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved
#if !defined(__TT_QUADTREE_H)

#define __TT_QUADTREE_H
#if !defined(__TT_BLOCK_H)   
#include "block.h"
#endif   
//class Block;
//typedef Block *Block_pointer;
#if !defined(__TT_REGION_H)   
#include "region.h"
#endif   

//const short int QuadTree::max_per_level = 16;
const short int max_blocks_per_level = 16;

class QuadTree {
 public:
  QuadTree();
  ~QuadTree();
  void prepare_for_deletion();
  void display(TTRegion *view_region);
  void update_display();
  void finish_instantly(); // broadcast to everyone
  void screen_flushed();
#if TT_XML
  void add_xml(xml_element *city_element, xml_document *document);
#endif;
  void dump(output_stream &stream, boolean just_prepare=FALSE);
  void add_block(Block *new_block);
  void remove_house(House *house, block_number x, block_number y);
  Block *block_at_address(city_coordinate x, city_coordinate y);
  Block *block_at(block_number x, block_number y, boolean create);
  void set_region(block_number min_x, block_number max_x,
                  block_number min_y, block_number max_y);
  int quad_index(block_number x, block_number y) {
     return(2*(x > mid_x)+(y > mid_y));
  };
  boolean predicate_true_of_any_extras(PredicateCallBack predicate, void *arg);
 private:
  unsigned char which_quads(TTRegion *view_region);
  short int blocks_in_region;  // either directly or via sub quads
//  static const short int max_per_level;
//  Block_pointer *blocks;
  Block_pointer blocks[max_blocks_per_level];
  block_number min_x, max_x, min_y, max_y, mid_x, mid_y;
  QuadTree *sub_quads;
};

void block_address_at(city_coordinate x, city_coordinate y,
                        block_number &block_x, block_number &block_y);

#endif
