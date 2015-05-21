// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.
#ifndef __TT_PAD_H

#define __TT_PAD_H

#if !defined(__TT_TOOLS_H)   
#include "tools.h"
#endif   
#if !defined(__TT_EVENT_H)   
#include "event.h"
#endif   

class Cubby;
class Programmer_At_Floor;
class Pages; // defined below

enum OverlayStatus {OVERLAY_STARTED,OVERLAY_FINISHING,OVERLAY_OFF};

enum AddToPageNumber {ADD_TO_LEFT_PAGE, ADD_TO_RIGHT_PAGE, DONT_ADD_TO_PAGE};

typedef Notebook *NotebookPointer;

enum BuiltinNotebooks {LOCAL_REMOTES_NOTEBOOK,PICTURES_NOTEBOOK,
                       GLOBAL_REMOTES_NOTEBOOK,
                       SOUNDS_NOTEBOOK,OPTIONS_NOTEBOOK,MAIN_NOTEBOOK,
                       EXAMPLES_NOTEBOOK,USER_NOTEBOOK,
							  MATH_NOTEBOOK}; // new on 160104

class Page;
typedef Page *PagePointer;

class Notebook : public Sprite {
	public:
	  Notebook(city_coordinate x=0, city_coordinate y=0,
				  long initial_priority=0,
				  ascii_string file_name=NULL,
				  city_coordinate width=0,
				  city_coordinate height=0,
				  Sprite *comment=NULL,
				  Pages *pages=NULL,
              Picture *picture=NULL,
				  int version_number=latest_version_number);
	  void release_all_references(); //(Sprite *by);
     void remove_pages();
	  Sprite *copy(boolean also_copy_offsets=FALSE);
//	  void flush_cache();
	  void update_display(const UpdateStatus update_status=UPDATE_CHANGE);
	  void display(SelectionFeedback selection_feedback=NO_FEEDBACK,
						boolean followers_too=TRUE, TTRegion *region=NULL);
	  void set_background(Background *new_background, boolean recur=FALSE, boolean warn=TRUE);
	  void generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection);
	  void describe(output_stream &message, Article article);
     MatchStatus match(Notebook *other_notebook, int version);
	  void move_to_hand(); // Programmer_At_Floor *the_programmer);
     void dramatic_entrance(millisecond duration);
	  void get_grasped();
	  void stop_running();
//     void locomote_to_favorite_location_and_size();
     Sprite *released(Sprite *by, boolean top_level=TRUE, SpecialDrop special_drop=NOT_SPECIAL_DROP);
     void now_on_floor(Background *floor, Sprite *by);
     void fly_to_favorite_location(millisecond duration,
                                   boolean end_up_on_first_page=TRUE);
	  void fly_to(city_coordinate goal_x, city_coordinate goal_y,
					  millisecond duration, boolean end_up_on_first_page=TRUE);
	  void settle_down();
	  void finished_settling_down();
	  void go_to_a_blank_page();
	  void go_to_recent_page(boolean show_page_turning=FALSE);
     int last_non_blank_page_number();
	  void go_to_page(short int n, boolean show_page_turning=FALSE,
							boolean check_range=FALSE,
							boolean just_set_page_offset_number=FALSE,
							Robot *robot=NULL);
	  void switch_contents(Sprite *by=NULL);
	  void finish_switching_contents();
//	  void signal_size_and_location();
	  void fit_on_page(boolean left);
	  void turn_page(boolean forward);
	  void continue_turn_page();
	  void forward_switch_contents();
	  void overlay_previous_done();
     Sprite *used(Sprite *, Sprite *by, boolean , boolean , millisecond ,
                  boolean modified, Sprite *original_subject);
	  Sprite *respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected, Sprite *by,
										   boolean record_action, Sprite *original_recipient, boolean size_constrained_regardless=FALSE);
	  ArrowKeyEditing arrow_key_editing_status();
	  void unselect();
	  void go_back_a_page(Sprite *by, boolean show_page_turing, boolean just_set_recent_page_number, Robot *robot);
	  Sprites *java_pages();
	  Sprites *compute_java_pages();
     void java(java_stream &out);
     void java_after(java_stream &out, const_java_string path);
	  void java_initialize(java_stream &out);
     void java_class(java_stream &out);
	  void java_flipped_followers(java_stream &out, const_java_string picture_name);
	  Sprites *reachable_sprites_of_type(SpriteType type);
	  Sprites *reachable_non_equal_sprites_of_type(SpriteType type);
	  void define_page(boolean dump=TRUE, boolean switch_contents_too=TRUE);
	  boolean save_encoding();
	  void save_encoding_internal();
