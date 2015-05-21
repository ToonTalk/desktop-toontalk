// Copyright (c) 1992-2003.  Ken Kahn, Animated Programs, All rights reserved.  

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif   
#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif   
#if !defined(__TT_TOOLS_H)   
#include "tools.h"
#endif   
#if !defined(__TT_CUBBY_H)   
#include "cubby.h"
#endif   
#if !defined(__TT_BIRD_H)   
#include "bird.h"
#endif  
#if !defined(__TT_MARTIAN_H)   
#include "martian.h"
#endif   
#if !defined(__TT_NUMBER_H)   
#include "number.h"
#endif
#if !defined(__TT_MOUSE_H)   
#include "mouse.h"
#endif   
#if !defined(__TT_TEXT_H)   
#include "text.h"
#endif   
#if !defined(__TT_PAD_H)   
#include "pad.h"
#endif   
#if !defined(__TT_PRGRMMR_H)   
#include "prgrmmr.h"
#endif
//#if !defined(__TT_HYPERBOT_H)
//#include "hyperbot.h"
//#endif 
#if !defined(__TT_PRGRMMR_H)   
#include "prgrmmr.h"
#endif   
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif 
#if !defined(__TT_LOG_H)   
#include "log.h"
#endif   
#if !defined(__TT_BCKGRND_H)   
#include "bckgrnd.h"
#endif   
#if !defined(__TT_FLOOR_H)   
#include "floor.h"
#endif   
#if !defined(__TT_WINMAIN_H)   
#include "winmain.h"
#endif   
#if !defined(__TT_USER_H)   
#include "user.h" // for last_mouse_deltas
#endif   
#if !defined(__TT_HELP_H)   
#include "help.h"
#endif   
//#include "martian.h"
#if !defined(__TT_PICTURE_H)   
#include "picture.h"
#endif 
#if !defined(__TT_REMOTE_H)
#include "remote.h"
#endif
#if TT_DIRECT_INPUT
#if !defined(__TT_INPUT_H)
#include "input.h"
#endif
#endif
#if !defined(__TT_ROBOT_H)
#include "robot.h"
#endif
#if !defined(__TT_ROOM_H)
#include "room.h"
#endif
   
#include <string.h>

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

city_coordinate previous_mouse_delta_x = max_long;
city_coordinate previous_mouse_delta_y = max_long;

boolean *motor_state_changed = NULL;
int *current_power_levels = NULL;

unsigned char lego_sensor_states = 15; // not yet read
boolean sensor_request_sent = FALSE;
boolean *sensor_count_request_sent = NULL;
unsigned long *lego_sensor_count = NULL;
unsigned short *lego_sensor_count_delta = NULL;
boolean *sensor_count_needs_reading = NULL;

Remote::Remote(Picture *controlee) :
	controlee(controlee),
//	collidee(NULL),
	collision(NULL),
//	old_collidees(NULL),
//	current_collidees(NULL),
//	new_x_collidees_count(0),
//	new_y_collidees_count(0),

//	last_collision_direction(NO_COLLISION) 
//	x_penetration(0), 
//	y_penetration(0),
//	speed_changed_flag(TRUE)
//	x_collided(FALSE),
//	y_collided(FALSE) 
	remote_notebooks(NULL) {
   first_identifier = controlee->return_first_identifier();
   int last = controlee->return_last_identifier();
   length = (unsigned char) (1+last-first_identifier);
   remotes = new SpritesPointer[length];
	for (int i = 0; i < length; i++) {
		remotes[i] = NULL;
	};
};

Remote::~Remote() {
//	if (!tt_resetting_or_destroying_city) { // experiment on 250703 to comment this out 
		// conditional new on 270603 since Gordon had a time travel crash - not clear if this fix introduces a leak
		for (int i = 0; i < length; i++) {
			if (remotes[i] != NULL) {
				// experiment on 141003 to comment this out
//#if TT_ALPHA2
//				remotes[i]->decrement_ref_count_of_each(); // new on 131003
//#endif
				remotes[i]->remove_all();
				delete remotes[i];
			};
		};
//	};
   delete [] remotes;
	if (remote_notebooks != NULL) {
// ref count stuff fails here
//      destroy_sprites(remote_notebooks); // only if refcount is zero
		remote_notebooks->remove_all();
		delete remote_notebooks;
	};
   if (collision != NULL) delete collision;
};

flag left_button_down_reported = FALSE;
flag middle_button_down_reported = FALSE;
flag right_button_down_reported = FALSE;

flag left_button_still_down = FALSE;
flag middle_button_still_down = FALSE;
flag right_button_still_down = FALSE;

//short int last_key_sensed = 0;
flag reported_key_not_available = FALSE;
flag last_shift_down = FALSE;
flag last_control_down = FALSE;
flag last_game_switch_on = FALSE;
flag last_sound_on = FALSE;
unsigned char last_screen_size = 255; // new on 100602

#if TT_SICS_ULTRASOUND_CONTROLLER
millisecond next_ultrasound_read = 20;
int two_byte_hex_to_int(char h1, char h2) {
   return((h1-'0')*16+(h2-'0'));
};
#endif

boolean any_active(Sprites *list) {
   Sprites *remaining = list;
   while (remaining != NULL) {
		Sprite *sprite = remaining->first();
      if (sprite->ok_to_activate()) { // was sprite->active() && !sprite->inside_vacuum() prior to 200802 but breaks if in_notebook
         return(TRUE);
      };
      remaining = remaining->rest();
   };
   return(FALSE);
};

#if TT_DIRECT_INPUT
// only via Direct Input are there joysticks

unsigned long joystick_button_down_reported[max_number_of_joysticks] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void Remote::update_joystick_globals(short int joystick_number) {
   for (int i = JOYSTICK_SPEED_X_REMOTE; i <= JOYSTICK_BUTTON32_DOWN_REMOTE; i++) {
      RemoteIdentifier identifier = (RemoteIdentifier) i;
      if (any_active(remotes_for(identifier))) {
         switch (identifier) {
            case JOYSTICK_SPEED_X_REMOTE:
               // 1000 means it will take 1 second to cross the screen (think about whether vertical speed should reflect aspect ratio of screen)
               changed(identifier,joystick_x(joystick_number));
               break;
            case JOYSTICK_SPEED_Y_REMOTE:
               changed(identifier,-joystick_y(joystick_number));
               break;
            case JOYSTICK_SPEED_Z_REMOTE:
               changed(identifier,joystick_z(joystick_number));
               break;
            case JOYSTICK_AXIS_ROTATION_X_REMOTE:
               changed(identifier,joystick_axis_x(joystick_number));
               break;
            case JOYSTICK_AXIS_ROTATION_Y_REMOTE:
               changed(identifier,joystick_axis_y(joystick_number));
               break;
            case JOYSTICK_AXIS_ROTATION_Z_REMOTE:
               changed(identifier,joystick_axis_z(joystick_number));
               break;
            default:
               int button_number;
               unsigned long mask;
               boolean button_down;
               boolean button_click_remote = (identifier >= JOYSTICK_BUTTON1_REMOTE && identifier <= JOYSTICK_BUTTON32_REMOTE);
               boolean button_down_remote = (identifier >= JOYSTICK_BUTTON1_DOWN_REMOTE && identifier <= JOYSTICK_BUTTON32_DOWN_REMOTE);
               if (button_click_remote) {
                  button_number = identifier-JOYSTICK_BUTTON1_REMOTE;
                  mask = 1<<button_number;
                  button_down = joystick_button(joystick_number,button_number);
                  // just goes true for one cycle
                  if (joystick_button_down_reported[joystick_number] & mask) {
                     changed(identifier,0);
                     joystick_button_down_reported[joystick_number] &= ~mask;
                  } else if (button_down) {
                     if (!joystick_button_still_down(joystick_number,button_number)) {
                        changed(identifier,1);
                        joystick_button_down_reported[joystick_number] |= mask;
//                        joystick_button_still_down[joystick_number] |= mask;
                     };
//                  } else if (joystick_button_still_down(joystick_number,button_number)) {
//                     joystick_button_still_down[joystick_number] &= ~mask;
                  };
//                  if (button_down) {
//                     joystick_button_still_down[joystick_number] |= mask;
//                  } else if (joystick_button_still_down[joystick_number] & mask) {
//                     joystick_button_still_down[joystick_number] &= ~mask;
//                  };
               } else if (button_down_remote) {
                  button_number = identifier-JOYSTICK_BUTTON1_DOWN_REMOTE;
                  button_down = joystick_button(joystick_number,button_number);
                  // shows current state
                  if (button_down) {
                     changed(identifier,1);
                  } else {
                     changed(identifier,0);
                  };
               };
            };
         };
   };
};

#endif

