// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.
#ifndef __BIRD_H

#define __BIRD_H

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_TOOLS_H)   
#include "tools.h"
#endif   
class Bird; // defined later
class Path;
class Cubby;
class Robot;
typedef Cubby *CubbyPointer;
typedef Robot *RobotPointer;

#if TT_DIRECT_PLAY
const unsigned char MESSAGE_FROM_BIRD_TO_NEST = 0; // enum when there are more
const unsigned char first_message_version_number = 0;
const unsigned char current_message_version_number = 0;
#endif

enum NestStates {NEST_INERT,NEST_WITH_BROKEN_EGG,BIRD_HATCH,NEST_EMPTY};

// first 8 states are flying in 8 directions
enum BirdStates {BIRD_INERT=8,BIRD_MORPH_TO_LIFE,BIRD_MORPH_TO_INERT,
					  BIRD_FLY_DOWN,BIRD_FLY_UP,
					  BIRD_CONFUSED,BIRD_GIVE_ME,BIRD_MAKE_NEST,BIRD_BREATHING};


class Nest : public Sprite {
  public:
	 Nest(city_coordinate x=0, city_coordinate y=0,
			long priority=0,
//			Background *floor,
			city_coordinate width=0,
			city_coordinate height=0,
			short parameter=NEST_INERT,
			Sprites *contents_stack=NULL);
	 void release_all_references();
	 boolean release_nest_connections(Sprite *by=NULL);
	 boolean ok_to_delete();
	 Sprite *copy(boolean also_copy_offsets=FALSE);
	 void bird_just_copied();
    void inside_thought_bubble();
    Sprite *now_inside_thought_bubble(boolean in_training);
    void java_before(java_stream &out);
    void java(java_stream &out);
    void java_after(java_stream &out, const_java_string variable);
	 void java_initialize_nest(java_stream &out, int index);
	 boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
    boolean to_windows(Pointer *data, short int &data_offset,
                       ascii_string types, short int &types_offset,
                       BirdPointer *bird, short int &bird_count);
	 Sprite *select(TTRegion *region, SelectionReason reason, Sprite *in_hand);
	 void remove_selected_element(Sprite *element, SelectionReason reason, Sprite *by, boolean add_to_floor);
	 Sprite *released(Sprite *by, boolean top_level=TRUE, SpecialDrop special_drop=NOT_SPECIAL_DROP);
	 void unselect();
    void refit_contents();
	 void recursively_notice_on_screen_change(boolean on_screen);
	 void hatch_bird();
    void bird_has_hatched();
	 ReleaseStatus item_released_on_top(Sprite *item, Sprite *by, SpritePointer &recipient, int level);
	 boolean receive_item(Sprite *item, Sprite *by,
								 millisecond duration, Sprite *original_recipient, Sprite *original_item);
	 void add_item(Sprite *item, millisecond duration, Sprite *by);
	 void describe(output_stream &message, Article article);
	 void generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection);
//    void update_display();
//	 void enqueue(Sprite *item);
	 void insert_at_end_of_contents(Sprite *item);
	 void about_to_put_stack_back();
	 void accept_contents(boolean by_bird, Sprite *item=NULL, int retries=0);
//	 void now_on_back_of(Picture *picture);
    void set_priority(long new_priority);
//	 void activate_waiting_bird();
//	 void remove_bird();
//	 boolean ready_to_deliver(Bird *bird);
//	 void prepare_for_delivery(millisecond duration);
	 void stack_restored();
	 void fit_contents();
	 void	give_size_and_location_to_cover_to(Sprite *cover);
	 void location_and_size_to_cover(city_coordinate &cover_llx,
												city_coordinate &cover_lly,
												city_coordinate &cover_width,
												city_coordinate &cover_height); 
	 void remove_contents(Sprite *item, Background *floor_to_add_to); //millisecond delay, boolean animate_contents);
	 void remove_stack(Bird *for_bird);
	 void display(SelectionFeedback selection_feedback=NO_FEEDBACK,
					  boolean followers_too=TRUE, TTRegion *region=NULL);
	 void display_stack(Sprites *stack, SelectionFeedback selection_feedback=NO_FEEDBACK,
							  boolean followers_too=TRUE, TTRegion *region=NULL);
