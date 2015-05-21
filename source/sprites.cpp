// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.
 
#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif 
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif 
#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif 
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif   
#if !defined(__TT_CLEANUP_H)   
#include "cleanup.h"
#endif   
#if !defined(__TT_SCREEN_H)   
#include "screen.h"
#endif   
#if !defined(__TT_TOOLS_H)   
#include "tools.h"
#endif   
#if !defined(__TT_HELP_H)   
#include "help.h"
#endif
#if !defined(__TT_BIRD_H)
#include "bird.h"
#endif
#if TT_XML
#if !defined(__TT_XML_H)
#include "xml.h"
#endif
#endif
#if !defined(__TT_SPRITES_H)   
#include "sprites.h"
#endif 

#ifdef _DEBUG // new on 160103 to debug leaks 
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
   
Sprites::~Sprites() {
  // this should NOT normally be called with sprites to be destroyed
  // since this list will end up on tt_deleted_lists due to remove
  // and is deleted here
#if TT_DEBUG_ON
	if (tt_debug_mode == 66 || tt_debug_mode == 6666) {
		if (deleted()) {
			debug_this("Deleting list of sprites twice!");
		} else {
			set_deleted(TRUE);
		};
	} else if (tt_debug_mode == 666666) {
		tt_error_file() << "Deleting Sprites#" << debug_counter << " on frame " << tt_frame_number << endl;
	};
#endif
	// recursive version blew the stack
	Sprites *remaining = next;
	if (sprite != NULL) sprite->destroy();
	while (remaining != NULL) {
		Sprites *new_remaining = remaining->rest();
		if (remaining->first() != NULL) {
			remaining->first()->destroy();
			remaining->set_first(NULL);
		};
		remaining->set_rest(NULL);
		delete remaining;
		remaining = new_remaining;
	};
};

void Sprites::delete_all() {
	if (sprite != NULL) {
#if TT_DEBUG_ON
      if (!sprite->prepared_for_deletion()) {
			debug_this();
         tt_error_file() << "Deleting a " << type_name(sprite) << " who wasn't prepared for deletion first!" << endl;
      };
		if (tt_debug_mode == 100) {
			tt_error_file() << "Deleting a " << type_name(sprite) << "#" << sprite->debug_counter << " " << (long) sprite << endl;
		};
		if (tt_debug_mode == 666) {
			sprite->set_deleted();
		} else // really delete it
#endif
		delete sprite;
		sprite = NULL;
	};
	Sprites *remaining = next;
	while (remaining != NULL) {
		Sprites *new_remaining = remaining->rest();
      Sprite *sprite = remaining->first();
		if (sprite != NULL) {
//tt_error_file() << remaining->first() << endl;
#if TT_DEBUG_ON
         if (!sprite->prepared_for_deletion()) {
				debug_this();
            tt_error_file() << "Deleting a " << type_name(sprite) << " who wasn't prepared for deletion first!" << endl;
         };
			if (tt_debug_mode == 100) {
				tt_error_file() << "Deleting a " << type_name(sprite) << "#" << sprite->debug_counter << " " << (long) sprite << endl;
			};
         if (sprite->deleted()) { // useful while debugging
				say_error("Deleting something twice.");
         } else if (tt_debug_mode == 666) { // re-ordered on 201102 so error above is caught first
			   sprite->set_deleted();
			} else  // really delete it
#endif
				delete sprite;
			remaining->set_first(NULL);
		};
		remaining->set_rest(NULL);
#if TT_DEBUG_ON
		if (tt_debug_mode == 666) {
			if (remaining->deleted()) {
				debug_this("Deleting a list twice");
			} else {
				remaining->set_deleted(TRUE);
			};
		} else
#endif
		delete remaining;
		remaining = new_remaining;
	};
	next = NULL;
};

void destroy_sprites(SpritesPointer &sprites, Sprite *by) {
   if (sprites == NULL) return;
   sprites->destroy_all(by);
   delete sprites;
   sprites = NULL;
};

void just_destroy_list(SpritesPointer &items) {
   if (items != NULL) {
      items->remove_all();
      delete items;
      items = NULL;
   };
};

void Sprites::destroy_all(Sprite *by) {
	Sprites *remaining = this;
	while (remaining != NULL) {
      Sprite *sprite = remaining->first();
      if (sprite != NULL) {
         remaining->set_first(NULL);
         sprite->destroy(by);
      };
      remaining = remaining->rest();
   };
};

void Sprites::decrement_ref_count_of_each() { // new on 230103
	Sprites *remaining = this;
	while (remaining != NULL) {
      Sprite *sprite = remaining->first();
      if (sprite != NULL) {
         sprite->decrement_ref_count();
      };
      remaining = remaining->rest();
   };
};

void Sprites::recursively_activate_pictures(boolean reset_animation, PictureActivateContext context) {
	Sprites *remaining = this;
	while (remaining != NULL) {
      Sprite *sprite = remaining->first();
      if (sprite != NULL) { // && (!pictures_only || sprite->kind_of() == PICTURE)) {
         sprite->recursively_activate_pictures(reset_animation,context);
      };
      remaining = remaining->rest();
   };
};

void Sprites::recursively_propagate_changes(ChangeReason reason) {
	Sprites *remaining = this;
	while (remaining != NULL) {
      Sprite *sprite = remaining->first();
      if (sprite != NULL) {
         sprite->recursively_propagate_changes(reason);
      };
      remaining = remaining->rest();
   };
};

void Sprites::set_background(Background *new_background, boolean recur, boolean warn) { // new on 060204
	Sprites *remaining = this;
	while (remaining != NULL) {
      Sprite *sprite = remaining->first();
      if (sprite != NULL) {
         sprite->set_background(new_background,recur,warn);
      };
      remaining = remaining->rest();
   };
};

boolean Sprites::insert_at_end_if_not_member(Sprite *addition) {
//	if (sprite == addition) {
//		return(FALSE); // already three
//	} else if (next == NULL) {
//		next = new Sprites(addition);
//	} else {
	  Sprites *remaining = this; // was next
	  while (TRUE) {
		 if (remaining->first() == addition) return(FALSE);
		 if (remaining->rest() == NULL) {
			 remaining->set_rest(new Sprites(addition));
			 return(TRUE);
		 };
		 remaining = remaining->rest();
	  };
//	};
};

boolean Sprites::insert_at_end_if_not_member_or_same_nest_guid(Sprite *nest) {
	Sprites *remaining = this; // was next
	Nest *addition = (Nest *) nest;
	GUID *addition_guid = addition->pointer_to_nest_guid();
	while (TRUE) {
		Nest *next_nest = (Nest *) (remaining->first());
		if (next_nest == addition) return(FALSE);
		if (addition_guid != NULL) {
			GUID *next_guid = next_nest->pointer_to_nest_guid();
			if (next_guid != NULL && IsEqualGUID(*addition_guid,*next_guid)) {
				return(FALSE);
			};
		};		
		if (remaining->rest() == NULL) {
			remaining->set_rest(new Sprites(nest));
			return(TRUE);
		};
		remaining = remaining->rest();
	};
};

