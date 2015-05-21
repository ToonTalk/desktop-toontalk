// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved

#ifndef __PICTURE_H

#define __PICTURE_H

#if !defined(__TT_TOOLS_H)   
#include "tools.h"
#endif

class Floor;
class Bird;
class TTImage;
class Remote_Integer;
class Cubby;
class Robot;
//class Mouse;

typedef Sprites *SpritesPointer;
typedef Cubby *CubbyPointer;
typedef Robot *RobotPointer;

//const int first_identifier = X_REMOTE;
//const int last_identifier = NO_IDENTIFIER_GIVEN-1;

enum ScaleStates {VASCILATE_SCALE,
						EQUAL_SCALE,GREATER_THAN_SCALE,LESS_THAN_SCALE,
						GREATER_THAN_FROM_EQUAL_SCALE,LESS_THAN_FROM_EQUAL_SCALE,
						EQUAL_FROM_GREATER_SCALE,EQUAL_FROM_LESS_SCALE};

enum CollisionDirection {NO_COLLISION,NORTH_COLLISION,EAST_COLLISION,
								 SOUTH_COLLISION,WEST_COLLISION};

class Remote;

class Picture : public Sprite {
  public:
	 Picture(city_coordinate center_x=0, city_coordinate center_y=0,
			   long priority=0,
			   short int image_index=0,
			   SpriteCode image_code=SYNTHETIC_SPRITE,
//			   Sprite *image, 
//			   Background *floor=NULL,
			   city_coordinate width=0, city_coordinate height=0); //NUMBER_PAD_SPRITE));
//	 ~Picture();
	 void prepare_for_deletion();
	 void re_initialize();
//	 void release_outgoing_connections();
	 void release_all_references(); // prior to 060204 was (Sprite *by=NULL);
	 void set_slated_for_deletion(boolean new_flag);
	 void release_remote_notebook(); // Sprite *by);
	 void display(SelectionFeedback selection_feedback=NO_FEEDBACK,
					  boolean followers_too=TRUE, TTRegion *region=NULL);
    void display_followers(SelectionFeedback selection_feedback, TTRegion *region);
//	 boolean draw_backing_and_frame();
//	 void recursively_set_clip_region(SelectionFeedback selection_feedback=NO_FEEDBACK);
	 void update_display(const UpdateStatus update_status=UPDATE_CHANGE);
    void check_collisions(boolean new_collidee=FALSE);
	 void resume_all_robots();
	 Sprite *copy(boolean also_copy_offsets=FALSE);
	 Picture *controlee_replacement(boolean also_copy_offsets=FALSE);
	 Sprite *now_inside_thought_bubble(boolean in_training);
    void copy_flipped_followers(Picture *copy);
 	 Sprite *shallow_copy();
	 Picture *front_side_copy();
	 Picture *current_side_copy();
//	 Cubby *new_remote_cubby(Background *floor);
	 Notebook *new_remote_notebook(Background *floor=NULL, boolean add_pages_now=TRUE, boolean tell_remote=TRUE);
    void add_pages_to_notebook(Notebook *notebook,Background *floor);
    void add_pages_to_remote_notebook();
    void action_completed();
    void remote_notebook_flies_out(Background *background_of_picture=NULL);
	 void remote_notebook_added(Notebook *notebook);
	 void remote_notebook_deleted(Notebook *notebook);
    void add_unflipped_follower(Sprite *follower, AddFollowerReason reason);
	 void add_follower(Sprite *follower, boolean completely_inside=TRUE, InsertionOrdering insertion_ordering=INSERT_AT_END, 
							 boolean dont_record_new_offsets=FALSE, AddFollowerReason reason=NO_ADD_FOLLOWER_REASON_GIVEN);
	 boolean remove_follower(Sprite *sprite, boolean warn_if_not_follower=TRUE);
    void backside_size(city_coordinate &w, city_coordinate &h, Sprite *source=NULL);
	 Path *path_to(Sprite *target, boolean &ok);
//	 void add_dropped_cubby();
//	 void add_cubby(Cubby *remote_cubby=NULL);
//	 void add_dropped_robot();
//	 void add_robot(Robot *remote_robot=NULL);
	 Sprite *select(TTRegion *region, SelectionReason reason, Sprite *in_hand);
	 Sprite *selected(SelectionReason reason, Sprite *by);
	 void unselect();
	 boolean is_blank();
//	 void collision_region(TTRegion &region) {
//		 full_region(region);
//	 };
//	 void remove_selected_element(Sprite *element, SelectionReason );
	 Sprite *respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected,
	                             Sprite *, boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless=FALSE);
//	 void side_about_to_be_seen(boolean front);
	 void stop_all(boolean for_good=FALSE, boolean suspend_robots_too=TRUE);
    void add_robots_to_flipped_followers();
    void add_robot_as_flipped_follower(Robot *robot);
//	 void inside_cubby_hole() {
//		 stop_all();
//	 };
	 Sprite *next_to_collide(TTRegion &region, Sprite *starting_after=NULL, PredicateCallBack predicate=NULL, void *predicate_arg=NULL, 
									 boolean loop_around_if_not_found=TRUE);
	 void full_collision_region(TTRegion &region);
	 Sprite *released(Sprite *by, boolean top_level=TRUE, SpecialDrop special_drop=NOT_SPECIAL_DROP);
	 void tossed(Sprite *by);
	 void tell_flipped_followers_of_release(Sprite *by);
    void now_on_back_of(Picture *picture);
