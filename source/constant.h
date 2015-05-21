// Copyright (c) 1992-2006. Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_CONSTANT_H)

#define __TT_CONSTANT_H

// constants available for ToonTalk

//not using clipping just now
//enum clip_status {LINE_INSIDE, LINE_OUTSIDE,
//                BOTH_ENDS_CLIPPED, START_CLIPPED, END_CLIPPED};

enum extended_keyboard
	{F1=59,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12, // last two non-standard??
	 UPARROW=72, LEFTARROW=75, DOWNARROW=80, RIGHTARROW=77,
	 HOME=71, PAGEUP=73, PAGEDOWN=81, END=79,
	 INSERT=82, DELETE_KEY=83}; // was DELETE

enum standard_keyboard {BACKSPACE=8, TAB=9, RETURN=13, // OS specific?
								ESCAPE=27,
								SPACE=32,
								SINGLEQUOTE=39,
								PLUS=43, MINUS=45,
								COMMA=44,
								BACKSLASH=92,
								BACKQUOTE=96,
								CONTROL_BS=127};

#if !TT_STARTTT
// first 16 VGA colors
// color zero is clear when used in images
#if TT_WINDOWS
#include <windows.h>
//#include "rc.h"
#include "images.h"
#include "sounds.h"
#endif
#if TT_WINDOWS 
// colors that follow are based upon colors supported by OWL
//#if !GRAPHICS_TO_MEMORY // otherwise colors are computed in winmain
//const COLORREF CLEAR = RGB(0,0,0);
//const COLORREF BLUE = RGB(0,0,127);
//const COLORREF GREEN = RGB(0,127,0);
//const COLORREF CYAN = RGB(0,127,127);
//const COLORREF RED = RGB(127,0,0);
//const COLORREF MAGENTA = RGB(127,0,127);
//const COLORREF BROWN = RGB(127,127,0);
const COLORREF BLACK = RGB(0,0,0);
const COLORREF LIGHT_GRAY = RGB(192,192,192);
const COLORREF GRAY = RGB(128,128,128);
const COLORREF WHITE = RGB(255,255,255); 
const COLORREF LIGHT_BLUE = RGB(0,0,255);
const COLORREF LIGHT_GREEN = RGB(0,255,0);
const COLORREF LIGHT_CYAN = RGB(0,255,255);
const COLORREF LIGHT_RED = RGB(255,0,0);
const COLORREF LIGHT_MAGENTA = RGB(255,0,255);
const COLORREF LIGHT_YELLOW = RGB(255,255,0); 

#endif

#endif
//#else
//enum colors {CLEAR,BLUE,GREEN,CYAN,RED,MAGENTA,BROWN,WHITE,GRAY,
//				 LIGHT_BLUE,LIGHT_GREEN,LIGHT_CYAN,LIGHT_RED,LIGHT_MAGENTA,
//				 YELLOW,BRIGHT_WHITE,BLACK=254};
//#endif

const int colors_count = 256; // was just the 10 above

enum CameraStatus {CAMERA_ABOVE,  // flying above city
                   CAMERA_IN_FRONT,  // walking around or landing
                   CAMERA_MOSTLY_ABOVE};  // on floor or ground or thought bubble

enum MatchStatus {MATCH_BAD,MATCH_GOOD,
						MATCH_SUSPENDED_ON_NEST,MATCH_SUSPENDED_ON_EMPTY_CUBBY_HOLE,
                  MATCH_SUSPENDED_ON_TRUCK};

//enum ActionStatus {ACTION_IN_PROGRESS,ACTION_COMPLETED,
//                   ACTION_ABORTED,ACTION_NOT_STARTED};
typedef long ActionStatus; // can handle conjunctive actions

enum AudioSelection {MUSIC_BIT=1,SOUND_BIT=2};

enum Direction {EAST,SOUTHEAST,SOUTH,SOUTHWEST,
					 WEST,NORTHWEST,NORTH,NORTHEAST,
                NO_DIRECTION};

// text and number only differ in color; sound and force differ in color and are never animated (since aren't the value of a sensor)
// major rewrite on 030102 and again on 130202
enum PlateState {NUMBER_PAD_WITHOUT_PLATE, OPERATION_PAD_WITHOUT_PLATE,
                 TEXT_PAD_WITHOUT_PLATE, OPERATION_PAD_HORIZONTAL,
                 NUMBER_PAD_WITH_PLATE_ANIMATED_LEFT, NUMBER_PAD_WITH_PLATE_ANIMATED_RIGHT, NUMBER_PAD_WITH_PLATE_ANIMATED_TOP, NUMBER_PAD_WITH_PLATE_ANIMATED_BOTTOM,
                 TEXT_PAD_WITH_PLATE_ANIMATED_LEFT, TEXT_PAD_WITH_PLATE_ANIMATED_RIGHT, TEXT_PAD_WITH_PLATE_ANIMATED_TOP, TEXT_PAD_WITH_PLATE_ANIMATED_BOTTOM,
                 SOUND_PAD_WITHOUT_PLATE, OPERATION_PAD_VERTICAL,
					  NUMBER_PAD_HORIZONTAL, NUMBER_PAD_VERTICAL,
					  TEXT_PAD_HORIZONTAL, TEXT_PAD_VERTICAL,
					  SOUND_PAD_HORIZONTAL, SOUND_PAD_VERTICAL,
					  FORCE_PAD_WITHOUT_PLATE, FORCE_PAD_HORIZONTAL, FORCE_PAD_VERTICAL,
					   // FORCE_PAD_WITH_PLATE,
					  // the following are old states that are converted to the new scheme before being used (040102)
					  TEXT_PAD_WITH_PLATE_ANIMATED, NUMBER_PAD_WITH_PLATE_ANIMATED,
					  NUMBER_PAD_WITH_PLATE, TEXT_PAD_WITH_PLATE, SOUND_PAD_WITH_PLATE, FORCE_PAD_WITH_PLATE, OPERATION_PAD_WITH_PLATE
};

/*
enum PlateState {NUMBER_PAD_WITHOUT_PLATE,NUMBER_PAD_WITH_PLATE, // major rewrite on 030102
                 TEXT_PAD_WITHOUT_PLATE,TEXT_PAD_WITH_PLATE,
                 NUMBER_PAD_WITH_PLATE_ANIMATED_LEFT,NUMBER_PAD_WITH_PLATE_ANIMATED_RIGHT,NUMBER_PAD_WITH_PLATE_ANIMATED_TOP,NUMBER_PAD_WITH_PLATE_ANIMATED_BOTTOM,
                 TEXT_PAD_WITH_PLATE_ANIMATED_LEFT,TEXT_PAD_WITH_PLATE_ANIMATED_RIGHT,TEXT_PAD_WITH_PLATE_ANIMATED_TOP,TEXT_PAD_WITH_PLATE_ANIMATED_BOTTOM,
                 SOUND_PAD_WITHOUT_PLATE,SOUND_PAD_WITH_PLATE,
					  NUMBER_PAD_HORIZONTAL, NUMBER_PAD_VERTICAL,
					  TEXT_PAD_HORIZONTAL, TEXT_PAD_VERTICAL,
					  SOUND_PAD_HORIZONTAL, SOUND_PAD_VERTICAL,
					  FORCE_PAD_HORIZONTAL, FORCE_PAD_VERTICAL,
					  FORCE_PAD_WITHOUT_PLATE,FORCE_PAD_WITH_PLATE,
					  // the following are old states that are converted to the new scheme before being used (040102)
					  TEXT_PAD_WITH_PLATE_ANIMATED, NUMBER_PAD_WITH_PLATE_ANIMATED
};
enum PlateState {NUMBER_PAD_WITHOUT_PLATE,NUMBER_PAD_WITH_PLATE,
					  TEXT_PAD_WITHOUT_PLATE,TEXT_PAD_WITH_PLATE,
                 NUMBER_PAD_WITH_PLATE_ANIMATED,
                 TEXT_PAD_WITH_PLATE_ANIMATED,
                 SOUND_PAD_WITHOUT_PLATE,SOUND_PAD_WITH_PLATE,
					  // following new as of 171201
					  NUMBER_PAD_WITH_PLATE_LEFT,NUMBER_PAD_WITH_PLATE_MIDDLE,NUMBER_PAD_WITH_PLATE_RIGHT,
					  NUMBER_PAD_WITH_PLATE_ANIMATED_LEFT,NUMBER_PAD_WITH_PLATE_ANIMATED_MIDDLE,NUMBER_PAD_WITH_PLATE_ANIMATED_RIGHT,
					  TEXT_PAD_WITH_PLATE_LEFT,TEXT_PAD_WITH_PLATE_MIDDLE,TEXT_PAD_WITH_PLATE_RIGHT,
					  TEXT_PAD_WITH_PLATE_ANIMATED_LEFT,TEXT_PAD_WITH_PLATE_ANIMATED_MIDDLE,TEXT_PAD_WITH_PLATE_ANIMATED_RIGHT,
					  SOUND_PAD_WITH_PLATE_LEFT,SOUND_PAD_WITH_PLATE_MIDDLE,SOUND_PAD_WITH_PLATE_RIGHT,
					  FORCE_PAD_WITHOUT_PLATE,FORCE_PAD_WITH_PLATE,
					  FORCE_PAD_WITH_PLATE_LEFT,FORCE_PAD_WITH_PLATE_MIDDLE,FORCE_PAD_WITH_PLATE_RIGHT
}; */

