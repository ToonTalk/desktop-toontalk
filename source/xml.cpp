// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.
// XML DOM code - started with MSDN sample code

class Picture; // experiment on 231102

#include "stdafx.h" // new on 281002 for mini-dumps

//#include "flags.h" // commented out son 231102 ince included in defs.h

#include <tchar.h>
//double _wtof(const wchar_t *); // seems to be need on my Win98 laptop


#import "msxml4.dll"
//using namespace MSXML2;
#include <atlbase.h> // Includes CComVariant and CComBSTR.

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif

//#if TT_XML // moved here on 231102 so defs.h can set this flag since flags.h not loaded first -- commented out on 231102

#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif
#if !defined(__TT_TEXT_H)   
#include "text.h"
#endif
#if !defined(__TT_CUBBY_H)   
#include "cubby.h"
#endif
#if !defined(__TT_BIRD_H)   
#include "bird.h"
#endif
#if !defined(__TT_NUMBER_H)   
#include "number.h"
#endif
#if !defined(__TT_PAD_H)   
#include "pad.h"
#endif
#if !defined(__TT_SOUND_H)   
#include "sound.h"
#endif
#if !defined(__TT_TRUCK_H)   
#include "truck.h"
#endif
#if !defined(__TT_BOMB_H)   
#include "bomb.h"
#endif
#if !defined(__TT_ROBOT_H)   
#include "robot.h"
#endif
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif
#if !defined(__TT_PUZZLE_H)   
#include "puzzle.h"
#endif
#if !defined(__TT_MARTIAN_H)   
#include "martian.h"
#endif
#if !defined(__TT_HELP_H)   
#include "help.h"
#endif
#if !defined(__TT_COMMON_H)   
#include "common.h"
#endif
#if !defined(__TT_ZIP_H)   
#include "zip.h"
#endif
#if !defined(__TT_FILE_H)   
#include "file.h"
#endif
#if !defined(__TT_LOG_H)   
#include "log.h"
#endif
#if !defined(__TT_DIRTY_H)   
#include "dirty.h"
#endif
#if !defined(__TT_WINMAIN_H)   
#include "winmain.h"
#endif
#if !defined(__TT_XML_H)   
#include "xml.h"
#endif

#include <wchar.h> // for wmemcpy

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// must keep these in correspondence with enum Tag
wide_string tag_string[] = {L"Box", L"TextPad", L"Picture", L"PictureOf", L"UserPicture", L"Nest", L"Bird", L"Scale", 
                            // L"Integer32Bit", 
                            L"Integer", L"FloatingPointNumber64Bit", L"RationalNumber", 
                            L"RemoteControl", L"Notebook",
                            L"BuiltInSound", L"ForceFeeback", L"Speech", L"Sound", L"MCISound",
//                            L"ForceRemoteControl", 
                            L"Robot", L"Bomb", L"Truck", L"Toolbox", L"Vacuum", L"BikePump", L"MagicWand",
									 L"TruckOutsideViewedFromSide", L"TruckOutsideViewedFromAbove",
                            L"Martian", L"TalkBalloon",
                            L"ForeignBird", L"ToonTalkObject",
//									 L"Include", // new on 080703 -- made an attribute on 090703
									 L"MissingPictureFile", // new on 120703
                            L"Link",
                            L"Left", L"Bottom", L"Width", L"Height",
                            L"InfiniteStack", L"Invisible", L"SeeAll", L"SeeSome", L"Erased",
                            L"Geometry", // L"SpecialSettings",
                            L"Hole", L"Label", L"NumberOfHoles", // cubby
                            L"TextValue", L"ColorIndex", L"VariableWidthFont", // text pads
                            L"OnTop", L"NestCopies", L"MyNest", L"TemporarilyRemovedFromOnTop", 
									 L"OriginalNest", // new on 180903
                            L"Address", L"ID", L"ForwardsToRealNest",
									 L"MergedWith", L"NestsWhoseBirdsForwardHere", // new on 140205
                            L"BuiltIn", L"Shape", L"Button",
                            L"Next", L"MaximumTrainingSteps", L"ToolInHand", L"WorkingInfo", L"MadeOrMovedSoFar", 
                            L"FirstTeammateToFail", L"NextAfterRunning", L"ThoughtBubble", 
                            L"GluedDown", L"Active", L"PartlyVisibleIfPartOfPicture", L"FlippedOver", L"NotFlippedOver",
                            L"VolatileAttributes", // new on 211102 - renamed 181202
                            L"TopOfStack", L"NotifyWhenDone",
                            L"GeometryWhenFreeAndUnflipped", L"SpeedToRight", L"SpeedToTop", L"OnFront", L"OnBack", L"GeometricRelationToContainer",             
                            L"For", L"Value", L"OperationsAppliedToValue", L"Language",
                            L"Operation", L"NextOperation",
                            L"ToonTalkComment",
                            L"OpenToPage", L"Page", // L"LivesInTooly", 
                            L"FileName", L"NumberOfPages", L"RemoteControlsFor",
									 L"PageBeingDefined", // new on 171203
                            L"Appearance", 
                            L"GlobalSensor", L"JoystickSensor", L"ForceFeedbackSensor",
                            L"Maker", L"MessageToMaker", // foreign birds
                            L"NotebookICameFrom", L"NotebookThatWasInTruckWithMe", // home and team notebooks
                            L"City", L"Inside", // L"OnTheGround", 
									 L"Running", L"Size", L"MaximumHousesInABlock", L"GlobalPicture",
									 L"PicturesToRunWhenNotStopped", // new on 220704
                            L"Block", L"On", 
                            L"House", L"OnRoof", L"OnFrontWall",
                            L"Room", L"OnWall", // L"Inside", -- since also used by City
                            L"Floor", L"OnFloor", L"MainBox", L"InitialRobot", L"AlternativesForMyBox",
									 L"Ground",
                            L"Queue", // L"Waiting",
                            L"Programmer", L"OriginalBox", L"RecentMovement", L"InThoughtBubble", L"EventsSoFar", L"InHand",
									 L"Screen",
//									 L"ProgrammerAppearance",
									 L"ProgrammerAtFloor", L"ProgrammerTitlesFlying", L"ProgrammerCityFlying",
									 L"ProgrammerCityLanding", L"ProgrammerCityWalking",L"ProgrammerRoomWalking",
									 // following new on 050103 for programmer at floor
									 L"ThoughtBubbleEntered", L"OriginalThoughtBubble", L"ThoughtBubbleRobot", 
									 L"Hand", L"HandHoldingWand", L"Arm", L"CurrentSelection", L"SavedSelection", L"LastReleased",
									 L"BoxThatStartedTraining", // L"WaitingFor",
									 L"ToolboxViewedFromSide", L"PersonWalking", L"PersonSitting", L"PersonSittingWithHat",
									 L"PersonSittingWithHair", L"DizzyPerson", L"DizzyPersonWithHair", L"DizzyPersonWithHat",
									 L"LandingHelicopter", L"FlyingHelicopter", L"Explosion",
                            // L"FriendOfTooly", 
                            L"Sucked", L"Nothing",  
                            L"Counter", L"Attached",
                            L"PotentialHelpTarget", L"TeleportInSelection", L"TeleportInToolInHand", L"PreviousTarget", L"Subject", // L"Balloon", // for Marty
                            L"PendingMessage", L"AlternativeSpokenPendingMessage", L"LastThingSaid0", L"LastThingSaid1",
                            L"RecentSentence", L"Text", L"AlternativeSpokenText",
                            L"Notify", L"GoalHolder", L"VariableHolder",
                            L"DoAfter", L"AnimateToStaticGoal", L"AnimateToDynamicGoal",
                            L"TurningNotebookPage", L"PageNumberItem", // for notebooks - new on 181102
									 L"LeftPage", L"RightPage", // new on 250803 - obsolete on 260803 - reused on 280803
									 L"SubPicturesWithNotebooks", L"SavedPictureOf", L"ItemToAdd", L"RemoteNotebook", // for Pictures
									 L"NewContents", L"RobotToTry", L"BoxToTryRobotOn", L"WaitingBirds", L"HatchingBird", L"PendingMessages", // for nests - new on 051202
									 L"ItemToDeliver", L"TopOfStackOnNest", L"ReturnCubby", L"ReturnPath", L"SavedIem", L"WaitingFor", // for birds - new on 051202
                            L"HouseLocation", // L"ReturnFloor", // renamed on 201202 to be more accurate and general
									 L"Animation", L"ItemGoingIntoHole",
									 L"Mouse", L"ItemToSmash", L"ItemUnderneath", L"OriginalSmashee", L"PreviousOverItem", L"PreviousMouse",
									 L"PreviousUnderItemEncoding", L"PreviousOverItemEncoding",
									 L"PageOffset", L"CurrentToolInHand", L"CurrentTarget", L"SavedSubject", L"RobotToPutBehind", // L"SavedStack", 
									 L"SuspendedOn", // for robots
									 L"NextToolInHand", // new on 261003
									 L"SavedParts",
									 L"BeingAdded", // for trucks
									 L"BeingSucked", // new on 140703
									 // obsolete:
                            L"Stopped", L"ThingsOnTop", // handle specially 011102
									 L"SpecialAttributes", // renamed
                            // now for robot actions:
                            L"Actions",
//                            L"MadeOrMovedCount", -- is an attribute now
                            L"MoveTo", L"MadeOrMoved",
                            L"Grasp", L"Use", L"UseOn", L"Drop", L"DropOn", L"DropOnLeftSide",L"DropOnRightSide",
	                         L"TypeTo", L"Hatch", L"GiveBird", L"DropInTruck",
	                         L"BlowUpHouse", L"TypeLabel", L"SelectAndType", L"GraspNest", L"ReplaceMyBox",
	                         L"UseMagicWandOnNest", L"UseVacuumOnNest", L"UseVacuumInReverse",
									 // following new (or restored) on 170603
									 L"GraspMagicWand", L"UseMagicWand", L"GraspBikePump", L"DropMagicWand", L"UseBikePump",
									 L"GraspVacuum", L"UseVacuum", L"DropVacuum", L"DropBikePump",
									 L"SoundFile", L"TextToSpeech" // new on 281003 for demos and time travel
};

wide_string tag_to_wide_string(Tag tag_code) { 
   return(tag_string[tag_code]);
};

typedef Tag *Tag_table;
typedef wide_string *wide_string_table;

const int max_tag_length = 32; // should be OK

wide_string_table *tag_string_table; // tag strings sorted into buckets by length
Tag_table *tag_table;
int *tag_count;

void intialize_xml_tag_table() {
   tag_string_table = new wide_string_table[max_tag_length];
   tag_table = new Tag_table[max_tag_length];
   tag_count = new int[max_tag_length];
	int i;
   for (i = 0; i < max_tag_length; i++) {
      tag_count[i] = 0;
      tag_string_table[i] = NULL;
   };
   for (i = 0; i < NO_SUCH_TAG; i++) {
      int size = wide_string_length(tag_string[i]);
      if (size >= max_tag_length) {
         ::MessageBox(NULL,"Fix max_tag_length","intialize_xml_tag_table",MB_OK);
         return;
      };
      int old_tag_count = tag_count[size];
      int new_tag_count = old_tag_count+1; // got a new one
      tag_count[size] = new_tag_count; 
      wide_string *new_bucket = new wide_string[new_tag_count];
      Tag *new_tag_bucket = new Tag[new_tag_count];
      wide_string *old_bucket = tag_string_table[size];
      Tag *old_tag_bucket = tag_table[size];
      if (old_bucket != NULL) {
         for (int i = 0; i < old_tag_count; i++) { // transfer old to new bucket
            new_bucket[i] = old_bucket[i];
            new_tag_bucket[i] = old_tag_bucket[i];
         };
         delete [] old_bucket;
         delete [] old_tag_bucket;
      };
      new_bucket[old_tag_count] = tag_string[i];
      new_tag_bucket[old_tag_count] = (Tag) i;
      tag_string_table[size] = new_bucket;
      tag_table[size] = new_tag_bucket;
   };
};

void release_xml_tag_table() { 
   for (int i = 0; i < max_tag_length; i++) {
      if (tag_count[i] > 0) {
         delete [] tag_string_table[i];
         delete [] tag_table[i];
      };
   };
   delete [] tag_string_table;
   delete [] tag_table;
   delete [] tag_count;
};
  
Tag tag_token_from_string(BSTR tag_string) {
   if (tag_string == NULL) return(NO_SUCH_TAG);
 //  int size = wide_string_length(tag_string);
   int size = ::SysStringLen(tag_string); // this is just a look up rather than a scan
   if (size >= max_tag_length) {
      return(NO_SUCH_TAG);  // warn??
   };
   for (int i = 0; i < tag_count[size]; i++) {
      if (compare_wide_strings(tag_string_table[size][i],tag_string) == 0) {
         return(tag_table[size][i]);
      };
   };
	if (compare_wide_strings(L"FlyingHelicoopter",tag_string) == 0) { // and old misspelling caught on 120703 - here to deal with "legacy" code
		return(FLYING_HELICOPTER_TAG);
	};
	string narrow_tag_string = copy_narrow_string(tag_string);
	tt_error_file() << "This version of ToonTalk doesn't know the XML tag: " << narrow_tag_string << endl; 
	// warning new on 060405
	delete [] narrow_tag_string;
   return(NO_SUCH_TAG);
};

Tag tag_token(xml_node *element) {
   if (element == NULL) return(NO_SUCH_TAG);
   BSTR tag_string;
   if (FAILED(element->get_nodeName(&tag_string))) {
      return(NO_SUCH_TAG);  // warn??
   };
   Tag result = tag_token_from_string(tag_string);
	//if (tt_debug_mode == 240103) {
	//	static int tag_count = 0;
	//	tt_error_file() << "Processing " << copy_narrow_string(tag_string) << " #" << tag_count++ << endl;
	//};
//	tt_error_file() << "Processing tag " << copy_narrow_string(tag_string) << endl; // REMOVE 201003
   ::SysFreeString(tag_string);
   return(result);
};

xml_node *first_node_that_is_an_element(xml_node *parent) {
	// changed on 190703 to return xml_element * rather than xml_node * - must use QueryInterface
   if (parent == NULL) return(NULL);
   xml_node *node;
	if (parent->get_firstChild(&node) != S_OK) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 201003) {
			xml_debug_node(parent,L"No firstChild");
		};
#endif
		return(NULL);
	};
   if (node == NULL) return(NULL);
   DOMNodeType type;
   if (node->get_nodeType(&type) != S_OK) {
      return(NULL); // report problem?
   };
   while (type != NODE_ELEMENT) {
		if (node->get_nextSibling(&node) == S_FALSE) {
//#if TT_DEBUG_ON
//			xml_debug_node(parent,L"Parent in first_node_that_is_an_element");
//#endif
			return(NULL);
		};
      if (FAILED(node->get_nodeType(&type))) {
#if TT_DEBUG_ON
			xml_debug_node(parent,L"Parent in first_node_that_is_an_element");
#endif
         return(NULL); // report problem?
      };
   };
   return(node); 
};

xml_element *first_element_with_tag_name(xml_node *parent, wide_string tag_string) {
	// new on 200703 -- based upon first_node_that_is_an_element
   if (parent == NULL) return(NULL);
   xml_node *node;
   if (parent->get_firstChild(&node) != S_OK) return(NULL);
   if (node == NULL) return(NULL);
   DOMNodeType type;
   if (node->get_nodeType(&type) != S_OK) {
      return(NULL); // report problem?
   };
   while (TRUE) {
		if (type != NODE_ELEMENT) {
			if (node->get_nextSibling(&node) == S_FALSE) return(NULL);
			if (FAILED(node->get_nodeType(&type))) {
				return(NULL); // report problem?
			};
		};
		xml_element *element = xml_node_to_element(node);
		BSTR tag_name;
		element->get_tagName(&tag_name);
		boolean ok = (compare_wide_strings(tag_name,tag_string) == 0);
		::SysFreeString(tag_name);
		if (ok) { // could use compare_wide_strings_of_length and be more clever
			return(element);
		};
		if (node->get_nextSibling(&node) == S_FALSE) {
			return(NULL); // new on 300105
		};
   };
   return(NULL); 
};

xml_element *xml_node_to_element(xml_node *node) { // new on 190703
	// caller should know what they are doing
	// based upon http://groups.google.com/groups?hl=en&lr=&ie=UTF-8&oe=UTF-8&threadm=8sbi8m%2418l4%241%40news5.isdnet.net&rnum=4&prev=/groups%3Fq%3Dget_nodeType%2Belement%26hl%3Den%26lr%3D%26ie%3DUTF-8%26oe%3DUTF-8%26selm%3D8sbi8m%252418l4%25241%2540news5.isdnet.net%26rnum%3D4
	xml_element *element;
	node->QueryInterface(__uuidof(IXMLDOMElement), (void **) &element); 
   return(element); 
};

xml_node *first_node_that_is_text(xml_node *parent, DOMNodeType &type) {
   if (parent == NULL) return(NULL);
   xml_node *node;
   if (FAILED(parent->get_firstChild(&node))) return(NULL);
   if (node == NULL) return(NULL);
//   DOMNodeType type;
   if (FAILED(node->get_nodeType(&type))) {
      return(NULL); // report problem?
   };
   while (type != NODE_TEXT && type != NODE_CDATA_SECTION) {
      if (node->get_nextSibling(&node) == S_FALSE) return(NULL);
      //if (FAILED(result)) {
      //   return(NULL); // report problem?
      //};
      if (FAILED(node->get_nodeType(&type))) {
         return(NULL); // report problem?
      };
   };
   return(node);
};

BSTR xml_get_text(xml_node *node) { 
   // warning caller is responsible for calling SysFreeString on the result
#if TT_DEBUG_ON 
	DWORD start;
	if (tt_debug_mode == 310303) {
		start = timeGetTime();
	};
#endif
	DOMNodeType type; // new on 070403 so trim if NODE_TEXT
   xml_node *text_node = first_node_that_is_text(node,type);
#if TT_DEBUG_ON
	if (tt_debug_mode == 310303) {
		tt_error_file() << (timeGetTime()-start) << " ms to first_node_that_is_text in xml_get_text" << endl;
		start = timeGetTime();
	};
#endif
	BSTR text_value;
	if (text_node == NULL) {
//#if TT_DEBUG_ON
//		xml_debug_node(node,L"Failed to get_text");
//#endif
		node->get_text(&text_value);
		return(text_value); // used to return NULL prior to 050803
	};
	if (FAILED(text_node->get_text(&text_value))) {
#if TT_DEBUG_ON
		xml_debug_node(node,L"Failed to get_text");
#endif
		return(NULL); // warn? -- restored on 070403
	};
	// commented out the following since did the wrong thing for special characters like & ( became &amp or something )
//	int offset = 0;
//	if (type == NODE_TEXT) { // new on 070403
//		text_node->get_xml(&text_value); // experiment on 310303 -- instead of 10 or more milliseconds this is very fast
//#if TT_DEBUG_ON 
//		if (tt_debug_mode == 310303) {
//			DWORD duration = (timeGetTime()-start);
//			tt_error_file() << duration << " ms just to get_text in xml_get_text" << endl;
////			if (duration < 1) {
////				xml_debug_node(text_node);
////			};
//		};
//#endif
//		// trim
//		int length = wide_string_length(text_value);
//		for (int i = 0; i < length; i++) {
//			if (text_value[i] != ' ') { // could there be other kinds of white space?
//				break;
//			};
//			offset = i;
//		};
//		for (int i = length-1; i > 0; i--) {
//			if (text_value[i] == ' ' || text_value[i] == '\r' || text_value[i] == '\n') {
//				text_value[i] = NULL;
//			} else {
//				break;
//			};
//		};
//#if TT_DEBUG_ON
//		if (tt_debug_mode == 70303) {
//			BSTR good_text_value;
//			text_node->get_text(&good_text_value);
//			if (compare_wide_strings(text_value+offset,good_text_value) != 0) {
//				log("get_text and get_xml disagree still");
//			};
//		};
//#endif
//		return(text_value+offset);
//	}; // else CDATA
//	text_node->get_text(&text_value);
	return(text_value);
};