//	 void increment_screen_ref_count();
//	 void decrement_screen_ref_count();
    void recursively_activate_pictures(boolean reset_animation=TRUE, PictureActivateContext context=PICTURE_ACTIVATE_CONTEXT_NOT_INTIALIZED);
	 void activate(boolean reset_animation=TRUE, PictureActivateContext context=PICTURE_ACTIVATE_CONTEXT_NOT_INTIALIZED);
//	 Sprite *blank_all_remotes();
	 void become_blank(boolean by_user=FALSE, boolean controlee_too=TRUE);
	 void become_non_blank(boolean controlee_too=TRUE);
	 boolean blankable();
	 boolean current_text(const_string &the_text, long &the_text_length);
	 boolean current_wide_text(wide_string &the_text, long &the_text_length);
	 boolean can_be_text();
    ascii_string return_file_name();
    boolean current_value(NumberValuePointer &the_value, boolean &shared, Sprite *by=NULL);
	 boolean is_an_operation_with_no_value();
	 NumberOperation true_operation();
//	 void set_priority(long new_priority);
	 MatchStatus match(Picture *other, SpritePointerPointer &suspension_cell, int version, int index); 
//	 boolean is_truely_flipped();
	 void compute_scale_value(Relation relation);
	 boolean neighbors(SpritePointer &left_original, SpritePointer &right_original);
	 Relation compute_relation(boolean maintain_touched=FALSE,
                              boolean blanks_as_originals=TRUE,
                              boolean ignore_constant_relation=FALSE);
    Relation constant_relation();
	 boolean is_constant_scale() {
		 return (current_parameter() == EQUAL_SCALE ||
					current_parameter() == GREATER_THAN_SCALE ||
					current_parameter() == LESS_THAN_SCALE);
	 };
    void increment_scale_parameter(boolean up);
	 boolean is_synthetic() {
		 return(code == SYNTHETIC_SPRITE); 
	 };
    Sprites *sort_flipped_followers();
    Sprites *robots_on_the_back();
    Sprites *reachable_sprites_of_type(SpriteType type);
	 Sprites *reachable_non_equal_sprites_of_type(SpriteType type);
    Robot *robot_attached_to(Sprite *cubby);
    string java_start(Cubby *cubby=NULL, Text *text=NULL, Notebook *notebook=NULL,
                      Picture *picture=NULL,
                      Sprites *more_robots_for_picture=NULL, Sprites *more_notebooks=NULL, Sprite *display_this=NULL, java_string extra_text=NULL);
    void java(java_stream &out);
    void java_before(java_stream &out);
    java_string java_name();
	 void java_after(java_stream &out, const_java_string variable);
    void java_followers(java_stream &out, const_java_string picture_name);
	 void java_as_follower(java_stream &out);
    void java_flipped_followers(java_stream &out, const_java_string picture_name);
	 boolean java_flipped_followers_each(java_stream &out, const_java_string picture_name, Sprites *remaining);
//	 boolean worth_dumping_relative_geometry(boolean is_controlee, boolean on_nest, boolean flipped_originally);
//	 void dump_relative_geometry(output_stream &stream);
	 boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
	 unsigned char combine_flags();
	 boolean non_zero_speed();
//	 void dump_follower(Sprite *follower, output_stream &stream);
	 void generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection);
    boolean history_full();
	 void describe(output_stream &message, Article article=INDEFINITE_ARTICLE);
	 void describe_briefly(output_stream &message, Article article=INDEFINITE_ARTICLE,
							     boolean capitalize_first=FALSE);
	 void describe_type(output_stream &text_out, Article article=INDEFINITE_ARTICLE,
							  boolean verbose=TRUE, boolean capitalize_first=FALSE);
	 Sprite *used(Sprite *, Sprite * , boolean , boolean record_action, millisecond , 
				     boolean modified, Sprite *original_subject);
	 void flip_to_back();
//	 void copy_flipped_followers_of_controlee(Picture *copy);
	 void expand_back();
//	 city_coordinate full_width();
//	 city_coordinate full_height();
	 void send_remote_notebook_home();
	 void start_flip_to_front();
	 void finish_flip_to_front();
    void flip_to_front_finished();
	 ReleaseStatus item_released_on_top(Sprite *item, Sprite *by, SpritePointer &recipient, int level);
	 boolean receive_item(Sprite *item, Sprite *by, 
								 millisecond duration, Sprite *original_recipient, Sprite *original_item);
	 ReleaseStatus add_to_side(boolean right_side, Sprite *other,
										Sprite *by, 
										millisecond duration, Sprite *original_recipient, Sprite *original_item);
	 void update_text_and_widths_of_controlee();
	 void update_text_and_widths(boolean sizes_constrained);
	 void ignore_collision();
	 void set_size_and_location(city_coordinate new_width,
										 city_coordinate new_height,
										 city_coordinate new_llx,
										 city_coordinate new_lly,
										 ChangeReason reason=NO_REASON_GIVEN);
