// Copyright (c) 1992-2003.  Ken Kahn, Animated Programs, All rights reserved.
#ifndef __TT_BOMB_H

#define __TT_BOMB_H

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif   
//#include "tools.h"

class Bomb : public Sprite {
	public:
	  Bomb(city_coordinate x=0, city_coordinate y=0,
			 long initial_priority=0,
//			 Background *floor=NULL,
			 city_coordinate width=0, city_coordinate height=0);
//     void prepare_for_deletion();
	  Sprite *copy(boolean also_copy_offsets=FALSE);
     Sprite *respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected,
                                 Sprite *by, boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless=FALSE);
	  Sprite *used(Sprite *, Sprite *by, boolean , boolean record_action,
                  millisecond , boolean modified, Sprite *original_subject);
	  void explode();
	  void explosion_done();
	  ReleaseStatus item_released_on_top(Sprite *item, Sprite *by, SpritePointer &, int );
	  void generate_help(output_stream &message, Sprite *in_hand, Sprite *selection);
	  boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
	  void java(java_stream &out);
	  boolean equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, SpritesPointer &expected_corresponding_nests, EqualReason reason=NORMAL_EQUAL) {
		  return(other->kind_of() == BOMB);
	  };
#if TT_XML
    wide_string xml_tag() {
       return(L"Bomb");
    };
#endif
	private: 
//	  ActionStatus *action_status; // obsoleted by following
//	  Sprite *saved_by;
};

boolean bomb_ever_used();
boolean describe_bomb(InputStream *pad_in, output_stream &text_out, int version_number, Article article);

#endif
