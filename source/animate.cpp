// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved.
  
#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif  
#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif  
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif
#if TT_XML
#if !defined(__TT_XML_H)   
#include "xml.h"
#endif
#endif
//#if TT_DEBUG_ON
//#if !defined(__TT_LOG_H)   
//#include "log.h" // for tt_current_log_segment
//#endif 
//#endif
#if !defined(__TT_ANIMATE_H)   
#include "animate.h"
#endif 

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Animation::Animation() :
		next(NULL) {
//	postpone_new_logs(); // new on 200100
#if TT_DEBUG_ON
	animation_counter = tt_animation_counter++;
#endif		
};  

Animation::~Animation() {
#if TT_DEBUG_ON
	if (tt_debug_mode == 180499) {
	   tt_error_file() << "Deleting animation#" << animation_counter << " on frame " << tt_frame_number << endl;
		animation_counter = -1; // in case it is used after this
	};
#endif
//	stop_postponing_new_logs(); // new on 200100
	if (next != NULL) {
		delete next;
	};
};

// this is so while running animations a sprite can update this flag
boolean callback_only = FALSE;

void set_callback_only(boolean new_value) {
	callback_only = new_value;
};

boolean all_at_once = FALSE;

void set_all_at_once(boolean new_value) {
	all_at_once = new_value;
};

Animation *Animation::update(boolean initial_all_at_once, boolean initial_callback_only) {
	all_at_once = initial_all_at_once;
	callback_only = initial_callback_only;
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	boolean first_guy_done = tick();
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
#if TT_DEBUG_ON
	if (tt_debug_mode == 180499) {
		tt_error_file() << "Ticked #" << return_animation_counter() << endl;
	};
#endif
	Animation *animations_left = next;
	Animation *previous_animations = this;
	while (animations_left != NULL) {
#if TT_DEBUG_ON
		if (tt_debug_mode == 180499) {
			tt_error_file() << "About to tick #" << animations_left->return_animation_counter() << endl;
		};
#endif
		if (animations_left->tick()) { // finished
#if TT_DEBUG_ON
			if (tt_debug_mode == 180499) {
				tt_error_file() << "Finished #" << animations_left->return_animation_counter() << endl;
			};
			add_to_log_if_interesting_error();
#endif
			Animation *temp = animations_left;
			previous_animations->set_rest(animations_left->rest());
			temp->set_rest(NULL);
			delete temp;
		} else {
#if TT_DEBUG_ON
			if (tt_debug_mode == 180499) {
				tt_error_file() << "Still running #" << animations_left->return_animation_counter() << endl;
			};
			add_to_log_if_interesting_error();
#endif
			previous_animations = animations_left;
		};
		animations_left = previous_animations->rest();
//#if TT_DEBUG_ON
//		if (tt_debug_mode == 180499 && animations_left != NULL && animations_left->return_animation_counter() < 0) {
//			tt_error_file() << "Shouldn't happen!" << endl;
//		};
//#endif
	};
	if (first_guy_done) {
		Animation *temp = next;
		next = NULL;
		delete this;
		return(temp);
	} else {
		return(this);
	};
};

millisecond Animation::last_stop_time() {
	if (next == NULL) return(stop_time());
	Animation *remaining = next;
	while (remaining != NULL) {
		if (remaining->rest() == NULL) {
			return(remaining->stop_time());
		};
		remaining = remaining->rest();
	};
	return(0); // shouldn't happen
};


void Animation::shift_viewpoint(city_coordinate delta_x, city_coordinate delta_y) { // new on 020204
	// subclasses do the actual work
	if (next != NULL) {
		next->shift_viewpoint(delta_x,delta_y);
	};
};

// didn't use to be sorted:
//	millisecond first_stop_time = stop_time();
//	if (next != NULL) {
//		return(max(first_stop_time,next->last_stop_time()));
//	} else {
//		return(first_stop_time);
//	};

