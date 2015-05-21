// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif
#if !defined(__TT_FLOOR_H)
#include "floor.h"
#endif
#if !defined(__TT_TEXT_H)
#include "text.h"
#endif
#if !defined(__TT_CUBBY_H)
#include "cubby.h"
#endif
#if !defined(__TT_ROBOT_H)
#include "robot.h"
#endif
#if !defined(__TT_NUMBER_H)   
#include "number.h"
#endif	
#if !defined(__TT_ROOM_H)   
#include "room.h"
#endif   
#if !defined(__TT_HOUSE_H)   
#include "house.h"
#endif   
#if !defined(__TT_BLOCK_H)   
#include "block.h"
#endif
#if !defined(__TT_CITY_H)   
#include "city.h"
#endif
#if !defined(__TT_PRGRMMR_H)   
#include "prgrmmr.h"
#endif	
#if !defined(__TT_HELP_H)
#include "help.h"
#endif
#if !defined(__TT_JAVA_H)
#include "java.h"
#endif
#if !defined(__TT_UTILS_H)   
#include "utils.h" // for sound stuff
#endif 
#if TT_XML
#if !defined(__TT_XML_H)
#include "xml.h"
#endif
#endif
#if !defined(__TT_TRUCK_H)   
#include "truck.h"
#endif

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// if changed update help.cpp
enum TruckHistory {TRUCK_RECEIVED_CUBBY=0,
						 TRUCK_RECEIVED_ROBOT,
						 TRUCK_RECEIVED_HOUSE_PICTURE,
						 TRUCK_RECEIVED_ADDRESS,
						 TRUCKS_CARGO_REMOVED};

//const millisecond duration_between_trucks = 4000;
const millisecond wait_before_leaving = 3000; // house being built
const millisecond time_to_drive_one_block_above = 2000;
const millisecond time_to_drive_one_block_side = 8000;
const millisecond truck_disappear_duration = 2000;
const millisecond time_to_get_to_street = 1000;
const millisecond grow_full_size_duration = 10000;

const city_coordinate truck_height = tt_block_height/2; // const restored on 080705
const city_coordinate truck_width = truck_height;
const city_coordinate north_south_truck_width = (truck_width*3)/2;
const city_coordinate north_south_truck_height = (3*truck_height)/4; // empirical

// moved here on 080705 so shared (and made const)
const CargoType cargo_types[cargo_type_count] = {TRUCK_ROBOT,TRUCK_CUBBY,TRUCK_HOUSE,TRUCK_AVENUE,TRUCK_STREET,TRUCK_TEXT,TRUCK_NOTEBOOK};

boolean robot_loaded_in_truck() {
	return((boolean) (tt_histories[TRUCK]&TRUCK_RECEIVED_ROBOT));
};

Truck::Truck(SpriteCode code, city_coordinate x, city_coordinate y,
//				 Background *floor,
				 city_coordinate width,
				 city_coordinate height,
				 Robot *robot,
				 Cubby *cubby,
             Text *text,
             Notebook *notebook,
             Sprite *by) : // new on 171102 to replace saved_action_status
//				 ActionStatus *saved_action_status) :
  Sprite(code,x,y,TRUCK,0,ground_scale,width,height),
  cubby(cubby),
  robot(robot),
  text(text),
  notebook(notebook) {  // if no notebook should get main one for tt_toolbox
	if (notebook != NULL) {
		notebook->increment_ref_count(); // new on 230305
	};
  set_saved_by(by);
//  saved_action_status(saved_action_status) {
//#if TT_DEBUG_ON
//   if (tt_debug_mode == 18229 && cubby != NULL && cubby != NULL) {  // tracking down cubby with destroyed floor
//      tt_error_file() << cubby << " truck created with cubby whose floor is " << cubby->pointer_to_background() << endl;
//   };
//#endif
//tt_error_file() << "Truck#" << return_debug_counter() << " " << tt_frame_number << endl;
};

//void Truck::prepare_for_deletion(Sprite *by) {
 // commented out since are followers...
 //  if (text != NULL) {
 //     text->destroy();
 //     text = NULL;
 //  };
 //  if (notebook != NULL) {
 //     notebook->destroy();
 //     notebook = NULL;
 //  };
//   Sprite::prepare_for_deletion(by);
//};

boolean Truck::set_cargo(CargoType type, Sprite *item) { // moved these up from Truck_Inside on 231202
	// returns TRUE unless type can't be handled
   if (item == NULL) return(TRUE);
   switch (type) {
		case TRUCK_ROBOT:
			if (item->kind_of() == ROBOT) {
            robot = (Robot *) item;
         } else {
#if TT_DEBUG_ON
            say_error(_T("Internal problem re-creating a truck."),TRUE);
#endif
         };
			return(TRUE);
		case TRUCK_CUBBY:
			if (item->kind_of() == CUBBY) {
            cubby = (Cubby *) item;
         } else {
#if TT_DEBUG_ON
            say_error(_T("Internal problem re-creating a truck."),TRUE);
#endif
         };
			return(TRUE);
		case TRUCK_TEXT:
			if (item->kind_of() == TEXT) {
            text = (Text *) item;
         } else {
#if TT_DEBUG_ON
            say_error(_T("Internal problem re-creating a truck."),TRUE);
#endif
         };
			return(TRUE);
      case TRUCK_NOTEBOOK:
			if (item->kind_of() == PROGRAM_PAD) {
            notebook = (Notebook *) item;
				if (notebook != NULL) { // new on 230305 since otherwise for examples like Sentence Generator can have too low a ref count
					notebook->increment_ref_count();
				};
         } else {
#if TT_DEBUG_ON
            say_error(_T("Internal problem re-creating a truck."),TRUE);
#endif
         };
			return(TRUE);
		default:
			return(FALSE);
	};
};

void Truck::fill_house(House *new_house) {
   if (notebook != NULL) {
      remove_follower(notebook);
      robot->set_team_notebook(notebook);
   };
	robot->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL);
	robot->move_to(2*tile_width,12*tile_height);
	cubby->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL);
	cubby->move_to(2*tile_width,10*tile_height);
	cubby->recursively_propagate_changes();
	cubby->set_selectable(TRUE);
	robot->set_selectable(TRUE);
	if (robot->pointer_to_leader() != NULL) {
		robot->pointer_to_leader()->remove_follower(robot);
	};
	if (cubby->pointer_to_leader() != NULL) {
		cubby->pointer_to_leader()->remove_follower(cubby);
	};	
	// following are no-ops as of 070105 if the above ran remove_follower
	robot->remove_from_floor(FALSE,TRUE); // new on 201103 since can happen if robot puts main box in a truck
	cubby->remove_from_floor(FALSE,TRUE);
	new_house->initial_contents(robot,cubby);
//tt_error_file() << "Truck " << this << " initializing house. f=" << tt_frame_number << endl;
//#if TT_DEBUG_ON
//   if (tt_debug_mode == 18229 && cubby != NULL) {  // tracking down cubby with destroyed floor
//      tt_error_file() << cubby << " truck initialized house " << new_house << " with cubby whose floor is " << cubby->pointer_to_background() << endl;
//   };
//#endif
	robot = NULL;
	cubby = NULL;
   if (text != NULL) {
      if (text->pointer_to_leader() == this) remove_follower(text);
      text->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL);
     	text->move_to(6*tile_width,6*tile_height);
      text->set_selectable(TRUE);
      new_house->pointer_to_floor()->add_item(text);
      text = NULL;
   };
   if (notebook != NULL) {
      notebook->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL);
     	notebook->move_to(10*tile_width,2*tile_height);
      notebook->set_selectable(TRUE);
		if (notebook->pointer_to_leader() == this) remove_follower(notebook); // was unconditional and commented out
      new_house->pointer_to_floor()->add_item(notebook);
      // house gets the ref count and this truck loses it
      notebook->destroy(); // if zero ref count
#if TT_DEBUG_ON
      if (tt_debug_mode == 1717) {
         tt_error_file() << "Ref count of truck pad#" << notebook->return_debug_counter() << " is " <<  notebook->return_ref_count() << endl;
      };
#endif
      notebook = NULL;
   };
   action_completed();
};

#if TT_XML

void Truck::xml(xml_element *element, xml_document *document) {
	// moved here from Truck_Inside on 231202
	// no need for independence checks below, right?? asked 100703
   xml_element *cargo;
#if TT_DEBUG_ON
	int ref_count; // for debugging XML ref counts
#endif
   if (cubby != NULL) {
      cargo = cubby->xml_create_and_append_element(element,document);
      if (cargo != NULL) {
         xml_set_attribute(cargo,L"CargoCode",TRUCK_CUBBY);
#if TT_DEBUG_ON
			ref_count = // for debugging XML ref counts
#endif
			cargo->Release(); // new on 200804
#if TT_DEBUG_ON
			debug_ref_count(ref_count,cargo);
#endif
      };
   };
   if (robot != NULL) {
      cargo = robot->xml_create_and_append_element(element,document);
      if (cargo != NULL) {
         xml_set_attribute(cargo,L"CargoCode",TRUCK_ROBOT);
#if TT_DEBUG_ON
			ref_count = // for debugging XML ref counts
#endif
			cargo->Release(); // new on 200804
#if TT_DEBUG_ON
			debug_ref_count(ref_count,cargo);
#endif
      };
   };
	if (text != NULL) {
      // what if picture indirection?
      cargo = text->xml_create_and_append_element(element,document);
      if (cargo != NULL) {
         xml_set_attribute(cargo,L"CargoCode",TRUCK_TEXT);
#if TT_DEBUG_ON
			ref_count = // for debugging XML ref counts
#endif
			cargo->Release(); // new on 200804
#if TT_DEBUG_ON
			debug_ref_count(ref_count,cargo);
#endif
      };
   };
   if (notebook != NULL) {
      cargo = notebook->xml_create_and_append_element(element,document);
      if (cargo != NULL) {
         xml_set_attribute(cargo,L"CargoCode",TRUCK_NOTEBOOK);
#if TT_DEBUG_ON
			ref_count = // for debugging XML ref counts
#endif
			cargo->Release(); // new on 200804
#if TT_DEBUG_ON
			debug_ref_count(ref_count,cargo);
#endif
      };
   };
	Sprite::xml(element,document);
};

boolean Truck::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
      case TEXT_PAD_TAG:
      case BOX_TAG:
      case NOTEBOOK_TAG:
      case ROBOT_TAG: {
         Sprite *cargo = xml_load_sprite(node,tag);
         if (cargo != NULL) { // else warn??
            CargoType type = (CargoType) xml_get_attribute_int(node,L"CargoCode",TRUCK_UNKNOWN);
            cargo->set_priority(priority()-type); // so all items are on top of truck and at different levels
            set_cargo(type,cargo);
         };
         return(TRUE);
                      };
      default:
         return(Sprite::handle_xml_tag(tag,node));
   };
};

#endif

Truck_Inside::Truck_Inside(city_coordinate x, city_coordinate y,
				               long initial_priority,
				               city_coordinate width,
				               city_coordinate height,
				               Robot *robot,
				               Cubby *cubby,
                           Text *text,
                           Notebook *notebook,
                           Remote_Address *avenue,
                           Remote_Address *street,
                           Picture *house_picture) :
  Truck(TRUCK_INSIDE_SPRITE,x,y,width,height,robot,cubby,text,notebook), // no action_status so should it really be part of Truck?? - asked on 171102
	avenue(avenue),
   street(street),
	house_picture(house_picture),
	adding(NULL) {
//   move_after_receiving_item(FALSE) {
	set_priority(initial_priority);
//	set_priority_function_of_lly(FALSE);
	if (cubby != NULL) {
		add_follower(cubby,FALSE,INSERT_AT_END,TRUE); // not clear if INSERT_BY_PRIORITY would be better
//#if TT_DEBUG_ON
//      if (tt_debug_mode == 18229 && cubby != NULL) {  // tracking down cubby with destroyed floor
//         tt_error_file() << cubby << " truck added follower of a cubby whose floor is " << cubby->pointer_to_background() << endl;
//      };
//#endif
	};
	if (robot != NULL) {
		add_follower(robot,FALSE,INSERT_BY_PRIORITY,TRUE);
	};
   if (text != NULL) {
		add_follower(text,FALSE,INSERT_AT_END,TRUE);
	};
   if (notebook != NULL) {
		add_follower(notebook,FALSE,INSERT_AT_END,TRUE);
	};
   if (avenue != NULL) {
      add_follower(avenue,FALSE,INSERT_AT_END,TRUE);
   };
   if (street != NULL) {
      add_follower(street,FALSE,INSERT_AT_END,TRUE);
   };
   if (house_picture != NULL) {
      add_follower(house_picture,FALSE,INSERT_AT_END,TRUE);
   };
	// commented out on 210205 since this made robots stick up beyond the page when a truck is in the notebook
//	set_size_is_true(TRUE); // so animates better when dropped into boxes etc -- or is the problem that it was OK but saved wrong? -- 040105
};