//	 void signal_size_and_location();
//    void place_new_item_old_version(Sprite *item);
    void place_new_item(Sprite *item=NULL, Sprite *underneath=NULL);
    void now_flipped();
	 Sprite *forwarding_to(boolean pictures_ok);
    boolean cycle_really_stopped();
	 void set_background(Background *new_background, boolean recur=FALSE, boolean warn=TRUE);
	 void set_to_future_value(Sprite *item, Sprite *original_recipient, Sprite *by);
//	 void mouse_done();
	 void set_inside_vacuum(boolean new_flag, boolean recur=TRUE);
    void set_in_notebook(Notebook *notebook, boolean recur=TRUE);
//	 void set_background(Background *new_floor);
	 void recursively_notice_on_screen_change(boolean on_screen);
	 void notice_on_screen_change(boolean on_screen);
	 boolean add_remote(RemoteIdentifier identifier, Sprite *remote_integer);
	 void remove_remote(RemoteIdentifier identifier, Sprite *remote);
	 virtual user_coordinate display_changed(RemoteIdentifier identifier, user_coordinate new_value);
	 double display_changed(RemoteIdentifier identifier, double new_value);
	 void speed_display_changed(RemoteIdentifier identifier, NumberValue *new_value);
	 double current_user_value_double(RemoteIdentifier identifier);
	 NumberValue *current_speed(RemoteIdentifier identifier, boolean copy);
	 virtual user_coordinate current_user_value(RemoteIdentifier identifier);
	 boolean using_room_coordinates();
	 void inside_thought_bubble();
	 boolean current_image_index(short int &index, SpriteCode &the_code);
	 short int picture_id(); // encodes image_index and cycle number
	 void set_image_index(short int new_value) {
		 // happens to be implemented by using parameter
		 set_parameter(new_value,FALSE); // don't warn if no good
	 };
	 user_coordinate user_x(city_coordinate value);
	 user_coordinate user_y(city_coordinate value);
	 double user_x_double_float(double value);
	 double user_y_double_float(double value);
	 user_coordinate user_saved_width();
	 user_coordinate user_saved_height();
	 city_coordinate from_user_x(user_coordinate value);
	 city_coordinate from_user_y(user_coordinate value);
	 city_coordinate from_user_x(double value);
	 city_coordinate from_user_y(double value);
	 void adjust_max_size(city_coordinate &max_width,
								 city_coordinate &max_height) {
	     // picture can fill entire screen
		  max_width = ideal_screen_width;
		  max_height = ideal_screen_height;
	 };
//	 Sprite *current_image() {
//		 return(image);
//	 };
//	 void add_image();
//	 void update_sizes();
//	 void set_image(Sprite *new_image);
//	 void make_remote();
    void stop_moving(boolean horizontal, boolean vertical) {
       if (horizontal) set_x_speed(NULL); // was x_speed = 0 prior to 021002
       if (vertical) set_y_speed(NULL); // was y_speed = 0;
    };
	 NumberValue *current_x_speed() {
		 return(x_speed);
	 };
	 NumberValue *current_y_speed() {
		 return(y_speed);
	 };
//	 void set_x_speed(double new_x_speed);
//	 void set_y_speed(double new_value);
	 void set_x_speed(NumberValue *new_x_speed);
	 void set_y_speed(NumberValue *new_value);
//	 user_coordinate previous_x_speed() { // noticed these were not called on 030302
//		 return(old_x_speed);
//	 };
//	 user_coordinate previous_y_speed() {
//		 return(old_y_speed);
//	 };
	 Picture *leader_picture();
	 void become_part_of_picture(boolean set_show_some=TRUE);
	 Picture *which_sub_picture_on_page(int page_number);
//	 void add_flipped_follower(Sprite *follower, Background *floor);
	 void remove_flipped_follower(Sprite *follower);
	 boolean is_flipped() {
		return(flipped);
	 };
	 void set_flipped(boolean new_flag) {
#if TT_DEBUG_ON
		if (tt_debug_target == debug_counter) {
			tt_error_file() << "Setting flipness of debug target." << endl;
		};
#endif
		 flipped = (flag) new_flag;
	 };
	 boolean is_temporarily_flipped() {
		 return(temporarily_flipped);
	 };
	 void set_temporarily_flipped(boolean new_flag) {
#if TT_DEBUG_ON
		 if (tt_debug_target == debug_counter) {
			 tt_error_file() << "Setting temporarily flipped flag to " << new_flag << endl;
		 };
#endif
		 temporarily_flipped = (flag) new_flag;
	 };
	 boolean is_temporarily_unflipped() {
		 return(temporarily_unflipped);
	 };
	 void set_temporarily_unflipped(boolean new_flag) {
		 temporarily_unflipped = (flag) new_flag;
	 };
	 boolean ok_to_cache_without_followers() {
		 // flipped pictures need to clip followers so can't cache them like this
		 return(Sprite::ok_to_cache_without_followers() && !flipped && controlee == NULL); // added second condition on 110200 since if remote looks not a good idea since appears to lose its followers
	 };
//	 void set_size_to_before_flip() {
//		 set_size(width_before_flip,height_before_flip);
//	 };
	 city_coordinate return_width_before_flip();
	 void set_width_before_flip(city_coordinate new_width);
	 city_coordinate return_height_before_flip();
	 void set_height_before_flip(city_coordinate new_height);