Animation *Animation::insert(Animation *new_animation) {
	millisecond new_stop_time = new_animation->stop_time();
	if (new_stop_time < stop_time()) { // new one is first
		new_animation->set_rest(this);
		return(new_animation);
	};
	Animation *remaining = next;
	Animation *previous_remaining = this;
	while (remaining != NULL) {
		if (new_stop_time < remaining->stop_time()) {
			// found a place to insert it
			previous_remaining->set_rest(new_animation);
			new_animation->set_rest(remaining);
			return(this);
		};
#if TT_DEBUG_ON
		if (tt_debug_mode == 91199) {
			tt_error_file() << new_stop_time << " " << remaining->stop_time() << " at " << timeGetTime() << endl;
		};
#endif
		previous_remaining = remaining;
		remaining = remaining->rest();
	};
	// put it at the end
	previous_remaining->set_rest(new_animation);
	return(this);
};

Animation *Animation::remove_all_of(Sprite *sprite) {
	Animation *remaining = next;
   Animation *previous_remaining = this;
	while (remaining != NULL) {
		if (remaining->pointer_to_notify() == sprite) {  // splice it out
			previous_remaining->set_rest(remaining->rest());
         remaining->set_rest(NULL);
         delete remaining;
         remaining = previous_remaining->rest();
		} else {
         previous_remaining = remaining;
		   remaining = remaining->rest();
      };
	};
   if (pointer_to_notify() == sprite) { // first guy (i.e. this)
      return(next);
   } else {
	   return(this);
   };
};

#if TT_XML

void Animation::add_xml_each(xml_element *parent, xml_document *document) {
   // rewritten to be recursive since easier to process
   xml_element *animation_element = create_xml_element(xml_tag(),document);
   add_xml(animation_element,document);
   if (rest() != NULL) {
      rest()->add_xml_each(animation_element,document);
   };
	xml_append_child(animation_element,parent); // moved on 190804
};

 //  Animation *remaining = this;
	//while (remaining != NULL) {
 //     xml_element *animation_element = create_xml_element(tag,document);
 //     xml_append_child(animation_element,parent);
 //     remaining->add_xml(animation_element,document);
	//	remaining = remaining->rest();
	//};

Animation *xml_load_animation(Tag tag) {
   switch (tag) {
      case DO_AFTER_TAG: 
         return(new Do_After());
      case ANIMATE_TO_STATIC_GOAL_TAG:
         return(new EaseInAndOut_To_Static_Goal());
      case ANIMATE_TO_DYNAMIC_GOAL_TAG:
         return(new EaseInAndOut_To_Variable_Goal());
   };
   return(NULL); // shouldn't happen
};

boolean Animation::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
      case DO_AFTER_TAG:
         set_rest(new Do_After());
         return(xml_entity(node,rest()));
      case ANIMATE_TO_STATIC_GOAL_TAG:
         set_rest(new EaseInAndOut_To_Static_Goal());
         return(xml_entity(node,rest()));
      case ANIMATE_TO_DYNAMIC_GOAL_TAG:
         set_rest(new EaseInAndOut_To_Variable_Goal());
         return(xml_entity(node,rest()));
      case NO_MORE_TAGS:
         return(TRUE);
   };
   return(FALSE); // warn??
};

#endif

#if TT_DEBUG_ON
void Animation::debug() {
	Animation *remaining = next;
	while (remaining != NULL) {
		if (remaining == this) { 
			tt_error_file() << "debug" << endl;
		};
		remaining->debug();
		remaining = remaining->rest();
	};
};
#endif

long animating_variable_value(Sprite *sprite, AnimatingVariable variable) {
	if (sprite == NULL) {
		return(0); // new on 280803 for robustness
	};
   switch (variable) {
      case ANIMATING_LLX:
         return(sprite->current_llx());
      case ANIMATING_LLY:
         return(sprite->current_lly());
      case ANIMATING_WIDTH:
         return(sprite->current_width());
      case ANIMATING_HEIGHT:
         return(sprite->current_height());
      case ANIMATING_SIZE_PERCENT:
         return(sprite->current_size_percent());
      default:
         return(0); // should never happen
   };
};