/*
void Remote::update_room_decoration() {
   Sprites *decoration_remotes = remotes_for(ROOM_DECORATION_REMOTE);
   if (decoration_remotes != NULL) {
      Remote_Picture *decoration_remote = (Remote_Picture *) decoration_remotes->first();
      Sprite *appearance = decoration_remote->pointer_to_appearance();
      if (appearance != NULL) {
         Background *background = decoration_remote->pointer_to_background();
         if (background != NULL) {
            Room *room = background->pointer_to_room();
            if (room != NULL) {
               room->set_wall_decoration(NULL); // reset and then add in case some have been vacuumed or whatever
               Sprites *remaining = appearance->pointer_to_followers();
               while (remaining != NULL) {
                  position_and_add_to_wall(appearance,remaining->first()->copy(),room);
                  remaining = remaining->rest();
               };
            };
         };
      };
   };
};

void Remote::update_house_decoration() {
   Sprites *decoration_remotes = remotes_for(HOUSE_DECORATION_REMOTE);
   if (decoration_remotes != NULL) {
      Remote_Picture *decoration_remote = (Remote_Picture *) decoration_remotes->first();
      Sprite *appearance = decoration_remote->pointer_to_appearance();
      if (appearance != NULL) {
         Background *background = decoration_remote->pointer_to_background();
         if (background != NULL) {
            House *house = background->pointer_to_house();
            if (house != NULL) {
               house->set_decoration(NULL,TRUE); // reset and then add in case some have been vacuumed or whatever
               Sprites *remaining = appearance->pointer_to_followers();
               while (remaining != NULL) {
                  position_and_add_to_house(appearance,remaining->first()->copy(),house,TRUE);
                  remaining = remaining->rest();
               };
            };
         };
      };
   };
};

void Remote::update_roof_decoration() {
   Sprites *decoration_remotes = remotes_for(ROOF_DECORATION_REMOTE);
   if (decoration_remotes != NULL) {
      Remote_Picture *decoration_remote = (Remote_Picture *) decoration_remotes->first();
      Sprite *appearance = decoration_remote->pointer_to_appearance();
      if (appearance != NULL) {
         Background *background = decoration_remote->pointer_to_background();
         if (background != NULL) {
            House *house = background->pointer_to_house();
            if (house != NULL) {
               house->set_decoration(NULL,FALSE); // reset and then add in case some have been vacuumed or whatever
               Sprites *remaining = appearance->pointer_to_followers();
               while (remaining != NULL) {
                  position_and_add_to_house(appearance,remaining->first()->copy(),house,FALSE);
                  remaining = remaining->rest();
               };
            };
         };
      };
   };
};
*/

void Remote::update_globals() {
	if (//tt_game_switch_on && 
		 (remotes_for(MOUSE_SPEED_X_REMOTE) != NULL ||
		  remotes_for(MOUSE_SPEED_Y_REMOTE) != NULL)) {
		city_coordinate delta_x, delta_y;  
		last_mouse_deltas(delta_x,delta_y);
      if (absolute_mouse_mode()) {
         changed(MOUSE_SPEED_X_REMOTE,tt_screen->user_x(((delta_x-previous_mouse_delta_x)*1000)/tt_millisecond_delta));
         changed(MOUSE_SPEED_Y_REMOTE,tt_screen->user_y(((delta_y-previous_mouse_delta_y)*1000)/tt_millisecond_delta));
		   previous_mouse_delta_x = delta_x;
         previous_mouse_delta_y = delta_y;
      } else {
         if (delta_x != previous_mouse_delta_x) {
			   changed(MOUSE_SPEED_X_REMOTE,tt_screen->user_x((delta_x*1000)/tt_millisecond_delta));
            previous_mouse_delta_x = delta_x;
         };
		   if (delta_y != previous_mouse_delta_y) {
		      changed(MOUSE_SPEED_Y_REMOTE,tt_screen->user_y((delta_y*1000)/tt_millisecond_delta));
            previous_mouse_delta_y = delta_y;
         };	
		};
	};
	// maybe only do this if needed
	short int key;
	boolean key_available = next_key_from_buffer(key);
//	for (RemoteIdentifier identifier = MOUSE_LEFT_BUTTON_REMOTE;
//		  identifier <= TEXT_TO_SPEECH_REMOTE; identifier++) {
	for (int i = MOUSE_LEFT_BUTTON_REMOTE; i <= TEXT_TO_SPEECH_REMOTE; i++) {
      RemoteIdentifier identifier = (RemoteIdentifier) i;
      // no need to include the decoration remotes since they are the only way to change things
		if (any_active(remotes_for(identifier))) {
			switch (identifier) {
				case MOUSE_LEFT_BUTTON_REMOTE: 
					// just goes true for one cycle
					// went "virtual" on 051199
					if (left_button_down_reported) {
						changed(identifier,0);
						left_button_down_reported = FALSE;
					} else if (virtual_left_button(tt_current_mouse_button) && // tt_current_mouse_button&LEFT_BUTTON &&
								  !(tt_current_mouse_button&BUTTON_STILL_DOWN)) {
						left_button_down_reported = TRUE;
						changed(identifier,1);
					};
					break;
				case MOUSE_MIDDLE_BUTTON_REMOTE:
					if (middle_button_down_reported) {
						changed(identifier,0);
						middle_button_down_reported = FALSE;
					} else if (virtual_middle_button(tt_current_mouse_button) && // tt_current_mouse_button&MIDDLE_BUTTON &&
								  !(tt_current_mouse_button&BUTTON_STILL_DOWN)) {
						middle_button_down_reported = TRUE;
						changed(identifier,1);
					};
					break;
				case MOUSE_RIGHT_BUTTON_REMOTE:
					if (right_button_down_reported) {
						changed(identifier,0);
						right_button_down_reported = FALSE;
					} else if (virtual_right_button(tt_current_mouse_button) && // tt_current_mouse_button&RIGHT_BUTTON &&
								  !(tt_current_mouse_button&BUTTON_STILL_DOWN)) {
						right_button_down_reported = TRUE;
						changed(identifier,1);
					};
					break;
				case MOUSE_LEFT_BUTTON_DOWN_REMOTE:
					// shows current state
					// went "virtual" on 051199
					if (left_button_still_down) {
						if (!(tt_current_mouse_button&BUTTON_STILL_DOWN)) {
							changed(identifier,0);
							left_button_still_down = FALSE;
						};
					} else if (virtual_left_button(tt_current_mouse_button)) { // (tt_current_mouse_button&LEFT_BUTTON) {
						changed(identifier,1);
						left_button_still_down = TRUE;
					};
					break;
				case MOUSE_MIDDLE_BUTTON_DOWN_REMOTE:
					if (middle_button_still_down) {
						if (!(tt_current_mouse_button&BUTTON_STILL_DOWN) ||
							 (!(tt_current_mouse_button&MIDDLE_BUTTON) && !(tt_current_mouse_button&CONTROL_KEY_DOWN))) {
							changed(identifier,0);
							middle_button_still_down = FALSE;
						};
					} else if (virtual_middle_button(tt_current_mouse_button)) { // (tt_current_mouse_button&MIDDLE_BUTTON) {
						changed(identifier,1);
						middle_button_still_down = TRUE;
					};
					break;
				case MOUSE_RIGHT_BUTTON_DOWN_REMOTE:
					if (right_button_still_down) {
						if (!(tt_current_mouse_button&BUTTON_STILL_DOWN) ||
							 (!(tt_current_mouse_button&RIGHT_BUTTON) && !(tt_current_mouse_button&SHIFT_KEY_DOWN))) {
							changed(identifier,0);
							right_button_still_down = FALSE;
						};
					} else if (virtual_right_button(tt_current_mouse_button)) { // (tt_current_mouse_button&RIGHT_BUTTON) {
						changed(identifier,1);
						right_button_still_down = TRUE;
					};
					break;
				case LAST_KEYSTROKE_REMOTE: 
					if (key_available) {
						changed(identifier,key);
					};
//					// if key is upper case fix...
//					if (key > 0) {
//						if (GetAsyncKeyState(48+key-'0') < 0) {
//							changed(identifier,key); // new on 081200
//						} else {
//							changed(identifier,0);
//						};
//					};
					break;
				case KEYBOARD_BUTTON_REMOTE: 
//					if (tt_last_key_frame_number == tt_frame_number) {
//						changed(identifier,tt_last_key_sensed);
//						last_key_sensed = tt_last_key_sensed;
//						break;
//					}
//					if (last_key_sensed != 0) { 
//						changed(identifier,0);
//						last_key_sensed = 0;
//					};
					if (key_available) {
						changed(identifier,key);
						reported_key_not_available = FALSE;
					} else if (!reported_key_not_available) {
						changed(identifier,0);
						reported_key_not_available = TRUE;
					};
					break;
				case SHIFT_DOWN_REMOTE: 
					if (tt_shift_down != last_shift_down) {
						last_shift_down = (flag) !last_shift_down;
						changed(identifier,last_shift_down);
					};
					break;
				case CONTROL_DOWN_REMOTE: 
					if (tt_control_down != last_control_down) {
						last_control_down = (flag) !last_control_down;
						changed(identifier,last_control_down);
					};
					break;
				case CLOCK_REMOTE:
					changed(identifier,tt_true_millisecond_delta); // was tt_millisecond_delta and even earlier was tt_current_time
					break;
				case RANDOM_REMOTE:
					changed(identifier,my_random(1000)); // 0-999 inclusive
					// changed on 160200 to be from 0-1000 since operations like /100 resulted in uneven distribution
					break;
				case CLIPBOARD_REMOTE: {
					// policy is that if blank then check clipboard
					boolean new_clipboard_value;
					Sprite *appearance = NULL;
					// added FALSE below on 280404 so that this doesn't consider files on clipboard
					string clipboard_text = tt_main_window->clipboard_text(new_clipboard_value,&appearance,FALSE,FALSE,FALSE);
					if (new_clipboard_value) {
						Sprites *remaining = remotes_for(CLIPBOARD_REMOTE);
						if (clipboard_text != NULL) {
//                     Background *floor = remaining->first()->pointer_to_background();
							// no need to do the following if none of the remaining are non-blank - noticed on 090203
							appearance = uudecode_sprite(clipboard_text,TRUE,TRUE,FALSE); // args new on 080200
// commented out the following on 080200 since clipboard might contain a contents of the floor or a house (or a city)
//							if (appearance == NULL) { // couldn't decode it
//                        tt_main_window->clear_clipboard();
//								break;
//							};
							while (remaining != NULL) { // skip any blank remotes
								Remote_Picture *clipboard_remote	= ((Remote_Picture *) remaining->first());
								remaining = remaining->rest();
								if (!clipboard_remote->is_blank()) {
									clipboard_remote->set_appearance(appearance);
									break;
								};
							};
							if (remaining != NULL) {
								// following is only to prevent an infinite loop
								// when adding the clipboard remote to the clipboard -- sigh
								Sprites *remaining_copy = remaining->just_copy_list();
								remaining = remaining_copy;
								while (remaining != NULL) {   // rest get copies
									Remote_Picture *clipboard_remote	= ((Remote_Picture *) remaining->first());
									if (appearance == NULL) {
										clipboard_remote->set_appearance(NULL);
									} else {
										clipboard_remote->set_appearance(appearance->copy());
									};
									remaining = remaining->rest();
								};
								just_destroy_list(remaining_copy); // since was just copied for this
							};
						} else {
							while (remaining != NULL) {
								((Remote_Picture *) remaining->first())->set_appearance(NULL);
								remaining = remaining->rest();
							};
						};
						delete [] clipboard_text; // new on 281199
					};
               };
					break;
				case HAND_VISIBLE_REMOTE:
					if (last_game_switch_on != tt_programmer->invisible()) {
						changed(identifier,!tt_programmer->invisible());
						last_game_switch_on = (flag) tt_programmer->invisible();
					};
					break;
				case SOUND_ON_REMOTE:
					if (last_sound_on != ((tt_sound_option&SOUND_BIT) != 0)) { // was (tt_sound_option&SOUND_BIT) prior to 160301
						last_sound_on = (flag) ((tt_sound_option&SOUND_BIT) != 0);
						changed(identifier,last_sound_on);
					};
					break;
            case SCREEN_SIZE_REMOTE:
               if (last_screen_size != current_dispdib_value()) {
                  last_screen_size = current_dispdib_value();
                  changed(SCREEN_SIZE_REMOTE,last_screen_size);
               };
               break;
         };
		};
	};
};