//	 void set_size_and_location(city_coordinate width,
//										 city_coordinate height,
//										 city_coordinate llx,
//										 city_coordinate lly,
//										 ChangeReason reason=NO_REASON_GIVEN);
	 void set_true_size_and_location(city_coordinate new_width, city_coordinate new_height,
												city_coordinate new_llx, city_coordinate new_lly,
												ChangeReason reason=NO_REASON_GIVEN);
	 boolean size_is_true();
	 void add_copy(Nest *nest);
	 void remove_copy(Nest *nest);
    Nest *first_copy_that_is_part_of_top_level_copy();
    void transfer_copies_to(Nest *a_copy);
	 void when_non_empty_try(Robot *robot, Cubby *cubby);
	 Cubby *transfer_cubby_to_try_robot_on() {
		 // prior to 240103 just returned the pointer and didn't reset to NULL
		 Cubby *cubby = cubby_to_try_robot_on;
		 set_cubby_to_try_robot_on(NULL);
		 return(cubby);
	 };
	 void set_robot_to_try(Robot *robot); // new on 130203
	 void set_cubby_to_try_robot_on(Cubby *cubby);
	 Robot *transfer_robot_to_try() {
		 Robot *robot = robot_to_try;
		 set_robot_to_try(NULL); // no-op if already is
		 return(robot);
	 };
	 void no_longer_waiting(Robot *robot) {
#if TT_POST3187
		 if (robot == robot_to_try)  // conditional new on 210307 due to bug reported by Jack Waugh on 040307
#endif
			 set_robot_to_try(NULL);
	 };
	 void adjust_size_and_location(city_coordinate &item_width,
											 city_coordinate &item_height,
											 city_coordinate &item_llx,
											 city_coordinate &item_lly);
//	 void adjust_grasp(city_coordinate &adjustment_x,
//							 city_coordinate &adjustment_y);
//    boolean ready_to_receive() {
//       return(ready_to_receive_flag);
//    };
	 void collision_region(TTRegion &region);
	 boolean save_region();
	 boolean stack_empty() {
		 return(contents_stack == NULL || stack_temporarily_removed);
	 };
	 boolean stack_removed() { // new on 110203
		 return(stack_temporarily_removed);
	 };
	 boolean empty() { // commented out on 071201 since redundant and not quite right - retored on 110203 for use by bird
		return(contents_stack == NULL);
	 };
	 boolean deleted() {
		return(deleted_flag);
	 };
	 Path *path_to(Sprite *target, boolean &ok);
	 void increment_bird_count();
	 void decrement_bird_count();
	 int current_bird_count();
	 void set_bird_count(int new_count) { // only used for special purposes
		 bird_count = new_count;
	 };
	 Sprite *pointer_to_contents(boolean even_if_removed=FALSE);
	 Sprite *dereference();
    long stack_size();
    boolean contains(Sprite *item);
	 Nest *pointer_to_original_nest() {
		return(original_nest);
	 };
	 Nest *ultimate_original_nest();
	 void set_original_nest(Nest *nest);
	 boolean ignore_me(boolean ignore_those_in_limbo);
	 Sprites *all_nests(boolean ignore_those_in_a_notebook, boolean ignore_those_in_limbo, 
							  Sprites *initial_nests=NULL, Sprites *nests_already_processed=NULL);
	 boolean in_a_notebook();
	 Sprites *pointer_to_nest_copies() {
		 return(nest_copies);
	 };
	 void set_nest_copies(Sprites *nests) { // usually NULL
		 nest_copies = nests;
	 };
	 void set_forwarding(boolean new_flag) {
		 forwarding_flag = (flag) new_flag;
	 };
	 boolean forwarding() {
		 return(forwarding_flag);
	 };
	 void set_forwarding_to(Nest *nest);
	 Nest *forwarding_to() {
		 return(forwarding_to_nest);
	 };
	 void add_to_original_nests_forwarding_here(Nest *nest);
	 void set_original_nests_forwarding_here(Sprites *nests) {
		 original_nests_forwarding_here = nests;
	 };
	 Nest *ultimate_forwarding_to();
	 Nest *ultimate_forwarding_to_or_original_nest();
	 void set_label(Text *new_label);
	 Sprite *respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected,
                                Sprite *by, boolean record_action, Sprite *original_recipient,
                                boolean size_constrained_regardless=FALSE);
	 ArrowKeyEditing arrow_key_editing_status();
	 Sprite *pointer_to_top_contents() {
		return(new_contents); // this is now maintained as the true top of stack even if stacks are being moved around or if popped
	 };
	 void set_new_contents(Sprite *item, boolean remove_from_floor_too=TRUE);
	 void set_to_future_value(Sprite *other, Sprite *original_recipient, Sprite *by); 
	 void merge(Nest *other_original); 
