// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved
#if !defined(__TT_BLOCK_H)

#define __TT_BLOCK_H

#if !defined(__TT_HOUSE_H)   
#include "house.h"
#endif   

class Sprite;
class TTRegion;
class City;
class Screen;

typedef House *HousePointer;

class Block : public Entity { // implements a city block
  public:
    Block(block_number x, block_number y);
    ~Block();
	 void display(TTRegion *view_region);
	 void update_display();
	 void finish_instantly();
    void screen_flushed();
	 boolean predicate_true_of_any_extras(PredicateCallBack predicate, void *arg);
#if TT_XML
    xml_element *xml(xml_document *document);
    boolean handle_xml_tag(Tag tag, xml_node *node);
    boolean handle_block_extra_xml_tag(Sprite *ignore1, Tag tag, xml_node *node, void *ignore2);
#endif
    void dump(output_stream &stream, boolean just_prepare=TRUE);
    void load_houses_and_extras(InputStream *stream, int notebook_version, NaturalLanguage language);
	 House *build_house(HouseStyle style=HOUSE_A);
//    House *house_at(city_coordinate x, city_coordinate y);
	 void add_house(House *house, int index) {
		 houses[index] = house;
	 };
    void remove_house(House *house);
	 boolean next_free_lot(short int &index);
	 boolean next_free_lot(city_coordinate &x, city_coordinate &y, short int &index);
	 boolean closest_free_lot(city_coordinate &city_x, city_coordinate &city_y, short int &best_index);
	 void add_extra(Sprite *appearance);
	 void remove_extra(Sprite *appearance);
    Sprites *extras_in_region(TTRegion region);
	 void location(block_number &block_x, block_number &block_y) {
		block_x = x;
		block_y = y;
	 };
    short int lot_index(House *house);
    House *house_at(short int index) {
		 if (index < 0 || index >= tt_houses_to_a_block) return(NULL); // new on 261202
       return(houses[index]);
    };
	 void city_location(city_coordinate &city_x, city_coordinate &city_y,
							  short int index);
	 block_number return_avenue_number() {
		 return(x);
	 };
	 block_number return_street_number() {
		 return(y);
	 };
	 boolean at(block_number x1, block_number y1) {
		return(x1 == x && y1 == y);
	 };
  private:
	 block_number x,y; // x = city_x/block_width, ...
//	 int number_of_houses
	 int number_of_extras; // was a short int prior to 051102
	 HousePointer *houses; //houses[houses_to_a_block];
//	 TTRegion **bounding_boxes;
	 long frame_number_of_last_display;
	 Sprite **extras; // e.g. the empty helicopter
};

typedef Block *Block_pointer;
void load_block(InputStream *stream, int notebook_version, NaturalLanguage language, City *city);

#endif