#if !TT_32 // for now
void Remote::update_hyperbot_globals() {
   if (tt_lego_in_use != SICS_ULTRASOUND_CONTROLLER) {
      // should really split lego ports into in and out
      // is 0 when no controller at all
	   for (int outlet = 0; outlet < tt_lego_ports; outlet++) {
		   if (motor_state_changed[outlet]) {
			   changed((RemoteIdentifier) (LEGO_MOTOR1_REMOTE+outlet),
					     current_power_levels[outlet]);
			   motor_state_changed[outlet] = FALSE; // taken care of
         };
		};
	};
	if (tt_lego_in_use == HYPERBOT_LEGO_CONTROLLER
#if TT_SICS_ULTRASOUND_CONTROLLER
       || tt_lego_in_use == SICS_ULTRASOUND_CONTROLLER
#endif
       ) {
		boolean sensors_need_reading = FALSE;
      int inlet;
		for (inlet = 0; inlet < tt_lego_ports; inlet++) { // 0 is sensor 1
			sensor_count_needs_reading[inlet] = FALSE;
			if (remotes_for((RemoteIdentifier) (LEGO_SENSOR1_REMOTE+inlet)) != NULL) {
				sensors_need_reading = TRUE;
			};
			if (remotes_for((RemoteIdentifier) (LEGO_SENSOR_COUNT1_REMOTE+inlet)) != NULL) {
				sensor_count_needs_reading[inlet] = TRUE;
			};
		};
		if (tt_lego_in_use == HYPERBOT_LEGO_CONTROLLER &&
          sensor_request_sent &&
          !read_byte_from_serial_port(lego_sensor_states)) {
			// can't read reply -- wait??
			lego_sensor_states = 15; //??
		};
#if TT_DEBUG_ON
      if (next_ultrasound_read > tt_current_time) {
         sensors_need_reading = FALSE;
      };
#endif
		if (sensors_need_reading) {
         if (tt_lego_in_use == HYPERBOT_LEGO_CONTROLLER) {
			   add_to_serial_port_buffer(137);
#if TT_SICS_ULTRASOUND_CONTROLLER
         } else if (tt_lego_in_use == SICS_ULTRASOUND_CONTROLLER) {
            add_to_serial_port_buffer('A');
#endif
         };
         sensor_request_sent = TRUE; // read it next frame
		} else {
			sensor_request_sent = FALSE;
		};
#if TT_SICS_ULTRASOUND_CONTROLLER
      if (tt_lego_in_use == SICS_ULTRASOUND_CONTROLLER) {
         char response[12];
         if (sensor_request_sent &&
             read_bytes_from_serial_port((char *) &response,12)) {
            for (inlet = 0; inlet < tt_lego_ports; inlet++) {
               char byte1 = response[inlet*2];
               char byte2 = response[inlet*2+1];
               if (byte1 != 0 && byte2 != 0) {
                  changed((RemoteIdentifier) (LEGO_SENSOR1_REMOTE+inlet),
                          two_byte_hex_to_int(byte1,byte2));
//                  tt_error_file() << byte1 << byte2 << " for input " << inlet
//                               << " value is " << two_byte_hex_to_int(byte1,byte2) << endl;
               };
            };
            next_ultrasound_read = tt_current_time+200;
         };
      } else {
#endif
		   for (inlet = 0; inlet < tt_lego_ports; inlet++) {
			   changed((RemoteIdentifier) (LEGO_SENSOR1_REMOTE+inlet),
					     !((lego_sensor_states>>(4+inlet))&1)); // pick out the bit
			   if (sensor_count_request_sent[inlet]) {
				   if (!read_short_from_serial_port(lego_sensor_count_delta[inlet])) {
					    lego_sensor_count_delta[inlet] = 0;
				   };
			   };
			   if (lego_sensor_count[inlet] != lego_sensor_count_delta[inlet]) {
				    lego_sensor_count[inlet] = lego_sensor_count_delta[inlet];
				   changed((RemoteIdentifier) (LEGO_SENSOR_COUNT1_REMOTE+inlet),
						     lego_sensor_count[inlet]);
			   };
			   if (lego_sensor_count_delta[inlet] != 0) {
				   // following was += so count was since start up
				   // but count since last frame is more useful
				   add_to_serial_port_buffer(159+inlet); // reset counter
				   lego_sensor_count_delta[inlet] = 0;
			   };
			   if (sensor_count_needs_reading[inlet]) {
				   add_to_serial_port_buffer(138+inlet);
				   sensor_count_request_sent[inlet] = TRUE;
			   } else {
				   sensor_count_request_sent[inlet] = FALSE;
            };
			};
//#if TT_DEBUG_ON
//		};
//#endif
		transmit_buffer_to_serial_port(); // no-op if nothing buffered
	} else if (tt_lego_in_use == CONTROL_LAB_LEGO_CONTROLLER) {
//		char message[34] = "p0###Do you byte, when I knock?$$$";
//		for (int i = 0; i < 34; i++) {
//			if (i == 1) {
//				add_to_serial_port_buffer(0);
//			} else {
//				add_to_serial_port_buffer(message[i]);
//			};
//		};
//		add_to_serial_port_buffer(0);
//		add_to_serial_port_buffer(112);
//		transmit_buffer_to_serial_port();
//	long k,j;
//	for (k = 0; k < 1000000L; k++) {
//		j += k; // busy wait
//	};
//		bytes control_lab_input_buffer = new unsigned char[19];
//		read_bytes_from_serial_port(control_lab_input_buffer,19);
//      delete [] control_lab_input_buffer;
//		unsigned char foo;
//		read_byte_from_serial_port(foo);
	};
};
#endif

#if TT_DEBUG_ON
Sprites *Remote::remotes_for(RemoteIdentifier identifier) {
   if (identifier-first_identifier < 0 ||
       identifier-first_identifier >= length) {
      tt_error_file() << "Bad identifier " << (int) identifier << endl;
      return(NULL);
   };
   return(remotes[identifier-first_identifier]);
};
#endif

void Remote::changed(RemoteIdentifier identifier, user_coordinate new_value) {
	if (identifier == APPEARANCE_REMOTE || identifier == COLLIDEE_REMOTE) {
		return; // not an integer like the others
	};
	Sprites *remaining = remotes_for(identifier);
	while (remaining != NULL) {
		Remote_Integer *remote = (Remote_Integer *) remaining->first();
		if (remote->active() && !remote->inside_vacuum()) { // conditions added 190799
			//user_coordinate newest_value = remote->perform_operations(new_value);
			//remote->set_long_value(newest_value,TRUE);
			remote->set_value(remote->perform_operations(new_value),TRUE);
			if (identifier == RANDOM_REMOTE) { // new on 080403 since otherwise all instances have the same value
				new_value = my_random(1000);
			};
		};
 	   remaining = remaining->rest();
	};
};

void Remote::broadcast_new_value(RemoteIdentifier identifier, NumberValue *value, Remote_Integer *exception) { // new on 030302
	// exception is typically the guy who started this
	if (identifier == APPEARANCE_REMOTE || identifier == COLLIDEE_REMOTE) {
		return; // not an integer like the others
	};
	Sprites *remaining = remotes_for(identifier);
	while (remaining != NULL) {
		Remote_Integer *remote = (Remote_Integer *) remaining->first();
		if (remote != exception && remote->active()) { // added active test on 121202 since should not be updating those that are inactive
			remote->set_value(value->copy(),TRUE); // TRUE (ignore_changes) should prevent this from propogating further
		};
 	   remaining = remaining->rest();
	};
};

void Remote::changed_appearance(RemoteIdentifier identifier, Sprite *new_appearance) {
	Sprites *remaining = remotes_for(identifier);
	while (remaining != NULL) {
		Remote_Picture *remote = (Remote_Picture *) remaining->first();
		if (new_appearance != NULL) {
//			boolean saved_tt_copying_entire_thought_bubble = tt_copying_entire_thought_bubble;
//			tt_copying_entire_thought_bubble = (identifier == COLLIDEE_REMOTE); // so followers on back aren't copied until needed
			if (remote->copy_set_appearance(new_appearance)) {
				// don't think I need to refcount new_appearance since won't be colliding with it if it goes away
//				if (identifier == COLLIDEE_REMOTE) {
//					((Picture *) remote->pointer_to_appearance())->set_extra_pointer_while_inaccessible(new_appearance);
//				};
			};
//			tt_copying_entire_thought_bubble = saved_tt_copying_entire_thought_bubble;
		} else {
//			remote->set_appearance(NULL); // commented out and added TRUE below since it does the same thing
			remote->update_picture(TRUE);
		};
 	   remaining = remaining->rest();
	};
};

void Remote::blankness_changed(boolean blank_now) {
	Sprites *remaining = remotes_for(APPEARANCE_REMOTE);
	while (remaining != NULL) {
		Picture *remote = (Picture *) remaining->first();
		if (blank_now) {
			remote->Sprite::become_blank();
		} else {
			remote->Sprite::become_non_blank();
		};
		remote->set_clean_status(DIRTY);
		remaining = remaining->rest();
	};
};	

void Remote::not_clean(CleanStatus status) {
	Sprites *remaining = remotes_for(APPEARANCE_REMOTE);
	while (remaining != NULL) {
		Picture *remote = (Picture *) remaining->first();
		remote->set_clean_status(status);
		remaining = remaining->rest();
	};
};	

//void Remote::speed_changed() {
	// speed changed so which direction to blame for collision might as well
//	last_collision_direction = NO_COLLISION;
	// recompute instead ??
//};

void Remote::changed_vacuum_status(boolean in_vacuum) {
	for (int i = 0; i < length; i++) {
		if (i+first_identifier == APPEARANCE_REMOTE) break;
		Sprites *remaining = remotes[i];
		while (remaining != NULL) {
			 Remote_Integer *remote = (Remote_Integer *) remaining->first();
			 remote->changed_vacuum_status(in_vacuum);
			 remaining = remaining->rest();
		};
	};
};

void Remote::no_longer_on_screen() {
	ignore_collision();
};

void Remote::ignore_collision() {
	if (collision != NULL) {
		// updated on 230499 so that this resets everything properly including COLLIDEE_REMOTE
		no_longer_colliding();
		delete collision;
		collision = NULL;
	};
};

void Remote::controlee_deleted() {
	if (!tt_reclaiming_memory) return; // new on 240204 to simplify and speed up most exits
	for (int i = 0; i < length; i++) {
		Sprites *remaining = remotes[i];
		while (remaining != NULL) {
			remaining->first()->controlee_deleted();
			remaining = remaining->rest();
		};
	};
	if (remote_notebooks != NULL) {
		Sprites *remaining = remote_notebooks;
		while (remaining != NULL) {
			Sprites *next_remaining = remaining->rest(); 
			((Notebook *) remaining->first())->picture_deleted(controlee); // arg new on 280300
// notebook will remove itself
			remaining->set_first(NULL); // taken care of -- had been commented out but restored on 250204 to avoid double deletion warnings
			remaining = next_remaining;
		};
		destroy_sprites(remote_notebooks); // new on 150103 - no op if already removed by calls to remote_notebook_deleted generated above
//		delete remote_notebooks;
//		remote_notebooks = NULL; // new on 291000
	};
};

void Remote::remote_notebook_added(Notebook *notebook) {
	if (remote_notebooks == NULL) {
		remote_notebooks = new Sprites(notebook);
		notebook->increment_ref_count(controlee); // was commented out -- restored on 1/19/98
	} else {
		if (remote_notebooks->insert_at_end_if_not_member(notebook)) { 
			// conditionalized on 301000 so if already there will this won't cause a leak by having too high a ref count
			notebook->increment_ref_count(controlee); // was commented out -- restored on 1/19/98
		};
	};
};

void Remote::remote_notebook_deleted(Notebook *notebook) {
   if (remote_notebooks == NULL) return; // rewritten on 110702 since found doesn't have a value if no remote_notebooks
	boolean found; // new on 301000
//	if (remote_notebooks != NULL) 
   remote_notebooks = remote_notebooks->remove(notebook,&found);
   if (found && !notebook->inside_vacuum()) { 
		// condition added 201199 since it will be destroyed if vacuum belongs to robot -- really???
		// should I worry about user spitting out again??
//		notebook->set_background(NULL); // new on 290300 since background can be destroyed and notebook not
		// commented out on 080400 the above since if floor was deleted without notebook update should be fixed
		notebook->remove_from_floor(FALSE,FALSE,FALSE);
		notebook->destroy(); // if ref count is zero   // was commented out -- restored on 1/19/98
	};
};

void add_follower_each(Sprites *sprites, Sprite *exception, Sprite *new_follower) {
   Sprites *remaining = sprites;
   while (remaining != NULL) {
      Sprite *sprite = remaining->first();
      if (sprite != exception) {
         Sprite *follower_copy = new_follower->top_level_copy();
         set_relative_size_and_location(new_follower,exception,follower_copy,sprite);
         follower_copy->set_subpicture(TRUE,FALSE);  // this is true only because adding it to a Remote_Picture (second arg added 190500)
         sprite->add_follower(follower_copy);
      };
      remaining = remaining->rest();
   };
};

// new on 021203
void add_follower_each_if_in_same_house(Sprites *sprites, Sprite *exception, Sprite *new_follower, House *house) {
   Sprites *remaining = sprites;
   while (remaining != NULL) {
      Sprite *sprite = remaining->first();
      if (sprite != exception && sprite->pointer_to_house() == house) {
         Sprite *follower_copy = new_follower->top_level_copy();
         set_relative_size_and_location(new_follower,exception,follower_copy,sprite);
         follower_copy->set_subpicture(TRUE,FALSE);  // this is true only because adding it to a Remote_Picture (second arg added 190500)
         sprite->add_follower(follower_copy);
      };
      remaining = remaining->rest();
   };
};

void remove_all_followers(Sprites *sprites, Sprite *exception) {
   Sprites *remaining = sprites;
   while (remaining != NULL) {
      Sprite *sprite = remaining->first();
      if (sprite != exception) {
         sprite->remove_all_followers();
      };
      remaining = remaining->rest();
   };
};

void Remote::follower_added(Sprite *leader, Sprite *new_follower) {
   if (tt_dealing_with_follower_change) return; // recursively invoked
	// simplified on 120203
	Sprites *remotes = remotes_for(ROOM_DECORATION_REMOTE);
   if (remotes == NULL || remotes->member_cons(leader) == NULL) {
		remotes = remotes_for(HOUSE_DECORATION_REMOTE);
		if (remotes == NULL || remotes->member_cons(leader) == NULL) {
			remotes = remotes_for(ROOF_DECORATION_REMOTE);
		};
	};
	if (remotes != NULL && remotes->member_cons(leader)) {
		tt_dealing_with_follower_change = TRUE; // could use UnwindProtect in case this code throws an error - noted on 021203
		add_follower_each_if_in_same_house(remotes,leader,new_follower,leader->pointer_to_house()); 
		// new on 120203 since only those remotes for the same house should be updated like this
		tt_dealing_with_follower_change = FALSE;
	};
   //tt_dealing_with_follower_change = TRUE; // could use UnwindProtect in case this code throws an error - noted on 021203
   //// see if leader is a decoration control
   //// and if so update the others of its sort
   //Sprites *remotes = remotes_for(ROOM_DECORATION_REMOTE);
   //if (remotes != NULL && remotes->member_cons(leader)) {
   //   add_follower_each(remotes,leader,new_follower);
   //   tt_dealing_with_follower_change = FALSE;
   //   return;
   //};
   //remotes = remotes_for(HOUSE_DECORATION_REMOTE);
   //if (remotes != NULL && remotes->member_cons(leader)) {
   //   add_follower_each(remotes,leader,new_follower);
   //   tt_dealing_with_follower_change = FALSE;
   //   return;
   //};
   //remotes = remotes_for(ROOF_DECORATION_REMOTE);
   //if (remotes != NULL && remotes->member_cons(leader)) {
   //   add_follower_each(remotes,leader,new_follower);
   //   tt_dealing_with_follower_change = FALSE;
   //   return;
   //};
   //tt_dealing_with_follower_change = FALSE;
};

void Remote::follower_removed(Sprite *leader, Sprite *) {
   // others have copy of old follower so need to reset and add rest ...
   if (tt_dealing_with_follower_change) return; // recursively invoked
   tt_dealing_with_follower_change = TRUE;
   Background *background = leader->pointer_to_background();
   if (background != NULL) {
      Room *room = background->pointer_to_room();
      House *house = background->pointer_to_house();
      Sprites *remotes = remotes_for(ROOM_DECORATION_REMOTE);
      if (room != NULL && remotes != NULL && remotes->member_cons(leader)) {
         remove_all_followers(remotes,leader);
         tt_dealing_with_follower_change = FALSE;
//         room->set_wall_decoration(leader->pointer_to_followers()->top_level_copy());
//			replaced the above on 200601 since geometry was getting messed up
			Sprites *remaining_followers = leader->pointer_to_followers()->top_level_copy();
//			if (remaining_followers == NULL) {
				room->set_wall_decoration(NULL);
//			} else {
				Sprites *remaining = remaining_followers;
				while (remaining != NULL) {
					position_and_add_to_wall(leader,remaining->first(),room);
					remaining = remaining->rest();
				};
				just_destroy_list(remaining_followers);
//			};
      } else {
         remotes = remotes_for(HOUSE_DECORATION_REMOTE);
         if (house != NULL) {
            if (remotes != NULL && remotes->member_cons(leader)) {
               remove_all_followers(remotes,leader);
               tt_dealing_with_follower_change = FALSE;
               house->set_decoration(leader->pointer_to_followers()->top_level_copy(),TRUE,leader);
            } else {
               remotes = remotes_for(ROOF_DECORATION_REMOTE);
               if (remotes != NULL && remotes->member_cons(leader)) {
                  remove_all_followers(remotes,leader);
                  tt_dealing_with_follower_change = FALSE;
                  house->set_decoration(leader->pointer_to_followers()->top_level_copy(),FALSE,leader);
               };
            };
         };
      };
   };
   tt_dealing_with_follower_change = FALSE;
};

void Remote::followers_changed() {
	changed_appearance(CHILDREN_REMOTE,NULL);
};

void Remote::leader_changed() {
	changed_appearance(ANCESTORS_REMOTE,NULL);
};

/*
// used to update remote notebooks
void Remote::follower_added(Sprite *subpicture) {
	Sprites *remaining = remote_notebooks;
	while (remaining != NULL) {
		Notebook *notebook = (Notebook *) remaining->first();
		int current_page_number = notebook->left_page_number();
		short int page = notebook->last_non_blank_page_number();
		if (!(page&1)) page--; // if even subtract one -- ok??
		notebook->add_page(page++,subpicture->shallow_copy()); // left side
		Notebook *subnotebook = subpicture->new_remote_notebook();
		notebook->add_page(page++,subnotebook);
		notebook->go_to_page((short int) current_page_number); // put it back
		remaining = remaining->rest();
	};
};

void Remote::follower_removed(Sprite *subpicture) {
	Sprites *remaining = remote_notebooks;
	while (remaining != NULL) {
		Notebook *notebook = (Notebook *) remaining->first();
		short int page_number = notebook->page_number_of_item(subpicture);
		if (page_number > 0) {
			notebook->remove_page(page_number);
			notebook->remove_page((short int) (page_number+1));
		};
		remaining = remaining->rest();
	};
};
*/

boolean Remote::add_remote(RemoteIdentifier identifier, Sprite *remote) {
	if (tt_recording_notebook_for_log) {
		return(FALSE); // new on 250503
	};
	if (identifier-first_identifier < 0 || // moved up here from inside the TT_DEBUG_ON on 090699 for robustness
       identifier-first_identifier >= length) {
      tt_error_file() << "Bad remote identifier " << (int) identifier << endl;
		tt_err_file_important = TRUE; // new on 290803
      return(FALSE);
   };
	if (identifier == AVENUE_REMOTE || identifier == STREET_REMOTE) {
		// new on 100603 since there is no single true value for these remotes accross the entire city
		return(TRUE);
	};
#if TT_DEBUG_ON
//   tt_error_file() << "Adding " << remote << " " << tt_frame_number << endl;   
	if (tt_debug_mode == 16299 && identifier == RANDOM_REMOTE) {
		tt_error_file() << "Adding remote ";
		remote->print(tt_error_file());
		tt_error_file() << " of type " << (int) identifier << " for " << (long) this <<endl;
	};
#endif;
	// order doesn't matter but duplicates do
	// experiment on 141003 to comment this out
//#if TT_ALPHA2
//	if (insert_at_end_if_not_member(remote,remotes[identifier-first_identifier])) {
//		remote->increment_ref_count(); // new on 131003
//	};
//#else
	insert_at_end_if_not_member(remote,remotes[identifier-first_identifier]);
//#endif
//	remotes[identifier-first_identifier] = new Sprites(remote,remotes[identifier-first_identifier]);
	return(TRUE);
};

void Remote::remove_remote(RemoteIdentifier identifier, Sprite *remote) {
#if TT_DEBUG_ON
   if (identifier-first_identifier < 0 ||
       identifier-first_identifier >= length) {
      tt_error_file() << "Bad identifier " << (int) identifier << endl;
      return;
   };
	if (tt_debug_mode == 16299 && identifier == RANDOM_REMOTE) {
		tt_error_file() << "Removing remote ";
		remote->print(tt_error_file());
		tt_error_file() << " of type " << (int) identifier << " for " << (long) this << endl;
	};
#endif;
	int index = identifier-first_identifier;
	if (remotes[index] != NULL) {
		// following doesn't always find remote since is sometimes called just to be safe
	// experiment on 141003 to comment this out
//#if TT_ALPHA2
//		boolean found = FALSE;
//		remotes[index] = remotes[index]->remove(remote,&found);
//		if (found) { // new on 131003
//			remote->destroy(); // or decrement ref count
//		};
//#else
		remotes[index] = remotes[index]->remove(remote);
//#endif
	};
};

boolean Remote::collision_status_wanted() {
	// could be more clever so that when non-null check if visible
	// or "looked at" by a robot (or if tt_stop_all is TRUE)
   if (controlee->infinite_stack()) return(FALSE); // e.g. in a notebook
	if (controlee->contained_in(PROGRAM_PAD) || controlee->contained_in(CUBBY)) { 
		// new on 110100 (Jakob reported that collision detectors were being set off when inactive)
		return(FALSE);
	};
	return(remotes_for(HIT_OR_MISS_REMOTE) != NULL ||
			 remotes_for(X_HIT_OR_MISS_REMOTE) != NULL ||
			 remotes_for(Y_HIT_OR_MISS_REMOTE) != NULL ||
			 remotes_for(COLLIDEE_REMOTE) != NULL);
//			 remotes[X_PENETRATION_REMOTE] != NULL ||
//			 remotes[Y_PENETRATION_REMOTE] != NULL
};

// should be changed to be call for every current collision?
// or something else since not all of these make sense?

void Remote::check_collisions(boolean new_collidee) {
   // is it possible to optimize this for when neither party has moved (i.e. are clean)
#if TT_DEBUG_ON
	if (controlee->debug_counter == tt_debug_target) {
		log("Checking for collisions with debug target.");
	};
#endif
	TTRegion region;
	Sprite *previous_collidee = NULL; 
	// collision_region not always appropriate? -- e.g. walking guy
	controlee->full_collision_region(region);
	if (collision != NULL) {
		previous_collidee = collision->collidee;
		TTRegion old_collidee_region;
		previous_collidee->true_region(old_collidee_region); // was collision_region
		if (region.overlaps(old_collidee_region) && !previous_collidee->slated_for_deletion()) {
			// already know about it and its still colliding
			// here is where I could check for next to collide
//			if (tt_log_version_number <= 8) return; 
		} else { 
			no_longer_colliding();
			delete collision;
			collision = NULL;
			previous_collidee = NULL;
		};
	};
	Sprite *collidee;
	if (new_collidee) {
		collidee = controlee->next_to_collide(region,previous_collidee);
	} else if (previous_collidee != NULL && !previous_collidee->prepared_for_deletion()) { // new on May 5, '98  - second condition added 191200
		TTRegion previous_collidee_region;
		previous_collidee->full_collision_region(previous_collidee_region);
		if (previous_collidee_region.overlaps(region)) {
			collidee = previous_collidee; // still colliding
		} else {
			collidee = controlee->next_to_collide(region);
		};
	} else {
		collidee = controlee->next_to_collide(region);
	};
	if (collidee != NULL) {
		CollisionDirection direction = collision_direction(collidee);
		TTRegion *region_for_record = new TTRegion();
		(*region_for_record) = region;
      if (collision != NULL) delete collision;
		if (collidee->prepared_for_deletion()) { // new on 191200
			collision = NULL; 
			return; // must have been colliding with multiple things??
		};
		collision = new CollisionRecord(collidee,
												  region_for_record,
//												  controlee->previous_llx(),
//												  controlee->previous_lly(),
												  direction,
												  tt_frame_number);
#if TT_DEBUG_ON
		if (tt_debug_mode == 45) { 
			tt_error_file() << "Collision of #" << controlee->debug_counter << " detected on frame " << tt_frame_number
								 << " with " << type_name(collidee) << "#" << collidee->debug_counter
								 << " in direction " << direction
								 << " at " << controlee->current_llx() << "," << controlee->current_lly()
								 << " and its at " << collidee->current_llx() << "," << collidee->current_lly()
								 << endl;
         TTRegion temp;
         region.print(tt_error_file());
         tt_error_file() << endl;
         collidee->full_collision_region(temp);
         temp.print(tt_error_file());
         tt_error_file() << endl;
		};
#endif
		boolean not_ew_collision = (direction != EAST_COLLISION && direction != WEST_COLLISION);
		boolean not_ns_collision = (direction != NORTH_COLLISION && direction != SOUTH_COLLISION);
		changed(X_HIT_OR_MISS_REMOTE, compute_picture_id(HIT_OR_MISS_SPRITE, (short) not_ew_collision));
		changed(Y_HIT_OR_MISS_REMOTE, compute_picture_id(HIT_OR_MISS_SPRITE, (short) not_ns_collision));
		changed(HIT_OR_MISS_REMOTE, compute_picture_id(HIT_OR_MISS_SPRITE,!NOT_COLLIDING));
		if (collidee->kind_of() == PICTURE) { // commented out on 160101 and restored on 170101 (fixing this so can hit non-picture is tricky)
			Picture *picture_collidee = (Picture *) collidee;
			if (!picture_collidee->is_flipped() && picture_collidee->remote_status() == NOT_REMOTE) { // added not flipped check on 130201
//				collidee != previous_collidee) {
				// for now just deal with Pictures
//				changed(COLLIDEE_REMOTE,((Picture *) collidee)->picture_id());
				changed_appearance(COLLIDEE_REMOTE,collidee); // copied for each remote
			};
		};
	};
};

void Remote::no_longer_colliding() {
	//changed(COLLIDEE_REMOTE,-1);
	changed_appearance(COLLIDEE_REMOTE,NULL);
	short int id = compute_picture_id(HIT_OR_MISS_SPRITE,NOT_COLLIDING);
	changed(HIT_OR_MISS_REMOTE,id);
	if (collision->direction == SOUTH_COLLISION ||
		 collision->direction == NORTH_COLLISION) {
		changed(Y_HIT_OR_MISS_REMOTE,id);
	} else {
		changed(X_HIT_OR_MISS_REMOTE,id);
	};
};

/*
void Remote::colliding_with(Sprite *next_collidee) {
	boolean new_collidee = (collidee != next_collidee);
	collidee = next_collidee;
	if (new_collidee) {
		changed(HIT_OR_MISS_REMOTE,
				  compute_picture_id(HIT_OR_MISS_SPRITE,(short) (collidee == NULL)));
	};
	if (collidee != NULL) {
		if (last_collision_direction == NO_COLLISION || new_collidee || speed_changed()) {
			// don't already know so capture what it is/was when collison was
			// first noticed
			last_collision_direction = collision_direction(collidee);
		};
#if TT_DEBUG_ON
		if (tt_debug_mode == 54) {
			tt_error_file() << "Direction = " << last_collision_direction
							<< " frame number = " << tt_frame_number << endl;
		};
#endif
	} else {
		last_collision_direction = NO_COLLISION;
	};
	// redo this sometime to be a single sensor showing which direction??
	boolean not_ew_collision =
		(last_collision_direction != EAST_COLLISION &&
		 last_collision_direction != WEST_COLLISION);
	boolean not_ns_collision =
		(last_collision_direction != NORTH_COLLISION &&
		 last_collision_direction != SOUTH_COLLISION);
	changed(X_HIT_OR_MISS_REMOTE,
			  compute_picture_id(HIT_OR_MISS_SPRITE, (short) not_ew_collision));
	changed(Y_HIT_OR_MISS_REMOTE,
			  compute_picture_id(HIT_OR_MISS_SPRITE, (short) not_ns_collision));
//	int old_new_x_collidees_count = new_x_collidees_count;
//	int old_new_y_collidees_count = new_y_collidees_count;
//	new_x_collidees_count = (new_collidee && collidee != NULL && not_ns_collision);
//	new_y_collidees_count = (new_collidee && collidee != NULL && not_ew_collision);
	if (collidee == NULL) {
		changed(COLLIDEE_REMOTE,-1);
//		changed(X_PENETRATION_REMOTE,0);
//		changed(Y_PENETRATION_REMOTE,0);
	} else {
		if (new_collidee && collidee->kind_of() == PICTURE && 
			 ((Picture *) collidee)->remote_status() == NOT_REMOTE) {
			// for now just deal with Pictures
			changed(COLLIDEE_REMOTE,((Picture *) collidee)->picture_id());
		};
//		controlee->compute_penetration(collidee,last_collision_direction,
//												 x_penetration,y_penetration);
//		changed(X_PENETRATION_REMOTE,x_penetration);
//		changed(Y_PENETRATION_REMOTE,y_penetration);
  // use the following when the full-blown version of this is implemented
//		if (collidees == NULL || 
//			 tt_frame_number&1 != current_collidees_frame_number_bit) {
			// new frame or no current collidees
	}; 
//	if (old_new_x_collidees_count != new_x_collidees_count) {
//		changed(X_PENETRATION_REMOTE,new_x_collidees_count);
//	};
//	if (old_new_y_collidees_count != new_y_collidees_count) {
//		changed(Y_PENETRATION_REMOTE,new_y_collidees_count);
//	};
};
*/

CollisionDirection Remote::collision_direction(Sprite *collidee) {
	TTRegion controlee_region;
	controlee->true_region(controlee_region);
	TTRegion collidee_region;
	collidee->true_region(collidee_region); // was collision_region
	city_coordinate controlee_delta_x = controlee->current_llx()-controlee->previous_llx();
	city_coordinate controlee_delta_y = controlee->current_lly()-controlee->previous_lly();
	city_coordinate collidee_delta_x = collidee->current_llx()-collidee->previous_llx();
	city_coordinate collidee_delta_y = collidee->current_lly()-collidee->previous_lly();
	CollisionDirection direction;
	if (labs(controlee_delta_x-collidee_delta_x) > labs(controlee_delta_y-collidee_delta_y)) {
		// in case it is colliding both left/right and up/down
		// bias it towards the one the objects are moving
		direction  = left_right_collision(controlee_region,collidee_region,controlee_delta_x,collidee_delta_x);
		if (direction == NO_COLLISION) {
			direction  = up_down_collision(controlee_region,collidee_region,controlee_delta_y,collidee_delta_y);
		};
		if (direction == NO_COLLISION && controlee_delta_x != 0 && tt_log_version_number >= 50) { // new on 290704
			if (controlee_delta_x > 0) {
				direction = EAST_COLLISION;
			} else {
				direction = WEST_COLLISION;
			};
		};
	} else {
		direction  = up_down_collision(controlee_region,collidee_region,controlee_delta_y,collidee_delta_y);
		if (direction == NO_COLLISION) {
			direction  = left_right_collision(controlee_region,collidee_region,controlee_delta_x,collidee_delta_y);
		};
		if (direction == NO_COLLISION && controlee_delta_y != 0 && tt_log_version_number >= 50) { // new on 290704
			if (controlee_delta_y > 0) {
				direction = NORTH_COLLISION;
			} else {
				direction = SOUTH_COLLISION;
			};
		};
	};
	if (direction != NO_COLLISION) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 45) {
			tt_error_file() << "Computed direction " << direction 
							<< " on frame " << tt_frame_number << endl;
	   };
#endif
		return(direction);	 
	};
	// don't know how we got here and not moving
	// so assume that the smallest penetration is the answer
	city_coordinate new_x_penetration,new_y_penetration;
	return(controlee->best_penetration(collidee,new_x_penetration,new_y_penetration));
};