//void Truck_Inside::prepare_for_deletion(Sprite *by) { // commented out on 191102 since now done by Sprite::prepare_for_deletion
//   if (prepared_for_deletion()) return; // already done
//   if (saved_by != NULL && !tt_shutting_down) saved_by->destroy(); // if zero ref count
//	Truck::prepare_for_deletion(by);
//};

/*
Truck_Inside::~Truck() {
	if (cubby != NULL && cubby->pointer_to_leader() == this) {
		// animation may be in progress that will add cubby as follower
		remove_follower(cubby,floor);
		delete cubby;
	};
	if (robot != NULL && robot->pointer_to_leader() == this) {
		remove_follower(robot,floor);
		delete robot;
	};
};
*/

Sprite *Truck_Inside::copy(boolean also_copy_offsets) {
	boolean ok_to_copy_any_followers = (!tt_copying_entire_thought_bubble || !blank || tt_copy_restores_pre_blank || !tt_remove_unneeded_things_from_thought_bubble); // new on 100105
	// !tt_remove_unneeded_things_from_thought_bubble new on 110105
	Cubby *cubby_copy = NULL;
	if (cubby != NULL && ok_to_copy_any_followers) { // second arg added on 100105 to lower the cost of thought bubbles
		cubby_copy = (Cubby *) (cubby->copy(TRUE));
		if (tt_copy_restores_pre_blank) { // new on 030105
			cubby_copy->recursively_set_visible(TRUE);
		};
	};
	Robot *robot_copy = NULL;
	if (robot != NULL && ok_to_copy_any_followers) { // second arg added on 100105 to lower the cost of thought bubbles
		robot_copy = (Robot *) robot->copy(TRUE);
		if (tt_copy_restores_pre_blank) { // new on 030105
			robot_copy->recursively_set_visible(TRUE);
		};
	};
   Text *text_copy = NULL;
   if (text != NULL && ok_to_copy_any_followers) { // second arg added on 100105 to lower the cost of thought bubbles
      text_copy = (Text *) text->copy(TRUE);
		if (tt_copy_restores_pre_blank) { // new on 030105
			text_copy->recursively_set_visible(TRUE);
		};
   };
   Remote_Address *avenue_copy = NULL;
   if (avenue != NULL && ok_to_copy_any_followers) { // second arg added on 100105 to lower the cost of thought bubbles
      avenue_copy = (Remote_Address *) avenue->copy(TRUE);
		if (tt_copy_restores_pre_blank) { // new on 030105
			avenue_copy->recursively_set_visible(TRUE);
		};
   };
   Remote_Address *street_copy = NULL;
   if (street != NULL && ok_to_copy_any_followers) { // second arg added on 100105 to lower the cost of thought bubbles
      street_copy = (Remote_Address *) street->copy(TRUE);
		if (tt_copy_restores_pre_blank) { // new on 030105
			street_copy->recursively_set_visible(TRUE);
		};
   };
   Picture *house_picture_copy = NULL;
   if (house_picture != NULL && ok_to_copy_any_followers) { // second arg added on 100105 to lower the cost of thought bubbles
      house_picture_copy = (Picture *) house_picture->copy(TRUE);
		if (tt_copy_restores_pre_blank) { // new on 030105
			house_picture->recursively_set_visible(TRUE);
		};
   };
   Notebook *notebook_copy = NULL;
   if (notebook != NULL && ok_to_copy_any_followers) { // second arg added on 100105 to lower the cost of thought bubbles
      notebook_copy = (Notebook *) notebook->copy(TRUE);
		if (tt_copy_restores_pre_blank) { // new on 030105
			notebook_copy->recursively_set_visible(TRUE);
		};
   };
	Truck *copy = new Truck_Inside(llx,lly, //center_x,center_y,
											 current_priority-1,
									       width,height,
                                  robot_copy,cubby_copy,text_copy,notebook_copy,
                                  avenue_copy,street_copy,house_picture_copy);
// 	if (saved_width != 0) {
//		copy->set_saved_size(saved_width,saved_height);
//	};
 //  copy->set_priority_fixed(priority_fixed_flag);
	//if (comment != NULL) {
	//	copy->set_comment(comment->copy());
	//};
	// last arg added on 100105 -- avoids a very heavy overhead occassionally for things in thought bubbles
   finish_making_copy(copy,also_copy_offsets,ok_to_copy_any_followers);
	return(copy);
};

boolean Truck_Inside::dump(output_stream &stream, boolean just_prepare, boolean , boolean ) {
   if (just_prepare) {
      if (cubby != NULL && robot != NULL) { // if in the midst of driving finish before dumping
			finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
		};	
		return(FALSE);
   };
   int dump_index = about_to_dump(this,stream); // rewritten on 020802
	if (dump_index < 0) return(FALSE); // new on 051099
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
		print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
		tt_error_file() << "About to dump ";
		print(tt_error_file());
		tt_error_file() << endl;
		tt_debug_dump_depth++;
	};
#endif
   stream.put((unsigned char) sprite_type);
	dump_if_not_null(stream,comment);
//   stream.put((unsigned char) 7); // there are now 7 kinds of things in trucks
   if (cubby != NULL) {
      stream.put((unsigned char) TRUCK_CUBBY);
      cubby->dump(stream);
   };
   if (robot != NULL) {
      stream.put((unsigned char) TRUCK_ROBOT);
      robot->dump(stream);
   };
   if (avenue != NULL) {
      stream.put((unsigned char) TRUCK_AVENUE);
      avenue->dump(stream);
   };
   if (street != NULL) {
      stream.put((unsigned char) TRUCK_STREET);
      street->dump(stream);
   };
   if (house_picture != NULL) {
      stream.put((unsigned char) TRUCK_HOUSE);
      house_picture->dump(stream);
   };
   if (text != NULL) {
      stream.put((unsigned char) TRUCK_TEXT);
      text->dump(stream);
   };
   if (notebook != NULL) {
      stream.put((unsigned char) TRUCK_NOTEBOOK);
      notebook->dump(stream);
   };
   stream.put((unsigned char) TRUCK_UNKNOWN);
#if TT_DEBUG_ON
	if (tt_debug_mode == 160299 && !tt_dumping_to_test_equality_or_describe) {
		print_spaces(min(tt_debug_dump_depth,max_debug_depth),tt_error_file());
		tt_error_file() << "Finished dumping ";
		print(tt_error_file());
		tt_error_file() << endl;
		tt_debug_dump_depth--;
	};
#endif
	return(TRUE);
};

#if TT_XML
//
//xml_element *Truck_Inside::xml_create_element(xml_document *document) {
//   return(create_xml_element(L"Truck",document));
//};

void Truck_Inside::xml(xml_element *element, xml_document *document) {
   xml_element *cargo;
   if (avenue != NULL) {
      cargo = avenue->xml_create_and_append_element(element,document);
      if (cargo != NULL) {
         xml_set_attribute(cargo,L"CargoCode",TRUCK_AVENUE);
      };
   };
   if (street != NULL) {
      cargo = street->xml_create_and_append_element(element,document);
      if (cargo != NULL) {
         xml_set_attribute(cargo,L"CargoCode",TRUCK_STREET);
      };
   };
   if (house_picture != NULL) {
      cargo = house_picture->xml_create_and_append_element(element,document);
      if (cargo != NULL) {
         xml_set_attribute(cargo,L"CargoCode",TRUCK_HOUSE);
      };
   };
   Truck::xml(element,document);
};

void Truck_Inside::add_special_xml(xml_element *element, xml_document *document) {
	if (adding != NULL) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	adding->check_for_independence();
		//};
		adding->add_xml(element,L"BeingAdded",document);
	};
	Truck::add_special_xml(element,document); // new on 121103 since otherwise animation isn't saved
};

boolean Truck_Inside::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
      case PICTURE_TAG:
      case USER_PICTURE_TAG: 
      case PICTURE_OF_TAG:
      case REMOTE_CONTROL_TAG: {
         Sprite *cargo = xml_load_sprite(node,tag);
         if (cargo != NULL) { // else warn??
            CargoType type = (CargoType) xml_get_attribute_int(node,L"CargoCode",TRUCK_UNKNOWN);
            cargo->set_priority(priority()-type); // so all items are on top of truck and at different levels
            set_cargo(type,cargo);
         };
         return(TRUE);
                      };
		case BEING_ADDED_TAG: // new on 231202
			if (adding != NULL) { // new on 070103
				adding->destroy();
			};
			adding = xml_get_sprite(node);
			return(TRUE);
      default:
         return(Truck::handle_xml_tag(tag,node));
   };
};

#endif

void Truck_Inside::become_blank(boolean by_user, boolean controlee_too) {
	// new on 030105 since its followers are out side and displayed by Screen not as parts
	Sprites *remaining = followers;
	while (remaining != NULL) {
		remaining->first()->recursively_set_visible(FALSE);
		remaining = remaining->rest();
	};
	Truck::become_blank(by_user,controlee_too);
};

void Truck_Inside::become_non_blank(boolean controlee_too) {
	// new on 030105 since its followers are out side and displayed by Screen not as parts
	// I suppose a truck might have some invisible pictures that become visible hereby
	Sprites *remaining = followers;
	while (remaining != NULL) {
		remaining->first()->recursively_set_visible(TRUE);
		remaining = remaining->rest();
	};
	Truck::become_non_blank(controlee_too);
};

boolean Truck_Inside::equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, SpritesPointer &expected_corresponding_nests, EqualReason reason) { // new on 060601
	if (other == this) return(TRUE);
	if (other->kind_of() != kind_of()) return(FALSE);
	if (is_blank() != other->is_blank()) return(FALSE);
	Truck_Inside *other_truck = (Truck_Inside *) other;
	if (cargo(TRUCK_ROBOT) != NULL) {
		if (other_truck->cargo(TRUCK_ROBOT) != NULL) {
			if (!cargo(TRUCK_ROBOT)->equal_to(other_truck->cargo(TRUCK_ROBOT),pending,corresponding_nests,expected_corresponding_nests,reason)) return(FALSE);
		};
	} else if (other_truck->cargo(TRUCK_ROBOT) != NULL) { // was == prior to 041201
		return(FALSE);
	};
	if (cargo(TRUCK_CUBBY) != NULL) {
		if (other_truck->cargo(TRUCK_CUBBY) != NULL) {
			if (!cargo(TRUCK_CUBBY)->equal_to(other_truck->cargo(TRUCK_CUBBY),pending,corresponding_nests,expected_corresponding_nests,reason)) return(FALSE);
		};
	} else if (other_truck->cargo(TRUCK_CUBBY) != NULL) { // was == prior to 041201
		return(FALSE);
	};
	if (cargo(TRUCK_HOUSE) != NULL) {
		if (other_truck->cargo(TRUCK_HOUSE) != NULL) {
			if (!cargo(TRUCK_HOUSE)->equal_to(other_truck->cargo(TRUCK_HOUSE),pending,corresponding_nests,expected_corresponding_nests,reason)) return(FALSE);
		};
	} else if (other_truck->cargo(TRUCK_HOUSE) != NULL) { // was == prior to 041201
		return(FALSE);
	};
	if (cargo(TRUCK_AVENUE) != NULL) {
		if (other_truck->cargo(TRUCK_AVENUE) != NULL) {
			if (!cargo(TRUCK_AVENUE)->equal_to(other_truck->cargo(TRUCK_AVENUE),pending,corresponding_nests,expected_corresponding_nests,reason)) return(FALSE);
		};
	} else if (other_truck->cargo(TRUCK_AVENUE) != NULL) { // was == prior to 041201
		return(FALSE);
	};
	if (cargo(TRUCK_STREET) != NULL) {
		if (other_truck->cargo(TRUCK_STREET) != NULL) { 
			if (!cargo(TRUCK_STREET)->equal_to(other_truck->cargo(TRUCK_STREET),pending,corresponding_nests,expected_corresponding_nests,reason)) return(FALSE);
		};
	} else if (other_truck->cargo(TRUCK_STREET) != NULL) { // was == prior to 041201
		return(FALSE);
	};
	if (cargo(TRUCK_TEXT) != NULL) {
		if (other_truck->cargo(TRUCK_TEXT) != NULL) {
			if (!cargo(TRUCK_TEXT)->equal_to(other_truck->cargo(TRUCK_TEXT),pending,corresponding_nests,expected_corresponding_nests,reason)) return(FALSE);
		};
	} else if (other_truck->cargo(TRUCK_TEXT) != NULL) { // was == prior to 041201
		return(FALSE);
	};
   if (cargo(TRUCK_NOTEBOOK) != NULL) {
		if (other_truck->cargo(TRUCK_NOTEBOOK) != NULL) {
			if (!cargo(TRUCK_NOTEBOOK)->equal_to(other_truck->cargo(TRUCK_NOTEBOOK),pending,corresponding_nests,expected_corresponding_nests,reason)) return(FALSE);
		};
	} else if (other_truck->cargo(TRUCK_NOTEBOOK) != NULL) { // was == prior to 041201
		return(FALSE);
	};
   return(TRUE);	
};


