// Copyright (c) 1992-2006.  Ken Kahn, Animated Programs, All rights reserved.
#if !defined(__TT_SPRITE_H)

#define __TT_SPRITE_H

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif
#if !defined(__TT_GLOBALS_H)
#include "globals.h"
#endif   
#if !defined(__TT_REGION_H)   
#include "region.h"
#endif   
#if !defined(__TT_ANIMATE_H)   
#include "animate.h"
#endif   
#if !defined(__TT_BCKGRND_H)   
#include "bckgrnd.h"
#endif  
#if !defined(__TT_CACHE_H)   
#include "cache.h"
#endif

const int GLOBAL_HISTORY = LAST_MANIPULABLE;
const int USER_TOLD = LAST_MANIPULABLE+1; 
const int TOOL_BUTTON_HISTORY = LAST_MANIPULABLE+2;  // added late
const int TROUBLE_SHOOT_HISTORY = LAST_MANIPULABLE+3;

enum GlobalHistory {USER_HAS_PICKED_UP=0,
						  USER_HAS_DROPPED,
						  USER_HAS_APPLIED,
						  USER_HAS_LEFT_FLOOR,
						  USER_HAS_LEFT_THOUGHT_BUBBLE,
						  USER_HAS_HURRIED_UP, // F7 currently
						  USER_HAS_PAUSED,
						  USER_HAS_RECALLED_MARTIAN,
						  USER_HAS_SAT,
						  USER_HAS_TARGETED_HELP,
						  USER_HAS_STOPPED_ALL,
						  USER_HAS_HIDDEN_HAND, // F9 -- no help to suggest its possible?
						  USER_LOOKED_AT_OPTIONS_NOTEBOOK,
                    USER_HAS_TYPED_PAGE_DOWN,
                    USER_HAS_ASKED_FOR_PUZZLE_HINT,
                    USER_HAS_GENERATED_JAVA_APPLET,
						  USER_HAS_DONE_ALL}; // not currently used

enum ToldUserHistory {TOLD_WINDOWS_SHOULD_BE_256_COLORS=0,
							 TOLD_LEAVING_DISPDIB_SHOULD_BE_256_COLORS,
							 TOLD_FULL_SCREEN_PROBLEMS,
							 TOLD_ROBOTS_ARE_FUSSY,
							 TOLD_WHAT_TRUCKS_ACCEPT,
							 TOLD_SCALES_AND_TEXT,
							 TOLD_SCALES_AND_NUMBERS,
							 TOLD_NUMBERS_ON_NUMBERS,
							 TOLD_TEXT_WOULDNT_ACCEPT,
							 TOLD_STACK_OF_NESTS_WONT_ACCEPT,
							 TOLD_BOMBS_WONT_ACCEPT,
							 TOLD_SCALES_ON_PICTURES,
							 TOLD_FLIPPED_PICTURES_ON_PICTURES,
							 TOLD_STUFF_ON_SENSORS,
							 TOLD_BIRD_WONT_ACCEPT,
							 TOLD_MAIN_CUBBY_IN_TRUCK_NO_LOOP,
							 TOLD_FULL_SCREEN_MAY_CAUSE_CRASH,
                      TOLD_REMOTE_NOTEBOOK_READONLY,
                      TOLD_TYPING_SOUND_NAMES_OBSOLETE,
                      TOLD_HOW_TO_GENERATE_JAVA_APPLET};

enum SelectionReason {GRASPED,MIGHT_GRASP,JUMP_TO_GRASP,
							 EXPANDER_USED,MIGHT_USE_EXPANDER,
							 VACUUM_USED,MIGHT_USE_VACUUM,
                      VACUUM_USED_TO_BLANK,
							 COPIER_USED,MIGHT_USE_COPIER,
//							 COPIER_NUMBER_USED,MIGHT_USE_COPIER_NUMBER,
//							 NUMBER_USED,MIGHT_USE_NUMBER,
//							 CUBBY_USED,MIGHT_USE_CUBBY,
							 ITEM_DROPPED,
							 MIGHT_DROP_ITEM,
							 ROBOT_EXTRACTED,
                      VACUUM_USED_TO_SPIT,
							 NO_SELECTION_REASON_GIVEN};

// replace this with Boolean??
//enum SelectionStatus {SELECTION_OK,// SELECTION_OK_AND_RECORDED,
							 //SELECTION_BLANKED, 
//							 SELECTION_REFUSED};

enum ReleaseStatus {RELEASE_IGNORED,
                    RELEASE_ACCEPTED, // but nothing else special
						  RELEASED_IN_CUBBY_HOLE,
						  RELEASED_ITEM_LEFT_OF_ITEM,
						  RELEASED_ITEM_RIGHT_OF_ITEM,						  
						  RELEASED_BODY_CUBBY_LEFT_OF_CUBBY,
						  RELEASED_BODY_CUBBY_RIGHT_OF_CUBBY,
						  RELEASED_ON_NUMBER, RELEASED_ON_TEXT,
						  RELEASED_ON_PICTURE,
						  RELEASED_ROBOT_ON_CUBBY,
						  RELEASED_CUBBY_ON_ROBOT,
						  RELEASED_CUBBY_ON_UNTRAINED_ROBOT,
						  RELEASED_ITEM_TO_BIRD,
						  RELEASED_ITEM_ON_NEST,
						  RELEASED_ROBOT_ON_THOUGHT_BUBBLE,
						  RELEASED_THOUGHT_BUBBLE_ON_ROBOT,
						  RELEASED_ON_ITS_STACK,
						  RELEASED_IN_TRUCK,
						  RELEASED_CUBBY_ON_THOUGHT_BUBBLE,
						  RELEASED_ON_NOTEBOOK,
						  RELEASE_HANDLED};

//enum Relation {DIRECTLY_ON_TOP,TO_THE_LEFT,TO_THE_RIGHT};
					 
// order is used so set_clean_status can only to later enumeration
enum CleanStatus {CLEAN,
						BELOW_DIRTY,
						FOLLOWERS_DIRTY,
						MOVED,
						DIRTY_BUT_OBSCURED,
						DIRTY_PART_OBSCURED,
						DIRTY}; // moved here on 310104 since this is the "most dirty"

enum HeldStatus {NOT_HELD,HELD_BY_PROGRAMMER,HELD_BY_ROBOT}; // declared 2 bits below

enum CacheStatus {NOT_CACHED,CACHED_WITHOUT_FOLLOWERS,CACHED_WITH_FOLLOWERS,
						CACHED_BY_LEADER};

class Screen;
class TTRegion;
//class Floor;
class Background;
class Sprites;
//class Animation;
class Programmer;
class Block;
class Button;
class Text;
class Programmer_At_Floor;

typedef Sprite *SpritePointer;
typedef Sprite **SpritePointerPointer;

enum SpriteCode {//CHAIRSID,TABLESID,
							TOOLBOX_SIDE_SPRITE,TOOLBOX_ABOVE_SPRITE,
							PERSON_WALKING_SPRITE, // MANWALK2,
							WIRE_SPRITE,
							HELIOFLY,
							FRAME_SPRITE,
//							HELIOGON,
							HELIOLND,
							SIT_SPRITE,
//							HELIOLEV,
//							HOUSEABV,HOUSESID,DOORSID,LAWNSID,
							HOUSE_B_TOP_SPRITE,
							HOUSE_B_SIDE_SPRITE,
							DOOR_B_SPRITE,
							DOOR_A_SPRITE,
							NUMBER_PAD_SPRITE, //DIGIT,
							SCALE_SPRITE,
							VACUUM_SPRITE, EXPANDER_SPRITE, COPIER_SPRITE,
							THOUGHT_BUBBLE_SPRITE, ROBOT_SPRITE,
							NEST_SPRITE, BIRD_SPRITE, //PARROT_SPRITE,
//							TRUCK_SPRITE,
							BOMB_SPRITE, 
							NOTEPAD_SPRITE,
							CUBBY_SPRITE,
//							HANDARM,
							MOUSE_SPRITE,
							ARM_ONLY_SPRITE,HAND_SPRITE,HAND_HOLDING_WAND_SPRITE,
                     BIKEPUMP_BUTTON_SPRITE,  // PICTURES_SPRITE,
							HAIR_SPRITE,HAT_SPRITE,
							VACUUM_BUTTON_SPRITE, //  PLATFORM_SPRITE,
                     SWITCH_SPRITE,
							SIT_WITH_HAT_SPRITE,
							SIT_WITH_HAIR_SPRITE,
							HOUSE_A_SIDE_SPRITE,
							HOUSE_A_TOP_SPRITE,
							HOUSE_C_SIDE_SPRITE,
							HOUSE_C_TOP_SPRITE,
							DOOR_C_SPRITE,
							MARTIAN_SPRITE,
							TRUCK_INSIDE_SPRITE,
							TRUCK_ABOVE_SPRITE,
							TRUCK_SIDE_SPRITE,
							TALK_BALLOON_SPRITE,
							WAND_BUTTON_SPRITE, //CHARACTER_PLATE_SPRITE,
							TEXT_PAD_SPRITE,
							NOTEBOOK_PAGE_TURN_SPRITE,
							DIZZY_PERSON_SPRITE,
							DIZZY_PERSON_WITH_HAIR_SPRITE,
							DIZZY_PERSON_WITH_HAT_SPRITE,
							EXPLOSION_SPRITE,
//                   ALIEN_TALK_SPRITE,
							BROKEN_ROCKET_DOOR_SPRITE,
							HIT_OR_MISS_SPRITE,
							BALL_SPRITE,
							PADDLE_SPRITE,
							MANWALK_NORMAL_SPRITE,
							FLOWER_SPRITE,
							TREE_SPRITE,
							TRAFFIC_LIGHT_SPRITE,
                     SYNTHETIC_SPRITE,
                     USER_SPRITE,
                     BROKEN_ROCKET_SPRITE,
                     FRONT_WALL_SPRITE,
                     LEFT_CORNER_SPRITE,
                     RIGHT_CORNER_SPRITE,
                     WALL_SPRITE, // for adding decoration to the wall of the room
                     ROOM_DOOR_SPRITE,
                     BACKGROUND_SPRITE}; // when changing update english.rc too
   
//const int first_sprite_code = TOOLBOX_SIDE_SPRITE;   

//const int appearance_count = NOTEBOOK_PAGE_TURN_SPRITE+1;

//void initialize_head_images();
void deallocate_saved_heads();
//void update_saved_heads(int programmer_head);

city_coordinate maximum_width(SpriteCode code);
city_coordinate maximum_height(SpriteCode code);

city_coordinate compute_sprite_width(city_coordinate width_specified,
												 city_coordinate height_specified,
												 SpriteCode code);
city_coordinate compute_sprite_height(city_coordinate width_specified,
												  city_coordinate height_specified,
												  SpriteCode code);

class Leader_Offsets : public Entity {
  public:
	  Leader_Offsets() : // new on 090304 to initialize these so they end up a good size and near the center
			recorded_leader_width_squared(11068929),
			recorded_leader_height_squared(6071296),
			width_times_leader_width(3267114), 
			height_times_leader_height(2390080),
			x_offset_times_leader_width(4734321), 
			y_offset_times_leader_height(1761760) {
			};
	 void follow_leader_changes(Sprite *follower,
										 city_coordinate leader_width,
										 city_coordinate leader_height,
										 city_coordinate leader_center_x,
										 city_coordinate leader_center_y,
										 ChangeReason reason=LEADER_CHANGED);
	 void follow_leader_size_changes(Sprite *follower,
												city_coordinate leader_width,
												city_coordinate leader_height,
												ChangeReason reason=LEADER_CHANGED);
	 void copy_leader_offsets(Leader_Offsets *other_leader_offsets);
#if TT_XML
	 void xml_geometry(xml_element *element);
	 boolean handle_xml_tag(Tag tag, xml_node *node);
#endif
#if TT_MICROSOFT
	 // changed the following to int64 from long on 260400 since for sizes larger than 40000 it overflows
	 int64 recorded_leader_width_squared, recorded_leader_height_squared; // squared versions introduced on 100200
	 // int64 was long prior to 080200
	 int64 width_times_leader_width, height_times_leader_height,
			 x_offset_times_leader_width, y_offset_times_leader_height;
#else
	 long recorded_leader_width, recorded_leader_height;
	 // int64 not supported by other compilers
	 long width_times_leader_width, height_times_leader_height,
			 x_offset_times_leader_width, y_offset_times_leader_height;
#endif
};			

class AnimationCallBacks : public Entity {
	public:
	  AnimationCallBacks(Sprite *callee,
								AnimationCallBack callback, 
								AnimationCallBacks *next=NULL) :
		  callback(callback),
		  callee(callee),
		  next(next) {};
	  ~AnimationCallBacks() {
		  if (next != NULL) delete next;
	  };
	  void run_first() {
        AnimationCallBackProcedure procedure = callback_procedure(callback);
		  if (procedure != NULL) { // condition new on 040503
			  procedure(callee);
		  };
//		  callback(callee);
	  };
	  AnimationCallBacks *rest() {
		 return(next);
	  };
	  void run();
#if TT_XML
	  void add_xml_each(xml_element *parent, xml_document *document);
	  boolean handle_xml_tag(Tag tag, xml_node *node);
#endif
	private:
	  AnimationCallBack callback;
	  Sprite *callee;
	  AnimationCallBacks *next;
};

