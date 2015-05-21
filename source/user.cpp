// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)
#include "defs.h"
#endif   
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif   
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif   
#if !defined(__TT_LOG_H)   
#include "log.h"
#endif   
#if !defined(__TT_USER_H)   
#include "user.h"
#endif
#if !defined(__TT_PRGRMMR_H)
#include "prgrmmr.h"
#endif
#if !defined(__TT_MARTIAN_H)
#include "martian.h"
#endif    
#if !defined(__TT_CLEANUP_H)   
#include "cleanup.h"
#endif   
#if !defined(__TT_HELP_H)
#include "help.h"
#endif
#if !defined(__TT_MAIN_H)
#include "main.h"
#endif  
#if !defined(__TT_SCREEN_H)
#include "screen.h"
#endif

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Input_Events *input_events = NULL;
Input_Events *next_frame_input_events = NULL;
Input_Events *saved_input_events = NULL; // new on 290803
Input_Events *saved_next_frame_input_events = NULL;

//boolean first_time = TRUE;

city_coordinate last_mouse_delta_x = 0;
city_coordinate last_mouse_delta_y = 0;

void save_input_events() { // new for events to be preserved between segments when time travelling
	saved_input_events = input_events;
	saved_next_frame_input_events = next_frame_input_events;
	input_events = NULL;
	next_frame_input_events = NULL;
};

void restore_input_events() {
	input_events = saved_input_events;
	next_frame_input_events = saved_next_frame_input_events;
	saved_input_events = NULL; 
	saved_next_frame_input_events = NULL;
};

boolean log_or_user_actions(boolean &event,
									 unsigned char &key,
									 boolean &extended,
									 city_coordinate &delta_x,
									 city_coordinate &delta_y,
									 char &mouse_button_status,
                            char &joystick_button_status,
									 millisecond &duration,
									 unsigned char &second_byte) {
	boolean more;
	if (replaying()) { //  && tt_titles_ended_on_frame <= tt_frame_number) { //  && tt_titles_ended_on_frame new on 230304
		// added last_mouse_delta_x,last_mouse_delta_y on 051100
		more = replay_user(event,key,extended,delta_x,delta_y,last_mouse_delta_x,last_mouse_delta_y,
								 mouse_button_status,joystick_button_status,duration,second_byte);
#if TT_DEBUG_ON
		if (tt_debug_mode == 101) {
			tt_error_file() << (int) event << " " << (int) key << " " << (int) extended << " "
								 << delta_x << " " << delta_y << " " << (int) mouse_button_status << " " << (int) joystick_button_status << " "
								 << duration << " " << (int) more << " " << tt_frame_number << " "
								 << tt_current_time << endl;
		};
#endif
		// commented out the following on 051100 since handled in log file now (and this was mouse and arrow key deltas combined)
//    last_mouse_delta_x = delta_x;
//	   last_mouse_delta_y = delta_y;
//		set_last_mouse_deltas(delta_x,delta_y); // wrong units
//		return(more);
	} else {
		user_actions(event,key,extended,delta_x,delta_y,mouse_button_status,joystick_button_status,duration,second_byte);
		more = (input_events != NULL);
		if (key == 0) { // new on 030900 -- tried moving this so it applied to demos but that made the demos hang
			key = tt_programmer->simulated_key(); 
			if (key != 0) {
				event = TRUE;
			};
		};
#if TT_DEBUG_ON
		if (tt_debug_mode == 101) {
			tt_error_file() << (int) event << " " << (int) key << " " << (int) extended << " "
								 << delta_x << " "	<< delta_y << " " << (int) mouse_button_status << " " << (int) joystick_button_status << " "
								 << duration << " " << (int) more << " " << tt_frame_number << " "
								 << tt_current_time << endl;
		};
#endif
	};
	if (tt_logging) { // used to be event && ... so only eventful events logged
		// added last_mouse_delta_x,last_mouse_delta_y on 051100
		tt_record_clock = (tt_frame_number >= tt_titles_ended_on_frame || event); // rewritten on 300104 to avoid recording extra stuff during titles
		if (tt_record_clock) {
			log_user(key,extended,event,delta_x,delta_y,last_mouse_delta_x,last_mouse_delta_y,
						mouse_button_status,joystick_button_status,duration,more,second_byte);
		};
//       tt_error_file() << event << " " << (int) key << " " << delta_x << " "
//                << delta_y << " "  << mouse_button_status << " "
//                << iduration << " " << more << endl;
	};
	if (!more) {
      if (input_events != NULL) delete input_events;
		input_events = next_frame_input_events;
		next_frame_input_events = NULL;
	};
	return(more);
};