/*
	// probably just started moving if x_speed or y_speed is non-zero
	// so use them to compute direction
	if (labs(controlee_delta_x) > labs(controlee_delta_y)) {
		if (controlee_delta_x > 0) {
			return(EAST_COLLISION);
		} else {
			return(WEST_COLLISION);
		};
	} else if (controlee_delta_y > 0) {
		return(NORTH_COLLISION);
	} else if (controlee_delta_y < 0) {
		return(SOUTH_COLLISION);
	};
*/

CollisionDirection Remote::left_right_collision(TTRegion &controlee_region,
																TTRegion &collidee_region,
																city_coordinate controlee_delta,
																city_coordinate collidee_delta) {
	if (collidee_region.max_x-collidee_delta < controlee_region.min_x-controlee_delta &&
		 collidee_region.max_x >= controlee_region.min_x) {
		 // collidee was completely to the left of picture and is no longer
		 return(WEST_COLLISION); 
	};
	if (collidee_region.min_x-collidee_delta > controlee_region.max_x-controlee_delta &&
		 collidee_region.min_x <= controlee_region.max_x) {
		// was to the right
		return(EAST_COLLISION);
	};
	return(NO_COLLISION);
};

CollisionDirection Remote::up_down_collision(TTRegion &controlee_region,
															TTRegion &collidee_region,
															city_coordinate controlee_delta,
															city_coordinate collidee_delta) {
	if (collidee_region.max_y-collidee_delta < controlee_region.min_y-controlee_delta &&
		 collidee_region.max_y >= controlee_region.min_y) {
		// collidee was underneath and isn't now
		return(SOUTH_COLLISION);
	};
	if (collidee_region.min_y-collidee_delta > controlee_region.max_y-controlee_delta &&
		 collidee_region.min_y <= controlee_region.max_y) {
		return(NORTH_COLLISION);
	};
	return(NO_COLLISION);
};

