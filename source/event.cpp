// Copyright (c) 1992-2003.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif   
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif   
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif   
#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif   
#if !defined(__TT_NUMBER_H)   
#include "number.h"
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
#if !defined(__TT_BIRD_H)   
#include "bird.h"
#endif   
#if !defined(__TT_TRUCK_H)   
#include "truck.h"
#endif   
#if !defined(__TT_THOUGHT_H)   
#include "thought.h"
#endif   
#if !defined(__TT_BOMB_H)   
#include "bomb.h"
#endif   
#if !defined(__TT_PAD_H)   
#include "pad.h"
#endif   
#if !defined(__TT_FLOOR_H)   
#include "floor.h"
#endif   
#if !defined(__TT_HELP_H)   
#include "help.h"
#endif   
#if !defined(__TT_LOG_H)   
#include "log.h"
#endif   
#if !defined(__TT_CLEANUP_H)   
#include "cleanup.h"
#endif   
#if !defined(__TT_EVENT_H)   
#include "event.h"
#endif
#if !defined(__TT_JAVA_H)
#include "java.h"
#endif
#if !defined(__TT_PRGRMMR_H)   
#include "prgrmmr.h"
#endif
#if !defined(__TT_MARTIAN_H)   
#include "martian.h"
#endif
#if TT_XML
#if !defined(__TT_XML_H)
#include "xml.h"
#endif
#endif

//#include <strstrea.h> commented out 210703
#if !TT_32
#include <stdlib.h> // for abs
#endif

// Windows Header Files:
#include <windows.h>
#include <mmsystem.h>

#include <wchar.h> // for wmemcpy

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int last_event_arg = -1;
int last_temp_variable_index = 0; // new on 260700
#if TT_POST326
int temp_variables_count = 0; // new on 121103 since Spread Out in grapher was an example of where last_temp_variable_index is reset and then incremented
#endif

boolean describe_path(unsigned char i,
                      EventCode event_code, InputStream *pad_in, output_stream &text_out,
							 SpriteType type, boolean first_time=TRUE, boolean type_path=FALSE);
boolean describe_path(EventCode event_code, InputStream *pad_in, output_stream &text_out,
						    SpriteType type, boolean first_time=TRUE, boolean type_path=FALSE);

const_string event_string(EventCode code) {
   return(S(IDS_DEBUG_START_EVENT+code));
};

boolean event_arg_is_stack(int index) {
	return(index > 3 && index != 9 && index < 13);
};

boolean no_gender() {
	return(S(IDS_ENGLISH_LIKE_ADJECTIVES_AND_PRONOUNS)[0] == '1');
};

Events::Events(Event *event, Events *next) :
      event(event),
      next(next),
#if TT_DEBUG_ON
		deleted(FALSE),
#endif
		ref_count(1)   
		{};

Events::~Events() {
#if TT_DEBUG_ON
	if (deleted) debug_this();
	if (tt_debug_mode == 777) {
		deleted = TRUE;
		return;
	};
#endif
   delete event;
   if (next != NULL && next->decrement_ref_count()) {
      delete next;
	};
};

Events *Events::run(Background *floor, Robot *robot) {
#if TT_DEBUG_ON
	if (robot->debug_counter == tt_debug_target) {
		tt_error_file() << "Debug target running events " << endl;
	};
#endif
	// these are in robot so the body can be shared between robots
	if ((robot->suspended() || robot->waiting_for_release())) { // && tt_log_version_number > 14) { 
		// moved here from if (*started) branch on 290699 so suspended robots don't run if they get this far
		// robot suspension check added on 260399
		// || robot->waiting_for_release() added on 120499 to prevent robots from sometimes running on the back of flipped pictures
		if (event != NULL && event->event_code() == SERIAL_NUMBER) { 
			// new on 141100 - if haven't started and suspended then switch this over to tt_running_robots
			if (tt_running_robots == NULL || tt_running_robots->member_cons(robot) == NULL) 
				// condition new on 241003 to avoid duplicates
			tt_running_robots = new Sprites(robot,tt_running_robots);
			return(NULL);
		} else {
			return(this);
		};
	};
//	ActionStatus *action_status = robot->pointer_to_body_status();
   // commented out on 171102 since action_status doesn't save well (e.g. in cities)
//	flag *robot_finished = robot->pointer_to_true_when_body_finished();
//	flag started = robot->body_started(); // was *started prior to 171102
	Events *remaining = this;
	while (TRUE) {
		if (robot->body_started()) {
			if (robot->is_action_in_progress()) {
//#if TT_DEBUG_ON -- can happen when waiting for a bird for example
//            if (!robot->showing_on_a_screen()) {
//               log("Off-screen robot waiting for something to finish his action.");
//            };
//#endif
				return(remaining);
			} else {
				robot->set_body_started(FALSE);
				if (robot->is_action_aborted()) return(NULL);
				if (remaining->rest() != NULL) {
//					robot->reset_action_status(); // commented out on 271102 since happens again next iteration
					remaining = remaining->rest();
				} else {
					robot->done_running_body(TRUE);//top_cubby);
					return(NULL); // completely done so remove me from queue
				};
			};
		} else {
			if (tt_log_version_number >= 60) { 
				// condition new on 311204 to fix demo/time travel problems -- not clear if any demos broke without this
				robot->finish_instantly(); 
				// new on 221104 since may be busy with put_robot_behind_me and that may trigger a completion event
			};
			robot->set_body_started(TRUE);
			robot->reset_action_status();
			Event *this_event = remaining->first();
			StepStatus step_status;
			if (robot->pointer_to_background() == NULL) { // new on 100400 - don't run without a background
				robot->completed();
            return(NULL); // added on 050602 since otherwise step_status has no value below
			} else {
				Event *next_event;
				if (remaining->rest() != NULL) {
					next_event = remaining->rest()->first();
				} else {
					next_event = NULL;
				};
				step_status = this_event->run(floor,robot,next_event);
			};
			// fold following in event::run ??
			if (robot->is_action_aborted()) {
				step_status = STEP_HAD_DIFFICULTIES;
			};
			switch (step_status) {
				case STEP_RAN_OK:
				  if (robot->is_completed()) {
//					  return(run(floor,top_cubby,robot));
					  break; // loop around and try again 
				  } else { //break; // do nothing
					  return(remaining);
				  };
				case STEP_WAITING:
				  robot->set_body_started(FALSE); // pretend haven't done this yet
				  return(remaining); 
				case STEP_HAD_DIFFICULTIES: // probably programmer changed the cubby
				  robot->set_body_started(FALSE);
				  robot->action_aborted();
				  if (tt_log_version_number >= 71 && robot != tt_programmer->pointer_to_appearance()) { 
					  // new on 130205 -- second condition added on 060305 so if just stopped retraining this doesn't happen
					  robot->move_to_side_of_box(robot->default_duration(1000));
					  robot->forget_tool();
				  };
				  return(NULL);
				case STEP_TERMINATED_HOUSE:
				  return(NULL);
			};
		};
	};
};

Events *Events::remove_last() {
	if (next == NULL) {
      delete this;
      return(NULL);
   } else {
      next = next->remove_last();
		return(this);
   };
};

Events *Events::last() {
	Events *remaining = this;
	while (remaining->rest() != NULL) {
		remaining = remaining->rest();
	};
	return(remaining);
};

#if TT_DEBUG_ON
void Events::debug() {
	event->debug();
	if (next != NULL) next->debug();
};
#endif

short int Events::serial_number() {
   return(event->serial_number());
};

// move to utils soon?
unsigned long xor_at(int location, unsigned long n1, unsigned long n2) {
   // xors n2 into n1 beginning at location and continues around to the other side
   unsigned long n3 = (n1<<location)^n2;
   return((n1>>(sizeof(unsigned long)-location))^n3); // updated 250702 to shift 32-location
};

unsigned long Events::events_hash(int counter) { // new on 240702
   // generates a 32-bit hash that minimizes the chance of collisions -- new on 240702
   // each event xors a 32-bit representation of itself in the hash computed "so_far"
   // to spread the "wealth" the location of where the xor begins (it wraps) increments each time around
   if (next == NULL) {
      return(event->event_hash());
   } else {
      return(xor_at(counter,event->event_hash(),next->events_hash((counter+1)%sizeof(unsigned long))));
   };
};

Event *previous_event = NULL; // new on 141100 since sometimes need to know (e.g. releasing box on number)
Event *previous_previous_event = NULL;

//short int temp_index = 1;
SpriteType hand_kind = NONE_GIVEN;
flag copier_in_hand_unattached = TRUE;
flag new_main_cubby = FALSE;
flag vacuum_created = FALSE;
flag copier_created = FALSE;

void Events::java(java_stream &out, Robot *robot) {
	copier_in_hand_unattached = TRUE; // new on 301100 since don't want state from previous robot to affect this
   last_event_arg = -1;
	last_temp_variable_index = 0;
	temp_variables_count = 0; // new on 121103
	SpriteTypePointer working_set_types = NULL;
	previous_event = NULL;
	previous_previous_event = NULL;
   Events *remaining = this;
	int working_set_size; // new on 040304
   while (remaining != NULL) {
		Event *current_event = remaining->first();
		remaining = remaining->rest(); // moved here on 301100 so event::java can skip events
      if (!current_event->java(out,remaining,robot,working_set_types,working_set_size)) break;
		previous_previous_event = previous_event;
		previous_event = current_event;
//      remaining = remaining->rest();
   };
   if (working_set_types) {
      delete [] working_set_types;
   };
};
      
boolean Events::describe(output_stream &text_out) {
#if TT_NEW_IO
	output_string_stream body_encoding;
	dump(body_encoding);
	input_string_stream body_in(body_encoding.str());
	return(describe_body(body_in,text_out));
#else
	character *buffer = new char[30000];
	output_string_stream body_encoding(buffer,30000);
	dump(body_encoding);
	StringInputStream body_in(buffer,body_encoding.LENGTH);
	boolean result = describe_body(&body_in,text_out);
	delete [] buffer;
	return(result);
#endif
};

void Events::dump(output_stream &stream) {
	if (event->dump(stream)) {
		if (next != NULL) next->dump(stream);
	};
};

#if TT_XML

//#if TT_DEBUG_ON
//Events *previous_remaining = NULL;
//#endif

void Events::xml(xml_element *parent, xml_document *document) { // args re-ordered on 011102 to be consistent
//   xml_element *event_element = 
	event->xml(parent,document);
   Events *remaining = next;
   while (remaining != NULL) {
//      event_element = 
      remaining->first()->xml(parent,document); // ,event_element);
//#if TT_DEBUG_ON
//      if (remaining->first()->event_code() == NEW_ITEM && remaining->rest()->first()->event_code() == NEW_ITEM) {
//         log("debug this");
//      };
//      previous_remaining = remaining;
//#endif
      remaining = remaining->rest();
   };
};

#endif

boolean Events::equal_to(Events *other) {
	if (other == NULL) return(FALSE);
	if (!first()->equal_to(other->first())) return(FALSE);
	if (rest() == NULL) {
		if (other->rest() == NULL) {
			return(TRUE);
		} else {
			return(FALSE);
		};
	} else {
		return(rest()->equal_to(other->rest()));
	};
};

Events *load_events(InputStream *pad_in, int notebook_version) {
	Path *path;
	Event *new_event;
	Events *body = NULL;
	Events *current_events = NULL;
	while (!pad_in->empty()) {
		unsigned char token = (unsigned char) pad_in->get();
      if (token == END_OF_ROBOT_MARKER) return(body); // no body (or loading tt_events since saved while training)
      EventCode event_code = (EventCode) token;
		switch (event_code) {
//			case KEYBOARD_CHARACTER: // has key
//			case LABEL_CHARACTER: // has key
//			case KEYBOARD_CHARACTER_AND_SELECT:
//				path = load_path(event_code,pad_in);
//				new_event = new Event(event_code,path,(unsigned char) pad_in->get());
//				break;
			case LAST_EVENT:
				current_events->set_rest(new Events(create_last_event()));
				if (body == NULL) { 
				   // there is a difference between a body that does nothing and no body at all
					body = current_events;
				};
				return(body);
			default:
				if (event_code >= NON_EVENT) {
					say_error(IDS_ROBOT_FILE_BAD,TRUE);
					return(NULL);
				};
				path = load_path(event_code,pad_in,notebook_version);
				int key = pad_in->get(); // new on 160205
				if (key == 255) {
					key += pad_in->get()+1;
				};
				new_event = new Event(event_code,path,key); // (char) pad_in->get());
//#if TT_DEBUG_ON
//				if (tt_debug_mode == 290301 && (event_code == KEYBOARD_CHARACTER || event_code == KEYBOARD_CHARACTER_AND_SELECT)) {
//					log("edit new_event");
//				};
//#endif
		};
		Events *new_current_events = new Events(new_event);
		if (current_events != NULL) {
			current_events->set_rest(new_current_events);
			if (body == NULL) {
				body = current_events;
			};
		};
		current_events = new_current_events;
	};
	say_error(IDS_ROBOT_TRAINING_LOST,TRUE);
	tt_error_file() << (pad_in->tellg()) << " is how far it got in the encoding or file." << endl;
	if (body != NULL) delete body;
	return(NULL);
};

boolean describe_body(InputStream *pad_in, output_stream &text_out) {
	last_event_arg = -1;
	boolean first = TRUE;
	SpriteType *working_set_types = NULL;
	int working_set_size = 0; // new on 040304 for robustness
	while (!pad_in->empty()) {
		unsigned char code = (unsigned char) pad_in->get();
		if (code >= NON_EVENT) {
			log(S(IDS_CONFUSED_ABOUT_ROBOT_DESCRIPTION));
//			say_error(IDS_ROBOT_FILE_BAD,TRUE); // commented out since confusing -- 040304
			return(FALSE);
		};
		EventCode event_code = (EventCode) code;
		if (event_code == LAST_EVENT) {
			if (first) { // empty body
				text_out << S(IDS_DO_NOTHING);
			};
			break;
		};
		unsigned char event_arg = (unsigned char) pad_in->get();
		if (event_code == START_EVENT) {
			working_set_size = event_arg; // new on 040304
			if (event_arg > 0) {
				working_set_types = new SpriteType[event_arg];
				for (int i = 0; i < event_arg; i++) {
					working_set_types[i] = NONE_GIVEN;
				};
			};
		} else if (event_code == NEW_ITEM) {
			if (working_set_types == NULL) { // new error checking as of 090799
				log(AC(IDC_NO_WORKING_SET_TYPES));
				say_error(IDS_ROBOT_FILE_BAD,TRUE);
				return(FALSE);
			};
			working_set_types[event_arg] = (SpriteType) pad_in->get();
			if ((unsigned char) pad_in->get() != END_OF_PATH_MARKER) {
				say_error(IDS_ROBOT_FILE_BAD,TRUE);
//				tt_error_file() << "Bad path on NEW_ITEM event "
//								<< event_arg << " "
//								<< type_string(working_set_types[event_arg-1]) << endl;
			};
      } else if (event_code == SERIAL_NUMBER) {
         // nothing to say about this -- used internally
//         pad_in->get(); // half of serial number
			while (TRUE) {
				if ((unsigned char) pad_in->get() == END_OF_PATH_MARKER) { // rewritten on 161000
					break;
				};
			};
			// replaced following with above on 131000 since now contains robot_version_number
//			if ((unsigned char) pad_in->get() != END_OF_PATH_MARKER) {
//				say_error(IDS_ROBOT_FILE_BAD,TRUE);
//				return(FALSE);
//         };
		} else if (first) {
			first = FALSE;
		} else {
			text_out << end_sentence() << S(IDS_AND_BEGIN_SENTENCE) << space();
		};
      if (event_code >= NON_EVENT) {
         log(S(IDS_CONFUSED_ABOUT_ROBOT_DESCRIPTION));
			say_error(IDS_ROBOT_FILE_BAD,TRUE);
         return(FALSE);
      };
      if (event_code != NEW_ITEM && event_code != SERIAL_NUMBER) {
			// need to read path before reading character if SEND_TO
			string buffer = new character[256];
			output_string_stream event_arg_stream(buffer,256);
			describe_event_arg(event_arg,event_code,working_set_types,working_set_size,pad_in,event_arg_stream);
			event_arg_stream.put('\0');
			int key = pad_in->get(); // was char prior to 160205
			if (key == 255) {
				key += pad_in->get()+1;
			};
			describe_event(event_code,key,text_out,event_arg);
			text_out << buffer;
			delete [] buffer;
		} else {
			pad_in->get(); // ignore "key"
		};
	};
	if (working_set_types != NULL) {
		delete [] working_set_types;
	};
	return(TRUE);
};

boolean holding_copier_in_description = FALSE;
boolean last_event_was_a_grasp_or_use = FALSE;

void describe_event(EventCode event_code, int key, output_stream &text_out,
						  unsigned char event_arg) {
	if (event_code == START_EVENT || event_code == SERIAL_NUMBER) return;
	if (key != 0 && (event_code == VACUUM_APPLY || event_code == VACUUM_APPLY_NEST)) {
		text_out << S(IDS_VACUUM_TO_BLANK);
	} else if (key != 0 && (event_code == COPIER_APPLY || event_code == COPIER_APPLY_NEST)) {
		text_out << S(IDS_SPECIAL_MAGIC_WAND_USE);
	} else {
		text_out << S(IDS_FIRST_EVENT_CODE+event_code*2);
	};
	const_string second_part = NULL; //event_description[event_code*2+1];
	switch (event_code) {
		case RELEASE_ITEM_ON:
		case RELEASE_ITEM_LEFT_OF:
		case RELEASE_ITEM_RIGHT_OF:
		case GIVE_BIRD_ITEM:
		case DROP_ITEM_IN_TRUCK:
      case EXPANDER_APPLY:
			second_part = S(IDS_FIRST_EVENT_CODE+event_code*2+1);
			break;
		 case APPLY_GRASPED_ITEM:
		 case APPLY_GRASPED_NEST:
			second_part = space();
			break;
	};
	if (second_part != NULL) { // something more to say
		if (holding_copier_in_description) {
			// could with some work keep a copy of a description of the arg
			// of the last COPIER_APPLY and say the copy of ... instead
			text_out << space() << S(IDS_WHAT_I_JUST_COPIED) << second_part;
      } else if (event_code == EXPANDER_APPLY) {
         text_out << second_part << space() 
                  << decode_duration(key) << space() << S(IDS_MILLISECONDS)
                  << space() << S(IDS_ON);
		} else if (last_event_was_a_grasp_or_use && no_gender()) {
         // "it" is a problem for most other languages so punt
			text_out << space() << S(IDS_IT); 
			if (second_part[0] != ' ' || second_part[1] != '\0') text_out << second_part;
		} else {
			text_out << space() << S(IDS_WHAT_IM_HOLDING);
			if (second_part[0] != ' ' || second_part[1] != '\0') text_out << second_part;
		};
	};
	if (event_code == GRASP_COPIER) {
		holding_copier_in_description = TRUE;
	} else if (event_code == RELEASE_COPIER) {
		holding_copier_in_description = FALSE;
	};
	long next_id = 0; // if needed
	if (key != 0) {
		if (event_code == KEYBOARD_CHARACTER) {
//			text_out << S(IDS_THE_LETTER);
         text_out << space();
			describe_key_stroke(key,text_out);
			if (event_arg == ITEM_HELD || holding_copier_in_description) {
				text_out << S(IDS_KEY_WHILE_HOLDING);
				next_id = IDS_KEY_WHILE_HOLDING2;
			} else {
				text_out	<< S(IDS_KEY_WHILE_POINTING);
				next_id = IDS_KEY_WHILE_POINTING2;
			};
		} else if (event_code == KEYBOARD_CHARACTER_AND_SELECT) {
//			text_out << S(IDS_THE_LETTER);
         text_out << space();
			describe_key_stroke(key,text_out);
			if (event_arg == ITEM_HELD || holding_copier_in_description) {
				text_out << S(IDS_KEY_WHILE_HOLDING);
				next_id = IDS_KEY_WHILE_HOLDING2;
			} else {
			  text_out << S(IDS_KEY_AFTER_MOVING);
			  next_id = IDS_KEY_AFTER_MOVING2;
			};
		} else if (event_code == LABEL_CHARACTER) { // nothing else now
			text_out << space();
//			text_out << S(IDS_THE_LETTER);
			describe_key_stroke(key,text_out);
			text_out	<< S(IDS_TO_LABEL_OF); // << space();
			next_id = IDS_TO_LABEL_OF2;
		};
	};
	switch (event_code) {
		case GRASP_ITEM:
		case GRASP_NEST:
		case APPLY_GRASPED_ITEM:
		case APPLY_GRASPED_NEST:
		case SELECT_STACK_ITEM:
		case KEYBOARD_CHARACTER:
		case KEYBOARD_CHARACTER_AND_SELECT:
		case LABEL_CHARACTER:
			last_event_was_a_grasp_or_use = TRUE;
			break;
		default:
			last_event_was_a_grasp_or_use = FALSE;
	};
	if (event_code == RELEASE_ITEM) {
      if (no_gender()) {   
         // "it" is a problem for most other languages so punt
			text_out << space() << S(IDS_IT); 
		} else {
			text_out << space() << S(IDS_WHAT_IM_HOLDING);
	   };
   };
	if (next_id != 0) {
		text_out << S(next_id,"");
	};
};