/* by commenting out this from above replaying logs might run too fast but
   will reproduce behavior accurately
	  if (tt_current_time < next_log_time) { // not there yet
//      tt_error_file() << "Time is " << tt_current_time << " waiting for " << next_log_time << endl;
        event = FALSE;
        delta_x = 0;
        delta_y = 0;
        button1 = FALSE;
		  button2 = FALSE;
        return(0);
     } else {
*/
//      event = TRUE;
		  // reset the following so it replays the log with same
        // timing parameters as the original had
//      tt_error_file() << "Time is " << tt_current_time << " reseting it to " << next_log_time << endl;
//      tt_millisecond_delta = log_millisecond_delta;
//      tt_current_time = next_log_time;
//      tt_delta = tt_millisecond_delta*1.0E-3;
//      next_log_time = 0; // look it up next time around


Input_Event::Input_Event(millisecond start_time, millisecond end_time) :
   start_time(start_time),
   end_time(end_time),
   processed(FALSE) {
};

Button_Event::Button_Event(InputDeviceType device_type, char button_status,
									millisecond start_time, millisecond end_time) :
   Input_Event(start_time,end_time),
   device_type(device_type),
   button_status(button_status) {
};

//void Button_Event::check_if_over() {
//	switch (device_type) {
//		case MOUSE_DEVICE:
//		  if (tt_mouse_button == 0) {
//			  end_time = tt_mouse_button_up_time;
//		  };
//		  break;
//      case JOYSTICK_DEVICE:
//        // to be done
//        break;
//   };
//};

boolean Button_Event::same_button_event(InputDeviceType new_device_type, char new_button_status) {
//tt_error_file() << "New " << new_device_type << "," << new_button_status
//            << " Old " << device_type << "," << mouse_button_status << endl;
   return(new_button_status == button_status &&
			 new_device_type == device_type);
};

boolean Button_Event::interpret(boolean &event_flag,
										  unsigned char &key,
										  boolean &extended_key,
										  city_coordinate &delta_x,
										  city_coordinate &delta_y,
										  char &the_mouse_button_status,
                                char &the_joystick_button_status,
										  long &duration,
										  unsigned char &second_byte) {
  boolean saved = FALSE; // by default use up this event
  event_flag = TRUE;
  key = 0;
  extended_key = FALSE;
  second_byte = 0;
  delta_x = 0;
  delta_y = 0;
  if (end_time == 0) { // button has not been released so keep this around
	  next_frame_input_events = new Input_Events(this,next_frame_input_events);
	  saved = TRUE;
	  duration = tt_millisecond_delta;
  } else {
	  duration = end_time-start_time;
  };
#if TT_DEBUG_ON
//  if (duration > 3000L) {
//	  tt_error_file() << duration << "is a long time.  Frame number is "
//					  << tt_frame_number << endl;
//  };
  if (duration < 0) {
	  tt_error_file() << "Button duration is negative " << duration << endl;
	  say_error(_T("Warning: somehow ToonTalk thought a button was down for a negative amount of time. "));
  };
#endif
  if (processed) {
	  // button is still down so ignore this for now
	  // could have auto-repeat
	  event_flag = FALSE;
	  the_mouse_button_status = 0;
     the_joystick_button_status = 0;
  } else {
	  processed = TRUE;
	  // if button is still down return negative button status
//	  the_button_status = (end_time==0)?-mouse_button_status:mouse_button_status;
	  switch (device_type) {
        case MOUSE_DEVICE:
           the_mouse_button_status = button_status;
           the_joystick_button_status = 0; // does this exclude simultaneous use?
           break;
        case JOYSTICK_DEVICE:
           the_joystick_button_status = button_status;
           the_mouse_button_status = 0;
           break;
     };
  };
//  tt_error_file() << (int) the_button_status << " " << duration << " " << (int) event_flag << endl;
  return(saved);
};

