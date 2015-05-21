// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved
#if !defined(__TT_EVENT_H)

#define __TT_EVENT_H

#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif
class Robot;
class Cubby;
class Background;
class Floor;
class Notebook;

typedef Cubby *CubbyPointer;
// when changing remember to alter debugging names for these in event.cpp
enum EventCode {START_EVENT=0, // added =0 on 260802 since use this as array for generating XML
					 SELECT_STACK_ITEM,NEW_ITEM,
					 GRASP_COPIER, // obsolete
                COPIER_APPLY,
					 GRASP_EXPANDER, // obsolete
					 RELEASE_COPIER, // obsolete
					 GRASP_ITEM,RELEASE_ITEM,RELEASE_ITEM_ON,
					 RELEASE_ITEM_LEFT_OF,RELEASE_ITEM_RIGHT_OF,
					 EXPANDER_APPLY,
					 GRASP_VACUUM, // obsolete
                VACUUM_APPLY,
                VACUUM_APPLY_RESTORE,
                RELEASE_VACUUM, // obsolete
					 KEYBOARD_CHARACTER,
					 HATCH_BIRD, 
                GIVE_BIRD_ITEM,
					 DROP_ITEM_IN_TRUCK,
					 DESTROY_HOUSE,
					 APPLY_GRASPED_ITEM,
					 LAST_EVENT,
					 LABEL_CHARACTER,
					 KEYBOARD_CHARACTER_AND_SELECT,
					 GRASP_NEST, // unlike GRASP_ITEM won't take top element off stack
					 NEW_MAIN_CUBBY,
					 COPIER_APPLY_NEST,
					 VACUUM_APPLY_NEST,
                APPLY_GRASPED_NEST, // not used just now
                RELEASE_EXPANDER, // obsolete
                SERIAL_NUMBER, // of this sequence of events
					 NON_EVENT}; // used for range checking

enum EventArg {MAIN_CUBBY,NO_ARG,A_COPIER,A_VACUUM,
               INTEGER_STACK, TEXT_STACK,
					CUBBY_STACK, NEST_STACK, TRUCK_STACK, PROGRAM_PADS,
					ROBOT_STACK, SCALE_STACK, BOMB_STACK, ITEM_HELD,
					AN_EXPANDER, EXPANSION2, EXPANSION3, EXPANSION4, EXPANSION5,
					LAST_EVENT_ARG_TYPE}; // changing this breaks all saved programs

enum EventStatus {EVENT_NOT_STARTED, EVENT_STARTED, EVENT_DONE};

enum StepStatus {STEP_RAN_OK, STEP_WAITING,
                 STEP_HAD_DIFFICULTIES, STEP_TERMINATED_HOUSE};

// could have an event code of grasp, use, release, and release_on
// but hard to keep tracking of copied items, etc.

enum PathDereferenceStatus { 
	PATH_DEREFERENCE_OK,
	PATH_DEREFERENCE_WAIT,
	PATH_DEREFERENCE_ABORT // new on 281004 so sometimes robots abort and sometimes show confusion
};

class Path : public Entity {
  // an index into the list of freshly make objects
  // and a path if it is cubby
  public:
	 Path(int index, Path *next=NULL) :
		index(index),
		next(next) {};
	 ~Path();
	 PathDereferenceStatus dereference(Background *floor,
												  Robot *robot,
												  Cubby *top_cubby,
												  EventCode event_code,
												  SpritePointer &tool,
												  SpritePointer &holder,
												  long &last_index,  
												  millisecond &duration);
	 boolean dereference_level(Sprite *previous_level_item,
									   Background *floor,
									   Robot *robot,
									   Cubby *top_cubby,
									   EventCode event_code,
									   SpritePointer &tool,
									   SpritePointer &holder,
										boolean &dont_wait,
										long &last_index);
	 void dereference_pad(Notebook *notebook, Robot *robot, SpritePointer &tool, EventCode event_code);
	 void dereference_cubby(Cubby *cubby, SpritePointer &holder,	long &last_index, Sprite *by);
	 boolean dereference_cubby(Cubby *cubby, EventCode event_code,
										SpritePointer &sprite,
										SpritePointer &holder,
									   long &last_index,
										Sprite *by,
									   Robot *robot,
									   Cubby *top_cubby,
										Background *floor);
	 Path *skip_over_type_path();
	 void add_to_end(Path *new_end);
#if TT_DEBUG_ON
    void debug(boolean first);
#endif
    void kind_is(SpriteType &kind);
    void java(java_stream &out, boolean first=TRUE, const_java_string set_new_value=NULL, const_java_string set_mode=NULL);
	 boolean contains_special_path_marker();
    void describe(output_stream &text_out, SpriteType type);
	 void dump(output_stream &stream, boolean warn=TRUE);
#if TT_XML
   boolean xml_path_string(wide_string path_string, int location, int max_location, boolean first=TRUE);
#endif
    boolean top_level() {
       return(next == NULL);
    };
    int return_index() {
      return(index);
    };
    void set_index(int new_index) {
       index = new_index;
    };
    Path *pointer_to_next() {
      return(next);
    };
	 void set_next(Path *new_next) {
		 next = new_next;
	 };
	 boolean references_dusty() { // new on 141100
		 return(index == A_VACUUM);
	 };
	 boolean references_copier() { // new on 140101
		 return(index == A_COPIER);
	 };
	 boolean equal_to(Path *other);
    unsigned long path_hash(int location=0);
  private:
    int index;
    Path *next;
};