boolean Sprites::insert_at_end_if_not_equal_member(Sprite *addition, int *index_if_member) { // new on 250601
   Sprites *remaining = this; 
	int index = 0;
	while (TRUE) {
		if (equal(remaining->first(),addition)) {
			if (index_if_member != NULL) {
				*index_if_member = index;
			};
			return(FALSE);
		};
	   if (remaining->rest() == NULL) {
			remaining->set_rest(new Sprites(addition));
			return(TRUE);
		};
		remaining = remaining->rest();
		index++;
	};
};

Sprite *Sprites::insert_at_end_or_replace_same_guid(Sprite *addition, GUID *guid) {
	// new on 050304 that this returns the old Sprite if replacing
	Sprites *remaining = this; 
	while (TRUE) {
		Sprite *sprite = remaining->first();
		if (sprite->has_guid(guid)) {
			remaining->set_first(addition);
			// think about whether for debugging it would be good to pass the old debug_counter to the new one?? - asked 150703
			return(sprite);
		};
		if (remaining->rest() == NULL) {
			remaining->set_rest(new Sprites(addition));
			return(NULL);
		};
		remaining = remaining->rest();
	};
	return(NULL);
};

//boolean Sprites::insert_at_end_if_unique(Sprite *addition, GUID *guid) {
//	// new on 050304 returns TRUE if successful or if the addition is already there
//	Sprites *remaining = this; 
//	while (TRUE) {
//		if (remaining->first()->has_guid(guid)) {
//			return(remaining->first() == addition);
//		};
//		if (remaining->rest() == NULL) {
//			remaining->set_rest(new Sprites(addition));
//			return(TRUE);
//		};
//		remaining = remaining->rest();
//	};
//	return(FALSE);
//};

void Sprites::insert_by_priority(Sprite *addition) {
	// sorted by descending priority so they are displayed
	// in the right order
	// note currently if priority changes this is not recomputed
//	if (sprite == addition) {  // already is a member
//		return;
//	} else if (next == NULL) {
//		if (// !sprite->completely_inside_leader() &&
//			 sprite->priority() < addition->priority()) {
//			next = new Sprites(sprite);
//			sprite = addition;
//		} else {
//			next = new Sprites(addition);
//		};
//	} else {
	  Sprites *remaining = this; // was next;
	  while (TRUE) {
		 Sprite *sprite = remaining->first();
		 if (sprite == addition) return; // already there
		 if (// !sprite->completely_inside_leader() && 
			 sprite->priority() < addition->priority()) { 
			 remaining->set_first(addition);
			 remaining->set_rest(new Sprites(sprite,remaining->rest()));
			 return;
		 };
		 if (remaining->rest() == NULL) {
			 remaining->set_rest(new Sprites(addition));
			 return;
		 };
		 remaining = remaining->rest();
	  };
//	};
};	

unsigned long Sprites::length() {
	if (next == NULL) {
		return(1);
	} else {
//      return(1+next->length());
	  unsigned int result = 2;
	  Sprites *remaining = next;
	  while (TRUE) {
		  if (remaining->rest() == NULL) return(result);
		  remaining = remaining->rest();
		  result++;
	  };	  
	};
};

Sprites *Sprites::last_cons() {
	Sprites *remaining = this;
	while (remaining != NULL) {
		if (remaining->rest() == NULL) return(remaining);
		remaining = remaining->rest();
	};
   return(this); // should never happen but keeps the compiler from warning
};

Sprites *Sprites::pop(SpritePointer &sprite_pointer) {
	sprite_pointer = sprite;
#if TT_DEBUG_ON
	if (tt_debug_mode == 160205 && tt_deleted_lists != NULL && tt_deleted_lists->member_cons(this) != NULL) {
		debug_this("Same list to be deleted twice.");
	};
#endif
	tt_deleted_lists = new SpritesList(this,tt_deleted_lists);
//	if (tt_frame_number == 3538) tt_error_file() << tt_deleted_lists << " " << tt_deleted_lists->first() << " 1" << endl;
//	sprite = NULL;
//	Sprites *saved_next = next;
//	next = NULL;
//	delete this;
//	return(saved_next);
   return(next);
};

#if TT_XML

// new on 051202
void Sprites::add_xml(xml_node *parent, wide_string tag, xml_document *document) {
	// prior to 170803 parent was xml_element
	// this creates a single element with the list inside instead of an element for each item
	xml_element *list_element = create_xml_element(tag,document);
	add_xml(list_element,document);
	xml_append_child(list_element,parent); // moved here on 190804
};

// new on 011102
void Sprites::add_xml(xml_node *parent, xml_document *document, wide_string tag, 
                      boolean geometry_too, boolean dump_all_regardless, PredicateCallBack predicate, void *predicate_arg, city_coordinate delta_x, city_coordinate delta_y) {
   // revisit geometry_too stuff 011102
	// prior to 170803 parent was xml_element
   boolean need_to_apply_delta = (geometry_too && (delta_x != 0 || delta_y != 0)); // new on 051102
   Sprites *remaining = this;
   while (remaining != NULL) {
      Sprite *sprite = remaining->first();
		if (sprite == NULL) { // could be for example a marker used by Dusty
			xml_append_child(create_xml_element(L"Nothing",document),parent);
			// why does dump_all_regardless override ok_to_dump? -- is it only used by picture to dump followers??
		} else if (dump_all_regardless || 
			        (!sprite->held() && sprite->ok_to_dump() && (predicate == NULL || predicate(sprite,predicate_arg)))) { 
         // held condition bad modularity?? -- revisit this conditional? 011102
 //        xml_element *element; // commented out on 200804 since not used
         if (need_to_apply_delta) {
            sprite->just_apply_delta(delta_x,delta_y);
         };
         if (tag == NULL) {
            // element = 
				sprite->xml_create_and_append_element(parent,document)->Release(); // think about sharing and include_file args - asked 120703
         } else {
            xml_element *surrounding_element = create_xml_element(tag,document);
            // element = 
				sprite->xml_create_and_append_element(surrounding_element,document)->Release();
				xml_append_child(surrounding_element,parent); // moved here on 190804
         };
         if (need_to_apply_delta) {
            sprite->just_apply_delta(-delta_x,-delta_y); // restore location
         };
    //     if (geometry_too) {
				//if (sprite->kind_of() == PICTURE) { // starting 120200 they dump geometry regardless on their own
				//	//sprite->dump_priority(stream); // revisit 011102
				//	//sprite->dump_misc(stream);
				//	if (delta_x != 0 || delta_y != 0) {
    //              xml_set_attribute(element,L"DeltaX",delta_x);
    //              xml_set_attribute(element,L"DeltaY",delta_y);
				//	};
				////} else {
				////	sprite->dump_geometry(stream,delta_x,delta_y);
				//};
    //     };
		};
      remaining = remaining->rest();
   };
};