Move_Event::Move_Event(city_coordinate the_delta_x,
							  city_coordinate the_delta_y,
							  millisecond start_time,
							  millisecond end_time) :
  Input_Event(start_time, end_time),
  delta_x(the_delta_x),
  delta_y(the_delta_y) {
};

boolean Move_Event::interpret(boolean &event_flag,
										unsigned char &key,
										boolean &extended_key,
										city_coordinate &the_delta_x,
										city_coordinate &the_delta_y,
										char &mouse_button_status,
                              char &joystick_button_status,
                              long &duration,
										unsigned char &second_byte) {
  event_flag = TRUE;
  key = 0;
  extended_key = FALSE;
  second_byte = 0;
  the_delta_x = delta_x;
  the_delta_y = delta_y;
  mouse_button_status = 0;
  joystick_button_status = 0;
  duration = tt_millisecond_delta; // since movement since last frame
  return(FALSE); // ok to delete this guy now
};

void record_mouse_movement(int mouse_x, int mouse_y) {
	record_mouse_delta(mouse_x-tt_screen_half_width,
							 tt_screen_half_height-mouse_y); // up is less
};

void last_mouse_deltas(city_coordinate &delta_x, city_coordinate &delta_y) {
	delta_x = last_mouse_delta_x;
	delta_y = last_mouse_delta_y;
#if TT_DEBUG_ON
	if (tt_debug_mode == 271000) {
		tt_error_file() << "Mouse deltas read  " << last_mouse_delta_x << "," << last_mouse_delta_y << " f=" << tt_frame_number << endl;
	};
#endif
};

void set_last_mouse_deltas(city_coordinate delta_x, city_coordinate delta_y) {
   // args are really coordinates (i.e. pixels)
//   if (tt_absolute_mouse_mode == RELATIVE_MOUSE_MODE) {
	   last_mouse_delta_x = ideal_horizontal_units(delta_x); // delta_x*tt_ideal_units_per_horizontal_pixel;
	   last_mouse_delta_y = ideal_vertical_units(delta_y); // delta_y*tt_ideal_units_per_vertical_pixel;
//   } else { // new on 220402 - need absolute screen position
//      last_mouse_delta_x = tt_screen->x_pixel_to_screen(delta_x);
//      last_mouse_delta_y = tt_screen->y_pixel_to_screen(delta_y);
//   };
#if TT_DEBUG_ON
	if ((tt_debug_mode == 271000 || tt_debug_mode == 30502) && (last_mouse_delta_x != 0 || last_mouse_delta_y != 0)) {
		tt_error_file() << "Mouse deltas set to " << last_mouse_delta_x << "," << last_mouse_delta_y << " f=" << tt_frame_number << endl;
	};
#endif
//#if TT_ABSOLUTE_MOUSE
// new on 180402 -- removed 220402
//   if (tt_absolute_mouse_mode == ABSOLUTE_MOUSE_MODE && last_mouse_delta_x == 0 && last_mouse_delta_y == 0) { 
      // since 0 0 is intrepreted as no movement set to very very near the lower left corner
//      last_mouse_delta_x = 1;
//   };
//#endif
};

void record_mouse_delta(coordinate delta_mouse_x, coordinate delta_mouse_y) {
	int units_per_horizontal_pixel,units_per_vertical_pixel;
	if (tt_screen_width <= tt_graphics_video_mode_width) { // new on 170999 -- was > prior to 271000 which messed up mouse movement recording at resolutions higher than 640
		if (tt_graphics_video_mode_width == 320) {
			units_per_horizontal_pixel = 100;
		} else {
			units_per_horizontal_pixel = 50; // too sluggish otherwise
		};
	} else {
		units_per_horizontal_pixel = tt_ideal_units_per_horizontal_pixel;
	};
	if (tt_screen_height <= tt_graphics_video_mode_height) { // was > prior to 271000
		if (tt_graphics_video_mode_height == 240) {
			units_per_vertical_pixel = 100;
		} else {
			units_per_vertical_pixel = 50;
		};
	} else {
		units_per_vertical_pixel = tt_ideal_units_per_vertical_pixel;
	};
	city_coordinate mouse_delta_x = ((city_coordinate) delta_mouse_x)*units_per_horizontal_pixel;
	city_coordinate mouse_delta_y = ((city_coordinate) delta_mouse_y)*units_per_vertical_pixel;
	if (//!tt_game_switch_on && // ignore mouse if running game
		 (mouse_delta_x != 0 || mouse_delta_y != 0)) {
		// not clear if order matters and where is should be
		input_events = new Input_Events(new Move_Event(mouse_delta_x,mouse_delta_y),input_events);
#if TT_DEBUG_ON
		if (tt_debug_mode == 271000) {
			tt_error_file() << "Mouse deltas record " << mouse_delta_x << "," << mouse_delta_y << " f=" << tt_frame_number << endl;
		};
#endif
	};
};