//	 void set_remote_notebook_added(boolean new_flag) {
//		 remote_notebook_added_flag = (flag) new_flag;
//	 };
	 Sprites *flipped_followers();
//	 void copy_flipped_followers_of_controlee_now();
	 Sprite *first_follower_of_type(SpriteType type);
	 boolean no_flipped_followers();
	 void just_set_flipped_followers(Sprites *replacement);
	 Sprites *unflipped_followers() {
		 if (!flipped) return(pointer_to_followers());
		 return(followers_on_other_side);
	 };
	 Sprites *pointer_to_followers_on_other_side() {
		 return(followers_on_other_side);
	 };
	 void flip(boolean temporarily=FALSE, boolean change_size=TRUE);
	 void unflip(boolean temporarily=FALSE, boolean change_size=TRUE);
	 CollisionDirection best_penetration(Sprite *collidee, 
													 city_coordinate &x_penetration,
													 city_coordinate &y_penetration);
	 void compute_penetration(Sprite *other, CollisionDirection direction,
									  city_coordinate &x, city_coordinate &y);
	 void set_suspended(boolean new_flag) {
		 suspended_flag = (flag) new_flag;
	 };
	 boolean suspended() {
		 return(suspended_flag);
	 };
	 void set_subpicture(boolean new_flag, boolean set_show_some=TRUE);
	 boolean subpicture() {
		 return(subpicture_flag);
	 };
	 void set_controlee(Picture *initial_controlee);
//	 void set_remote_appearance(boolean new_flag) {
//		 remote_appearance_flag = (flag) new_flag;
//	 };
//	 boolean remote_appearance() {
//		 return(remote_appearance_flag);
//	 };
	 boolean stopped();
	 boolean locally_stopped() {
		return(flipped || !active_flag || stopped_flag); // restored stopped_flag on 170702
	 };
//	 boolean is_stopped() {
//		 return(stopped_flag);
//	 };
	 void set_active(boolean new_flag, boolean recur=TRUE, boolean initialization=FALSE);
//	 void set_stopped(boolean new_flag);
    void set_show_all(boolean new_flag);
	 void collision_region(TTRegion &region) {
		 true_region(region); // was full_region
	 };
	 void reset_indirection() { // new on 280300
		 indirection = NULL;
	 };
	 Sprite *pointer_to_indirection() {
		 return(indirection);
	 };
	 Picture *pointer_to_controlee() {
		 return(controlee);
	 };
	 boolean reachable_from_remote_looks(Sprite *possible_remote_looks);
	 Sprite *dereference();
	 void controlee_deleted();
	 Remote *pointer_to_remote() {
		 return(remote); 
	 };
	 void just_set_remote_notebook(Notebook *notebook) { // new on 271000
#if TT_DEBUG_ON
		 if (debug_counter == tt_debug_target) {
			 tt_error_file() << "Debug this" << endl;
		 };
#endif
		 remote_notebook = notebook;
	 };
    void just_apply_delta(city_coordinate delta_x, city_coordinate delta_y);
//	 int image_index() {
//		if (image == NULL) return(-1);
//		return(image->current_parameter());
//	 };
//	 int image_resource_index() {
//		 if (image == NULL) return(-1);
//		 return(image->resource_index());
//	 };
	 SpriteType fine_kind_of();
	 void move_back_to_previous_location() {
		 move_to(old_llx,old_lly);
	 };
	 city_coordinate previous_llx() {
		 return(old_llx);
	 };
	 city_coordinate previous_urx() {
		 return(old_llx+old_width);
	 };
	 city_coordinate previous_lly() {
		 return(old_lly);
	 };
	 city_coordinate previous_ury() {
		 return(old_lly+old_height);
	 };
	 void set_previous_llx(city_coordinate x) {
		 old_llx = x;
	 };
	 void set_previous_lly(city_coordinate y) {
		 old_lly = y;
	 };
	 void set_indirection(Sprite *new_indirection, boolean transfer_followers);
	 boolean ultimate_indirection_is_picture();
	 void good_size(city_coordinate &w, city_coordinate &h, Sprite *source=NULL, boolean for_expander=FALSE);
	 void set_saved_size(city_coordinate w, city_coordinate h) {
		 set_saved_width(w);
		 set_saved_height(h);
	 };
	 void save_size() {
		 set_saved_width(width);
		 set_saved_height(height);
	 };
	 void set_saved_width(city_coordinate w);
	 void set_saved_height(city_coordinate h);
	 city_coordinate return_saved_width() {
		return(saved_width);
	 };
	 city_coordinate return_saved_height() {
		return(saved_height);
	 };
	 boolean robots_running_on_back() {
		 return(robots_running_on_back_flag);
	 };
    void set_show_some_if_subpicture(boolean new_flag) {
       show_some_if_subpicture_flag = (flag) new_flag;
    };
    boolean show_some_if_subpicture() {
       return(show_some_if_subpicture_flag);
    };
	 Notebook *pointer_to_remote_notebook();
    Background *pointer_to_backside() {
      return(backside_background);
    };
	 Background *create_backside_background_if_need_be();
    virtual boolean global() {
       return(FALSE);
    };
    // following two are used by remotes to define the range of identifiers used by this kind of picture
    virtual RemoteIdentifier return_first_identifier() {
       return(X_REMOTE);
    };
    virtual RemoteIdentifier return_last_identifier() {
       return(ANCESTORS_REMOTE); // was VISIBLE_REMOTE prior to 060499
    };
    virtual int return_joystick_number() {
       return(-1);
    };
