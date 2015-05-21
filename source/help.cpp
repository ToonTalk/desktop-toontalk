// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_STRINGS_H)
#include "strings.h"
#endif
#if !defined(__TT_DEFS_H)
#include "defs.h"
#endif
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif   
#if !defined(__TT_SPRITE_H)   
#include "sprite.h"
#endif   
#if !defined(__TT_CUBBY_H)   
#include "cubby.h"
#endif   
#if !defined(__TT_NUMBER_H)   
#include "number.h"
#endif
#if !defined(__TT_BIRD_H)
#include "bird.h"
#endif	
#if !defined(__TT_SCREEN_H)   
#include "screen.h"
#endif   
#if !defined(__TT_PICTURE_H)   
#include "picture.h"
#endif
#if !defined(__TT_TEXT_H)   
#include "text.h"
#endif
#if !defined(__TT_SOUND_H)
#include "sound.h"
#endif
#if !defined(__TT_ROBOT_H)   
#include "robot.h"
#endif   
#if !defined(__TT_MARTIAN_H)   
#include "martian.h"
#endif   
#if !defined(__TT_UTILS_H)
#include "utils.h"
#endif
#if !defined(__TT_WINMAIN_H)
#include "winmain.h"
#endif
#if !defined(__TT_BOMB_H)
#include "bomb.h"
#endif
#if !defined(__TT_TRUCK_H)
#include "truck.h"
#endif
#if !defined(__TT_HELP_H)
#include "help.h"
#endif

#include <stdlib.h> // for itoa
//#include <fstream.h>
#include <string.h>
//#include <strstrea.h> commented out 210703

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
	
void describe_appearance(SpriteCode code, output_stream &text_out, Article ) {
	// was -1 but then IDS_TOOLBOX_FROM_SIDE becomes 0
//	print_article(article,text_out,S(IDS_TOOLBOX_FROM_SIDE+code));
   text_out << S(IDS_TOOLBOX_FROM_SIDE+code);
};

//void read_page(int page, InputStream *pad_in, output_stream &text_out, int version_number) {
//	text_out << S(IDS_PAGE_BEGIN_SENTENCE) << page << S(IDS_CONTAINS);
//	describe_encoding((SpriteType) pad_in->get(),pad_in,text_out,version_number);
//	text_out << "  "; // to follow end of sentence
//	text_out << endl;
//};

void print_type(SpriteType type, output_stream &text_out, Article article, boolean capitalize_first) {
	if (type > LAST_SPRITE_TYPE || type < 0) {
		text_out << S(IDS_INVALID_TYPE);
		return;
	};
   unsigned short int index;
	switch (type) {
		case BLANK_ROBOT: // added very late
			index = IDS_BLANK_ROBOT_TYPE_INDEFINITE;
			break;
		case FORCE_FEEDBACK:
			index = IDS_FORCE_EFFECT_TYPE_INDEFINITE;
			break;
		case MARTIAN: // added 310500
			index = IDS_MARTIAN;
			break;
		case THOUGHT_BUBBLE: // added 310500
			index = IDS_THOUGHT_BUBBLE;
			break;
			// what about blank force effects?
		default:
			index = (unsigned short int) (IDS_NUMBER_TYPE_INDEFINITE+(type*10));
	};
   if (article == NO_ARTICLE) { 
      if (english()) {
         const_string indefinite = S_maybe_capitalize_first(index,capitalize_first);
			if (indefinite == NULL) {
				text_out << type_string(type,capitalize_first);
				return;
			};
         string no_article = _tcschr(indefinite,' ');
         if (no_article != NULL) {
            text_out << (no_article+1); // skip leading blank
         } else { // else something wrong
            text_out << indefinite;
         };
         return;
#if TT_DEBUG_ON         
       } else {
         say_error(_T("Non English version called with NO_ARTICLE."));
#endif
       }; 
   };
   print_with_article(article,index,type,text_out,capitalize_first);
}; 

const_string indefinite_type(SpriteType type) {
	if (type > LAST_SPRITE_TYPE || type < 0) {
		return(S(IDS_INVALID_TYPE));
	};
   unsigned short int index = (unsigned short int) (IDS_NUMBER_TYPE_INDEFINITE+(type*10));
	if (type == BLANK_ROBOT) { // added very late
		index = IDS_BLANK_ROBOT_TYPE_INDEFINITE;
	};
	return(S(index,""));
};

void print_with_article(Article article, unsigned short int index, SpriteType type, 
								output_stream &text_out, boolean capitalize_first, string default_string) {
	// null_ok arg added on 020899
#if TT_DEBUG_ON
	switch (type) { // should only called by debugging code 
		case TALK_BALLOON:
			text_out << "***talk balloon***";
			return;
		case PROGRAMMER:
			text_out << "***programmer***";
			return;
		case PROGRAMMER_ARM:
			text_out << "***programmer arm***";
			return;
	};
#endif
	const_string out;
   switch (article) {
      case PLURAL_ARTICLE:
         out = S_maybe_capitalize_first(index+PLURAL_OFFSET,capitalize_first,default_string);
         break;
      case DEFINITE_ARTICLE:
         out = S_maybe_capitalize_first(index+DEFINITE_OFFSET,capitalize_first,default_string);
         break;
      case INDEFINITE_ARTICLE:
         out = S_maybe_capitalize_first(index,capitalize_first,default_string); // indefinite's offset is 0
         break;
      case PRONOUN:
         out = S_maybe_capitalize_first(index+PRONOUN_OFFSET,capitalize_first,default_string); 
         break;
      default: // shouldn't happen 
         out = S_maybe_capitalize_first(index,capitalize_first,default_string);
   };
	if (out == NULL) {
		text_out << type_string(type,capitalize_first);
	} else {
		text_out << out;
	};
};

void print_it(SpriteType type, output_stream &text_out) {
   print_type(type,text_out,PRONOUN);
};