/*
void react_to_mouse(boolean &mouse_event,
						  city_coordinate &delta_x, city_coordinate &delta_y,
						  int &button1, int &button2) {


	int count,ignore_x,ignore_y;
   // following handles quick clicks which are
   // still missed if using a joy stick
   fg_mousebut(1,&count,&ignore_x,&ignore_y);
   if (!button1) { // button1 isn't down now but maybe it was since last check
      button1 = (count > 0);
   };
   fg_mousebut(2,&count,&ignore_x,&ignore_y);
   if (!button2) {
      button2 = (count > 0);
	};
   fg_mousebut(3,&count,&ignore_x,&ignore_y);
   if (count > 0) {
      button1 = TRUE;
		button2 = TRUE;
   };
   delta_x = (cursor_x-tt_screen_half_width)
              *tt_ideal_units_per_horizontal_pixel;
   delta_y = (tt_screen_half_height-cursor_y) // up is less
              *tt_ideal_units_per_vertical_pixel;
   fg_mousemov(tt_screen_half_width,
               tt_screen_half_height);  // restore mouse to center
   mouse_event = ((delta_x != 0) || (delta_y != 0) ||
						(button1 != 0) || (button2 != 0));
};
*/

Key_Event::Key_Event(//int the_scan_code,
							char character,
							boolean extended_key, // e.g. F1
							unsigned char second_byte,
							millisecond start_time,
							millisecond end_time) :
  Input_Event(start_time, end_time),
//  scan_code(the_scan_code),
  character(character),
  extended_key(extended_key),
  second_byte(second_byte) {
};

boolean Key_Event::interpret(boolean &event_flag,
									  unsigned char &key,
									  boolean &extended_flag,
									  city_coordinate &delta_x,
									  city_coordinate &delta_y,
									  char &mouse_button_status,
                             char &joystick_button_status,
									  long &duration,
									  unsigned char &second_key) {
	event_flag = TRUE;
	delta_x = 0;
	delta_y = 0;
	mouse_button_status = 0;
   joystick_button_status = 0;
	if (end_time == 0) {
	  duration = tt_millisecond_delta;
	} else {
	  duration = end_time-start_time;
	};
//	if (end_time == 0) {  // end it now
//		duration = polling_time-start_time;
//	} else {
//		duration = end_time-start_time;
//	};
	processed = TRUE;
	key = character;
	extended_flag = extended_key;
	second_key = second_byte;
#if TT_DEBUG_ON
	if (tt_debug_mode == 290803) { // || tt_debug_mode == 3) { // == 3 just for now 100105
		tt_error_file() << (int) key << " (" << (char) key << ") interpreted on frame " << tt_frame_number << endl;
	};
#endif
	return(FALSE);
};

Input_Events::Input_Events(Input_Event *the_event, Input_Events *the_next) :
	event(the_event),
	next(the_next) {};

Input_Events::~Input_Events() {
	delete event;
	if (next != NULL) {
		delete next;
	};
};

Key_Event *Input_Events::insert_keyboard_event(char character, 
															  boolean extended,
															  unsigned char second_byte) {
	if (next == NULL) {
		Key_Event *event = new Key_Event(character,extended,second_byte);
		next = new Input_Events(event);
		return(event);
	} else {
		return(next->insert_keyboard_event(character,extended,second_byte));
	};
};

Button_Event *Input_Events::insert_button_event(InputDeviceType device_type, char button_status) {
	if (!event->same_button_event(device_type, button_status)) {
		if (next == NULL) {
			Button_Event *event = new Button_Event(device_type,button_status);
			next = new Input_Events(event);
			return(event);
		} else {
			return(next->insert_button_event(device_type,button_status));
		};
   } else {
      return(NULL);
   };
};