void set_animating_variable(Sprite *sprite, AnimatingVariable variable, long new_value, boolean finished) {
	if (sprite == NULL) {
		return; // new on 180803 for robustness
	};
	if (finished) {
		// version that enables those who react to :set_size_and_position to do so (possibly 4 times) - new on 050803
		switch (variable) {
			case ANIMATING_LLX:
				sprite->set_llx(new_value,ANIMATION_COMPLETED);
				break;
			case ANIMATING_LLY:
				sprite->set_lly(new_value,ANIMATION_COMPLETED);
				break;
			case ANIMATING_WIDTH:
				sprite->set_width(new_value,ANIMATION_COMPLETED); 
				break;
			case ANIMATING_HEIGHT:
				sprite->set_height(new_value,ANIMATION_COMPLETED);
				break;
			case ANIMATING_SIZE_PERCENT:
				sprite->set_size_percent(new_value,ANIMATION_COMPLETED);
				break;
		};
	} else {
		switch (variable) {
			case ANIMATING_LLX:
				sprite->just_set_llx(new_value);
				break;
			case ANIMATING_LLY:
				sprite->just_set_lly(new_value);
				break;
			case ANIMATING_WIDTH:
				sprite->just_set_width(new_value); // was set_width prior to 050803
				break;
			case ANIMATING_HEIGHT:
				sprite->just_set_height(new_value);
				break;
			case ANIMATING_SIZE_PERCENT:
				sprite->just_set_size_percent(new_value);
				break;
		};
	};
};

EaseInAndOut_To_Goal::EaseInAndOut_To_Goal(Sprite *sprite,
                                           AnimatingVariable variable,
										             millisecond duration,
							                      Sprite *notify,
										             AnimationCallBack callback) :
  Animation(),
  sprite(sprite),
  variable(variable),
  duration(duration),
  end_time(tt_current_time+duration),
  notify(notify),
  callback(callback) {
	if (notify != NULL && sprite != notify) { // new on 130203 -- second condition new on 140104 to avoid cycles of references
		notify->increment_ref_count();
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 80904 && notify != NULL) {
		sprite->print(tt_error_file());
		tt_error_file() << " will at " << end_time << " run a callback. Frame=" << tt_frame_number << "; time=" << tt_current_time << endl;
	};
#endif
};

EaseInAndOut_To_Goal::~EaseInAndOut_To_Goal() {
	if (notify != NULL && sprite != notify) { // new on 130203 -- second condition new on 140104
		notify->destroy();
	};
};

#if TT_XML

void EaseInAndOut_To_Goal::add_xml(xml_element *parent, xml_document *document) {
//   xml_element *sprite_element = sprite->xml_create_and_append_element(parent,document);
	//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
	//	sprite->check_for_independence();
	//};
	if (sprite != NULL) { // condition new on 131003 - didn't use ALPHA2 since just adds robustness
//		xml_element *sprite_element = sprite->add_xml(parent,L"VariableHolder",document); // new on 140103
		// replaced above with following on 200804 to move xml_append_child to end
		xml_element *sprite_element = create_xml_element(L"VariableHolder",document);
		sprite->xml_create_and_append_element(sprite_element,document,TRUE)->Release();
		xml_set_attribute(sprite_element,L"VariableIndex",(int) variable);
		xml_append_child(sprite_element,parent);
	};
   xml_set_attribute(parent,L"Duration",duration);
   xml_set_attribute(parent,L"EndTime",end_time);
	if (callback != CALLBACK_NOTHING) { // conditional added 131202
		xml_set_attribute(parent,L"CallbackIndex",(int) callback);
	};
   if (notify != NULL) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	notify->check_for_independence();
		//};
      notify->add_xml(parent,L"Notify",document);
#if TT_DEBUG_ON
	if (tt_debug_mode == 80904 && notify != NULL) {
		sprite->print(tt_error_file());
		tt_error_file() << " will at " << end_time << " run a callback. Frame=" << tt_frame_number << "; time=" << tt_current_time << " saved XML." << endl;
	};
#endif
   };
};

