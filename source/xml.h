// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.

#ifndef __XML_H

#define __XML_H

void intialize_xml_tag_table();
void release_xml_tag_table();

xml_document *document_from_file(string file_name);
boolean xml_entity(xml_document *document, Entity *entity);
boolean xml_entity(xml_node *parent, Entity *entity, boolean also_no_more_tags=TRUE);
//boolean xml_block(xml_node *parent, Block *block);
//boolean xml_house(xml_node *parent, House *house);

Sprite *xml_load_sprite_from_file(string file_name, boolean &aborted, Sprite *sprite=NULL);
Sprite *xml_load_sprite(string xml_string, boolean &aborted, Sprite *sprite=NULL, boolean floors_or_houses_ok=TRUE, boolean warn=TRUE);
Sprite *xml_load_sprite(wide_string xml_string, boolean &aborted, Sprite *sprite=NULL, boolean floors_or_houses_ok=TRUE, boolean warn=TRUE);
Sprite *xml_load_sprite(xml_document *document, boolean &aborted, Sprite *sprite=NULL, boolean floors_or_houses_ok=TRUE);
Sprite *xml_load_sprite(xml_node *node, Tag tag, Sprite *sprite=NULL, SpriteType type=NONE_GIVEN, boolean floors_or_houses_ok=TRUE);
Sprites *xml_load_sprites(xml_node *parent, boolean NULL_elements_ok=FALSE);
Sprites *xml_load_sprites(xml_document *document, boolean NULL_elements_ok=FALSE);
Sprites *xml_load_sprites(InputStream *stream, boolean NULL_element_ok=FALSE);
Sprite *xml_load_sprite(InputStream *stream, boolean &abort);

Sprites *xml_load_sprites_from_floor(string xml_string);

Tag tag_token(xml_node *element); // added here on 111202 

void xml_load_geometry(Sprite *sprite, xml_node *geometry_node);
//void xml_load_flags(Sprite *sprite, xml_node *flags_node);
boolean xml_process_children(xml_node *parent, ChildTagHandler handler, Entity *entity, void *extra_info=NULL);
boolean xml_process_attributes(xml_node_map *attribute_map, AttributeHandler handler, void *extra_info=NULL);
xml_node_map *xml_get_xml_node_map(xml_element *element);
int xml_attribute_length(xml_node_map *map);

int xml_get_int(xml_node *node, int default_value=0);
int64 xml_get_int64(xml_node *node, int64 default_value=0);
double xml_get_double(xml_node *node, double default_value=0);
wide_string xml_get_text_copy(xml_node *node);
BSTR xml_get_text(xml_node *node);
wide_string xml_get_text_all_copy(xml_node *node, int &result_length);
wide_character xml_get_text_first_character(xml_node *node);
string xml_get_narrow_text_copy(xml_node *node);
Sprite *xml_get_sprite(xml_node *node, Sprite *sprite=NULL, SpriteType type=NONE_GIVEN);
NumberValue *xml_get_number_value(xml_node *node);
xml_document *xml_get_document(xml_node *node);
xml_element *xml_get_document_element(xml_document *document);

void xml_to_stream(xml_node *node, output_stream &stream);
xml_document *xml_from_stream(InputStream *stream, long length=-1); // default new on 140203
void ignore_xml_from_stream(InputStream *stream, long length=-1);

xml_document *create_xml_document();

xml_element *create_xml_element(wide_string tag, xml_document *document);
xml_element *create_xml_element(wide_string tag, xml_document *document, Sprite *sprite);
xml_element *create_xml_element_shared(wide_string tag, xml_document *document, Sprite *sprite); // boolean include_file_ok); // possibly sprite is shared version
xml_element *create_xml_element(wide_string tag, xml_document *document, long long_value);
//xml_element *create_xml_element(wide_string tag, xml_document *document, Sprite *sprite, boolean might_be_shared);
xml_element *create_xml_element(wide_string tag, xml_document *document, NumberValue *value);
xml_element *create_xml_element(wide_string tag, xml_document *document, string narrow_value, long string_length=-1);
xml_element *create_xml_element(wide_string tag, xml_document *document, wide_string value);