//void Input_Events::check_if_over() {
//	event->check_if_over();
//	if (next != NULL) next->check_if_over();
//};

Input_Events *Input_Events::interpret(boolean &event_flag,
												  unsigned char &key,
												  boolean &extended_key,
												  city_coordinate &delta_x,
												  city_coordinate &delta_y,
												  char &mouse_button_status,
                                      char &joystick_button_status,
												  long &duration,
												  unsigned char &second_byte) {
	if (event->interpret(event_flag,
								key,extended_key,
								delta_x,delta_y,mouse_button_status,joystick_button_status,duration,second_byte)) {
		// saved for next frame
		event = NULL;
	};
	Input_Events *to_return = next;
	next = NULL;
	// following wasn't necessary since caller does it
//	delete this; // new on 290803 to clean up while processing so no need to flush on an cycle (and lose events between log segments during time travel)
	return(to_return);
};

const short int keyboard_buffer_size = 256;
unsigned char keyboard_first = 0; // just wraps around
unsigned char keyboard_last = 0;
//short int keyboard_buffer_count = 0;

//short int keyboard_buffer[keyboard_buffer_size];
short int *keyboard_buffer = NULL;

void release_keyboard_buffer() {
	if (keyboard_buffer != NULL) delete [] keyboard_buffer;
};

boolean add_key_to_buffer(int key) {
/*#if TT_DEBUG_ON
	if (tt_debug_mode == 100105) {
		tt_error_file() << "Adding " << (int) key << " to buffer; f=" << tt_frame_number << endl;
	};
#endif*/ 
  	if (keyboard_buffer == NULL) {
		keyboard_buffer = new short int[keyboard_buffer_size];
	};
	if (tt_last_key_frame_number == tt_frame_number && tt_last_key_sensed == key) {
		return(FALSE); // new on 271000
	};
	// revived these unused globals for a similar purpuse
	tt_last_key_frame_number = tt_frame_number;
	tt_last_key_sensed = key;
//	if (keyboard_buffer_count < keyboard_buffer_size) {
   if (keyboard_last+1 != keyboard_first) { // not full
		keyboard_buffer[keyboard_last++] = (short int) key;
#if TT_DEBUG_ON
		if (tt_debug_mode == 202202) {
			tt_error_file() << "Adding " << (int) key << " to buffer position " << (keyboard_last-1) << " f=" << tt_frame_number << endl;
		};
#endif 
//		keyboard_buffer_count++;
		return(TRUE);
	};
	return(FALSE);
};

boolean next_key_from_buffer(short int &key) {
	if (keyboard_last == keyboard_first) {
		return(FALSE); // empty
	};
//	key = keyboard_buffer[keyboard_index-keyboard_buffer_count]; // oldest one
   key = keyboard_buffer[keyboard_first++];
#if TT_DEBUG_ON
	if (tt_debug_mode == 202202) {
		tt_error_file() << "Next key from buffer is " << (int) key << " from buffer position " << (keyboard_first-1) << " f=" << tt_frame_number << endl;
	};
#endif 
//	keyboard_buffer_count--;
 	return(TRUE);
};

void record_key(unsigned char key, boolean extended, unsigned char second_byte) {
	// second byte added on 240800 for wide characters - e.g. Japanese
#if TT_DEBUG_ON
	if (tt_debug_mode == 290803) { // || tt_debug_mode == 3) { // == 3 just for now 100105
		tt_error_file() << (int) key << " (" << (char) key << ") just typed on frame " << tt_frame_number << endl;
	};
#endif
	if (tt_time_travel != TIME_TRAVEL_OFF) {
		return;
	};
	Key_Event *event;
	if (input_events == NULL) {
		event = new Key_Event(key,extended,second_byte);
		input_events = new Input_Events(event);
	} else {
		input_events->insert_keyboard_event(key,extended,second_byte);
	};
};

void add_button_event(InputDeviceType device_type, char button_status) {
// tt_error_file() << "Adding button event " << (int) device_type << " whose status is " << (int) mouse_button_status << endl;
	if (tt_time_travel != TIME_TRAVEL_OFF) {
		return;
	};
	if (input_events != NULL) {
		input_events->insert_button_event(device_type,button_status);
	} else {
		Button_Event *event = new Button_Event(device_type,button_status);
		input_events = new Input_Events(event);
	};
};