enum RemoteIdentifier {X_REMOTE,Y_REMOTE,
							  X_SPEED_REMOTE,Y_SPEED_REMOTE,
							  WIDTH_REMOTE,HEIGHT_REMOTE,
							  PARAMETER_REMOTE,
							  HIT_OR_MISS_REMOTE,
							  COLLIDEE_REMOTE,
// moved to globals in going from 5 to 6
//							  MOUSE_SPEED_X_REMOTE,
//							  MOUSE_SPEED_Y_REMOTE,
//							  CLOCK_REMOTE,
//							  RANDOM_REMOTE,
							  END_OF_ANIMATION_REMOTE,
							  GRASPED_REMOTE,
                       SELECTED_REMOTE,
							  RELEASED_REMOTE,
							  X_HIT_OR_MISS_REMOTE,
							  Y_HIT_OR_MISS_REMOTE,
							  APPEARANCE_REMOTE,
							  VISIBLE_REMOTE,
// moved up in going from 5 to 6
//							  END_OF_ANIMATION_REMOTE,
//							  GRASPED_REMOTE,
//							  SELECTED_REMOTE,
//							  RELEASED_REMOTE,
                       CHILDREN_REMOTE, //UNUSED1_REMOTE,
                       ANCESTORS_REMOTE, //UNUSED2_REMOTE,
                       UNUSED3_REMOTE, // for growth of locals
                       UNUSED4_REMOTE,
							  // now for globals
							  MOUSE_LEFT_BUTTON_REMOTE,
							  MOUSE_MIDDLE_BUTTON_REMOTE,
							  MOUSE_RIGHT_BUTTON_REMOTE,
							  MOUSE_LEFT_BUTTON_DOWN_REMOTE,
							  MOUSE_MIDDLE_BUTTON_DOWN_REMOTE,
							  MOUSE_RIGHT_BUTTON_DOWN_REMOTE,
							  KEYBOARD_BUTTON_REMOTE,
							  LAST_KEYSTROKE_REMOTE,
							  SHIFT_DOWN_REMOTE,
							  CONTROL_DOWN_REMOTE,
							  HAND_VISIBLE_REMOTE,
							  CLIPBOARD_REMOTE,
							  AVENUE_REMOTE,
							  STREET_REMOTE,
							  SOUND_ON_REMOTE,
							  LETTER_SIZE_REMOTE,
							  READING_SPEED_REMOTE,
							  MARTIAN_ADVICE_LEVEL_REMOTE,
							  PROGRAMMER_HEAD_REMOTE,
							  SCREEN_SIZE_REMOTE,
							  SYSTEM_SPEED_REMOTE,
							  CITY_SIZE_REMOTE,
							  SYSTEM_SLEEP_REMOTE,
							  MOUSE_BUTTONS_REMOTE,
							  LEGO_SERIAL_PORT_REMOTE,
							  MARTIAN_FONT_REMOTE,
                       FILE_TO_PICTURE_REMOTE,
                       FILE_TO_SOUND_REMOTE, // uses MCI so doesn't matter whether MIDI or WAVE
                       FILE_TO_MOVIE_REMOTE,  // obsolete??
                       MARTIAN_SPEAK_REMOTE,
                       MOUSE_SPEED_X_REMOTE,  // moved here in switching from version 5 to 6
							  MOUSE_SPEED_Y_REMOTE,
                       CLOCK_REMOTE,
							  RANDOM_REMOTE,
                       TEXT_TO_MCI_REMOTE,
                       TEXT_TO_SPEECH_REMOTE,
                       ROOM_DECORATION_REMOTE,
                       HOUSE_DECORATION_REMOTE,
                       ROOF_DECORATION_REMOTE,
                       LANGUAGE_REMOTE, 
                       UNUSED6_REMOTE, // for growth of globals
                       UNUSED7_REMOTE,
                       UNUSED8_REMOTE,
                       // now for joystick
                       JOYSTICK_SPEED_X_REMOTE,JOYSTICK_SPEED_Y_REMOTE,JOYSTICK_SPEED_Z_REMOTE,
                       JOYSTICK_AXIS_ROTATION_X_REMOTE,JOYSTICK_AXIS_ROTATION_Y_REMOTE,JOYSTICK_AXIS_ROTATION_Z_REMOTE,
                       JOYSTICK_BUTTON1_REMOTE,
                       JOYSTICK_BUTTON2_REMOTE,
                       JOYSTICK_BUTTON3_REMOTE,
                       JOYSTICK_BUTTON4_REMOTE,
                       JOYSTICK_BUTTON5_REMOTE,
                       JOYSTICK_BUTTON6_REMOTE,
                       JOYSTICK_BUTTON7_REMOTE,
                       JOYSTICK_BUTTON8_REMOTE,
                       JOYSTICK_BUTTON9_REMOTE,
                       JOYSTICK_BUTTON10_REMOTE,
                       JOYSTICK_BUTTON11_REMOTE,
                       JOYSTICK_BUTTON12_REMOTE,
                       JOYSTICK_BUTTON13_REMOTE,
                       JOYSTICK_BUTTON14_REMOTE,
                       JOYSTICK_BUTTON15_REMOTE,
                       JOYSTICK_BUTTON16_REMOTE,
                       JOYSTICK_BUTTON17_REMOTE,
                       JOYSTICK_BUTTON18_REMOTE,
                       JOYSTICK_BUTTON19_REMOTE,
                       JOYSTICK_BUTTON20_REMOTE,
                       JOYSTICK_BUTTON21_REMOTE,
                       JOYSTICK_BUTTON22_REMOTE,
                       JOYSTICK_BUTTON23_REMOTE,
                       JOYSTICK_BUTTON24_REMOTE,
                       JOYSTICK_BUTTON25_REMOTE,
                       JOYSTICK_BUTTON26_REMOTE,
                       JOYSTICK_BUTTON27_REMOTE,
                       JOYSTICK_BUTTON28_REMOTE,
                       JOYSTICK_BUTTON29_REMOTE,
                       JOYSTICK_BUTTON30_REMOTE,
                       JOYSTICK_BUTTON31_REMOTE,
                       JOYSTICK_BUTTON32_REMOTE,
                       JOYSTICK_BUTTON1_DOWN_REMOTE,
                       JOYSTICK_BUTTON2_DOWN_REMOTE,
                       JOYSTICK_BUTTON3_DOWN_REMOTE,
                       JOYSTICK_BUTTON4_DOWN_REMOTE,
                       JOYSTICK_BUTTON5_DOWN_REMOTE,
                       JOYSTICK_BUTTON6_DOWN_REMOTE,
                       JOYSTICK_BUTTON7_DOWN_REMOTE,
                       JOYSTICK_BUTTON8_DOWN_REMOTE,
                       JOYSTICK_BUTTON9_DOWN_REMOTE,
                       JOYSTICK_BUTTON10_DOWN_REMOTE,
                       JOYSTICK_BUTTON11_DOWN_REMOTE,
                       JOYSTICK_BUTTON12_DOWN_REMOTE,
                       JOYSTICK_BUTTON13_DOWN_REMOTE,
                       JOYSTICK_BUTTON14_DOWN_REMOTE,
                       JOYSTICK_BUTTON15_DOWN_REMOTE,
                       JOYSTICK_BUTTON16_DOWN_REMOTE,
                       JOYSTICK_BUTTON17_DOWN_REMOTE,
                       JOYSTICK_BUTTON18_DOWN_REMOTE,
                       JOYSTICK_BUTTON19_DOWN_REMOTE,
                       JOYSTICK_BUTTON20_DOWN_REMOTE,
                       JOYSTICK_BUTTON21_DOWN_REMOTE,
                       JOYSTICK_BUTTON22_DOWN_REMOTE,
                       JOYSTICK_BUTTON23_DOWN_REMOTE,
                       JOYSTICK_BUTTON24_DOWN_REMOTE,
                       JOYSTICK_BUTTON25_DOWN_REMOTE,
                       JOYSTICK_BUTTON26_DOWN_REMOTE,
                       JOYSTICK_BUTTON27_DOWN_REMOTE,
                       JOYSTICK_BUTTON28_DOWN_REMOTE,
                       JOYSTICK_BUTTON29_DOWN_REMOTE,
                       JOYSTICK_BUTTON30_DOWN_REMOTE,
                       JOYSTICK_BUTTON31_DOWN_REMOTE,
                       JOYSTICK_BUTTON32_DOWN_REMOTE,
							  LEGO_MOTOR1_REMOTE,
							  LEGO_MOTOR2_REMOTE,
							  LEGO_MOTOR3_REMOTE,
							  LEGO_MOTOR4_REMOTE,
							  LEGO_MOTOR5_REMOTE, // not used by Hyperbot
							  LEGO_MOTOR6_REMOTE,
							  LEGO_MOTOR7_REMOTE,
							  LEGO_MOTOR8_REMOTE,
							  LEGO_SENSOR1_REMOTE,
							  LEGO_SENSOR2_REMOTE,
							  LEGO_SENSOR3_REMOTE,
							  LEGO_SENSOR4_REMOTE,
							  LEGO_SENSOR5_REMOTE,
							  LEGO_SENSOR6_REMOTE,
							  LEGO_SENSOR7_REMOTE,
							  LEGO_SENSOR8_REMOTE,
							  LEGO_SENSOR_COUNT1_REMOTE,
							  LEGO_SENSOR_COUNT2_REMOTE,
							  LEGO_SENSOR_COUNT3_REMOTE,
							  LEGO_SENSOR_COUNT4_REMOTE,
							  LEGO_SENSOR_COUNT5_REMOTE,
							  LEGO_SENSOR_COUNT6_REMOTE,
							  LEGO_SENSOR_COUNT7_REMOTE,
							  LEGO_SENSOR_COUNT8_REMOTE,
                       FORCE_FEEDBACK_DURATION_REMOTE,
                       FORCE_FEEDBACK_GAIN_REMOTE,
                       FORCE_FEEDBACK_DIRECTION_REMOTE,
//                       FORCE_FEEDBACK_TRIGGER_BUTTON_REMOTE, // commented out on 150801 since obsolete
							  NO_IDENTIFIER_GIVEN};