boolean EaseInAndOut_To_Goal::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
		case NOTIFY_TAG: 
         notify = xml_get_sprite(node);
			if (notify != NULL && notify != sprite) { // new on 130203 // second and third conditions new on 140104
				notify->increment_ref_count();
			};
         return(TRUE);
		case VARIABLE_HOLDER_TAG:
			// new on 140103 since sprite_tag(tag) isn't so clean and missed Mouse
			sprite = xml_get_sprite(node);
			variable = (AnimatingVariable) xml_get_attribute_int(node,L"VariableIndex",(int) variable);
			return(sprite != NULL);
      case NO_MORE_TAGS:      
         duration = xml_get_attribute_int(node,L"Duration",duration);
         end_time = xml_get_attribute_int(node,L"EndTime",end_time);
         callback = (AnimationCallBack) xml_get_attribute_int(node,L"CallbackIndex",CALLBACK_NOTHING);
#if TT_DEBUG_ON
			if (tt_debug_mode == 80904 && callback != CALLBACK_NOTHING) {
				sprite->print(tt_error_file());
				tt_error_file() << " will at " << end_time << " run a callback. Frame=" << tt_frame_number << "; time=" << tt_current_time << " restored XML." << endl;
			};
#endif
         break; // and do Animation::handle_xml_tag
      default:
			// this was made obsolete on 140103 but keep it around for old saved cities
         if (sprite_tag(tag)) {
            sprite = xml_load_sprite(node,tag);
            variable = (AnimatingVariable) xml_get_attribute_int(node,L"VariableIndex",(int) variable);
            return(TRUE);
         };
   };
   return(Animation::handle_xml_tag(tag,node));
};

#endif

EaseInAndOut_To_Static_Goal::EaseInAndOut_To_Static_Goal(//long *the_variable, // replaced on 131102
                                                         Sprite *sprite,
                                                         AnimatingVariable variable,
                                                         long the_goal,
										                           millisecond duration,
//										                           boolean *to_set_when_finished,
										                           Sprite *notify,
										                           AnimationCallBack callback) :
  EaseInAndOut_To_Goal(sprite,variable,duration,notify,callback) {
  original_value = animating_variable_value(sprite,variable);
  difference = the_goal-original_value;
//#if TT_DEBUG_ON
//	if (tt_debug_mode == 50504 && variable == ANIMATING_WIDTH) { // 050504 to debug intro_v2
//		tt_error_file() << "the_goal = " << the_goal << "; original_value = " << original_value << "; difference = " << difference << endl;
//	};
//#endif
};

boolean variable_changed = FALSE; // why?? - asked on 131102

boolean any_variables_changed() {
	if (variable_changed) {
		variable_changed = FALSE;
		return(TRUE);
	} else {
		return(FALSE);
	};
};