xml_document *Sprites::xml_new_document(wide_string tag_string) {
	Element_Sprite_Table *old_table = push_current_element_sprite_table(); // new on 190103
	Numbered_Sprites *old_numbered_sprites = push_current_numbered_sprites(); // new on 190103
	xml_document *document = create_xml_document();
   add_processing_instruction(document);
	add_xml(document,tag_string,document);
	restore_numbered_sprites(old_numbered_sprites);
	restore_current_element_sprite_table(old_table,document);
   restore_saved_nest_guids();
	return(document);
};

void Sprites::xml_to_stream(output_stream &stream, wide_string tag_string) {
	xml_document *document = xml_new_document(tag_string);
	if (document == NULL) {
		return; // new on 140105 -- just in case
	};
	::xml_to_stream(document,stream);
	do_postponed_nest_xml(document); // new on 060204 -- broke puzzle 44 (due to nest from puzzle 43)
	xml_release_document(document);
};

//void Sprites::check_for_independence() { // new on 100703 // obsolete as of 150703
//	Sprites *remaining = this;
//   while (remaining != NULL) {
//      Sprite *sprite = remaining->first();
//		if (!sprite->part_of(tt_part_of_city_being_dumped_for_time_travel)) {
//			throw(this);
//		};
//	};
//};

#endif

void Sprites::dump(output_stream &stream, boolean geometry_too, boolean just_prepare, 
						 boolean dump_all_regardless, PredicateCallBack predicate, void *predicate_arg, city_coordinate delta_x, city_coordinate delta_y) {
   // WARNING: load_items expects geometry_too to be TRUE
   Sprites *remaining = this;
   if (just_prepare) {
      while (remaining != NULL) {
			Sprite *sprite = remaining->first();
         if (sprite != NULL) sprite->dump(stream,just_prepare);
         remaining = remaining->rest();
      };
      return;
   };
   while (remaining != NULL) {
      Sprite *sprite = remaining->first();
		if (sprite == NULL) { // could be for example a marker used by Dusty
			stream.put(NULL_MARKER); // new on 180100
			// why does dump_all_regardless override ok_to_dump? -- is it only used by picture to dump followers??
		} else if (dump_all_regardless || 
			        (!sprite->held() && sprite->ok_to_dump() && (predicate == NULL || predicate(sprite,predicate_arg)))) { // held thing bad modularity??
         // IF THIS CHANGES ALSO CHANGE City::dump
			sprite->dump(stream,just_prepare);
         if (geometry_too && !tt_dumping_to_test_equality_or_describe) {
				if (sprite->kind_of() == PICTURE) { // starting 120200 they dump geometry regardless on their own
					sprite->dump_priority(stream);
					sprite->dump_misc(stream);
					if (delta_x == 0 && delta_y == 0) {
						stream.put((char) FALSE); // just a boolean indicating no deltas - saves 7 bytes
					} else {
						stream.put((char) TRUE);
						stream.write((char *) &delta_x,sizeof(delta_x));
						stream.write((char *) &delta_y,sizeof(delta_y));
					};
				} else {
					sprite->dump_geometry(stream,delta_x,delta_y);
				};
         };
		};
      remaining = remaining->rest();
   };
//   stream.put((unsigned char) END_OF_LIST_MARKER);
};

boolean Sprites::update_display(boolean from_screen, TTRegion &region, boolean containment_active) {
	// returns false if first element is NULL and needs to be removed by caller
	boolean result = TRUE;
	Sprites *remaining = this;
	Sprites *previous_remaining = this;
	while (remaining != NULL) {
		Sprite *sprite = remaining->first();
#if TT_DEBUG_ON
		if (sprite == NULL) {
			say_error(_T("Something wrong with ToonTalk -- something is missing from background."));
		} else if (sprite->deleted()) {
			say_error(_T("Oh boy, ToonTalk is mixed up. It tried to use something after destroying it."));
			sprite = NULL;
		};
#endif
		if (sprite != NULL && !sprite->prepared_for_deletion()) {
			if (from_screen == sprite->screen_updates()) {
				sprite->update_display();
#if TT_DEBUG_ON
				add_to_log_if_interesting_error();
#endif
            if (containment_active && !sprite->clean() && sprite->containable_by_background() && !sprite->held() && !sprite->animation_in_progress() 
					// new on 240102 to not bother while it is animating - e.g. might be dropping into a box hole but be very wide
                && tt_system_mode != PUZZLE) { // for now don't need to constrain puzzle items
//                && tt_log_version_number > 9) {
               // make sure it fits in region
               city_coordinate llx,lly;
               sprite->lower_left_corner(llx,lly);
               city_coordinate true_width = sprite->true_width();
               city_coordinate true_height = sprite->true_height();
               city_coordinate width = sprite->current_width();
               city_coordinate height = sprite->current_height();
					city_coordinate region_width = region.width();
					city_coordinate region_height = region.height();
					if (width <= region_width && height <= region_height) { // new on 060202 so that very large objects (e.g. bignums) aren't shrunk by setting them down
						boolean x_changed = FALSE;
						boolean y_changed = FALSE;
						if (llx < region.min_x) {
							llx = region.min_x;
							x_changed = TRUE;
						} else if (llx+true_width >= region.max_x) {
							llx = region.max_x-true_width;
							x_changed = TRUE;
						};
						if (lly < region.min_y) {
							lly = region.min_y;
							y_changed = TRUE;
						} else if (lly+true_height >= region.max_y) {
							lly = region.max_y-true_height;
							y_changed = TRUE;
						};
						if (true_width > region_width) {
							width = region_width;
							x_changed = TRUE;
						};
						if (true_height > region_height) {
							height = region_height;
							y_changed = TRUE;
						};
						if (x_changed || y_changed) {  // and make "thud" like noise??
							sprite->set_size_and_location(width,height,llx,lly,UPDATING); // UPDATING added 090102
							sprite->signal_size_and_location(); // new on 310799 so things like cubbies with many holes adjust to the change
							sprite->stop_moving(x_changed,y_changed);
						};
					};
				};
			};
		} else if (sprite == NULL) {
		   // splice it out (was unconditional before but others (postponed_removals) should splice it out later)
			if (remaining == this) { // first element
				result = FALSE;
			} else {
				previous_remaining->set_rest(remaining->rest());
#if TT_DEBUG_ON
				if (tt_debug_mode == 160205 && tt_deleted_lists != NULL && tt_deleted_lists->member_cons(remaining) != NULL) {
					debug_this("Same list to be deleted twice.");
				};
#endif
				tt_deleted_lists = new SpritesList(remaining,tt_deleted_lists);
			};
		};
		previous_remaining = remaining;
		remaining = remaining->rest(); //saved_next;
	};
	return(result);
};