const RemoteIdentifier OLD_MOUSE_SPEED_X_REMOTE = END_OF_ANIMATION_REMOTE;
const RemoteIdentifier OLD_MOUSE_SPEED_Y_REMOTE = GRASPED_REMOTE;
const RemoteIdentifier OLD_CLOCK_REMOTE = SELECTED_REMOTE;
const RemoteIdentifier OLD_RANDOM_REMOTE = RELEASED_REMOTE;

const RemoteIdentifier OLD_END_OF_ANIMATION_REMOTE = CHILDREN_REMOTE; //UNUSED1_REMOTE;
const RemoteIdentifier OLD_GRASPED_REMOTE = ANCESTORS_REMOTE; //UNUSED2_REMOTE;
const RemoteIdentifier OLD_SELECTED_REMOTE = UNUSED3_REMOTE;
const RemoteIdentifier OLD_RELEASED_REMOTE = UNUSED4_REMOTE;
// not bothering with SELECTED_REMOTE thru GRASPED_REMOTE since not used currently

enum RemoteStatus {NOT_REMOTE,GLOBAL_REMOTE,PICTURE_REMOTE};

const int DIB_brush_count = 14;

enum BrushId {LAWN1_BRUSH,LAWN2_BRUSH,LAWN4_BRUSH,LAWN_SIDE_BRUSH,
				  ROOF_A_BRUSH,ROOF_B_BRUSH,ROOF_C_BRUSH,
				  STREET1_BRUSH,STREET2_BRUSH,STREET4_BRUSH,STREET_SIDE_BRUSH,				  
				  WATER1_BRUSH,WATER2_BRUSH,WATER4_BRUSH};
				  
// these units are so tiny so that
// I can use integer arithmetic and avoid floats
// why did I use int rather than city_coordinate here??
const city_coordinate ideal_screen_width = 32000;
const city_coordinate ideal_screen_height = 24000;
const city_coordinate half_screen_width = 16000;
const city_coordinate half_screen_height = 12000;
// a tile is just an arbitrary convenience
// for most purposes integer multiples of tiles are accurate enough
const city_coordinate tile_width = 1600;
const city_coordinate tile_height = 1200;

//const int houses_to_a_block = 4; // generalize this later

const city_scale ground_scale = 100; // 100% on the ground
const city_scale initial_scale = 10*ground_scale;

enum ManSitState {MAN_SIT_DOWN,MAN_STAND_UP,
                  MAN_SIT_TAKE_OUT_OF_POCKET,MAN_STAND_AND_POCKET};

enum HelicopterState {HELICOPTER_LANDING,
							 HELICOPTER_PERSON_LEAVING,HELICOPTER_PERSON_RETURNING,
							 HELICOPTER_EMPTY};

enum HandSpriteState {HAND_OPEN, // would be good if this had stactic (last frame) versions of HAND_GRASP_X
							 HAND_GRASP_HORIZONTAL,HAND_RELEASE_HORIZONTAL,
							 HAND_GRASP_VERTICAL,HAND_RELEASE_VERTICAL};
enum HandWandSpriteState {HAND_WITHOUT_WAND,
								  HAND_USING_WAND,HAND_RESTORE_WAND,
								  HAND_GRASP_WAND,HAND_RELEASE_WAND};

enum SelectionFeedback {NO_FEEDBACK,MOVEMENT_FEEDBACK,
								RECURSIVE_MOVEMENT_FEEDBACK,
//								SIZE_CHANGE_FEEDBACK,RECURSIVE_SIZE_CHANGE_FEEDBACK,
								NO_FEEDBACK_BUT_DISPLAY_OUTSIDE_FOLLOWERS};

// too hard to deal with arbitray sized sprites -- overflows, etc.
// const city_coordinate causes min(long, const long) template problems
#define max_sprite_width 32000L
#define max_sprite_height 32000L

#if TT_32
const long max_allocation = 64000000L; // something big... not sure what
#else
const long max_allocation = 64000L;
#endif

const long max_long = 0x7FFFFFFFL;
const long min_long = -0x7FFFFFFFL-1;
const unsigned long int max_unsigned_long = 0xFFFFFFFFUL;
const unsigned short int max_unsigned_short = 0xFFFF;
const short int max_short = 0x7FFF;
const short int min_short = -0x7FFF-1;

const millisecond hold_before_event_duration = 500;
// e.g. dropping something into cubby hole
const millisecond accept_item_duration = 1000; 

const unsigned char END_OF_PATH_MARKER = 255;
const unsigned char SPECIAL_PATH_MARKER = 254;
const unsigned char EMPTY_HOLE_MAKER = 255;
const unsigned char END_OF_ROBOT_MARKER = 255;
const unsigned char END_OF_LIST_MARKER = 255;
const unsigned char EMPTY_PAGE_MARKER = 255;
const unsigned char UNDEFINED_ROBOT_MARKER = 254;
const unsigned char BLOCK_MARKER = 253;
const unsigned char HOUSE_MARKER = 252;
const unsigned char ROOM_MARKER = 251;
const unsigned char FLOOR_MARKER = 250;
const unsigned char OLD_SHARED_ITEM = 249;
const unsigned char NEW_SHARED_ITEM = 248;
const unsigned char NOTHING_MARKER = 255;
const unsigned char NULL_MARKER = 247; // added on 180100 since needs to be distinct from END_OF_LIST_MARKER and the others
const unsigned char QUEUE_MARKER = 246;
const unsigned char FILE_NAME_MARKER = 245;
const unsigned char EQUAL_TO_OLD_ITEM = 244; // new on 040601
const unsigned char EQUAL_TO_OLD_ITEM_AND_GEOMETRY = 243; // new on 040601
const unsigned char ZIP_ARCHIVE_MARKER = 242; // used beginning 171203 to refer to archives of old versions of the main notebook
//const short int appearance_count = 57;
//const int background_count = 9;

const unsigned char NEST_NETWORK_INFO_MARKER = 1;