//Sprite *last_collidee = NULL;

//boolean not_last_collidee_callback(void *s) {
//   return(s != last_collidee);
//};

void Remote::uncollide(user_coordinate value) {
	if (!controlee->ultimate_leader()->like_a_picture()) return; // generalized on 141299
// problem with the following is that the original may have
// been computed using a region widened by movement and this
// will miss it and allow "burrowing"
// but in general that collision record may be too old
// and this is needed in case no longer colliding...
	boolean collision_record_up_to_date;
//	if (tt_log_version_number >= 50) { // commented out on 170804 since allowed some stale collision records to remain
//		collision_record_up_to_date = (collision != NULL && collision->frame_number+1 >= tt_frame_number); // makes more sense - new on 290704
//	} else {
		collision_record_up_to_date = (collision != NULL && collision->frame_number+1 == tt_frame_number);
//	};
	if (!collision_record_up_to_date) { // collision record missing or old
		check_collisions();
		if (collision == NULL) return; // no longer colliding
	};
	SpriteCode picture_code;
	short int picture_index;
	decode_picture_id((short int) value,picture_index,picture_code);
	if (picture_index == NOT_COLLIDING && !controlee->is_flipped()) {
		city_coordinate x_penetration,y_penetration;
// now I do the one I know about and then deal with remaining collisions
//		if (next == NULL) { // only one
			controlee->compute_penetration(collision->collidee,
													 collision->direction,
													 x_penetration,y_penetration);
			if (collision->direction == WEST_COLLISION || collision->direction == SOUTH_COLLISION) { // new on 230503
				if (x_penetration >= 0 && y_penetration >= 0) {
					return; 
				};
			} else if (x_penetration <= 0 && y_penetration <= 0) {
				return;
			};
//		city_coordinate delta_x = collision->llx_before-controlee->current_llx();
//		city_coordinate delta_y = collision->lly_before-controlee->current_lly();
//		if (labs(x_penetration) < labs(delta_x) &&
//			 labs(y_penetration) < labs(delta_y)) {
//			delta_x = -x_penetration;
//			delta_y = -y_penetration;
//		};
			city_coordinate delta_x = (x_penetration*102)/-100; // add 2% for round off -- new on 131200
			if (delta_x < 0) { // new on 180901 to increase by a pixel here (previously :compute_penetration did some of the work)
				delta_x -= tt_screen->one_pixel_in_x_city_units();
			};
			city_coordinate delta_y = (y_penetration*102)/-100; // add 2% for round off
			if (delta_y < 0) { // new on 180901 to increase by a pixel here (previously :compute_penetration did some of the work)
				delta_y -= tt_screen->one_pixel_in_y_city_units();
			};
			controlee->move_by(delta_x,delta_y);
			// this backing out should not be used for computing movement
			// direction on the next frame
         // +1 so that a change is noticed
			if (x_penetration != 0) {
				controlee->set_previous_llx(controlee->previous_llx()-x_penetration+(x_penetration>0?1:-1));
			};
			if (y_penetration != 0) {
				controlee->set_previous_lly(controlee->previous_lly()-y_penetration+(y_penetration>0?1:-1));
			};
#if TT_DEBUG_ON
			if (tt_debug_mode == 45 || tt_debug_mode == 190503) {
				collision->collidee->print(tt_error_file());
				tt_error_file() << " frame " << tt_frame_number
								<< " direction is " << collision->direction
								<< " and penetration is "
								<< x_penetration << "," << y_penetration
								<< " and delta is " << delta_x << "," << delta_y
								<< " and speed is ";
				if (controlee->current_x_speed() == NULL) {
					tt_error_file() << "0";
				} else {
					controlee->current_x_speed()->print(tt_error_file());
				};
				tt_error_file() << ",";
				if (controlee->current_y_speed() == NULL) {
					tt_error_file() << "0";
				} else {
					controlee->current_y_speed()->print(tt_error_file());
				};
				tt_error_file() << endl;
				tt_error_file() << "New location is " << controlee->current_llx()
								<< "," << controlee->current_lly() << endl;
			};
#endif
//		} else { // go thru them all
		TTRegion region;
		if (collision_record_up_to_date) {
			region = *(collision->region);
		} else {
			controlee->full_region(region); // does not include movement...
		};
		Sprite *next = controlee->next_to_collide(region,collision->collidee,NULL,NULL,FALSE);
		// added extra args to next_to_collide on 290105 since don't want it to loop around in searching
		if (next != NULL) {
			next = NULL;
#if TT_DEBUG_ON
			if (tt_debug_mode == 45) {
				tt_error_file() << "Multiple collisions " << tt_frame_number << endl;
			};
#endif
			city_coordinate delta_x = 0, delta_y = 0;
			while ((next = controlee->next_to_collide(region,next,NULL,NULL,FALSE)) != NULL && next != collision->collidee) { 
				// and not the one already done
				// prior to 190503 the second condition was in a nested conditional but that can cause it to loop here
				// added extra args to next_to_collide on 290105 since don't want it to loop around in searching
				controlee->best_penetration(next,x_penetration,y_penetration);
//				controlee->move_by(-x_penetration,-y_penetration);
				if (labs(x_penetration) > labs(delta_x)) {
					delta_x = (x_penetration*102)/-100; // add 2% for round off (new on 131200)
				};
				if (labs(y_penetration) > labs(delta_y)) {
					delta_y = (y_penetration*102)/-100; // add 2% for round off (new on 131200)
				};
#if TT_DEBUG_ON
				if (tt_debug_mode == 45) {
					tt_error_file() << "And " << type_name(next)
										 << " and best penetration is "
										 << x_penetration << "," << y_penetration << endl;
				};
#endif
         };
			controlee->move_by(delta_x,delta_y);
			// this backing out should not be used for computing movement
			// direction on the next frame 
			// +1 so that a change is noticed
			if (x_penetration != 0) {
				controlee->set_previous_llx(controlee->previous_llx()+delta_x+(delta_x>0?-1:1));
			};
			if (y_penetration != 0) {
				controlee->set_previous_lly(controlee->previous_lly()-delta_y+(delta_y>0?-1:1));
			};
#if TT_DEBUG_ON
			if (tt_debug_mode == 45) {
				tt_error_file() << "Joint max deltas are " << delta_x << "," << delta_y << endl;
				tt_error_file() << "New location is " << controlee->current_llx()
									 << "," << controlee->current_lly() << endl;
			};
#endif
		};
//		delete collision;
//		collision = NULL;
	};
};