int total_arg_count = 0;

void describe_event_arg(unsigned char event_arg, EventCode event_code,
								SpriteType *working_set_types, 
								int working_set_size, // added for error checking on 040304
								InputStream *pad_in, output_stream &text_out) {
	unsigned char i = (unsigned char) pad_in->get();
	boolean more_path = (i != END_OF_PATH_MARKER);
	if ((event_code == GIVE_BIRD_ITEM ||
		  event_code == DROP_ITEM_IN_TRUCK) &&
		 (event_arg != 1)) { 
		if (more_path) {
			text_out << space() << S(IDS_IN,""); // ,"" added 071299
		} else {
			text_out << S(IDS_WHICH_IS);
		};
	};
	if (event_arg < LAST_EVENT_ARG_TYPE || event_code == START_EVENT || event_code == SERIAL_NUMBER) {
		if (event_code == START_EVENT) total_arg_count = event_arg;
		if (describe_path(i,event_code,pad_in,text_out,type_of_event_arg((EventArg) event_arg))) {
			// completely described it
			last_event_arg = event_arg; // I think?
			return;
		};
		if (event_code != START_EVENT && event_code != SERIAL_NUMBER &&
			 event_code != RELEASE_ITEM && // was missing...
			 event_code != GRASP_COPIER &&
			 event_code != RELEASE_COPIER &&
			 event_code != GRASP_VACUUM &&
			 event_code != VACUUM_APPLY_RESTORE &&
			 event_code != RELEASE_VACUUM &&
          event_code != GRASP_EXPANDER &&
          event_code != RELEASE_EXPANDER)  {
			if (event_arg != NO_ARG) {
				text_out << space() << S(IDS_EVENT_ARG1+event_arg);
				last_event_arg = event_arg;
			};
		};
	} else {
		if (working_set_types == NULL) { // new error checking as of 090799
			log(AC(IDC_NO_WORKING_SET_TYPES),FALSE,TRUE);
			return;
		};
		event_arg -= (unsigned char) LAST_EVENT_ARG_TYPE;
//		event_arg = -event_arg;
		SpriteType type = NONE_GIVEN; // default value new on 040304
		if (event_arg <= working_set_size) { // conditional new on 040304
			type = working_set_types[event_arg-1];
		} else { // something wrong
			return; // new on 040304
		};
		long next_id = 0;
		if (type == NEST && event_code == GRASP_ITEM) {
			text_out << space() << S(IDS_WHAT_IS_ON_TOP_OF);
			next_id = IDS_WHAT_IS_ON_TOP_OF2;
		};
		if (describe_path(i,event_code,pad_in,text_out,type)) {	// completely described it
			if (next_id != 0) {
				text_out << S(next_id,"");
			};
			last_event_arg = event_arg; // I think?
			return;
		};
		if (event_code != RELEASE_ITEM) {
//			if (event_code == GIVE_BIRD_ITEM || 
//				 event_code == DROP_ITEM_IN_TRUCK) {
//				if (event_arg == 1) { 
//					return; // no need to describe which truck or bird
//				};
//				text_out << S(IDS_WHICH_IS);
//			};
			// not an exception whose arg needed to be described 
			if (event_arg_is_stack(last_event_arg)) {
				// last action was with a stack so this is a element
				text_out << space();
//				print_article(INDEFINITE_ARTICLE,text_out,
//								  S(IDS_STACK_TYPE1+last_event_arg-4));
            text_out << S(IDS_STACK_TYPE1+last_event_arg-4);
			} else if (type != NONE_GIVEN && no_gender()) {
            // too hard what with adjective/noun word order
            // and possible gender matching
            // computes n out of count for this type
			  int n = 0; // initialized for robustness on 040304
			  int count = 0;
			  for (int i = 0; i < total_arg_count; i++) {
				  if (working_set_types[i] == type) {
					  count++;
					  n = count;
				  };
			  };
           if (count == 1) { // only one
				  text_out << space(); // should really rationalize strings and spaces
				  print_article(DEFINITE_ARTICLE,text_out,S(IDS_NEW)); // no gender branch
				  text_out << space();
           } else if (n == 1) { // first one
				  text_out << space();
				  print_article(DEFINITE_ARTICLE,text_out,S(IDS_FIRST_NEW));
				  text_out << space();
           } else if (n == count) {
				  text_out << space();
				  print_article(DEFINITE_ARTICLE,text_out,S(IDS_NEWEST));
				  text_out << space();
           } else {
				  text_out << space() << S(IDS_THE); // it is english only branch...
				  print_ordinal(count+1-n,text_out);
				  text_out << space() << S(IDS_TO_LAST) << space();
           };
           print_type(type,text_out,NO_ARTICLE);
			} else if (event_arg == total_arg_count) {
			   text_out << S(IDS_THE_LAST_THING);
			} else if (total_arg_count-event_arg > 3 || // except for the last few
						  event_arg < 4) {
            text_out << space();        
            print_ordinal_definite_phrase(event_arg,text_out,FALSE,
                                          S(IDS_MADE_OR_FOUND1),
                                          S(IDS_MADE_OR_FOUND2));
//            print_the(text_out,FALSE,type);
//				print_ordinal(event_arg,text_out);
			  // could I keep track to know whether it was made or "found"
//				text_out << " ";
//				text_out << S(IDS_MADE_OR_FOUND);
			} else {
            text_out << space();        
            print_ordinal_definite_phrase(total_arg_count+1-event_arg,text_out,
                                          FALSE,
                                          S(IDS_TO_LAST_THING1),
                                          S(IDS_TO_LAST_THING2));
//            print_the(text_out,FALSE,type);
//				print_ordinal(total_arg_count+1-event_arg,text_out);
//				// event_arg is 1 indexed already
//				text_out << S(IDS_TO_LAST_THING);
			};
		};
		last_event_arg = -1;
	};
};

boolean describe_path(EventCode event_code, InputStream *pad_in, output_stream &text_out,
							 SpriteType type, boolean first_time, boolean type_path) {
	return(describe_path((unsigned char) pad_in->get(),
								event_code,pad_in,text_out,type,first_time,type_path));
};

boolean describe_path(unsigned char i, EventCode event_code, InputStream *pad_in, output_stream &text_out,
							 SpriteType type, boolean first_time, boolean type_path) {
	// returns TRUE if generated a FULL description
	if (i == END_OF_PATH_MARKER) return(FALSE); 
	if (i == SPECIAL_PATH_MARKER) { // toggle type_path
		return(describe_path((unsigned char) pad_in->get(),event_code,pad_in,text_out,type,first_time,!type_path));
	};
	SpriteType new_type = type;
   unsigned char next_char = (unsigned char) pad_in->get();
	long next_id = 0;
	if (first_time && (!type_path || next_char != END_OF_PATH_MARKER) && next_char != SPECIAL_PATH_MARKER &&
		 (event_code == GRASP_ITEM || event_code == GRASP_NEST || event_code == VACUUM_APPLY ||
		  event_code == VACUUM_APPLY_NEST)) {
		switch (type) {
         case TRUCK: // rewritten for clarity on 260802
			   if (pad_in->get() != END_OF_PATH_MARKER) {
				   text_out << S(IDS_ROBOT_FILE_BAD);
			   };
			   text_out << space() << S(IDS_ROBOT_IN_TRUCK+i);
			   return(TRUE);
			case PROGRAM_PAD:
				text_out << space() << S(IDS_WHATS_ON);
				next_id = IDS_WHATS_ON2;
				break;
			default:
				text_out << space() << S(IDS_WHATS_IN);
				next_id = IDS_WHATS_IN2;
		};
	};
	// reverse order is accomplished by recurring before printing
	if (describe_path(next_char,event_code,pad_in,text_out,new_type,FALSE,type_path)) { // completely described it
		return(TRUE);
	};
//	if (first_time && next_char == SPECIAL_PATH_MARKER && // !type_path && 
//		 (event_code == GRASP_ITEM || event_code == GRASP_NEST || event_code == VACUUM_APPLY ||
//		  event_code == VACUUM_APPLY_NEST)) {
//		text_out << " " << S(IDS_IN) << " ";
//	} else {
		text_out << space();
//	};
	if (type_path) {
		print_type((SpriteType) i,text_out,DEFINITE_ARTICLE);
//		if (next_char != END_OF_PATH_MARKER) { 
			text_out << space() << S(IDS_IN,""); // ,"" added 071299
//		};
	} else if (event_code != RELEASE_ITEM) {
		switch (type) {
			case PROGRAM_PAD:
				if (i == 0) { // might be relative addressing so call them left and right
               text_out << S(THE_LEFT_PAGE_OF);
				} else { // if (i == 1) {
               text_out << S(THE_RIGHT_PAGE_OF);
//				} else { // obsolete??
//					text_out << " " << S(IDS_PAGE) << " " << (int) i << " " << S(IDS_OF);
				};
				break;
			case TRUCK:    // obsolete??
				text_out << S(IDS_BOX_IN_TRUCK);
				if (next_id != 0) {
					text_out << S(next_id,"");
				};
				return(TRUE);
			default: // only boxes -- are pictures possible?
            if (next_char == END_OF_PATH_MARKER) { 
               print_ordinal_definite_phrase(i+1,text_out,
                                             FALSE,S(IDS_HOLE_INSIDE1),S(IDS_HOLE_INSIDE2));
            } else {
               print_ordinal_definite_phrase(i+1,text_out,
                                             FALSE,S(IDS_HOLE_OF1),S(IDS_HOLE_OF2));
            };
//            print_the(text_out,FALSE,NONE_GIVEN,S(IDS_HOLE_OF));
//				print_ordinal(i+1,text_out);
//				text_out << " " << S(IDS_HOLE_OF);
		};
	};
	if (next_id != 0) {
		text_out << S(next_id,"");
	};
	return(FALSE);
};

SpriteType type_of_event_arg(EventArg arg) {
	switch (arg) {
		case MAIN_CUBBY:
		case CUBBY_STACK:
			return(CUBBY);
		case A_COPIER:
			return(COPIER);
		case A_VACUUM:
			return(VACUUM);
      case AN_EXPANDER:
         return(EXPANDER); 
		case INTEGER_STACK:
			return(INTEGER);
		case TEXT_STACK:
			return(TEXT);
		case NEST_STACK:
			return(NEST);
		case TRUCK_STACK:
			return(TRUCK);
		case PROGRAM_PADS:
			return(PROGRAM_PAD);
		case ROBOT_STACK:
			return(ROBOT);
		case SCALE_STACK:
			return(PICTURE);
		case BOMB_STACK:
			return(BOMB);
//		case NO_ARG:
		default:
			return(NONE_GIVEN);
	};
};

void Events::insert_at_end(Event *last_event) {
	if (next == NULL) {
		next = new Events(last_event);
	} else {
		Events *new_next = next;
		while (TRUE) {
			if (new_next->rest() == NULL) {
				new_next->set_rest(new Events(last_event));
				return;
			};
			new_next = new_next->rest();
		};
	};
};

Event::~Event() {
  if (subject_path != NULL) {
	  delete subject_path;
  };
};

boolean Event::equal_to(Event *other) {
	if (other == NULL) return(FALSE);
	if (event_code() != other->event_code()) return(FALSE);
	if (pointer_to_path() == NULL) {
		if (other->pointer_to_path() == NULL) {
			return(TRUE);
		} else {
			return(FALSE);
		};
	} else {
		return(pointer_to_path()->equal_to(other->pointer_to_path()));
	};
};

const millisecond tile_crossing_duration = 100; // 2 second whole screen

flag have_asked_whether_to_update_robots_who_use_space = FALSE; // new on 181199
flag update_robots_who_use_space = FALSE;