//const int max_number_of_layers = 128; // re-written on 080201 to dynamically grow the number of layers
//const int max_number_of_postponed_layers = 256;

#if TT_WINDOWS&&GRAPHICS_TO_MEMORY
const int dib_leader_size = sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD);
#endif

// when this is changed so should debugging names in sprite.cpp
enum SpriteType {INTEGER, 
					  EXPANDER, VACUUM, COPIER,
					  THOUGHT_BUBBLE, ROBOT, 
					  NEST, BIRD,
					  TRUCK,
					  PROGRAM_PAD,
					  CUBBY, 
					  TOOLBOX,
					  PICTURE,
					  BOMB,
					  TEXT,
  					  REMOTE_PICTURE,
					  LAST_MANIPULABLE, // after this are non-graspable items
					  PICTURE_INSIDES,
					  PROGRAMMER,
					  DOOR, OPEN_DOOR, HOUSE_ABOVE, HOUSE, //LAWN,
					  HELICOPTER,
					  //	 FLOOR, FLOOR_CHAIR, FLOOR_FROM_ABOVE,
					  ROBOT_IN_TRAINING,
					  PROGRAMMER_ARM, MOUSE_WITH_HAMMER,
					  TALK_BALLOON, MARTIAN,
					  // following used when dumping and loading but
					  // aren't used otherwise
					  BLANK_INTEGER, ERROR_INTEGER, 
					  BLANK_CUBBY, LABELED_CUBBY,
					  BLANK_PICTURE, 
					  BLANK_TEXT,
					  PICTURE_WITH_INSIDES,
					  REMOTE_INTEGER,
					  GLOBAL_REMOTE_INTEGER,
					  SCALE_PICTURE,
					  REMOTE_APPEARANCE,
					  PICTURE_WITH_INDIRECTION,
                 USER_PICTURE,
                 WHOLE_FLOOR,
                 WHOLE_CITY,
                 ROBOT_WITH_TOOL,
                 ROBOT_WITH_TRAINING_COUNTER,
                 TOOL_BUTTON,
                 WHOLE_HOUSE,
                 VARIABLE_WIDTH_TEXT,
                 SOUND,
  					  REMOTE_TEXT_PICTURE,
                 BLANK_SOUND,
                 GLOBAL_USER_REMOTE, // REMOTE that make user objects like FILE_TO_SOUND
                 USER_SOUND,
                 SPEECH_SOUND,
                 MCI_SOUND,
                 BUILT_IN_SOUND,
                 BLANK_USER_SOUND,
                 BLANK_SPEECH_SOUND,
                 BLANK_MCI_SOUND,
                 BLANK_BUILT_IN_SOUND,
                 BLANK_USER_PICTURE,
                 BLANK_FORCE_FEEDBACK,
                 FORCE_FEEDBACK,
                 GLOBAL_JOYSTICK0_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK1_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK2_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK3_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK4_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK5_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK6_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK7_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK8_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK9_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK10_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK11_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK12_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK13_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK14_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK15_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK16_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK17_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK18_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK19_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK20_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK21_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK22_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK23_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK24_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK25_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK26_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK27_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK28_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK29_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK30_REMOTE_INTEGER,
                 GLOBAL_JOYSTICK31_REMOTE_INTEGER,
                 GLOBAL_HYPERBOT_REMOTE_INTEGER,
                 FORCE_REMOTE_INTEGER,
                 FORCE_REMOTE_CONTROLEE,
                 PRIMITIVE_BIRD,
					  BLANK_ROBOT,
					  BLANK_ROBOT_WITH_TOOL, // update LAST_SPRITE_TYPE if adding more
					  RATIONAL_NUMBER, // new on 080202
					  NONE_GIVEN=254}; // needs to be "stable"

const unsigned char LAST_SPRITE_TYPE = RATIONAL_NUMBER; // was BLANK_ROBOT_WITH_TOOL prior to 110402

enum BuiltIn {NOT_BUILTIN,MATH_WIZARD,MOUSE_HANDLER,SERIAL_PORT_HANDLER,
				  SPRITE_HANDLER,TIME_WIZARD};

enum PictureCycles {//PADDLE_PICTURE,BALL_PICTURE,
//						  HOUSE_A_SIDE_PICTURE,  
//						  HOUSE_B_SIDE_PICTURE,  
//						  HOUSE_C_SIDE_PICTURE,  
						  DIVIDE_PICTURE,
						  GREATER_THAN_PICTURE,LESS_THAN_PICTURE,
//						  COLLISION_QUERY_PICTURE,
//						  COLLISION_YES_PICTURE,COLLISION_NO_PICTURE,
						  UP_DOWN_ARROW_PICTURE,FOUR_WAY_ARROW_PICTURE,
						  COMPUTUER_MOUSE_PICTURE,CLOCK_PICTURE,
						  MATH_EXAMPLE_PICTURE};

enum CollisionPictures {COLLISION_YES_PICTURE,COLLISION_NO_PICTURE,
								COLLISION_QUERY_PICTURE};

enum ToolboxCacheStatus {TOOLBOX_READY_TO_CACHE, TOOLBOX_NOT_READY_TO_CACHE,
								 TOOLBOX_CACHE_VALID, TOOLBOX_CACHE_INVALID,
                         TOOLBOX_CACHE_STATUS_UNINITIALIZED};

//enum MouseButtons {LEFT_BUTTON=1,RIGHT_BUTTON=2,MIDDLE_BUTTON=4,
//						 SHIFT_KEY_DOWN=8,CONTROL_KEY_DOWN=16};

const char LEFT_BUTTON=1;
const char RIGHT_BUTTON=2;
const char MIDDLE_BUTTON=4;
const char SHIFT_KEY_DOWN=8;
const char CONTROL_KEY_DOWN=16;
const char BUTTON_STILL_DOWN=32;

enum Article {INDEFINITE_ARTICLE,DEFINITE_ARTICLE,PLURAL_ARTICLE,
              NO_ARTICLE,PRONOUN};

const int TYPE_HELP_GIVEN = 31;

const unsigned int read_buffer_size = (unsigned int) (32*1024L);

enum LegoStatus 
   {NO_LEGO_CONTROLLER,
    HYPERBOT_LEGO_CONTROLLER,CONTROL_LAB_LEGO_CONTROLLER,
    SICS_ULTRASOUND_CONTROLLER};

const long default_robot_priority = 1;

const int tt_temp_files_max = 3; // only used for DAT files

enum ChangeFrequency {NEVER_CHANGES,ALWAYS_CHANGES,CAN_CHANGE_TO_MATCH_FAILURE,ONLY_ON_NEW_CYCLE_IF_SUCCEEDS,ONLY_ON_NEW_CYCLE}; // CAN_CHANGE_TO_MATCH_FAILURE new on 250302
// removed unused CAN_CHANGE_ANYTIME on 200902 and introduced ONLY_ON_NEW_CYCLE_IF_SUCCEEDS
// moved CAN_CHANGE_TO_MATCH_FAILURE on 060103 from the end since isn't very important information if ONLY_ON_NEW_CYCLE_IF_SUCCEEDS or ONLY_ON_NEW_CYCLE apply

const int version_number_just_before_joysticks = 5;
const int first_version_to_save_cities = 7;
const int first_version_to_save_controlees = 8;
const int first_version_to_save_copied_nests = 9;
const int first_version_to_label_notebooks = 10;
const int first_version_with_houses_per_block = 11;
const int first_version_to_save_city_items = 12; // unicode as well but that didn't require file format changes
const int first_version_to_correctly_save_remote_looks = 13;
const int first_version_to_save_programmer_and_remotes_accurately = 14; // added 050100
const int first_version_to_save_number_colors = 15; // added 100100
const int first_version_to_save_cubby_of_robot = 16; // added 190100
const int first_version_with_large_pages = 17; // added 070200
const int first_version_to_save_absolute_picture_geometry = 18;
const int first_version_to_save_remote_looks_flipness = 19; // new on 191200
const int first_version_to_save_thought_bubbles_disconnected = 20; // new on 060201
const int first_version_to_save_copies_compactly = 21;
const int first_version_to_save_rational_numbers = 22;
const int latest_version_number = 22;
// when generating a new file format include way to save top_of_stack
// and tool held by a robot
// and glued to floor stuff
// and new sensors
// clean up some wasted stuff dumping geometry of pictures
// strings longer than 64K
// and saving previous_mouse of Floor
// save active flag better so that joystick buttons don't have to assume they are always active
// stuff for time travel (at least a way to dump indices into callback table)
// user pictures can save the image dimensions
// numbers can be linked operations (e.g. x2 +5)