class Events;

class Event : public Entity {
  public:
	 Event(EventCode code, Path *subject_path, int key=0) : // was char key but as of 100604 we encode extended by adding 256
		code(code),
		subject_path(subject_path),
		key(key) {};
    ~Event();
	 StepStatus run(Background *floor, Robot *robot, Event *next_event); //, ActionStatus *action_status);
	 boolean event_sends_character_to_same_subject(Event *other_event);
    boolean java(java_stream &out, Events * &events, Robot *robot, SpriteTypePointer &working_set_types, int &working_set_size);
    short int serial_number();
#if TT_DEBUG_ON
	 void debug(Robot *robot=NULL);
#endif
	 boolean dump(output_stream &stream);
#if TT_XML
   void xml(xml_element *parent, xml_document *document);
#endif
	 boolean is_first() {
		 return(code == START_EVENT);
	 };
    Path *pointer_to_path() {
      return(subject_path);
    };
    EventCode event_code() {
       return(code);
    };
	 void set_event_code(EventCode new_code) { // new on 090701 for undoing Bammer while training
		 code = new_code;
	 };
	 int robot_version_number(); // really only one byte
	 boolean equal_to(Event *other);
    unsigned long event_hash();
  private:
	 EventCode code;
	 Path *subject_path;
	 int key; // also used for modified flag
};

Event *create_serial_number(int serial_number=-1, int version_number=latest_robot_version);
Event *create_first_event(int max_working_set_size);
Event *create_last_event();

class Events : public Entity {
  public:
    Events(Event *event, Events *next=NULL);
    ~Events();
    Events *run(Background *floor, Robot *robot);
    Events *remove_last();
	 void insert_at_end(Event *last_event);
	 boolean decrement_ref_count() {
      ref_count--;
      return(ref_count == 0);
    };
    void increment_ref_count() {
		ref_count++;
	 };
    void set_ref_count(int new_ref_count) {
       ref_count = new_ref_count;
    };
    int current_ref_count() {
       return(ref_count);
    };
#if TT_DEBUG_ON
    void debug();
#endif
    void java(java_stream &out, Robot *robot);
    short int serial_number();
    boolean describe(output_stream &text_out);
	 void dump(output_stream &stream);
#if TT_XML
   void xml(xml_element *parent, xml_document *document);
#endif
	 Event *first() {
		 return(event);
	 };
	 void set_first(Event *new_first) {
#if TT_DEBUG_ON
		 if (new_first == NULL) {
			 tt_error_file() << "Debug this -- NULL event" << endl;
		 };
#endif
		 event = new_first;
	 };
	 Events *rest() {
		 return(next);
	 };
	 void set_rest(Events *new_next) {
		 next = new_next;
	 };
	 int robot_version_number() {
		 return(event->robot_version_number());
	 };
	 boolean equal_to(Events *other);
	 Events *last(); // new on 090701
    unsigned long events_hash(int counter=0);
#if TT_DEBUG_ON
	 flag deleted; // new on 080604 for debugging
#endif
  private:
	 Event *event;
	 Events *next;
    int ref_count;
};

class Queue;
typedef Queue *QueuePointer;