#if TT_DIRECT_PLAY
	 void set_nest_guid(GUID *initial_guid) {
		 nest_guid = initial_guid;
	 };
	 GUID *pointer_to_nest_guid() {
		 return(nest_guid);
	 };
	 void set_proxy(boolean new_flag) {
		 proxy_flag = (flag) new_flag;
	 };
	 boolean proxy() {
		 return(proxy_flag);
	 };
	 void set_dump_as_proxy(boolean new_flag) {
		 dump_as_proxy_flag = (flag) new_flag;
	 };
	 boolean dump_as_proxy() {
		 return(dump_as_proxy_flag);
	 };
//	 void set_player_address(bytes initial_player_address, int initial_address_size) {
//		 player_address = initial_player_address;
//		 player_address_size = initial_address_size;
//	 };
	 void set_addresses(string *initial_addresses, unsigned char initial_address_count) {
		 addresses = initial_addresses;
		 address_count = initial_address_count;
	 };
	 DPID return_player_id() {
		 return(player_id);
	 };
	 void player_id_assigned(DPID id, boolean host);
	 DPID regenerate_player_id();
    void assign_guid();
	 void become_networked();
	 void host_a_new_direct_play_session();
    LPDIRECTPLAY4A pointer_to_direct_play() {
		 return(direct_play);
	 };
	 void join_a_direct_play_session(); 
	 millisecond delay_before_another_attempt_to_join_session();
	 void send_pending_messages();
	 boolean ready_to_receive_messages();
	 void release_alternative_direct_plays();
	 void drop_direct_play_object();
	 void describe_networked_nest(output_stream &stream);
    void connect_to_nest_with_same_guid();
#endif
	 boolean equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, 
							SpritesPointer &expected_corresponding_nests, EqualReason reason=NORMAL_EQUAL);
	 Sprites *pointer_to_contents_stack() { // new on 110601 for equal_to
		 return(contents_stack);
	 };
//	 void become(Sprite *other);
	 Text *pointer_to_label() {
		 return(label);
	 };
	 void set_contents_stack(Sprites *stack);
#if TT_XML
//    xml_element *xml_create_element(xml_document *document);
    void xml(xml_element *element, xml_document *document);
	 void contents_stack_xml(xml_element *nest_element, xml_document *document);
	 xml_element *xml_set_special_attributes(xml_element *element, xml_document *document);
	 void add_special_xml(xml_element *element, xml_document *document);
    boolean handle_xml_on_top_tag(Tag tag, xml_node *node);
    boolean handle_nest_copies_tag(Tag tag, xml_node *node);
    boolean handle_xml_tag(Tag tag, xml_node *node);
	 short int default_parameter();
	 boolean xml_get_special_attributes(xml_node *node);
    boolean geometry_worth_saving();
	 wide_string xml_tag() {
		 return(L"Nest"); // new on 090703
	 };
 	// boolean ok_to_save_in_include_file() { // new on 190703
		// return(FALSE);
	 //};