CollisionRecord::CollisionRecord(Sprite *collidee,
//											city_coordinate llx, city_coordinate lly,
											TTRegion *region, 
											CollisionDirection direction,
											millisecond frame_number) :
		  collidee(collidee),
//		  llx_before(llx),
//		  lly_before(lly),
		  region(region),
		  direction(direction),
		  frame_number(frame_number) {
	if (collidee != NULL) {
		collidee->increment_ref_count(); // new on 140200
	};
};

CollisionRecord::~CollisionRecord() {
	if (collidee != NULL && tt_reclaiming_memory) { 
		// was !about_to_quit() prior to 180103 // caused problems when tt_shutting_down - updated 150101 to use about_to_quit
		collidee->destroy(); // new on 140200 (typically decrement ref count)
	};
	delete region;
};

#if !TT_32
// "removed" on 060201
void initialize_lego_state(int input_port_count, int ) {
	tt_lego_ports = input_port_count;
	if (input_port_count <= 0) return;
	motor_state_changed = new boolean[tt_lego_ports];
	current_power_levels = new int[tt_lego_ports];
	sensor_count_request_sent = new boolean[tt_lego_ports];
	lego_sensor_count = new unsigned long[tt_lego_ports];
	lego_sensor_count_delta = new unsigned short[tt_lego_ports];  
	sensor_count_needs_reading = new boolean[tt_lego_ports];  
	for (int i = 0; i < tt_lego_ports; i++) {
		motor_state_changed[i] = FALSE;
		current_power_levels[i] = 0;
		sensor_count_request_sent[i] = FALSE;
		lego_sensor_count[i] = 0;
		lego_sensor_count_delta[i] = 0;
	};
};