/*
   // if character is him
   // in Swedish ett eller en
   switch (type) {
      case EXPANDER:
      case VACUUM:
      case ROBOT:
      case TOOLBOX:
         text_out << S(IDS_HIM);
         break;
      case BIRD:
         text_out << S(IDS_HER);
         break;
      default:
         switch (tt_language) {
            case AMERICAN:
            case BRITISH:
               text_out << "it"; // S(IDS_IT);
               break;
            case SWEDISH:
               {
                string definite_form = S(IDS_NUMBER_TYPE_DEFINITE+type);
                int length = strlen(definite_form);
                if (definite_form[length-1] == 't') {
                   text_out << "ett";
                } else if (definite_form[length-1] == 'n') {
                   text_out << "en";
                } else { // something wrong
                   text_out << "en";
                };
               };
               break;
          };
          break;
    };
*/

void print_article(Article article, output_stream &text_out, const_string next) {
   // only called by english() version
	if (next != NULL &&
		 _tcslen(next) > 3 &&
		 (_tcsncmp(next,_T("the"),3) == 0 || _tcsncmp(next,_T("any"),3) == 0)) {
		text_out << next;
		return;
	};
	if (next != NULL && IsCharUpper(next[0])) { // proper noun
		if (next != NULL) text_out << next;
		return; 
	};
	switch (article) {
		case DEFINITE_ARTICLE:
			text_out << "the ";
		case NO_ARTICLE: 
			if (next != NULL) text_out << next;
			return;  
		case PLURAL_ARTICLE: { // only called by english() version
			// sometimes next is things like "movie of" and becomes "movies of"
			int length = _tcslen(next);
         int i;
			for (i = 0; i < length; i++) {
				if (i+2 < length && starts_with_preposition(next+i+1,length-(i+1))) {
					// was next[i] == ' ') {
					if (i > 2 && _tcsncmp(next,_T("box"),3) == 0) { // hack to get boxes
						text_out.put('e');
					};
					text_out.put('s');
					break;
				};
				text_out.put(next[i]);
			};
			if (i == length) {   // was no " of"
				if (i > 2 && _tcsncmp(next,_T("box"),3) == 0) {
					text_out.put('e'); 
				};
				text_out.put('s');
			} else {
				text_out.write(next+i,length-i);
			};
			return; 
		  };	
		case INDEFINITE_ARTICLE:		
			if (next != NULL &&
				 (next[0] == 'a' || next[0] == 'e' || next[0] == 'i' ||
				  next[0] == 'o' || next[0] == 'u')) {
				text_out << "an ";
				if (next != NULL) text_out << next;
			} else {
				text_out << "a ";
				if (next != NULL) text_out << next;
			};
			return;
	};
};

boolean starts_with_preposition(const_string text, int length) {
	return((length > 2 && _tcsncmp(text,_T("of"),2) == 0) ||
			 (length > 4 && _tcsncmp(text,_T("with"),4) == 0));
};

void describe_hand(output_stream &message) {
  if (tt_screen->inside_rule_body()) {
	  message << S(IDS_THE_ROBOTS_HAND);
  } else {
	  message << S(IDS_YOUR_HAND);
  };
};

void you_are(output_stream &message, boolean capitalize) {
  if (tt_screen->inside_rule_body()) {
	  message << S_maybe_capitalize_first(IDS_THE_ROBOT_IS,capitalize);
  } else {
	  message << S_maybe_capitalize_first(IDS_YOU_ARE,capitalize);
  };
};

void you_are2(output_stream &message) {
  if (tt_screen->inside_rule_body()) {
		message << S(IDS_THE_ROBOT_IS2,""); // "" added on 030999
  } else {
		message << S(IDS_YOU_ARE2,"");
  };
};

boolean ignore_next_item(InputStream *stream, int version_number, boolean in_thought_bubble, 
								 SpriteType container_type, boolean is_controlee, boolean on_nest) {
	// re-written on 120101
	return(ignore_next_item((SpriteType) stream->get(),stream,version_number,in_thought_bubble,container_type,is_controlee,on_nest));
};
/*	
#if TT_NEW_IO
	output_string_stream ignore;
#else
	char *buffer = new char[max_buffer_size];
   output_string_stream ignore(buffer,max_buffer_size);
#endif
   boolean result = describe_item(stream,ignore,version_number,in_thought_bubble,
											 INDEFINITE_ARTICLE,container_type,is_controlee,on_nest);
#if !TT_NEW_IO
   delete [] buffer;
#endif
	return(result);
*/

boolean ignore_next_item(SpriteType type, InputStream *stream, int version_number, 
								 boolean in_thought_bubble, SpriteType container_type, boolean is_controlee, boolean on_nest) {
	// this currently can't load in item and destroy it unless shared items are turned off
#if TT_NEW_IO
	output_string_stream ignore;
#else
//	unsigned char *buffer = new unsigned char[max_buffer_size];
	unsigned char buffer[max_buffer_size]; // made static on 120101
   output_string_stream ignore((char *) buffer,max_buffer_size);
	UnwindProtect<output_stream*> set(tt_current_output_stream,&ignore); // this and following is new on 120101 in case gets too long
	UnwindProtect<int> set2(tt_current_output_stream_size,max_buffer_size);
#endif
   boolean result = describe_item(type,stream,ignore,version_number,
											 in_thought_bubble,INDEFINITE_ARTICLE,container_type,is_controlee,on_nest);
#if !TT_NEW_IO
//   delete [] buffer;
#endif
	return(result);
};