boolean EaseInAndOut_To_Static_Goal::tick() {
	variable_changed = TRUE;
	if (tt_current_time >= end_time || all_at_once) {
		if (!callback_only) {
			set_animating_variable(sprite,variable,original_value+difference,TRUE);
		};
		if (callback != CALLBACK_NOTHING) {
//			AnimationCallBack saved_callback = callback;
         AnimationCallBackProcedure procedure = callback_procedure(callback); // rewritten on 121102
			callback = CALLBACK_NOTHING; // in case recursively called (e.g. finish_instantly)
			//Sprite *saved_notify = notify; // added on 190499 -- rewritten on 130203
			//notify = NULL; // so if remove_animations_of is called this guy isn't double deleted
#if TT_DEBUG_ON
			if (notify != NULL && notify->debug_counter == tt_debug_target) {
				tt_error_file() << "Running EaseInAndOut_To_Goal of debug target." << endl;
			};
#endif
			procedure(notify);
			if (notify != NULL && notify != sprite) { // new on 130203 // second condition new on 140104
				notify->destroy();
				notify = NULL;
			};
		};
		return(TRUE);
	};
	long percent = 100-((end_time-tt_current_time)*100)/duration;
	// ease in and out with the following
	// comment out if want to go back to Linear (maybe class names should change)
//	tt_error_file() << "linear percent " << percent << " ";
	if (percent < 20) {
		percent /= 2;
	} else if (percent < 80) {
		percent += (percent-20)/3-10;
	} else {
//		percent = 90+(100-percent)/2;
		percent = 90+(percent-80)/2;
	};
//	*variable += ((current_goal()+offset-*variable)*percent)/100;
	set_animating_variable(sprite,variable,original_value+(difference*percent)/100);
//#if TT_DEBUG_ON
//	if (tt_debug_mode == 50504 && tt_frame_number == 1161 && variable == ANIMATING_WIDTH) { // 050504 to debug intro_v2
//		tt_error_file() << "end_time = " << end_time << "; tt_current_time = " << tt_current_time << "; duration = " << duration
//						<< "; original_value = " << original_value << "; difference = " << difference << endl
//						<< " so new width is " << original_value+(difference*percent)/100 << endl;
//	};
//#endif
//	tt_error_file() << *variable << " value; percent is " << percent << endl;
	return(FALSE); // not done yet
};

void EaseInAndOut_To_Static_Goal::shift_viewpoint(city_coordinate delta_x, city_coordinate delta_y) {
	if (variable == ANIMATING_LLX) {
		original_value += delta_x;
	} else if (variable == ANIMATING_LLY) {
		original_value += delta_y;
	};
	Animation::shift_viewpoint(delta_x,delta_y);
};

#if TT_XML

void EaseInAndOut_To_Static_Goal::add_xml(xml_element *parent, xml_document *document) {
//#if TT_DEBUG_ON
//   if (difference < -100000) {
//      tt_error_file() << "debug this" << endl;
//   };
//#endif
   xml_set_attribute(parent,L"Difference",difference); 
   xml_set_attribute(parent,L"OriginalValue",original_value);
   EaseInAndOut_To_Goal::add_xml(parent,document);
};

boolean EaseInAndOut_To_Static_Goal::handle_xml_tag(Tag tag, xml_node *node) {
   if (tag == NO_MORE_TAGS) {
      difference = xml_get_attribute_int(node,L"Difference",difference);
      original_value = xml_get_attribute_int(node,L"OriginalValue",original_value);
   };
   return(EaseInAndOut_To_Goal::handle_xml_tag(tag,node));
};

#endif

EaseInAndOut_To_Variable_Goal::EaseInAndOut_To_Variable_Goal(Sprite *sprite,
                                                             AnimatingVariable variable,
                                                             Sprite *other_sprite,
                                                             AnimatingVariable other_variable, 
						                                           millisecond duration, // was called the_duration prior to 261102 so was ignored
						                                           Sprite *notify,
                                                             AnimationCallBack callback,
                                                             long offset) :
  EaseInAndOut_To_Goal(sprite,variable,duration,notify,callback),
  other_sprite(other_sprite),
  other_variable(other_variable),
  offset(offset) {
	  if (other_sprite != NULL) { // good practice and on 021003 discovered a case when moving a nest around while bird was delivering that this is needed
		  other_sprite->increment_ref_count();
	  };
};

EaseInAndOut_To_Variable_Goal::~EaseInAndOut_To_Variable_Goal() { // new on 021003
	if (other_sprite != NULL && !tt_resetting_or_destroying_city) { // second condition added 201003 since otherwise can have troubles time traveling (e.g. saving a city while Dusty is sucking)
		other_sprite->destroy(); // release reference
	};
};

long EaseInAndOut_To_Variable_Goal::current_goal() {
   return(animating_variable_value(other_sprite,other_variable));
};