void free_lego_state() {
	if (tt_lego_ports <= 0) return;
	delete [] motor_state_changed;
	delete [] current_power_levels;
	delete [] sensor_count_request_sent;
	delete [] lego_sensor_count;
	delete [] lego_sensor_count_delta;
	delete [] sensor_count_needs_reading;
};

user_coordinate set_motor_value(int outlet, user_coordinate new_power_level) {
	int max_power_level;
	if (tt_lego_in_use == CONTROL_LAB_LEGO_CONTROLLER) {
	  max_power_level = 8;
	} else {
	  max_power_level = 12;
	};
	if (new_power_level > max_power_level) {
		new_power_level = max_power_level;
	} else if (new_power_level < -max_power_level) {
		new_power_level = -max_power_level;
	};
	motor_state_changed[outlet]
		= (current_power_levels[outlet] != new_power_level);
	if (!motor_state_changed[outlet]) return(new_power_level); // no change
	if (new_power_level > 0) {
		if (tt_lego_in_use == CONTROL_LAB_LEGO_CONTROLLER) {
			add_to_serial_port_buffer(175+new_power_level);
			add_to_serial_port_buffer(1<<outlet);
		} else {
			add_to_serial_port_buffer(129+outlet);
			add_to_serial_port_buffer(new_power_level);
			add_to_serial_port_buffer(146+outlet*3);
		};
	} else if (new_power_level < 0) {
		if (tt_lego_in_use == CONTROL_LAB_LEGO_CONTROLLER) {
			add_to_serial_port_buffer(175+new_power_level);
			add_to_serial_port_buffer(1<<outlet);
		} else {
			add_to_serial_port_buffer(129+outlet);
			add_to_serial_port_buffer(-new_power_level);
			add_to_serial_port_buffer(147+outlet*3);
		};
	} else { // must be zero
		if (tt_lego_in_use == CONTROL_LAB_LEGO_CONTROLLER) {
			add_to_serial_port_buffer(56+outlet); // turn off
		} else {
			add_to_serial_port_buffer(148+outlet*3);
		};
	};
	if (tt_lego_in_use == CONTROL_LAB_LEGO_CONTROLLER &&
		 ((new_power_level > 0 && current_power_levels[outlet] < 0) ||
		  (new_power_level < 0 && current_power_levels[outlet] > 0))) {
		add_to_serial_port_buffer(32+outlet); // reverse direction
	};
	if (current_power_levels[outlet] == 0) { // was off
		add_to_serial_port_buffer(40+outlet); // turn on
	};
	current_power_levels[outlet] = new_power_level;
	return(new_power_level);
};
#endif