const int first_robot_version_to_delete_when_coercing = 1; // new on 131000 so that old robots can continue to drop things on blank things and still have them around afterwards
const int first_robot_version_to_destroy_vacuum_contents_only_twice = 2; // new on 040201
const int latest_robot_version = 2;
 

const int working_set_size_limit = 256; // can't grow beyond this

enum HouseStyle {HOUSE_A,HOUSE_B,HOUSE_C,NOT_HOUSE,BROKEN_ROCKET};
   // replace with better names?

enum NaturalLanguage {NO_LANGUAGE_SPECIFIED,AMERICAN,SWEDISH,
                      BRAZILIAN_PORTUGUESE,GERMAN,FRENCH,SPANISH,
                      BRITISH,TURKISH,JAPANESE,PORTUGAL_PORTUGUESE,
							 ITALIAN, // added on 110803
							 HEBREW, // added on 131203
							 UNKNOWN_LANGUAGE,LANGUAGE_COUNT};

const string country_codes[LANGUAGE_COUNT] = {"??","US","SE","BR","DE","FR","ES","UK","TR","JP","PT","IT","IS"}; // new on 131203

const millisecond max_dispdib_time = 60; // bad driver

enum DirectXObject {DIRECT_SOUND_BUFFER,DIRECT_DRAW_SURFACE,NOT_DIRECT_DRAW_OBJECT};

enum SystemMode {FREE_PLAY,DEMO_REPLAY,PUZZLE};

const long max_background_dump_size = 32000;

enum FavoriteNames
  {VACUUM_FAVORITE,EXPANDER_FAVORITE,COPIER_FAVORITE,
   NOTEBOOK_FAVORITE,TOOLBOX_FAVORITE};

enum AutoDemo {NO_AUTODEMO, REPEAT_INTRO, REPEAT_ALL_DEMOS};

enum MartyTalk {MARTY_TALK_BALLOONS=0,
                MARTY_SPEAKS,MARTY_SPEAKS_AND_TALK_BALLOONS,
					 MARTY_SPEAKS_WITH_SUBTITLES, // new on 101199
					 MARTY_WITH_SUBTITLES, // new on 110401
					 MARTY_NO_TALK}; // must be last one due to the following 

const int marty_talk_options_count = MARTY_NO_TALK; // new on 150903

const int max_robot_classes_per_file = 1000;

enum WhenToBreakSentences
   {NEVER_BREAK_SENTENCES,BREAK_SENTENCES_TO_NOT_WASTE_HALF_BALLOON,OK_TO_BREAK_SENTENCES};

const int favorites_count = 6; // 1 extra for growth

const int NOT_COLLIDING = 1;

// if the following is changed also change javacom.rc
enum SyntheticShape {SYNTHETIC_RECTANGLE=0, SYNTHETIC_CIRCLE, SYNTHETIC_ROUNDED_RECTANGLE, SYNTHETIC_LINE_45, SYNTHETIC_LINE_315,
						   SYNTHETIC_HOLLOW_CIRCLE, SYNTHETIC_ROUNDED_HOLLOW_RECTANGLE, SYNTHETIC_DOTTED_LINE_45, SYNTHETIC_DOTTED_LINE_315,
                     SYNTHETIC_HOLLOW_RECTANGLE,// at end because rectangles have been in ToonTalk since beginning...
                     SYNTHETIC_BUT_INVISIBLE};  

enum AddToWorkingSet {DONT_ADD_TO_WORKING_SET,ADD_TO_WORKING_SET_IF_NEW,ADD_TO_WORKING_SET_REGARDLESS};

////#include <strstrea.h> commented out 210703
//#include <fstream>

extern output_file_stream tt_err_file;

#if TT_SPECIAL_DEBUGGING
extern output_file_stream tt_special_debugging_file;
#endif

const millisecond seconds_in_a_day = 24L*60L*60L;

const int joystick_button_count = 32;
const int max_number_of_joysticks = 32;

const int room_speedup = 5; // 5X speedup when watching robots on the floor in the room

const city_coordinate wall_x = 3836; // lower left corner of wall in room view -- measured in Photoshop
const city_coordinate left_room_margin = wall_x;
const city_coordinate wall_y = 9409;
const city_coordinate wall_width = 25870;
const city_coordinate wall_height = 14690;
const city_coordinate room_door_llx = 455;
const city_coordinate room_door_lly = 7064;
//const int floor_width_percent = 1900/24;
//const int floor_height_percent = 1400/35;
const int floor_width_percent = (int) (wall_width/320);
const int floor_height_percent = (24000-14690)/240;

//const city_coordinate left_room_margin = (7*ideal_screen_width)/48; // since bottom of useable floor is centered

const city_coordinate milli_width = ideal_screen_width/1000;
const city_coordinate milli_height = ideal_screen_height/1000;

const int saved_remotes_count = 6;

enum Trouble {TROUBLE_SHOOT_DISPDIB_ABNORMAL_EXIT=0,
              TROUBLE_SHOOT_GENERAL_ABNORMAL_EXIT,
              TROUBLE_SHOOT_TOO_SLOW,
              TROUBLE_SHOOT_VERY_FAST_IN_LOW_RESOLUTION,
              TROUBLE_SHOOT_JOYSTICK_CALIBRATION,
              TROUBLE_SHOOT_DIRECTX,
              TROUBLE_SHOOT_MOUSE,
              TROUBLE_SHOOT_GENERAL_TROUBLE,
              TROUBLE_SHOOT_BAD_INSTALLATION,
              TROUBLE_SHOOT_BAD_DEMO,
              TROUBLE_SHOOT_TRIAL_PERIOD_OVER,
              TROUBLE_SHOOT_QUIT_ON_FIRST_PUZZLE,
              TROUBLE_SHOOT_QUIT_ON_EARLY_ROBOT_PUZZLE,
				  TROUBLE_SHOOT_EXTENSION_NOT_INSTALLED,
				  TROUBLE_SHOOT_TTS_SELECT_FAILURE,
				  TROUBLE_SHOOT_TRIAL_VERSION_DOES_NOT_GENERATE_JAVA, // added 100600 but not really used like trouble shooting
				  TROUBLE_SHOOT_DIRECTX_MEDIA, // new on 100600
				  TROUBLE_SHOOT_JAVA_APPLET, // new on 270201
				  TROUBLE_SHOOT_BIG_NUMBERS_SLOW, // new on 270202
				  TROUBLE_SHOOT_TIME_TRAVEL_SLOW, // new on 131203
              DONT_TROUBLE_SHOOT}; // keep this one at the end of the list

const string trouble_shooting_ini_key[DONT_TROUBLE_SHOOT] = {
				  "ObsoleteAbnormalExit",
              "GeneralAbnormalExit",
              "TooSlow",
              "VeryFastInLowResolution",
              "JoystickCalibration",
              "DirectX",
              "Mouse",
              "GeneralTrouble",
              "BadInstallation",
              "BadDemo",
              NULL,
              "QuitOnFirstPuzzle",
              "QuitOnEarlyRobotPuzzle",
				  "ExtensionNotInstalled",
				  "TTSSelectFailure",
				  "TrialVersionDoesNotGenerateJava", 
				  "DirectXMedia",
				  "JavaApplet", 
				  "BigNumbersSlow",
				  "TimeTravelSlow"
};

const int max_buffer_size = 30000;

const long max_encoding_buffer_size = 2000000; // changed on 150799
// used to be max_unsigned_short with comment: starts running really slow with such large encodings anyway

// for showing the number of steps left that the robot can be trained
const city_coordinate steps_left_counter_llx = tile_width/8;
const city_coordinate steps_left_counter_lly = tile_height*2;
const city_coordinate steps_left_counter_font_height = tile_height*2;
const city_coordinate steps_left_counter_font_width = steps_left_counter_font_height/2;


const int history_count = LAST_MANIPULABLE+4;

const long default_force_intensity = 100;
const long default_force_direction = 0;
const long default_force_duration = 1000;
const long default_force_trigger_button = 0;

const short int to_windows_types_max_length = 512*4; // increased all these by *4 on 071003 for use by RCX in Italy
const short int to_windows_data_max_length = 256*4;
const short int to_windows_birds_max_length = 256*4;

enum InsertionOrdering {INSERT_UNORDERED,INSERT_BY_PRIORITY,INSERT_AT_END};

const millisecond mouse_to_run_out_time = 2000;
const millisecond mouse_to_smash_time = 6*100; // 6th image at 100ms each is the smash

const short int max_nest_items_to_dump = 1000;