StepStatus Event::run(Background *floor, Robot *robot, Event *next_event) { // next_event new on 270704 for arrow key editing
	//  ActionStatus *action_status) { -- all calls should have robot owning the old action_status
	if (!robot->still_running()) {
		// probably stopped by the programmer
		return(STEP_HAD_DIFFICULTIES);
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 133 || tt_debug_mode == 101 || tt_debug_mode == 1717 || 
		 tt_debug_mode == 301000 || tt_debug_mode == 170703 || tt_debug_mode == 120105) {
		character robot_name[max_resource_string_length];
		robot->name(robot_name);
		if (tt_debug_mode == 120105) {
			tt_error_file() << timeGetTime() << " " << robot_name << " on frame " << tt_frame_number << endl;
		} else {
			tt_error_file() << robot_name << " " // << "#" << robot->return_debug_counter() << " " -- commented out to enable file comparisons
								 // << robot->serial_number() << " " 
								 << (int) robot->showing_on_a_screen() << " "
								 << tt_frame_number << " " << tt_current_time << " ";
			if (tt_debug_mode == 133) tt_error_file() << timeGetTime() << " ";
			debug(robot);
		};
	} else if (robot->debug_counter == tt_debug_target) {
		tt_error_file() << "Debug target running event ";
//		debug(robot);
		tt_error_file() << endl;
	};
#endif
	if (code == NEW_ITEM || code == SERIAL_NUMBER) { // no action to take
		robot->completed();
		return(STEP_RAN_OK);		 
	};
	// returns TRUE is no problems were encountered
	Sprite *subject = NULL;
	Sprite *subject_holder = NULL; // was Cubby
	Cubby *top_cubby = robot->pointer_to_working_cubby();
	city_coordinate subject_llx = half_screen_width;
   city_coordinate subject_lly = half_screen_height;  // default at center of view
	long subject_index;
//	Vacuum *robots_vacuum;
//	Copier *robots_copier;
	millisecond duration = 0, duration_and_hold;
//	Tool *attached;
//	Tool *current_contents;
	PathDereferenceStatus path_dereference_status = PATH_DEREFERENCE_OK; // new on 281004 
	switch (code) {
		case RELEASE_VACUUM:
		case RELEASE_COPIER:
      case RELEASE_EXPANDER:
			robot->release_tool(0); // ,action_status);
			return(STEP_RAN_OK);
		case RELEASE_ITEM:
//       robot->saved_tool_coordinates(subject_llx,subject_lly);
			pick_location(robot,subject_llx,subject_lly);
			break;
		case RELEASE_ITEM_LEFT_OF: 
		case RELEASE_ITEM_RIGHT_OF: 
		case RELEASE_ITEM_ON:
#if TT_DEBUG_ON
			if (robot->debug_counter == tt_debug_target) {
				log(_T("Debug target is about to dereference target of release."));
			};
#endif
			path_dereference_status = subject_path->dereference(floor,robot,top_cubby,code,
																				 subject,subject_holder,subject_index,
																				 duration);
			if (path_dereference_status == PATH_DEREFERENCE_WAIT) {
				// dereference not ready (e.g. a nest)
				return(STEP_WAITING); 
			};
#if TT_DEBUG_ON
			if (subject != NULL && subject->debug_counter == tt_debug_target) {
				log(_T("Robot is referencing debug target."));
			};
#endif
			if (subject == NULL && code == RELEASE_ITEM_ON &&
				 subject_holder == NULL) {
#if TT_DEBUG_ON
				if (tt_debug_mode == 11) {
					tt_error_file() << "Warning place to drop item is missing -- programmer took it?" << endl;
					debug();
				};
#endif
//				say("A robot couldn't find the place it was going to drop something.");
//				robot->stop_all();
				if (path_dereference_status == PATH_DEREFERENCE_ABORT) {
					robot->abort_action(IDS_ROBOT_COULDNT_FIND_PLACE_TO_RELEASE); // removed NULL, first arg (action_status) on 171102
				} else {
					// new on 281004
					robot->set_parameter(ROBOT_CONFUSED);
					robot->completed();
				};
				return(STEP_HAD_DIFFICULTIES);
			};
/*
			current_contents = subject_holder->component(subject_index);
			if (current_contents != NULL) {
				subject_holder->remove_component(subject_index,ROBOT_EXTRACTED,
															robot->showing_on_a_screen());
				floor->remove_item(current_contents);
				recursively_remove_leaders(current_contents,floor);
				delete current_contents;
			};
*/
			switch (code) {
				case RELEASE_ITEM_LEFT_OF: 
					if (subject != NULL) {
						subject->lower_left_corner(subject_llx,subject_lly);
						subject_llx -= (9*robot->pointer_to_ultimate_item()->current_width())/10;  // 90% to get a little overlap
					};
					break;
				case RELEASE_ITEM_RIGHT_OF:
					if (subject != NULL) {
						subject->lower_left_corner(subject_llx,subject_lly);
						subject_llx += (9*subject->current_width())/10; // was -tile_width rather than 90%
					};
					break;
				case RELEASE_ITEM_ON:
					if (subject_holder == NULL) {
						subject->lower_left_corner(subject_llx,subject_lly);
					} else {
						switch (subject_holder->kind_of()) {
							case CUBBY: {
									city_coordinate cubby_hole_width, cubby_hole_height;
									((Cubby *) subject_holder)
											->cubby_insides_of_index(subject_index, // coerced to (cubby_index) prior to 070203
																			 subject_llx,subject_lly,
																			 cubby_hole_width,cubby_hole_height);
//									if (tt_log_version_number < 13) subject_llx += cubby_hole_width/2;
											};
								break;
							default: // e.g. notebook
								subject_holder->center_location(subject_llx,subject_lly);
								break;
						};
						// looks better not doing this...
//						subject_lly += cubby_hole_height/2;
					};
			};
			robot->adjust_release_coordinates(subject_llx,subject_lly);
//			if (tt_log_version_number > 12) 
			subject_llx -= robot->true_width()/4;
			break;
		case DESTROY_HOUSE:
			// for now it just disappears but programmer has to be notified
//			robot->destroy_house();
			robot->completed();
			return(STEP_TERMINATED_HOUSE);
		case START_EVENT:
			robot->starting_event();
			break;
		case LAST_EVENT:
         robot->last_event();
			break;
		default:
			switch (code) {
				case GRASP_VACUUM:
					if (key == '1' && robot->kind_of() == ROBOT_IN_TRAINING) { 
						// marker that this was automatically generated and am retraining so stop
						tt_programmer->stop_retraining(robot);
						return(STEP_RAN_OK); // sort of
               } else if (robot->grasp_vacuum()) { // action_status)) {
						return(STEP_RAN_OK);
					};
				case GRASP_COPIER:
               if (robot->grasp_copier()) { // action_status)) {
                  return(STEP_RAN_OK);
               };
  				case GRASP_EXPANDER:
               if (robot->grasp_expander()) { // action_status)) {
                  return(STEP_RAN_OK);
               };
			}; // else fall thru to below
			path_dereference_status = subject_path->dereference(floor,robot,top_cubby,code,
																				 subject,subject_holder,subject_index,
																				 duration);
			if (path_dereference_status == PATH_DEREFERENCE_WAIT && 
				 code != LABEL_CHARACTER) { // conjunction added on 020405 so a robot can comment a box hole with an empty nest 
					 // (or an empty box hole too?)
				// dereference not ready (e.g. a nest)
				return(STEP_WAITING); // but not completed
			};
//#if TT_DEBUG_ON
//			perform_integrity_checks(subject);
//			perform_integrity_checks(subject_holder);
//#endif
#if TT_DEBUG_ON
			if (subject != NULL && subject->debug_counter == tt_debug_target) {
				log(_T("Robot is referencing debug target."));
			};
#endif
			if (code == LABEL_CHARACTER) { // holder is subject and index is ok
				subject = subject_holder;
			};
			if (subject == NULL) {
				if (code != VACUUM_APPLY_RESTORE && 
				    code != APPLY_GRASPED_ITEM && code != APPLY_GRASPED_NEST &&
					 code != KEYBOARD_CHARACTER &&
					 // following added on 170500
					 !(code == COPIER_APPLY && !robot->pointer_to_tool()->need_subject_to_work())) {
#if TT_DEBUG_ON
					if (tt_debug_mode == 11) {
						 tt_error_file() << "Warning place to apply action missing -- programmer took it?" << endl;
						 debug();
					};
#endif
					int reason;
					if (code == GRASP_ITEM || code == GRASP_VACUUM || code == GRASP_COPIER || code == GRASP_EXPANDER) {
						//robot->say_a_robot_named(0,IDS_ROBOT_COULDNT_FIND_ITEM); // replaced by following on 281004
						reason = IDS_ROBOT_COULDNT_FIND_ITEM; // new on 281004
//						say("A robot couldn't find something, but continuing anyway");
//						completed(action_status);
//                robot->nothing_to_grasp();
//						return(STEP_RAN_OK);
					} else if (code == GRASP_NEST) {
//						robot->say_a_robot_named(0,IDS_ROBOT_COULDNT_FIND_NEST);
						reason = IDS_ROBOT_COULDNT_FIND_NEST;
					} else {
//						robot->say_a_robot_named(0,IDS_ROBOT_COULDNT_FIND_PLACE);
						reason = IDS_ROBOT_COULDNT_FIND_PLACE;
					};
					if (path_dereference_status == PATH_DEREFERENCE_ABORT) {
						// new to abort rather be confused depending upon cause (e.g. programmer holding the subject)
						robot->abort_action(reason);
					} else {
						robot->say_a_robot_named(0,reason); // "moved" here on 281004
						robot->set_parameter(ROBOT_CONFUSED); // new on 100304
						robot->completed(); // was action_status
					};
					return(STEP_HAD_DIFFICULTIES);
				};
			} else if (code == GIVE_BIRD_ITEM) {
				if (subject->kind_of() == BIRD) {
					subject->center_location(subject_llx,subject_lly); 
					robot->adjust_release_coordinates(subject_llx,subject_lly);
					if (subject->showing_on_a_screen()) {
						((Bird *) subject)->show_anticipation();
					};
				} else {
					robot->say_a_robot_named(0,IDS_ROBOT_COULDNT_FIND_BIRD);
					code = RELEASE_ITEM_ON;
					subject->lower_left_corner(subject_llx,subject_lly);
//					say("A robot was trained to give a bird something, but now something else is there.");
//					completed(action_status);
//					return(STEP_HAD_DIFFICULTIES);
				};
			} else if (subject->pointer_to_background() == NULL && (!subject->infinite_stack() && tt_log_version_number >= 53)) { 
				// && tt_log_version_number > 12) { // e.g. a robot's vacuum's buttons are being clicked
				// added infinite_stack test on 041004 since can be copying the contents of Tooly (or a notebook?)
				// tt_log_version_number > 53 added on 191004 since this broke old demos
				// changed to >= 53 on 181204
				robot->lower_left_corner(subject_llx,subject_lly);
			} else {
				subject->lower_left_corner(subject_llx,subject_lly);
			};
	};
	if (!robot->showing_on_a_screen() || 
		 (subject != NULL && (tt_log_version_number >= 53?!subject->ultimate_leader()->showing_on_a_screen():!subject->showing_on_a_screen()))) { 
		// added ultimate_leader() on 041004 since otherwise copying contents of Tooly with Wand look bad
		// added tt_log_version_number > 53 on 191004 since this might break old demos (really is between 53 and 54)
		// changed to >= 53 on 181204
		duration = 0;
		duration_and_hold = 0;
	} else {
		if (duration == 0) {
			city_coordinate robot_llx,robot_lly;
			robot->lower_left_corner(robot_llx,robot_lly);
			duration = duration_delta_location(robot_llx,robot_lly,
														  subject_llx,subject_lly);
		};
		if (duration == 0) {
			duration_and_hold = 0;
		} else {
			duration_and_hold = duration+hold_before_event_duration;
			// add a noise for variety and also so if multiple robots running
			// they aren't on top of each other exactly
			subject_llx += my_random(tile_width/4);
			subject_lly += my_random(tile_height/4);
		};
		long new_robot_priority;
		if (subject != NULL) {
			new_robot_priority = subject->ultimate_leader()->priority(); 
			// float above it -- e.g. held by programmer
			if (new_robot_priority < default_robot_priority) new_robot_priority -= 10; 
		} else {
			new_robot_priority = default_robot_priority;
		};
		if (new_robot_priority < robot->priority()) {
			// need to float up
			robot->set_priority(new_robot_priority);
		} else if (default_robot_priority != robot->priority()) {
         // do nothing if already at the right priority
//         tt_error_file() << "debugging -- scheduling reset robot priority callback for " << (tt_current_time+duration) << endl;
			robot->do_after(duration,robot,RESET_ROBOT_PRIORITY_CALLBACK);
		};
      if (robot->pointer_to_background()->in_the_room()) {
         tt_programmer->translate_floor_to_room_coordinates(subject_llx,subject_lly);
         if (code == SELECT_STACK_ITEM) {  // doesn't go to Tooly in room so takes no time to get what he needs
            duration = 0;
            duration_and_hold = 0;
         } else {
            duration /= room_speedup; // much quicker when standing up
            duration_and_hold /= room_speedup;
//            duration_and_hold = duration+hold_before_event_duration;
         };
      };
	};
	switch (code) {
		case START_EVENT:
			robot->set_max_working_set_size(subject_path->return_index());
			robot->completed(); // why? - to be sure it is reset??
			return(STEP_RAN_OK);
		case LAST_EVENT:
			if (duration > 0) {
				Robot *first_robot = robot->pointer_to_first_in_line();
				if (first_robot != NULL) {
					first_robot->add_to_end_of_line(duration); 
					// duration added on 010503 -- this ends up calling completed() on wrong robot (noticed 110704)
					robot->do_after(duration,robot,ACTION_COMPLETED_CALLBACK); // new on 110704
				} else { // I'm first so no need to do anything
					robot->completed();
				};
			} else {
				robot->completed();
			};
			return(STEP_RAN_OK);
		case NEW_MAIN_CUBBY:
//			floor->set_body_cubby((Cubby *) subject);
			if (subject->kind_of() == CUBBY) {
				robot->add_to_working_set(robot->pointer_to_working_cubby(),TRUE); // no longer special
				Cubby *cubby = (Cubby *) subject;
				robot->set_working_cubby(cubby);
				// following new on 170405 so that if on back of a picture the new cubby isn't in limbo
				Background *robot_background = robot->pointer_to_background();
				if (robot_background != NULL) {
					Picture *picture = robot_background->pointer_to_picture();
					if (picture != NULL) {
						add_flipped_follower(cubby,NULL,picture);
					};
				};
				robot->completed();
				return(STEP_RAN_OK);
			} else { 
				robot->say_a_robot_named(0,IDS_ROBOT_EXPECTED_BOX_BUT,4);
				robot->action_aborted();
				return(STEP_HAD_DIFFICULTIES);
			};
		case SELECT_STACK_ITEM:
			if (duration > 0) {
				robot->animate_location(subject_llx,subject_lly,duration);
			};
			robot->select_from_stack(subject,duration_and_hold); // ,action_status);
			break;
		case GRASP_ITEM:
		case GRASP_NEST:
		case GRASP_VACUUM:
		case GRASP_COPIER:
		case GRASP_EXPANDER:
			if (duration > 0) {
				robot->animate_location(subject_llx, //+subject->current_width()/2,
												subject_lly, //+subject->current_height()/2,
												duration);
			};
#if TT_DEBUG_ON
			DWORD start;
			if (tt_debug_mode == 120105) {
				start = timeGetTime();
			};
#endif
			robot->grasp_tool(subject,subject_holder,subject_index,
									duration_and_hold); // ,action_status);
#if TT_DEBUG_ON
			if (tt_debug_mode == 120105) {
				tt_error_file() << "grasp_tool took " << (timeGetTime()-start) << "ms" << endl;
			};
#endif
			break;
//		case NUMBER_APPLY:
//		case COPY_NUMBER_APPLY:
		case COPIER_APPLY:
      case COPIER_APPLY_NEST:
		case VACUUM_APPLY:
      case VACUUM_APPLY_NEST:
		case VACUUM_APPLY_RESTORE:
		case APPLY_GRASPED_ITEM:
      case APPLY_GRASPED_NEST:
      case EXPANDER_APPLY:
			if (duration > 0) {
				city_coordinate new_llx, new_lly;
				if (subject == NULL || code == VACUUM_APPLY_RESTORE) {
					pick_location(robot,new_llx,new_lly);//subject_llx,subject_lly);
//					robot->next_set_down_location(subject_llx,subject_lly);
				} else {
					robot->location_to_apply_tool(subject_llx+subject->current_width()/2,
															subject_lly+subject->current_height()/2,
															new_llx,new_lly);
				};
				robot->animate_location(new_llx,new_lly,duration);
			};
			robot->apply_tool(subject,subject_holder,subject_index,
									duration_and_hold,key); // removed action_status
			if (robot->prepared_for_deletion()) { 
				// e.g. tool was bomb on back of picture so I'm blown up
            // could avoid this if I postpone destroying the picture
				return(STEP_TERMINATED_HOUSE); // house, picture -- doesn't matter
			};
			break;
		case RELEASE_ITEM:
			// commented out the conditional so that things are released at a reasonable location
			// restored on 280403 since is so wasteful for robots running on the back
			if (duration > 0) {
				robot->animate_location(subject_llx,subject_lly,duration);
			};
		case RELEASE_VACUUM:
		case RELEASE_COPIER:
      case RELEASE_EXPANDER:
			robot->release_tool(duration_and_hold); // ,action_status);
			break;
		case RELEASE_ITEM_LEFT_OF:
		case RELEASE_ITEM_RIGHT_OF:
			if (duration > 0) {
				robot->animate_location(subject_llx,subject_lly,duration);
			};
			robot->release_tool(duration_and_hold, // action_status,
									  (Cubby *) subject, - (int) code);
			break;
		case RELEASE_ITEM_ON:
			if (duration > 0) {
				robot->animate_location(subject_llx,subject_lly,duration);
			};
         if (!(subject != NULL &&
				  // updated on 110699 to tool_to_drop so a cubby attached to a wand works fine
				  // isn't there a generic way to ask the following? asked 090203
               ((subject->kind_of() == INTEGER && robot->pointer_to_tool() != NULL && robot->tool_to_drop()->kind_of() == CUBBY)
                // if dropping a cubby on a number, cubby is still around
				    || subject->is_blank()
					 || subject->is_flipped()))) { // new on 070105 since if dropped on a flipped picture might be removed later
					// or dropping anything on a blank item
				// following did nothing until arg added on 210505
            robot->remove_tool_from_working_set(robot->pointer_to_tool());
         };
			if (subject_holder == NULL) {
				robot->release_tool(duration_and_hold,// action_status,
										  subject,0,(key == 1)); // when key == 1 really released_item_on_its_stack
			} else {
				robot->release_tool(duration_and_hold,// action_status,
										  subject_holder,subject_index,(key == 1));
			};
			if (robot->is_action_in_progress() && duration > 0) {
				// release will take time so move robot out of the way
				robot->do_after(duration_and_hold,robot,MOVE_ROBOT_OUT_OF_THE_WAY_CALLBACK);
			};
			break;
		case GIVE_BIRD_ITEM:
			if (duration > 0) {
				robot->animate_location(subject_llx,subject_lly,duration);
			};
			robot->release_tool(duration_and_hold,(Bird *) subject); // removed action_status
			break;
		case DROP_ITEM_IN_TRUCK:
			if (duration > 0) {
				robot->animate_location(subject_llx,subject_lly,duration);
			};
			robot->release_tool(duration_and_hold,(Truck_Inside *) subject);  // removed action_status
			break;
		case KEYBOARD_CHARACTER:
		case KEYBOARD_CHARACTER_AND_SELECT:
		case LABEL_CHARACTER:
			if (duration > 0) {
				if (subject == NULL || robot->holding(subject)) {
					city_coordinate delta_x = 0, delta_y = 0;
					if (robot->current_llx() > 15*tile_width) {
						delta_x = -5*tile_width;
					} else if (robot->current_llx() < 2*tile_width) {
						delta_x = 5*tile_width;
					};
					if (robot->current_lly() > 15*tile_height) {
						delta_y = -5*tile_height;
					} else if (robot->current_lly() < 2*tile_height) {
						delta_y = 5*tile_height;
					};
					robot->animate_delta_location(delta_x,delta_y,duration);
				} else {
					if (code == LABEL_CHARACTER && subject->kind_of() == CUBBY) {
						// warn/complain if not a cubby???
						subject_llx += (subject_index*subject->current_width())/
											 ((Cubby *) subject)->current_number_of_holes();
					};
					robot->animate_location(subject_llx,subject_lly,duration);
				};
			};
			// following is new on 241099 to fix old programs that used space to change picture number
			if ((!have_asked_whether_to_update_robots_who_use_space || update_robots_who_use_space) &&
				 key == ' ' && subject != NULL && 
				 subject->active() && // if space doesn't accomplish anything
				 (subject->kind_of() == PICTURE || subject->kind_of() == REMOTE_PICTURE) &&
				 robot->version_number() == 0) { // this conjunct added on 010201 since only old robots should do this
#if TT_DEBUG_ON
				if (tt_debug_mode == 191099) update_robots_who_use_space = TRUE;
#endif
				if (tt_trouble_shooting && // are sophisticated enough to deal with this
					 !update_robots_who_use_space && 
					 !have_asked_whether_to_update_robots_who_use_space) {
					string message;
					if (subject->kind_of() == PICTURE) { 
						message = S(IDS_SPACE_NO_LONGER_CHANGES_PICTURES,"A robot is typing space to a picture that wasn't stopped. Space no longer changes a picture. Should I change such robots so they type '+' instead?");
					} else {
						message = S(IDS_SPACE_NO_LONGER_CHANGES_REMOTE_CONTROLS,"A robot is typing space to a remote control that wasn't stopped. Space no longer changes it. Should I change such robots so they type '+' instead?");
					};
					update_robots_who_use_space = (message_box(MB_ICONQUESTION|MB_YESNO,message) == IDYES);
					have_asked_whether_to_update_robots_who_use_space = TRUE;
					message_box(MB_ICONINFORMATION|MB_OK,
						         S(IDS_ROBOT_UPDATED_WHEN_RUN,"Robots will be updated the first time they run. You'll need to save the updated robots to avoid being asked again about this in your next session."));
				};
				if (update_robots_who_use_space) {
					character name_string[256];
					robot->name(name_string);
					tt_error_file() << "Updated robot " << name_string << " to use + on a picture or remote control rather than space." << endl;
					key = '+'; // new way of changing the picture
				};
			};
			robot->send_keyboard_character(duration,key,subject,
													 (code == LABEL_CHARACTER),
													 (code == KEYBOARD_CHARACTER_AND_SELECT),
													 subject_index,
													 event_sends_character_to_same_subject(next_event)); // arg new on 270704
			break;
/*
		case NEW_CUBBY_SIZE:
			if (duration > 0 && !robot->holding(subject)) {
				robot->adjust_release_coordinates(subject_llx,subject_lly,
															 subject->current_width(),	
															 subject->current_height());	
				robot->animate_location(subject_llx,subject_lly,duration);
				robot->set_after(duration,action_status,ACTION_COMPLETED);
				((Cubby *) subject)
					->set_number_of_holes_after(duration,parameter);
			} else {
				((Cubby *) subject)->set_number_of_holes(parameter);
				*action_status = ACTION_COMPLETED;
			};
			break;
*/
		default:
			robot->completed(); // for now -- better to warn??
	};
	return(STEP_RAN_OK);
};

boolean Event::event_sends_character_to_same_subject(Event *other_event) { // new on 270704
	return(other_event->event_code() == event_code() && subject_path->equal_to(other_event->pointer_to_path()));
};

short int Event::serial_number() {
   if (code != SERIAL_NUMBER) return(0);
   return((short int) (((unsigned char) key)*256+subject_path->return_index()));
};

unsigned long Event::event_hash() {
   // 32-bit hash of just this event
   return((code<<16)+(key<<24)+subject_path->path_hash());
};

int Event::robot_version_number() { // new on 131000
	Path *next = subject_path->pointer_to_next();
	if (next == NULL) return(0);
	return(next->return_index());
};

Event *create_serial_number(int serial_number, int version_number) {
//   unsigned short int serial_number = next_serial_number();
   // updated on 270802 to be passed a serial number
   // replace this soon with GUID!!!
   if (serial_number < 0) {
      serial_number = next_serial_number();
   };
   return(new Event(SERIAL_NUMBER,new Path(serial_number%256,new Path(version_number)), // was latest_robot_version prior to 300802
                    (char) (serial_number/256)));
};

Event *create_first_event(int working_set_size) {
	return(new Event(START_EVENT,new Path(working_set_size))); 
};

Event *create_last_event() {
	return(new Event(LAST_EVENT,new Path(0)));
};

void pick_location(Robot *robot, city_coordinate &subject_llx, city_coordinate &subject_lly) {
	if (robot->showing_on_a_screen()) {
		robot->next_set_down_location(subject_llx,subject_lly);
		if (tt_log_version_number < 40) { // rewritten on 040404 so this is unnecessary
			robot->adjust_release_coordinates(subject_llx,subject_lly); 
		};
	} else { // leave it in the center so when you sit it'll be easy to find
		subject_llx = half_screen_width;
//      if (tt_log_version_number > 12) {
		   subject_lly = 4*tile_height; 
			// changed policy on 2/27/98 since half_screen_height is too high now that room and floor have different coordinates
//		} else {
//			subject_lly = half_screen_height;
//		};
	};
};

/*
		robot->lower_left_corner(subject_llx,subject_lly);
		if (subject_llx < 5*tile_width || subject_llx > 15*tile_width ||
			 subject_lly < 5*tile_height || subject_lly > 15*tile_height) {
			// left, right, bottom or top quarter
			// so near the center
			subject_llx = (8+random(4))*tile_height;
			subject_lly = (8+random(4))*tile_height;
		} else { // pretty much in middle so go to lower left
			subject_llx = 2*tile_width;
			subject_lly = 2*tile_height;
		};
*/

millisecond duration_delta_location(city_coordinate current_llx,
												city_coordinate current_lly,
												city_coordinate goal_llx,
												city_coordinate goal_lly) {
	city_coordinate delta_x = labs(current_llx-goal_llx);
   city_coordinate delta_y = labs(current_lly-goal_lly);
   if (delta_x == 0 && delta_y == 0) return(0);
	return(min(2000L, // no more than 2 seconds
				  max((delta_x*tile_crossing_duration)/tile_width
						+(delta_y*tile_crossing_duration)/tile_height,
						250L)));
};

void update_hand_kind_for_release() {
   if (hand_kind != COPIER || copier_in_hand_unattached) {
      hand_kind = NONE_GIVEN;
   } else {
      copier_in_hand_unattached = TRUE;
   };
};

void check_if_bird_hatches(Event *next_event, output_stream &out) {
   if (next_event->event_code() == NEW_ITEM) {
      int temp_index = (next_event->pointer_to_path()->return_index()+1);
      java_assign_new(out,S2(IDS_JAVA_TEMP,temp_index),S(IDS_JAVA_TT_BIRD),NULL);
      java_send(out,S2(IDS_JAVA_TEMP,temp_index),S(IDS_JAVA_SET_NEST),S(IDS_JAVA_HAND));
//      out << "  temp" << temp_index
//          << " = new TTBird(); // bird hatches from nest" << endl
//          << "  temp" << temp_index << ".SetNest(hand);" << endl;
   };
};

void check_if_remote_notebook(Event *next_event, output_stream &out) {
   if (next_event->event_code() == NEW_ITEM) {
      int temp_index = (next_event->pointer_to_path()->return_index()+1);
      java_assign_send(out,S2(IDS_JAVA_TEMP,temp_index),S(IDS_JAVA_TT_NOTEBOOK_CLASS),S(IDS_JAVA_GET_LOCAL_REMOTES_NOTEBOOK));
      java_send(out,S2(IDS_JAVA_TEMP,temp_index),S(IDS_JAVA_SET_CONTROLEE),S(IDS_JAVA_HAND));
   };
};

boolean remaining_actions_all_final_cleanup(Events *events) { // new on 230205
	EventCode code = events->first()->event_code();
	if (code == KEYBOARD_CHARACTER ||
		 code == RELEASE_VACUUM ||
		 code == VACUUM_APPLY) {
		return(remaining_actions_all_final_cleanup(events->rest()));
	};
	if (code == LAST_EVENT) {
		return(TRUE);
	};
	return(FALSE);
};

