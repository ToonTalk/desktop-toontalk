// Copyright (c) 1992-2006.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)
#include "defs.h"
#endif   
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif   
#if !defined(__TT_BLOCK_H)
#include "block.h"
#endif   
#if !defined(__TT_HOUSE_H)   
#include "house.h"
#endif   
#if !defined(__TT_HELP_H)   
#include "help.h"
#endif   
#if !defined(__TT_SCREEN_H)
#include "screen.h"
#endif
#if !defined(__TT_CITY_H)
#include "city.h"
#endif
#if !defined(__TT_PAD_H)
#include "pad.h"
#endif
#if !defined(__TT_SPRITES_H)
#include "sprites.h"
#endif

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const city_coordinate ideal_block_height = ideal_screen_width;

Block::Block(block_number x, block_number y) :
//	number_of_houses(0),
	number_of_extras(0),
	x(x),
	y(y),
	extras(NULL),
	frame_number_of_last_display(0) {
  houses = new HousePointer[tt_houses_to_a_block];
  for (int i = 0; i < tt_houses_to_a_block; i++) {
	  houses[i] = NULL;
  };
};

Block::~Block() {
#if TT_DEBUG_ON
	if (tt_debug_mode == 3231) {
		tt_error_file() << "Deleting block " << (int) x << "," << (int) y << " frame=" << tt_frame_number << endl;
	};
#endif
  int i;
//  if (number_of_houses > 0) {
	  for (i = 0; i < tt_houses_to_a_block; i++) {
		  if (houses[i] != NULL) delete houses[i];
	  };
	  delete [] houses; // was commented out prior to 080699
	  houses = NULL; // new on 210704
//  };
//#if TT_DEBUG_ON
//  if (tt_debug_mode == 210704) return;
//#endif
  if (number_of_extras > 0) {
	  for (i = 0; i < number_of_extras; i++) {
	  	  extras[i]->destroy(); 
		  // only once since is a weak pointer but may be only way to reach sprite (e.g. time travel segment boundary)
	  };
	  number_of_extras = 0; // new on 210704
	  delete [] extras;
	  extras = NULL; // new on 210704
  };
};

void Block::remove_house(House *house) {
  for (int i = 0; i < tt_houses_to_a_block; i++) {
	  if (houses[i] == house) {
		  houses[i] = NULL;
		  return;
	  };
  };
//#if TT_DEBUG_ON -- commented out on 020405
//	 say_error(_T("Warning: ToonTalk tried to remove a house from a block where it wasn't!"));
//#endif
};
  
House *Block::build_house(HouseStyle style) {
  city_coordinate x,y;
  short int index;
  if (next_free_lot(x,y,index)) {
		House *house = new House(x,y,this,style);
//		tt_error_file() << "new house at " << x << "," << y << " at frame " << tt_frame_number << endl;
		add_house(house,index);
		return(house);
	} else {
	  return(NULL);
	};
};

void Block::add_extra(Sprite *extra) {
	if (extra == NULL) return; // new on 220704 for robustness
  SpritePointer *temp_extras = new SpritePointer[number_of_extras+1];
  if (number_of_extras > 0) {
	  for (int i = 0; i < number_of_extras; i++) {
#if TT_DEBUG_ON
		  if (extras[i] == extra) {
			  say_error(_T("ToonTalk confused and tried to add something to a block that was already there."));
		  };
#endif
		  temp_extras[i+1] = extras[i];
	  };
	  delete [] extras;
  };
  extras = temp_extras;
  extras[0] = extra;
  extra->increment_ref_count(); // new on 220704
  number_of_extras++;
  frame_number_of_last_display = 0; // force re-addition of all if visible
};

void Block::remove_extra(Sprite *extra) {
#if TT_DEBUG_ON
	boolean found = FALSE;
#endif
	for (int i = 0; i < number_of_extras; i++) {
		if (extras[i] == extra) {
			for (int j = i; j < number_of_extras-1; j++) {
				extras[j] = extras[j+1];
			};
			number_of_extras--;
			extras[number_of_extras] = NULL; // tidier and possibly more robot -- new on 220704
			extra->destroy(); // release reference -- new on 220704
#if TT_DEBUG_ON
			found = TRUE;
#endif
			break;
		};
	};
#if TT_DEBUG_ON
	if (!found) {
		say_error(_T("ToonTalk confused and tried to remove something from a block that wasn't there."));
	};
#endif
	if (number_of_extras == 0) {
		delete [] extras;
		extras = NULL;
	};
};