boolean Sprites::update_display_all(const UpdateStatus update_status) {
	// same as above but unconditional and calls propagate_changes
	// returns false if first element is NULL and needs to be removed by caller
	boolean result = TRUE;
	Sprites *remaining = this;
	Sprites *previous_remaining = this;
	while (remaining != NULL) {
		Sprite *sprite = remaining->first();
//		Sprites *saved_next = remaining->rest();
#if TT_DEBUG_ON
		if (sprite == NULL) {
			say_error(_T("Something wrong with ToonTalk -- something is missing from background."));
		} else if (sprite->deleted()) {
			say_error(_T("Oh boy, ToonTalk is mixed up. It tried to use something after destroying it. "));
			sprite = NULL;
		};
#endif
		if (sprite != NULL) {
//			if (from_screen == sprite->screen_updates()) {
				sprite->update_display(update_status); // argument added on 280199
				sprite->propagate_changes(UPDATING); // so things work fine recursively - arg new on 311004
//			};
		} else { // splice it out
			if (remaining == this) { // first element
				result = FALSE;
			} else {
				previous_remaining->set_rest(remaining->rest());
#if TT_DEBUG_ON
				if (tt_deleted_lists != NULL && tt_deleted_lists->member_cons(remaining) != NULL) {
					debug_this("Same list to be deleted twice.");
				};
#endif
				tt_deleted_lists = new SpritesList(remaining,tt_deleted_lists);
//	if (tt_frame_number == 3538) tt_error_file() << tt_deleted_lists << " " << tt_deleted_lists->first() << " 3" << endl;
			};
//			remaining->set_first(NULL);
//			remaining->set_rest(NULL);
//			delete remaining;
		};
//		remaining = saved_next;
      previous_remaining = remaining;
		remaining = remaining->rest();
	};
	return(result);
};

void Sprites::add_all_to_screen() {
	Sprites *remaining = this;
	while (remaining != NULL) {
		Sprite *sprite = remaining->first();
#if TT_DEBUG_ON
		if (sprite != NULL && sprite->deleted()) {
			debug_this("Deleted sprite being added to screen!");
		};
#endif
		if (tt_screen->add(sprite)) {
			sprite->add_outside_followers_to_screen(); // I tried commenting this out on 151199 but requires major changes
			// commented out the following on 170703 since screen no longer gets full and screen::add returns FALSE if already had been added
		//} else { // add failed must be that screen is full
		//	return;
		};
		remaining = remaining->rest();
	};
};

void Sprites::shift_all(city_coordinate delta_x, city_coordinate delta_y) {
//	finish_instantly(); // rewritten on 180903 so that all finish and then are shifted (in case Bammer is in progress)
   Sprites *remaining = this;
   while (remaining != NULL) {
      Sprite *sprite = remaining->first();
		if (tt_log_version_number < 33) {
			sprite->finish_instantly(); // new on 260803 - not certain if good for all but there are many problems animating while scrolling
			sprite->move_by(delta_x,delta_y);
		} else {
			// new on 020204
			sprite->shift_viewpoint(delta_x,delta_y);
		};
      remaining = remaining->rest();
   };
};

//void Sprites::clear_changes() {
//	sprite->clear_changes();
//	if (next != NULL) {
//		next->clear_changes();
//	};
//};

/*
void Sprites::remove(Sprite *removal) {
  // would be easy to return boolean whether it was in list or not
  if (sprite == removal) {
	  sprite = NULL;
  } else if (next != NULL) {
	  Sprites *new_next = next->remove1(removal);
	  if (new_next != next) {
		  if (new_next != NULL) {
			  delete next;
		  };
		  next = new_next;
	  };
  };
};
*/

Sprites *Sprites::remove(Sprite *removal, boolean *found) {
	if (sprite == removal) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 160205 && tt_deleted_lists != NULL && tt_deleted_lists->member_cons(this) != NULL) {
			debug_this("Same list to be deleted twice.");
		};
#endif
		tt_deleted_lists = new SpritesList(this,tt_deleted_lists);
//	if (tt_frame_number == 3538) tt_error_file() << tt_deleted_lists << " " << tt_deleted_lists->first() << " 4" << endl;
//		Sprites *rest = next;
//		sprite = NULL;
//		next = NULL;
//		delete this;
		if (found != NULL) *found = TRUE;
//		return(rest);
		return(next);
	} else { // not the first element
		Sprites *remaining = next;
		Sprites *previous_remaining = this;
		while (remaining != NULL) {
			if (remaining->first() == removal) {
				Sprites *rest = remaining->rest();
#if TT_DEBUG_ON
				if (tt_debug_mode == 160205 && tt_deleted_lists != NULL && tt_deleted_lists->member_cons(remaining) != NULL) {
					debug_this("Same list to be deleted twice.");
				};
#endif
				tt_deleted_lists = new SpritesList(remaining,tt_deleted_lists);
				previous_remaining->set_rest(rest);
				if (found != NULL) *found = TRUE;
				return(this);
			};
			previous_remaining = remaining;
			remaining = remaining->rest();
		};
	};
	if (found != NULL) *found = FALSE;
	return(this); // nothing found
};

Sprites *Sprites::remove_guid(GUID *guid) {
	if (sprite->has_guid(guid)) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 160205 && tt_deleted_lists != NULL && tt_deleted_lists->member_cons(this) != NULL) {
			debug_this("Same list to be deleted twice.");
		};
#endif
		tt_deleted_lists = new SpritesList(this,tt_deleted_lists);
		return(next);
	} else { // not the first element
		Sprites *remaining = next;
		Sprites *previous_remaining = this;
		while (remaining != NULL) {
			if (remaining->first()->has_guid(guid)) {
				Sprites *rest = remaining->rest();
#if TT_DEBUG_ON
				if (tt_debug_mode == 160205 && tt_deleted_lists != NULL && tt_deleted_lists->member_cons(remaining) != NULL) {
					debug_this("Same list to be deleted twice.");
				};
#endif
				tt_deleted_lists = new SpritesList(remaining,tt_deleted_lists);
				previous_remaining->set_rest(rest);
				return(this);
			};
			previous_remaining = remaining;
			remaining = remaining->rest();
		};
	};
	return(this); // nothing found
};

Sprites *Sprites::remove_type(SpriteType type, boolean *found) {
	if (sprite->kind_of() == type) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 160205 && tt_deleted_lists != NULL && tt_deleted_lists->member_cons(this) != NULL) {
			debug_this("Same list to be deleted twice.");
		};
#endif
		tt_deleted_lists = new SpritesList(this,tt_deleted_lists);
//	if (tt_frame_number == 3538) tt_error_file() << tt_deleted_lists << " " << tt_deleted_lists->first() << " 6" << endl;
//		Sprites *rest = next;
//		sprite = NULL;
//		next = NULL;
//		delete this;
		if (found != NULL) *found = TRUE;
