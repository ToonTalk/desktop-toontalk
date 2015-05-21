// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.
#if !defined(__TT_SPRITES_H)

#define __TT_SPRITES_H

//#if !defined(__TT_SPRITE_H)
//#include "sprite.h"
//#endif

//class Sprite; 

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif
#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif


class Sprites : public Entity { // a list of Sprite
 public:
	Sprites(Sprite *sprite=NULL, Sprites *next=NULL) :
#if TT_DEBUG_ON
	  deleted_flag(FALSE),
#endif
	  sprite(sprite),
	  next(next) {
#if TT_DEBUG_ON
	  debug_counter = tt_sprites_counter++;
#endif
	  };
	virtual ~Sprites();
//   void delete_all();
//   void insert(Sprite *sprite);
	boolean insert_at_end_if_not_member(Sprite *sprite); // could add int *index_if_member=NULL here too
	boolean insert_at_end_if_not_equal_member(Sprite *sprite, int *index_if_member=NULL);
	Sprite *insert_at_end_or_replace_same_guid(Sprite *addition, GUID *guid);
//	boolean insert_at_end_if_unique(Sprite *addition, GUID *guid);
	boolean insert_at_end_if_not_member_or_same_nest_guid(Sprite *nest);
	void insert_by_priority(Sprite *addition);
	void insert_at_end(Sprite *sprite) {
		// identical for now
		insert_at_end_if_not_member(sprite);
	};
	Sprites *remove(Sprite *sprite, boolean *found=NULL);
	Sprites *remove_type(SpriteType type, boolean *found=NULL);
	Sprites *remove_guid(GUID *guid);
	void remove_all();
   void delete_all();
   void destroy_all(Sprite *by=NULL);
   void recursively_activate_pictures(boolean reset_animation=TRUE, PictureActivateContext context=PICTURE_ACTIVATE_CONTEXT_NOT_INTIALIZED); 
	// 1st arg added and virtual removed on 150200
	void recursively_propagate_changes(ChangeReason reason=NO_REASON_GIVEN);
	void set_background(Background *new_background, boolean recur=FALSE, boolean warn=TRUE);
	Sprites *remove_last();
	Sprites *top_level_copy();
	Sprites *top_level_copy_each();
	Sprites *copy(boolean also_copy_offsets=FALSE);
	Sprites *just_copy_list(boolean increment_ref_count_too=FALSE);
#if TT_XML
	void add_xml(xml_node *parent, wide_string tag, xml_document *document);
   void add_xml(xml_node *parent, xml_document *document, wide_string tag=NULL, 
                boolean geometry_too=FALSE, boolean dump_all_regardless=FALSE, PredicateCallBack predicate=NULL, void *predicate_arg=NULL,
					 city_coordinate delta_x=0, city_coordinate delta_y=0);
	xml_document *xml_new_document(wide_string tag_string);
	void xml_to_stream(output_stream &stream, wide_string tag_string);
#endif;
	void dump(output_stream &stream, boolean geometry_too=FALSE, boolean just_prepare=FALSE, 
				 boolean dump_all_regardless=FALSE, PredicateCallBack predicate=NULL, void *predicate_arg=NULL, 
				 city_coordinate delta_x=0, city_coordinate delta_y=0);
	Sprites *member_cons(Sprite *search);
	Sprites *successive_member_cons(Sprite *search1, Sprite *search2);
	Sprites *member_of_type(SpriteType type);
	Sprite *first_to_contain(TTRegion *region);
	Sprite *first_to_overlap(TTRegion *region);
	Sprite *first_to_have_inside(city_coordinate x, city_coordinate y);
	boolean move_to_front(Sprite *new_front);
//	void display(Screen *screen, SelectionFeedback selection_feedback=NO_FEEDBACK,
//					 boolean followers_too=TRUE);
//	void display_reversed(Screen *screen, SelectionFeedback selection_feedback=NO_FEEDBACK,
//								 boolean followers_too=TRUE);
//	void display_reversed_type(SpriteType type,
//										Screen *screen, 
//										SelectionFeedback selection_feedback=NO_FEEDBACK);
//	void display_reversed_type_not(SpriteType type,
//											 Screen *screen, 
//											 SelectionFeedback selection_feedback=NO_FEEDBACK);
//	boolean animate_though_offscreen();
	boolean update_display(boolean from_screen, TTRegion &region, boolean containment_active);
	boolean update_display_all(const UpdateStatus update_status=UPDATE_CHANGE);
	void add_all_to_screen();
//	void clear_changes();
	Sprites *pop(SpritePointer &sprite_pointer);
	void delete_cons() {
		sprite = NULL;
		next = NULL;
		delete this;
	};
	Sprite *first() {
	  return(sprite); // can be NULL
	};
	void set_first(Sprite *new_sprite) {
		sprite = new_sprite;
	};
	Sprites *rest() {
	  return(next);
	};
	void set_rest(Sprites *new_next) {
		next = new_next;
	};
	int index_of(Sprite *sprite, boolean &found);
	int index_of_using_equal(Sprite *sprite, boolean &found);
	Sprite *nth(long index, boolean warn=TRUE);
	void set_nth(long index, Sprite *new_sprite, boolean warn=TRUE);
	unsigned long length();
	Sprites *last_cons();
   void shift_all(city_coordinate delta_x, city_coordinate delta_y);
   void display(SelectionFeedback selection_feedback=NO_FEEDBACK,
					 boolean followers_too=TRUE, TTRegion *region=NULL);
   void activate(boolean reset_animation=TRUE, PictureActivateContext context=PICTURE_ACTIVATE_CONTEXT_NOT_INTIALIZED);
	Sprites *reverse();
	Sprite *find_copy(Sprite *original);
	Sprite *last_element_of_type(SpriteType type);
	void increment_ref_count(Sprite *by=NULL);
	void decrement_ref_count();
	void stop_all(boolean for_good, boolean suspend_robots_too=TRUE);
	void finish_instantly();
	Sprites *truncate_to_length(int new_length);
	int count_occurrences(Sprite *target);
   void set_cause_of_match_failure(boolean new_flag, boolean recur);
	void decrement_ref_count_of_each();
//#if TT_XML
//   boolean xml_create_and_append_element(xml_element *parent, xml_document *document);
//#endif
//	void keep_first_n(unsigned int n);
#if TT_DEBUG_ON
	void set_deleted(boolean new_value) {
	  deleted_flag = (flag) new_value;
	};
	boolean deleted() {
	  return(deleted_flag);
	};
	int debug_counter; // new on 170403
#endif
 protected:
	Sprites *remove1(Sprite *sprite);
	Sprite *sprite;
	Sprites *next;
#if TT_DEBUG_ON
	flag deleted_flag : 1;
#endif
};