Sprites *Block::extras_in_region(TTRegion region) {
   Sprites *result = NULL;
   TTRegion extras_region;
   for (int i = 0; i < number_of_extras; i++) {
      extras[i]->full_region(extras_region);
      if (extras_region.overlaps(region)) {
         result = new Sprites(extras[i],result);
      };
   };
   return(result);
};

boolean Block::next_free_lot(short int &index) {
	for (index = 0; index < tt_houses_to_a_block; index++) {
		if (houses[index] == NULL) { // || houses[index]->is_about_to_be_destroyed()) { 
			// second disjunct added 020405 since about to go away so OK to replace it -- commented out on 020405 since too hackish
			return(TRUE);
		};
	};
	return(FALSE);
};

boolean Block::next_free_lot(city_coordinate &city_x, city_coordinate &city_y, short int &index) {
	if (next_free_lot(index)) {
      city_location(city_x,city_y,index);
		return(TRUE);
	};
	return(FALSE);
};

boolean Block::closest_free_lot(city_coordinate &city_x, city_coordinate &city_y, short int &best_index) {
	// new on 040803 - variant of next_free_lot but instead of the leftmost free one it is the closest to the orginal coordinates
	best_index = -1;
	int best_distance_squared = max_long;
	for (int index = 0; index < tt_houses_to_a_block; index++) {
		if (houses[index] == NULL) {
			coordinate lot_x, lot_y;
			city_location(lot_x,lot_y,index);
			int distance_squared = (city_x-lot_x)*(city_x-lot_x)+(city_y-lot_y)*(city_y-lot_y);
			if (distance_squared < best_distance_squared) {
				best_distance_squared = distance_squared;
				city_x = lot_x;
				city_y = lot_y;
				best_index = index;
			};
		};
	};
	return(best_index >= 0);
};

void Block::city_location(city_coordinate &city_x, city_coordinate &city_y, short int index) {
	city_x = x*tt_ideal_block_width+(index+1)*(tt_ideal_block_width/(tt_houses_to_a_block+1))+tt_default_ideal_block_width/30;
	city_y = y*ideal_block_height+(2*ideal_block_height)/3; // should really know the street size
};

void Block::display(TTRegion *view_region) {
	int i;
	for (i = 0; i < tt_houses_to_a_block; i++) {
		if (houses[i] != NULL) houses[i]->display(view_region);
	};
	if (frame_number_of_last_display+1 != tt_frame_number) {
		for (i = 0; i < number_of_extras; i++) {
			if (extras[i]->return_ref_count() == 1) { // only pointer is from this and this is weak so get rid of it -- new on 220704
				remove_extra(extras[i]);
			} else {
				tt_screen->add(extras[i]); // no-op if already there
			};
//tt_error_file() << "Adding " <<  extras[i] << " f=" << tt_frame_number << endl;
		};
	};
	frame_number_of_last_display = tt_frame_number;
};

void Block::update_display() {
	for (int i = tt_houses_to_a_block-1; i >= 0; i--) { // go in reverse order in case one removes itself
		if (houses[i] != NULL) houses[i]->update_display();
	};
	for (int i = number_of_extras-1; i >= 0; i--) { 
		if (extras[i]->return_ref_count() == 1) { // only pointer is from this and this is weak so get rid of it -- new on 220704
			remove_extra(extras[i]); // should be OK since work from the end to the beginning
		} else {
			extras[i]->update_display();
		};
	};
	if (frame_number_of_last_display+2 == tt_frame_number) {
	// didn't display this block last frame
		for (int i = 0; i < number_of_extras; i++) {
			tt_screen->remove(extras[i]);
//			tt_error_file() << "Removing " <<  extras[i] << " f=" << tt_frame_number << endl;
		};
	};
};

void Block::finish_instantly() {
	int i;
	for (i = 0; i < tt_houses_to_a_block; i++) {
		if (houses[i] != NULL) houses[i]->finish_instantly();
	};
	for (i = 0; i < number_of_extras; i++) {
		if (extras[i]->return_ref_count() > 1) { // conditional new on 220704
			extras[i]->finish_instantly();
#if TT_DEBUG_ON
		} else {
			debug_this("OK that Block::finish_instantly has an extra with a low ref count?");
#endif
		}; // else will be removed soon
	};
};

void Block::screen_flushed() {
   frame_number_of_last_display = 0;
};

boolean Block::predicate_true_of_any_extras(PredicateCallBack predicate, void *arg) { // new on 130205
	for (int i = 0; i < number_of_extras; i++) {
		if (predicate(extras[i],arg)) {
			return(TRUE);
		};
	};
	return(FALSE);
};

#if TT_XML