#endif
	 void set_in_notebook(Notebook *notebook, boolean recur=TRUE);
	 boolean ok_to_cache_without_followers() { 
		 return(FALSE); // new on 261003 since otherwise sometimes the label is displayed incorrectly (when contents are selected)
	 };
	 boolean show_all_default() { // new on 170404 to shrink the XML a bit
       return(TRUE);
    };
	 void set_leader(Sprite *the_leader, boolean temporarily=FALSE);
	 void set_inside_vacuum(boolean new_flag, boolean recur=TRUE);
	 void set_active(boolean new_flag, boolean recur=TRUE, boolean initialization=FALSE) { 
		 // moved here on 140605 since was accidentally placed on PrimitiveBird
       // ignore attempts to inactivate birds and nests - might reconsider this some day
       // note that inactive birds currently animate very poorly
       active_flag = TRUE; // new on 171002 to counteract pretending to be inactive in XML loading
    };
	 void just_set_inactive() {
		 // ignore -- new on 140605
	 };
//    Bird *hatched_bird() {
//       return(bird_inside?((Bird *) contents):NULL);
//    };
  private:
	 Sprite *new_contents;
	 Sprites *contents_stack;
	 Robot *robot_to_try;
	 Cubby *cubby_to_try_robot_on;
//	 Sprite *saved_by;
    // above replaces saved_status on 171102
//	 ActionStatus *saved_status;
	 Nest *original_nest; // if I'm a copy of some other nest
	 Sprites *nest_copies;
	 Sprites *waiting_birds;
//	 Sprites *waiting_birds_tail; // noticed obsolete on 140205
    Bird *hatching_bird;
	 Nest *forwarding_to_nest; // new on 140205
	 Sprites *original_nests_forwarding_here; // new on 140205	 
	 Text *label; // new on 030399
//    boolean ready_to_receive_flag;
//    boolean just_covered;
#if TT_DIRECT_PLAY
	 GUID *nest_guid; // this is not the same concept as the Sprite's GUID - all nest copies should share the same nest_guid
	 LPDIRECTPLAY4A direct_play;
	 LPDIRECTPLAY4A *alternative_direct_plays; // if there is more than one connection in the address
	 DPID player_id;
//	 unsigned short int player_address_size;
//	 bytes player_address;
	 // replaced above with following on 050500
	 unsigned char address_count;
	 string *addresses;
	 Sprites *pending_messages;
	 short int pending_message_count;
	 int attempts_to_join_session;
	 short int nth_address; 
	 short int direct_play_count;
	 flag proxy_flag : 1;
	 flag dump_as_proxy_flag : 1; // new on 010500 - used when birds are dumping
#endif
	 // moved these around to give the compiler a chance to pack things better
	 flag deleted_flag : 1;
	 flag stack_temporarily_removed : 1;
	 flag display_new_contents : 1;
	 flag forwarding_flag : 1; // obsolete unless tt_old_nest_merging_behavior
	 int bird_count;
//    boolean egg_hatched : 1;
//    boolean bird_inside : 1;
//	 boolean covered : 1;
};

Sprites *copy_contents_stack(Sprites *contents_stack);

enum DeliveryStatus 
	  {NO_DELIVERY,
		HEADED_TO_NEST,
		SETTING_DOWN_MESSAGE,// obsolete - noticed 110203
		HEADED_BACK_TO_NEST_EMPTY_HANDED,
		SETTING_DOWN_STACK, // obsolete - noticed 110203
		HEADED_BACK_TO_MESSAGE,
		HEADED_BACK_TO_STACK,
		HEADED_BACK_TO_NEST,
		HEADED_BACK_TO_NEST_WITH_STACK,
		DELIVERY_COMPLETE_AND_RETURNING,
		HEADED_TO_NEST_ON_FLIPSIDE};