wide_character xml_get_text_first_character(xml_node *node) { 
	DOMNodeType type; 
   xml_node *text_node = first_node_that_is_text(node,type);
   if (text_node == NULL) return(0); // warn?
   BSTR text_value;
   if (FAILED(text_node->get_text(&text_value))) return(NULL); // warn?
   wide_character result = text_value[0];
   ::SysFreeString(text_value);
   return(result);
};

wide_string xml_get_text_all_copy(xml_node *parent, int &result_length) { 
   // this gets (and concatenates) all of the textual children
//   BSTR text_value;
//   if (FAILED(parent->get_text(&text_value))) return(NULL); // warn?
   // problem with the above is that it concatenates but puts spaces between parts that were on different lines - sigh
   xml_node *node;
   if (FAILED(parent->get_firstChild(&node))) return(NULL);
   if (node == NULL) return(NULL);
   DOMNodeType type;
   if (FAILED(node->get_nodeType(&type))) {
      return(NULL); // report problem?
   };
   wide_string result = NULL;
   result_length = 0;
   while (type == NODE_TEXT || type == NODE_CDATA_SECTION) {
      BSTR text_value;
//#if TT_DEBUG_ON
//		xml_debug_node(parent);
//#endif
      if (SUCCEEDED(node->get_text(&text_value))) {
         if (result == NULL) {
            result_length = ::SysStringLen(text_value);
            result = copy_wide_string(text_value,result_length);
         } else {
            int length_of_addition = ::SysStringLen(text_value);
            int new_result_length = result_length+length_of_addition;
            wide_string new_result = new wide_character[new_result_length+1];
            wmemcpy(new_result,result,result_length);
            wmemcpy(new_result+result_length,text_value,length_of_addition);
            new_result[new_result_length] = NULL; // terminate
            delete [] result;
            result = new_result;
            result_length = new_result_length;
         };
         ::SysFreeString(text_value);
      };
      if (node->get_nextSibling(&node) == S_FALSE) return(result);
      if (FAILED(node->get_nodeType(&type))) {
         return(NULL); // report problem?
      };
   };
   return(result);
};


int xml_get_int(xml_node *node, int default_value) {
#if TT_DEBUG_ON
	DWORD start;
	if (tt_debug_mode == 310303) {
		start = timeGetTime();
	};
#endif
	DOMNodeType type; 
   xml_node *text_node = first_node_that_is_text(node,type);
	if (text_node == NULL) {
		return(default_value);
	};
	// prior to 070403 this used to call xml_get_text but that is too heavy
	// worth checking that it is NODE_TEXT? 
   BSTR text_value;
	text_node->get_xml(&text_value);
#if TT_DEBUG_ON
	if (tt_debug_mode == 310303) {
		tt_error_file() << (timeGetTime()-start) << " ms to get_xml in xml_get_int" << endl;
	};
//	xml_debug_node(node);
#endif
   if (text_value == NULL) {
      return(default_value); // report problem?
   };
   int integer_value = _wtoi(text_value);
   ::SysFreeString(text_value);
#if TT_DEBUG_ON
	if (tt_debug_mode == 310303) {
		tt_error_file() << (timeGetTime()-start) << " ms to xml_get_int" << endl;
	};
#endif
   return(integer_value);
};

int64 xml_get_int64(xml_node *node, int64 default_value) {
//	BSTR text_value = xml_get_text(node);
	DOMNodeType type; 
   xml_node *text_node = first_node_that_is_text(node,type);
	if (text_node == NULL) return(default_value);
	// prior to 070403 this used to call xml_get_text but that is too heavy
	// worth checking that it is NODE_TEXT? 
   BSTR text_value;
	text_node->get_xml(&text_value);
   if (text_value == NULL) {
      return(default_value); // report problem?
   };
   int64 integer_value = _wtoi64(text_value);
   ::SysFreeString(text_value);
   return(integer_value);
};

double xml_get_double(xml_node *node, double default_value)  {
   BSTR text_value = xml_get_text(node);
   if (text_value == NULL) {
      return(default_value); // report problem?
   };
   double double_value = _wtof(text_value);
   ::SysFreeString(text_value);
   return(double_value);
};

NumberValue *xml_get_number_value(xml_node *node) {
   BSTR text_value = xml_get_text(node);
   if (text_value == NULL) {
#if TT_DEBUG_ON
		debug_this();
#endif
		tt_error_file() << "Node has no text value to extract number" << endl; // new on 201003
      return(NULL); // report problem?
   };
	//tt_error_file() << "Number's text value is " << copy_narrow_string(text_value) << endl; // REMOVE!!! 201003
   int base;
   int offset_to_skip_base = extract_base(text_value,base);
   NumberValue *number_value = NULL;
	Tag tag = tag_token(node);
	if (tag != RATIONAL_NUMBER_TAG && tag != FLOATING_POINT_NUMBER_64_BIT_TAG) {
      // first see if it'll fit in a long
      wide_string stopped_at = NULL;
      long integer_value = wcstol(text_value+offset_to_skip_base,&stopped_at,base);
      if ((stopped_at == NULL || stopped_at[0] == NULL) && integer_value != max_long && integer_value != min_long) {
         // added stopped_at[0] == NULL condition on 081002
         // fits in 32 bits
         number_value = new LongNumberValue(integer_value);
			::SysFreeString(text_value); // return here new on 211003 since no longer assume that it must be a bignum otherwise
			return(number_value);
		};
	};
//			// commented out 211003 since handled below more generally
//		} else {
//			// otherwise got a big num         
//			number_value = new BigNumberValue();
//			bignum_pointer bignum_value;
//			number_value->bignum_value(bignum_value);
//			string narrow_text_value = copy_narrow_string(text_value+offset_to_skip_base,::SysStringLen(text_value)-offset_to_skip_base);
//			// unfortunately must convert from Unicode to 8bit
//			set_bignum_value_to_string(bignum_value,narrow_text_value,base);
//			delete [] narrow_text_value;
////				if (mpz_sgn(bignum_value) == 0) return(FALSE); // not a good number string
//		};
//	} else {
	switch (tag) {
		case FLOATING_POINT_NUMBER_64_BIT_TAG: // moved here on 211003 from below case RATIONAL_NUMBER_TAG: 
			number_value = new DoubleFloatNumberValue(_wtof(text_value));
			break;
//      case INTEGER_32_BIT_TAG:  // made obsolete on 190802 since this should be an internal implementation detail
		default: // unless returned above
//		case RATIONAL_NUMBER_TAG: {
			int text_length = ::SysStringLen(text_value);
			string narrow_text_value = copy_narrow_string(text_value+offset_to_skip_base,text_length-offset_to_skip_base);
//				int slash_location = strcspn(narrow_text_value,"/");
			string slash_and_denominator = strchr(narrow_text_value,'/');
			if (slash_and_denominator == NULL) { // slash_location == 0) { // degenerate Rational - treat it like a bignum
				number_value = new BigNumberValue();
				bignum_pointer bignum_value;
				number_value->bignum_value(bignum_value);
				set_bignum_value_to_string(bignum_value,narrow_text_value,base);
			} else {
				number_value = new RationalNumberValue();
				rational_pointer rational_value;
				number_value->rational_value(rational_value);
//					narrow_text_value[slash_location] = '\0'; // clobber the /
				slash_and_denominator[0] = '\0'; // clobber the slash
				set_bignum_value_to_string(mpq_numref(rational_value),narrow_text_value,base);
//			   if (mpz_sgn(mpq_numref(rational_value)) == 0 && text[0] != '0') return(FALSE); // not a good number string
				// numerator and denominator might have different bases so check
				string remaining_text = slash_and_denominator+1;
				offset_to_skip_base = extract_base(remaining_text,base);
				set_bignum_value_to_string(mpq_denref(rational_value),remaining_text+offset_to_skip_base,base);
//					if (mpz_sgn(mpq_denref(rational_value)) == 0) return(FALSE); // not a good number string
				// shouldn't need to canonicalize since was OK before it was dumped but user might have changed it
#if TT_DEBUG_ON
				if (tt_debug_mode == 290303) {
					tt_error_file() << "About to canonicalize " << narrow_text_value << " in base " << base << endl
										 << " divided by " << remaining_text+offset_to_skip_base << " in base " << base << endl;
				};
#endif
				mpq_canonicalize(rational_value);
			};
			delete [] narrow_text_value;
			break;
										};
			//default:
			//	return(NULL); // warn?? - save??
   ::SysFreeString(text_value);
   return(number_value);
};

Sprite *xml_get_sprite(xml_node *node, Sprite *sprite, SpriteType type) { 
   xml_node *sprite_node = first_node_that_is_an_element(node);
#if TT_DEBUG_ON
	if (sprite_node == NULL) {
		xml_debug_node(node,L"xml_get_sprite");
	};
#endif
//   if (sprite_node == NULL) return(NULL); // warn? -- removed since xml_load_sprite checks this as well
   return(xml_load_sprite(sprite_node,tag_token(sprite_node),sprite,type));
};

xml_document *xml_get_document(xml_node *node) {
   if (node == NULL) return(NULL);
   xml_document *document;
   if (FAILED(node->get_ownerDocument(&document))) return(NULL);
   return(document);
};

xml_element *xml_get_document_element(xml_document *document) {
	// returned xml_node * prior to 190703
   if (document == NULL) return(NULL);
   xml_element *element;
   if (FAILED(document->get_documentElement(&element))) return(NULL);
   return(element);
};
xml_document *correct_or_report_parse_error(IXMLDOMDocument *document, wide_string where, int length, boolean from_file, boolean warn=TRUE);

xml_document *correct_or_report_parse_error(IXMLDOMDocument *document, wide_string where, int length, boolean from_file, boolean warn) {
	// changed on 150404 to return TRUE if corrected the problem
   IXMLDOMParseError *error;
   HRESULT result = document->get_parseError(&error);
   long error_code;
   error->get_errorCode(&error_code);
	if (error_code == -1072896682) return(NULL); // top-level not XML so don't fill up log file with reports of this -- new on 181202
	if (error_code == -1072896657) { // new on 150404 to try to recover from the
		// Switch from current encoding to specified encoding not supported
		// since this happens in Win98/Me when loading XML with an encoding property
		if (from_file) {
			string file_name = copy_narrow_string(where);
			long file_size = 0;
			string contents = copy_file_or_url_to_string(file_name,file_size);
			if (contents != NULL) {
				where = copy_wide_string(contents,file_size);
				delete [] contents;
				delete file_name;
			} else {
				where = NULL;
			};
		};
		wide_string encoding_start = NULL;
		if (where != NULL) {
		   encoding_start = wcsstr(where,L"encoding=");
		};
		if (encoding_start != NULL) {
			// replace encoding with white space
			while (encoding_start[0] != NULL && encoding_start[0] != L'?') { // ends with ?>
				encoding_start[0] = ' ';
				encoding_start++;
			};
			xml_document *new_document = create_xml_document();
			VARIANT_BOOL bIsSuccessful;
			if (FAILED(new_document->loadXML(where,&bIsSuccessful))) {
				return(NULL); // warn?
			};
			if (bIsSuccessful) {
				return(new_document);
			};
		};
		// otherwise report error
	};
	if (!warn) { // new on 181204
		return(NULL);
	};
   BSTR reason;
   error->get_reason(&reason);
   _bstr_t bstrErr(reason);
//   printf("Error:\n");
//   printf("Code = 0x%x\n", error_code);
   long line, line_position;
   error->get_line(&line);
   error->get_linepos(&line_position);
//   printf("Source = Line : %ld; Char : %ld\n", line, line_pos);
//   printf("Error Description = %s\n", (char*)bstrErr);
   if (from_file) {
      if (where != NULL && where[0] != NULL) {
         tt_error_file() << "Error parsing the XML file " << where << " ";
      } else {
         tt_error_file() << "Error parsing an XML file. ";
      };
   } else { // reworded on 100103 and updated 150404 -- reworded again on 291104
      tt_error_file() << "Warning: failed to interpret the following as XML. If the following is from an old ToonTalk file format then all is well and ignore this warning." << endl;
		string narrow_node_text = copy_narrow_string(where,length);		
		tt_error_file() << narrow_node_text << endl;
		delete [] narrow_node_text;
   };
	if ((char *) bstrErr != NULL) { // condition new -- seems to have happened when 60MB text was on clipboard
		tt_error_file() << (char *) bstrErr << endl;
	};
	tt_error_file() << "Error code is " << error_code << " on line " << line << " at position " << line_position << endl;
   //if (!from_file) {
   //   tt_error_file() << "While processing the following:" << endl;
   //   tt_error_file() << where << endl;
   //};
	if (from_file) {
		tt_err_file_important_but_no_error = TRUE; 
		// clipboard is more tolerant -- prior to 291104 was tt_err_file_important but old TT files then trigger this
//#if TT_DEBUG_ON
//		if (where != NULL) {
//			tt_main_window->add_to_clipboard(where,strlen(where));
//		};
//#endif
	};
	return(NULL);
};

void xml_load_geometry(Sprite *sprite, xml_node *geometry_node) { // move to Sprite:: ??
   city_coordinate llx = 0, lly = 0, width = 0, height = 0;
   xml_node *node;
   if (geometry_node->get_firstChild(&node) == S_FALSE) return;
   //if (FAILED(result)) {
   //   return; // report problem?
   //};
   while (node != NULL) {  
      DOMNodeType type;
      if (FAILED(node->get_nodeType(&type))) {
         return; // report problem?
      };
      if (type == NODE_ELEMENT) {
         switch (tag_token(node)) {
            case LEFT_TAG:
               llx = xml_get_int(node);
               break;
            case BOTTOM_TAG:
               lly = xml_get_int(node);
               break;
            case WIDTH_TAG:
               width = xml_get_int(node);
               break;
            case HEIGHT_TAG:
               height = xml_get_int(node);
               break;
            default:
               break; // capture extra stuff??
         };
      };
      if (node->get_nextSibling(&node) == S_FALSE) break;
      //if (FAILED(result)) {
      //   return; // report problem?
      //};
   };
   sprite->set_size_and_location(width,height,llx,lly,LOADING_GEOMETRY);
};

//void xml_load_flags(Sprite *sprite, xml_node *flags_node) {
//   xml_node *node;
//   if (flags_node->get_firstChild(&node) == S_FALSE) return;
//   //if (FAILED(result)) {
//   //   return; // report problem?
//   //};
//   while (node != NULL) {  
//      DOMNodeType type;
//      if (FAILED(node->get_nodeType(&type))) {
//         return; // report problem?
//      };
//      if (type == NODE_ELEMENT) {
//         switch (tag_token(node)) {
//            case INFINITE_STACK_TAG:
//               sprite->set_infinite_stack(TRUE);
//               break;
//            case INVISIBLE_TAG:
//               sprite->set_visible(FALSE);
//               break;
//            case SHOW_ALL_TAG:
//               sprite->set_show_all(TRUE);
//               break;
//            case ERASED_TAG:
//               sprite->become_blank();
//               break;
//            default:
//               break; // capture extra stuff??
//         };
//      };
//      if (node->get_nextSibling(&node) == S_FALSE) break;
//      //if (FAILED(result)) {
//      //   return; // report problem?
//      //};
//   };
//};

xml_node *xml_first_child(xml_node *parent) {
   xml_node *node;
   if (parent->get_firstChild(&node) == S_FALSE) return(NULL);
   //if (FAILED(result)) {
   //   return(NULL); // report problem?
   //};
   return(node);
};

DOMNodeType node_type(xml_node *node) {
   DOMNodeType type;
   node->get_nodeType(&type); // can this fail?
   return(type);
};

wide_string xml_get_text_copy(xml_node *node) {
   BSTR node_text = xml_get_text(node);
   if (node_text == NULL) {
      return(NULL); // report problem?
   };
   wide_string result = copy_wide_string(node_text);
   ::SysFreeString(node_text);
   return(result);
};

string xml_get_narrow_text_copy(xml_node *node) {
   BSTR node_text = xml_get_text(node);
   if (node_text == NULL) {
      return(NULL); // report problem?
   };
   string result = copy_narrow_string(node_text);
   ::SysFreeString(node_text);
   return(result);
};

void xml_to_stream(xml_node *node, output_stream &stream) { // new on 090103
	// could I use some XML API instead?
	if (node == NULL) { // new on 170803
		long length = 0;
		stream.write((char *) &length,sizeof(length));
		return;
	};
   BSTR node_text = NULL;
   if (node != NULL) node->get_xml(&node_text);
	long length = SysStringLen(node_text);
	stream.write((char *) &length,sizeof(length));
   stream.write((char *) node_text,length*sizeof(wide_character));
#if TT_DEBUG_ON
	if (tt_debug_mode == 100703) {
		string narrow_node_text = copy_narrow_string(node_text,length);
		tt_error_file() << narrow_node_text << endl;
		delete [] narrow_node_text;
	};
#endif
   ::SysFreeString(node_text);
};

xml_document *xml_from_stream(InputStream *stream, long length) { // new on 090103
//#if TT_DEBUG_ON
//	int loc = stream.tellg();
//	tt_error_file() << "debug this" << loc << endl;
//#endif
	if (length < 0) { // new on 140203
		stream->read((string) &length, sizeof(length));
	};
	if (length <= 0) return(NULL); // changed == to <= on 140603
	wide_string node_text = new wide_character[length+1]; // add 1 for terminating NULL
	stream->read((string) node_text,length*sizeof(wide_character));
   if (node_text == NULL) {
      return(NULL);
   };
	node_text[length] = NULL; // terminate string
#if TT_DEBUG_ON
	if (tt_debug_mode == 130703) { // come here to debug cities in time travel segments
		string narrow_node_text = copy_narrow_string(node_text,length);
		tt_error_file() << narrow_node_text << endl;
		delete [] narrow_node_text;
	};
#endif
   IXMLDOMDocument *document = create_xml_document();
   VARIANT_BOOL bIsSuccessful;
	if (FAILED(document->loadXML(node_text,&bIsSuccessful))) {
		return(NULL); // warn?
	};
	xml_document *result;
   if (!bIsSuccessful) {
		result = correct_or_report_parse_error(document,node_text,length,FALSE);
		// rewritten on 150404 to try to recover
		document->Release(); // release the bad one in any case
//		string narrow_node_text = copy_narrow_string(node_text,length);
//      report_parse_error(document,narrow_node_text,TRUE);
//		delete [] narrow_node_text;
//		result = NULL;
	} else {
		result = document; // first_node_that_is_an_element(document);
   };
//	document->Release();
	delete [] node_text;
   return(result);
};

void ignore_xml_from_stream(InputStream *stream, long length) { // new on 170803
	if (length < 0) { // new on 140203
		stream->read((string) &length, sizeof(length));
	};
	if (length <= 0) return;
	stream->ignore(length*sizeof(wide_character));
};

void xml_debug_node(xml_node *node, wide_string label) {
	if (node == NULL) return;
	BSTR xml;
   node->get_xml(&xml);
	if (label == NULL) {
		label = L"debug XML";
	};
   ::MessageBoxW(NULL,xml,label,MB_OK);
	tt_main_window->add_to_clipboard(copy_narrow_string(xml),::SysStringLen(xml)); 
	// don't really want this to call save_clipping_to_file...
};