boolean describe_item(InputStream *pad_in, output_stream &text_out, int version_number,
							 boolean in_thought_bubble, Article article, SpriteType container_type,
							 boolean is_controlee, boolean on_nest) {
// commented out the following since tt_dumping_to_test_equality_or_describe means geometry wasn't dumped, etc.
//	boolean saved_tt_dumping_to_test_equality_or_describe = tt_dumping_to_test_equality_or_describe;
//	tt_dumping_to_test_equality_or_describe = TRUE; // added 020299 to avoid superflorous warnings
   boolean result = describe_encoding((SpriteType) pad_in->get(),pad_in,text_out,version_number,
												  in_thought_bubble,article,container_type,is_controlee,on_nest);
//   tt_dumping_to_test_equality_or_describe = saved_tt_dumping_to_test_equality_or_describe;
	return(result);
};

boolean describe_item(SpriteType type, InputStream *pad_in, output_stream &text_out, int version_number,
							 boolean in_thought_bubble, Article article, SpriteType container_type,
							 boolean is_controlee, boolean on_nest) {
	if (pad_in->empty()) return(FALSE); // added on 190201 for robustness
// commented out the following since tt_dumping_to_test_equality_or_describe means geometry wasn't dumped, etc.
//	boolean saved_tt_dumping_to_test_equality_or_describe = tt_dumping_to_test_equality_or_describe;
//	tt_dumping_to_test_equality_or_describe = TRUE; // added 020299 to avoid superflorous warnings
   boolean result = describe_encoding(type,pad_in,text_out,version_number,
												  in_thought_bubble,article,container_type,is_controlee,on_nest);
//   tt_dumping_to_test_equality_or_describe = saved_tt_dumping_to_test_equality_or_describe;
	return(result);
};

boolean describe_encoding_internal(SpriteType type, InputStream *pad_in, output_stream &text_out, int version_number,
								           boolean in_thought_bubble, Article article,
                                   SpriteType container_type, boolean is_controlee, boolean on_nest) {
//	long value;
//	unsigned char size;
//	int image_index;
	switch (type) {
		case CUBBY: 
		case BLANK_CUBBY:
		case LABELED_CUBBY:
			return(describe_cubby(type,pad_in,text_out,version_number,in_thought_bubble,article));
		case NEST:
			return(describe_nest(pad_in,text_out,version_number,in_thought_bubble,article));
		case BIRD:
      case PRIMITIVE_BIRD:
			return(describe_bird(type,pad_in,text_out,version_number,in_thought_bubble,article));
		case INTEGER:
		case RATIONAL_NUMBER:
//			pad_in->read((unsigned char *) &value, sizeof(value));
//			text_out << "the number " << value;
//			break;
//		case INTEGER:
		case BLANK_INTEGER:
		case ERROR_INTEGER:
		case REMOTE_INTEGER:
		case GLOBAL_REMOTE_INTEGER:
		case REMOTE_PICTURE:
      case REMOTE_TEXT_PICTURE:
      case GLOBAL_HYPERBOT_REMOTE_INTEGER:
      case GLOBAL_USER_REMOTE:
      case FORCE_REMOTE_INTEGER:
			return(describe_number(type,pad_in,text_out,version_number,in_thought_bubble,article));
		case TEXT:
		case BLANK_TEXT:
		case VARIABLE_WIDTH_TEXT:
         return(describe_text(type,pad_in,text_out,version_number,in_thought_bubble,article));
      case SOUND:
      case BLANK_SOUND:
      case BUILT_IN_SOUND:
      case USER_SOUND:
      case SPEECH_SOUND:
      case MCI_SOUND:
      case BLANK_BUILT_IN_SOUND:
      case BLANK_USER_SOUND:
      case BLANK_SPEECH_SOUND:
      case BLANK_MCI_SOUND:
		case FORCE_FEEDBACK: // added on 160799
		case BLANK_FORCE_FEEDBACK: // added on 020200
         return(describe_sound(type,pad_in,text_out,version_number,in_thought_bubble,article));
		case PROGRAM_PAD: 
			return(describe_pad(pad_in,text_out,version_number,article));
		case PICTURE:
//			pad_in->read((unsigned char *) &image_index, sizeof(image_index));
//			text_out << "the picture numbered " << image_index;
//			break;
		case BLANK_PICTURE:
//			pad_in->read((unsigned char *) &image_index, sizeof(image_index));
//			describe_type(type,text_out);
		case PICTURE_WITH_INSIDES:
		case PICTURE_WITH_INDIRECTION:
		case REMOTE_APPEARANCE:
      case USER_PICTURE:
		case BLANK_USER_PICTURE: // added 021199
			return(describe_picture(type,pad_in,text_out,version_number,in_thought_bubble,article,container_type,is_controlee,on_nest));
		case EMPTY_PAGE_MARKER: // same as NOTHING_MARKER
			text_out << S(IDS_NOTHING);
			break;
		case UNDEFINED_ROBOT_MARKER:
//			print_article(article,text_out,S(IDS_ROBOT_WITHOUT_A_THOUGHT));
			if (version_number >= first_version_to_save_copied_nests) {
				describe_comment_encoding(pad_in,text_out,version_number,ROBOT);
				//character ignore[256];
				//load_string(pad_in,ignore,255); // ignore name
				// rewritten on 030105 since the above caused errors
				ignore_next_item(pad_in,version_number,tt_language); // name
			};
         text_out << S(IDS_ROBOT_WITHOUT_A_THOUGHT);
			break;
		case ROBOT:
		case ROBOT_WITH_TOOL:
         {
         boolean result = describe_robot_line(pad_in,text_out,version_number,article,in_thought_bubble);
         if (type == ROBOT_WITH_TOOL) {
            text_out << space() << S(IDS_WHO_IS_HOLDING) << space();
            result = (result && describe_item(pad_in,text_out,version_number));
				text_out << S(IDS_WHO_IS_HOLDING2,"");
         };
         return(result);
         };
		case BLANK_ROBOT: // added on 090799 since otherwise breaks trying to describe them
		case BLANK_ROBOT_WITH_TOOL:
			{
			text_out << AS(IDS_ANY_ROBOT);
			char *buffer = new char[max_buffer_size];
			output_string_stream ignore(buffer,max_buffer_size);
			describe_robot_line(pad_in,ignore,version_number,article,in_thought_bubble);
			delete [] buffer;
			};
			break;
      case COPIER:
          return(describe_copier(pad_in,text_out,version_number,article));
      case VACUUM:
          return(describe_vacuum(pad_in,text_out,version_number,article));
      case EXPANDER:
          return(describe_bike_pump(pad_in,text_out,version_number,article));
		case BOMB:
			return(describe_bomb(pad_in,text_out,version_number,article));
      case NEW_SHARED_ITEM:
         {
         short int index;
         pad_in->read((string) &index, sizeof(index));
//			text_out << "[#" << index << " = " // new on 081200
         boolean result = describe_item(pad_in,text_out,version_number,in_thought_bubble,article,container_type,is_controlee);
//			text_out << "]"; 
			return result;
         };
      case OLD_SHARED_ITEM:
		case EQUAL_TO_OLD_ITEM: // new on 060601
		case EQUAL_TO_OLD_ITEM_AND_GEOMETRY: // new on 120601
         {
         short int index;
         pad_in->read((string) &index, sizeof(index));
			text_out << S(IDS_SOMETHING);
			if (type == EQUAL_TO_OLD_ITEM_AND_GEOMETRY) {
            if (tt_temp_picture == NULL) {
               tt_temp_picture = new GlobalPicture();
            };
				tt_temp_picture->load_geometry(pad_in,version_number); 
				// was tt_global_picture prior to 011002 but that changed whether it was blank which is used in matching some sensors
			};
//			text_out << "#" << index; // new on 081200
//			return(FALSE); // new on 081200 to signal to say nothing (but not to abort description process)
         };
         break;
		case TRUCK: // new on 130200
			return(describe_truck(pad_in,text_out,version_number,article));
      case PROGRAMMER:
      case PROGRAMMER_ARM:
      case MOUSE_WITH_HAMMER:
      case TALK_BALLOON:
      case MARTIAN:
         break; // nothing to say or more to read
		default:
         if (type >= GLOBAL_JOYSTICK0_REMOTE_INTEGER && type <= GLOBAL_JOYSTICK31_REMOTE_INTEGER) {
            return(describe_number(type,pad_in,text_out,version_number,in_thought_bubble,article));
         } else {
				if (version_number >= first_version_to_save_copied_nests) {
					// added on 290699
					if (type != TOOLBOX) { // condition added on 110700 since toolbox isn't dumped with comments - should it??
						describe_comment_encoding(pad_in,text_out,version_number,type);
					};
				};
			   print_type(type,text_out,article);
				return(type <= LAST_SPRITE_TYPE); 
         };
	};
	return(TRUE);
};