//	  boolean left_side(Sprite *item);
	  millisecond set_page(Sprite *item, boolean left=TRUE, Sprite *by=NULL);
	  void dump();
	  void xml_to_stream(output_stream &stream);
	  boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
	  void dump_contents(output_stream &stream);
	  void load(); // rationalized on 210802 -- ascii_string file_name, boolean add_suffix=TRUE);
	  void load_page_resource(ascii_string resource_name);
	  int load_notebook(InputStream *pad_in);
     void become_main_notebook();
	  short int add_remotes(RemoteIdentifier *identifiers, int start, int count,	Picture *controlee);
	  void picture_deleted(Picture *picture);
	  void add_page(int page_number, Sprite *page);
//	  void add_right_page_as_follower();
	  void which_side(TTRegion *region,
							boolean &left_item_selected, boolean &right_item_selected);
	  ReleaseStatus item_released_on_top(Sprite *item, Sprite *by, SpritePointer &recipient, int level);
	  boolean receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item);
	  ReleaseStatus add_to_side(boolean right_side, Sprite *other,
										Sprite *by, 
										millisecond duration, Sprite *original_recipient, Sprite *original_item);
	  void set_to_future_value(Sprite *other, Sprite *original_recipient, Sprite *by);
	  boolean vacuum_if_left_on_floor(SpritePointer *others_being_vaccuumed, int others_count);
	  void go_to_items_page();
	  int page_number_of_item(Sprite *item);
//     short int page_number_of_subnotebook_with_item(Sprite *item);
	  boolean closer_to_left_page(TTRegion *region);
	  Sprite *select(TTRegion *region, SelectionReason reason, Sprite *in_hand);
	  Sprite *selected(SelectionReason reason, Sprite *by);
	  Sprite *copy_page(Sprite *page, SelectionReason reason, Sprite *by, Background *background_of_by);
	  void remove_selected_element(Sprite *element, SelectionReason reason, Sprite *by, boolean add_to_floor);
	  void remove_page(short int page_number);
	  void remove_all_pages();
	  void new_file_name(int number);
	  void set_inside_vacuum(boolean new_flag, boolean recur=TRUE);
//	  boolean vacuum_if_left_on_floor() {
//		 return(!shared && Sprite::vacuum_if_left_on_floor());
//	  };
	  short int right_page_number(Sprite *by=NULL);
	  short int left_page_number(Sprite *by=NULL);
	  Sprite *pointer_to_right_page() {
		  return(right_page);
	  };
	  Sprite *pointer_to_left_page() {
		  return(left_page);
	  };
	  Sprite *pointer_to_page_i_am_on(Sprite *sprite);
	  Sprite *pointer_to_page(short int n, Sprite *by=NULL, boolean copy_page=TRUE);
	  boolean ok_to_delete();
	  void set_picture(Picture *new_picture);
	  void just_set_picture(Picture *new_picture) {
		  picture = new_picture;
	  };
	  Picture *pointer_to_picture() {
		  return(picture);
	  };
//     Events *pointer_to_body();
	  ascii_string pointer_to_file_name() {
		  return(file_name);
	  };
