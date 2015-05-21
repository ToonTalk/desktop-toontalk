// Copyright (c) 1992-2006.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)
#include "defs.h"
#endif
#if !defined(__TT_GLOBALS_H)
#include "globals.h"
#endif
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif   
#if !defined(__TT_BCKGRND_H)   
#include "bckgrnd.h"
#endif   
#if !defined(__TT_FLOOR_H)   
#include "floor.h"
#endif   
#if !defined(__TT_CUBBY_H)   
#include "cubby.h"
#endif   
//#if !defined(__TT_PRIMITIV_H)   
//#include "primitiv.h"
//#endif   
#if !defined(__TT_PRGRMMR_H)   
#include "prgrmmr.h"
#endif   
#if !defined(__TT_PICTURE_H)   
#include "picture.h"
#endif   
#if !defined(__TT_PAD_H)   
#include "pad.h"
#endif   
#if !defined(__TT_SPRITES_H)   
#include "sprites.h"
#endif   
#if !defined(__TT_HELP_H)
#include "help.h"
#endif
#if !defined(__TT_JAVA_H)
#include "java.h"
#endif
#if !defined(__TT_MARTIAN_H)
#include "martian.h"
#endif
#if !defined(__TT_WINMAIN_H)   
#include "winmain.h"
#endif   
#if !defined(__TT_UTILS_H)   
#include "utils.h" // for sounds
#endif   
#if !defined(__TT_ROBOT_H)
#include "robot.h"
#endif
#if !defined(__TT_NUMBER_H)
#include "number.h"
#endif
#if !defined(__TT_TEXT_H)
#include "text.h"
#endif
#if !defined(__TT_MOUSE_H)
#include "mouse.h"
#endif
#if TT_DIRECT_PLAY
#if !defined(__TT_NETWORK_H)
#include "network.h"
#endif
#endif
#if TT_XML
#if !defined(__TT_XML_H)
#include "xml.h"
#endif
#endif
#if !defined(__TT_BIRD_H)   
#include "bird.h"
#endif
//#if !TT_32
//#include <stdlib.h>  // for random
//#endif   
   
#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  

// not clear after 040903 that this needs to put animations on tt_global_picture since the "right" thing is now done
// when switching between backgrounds

// if changed update help.cpp
enum NestHistory {NEST_HATCHED_BIRD=0,
						NEST_RECEIVED_ITEM_FROM_BIRD,
						NEST_ITEM_REMOVED,
						NEST_ITEM_ADDED};

enum BirdHistory {BIRD_RECEIVED_ITEM=0}; // for now

#if TT_DIRECT_PLAY
void register_networked_nest(Nest *nest);
Nest *find_nest_with_same_guid(GUID *nest_guid, Sprites *nests);

#endif

//void set_selectable_true_callback(void *s) {
//	((Sprite *) s)->set_selectable(TRUE); // could be moved to sprite.cpp
//};

//typedef Nest *NestPointer; -- also defined in defs.h
NestPointer *dumped_nests = NULL;
flag *java_before_done_already = NULL;
int nests_assigned_index = 0;
const int max_nests_dumped = 255;
const unsigned char NO_NEST_INDEX = 255;

const millisecond time_to_fly_one_block = 1000;
const millisecond bird_grow_full_size_duration = 1000;
const millisecond bird_shrink_full_size_duration = 1000;
const millisecond time_to_fly_from_door = 4000;
const millisecond time_to_fly_from_inside = 1000;
const millisecond time_to_fly_to_programmer = 3000;
const long bird_side_view_size_percent = 40;

#if TT_DEBUG_ON
string delivery_status_name[] = 
	  {"NO_DELIVERY",
		"HEADED_TO_NEST",
		"SETTING_DOWN_MESSAGE",// obsolete - noticed 110203
		"HEADED_BACK_TO_NEST_EMPTY_HANDED",
		"SETTING_DOWN_STACK", // obsolete - noticed 110203
		"HEADED_BACK_TO_MESSAGE",
		"HEADED_BACK_TO_STACK",
		"HEADED_BACK_TO_NEST",
		"HEADED_BACK_TO_NEST_WITH_STACK",
		"DELIVERY_COMPLETE_AND_RETURNING",
		"HEADED_TO_NEST_ON_FLIPSIDE"};
#endif

boolean guid_temporarily_reset(Nest *nest);

void release_nest_indices() {
	if (dumped_nests != NULL) {
		delete [] dumped_nests;
		dumped_nests = NULL;
		nests_assigned_index = 0;
	};
   if (java_before_done_already != NULL) {
      delete [] java_before_done_already;
      java_before_done_already = NULL;
   };
};

unsigned char nest_dump_index(Nest *nest) {
	if (dumped_nests == NULL) {
		dumped_nests = new NestPointer[max_nests_dumped];
		for (int i = 1; i < max_nests_dumped; i++) {
			dumped_nests[i] = NULL;
		};
		dumped_nests[0] = nest;
		nests_assigned_index = 1;
		return(0);
	};
	for (unsigned char i = 0; i < nests_assigned_index; i++) {
		if (dumped_nests[i] == nest) {
			return(i);
		};
	};
	if (nests_assigned_index < max_nests_dumped) {
		dumped_nests[nests_assigned_index] = nest;
		return((unsigned char) nests_assigned_index++);
	} else {
		say_error(IDS_SAVING_TOO_MANY_NESTS);
		return(NO_NEST_INDEX);
	};
};

Nest *indexed_nest(unsigned char index) {
	if (index == NO_NEST_INDEX) {
		return(new Nest(0,0,0,2*tile_width));
	};
	if (dumped_nests == NULL) {
		dumped_nests = new NestPointer[max_nests_dumped];
		for (int i = 0; i < max_nests_dumped; i++) {
			dumped_nests[i] = NULL;
		};
	};
	if (dumped_nests[index] == NULL) {
		dumped_nests[index] = new Nest(0,0,0,2*tile_width);
	};
	return(dumped_nests[index]);
};

void java_declare_all_nests(java_stream &out) {
	// re-written on 170201 since nests are shared accross pictures and subpictures are now initialized by seperate methods
	for (int i = 0; i < nests_assigned_index; i++) {
//		java_declare(out,S(IDS_JAVA_TT_NEST),S2(IDS_JAVA_NEST,index));
		java_indent(out);
      out << "protected static TTNest nest" << i << ";" << endl;
   };
};

void java_assign_all_nests(java_stream &out) {
	// re-written on 170201 since nests are shared accross pictures and subpictures are now initialized by seperate methods
	java_indent(out);
	out << "private void intializeNests() {" << endl;
	tt_java_indent += 2;
	for (int i = 0; i < nests_assigned_index; i++) {
      dumped_nests[i]->java_initialize_nest(out,i);
   };
	tt_java_indent -= 2;
	java_indent(out);
	out << "}" << endl;
};

Nest *load_nest(InputStream *pad_in, 
                int notebook_version, NaturalLanguage language,
					 Sprite *container, Picture *controlee, long index, boolean descendants_inactive) {
	// following moved here on 130700 since was missing for a while when !TT_DIRECT_PLAY
	if (notebook_version < first_version_to_save_copied_nests) { // old way (prior to 240299)
		unsigned char index = (unsigned char) pad_in->get();
		Nest *nest = indexed_nest(index);
	//	nest->save_size();
		// why do the following if already indexed - not just made
		unsigned short int stack_size;
		pad_in->read((string) &stack_size, sizeof(stack_size));
		for (int i = 0; i < stack_size; i++) {
			 Sprite *item = load_item(pad_in,notebook_version,language,container,controlee);
			 if (item != NULL) {
				 if (i == 0) { // first one
					 nest->give_size_and_location_to_cover_to(item); // might be too early since nest doesn't have good geometry yet
				 };
				 nest->insert_at_end_of_contents(item);
			 };
		};
		return(nest);
	};
	Nest *nest = new Nest(0,0,0,2*tile_width);
	associate_shared_item(index,nest);
   if (container != NULL) {  // new on 160702
      if (container->kind_of() == PROGRAM_PAD) {
         nest->set_in_notebook((Notebook *) container,FALSE); // no point recuring
      } else if (container->kind_of() == VACUUM) {
         nest->set_inside_vacuum(TRUE,FALSE);
      };
   }; 
	nest->set_comment(load_item(pad_in,notebook_version,language));
	SpriteType type = (SpriteType) pad_in->get();
	if (type != END_OF_LIST_MARKER) {
		Sprite *item = load_item(type,pad_in,notebook_version,language,NULL,NULL,-1,FALSE,TRUE); // last NULL was controlee prior to 140301 but nests "protect" alligience of remote controls
		if (item != NULL) {
			// only first item on nest needs size info
			nest->give_size_and_location_to_cover_to(item); // might be too early since nest doesn't have good geometry yet
			nest->insert_at_end_of_contents(item);
		}; // else warn? - return NULL?
		type = (SpriteType) pad_in->get();
		while (type != END_OF_LIST_MARKER && !pad_in->empty()) {
			Sprite *item = load_item(type,pad_in,notebook_version,language,NULL,NULL,-1,FALSE,TRUE); // new on 140301 (same as above)
			if (item != NULL) {
				nest->insert_at_end_of_contents(item);
			}; // else warn? - return NULL?
			type = (SpriteType) pad_in->get();
#if TT_DEBUG_ON
			if (type > LAST_SPRITE_TYPE && type < EQUAL_TO_OLD_ITEM_AND_GEOMETRY) {
				log("Something on a nest was stored in a way that can't be read again. Sorry");
			};
#endif
		};
	};
	// and now for nest copies
	type = (SpriteType) pad_in->get();
	while (type != END_OF_LIST_MARKER && !pad_in->empty()) {
		Nest *nest_copy =  (Nest *) load_item(type,pad_in,notebook_version,language,NULL,controlee);
		if (nest_copy != NULL) {
			nest->add_copy(nest_copy);
			nest_copy->set_original_nest(nest);
		};
		type = (SpriteType) pad_in->get();
	};
	Text *label = (Text *) load_item(pad_in,notebook_version,language);
	if (label != NULL) {
		nest->set_label(label);
	};
	if (notebook_version >= first_version_to_save_programmer_and_remotes_accurately) {
//#if TT_DIRECT_PLAY - moved downward on 240600 so still reads the same stuff but ignores it if not TT_DIRECT_PLAY
		unsigned short int address_and_guid_size;
		pad_in->read((string) &address_and_guid_size,sizeof(address_and_guid_size));
		if (address_and_guid_size > 0) {
			GUID *nest_guid = new GUID;
			boolean proxy;
			string *addresses;// = NULL;
			int address_count = 0;
			unsigned char first_character_of_address = pad_in->get();
			unsigned char nest_info_version_number = pad_in->get();
			if (first_character_of_address == NEST_NETWORK_INFO_MARKER) { // && nest_info_version_number == 0) { - commented out on 290700 since version 1 fixes size that forgot to include proxy flag
				// I doubt there will be a conflict with old style address
				address_count = pad_in->get();
				addresses = new string[address_count];
				for (int i = 0; i < address_count; i++) {
					unsigned char IP_address_size = pad_in->get();
					addresses[i] = new char[IP_address_size];
					pad_in->read((string) addresses[i],IP_address_size);
				};
				pad_in->read((string) nest_guid,sizeof(GUID));
				proxy = pad_in->get();
#if !TT_DIRECT_PLAY
				say_error(S(IDS_NETWORK_NESTS_NOT_SUPPORTED_YET,"Loading a nest that works over a network but this version of ToonTalk can't deal with the network. Network stuff ignored. Visit www.toontalk.com for information on how to upgrade."));
				delete nest_guid;
				if (addresses != NULL) {
					delete_strings(addresses,address_count);
				};
			};
		};
	};
	return(nest);
#else
			} else { // older convention was to use DirectPlay address directly
				int address_size = address_and_guid_size-(sizeof(GUID)+1);
				if (address_size < 0) return(FALSE); // added for robustness on 180201
				bytes player_address = new byte[address_size];
				player_address[0] = first_character_of_address;
				player_address[1] = nest_info_version_number;
				pad_in->read((string) player_address+2,address_size-2);
				addresses = IP_addresses_of_player(player_address,address_size,address_count);
//				nest->set_player_address(old_style_player_address,address_size);
				delete [] player_address;
				pad_in->read((string) nest_guid,sizeof(GUID));
				proxy = pad_in->get();
			};
			nest->set_addresses(addresses,address_count);
         nest->set_nest_guid(nest_guid);
		   nest->set_proxy(proxy);
			if (!tt_loading_to_ignore) { // condition new on 181202
				nest->connect_to_nest_with_same_guid();
			};
//				nest->set_parameter(NEST_EMPTY); - maybe this is useful to have the egg so can easily get a new bird...
		};
//#else
//			unsigned long int extra_stuff = ignore_growth(pad_in,2);
//			if (extra_stuff > 0) {
//				say_error(S(IDS_NETWORK_NESTS_NOT_SUPPORTED_YET,"Loading a nest that works over a network but this version of ToonTalk can't deal with the network. Network stuff ignored. Visit www.toontalk.com for information on how to upgrade."));
//			};
//#endif
	};
	return(nest);
#endif
};

Sprites *non_networked_nests_with_guid = NULL; // new on 181002 to deal better with nest copies

// new on 011203 to prevent any sharing between pages or separate pastes
Sprites *push_non_networked_nests_with_guid() {
	Sprites *result = non_networked_nests_with_guid;
	non_networked_nests_with_guid = NULL;
	return(result);
};

void restore_non_networked_nests_with_guid(Sprites *sprites) {
	// shouldn't this call release_non_networked_nests_with_guid() -- asked 180704
	release_non_networked_nests_with_guid(); // I think yes -- added to fix a leak on 120804
	non_networked_nests_with_guid = sprites;
};

void release_non_networked_nests_with_guid() {
   just_destroy_list(non_networked_nests_with_guid);
};

// some of the following could be made to work when !TT_DIRECT_PLAY

#if TT_DIRECT_PLAY

void Nest::connect_to_nest_with_same_guid() { // abstracted from load_nest on 041002
   if (nest_guid == NULL) return; // Isn't long distance
   set_parameter(NEST_EMPTY); // should be a bird somewhere (even if remote)
   Nest *networked_nest = NULL;
	if (!proxy()) {
      if (addresses == NULL) {
//#if TT_ALPHA5 // commented out on 311003 since breaks some connections and unbamming handled differently
//			if (!tt_loading_to_undo_bammer) { 
//				// new on 211003 so don't get extra connections unless unbamming (or are really networked - e.g. proxied)
//				return;
//			};
//#endif
         networked_nest = find_nest_with_same_guid(nest_guid,non_networked_nests_with_guid);
      } else {
		   networked_nest = find_nest_with_same_guid(nest_guid,tt_networked_nests);
      };
	};
	if (networked_nest == NULL) { 
		//set_nest_guid(nest_guid); // done by caller now
		//set_proxy(proxy);
      if (addresses == NULL) {
         // new on 181002 since GUID are used locally as well - if no address then no need for DirectPlay
//         register_networked_nest(this);
         non_networked_nests_with_guid = new Sprites(this,non_networked_nests_with_guid);
         return;
      };
		if (proxy()) {
			increment_bird_count();
			if (tt_debug_direct_play) {
				tt_error_file() << "Loaded a long-distance bird";
				if (label != NULL) {
					long length; // was unsigned short int prior to 090202
					string label_string;
					label->current_text(label_string,length);
					if (length > 0) {
						tt_error_file() << " with the label " << label_string;
					};
				};
				tt_error_file() << " at " << timeGetTime() << endl;
				tt_error_file() << " She sends to ";
				describe_networked_nest(tt_error_file());
				tt_error_file() << endl;
			};
		} else if (!tt_loading_a_notebook) { // condition added on 270900
			// shouldn't do this if loading a page of a notebook
			host_a_new_direct_play_session();
			if (tt_debug_direct_play) {
				tt_error_file() << "Loaded ";
				describe_networked_nest(tt_error_file());
				tt_error_file() << " at " << timeGetTime() << endl;
			};
		};
		if (proxy()) { // new on 280400 to merge the proxies
			Nest *another_proxy = find_nest_with_same_guid(nest_guid,tt_nest_proxies);
			if (another_proxy != NULL) {
				another_proxy->merge(this);
			} else {
				tt_nest_proxies = new Sprites(this,tt_nest_proxies);
			};
		};
	} else { // I'm a copy of networked_nest (new on 270400)
      // commented out the following on 041002 since may save this copy and it needs to know its GUID etc
		//delete nest_guid;
		//set_nest_guid(NULL);
		//if (addresses != NULL) {
		//	delete_strings(addresses,address_count);
		//	set_addresses(NULL,0);
		//};
		if (original_nest == networked_nest) {
			// do nothing here (new with ALPHA3)
			// do nothing since already know about this (from XML ordinary GUID links probably)
		} else if (original_nest == this && networked_nest->pointer_to_original_nest() != this) { 
			// new on 181002 - if it has no original nest then the networked nest is it
			// second condition added on 091202 to avoid loops
         networked_nest->add_copy(this);
         set_original_nest(networked_nest);
      } else if (networked_nest->pointer_to_original_nest() == networked_nest) {
         add_copy(networked_nest);
         networked_nest->set_original_nest(this);
      } else if (tt_old_nest_merging_behavior) {
         merge(networked_nest);
      }; // or else what?? - asked 140205
	};
};

#endif

boolean describe_nest(InputStream *pad_in, output_stream &text_out, int version_number, 
						    boolean in_thought_bubble, Article article) {
   if (version_number >= first_version_to_save_copied_nests) {
		describe_comment_encoding(pad_in,text_out,version_number,NEST);
		SpriteType type = (SpriteType) pad_in->get();
		if (type != END_OF_LIST_MARKER) {
			text_out << S(IDS_COVERED_NEST);
			if (!describe_encoding(type,pad_in,text_out,version_number,in_thought_bubble,article,NONE_GIVEN,FALSE,TRUE)) {
				return(FALSE); // something wrong
			};
			type = (SpriteType) pad_in->get();
			while (type != END_OF_LIST_MARKER && !pad_in->empty()) {
				text_out << ", " << S(IDS_AND) << space(); // comma added on 120799
				if (!describe_encoding(type,pad_in,text_out,version_number,in_thought_bubble,article)) {
					return(FALSE); // something wrong
				};
				type = (SpriteType) pad_in->get();
			};
		} else {
			text_out << S(IDS_EMPTY_NEST);
		};
		// and now for nest copies
		type = (SpriteType) pad_in->get();
		while (type != END_OF_LIST_MARKER && !pad_in->empty()) {
			if (!ignore_next_item(type,pad_in,version_number)) {
				return(FALSE); // new on 120101
			};
			type = (SpriteType) pad_in->get();
		};
		if (!ignore_next_item(pad_in,version_number)) { // label might be worth mentioning
			return(FALSE); // new on 120101
		};
//		ignore_growth(pad_in,2);
		// on 070800 replaced the above with the following
		unsigned short int address_and_guid_size;
		pad_in->read((string) &address_and_guid_size,sizeof(address_and_guid_size));
		if (address_and_guid_size > 0) {
			unsigned char first_character_of_address = pad_in->get();
			unsigned char nest_info_version_number = pad_in->get();
			if (first_character_of_address == NEST_NETWORK_INFO_MARKER) { // && nest_info_version_number == 0) { - commented out on 290700 since version 1 fixes size that forgot to include proxy flag
				// I doubt there will be a conflict with old style address
				int address_count = pad_in->get();
				int i;
				for (i = 0; i < address_count; i++) {
					ignore_growth(pad_in,1);
//					unsigned char IP_address_size = pad_in->get();
//					addresses[i] = new char[IP_address_size];
//					pad_in->read(addresses[i],IP_address_size);
				};
				for (i = 0; i < sizeof(GUID); i++) pad_in->get(); // ignore nest_guid
				pad_in->get(); // proxy
			};
		};
	} else {
		pad_in->get(); // index
		unsigned short int stack_size;
		pad_in->read((string) &stack_size, sizeof(stack_size));
		if (stack_size == 0) {
			text_out << S(IDS_EMPTY_NEST);
	//		print_article(article,text_out,S(IDS_EMPTY_NEST));
		} else {
	//		print_article(article,text_out,S(IDS_COVERED_NEST));
			text_out << S(IDS_COVERED_NEST);
			for (int i = 0; i < stack_size; i++) {
				if (!describe_encoding((SpriteType) pad_in->get(),
											  pad_in,text_out,version_number,in_thought_bubble,article)) {
					return(FALSE); // something wrong
				};
				if (stack_size > 1 && i != stack_size-1) {
					text_out << space() << S(IDS_AND) << space();
				};
			};
		};
	};
	return(TRUE);
};

Bird *load_bird(SpriteType type, InputStream *pad_in, int version_number, NaturalLanguage language, long index, Sprite *container, 
					 boolean descendants_inactive) {
	Sprite *comment = NULL;
	if (version_number >= first_version_to_save_copied_nests) {
		comment = load_item(pad_in,version_number,language);
	};
   Bird *bird;
   if (type == BIRD) { // not primitive
		Nest *nest = NULL;
      if (version_number >= first_version_to_save_copied_nests) {
			nest = (Nest *) load_item(pad_in,version_number,language);
		} else {
			pad_in->get();  // ignore the slot for primitives (old style)
			unsigned char nest_index = (unsigned char) pad_in->get();
			if (nest_index != NO_NEST_INDEX) {
				nest = indexed_nest(nest_index);
			};
		};
#if TT_DIRECT_PLAY
		if (nest != NULL && nest->proxy()) {
			Nest *networked_nest = find_nest_with_same_guid(nest->pointer_to_nest_guid(),tt_networked_nests);
			if (networked_nest != NULL) {
//				nest->destroy(); // commented out on 120201 since networked nests aren't deleted until shut down
				nest = networked_nest;
//				networked_nest->increment_bird_count(); // commented out on 120201 since obsolete
			};
		};
#endif
	   bird = new Bird(0,0,0,nest,0,0);
		associate_shared_item(index,bird);
      if (container != NULL) {  // new on 160702
         if (container->kind_of() == PROGRAM_PAD) {
            bird->set_in_notebook((Notebook *) container,FALSE); // no point recuring
         } else if (container->kind_of() == VACUUM) {
            bird->set_inside_vacuum(TRUE,FALSE);
         };
      }; 
	   if (nest != NULL) {
		   nest->increment_bird_count();
#if TT_DEBUG_ON
         if (tt_debug_mode == 13579) {
            nest->print(tt_error_file());
            tt_error_file() << " incrementing bird count due to load_bird ";
            bird->print(tt_error_file());
            tt_error_file() << endl;
         };
#endif
      };
	} else { // primitive bird
		short int handle_index;
		if (version_number >= first_version_to_save_copied_nests) {
			pad_in->read((string) &handle_index, sizeof(handle_index));
		} else {
			handle_index = pad_in->get();
		};
		PrimitiveBird *creator = (PrimitiveBird *) load_item(pad_in,version_number,language);
		associate_shared_item(index,creator);
		Sprite *message = load_item(pad_in,version_number,language);
      // t-shirt handled below
		if (creator != NULL) {
			// use receiver of creator and message to recreate bird
			bird = creator->reconstruct(message,handle_index);
			creator->destroy();
			creator = NULL; // new on 150303
		} else { // warn if bird is NULL after attempting to reconstruct it?
			bird = NULL;
		};
		if (message != NULL) {
			message->destroy();
			message = NULL; // new on 150303
		};
		if (bird == NULL) {
         bird = new PrimitiveBird(receive_extension_name); // just the initial one that opens libraries
		};
   };
   if (version_number > 6) {
      Sprite *t_shirt = load_item(pad_in,version_number,language);
      if (t_shirt != NULL) {
         bird->add_t_shirt(t_shirt);
      };
   };
	bird->set_comment(comment);
	return(bird);
};

boolean describe_bird(SpriteType type, InputStream *pad_in, output_stream &text_out, int version_number, 
						    boolean , Article article) {
	if (version_number >= first_version_to_save_copied_nests) {
		describe_comment_encoding(pad_in,text_out,version_number,BIRD);
	};
   if (type == BIRD) {
		if (version_number < first_version_to_save_copied_nests) {
			pad_in->get(); // primitive
			pad_in->get(); // nest index
		} else {
			if (!ignore_next_item(pad_in,version_number)) { // nest
				return(FALSE); // new on 120101
			};
		};
	   print_type(BIRD,text_out,article); 
   } else if (type == PRIMITIVE_BIRD) {
	   text_out << S(IDS_A_PRIMITIVE_BIRD); // ok to ignore article?
		// could use the following to generate a better description
		if (version_number >= first_version_to_save_copied_nests) {
			short int handle_index;
			pad_in->read((string) &handle_index, sizeof(handle_index)); // ignore handle_index
		} else {
			pad_in->get();
		};
		if (!ignore_next_item(pad_in,version_number) || // creator
			 !ignore_next_item(pad_in,version_number)) { // message
				return(FALSE); // new on 120101
		};
   };
	if (!ignore_next_item(pad_in,version_number)) { // t_shirt
		return(FALSE); // new on 120101
	};
	return(TRUE);
};

Nest::Nest(city_coordinate center_x, city_coordinate center_y,
			  long priority,
//			  Background *floor,
			  city_coordinate width,
			  city_coordinate height,
			  short parameter,
			  Sprites *contents_stack) :
	Sprite(NEST_SPRITE,
		    center_x,center_y,NEST,
		    parameter,ground_scale,//FALSE,
		    width,height),
	contents_stack(contents_stack),
	stack_temporarily_removed(FALSE),
	display_new_contents(FALSE),
	new_contents(NULL),
	bird_count(0),
//   covered(FALSE),
	waiting_birds(NULL),
   hatching_bird(NULL),
//	saved_by(NULL),
//	saved_status(NULL),
	deleted_flag(FALSE),
	robot_to_try(NULL),
	cubby_to_try_robot_on(NULL),
	nest_copies(NULL),
	label(NULL),
#if TT_DIRECT_PLAY
	nest_guid(NULL),
	direct_play(NULL),
	alternative_direct_plays(NULL),
	player_id(0),
//	player_address(NULL), // updated later
   addresses(NULL),
	address_count(0),
	proxy_flag(FALSE),
	dump_as_proxy_flag(FALSE), // new on 050704 since can break time travel because nest is saved without geometry
	pending_messages(NULL),
	pending_message_count(0),
	attempts_to_join_session(0),
	nth_address(0),
	direct_play_count(-1), // negative means it is unknown
#endif
	forwarding_flag(FALSE),
	forwarding_to_nest(NULL),
	original_nests_forwarding_here(NULL) {
	current_priority = priority;
	original_nest = this; // tried set_original_nest on 220203 but then need to initialize original_nest to NULL
//	increment_ref_count(); // new on 220203 -- commented out on 050104 since set_original_nest now treats specially self pointers
//	set_priority_function_of_lly(FALSE);
//	set_parameter(parameter);
	if (contents_stack != NULL) {
		add_follower(contents_stack->first());
//		contents_stack->first()->increment_ref_count(this); // new on 070999 to fix bug introduced on 170799
		contents_stack->increment_ref_count(this); // new on 081100 is that every element has its ref counted increased
	};
//	Sprites *remaining = contents_stack;
//	while (remaining != NULL) {
//		add_follower(remaining->first());
//		remaining = remaining->rest();
//	};
//	true_size_flag = TRUE;
};

//Sprites *nests_being_copied = NULL;

//void Nest::prepare_for_deletion() {
//   if (preparing_for_deletion) return; // already done
//	preparing_for_deletion = TRUE; // new on 010503 to deal with recursive calls (e.g. from deleting pending_messages)
//#if TT_DEBUG_ON
//   if (tt_debug_mode == 13579) {
//      print(tt_error_file());
//      if (original_nest == this) {
//         tt_error_file() << " being deleted whose bird count is " << bird_count << endl;
//      } else {
//         tt_error_file() << " being deleted. Is a copy of ";
//         if (original_nest != NULL) original_nest->print(tt_error_file());
//         tt_error_file() << endl;
//      };
//   };
//	if (tt_debug_target == debug_counter) {
//		log("Preparing nest debug target for deletion.");
//	};
//#endif
///*
//  if (waiting_birds != NULL) {
//	  waiting_birds->remove_all();
//	  delete waiting_birds;
//  };
//*/
//  // only the first guy is a follower
////	Sprites *remaining = contents_stack;
////	while (remaining != NULL) {
////		remove_follower(remaining->first());
////		remaining = remaining->rest();
////	};
//};

void Nest::release_all_references() { // new on 130504
	if (releasing_all_references) { // new on 230305
		return;
	};
	boolean saved_releasing_all_references = releasing_all_references;
	releasing_all_references = TRUE; // new on 030405 since can be called recursively and crash
#if TT_DEBUG_ON
	debug_log(debug_counter,"Preparing nest debug target for deletion.");
#endif
	if (contents_stack != NULL) { // since not an item of the floor
//		if (!stack_temporarily_removed) {// && !tt_shutting_down) {
//			remove_follower(contents_stack->first());
//		};
		// following is new on 090499 since with non-rectangular pictures can have more than one stack element as follower
		Sprites *remaining = contents_stack;
		while (remaining != NULL) {
			Sprite *sprite = remaining->first();
			Sprites *next_remaining = remaining->rest(); // bound here in case the following changes this list (new on 280201)
			sprite->destroy(this); // new on 271000 since incremented when added to nest
			if (tt_shutting_down || sprite->pointer_to_leader() == this) {
				remaining->set_first(NULL); // is a follower so will be deleted elsewhere
			};
			remaining = next_remaining;
		};
		/* old way prior to 090499
		Sprites *contents_stack_rest = contents_stack->rest(); // first is a follower and handled elsewhere
      contents_stack->set_rest(NULL);
      destroy_sprites(contents_stack_rest);
      contents_stack->set_first(NULL);
		*/
//#if TT_DEBUG_ON
//		if (tt_debug_mode != 160205) // experiment on 090305
//#endif
      delete contents_stack; 
		contents_stack = NULL; // new on 280201
//		if (contents_stack != NULL) {
//			delete contents_stack;
//			contents_stack = NULL;
//		};
//		Sprites *remaining = contents_stack;
//		while (remaining != NULL) {
//			destroy(remaining->first());
//			remaining = remaining->rest();
//		};
	};
	boolean keep_around_for_time_travel = release_nest_connections();
	if (!tt_resetting_or_destroying_city && robot_to_try != NULL) {
		robot_to_try->remove_suspension();
	};
	robot_to_try = NULL; // new on 130504
	// tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 but can cause memory errors while resetting during time travel
//	prepared_for_deletion_flag = FALSE; // restore so the following works OK -- replaced by preparing_for_deletion on 060204
	releasing_all_references = saved_releasing_all_references; // new on 030405
	if (keep_around_for_time_travel) {// || (!tt_old_nest_merging_behavior && original_nest != this)) { 
		// second condition new on 140205 so if copies kept by non-original then released here -- removed 140205 as well
		destroy_sprites(nest_copies); // new on 140603 prior to 150205 was just_destroy_list
	} else {
		Sprite::release_all_references();
	};
	if (!tt_resetting_or_destroying_city && original_nest != this && original_nest != NULL) { // new on 050104
		original_nest->remove_copy(this);
	};
	if (original_nest != this) { // OK to still refer to itself -- maybe prevent this from being prematurely deleted
		original_nest = NULL; // new on 130504
	};
	label = NULL; // nwe on 130504 -- is a follower so will be reclaimed
	if (tt_old_nest_merging_behavior) {
		set_forwarding(FALSE); // new on 160904 so unbamming nest merging doesn't leave things broken
	} else {
		set_forwarding_to(NULL);
	};
	destroy_sprites(original_nests_forwarding_here); // new on 140205 -- prior to 150205 was just_destroy_list
//	Sprite::release_all_references(); // conditionally done above
};

boolean Nest::release_nest_connections(Sprite *by) { // abstracted on 240903
//#if TT_DEBUG_ON
//	if (tt_debug_mode == 160205) {// experiment on 090305
//		return(FALSE);
//	};
//#endif
	release_nest_indices(); // just in case -- really needed???
//	if (nests_being_copied != NULL) {
//		nests_being_copied = nests_being_copied->remove(this); // just in case
//	};
	// following commented out on 160499 since this is taken care of in nest::ok_to_delete()
//	if (original_nest != this && original_nest != NULL) {
//		original_nest->remove_copy(this);
//	};
#if TT_DIRECT_PLAY
	// re-aranged this on 280400 so cleanup of the following happens before deleting nest_guid
	if (pending_messages != NULL) { // new on 030500
		// worth trying to save these??
		tt_error_file() << (pending_messages->length()+message_queue_size(direct_play)) << " messages to ";
		describe_networked_nest(tt_error_file());
		tt_error_file() << " were never sent and are lost now. Sorry." << endl;
//		if (player_address != NULL) {
//			int address_count;
//			string *IP_addresses = IP_addresses_of_player(player_address,player_address_size,address_count);
		if (address_count > 0) {
			tt_error_file() << "Probably couldn't connect to ";
			for (int i = 0; i < address_count; i++) {
				tt_error_file() << addresses[i];
				if (i+1 < address_count) tt_error_file() << " or ";
			};
			tt_error_file() << endl;
//			delete_strings(IP_addresses,address_count);
		};
		//// new on 010503 to save it first in case while destroying any of the messages this is called recursively
		//Sprites *pending_messages_saved = pending_messages;
		//pending_messages = NULL;
		destroy_sprites(pending_messages,this);
	};
	boolean keep_around_for_time_travel = FALSE; // new on 140603
	if (proxy()) {
		if (tt_nest_proxies != NULL) {
			tt_nest_proxies = tt_nest_proxies->remove(this);
		};
		destroy_player(direct_play,player_id); // new on 050500
	} else {
		if (tt_networked_nests != NULL) {
			if (tt_resetting_or_destroying_city && tt_time_travel != TIME_TRAVEL_OFF) { // new on 140603 to avoid remaking this while time travelling
				keep_around_for_time_travel = (tt_networked_nests->member_cons(this) != NULL);
			} else {
				tt_networked_nests = tt_networked_nests->remove(this);
			};
		};
	};
	if (!keep_around_for_time_travel) {
		if (nest_guid != NULL) {
#if TT_DEBUG_ON
			if (tt_debug_mode == 160205) { 
				tt_error_file() << "Nest#" << debug_counter << " ";
				print_guid(nest_guid,tt_error_file());
				tt_error_file() << endl;
			}; //  else				
#endif
			delete nest_guid;
			nest_guid = NULL; // new on 131003
		};
//		if (player_address != NULL) delete [] player_address;
		delete_strings(addresses,address_count);
		if (direct_play != NULL) release_direct_play_object(direct_play);
		if (alternative_direct_plays != NULL) { // new on 010500
			release_alternative_direct_plays();
		};
	};
#endif
//#if TT_DEBUG_ON
//	if (tt_debug_mode == 160205) {// experiment on 090305
//		return(keep_around_for_time_travel);
//	};
//#endif
	if (nest_copies != NULL) {
		if (tt_reclaiming_memory) { // copies will be deleted normally if shutting down - was !tt_shutting_down prior to 190103
			Sprites *remaining = nest_copies; // should be NULL for all but original
			while (remaining != NULL) {
				Sprites *next_remaining = remaining->rest(); // bind now in case altered below
				Nest *nest = (Nest *) remaining->first();
				nest->set_original_nest(NULL);
				// might this be a leak? -- these aren't ref counted so should be OK, right?
//				nest->destroy(by); // commented out on 260204 since if giving a box with nest copies to a untrained robot this is called
				remaining->set_first(NULL);
				remaining = next_remaining;
			};
		} else {
			nest_copies->remove_all();
		};
//#if TT_DEBUG_ON
//		if (tt_debug_mode != 160205) // experiment on 090305
//#endif
		delete nest_copies;
      nest_copies = NULL;
	};
	if (original_nest != NULL && !tt_resetting_or_destroying_city) {
		// following copied here from :ok_to_delete (need it be there still?) on 131003
		// second condition added on 060205 since everything is being destroyed and this caused a time travel crash
		Nest *saved_original_nest = original_nest;
		original_nest = NULL; // so that the following doesn't recur to here again
		saved_original_nest->remove_copy(this);
	};
	return(keep_around_for_time_travel);
};

boolean Nest::ok_to_delete() {
#if TT_DIRECT_PLAY
	if (proxy() != NULL) { // this was just proxy prior to 230103! This was always TRUE!
		// simplified on 190103 and now returns TRUE when shutting down regardless of ref count
		return(tt_shutting_down && tt_reclaiming_memory); 
	}; // new on 120201 since networked nests aren't destroyed until shut down now
#endif
	if (ref_count > 1) { // will just decrement ref count and don't need to do the rest (added 051199)
		return(Sprite::ok_to_delete());
	};
	return(bird_count == 0 && original_nests_forwarding_here == NULL); 
	// was FALSE from 150504 to 140205 but why not delete it if no birds and no other ref count?
};

/*
	if (original_nest == this) {
#if TT_DEBUG_ON
      if (tt_debug_mode == 13579) {
         print(tt_error_file());
         tt_error_file()  << " checking if OK to delete original nest whose bird count is " << bird_count << endl;
      };
#endif
		if (bird_count > 0) { // && !proxy()) { // second disjunct added 030500
			// bird_count is set to max_long when networked so will they ever be recovered?
			// birds are still out there -- last one should delete this
			deleted_flag = TRUE;
			remove_from_floor(FALSE); // but my floor might be destroyed too
			if (contents_stack != NULL) {
				if (!stack_temporarily_removed) {
				   // since not an item of the floor
				   remove_follower(contents_stack->first());
				   contents_stack->first()->destroy(this);
					contents_stack->first()->destroy(this); // new on 170799 since ref count has been incremented 
				}; // else first item has already been removed and put on the floor
				contents_stack = contents_stack->rest(); // first is a follower and just removed
			};
			if (contents_stack != NULL) {
				Sprites *remaining = contents_stack;
				while (remaining != NULL) {
					remaining->first()->destroy(this); // new on 170799 to decrement ref count
					remaining = remaining->rest();
				};
				delete contents_stack;
				contents_stack = NULL;
			};
			return(FALSE);
		} else {
			return(Sprite::ok_to_delete()); // don't do this is postponed -- as in above
		};
		// commented out the following on 050104 since responsibility of removing self from
	//} else if (original_nest != NULL && !forwarding()) { // new on 160499 - !forwarding() added on 190799
	//	// original nest is holding on to me so make him let go
	//	// should this check if ref_count is 2??
	//	Nest *saved_original_nest = original_nest;
	//	original_nest = NULL; // so that the following doesn't recur to here again
	//	saved_original_nest->remove_copy(this);
	};
	return(Sprite::ok_to_delete()); // ok to delete a copy
};
*/

#if TT_DIRECT_PLAY
void Nest::release_alternative_direct_plays() {
	if (alternative_direct_plays != NULL) { // new on 010500
		int number_of_alternatives;
		if (direct_play_count < 0) {
			number_of_alternatives = nth_address;
		} else {
			number_of_alternatives = direct_play_count-1;
		};
		for (int i = 0; i < number_of_alternatives; i++) {
			alternative_direct_plays[i]->Release();
		};
		delete [] alternative_direct_plays;
		alternative_direct_plays = NULL;
		direct_play_count = 1; // new on 050500
		nth_address = 0;
	};
};

void Nest::drop_direct_play_object() {
	release_direct_play_object(direct_play);
	direct_play = NULL;
	player_id = 0;
	direct_play_count = -1;
};

#endif

int Nest::current_bird_count() {
	if (original_nest == this) {
		return(bird_count);
	} else if (original_nest != NULL) { // could it be otherwise?
		return(original_nest->current_bird_count());
	} else {
		return(bird_count); // or error? or warning?
	};
};

void Nest::increment_bird_count() {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Incrementing bird count of debug target.");
	};
#endif
#if TT_DIRECT_PLAY
	if (nest_guid != NULL) return; // new on 050201 since bird_count is meaningless for networked birds
#endif
	if (original_nest == this) {
		set_parameter(NEST_EMPTY); 
		// just in case -- may be just copied -- moved here on 010500 so applies even if bird_count == max_long
		if (bird_count == max_long) return; // new on 270400
		bird_count++;
		Sprites *remaining = nest_copies;
		while (remaining != NULL) {
			remaining->first()->set_parameter(NEST_EMPTY);
			remaining = remaining->rest();
		};
//	} else if (original_nest != NULL) { // could it be otherwise?
//		original_nest->increment_bird_count();
	};
};

void Nest::decrement_bird_count() {
   if (original_nest != this) return; // bird counts are only for the "original"
	if (tt_resetting_or_destroying_city) return; // not keeping track if the city is going away -- new on 100305
	if (bird_count == max_long) return; // new on 270400
#if TT_DIRECT_PLAY
	if (nest_guid != NULL) return; // new on 050201 since bird_count is meaningless for networked birds
#endif
	bird_count--;
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Decrementing bird count of debug target.");
	};
   if (bird_count < 0) {
      tt_error_file() << "Bird count is negative ";
      print(tt_error_file());
      tt_error_file() << endl;
		debug_this();
   };
   if (tt_debug_mode == 13579) {
      print(tt_error_file());
      tt_error_file() << " decrementing bird count which is now " << bird_count << " and deleted_flag is " << (int) deleted_flag << endl;
   };
#endif
	if (bird_count == 0) {
		if (deleted_flag) {
			destroy();
//		} else {
//			set_parameter(0); // just a visual way of seeing that no birds left
//        set_clean_status(DIRTY);
		};
	};
};

//void release_nests_being_copied() {
	// called after a "top level" copy
//	if (nests_being_copied != NULL) {
//		nests_being_copied->remove_all();
//		delete nests_being_copied;
//      nests_being_copied = NULL;
//	};
//};

void Nest::set_contents_stack(Sprites *stack) { // new on 250203
	if (contents_stack != NULL) {
		// new on 140205
		remove_follower(contents_stack->first());
		contents_stack->decrement_ref_count();
	};
	contents_stack = stack;
	if (contents_stack != NULL) { // copied here from nest::nest on 250203
		add_follower(contents_stack->first());
		contents_stack->increment_ref_count(this); // new on 081100 is that every element has its ref counted increased
	};
};

Sprite *Nest::copy(boolean also_copy_offsets) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Copying nest debug target.");
	};
#endif
// following now handled by now_inside_thought_bubble(in_training)
//	if (contents_stack != NULL && 
//		 tt_copying_entire_thought_bubble) {// ||
////		  (bird_count == 0 && contents_stack->rest() == NULL))) {
		// nests without birds copy as their contents?
//		Sprite *replacement = contents_stack->first()->copy(floor);
////		give_size_and_location_to_cover_to(replacement);
//		return(replacement);
//	} else {
      // this searches a list whose format is (item1 value1 item2 value2 ...)
//		Sprites *cons = nests_being_copied;
//		while (cons != NULL) {
//			if (cons->first() == this) {
//				break;
//			};
//			cons = cons->rest()->rest();
//		};
		Nest *nest_copy = (Nest *) already_copied(this);
		if (nest_copy == NULL) {
			nest_copy = new Nest(llx,lly, //center_x,center_y,
										current_priority-1,
										width,height,
//										(bird_count == 0)?(short) NEST_INERT:parameter,
										parameter);
//										copy_contents_stack(contents_stack)); 
			// removed on 250203 to avoid infinite recursion - copied after call to just_copied now
//			if (saved_width != 0) {
//				nest_copy->set_saved_size(saved_width,saved_height);
//			};
			if (!infinite_stack()) {
//				nests_being_copied = new Sprites(this,new Sprites(nest_copy,nests_being_copied));
            just_copied(this,nest_copy);
				if (contents_stack != NULL) {
					nest_copy->set_contents_stack(contents_stack->copy());
				};
				if (current_bird_count() > 0 && !tt_dont_connect_things_while_copying) { //!tt_copying_entire_thought_bubble &&
					// commented out the following on 200704 since it misbehaves if nests are already copied --
					// e.g. [bird | nest | nest_copy]
//					 !contained_in(PROGRAM_PAD)) { // new on 120399 since otherwise copies where kept incorrectly when taken from pad
//                && original_nest != nest_copy) {
				   // copying a nest which already has a bird
//					Sprite *top_container = ultimate_container();
//				   Sprite *original_nest_top_container = NULL;
//					if (original_nest != NULL && original_nest->part_of_top_level_copy()) {
//						original_nest_top_container = original_nest->ultimate_container();
//					};
//					if (top_container == original_nest_top_container) {
						// both nest and original nest being copied as a unit -- added on 230299
//						Nest *original_nest_copy = (Nest *) (original_nest->copy());
//						if (original_nest_copy != nest_copy) {
//							original_nest_copy->add_copy(nest_copy);
//							nest_copy->set_original_nest(original_nest_copy);
//						};
//					} else {
					if (original_nest != nest_copy) {
						Nest *original_nest_copy = (Nest *) already_copied(original_nest);
						//if (original_nest_copy == NULL && tt_top_copy != NULL) { // commented out as experiment on 050104
						//	// added on 191199 shouldn't have connections with "outside world" so copy now
						//	original_nest_copy = (Nest *) original_nest->copy();
						//	just_copied(original_nest,original_nest_copy); // for future references
						//};
						if (original_nest_copy != NULL) {
							if (copied_bird_has_as_original_nest(original_nest_copy)) { 
								// tried to replace copied_bird_has_as_original_nest(original_nest_copy) with original_nest != this on 140804
								// since copies of copies share nests incorrectly... (unless bird is encountered first...)
								// added on 230299 so that copies of non-original still share birds correctly
								original_nest_copy->add_copy(nest_copy);
								nest_copy->set_original_nest(original_nest_copy);
	//							nest_copy->set_bird_count(bird_count);
							} else {
								// updated on 180804 since it is too early to decide between these alternatives
								// a bird to this nest may or may not be copied later
								tt_nests_to_set_original_nest
									= new Sprites(nest_copy,new Sprites(original_nest_copy, new Sprites(original_nest,tt_nests_to_set_original_nest)));
							};
						} else {
							original_nest->add_copy(nest_copy);
							nest_copy->set_original_nest(original_nest);
//							nest_copy->set_bird_count(bird_count); // added on 230299 so that copies of non-original still share birds correctly
						};
					//} else if (!tt_old_nest_merging_behavior) {
					//	add_copy(nest_copy); // new on 140205
					};
					if (original_nests_forwarding_here != NULL) { // new on 140205
						Sprites *remaining = original_nests_forwarding_here; 
						while (remaining != NULL) {
							Nest *nest = (Nest *) (remaining->first());
							nest->add_copy(nest_copy);
							remaining = remaining->rest();
						};
						nest_copy->set_original_nests_forwarding_here(original_nests_forwarding_here->just_copy_list(TRUE));
					};
				};
			};
//		} else { // already copied
//#if TT_DEBUG_ON
//			log("Nest already copied.");
//#endif
			// commented out on 230299 since is now handled more generally in bird::copy(boolean also_copy_offsets)
//			if (nest_copy->pointer_to_original_nest() == this) { // I'm the original
//				remove_copy(nest_copy);
//				nest_copy->set_original_nest(nest_copy);
//			};
		};
  //    nest_copy->set_priority_fixed(priority_fixed_flag);
		//if (comment != NULL) {
		//	nest_copy->set_comment(comment->copy());
		//};
      finish_making_copy(nest_copy,also_copy_offsets);
		if (label != NULL) {
			nest_copy->set_label((Text *) label->copy(TRUE));
		};
		return(nest_copy);
//	};
};

void process_tt_nests_to_set_original_nest() {
	// called when copy is finished so all of these nests were copied without their bird
	// list is a triple: nest, original_nest_copy, and original_nest
	Sprites *remaining = tt_nests_to_set_original_nest;
	while (remaining != NULL) {
		Nest *nest_copy = (Nest *) remaining->first();
		if (nest_copy != NULL) {
			remaining->set_first(NULL); // to collect just the list
			remaining = remaining->rest();
			// skip the original_nest_copy
			remaining->set_first(NULL); 
			remaining = remaining->rest();
			Nest *original_nest = (Nest *) remaining->first();
			remaining->set_first(NULL); 
			remaining = remaining->rest();
			original_nest->add_copy(nest_copy);
			nest_copy->set_original_nest(original_nest);
			// else warn?
		};
	};
	delete tt_nests_to_set_original_nest;
	tt_nests_to_set_original_nest = NULL;
};

void Nest::bird_just_copied() {
	// list is a triple: nest, original_nest_copy, and original_nest
	Sprites *remaining = tt_nests_to_set_original_nest;
	Sprites *previous_remaining; // no need to initialize since will only be used if set earlier
	while (remaining != NULL) {
		Nest *saved_nest_copy = (Nest *) remaining->first();
		Sprites *current_remaining = remaining->rest();
		Nest *saved_original_nest_copy = (Nest *) current_remaining->first();
		current_remaining = current_remaining->rest();
		Nest *saved_original_nest = (Nest *) current_remaining->first();
		if (saved_original_nest == original_nest) {
			saved_original_nest_copy->add_copy(saved_nest_copy);
			saved_nest_copy->set_original_nest(saved_original_nest_copy);
			// splice this triple out
			if (remaining == tt_nests_to_set_original_nest) { // first one
				tt_nests_to_set_original_nest = current_remaining->rest(); // starting with the next triple
				current_remaining->set_rest(NULL); // to reclaim this triple
				just_destroy_list(remaining); // only the initial triple
				// keep searching even if found one since may be more nest copies
				remaining = tt_nests_to_set_original_nest;
			} else {
				Sprites *triple = remaining;
				remaining = current_remaining->rest();
				previous_remaining->rest()->rest()->set_rest(remaining); // spliced out
				current_remaining->set_rest(NULL); // to reclaim this triple make it contain only the 3 items
				just_destroy_list(triple); // rewritten on 220804
				// keep searching even if found one since may be more nest copies
			};
		} else {
			previous_remaining = remaining;
			remaining = current_remaining->rest();
		};
	};
};

Sprites *copy_contents_stack(Sprites *contents_stack) {
   if (contents_stack == NULL) return(NULL);
   return(contents_stack->copy());
};
//  return(new Sprites(contents_stack->first()->copy(floor),
//							copy_contents_stack(contents_stack->rest(),floor)));
//};

void Nest::inside_thought_bubble() { // different from the following but rationalize someday??
   set_parameter(NEST_EMPTY);
   Sprite::inside_thought_bubble();
};

Sprite *Nest::now_inside_thought_bubble(boolean in_training) {
	release_nest_connections(); // new on 240903
	bird_count = 0; // new on 131003
	set_guid(NULL); // new on 151003 to sever any connections
	if (nest_guid != NULL) { // will be released by release_nest_connections unless kept for time travel
		delete nest_guid; // unregister too??
		nest_guid = NULL;
	};
   if (contents_stack != NULL && leader != NULL) { // in thought bubble replace with top
		// added leader != NULL test on 160405 but not clear if this is just a symptom of something else broken
//      Sprite *replacement = contents_stack->first();
//      contents_stack->set_first(NULL);
      Sprite *replacement;
      contents_stack = contents_stack->pop(replacement); 
      remove_follower(replacement);
		leader->add_follower(replacement);
		leader->remove_follower(this);
		replacement->set_size_and_location(width,height,llx,lly);
		replacement->destroy(); // new on 170799 to decrement ref count
      destroy_sprites(contents_stack);
		replacement->now_inside_thought_bubble(in_training); // new on 160405
		// problem with the following is if bird is also in the same box
//		destroy(); // commented out on 130504 as experiment -- maybe should be deleted when robot is finished being trained?
		return(replacement);
	} else {
		become_blank(); // new on 101199 so labels don't show when in thought bubble
		return(this);
	};
};

Nest *Nest::ultimate_original_nest() {
	if (original_nest == this) return(this);
	return(original_nest->ultimate_original_nest());
};

Nest *Nest::ultimate_forwarding_to() { // new on 140205
	if (forwarding_to_nest == NULL) return(this);
	return(forwarding_to_nest->ultimate_forwarding_to());
};

Nest *Nest::ultimate_forwarding_to_or_original_nest() { // new on 140205
	if (forwarding_to_nest == NULL || original_nest == this) return(this);
	return(forwarding_to_nest->ultimate_forwarding_to_or_original_nest());
};

void Nest::set_forwarding_to(Nest *nest) { // new on 140205
	if (forwarding_to_nest != NULL) {
		// this could happen while unbamming
		forwarding_to_nest->destroy(); // release
	};
	if (nest != NULL) {
		forwarding_to_nest = nest->ultimate_forwarding_to_or_original_nest(); // no point building long links
		forwarding_to_nest->increment_ref_count();
		if (contents_stack != NULL) { // new on 140205 to transfer stack
			Sprites *my_contents_stack = contents_stack;
			set_contents_stack(NULL);
			forwarding_to_nest->set_contents_stack(my_contents_stack);
		};
		forwarding_to_nest->add_to_original_nests_forwarding_here(pointer_to_original_nest());
	} else {
		forwarding_to_nest = NULL;
	};
};

void Nest::add_to_original_nests_forwarding_here(Nest *nest) {
	if (nest == NULL) {
		return; // OK?
	};
	nest->increment_ref_count(); // new on 150205
	insert_at_end_if_not_member(nest,original_nests_forwarding_here);
};

void Nest::set_original_nest(Nest *nest) { // called with NULL before deleting
	// maintains ref counts beginning on 210203
	if (nest == original_nest) { // already knew it - new on 220203
		return; 
	};
//#if TT_BUG_FIXES // commented out on 031203 since broke copying a fresh nest/bird pair (but not a bird/nest pair)
//	// moved after the above test on 031203 since this broke copying a fresh nest/bird pair (but not a bird/nest pair)
//	if (nest != NULL) {
//		nest = nest->ultimate_original_nest(); // new on 011203 to keep the chains short and simple
//		if (nest == original_nest) { // copied here on 031203
//			return; 
//		};
//	};
//#endif
#if TT_DEBUG_ON
	Nest *next_nest = nest;
	while (next_nest != NULL) {
		Nest *next_next_nest = next_nest->pointer_to_original_nest();
		if (next_next_nest == next_nest) break;
		if (next_next_nest == this) {
			debug_this("About to create a cycle of original_nest pointers!");
		};
		next_nest = next_next_nest;
	};
#endif
	if (original_nest != this && original_nest != NULL) { // added original_nest != this since otherwise isn't reclaimed
		original_nest->destroy(); // if this is last reference this should remove copy from original_nest (if != this)
	};
   original_nest = nest;
	if (original_nest != this && original_nest != NULL) { // added original_nest != this since otherwise isn't reclaimed
		original_nest->increment_ref_count();
	};
};

boolean Nest::ignore_me(boolean ignore_those_in_limbo) {
	// I wonder if some of this could be optimized - would it be worth it?
	return(deleted() || slated_for_deletion() || // forwarding() || -- commented out on 301101
			 // following added 012900 since if nest has no leader and no background then ignore it
			 // perhaps it could be removed from the list but could it be in transition in some way??
			 // removed inside_vacuum() since shouldn't be treated specially if saving a city - 280403
			 (ignore_those_in_limbo && pointer_to_leader() == NULL && pointer_to_background() == NULL
#if TT_DIRECT_PLAY
			  && !proxy()
#endif
			  && !held() // don't ignore nests if held by programmer (or a robot)
			 ) ||
			 contained_in_robots_thought_bubble() // ||
//			 contained_in(THOUGHT_BUBBLE) // this added on 140200
//        following moved out of here on 100504 since only should apply when delivering messages not when saving
//			 (pointer_to_leader() != NULL && (pointer_to_leader()->in_notebook() || ultimate_leader()->kind_of() == PROGRAM_PAD)) // new on 120104
				 // on 290404 added the in_notebook() test - shouldn't really need both but it seems to help
				 // on 290404 replaced pointer_to_leader with ultimate_leader 
			);
};

boolean Nest::in_a_notebook() { // abstracted and updated on 100504
	if (leader == NULL) return(FALSE);
	Sprite *container = ultimate_leader();
	return(container->kind_of() == PROGRAM_PAD);
};

Sprites *Nest::all_nests(boolean ignore_those_in_a_notebook, boolean ignore_those_in_limbo, 
								 Sprites *initial_nests, Sprites *nests_already_processed) {
	// ignore_those_in_a_notebook new on 100504 since if sending messages should be ignored but not if saving
	// ignore_those_in_limbo new on 130505 since when saving a city nests in Dusty will be in limbo but need to be saved in case spit out again
	if (original_nest == this) {
		Sprites *nests = initial_nests;
		Sprites *remaining = nest_copies;
		while (remaining != NULL) {
			Nest *nest = (Nest *) remaining->first();
			Nest *nests_original_nest = nest->pointer_to_original_nest();
			if (nests_already_processed == NULL || nests_already_processed->member_cons(nest) == NULL) {
				if (nest->forwarding()) { // must be old behavior or loaded something in the middle of old nest forwarding behavior
					// new on 301101
					Sprites *new_nests_already_processed = new Sprites(nest,nests_already_processed);
//					new_nests_already_processed = new Sprites(this,new_nests_already_processed); // new on 121103 to prevent loops
					nests = nests_original_nest->all_nests(ignore_those_in_a_notebook,ignore_those_in_limbo,nests,new_nests_already_processed);
					new_nests_already_processed->set_first(NULL);
//					new_nests_already_processed->rest()->set_first(NULL); // new on 121103
//					new_nests_already_processed->rest()->set_rest(NULL);
					new_nests_already_processed->set_rest(NULL);
					delete new_nests_already_processed;
				} else if (!tt_old_nest_merging_behavior && nest->forwarding_to() != NULL) {
					// new on 140205
					Nest *end_of_the_line = nest->ultimate_forwarding_to();
					if (!end_of_the_line->ignore_me(ignore_those_in_limbo) && (!ignore_those_in_a_notebook || !end_of_the_line->in_a_notebook())) {
						insert_at_end_if_not_member(end_of_the_line,nests);
					};
				} else if (!nest->ignore_me(ignore_those_in_limbo) && (!ignore_those_in_a_notebook || !nest->in_a_notebook())) {
					// on 190799 added !deleted() && !slated_for_deletion() && !forwarding()
					// commented out the following on 140205 since this doesn't make sense and creates asymmetries 
					// (unless overridden by setting tt_old_nest_merging_behavior)
					// I am assuming the only way the following could happen is if two nests were bammed together
					if (tt_old_nest_merging_behavior && nests_original_nest == nest) { // new on 301101 to include its nests as well 
						Sprites *new_nests_already_processed = new Sprites(nest,nests_already_processed);
						nests = nest->all_nests(ignore_those_in_a_notebook,ignore_those_in_limbo,nests,new_nests_already_processed);
						new_nests_already_processed->set_first(NULL);
						new_nests_already_processed->set_rest(NULL);
						delete new_nests_already_processed;
					} else {
//#if TT_ALPHA5 // commented out 311003 since this caused bird/nest connection problems and now release_outgoing_connections fixes the F11 problem
//						insert_at_end_if_not_member_or_same_nest_guid(nest,nests); // at least while unbamming can get duplication - new 211003
//#else
						insert_at_end_if_not_member(nest,nests);
//#endif
					};
				};
			};
			remaining = remaining->rest();
		};
		// in the following replaced insert_at_end_if_not_member with insert_at_beginning_and_remove_duplicates on 140205
		if (!tt_old_nest_merging_behavior && forwarding_to() != NULL) {
			// new on 140205
			Nest *end_of_the_line = ultimate_forwarding_to();
			if (!end_of_the_line->ignore_me(ignore_those_in_limbo) && (!ignore_those_in_a_notebook || !end_of_the_line->in_a_notebook())) {
				if (tt_old_nest_merging_behavior) {
				insert_at_end_if_not_member(end_of_the_line,nests); 
			} else {
				insert_at_beginning_and_remove_duplicates(end_of_the_line,nests);
			};
			};
		} else if (forwarding()) { // must be old behavior or loaded something in the middle of old nest forwarding behavior
//#if TT_ALPHA5 // commented out 311003 since this caused bird/nest connection problems and now release_outgoing_connections fixes the F11 problem
//			insert_at_end_if_not_member_or_same_nest_guid(pointer_to_original_nest(),nests); // at least while unbamming can get duplication - new 211003
//#else
			if (tt_old_nest_merging_behavior) {
				insert_at_end_if_not_member(pointer_to_original_nest(),nests); 
			} else {
				insert_at_beginning_and_remove_duplicates(pointer_to_original_nest(),nests);  // original first is important
			};
//#endif
      } else if (!ignore_me(ignore_those_in_limbo) && (!ignore_those_in_a_notebook || !in_a_notebook())) { 
			// !deleted() && !inside_vacuum() && !slated_for_deletion() && !forwarding()) { 
			// note that this now has 2 additional disjuncts
			// on 190799 added !slated_for_deletion() && !forwarding()
//#if TT_ALPHA5 // commented out 311003 since this caused bird/nest connection problems and now release_outgoing_connections fixes the F11 problem
//			insert_at_end_if_not_member_or_same_nest_guid(this,nests); // at least while unbamming can get duplication - new 211003
//#else
			if (tt_old_nest_merging_behavior) {
				insert_at_end_if_not_member(this,nests); 
			} else {
				insert_at_beginning_and_remove_duplicates(this,nests);  // original first is important
			};
//#endif
      };
		return(nests);
 	} else if (original_nest != NULL) {
		return(original_nest->all_nests(ignore_those_in_a_notebook,ignore_those_in_limbo,NULL,nests_already_processed)); 
		// new on 121103 to prevent loops
//		return(original_nest->all_nests());
	};
   return(NULL);
};

void Nest::add_copy(Nest *nest) {
	nest_copies = new Sprites(nest,nest_copies);
	nest->increment_ref_count(this); // commented out on 050104 since prevented nests from being reclaimed -- restored on 150205
};

void Nest::remove_copy(Nest *nest) {
	// only original should be called to do this
	if (nest_copies != NULL) {
		boolean found = FALSE;
		nest_copies = nest_copies->remove(nest,&found);
		// following restored on 150205
		if (found) { // commented out on 050104 since doesn't hold on anymore - when a nest is destroyed it needs to call remove_copy now
			nest->destroy(this); // only if ref count is zero
		};
	};
};

Nest *Nest::first_copy_that_is_part_of_top_level_copy() { // new on 181002
   Sprites *remaining = nest_copies;
   while (remaining != NULL) {
      if (remaining->first()->part_of_top_level_copy()) {
         return((Nest *) remaining->first());
      };
      remaining = remaining->rest();
   };
   return(NULL);
};

void Nest::transfer_copies_to(Nest *a_copy) { // new on 181002
   // a_copy should be an element in nest_copies
   if (nest_copies != NULL) {
      boolean found;
      nest_copies = nest_copies->remove(a_copy,&found);
      // warn if not found??
      a_copy->set_nest_copies(new Sprites(this,nest_copies));
      set_nest_copies(NULL);
      a_copy->set_original_nest(a_copy);
      set_original_nest(a_copy);
   } else {
      // warn??
   };
};

void Nest::java_before(java_stream &out) {
//   int index = 
	nest_dump_index(this); // only needs to create the index being 170201
//   if (java_before_done_already == NULL) {
//       java_before_done_already = new flag[max_nests_dumped];
//       for (int i = 0; i < max_nests_dumped; i++) {
//         java_before_done_already[i] = FALSE;
//       };
//   };
	// prior to 170201 this then declared first occurences of nests - now java_declare_all_nests does this
};

void Nest::java_initialize_nest(java_stream &out, int index) {
	string java_label = NULL;
	if (label != NULL) { // new on 150201 to use nest's label primarily
		long length;
		label->current_text(java_label,length);
		if (length > 0) {
			java_label = (string) java_quote(java_label);
		};
	};
   if (java_label == NULL && leader != NULL && leader->kind_of() == CUBBY) {
      Cubby *cubby = (Cubby *) leader;
      java_label = cubby->label(cubby->my_index(this)); // removed (cubby_index) coerecion on 070203
      if (java_label != NULL) {
         java_label = java_quote(java_label);
      };
   };
//   java_declare_new(out,S(IDS_JAVA_TT_NEST),S2(IDS_JAVA_NEST,index),S(IDS_JAVA_TT_NEST),java_label);
	java_assign_new(out,S2(IDS_JAVA_NEST,index),S(IDS_JAVA_TT_NEST),java_label);
   if (java_label != NULL) delete [] java_label;
};

void Nest::java(java_stream &out) {
   out << S2(IDS_JAVA_NEST,nest_dump_index(this));
//   out << "nest" << (int) nest_dump_index(this);
};

unsigned short int item_on_nest_counter = 1;

void Nest::java_after(java_stream &out, const_java_string path) {
   if (contents_stack == NULL) return;
//   java_begin_block(out);
   string item_on_nest = SC(IDS_JAVA_ITEM_ON_NEST,item_on_nest_counter++);
   java_declare(out,S(IDS_JAVA_TT_OBJECT),item_on_nest);
//   out << "  TTObject item_on_nest;" << endl;
   Sprites *remaining = contents_stack;
	while (remaining != NULL) {
      Sprite *item = remaining->first();
      item->java_before(out);
      java_assign(out,item_on_nest,item);
//      out << "  item_on_nest = ";
//		item->java(out);
//      out << ";" << endl;
      java_send(out,path,S(IDS_JAVA_PUT_ON_NEST),item_on_nest);
//      out << "  " << path << ".PutOnNest(item_on_nest);" << endl;
      item->java_after(out,item_on_nest);
		remaining = remaining->rest();
	};
   delete [] item_on_nest;
//   java_end_block(out);
};

int addresses_size(string *addresses, int address_count) {
	int result = 0;
	for (int i = 0; i < address_count; i++) {
		result += strlen(addresses[i])+1; // 1 for a byte for the length
	};
	return(result+1); // 1 byte for address_count itself
};

const unsigned char current_nest_info_version_number = 1;
// became 1 on 240600 since the size of address and GUID missed a flag

boolean Nest::dump(output_stream &stream, boolean just_prepare, boolean is_controlee, boolean on_nest) {
	// not bothering with the new forwarding_to stuff from 140205 here
	if (forwarding() && original_nest != NULL) { // new on 051199
		return(original_nest->dump(stream,just_prepare,is_controlee,on_nest));
	};
   if (just_prepare) {
      finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
      return(FALSE);
   };
	if (!Sprite::dump(stream,just_prepare,is_controlee)) return(FALSE); // do normal thing
	dump_if_not_null(stream,comment);
	// new on 240299 -- this is more general and doesn't have the 255 nest limit the old way did
	// and it is less wasteful
	Sprites *remaining = contents_stack;
	while (remaining != NULL) {
		remaining->first()->dump(stream,just_prepare,FALSE,TRUE);
		remaining = remaining->rest();
	};
	stream.put(NOTHING_MARKER);
	remaining = nest_copies;
	while (remaining != NULL) {
		Nest *nest_copy = (Nest *) remaining->first();
		if (being_dumped_together(this,nest_copy)) { // only if saved together is relationship kept
			nest_copy->dump(stream,just_prepare);
		};
		remaining = remaining->rest();
	};
	stream.put(NOTHING_MARKER);
	dump_if_not_null(stream,label);
#if TT_DIRECT_PLAY
	// commented out the following on 030500 since bird now arranges for this to happen by calling become_networked()
//	if (!being_marshalled() && !tt_dumping_to_test_equality_or_describe && dump_as_proxy() &&
//		 (tt_dumping_background == NULL || !tt_dumping_background->outside())) {
		// if nest is being sent over the wire or the entire city is being saved don't do the following
		// added !proxy() as a condition since they shouldn't do this stuff
		// on 020500 changed (!proxy() || dump_as_proxy()) to dump_as_proxy()
//		if (nest_guid == NULL) {
//			nest_guid = new GUID;
//			UuidCreate(nest_guid); // tried CoCreateGuid but this is lower overhead
//		};
//		if (direct_play == NULL) {
//			host_a_new_direct_play_session();
//		};
//		if (player_address == NULL && player_id != 0) {
//			player_address = get_player_address(direct_play,player_id,player_address_size);
//		};
//	};
	/* old style:
	if (player_address != NULL) {
		unsigned short int address_and_guid_size = player_address_size+sizeof(GUID)+1; // +1 for flags
		stream.write((char *) &address_and_guid_size,sizeof(address_and_guid_size)); // so older versions can ignore all of this
		if (player_address != NULL) {
			stream.write((char *) player_address,player_address_size);
		};
		stream.write((char *) nest_guid,sizeof(GUID));
		stream.put((flag) (proxy() || dump_as_proxy()));
		return(TRUE);
	}; // else warn??
	*/
	if (addresses != NULL && !tt_save_nests_as_local) {
		unsigned short int address_and_guid_size = addresses_size(addresses,address_count)+sizeof(GUID)+5; // +5 for flags and tags
		// was +3 prior to 240600 (forgot proxy flag) so non TT_DIRECT_PLAY versions had errors
		// was +4 prior to 290700
		stream.write((char *) &address_and_guid_size,sizeof(address_and_guid_size)); 
		stream.put(NEST_NETWORK_INFO_MARKER);
		stream.put(current_nest_info_version_number);
		stream.put(address_count);
		for (int i = 0; i < address_count; i++) {
			unsigned char address_length = strlen(addresses[i])+1; // include terminator
			stream.put(address_length);
			stream.write(addresses[i],address_length);
		};
		stream.write((char *) nest_guid,sizeof(GUID));
		stream.put((flag) (proxy() || dump_as_proxy()));
		return(TRUE);
	};
#endif
	leave_room_for_growth(stream,2); // new on 070100 64K should be more than enough for network awareness
	return(TRUE);
	// if nest knew whether any of its birds were in ultimate_container
	// could optimize this
/* -- commented out on 240299
	stream.put((unsigned char) nest_dump_index(this)); 
	unsigned short int stack_size = 0;
	long size = 0;
	if (contents_stack != NULL) {
		size = contents_stack->length();
		if (size > max_nest_items_to_dump) {
			say_error(S(IDS_NEST_TOO_FULL_TO_SAVE));
			stack_size = max_nest_items_to_dump;
		} else {
		   stack_size = (unsigned short int) size;
		};
		size = stack_size;
	};
	stream.write((unsigned char *) &stack_size, sizeof(stack_size));
	Sprites *remaining = contents_stack;
	while (remaining != NULL) {
		remaining->first()->dump(stream);
		size--;
		if (size <= 0) return; // too many
		remaining = remaining->rest();
	};
	// following is new on 230299 so nest relationships are preseved
	unsigned short int nest_copies_size = 0;
	if (nest_copies != NULL) {
		nest_copies_size = (unsigned short int) nest_copies->length();
	};
	stream.write((unsigned char *) &nest_copies_size, sizeof(nest_copies_size));
	remaining = nest_copies;
	while (remaining != NULL) {
		remaining->first()->dump(stream);
		remaining = remaining->rest();
		// what if more than 64K of them??
	};
	*/
};

#if TT_XML
// new on 120802
//xml_element *Nest::xml_create_element(xml_document *document) {
//   //if (forwarding() && original_nest != NULL) { // commented out on 051202 since this will just use L"Nest" as well
//	//	return(original_nest->xml_create_element(document));
//	//};
//   return(create_xml_element(xml_tag(),document));
//};

PostponedNestXML *postponed_nest_xml = NULL; 
// new on 171002 to deal with nests that might be saved before birds who connect to them
int postponed_nest_xml_count = 0;
int max_postponed_nest_xml_count = 0;
boolean first_pass = TRUE;
boolean dumping_as_temporary_copy = FALSE; // new on 190704

void postpone_nest_xml(Nest *nest, xml_element *nest_element) { // xml_document *document) {
	// aren't all these for the same document? -- why store it over and over again?? asked 120703
   if (postponed_nest_xml_count >= max_postponed_nest_xml_count) {
      int new_max_postponed_nest_xml_count = max(32,max_postponed_nest_xml_count*2);
      PostponedNestXML *new_postponed_nest_xml = new PostponedNestXML[new_max_postponed_nest_xml_count];
      for (int i = 0; i < postponed_nest_xml_count; i++) { // copy over old ones
         new_postponed_nest_xml[i].nest = postponed_nest_xml[i].nest;
         new_postponed_nest_xml[i].nest_element = postponed_nest_xml[i].nest_element;
 //        new_postponed_nest_xml[i].document = postponed_nest_xml[i].document;
      };
      delete [] postponed_nest_xml;
      postponed_nest_xml = new_postponed_nest_xml;
      max_postponed_nest_xml_count = new_max_postponed_nest_xml_count;
   };
   postponed_nest_xml[postponed_nest_xml_count].nest = nest;
   postponed_nest_xml[postponed_nest_xml_count].nest_element = nest_element;
//   postponed_nest_xml[postponed_nest_xml_count].document = document;
#if TT_DEBUG_ON
	int ref_count = // for debugging XML ref counts
#endif
	nest_element->AddRef(); // new on 190804
#if TT_DEBUG_ON
		debug_ref_count(ref_count,nest_element);
#endif
	postponed_nest_xml_count++;
};

void release_postponed_nest_xml() {
   if (postponed_nest_xml != NULL) {
#if TT_DEBUG_ON
	int ref_count; // for debugging XML ref counts
#endif
		for (int i = 0; i < postponed_nest_xml_count; i++) { // new on 200804
			if (postponed_nest_xml[i].nest_element != NULL) {
#if TT_DEBUG_ON
				ref_count =
#endif
				postponed_nest_xml[i].nest_element->Release();
#if TT_DEBUG_ON
				debug_ref_count(ref_count,postponed_nest_xml[i].nest_element);
#endif
			};
		};
      delete [] postponed_nest_xml;
      postponed_nest_xml = NULL;
      max_postponed_nest_xml_count = 0;
   };
};

void do_postponed_nest_xml(xml_document *document) {
   first_pass = FALSE;
#if TT_DEBUG_ON
	int ref_count; // for debugging XML ref counts
#endif
   for (int i = 0; i < postponed_nest_xml_count; i++) {
      postponed_nest_xml[i].nest->xml(postponed_nest_xml[i].nest_element,document); // postponed_nest_xml[i].document);
#if TT_DEBUG_ON
		ref_count = 
#endif
		postponed_nest_xml[i].nest_element->Release(); // new on 190804
#if TT_DEBUG_ON
		debug_ref_count(ref_count,postponed_nest_xml[i].nest_element);
#endif
   };
   first_pass = TRUE; // for next time
   postponed_nest_xml_count = 0;
};

/*
// on 180804 tried to revert back to the 050704 version but caused circular references in original_nest (with Gordon's mass tool)
void Nest::xml(xml_element *nest_element, xml_document *document) {
   if (forwarding() && original_nest != NULL) { 
		original_nest->xml(nest_element,document);
      return;
	};
	if (first_pass && (dumping_an_individual() || tt_dump_reason == DUMPING_TO_ENABLE_UNBAMMING)) {
		// removed && !tt_save_nests_as_local on 290404 since that makes the order dependent as to whether it saves correctly
		// added tt_dump_reason == DUMPING_TO_ENABLE_UNBAMMING on 121202 
		// since no need to postpone if dumping a city or the like since won't be creating long-distance birds
//   if (first_pass && !tt_save_nests_as_local && dumping_an_individual()) {
		postpone_nest_xml(this,nest_element); //,document);
      return;
   };
	// prior to 220104 the following was bird_count rather than current_bird_count() 
	// but it is only the original nest that really maintains the count
	if (dumping_an_individual() && original_nest == this && current_bird_count() > 0 && !guid_temporarily_reset(this) 
	//   && addresses == NULL && guid == NULL) { 
		// new on 050104 to dump as if this were a copy
		// added bird_count > 0 on 120104 since no need to do this otherwise and can lead to infinite recursion otherwise
		// added guid_temporarily_reset on 130104 so generate new pairs if bird has already encountered this nest
		// added addresses == NULL on 210104 since this interferes with long-distance birds otherwise
		// guid == NULL condition new on 150604 -- fixed bug where if city was saved then bird/nest pairs didn't save right
		Nest *temp_copy = (Nest *) copy();
		temp_copy->xml(nest_element,document);
		temp_copy->destroy();
//		temp_copy->destroy(); // twice since original nest refers to this (bug?) - 050104
		return;
	};
   if (label != NULL) {
      long length;
      wide_string label_text;
      label->current_wide_text(label_text,length);
      // this assumes that color and font haven't been changed - can they??
      xml_append_child(xml_character_data_element(label_text,L"Label",document),nest_element);
		// was create_xml_element prior to 220203
		int insertion_point = label->return_insertion_point();
		if (insertion_point >= 0) { // new on 120604
			xml_set_attribute(nest_element,L"EditInsertionPoint",insertion_point);
		};
   };
	if (bird_count != 1) { 
	// 1 seems like a good default -- moved here on 080403 from volatile_attributes since needed even if not volatile
		xml_set_attribute(nest_element,L"BirdCount",bird_count);
	};
   if (contents_stack != NULL) {
      // the dump version calls with a flag so dumpee knows if on_nest -- 011102
		if (contents_stack->first()->pointer_to_leader() != this) {
			// new on 121202 to mark stacks that have been removed by bird
			//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
			//	contents_stack->check_for_independence();
			//};
			contents_stack->add_xml(nest_element,L"TemporarilyRemovedFromOnTop",document); // this doesn't wrap each element
		} else {
			contents_stack->add_xml(nest_element,document,L"OnTop");
		};
		//// rewrote the above on 121202 so that only the contents of the stack 
		// that have been temporarily removed by a bird are marked with an attribute
		//xml_element *list_element = create_xml_element(L"OnTop",document);
		//xml_append_child(list_element,nest_element);
	   //Sprites *remaining = contents_stack;
		//while (remaining != NULL) {
		//	Sprite *sprite = remaining->first();
		//	xml_element *element = sprite->xml_create_and_append_element(parent_element,document);
		//	if (sprite->pointer_to_leader() != this) {
		//		xml_set_attribute(element,L"TemporarilyRemovedFromNest",1); // new on 121202
		//	};
		//	remaining = remaining->rest();
		//};
   };
   // seems the following was written on 171002 and then commented out (probably the same day) - 
	// but it is important - restored 18093
	// commented out again on 050104 since now when OriginalNest is loaded copy is added to nest_copies
   //if (nest_copies != NULL) { // new on 171002
   //   xml_element *copies_element = create_xml_element(L"NestCopies",document);
   //   Sprites *remaining = nest_copies;
   //   while (remaining != NULL) {
   //      Nest *nest_copy = (Nest *) remaining->first();
   //      if (being_dumped_together(this,nest_copy)) { // only if saved together is relationship kept
		 //     nest_copy->xml_create_and_append_element(copies_element,document);
   //      };
			//remaining = remaining->rest(); 
			// on 190903 noticed that this was in the conditional above so didn't work if say saving in a notebook
	  // };
   //   xml_append_child(copies_element,nest_element);
   //};
	if (original_nest != this && original_nest != NULL) { // new on 180903 - otherwise copies break after first use
		// added original_nest != NULL on 111003 since crashes otherwise - can happen with nests in thought bubbles
		// was add_xml prior to 050104 - new policy to maintain links
		if (dumping_an_individual() && !guid_temporarily_reset(this)) { 
			// added dumping_an_individual() on 220104 since doesn't make sense when saving an entire city
			// added && !guid_temporarily_reset(this) on 180804 so if bird is also being saved then new pair is generated
			original_nest->xml_create_external_link_and_append_element(nest_element,document,L"OriginalNest"); // was add_xml prior to 050104 - new policy to maintain links
			original_nest->set_bird_count(max_long); // new on 050104 since if saved to clipboard then any number of nest copies may appear
		} else { // new on 220104
			original_nest->add_xml(nest_element,L"OriginalNest",document);
		};
	};
#if TT_DIRECT_PLAY
	if (tt_dump_reason == DUMPING_TO_ENABLE_UNBAMMING) { // prior to 290103 was tt_dump_nests_as_networked) { // new on 041020 -- 
      assign_guid(); // prior to 151002 was become_networked();
//      register_networked_nest(this); // perhaps not best name for this -- new on 151002
		insert_at_end_if_not_member(this,non_networked_nests_with_guid); // rewritten on 290103 to work when long distance birds are off (besides no need to involve DirectPlay in this)
   };
	if (addresses != NULL && (!tt_save_nests_as_local || tt_dump_reason == DUMPING_TO_ENABLE_UNBAMMING)) { // prior to 290103 was tt_dump_nests_as_networked
      if (proxy() || dump_as_proxy()) {
         xml_append_child(create_xml_element(L"ForwardsToRealNest",document),nest_element);
      };
      for (int i = 0; i < address_count; i++) {
         xml_append_child(create_xml_element(L"Address",document,addresses[i]),nest_element);
      };
   };
   if (nest_guid != NULL) { // re-organized on 151002 so this happens even if there are no addresses
      string guid_string;
      string_of_guid(nest_guid,guid_string);
      xml_append_child(create_xml_element(L"ID",document,guid_string),nest_element);
      release_guid_string(guid_string);
//      set_nest_guid(NULL); // 171002 -- just a temporarly measure!!! 
	};
#endif
   Sprite::xml(nest_element,document);
};
*/

void Nest::xml(xml_element *nest_element, xml_document *document) {
	//if (!tt_old_nest_merging_behavior) {
		// commented out also on 140205 since the original nest can become the wrong guy here
		//if (original_nest != this && forwarding_to() != NULL) {
		//	ultimate_forwarding_to_or_original_nest()->xml(nest_element,document); // new on 140205
		//	return;
		//};
	//} else 
	if (forwarding() && original_nest != NULL) { 
		// must be old behavior or loaded something in the middle of old nest forwarding behavior
		original_nest->xml(nest_element,document);
      return;
	};
	// following moved before nest postponement on 190704 since otherwise references to original_nest can be to previous pasting
	if ((first_pass || dumping_as_temporary_copy) && original_nest != this && original_nest != NULL) { 
		// new on 180903 - otherwise copies break after first use
		// added first_pass || dumping_as_temporary_copy test on 190704
		// added original_nest != NULL on 111003 since crashes otherwise - can happen with nests in thought bubbles
		// was add_xml prior to 050104 - new policy to maintain links
		if (dumping_as_temporary_copy) {
			dumping_as_temporary_copy = FALSE; // so don't allow this to be dumped twice
		};
		if (!contained_in(THOUGHT_BUBBLE)) {
			// condition new on 101204 since it somehow did save such a city with a loop of original_nest links
			if (dumping_an_individual() && !guid_temporarily_reset(this)) { 
				// updated 180804 !being_dumped_together(this,original_nest)) { 
				// added dumping_an_individual() on 220104 since doesn't make sense when saving an entire city
				// added !being_dumped_together test on 180704	
				original_nest->xml_create_external_link_and_append_element(nest_element,document,L"OriginalNest"); 
				// was add_xml prior to 050104 - new policy to maintain links
				original_nest->set_bird_count(max_long); 
				// new on 050104 since if saved to clipboard then any number of nest copies may appear
			} else { // new on 220104
				original_nest->add_xml(nest_element,L"OriginalNest",document);
			};
		};
	};
	boolean postponement_sensible = (dumping_an_individual() || tt_dump_reason == DUMPING_TO_ENABLE_UNBAMMING);
	if (first_pass && postponement_sensible) {
		// removed && !tt_save_nests_as_local on 290404 since that makes the order dependent as to whether it saves correctly
		// added tt_dump_reason == DUMPING_TO_ENABLE_UNBAMMING on 121202 
		// since no need to postpone if dumping a city or the like since won't be creating long-distance birds
//   if (first_pass && !tt_save_nests_as_local && dumping_an_individual()) {
		if (contents_stack != NULL) { // new on 011105 since contents may have links (e.g. picture and remote controls)
			contents_stack_xml(nest_element,document);
		};
		postpone_nest_xml(this,nest_element); //,document);
      return;
   };
	// prior to 220104 the following was bird_count rather than current_bird_count() 
	// but it is only the original nest that really maintains the count
	if (dumping_an_individual() && original_nest == this && current_bird_count() > 0 && !guid_temporarily_reset(this) && 
		 addresses == NULL && guid == NULL) { 
		// new on 050104 to dump as if this were a copy
		// added bird_count > 0 on 120104 since no need to do this otherwise and can lead to infinite recursion otherwise
		// added guid_temporarily_reset on 130104 so generate new pairs if bird has already encountered this nest
		// added addresses == NULL on 210104 since this interferes with long-distance birds otherwise
		// guid == NULL condition new on 150604 -- fixed bug where if city was saved then bird/nest pairs didn't save right
		Nest *temp_copy = (Nest *) copy();
		dumping_as_temporary_copy = TRUE;
		temp_copy->xml(nest_element,document);
		dumping_as_temporary_copy = FALSE;
		temp_copy->destroy();
//		temp_copy->destroy(); // twice since original nest refers to this (bug?) - 050104
		return;
	};
   if (label != NULL) {
      long length;
      wide_string label_text;
      label->current_wide_text(label_text,length);
      // this assumes that color and font haven't been changed - can they??
      xml_append_child(xml_character_data_element(label_text,L"Label",document),nest_element); 
		// was create_xml_element prior to 220203
		int insertion_point = label->return_insertion_point();
		if (insertion_point >= 0) { // new on 120604
			xml_set_attribute(nest_element,L"EditInsertionPoint",insertion_point);
		};
   };
	int true_bird_count = current_bird_count(); 
	// may differ if not being saved with original_nest where it gets the true bird count from -- new on 040205
	if (true_bird_count != 1) { 
		// 1 seems like a good default -- moved here on 080403 from volatile_attributes since needed even if not volatile
		xml_set_attribute(nest_element,L"BirdCount",true_bird_count);
	};
   if (contents_stack != NULL && !postponement_sensible) {
		// extra conditions added 011105 so this isn't done if already postponed
      contents_stack_xml(nest_element,document);
   };
	if (forwarding_to_nest != NULL) { 
		// new on 140205 since original nest is both forwarding and holding the nest copies for the first
		forwarding_to_nest->add_xml(nest_element,L"MergedWith",document);
	};
	if (original_nests_forwarding_here != NULL) { // new on 140205
		original_nests_forwarding_here->add_xml(nest_element,document,L"NestsWhoseBirdsForwardHere");
	};
   // seems the following was written on 171002 and then commented out (probably the same day) -
	// but it is important - restored 18093
	// commented out again on 050104 since now when OriginalNest is loaded copy is added to nest_copies
	// restored on 140205 but only for nest_copies that are forwarding since otherwise links are lost
   if (nest_copies != NULL) { // new on 171002
      xml_element *copies_element = NULL; // recreate on need now
      Sprites *remaining = nest_copies;
      while (remaining != NULL) {
         Nest *nest_copy = (Nest *) remaining->first();
			if (nest_copy->forwarding_to() != NULL || nest_copy->pointer_to_original_nest() != this) {
				// if forwarding or was added due to forwarding then need to save the info here
				if (copies_element == NULL) { // initialize on need
					copies_element = create_xml_element(L"NestCopies",document);
				};
		      nest_copy->xml_create_and_append_element(copies_element,document);
         };
			remaining = remaining->rest(); 
			// on 190903 noticed that this was in the conditional above so didn't work if say saving in a notebook
	   };
		if (copies_element != NULL) {
			xml_append_child(copies_element,nest_element);
		};
   };
#if TT_DIRECT_PLAY
	if (tt_dump_reason == DUMPING_TO_ENABLE_UNBAMMING) { // prior to 290103 was tt_dump_nests_as_networked) { // new on 041020 -- 
      assign_guid(); // prior to 151002 was become_networked();
//      register_networked_nest(this); // perhaps not best name for this -- new on 151002
		insert_at_end_if_not_member(this,non_networked_nests_with_guid); 
		// rewritten on 290103 to work when long distance birds are off (besides no need to involve DirectPlay in this)
   };
	if (addresses != NULL && (!tt_save_nests_as_local || tt_dump_reason == DUMPING_TO_ENABLE_UNBAMMING)) { 
		// prior to 290103 was tt_dump_nests_as_networked
      if (proxy() || dump_as_proxy()) {
         xml_append_child(create_xml_element(L"ForwardsToRealNest",document),nest_element);
      };
      for (int i = 0; i < address_count; i++) {
         xml_append_child(create_xml_element(L"Address",document,addresses[i]),nest_element);
      };
   };
   if (nest_guid != NULL && tt_saved_city_file_name_when_demo_ends == NULL) { 
		// re-organized on 151002 so this happens even if there are no addresses
      string guid_string;
      string_of_guid(nest_guid,guid_string);
      xml_append_child(create_xml_element(L"ID",document,guid_string),nest_element);
      release_guid_string(guid_string);
//      set_nest_guid(NULL); // 171002 -- just a temporarly measure!!! 
	};
#endif
   Sprite::xml(nest_element,document);
};

void Nest::contents_stack_xml(xml_element *nest_element, xml_document *document) {
	// abstracted on 011105
// the dump version calls with a flag so dumpee knows if on_nest -- 011102
	if (contents_stack->first()->pointer_to_leader() != this) {
		// new on 121202 to mark stacks that have been removed by bird
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	contents_stack->check_for_independence();
		//};
		contents_stack->add_xml(nest_element,L"TemporarilyRemovedFromOnTop",document); // this doesn't wrap each element
	} else {
		contents_stack->add_xml(nest_element,document,L"OnTop");
	};
	//// rewrote the above on 121202 so that only the contents of the stack that have been temporarily removed by a bird 
	// are marked with an attribute
	//xml_element *list_element = create_xml_element(L"OnTop",document);
	//xml_append_child(list_element,nest_element);
	//Sprites *remaining = contents_stack;
	//while (remaining != NULL) {
	//	Sprite *sprite = remaining->first();
	//	xml_element *element = sprite->xml_create_and_append_element(parent_element,document);
	//	if (sprite->pointer_to_leader() != this) {
	//		xml_set_attribute(element,L"TemporarilyRemovedFromNest",1); // new on 121202
	//	};
	//	remaining = remaining->rest();
	//};
};

xml_element *Nest::xml_set_special_attributes(xml_element *element, xml_document *document) {
	// new on 051202 -- went through all the state variables
//	if (!tt_old_nest_merging_behavior) {
		//if (original_nest != this && forwarding_to() != NULL) {
		//	return(ultimate_forwarding_to()->xml_set_special_attributes(element,document));
		//};
//	} else 
	if (forwarding() && original_nest != NULL) { // must be old behavior or loaded something in the middle of old nest forwarding behavior
		return(original_nest->xml_set_special_attributes(element,document));
	};
	xml_element *special_element = Sprite::xml_set_special_attributes(element,document);
	if (deleted_flag) {
		xml_set_attribute(special_element,L"Deleted",1);
	};
	if (stack_temporarily_removed) {
		xml_set_attribute(special_element,L"StackTemporarilyRemoved",1);
	};
	if (display_new_contents) {
		xml_set_attribute(special_element,L"DisplayNewContents",1);
	};	
	return(special_element);
};

boolean Nest::xml_get_special_attributes(xml_node *node) {
//	bird_count = xml_get_attribute_int(node,L"BirdCount",1); // commented out on 070104 since isn't a volatile attribute and is read when NO_MORE_TAGS
	deleted_flag = xml_get_attribute_int(node,L"Deleted",0); // prior to 210404 was deleted_flag
	stack_temporarily_removed = xml_get_attribute_int(node,L"StackTemporarilyRemoved",0); // prior to 210404 was stack_temporarily_removed
	display_new_contents = xml_get_attribute_int(node,L"DisplayNewContents",0); // prior to 210404 was display_new_contents
	return(Sprite::xml_get_special_attributes(node));
};

void Nest::add_special_xml(xml_element *element, xml_document *document) {
	//if (!tt_old_nest_merging_behavior) {
		//if (original_nest != this && forwarding_to() != NULL) {
		//	ultimate_forwarding_to()->xml_set_special_attributes(element,document);
		//	return;
		//};
	//} else 
	if (forwarding() && original_nest != NULL) { 
		// must be old behavior or loaded something in the middle of old nest forwarding behavior
		original_nest->xml_set_special_attributes(element,document);
		return;
	};
	if (new_contents != NULL) {
		new_contents->add_xml(element,L"NewContents",document);
	};
	if (robot_to_try != NULL && cubby_to_try_robot_on != NULL) {
		// updated on 181104 since no point saving one of these one is NULL -- 
		// also avoids a crash if cubby_to_try_robot_on has trash for floor
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	robot_to_try->check_for_independence();
		//};
		robot_to_try->add_xml(element,L"RobotToTry",document);
	//};
	//if (cubby_to_try_robot_on != NULL) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	cubby_to_try_robot_on->check_for_independence();
		//};
		cubby_to_try_robot_on->add_xml(element,L"BoxToTryRobotOn",document);
	};	
	if (waiting_birds != NULL) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	waiting_birds->check_for_independence();
		//};
		waiting_birds->add_xml(element,L"WaitingBirds",document);
		// waiting_birds_tail can be found upon loading
	};
	if (hatching_bird) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	hatching_bird->check_for_independence();
		//};
		hatching_bird->add_xml(element,L"HatchingBird",document);
	};
#if TT_DIRECT_PLAY
	if (pending_messages != NULL) {
		pending_messages->add_xml(element,L"PendingMessages",document);
		// pending_message_count can be computed upon loading
	};
	// no need to save attempts_to_join_session since is another session
	// not saving nth_address or direct_play_count since they should be recreated on loading
#endif
	Sprite::add_special_xml(element,document);
};

void nest_handle_xml_on_top_tag(Entity *nest, Tag tag, xml_node *node, void *extra_info) {
   ((Nest *) nest)->handle_xml_on_top_tag(tag,node);
};

boolean Nest::handle_xml_on_top_tag(Tag tag, xml_node *node) {
   if (sprite_tag(tag)) {
      Sprite *item = xml_load_sprite(node,tag);
      if (item != NULL) {
			if (contents_stack == NULL) { // first one
				give_size_and_location_to_cover_to(item); // might be too early since nest doesn't have good geometry yet
			};
			insert_at_end_of_contents(item); // could optimize this by keeping track of the "last cons" (noticed on 071102)
		}; // else warn??
   }; // else save?? - except for NO_MORE_TAGS
   return(TRUE);
};

void nest_handle_nest_copies_tag(Entity *nest, Tag tag, xml_node *node, void *extra_info) {
   ((Nest *) nest)->handle_nest_copies_tag(tag,node);
};

boolean Nest::handle_nest_copies_tag(Tag tag, xml_node *node) {
	if (tag == NEST_TAG) { // otherwise why continue? - new on 261003
//   if (sprite_tag(tag)) {
      Sprite *item = xml_load_sprite(node,tag);
      if (item != NULL) {
			add_copy((Nest *) item); // this deals with reference counting properly
//			nest_copies = new Sprites(item,nest_copies);
      };
   };
   return(TRUE);
};

boolean Nest::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
      case ON_TOP_TAG:
      case THINGS_ON_TOP_TAG: // old tag - for now - 011102
         return(xml_process_children(node,nest_handle_xml_on_top_tag,this));
		case TEMPORARILY_REMOVED_FROM_TOP_TAG:
			// new on 121202 -- maybe ON_TOP should be made obsolete and new version work like this
			destroy_sprites(contents_stack); // new on 070103
			contents_stack = xml_load_sprites(node);
			if (contents_stack != NULL) {
				contents_stack->increment_ref_count(); // new on 261003
			};
			return(contents_stack != NULL);
      case NEST_COPIES_TAG: // new on 171002
         return(xml_process_children(node,nest_handle_nest_copies_tag,this));
      case LABEL_TAG:
         set_label(make_label(xml_get_text_copy(node)));
         return(TRUE);
      case ADDRESS_TAG: {
         string new_address = xml_get_narrow_text_copy(node);
         if (new_address == NULL) return(FALSE);
			//if (strcmp(new_address,"127.0.0.1") == 0) { 
			//	// not really remote -- ignore (does this eliminate 2 toontalks talking on same computer?) - new on 050304
			//	return(TRUE);
			//};
         string *new_addresses = new string[address_count+1];
         for (int i = 0; i < address_count; i++) {
            new_addresses[i] = addresses[i];
         };
         new_addresses[address_count] = new_address;
         address_count++;
         if (addresses != NULL) delete [] addresses;
         addresses = new_addresses;
         return(TRUE);
								};
      case ID_TAG: { 
         string guid_string = xml_get_narrow_text_copy(node);
         nest_guid = new GUID;
         boolean ok = guid_from_string(guid_string,nest_guid); 
         delete [] guid_string;
         if (ok) {
            return(TRUE);
         } else {
            return(FALSE);
         };
						 };
      case FORWARD_TO_REAL_NEST_TAG:
         set_proxy(TRUE);
         return(TRUE);
		case NEW_CONTENTS_TAG:
			set_new_contents(xml_get_sprite(node)); // was an assignment prior to 121202 but then ref counting wasn't right
			return(TRUE);
		case ROBOT_TO_TRY_TAG:
			set_robot_to_try((Robot *) xml_get_sprite(node,robot_to_try,ROBOT));  // was simple assignment prior to 130203
			return(TRUE);
		case BOX_TO_TRY_ROBOT_ON_TAG:
			set_cubby_to_try_robot_on((Cubby *) xml_get_sprite(node,cubby_to_try_robot_on,CUBBY)); 
			return(TRUE);
		case WAITING_BIRDS_TAG:
			destroy_sprites(waiting_birds); // new on 070103
			waiting_birds = xml_load_sprites(node);
			//if (waiting_birds != NULL) { // noticed obsolete on 140205
			//	waiting_birds_tail = waiting_birds->last_cons();
			//};
			return(TRUE);
		case HATCHING_BIRD_TAG:
			hatching_bird = (Bird *) xml_get_sprite(node,hatching_bird,BIRD);
#if TT_DEBUG_ON
			if (hatching_bird == NULL) {
				xml_debug_node(node,L"No hatching_bird");
			};
#endif
//			hatching_bird->set_background(floor); // seems to be needed
			return(TRUE);
#if TT_DIRECT_PLAY // otherwise counts as extra_xml
		case PENDING_MESSAGES_TAG:
			destroy_sprites(pending_messages); // new on 070103
			pending_messages = xml_load_sprites(node);
			if (pending_messages != NULL) {
				pending_message_count = (short) pending_messages->length(); 
				// reconsider whether pending_message_count should still be a short
			};
			return(TRUE);
#endif
		case ORIGINAL_NEST_TAG: { // new on 180903
			Nest *nest_to_clobber;
			if (original_nest == this) {
				nest_to_clobber = NULL; // new on 151003 since shouldn't be clobbering "this"
			} else {
				nest_to_clobber = original_nest;
			};
			Nest *nest = (Nest *) xml_get_sprite(node,nest_to_clobber,NEST); 
			// xml_get_sprite already did the type test - moved coercion here on 050104
			if (nest != NULL) {
				if (nest != this) { // new on 050104
					nest->add_copy(this);
				};
				set_original_nest(nest); 
				return(TRUE);
			} else {
				return(FALSE);
			};
										};
		case MERGED_WITH_TAG:  // new on 140205
			set_forwarding_to((Nest *) xml_get_sprite(node,forwarding_to_nest,NEST));
			return(TRUE);
		case NESTS_WHOSE_BIRDS_FORWARD_HERE_TAG:  // new on 140205
			original_nests_forwarding_here = xml_load_sprites(node,FALSE);
			return(original_nests_forwarding_here != NULL);
      case NO_MORE_TAGS: // new on 041002
//			if (bird_count == 0) { // conditional removed on 220104
				bird_count = xml_get_attribute_int(node,L"BirdCount",1); // copied here on 080403 since needed even if not volatile
//			};
         connect_to_nest_with_same_guid();
			if (label != NULL) { // new on 120406
				int insertion_point = xml_get_attribute_int(node,L"EditInsertionPoint",-1); 
				if (insertion_point >= 0) {
					label->set_insertion_point(insertion_point);
				};
			};
			if (original_nest == NULL) { // new on 140904 -- since if unbamming may have dropped this reference
				original_nest = this;
			};
			// the following was confused since should let bird know to set its nest to NULL
			//if (proxy() && addresses == NULL) { // new on 050304 to deal with 127.0.0.1
			//	set_proxy(FALSE);
			//	if (pending_messages != NULL) {
			//		accept_contents(FALSE);
			//	};
			//};
         // and do the following (new on 121102)
      default:
         boolean result = Sprite::handle_xml_tag(tag,node);
			if (bird_count == 0) { // new on 090506
				just_set_parameter(NEST_INERT); // so egg is there if there are no birds
			};
			return(result);
   };
};

short int Nest::default_parameter() { // new on 090506 so that if there are no birds the default is NEST_EMPTY
	if (bird_count == 0) {
		return(NEST_INERT);
	} else {
		return(Sprite::default_parameter());
	};
};

boolean Nest::geometry_worth_saving() {
	if (proxy() || dump_as_proxy()) {
		return(FALSE);
	};
   return(Sprite::geometry_worth_saving());
};

#endif


void Nest::set_in_notebook(Notebook *notebook, boolean recur) { 
	// new on 210506 so that if has no birds reverts to having egg again
	Sprite::set_in_notebook(notebook,recur);
	if (bird_count == 0) {
		set_parameter(NEST_INERT);
	};
};

void Nest::set_robot_to_try(Robot *robot) { 
	// new on 130203 to maintain ref counts (saw a crash from Yishay that seems to indicate the need)
	if (robot == robot_to_try) return; // new on 120205
	if (robot_to_try != NULL) {
		Robot *old_robot_to_try = robot_to_try;
		robot_to_try = NULL; // new on 120205 since the following can trigger this again causing an infinite recursion
		if (robot != NULL) {
			old_robot_to_try->stop_all(TRUE); // new on 120205 since box taken from suspended robot and given to another robot
		};
		old_robot_to_try->destroy();
	};
	robot_to_try = robot;
	if (robot_to_try != NULL) {
		robot_to_try->increment_ref_count();
	};
};

void Nest::set_cubby_to_try_robot_on(Cubby *cubby) { // new on 130203 to maintain ref counts
	if (cubby_to_try_robot_on != NULL) {
		cubby_to_try_robot_on->destroy();
	};
	cubby_to_try_robot_on = cubby;
	if (cubby_to_try_robot_on != NULL) {
		cubby_to_try_robot_on->increment_ref_count();
	};
};

void Nest::set_inside_vacuum(boolean new_flag, boolean recur) { 
	// new on 160405 so if vacuumed up while a robot is waiting the robot gives up
	if (new_flag) {
		if (robot_to_try != NULL) { 
			// do this here since the following won't stop the current robot when setting robot_to_try to NULL
			robot_to_try->stop_all(TRUE);
		};
		set_robot_to_try(NULL);
		set_cubby_to_try_robot_on(NULL);
	};
	Sprite::set_inside_vacuum(new_flag,recur);
};

void Nest::set_leader(Sprite *the_leader, boolean temporarily) { // new on 040205
	// if removed from box (or anything else?) while a robot is waiting on it then stop that robot and forget about it
	if (robot_to_try != NULL && leader != NULL && !tt_resetting_or_destroying_city) { 
		// second condition new on 130205 -- otherwise while loading forgets robot to try
		// third condition new on 130205 since otherwise get warnings in the debug version and waste time 
		// (or crashes -- Yishay sent on 140205)
		if (robot_to_try->body_started()) { // condition new on 140205
			robot_to_try->stop_all(TRUE);
		};
		set_robot_to_try(NULL);
		set_cubby_to_try_robot_on(NULL);
	};
	Sprite::set_leader(the_leader,temporarily);
};

boolean Nest::equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, 
							  SpritesPointer &expected_corresponding_nests,
							  EqualReason reason) { // new on 060601
	if (other == this) return(TRUE);
	if (other->kind_of() != kind_of()) return(FALSE);
	if (is_blank() != other->is_blank()) return(FALSE);
	Nest *other_nest = (Nest *) other;
	if (reason == NORMAL_EQUAL) {
		if ((unsigned long) this < (unsigned long) other) { // canonicalize the order in both lists
			if (corresponding_nests == NULL || corresponding_nests->successive_member_cons(this,other) == NULL) { 
				// not already there
				corresponding_nests = new Sprites(this,new Sprites(other,corresponding_nests));
#if TT_DEBUG_ON
				if (tt_debug_mode == 60601) {
					tt_error_file() << "Found the following are in corresponding positions: ";
					print(tt_error_file());
					tt_error_file() << " and ";
					other->print(tt_error_file());
					tt_error_file() << endl;
				};
#endif
			};
		} else if (corresponding_nests == NULL || corresponding_nests->successive_member_cons(other,this) == NULL) { 
			// not already there
			corresponding_nests = new Sprites(other,new Sprites(this,corresponding_nests));
#if TT_DEBUG_ON
			if (tt_debug_mode == 60601) {
				tt_error_file() << "Found the following are in corresponding positions: ";
				other->print(tt_error_file());
				tt_error_file() << " and ";
				print(tt_error_file());
				tt_error_file() << endl;
			};
#endif
		};
	} else if (reason == BIRDS_CHECKING_EQUAL_NESTS) {
		if (leader == NULL && other->pointer_to_leader() == NULL) return(TRUE); // both birds are "sinks" - new on 080601
		// check nest copies and GUID
		if (pointer_to_nest_copies() != NULL && pointer_to_nest_copies()->member_cons(other) != NULL) return(TRUE);
		if (other_nest->pointer_to_nest_copies() != NULL && other_nest->pointer_to_nest_copies()->member_cons(this) != NULL) 
			return(TRUE);
#if TT_DIRECT_PLAY
		if (pointer_to_nest_guid() != NULL && other_nest->pointer_to_nest_guid() != NULL) {
			return(IsEqualGUID(*pointer_to_nest_guid(),*other_nest->pointer_to_nest_guid()));
		};
#endif
		return(FALSE);
	};
	// following is new on 110601
	Sprites *my_stack = pointer_to_contents_stack();
	Sprites *others_stack = other_nest->pointer_to_contents_stack();
	if (my_stack == NULL) return(others_stack == NULL);
	if (others_stack == NULL) return(FALSE);
	Sprites *remaining = my_stack;
	Sprites *others_remaining = others_stack;
	while (remaining != NULL && others_remaining != NULL) {
		if (!remaining->first()->equal_to(others_remaining->first(),pending,corresponding_nests,expected_corresponding_nests,reason))
			return(FALSE);
		remaining = remaining->rest();
		others_remaining = others_remaining->rest();
	};
	return(remaining == NULL && others_remaining == NULL); // unless birds disagree
};

#if TT_DIRECT_PLAY

void Nest::describe_networked_nest(output_stream &stream) {
	tt_error_file() << "a nest";
	if (label != NULL) {
		long length;
		string label_string;
		label->current_text(label_string,length);
		if (length > 0) {
			tt_error_file() << " with the label " << label_string;
		};
	};
	tt_error_file() << " whose id is ";
	print_guid(nest_guid,tt_error_file());
};

void Nest::assign_guid() { // abstracted on 151002
   if (nest_guid == NULL) {
		nest_guid = new GUID;
		UuidCreate(nest_guid); // tried CoCreateGuid but this is lower overhead
	};
};

void Nest::become_networked() {
	if (tt_system_mode == PUZZLE && tt_dumping_background != NULL) return;
	// above is new on 070600 so in puzzles when saving floor items no networked connections are established
	// but some one should still be able to make puzzles that are networked - just can't save the floor during puzzle runs
	assign_guid();
	if (tt_dump_reason != DUMPING_TO_ENABLE_UNBAMMING) { // new on 290103
		if (direct_play == NULL && !proxy()) { // second and third conditions added 240500
			// removed !dump_as_proxy() on 260500
			host_a_new_direct_play_session();
		};
		if (addresses == NULL && player_id != 0) {
			addresses = get_IP_addresses(direct_play,player_id,address_count);
		};
		if (tt_debug_direct_play && direct_play != NULL) { 
			// moved here on 260103 from assign_guid since that can happen without networking being involved now
	//			top_level_describe(tt_error_file()); // this confused things by dumping within a dump and screwing up indices
			// added second condition on 021003 since if this nest is a proxy then might not have become networked here
			tt_error_file() << "Just made networked ";
			describe_networked_nest(tt_error_file());
			tt_error_file() << " at " << timeGetTime() << endl;
		};
	};
};

void Nest::host_a_new_direct_play_session() {
	if (direct_play == NULL) {
		direct_play = create_direct_play_object(NULL);
	};
	if (player_id == 0) {
		::host_a_new_direct_play_session(direct_play,nest_guid,this);
	};
	set_bird_count(max_long); // in general any number of birds might be coming in from the net - be more clever here??
};

flag already_told_user_about_different_IP_addresses = FALSE;
flag already_told_user_about_no_network = FALSE;

void Nest::player_id_assigned(DPID id, boolean host) {
//	if (id == 0) { // timed out trying to make connection
//		destroy_sprites(pending_messages,this);
//		pending_message_count = 0;
//		return;
//	};
	player_id = id;
	if (host) {
		register_networked_nest(this); // this was part of the conditional below prior to 010500
		if (addresses != NULL) {
//			int nest_IP_address_count;
//			string *nests_IP_addresses = IP_addresses_of_player(player_address,player_address_size,nest_IP_address_count);
//			unsigned short int this_machines_address_size;
//			bytes this_machines_address = 
			unsigned char this_machine_IP_address_count;
			string *this_machines_IP_addresses = get_IP_addresses(direct_play,player_id,this_machine_IP_address_count);
//				IP_addresses_of_player(this_machines_address,this_machines_address_size,this_machine_IP_address_count);
			if (this_machines_IP_addresses == NULL) {
				if (!already_told_user_about_no_network) {
					message_box(MB_ICONEXCLAMATION|MB_OK|MB_SYSTEMMODAL, 
									S(IDS_NO_IP_ADDRESS,"ToonTalk can't find an IP address for this machine. Long-distance birds won't work unless you quit, connect to the Internet, and try again."));
					already_told_user_about_no_network = TRUE;
				};
			} else {
				int i;
				for (i = 0; i < address_count; i++) {
					for (int j = 0; j < this_machine_IP_address_count; j++) {
						if (strcmp(addresses[i],this_machines_IP_addresses[j]) == 0) {
//							delete_strings(nests_IP_addresses,nest_IP_address_count);
							delete_strings(this_machines_IP_addresses,this_machine_IP_address_count);
							return; // OK
						};
					};
				};
				// maybe Marty should say this and the above (with very high importance) but it is often when loading a city...
//				if (!already_told_user_about_different_IP_addresses) {
//					message_box(MB_ICONEXCLAMATION|MB_OK|MB_SYSTEMMODAL, 
					// changed on 110500 so it just adds to the log so we don't bother users
					log(S(IDS_NETWORK_NEST_CREATED_WITH_DIFFERENT_IP_ADDRESS,
						   "Loading a networked nest that was created on a computer with a different Internet address. Saved birds won't be able to find this nest.")); 
					// commented out args on 170205 ,FALSE,TRUE);
					tt_err_file_important_but_no_error = TRUE; // new on 180205 to avoid considering this a ToonTalk error
//					already_told_user_about_different_IP_addresses = TRUE;
//				};
				tt_error_file() << " It is ";
				describe_networked_nest(tt_error_file());
				if (address_count == 1) {
					tt_error_file() << " whose IP address is "; // fixed spelling on 070800
				} else {
					tt_error_file() << " whose IP addresses are ";
				};
				for (i = 0; i < address_count; i++) {
					tt_error_file() << addresses[i];
					if (i+1 < address_count) tt_error_file() << " and ";
				};
				if (this_machine_IP_address_count == 1) {
					tt_error_file() << endl << " While this machine's IP address is currently "; 
				} else {
					tt_error_file() << endl << " While this machine's IP addresses are currently "; 
				};
				for (i = 0; i < this_machine_IP_address_count; i++) {
					tt_error_file() << this_machines_IP_addresses[i];
					if (i+1 < this_machine_IP_address_count) tt_error_file() << " and ";
				};
				tt_error_file() << endl;
//				tt_err_file_important = TRUE; // I guess - already set by log above
			};
//			delete_strings(nests_IP_addresses,nest_IP_address_count);
			delete_strings(this_machines_IP_addresses,this_machine_IP_address_count);
		};
	} else {
		if (tt_debug_direct_play) {
			tt_error_file() << "A nest is identified by ";
			print_guid(nest_guid,tt_error_file());
			tt_error_file() << " just made a connection at " << timeGetTime() << endl;
		};
		release_alternative_direct_plays(); // new on 050200
		if (pending_messages != NULL) {
			send_pending_messages();
		};		
	};
};

void Nest::send_pending_messages() {
	if (player_id == 0 || pending_messages == NULL) return; // not connected yet
	// could batch these -- is there a reason?
	bytes outgoing_message_buffer = new byte[max_encoding_buffer_size];
	outgoing_message_buffer[0] = current_message_version_number;
	outgoing_message_buffer[1] = MESSAGE_FROM_BIRD_TO_NEST;
	long encoding_size;
	Sprites *remaining = pending_messages;
	int remaining_count = tt_max_pending_messages_sent_at_once;
	while (remaining != NULL) {
		Sprite *item = remaining->first();
		bytes encoding = (bytes) item->return_encoding(encoding_size,FALSE,TRUE,(string) outgoing_message_buffer+2); 
		if (encoding != NULL) {
			// don't need to trim - but size is important - but it is important to save version number
			send_network_message(direct_play,player_id,nest_guid,outgoing_message_buffer,encoding_size+2,this);
//			delete [] encoding;
		};
		remaining_count--;
		pending_message_count--;
		if (remaining_count == 0) {
			Sprites *old_pending_messages = pending_messages;
			pending_messages = remaining->rest();
			remaining->set_rest(NULL);
			destroy_sprites(old_pending_messages,this);
			tt_global_picture->do_after(100,this,SEND_PENDING_MESSAGES_CALLBACK);
			delete [] outgoing_message_buffer;
			return;
		};
		remaining = remaining->rest();
	};
	destroy_sprites(pending_messages,this);
	delete [] outgoing_message_buffer;
};

DPID Nest::regenerate_player_id() {
	// called when host (nest) went down (and hopefully came back)
	if (direct_play != NULL) {
		drop_direct_play_object();
	};
	attempts_to_join_session = 0;
	if (tt_debug_direct_play && addresses != NULL) {
		tt_error_file() << "About to connect to IP address #" << (nth_address+1) << " of ";
		describe_networked_nest(tt_error_file());
		tt_error_file() << " at " << timeGetTime() << endl;
	};
	if (nth_address < address_count) {
		direct_play = create_direct_play_object(addresses[nth_address]);
	};
	::join_a_direct_play_session(direct_play,nest_guid,this);
	return(player_id);
};

void Nest::join_a_direct_play_session() {
	if (about_to_quit()) return; // new on 300400
	attempts_to_join_session++;
	if (direct_play_count > 1) { // new on 300400
		nth_address = (nth_address+1)%(direct_play_count-1); // -1 since one address is called direct_play
//#if TT_DEBUG_ON
//		if (tt_debug_direct_play) {
//			tt_error_file() << "Switching to address#" << nth_address << " at " << timeGetTime() << endl;
//		};
//#endif
		if (alternative_direct_plays == NULL) { // new on 300700
			alternative_direct_plays = new LPDIRECTPLAY4A[direct_play_count];
			for (int i = 0; i < direct_play_count; i++) alternative_direct_plays[i] = NULL;
		};
		LPDIRECTPLAY4A new_alternative_direct_play = alternative_direct_plays[nth_address];
		alternative_direct_plays[nth_address] = direct_play;
		direct_play = new_alternative_direct_play;
	} else if (direct_play_count < 0) {
		// see if there is another address to connect to
		DPID saved_player_id = player_id; // how could this be other than 0??
		player_id = 0; // until reset by the following
//#if TT_DEBUG_ON
//		if (tt_debug_direct_play) {
//			tt_error_file() << "See if I can switch to address#" << (nth_address+1) << " at " << timeGetTime() << endl;
//		};
//#endif
		LPDIRECTPLAY4A new_alternative_direct_play = NULL;
		if (nth_address+1 < address_count) {
			new_alternative_direct_play = create_direct_play_object(addresses[nth_address+1]);
		};
		if (new_alternative_direct_play != NULL) { // found a new one
			LPDIRECTPLAY4A *new_alternatives = new LPDIRECTPLAY4A[nth_address+1];
			if (alternative_direct_plays != NULL) {
				for (int i = 0; i < nth_address; i++) {
					new_alternatives[i] = alternative_direct_plays[i];
				};
				delete [] alternative_direct_plays;			
			};
			alternative_direct_plays = new_alternatives;
			alternative_direct_plays[nth_address] = direct_play;
			direct_play = new_alternative_direct_play;
//#if TT_DEBUG_ON
//			if (tt_debug_direct_play) {
//				tt_error_file() << "Switching to address#" << nth_address << " at " << timeGetTime() << endl;
//			};
//#endif
			nth_address++;
		} else { // no more new ones
//#if TT_DEBUG_ON
//			if (tt_debug_direct_play) {
//				tt_error_file() << "Didn't find another connection at " << timeGetTime() << endl;
//			};
//#endif
			player_id = saved_player_id;
			direct_play_count = nth_address+1;
		};
	};
//	if (player_id == 0) {
		// WHAT DOES THE FOLLOWING DO IF tt_finish_instantly is TRUE??
//		tt_global_picture->do_after(delay_before_another_attempt_to_join_session(),this,join_a_direct_play_session_callback); // is 1/2 second a good amount??
//	} else {
		::join_a_direct_play_session(direct_play,nest_guid,this);
//	};
};

millisecond Nest::delay_before_another_attempt_to_join_session() {
	if (attempts_to_join_session < 10) {
		return(500);
	} else if (attempts_to_join_session < 100) {
		return(2000);
	} else {
		return(10000);
	};
};

#endif

boolean Nest::to_windows(Pointer *data, short int &data_offset,
                         ascii_string types, short int &types_offset,
                         BirdPointer *birds, short int &bird_count) {
   if (contents_stack == NULL) return(FALSE); // have to wait
   return(contents_stack->first()->to_windows(data,data_offset,types,types_offset,birds,bird_count));
};

void Nest::remove_contents(Sprite *item, Background *floor_to_add_to) {
	if (contents_stack == NULL) return;
	if (item != contents_stack->first()) { // the nest has changed since (e.g. robot started moving to nest to pick up item)
		boolean found;
		contents_stack = contents_stack->remove(item,&found);
		if (!found) {
			say_error(_T("Robot trying to remove something from a nest that isn't there."));
		} else {
			item->destroy(this); // new on 170799 to decrement ref count
		};
		if (!item->prepared_for_deletion()) { // conditional new on 170799
			return;
		};
	};
	Sprite *contents;
	do { // new on 170799 keep trying until something not destroyed is found
		contents_stack = contents_stack->pop(contents);
		contents->destroy(this); // new on 170799 to decrement ref count
	} while (contents->prepared_for_deletion() && contents_stack != NULL); 
	if (contents->prepared_for_deletion()) return;
	contents->set_selectable(TRUE);
#if TT_DEBUG_ON
	if (tt_debug_mode == 518 || tt_debug_mode == 170703) {
		print(tt_error_file());
		tt_error_file() << " removing ";
		contents->print(tt_error_file());
		tt_error_file() << " on frame " << tt_frame_number << endl;
	};
#endif
	if (contents_stack != NULL) { // && showing_on_a_screen()) {
		// revised on 190499 so that remotes can learn they have "surfaced" and update appropriately
		// new guy may be seen for the first time now
		contents_stack->first()->now_on_top_of_nest(FALSE);
//		contents_stack->first()->recursively_notice_on_screen_change(TRUE);
	};
   if (floor == NULL) {
      set_background(ultimate_container()->pointer_to_background()); // or would floor_to_add_to be better/
   };
// commented out on 150300 since if remote looks don't expect it to change alligence
//	if (floor != NULL && floor->pointer_to_picture() != NULL) {
//		contents->now_on_back_of(NULL);
//	};
//#if TT_DEBUG_ON
//	perform_integrity_checks(contents);
//	tt_error_file() << "Removing " << contents << " from nest with bird count " << bird_count << " "
//					<< this << " on frame " << tt_frame_number << endl; 
//#endif
	remove_follower(contents);
	if (floor_to_add_to != NULL && tt_log_version_number < 76) { // "moved" to after the following on 030605
		// re-written on 090100 - was floor != NULL && add_to_floor) { // add_to_floor added 191199
		floor_to_add_to->add_item(contents,FALSE);
		contents->animate_to_good_size(contents->default_duration(),tt_log_version_number >= 64?tt_toolbox:NULL); 
		// second arg new on 141204 (and elsewhere)
	};
	if (contents_stack != NULL) { // start showing the next one
		set_new_contents(contents_stack->first());
// better to fix it when added
//      next->set_background(floor); // may be obsolete (even gone)
		new_contents->remove_from_floor(FALSE); // added 7/9/98
		new_contents->finish_instantly(TRUE); // may have animations scheduled
		give_size_and_location_to_cover_to(new_contents); // nest may have moved
		add_follower(new_contents);
//		floor->add_item(next);
	} else {
		set_new_contents(NULL,FALSE); // new on 120399 so that matching of nests works properly 
		// (FALSE added on 140201 so old contents doesn't forget its "floor")
		if (contained_in(CUBBY)) {//leader != NULL && leader->kind_of() == CUBBY) {
			give_size_and_location_to_cover_to(this); // nest may have moved
		};
	};
	if (floor_to_add_to != NULL && tt_log_version_number >= 76) { // "moved" here on 030605 since the above can undo this work
		// by moving this here it fixes bugs where things on nests don't animate to a good size 
		// when picked up until dropped and the like
		// re-written on 090100 - was floor != NULL && add_to_floor) { // add_to_floor added 191199
		floor_to_add_to->add_item(contents,FALSE);
		contents->animate_to_good_size(contents->default_duration(),tt_log_version_number >= 64?tt_toolbox:NULL); 
		// second arg new on 141204 (and elsewhere)
	};
//	set_clean_status(DIRTY); // remove_follower does this too
	if (followers_cached()) { 
		// not clear why general mechanism misses this
		tt_screen->invalidate_cache();
	};
};

void Nest::remove_stack(Bird *for_bird) {
	if (contents_stack == NULL) {
//		say(S(IDS_REMOVE_FROM_EMPTY_NEST));
		for_bird->set_delivery_status(HEADED_TO_NEST);
		for_bird->release();
		return;
	};
	if (stack_temporarily_removed) { // already in use
		if (waiting_birds == NULL) {
			waiting_birds = new Sprites(for_bird);
		} else {
			waiting_birds->insert_at_end_if_not_member(for_bird);
		};
		return;
	};
	Sprite *top = contents_stack->first();
	if (!top->selectable()) { // robot has it
		for_bird->just_fly_to_nest(max(1,for_bird->default_duration())); // added max(1, ...) on 230903 so if finishing instantly this doesn't loop
		return;
	};
	stack_temporarily_removed = TRUE;
	set_selectable(FALSE);
	display_new_contents = FALSE;
	set_clean_status(DIRTY);
	if (top->pointer_to_leader() != NULL) {
		top->pointer_to_leader()->remove_follower(top); 
		// rewritten on 210205 since in some cases it can be the bird that is the leader
	};
	//if (top->pointer_to_leader() == this) {
	//	remove_follower(top);
	//};
	if (floor != NULL) {
      floor->add_item(top,FALSE);
   //} else { // commented out on 171203 since this confuses things if getting up holding a box with a nest in it
   //   set_background(for_bird->pointer_to_background()); // nest should know where it is
   };
	for_bird->got_stack(top);
#if TT_DEBUG_ON
	if (tt_debug_mode == 518) {
		print(tt_error_file());
		tt_error_file() << " temporarily removed stack" << endl;
	};
#endif
};

void Nest::display(SelectionFeedback selection_feedback, boolean followers_too, TTRegion *region) {
#if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {
      log("Displaying nest that is debug target.");
   };
#endif
//	if (rescale_if_camera_above(selection_feedback,followers_too,region)) return; // made obsolete on 230804
	if (pointer_to_top_of_stack() != NULL) { // new on 260500
		pointer_to_top_of_stack()->display(selection_feedback,followers_too,region);
		return;
	};
//   if (new_contents == NULL || !new_contents->rectangular()) { 
	if (contents_stack != NULL && !stack_temporarily_removed && followers_too) { 
		// added (once again) followers_too so that br\p41.pzl works for example
//		if (tt_log_version_number > 14) {
			// new on 300399 so you can see the nest if "see some" picture is on top
			boolean saved_postpone_dirty_displays = tt_postpone_dirty_displays; // added on 290499
			// so that this gets cached properly
         tt_postpone_dirty_displays = FALSE;
			display_stack(contents_stack,selection_feedback,followers_too,region);
			tt_postpone_dirty_displays = saved_postpone_dirty_displays;
//		} else {
//			display_followers(selection_feedback,region);
//		};
//		contents_stack->first()->display(screen,selection_feedback,followers_too);
	} else if (display_new_contents && new_contents->rectangular() && leader != NULL) { // obscures everything
		// added leader test on 160603 so you see the nest if on the floor
//		display_followers(screen,selection_feedback);
		new_contents->display(selection_feedback,followers_too,region);
	} else {
		Sprite::display(selection_feedback,FALSE,region); 
		// followers_too is needed for label (was FALSE from 300399 to 070499) and is again on 290499
		// since if stack_temporarily_removed then only label should be displayed
		if (label != NULL && !label->is_blank() // added blank test on 250903
			 && followers_too // new on 261003 since otherwise is displayed twice and once with wrong geometry
			 ) {
			label->display_as_a_follower(selection_feedback,region);
		};
		if (display_new_contents) { // moved here on 290499
			new_contents->display(selection_feedback,followers_too,region);
		};
	};
};

void Nest::display_stack(Sprites *stack, SelectionFeedback selection_feedback, boolean followers_too, TTRegion *region) {
   if (stack == NULL) {
		// FALSE was followers_too prior to 290499 but no point doing that again
		// label is handled specially now
		Sprite::display(selection_feedback,FALSE,region); // show nest if went thru entire stack
		if (label != NULL) label->display_as_a_follower(selection_feedback,region);
		return;
	};
	Sprite *sprite = stack->first();
//	if (!clean()) { // condition new on 180903 since no point doing this repeatedly if clean -- didn't work right - maybe if clean for 2 cycles??
		give_size_and_location_to_cover_to(sprite); // on 310399 used this to get the geometry right
//	};
//	sprite->set_true_size_and_location(true_width(),true_height(),true_llx(),true_lly(),TO_FIT_INSIDE);
	boolean top_is_rectangular = sprite->rectangular();
	if (top_is_rectangular && leader != NULL && completely_inside_leader()) { // leader test new on 160603 -- completely_inside_leader new on 180903
		// replaced region with NULL below on 290499
		// restored it on 030899 since otherwise can get stack overflows when displaying back of picture
		sprite->display_as_a_follower(selection_feedback,region); // was just display prior to 090499 but that interferred with selection feedback
		return;
	};
	if (!top_is_rectangular) { // condition new on 140803 so that numbers on a nest aren't displayed (e.g. shrinking digits)
		display_stack(stack->rest(),selection_feedback,followers_too,region);
	} else {	// new on 140803
		Sprite::display(selection_feedback,FALSE,region);
	};
	sprite->display_as_a_follower(selection_feedback,region);
};

void Nest::collision_region(TTRegion &region) {
	if (contents_stack != NULL && !stack_temporarily_removed && leader != NULL) { // leader condition added 160603
		contents_stack->first()->collision_region(region);
	} else {
		Sprite::collision_region(region);
	};
};

boolean Nest::save_region() {
	if (!stack_empty() && leader != NULL) { // leader condition added 160603
		return(contents_stack->first()->save_region());
	} else {
		return(Sprite::save_region());
	};
};

void Nest::describe(output_stream &message, Article article) {
//	describe_comment(message);
	if (current_parameter() == NEST_INERT) {
//		print_article(article,message,S(IDS_NEST_WITH_EGG));
      message << S(IDS_NEST_WITH_EGG);
		return;
	};
	Sprite::describe(message,article);
	if (!stack_empty()) {
		message << space() << S(IDS_WITH) << space();
		contents_stack->first()->describe_type(message,INDEFINITE_ARTICLE);
		message << S(IDS_ON_TOP);
	};
	if (label != NULL) {
		string label_text;
		long label_text_length;
		if (label->current_text(label_text,label_text_length)) {
			message << space() << S(IDS_AND_IT_HAS_THE_LABEL) << space() << label_text << endl; 
		};
	};
};

void Nest::generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection) {
	if ((parameter == NEST_INERT && !did(NEST_HATCHED_BIRD)
		  && tt_help_counter == 0) || 
		 tt_help_counter == 1) {
		tt_help_history_action = NEST_HATCHED_BIRD;
//		if (tt_help_counter == 0) {
			describe_pointing(message,tool_in_hand,selection);
//		};
		message << S(IDS_NEST_HATCHED_BIRD_HELP);
		return;
	}; 
	if ((parameter != NEST_INERT && !did(NEST_RECEIVED_ITEM_FROM_BIRD) 
		  && tt_help_counter == 0) || 
		 tt_help_counter == 2) {
		tt_help_history_action = NEST_RECEIVED_ITEM_FROM_BIRD;
//		message << S(IDS_NEST_RECEIVED_ITEM_FROM_BIRD_HELP);
		message << S(IDS_BIRD_RECEIVED_ITEM_HELP);
		return;
	};
	if ((parameter != NEST_INERT && !stack_empty()
		  && !did(NEST_ITEM_REMOVED) && tt_help_counter == 0) ||
		 tt_help_counter == 3) {
		tt_help_history_action = NEST_ITEM_REMOVED;
		message << S(IDS_NEST_ITEM_REMOVED_HELP);
		if (tt_help_counter == 0 && english()) { // avoid definite article
			contents_stack->first()->describe_type(message,DEFINITE_ARTICLE);
		} else {
			message << S(IDS_THINGS);
		};
		message << S(IDS_NEST_ITEM_REMOVED_HELP2);
		return;
	};
	if ((parameter != NEST_INERT && stack_empty()
		  && !did(NEST_ITEM_ADDED) && tt_help_counter == 0) ||
		 tt_help_counter == 4) {
		tt_help_history_action = NEST_ITEM_ADDED;
		message << S(IDS_NEST_ITEM_ADDED_HELP)
				  << S(IDS_NEST_ITEM_ADDED_HELP2);
		return;
	};
	if ((!did(TYPE_HELP_GIVEN) && tt_help_counter == 0) || 
		 tt_help_counter == 5) {
//		tt_help_history_action = TYPE_HELP_GIVEN;
		message << S(IDS_NEST_HELP_GIVEN_HELP2) // order switched on 12/20/96
				  << S(IDS_NEST_HELP_GIVEN_HELP)
				  << S(IDS_NEST_HELP_GIVEN_HELP3);
		if (!(tt_histories[BIRD]&&TYPE_HELP_GIVEN)) {
			message << S(IDS_NEST_HELP_GIVEN_BIRDS);
		};
		no_help_interruptions();
		user_did(NEST,TYPE_HELP_GIVEN);
	};
};

Sprite *Nest::select(TTRegion *region, SelectionReason reason, Sprite *in_hand) {
	if (contents_stack != NULL) {
	  // && by->kind_of() != ROBOT) {
	  // robot was trained to get nest not contents
	  // but robots don't call "select"
		Sprite *contents = contents_stack->first();
		if (!contents->selectable()) { // new on 060505 
			// this can happen if someone grabs a nest while a bird is putting the stack back
			return(NULL);
		};
		contents->now_on_top_of_nest(FALSE); // wasteful? -- asked 160603
		if (leader == NULL) { // new on 160603
			TTRegion contents_region;
			contents->collision_region(contents_region);
			if (!contents_region.overlaps(region)) { // selecting just the nest
				if (reason == MIGHT_DROP_ITEM) { // if dropping and nest has contents then must overlap with contents (added for version 3.19)
					return(NULL);
				};
				return(this);
			}; // otherwise do the following
		};
		switch (reason) {
			case MIGHT_USE_EXPANDER:
				return(this);
			case MIGHT_USE_COPIER:
				return(contents->select(region,reason,in_hand));
			case MIGHT_GRASP:
				if (stack_removed()) {
					return(NULL); // new on 130904 so you can't break things while the bird is moving around her stack
				};
			case MIGHT_USE_VACUUM:
			case MIGHT_DROP_ITEM:
				Sprite *subselection = contents->select(region,reason,in_hand);
				if (subselection != contents) { // selected a sub component
					return(subselection);
				};
				break;
		};
		return(contents);
//		Tool *saved_contents = contents;
//		remove_contents(showing_on_a_screen()?1000:0,TRUE);
//		return(saved_contents);
	};
	if (reason == MIGHT_DROP_ITEM) {
		switch (in_hand->kind_of()) {
			case TEXT:
			case INTEGER:
			case PICTURE:
			case CUBBY:
         case SOUND:
			case NEST: // missing prior to 160904
			case REMOTE_PICTURE: // miss prior to 11104
				return(this);
			default:
			  return(NULL);
		};
	};
	return(this);
};

void Nest::remove_selected_element(Sprite *item, SelectionReason , Sprite *by, boolean add_to_floor) {
	user_did(NEST,NEST_ITEM_REMOVED,by);
	Background *floor;
	if (add_to_floor && by != NULL) { // new on 090100
		floor = by->pointer_to_background();
	} else {
		floor = NULL;
	};
	remove_contents(item,floor);
};

ReleaseStatus Nest::item_released_on_top(Sprite *item, Sprite *by,
													  SpritePointer &recipient, int level) {
//#if TT_DEBUG_ON
//	perform_integrity_checks(item);
//#endif
	if (infinite_stack()) {
//		if (item->kind_of() == NEST &&
//			 item->current_parameter() == current_parameter()) { // same state
//			return(RELEASED_ON_ITS_STACK);
//		};
		by->abort_action_if_robot(IDS_CANT_ADD_TO_STACK_OF_NESTS,
										  NONE_GIVEN,
										  (tt_copy_top_of_stack && !told_user(TOLD_STACK_OF_NESTS_WONT_ACCEPT)));
		user_told(TOLD_STACK_OF_NESTS_WONT_ACCEPT);
		return(RELEASE_IGNORED);
	};
	if (//parameter == NEST_INERT || // if filled with an egg -- huh??
       // commented out since might have lost all its birds and have been saved and restored and robot is dropping something on the nest
		 contained_in_robots_thought_bubble() // updated with ALPHA2 (121003)
//		contained_in(THOUGHT_BUBBLE)
		) {
		by->abort_action_if_robot(IDS_CANT_ADD_TO_NEST);
		return(RELEASE_IGNORED);
	};
	//if (stack_temporarily_removed) { // new on 5/1/98 -- commented out on 071004
 //     ::completed(by);
	//	return(RELEASE_IGNORED);
	//};
	if (contents_stack != NULL) { // && !stack_temporarily_removed) {
		// pass it along
		recipient = contents_stack->first();
		if (leader == NULL && !recipient->colliding(item)) { // new on 291003 to check if overlaps with smaller contents
			return(RELEASE_IGNORED);
		};
		return(recipient->item_released_on_top(item,by,recipient,level+1));
	};
	millisecond duration = default_duration();
	switch (item->kind_of()) {
	  case BIRD:
		  if (!by->still_running()) break; // users can't put them in nest
	  case PICTURE:
		  // as of 080399 Pictures can be dropped on nests
//		  if (!by->still_running() && tt_log_version_number < 15) break; // users can't put them in nest
	  case NEST: // moved here on 180799 so nests can be bammed
	  case INTEGER:
	  case TEXT:
     case SOUND:
	  case REMOTE_PICTURE:
	  case CUBBY:
//      ready_to_receive_flag = FALSE;
		  if (receive_item(item,by,duration,this,item)) { // went ok
			  item->set_available_for_indexing(TRUE); // so it can be removed
           record_event(RELEASE_ITEM_ON,by,floor,this);
           // following commented out on 4/23/97
           // interferes with use of nests on the floor
//           if (by->kind_of() == ROBOT_IN_TRAINING) {
//              set_available_for_indexing(FALSE);
//           };
           item->now_released_on(this,by);
			  user_did(NEST,NEST_ITEM_ADDED,by);
			  return(RELEASED_ITEM_ON_NEST);
		  } else {
			  return(RELEASE_IGNORED);			  
		  };
	};
	by->abort_action_if_robot(IDS_CANT_ADD_TO_NEST);
	return(RELEASE_IGNORED);
};

boolean Nest::receive_item(Sprite *item, Sprite *by,
									millisecond duration, Sprite *original_recipient, Sprite *original_item) {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Debug target is a nest receiving something."));
	};
#endif
   if (inside_vacuum()) {
      by->abort_action(IDS_ROBOT_MISSING_RECIPIENT_ABORT);
      return(FALSE);
   };
   if (contents_stack != NULL && !stack_temporarily_removed) {
		return(contents_stack->first()->receive_item(item,by,duration,original_recipient,original_item)); 
	};
//	saved_by = by;
//	completed(action_status); // replaced by the following on 130799 so a robot waits for the item to be accepted
	if (item->kind_of() == NEST) { // new on 180799
		if (duration > 0) {
			// on 160204 replaced 'by' with NULL since it is way to hard to reconstruct the connections to undo a bam of nests
			// on 140605 replaced NULL with by below since otherwise a robot will wait for ever for the smash
			call_in_a_mouse(this,item,item->pointer_to_background(),by,FALSE,TRUE,duration,original_recipient,original_item); 
		} else {
			set_to_future_value(item,original_recipient,by);
			item->remove_from_floor(FALSE); // new on 141204
			item->destroy(by); // new on 141204
			by->completed();
		};		
	} else {
//		saved_status = action_status;
      set_saved_by(by); // replaces the above on 171102
		add_item(item,duration,by);
	};
//#if TT_DEBUG_ON
//	perform_integrity_checks(this);
//#endif
	return(TRUE); // went fine
};

void Nest::set_to_future_value(Sprite *other, Sprite *original_recipient, Sprite *by) { // new on 180799
	// link the nests so that their birds fly to both
	// other must be a nest - worth checking just in case?
	if (!tt_old_nest_merging_behavior) {
		merge(((Nest *) other)); // new on 140205
	} else {
		original_nest->merge(((Nest *) other));
	};
	if (by != NULL) { // new on 140605
		by->completed();
	};	
};

/*
void Nest::merge(Nest *other) {
	Nest *other_original = other->pointer_to_original_nest(); // moved here from caller in Nest::set_to_future_value on 291101
	if (other_original == this) return; // already bammed
	Sprites *others_nest_copies = other_original->pointer_to_nest_copies(); // new on 180799
	if (others_nest_copies != NULL) {
		Sprites *remaining = others_nest_copies;
		other_original->set_nest_copies(NULL); // transferred here
		while (remaining != NULL) {
			Nest *nest = (Nest *) remaining->first();
			nest->decrement_ref_count(); // since was incremented when added to nest_copies of other_original
			nest->set_original_nest(this);
			add_copy(nest);
			remaining->set_first(NULL);
			remaining = remaining->rest();
		};
		delete others_nest_copies; // just the list - the nests have been removed
	};
	other->set_original_nest(this); // was other_original-> prior to 291101 
	if (other != other_original) { // conditional new on 291101
		add_copy(other_original); // commented out on 051199 -- restored on 291101
	};
	other->increment_ref_count(this); // added 051199 // was other_original-> prior to 291101
	other->set_forwarding(TRUE); // so it won't remove itself when deleted // was other_original-> prior to 291101
};
*/
void Nest::merge(Nest *other) {
	// new version on 291101 to preserve the semantics of there being a forwarder of all messages to other going to this
	if (other == this) return; // new on 160904 -- can happen while unbamming a merge
	if (!tt_old_nest_merging_behavior) {
		other->set_forwarding_to(this);
		return;
	};
	Nest *other_original = other->pointer_to_original_nest(); // moved here from caller in Nest::set_to_future_value on 291101
	if (other_original == this) return; // already bammed -- huh??
//	Sprites *others_nest_copies = other_original->pointer_to_nest_copies(); // new on 180799
//	if (others_nest_copies != NULL && other == other_original) { 
	// this guy can't be the original since it's going to forward so pick one of the copies and make it the original
//		Nest *first_nest = (Nest *) others_nest_copies->first();
//		first_nest->set_original_nest(first_nest);
//		first_nest->set_nest_copies(others_nest_copies->rest());
//		this should have informed the other nest copies of the change
//		other->set_nest_copies(NULL);
//		others_nest_copies->set_first(NULL);
//		others_nest_copies->set_rest(NULL);
//		delete others_nest_copies; // just this cons
//	};
	if (other == other_original) { // other is being clobbered so add guy I'm merging with to my nest copies
		other->add_copy(this);
	} else { // forward to this
		other->set_forwarding(TRUE);
		other->set_original_nest(this);
	};
};

void Nest::add_item(Sprite *item, millisecond duration, Sprite *by) {
	if (!tt_old_nest_merging_behavior) {
		if (forwarding_to() != NULL) {
			ultimate_forwarding_to_or_original_nest()->add_item(item,duration,by); // new on 140205
			return;
		};
	};
	if (forwarding()) { // new on 051199 // must be old behavior or loaded something in the middle of old nest forwarding behavior
		original_nest->add_item(item,duration,by);
		return;
	};
	// following removed on 150505
	//if (stack_temporarily_removed) { // new on 090105 since happened in Logotron report error 19.2
	//	stack_restored(); // restore the stack before doing this
	//};
   if (!showing_on_a_screen()) { // removed duration == 0 || since that optimization caused havoc when a bird is flying and F7 is pushed
      accept_contents((by != NULL && by->kind_of() == BIRD),item);  
      return;
   };
//	item->remove_from_floor(FALSE,FALSE); // commented out on 151199 // no problem if not already there
//	add_follower(item,FALSE); // commented out on 151199 since isn't on the nest yet and this messes up true_size() and the like
//	tt_screen->add(item); // commented out on 151199
//	item->save_size();
	if (floor != NULL) { // new on 151199
		floor->add_item(item,TRUE,FALSE,FALSE);
	};
	city_coordinate contents_width, contents_height,
						 contents_llx, contents_lly;
	location_and_size_to_cover(contents_llx,contents_lly,
										contents_width,contents_height);
	set_new_contents(item);
	// use of ultimate leader here is in case nest is in a cubby with
	// lower priority -- could alternatively make sure that doesn't happen
//	item->set_priority(ultimate_leader()->priority()-1); // on top of nest
	item->set_priority(priority()); // on top of nest
	item->animate_size_and_location(contents_width,contents_height,
											  contents_llx,contents_lly,
											  duration);
	if (by->kind_of() == BIRD) {
		item->do_after(duration,this,ACCEPT_CONTENTS_BY_BIRD_CALLBACK);
	} else {
		set_selectable(FALSE); // new on 130904 so you can't grab the nest after dropping something on it
		item->do_after(duration,this,ACCEPT_CONTENTS_BY_NON_BIRD_CALLBACK);
	};
};

void Nest::location_and_size_to_cover(city_coordinate &cover_llx, city_coordinate &cover_lly,
												  city_coordinate &cover_width, city_coordinate &cover_height) {
	Sprite *container = leader;
	Sprite *contents = this;
	while (container != NULL && container->kind_of() == NEST) {
		// robots can put nests in nests ...
		contents = container;
		container = container->pointer_to_leader();
	};
	if (container != NULL && container->kind_of() == CUBBY) {
		// if nest is in cubby use cubby hole dimensions
		((Cubby *) container)->cubby_insides(contents,
														 cover_llx,cover_lly,
														 cover_width,cover_height);
	} else {
		cover_width = true_width(); // was 3/2 of that
		cover_height = true_height();
		cover_llx = llx; // -(cover_width-width)/2;
		cover_lly = lly;
		if (leader == NULL) { // new on 160603 so that there is room to pick it up and it is less confusing - thanks Yishay
			cover_llx += cover_width/5;
			cover_lly += cover_height/5;
			cover_width /= 2;
			cover_height /= 3;
		};
/*
		cover_width = width;
		cover_height = new_contents->height_given_width(width);
		cover_llx = llx;
		cover_lly = lly+(height-cover_height)/2;
*/
	};
};

//void Nest::enqueue(Sprite *item) {
//	new_contents = item;
//	accept_contents();
//};

void Nest::insert_at_end_of_contents(Sprite *item) {
  if (contents_stack == NULL) {
	  contents_stack = new Sprites(item);
//	  give_size_and_location_to_cover_to(item);
     add_follower(item);
  } else {
	  contents_stack->insert_at_end(item);
  };
  item->increment_ref_count(this); // new on 170799
};

void Nest::fit_contents() {
	if (contents_stack != NULL) {
		give_size_and_location_to_cover_to(contents_stack->first());
	};
};

void Nest::give_size_and_location_to_cover_to(Sprite *cover) {
	if (cover->kind_of() == NEST) {
		cover = cover->dereference();
	};
	city_coordinate contents_width, contents_height,
						 contents_llx, contents_lly;
	location_and_size_to_cover(contents_llx,contents_lly,
										contents_width,contents_height);
//	cover->adjust_size_and_location(contents_width,contents_height,
//											  contents_llx,contents_lly);
	cover->set_true_size_and_location(contents_width,contents_height,
												 contents_llx,contents_lly,TO_FIT_ON_A_NEST);
//   if (cover->kind_of() == CUBBY) {
//      cover->refit_contents(); // improve geometry
//   };
//	cover->record_offsets(); // seems to be necessary
};

void Nest::refit_contents() {
   if (contents_stack != NULL) {
      contents_stack->first()->refit_contents();
   };
};

#if TT_DIRECT_PLAY
boolean Nest::ready_to_receive_messages() { 
	return(!proxy() || pending_message_count <= tt_max_pending_message_count);
};
#endif

void Nest::about_to_put_stack_back() { // new on 170703
	if (new_contents != NULL) {
		new_contents->finish_instantly();
	};
};

void Nest::accept_contents(boolean by_bird, Sprite *item, int retries) {
//#if TT_DEBUG_ON
//	if (item != NULL && !item->selectable()) {
//		debug_this();
//	};
//#endif
	if (!tt_old_nest_merging_behavior) {
		if (forwarding_to() != NULL) {
			ultimate_forwarding_to()->accept_contents(by_bird,item); // new on 140205
			return;
		};
	};
	if (forwarding()) { // new on 051199 // must be old behavior or loaded something in the middle of old nest forwarding behavior
		original_nest->accept_contents(by_bird,item);
		return;
	};
	if (!by_bird) {
		set_selectable(TRUE); // new on 130904 so you can once again pick up the nest
	};
	if (item != NULL) set_new_contents(item);   // called by callback - use new_contents now
#if TT_DIRECT_PLAY
	if (proxy()) {
//		new_contents->set_being_marshalled(TRUE); -- commented out on 030500 - obsolete flag???
		new_contents->remove_from_floor(FALSE);
		if (direct_play == NULL && nth_address < address_count) {
			direct_play = create_direct_play_object(addresses[nth_address]);
		};
      if (direct_play == NULL) { // new on 310702 that if not running direct play to just act like a sink
         new_contents->destroy(); // no longer on this machine
			new_contents = NULL;
         return;
      };
		if (player_id == 0 && pending_messages == NULL) {
			while (TRUE) { // new on 300700 to loop to try alternative IP addresses
				if (::join_a_direct_play_session(direct_play,nest_guid,this)) { 
					break; // all is fine
				} else if (nth_address+1 < address_count) { // try next one
					release_direct_play_object(direct_play);
					nth_address++;
					direct_play = create_direct_play_object(addresses[nth_address]);
				} else {
					break; // action_aborted ?? warn?? what??
				};
			};
		};
		if (player_id == 0 || message_queue_size(direct_play) > tt_max_direct_play_queue_size) { 
			// not yet connected or outgoing queue is full
#if TT_DEBUG_ON
			debug_this();
#endif
			if (pending_message_count > tt_max_pending_message_count) { // new on 290400
				tt_global_picture->do_after(1000,this,ACCEPT_CONTENTS_BY_BIRD_CALLBACK);
				return; // and saved status isn't updated so robot will wait
			};
			insert_at_end(new_contents,pending_messages); // new on 280400 to keep the ordering
			pending_message_count++;
			if (player_id != 0) {
#if TT_DEBUG_ON
				if (tt_debug_mode == 280400) {
					log("Message queue nearly full using pending messages now.");
				};
#endif
				tt_global_picture->do_after(1000,this,SEND_PENDING_MESSAGES_CALLBACK);
			};
		} else {
			if (pending_messages != NULL) {
				send_pending_messages(); 
			};
			bytes outgoing_message_buffer = new byte[max_encoding_buffer_size];
			outgoing_message_buffer[0] = current_message_version_number;
			outgoing_message_buffer[1] = MESSAGE_FROM_BIRD_TO_NEST;
			long encoding_size;
			// return_encoding should really return bytes not string
			bytes encoding = (bytes) new_contents->return_encoding(encoding_size,FALSE,TRUE,(string) outgoing_message_buffer+2); 
		   // don't need to trim - but size is important - and it is important to save version number
			if (encoding != NULL) {
				if (send_network_message(direct_play,player_id,nest_guid,outgoing_message_buffer,encoding_size+2,this) == TT_SEND_RETRY && retries < 3) {
					// maybe the recipient went down and restarted 
					drop_direct_play_object();
					delete [] outgoing_message_buffer; // was encoding prior to 080500
					accept_contents(by_bird,item,retries+1);
					return;
				};
			};	
			delete [] outgoing_message_buffer; // copied here on 300400 and changed from encoding on 080500
			new_contents->destroy(); // no longer on this machine
			new_contents = NULL;
		};
      action_completed(); // even though message might not really have been delivered - another type of send??
		return;
	};
#endif
//	if (original_nest == this && by_bird) {
//		Sprites *remaining = nest_copies;
//		while (remaining != NULL) {
//			Nest *nest = (Nest *) remaining->first();
//			nest->accept_contents(by_bird,new_contents->copy(floor));
//			remaining = remaining->rest();
//		};
//		if (deleted_flag) return; // original may be deleted and copies remain
//	};
//#if TT_DEBUG_ON
//	perform_integrity_checks(this);
//	perform_integrity_checks(new_contents);
//	tt_error_file() << "Accepting " << new_contents << " in nest with bird count " << bird_count << " "
//					<< this << " on frame " << tt_frame_number << endl; 
//#endif
#if TT_DEBUG_ON
	if (tt_debug_mode == 518) {
		print(tt_error_file());
		tt_error_file() << " receiving ";
		new_contents->print(tt_error_file());
		tt_error_file() << " on frame " << tt_frame_number << endl;
	};
#endif
	if (contents_stack == NULL) { 
		new_contents->now_on_top_of_nest(TRUE); // new on 190499
		// first guy on this nest so fix its geometry
		give_size_and_location_to_cover_to(new_contents);
// commented out on 150300 since this redirected remote looks in unexpected ways
// but it also made sense ... - except it didn't even happen if stack wasn't empty
//		if (floor != NULL && floor->pointer_to_picture() != NULL) {
//			new_contents->now_on_back_of(floor->pointer_to_picture());
//		};
	};
	if (new_contents->pointer_to_leader() == this) {
		remove_follower(new_contents); // added as not fully inside
	} else {
		new_contents->remove_from_floor(FALSE,TRUE);
		// second arg added 150505 so doesn't become a "turd" -- e.g. Logotron bug 19.2
	};
//#if TT_DEBUG_ON
//	perform_integrity_checks(new_contents);
//#endif
	// might be nest (picked up by robot)
	if (new_contents->kind_of() == NEST) { // conditional added on 020899 so when a REMOTE_LOOKS is delivered the controlee is left alone
		Sprite *dereferenced_contents = new_contents->dereference();
		if (dereferenced_contents != new_contents) {
			dereferenced_contents->set_size_and_location(new_contents->current_width(),new_contents->current_height(),
																		new_contents->current_llx(),new_contents->current_lly());
		};
	};
	//	new_contents->update_display();
	if (stack_temporarily_removed) {
		display_new_contents = TRUE; 
		set_clean_status(DIRTY);
/*
		millisecond duration = showing_on_a_screen()?1000:0; // for now
		Tool *top = (Tool *) contents_stack->first();
		city_coordinate cover_width, cover_height, cover_llx, cover_lly;
		location_and_size_to_cover(top,
											cover_llx,cover_lly,
											cover_width,cover_height);
		// width should still be fine
		top->animate_location(cover_llx,cover_lly,duration,
									 NULL,this,stack_restored_callback);
*/
	} else if (!by_bird && contents_stack != NULL) {
		// placing something on top of stack (by robot or programmer -- not bird)
      // following was commented out but is needed (e.g. sieve of erasthenes)
//		contents_stack->first()->remove_from_floor(FALSE); // moved here then commented out on 070105 since sprite::remove_follower now deals with this
		if (contents_stack->first()->pointer_to_leader() == this) remove_follower(contents_stack->first());
//		tt_screen->remove(new_contents); // commented out on 070604 since is called below as well
		add_follower(new_contents);
		new_contents->increment_ref_count(this); // new on 170799 - since added to contents_stack
	};
	if (contents_stack == NULL) {
		contents_stack = new Sprites(new_contents);
		add_follower(new_contents);
//		new_contents->increment_ref_count(this); // new on 170799 - commented out since done below
	} else {
      new_contents->remove_from_floor(FALSE,TRUE); // I believe this is always a no-op (noted 090105)
		contents_stack->insert_at_end_if_not_member(new_contents);
	};
	new_contents->increment_ref_count(this); // new on 170799 - since added to contents_stack
	tt_screen->remove(new_contents);
   if (!stack_temporarily_removed) {
		new_contents->set_selectable(TRUE); // bird may have left it unselectable
	};
	if (!stack_temporarily_removed && robot_to_try != NULL) {
		if (new_contents->kind_of() == NEST && 
			 ((Nest *) new_contents)->stack_empty()) {
			// pass suspension record along to nest on top
			((Nest *) new_contents)->when_non_empty_try(robot_to_try,cubby_to_try_robot_on);
		} else {
			millisecond duration = default_duration();
			cubby_to_try_robot_on->decrement_busy_counter(); // unlock it
//#if TT_DEBUG_ON
//	perform_integrity_checks(new_contents);
//#endif
			robot_to_try->try_clause(cubby_to_try_robot_on,duration);
//#if TT_DEBUG_ON
//	perform_integrity_checks(new_contents);
//#endif
		};
		set_robot_to_try(NULL); // was assignment and in each branch above prior to 130203
		set_cubby_to_try_robot_on(NULL); // new on 241104 since not needed anymore
	} else if (leader != NULL && leader->kind_of() == NEST) {
		Nest *leader_nest = (Nest *) leader; 
		set_robot_to_try(leader_nest->transfer_robot_to_try());
		cubby_to_try_robot_on = leader_nest->transfer_cubby_to_try_robot_on();
		// is this always right?
	};
   action_completed();
   // rewritten on 171102
	//if (saved_status != NULL) { // added on 130799
	//	completed(saved_status);
	//	saved_status = NULL;
	//};
//	new_contents->save_size(); // in case it doesn't have one
//#if TT_DEBUG_ON
//	perform_integrity_checks(this);
//#endif
};

void Nest::stack_restored() {
	if (contents_stack == NULL) {
		return; // new on 200405 since can happen when birds are flying to nests in boxes in nests (e.g. Hotel Infinity)
	};
	Sprite *top = contents_stack->first();
	top->set_selectable(TRUE);
	if (contents_stack->rest() != NULL) { // not clear why I do this and how it can be NULL
		contents_stack->rest()->first()->set_selectable(TRUE);
#if TT_DEBUG_ON
	} else if (tt_debug_mode == 518) {
		print(tt_error_file());
		tt_error_file() << " expected to have more than 1 item on it when restored." << endl;
#endif
	};
	set_selectable(TRUE);
	top->remove_from_floor(FALSE);
	if (top->pointer_to_leader() == NULL) { // can be this -- if something else there is a problem -- condition new on 111204
		add_follower(top);
	};
//   if (showing_on_a_screen()) tt_screen->add(top); 
	// commented out on 031202 since left top as element of screen even after nest is sent away (thanks Gordon)
	top->set_priority(priority());
	stack_temporarily_removed = FALSE;
	give_size_and_location_to_cover_to(top); 
	// moved here from before add_follower on 180903 since this behaves differently depending upon whether the stack has been resotred
//	set_clean_status(DIRTY); // new on 180903
//#if TT_DEBUG_ON
//	print(tt_error_file());
//	tt_error_file() << " stack restored" << endl;
//#endif
	display_new_contents = FALSE; 
	set_clean_status(DIRTY); // restored on 290499 in case cached with old appearance
	if (waiting_birds != NULL) {
		Sprite *next_bird;
		waiting_birds = waiting_birds->pop(next_bird);
		remove_stack((Bird *) next_bird);
	} else if (robot_to_try != NULL && cubby_to_try_robot_on != NULL) { 
		// new on 241104 since robot may tried to run while bird was returning stack
		robot_to_try->try_clause(cubby_to_try_robot_on,default_duration());
		set_robot_to_try(NULL);
		set_cubby_to_try_robot_on(NULL);
	};
};

Sprite *Nest::released(Sprite *by, boolean top_level, SpecialDrop special_drop) {
	// do this first to update priority etc.
	Sprite *released_item = Sprite::released(by,top_level,special_drop);
	// following moved here on 210103 since is completed in all the following paths (and bird_has_hatched() caused problems)
	if (by != NULL) { // new on 161202 to make the robot wait for the bird to hatch
		by->add_completion();
	};
	if (parameter == NEST_INERT && contents_stack != NULL) {
		set_parameter(NEST_EMPTY);
      contents_stack->first()->released(by); // really should conjoin status??
	} else if (parameter == NEST_INERT && hatching_bird == NULL) { // if egg still inside
		// added test for hatching_bird in case this is dropped twice quickly
//		saved_status = status; // replaced by the following on 171102
      set_saved_by(by);
//		Sprite *container;
//    	Background *birds_floor;
//		boolean on_picture_flipside
//		  = (floor != NULL && (container = floor->pointer_to_picture()) != NULL);
//		if (on_picture_flipside) {
//			birds_floor = container->pointer_to_background();
//		} else {
//			birds_floor = floor;
//		};
		hatching_bird = new Bird(0,0,0,this);
		if (label != NULL) {
			hatching_bird->add_t_shirt(label->copy());
		};
      hatching_bird->set_background(floor);
      // why??
//      if (floor != NULL) floor->add_item(hatching_bird);
		// create bird now so if training order of working set fixed
		record_event(HATCH_BIRD,by,floor,hatching_bird,TRUE,0,ADD_TO_WORKING_SET_REGARDLESS); 
		// above moved here from else branch on 210103 since this should apply even if back on picture 
		if (floor != NULL && floor->pointer_to_picture() != NULL) {
			// on back of a picture
			bird_has_hatched();
		} else {
			do_after(default_duration(1000),this,HATCH_BIRD_CALLBACK);
			user_did(NEST,NEST_HATCHED_BIRD,by);
		};
	} else {
      ::completed(by);
	};
	if (label != NULL) { // new on 110604
		label->released(by,FALSE,special_drop);
	};
	return(released_item);
};

void Nest::unselect() { // new on 110604
	if (label != NULL) {
		label->unselect();
	};
	Sprite::unselect();
};

void Nest::recursively_notice_on_screen_change(boolean on_screen) {
	if (contents_stack == NULL) {
		notice_on_screen_change(on_screen);
	} else {
		contents_stack->first()->recursively_notice_on_screen_change(on_screen);
	};
};

void Nest::hatch_bird() {
	if (showing_on_a_screen()) {
		play_sound(BIRD_HATCH_SOUND);
	};
	set_parameter(BIRD_HATCH);
//	set_cycle_stopped_callback(bird_has_hatched_callback);
	tt_global_picture->do_after(default_duration(total_cycle_duration()),this,BIRD_HAS_HATCHED_CALLBACK); 
	// re-written since this will happen even if the nest is in a truck and won't finish
//	save_size(); // mostly so bird is made a good size even if dropped in cubby
};

void Nest::bird_has_hatched() {
	Sprite *container;
	boolean on_picture_flipside = (floor != NULL && (container = floor->pointer_to_picture()) != NULL);
	city_coordinate delta_x = 0,delta_y;
	if (on_picture_flipside) {
		container = container->ultimate_flipside_picture(); // added 250399
		delta_y = 2*container->current_height();
	} else {
		container = this;
		if (leader == NULL) {
			delta_y = 6*tile_height;
		} else {
			delta_x = -3*tile_width;
			delta_y = leader->current_height()+tile_height;
		};
	};
	set_parameter(NEST_EMPTY);
	set_clean_status(DIRTY);
	city_coordinate bird_x, bird_y;
	boolean nest_off_screen_but_not_bird = (!showing_on_a_screen() && hatching_bird->showing_on_a_screen());
	if (nest_off_screen_but_not_bird) { // new on 210201 since nest may have traveled away - e.g. in a truck
		bird_x = -2*tile_width;
		bird_y = 12*tile_height; // appear to come from door
	} else {
		container->lower_left_corner(bird_x,bird_y);
		if (floor != NULL && floor->on_the_floor()) {
			if (bird_x < -2*tile_width) {
				bird_x = 0;
			} else if (bird_x > ideal_screen_width-tile_width) {
				bird_x = 18*tile_width;
			};
			if (bird_y < -2*tile_height) {
	     		bird_y = 0;
			} else if (bird_y > ideal_screen_height-tile_height) {
				bird_y = 18*tile_height;
			};
		};
   };
//	city_coordinate bird_height;
//	if (x_scale == 256) { // "natural size"
//		bird_height = 0;
//	} else {
//		bird_height = 3*saved_height/4;
//		if (bird_height < 3*tile_height) bird_height = 3*tile_height;
		// small birds are a problem especially if nest is animating
		// to a reasonable size
//	};
	if (!on_picture_flipside) {
		hatching_bird->set_size_percents(get_width_percent(),get_height_percent());
	};
	hatching_bird->set_parameter(BIRD_FLY_UP);
	hatching_bird->set_selectable(FALSE); // too hard to deal with programmer grabing birds
	hatching_bird->move_to(bird_x,bird_y);
//	if (!on_picture_flipside) {
//		bird->move_by((xn*bird->current_width())/xd,(yn*bird->current_height())/yd);
//	};
//	hatching_bird->pointer_to_background()->add_item(hatching_bird); // huh?
	// rewritten on 051202 to be less weird and to work better with new XML stuff
	if (floor != NULL) floor->add_item(hatching_bird);
	hatching_bird->update_display();
	millisecond duration = hatching_bird->default_duration(1000);
	if (on_picture_flipside) {
		hatching_bird->set_size_and_location(1,1,bird_x,bird_y);
		if (duration > 0) play_sound(BIRD_HATCH_SOUND);
	};
	int width_percent = get_width_percent(); 
	int height_percent = get_height_percent();
	if (leader != NULL && (width_percent < 100 || height_percent < 100)) {
		hatching_bird->animate_to_good_size(duration,tt_log_version_number >= 64?tt_toolbox:NULL); // new on 210201  because nests can be very small and then bird is hard to see or use
		// but if user is dropping just the nest then must have made Tooly small and want small things
	} else {
		hatching_bird->animate_to_size_percents(width_percent,height_percent,duration);
	};
	if (nest_off_screen_but_not_bird) { // new on 210201
		delta_x = 12*tile_width;
		delta_y = -4*tile_height;
	} else if (bird_y > 10*tile_height) {
//		if (!on_picture_flipside && leader == NULL) {
//			delta_y = -6*delta_y;
//		} else {
			delta_y = -delta_y;
//		};
	};
//	if (bird->current_lly() < 10*tile_height) {
		// why not fly??
//		millisecond fly_up_time 
//			= bird->showing_on_a_screen()?bird->total_cycle_duration():0;
//		bird->animate_delta_location(0,delta_y,fly_up_time);
//		bird->set_cycle_stopped_callback(bird_morph_to_inert_callback);
//	} else {
		hatching_bird->fly_to(bird_x+delta_x,bird_y+delta_y,
						          BIRD_MORPH_TO_INERT_CALLBACK,duration);
//	};
	increment_bird_count();
#if TT_DEBUG_ON
   if (tt_debug_mode == 13579) {
      print(tt_error_file());
      tt_error_file() << " incrementing bird count due to bird_has_hatched. ";
      hatching_bird->print(tt_error_file());
      tt_error_file() << endl;
   };
#endif
   hatching_bird = NULL; // no longer needs to know about it
//   action_completed(); 
	// rewritten on 161202 since don't want the robot to continue until these things here are restored
	do_after(duration,this,ACTION_COMPLETED_CALLBACK);
};

// commented out on 150300 since may have a remote picture on top and why should it change "alligience"? And why only the top guy?
//void Nest::now_on_back_of(Picture *picture) {
//	if (contents_stack != NULL) {
//		contents_stack->first()->now_on_back_of(picture);
//	};
//};

void Nest::set_priority(long new_priority) {
	Sprite::set_priority(new_priority);
	if (contents_stack != NULL) {
		contents_stack->first()->set_priority(new_priority);
	};
};

/*
void Nest::remove_bird() {
//   bird_inside = FALSE;
	ready_to_receive_flag = TRUE;
	remove_follower(contents,floor);
   floor->add_item(contents);
   contents = NULL;
};
*/

//void Nest::set_size_and_location(city_coordinate width,
//											city_coordinate height,
//											city_coordinate llx,
//											city_coordinate lly,
//											ChangeReason reason) {
//	Sprite::set_size_and_location(width,height,llx,lly,reason);
//	if (contents_stack != NULL && !stack_temporarily_removed) {
//		contents_stack->first()->set_size_and_location(width,height,llx,lly);
//	};
//};

//commented out on 180499 since nests now display as much of their stack as they can
boolean Nest::size_is_true() {
	if (//tt_log_version_number > 14 || 
		 contents_stack == NULL || stack_temporarily_removed) {
		return(Sprite::size_is_true());
	} else {
		return(contents_stack->first()->size_is_true());
	};
};

void Nest::set_true_size_and_location(city_coordinate width, city_coordinate height,
												  city_coordinate llx, city_coordinate lly,
												  ChangeReason reason) {
	// this fixes the problem that nest's true size is adjusted
	// which isn't quite right for followers...
	Sprite::set_true_size_and_location(width,height,llx,lly,reason);
	if (contents_stack != NULL && !stack_temporarily_removed) {
//		contents_stack->first()->set_size_and_location(width,height,llx,lly,reason);
      fit_contents();
	};
};

void Nest::when_non_empty_try(Robot *robot, Cubby *cubby) {
	set_robot_to_try(robot);
	set_cubby_to_try_robot_on(cubby);
};

Sprite *Nest::pointer_to_contents(boolean even_if_removed) {
	if (contents_stack == NULL) {
		return(NULL);
	} else if (stack_temporarily_removed) { // force robot to wait
		if (even_if_removed && contents_stack->rest() != NULL) { // new on 120203 -- do I really need the second test??
			Sprite *item = contents_stack->first();
			if (item->pointer_to_leader() == NULL) {
				item->remove_from_floor(FALSE); // new on 210105 since otherwise the following can generate spurious warnings
				add_follower(item,TRUE,INSERT_AT_END,TRUE); // new on 120203
				// idea is to let the robot have the top of the moved stack
				return(item);
			};
			return(NULL); // hopefully will wait until it is available			
		};
		return(NULL);
	} else {
		return(contents_stack->first());
	};
};

Sprite *Nest::dereference() {
	if (contents_stack == NULL) {
		if (new_contents != NULL) {
			return(new_contents->dereference());
		};
		return(this);
	} else {
		return(contents_stack->first()->dereference());
	};
};

long Nest::stack_size() {
   if (contents_stack == NULL) return(0);
   return(contents_stack->length());
};

boolean Nest::contains(Sprite *item) {
	if (item == this) return(TRUE);
	if (contents_stack != NULL) {
		return(contents_stack->first()->contains(item));
	};
	return(FALSE);
};

// obsolete??
void Nest::adjust_size_and_location(city_coordinate &item_width,
												city_coordinate &item_height,
												city_coordinate &item_llx,
												city_coordinate &) {
//   if (contents_stack != NULL && !stack_temporarily_removed) return; // covered
//	city_coordinate new_item_width = width_given_height(item_height);
//   item_llx += (item_width-new_item_width)/2;
//	item_width = new_item_width;
	scale_to_fit(); // to be sure that true_height is right
	city_coordinate real_height = true_height();
	if (real_height == 0) return;
	city_coordinate new_item_height = (height*item_height)/real_height;
	city_coordinate new_item_width = width_given_height(new_item_height);
	if (new_item_width < item_width) {
		item_llx += (item_width-new_item_width)/2;
		item_width = new_item_width;
	};
	item_height = new_item_height;
};

Path *Nest::path_to(Sprite *target, boolean &ok) {
	Sprite *nest_top = pointer_to_contents(TRUE); // TRUE arg added on 120203
	if (nest_top == NULL) {
		ok = FALSE;
		return(NULL);
	};
	if (nest_top == target) { // probably too late to rationalize this with indicator that top of nest is desired
		return(NULL);
	} else {
		return(nest_top->path_to(target,ok));
	};
};

void Nest::set_label(Text *new_label) {
	if (label != NULL) {
		remove_follower(label);
		label->destroy();
	};
	label = new_label;
	if (label != NULL) {
		if (label->pointer_to_leader_offsets() == NULL) {
			update_display(); // new on 140802 to improve the geometry of the label when loaded
			city_coordinate new_width = (width*541)/1000;
			city_coordinate new_height = (height*348)/1000;
			label->set_size_and_location(new_width,new_height,
												  llx+(width*476)/1000-new_width/2, // location is for the center
												  lly+(height*357)/1000-new_height/2);
			add_follower(label,TRUE,INSERT_UNORDERED); // args added on 080500 so label is before contents in followers list so when selected doesn't display label and contents in wrong order
		} else {
			// new on 260103
			add_follower(label,TRUE,INSERT_UNORDERED,TRUE);
		};
	};
};

Sprite *Nest::respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected, Sprite *by,
										    boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless) {
	if (label == NULL) {
		if (!rubout(key) && !extended) {
//			string name = new character[2]; 
//			name[0] = key;
//			name[1] = NULL; // terminate
//			set_label(make_label(name,tt_black));
			wide_string name = update_string((wide_string) NULL,key,extended); // on 240800 replaced the above with this
			set_label(make_label((string) NULL,tt_black));
			label->set_wide_text(name,wide_string_length(name),FALSE);
		};
	} else if (by->can_send_keyboard_editing_keys()) { // new on 110604
		// replaced label with original_recipient on 030105 so robots can record this properly
		label->respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,size_constrained_regardless);
		return(this);
	} else {
		wide_string text = label->copy_text_string();
		wide_string new_text = update_string(text,key,extended);
		boolean just_modified = (text == new_text);
		if (new_text == NULL || new_text[0] == NULL) {
			remove_follower(label);
			label->destroy();
			label = NULL;
		} else {
			label->set_wide_text(new_text,wide_string_length(new_text),just_modified);
		};
   };
//   completed(the_action_status); // replaced this with the following on 080400
   return(Sprite::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,size_constrained_regardless));
};

ArrowKeyEditing Nest::arrow_key_editing_status() { // new on 120604
	if (label != NULL) {
		return(label->arrow_key_editing_status());
	} else { 
		return(Sprite::arrow_key_editing_status());
	};
};

void Nest::set_new_contents(Sprite *item, boolean remove_from_floor_too) {  // new on 170799 so if item is destroyed ref counts save things
#if TT_DEBUG_ON
	if (item == this) {
		log("setting nest's contents to itself!");
		return;
	};
#endif
	if (new_contents != NULL) {
		if (item != NULL || tt_log_version_number < 76) { // new on 030605
			new_contents->finish_instantly(); // added on 051199 since simultaneous access broke otherwise
		};
		if (new_contents != NULL) {
			if (remove_from_floor_too) { // conditional new on 140201
				new_contents->remove_from_floor(FALSE,TRUE); // new on 070600 
				// second arg added 150505 so doesn't become a "turd" -- e.g. Logotron bug 19.2
			};
			// could it also be that we need to run remove_follower(new_contents); too?
			new_contents->destroy(this); // decrement ref count
		};
	};
	if (item != NULL) {
		item->increment_ref_count(this);
	};
	new_contents = item;
};

//void Nest::become(Sprite *other) { // noticed obsolete on 140205
//	if (other == NULL || other->kind_of() != NEST) return;
//	Nest *other_nest = (Nest *) other;
//	Text *others_label = other_nest->pointer_to_label();
//	Text *label_copy = NULL;
//	if (others_label != NULL) { // conditional new on 210203 - Marios had a crash
//		label_copy = (Text *) (others_label->copy());
//		other_nest->set_label(NULL);
//	};
//	set_label(NULL); // since following removes followers
//	Sprite::become(other);
//	if (label_copy != NULL) { // conditional new on 210203
//		set_label(label_copy);
//	};
//	set_nest_copies(other_nest->pointer_to_nest_copies());
//	other_nest->set_nest_copies(NULL);
//	if (other_nest->pointer_to_original_nest() == other_nest) {
//		set_original_nest(this);
//	} else {
//		set_original_nest(other_nest->pointer_to_original_nest());
//	};
//	other_nest->set_original_nest(NULL);
//	if (!tt_old_nest_merging_behavior) {
//		set_forwarding_to(other_nest->forwarding_to());
//	} else {
//		set_forwarding(other_nest->forwarding());
//	};
//};

const int bird_flying_priority = -3; // flying above everything

Bird::Bird(city_coordinate center_x, city_coordinate center_y,
			  long priority,
//			  Background *floor,
			  Nest *nest,
			  city_coordinate initial_width,
			  city_coordinate initial_height,
           Sprite *t_shirt) :
//			  BuiltIn primitive,
//			  Sprite *controlled_sprite) :
  Sprite(BIRD_SPRITE,
		 center_x,center_y,BIRD,
		 BIRD_INERT,ground_scale,//FALSE,
		 initial_width,initial_height),
  nest(nest),
  delivery_status(NO_DELIVERY),
//  action_status(NULL),
//  dont_add_t_shirt(FALSE),
  t_shirt(t_shirt),
//  t_shirt_added(t_shirt != NULL),
  no_home_to_return_to(FALSE),
  destroy_item_flag(FALSE),
//  primitive(primitive),
//  controlled_sprite(controlled_sprite), // for user control of sprites
  top_of_stack_on_nest(NULL), // to keep track of stack when adding item to full nest
  // return_x and return_y set before use
  return_cubby(NULL),
  return_floor(NULL),
  return_path(NULL),
  waiting_for(NULL),
  saved_item(NULL), // wasn't initialized prior to 051202
//  saved_by(NULL),
//  saved_status(NULL),
  size_percent(100),
  size_is_100_percent(TRUE),
  global_picture_will_animate_me(FALSE),
  favorite_width(-1),  // don't have one
  favorite_height(-1),
  item_to_deliver(NULL),
  return_x(min_long), // not initialized prior to 051202
  return_y(min_long),
  saved_duration(0) {
//	set_priority_function_of_lly(FALSE);
	current_priority = priority;
   set_show_all(FALSE); // new on 260802 - kind of obvious so not checked but helps generate better XML
	if (t_shirt != NULL) {
//      t_shirt->set_size_is_true(TRUE); // size is image size not appearance size
		add_t_shirt(t_shirt);
	};
};

void Bird::reset_nest() { // should be used carefully... could check first that is indeed a proxy
	nest->destroy();
	nest = NULL;
};

//void Bird::prepare_for_deletion() {
//   if (preparing_for_deletion) return; // already done
////	preparing_for_deletion = TRUE; // new on 060204
////	if (nest != NULL && !tt_shutting_down) {
////	tt_error_file() << "deleting ";
////	describe(tt_error_file(),DEFINITE_ARTICLE);
////	tt_error_file() << endl << " and its nest: ";
////	nest->describe(tt_error_file(),DEFINITE_ARTICLE);
//// tt_error_file() << endl;
////	};
//	Sprite::prepare_for_deletion();
//};

void Bird::release_all_references() { // new on 130504 -- copied from :prepare_for_deletion
	if (releasing_all_references) { // new on 230305
		return;
	};
	if (nest != NULL) {
#if TT_DEBUG_ON
      if (tt_debug_mode == 13579) {
         print(tt_error_file());
         tt_error_file() << " bird being deleted whose nest is ";
         nest->print(tt_error_file());
         tt_error_file() << endl;
      };
#endif
		// why doesn't the following just unconditionally do nest->destroy() - asked 190103
		if (about_to_quit()) { // new on 270400 -- was tt_shutting_down prior to 120201
#if TT_DIRECT_PLAY
			nest->destroy(); // restored on 190103 prepare_for_deletion(this); // had destroy() here from 280201 to 010301 but that caused errors while shutting down
#endif
		} else {
			nest->decrement_bird_count();
			nest = NULL;
		};
	};
	if (return_path != NULL) {
		delete return_path;
		return_path = NULL;
	};
	t_shirt = NULL; // is also a follower so will be released as well -- new on 130504
	saved_item = NULL; // new on 130504
	waiting_for = NULL; // new on 130504 
   set_return_floor(NULL);
	set_item_to_deliver(NULL); // drops ref count
	set_top_of_stack_on_nest(NULL); // drops ref count - new on 120203
	Sprite::release_all_references();
};

void Bird::set_return_floor(Background *new_floor) { // abstracted out on 191102
	if (return_floor == new_floor) {
		return; // new on 200404 for robustness
	};
   if (return_floor != NULL) {
		if (!tt_resetting_or_destroying_city && return_floor->decrement_ref_count()) { 
			// not needed anymore - 220103 since decrement_ref_count does it now -- was && return_floor->deletion_pending()) {
         // tt_resetting_or_destroying_city was tt_shutting_down prior to 250903 but can cause memory errors while resetting during time travel
			if (return_floor->deletion_pending()) {
				delete return_floor; // branch new on 090804 since is already prepared for deletion
			} else {
				return_floor->destroy();
			};
      };
   };
   return_floor = new_floor;
	if (return_floor != NULL){
		return_floor->increment_ref_count();
//		return_floor->increment_ref_count(); // experiment on 130203 -- removed 090804
	};
};

Sprite *Bird::copy(boolean also_copy_offsets) {
#if TT_DEBUG_ON
	if (tt_debug_target == debug_counter) {
		log("Debug target bird is being copied.");
	};
#endif
	Nest *nest_copy;
//	Sprite *top_container = ultimate_container();
	// commented out the following on 241199 since birds create nests during training inappropriately
	// also if part of top level copy then was OK
//	if (tt_copying_entire_thought_bubble) { // new on 211199 so nest won't know about this copy
//		nest_copy = NULL;
//	} else 
   if (nest != NULL) {
      // new on 181002 to see if any nest copies are part_of_top_level_copy
      Nest *a_nest = NULL;
      if (nest->part_of_top_level_copy()) {
         a_nest = nest;
      } else {
         a_nest = nest->first_copy_that_is_part_of_top_level_copy();
         if (a_nest != NULL) {
            nest->transfer_copies_to(a_nest);
            nest = a_nest; // bird will use one of the copies being saved with instead
         };
      };
      if (a_nest == NULL) {
         nest_copy = nest; // not related
      } else {      
         // nest->ultimate_container() == top_container && -- commented out on 221200 since part_of test is good enough
		   // not just a part with only bird being copied
		   // nest is being copied as well (but only once)
		   int bird_count = nest->current_bird_count();
		   // pretend it doesn't know any birds so no multcasting
		   // difference between copying a nest by itself and in a structure
		   // with its bird(s)
		   nest->set_bird_count(0); 
		   nest_copy = (Nest *) nest->copy();
#if TT_DEBUG_ON
         if (tt_debug_mode == 13579) {
            nest->print(tt_error_file());
            tt_error_file() << " setting bird count due to bird copy. Bird count is " << bird_count << " bird is ";
            print(tt_error_file());
            tt_error_file() << endl;
         };
#endif
		   nest->set_bird_count(bird_count);
		   Sprites *remaining = nest->pointer_to_nest_copies(); 
			// was all_nests() prior to 041201 but that includes those forwarded to now 
		   while (remaining != NULL) {
			   Nest *a_nest = (Nest *) (remaining->first());
			   if (!a_nest->ignore_me(FALSE) && is_a_current_copy(a_nest)) { // ignore_me() test added on 041201
				   a_nest->set_original_nest(nest_copy);
				   nest->remove_copy(a_nest); // the nest of this guy should be restored
				   if (a_nest != nest_copy) nest_copy->add_copy(a_nest);
			   };
			   remaining = remaining->rest();
		   };
      };
		nest->bird_just_copied(); // new on 180804
	} else {
		nest_copy = nest;
	};
	Bird *copy = (Bird *) new Bird(llx,lly, //center_x,center_y,
											 current_priority-1,
											 nest_copy,
											 width,height); // primitive,controlled_sprite);
   just_copied(this,copy); 
	// added 230299 so nests copied with bird get right connection (used in nest::copy(boolean also_copy_offsets))
	copy->set_old_x_offset(old_x_offset);
	copy->set_old_y_offset(old_y_offset);
//	if (saved_width != 0) {
//		copy->set_saved_size(saved_width,saved_height);
//	};
	if (nest_copy == NULL) {
//		copy->build_nest();
	} else {
#if TT_DEBUG_ON
      if (tt_debug_mode == 13579) {
         nest_copy->print(tt_error_file());
         tt_error_file() << " incrementing bird count due to bird copy who is ";
         copy->print(tt_error_file());
         tt_error_file() << endl;
      };
#endif
		nest_copy->increment_bird_count();
	};
   if (t_shirt != NULL) {
      copy->add_t_shirt(t_shirt->copy(TRUE));
   };
   finish_making_copy(copy,also_copy_offsets);
	//if (comment != NULL) {
	//	copy->set_comment(comment->copy());
	//};
 //  copy->set_priority_fixed(priority_fixed_flag);
	return(copy);
};

void Bird::java_before(java_stream &out) {
	// prior to 061100 this only checked the nest and not its copies as well
	if (nest == NULL || contained_in(THOUGHT_BUBBLE)) return; // same condition??
	Sprites *nests = nest->all_nests(FALSE,TRUE);
	Sprites *remaining = nests;
	Sprite *container = ultimate_container();
	while (remaining != NULL) {
		Nest *nest = (Nest *) remaining->first();
		if (container == nest->ultimate_container()) {
			nest->java_before(out);
		};
		remaining = remaining->rest();
	};
	just_destroy_list(nests);
};

void Bird::java(java_stream &out) {
   java_new(out,S(IDS_JAVA_TT_BIRD));
};

void Bird::java_after(java_stream &out, const_java_string path) {
   if (contained_in_thought_bubbles_cubby()) return; // prior to 190205 was contained_in(THOUGHT_BUBBLE)) return; 
	// but that didn't work for restored cubbies -- i.e. those created for a robot team (not truck) top level
	boolean any_nests_set = FALSE;
	if (nest != NULL) { // conditional new on 201100
		// following also updated for nest copies on 061100
		Sprites *nests = nest->all_nests(FALSE,TRUE);
		Sprites *remaining = nests;
		Sprite *container = ultimate_container();
		while (remaining != NULL) {
			Nest *nest_copy = (Nest *) remaining->first();
			if (container == nest_copy->ultimate_container()) {
				java_send(out,path,S(IDS_JAVA_SET_NEST),nest_copy);
				any_nests_set = TRUE;
			};
			remaining = remaining->rest();
		};
		just_destroy_list(nests);
	};
	if (any_nests_set) return;
	if (tt_programmer->pointer_to_tool_in_hand()->kind_of() == PICTURE) return; // new on 160101 since if picture don't worry about birds without nests
	string answer = java_array(S(IDS_JAVA_ANSWER),S(IDS_JAVA_ANSWER_COUNT));
	string label = NULL;
	if (t_shirt != NULL) { // new on 150201 to use bird's label primarily
		long length;
		t_shirt->current_text(label,length);
		if (length > 0) {
			label = (string) java_quote(label);
		};
	};
	if (label == NULL && leader != NULL && leader->kind_of() == CUBBY) {
		Cubby *cubby = (Cubby *) leader;
		label = cubby->label(cubby->my_index(this)); // removed (cubby_index) coerecion on 070203
		if (label != NULL) {
			label = (string) java_quote(label);
		};
	};
	java_assign_new(out,answer,S(IDS_JAVA_TT_NEST),label);
	if (label != NULL) delete [] label;
	java_comment(out,S(IDS_JAVA_NEST_FOR_BIRD_WITHOUT_ONE));
// out << "  answer[answer_count] = new TTNest(); // nest for bird without one" << endl;
	java_send_array_arg(out,path,S(IDS_JAVA_SET_NEST),S(IDS_JAVA_ANSWER),S(IDS_JAVA_ANSWER_COUNT_PLUS_PLUS));
// out << "  " << path << ".SetNest(answer[answer_count++]);" << endl;
	delete [] answer;
};

boolean Bird::dump(output_stream &stream, boolean just_prepare, boolean , boolean ) {
	update_pointer_to_nest();
   if (just_prepare) { 
		// need to do more? -- deliver and destroy??
      finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
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
	stream.put((unsigned char) BIRD);
	dump_if_not_null(stream,comment);
//	stream.put('\0'); // not primitive marker
	// added 3/4-95:
//	if (nest != NULL && being_dumped_together(this,nest)) {
		// nest exists and we are both inside the same container
//		stream.put((unsigned char) nest_dump_index(nest));
//	} else {
//		stream.put(NO_NEST_INDEX); // bird without nest
//	};
	if (nest != NULL && 
		 !nest->deleted() && // new on 030500
//#if !TT_DIRECT_PLAY
		 (!tt_save_nests_as_local || being_dumped_together(this,nest)) // if networked then always "dumped" together
		 // changed on 180201 so can be compiled with DirectPlay but still have it turned off
//#endif
		 ) {
#if TT_DIRECT_PLAY
		if (!being_dumped_together(this,nest) && !tt_dumping_to_test_equality_or_describe && !tt_save_nests_as_local && 
#if TT_ALPHA2
			!contained_in_robots_thought_bubble()
#else
			!contained_in(THOUGHT_BUBBLE)
#endif					
			) {
			// not contained in a thought bubble added on 050201 - note that this still allows networked birds in thought bubbles but they won't be created while in a thought bubble
			// second condition added on 150500
			// restored this on 020500 
			nest->set_dump_as_proxy(TRUE); // new on 050100
			nest->become_networked(); // new on 030500
		};
#endif
		nest->dump(stream);
#if TT_DIRECT_PLAY
		if (!tt_save_nests_as_local) nest->set_dump_as_proxy(FALSE); // since next time it might be dumped along with its bird
#endif
	} else {
		stream.put(NOTHING_MARKER);
   };
	dump_if_not_null(stream,t_shirt);
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
// new on 120802

//xml_element *Bird::xml_create_element(xml_document *document) {
//   update_pointer_to_nest(); // this line moved to :xml on 120703
//   return(create_xml_element(xml_tag(),document));
//};

SavedNestGUID *saved_nest_guids = NULL; // new on 181002 so nest copies are saved with a new GUID and then it is restored
int saved_nest_guids_count = 0;
int max_saved_nest_guids_count = 0;

void save_nests_old_guid(Nest *nest, boolean first) {
   const int minimum_size = 32; 
   if (saved_nest_guids_count >= max_saved_nest_guids_count) {
      int new_max_saved_nest_guids_count = max(minimum_size,max_saved_nest_guids_count*2);
      SavedNestGUID *new_saved_nest_guids = new SavedNestGUID[new_max_saved_nest_guids_count];
      for (int i = 0; i < saved_nest_guids_count; i++) { // copy over old ones
         new_saved_nest_guids[i].nest = saved_nest_guids[i].nest;
         new_saved_nest_guids[i].nest_guid = saved_nest_guids[i].nest_guid;
         new_saved_nest_guids[i].first = saved_nest_guids[i].first;
      };
      delete [] saved_nest_guids;
      saved_nest_guids = new_saved_nest_guids;
      max_saved_nest_guids_count = new_max_saved_nest_guids_count;
   };
	for (int i = 0; i < saved_nest_guids_count; i++) {
		if (saved_nest_guids[i].nest == nest) {
#if TT_DEBUG_ON
			if (saved_nest_guids[i].first != first) {
				debug_this(); // not clear if this can happen and if so what to do
			};
#endif
			return; // already done -- new on 100305
		};
	};
   saved_nest_guids[saved_nest_guids_count].nest = nest;
   saved_nest_guids[saved_nest_guids_count].nest_guid = nest->pointer_to_nest_guid();
   saved_nest_guids[saved_nest_guids_count].first = first;
   saved_nest_guids_count++;
};

void release_saved_nest_guids() {
   if (saved_nest_guids != NULL) {
      delete [] saved_nest_guids;
      saved_nest_guids = NULL;
      max_saved_nest_guids_count = 0;
   };
};

void restore_saved_nest_guids() {
   for (int i = 0; i < saved_nest_guids_count; i++) {
      if (saved_nest_guids[i].first) {
         delete saved_nest_guids[i].nest->pointer_to_nest_guid();
      };
      saved_nest_guids[i].nest->set_nest_guid(saved_nest_guids[i].nest_guid);
   };
   saved_nest_guids_count = 0;
};

boolean guid_temporarily_reset(Nest *nest) { // new on 130104
	for (int i = 0; i < saved_nest_guids_count; i++) {
      if (saved_nest_guids[i].nest == nest) return(TRUE);
   };
	return(FALSE);
};

void Bird::prepare_nest_copy_for_xml(Nest *a_nest, NestPointer &one_nest_dumped_together_with) {
   if (one_nest_dumped_together_with != NULL) {
		if (a_nest->pointer_to_nest_guid() != one_nest_dumped_together_with->pointer_to_nest_guid()) { 
			// condition new on 100305 (e.g. issue 660)
			save_nests_old_guid(a_nest,FALSE);
			a_nest->set_nest_guid(one_nest_dumped_together_with->pointer_to_nest_guid());
#if TT_DEBUG_ON
		} else {
			debug_this();
#endif
		};
   } else {
      save_nests_old_guid(a_nest,TRUE);
      one_nest_dumped_together_with = a_nest;
      a_nest->set_nest_guid(NULL); // so a new one is generated below
      a_nest->assign_guid();
   };
};

void Bird::xml(xml_element *bird_element, xml_document *document) {
	update_pointer_to_nest();
   if (t_shirt != NULL) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 -- needed? can it really be shared??
		//	t_shirt->check_for_independence();
		//};
		xml_append_child(create_xml_element_shared(L"Label",document,t_shirt),bird_element); // can it really be shared??
   };
   if (nest != NULL && !nest->deleted()) {
      Nest *one_nest_dumped_together_with = NULL;
		boolean save_nest_as_local = (tt_save_nests_as_local || blank || no_home_to_return_to); 
		// no_home_to_return_to new on 261003 since if is a temporary copy of a bird (during delivery) 
		// then the other nests just confuse things - 
		// probably true as well for foriegn bird (give_bird)											 
		// blank is a better test than contained_in(THOUGHT_BUBBLE) - rewritten on 101202
//      if (!tt_save_nests_as_local && !tt_dumping_to_test_equality_or_describe && !contained_in(THOUGHT_BUBBLE) 
		// && !inside_toolbox_vacuum()) { 
		if (!save_nest_as_local && !tt_dumping_to_test_equality_or_describe) { // && !inside_toolbox_vacuum()) { 
         // re-written on 171002 so doesn't do extra work when tt_save_nests_as_local
         // and so that we check all nest copies not just the original one
         // added inside_toolbox_vacuum() condition since it comes from being_dumped_together -- but is it a good idea???????
			// commented out the condition on 280403 since causes long-distance birds when pair is vacuumed and then city is saved
			Sprites *remaining = nest->all_nests(FALSE,(tt_dumping_background != tt_city)); 
			// new on 171002 so if any of the nest copies is saved together they become local
			// added second arg on 130505 so if saving a city then include nests in limbo (e.g. in Dusty)
         if (remaining == NULL) {
				save_nest_as_local = TRUE; // new on 280403 since if nest is say in a vacuum then don't make the bird long-distance
				if (pointer_to_cycle_stopped_callback() == NEST_IS_MADE_CALLBACK && nest != NULL) { 
					// new on 030504 to save the nest when a bird is creating a nest
					prepare_nest_copy_for_xml(nest,one_nest_dumped_together_with);
				};
			} else if (tt_dumping_background == tt_city) {
            while (remaining != NULL) {
					Nest *a_nest = (Nest *) remaining->first();
					if (!a_nest->proxy()) { 
						// conditional new on 230103 since otherwise this gives the long-distance bird's nest a new GUID
						prepare_nest_copy_for_xml(a_nest,one_nest_dumped_together_with);
					};
               remaining = remaining->rest();
            };              
         } else if (tt_dumping_background == NULL) {
            Sprite *my_ultimate_container = ultimate_container();
            while (remaining != NULL) {
               Nest *a_nest = (Nest *) remaining->first();
//               if (a_nest->ultimate_container() == my_ultimate_container) { 
					// noticed that on 211003 that nest_guid is assigned incorrectly here
//						one_nest_dumped_together_with = a_nest;
//					} else {
					if (a_nest->ultimate_container() == my_ultimate_container) {
                  prepare_nest_copy_for_xml(a_nest,one_nest_dumped_together_with);
//#if TT_DEBUG_ON
//					} else {
//						debug_this(); // just for now 150104
//#endif
               };
               remaining = remaining->rest();
            };
         } else {
            if (member_of_background(tt_dumping_background)) { // could this be other than TRUE??
               while (remaining != NULL) {
                  Nest *a_nest = (Nest *) remaining->first();
                  if (a_nest->member_of_background(tt_dumping_background)) {
                     prepare_nest_copy_for_xml(a_nest,one_nest_dumped_together_with);
                  };
                  remaining = remaining->rest();
               };
            };
         };
      } else if (being_dumped_together(this,nest)) {
         one_nest_dumped_together_with = nest;
      };
		nest->set_dump_as_proxy(FALSE); 
		// new on 170103 in case had been set by a previous save where this bird was separated from her nest
		// reset below as necessary
      if (!save_nest_as_local || one_nest_dumped_together_with != NULL) { // if networked then always "dumped" together 
			// first disjunct was tt_save_nests_as_local prior to 101202 
			// so don't save as long-distance birds that are in the thought bubble
         // following can't be done locally like this anymore since can be many nests that are saved as proxies 
         // so needs to be the scope of the entire save (city, object, ...)
#if TT_DIRECT_PLAY
//       boolean temporarily_made_a_proxy = FALSE; // new on 171002
			if (one_nest_dumped_together_with == NULL && !tt_dumping_to_test_equality_or_describe && !tt_save_nests_as_local &&
				!contained_in_robots_thought_bubble()) { //	!contained_in(THOUGHT_BUBBLE)	done when TT_ALPHA2 was current
				// note that this still allows networked birds in thought bubbles but they won't be created while in a thought bubble
				nest->set_dump_as_proxy(TRUE); // new on 050100
				nest->become_networked(); // new on 030500
//          temporarily_made_a_proxy = TRUE;
			};
#endif
         if (one_nest_dumped_together_with == NULL) {
            one_nest_dumped_together_with = nest;
         };
			xml_element *nest_element = create_xml_element(L"MyNest",document); 
			// don't need to check for independence for time travel include segments 
			// since being dumped together is a good enough check
			one_nest_dumped_together_with->xml_create_and_append_element(nest_element,document)->Release(); 
			// used to be nest but any copy that is also being saved will do will do 
			xml_append_child(nest_element,bird_element);
			// commented out the following since with postponed nests this restores it too soon
//#if TT_DIRECT_PLAY
//         if (temporarily_made_a_proxy) { // was !tt_save_nests_as_local) { 
//            nest->set_dump_as_proxy(FALSE); // since next time it might not be dumped along with its bird
//         };
//#endif
		};
   };
   Sprite::xml(bird_element,document);
};

xml_element *Bird::xml_set_special_attributes(xml_element *element, xml_document *document) {
	// new on 051202 -- went through all the state variables
	xml_element *special_element = Sprite::xml_set_special_attributes(element,document);
	if (delivery_status != NO_DELIVERY) { 
		xml_set_attribute(special_element,L"DeliveryStatusCode",(int) delivery_status);
	};
	if (no_home_to_return_to) { 
		xml_set_attribute(special_element,L"NoHomeToReturnTo",1);
	};
	if (destroy_item_flag) { 
		xml_set_attribute(special_element,L"DestroyItem",1);
	};
	if (global_picture_will_animate_me) { 
		xml_set_attribute(special_element,L"GlobalPictureWillAnimatedMe",1);
	};
	if (favorite_width > 0) {
		xml_set_attribute(special_element,L"FavoriteWidth",favorite_width);
	};
	if (favorite_height > 0) {
		xml_set_attribute(special_element,L"FavoriteHeight",favorite_height);
	};
	if (return_x != min_long) {
		xml_set_attribute(special_element,L"ReturnX",return_x);
	};
	if (return_y != min_long) {
		xml_set_attribute(special_element,L"ReturnY",return_y);
	};
	if (saved_duration != 0) {
		xml_set_attribute(special_element,L"SavedDuration",saved_duration);
	};
	if (size_percent != 100) {
		xml_set_attribute(special_element,L"SizePercent",size_percent);
	};
	if (size_is_100_percent) {
		xml_set_attribute(special_element,L"SizeIs100Percent",1);
	};
	if (return_floor != NULL) {
		xml_append_child(return_floor->xml_path(document),special_element);
	};
	return(special_element);
};

//boolean handle_return_floor(Entity *bird, Tag tag, xml_node *node, void *extra_info) {
//	switch (tag) {
//		case HOUSE_LOCATION_TAG: {
//			House *house = house_at_location(node);
//			if (house == NULL) return(FALSE);
//			Floor *floor = house->pointer_to_floor();
//			if (floor == NULL) return(FALSE);
//			((Bird *) bird)->set_return_floor(floor);
//			return(TRUE);
//									  };
//			// birds can't start from the ground currently
//		/*case RETURN_GROUND_TAG:
//			if (tt_programmer->kind_of() == PROGRAMMER_AT_FLOOR) {
//				Background *background = tt_programmer->pointer_to_background();
//				if (background != NULL && background->on_the_ground()) {
//					((Bird *) bird)->set_return_floor(background);
//					return(TRUE);
//				};
//			};
//			return(FALSE);*/
//	};
//	return(FALSE);
//};

boolean Bird::xml_get_special_attributes(xml_node *node) {
	delivery_status = (DeliveryStatus) xml_get_attribute_int(node,L"DeliveryStatusCode",NO_DELIVERY); 
	// prior to 210404 was (int) delivery_status
	no_home_to_return_to = xml_get_attribute_int(node,L"NoHomeToReturnTo",0); // prior to 210404 was no_home_to_return_to
	destroy_item_flag = xml_get_attribute_int(node,L"DestroyItem",0); // prior to 210404 destroy_item_flag
	global_picture_will_animate_me = xml_get_attribute_int(node,L"GlobalPictureWillAnimatedMe",0); 
	// prior to 210404 global_picture_will_animate_me
	favorite_width = xml_get_attribute_int(node,L"FavoriteWidth",0); // prior to 210404 favorite_width
	favorite_height = xml_get_attribute_int(node,L"FavoriteHeight",0); // prior to 210404 favorite_height
	return_x = xml_get_attribute_int(node,L"ReturnX",min_long); // prior to 210404 return_x
	return_y = xml_get_attribute_int(node,L"ReturnY",min_long); // prior to 210404 return_y
	saved_duration = xml_get_attribute_int(node,L"SavedDuration",0); // prior to 210404 saved_duration
	size_percent = xml_get_attribute_int(node,L"SizePercent",100); // prior to 210404 size_percent
	size_is_100_percent = xml_get_attribute_int(node,L"SizeIs100Percent",0); // prior to 210404 size_is_100_percent
//	xml_process_children(node,::handle_return_floor,this,NULL);
	return(Sprite::xml_get_special_attributes(node));
};

void Bird::add_special_xml(xml_element *element, xml_document *document) {
	if (item_to_deliver != NULL) {
//		xml_element *item_to_deliver_node = item_to_deliver->add_xml(element,L"ItemToDeliver",document);
		// replaced above with following on 200804 to move xml_append_child to end
		xml_element *item_to_deliver_node = create_xml_element(L"ItemToDeliver",document);
		item_to_deliver->xml_create_and_append_element(item_to_deliver_node,document,TRUE)->Release(); // prior to 240804 missing item_to_deliver->
		if (item_to_deliver->pointer_to_leader() == NULL) {
			// new on 060504 since sometimes the item is temporarily removed -- e.g. in :set_down_message
			xml_set_attribute(item_to_deliver_node,L"TemporarilyNotFollowingBird",1);
		};
		xml_append_child(item_to_deliver_node,element);
	};
	if (top_of_stack_on_nest != NULL) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	top_of_stack_on_nest->check_for_independence();
		//};
		top_of_stack_on_nest->add_xml(element,L"TopOfStackOnNest",document);
	};
	if (return_cubby != NULL) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	return_cubby->check_for_independence();
		//};
		return_cubby->add_xml(element,L"ReturnCubby",document); // should have been ReturnBox but too late...
	};
	// restored on 150603 since cities saved while bird is flying broke without a return_path
	if (return_path != NULL) { // commented out on 201202 since return_floor->xml_path(document) above works better
		const int path_string_size = 1024;
      wide_character path_string[path_string_size];
      if (return_path->xml_path_string(path_string,0,path_string_size-30)) {
         // -30 since each step in the path is always less than 30 characters long
		   xml_append_child(create_xml_element(L"ReturnPath",document,path_string),element);
      }; // else try again with larger path?? or warn??
	};
	if (saved_item != NULL) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	saved_item->check_for_independence();
		//};
		saved_item->add_xml(element,L"SavedIem",document);
	};
	if (waiting_for != NULL) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	waiting_for->check_for_independence();
		//};
		waiting_for->add_xml(element,L"WaitingFor",document);
	};	
	Sprite::add_special_xml(element,document);
};

//void restore_nests_temporarily_dumping_as_proxies() { // new on 171002
//   if (tt_nests_temporarily_dumping_as_proxies == NULL) return;
//   Sprites *remaining = tt_nests_temporarily_dumping_as_proxies;
//   while (remaining != NULL) {
//      Nest *nest = (Nest *) remaining->first();
//      nest->set_dump_as_proxy(FALSE); // since next time it might not be dumped along with its bird
//      remaining->set_first(NULL);
//      remaining = remaining->rest();
//   };
//   delete tt_nests_temporarily_dumping_as_proxies;
//   tt_nests_temporarily_dumping_as_proxies = NULL;
//};

boolean Bird::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
		case MY_NEST_TAG: {
//         return(xml_process_children(node,bird_handle_xml_my_nest_tag,this,NULL));
			Nest *my_nest = (Nest *) xml_get_sprite(node,nest,NEST);
//#if TT_BUG_FIXES
			// commented out on 091203 since can cause excessive sharing resulting in an xml_load of a sprite already with a leader
//			if (my_nest != NULL) {
//				my_nest = my_nest->ultimate_original_nest(); // new on 011203 to keep the chains short and simple
//			};
//#endif
			set_nest(my_nest); // simplified on 070103
			if (my_nest == NULL) {
				// added this branch on 011105 since Ian had a crash in puzzle game where a guid was missing
				// probably caused by a bird that was flying to nest on the rocket wall
				tt_error_file() << "A bird was loaded but ToonTalk couldn't find its nest." << endl;
			} else if (my_nest->current_bird_count() == 0) { // new on 040205 to fix badly saved things
				Nest *original_nest = my_nest->ultimate_original_nest();
				original_nest->set_bird_count(max_long); // just to be sure settting count to infinity
			};
         return(TRUE);
								};
      case LABEL_TAG: {
         Sprite *sprite = xml_get_sprite(node);
         if (sprite == NULL) return(TRUE);
         add_t_shirt(sprite);
         return(TRUE);
                      };
		case ITEM_TO_DELIVER_TAG:
			set_item_to_deliver(xml_get_sprite(node,item_to_deliver)); // changed on 121202 to use set_item_to_deliver to maintain ref counts correctly
			if (!xml_get_attribute_int(node,L"TemporarilyNotFollowingBird",0)) { // condition new on 060504
				add_follower(item_to_deliver,FALSE,INSERT_AT_END,TRUE);	
				// extra args new on 261003 to get the geometry right when bird saved holding something										
			};
//			if (showing_on_a_screen()) tt_screen->add(item_to_deliver); -- TOO EARLY !!! 
			return(TRUE);
		case TOP_OF_STACK_ON_NEST_TAG:
			set_top_of_stack_on_nest(xml_get_sprite(node,top_of_stack_on_nest)); // was just an assignment prior to 120203
			if (top_of_stack_on_nest != NULL) {
				if (top_of_stack_on_nest->pointer_to_leader() != NULL) { 
					// new on 040205 since nest may have already added it since it is "on" the nest (but temporarlily removed)
					top_of_stack_on_nest->pointer_to_leader()->remove_follower(top_of_stack_on_nest);
				};
				add_follower(top_of_stack_on_nest,FALSE,INSERT_AT_END,TRUE); 
				// extra args new on 261003 to get the geometry right when bird saved holding something										
				return(TRUE);
			};
			return(FALSE);
		case RETURN_BOX_TAG:
			set_return_cubby((Cubby *) xml_get_sprite(node,return_cubby,CUBBY)); // rewritten 091202 to use set_return_cubby for reference counting
			return(TRUE);
		case RETURN_PATH_TAG: {
			SpriteType path_type;
			return_path = xml_decode_event_path(node,path_type);
			return(TRUE);
									 };
		case HOUSE_LOCATION_TAG: {
			House *house = house_at_location(node);
			if (house == NULL) return(FALSE);
			Floor *floor = house->pointer_to_floor();
			if (floor == NULL) return(FALSE);
			set_return_floor(floor);
			return(TRUE);
									  };
		case SAVED_ITEM_TAG:
			saved_item = xml_get_sprite(node,saved_item);
			return(TRUE);
		case WAITING_FOR_TAG:
			waiting_for = (Cubby *) xml_get_sprite(node,waiting_for,CUBBY); 
			return(TRUE);
      default:
         return(Sprite::handle_xml_tag(tag,node));
   };
};

#endif

boolean Bird::equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, SpritesPointer &expected_corresponding_nests, 
							  EqualReason reason) { // new on 060601
	if (other == this) return(TRUE);
	if (other->kind_of() != kind_of()) return(FALSE);
	if (is_blank() != other->is_blank()) return(FALSE);
	Bird *other_bird = (Bird *) other;
	Nest *nest = pointer_to_nest();
	if (nest != NULL) {
		Nest *other_nest = other_bird->pointer_to_nest();
		if (other_nest != NULL) {
			if (nest->equal_to(other_nest,pending,corresponding_nests,expected_corresponding_nests,BIRDS_CHECKING_EQUAL_NESTS)) {
				return(TRUE);
			} else { // nests may be in corresponding location so
				if ((unsigned long) nest < (unsigned long) other_nest) { // canonicalize the order in both list
					if (expected_corresponding_nests == NULL || expected_corresponding_nests->successive_member_cons(nest,other_nest) == NULL) { 
						// not already there
						expected_corresponding_nests = new Sprites(nest,new Sprites(other_nest,expected_corresponding_nests));
#if TT_DEBUG_ON
						if (tt_debug_mode == 60601) {
							tt_error_file() << "Expecting to find the following in corresponding positions: ";
							nest->print(tt_error_file());
							tt_error_file() << " and ";
							other_nest->print(tt_error_file());
							tt_error_file() << endl;
						};
						if (nest->debug_counter == tt_debug_target) {
							log("Expecting debug target nest to have corresponding nest.");
						};
#endif
					};
				} else if (expected_corresponding_nests == NULL || expected_corresponding_nests->successive_member_cons(other_nest,nest) == NULL) { 
					// not already there
					expected_corresponding_nests = new Sprites(other_nest,new Sprites(nest,expected_corresponding_nests));
#if TT_DEBUG_ON
					if (tt_debug_mode == 60601) {
						tt_error_file() << "Expecting to find the following in corresponding positions: ";
						other_nest->print(tt_error_file());
						tt_error_file() << " and ";
						nest->print(tt_error_file());
						tt_error_file() << endl;
					};
					if (other_nest->debug_counter == tt_debug_target) {
						log("Expecting debug target nest to have corresponding nest.");
					};
#endif
				};
			};
		} else return(FALSE);
	} else if (other_bird->pointer_to_nest() != NULL) { // was == prior to 031002 so broke F11 undo (e.g. Pong in Playground)
		return(FALSE);
	};
	return(TRUE); // either have same nest or both are missing a nest - or will fail later if nests aren't corresponding
};

boolean Bird::to_windows(Pointer *data, short int &data_offset,
                         ascii_string types, short int &types_offset,
                         BirdPointer *birds, short int &bird_count) {
   types[types_offset] = 'B'; // token for bird
   types_offset++;
//   types[types_offset] = (char) ('0'+bird_count);
//   types_offset++;
   birds[bird_count] = this;
   bird_count++;
   data[data_offset] = this;
   data_offset++;
   return(TRUE);
};

void Bird::set_priority(long new_priority) {
	Sprite::set_priority(new_priority);
	if (followers != NULL) {
		followers->first()->set_priority(new_priority+1);
	};
};

const city_coordinate bird_above_size = tt_default_block_width/10; // flying close

boolean Bird::inert() {
	return(current_parameter() == BIRD_INERT || current_parameter() == BIRD_MORPH_TO_INERT);
};

void Bird::display(SelectionFeedback selection_feedback, boolean followers_too, TTRegion *region) {
#if TT_DEBUG_ON
   if (tt_debug_target == debug_counter) {
      log("Displaying bird that is debug target.");
   };
#endif
//   if (inert() && rescale_if_camera_above(selection_feedback,followers_too,region)) return; // made obsolete on 230804
	if (pointer_to_top_of_stack() != NULL) { // new on 260500
		pointer_to_top_of_stack()->display(selection_feedback,followers_too,region);
		return;
	};
	if (width <= 1 || height <= 1) return; // new on 060705 -- even if carrying a big item
	boolean view_from_above = (leader == NULL && tt_screen->camera_status() == CAMERA_ABOVE); // leader test added on 230804
	if (region == NULL && delivery_status != NO_DELIVERY) {
		// second condition added on 221105 since if sitting on the ground shouldn't change size
		if (view_from_above) {
			city_scale scale = tt_screen->current_scale();
			city_coordinate bird_size = (size_percent*(bird_above_size*ground_scale)/scale)/100;
			set_size(bird_size,bird_size); //was scale_to_fit
			if (bird_size < scale) return; // too small to worry about
		} else if (!size_is_100_percent) {
			city_coordinate bird_width = (size_percent*bird_above_size)/100;
			city_coordinate bird_height = bird_width; // good enuf
			if (floor->in_the_room_with_programmer()) { // updated 020205
				tt_programmer->translate_floor_to_room_dimensions(bird_width,bird_height);
			};
			set_size(bird_width,bird_height);
	//	} else if (floor->in_the_room()) {
	//      city_coordinate bird_width = bird_above_size;
	//      city_coordinate bird_height = bird_above_size;
	//      tt_programmer->translate_floor_to_room_dimensions(bird_width,bird_height);
	//      set_size(bird_width,bird_height);
		};
	};
   if (held() && cycle_stopped()) {
      set_parameter(BIRD_BREATHING); // looks nicer
   };
   if (t_shirt != NULL) {
      boolean t_shirt_visible = (current_parameter() == BIRD_BREATHING ||
                                 current_parameter() == BIRD_INERT ||
                                 current_parameter() == BIRD_MORPH_TO_INERT ||
                                 current_parameter() == BIRD_MORPH_TO_LIFE);
//      if (!t_shirt->visible() && t_shirt_visible) { // becoming visible
//         t_shirt->finish_instantly();
//         fix_t_shirt();
//      };
      t_shirt->set_visible(t_shirt_visible);
      if (t_shirt_visible && !cycle_stopped()) { // why was && leader == NULL here??
         fix_t_shirt();
      };
   };
	Sprite::display(selection_feedback,blank?FALSE:followers_too,region); // blank stuff new on 101199
};

void Bird::t_shirt_region(TTRegion &region) {
   true_region(region);
   city_coordinate region_width = region.width();
   city_coordinate region_height = region.height();
   region.min_x += (31*region_width)/100;
   region.max_x -= (30*region_width)/100;
   region.min_y += (23*region_height)/100;
   region.max_y -= (52*region_height)/100;
};

void Bird::t_shirt_region_drop_region(TTRegion &region) {
   true_region(region);
   city_coordinate region_width = region.width();
   city_coordinate region_height = region.height();
   region.min_x += (25*region_width)/100;
   region.max_x -= (25*region_width)/100;
   region.min_y += (20*region_height)/100;
   region.max_y -= (40*region_height)/100;
};

void Bird::add_t_shirt(Sprite *picture) {
//	if (dont_add_t_shirt) return;
	if (t_shirt != NULL) {
		remove_follower(t_shirt);
		tt_screen->remove(t_shirt);
		t_shirt->destroy();
	};
	t_shirt = picture;
	if (picture->pointer_to_leader_offsets() == NULL) {
		// a bit of magic to get the geometry right -- shouldn't be necessary...
		update_display(); // moved here on 140802 so all callers do this
	   fix_t_shirt();
		t_shirt->remove_from_floor(FALSE);
		add_follower(t_shirt);
	} else {
		// new on 260103
		add_follower(t_shirt,TRUE,INSERT_AT_END,TRUE);
	};
	t_shirt->set_graspable(FALSE);
	t_shirt->set_selectable(FALSE);
// if (showing_on_a_screen()) tt_screen->add(t_shirt);
//   update_display(); // seeing if this helps when bird is in notebook
};

void Bird::fix_t_shirt() {
   TTRegion region;
   t_shirt_region(region);
   t_shirt->set_size_and_location_to_region(region);
};

/*
   city_coordinate picture_width = true_width()/2;
   city_coordinate max_picture_height = true_height()/3;
   t_shirt->set_width_maintain_proportionality(picture_width);
   if (t_shirt->current_height() > max_picture_height) {
			// too tall
      t_shirt->set_height_maintain_proportionality(max_picture_height);
//			picture_width = t_shirt->current_width();
   };
	t_shirt->move_to(llx+width/5-t_shirt->current_width()/2,
						  lly+(2*true_height())/5-t_shirt->current_height()/2);
	add_follower(t_shirt);
//	t_shirt_added = TRUE;
*/

//void Bird::remove_t_shirt() {
//	t_shirt_added = FALSE;
//	remove_follower(t_shirt);
//};

//void Bird::inside_thought_bubble() {
//	if (t_shirt_added) {
//		remove_t_shirt();
//	};
//	dont_add_t_shirt = TRUE;
//};

Sprite *Bird::select(TTRegion *region, SelectionReason reason, Sprite *in_hand) {
	if (reason == MIGHT_DROP_ITEM) {
		if (acceptable(in_hand)) {
			return(this);
		} else {
			return(NULL);
		};
		// rewritten on 111104 -- seems to only differ in handling of REMOTE_PICTURE
		//switch (in_hand->kind_of()) {
		//	case INTEGER:
		//	case TEXT:
  //       case SOUND:
		//	case CUBBY:
		//	case PICTURE:
		//		return(this);
		//	default:
		//		return(NULL);
		//};
   } else if (t_shirt != NULL && reason == MIGHT_USE_VACUUM && t_shirt->overlaps(region)) { 
		// overlaps condition new on 261199
		return(t_shirt);
	} else {
		return(this);
	};
};

Sprite *Bird::selected(SelectionReason reason, Sprite *by) {
	if (reason != COPIER_USED && reason != EXPANDER_USED && !in_notebook() && (leader == NULL || leader->kind_of() != PROGRAM_PAD)) {
      // added last test because could be the open pages of a notebook so don't act like they are in_notebook() - new on 210802
		morph_to_life();
	};
	return(Sprite::selected(reason,by));
};

Sprite *Bird::released(Sprite *by, boolean top_level, SpecialDrop special_drop) {
	// do this first to update priority etc.
	Sprite *released_item = Sprite::released(by,top_level,special_drop);
	if (nest == NULL && !primitive() && (leader == NULL || leader->kind_of() != COPIER)) {
		// last condition new on 160304 in case copying bird w/o nest with wand -- since the wand assumes the attached doesn't go away here
		// release should not start this since bird will end up
		// under cubby it came from
		// not clear if this should happen when training a robot (but then again if copying the thought bubble of another robot...) - 130504
//		saved_status = status;
      set_saved_by(by); // replaces the above as of 171102
		if (create_nest(by)) { // set stuff up now
			// and all went well
			record_event(NEW_ITEM,by,floor,nest,TRUE,0,ADD_TO_WORKING_SET_REGARDLESS);
			if (on_back_of_picture()) {
				build_nest(by);
			} else {
				do_after(default_duration(250),this,BUILD_NEST_CALLBACK);
			};
		}; // else do nothing (new on 121199)
	} else {
		morph_to_inert();
//      ::completed(by); // commented out on 190103 since Sprite::released does this just fine
	};
	if (t_shirt != NULL) {
		t_shirt->released(by,FALSE,special_drop); // new on 110604
	};
	return(released_item);
};

void Bird::unselect() { // new on 110604
	if (t_shirt != NULL) {
		t_shirt->unselect();
	};
	Sprite::unselect();
};

void Bird::remove_selected_element(Sprite *element, SelectionReason reason, Sprite *by, boolean add_to_floor) {
   if (reason == VACUUM_USED && element == t_shirt) {
      remove_follower(t_shirt);
		t_shirt->set_graspable(TRUE);
		t_shirt->set_selectable(TRUE);
      t_shirt = NULL;
		if (floor != NULL && add_to_floor) {
			floor->add_item(element);
		};
	}; // otherwise don't do anything -- will cause busy waiting
};

void Bird::generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection) {
	if ((!did(BIRD_RECEIVED_ITEM) && tt_help_counter == 0) 
		 || tt_help_counter == 1) {
		tt_help_history_action = BIRD_RECEIVED_ITEM;
		if (tt_help_counter == 0) {
			describe_pointing(message,tool_in_hand,selection);
		};
		message << S(IDS_BIRD_RECEIVED_ITEM_HELP)
				  << S(IDS_BIRD_RECEIVED_ITEM_HELP2);
		return;
	}; 
	// can add help here about getting a bird's notebook later
	if ((!did(TYPE_HELP_GIVEN) && tt_help_counter == 0) || 
		 tt_help_counter == 2) {
//		tt_help_history_action = TYPE_HELP_GIVEN;
		message << S(IDS_BIRD_HELP_GIVEN_HELP)
				  << S(IDS_BIRD_HELP_GIVEN_HELP2);
		if (!(tt_histories[NEST]&&TYPE_HELP_GIVEN)) {
			message << S(IDS_BIRD_HELP_GIVEN_NESTS);
		};
		no_help_interruptions();
		user_did(BIRD,TYPE_HELP_GIVEN);
	};
};

Sprite *Bird::used(Sprite *, Sprite *by,
						 boolean , boolean ,
						 millisecond , boolean , Sprite *) {
	by->completed();
	simulate_mouse_up(); // so each click makes a notebook?
/*
	// pressing down a bird so a notebook hops out
	if (primitive != NOT_BUILTIN) {
		Notebook *doc = documentation_notebook(primitive,floor);
		floor->add_item(doc);
		record_event(APPLY_GRASPED_ITEM,by,floor);
		record_event(NEW_ITEM,by,floor,doc,TRUE);
//   if (body_cubby == NULL) return(NULL);
// could animate the following like the vacuum -- i.e. starts full width
		doc->animate_width_maintain_proportionality(4*tile_width,750);
		doc->set_size_and_location(1,1,llx,lly);
		doc->animate_location(llx,lly-4*tile_width,750);	
	};
*/
	return(this);
};

void Bird::build_nest(Sprite *by) {
	// create_nest() has already been called
//	city_coordinate new_height;
//	if (saved_height == 0) {
//		new_height = 3*height/2;
//	} else {
//		new_height = 3*saved_height/2;
//	};
   set_return_floor(floor);
	return_x = llx;
	return_y = lly;
	if (leader != NULL) {
//		set_saved_size(0,0); // flush what was there
		set_size_percent(50);
//		save_size();
		leave_leader();
	};
	Sprite *container;
	if ((by == NULL || by->kind_of() != ROBOT) && 
		 (floor != NULL && (container = floor->pointer_to_picture()) != NULL)) {
		// on back of picture
		container = container->ultimate_flipside_picture(); // added 250399
		remove_from_floor();
		container->pointer_to_background()->add_item(this);
		city_coordinate picture_center_x, picture_center_y;
		container->center_location(picture_center_x,picture_center_y);
		move_to(picture_center_x,picture_center_y);
	};
	millisecond duration = default_duration();
	if (duration > 0) {
		play_sound(BIRD_FLY_SOUND);
	};
//	animate_location(center_x,center_y,duration,NULL,
//						  this,show_nest_and_return_callback);
	set_item_to_deliver(NULL); // not delivering anything
	city_coordinate nest_x, nest_y;
	nest->lower_left_corner(nest_x,nest_y);
	if (duration == 0) { // new on 080799
		show_nest_and_return();
	} else {
		fly_to(nest_x,nest_y,SHOW_NEST_AND_RETURN_CALLBACK);
	   // should also record this or add to working set like hatch_bird does??
	};
};

boolean Bird::create_nest(Sprite *by) {
//	Background *nests_floor;
	Sprite *container = NULL; // wasn't initialized prior to 130504
	boolean on_back_of_picture = (floor != NULL && (container = floor->pointer_to_picture()) != NULL);
	if (by != NULL && by->kind_of() == ROBOT) { // then nest should be created at same level -- added on 260399
		container = leader;
	} else {
		if (on_back_of_picture) {
	//		nests_floor = container->pointer_to_background(); 
			container = container->ultimate_flipside_picture(); // added on 250399 in case flipped picture on back of flipped picture
		} else {
	//		nests_floor = floor;
			if (container == NULL) { // e.g. came from a notebook by itself
				container = this;
			} else if (leader != NULL) {
				container = leader; //ultimate_leader();
			} else { // new on 121199 something wrong -- e.g. is part of an erased box
				return(FALSE);
			};
		};
	};
	nest = new Nest(0,0,0,0,0,NEST_EMPTY);
	if (leader != NULL) { // if contained then bird and nest 1/2 size
		nest->set_size_percent(50);
	};
//	nest->save_size();
//	nest->set_size(1,1);
	nest->update_display(); // so all this is propagated properly??
//	city_coordinate container_llx, container_lly;
//	container->lower_left_corner(container_llx,container_lly);
	city_coordinate container_lly;
	if (on_back_of_picture) {
		container_lly = container->current_lly();
	} else {
		container_lly = lly; // new on 121199
	};
	// compute new location centered to left of "holder"
	city_coordinate center_x, center_y; // not really center but lower left
	if (on_back_of_picture) {
		center_x = container->center_x();
	} else {
		center_x = llx; //(free_region->min_x+free_region->max_x)/2;
	};
	center_y = container_lly-4*tile_height;
	if (center_y < tile_height) { // off the bottom or almost
		center_y = container_lly+container->current_height()+tile_height;
	};
	if (center_x < -2*tile_width) {
		center_x = 0;
	} else if (center_x > ideal_screen_width-tile_width) {
		center_x = 18*tile_width;		
	};
//	if (tt_log_version_number > 14) {
		// added on 290699 since if there are many nestless birds on the back of the same picture they shouldn't go exactly to the same place
		// moved by max 2*tile
		center_x += my_random(4*tile_width)-2*tile_width;
		center_y += my_random(4*tile_height)-2*tile_height;
//	};
	nest->move_to(center_x,center_y);
#if TT_DEBUG_ON
   if (tt_debug_mode == 13579) {
      nest->print(tt_error_file());
      tt_error_file() << " incrementing bird count due to create_nest of ";
      print(tt_error_file());
      tt_error_file() << endl;
   };
#endif
	if (t_shirt != NULL && t_shirt->kind_of() == TEXT) { // new on 101199 - thanks Miki for the suggestion
		Text *label = (Text *) t_shirt->copy();
		if (label->is_blank()) {
			label->become_non_blank(); // new on 250903
		};
		nest->set_label(label);
	};
	nest->increment_bird_count();
	return_x = llx;
	return_y = lly;
   set_return_floor(floor);
	if (container != NULL && container->kind_of() == CUBBY) {
		waiting_for = (Cubby *) container;
		waiting_for->increment_busy_counter();
	};
	return(TRUE);
};

void Bird::show_nest_and_return() {
	set_parameter(BIRD_MAKE_NEST);
	set_selectable(FALSE);
	if (showing_on_a_screen()) {
		set_cycle_stopped_callback(NEST_IS_MADE_CALLBACK);
	} else { // new on 080799
		nest_is_made(); // when off screen this must happen before later things
	};
};

void Bird::update_return_floor() {
   // cubby may have moved and no longer be same as return_floor
   if (return_cubby == NULL) return;
   Background *cubby_floor = return_cubby->pointer_to_background();
   if (cubby_floor == NULL) { // cubby is in limbo so return invisibly
//		return_floor = NULL;
      set_return_floor(NULL); // rewritten on 191102 so the old one is released
		return;
	};
   if (cubby_floor != return_floor) {
      set_return_floor(cubby_floor);
      // do I really need both return_cubby and return_floor simultaneously? -- if return_cubby != NULL then return to its floor
   };
};

void Bird::set_inside_vacuum(boolean new_flag, boolean recur) { // new on 030803 so that no dangling references to return_floor
	set_return_floor(NULL);
	Sprite::set_inside_vacuum(new_flag,recur);
};

void Bird::nest_is_made() {
	if (floor == NULL) return; // added 121199 to be more robust
	floor->add_item(nest);
//	nest->move_to(llx+width/2,lly);
//	nest->move_by(width/4,0);
	nest->set_size_percents(get_width_percent(),get_height_percent());
	nest->animate_delta_location(nest->current_width()/-6,nest->current_height()/-4,
	                             nest->default_duration(400));
	fly_back();
};

boolean Bird::ok_to_delete() {
	// wouldn't this be better and simpler if when global_picture_will_animate_me is set to TRUE to increment ref count 
	// and then decrement later? - asked 190103 
   if (global_picture_will_animate_me && ref_count == 1) { //  && ref_count == 1 new on 051199
      tt_global_picture->remove_animations_of(this);
   };
   return(Sprite::ok_to_delete());
};

millisecond Bird::fly_to(city_coordinate x, city_coordinate y, AnimationCallBack callback,
								 millisecond minimum_duration, boolean outside) {
	millisecond duration;
	if (inside_vacuum()) {
		action_completed(); // new on 041202
		return(0);
	};
	if (outside) {
		duration = outside_flight_duration(x,y,minimum_duration);
	} else {
		duration = max(minimum_duration,default_duration(duration_delta_location(llx,lly,x,y)));
	};
	set_parameter((short) direction(x-llx,y-lly));
	set_selectable(FALSE); // too hard to deal with programmer grabing birds
	set_containable_by_background(FALSE); // so can fly thru door or "windows" -- restored when inert again
	set_priority(bird_flying_priority);
//	do_after(duration,this,set_selectable_true_callback);
   if (duration == 0) {  //  && tt_log_version_number >= 7
      move_to(x,y);
		if (minimum_duration != 0) {
			// otherwise no need to wait at all - e.g. all this is off screen - new on 190103
			duration = 500; // try again in 1/2 second
		};
      global_picture_will_animate_me = TRUE;
   } else {
		if (finishing_instantly()) { // new on 210104 to stop infinite recursions
			global_picture_will_animate_me = TRUE;
		};
	   animate_location(x,y,duration,this);
   };
   // by using tt_global_picture this will run later regardless of tt_finish_all_animations
	// moved here so this happens after both branches above - prior to 070803 this bird was responsible for callback if duration != 0
	if (global_picture_will_animate_me) {
		tt_global_picture->do_after(duration,this,callback);
	} else {
		do_after(duration,this,callback); // new on 031003 since this will work better with finish instantly
	};
//	tt_global_picture->do_after(duration,this,callback);
//	if (done) {
//		do_after(duration,this,bird_morph_to_inert_callback);
//	};
	return(duration);
};

millisecond Bird::fly_to_goal(Sprite *goal_sprite, // city_coordinate *x, city_coordinate *y,
                              city_coordinate x_offset, city_coordinate y_offset,
                              AnimationCallBack callback, millisecond duration) {
   city_coordinate x,y;
   goal_sprite->lower_left_corner(x,y);
	set_parameter((short) direction(x-llx,y-lly));
	set_selectable(FALSE); // too hard to deal with programmer grabing birds
	set_priority(bird_flying_priority);
	animate_location_to(goal_sprite,duration,this,callback,x_offset,y_offset);
	return(duration);
};

ReleaseStatus Bird::item_released_on_top(Sprite *item, Sprite *by,
													  SpritePointer &, int ) {
	if (animation_or_callbacks_in_progress()) { // was just animation_in_progress prior to 130504
		by->abort_action_if_robot(IDS_BIRD_TOO_BUSY);
		return(RELEASE_IGNORED);
	};
   if (t_shirt == NULL) { // && item->kind_of() == PICTURE) {
      TTRegion t_shirt_region;
      item->full_region(t_shirt_region);
      TTRegion shirt_region;
      t_shirt_region_drop_region(shirt_region);
      if (shirt_region.contains(t_shirt_region)) {
//			if (item->kind_of() == PICTURE) { // commented out on 030399
				add_t_shirt(item);
//			} else {
//				add_t_shirt(make_indirect_picture(item)); // new on 030399
//			};
         return(RELEASE_ACCEPTED);
      };
   };
	// can nest ever be NULL??
// following handled by :receive_item now
//	if (nest != NULL && nest->deleted()) {		// e.g. sucked up in a vacuum by a robot
//		leave_leader(); // in case it is inside something
//		fly_to(-5*tile_width,-5*tile_height,destroy_bird_callback);
//		completed(action_status);
//		return(RELEASE_HANDLED);
//	};
	if (delivery_status != NO_DELIVERY && delivery_status != DELIVERY_COMPLETE_AND_RETURNING) {
		return(RELEASE_IGNORED); // new on 031202 so you can't drop 2 things on a bird
   };
	millisecond duration = default_duration(1800);
	if (acceptable(item)) {
		record_event(GIVE_BIRD_ITEM,by,floor,this);
		if (item->is_main_cubby_in_body()) { // new on 060705 so if main cubby given to a bird the robot doesn't get confused
			floor->need_new_main_cubby();
		};
		if (receive_item(item,by,duration,this,item)) {
		   return(RELEASED_ITEM_TO_BIRD);
      } else {
         return(RELEASE_IGNORED);
      };
	};
	if (duration > 0) show_confusion();
   if (item->kind_of() == BIRD || item->kind_of() == NEST) {
	   by->abort_action_if_robot(IDS_BIRD_WONT_ACCEPT,
									     BIRD,
									     !told_user(TOLD_BIRD_WONT_ACCEPT));
	   user_told(TOLD_BIRD_WONT_ACCEPT);
   };
	return(RELEASE_IGNORED);
};

boolean Bird::acceptable(Sprite *item) {
	switch (item->kind_of()) {
	// computationally ok, but confusing and weird to take a nest to a nest	
	// so need to put bird in a cubby to send it
	  case NEST:
	  case BIRD:
		  return(FALSE); // was tt_user_is_debugger prior to 060301 but that causes other bugs and is a bad idea // enabled if -k 1 -- reusing this flag for now
	  case INTEGER:
	  case TEXT:
     case SOUND:
	  case PICTURE:
	  case REMOTE_PICTURE:
//		  if (primitive != NOT_BUILTIN && !floor->inside_rule_body()) {
			  // primitives only accept cubbies
//			  return(FALSE);
//		  };
		  // otherwise fall thru
	  case CUBBY:
		  return(TRUE);
	  default:
		  return(FALSE);
   };
};

void Bird::set_return_cubby(Cubby *cubby) { // new on 091202 to maintain reference counts
	if (cubby == return_cubby) return;
	if (return_cubby != NULL) { // don't think this currently happens
		return_cubby->destroy();
	};
	return_cubby = cubby;
	if (return_cubby != NULL) {
		return_cubby->increment_ref_count(this);
		//if (return_path == NULL) { // new on 150603 -- can't generate path since don't know where bird was
		//	boolean ignore;
		//	return_path = return_cubby->path_to(this,ignore);
		//};
	};
};

void Bird::leave_leader() {
	if (leader != NULL) { // inside a cubby
		switch (leader->kind_of()) {
			case CUBBY:
//            if (return_cubby != NULL) return_cubby->destroy(); // if zero ref count -- set_return_cubby takes care of this now
				set_return_cubby((Cubby *) leader);
				boolean ignore;
				return_path = return_cubby->path_to(this,ignore);
				return_cubby->remove_me(this,TRUE);
//            return_cubby->increment_ref_count(this); // I'm responsible for decrementing after I stop referring to it -- taken care of by set_return_cubby now (091202)
//            tt_error_file() << "Leaving cubby " << this << " frame=" << tt_frame_number << endl;
				break;
/* bird shouldn't be in a nest
			case NEST:
				((Nest *) leader)->remove_contents(duration,FALSE);
				break;
*/
			default:
				leader->remove_selected_element(this,NO_SELECTION_REASON_GIVEN,NULL,TRUE);
//			default:
//				say(S(IDS_EXPECTED_BIRD_IN_BOX));
		};
//		if (return_path != NULL) {
//			delete return_path;
//			return_path = NULL;
//		};
	} else {
      if (return_cubby != NULL) {
			return_cubby->destroy();  // if ref_count is zero
		   return_cubby = NULL; 
		};
	};

};

void Bird::receive_item_again() {
	// used some free instance variables temporarily here
	if (about_to_quit()) return; // this can be on tt_global_picture and run while cleaning up - new on 070403
	receive_item(saved_item,saved_by,saved_duration,this,saved_item);
};

void Bird::receive_new_item(Sprite *item) {
   if (showing_on_a_screen()) tt_screen->add(item);
   receive_item(item,NULL,0,NULL,item);
};

boolean Bird::receive_item(Sprite *item, Sprite *who_by, millisecond duration, Sprite *original_recipient, Sprite *original_item) {
#if TT_DEBUG_ON
	if (debug_counter == tt_debug_target) {
		log(_T("Debug target is a bird receiving something."));
	};
#endif
   if (inside_vacuum()) {
		if (who_by != NULL) { // added for robustness on 050304
			who_by->abort_action(IDS_ROBOT_MISSING_RECIPIENT_ABORT);
		};
      return(FALSE);
   };
	update_pointer_to_nest(); // in case forwarding - new on 051199 
   if (nest != NULL && nest->ultimate_forwarding_to()->part_of(item)) { //item->contains(nest)) {
		// added ultimate_forwarding_to on 140605 since may contain a nest that was bammed with this bird's nest
		if (who_by != NULL) { // added for robustness on 050304
			who_by->abort_action(IDS_BIRD_RECUR_ABORT,NONE_GIVEN,4);
		};
		return(FALSE);
	};
   if (floor == NULL || (floor->outside() && floor->pointer_to_picture() == NULL)) { // floor == NULL added 250399
		if (who_by != NULL) { // added for robustness on 050304
			who_by->abort_action(IDS_BIRDS_ONLY_START_FLYING_FROM_INSIDE_A_HOUSE_ABORT);
		};
      return(FALSE);
   };
	floor->destroy_previous_mouse_if_involving(item); // new on 071003 so if item or its parts are clobbered no problem with undoing Bammer
	boolean training_a_robot = floor->inside_rule_body(); 
	Sprites *nests = NULL;
	if (nest != NULL) {
      if (training_a_robot) {// && !nest_inside_rule_body()) {
         nests = new Sprites(nest);
      } else {
         nests = nest->all_nests(TRUE,TRUE);
//			nests = insert_at_beginning_and_remove_duplicates(nest,nests); // new on 111204 since important that the nest is first
#if TT_DEBUG_ON
			if (tt_debug_mode == 101 || tt_debug_mode == 133) {
				tt_error_file() << "About to broadcast to " << nests->length() << " birds on frame " << tt_frame_number << endl;
			};
#endif
      };
   };
	if (nests == NULL) {
		// commented out the following on 250399 since it was a bad policy
		// why should a robot stop just because a nest has been vacuum - it should be like a "sink"
//		if (nest != NULL && nest->inside_vacuum()) {
			// might still be spit back out so don't destroy bird
//			who_by->abort_action_if_robot(the_action_status,S(IDS_BIRDS_NEST_IN_VACUUM),
//											  NONE_GIVEN,acceptable(item));
//			return(FALSE);
//		};
		if (nest != NULL && nest->contained_in_robots_thought_bubble()) {
//			was nest->contained_in(THOUGHT_BUBBLE) when TT_ALPHA2 was new			
			if (who_by != NULL) { // added for robustness on 050304
				who_by->abort_action(IDS_BIRDS_NEST_IN_BUBBLE);
			};
			return(FALSE);
		};
//		Sprite *my_container = ultimate_container();
		if (on_back_of_picture() || // primitive() || // primitive() added 111299 since primitive birds were broken - 
			 // and commented out again on 010301
			 // (my_container != NULL && my_container->is_picture()) || // kind_of() == PICTURE) ||
			 flying_outside()) {
			// there is no nest and I'm on the back of a picture or watched from outside
         set_item_to_deliver(item); // for the following
			if (primitive() && who_by != NULL) { // new on 280201
				set_saved_by(who_by);
			};
			deliver_to_nowhere();
			if (who_by != NULL) { // added for robustness on 050304
				who_by->completed();
			};
		} else {	// else off, destroy item and return
			set_destroy_item(TRUE);
			receive_item_each(item,who_by,duration);
		};
		return(TRUE);
	};
	Sprites *remaining = nests;
	// don't copy anything for first one
	if (duration == 0) { 
		// new on 200304 to skip over those who don't need to be seen receiving the item
		Sprite *nest;
		while (remaining != NULL && (nest = remaining->first()) && nest->pointer_to_background() == NULL && !nest->ultimate_container()->held()) {
			// prior to 010704 condition was !remaining->first()->showing_on_a_screen() but then you don't see birds flying between houses
			Sprite *item_or_copy; // new on 200304 since all but one should be copies
			if (remaining->rest() == NULL) {
				item_or_copy = item;
			} else {
				item_or_copy = item->copy();
			};
			((Nest *) nest)->accept_contents(TRUE,item_or_copy);
			remaining->set_first(NULL); // clean up before deleting
			remaining = remaining->rest();
		};
	};
	if (remaining == NULL) { //  just deal with them all above -- new on 200304
		if (who_by != NULL) { // added for robustness on 050304
			who_by->completed();
		};
	} else {
		// commented out the following on 111204 since don't want the bird to change which nest it has just because of broadcasting
	//	set_nest((Nest *) remaining->first());
	//	remaining->set_first(NULL); // clean up before deleting
	////	receive_item_each(item,who_by,duration,the_action_status);  moved to below
	//	remaining = remaining->rest();
		boolean my_nest_in_all_nests = FALSE;
		boolean my_nest_no_good = (nest->ignore_me(tt_dumping_background != tt_city) || !nest->ok_to_activate()); //new on 141204
		// added arg to ignore_me on 130505 so if saving a city the nest can be in limbo (e.g. in Dusty)
		boolean first_nest = TRUE;
		if (remaining != NULL) {
			if (duration > 0 && remaining->rest() != NULL) { // second condition needed as of 111204
				play_sound(MAGIC_SOUND);
			};
			city_coordinate delta_x = tile_width/8;
			city_coordinate delta_y = tile_height/-8;
			city_coordinate total_delta_x = delta_x;
			city_coordinate total_delta_y = delta_y;
			while (remaining != NULL) {
				Nest *next_nest = (Nest *) remaining->first();
				if (next_nest != nest) { // condition new on 111204
					if (next_nest->ok_to_activate()) { // new on 220804 since can be in a vacuum (or notebook) or inactive?
						if (duration == 0 && !next_nest->showing_on_a_screen()) { 
							// new on 201199 to optimize case where neither birds nor next_nests are seen
							if (my_nest_no_good && first_nest) { // new on 141204 since no need to copy if not going to my own nest
								next_nest->accept_contents(TRUE,item);
								if (who_by != NULL) { // new on 170405 since if nest is no good then won't return and complete the action status
									// but since no body is returning can complete now
									who_by->completed();
								};
							} else {
								next_nest->accept_contents(TRUE,item->copy());
							};
						} else {
							Bird *bird_copy = (Bird *) copy();
							bird_copy->set_nest(next_nest);
							next_nest->increment_bird_count(); // new on 301101 - will be decremented when this temporary bird is destroyed
							// need to set bit so they don't return
							bird_copy->set_no_home_to_return_to(TRUE);
							bird_copy->move_by(total_delta_x,total_delta_y);
							total_delta_x += delta_x;
							total_delta_y += delta_y;
							floor->add_item(bird_copy);
							if (my_nest_no_good && first_nest) { // new on 141204 since no need to copy if not going to my own nest
								bird_copy->receive_item_each(item,who_by,duration);
								if (who_by != NULL) { // new on 170405 since if nest is no good then won't return and complete the action status
									// but since no body is returning can complete now
									who_by->completed();
								};
							} else {
								Sprite *item_copy = item->copy();
								floor->add_item(item_copy);
								if ((my_nest_in_all_nests || !first_nest) && who_by != NULL) { // new on 060705
									// since if there are multiple nests to fly to the robot should wait until ALL have been completed
									who_by->add_completion();
								};
								bird_copy->receive_item_each(item_copy,who_by,duration); // ,NULL
							};
						};
						first_nest = FALSE;
#if TT_DEBUG_ON
					} else {
						tt_error_file() << "Inactive nest ignoring what bird received." << endl;
#endif
					};
				} else if (!my_nest_no_good || training_a_robot) { 
					// condition new on 280505 since if for example nest is in vacuum then don't do receive_item_each below
					// added training_a_robot on 140605
					my_nest_in_all_nests = TRUE;
				};
				remaining->set_first(NULL); // clean up before deleting
				remaining = remaining->rest();
			};
		};
		// moved here since if duration is 0 will move bird and confuse bird copies
		if (my_nest_in_all_nests) { // condition new on 111204
			receive_item_each(item,who_by,duration);
		};
	};
	if (nests != NULL) {
//		nests->remove_all();
		delete nests; // this should just be a list of NULLs
	};
   return(TRUE);
};

Sprite *Bird::deliver_to_nowhere(short int) {
	//  primitive birds catch this and do additional stuff
	item_to_deliver->remove_from_floor(FALSE,TRUE); // second arg added on 090105 since it must be removed from the screen as well
   item_to_deliver->destroy(this); 
	item_to_deliver = NULL; // new on 111202 - needed by primitive birds
	return(NULL);
};

void Bird::receive_item_each(Sprite *item, Sprite *by, millisecond duration) { 
	// removed , boolean last_one on 021202 since relying upon no_home_to_return_to now
   set_saved_by(by);
//	action_status = the_action_status;
	if (item->is_busy()) {
		saved_item = item;
		saved_duration = duration;
//		do_after(1000,this,receive_item_again_callback);
		if (item->kind_of() == CUBBY) { // right now can't be anything else
			((Cubby *) item)->run_when_non_busy(this,RECEIVE_ITEM_AGAIN_CALLBACK);
		};
		return;
	};
#if TT_DIRECT_PLAY
	if (nest != NULL && nest->proxy()) {
		Nest *networked_nest = NULL;
		if (nest->pointer_to_direct_play() == NULL) { 
			// check first if local - rewritten on 280400
			networked_nest = find_nest_with_same_guid(nest->pointer_to_nest_guid(),tt_networked_nests);
			if (networked_nest != NULL && networked_nest->pointer_to_background() != NULL) { 
				// second condition added as experiment on 261203
				reset_nest();
				set_nest(networked_nest); // so next time it'll know where to go without using the network since its nest is local
				nest->set_parameter(NEST_EMPTY); // new on 030500
			};
		};
		if (networked_nest == NULL && !nest->ready_to_receive_messages() && !about_to_quit()) {
			// nest isn't connected and its outgoing queue is full so try again later
			// about_to_quit() added 010500
			saved_item = item;
			const millisecond max_duration = 60000; // one minute
			if (saved_duration < max_duration) {
				saved_duration = duration+1000; // wait one second longer each time -- new on 050304
			} else {
				saved_duration = duration;
			};
			saved_by = by; // new on 050304
			tt_global_picture->do_after(saved_duration,this,RECEIVE_ITEM_AGAIN_CALLBACK); // saved_duration was 1000 prior to 050304
			return;
		};
	};
#endif
	Sprite *nest_container = NULL;
	if (nest != NULL && !nest->inside_vacuum()) {
		nest_container = nest->ultimate_container();
	};
	if (nest_container != NULL && nest_container->inside_vacuum()) { // added 250399 so don't see bird flying around when it shouldn't
		// what if a robot is doing this? Fail?
      action_completed();
		return;
	};
	// commented since why is this an important optimization?
//	if (nest != NULL && !nest->on_back_of_picture() && !nest->showing_on_a_screen() && nest->pointer_to_background() == pointer_to_background()) {
		// new on 260301 since if bird and nest are in same house and not being watched then why show anything
//		nest->accept_contents(TRUE,item);
//		completed(action_status);
//		return;
//	};
	Sprite *my_container = ultimate_container();
	// on 6/17/98 changed this so that you don't see birds when flying on back of a picture that is in a box
	if (//nest != NULL && // added on 030500 - removed on 260301 since nest_container will be NULL if nest is NULL
		 nest_container != NULL && 
		 nest->on_back_of_picture() &&
		// nest_container->is_picture()) && 
		// added an extra set of parens below on 010301 since disjunction was "competing" with the above
       (((nest_container == my_container && 
		 // fixed following on 040399
		  (!nest_container->showing_on_a_screen() || 
		   nest_container->kind_of() == PICTURE)) || // added 230299 so that if bird and picture are in the same cubby you see the bird fly
         (// tt_log_version_number > 9 &&
          (nest_container->outside() ||
           (my_container != NULL && on_back_of_picture()) ||
			  //  && my_container->outside()) -- removed this on 151200 so if both on back of picture bird takes no time
			   common_picture_container(nest)))) ||  
				// added on 080699 so that if ultimate container is a box you don't see bird if both are on same picture
			  // following disjunct added 191199
		 ((nest->pointer_to_background() == NULL || 
		   (nest->on_back_of_picture() && nest_container->pointer_to_background() == NULL)) && 
		  my_container != NULL && !showing_on_a_screen()) ||
		  // following disjunct added on 060300 so that if bird is in another house 
		  // and nest is on the back of a picture you don't see the flight
		 (!showing_on_a_screen() && my_container != NULL && !my_container->showing_on_a_screen() && nest->on_back_of_picture()))) {  
		// on flipside and have same ultimate container
//		deliver_instantly();
      nest->accept_contents(TRUE,item);
//		completed(action_status);
      if (!no_home_to_return_to) by->completed(); // conditional was last_one prior to 021202 -- if a copy then robot isn't waiting for her
		return;
	};
	Sprite *saved_leader = leader; // new on 051200
	leave_leader();
   set_return_floor(floor);
	millisecond container_duration;
   boolean in_same_room = (nest_container != NULL && my_container != NULL &&
                           nest_container->pointer_to_background() == my_container->pointer_to_background());
	delivery_status = HEADED_TO_NEST; // moved here from inside the first branch below on 130203
	boolean nest_in_programmers_pocket = (nest_container == tt_programmer->pointer_to_tool_in_hand());
	if (duration > 0 || 
		 (container_duration = my_container->default_duration()) > 0 ||
		 (nest_container != NULL && nest_container->default_duration() > 0)) {
//		if (duration == 0 && container_duration > 0) { // new on 191200
			// e.g. robot is on the back of a picture but picture is visible and so is nest
//			duration = container_duration;
//		};
		morph_to_life();
      set_selectable(FALSE);
		set_cycle_stopped_callback(BIRD_FLY_UP_CALLBACK);
//		bird->set_parameter(BIRD_FLY_UP);
      return_x = llx;
      return_y = lly;
		set_item_to_deliver(item);
		city_coordinate item_width,item_height;
		item->good_size(item_width,item_height,this);
		// following handled by passing this to good_size above
		// shrink or grow to compensate for bird's growth or shrinkage
//		item_width = (item_width*width)/good_width();
//		item_height = (item_height*height)/good_height();
//		play_sound(BIRD_FLY_SOUND);
//		leave_leader();
		// following used to be in fly_to_nest but problem is that
		// item is neither updated by floor or by this bird
//		item->save_size();
//		item->animate_size(item_width,item_height,duration); "moved" to each branch below on 080705
// problem with the following is that the bird may be changing size too
//		city_coordinate good_width,good_height;
//		item->good_size(good_width,good_height);
//		item->animate_size(good_width,good_height,duration);
		if (duration == 0) { 
			// shouldn't be necessary but general case screws up
			// since fly_to begins to run before add_follower
			item->set_size(item_width,item_height);
			Background *new_floor = NULL; // = my_container->pointer_to_background();
			if (saved_leader != NULL && !nest->showing_on_a_screen()) { 
				// new on 051200 so don't waste time (or make flying noise) unnecessarily -- second condition added 160101
				new_floor = saved_leader->pointer_to_background();
			} else { // new on 191200
				new_floor = nest_container->pointer_to_background();
			};
			if (new_floor == NULL) { // added on 081299 for robustness - I think it is OK to do nothing in this case
				add_follower(item,FALSE); // copied here on 070301 so it happens even when new_floor is NULL
			} else if (new_floor != floor) { // I'm on flipside of visible picture
//				set_background(my_container->pointer_to_background());
				floor->remove_item(this,FALSE); // FALSE added 191200 to avoid spurious warning
				item->remove_from_floor(FALSE);
//				if (item->pointer_to_leader() != NULL) {
//					pointer_to_leader()->remove_follower(item);
//				};
				set_to_good_size(); // new on 030405 -- since otherwise when flies out things carried can be very huge
				add_follower(item,FALSE); // moved here back here on 070301 (on 280201 had moved it before conditional but then was still on floor) 
				city_coordinate x,y;
				my_container->center_location(x,y);
				move_to(x,y);
				if (new_floor->add_item(this) && new_floor->showing_on_a_screen()) { // condition new on 180200
					// following commented out on 171199 since visible_background::add_item will do this instead
					// revised on 180200
					tt_screen->add(item); // this was unconditional and commented out prior to August 26 '98 and between 171199 and 180200
					if (tt_log_version_number >= 75) {
						item->set_to_good_size(this); // new on 130505 otherwise stuff being carried by bird have very poor geometry
					};
				};
				item->move_to(x,y-item_height);			
//				save_size();
				set_size(1,1); // start off tiny
				animate_to_good_size(container_duration/2,tt_log_version_number >= 64?tt_toolbox:NULL);
			} else {
				item->move_to(llx,lly-item_height);
				item->remove_from_floor(FALSE,FALSE);
//				add_follower(item,FALSE);
				item->set_selectable(FALSE);
			};
			// moved up to branch and conditions where appropriate
//			tt_screen->add(item); // new on 150200 since it might have been picked up on the back of something 
			// and since it is not completely inside its leader
			fly_to_nest();
		} else {
			item->animate_size(item_width,item_height,duration);
			item->animate_location_to(this, //&llx,&lly,
										     duration,this,FLY_TO_NEST_CALLBACK,
										     0,-item_height);
			item->remove_from_floor(FALSE,FALSE);
			item->set_priority(priority()+1);
			if (item->pointer_to_leader() != this) { // condition new since caller (e.g. get_item_to_nest may have done this already)
				// condition makes no sense (noticed 260505) and was introduced between 3.121 and 3.150
				add_follower(item,FALSE);
			};
			item->set_selectable(FALSE);
		};
//		if (item != NULL) {
//			item->remove_from_floor(FALSE,FALSE);
//		};
		user_did(NEST,NEST_RECEIVED_ITEM_FROM_BIRD,by);
		user_did(BIRD,BIRD_RECEIVED_ITEM,by);
	} else if (flying_outside()) {
      House *destination = pointer_to_destination_house();
      if ((destination != NULL && !in_same_room) 
			 || nest_in_programmers_pocket) { // new on 031003 so can fly to programmer outside		 
//          (floor != NULL && destination != floor->pointer_to_house())) {
		   return_x = llx;
		   return_y = lly;
		   set_item_to_deliver(item);
//		   city_coordinate item_width, item_height;
//		   item->good_size(item_width,item_height);
//		   item->set_size((item_width*width)/good_width(),(item_height*height)/good_height());
			item->set_to_good_size(this); // new on 190999 to replace the above
		   delivery_status = HEADED_TO_NEST;
		   fly_outside();
      } else { 
         //  just deliver instantly...
         set_item_to_deliver(item);
         deliver_instantly();
//         nest->accept_contents(TRUE,item);
//			set_item_to_deliver(NULL);
//         completed(action_status);
         return;
      };
	} else if (nest == NULL) {
		// new on 111202 so primitive birds work when not watched
		set_item_to_deliver(item);
		deliver_to_nowhere();
		fly_back();
	} else if (nest_in_programmers_pocket) { // new on 031003 so can fly to programmer when he is standing in another house
		return_x = llx;
		return_y = lly;
		set_item_to_deliver(item);
		item->set_to_good_size(this);
		set_size_percent(50);
		item->remove_from_floor(FALSE,FALSE);
		add_follower(item,FALSE);
		delivery_status = HEADED_TO_NEST;
		fly_to_nest();
	} else if (!nest->inside_vacuum()) {
		// why leave and come back if not being watched?
		// might be standing up in room??
//         deliver_instantly();
		nest->accept_contents(TRUE,item);
		set_item_to_deliver(NULL);
		fly_back();
	};
};

boolean Bird::flying_outside() {
	return(!tt_programmer->inside_a_house()); // rewritten on 111202
};

void Bird::fly_to_nest() {
	millisecond duration = default_duration();
	if (destroy_item_flag || floor->inside_rule_body()) { // && !nest_inside_rule_body())) {
		// fly away, destroy item, and return
		if (duration > 0) {
			play_sound(BIRD_FLY_SOUND);
		};
      city_coordinate x_goal = -5*tile_width;
      if (item_to_deliver != NULL) {  // so wide items are really carried off screen
			//if (destroy_item_flag && tt_log_version_number >= 66) { // new on 090105 
			//	// if the user follows by scrolling the bird it would see the bird setting it down prior to 090105 
			//	x_goal = tt_screen->viewing_region()->min_x;
			//};
         x_goal -= item_to_deliver->current_width();
      };
		fly_to(x_goal,7*tile_height,DESTROY_ITEM_AND_RETURN_CALLBACK);
//	} else if (primitive != NOT_BUILTIN) {
//		if (duration > 0) play_sound(BIRD_FLY_SOUND);
//		animate_location(-5*tile_width,-5*tile_height,
//							  duration,NULL,this,invoke_primitive_callback);
//		if (controlled_sprite == NULL) {
			// just fly off to primitives nest somewhere out there
//			fly_to(-5*tile_width,-5*tile_height,invoke_primitive_callback);
//		} else {
//			city_coordinate picture_x,picture_y;
//			controlled_sprite->center(picture_x,picture_y);
//			if (duration > 0) { // no need to bother if not being watched
//				animate_size(width/10,height/10,duration);
//			};
//			fly_to(picture_x,picture_y,invoke_primitive_callback);			
//		};
	} else if (nest == NULL) {
		if (duration > 0) {
			fly_to(-5*tile_width,7*tile_height,DESTROY_ITEM_AND_RETURN_CALLBACK);
		} else {
			remove_follower(item_to_deliver);
			floor->add_item(item_to_deliver); // so it will be deleted when floor is 
			item_to_deliver->move_to(-100*ideal_screen_width,-100*ideal_screen_height); // far far away
			item_to_deliver->set_available_for_indexing(FALSE); // was set_slated_for_deletion(TRUE);
			set_item_to_deliver(NULL);
         action_completed();
		};
#if TT_DIRECT_PLAY
	} else if (nest != NULL && nest->proxy()) {
		fly_to(-2*ideal_screen_width,7*tile_height,RELEASE_CALLBACK); 
		// flies far away so you can't see a big thing it is holding during pause of transmission
#endif
	} else {
		just_fly_to_nest(duration);
	};
};

//boolean Bird::nest_inside_rule_body() {
//	if (nest == NULL) return(FALSE);
//	Background *nest_floor = nest->pointer_to_background();
//	return(nest_floor == NULL || nest_floor->inside_rule_body());
//};

void Bird::destroy_item_and_return() {
	// could this be a memory leak?? asked 090105
	if (item_to_deliver != NULL) { // condition new due to crash (at Cherwell School) when vacuuming up a box with a nest that was receiving values
		remove_follower(item_to_deliver,FALSE); // added FALSE on 220804 since isn't always a follower if, for example,
		// flying on the back of a picture to a nest also on the back of the same picture
		if (floor->inside_rule_body()) { // && !nest_inside_rule_body()) {
   		// will be deleted when thought bubble "floor" terminates
			// this prevents it being "auto vacuumed" or used in indexing
	//      item_to_deliver->set_slated_for_deletion(TRUE);
			floor->add_item(item_to_deliver); // so it will be deleted when floor is --
			item_to_deliver->move_to(-100*ideal_screen_width,-100*ideal_screen_height); // far far away
			item_to_deliver->set_available_for_indexing(FALSE); // was set_slated_for_deletion(TRUE);
		} else {
			deliver_to_nowhere();
		};
		set_destroy_item(FALSE); // added on 090699 since if nest was vacuumed and then spit out this must be reset
	//	item_to_deliver->set_available_for_indexing(FALSE);
		set_item_to_deliver(NULL);
	};
	fly_back();
};

/*
void Bird::invoke_primitive() {
	if (item_to_deliver->kind_of() == CUBBY) {
		// shouldn't be called unless cubby
		Cubby *cubby = (Cubby *) item_to_deliver;
		if ((all_builtins[primitive])(cubby,this)) {	// everything fine
			set_item_to_deliver(NULL); // otherwise fly_back will release "bad message"
			if (followers != NULL) { // e.g. not running off screen
				remove_follower(cubby); 
			};
			floor->remove_item(cubby,TRUE); //,TRUE); // delete it too
//			cubby->recursively_remove_leaders(floor);
			cubby->destroy(saved_by);
//			tt_deleted_sprites = new Sprites(cubby,tt_deleted_sprites);
		};
	};
	const millisecond duration = default_duration();
	if (controlled_sprite != NULL && duration > 0) {
		// shrank to visit it so grow now
		do_after(duration,this,grow_by_ten_callback);
	};
	do_after(duration,this,fly_back_callback);
};

void invoke_primitive_callback(void *sprite) {
	((Bird *) sprite)->invoke_primitive();
};
*/

void Bird::grow_by_ten() {
	int size_percent = get_size_percent();
	// this way blitting won't need to stretch at all
	if (size_percent > 8 && size_percent < 12) { // close enough
		animate_to_size_percent(100,default_duration());
	} else {
		animate_size(width*10,height*10,default_duration());
	};
};

Background *Bird::pointer_to_nests_background() {
   if (nest->inside_vacuum() || nest->deleted()) return(NULL);
   Background *nests_floor = nest->pointer_to_background();
	if (nests_floor != NULL) {// && nests_floor->pointer_to_picture() == NULL) {
		// second condition added on 010300
		// and commented out on 180300 since it broke birds flying to the back of pictures
		return(nests_floor);
	};
   Sprite *nest_container = nest->ultimate_container();
   if (nest_container == tt_programmer->pointer_to_tool_in_hand()) return(tt_programmer->pointer_to_background());
   return(nest_container->pointer_to_background());
};

void Bird::just_fly_to_nest(millisecond duration) {
	// even if called by bird flying between houses the nest might have moved
#if TT_DEBUG_ON
	if (tt_debug_mode == 170703 || debug_counter == tt_debug_target) {
		tt_error_file() << "Bird#" << debug_counter << " is flying to nest with delivery status " 
							 << delivery_status_name[delivery_status] << " for duration " << duration << " on frame " << tt_frame_number << endl;
	};
#endif
   boolean nest_temporarily_unavailable = FALSE;
//   if (nest->inside_vacuum()) nest_temporarily_unavailable = TRUE; -- dealt with below as of 150603
   Sprite *nest_container = nest->ultimate_container();
   boolean nest_in_pocket = (nest_container == tt_programmer->pointer_to_tool_in_hand());
	if (nest_in_pocket) {
		if (tt_programmer->kind_of() == PROGRAMMER_AT_FLOOR && nest->pointer_to_leader() != NULL) { 
			// second conjunct new on 021003 since can be just the "naked" nest
			nest_in_pocket = FALSE; // just holding the box			
		} else if (tt_programmer->kind_of() != PROGRAMMER_ROOM_WALKING) { // why?? asked 170703
			nest_temporarily_unavailable = TRUE;
		};
	};
   if (nest_temporarily_unavailable) { // try again in 2 seconds
      if (finishing_instantly()) {
         deliver_instantly();
      } else {
			TTRegion *region = tt_screen->viewing_region();
			fly_to(region->random_x(),region->random_y(),FLY_TO_NEST_CALLBACK,2000); // otherwise is just frozen waiting for the nest to be dropped
//       do_after(2000,this,FLY_TO_NEST_CALLBACK); // commented out when TT_ALPHA was in use -- maybe 021003
      };
      return;
   };
   Background *nests_floor;
	// used to destroy bird prior to 190499 but bird shouldn't be destroyed just because her nest is
   if (nest->deleted() || nest->inside_vacuum()) { // includes nests in vacuum as of 150603
	   fly_to(-5*tile_width,7*tile_height,DESTROY_ITEM_AND_RETURN_CALLBACK);
		return;
   } else { 
	   nests_floor = pointer_to_nests_background();
	   if (nests_floor == NULL && delivery_status != HEADED_BACK_TO_NEST_WITH_STACK) {
			// might be in a truck (or even stuck in a truck if the city is full
			// second condition new on 200405 since if already put item_to_deliver then the following makes no sense
//	      say(S(IDS_BIRD_CANT_FIND_NEST),3,TRUE);
//     	   destroy_bird = TRUE;
			// new on 190499
			if (item_to_deliver != NULL) { // new on 111003 for robustness
				remove_follower(item_to_deliver,FALSE); // added FALSE on 090203 since needn't be a follower?
				nest->accept_contents(TRUE,item_to_deliver);
#if TT_DEBUG_ON
			} else {
				tt_error_file() << "Bird is flying to nest but has NULL item_to_deliver" << endl;
#endif
			};
			set_item_to_deliver(NULL);
			set_top_of_stack_on_nest(NULL); // new on 160405 since can happen if nest is "in limbo" -- not sure this is the best solution
			// but it avoids other inconsistencies
         delivery_status = DELIVERY_COMPLETE_AND_RETURNING;
			really_go_back(); // new on 140904 since otherwise this bird gets frozen flying in the air
			return;
      };
	};
	boolean same_floor = (nests_floor == floor);
	Background *true_floor = floor;
	if (!same_floor && duration == 0) { // maybe I'm on the flipside of a picture
		// why duration == 0 ??
		Sprite *my_picture = floor->pointer_to_picture();
		if (my_picture != NULL) {
			my_picture = my_picture->ultimate_flipside_picture(); // added 250399
			true_floor = my_picture->pointer_to_background();
         same_floor = (nests_floor == true_floor);
		};
	};
	boolean fiddling_with_stack = (delivery_status == HEADED_BACK_TO_NEST_WITH_STACK || delivery_status == HEADED_BACK_TO_NEST || 
											 delivery_status == HEADED_BACK_TO_NEST_EMPTY_HANDED); // abstracted on 150405
	city_coordinate nest_x, nest_y;
	if (!same_floor && nests_floor != NULL && !fiddling_with_stack) {
      // maybe nest is on flip side of picture on floor -- added fiddling_with_stack on 150405 so this doesn't happen while stack is set down
		Sprite *nests_picture = nests_floor->pointer_to_picture();
		if (nests_picture != NULL) {
			nests_picture = nests_picture->ultimate_flipside_picture(); // added 250399
		};
		same_floor = (nests_picture != NULL && (nests_picture->pointer_to_background() == true_floor));
		if (same_floor) {
			delivery_status = HEADED_TO_NEST_ON_FLIPSIDE;
			nests_picture->center_location(nest_x,nest_y);
			millisecond duration = nests_picture->default_duration();
			do_after(duration/2,this,SHRINK_TO_TINY_CALLBACK);
		};
	};
	if (same_floor ||  // && floor->on_the_floor()
		 // following are all new on 120203 since nest could be in a truck that drove off while moving stack around
		 // not clear if I need to test for showing_on_a_screen below
		 (showing_on_a_screen() && fiddling_with_stack)) { 
      if (duration > 0) {
			play_sound(BIRD_FLY_SOUND);
		};
		// restore this kind of tracking once I allow offsets
		//   city_coordinate *nest_llx, *nest_lly;
		//   nest->pointers_to_lower_left_corner(nest_llx,nest_lly);
		set_priority(bird_flying_priority); // floor->next_tool_priority());
//		if (delivery_status == HEADED_BACK_TO_NEST_WITH_STACK) {
//			item_to_deliver->lower_left_corner(nest_x,nest_y);
//		} else {
      if (nest_in_pocket && delivery_status == HEADED_TO_NEST) {
			if (duration == 0) { 
				// new on 160605 -- can happen if flying to nest on flipside of picture while picture is held and programmer is standing up
				release();
				return;
			};
//         city_coordinate *programmer_llx, *programmer_lly;
         Sprite *programmer_appearance = tt_programmer->pointer_to_appearance();
//         programmer_appearance->pointers_to_lower_left_corner(programmer_llx,programmer_lly);
         fly_to_goal(programmer_appearance, // programmer_llx,programmer_lly,
                     programmer_appearance->current_width()/2,programmer_appearance->current_height()/2,
                     RELEASE_CALLBACK,time_to_fly_to_programmer/room_speedup); 
         return;
		};
		if (delivery_status != HEADED_TO_NEST_ON_FLIPSIDE) { 
			if (nest_in_pocket) { // branch new on 021003 to go to programmer
				Sprite *programmer_appearance = tt_programmer->pointer_to_appearance();
				programmer_appearance->center_location(nest_x,nest_y);
			} else if (same_floor) { 
				nest->center_location(nest_x,nest_y); //lower_left_corner(nest_x,nest_y);
			} else { // new on 120203
				nest_x = ideal_screen_width/-4;
				nest_y = ideal_screen_height/3;	
			};
      };
		if (delivery_status == HEADED_BACK_TO_NEST_WITH_STACK) {
			// putting the stack back on top
//			animate_location(nest_x,nest_y,duration,NULL,this,release_callback);
			fly_to(nest_x,nest_y,RELEASE_CALLBACK);
		} else if (nest->stack_empty() || delivery_status == HEADED_BACK_TO_NEST || 
					  // stack_empty was empty prior to 071201 -- and is once again on 110203 - restored 120203
					  delivery_status == HEADED_TO_NEST_ON_FLIPSIDE) {
//			animate_location(nest_x,nest_y,duration,NULL,this,release_callback);
			fly_to(nest_x,nest_y,RELEASE_CALLBACK);
		} else if (delivery_status == HEADED_BACK_TO_NEST_EMPTY_HANDED) {// || nest->stack_removed()) {
			// added nest->stack_removed() on 110203 so if due to house change the stack has been removed then repair things - removed 120203
			// bird's "hands" free so empty the nest temporarily
//			animate_location(nest_x,nest_y,duration,NULL,this,pick_up_stack_callback);
			fly_to(nest_x,nest_y,PICK_UP_STACK_CALLBACK,duration);
		} else {
			// else fly a bit below the nest and set down message
			fly_to(nest_x,nest_y-max(nest->true_height()/2,2*tile_height),SET_DOWN_MESSAGE_CALLBACK);
		};
	} else {
		delivery_status = HEADED_TO_NEST;
		if (duration > 0) {
			play_sound(BIRD_FLY_SOUND);
		};
//      if (same_floor && !floor->on_the_floor()) {
//         nest->ultimate_container()->center_location(nest_x,nest_y);
//  		   fly_to(nest_x,nest_y,fly_to_another_house_callback,duration);
//      } else {
		   fly_to(-5*tile_width,7*tile_height,FLY_TO_ANOTHER_HOUSE_CALLBACK,duration);
//      };
	};
};

void Bird::shrink_to_tiny() {
//	save_size();
	animate_size(1,1,default_duration()/2);
};

void Bird::set_down_message() {
  // set down message and then fly back to nest and remove contents
  if (item_to_deliver != NULL && item_to_deliver->pointer_to_leader() == this) {
     remove_follower(item_to_deliver);
	  if (floor != NULL) { // can it be otherwise?
		  // this used to rely upon remove_follower not restoring background status to what it was before add_follower
		  floor->add_item(item_to_deliver); // new on 090105 since needed for Logotron bug report 19.2
	  };
//     item_to_deliver->set_selectable(TRUE);
     item_to_deliver->update_display();
  };
  delivery_status = HEADED_BACK_TO_NEST_EMPTY_HANDED;
  millisecond duration = default_duration();
  just_fly_to_nest(duration);
};

void Bird::set_down_stack() {
  // set down stack and then fly back to message
#if TT_DEBUG_ON
	if (tt_debug_mode == 170703) {
		tt_error_file() << "Bird#" << debug_counter << " set_down_stack on frame " <<  tt_frame_number << endl;
	};
#endif
  remove_follower(top_of_stack_on_nest);
//  top_of_stack_on_nest->set_selectable(TRUE);
  if (floor != NULL) {
	  floor->add_item(top_of_stack_on_nest);
  }; // new on 090105
  top_of_stack_on_nest->update_display(); // probably not needed now that the above was added on 090105
//  top_of_stack_on_nest = NULL; // new on 121202 so XML for cities doesn't get mixed up - done later now
  delivery_status = HEADED_BACK_TO_MESSAGE;
  millisecond duration = default_duration();
  city_coordinate message_x,message_y;
  item_to_deliver->center_location(message_x,message_y); // was lower_left_corner
  if (duration > 0) {
	  play_sound(BIRD_FLY_SOUND);
  };
//  animate_location(message_llx,message_lly,
//						   duration,NULL,this,pick_up_message_callback);
  fly_to(message_x,message_y,PICK_UP_MESSAGE_CALLBACK);
};

void Bird::pick_up_message() {
	item_to_deliver->remove_from_floor(FALSE);
	add_follower(item_to_deliver,FALSE);
	if (showing_on_a_screen()) tt_screen->add(item_to_deliver);
   item_to_deliver->set_selectable(FALSE);
	delivery_status = HEADED_BACK_TO_NEST;
	millisecond duration = default_duration();
	just_fly_to_nest(duration);
};

void Bird::pick_up_stack() {
//	top_of_stack_on_nest = nest->remove_contents(0,FALSE,TRUE);
	nest->remove_stack(this);
};

void Bird::got_stack(Sprite *top) {
#if TT_DEBUG_ON
	if (top->pointer_to_leader() != NULL) { // && tt_user_is_debugger) {
		say_error(_T("Bird about to pick up something held by another."));
	};
#endif
	set_top_of_stack_on_nest(top); // was ordinary assignment prior to 120203
	top_of_stack_on_nest->remove_from_floor(FALSE,FALSE);
   if (showing_on_a_screen()) tt_screen->add(top_of_stack_on_nest); // just in case
	add_follower(top_of_stack_on_nest,FALSE);
	top_of_stack_on_nest->set_selectable(FALSE);
	millisecond duration = default_duration();
	city_coordinate nest_x, nest_y;
	nest->center_location(nest_x,nest_y);
	if (duration > 0) {
		play_sound(BIRD_FLY_SOUND);
	};
//	animate_location(nest_x,nest_y+2*nest->current_height(),duration,
//						  NULL,this,set_down_stack_callback);
	fly_to(nest_x,nest_y+max(nest->true_height(),2*tile_height),
	       SET_DOWN_STACK_CALLBACK);
};

void Bird::set_top_of_stack_on_nest(Sprite *item) { // new on 120203
	if (item == top_of_stack_on_nest) return; // new on 170703 to avoid all this work
	if (top_of_stack_on_nest != NULL) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 170703) {
			tt_error_file() << "Bird#" << debug_counter << " set_top_of_stack_on_nest. Old contents were "; 
			top_of_stack_on_nest->print(tt_error_file());
			tt_error_file() << endl;
		};
#endif
		if (top_of_stack_on_nest->pointer_to_leader() == this) { // new on 190405
			remove_follower(top_of_stack_on_nest);
		};
		top_of_stack_on_nest->destroy();
	};
	top_of_stack_on_nest = item;
	if (top_of_stack_on_nest != NULL) {
		top_of_stack_on_nest->increment_ref_count();
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 170703) {
		tt_error_file() << "Bird#" << debug_counter << " set_top_of_stack_on_nest. " ;
		if (top_of_stack_on_nest == NULL) {
			tt_error_file() << "Nothing";
		} else {
			top_of_stack_on_nest->print(tt_error_file());
		};
		tt_error_file() << endl;
	};
#endif
};

#if TT_DEBUG_ON
void Bird::set_leader(Sprite *the_leader, boolean temporarily) {
	if (the_leader != NULL && top_of_stack_on_nest != NULL) {
		debug_this();
	};
	Sprite::set_leader(the_leader,temporarily);
};
#endif

void Bird::fly_to_another_house() {
	if (flying_outside()) {
      fly_outside();
	} else {
		deliver_to_another_house();
	};
};

void Bird::fly_outside() {
	boolean camera_above = (tt_screen->camera_status() == CAMERA_ABOVE);
	House *current_house = NULL;
   if (floor != NULL) current_house = floor->pointer_to_house();
   boolean current_house_visible;
   boolean already_outside = FALSE;
   city_coordinate initial_x, initial_y;
   if (current_house != NULL) {
	   current_house->exit_point(initial_x,initial_y);
//	Block *current_block = house->block_im_on();
//	Block *destination_block = destination_house->block_im_on();
//	current_block->add_extra(this);
      remove_from_floor(FALSE);
	   tt_city->add_item(this);
//	   tt_screen->add(this);
      current_house_visible = current_house->visible();
	} else {
		if (floor->pointer_to_picture() != NULL) {
			// not outside but floor still back of a picture - new on 070803
			tt_programmer->pointer_to_appearance()->center_location(initial_x,initial_y);
			remove_from_floor(FALSE);
			tt_city->add_item(this);
		} else { // already outside
			lower_left_corner(initial_x,initial_y);
		};
		current_house_visible = FALSE;
		already_outside = TRUE;
	};
	if (item_to_deliver != NULL && item_to_deliver->pointer_to_leader() != this) {
      item_to_deliver->remove_from_floor(FALSE);
      item_to_deliver->move_to(llx+(width-item_to_deliver->current_width())/2,
                               lly-(item_to_deliver->current_height()));
		add_follower(item_to_deliver,FALSE);
      item_to_deliver->set_selectable(FALSE);
		tt_screen->add(item_to_deliver);
//		tt_city->add_item(item_to_deliver);
	};
	Sprite *programmers_tool = tt_programmer->pointer_to_tool_in_hand();
   boolean programmer_has_nest = (nest->ultimate_container() == programmers_tool);
	boolean programmer_has_this_bird = (return_cubby != NULL && return_cubby->ultimate_container() == programmers_tool);
   //city_coordinate *programmer_llx, *programmer_lly;
   //if (programmer_has_nest && delivery_status == HEADED_TO_NEST) {
   //   tt_programmer->pointer_to_appearance()->pointers_to_lower_left_corner(programmer_llx,programmer_lly);
   //};
   House *destination_house = pointer_to_destination_house();
   boolean destination_house_visible = (destination_house != NULL && destination_house->visible());
	if (camera_above) {
      if (already_outside) {
         fly_outside_now_full_size();
      } else {
		   move_to(initial_x,initial_y);
         size_percent = 10;
		   set_size_percent(size_percent);
		   size_is_100_percent = FALSE;
		   set_parameter(SOUTH); // looks best
         set_selectable(FALSE);
         if (programmer_has_nest) {
            if (delivery_status == HEADED_TO_NEST) {
               Sprite *programmer_appearance = tt_programmer->pointer_to_appearance();
               fly_to_goal(programmer_appearance, // programmer_llx,programmer_lly,
                           programmer_appearance->current_width()/2,programmer_appearance->current_height()/2,
                           SHRINK_THEN_DELIVER_TO_ANOTHER_HOUSE_CALLBACK,time_to_fly_to_programmer);
               // gets bigger as she flies up
               add_ease_in_and_out_to_goal(ANIMATING_SIZE_PERCENT,100, //(75*tt_programmer->return_city_scale())/ground_scale,
												       time_to_fly_to_programmer);
//            } else {
//               fly_down();
            };
         } else {
            millisecond duration = 0;
            if (tt_show_birds_return && !finishing_instantly()) {
               duration = bird_grow_full_size_duration;
            };
		      add_ease_in_and_out_to_goal(ANIMATING_SIZE_PERCENT,100,duration,
											       this,FLY_OUTSIDE_NOW_FULL_SIZE_CALLBACK);
         };
      };
	} else {
		if (destination_house == NULL && !programmer_has_nest && !programmer_has_this_bird) { // added !programmer_has_this_bird on 070803
         if (nest->deleted() || no_home_to_return_to) {
			   tt_screen->remove(this);
			   tt_city->remove_item(this);
            action_completed();
            if (!nest->deleted() && delivery_status == HEADED_TO_NEST) {
               // destination may not be available due to
               // switching from outside to inside
					if (item_to_deliver->pointer_to_leader() == this) {
						remove_follower(item_to_deliver);
					};
               nest->accept_contents(TRUE,item_to_deliver);
               set_item_to_deliver(NULL);
            };
			   destroy(); // could be more dramatic
         } else {
//          completed(action_status);
            if (delivery_status == HEADED_TO_NEST) {
               deliver_instantly();
            } else {
               returned_to_house(TRUE); // else no point going outside
            };
         };
			return;
		};
		if (!current_house_visible && !destination_house_visible && !programmer_has_nest && !programmer_has_this_bird) { 
			// added !programmer_has_this_bird on 070803
			if (delivery_status == HEADED_TO_NEST) {
//				deliver_to_another_house();
            if (item_to_deliver->pointer_to_leader() == this) {
               remove_follower(item_to_deliver);
            };
            nest->accept_contents(TRUE,item_to_deliver);
			   set_item_to_deliver(NULL);
            delivery_status = DELIVERY_COMPLETE_AND_RETURNING;
//		   } else {
//				returned_to_house();
			};
         returned_to_house();
			return;
		};
		TTRegion *screen_region = tt_screen->viewing_region();
		city_coordinate goal_x;
		city_coordinate goal_y = screen_region->max_y+ideal_screen_height/2;
		if (current_house_visible) {
			current_house->open_door();
			play_sound(BIRD_FLY_SOUND);
		};
      if (programmer_has_nest && delivery_status != HEADED_TO_NEST && item_to_deliver != NULL) {
			if (item_to_deliver->pointer_to_leader() == this) {
				remove_follower(item_to_deliver);
			};
         nest->accept_contents(TRUE,item_to_deliver);
         set_item_to_deliver(NULL);
         if (tt_finish_all_animations) {
            returned_to_house(TRUE);
            return;
         };
      };
      if (programmer_has_nest && delivery_status == HEADED_TO_NEST) {
         tt_programmer->pointer_to_appearance()->center_location(goal_x,goal_y);   // obsolete now??
		} else if (destination_house_visible) {
			destination_house->open_door();
			if (!current_house_visible) play_sound(BIRD_FLY_SOUND);
		   city_coordinate destination_house_x, destination_house_y;
		   destination_house->exit_point(destination_house_x,destination_house_y);
		   if (destination_house_x < initial_x) {
			   goal_x = screen_region->min_x-ideal_screen_width/2; // fly past edge
		   } else {
			   goal_x = screen_region->max_x+ideal_screen_width/2;
		   };
		   if (current_house_visible) {
			   goal_x = destination_house_x;
			   goal_y = destination_house_y+2*tile_height;
		   } else { // swap them
			   if (destination_house_x < initial_x) {
				   goal_x += ideal_screen_width;
			   } else {
				   goal_x -= ideal_screen_width;
			   };
            if (!programmer_has_nest && !programmer_has_this_bird) { // second condition new on 070803
			      initial_x = goal_x;
			      initial_y = goal_y;
            };
			   goal_x = destination_house_x;
			   goal_y = destination_house_y+2*tile_height; // don't fly to very bottom of doorway
         };
		} else if (// commented out on 110203 since makes goal_x unbound - !current_house_visible && 
						return_cubby != NULL && return_cubby->pointer_to_background() != NULL) {
         // e.g. programmer has nest and bird flying back
			Sprite *return_cubby_ultimate_container = return_cubby->ultimate_container(); // added ultimate_containter() on 070803
         Background *background = return_cubby_ultimate_container->pointer_to_background();
			if (background == NULL) { // new on 070803 
				// don't check if held since what to do if not??
				tt_programmer->pointer_to_appearance()->center_location(goal_x,goal_y);
			} else {
				background->pointer_to_house()->exit_point(goal_x,goal_y);
			};
         play_sound(BIRD_FLY_SOUND);
//         goal_x = initial_x+tile_width;
//         goal_y = initial_y+ideal_screen_height;   // fly off the top of the screen
		} else {
			goal_x = -tile_width; // new on 030405 since when leaving a house it is possible that this isn't initialized 
			// (but I think it doesn't matter much what value it has)
      };
		if (destination_house_visible && destination_house->house_style() == HOUSE_B) {
			goal_y += 2*tile_height; // since door is up stairs
		};
		if (current_house_visible && current_house->house_style() == HOUSE_B) {
			initial_y += 2*tile_height;
		};
		AnimationCallBack callback;
		if (delivery_status == HEADED_TO_NEST) {
			if (destination_house_visible) {
				callback = SHRINK_THEN_DELIVER_TO_ANOTHER_HOUSE_CALLBACK;
         } else if (programmer_has_nest) {
            callback = FLY_BACK_CALLBACK;
			} else {
				callback = DELIVER_TO_ANOTHER_HOUSE_CALLBACK;
			};
		} else {
			if (finishing_instantly()) {
            returned_to_house(TRUE);
            return;
         } else if (destination_house_visible) {
				callback = SHRINK_THEN_RETURNED_TO_HOUSE_CALLBACK;
			} else {
				callback = RETURNED_TO_HOUSE_CALLBACK;
			};
		};
      if (!programmer_has_nest || delivery_status == HEADED_TO_NEST) {
         move_to(initial_x,initial_y);
      };
      boolean flight_taken_care_of = FALSE;
      if (programmer_has_nest && delivery_status == HEADED_TO_NEST) {
         city_coordinate x_offset, y_offset;
         Sprite *programmer_appearance = tt_programmer->pointer_to_appearance();
         switch (tt_programmer->kind_of()) {
            case PROGRAMMER_CITY_LANDING:
               x_offset = (2*programmer_appearance->current_width())/3;
               y_offset = (2*programmer_appearance->current_height())/3;
               break;
            default:
               x_offset = programmer_appearance->current_width()/4;
               y_offset = programmer_appearance->current_height()/2;
               break;
         };
         fly_to_goal(programmer_appearance,x_offset,y_offset,callback,time_to_fly_to_programmer);
         flight_taken_care_of = TRUE;
      };
		if (current_house_visible) {
			size_percent = 10;
         millisecond duration = 0;
         if (tt_show_birds_return && !finishing_instantly()) {
            duration = time_to_fly_from_inside;
         };
			add_ease_in_and_out_to_goal(ANIMATING_SIZE_PERCENT,bird_side_view_size_percent,
												 duration);
		} else {
			size_percent = bird_side_view_size_percent;
		};
      set_size_percent(size_percent); // for followers to follow
		size_is_100_percent = FALSE;
      millisecond duration = 0;
      if (tt_show_birds_return && !finishing_instantly()) {
         duration = time_to_fly_from_door;
      };
		if (!flight_taken_care_of) fly_to(goal_x,goal_y,callback,duration,TRUE);
	};
};
			
void Bird::fly_outside_now_full_size() {
	size_is_100_percent = TRUE;
	House *destination_house = pointer_to_destination_house();
	if (destination_house == NULL) {
//		deliver_to_another_house(); // really ok?
		if (item_to_deliver != NULL) { // new on 090203 - not sure how this happens
			if (item_to_deliver->pointer_to_leader() == this) {
				remove_follower(item_to_deliver);
			};
			if (delivery_status == HEADED_TO_NEST && item_to_deliver != NULL) {
				nest->accept_contents(TRUE,item_to_deliver);
				set_item_to_deliver(NULL);
			} else if (floor != NULL) {
				floor->add_item(item_to_deliver); // new on 090105
			};
		};
      if (nest->deleted() || no_home_to_return_to) {
         // house probably blown up
		   tt_screen->remove(this);
		   tt_city->remove_item(this);
         action_completed(); // moved this before destroy() on 191102 just to be extra safe
         destroy(); // could be more dramatic
      } else {
         // nest is probably in transit
         delivery_status = DELIVERY_COMPLETE_AND_RETURNING;
			really_go_back(TRUE);
//         completed(action_status);
      };
		return;
	};
	city_coordinate destination_llx, destination_lly;
	destination_house->exit_point(destination_llx,destination_lly);
	destination_llx -= tt_block_width/20; // somehow this fixes a geometry bug...
   millisecond duration = 0;
   if ((delivery_status != DELIVERY_COMPLETE_AND_RETURNING ||
        tt_show_birds_return) &&
       !finishing_instantly()) {
      duration = time_to_fly_one_block;
   };
	fly_to(destination_llx,destination_lly,FLY_DOWN_CALLBACK,duration,TRUE);
};

void Bird::fly_down() {
	AnimationCallBack callback;
	if (delivery_status == HEADED_TO_NEST) {
		callback = DELIVER_TO_ANOTHER_HOUSE_CALLBACK;
	} else {
		callback = RETURNED_TO_HOUSE_CALLBACK;
	};
	set_parameter(NORTH); // looks best
	set_selectable(FALSE);
	size_is_100_percent = FALSE; // or will be very soon
   millisecond duration = 0;
   if ((delivery_status != DELIVERY_COMPLETE_AND_RETURNING ||
        tt_show_birds_return) &&
       !finishing_instantly()) {
      duration = bird_shrink_full_size_duration;
   };
	add_ease_in_and_out_to_goal(ANIMATING_SIZE_PERCENT,10,duration,this,callback);
};

House *Bird::pointer_to_destination_house() {
	Background *floor_of_destination = NULL;
	if (delivery_status == HEADED_TO_NEST) {
		floor_of_destination = nest->ultimate_container()->pointer_to_background();
	} else {
      if (return_floor != NULL) {
         if (return_floor->deletion_pending()) {
            if (return_floor->decrement_ref_count()) { // last one
               return_floor->destroy();
            };
            return_floor = NULL;
            return(NULL);
         } else {
		      floor_of_destination = return_floor;
         };
      };
	};
	if (floor_of_destination == NULL) {
		return(NULL);
	};
	return(floor_of_destination->pointer_to_house());
};

millisecond Bird::outside_flight_duration(city_coordinate destination_llx,
														city_coordinate destination_lly,
														millisecond minimum_duration) {
	if (finishing_instantly() ||
       (delivery_status == DELIVERY_COMPLETE_AND_RETURNING &&
        !tt_show_birds_return)) {
		return(0);
	} else {
		city_coordinate distance =
			 max((city_coordinate) labs(destination_llx-llx),
				  (city_coordinate) labs(destination_lly-lly));
		return(max(minimum_duration,(distance*time_to_fly_one_block)/tt_block_width));
	};
};

void Bird::shrink_then_deliver_to_another_house() {
	set_parameter(NORTH);
	set_selectable(FALSE);
	if (item_to_deliver != NULL) {
		remove_follower(item_to_deliver);
		item_to_deliver->set_selectable(TRUE);
		tt_screen->remove(item_to_deliver);
	};
	// added tt_global_picture-> on 030903 since if programmer just entered house this bird is still outside and won't run
	tt_global_picture->add_ease_in_and_out_to_goal(ANIMATING_SIZE_PERCENT,10,
																  finishing_instantly()?0:time_to_fly_from_inside,
																  this,DELIVER_TO_ANOTHER_HOUSE_CALLBACK);
};

void Bird::deliver_to_another_house() { // without flying inside
   // nest shouldn't be NULL if this called
	Background *new_floor = pointer_to_nests_background();
   millisecond duration = 0;
   if (new_floor != NULL) duration = new_floor->default_duration();
   if (flying_outside()) {
#if TT_DEBUG_ON
      if (floor != tt_city && floor != NULL && floor->pointer_to_picture() == NULL) { 
         say_error(_T("Somehow bird is outside but doesn't know it."));
      };
#endif
		if (pointer_to_destination_house() == NULL || duration == 0) { // or not being seen anyway 
		   // nest is probably in transit
         // for now just deliver instantly...
         deliver_instantly();
         return;
//    } else {
//		   fly_outside();
      };
	};
	size_percent = 100;
	size_is_100_percent = TRUE;
	if (delivery_status == HEADED_TO_NEST) {
		remove_from_floor(FALSE);
//		item_to_deliver->remove_from_floor(FALSE);
//		if (floor->has_item(item_to_deliver)) {
//			floor->remove_item(item_to_deliver,FALSE);
//		};
//		recursively_remove_leaders(floor);
		if (new_floor == NULL) {
			say(IDS_BIRD_CANT_FIND_NEST,4);
#if TT_DEBUG_ON
			debug_this();
#endif
         return;
		};
//		recursively_set_background(new_floor);
		// the house may be ok to skip if bird flies out a "back" window
		// skip over flying out of room
		// skip over flying out of house from side
		// skip over flying into house (through window?)
		// skip over flying in room to floor
//		recursively_restore_leaders(new_floor);
		new_floor->add_item(this);
		just_fly_to_nest(duration);
	} else { // headed back
//	   remove_follower(item_to_deliver,floor); 
//		set_item_to_deliver(NULL);
		remove_from_floor(FALSE);
      update_return_floor();
		if (return_floor != NULL && floor != return_floor && return_floor != NULL &&
          !return_floor->deletion_pending()) { 
//		Background *new_floor 
//			= return_cubby->pointer_to_background();		
//			set_background(return_floor);
			return_floor->add_item(this);
		};
//		if (delivery_status != DELIVERY_COMPLETE_AND_RETURNING) {
			// conditional prevents infinite recursion but is this right??
			// where should bird go?
			fly_back();
//		};
	};
};

void Bird::deliver_instantly() {
   if (item_to_deliver->pointer_to_leader() == this) {
      remove_follower(item_to_deliver);
   };
   nest->accept_contents(TRUE,item_to_deliver);
   set_item_to_deliver(NULL);
  	Background *new_floor = pointer_to_nests_background();
   update_return_floor();
   if (return_floor == floor && floor == new_floor) { // in same room
      really_go_back(TRUE);
      return;
   };
   if (new_floor != NULL) {
      remove_from_floor(FALSE);
      new_floor->add_item(this);
      // will leave very soon but this way computes how and whether to
      // show return flight better
   };
   fly_back();
};

void Bird::release() {
	// hovering over nest, releases message or stack here
	// what if there is a problem?? -- check the status returned??
	switch (delivery_status) {
		case HEADED_BACK_TO_NEST_WITH_STACK:
			remove_follower(top_of_stack_on_nest);
			if (floor != NULL) {
				floor->add_item(top_of_stack_on_nest); // new on 090105
			};
//			top_of_stack_on_nest = NULL; // new on 121202 so XML for cities doesn't get mixed up
			set_top_of_stack_on_nest(NULL); // new on 120203 both for the reason above and for ref counting
//         top_of_stack_on_nest->set_selectable(TRUE);
			nest->stack_restored();
			fly_back();
			break;
		case HEADED_BACK_TO_NEST: {
			// still have to pick up the stack and put it back on top
         millisecond duration = nest->default_duration(500);
         city_coordinate stack_x,stack_y;
			top_of_stack_on_nest->center_location(stack_x,stack_y); // was lower_left_corner
         if (item_to_deliver != NULL) { // conditional added for robustness on 151102
			   remove_follower(item_to_deliver);
//		      item_to_deliver->set_selectable(TRUE);
//			   item_to_deliver->update_display();
				if (showing_on_a_screen()) {
					tt_screen->add(item_to_deliver);
				};
				// swapped the order of the above and below on 070604 since if F7 then duration is 0 and the above undoes what the following does
				nest->add_item(item_to_deliver,duration,this);
			   set_item_to_deliver(NULL);
#if TT_DEBUG_ON
         } else {
            log("Bird has nothing to release!");
#endif
         };
			if (duration > 0) {
				play_sound(BIRD_FLY_SOUND);
			};
//			animate_location(stack_llx,stack_lly,
//								  duration,NULL,this,put_stack_back_callback);
#if TT_DEBUG_ON
			if (tt_debug_mode == 170703) {
				tt_error_file() << "Bird#" << debug_counter << " scheduling PUT_STACK_BACK_CALLBACK in " << duration << "ms. On frame " 
									 << tt_frame_number << endl;
			};
#endif
			fly_to(stack_x,stack_y,PUT_STACK_BACK_CALLBACK,duration);
                                };
			break;
		case HEADED_TO_NEST_ON_FLIPSIDE: {
			millisecond duration = default_duration();
         if (item_to_deliver != NULL) { // conditional added for robustness (Gordon's crash report of 151102)
			   remove_follower(item_to_deliver);
		      item_to_deliver->set_selectable(TRUE);
//			   floor->remove_item(item_to_deliver);
			   nest->accept_contents(TRUE,item_to_deliver);
			   set_item_to_deliver(NULL);
#if TT_DEBUG_ON
         } else {
            log("Bird has nothing to release!");
#endif
         };
//			duration += duration/2; // wait a bit before leaving
			do_after(duration/2,this,ANIMATE_TO_GOOD_SIZE_CALLBACK);
			do_after(duration,this,FLY_BACK_CALLBACK);
                                       };
			break;
		default: // ordinary release
         if (item_to_deliver != NULL) { // conditional added for robustness on 151102
//			   item_to_deliver->update_display();
				if (item_to_deliver->pointer_to_leader() == this) {
				   remove_follower(item_to_deliver);
					floor->add_item(item_to_deliver); // new on 090105 since sprite::remove_follower removes from floor now
				   item_to_deliver->set_selectable(TRUE);
			   };
				if (item_to_deliver->pointer_to_background() != floor) {
               item_to_deliver->remove_from_floor(FALSE); // needed??
               floor->add_item(item_to_deliver);
            };
			   nest->add_item(item_to_deliver,nest->default_duration(),this);
			   set_item_to_deliver(NULL);
#if TT_DEBUG_ON
         } else {
            log("Bird has nothing to release!");
#endif
         };
			fly_back();
	};
};

void Bird::put_stack_back() {
	Sprite *top_of_stack_leader = top_of_stack_on_nest->pointer_to_leader();
	if (top_of_stack_leader == nest) { 
		// new on 170703 since is not a problem since Nest::pointer_to_contents probably did it
		nest->remove_follower(top_of_stack_on_nest);
		top_of_stack_leader = NULL; // new on 040205
	} else if (top_of_stack_leader != this && top_of_stack_leader != NULL && top_of_stack_leader->kind_of() != CUBBY) { 
		// new on 100803 in case robot has grabbed it then wait -- 
		// added cubby type test on 200405 since can be that leader is a cubby and it has been flown somewhere
		if (nest->pointer_to_leader() == NULL && nest->pointer_to_background() == NULL) { // new on 200405
			// nest is in limbo -- probably part of something that was destroyed -- possible leak??
			set_top_of_stack_on_nest(NULL);
			fly_back();
			return;
		};
		do_after(1000,this,PUT_STACK_BACK_CALLBACK);
		return;
	};
	//nest->about_to_put_stack_back(); // new on 170703
	//if (top_of_stack_on_nest->pointer_to_leader() == nest) { // new branch on 170703
//		// another bird has already returned the stack so skip that
//#if TT_DEBUG_ON
//		if (tt_debug_mode == 170703) {
//			tt_error_file() << "Bird#" << debug_counter << " was going to put the stack back on the nest but it seems it already is. ";
//			top_of_stack_on_nest->print(tt_error_file());
//			tt_error_file() << endl;
//		};
//#endif
//		set_top_of_stack_on_nest(NULL);
//		fly_back();
//	} else {
//#if TT_DEBUG_ON // handled above as of 100803
//	if (top_of_stack_on_nest->pointer_to_leader() != NULL && 
//		top_of_stack_on_nest->pointer_to_leader() != this) { // second conjunct added 121202
//		say_error(_T("Bird about to pick up something held by another."));
//	};
//#endif
	if (top_of_stack_leader != nest) { // condition new on 090105 since otherwise was done above
		top_of_stack_on_nest->remove_from_floor(FALSE); // had been commented out on 070105 since sprite::remove_follower now deals with this
	};
	if (top_of_stack_leader != this) { // may already know about it -- new on 040205
		if (top_of_stack_leader != NULL) { // can this happen
			top_of_stack_leader->remove_follower(top_of_stack_on_nest);
		};
		add_follower(top_of_stack_on_nest,FALSE);
	};
	if (showing_on_a_screen()) tt_screen->add(top_of_stack_on_nest);
	top_of_stack_on_nest->set_selectable(FALSE);
	delivery_status = HEADED_BACK_TO_NEST_WITH_STACK;
	just_fly_to_nest(default_duration());
};

void Bird::fly_back() {
	delivery_status = DELIVERY_COMPLETE_AND_RETURNING;
	millisecond duration = 0;
   if (tt_show_birds_return) duration = default_duration();
	if (no_home_to_return_to) {
		if (duration > 0) {
			play_sound(BIRD_FLY_SOUND);
			fly_to(-5*tile_width,7*tile_height,DESTROY_BIRD_CALLBACK);
		} else {
			destroy_bird();
		};
	} else if (flying_outside()) {
		fly_outside();
	} else {
		really_go_back();
	};
};

void Bird::shrink_then_returned_to_house() {
	set_parameter(NORTH);
	set_selectable(FALSE);
	if (item_to_deliver != NULL) {
		remove_follower(item_to_deliver);
		item_to_deliver->set_selectable(TRUE);
		tt_screen->remove(item_to_deliver);
	};
	add_ease_in_and_out_to_goal(ANIMATING_SIZE_PERCENT,10,
										 finishing_instantly()?0:time_to_fly_from_inside,
										 this,RETURNED_TO_HOUSE_CALLBACK);
};

void Bird::returned_to_house(boolean instantly) {
#if TT_DEBUG_ON
   if (item_to_deliver != NULL) {
      say_error(_T("Whoops. Bird going back but hasn't delivered something."));
   };
#endif
	size_percent = 100;
	size_is_100_percent = TRUE;
   remove_from_floor(FALSE);
   update_return_floor();
	if (return_floor != NULL && !return_floor->deletion_pending()) return_floor->add_item(this);
	really_go_back(instantly);
};

void Bird::really_go_back(boolean instantly) {
	millisecond duration;
   if (instantly) {
      duration = 0;
   } else {
      duration = default_duration();
   };
	Picture *return_floor_picture = NULL;
	Background *return_floor_picture_floor = NULL;
   update_return_floor();
	if (return_floor != NULL && !return_floor->deletion_pending()) {
		return_floor_picture = return_floor->pointer_to_picture();
		if (return_floor_picture != NULL) {
			return_floor_picture_floor = return_floor_picture->ultimate_flipside_picture()->pointer_to_background(); 
			// ultimate_flipside_picture() added on 250399
		};
	};
	if (duration > 0 && floor != return_floor && floor != return_floor_picture_floor
       && return_floor != NULL && !return_floor->deletion_pending()) {
		// fly away from nest floor
		play_sound(BIRD_FLY_SOUND);
		fly_to(-5*tile_width,7*tile_height,FLY_TO_ANOTHER_HOUSE_CALLBACK);
	} else if (no_home_to_return_to) {
		// new on 101204 since for example if you stand up while holding a nest while long-distance birds are trying
		// to deliver they need to fly away
		fly_back();
	} else if (return_cubby == NULL || return_cubby->inside_vacuum()) {
		if (return_floor != floor && return_floor != NULL && !return_floor->deletion_pending()) {
			remove_from_floor(FALSE);
			return_floor->add_item(this);
			move_to(-5*tile_width,7*tile_height);
		};
		if (duration > 0) {
			play_sound(BIRD_FLY_SOUND);
		};
		AnimationCallBack callback;
		if (item_to_deliver == NULL) {
			callback = RETURN_TO_FLOOR_CALLBACK; // new on 151003 so if robots are doing this they notice the action has completed
		} else {
			callback = RELEASE_BAD_MESSAGE_CALLBACK;
		};
		fly_to(return_x,return_y,callback);
	} else if (return_floor_picture != NULL && return_cubby->pointer_to_background() != NULL &&
				  return_cubby->pointer_to_background()->pointer_to_picture() == return_floor_picture) {
		city_coordinate x,y;
		return_floor_picture->center_location(x,y);
		set_priority(bird_flying_priority); //floor->next_tool_priority());
		if (duration > 0)  {
			play_sound(BIRD_FLY_SOUND);
			duration = fly_to(x,y,RETURN_TO_CUBBY_ON_FLIPSIDE_CALLBACK);
			animate_to_good_size(duration/2,tt_log_version_number >= 64?tt_toolbox:NULL);
			do_after(duration/2,this,SHRINK_TO_TINY_CALLBACK);
		} else {
			move_to(x,y);
			return_to_cubby_on_flipside();
		};
	} else if (return_cubby != NULL) {
		// restore this kind of tracking once I allow offsets
		//   city_coordinate *nest_llx, *nest_lly;
		//   nest->pointers_to_lower_left_corner(nest_llx,nest_lly);
		city_coordinate cubby_x, cubby_y;
		// should really figure out location of center of hole of return_path
		return_cubby->center_location(cubby_x,cubby_y);
		set_priority(bird_flying_priority); //floor->next_tool_priority());
//		Background *return_floor = return_cubby->pointer_to_background();
		if (floor != return_floor && return_floor != NULL && !return_floor->deletion_pending()) {
			// added && !return_floor->deletion_pending() on 011204
			remove_from_floor(FALSE);
			return_floor->add_item(this);
			move_to(-5*tile_width,7*tile_height);
		};
//		move_to(cubby_x,cubby_y);
		duration = default_duration(); // if its now visible
		if (duration > 0)  {
			play_sound(BIRD_FLY_SOUND);
			fly_to(cubby_x,cubby_y,RETURN_TO_CUBBY_CALLBACK);
		} else {
			return_to_cubby();
//			move_to(cubby_x,cubby_y); // commented out on 230205
         set_return_floor(NULL);
		};
	} else { // if not handled above just fly off -- but end up at destination
		if (duration > 0) {
			play_sound(BIRD_FLY_SOUND);
		};
		fly_to(-5*tile_width,7*tile_height,FLY_TO_ANOTHER_HOUSE_CALLBACK);
	};
};

/*
void Bird::fly_back_to_other_house() {
	// am offscreen so for now just teleport to other place
	if (return_floor != NULL) {
		set_floor(return_floor);
		fly_back();
	};
};
		
void fly_back_to_other_house_callback(void *sprite) {
	((Bird *) sprite)->fly_back_to_other_house();
};
*/

void Bird::return_to_floor() { // new on 151003
	set_parameter(BIRD_FLY_DOWN);
	set_selectable(FALSE); // why?
   set_cycle_stopped_callback(BIRD_MORPH_TO_INERT_AND_FIX_DRIFT_CALLBACK); // BIRD_MORPH_TO_INERT_CALLBACK better? asked 140504
   action_completed(); // this is the important difference with release_bad_message when item_to_deliver == NULL
};

void Bird::release_bad_message() { // boolean last_action) {
	millisecond duration = default_duration();
	if (item_to_deliver == NULL) {
		set_parameter(BIRD_FLY_DOWN);
	   set_selectable(FALSE);
//      if (tt_log_version_number > 5) {  // otherwise bird drifts
		   set_cycle_stopped_callback(BIRD_MORPH_TO_INERT_CALLBACK); // was BIRD_MORPH_TO_INERT_AND_FIX_DRIFT_CALLBACK prior to 140504
//      } else { // not as good a way of fixing drift
//         set_cycle_stopped_callback(bird_morph_to_inert_callback);
//         animate_location(return_x,return_y,duration==0?0:total_cycle_duration());
//      };
//		set_parameter(BIRD_MORPH_TO_INERT);
		// fixes a problem due to widely different size causing drift
//		set_cycle_stopped_callback(move_to_return_x_y_callback);
		return;
	};
	// moved the following here on 150503 since it shouldn't be called if there is no item_to_deliver - e.g. after giving a bird something in append then removing the bird
// if (last_action) { // commented out on 021202
      action_completed();
// };
	remove_follower(item_to_deliver);
	item_to_deliver->set_selectable(TRUE);
	if (floor != NULL) floor->add_item(item_to_deliver); // restored on 090105 (don't know when it had been commented out)
//	item_to_deliver->update_display();
	item_to_deliver->animate_to_good_size(duration,tt_log_version_number >= 64?tt_toolbox:NULL);
	set_item_to_deliver(NULL);
	if (duration > 0) show_confusion();
};

void Bird::show_confusion() {
	set_parameter(BIRD_CONFUSED);
	set_cycle_stopped_callback(BIRD_MORPH_TO_INERT_CALLBACK);
};

void Bird::show_anticipation() {
	morph_to_life();
	set_cycle_stopped_callback(BIRD_GIVE_ME_CALLBACK);
};

void Bird::return_to_cubby() {
//   tt_error_file() << "Return to cubby " << this << " frame=" << tt_frame_number << endl;
//	release_bad_message(); // FALSE); // moved to the appropriate branches below on 210104
	if (return_path == NULL) { // added for robustness on 110203 - not clear how it happens
		action_aborted();
		release_bad_message(); // copied here on 210104
		return;
	};
	millisecond duration = default_duration();
	long last_index;
	Sprite *container = NULL;
	return_path->dereference_cubby(return_cubby,container,last_index,this);
	Cubby *embedded_cubby = (Cubby *) container;
   if (embedded_cubby->inside_vacuum()) {
		return_cubby->destroy(); // if ref count is zero
		return_cubby = NULL; // done with it
		action_completed(); // new on 041202
		release_bad_message(); // copied here on 210104
		return; // anything else??
	};
	if (embedded_cubby->current_number_of_holes() < last_index) {
		// there aren't enough holes anymore -- new on 011204
		action_aborted();
		release_bad_message(); // copied here on 210104
		return;
	};
	Sprite *home = embedded_cubby->component(last_index); // removed (cubby_index) coerecion on 070203
	if (home == NULL && // still empty
       !embedded_cubby->suck_up_in_progress()) {
      remove_from_floor(FALSE,TRUE,FALSE);
		set_parameter(BIRD_INERT); // new on 230205 so the following has the geometry for the bird when inert
		embedded_cubby->hole_receives_item(this,last_index,this,duration,FALSE); // removed (cubby_index) coerecion on 070203 
		morph_to_inert();
		if (duration > 0) play_sound(BIRD_FLY_SOUND);
      return_cubby->destroy(); // if ref count is zero
      return_cubby = NULL; // done with it
		delivery_status = DELIVERY_COMPLETE_AND_RETURNING; // new on 150104 -- well, really have already returned NO_DELIVERY should work too
		do_after(duration,this,ACTION_COMPLETED_CALLBACK); // this replaced the following on 150503 since the action isn't done yet
		release_bad_message(); // copied here on 210104
//		action_completed(); // new on 041202
	} else {
//      if (tt_log_version_number >= 7) {
         city_coordinate x,y;
         embedded_cubby->lower_left_corner(x,y);
         if (llx <= x) {
            x += embedded_cubby->current_width();
            y += embedded_cubby->current_height();
         };
			fly_to(x,y,RETURN_TO_CUBBY_CALLBACK,1000); // 1000 added when TT_ALPHA was added
//      } else {
//         fly_to(llx+my_random(tile_width),lly+my_random(tile_height),
//                return_to_cubby_callback);
//      };
//		say(S(IDS_BIRD_RETURNED_TO_OCCUPIED_CUBBY),4);
//		set_no_home_to_return_to(TRUE);
//		fly_back();
	};
	if (waiting_for != NULL) {
		do_after(duration,this,NOTE_NEST_BUILDING_FINISHED_CALLBACK);
	};
};

void Bird::return_to_cubby_on_flipside() {
	remove_from_floor(FALSE);
//	set_background(return_floor);
	if (return_floor != NULL && !return_floor->deletion_pending()) {
		// added && !return_floor->deletion_pending() on 011204
		return_floor->add_item(this);
	};
	long last_index;
	Sprite *container = NULL;
	if (return_path != NULL) { // condition new on 230103 - can happen when loaded from XML but where does the bird end up?
		// note it is too late to compute the return_path since don't know where in the cubby the bird came from
		return_path->dereference_cubby(return_cubby,container,last_index,this);
	};
	Cubby *embedded_cubby = (Cubby *) container;
	if (embedded_cubby != NULL && embedded_cubby->component(last_index) == NULL) { // still empty // removed (cubby_index) coerecion on 070203
		// schedule this for 10 second from now so other stuff runs first
		morph_to_inert();
		embedded_cubby->hole_receives_item(this,last_index,this,10000,FALSE); // removed (cubby_index) coerecion on 070203
		finish_instantly(TRUE); // if animating since now on flip side
	} else {
		finish_instantly(FALSE);
	};
	set_return_cubby(NULL); // new on 150405 to fix a leak since cubby may not be collected otherwise
	action_completed(); // new on 041202
};

void Bird::note_nest_building_finished() {
	waiting_for->decrement_busy_counter();
	waiting_for = NULL;
};
	
void Bird::destroy_bird() {
//	floor->remove_item(this,FALSE);//,TRUE); // delete it too
	remove_from_floor(FALSE);
	action_completed(); // moved here on 060705 since was after destroy and that can lose the saved_by setting
	destroy(saved_by);
//	completed(action_status); 
};

void Bird::fix_drift() {
   if (leader != NULL) return;
   // drift is caused by bird cycles having very different bounding boxes...
   if (showing_on_a_screen()) {
		animate_location(return_x,return_y,total_cycle_duration());
   } else {
      move_to(return_x,return_y);
   };
};

void Bird::morph_to_life() {
//   if (t_shirt != NULL && (current_parameter() == BIRD_INERT || current_parameter() == BIRD_MORPH_TO_INERT)) {
//      // counter a drift downwards
//      city_coordinate corner_offset_x = t_shirt->current_llx()-llx;
//      city_coordinate corner_offset_y = t_shirt->current_lly()-lly;
//      t_shirt->animate_location(&llx,&lly,total_cycle_duration(),NULL,NULL,corner_offset_x,corner_offset_y+height/20);
//   };
	set_parameter(BIRD_MORPH_TO_LIFE);
};

void Bird::morph_to_inert() {
//   if (t_shirt != NULL && current_parameter() != BIRD_INERT && current_parameter() != BIRD_MORPH_TO_INERT) {
//      // counter a drift upwards
//      city_coordinate corner_offset_x = t_shirt->current_llx()-llx;
//      city_coordinate corner_offset_y = t_shirt->current_lly()-lly;
//      t_shirt->animate_location(&llx,&lly,total_cycle_duration(),this,fix_t_shirt_callback,corner_offset_x,corner_offset_y-height/20);
//   };
	set_parameter(BIRD_MORPH_TO_INERT);
   set_selectable(TRUE);
	if (leader == NULL && floor != NULL) now_on_floor(floor,this); // at least to get right priority
   set_containable_by_background(TRUE); // can't pass thru wall until really flying away
};

void Bird::adjust_size_and_location(city_coordinate &item_width,
												city_coordinate &item_height,
												city_coordinate &item_llx,
												city_coordinate &) {
	// adjust height so true height is item_height
	scale_to_fit(); // to be sure that true_height is right
	city_coordinate real_height = true_height();
	if (real_height == 0) return;
	city_coordinate real_width = true_width();
	if (real_width == 0) return;
	// compute height for the current image so it is as tall as expected
	city_coordinate new_item_height = (item_height*height)/real_height;
	city_coordinate new_item_width = width_given_height(new_item_height);
	if (new_item_width < item_width) {
		item_llx += (item_width-(new_item_width*real_width)/width)/2;
//		item_llx += (item_width-new_item_width)/2;
		item_width = new_item_width;
	} else {
		item_width = (item_width*width)/real_width;
	};
	item_height = new_item_height;
};

void Bird::good_size(city_coordinate &w, city_coordinate &h, Sprite *source, boolean for_expander) {
   if (favorite_width <= 0 || for_expander) { // added for_expander on 130904 so that can make it go back to a good (non-favorite) size
      Sprite::good_size(w,h,source,for_expander);
   } else {
      w = favorite_width;
      h = favorite_height;
      if (floor != NULL && floor->in_the_room_with_programmer()) { 
         tt_programmer->translate_floor_to_room_dimensions(w,h);
      };
   };
};

void Bird::set_size_and_location(city_coordinate width,
											city_coordinate height,
											city_coordinate llx,
											city_coordinate lly,
											ChangeReason reason) {
	Sprite::set_size_and_location(width,height,llx,lly,reason);
   if (reason == EXPANDER_CHANGED_IT) set_favorite_size(); //  && tt_log_version_number > 9
};

Sprite *Bird::respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected, Sprite *by,
										    boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless) {
	if (t_shirt == NULL) {
		if (!rubout(key)) {
//			string name = new character[2]; 
//			name[0] = key;
//			name[1] = NULL; // terminate
//			add_t_shirt(variable_width_text_pad(name,FALSE,tt_black));
			wide_string name = update_string((wide_string) NULL,key,extended); // on 240800 replaced the above with this
			if (name != NULL) { // condition new on 110604
				add_t_shirt(variable_width_text_pad(NULL,FALSE,tt_black));
				((Text *) t_shirt)->set_wide_text(name,wide_string_length(name),FALSE);
			};
		};
	} else if (t_shirt->looks_like_is_of_type(TEXT)) { // updated for generality and simplicity
		// prior to 110604 was t_shirt->kind_of() == TEXT || t_shirt->kind_of() == VARIABLE_WIDTH_TEXT) {
		if (by->can_send_keyboard_editing_keys()) { // new on 110604
			// replaced t_shirt with original_recipient on 030105 to enabled robots to be trained to this
			t_shirt->respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,size_constrained_regardless);
			fix_t_shirt(); // new on 120604 to improve the geometry
			return(this);
		};
		Text *text_t_shirt = (Text *) t_shirt;
		wide_string text = text_t_shirt->copy_text_string();
		wide_string new_text = update_string(text,key,extended);
		boolean just_modified = (text == new_text);
		if (new_text == NULL || new_text[0] == NULL) {
			remove_follower(t_shirt);
			t_shirt->destroy();
			t_shirt = NULL;
		} else {
			text_t_shirt->set_wide_text(new_text,wide_string_length(new_text),just_modified);
		};
   };
//   completed(the_action_status);
   return(Sprite::respond_to_keyboard(key,extended,robot_selected,by,record_action,original_recipient,size_constrained_regardless));
};

ArrowKeyEditing Bird::arrow_key_editing_status() { // new on 120604
	if (t_shirt != NULL) {
		return(t_shirt->arrow_key_editing_status());
	} else { 
		return(Sprite::arrow_key_editing_status());
	};
};

void Bird::set_item_to_deliver(Sprite *item) { 
	// new on 170799 so if item is destroyed ref counts save things
	if (item_to_deliver != NULL) { // && item != NULL // removed this on 261003 since otherwise items can end up on the nest that are not selectable
#if TT_DEBUG_ON
		if (tt_debug_mode == 170703 || debug_counter == tt_debug_target) {
			tt_error_file() << "Bird#" << debug_counter << " forgetting to deliver ";
			item_to_deliver->print(tt_error_file());
			tt_error_file() << endl;
		};
#endif
		item_to_deliver->set_selectable(TRUE); // if not destroyed below
      if (item_to_deliver != this) { // conditional needed due to strange code in PrimitiveBird::prepare_for_deletion - rationalize?? (new on 140802)
         item_to_deliver->destroy(this); // decrement ref count
      };
	};
	if (item != NULL) {
		item->increment_ref_count(this);
		item->set_selectable(FALSE);
	};
#if TT_DEBUG_ON
	if (!about_to_quit() && (tt_debug_mode == 170703 || (tt_debug_mode == 150405 && debug_counter == tt_debug_target))) {
		if (item == NULL) {
//			debug_this();
			tt_error_file() << "Bird#" << debug_counter << " no longer has anything to deliver" << endl;
		} else {
			tt_error_file() << "Bird#" << debug_counter << " is getting item to deliver ";
			item->print(tt_error_file());
			tt_error_file() << endl;
		};
	};
#endif
	item_to_deliver = item;
};

void Bird::update_pointer_to_nest() {
//	if (nest == NULL) return; // moved into while loop for safety on 130504
//	if (!tt_old_nest_merging_behavior) {
//		nest = nest->ultimate_forwarding_to_or_original_nest();
//	} else {
		while (nest != NULL && nest->forwarding()) { // must be old behavior or loaded something in the middle of old nest forwarding behavior
			nest = nest->pointer_to_original_nest();
		};
//	};
};

Sprite *Bird::now_inside_thought_bubble(boolean in_training) { 
	// new on 101199 so labels don't show when in thought bubble
	if (in_training) return(this);
	if (nest != NULL) { // for robustness - new on 151003
		nest->decrement_bird_count(); // new on 240903 since this is very wasteful and can cause problems when loading XML
		nest = NULL;
	};
	set_guid(NULL); // new on 151003 to sever any connections
	become_blank();
	return(this);
};

//void Bird::action_completed() { // new on 021202
	// commented out on 060705 since each copy now causes the robot to wait
//   if (!no_home_to_return_to) { 
//      Sprite::action_completed();
//	}; // else is a copy so no robot is waiting for this
//};

void Bird::shift_viewpoint(city_coordinate delta_x, city_coordinate delta_y) {
	// new on 020204 to update return coordinates when scrolling
	if (return_x != min_long) { // if initialized
		return_x += delta_x;
		return_y += delta_y;
	};
	Sprite::shift_viewpoint(delta_x,delta_y);
};

PrimitiveBird::PrimitiveBird(PrimitiveReceiver original_receiver,
//									  PrimitiveHelper helper,
//									  PrimitiveDumper dumper,
//									  PrimitiveRestorer restorer,
									  HANDLE handle,
//									  ascii_string original_label,
									  PrimitiveBird *original_creator,
									  Sprite *original_message,
									  short int handle_index,
									  unsigned short *original_handle_ref_count,
									  city_coordinate x, city_coordinate y,
									  long priority,
									  city_coordinate width, city_coordinate height,
								     Sprite *t_shirt) :
         receiver(original_receiver),
			handle(handle),
			handle_index(handle_index), // when recreating which handle this one is
			Bird(x,y,priority,NULL,width,height,t_shirt) {
   if (original_receiver == NULL) {
      receiver = receive_extension_name; // new on 130802
   };
   if (t_shirt == NULL && receiver == receive_extension_name) { // new on 140802
//      label = copy_ascii_string("?"); 
//      Picture *t_shirt = t_shirt_label("?");
//		update_display(); // add_t_shirt now does this
      add_t_shirt(t_shirt_label("?"));
   };
	//if (original_label != NULL) {
	//	label = copy_ascii_string(original_label);
	//} else {
	//	label = NULL;
	//};
	if (original_creator != NULL) { // this when it receives the following can reconstruct this bird
		creator = (PrimitiveBird *) original_creator->copy(); // could increase ref_count instead, right? asked 250203
	} else {
		creator = NULL;
	};
   if (original_message != NULL) {
		message = original_message->copy(); // could increase ref_count instead, right? asked 250203
	} else {
		message = NULL;
	};
   if (handle != NULL) {
	   if (original_handle_ref_count == NULL) {  // receiver != receive_extension_name
         handle_ref_count = new unsigned short[1];
		   handle_ref_count[0] = 1;
	   } else {
		   handle_ref_count = original_handle_ref_count;
         if (receiver != receive_extension_name) {
		      handle_ref_count[0]++;
	      };
      };
	};
//	marty_can_talk = (receiver == receive_extension_name || (creator != NULL && creator->return_marty_can_talk()));
//	if (creator != NULL && message == NULL) {
//		tt_error_file() << "Something wrong creating a Primitive Bird." << endl;
//	};
};

//void PrimitiveBird::prepare_for_deletion() {
//	if (preparing_for_deletion) return;
//	preparing_for_deletion = TRUE; // new on 060204
//	Bird::prepare_for_deletion();
//};

void PrimitiveBird::release_all_references() { // new on 130504
	if (releasing_all_references) { // new on 230305
		return;
	};
	if (handle != NULL) { //receiver != receive_extension_name) {
		handle_ref_count[0]--;
		if (handle_ref_count[0] == 0) { // last one
			Sprite *saved_item_to_deliver = item_to_deliver;
		   set_item_to_deliver(this); // to signal that handles should be closed etc.
		   deliver_to_nowhere(0); // won't really return anything but don't want this to be destroyed
         set_item_to_deliver(saved_item_to_deliver);
			delete [] handle_ref_count;
		};
		handle = NULL; // new on 130504
	};
//	if (label != NULL) delete [] label;
	if (creator != NULL) {
		creator->destroy(); //private copy
		creator = NULL; // new on 130504
	};
	if (message != NULL) {
		message->destroy();
		message = NULL; // new on 130504
	};
	Bird::release_all_references();
};

Sprite *PrimitiveBird::copy(boolean also_copy_offsets) {
	// simplified on 150303 (should behave identically)
   PrimitiveBird *copy = new PrimitiveBird(receiver,handle, // label,
		                                     creator,message,handle_index, // copied upon creation
														 handle_ref_count,
		                                     llx,lly,
											          current_priority-1,
											          width,height);
	copy->set_old_x_offset(old_x_offset);
	copy->set_old_y_offset(old_y_offset);
   //if (t_shirt != NULL) { //  commented out on 250203 since bird gets t_shirt above
   //   copy->add_t_shirt(t_shirt->copy(TRUE));
   //};
 //  copy->set_priority_fixed(priority_fixed_flag);
	//if (comment != NULL) {
	//	copy->set_comment(comment->copy());
	//};
   finish_making_copy(copy,also_copy_offsets);
	return(copy);
};

boolean PrimitiveBird::to_windows(Pointer *data, short int &data_offset,
                                  ascii_string types, short int &types_offset,
                                  BirdPointer *, short int &) {
   types[types_offset] = 'H'; // token for handle
   types_offset++;
   data[data_offset] = receiver;
   data_offset++;
   data[data_offset] = (Pointer) return_handle();
   data_offset++;
   data[data_offset] = NULL;   // could be text string of t_shirt ...
   data_offset++;
   return(TRUE);
};

boolean PrimitiveBird::dump(output_stream &stream, boolean just_prepare, boolean , boolean ) {
   if (just_prepare) {  // need to do more? 
      finish_instantly(FALSE,TRUE); // added TRUE on 010399 so that this happens even if power is off
      return(FALSE);
   };
   int dump_index = about_to_dump(this,stream); // rewritten on 020802
	if (dump_index < 0) return(FALSE); // new on 051099
	stream.put((unsigned char) PRIMITIVE_BIRD);
	dump_if_not_null(stream,comment);
	stream.write((character *) &handle_index, sizeof(handle_index));
	dump_if_not_null(stream,creator);
	dump_if_not_null(stream,message);
	dump_if_not_null(stream,t_shirt);
	return(TRUE);
};

#if TT_XML
// new on 130802

void PrimitiveBird::xml(xml_element *bird_element, xml_document *document) {
//	  changed from xml_create_element on 120703
//   update_pointer_to_nest(); // commented out on 120703 since foreign birds don't have a nest
//   xml_element *bird_element = create_xml_element(xml_tag(),document); 
   if (message != NULL) { // needs to be first so creator can use it
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { 
		// new on 100703 -- is this needed? - can it be also on the floor or elsewhere?
		//	message->check_for_independence();
		//};
      UnwindProtect<boolean> set(tt_save_nests_as_local,TRUE); // the bird in the message need not become long-distance	     
		xml_append_child(create_xml_element_shared(L"MessageToMaker",document,message),bird_element); 
		// restored on 250203 since can loop
		// rewritten on 190103 to use top_level since there shouldn't be any sharing
		//xml_element *message_element = create_xml_element(L"MessageToMaker",document);
		//xml_append_child(message->top_level_xml(document),message_element);
		//xml_append_child(message_element,bird_element);
   };
   if (creator != NULL) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { 
		// new on 100703 -- is this needed? - can it be also on the floor or elsewhere?
		//	creator->check_for_independence();
		//};
      xml_element *creator_element = create_xml_element_shared(L"Maker",document,creator);
      xml_set_attribute(creator_element,L"HandleIndex",handle_index);
      xml_append_child(creator_element,bird_element);
   };
   // commented out the following since primitive birds have a label determined by their receiver (or ? if default)
   //if (t_shirt != NULL) {
   //   xml_append_child(create_xml_element(L"Label",document,t_shirt),bird_element);
   //};
//   append_standard_elements(bird_element,document); -- renamed to sprite::xml so will happen anyway (121102)
	Bird::xml(bird_element,document);
};

boolean PrimitiveBird::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
      case MAKER_TAG: {
         PrimitiveBird *replacement = NULL;
         creator = (PrimitiveBird *) xml_get_sprite(node,creator,BIRD); // PRIMITIVE_BIRD - updated on 180103
         if (creator != NULL && message != NULL) {
            replacement = creator->reconstruct(message,xml_get_attribute_int(node,L"HandleIndex",-1)); 
				// message should be created first
//               replacement = creator->reconstruct(message,xml_get_attribute_int(node,L"HandleIndex",-1)); 
				// message should be created first
				if (replacement == NULL) {
					// new on 150303 since at least preserve it so can work later - should it return TRUE or FALSE?
					add_t_shirt(t_shirt_label("?"));
					return(FALSE); 
				};
				creator->destroy(); // restored on 190103 done its job -- this is about to be destroyed along with creator
				creator = NULL; // new on 150303
         };
			if (message != NULL) {
				message->destroy(); // restored on 190103 -- old comment: this is about to be destroyed along with creator
				message = NULL; // new on 150303
			};
         if (replacement != NULL) {
            replacement->transfer_state_to(this);
            replacement->destroy();
         } else {
//            replacement = new PrimitiveBird(receive_extension_name); // just the initial one that opens libraries
            receiver = receive_extension_name;
            if (t_shirt == NULL) {
               add_t_shirt(t_shirt_label("?"));
            };
		   };
         return(TRUE);
                      };
      case MESSAGE_TO_MAKER_TAG:
         message = xml_get_sprite(node,message);
         return(TRUE);
      default:
         return(Bird::handle_xml_tag(tag,node));
   };
};

void PrimitiveBird::transfer_state_to(PrimitiveBird *other) { // new on 061102
   other->set_handle(handle);
   handle = NULL;
   other->set_receiver(receiver);
   receiver = NULL;
   other->set_message(message);
   message = NULL;
   other->set_creator(creator);
   creator = NULL;
   other->set_handle_index(handle_index);
   handle_index = 0;
   other->set_handle_ref_count(handle_ref_count);
   handle_ref_count = NULL;
};

#endif

void PrimitiveBird::generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection) {
	if (receiver == NULL) { // new on 010301
		Bird::generate_help(message,tool_in_hand,selection);
		return;
	};
	if (tt_help_counter == 0 || tt_help_counter == 1) {
		describe_pointing(message,tool_in_hand,selection);
		ascii_string to_say = NULL;
		BOOL ok_to_speak = FALSE; // unless set by receiver     
      string label = NULL; // new on 140802 since no longer store the label
      if (t_shirt != NULL) {
         long label_length;
         t_shirt->current_text(label,label_length);
      };      
		if (receiver(handle,NULL,NULL,label,tt_country_code,NULL,NULL,&to_say,&ok_to_speak,resource_source()) && to_say != NULL) {
			if (!ok_to_speak && limited_text_to_speech()) {
				tt_martian->just_talk_balloons_for(60000L); // one minute should be long enough -- could make it a function of length of to_say
			};
			message << to_say;
			if (receiver == receive_extension_name) {
				delete [] to_say;
			} else {
				GlobalFree((void *) to_say);
			};
		};
	};
};

Sprite *PrimitiveBird::recreate_item(Pointer *data, 
												 short int &data_offset, ascii_string types, short int &types_offset,
												 Sprite *creation_message, short int &handle_index, short int desired_handle_index) {
   // if desired_handle_index < 0 then always creates and returns item
	// otherwise only if == handle_index
	Sprite *new_item = NULL;
   switch (types[types_offset++]) {
      case 'L': // long
         if (desired_handle_index < 0) {
            Number *integer = new NUMBER();
            integer->set_long_value((long) data[data_offset]);
            new_item = integer;
			};
			data_offset++;
			break;
      case 'S': // string
         if (desired_handle_index < 0) {
				Text *new_text = new Text();
				new_text->set_text((string) data[data_offset]); 
				new_item = new_text;
			};
			data_offset++;
			break;
      case 'B': // can't be a new bird
      case '?': // don't know or care
      case '-': // should be NULL
         if (desired_handle_index < 0) {
				new_item = (Sprite *) data[data_offset];
			};
			data_offset++;
			break;
      case 'H': // handle
         {
            PrimitiveReceiver receiver = ((PrimitiveReceiver) data[data_offset++]);
            HANDLE handle = (HANDLE) data[data_offset++];
			   ascii_string label = (ascii_string) data[data_offset++];
				if (desired_handle_index < 0 || handle_index == desired_handle_index) {
               PrimitiveBird *bird = new PrimitiveBird(receiver,handle,this,creation_message,handle_index);
		         if (label != NULL) {
		            Picture *t_shirt = t_shirt_label(a2u(label));
	//	  	         bird->update_display(); // add_t_shirt now does this
                  bird->add_t_shirt(t_shirt);
					};
					new_item = bird;
				};
            if (label != NULL) GlobalFree((void *) label); // was copied up above
			   handle_index++;
            break;
			};
      case '[':
         {
			 int contents_max_size = 1024; // for testing 256; // new on 071003 for RCX usage in Italy to grow contents as needed
          SpritePointer *contents = new SpritePointer[contents_max_size]; // -- was [256]; // must be enough
          int count = 0; // removed (cubby_index) coerecion on 070203
          while (types[types_offset] != ']') {
 				  if (count == contents_max_size) { // filled up - this is new on 071003 -- shouldn't really be needed but why not
					  int new_contents_max_size = contents_max_size*2;
					  SpritePointer *new_contents = new SpritePointer[new_contents_max_size];
					  for (int i = 0; i < count; i++) {
						  new_contents[i] = contents[i];
					  };
					  delete [] contents;
					  contents = new_contents;
					  contents_max_size = new_contents_max_size;
				  };
              contents[count] = recreate_item(data,data_offset,types,types_offset,creation_message,handle_index,desired_handle_index);
              if (desired_handle_index >= 0 && contents[count] != NULL) {
					  new_item = contents[count];
					  break;
				  };
				  count++;
          };
          types_offset++; // skip over the ]
          if (desired_handle_index < 0) {
				 new_item = new Cubby(0,0,0,0,0,count,contents);
			 } else {
				 delete [] contents;
			 };
			 break;
         };
      default:
         tt_error_file() << S(IDS_SOMETHING_WRONG_WITH_TYPE_STRING1) << types << S(IDS_SOMETHING_WRONG_WITH_TYPE_STRING2) << endl;
   };
   return(new_item);
};

Picture *t_shirt_label(string label) {
	Text *t_shirt_text = variable_width_text_pad(label); 
	Picture *t_shirt = make_indirect_picture(t_shirt_text,FALSE); // so show some works -- added FALSE on 210802 so isn't added as a layer to the screen
	t_shirt->set_show_all(FALSE);
	return(t_shirt);
};

void give_bird(void *b, Sprite *item) {
	Bird *bird = (Bird *) b;
	Sprite *leader = bird->pointer_to_leader();
	if (leader != NULL) { // remove since container is going to be destroyed
		leader->remove_follower(bird);
		Background *background = leader->pointer_to_background();
		if (background != NULL) {
			background->add_item(bird);
		}; // else something is wrong?
	};
	bird->set_to_good_size(tt_log_version_number >= 64?tt_toolbox:NULL); // seems to get to a short wide shape ... -- arg new on 141204 (many other places too)
	bird->set_no_home_to_return_to(TRUE); // destroy it afterwards
   bird->receive_new_item(item);
};

PrimitiveBird *PrimitiveBird::reconstruct(Sprite *message, short int desired_handle_index) {
	set_item_to_deliver(message);
	PrimitiveBird *result = (PrimitiveBird *) deliver_to_nowhere(desired_handle_index);
	set_item_to_deliver(NULL);
	return(result);
};

Sprite *PrimitiveBird::deliver_to_nowhere(short int desired_handle_index) {
	// if desired_handle_index is non-negative then this returns that "handle" with that index
	// otherwise it gives things the primitive specifies to the corresponding birds
	if (item_to_deliver == NULL) { // new on 101202 since can happen if incorrect box passed to initial primitive bird
		tt_martian->say(IDS_PRIMITIVE_BIRD_DIDNT_HANDLE,4);
		return(NULL); 
	};
	if (receiver == NULL) { // maybe rejected load of DLL - new on 010301
		return(Bird::deliver_to_nowhere(desired_handle_index));
	};
   Pointer *data = new Pointer[to_windows_data_max_length+3];  // add a bit extra because several items can be added before a check
   ascii_string types = new char[to_windows_types_max_length+3];
//	int birds_size = to_windows_birds_max_length+3;
   BirdPointer *birds = new BirdPointer[to_windows_birds_max_length+3];
   short int data_offset = 0;
   short int types_offset = 0;
   short int bird_count = 0;
	Sprite *new_item = NULL;
   if (item_to_deliver->to_windows(data,data_offset,types,types_offset,birds,bird_count)) {
      types[types_offset] = NULL; // terminate string
      PointerPointer *out;
      ascii_string *out_types;
      if (bird_count > 0) {
         out = new PointerPointer[bird_count];
         out_types = new ascii_string[bird_count];
         for (int i = 0; i < bird_count; i++) {
				out[i] = NULL; // for robustness if receiver doesn't set them (new on 280201)
				out_types[i] = NULL;
			};
      } else {
         out = NULL;
         out_types = NULL;
      };
      ascii_string to_say = NULL;
		BOOL ok_to_speak = FALSE;
		tt_most_recent_primitive_bird = this; // new on 280201
      string label = NULL; // new on 140802 since no longer store the label
      if (t_shirt != NULL) {
         long label_length;
         t_shirt->current_text(label,label_length);
      }; 
      BOOL OK = receiver(handle,data,types,label,tt_country_code,out,out_types,&to_say,&ok_to_speak,resource_source());
		if (tt_most_recent_primitive_bird == NULL) { // new on 010301 - DLL was rejected
			receiver = NULL;
			if (creator != NULL) {
				creator->destroy();
				creator = NULL;
			};
		} else if (to_say != NULL) {
			if (!ok_to_speak && has_initialization_completed() && limited_text_to_speech()) { 
				// if occurs during loading of a city don't mess with Marty
				tt_martian->just_talk_balloons_for(60000L); // one minute should be long enough -- could make it a function of length of to_say
         };
			say(a2u(to_say),4,TRUE); // TRUE added on 180700
		   GlobalFree((void *) to_say);
      };
      if (OK) {
			short int handle_index = 0;
		   for (int i = 0; i < bird_count; i++) {
			   if (out_types[i] != NULL) {
				   short int data_offset = 0;
               short int types_offset = 0;
					new_item = recreate_item(out[i],data_offset,out_types[i],types_offset,item_to_deliver,handle_index,desired_handle_index); 
					if (desired_handle_index < 0) {
                  give_bird(birds[i],new_item);
					} else if (new_item != NULL) {
						break; // out of the for loop
					};						
				};
         };
      };
	   if (out != NULL) {
		   for (int i = 0; i < bird_count; i++) {
             if (out[i] != NULL) GlobalFree((void *) out[i]);
			};
		   delete [] out;
		};
	   if (out_types != NULL) {
         for (int i = 0; i < bird_count; i++) {
            if (out_types[i] != NULL) GlobalFree((void *) out_types[i]);
			};
		   delete [] out_types;
		};
	   delete [] data;
	   delete [] types;
		//for (int i = 0; i < birds_size; i++) {
		//	birds[i] = NULL; // new on 071003 since birds shouldn't be deleted
		//};
	   delete [] birds;
	   if (desired_handle_index < 0) Bird::deliver_to_nowhere(desired_handle_index);
   } else { // encountered a nest
      // wait a second and try again
      // what if bird is destroyed before this works??
		// shouldn't get here if desired_handle_index >= 0
      tt_global_picture->do_after(1000,this,DELIVER_TO_NOWHERE_CALLBACK);
   };
	return(new_item);
};

void PrimitiveBird::java_before(java_stream &out) { // new on 141100
	Text *text = new Text();
	text->set_text("Primitive birds don't work in Java. Sorry.");
	text->java_before(out);
	text->destroy();
};	

void PrimitiveBird::java(java_stream &stream) {
   say(IDS_CANT_MAKE_JAVA_APPLET_OF_PRIMITIVE_BIRD);
	stream << S(IDS_JAVA_TT_DOT_NOTHING);
};

void PrimitiveBird::java_after(java_stream &out, const_java_string path) { // new on 141100
	Text *text = new Text();
	text->set_text("Primitive birds don't work in Java. Sorry.");
	text->java_after(out,path);
	text->destroy();
};	

ascii_string global_copy_ascii_string(const_ascii_string source, int length) {
	// need to alloc this globally since used in context where normally allocated by DLL
   if (source == NULL) return(NULL);
   if (length <= 0) length = strlen(source);
   ascii_string destination = (ascii_string) GlobalAlloc(0,(length+1)*sizeof(character)); 
   memcpy(destination,source,length);
   destination[length] = '\0'; // terminate the string
   return(destination);
};

ascii_string global_append_ascii_strings(const_ascii_string a, const_ascii_string b) {
	// need to alloc this globally since used in context where normally allocated by DLL
//  if (a == NULL) return(b);
//  if (b == NULL) return(a);
  int a_length = strlen(a);
  int b_length = strlen(b);
  ascii_string result = (ascii_string) GlobalAlloc(0,(a_length+b_length+1)*sizeof(character));
  strcpy(result,a);
  strcpy(result+a_length,b);
  return(result);
};

string excluded_dlls = NULL;
string approved_dlls = NULL;

BOOL receive_extension_name(HANDLE , Pointer *data, ascii_string types, ascii_string , ascii_string , 
									 PointerPointer *out, ascii_string *out_types, ascii_string *to_say, BOOL *ok_to_speak, HINSTANCE ) {
	if (types == NULL) { // help wanted
		*to_say = ASC(IDS_DLL_PRIMITIVE_BIRD_HELP);
		*ok_to_speak = TRUE;
		return(TRUE);
	};
//#if TT_32
	if (strcmp(types,AS(IDS_STRING_BIRD)) == 0) { // Library name and bird to receive reply
      FileNameStatus file_status;
		const_ascii_string name = (const_ascii_string) data[0];
		ascii_string library_name = append_ascii_strings(AS(IDS_32BIT_LIBRARY_PREFIX),name);
//		ascii_string dll_file_name = existing_file_name(file_status,library_name,AS(IDS_DLL),NULL,TRUE,FALSE);
		// rewritten on 280201 since this could be a major security link if FileSearchPath worked for DLLs like this
		ascii_string dll_file_name = existing_file_name(file_status,library_name,AS(IDS_DLL),NULL,FALSE,FALSE,FALSE,FALSE,FALSE);
		if (dll_file_name != NULL) {
			if (excluded_dlls != NULL && strstr(excluded_dlls,dll_file_name) != NULL) { // already said no to these (new on 280201)
				tt_most_recent_primitive_bird = NULL; // new on 010301 to signal that DLL was rejected
				return(FALSE);
			};
			if (tt_ask_if_ok_to_load_dll && tt_most_recent_primitive_bird != NULL && // tt_most_recent_primitive_bird should always be bound (new on 280201)
				 (approved_dlls == NULL || strstr(approved_dlls,dll_file_name) == NULL)) {
				Sprite *by = tt_most_recent_primitive_bird->pointer_to_saved_by();
				string message = NULL;
				if (by == NULL) {
					message = S(IDS_LOADING_FOREIGN_BIRD,"ToonTalk is loading a foreign bird that uses an extension to ToonTalk (a DLL)."); 
				} else if (by->kind_of() == ROBOT) {
					message = S(IDS_ROBOT_USING_FOREIGN_BIRD,"A robot is about to use a foreign bird that uses an extension to ToonTalk (a DLL).");
				};
				if (message != NULL) {
					char buffer[1024];
					output_string_stream stream(buffer,1024);
					stream << message << " " << S(IDC_DLL_WARNING,"A DLL from an unknown or untrusted source might do damage and so might a robot from an untrusted source using a trusted DLL."); 
					// added above to string table and reworded slightly on 030304
					stream << " " << S(IDS_OK_TO_LOAD_DLL,"Is it OK to load") << " " << library_name << ".dll?" << S(IDS_OK_TO_LOAD_DLL2,"");
					stream.put('\0');
					if (message_box(MB_YESNO|MB_SYSTEMMODAL,buffer,NULL,has_initialization_completed()) != IDYES) {
						delete [] library_name;
						if (excluded_dlls == NULL) {
							excluded_dlls = dll_file_name; // no need to copy since isn't deleted below
						} else {
							string new_excluded_dlls = append_strings(excluded_dlls,dll_file_name);
							delete [] dll_file_name;
							delete [] excluded_dlls;
							excluded_dlls = new_excluded_dlls;
						};
						tt_most_recent_primitive_bird = NULL; // new on 010301 to signal that DLL was rejected
						return(FALSE);
					} else {
						if (approved_dlls == NULL) {
							approved_dlls = copy_string(dll_file_name);
						} else {
							string new_approved_dlls = append_strings(approved_dlls,dll_file_name);
							delete [] approved_dlls;
							approved_dlls = new_approved_dlls;
						};
					};
				};
			};
		   HINSTANCE library = LoadLibraryG(dll_file_name);
			if (library == NULL) { // couldn't find it - new on 010301
				tt_error_file() << "Couldn't load a primitive bird's DLL: " << dll_file_name << endl;
				tt_most_recent_primitive_bird = NULL; // to signal that DLL was rejected
			} else {
				out_types[0] = global_copy_ascii_string(AS(IDS_HANDLE),3);
				out[0] = (Pointer *) GlobalAlloc(0,6*sizeof(Pointer)); // new Pointer[6];
				out[0][0] = (Pointer) GetProcAddress(library, (LPCSTR) 1);
				out[0][1] = NULL; // handle
				out[0][2] = global_copy_ascii_string(name,0); // a label of new handle
			};
			delete [] library_name;
		   delete [] dll_file_name;
         return(library != NULL); // was TRUE prior to 010301
		} else { // if (stricmp(name,"file") == 0) { // extension name is OK just not installed
			// could have installer set entries of toontalk.ini for all system defined extensions 
			trouble_shoot(TROUBLE_SHOOT_EXTENSION_NOT_INSTALLED,NULL,NULL,TRUE);
//      } else {
//			*to_say = global_append_strings(S(IDS_NO_FILE_FOR_EXTENSION_FILE),library_name);
		};
		delete [] library_name;
		return(FALSE);
   };
   *to_say = global_append_ascii_strings(AS(IDS_PRIMITIVE_BIRD_DIDNT_HANDLE),AS(IDS_DLL_PRIMITIVE_BIRD_HELP));
	*ok_to_speak = TRUE;
//#else
//   *to_say = SC(IDS_NO_EXTENSIONS_IN_16BIT);
//#endif
   return(FALSE);
};

void create_bird_nest_pair(BirdPointer &bird, NestPointer &nest) {
	nest = new Nest();
	bird = new Bird(0,0,0,nest);
};

#if TT_DIRECT_PLAY
void register_networked_nest(Nest *nest) {
	if (insert_at_end_if_not_member(nest,tt_networked_nests)) {
//		nest->increment_ref_count(); // commented out since when nest is prepared for deletion it removes itself from tt_networked_nests 
	};
};

Nest *find_nest_with_same_guid(GUID *nest_guid, Sprites *nests) {
	Sprites *remaining = nests;
	while (remaining != NULL) {
		Nest *nest = (Nest *) remaining->first();
		GUID *other_nest_guid = nest->pointer_to_nest_guid();
		if (other_nest_guid != NULL && IsEqualGUID(*nest_guid,*other_nest_guid)) { // other_nest_guid != NULL test is new on 120203 - Gordon had a crash
			return(nest);
		};
		remaining = remaining->rest();
	};
	return(NULL);
};
#endif


//#if TT_DEBUG_ON
//void Bird::check_integrity() {
//	perform_integrity_checks(nest); // nothing wrong with me but maybe my nest
//};
//#endif

//void Bird::adjust_grasp(city_coordinate &adjustment_x,
//								city_coordinate &adjustment_y) {
//	adjustment_x = 0;
//	adjustment_y = height/4;
//};

/*
inline void Nest::activate_waiting_bird() {
	if (waiting_birds != NULL && contents == NULL) {
		Sprite *waiting;
		waiting_birds = waiting_birds->pop(waiting);
		((Bird *) waiting)->release_and_fly_away();
	};
};

void activate_waiting_bird_callback(void *sprite) {
	((Nest *) sprite)->activate_waiting_bird();
};

boolean Nest::ready_to_deliver(Bird *bird) {
	if (contents == NULL) {
		bird->release_and_fly_away();
		return(TRUE);
	} else if (waiting_birds == NULL) {
		waiting_birds = new Sprites(bird);
		waiting_birds_tail = waiting_birds;
	} else {
		Sprites *new_tail = new Sprites(bird);
		waiting_birds_tail->set_rest(new_tail);
		waiting_birds_tail = new_tail;
	};
	return(FALSE);
};
*/


/*
	if (controlled_sprite != NULL) {
		// place copy on T shirt
//		if (controlled_sprite->kind_of() == PICTURE) {
//			t_shirt = (Sprite *) (((Picture *) controlled_sprite)
//												  ->current_image()->copy(floor));
//		} else {
			// don't really need a copy -- could just have a sprite
			// with same SpriteCode and parameter...
			t_shirt = (Sprite *) (controlled_sprite->copy(floor));
//		};
	} else {
		switch (primitive) {
			case NOT_BUILTIN:
		      // following not too useful or aesthetic
//				t_shirt = new Sprite(NEST_SPRITE,0,0,NONE_GIVEN,NEST_EMPTY);
				t_shirt = NULL;
				break;
			case MATH_WIZARD:
				t_shirt = new Sprite(PICTURES_SPRITE,0,0,NULL,NONE_GIVEN,MATH_EXAMPLE_PICTURE);
				break;
			case MOUSE_HANDLER:
				t_shirt = new Sprite(PICTURES_SPRITE,0,0,NULL,NONE_GIVEN,COMPUTUER_MOUSE_PICTURE);
				break;
			case TIME_WIZARD:
				t_shirt = new Sprite(PICTURES_SPRITE,0,0,NULL,NONE_GIVEN,CLOCK_PICTURE);
				break;
			case SERIAL_PORT_HANDLER:
				t_shirt = NULL; // for now
				break;
		};
	};
*/