//    city_coordinate true_width(boolean including_followers=FALSE);
//    city_coordinate true_height(boolean including_followers=FALSE);
    // commented out on 080502 the following since is added as not completely inside and works fine
	 //boolean ok_for_mouse_to_temporarily_add_follower() {
  //      return(FALSE); 
		//  // since this will cause it to clip before the mouse gets to it
		//  // this does mean that if the user grabs the picture underneath it'll look bad
  //   };
	 // following is currently only used by COLLIDEE_REMOTE to delay the copying of the flipped followers
	 //void set_extra_pointer_while_inaccessible(Sprite *item) { // noticed obsolete on 201202
		// item_to_add = item;
	 //};
	 //Sprite *extra_pointer_while_inaccessible() {
		// return(item_to_add);
	 //};
	 boolean circular_forwarding(Picture *other);
	 void tell_flipped_followers_now_on_back();
	 void now_on_floor(Background *floor, Sprite *by);
	 void animate_size(city_coordinate new_width,
							 city_coordinate new_height,
							 millisecond expansion_duration,
							 Sprite *notify=NULL,
							 AnimationCallBack callback=CALLBACK_NOTHING,
							 boolean signal_change=TRUE);
	 //void animate_size(city_coordinate *new_width,
		//					 city_coordinate *new_height,
		//					 millisecond expansion_duration,
		//					 Sprite *notify=NULL,
		//					 AnimationCallBack callback=CALLBACK_NOTHING,
		//					 boolean signal_change=TRUE);
	//void animate_location(city_coordinate new_llx,
	//							 city_coordinate new_lly,
	//							 millisecond expansion_duration,
	//							 Sprite *notify=NULL,
	//							 AnimationCallBack callback=CALLBACK_NOTHING,
	//							 boolean signal_change=TRUE);
	//void animate_location(city_coordinate *new_llx,
	//							  city_coordinate *new_lly,
	//							  millisecond expansion_duration,
	//							  Sprite *notify=NULL,
	//							  AnimationCallBack callback=CALLBACK_NOTHING,
	//							  city_coordinate x_offset=0,
	//							  city_coordinate y_offset=0,
	//							  boolean signal_change=TRUE);
//	void recursively_set_background(Background *new_background);
	void leader_changed();
	boolean size_is_true() {
		// new on 130499 so that flipping doesn't reset true_size_flag
		// but when flipped it is TRUE
		// on 040899 added part about controlee being flipped as well
	   return(true_size_flag || flipped || (controlee != NULL && controlee->is_flipped())); //  && tt_log_version_number > 14
	};
	//void set_ignore_controlee_when_size_changes(boolean new_flag) {
	//	ignore_controlee_when_size_changes_flag = (flag) new_flag;
	//};
	void update_remote(boolean regardless, UpdateStatus update_status=NO_UPDATE_CHANGE);
	boolean like_a_picture() {
		return(TRUE);
	};
	void remove_all_flipped_followers(boolean destroy_too);
	boolean has_followers();
	boolean rectangular();
	ArrowKeyEditing arrow_key_editing_status();
	ChangeFrequency change_frequency(); // new on 150200
	//void set_saved_by(Sprite *sprite) { // new on 121200 commented out on 191102 since taken care of by Sprite::set_saved_by now
	//	saved_by = sprite; // this could have been a leak?
	//};
//#if TT_DIRECT_PLAY
//	void set_being_marshalled(boolean new_flag);
//#endif
	void update_remote_looks_of_me_count(int change) {
		remote_looks_of_me_count += change;
	};
	void set_placed_new_items(boolean new_flag) {
		placed_new_items_flag = (flag) new_flag;
	};
	boolean placed_new_items() {
		return(placed_new_items_flag);
	};
	boolean equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, SpritesPointer &expected_corresponding_nests, 
						  EqualReason reason=NORMAL_EQUAL); // new on 050601
//	void become(Sprite *other);
   boolean on_backside();
#if TT_XML
//   xml_element *xml_create_element(xml_document *document);
   void xml(xml_element *element, xml_document *document);
   boolean handle_xml_tag(Tag tag, xml_node *node);
   boolean handle_geometry_when_free_and_unflipped_tag(Tag tag, xml_node *node);
	xml_element *xml_set_special_attributes(xml_element *element, xml_document *document);
	void add_special_xml(xml_element *element, xml_document *document);
	boolean xml_get_special_attributes(xml_node *node);
	boolean geometry_worth_saving();
	wide_string xml_tag();
	virtual boolean ok_to_save_geometry_when_free_and_unflipped() { // new on 170703 to avoid extra work saving such for buttons and the like
		return(TRUE);
	};
#endif
   virtual int return_force_index() {
      return(0); // only applies to ForceRemoteControlee
   };
	boolean looks_like_is_of_type(SpriteType type);
	void set_cause_of_match_failure(boolean new_flag, boolean recur);
	boolean show_all_default() { // new on 070103 since pictures are show all by default
      return(TRUE);
   };
	void just_set_original_file_name(string name);
	string private_media_file_name(boolean &worth_compressing, boolean full_path=TRUE);
	wide_string private_media_file_extension() {
		return(L"png");
	};
	void set_private_media_file_name(string name);
	hash return_hash();
	void translated_from_floor_to_room();
	void translated_from_room_to_floor();
	void set_programmer_state_before_flip(ProgrammerState new_state) {
		programmer_state_before_flip = new_state;
	};