//	  void initialize_file_name(ascii_string name); // noticed obsolete on 130104
	  void set_original(boolean new_flag) {
		  original = (flag) new_flag;
	  };
	  int page_count();
     int blank_page_number();
	  void set_recent_page_number(short int n, Robot *robot);
	  short int return_recent_page_number() {
		  return(recent_page_number);
	  };
	  short int return_page_number_offset() {
		 return(page_number_offset);
	  };
	  void reset_page_number_offset() {
		  page_number_offset = 0;
	  };
	  boolean is_left_page_selected() {
		  return(left_page_selected);
	  };
	  void set_left_page_selected(boolean new_flag) {
			left_page_selected = (flag) new_flag;
	  };
	  unsigned char return_version_number() {
		  return(version_number);
	  };
     void set_version_number(int number) {
        version_number = (unsigned char) number;
     };
     void set_language(NaturalLanguage l) {
        language = l;
     };
     void current_favorite(FavoriteSizeAndLocation &f) {
         f.favorite_width = favorite_width;
         f.favorite_height = favorite_height;
         f.favorite_llx = favorite_llx;
         f.favorite_lly = favorite_lly;
     };
     void set_favorite(FavoriteSizeAndLocation &f) {
		  if (f.favorite_width > 0) { // conditional added 181000
				favorite_width = f.favorite_width;
				favorite_height = f.favorite_height;
				favorite_llx = f.favorite_llx;
				favorite_lly = f.favorite_lly;
		  };
     };
    void set_default_favorite_size_and_location(city_coordinate width, city_coordinate height,
															   city_coordinate x, city_coordinate y);
    void favorite_size(city_coordinate &width, city_coordinate &height);
    void favorite_location(city_coordinate &x, city_coordinate &y) {
        x = favorite_llx;
        y = favorite_lly;
     };
     void set_favorite_size_and_location();
     void set_home_notebook(Notebook *notebook);
	  void stop_all(boolean for_good=FALSE, boolean suspend_robots_too=TRUE) {
	  }; // no need to do anything
	  void convert_to_latest_version_number(short unsigned int page_count=0);
	  Sprite *pointer_to_label() {
		  return(label);
	  };
	  void set_label(Sprite *new_label);
	  void set_label_string(const_string text);
	  void recursively_activate_pictures(boolean reset_animation=TRUE, 
		                                  PictureActivateContext context=PICTURE_ACTIVATE_CONTEXT_NOT_INTIALIZED) { 
		  // added on 080699 so don't activate any pictures inside of a notebook
	  };
	  //void set_called_since_last_release(boolean new_flag) { // noticed obsolete on on 171102
		 // called_since_last_release = (flag) new_flag;
	  //};
//	  Picture *return_picture_copy();
//	  void reset_picture_copy(Picture *new_copy);
	  boolean current_value(NumberValuePointer &the_value, boolean &shared, Sprite *by=NULL);
	  void set_file_name(string new_name);
	  void update_textual_label_size_and_location();
	  boolean return_changed_since_last_save() {
		  return(changed_since_last_save);
	  };
	  void set_changed_since_last_save(boolean new_flag) {
		  changed_since_last_save = new_flag;
		  //if (changed_since_last_save) {
			 // xml_location_of_last_good_full_save = 0; -- obsolete 160204
		  //};
	  };
	  boolean user_notebook() {
		  return(built_in_notebook == USER_NOTEBOOK); // new on 100200
	  };
	  boolean main_notebook() {
		  return(built_in_notebook == MAIN_NOTEBOOK); // new on 120101
	  };
	  boolean blankable() {
		  return(TRUE); // new on 220500 since can be in thought bubble
	  };
//	  void side_about_to_be_seen(boolean front) { // new on 311000
		  // do nothing
//	  };
	  Path *path_to(Sprite *target, boolean &ok); // new on 141200
	  int return_id() {
		  return(id);
	  };
	  void set_built_in_notebook(BuiltinNotebooks type) { // new on 050201
		  built_in_notebook = type;
	  };
	  boolean ok_to_cache_without_followers() {
		  return(FALSE); // new on 080201
	  };
	  boolean equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, 
		                SpritesPointer &expected_corresponding_nests, 
							 EqualReason reason=NORMAL_EQUAL);
	  Pages *pointer_to_old_pages() {
		  return(old_pages);
	  };
	  boolean is_built_in_notebook();
     void set_active(boolean new_flag, boolean recur=TRUE, boolean initialization=FALSE) { 
		  // new on 141002 so don't recur or turn pads gray (and break their animations)
	      active_flag = TRUE; // can never become inactive -- new on 151002
     };
	  void just_set_inactive() {
		 // ignore -- new on 140605
	  };
	  boolean pages_instatiated();
	  void instantiate_pages();
#if TT_XML
//     xml_element *xml_create_element(xml_document *document);
     void xml(xml_element *element, xml_document *document);
	  xml_element *xml_set_special_attributes(xml_element *element, xml_document *document);
	  boolean xml_get_special_attributes(xml_node *node);
	  void add_special_xml(xml_element *element, xml_document *document);
     xml_element *page_xml(int page_number, xml_document *document);
     boolean handle_xml_tag(Tag tag, xml_node *node);
     Sprite *page_sprite(int page_number, boolean expand_if_notebook_defined_by_file=TRUE);
     void set_page_internal(int page_number, Page *page);
     void compute_pages(boolean make_empty_notebook_if_need_be=FALSE);
     void expand_pages(int new_pages_count);
     void initialize_pages(PagePointer *shared_sprite_pages, int new_pages_count);