boolean describe_encoding(SpriteType type, InputStream *pad_in, output_stream &text_out, int version_number,
								  boolean in_thought_bubble, Article article,
                          SpriteType container_type, boolean is_controlee, boolean on_nest) {
	// new on 140301 to catch errors here
   if (type == 0 && pad_in == NULL) {
		// new on 030105 to check if ran out of stuff to describe
		tt_error_file() << "Reached unexpected end of input encoding while generating a description (e.g. for Marty to speak)" << endl;
		return(FALSE);
	};
	// commented out the following on 030105 since something has to catch the throws
   //if (tt_want_exception_breaks) {
	  // return(describe_encoding_internal(type,pad_in,text_out,version_number,in_thought_bubble,article,container_type,is_controlee,on_nest));
   //} else {
	   try {
		   return(describe_encoding_internal(type,pad_in,text_out,version_number,in_thought_bubble,article,container_type,is_controlee,on_nest));
	   } catch (...) {
		   text_out << " (Warning: internal error in ToonTalk generating this description. Sorry.) " << endl;
		   tt_error_file() << "Error describing something whose type code is " << (int) type << endl;
		   throw(tt_global_picture); // was return(NULL) prior to 030105
	   };
 //  };
};

boolean describe_comment_encoding(InputStream *pad_in, output_stream &text_out, int version_number, SpriteType owners_type) {
//	Sprite *comment = load_item(pad_in,version_number);
//	if (comment != NULL) {
//		comment->describe_comment(text_out);
//		comment->destroy();
//	};
//	return(TRUE);
	SpriteType type = (SpriteType) pad_in->get();
	if (type == NOTHING_MARKER || pad_in->empty()) return(TRUE);
	boolean ended_with_period = FALSE;
	switch (type) {
		case TEXT:
		case VARIABLE_WIDTH_TEXT:
			if (version_number >= first_version_to_save_copied_nests) {
				ignore_next_item(pad_in,version_number); // comment
			};
			{
			unsigned short int text_length;
			pad_in->read((string) &text_length, sizeof(text_length));
			if (text_length > 0) {
				string text = new character[text_length+1];
				pad_in->read((string) text,text_length);
				replace_carriage_returns_with_spaces(text,text_length); // new on 261100 - looks nicer and is critical in Java comments
				text_out.write(text,text_length);
				ended_with_period 
					= (text[text_length-1] == '.' || text[text_length-1] == '!' || text[text_length-1] == '?' || text[text_length-1] == ',');
				delete [] text;
			};
			pad_in->get(); // ignore color added on 150599 so described properly
			pad_in->get(); // ignore visibility flags 
			};
			break;
      case SOUND:
      case BLANK_SOUND:
      case BUILT_IN_SOUND:
      case USER_SOUND:
      case SPEECH_SOUND:
      case MCI_SOUND:
      case BLANK_BUILT_IN_SOUND:
      case BLANK_USER_SOUND:
      case BLANK_SPEECH_SOUND:
      case BLANK_MCI_SOUND:
//#if TT_DIRECT_INPUT
      case BLANK_FORCE_FEEDBACK:
      case FORCE_FEEDBACK:
//#endif
         { // new on 290699 so that comments that are based on audio are just heard
				Sound *sound = load_sound(type,pad_in,version_number,tt_language,-1,NULL,FALSE);
				if (sound != NULL) {
					if (sound->make_sound()) { // made a sound
						// following commented out since too hard to get Marty to just say this and nothing else
//						if (type == SPEECH_SOUND) { // And make Marty show text pad contents
//							string text;
//							unsigned short text_length;
//							sound->current_text(text,text_length);
//							text_out << text;
//							if (limited_text_to_speech()) tt_martian->just_talk_balloons_for(30000); // 30 seconds should be enough
//						};
						sound->destroy();
						return(TRUE);
					} else {
						sound->destroy();
					};
				};
				break;
			};
		default: {
			print_type(owners_type,text_out,INDEFINITE_ARTICLE);
			text_out << space() << S(IDS_THAT_REMINDS_ME_OF) << space();
			UnwindProtect<boolean> set(tt_describing_a_comment,TRUE); // new on 210205
			describe_item(type,pad_in,text_out,version_number);
			text_out << S(IDS_THAT_REMINDS_ME_OF2,"");
					};
			break;
	};
	if (ended_with_period) {
		text_out << space();
	} else {
		text_out << end_sentence();
	};
	print_type(owners_type,text_out,DEFINITE_ARTICLE,TRUE); // Capitalize first
	text_out << S(IDS_IS); 
	return(TRUE);
};