class Leaders : public Sprites {
  public:
	 Leaders(Sprite *sprite=NULL, Leaders *next=NULL);
	 void propagate_changes();
	 Leaders *remove(Sprite *removal, boolean *found=NULL) {
		 // using C++ templates would be cleaner
		 return((Leaders *) Sprites::remove(removal,found));
	 };
};

class SpritesList { // a list of list of sprites
  public:
	  SpritesList(Sprites *sprites, SpritesList *next=NULL);
//#if TT_DEBUG_ON
//	  ~SpritesList();
//#endif
	  void delete_all();
	  Sprites *first() {
		  return(sprites);
	  };
	  SpritesList *rest() {
		 return(next);
	  };
#if TT_DEBUG_ON
	  SpritesList *member_cons(Sprites *sprites); // new on 120804
	  int debug_counter; // new on 090305
#endif
  private:
	 Sprites *sprites;
	 SpritesList *next;
};

void insert_at_end(Sprite *sprite, SpritesPointer &sprites);
void insert_sprites_at_end(SpritesPointer &first_sprites, SpritesPointer &second_sprites);
void insert_sprites_at_end_if_not_member(SpritesPointer &first_sprites, SpritesPointer &second_sprites);
void insert_sprites_at_end_if_not_equal_member(SpritesPointer &first_sprites, SpritesPointer &second_sprites);
void insert_at_beginning_and_remove_duplicates(Sprite *sprite, SpritesPointer &sprites);
boolean insert_at_end_if_not_member(Sprite *sprite, SpritesPointer &sprites);
boolean insert_at_end_if_not_equal_member(Sprite *sprite, SpritesPointer &sprites);
void insert_at_end_or_replace_same_guid(Sprite *addition, GUID *guid, SpritesPointer &sprites);
void insert_at_end_if_not_member_or_same_nest_guid(Sprite *nest, SpritesPointer &nests);
void insert_by_priority(Sprite *sprite, SpritesPointer &sprites);

void destroy_sprites(SpritesPointer &sprites, Sprite *by=NULL);
void just_destroy_list(SpritesPointer &items);

/*
class Followers {
	public:
	  Followers(Sprite *follower);
	  ~Followers();	 
	  void add(Sprite *follower);
	  void remove(Sprite *follower);
	  void display(Screen *screen);
	  void follow_leader_changes(city_coordinate width, 
										  city_coordinate height,
										  city_coordinate center_x,
										  city_coordinate center_y,
										  boolean recur);
//	  Sprites *pointer_to_followers() {
//		  return(followers);
//	  };
//	  void set_followers(Sprites *sprites) {
//		 followers = sprites;
//	  };
	private:  
	 long recorded_leader_width, recorded_leader_height;
	 long width_times_leader_width, height_times_leader_height,
			x_offset_times_leader_width, y_offset_times_leader_height;
	 Sprites *followers;
};
*/

#endif