boolean EaseInAndOut_To_Variable_Goal::tick() {
	variable_changed = TRUE;
	if (tt_current_time >= end_time || all_at_once) {
		if (!callback_only) {
			set_animating_variable(sprite,variable,current_goal()+offset,TRUE);
		};
		if (callback != CALLBACK_NOTHING) {
//			AnimationCallBack saved_callback = callback;
         AnimationCallBackProcedure procedure = callback_procedure(callback);
			callback = CALLBACK_NOTHING; // in case recursively called (e.g. finish_instantly)
			//Sprite *saved_notify = notify; // added on 190499 -- rewritten on 130203
			//notify = NULL; // so if remove_animations_of is called this guy isn't double deleted
#if TT_DEBUG_ON
			if (notify != NULL && notify->debug_counter == tt_debug_target) {
				tt_error_file() << "Running EaseInAndOut_To_Goal of debug target." << endl;
			};
#endif
			procedure(notify);
			if (notify != NULL && notify != sprite) { // new on 130203 // second condition new on 140104
				notify->destroy();
				notify = NULL;
			};
		};
		return(TRUE);
	};
   // adds (delta_time/time_left)*distance_to_goal
   long current_value = animating_variable_value(sprite,variable);
	set_animating_variable(sprite,variable,current_value+((current_goal()+offset-current_value)*tt_millisecond_delta)/(end_time-tt_current_time));
	return(FALSE); // not done yet
};

#if TT_XML

void EaseInAndOut_To_Variable_Goal::add_xml(xml_element *parent, xml_document *document) {
	//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
	//	other_sprite->check_for_independence();
	//};
#if TT_DEBUG_ON
	if (tt_debug_mode == 160703) {
		if (sprite->kind_of() == INTEGER) {
			long long_value;
			sprite->current_long_value(long_value);
			tt_error_file() << "About to generate GoalHolder XML for Integer#" << sprite->debug_counter << " whose value is " << long_value << " for segment " << tt_current_log_segment << " on frame " << tt_frame_number << endl;
		};
	};
#endif
	if (other_sprite != NULL) { // condition new on 131003 for robustness
//		xml_element *other_sprite_element = other_sprite->add_xml(parent,L"GoalHolder",document);
		// replaced above with following on 200804 to move xml_append_child to end
		xml_element *other_sprite_element = create_xml_element(L"GoalHolder",document);
		other_sprite->xml_create_and_append_element(other_sprite_element,document,TRUE)->Release();
#if TT_DEBUG_ON
		if (tt_debug_mode == 160703) {
			if (sprite->kind_of() == INTEGER) {
				long long_value;
				sprite->current_long_value(long_value);
				tt_error_file() << "Done generating GoalHolder XML for Integer#" << sprite->debug_counter << " whose value is " << long_value << " for segment " << tt_current_log_segment << " on frame " << tt_frame_number << endl;
			};
		};
#endif
		xml_set_attribute(other_sprite_element,L"GoalVariableIndex",(int) other_variable);
		xml_append_child(other_sprite_element,parent);
	};
   xml_set_attribute(parent,L"Offset",offset);
   EaseInAndOut_To_Goal::add_xml(parent,document);
};

boolean EaseInAndOut_To_Variable_Goal::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
      case GOAL_HOLDER_TAG:
         other_sprite = xml_get_sprite(node);
			if (other_sprite != NULL) { // new on 031003
				other_sprite->increment_ref_count();
			};
         other_variable = (AnimatingVariable) xml_get_attribute_int(node,L"GoalVariableIndex",(int) other_variable);
         return(TRUE);
      case NO_MORE_TAGS:
         offset = xml_get_attribute_int(node,L"Offset",offset);
         break;
   };
   return(EaseInAndOut_To_Goal::handle_xml_tag(tag,node));
};

#endif