class TTImage {
 public:
  TTImage();
//  file_name(NULL)
  virtual ~TTImage();
//  void free_resources();
  void initialize(//string file,
//						boolean mirror,
						dimension w, dimension h, 
						city_coordinate x_off, city_coordinate y_off,
						city_coordinate original_width,
						city_coordinate original_height,
						boolean rectangular_mask,
						long size,
						int compression_method,
						SpriteCode appearance_code,
						int resource_index,
						image_id id);
  void update_display(//int &pixel_width, int &pixel_height,
							 city_coordinate &x_offset, city_coordinate &y_offset);
//							 city_coordinate &original_width,
//							 city_coordinate &original_height);
  virtual boolean display(long &width, long &height, // made virtual on 081001 -- made & on 240102
								  boolean &just_discovered_transparency,
								  bytes permutation,
								  unsigned short int x_scale, unsigned short int y_scale,
								  draw_surface image_surface=NULL);
  void display_on(coordinate x, coordinate y,
						work_page destination, 
						int destination_width, int destination_height,
						bytes permutation);
  draw_surface cached_stretch(unsigned short pixel_width, unsigned short pixel_height); //, bytes color_permutation);
  boolean cache_stretch_if_ok(unsigned short pixel_width, unsigned short pixel_height,
										unsigned short x_scale, unsigned short y_scale,
										draw_surface image_bytes); // bytes color_permutation
  unsigned long candidate_speed_up_reciprocal(); 
  void add_candidate_to_cache(bytes memory, unsigned long size, 
										unsigned short cache_index
#if TT_DIRECTX
                              ,DirectXObject directx_type
#endif
										); // bytes color_permutation
  unsigned short increase_x_scale(unsigned long x_scale, int pixel_growth);
  unsigned short increase_y_scale(unsigned long y_scale, int pixel_growth); 
  void how_to_scale_to(city_coordinate goal_width, city_coordinate goal_height,
							  unsigned short int &x_scale, unsigned short int &y_scale);
  draw_surface load_image(boolean palette_ok); // bytes color_permutation, 
  draw_surface load_image_internal(boolean palette_ok); // used to be virtual but no need currently - removed bytes color_permutation, on 291101
  //virtual boolean set_file_name(ascii_string file_name, boolean warn=TRUE) {
  //  return(TRUE);
  //};
  virtual string just_set_file_name(ascii_string file_name, boolean warn=TRUE) {
    return(file_name);
  };
  virtual ascii_string return_file_name() {
     return(NULL);
  };
  virtual ascii_string return_full_file_name() { // new 081001
     return(NULL);
  };
  virtual ascii_string copy_full_file_name() {
     return(NULL);
  };
  city_coordinate current_x_offset() {
	  return(x_offset);
  };
  city_coordinate current_y_offset() {
	  return(y_offset);
  };
  coordinate current_pixel_width() { // restored on 301002
	  return(pixel_width);
  };
  coordinate current_pixel_height() {
	  return(pixel_height);
  };
#if TT_DEBUG_ON||TT_CAREFUL
  void set_deleted();
#endif
//  void set_rotation(int new_rotation) {
//     rotation = new_rotation;
//  };
 int return_resource_index() {
	 return(resource_index);
 };
 boolean built_in() {
	 return(resource_index >= -1);
 };
 void set_rectangular_mask(boolean new_flag) {
	 rectangular_mask = (flag) new_flag;
 };
 boolean rectangular() {
	 return(rectangular_mask);
 };
// void set_file_name(string new_file_name) {
	 // used for extra person with hat images
//	 if (file_name != NULL) delete [] file_name;
//	 file_name = new_file_name;
// };
 city_coordinate width_without_scaling() {
	 return(original_width);
 };
 city_coordinate height_without_scaling() {
	 return(original_height);
 };
 void clear_private_data() { // called after copying values
   stretch_blit_cache = NULL;
	stretch_blit_cache_size = 0;
 };
 virtual work_page retrieve_image(int &transparent_color, int &bits_per_pixel, boolean &adjust_pitch, draw_surface *mask_surface=NULL);
 void set_dimensions();
#if TT_DEBUG_ON||TT_CAREFUL
	flag deleted : 1; // was just boolean prior to 270200
#endif
 virtual boolean ok_to_check_paths() { // new on 060501
    return(TRUE);
 };
 virtual boolean initialized() { // new on 270901
	 return(TRUE); // only an issue for UserImages
 };
 virtual boolean ok_to_set_dimensions_now() { // new on 081001
	 return(TRUE);
 };
 virtual boolean compute_dimensions(ascii_string initial_file_name, boolean warn, draw_surface image_surface=NULL) {
	 return(TRUE);
 };
 virtual boolean compute_full_file_name() {
	 return(FALSE); // there is no file name to compute
 };
 void process_mask_surface(draw_surface mask_surface, int main_surface_width, int main_surface_height,
									int pixel_width, int pixel_height, bytes surface_bytes, int pitch, int bits_per_pixel);
 hash return_hash() {
	 return(image_hash);
 };
 void set_hash(hash new_hash) {
	 if (image_hash != NULL) delete [] image_hash; // new on 140104 to fix a small leak
	 image_hash = new_hash;
 };
 virtual void just_set_original_file_name(string original_file_name) {
	 // only UserImage cares about this
 };
 //virtual void set_stored_in_private_media_directory(boolean new_flag) {
	// // only UserImage cares about this
 //};
 virtual void reset_full_file_name(string new_name) {
 };
 virtual void set_converted_to_black_as_transparency(boolean new_flag) {
 };
 string save_gif_file(string directory); // new on 210303
 protected:
//  boolean mirror_flag; // whether to displayed "mirrored"
//  city_coordinate collision_width, collision_height;
  int pixel_width, pixel_height;
  city_coordinate x_offset, y_offset; //, collision_x_offset, collision_y_offset;
  city_coordinate original_width, original_height;
//  string file_name;
  long image_size; // can be computed when using .DAT files by substracting offsets
  // now using only 2 so store this in one bit
  // was the BMP values -- BI_RGB, BI_RLE8, BI_RLE4
  // but now is only 0 for RGB and 1 for RLE8
//  int image_compression;
	flag rectangular_mask : 1;
	flag image_compression : 1;
//  int ref_count;
  SpriteCode code;
//#if TT_WINDOWS //&&!GRAPHICS_TO_MEMORY
  int resource_index;
//#if GRAPHICS_TO_MEMORY
//	HGLOBAL bitmap_resource_handle;
//#if !TT_32
//	HGLOBAL memory_handle;
//#endif
//#else
//	HBITMAP bitmap_handle, mask_bitmap_handle;
//#endif
//#else // not windows
//  image_id image_handle;
//#endif
  unsigned short image_cache_entry_index;
  unsigned short image_cache_id; 
  StretchBlitCacheEntryPointer *stretch_blit_cache;
  unsigned short int stretch_blit_cache_size;
  millisecond candidate_when_cached;
  unsigned short candidate_savings_per_use;
  unsigned short candidate_used_count;
  unsigned short candidate_pixel_width;
  unsigned short candidate_pixel_height;
  flag ini_file; // new on 231001 - to handle images defined by INI files -- is this a premature optimization?
  int transparent_color;
  hash image_hash; // new on 170303
};

class UserImage : public TTImage {
  public:
     UserImage();
     ~UserImage();
//     void initialize();
     work_page retrieve_image(int &transparent_color, int &bits_per_pixel, boolean &adjust_pitch, draw_surface *mask_surface=NULL);
     boolean set_file_name(ascii_string initial_file_name, ascii_string original_file_name=NULL, boolean warn=TRUE, 
									int known_pixel_width=-1, int known_pixel_height=-1); // pixel dimensions new on 301002
	  string just_set_file_name(ascii_string initial_file_name, boolean warn=TRUE);
	  void reset_full_file_name(string new_name);
	  void just_set_original_file_name(string original_file_name) {
		  if (file_name != NULL) delete [] file_name; // new on 140104 to fix a small leak
		  file_name = original_file_name;
	  };
     ascii_string return_file_name() {
       return(file_name);
     };
	  ascii_string return_full_file_name() {
		  return(full_file_name);
	  };
	  boolean compute_full_file_name();
	  ascii_string copy_full_file_name();
	  void increment_ref_count() { 
		  ref_count++;
	  };
	  void decrement_ref_count() {
		  ref_count--;
	  };
     int return_ref_count() { 
        return(ref_count);
     };
	  boolean initialized() { // new on 270901
		  return(file_name != NULL); 
	  };
	  boolean dimensions_set() {
		  return(image_size != 0);
	  };
//	  void set_dimensions_set(boolean new_flag) {
//		  dimensions_set_flag = (flag) new_flag;
//	  };
	  boolean display(long &width, long &height, // made virtual on 081001
							boolean &just_discovered_transparency,
							bytes permutation,
							unsigned short int x_scale, unsigned short int y_scale,
							draw_surface image_surface=NULL);
	  boolean compute_dimensions(ascii_string initial_file_name, boolean warn, draw_surface image_surface=NULL);
	  //void set_stored_in_private_media_directory(boolean new_flag) {
		 // stored_in_private_media_directory = (flag) new_flag;
	  //};
	  void set_converted_to_black_as_transparency(boolean new_flag) {
		  converted_to_black_as_transparency = (flag) new_flag;
	  };
//#if TT_DIRECTX_TRANSFORM
//	  draw_surface load_image_internal();
//#endif
  protected:
     ascii_string file_name, full_file_name;
	  int ref_count; // new on 201199
	  flag file_is_BMP : 1; // new on 260500
	  flag converted_to_black_as_transparency : 1; // new on 200303
#if TT_WANT_IMAGE_STATISTICS
	  int display_count;
#endif
//	  flag stored_in_private_media_directory : 1; // new on 180303
//	  flag treat_white_as_black : 1; // new on 121001
//	  flag dimensions_set_flag : 1; // new on 081001
//	  LPBITMAPINFOHEADER saved_dib; // could subclass this for web based images
};

typedef UserImage *UserImagePointer;

class BackgroundImage : public UserImage { 
	// prior to 210999 was subclass of just TTImage - but now can retrieve files instead
  public:
	 BackgroundImage();
	 work_page retrieve_image(int &transparent_color, int &bits_per_pixel, boolean &adjust_pitch, draw_surface *mask_surface=NULL);
	 boolean display();
	 boolean ok_to_check_paths() { // new on 060501
		 return(FALSE);
	 };
	 boolean ok_to_set_dimensions_now() {
		 return(FALSE); // can wait until first time it needs to be displayed (can all userimages do this?)
	 };
//#if TT_DIRECTX_TRANSFORM
//	 draw_surface load_image_internal();
//#endif
  private:
};