Truck *load_truck(InputStream *pad_in, 
                  int notebook_version, NaturalLanguage language,
                  Sprite *container, Picture *controlee, long index, boolean descendants_inactive) {
   Truck_Inside *truck = new Truck_Inside(0,0,0);
   if (container != NULL) {  // new on 160702
      if (container->kind_of() == PROGRAM_PAD) {
         truck->set_in_notebook((Notebook *) container,FALSE); // no point recuring
      } else if (container->kind_of() == VACUUM) {
         truck->set_inside_vacuum(TRUE,FALSE);
      };
   }; 
	associate_shared_item(index,truck);
	if (notebook_version >= first_version_to_save_copied_nests) {
		truck->set_comment(load_item(pad_in,notebook_version,language));
	};
//   if (notebook_version < 6) {
      // empty trucks probably had null objects inside
//      unsigned char type = pad_in->get();
//      while (type != 240) {
//         type = pad_in->get();
//      };
//      Sprite *x = load_item(
//      return(truck);
//   };
   CargoType type = (CargoType) pad_in->get();
   while (type != TRUCK_UNKNOWN && !pad_in->empty()) {
      Sprite *cargo = load_item(pad_in,notebook_version,language,container,controlee,-1,FALSE,FALSE,descendants_inactive);
      if (cargo == NULL) {
			log("Something went wrong while recreating a saved truck. Sorry.",FALSE,TRUE); // new on 100500
         return(truck); // problems -- shouldn't this throw instead??
      };
		cargo->set_priority(truck->priority()-type); // so all items are on top of truck and at different levels
      truck->set_cargo(type,cargo);
      type = (CargoType) pad_in->get();
   };
   return(truck);
};

boolean describe_truck(InputStream *stream, output_stream &text_out, int version_number, Article article) {
	// this was missing prior to 130200
	print_type(TRUCK,text_out,article);
	if (version_number >= first_version_to_save_copied_nests) {
		describe_comment_encoding(stream,text_out,version_number,PROGRAM_PAD);
	};
	CargoType type = (CargoType) stream->get();
	boolean first_time = TRUE;
   while (type != TRUCK_UNKNOWN && !stream->empty()) {
		if (first_time) {
			text_out << space() << S(IDS_CONTAINING) << space();
			first_time = FALSE;
		} else {
			text_out << space() << S(IDS_AND) << space();
		};
      if (!describe_item(stream,text_out,version_number)) {
			text_out << "???";
			return(FALSE);
      };
      type = (CargoType) stream->get();
   };
   return(TRUE);
};

string Truck_Inside::java_start(Cubby *, Text *, Notebook *, Picture *, Sprites *, Sprites *, Sprite *, java_string extra_text) {
   if (robot != NULL) {
      return(robot->java_start(cubby,text,notebook));
	} else {
		return(NULL);
   };
};

void Truck_Inside::java_before(java_stream &out) {
   if (robot != NULL) {
      robot->java_before(out);
   };
	if (cubby != NULL) { // new on 180201 - what about the rest of the cargo??
      cubby->java_before(out);
   };
};

void Truck_Inside::java_class(java_stream &out) {
   if (robot != NULL) {
      robot->java_class(out);
   };
};

void Truck_Inside::java_after(java_stream &out, const_java_string variable_temp) { // was missing prior to 080205
   if (robot != NULL) {
      robot->java_after(out,variable_temp);
   };
	if (cubby != NULL) { 
      cubby->java_after(out,variable_temp);
   };
};

void Truck_Inside::java(java_stream &out) {
	string by;
	if (tt_creating_java_for_an_event) { 
		by = "this";
	} else {
		by = "null";
	};
	if (is_blank() || contained_in(THOUGHT_BUBBLE)) {
		// new on 070205 to treat any truck as if blank if in thought bubble -- also if contained a robot this generated Java that
		// referenced the robot but didn't bind it
		java_new(out,S(IDS_JAVA_TT_TRUCK),(Sprite *) NULL,NULL,NULL,NULL,NULL,NULL,NULL,by); // "this" added on 150201 - updated to "by" on 180201
	} else {
//		if (robot != NULL) { // new on 080205
//         robot->java_team(out,robot->pointer_to_working_cubby());
//		};
		java_new(out,S(IDS_JAVA_TT_TRUCK),cubby,robot,house_picture,avenue,street,text,notebook,by); // "this" added on 150201 - updated to "by" on 180201
	};
};

void Truck_Inside::set_priority(long new_priority) {
//	long change = new_priority-current_priority;
	Sprite::set_priority(new_priority);
	Sprites *remaining = followers;
	while (remaining != NULL) {
//		remaining->first()->increment_priority(change);
		Sprite *sprite = remaining->first();
		sprite->set_priority(new_priority-sprite->kind_of()); // new on 291199 so order is fixed
		remaining = remaining->rest();
	};
//   if (robot != NULL) {
//		robot->set_priority(current_priority-2);
//	};
//	if (cubby != NULL) {
//		cubby->set_priority(current_priority-1);
//	};
};

ReleaseStatus Truck_Inside::item_released_on_top(Sprite *item, Sprite *by,
														       SpritePointer &recipient, int level) {
	if (infinite_stack() && !tt_copy_top_of_stack && !by->still_running()) {
		by->action_aborted();
		return(RELEASE_IGNORED);
	};
//   if (contained_in(CUBBY)) {
      // this isn't right!!!
//		return(item_released_on_top(item,by,recipient,level,action_status));
//   };
	switch (item->kind_of()) {
//		case TRUCK:
//			if (infinite_stack() && ((Truck_Inside *) item)->empty()) {
//				return(RELEASED_ON_ITS_STACK);
//			};
//			return(RELEASE_IGNORED);
		case ROBOT:
//           if (!((Robot *) item)->fully_defined()) {
//              return(RELEASE_IGNORED);
//           }; // otherwise do the following
			if (robot != NULL) { // already has one
				by->abort_action_if_robot(IDS_TRUCK_ALREADY_HAS_ROBOTS);
				return(RELEASE_IGNORED);
			};
			user_did(TRUCK,TRUCK_RECEIVED_ROBOT,by);
		case CUBBY:
			if (cubby != NULL && item->kind_of() != ROBOT) {
				by->abort_action_if_robot(IDS_TRUCK_ALREADY_HAS_BOX);
				return(RELEASE_IGNORED);
			};
			if (infinite_stack()) {
				if (!tt_copy_top_of_stack && !by->still_running()) {
					by->action_aborted();
					return(RELEASE_IGNORED);
				};
//				Background *floor = item->pointer_to_background();
//  				copy = (Truck *) this->copy(floor);
//				if (floor != NULL) {
//					floor->add_item(copy);
//					item->set_priority(copy->priority()-1);
//					record_event(NEW_ITEM,by,floor,copy,TRUE);
//				};
				recipient = copy_top_of_stack(by);
				UnwindProtect<boolean> set(tt_recording_off,TRUE);
				recipient->item_released_on_top(item,by,recipient,level);
				record_event(DROP_ITEM_IN_TRUCK,by,floor,this);
				return(RELEASE_HANDLED);
			};
			if (item->kind_of() == CUBBY) {
				user_did(TRUCK,TRUCK_RECEIVED_CUBBY,by);
				if (item->is_main_cubby_in_body()) { // removed coercion of item to Cubby on 060705
					if (!told_user(TOLD_MAIN_CUBBY_IN_TRUCK_NO_LOOP)) {
						say(IDS_MAIN_CUBBY_IN_TRUCK_NO_LOOP);
						user_told(TOLD_MAIN_CUBBY_IN_TRUCK_NO_LOOP);
					};
					floor->need_new_main_cubby();	// new on 080705 since if robot sent away top_cubby then to update things 
				};
			};
/*
			  copy->set_robot((Robot *) item); // a temporary lie
			  item->set_priority(floor->next_item_priority());
			  copy->animate_location(llx+tile_width*8,lly,duration,NULL,
											 copy,&receive_item_callback);
			  return(RELEASED_IN_TRUCK);
*/
//			set_move_after_receiving_item(TRUE);
			if (receive_item(item,by,default_duration(),this,item)) { // conditionalized on 010799
				return(RELEASED_IN_TRUCK);
			} else {
				return(RELEASE_IGNORED);
			};
		case REMOTE_PICTURE:
			if (((Remote_Picture *) item)->is_remote_address()) {
				if (receive_item(item,by,default_duration(),this,item)) { // conditionalized on 010799
					user_did(TRUCK,TRUCK_RECEIVED_ADDRESS,by);
					record_event(DROP_ITEM_IN_TRUCK,by,floor,this);
					return(RELEASE_HANDLED);
				} else {
					return(RELEASE_IGNORED);
				};
			} else break;
		case PICTURE:
			if (house_style(((Picture *) item)->appearance_code()) != NOT_HOUSE) {
				// starting 291199 check if there already is a picture first
				if (house_picture == NULL && receive_item(item,by,default_duration(),this,item)) { // conditionalized on 010799
					user_did(TRUCK,TRUCK_RECEIVED_HOUSE_PICTURE,by);
					record_event(DROP_ITEM_IN_TRUCK,by,floor,this);
					return(RELEASE_HANDLED);
				} else {
					return(RELEASE_IGNORED);
				};
			} else break;
      case TEXT:
			if (text == NULL && receive_item(item,by,default_duration(),this,item)) { // conditionalized on 010799
				// starting 291199 check if there already is a text pad first
				record_event(DROP_ITEM_IN_TRUCK,by,floor,this);
				return(RELEASE_HANDLED);
			} else {
				return(RELEASE_IGNORED);
			};
      case PROGRAM_PAD:
         if (notebook == NULL && receive_item(item,by,default_duration(),this,item)) { // conditionalized on 010799
				// starting 291199 check if there already is a notebook first
				record_event(DROP_ITEM_IN_TRUCK,by,floor,this);
				return(RELEASE_HANDLED);
			} else {
				return(RELEASE_IGNORED);
			};
	};
	if (cubby == NULL) {
		by->abort_action_if_robot(IDS_TRUCK_HELP,
										  NONE_GIVEN, // really TRUCK but shorter message this way
										  (!infinite_stack() && !item->is_tool() // !is_wand(item->kind_of()) // noticed this was obsolete on 220901
											&& !told_user(TOLD_WHAT_TRUCKS_ACCEPT)));
		user_told(TOLD_WHAT_TRUCKS_ACCEPT);
		return(RELEASE_IGNORED);
	} else {
		return(cubby->item_released_on_top(item,by,recipient,level));
	};
};

boolean Truck_Inside::receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item) {
   if (inside_vacuum()) {
      by->abort_action(IDS_ROBOT_MISSING_RECIPIENT_ABORT);
      return(FALSE);
   };
   if (floor->outside() && floor->pointer_to_picture() == NULL) {
      by->abort_action(IDS_TRUCKS_START_DRIVING_ONLY_INSIDE_OF_HOUSES_ABORT);
      return(FALSE);
   };
   set_saved_by(by);
//	saved_action_status = action_status;
	if (duration > 0) {
		tt_screen->add(item); // just in case -- or should robot do this??
		// might be a stack item animating
//		if (animation_in_progress()) { // conditional new on 121202 since otherwise set duration is set 0 and no nice animation happens
		// was a bad idea since it looked bad
			duration = animations_stop_time()-tt_current_time;
//		};
	};
	boolean followers_are_outside = (leader != NULL && leader->followers_completely_inside()); // new on 080705 -- make recursive predicate
	switch(item->kind_of()) {
		case ROBOT:
         if (robot != NULL) {
            by->abort_action_if_robot(IDS_TRUCK_ALREADY_HAS_ROBOTS);
            return(FALSE);
         };
			robot = (Robot *) item;	
			floor->remove_item(robot,followers_are_outside,followers_are_outside); // args were FALSE prior to 080705
		   robot->set_priority(priority()-2); 
         do_after(duration,this,ANIMATE_ROBOT_CALLBACK);
			return(TRUE);
		case CUBBY:
         if (cubby != NULL) {
            by->abort_action_if_robot(IDS_TRUCK_ALREADY_HAS_BOX);
            return(FALSE);
         };
			cubby = (Cubby *) item;
//#if TT_DEBUG_ON
//         if (tt_debug_mode == 18229 && cubby != NULL) {  // tracking down cubby with destroyed floor
//            tt_error_file() << cubby << " truck about to receive cubby whose floor is " << cubby->pointer_to_background() << endl;
//         };
//#endif
         floor->remove_item(cubby,followers_are_outside,followers_are_outside); // args were FALSE prior to 080705
//#if TT_DEBUG_ON
//        if (tt_debug_mode == 18229 && cubby != NULL) {  // tracking down cubby with destroyed floor
//           tt_error_file() << cubby << " truck received cubby whose floor is " << cubby->pointer_to_background() << endl;
//        };
//#endif
         cubby->set_priority(priority()-1);
         cubby->set_selectable(FALSE);
         do_after(duration,this,ANIMATE_CUBBY_CALLBACK);
			return(TRUE);
		 case PICTURE:
			if (adding == NULL) {
				if (house_style(((Picture *) item)->appearance_code()) == NOT_HOUSE) break;
				house_picture = (Picture *) item;
				start_adding(house_picture,duration);
				return(TRUE);
			};
			break;
       case TEXT:
			if (adding == NULL) {
				text = (Text *) item;
				start_adding(text,duration);
				return(TRUE);
			};
			break;
       case PROGRAM_PAD:
			if (adding == NULL) {
				notebook = (Notebook *) item;
            notebook->increment_ref_count(this);
				start_adding(notebook,duration);
				return(TRUE);
			};
			break;
		 case REMOTE_PICTURE: 
			 if (adding == NULL) {
				 Remote_Picture *remote_picture = (Remote_Picture *) item;
				 if (remote_picture->return_avenue_number() != -1 && avenue == NULL) {
					 avenue = (Remote_Address *) remote_picture;
					 start_adding(avenue,duration);
					 return(TRUE);
				 };
				 if (remote_picture->return_street_number() != -1 && street == NULL) {
					 street = (Remote_Address *) remote_picture;
					 start_adding(street,duration);
					 return(TRUE);
				 };
			 };
			 break;
	};
	by->abort_action(IDS_TRUCK_CANT_ACCEPT);
   return(FALSE);
};