class Queue : public Entity {
  public:
	 Queue(Events *events, Robot *robot, Queue *next=NULL);
	 ~Queue();
    void begin_run(QueuePointer &queue_pointer);
//	 void run(QueuePointer &queue_pointer, millisecond until_time);
	 void insert_at_end(Queue *end);
	 void move_on_screen_to_front();
	 Queue *rest() {
		 return(next);
	 };
	 void first_and_rest(QueuePointer &first, QueuePointer &rest);
	 void insert_second(Queue *new_second);
	 void set_rest(Queue *new_next) {
		 next = new_next;
	 };
	 boolean first_is_on_a_screen();
	 void remove_items_of_robot(Robot *robot);
	 boolean robot_already_in_queue(Robot *a_robot);
	 Robot *robot_in_queue_with_cubby(Cubby *cubby);
    void add_robots_to_flipped_followers(Picture *picture);
	 void remove_events();
	 Robot *pointer_to_robot() {
		 return(robot);
	 };
	 void set_robot(Robot *initial_robot);
//    void remove_pointer_to_robot() {
//       robot = NULL;
//    };
	 Events *pointer_to_events() {
		 return(events);
	 };
	 void set_events(Events *new_events) {
		 events = new_events;
	 };
#if TT_XML
    xml_element *xml(xml_document *document);
    boolean handle_xml_tag(Tag tag, xml_node *node);
#endif
	 void dump(output_stream &stream);
	 boolean empty() { // new on 100603
		 return(events == NULL && next == NULL);
	 };
//	 Cubby *pointer_to_cubby() {
//		return(top_cubby);
//	 };
//	 Background *pointer_to_floor() {
//		return(floor);
//	 };
#if TT_DEBUG_ON
	 int length();
#endif
  private:
	 Events *events;
//	 Background *floor;
//	 Cubby *top_cubby;
//    Sprites *working_set;
	 Queue *next;
	 Robot *robot;
#if TT_DEBUG_ON
    flag deleted;
	 int debug_counter; // new on 071100
#endif
};

Queue *run(Queue *q, millisecond until_time);

void combine_queues(Queue *first, Queue *second);

Queue *load_queue(InputStream *stream, int version_number, NaturalLanguage langauge);

class Suspensions : public Entity {
  public:
	 Suspensions(//Events *events, Background *floor // simplified on 120104	
					 Robot *robot, SpritePointerPointer cell);
	 Suspensions() :
//		events(NULL), // simplified on 120104
		robot(NULL),
		cell(NULL),
		next(NULL),
		previous(NULL),
#if TT_DEBUG_ON
		deleted(FALSE),
#endif
		first_in_list(TRUE) {};
	 ~Suspensions();
	 void add(Suspensions *new_item);
	 void remove();
	 void delete_all();
	 void retry(Sprite **cell_receiving_item);
	 void remove_if_robot_is(Robot *target_robot);
	 boolean remove_and_stop_if_different_robot_waiting_on_same_cell(Robot *new_robot, SpritePointerPointer new_cell);
	 boolean is_robot_waiting_for_programmer(Robot *target_robot);
//#if TT_XML
//	 xml_element *xml(xml_document *document);
//    boolean handle_xml_tag(Tag tag, xml_node *node);
//#endif
	 void set_previous(Suspensions *new_previous) {
		 previous = new_previous;
	 };
	 void set_next(Suspensions *new_next) {
		 next = new_next;
	 };
	 Suspensions *pointer_to_next() {
		 return(next);
	 };
	 Robot *pointer_to_robot() {
		 return(robot);
	 };
  private:
//	 Events *events; // simplified on 120104
//	 Background *floor; // simplified on 120104
	 Robot *robot;
	 SpritePointerPointer cell;
	 Suspensions *next;
	 Suspensions *previous;
	 boolean first_in_list;
#if TT_DEBUG_ON
	 boolean deleted;
#endif
};

#if TT_DEBUG_ON
const_string event_string(EventCode code);
#endif

void pick_location(Robot *robot, city_coordinate &subject_llx, city_coordinate &subject_lly);
millisecond duration_delta_location(city_coordinate current_llx,
												city_coordinate current_lly,
												city_coordinate goal_llx,
                                    city_coordinate goal_lly);
//Event *create_first_event();
//void record_event2(EventCode code, Sprite *subject, long parameter);
//Event *record_event(EventCode code, Sprite *subject);
void body_defined();

Events *load_events(InputStream *pad_in, int notebook_version);
Path *load_path(EventCode event_code, InputStream *pad_in, int notebook_version);
Path *load_rest_of_path(EventCode event_code, InputStream *pad_in);

boolean describe_body(InputStream *pad_in, output_stream &text_out);
void describe_event(EventCode event_code, int key, output_stream &text_out,
                    unsigned char event_arg);
void describe_event_arg(unsigned char event_arg, EventCode event_code,
								SpriteType *working_set_types, int working_set_size,
								InputStream *pad_in, output_stream &text_out);
SpriteType type_of_event_arg(EventArg arg);
Sprite *need_to_wait(EventCode event_code, Sprite *next_level, boolean last_in_path, 
							boolean &wait, boolean &done);
Sprite *recursively_dereference_nest(Sprite *next_level, EventCode event_code,
												 boolean &wait);
void reset_events_counters();
void reset_current_queue();

#if TT_XML
Path *xml_decode_event_path(xml_node *node, SpriteType &path_type);
//boolean handle_actions_tag(Entity *robot, Tag tag, xml_node *node, void *extra_info);
Events *xml_load_events(xml_node *node, boolean whole_body, Robot *robot);
#endif
#endif