class Sprite : public Entity {
  public:
	 Sprite(SpriteCode appearance_code,
			  city_coordinate center_x=0,
			  city_coordinate center_y=0,
//			  Background *floor=NULL,
			  SpriteType col_type=NONE_GIVEN,
			  short int initial_parameter=0,
			  city_scale current_scale=ground_scale,
//             boolean notify_if_colliding=FALSE,
			  city_coordinate width=0,
			  city_coordinate height=0,
			  boolean true_sizes=FALSE);
//			  boolean function_of_lly_flag=TRUE); // needed here??
	 virtual ~Sprite();
	 boolean destroy(Sprite *by=NULL, boolean save=FALSE);
	 virtual void prepare_for_deletion();
	 virtual void release_all_references();
	 virtual void re_initialize(); // made virtual and given a definition on 160204
//	 void delete_followers();
	 virtual void display(SelectionFeedback selection_feedback=NO_FEEDBACK,
								 boolean followers_too=TRUE, TTRegion *region=NULL);
	 //boolean rescale_if_camera_above(SelectionFeedback selection_feedback,
 	//										   boolean followers_too, TTRegion *region);
	 virtual void display_followers(SelectionFeedback selection_feedback=NO_FEEDBACK,
									        TTRegion *region=NULL);
	 void display_as_a_follower(SelectionFeedback selection_feedback, TTRegion *region);
//	 void display_as_a_follower_in_region(SelectionFeedback selection_feedback,
//													  TTRegion *region);
//	 void display_followers_in_region(SelectionFeedback selection_feedback,
//												 TTRegion *region);
	 void display_without_followers(SelectionFeedback selection_feedback=NO_FEEDBACK,
											  TTRegion *region=NULL);
	 void display_without_followers_in_region(SelectionFeedback selection_feedback,
															TTRegion *region);
	 void display_without_followers(TTRegion &region, SelectionFeedback selection_feedback=NO_FEEDBACK);
	 void save_snapshot(string file_name, boolean white);
	 void recursively_display_followers_that_screen_updates();
	 void recursively_set_slated_for_deletion(boolean flag);
	 void move_mark(SelectionFeedback selection_feedback);
	 void move_mark_in_region(SelectionFeedback selection_feedback, TTRegion *region);
	 virtual boolean save_region();
	 void save_region_to_memory(bytes buffer); // should this be virtual too?
	 void set_clip_region(SelectionFeedback selection_feedback=NO_FEEDBACK,
								 TTRegion *region=NULL);
//	 virtual void finalize() {}; // nothing by default
	 virtual void remove_selected_element(Sprite *element,
													  SelectionReason reason, Sprite *by, boolean add_to_floor);
    Sprite *top_level_copy(boolean also_copy_offsets=FALSE, Sprite *original=NULL, Sprite *originals_copy=NULL);
	virtual Sprite *copy(boolean also_copy_offsets=FALSE);
	virtual Sprite *shallow_copy() {
		return(copy());
	};
   void finish_making_copy(Sprite *copy, boolean also_copy_offsets, boolean copy_followers_too=TRUE);
	void set_followers_copy(Sprites *sprites);
	boolean top_level_dump(output_stream &stream);
   virtual boolean dump(output_stream &stream, boolean just_prepare=FALSE, boolean is_controlee=FALSE, boolean on_nest=FALSE);
   void dump_geometry(output_stream &stream, city_coordinate delta_x=0, city_coordinate delta_y=0);
	void dump_just_geometry(output_stream &stream, city_coordinate delta_x=0, city_coordinate delta_y=0);
	void dump_priority(output_stream &stream);
   void load_geometry(InputStream *stream, int version_number, boolean warn=TRUE);
	void load_just_geometry(InputStream *stream, int version_number, boolean warn=TRUE);
	void load_priority(InputStream *stream, boolean warn=TRUE);
	void dump_misc(output_stream &stream);
	void load_misc(InputStream *stream, boolean warn=TRUE);
	void dump_common_flags(output_stream &stream);
	void transfer_geometry_and_misc(Sprite *other);
	virtual ReleaseStatus item_released_on_top(Sprite *item, Sprite *by, SpritePointer &recipient, int level) {
		// do nothing by default
		recipient = this;
		return(RELEASE_IGNORED);
   };
//  void move_back_to_previous_location();
  void update_display(city_coordinate delta_x, city_coordinate delta_y,
							 const UpdateStatus update_status=UPDATE_CHANGE);
//  void update_display(city_coordinate delta_x, city_coordinate delta_y,
//							 const boolean change);
  virtual void update_display(const UpdateStatus update_status=UPDATE_CHANGE) {
	  update_display(0,0,update_status);
  };
  void recursively_update_display();
  void zero_clean_run();
//  void recompute_parameters();
  void clear_changes();
//  void ref_count_is_zero();
//  void display_picture_frame(SelectionFeedback selection_feedback,
//									  city_coordinate &frame_llx,
//									  city_coordinate &frame_lly);
  virtual void recent_speed(city_coordinate &x_speed, 
									 city_coordinate &y_speed) {
	  x_speed = 0;
	  y_speed = 0;
  };								
  void set_scale(city_scale scale) {
	  current_scale = scale;
  };
  void increment_parameter(short int increment);
  // following wasn't virtual prior to 030399 though it was used as if it was by Tools
  virtual boolean set_parameter(short int new_parameter, boolean warn=TRUE,
										  boolean reset_cycle_regardless=FALSE); //, boolean loading=FALSE);
  virtual void just_set_parameter(short int new_parameter) { // new on 290103
     parameter = new_parameter;
  };
  virtual void indirection_of_set_parameter(short int new_parameter, boolean warn=TRUE,
                                            boolean reset_cycle_regardless=FALSE) {
     set_parameter(new_parameter,warn,reset_cycle_regardless);
  };
//  void set_followers_on_top(boolean value) {
//	  followers_on_top_flag = (flag) value;
//  };
  short int current_parameter() {
	  return(parameter);
  };
  short int image_cycle_index() {
     return(current_index);
  };
  void set_scales(unsigned short int new_x_scale, unsigned short int new_y_scale) {
	  x_scale = new_x_scale;
	  y_scale = new_y_scale;
  };
  void reset_cycle();
  void update_cycle_duration();
  void convert_to_synthetic_image();
  boolean scale_to_fit(city_coordinate new_width, city_coordinate new_height);
  void scale_to_fit() {
	  scale_to_fit(width,height);
  };
  city_coordinate width_given_scale(unsigned short int scale);
  city_coordinate height_given_scale(unsigned short int scale);
  city_coordinate height_given_width(city_coordinate width_given) {
	  return(compute_sprite_height(width_given,0,code));
  };
  city_coordinate width_given_height(city_coordinate height_given) {
	  return(compute_sprite_width(0,height_given,code));
  };
  city_coordinate true_llx(boolean including_followers=FALSE);
  city_coordinate true_lly(boolean including_followers=FALSE);
  virtual city_coordinate true_width(boolean including_followers=FALSE);
  virtual city_coordinate true_height(boolean including_followers=FALSE);
  void set_true_size(city_coordinate new_width, city_coordinate new_height,ChangeReason reason=NO_REASON_GIVEN);
  virtual void set_true_size_and_location(city_coordinate new_width, city_coordinate new_height,
														city_coordinate new_llx, city_coordinate new_lly,
														ChangeReason reason=NO_REASON_GIVEN);
//  void original_size(city_coordinate &width, city_coordinate &height) {
//	  width = original_width;
//	  height = original_height;
//  };
  int get_size_percent();
  int get_width_percent();
  int get_height_percent();
  void set_size_percent(int percent, ChangeReason reason=NO_REASON_GIVEN); // reason new on 050803
  void set_size_percents(int x_percent, int y_percent);
  void animate_to_size_percent(int percent, millisecond duration);
  void animate_to_size_percents(int x_percent, int y_percent, millisecond duration);
  void true_region(TTRegion &region, boolean including_followers=FALSE);
//  TTRegion *full_region();
//  void true_size(city_coordinate &true_width, city_coordinate &true_height);
//  void set_size_and_location(city_coordinate new_width,
//									  city_coordinate new_height,
//									  city_coordinate new_llx,
//									  city_coordinate new_lly,
//									  ChangeReason reason=NO_REASON_GIVEN);
//  void scale_to_fit_horizontally(city_coordinate new_width);
//  void scale_to_fit_vertically(city_coordinate new_height);
  void center_location(city_coordinate &x, city_coordinate &y);
  void true_center(city_coordinate &x, city_coordinate &y);
  void set_center_location(city_coordinate center_x, 
									city_coordinate center_y) {
	  move_to(center_x-width/2,center_y-height/2);
  };
  millisecond total_cycle_duration() {
	  return(cycle_duration);
  };
  boolean cycle_stopped();
  boolean cycles();
  boolean cycle_looped_back() {
	  return(looped_back_already);
  };
  millisecond return_cycle_start_time() {
    return(cycle_start_time);
  };
  void set_cycle_start_time(millisecond time) {
	  cycle_start_time = time;
  };
  void set_ideal_speed(long new_speed) {
	  ideal_speed = new_speed;
  };
  int resource_index();
  SpriteCode appearance_code() {
	  return(code);
  };
  void set_appearance_code(SpriteCode new_code);
  void just_set_appearance_code(SpriteCode new_code) { // new on 070103
     code = new_code;
  };
  virtual boolean current_value(NumberValuePointer &the_value, boolean &shared, Sprite *by=NULL) {
//	  the_value = 0;
	  return(FALSE);
  };
  boolean current_long_value(long &value, boolean best_fit=TRUE); // special purpose version of the above (new on 300102)
  boolean current_double_value(double &double_value, boolean best_fit=TRUE);
  boolean is_integer();
  virtual NumberOperation true_operation() {
	  return(NO_NUMBER_OPERATION); // default -- was '=" prior to 100202
  };
  virtual boolean current_text(const_string &the_text, long &the_text_length) {
	  the_text = NULL;
	  the_text_length = 0;
	  return(FALSE);
  };
  virtual boolean current_wide_text(wide_string &the_text, long &the_text_length) {
	  the_text = NULL;
	  the_text_length = 0;
	  return(FALSE);
  };
  virtual void collision_region(TTRegion &region);
  virtual void full_collision_region(TTRegion &region) {
     // this one takes into account movement from last frame
     collision_region(region);
  };
  virtual void generate_help(output_stream &message, Sprite *tool_in_hand, Sprite *selection);
//  virtual void held_help(output_stream &message, Sprite *current_selection);
  virtual boolean history_full();
  boolean did(int action);
  void doing(int action, Sprite *by=NULL);
  virtual Sprites *reachable_sprites_of_type(SpriteType type);
  virtual Sprites *reachable_non_equal_sprites_of_type(SpriteType type);
  virtual string java_start(Cubby *cubby=NULL, Text *text=NULL, Notebook *notebook=NULL,
                            Picture *picture=NULL,
                            Sprites *more_robots_for_picture=NULL, Sprites *more_notebooks=NULL, 
									 Sprite *display_this=NULL, java_string extra_text=NULL) {
		return(NULL);
  };
  virtual void java_class(java_stream &out) {
  };
  virtual void java_before(java_stream &out) {
    // statements before expressions for this
  };
  virtual void java(java_stream &out) {
    // expression
  };
  virtual void java_after(java_stream &out, const_java_string variable) {
    // statements after expressions for this
  };
  virtual void java_flipped_followers(java_stream &out, const_java_string picture_name);
  void java_describe_as_comment(java_stream &out, const_java_string preface=NULL);
  void top_level_describe(output_stream &message, Article article=INDEFINITE_ARTICLE);
  virtual void describe(output_stream &message, Article article=INDEFINITE_ARTICLE);
  virtual void describe_type(output_stream &text_out, Article article=INDEFINITE_ARTICLE,
									  boolean verbose=TRUE, boolean capitalize_first=FALSE);
  virtual void describe_briefly(output_stream &message, Article article=INDEFINITE_ARTICLE, 
										  boolean capitalize_first=FALSE) {
	  describe_type(message,article,TRUE,capitalize_first);
  };
  string return_encoding(long &length, boolean trim, boolean include_version_number, string buffer=NULL);
  virtual boolean to_windows(Pointer *data, short int &data_offset,
                             ascii_string types, short int &types_offset,
                             BirdPointer *birds, short int &bird_count) {
     types[types_offset] = '?'; // token for unknown item
     types_offset++;
     data[data_offset] = this;
     data_offset++;
     return(TRUE);
  };
  //virtual Programmer *pointer_to_programmer() {
	 // // hack since robot and programmer_appearance meet here 
	 // return(NULL);
  //};
  virtual boolean is_programmer_appearance() { // new on 201202 to replace use of pointer_to_programmer()
	  return(FALSE);
  };
  virtual boolean ok_to_delete();
  void set_size_is_true(boolean new_flag) {
	 true_size_flag = (flag) new_flag;
  };
  virtual boolean size_is_true() {
	 return(true_size_flag);
  };
  virtual boolean global() { // as in a global remote
    return(FALSE);
  };
//  void set_transparency_on(boolean new_flag) {
//	 transparency_on_flag = (flag) new_flag;
//  };
  AnimationCallBack pointer_to_cycle_stopped_callback() {
	 return(cycle_stopped_callback);
  };
  //void reset_cycle_stopped_callback() { // noticed obsolete on 010704
	 // cycle_stopped_callback = CALLBACK_NOTHING;
  //};
  void set_cycle_stopped_callback(AnimationCallBack callback,
											 boolean even_if_not_showing=FALSE);
  millisecond animations_stop_time();
  virtual void finish_instantly(boolean callbacks_only=FALSE, boolean ignore_city_stopped=FALSE); // made virtual on 260803
  void check_cycle_callback(boolean run_regardless);
  void run_animations(boolean all_at_once=FALSE, boolean just_callbacks=FALSE, int call_level=0);
  virtual void ignore_collision() {};
//    void avoid_collision(TTRegion *region,
//                         city_coordinate delta_x, city_coordinate delta_y,
//								 city_coordinate &new_delta_x, city_coordinate &new_delta_y,
//								 boolean add_extra=TRUE);
  boolean colliding(Sprite *another);
  boolean overlaps(TTRegion *region);
//    virtual void colliding_with(Sprite *sprite, Screen *screen) {
		// do nothing by default
//	 };
//    void update_collision_region();
//	 void initial_position(city_coordinate x, city_coordinate y);
//    virtual void move_to(city_coordinate x, city_coordinate y,
//                         ChangeReason reason);
	 void move_to(city_coordinate x, city_coordinate y,
					  ChangeReason reason=JUST_MOVED) {
		set_size_and_location(width,height,x,y,reason);
	 };
	 void set_center_x(city_coordinate x) {
		 move_to(x-width/2,lly);
	 };
	 city_coordinate center_x() {
		 return(llx+width/2);
	 };
	 void set_center_y(city_coordinate y) {
		 move_to(llx,y-height/2);
	 };
	 city_coordinate center_y() {
		 return(lly+height/2);
	 };
	 void move_by(city_coordinate delta_x, city_coordinate delta_y) {
		 move_by(delta_x, delta_y, JUST_MOVED);
	 };
//    void recursively_move_by(city_coordinate delta_x, city_coordinate delta_y);
	 void move_by(city_coordinate delta_x, city_coordinate delta_y,
					  ChangeReason reason) {
		 if (delta_x != 0 || delta_y != 0) {
			 move_to(llx+delta_x,lly+delta_y,reason);
		 };
	 };
	 virtual void shift_viewpoint(city_coordinate delta_x, city_coordinate delta_y); // new on 020204 -- i.e. scrolling
	 void increase_size_by(city_coordinate delta_x, city_coordinate delta_y) {
		 if (delta_x != 0 || delta_y != 0) {
			 set_size(width+delta_x,height+delta_y);
		 };
	 };
	 void set_width(city_coordinate new_width, ChangeReason reason=NO_REASON_GIVEN) { // added reason on 050803
		 set_size(new_width,height,reason);
	 };
    void just_set_width(city_coordinate new_width) { // new on 131102
		 width = new_width;
	 };
    void just_set_height(city_coordinate new_height) { // new on 131102
		 height = new_height;
	 };
	 void set_width_maintain_proportionality(city_coordinate new_width);
	 void animate_width_maintain_proportionality(city_coordinate new_width, millisecond duration);
	 void set_height(city_coordinate new_height, ChangeReason reason=NO_REASON_GIVEN) { // added reason on 050803
		 set_size(width,new_height,reason);
	 };
	 void set_height_maintain_proportionality(city_coordinate new_height);
	 void animate_height_maintain_proportionality(city_coordinate new_height, millisecond duration);
	 virtual void signal_size_and_location();
	 virtual void signal_good_size() {
		 // do nothing by default
	 };
	 void set_full_size_and_location(city_coordinate goal_width,
												city_coordinate goal_height,
												city_coordinate new_llx,
												city_coordinate new_lly);
	 virtual void set_size_and_location(city_coordinate width,
												   city_coordinate height,
												   city_coordinate llx,
												   city_coordinate lly,
												   ChangeReason reason=NO_REASON_GIVEN);
//   void set_size_and_location(city_coordinate width,
//                               city_coordinate height,
//                               city_coordinate llx,
//                               city_coordinate lly) {
//       set_size_and_location(width,height,llx,lly,NO_REASON_GIVEN);
//	 };
	 void just_move_by(city_coordinate delta_x,city_coordinate delta_y) {
		 set_size_and_location(width,height,llx+delta_x,lly+delta_y,
									  SELECTION_HIGHLIGHT);
	 };
	 void set_size(city_coordinate new_width, city_coordinate new_height) {
       set_size_and_location(new_width,new_height,llx,lly);
    };
	 void set_size(city_coordinate new_width, city_coordinate new_height,
						ChangeReason reason) {
		 set_size_and_location(new_width,new_height,llx,lly,reason);
	 };
	 void set_size_stay_centered(city_coordinate new_width, city_coordinate new_height,
										  ChangeReason reason=NO_REASON_GIVEN);
	 void set_llx(city_coordinate new_llx, ChangeReason reason=NO_REASON_GIVEN) { // reason new on 050803
		 set_size_and_location(width,height,new_llx,lly,reason);
	 };
	 void set_lly(city_coordinate new_lly, ChangeReason reason=NO_REASON_GIVEN) {
		 set_size_and_location(width,height,llx,new_lly,reason);
	 };
//    void just_set_size_and_location(city_coordinate new_width,
//										      city_coordinate new_height,
//										      city_coordinate new_llx,
//										      city_coordinate new_lly){
//    	llx = new_llx;
//	   lly = new_lly;
//	   width = new_width;
//	   height = new_height;
//    };
//	 void changed_so_update();
	 virtual void update_priority(); // made virtual on 211004 to deal with robots and their boxes while standing
	 Sprite *ultimate_leader();
	 Sprite *immediate_container();
    Sprite *ultimate_container();
	 Sprite *ultimate_top_level_container();
	 Sprite *ultimate_flipside_picture();
	 Sprite *ultimate_unflipped_picture();
	 boolean common_picture_container(Sprite *other);
	 virtual boolean size_constrained(); // renamed and made virtual on 180804
	 virtual void controlee_deleted() {};
//	 TTRegion *near_free_region(Screen *screen);
	 void follow_leader_changes(city_coordinate leader_delta_width,
                               city_coordinate leader_delta_height,
                               city_coordinate delta_llx,
                               city_coordinate delta_lly,
										 ChangeReason reason=LEADER_CHANGED); // was implicit prior to 051100
	 void follow_leader_size_changes(city_coordinate leader_width, city_coordinate leader_height, 
												ChangeReason reason=NO_REASON_GIVEN);
	 void propagate_changes(ChangeReason reason=NO_REASON_GIVEN);
	 virtual void really_propagate_changes(ChangeReason reason=NO_REASON_GIVEN);
//	 void really_propagate_changes_to_follower(Sprite *follower, ChangeReason reason=NO_REASON_GIVEN);
	 virtual void recursively_propagate_changes(ChangeReason reason=NO_REASON_GIVEN);
	 virtual void add_follower(Sprite *follower, boolean completely_inside=TRUE, 
										InsertionOrdering insertion_ordering=INSERT_AT_END, 
										boolean dont_record_new_offsets=FALSE, AddFollowerReason reason=NO_ADD_FOLLOWER_REASON_GIVEN);
	 virtual void add_outside_followers_to_screen(); 
	 // made virtual on 170101 so things like robots can fix the location of their name, etc.
	 void record_offsets();
//    void record_size_offsets();
	 void record_offsets(city_coordinate leader_llx,
								city_coordinate leader_lly,
								city_coordinate leader_width,
								city_coordinate leader_height);
	virtual void follower_changed(Sprite *follower) {
		follower->record_offsets(llx,lly,width,height);
	};
	virtual boolean remove_follower(Sprite *sprite, boolean warn_if_not_follower=TRUE); 
	// changed on 191202 to return TRUE if successful
   void remove_all_followers(boolean destroy_too=FALSE);
	//virtual void release_outgoing_connections() { // new on 311003 - so it doesn't remember its followers or the like
	//	remove_all_followers(TRUE);
	//};
//	void recursively_remove_leaders(Background *background);
//	void recursively_restore_leaders(Background *background);
	void compute_dimensions_to_fit(city_coordinate goal_width,
											 city_coordinate goal_height,
											 city_coordinate &full_width,
											 city_coordinate &full_height);
	boolean full_region_including_outside_followers(TTRegion &region);
	boolean full_size_and_location(city_coordinate &full_width,
											 city_coordinate &full_height,
											 city_coordinate &full_llx,
											 city_coordinate &full_lly);
	boolean true_size_and_location(city_coordinate &full_width,
											 city_coordinate &full_height,
											 city_coordinate &full_llx,
											 city_coordinate &full_lly);
//	 void done_with_previous_values() {
//		 previous_llx = llx;
//       previous_lly = lly;
//		 previous_width = width;
//		 previous_height = height;
//	 };
    void insert_animation(Animation *new_animation);
    void remove_animations_of(Sprite *sprite);
//	 void set_after(millisecond duration, long *variable, long value);
//	 void set_after(millisecond duration, ActionStatus *variable, ActionStatus value) {
		 // how safe is this really??
//		 set_after(duration,(long *) variable,(long) value);
//	 };
    //virtual void action_completed() {
    //   // do nothing by default - new on 131102
    //};
//    void increment_after(millisecond duration, long *variable, long increment);
//	 void true_after(millisecond duration, flag *status);
	 void do_after(millisecond duration, Sprite *notify, AnimationCallBack callback);
	 void animate_size_and_location(city_coordinate width,
											  city_coordinate height,
											  city_coordinate llx,
											  city_coordinate lly,
											  millisecond expansion_duration,
//											  boolean *status=NULL,
											  Sprite *notify=NULL,
											  AnimationCallBack callback=CALLBACK_NOTHING,
											  boolean signal_change=TRUE);
	 void animate_true_size_and_location(city_coordinate new_width, city_coordinate new_height,
												    city_coordinate new_llx, city_coordinate new_lly,
												    millisecond duration,
													 Sprite *notify=NULL,
													 AnimationCallBack callback=CALLBACK_NOTHING,
													 boolean signal_change=TRUE); 
	 void set_size_and_location_to_region(TTRegion &region) {
		 set_size_and_location(region.max_x-region.min_x,
									  region.max_y-region.min_y,
									  region.min_x,region.min_y);
	 };
    void set_size_and_location_to_region_maintain_proportionality(TTRegion &region) {
		 set_size_maintain_proportionality(region.max_x-region.min_x,
									              region.max_y-region.min_y);
       move_to(region.min_x,region.min_y);
	 };
    void set_size_maintain_proportionality(city_coordinate new_width,
									                city_coordinate new_height);
    void set_size_and_location_to_those_of(Sprite *other, ChangeReason=NO_REASON_GIVEN);
    void set_true_size_and_location_to_those_of(Sprite *other, ChangeReason=NO_REASON_GIVEN);
	 void set_size_and_location_to_true_values_of(Sprite *other, ChangeReason reason=NO_REASON_GIVEN);
	 void animate_size_and_location(TTRegion *region,
											  millisecond expansion_duration,
//											  boolean *status=NULL,
											  Sprite *notify=NULL,
											  AnimationCallBack callback=CALLBACK_NOTHING,
											  boolean signal_change=TRUE) {
		 animate_size_and_location(region->max_x-region->min_x,
											region->max_y-region->min_y,
											region->min_x, region->min_y,
											expansion_duration,// status,
											notify,callback,
											signal_change);
	 };
	 void animate_location(city_coordinate new_llx, // was virtual prior to 131102
								 city_coordinate new_lly,
								 millisecond expansion_duration,
								 Sprite *notify=NULL,
								 AnimationCallBack callback=CALLBACK_NOTHING,
								 boolean signal_change=TRUE);
//	 void animate_location(city_coordinate new_llx,
//								  city_coordinate new_lly,
//								  millisecond expansion_duration) {
//		 animate_location(new_llx,new_lly,expansion_duration,NULL,NULL);
//	 };
	 void animate_location_to(Sprite *other,
								     millisecond expansion_duration,
								     Sprite *notify=NULL,
								     AnimationCallBack callback=CALLBACK_NOTHING,
								     city_coordinate x_offset=0,
								     city_coordinate y_offset=0,
								     boolean signal_change=TRUE);
	 void animate_delta_location(city_coordinate delta_x,
										  city_coordinate delta_y,
										  millisecond duration,
//										  boolean *status=NULL,
										  Sprite *notify=NULL,
										  AnimationCallBack callback=CALLBACK_NOTHING,
										  boolean signal_change=TRUE) {
		 animate_location(llx+delta_x,lly+delta_y,duration,notify,callback,signal_change);
	 };
	 virtual void animate_size(city_coordinate new_width, 
							         city_coordinate new_height,
							         millisecond expansion_duration,
							         Sprite *notify=NULL,
							         AnimationCallBack callback=CALLBACK_NOTHING,
							         boolean signal_change=TRUE);
	 //virtual void animate_size(city_coordinate *new_width,
		//								 city_coordinate *new_height,
		//								 millisecond expansion_duration,
		//								 Sprite *notify=NULL,
		//								 AnimationCallBack callback=CALLBACK_NOTHING,
		//								 boolean signal_change=TRUE);
//	 void animate_size(city_coordinate new_width,
//							 city_coordinate new_height,
//							 millisecond duration) {
//		 animate_size(new_width,new_height,duration,NULL,NULL);
//	 };
	 void add_ease_in_and_out_to_goal(AnimatingVariable variable,
                                     Sprite *other_sprite,
                                     AnimatingVariable other_variable,
												 millisecond the_duration,
												 Sprite *notify=NULL,
												 AnimationCallBack callback=CALLBACK_NOTHING,
												 long offset=0);
	 void add_ease_in_and_out_to_goal(AnimatingVariable variable, long the_goal,
												 millisecond the_duration,
												 Sprite *notify=NULL,
												 AnimationCallBack callback=CALLBACK_NOTHING);
	 void stop_animations();
	 boolean animation_in_progress() {
		 return(animations != NULL);
	 };
	 boolean animation_or_callbacks_in_progress() { // new on 130504
		 return(animations != NULL || cycle_stopped_callback != CALLBACK_NOTHING);
	 };
	 //AnimationStatus animation_status() {
		// if (animation_status_flag) {
		//	 return(JUST_FINISHED);
		// } else if (animations == NULL) {
		//	 return(NONE_IN_PROGRESS);
		// } else {
		//	return(IN_PROGRESS);
  //     };
	 //};
	 void respond_to_stop_character();
	 virtual Sprite *respond_to_keyboard(unsigned char key, boolean extended, boolean robot_selected, Sprite *by,
												    boolean record_action,
													 Sprite *original_recipient, // new on 080604
													 boolean size_constrained_regardless=FALSE); // new on 281002								 
	 virtual void adjust_size_and_location(city_coordinate &item_width, city_coordinate &item_height,
                                          city_coordinate &item_llx, city_coordinate &item_lly) {
	 }; // ok by default
	 city_coordinate current_width() {
       return(width);
	 };
	 city_coordinate current_height() {
		 return(height);
	 };
	 virtual void notice_on_screen_change(boolean on_screen) {
	 }; // do nothing by default
    virtual void refit_contents() {};
    virtual void recursively_notice_on_screen_change(boolean on_screen);
//	 void pixel_dimensions(int &width, int &height) {
//		 width = pixel_width;
//       height = pixel_height;
//	 };
//	 virtual void increment_screen_ref_count() {
//		 screen_ref_count = TRUE;
//	 };
//	 virtual void decrement_screen_ref_count() {
//		 screen_ref_count = FALSE;
//	 };
//	 boolean current_screen_ref_count() {
		 // can be generalized to non-boolean ints later if needed
//		 return(screen_ref_count);
//	 };
//	 void set_screen_ref_count(boolean new_ref_count) {
//		 screen_ref_count = new_ref_count;
//	 };
//	 void recursively_increment_screen_ref_count();
//	 void recursively_decrement_screen_ref_count();
//	 void recursively_set_screen_ref_count(boolean new_ref_count);
	 boolean showing_on_a_screen();
    Block *block_im_on();
	 virtual boolean contains(Sprite *other) {
		 return(other == this);
	 };
//    boolean is_screen_ref_count_zero() {
//      return(screen_ref_count == 0);
//    };
	 virtual boolean ok_to_cache();
    void set_never_cache(boolean new_flag) { // updated on 020302
      never_cache_flag = (flag) new_flag;
    };
	 virtual boolean ok_to_cache_without_followers() {
		 return(!blank); // was TRUE prior to 191200
	 };
//	 virtual boolean ok_to_save_in_include_file(); // major upgrade on 190703 since really don't want small things or changing things
	 // new on 120703 to stop birds, remotes, and notebooks of remotes to save this way since they refer to others
	 boolean completely_clean() {
		 return(clean_status == CLEAN);
	 };
	 boolean clean() {
		 return(clean_status == CLEAN  || 
				  clean_status == FOLLOWERS_DIRTY);
	 };
	 boolean clean_or_moved() {
		 return(clean_status == CLEAN  || 
				  clean_status == MOVED);
	 };
	 boolean clean_or_obscured() {
		 return(clean_status == CLEAN || 
				  clean_status == FOLLOWERS_DIRTY ||
				  clean_status == DIRTY_BUT_OBSCURED ||
				  clean_status == DIRTY_PART_OBSCURED);
	 };
	 void reset_clean_status();
	 CleanStatus current_clean_status() {
		 return(clean_status);
	 };
	 void just_set_clean_status(CleanStatus new_status) {
		 clean_status = new_status;
	 };
//	 void set_previous_change_reason(ChangeReason reason) {
//		 previous_change_reason = reason;
//	 };
//	 ChangeReason return_previous_change_reason() {
//		 return(previous_change_reason);
//	 };
#if TT_DEBUG_ON
	const_string current_clean_status_name();
   virtual void check_integrity() {};
#endif
	 virtual void set_clean_status(CleanStatus new_status, boolean recur=TRUE);
    void set_clean_status_of_outside_followers(CleanStatus new_status);
	 long priority() {
		return(current_priority);
	 };
//	 void set_display_updated(boolean new_flag) {
//		 display_updated = (flag) new_flag;
//	 };
	 virtual void set_priority(long priority);
    void just_set_priority(long priority) {
       current_priority = priority;
    };
	 void just_set_floor_priority(long priority) {
       floor_priority = priority;
    };
	 long return_floor_priority() {
		 return(floor_priority);
	 };
	 void save_floor_priority() {
		 floor_priority = current_priority;
	 };
	 void restore_floor_priority();
    boolean priority_fixed() {
      return(priority_fixed_flag);
    };
    void set_priority_fixed(boolean new_flag) {
       priority_fixed_flag = (flag) new_flag;
    };
	 void increment_priority(long change) {
		 set_priority(current_priority+change);
	 };
//	 void set_priority_function_of_lly(boolean new_flag) {
//		 priority_function_of_lly_flag = (flag) new_flag;
//	 };
//    boolean priority_function_of_lly() {
//       return(priority_function_of_lly_flag);
//    };
	 void full_region(TTRegion &region);
	 void dirty_region(TTRegion &region);
//	 virtual void collision_region(TTRegion &region) {
//		// virtual so things like occupied nests can forward to occupier
//		full_region(region);
//	 };
	 virtual void pointer_region(TTRegion &region) {
		 full_region(region); // probably never called generically
	 };
	 virtual void active_region(TTRegion &region);
//	 void collision_region_center(city_coordinate &center_x,
//											city_coordinate &center_y) {
//		 pointer_to_collision_region()->region_center(center_x,center_y);
//	 };
//	 void collision_region_size(city_coordinate &width,
//										 city_coordinate &height) {
//		 pointer_to_collision_region()->region_size(width,height);
//	 };
	 virtual SpriteType fine_kind_of() {
		 return(sprite_type);
	 };
    virtual int history_index() {
       return(kind_of());
    };
	 SpriteType kind_of() {
		 return(sprite_type);
	 };
	 SpriteType entity_type() {
		 return(sprite_type);
	 };
	 virtual boolean looks_like_is_of_type(SpriteType type) { // new on 210902
		 return(sprite_type == type);
	 };
	 void set_sprite_type(SpriteType type) {
		 sprite_type = type;
	 };
//	 void set_motion_extends_collision_region(boolean new_flag) {
//		 motion_extends_collision_region = (flag) new_flag;
//	 };
//	 void set_collision_region_equals_region(boolean new_flag) {
//		 collision_region_equals_region = (flag) new_flag;
//	 };
	 void lower_left_corner(city_coordinate &x, city_coordinate &y) {
		 // returns the current true corner of current image
		 x = llx;
		 y = lly;
	 };
	 city_coordinate current_llx() {
		 return(llx);
	 };
	 city_coordinate current_lly() {
		 return(lly);
	 };
	 city_coordinate current_urx() {
		 return(llx+width);
	 };
	 city_coordinate current_ury() {
		 return(lly+height);
	 };
	 city_coordinate true_urx() {
		 return(llx+true_width());
	 };
	 city_coordinate true_ury() {
		 return(lly+true_height());
	 };
	 void just_set_llx(city_coordinate new_llx) {
		 llx = new_llx;
	 };
	 void just_set_lly(city_coordinate new_lly) {
		 lly = new_lly;
	 };
    virtual void just_apply_delta(city_coordinate delta_x, city_coordinate delta_y) { // new on 051102 for dumping XML 
       llx += delta_x;
       lly += delta_y;
    };
//	 void just_set_width(city_coordinate new_width) {
//		 width = new_width;
//	 };
//	 void just_set_height(city_coordinate new_height) {
//		 height = new_height;
//	 };
    virtual long current_size_percent() { // new on 131102
        return(100); // shouldn't happen, right?
     };
    virtual void just_set_size_percent(long new_percent) {
    };
	 //long width_percentage_of_true_width(boolean including_followers=FALSE);
	 //long height_percentage_of_true_height(boolean including_followers=FALSE);
	 double width_fraction_of_true_width(boolean including_followers=FALSE);
	 double height_fraction_of_true_height(boolean including_followers=FALSE);
	 virtual city_coordinate previous_llx() {
		 return(llx); // doesn't move by default
	 };
	 virtual city_coordinate previous_urx() {
		 return(llx+width);
	 };
	 virtual city_coordinate previous_lly() {
		 return(lly);
	 };
	 virtual city_coordinate previous_ury() {
		 return(lly+height);
	 };
	 city_coordinate true_x_offset();
	 city_coordinate true_y_offset();
	 void set_old_x_offset(city_coordinate x) {
#if TT_DEBUG_ON
		if (debug_counter == tt_debug_target) {
			tt_error_file() << "Debug target setting old_x_offset." << endl;
		};
#endif
		old_x_offset = x;
	 };
	 void set_old_y_offset(city_coordinate y) {
		 old_y_offset = y;
	 };
	 city_coordinate return_old_x_offset() { // used currently for accurate dumping
		 return(old_x_offset);
	 };
	 city_coordinate return_old_y_offset() {
		 return(old_y_offset);
	 };
// obsolete and should fix old offsets by scale
//	 city_coordinate stable_llx() {
		 // this doesn't change as images are cycled thru
//		 return(llx-old_x_offset);
//	 };
//	 city_coordinate stable_lly() {
//		 return(lly-old_y_offset);
//	 };
	 void center(city_coordinate &x, city_coordinate &y) {
		 x = llx+width/2;
		 y = lly+height/2;
	 };
	 void pointers_to_lower_left_corner(city_coordinate *&x,
													city_coordinate *&y) {
		 x = &llx;
		 y = &lly;
	 };
	 virtual void set_leader(Sprite *the_leader, boolean temporarily=FALSE); // made virtual on 040205
	 Sprite *pointer_to_leader() {
		 return(leader);
	 };
	 boolean has_leader(Sprite *sprite);
	 void set_changes_propagated(boolean new_flag) {
#if TT_DEBUG_ON
		 if (tt_debug_target == debug_counter) {
			 tt_error_file() << "Setting changes propagated flag of debug target to " << (int) new_flag << endl;
		 };
#endif
		 changes_propagated_flag = (flag) new_flag;
	 };
	 boolean changes_propagated() {
		 return(changes_propagated_flag);
	 };
	 boolean contained_in(SpriteType type);
	 boolean contained_in_robots_thought_bubble();
	 boolean contained_in_thought_bubbles_cubby();
	 boolean part_of_top_level_copy();
    boolean part_of(Sprite *other);
	 Sprites *pointer_to_followers() {
		 return(followers);
	 };
//	 void set_followers(Sprites *sprites) {
//		 followers = sprites;
//	 };
	 virtual boolean has_followers() { // added on 110200 since remote looks has no followers of its own but its controlee might
		 return(followers != NULL);
	 };
    virtual void forget_tool_dimensions() {
    };
//	 void recursively_save_size();
	 virtual void save_size() {}; // nothing by default
//	 void unsave_size() {
//		 saved_width = 0;
//	 };
//	 void set_saved_size(city_coordinate w, city_coordinate h) {
//		 saved_width = w;
//		 saved_height = h;
//	 };
	 virtual void good_size(city_coordinate &w, city_coordinate &h, Sprite *source=NULL, boolean for_expander=FALSE);
	 void adjust_good_size(city_coordinate &w, city_coordinate &h, Sprite *source, boolean adjust_to_screen_size=FALSE);
    void set_to_good_size(Sprite *source=NULL);
    void adjust_size_for_source(city_coordinate &w, city_coordinate &h, Sprite *source);
    void animate_to_good_size(millisecond duration, Sprite *source=NULL);
    void current_percent_of_good_size(long &x_percent, long &y_percent);
	 city_coordinate good_width(Sprite *source=NULL) {
		 city_coordinate good_w,good_h;
		 good_size(good_w,good_h,source);		 
		 return(good_w);
	 };
	 city_coordinate good_height(Sprite *source=NULL) {
		 city_coordinate good_w,good_h;
		 good_size(good_w,good_h,source);		 
		 return(good_h);
	 };
//	 city_coordinate return_good_width() {
		 // unlike saved_size doesn't reset while reading
//		 return(saved_width?saved_width:width);
//	 };
//	 city_coordinate return_good_height() {
//		 return(saved_height?saved_height:height);
//	 };
//	 void set_size_to_saved_size() {
//		 if (saved_width != 0) {
//			 width = saved_width;
//			 height = saved_height;
//			 saved_width = 0;
//		 };
//	 };
    virtual void dramatic_entrance(millisecond duration) {
    }; // do nothing by default
    virtual void favorite_size(city_coordinate &width, city_coordinate &height) {
        good_size(width,height);
    };
    virtual void favorite_location(city_coordinate &x, city_coordinate &y) {
        x = 0; // error? warn?
        y = 0;
     };
    virtual void set_favorite_size_and_location() {
        // nothing by default
     };
     virtual void current_favorite(FavoriteSizeAndLocation &f) {
     };
     virtual void set_favorite(FavoriteSizeAndLocation &f) {
       // nothing?  error??
     };
//     virtual void locomote_to_favorite_location_and_size();
	  void set_to_favorite_size() { // new on 070704
		  city_coordinate fav_width, fav_height;
        favorite_size(fav_width,fav_height);
        set_size(fav_width,fav_height);
	  };
     void animate_to_favorite_size(millisecond duration) {
        city_coordinate fav_width, fav_height;
        favorite_size(fav_width,fav_height);
        animate_size(fav_width,fav_height,duration);
     };
     void animate_to_favorite_location(millisecond duration) {
        city_coordinate fav_x, fav_y;
        favorite_location(fav_x,fav_y);
        animate_location(fav_x,fav_y,duration);
     };
	 Background *pointer_to_background() {
		return(floor);
	 };
//	 virtual void recursively_set_background(Background *new_background);
	 virtual void set_background(Background *new_background, boolean recur=FALSE, boolean warn=TRUE);
	 void no_background() {
		 // low level call that should not trigger remotes, etc.
		 floor = NULL;
	 };
    virtual PlateState plate_state() {
       return(NUMBER_PAD_WITH_PLATE); // doesn't matter since should never reach here
    };
	 boolean screen_updates() {
		return(screen_updates_flag);
	 };
	 void set_screen_updates(boolean new_flag) {
		 screen_updates_flag = (flag) new_flag;
	 };
	 boolean inside_vacuum();
	 boolean inside_toolbox_vacuum();
//    boolean *pointer_to_inside_vacuum_flag() {
//       return(&inside_vacuum_flag);
//    };
	 virtual void set_inside_vacuum(boolean new_flag, boolean recur=TRUE);
	 boolean infinite_stack() {
		return(infinite_stack_flag);
	 };
	 void set_infinite_stack(boolean new_flag) {
		 infinite_stack_flag = (flag) new_flag;  
	 };
	 void set_available_for_indexing(boolean new_flag) {
#if TT_DEBUG_ON
		 if (debug_counter == tt_debug_target) {
			 tt_error_file() << "Setting available_for_indexing of debug target." << endl;
		 };
#endif
		 available_for_indexing_flag = (flag) new_flag;
	 };
	 boolean available_for_indexing() {
		 return(available_for_indexing_flag);
	 };
	 virtual void set_slated_for_deletion(boolean new_flag) {
#if TT_DEBUG_ON
		 if (debug_counter == tt_debug_target) {
			 tt_error_file() << "Setting slated_for_deletion of debug target." << endl;
		 };
#endif
		 slated_for_deletion_flag = (flag) new_flag;
	 };
	 boolean slated_for_deletion() {
		 return(slated_for_deletion_flag);
	 };
	 boolean prepared_for_deletion() {
		 return(prepared_for_deletion_flag);
	 };
	 void set_graspable(boolean new_flag) {
		 graspable_flag = (flag) new_flag;
	 };
	 boolean graspable() {
		 return(graspable_flag);
	 };
    void set_selectable(boolean new_flag) {
		 selectable_flag = (flag) new_flag;
	 };
	 boolean selectable() {
		 return(selectable_flag);
	 };
	 virtual boolean is_tool() {
		  return(FALSE); // not wand, vacuum or pump
	 };
    boolean is_container() {
       return(kind_of() == CUBBY || kind_of() == TRUCK || kind_of() == PICTURE || kind_of() == PROGRAM_PAD);
    };
    virtual boolean is_on() {
       return(FALSE); // tools over-ride this
    };
    virtual boolean is_still_on() {
       return(FALSE); // tools over-ride this
    };
    virtual void turn_on(Sprite *by) {
    };
    virtual void turn_off(Sprite *by) {
    };
//    virtual millisecond decode_duration(char encoding) {
//       return(0);
//    };
    virtual boolean used_since_pick_up() {
      return(TRUE);
    };
    virtual void reset_used_since_pick_up() {
    };
	 boolean followers_completely_inside() {
		return(followers_completely_inside_flag);
	 };
	 void set_followers_completely_inside(boolean new_flag) {
		followers_completely_inside_flag = (flag) new_flag;
	 };
	 void set_completely_inside_leader(boolean new_flag) {
		 completely_inside_leader_flag = (flag) new_flag;
	 };
	 boolean completely_inside_leader() {
		 return(completely_inside_leader_flag);
	 };
	 void set_followers_absorbed(flag new_flag) {
		 followers_absorbed_flag = (flag) new_flag;
	 };
	 boolean followers_absorbed() {
		 return(followers_absorbed_flag);
	 };
//	 void set_leaders_removed(flag new_flag) {
//		 leaders_removed_flag = (flag) new_flag;
//	 };
//	 boolean leaders_removed() {
//		 return(leaders_removed_flag);
//	 };
//	 boolean is_game_element() {
//		return(game_element);
//	 };
//	 void set_game_element(boolean new_flag) {
//		 game_element = (flag) new_flag;
//	 };
//	 virtual city_coordinate full_width();
//	 virtual city_coordinate full_height();
	 virtual boolean rectangular();
    boolean is_rectangle() {
	   return(code == SYNTHETIC_SPRITE && current_parameter()/256 == SYNTHETIC_RECTANGLE);
    };
	 virtual Sprite *used(Sprite *subject,
								 Sprite *by,
								 boolean button_unchanged,
								 boolean record_action,
								 millisecond duration,
							    boolean modified,
								 Sprite *original_subject) {
		  return(this);
	 };
	 virtual Sprite *select(TTRegion *region, SelectionReason reason, 
	                        Sprite *in_hand);
	 virtual void unselect() {
//       selected_flag = FALSE;
    };
	 Sprite *copy_top_of_stack(Sprite *by, SelectionReason reason=ITEM_DROPPED);
	 Sprite *top_of_stack_or_create(Sprite *by, SelectionReason reason=NO_SELECTION_REASON_GIVEN);
	 Sprite *remove_top_of_stack();
	 Sprite *pointer_to_top_of_stack() {
		 return(top_of_stack);
	 };
	 virtual Sprite *selected(SelectionReason reason, Sprite *by);
	 virtual Sprite *released(Sprite *by, boolean top_level=TRUE, SpecialDrop special_drop=NOT_SPECIAL_DROP);
	 virtual boolean receive_item(Sprite *item, Sprite *by, millisecond duration, Sprite *original_recipient, Sprite *original_item);
	 virtual ReleaseStatus add_to_side(boolean right_side, Sprite *other, 
												  Sprite *by, 
												  millisecond duration, Sprite *original_recipient, Sprite *original_item);
	 boolean inside_a_house();
	 virtual boolean vacuum_if_left_on_floor(SpritePointer *others_being_vaccuumed, int others_count);
	 virtual void inside_thought_bubble() {
		 // by default ignore the fact
	 };	
	 //virtual void inside_cubby_hole() { // noticed obsolete on 180103
		//// nothing by default
	 //};
	 virtual boolean is_busy() {
		 return(FALSE);
	 };
	 virtual void become_blank(boolean by_user=FALSE, boolean controlee_too=TRUE) {
		 set_clean_status(DIRTY);
		 blank = TRUE;
	 };
	 virtual void become_non_blank(boolean controlee_too=TRUE) {
	 	 set_clean_status(DIRTY);
		 blank = FALSE;
	 };
    void just_set_blank(boolean new_flag) {
       blank = (flag) new_flag;
    };
	 virtual boolean is_blank() { // made virtual on 260201
		return(blank);
	 };
	 virtual boolean blankable() {
		return(FALSE); // by default
	 };
	 virtual void set_to_future_value(Sprite *other, Sprite *original_recipient, Sprite *by) { // added by and action_status on 140202
		// signal an error ??
	 };
//	 void destroy();
	 virtual void adjust_grasp(city_coordinate &adjustment_x,
										city_coordinate &adjustment_y) {
		 // by default make no adjustment
		 adjustment_x = 0;
		 adjustment_y = 0;
	 };
	 virtual void adjust_max_size(city_coordinate &max_width,
											city_coordinate &max_height) {
		 // ok by default
	 };	  
	  virtual Sprite *if_released() {
		  return(this); // copier wand returns attached
	  };
	  virtual Sprite *dereference() {
		  return(this); // nests return contents instead
	  };
	  virtual void set_in_notebook(Notebook *notebook, boolean recur=TRUE);
	  boolean in_notebook() {
		  return(in_notebook_flag);
	  };
     //void pretend_to_be_in_notebook() { // new on 071002
     //   in_notebook_flag = TRUE;
     //};
     //void stop_pretending_to_be_in_notebook() {
     //   in_notebook_flag = FALSE;
     //};
//	  virtual void abort_operation() {
//		  // nothing by default
//	  };
	  virtual void mouse_done() {
	  };
	  virtual RemoteStatus remote_status() {
		 return(NOT_REMOTE);
	  };
	  virtual ChangeFrequency change_frequency() {
		  return(NEVER_CHANGES); 
	  };
	  virtual Sprite *now_inside_thought_bubble(boolean in_training) {
		  return(this); // nothing by default
	  };
     void now_released_on(Sprite *underneath, Sprite *by);
	  virtual void now_on_floor(Background *floor, Sprite *by);
	  void signal_now_on_floor();
     virtual void recursively_activate_pictures(boolean reset_animation=TRUE, 
																PictureActivateContext context=PICTURE_ACTIVATE_CONTEXT_NOT_INTIALIZED); 
																// removed 091203, millisecond delay=0);
	  virtual void activate(boolean reset_animation=TRUE, 
									PictureActivateContext context=PICTURE_ACTIVATE_CONTEXT_NOT_INTIALIZED) {
	  }; 
	  // do nothing -- removed 091203 millisecond delay=0 
//	  virtual boolean is_stopped() {
//		  return(FALSE);
//	  };
	  void remove_from_floor(boolean report_problems=TRUE,
									 boolean remove_from_screen=TRUE,
                            boolean reset_background=TRUE);
	  virtual void abort_action_if_robot(int reason, 
		  // default removed on 130205 since was a bad idea to generate confusing "something is wrong" message
                                        SpriteType actor=NONE_GIVEN, 
													 boolean explain_anyway=FALSE,
                                        int importance=-1,
													 string default_string="");
	  virtual void abort_action(int reason=0, SpriteType actor=NONE_GIVEN, int importance=-1, string default_string="");
	  millisecond default_duration(millisecond default_duration=1000);
	  void set_held_status(HeldStatus new_status) {
#if TT_DEBUG_ON
		if (debug_counter == tt_debug_target) {
			tt_error_file() << "Debug target setting held status." << endl;
		};
#endif
		  held_status = new_status;
	  };
	  HeldStatus current_held_status() {
		  return(held_status);
	  };
     boolean held() {
       return(held_status != NOT_HELD);
     };
	  boolean to_be_smashed() {
		 return(to_be_smashed_flag);
	  };
	  void set_to_be_smashed(boolean new_flag) {
		 to_be_smashed_flag = (flag) new_flag;
	  };
	  boolean followers_cached() {
		  return(cache_status == CACHED_WITH_FOLLOWERS);
	  };
	  boolean cached_by_leader() {
		  return(cache_status == CACHED_BY_LEADER);
	  };
	  boolean cached_without_followers() {
		  return(cache_status == CACHED_WITHOUT_FOLLOWERS);
	  };
	  void set_cached_with_followers();
	  void set_cached_without_followers() {
		  cache_status = CACHED_WITHOUT_FOLLOWERS;
	  };
	  void recursively_reset_cache_status();
	  void recursively_set_cache_status(CacheStatus new_status);
	  void set_cache_status(CacheStatus new_status) {
		  cache_status = new_status;
	  };
	  CacheStatus current_cache_status() {
		  return(cache_status);
	  };
	  boolean cached() {
		  return(cache_status != NOT_CACHED);
	  };
	  void set_display_cost(cost new_cost) {
		  display_cost = new_cost;
	  };
	  void recursively_set_visible(boolean new_flag);
	  void set_visible(boolean new_flag) {
#if TT_DEBUG_ON
		  if (debug_counter == tt_debug_target) {
			  tt_error_file() << "Debug target setting visibility." << endl;
		  };
#endif
		  visible_flag = (flag) new_flag;
	  };
	  boolean visible() {
		  return(visible_flag);
	  };
	  virtual void set_show_all(boolean new_flag) {
		  show_all_flag = (flag) new_flag;
	  };
	  boolean show_all() {
		 return(show_all_flag);
	  };
     virtual boolean show_all_default() { // new on 160802
        return(FALSE);
     };
	  void set_is_picture(boolean new_flag) { 
		  is_picture_flag = (flag) new_flag;
	  };
	  boolean is_picture() {
		  return(is_picture_flag);
	  };
	  virtual boolean like_a_picture() {
		  // new on 141299 - currently only PICTURES and houses return TRUE
		  return(FALSE); 
	  };
     void set_ok_to_dump(boolean new_flag) {
        ok_to_dump_flag = (flag) new_flag;
     };
     virtual boolean ok_to_dump() { // made virtual on 040803
        return(ok_to_dump_flag);
     };
	  virtual boolean being_used() {
		 return(FALSE); // but not always for tools
	  };
	  TTImage *pointer_to_current_image() { // restored as simple access on 240803
		  return(current_image);
	  };
	  virtual void now_on_back_of(Picture *picture) {
		 // only used now to find and update controlees
	  };
     virtual void button_pushed(Button *button, Sprite *by) {
       // should it complete the action? -- will this ever get called??
     };
     virtual void set_home_notebook(Notebook *notebook); // was made virtual on 260900 and restored on 270900
	  void copy_leader_offsets(Sprite *other);
     Leader_Offsets *pointer_to_leader_offsets() {
        return(leader_offsets);
     };
//     virtual boolean is_outside_collection() {
//       return(FALSE);
//     };
     boolean outside();
     boolean resize_if_in_room();
     virtual void stop_moving(boolean horizontal, boolean vertical) {
     };
     void set_containable_by_background(boolean new_flag) {
        containable_by_background_flag = (flag) new_flag;
     };
     boolean containable_by_background() {
        return(containable_by_background_flag);
     };
	  void set_move_between_floor_and_room(boolean new_flag) {
		  move_between_floor_and_room_flag = (flag) new_flag;
	  };
	  boolean move_between_floor_and_room() {
		  return(move_between_floor_and_room_flag);
	  };
     virtual void set_subpicture(boolean new_flag, boolean set_show_some=TRUE) { // only pictures care about this
     };
	  virtual void become_part_of_picture(boolean set_show_some=TRUE) { // only pictures care about this
	  };
     boolean finishing_instantly() {
        return(finishing_instantly_flag || tt_finish_all_animations);
     };
	  virtual void update_text_and_widths(boolean sizes_constrained) {
	  };
#if TT_DEBUG_ON
     void increment_ref_count(Sprite *by=NULL); // by is just for debuggin
	  void decrement_ref_count();
#else
     void increment_ref_count(Sprite *by=NULL) {  // should get optimized away...
		  ref_count++;
	  };
	  void decrement_ref_count() {
		  ref_count--;
	  };
#endif
     int return_ref_count() { // was unsigned short int prior to 260803 but ref_count type updated much earlier
        return(ref_count);
     };
	  void dont_ref_count() { // new on 260803
		  ref_count = max_long/2; // should be good enough -- don't want it to be incremented to negative or decremented to zero 
	  };
     virtual boolean ok_for_mouse_to_temporarily_add_follower() {
        return(TRUE);
     };
     virtual void stop_running() {
		  stop_animations();
	  };  // only tools care about this
	  boolean on_back_of_picture();
	  boolean on_back_of_picture_or_limbo();
	  virtual boolean is_flipped() {
		 return(FALSE);
	  };
	  virtual boolean is_temporarily_flipped() {
		 return(FALSE);
	  };
	  virtual boolean is_temporarily_unflipped() {
		 return(FALSE);
	  };
//	  virtual Sprite *appearance_before_vacuuming() {
//		  return(NULL);
//	  };
//	  virtual void reset_appearance_before_vacuuming() {
//	  };
	  virtual Sprite *first_follower_of_type(SpriteType type);
	  virtual Path *path_to(Sprite *target, boolean &ok) {
		  ok = FALSE;
		  return(NULL);
	  };
	  virtual boolean still_running() {
		  return(FALSE); // only running robots are
	  };
	  virtual void stop_all(boolean for_good=FALSE, boolean suspend_robots_too=TRUE);
	  Sprite *pointer_to_comment() {
		  return(comment);
	  };
	  void set_comment(Sprite *new_comment);
	  void describe_comment(output_stream &message);
	  int image_cycle_resource_index();
	  virtual boolean can_be_a_number() {
		  return(FALSE);
	  };
	  virtual boolean can_be_text() { // new on 201202
		  return(FALSE);
	  };
	  virtual void leader_changed() {}; // ignore it
	  boolean member_of_background(Background *background);
	  virtual void now_on_top_of_nest(boolean just_arrived);
     virtual void move_to_hand() { // Programmer_At_Floor *the_programmer) {
	  };
	  virtual boolean outside_truck() {
		  return(FALSE);
	  };
	  virtual void set_called_since_last_release(boolean new_flag) {
	  };
	  virtual boolean make_sound() {
		  return(FALSE);
	  };
     virtual void stop_making_sound() {
     };
	  virtual void set_active(boolean new_flag, boolean recur=TRUE, boolean initialization=FALSE);
//     void set_activatable(boolean new_flag, boolean recur=TRUE); 
     virtual void just_set_inactive() { // new on 081002 -- made virtual on 140605
        active_flag = FALSE;
     };
	  virtual void restore_active(boolean new_flag) { // new on 100901
		  set_active(new_flag);
	  };
	  boolean active() {
	 	 return(active_flag);
	  };
     boolean activatable() {
//	 	 return(activatable_flag);
        return(!inside_vacuum_flag && !in_notebook_flag); // no need to introduce activatable_flag (new on 160702)
	  };
     boolean ok_to_activate() {
        // not clear I need both this and the above - new on 160702
        return(active_flag && !inside_vacuum_flag && !in_notebook_flag);
     };
	  virtual void remove_all_flipped_followers(boolean destroy_too) {
	  };
	  boolean return_single_step() {
		  return(single_step);
	  };
	  virtual void finished_loading() { // new on 170100
		  // used to fix geometric relationships after loading geometry
	  };
	  //boolean adjust_height_if_camera_above() { // new on 210100 -- commented out on 230804
		 //return(adjust_height_if_camera_above_flag);
	  //};
	  //void set_adjust_height_if_camera_above(boolean new_flag) {
		 // adjust_height_if_camera_above_flag = (flag) new_flag;
	  //};
	  boolean scale_for_height_of_camera() {
		  return(scale_for_height_of_camera_flag);
	  };
	  void set_scale_for_height_of_camera(boolean new_flag) {
		  scale_for_height_of_camera_flag = (flag) new_flag;
	  };
	  virtual Sprites *flipped_followers() { // made virtual on 140200 - arg removed 201200
		  return(NULL);
	  };
	  virtual boolean no_flipped_followers() { // new on 091100
		  return(TRUE);
	  };
//	  virtual void side_about_to_be_seen(boolean front);
	  virtual void changed_by_keyboard() {
	  };
	  virtual boolean need_subject_to_work() {
		  // new on 170500 - only really applies to tools like Dusty and Wand that need something to work on (usually)
		  return(FALSE);
	  };
//#if TT_DIRECT_PLAY
     // noticed this was obsolete on 161102
//	  virtual void set_being_marshalled(boolean new_flag);
//	  boolean being_marshalled() {
//		  return(being_marshalled_flag);
//	  };
//#endif
	  void set_is_top_of_a_stack(boolean new_flag) {
		  is_top_of_a_stack_flag = (flag) new_flag;
	  };
	  boolean is_top_of_a_stack() {
		  return(is_top_of_a_stack_flag);
	  };
     virtual void set_cause_of_match_failure(boolean new_flag, boolean recur); // made virtual on 220902
     boolean cause_of_match_failure() {
        return(cause_of_match_failure_flag);
     };
	  //virtual House *pointer_to_house() {
		 // return(NULL); // made virtual on 290500
	  //};
	  virtual House *pointer_to_house(); // the above was never specialized so it always returned NULL -- fixed on 020405
	  // made virtual again on 080705 since House_sprite does specialize this
	  virtual boolean house_sprite() {
		  return(FALSE); // new on 080705
	  };
	  virtual boolean ok_to_delete_bammed() { // shouldn't really be called on non-robot, right??
		  return(TRUE);
	  };
	  boolean ok_to_temporarily_reset_size();
	  virtual void notebook_just_flew_out(Notebook *notebook) {
	  };
	  virtual city_coordinate return_character_width() {
		  return(0);
	  };
	  virtual city_coordinate return_character_height() {
		  return(0);
	  };
	  virtual void set_character_width(city_coordinate new_width) {
		  // ignore
	  };
	  virtual void set_character_height(city_coordinate new_height) {
		  // ignore
	  };
	  boolean top_level_equal_to(Sprite *other, SpritesPointer &corresponding_nests, SpritesPointer &expected_corresponding_nests, 
										  EqualReason reason=NORMAL_EQUAL); // new on 080601
	  virtual boolean equal_to(Sprite *other, Sprites *pending, SpritesPointer &corresponding_nests, 
										SpritesPointer &expected_corresponding_nests,
										EqualReason reason=NORMAL_EQUAL); // new on 050601
	  boolean check_pending_for_equals(Sprite *other, Sprites *pending); // new on 050601
	  virtual void set_saved_width(city_coordinate w) { // new on 050601
	  };
	  virtual void set_saved_height(city_coordinate h) { // new on 050601
	  };
	  virtual SpriteType dump_type() { // moved here from text.h on 060601 so everyone answers this
		  return(kind_of());
	  };
     virtual void completed() { // only robots (and programmer_on_the_floor) care about this -- new on 171102
     };
     virtual void action_aborted() { // only robots care about this -- new on 171102
     };
     virtual boolean is_action_aborted() { // only robots care about this -- new on 171102
        return(FALSE);
     };
     void set_saved_by(Sprite *by, boolean regardless=FALSE);
     // the following XML stuff is new as of 010802
#if TT_XML
     virtual wide_string xml_tag();
	  void xml_to_stream(output_stream &stream);
     xml_document *xml_create_document(boolean include_explanation=TRUE);
	  void top_level_xml(xml_document *document, xml_element *parent); // boolean generate_toontalk_object=TRUE);
     boolean might_dump_as_shared_or_equal();
     void add_xml(xml_element *parent_element, wide_string tag, xml_document *document); // boolean include_file_ok=TRUE);
	  void xml_create_external_link_and_append_element(xml_element *parent, xml_document *document, wide_string tag=NULL); 
	  // tag new on 050104
     xml_element *xml_create_and_append_element(xml_node *parent, xml_document *document, boolean might_be_shared=TRUE, 
																boolean top_level=FALSE);
	  // removed boolean include_file_ok=TRUE);
//	  void generate_include_file();
	  VisibleNotebookPage is_visible_notebook_page();
	  //	  void check_for_independence();
	  //void ultimate_leader_not_saved_as_include_file() { // new on 120703 so this won't become an include file
			//ultimate_leader()->set_recent_xml_location(-2);
	  //};
//     virtual xml_element *xml_create_element(xml_document *document);
     // sprite::xml s where extra information is given to the element
     // it is split in two (xml_create_element) so that recursive cycles can be handled correctly
     // this method can recursively call while xml_create_element should not
     virtual void xml(xml_element *element, xml_document *document);
     virtual boolean handle_xml_tag(Tag tag, xml_node *node);
     void set_extra_xml(xml_node_pointer *initial_extra_xml, int initial_extra_xml_count);
     xml_element *xml_geometry(xml_document *document);
//     xml_element *xml_flags(xml_document *document);
//     xml_element *xml_index(int dump_index, xml_document *document);
//     xml_element *xml_comment(xml_document *document);
//     void append_standard_elements(xml_element *element, xml_document *document); -- renamed to sprite::xml on 121102
     virtual xml_element *xml_set_special_attributes(xml_element *element, xml_document *document); 
	  // made virtual on 211102 since no need to save this stuff for boxes and pads
     virtual boolean xml_get_special_attributes(xml_node *node); // made virtual on 261102
	  virtual void add_special_xml(xml_element *element, xml_document *document);
     virtual boolean geometry_worth_saving(); // new on 130802
	  virtual void set_current_block(Block *block) {
		  // do nothing - needed for trucks driving outside - new on 261202
	  };
	  virtual short int default_parameter() { // new on 250103 - only used for dumping XML now
	     return(0);
	  };
//	  virtual void set_recent_xml_location(int location); // made virtual on 190703 to optimize notebooks (and maybe more)
//	  int return_recent_xml_location() {
//		  return(recent_xml_location);
//	  };
     //virtual void xml_dump(output_stream &stream) {
     //   // should make this abstract instead
     //};
     //void xml_dump_geometry(output_stream &stream);
     //void xml_dump_flags(output_stream &stream);
     //void xml_dump_comment(output_stream &stream);
     //void xml_dump_index(int dump_index, output_stream &stream);
//   void xml_guid(output_stream &stream);
#endif
//	  virtual void become(Sprite *other);
	  bytes color_permutation(SelectionFeedback selection_feedback=NO_FEEDBACK); // new on 200901
//	  virtual boolean stable_while_in_hand() { // new on 210901
//		  return(FALSE); // by default looking nice is better than accuracy (not true of tools)
//	  };
	  void tell_programmer_am_ready();
	  virtual boolean ok_to_dump_as_text() { // new on 041201
		  return(FALSE);
	  };
	  virtual color appropriate_background_color() {
		 return(WHITE);
	  };
	  virtual color_index appropriate_background_color_index() { // new on 080202
		 return(tt_white);
	  };
	  virtual void set_change_size_to_fit_extent(boolean new_flag) {
		  // do nothing by default
	  };
	  virtual void set_change_font_size_to_fit_extent(boolean new_flag) {
		  // do nothing by default
	  };
	  virtual boolean is_an_operation_with_no_value() {
		 return(FALSE);
	  };
     boolean want_feedback(SelectionFeedback feedback) {
        // prior to 250302 this was an inlined function - 
		  // moved here and added tt_cause_of_last_mismatch so see why a robot rejected box
        return(feedback != NO_FEEDBACK && feedback != NO_FEEDBACK_BUT_DISPLAY_OUTSIDE_FOLLOWERS); 
		  // || this == tt_cause_of_last_mismatch);
     };
     virtual Picture *pointer_to_controlee() { // new on 281002 so can be called without knowing if it is a picture
        return(NULL);
     };
	  virtual Sprite *pointer_to_indirection() { // new on 010704 so can be called without knowing if it is a picture
        return(NULL);
     };
     virtual void action_completed();
	  virtual void add_completion() { // new on 210103 to simplify callers
		  // only robots care
	  };
	  void initialize_followers(Sprites *sprites) {
		  followers = sprites;
	  };
	  virtual string private_media_file_name(boolean &worth_compressing, boolean full_path=TRUE) { // new on 180303
		  // added full_path arg on 200703
		  return(NULL);
	  };
	  virtual hash return_hash() {
		  return(NULL);
	  };
	  virtual wide_string private_media_file_extension() { // new on 200703
		 return(L""); // OK?
	  };
	  virtual void set_private_media_file_name(string name) { // new on 180303
	  };
	  virtual boolean ok_to_rescue_from_exploding_house() { // new on 080403
		  return(TRUE);
	  };
	  void snapshot(string snapshot_file_name, boolean without_followers=FALSE);
	  virtual void translated_from_floor_to_room() {
	  };
	  virtual void translated_from_room_to_floor() {
	  };
	  GUID *create_guid(); // moved here on 140703
	  boolean has_guid(GUID *another_guid) {
		  return(guid != NULL && IsEqualGUID(*guid,*another_guid));
	  };
	  void set_guid(GUID *new_guid);
	  GUID *pointer_to_guid() {
		  return(guid);
	  };
	  void just_remove_guid();
//	  void check_and_update_include_locations();
		boolean smaller_than_a_pixel();
	  virtual boolean is_built_in_notebook() {
		  return(FALSE); // new on 140803
	  };
	  virtual ascii_string pointer_to_file_name() {
		  return(NULL); // new on 140803
	  };
	  virtual void tell_flipped_followers_now_on_back() { // new on 101203
	  };
	  virtual ArrowKeyEditing arrow_key_editing_status() { // new on 110604
		  return(NO_ARROW_KEY_EDITING_POSSIBLE);
	  };
	  virtual int return_insertion_point() { // new on 120604
		 return(-1);
	  };
	  virtual void set_insertion_point(int new_insertion_point) { // called when loading XML
	  };
	  virtual boolean can_send_keyboard_editing_keys() { // new on 110604
		  return(tt_log_version_number >= 45);
	  };
	  virtual void morph_to_inert() {
		  // made virtual on 300604
	  };
	  virtual boolean displays_as_pad() { // new on 011004
		  return(FALSE);
	  };
	  virtual void note_that_team_is_about_to_run_again(); // new on 030105
	  virtual Sprite *pointer_to_top_contents() {
		 return(NULL); // made virtual on 220505
	  };
	  virtual void update_remote(boolean regardless, UpdateStatus update_status=NO_UPDATE_CHANGE) {
		  // new on 060605 to be virtual
	  };
	  virtual boolean is_main_cubby_in_body() {
		  return(FALSE); // new on 060705 to be virtual (wasn't really done until 080705)
	  };
	  virtual boolean running_to_hand() {
		  return(FALSE); // new on 221105
	  };
	  virtual boolean decoration_remote() {
		  return(FALSE); // new on 170106
	  };
	  virtual void follower_used_in_decoration_changed(Sprite *follower) {
		  // new on 170106
	  };
//	  virtual void set_stopped(boolean new_flag) {
//	  };
//	  void set_size_porportional_to_screen_size(boolean new_flag);
//	  boolean size_porportional_to_screen_size() {
//		  return(size_porportional_to_screen_size_flag);
//	  };
//	  boolean last_size_change_due_to_indirection() {
//		  return(FALSE);
//	  };
//     void set_selected(boolean new_flag) {
//        selected_flag = (flag) new_flag;
//     };
//     boolean selected() {
//        return(selected_flag);
//     };
#if TT_DEBUG_ON
     long return_debug_counter() {
        return(debug_counter);
     };
     void print(output_stream &stream);
//	  void display_debug_info(output_stream &stream, int indent);
	  virtual boolean does_follower_matter(Sprite *sprite) {
		  return(TRUE); // good default
	  };
	  boolean trashed() { // new on 280903
		  return(debug_counter < 0 || code < 0); // or lots more things I can could add
	  };
#endif
//	  void set_animations(Animation *replacement_animations, Animation *replacement_new_animations) {
//		  animations = replacement_animations;
//		  new_animations = replacement_new_animations;
//	  };
//	  void transfer_animations_to(Sprite *other);
//	  boolean on_flip_side_of_picture() {
//		  return(on_flip_side_of_picture_flag);
//	  };
//	  void set_on_flip_side_of_picture(boolean new_flag) {
//		  on_flip_side_of_picture_flag = (flag) new_flag;
//	  };
//	  void set_cached_in_background(boolean new_flag) {
//		 cached_in_background = (flag) new_flag;
//	  };
//	  boolean size_constrained() {
//		 return(size_constrained_flag);
//	  };
//	  void set_size_constrained(boolean new_flag) {
//		 size_constrained_flag = (flag) new_flag;
//	  };
//	  Sprite *pop_off_stack(TTRegion *region, SelectionReason reason,
//								 Screen *screen, Floor *floor);
//		Tool *next;
//		TTRegion region; // the active part that works

//		flag on_flip_side_of_picture_flag : 1;
//		flag cached_in_background : 1;
//		flag size_constrained_flag : 1; // e.g. not for a number of the floor
#if TT_DEBUG_ON
	 long debug_counter;
#endif
#if TT_DEBUG_ON||TT_CAREFUL
	 void set_deleted();
	 boolean deleted() {
		 return(deleted_flag);
	 };
#endif
 protected:
	 void absorb_followers();
	 void un_absorb_followers();
  SpriteCode code;
  short int current_index;
  short int parameter; //, original_parameter, goal_delta_parameter;
  TTImage *current_image;
  millisecond cycle_start_time; // used by current cycle
  millisecond cycle_duration;
  long ideal_speed;
//  int cycle_index; // current_image_index
  city_coordinate old_x_offset, old_y_offset;
//  city_coordinate original_width, original_height;
  Background *floor;
  city_scale current_scale;
  // tried : 3 below but there once were problems
  CleanStatus clean_status; // whether it has changed since last cached
//  ChangeReason previous_change_reason; // updated when clean_status is set to DIRTY
  CacheStatus cache_status; // : 2 caused bugs
	// removed from private because it interferes with bit packing flags
	// made private because can be 0 if leader has a positive screen ref count
	// so showing_on_a_screen() should be used
//	 int screen_ref_count; // number of screens showing this thing
//	flag screen_ref_count : 1; // single bit fine since only 1 screen
//	 city_coordinate collision_region_width, collision_region_height;
//	 city_coordinate collision_x_offset, collision_y_offset;
//	 TTRegion *region;
	 // if it turns out to be necessary could add collision_region_offsets
//    Collider *collider;
	 SpriteType sprite_type;
	 city_coordinate llx, lly;//, previous_llx, previous_lly;
	 city_coordinate width, height; //, previous_width, previous_height,
//						  saved_width, saved_height;
	 unsigned short int x_scale, y_scale;
//  TTRegion region; // for implementing full_region();
	 AnimationCallBack cycle_stopped_callback;
//    void update_display_and_followers();
	 long current_priority; // the lower the number the closer to the camera
	 long floor_priority; // added on 060699 since otherwise is clobbered when standing up
//	 int pixel_width, pixel_height;
//	 boolean animation_status_flag;
	 Animation *animations, *new_animations; //, *animations_tail;
	 Sprites *followers;
	 Sprite *leader;
	 Leader_Offsets *leader_offsets;
	 flag deleted_flag : 1; // rewritten on 020604
    flag looped_back_already : 1;
    flag single_step : 1; // used by current cycle
//  flag cached_flag : 1;
//  flag followers_cached_flag : 1;
//  flag clipping_flag : 1;
    flag true_size_flag : 1;
    flag size_changed_flag : 1;
//  flag followers_on_top_flag : 1;
//  flag transparency_on_flag : 1;
//	 flag visible_flag : 1;
	 flag changes_propagated_flag : 1;
    flag priority_fixed_flag : 1;
//	 flag priority_function_of_lly_flag : 1;
//	 flag notify_if_colliding_flag : 1;
//	 flag can_collide_flag : 1;
//	 flag motion_extends_collision_region : 1;
//	 flag collision_region_equals_region : 1;
    // following 2 are identical unless followers are absorbed
	 flag screen_updates_flag : 1;
	 flag followers_completely_inside_flag : 1;
	 flag inside_vacuum_flag : 1; 
	 flag infinite_stack_flag : 1;
	 flag available_for_indexing_flag : 1;
	 flag slated_for_deletion_flag : 1;
    flag graspable_flag : 1;
    flag selectable_flag : 1;
	 flag completely_inside_leader_flag : 1;
	 flag followers_absorbed_flag : 1;
//	 flag leaders_removed_flag : 1;
	 flag running_animations : 1;
	 flag finish_animation_pending : 1;
    flag animation_pending_callbacks_only : 1;
//	 flag update_display_in_progress : 1;
//	 flag display_updated : 1;	 
//	 flag game_element : 1; // visible when running the game
	 // above group moved on 060204 to join up all the 1 bit flags
	 flag in_notebook_flag : 1;
 	 flag to_be_smashed_flag : 1;
	 flag blank : 1;
	 flag visible_flag : 1;
	 flag show_all_flag : 1;
	 flag is_picture_flag : 1;
	 flag prepared_for_deletion_flag : 1;
    flag never_cache_flag : 1;
    flag containable_by_background_flag : 1;
    flag ok_to_dump_flag : 1;
    flag finishing_instantly_flag : 1;
	 flag move_between_floor_and_room_flag : 1;
	 flag active_flag : 1; // moved here from number.h on 191199
//    flag activatable_flag : 1; // new on 160702 so that when active but in vacuum or in notebook isn't activated
//	 flag size_porportional_to_screen_size_flag : 1;
//    flag selected_flag : 1;
	 flag is_top_of_a_stack_flag : 1; // new on 260500
    flag cause_of_match_failure_flag : 1; // new on 250302
	 flag preparing_for_deletion : 1; // new on 060204
	 flag releasing_all_references : 1; // new on 230305 since can be called recursively (e.g. via followers and saved_by)
	 // following moved here on 060204 so all the 1 bit flags are together and will hopefully be optimized
//	 flag adjust_height_if_camera_above_flag : 1; // new on 210100 - initially only used for time travel buttons
	 flag scale_for_height_of_camera_flag : 1; // new on 230804 to deal better with displaying things when flying high over the city
   // following (with : 2) wasn't behaving properly when the above flag was set ...
	 HeldStatus held_status; // : 2; // seems to behave ok but inspector shows some values as negative
    int ref_count; // prior to 090203 was unsigned short int but want negative to deal with too many decrements - 
	 // e.g. deleting excess in Dusty -- also 64K is too small a limit
 	 cost clean_run, clean_run_average, display_cost;
	 Sprite *comment; // most general since can be text or audio or whatever
	 
//#if TT_DIRECT_PLAY
//	 flag being_marshalled_flag : 1;
//#endif
	 Sprite *top_of_stack; // new on 260500
    Sprite *saved_by; // moved here on 171102 since used by nearly every subclass 
    GUID *guid; // moved here from nest on 140703
#if TT_XML
    xml_node_pointer *extra_xml; // new on 310102 to store XML elements that this version doesn't know how to handle
    int extra_xml_count;
//	 int recent_xml_location; // new on 080703 - this is an integer than with DynaZip is the file number - 
	 // otherwise could be converted to a string version of a file name
#endif
};

