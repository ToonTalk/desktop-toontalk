// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved
#if !defined(__TT_CITY_H)

#define __TT_CITY_H

#if !defined(__TT_QUADTREE_H)   
#include "quadtree.h"
#endif   
#if !defined(__TT_BCKGRND_H)   
#include "bckgrnd.h"
#endif
#if !defined(__TT_REGION_H)   
#include "region.h"
#endif
#if !defined(__TT_HOUSE_H)
#include "house.h"
#endif
#if TT_XML
#if !defined(__TT_XML_H)
#include "xml.h"
#endif
#endif

class City : public Visible_Background {
 public:
  City();
  ~City();
  	SpriteType entity_type() { // new on 040803
		return(WHOLE_CITY);
	};
  void build_initial_houses();
  boolean update_display(TTRegion *region);
  boolean display_region();
  void finish_instantly();
  void screen_flushed();
#if TT_XML
//   xml_element *xml(xml_document *document);
   xml_document *xml_new_document(boolean include_explanation=FALSE);
	void add_xml_attributes(xml_element *element, xml_document *document);
   void add_xml(xml_element *city_element, xml_document *document);
   boolean handle_xml_tag(Tag tag, xml_node *node);
	void set_xml_after_titles(xml_document *node);
	void xml_entity_and_activate(xml_document *node);
	void process_xml_after_titles();
	wide_string xml_tag() {
		return(L"City");
	};
	//boolean xml_processing_pending() { // new on 090103
	//	return(xml_after_titles != NULL);
	//};
#endif
  boolean dump_to_new_file(ascii_string file_name, boolean save_media_too=FALSE, boolean check_xml_after_titles=FALSE, boolean zip_file=tt_zip_files);
  void dump_to_new_file_internal(ascii_string file_name, ascii_string temp_file_name);
  ascii_string compute_full_file_name(ascii_string file_name, ascii_string extension, boolean &copied);
  void dump_to_stream(output_stream &stream, boolean save_media_too=FALSE);
  void dump(output_stream &stream, boolean just_prepare=FALSE);
  boolean load_city(ascii_string city_name, boolean directly_loadable=FALSE);
  boolean load_city_from_stream(InputStream *stream, boolean ignore_city=FALSE);
  boolean load(InputStream *stream, boolean ignore_city=FALSE);
//  boolean load_city_from_clipboard_string(string clipboard_text);
  BrushId street_brush_id();
  BrushId lawn_brush_id();
  BrushId water_brush_id();
//  void add_extra(Sprite *extra);
  void add_extra(Sprite *extra, city_coordinate x, city_coordinate y);
//  void remove_extra(Sprite *extra);
  void remove_extra(Sprite *extra, city_coordinate x, city_coordinate y);
  void remove_house(House *house);
  city_coordinate nearest_x_street(city_coordinate view_y);
  city_coordinate nearest_y_street(city_coordinate view_x);
  void nearest_lane(city_coordinate &x, city_coordinate &y, Direction direction);
  Block *block_at(block_number x, block_number y, boolean create=TRUE);
  House *new_house_nearest(block_number x, block_number y, HouseStyle style=HOUSE_A);
  boolean nearest_free_lot(block_number &x, block_number &y, short int &lot);
  boolean valid_address(int block_x, int block_y);
  void transfer_pictures(Floor *floor, TTRegion &region);
  void center(city_coordinate &x, city_coordinate &y);
  void extent(city_coordinate &min_x, city_coordinate &max_x,
              city_coordinate &min_y, city_coordinate &max_y) {
     min_x = city_min_x;
     max_x = city_max_x;
     min_y = city_min_y;
     max_y = city_max_y;
  };
  city_coordinate min_x() {
	 return(city_min_x);
  };
  city_coordinate max_x() {
	 return(city_max_x);
  };
  city_coordinate min_y() {
	 return(city_min_y);
  };
  city_coordinate max_y() {
	 return(city_max_y);
  };
  boolean inside_a_house() {
	return(FALSE);
  };
  block_number return_number_of_x_streets() {
	  return(number_of_x_streets);
  };
  block_number return_number_of_y_streets() {
	  return(number_of_y_streets);
  };
  boolean outside() {
     return(TRUE);
  };
  void set_containment();
  void reset_all_blocks(boolean reset_toontalk_too=FALSE);
  void initialize(boolean reset_toontalk_too=FALSE);
  boolean stopped() {
	  return(stopped_flag);
  };
  void set_stopped(boolean flag) {
	  stopped_flag = flag;
  };
  boolean predicate_true_of_any_extras(PredicateCallBack predicate, void *arg) {
	  return(all_blocks.predicate_true_of_any_extras(predicate,arg));
  };
 private:
  void draw_streets();
  city_coordinate x_street_width(long n);
  city_coordinate y_street_width(long n);
  QuadTree all_blocks;
  city_coordinate city_min_x, city_max_x, city_min_y, city_max_y;
  block_number number_of_x_streets, number_of_y_streets;
  TTRegion view_region;
//  Sprites *extras;
  boolean changed; // no point bit-packing cities - updated 090103 - was flag : 1;
  boolean stopped_flag; // new on 090103
#if TT_XML
  xml_document *xml_after_titles;
#endif
};


void release_file_name_of_last_city_loaded();

#endif