boolean Event::java(java_stream &out, Events * &next, Robot *robot, SpriteTypePointer &working_set_types, int &working_set_size) {
//	boolean first = TRUE;
	UnwindProtect<boolean> set(tt_creating_java_for_an_event,TRUE); // new on 180201
   int path_index = subject_path->return_index();
   int next_path_index = -1;
   if (next != NULL) {
      next_path_index = next->first()->pointer_to_path()->return_index();
   };
   if (code != NEW_ITEM && code != SERIAL_NUMBER && code != LAST_EVENT && code != START_EVENT) { // internal events
#if TT_NEW_IO
		output_string_stream comment_out; 
		output_string_stream encoding_out;
#else
		const int comment_size = 2048;
      string comment = new character[comment_size];
      output_string_stream comment_out(comment,comment_size);
      const int encoding_size = 1024;
      unsigned char *path_encoding = new unsigned char[encoding_size];
      output_string_stream encoding_out((char *) path_encoding,encoding_size);
#endif
      subject_path->dump(encoding_out);
      encoding_out.put(END_OF_PATH_MARKER);
      encoding_out.put((char) 0);
#if TT_NEW_IO
		input_string_stream encoding_in(encoding_out.str());
#else
      StringInputStream encoding_in((char *) path_encoding,encoding_size);
#endif
      unsigned char event_arg = (unsigned char) encoding_in.get();
      describe_event(code,key,comment_out,event_arg);
      describe_event_arg(event_arg,code,working_set_types,working_set_size,&encoding_in,comment_out);
      comment_out.put('\0');
		print_as_comment(out,comment_out.STR,encoding_size);
#if !TT_NEW_IO
      delete [] comment;
		delete [] path_encoding;
#endif
   };
   switch (code) {
      case START_EVENT:
         {
         if (path_index > 0) {
				working_set_types = new SpriteType[path_index];
				working_set_size = path_index; // new on 040304
				for (int i = 0; i < path_index; i++) {
					working_set_types[i] = NONE_GIVEN;
				   java_declare(out,S(IDS_JAVA_TT_OBJECT),S2(IDS_JAVA_TEMP,i+1));
//             out << "  TTObject temp" << (i+1) << ";" << endl;
            };
         };
         total_arg_count = path_index;
         new_main_cubby = FALSE;
         vacuum_created = FALSE;
			copier_created = FALSE;
			hand_kind = NONE_GIVEN; // new on 080205 -- can make a difference since RELEASE_ITEM can get confused if still on old value here
         };
         break;
      case SELECT_STACK_ITEM:
			last_temp_variable_index = next_path_index+1;
			temp_variables_count = max(temp_variables_count,last_temp_variable_index); // new on 121103
         java_assign(out,S2(IDS_JAVA_TEMP,last_temp_variable_index),subject_path);
//         out << "  temp" << (next->first()->pointer_to_path()->return_index()+1) << " = ";
         // next event should always be a NEW_ITEM which specifies the index
//         subject_path->java(out);
//         out << "; // went to a stack in the Tool Box" << endl;
         subject_path->kind_is(hand_kind); // see NEW_ITEM comment
         break;
      case SERIAL_NUMBER: // used internally
         break;
      case NEW_ITEM: // second part of path encodes the SpriteType ...
			working_set_types[path_index] = (SpriteType) (subject_path->pointer_to_next()->return_index());
         break;
		case GRASP_COPIER:
//         out << "  // picked up the Magic Wand" << endl;
			if (!copier_created) {
				java_declare_new(out,"TTCopier","copier","TTCopier"); // was java_assign_new prior to 250101 - could add to javacom.rc 
				copier_created = true;
			};
			java_assign(out,S(IDS_JAVA_HAND),"copier");
         hand_kind = COPIER;
       break; 
      case GRASP_ITEM:
         {
         const_string variable;
         if (next->first()->event_code() == NEW_ITEM) {
//            out << "  temp" << (next->first()->pointer_to_path()->return_index()+1) << " = ";
				last_temp_variable_index = next_path_index+1;
				temp_variables_count = max(temp_variables_count,last_temp_variable_index); // new on 121103
            variable = S2(IDS_JAVA_TEMP,last_temp_variable_index);
				// following was too general - OK only is special circumstances since can put things down to make them temporary variables
//			} else if (next->first()->event_code() == RELEASE_ITEM) { // new on 301100 to deal with old robots that dropped things on blank things - a good optimization even if not so general
//				next = next->rest(); // skip release
//				break;
				// experiment on 181203 to comment out the following
				// restored on 220205 to deal with old Playground robots -- e.g. speak
			} else if (next->first()->event_code() == RELEASE_ITEM && // new on 301100 -  if it is less than abs of subject_path (if negative)
						  subject_path->return_index() < 0 &&
						  robot->version_number() == 0 && // new on 220205 so only old robots try this
						  last_temp_variable_index < -subject_path->return_index()) {
				next = next->rest(); // skip release
				break;
         } else {
//            out << "  hand = ";
            variable = S(IDS_JAVA_HAND);
         };
//         subject_path->java(out,TRUE,NULL,"PickUp");
//         out << "; // picked up something" << endl;
         java_assign(out,variable,subject_path,TRUE,NULL,S(IDS_JAVA_PICK_UP));
         if (next->first()->event_code() == NEW_ITEM) {
             java_assign(out,S(IDS_JAVA_HAND),S2(IDS_JAVA_TEMP,next_path_index+1));
//             out << "  hand = temp"
//                 << (next->first()->pointer_to_path()->return_index()+1)
//                 << ";" << endl;
         };
			subject_path->kind_is(hand_kind);
         };
         break;
      case GRASP_NEST:
         java_assign(out,S(IDS_JAVA_HAND),subject_path);
//         out << "  hand = ";
//         subject_path->java(out);
//         out << "; // picked up a nest" << endl;
         hand_kind = NEST;
			while (next->first()->event_code() == KEYBOARD_CHARACTER || next->first()->event_code() == KEYBOARD_CHARACTER_AND_SELECT) { 
				// new on 070205 to skip over labeling of nest and bird
				next = next->rest();
			};
         check_if_bird_hatches(next->first(),out);
         if (subject_path->pointer_to_next() != NULL) { // cubby path (or truck?)
            java_send(out,subject_path,TRUE,S(IDS_JAVA_TT_DOT_NOTHING),S(IDS_JAVA_SET_HOLE));
//            out << "  ";
//            subject_path->java(out,TRUE,"TT.NOTHING","SetHole");
//            out << ";" << endl;
         };
         break;
      case COPIER_APPLY:
         if (next->first()->event_code() == NEW_ITEM) {
            Path *temp_path = next->first()->pointer_to_path();
//            java_assign_send(out,temp_path,subject_path,"onTop().copy"); // was S(IDS_JAVA_COPY) prior to 151100
				java_assign_send(out,temp_path,"copier","applyTo",subject_path,"this"); // re-written on 140101
//            out << "  ";
//            temp_path->java(out);
//            out << " = ";
//            subject_path->java(out);
//            out << ".Copy(); // used the Magic Wand" << endl;
            // it is as if you picked up a copy of the subject
            java_assign(out,S(IDS_JAVA_HAND),temp_path);
//            out << "  hand = ";
//            temp_path->java(out);
//            out << ";" << endl;
            subject_path->kind_is(hand_kind);
         } else { 
//            java_assign_send(out,S(IDS_JAVA_HAND),subject_path,"onTop().copy"); // was S(IDS_JAVA_COPY) prior to 151100
				java_assign_send(out,S(IDS_JAVA_HAND),"copier","applyTo",subject_path,"this"); // re-written on 140101
				// added the following on 260700
				if (next->first()->event_code() != RELEASE_ITEM_ON && next->first()->event_code() != GIVE_BIRD_ITEM) { 
					// conditions added on 051000 since item isn't created if dropped on something immediately
					temp_variables_count++;
					last_temp_variable_index = temp_variables_count; // new on 121103 since the old code incremented what might not be the max
//					last_temp_variable_index++; // since the wand creates an item that ends up in the working set
					java_assign(out,S2(IDS_JAVA_TEMP,last_temp_variable_index),S(IDS_JAVA_HAND));
				};
//            out << "  hand = ";
//            subject_path->java(out);
//            out << ".Copy(); // used the Magic Wand" << endl;
         };
			copier_in_hand_unattached = FALSE;
         break;
      case RELEASE_COPIER: // I think this is right...
//         out << "  // dropped the Magic Wand" << endl;
         hand_kind = NONE_GIVEN;
         break;
      case GRASP_VACUUM:
			if (key != '1' && (robot->version_number() != 0 || !remaining_actions_all_final_cleanup(next))) { 
				// new on 191100 - if generated to clean up no need to waste time doing so ('1' marker to indicate generated events)
				// added second condition on 230205 to compile old code (e.g. speak in Playground)
				if (!vacuum_created) {
					java_declare_new(out,S(IDS_JAVA_TT_VACUUM),S(IDS_JAVA_VACUUM),S(IDS_JAVA_TT_VACUUM)); // was java_assign_new from 140101 to 250101 
					vacuum_created = true;
				};
				java_assign(out,S(IDS_JAVA_HAND),S(IDS_JAVA_VACUUM));
	//         out << "  hand = vacuum;" << endl;
				hand_kind = VACUUM;
				break;
			}; // else if key == 1 then treat like the end
		case LAST_EVENT:
         if (new_main_cubby) {
            java_send(out,S(IDS_JAVA_OLD_GIVEN),S(IDS_JAVA_BECOMES),S(IDS_JAVA_GIVEN));
//            out << "  oldGiven.Becomes(given); // this is now the box the team is working on" << endl;
         };
			// commented out on 191100 since is now always a local variable
//       if (vacuum_created) { 
//           java_assign(out,S(IDS_JAVA_VACUUM),S(IDS_JAVA_NULL));
//       };
			if (tt_main_notebook_referenced) { // new on 141100
				out << "  notebook.setPageNumber(1); // restore main notebook" << endl;
			};
         out << "  " << S(IDS_JAVA_RETURN_THIS) << " ";
         if (robot->place_in_team() == 0) {
            // different comment if first robot
            out << S(IDS_JAVA_LAST_EVENT_COMMENT2) << endl;
         } else {
            out << S(IDS_JAVA_LAST_EVENT_COMMENT) << endl;
         };
         return(FALSE); // new on 191100 to terminate regardless
      case VACUUM_APPLY:
         if (key != 0) { // modified so it is erasing
//           java_begin_block(out);
//           java_declare(3,out,S(IDS_JAVA_TT_OBJECT),S(IDS_JAVA_ERASED));
           java_send(out,subject_path,S(IDS_JAVA_ERASE));
//         out << "  {TTObject item = ";
//         subject_path->java(out);
//         out << ";" << endl;
//         out << "  if (item instanceof TTNest) {" << endl;
//         out << "    vacuum.SuckUp(item.RemoveOnTop()); // used vacuum on top of nest" << endl;
//         out << "  } else {" << endl;
//         out << "    vacuum.SuckUp(item); // used vacuum on something" << endl;
//           if (subject_path->pointer_to_next() != NULL) { // cubby path (or truck?)
//              java_send(3,out,subject_path,TRUE,S(IDS_JAVA_ERASED),S(IDS_JAVA_SET_HOLE));
//           } else {
//              java_assign(3,out,subject_path,S(IDS_JAVA_ERASED));
//           };
//           java_end_block(out);
			} else if (subject_path->return_index() == 0 && subject_path->pointer_to_next() == NULL) { // new on 160201 to deal with vacuuming up the main cubby
				java_indent(out);
				out << "throw new TTException(\"" << S(IDS_ROBOT_VACUUMED_UP_HIS_BOX,"Robot vacuumed up his box.") << "\");" << endl; 
				return(FALSE); // following statements can never be reached
         } else {
           java_send(out,S(IDS_JAVA_VACUUM),S(IDS_JAVA_SUCK_UP),subject_path);
//         out << "  {TTObject item = ";
//         subject_path->java(out);
//         out << ";" << endl;
//         out << "  if (item instanceof TTNest) {" << endl;
//         out << "    vacuum.SuckUp(item.RemoveOnTop()); // used vacuum on top of nest" << endl;
//         out << "  } else {" << endl;
//         out << "    vacuum.SuckUp(item); // used vacuum on something" << endl;
           if (subject_path->pointer_to_next() != NULL && !subject_path->contains_special_path_marker()) { 
				  // cubby path (or truck?)
//				  java_if(out,S(IDS_JAVA_VACUUM,"?"),S(IDS_JAVA_GET_STATE,"?")," == ",S(IDS_JAVA_VACUUM_SUCK_STATE,"?")); // new on 051000
//            java_send(out,subject_path,TRUE,S(IDS_JAVA_TT_DOT_NOTHING),S(IDS_JAVA_SET_HOLE));
				  // re-wrote the above on 161100 since the old way didn't work if say vacuuming something off a nest in a box
				  java_send(out,subject_path,TRUE,S(IDS_JAVA_VACUUM),"vacuumedHole"); // could add to javacom.rc
           };
         };
         break;
      case VACUUM_APPLY_RESTORE:
			if (next->first()->event_code() == NEW_ITEM) {
				last_temp_variable_index = next->first()->pointer_to_path()->return_index()+1; // new on 220101 so subsequent uses of last_temp_variable_index don't conflict
				temp_variables_count = max(temp_variables_count,last_temp_variable_index); // new on 121103
				java_assign_send(out,S2(IDS_JAVA_TEMP,last_temp_variable_index),S(IDS_JAVA_VACUUM),S(IDS_SPIT_OUT));
			} else { // should only be if robot was trained before beta 13A
				java_send(out,S(IDS_JAVA_VACUUM),S(IDS_SPIT_OUT));
			};
//         out << "  ";
//         subject_path->java(out);
//         out << " = vacuum.SpitOut()";
//         out << "); // used vacuum in reverse" << endl;
         break;
      case RELEASE_VACUUM:
         hand_kind = NONE_GIVEN;
         break;
      case RELEASE_ITEM_ON:
//         out << "  ";
			if (key == 1) { // new on 120201 - released on its stack
				java_assign(out,S(IDS_JAVA_HAND),S(IDS_JAVA_NULL));
			} else if (subject_path->pointer_to_next() == NULL) {
            int index = subject_path->return_index();
            if (index >= INTEGER_STACK && index <= BOMB_STACK && index != PROGRAM_PAD) {
               // just "returning" it to get rid of it
               java_assign(out,S(IDS_JAVA_HAND),S(IDS_JAVA_NULL));
            } else {
               const_string variable = NULL;
               if (next->first()->event_code() == NEW_ITEM) { 
                  // e.g. a box dropped on a number
                  if ((next->rest() != NULL && next->rest()->first()->event_code() == NEW_ITEM) || (previous_event != NULL && previous_event->event_code() == NEW_ITEM)) {
                     // this one counts -- the other one should be already bound -- check first?
                     variable = S2(IDS_JAVA_TEMP,next->rest()->first()->pointer_to_path()->return_index()+1);
//                     out << "temp" << (next->rest()->first()->pointer_to_path()->return_index()+1);
                  } else {
                     variable = S2(IDS_JAVA_TEMP,next->first()->pointer_to_path()->return_index()+1);
//                     out << "temp" << (next->first()->pointer_to_path()->return_index()+1);
                  };
//                  out << " = ";
               } else if (previous_event != NULL && previous_event->event_code() == NEW_ITEM) { // comes out in the wrong order now (141100)
						variable = S2(IDS_JAVA_TEMP,previous_event->pointer_to_path()->return_index()+1);
					};
               java_assign_send(out,variable,subject_path,S(IDS_JAVA_GETS),S(IDS_JAVA_HAND));
            };
         } else {
            java_send(out,subject_path,TRUE,S(IDS_JAVA_HAND),S(IDS_JAVA_HOLE_GETS));
//            subject_path->java(out,TRUE,"hand","HoleGets");
         };
//         out << "; // dropped it on something" << endl;
         update_hand_kind_for_release();
         break;
      case RELEASE_ITEM_LEFT_OF:
         java_send(out,subject_path,S(IDS_JAVA_SIDE_GETS),S(IDS_JAVA_HAND),S(IDS_JAVA_FALSE));
//         out << "  ";
//         subject_path->java(out);
//         out << ".SideGets(hand,false); // dropped on the left side" << endl;
         break;
      case RELEASE_ITEM_RIGHT_OF:
         java_send(out,subject_path,S(IDS_JAVA_SIDE_GETS),S(IDS_JAVA_HAND),S(IDS_JAVA_TRUE));
//         out << "  ";
//         subject_path->java(out);
//         out << ".SideGets(hand,true); // dropped on the right side" << endl;
         break;
      case DROP_ITEM_IN_TRUCK:
      case GIVE_BIRD_ITEM:
         java_send(out,subject_path,S(IDS_JAVA_GETS),S(IDS_JAVA_HAND));
//         out << "  ";
//         subject_path->java(out);
//         out << ".Gets(hand); ";
//         if (code == DROP_ITEM_IN_TRUCK) {
//            out << "// gave bird something" << endl;
//         } else {
//            out << "// dropped something in a truck" << endl;
//         };
         update_hand_kind_for_release();
         break;
      case RELEASE_ITEM:
         if (!(hand_kind == COPIER && copier_in_hand_unattached) &&
             hand_kind != VACUUM && hand_kind != EXPANDER &&
             // and not a reference to the MAIN_CUBBY
             !(subject_path->top_level() && subject_path->return_index() == 0)) {
//            temp_index = (short int) (-subject_path->return_index());
//            out << "  temp" << temp_index << " = hand; // dropped it on the floor" << endl;
            java_assign(out,subject_path,S(IDS_JAVA_HAND));
				last_temp_variable_index = (short int) (-subject_path->return_index()); // new on 211100
				temp_variables_count = max(temp_variables_count,last_temp_variable_index); // new on 121103
//            out << "  ";
//            subject_path->java(out);
//            out << " = hand; // dropped it on the floor" << endl;
				if (hand_kind == BIRD) { // conditional new on 050201
					check_if_bird_hatches(next->first(),out);
				};
//         } else {
//            out << "  // dropped something" << endl;
         };
         update_hand_kind_for_release();
         break;
      case NEW_MAIN_CUBBY:
         // can this happen more than once in the same body???
         java_declare(out,S(IDS_JAVA_TT_OBJECT),S(IDS_JAVA_OLD_GIVEN),S(IDS_JAVA_GIVEN));
//         out << "  TTObject oldGiven = given; // save old version" << endl;
         java_assign(out,S(IDS_JAVA_GIVEN),subject_path);
			// following new on 060205 since may vacuum up or manipulate the old My Box portion
			last_temp_variable_index++;
			java_assign(out,S2(IDS_JAVA_TEMP,last_temp_variable_index),S(IDS_JAVA_OLD_GIVEN));
//         out << "  given = ";
//         subject_path->java(out);
//         out << ";" << endl;
         new_main_cubby = TRUE;
         break;
      case KEYBOARD_CHARACTER:
      case KEYBOARD_CHARACTER_AND_SELECT:
			{ 
				const_java_string variable = NULL;
				character arg1[5];
				const_java_string arg2, arg3; // arg3 new on 070205 to indicate whether extended character
				if (next->first()->event_code() == NEW_ITEM) {
					// e.g. a piece broke off of a box
	//            next->first()->pointer_to_path()->java(out);
					last_temp_variable_index = next_path_index+1; // updating last_temp_variable_index is new on 060205
					variable = S2(IDS_JAVA_TEMP,last_temp_variable_index);
	//            out << "temp" << (next->first()->pointer_to_path()->return_index()+1);
	//            out << " = ";
				};
	//         if (hand_kind == NONE_GIVEN) {
	//            subject_path->java(out);
	//         } else {
	//            out << "hand";
	//         };
	//         out << ".Keyboard(\'";
				int j = 0;
				arg1[j++] = '\'';
            if (new_line(key)) { // prior to 090802 was == '\r') {  // new line
					arg1[j++] = '\\';
					arg1[j++] = 'r';
	//            out << "\\" << "r";   // print \r without breaking line
				} else if (key == BACKSPACE) {
	//            out << "\\" << "b";
					arg1[j++] = '\\';
					arg1[j++] = 'b';
				} else {
					arg1[j++] = key;
				};
				arg1[j++] = '\'';
				arg1[j] = NULL; // terminate string
				if (code == KEYBOARD_CHARACTER_AND_SELECT) {
					arg2 = S(IDS_JAVA_TRUE);
				} else {
					arg2 = S(IDS_JAVA_FALSE);
				};
				if (robot->can_send_keyboard_editing_keys()) { // new on 190205 so old robots produce "old" Java
					if (key > 255) {
						arg3 = S(IDS_JAVA_TRUE);
					} else {
						arg3 = S(IDS_JAVA_FALSE);
					};
				} else {
					arg3 = NULL; // don't emit third arg so old robots work
				};
				string keyboard_target = NULL; // new on 210205 to be sure that stoppedTyping goes to the right guy
				if (path_index == ITEM_HELD) { // commented out on 140101 || hand_kind == COPIER) {
					if (!copier_in_hand_unattached || (path_index == ITEM_HELD && hand_kind != COPIER)) {  // second conjunct added 301100
						// starting 021100 - ignore keyboard events to the copier for now since not explicitly represented (could track the copier state instead)
						keyboard_target = S(IDS_JAVA_HAND);
						java_assign_send(out,variable,keyboard_target,S(IDS_JAVA_KEYBOARD),arg1,arg2,arg3);
					};
				} else if (hand_kind == COPIER && !copier_in_hand_unattached) { // new on 230101
					keyboard_target = S2(IDS_JAVA_TEMP,last_temp_variable_index);
					java_assign_send(out,variable,keyboard_target,S(IDS_JAVA_KEYBOARD),arg1,arg2,arg3);
				} else {
					if (!vacuum_created && subject_path->references_dusty()) { // new on 141100 
						java_declare_new(out,S(IDS_JAVA_TT_VACUUM),S(IDS_JAVA_VACUUM),S(IDS_JAVA_TT_VACUUM)); // was java_assign_new from 140101 to 250101
						vacuum_created = true;
					} else if (!copier_created && subject_path->references_copier()) { // new on 140101
						java_declare_new(out,"TTCopier","copier","TTCopier"); // was java_assign_new prior to 250101
						copier_created = true;
					};
					java_assign_send(out,variable,subject_path,S(IDS_JAVA_KEYBOARD),arg1,arg2,arg3);
				};
				EventCode next_event_code = next->first()->event_code(); // new on 080205
				if (next_event_code != KEYBOARD_CHARACTER && next_event_code != KEYBOARD_CHARACTER_AND_SELECT) {
					if (keyboard_target != NULL) {
						java_send(out,keyboard_target,"stoppedTyping"); // new on 210205
					} else {
						java_send(out,subject_path,"stoppedTyping"); // new on 080205 to mark the last keyboard event
					};
				};
			};
			break;
      case LABEL_CHARACTER: // labels are really comments so ignore
      case DESTROY_HOUSE: // redundant
         break;
      case APPLY_GRASPED_ITEM:
         if (hand_kind == BOMB) {
            out << "  " << S(IDS_JAVA_THROW_TT_EXCEPTION)
                << " " << S(IDS_JAVA_BOMB_SET_OFF_COMMENT) << endl;
            return(FALSE); // following statements can never be reached
         } else {
            check_if_remote_notebook(next->first(),out);
            java_send(out,subject_path,S(IDS_JAVA_USE));
         };
         break;
      default:  // more later
         out << "  // " << event_string(code) << endl; // e.g. picking up and dropping Pumpy ...
   };
   return(TRUE);
};