#if TT_LEGO
// conditionalized on 220802
#if TT_32
user_coordinate handle_lego_sensors(RemoteIdentifier ) {
   return(0);
};
#else
user_coordinate handle_lego_sensors(RemoteIdentifier identifier) {
	switch (identifier) {
      case LEGO_SENSOR_COUNT1_REMOTE:
			return(lego_sensor_count && lego_sensor_count[0]);
		case LEGO_SENSOR_COUNT2_REMOTE:
			return(lego_sensor_count && lego_sensor_count[1]);
		case LEGO_SENSOR_COUNT3_REMOTE:
			return(lego_sensor_count && lego_sensor_count[2]);
		case LEGO_SENSOR_COUNT4_REMOTE:
			return(lego_sensor_count && lego_sensor_count[3]);
		case LEGO_SENSOR_COUNT5_REMOTE:
			return(lego_sensor_count && lego_sensor_count[4]);
		case LEGO_SENSOR_COUNT6_REMOTE:
			return(lego_sensor_count && lego_sensor_count[5]);
		case LEGO_SENSOR_COUNT7_REMOTE:
			return(lego_sensor_count && lego_sensor_count[6]);
		case LEGO_SENSOR_COUNT8_REMOTE:
			return(lego_sensor_count && lego_sensor_count[7]);
		case LEGO_MOTOR1_REMOTE:
			return(current_power_levels && current_power_levels[0]);
		case LEGO_MOTOR2_REMOTE:
			return(current_power_levels && current_power_levels[1]);
		case LEGO_MOTOR3_REMOTE:
			return(current_power_levels && current_power_levels[2]);
		case LEGO_MOTOR4_REMOTE:
			return(current_power_levels && current_power_levels[3]);
		case LEGO_MOTOR5_REMOTE:
			return(current_power_levels && current_power_levels[4]);
		case LEGO_MOTOR6_REMOTE:
			return(current_power_levels && current_power_levels[5]);
		case LEGO_MOTOR7_REMOTE:
			return(current_power_levels && current_power_levels[6]);
		case LEGO_MOTOR8_REMOTE:
			return(current_power_levels && current_power_levels[7]);
		
   };
   return(0);
};

user_coordinate handle_lego_motors(RemoteIdentifier identifier, user_coordinate new_value) {
   switch (identifier) {
		case LEGO_MOTOR1_REMOTE:
			return(set_motor_value(0,new_value));
		case LEGO_MOTOR2_REMOTE:
			return(set_motor_value(1,new_value));
		case LEGO_MOTOR3_REMOTE:
			return(set_motor_value(2,new_value));
		case LEGO_MOTOR4_REMOTE:
			return(set_motor_value(3,new_value));
		case LEGO_MOTOR5_REMOTE:
			return(set_motor_value(4,new_value));
		case LEGO_MOTOR6_REMOTE:
			return(set_motor_value(5,new_value));
		case LEGO_MOTOR7_REMOTE:
			return(set_motor_value(6,new_value));
		case LEGO_MOTOR8_REMOTE:
			return(set_motor_value(7,new_value));
		case LEGO_SENSOR_COUNT1_REMOTE:
			return(lego_sensor_count[0] = new_value);
		case LEGO_SENSOR_COUNT2_REMOTE:
			return(lego_sensor_count[1] = new_value);
		case LEGO_SENSOR_COUNT3_REMOTE:
			return(lego_sensor_count[2] = new_value);
		case LEGO_SENSOR_COUNT4_REMOTE:
			return(lego_sensor_count[3] = new_value);
		case LEGO_SENSOR_COUNT5_REMOTE:
			return(lego_sensor_count[4] = new_value);
		case LEGO_SENSOR_COUNT6_REMOTE:
			return(lego_sensor_count[5] = new_value);
		case LEGO_SENSOR_COUNT7_REMOTE:
			return(lego_sensor_count[6] = new_value);
		case LEGO_SENSOR_COUNT8_REMOTE:
			return(lego_sensor_count[7] = new_value);
   };
   return(new_value); // should it happen?
};
#endif
#endif

//void Remote::changed_floor(Background *controlees_floor) {
//	// control panel may be in a different floor than sprite
//	for (int i = first_identifier; i <= last_identifier; i++) {
//		if (i == APPEARANCE_REMOTE) break; // ok??
//		 Sprites *remaining = remotes[i];
//		 while (remaining != NULL) {
//			 Remote_Integer *remote = (Remote_Integer *) remaining->first();
//			 if (!remote->inside_sprite()) {
//				 remote->changed_floor(controlees_floor);
//			 };
//			 remaining = remaining->rest();
//		 };
//	};
//};