class Bird : public Sprite {
  public:
	 Bird(city_coordinate x=0, city_coordinate y=0,
			long priority=0,
//			Background *floor,
			Nest *nest=NULL,
			city_coordinate width=0,
			city_coordinate height=0,
         Sprite *t_shirt=NULL);
	 void release_all_references();
	 Sprite *copy(boolean also_copy_offsets=FALSE);
    boolean ok_to_delete();
    void java_before(java_stream &out);
    void java(java_stream &out);
    void java_after(java_stream &out, const_java_string variable);
	 boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
#if TT_XML
//    xml_element *xml_create_element(xml_document *document);
    void xml(xml_element *element, xml_document *document);
	 xml_element *xml_set_special_attributes(xml_element *element, xml_document *document);
	 void add_special_xml(xml_element *element, xml_document *document); 
//     boolean handle_xml_my_nest_tag(Tag tag, xml_node *node);
//     boolean handle_xml_label_tag(Tag tag, xml_node *node);
     boolean handle_xml_tag(Tag tag, xml_node *node);
	  boolean xml_get_special_attributes(xml_node *node);
     void prepare_nest_copy_for_xml(Nest *a_nest, NestPointer &one_nest_dumped_together_with);
	  wide_string xml_tag() {
		  return(L"Bird"); // new on 090703
	  };
#endif
    boolean to_windows(Pointer *data, short int &data_offset,
                       ascii_string types, short int &types_offset,
                       BirdPointer *bird, short int &bird_count);
	 void display(SelectionFeedback selection_feedback,
					  boolean followers_too, TTRegion *region);
//    void size_t_shirt();
	 void add_t_shirt(Sprite *picture);
    void t_shirt_region(TTRegion &region);
    void t_shirt_region_drop_region(TTRegion &region);
    void fix_t_shirt();
//	 void remove_t_shirt();
//	 void inside_thought_bubble();
    Sprite *select(TTRegion *region, SelectionReason reason, Sprite *in_hand);
	 Sprite *selected(SelectionReason reason, Sprite *by);
    void remove_selected_element(Sprite *element, SelectionReason reason, Sprite *, boolean add_to_floor);
	 Sprite *released(Sprite *by, boolean top_level=TRUE, SpecialDrop special_drop=NOT_SPECIAL_DROP);
	 void unselect();
 	 void generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection);
	 Sprite *used(Sprite *, Sprite *,
				     boolean , boolean ,
				     millisecond , 
				     boolean modified, Sprite *original_subject);
	 void build_nest(Sprite *by=NULL);
	 boolean create_nest(Sprite *by=NULL);
	 millisecond fly_to(city_coordinate x, city_coordinate y,
							  AnimationCallBack callback=CALLBACK_NOTHING,
							  millisecond minimum_duration=0,
							  boolean outside=FALSE);
    millisecond fly_to_goal(Sprite *goal_sprite, // city_coordinate *x, city_coordinate *y,
                            city_coordinate x_offset, city_coordinate y_offset,
                            AnimationCallBack callback, millisecond duration);
	 void grow_by_ten();
    Background *pointer_to_nests_background();
//	 void move_to_return_x_y() {
//		 move_to(return_x,return_y);
//	 };
//    void update_display();
	 ReleaseStatus item_released_on_top(Sprite *item, Sprite *by, SpritePointer &recipient, int level);
    boolean acceptable(Sprite *item);
	 void receive_item_again();
    void receive_new_item(Sprite *item);
	 boolean receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item);
	 void receive_item_each(Sprite *item, Sprite *by, millisecond duration); //  boolean last_one 
	 void shrink_to_tiny();
	 void leave_leader();
	 void set_return_cubby(Cubby *cubby);
