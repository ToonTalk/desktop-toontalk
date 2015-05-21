// Copyright (c) 1992-2003.  Ken Kahn, Animated Programs, All rights reserved

#ifndef __TT_USER_H

#define __TT_USER_H

void save_input_events();
void restore_input_events();

boolean log_or_user_actions(boolean &event,
									 unsigned char &key,
									 boolean &extended_key,
                            city_coordinate &delta_x,
                            city_coordinate &delta_y,
                            char &mouse_button_status,
                            char &joystick_button_status,
                            millisecond &duration,
									 unsigned char &second_byte);

enum InputDeviceType {MOUSE_DEVICE,JOYSTICK_DEVICE};

class Input_Event {
  public:
	 Input_Event(millisecond start_time,
                millisecond end_time);
    virtual boolean interpret(boolean &event,
										unsigned char &key,
										boolean &extended_key,
										city_coordinate &delta_x,
                              city_coordinate &delta_y,
                              char &mouse_button_status,
                              char &joystick_button_status,
                              long &duration,
										unsigned char &second_byte) = 0;
	 virtual boolean same_keyboard_event(int new_scan_code) {
		 return(FALSE);
	 };
	 virtual boolean same_button_event(InputDeviceType device_type, char button_status) {
		 return(FALSE);
	 };
//	 virtual void check_if_over() = 0;
	 millisecond the_start_time() {
		 return(start_time);
	 };
	 millisecond the_end_time() {
		 return(end_time);
	 };
	 void set_start_time(millisecond new_start_time) {
		 start_time = new_start_time;
	 };
	 void set_end_time(millisecond new_end_time) {
		 end_time = new_end_time;
	 };
  protected:
	 millisecond start_time, end_time;
	 boolean processed;
};

class Button_Event : public Input_Event {
  public:
	Button_Event(InputDeviceType device_type, char button_status,
					 millisecond start_time=tt_current_time,
					 millisecond end_time=0);
	boolean same_button_event(InputDeviceType new_device_type, char new_button_status);
//	void check_if_over();
	boolean interpret(boolean &event,
							unsigned char &key,
							boolean &extended_key,
							city_coordinate &delta_x,
							city_coordinate &delta_y,
							char &mouse_button_status,
                     char &joystick_button_status,
							long &duration,
							unsigned char &second_byte);
  private:
	InputDeviceType device_type;
	char button_status;
};

class Move_Event : public Input_Event {
  public:
	Move_Event(city_coordinate delta_x,
				  city_coordinate delta_y,
				  millisecond start_time=tt_current_time,
				  millisecond end_time=0);
	boolean interpret(boolean &event,
							unsigned char &key,
							boolean &extended_key,
							city_coordinate &delta_x,
							city_coordinate &delta_y,
							char &mouse_button_status,
                     char &joystick_button_status,
							long &duration,
							unsigned char &second_byte);
//	void check_if_over() {}; // do nothing since created at end of frame
  private:
	city_coordinate delta_x, delta_y;
};

class Key_Event : public Input_Event {
  public:
	Key_Event(char character,
				 boolean extended, // added after moving to Windows
				 unsigned char second_byte, // for wide text (e.g. Japanese) - added 240800
				 millisecond start_time=tt_current_time,
				 millisecond end_time=0);
	boolean interpret(boolean &event,
							unsigned char &key,
							boolean &extended,
							city_coordinate &delta_x,
							city_coordinate &delta_y,
							char &mouse_button_status,
                     char &joystick_button_status,
							long &duration,
							unsigned char &second_byte); // new on 240800
//	void check_if_over();
  private:
	char character;
	boolean extended_key;
	unsigned char second_byte;
};

void record_key(unsigned char key, boolean extended=FALSE, unsigned char second_byte=0);

boolean add_key_to_buffer(int key);
boolean next_key_from_buffer(short int &key);

void add_button_event(InputDeviceType device_type, char button_status);

class Input_Events {
  public:
	 Input_Events(Input_Event *event, Input_Events *next=NULL);
	 ~Input_Events();
	 Key_Event *insert_keyboard_event(char character, boolean extended, unsigned char second_byte);
	 Button_Event *insert_button_event(InputDeviceType device_type, char button_status);
//	 void check_if_over();
	 Input_Events *interpret(boolean &event,
									 unsigned char &key,
									 boolean &extended_key,
									 city_coordinate &delta_x,
									 city_coordinate &delta_y,
									 char &mouse_button_status,
                            char &joystick_button_status,
									 long &duration,
									 unsigned char &second_byte);
//    void deltas(city_coordinate &delta_x, city_coordinate delta_y);
	 Input_Event *first() {
		 return(event);
	 };
	 Input_Events *rest() {
		 return(next);
	 };
  private:
	 Input_Event *event;
	 Input_Events *next;
};

void last_mouse_deltas(city_coordinate &delta_x, city_coordinate &delta_y);
void set_last_mouse_deltas(city_coordinate delta_x, city_coordinate delta_y);

void record_mouse_movement(int mouse_x, int mouse_y);
void record_mouse_delta(coordinate delta_mouse_x, coordinate delta_mouse_y);
void record_key_down(int scan_code);
//void record_key(unsigned char key, boolean standard);
void release_keyboard_buffer();

void user_actions(boolean &event,
						unsigned char &key,
						boolean &extended,
                  city_coordinate &delta_x,
                  city_coordinate &delta_y,
                  char &mouse_button_status,
                  char &joystick_button_status,
                  long &duration,
						unsigned char &second_byte);

void flush_input_events();


#endif
