// Copyright (c) 1992-2000.  Ken Kahn, Animated Programs, All rights reserved
#ifndef __TT_INPUT_H

#define __TT_INPUT_H
#if TT_DIRECT_INPUT

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif

class ForceRemoteControlee;

extern DIJOYSTATE joystick_state[];
extern unsigned long joystick_button_still_down_masks[];
//extern unsigned long joystick_button_down_reported[];

inline long joystick_x(int number) {
   return(joystick_state[number].lX);
};
inline long joystick_y(int number) {
   return(joystick_state[number].lY);
};
inline long joystick_z(int number) {
   return(joystick_state[number].lZ);
};
inline long joystick_axis_x(int number) {
   return(joystick_state[number].lRx);
};
inline long joystick_axis_y(int number) {
   return(joystick_state[number].lRy);
};
inline long joystick_axis_z(int number) {
   return(joystick_state[number].lRz);
};
#if TT_DEBUG_ON
boolean joystick_button(int joystick_number, int button_number);
boolean joystick_button_still_down(int joystick_number, int joystick_button_number);
#else
inline boolean joystick_button(int joystick_number, int button_number) {
   return((boolean) (joystick_state[joystick_number].rgbButtons[button_number] & 0x80));
};
inline boolean joystick_button_still_down(int joystick_number, int joystick_button_number) {
   return((boolean) (joystick_button_still_down_masks[joystick_number]&(1<<joystick_button_number)));
};
#endif

//#if TT_MICROSOFT
boolean initialize_direct_input(HINSTANCE instance);
//#else
//boolean initialize_direct_input(HANDLE instance);
//#endif
void release_direct_input();

boolean read_joystick_state(int index, DIJOYSTATE &state);
void update_joystick_button_still_down();
void direct_input_mouse_delta(long &delta_x, long &delta_y);
void direct_input_read_joysticks(long &delta_x, long &delta_y);
int first_new_joystick_button_down(int joystick_number);
int first_joystick_button_down(int joystick_number);

void unacquire_input_devices();
void acquire_input_devices();

LPDIRECTINPUTEFFECT *create_force_feedback_effects(ascii_string name, int joystick_index, int &count);
void release_force_feedback_effects(int joystick_index, LPDIRECTINPUTEFFECT *effects);
void set_background_force_feedback_effect(ascii_string name);
void play_force_feedback_effects(LPDIRECTINPUTEFFECT *effects, int count);
void play_force_feedback_effect(ascii_string name, boolean remember_this=TRUE, int joystick_index=0);
void stop_force_feedback_effect();
void stop_force_feedback_effects(LPDIRECTINPUTEFFECT *effects, int count);
void set_joystick_sensitivity(unsigned short int percent);
ascii_string *return_force_names();
ascii_string force_name(int index);
int return_force_count();
int force_index(ascii_string name);

long force_feedback_duration(LPDIRECTINPUTEFFECT *effects);
long force_feedback_intensity(LPDIRECTINPUTEFFECT *effects);
//long force_feedback_trigger_button(LPDIRECTINPUTEFFECT *effects);
long force_feedback_direction(LPDIRECTINPUTEFFECT *effects);
long set_force_feedback_duration(LPDIRECTINPUTEFFECT *effects, int count, long new_value);
long set_force_feedback_intensity(LPDIRECTINPUTEFFECT *effects, int count, long new_value);
//long set_force_feedback_trigger_button(LPDIRECTINPUTEFFECT *effects, int count, long new_value);
long set_force_feedback_direction(LPDIRECTINPUTEFFECT *effects, int count, long new_value);

void release_mouse_capture();
void capture_mouse();

boolean play_touch_sensitive_mouse_effect(ascii_string name);
boolean get_touch_sensitive_mouse_effect_direction(ascii_string name, long &value);
boolean get_touch_sensitive_mouse_effect_duration(ascii_string name, DWORD &value);
boolean get_touch_sensitive_mouse_effect_intensity(ascii_string name, DWORD &value);
boolean modify_and_play_touch_sensitive_mouse_effect(ascii_string name, int intensity, int direction, int duration);
boolean stop_touch_sensitive_mouse_effect();

ascii_string return_background_force_feedback_effect();

#endif
#endif
