// Copyright (c) 1992-2003. Ken Kahn, Animated Programs, All rights reserved.

// Uses Microsoft Direct Input for reading the keyboard, mouse, and joysticks.

// commented out the following on 210400 since I now put c:\mssdk stuff in Include and Library paths 
// what about BC++ ??
//#include <initguid.h>
//#include <objbase.h>
//#include <objerror.h>
//#include <ole2ver.h>

//#include <dinput.h>
//#include "d:\dxsdk\include\dinput.h"

#if !defined(__TT_DEFS_H)
#include "defs.h"
#endif

#if TT_DIRECT_INPUT

#include <windows.h>
#include <mmsystem.h>
#if !defined(__TT_GLOBALS_H)
#include "globals.h"
#endif
#if !defined(__TT_STRINGS_H)
#include "strings.h"
#endif
#if !defined(__TT_UTILS_H)
#include "utils.h"
#endif
#if !defined(__TT_WINMAIN_H)
#include "winmain.h"
#endif
#if !defined(__TT_USER_H)
#include "user.h"
#endif
#if !defined(__TT_PICTURE_H)
#include "picture.h"
#endif
#if !defined(__TT_HELP_H)
#include "help.h"
#endif
//#if !defined(__TT_SPRITE_H)
//#include "sprite.h"
//#endif
//#if !defined(__TT_REMOTE_H)
//#include "remote.h"
//#endif

#if !defined(__TT_INPUT_H)
#include "input.h"
#endif

#include "iforce2.h" // Immersion's IFORCE library force feedback stuff
#include "ifc.h" // new version - 070801

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/****************************************************************************
 *
 *      DirectInput globals
 *
 ****************************************************************************/

LPDIRECTINPUT           direct_input = NULL;
LPDIRECTINPUTDEVICE     direct_input_mouse = NULL;
LPDIRECTINPUTDEVICE2A    *direct_input_joystick = NULL;
LPDIRECTINPUTDEVICE2A    *direct_input_force_feedback_joystick = NULL;
//DIJOYSTATE              *joystick_state = NULL;
//LPDIRECTINPUTDEVICE2    direct_input_joystick[max_number_of_joysticks];
//LPDIRECTINPUTDEVICE2    direct_input_force_feedback_joystick[max_number_of_joysticks];
DIJOYSTATE              joystick_state[max_number_of_joysticks];

unsigned long joystick_button_still_down_masks[max_number_of_joysticks] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

long dead_zone = 2500; // out of 10000 is dead by default -- toontalk.ini can override this
const long max_joystick_x = 1000;
const long max_joystick_y = 1000; // was 750

//const int users_force_joystick_index = 0; // others are available via remote controls

HINSTANCE direct_input_library = NULL;

flag autocenter_joystick = TRUE;

CImmMouse *touch_sensitive_mouse = NULL; // new on 070801

CImmProject touch_sensitive_mouse_project;

boolean initialize_touch_senstive_mouse() { 
	touch_sensitive_mouse = new CImmMouse;
	if (!(touch_sensitive_mouse->Initialize(Main::hInstance, tt_main_window->get_handle()))) {
		// failed Initialize call
		delete touch_sensitive_mouse;
		touch_sensitive_mouse = NULL;
		return(FALSE);
	} else {
		if (tt_using_direct_input) {
			touch_sensitive_mouse->UsesWin32MouseServices(FALSE);
		};
		// Open Immersion resource file
		FileNameStatus file_status;
		ascii_string mouse_force_resource_file_name = existing_file_name(file_status,"mforces","ifr");
      if (mouse_force_resource_file_name == NULL || !touch_sensitive_mouse_project.OpenFile(mouse_force_resource_file_name,touch_sensitive_mouse)) { 
			delete touch_sensitive_mouse;
			touch_sensitive_mouse = NULL;
			if (mouse_force_resource_file_name != NULL) delete [] mouse_force_resource_file_name;
			return(FALSE);
		};
		if (mouse_force_resource_file_name != NULL) delete [] mouse_force_resource_file_name;
		tt_force_sensitive_mouse_count = 1; 
		return(TRUE);
	};
};

CImmCompoundEffect *touch_sensitive_mouse_playing_effect = NULL;

boolean play_touch_sensitive_mouse_effect(CImmCompoundEffect *effect) {
	if (touch_sensitive_mouse_playing_effect != NULL) {
		touch_sensitive_mouse_playing_effect->Stop();
	};
	HRESULT result = effect->Start();
	if (result == 0) {
		return(FALSE);
	};
	touch_sensitive_mouse_playing_effect = effect;
	return(TRUE);
};

boolean modify_effect(CImmCompoundEffect *effect, int intensity, int direction, int duration) {
	long simple_effect_count = effect->GetNumberOfContainedEffects();
	for (int i = 0; i < simple_effect_count; i++) {
		GENERIC_EFFECT_PTR sub_effect = effect->GetContainedEffect(i); 
		if (!sub_effect->ChangeBaseParamsPolar(direction*100,duration,(LPIMM_ENVELOPE) IMM_EFFECT_DONT_CHANGE_PTR,0,intensity*100,
															IMM_EFFECT_DONT_CHANGE,IMM_EFFECT_DONT_CHANGE,IMM_EFFECT_DONT_CHANGE)) {
			return(FALSE);
		};
	};
	return(TRUE);
};

CImmCompoundEffect *touch_sensitive_mouse_effect(ascii_string name) {
	return(touch_sensitive_mouse_project.CreateEffect(name,touch_sensitive_mouse));
};

boolean get_touch_sensitive_mouse_effect_direction(ascii_string name, long &value) {
	CImmCompoundEffect *effect = touch_sensitive_mouse_effect(name);
	if (effect == NULL) return(FALSE);
	return(effect->GetContainedEffect(0L)->GetDirection(value));
};

boolean get_touch_sensitive_mouse_effect_duration(ascii_string name, DWORD &value) {
	CImmCompoundEffect *effect = touch_sensitive_mouse_effect(name);
	if (effect == NULL) return(FALSE);
	return(effect->GetContainedEffect(0L)->GetDuration(value));
};

boolean get_touch_sensitive_mouse_effect_intensity(ascii_string name, DWORD &value) {
	CImmCompoundEffect *effect = touch_sensitive_mouse_effect(name);
	if (effect == NULL) return(FALSE);
	return(effect->GetContainedEffect(0L)->GetGain(value));
};

boolean modify_and_play_touch_sensitive_mouse_effect(ascii_string name, int intensity, int direction, int duration) { // new on 140801
	CImmCompoundEffect *effect = touch_sensitive_mouse_project.CreateEffect(name,touch_sensitive_mouse);
	if (effect == NULL) return(FALSE);
	if (!modify_effect(effect,intensity,direction,duration)) return(FALSE);
	return(play_touch_sensitive_mouse_effect(effect));
};

boolean play_touch_sensitive_mouse_effect(ascii_string name) { // new on 090801
	CImmCompoundEffect *effect = touch_sensitive_mouse_project.CreateEffect(name,touch_sensitive_mouse);
	if (effect == NULL) return(FALSE);
   if (!play_touch_sensitive_mouse_effect(effect)) {
		log("Troubles starting mouse effect.");
		tt_error_file() << "Named: " << name << endl;
		return(FALSE);
	};
	return(TRUE);
};

boolean stop_touch_sensitive_mouse_effect() { // new on 090801
//	CImmCompoundEffect *effect = touch_sensitive_mouse_project.CreateEffect(name,touch_sensitive_mouse);
	if (touch_sensitive_mouse_playing_effect == NULL) return(FALSE);
	HRESULT result = touch_sensitive_mouse_playing_effect->Stop();
	touch_sensitive_mouse_playing_effect = NULL;
	if (result == 0) {
		log("Troubles stopping mouse effect.");
		return(FALSE);
	};
	return(TRUE);
};

void set_joystick_sensitivity(unsigned short int percent) {
   tt_joystick_sensitivity = percent;
   if (!autocenter_joystick && percent <= 25) {
      tt_joystick_sensitivity = (unsigned short int) (2*tt_joystick_sensitivity); // too sloppy without autocenter
   };
};