xml_node *xml_get_attribute_node(xml_node *node, wide_string attribute_name) {
#if TT_DEBUG_ON
   if (tt_debug_mode == 280802) {
      BSTR xml;
      node->get_xml(&xml);
      ::MessageBoxW(NULL,xml,L"xml_get_attribute_int",MB_OK);
   };
#endif
   IXMLDOMNamedNodeMap *attribute_map;
   if (FAILED(node->get_attributes(&attribute_map)) || attribute_map == NULL) return(NULL);
   long attribute_length;
   attribute_map->get_length(&attribute_length);
   if (attribute_length == 0) {
      attribute_map->Release();
      return(NULL);
   };
   xml_node *attribute_node;
   if (FAILED(attribute_map->getNamedItem(attribute_name,&attribute_node))) {
      attribute_map->Release();
      return(NULL);
   };
   attribute_map->Release();
   return(attribute_node);
};

int xml_get_attribute_int(xml_node *node, wide_string attribute_name, int default_value) {
   xml_node *number_node = xml_get_attribute_node(node,attribute_name);
   if (number_node == NULL) {
//#if TT_DEBUG_ON
//		xml_debug_node(node,L"NoNumberNode");
//#endif
      return(default_value);
   };
   int number = xml_get_int(number_node,default_value);
   number_node->Release();
   return(number);
};

int64 xml_get_attribute_int64(xml_node *node, wide_string attribute_name, int64 default_value) {
	xml_node *number_node = xml_get_attribute_node(node,attribute_name);
   if (number_node == NULL) {
      return(default_value);
   };
   int64 number = xml_get_int64(number_node,default_value);
   number_node->Release();
   return(number);
};

double xml_get_attribute_double(xml_node *node, wide_string attribute_name, double default_value) {
   xml_node *number_node = xml_get_attribute_node(node,attribute_name);
   if (number_node == NULL) {
      return(default_value);
   };
   double number = xml_get_double(number_node,default_value);
   number_node->Release();
   return(number);
};

wide_character xml_get_attribute_wide_character(xml_node *node, wide_string attribute_name) {
   xml_node *wide_character_node = xml_get_attribute_node(node,attribute_name);
   if (wide_character_node == NULL) {
      return(0);
   };
   BSTR text_value;
   if (FAILED(wide_character_node->get_text(&text_value))) return(0); // warn?
   wide_character result = text_value[0];
   //if (result == 0) {
   //   result = '\n'; // new lines get lost so restore them -- general solution or just OK for robot's keyboard actions?
   //};
   ::SysFreeString(text_value);
   wide_character_node->Release();
   return(result);
};

string xml_get_attribute_string(xml_node *node, wide_string attribute_name, string default_value) {
   xml_node *wide_character_node = xml_get_attribute_node(node,attribute_name);
   if (wide_character_node == NULL) {
      return(default_value);
   };
   BSTR text_value;
   if (FAILED(wide_character_node->get_text(&text_value))) return(default_value); // warn?
//#if TT_DEBUG_ON
//	xml_debug_node(node);
//#endif
   string result = copy_narrow_string(text_value,::SysStringLen(text_value));
   ::SysFreeString(text_value);
   wide_character_node->Release();
   return(result);
};

wide_string xml_get_attribute_wide_string(xml_node *node, wide_string attribute_name, wide_string default_value) { // new on 121202
   xml_node *wide_character_node = xml_get_attribute_node(node,attribute_name);
   if (wide_character_node == NULL) {
      return(default_value);
   };
   BSTR text_value;
   if (FAILED(wide_character_node->get_text(&text_value))) return(default_value); // warn?
   wide_string result = copy_wide_string(text_value,::SysStringLen(text_value));
   ::SysFreeString(text_value);
   wide_character_node->Release();
   return(result);
};

boolean xml_has_attribute(xml_node *node, wide_string attribute_name) { // new on 250803
   xml_node *wide_character_node = xml_get_attribute_node(node,attribute_name);
   return(wide_character_node != NULL); // was is the story about releasing this stuff?
};

BSTR xml_get_attribute_BSTR(xml_node *node, wide_string attribute_name) { // new on 150703 - more efficient than above but use is more limitted
   xml_node *wide_character_node = xml_get_attribute_node(node,attribute_name);
   if (wide_character_node == NULL) {
      return(NULL);
   };
   BSTR text_value;
   if (FAILED(wide_character_node->get_text(&text_value))) return(NULL); // warn?
   return(text_value);
};

//#if TT_DEBUG_ON
//   BSTR debug_xml;
//   node->get_xml(&debug_xml);
//   ::MessageBoxW(NULL,debug_xml,L"SerialNumber node",MB_OK);
//#endif
   //IXMLDOMAttribute *attribute_node;
   //if (FAILED(element->getAttributeNode(L"SerialNumber",&attribute_node))) return;
   //_variant_t value;
   //if (FAILED(attribute_node->get_nodeValue(&value))) return;
//   if (FAILED(element->getAttribute(L"SerialNumber",&value))) return; -- signals an error rather than failing - sigh
   //if (value.vt == VT_NULL) return; 
   //// does have a serial number
   //int index = _wtoi(_bstr_t(value));


Numbered_Sprites *current_numbered_sprites = NULL;

//void reset_serial_numbering() {
//   if (serial_numbered_sprites != NULL) delete [] serial_numbered_sprites;
//   serial_numbered_sprites = NULL;
//   max_serial_number = 0;
//};

void initialize_current_serial_numbering(int max_needed) {
	if (current_numbered_sprites == NULL) {
		current_numbered_sprites = new Numbered_Sprites();
	};
	current_numbered_sprites->initialize_serial_numbering(max_needed);
};

void Numbered_Sprites::initialize_serial_numbering(int max_needed) {
   max_serial_number_used = 0; // added on 140802
   if (max_needed < 1) {
      // in case do end up needing some it is then best to reset table
		if (serial_numbered_sprites != NULL) delete [] serial_numbered_sprites;
		serial_numbered_sprites = NULL;
		max_serial_number = 0;
      return;
   };
   max_serial_number = max_needed;
	if (serial_numbered_sprites != NULL) delete [] serial_numbered_sprites; // new on 170103
   serial_numbered_sprites = new SpritePointer[max_serial_number];
   for (int i = 0; i < max_serial_number; i++) {
      serial_numbered_sprites[i] = NULL;
   };
};
   
int return_max_serial_number() {
	if (current_numbered_sprites == NULL) return(0);
   return(current_numbered_sprites->return_max_serial_number());
};

void allocate_serial_numbers(xml_node *node) {
   int max_serial_number = xml_get_attribute_int(node,L"MaxSerialNumber");
   if (max_serial_number > 0) {
      initialize_current_serial_numbering(max_serial_number);
   };
};

Numbered_Sprites *push_current_numbered_sprites() {
#if TT_DEBUG_ON
	if (tt_debug_mode == 100703) {
		tt_error_file() << "push_current_numbered_sprites during segment " << tt_current_log_segment;
		if (current_numbered_sprites == NULL) {
			tt_error_file() << " no previous numbered sprites." << endl;
		} else {
			 tt_error_file() << " old contained " << current_numbered_sprites->return_max_serial_number() << " sprites." << endl;
		};
	};
#endif
	Numbered_Sprites *result = current_numbered_sprites;
	current_numbered_sprites = new Numbered_Sprites();
	return(result);
};

void restore_numbered_sprites(Numbered_Sprites *old_table) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 100703) {
		tt_error_file() << "restore_numbered_sprites during segment " << tt_current_log_segment;
		if (old_table == NULL) {
			tt_error_file() << " no previous numbered sprites." << endl;
		} else {
			 tt_error_file() << " restored contains " << old_table->return_max_serial_number() << " sprites." << endl;
		};
	};
#endif
	if (current_numbered_sprites != NULL) delete current_numbered_sprites;
	current_numbered_sprites = old_table;
};

void release_numbered_sprites() {
	if (current_numbered_sprites != NULL) {
		delete current_numbered_sprites;
		current_numbered_sprites = NULL;
	};
};

void remember_if_serial_numbered(xml_node *node, Sprite *sprite) {
//#if TT_DEBUG_ON
//   BSTR debug_xml;
//   element->get_xml(&debug_xml);
//   ::MessageBoxW(NULL,debug_xml,L"Element",MB_OK);
//#endif
   int index = xml_get_attribute_int(node,L"SerialNumber"); // nothing created with a version after 150703 should make these
   if (index <= 0) { 
      return;
   };
	if (current_numbered_sprites == NULL) {
		current_numbered_sprites = new Numbered_Sprites();
	};
	current_numbered_sprites->remember_if_serial_numbered(index,sprite);
};

void Numbered_Sprites::remember_if_serial_numbered(int index, Sprite *sprite) {
   if (index > max_serial_number) { // need more room
      int new_max_serial_number = index+32; // was max_serial_number+32 prior to 151002
      // grow by units of 32 -- hopefully initialized to an ideal value anyway
      SpritePointer *new_serial_numbered_sprites = new SpritePointer[new_max_serial_number];
      int i = 0;
      for (; i < max_serial_number; i++) {
         new_serial_numbered_sprites[i] = serial_numbered_sprites[i];
      };
      for (; i < new_max_serial_number; i++) {
         new_serial_numbered_sprites[i] = NULL; // initialize new ones
      };
      if (serial_numbered_sprites != NULL) delete [] serial_numbered_sprites;
      serial_numbered_sprites = new_serial_numbered_sprites;
      max_serial_number = new_max_serial_number;
   };
#if TT_DEBUG_ON
	if (tt_debug_mode == 100703) {
		tt_error_file() << "Associating ";
		sprite->print(tt_error_file());
		tt_error_file() << " with index " << index-1 << " during segment " << tt_current_log_segment << endl;
	};
#endif
   serial_numbered_sprites[index-1] = sprite; // converting from 1-indexing to 0-indexing
   if (index > max_serial_number_used) {
      max_serial_number_used = index;
   };
};

Sprite *Numbered_Sprites::serial_numbered_sprite(int index) {
	if (index > 0 && index <= max_serial_number) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 100703) {
			tt_error_file() << "Finding ";
			if (serial_numbered_sprites[index-1] != NULL) {
				serial_numbered_sprites[index-1]->print(tt_error_file());
			} else {
				tt_error_file() << "NOTHING";
			};
			tt_error_file() << " with index " << index-1 << " during segment " << tt_current_log_segment << endl;
		};
#endif
      return(serial_numbered_sprites[index-1]); // int_value is 1-indexed
   };
	return(NULL);
};

Sprite *serial_numbered_sprite(int index) {
	if (current_numbered_sprites == NULL) return(NULL);
	return(current_numbered_sprites->serial_numbered_sprite(index));
};

Sprite *xml_load_link(xml_node *link_node) {
   //VARIANT value;
   //if (FAILED(link_element->getAttribute(L"SerialNumber",&value))) return(NULL); // warn??
   //if (value.vt == VT_NULL) return(NULL); // warn??
   //int int_value = _wtoi(_bstr_t(value));
	int index = xml_get_attribute_int(link_node,L"SerialNumber"); // nothing created with a version after 150703 should make these
	Sprite *result = serial_numbered_sprite(index);
   if (result != NULL) return(result);
   tt_error_file() << "Couldn't find object with serial number " << index << endl;
   return(NULL);
};

// Old buggy XPath version
//Sprite *xml_load_link(xml_node *node) {
//   BSTR node_text = xml_get_text(node);
////#if TT_DEBUG_ON
////   BSTR debug_xml;
////   node->get_xml(&debug_xml);
////   ::MessageBoxW(NULL,debug_xml,L"Link",MB_OK);
////   xml_node *parent_node;
////   node->get_parentNode(&parent_node);
////   parent_node->get_xml(&debug_xml);
////   ::MessageBoxW(NULL,debug_xml,L"Parent",MB_OK);
////   parent_node->get_parentNode(&parent_node);
////   parent_node->get_xml(&debug_xml);
////   ::MessageBoxW(NULL,debug_xml,L"GrandParent",MB_OK);
////   parent_node->get_parentNode(&parent_node);
////   parent_node->get_xml(&debug_xml);
////   ::MessageBoxW(NULL,debug_xml,L"Great GrandParent",MB_OK);
////#endif
//   if (node_text == NULL) {
//      return(NULL); // report problem?
//   };
//   // to do
//   Sprite *sprite = NULL;
//   xml_node *linked_node;
////   HRESULT result = node->selectSingleNode(node_text,&linked_node); -- should work but doesn't - programming around it
//   wide_string node_text_remaining = node_text;
//   wide_character parent_pattern[] = L"parent::*/";
//   const int parent_pattern_length = 10;
//   while (compare_wide_strings_of_length(parent_pattern,node_text_remaining,parent_pattern_length) == 0) {
//      if (FAILED(node->get_parentNode(&node))) return(NULL); // warn??
//      node_text_remaining += parent_pattern_length;
////#if TT_DEBUG_ON
////      BSTR debug_xml;
////      node->get_xml(&debug_xml);
////      ::MessageBoxW(NULL,debug_xml,L"Current",MB_OK);
////#endif
//   };
//   HRESULT result = node->selectSingleNode(node_text_remaining,&linked_node);
////   result = node->selectSingleNode(L"Hole[1]",&linked_node);
////#if TT_DEBUG_ON
////   BSTR debug_xml;
////   linked_node->get_xml(&debug_xml);
////   ::MessageBoxW(NULL,debug_xml,L"Hole[0]",MB_OK);
////#endif
////   result = linked_node->selectSingleNode(L"Nest",&linked_node);
//   if (FAILED(result) || linked_node == NULL) return(NULL);
//#if TT_DEBUG_ON
//   BSTR xml;
//   linked_node->get_xml(&xml);
//   ::MessageBoxW(NULL,xml,L"Destination",MB_OK);
//#endif
//   ::SysFreeString(node_text);
//   return(sprite);
//};

Sprites *xml_load_sprites(xml_node *parent, boolean NULL_elements_ok) { 
	// should this be renamed xml_get_sprites since it is more like xml_get_sprite ??
   // new on 041102
   if (parent == NULL) {
      return(NULL);
   };
   Sprites *sprites = NULL;
   Sprites *last_cons = NULL;
   xml_node *node;
   if (parent->get_firstChild(&node) == S_FALSE) return(NULL); // no children
   while (node != NULL) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 170802) {
			xml_debug_node(node,L"xml_load_sprites");
		};
#endif
      DOMNodeType type;
      if (FAILED(node->get_nodeType(&type))) {
         return(sprites); // report problem?
      };
      if (type == NODE_ELEMENT) {
			Sprite *sprite = xml_load_sprite(node,tag_token(node));
			if (sprite != NULL || NULL_elements_ok) { // conditional new on 100103 to handle problems better
				Sprites *new_cons = new Sprites(sprite);
				if (last_cons == NULL) {
					sprites = new_cons;
					last_cons = new_cons;
				} else {  
					last_cons->set_rest(new_cons);
					last_cons = new_cons;
				}; 
			};
      };
      if (node->get_nextSibling(&node) == S_FALSE) {
         return(sprites);
      };
   };
   return(sprites);
};

#if TT_DEBUG_ON 
int debug_load_sprite_indent = 0;
int debug_load_sprite_counter = 0;
#endif

#if TT_DEBUG_ON
boolean debugging_guids = FALSE;
#endif