#if TT_DEBUG_ON
void Event::debug(Robot *robot) {
  tt_error_file() << event_string(code) << " ";
  if (subject_path != NULL) {
	  if (code == NEW_ITEM) {
		  tt_error_file() << (subject_path->return_index()+1) << " "
						  << type_string(subject_path->pointer_to_next()->return_index());
	  } else if (code == START_EVENT || code == LAST_EVENT) {
		  tt_error_file() << subject_path->return_index();
     } else if (code == SERIAL_NUMBER) {
        tt_error_file() << serial_number();
	  } else {
		  subject_path->debug(TRUE);
	  };
  };
  if (key != 0) {
     if (key >= ' ') {
	     tt_error_file() << " " << key;
     } else {
        tt_error_file() << " control char " << (int) key;
     };
  };
  if (robot != NULL &&
	   (code == RELEASE_ITEM || code == RELEASE_ITEM_ON || code == GIVE_BIRD_ITEM ||
	    code == RELEASE_ITEM_LEFT_OF || code == RELEASE_ITEM_RIGHT_OF)) {
		Sprite *tool = robot->pointer_to_tool();
		if (tool == NULL) {
			tt_error_file() << " ROBOT WAS NOT HOLDING ANYTHING! ";
		} else {
			tt_error_file() << " ";
			if (tool->kind_of() == COPIER && ((Copier *) tool)->attached_tool() != NULL) {
				((Copier *) tool)->attached_tool()->describe_briefly(tt_error_file());
			} else {
				tool->describe_briefly(tt_error_file());
			};
		};
  };
  tt_error_file() << endl;
};

//string event_string(EventCode code) {
//	return(event_text_string[code]);
//};
#endif

boolean Event::dump(output_stream &stream) {
  stream.put((unsigned char) code);
//  if (code == START_EVENT) return; // not needed
  if (code == LAST_EVENT) return(FALSE); // no args and nothing follows
  if (subject_path != NULL) {
	  subject_path->dump(stream,(code != SERIAL_NUMBER));
//     int index = subject_path->return_index();
//     if (index < 0 || index == MAIN_CUBBY || index == PROGRAM_PADS) {
		  stream.put(END_OF_PATH_MARKER);
//     };
  };
//  if (key != 0) {
  if (code == KEYBOARD_CHARACTER && key > 255) { 
	  // new on 160205 so Marty can described extended keys
	  stream.put((unsigned char) 255);
  };
	  stream.put(key);
//	  stream.write((unsigned char *) &parameter, sizeof(parameter));
	  // no end of parameter marker since event code determines whether
	  // there is a parameter
//  };
  return(TRUE);
};

#if TT_XML

wide_string xml_event_string[] = // update tag_string[] if changing this
// on 170603 reverted to using special events for DropVacuum, etc. since when saved in a Queue there isn't enough context to disambiguate
  {L"START_EVENT",L"MoveTo", // L"SelectFromStack",
   L"MadeOrMoved",
	L"GraspMagicWand",L"UseMagicWand",
//   L"Grasp", L"UseOn",
	L"GraspBikePump",
//   L"Grasp",
	L"DropMagicWand",
//   L"Drop",
	L"Grasp",L"Drop",L"DropOn",
	L"DropOnLeftSide",L"DropOnRightSide",
	L"UseBikePump",
//   L"UseOn",
	L"GraspVacuum",L"UseVacuum",L"UseVacuumInReverse",L"DropVacuum",
//   L"Grasp",L"UseOn",L"UseVacuumInReverse",L"Drop",
	L"TypeTo",
	L"Hatch", // needed??
   L"GiveBird", // could DropOn be used instead? -- would need to rationalize describe, Java, etc.
	L"DropInTruck", // could DropOn be used instead?
	L"BlowUpHouse",
	L"Use",
	L"LAST_EVENT", //  -- why here??
	L"TypeLabel",
	L"SelectAndType",
	L"GraspNest", // unlike GRASP_ITEM won't take top element off stack
	L"ReplaceMyBox",
	L"UseMagicWandOnNest", // rationalize this and others
	L"UseVacuumOnNest",
   L"APPLY_GRASPED_NEST", // not used just now
   L"DropBikePump"
//   L"Drop"
  };
// L"SERIAL_NUMBER", // of this sequence of events
//	L"NON_EVENT"}; // used for range checking

void Event::xml(xml_element *parent, xml_document *document) {
   xml_element *event_element = NULL;
   switch (code) {
      case START_EVENT: 
         xml_set_attribute(parent,L"MadeOrMovedCount",subject_path->return_index());
         return; 
      case SERIAL_NUMBER:
         xml_set_attribute(parent,L"ID",serial_number());
         xml_set_attribute(parent,L"Version",robot_version_number());
         return;
      case LAST_EVENT:
         // subject_path->return_index()
         return;
      default: 
         if (subject_path != NULL) {
	         if (code == NEW_ITEM) {
               event_element = create_xml_element(L"MadeOrMoved",document,subject_path->return_index()+1); 
					// 1-indexing friendlier, right?
               int type_code = subject_path->pointer_to_next()->return_index();
               xml_set_attribute(event_element,L"TypeCode",type_code);
//               xml_set_attribute(event_element,L"TypeDescription",type_string(type_code));
	         } else {
               const int path_string_size = 1024;
               wide_character path_string[path_string_size];
               if (subject_path->xml_path_string(path_string,0,path_string_size-30)) {
                  // -30 since each step in the path is always less than 30 characters long
		            event_element = create_xml_element(xml_event_string[code],document,path_string);
               }; // else try again with larger path?? or warn??
	         };
         } else {
            event_element = create_xml_element(xml_event_string[code],document);
         };
         if (event_element == NULL) return; // doesn't happen, right?
   }; // the following could be moved up as part of the default: case
   if (key != 0) {
      if (code == EXPANDER_APPLY) { // only Pumpy encodes a duration of use - right?
         xml_set_attribute(event_element,L"Duration",decode_duration(key)); 
			// should rationalize this some day - encoding duration as key is goofy -- done on 280704 but internally still use key
      } else {
         if (key != 1) { // no point generating confusing stuff for human consumption if just a flag (e.g. return to stack)
            //wide_character key_string[2];
            //key_string[0] = key;
            //key_string[1] = NULL;
            // key_string as the "truth" caused many problems with spaces and new lines
				// new version as of 100604
				// character_name already deals with the encoding of extended by adding 256
            xml_set_attribute(event_element,L"KeyDescription",character_name(key,tt_language)); // for human consumption
         };
         xml_set_attribute(event_element,L"KeyCode",key);
      };
   };
	xml_append_child(event_element,parent); // moved here on 190804
//   return(event_element); // commented out on 20084
};

SpriteType type_of_thing_held = NONE_GIVEN;
SpriteType type_of_attached_to_wand = NONE_GIVEN;

Events *last_events = NULL;

void handle_actions_tag(Entity *robot, Tag tag, xml_node *node, void *extra_info) {
   Events *current_events = (Events *) extra_info;
   if (last_events == NULL) {
      last_events = current_events;
   };
   Event *event = NULL;
//   Events *events = NULL; // so some tags can generate more than one event
   SpriteType path_type;
   Path *path = NULL;
	if (tag != NO_MORE_TAGS) { // conditional new on 160103
		path = xml_decode_event_path(node,path_type);
	};
   switch (tag) {
      case MOVE_TO_TAG:
         //events = new Events(new Event(SELECT_STACK_ITEM,path),
         //                    new Events(new Event(NEW_ITEM,new Path(xml_get_attribute_int(node,L"Index",1)-1,
         //                                                           new Path(xml_get_attribute_int(node,L"TypeCode",0))))));
         event = new Event(SELECT_STACK_ITEM,path);
         break;
      case MADE_OR_MOVED_TAG:
         if (path != NULL) { // should always be a path just containing the working set index
            path->set_index((-path->return_index())-1); // comes out negative since xml_decode_event_path treats it that way
            // and convert from 1-indexing to zero
            path->set_next(new Path(xml_get_attribute_int(node,L"TypeCode",0)));
         };
         event = new Event(NEW_ITEM,path);
         break;
      case GRASP_TAG: {
         EventCode code;
         switch (path_type) {
            case COPIER:
               code = GRASP_COPIER;
               break;
            case VACUUM:
               code = GRASP_VACUUM;
               break;
            case EXPANDER:
               code = GRASP_EXPANDER;
               break;
            default:
               code = GRASP_ITEM;
         };
         type_of_thing_held = path_type;
         event = new Event(code,path);
         break;
                      };
      case USE_TAG:
         event = new Event(APPLY_GRASPED_ITEM,path);
         break;
      case USE_ON_TAG:
			if (type_of_thing_held == NONE_GIVEN && robot != NULL) { // new on 171202
				Sprite *tool = ((Robot *) robot)->pointer_to_tool();
				if (tool != NULL) {
					type_of_thing_held = tool->kind_of();
				};
			};
         switch (type_of_thing_held) {
            case COPIER:
               event = new Event(COPIER_APPLY,path);
               type_of_attached_to_wand = path_type;
               break;
            case VACUUM:
               event = new Event(VACUUM_APPLY,path);
               break;
				case EXPANDER: {
					millisecond duration = xml_get_attribute_int(node,L"Duration",-1);
					if (duration < 0) { // no duration
	               event = new Event(EXPANDER_APPLY,path,0);
					} else {
						event = new Event(EXPANDER_APPLY,path,encode_duration(duration));
					};
									};
               break;
				default:
					log("ToonTalk doesn't know what tool a loaded robot should use.");
         }; // else what?? - warn??
         break;
      case DROP_TAG:
      case DROP_ON_TAG:
      case DROP_ON_LEFT_SIDE_TAG:
      case DROP_ON_RIGHT_SIDE_TAG: {
         EventCode drop_code;
         if (tag == DROP_TAG) {
            switch (type_of_thing_held) {
               case COPIER:
                  drop_code = RELEASE_COPIER;
                  break;
               case VACUUM:
                  drop_code = RELEASE_VACUUM;
                  break;
               case EXPANDER:
                  drop_code = RELEASE_EXPANDER;
                  break;
               default:
                  drop_code = RELEASE_ITEM;
            };
         };
         if (type_of_thing_held == COPIER) {
            if (type_of_attached_to_wand != NONE_GIVEN) {
               type_of_attached_to_wand = NONE_GIVEN;
            } else {
               type_of_thing_held = NONE_GIVEN;
            };
         } else {
            type_of_thing_held = NONE_GIVEN;
         };
         switch (tag) {
            case DROP_TAG:               
               event = new Event(drop_code,path);
               break;
            case DROP_ON_TAG:
               // last arg added on 041002 since can be a flag to indicate dropped on its own stack -- are there others like this??
               event = new Event(RELEASE_ITEM_ON,path,xml_get_attribute_int(node,L"KeyCode"));
               break;
            case DROP_ON_LEFT_SIDE_TAG:
               event = new Event(RELEASE_ITEM_LEFT_OF,path);
               break;               
            case DROP_ON_RIGHT_SIDE_TAG:
               event = new Event(RELEASE_ITEM_RIGHT_OF,path);
               break;
         };
         break;
                                   };
      case TYPE_TO_TAG:
         event = new Event(KEYBOARD_CHARACTER,path,xml_get_attribute_int(node,L"KeyCode"));
         break;
      case HATCH_TAG:
         event = new Event(HATCH_BIRD,path);
         break;
      case GIVE_BIRD_TAG:
         event = new Event(GIVE_BIRD_ITEM,path);
         break;
      case DROP_IN_TRUCK_TAG:
         event = new Event(DROP_ITEM_IN_TRUCK,path);
         break;
      case BLOW_UP_HOUSE_TAG:
         event = new Event(DESTROY_HOUSE,path);
         break;
      case TYPE_LABEL_TAG:
         event = new Event(LABEL_CHARACTER,path,xml_get_attribute_int(node,L"KeyCode"));
         break;
      case SELECT_AND_TYPE_TAG:
         event = new Event(KEYBOARD_CHARACTER_AND_SELECT,path,xml_get_attribute_int(node,L"KeyCode"));
         break;
      case GRASP_NEST_TAG:
         event = new Event(GRASP_NEST,path);
         break;
      case REPLACE_MY_BOX_TAG:
         event = new Event(NEW_MAIN_CUBBY,path);
         break;
      case USE_MAGIC_WAND_ON_NEST_TAG:
         event = new Event(COPIER_APPLY_NEST,path);
         break;
      case USE_VACUUM_ON_NEST_TAG:
         event = new Event(VACUUM_APPLY_NEST,path);
         break;
      case USE_VACUUM_IN_REVERSE_TAG:
         // this could be rationalized but used by describe, Java and more
         event = new Event(VACUUM_APPLY_RESTORE,path);
         break;
		// following new on 170603
		case GRASP_MAGIC_WAND_TAG:
			event = new Event(GRASP_COPIER,path);
         break;
		case USE_MAGIC_WAND_TAG:
			event = new Event(COPIER_APPLY,path);
         break;
		case GRASP_BIKE_PUMP_TAG:
			event = new Event(GRASP_EXPANDER,path);
         break;
		case DROP_MAGIC_WAND_TAG:
			event = new Event(RELEASE_COPIER,path);
         break;
		case USE_BIKE_PUMP_TAG: {
			millisecond duration = xml_get_attribute_int(node,L"Duration",-1);
			if (duration < 0) { // no duration
	         event = new Event(EXPANDER_APPLY,path);
			} else {
				event = new Event(EXPANDER_APPLY,path,encode_duration(duration));
			};
										};
         break;
		case GRASP_VACUUM_TAG:
			event = new Event(GRASP_VACUUM,path);
         break;
		case USE_VACUUM_TAG:
			event = new Event(VACUUM_APPLY,path);
         break;
		case DROP_VACUUM_TAG:
			event = new Event(RELEASE_VACUUM,path);
         break;
		case DROP_BIKE_PUMP_TAG:
			event = new Event(RELEASE_EXPANDER,path);
         break;
      case NO_MORE_TAGS:
         last_events = NULL;
         return;
      case NO_SUCH_TAG:
         log("Something wrong with XML describing the actions of a robot.",FALSE,TRUE);
#if TT_DEBUG_ON
			debug_this();
#endif
         // could report robot name, etc.
   };
   //if (events != NULL) {
   //   current_events->last()->set_rest(events);
   //} else 
   if (event != NULL) {
      if (last_events->first() == NULL) {
         last_events->set_first(event); // new on 071102 since is from tt_queue isn't always a full body
      } else {
         Events *new_last_events = new Events(event);
         last_events->last()->set_rest(new_last_events); // ooptimized by keeping track of last_cons (new on 071102)
         last_events = new_last_events;
      };
   };
};

Events *xml_load_events(xml_node *node, boolean whole_body, Robot *robot) { // robot new arg on 171202
   Events *events;
   if (whole_body) {
      int serial_number = xml_get_attribute_int(node,L"ID",0);
      int version_number = xml_get_attribute_int(node,L"Version",latest_robot_version);
      int working_set_size = xml_get_attribute_int(node,L"MadeOrMovedCount",0);
      events = new Events(create_serial_number(serial_number,version_number),new Events(create_first_event(working_set_size)));
   } else {
      events = new Events(NULL);   
   };
   xml_process_children(node,handle_actions_tag,robot,events); // robot was NULL prior to 171202
	if (events->first() == NULL) { // can happen if a robot trained to do nothing is running off screen -- new on 241003
		events->set_first(create_last_event());
	} else
   events->last()->set_rest(new Events(create_last_event()));
   return(events);
};

#endif

Path::~Path() {
	if (next != NULL) {
		delete next;
	};
};

const int last_builtin_page_number = 12; // of main notebook -- move elsewhere?