const long programmer_hand_priority = -2;
const long programmer_landing_priority = -2;
const long programmer_room_walking_priority = min_long+3; // Marty is min_long+1

const int max_resource_string_length = 400;

const millisecond clipboard_paste_duration = 500;

// Marty only needs this for boxes with more than 13 holes -- too much trouble to deal with -- worst case Marty says 20th as "twenty t h".			
//enum Ordinal {CARDINAL, ORDINAL_TH, ORDINAL_ST, ORDINAL_RD, ORDINAL_ND};

const coordinate minimum_character_pixel_width = 3;  
const coordinate minimum_character_pixel_height = 5; 
//const city_coordinate minimum_character_width = minimum_character_pixel_width*100;
//const city_coordinate minimum_character_height = minimum_character_pixel_height*100;

// made global variables on 070502 so they can be customized for tablet PCs and the like
//const char stop_character = '.';
//const char run_character = ' ';

enum KeyboardAccelerator 
	{DOWN_IF_IN_HELICOPTER=0,UP_IF_IN_HELICOPTER,
	 CALL_FOR_HELICOPTER_IF_WALKING,
	 STANDUP_IF_HAND_EMPTY,PICKUP_IF_HAND_EMPTY,PICKUP_AND_FLIP_IF_HAND_EMPTY,
	 FLIP_THING_HELD,DROP_THING_HELD};

// Also change the documentation in void log_initialization
const unsigned char latest_log_version_number = 79;

enum SpaceBetweenWords {DEFAULT_SPACE_BETWEEN_WORDS,SPACE_BEFORE_PRONOUN,SPACE_BEFORE_OR_AFTER_NUMBERS};

enum ClipboardTokens {NO_CLIPBOARD_CHANGE,CLIPBOARD_EMPTY,CLIPBOARD_STRING_FOLLOWS,
							 CLIPBOARD_FILE_NAMES_FOLLOW // new on 010503
							};

enum FileNameStatus {FILE_ON_HARD_DRIVE,FILE_ON_CDROM,FILE_ON_INTERNET}; // ,FILE_ON_INTERNET_AND_DELETE_COPY};

const int max_fat_characters_per_subtitle_line = 40; // e.g. XXX or WWW since using variable width font
const int max_characters_in_talk_balloon = 200; // but not if subtitled

const wide_character wide_space = 0x3000;
const wide_character wide_comma = 0x3001;
const wide_character wide_period = 0x3002;
const wide_character wide_exclamation = 0xFF01;
const wide_character wide_question = 0xFF1F;
const wide_character wide_close_paren = 0xFF09;

const millisecond minimum_hold_for_help = 750; // 3/4 second before giving help - moved here from martian.cpp on 110100

enum TimeTravelState {TIME_TRAVEL_OFF,TIME_TRAVEL_PAUSED,TIME_TRAVEL_JUST_PAUSED,TIME_TRAVEL_ON,TIME_TRAVEL_RECORD,TIME_TRAVEL_ABOUT_TO_START,TIME_TRAVELLING_BEFORE_DIALOG};

enum SpecialDrop {NOT_SPECIAL_DROP,IGNORE_UNDERNEATH_DROP,GIVE_TOSS_DROP};

const int max_debug_depth = 20;

const int FILE_FOR_CLIPBOARD_MESSAGE_ID = 1;
const int DROP_FILES_MESSAGE_ID = 2; // new on 140204

const unsigned char toggle_infinite_stack = 19; // control-s prior to 010903 was 9 for control-i but tabs are more useful and are the same code
const unsigned char toggle_graspable = 7; // control-g

const int dib_header_size = sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD); // moved here on 250400
const int dib_header_size_no_palette = sizeof(BITMAPINFOHEADER)+2; // don't know why sizeof(RGBQUAD) was wrong // new on 051001

// was FALSE (and in-place) prior to 170800 but this looks better and isn't likely to be confused with text on the notebook page itself
const boolean text_pad_see_all = TRUE; // on 010900 switched back since it is hard to read text against a rich background 

// the following are because adding constants like 'à' to code breaks in double byte environments like Japanese
const char a224 = (char) 224; // 'à'
const char a192 = (char) 192; // 'À'
const char a225 = (char) 225; // 'á'
const char o244 = (char) 244; // 'ô'

const city_coordinate minimum_width = 50; // new on 061000 - chosen so will be one pixel at 640x480
const city_coordinate minimum_height = 50;

enum PictureActivateContext 
	{PICTURE_ACTIVATE_CONTEXT_NOT_INTIALIZED,ULTIMATE_PICTURE_CONTAINER_FLIPPED,ULTIMATE_PICTURE_CONTAINER_NOT_FLIPPED};

const city_coordinate minimum_dimension = 100000;

// 1 enables F11 and 2 means use subtitle timings
enum NarrationCreationMode {NARRATION_CREATION_OFF,NARRATION_CREATION_USING_F11,NARRATION_CREATION_USING_SUBTITLES};

enum EqualReason {NORMAL_EQUAL, FIND_NOTEBOOK_PAGE, BIRDS_CHECKING_EQUAL_NESTS, // new on 050601
					   DUMPING_EQUALS_TOGETHER}; // new on 030202

enum ColorIndex {all_index=0, red_index, green_index, blue_index}; // new on 011001

#if TT_IMGSOURCE
enum GraphicFileType {UNABLE_TO_IDENTIFY=0,BMP_FILE,GIF_FILE,JPG_FILE,PNG_FILE,PCX_FILE,TIFF_FILE,
							 WMF_FILE,EMF_FILE,PAX_FILE,PSD_FILE,TLA_ENCRYPTED_FILE,TLA_UNENCRYPTED_FILE,WBMP_FILE,TGA_FILE};
#endif

enum LeftRightTopBottom {LEFT_SIDE,RIGHT_SIDE,TOP_SIDE,BOTTOM_SIDE}; // new on 040102

// these are the colors they have been since the beginning
const color text_background_color = RGB(255,195,241);
const color number_background_color = RGB(185,250,198);
const color sound_background_color = RGB(249,241,204); 
const color force_background_color = RGB(188,185,250);
const color operation_background_color = RGB(195,220,108); // RGB(145,210,158); // new on 130202 took 40 units off of number_background_color -- changed on 170202 to look different from dark due to being off (e.g. F8)
const color active_simulated_directional_pad_color = RGB(64,255,0);
const color inactive_simulated_directional_pad_color = RGB(96,96,96);
const color simulated_directional_pad_arrow_color = RGB(255,32,32);

const int edge_size_proportion = 10; // the edges are 1/10 of the smaller dimension of a pad

const int maximum_operator_text_size = 20;
const int maximum_integer_text_size = 19+maximum_operator_text_size; // for max_long, commas, radix, and sign -- added maximum_operator_text_size for long named operators on 100202
const int maximum_double_flat_text_size = 325; // updated 050305 since can be 10^-328 to 10^308 100; // don't really know

const double pi_as_double = 3.14159265358979323846;
const double half_pi = pi_as_double/2.0;
const double radians_per_degree = pi_as_double/180.0;

const int dumped_bignum_base = 32; // nearly optimal in space and the faster than alternatives

const city_coordinate max_absolute_mouse_x_speed = ideal_screen_width; // can cross the screen in a second
const city_coordinate max_absolute_mouse_y_speed = ideal_screen_height;