void capitalize_first_letter(const_string text, output_stream &text_out) {
   if (text == NULL) return;
   text_out << upper_case_character(text[0]) << (text+1);
};

void print_ordinal_definite_phrase(int count, output_stream &text_out,
                                   boolean capitalize_first,
                                   const_string first, const_string second) {
   if (first != NULL) {
      if (capitalize_first) {
         capitalize_first_letter(first,text_out);
      } else {
         text_out << first;
      };
      text_out << space(); 
   };
   print_ordinal(count,text_out);
   text_out << space() << second;
};

void print_ordinal(int i, output_stream &text_out) { // , boolean capitalize_first
   const_string ordinal;
	switch(i) {
		case 1:
		  ordinal = S(IDS_FIRST);
		  break;
		case 2:
		  ordinal = S(IDS_SECOND);
		  break;
		case 3:
		  ordinal = S(IDS_THIRD);
		  break;
		case 4:
		  ordinal = S(IDS_FOURTH);
		  break;
		case 5:
		  ordinal = S(IDS_FIFTH);
		  break;
		case 6:
		  ordinal = S(IDS_SIXTH);
		  break;
      case 7:
		  ordinal = S(IDS_SEVENTH);
		  break;
      case 8:
		  ordinal = S(IDS_EIGHTH);
		  break;
      case 9:
		  ordinal = S(IDS_NINTH);
		  break;
      case 10:
		  ordinal = S(IDS_TENTH);
		  break;
      case 11:
        ordinal = S(IDS_ELEVENTH);
        break;
      case 12:
        ordinal = S(IDS_TWELFTH);
        break;
      case 13:
         ordinal = S(IDS_THIRTEENTH);
         break;
		default:
		   text_out << i << S(IDS_CARDINAL_MOD_IS_0+i%10);
         return;
   };
//   if (capitalize_first) {
//      ordinal[0] = (char) CharUpper((LPTSTR) ordinal[0]);
//   };
   text_out << ordinal;
};

void describe_pointing(output_stream &message, Sprite *tool_in_hand, Sprite *selection) {
//	if (tt_martian->pointer_to_previous_selection() == selection &&
//	    selection != tool_in_hand) return;
	long next_id = 0;
	if (tool_in_hand == NULL) {
		message << S(IDS_YOU_ARE_POINTING_TO);
		next_id = IDS_YOU_ARE_POINTING_TO2;
	} else if (tool_in_hand == selection || selection == NULL || 
              !tool_in_hand->is_tool()) {
		message << S(IDS_YOU_ARE_HOLDING);
      tool_in_hand->describe(message,INDEFINITE_ARTICLE);
		message << S(IDS_YOU_ARE_HOLDING2,"");
      message << end_sentence();
      return;
	} else {
		switch (tool_in_hand->kind_of()) {
			case VACUUM:
			case COPIER:
			case EXPANDER:
				message << S(IDS_UNDER);
				tool_in_hand->describe_type(message,DEFINITE_ARTICLE); // was just describe
				message << S(IDS_IS);
				next_id = IDS_UNDER_TOOL_IS2;
		};
	};
	if (selection != NULL) {
		selection->describe(message,INDEFINITE_ARTICLE);
		if (next_id != 0) {
			message << S(next_id,"");
		};
		message << end_sentence();
	}; 
};

void enter_thought_bubble_help() {
	if (!did_user(USER_HAS_LEFT_THOUGHT_BUBBLE)) { // first time
		say(IDS_IN_THOUGHT_BUBBLE_HELP1,3);
		say(IDS_IN_THOUGHT_BUBBLE_HELP2,3);
		say(IDS_LEAVE_TB_HELP,3);
	};
};

void left_thought_bubble_help() {
	if (!did_user(USER_HAS_LEFT_THOUGHT_BUBBLE)) { // first time
		say(IDS_LEFT_THOUGHT_BUBBLE_HELP,3);
	};
};

void user_did(int history_index, int action, Sprite *by) {
   if (tt_histories == NULL) return; // too early -- maybe called by trouble shooting
//	if (!martian_active()) return;  // doesn't count if Martee doesn't see it
	if (by != NULL && by->still_running()) return; // user didn't
   // following caused Marty to jump topics too quickly
//   if (tt_martian != NULL && by != NULL &&
//       history_index != GLOBAL_HISTORY && history_index != USER_TOLD) {
//      tt_martian->dont_wait_to_give_help();
//  };
	if (!did_user(action,history_index)) {  //(tt_histories[history_index]&mask)) {
		tt_histories[history_index] |= 1UL<<action;
		tt_frame_of_last_history = tt_frame_number;
		if (history_index != TROUBLE_SHOOT_HISTORY) { // conditions new on 100100
			dump_history();
		};
	};
	tt_last_history_index = history_index;
	tt_last_history_action = action;
};