void Truck_Inside::start_adding(Sprite *item, millisecond duration) {
	adding = item;
	adding->set_priority(priority()-1);
	boolean followers_are_outside = (leader != NULL && leader->followers_completely_inside()); 
	// above new on 080705 -- make recursive predicate or is this OK?
	floor->remove_item(adding,followers_are_outside,followers_are_outside); // args were FALSE prior to 080705
	adding->set_selectable(FALSE);
	do_after(duration,this,ANIMATE_ADDING_CALLBACK);
};

void Truck_Inside::animate_robot() {
//	if (robot->pointer_to_background() != floor) { // somehow they are different - conditional added on 100400
//		robot->remove_from_floor(FALSE,FALSE);
//		if (floor != NULL) floor->add_item(robot); // new on 100400
//	};
	add_follower(robot,FALSE);
	millisecond duration = robot->default_duration();
	if (duration > 0) {
		if (cubby == NULL) {
			play_sound(TRUCK_REV_SOUND);
		} else {
			play_sound(TRUCK_DRIVE_SOUND);
		};
	};
   robot->set_selectable(FALSE);
	city_coordinate robot_width,robot_height,robot_llx,robot_lly;
	size_and_location_of(robot,robot_width,robot_height,robot_llx,robot_lly);
	robot->animate_size_and_location(robot_width,robot_height,robot_llx,robot_lly,
												duration,
												this,ADD_ROBOT_CALLBACK);
};

void Truck_Inside::animate_cubby() {
	add_follower(cubby,FALSE);
//#if TT_DEBUG_ON
//   if (tt_debug_mode == 18229 && cubby != NULL) {  // tracking down cubby with destroyed floor
//      tt_error_file() << cubby << " truck animating cubby whose floor is " << cubby->pointer_to_background() << endl;
//   };
//#endif
	millisecond duration = default_duration();
	if (duration > 0) {
		if (robot == NULL) {
			play_sound(TRUCK_REV_SOUND);
		} else {
			play_sound(TRUCK_DRIVE_SOUND);
		};
	};
	cubby->set_selectable(FALSE);
	city_coordinate cubby_width,cubby_height,cubby_llx,cubby_lly;
	size_and_location_of(cubby,cubby_width,cubby_height,cubby_llx,cubby_lly);
	cubby->animate_size_and_location(cubby_width,cubby_height,cubby_llx,cubby_lly,
												duration,
												this,ADD_CUBBY_CALLBACK);
};

void Truck_Inside::animate_adding() {
	add_follower(adding,FALSE);
	millisecond duration = default_duration();
	if (duration > 0) {
		play_sound(TRUCK_DRIVE_SOUND);
	};
	city_coordinate cargo_width,cargo_height,cargo_llx,cargo_lly;
	size_and_location_of(adding,cargo_width,cargo_height,cargo_llx,cargo_lly);
	adding->animate_size_and_location(cargo_width,cargo_height,cargo_llx,cargo_lly,
												 duration,
												 this,ADD_ADDING_CALLBACK);
};

void Truck_Inside::shift_viewpoint(city_coordinate delta_x, city_coordinate delta_y) {
	// new on 020204 to shift animating followers as well (since they were added)
	for (int i = 0; i < cargo_type_count; i++) {
		Sprite *sprite = cargo((CargoType) i);
		if (sprite != NULL && sprite->animation_in_progress()) {
			sprite->shift_viewpoint(delta_x,delta_y);
		};
	};
	Truck::shift_viewpoint(delta_x,delta_y);
};

void Truck_Inside::correct_cargo_size_and_location(Sprite *cargo) {
	// new on 020999 to fix cargo size if messed up by scrolling or standing up or sitting
	city_coordinate cargo_width,cargo_height,cargo_llx,cargo_lly;
	size_and_location_of(cargo,cargo_width,cargo_height,cargo_llx,cargo_lly);
	cargo->set_size_and_location(cargo_width,cargo_height,cargo_llx,cargo_lly);
};

void Truck_Inside::size_and_location_of(Sprite *cargo,
													 city_coordinate &cargo_width,city_coordinate &cargo_height,
													 city_coordinate &cargo_llx, city_coordinate &cargo_lly) {
	switch (cargo->kind_of()) {
		case CUBBY:
			cargo_width = 2*width/3;
			cargo_height = height/3;
			cargo_llx = llx+width/3;
			cargo_lly = lly+height/2;
			break;
		case ROBOT:
			((Robot *) cargo)->compute_dimensions_to_fit(width/2,2*height/3,
																		cargo_width,cargo_height);
			cargo_llx = llx+(2*width)/5;
			cargo_lly = lly+(3*height)/5;
			break;
		default:
			cargo_width = width/3;
			cargo_height = height/3;
			cargo_llx = llx+2*width/5;
			cargo_lly = lly+height/2;
	};
};

void Truck_Inside::add_cubby() {
//	add_follower(cubby,floor);
//	floor->remove_item(cubby);
//	tt_screen->remove(cubby);
//	if (move_after_receiving_item) { // am on top of infinite stack of trucks
//		animate_location(llx-tile_width,lly,default_duration(500));
//		move_after_receiving_item = FALSE;
//	};
	if (cubby == NULL) { // new on 250804
		log("Truck lost track of a box that was dropping into it.",FALSE,TRUE);
		return;
	};
   if (robot != NULL && leader == NULL) {
		drive_away_and_build();
	} else {
		correct_cargo_size_and_location(cubby);
      millisecond duration = default_duration(500);
   	if (duration > 0 && leader == NULL) {
         animate_delta_location(-tile_width,0,duration);
		};
		cubby->set_selectable(TRUE);
      ::completed(saved_by);
//		cubby->set_priority(priority()-1);
	};
};

void Truck_Inside::add_adding() {
	correct_cargo_size_and_location(adding);
   millisecond duration = default_duration(500);
	if (duration > 0 && leader == NULL) {
      animate_delta_location(-tile_width,0,duration);
   };
	adding->set_selectable(TRUE);
   ::completed(saved_by);
//	adding->set_priority(priority()-1);
	adding = NULL;
};

void Truck_Inside::add_robot() {
//	robot->absorb_followers();
//	add_follower(robot,floor);
//	floor->remove_item(robot);
//	tt_screen->remove(robot);
//	if (move_after_receiving_item) { // am on top of infinite stack of trucks
//		animate_location(llx-tile_width,lly,default_duration(500));
//		move_after_receiving_item = FALSE;
//	};
	if (robot == NULL) { // happened while debugging -- may not happen "naturally" -- new on 250804
		log("Truck lost track of a robot that was dropping into it.",FALSE,TRUE);
		return;
	};
	if (cubby != NULL && leader == NULL) {
		drive_away_and_build();
	} else {
		correct_cargo_size_and_location(robot);
      millisecond duration = default_duration(500);
      if (duration > 0 && leader == NULL) {
         animate_delta_location(-tile_width,0,duration);
      };
		robot->set_selectable(TRUE);
      action_completed();
		//completed(saved_action_status);
		//saved_action_status = NULL;
//		robot->set_priority(priority()-1);
	};
};

Sprite *Truck_Inside::select(TTRegion *region, SelectionReason reason, Sprite *in_hand) {
	// the following was a bad idea (see issue 687) and was commented out on 080705
	// also it only worked one some conditions since if the cargo had been added by hand then it was a follower that the background knew about
	// since it could be outside the bounding box of the truck
   //if (contained_in(CUBBY)) {
   //   return(this);
   //};
	// harder to accidentally remove stuff from truck with hand than vacuum
	// following rewritten on 080705 to be consistent for all types of cargo
	for (int i = 0; i < cargo_type_count; i++) {
      Sprite *item = cargo(cargo_types[i]);
      if (item != NULL && item->selectable()) {
			TTRegion item_region;
			item->collision_region(item_region);
			if ((reason == MIGHT_USE_VACUUM && item_region.overlaps(region)) ||
				(reason == MIGHT_GRASP && item_region.contains(region))) {
				return(item);
			};
		};
	};
//	if (robot != NULL && robot->selectable()) {
//		TTRegion robot_region;
//		robot->collision_region(robot_region); // grasp used collision region -- why??
//		if ((reason == MIGHT_USE_VACUUM && robot_region.overlaps(region)) ||
//			 (reason == MIGHT_GRASP && robot_region.contains(region))) {
////		 ((reason == MIGHT_USE_VACUUM && region->overlaps(robot->active_region())) ||
////		  (reason == MIGHT_GRASP && robot->pointer_to_collision_region()->contains(region)))) {
//			return(robot);
//		};
//	};
//	// ditto for cubby
//	if (cubby != NULL && cubby->selectable()) {
//		TTRegion cubby_region;
//		cubby->collision_region(cubby_region);
//		if ((reason == MIGHT_USE_VACUUM && cubby_region.overlaps(region)) ||
//			 (reason == MIGHT_GRASP && cubby_region.contains(region))) {
//			return(cubby);
//		};
//	};
	if (reason == MIGHT_DROP_ITEM) {
		if (tt_log_version_number >= 38) { // new on 290204 to do this correctly
			if (acceptable(in_hand)) {
				return(this); // don't have it and want it
			} else {
				return(NULL);
			};
		} else {
			switch (in_hand->kind_of()) {
				case CUBBY: // what about dropping things on cubby in truck ??
				case ROBOT:
					return(this);
				default:
				return(NULL);
			};
		};
	};
	return(this);
};

void Truck_Inside::remove_selected_element(Sprite *element, SelectionReason reason, Sprite *by, boolean add_to_floor) {
	user_did(TRUCK,TRUCKS_CARGO_REMOVED,by);
	if (element == robot) {
		remove_item(robot,reason,add_to_floor);
		robot = NULL;
	} else if (element == cubby) {
		remove_item(cubby,reason,add_to_floor);
		cubby = NULL;
	} else if (element == house_picture) {
		remove_item(house_picture,reason,add_to_floor);
		house_picture = NULL;
	} else if (element == street) {
		remove_item(street,reason,add_to_floor);
		street = NULL;
	} else if (element == avenue) {
		remove_item(avenue,reason,add_to_floor);
		avenue = NULL;
	} else if (element == text) {
      remove_item(text,reason,add_to_floor);
      text = NULL;
   } else if (element == notebook) {
      remove_item(notebook,reason,add_to_floor);
      notebook = NULL;
   };
	// else warn??
};

void Truck_Inside::remove_item(Sprite *item, SelectionReason reason, boolean add_to_floor) {
	remove_follower(item);
	if (add_to_floor) {
		floor->add_item(item);
		if (reason == GRASPED) {
			item->animate_to_good_size(default_duration(),tt_log_version_number >= 64?tt_toolbox:NULL);
		};
	};
};

void Truck_Inside::now_on_floor(Background *floor, Sprite *by) {
   if (cubby != NULL) {
      cubby->released(by); // not quite true but will hatch birds, etc.
//#if TT_DEBUG_ON
//      if (tt_debug_mode == 18229 && cubby != NULL) {  // tracking down cubby with destroyed floor
//         tt_error_file() << cubby << " truck released on " << floor << " while cubby whose floor is " << cubby->pointer_to_background() << endl;
//      };
//#endif
   };
   if (robot != NULL && cubby != NULL) {
      if (floor->outside() && floor->pointer_to_picture() == NULL) {
         by->abort_action(IDS_TRUCKS_START_DRIVING_ONLY_INSIDE_OF_HOUSES_ABORT); // first arg (status_action) was NULL prior to 171102
      } else {
//         if (saved_by != by) {
//            if (saved_by != NULL) saved_by->destroy(); // if zero ref count
//            saved_by = by;
//            if (saved_by != NULL) saved_by->increment_ref_count(this);
//         };
         drive_away_and_build();
      };
   };
   Truck::now_on_floor(floor,by);
};