//     PagePointer *transfer_pages(int &count);
     PagePointer *pointer_to_pages(int &count) {
        count = pages_count;
        return(pages);
     };
	  short int default_parameter() { // new on 250103 - only used for dumping XML now
	     return(6);
	  };	  
		wide_string xml_tag() {
			return(L"Notebook"); // new on 090703
		};
//		void set_recent_xml_location(int location);
//		boolean notebook_is_only_on_default_user_directory();
#endif
	  boolean ok_to_rescue_from_exploding_house() {
		  return(built_in_notebook != LOCAL_REMOTES_NOTEBOOK); // no point rescuing the notebook of remote controls
	  };
//	  boolean ok_to_save_in_include_file() {
//		  return(xml_location_of_last_good_full_save == 0); 
	  // new semantics for this on 190703 - dont' make include if this will just use another include
////		  return(built_in_notebook != LOCAL_REMOTES_NOTEBOOK); // since picture would otherwise be saved separately
//	  };
#if TT_ALPHA_FEATURE
//		string collect_media_file_names();
#else
	  void set_media_file_names(string names);
	  void update_media_file_names(Sprites *sprites_with_media_to_save);
#endif
	  Sprite *pointer_to_page_being_defined() {
		  return(page_being_defined);
	  };
//	  void set_leader(Sprite *the_leader, boolean temporarily=FALSE);
#if TT_DEBUG_ON
	  boolean does_follower_matter(Sprite *sprite) {
		  return(sprite != label); // label can't change the behavior of things
	  };
#endif
	  boolean running_to_hand() {
		  return(running_to_hand_flag); // new on 221105
	  };
	private:
	  void compute_contents_dimensions(boolean left,
												  city_coordinate &contents_width,
												  city_coordinate &contents_height,
												  city_coordinate &contents_llx,
												  city_coordinate &contents_lly);
//	  void page_size(city_coordinate &page_width,	
//						  city_coordinate &page_height);
	  void page_region(boolean left, TTRegion &region);
	  void page_corner(boolean left, 
							 city_coordinate &page_llx, city_coordinate &page_lly);
	  city_coordinate page_width();
	  city_coordinate page_height();
	  Pages *old_pages;
	  Sprite *left_page;
	  Sprite *right_page;
//	  Tool *saved_page;
//	  bytes saved_encoding;
	  short int page_number, next_page_number, saved_page_number, recent_page_number;
	  short int page_number_offset;
	  Sprite *page_number_item;
//	  ActionStatus *saved_action_status;
//	  int saved_encoding_length;
//	  int pad_number;
	  ascii_string file_name;
	  //NotebookPointer *cache;
	  //short int cache_size;
//	  boolean current_page_shown : 1;
//	  flag shared;
	  OverlayStatus overlay_status;
	  Sprite *page_flipping;
//	  Programmer_At_Floor *programmer; -- beginning 171102 find appearance via tt_programmer
	  flag running_to_hand_flag : 1;
	  flag left_page_current : 1;
	  flag left_page_selected : 1;
	  flag original : 1;
//     flag called_since_last_release : 1;
//	  Sprite *saved_by; // restored on 171102 to replace saved_action_status
	  Picture *picture; // if notebook of a picture otherwise NULL
//	  Picture *picture_copy; // used in remote notebooks starting 251099
	  AddToPageNumber add_to_number;
     unsigned char version_number; 
     NaturalLanguage language;
     city_coordinate favorite_llx, favorite_lly, favorite_width, favorite_height;
	  city_coordinate default_favorite_llx, default_favorite_lly;
     BuiltinNotebooks built_in_notebook;
	  Sprite *label;
	  flag last_key_for_label : 1;
	  flag changed_since_last_save : 1; // only used for main notebook to record it in log segments
	  int id; // new on 151200 must be unique within a session for robots to maintain page offsets
     //flag left_page_active : 1; // new on 150702 - replaced by local activatable_flag on 160702
     //flag right_page_active : 1;
     flag loaded : 1; // new on 220802 -- only is TRUE if there is a file_name and its been loaded (or at least attempted)
#if TT_XML
//     SpritePointer *sprite_pages; 
	  // real sprites (not activated - and might be notebooks with unprocessed contents in another file)
     // includes NULL for empty pages
     PagePointer *pages; // new on 181002
     int pages_count;
     flag right_page_is_copy : 1; // made obsolete on 260103 - restored 2980103
     flag left_page_is_copy : 1;
//	  int xml_location_of_last_good_full_save; // new on 190703
#endif
	  int left_page_number_when_displayed; // new on 180103