//	void set_size_to_that_of_indirection();
	//	void ensure_remote_looks_has_flipped_followers();
//	void set_frame_flipped_followers_changed_last() {
//		frame_flipped_followers_changed_last = tt_frame_number;
//	};
//	int return_frame_flipped_followers_changed_last() {
//		return(frame_flipped_followers_changed_last);
//	};
//	void set_copying_flipped_followers_of_controlee(boolean new_flag) {
//		copying_flipped_followers_of_controlee_flag = (flag) new_flag;
//	};
//	void set_postponed_copy_flipped_followers_of_controlee(boolean new_flag) {
//	   postponed_copy_flipped_followers_of_controlee_flag = (flag) new_flag;
//	};
//	Sprites *pointer_to_followers(); // added on 040100 since if indirection should follow it
//	void set_last_size_change_due_to_indirection(boolean new_flag) {
//		last_size_change_due_to_indirection_flag = (flag) new_flag;
//	};
//	boolean last_size_change_due_to_indirection() {
//		return(last_size_change_due_to_indirection_flag);
//	};
//#if !TT_32
//    virtual boolean hyperbot_remote() {
//       return(FALSE);
//    };
//#endif
	 boolean flip_going_on();
	 void set_clean_status(CleanStatus status, boolean recur=TRUE);
	 void set_version_number(int n) { // new on 210506
		 version_number = n;
	 };
	 //boolean activated() {
		// return(activated_flag);
	 //};
	 //void set_activated(boolean new_flag) {
		// activated_flag = (flag) new_flag;
	 //};
  protected:
	 Sprite *next_to_collide_with(TTRegion &target_region, Sprite *exception,
											Sprite *starting_after=NULL,
											boolean want_flipped=FALSE,
											PredicateCallBack predicate=NULL, void *predicate_arg=NULL,
											boolean loop_around_if_not_found=TRUE);
	 void swap_followers_to_front(boolean change_size=TRUE, boolean temporarily=FALSE);
	 void swap_followers_to_back(boolean change_size=TRUE, boolean temporarily=FALSE);
//	 void push_cubby(Cubby *remote_cubby);
	 flag flipped : 1;
	 flag temporarily_flipped : 1;
	 flag temporarily_unflipped : 1; // new on 171200
//	 flag remote_notebook_added_flag : 1; // don't make until needed
	 flag suspended_flag : 1;
	 flag stopped_flag : 1; 
	 flag subpicture_flag : 1;
	 flag old_cycle_stopped : 1;
	 flag old_grasped : 1;
//	 flag selected_flag : 1;
	 flag old_selected_flag : 1;
	 flag released_last_frame : 1;
	 flag	released_flag : 1;
	 flag robots_running_on_back_flag : 1;
    flag show_some_if_subpicture_flag : 1;
//  flag remote_appearance_flag : 1;
	 // moved here on 180405 since best to keep flags together for compactness
//  flag last_size_change_due_to_indirection_flag : 1;
	 flag flip_going_on_flag : 1;
//	 flag ignore_controlee_when_size_changes_flag : 1; 
	 flag placed_new_items_flag : 1; // new on 060201 so flipped pictures always displays stuff on back properly
//	 flag activated_flag : 1; // new on 180405 so is only activated once
	 short int old_parameter; // moved here on 180405
	 Remote *remote;
	 Sprites *followers_on_other_side;
	 Sprite *indirection; // when appearance is derived from indirection sprite
	 Sprite *saved_indirection; // used when erased and then restored
	 Picture *controlee; // if this is a remote appearance for controlee
//	 CubbyPointer *remote_cubbys; // is remote_cubbys[] better?
//	 RobotPointer *remote_robots;
//	 int remote_cubby_count;
	 city_coordinate old_llx,old_lly,old_width,old_height;
	 city_coordinate width_before_flip,height_before_flip;
	 NumberValuePointer x_speed,y_speed; // ,old_x_speed,old_y_speed;
	 double llx_accumulated_error, lly_accumulated_error; // new on 030302 to support very slow speeds	 
//	 Sprite *item_to_add; -- commented out on 130103 since redundant (Mouse keeps hold of this pointer)
//	 Mouse *mouse;
//	 ActionStatus *saved_action_status; // made obsolete on 171102
//	 short unsigned int page_number_of_first_subnotebook;
	 Sprites *subpictures_with_notebooks;
	 Background *backside_background;
	 Notebook *remote_notebook;
	 Picture *label; // new on 110604 -- unfortunate that all have to pay to make displaying flipped pictures much faster 
	 // - cache some other way?
	 // dimensions are important to pictures so maintain them
	 city_coordinate saved_width, saved_height; 
//    Sprite *saved_by;
//	 Sprites *controlling_birds;
	 int remote_looks_of_me_count;
	 ProgrammerState programmer_state_before_flip; // new on 040604