PathDereferenceStatus Path::dereference(Background *floor,
													 Robot *robot,
													 Cubby *top_cubby,
													 EventCode event_code,
													 SpritePointer &tool,
													 SpritePointer &holder, 
													 long &last_index,
													 millisecond &duration) {
	holder = NULL;
	last_index = index;
	Sprite *top_level_item = NULL;
	boolean sharable_item = FALSE;
	tool = NULL; // until recomputed below 
	boolean dont_wait = TRUE; // unless an empty nest is found
	if (index < 0) {
		top_level_item = robot->working_set_nth((-index)-1);
	} else {
		boolean ignore_new_flag = FALSE; // wasn't initialized prior to 131000
		switch (index) {
			case MAIN_CUBBY:
				top_level_item = top_cubby;
				break;
			case NO_ARG:
				tool = NULL;
				return(PATH_DEREFERENCE_OK);
			case ITEM_HELD:
				tool = robot->pointer_to_tool();
            break;
			case A_COPIER:
				tool = robot->pointer_to_copier(ignore_new_flag); // was tt_toolbox
				break;
         case AN_EXPANDER:
            tool = robot->pointer_to_expander(ignore_new_flag);
            break;
			case A_VACUUM:
				tool = robot->pointer_to_vacuum(ignore_new_flag);
				break;
			// these stacks could be optimized by selecting them from an array	
			case INTEGER_STACK:
				if (tt_toolbox != NULL) tool = tt_toolbox->pointer_to_integer_stack();
				break;
			case TEXT_STACK:
				if (tt_toolbox != NULL) tool = tt_toolbox->pointer_to_text_stack();
				break;
			case NEST_STACK:
				if (tt_toolbox != NULL) tool = tt_toolbox->pointer_to_nest_stack();
				break;
			case CUBBY_STACK:
				// why different?
				if (tt_toolbox != NULL) top_level_item = tt_toolbox->pointer_to_cubby_stack();
				sharable_item = TRUE;
				break;
			case BOMB_STACK:
				if (tt_toolbox != NULL) tool = tt_toolbox->pointer_to_bomb_stack();
				break;
			case TRUCK_STACK:
				if (tt_toolbox != NULL) tool = tt_toolbox->pointer_to_truck_stack();
				break;
			case ROBOT_STACK:
				if (tt_toolbox != NULL) tool = tt_toolbox->pointer_to_robot_stack();
				break;
			case SCALE_STACK:
				if (tt_toolbox != NULL) tool = tt_toolbox->pointer_to_scale_stack();
				break;
			case PROGRAM_PADS:
//    Unclear how to interpret things like dropping text to find page...  
//				if (next != NULL && next->return_index() > last_builtin_page_number) {
					// warning this is DWIM (do what I mean)
					// it looks in notebook robot has been placed in if accessing user pages
//					top_level_item = robot->pointer_to_home_notebook();
//				};
//				top_level_item = robot->pointer_to_home_notebook();
//				if (top_level_item == NULL) { // doesn't have one so use tool box's
//					top_level_item = pointer_to_toolbox_notebook();
//				};
            top_level_item = robot->notebook_to_use((short int) (next == NULL?0:next->return_index()));
//				dereference_pad((Notebook *) top_level_item,robot,floor,tool);
//				sharable_item = TRUE;
				break;
		  default:
//			  tt_error_file() << "Unrecognized path index: " << index;
//			  tt_exit_failure();
			  tool = NULL;
			  say_error(IDS_COULDNT_FIND_SOMETHING,TRUE);
			  return(PATH_DEREFERENCE_OK); // since tool is NULL will show robot confusion
		};
	};
	if (event_code == LABEL_CHARACTER && tt_log_version_number >= 66) { // new on 050105 
		duration = 100; // otherwise it is really slow watching a robot type a label
	} else if (robot->showing_on_a_screen()) {
		city_coordinate robot_llx,robot_lly;
		robot->lower_left_corner(robot_llx,robot_lly);
		city_coordinate tool_llx,tool_lly;
		if (tool != NULL) {
			tool->lower_left_corner(tool_llx,tool_lly);
		} else if (top_level_item != NULL) {
			top_level_item->lower_left_corner(tool_llx,tool_lly);
		} else {
			tool_llx = 0;
			tool_lly = 0;			
		};
		duration = duration_delta_location(robot_llx,robot_lly,tool_llx,tool_lly);
	} else {
		duration = 0;
	};
	if (tool != NULL && top_level_item == NULL) {
      // had better not be taken by programmer -- what about a truck or bird?
      if (tool->pointer_to_background() != floor &&
          tool->current_held_status() == HELD_BY_PROGRAMMER &&
          tool->kind_of() != PROGRAM_PAD) { // pads are shared by robots and programmer
         tool = NULL; // abort
			return(PATH_DEREFERENCE_ABORT);
      };
      return(PATH_DEREFERENCE_OK); // done
   };
	boolean wait = FALSE;
	boolean done = FALSE;
	top_level_item = need_to_wait(event_code,top_level_item,(next == NULL),wait,done);
	if (done) {
		tool = top_level_item;
		return(PATH_DEREFERENCE_OK);
	};
	if (wait) return(PATH_DEREFERENCE_WAIT);
   if (top_level_item != NULL && // top_level_item->pointer_to_background() != floor &&
       top_level_item->current_held_status() == HELD_BY_PROGRAMMER &&
       top_level_item->kind_of() != PROGRAM_PAD) { // pads are shared by robots and programmer
		// could wait -- better???
      tool = NULL;
      return(PATH_DEREFERENCE_ABORT); // abort
   };
	if (top_level_item == NULL) {
		tool = NULL;
	} else if (top_level_item->kind_of() == PROGRAM_PAD) {
		Notebook *notebook = (Notebook *) top_level_item;
		dereference_pad(notebook,robot,tool,event_code); // event_code new on 141200
//	   if (tt_log_version_number > 13) 
		holder = notebook;
		if (duration > 0 && tool != NULL && event_code == GRASP_ITEM) { // && tt_log_version_number > 14
			// new on 230499 so it looks much nicer
			tool->animate_to_good_size(duration,tt_log_version_number >= 64?tt_toolbox:NULL);
		};
//		if (duration != 0) {
//		   notebook->set_recent_page_number(robot->return_recent_page_number());
//			notebook->do_after(duration,notebook,go_to_recent_page_callback);
//		};
		return(PATH_DEREFERENCE_OK);
//		tool = top_level_item->selected(GRASPED,robot);
	} else if (next != NULL) {
		// following added on 260499 by abstracting what was there before so dereference_cubby
		// can call it too
//		boolean dont_wait = TRUE;
		if (dereference_level(top_level_item,floor,robot,top_cubby,event_code,
			                   tool,holder,dont_wait,last_index)) {
			// finished
			if (dont_wait) {
				return(PATH_DEREFERENCE_OK); // was just this branch prior to 101199
			} else {
				return(PATH_DEREFERENCE_WAIT);
			};
		};
	} else {
		tool = top_level_item;
	};
// taken care of elsewhere
//	} else if (next != NULL && top_level_item->kind_of() == NEST) {
//		Nest *nest = (Nest *) top_level_item;
//		tool = nest->pointer_to_contents();
//		if (tool == NULL) {
//			say(S(IDS_EXPECTED_TO_FIND_SOMETHING_IN_NEST),4);
//			tool = NULL;
//			holder = NULL;
//			return(TRUE); // aborting (rather than waiting)
//		} else if (next->pointer_to_next() != NULL) { // its contents is cubby ...
//			Cubby *cubby_in_nest = (Cubby *) tool;
//			Cubby *cubby_holder; // ingored...
//			dont_wait = next->dereference_cubby(cubby_in_nest,event_code,
//															tool,cubby_holder,last_index);
//			holder = nest;
//		};
	if (tool != NULL && tool->inside_vacuum() && event_code != VACUUM_APPLY_RESTORE) { // last conjunct added on 110699
		robot->say_a_robot_named(0,IDS_ROBOT_COULDNT_USE_IN_VACUUM);
		tool = NULL;
		holder = NULL;
	};
	if (holder != NULL && holder->inside_vacuum()) {
		robot->say_a_robot_named(0,IDS_ROBOT_COULDNT_PUT_IN_VACUUM);
		tool = NULL;
		holder = NULL;
	};
	if (tool != NULL && !sharable_item && 
//		 floor != NULL && // never was on the floor
//		 tool->pointer_to_background() != floor) {// not on the floor anymore
		 holder != NULL &&
		 robot->pointer_to_background() != holder->ultimate_container()->pointer_to_background() &&
		 // added ultimate_container above on 200305 since if a box in a box on a nest it may have a NULL floor
		 robot->pointer_to_background()->pointer_to_picture() == NULL) { // added and on not back of picture on 040899
		robot->say_a_robot_named(0,IDS_ROBOT_THING_NOT_IN_ROOM); 
		tool = NULL;
		holder = NULL;
	};
	if (dont_wait) {
		return(PATH_DEREFERENCE_OK); // was just this branch prior to 101199
	} else {
		return(PATH_DEREFERENCE_WAIT);
	};
};

SelectionReason selection_reason(EventCode code) {
	switch (code) {
		case GRASP_COPIER:
		case GRASP_EXPANDER:
		case GRASP_ITEM:
		case GRASP_VACUUM:
		case GRASP_NEST:
			return(GRASPED);
		case GIVE_BIRD_ITEM:
		case DROP_ITEM_IN_TRUCK:
		case RELEASE_EXPANDER:
			return(ITEM_DROPPED);
		default:
			return(NO_SELECTION_REASON_GIVEN);
	};
};

boolean Path::dereference_level(Sprite *previous_level_item,
										  Background *floor,
										  Robot *robot,
										  Cubby *top_cubby,
										  EventCode event_code,
										  SpritePointer &tool,
										  SpritePointer &holder,
										  boolean &dont_wait,
										  long &last_index) {
	if (previous_level_item == NULL) {
		// warn??
		return(TRUE);
	};
	switch (previous_level_item->kind_of()) {
		case TRUCK:
			{
			Truck_Inside *truck = ((Truck_Inside *) previous_level_item);
			holder = truck;
			Sprite *cargo = truck->cargo((CargoType) next->return_index());
			//		Cubby *trucks_cubby = ((Truck *) top_level_item)->pointer_to_cubby();
			//		if (trucks_cubby != NULL) {
			//			next->dereference_cubby(trucks_cubby,tool,holder,last_index);
			//		} else {
			if (cargo == NULL) { 
				robot->say_a_robot_named(0,IDS_ROBOT_DIDNT_FIND_ROBOT_IN_TRUCK+next->return_index(),4);
				tool = NULL;
				holder = NULL;
				return(TRUE); // aborting (rather than waiting)
			} else if (next->pointer_to_next() != NULL) { // new on 070799
				return(next->dereference_level(cargo,floor,robot,top_cubby,event_code,tool,holder,dont_wait,last_index));
			} else {
				tool = cargo;
			};
			};
			break;
		case CUBBY:
			if (!previous_level_item->is_picture()) {
				dont_wait = next->dereference_cubby((Cubby *) previous_level_item,event_code,
																tool,holder,last_index,robot,robot,top_cubby,floor);
				break;
			}; // added 121199 - else fall thru to the following - e.g. a parts or container sensor
		case REMOTE_PICTURE:
//			if (event_code == VACUUM_APPLY) {
//				tool = previous_level_item->selected(VACUUM_USED,robot); 
//			} else { 
//				tool = previous_level_item; // should this be dererenced???
//			};
			if (previous_level_item != NULL) {
				tool = previous_level_item->selected(selection_reason(event_code),robot); // new more general way on 121199
			};
			break;
		case PICTURE:
			if (next->return_index() == SPECIAL_PATH_MARKER) {
//				((Picture *) previous_level_item)->copy_flipped_followers_of_controlee_now(); // new on 091100
				Sprite *follower = previous_level_item->first_follower_of_type((SpriteType) next->pointer_to_next()->return_index());
				if (follower != NULL) {
					tool = follower;
				} else {
					// the idea is that if the expected item is missing just vacuum up the whole container
					// kind of the way Dusty works anyway.
					// This makes it easier to dispatch on what you find on the back of a picture.
					tool = previous_level_item;
				};
				// following given second condition on 161105 since interferes with retraining 
				// since can cause remove_selected_element
				// and then there is no path for the retraining
				if (tool != NULL && robot->kind_of() != ROBOT_IN_TRAINING) { // new on 121199
					tool = tool->selected(selection_reason(event_code),robot);
				};
			};
			break;
		case ROBOT:
			{
			Robot *robot = (Robot *) previous_level_item;
			if (event_code == RELEASE_ITEM_ON) {
				tool = robot->pointer_to_thought_bubble();
			} else {
				Cubby *cubby = robot->pointer_to_thought_bubble()->pointer_to_cubby();
				if (cubby == NULL) {
					tool = previous_level_item;
				} else if (next != NULL) {
//					Cubby *cubby_holder; // = (Cubby *) holder;
					Path *remaining_path = next->skip_over_type_path();
					if (remaining_path != NULL) {
						dont_wait = remaining_path->dereference_cubby(cubby,event_code,
																					 tool,holder,last_index,
																					 robot,robot,top_cubby,floor);
//						holder = cubby_holder;
					} else {
						tool = cubby;
					};
				};
			};
			};
			break;
		default:
			tool = previous_level_item;
	};
	return(FALSE);
};

Path *Path::skip_over_type_path() {
	Path *current = this;
	if (current->return_index() == SPECIAL_PATH_MARKER) {
		// currently only used to describe it properly between SPECIAL_PATH_MARKER is path of types
		current = current->pointer_to_next();
		while (current != NULL) { 
			int current_index = current->return_index();
			if (current_index == SPECIAL_PATH_MARKER) return(current->pointer_to_next());
//			if (current_index == END_OF_PATH_MARKER) return(NULL);
			current = current->pointer_to_next();
		};
//		if (current != NULL) current = current->pointer_to_next();
	};
	return(current);
};

#if TT_BETA
boolean warned_about_old_pad_references = FALSE;
#endif

void Path::dereference_pad(Notebook *notebook, Robot *robot, SpritePointer &tool, EventCode event_code) {
#if TT_DEBUG_ON
	if (robot->debug_counter == tt_debug_target) {
		log("Debug target dereferencing a notebook.");
	};
#endif
	if (next == NULL) { // e.g. when vacuuming up entire pad
//		if (notebook->showing_on_a_screen()) {
//			notebook->set_recent_page_number(-1); // to manipulate the whole thing
//			robot->set_recent_page_number(-1);
//		};
		tool = notebook;
		return;
	};
	if (event_code == VACUUM_APPLY) { // new on 141200
		boolean left_page = next->return_index();
//		if (!notebook->showing_on_a_screen()) {
			notebook->go_to_page(notebook->left_page_number(robot)); // to make sure the pages are up-to-date
//		};
		if (left_page) {
			tool = notebook->pointer_to_left_page();
		} else {
			tool = notebook->pointer_to_right_page();
		};
		if (tool == NULL) {
			robot->say_a_robot_named(0,IDS_SOMETHING_NOT_IN_NOTEBOOK);
			return;
		};
		Sprite *new_tool = tool->selected(VACUUM_USED,robot);
		if (new_tool == tool) {
			tool->increment_ref_count(); // new on 150103 since notebooks now destroy left and right page when flipping
		} else {
			tool = new_tool;
		};
		return;
	};
//				pad_number = next->return_index();
//				page_number = next->pointer_to_next()->return_index();
	short int page_number = (short int) next->return_index();
//				tool = (Tool *) tt_main_notebook->nth(pad_number);
//				if (tool == NULL) {
//					tt_error_file() << "Warning pad number " << pad_number << " missing." << endl;
//					return;
//				};
//				pad = (Notebook *) tool;
				// should delay the visuals ...
//	if (robot->showing_on_a_screen()) {
#if TT_BETA
		if (page_number > 1) { // old convention -- simplified here
			if (!warned_about_old_pad_references) {
				say_error(_T("Either ToonTalk is broken or you are using a robot trained in an earlier version of ToonTalk. It might not work. Sorry."));
				warned_about_old_pad_references = TRUE;
			};
		} else    
#endif
			if (robot->kind_of() == ROBOT_IN_TRAINING) notebook->finish_instantly(); // so page number is up-to-date if animation is going on
			page_number += notebook->left_page_number(robot);
//			if (robot->kind_of() == ROBOT) { // if ROBOT_IN_TRAINING then notebook is open the right page
//				if (notebook == pointer_to_toolbox_notebook()) { // shared
//					page_number += robot->return_recent_page_number();
//				} else {
//					page_number += notebook->return_recent_page_number();
//				};
//			} else {
//				page_number += notebook->current_page_number();
//			};
//		robot->set_recent_page_number(page_number);
//	} else {
//		page_number += (short int) robot->page_number_offset(notebook);
//	};
	tool = notebook->pointer_to_page(page_number,robot,(event_code != COPIER_APPLY));
	// last arg added above so don't copy the page twice if using a wand on a notebook page
#if TT_DEBUG_ON
	if (tt_debug_mode == 120201 || tt_debug_target == notebook->debug_counter) {
		tt_error_file() << "Page #" << page_number << " of ";
		notebook->print(tt_error_file());
		tt_error_file() << " fetching ";
		tool->print(tt_error_file());
		long length;
		string text;
		if (tool != NULL && tool->current_text(text,length)) {
			tt_error_file() << " (" << text << ")";
		};
		tt_error_file() << endl;
	};
#endif
	if (tool == NULL) {
//		tt_error_file() << "Warning pad page number " << page_number << " missing." << endl;
		robot->say_a_robot_named(0,IDS_SOMETHING_NOT_IN_NOTEBOOK);
		return;
	};
	// ::select should take care of this -- maybe don't need the following line now even if ROBOT
	if (event_code != COPIER_APPLY) { // condition new on 180204
		if (robot->kind_of() != ROBOT_IN_TRAINING) {
			robot->add_to_working_set(tool,TRUE);
		} else {
			notebook->set_left_page_selected(page_number%2); // only used when re-training a robot 
		};
	};
   Background *floor = robot->pointer_to_background();
	// new on 180204 to not do this if COPIER applied to notebook page
	if ((event_code != COPIER_APPLY) && (tool->kind_of() != PROGRAM_PAD ||
//		 (!((Notebook *) tool)->vacuum_should_not_delete())) {
		// shared notebooks needn't be added 
		// except for the first time
		(floor != NULL && notebook->showing_on_a_screen()))) { 
//		 !floor->has_item(tool))) {
//		tool->set_priority(notebook->priority()+1); // hide under for a while -- commented out on 160103 since it looks bad
		floor->add_item(tool,TRUE,FALSE,FALSE); // TRUE was FALSE prior to 160103
	};
};

void Path::dereference_cubby(Cubby *cubby, SpritePointer &holder, long &last_index, Sprite *by) {
   // changed on 080699 so that holder needn't be a cubby
	// since in old scheme got an outer container when deeply nested.
	Sprite *ignore;
	dereference_cubby(cubby,NON_EVENT,ignore,holder,last_index,by,NULL,NULL,NULL);
};

boolean Path::dereference_cubby(Cubby *cubby, EventCode event_code,
										  SpritePointer &tool,
										  SpritePointer &holder,
										  long &last_index,
										  Sprite *by,
										  Robot *robot, Cubby *top_cubby, Background *floor) {
  if (cubby == NULL) {
	  tool = NULL;
	  return(TRUE); // got a problem
  };
#if TT_CAREFUL
  if (index == SPECIAL_PATH_MARKER) {
     // shouldn't happen
	  say_error(_T("Box path has special path marker in it."));
	  tool = NULL;
	  return(TRUE);
  };
#endif
  holder = cubby;
  last_index = index;
  Sprite *next_level = NULL;
  if (index >= cubby->current_number_of_holes()) { // improved the error handling here on 080499
	  tt_martian->action_aborted(IDS_BOX_DOESNT_HAVE_EXPECTED_HOLE,by);
  } else {
	  next_level = cubby->component(index); // coerced to (cubby_index) prior to 070203
  };
  boolean wait = FALSE;
  boolean done = FALSE;
  next_level = need_to_wait(event_code,next_level,(next == NULL),wait,done);
  if (done) {
	  tool = next_level;
	  return(TRUE);
  };
  if (wait) return(FALSE);
  if (next == NULL) {
	  tool = next_level;
  } else {
	  boolean dont_wait = TRUE; // was ignored prior to 101199
//	  SpritePointer sprite_holder = holder;
	  if (dereference_level(next_level,floor,robot,top_cubby,event_code,tool,holder,
									dont_wait,last_index)) {
		  // finished
		  return(dont_wait); // was TRUE prior to 101199
	  };
	  /* replaced all this with the more general code from dereference
	  if (next_level != NULL && next_level->kind_of() == REMOTE_PICTURE) { // e.g. CHILDREN_REMOTE or ANCESTORS_REMOTE
		  // next_level != NULL && added on 230499 so it doesn't crash when robot can't find a hole
		  next_level = ((Remote_Picture *) next_level)->pointer_to_appearance();
	  };
	  if (next_level != NULL && next_level->kind_of() != CUBBY) { 
		  // more trouble
//		  tt_error_file() << "Warning dereferencing a path and found non-cubby "
//						  << "or non-filled nest. "  << next_level->kind_of() << endl;
//		  say(S(IDS_BOX_OR_NEST_MISSING));
		  tt_martian->action_aborted(S(IDS_BOX_OR_NEST_MISSING),by);
	  } else {
		  return(next->dereference_cubby((Cubby *) next_level,event_code,
													tool,holder,last_index,by));
	  };
	  */
  };
  return(TRUE);
};

Sprite *need_to_wait(EventCode event_code, Sprite *next_level, boolean last_in_path, boolean &wait, boolean &done) {
	switch (event_code) {
		case GRASP_NEST:
		case COPIER_APPLY_NEST:
		case VACUUM_APPLY_NEST:
		case VACUUM_APPLY_RESTORE: // huh??
		case APPLY_GRASPED_NEST:
			return(next_level);
		default:
			if (!last_in_path && next_level != NULL && next_level->kind_of() == REMOTE_PICTURE && event_code == VACUUM_APPLY) {
//				done = TRUE;
//				return(next_level->selected(VACUUM_USED,NULL));
				// rewritten on 260499
				Sprite *result = next_level->selected(VACUUM_USED,NULL);
				if (result == next_level) { // is kind of remote that returns self rather than a part
					done = TRUE;
				};
				return(result);
			};
			return(recursively_dereference_nest(next_level,event_code,wait));
	};
};