boolean did_user(int action, int history_index) {
	return(tt_histories != NULL && (tt_histories[history_index]&(1UL<<action)) != 0);
};

void user_told(int action) {
   user_did(USER_TOLD,action,NULL);
};

boolean told_user(int action) {
	return(did_user(action,USER_TOLD));
};

boolean type_history_full(int type) {
	// mask out the bit which is whether type help was given
   if (type < 0 || type >= history_count || tt_histories == NULL) return(TRUE); // full so won't be explored further...
   // added tt_histories == NULL on 211102 due to crash report but not clear how this can happen
	History history = tt_histories[type]; // &(~(1UL<<TYPE_HELP_GIVEN));
	int last_action = -1; 
	switch (type) {
		case INTEGER:
			last_action = 10;
			break;
		case EXPANDER:
			last_action = 3;
			break;
		case VACUUM: 
			last_action = 5;
			break;
		case COPIER: 
			last_action = 5;
			break;
		case THOUGHT_BUBBLE: 
			return(history == 0); 
		case ROBOT: 
			last_action = 4;			
			break;
		case NEST: 
			last_action = 3;
			break;
		case BIRD:
			last_action = 0; // just one
			break;
		case TRUCK:
			last_action = 2;
			break;
		case PROGRAM_PAD:
			last_action = 8;
			break;
		case CUBBY: 
			last_action = 10;
			break;
		case TOOLBOX:
			return(history == 0); 
		case PICTURE:
			// ignore stuff about scale etc
			last_action = 11;
			break;
		case BOMB:
			last_action = 1;
			break;
		case TEXT:
			last_action = 9;     // changed 9/24/97
			break;
      case REMOTE_TEXT_PICTURE:
		case REMOTE_PICTURE:	{
			unsigned int mask = (1UL<<NO_IDENTIFIER_GIVEN)-1; 
			return((history&mask) == mask);
         };
      case TOOL_BUTTON_HISTORY:
         last_action = 2;
         break;
	};
	if (last_action != -1) {
		unsigned int mask = (1UL<<(last_action+1))-1;
		return((history&mask) == mask); // all those bits are set
	} else {
		return(FALSE);
	};
};

void describe_space_bar_alternatives(output_stream &message) {
   if (tt_mouse_button_count > 1) {
      message << S(IDS_OR_SENTENCE_START) << space() << S(IDS_PICK_UP_HELP2,"") << space() // "Or by clicking "
              << S(IDS_THE_RIGHT_MOUSE_BUTTON);
      if (tt_joystick_count > 0) {
         message << space() << S(IDS_OR) << space() << S(IDS_SECOND_JOYSTICK_BUTTON);
      };
      message << S(IDS_PICK_UP_HELP3,"") << end_sentence();
   } else  if (tt_joystick_count > 0) {
      message << S(IDS_OR_SENTENCE_START) << space() << S(IDS_PICK_UP_HELP2,"") << space() 
			     << S(IDS_SECOND_JOYSTICK_BUTTON) << S(IDS_PICK_UP_HELP3,"") << end_sentence();
   };
};

void describe_right_button(output_stream &message, char alternative) {
  if (tt_mouse_button_count == 1) {
//    if (tt_new_input_scheme) {
         if (alternative == ' ') {
            message << S(IDS_THE_SPACE_BAR);
         } else if (alternative != 0) {
            message << "'" << alternative << "'";
				if (alternative == '-' && text_to_speech()) {
					message << space() << "(" << S(IDS_MINUS,"minus") << ")"; // new on 091004 for speaking about the - key
				};
			} else {
				// explain right mouse button equivalent -- new on 091004
				// replaced IDS_THE_LEFT_MOUSE_BUTTON with IDS_LEFT_BUTTON_WITH_ONE_BUTTON on 131004
				message << S(IDS_LEFT_BUTTON_WITH_ONE_BUTTON) << space() 
						  << S(IDS_WHILE_HOLDING_DOWN_THE_SHIFT_BUTTON,"while holding down the shift button");
         };
         if (tt_joystick_count > 0) {
            message << space() << S(IDS_OR) << space() << S(IDS_SECOND_JOYSTICK_BUTTON);
         };
//      } else {
//		   message << S(IDS_RIGHT_MOUSE_BUTTON_WITH_ONE_BUTTON);
//      };
	} else {
		message << S(IDS_THE_RIGHT_MOUSE_BUTTON);
      if (tt_joystick_count > 0) {
         message << space() << S(IDS_OR) << space() << S(IDS_SECOND_JOYSTICK_BUTTON);
      };
	};
};

void describe_left_button(output_stream &message) {
	if (tt_mouse_button_count == 1) {
		message << S(IDS_LEFT_BUTTON_WITH_ONE_BUTTON);
	} else {
		message << S(IDS_THE_LEFT_MOUSE_BUTTON);
	};
};

void describe_middle_button(output_stream &message) {
	if (tt_mouse_button_count == 3) {
		message << S(IDS_MIDDLE_BUTTON_WITH_THREE);
	} else if (tt_mouse_button_count == 2) {
		message << S(IDS_MIDDLE_BOTTON_WITH_TWO);
	} else {
		message << S(IDS_MIDDLE_BUTTON_WITH_ONE);
	};
};

void no_help_interruptions() {
	tt_martian->no_interruptions();
};