Sprite *xml_load_sprite(xml_node *node, Tag tag, Sprite *sprite, SpriteType type, boolean floors_or_houses_ok) {
   if (node == NULL) {
      return(NULL);
   };
#if TT_LOADING_AND_SAVING_INTERRUPTABLE
	// new on 140105
	boolean only_allow_quiting 
		= (time_travel_enabled() || tt_loading_to_undo_bammer || !tt_loading_is_ok_to_abort || replaying());
	// on 160105 added condition so don't interrupt when replaying a demo
	if (user_wants_to_interrupt_also_process_messages_now(only_allow_quiting)) { 
		// too hard to make this work sensibly if time travel is involved
		// and letting unbamming be interrupted seems dangerous
		ask_response_to_load_interruption();
	};
	if (tt_loading_is_ok_to_abort) {
		if (tt_sprites_loaded_count%100 == 0) {
			if (tt_titles_ended_on_frame < tt_frame_number) {
				loading_wait(FALSE); // this does the other branch as well as updating the subtitle
			} else {
				tt_screen->display(FALSE);
				tt_main_window->Paint();
			};
		};
		tt_sprites_loaded_count++; // made conditional on 190105 so don't get 3 objects loaded messages
	};
#endif
#if TT_DEBUG_ON
	if (tt_debug_mode == 170802) {
		xml_debug_node(node,L"xml_load_sprite");
	} else if (tt_debug_mode == 190903) {
		BSTR xml;
		node->get_xml(&xml);
		int ignore_xml_indented_string_length;
		wide_string xml_indented = xml_indent(xml,::SysStringLen(xml),1,80,ignore_xml_indented_string_length); 
		// 1 for each level, 80 is maximum width
		::SysFreeString(xml);
		string narrow_xml = copy_narrow_string(xml_indented);
		static int xml_count = 0;
		tt_error_file() << xml_count++ << " " << narrow_xml << endl;
		delete [] narrow_xml;
		delete [] xml_indented;
		
   };
	DWORD start;
	if (tt_debug_mode == 310303) {
		debug_load_sprite_indent++;
		debug_load_sprite_counter++;
		start = timeGetTime();
	};
#endif	
	//if (tag == INCLUDE_TAG) {
	//	return(handle_include_tag(node,sprite));		
	//};
   //if (tag == NO_SUCH_TAG) { // callers job beginning 111202
   //   tag = tag_token(node);
   //};
//   Sprite *sprite = NULL;
	GUID *guid = NULL;
	boolean external_link = FALSE;
	BSTR guid_string = xml_get_attribute_BSTR(node,L"Link");
	if (guid_string == NULL) {
		guid_string = xml_get_attribute_BSTR(node,L"ExternalLink");
		if (guid_string != NULL) {
			external_link = TRUE;
		};
	};
	if (guid_string != NULL) {
		if (::SysStringLen(guid_string) < 32) { // has been truncated
			tt_error_file() << "Link attribute in the XML should be 32-characters long. It is " 
								 << copy_narrow_string(guid_string) << endl;
		};
		GUID guid;
		hexidecimal_as_guid(guid_string,&guid);
		Sprite *result = find_sprite(&guid);
		if (result != NULL) {
			if (type != NONE_GIVEN && result->kind_of() != type) {
#if TT_DEBUG_ON
				debug_this();
#endif
				log("Loaded an object of un-expected type.",FALSE,TRUE);
				tt_error_file() << "For guid " << copy_narrow_string(guid_string) 
									 << " expected " << type_string(type) << " but found ";
				result->describe_type(tt_error_file());
				tt_error_file() << endl;
				::SysFreeString(guid_string);
#if TT_DEBUG_ON
				debugging_guids = TRUE;
#endif
				return(NULL);
			};
			::SysFreeString(guid_string);
			return(result);
		} else { // changed on 290803 to be unconditional
			if (!external_link) {
				// can happen when unbamming something added to the Pong game -- noticed 230704
#if TT_DEBUG_ON
				debug_this();
#endif
				tt_error_file() << "Couldn't find object with guid " << copy_narrow_string(guid_string) 
									 << " during log segment " << tt_current_log_segment << endl;
				::SysFreeString(guid_string);
#if TT_DEBUG_ON
				debugging_guids = TRUE;
#endif
			};	
			if (type == PICTURE || type == TEXT || type == CUBBY) {
				// new on 290803 to deal better with the problem
				const int max_message_size = 512; // should be more than enough
				char missing_guid_message[max_message_size];
				// wording improved on 140504
				if (type == PICTURE) {
					strcpy(missing_guid_message,"Something refers to a picture \rwith the generated name of \r");				
				} else if (type == TEXT) {
					strcpy(missing_guid_message,"Something refers to a text pad \rwith the generated name of \r");
				} else {
					strcpy(missing_guid_message,"Something refers to a box \rwith the generated name of \r");	
					// why a box? asked 140504			
				};
				int message_length = strlen(missing_guid_message);
				copy_narrow_string(guid_string,guid_string_size,missing_guid_message+message_length,guid_string_size);
				Text *pad = variable_width_text_pad(missing_guid_message,TRUE,tt_gray);
				if (type == PICTURE) {
					// rewritten on 260204 so that when erased is an erased picture
					Picture *picture = new Picture(0,0,0,0,SYNTHETIC_SPRITE,tile_width*2,tile_height*2);
					pad->set_size_and_location(tile_width,tile_height,tile_width/2,tile_height/2); 
					// centered and half size -- /2 new on 140504
					picture->add_follower(pad);
					return(picture);
//					return(make_indirect_picture(pad,FALSE));				
				} else if (type == TEXT) {
					return(pad);
				} else {
					Cubby *cubby = new Cubby();
					cubby->set_number_of_holes(1);
					cubby->initialize_hole(0,pad);
					return(cubby); // missing prior to 050304
				};
			} else {
				return(NULL);
			};
		};
	} else { 
		// no point checking if is a "Link" -- moved here on 160703 so this XML can be used to update
		// an existing sprite with the same GUID
		guid_string = xml_get_attribute_BSTR(node,L"GUID");
		if (guid_string != NULL) { 
			if (::SysStringLen(guid_string) < guid_string_size-1) { // has been truncated
				tt_error_file() << "Link attribute in the XML should be 32-characters long. It is " << guid_string << endl;
			} else {
#if TT_DEBUG_ON
				if (debugging_guids) {
					string narrow_guid_string = copy_narrow_string(guid_string);
					tt_error_file() << "Just loaded GUID " << narrow_guid_string 
										 << " during log segment " << tt_current_log_segment << endl;
					delete [] narrow_guid_string;
				};
#endif
				guid = new GUID; // was new GUID() prior to 040803
				hexidecimal_as_guid(guid_string,guid);
				::SysFreeString(guid_string);
				// the following is an interesting optimization (to re-use previous "histories" of the same object) 
				// but need to watch out for ref counting and the like
				//Sprite *existing_sprite = find_sprite(guid); // new on 160703 -- commenting out an experiment on 180703
				//if (existing_sprite != NULL) {
				//	sprite = existing_sprite;
				//	guid = NULL; // no need to use this more - later it is used if there isn't an existing one
				//};
				if (tt_uudecoding_a_sprite && external_link) { 
					// new on 070803 to produce a copy not the same thing - added external_link condition on 110803
					// but how can this be reached if external_link is TRUE since it would be on the other branch, right?
					// - asked 050304
#if TT_DEBUG_ON
					debug_this();
#endif					
					Sprite *original = find_sprite(guid);
					if (original != NULL) {
						return(original->top_level_copy());
					};
				};
			};
		};
	};
	// -- obsolete 160204
//	boolean process_after_include = FALSE;
//	int include_file_index = xml_get_attribute_int(node,L"Include",-1); // moved back here on 160703 since no longer acts like top_level item
//	if (include_file_index == -1) {
//		include_file_index = xml_get_attribute_int(node,L"IncludeAndProcess",-1);
//		process_after_include = TRUE;
//	};
//	if (include_file_index > 0) { // zero doesn't make sense - used for counters (and log00001.dmo)
//#if TT_DEBUG_ON
//		if (tt_debug_mode == 100703 || tt_debug_mode == 110703 || tt_debug_mode == 160703) {
//			tt_error_file() << "Loading include #" << include_file_index;
//			if (sprite != NULL) {
//				tt_error_file() << " of type " << type_name(sprite);
//			};
//			tt_error_file() << " for segment " << tt_current_log_segment << " on frame " << tt_frame_number << endl;
//		};
//#endif
//		Sprite *new_sprite = sprite_from_archive_index(include_file_index,sprite);
//#if TT_DEBUG_ON
//		if (tt_debug_mode == 100703 || tt_debug_mode == 160703) {
//			tt_error_file() << "Loaded include #" << include_file_index;
//			if (new_sprite != NULL) {
//				tt_error_file() << " of type " << type_name(new_sprite);
//				//if (new_sprite->kind_of() == INTEGER) {
//				//	log("debug this");
//				//};
//			} else {
//				log("Why no sprite in archive?");
//			};
//			tt_error_file() << endl;
//		};
//#endif
//		if (new_sprite != NULL) { // just to be safe - beginning 180703
//			new_sprite->set_recent_xml_location(include_file_index);
//			if (guid_string != NULL) {
//				new_sprite->set_guid(guid); // new on 090803
//				guid = NULL; // new on 180803 since held onto or destroyed above
//			};
//		};
//		if (process_after_include) {
//			sprite = new_sprite;
//		} else {
//			return(new_sprite);
//		};
//	};
//	if (tt_xml_version_number < 3) { // experiment on 150703 to see if this works for notebook pages, etc.
		// following for old version (prior to 150703)
		int serial_number = xml_get_attribute_int(node,L"SameAs",0);
		if (serial_number > 0) {
			Sprite *result = serial_numbered_sprite(serial_number);
			if (result != NULL) {
				if (type != NONE_GIVEN && result->kind_of() != type) {
					log("Loaded an object of un-expected type.",FALSE,TRUE);
					try { // new on 041003
						tt_error_file() << "For serial number " << serial_number << " expected " << type_string(type) << " but found ";
						result->describe_type(tt_error_file());
						tt_error_file() << endl;
					} catch (...) {
						// if error while printing error message then don't worry about it
						tt_error_file() << "Something went wrong trying to provide more details of the problem" << endl;
					};
#if TT_DEBUG_ON
					tt_error_file() << "Remember that serial numbers are 1-indexed while rest is 0." << endl;
					if (tt_debug_mode == 60403) {
						xml_debug_node(node,L"xml_load_sprite");
					};
#endif
					return(NULL);
				};
				return(result);
			} else {
				tt_error_file() << "Couldn't find object with serial number " << serial_number << endl;
#if TT_DEBUG_ON
				tt_error_file() << "Remember that serial numbers are 1-indexed while rest is 0." << endl;
				if (tt_debug_mode == 60403) {
					xml_debug_node(node,L"xml_load_sprite");
				};
#endif
				return(NULL);
			};
		};
//	};
	if (tag == TOONTALK_OBJECT_TAG || tag == PAGE_TAG) { // PAGE_TAG new on 200803
		// treated specially since just sets up a new environment and recurs (regardless of whether sprite is NULL) - new on 240103
		UnwindProtect<int> set(tt_xml_version_number,xml_get_attribute_int(node,L"Version",tt_xml_version_number)); 
		// new on 240103
		UnwindProtect<int> set2(tt_xml_language_index,xml_get_attribute_int(node,L"Language",tt_language)); // new on 131103
//#if TT_DEBUG_ON
//		if (xml_get_attribute_int(node,L"Language",-1) == -1) {
//			xml_debug_node(node);
//		};
//#endif
		Numbered_Sprites *old_numbered_sprites = push_current_numbered_sprites(); // new on 240103
		// so when pasted in multiple times there are no connections
		Sprites *old_non_networked_nests_with_guid = NULL;
		if (!tt_loading_to_undo_bammer) { // even top level objects can reference somethting external while Unbamming
			old_non_networked_nests_with_guid = push_non_networked_nests_with_guid();
		};
		// following was an attempt to deal better with XML for Mouse that includes XML to unbam - 
		// done on 130603 - removed on 140603
		//Sprites *saved_sprites_to_set_active = return_sprites_to_set_active();
	   //set_sprites_to_set_active(NULL);
		allocate_serial_numbers(node);
		Sprite *new_sprite = xml_get_sprite(node,sprite);
		restore_numbered_sprites(old_numbered_sprites);
		if (!tt_loading_to_undo_bammer) { // new on 011203
			restore_non_networked_nests_with_guid(old_non_networked_nests_with_guid);
		};
		set_sprites_to_active_now();
		//Sprites *trash = return_sprites_to_set_active();
		//just_destroy_list(trash);
		//set_sprites_to_set_active(saved_sprites_to_set_active);
		return(new_sprite);
	};
   xml_node *element = NULL; // re-organized on 081002 and yet again on 070103
	if (sprite == NULL) {
		switch (tag) {
			case TEXT_PAD_TAG:
				sprite = new Text();
				break;
			case BOX_TAG:
				sprite = new Cubby();
				break;
			case PICTURE_TAG:
				sprite = new Picture();
				break;
			case ROBOT_TAG:
				sprite = new Robot();
				break;
			case THOUGHT_BUBBLE_TAG: // new on 271102
				sprite = new Thought_Bubble();
				break;
			case USER_PICTURE_TAG: {
				boolean ok = FALSE; // was TRUE prior to 250303
				string file_name = NULL;
				string unique_name = xml_get_attribute_string(node,L"UniqueName",NULL);
				if (unique_name != NULL) {
					char private_file_name[MAX_PATH];
					media_file_name(unique_name,".png",private_file_name,tt_private_media_directory,tt_private_media_directory_length);
					//memcpy(private_file_name,tt_private_media_directory,tt_private_media_directory_length);
					//strcpy(private_file_name+tt_private_media_directory_length,unique_name);
					//strcat(private_file_name+tt_private_media_directory_length,".png");
//					sprite = new UserPicture(private_file_name,ok,FALSE,FALSE,xml_get_attribute_int(node,L"PixelWidth",-1),xml_get_attribute_int(node,L"PixelHeight",-1));
					// rewritten on 280403
					string original_name = xml_get_attribute_string(node,L"OriginalFile",NULL);
					sprite = new UserPicture(original_name,ok,FALSE,xml_get_attribute_int(node,L"PixelWidth",-1),xml_get_attribute_int(node,L"PixelHeight",-1),private_file_name);
					if (ok) {						
						Picture *picture = (Picture *) sprite;
						picture->set_private_media_file_name(unique_name);
						delete [] unique_name; // not needed anymore - added 140104 to fix a small leak
						delete [] original_name; // not needed anymore (copied by UserPicture) - added 140104 to fix a small leak
						// commented out on 280403 since taken care of by ::UserPicture now
						//if (original_name != NULL) {
						//	picture->just_set_original_file_name(original_name);
						//};
						sprite->set_show_all(TRUE);
						break; // all is well
					} else {
						file_name = original_name; // try with old name in case private store has been cleaned up
					};
				};
				if (!ok) {
					if (file_name == NULL) {
						file_name = xml_get_narrow_text_copy(node); // leak?? - 301002
					};
					// attributes new on 301002
					sprite = new UserPicture(file_name,ok,TRUE,xml_get_attribute_int(node,L"PixelWidth",-1),xml_get_attribute_int(node,L"PixelHeight",-1));
					if (ok) {
						sprite->set_show_all(TRUE); // unless Sprite::handle_xml_tag(tag,node) overrides this -- was FALSE prior to 240203
					};
				};
				if (!ok) {
					// move this to picture.cpp ??
					//Picture *picture = (Picture *) sprite;
					//picture->set_size_is_true(TRUE); // convenient "lie"
					//picture->set_size_and_location(100,100,0,0); // another lie
					//picture->set_indirection(variable_width_text_pad(file_name),TRUE);
					sprite->destroy();
					UserPictureFileMissing *picture = new UserPictureFileMissing(file_name,node);
//					picture->set_xml(node);
					sprite = picture;
					tt_error_file() << "Replaced a picture with a missing file with the text pad: " << file_name << endl;
#if TT_DEBUG_ON
					if (tt_debug_mode == 1070403) {
						xml_debug_node(node,L"Can't find image file.");
					};
#endif
				};
										  };
				break;
//       case INTEGER_32_BIT_TAG:
			case INTEGER_TAG:
			case FLOATING_POINT_NUMBER_64_BIT_TAG:
			case RATIONAL_NUMBER_TAG: {
					Rational *number = new Rational();
					//tt_error_file() << "Created new Rational" << endl; // REMOVE!!! 201003
					//tt_error_file() << "Address is " << (unsigned long) number << endl;
					int edit_and_value_not_updated = xml_get_attribute_int(node,L"EditedAndValueNotUpdated",0);
					if (edit_and_value_not_updated == 2) {
						// new on 230704 to deal with completely rubbed out numbers
						number->set_edited(TRUE); // new on 071104 otherwise may look bad when loaded
						number->just_set_narrow_text(L"",0);					
					} else if (edit_and_value_not_updated != 0) {
						number->set_edited(TRUE); // new on 071104 otherwise may look bad when loaded
						BSTR text_value = xml_get_text(node);
						number->just_set_narrow_text(text_value,::SysStringLen(text_value));
						::SysFreeString(text_value);
					} else {
						NumberValue *number_value = xml_get_number_value(node);
						if (number_value != NULL) {
							number->set_value(number_value,FALSE,TRUE); // args added on 061102 so numbers on the floor don't change size
						}; // else warn??
					};
					sprite = number;
											  };
				break;
			case REMOTE_CONTROL_TAG: {
				RemoteIdentifier identifier = (RemoteIdentifier) xml_get_attribute_int(node,L"Code");
				// maybe worth making a better suited constructor for this?
				if (identifier == APPEARANCE_REMOTE) {
					sprite = new Picture();
				} else if (remote_picture(identifier)) {
					sprite = new Remote_Picture(NULL,identifier);
					sprite->handle_xml_tag(tag,node); // deal with stuff like language associated with this node
				} else if (remote_address(identifier)) {
					sprite = new Remote_Address(identifier); // start it off inactive regardless (new on 111202)
					sprite->handle_xml_tag(tag,node);
				} else if (force_feedback_effect(identifier)) {
					sprite = new Force_Remote_Integer(NULL,identifier);
				} else {
					sprite = new Remote_Integer(NULL,identifier);
				};
											 };
				break;
			case PICTURE_OF_TAG:
				sprite = make_indirect_picture(xml_get_sprite(node),FALSE); // new on 081002 -- simplified on 201202
				break;
			case SCALE_TAG:
				sprite = new Picture();
				sprite->set_show_all(FALSE);
				sprite->handle_xml_tag(tag,node); // to read attributes -- doesn't use NO_MORE_TAGS since same data type as PICTUREs
				break;
			case NEST_TAG:
				sprite = new Nest();
				break;
			case BIRD_TAG:
				sprite = new Bird();
				break;
			case BUILT_IN_SOUND_TAG:
				sprite = new BuiltInSound();
//				sprite->handle_xml_tag(tag,node);
				break;
			case FORCE_FEEDBACK_TAG:
				sprite = new ForceFeedback(0,NULL);
//				sprite->handle_xml_tag(tag,node);
				break;
			case SPEECH_TAG:
				sprite = new SpeechSound();
				break;
			case SOUND_TAG: {
				WaveSound *sound = new WaveSound();
				string unique_name = xml_get_attribute_string(node,L"UniqueName",NULL);
				if (unique_name != NULL) {
					sound->set_hash_string(unique_name);
					delete [] unique_name;
//#if TT_DEBUG_ON
//				} else {
//					xml_debug_node(node);
//#endif
				};
				sprite = sound;
								 };
				break;
			case MCI_SOUND_TAG:
				sprite = new MCISound();
				break;
			case FOREIGN_BIRD_TAG:
				sprite = new PrimitiveBird();
				break;
			case TRUCK_TAG:
				sprite = new Truck_Inside();
				break;
			case TRUCK_OUTSIDE_VIEWED_FROM_SIDE_TAG:
			case TRUCK_OUTSIDE_VIEWED_FROM_ABOVE_TAG: // new on 231202
				sprite = new Truck_Outside(0,0,(tag == TRUCK_OUTSIDE_VIEWED_FROM_ABOVE_TAG));
				break;
			case BOMB_TAG:
				sprite = new Bomb();
				break;
			case NOTEBOOK_TAG:
				if (xml_get_attribute_int(node,L"LivesInTooly",0)) { // new on 171102
					if (tt_toolbox == NULL) {
						tt_toolbox = new Toolbox();
						tt_toolbox->initialize();
					};
					sprite = pointer_to_toolbox_notebook();
					if (sprite != NULL) { // for robustness 150704
						sprite->remove_from_floor(FALSE); // new on 140103 - needed for time travel
					};
				} else {
					sprite = new Notebook();
				}; 
				break;
			case LEFT_PAGE_TAG:
			case RIGHT_PAGE_TAG: { // new on 280803
				Sprite *should_be_notebook = xml_get_sprite(node);
				if (should_be_notebook != NULL && should_be_notebook->kind_of() == PROGRAM_PAD) {
					Notebook *notebook = (Notebook *) should_be_notebook;
					if (tag == LEFT_PAGE_TAG) {
						sprite = notebook->pointer_to_left_page();
					} else {
						sprite = notebook->pointer_to_right_page();
					};
					if (sprite == NULL) {
						notebook->finish_switching_contents(); // maybe notebook hasn't had a chance to generate these pages
						// try again
						if (tag == LEFT_PAGE_TAG) {
							sprite = notebook->pointer_to_left_page();
						} else {
							sprite = notebook->pointer_to_right_page();
						};
					};
				};
				return(sprite); // done since recursively called on the notebook - new on 290803
								  };

			case BUTTON_TAG: // new on 240103
				sprite = new Button((SpriteCode) xml_get_attribute_int(node,L"ImageCode",WAND_BUTTON_SPRITE));
				break;
			case BIKE_PUMP_TAG: // new on 011102
				if (xml_get_attribute_int(node,L"LivesInTooly",0)) { // new on 171102
					if (tt_toolbox == NULL) {
						tt_toolbox = new Toolbox();
						tt_toolbox->initialize();
					};
					sprite = tt_toolbox_expander;
					sprite->remove_from_floor(FALSE); // new on 140103 - needed for time travel
				} else {
					sprite = new Expander();
				};
				break;
			case VACUUM_TAG:
				if (xml_get_attribute_int(node,L"LivesInTooly",0)) { // new on 171102
					if (tt_toolbox == NULL) {
						tt_toolbox = new Toolbox();
						tt_toolbox->initialize();
					};
					sprite = tt_toolbox_vacuum;
					sprite->remove_from_floor(FALSE); // new on 140103 - needed for time travel
				} else {
					sprite = new Vacuum();
				};
				break;
			case MAGIC_WAND_TAG:
				if (xml_get_attribute_int(node,L"LivesInTooly",0)) { // new on 171102
					if (tt_toolbox == NULL) {
						tt_toolbox = new Toolbox();
						tt_toolbox->initialize();
					};
					sprite = tt_toolbox_copier;
					sprite->remove_from_floor(FALSE); // new on 140103 - needed for time travel
				} else {
					sprite = new Copier();
				};
				break;
			case TOOLBOX_TAG: 
				toolbox_is_part_of_puzzle(); // no-op if not running puzzles
				if (xml_get_attribute_int(node,L"IsTooly",0)) {
					if (tt_toolbox == NULL) { // new on 140103 since may be time traveling
						tt_toolbox = new Toolbox();
						tt_toolbox->initialize();
					};
					sprite = tt_toolbox;
					sprite->remove_from_floor(FALSE); // new on 140103 - needed for time travel
				} else {
					Toolbox *toolbox = new Toolbox();
					tt_toolbox->initialize();
					toolbox->become_inert(); // a copy should be inert (for now?)
					sprite = toolbox;
				};
				// notice that this makes a copy of Tooly if one already exists - unlike the old format style - new on 041102
				break;
			case MARTIAN_TAG:
				if (tt_martian == NULL) { // new on 141299
					tt_martian = new Martian();
				} else {
					tt_martian->re_initialize(); // new on 250203
				};
				sprite = tt_martian;
//				sprite->remove_from_floor(FALSE); // new on 140103 - needed for time travel -- replaced by initialize above
				break;
			case TALK_BALLOON_TAG:
				sprite = new Talk_Balloon();
				break;
			case MOUSE_TAG:
				sprite = new Mouse();
				break;
			case APPEARANCE_TAG:
				// used by PICTURE_OF as of 201202
				return(xml_get_sprite(node,sprite)); // added sprite arg on 060103
			case TURNING_NOTEBOOK_PAGE_TAG:  // new on 181102
				sprite = new Sprite(NOTEBOOK_PAGE_TURN_SPRITE);
				break;
			//case PROGRAMMER_APPEARANCE_TAG: // replaced by HAND_TAG, etc. below
			//	sprite = new Programmer_Sprite((SpriteCode) xml_get_attribute_int(node,L"AppearanceCode",0));
			//	break;
			case HAND_TAG:
			case HAND_HOLDING_WAND_TAG:
				sprite = new Programmer_Sprite((SpriteCode) xml_get_attribute_int(node,L"AppearanceCode",0));
				break;
			case ARM_TAG: 
				sprite = new Sprite(ARM_ONLY_SPRITE,0,0,PROGRAMMER_ARM); // added extra args on 240803 so knows it is a PROGRAMMER_ARM
				break;
			case TOOLBOX_VIEWED_FROM_SIDE_TAG: // new on 090103
				sprite = new Sprite(TOOLBOX_SIDE_SPRITE,0,0,TOOLBOX);
				break;
			case PERSON_WALKING_TAG:
				sprite = new Programmer_Sprite(PERSON_WALKING_SPRITE);
				break;
			case PERSON_SITTING_TAG:
				sprite = new Programmer_Sprite(SIT_SPRITE);
				break;
			case PERSON_SITTING_WITH_HAT_TAG:
				sprite = new Programmer_Sprite(SIT_WITH_HAT_SPRITE);
				break;
			case PERSON_SITTING_WITH_HAIR_TAG:
				sprite = new Programmer_Sprite(SIT_WITH_HAIR_SPRITE);
				break;
			case DIZZY_PERSON_TAG:
				sprite = new Programmer_Sprite(DIZZY_PERSON_SPRITE);
				break;
			case DIZZY_PERSON_WITH_HAIR_TAG:
				sprite = new Programmer_Sprite(DIZZY_PERSON_WITH_HAIR_SPRITE);
				break;
			case DIZZY_PERSON_WITH_HAT_TAG:
				sprite = new Programmer_Sprite(DIZZY_PERSON_WITH_HAT_SPRITE);
				break;
			case LANDING_HELICOPTER_TAG:
				sprite = new Sprite(HELIOLND,0,0,HELICOPTER);
				sprite->set_ok_to_dump(FALSE); // new on 211103 so no risk of getting 2 helicopters while time traveling
				break;
			case FLYING_HELICOPTER_TAG:
				sprite = new Programmer_Sprite(HELIOFLY);
				sprite->set_scale_for_height_of_camera(FALSE); // new on 230804
				break;
			case EXPLOSION_TAG:
				sprite = new Sprite(EXPLOSION_SPRITE); // new on 030204
				break;
			case LINK_TAG: // obsolete as of 241202 but kept to process old encodings
				return(xml_load_link(node));
			case NOTHING_TAG: // e.g. in list of erased stuff in a vacuum
				return(NULL);
			case HOUSE_TAG: 
				if (floors_or_houses_ok) { // conditional new on 110803
					// new on 040803 - based on old format code in pad.cpp - builds a house near the programmer
					city_coordinate x,y;
					tt_programmer->pointer_to_appearance()->lower_left_corner(x,y);
					block_number block_x,block_y;
					block_address_at(x,y,block_x,block_y);
					Block *block = tt_city->block_at(block_x,block_y,TRUE); // (block_number) (x/tt_ideal_block_width),(block_number) (y/ideal_screen_height),TRUE);
					short int index;
					boolean lot_is_free = block->closest_free_lot(x,y,index); // resets x and y to good values
					if (lot_is_free) {
						House *house = new House(x,y,block,(HouseStyle) xml_get_attribute_int(node,L"StyleCode",0),FALSE); // final arg is new so house is built slowly
						xml_entity(node,house);
						block->add_house(house,index);
						set_sprites_to_active_now();
					} else {
						play_sound(PLOP_SOUND);
					};
					return(tt_global_picture); // not a sprite -- just an indication that there is no sprite but that's OK -- caller better not clobber this
				} else {
					return(NULL);
				};
			case FLOOR_TAG: // new on 040803
				if (floors_or_houses_ok) { // conditional new on 110803
					if (tt_programmer->kind_of() == PROGRAMMER_AT_FLOOR) {
						xml_entity(node,tt_programmer->pointer_to_appearance()->pointer_to_background()); // add items to current floor
						set_sprites_to_active_now();
					} else {
						play_sound(PLOP_SOUND);
					};
					return(tt_global_picture);
				} else {
					return(NULL);
				};
			case CITY_TAG: // if legitimate then handled by caller
			case ROOM_TAG:
			case GROUND_TAG:
//#if TT_DEBUG_ON
//				xml_debug_node(node,L"xml_load_sprite");
//#endif
				log("Attempted to paste into ToonTalk, a room, the ground outside, or a city when not expected."); // new on 270904
				return(tt_global_picture);  // returned NULL prior to 270904
			case MISSING_PICTURE_FILE_TAG: // new on 280204 {
				xml_node *child = first_node_that_is_an_element(node);
				if (child != NULL) {
					Sprite *result = xml_load_sprite(child,tag_token(child));
					// following new on 310105
					if (result != NULL && result->kind_of() == PICTURE) {
						Picture *picture = (Picture *) result;
						boolean flipped_over = xml_get_attribute_int(node,L"FlippedOver",0);
						if (flipped_over && !picture->is_flipped()) {
							picture->flip();
						} else if (!flipped_over && picture->is_flipped()) {
							picture->unflip();
						};
						city_coordinate width = xml_get_attribute_int(node,L"Width",0);
						city_coordinate height = xml_get_attribute_int(node,L"Height",0);
						if (width > 0 && height > 0) { // if one is set the other is too
							picture->set_size(width,height);
						};
					};
					return(result);
				} else {
					return(NULL); // warn?
				};
		};
		if (sprite == NULL) {
#if TT_DEBUG_ON
			xml_debug_node(node,L"xml_load_sprite");
#endif
			return(NULL);
		};
		if (type != NONE_GIVEN && sprite->kind_of() != type) { // new on 070103
			sprite->destroy(); // wrong type
			log("Loaded an object of un-expected type.",FALSE,TRUE);
#if TT_DEBUG_ON
			xml_debug_node(node,L"xml_load_sprite");
#endif
			return(NULL);
		};
	} else {
		switch (tag) {
			case PICTURE_OF_TAG: 
				// rewritten on 140803 since needed when previously flipped text pad dropped on another text pad
				if (sprite->kind_of() == PICTURE) {
					((Picture *) sprite)->set_indirection(xml_get_sprite(node),TRUE);
				}; // else warn??
				// commented out on 130103 since broke F11 after dropping text pad on picture (since is indirection to text)
				//if (sprite->kind_of() == PICTURE) {
				//	Sprite *indirection = ((Picture *) sprite)->pointer_to_indirection();
				//	if (indirection == NULL) {
				//		log("Warning: loading indirect picture XML into a non-indirect picture.",FALSE,TRUE);
				//	} else {
				//		sprite = indirection; // that is who the rest of the XML is for
				//	};
				//};
				break;
			case HAND_TAG:
			case HAND_HOLDING_WAND_TAG:
				sprite->just_set_appearance_code((SpriteCode) xml_get_attribute_int(node,L"AppearanceCode",0));
				break;
		};
//#if TT_DEBUG_ON -- removed on 201003 -- and restored just on those cases where it matters
		switch (tag) {
			// moved up on 201003 since it is only for these guys that this matters
			case INTEGER_TAG:
			case FLOATING_POINT_NUMBER_64_BIT_TAG:
			case RATIONAL_NUMBER_TAG: 
				if (sprite->kind_of() != INTEGER) {
					log("Warning: loading number XML into a non-number.",FALSE,TRUE);
				} else { // new on 170803 -- copied from branch where sprite == NULL
					NumberValue *number_value = xml_get_number_value(node);
					if (number_value != NULL) {
						((Number *) sprite)->set_value(number_value,FALSE,TRUE); 
					}; // else warn??
				};
				break;
				// following moved and removed from TT_DEBUG_ON section on 201003 as well
			case LINK_TAG: // obsolete as of 241202 but kept to process old encodings
				return(xml_load_link(node));
			case NOTHING_TAG: // e.g. in list of erased stuff in a vacuum
				return(NULL);
			case CITY_TAG: 
			case ROOM_TAG:
			case FLOOR_TAG:
			case GROUND_TAG:
				return(NULL); // warn? -- isn't a sprite in any case
#if TT_DEBUG_ON
			case TEXT_PAD_TAG:
				if (sprite->kind_of() != TEXT) {
					log("Warning: loading TextPad XML into a non-text pad.",FALSE,TRUE);
				};
				break;
			case BOX_TAG:
				if (sprite->kind_of() != CUBBY) {
					log("Warning: loading box XML into a non-box.",FALSE,TRUE);
				};
				break;
			case PICTURE_TAG:
				if (sprite->kind_of() != PICTURE) {
					log("Warning: loading picture XML into a non-picture.",FALSE,TRUE);
				};
				break;
			case ROBOT_TAG:
				if (sprite->kind_of() != ROBOT) {
					log("Warning: loading robot XML into a non-robot.",FALSE,TRUE);
				};
				break;
			case THOUGHT_BUBBLE_TAG: // new on 271102
				if (sprite->kind_of() != THOUGHT_BUBBLE) {
					log("Warning: loading thought bubble XML into a non-thought bubble.",FALSE,TRUE);
				};
				break;
			case USER_PICTURE_TAG: 
				if (sprite->kind_of() != PICTURE) {
					log("Warning: loading user picture XML into a non-picture.",FALSE,TRUE);
				};
				break;
			case REMOTE_CONTROL_TAG: 
//				if (sprite->kind_of() == REMOTE_PICTURE) {
//					sprite->handle_xml_tag(tag,node); // deal with stuff like language associated with this node
//				} else 
				// commented out the following on 050504 since might be INTEGER or REMOTE_PICTURE or ...
				//if (sprite->kind_of() != REMOTE_INTEGER) {
				//	log("Warning: loading remote control XML into a non-remote control.",FALSE,TRUE);
				//};
				break;
			case SCALE_TAG:
				if (sprite->kind_of() != PICTURE) { // check the appearance code as well?
					log("Warning: loading scale XML into a non-scale.",FALSE,TRUE);
				};
				break;
			case NEST_TAG:
				if (sprite->kind_of() != NEST) {
					log("Warning: loading nest XML into a non-nest.",FALSE,TRUE);
				};
				break;
			case BIRD_TAG:
				if (sprite->kind_of() != BIRD) {
					log("Warning: loading bird XML into a non-bird.",FALSE,TRUE);
				};
				break;
			case BUILT_IN_SOUND_TAG:
				if (sprite->kind_of() != SOUND) {
					log("Warning: loading built-in sound XML into a non-builtin sound.",FALSE,TRUE);
				};
				break;
			case FORCE_FEEDBACK_TAG:
				if (sprite->kind_of() != FORCE_FEEDBACK) {
					log("Warning: loading force XML into a non-force.",FALSE,TRUE);
				};
				break;
			case SPEECH_TAG:
				if (sprite->kind_of() != SOUND) {
					log("Warning: loading speech sound XML into a non-sound.",FALSE,TRUE);
				};
				break;
			case SOUND_TAG:
				if (sprite->kind_of() != SOUND) {
					log("Warning: loading user sound XML into a non-sound.",FALSE,TRUE);
				};       
				break;
			case MCI_SOUND_TAG:
				if (sprite->kind_of() != SOUND) {
					log("Warning: loading MCI sound XML into a non-sound.",FALSE,TRUE);
				};        
				break;
			case FOREIGN_BIRD_TAG:
				if (sprite->kind_of() != BIRD) {
					log("Warning: loading foreign bird XML into a non-bird.",FALSE,TRUE);
				};            
				break;
			case TRUCK_TAG:
				if (sprite->kind_of() != TRUCK) {
					log("Warning: loading inside truck XML into a non-truck.",FALSE,TRUE);
				}; 
				break;
			case TRUCK_OUTSIDE_VIEWED_FROM_SIDE_TAG:
			case TRUCK_OUTSIDE_VIEWED_FROM_ABOVE_TAG: // new on 231202
				if (sprite->kind_of() != TRUCK) {
					log("Warning: loading outdoor truck XML into a non-truck.",FALSE,TRUE);
				}; 
				break;
			case BOMB_TAG:
				if (sprite->kind_of() != BOMB) {
					log("Warning: loading bomb XML into a non-bomb.",FALSE,TRUE);
				};        
				break;
			case NOTEBOOK_TAG:
				if (sprite->kind_of() != PROGRAM_PAD) {
					log("Warning: loading notebook XML into a non-notebook.",FALSE,TRUE);
					xml_debug_node(node);
				};   
				break;
			case BIKE_PUMP_TAG: // new on 011102
				if (sprite->kind_of() != EXPANDER) {
					log("Warning: loading bike pump XML into a non-bike pump.",FALSE,TRUE);
				}; 
				break;
			case VACUUM_TAG:
				if (sprite->kind_of() != VACUUM) {
					log("Warning: loading vacuum XML into a non-vacuum.",FALSE,TRUE);
				};
				break;
			case MAGIC_WAND_TAG:
				if (sprite->kind_of() != COPIER) {
					log("Warning: loading wand XML into a non-wand.",FALSE,TRUE);
				};
				break;
			case TOOLBOX_TAG: 
				if (sprite->kind_of() != TOOLBOX) {
					log("Warning: loading toolbox XML into a non-toolbox.",FALSE,TRUE);
				};
				break;
			case MARTIAN_TAG:
				if (sprite->kind_of() != MARTIAN) {
					log("Warning: loading martian XML into a non-martian.",FALSE,TRUE);
				};
				break;
			case TALK_BALLOON_TAG:
				if (sprite->kind_of() != TALK_BALLOON) {
					log("Warning: loading talk balloon XML into a non-balloon.",FALSE,TRUE);
				};
				break;
			case MOUSE_TAG:
				if (sprite->kind_of() != MOUSE_WITH_HAMMER) {
					log("Warning: loading mouse XML into a non-mouse.",FALSE,TRUE);
				};
				break;
			case TURNING_NOTEBOOK_PAGE_TAG:  // new on 181102
				if (sprite->appearance_code() != NOTEBOOK_PAGE_TURN_SPRITE) {
					log("Warning: loading NOTEBOOK_PAGE_TURN_SPRITE XML into a non-NOTEBOOK_PAGE_TURN_SPRITE.",FALSE,TRUE);
				};
				break;
			case ARM_TAG: 
				if (sprite->appearance_code() != ARM_ONLY_SPRITE) {
					log("Warning: loading ARM_ONLY_SPRITE XML into a non-ARM_ONLY_SPRITE.",FALSE,TRUE);
				};
				break;
#endif
			//case TOONTALK_OBJECT_TAG: {
			//	allocate_serial_numbers(node); 
			//	Sprite *new_sprite = xml_get_sprite(node,sprite);
			//	set_sprites_to_active_now();
			//	return(new_sprite);
			//								  };
		};

	};
 #if TT_DEBUG_ON
	if (sprite->debug_counter == tt_debug_target && tt_debug_mode == 71004) {
		xml_debug_node(node,L"Debug target");
	};
	string narrow_tag_string;
	if (tt_debug_mode == 310303) {
		wide_string tag_string = tag_to_wide_string(tag);
		narrow_tag_string = copy_narrow_string(tag_string);
		DWORD duration = timeGetTime()-start;
		if (duration > 0) {
			for (int i = 0; i < debug_load_sprite_indent; i++) tt_error_file() << " ";
			tt_error_file() << (timeGetTime()-start) << " ms to process " << narrow_tag_string << "#" << debug_load_sprite_counter << endl;
		};
	};
 #endif
	if (guid != NULL) {
		sprite->set_guid(guid); // this is all that remains of what used to be the entire GUID processing here
	};
	if (guid_string == NULL) {
//	if (tt_xml_version_number < 3) { // experiment on 150703 to see if this works for notebook pages, etc.
		remember_if_serial_numbered(node,sprite); // for old (prior to 150703) XML
	};
   if (element == NULL) {
      element = xml_first_child(node);
   } else if (FAILED(element->get_nextSibling(&element))) { 
		// new on 081002 that if element already processed above go for next one
      // does this ever happen??
      return(sprite); 
   };
   if (element == NULL) {
      return(sprite);
   };
   sprite->just_set_inactive(); // new on 081002 so activated only if ACTIVE_TAG is encountered
   while (element != NULL) {  
      DOMNodeType type = node_type(element);
      if (type == NODE_ELEMENT) {
         sprite->handle_xml_tag(tag_token(element),element); // do I care if it returns FALSE??
         //Sprite *replacement = sprite->handle_xml_tag(tag_token(element),element);
         //if (replacement != NULL && replacement != sprite) {
         //   sprite->destroy();
         //   sprite = replacement;
         //};
      }; // capture other stuff for future versions??
      if (FAILED(element->get_nextSibling(&element))) { 
         // does this ever happen??
//         sprite->stop_pretending_to_be_in_notebook(); // undo the lie
         return(sprite); 
      };
   };
   //// alternatively could emit a stopped or notStopped tag and let it do this properly...
   //sprite->stop_pretending_to_be_in_notebook(); // undo the lie
   sprite->handle_xml_tag(NO_MORE_TAGS,node); // new on 041002 -- last arg was NULL prior to 041102
#if TT_DEBUG_ON 
	if (tt_debug_mode == 310303) {
		DWORD duration = timeGetTime()-start;
		if (duration > 0) {
			for (int i = 0; i < debug_load_sprite_indent; i++) tt_error_file() << " ";	
			tt_error_file() << (timeGetTime()-start) << " ms to process " << narrow_tag_string << "#" << debug_load_sprite_counter 
								 << " and all its tags." << endl;
		};
		debug_load_sprite_indent--;
		delete [] narrow_tag_string;
	};
#endif
   return(sprite);
};