void add_xml_value(xml_element *element, xml_document *document, long long_value);
void add_xml_value(xml_element *element, xml_document *document, wide_string value);
void add_xml_value(xml_element *element, xml_document *document, string narrow_value, long string_length=-1);
void add_xml_value(xml_element *element, xml_document *document, Sprite *sprite);

xml_character_data *create_xml_character_data(wide_string data_string, xml_document *document);
xml_character_data *create_xml_character_data(string data_string, xml_document *document);
xml_text *create_xml_text(wide_string data_string, xml_document *document);

xml_element *xml_character_data_element(wide_string text, wide_string tag, xml_document *document);
xml_element *xml_character_data_element(string text, wide_string tag, xml_document *document);

boolean xml_set_attribute(xml_element *element, wide_string name, wide_string value);
boolean xml_set_attribute(xml_element *element, wide_string name, string value);
boolean xml_set_attribute(xml_element *element, wide_string name, int value);
boolean xml_set_attribute(xml_element *element, wide_string name, LONGLONG value);
boolean xml_set_attribute(xml_element *element, wide_string name, double value);

boolean xml_remove_attribute(xml_element *element, wide_string name);

int xml_get_attribute_int(xml_node *node, wide_string attribute_name, int default_value=0);
int64 xml_get_attribute_int64(xml_node *node, wide_string attribute_name, int64 default_value=0);
double xml_get_attribute_double(xml_node *node, wide_string attribute_name, double default_value=0);
wide_character xml_get_attribute_wide_character(xml_node *node, wide_string attribute_name);
string xml_get_attribute_string(xml_node *node, wide_string attribute_name, string default_value=NULL);
wide_string xml_get_attribute_wide_string(xml_node *node, wide_string attribute_name, wide_string default_value=NULL);
BSTR xml_get_attribute_BSTR(xml_node *node, wide_string attribute_name);
boolean xml_has_attribute(xml_node *node, wide_string attribute_name);

boolean add_processing_instruction(xml_document *document);
boolean add_comment(xml_document *document, wide_string string_comment);

boolean xml_append_child(xml_node *node, xml_node *parent); // new on 190703 to replace the following
//boolean xml_append_child(xml_node *node, xml_document *document);
//boolean xml_append_child(xml_node *node, xml_element *parent);
//boolean xml_append_child(xml_comment *node, xml_document *document);
//boolean xml_append_child(xml_processing_instruction *node, xml_document *document);
//boolean xml_append_child(xml_element *node, xml_document *document);
//boolean xml_append_child(xml_character_data *node, xml_element *parent);
//boolean xml_append_child(xml_text *node, xml_element *parent);
//boolean xml_append_child(xml_element *node, xml_element *parent);

xml_node *first_node_that_is_an_element(xml_node *parent);
xml_element *first_element_with_tag_name(xml_node *parent, wide_string tag_string);
xml_element *xml_node_to_element(xml_node *node);

inline boolean sprite_tag(Tag tag) {
   return(tag <= LINK_TAG); // make sure only sprite tags come before LINK_TAG
   // may as well allow TOONTALK_OBJECT_TAG via cut and paste
};

class Numbered_Sprites { // made into a class on 190103 so can save and restore for recursive top_level calls
	public:
		Numbered_Sprites(SpritePointer *serial_numbered_sprites=NULL, int max_serial_number=0, int max_serial_number_used=0) :
			serial_numbered_sprites(serial_numbered_sprites),
			max_serial_number(max_serial_number),
			max_serial_number_used(max_serial_number_used) {
		};
		~Numbered_Sprites() {
			if (serial_numbered_sprites != NULL) delete [] serial_numbered_sprites;
		};
		void initialize_serial_numbering(int max_needed);
		void remember_if_serial_numbered(int index, Sprite *sprite);
		Sprite *serial_numbered_sprite(int index);
		int return_max_serial_number() {
			return(max_serial_number_used);
		};
	private:
		SpritePointer *serial_numbered_sprites;
		int max_serial_number;
		int max_serial_number_used;
};