xml_element *Block::xml(xml_document *document) { // new on 311002
   xml_element *block_element = create_xml_element(L"Block",document);
   xml_set_attribute(block_element,L"X",x);
   xml_set_attribute(block_element,L"Y",y);
   int i;
	for (i = 0; i < tt_houses_to_a_block; i++) {
		if (houses[i] != NULL) {
         xml_element *house_element = houses[i]->xml(document);
         xml_set_attribute(house_element,L"Number",(i+1)); // 1-indexing 
         xml_append_child(house_element,block_element);
      };
	};
   if (number_of_extras > 0) {
      int number_extras_dumped = 0;
      xml_element *extras_element = create_xml_element(L"On",document); // new on 041102
	   for (i = 0; i < number_of_extras; i++) {
			if (extras[i]->ok_to_dump() && extras[i]->return_ref_count() > 1) { // second condition new on 220704
#if TT_DEBUG_ON
				if (extras[i]->kind_of() == HELICOPTER) {
					debug_this();
				};
#endif
            extras[i]->xml_create_and_append_element(extras_element,document)->Release(); // geometry should be saved -- 011102
            number_extras_dumped++;
         };
	   };
      if (number_extras_dumped > 0) {  
         xml_append_child(extras_element,block_element);
      } else {
         extras_element->Release();
      };
   };
   return(block_element);
};

boolean Block::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
      case HOUSE_TAG: {
         city_coordinate house_x, house_y;
         int number = xml_get_attribute_int(node,L"Number",1)-1;
         if (number < 0 || number >= tt_houses_to_a_block) return(FALSE); // warn??
         city_location(house_x,house_y,number); // convert from 1-indexing to 0
         houses[number] = new House(house_x,house_y,this,(HouseStyle) xml_get_attribute_int(node,L"StyleCode",0),TRUE); 
			// final arg is new so house is built instanntly
         return(xml_entity(node,houses[number]));
                      };
      case ON_TAG: {
//#if TT_DEBUG_ON
//			xml_debug_node(node);
//#endif
         Sprites *extras_as_list = xml_load_sprites(node);
         if (extras_as_list == NULL) {
            number_of_extras = 0;
         } else {
            number_of_extras = extras_as_list->length();
				extras = new SpritePointer[number_of_extras]; // new on 261202
            for (int i = 0; i < number_of_extras; i++) {
               extras[i] = extras_as_list->first();
					extras[i]->increment_ref_count(); // new on 220704
					extras_as_list->set_first(NULL); // new on 220704 to plug leak
					extras[i]->set_current_block(this); // new on 261202
               extras[i]->activate(); // still make sense? 041102
               extras_as_list = extras_as_list->rest();
            };
				delete extras_as_list; // since all NULLs -- new on 220704
         };         
         return(TRUE);
                   };
      case NO_MORE_TAGS:
         return(TRUE);
      default:
         return(FALSE); // save since is extra_xml?? 041102
   };
};

#endif

void Block::dump(output_stream &stream, boolean just_prepare) {
   if (just_prepare) {
      // problem is that running the following may add or remove houses or extras so copy the list first
      HousePointer *original_houses = new HousePointer[tt_houses_to_a_block];
	   int i;
      for (i = 0; i < tt_houses_to_a_block; i++) original_houses[i] = houses[i];
      for (i = 0; i < tt_houses_to_a_block; i++) {
		   if (original_houses[i] != NULL) {
            original_houses[i]->dump(stream,just_prepare);
         };
      };
      delete [] original_houses;
      if (number_of_extras > 0) {
         SpritePointer *original_extras = new SpritePointer[number_of_extras];
         int original_number_of_extras = number_of_extras;
         for (i = 0; i < number_of_extras; i++) original_extras[i] = extras[i];
         for (i = 0; i < original_number_of_extras; i++) {
            original_extras[i]->dump(stream,just_prepare);
         };
         delete [] original_extras;
      };
      return;
   };
	if (number_of_extras == 0) { // new on 080699
		int i;
		for (i = 0; i < tt_houses_to_a_block; i++) {
			if (houses[i] != NULL) break;
		};
		if (i == tt_houses_to_a_block) { // no houses
			return;
		};
	};
   stream.put(BLOCK_MARKER);
   stream.write((character *) &x, sizeof(x));
   stream.write((character *) &y, sizeof(y));
   int i;
	for (i = 0; i < tt_houses_to_a_block; i++) {
		if (houses[i] != NULL) {
         houses[i]->dump(stream);
      } else {
         stream.put(NOTHING_MARKER);
      };
	};
   short int dumpable_number_of_extras = (short int) number_of_extras;
   for (i = 0; i < number_of_extras; i++) {
      if (!extras[i]->ok_to_dump()) dumpable_number_of_extras--;
   };
   stream.write((character *) &dumpable_number_of_extras, 
		          sizeof(dumpable_number_of_extras));
	for (i = 0; i < number_of_extras; i++) {
		if (extras[i]->ok_to_dump()) {
         extras[i]->dump(stream,just_prepare);
         extras[i]->dump_geometry(stream);
      };
	};
};