Sprites *xml_load_sprites_from_floor(string xml_string) {
	xml_document *document = document_from_string(xml_string);
	if (document == NULL) return(NULL);
	xml_node *node = first_node_that_is_an_element(document);
	Sprites *sprites = NULL;
	if (node != NULL && tag_token(node) == FLOOR_TAG) {
		node = first_node_that_is_an_element(node);
		if (tag_token(node) == INSIDE_TAG) {
			sprites = xml_load_sprites(node);
		};
	};
	document->Release(); 
   return(sprites);
};

boolean xml_process_children(xml_node *parent, ChildTagHandler handler, Entity *entity, void *extra_info) {
   xml_node *node;
   if (parent->get_firstChild(&node) == S_FALSE) { // no children
      handler(entity,NO_MORE_TAGS,node,extra_info); // new on 071102
		return(TRUE);
   };
   while (node != NULL) {  
      DOMNodeType type;
      if (FAILED(node->get_nodeType(&type))) {
         return(FALSE); // report problem?
      };
      if (type == NODE_ELEMENT) {
         handler(entity,tag_token(node),node,extra_info); // decided on 071102 to not quit just because some part is defective
         //if (!handler(entity,tag_token(node),node,extra_info)) {
         //   return(FALSE); // something went wrong -- good idea to quit?? 061102
         //};
      };
      if (node->get_nextSibling(&node) == S_FALSE) break;
   };
   handler(entity,NO_MORE_TAGS,parent,extra_info); // new on 071102 -- parent was node prior to 231202 but that is NULL - broke working set info loading
	return(TRUE);
};

boolean xml_process_attributes(xml_node_map *attribute_map, AttributeHandler handler, void *extra_info) {
	long length = xml_attribute_length(attribute_map);
	if (FAILED(attribute_map->get_length(&length))) return(FALSE);
	for (int i = 0; i < length; i++) {
		xml_node *node;
		if (FAILED(attribute_map->get_item(i,&node))) return(FALSE);
		BSTR xml;
		node->get_xml(&xml); // could try the firstChild first since I just want the attribute name...
		handler(xml,extra_info);
		::SysFreeString(xml);
	};
	return(TRUE);
};