Sprite *recursively_dereference_nest(Sprite *next_level, EventCode event_code, boolean &wait) {
	while (next_level != NULL && next_level->kind_of() == NEST) {
		Sprite *nest_contents = ((Nest *) next_level)->pointer_to_contents(TRUE); // TRUE arg added on 120203
		if (nest_contents != NULL) {
			next_level = nest_contents;
//			if (next_level != NULL && next_level->kind_of() == NEST) {
//				tt_error_file() << "debug" << endl;
//			};
		} else if (event_code != RELEASE_ITEM_ON) {
			// wait for nest to fill unless dropping something on it
			wait = TRUE;
			return(NULL);
		} else {
			return(next_level);
		};
	};
//#if TT_DEBUG_ON
//	perform_integrity_checks(next_level);
//#endif
	return(next_level); 
};

void Path::add_to_end(Path *new_end) {
	if (next == NULL) {
		next = new_end;
	} else {
		next->add_to_end(new_end);
	};
};

void Path::kind_is(SpriteType &kind) {
   if (index < 0 || next != NULL) { // can be more clever later
//      kind = NONE_GIVEN; // leave it alone
   } else {
      if (index == 13) return; // 13 is the hand so leave it alone
      SpriteType types[] 
	   		 = {CUBBY,NONE_GIVEN,COPIER,VACUUM,
                INTEGER,TEXT,CUBBY,NEST,TRUCK,PROGRAM_PAD,
                ROBOT,SCALE_PICTURE,BOMB,NONE_GIVEN,EXPANDER};
	   kind = types[index];		 
   }; 
};

void Path::java(java_stream &out, boolean first,
                const_java_string set_new_value, const_java_string set_method) {
	if (index == SPECIAL_PATH_MARKER) {
		// currently only used to describe it properly between SPECIAL_PATH_MARKER is path of types
		Path *current = next;
		while (current != NULL && current->return_index() != SPECIAL_PATH_MARKER) {
			current = current->pointer_to_next();
		};
		if (current != NULL) current = current->pointer_to_next();
		if (current != NULL) {
			current->java(out,first,set_new_value,set_method);
		} else if (next != NULL) {
//			const_java_string type_name = indefinite_type((SpriteType) next->return_index());
//			java_string java_type_name = java_name_from_text(type_name);
//			out << "." << S(IDS_JAVA_GET_PART_OF_TYPE) << java_type_name << "(" << java_boolean(set_method != NULL) << ")"; // boolean added on 230101
			out << ".findOnBack(TT." << java_type_from_sprite_type((SpriteType) next->return_index()) << ", " << java_boolean(set_method != NULL) << ")"; // boolean added on 230101 - re-written 250101
//			delete [] java_type_name;
			if (next->pointer_to_next() != NULL) {
				next->pointer_to_next()->java(out,FALSE,set_new_value,set_method);
			};
		};
		return;
	};
	if (index < 0) {
      if (next == NULL || first) {
		   out << S(IDS_JAVA_TEMP) << (-index);
      } else { // does this happen?
         out << "." << S(IDS_JAVA_HOLE) << "(" << (-index) << ")";
      };
	} else if (!first) {
      if (set_method == NULL) {
         out << "." << S(IDS_JAVA_HOLE) << "(" << index << ")";
      } else if (next == NULL) {
         out << "." << set_method << "(" << index;
         if (set_new_value != NULL) out << ", " << set_new_value;
         out << ")";
      } else {
         out << "." << S(IDS_JAVA_HOLE) << "("  << index << ")";
      };
   } else if (index < LAST_EVENT_ARG_TYPE) {
		if (IDS_JAVA_MAIN_CUBBY+index == IDS_JAVA_INTEGER_STACK) { // new on 040503 until either update or rationalize dependence on language DLL here
			out << "new TTNumber(1)";
		} else {
			out << S(IDS_JAVA_MAIN_CUBBY+index);
		};
      if (index == PROGRAM_PADS) {
         tt_main_notebook_referenced = TRUE;
      };
   } else {
      out << S(IDS_JAVA_TEMP) << ((index-LAST_EVENT_ARG_TYPE)-1);
   };
   if (next != NULL) {
      next->java(out,FALSE,set_new_value,set_method);
   };
};

boolean Path::contains_special_path_marker() {
	if (index == SPECIAL_PATH_MARKER) return(TRUE);
	if (next == NULL) return(FALSE);
	return(next->contains_special_path_marker());
};

#if TT_DEBUG_ON
void Path::debug(boolean first) {
	if (index < 0) {
		tt_error_file() << -index;
	} else if (!first) {
		tt_error_file() << index;
	} else {
		tt_error_file() << S(IDS_IDS_EVENT_ARG_DEBUG+index); // event_arg_text_string[index];
	};
	if (next != NULL) {
		tt_error_file() << ".";
		next->debug(FALSE);
	};
};
#endif

void Path::describe(output_stream &text_out, SpriteType type) {
#if TT_NEW_IO
	output_string_stream path_encoding;
	dump(path_encoding);
	input_string_stream path_in(path_encoding.str());
	describe_path(START_EVENT,path_in,text_out,type);
#else
	string buffer = new character[10000];
	output_string_stream path_encoding(buffer,10000);
	dump(path_encoding);
	int length = path_encoding.pcount();
	StringInputStream path_in(buffer,length);
	describe_path(START_EVENT,&path_in,text_out,type);
	delete [] buffer;
#endif
};

boolean Path::equal_to(Path *other) {
   // update this when serial number is a GUID to just compare serial numbers
	if (return_index() != other->return_index()) return(FALSE);
	if (pointer_to_next() != NULL) {
		if (other->pointer_to_next() == NULL) return(FALSE);
		return(pointer_to_next()->equal_to(other->pointer_to_next()));
	} else {
		return(other->pointer_to_next() == NULL);
	};
};

void Path::dump(output_stream &stream, boolean warn) {
	// warn argument added on 061199 since some paths are used for other purposes - e.g. serial numbers
	// rather than use negative for first index or code
	// code is followed by index
	if (index < 0) {
		int code = -index+(int) LAST_EVENT_ARG_TYPE;
		// if a robot address its 254th hole there may be problems here... -- due to SPECIAL_PATH_MARKER
		if (warn && code > 254) { 
			//say_error(IDS_TOO_MANY_HOLES_FOR_ROBOT,TRUE);
			say(IDS_TOO_MANY_HOLES_FOR_ROBOT,3,TRUE); // rewritten on 070305 since may be user error
		};
		stream.put((unsigned char) code);
	} else {
		if (warn && index > 254) {
			//say_error(IDS_TOO_MANY_HOLES_FOR_ROBOT,TRUE);
			say(IDS_TOO_MANY_HOLES_FOR_ROBOT,3,TRUE); // rewritten on 070305 since may be user error
		};
		stream.put((unsigned char) index);
	};
	if (next != NULL) {
		next->dump(stream);
	};
};

#if TT_XML

const int event_arg_count = 15;

wide_string xml_event_arg_string[event_arg_count] =
  {L"MyBox",L"",L"MagicWand",L"Vacuum",
   L"NumberStack",L"TextPadStack",
   L"BoxStack",L"NestStack",
   L"TruckStack",L"Notebook", // was "NotebookStack"
   L"RobotStack",L"ScalesStatck",
   L"BombStack",L"InHand",
   L"BikePump"};

SpriteType event_arg_type[event_arg_count] =
{CUBBY,NONE_GIVEN,COPIER,VACUUM,INTEGER,TEXT,CUBBY,NEST,TRUCK,PROGRAM_PAD,ROBOT,
 SCALE_PICTURE, // I think so....
 BOMB,NONE_GIVEN,EXPANDER};

boolean Path::xml_path_string(wide_string path_string, int location, int max_location, boolean first) {
   if (index < 0) {
      wide_string remaining_path_string = path_string+location;
      _itow(-index,remaining_path_string,10);
      location += wide_string_length(remaining_path_string);
	} else if (!first) {
      wide_string remaining_path_string = path_string+location;
      _itow(index+1,remaining_path_string,10); // +1 for 1-indexing
      location += wide_string_length(remaining_path_string);
	} else {
		wide_string event_arg_string = xml_event_arg_string[index];
      int event_arg_string_length = wide_string_length(event_arg_string);
      wmemcpy(path_string+location,event_arg_string,event_arg_string_length);
      location += event_arg_string_length;
	};
	if (next != NULL) {
      if (location > max_location) { 
         return(FALSE); // not big enough string
      };
      path_string[location++] = '.';
		next->xml_path_string(path_string,location,max_location,FALSE);
   } else {
      path_string[location] = NULL; // terminate string
	};
   return(TRUE);
};

Path *xml_decode_event_path(xml_node *node, SpriteType &path_type) {
   // at least when path references a tool should path_type get bound
   Path *path = NULL;
   wide_string path_string = xml_get_text(node);
   int path_string_length = ::SysStringLen(path_string);
   if (path_string_length == 0) { // seems that an implicit arg (e.g. to Use) comes out empty but expects a path like the following
      return(new Path(1));
   };
   int dot_location;
   int index = path_string_length-1; // start at end and work backwards so path is built up most easily
   while (index >= 0) {
      dot_location = -1; // unless discovered otherwise below
      for (int i = index; i > 0; i--) {
         if (path_string[i] == '.') {
            dot_location = i;
            break;
         };
      };
      wide_string segment = path_string+dot_location+1;
      if (path_string[index] >= '0' && path_string[index] <= '9') { // digit
         int path_number = _wtoi(segment);
         if (dot_location == -1) { // first one
            path_number = -path_number;
         } else {
            path_number--; // convert back to zero-indexing
         };
         path = new Path(path_number,path);
      } else {
         for (int i = 0; i < event_arg_count; i++) {
            if (compare_wide_strings(segment,xml_event_arg_string[i]) == 0) {
               path = new Path(i,path);
               path_type = event_arg_type[i];
               break;
            };
         };
      };
      if (dot_location == -1) break;
      path_string[dot_location] = NULL;
      index = dot_location-1;
   };
#if TT_DEBUG_ON
	if (path == NULL) {
		log("Couldn't decode event path.");
	};
#endif
   ::SysFreeString(path_string);
   return(path);
};

#endif

unsigned long Path::path_hash(int location) { // new on 240702
   if (next == NULL) {
      return(index);
   } else {
      return(xor_at(location,index,next->path_hash((location+1)%sizeof(unsigned long))));
   };
};


Path *load_path(EventCode event_code, InputStream *pad_in, int ) {
	int event_arg = pad_in->get(); // can be modified below to be negative
	Path *rest;
//#if TT_BETA
//	int last_event_arg_type;
//	if (notebook_version < 4) {
//		last_event_arg_type = LAST_EVENT_ARG_TYPE-6;
//	} else {
//		last_event_arg_type = LAST_EVENT_ARG_TYPE;
//	};
//#else 
	const int last_event_arg_type = LAST_EVENT_ARG_TYPE;
//#endif
	if (event_arg < last_event_arg_type || event_code == START_EVENT ||
		 event_code == NEW_ITEM  || event_code == SERIAL_NUMBER) {
		// START_EVENT uses path to encode max working set size
		// NEW_ITEM uses it for index and type info
      // SERIAL_NUMBER uses it to encode the robot's serial number
//		unsigned char page_number;
//		switch ((EventArg) event_arg) {
//			case PROGRAM_PADS:
//				pad_number = (unsigned char) pad_in->get();
//				page_number = (unsigned char) pad_in->get();
//				pad_in->get(); // end of path marker
//				return(new Path(PROGRAM_PADS,new Path(page_number)));
//			default:
				rest = load_rest_of_path(event_code,pad_in);
//		};
	} else {
		event_arg -= last_event_arg_type;
		event_arg = -event_arg;
		rest = load_rest_of_path(event_code,pad_in);
	};
	return(new Path(event_arg,rest)); // rest may be NULL
};

const int max_path_depth = 256;

Path *load_rest_of_path(EventCode event_code, InputStream *pad_in) {
	// re-written to be iterative to deal with broken files
	unsigned char i = (unsigned char) pad_in->get();
	if (i == END_OF_PATH_MARKER) return(NULL);
	Path *result = new Path(i);
	Path *path_tail = result;
	i = (unsigned char) pad_in->get();
	int depth_level = 0;
	while (i != END_OF_PATH_MARKER && !pad_in->empty() && depth_level < max_path_depth) {
		Path *new_tail = new Path(i);
		path_tail->set_next(new_tail);
		path_tail = new_tail;
		i = (unsigned char) pad_in->get();
		depth_level++;
	};
	if (depth_level == max_path_depth) {
		log("Loading a path whose depth exceeds the limit");
	};
	return(result);
};

//Path *load_rest_of_path(EventCode event_code, InputStream *pad_in) {
//	unsigned char i = (unsigned char) pad_in->get();
//	if (i == END_OF_PATH_MARKER || pad_in == NULL) return(NULL);
//	//  || pad_in == NULL added on 111099 to deal better with broken files
//	return(new Path(i,load_rest_of_path(event_code,pad_in)));
//};

#if TT_DEBUG_ON
int queue_counter = 0;
#endif

Queue::Queue(Events *events, Robot *initial_robot, Queue *next) :
	events(events),
//	floor(floor), // probably can eliminate this and use floor of robot (done on 190100)
	robot(NULL),
#if TT_DEBUG_ON
   deleted(FALSE),
#endif
	next(next) {
		set_robot(initial_robot);
  if (events != NULL) events->increment_ref_count();
//  tt_error_file() << "Created queue " << this << " frame=" << tt_frame_number << endl; // debuggin
#if TT_DEBUG_ON
  debug_counter = queue_counter++;
  if (tt_debug_mode == 71100) {
	  tt_error_file() << "Creating queue#" << debug_counter << " " << endl; // space to using Emacs delete-matching-lines
	  if (tt_debug_target == debug_counter) {
		  log("Creating debug target queue");
	  };
  };
#endif
};

Queue::~Queue() {
//	tt_error_file() << "Deleted queue " << this << " frame=" << tt_frame_number << endl; // debuggin
#if TT_DEBUG_ON
   if (tt_debug_mode == 71100) {
	   tt_error_file() << "Deleting queue#" << debug_counter << " " << endl;
		if (tt_debug_target == debug_counter) {
			log("Deleting debug target queue.");
		};
   };
   if (tt_debug_mode == 13578 && events != NULL) {
      tt_error_file() << events << " decrementing ref count since queue is being destroyed." << endl;
   };
   if (deleted) {
      say_error(_T("Queue deleted twice!"));
   };
   deleted = TRUE;
#endif
	if (events != NULL && events->decrement_ref_count()) { // last one
#if TT_DEBUG_ON
      if (tt_debug_mode == 13578 && events != NULL) {
         tt_error_file() << events << " deleting since ref count is zero." << endl;
      };
		if (tt_debug_mode == 777) {
			if (events != NULL && events->decrement_ref_count()) {
				Events *remaining = events;
				while (remaining != NULL) {
					if (remaining->deleted) debug_this();
					if (remaining->decrement_ref_count()) remaining->deleted = TRUE;
					remaining = remaining->rest();
				};
			};
		} else
#endif
		delete events;
	};
   set_robot(NULL);
// following can blow the stack
//  if (next != NULL) delete next;
	Queue *remaining = next;
	while (remaining != NULL) {
		Queue *new_remaining = remaining->rest();
		remaining->set_rest(NULL);
#if TT_DEBUG_ON
		if (tt_debug_mode == 777) {
			if (remaining->deleted) debug_this();
			remaining->deleted = TRUE;
		} else
#endif
		delete remaining;
		remaining = new_remaining;
	};
};

void Queue::insert_at_end(Queue *end) {
	if (next == NULL) {
		next = end;
	} else {
		Queue *new_next = next;
		while (TRUE) {
			if (new_next->rest() == NULL) {
				new_next->set_rest(end);
				return;
			};
			new_next = new_next->rest();
		};
	};
};

// put off till next frame if more than 1/20 second
const millisecond queue_time_slice = 50;

unsigned short int events_counter_in = 0; // for replaying
unsigned short int events_counter_out = 0; // for logging

void reset_events_counters() { // new on 120603 for time travel
	events_counter_in = 0; // for replaying
	events_counter_out = 0;
};

void Queue::begin_run(QueuePointer &queue_pointer) {
#if TT_DEBUG_ON
	if (deleted) debug_this();
#endif
//	if (events == NULL) {
//		queue_pointer = run(this,0); // just updates queue_pointer and cleans up -- doesn't run any events
//      return;
//	};
	millisecond until_time = 0;
	if (replaying()) { // initialize counter
		replay_events_counter(events_counter_in);
	} else {
		events_counter_in = 0;
//		if (!tt_game_switch_on) {
			// time not used if replaying logs
			until_time = timeGetTime()+queue_time_slice;
//		};
	};
	events_counter_out = 0;
#if TT_DEBUG_ON
	millisecond before_queue;
	if (tt_debug_mode == 7023) before_queue = timeGetTime();
	unsigned short int saved_events_counter_in = events_counter_in;
#endif
// bad idea: -- note boolean value returned not used anymore
//	while (run(queue_pointer,until_time) && queue_pointer != NULL) {
		 // didn't run out of time just stuff to do
//	};
   queue_pointer = run(this,until_time);  // this may be deleted...
//	tt_running_queue = NULL; // not running any of these now
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
	//if (tt_debug_mode == 250405 && tt_frame_number == 725239) {
	//	Sprites *ignore = new Sprites(); // test on 250405
	//};
   if (replaying() && (tt_debug_mode == 101 || tt_debug_mode == 1717)) {
      tt_error_file() << saved_events_counter_in << " event counter; frame="
                  << tt_frame_number << endl;
   };
	if (tt_debug_mode == 7023) {
		if (replaying()) {
//			if (saved_events_counter_in > 0) {
				tt_error_file() << saved_events_counter_in << " events in "
								<< (timeGetTime()-before_queue) 
								<< "ms; frame = " << tt_frame_number << endl;
//			};
		} else {
//			if (events_counter_out > 0) {
				tt_error_file() << events_counter_out << " events in " 
								<< (timeGetTime()-before_queue)
								<< "ms; frame = " << tt_frame_number << endl;
//			};
		};
	};
#endif
	if (tt_logging) { // && events_counter_out > 0) {
		log_events_counter(events_counter_out);
#if TT_DEBUG_ON
		if (tt_debug_mode == 101 || tt_debug_mode == 1717) {
			tt_error_file() << events_counter_out << " event counter; frame="
								 << tt_frame_number << endl;
	   };
#endif
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 10904) return;
#endif
   if (replaying() && events_counter_in > 0) {
//    tt_exit_failure(TROUBLE_SHOOT_BAD_DEMO,S(IDS_LOG_REPLAY_ERROR));
		log(S(IDS_LOG_REPLAY_ERROR),FALSE,!tt_run_demos_despite_anomalies); // last arg was TRUE prior to 311204
		tt_error_file() << "There are " << events_counter_in 
							 << " events that happened while recording that didn't happen when replaying during segment number " 
							 // space added at end above on 110305
						    << tt_current_log_segment << endl; // new on 051004 -- updated 291204
		if (tt_time_travel != TIME_TRAVEL_OFF && !tt_run_demos_despite_anomalies) { // new on 180703
#if TT_DEBUG_ON
 			debug_this();
#endif
 			throw(EVENTS_COUNTER_TOO_LARGE);
		};
//#if TT_DEBUG_ON
//		if (tt_debug_mode != 211104)
//#endif
		if (!tt_run_demos_despite_anomalies) {
			close_input_log(FALSE,FALSE); // second arg was TRUE prior to 210300 and prior to 180703 was (tt_time_travel != TIME_TRAVEL_OFF)
		};
   };
};

Queue *run(Queue *this_queue, millisecond until_time) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 201199) {
		tt_error_file() << "Starting to run queue of " << this_queue->length() 
							 << " events on frame " << tt_frame_number << " long at " << timeGetTime() << endl;
	};
#endif
   Queue *remaining = this_queue;
