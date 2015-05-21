// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved
#ifndef __SOUND_H

#define __SOUND_H

class ForceRemoteControlee;

class Sound : public Text {
  public:
    Sound(city_coordinate llx=0, city_coordinate lly=0,
			 wide_string text=NULL, long text_length=-1,
          long priority=0,
          city_coordinate width=maximum_width(NUMBER_PAD_SPRITE),
          city_coordinate height=0,
//          boolean ok_to_reuse_initial_text=FALSE,
          color_index text_color=tt_black,
          short unsigned int index=0) :
        Text(llx,lly,text,text_length,priority,
			    width,height,text_color) {
       set_sprite_type(SOUND);
		 default_plate_state = SOUND_PAD_WITH_PLATE; // new on 121104 since used when copying text pads
    };
//    void prepare_for_deletion();
    PlateState plate_state() {
       return(SOUND_PAD_WITH_PLATE);
    };
    Sprite *respond_to_keyboard(unsigned char key, boolean extended, boolean , Sprite *by,
										  boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless=FALSE);
    Sprite *used(Sprite *subject, Sprite *by, boolean button_unchanged,
				     boolean record_action, millisecond , boolean modified, Sprite *original_subject);
    //ReleaseStatus add_to_side(boolean right_side, Sprite *other,
				//					   Sprite *by,
				//					   millisecond duration, Sprite *original_recipient, Sprite *original_item);
//    boolean receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item);
    void generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection);
    void java(java_stream &out);
    void set_inside_vacuum(boolean new_flag, boolean recur=TRUE);
	 boolean ok_to_dump_as_text() {
		 return(FALSE);
	 };
	 color appropriate_background_color() {
		 return(sound_background_color);
	 };
	 color_index appropriate_background_color_index() {
		 return(tt_nearest_sound_background_color);
	 };
	 boolean looks_like_is_of_type(SpriteType type) {
		 // new on 071104 to re-enable dropping things sounds, especially speech sounds 
		 return(type == TEXT || sprite_type == type);
	 };
};

class MCISound : public Sound {
  public:
   MCISound(city_coordinate llx=0, city_coordinate lly=0,
			   wide_string text=NULL, long text_length=-1,
		      long priority=0,
			   city_coordinate width=maximum_width(NUMBER_PAD_SPRITE),
			   city_coordinate height=0,
//			   boolean ok_to_reuse_initial_text=FALSE,
            color_index text_color=tt_black,
            short unsigned int index=0) :
      Sound(llx,lly,text,text_length,priority,
			   width,height,text_color) {
   };
   boolean make_sound();
   SpriteType dump_type();
   Sprite *copy(boolean also_copy_offsets=FALSE);
#if TT_XML
    wide_string xml_tag() {
       return(L"MCISound");
    };
#endif
//  protected:
//   string command;
};

class SpeechSound : public Sound {
  public:
   SpeechSound(city_coordinate llx=0, city_coordinate lly=0,
			      wide_string text=NULL, long text_length=-1,
		         long priority=0,
			      city_coordinate width=maximum_width(NUMBER_PAD_SPRITE),
			      city_coordinate height=0,
//			      boolean ok_to_reuse_initial_text=FALSE,
               color_index text_color=tt_black,
               short unsigned int index=0) :
      Sound(llx,lly,text,text_length,priority,
			   width,height,text_color) {
   };
   boolean make_sound();
   void stop_making_sound();
   Sprite *copy(boolean also_copy_offsets=FALSE);
   SpriteType dump_type();
   //ReleaseStatus add_to_side(boolean right_side, Sprite *other,
			//						  Sprite *by,
			//						  millisecond duration, Sprite *original_recipient, Sprite *original_item);
//	boolean receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item);
#if TT_SPEAK_ALL_NUMBERS
	void set_wide_text(wide_string text, int new_text_length=-1,
							 boolean text_was_updated=FALSE,
							 boolean size_constrained_regardless=FALSE);
#endif
	void java(java_stream &out);
#if TT_XML
    wide_string xml_tag() {
       return(L"Speech");
    };
#endif
};

class WaveSound : public Sound {
  public:
   WaveSound(city_coordinate llx=0, city_coordinate lly=0,
			    wide_string text=NULL, long text_length=-1,
		       long priority=0,
			    city_coordinate width=maximum_width(NUMBER_PAD_SPRITE),
			    city_coordinate height=0,
//			    boolean ok_to_reuse_initial_text=FALSE,
             color_index text_color=tt_black);
	void release_all_references();
	void set_wide_text(wide_string text, int new_text_length=-1,
							 boolean text_was_updated=FALSE,
							 boolean size_constrained_regardless=FALSE);
	void initialize_sound(wide_string text);
   boolean make_sound();
	void load_sound_and_generate_hash();
   void stop_making_sound();
   Sprite *copy(boolean also_copy_offsets=FALSE);
   SpriteType dump_type();
   void java(java_stream &out);
	string return_dump_text();
//	boolean receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item);
#if TT_XML
   xml_character_data *xml_create_character_data(xml_element *element, xml_document *document);
   wide_string xml_tag() {
      return(L"Sound");
   };
//	xml_element *xml_create_element(xml_document *document);
	void xml(xml_element *element, xml_document *document);
//	boolean handle_xml_tag(Tag tag, xml_node *node);
#endif
	hash return_hash() {
		return(sound_hash);
	};
	void set_hash(hash new_hash) {
		sound_hash = new_hash;
	};
	void set_hash_string(string hash_string);
	string private_media_file_name(boolean &worth_compressing, boolean full_path=TRUE);
	wide_string private_media_file_extension() {
		return(L"wav");
	};
	void set_sound_file_name(string file_name) {
		sound_file_name = file_name; // this is only used by copy so no need to deal with previous value -- new on 151103
	};
  protected:
   sound_buffer sound;
	string sound_file_name; // new on 291099 so displayed name can be short
	hash sound_hash; // new on 203030
//	string original_file_name;
	int sound_buffer_table_index;
};