#if !TT_ALPHA_FEATURE
	  // as of 081203 this is computed on the fly since each page keeps track of what it needs
	  string media_file_names; // new on 200903 to keep track of what media files this notebook requires
#endif
	  Sprite *page_being_defined; // new on 171203
};

class Page { // new on 181002 since performance hit of having every page be a Sprite is too high sometimes (e.g. Playground)
   public:
      Page() :
			ref_count(1)
#if TT_ALPHA_FEATURE
			,media_file_names(NULL) 
#endif
          {
      };
		virtual ~Page(); // new and virtual on 150103 - so subclasses are called
      virtual Sprite *contents() = 0; // abstract
      virtual void set_in_notebook(Notebook *notebook, boolean recur=TRUE) = 0;
      virtual boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, 
			                  boolean on_nest=FALSE) = 0;
#if TT_XML
		virtual xml_element *top_level_xml(xml_document *document, xml_element *parent, Notebook *notebook) = 0; // updated 200803 and 240105
		virtual boolean instantiated() { // new on 170105
			return(TRUE); // by default
		};
//		virtual xml_element *xml_create_and_append_element(xml_element *parent, xml_document *document) = 0; // new on 220203
//    virtual void xml(xml_element *element, xml_document *document) = 0;
#endif
		void destroy();
      void increment_ref_count() {
         ref_count++;
      };
      void decrement_ref_count() {
         ref_count--;
      };
      boolean is_ref_count_zero() {
         return(ref_count == 0);
      };
		string return_media_file_names() {
			return(media_file_names);
		};
		void set_media_file_names(string file_names) { // shouldn't already have a value
			media_file_names = file_names;
		};
   protected:
      int ref_count; // since pages can be shared
	  string media_file_names; // new on 081203 to keep track of what media files this page requires
};

class SpritePage : public Page { // just holds a Sprite
   public:
      SpritePage(Sprite *sprite) : // sprite should never be NULL
         sprite(sprite),
			XML(NULL) {
//			xml_generated_this_session(FALSE) { // made obsolete on 200903
				sprite->increment_ref_count(); // new on 260103 - not sure it is needed
      };
      ~SpritePage();
      Sprite *contents();
      void set_in_notebook(Notebook *notebook, boolean recur=TRUE) {
         sprite->set_in_notebook(notebook,recur);
      };
      boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
#if TT_XML
		xml_element *top_level_xml(xml_document *document, xml_element *parent, Notebook *notebook);
		boolean instantiated() { // new on 170105
			return(sprite != NULL);
		};
//    xml_element *top_level_xml(xml_document *document);
//		xml_element *xml_create_and_append_element(xml_element *parent, xml_document *document);
      //void xml(xml_element *element, xml_document *document) {
      //   sprite->xml(element,document);
      //};
#endif
   protected:
      Sprite *sprite;
		xml_element *XML; // new on 240103 so it isn't recomputed
//		flag xml_generated_this_session; // new on 210703 so don't regenerate XML repeatedly for time travel -- made obsolete on 200903
};

#if TT_XML
// new on 201002 so don't have to load all contents of notebooks

class XMLPage : public Page { // holds the XML encoding and perhaps also the sprite
   public:
      XMLPage(xml_element *xml);
      ~XMLPage();
      Sprite *contents();
		xml_element *top_level_xml(xml_document *document, xml_element *parent, Notebook *notebook);
//		xml_element *xml_create_and_append_element(xml_element *parent, xml_document *document);
      //void xml(xml_element *element, xml_document *document) {
      //   // nothing more to do
      //};
      void set_in_notebook(Notebook *notebook, boolean recur=TRUE) {
         // nothing to do
      };
      boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
		boolean instantiated() { // new on 170105
			return(sprite != NULL);
		};
   protected:
      Sprite *sprite;
      xml_element *XML;
//		flag xml_generated_this_session; 
		// new on 210703 - much simpler than the following and almost as good (except when saving with media) -- made obsolete on 200903
//		string media_files; // new on 210703 - list (space separated) of short media file names used on this page
};

#endif

class OldFormatPage : public Page { // holds the XML encoding and perhaps also the sprite
   public:
      OldFormatPage(string encoding, int encoding_length, int version_number, NaturalLanguage language);
      ~OldFormatPage();
      Sprite *contents();
#if TT_XML
		xml_element *top_level_xml(xml_document *document, xml_element *parent, Notebook *notebook);
//		xml_element *xml_create_and_append_element(xml_element *parent, xml_document *document);
//      void xml(xml_element *element, xml_document *document);
#endif
      void set_in_notebook(Notebook *notebook, boolean recur=TRUE) {
         // nothing to do
      };
      boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
		boolean instantiated() { // new on 170105
			return(sprite != NULL);
		};
   protected:
      Sprite *sprite;
      string encoding;
      int encoding_length;
      int version_number;
      NaturalLanguage language;
};