boolean stop_generating_help(int size_of_next_output) { // prior to 251104 size_of_next_output was hardwired as 512
	if (tt_current_output_stream == NULL) return(TRUE);
	int count = tt_current_output_stream->tellp(); // no longer using pcount
	// restored and updated the following on 171000 (new is that pcount can be zero if full)
   return(count > tt_current_output_stream_size-size_of_next_output); // leave some room for more output before next check
};
//#if !TT_MICROSOFT
		// VC++ 6.0 doesn't work right with pcount() of string streams - so removed on 081299
//		|| tt_current_output_stream->LENGTH > tt_current_output_stream-512
//#endif
//			 ); 
//	return(tt_martian->stop_generating_help());


void report_action_aborted(int reason, Sprite *by, SpriteType actor, int importance, string default_string) {
	tt_martian->action_aborted(reason,by,actor,importance,default_string);
};

// obsolete??
//boolean martians_previous_selection(Sprite *other) {
//	return(tt_martian->pointer_to_previous_selection() == other);
//};

#if TT_DEBUG_ON
const long int max_log_count = 1000000;
#else
const long int max_log_count = 100;  // don't fill up users disk with long log files
#endif

long int log_count = 0;

void log(const_string message, boolean message_box_too, boolean important) {
   if (log_count > max_log_count) return;
   log_count++;
	tt_error_file() << message << " (frame=" << tt_frame_number << ")";
	int last_error = GetLastError();
	if (interesting_error(last_error)) { 
	  // and not file not found and not overlapped I/O in progress and not can't create existing file and not timeout
		tt_error_file() << "(error number=" << last_error << ")";
	};
   tt_error_file().flush(); // to be sure it is there if system crashes
   if (message_box_too) {
      message_box(MB_ICONINFORMATION|MB_OK,message);
   };
	if (important) {
		tt_err_file_important = TRUE;
#if TT_DEBUG_ON
      debug_this("  And it is important!");
#endif
	};
   tt_error_file() << endl;
};

#if TT_DEBUG_ON
void debug_this(string extra) {
  	tt_error_file() << "Debugging event on frame " << tt_frame_number;
	if (extra != NULL) {
		tt_error_file() << ". " << extra;
	};
	tt_error_file() << endl;
};

boolean debug_log(int debug_counter, string message) {
	if (debug_counter == tt_debug_target) {
		if (message != NULL) {
			if (strcmp(message,"Debug target becoming a follower.") == 0) {
            log(message); // to set breakpoint
			} else if (strcmp(message,"Debug target removed as a follower.") == 0) {
				log(message);
			} else {
				log(message);
			};
		};
		return(TRUE);
	} else {
		return(FALSE);
	};
};

xml_element *debug_element = NULL;

void debug_ref_count(int ref_count, xml_node *element) {
	if (element == debug_element) {
		tt_error_file() << "Debug element's ref count is " << ref_count << endl;
	};
};

#endif

boolean martian_saying = FALSE;

void martian_says(const_string message, int importance, const_string spoken_alternative_message) {
	if (tt_martian != NULL && 
		 (importance >= tt_minimum_say_importance || tt_martian->called_by_user())) { 
		if (!martian_saying) {
			// not re-entrant (e.g. trouble talking shouldn't try to have Martian describe it)
			martian_saying = TRUE;
			tt_martian->say(message,importance,spoken_alternative_message);
			tt_martian->no_interruptions();
			martian_saying = FALSE;
		} else {
			log(message);
		};
	};
};

void martian_says(int message_id, int importance) {
	string spoken_alternative_message = NULL;
	if (alternative_language_spoken()) {
		spoken_alternative_message = S(message_id,"",TRUE);
	};
	martian_says(S(message_id),importance,spoken_alternative_message);
};

char pre_initialization_warning_max = 3; // was 4 prior to 300100 since is a bother to most users

void set_pre_initialization_warning_max(int new_max) {
	pre_initialization_warning_max = new_max;
};

void say_error(int message_id, boolean always_tell_user) {
	if (about_to_quit() || !always_tell_user) { // always_tell_user new on 130200 for now most errors are just logged
		if (about_to_quit()) { // new on 210102
			tt_error_file() << "Have begun program shutdown." << endl;
		};
      log(S(message_id));
   } else {
      if (limited_text_to_speech()) tt_martian->just_talk_balloons_for(30000); // 30 seconds should be enough
	   say(message_id,5,TRUE);
   };
	tt_err_file_important = !about_to_quit(); // was just TRUE prior to 210102
};

void say_error(const_string message, boolean always_tell_user) {
 	if (about_to_quit() || !always_tell_user) {
		if (about_to_quit()) { // new on 210102
			tt_error_file() << "Have begun program shutdown." << endl;
		};
      log(message);
   } else {
      if (limited_text_to_speech()) tt_martian->just_talk_balloons_for(30000); // 30 seconds should be enough
	   say(message,5,TRUE);
   };
	tt_err_file_important = !about_to_quit();
};

string return_file_name_error(int message_id, string file_name) {
#if TT_NEW_IO
	output_string_stream message;
#else
   string buffer = new char[512];
   output_string_stream message(buffer,512);
#endif
   message << S(message_id) << space() << file_name << end_sentence();
	message.put('\0');
	return(buffer);
};

int file_errors = 0;

void say_file_name_error(int message_id, string file_name, int importance, string extension) {
	if (file_name == NULL) return; // added on 171199 - something is broken so don't try - maybe warn??
	string message;
	if (already_has_extension(file_name,extension) || extension == NULL) {
		message = return_file_name_error(message_id,file_name);
	} else {
		string full_file_name = append_3_strings(file_name,".",extension);
		message = return_file_name_error(message_id,full_file_name);
		delete [] full_file_name;
	};
	log(message);
	string spoken_alternative_message = NULL;
	if (alternative_language_spoken()) {
		spoken_alternative_message = S(message_id,"",TRUE);
	};
	if (file_errors > 3) { // new on 111000
		log(message,FALSE,FALSE); 
		// last arg was TRUE prior to 180702 but not necessarily so important -- prior to 101204 was (importance > 2)
		tt_err_file_important_but_no_error = (importance > 2); // new on 101204
	} else {
		martian_says(message,importance,spoken_alternative_message);
	};
	delete [] message; // new on 240601
	file_errors++;
};