/*
class FixedPrioritySprite : public Sprite {
  // just like Sprite but set_priority has no effect
  public:
	 FixedPrioritySprite(SpriteCode appearance_code,
			  city_coordinate center_x=0,
			  city_coordinate center_y=0,
			  SpriteType col_type=NONE_GIVEN,
			  short int initial_parameter=0,
			  city_scale current_scale=ground_scale,
			  city_coordinate width=0,
			  city_coordinate height=0,
			  boolean true_sizes=FALSE,
			  boolean function_of_lly_flag=TRUE) :
    Sprite(appearance_code,
			  center_x,
			  center_y,
			  col_type,
			  initial_parameter,
			  current_scale,
		     width,
			  height,
			  true_sizes,
			  function_of_lly_flag),
    priority_fixed(TRUE) {
    };
    void set_priority(long priority) {
       if (!priority_fixed) current_priority = priority;
    };
    
  protected:
    flag priority_fixed;
};
*/

class Image_cycle {
 public:
	 Image_cycle() { };
  ~Image_cycle();
  void initialize(short int count, TTImagePointer *i, short int *d,
//						unsigned int the_cycle_duration,
						short int loop_back_index, boolean distance_flag);
//  void reset(); // obsolete so commented out on 120302
  void java(java_stream &out, const_java_string image_cycle_name, int parameter);
  TTImage *update_display(const boolean change,
								  const city_coordinate delta_x, const city_coordinate delta_y,
								  long ideal_speed,
								  short int &current_index,
								  millisecond &cycle_start_time,
								  millisecond &cycle_duration,
								  boolean &looped_back_already,
								  boolean &single_step,
//								  int &pixel_width, int &pixel_height,
								  city_coordinate &x_offset, city_coordinate &y_offset);
//								  city_coordinate &the_original_width,
//								  city_coordinate &the_original_height);
  millisecond total_cycle_duration(boolean looped_back_already);
  TTImage *nth_image(int i) {
	  return(images[i]); // removed &
  };
  void set_nth_image(int i, TTImage *image) { // was &image prior to 231001
	  images[i] = image;
  };
  void save_nth_image(int i, TTImagePointer &image) { // was &image prior to 231001 -- and was TTImage * prior to 190603
	  image = images[i];
  };
  int return_number_of_images() {
	  return(number_of_images);
  };
  boolean cycle_stopped(short int current_index) {
	  return(current_index == loop_back_index &&
			   current_index == number_of_images-1);
  };
  boolean cycles() {
	  return(loop_back_index != number_of_images-1);
  };
  int resource_index_of_first_cycle_image() {
	  return(images[0]->return_resource_index()); // was . prior to 231001
  };
  int return_loop_back_index() { // new on 050204
	  return(loop_back_index);
  };
 private:                                
  short int number_of_images;
//  millisecond cycle_duration;
  short int loop_back_index;
  flag always_cycle : 1, function_of_distance : 1;
//  flag recomputed_cycle_duration : 1; // new on 040204
  TTImagePointer *images; // was just TTImage prior to 231001
  short int *durations;
};