xml_node_map *xml_get_xml_node_map(xml_element *element) {
	xml_node_map *attribute_map;
	if (FAILED(element->get_attributes(&attribute_map))) return(NULL);
	return(attribute_map);
};

int xml_attribute_length(xml_node_map *map) {
	long length;
	if (FAILED(map->get_length(&length))) return(0);
	return(length);
};

IXMLDOMDocument *create_xml_document() {
   IXMLDOMDocument *document;
   if (FAILED(CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&document)))  {
      return(NULL); // report problem?
   };
   return(document);
};

xml_element *create_xml_element(wide_string tag, xml_document *document) {
   xml_element *element;
   if (FAILED(document->createElement(tag,&element))) {
      HRESULT result = document->createElement(tag,&element);
      tt_error_file() << (int) result << endl;
      return(NULL);
   };
   return(element);
};

xml_element *create_xml_element(wide_string tag, xml_document *document, long long_value) {
   xml_element *element = create_xml_element(tag,document);
	add_xml_value(element,document,long_value);
	return(element);
};

xml_element *create_xml_element(wide_string tag, xml_document *document, Sprite *sprite) {
	// this should only be called if no chance sprite is shared
	xml_element *element = create_xml_element(tag,document);
	add_xml_value(element,document,sprite);
	return(element);
};

xml_element *create_xml_element(wide_string tag, xml_document *document, string narrow_value, long string_length) {
	xml_element *element = create_xml_element(tag,document);
	add_xml_value(element,document,narrow_value,string_length);
	return(element);
};

xml_element *create_xml_element(wide_string tag, xml_document *document, wide_string value) {
	xml_element *element = create_xml_element(tag,document);
	add_xml_value(element,document,value);
	return(element);
};

xml_element *create_xml_element(wide_string tag, xml_document *document, NumberValue *value) {
   xml_element *element = create_xml_element(tag,document);
	value->xml(element,document);
   return(element);
};

void add_xml_value(xml_element *element, xml_document *document, long long_value) { // rewritten on 120703
	if (element == NULL) return;
   wide_character value[20]; // enough for any long
   _itow(long_value,value,10);
   xml_text *text = create_xml_text(value,document);
   xml_append_child(text,element);
};

void add_xml_value(xml_element *element, xml_document *document, wide_string value) {
	if (element == NULL) return;
   xml_text *text = create_xml_text(value,document);
   xml_append_child(text,element);
};

void add_xml_value(xml_element *element, xml_document *document, string narrow_value, long string_length) {
	if (element == NULL) return;
   wide_string value = copy_wide_string(narrow_value,string_length);
   add_xml_value(element,document,value);
   delete [] value;
};

void add_xml_value(xml_element *element, xml_document *document, Sprite *sprite) {
	// this should only be called if no chance sprite is shared
	if (element == NULL) return;
	xml_element *sprite_element = create_xml_element(sprite->xml_tag(),document);
	sprite->xml(sprite_element,document);
	xml_append_child(sprite_element,element);
};

xml_element *create_xml_element_shared(wide_string tag, xml_document *document, Sprite *sprite) {// , boolean include_file_ok) {
	xml_element *element = create_xml_element(tag,document);
	sprite->xml_create_and_append_element(element,document,TRUE)->Release();;
	return(element);
};

//xml_element *create_xml_element(wide_string tag, xml_document *document, Sprite *sprite, boolean might_be_shared) {
//   xml_element *element;
//   if (FAILED(document->createElement(tag,&element))) {
//      return(NULL);
//   };
//   if (sprite == NULL) return(element);
//   if (might_be_shared) {
//      sprite->xml_create_and_append_element(element,document); 
//   } else {
//      // this assumes that sprite is not "shared" with anything
//      xml_element *sprite_element = sprite->xml_create_element(document);
//      sprite->xml(sprite_element,document);
//      xml_append_child(sprite_element,element); 
//   };
//   return(element);
//};


xml_element *xml_character_data_element(wide_string text, wide_string tag, xml_document *document) { // new on 121102
   xml_element *element = create_xml_element(tag,document);
   if (text == NULL) return(element); // prior to 151102 returned NULL
   xml_character_data *data = create_xml_character_data(text,document);
   xml_append_child(data,element);
   return(element);
};

xml_element *xml_character_data_element(string text, wide_string tag, xml_document *document) { // new on 121102
   xml_element *element = create_xml_element(tag,document);
   if (text == NULL) return(element); // prior to 151102 returned NULL
   xml_character_data *data = create_xml_character_data(text,document); // this will convert to wide_string
   xml_append_child(data,element);
   return(element);
};

xml_character_data *create_xml_character_data(wide_string data_string, xml_document *document) {
   xml_character_data *data;
   if (FAILED(document->createCDATASection(data_string,&data))) {
		// error handling new on 040704
		tt_error_file() << "Failed to create a CDATA section. Error code is " << GetLastError() << endl;
		string narrow_data_string = copy_narrow_string(data_string);
		tt_error_file() << "Data string is:" << endl;
		tt_error_file() << narrow_data_string << endl;
		delete [] narrow_data_string;
      return(NULL);
   };
   return(data);
};

xml_character_data *create_xml_character_data(string data_string, xml_document *document) {
	if (data_string == NULL) return(NULL); // new on 270103 -- rewritten on 040704
   wide_string wide_data_string = copy_wide_string(data_string);
   xml_character_data *data;
   if (FAILED(document->createCDATASection(wide_data_string,&data))) {
      delete [] wide_data_string;
      return(NULL);
   };
   delete [] wide_data_string;
   return(data);
};

xml_text *create_xml_text(wide_string data_string, xml_document *document) {
   xml_text *data;
   if (FAILED(document->createTextNode(data_string,&data))) {
      return(NULL);
   };
   return(data);
};

xml_document *document_from_string(string xml_string, int length, boolean originally_from_a_file, boolean warn) { 
	// abstracted on 230203 -- length arg added 281003
	// should really have been named xml_document_from_string
	// originally_from_a_file added 291104 to handle parse errors better
	// warn is new on 181204 to selectively turn off warnings about parse errors
#if TT_DEBUG_ON 
   DWORD start;
	if (tt_debug_mode == 230203) {
		start = timeGetTime();
	}
#endif
	UnwindProtect<boolean> set(tt_loading_is_ok_to_abort,FALSE); // since can't interrupt XML load -- new on 250105
	xml_document *document = create_xml_document();
   VARIANT_BOOL bIsSuccessful;
   wide_string wide_xml_string = copy_wide_string(xml_string,length);
#if TT_DEBUG_ON
	if (tt_debug_mode == 230203) {
		tt_error_file() << "Took " << (timeGetTime()-start) << "ms to convert document string to wide string." << endl;
	};
#endif
   HRESULT result = document->loadXML(wide_xml_string,&bIsSuccessful);
//#if TT_DEBUG_ON
//   BSTR xml;
//   document->get_xml(&xml);
//   ::MessageBoxW(NULL,xml,L"Document Just Loaded",MB_OK);
//#endif
   if (FAILED(result))  {
		delete [] wide_xml_string;
      return(NULL); 
   };
	if (bIsSuccessful) {
		delete [] wide_xml_string;
      return(document);
   } else {
      xml_document *corrected_document = correct_or_report_parse_error(document,wide_xml_string,-1,originally_from_a_file,warn);
		// last arg was FALSE prior to 291104
		document->Release();
	   delete [] wide_xml_string;
		return(corrected_document);
   };
};

xml_document *document_from_string(wide_string xml_string, boolean originally_from_a_file, boolean warn) { // abstracted on 230203
	// originally_from_a_file new on 291104 to decide how important a parse error is
#if TT_DEBUG_ON
   DWORD start;
	if (tt_debug_mode == 230203) {
		start = timeGetTime();
	};
#endif
	xml_document *document = create_xml_document();
	if (xml_string == NULL || xml_string[0] == NULL) { // new on 140603
		return(document);
	};
   VARIANT_BOOL bIsSuccessful;
   HRESULT result = document->loadXML(xml_string,&bIsSuccessful);
   if (FAILED(result))  {
      return(NULL); 
   };
	if (bIsSuccessful) {
//		xml_debug_node(document);
      return(document);
   } else {
		xml_document *corrected_document = correct_or_report_parse_error(document,xml_string,-1,originally_from_a_file,warn); 
		// penultimate arg was FALSE prior to 291104
		document->Release();
		return(corrected_document);
//		string narrow_xml_string = copy_narrow_string(xml_string);
//      report_parse_error(document,narrow_xml_string,FALSE);
//		delete [] narrow_xml_string;
//		return(NULL);
   };
};

boolean xml_set_attribute(xml_element *element, wide_string name, wide_string value) {
   CComVariant variant_value = value;
   return(SUCCEEDED(element->setAttribute(name,variant_value)));
};

boolean xml_set_attribute(xml_element *element, wide_string name, string value) {
   CComVariant variant_value = value;
   return(SUCCEEDED(element->setAttribute(name,variant_value)));
};

boolean xml_set_attribute(xml_element *element, wide_string name, int value) {
   CComVariant variant_value = value;
   return(SUCCEEDED(element->setAttribute(name,variant_value)));
};

boolean xml_set_attribute(xml_element *element, wide_string name, LONGLONG value) { // new on 101202
 //  VARIANT variant_value;
	//variant_value.vt = VT_I8;
	//variant_value.llVal = value;
 //  return(SUCCEEDED(element->setAttribute(name,variant_value)));
	// re-written since the above doesn't work in Windows 98 (and probably Me as well)
	wide_character value_as_string[100]; // must be more than enough for a 64-bit integer
	_i64tow(value,value_as_string,10);
	return(xml_set_attribute(element,name,value_as_string));
};

boolean xml_set_attribute(xml_element *element, wide_string name, double value) { // new on 261102
   CComVariant variant_value = value;
   return(SUCCEEDED(element->setAttribute(name,variant_value)));
};

boolean xml_remove_attribute(xml_element *element, wide_string name) { // new on 240904
	return(SUCCEEDED(element->removeAttribute(name)));
};

boolean xml_append_child(xml_node *node, xml_node *parent) {
   if (node == NULL || parent == NULL) return(FALSE); // added parent test on 280803
//	xml_debug_node(node);
   boolean ok = SUCCEEDED(parent->appendChild(node,NULL)); // NULL was &node prior to 190804 -- resulting in a major memory leak!
#if TT_DEBUG_ON
	int ref_count;
#endif
	if (ok) {
		// watch out for situations like xml_append_child(tt_zip_description_archive_element,document) where document will be released
#if TT_DEBUG_ON
		ref_count = // for debugging XML ref counts
#endif
		node->Release(); // only parent needs it from now on -- new on 190804
#if TT_DEBUG_ON
		debug_ref_count(ref_count,node);
#endif
	};
	return(ok);
};

//boolean xml_append_child(xml_node *node, xml_document *document) {
//   if (node == NULL) return(FALSE);
//   return(SUCCEEDED(document->appendChild(node,&node))); 
//};
//
//boolean xml_append_child(xml_node *node, xml_element *parent) { // new on 311002
//   if (node == NULL) return(FALSE);
//   return(SUCCEEDED(parent->appendChild(node,&node))); 
//};
//
//boolean xml_append_child(xml_comment *node, xml_document *document) {
//   if (node == NULL) return(FALSE);
//   return(SUCCEEDED(document->appendChild(node,(xml_node **) &node))); 
//};
//
//boolean xml_append_child(xml_element *node, xml_document *document) {
//   if (node == NULL) return(FALSE);
//   return(SUCCEEDED(document->appendChild(node,(xml_node **) &node))); 
//};
//
//boolean xml_append_child(xml_processing_instruction *node, xml_document *document) {
//   if (node == NULL) return(FALSE);
//   return(SUCCEEDED(document->appendChild(node,(xml_node **) &node))); 
//};
//
//boolean xml_append_child(xml_character_data *node, xml_element *parent) {
//   if (node == NULL) return(FALSE);
//   return(SUCCEEDED(parent->appendChild(node,(xml_node **) &node))); 
//};
//
//boolean xml_append_child(xml_element *node, xml_element *parent) {
//   if (node == NULL) return(FALSE);
//   return(SUCCEEDED(parent->appendChild(node,(xml_node **) &node))); 
//};
//
//boolean xml_append_child(xml_text *node, xml_element *parent) {
//   if (node == NULL) return(FALSE);
//   return(SUCCEEDED(parent->appendChild(node,(xml_node **) &node))); 
//};

boolean add_processing_instruction(xml_document *document) {
	xml_processing_instruction *xml_processing_instruction;
   if (FAILED(document->createProcessingInstruction(L"xml",L"version='1.0' encoding='ISO-8859-1'",
																	 &xml_processing_instruction))) { 
		// encoding is lost when calling get_xml but is preserved when using save so added on 210803 -- 
		// not really appropriate for Japanese
      return(FALSE);
   };
	return(xml_append_child(xml_processing_instruction,document));
};

boolean add_comment(xml_document *document, wide_string string_comment) {
   xml_comment *comment;
   if (FAILED(document->createComment(string_comment,&comment))) {
      return(FALSE);
   };
   return(xml_append_child(comment,document));
};

xml_document *document_from_file(string file_name) {
   // abstracted on 041102
#if TT_DEBUG_ON // restored as debug version only on 040803
   DWORD start;
	if (tt_debug_mode == 230203) {
		start = timeGetTime();
	};
#endif
	if (!file_exists(file_name)) { // new on 040803
		return(NULL);
	};
	input_ascii_file_stream file;
	file.open(file_name,std::ios_base::in|std::ios_base::binary);
	char cookie[2];
	file.read(cookie,sizeof(cookie));
	if (file == NULL) { // new on 040903 to check for 0 byte files
		tt_error_file() << "ToonTalk tried to read the following file but it is empty: " << file_name << endl;
		tt_err_file_important = TRUE;
		return(NULL);
	};
	file.close();
	UnwindProtect<boolean> set(tt_loading_is_ok_to_abort,FALSE); // since can't interrupt XML load -- new on 250105
	if (is_zip_cookie((string) cookie)) { // updated 281204 cookie[0] == 'P' && cookie[1] == 'K') { // is a zip file
		unzFile archive = open_archive_to_unzip(file_name);
		if (archive == NULL) {
			return(NULL); // warn?
		};
#if TT_DYNA_ZIP
		//char quoted_file_name[MAX_PATH+2];
		//quote_file_name(main_xml_file_name,quoted_file_name);
		string xml_string = narrow_contents_of_current_file_in_archive(archive,main_xml_file_name); //experiment on 270603 to use main_xml_file_name rather than quoted_file_name
#else
		go_to_named_file_in_unzip_archive(archive,main_xml_file_name); 
		// wasteful to get the narrow contents and then convert to wide for XML
		string xml_string = narrow_contents_of_current_file_in_archive(archive);
#endif
		update_private_media_directory(archive);
		close_unzip_archive(archive);
#if TT_DEBUG_ON 
		if (tt_debug_mode == 230203) {
			tt_error_file() << "Took " << (timeGetTime()-start) << "ms to unzip " << file_name << endl;
		};
#endif
		if (xml_string == NULL) {
			return(NULL); // warn?
		};
		xml_document *document = document_from_string(xml_string,-1,TRUE); // args added on 291104 for parse error handling
		delete [] xml_string;
#if TT_DEBUG_ON 
		if (tt_debug_mode == 230203) {
			tt_error_file() << "Took " << (timeGetTime()-start) << "ms to unzip and load " << file_name << endl; 
		};
#endif
		return(document);
	};
   xml_document *document = create_xml_document();
   VARIANT_BOOL bIsSuccessful;
   CComVariant varValue = file_name;
   if (FAILED(document->load(varValue,&bIsSuccessful)))  {
      return(NULL); 
   };
#if TT_DEBUG_ON 
	if (tt_debug_mode == 230203) {
		tt_error_file() << "Took " << (timeGetTime()-start) << "ms to load " << file_name << endl;
	};
#endif
   if (bIsSuccessful) {
      return(document);
   } else {
		wide_string wide_file_name = copy_wide_string(file_name);
		xml_document *corrected_document = correct_or_report_parse_error(document,wide_file_name,-1,TRUE); // rewritten 150404
		delete [] wide_file_name;
		document->Release();
		return(corrected_document);
   };
   return(NULL);
};

boolean xml_entity(xml_document *document, Entity *entity) {
   // do I really care what this returns?
   xml_node *parent = first_node_that_is_an_element(document);
   if (parent == NULL)  {
      return(FALSE); // report problem?
   };
   return(xml_entity(parent,entity));
};

boolean xml_entity(xml_node *parent, Entity *entity, boolean also_no_more_tags) {
	// also_no_more_tags is new on 200204 to fix a bug with loading city where the power off attribute is reset
#if TT_DEBUG_ON
   if (tt_debug_mode == 280802) {
		xml_debug_node(parent,L"xml_entity");
   };
#endif
   xml_node *node;
   if (parent->get_firstChild(&node) == S_FALSE) { // no children
      return(entity->handle_xml_tag(NO_MORE_TAGS,parent)); // new on 061102
//      return(FALSE);
   };
   while (node != NULL) {  
      DOMNodeType type;
      if (FAILED(node->get_nodeType(&type))) {
         break; // return(FALSE); // report problem?
      };
      if (type == NODE_ELEMENT) {
         entity->handle_xml_tag(tag_token(node),node);
      };
      if (node->get_nextSibling(&node) == S_FALSE) break;
   };
	if (also_no_more_tags) {
		return(entity->handle_xml_tag(NO_MORE_TAGS,parent));
	} else {
		return(TRUE);
	};
};

//boolean xml_block(xml_node *parent, Block *block) {
//   xml_node *node;
//   if (parent->get_firstChild(&node) == S_FALSE) return(FALSE); // no children
//   while (node != NULL) {  
//      DOMNodeType type;
//      if (FAILED(node->get_nodeType(&type))) {
//         return(FALSE); // report problem?
//      };
//      if (type == NODE_ELEMENT) {
//         block->handle_xml_tag(tag_token(node),node);
//      };
//      if (node->get_nextSibling(&node) == S_FALSE) break;
//   };
//   return(block->handle_xml_tag(NO_MORE_TAGS,node));
//};
//
//boolean xml_house(xml_node *parent, House *house) {
//   xml_node *node;
//   if (parent->get_firstChild(&node) == S_FALSE) return(FALSE); // no children
//   while (node != NULL) {  
//      DOMNodeType type;
//      if (FAILED(node->get_nodeType(&type))) {
//         return(FALSE); // report problem?
//      };
//      if (type == NODE_ELEMENT) {
//         house->handle_xml_tag(tag_token(node),node);
//      };
//      if (node->get_nextSibling(&node) == S_FALSE) break;
//   };
//   return(house->handle_xml_tag(NO_MORE_TAGS,node));
//};

Sprite *xml_load_sprite_from_file(string file_name, boolean &aborted, Sprite *sprite) { // Variant better??
   xml_document *document = document_from_file(file_name);
   if (document != NULL) {
      return(xml_load_sprite(document,aborted,sprite,TRUE));
   } else {
      return(NULL);
   };
};

Sprite *xml_load_sprite(string xml_string, boolean &aborted, Sprite *sprite, boolean floors_or_houses_ok, boolean warn) { 
   xml_document *document = document_from_string(xml_string,-1,FALSE,warn);
	if (document == NULL) return(NULL);
	Sprite *sprite_result = xml_load_sprite(document,aborted,sprite,floors_or_houses_ok); 
	document->Release(); // new on 090103
   return(sprite_result);
};