//	 flag copying_flipped_followers_of_controlee_flag : 1; // new on 091100
//	 flag postponed_copy_flipped_followers_of_controlee_flag : 1; // new on 091100
//	 int frame_flipped_followers_changed_last; // new on 161200 for remote looks to show flipped followers correctly
	 int version_number; // new on 090506 -- initially only used by scales to fix case sensitivity problem
};

class UserPicture : public Picture {
  public:
     UserPicture(ascii_string original_file_name, boolean &ok, 
					  // warn arg added on 200201
					  boolean warn=TRUE, int pixel_width=-1, int pixel_height=-1,
					  ascii_string private_media_file_name=NULL); // original_file_name added 280403
	  // following used by attempt to load picture in lazy manner
//	  GenericImage *pointer_to_current_image();
//  protected:
//	  string file_name;
};

class UserPictureFileMissing : public Picture {
  public:
     UserPictureFileMissing(ascii_string file_name, xml_node *node=NULL);
	  void release_all_references();
	  Sprite *copy(boolean also_copy_offsets=FALSE);
	  void good_size(city_coordinate &w, city_coordinate &h, Sprite *source=NULL, boolean for_expander=FALSE) {
		  // to look nicer - 1/4 of screen to see well
		  w = ideal_screen_width/4;
		  h = ideal_screen_height/4;
	  };
	  void just_set_parameter(short int new_parameter) {
		  // ignore it since shouldn't change the color
	  };
#if TT_XML
//	  void set_xml(xml_node *node);
//	  xml_element *xml_create_element(xml_document *document);
	  wide_string xml_tag();
	  void xml(xml_element *element, xml_document *document);
	  xml_element *xml_set_special_attributes(xml_element *element, xml_document *document);
	  void add_special_xml(xml_element *element, xml_document *document);
#endif
  protected:
	  string file_name_short;
#if TT_XML
	  xml_node *XML;
#endif
};

class GlobalPicture : public Picture {
  // just used to handle global remote controls
  public:
      GlobalPicture() :
         Picture(0,0,0,0,SYNTHETIC_SPRITE,1,1) {
      };
		~GlobalPicture() {
			// new on 240105 since needed if dropping a DMO file on the file to object sensor
			tt_global_picture = NULL;
		};
		void update_display(const UpdateStatus update_status=UPDATE_CHANGE) {
			// never displayed and might be created before enough is known for updating the display
		};
      void update_globals();
      void follower_added(Sprite *leader, Sprite *new_follower);
      void follower_removed(Sprite *leader, Sprite *);
      boolean global() {
         return(TRUE);
      };
      user_coordinate display_changed(RemoteIdentifier identifier, user_coordinate new_value);
      user_coordinate current_user_value(RemoteIdentifier identifier);
      RemoteIdentifier return_first_identifier() {
         return(MOUSE_LEFT_BUTTON_REMOTE);
      };
      RemoteIdentifier return_last_identifier() {
         return(LANGUAGE_REMOTE); // updated 040200
      };
		boolean ok_to_cache() { // new on 090703 - important for the new time travel scheme for saving cities
			return(FALSE);
		};
		boolean ok_to_delete() {
			return(tt_resetting_or_destroying_city); // new on 290105 to protect tt_global_picture --  Jakob's crashes inspired this
		};
#if TT_XML
		boolean ok_to_save_geometry_when_free_and_unflipped() { 
			return(FALSE);
		};
		boolean geometry_worth_saving() {
			return(FALSE); // new on 190703
		};
#endif
};

#if !TT_32
class HyperBotGlobalPicture : public GlobalPicture {
  // just used to handle Hyperbot global remote controls
  public:
      HyperBotGlobalPicture() :
         GlobalPicture() {
      };
      void update_globals();
      user_coordinate display_changed(RemoteIdentifier identifier, user_coordinate new_value);
      user_coordinate current_user_value(RemoteIdentifier identifier);
      RemoteIdentifier return_first_identifier() {
         return(LEGO_MOTOR1_REMOTE);
      };
      RemoteIdentifier return_last_identifier() {
         return(LEGO_SENSOR_COUNT8_REMOTE);
      };
      boolean hyperbot_remote() {
         return(TRUE);
      };
};
#endif

class JoystickRemotePicture : public GlobalPicture {
  public:
      JoystickRemotePicture(short int index) :
         GlobalPicture(),
         joystick_number(index) {
      };
      void update_globals();
      user_coordinate display_changed(RemoteIdentifier identifier, user_coordinate new_value);
      user_coordinate current_user_value(RemoteIdentifier identifier);
      RemoteIdentifier return_first_identifier() {
         return(JOYSTICK_SPEED_X_REMOTE);
      };
      RemoteIdentifier return_last_identifier() {
         return(JOYSTICK_BUTTON32_DOWN_REMOTE);
      };
      int return_joystick_number() {
         return(joystick_number);
      };
  protected:
      short int joystick_number;
};

typedef JoystickRemotePicture *JoystickRemotePicturePointer;