void Truck_Inside::drive_away_and_build() {
	if (cubby == NULL || robot == NULL || held() || floor == NULL) {
      // e.g., programmer took it out or has grabbed whole truck
//		completed(saved_action_status); // shouldn't it abort?
      action_completed();
//		saved_action_status = NULL;
		// added floor == NULL for robustness on 170205 (since if holding a loaded truck with power off this can be called)
		return;
	};
	if (cubby->is_busy()) { // e.g. bird is creating its nest
//		do_after(1000,this,drive_away_and_build_callback);
		cubby->run_when_non_busy(this,DRIVE_AWAY_AND_BUILD_CALLBACK);
		return;
	};
	if (!active()) { // new on 190205
		cubby->set_selectable(TRUE); // new on 220205
	   robot->set_selectable(TRUE); // new on 220205
		do_after(1,this,DRIVE_AWAY_AND_BUILD_CALLBACK);
		return;
	};
   if (tt_city->stopped() && tt_programmer->pointer_to_appearance()->kind_of() != ROBOT_IN_TRAINING) {
		// added conjunct on 120499 so that you can train a robot to load up a truck even with the power off
      cubby->set_selectable(TRUE);
	   robot->set_selectable(TRUE);
//      do_after(1000,this,drive_away_and_build_callback);
      do_when_not_city_stopped(this,DRIVE_AWAY_AND_BUILD_CALLBACK);
      return;
   };
	if (floor->inside_rule_body()) {
		set_slated_for_deletion(TRUE);
		robot->set_available_for_indexing(FALSE); // can't be used after this
		cubby->set_available_for_indexing(FALSE); // can't be used after this
	};
   set_selectable(FALSE); // whole truck
	set_parameter(1);
   // looked bad
//	set_priority(max_long); // underneath everything
   city_coordinate goal_x, goal_y;
   if (floor->in_the_room()) {
       goal_x = 0;
       goal_y = 4*tile_height; // about where the door is
   } else {
      goal_x = -width;
      goal_y = lly;
   };
	set_containable_by_background(FALSE); // can drive out the door
	animate_location(goal_x,goal_y,
						  default_duration(2000),
						  this,REPLACE_TRUCK_AND_BUILD_CALLBACK);
};

void Truck_Inside::replace_truck_and_build() {
   if (cubby == NULL || robot == NULL) { // taken out after started
      action_completed();
      return;
   };
   Picture *picture_back = NULL;
   if (floor != NULL) picture_back = floor->pointer_to_picture();
   Background *old_floor = floor;
	if (picture_back == NULL) remove_from_floor();
   if (!old_floor->inside_rule_body()) { // no need to really spawn it if defining a body
	   remove_follower(robot,FALSE);
	   remove_follower(cubby,FALSE);
//#if TT_DEBUG_ON
//      if (tt_debug_mode == 18229 && cubby != NULL) {  // tracking down cubby with destroyed floor
//         tt_error_file() << cubby << " truck removed follower cubby whose floor is " << cubby->pointer_to_background() << endl;
//      };
//#endif
      if (picture_back != NULL) { // running on back of a picture
         if (saved_by != NULL) {
           if (saved_by->still_running()) {
               Robot *old_owner = (Robot *) saved_by;
               if (old_owner->pointer_to_working_cubby() == cubby) {
                  old_owner->set_working_cubby(NULL);
						old_owner->set_remove_me_from_backside(TRUE);
               };
            };
         };
			if (avenue == NULL && street == NULL && house_picture == NULL) { 
				// conditional new on 010300 user hasn't indicated a house address so run on the back
            //saved_by->destroy(); // if ref count is zero -- commented out on 191102 since action_completed() takes care of this now
            //saved_by = NULL; // finished with it
				// new on 021204 to check if picture is flipped and on back of another picture
				Background *picture_background = picture_back->pointer_to_background();
				if (picture_background != NULL) {
					Picture *picture_background_picture = picture_background->pointer_to_picture();
					if (picture_background_picture != NULL) {
						picture_back = picture_background_picture;
					};
				};
				cubby->now_on_back_of(picture_back); // for remotes to hook up right
				floor->add_item(robot); // new on 110201
				floor->add_item(cubby);
				robot->try_clause(cubby);
            action_completed();
				return;
			} else {
				remove_from_floor();
			};
      };
		Room *room = old_floor->pointer_to_room();
		House *old_house = NULL;
      if (room != NULL) old_house = room->pointer_to_house();
  		Block *block = NULL;
      if (old_house != NULL) block = old_house->block_im_on();
		HouseStyle new_house_style;
		if (house_picture == NULL) {
         if (old_house == NULL) {
            new_house_style = HOUSE_A;
         } else {
			   new_house_style = old_house->house_style();
         };
		} else {
			new_house_style = house_style(house_picture->appearance_code());
		};
		block_number avenue_number, street_number;
		if (avenue != NULL) {
			avenue_number = (block_number) avenue->return_avenue_number();
      } else if (block == NULL) { // probably truck is leaving an exploding house...
         avenue_number = 0;
		} else {
			avenue_number = block->return_avenue_number();
		};
		if (street != NULL) {
			street_number = (block_number) street->return_street_number();
      } else if (block == NULL) {
         street_number = 0;
		} else {
			street_number = block->return_street_number();
		};
//		tt_error_file() << (int) avenue_number << "," << (int) street_number << " style= " << new_house_style << endl; 
		if (!tt_programmer->pointer_to_appearance()->inside_a_house() && !finishing_instantly()) {
			drive_outside_to(old_house,avenue_number,street_number,new_house_style);
			return;
		};
      House *new_house = tt_city->new_house_nearest(avenue_number,street_number,new_house_style);
      if (new_house != NULL) {
         fill_house(new_house);
			destroy(); // new on 161204 to fix a truck leak -- e.g. exploding houses
      } else if (saved_by != NULL) { // no room for any more houses
			if (tt_log_version_number >= 36) { 
				// new on 220204 to let trucks drive outside and keep trying even though the city is full
				drive_outside_to(old_house,avenue_number,street_number,new_house_style);
			} else { 
				saved_by->abort_action(IDS_TRUCK_STOPPED_BECAUSE_CITY_FULL,TRUCK);
			};
      };
	} else if (saved_by != NULL) { // condition new on 240603 - Yishay had a crash
		saved_by->completed();
	};
   set_saved_by(NULL); // new on 171102 for safety
};

CargoType Truck_Inside::cargo_type(Sprite *item) {
	if (item == robot) return(TRUCK_ROBOT);
	if (item == cubby) return(TRUCK_CUBBY);
	if (item == house_picture) return(TRUCK_HOUSE);
	if (item == avenue) return(TRUCK_AVENUE);
	if (item == street) return(TRUCK_STREET);
   if (item == text) return(TRUCK_TEXT);
   if (item == notebook) return(TRUCK_NOTEBOOK);
	return(TRUCK_UNKNOWN);
};

Sprite *Truck_Inside::cargo(CargoType type) {
	switch (type) {
		case TRUCK_ROBOT:
			return(robot);
		case TRUCK_CUBBY:
			return(cubby);
		case TRUCK_HOUSE:
			return(house_picture);
		case TRUCK_AVENUE:
			return(avenue);
		case TRUCK_STREET:
			return(street);
      case TRUCK_TEXT:
         return(text);
      case TRUCK_NOTEBOOK:
         return(notebook);
	};
	return(NULL);
};

boolean Truck_Inside::acceptable(Sprite *item) { // new on 290204
	switch(item->kind_of()) {
		case ROBOT:
         return(robot == NULL);
		case CUBBY:
         return(cubby == NULL);
		 case PICTURE:
			 return (house_picture == NULL && house_style(((Picture *) item)->appearance_code()) != NOT_HOUSE);
       case TEXT:
			 return(text == NULL);
       case PROGRAM_PAD:
			 return(notebook == NULL);
		 case REMOTE_PICTURE: 
			 return((avenue == NULL && ((Remote_Picture *) item)->return_avenue_number() != -1) ||
					  (street == NULL && ((Remote_Picture *) item)->return_street_number() != -1));
		 default:
			 return(FALSE);
	};
};

boolean Truck_Inside::set_cargo(CargoType type, Sprite *item) {
	// returns TRUE unless type can't be handled
   if (item == NULL) return(TRUE);
	if (!Truck::set_cargo(type,item)) {
		switch (type) {
			case TRUCK_HOUSE:
				if (item->kind_of() == PICTURE) {
					house_picture = (Picture *) item;
					break;
				} else {
#if TT_DEBUG_ON
					say_error(_T("Internal problem re-creating a truck."),TRUE);
#endif
					return(TRUE);
				};
			case TRUCK_AVENUE:
				if (item->kind_of() == REMOTE_PICTURE) {
					avenue = (Remote_Address *) item;
					break;
				} else {
#if TT_DEBUG_ON
					say_error(_T("Internal problem re-creating a truck."),TRUE);
#endif
					return(TRUE);
				};
			case TRUCK_STREET:
				if (item->kind_of() == REMOTE_PICTURE) {
					street = (Remote_Address *) item;
					break;
				} else {
#if TT_DEBUG_ON
					say_error(_T("Internal problem re-creating a truck."),TRUE);
#endif
					return(TRUE);
				};
			default:
#if TT_DEBUG_ON
				say_error(_T("Internal problem re-creating a truck."),TRUE);
#endif
				return(FALSE);
		};	
	};
   item->set_size_and_location(width/3,height/3,llx+width/2,lly+(2*height/3),TO_FIT_INSIDE);
   // don't know why 2/5 width and 1/2 height looked bad
   add_follower(item,FALSE);
	return(TRUE);
};

MatchStatus Truck_Inside::match(Truck_Inside *other_truck, SpritePointerPointer suspension_cell, int version) {
//   const int type_count = 7; // made a constant on 020204
	if (blank) return(MATCH_GOOD); // new on 110105
   SpriteType types[cargo_type_count] = {ROBOT,CUBBY,HOUSE,REMOTE_PICTURE,REMOTE_PICTURE,TEXT,PROGRAM_PAD};
   for (int i = 0; i < cargo_type_count; i++) {
      Sprite *item = cargo(cargo_types[i]);
      if (item != NULL) {
         Sprite *other = other_truck->cargo(cargo_types[i]);
         if (other == NULL) { // can't suspend on a truck cell can I?
            return(MATCH_SUSPENDED_ON_TRUCK);
         };
         MatchStatus submatch_status = same_type_match(types[i],item,other,suspension_cell,version);
         if (submatch_status != MATCH_GOOD) return(submatch_status);
       };
   };
   return(MATCH_GOOD);
};

Path *Truck_Inside::path_to(Sprite *target, boolean &ok) {
	CargoType type = cargo_type(target);
	if (type != TRUCK_UNKNOWN) {
		if (cargo(type) == target) { // new on 070799
			return(new Path(type)); // could update this to follow the new SPECIAL_PATH_MARKER scheme
		};
	};
	if (cargo(TRUCK_CUBBY) != NULL) { // new on 070799 so can train robots to add or remove things from the cubby in the truck 
		Path *path = cargo(TRUCK_CUBBY)->path_to(target,ok);
		if (ok) {
			return(new Path(TRUCK_CUBBY,path));
		};
	};
	ok = FALSE;
	return(NULL);
};