void say(int message_id, int importance, boolean log_too) {
	if (tt_user_is_debugger || log_too) {
		log(S(message_id),FALSE,(importance > 4));
	};
	if (tt_system_mode == PUZZLE) return;
	if (has_initialization_completed() && tt_titles_ended_on_frame <= tt_frame_number) { // tt_programmer->are_titles_over()) {
	   martian_says(message_id,importance);
	} else if (pre_initialization_warning_max > 0) {
		message_box(MB_ICONINFORMATION|MB_SYSTEMMODAL,S(message_id));
		// |MB_SYSTEMMODAL added on 150400 so these boxes aren't "hidden"
		pre_initialization_warning_max--;
	};
};
  
void say(const_string message, int importance, boolean log_too) {
	if (tt_user_is_debugger || log_too) {
		log(message,FALSE,(importance > 4));
	};
	if (tt_system_mode == PUZZLE) return;
	if (has_initialization_completed() && tt_titles_ended_on_frame <= tt_frame_number) { // tt_programmer->are_titles_over()) {
	   martian_says(message,importance,NULL);
	} else if (tt_time_travel == TIME_TRAVEL_JUST_PAUSED) { 
		// branch new on 071003 so for example warnings loading file birds don't show up here
		tt_error_file() << "Warning while jumping between time travel segments: " << message << endl;
	} else if (pre_initialization_warning_max > 0) {
		message_box(MB_ICONINFORMATION,message);
		pre_initialization_warning_max--;
	};
};

void describe_key_stroke(int key, output_stream &text_out) {
//	if (rubout(key)) {
//		text_out << "backspace";
//	} else if (key == '\r') {
//      text_out << "enter";
//	} else {
	if (key == ' ') {
		text_out << S(IDS_THE_SPACE_BAR); // clearer and can be spoken
	} else if (key >= 32 && key < 128) { // second condition new on 160205
		text_out << "'" << (char) key << "'";
	} else {
		text_out << character_name(key,current_language());
	};
};

// caller should copy this if kept
char letter[2]="a";
//char control_letter[10]="Control-a";
char fn[4]="Fnn";

const_string character_name(int key, NaturalLanguage language) {
	if (key == 0) {
		return(NULL); // new on 160499 so that sensors go black rather than blank when nothing is there
	};
   int offset = (language-AMERICAN)*16; // 16 named keys below
	if (language == BRITISH) offset = 0; // same as AMERICAN
	if (key > 255) { // if key is >= 256 then is an extended key
		key -= 256; // removed (short int) on 160205 since it was silly
		if (key >= VK_F1 && key <= VK_F16) {
			int n = key-VK_F1+1;
			itoa(n,fn+1,10);
			return(a2u(fn)); 
		};
      switch (key) {
			case VK_HOME:
				return(S(IDS_VK_HOME+offset));
			case VK_END:
				return(S(IDS_VK_END+offset));
			case VK_PRIOR:
				return(S(IDS_VK_PRIOR+offset));
			case VK_NEXT:
				return(S(IDS_VK_NEXT+offset));
			case VK_LEFT:
				return(S(IDS_VK_LEFT+offset));
			case VK_RIGHT:
				return(S(IDS_VK_RIGHT+offset));
			case VK_UP:
				return(S(IDS_VK_UP+offset));
			case VK_DOWN:
				return(S(IDS_VK_DOWN+offset));
			case VK_DELETE:
				return(S(IDS_VK_DELETE+offset));
			case VK_INSERT:
				return(S(IDS_VK_INSERT+offset));
			default:
				return(S(IDS_OTHER_SPECIAL_KEY+offset));
		};
	};
	if (key >= 32 && key < 128) {
		letter[0] = (char) key;
		return(a2u(letter));
	};
	if (rubout((char) key)) return(S(IDS_BACKSPACE_KEY+offset));
	if (key == '\r') return(S(IDS_ENTER_KEY+offset));
	if (key == VK_ESCAPE) return(S(IDS_ESCAPE_KEY+offset));
	if (key == 0) return(_T(""));
	// lots more
	if (key >= 0 && key < 32) {
      string sample = SC(IDS_SAMPLE_CONTROL_LETTER+offset);
		sample[_tcslen(sample)-1] = (char) (key+96);
		return(sample); // expected to use it right away to print...
	};
	return(S(IDS_SOME_KEY+offset));
};


/*
void print_the(output_stream &text_out, boolean capitalize_first,
               SpriteType type, string noun) { 
   switch (tt_language) {
      case AMERICAN:
      case BRITISH: 
         if (capitalize_first) {
            text_out << S(IDS_BEGIN_THE);
         } else {
            text_out << S(IDS_THE);
         };
         return;
      case SWEDISH:
         {
          string the;
          if (type == NONE_GIVEN && noun == NULL) { // added 2nd condition
             the = S(IDS_THE); // just the worst case default
          } else {
             string definite_form;
             if (noun == NULL) {
                definite_form = S(IDS_NUMBER_TYPE_DEFINITE+type);
             } else {
                definite_form = noun;
             };
             int length = strlen(definite_form);
             for (int i = 0; i < length; i++) {
                if (definite_form[i] == ' ') {
                   length = i; // ignore anything after blank
                   break;
                };
             };
             if (definite_form[length-1] == 't') {
                the = "det";
             } else if (definite_form[length-1] == 'n') {
                the = "den";
             } else { // something wrong
                the = S(IDS_THE);
             };
          };
          if (capitalize_first) {
             the[0] = 'D';
          };
          text_out << the;
         };
         break;
   }; 
};
*/