void initialize_force_feedback_joystick_input() {
   HRESULT result;
   // Turn off the device’s autocenter feature.
   // Autocenter is essentially a condition effect that uses the motors to simulate the springs in a standard joystick.
   // Turning it off gives you more control over the device.
   if (ini_int(AC(IDC_SWITCHES),AC(IDC_TURN_OFF_JOYSTICK_AUTO_CENTER),FALSE,1)) {
      DIPROPDWORD DIPropAutoCenter;
      DIPropAutoCenter.diph.dwSize = sizeof(DIPropAutoCenter);
      DIPropAutoCenter.diph.dwHeaderSize = sizeof(DIPROPHEADER);
      DIPropAutoCenter.diph.dwObj = 0;
      DIPropAutoCenter.diph.dwHow = DIPH_DEVICE;
      DIPropAutoCenter.dwData = DIPROPAUTOCENTER_OFF;
      result = direct_input_joystick[tt_joystick_count]->SetProperty(DIPROP_AUTOCENTER,&DIPropAutoCenter.diph);
      if (FAILED(result)) {
         tt_error_file() << "DirectInput. Failed to change device property of force feedback joystick#" << tt_force_feedback_joystick_count << endl;
      } else {
         autocenter_joystick = FALSE;
      };
      // without autocenter the dead zone is too big
      DIPROPDWORD word;
      word.diph.dwSize       = sizeof(word);
      word.diph.dwHeaderSize = sizeof(word.diph);
      word.diph.dwObj        = DIJOFS_X;
      word.diph.dwHow        = DIPH_BYOFFSET;
      word.dwData            = dead_zone/8;
      result = direct_input_joystick[tt_joystick_count]->SetProperty(DIPROP_DEADZONE, &word.diph);
      if (FAILED(result)) {
         tt_error_file() << "DirectInput. Resetting Joystick SetProperty dead zone failed with result=" << result << endl;
      };
   };
   // force feedback joysticks need to be exclusive 
//   result = direct_input_joystick[tt_joystick_count]->SetCooperativeLevel(tt_main_window->get_handle(),
//                                                                             DISCL_EXCLUSIVE | DISCL_FOREGROUND);
//   if (FAILED(result)) {
//      tt_error_file() << "DirectInput. Joystick SetCooperativeLevel failed with result=" << result << endl;
//   };
   direct_input_force_feedback_joystick[tt_force_feedback_joystick_count] = direct_input_joystick[tt_joystick_count];
   tt_force_feedback_joystick_count++;
};

/*
#if TT_MICROSOFT

#define CreateEffects IFCreateEffects
#define LoadProjectFile IFLoadProjectFile
#define ReleaseEffects IFReleaseEffects
#define ReleaseProject IFReleaseProject

#else
*/
//extern "C" {
//     typedef HIFORCEPROJECT __stdcall *IFLoadProjectFileType(LPCSTR pProjectFileName, LPDIRECTINPUTDEVICE2A pDevice);
//};

//IFLoadProjectFileType *LoadProjectFile;
typedef HIFORCEPROJECT (__stdcall *IFLoadProjectFileType) (LPCSTR pProjectFileName, LPDIRECTINPUTDEVICE2A pDevice);
HIFORCEPROJECT (__stdcall *LoadProjectFile) (LPCSTR pProjectFileName, LPDIRECTINPUTDEVICE2A pDevice);

typedef LPDIRECTINPUTEFFECT * (__stdcall *IFCreateEffectsType) (HIFORCEPROJECT hProject, LPCSTR pObjectName, int *pNumEffects );
LPDIRECTINPUTEFFECT * (__stdcall *CreateEffects) (HIFORCEPROJECT hProject, LPCSTR pObjectName, int *pNumEffects );

typedef BOOL (__stdcall *IFReleaseEffectsType) (HIFORCEPROJECT hProject, LPDIRECTINPUTEFFECT *pEffects);
BOOL (__stdcall *ReleaseEffects) (HIFORCEPROJECT hProject, LPDIRECTINPUTEFFECT *pEffects);

typedef BOOL (__stdcall *IFReleaseProjectType) (HIFORCEPROJECT hProject);
BOOL (__stdcall *ReleaseProject) (HIFORCEPROJECT hProject);

//#endif

HIFORCEPROJECT *force_projects = NULL;
HINSTANCE iforce2_libary = NULL;

//LPDIRECTINPUTEFFECT *force_feedback_effects = NULL;
//int number_of_effects = 0;
ForceRemoteControlee *current_force = NULL;

ascii_string current_force_feedback_effect = NULL;
ascii_string background_force_feedback_effect = NULL;

ascii_string return_background_force_feedback_effect() { // new on 121102
   return(background_force_feedback_effect);
};

void set_background_force_feedback_effect(ascii_string name) {
   if (force_projects == NULL && touch_sensitive_mouse == NULL) {
		return;
	};
   if (name != NULL) {
		if (background_force_feedback_effect != NULL && strcmp(name,background_force_feedback_effect) == 0) return; // already is (new on 090801)
      background_force_feedback_effect = name;
      play_force_feedback_effect(name);
   } else if (background_force_feedback_effect != NULL) {
		background_force_feedback_effect = NULL;
      stop_force_feedback_effect();
//      boolean old;
//      current_force = force_parameter_controlee(force_index(background_force_feedback_effect),users_force_joystick_index,old);
//      current_force->stop_effect();
//		if (current_force_feedback_effect == background_force_feedback_effect) {
//			current_force_feedback_effect = NULL;
//		};
//		current_force = NULL;
//      background_force_feedback_effect = NULL;
   };
};

void stop_force_feedback_effects(LPDIRECTINPUTEFFECT *effects, int count) {
   if (force_projects == NULL) return;
   for (int i = 0; i < count; i++) {
      effects[i]->Stop();
   };
};

/*
void really_stop_force_feedback_effect() {
   stop_force_feedback_effects(force_feedback_effects,number_of_effects);
#if TT_DEBUG_ON
   if (tt_debug_mode == 22222) {
      if (current_force_feedback_effect != NULL) {
          tt_error_file() << "Stopping force feedback effect " << current_force_feedback_effect << " on frame " << tt_frame_number << endl;
      } else {
          tt_error_file() << "Stopping previous not remembered force feedback effect on frame " << tt_frame_number << endl;
      };
   };
#endif
   ReleaseEffects(force_projects[users_force_joystick_index],force_feedback_effects);
   force_feedback_effects = NULL;
   current_force_feedback_effect = NULL;
};
*/

void release_force_feedback_effects(int joystick_index, LPDIRECTINPUTEFFECT *effects) {
	if (touch_sensitive_mouse != NULL) {
		delete touch_sensitive_mouse; // new on 090801
		touch_sensitive_mouse = NULL;
	};
   if (force_projects == NULL) return;
#if TT_DEBUG_ON
   if (tt_debug_mode == 22225) {
      tt_error_file() << "Releasing force feedback effects for joystick #" << joystick_index << endl;
   };
#endif
//#if TT_MICROSOFT||TT_DEBUG_ON
   // following should be called even if not TT_DEBUG_ON but it causes a crash on cleanup
   ReleaseEffects(force_projects[joystick_index],effects);
//#endif
};

void stop_force_feedback_effect() {
   if (force_projects == NULL && touch_sensitive_mouse == NULL) return;
   if (current_force_feedback_effect == NULL) return;
   if (touch_sensitive_mouse != NULL) { // moved here on 110602 since need to turn off even if the following returns
		stop_touch_sensitive_mouse_effect();
	};
   if (current_force_feedback_effect == background_force_feedback_effect) return;  // background turned off by setting it to NULL
   if (current_force != NULL) current_force->stop_effect();
	current_force_feedback_effect = NULL;
	current_force = NULL;
   restore_background_force_callback(NULL);
};