void Truck_Inside::generate_help(output_stream &message, Sprite *tool_in_hand,
                                 Sprite *selection) {
	if ((cubby == NULL && !did(TRUCK_RECEIVED_CUBBY)
		  && tt_help_counter == 0) || 
		 tt_help_counter == 1) {
		describe_comment(message); // added on 290699
		tt_help_history_action = TRUCK_RECEIVED_CUBBY;
		if (tt_help_counter == 0) {
			describe_pointing(message,tool_in_hand,selection);
		};
		message << S(IDS_TRUCK_RECEIVED_CUBBY_HELP)
				  << S(IDS_TRUCK_RECEIVED_CARGO_HELP);
	} else if ((robot == NULL && !did(TRUCK_RECEIVED_ROBOT)
				  && tt_help_counter == 0) || 
		 tt_help_counter == 2) {
		tt_help_history_action = TRUCK_RECEIVED_ROBOT;
		message << S(IDS_TRUCK_RECEIVED_ROBOT_HELP);
		if (tt_help_counter == 0) { // otherwise it would be repeatious
			message << S(IDS_TRUCK_RECEIVED_CARGO_HELP);
		};
	} else if ((house_picture == NULL && !did(TRUCK_RECEIVED_HOUSE_PICTURE)
				  && tt_help_counter == 0) || 
		 tt_help_counter == 3) {
		tt_help_history_action = TRUCK_RECEIVED_HOUSE_PICTURE;
		message << S(IDS_TRUCK_RECEIVED_HOUSE_PICTURE_HELP);
	} else if (((avenue == NULL && street == NULL) && !did(TRUCK_RECEIVED_ADDRESS)
				  && tt_help_counter == 0) || 
		 tt_help_counter == 4) {
		tt_help_history_action = TRUCK_RECEIVED_ADDRESS;
		message << S(IDS_TRUCK_RECEIVED_ADDRESS_HELP);
		message << S(IDS_TRUCK_RECEIVED_ADDRESS_HELP2);
	} else if ((cubby != NULL && !did(TRUCKS_CARGO_REMOVED)
				  && tt_help_counter == 0) || 
		 tt_help_counter == 5) {
		tt_help_history_action = TRUCKS_CARGO_REMOVED;
		message << S(IDS_TRUCKS_CARGO_REMOVED_HELP);
	} else if ((!did(TYPE_HELP_GIVEN) && tt_help_counter == 0) || 
				  tt_help_counter == 6) {
//		tt_help_history_action = TYPE_HELP_GIVEN;
		message << S(IDS_TRUCK_HELP_GIVEN_HELP)
				  << S(IDS_TRUCK_HELP_GIVEN_HELP2);
		no_help_interruptions();
		user_did(TRUCK,TYPE_HELP_GIVEN);
	};
};

void Truck_Inside::drive_outside_to(House *house,
												block_number to_avenue_number,
												block_number to_street_number,
												HouseStyle new_house_style) {
	boolean camera_above = (tt_screen->camera_status() == CAMERA_ABOVE);
	city_coordinate initial_x, initial_y;
	house->exit_point(initial_x,initial_y);
	if (!camera_above && tt_programmer->kind_of() != PROGRAMMER_AT_FLOOR) {
		// second condition new on 300704 since otherwise doors begin to appear when the city fills up and you are on the floor
		house->open_door();
	};
	Block *block = house->block_im_on();
	// added following on 120499 since with explicit addresses it matters
	Block *goal_block = tt_city->block_at(to_avenue_number,to_street_number);
	short int lot_number;
	if (!goal_block->next_free_lot(lot_number)) { // update goal now since block full
		if (!tt_city->nearest_free_lot(to_avenue_number,to_street_number,lot_number)) {
			if (tt_log_version_number < 36) { // conditional new on 220204
				// no room for any more houses in whole city
				saved_by->abort_action(IDS_TRUCK_STOPPED_BECAUSE_CITY_FULL,TRUCK);
				set_saved_by(NULL);
				return;
			};
		};
	};
	Truck_Outside *truck_outside = new Truck_Outside(initial_x,initial_y,
								                            camera_above,
								                            truck_width,truck_height,
								                            robot,cubby,text,notebook,
								                            to_avenue_number,to_street_number,new_house_style,
								                            block, // was block prior to 120499
																	 saved_by); // (tt_log_version_number>=74)?NULL:saved_by); 
	// last arg updated on 010505 so that truck outside doesn't signal completion but this does
	// removed on 010505 since this made it too easy to have thousands of trucks driving around looking for space
	// following moved here on 090803 since setting saved_by to NULL made this a no-op
	if (saved_by != NULL && saved_by->kind_of() == ROBOT) { 
		// new on 100500 so if dumped before truck builds house the robot is OK
		((Robot *) saved_by)->remove_tool_from_working_set(this);
	};
	//if (tt_log_version_number >= 74) {
	//	// new on 010505
	//	if (saved_by != NULL) {
	//		saved_by->action_completed();
	//	};
	//} else {
		saved_by = NULL; // for safety - new on 171102 since passed along to truck_outside
	//};
	// associate timer with something that won't go away...
//	truck_outside->set_size_percent(1);
//	completed_after(duration_between_trucks,saved_action_status,tt_programmer->pointer_to_appearance());
	block->add_extra(truck_outside);
//	tt_screen->add(truck_outside); 
	// commented out on 010505 -- what sense did this make? And it left little truck turds on the screen
	truck_outside->start_activity(TRUCK_OUTSIDE_MOVING_TO_STREET);
//	if (!camera_above) { // commented out on 201103 since need to do this regardless of the view
		if (robot->pointer_to_leader() != NULL) robot->pointer_to_leader()->remove_follower(robot); 
		// new on 201103 since can be passed along if robot puts main box in truck
//		if (robot->pointer_to_leader() == this) remove_follower(robot);
		robot = NULL;
		if (cubby->pointer_to_leader() != NULL) cubby->pointer_to_leader()->remove_follower(cubby); 
		// new on 201103 since can be passed along if robot puts main box in truck
		//if (cubby->pointer_to_leader() == this) {
  //       remove_follower(cubby);
//#if TT_DEBUG_ON
//         if (tt_debug_mode == 18229 && cubby != NULL) {  // tracking down cubby with destroyed floor
//            tt_error_file() << cubby << " truck removed cubby to go outside and the cubby's floor is " 
		//                      << cubby->pointer_to_background() << endl;
//         };
//#endif
//      };
		cubby = NULL;
//	};
	if (text != NULL) {
		if (text->pointer_to_leader() != NULL) text->pointer_to_leader()->remove_follower(text);
		// new on 201103 since can be passed along if robot puts main box in truck
//		if (text->pointer_to_leader() == this) remove_follower(text);
		text = NULL;
	};
	if (notebook != NULL) {
		if (notebook->pointer_to_leader() != NULL) notebook->pointer_to_leader()->remove_follower(notebook); 
		// new on 201103 since can be passed along if robot puts main box in truck
//		if (notebook->pointer_to_leader() == this) remove_follower(notebook);
		notebook = NULL;
	};
//	destroy(saved_by); // I'm out of the picture now -- commented out on 171102 since passed along to truck_outside
};

Sprite *Truck_Inside::respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected, Sprite *by,
														boolean record_action, Sprite *original_recipient, 
														boolean size_constrained_regardless) {
	// new on 190205 so you can turn on and off trucks
	if (key == tt_stop_character) { 
		set_active(FALSE,FALSE); // FALSE added 020405 since shouldn't turn on and off contents (e.g. a box with an inactive sensor)
	} else if (key == tt_run_character) { 
		set_active(TRUE,FALSE); // FALSE added 020405 since shouldn't turn on and off contents (e.g. a box with an inactive sensor)
	};
	return(Truck::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,size_constrained_regardless));
};

Truck_Outside::Truck_Outside(city_coordinate x, city_coordinate y,
									  boolean from_above,
									  city_coordinate width, city_coordinate height,
									  Robot *robot,
									  Cubby *cubby,
                             Text *text,
                             Notebook *notebook,
									  block_number goal_avenue, block_number goal_street, 
									  HouseStyle house_style,
									  Block *block,
//									  ActionStatus *action_status,
                             Sprite *saved_by, // new on 171102 to replace action_status
									  short int lot_number) : // new on 080403
//									  House *house) :
	Truck(from_above?TRUCK_ABOVE_SPRITE:TRUCK_SIDE_SPRITE,
			 x,y,width,height,robot,cubby,text,notebook,saved_by), //,action_status),
	goal_avenue(goal_avenue),
	goal_street(goal_street),
	house_style(house_style),
	current_block(block),
	east_west_next(TRUE),
	activity(TRUCK_OUTSIDE_DOING_NOTHING),
	size_percent(100),
	size_is_100_percent(TRUE),
	lot_number(lot_number) {
//	house(house) {
//   immediate_destination_x(x),
//   immediate_destination_y(y) {
	 if (from_above) {
		 set_priority(max_long-1); // on the ground -- changed on 121204 to be -1 in case there are glued things on the ground
//		 set_priority_function_of_lly(FALSE);
//	 } else {
//		 set_priority_function_of_lly(TRUE);
	 };
//    tt_error_file() << "goal: " << goal_street << "," << goal_avenue << endl;
//    set_ok_to_dump(FALSE); // too hard to save its state -- finishes its job first -- 
	 // commented out on 241202 since is now saved
	 set_selectable(FALSE); // added on 150699 so size is right with new scaling when flying scheme
#if TT_DEBUG_ON
	 if (tt_debug_mode == 240405) {
		 tt_error_file() << "Truck#" << debug_counter << " just created on frame " << tt_frame_number << endl;
	 };
#endif
};

void Truck_Outside::release_all_references() { // renamed on 130504
	if (releasing_all_references) { // new on 230305
		return;
	};
#if TT_DEBUG_ON
	 if (tt_debug_mode == 240405) {
		 tt_error_file() << "Truck#" << debug_counter << " releasing all references on frame " << tt_frame_number << endl;
	 };
#endif
   //if (preparing_for_deletion) return; // already done
	// commented out above on 220704 since handled in sprite::destroy now and this prevented the following from running
	//preparing_for_deletion = TRUE; // new on 060204
   if (followers == NULL) { // flying above so no point adding them
		if (robot != NULL) {
			robot->destroy();
			robot = NULL;
		};
  	   if (cubby != NULL) {
			cubby->destroy();
			cubby = NULL;
		};
      if (text != NULL) {
			text->destroy();
			text = NULL;
		};
      if (notebook != NULL) {
			notebook->destroy();
			notebook = NULL;
		};
   };
// tt_error_file() << "Destroying robot " << robot << " of truck " << this << " on frame " << tt_frame_number << endl;
	Truck::release_all_references();
};

void Truck_Outside::display(SelectionFeedback selection_feedback,
									 boolean followers_too, TTRegion *region) {
	boolean view_from_above = (tt_screen->camera_status() == CAMERA_ABOVE);
//tt_error_file() << "Truck#" << return_debug_counter() << " " << (int) view_from_above 
	//             << " " << (int) side_view() << " " << tt_frame_number << endl;
	if ((view_from_above && side_view())  ||
		 (!view_from_above && !side_view())) { // wrong
//      city_coordinate x_correction;
//      if (view_from_above) { // a kludge to fix a geometry bug
//         x_correction = 0;
//      } else {
//         x_correction = 5*tile_width; // off by 1/4 screen
//      };
		Truck_Outside *replacement =
			new Truck_Outside(llx-true_x_offset(),//+x_correction,
                           lly-true_y_offset(),
									(tt_screen->camera_status() == CAMERA_ABOVE),
									truck_width,truck_height,
                           robot,cubby,text,notebook,
									goal_avenue,goal_street,house_style,
									current_block,
                           saved_by,
									lot_number); 
		// new on 080403 since if house has already been built this will pass info in as to where the house is
//									house);
		replacement->set_parameter(current_parameter());
		if (cubby->pointer_to_leader() != NULL) cubby->pointer_to_leader()->remove_follower(cubby); 
		// new on 201103 since can be passed along if robot puts main box in truck
//		if (cubby->pointer_to_leader() == this) remove_follower(cubby);
		cubby = NULL;
		if (robot->pointer_to_leader() != NULL) robot->pointer_to_leader()->remove_follower(robot); 
		// new on 201103 since can be passed along if robot puts main box in truck
//      if (robot->pointer_to_leader() == this) remove_follower(robot);
		robot = NULL;
		if (text != NULL) {
			if (text->pointer_to_leader() != NULL) text->pointer_to_leader()->remove_follower(text); 
			// new on 201103 since can be passed along if robot puts main box in truck
//			if (text->pointer_to_leader() == this) remove_follower(text);
			text = NULL;
		};
		if (notebook != NULL) {
			if (notebook->pointer_to_leader() != NULL) notebook->pointer_to_leader()->remove_follower(notebook); 
			// new on 201103 since can be passed along if robot puts main box in truck
//			if (notebook->pointer_to_leader() == this) remove_follower(notebook);
			notebook = NULL;
		};
//		tt_screen->remove(this);
//		current_block->remove_extra(this); // destroy_truck does this
		destroy_truck(FALSE);
		tt_screen->add(replacement);
		current_block->add_extra(replacement);
//     if (activity == TRUCK_OUTSIDE_BUILDING_HOUSE) { // act like its done
//			replacement->start_activity(TRUCK_OUTSIDE_LEAVING);
//      } else {
		   replacement->start_activity(activity);
//      };
		replacement->display(selection_feedback,followers_too,region);
		return;
	};
	if (view_from_above) {
		// divide by 150 rather than 100 because these trucks are too big
		const int divisor = 150;
		city_scale scale = tt_screen->current_scale();
		if (current_parameter() == TRUCK_OUTSIDE_NORTH || 
			 current_parameter() == TRUCK_OUTSIDE_SOUTH) {
			// turned 90 degrees 
			set_size((size_percent*(truck_height*ground_scale)/scale)/divisor,
						(size_percent*(truck_width*ground_scale)/scale)/divisor);
		} else {
			set_size((size_percent*(truck_width*ground_scale)/scale)/divisor,
						(size_percent*(truck_height*ground_scale)/scale)/divisor);
		};
//		tt_error_file() << "city scale = " << scale << " x_scale = " << x_scale
//                  << " y_scale = " << y_scale << endl;
	} else if (!size_is_100_percent) {
		size_is_100_percent = (flag) (size_percent == 100);
		if (current_parameter() == TRUCK_OUTSIDE_NORTH || 
			 current_parameter() == TRUCK_OUTSIDE_SOUTH) {
			city_coordinate new_width = (size_percent*north_south_truck_width)/100;
			city_coordinate new_height = (size_percent*north_south_truck_height)/100;
//			if (current_parameter() == TRUCK_OUTSIDE_SOUTH) {
//				city_coordinate delta_height = (new_height-height);
//            if (delta_height != 0) {
//				   set_size_and_location(new_width,new_height,llx,lly+delta_height/2);
//            };
//			} else {
				set_size(new_width,new_height);
//			};
		} else {
			city_coordinate new_width = (size_percent*truck_width)/100;
			city_coordinate new_height = (size_percent*truck_height)/100;
//			if (current_parameter() == TRUCK_OUTSIDE_WEST) {
//			   city_coordinate delta_width = (new_width-width);
//            if (delta_width != 0) {
//				   set_size_and_location(new_width,new_height,llx+delta_width*5,lly);
//            };
//			} else {
				// things grow towards right since llx,lly left alone
				set_size(new_width,new_height);
//			};
		};
	};
//   tt_error_file() << "Frame=" << tt_frame_number << "; x=" << llx << "; y=" << lly << "; this=" << this << endl;
	Sprite::display(selection_feedback,followers_too,region);	
	if (activity == TRUCK_OUTSIDE_WAITING_TO_MOVE_TO_GOAL &&
	    !finishing_instantly()) {
		start_activity(TRUCK_OUTSIDE_MOVING_TO_GOAL);
	};
};