// keep in correspondence with wide_string tag_string[]
enum Tag {BOX_TAG=0, TEXT_PAD_TAG, PICTURE_TAG, PICTURE_OF_TAG, USER_PICTURE_TAG, NEST_TAG, BIRD_TAG, SCALE_TAG, 
//          INTEGER_32_BIT_TAG, 
          INTEGER_TAG, FLOATING_POINT_NUMBER_64_BIT_TAG, RATIONAL_NUMBER_TAG, 
          REMOTE_CONTROL_TAG, NOTEBOOK_TAG,
          BUILT_IN_SOUND_TAG, FORCE_FEEDBACK_TAG, SPEECH_TAG, SOUND_TAG, MCI_SOUND_TAG,
//          FORCE_REMOTE_CONTROL_TAG,
          ROBOT_TAG, BOMB_TAG, TRUCK_TAG, TOOLBOX_TAG, VACUUM_TAG, BIKE_PUMP_TAG, MAGIC_WAND_TAG,
			 TRUCK_OUTSIDE_VIEWED_FROM_SIDE_TAG, TRUCK_OUTSIDE_VIEWED_FROM_ABOVE_TAG, 
			 // moved here on 261202 and then updated to have 2 views L"TruckDrivingOutsideViewedFromSide", L"TruckDrivingOutsideViewedFromAbove",
			 MARTIAN_TAG, TALK_BALLOON_TAG, 
          FOREIGN_BIRD_TAG, TOONTALK_OBJECT_TAG,
//			 INCLUDE_TAG,
			 MISSING_PICTURE_FILE_TAG,
          LINK_TAG, // only used by old XML - also all above this are sprites
          LEFT_TAG, BOTTOM_TAG, WIDTH_TAG, HEIGHT_TAG,
          INFINITE_STACK_TAG, INVISIBLE_TAG, SEE_ALL_TAG, SEE_SOME_TAG, ERASED_TAG,
          GEOMETRY_TAG, // SPECIAL_SETTINGS_TAG,
          HOLE_TAG, LABEL_TAG, NUMBER_OF_HOLES_TAG,
          TEXT_VALUE_TAG, COLOR_INDEX_TAG, VARIABLE_WIDTH_FONT_TAG,
          ON_TOP_TAG, NEST_COPIES_TAG, MY_NEST_TAG, TEMPORARILY_REMOVED_FROM_TOP_TAG,
			 ORIGINAL_NEST_TAG, 
          ADDRESS_TAG, ID_TAG, FORWARD_TO_REAL_NEST_TAG,
			 MERGED_WITH_TAG, NESTS_WHOSE_BIRDS_FORWARD_HERE_TAG,
          BUILT_IN_TAG, SHAPE_TAG, 
			 BUTTON_TAG,
          NEXT_TAG, MAXIMUM_TRAINING_STEPS_TAG, TOOL_IN_HAND_TAG, WORKING_INFO_TAG, MADE_OR_MOVED_SO_FAR_TAG, 
          FIRST_TEAMMATE_TO_FAIL_TAG, NEXT_AFTER_RUNNING_TAG, THOUGHT_BUBBLE_TAG,
          GLUED_DOWN_TAG, ACTIVE_TAG, PARTLY_VISIBLE_IF_PART_OF_PICTURE_TAG, FLIPPED_OVER_TAG, NOT_FLIPPED_OVER_TAG,
          VOLATILE_ATTRIBUTES_TAG,
          TOP_OF_STACK_TAG, NOTIFY_WHEN_DONE_TAG, 
          GEOMETRY_WHEN_FREE_AND_UNFLIPPED_TAG, SPEED_TO_RIGHT_TAG, SPEED_TO_TOP_TAG, ON_FRONT_TAG, ON_BACK_TAG, 
			 GEOMETRIC_RELATION_TO_CONTAINER_TAG,
          FOR_TAG, VALUE_TAG, OPERATIONS_APPLIED_TO_VALUE_TAG, LANGUAGE_TAG, 
          OPERATION_TAG, NEXT_OPERATION_TAG,
          TOONTALK_COMMENT_TAG,
          OPEN_TO_PAGE_TAG, PAGE_TAG, //LIVES_IN_TOOLY_TAG, 
          FILE_NAME_TAG, NUMBER_OF_PAGES_TAG, REMOTE_CONTROLS_FOR_TAG,
			 PAGE_BEING_DEFINED_TAG,
          APPEARANCE_TAG,
          GLOBAL_SENSOR_TAG, JOYSTICK_SENSOR_TAG, FORCE_FEEDBACK_SENSOR_TAG, 
          MAKER_TAG, MESSAGE_TO_MAKER_TAG,
          NOTEBOOK_I_CAME_FROM_TAG, NOTEBOOK_THAT_WAS_IN_TRUCK_WITH_ME_TAG,
          CITY_TAG, INSIDE_TAG, //ON_THE_GROUND_TAG, -- made obsolete on 100103
			 RUNNING_TAG, SIZE_TAG, MAXIMUM_HOUSES_IN_A_BLOCK_TAG, GLOBAL_PICTURE_TAG,
			 PICTURES_TO_RUN_WHEN_NOT_STOPPED_TAG,
          BLOCK_TAG, ON_TAG,
          HOUSE_TAG, ON_ROOF_TAG, ON_FRONT_WALL_TAG,
          ROOM_TAG, ON_WALL_TAG,
          FLOOR_TAG, ON_FLOOR_TAG, MAIN_BOX_TAG, INITIAL_ROBOT_TAG, ALTERNATIVES_FOR_MY_BOX_TAG,
			 GROUND_TAG,
          QUEUE_TAG, // ACTIONS_TAG,
          PROGRAMMER_TAG, ORIGINAL_BOX_TAG, RECENT_MOVEMENT_TAG, IN_THOUGHT_BUBBLE_TAG, EVENTS_SO_FAR_TAG, IN_HAND_TAG,
			 SCREEN_TAG,
//			 PROGRAMMER_APPEARANCE_TAG,
			 PROGRAMMER_AT_FLOOR_TAG, PROGRAMMER_TITLES_FLYING_TAG, PROGRAMMER_CITY_FLYING_TAG,
			 PROGRAMMER_CITY_LANDING_TAG, PROGRAMMER_CITY_WALKING_TAG, PROGRAMMER_ROOM_WALKING_TAG,
			 THOUGHT_BUBBLE_ENTERED_TAG, ORIGINAL_THOUGHT_BUBBLE_TAG, THOUGHT_BUBBLE_ROBOT_TAG,
			 HAND_TAG, HAND_HOLDING_WAND_TAG, ARM_TAG, CURRENT_SELECTION_TAG, SAVED_SELECTION_TAG, LAST_RELEASED_TAG,
			 BOX_THAT_STARTED_TRAINING_TAG, // WAITING_FOR_TAG,
			 TOOLBOX_VIEWED_FROM_SIDE_TAG, PERSON_WALKING_TAG, PERSON_SITTING_TAG, PERSON_SITTING_WITH_HAT_TAG,
			 PERSON_SITTING_WITH_HAIR_TAG, DIZZY_PERSON_TAG, DIZZY_PERSON_WITH_HAIR_TAG, DIZZY_PERSON_WITH_HAT_TAG,
			 LANDING_HELICOPTER_TAG, FLYING_HELICOPTER_TAG, EXPLOSION_TAG, 
 //         FRIEND_OF_TOOLY_TAG, 
          SUCKED_TAG, NOTHING_TAG, 
          COUNTER_TAG, ATTACHED_TAG, // for tools
          POTENTIAL_HELP_TARGET_TAG, TELEPORT_IN_SELECTION_TAG, TELEPORT_IN_TOOL_IN_HAND_TAG, PREVIOUS_TARGET_TAG, SUBJECT_TAG, 
			 // BALLOON_TAG,
          PENDING_MESSAGE_TAG, ALTERNATIVE_SPOKEN_PENDING_MESSAGE_TAG, LAST_THING_SAID_0_TAG, LAST_THING_SAID_1_TAG,
          RECENT_SENTENCE_TAG, TEXT_TAG, ALTERNATIVE_SPOKEN_TEXT_TAG, // for talk balloons
          NOTIFY_TAG, GOAL_HOLDER_TAG, VARIABLE_HOLDER_TAG, // for animations
          DO_AFTER_TAG, ANIMATE_TO_STATIC_GOAL_TAG, ANIMATE_TO_DYNAMIC_GOAL_TAG,
          TURNING_NOTEBOOK_PAGE_TAG, PAGE_NUMBER_ITEM_TAG, 
			 LEFT_PAGE_TAG, RIGHT_PAGE_TAG, 
			 SUB_PICTURES_WITH_NOTEBOOKS_TAG, SAVED_PICTURE_OF_TAG, ITEM_TO_ADD_TAG, REMOTE_NOTEBOOK_TAG,
			 NEW_CONTENTS_TAG, ROBOT_TO_TRY_TAG, BOX_TO_TRY_ROBOT_ON_TAG, WAITING_BIRDS_TAG, HATCHING_BIRD_TAG, PENDING_MESSAGES_TAG,
			 ITEM_TO_DELIVER_TAG, TOP_OF_STACK_ON_NEST_TAG, RETURN_BOX_TAG, RETURN_PATH_TAG, SAVED_ITEM_TAG, WAITING_FOR_TAG,
			 HOUSE_LOCATION_TAG, 
			 ANIMATION_TAG, ITEM_GOING_INTO_HOLE_TAG,
			 MOUSE_TAG, ITEM_TO_SMASH_TAG, ITEM_UNDERNEATH_TAG, ORIGINAL_SMASHEE_TAG, PREVIOUS_OVER_ITEM_TAG, PREVIOUS_MOUSE_TAG,
			 PREVIOUS_UNDER_ITEM_ENCODING_TAG, PREVIOIUS_OVER_ITEM_ENCODING_TAG,
			 PAGE_OFFSET_TAG, CURRENT_TOOL_IN_HAND_TAG, CURRENT_TARGET_TAG, SAVED_SUBJECT_TAG, ROBOT_TO_PUT_BEHIND_TAG,
			 SUPSENDED_ON_TAG,
			 NEXT_TOOL_IN_HAND_TAG, // new on 261003
			 SAVED_PARTS_TAG,
			 BEING_ADDED_TAG,
			 BEING_SUCKED_TAG,
          STOPPED_TAG, THINGS_ON_TOP_TAG, // obsolete - handle better 011102
			 SPECIAL_ATTRIBUTES_TAG,
          ACTIONS_TAG,
//          MADE_OR_MOVED_COUNT_TAG, 
          MOVE_TO_TAG, MADE_OR_MOVED_TAG,
          GRASP_TAG, USE_TAG, USE_ON_TAG, DROP_TAG, DROP_ON_TAG, DROP_ON_LEFT_SIDE_TAG, DROP_ON_RIGHT_SIDE_TAG,
          TYPE_TO_TAG, HATCH_TAG, GIVE_BIRD_TAG, DROP_IN_TRUCK_TAG,
          BLOW_UP_HOUSE_TAG, TYPE_LABEL_TAG, SELECT_AND_TYPE_TAG, GRASP_NEST_TAG, REPLACE_MY_BOX_TAG,
          USE_MAGIC_WAND_ON_NEST_TAG, USE_VACUUM_ON_NEST_TAG, USE_VACUUM_IN_REVERSE_TAG,
			 GRASP_MAGIC_WAND_TAG, USE_MAGIC_WAND_TAG, GRASP_BIKE_PUMP_TAG, DROP_MAGIC_WAND_TAG, USE_BIKE_PUMP_TAG,
			 GRASP_VACUUM_TAG, USE_VACUUM_TAG, DROP_VACUUM_TAG, DROP_BIKE_PUMP_TAG,
			 SOUND_FILE_TAG, TEXT_TO_SPEECH_TAG,
          NO_SUCH_TAG,NO_MORE_TAGS}; // must be last ones NO_MORE_TAGS new on 041002