//	 void leave_nest() {
//		 nest->remove_contents(this);
//	 };
	 void show_confusion();
	 void show_anticipation();
	 void adjust_size_and_location(city_coordinate &item_width,
											 city_coordinate &item_height,
											 city_coordinate &item_llx,
											 city_coordinate &item_lly);
	 void release_and_fly_away();
    void deliver_instantly();
	 void fly_to_nest();
	 void just_fly_to_nest(millisecond duration);
	 void fly_to_another_house();
	 void fly_outside();
	 void fly_outside_now_full_size();
    void fly_down();
	 boolean flying_outside();
    House *pointer_to_destination_house();
	 millisecond outside_flight_duration(city_coordinate destination_llx,
													 city_coordinate destination_lly,
													 millisecond minimum_duration=0);
	 void shrink_then_deliver_to_another_house();
	 void deliver_to_another_house();
	 void set_down_message();
	 void set_down_stack();
	 void pick_up_message();
	 void pick_up_stack();	 
	 void got_stack(Sprite *top);
	 void set_top_of_stack_on_nest(Sprite *item);
	 void release();
	 void put_stack_back();
	 void fly_back();
	 void really_go_back(boolean instantly=FALSE);
	 void returned_to_house(boolean instantly=FALSE);
	 void shrink_then_returned_to_house();
	 void return_to_cubby();
	 void return_to_cubby_on_flipside();
//    boolean nest_inside_rule_body();
	 void destroy_bird();
	 void destroy_item_and_return();
	 void release_bad_message(); // boolean last_action=TRUE);
	 void return_to_floor();
	 void show_nest_and_return();
	 void nest_is_made();
    void update_return_floor();
	 void set_inside_vacuum(boolean new_flag, boolean recur=TRUE);
    virtual Sprite *deliver_to_nowhere(short int desired_handle_index=-1);
    virtual boolean primitive() {
       return(FALSE);
    };
    virtual HANDLE return_handle() {
      return(NULL);
    };
//	 void set_parameter(short new_parameter);
	 void set_priority(long new_priority);
//	 void adjust_grasp(city_coordinate &adjustment_x,
//							 city_coordinate &adjustment_y);
//	 Sprite *pointer_to_controlled_sprite() {
//		 return(controlled_sprite);
//	 };
//	 void set_controlled_sprite(Sprite *new_controlled_sprite) {
//		 controlled_sprite = new_controlled_sprite;
//	 };
	 void set_no_home_to_return_to(boolean new_flag) {
		 no_home_to_return_to = (flag) new_flag;
	 };
	 void set_destroy_item(boolean new_flag) {
		 destroy_item_flag = (flag) new_flag;
	 };
	 void note_nest_building_finished();
    void morph_to_life();
	 void morph_to_inert();
	 boolean inert();
    void fix_drift();
	 Nest *pointer_to_nest() { 
		return(nest); 
	 };
	 void reset_nest(); // currently only used when discovering that a proxy is to a local nest
	 void set_nest(Nest *new_nest) {
		 nest = new_nest; // should just be another nest copy
	 };
	 void set_delivery_status(DeliveryStatus new_status) {
		 delivery_status = new_status;
	 };
    void good_size(city_coordinate &w, city_coordinate &h, Sprite *source=NULL, boolean for_expander=FALSE);
    void set_favorite_size() {
       favorite_width = width;
       favorite_height = height;
    };
    void set_size_and_location(city_coordinate width,
										 city_coordinate height,
										 city_coordinate llx,
										 city_coordinate lly,
										 ChangeReason reason=NO_REASON_GIVEN);
     Sprite *respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected,
                                 Sprite *by, boolean record_action, Sprite *original_recipient,
                                 boolean size_constrained_regardless=FALSE);
	  ArrowKeyEditing arrow_key_editing_status();
	  void set_item_to_deliver(Sprite *item);
	  void update_pointer_to_nest();
	  Sprite *now_inside_thought_bubble(boolean in_training);
	  Sprite *pointer_to_saved_by() { // new on 280201
		  return(saved_by);
	  };
	  boolean equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, SpritesPointer &expected_corresponding_nests, 
							 EqualReason reason=NORMAL_EQUAL);
     void set_active(boolean new_flag, boolean recur=TRUE, boolean initialization=FALSE) { // new on 220702
        // ignore attempts to inactivate birds and nests - might reconsider this some day
        // note that inactive birds currently animate very poorly
        active_flag = TRUE; // new on 171002 to counteract pretending to be inactive in XML loading
     };
	  void just_set_inactive() {
		 // ignore -- new on 140605
	  };
     long current_size_percent() { // new on 131102
        return(size_percent);
     };
     void just_set_size_percent(long new_percent) { // new on 131102
        size_percent = new_percent;
     };
     void set_return_floor(Background *new_floor);
