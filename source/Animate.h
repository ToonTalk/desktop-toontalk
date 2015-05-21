// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved
#ifndef __ANIMATE_H

#define __ANIMATE_H

class Animation;
typedef Animation *AnimationPointer;

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif

#if !defined(__TT_ENTITY_H)
#include "entity.h"
#endif

class Animation : public Entity {
  public:
	 Animation();
	 virtual ~Animation(); // virtual added 200100
	 virtual boolean tick() = 0;
	 Animation *update(boolean all_at_once, boolean callback_only);
	 millisecond last_stop_time();
	 virtual void shift_viewpoint(city_coordinate delta_x, city_coordinate delta_y);
	 virtual millisecond stop_time() = 0;
	 virtual void set_owner(Sprite *sprite) = 0; // new on 140104
	 Animation *insert(Animation *new_animation);
    Animation *remove_all_of(Sprite *sprite);
#if TT_XML
    void add_xml_each(xml_element *parent, xml_document *document);
    virtual void add_xml(xml_element *element, xml_document *document) = 0;
    boolean handle_xml_tag(Tag tag, xml_node *node);
    virtual wide_string xml_tag() = 0;
#endif
#if TT_DEBUG_ON
	 void debug();
#endif
	 Animation *rest() {
		 return(next);
	 };
	 void set_rest(Animation *new_next) {
		 next = new_next;
	 };
    Sprite *pointer_to_notify() {
       return(NULL);
    };
#if TT_DEBUG_ON
	 long return_animation_counter() {
		 return(animation_counter);
	 };
#endif
  protected:
	 Animation *next;
#if TT_DEBUG_ON
	 long animation_counter;
#endif
};

class Do_After: public Animation {
  public:
    Do_After() :
       Animation(),
		 owner(NULL),
       notify(NULL),
       callback(CALLBACK_NOTHING) {
    }; // new on 131102 for loading XML
	 Do_After(Sprite *owner, // new on 140104
		       millisecond end_time,
				 Sprite *notify=NULL,
				 AnimationCallBack callback=CALLBACK_NOTHING);
	 virtual ~Do_After();
	 boolean tick();
	 millisecond stop_time() {
		return(end_time);
	 };
    Sprite *pointer_to_notify() {
       return(notify);
    };
	 void set_owner(Sprite *sprite) {
		 owner = sprite; // new on 140104
	 };
#if TT_XML
    void add_xml(xml_element *element, xml_document *document);
    boolean handle_xml_tag(Tag tag, xml_node *node);
    wide_string xml_tag() {
       return(L"DoAfter");
    };
#endif
	private:
	 Sprite *owner; // new on 140104
	 millisecond end_time;
//	 flag *set_when_finished;
	 AnimationCallBack callback;
	 Sprite *notify;
};

class EaseInAndOut_To_Goal : public Animation {
  public:
    EaseInAndOut_To_Goal() :
       Animation(),
       notify(NULL),
       callback(CALLBACK_NOTHING),
       sprite(NULL) {
    };
	 EaseInAndOut_To_Goal(Sprite *sprite,
                         AnimatingVariable variable,
						       millisecond the_duration,
						       Sprite *notify=NULL, // was void prior to 131102
						       AnimationCallBack callback=CALLBACK_NOTHING);
	 virtual ~EaseInAndOut_To_Goal();
//	 virtual long current_goal() = 0;
	 millisecond stop_time() {
		return(end_time);
	 };
    Sprite *pointer_to_notify() {
       return(notify);
    };
 #if TT_XML
    void add_xml(xml_element *element, xml_document *document);
    boolean handle_xml_tag(Tag tag, xml_node *node);
#endif
	 void set_owner(Sprite *s) { // new on 140104
		 sprite = s;
	 };
  protected:
//	 long *variable;
//	 long offset;
    Sprite *sprite;
    AnimatingVariable variable;
	 millisecond end_time, duration;
//	 boolean *set_when_finished;
	 Sprite *notify;
	 AnimationCallBack callback;
};

class EaseInAndOut_To_Static_Goal : public EaseInAndOut_To_Goal {
  public:
     EaseInAndOut_To_Static_Goal() :
       EaseInAndOut_To_Goal() {
     }; // new on 131102 for loading XML
	 EaseInAndOut_To_Static_Goal(Sprite *sprite,
                                AnimatingVariable variable,
                                long the_goal, 
						              millisecond the_duration,
						              Sprite *notify=NULL,
						              AnimationCallBack callback=CALLBACK_NOTHING);
    boolean tick();
	 void shift_viewpoint(city_coordinate delta_x, city_coordinate delta_y);
#if TT_XML
    void add_xml(xml_element *element, xml_document *document);
    boolean handle_xml_tag(Tag tag, xml_node *node);
    wide_string xml_tag() {
       return(L"AnimateToStaticGoal");
    };
#endif
  protected:
    // following moved up here from EaseInAndOut_To_Static_Goal
    long difference;
	 long original_value;
};

class EaseInAndOut_To_Variable_Goal : public EaseInAndOut_To_Goal {
  public:
     EaseInAndOut_To_Variable_Goal() :
        EaseInAndOut_To_Goal(),
        other_sprite(NULL) {
    }; // new on 131102 for loading XML
	 EaseInAndOut_To_Variable_Goal(Sprite *sprite,
                                  AnimatingVariable variable,
                                  Sprite *other_sprite,
                                  AnimatingVariable other_variable, 
						                millisecond the_duration,
						                Sprite *notify=NULL,
						                AnimationCallBack callback=CALLBACK_NOTHING,
                                  long offset=0);
#if TT_ALPHA
	  ~EaseInAndOut_To_Variable_Goal(); // new on 021003
#endif
    boolean tick();
#if TT_XML
    void add_xml(xml_element *element, xml_document *document);
    boolean handle_xml_tag(Tag tag, xml_node *node);
    wide_string xml_tag() {
       return(L"AnimateToDynamicGoal");
    };
#endif
  protected:
    long current_goal();
    Sprite *other_sprite;
    AnimatingVariable other_variable;
    long offset;
};

boolean any_variables_changed();
void set_callback_only(boolean new_value);
void set_all_at_once(boolean new_value);

void set_animating_variable(Sprite *sprite, AnimatingVariable variable, long new_value, boolean finished=FALSE);
long animating_variable_value(Sprite *sprite, AnimatingVariable variable);

#if TT_XML
Animation *xml_load_animation(Tag tag);
#endif

/* obsoleted on 131102

class Set_After : public Animation {
  public:
	 Set_After(millisecond end_time, long *to_set, long future_value);
	 boolean tick();
	 millisecond stop_time() {
		return(end_time);
	 };
#if TT_XML
    void add_xml(xml_element *element, xml_document *document);
#endif
	private:
	 millisecond end_time;
	 long *to_set;
	 long future_value;
};

class Increment_After : public Animation {
  public:
	 Increment_After(millisecond end_time, long *to_set, long increment);
	 boolean tick();
	 millisecond stop_time() {
		return(end_time);
	 };
 #if TT_XML
    void add_xml(xml_element *element, xml_document *document);
#endif
	private:
	 millisecond end_time;
	 long *to_set;
	 long increment;
};
*/
#endif