city_coordinate cumulative_delta_x = 0;
city_coordinate cumulative_delta_y = 0;

#if TT_ABSOLUTE_MOUSE
city_coordinate absolute_goal_x = 0; // new on 290801 -- this is for touch screens, tablets and the like
city_coordinate absolute_goal_y = 0; // goal is where the mouse click happened and we move towards it at max_mouse_speed
const long max_absolute_mouse_speed = 1000; 
millisecond goal_good_until = 0;

void compute_delta_to_goal(city_coordinate &delta_x, city_coordinate &delta_y) {
   TTRegion programmer_region;
   if (// tt_current_time <= goal_good_until && 
       tt_programmer->compute_selection_region(programmer_region)) { // prior to 170402 was tt_programmer->lower_left_corner(programmer_x,programmer_y)) {
      city_coordinate programmer_x, programmer_y;
      programmer_region.region_center(programmer_x,programmer_y);
//		long scale = tt_screen->current_scale();
//		delta_x = ((absolute_goal_x-((programmer_x-tt_screen->min_x())*100)/scale)*tt_millisecond_delta)/max_absolute_mouse_speed;
//		delta_y = (((((programmer_y-tt_screen->min_y())*100)/scale)-absolute_goal_y)*tt_millisecond_delta)/max_absolute_mouse_speed;
//		delta_x = absolute_goal_x-(tt_screen->screen_x(programmer_x-tt_screen->min_x())*tt_screen->one_pixel_in_x_city_units());
//		delta_x -= tt_programmer->current_width()/2; // so click is interpreted as sending the center to point -- commented out on 170402 since now sends center of selection region to goal
//		delta_y = absolute_goal_y-(tt_screen->screen_y(programmer_y-tt_screen->min_y())*tt_screen->one_pixel_in_y_city_units());
//		delta_y -= tt_programmer->current_height()/2;
      // above simplified on 170402 to the following
      delta_x = absolute_goal_x-programmer_x;
      delta_y = absolute_goal_y-programmer_y;
#if TT_DEBUG_ON
		if (tt_debug_mode == 290801) {
			tt_error_file() << "delta_x= " << delta_x << "; absolute_goal_x=" << absolute_goal_x 
							<< "; programmer_x=" << programmer_x  << "; screen_x=" << tt_screen->screen_x(programmer_x) << endl;
			tt_error_file() << "delta_y= " << delta_y << "; absolute_goal_y=" << absolute_goal_y 
							<< "; programmer_y=" << programmer_y << "; screen_y=" << tt_screen->screen_y(programmer_y) << endl;
			tt_error_file().flush();
		};
#endif
	} else {
		delta_x = 0;
		delta_y = 0;
	};
};

long last_absolute_mouse_mode_frame_number = -1;
const millisecond ignore_mouse_button_in_absolute_mouse_mode_duration = 2000;
//millisecond ignore_mouse_button_until = 0;
#endif