//void reset_serial_numbering();
void allocate_serial_numbers(xml_node *node);
int return_max_serial_number();
void initialize_serial_numbering(int max_needed);
Numbered_Sprites *push_current_numbered_sprites();
void restore_numbered_sprites(Numbered_Sprites *old_table);
void release_numbered_sprites();

wide_string xml_indent(wide_string xml, int xml_string_length, int indent_increment, int max_width, int &indented_xml_length, wide_string extra_processing_instructions=NULL);

boolean xml_save_document(xml_document *document, string full_file_name, string extra_file_for_archive=NULL, string extra_media_file_names=NULL, boolean zip_files=tt_zip_files);

boolean xml_release_document(xml_document *document);

xml_node *xml_clone_node(xml_node *node);
xml_document *xml_copy_document(xml_node *node);
#if TT_ALPHA4
xml_node *xml_copy_first_element(xml_node *node);
#endif
xml_node *xml_shallow_copy(xml_node *node);

class Element_Sprite_Table { // made into a class on 190103 so can save and restore for recursive top_level calls
	public:
		Element_Sprite_Table(int element_sprite_table_size=0, int max_element_sprite_table_size=0,
									SpritePointer *sprites=NULL, xml_element_pointer *elements=NULL) :
			element_sprite_table_size(element_sprite_table_size),
			max_element_sprite_table_size(max_element_sprite_table_size),
			sprites(sprites),
			elements(elements),
			sprites_with_newly_minted_guids(NULL) {	};
		~Element_Sprite_Table();
//		void reset_element_sprite_table();
		void associate_element_and_sprite(xml_element *element, Sprite *sprite);
		xml_element *reference_to_existing_element(Sprite *sprite, xml_document *document);
		xml_element *remove_existing_element(Sprite *sprite);
		void process_sprites_with_newly_minted_guids(Element_Sprite_Table *element_sprite_table);
		void add_to_sprites_with_newly_minted_guids(Sprite *sprite);
#if !TT_DEBUG_ON
	private: // public if debugging
#endif
		int element_sprite_table_size;
		int max_element_sprite_table_size;
		SpritePointer *sprites;
		xml_element_pointer *elements;
		Sprites *sprites_with_newly_minted_guids; // new on 310803 - these are to deal with push and restoring these tables
		// so if pushed and guids are minted the restored tables learn about this as well
};

xml_element *reference_to_existing_element(Sprite *sprite, xml_document *document);
xml_element *remove_existing_element(Sprite *sprite);
void add_to_sprites_with_newly_minted_guids(Sprite *sprite);
void associate_element_and_sprite(xml_element *element, Sprite *sprite);
//void reset_current_element_sprite_table();
Element_Sprite_Table *push_current_element_sprite_table(); // new on 190103
void restore_current_element_sprite_table(Element_Sprite_Table *old_table, xml_document *document);
void release_current_element_sprite_table();

void xml_debug_node(xml_node *node, wide_string label=NULL);

wide_string tag_to_wide_string(Tag tag_code); // only used for performance and debugging

void dump_xml_include_file(xml_element *element, int include_location);
void dump_xml_to_zip_file(xml_element *element, string file_name);

xml_document *document_from_string(string xml_string, int length=-1, boolean originally_from_a_file=FALSE, boolean warn=TRUE);
xml_document *document_from_string(wide_string xml_string, boolean originally_from_a_file, boolean warn=TRUE);

typedef xml_document *xml_document_pointer;

#endif