/* obsolete as of 131102

boolean Set_After::tick() {
	if (tt_current_time >= end_time || all_at_once) {
		if (!callback_only) *to_set = future_value;
		return(TRUE);
	};
	return(FALSE); // not done yet
};
 

Increment_After::Increment_After(millisecond end_time, long *to_set, long increment) :
  Animation(),
  end_time(end_time),
  to_set(to_set),
  increment(increment) {
#if TT_DEBUG_ON
  if (tt_debug_mode == 33) {
		tt_error_file() << "Waiting until " << end_time
						<< " at " << tt_frame_number << endl;
	};
#endif
};

boolean Increment_After::tick() {
	if (tt_current_time >= end_time || all_at_once) {
		if (!callback_only) *to_set += increment;
		return(TRUE);
	};
	return(FALSE); // not done yet
};
*/

// was called True_When prior to 131102 but after removing flag *set_when_finished that no longer made sense
Do_After::Do_After(Sprite *owner, millisecond end_time, Sprite *notify, AnimationCallBack callback) :
  Animation(),
  owner(owner), // new on 140104
  end_time(end_time),
//  set_when_finished(set_when_finished),
  notify(notify),
  callback(callback) {
//  done(FALSE) 
#if TT_DEBUG_ON
  if (tt_debug_mode == 33) {
		tt_error_file() << "Waiting until " << end_time
						<< " at " << tt_frame_number << endl;
	};
#endif
  	if (notify != NULL && notify != owner) { // new on 130203 -- second condition new on 140104
		notify->increment_ref_count();
	};
};

Do_After::~Do_After() {
	if (notify != NULL && notify != owner) { // second condition new on 140104
		notify->destroy();
	};
};

boolean Do_After::tick() {
//	if (done) {
//		callback(notify);
//		return(TRUE);
//	};
	if (tt_current_time >= end_time || all_at_once) {
		//if (set_when_finished != NULL) {
		//	*set_when_finished = TRUE;
		//};
		if (callback != CALLBACK_NOTHING) {
//			AnimationCallBack saved_callback = callback;
         AnimationCallBackProcedure procedure = callback_procedure(callback);
			callback = CALLBACK_NOTHING; // in case recursively called (e.g. finish_instantly)
			//Sprite *saved_notify = notify; // added on 190499 -- rewritten on 130203 
			//notify = NULL; // so if remove_animations_of is called this guy isn't double deleted
#if TT_DEBUG_ON
			if (notify != NULL && notify->debug_counter == tt_debug_target) {
				tt_error_file() << "Running EaseInAndOut_To_Goal of debug target." << endl;
			};
#endif
			procedure(notify);
			if (notify != NULL && notify != owner) { // new on 130203 // second condition new on 140104
				notify->destroy();
				notify = NULL;
			};
		};
		return(TRUE);
//			done = TRUE;
//			return(FALSE);
//		} else {
//			return(TRUE);
	};
//	} else {
	return(FALSE); // not done yet
//	};
};

#if TT_XML

void Do_After::add_xml(xml_element *parent, xml_document *document) {
   if (notify != NULL) {
		//if (tt_dump_reason == DUMPING_PART_OF_CITY_FOR_TIME_TRAVEL) { // new on 100703 
		//	notify->check_for_independence();
		//};
      notify->add_xml(parent,L"Notify",document);
   }; 
	if (callback != CALLBACK_NOTHING) { // conditional added 131202
		xml_set_attribute(parent,L"CallbackIndex",(int) callback);
	};
   xml_set_attribute(parent,L"EndTime",end_time);
};

boolean Do_After::handle_xml_tag(Tag tag, xml_node *node) {
   switch (tag) {
      case NOTIFY_TAG: 
         notify = xml_get_sprite(node);
			if (notify != NULL && notify != owner) { // second condition new on 140104
				notify->increment_ref_count();
			};
         return(TRUE);
      case NO_MORE_TAGS:      
         end_time = xml_get_attribute_int(node,L"EndTime",end_time);
         callback = (AnimationCallBack) xml_get_attribute_int(node,L"CallbackIndex",CALLBACK_NOTHING);
         break; // and do Animation::handle_xml_tag
   };
   return(Animation::handle_xml_tag(tag,node));
};

#endif