//		return(rest);
      return(next);
	} else { // not the first element
		Sprites *remaining = next;
		Sprites *previous_remaining = this;
		while (remaining != NULL) {
			if (remaining->first()->kind_of() == type) {
				Sprites *rest = remaining->rest();
#if TT_DEBUG_ON
				if (tt_debug_mode == 160205 && tt_deleted_lists != NULL && tt_deleted_lists->member_cons(remaining) != NULL) {
					debug_this("Same list to be deleted twice.");
				};
#endif
				tt_deleted_lists = new SpritesList(remaining,tt_deleted_lists);
//	if (tt_frame_number == 3538) tt_error_file() << tt_deleted_lists << " " << tt_deleted_lists->first() << " 7" << endl;
//				remaining->set_first(NULL);
//				remaining->set_rest(NULL);
//				delete remaining;
				previous_remaining->set_rest(rest);
				if (found != NULL) *found = TRUE;
				return(this);
			};
			previous_remaining = remaining;
			remaining = remaining->rest();
		};
	};
	if (found != NULL) *found = FALSE;
	return(this); // nothing found
};

void Sprites::remove_all() {
	Sprites *remaining = this;
	while (remaining != NULL) {
      remaining->set_first(NULL);
		remaining = remaining->rest();
	};
};
//	sprite = NULL;
//	if (next != NULL) {
//		next->remove_all();
//		delete next;
//		next = NULL;
//	};

Sprites *Sprites::remove_last() {
	if (next == NULL) {
//		sprite = NULL;
//		delete this;
#if TT_DEBUG_ON
		if (tt_debug_mode == 160205 && tt_deleted_lists != NULL && tt_deleted_lists->member_cons(this) != NULL) {
			debug_this("Same list to be deleted twice.");
		};
#endif
		tt_deleted_lists = new SpritesList(this,tt_deleted_lists);
//	if (tt_frame_number == 3538) tt_error_file() << tt_deleted_lists << " " << tt_deleted_lists->first() << " 8" << endl;
		return(NULL);
	} else {
		next = next->remove_last();
		return(this);
	};
};

Sprites *Sprites::top_level_copy() {
   // this replicates the same bird/nest relationship in copy as in original
	// and the same controler/controlee relationship
	// type cast since compiler doesn't know that GlobalPicture is a sub sub class of Sprite
	UnwindProtect<Sprite *> set(tt_top_copy,(Sprite *) tt_global_picture); // to set up the scope of the copy // new on 191199
	Sprites *saved_copies = save_copied_items();
   Sprites *copies = copy();
   release_items_copied();
	restore_copied_items(saved_copies);
//   release_nests_being_copied();
//   release_non_networked_nests_with_guid(); // new on 181002
   return(copies);
};

Sprites *Sprites::top_level_copy_each() {
  Sprites *result = NULL;
  Sprites *last_cons = NULL;
  Sprites *remaining = this;
  while (remaining != NULL) {
     Sprite *sprite = remaining->first();
     if (sprite != NULL) {
        Sprites *new_last_cons = new Sprites(sprite->top_level_copy());
        if (last_cons == NULL) {
           result = new_last_cons;
           last_cons = new_last_cons;
        } else {
           last_cons->set_rest(new_last_cons);
           last_cons = new_last_cons;
        };
     };
     remaining = remaining->rest();
  };
  return(result);
};

Sprites *Sprites::copy(boolean also_copy_offsets) {
  Sprites *result = NULL;
  Sprites *last_cons = NULL;
  Sprites *remaining = this;
  while (remaining != NULL) {
     Sprite *sprite = remaining->first();
     if (sprite != NULL) {
        Sprites *new_last_cons = new Sprites(sprite->copy());
        if (last_cons == NULL) {
           result = new_last_cons;
           last_cons = new_last_cons;
        } else {
           last_cons->set_rest(new_last_cons);
           last_cons = new_last_cons;
        };
     };
     remaining = remaining->rest();
  };
  return(result);
};

Sprites *Sprites::just_copy_list(boolean increment_ref_count_too) {
  Sprites *result = NULL;
  Sprites *last_cons = NULL;
  Sprites *remaining = this;
  while (remaining != NULL) {
     Sprite *sprite = remaining->first();
     if (sprite != NULL) {
		  if (increment_ref_count_too) { // new on 150205
			  sprite->increment_ref_count();
		  };
        Sprites *new_last_cons = new Sprites(sprite);
        if (last_cons == NULL) {
           result = new_last_cons;
           last_cons = new_last_cons;
        } else {
           last_cons->set_rest(new_last_cons);
           last_cons = new_last_cons;
        };
     };
     remaining = remaining->rest();
  };
  return(result);
};

// following works but could blow the stack
//	if (next == NULL) {
//      if (sprite == NULL) return(NULL);
//		return(new Sprites(sprite->copy(background)));
//   } else if (sprite == NULL) {
//      return(next->copy(background));
//	} else {
//		return(new Sprites(sprite->copy(background),next->copy(background)));
//	};

Sprites *Sprites::member_cons(Sprite *search) {
  if (sprite == search) {
	  return(this);
  } else if (next == NULL) {
	  return(NULL);
  } else {
//	  return(next->member_cons(search));
	  Sprites *remaining = next;
	  while (remaining != NULL) {
		  if (remaining->first() == search) return(remaining);
		  remaining = remaining->rest();
	  };
	  return(NULL);
  };
};

Sprites *Sprites::successive_member_cons(Sprite *search1, Sprite *search2) { // new on 080601
	// returns rest of list if search1 and search2 are in this in successive positions (like a plist)
  if (next == NULL) {
	  return(NULL);
  } else {
//	  return(next->member_cons(search));
	  Sprites *remaining = this;
	  while (remaining != NULL) {
		  Sprite *sprite1 = remaining->first();
		  remaining = remaining->rest();
		  if (remaining == NULL) return(NULL);
		  if (sprite1 == search1) {
			  Sprite *sprite2 = remaining->first();
			  if (sprite2 == search2) {
				  return(remaining);
			  };
		  };
		  remaining = remaining->rest();
	  };
	  return(NULL);
  };
};

Sprites *Sprites::member_of_type(SpriteType type) {
  Sprites *remaining = this;
  while (remaining != NULL) {
     if (remaining->first()->kind_of() == type) {
	     return(remaining);
     };
     remaining = remaining->rest(); 
  };
  return(NULL);
};

int Sprites::index_of(Sprite *search, boolean &found) {
  Sprites *remaining = this;
  int index = 0;
  while (remaining != NULL) {
     if (remaining->first() == search) {
	     found = TRUE;
	     return(index);
     };
     index++;
     remaining = remaining->rest();
  };
  found = FALSE;
  return(0);
};

int Sprites::index_of_using_equal(Sprite *search, boolean &found) { // new on 250601
  Sprites *remaining = this;
  int index = 0;
  while (remaining != NULL) {
     if (equal(remaining->first(),search)) {
	     found = TRUE;
	     return(index);
     };
     index++;
     remaining = remaining->rest();
  };
  found = FALSE;
  return(0);
};