void Truck_Outside::start_activity(TruckOutsideActivity new_activity) {
#if TT_DEBUG_ON
	 if (tt_debug_mode == 240405) {
		 tt_error_file() << "Truck#" << debug_counter << " activity# " << (int) new_activity 
			              << " on frame " << tt_frame_number << endl;
	 };
	//if (tt_debug_mode == 250405 && tt_frame_number == 725239) {
	//	Sprites *ignore = new Sprites(); // test on 250405
	//};
#endif
	activity = new_activity;
	switch (activity) {
		case TRUCK_OUTSIDE_MOVING_TO_STREET: {
//			city_coordinate goal_width, goal_height;
//			if (side_view()) {
//				goal_width = north_south_truck_width;
//				goal_height = north_south_truck_width;
//			} else {
//				goal_width = truck_width;
//				goal_height = truck_height;
//			};
			city_coordinate saved_llx = llx, saved_lly = lly;
			turn_to(SOUTH);
			set_size(width/25,height/25); // changes size now
			size_percent = 4; // used by other truck code
			size_is_100_percent = FALSE;
			move_to(saved_llx,saved_lly);
			city_coordinate y_delta = 0;
			if (!side_view()) {
//				move_by(-tt_block_width/20,0); // fixes bad geometry -- not clear why
				y_delta = -tile_height;
			};
//         immediate_destination_x = llx;
//         immediate_destination_y = tt_city->nearest_x_street(lly)+y_delta;
			animate_location(llx,tt_city->nearest_x_street(lly)+y_delta,
								  finishing_instantly()?0:time_to_get_to_street,
								  this,DRIVE_TO_CORNER_ACTIVITY_CALLBACK);
         millisecond grow_duration;
         if (finishing_instantly()) {
            grow_duration = 0;
         } else {
            if (side_view()) {
               grow_duration = grow_full_size_duration;
            } else {
               grow_duration = grow_full_size_duration/3; // otherwise too small to see
            };
         };
			add_ease_in_and_out_to_goal(ANIMATING_SIZE_PERCENT,100,grow_duration);
		  };	
			break;
		case TRUCK_OUTSIDE_MOVING_TO_CORNER: {
			block_number current_avenue, current_street;
			current_block->location(current_avenue,current_street);
// no point checking so soon
//			if (!current_block->next_free_lot(lot_number)) { // lot has filled up
//				tt_city->nearest_free_lot(goal_avenue,goal_street,lot_number);
				// got a new goal -- what if city is full?
//			};
			millisecond duration;
			if (current_avenue < goal_avenue) {
				duration = drive_horizontally_to((current_avenue+1)*tt_block_width,DRIVE_TO_GOAL_ACTIVITY_CALLBACK);
			} else if (current_avenue > goal_avenue) {
				duration = drive_horizontally_to(current_avenue*tt_block_width,DRIVE_TO_GOAL_ACTIVITY_CALLBACK);
			} else if (current_street != goal_street) { // doesn't really matter which way
				duration = drive_horizontally_to((current_avenue+my_random(2))*tt_block_width,DRIVE_TO_GOAL_ACTIVITY_CALLBACK);
			} else {  // otherwise at the right block so skip this phase 
				start_activity(TRUCK_OUTSIDE_MOVING_TO_LOT);
				duration = 0; // let next truck out
			};
         // typically when truck gets to corner another can come out
         if (saved_by != NULL) { // prior to 191102 was (saved_action_status != NULL) 
//				completed_after(duration,saved_action_status,tt_programmer->pointer_to_appearance());
            tt_programmer->pointer_to_appearance()->do_after(duration,this,ACTION_COMPLETED_CALLBACK); // rewritten on 131102
//				saved_action_status = NULL; // commented out 131102
			};
			break;
		  };
		case TRUCK_OUTSIDE_MOVING_TO_GOAL:
			drive_to_next_block();
			break;
		case TRUCK_OUTSIDE_MOVING_TO_LOT:
			drive_to_free_lot();
			break;
		case TRUCK_OUTSIDE_BUILDING_HOUSE:
         // if still growing wait until done, so don't start shrinking same time
			do_after(delay_before_leaving(),this,LEAVE_SINCE_HOUSE_BUILT_CALLBACK);
         break;
		case TRUCK_OUTSIDE_LEAVING:
			leave_since_house_built();
			break;
		// no other cases
		default:
			tt_error_file() << "Truck is doing an activity that isn't handled" << (int) activity << endl;
	};
};

millisecond Truck_Outside::delay_before_leaving() {
   return(wait_before_leaving+animations_stop_time()-tt_current_time);
};

millisecond Truck_Outside::drive_horizontally_to(city_coordinate new_llx, AnimationCallBack callback) {
	city_coordinate distance;
	if (new_llx < llx) {
		distance = llx-new_llx; 
		turn_to(WEST);
//      if (size_percent != 100) {
         // grows around lower left corner so compenstate
//         new_llx += width-truck_width;
//      };
	} else if (new_llx > llx) {
		distance = new_llx-llx;
		turn_to(EAST);
	} else { // otherwise don't turn
		do_after(0,this,callback); // new on 020506 so doesn't get stuck
		return(0); // new on 100603 -- otherwise distance isn't bound -- and why make a sound if there is no distance to move?
	};
	millisecond duration = driving_duration(distance);
	truck_sound(new_llx,lly);
//   immediate_destination_x = new_llx;
//   immediate_destination_y = lly;
	animate_location(new_llx,lly,duration,this,callback);
	return(duration);
};

void Truck_Outside::truck_sound(city_coordinate new_llx, city_coordinate new_lly) {
	if (side_view()) {
		TTRegion *view_region = tt_screen->viewing_region();
		TTRegion travel_region;
		travel_region.min_x = min(new_llx,llx);
		travel_region.max_x = max(new_llx,llx)+width;
		travel_region.min_y = min(new_lly,lly);
		travel_region.max_y = min(new_lly,lly)+height;
		if (view_region->overlaps(travel_region)) {
			play_sound(TRAFFIC_SOUND);
		};
	};
};

millisecond Truck_Outside::driving_duration(city_coordinate distance) {
	if (finishing_instantly()) {
		return(0);
	} else if (side_view()) {
		return((distance*time_to_drive_one_block_side)/tt_block_width);
	} else {
		return((distance*time_to_drive_one_block_above)/tt_block_width);
	};
};

void Truck_Outside::turn_to(Direction direction) {
//	city_coordinate truck_width;
	switch (direction) {
		case EAST:
			set_parameter(TRUCK_OUTSIDE_EAST);
			if (side_view()) {
				set_size((size_percent*truck_width)/100,(size_percent*truck_height)/100);
			};
//			truck_width = height;
			move_by(0,tile_height);
			break;
		case WEST:
			set_parameter(TRUCK_OUTSIDE_WEST);
			if (side_view()) {
				set_size((size_percent*truck_width)/100,(size_percent*truck_height)/100);
			};
//			truck_width = height;
			move_by(0,-tile_height);
			break;
		case SOUTH:
			set_parameter(TRUCK_OUTSIDE_SOUTH);
			if (side_view()) {
				set_size((size_percent*north_south_truck_width)/100,
							(size_percent*north_south_truck_height)/100);
			};
//			truck_width = width;
			move_by(-tile_width,0);
			break;
		case NORTH:
			set_parameter(TRUCK_OUTSIDE_NORTH);
			if (side_view()) {
				set_size((size_percent*north_south_truck_width)/100,
							(size_percent*north_south_truck_height)/100);
			};
//			truck_width = width;
			move_by(tile_width,0);
			break;
	};
	tt_city->nearest_lane(llx,lly,direction); //,truck_width);
};

boolean Truck_Outside::update_goal() {
#if TT_DEBUG_ON
	 if (tt_debug_mode == 240405) {
		 tt_error_file() << "Truck#" << debug_counter << " updating goal on frame " << tt_frame_number << endl;
	 };
#endif
	Block *goal_block = tt_city->block_at(goal_avenue,goal_street); // new on 020405
	if (!goal_block->next_free_lot(lot_number)) { // was current_block prior to 020405
		if (!tt_city->nearest_free_lot(goal_avenue,goal_street,lot_number)) {
			// city full so wander around
			goal_avenue = (block_number) my_random(tt_city->return_number_of_x_streets());
			goal_street = (block_number) my_random(tt_city->return_number_of_y_streets());
//#if TT_DEBUG_ON
//			if (tt_debug_mode == 80298) {
//				tt_error_file() << "New goal for ";
//				print(tt_error_file());
//				tt_error_file() << " is " << (int) goal_avenue << " and " << (int) goal_street << endl;
//			};
//#endif
			return(FALSE); // not really updated...
		};
	};
	return(TRUE);
};

void Truck_Outside::drive_to_next_block() {
  if (finishing_instantly()) {
	  if (!update_goal()) {
        activity = TRUCK_OUTSIDE_WAITING_TO_MOVE_TO_GOAL;
		  return; // city full -- not much else to do now
	  };
	  switch_to(goal_avenue,goal_street);
	  city_coordinate city_x, city_y;
	  current_block->city_location(city_x,city_y,lot_number);
	  if (current_parameter() == TRUCK_OUTSIDE_EAST) {
		  city_y += tile_height;
	  } else {
		  city_y -= tile_height;
	  };
	  move_to(city_x-width,city_y);
	  build_house(TRUE);
	  return;
  };
  block_number current_avenue, current_street;
  current_block->location(current_avenue,current_street);
  if (current_street == goal_street) {
	  if (current_avenue == goal_avenue) { // on the goal block
		  drive_to_free_lot();
		  return;
	  };
	  east_west_next = TRUE; // no alternative
  } else if (current_avenue == goal_avenue) {
	  east_west_next = FALSE;
  };
  if (east_west_next) { 
	  if (current_avenue < goal_avenue) { // go east
		  switch_to((block_number) (current_avenue+1),current_street);
		  turn_to(EAST);
		  if (current_avenue+1 == goal_avenue) { // can get there from this corner as well
			  start_activity(TRUCK_OUTSIDE_MOVING_TO_GOAL);
		  } else {
//           immediate_destination_x = llx+tt_block_width;
//           immediate_destination_y = lly;
			  city_coordinate delta_x = tt_block_width;
			  // new on August 2 98 
		     // make sure delta isn't too much
			  if (llx+delta_x > tt_city->max_x()) delta_x = tt_city->max_x()-llx; //  && tt_log_version_number > 14
           truck_sound(llx+delta_x,lly);
			  animate_delta_location(delta_x,0,driving_duration(tt_block_width),this,DRIVE_TO_GOAL_ACTIVITY_CALLBACK);
		  };
	  } else if (current_avenue > goal_avenue) {
		  switch_to((block_number)(current_avenue-1),current_street);
		  turn_to(WEST);
//        immediate_destination_x = llx-tt_block_width;
//        immediate_destination_y = lly;
		  city_coordinate delta_x = tt_block_width;
		  // new on August 2 98  
		  // make sure delta isn't too much
		  if (llx-delta_x < 0) delta_x = llx; //  && tt_log_version_number > 14
        truck_sound(llx-delta_x,lly);
		  animate_delta_location(-delta_x,0,driving_duration(tt_block_width),this,DRIVE_TO_GOAL_ACTIVITY_CALLBACK);
	  };
	  east_west_next = FALSE;
  } else {
	  if (current_street < goal_street) { 
		  switch_to(current_avenue,(block_number) (current_street+1));
		  turn_to(NORTH);
		  truck_sound(llx,lly+tt_block_height);
		  animate_delta_location(0,tt_block_height,driving_duration(tt_block_height),this,DRIVE_TO_GOAL_ACTIVITY_CALLBACK);
	  } else if (current_street > goal_street) { 	  
		  switch_to(current_avenue,(block_number) (current_street-1));
		  turn_to(SOUTH);
//        immediate_destination_x = llx;
//        immediate_destination_y = lly-tt_block_height;
        truck_sound(llx,lly-tt_block_height);
		  animate_delta_location(0,-tt_block_height,driving_duration(tt_block_height),this,DRIVE_TO_GOAL_ACTIVITY_CALLBACK);
	  };
	  east_west_next = TRUE;
  };
};