void load_block(InputStream *stream, int notebook_version, NaturalLanguage language, City *city) {
   old_block_number x,y; // was just block_number prior to 161105 but then doesn't work if TT_LARGE_CITIES
   stream->read((string) &x, sizeof(x));
   stream->read((string) &y, sizeof(y));
   Block *block = city->block_at(x,y);
   block->load_houses_and_extras(stream,notebook_version,language);
};

void Block::load_houses_and_extras(InputStream *stream, int notebook_version, NaturalLanguage language) {
	for (short int i = 0; i < tt_houses_to_a_block; i++) {
      if (stream->get() != NOTHING_MARKER) {
         city_coordinate house_x, house_y;
         city_location(house_x,house_y,i);
#if TT_DEBUG_ON
			long start = stream->tellg();
#endif
         houses[i] = load_house(stream,notebook_version,language,house_x,house_y,this);
#if TT_DEBUG_ON
			if (tt_debug_mode == 141100) {
				tt_error_file() << (stream->tellg()-start) << " bytes in encoding of the house#" << i 
									 << " at " << (int) x << "," << (int) y << endl;
			};
#endif
      }; // otherwise is already NULL
	};
   short int short_number_of_extras; // new on 051102 to allow it to be a long
   stream->read((string) &short_number_of_extras, sizeof(short_number_of_extras));
   number_of_extras = short_number_of_extras;
   if (number_of_extras > 0) {
#if TT_DEBUG_ON
		long start = stream->tellg();
#endif
      extras = new SpritePointer[number_of_extras];
      short int NULL_count = 0;
      for (int i = 0; i < number_of_extras; i++) {
         extras[i] = load_item(stream,notebook_version,language);
         if (extras[i] != NULL) {
            extras[i]->load_geometry(stream,notebook_version);
            extras[i]->activate();
				extras[i]->increment_ref_count(); // new on 220704
         } else {
            NULL_count++;
         };
      };
      if (NULL_count > 0) {
         if (number_of_extras > NULL_count) {
            SpritePointer *new_extras = new SpritePointer[number_of_extras-NULL_count];
            int j = 0;
            for (int i = 0; i < number_of_extras; i++) {
               if (extras[i] != NULL) new_extras[j++] = extras[i];
            };
            delete [] extras;
            extras = new_extras;
         } else { // none left
            delete [] extras;
         };
         number_of_extras -= NULL_count;
      };
#if TT_DEBUG_ON
		if (tt_debug_mode == 141100) {
			tt_error_file() << (stream->tellg()-start) << " bytes in encoding of stuff outside on block " 
								 << (int) x << "," << (int) y << endl;
		};
#endif
   };
};

short int Block::lot_index(House *house) {
   for (short int i = 0; i < tt_houses_to_a_block; i++) {
      if (houses[i] == house) return(i);
   };
   return(-1);
};

/*
void Block::add_house(House *house) {
  House_Pointer *temp_houses = new House_Pointer[number_of_houses+1];
  if (number_of_houses > 0) {
	  for (int i = 0; i < number_of_houses; i++) {
		  temp_houses[i+1] = houses[i];
	  };
	  delete [] houses;
  };
  houses = temp_houses;
  houses[0] = house;
  number_of_houses++;
};

void Block::remove_house(House *house) {
#if TT_DEBUG_ON
	if (number_of_houses == 0) {
		tt_error_file() << "Attempting to remove a house from an empty block" << endl;
		return;
	};
#endif
	if (number_of_houses == 1) {
#if TT_DEBUG_ON
		if (houses[0] != house) {
			tt_error_file() << "Attempting to remove a missing house" << endl;
			return;
		};
#endif	
		delete [] houses;
		houses = NULL;
	} else {
		House_Pointer *temp_houses = new House_Pointer[number_of_houses-1];
		int house_index = 0;
		for (int i = 0; i < number_of_houses; i++) {
			if (houses[i] != house) {
				temp_houses[house_index++] = houses[i];
			};
		};
		delete [] houses;
		houses = temp_houses;
	};
	number_of_houses--;
};
*/