//	  void action_completed();
	  //boolean ok_to_save_in_include_file() {
		 // return(FALSE);
	  //};
	  void shift_viewpoint(city_coordinate delta_x, city_coordinate delta_y); // new on 020204
	  boolean show_all_default() { // new on 170404 to shrink the XML a bit
       return(TRUE);
    };
#if TT_DEBUG_ON
	  void set_leader(Sprite *the_leader, boolean temporarily=FALSE);
#endif
//#if TT_DEBUG_ON
//	 void check_integrity();
//#endif
//	 void set_primitive(PrimitiveImplementation new_primitive) {
//		 primitive = new_primitive;
//	 };
  protected:
	 Nest *nest;
	 Sprite *item_to_deliver;
	 Sprite *top_of_stack_on_nest; // if stack is moved because nest full -- renamed on 161102 to avoid name collision with top_of_stack in Sprite 
	 DeliveryStatus delivery_status;
//	 ActionStatus *action_status; // made obsolete on 171102
	 flag no_home_to_return_to : 1;
	 flag destroy_item_flag : 1;
//	 flag t_shirt_added : 1;
//    flag dont_add_t_shirt : 1;
//	 boolean headed_to_nest : 1, item_set_down : 1;
	 // following to store where to return to after delivering message
	 Cubby *return_cubby;
	 Path *return_path;
	 Background *return_floor;
	 city_coordinate return_x,return_y;
//	 PrimitiveImplementation primitive;
//	 BuiltIn primitive;
//	 Sprite *controlled_sprite; // could subclass -- just for birds to sprites
	 Sprite *t_shirt; // sprite on shirt
	 millisecond saved_duration;
	 Sprite *saved_item;
	 Cubby *waiting_for;
//	 Sprite *saved_by;
//	 ActionStatus *saved_status; -- had both saved_status and action_status??
	 long size_percent;
	 flag size_is_100_percent : 1;
    flag global_picture_will_animate_me : 1;
    city_coordinate favorite_width, favorite_height;
};