LPDIRECTINPUTEFFECT *create_force_feedback_effects(ascii_string name, int joystick_index, int &count) {
   if (force_projects == NULL || force_projects[joystick_index] == NULL) return(NULL);
   LPDIRECTINPUTEFFECT *effects = CreateEffects(force_projects[joystick_index],name,&count);
   //  following caused errors!!!?!!?
//#if TT_DEBUG_ON
//   if (tt_debug_mode == 22225) {
//      tt_error_file() << "Creating " << count << " force feedback effects named " << name << " for joystick #" << joystick_index << endl;
//   };
//#endif
   if (effects == NULL) {
      unload_some_effects();
      effects = CreateEffects(force_projects[joystick_index],name,&count); // try again
   };
   if (effects == NULL) {
//		string message = append_strings(_T("Couldn't load force feedback effect named "),name); 
//      log(message);
//      delete [] message;
		tt_error_file() << "Couldn't load force feedback effect named " << name << endl;
      count = 0;
   };
   return(effects);
};

void play_force_feedback_effects(LPDIRECTINPUTEFFECT *effects, int count) {
   if (force_projects == NULL) return;
   for (int i = 0; i < count; i++) {
//      if (tt_debug_mode == 22224) tt_error_file() << i << endl; // works around a bug in Direct Input
         // seems that since i otherwise is in a register then Start(0,1) sets the register to zero...
/*
           force_feedback_effects[i]->GetEffectGuid(&guid);
           pdei.dwSize = sizeof(pdei);
           direct_input_force_feedback_joystick[0]->GetEffectInfo(&pdei,guid);
           if (DIEFT_GETTYPE(pdei.dwEffType) == DIEFT_CONDITION) {
              peff.dwSize = sizeof(peff);
              int axis_count = 32;
              DICONDITION *conditions =  new DICONDITION[axis_count];
              peff.lpvTypeSpecificParams =  conditions;
              peff.cbTypeSpecificParams = axis_count*sizeof(DICONDITION);
              peff.dwFlags		= DIEFF_POLAR | DIEFF_OBJECTIDS;
	           peff.cAxes			= axis_count;
              HRESULT result = force_feedback_effects[i]->GetParameters(&peff,DIEP_TYPESPECIFICPARAMS);
              switch (result) {
                case DIERR_INVALIDPARAM:
                  tt_error_file() << "DIERR_INVALIDPARAM" << endl;
                  break;
                case DIERR_MOREDATA:
                  tt_error_file() << "DIERR_MOREDATA" << endl;
                  break;
                case DIERR_NOTINITIALIZED:
                  tt_error_file() << "DIERR_NOTINITIALIZED" << endl;
                  break;
               };
          };
*/
      HRESULT result = effects[i]->Start(1,0);
      if (FAILED(result)) {
         tt_error_file() << "Problem starting a force feedback effect." << endl;
      };
   };
};

// and stop from stop_sound??

void play_force_feedback_effect(ascii_string name, boolean remember_this, int joystick_index) {
	// joystick_index == 0 is touch sensitive mouse (if there is one otherwise first joystick)
   if (force_projects == NULL && touch_sensitive_mouse == NULL) return;
   if (current_force_feedback_effect != NULL && strcmp(current_force_feedback_effect,name) == 0) return; // already loaded and playing
   if (remember_this) current_force_feedback_effect = name; // moved here on 090801
//   if (force_feedback_effects != NULL) { // release old ones
//      really_stop_force_feedback_effect();
//   };
#if TT_DEBUG_ON
   if (tt_debug_mode == 22225) {
      tt_error_file() << "Playing force feedback effect " << name << " on frame " << tt_frame_number << endl;
   };
#endif
	if (touch_sensitive_mouse != NULL) {
		if (joystick_index == 0) {
			play_touch_sensitive_mouse_effect(name);
			return;
		};
		if (force_projects == NULL) return; // there are no other joysticks
	};
   boolean old;
   // note that this scheme means that if a user changes the parameters of a system effect then
   // I could reflect those changes when the system uses the effect as well -- think about it...
   current_force = force_parameter_controlee(force_index(name),joystick_index,old);
   current_force->play_effect();
//   force_feedback_effects = create_force_feedback_effects(name,joystick_index,number_of_effects);
//   if (force_feedback_effects != NULL) {
//      play_force_feedback_effects(force_feedback_effects,number_of_effects);
/*
      if (background_force_feedback_effect != NULL &&
          strcmp(name,background_force_feedback_effect) != 0) {
         // schedule the resumption of the background when this one is over
         effect_description.dwSize = sizeof(effect_description);
         HRESULT result = force_feedback_effects[0]->GetParameters(&effect_description,DIEP_DURATION);
         switch (result) {
            case DIERR_INVALIDPARAM:
               log("Invalid parameter to GetParameters of force feedback effect.");
               break;
            case DIERR_MOREDATA:
               log("GetParameters of force feedback effect has more data.");
               break;
            case DIERR_NOTINITIALIZED:
               log("GetParameters of force feedback effect not initialized.");
               break;
            default:
               if (effect_description.dwDuration != INFINITE) {
                  tt_global_picture->do_after(effect_description.dwDuration,NULL,restore_background_force_callback);
               };
         };
      };
*/
};

DIEFFECT effect_description;
const int axis_count = 32; // OK??

void initialize_effect_description() {
   effect_description.dwSize = sizeof(effect_description);
   effect_description.dwFlags = DIEFF_POLAR | DIEFF_OBJECTIDS; //DIEFF_OBJECTOFFSETS;
   effect_description.cAxes = axis_count;
   if (effect_description.rgdwAxes == NULL) effect_description.rgdwAxes = new DWORD[axis_count];
   if (effect_description.rglDirection == NULL) effect_description.rglDirection = new LONG[axis_count];
};

boolean get_simple_effect_parameter(LPDIRECTINPUTEFFECT effect, DWORD type) {
   if (force_projects == NULL) return(FALSE);
   initialize_effect_description();
   HRESULT result = effect->GetParameters(&effect_description,type);
   switch (result) {
      case DIERR_INVALIDPARAM:
         log(_T("Invalid parameter to GetParameters of force feedback effect."),FALSE,TRUE);
         return(FALSE);
      case DIERR_MOREDATA:
         log(_T("GetParameters of force feedback effect has more data."),FALSE,TRUE);
         // could handle it but shouldn't happen
         return(FALSE);
      case DIERR_NOTINITIALIZED:
         log(_T("GetParameters of force feedback effect not initialized."),FALSE,TRUE);
         return(FALSE);
      default:
         return(TRUE);
    };
};

boolean set_simple_effect_parameter(LPDIRECTINPUTEFFECT effect, DWORD type) {
   if (force_projects == NULL) return(FALSE);
   HRESULT result = effect->SetParameters(&effect_description,type); //|DIEP_NODOWNLOAD);
   switch (result) {
      case DI_EFFECTRESTARTED:
#if TT_DEBUG_ON
         log(_T("Info. SetParameters of force feedback effect restarted."));
#endif
         return(TRUE);
      case DI_DOWNLOADSKIPPED:
#if TT_DEBUG_ON
         log(_T("Info. SetParameters of force feedback effect download skipped."));
#endif
         return(TRUE);
      case DI_TRUNCATED:
#if TT_DEBUG_ON
         log(_T("Info. SetParameters of force feedback effect truncated."));
#endif
         return(TRUE);
      case DI_TRUNCATEDANDRESTARTED:
#if TT_DEBUG_ON
         log(_T("Info. SetParameters of force feedback effect truncated and downloaded."));
#endif
         return(TRUE);
      case DIERR_INVALIDPARAM:
         log(_T("Invalid parameter to SetParameters of force feedback effect."),FALSE,TRUE);
         return(FALSE);
      case DIERR_INCOMPLETEEFFECT:
         log(_T("Warning. SetParameters of force feedback effect incomplete."),FALSE,TRUE);
         return(FALSE);
      case DIERR_NOTINITIALIZED:
         log(_T("Warning. SetParameters of force feedback effect not initialized."),FALSE,TRUE);
         return(FALSE);
      case DIERR_INPUTLOST:
         log(_T("Warning. SetParameters of force feedback effect input lost."),FALSE,TRUE);
         return(FALSE);
      case DIERR_EFFECTPLAYING:
         log(_T("Warning. SetParameters of force feedback effect playing."),FALSE,TRUE);
         return(FALSE);
      default:
         return(TRUE);
    };
};