class Pages {
  public:
	 Pages(string encoding, long encoding_length, Pages *next_page=NULL,
//          unsigned char version_number=latest_version_number,
          NaturalLanguage language=tt_language) : // was AMERICAN prior to 250799
		encoding(encoding),
		encoding_length(encoding_length),
		next_page(next_page),
//      version_number(version_number),
      language(language) {};
	 ~Pages();
	 Pages *copy(boolean also_copy_offsets=FALSE);
	 short int length();
	 Sprite *nth(short int index, int version_number=latest_version_number,
					 Sprite *container=NULL, Picture *controlee=NULL);
    Page *nth_old_format_page(short int index, int version_number);
	 boolean nth_blank(short int index);
//    short int first_page_with_notebook_matching(Sprite *item);
	 short int first_page_matching_text(string match_text, long match_text_length, int version_number);
	 short int first_page_with_picture(int match_index, SpriteCode match_code, int version_number, string file_name=NULL);
	 Pages *skip_first_n(short int n);
	 int blank_page_number();
	 void encoding_and_length(short int index, 
									  string &result_encoding, 
									  long &result_encoding_length);
	 void encoding_and_length(string &result_encoding, 
									  long &result_encoding_length) {
		 result_encoding = encoding;
		 result_encoding_length = encoding_length;
	 };
	 void set_page(short int index, string encoding, long encoding_length);
	 void replace_encoding_and_length(string new_encoding, long new_encoding_length);
	 void dump(output_stream &stream, int max);
	 void dump_encoding_and_length(output_stream &stream);
	 boolean is_blank() {
		 return(encoding_length == 0);
	 };
    boolean no_pages() {
       return(encoding_length == 0 && next_page == NULL); // new on 220802
    };
    boolean all_pages_blank();
	 Pages *rest_or_add_blank();
	 Pages *rest() {
		 return(next_page);
	 };
	 void set_rest(Pages *new_rest) {
		 next_page = new_rest;
	 };
//    void set_version_number(int number) {
//       version_number = (unsigned char) number;
//    };
    void set_language(NaturalLanguage l) {
       language = l;
    };
	 boolean equal_to(Pages *other);
  private:
	 string encoding;
	 long encoding_length; // in characters -- was short unsigned int prior to 070200 but that was too small for Playgrond notebooks
	 Pages *next_page;
//   unsigned char version_number; 
    NaturalLanguage language;
};

Sprite *sprite_from_encoding(string encoding, int encoding_length, int notebook_version, NaturalLanguage language, Pages *remaining=NULL);

Sprites *load_items(InputStream *pad_in,
                    int notebook_version=latest_version_number, NaturalLanguage language=AMERICAN,
                    Picture *controlee=NULL, boolean associate_robots_and_cubbies=TRUE, Sprite *container=NULL, boolean geometry_too=TRUE, 
						  boolean descendants_inactive=FALSE);
Sprite *load_item(InputStream *pad_in,
                  int notebook_version=latest_version_number, NaturalLanguage language=AMERICAN,
						Sprite *container=NULL, Picture *controlee=NULL,
                  long index=-1,
						boolean is_controlee=FALSE, boolean on_nest=FALSE, boolean descendants_inactive=FALSE);
Sprite *load_item(SpriteType type, InputStream *pad_in, 
						int notebook_version=latest_version_number, NaturalLanguage language=AMERICAN,
						Sprite *container=NULL, Picture *controlee=NULL,
                  long index=-1,
						boolean is_controlee=FALSE, boolean on_nest=FALSE, boolean descendants_inactive=FALSE);
boolean describe_pad(InputStream *stream, output_stream &text_out, int version_number, Article article);
boolean skipped_sound_index(int i);
void release_shared_load_items(boolean regardless=FALSE);
void associate_shared_item(long index, Sprite *item);
Sprite *shared_item(unsigned short int index);
Notebook *load_notebook(InputStream *pad_in,
								int notebook_version, NaturalLanguage language, boolean go_to_page_number,
								Sprite *container, Picture *controlee, long index, boolean descendants_inactive);
void release_java_notebook_pages();

string zip_current_notebook_files();
boolean new_notebook_version(int new_notebook_version);

#endif