void user_actions(boolean &event_flag,
						unsigned char &key,
						boolean &extended_key,
						city_coordinate &delta_x,
						city_coordinate &delta_y,
						char &mouse_button_status,
                  char &joystick_button_status,
						long &duration,
						unsigned char &second_byte) {
	// provides a uniform interface to keyboard, mouse, and joystick
	// event_flag is TRUE if cursor moved or button pushed
	// returns TRUE is there are still more unprocessed input events
	tt_shift_down = (GetAsyncKeyState(VK_SHIFT) < 0); // new on 220900
	tt_control_down = (GetAsyncKeyState(VK_CONTROL) < 0);
	if (input_events == NULL) {
		event_flag = (tt_shift_down || tt_control_down); // was FALSE prior to 220900
		key = 0;
		extended_key = FALSE;
		second_byte = 0;
		delta_x = 0;
		delta_y = 0;
		mouse_button_status = 0;
      joystick_button_status = 0;
		duration = 0;
		return;
	};
	city_coordinate more_delta_x,more_delta_y;
	Input_Events *new_input_events =
		input_events->interpret(event_flag,
										key,extended_key,more_delta_x,more_delta_y,
										mouse_button_status,joystick_button_status,duration,second_byte);
#if TT_ABSOLUTE_MOUSE
	if (tt_mouse_mode == ABSOLUTE_MOUSE_MODE && (virtual_left_button(mouse_button_status,FALSE) && !(mouse_button_status&BUTTON_STILL_DOWN) && last_absolute_mouse_mode_frame_number != tt_frame_number)) {
		// new on 290801
//		absolute_goal_x = more_delta_x;
//		absolute_goal_y = more_delta_y;
//		city_coordinate old_absolute_goal_x = absolute_goal_x;
//		city_coordinate old_absolute_goal_y = absolute_goal_y;
		last_mouse_deltas(absolute_goal_x,absolute_goal_y);
//		if (old_absolute_goal_x != absolute_goal_x || old_absolute_goal_y != absolute_goal_y) {
//			goal_good_until = tt_current_time; // instantaneous for now - should be time to get there
//#if TT_DEBUG_ON
//			if (tt_debug_mode == 290801) {
//				tt_error_file() << "Goal good until " << goal_good_until << endl;
//			};
//#endif
//		};
//		compute_delta_to_goal(more_delta_x,more_delta_y); // removed on 180402
		last_absolute_mouse_mode_frame_number = tt_frame_number;
//		ignore_mouse_button_until = tt_current_time+ignore_mouse_button_in_absolute_mouse_mode_duration;
	};
//	if (mouse_button_status != 0 && ignore_mouse_button_until < tt_current_time) { // new on 140901
//		mouse_button_status = 0;
//		event_flag = (joystick_button_status || key || extended_key);
//	};
#endif
	if (event_flag) {
		cumulative_delta_x += more_delta_x;
		cumulative_delta_y += more_delta_y;
	} else {
		delta_x = 0;
		delta_y = 0;
		duration = 0;
	};
	if (new_input_events == NULL) { // last one
		if (event_flag) {
			delta_x = cumulative_delta_x;
			delta_y = cumulative_delta_y;
			// these are logged as short ints so to be sure log replaying
         // is accurate these are clipped
			if (delta_x > max_short) {
				delta_x = max_short;
			} else if (delta_x < min_short) {
				delta_x = min_short;
			};
			if (delta_y > max_short) {
				delta_y = max_short;
			} else if (delta_y < min_short) {
				delta_y = min_short;
			};
		};
		cumulative_delta_x = 0;
		cumulative_delta_y = 0;
	} else {
		if (mouse_button_status == 0 && key == 0) { // ignore this one
			delete input_events;
			input_events = new_input_events;
			user_actions(event_flag,key,extended_key,delta_x,delta_y,mouse_button_status,joystick_button_status,duration,second_byte);
//			if (mouse_button_status != 0 && ignore_mouse_button_until < tt_current_time) { // new on 140901
//				mouse_button_status = 0;
//				event_flag = (joystick_button_status || key || extended_key);
//			};
			return;
		} else {
			delta_x = 0;
			delta_y = 0;
		};
	};
	delete input_events; // the tail has been NULLed and is the new_input_events
	input_events = new_input_events;
	if (!event_flag) { // new on 220900
		event_flag = (tt_shift_down || tt_control_down);
	};
};

void flush_input_events() {
	if (input_events != NULL) {
		delete input_events;
		input_events = NULL;
	};
	if (next_frame_input_events != NULL) {
		delete next_frame_input_events;
		next_frame_input_events = NULL;
	};
};

/*
char tt_mouse_status(int ) {
  return(0);
};
void record_mouse_state(int mouse_status) {
	if (mouse_status) {
		add_button_event(MOUSE_DEVICE,tt_mouse_status(mouse_status));
	} else { // check if I missed a click
		int press_count = 1;
		for (char i = (char) mouse_buttons; i > 0; i--) {
			 for (int j = 0; j < press_count; j++) {
				 Button_Event *new_event = add_button_event(MOUSE_DEVICE,i);
             if (new_event != NULL) {
                // split the time equally -- might have had button up more
                // times aren't right but the differences betweeen them are
                new_event
                  ->set_end_time(new_event->the_start_time()
											+tt_millisecond_delta/press_count);
				 };
			 };
		};
	};
};
*/