/*
long longify(DWORD value) {
   // this is used in a context where INFINITE is max unsigned long -- this replaces it with max signed long
   if (value > max_long) {
      return(max_long);
   } else {
      return(value);
   };
};
*/

long force_feedback_duration(LPDIRECTINPUTEFFECT *effects) {
   // if compound effect just read value from first component
   if (!get_simple_effect_parameter(effects[0],DIEP_DURATION)) {
      log(_T("Problem retrieving force feedback effect's duration."),FALSE,TRUE);
   };
   return(effect_description.dwDuration/1000); // convert from microseconds to milliseconds
};

long set_force_feedback_duration(LPDIRECTINPUTEFFECT *effects, int count, long new_value) {
   unsigned long value;
   if (new_value < 0) {
      value = 0;
   } else if ((unsigned long) new_value >= max_unsigned_long/1000UL) {
      value = max_unsigned_long; // INFINITE
   } else {
      value = new_value*1000UL;
   };
   initialize_effect_description();
   effect_description.dwDuration = value;
   for (int i = 0; i < count; i++) {
      if (!set_simple_effect_parameter(effects[i],DIEP_DURATION))  {
         tt_error_file() << "Problem setting force feedback effect duration to " << new_value << endl;
         return(0);
      };
   };
   return(value/1000);
};

long force_feedback_intensity(LPDIRECTINPUTEFFECT *effects) {
   if (!get_simple_effect_parameter(effects[0],DIEP_GAIN)) {
      log(_T("Problem retrieving force feedback effect's strength."),FALSE,TRUE);
   };
   return(effect_description.dwGain/100);  // new range is 0 to 100 (not to 10,000)
};

long set_force_feedback_intensity(LPDIRECTINPUTEFFECT *effects, int count, long new_value) {
   unsigned long value;
   if (new_value < 0) {
      value = 0;
   } else if (new_value >= 100) {
      value = 10000; // max intensity
   } else {
      value = new_value*100UL;
   };
   initialize_effect_description();
   effect_description.dwGain = value;
   for (int i = 0; i < count; i++) {
      if (!set_simple_effect_parameter(effects[i],DIEP_GAIN))  {
         tt_error_file() << "Problem setting force feedback effect intensity to " << new_value << endl;
         return(0);
      };
   };
   return(value/100);
};

/*
long force_feedback_trigger_button(LPDIRECTINPUTEFFECT *effects) {
   if (!get_simple_effect_parameter(effects[0],DIEP_TRIGGERBUTTON) {
      log("Problem retrieving force feedback effect's trigger button.");
   };
   return(effect_description.dwTriggerButton+1); // 1st button is called 1 not 0 -- 0 means no trigger
};

long set_force_feedback_trigger_button(LPDIRECTINPUTEFFECT *effects, int count, long new_value) {
   unsigned long value;
   if (new_value <= 0) {
      value = DIEB_NOTRIGGER;
   } else {
      value = new_value-1;
   };
   initialize_effect_description();
   effect_description.dwTriggerButton = value;
   for (int i = 0; i < count; i++) {
      if (!set_simple_effect_parameter(effects[i],DIEP_TRIGGERBUTTON))  {
         tt_error_file() << "Problem setting force feedback effect trigger button to " << new_value << endl;
         return(0);
      };
   };
   return(value+1);
};
*/

long force_feedback_direction(LPDIRECTINPUTEFFECT *effects) {
   effect_description.cAxes = axis_count;
   if (!get_simple_effect_parameter(effects[0],DIEP_DIRECTION)) {
      log(_T("Problem retrieving force feedback effect's direction."),FALSE,TRUE);
   };
   return(effect_description.rglDirection[0]/100);  // degrees are good enough
};

long set_force_feedback_direction(LPDIRECTINPUTEFFECT *effects, int count, long new_value) {
   unsigned long value;
   new_value %= 360;
   if (new_value < 0) new_value += 360;
   value = new_value*100UL;
   effect_description.cAxes = axis_count;
   get_simple_effect_parameter(effects[0],DIEP_DIRECTION); // set cAxes to correct value
   effect_description.dwFlags = DIEFF_POLAR | DIEFF_OBJECTIDS;
   effect_description.rglDirection[0] = value;
   // this ignores any other axis and assumes rgdwAxes is OK
   for (int i = 0; i < count; i++) {
      if (!set_simple_effect_parameter(effects[i],DIEP_DIRECTION)) {
         tt_error_file() << "Problem setting force feedback effect direction to " << new_value << endl;
         return(0);
      };
   };
   return(value/100);
};

int force_count = 0;
const int max_force_resources = 256; // OK?
ascii_string *force_names = NULL;

ascii_string *return_force_names() {
  return(force_names);
};

ascii_string force_name(int index) {
	if (force_names == NULL) return(""); // something better? -- new on 131100
	return(force_names[index]);
};

int return_force_count() {
   return(force_count);
};

int force_index(ascii_string name) {
  for (int i = 0; i < force_count; i++) {
     if (strcmp(name,force_names[i]) == 0) return(i);
  };
  return(0); // shouldn't happen
};

void read_force_resource_names(ascii_string file_name) {
   force_names = new ascii_string[max_force_resources];
   char buffer[128];
	input_file_stream infile(file_name, std::ios_base::binary ); // std::ios_base::nocreate | 
 	infile.get(buffer, 5);
	if (strncmp(buffer, "ifpr", 4) != 0 )	// file is not valid ifr file
		return;
	infile.ignore(8);						// skip size info
	while (infile.good() && force_count<max_force_resources) {
		infile.get(buffer, 128, '\0');		// force Name
      force_names[force_count] = new char[strlen(buffer) + 1];
		strcpy(force_names[force_count], buffer);
		infile.ignore(1);
		infile.get(buffer, 128, '\0');		// force type
		infile.ignore(48);					// skip the ID value
		do {
			infile.get(buffer, 128, '\0');	// Magnitude
			infile.ignore(1);
		} while (strlen(buffer) > 0);
		while (infile.peek() == '\0')
			infile.ignore(1);
		force_count++;
	}
};