class BuiltInSound : public Sound {
  public:
	 BuiltInSound(city_coordinate llx=0, city_coordinate lly=0,
					  wide_string text=NULL, long text_length=-1,
					  long priority=0,
					  city_coordinate width=maximum_width(NUMBER_PAD_SPRITE),
					  city_coordinate height=0,
//					  boolean ok_to_reuse_initial_text=FALSE,
					  color_index text_color=tt_black,
					  short unsigned int index=0) :
      Sound(llx,lly,text,text_length,priority,
			   width,height,text_color),
      index(index) {
    };
    Sprite *copy(boolean also_copy_offsets=FALSE);
    SpriteType dump_type();
    boolean make_sound();
    void set_index(unsigned short int new_index) {
      index = new_index;
    };
    void java(java_stream &out);
    boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
    boolean built_in_sound() {
       return(TRUE);
    };
#if TT_XML
//    xml_element *xml_create_element(xml_document *document);
	 void xml(xml_element *element, xml_document *document);
    boolean handle_xml_tag(Tag tag, xml_node *node);
    wide_string xml_tag() {
       return(L"BuiltInSound");
    };
#endif
  protected:
    short unsigned int index;
};

class ForceFeedback : public Sound { // is kinda like a Sound
  public:
   ForceFeedback(int joystick_number,
                 ForceRemoteControlee *parameter_controlee,
//                 long intensity, long direction, long duration, unsigned char trigger_button,
                 city_coordinate llx=0, city_coordinate lly=0,
			        wide_string text=NULL, long text_length=-1,
		           long priority=0,
			        city_coordinate width=maximum_width(NUMBER_PAD_SPRITE),
			        city_coordinate height=0,
//                 boolean ok_to_reuse_initial_text=FALSE,
                 color_index text_color=tt_black,
                 short unsigned int index=0) :
      joystick_number((unsigned char) joystick_number),
      parameter_controlee(parameter_controlee),
//      toggle_on(TRUE),
//      intensity(intensity),
//      direction(direction),
//      duration(duration),
//      trigger_button(trigger_button),
      Sound(llx,lly,text,text_length,priority,
			   width,height,text_color) {
			set_sprite_type(FORCE_FEEDBACK); // added on 310799 so Marty describes these right when no inside something else
			default_plate_state = FORCE_PAD_WITH_PLATE; // new on 121104 since used when copying text pads
   };
   boolean make_sound();
   void stop_making_sound();
   SpriteType dump_type();
   Sprite *copy(boolean also_copy_offsets=FALSE);
   boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
//   void display_changed(RemoteIdentifier identifier, user_coordinate new_value); // commented out on 220802 since never defined
	void generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection);
	PlateState plate_state() { // new on 171201
      return(FORCE_PAD_WITH_PLATE);
   }; 
	color appropriate_background_color() {
		return(force_background_color);
	};
	color_index appropriate_background_color_index() {
		return(tt_nearest_force_background_color);
	};
#if TT_XML
//    xml_element *xml_create_element(xml_document *document);
	 void xml(xml_element *element, xml_document *document);
    boolean handle_xml_tag(Tag tag, xml_node *node);
    wide_string xml_tag() {
       return(L"ForceFeeback");
    };
#endif
    void set_joystick_number(int n) {
       joystick_number = n;
    };
    void set_parameter_controlee(ForceRemoteControlee *c) {
       parameter_controlee = c;
    };
//   long current_intensity() {
//      return(intensity);
//   };
//   long current_direction() {
//      return(direction);
//   };
//   long current_duration() {
//      return(duration);
//   };
//   long current_trigger_button() {
//      return(trigger_button);
//   };
//   int return_force_index() {
//      return(force_index);
//   };
  protected:
   unsigned char joystick_number;
   ForceRemoteControlee *parameter_controlee;
//   flag toggle_on;
//   long intensity, direction, duration;
//   unsigned char trigger_button;
};

Sound *load_sound(SpriteType type, InputStream *pad_in, int , NaturalLanguage , long index, Sprite *container, boolean descendants_inactive);
boolean describe_sound(SpriteType type,
                       InputStream *pad_in, output_stream &text_out, int version_number,
                       boolean in_thought_bubble, Article article);

#endif