void Truck_Outside::switch_to(block_number avenue, block_number street) {
	current_block->remove_extra(this);
	current_block = tt_city->block_at(avenue,street);
	current_block->add_extra(this);
};

void Truck_Outside::drive_to_free_lot() {
	city_coordinate house_x, house_y;
	if (current_block->next_free_lot(house_x,house_y,lot_number)) {
		activity = TRUCK_OUTSIDE_MOVING_TO_LOT; // in case view switched
      // tt_error_file() << "Truck#" << return_debug_counter() << " driving " 
		//                 << (house_x-width/2) << " " << tt_frame_number << endl;
		drive_horizontally_to(house_x-width/2,BUILD_HOUSE_CALLBACK);
	} else {
		update_goal();
//		do_after(finishing_instantly()?0:1000,this,drive_to_goal_activity_callback);
      start_activity(TRUCK_OUTSIDE_MOVING_TO_CORNER);
	};
};

void Truck_Outside::build_house(boolean have_checked_lot_free) {
	city_coordinate house_x, house_y;
	short int new_lot_number;
	// following moved here from condition on 090399 since otherwise house_x and house_y aren't always computed
	boolean current_block_free = current_block->next_free_lot(house_x,house_y,new_lot_number);
	if (have_checked_lot_free || current_block_free) {
		if (new_lot_number == lot_number) { // not been beaten to it
#if TT_DEBUG_ON
			if (tt_debug_mode == 240405) {
				tt_error_file() << "Truck#" << debug_counter << " building house at " << house_x << "," << house_y 
									 << " on frame " << tt_frame_number << endl;
			};
#endif
			House *house = new House(house_x,house_y,current_block,house_style); 
			// house was an instance variable prior to 261202 but is easy to retrieve from current_block so why bother?
		   house->set_truck_builder(this); // to notify me if house blows up first
			current_block->add_house(house,lot_number);
			if (current_parameter() == TRUCK_OUTSIDE_WEST) {
				set_parameter(TRUCK_OUTSIDE_EMPTY_WEST);
			} else {
				set_parameter(TRUCK_OUTSIDE_EMPTY_EAST);
			};
			activity = TRUCK_OUTSIDE_BUILDING_HOUSE;
//			do_after(tt_finish_all_animations?0:delay_before_leaving(),this,leave_since_house_built_callback);
         do_after(finishing_instantly()?0:delay_before_leaving(),this,LEAVE_SINCE_HOUSE_BUILT_CALLBACK);
		} else { // block not full but lot taken
			lot_number = new_lot_number;
			start_activity(TRUCK_OUTSIDE_MOVING_TO_LOT);
		};
	} else {
//		drive_to_free_lot();
      start_activity(TRUCK_OUTSIDE_MOVING_TO_CORNER); // start again
	};
};

void Truck_Outside::leave_since_house_built() {
#if TT_DEBUG_ON
	 if (tt_debug_mode == 240405) {
		 tt_error_file() << "Truck#" << debug_counter << " leave_since_house_built on frame " << tt_frame_number << endl;
	 };
#endif
	activity = TRUCK_OUTSIDE_LEAVING;
	millisecond duration = finishing_instantly()?0:truck_disappear_duration;
	turn_to(NORTH); // go inside house after building (to deliver contents)
	city_coordinate house_x, house_y;
	boolean camera_above = (tt_screen->camera_status() == CAMERA_ABOVE);
	House *house = current_block->house_at(lot_number); // new on 261202
	if (house != NULL) { // might have blown up since being built
		if (!camera_above) house->open_door();
		house->exit_point(house_x,house_y);
	} else {
		house_x = llx; // OK?
		house_y = lly;
	};
//	if (current_parameter() == TRUCK_OUTSIDE_WEST) {
//		animate_delta_location(-tt_block_width,0,duration);
//	} else {
//		animate_delta_location(tt_block_width,0,duration);
//	};
	size_is_100_percent = FALSE; 
	size_percent = 80;
	set_size_percent(size_percent); // changed width etc now
	city_coordinate initial_house_x, initial_house_y;
	if (side_view()) {
		initial_house_x = house_x-width/4;
		initial_house_y = lly;
	} else {
		if (house != NULL && house->house_style() == HOUSE_B) { 
			house_x -= width/4; // misses doorway
		};
		initial_house_x = house_x-width/4;
		initial_house_y = lly-height/2;
		house_y -= height/8; // goes too far into house otherwise
	};
	move_to(initial_house_x,initial_house_y);
	if (duration > 0) {
		city_coordinate truck_width = north_south_truck_width/10;
		city_coordinate truck_height = (80*north_south_truck_height)/100;
		robot->set_size_and_location(truck_width,truck_height/3,
											  initial_house_x,initial_house_y,TO_FIT_INSIDE);
		cubby->set_size_and_location(truck_width,truck_height/3,
											  initial_house_x+truck_width/10,initial_house_y+truck_height/3,TO_FIT_INSIDE);
		robot->set_priority(priority()-2); 
		cubby->set_priority(priority()-1); 
	};
	if (!camera_above) { // too small otherwise anyway and costly and 
		// seen as too big because as followers not completely inside
		// they are displayed by screeen and don't know about scale stuff
		if (robot->pointer_to_leader() != NULL) { // new with 3.28
			robot->pointer_to_leader()->remove_follower(robot);
		};
		if (cubby->pointer_to_leader() != NULL) {
			cubby->pointer_to_leader()->remove_follower(cubby);
		};
		add_follower(robot,FALSE);
		add_follower(cubby,FALSE);
//#if TT_DEBUG_ON
//      if (tt_debug_mode == 18229 && cubby != NULL) {  // tracking down cubby with destroyed floor
//         tt_error_file() << cubby << " truck outside with camera not above added cubby whose floor is " << cubby->pointer_to_background() << endl;
//      };
//#endif
		tt_screen->add(robot);
		tt_screen->add(cubby);
	};
	truck_sound(house_x,house_y);
//   immediate_destination_x = house_x;
//   immediate_destination_y = house_y;
	animate_location(house_x,house_y,duration);
//tt_error_file() << "Animating destruction of truck " << this << " with duration " << duration 
	//             << " on frame " << tt_frame_number << endl;
//	// added tt_global_picture-> 080403 since if you walk into the house 
	// as the truck is driving in the following might not happen otherwise
	add_ease_in_and_out_to_goal(ANIMATING_SIZE_PERCENT,10,duration,this,DESTROY_TRUCK_CALLBACK);
//	driving_duration(llx+width)/2);
};

//	city_coordinate city_center_x, city_center_y;
//	tt_city->center(city_center_x,city_center_y);
//	if (llx < city_center_x) {
//		drive_horizontally_to(width,destroy_truck_callback);
//	} else {
//		drive_horizontally_to(2*city_center_x+width,destroy_truck_callback);
//	};

void Truck_Outside::destroy_truck(boolean fill_house_too) {
	if (prepared_for_deletion()) return; // not quite sure how this happens
#if TT_DEBUG_ON
	 if (tt_debug_mode == 240405) {
		 tt_error_file() << "Truck#" << debug_counter << " destroy_truck on frame " << tt_frame_number << endl;
	 };
#endif
	if (robot != NULL && robot->pointer_to_leader() == this) {
		tt_screen->remove(robot);
		remove_follower(robot);
	};
	if (cubby != NULL && cubby->pointer_to_leader() == this) {
		tt_screen->remove(cubby);
		remove_follower(cubby);
//#if TT_DEBUG_ON
//      if (tt_debug_mode == 18229 && cubby != NULL) {  // tracking down cubby with destroyed floor
//         tt_error_file() << cubby << " truck outside about to be destroyed removed follower cubby whose floor is " 
									//<< cubby->pointer_to_background() << endl;
//      };
//#endif
	};
	House *house = current_block->house_at(lot_number); // new on 261202
	if (fill_house_too && house != NULL) {
//tt_error_file() << "Truck#" << return_debug_counter() << " with robot#" 
		//          << robot->return_debug_counter() << " " << tt_frame_number << endl;
		fill_house(house);
		house->set_truck_builder(NULL); // no longer need to notify me if house blows up first
#if TT_DEBUG_ON
	} else if (fill_house_too) {
		log("Destroy_truck called with fill_house_too and no house.");
#endif
	};
	current_block->remove_extra(this);
	tt_screen->remove(this);
#if TT_DEBUG_ON
	if (tt_debug_mode == 220704 && fill_house_too) {
		tt_error_file() << "destroy truck#" << debug_counter << endl;
//		tt_debug_target = debug_counter;
	};
#endif
	destroy();
};

// so what happens when saving while flying or with time travel?? -- need to at least get the XML version of this working

boolean Truck_Outside::dump(output_stream &, boolean just_prepare, boolean , boolean ) {
   // too much work to dump trucks with all their state
   if (just_prepare) {  // finish instantly
      finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
//      build_house(FALSE);
//      destroy_truck(TRUE);
#if TT_DEBUG_ON
   } else {
      tt_error_file() << "Shouldn't be trying to save a truck that is outside." << endl;
#endif
   };
	return(FALSE);
};

#if TT_XML

xml_element *Truck_Outside::xml_set_special_attributes(xml_element *element, xml_document *document) {
	// new on 191202 -- went through all the state variables
	xml_element *special_element = Truck::xml_set_special_attributes(element,document);
	if (goal_avenue != 0) {
		xml_set_attribute(special_element,L"GoalAvenue",goal_avenue);
	};
	if (goal_street != 0) {
		xml_set_attribute(special_element,L"GoalStreet",goal_street);
	};
	if (lot_number >= 0) {
		xml_set_attribute(special_element,L"LotNumber",lot_number+1);
	};
	if (!east_west_next) {
		xml_set_attribute(special_element,L"NorthSouthMovementNext",1);
	};
	if (!size_is_100_percent) {
		xml_set_attribute(special_element,L"SizeWasNot100Percent",1);
	};
	if (size_percent != 100) {
		xml_set_attribute(special_element,L"SizePercent",size_percent);
	};
	if (house_style != HOUSE_A) {
		xml_set_attribute(special_element,L"HouseStyleCode",house_style);
	};
	if (activity != TRUCK_OUTSIDE_DOING_NOTHING) {
		xml_set_attribute(special_element,L"ActivityCode",(int) activity);
	};
	// current_block is set at load time 
	return(special_element);
};

boolean Truck_Outside::xml_get_special_attributes(xml_node *node) {
	goal_avenue = xml_get_attribute_int(node,L"GoalAvenue",0);
	goal_street = xml_get_attribute_int(node,L"GoalStreet",0);
	lot_number = xml_get_attribute_int(node,L"LotNumber",lot_number)-1; // convert from 1-indexing to 0-indexing
	east_west_next = !xml_get_attribute_int(node,L"NorthSouthMovementNext",0); // prior to 210404 was !east_west_next
	size_is_100_percent = !xml_get_attribute_int(node,L"SizeWasNot100Percent",0); // prior to 210404 was !size_is_100_percent
	size_percent = xml_get_attribute_int(node,L"SizePercent",100);
	house_style = (HouseStyle) xml_get_attribute_int(node,L"HouseStyleCode",(int) HOUSE_A);
	activity = (TruckOutsideActivity) xml_get_attribute_int(node,L"ActivityCode",(int) TRUCK_OUTSIDE_DOING_NOTHING);
	return(Truck::xml_get_special_attributes(node));
};

void Truck_Outside::set_current_block(Block *block) { // new on 261202
	current_block = block;
//	tt_screen->add(this); // too early for this
};

wide_string Truck_Outside::xml_tag() { // revised on 261202 to generate different tags depending upon view
	if (side_view()) {
		return(L"TruckOutsideViewedFromSide");
	} else {
		return(L"TruckOutsideViewedFromAbove");
	};
};

#endif