BOOL FAR PASCAL initialize_joystick_input(LPCDIDEVICEINSTANCE device_instance, LPVOID ) {
    if (tt_joystick_count > max_number_of_joysticks) return(FALSE);
    if (direct_input_joystick == NULL) {
       direct_input_joystick = new LPDIRECTINPUTDEVICE2A[max_number_of_joysticks];
	   int i;
       for (i = 0; i < max_number_of_joysticks; i++) {
          direct_input_joystick[i] = NULL;
       };
       // could move this so only allocated when needed
       direct_input_force_feedback_joystick = new LPDIRECTINPUTDEVICE2A[max_number_of_joysticks];
       for (i = 0; i < max_number_of_joysticks; i++) {
          direct_input_force_feedback_joystick[i] = NULL;
       };
//       joystick_state = new DIJOYSTATE[max_number_of_joysticks];
    };
    // called for each joystick attached to machine
    LPDIRECTINPUTDEVICE temp_joystick;
    HRESULT result = direct_input->CreateDevice(device_instance->guidInstance, &temp_joystick, NULL);
    if (FAILED(result)) {
        tt_error_file() << "DirectInput. CreateDevice for joystick #" << tt_joystick_count << " failed with result=" << result << endl;
        return(DIENUM_CONTINUE);  // skip this joystick
    };
    // need to do this so it can be polled
    result = temp_joystick->QueryInterface(IID_IDirectInputDevice2,
                                            (LPVOID *)&direct_input_joystick[tt_joystick_count]);
    if (FAILED(result)) {
        log(_T("DirectInput. Joystick QueryInterface failed. "),FALSE,TRUE);
        return(DIENUM_CONTINUE);
    }
    result = direct_input_joystick[tt_joystick_count]->SetDataFormat(&c_dfDIJoystick);
    if (FAILED(result)) {
        log(_T("DirectInput. Joystick SetDataFormat failed. "),FALSE,TRUE);
        return(DIENUM_CONTINUE);
    }
    result = direct_input_joystick[tt_joystick_count]->SetCooperativeLevel(tt_main_window->get_handle(),
                                                                           DISCL_EXCLUSIVE | DISCL_FOREGROUND);
    if (FAILED(result)) {
        tt_error_file() << "DirectInput. Joystick SetCooperativeLevel failed with result=" << result << endl;
        return(DIENUM_CONTINUE);
    };
//    if (device_instance->dwDevType == DIDEVTYPE_JOYSTICK) { // might be other input device
       DIPROPRANGE range;
       range.diph.dwSize       = sizeof(range);
       range.diph.dwHeaderSize = sizeof(range.diph);
       range.diph.dwObj        = DIJOFS_X;
       range.diph.dwHow        = DIPH_BYOFFSET;
       range.lMin              = -max_joystick_x;
       range.lMax              = max_joystick_x;
       result = direct_input_joystick[tt_joystick_count]->SetProperty(DIPROP_RANGE, &range.diph);
       if (FAILED(result)) {
          tt_error_file() << "DirectInput. Joystick SetProperty failed with result=" << result << endl;
          return(DIENUM_CONTINUE);
       };
       range.diph.dwObj        = DIJOFS_Y;
       range.lMin              = -max_joystick_y;
       range.lMax              = max_joystick_y;
       direct_input_joystick[tt_joystick_count]->SetProperty(DIPROP_RANGE, &range.diph); // do it for y now
       dead_zone = ini_int(AC(IDC_SWITCHES),AC(IDC_JOYSTICK_DEAD_ZONE),FALSE,dead_zone);
       DIPROPDWORD word;
       word.diph.dwSize       = sizeof(word);
       word.diph.dwHeaderSize = sizeof(word.diph);
       word.diph.dwObj        = DIJOFS_X;
       word.diph.dwHow        = DIPH_BYOFFSET;
       word.dwData            = dead_zone;
       result = direct_input_joystick[tt_joystick_count]->SetProperty(DIPROP_DEADZONE, &word.diph);
       if (FAILED(result)) {
          tt_error_file() << "DirectInput. Joystick SetProperty dead zone failed with result=" << result << endl;
//          return(DIENUM_CONTINUE); // not important enough to skip this
       };
       word.diph.dwObj        = DIJOFS_Y; // do it for y now
       direct_input_joystick[tt_joystick_count]->SetProperty(DIPROP_DEADZONE, &word.diph);
//    };
    // now check if it is a force feedback joystick and deal with it if so
    DIDEVCAPS DIDeviceCaps;
    DIDeviceCaps.dwSize = sizeof(DIDEVCAPS);
    direct_input_joystick[tt_joystick_count]->GetCapabilities(&DIDeviceCaps);
    if (DIDeviceCaps.dwFlags & DIDC_FORCEFEEDBACK) {
       initialize_force_feedback_joystick_input();
		 // following moved into conditional on 141299
		 FileNameStatus file_status;
		 ascii_string dll_file_name = existing_file_name(file_status,"iforce2","dll");
		 if (dll_file_name != NULL) {
			 iforce2_libary = LoadLibraryA(dll_file_name);
			 delete [] dll_file_name;
		 };
	 };
    direct_input_joystick[tt_joystick_count]->Acquire();
    tt_joystick_count++; // everything went fine
    return(DIENUM_CONTINUE);
};

extern "C" {
     typedef HRESULT (STDMETHODCALLTYPE *DIRECTINPUTCREATE)
                      (HINSTANCE, DWORD, LPDIRECTINPUT *, LPUNKNOWN);
};

flag mouse_exclusive = FALSE;