Sprite *Sprites::nth(long index, boolean warn) {
  Sprites *remaining = this;
  while (remaining != NULL) {
     if (index == 0) {
        return(remaining->first());
     };
     index--;
     remaining = remaining->rest();
  };
  if (warn) { // && tt_user_is_debugger) {
  	  say_error(_T("Warning ToonTalk tried to find nth item in too short a list."));
  };
  return(NULL);
};

void Sprites::set_nth(long index, Sprite *new_sprite, boolean warn) {
  Sprites *remaining = this;
  while (remaining != NULL) { 
     if (index == 0) {
	     remaining->set_first(new_sprite);
        return;
     };
     index--;
     remaining = remaining->rest();
  };
  if (warn) { // && tt_user_is_debugger) {
	  say_error(_T("Warning ToonTalk tried to change nth item in too short a list."));
  };
//  new_sprite = NULL;
};

Sprite *Sprites::first_to_have_inside(city_coordinate x, city_coordinate y) {
	Sprites *sprites_left = this;
	TTRegion region;
	while (sprites_left != NULL) {
		Sprite *current_sprite = sprites_left->first();
		current_sprite->collision_region(region);
		if (region.inside(x,y)) {
			return(current_sprite);
		};
		sprites_left = sprites_left->rest();
	};
	return(NULL);
};

Sprite *Sprites::first_to_contain(TTRegion *region) {
	Sprites *sprites_left = this;
	TTRegion current_region;
	while (sprites_left != NULL) {
		Sprite *current_sprite = sprites_left->first();
		current_sprite->collision_region(current_region);
		if (current_region.contains(region)) {
			return(current_sprite);
		};
		sprites_left = sprites_left->rest();
	};
	return(NULL);
};

Sprite *Sprites::first_to_overlap(TTRegion *region) {
	Sprites *sprites_left = this;
	TTRegion current_region;
	while (sprites_left != NULL) {
		Sprite *current_sprite = sprites_left->first();
		current_sprite->collision_region(current_region);
		if (current_region.overlaps(region)) {
         return(current_sprite);
      };
      sprites_left = sprites_left->rest();
   };
   return(NULL);
};

boolean Sprites::move_to_front(Sprite *new_front) {
   Sprites *old_location = member_cons(new_front);
   if (old_location == NULL) {
      return(FALSE); // wasn't there
	} else {
      Sprite *temp = old_location->first();
		old_location->set_first(sprite);
		sprite = temp;
		return(TRUE);
	};
};

void Sprites::display(SelectionFeedback selection_feedback,
							 boolean followers_too, TTRegion *region) {
   Sprites *remaining = this;
   while (remaining != NULL) {
      remaining->first()->display(selection_feedback,followers_too,region);
      remaining = remaining->rest();
   };
};

void Sprites::finish_instantly() {
   Sprites *remaining = this;
   while (remaining != NULL) {
		Sprite *sprite = remaining->first();
		// compute rest before running since finish_instantly might change this list
		remaining = remaining->rest();
      sprite->finish_instantly();
   };
};

Sprites *Sprites::reverse() {
	Sprites *answer = NULL;
	Sprites *remaining = this;
	while (remaining != NULL) {
		answer = new Sprites(remaining->first(),answer);
		remaining = remaining->rest();
	};
	return(answer);
};

Leaders::Leaders(Sprite *sprite, Leaders *next) :
  Sprites(sprite,next) {
};

void Leaders::propagate_changes() {
#if TT_CAREFUL
	if (sprite == NULL) return;
	if (sprite->deleted()) {
#if TT_DEBUG_ON
		say_error(_T("ToonTalk confused! A deleted item is still on its list of leaders."));
#endif
		sprite = NULL; // new on 070200
		return;
	};
#endif
	Sprite *leader = sprite->pointer_to_leader();
	if (leader != NULL && !leader->changes_propagated() && !sprite->is_top_of_a_stack()) {
		// added && !is_top_of_a_stack on 260500 since is treated specially when top of a stack
		// this guy depends on someone who hasn't run yet so run it
		// now and swap
		if (next == NULL) {
			// following new on 140904 since with nests the follower/leader hierarchy may have been constructed "out of order" so there is no problem here
			Sprites *leaders_followers = leader->pointer_to_followers();
			if (!leader->is_temporarily_flipped() && (leaders_followers == NULL || leaders_followers->member_cons(sprite) == NULL)) { // conditional new on 140904
				// there really is a problem so proceed -- added temporarily_flipped test on 041004
#if TT_DEBUG_ON
				say_error(IDS_HAS_PARTS_BUT_DOESNT);
				say_error(IDS_HAS_PARTS_BUT_DOESNT2);
#endif
				// following error message new on 040803
				tt_error_file() << "ToonTalk is repairing itself. ";
				print_type(sprite->kind_of(),tt_error_file(),INDEFINITE_ARTICLE,TRUE);
				tt_error_file() << " thinks it is a part of ";
				print_type(leader->kind_of(),tt_error_file(),INDEFINITE_ARTICLE,FALSE);
				tt_error_file() << " and it doesn't know that. Repair attempted but please report if any odd things happen." << endl;
				leader->add_follower(sprite); // new on 040903 to attempt to repair things
			};
			return;
		};
		Sprites *leader_cons = next->member_cons(leader);
#if TT_CAREFUL
		if (leader_cons == NULL) {
			tt_error_file() << "Internal ToonTalk error: " << type_name(sprite)
								 << " has a leader " << type_name(leader)
								 << " who is not known to the background." << endl;
#if TT_DEBUG_ON
			// following was say_error prior to 031000
			log(_T("ToonTalk is mixed up and thought something was a part of something and couldn't find it."),FALSE,TRUE);
			log(_T("Will try to fix things up."),FALSE,TRUE); // removed "get rid of it to " on 090105
#endif
//         sprite->destroy(); // commented out on 090105 since can make things worse (of course best to be sure this branch is never reached)
			sprite = NULL;
//			tt_exit_failure();
			return;
		};
#endif
		leader_cons->set_first(sprite);
		sprite = leader;
      propagate_changes(); // try again with new ordering
	} else {
// commented out since some sprites need to update followers even
// if no changes
//   if (!sprite->clean()) {
//		Sprite *this_sprite = sprite;
		// this cons might be deleted by following so save first
		sprite->propagate_changes(UPDATING); // arg is new on 311004
//   } else {
//      sprite->set_changes_propagated(TRUE);
//   };
//		if (next != NULL) {
//			((Leaders *) next)->propagate_changes();
//		};
//		this_sprite->set_changes_propagated(FALSE); // until next round
	};
};

#if TT_DEBUG_ON
int sprites_list_counter = 0; // for debugging only
#endif