class ForceRemoteControlee : public GlobalPicture {
  public:
      ForceRemoteControlee(int joystick_index, int force_index);
      void release_all_references();
      void load_effects();
      boolean unload_effects();
//      Sprite *copy(boolean also_copy_offsets=FALSE);
      void update_globals();
      user_coordinate display_changed(RemoteIdentifier identifier, user_coordinate new_value);
      user_coordinate current_user_value(RemoteIdentifier identifier);
      boolean dump(output_stream &stream, boolean just_prepare, boolean is_controlee=FALSE, boolean on_nest=FALSE);
      RemoteIdentifier return_first_identifier() {
         return(FORCE_FEEDBACK_DURATION_REMOTE);
      };
      RemoteIdentifier return_last_identifier() {
         return(FORCE_FEEDBACK_DIRECTION_REMOTE); // was FORCE_FEEDBACK_TRIGGER_BUTTON_REMOTE prior to 150801
      };
      void set_parameters(long intensity, long direction, long duration, unsigned char trigger_button);
      void play_effect();
      void stop_effect();
      ascii_string return_force_name();
      int return_force_index() {
         return(force_index); // new on 220802
      };
      int return_joystick_number() { // new on 230802
         return(joystick_index);
      };
  protected:
    long intensity, direction, duration;
    unsigned char trigger_button;
    unsigned short force_index;
    unsigned char joystick_index;
    flag parameters_explicitly_set;
    int effects_count;
#if TT_DIRECT_INPUT
    LPDIRECTINPUTEFFECT *effects;
#else
    void *effects;
#endif
};


class Button : public Picture {
   public:
      Button(SpriteCode image_code,
				 city_coordinate llx=0, city_coordinate lly=0,
			    char initial_letter=0,
			    long priority=0, // Background *floor,
			    city_coordinate width=0, city_coordinate height=0,
             color_index text_color=0);
      void display(SelectionFeedback selection_feedback=NO_FEEDBACK,
					    boolean followers_too=TRUE, TTRegion *region=NULL);
      Sprite *selected(SelectionReason reason, Sprite *by);
  	   Sprite *respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected, Sprite *by,
									       boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless=FALSE);
      void generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection);
      void describe(output_stream &message, Article article=INDEFINITE_ARTICLE);
//      void describe_type(output_stream &text_out, Article article=INDEFINITE_ARTICLE,
//		 					    boolean verbose=TRUE);
      void active_region(TTRegion &region);
      SpriteType fine_kind_of() {
         return(TOOL_BUTTON);
      };
      int history_index() {
         return(TOOL_BUTTON_HISTORY);
      };
      char current_letter() {
         return(letter);
      };
      void set_letter(char new_letter) {
         letter = new_letter;
      };
#if TT_XML
//	xml_element *xml_create_element(xml_document *document);
		wide_string xml_tag() {
			return(L"Button");
		};
		void xml(xml_element *element, xml_document *document);
		xml_element *xml_set_special_attributes(xml_element *element, xml_document *document);
		boolean xml_get_special_attributes(xml_node *node);
		boolean geometry_worth_saving() {
			return(FALSE); // makes sense to always keep it -- why?? -- changed to FALSE on 160804 due to drift of wand's button when pasted
		};
		boolean ok_to_save_geometry_when_free_and_unflipped() {
			return(FALSE);
		};
#endif
   private:
      char letter;
      color_index text_color;
};

void did_button_cycle(Sprite *by);
void button_letter_typed(Sprite *by);

Picture *load_picture(SpriteType type, InputStream *stream,
							 int notebook_version, NaturalLanguage language,
							 Sprite *container, Picture *controlee, long index, boolean is_controlee, boolean on_nest, boolean descendants_inactive);

boolean describe_picture(SpriteType type, InputStream *pad_in, output_stream &text_out, int version_number,
								 boolean in_thought_bubble=FALSE,
								 Article article=INDEFINITE_ARTICLE,  
								 SpriteType container_type=NONE_GIVEN, 
								 boolean is_controlee=FALSE, boolean on_nest=FALSE);
							 
void decode_picture_id(short id, short int &index, SpriteCode &the_code);

short int compute_picture_id(SpriteCode code, short int index);
//void picture_notebook_copied(Sprite *by);
void scale_used();
void removed_item_from_flip_side();

void run_postponed_collision_checks();
void release_java_pictures();

void release_postponed_collision_checks();

ForceRemoteControlee *load_force_remote_controlee(InputStream *pad_in, int notebook_version, NaturalLanguage language, long index);
ForceRemoteControlee *force_parameter_controlee(int force_index, int joystick, boolean &old);
int unload_some_effects(int count=4);
void release_force_parameter_controlees();
void add_flipped_follower(Sprite *follower, Sprite *previous_follower, Picture *picture, AddFollowerReason reason=NO_ADD_FOLLOWER_REASON_GIVEN);
void release_user_image_table();

void java_postponed_followers(java_stream &out);

/*
class is_outside_collection_Picture : public Picture {
  public:
	 Rectangular_Picture(city_coordinate center_x, city_coordinate center_y,
								long priority,
								unsigned char brush_color,
								Background *floor=NULL,
								city_coordinate width=5*tile_width, 
								city_coordinate height=5*tile_height);
	 Sprite *copy(Background *floor);
	 void display(Screen *screen, 
					  SelectionFeedback selection_feedback=NO_FEEDBACK);	
};

class Picture_Insides : public Sprite {
	public:
	 Picture_Insides(int image_index, SpriteCode sprite_code);
// 	 Picture_Insides *copy_insides();
	 void dump(output_stream &stream);
};
*/

//Picture_Insides *load_picture_insides(InputStream *stream, Background *floor);

#endif