//#if TT_MICROSOFT
boolean initialize_direct_input(HINSTANCE instance) {
//#else
	// not sure why Borland won't accept the above...
//	boolean initialize_direct_input(HANDLE instance) {
//#endif
// thanks to Raymond Chen for the following reply on directx.private.directx.directinput
/*
  If it's being loaded at load time, then it means that you are still using
  load-time imports to DirectInput instead of run-time imports.  You need to
  stop calling DirectInputCreate directly; use GetProcAddress instead.
*/
   // the problem this solves is for things to work if dinput.dll is missing
   // which is the case for NT 4.0
   direct_input_library = LoadLibraryA("DINPUT.DLL");
   if (!direct_input_library) return(FALSE);
//   typedef STDMETHOD(DIRECTINPUTCREATE) (HINSTANCE, DWORD, LPDIRECTINPUT *, LPUNKNOWN);
   DIRECTINPUTCREATE DirectInputCreate;
//   typedef HRESULT FAR PASCAL (*LPDIRECTINPUTCREATE) (HINSTANCE, DWORD, LPDIRECTINPUT *, LPUNKNOWN);
//   HRESULT FAR PASCAL (*DirectInputCreate) (HINSTANCE, DWORD, LPDIRECTINPUT *, LPUNKNOWN);
#ifdef UNICODE
   DirectInputCreate = (DIRECTINPUTCREATE) GetProcAddress(direct_input_library, "DirectInputCreateW");
#else
   DirectInputCreate = (DIRECTINPUTCREATE) GetProcAddress(direct_input_library, "DirectInputCreateA");
#endif
   if (DirectInputCreate == NULL) {
      FreeLibrary(direct_input_library);
      return(FALSE);
   };

    HWND hwnd = tt_main_window->get_handle();
    HRESULT result;

    /*
     *  Register with the DirectInput subsystem and get a pointer
     *  to a IDirectInput interface we can use.
     *
     *  Parameters:
     *
     *      instance
     *
     *          Instance handle to our application or DLL.
     *
     *      DIRECTINPUT_VERSION
     *
     *          The version of DirectInput we were designed for.
     *          We take the value from the <dinput.h> header file.
     *
     *      &direct_input
     *
     *          Receives pointer to the IDirectInput interface
     *          that was created.
     *
     *      NULL
     *
     *          We do not use OLE aggregation, so this parameter
     *          must be NULL.
     *
     */
    result = DirectInputCreate(instance, DIRECTINPUT_VERSION, &direct_input, NULL);

    if (FAILED(result)) {
        tt_error_file() << "DirectInputCreate failed with result=" << result << ". ToonTalk will run without joystick support. " << endl;
        return(FALSE);
    }

    /*
     *  Obtain an interface to the system mouse device.
     *
     *  Parameters:
     *
     *      GUID_SysMouse
     *
     *          The instance GUID for the device we wish to access.
     *          GUID_SysMouse is a predefined instance GUID that
     *          always refers to the system mouse device.
     *
     *      &direct_input_mouse
     *
     *          Receives pointer to the IDirectInputDevice interface
     *          that was created.
     *
     *      NULL
     *
     *          We do not use OLE aggregation, so this parameter
     *          must be NULL.
     *
     */
    result = direct_input->CreateDevice(GUID_SysMouse, &direct_input_mouse, NULL);

    if (FAILED(result)) {
        tt_error_file() << "DirectInput. CreateDevice for mouse failed with result=" << result << endl;
        return(FALSE);
    }

    /*
     *  Set the data format to "mouse format".
     *
     *  A data format specifies which controls on a device we
     *  are interested in, and how they should be reported.
     *
     *  This tells DirectInput that we will be passing a
     *  DIMOUSESTATE structure to IDirectInputDevice::GetDeviceState.
     *
     *  Parameters:
     *
     *      c_dfDIMouse
     *
     *          Predefined data format which describes
     *          a DIMOUSESTATE structure.
     */
    result = direct_input_mouse->SetDataFormat(&c_dfDIMouse);

    if (FAILED(result)) {
        log(_T("DirectInput. SetDataFormat failed. "),FALSE,TRUE);
        switch (result) {
           case DIERR_ACQUIRED:
              log(_T("Already acquired."));
              break;
           case DIERR_INVALIDPARAM:
              log(_T("Invalid parameter."));
              break;
           case DIERR_NOTINITIALIZED:
              log(_T("Not initialized."));
              break;
        };
//        tt_error_file() << endl;
        return(FALSE);
    }
    /*
     *  Set the cooperativity level to let DirectInput know how
     *  this device should interact with the system and with other
     *  DirectInput applications.
     *
     *  Parameters:
     *
     *      DISCL_NONEXCLUSIVE
     *
     *          Retrieve mouse data when acquired, not interfering
     *          with any other applications which are reading mouse
     *          data.
     *
     *      DISCL_FOREGROUND
     *
     *          If the user switches away from our application,
     *          automatically release the mouse back to the system.
     *
     */
	 if (//tt_using_dispdib == 1 && 
		  tt_direct_input_mouse_exclusive_ok) { // full screen - new on 130800
		 // introduced a new variable (from toontalk.ini) that controls whether this happens
		 // this should be more responsive and hopefully avoids the Virtual PC / SoftWindows mouse driver problems
		 // but requires that I also read the mouse buttons since Windows no longer sends mouse event messages
		 result = direct_input_mouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
		 if (SUCCEEDED(result)) mouse_exclusive = TRUE;
	 } else {
		 result = direct_input_mouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	 };
    if (FAILED(result)) {
        tt_error_file() << "DirectInput. SetCooperativeLevel failed with result=" << result << endl;
        return(FALSE);
    };
 #if TT_DEBUG_ON
    add_to_log_if_interesting_error();
#endif
    result = direct_input_mouse->Acquire();
	 if (FAILED(result)) { // new on 150404
#if TT_DEBUG_ON
		 // beginning 070904 this warning is only for debug version
		 add_to_log_if_interesting_error();
		 tt_error_file() << "DirectInput. Failed to acquire access to mouse. Result code=" << result << endl;
#endif
	 };
	 initialize_touch_senstive_mouse(); // new on 090801 - does nothing if there isn't one
    // Enumerate the joystick devices.
    // tried (tt_debug_mode==2)?0:DIDEVTYPE_JOYSTICK but got keyboard and mouse and didn't work (were all zeroes)
    tt_joystick_count = 0;
    direct_input->EnumDevices(DIDEVTYPE_JOYSTICK,
                              initialize_joystick_input, direct_input, DIEDFL_ATTACHEDONLY);
    if (tt_joystick_count > 0) {
       tt_error_file() << "The number of attached joysticks is " << (int) tt_joystick_count << endl;
    };
    if (tt_force_feedback_joystick_count > 0) {
       tt_error_file() << "of which " << (int) tt_joystick_count << " are force feedback joysticks." << endl;
       if (iforce2_libary != NULL) {
//#if !TT_MICROSOFT
          CreateEffects = (IFCreateEffectsType) GetProcAddress(iforce2_libary, (LPCSTR) 2);
          LoadProjectFile = (IFLoadProjectFileType) GetProcAddress(iforce2_libary, (LPCSTR) 3); //"IFLoadProjectFile");
          ReleaseEffects = (IFReleaseEffectsType) GetProcAddress(iforce2_libary, (LPCSTR) 8);
          ReleaseProject = (IFReleaseProjectType) GetProcAddress(iforce2_libary, (LPCSTR) 9);
//#endif
          FileNameStatus file_status;
          ascii_string force_resource_file_name = existing_file_name(file_status,"forces","ifr");
          if (force_resource_file_name != NULL) {
             read_force_resource_names(force_resource_file_name);
             force_projects = new HIFORCEPROJECT[tt_force_feedback_joystick_count+tt_force_sensitive_mouse_count];
             for (int i = 0; i < tt_force_feedback_joystick_count+tt_force_sensitive_mouse_count; i++) {
					 if (i > 0 || tt_force_sensitive_mouse_count == 0) {
						 force_projects[i] = LoadProjectFile(force_resource_file_name,direct_input_force_feedback_joystick[i-tt_force_sensitive_mouse_count]);
					 } else {
						 force_projects[i] = NULL; // just in case
					 };
             };
				 delete [] force_resource_file_name; // moved here on 140801 from outside the conditional
          };        
       };
    } else if (tt_force_sensitive_mouse_count > 0) {
		 FileNameStatus file_status;
       ascii_string force_resource_file_name = existing_file_name(file_status,"forces","ifr");
       if (force_resource_file_name != NULL) {
          read_force_resource_names(force_resource_file_name);
			 delete [] force_resource_file_name; // added 140801
		 };
	 };
    return(TRUE);
};

void release_direct_input() {
    /*
     *  Destroy any lingering IDirectInputDevice object.
     */
    if (direct_input_mouse != NULL) {
        /*
         *  Cleanliness is next to godliness.  Unacquire the device
         *  one last time just in case we got really confused and tried
         *  to exit while the device is still acquired.
         */
        direct_input_mouse->Unacquire();
        direct_input_mouse->Release();
        direct_input_mouse = NULL;
    };
    if (direct_input_joystick != NULL) {
       for (int i = 0; i < tt_joystick_count; i++) {
          if (direct_input_joystick[i] != NULL) {
             direct_input_joystick[i]->Unacquire();
             direct_input_joystick[i]->Release();
             direct_input_joystick[i] = NULL;
          };
       };
       delete [] direct_input_joystick;
    };
    if (direct_input_force_feedback_joystick != NULL) delete [] direct_input_force_feedback_joystick;
    /*
     *  Destroy any lingering IDirectInput object.
     */
    if (iforce2_libary != NULL) {
//       if (force_feedback_effects != NULL) {
//          ReleaseEffects(force_projects[users_force_joystick_index],force_feedback_effects);
//       };
	    int i;
       for (i = 0; i < tt_force_feedback_joystick_count+tt_force_sensitive_mouse_count; i++) {
			 if (i > 0 || tt_force_sensitive_mouse_count == 0) {
             ReleaseProject(force_projects[i]);
			 };
       };
       FreeLibrary(iforce2_libary);
       iforce2_libary = NULL;
       if (force_projects != NULL) delete [] force_projects;
       force_projects = NULL;
       for (i = 0; i < force_count; i++) {
          delete [] force_names[i];
       };
       delete [] force_names;
    };
    if (effect_description.rgdwAxes == NULL) delete [] effect_description.rgdwAxes;
    if (effect_description.rglDirection == NULL) delete [] effect_description.rglDirection;
	 // moved to after releasing forces since still need direct_input around for that (230300)
    if (direct_input) {
        direct_input->Release();
        direct_input = NULL;
    };
    if (direct_input_library != NULL) FreeLibrary(direct_input_library);
};

long joystick_x_zero = min_long, joystick_y_zero = min_long;

flag left_button_reported_down = FALSE;
flag right_button_reported_down = FALSE;
flag middle_button_reported_down = FALSE;