//void set_programmer_head(int choice); // was here but wasn't implemented prior to 180603

int selection_x_growth();
int selection_y_growth();
city_coordinate selection_delta_x();
city_coordinate selection_delta_y();

boolean still_picture(SpriteCode code);
int parameter_count(SpriteCode code);
//TTImage *image_for_picture(int index);
//void display_digit(int digit, unsigned short int scale);

TTImage *number_pad(PlateState plate);
void display_wall(SpriteCode code);
city_coordinate wall_lly();

boolean display_cubby_hole(int parameter, unsigned short int x_scale, unsigned short int y_scale, bytes color_permutation,
									long &pixel_width, long &pixel_height);

work_page load_background(int index);
boolean load_sprite_file(SpriteCode code);
boolean valid_image_id(int id);
void swap_heads(int new_programmer_head);

boolean initialize_sprite_values();
void release_sound_cache();
#if TT_DIRECTX
void stop_all_sounds_in_cache();
#endif
//void initialize_pictures();
void release_sprite_data();
sound_buffer retrieve_sound(int index, DeletedMemoryPointer &temporary_sound);
boolean open_images_file(int mode_number);

void update_keep_every();
boolean equal(Sprite *x, Sprite *y, EqualReason reason=NORMAL_EQUAL);
boolean they_dump_as_equals(Sprite *x, Sprite *y);
//#if TT_32
// 16-bit compiler complained that this was re-defining multiply...
inline boolean is_kind_of(Sprite *sprite, SpriteType type) {
   return((boolean) ((sprite==NULL)?FALSE:(sprite->kind_of()==type)));
};
//#else
//boolean is_kind_of(Sprite *sprite, SpriteType type);
//#endif
const_string type_name(Sprite *sprite);
const_string type_string(int i, boolean capitalize_first=FALSE);
void initialize_background_sprites();
BackgroundImage *background(int index);
void java_image_cycle(java_stream &out, int code);

//int return_debug_counter();
//void set_debug_counter(int n);

void dump_keep_every_data(output_stream &stream);
void load_keep_every_data(InputStream *stream);

void compute_decibal_level();

void load_common_flags(InputStream *stream, boolean &blank, boolean &infinite_stack);

void set_sprites_to_active_now();
void add_to_active_sprites(Sprite *sprite);
void remove_from_active_sprites(Sprite *sprite);
Sprites *return_sprites_to_set_active();
void set_sprites_to_set_active(Sprites *new_sprites_to_set_active);

void reallocate_appearance_data(); // new on 240105

//#if TT_DEBUG_ON
//void perform_integrity_checks(Sprite *item, boolean up=TRUE, boolean down=TRUE);
//#endif

#endif