Sprite *xml_load_sprite(wide_string xml_string, boolean &aborted, Sprite *sprite, boolean floors_or_houses_ok, boolean warn) { 
   xml_document *document = document_from_string(xml_string,FALSE,warn);
	if (document == NULL) return(NULL);
	Sprite *sprite_result = xml_load_sprite(document,aborted,sprite,floors_or_houses_ok); 
	document->Release(); // new on 090103
   return(sprite_result);
};

Sprite *xml_load_sprite(xml_document *document, boolean &aborted, Sprite *sprite, boolean floors_or_houses_ok) { 
	// added optional sprite arg on 111202 so it is clobbered rather than a new one consed up
   //xml_node *node;
   //if (FAILED(document->get_firstChild(&node)))  {
   //   return(NULL); // report problem?
   //};
   //DOMNodeType type;
   //if (FAILED(node->get_nodeType(&type))) {
   //   return(NULL); // report problem?
   //};
   //while (type != NODE_ELEMENT) {
   //   //BSTR temp1;
   //   //VARIANT temp2;
   //   //node->get_nodeName(&temp1);
   //   //node->get_nodeValue(&temp2);
   //   if (FAILED(node->get_nextSibling(&node))) {
   //      return(NULL); // report problem?
   //   };
   //   if (FAILED(node->get_nodeType(&type))) {
   //      return(NULL); // report problem?
   //   };
   //};
   // simplified on 041102
   xml_node *node = first_node_that_is_an_element(document);
	if (node == NULL) {
#if TT_DEBUG_ON
		if (document != NULL && tt_debug_mode == 201003) {
			xml_debug_node(document,L"first_node_that_is_an_element is NULL");
		};
#endif
		return(FALSE);
	};
	Tag tag = tag_token(node);
	// the following almost worked but changing the programmer's state while he/she was pasting lead to confusion...
	if (floors_or_houses_ok && tag == CITY_TAG) { // new on 300904
		tt_city_document_to_load = document;
		document->AddRef();
		return(tt_global_picture);
	};
	//	stop_sound(FALSE); // in case a repeating sound is going on -- note helicopter sound is restarted by programmer::load if need be
	//	tt_screen->initialize_background(tt_city); // just in case loading cares...
	//	tt_city->xml_entity_and_activate(document);
	//	return(tt_global_picture); // a token to indicate processed OK
	//} else {
	Sprite *result;
#if TT_LOADING_AND_SAVING_INTERRUPTABLE
	try { // try is new on 140105
		if (!tt_loading_a_city) { // condition new on 130405 since if loading a city don't reset this counter
			tt_sprites_loaded_count = 0; 
		};
		result = xml_load_sprite(node,tag,sprite,NONE_GIVEN,floors_or_houses_ok);
	} catch (int ) {
		aborted = TRUE;
		reset_grid(); // new on 200105
		tt_screen->screen_dirty();
		return(NULL);
	};
#else
	result = xml_load_sprite(node,tag,sprite,NONE_GIVEN,floors_or_houses_ok);
#endif	
	set_sprites_to_active_now(); // new on 300904 since may paste XML without ToonTalkObject tag (e.g. part of a larger clipping)
	return(result);
//	};
};

Sprites *xml_load_sprites(InputStream *stream, boolean NULL_element_ok) { // new on 170803
	xml_document *document = xml_from_stream(stream);
	if (document == NULL) {
		return(NULL); // warn??
	};
	Sprites *sprites = xml_load_sprites(document);
	xml_release_document(document);
	return(sprites);
};

Sprite *xml_load_sprite(InputStream *stream, boolean &aborted) {
	xml_document *document = xml_from_stream(stream);
	Sprite *sprite = xml_load_sprite(document,aborted);
	xml_release_document(document);
	return(sprite);
};

Sprites *xml_load_sprites(xml_document *document, boolean NULL_element_ok) { // new on 170803
   xml_node *node = first_node_that_is_an_element(document);
   if (node == NULL) return(FALSE);
//	UnwindProtect <boolean> set(tt_uudecoding_a_sprite,TRUE); // new on 180803 so I get copies when loading a list (e.g. in puzzle.cpp)
   Sprites *sprites = xml_load_sprites(node,NULL_element_ok);
//	xml_debug_node(node);
	set_sprites_to_active_now(); // new on 180803
	return(sprites);
};

wide_string xml_indent(wide_string xml, int xml_string_length, int indent_increment, int max_width, int &indented_xml_length, wide_string extra_processing_instructions) {
   // doesn't yet enforce max_width
	// added extra_processing_instructions on 210803 to deal with the problem that if I add an encoding to xml_processing_instruction it is removed
	// see http://msdn.microsoft.com/library/default.asp?URL=/library/en-us/dnxml/html/xmlencodings.asp
   int indent = 0;
   wide_string xml_indented = new wide_character[xml_string_length*(max_width/4)]; // 4 is shortest tag - e.g. <a/>
   indented_xml_length = 0;
   boolean end_tag_encountered = FALSE;
   int start_tags_on_this_line = 0;
   int column_start = 0;
   int max_width_for_cdata = max_width-3; // 3 for ]]> to close CDATA section
   for (int i = 0; i < xml_string_length; i++) {
      if ((xml[i] == '>' && end_tag_encountered) || (xml[i] == '/' && xml[i+1] == '>')) { // i+1 should be safe since XML needs to end with end tags
         indent -= indent_increment;
         if (!end_tag_encountered) xml_indented[indented_xml_length++] = xml[i++]; // only if />
         xml_indented[indented_xml_length++] = xml[i];
         xml_indented[indented_xml_length++] = '\n'; // new line
         column_start = indented_xml_length;
         start_tags_on_this_line = 0;
         for (int j = 0; j < indent; j++) { // space over
            xml_indented[indented_xml_length++] = ' ';
         };
         end_tag_encountered = FALSE;
      } else {
         if (xml[i] == '<') {
            if (xml[i+1] == '/') {
               end_tag_encountered = TRUE;
               if (xml_indented[indented_xml_length-1] == ' ') { // then remove indent_increment of them
                  indented_xml_length -= indent_increment;
               };
            } else if (xml[i+1] == '?') {
               // new on 140802 - copy without interpretation everything up to ?>
               while (i+1 < xml_string_length && !(xml[i] == '?' && xml[i+1] == '>')) {
                  xml_indented[indented_xml_length++] = xml[i++];
               };
					if (extra_processing_instructions != NULL) { // new on 210803
						int extra_processing_instructions_length = wide_string_length(extra_processing_instructions);
						for (int i = 0; i < extra_processing_instructions_length; i++) {
                     xml_indented[indented_xml_length++] = extra_processing_instructions[i];
						};
					};
               xml_indented[indented_xml_length++] = xml[i++]; // copy ?
               xml_indented[indented_xml_length++] = xml[i++]; // copy > // new on 311002
               xml_indented[indented_xml_length++] = '\r'; // new line // new on 311002
               xml_indented[indented_xml_length++] = '\n'; // new line // new on 311002
               // > will be copied after this conditional
            } else if (xml[i+1] == '!') { // new on 140802
               const wide_character start_cdata[] = L"[CDATA[";
               const int length_of_start_cdata = sizeof(start_cdata)/sizeof(wide_character)-1;
               if (compare_wide_strings_of_length(xml+i+2,start_cdata,length_of_start_cdata) == 0) { // is a character data section
                  // copy without interpretation everything up to ]]>
                  while (i+2 < xml_string_length && !(xml[i] == ']' && xml[i+1] == ']' && xml[i+2] == '>')) {
                     if (indented_xml_length-column_start == max_width_for_cdata) {
                        const wide_character end_and_start_cdata[] = L"]]>\n<![CDATA[";
                        // consider indenting this as well...
                        const int length_of_end_and_start_cdata = sizeof(end_and_start_cdata)/sizeof(wide_character)-1;
                        wmemcpy(xml_indented+indented_xml_length,end_and_start_cdata,length_of_end_and_start_cdata);
                        column_start = indented_xml_length+4; // for the ]]>\n
                        indented_xml_length += length_of_end_and_start_cdata;
                     };
                     if (xml[i] == '\n') {
                        column_start = indented_xml_length+1;
                     };
                     xml_indented[indented_xml_length++] = xml[i++];
                  };
                  xml_indented[indented_xml_length++] = xml[i++]; // copy ]
                  xml_indented[indented_xml_length++] = xml[i++]; // copy second ] 
               } else { // can it be a defective comment? - shouldn't be since this is dealing with 
                  // copy without interpretation everything up to !>
                  while (i+1 < xml_string_length && !(xml[i] == '-' && xml[i+1] == '-' && xml[i+2] == '>')) {
                     xml_indented[indented_xml_length++] = xml[i++];
                  };
                  xml_indented[indented_xml_length++] = xml[i++]; // copy -
                  xml_indented[indented_xml_length++] = xml[i++]; // copy second -
                  xml_indented[indented_xml_length++] = xml[i++]; // copy > // new on 311002
                  xml_indented[indented_xml_length++] = '\r'; // new line // new on 311002
                  xml_indented[indented_xml_length++] = '\n'; // new line // new on 311002
                  // > will be copied after this conditional
               };               
            } else { // not comment or xml header
               start_tags_on_this_line++;
               if (start_tags_on_this_line > 1) {
                  xml_indented[indented_xml_length++] = '\n'; // new line
                  column_start = indented_xml_length;
                  start_tags_on_this_line = 1; // this one counts
                  for (int j = 0; j < indent; j++) { // space over
                     xml_indented[indented_xml_length++] = ' ';
                  };
               };
               indent += indent_increment;
            };
         };
         if (xml[i] != 13 && xml[i] != 10 && xml[i] != 9) { 
            // new on 311002 to eliminate any white space -- not clear why it is there
            xml_indented[indented_xml_length++] = xml[i];
         };
      };
   };
   xml_indented[indented_xml_length] = NULL; // terminate
   return(xml_indented);
};

boolean xml_save_document(xml_document *document, string full_file_name, string extra_file_for_archive, string extra_media_file_names, boolean zip_files) {
	// added extra_media_file_names on 200903
	// should generalize so extra_file_for_archive can be many files
   if (document == NULL) return(FALSE);
//#if TT_DEBUG_ON - temporarily on 230203
   DWORD start;
	if (tt_debug_mode == 230203) {
		start = timeGetTime();
	};
// #endif
	// commented out on 220903 since interferes with saving city with media
//	UnwindProtect<xml_element *> set(tt_zip_description_archive_element,NULL); // new on 210903 so this gets its own media files and not the time travel or log archive
	if (zip_files) { // new on 230203 -- was tt_zip_files prior to 110104
		zipFile archive = open_archive_to_zip(full_file_name);
		if (archive != NULL) {
#if !TT_DYNA_ZIP
         open_zip_file(archive,main_xml_file_name); 
#endif
			BSTR node_text = NULL;
			document->get_xml(&node_text);
			if (tt_debug_mode == 230203) {
				tt_error_file() << "Took " << (timeGetTime()-start) << "ms to get wide text " << full_file_name << endl;
			};
			unsigned int wide_text_length_left = SysStringLen(node_text);
#if TT_DYNA_ZIP
			write_wide_string_to_zip_file(archive,main_xml_file_name,node_text,wide_text_length_left); // new on 290603
#else
			const int narrow_chunk_length = 16384;
			unsigned int wide_chunk_length = narrow_chunk_length/2;
			char narrow_node_text[narrow_chunk_length];
			wide_string wide_node_text_left = node_text;
			while (wide_text_length_left > 0) {
				if (wide_text_length_left < wide_chunk_length) {
					wide_chunk_length = wide_text_length_left; // last round
				};
				// trying CP_ACP as experiment on 090403 was CP_UTF8
				int narrow_characters_written = WideCharToMultiByte(CP_ACP,0,wide_node_text_left,min(wide_text_length_left,wide_chunk_length),
																					 narrow_node_text,narrow_chunk_length,NULL,NULL);
				write_zip_file(archive,(bytes) narrow_node_text,narrow_characters_written);
				wide_text_length_left -= wide_chunk_length;
				wide_node_text_left += wide_chunk_length;
			};
#endif
			close_zip_file(archive);
//			result = write_zip_file(archive,(bytes) node_text,length*sizeof(wide_character)); // this was in unicode
			::SysFreeString(node_text);
			save_media_files_in_archive(archive,extra_media_file_names,TRUE); // new on 200303
			if (extra_file_for_archive != NULL) {
//				string short_name = short_file_name(extra_file_for_archive,TRUE);
//				copy_file_or_url_to_zip_file(extra_file_for_archive,short_name,archive); 
				// rewritten 210703 - this is correct for snapshot.png - how about other things
				zip_file(archive,extra_file_for_archive,NULL,default_compression_level,TRUE); 
				// deleted original  (210703) - restored on 240703 since inconvenient when generating reports
				// on 110204 changed FALSE to TRUE to delete original after adding it to archive
//				delete [] short_name;
			};
			close_zip_archive(archive);
//#if TT_DEBUG_ON - temporarily on 230203
			if (tt_debug_mode == 230203) {
				tt_error_file() << "Took " << (timeGetTime()-start) << "ms to zip and save " << full_file_name << endl;
			};
//#endif
			return(TRUE);
		};
	} else {
		_variant_t varString = full_file_name;
		if (FAILED(document->save(varString))) {
			tt_error_file() << "Something went wrong trying to save the XML of a notebook as " << full_file_name << endl;
			return(FALSE);
		};
	};
//#if TT_DEBUG_ON - temporarily on 230203
	if (tt_debug_mode == 230203) {
		tt_error_file() << "Took " << (timeGetTime()-start) << "ms to save " << full_file_name << endl;
	};
//#endif
   return(TRUE);
};

boolean xml_release_document(xml_document *document) {
   if (document == NULL) return(FALSE);
   ULONG remaining_ref_count = document->Release();
   return(remaining_ref_count == 0);
};

xml_node *xml_shallow_copy(xml_node *node) { // new on 150303
   xml_node *node_clone;
   node->cloneNode(VARIANT_FALSE,&node_clone);
   return(node_clone);
};

xml_node *xml_clone_node(xml_node *node) { // new on 311002
   xml_node *node_clone;
   node->cloneNode(VARIANT_TRUE,&node_clone);
#if TT_DEBUG_ON
   if (tt_debug_mode == 311002) {
      BSTR xml;
      node->get_xml(&xml);
      ::MessageBoxW(NULL,xml,L"node",MB_OK);
      node_clone->get_xml(&xml);
      ::MessageBoxW(NULL,xml,L"node_clone",MB_OK);
   };
#endif
   return(node_clone);
};

xml_document *xml_copy_document(xml_node *node) { // new on 140603 since the cloneNode still seems to do some sharing - not clear why
   if (node == NULL) return(NULL);
	BSTR node_text = NULL;
   node->get_xml(&node_text);
	return(document_from_string(node_text,FALSE));
};

xml_node *xml_copy_first_element(xml_node *node) { // new on 201003 since the whole document doesn't nest inside another document
   xml_document *whole_document = xml_copy_document(node);
	if (whole_document == NULL) {
		return(NULL);
	};
	return(first_node_that_is_an_element(whole_document)); // does this leak the document itself??
};

Element_Sprite_Table *current_element_sprite_table = NULL;

//void reset_current_element_sprite_table() {
//	if (current_element_sprite_table == NULL) return;
//	current_element_sprite_table->reset_element_sprite_table();
//};

void associate_element_and_sprite(xml_element *element, Sprite *sprite) {
#if TT_DEBUG_ON
	if (sprite->debug_counter == tt_debug_target) {
		log("Associating element and debug target.");
	};
#endif
	if (current_element_sprite_table == NULL) {
		current_element_sprite_table = new Element_Sprite_Table();
	};
	current_element_sprite_table->associate_element_and_sprite(element,sprite);
};

xml_element *reference_to_existing_element(Sprite *sprite, xml_document *document) {
#if TT_DEBUG_ON
	if (sprite->debug_counter == tt_debug_target) {
		log("Searching of reference to existing elements of debug target.");
	};
#endif
	if (current_element_sprite_table == NULL) {
		return(NULL);
	};
	return(current_element_sprite_table->reference_to_existing_element(sprite,document));
};

xml_element *remove_existing_element(Sprite *sprite) {
#if TT_DEBUG_ON
	if (sprite->debug_counter == tt_debug_target) {
		log("Removing reference to existing elements of debug target.");
	};
#endif
	if (current_element_sprite_table == NULL) {
		return(NULL);
	};
	return(current_element_sprite_table->remove_existing_element(sprite));
};

void add_to_sprites_with_newly_minted_guids(Sprite *sprite) {
	if (current_element_sprite_table != NULL) {
		current_element_sprite_table->add_to_sprites_with_newly_minted_guids(sprite);
	};
};

Element_Sprite_Table *push_current_element_sprite_table() {
#if TT_DEBUG_ON
	if (tt_debug_mode == 100703 || tt_debug_mode == 160703) {
		tt_error_file() << "push_current_element_sprite_table during segment " << tt_current_log_segment;
		if (current_element_sprite_table == NULL) {
			tt_error_file() << " no old table." << endl;
		} else {
			tt_error_file() << " old table has " << current_element_sprite_table->element_sprite_table_size << " elements." << endl;
		};
	};
#endif
	Element_Sprite_Table *result = current_element_sprite_table;
	current_element_sprite_table = new Element_Sprite_Table();
	return(result);
};

void restore_current_element_sprite_table(Element_Sprite_Table *old_table, xml_document *document) {
#if TT_DEBUG_ON
	if (tt_debug_mode == 100703 || tt_debug_mode == 160703) {
		tt_error_file() << "restore_current_element_sprite_table during segment " << tt_current_log_segment;
		if (old_table == NULL) {
			tt_error_file() << " no old table." << endl;
		} else {
			tt_error_file() << " restored table has " << old_table->element_sprite_table_size << " elements." << endl;
		};
	};
#endif
	if (current_element_sprite_table != NULL) {
		if (old_table != NULL) {
			current_element_sprite_table->process_sprites_with_newly_minted_guids(old_table);
		};
		delete current_element_sprite_table;
	};	
	current_element_sprite_table = old_table;
};

void release_current_element_sprite_table() {
	if (current_element_sprite_table != NULL) {
		delete current_element_sprite_table;
		current_element_sprite_table = NULL;
	};
};

Element_Sprite_Table::~Element_Sprite_Table() {
	// replaces the older reset_element_sprite_table
	if (sprites != NULL) delete [] sprites;
	//for (int i = 0; i < element_sprite_table_size; i++) {
	//	if (elements[i] != NULL) {
	//		while (elements[i]->Release() > 0) {}; // experiment 190804
	//	};
	//};
#if TT_DEBUG_ON
	int ref_count;
#endif
	for (int i = 0; i < element_sprite_table_size; i++) {
		if (elements[i] != NULL) {
#if TT_DEBUG_ON
			ref_count = // for debugging XML ref counts
#endif
			elements[i]->Release(); // new on 190804
#if TT_DEBUG_ON
			debug_ref_count(ref_count,elements[i]);
#endif
		};
	};
	if (elements != NULL) delete [] elements;
	just_destroy_list(sprites_with_newly_minted_guids);
	//sprites = NULL;
	//elements = NULL;
	//element_sprite_table_size = 0;
	//max_element_sprite_table_size = 0;
};