void direct_input_mouse_delta(long &delta_x, long &delta_y) {
#if TT_DEBUG_ON
	 if (tt_debug_mode == 290602) {
		 if (direct_input_mouse == NULL) {
			 log("No direct_input_mouse");
		 };
	 };
#endif
    if (direct_input_mouse != NULL) {
        DIMOUSESTATE state;          /* DirectInput mouse state structure */
        HRESULT result;
        result = direct_input_mouse->GetDeviceState(sizeof(DIMOUSESTATE), &state);
        if (result == DIERR_INPUTLOST) {
            /*
             *  DirectInput is telling us that the input stream has
             *  been interrupted.  We aren't tracking any state
             *  between polls, so we don't have any special reset
             *  that needs to be done.  We just re-acquire and
             *  try again.
             */
            result = direct_input_mouse->Acquire();
            if (SUCCEEDED(result)) {  // try again
               direct_input_mouse_delta(delta_x,delta_y);
            };
        }
        if (SUCCEEDED(result)) {
//#if TT_DEBUG_ON
           if (tt_debug_mode == 1100 || tt_debug_mode == 290602) {
              tt_error_file() << "Just read mouse's position using Direct Input as " << state.lX << "," << state.lY << endl;
           };
//#endif
			  if (tt_mouse_acquired) { // new on 151099
              // what about Z??
              delta_x += state.lX;
              delta_y -= state.lY;
				  if (mouse_exclusive) { // new on 130800
					  if (state.rgbButtons[0]&0x80) {
						  if (!left_button_reported_down) {
							  tt_main_window->mouse_button_just_went_down(LEFT_BUTTON);
							  left_button_reported_down = TRUE;
						  };
					  } else if (left_button_reported_down) {
						  tt_main_window->a_mouse_button_is_now_up();
						  left_button_reported_down = FALSE;
					  };
					  if (state.rgbButtons[1]&0x80) {
						  if (!right_button_reported_down) {
							  tt_main_window->mouse_button_just_went_down(RIGHT_BUTTON);
							  right_button_reported_down = TRUE;
						  };
					  } else if (right_button_reported_down) {
						  tt_main_window->a_mouse_button_is_now_up();
						  right_button_reported_down = FALSE;
					  };
					  if (state.rgbButtons[2]&0x80) {
						  if (!middle_button_reported_down) {
						     tt_main_window->mouse_button_just_went_down(MIDDLE_BUTTON);
							  middle_button_reported_down = TRUE;
						  };
					  } else if (middle_button_reported_down) {
						  tt_main_window->a_mouse_button_is_now_up();
						  middle_button_reported_down = FALSE;
					  };
				  };
			  };
        };
    };
};