//  	tt_running_queue = this_queue;
	// renamed remaining to tt_running_queue on 090400 so when robots are destroyed they can remove themselves from this
	// undid this since doesn't work recursively
  while (remaining != NULL) {
	Events *events = remaining->pointer_to_events();
	Robot *robot = remaining->pointer_to_robot();
	Queue *next = remaining->rest();
	// could avoid the following if robots remove queue element when deleted or vacuumed - but they do now, right??
	if (events == NULL || robot->inside_vacuum()) {
#if TT_DEBUG_ON
		if ((tt_debug_mode == 101 || tt_debug_mode == 1717) && events == NULL) {
			// if robot was deleted it removes the events by setting them to NULL
         tt_error_file() << "Skipping over queue entry with NULL events." << endl;
      };
#endif
		remaining->set_rest(NULL);
		delete remaining;
      remaining = next;
      while (remaining != NULL) { // skip over this one -- new policy on 12/31/97
         events = remaining->pointer_to_events();
	      robot = remaining->pointer_to_robot();
	      next = remaining->rest();
         if (events != NULL) { // keep skipping over ones with NULL events
            break;
         } else {
				remaining->set_rest(NULL); // added 071100 to patch a leak
				delete remaining;
            remaining = next;
#if TT_DEBUG_ON
		      if (tt_debug_mode == 101 || tt_debug_mode == 1717) {
               tt_error_file() << "Skipping over next queue entry with NULL events." << endl;
            };
#endif
         };
      };
      if (remaining == NULL) return(NULL);
	};
	if (tt_logging) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 230804) { 
			char robot_name[1000];
			robot->name(robot_name);
			tt_error_file() << "On frame " << tt_frame_number << " robot " << robot_name << " incremented events_counter_out." << endl;
		};
#endif
		events_counter_out++;
	};
	Background *floor = remaining->pointer_to_robot()->pointer_to_background();
	events->decrement_ref_count();
	events = events->run(floor,robot);
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	if (events != NULL) {
      events->increment_ref_count();
#if TT_DEBUG_ON
   } else if (tt_debug_mode == 101 || tt_debug_mode == 1717) {
      log(_T("Setting next queue entry to NULL events."));
#endif
   };
	remaining->set_events(events);
	boolean slice_over;
	millisecond now;
	if (replaying()) {
		slice_over = (events_counter_in <= 1);
		events_counter_in--;
	} else {
		now = timeGetTime();
		slice_over = (until_time < now || tt_city->stopped() || robot->inside_vacuum() || events_counter_out == max_unsigned_short);
		// events_counter_out == max_unsigned_short is new on 280204
	};
	if (next != NULL) {
		if (events == NULL) {
			remaining->set_rest(NULL);
			delete remaining;
			if (slice_over) {
				combine_queues(next,tt_queue);
				tt_queue = next;
#if TT_DEBUG_ON
				if (tt_debug_mode == 201199 && until_time < now) {
					tt_error_file() << "Time slice over. There are " << tt_queue->length() 
										 << " events remaining on frame " << tt_frame_number << " long at " << now << endl;
				};
#endif
				return(NULL); 
			} else {
				remaining = next;
			};
		} else {
			if (slice_over) {
				// put next in the front of the queue for next time
				// unless first of tt_queue is on screen
				combine_queues(next,tt_queue);
				remaining->set_rest(NULL); // so this is terminated since next already put in tt_queue
				combine_queues(next,remaining); // put remaining events at end
				tt_queue = next;
#if TT_DEBUG_ON
				if (tt_debug_mode == 201199 && until_time < now) {
					tt_error_file() << "Time slice over. There are " << tt_queue->length() 
										 << " events remaining on frame " << tt_frame_number << " long at " << now << endl;
				};
#endif
// following caused major grief in a hard to track down bug
// due to sharing of remaining with the newly computed tt_queue
//				remaining->set_rest(NULL); // so this is terminated since next already put in tt_queue
				return(NULL);
			} else { // the only recursion here -- hopefully not deep
				remaining->set_rest(run(next,until_time));
				return(remaining);
			};
		};
	} else {  // nothing more in this queue
		Queue *new_remaining = tt_queue;
		Queue *rest = NULL;
		if (events == NULL) {
			delete remaining;
//			queue_pointer = NULL;
		} else {
			if (new_remaining != NULL && !slice_over) {
				combine_queues(new_remaining,remaining);
				rest = new_remaining; 
			} else {
				rest = remaining;
			};
		};
		if (new_remaining != NULL && !slice_over) {
			tt_queue = NULL; // this is cool since new_remaining started off being tt_queue
			remaining = new_remaining;
//			remaining->run(queue_pointer,until_time);
		} else {
			return(rest);
		};
	};
  };
  return(NULL);
};

void Queue::first_and_rest(QueuePointer &first, QueuePointer &rest) {
	rest = next;
	next = NULL; // so first is "by itself"
	first = this; 
};

void Queue::insert_second(Queue *new_second) {
	new_second->set_rest(next); // give it my rest
	next = new_second;
};

void Queue::move_on_screen_to_front() {
	if (next == NULL) return;
	if (robot != NULL && robot->showing_on_a_screen()) return; // already is
	Queue *previous_remaining = this;
	Queue *remaining = next;
	while (remaining != NULL) {
		if (remaining->first_is_on_a_screen()) { // splice this guy out
			Queue *on_screen;
			Queue *new_remaining;
			remaining->first_and_rest(on_screen,new_remaining);
			previous_remaining->set_rest(new_remaining);
			insert_second(on_screen);
			return;
		};
		previous_remaining = remaining;
		remaining = remaining->rest();
	};
};

boolean Queue::first_is_on_a_screen() {
	return(robot != NULL && robot->showing_on_a_screen());
};

void Queue::remove_items_of_robot(Robot *a_robot) {
	Queue *remaining = this;
	while (remaining != NULL) {
		if (remaining->pointer_to_robot() == a_robot) {
			// splice this guy out
			remaining->remove_events();
         remaining->set_robot(NULL);
		};
		remaining = remaining->rest();
	};
};

boolean Queue::robot_already_in_queue(Robot *a_robot) { // new with 3.18
	Queue *remaining = this;
	while (remaining != NULL) {
		if (remaining->pointer_to_robot() == a_robot) {
			return(TRUE);
		};
		remaining = remaining->rest();
	};
	return(FALSE);
};

Robot *Queue::robot_in_queue_with_cubby(Cubby *cubby) { // new on 120205
	Queue *remaining = this;
	while (remaining != NULL) {
		Robot *robot = remaining->pointer_to_robot();
		if (robot != NULL && robot->pointer_to_working_cubby() == cubby) {
			return(robot);
		};
		remaining = remaining->rest();
	};
	return(NULL);
};

void Queue::add_robots_to_flipped_followers(Picture *picture) {
	// this is for new robots that have been created by robots on the back of pictures
	// does this work correctly for teams of robots??
   Background *picture_backside = picture->pointer_to_backside();
	if (picture_backside == NULL) {
		return; // added 231000 since if robot is in "limbo" then it shouldn't add to picture
	};
	Queue *remaining = this;
   while (remaining != NULL) {
      Robot *robot = remaining->pointer_to_robot();
      if (robot != NULL && robot->pointer_to_background() == picture_backside &&
			 (robot->pointer_to_leader() == NULL || (robot->pointer_to_leader() == picture && tt_log_version_number >=22))) { 
			// conjunct added 6/17/98 since otherwise robot can end up part of picture and flipped sub picture
			// disjunct added on 070400 since OK if picture is both leader and background of robot
         picture->add_robot_as_flipped_follower(robot);
      };
      remaining = remaining->rest();
	};
};

void Queue::remove_events() {
	if (events != NULL) {
// shared body of the robot
//		delete events;
      events->decrement_ref_count();
		events = NULL;
#if TT_BUG_FIXES
		set_robot(NULL); // new on 301103 to clean up when team is stopped
#endif
#if TT_DEBUG_ON
      if (tt_debug_mode == 101 || tt_debug_mode == 1717) {
         tt_error_file() << "Remove events. Setting next queue entry to NULL events; f=" << tt_frame_number << endl;
      };
#endif
	};
};

#if TT_DEBUG_ON
int Queue::length() {
	if (next == NULL) return(1);
	return(1+next->length());
};
#endif

void Queue::set_robot(Robot *initial_robot) {
	// tried to comment out this ref counting but caused serious bugs - is there a risk of cycles here??
	if (robot != NULL) {
		if (tt_resetting_or_destroying_city) { // was tt_shutting_down prior to 120603
			robot->decrement_ref_count(); // destroy() can cause stack overflow when shutting down
		} else {
			robot->destroy(); // new on 261000 since otherwise can be a bad leak 
		};
	};
	robot = initial_robot;
	if (robot != NULL) {
		robot->increment_ref_count();
#if TT_DEBUG_ON
		if (robot->debug_counter == tt_debug_target) {
			log("debug target robot is added to queue.");
		};
#endif
	};
};

#if TT_XML

xml_element *Queue::xml(xml_document *document) { // new on 011102
   xml_element *queue_element = create_xml_element(L"Queue",document);
	Queue *remaining = this;
   xml_element *actions_element = NULL;
	while (remaining != NULL) {
		Robot *robot = remaining->pointer_to_robot();
		if (robot != NULL && robot->ultimate_container()->pointer_to_background() != NULL) { // added robot != NULL &&  for robustness on 311003
         // not in limbo (new on 141100) - is limbo ever OK - e.g. while carried by a bird??
			robot->xml_create_and_append_element(queue_element,document,TRUE)->Release();
			Events *e = remaining->pointer_to_events();
         actions_element = create_xml_element(L"Actions",document);
         // even if e is NULL (though it should be) so update queue appropriately
			if (e != NULL) { // can it ever be NULL?
				e->xml(actions_element,document);
			};
			if (remaining->rest() == NULL) { // "moved here" on 310804 so isn't already released
				xml_set_attribute(actions_element,L"Last",1);
		   };
			xml_append_child(actions_element,queue_element);  // moved here on 190804
		};
		remaining = remaining->rest();
	};
  // if (actions_element != NULL) { // this had been commented out - restored on 271102
  //    xml_set_attribute(actions_element,L"Last",1);
		////if (type_of_thing_held != NONE_GIVEN) { // new on 171202
		////	xml_set_attribute(actions_element,L"TypeOfThingHeldCode",(int) type_of_thing_held);
		////};
		////if (type_of_attached_to_wand != NONE_GIVEN) { // new on 171202
		////	xml_set_attribute(actions_element,L"TypeOfThingAttachedToWandCode",(int) type_of_attached_to_wand);
		////};
  // };
   return(queue_element);
};

Queue *current_queue = NULL;

void reset_current_queue() { // new on 130405 in case aborted loading of a city
	current_queue = NULL; 
};

boolean Queue::handle_xml_tag(Tag tag, xml_node *node) {
   if (current_queue == NULL) {
      current_queue = this;
   };
   switch (tag) {
		case ROBOT_TAG: 
//      case LINK_TAG: 
         current_queue->set_robot((Robot *) xml_load_sprite(node,tag,NULL,ROBOT));
			return(TRUE);
      case ACTIONS_TAG:
         current_queue->set_events(xml_load_events(node,FALSE,robot));
			//current_queue->set_type_of_thing_held(xml_get_attribute_int(node,L"TypeOfThingHeldCode",NONE_GIVEN));
			//current_queue->set_type_of_attached_to_wand(xml_get_attribute_int(node,L"TypeOfThingAttachedToWandCode",NONE_GIVEN));
         if (!xml_get_attribute_int(node,L"Last",0)) {
            // if not the last one then create next element to fill in
            Queue *queue = new Queue(NULL,NULL);
            current_queue->set_rest(queue);
            current_queue = queue;
         } else {
            current_queue = NULL;
         };
         return(TRUE);
      case NO_MORE_TAGS:
         current_queue = NULL;
         return(TRUE);
      default:
         return(Entity::handle_xml_tag(tag,node));
   };
};

#endif

void Queue::dump(output_stream &stream) {
	// new on 190100
	Queue *remaining = this;
	while (remaining != NULL) {
		Robot *robot = remaining->pointer_to_robot();
		if (robot->ultimate_container()->pointer_to_background() != NULL) { // not in limbo (new on 141100) - is limbo ever OK - e.g. while carried by a bird??
			stream.put(QUEUE_MARKER);
			robot->dump(stream); // since ref count should be greater than 2 is shared
			Events *e = remaining->pointer_to_events();
			if (e == NULL) {
				stream.put(NOTHING_MARKER);
			} else {
				e->dump(stream);
			};
		};
		remaining = remaining->rest();
	};
	stream.put(NOTHING_MARKER);
};

Queue *load_queue(InputStream *stream, int version_number, NaturalLanguage language) {
	Queue *previous_queue = NULL;
	Queue *first_queue = NULL;
	while (stream->get() == QUEUE_MARKER) {
		Robot *robot = (Robot *) load_item(stream,version_number,language);
		Events *events = load_events(stream,version_number);
		Queue *queue = new Queue(events,robot);
		if (previous_queue != NULL) {
			previous_queue->set_rest(queue);
		};
		if (first_queue == NULL) first_queue = queue;
		previous_queue = queue;
	};
	return(first_queue);
};

void combine_queues(Queue *first, Queue *next) {
	// clobbers first to have second following it
	// but if first element of second is on_screen make it
	// be second of first (not first or risk not getting any offscreen work done)
	if (next == NULL) return;
	if (next->first_is_on_a_screen()) {
		Queue *next_first;
		Queue *next_rest;
		next->first_and_rest(next_first,next_rest);
		first->insert_second(next_first);
		if (next_rest != NULL) first->insert_at_end(next_rest);
	} else {
		first->insert_at_end(next);
	};
};

Suspensions::Suspensions(//Events *events, Background *floor,	
								 Robot *robot, SpritePointerPointer cell) :
//		events(events),
//		floor(floor),
		robot(robot),
		cell(cell),
		next(NULL),
		previous(NULL),
#if TT_DEBUG_ON
		deleted(FALSE),
#endif
		first_in_list(FALSE) {
//   robot->increment_ref_count(NULL); // don't let it get deleted until this suspension is
	// commented out ref count stuff on 090400 since causes robots to not be deleted and to run (as orphans)
	// and when robot is prepared for deletion it removes itself from tt_queue
};

Suspensions::~Suspensions() {
//   if (robot != NULL) robot->destroy(); // if zero ref count
	// commented out ref count stuff on 090400 since causes robots to not be deleted and to run (as orphans)
	// and when robot is prepared for deletion it removes itself from tt_queue
};

void Suspensions::delete_all() {
   if (next != NULL) {
	   next->delete_all();
   };
   delete this;
};

void Suspensions::add(Suspensions *new_item) {
#if TT_DEBUG_ON
	if (deleted) {
//		tt_error_file() << "Suspension already deleted!" << endl;
//		tt_exit_failure();
		say_error(_T("Sorry. ToonTalk is confused and tried to add something after destroying it."));
		return;
	};
#endif
  if (next != NULL) {
	  next->set_previous(new_item);
  };
  new_item->set_previous(this);
  new_item->set_next(next);
  next = new_item;
};

void Suspensions::remove() {
#if TT_DEBUG_ON
	if (deleted) {
//		tt_error_file() << "Suspension already deleted!" << endl;
//	   tt_exit_failure();
		say_error(_T("Sorry. ToonTalk is confused and tried to remove something after destroying it."));
		return;
	};
#endif
	previous->set_next(next);
	if (next != NULL) next->set_previous(previous);
#if TT_DEBUG_ON
	if (tt_debug_mode == 66) {
		deleted = TRUE;
	} else { 
		delete this;
	};
#else
	delete this;
#endif
};

void Suspensions::retry(Sprite **cell_receiving_item) {
#if TT_DEBUG_ON
	if (deleted) {
//		tt_error_file() << "Suspension already deleted!" << endl;
//		tt_exit_failure();
		say_error(_T("Nuts. ToonTalk is confused and tried to use something after destroying it."));
		return;
	};
#endif
	if (next != NULL) {
		next->retry(cell_receiving_item);
	};
	if (cell_receiving_item == cell) {
		Cubby *cubby = robot->pointer_to_working_cubby();
//		cubby->decrement_busy_counter(); // unlock it
		robot->try_clause(cubby); // this will remove this as well
//		remove();
	};
};

void Suspensions::remove_if_robot_is(Robot *target_robot) {
#if TT_DEBUG_ON
	if (deleted) {
//		tt_error_file() << "Suspension already deleted!" << endl;
//		tt_exit_failure();
		say_error(_T("ToonTalk is confused and tried to remove something after destroying it."));
		return;
	};
#endif
	if (next != NULL) {
      next->remove_if_robot_is(target_robot);
   };
	if (robot == target_robot) {
		remove();
	};
	// rewrote the above as follows on 090400 since it avoids the stack 
//	Suspensions *suspensions = this;
//	while (suspensions != NULL) {
//		if (suspensions->pointer_to_robot() == target_robot) {
//			suspensions->remove();
//		};
//		suspensions = suspensions->pointer_to_next();
//	};
};

boolean Suspensions::is_robot_waiting_for_programmer(Robot *target_robot) { // new on 120104
	if (robot == target_robot) return(TRUE);
	if (next != NULL) {
		return(next->is_robot_waiting_for_programmer(target_robot));
	};
	return(FALSE);
};

boolean Suspensions::remove_and_stop_if_different_robot_waiting_on_same_cell(Robot *new_robot, SpritePointerPointer new_cell) {
	// new on 120205 to deal with box taken away and given to someone else
	// returns FALSE only if is already there with same robot and cell
	if (cell == new_cell) {
		if (new_robot != robot) {
			robot->stop_all(TRUE);
			return(TRUE); // stopped another
		};
		return(FALSE); // already knew about it
	};
	if (next == NULL) return(TRUE);
	return(next->remove_and_stop_if_different_robot_waiting_on_same_cell(new_robot,new_cell));
};

//#if TT_XML
//// this was missing prior to 120104 so if a city was saved with a robot waiting for the programmer to put something in a box
//// it wouldn't work
//
//xml_element *queue_element = create_xml_element(L"Queue",document);
//	Queue *remaining = this;
//   xml_element *actions_element = NULL;
//	while (remaining != NULL) {
//		Robot *robot = remaining->pointer_to_robot();
//		if (robot != NULL && robot->ultimate_container()->pointer_to_background() != NULL) { // added robot != NULL &&  for robustness on 311003
//         // not in limbo (new on 141100) - is limbo ever OK - e.g. while carried by a bird??
//			robot->xml_create_and_append_element(queue_element,document,TRUE);
//			Events *e = remaining->pointer_to_events();
//         actions_element = create_xml_element(L"Actions",document);
//         xml_append_child(actions_element,queue_element); // even if e is NULL (though it should be) so update queue appropriately
//			if (e != NULL) { // can it ever be NULL?
//				e->xml(actions_element,document);
//			};
//		};
//		remaining = remaining->rest();
//	};
//   if (actions_element != NULL) { // this had been commented out - restored on 271102
//      xml_set_attribute(actions_element,L"Last",1);
//		//if (type_of_thing_held != NONE_GIVEN) { // new on 171202
//		//	xml_set_attribute(actions_element,L"TypeOfThingHeldCode",(int) type_of_thing_held);
//		//};
//		//if (type_of_attached_to_wand != NONE_GIVEN) { // new on 171202
//		//	xml_set_attribute(actions_element,L"TypeOfThingAttachedToWandCode",(int) type_of_attached_to_wand);
//		//};
//   };
//   return(queue_element);
//};
//
//Queue *current_queue = NULL;
//
//boolean Queue::handle_xml_tag(Tag tag, xml_node *node) {
//   if (current_queue == NULL) {
//      current_queue = this;
//   };
//   switch (tag) {
//		case ROBOT_TAG: 
////      case LINK_TAG: 
//         current_queue->set_robot((Robot *) xml_load_sprite(node,tag,NULL,ROBOT));
//			return(TRUE);
//      case ACTIONS_TAG:
//         current_queue->set_events(xml_load_events(node,FALSE,robot));
//			//current_queue->set_type_of_thing_held(xml_get_attribute_int(node,L"TypeOfThingHeldCode",NONE_GIVEN));
//			//current_queue->set_type_of_attached_to_wand(xml_get_attribute_int(node,L"TypeOfThingAttachedToWandCode",NONE_GIVEN));
//         if (!xml_get_attribute_int(node,L"Last",0)) {
//            // if not the last one then create next element to fill in
//            Queue *queue = new Queue(NULL,NULL);
//            current_queue->set_rest(queue);
//            current_queue = queue;
//         } else {
//            current_queue = NULL;
//         };
//         return(TRUE);
//      case NO_MORE_TAGS:
//         current_queue = NULL;
//         return(TRUE);
//      default:
//         return(Entity::handle_xml_tag(tag,node));
//   };
//};
//

//xml_element *Suspensions::xml(xml_document *document) {
//	xml_element *queue_element = create_xml_element(L"Suspensions",document);
//	Queue *remaining = this;	
//};
//boolean Suspensions::handle_xml_tag(Tag tag, xml_node *node) {
//	
//};
//#endif