SpritesList::SpritesList(Sprites *sprites, SpritesList *next) :
	sprites(sprites),
	next(next) {
#if TT_DEBUG_ON
	if (sprites->deleted() || (next != NULL && next->first() == sprites)) { 
		say_error(_T("About to delete the same list twice!"));
	};
	debug_counter = sprites_list_counter++;
#endif
};

//#if TT_DEBUG_ON
//SpritesList::~SpritesList() {
	//if (debug_counter == tt_debug_target) {
	//	debug_this();
	//};
//};
//#endif

void SpritesList::delete_all() {
#if TT_DEBUG_ON
	if (tt_debug_mode == 666) return; // new on 181004
#endif
	SpritesList *remaining = this;
	while (remaining != NULL) {
		Sprites *first = remaining->first();
		SpritesList *rest = remaining->rest();
		if (first != NULL) { // conditional new on 031202 due to Crash Report from Lennart - not clear how it can happen
			first->set_first(NULL);
			first->set_rest(NULL);
//			remaining->set_rest(NULL); // not necessary since there is no deconstructor
//			first->remove_all(); // these lists are really just conses to clean up -- commented out on 181009 since is a no-op given 
			// previous lines set it to a cons of NULL and NULL
#if TT_DEBUG_ON
			if (tt_debug_mode == 66 || tt_debug_mode == 66666) {
				if (first->deleted()) {
					debug_this("Deleting a list of sprites twice!");
				} else {
					first->set_deleted(TRUE);
				};
			} else {
				delete first;
			};
#else
			delete first;	
#endif
		};
		delete remaining;
		remaining = rest;
	};
};

#if TT_DEBUG_ON

SpritesList *SpritesList::member_cons(Sprites *sprites) {
	SpritesList *remaining = this;
	SpritesList *previous_remaining = NULL;
	int count = 0;
	while (remaining != NULL) {
		Sprites *first = remaining->first();
		if (first == sprites) {
			return(remaining);
		};
		previous_remaining = remaining; // for debugging
		count++; // for debugging
		remaining = remaining->rest();
	};
	return(NULL);
};
#endif


void insert_at_end(Sprite *sprite, SpritesPointer &sprites) {
	if (sprites == NULL) {
		sprites = new Sprites(sprite);
	} else {
		sprites->insert_at_end(sprite);
	};
};

void insert_sprites_at_end(SpritesPointer &first_sprites, SpritesPointer &second_sprites) { // new on 180201
	if (second_sprites != NULL) {
		if (first_sprites != NULL) {
			first_sprites->last_cons()->set_rest(second_sprites);
		} else {
			first_sprites = second_sprites;
		};
	};
};

void insert_sprites_at_end_if_not_member(SpritesPointer &first_sprites, SpritesPointer &second_sprites) { // new on 180201
	if (second_sprites != NULL) {
		if (first_sprites != NULL) {
			Sprites *remaining = second_sprites;
			while (remaining != NULL) {
				Sprite *sprite = remaining->first();
				insert_at_end_if_not_member(sprite,first_sprites);
				remaining = remaining->rest();
			};
			just_destroy_list(second_sprites); // since elements transfered one by one
		} else {
			first_sprites = second_sprites;
		};
	};
};

void insert_sprites_at_end_if_not_equal_member(SpritesPointer &first_sprites, SpritesPointer &second_sprites) { // new on 250601
	if (second_sprites != NULL) {
		if (first_sprites != NULL) {
			Sprites *remaining = second_sprites;
			while (remaining != NULL) {
				Sprite *sprite = remaining->first();
				insert_at_end_if_not_equal_member(sprite,first_sprites);
				remaining = remaining->rest();
			};
			just_destroy_list(second_sprites); // since elements transfered one by one
		} else {
			first_sprites = second_sprites;
		};
	};
};

void insert_by_priority(Sprite *sprite, SpritesPointer &sprites) {
	if (sprites == NULL) {
		sprites = new Sprites(sprite);
	} else {
		sprites->insert_by_priority(sprite);
	};
};

// written on 111204 but then turned out not needed -- restored on 140205 -- nest::all_nests order does matter when dumping a city
void insert_at_beginning_and_remove_duplicates(Sprite *sprite, SpritesPointer &sprites) { 
	// new on 111204 -- sprite is the first element of the list and removed if already in sprites
	if (sprites == NULL) {
		sprites = new Sprites(sprite);
	} else {
		boolean ignore;
		sprites = new Sprites(sprite,sprites->remove(sprite,&ignore));
	};
};

boolean insert_at_end_if_not_member(Sprite *sprite, SpritesPointer &sprites) {
	if (sprites == NULL) {
		sprites = new Sprites(sprite);
		return(TRUE);
	} else {
		return(sprites->insert_at_end_if_not_member(sprite));
	};
};

boolean insert_at_end_if_not_equal_member(Sprite *sprite, SpritesPointer &sprites) {
	if (sprites == NULL) {
		sprites = new Sprites(sprite);
		return(TRUE);
	} else {
		return(sprites->insert_at_end_if_not_equal_member(sprite));
	};
};

void insert_at_end_or_replace_same_guid(Sprite *addition, GUID *guid, SpritesPointer &sprites) {
	if (sprites == NULL) {
		sprites = new Sprites(addition);
	} else {
		sprites->insert_at_end_or_replace_same_guid(addition,guid);
	};
};

void insert_at_end_if_not_member_or_same_nest_guid(Sprite *nest, SpritesPointer &nests) {
	if (nests == NULL) {
		nests = new Sprites(nest);
	} else {
		nests->insert_at_end_if_not_member_or_same_nest_guid(nest);
	};
};

void Sprites::activate(boolean reset_animation, PictureActivateContext context) {
   Sprites *remaining = this;
   while (remaining != NULL) {
		Sprite *sprite = remaining->first();
      if (sprite->ok_to_activate()) { // && (!pictures_only || sprite->kind_of() == PICTURE)) { // 1st conditional added 190799
			// condition updated on 160702
         sprite->activate(reset_animation,context);
		};
      remaining = remaining->rest();
   };
};

Sprite *Sprites::find_copy(Sprite *original) {
	Sprites *remaining = this;
   while (remaining != NULL) {
      if (equal(remaining->first(),original)) {
			// could optimize this by dumping the original only once...
			return(remaining->first());
		};
      remaining = remaining->rest();
   };
	return(NULL);
};

Sprite *Sprites::last_element_of_type(SpriteType type) { // new on 101100
	Sprites *remaining = this;
	Sprite *matching_element = NULL;
	while (remaining != NULL) {
		Sprite *sprite = remaining->first();
		if (sprite->looks_like_is_of_type(type)) { // updated 210902
			matching_element = sprite;
		};
		remaining = remaining->rest();
	};
	return(matching_element);
};

void Sprites::increment_ref_count(Sprite *by) {
	Sprites *remaining = this;
   while (remaining != NULL) {
      Sprite *sprite = remaining->first();
		if (sprite != NULL) sprite->increment_ref_count(by);
      remaining = remaining->rest();
   };
};