enum AnimatingVariable {ANIMATING_LLX=0, ANIMATING_LLY, ANIMATING_WIDTH, ANIMATING_HEIGHT, ANIMATING_SIZE_PERCENT}; 
// new on 131102

enum DumpReason { // new on 111202
	NOT_DUMPING, DUMPING_TO_CLIPBOARD, DUMPING_TO_NOTEBOOK_PAGE, DUMPING_FOR_BIRDS, 
	DUMPING_NOTEBOOK_CONTENTS, DUMPING_NOTEBOOK_NAMES, // new on 300103
	DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL, // new on 100703
	DUMPING_INDIVIDUAL, // not a true state but to split these more easily into categories -- new on 121202
	DUMPING_TO_ENABLE_UNBAMMING, // this one is tricky but there may be sharing between the two things saved - 
	// also shouldn't create long-distance birds
	DUMPING_CITY, DUMPING_HOUSE, DUMPING_FLOOR}; 

#if _DEBUG
void* operator new(size_t nSize, const char * lpszFileName, int nLine);
#endif

const int xml_version_number = 4; 
// changed to version 3 on 150703 since can avoid extra work dealing with old "SerialNumber" and "SameAs" attributes
// changed to version 4 on 090506 to fix case sensitivity issue for scales

const int hash_byte_length = 128/8; // MD5 is 128 bits
const int hash_string_length = hash_byte_length*2; // hexidecimal // was 26; // 36**13 < 2**64 so 13*2 should always fit

const int shared_buffer_size = 8000; // who knows what is a good value?

const string media_directory = "Media\\"; // the directory name inside of Zip files
const int media_directory_length = 6;

const string main_xml_file_name = "data.xml";

const int minimum_font_width = 2; // was open-coded and 3 prior to 220603
const int minimum_font_height = 2; // was open-coded and 3 prior to 220603

const int default_compression_level = 9; // for DynaZip
const int maximum_compression_level = 20; // not always unzipable (new zip format)

const int guid_string_size = 128/4+1; 
//  +1 for terminating NULL rnamed to size on 290803 since it is confusing to call that the length

enum MouseMode {RELATIVE_MOUSE_MODE=0,ABSOLUTE_MOUSE_MODE,SIMULATED_DIRECTIONAL_PAD_MOUSE_MODE}; // new on 180402

enum ProgrammerState { // moved here from prgrmmr.h on 040603
	PROGRAMMER_TITLES,
	PROGRAMMER_CITY_FLYING,	PROGRAMMER_CITY_LANDING,
	PROGRAMMER_LEAVE_HELICOPTER, // obsolete
	PROGRAMMER_CITY_WALKING, PROGRAMMER_ROOM_WALKING,
	PROGRAMMER_AT_FLOOR,
   PROGRAMMER_IN_TRANSITION}; 

enum CycleInterruptionReason { // new on 180703
	END_OF_LOG_FILE, EVENTS_COUNTER_TOO_LARGE, EVENTS_COUNTER_MISSING, 
	LOG_FILE_MISSING, // new on 031103
	UNRECOGNIZED_SPECIAL_EVENT_TOKEN, // new on 130204
	LOG_FILE_CORRUPTED, // new on 150204
	LOG_FILE_MISSING_CITY_FILE_NAME, // new on 030504
	EVENTS_COUNTER_NON_ZERO_WITH_NO_EVENT_QUEUE, // new on 030504
	UNABLE_TO_EXTRACT_CITY_FILE // new on 030504
};

enum ReplayState  // rationalized and extended on 230803
{NO_REPLAY=0,REPLAY_IGNORE_TIMING,REPLAY_REPRODUCE_TIMING,REPLAY_REPRODUCE_TIMING_ON_NEXT_FRAME};
//	 REPLAY_IGNORE_TIMING_AFTER_TITLES,REPLAY_REPRODUCE_TIMING_AFTER_TITLES}; // new on 300104 // didn't notice already had tt_replay_after_titles

enum ToolboxContents // new on 250803
  {TOOLBOX_INTEGER_STACK=0,TOOLBOX_TEXT_STACK,
	TOOLBOX_CUBBY_STACK,TOOLBOX_NEST_STACK,
	TOOLBOX_SCALE_STACK,TOOLBOX_ROBOT_STACK,
	TOOLBOX_TRUCK_STACK,TOOLBOX_BOMB_STACK,
   TOOLBOX_STACK_COUNT};


enum VisibleNotebookPage // new on 280803
	{NOT_NOTEBOOK_PAGE, IS_LEFT_PAGE, IS_RIGHT_PAGE};

enum AddFollowerReason { // new on 210104
	LOADING_FOLLOWER, NO_ADD_FOLLOWER_REASON_GIVEN, 
	COPYING_FLIPPED_FOLLOWERS}; // new on 210105

	
enum RobotState {ROBOT_MORPH_TO_LIFE, ROBOT_MORPH_TO_INERT, // moved here from robot.cpp
					  ROBOT_WAITING, ROBOT_POINTING,
					  ROBOT_PICKING_UP_AND_RESTORING, ROBOT_DROPING_ITEM,
					  ROBOT_WORKING, ROBOT_CONFUSED,
					  ROBOT_FRUSTRATED,
					  ROBOT_PICKING_UP_ONLY,
					  ROBOT_BREATHING,
					  ROBOT_INERT,
					  ROBOT_READY_TO_PICK_UP,
                 ROBOT_WAITING_NEW};

const int default_max_working_set_size = 4; // named on 030504

// version 2 new on 120504 so even blank pictures don't match flipped pictures
// version 3 new on 110604 so old robots don't use new editing for text and number pads
const int default_robot_version = 3; 

enum ArrowKeyEditing {NO_ARROW_KEY_EDITING_POSSIBLE, ARROW_KEY_EDITING_POSSIBLE_BUT_NOT_IN_PROGRESS, ARROW_KEY_EDITING_IN_PROGRESS}; // new on 120604

const int max_variable_width_text_length = max_unsigned_short; // new on 121104

const int USER_ABORTING_LOAD = -1; // new on 140105

enum TeleportInReason // new on 040805
	{MARTY_CALLED_BY_USER, MARTY_CALLED_BY_TIMER, MARTY_CALLED_BY_ERROR_HANDLER, MARTY_CALLED_BY_LOADER, MARTY_CALLED_BY_TITLES};

#endif