class PrimitiveBird : public Bird {
  public:
	 PrimitiveBird(PrimitiveReceiver receiver=NULL, // added =NULL on 130802
//		            PrimitiveHelper helper=NULL,
//						PrimitiveDumper dumper=NULL,
//                  PrimitiveRestorer restorer=NULL,
		            HANDLE handle=NULL,
//						ascii_string label=NULL, // obsolete as of 140802
						PrimitiveBird *creator=NULL,
						Sprite *message=NULL,
						short int handle_index=0,
						unsigned short *handle_ref_count=NULL,
                  city_coordinate x=0, city_coordinate y=0,
			         long priority=0,
			         city_coordinate width=0,
			         city_coordinate height=0,
                  Sprite *t_shirt=NULL);
	 void release_all_references();
    Sprite *copy(boolean also_copy_offsets=FALSE);
    boolean to_windows(Pointer *data, short int &data_offset,
                       ascii_string types, short int &types_offset,
                       BirdPointer *bird, short int &bird_count);
	 Sprite *recreate_item(Pointer *data, 
								  short int &data_offset, ascii_string types, short int &types_offset,
								  Sprite *message, short int &handle_index, short int desired_handle_index);
	 boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
#if TT_XML
//   xml_element *xml_create_element(xml_document *document);
	  void xml(xml_element *element, xml_document *document);
     boolean handle_xml_tag(Tag tag, xml_node *node);
     void transfer_state_to(PrimitiveBird *other);
     void set_handle(HANDLE new_handle) {
        handle = new_handle;
     };
     void set_receiver(PrimitiveReceiver new_receiver) {
        receiver = new_receiver;
     };
     void set_message(Sprite *new_message) {
        message = new_message;
     };
     void set_creator(PrimitiveBird *new_creator) {
        creator = new_creator;
     };
     void set_handle_index(short int new_handle_index) {
        handle_index = new_handle_index;
     };
     void set_handle_ref_count(unsigned short int *new_handle_ref_count) {
        handle_ref_count = new_handle_ref_count;
     };
	  wide_string xml_tag() {
		  return(L"ForeignBird"); // new on 090703
	  };
#endif
	 void generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection);
	 PrimitiveBird *reconstruct(Sprite *message, short int desired_handle_index);
    Sprite *deliver_to_nowhere(short int desired_handle_index=-1);
    boolean primitive() {
       return(TRUE);
    };
    HANDLE return_handle() {
      return(handle);
    };
	 PrimitiveReceiver return_receiver() {
		 return(receiver);
	 };
	 void java_before(java_stream &out);	
	 void java(java_stream &out);	
	 void java_after(java_stream &out, const_java_string path);
 protected:
  PrimitiveReceiver receiver;
  HANDLE handle;
//  ascii_string label; // obsolete 140802
  PrimitiveBird *creator;
  Sprite *message; // to re-create
  short int handle_index; // was just char prior to 250299 - changed to allow more primitives
  unsigned short *handle_ref_count;
};

Nest *load_nest(InputStream *pad_in,
                int notebook_version, NaturalLanguage language,
					 Sprite *container, Picture *controlee, long index, boolean descendants_inactive);
Bird *load_bird(SpriteType type, InputStream *pad_in, int notebook_version, NaturalLanguage language, long index, Sprite *container, boolean descendants_inactive);
boolean describe_nest(InputStream *pad_in, output_stream &text_out, int version_number, 
						    boolean in_thought_bubble, Article article);
boolean describe_bird(SpriteType type,InputStream *pad_in, output_stream &text_out, int version_number, 
						    boolean , Article article);
void release_nest_indices();
//void release_nests_being_copied();

BOOL receive_extension_name(HANDLE handle, Pointer *data, ascii_string types, ascii_string label, ascii_string language, 
									 PointerPointer *out, ascii_string *out_types, ascii_string *to_say, BOOL *ok_to_speak, HINSTANCE string_library);

Picture *t_shirt_label(string label);

void create_bird_nest_pair(BirdPointer &bird, NestPointer &nest);

void java_declare_all_nests(java_stream &out);
void java_assign_all_nests(java_stream &out);

void release_non_networked_nests_with_guid();
void process_tt_nests_to_set_original_nest();

#if TT_BUG_FIXES
Sprites *push_non_networked_nests_with_guid();
void restore_non_networked_nests_with_guid(Sprites *sprites);
#endif

#if TT_XML
//void restore_nests_temporarily_dumping_as_proxies();
void do_postponed_nest_xml(xml_document *document);
void release_postponed_nest_xml();
void restore_saved_nest_guids();
void release_saved_nest_guids();
class PostponedNestXML { // new on 171002 to deal with nests that might be saved before birds who connect to them
   public:
      PostponedNestXML() { // to be filled in before use...
      };
      Nest *nest;
      xml_element *nest_element;
      xml_document *document;
};
class SavedNestGUID { // new on 181002 to deal with saving nest copies
   public:
      SavedNestGUID() { // to be filled in before use...
      };
      Nest *nest;
      GUID *nest_guid;
      flag first; // this one holds the reference to the new GUID so it can be released
};
#endif

#endif