void release_mouse_capture() {
	if (mouse_exclusive) { // new on 140800
		HRESULT result;
		result = direct_input_mouse->Unacquire();
		if (FAILED(result)) {
			tt_error_file() << "Failed to unacquire the mouse. Error code is " << (int) result << endl;
		};
		result = direct_input_mouse->SetCooperativeLevel(tt_main_window->get_handle(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		if (FAILED(result)) {
			tt_error_file() << "Failed to set the cooperative level of the mouse to non-exclusive. Error code is " << (int) result << endl;
		};
		left_button_reported_down = FALSE; // these were added on 170800
		right_button_reported_down = FALSE;
		middle_button_reported_down = FALSE;
	};
	::ReleaseCapture(); 
};

void capture_mouse() {
	HWND hwnd = tt_main_window->get_handle();
	::SetForegroundWindow(hwnd);  // new on 230800
	if (::SetCapture(hwnd) == NULL) { // probably not need if running exclusively as below
//#if TT_DEBUG_ON
//		tt_error_file() << "Warning: failed to capture the mouse." << endl;
//#endif
	};
	if (mouse_exclusive) { // new on 140800
		HRESULT result = direct_input_mouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
		if (FAILED(result)) {
			tt_error_file() << "Failed to set the cooperative level of the mouse to exclusive. Error code is " << (int) result << endl;
		};
		result = direct_input_mouse->Acquire();
		if (FAILED(result)) {
			tt_error_file() << "Failed to acquire the mouse. Error code is " << (int) result << endl;
		};
	};
};

void update_joystick_button_still_down() {
   // called at the end of a cycle to maintain a bit as to whether a button down is "news"
   for (int joy = 0; joy < tt_joystick_count; joy++) {
      joystick_button_still_down_masks[joy] = 0;
      for (int button = 0; button < joystick_button_count; button++) {
          if (joystick_button(joy,button)) {
              joystick_button_still_down_masks[joy] |= (1<<button);
          };
      };
   };
};

boolean read_joystick_states() {
   boolean first_joystick_ok = FALSE;
   for (int i = 0; i < tt_joystick_count; i++) {
      if (direct_input_joystick[i] == NULL) break; // lost it due to some error
      HRESULT result = direct_input_joystick[i]->Poll();
      if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
            /*
             *  DirectInput is telling us that the input stream has
             *  been interrupted.  We aren't tracking any state
             *  between polls, so we don't have any special reset
             *  that needs to be done.  We just re-acquire and
             *  try again.
             */
          result = direct_input_joystick[i]->Acquire();
          if (SUCCEEDED(result)) {  // try again -- should I worry that I only try again once??
             return(read_joystick_states());			 
          };
      } else if (!SUCCEEDED(result)) {
          tt_error_file() << "Problems polling joystick #" << i << endl;
          if (result == DIERR_NOTINITIALIZED) { // how is that possible?
				 tt_error_file() << "Joystick not initialized." << endl;
			 };
          direct_input_joystick[i] = NULL; // release it first?
          return(FALSE);
      };
      result = direct_input_joystick[i]->GetDeviceState(sizeof(DIJOYSTATE), &joystick_state[i]);
      if (!SUCCEEDED(result)) return(FALSE);
      if (i == 0) first_joystick_ok = TRUE;
   };
   return(first_joystick_ok);
};

char previous_left_button = 0, previous_middle_button = 0, previous_right_button = 0;
flag ignore_left_button = FALSE, ignore_middle_button = FALSE, ignore_right_button = FALSE;

void direct_input_read_joysticks(long &delta_x, long &delta_y) {
   // is this in the right place??
   if (read_joystick_states()) {
//    if (tt_millisecond_delta > 0) {
      // what about Z??
      // if sensitivity is 100 then it should take 1 second to cross horizontally
      // and 3/4 second vertically at full speed
      city_coordinate joystick_delta_x = (tt_joystick_sensitivity*joystick_x(0)*tt_millisecond_delta)/(1000*100);
      city_coordinate joystick_delta_y = (tt_joystick_sensitivity*joystick_y(0)*tt_millisecond_delta)/(-1000*100);
      if (!absolute_mouse_mode()) {
         delta_x += joystick_delta_x;
         delta_y += joystick_delta_y;
      } else if (joystick_delta_x != 0 || joystick_delta_y != 0) {
         delta_x = joystick_delta_x;
         delta_y = joystick_delta_y;
         tt_hand_needs_to_move_relatively = TRUE; // new on 220402 for absolute mouse mode
      }; // otherwise "ignore" the joystick (except for buttons)
#if TT_DEBUG_ON
      if (tt_debug_mode == 709) {
         tt_error_file() << "Joystick x is " << joystick_x(0) << " and y is " << joystick_y(0) << endl;
         tt_error_file() << "Joystick delta x is " << (tt_joystick_sensitivity*joystick_x(0)*tt_millisecond_delta)/(1000*100)
                     << " and delta y is " << (tt_joystick_sensitivity*joystick_y(0)*tt_millisecond_delta)/(1000*100)
                     << " where delta time is " << tt_millisecond_delta << endl;
         for (int i = 0; i < joystick_button_count; i++) {
            if (joystick_button(0,i)) tt_error_file() << "And button #" << i << " is down." << endl;
         };
      };
#endif
// } else { // first time
//    if (abs(state.lX) == max_joystick_x && abs(state.lY) == max_joystick_y) {  // probably not connected
//       tt_error_file() << "Joystick not connected." << endl;
//       direct_input_joystick = NULL; // release it first?
//    };
// };
      // left, middle, right names here are goofy but it is what they correspond to with mouse and ToonTalk
      boolean left_button_down = joystick_button(0,0);
      boolean middle_button_down = joystick_button(0,1);
      boolean right_button_down = joystick_button(0,2);
      if (!right_button_down) { // for now map all other buttons to this one
         for (int i = 3; i < joystick_button_count; i++) {
            if (joystick_button(0,i)) {
               right_button_down = TRUE;
               break;
            };
         };
      };
      if (ignore_left_button) {
         if (!left_button_down) ignore_left_button = FALSE; // done ignoring it
      } else if (left_button_down) {
         if (tt_mouse_up_simulated) {  // pretend that button has been released
            ignore_left_button = TRUE;
            previous_left_button = 0;
         } else if (!ignore_left_button) {
            add_button_event(JOYSTICK_DEVICE,LEFT_BUTTON|previous_left_button);
            previous_left_button = BUTTON_STILL_DOWN;
         };
      } else if (previous_left_button == BUTTON_STILL_DOWN) {  // not any longer
         previous_left_button = 0;
         ignore_left_button = FALSE;
      };
      if (ignore_middle_button) {
         if (!middle_button_down) ignore_middle_button = FALSE; // done ignoring it
      } else if (middle_button_down) {
         if (tt_mouse_up_simulated) {  // pretend that button has been released
            ignore_middle_button = TRUE;
            previous_middle_button = 0;
         } else if (!ignore_middle_button) {
            add_button_event(JOYSTICK_DEVICE,MIDDLE_BUTTON|previous_middle_button|SHIFT_KEY_DOWN);
            previous_middle_button = BUTTON_STILL_DOWN;
         };
      } else if (previous_middle_button == BUTTON_STILL_DOWN) {  // not any longer
         previous_middle_button = 0;
         ignore_middle_button = FALSE;
      };
      if (ignore_right_button) {
         if (!right_button_down) ignore_right_button = FALSE; // done ignoring it
      } else if (right_button_down) {
         if (tt_mouse_up_simulated) {  // pretend that button has been released
            ignore_right_button = TRUE;
            previous_right_button = 0;
         } else if (!ignore_right_button) {
            // CONTROL_KEY_DOWN is so that even if only all mouse buttons are mapped to left
            // that doesn't happen with joystick buttons
            add_button_event(JOYSTICK_DEVICE,RIGHT_BUTTON|previous_right_button|CONTROL_KEY_DOWN);
            previous_right_button = BUTTON_STILL_DOWN;
         };
      } else if (previous_right_button == BUTTON_STILL_DOWN) {  // not any longer
         previous_right_button = 0;
         ignore_right_button = FALSE;
      };
   };
};

int first_joystick_button_down(int joystick_number) {
   for (int i = 0; i < joystick_button_count; i++) {
      if (joystick_button(joystick_number,i)) return(i);
   };
   return(-1); // didn't find one
};

int first_new_joystick_button_down(int joystick_number) {
   for (int i = 0; i < joystick_button_count; i++) {
      if (joystick_button(joystick_number,i) && !joystick_button_still_down(joystick_number,i)) return(i);
   };
   return(-1); // didn't find one
};

void unacquire_input_devices() {
#if TT_DEBUG_ON
	if (tt_debug_mode == 290602) {
		log("unacquire_input_devices");
	};
#endif
   if (direct_input_mouse != NULL) direct_input_mouse->Unacquire();
   if (direct_input_joystick != NULL) {
      for (int i = 0; i < max_number_of_joysticks; i++) {
          if (direct_input_joystick[i] != NULL) direct_input_joystick[i]->Unacquire();
      };
   };
};

void acquire_input_devices() {
#if TT_DEBUG_ON
	if (tt_debug_mode == 290602) {
		log("acquire_input_devices");
	};
#endif
   if (direct_input_mouse != NULL) direct_input_mouse->Acquire();
   if (direct_input_joystick != NULL) {
      for (int i = 0; i < max_number_of_joysticks; i++) {
          if (direct_input_joystick[i] != NULL) direct_input_joystick[i]->Acquire();
      };
   };
};

#if TT_DEBUG_ON
boolean joystick_button(int joystick_number, int button_number) {
   return((boolean) (joystick_state[joystick_number].rgbButtons[button_number] & 0x80));
};
boolean joystick_button_still_down(int joystick_number, int joystick_button_number) {
   return((boolean) (joystick_button_still_down_masks[joystick_number]&(1<<joystick_button_number)));
};
#endif


  /*

  BOOL  EffectFound = FALSE;  // global flag

BOOL CALLBACK DIEnumEffectsProc(LPCDIEFFECTINFO pei, LPVOID pv)
  {
  *((GUID *)pv) = pei->guid;
  EffectFound = TRUE;
  return DIENUM_STOP;  // one is enough
  }

//  LPDIEFFECT  g_lpdiEffect;  // didn't compile
IDirectInputEffect *g_lpdiEffect;  // global effect object for testing

boolean test_force_feedback(int joystick_number) {
 // The GUID variable pointed to by the application-defined value pv is assigned the value passed in the DIEFFECTINFO structure created by DirectInput for the effect.
 // In order to obtain the effect GUID, you set the callback in motion by calling the IDirectInputDevice2::EnumEffects method, as follows:

  HRESULT  hr;
  GUID     guidEffect;
  hr = direct_input_force_feedback_joystick[joystick_number]->EnumEffects(
                  (LPDIENUMEFFECTSCALLBACK) DIEnumEffectsProc,
                   &guidEffect,
                   DIEFT_PERIODIC);
  if (FAILED(hr)) {
     tt_error_file() << "Effect enumeration failed\n" << endl;
     // Note: success doesn't mean any effects were found,
     // only that the process went smoothly.
     return(FALSE);
  }
  if (!EffectFound) return(FALSE);
  DWORD      dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
  LONG       lDirection[2] = { 0, 0 };
 
  DIPERIODIC diPeriodic;      // type-specific parameters
  DIENVELOPE diEnvelope;      // envelope
  DIEFFECT   diEffect;        // general parameters


  // Now initialize the type-specific parameters. If you use the values in the example, you will create a full-force periodic effect with a period of one-twentieth of a second.

  diPeriodic.dwMagnitude = DI_FFNOMINALMAX;
  diPeriodic.lOffset = 0;
  diPeriodic.dwPhase = 0;
  diPeriodic.dwPeriod = (DWORD) (0.05 * DI_SECONDS);


  // To get the effect of the chain-saw motor trying to start, briefly coughing into life, and then slowly dying, you will set an envelope with an attack time of half a second and a fade time of one second. You'll get to the sustain value in a moment.

  diEnvelope.dwSize = sizeof(DIENVELOPE);
  diEnvelope.dwAttackLevel = 0;
  diEnvelope.dwAttackTime = (DWORD) (0.5 * DI_SECONDS);
  diEnvelope.dwFadeLevel = 0;
  diEnvelope.dwFadeTime = (DWORD) (1.0 * DI_SECONDS);


  // Now you set up the basic effect parameters. These include flags to determine how the directions and device objects (buttons and axes) are identified, the sample period and gain for the effect, and pointers to the other data that you have just prepared. You also associate the effect with the fire button of the joystick, so that it will automatically be played whenever that button is pressed.

  diEffect.dwSize = sizeof(DIEFFECT);
  diEffect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS;
  diEffect.dwDuration = INFINITE; // (DWORD) (2 * DI_SECONDS);

  diEffect.dwSamplePeriod = 0;               // = default
  diEffect.dwGain = DI_FFNOMINALMAX;         // no scaling
  diEffect.dwTriggerButton = DIEB_NOTRIGGER; // DIJOFS_BUTTON0;
  diEffect.dwTriggerRepeatInterval = 0;
  diEffect.cAxes = 2;
  diEffect.rgdwAxes = &dwAxes[0];
  diEffect.rglDirection = &lDirection[0];
  diEffect.lpEnvelope = &diEnvelope;

  diEffect.cbTypeSpecificParams = sizeof(diPeriodic);
  diEffect.lpvTypeSpecificParams = &diPeriodic;
 
  // So much for the setup. At last you can create the effect:

  hr = direct_input_force_feedback_joystick[joystick_number]->CreateEffect(
                     guidEffect,     // GUID from enumeration
                     &diEffect,      // where the data is
                     &g_lpdiEffect,  // where to put interface pointer
                     NULL);          // no aggregation
  if (FAILED(hr)) {
    tt_error_file() << "Failed to create periodic effect";
    return(FALSE);
  };
  return(TRUE);
  // Remember that, by default, the effect is downloaded to the device as soon as it has been created, provided that the device is in an acquired state at the exclusive cooperative level. So if everything has gone according to plan, you should be able to compile, run, press the "fire" button, and feel the sputtering of a chain saw that's out of gas.
};
*/

#endif