void Element_Sprite_Table::associate_element_and_sprite(xml_element *element, Sprite *sprite) {
   if (element_sprite_table_size == max_element_sprite_table_size) { // grow it
      if (max_element_sprite_table_size < 32) {
         max_element_sprite_table_size = 32;
      } else {
         max_element_sprite_table_size *= 2;
      };
      SpritePointer *new_sprites = new SpritePointer[max_element_sprite_table_size];
      xml_element_pointer *new_elements = new xml_element_pointer[max_element_sprite_table_size];
      for (int i = 0; i < element_sprite_table_size; i++) {
         new_sprites[i] = sprites[i];
         new_elements[i] = elements[i];
      };
      if (sprites != NULL) delete [] sprites;
      if (elements != NULL) delete [] elements;
      sprites = new_sprites;
      elements = new_elements;
   };
#if TT_DEBUG_ON
	if (tt_debug_mode == 100703) { // && element_sprite_table_size == 4 && sprite->kind_of() == THOUGHT_BUBBLE) {
		tt_error_file() << "Adding ";
		sprite->print(tt_error_file());
		tt_error_file() << " as index " << element_sprite_table_size << " for segment " << tt_current_log_segment << " on frame " << tt_frame_number << endl;
		//if (sprite->kind_of() == THOUGHT_BUBBLE) {
		//	log("debug this");
		//};
//		xml_debug_node(element,L"debug this");
	};
	if (tt_debug_mode == 160703) {
		if (sprite->kind_of() == INTEGER) {
			long long_value;
			sprite->current_long_value(long_value);
			tt_error_file() << "associate_element_and_sprite Integer#" << sprite->debug_counter << " whose value is " << long_value << " for segment " << tt_current_log_segment << " on frame " << tt_frame_number << endl;
		};
	};
#endif
   sprites[element_sprite_table_size] = sprite;
   elements[element_sprite_table_size] = element;
#if TT_DEBUG_ON
	int ref_count = // for debugging XML ref counts
#endif
	element->AddRef(); // new on 190804
#if TT_DEBUG_ON
	debug_ref_count(ref_count,element);
#endif
//#if TT_DEBUG_ON
//   if (element_sprite_table_size == 14) {
//      tt_error_file() << "debug this" << endl;
//   };
//#endif
   element_sprite_table_size++;
};

xml_element *Element_Sprite_Table::remove_existing_element(Sprite *sprite) {
   //new on 310803 for dealing with nested calls to top_level_xml 
   for (int i = 0; i < element_sprite_table_size; i++) {
      if (sprite == sprites[i]) { // there is an old one
			xml_element *element = elements[i];
			elements[i] = NULL;
			return(element);
		};
	};
	return(NULL);
};

xml_element *Element_Sprite_Table::reference_to_existing_element(Sprite *sprite, xml_document *document) {
   // parent is NULL if sprite is top-level object - removed xml_element *parent on 250203
   for (int i = 0; i < element_sprite_table_size; i++) {
      if (sprite == sprites[i]) { // there is an old one
//			if (document != NULL) { // conditional new on 250203 to allow dual use
//				int index = i+1; // use 1-indexing so that 0 is an error value when converting string to number
//				if (!xml_set_attribute(elements[i],L"SerialNumber",index)) return(NULL); 
				// rewritten on 150703 to use GUIDs instead
#if TT_DEBUG_ON
				if (tt_debug_target == sprite->debug_counter) {
					log("Just found debug target in element_sprite table.");
				};
#endif
			   GUID *guid = sprite->pointer_to_guid();
				boolean newly_minted_guid = (guid == NULL);
				if (newly_minted_guid) {
					guid = sprite->create_guid();
					// new on 160703 that this checks leaders to see if xml_include stuff needs to be updated now
#if TT_DEBUG_ON
					if (tt_debug_target == sprite->debug_counter) {
						log("Just created a GUID for debug target.");
					};
#endif				
				};
				wide_character guid_string[guid_string_size]; 
				guid_as_hexadecimal(guid,(wide_string) guid_string);
				if (elements[i] != NULL && newly_minted_guid && tt_saved_city_file_name_when_demo_ends == NULL) { 
					// optimization introduced on 160703 so that this only happens once per dump
					xml_set_attribute(elements[i],L"GUID",(wide_string) guid_string);
#if TT_DEBUG_ON
					int ref_count = // for debugging XML ref counts
#endif
					elements[i]->Release(); // new on 200804
#if TT_DEBUG_ON
					debug_ref_count(ref_count,elements[i]);
#endif
					elements[i] = NULL;
//					sprite->check_and_update_include_locations(); // moved here on 180703 so if this calls this recursively all is well -- obsolete 160204
				};
//          xml_element *link_element = create_xml_element(L"Link",document);
//				BSTR tag_string;
//				elements[i]->get_nodeName(&tag_string);
				xml_element *link_element = create_xml_element(sprite->xml_tag(),document); // rewritten on 241202
#if TT_DEBUG_ON
				if (tt_debug_mode == 100703) { // && index == 4 && sprite->kind_of() == THOUGHT_BUBBLE) {
					tt_error_file() << "Found reference to ";
					sprite->print(tt_error_file());
					tt_error_file() << " as index " << i << " for segment " << tt_current_log_segment << " on frame " << tt_frame_number << endl;
					//if (sprite->kind_of() == THOUGHT_BUBBLE) {
					//	log("debug this");
					//};
				};
				if (tt_debug_mode == 160703) {
					if (sprite->kind_of() == INTEGER) {
						long long_value;
						sprite->current_long_value(long_value);
						tt_error_file() << "found reference_to_existing_element #" << i << " for Integer#" << sprite->debug_counter << " whose value is " << long_value << " for segment " << tt_current_log_segment << " on frame " << tt_frame_number << endl;
					};
				};
#endif
//				if (!xml_set_attribute(link_element,L"SameAs",index)) return(NULL); // was SerialNumber prior to 241202
				if (tt_saved_city_file_name_when_demo_ends == NULL) {
					xml_set_attribute(link_element,L"Link",guid_string); // rewritten 150703
				};
				return(link_element);
         // xpath stuff didn't work well and besides it only linked to some node rather than the sprite itself
         //wide_string xpath = xpath_between(parent,elements[i],document);
         //if (xpath != NULL) {
         //   return(create_xml_element(L"Link",document,xpath));
         //} else { // warn?
         //   return(NULL);
         //};
      };
   };
#if TT_DEBUG_ON
	if (tt_debug_mode == 160703) {
		if (sprite->kind_of() == INTEGER) {
			long long_value;
			sprite->current_long_value(long_value);
			tt_error_file() << "failed to find reference_to_existing_element for Integer#" << sprite->debug_counter << " whose value is " << long_value << " for segment " << tt_current_log_segment << " on frame " << tt_frame_number << endl;
			//if (long_value > 1) {
			//	log("debug this");
			//};
		};
	};
#endif
   return(NULL);
};

void Element_Sprite_Table::add_to_sprites_with_newly_minted_guids(Sprite *sprite) {
	sprites_with_newly_minted_guids = new Sprites(sprite,sprites_with_newly_minted_guids);
};

void Element_Sprite_Table::process_sprites_with_newly_minted_guids(Element_Sprite_Table *element_sprite_table) { // new on 310803
	if (sprites_with_newly_minted_guids != NULL) {
		Sprites *remaining = sprites_with_newly_minted_guids;
		while (remaining != NULL) {
			Sprite *sprite = remaining->first();
			xml_element *old_element = element_sprite_table->remove_existing_element(sprite);
			// don't need to have it in the table since given a GUID
			if (old_element != NULL) {
				if (tt_saved_city_file_name_when_demo_ends == NULL) {
				   wide_character guid_string[guid_string_size]; 
				   guid_as_hexadecimal(sprite->pointer_to_guid(),(wide_string) guid_string);
				   xml_set_attribute(old_element,L"GUID",(wide_string) guid_string);
#if TT_DEBUG_ON
				   xml_debug_node(old_element);
#endif
				};
			} else {
				// maybe this is nested multiple levels down so pass this up
				element_sprite_table->add_to_sprites_with_newly_minted_guids(sprite);
			};
			remaining->set_first(NULL);
			remaining = remaining->rest();
		};
		delete sprites_with_newly_minted_guids;
		sprites_with_newly_minted_guids = NULL;
	};
};

void dump_xml_include_file(xml_element *element, int include_location) {
#if TT_DEBUG_ON
	millisecond start;
	if (tt_debug_mode == 1507033) {
		start = timeGetTime();
	};
#endif
	BSTR node_text = NULL;
	element->get_xml(&node_text);
#if TT_DEBUG_ON
	char file_name[100];
#else
	char file_name[26]; // two 32-bit integers in base 10 and a dot
#endif
	itoa(include_location,file_name,10);
	int length = strlen(file_name);
	file_name[length++] = '.'; // separator
	itoa(tt_current_log_segment,file_name+length,10);
#if TT_DEBUG_ON
	if (tt_debug_mode != 1507033) { // don't interfere with timing
		strcat(file_name,".");
		BSTR tag_string;
		if (SUCCEEDED(element->get_nodeName(&tag_string))) {
			string narrow_tag = copy_narrow_string(tag_string);
			strcat(file_name,narrow_tag);
			delete [] narrow_tag;
			::SysFreeString(tag_string);
		};
		if (tt_debug_mode == 100703 || tt_debug_mode == 160703) {
			tt_error_file() << "Just created " << file_name << endl;
		};
	};
#endif
	open_zip_file(tt_log_out_archive,file_name,maximum_compression_level); 
	write_zip_file(tt_log_out_archive,(bytes) node_text,SysStringByteLen(node_text)+2); // +2 to include terminating NULL
#if TT_DEBUG_ON
	if (tt_debug_mode == 1507033) {
		tt_error_file() << (timeGetTime()-start) << "ms to zip and clean up include #" << include_location << " which is " << SysStringByteLen(node_text) << " bytes long." << endl;
	};
#endif
	::SysFreeString(node_text);
};

void dump_xml_to_zip_file(xml_element *element, string file_name) { // copied and abstracted on 190703
	BSTR node_text = NULL;
	element->get_xml(&node_text);
	string narrow_node_text = copy_narrow_string(node_text,::SysStringLen(node_text));
	open_zip_file(tt_log_out_archive,file_name); // default compression probably best so can be opened by all
	write_zip_file(tt_log_out_archive,(bytes) narrow_node_text,strlen(narrow_node_text)); 
	delete [] narrow_node_text;
	::SysFreeString(node_text);
};


//void xml_color_index(Text *text, xml_node *color_index_node) {
//   // since Pictures do this too maybe should make set_color virtual in Sprite
//   xml_node *node;
//   HRESULT result = color_index_node->get_firstChild(&node);
//   if (result == S_FALSE) return;
//   if (FAILED(result)) {
//      return; // report problem?
//   };
//   text->set_color(xml_get_int(node));
//};

/*
void display_elements_recursively(xml_node *parent) {
   xml_node *node;
   HRESULT result = parent->get_firstChild(&node);
   if (result == S_FALSE) return;
   if (FAILED(result)) {
      return; // report problem?
   };
   while (node != NULL) {  
      DOMNodeType type;
      result = node->get_nodeType(&type);
      if (FAILED(result)) {
         return; // report problem?
      };
      if (type == NODE_ELEMENT) {
         BSTR tag;
         //result = element->get_tagName(&tag);
         node->get_nodeName(&tag);
         if (FAILED(result)) {
            return; // report problem?
         };
         ::MessageBoxW(NULL,tag,L"Tag Name",MB_OK);
         ::SysFreeString(tag);
         display_elements_recursively(node);
      } else if (type == NODE_TEXT) {
         BSTR item_text;
         result = node->get_text(&item_text);
         ::MessageBoxW(NULL,item_text,L"Text Node",MB_OK);
         ::SysFreeString(item_text);
      } else {
         char number_string[20];
         itoa((int) type,number_string,10);
         MessageBox(NULL,number_string,"Another Type",MB_OK);
      };
      result = node->get_nextSibling(&node);
      if (result == S_FALSE) break;
      if (FAILED(result)) {
         return; // report problem?
      };
   };
};
*/

   //init
   //HRESULT result = document->CreateInstance(_T("Msxml2.DOMDocument.4.0"));
   //if (FAILED(result)) {
   //   return; // report problem?
   //};      
   // load a document
   //_variant_t varXml(_T("c:\\temp\\temp.xml"));
   //_variant_t varOut((bool)TRUE);
   //varOut = document->load(varXml);
//   if ((bool)varOut == FALSE) {// Show error description - IXMLDOMParseError sample.

      //result = node->get_xml(&xml_string);
      //MessageBoxW(NULL, xml_string, L"Document from its root", MB_OK);
      //display_elements_recursively(node);

/*  xml_nodeList *node_list = NULL;
   HRESULT result = element->get_childNodes(&node_list);*/

   //// Get the length of the list returned by the previous find.
   //long length = 0;
   //result = node_list->get_length(&length);
   //if (FAILED(result)) {
   //   return; // report problem?
   //}; 
   //node_list->reset();
   //// Loop through the elements found and display the contents.
   //for (int i = 0; i < length; i++) {
   //   xml_node *node = NULL; 
   //   result = node_list->get_item(i,&node);
   //   if (FAILED(result)) {
   //      return; // report problem?
   //   };         
   //   if (node != NULL) {
         //BSTR item_text = NULL;
         //result = node->get_text(&item_text);
         //if (FAILED(result)) {
         //   return; // report problem?
         //}; 
//         if (item_text != NULL) {
//            ::MessageBoxW(NULL,item_text,L"Child Nodes",MB_OK);
////             ::SysFreeString(item_text);
//         };

//VARIANT value;
            //result = node->get_nodeTypedValue(&value);
            //if (FAILED(result)) {
            //   return; // report problem?
            //};
 //           IXMLDOMElement *element = (IXMLDOMElement *) &value;
            //IXMLDOMElement *element;
            //result = node->get_nodeTypedValue(element);
            //if (FAILED(result)) {
            //   return; // report problem?
            //};

            /*
   // Failed to parse stream, output error information. 
   IXMLError *pXMLError = NULL ; 
   XML_ERROR xmle; 

   HRESULT result = document->QueryInterface(IID_IXMLError, (void **)&pXMLError); 
   if (FAILED(result)) return;

   result = pXMLError->GetErrorInfo(&xmle); 
   if (pXMLError != NULL) pXMLError->Release();
   if (FAILED(result)) return;
   tt_error_file() << "Error on line " << xmle._nLine << ". Found " << xmle._pszFound << " while expecting " << xmle._pszExpected << endl;

   //printf("%s: Error on line %d. Found %S while expecting %S\r\n", 
   //         argv[0], 
   //         xmle._nLine, 
   //         xmle._pszFound, 
   //         xmle._pszExpected); 

   SysFreeString(xmle._pszFound); 
   SysFreeString(xmle._pszExpected); 
   SysFreeString(xmle._pchBuf); 
}*/

/*
   xml_node *child;
   if (FAILED(element_parent->get_firstChild(&child))) {
      return; // warn?
   };
   if (child == element) {
      return; // first one so need for an index
   };
   int index = 2; // one indexing - already checked first child
   while (SUCCEEDED(child->get_nextSibling(&child))) {
      if (child == element) {
         wcscat(xpath,L"[");
         _itow(index,xpath+wide_string_length(xpath),10); 
         wcscat(xpath,L"]");
         return;
      };
      if (child == NULL) {
         return; // warn!!
      };
   };
*/

/*
xml_nodeList *list;
   if (FAILED(element_parent->get_childNodes(&list))) {
      return; // warn??
   };
   long length;
   if (FAILED(list->get_length(&length))) {
      return; // report problem?
   };
   wcscat(xpath,L"/");
   wcscat(xpath,element_name);
   //if (length == 1) { // element is only child of element_parent of type element_name so no need for index
   //   return;
   //};
   list->reset(); // necessary??
   // Loop through the elements found and display the contents.
   int index = 1; // 1-indexing
   for (int i = 0; i < length; i++) {
      xml_node *node = NULL; 
      if (FAILED(list->get_item(i,&node))) {
         return; // report problem?
      };
      if (node == element) {
         if (index != 1) { // no need for index if 1
            wcscat(xpath,L"[");
            _itow(index,xpath+wide_string_length(xpath),10); 
            wcscat(xpath,L"]");
         };
         return;
      };
      BSTR node_name;
      if (SUCCEEDED(node->get_nodeName(&node_name)) && compare_wide_strings(node_name,element_name) == 0) {
         // found another of the same kind
         index++;
      };
   };
*/

/* XPATH stuff
const int max_xpath_length = 10000; // who knows?
wide_character xpath[max_xpath_length];

boolean has_ancestor(xml_element *element, xml_element *ancestor) {
   if (element == ancestor) return(TRUE);
   while (SUCCEEDED(element->get_parentNode((xml_node **) &element))) {
      if (element == ancestor) return(TRUE);
      if (element == NULL) return(FALSE);
   };
   return(FALSE);
};

void add_to_path_backwards(xml_element *element, xml_element *ancestor) {
   if (element == ancestor) return;
   xml_element *element_parent;
   element->get_parentNode((xml_node **) &element_parent); // must succeed since just did when has_ancestor was called
   if (element_parent == NULL) return;
   add_to_path_backwards(element_parent,ancestor);
   BSTR element_name;
   if (FAILED(element->get_nodeName(&element_name))) {
      return; // warn??
   };
   wcscat(xpath,L"/");
   wcscat(xpath,element_name);
   xml_node *previous_sibling = (xml_node *) element;
   int previous_sibling_count = 0;
   while (SUCCEEDED (previous_sibling->get_previousSibling(&previous_sibling))) {
      if (previous_sibling == NULL) break;
      BSTR previous_sibling_name;
      if (FAILED(previous_sibling->get_nodeName(&previous_sibling_name))) {
         return; // warn??
      };
      if (compare_wide_strings(previous_sibling_name,element_name) == 0) {
         previous_sibling_count++;
      };
   };
   if (previous_sibling_count > 0) {
      wcscat(xpath,L"[");
      _itow(previous_sibling_count+1,xpath+wide_string_length(xpath),10); // +1 since one-indexing
      wcscat(xpath,L"]");
   };
};

wide_string xpath_between(xml_element *parent, xml_element *destination, xml_document *document) {
   // finds the path by moving up from parent until an ancestor is found in common with xml_document
   xpath[0] = NULL; // start fresh
   wcscat(xpath,L"parent::*");
   while (!has_ancestor(destination,parent)) {
      wcscat(xpath,L"/parent::*");
      if (FAILED(parent->get_parentNode((xml_node **) &parent))) {
         return(NULL); // warn??
      };
   };
   // got up to common ancestor now go down to destination
   add_to_path_backwards(destination,parent);
   return(xpath); // caller better copy this if need be
};
*/

// noticed following is obsolete on 140105
//Sprite *sprite_from_archive_index(int index, Sprite *sprite) {
//	// can there ever already be a sprite that needs updating?? - asked 110703
////#if TT_DEBUG_ON
////	if (sprite != NULL) {
////		log("debug this");
////	};
////#endif
//	if (index < 1) {
//		return(sprite); // warn??
//	};
//	if (sprite == NULL) { // condition new on 150703
//		Sprite *existing_sprite = included_sprite(index);
//		if (existing_sprite != NULL) {
//			return(existing_sprite);
//		};
//	};
//#if TT_DEBUG_ON
//	millisecond start;
//	if (tt_debug_mode == 1507033) {
//		start = timeGetTime();
//	};
//#endif
//	wide_string node_text = (wide_string) narrow_contents_of_index_file_in_archive(tt_log_in_archive,index);
//#if TT_DEBUG_ON
//	if (tt_debug_mode == 1507033) {
//		tt_error_file() << (timeGetTime()-start) << "ms to get contents of archive #" << index << endl;
//		start = timeGetTime();
//	};
//#endif
//	Sprite *result = xml_load_sprite(node_text,sprite);
//	remember_included_sprite(index,result); // still a good idea? asked 150703
//	delete [] node_text;
//#if TT_DEBUG_ON
//	if (tt_debug_mode == 1507033) {
//		tt_error_file() << (timeGetTime()-start) << "ms to process and clean up." << endl;
//	};
//	if (result != NULL && result->kind_of() == NEST) {
//		debug_this();
//	};
//#endif
//	return(result);
//};

//Sprite *handle_include_tag(xml_node *node, Sprite *sprite) {
//	int location_index = xml_get_int(node,0);
//	Sprite *result = sprite_from_archive_index(location_index,sprite);
//	result->set_recent_xml_location(location_index);
//	return(result);
//};