void Sprites::decrement_ref_count() {
	Sprites *remaining = this;
   while (remaining != NULL) {
      Sprite *sprite = remaining->first();
		if (sprite != NULL) sprite->decrement_ref_count();
      remaining = remaining->rest();
   };
};

void Sprites::stop_all(boolean for_good, boolean suspend_robots_too) { // new on 260301
	Sprites *remaining = this;
   while (remaining != NULL) {
      Sprite *sprite = remaining->first();
		if (sprite != NULL) sprite->stop_all(for_good,suspend_robots_too);
      remaining = remaining->rest();
   };
};

Sprites *Sprites::truncate_to_length(int new_length) { // new on 190401
	Sprites *remaining = this;
   while (remaining != NULL) {
		if (new_length == 1) {
			Sprites *extra = remaining->rest();
			remaining->set_rest(NULL);
//			destroy_sprites(extra); // or at least decrements ref counts
			return(extra);
		};
		new_length--;
      remaining = remaining->rest();
   };
	return(NULL);
};

int Sprites::count_occurrences(Sprite *target) {
	int result = 0;
	Sprites *remaining = this;
   while (remaining != NULL) {
      if (remaining->first() == target) {
			result++;
		};
      remaining = remaining->rest();
   };
	return(result);
};

void Sprites::set_cause_of_match_failure(boolean new_flag, boolean recur) { // new on 250302
   Sprites *remaining = this;
   while (remaining != NULL) {
      remaining->first()->set_cause_of_match_failure(new_flag,recur);
      remaining = remaining->rest();
   };
};

//#if TT_XML
//
//boolean Sprites::xml_create_and_append_element(xml_element *parent, xml_document *document) {
//   // possibly obsolete
//   Sprites *remaining = this;
//   while (remaining != NULL) {
//      if (!remaining->first()->xml_create_and_append_element(parent,document)) return(FALSE);
//      remaining = remaining->rest();
//   };
//   return(TRUE);
//};
//
//#endif

/*
void Sprites::delete_all() {
  // like ~Sprites() but leaves a "NULL cons" behind
  if (sprite != NULL) {
	  delete sprite;
	  sprite = NULL;
  };
  if (next != NULL) {
	  delete next;
	  next = NULL;
  };
};
*/

/*
void Sprites::insert(Sprite *addition) {
	if (sprite == NULL) {
		sprite = addition;
	} else {
		next = new Sprites(addition,next);
	};
};
*/

/* old version -- items are now in the other order
Sprites *Sprites::update_display(boolean from_screen) {
	// do next first in case updating the display of a sprite changes this list
	// also so items of a floor are updated in oldest first
	// e.g. multiple birds waiting
	Sprite *saved_sprite = sprite;
#if TT_DEBUG_ON
	if (saved_sprite == NULL) {
		say("Something wrong with ToonTalk -- something is missing from background.");
	} else if (saved_sprite->deleted) {
		say("Oh boy, ToonTalk is mixed up.  It tried to use something after destroying it.");
		saved_sprite = NULL;
	};
#endif
	if (next != NULL) {
		next = next->update_display(from_screen);
	};
	if (saved_sprite != NULL && from_screen == saved_sprite->screen_updates()) {
		saved_sprite->update_display();
		return(this);
	} else { // splice it out
		Sprites *saved_next = next;
		next = NULL;
		delete this;
		return(saved_next);
	};
};

Sprites *Sprites::update_display_all() {
	// like above but updates all unconditionally
	Sprite *saved_sprite = sprite;
#if TT_DEBUG_ON
	if (saved_sprite == NULL) {
		say("Something wrong with ToonTalk -- something is missing from background.");
	};
	if (saved_sprite->deleted) {
		say("Oh boy, ToonTalk is mixed up.  It tried to use something after destroying it.");
		saved_sprite = NULL;
	};
#endif
	if (next != NULL) {
		next = next->update_display_all();
	};
	if (saved_sprite != NULL) {
		saved_sprite->update_display();
		return(this);
	} else { // splice it out
		Sprites *saved_next = next;
		next = NULL;
		delete this;
		return(saved_next);
	};
};
	  
void Sprites::display(Screen *screen, SelectionFeedback selection_feedback, 
							 boolean followers_too) {
	sprite->display(screen,selection_feedback,followers_too);
//	sprite->done_with_previous_values();
	if (next != NULL) {
		next->display(screen,selection_feedback,followers_too);
	};
};

void Sprites::display_reversed(Screen *screen, SelectionFeedback selection_feedback,
										 boolean followers_too) {
  // displays last one first
  if (next != NULL) {
	  next->display_reversed(screen,selection_feedback,followers_too);
  };
  sprite->display(screen,selection_feedback,followers_too);
//  sprite->done_with_previous_values();
};

void Sprites::display_reversed_type(SpriteType type,
												Screen *screen, SelectionFeedback selection_feedback) {
  // displays last one first
  if (next != NULL) {
	  next->display_reversed_type(type,screen,selection_feedback);
  };
  if (sprite->kind_of() == type) {
	  sprite->display(screen,selection_feedback);
  };
};

void Sprites::display_reversed_type_not(SpriteType type,
													 Screen *screen, 
													 SelectionFeedback selection_feedback) {
  // displays last one first
  if (next != NULL) {
	  next->display_reversed_type_not(type,screen,selection_feedback);
  };
  if (sprite->kind_of() != type) {
	  sprite->display(screen,selection_feedback);
  };
};

void Sprites::keep_first_n(unsigned int n) {
	if (n == 1) {
		if (next != NULL) {
			next->remove_all();
			delete next;
			next = NULL;
		};
	} else if (n > 1) {
		if (next != NULL) {
			next->keep_first_n(n-1);
		};
	};
};

boolean Sprites::animate_though_offscreen() {
	// run next first in case some remove elements from list
	boolean some_animation_in_progress = FALSE;
	Sprite *saved_sprite = sprite;
	if (next != NULL) {
		some_animation_in_progress = next->animate_though_offscreen();
	};
	if (saved_sprite->animation_in_progress() &&
		 saved_sprite->screen_updates()) {
		some_animation_in_progress = TRUE;
		saved_sprite->update_display();
	};
	return(some_animation_in_progress);
};
*/
/*
// temp
	Sprite *Sprites::first() {
	  if (deleted()) {
		  tt_error_file() << "first" << endl;
	  };
	  return(sprite); // can be NULL
	};
	void Sprites::set_first(Sprite *new_sprite) {
	  if (deleted()) {
		  tt_error_file() << "set first" << endl;
	  };
		sprite = new_sprite;
	};
	Sprites *Sprites::rest() {
	  if (deleted()) {
		  tt_error_file() << "rest" << endl;
	  };
	  return(next);
	};
	void Sprites::set_rest(Sprites *new_next) {
	  if (deleted()) {
		  tt_error_file() << "set rest" << endl;
	  };
		next = new_next;
	};
*/
