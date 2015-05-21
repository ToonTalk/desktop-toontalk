// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved
#ifndef TT_WINMAIN_H

#define TT_WINMAIN_H

class TTRegion;

#if TT_MICROSOFT
#define export_declaration __stdcall 
//__declspec(dllexport)
#else
#define export_declaration CALLBACK _export
#endif

LRESULT export_declaration WndProc(HWND window_handle, UINT iMessage, WPARAM wParam, LPARAM lParam);

class Main {
  public:
		  static HINSTANCE hInstance;
		  static HINSTANCE hPrevInstance;
		  static int nCmdShow;
		  static int MessageLoop();
		  static boolean MessageLoopOnce();
		  static void really_display_stuff();
};

class Window {
protected:
	 HWND window_handle;
public:
    Window() : window_handle(NULL) {}; // new on 290204 so is initialized for dialogs such as what is your name
	 // Provide (read) access to the window's handle in case it is needed
	 // elsewhere.
	 HWND get_handle() {
		return(window_handle);
	 };
	 BOOL Show( int nCmdShow ) { return ShowWindow( window_handle, nCmdShow ); }
	 void Update( void ) {
       UpdateWindow(window_handle);
    };
    void close_window() {
       CloseWindow(window_handle);
    };
    void open_icon() {
       OpenIcon(window_handle);
    };
	 void set_focus() { // new on 030702
		 SetFocus(window_handle);
	 };
	 // Pure virtual function makes Window an abstract class.
	 virtual LRESULT WndProc( UINT iMessage, WPARAM wParam, LPARAM lParam ) = 0;
};


class MainWindow : public Window {
 public:
	 // Register the class only AFTER WinMain assigns appropriate
	 // values to static members of Main and only if no previous
	 // instances of the program exist (a previous instance would
	 // have already performed the registration).
	 static void Register( void );
	 MainWindow();
    void initialize();
    void resize_window_and_center(int screen_width, int screen_height, boolean cut_in_half_if_full_screen); // new on 130502
    void compute_screen_dimensions(int width, int height); // abstracted on 130502
    void initialize_palette();
//    void use_palette_with_color_deltas(int red_delta, int green_delta, int blue_delta);
//	 void restore_original_palette();
#if TT_DIRECTX
    void initialize_back_buffer();
    void initialize_direct_draw(boolean only_enough_for_taking_snapshots=FALSE);
#endif
//	 ~MainWindow();
	 void shut_down();
	 int set_client_size(int width, int height);
/*
#if GRAPHICS_TO_MEMORY	
	 void use_bits(bytes image);
#else
*/
	 HDC get_windows_device_context() {
		return(GetDC(window_handle));
	 };
	 void release_windows_device_context(HDC device_context) { // restored and renamed on 061101 - not clear why it didn't matter earlier
		 ReleaseDC(window_handle,device_context);
	 };
	 // not clear if this stuff is in the right place??
	 void display_background(int bitmap_index);
	 boolean generic_text_out(coordinate ulx, coordinate uly, const_string text, int length);
	 void place_character(character character, 
								 coordinate ulx, coordinate uly,
								 double width, double height,
                         color text_color);
	 void place_wide_character(wide_character character, 
										coordinate ulx, coordinate uly,
										double width, double height,
										color text_color);
	 void set_font(double width, double height,
						boolean bold=FALSE, boolean fixed_width=FALSE);
    void deselect_font();
#if TT_ALPHA_FEATURE
	void set_text_align(UINT alignment);
#endif
	 coordinate get_extent(const_string text, int length,
                          double width, double height,
				              boolean bold=FALSE, boolean fixed_width=FALSE, boolean might_contain_tabs=TRUE);
#if TT_WIDE_TEXT
	 coordinate get_extent(wide_string text, int length,
                          double width, double height,
				              boolean bold=FALSE, boolean fixed_width=FALSE, boolean might_contain_tabs=TRUE);
#endif
	 coordinate get_vertical_extent(const_string text, int length,
											  double width, double height,
											  boolean bold=FALSE, boolean fixed_width=FALSE);
#if TT_WIDE_TEXT
	 coordinate get_vertical_extent(wide_string text, int length,
											  double width, double height,
											  boolean bold=FALSE, boolean fixed_width=FALSE);
#endif
	 void get_extent_size(wide_string text, int length, int number_of_lines, 
                         double width, double height, int &longest_line, double &new_width, double &new_height,
				             boolean bold=FALSE, boolean fixed_width=FALSE, boolean might_contain_tabs=TRUE);
	 boolean text_out(const_string text, int length, 
							coordinate ulx, coordinate uly,
							double new_font_width, 
							double new_font_height,
							boolean bold=FALSE, boolean fixed_width=FALSE,
							boolean background_transparent=TRUE,
							color_index text_color=tt_black,
							boolean text_is_wide=FALSE,
							bytes color_permutation=NULL,
							color background_color=WHITE,
							boolean might_contain_tabs=TRUE,
							int caret_location=-1);
	 //boolean create_caret(coordinate height);
	 //boolean destroy_caret();
	 void draw_line(coordinate start_x, coordinate start_y, coordinate end_x, coordinate end_y);
	 void correct_font_size(wide_string text, int length, int number_of_lines,
									double width, double height,
									boolean bold, boolean fixed_width, // added fixed_width on 050102
									boolean might_contain_tabs,
									double &new_font_width, double &new_font_height);
//	 coordinate correct_font_width(wide_string text, int length, 
//											 coordinate new_font_width, 
//											 coordinate new_font_height,
//											 boolean bold);
//	 coordinate correct_font_height(wide_string text, int length, int number_of_lines,
//											  coordinate new_font_width, 
//											  coordinate new_font_height,
//											  boolean bold);
	 int place_text(const_string text, int length, coordinate ulx, coordinate uly,
						 coordinate max_width, coordinate max_height,
						 coordinate new_font_width=12, 
						 coordinate new_font_height=12, // was 0 prior to 260900
                   WhenToBreakSentences break_sentences=BREAK_SENTENCES_TO_NOT_WASTE_HALF_BALLOON,
						 boolean vertically_center=FALSE,
                   boolean dots_if_more=FALSE,
						 coordinate extra_width_at_center=0,
					 	 coordinate blank_width=0,
                   color text_color=BLACK,
                   boolean first_call=TRUE,
						 long max_length=max_long);
	int place_wide_text(wide_string text, 
							  coordinate ulx, coordinate uly,
							  coordinate max_width, coordinate max_height,
							  coordinate new_font_width,
							  coordinate new_font_height,
							  WhenToBreakSentences break_sentences,
							  boolean vertically_center,
							  boolean add_dots_if_more,
							  coordinate extra_width_at_center,
							  coordinate blank_width,
							  color text_color,
							  boolean first_call,
							  long max_length);
	 void select_brush(BrushId brush_id);
    void deselect_brush();
	 void set_brush_origin(coordinate x, coordinate y);
	 void clear(BrushId brush_id);
    void draw_synthetic_shape(SyntheticShape shape,
                              coordinate min_x, coordinate min_y,
                              coordinate max_x, coordinate max_y);
    void draw_directional_pad();
    void save_direction_pad_region();
	 void fill_box(coordinate min_x, coordinate min_y,
						coordinate max_x, coordinate max_y);
	 void select_stock_object(int object);
	 HGDIOBJ select_object(HGDIOBJ object);
	 void select_pen(HPEN pen);
    HBRUSH create_solid_brush(COLORREF color);
    HPEN create_solid_pen(COLORREF color);
    HPEN create_dotted_pen(COLORREF color);
	 void set_clip_region();
    void current_clip_region(TTRegion &region);
    void restore_clip_region(TTRegion &region);
    void clip_now();
	 HGLOBAL get_resource(ascii_string resource_name, long &size);
    void close_MCI();
	 boolean MCI_send_string(ascii_string message,
                            boolean report_problems=TRUE,
                            ascii_string return_message=NULL,
                            int return_message_length=0);
	 string clipboard_text(boolean &new_value, SpritePointer *sprite_pointer=NULL, boolean ignore=FALSE, 
								  boolean initializing_input_log=FALSE, boolean check_files_too=TRUE);
	 HDROP get_drop_handle();
    void release_last_clipboard_string();
	 boolean add_to_clipboard(string text, long length, string suffix="");
    boolean add_to_clipboard(HGLOBAL text_handle, string suffix="");
    boolean clear_clipboard();
#if !GRAPHICS_TO_MEMORY
	 void display_bitmap(HBITMAP bitmap_handle,
								int width, int height,
								int x, int y,
								unsigned int x_scale, unsigned int y_scale,
								DWORD operation=SRCCOPY);
	 void save_region(int min_x, int min_y, int max_x, int max_y);
	 void restore_region(int min_x, int min_y, int max_x, int max_y);
#endif
	 LRESULT WndProc(UINT iMessage, WPARAM wParam, LPARAM lParam);
	 void Paint();
    HPALETTE return_palette_handle() {
      return(palette_handle);
    };
    int show_subtitle(wide_string subtitle, int subtitle_length, coordinate width, coordinate height);
//                       boolean blacken=FALSE); // not used so removed on 111104
	 void push_title(string new_title);
#if TT_ALPHA_FEATURE
	 void restore_title(string new_title=NULL);
#else
	 void restore_title();
#endif
    void toggle_exclusive_mouse();
    void toggle_full_screen();
    void switch_to_full_screen();
    void set_window_size(coordinate width, coordinate height, boolean center);
	 COLORREF convert_pixel_to_current_format(COLORREF color);
	 void set_cursor_position(coordinate x, coordinate y);
	 void current_mouse_location(city_coordinate &x, city_coordinate &y);
	 void a_mouse_button_is_now_up(); // new modularity (old functionality) on 130800
	 void mouse_button_just_went_down(int which_button);
    /*boolean need_to_open_icon() {
       return(handling_loss_of_focus || restoring_exclusive_screen);
    };*/
    void no_longer_handling_loss_of_focus();
    void no_longer_restoring_exclusive_screen();
    boolean return_restoring_exclusive_screen() {
       return(restoring_exclusive_screen);
    };
    boolean return_handling_loss_of_focus() {
       return(handling_loss_of_focus); // updated on 040602
    };
    boolean handling_transition_from_full_screen_this_frame() {
       return(handling_transition_from_full_screen_on_frame == tt_frame_number); // new on 040602
    };
	 boolean handled_transition_from_full_screen_last_frame() { // new on 070502
		 return(handling_transition_from_full_screen_on_frame+1 == tt_frame_number);
	 };
    void set_handling_transition_from_full_screen_on_frame() {
       handling_transition_from_full_screen_on_frame = tt_frame_number;
    };
    void set_device_bits_per_pixel(int bits_per_pixel) {
       device_bits_per_pixel = bits_per_pixel;
    };
private:
//	 static char szClassName[9];
#if GRAPHICS_TO_MEMORY
//	 HBITMAP bitmap;
#else
	 HDC background_device_context,restore_device_context;
	 HBITMAP background_bitmap,restore_bitmap;
#endif
//	 HGLOBAL palette_resource;
	 HPALETTE palette_handle, saved_palette_handle;
//	 LOGPALETTE *logical_palette;
	 HBITMAP original_background_bitmap, original_restore_bitmap,
				original_hidden_bitmap;
	 int previous_bitmap_index;
	 long last_clipboard_size;
	 string last_clipboard_string;
	 long last_clipboard_frame_number;
	 long current_clipboard_frame_number; // new on 310803 to avoid recording or replaying more than once per frame
    boolean need_to_select_palette;
    boolean handling_loss_of_focus; // new on 140502
    long handling_transition_from_full_screen_on_frame; // added on 040602 to prevent going from windowed and back to full-screen on the same frame
    boolean restoring_exclusive_screen; // new on 160502
    coordinate previous_window_x,previous_window_y,previous_window_width,previous_window_height; // new on 200502
    long device_width, device_height; // prior to 060602 were just local variables of ::set_client_width
    int device_bits_per_pixel; // new on 100602 to save Window's setting
};

//void read_palette(ifstream &palette);
void read_palette(HFILE &palette);
void create_DIB_brush(int index, bytes brush);

// from WinG help file
//HPALETTE CreateIdentityPalette(RGBQUAD aRGB[], int nColors);
//void ClearSystemPalette(void);


void pause_toontalk(boolean interrupted=FALSE, boolean ask=TRUE, boolean shutting_down=FALSE);
void postpone_pause_toontalk(boolean interrupted=FALSE, boolean ask=TRUE, boolean shutting_down=FALSE);
void run_postponed_pause_toontalk();
void unpause_toontalk();
void toggle_pause(boolean interrupted=FALSE, boolean shutting_down=FALSE, boolean ask=TRUE);

HINSTANCE main_instance();

#if TT_DIRECTX
boolean flip_to_GDI_surface();
boolean lock_back_surface();
boolean lock_front_surface();
boolean unlock_back_surface();
boolean unlock_front_surface();
boolean get_device_context();
boolean release_device_context();
//void copy_after_next_flip();
boolean flip_surfaces(boolean dont_flip=FALSE);
//boolean copy_front_to_back_surface();
boolean blt_full_surface(draw_surface source, draw_surface destination=NULL);
draw_surface create_surface(coordinate pixel_width, coordinate pixel_height, int transparent_color=0);
void set_transparent_color(draw_surface surface, color transparent_color);
boolean lock_surface(draw_surface surface);
boolean local_lock_surface(draw_surface surface, bytes &surface_bytes, int &pitch, int &bits_per_pixel, int &width, int &height);
boolean just_lock_surface(draw_surface surface, bytes &surface_bytes, int &pitch, int &bits_per_pixel, int &width, int &height);
boolean unlock_surface(draw_surface surface);
void blt_to_back_surface(draw_surface surface, RECT &source_rectangle, boolean transparent);
void stretch_blt_to_back_surface(draw_surface surface,
                                 RECT &source_rectangle, RECT destination_rectangle,
                                 boolean transparent);
boolean stretch_blt_to_another_surface(draw_surface source,
                                       RECT &source_rectangle,
                                       draw_surface destination,
                                       RECT destination_rectangle,
                                       boolean transparent);
void release_direct_draw(boolean direct_draw_too=TRUE);
void restore_display_mode();
#endif

//#if GRAPHICS_TO_MEMORY
//LPBITMAPINFOHEADER load_dib(int resource_index);
//void unload_dib(int resource_index);
//#endif
#if DISPDIB
void load_dispdib();
#endif
//void permute_colors(bytes image, long size);
void allocate_brushes();
void cache_background();
void display_background_cache();

void copy_screen_to_file(const_ascii_string file_name, int min_x=0, int max_x=tt_screen_width, int min_y=0, int max_y=tt_screen_height);
void copy_screen_to_clipboard();

void display_on_page(work_page page, LPBITMAPINFOHEADER dib, DWORD SizeImage);
//work_page bits_of_dib(LPBITMAPINFOHEADER dib);
 
void read_arrow_keys(long &delta_x, long &delta_y);
void read_shift_and_control_keys();
void simulate_mouse_up();
void ask_what_name();
void ask_continue_or_quit(boolean interrupted=FALSE);
void loading_wait(boolean not_interruptable, boolean initializing_direct_play=FALSE);
void done_waiting_for_load();
boolean all_mouse_buttons();
//boolean report_anomolous_timings();
//boolean absolute_mouse_position(city_coordinate &x, city_coordinate &y);

void set_desired_head();
int set_programmer_head(int new_value);
boolean show_cursor(boolean show, boolean regardless=FALSE);
boolean currently_paused();
void set_currently_paused(boolean flag);
void unpause_toontalk_soon();
#if TT_DIRECTX
void restore_surface(draw_surface surface);
void dserr(HRESULT result, string extra=NULL);
#endif
boolean has_initialization_completed();
boolean has_directx_been_initialized();
void set_user_wants_to_quit(boolean new_flag);
boolean about_to_quit();
void get_exclusive_control();
void restore_surfaces();
int default_setting(const_ascii_string id);
void print_guid(LPGUID guid, output_stream &stream);
void set_back_and_front_surface_palette();
boolean arrow_key(char key);

#if TT_DIRECTX_TRANSFORM
draw_surface load_image_using_directx_transform(ascii_string full_file_name, wide_string wide_file_name, boolean warn=TRUE, int bits_per_pixel=24, draw_surface *mask_surface=NULL);
#endif
//#if TT_DIRECTX_TRANSFORM||TT_IMGSOURCE
//LPBITMAPINFOHEADER surface_to_DIB(draw_surface surface, int &transparent_color);
//#endif

#if TT_GDIPLUS
draw_surface load_image_using_gdiplus(ascii_string full_file_name, wide_string wide_file_name, boolean warn=TRUE, 
												  int bits_per_pixel=32, draw_surface *mask_surface=NULL, int *transparent_color_pointer=NULL, // , bytes color_permutation=NULL
												  hash image_hash=NULL, TTImage *image_object=NULL);
//bytes load_bytes_using_gdiplus(ascii_string full_file_name, boolean warn);
#endif

draw_surface load_any_image(ascii_string full_file_name, int bits_per_pixel, boolean warn, 
									 boolean palette_ok=TRUE, draw_surface *mask_surface_pointer=NULL, int *transparent_color_pointer=NULL, // , bytes color_permutation=NULL
									 hash image_hash=NULL, TTImage *image_object=NULL);

boolean load_image_dimensions(ascii_string full_file_name, int &surface_width, int &surface_height);

boolean move_file(string old_name, string new_name);

string hash_and_copy_file(string file_name, string new_extension, hash media_hash, boolean delete_original, string directory, int directory_length);

#if TT_POST328
string save_private_media_picture_file_as_gif(string private_media_file_name, boolean transparent, string directory);
#endif

string save_picture_file_as_gif(string file_name, boolean transparent, string directory);
void save_picture_file_as_png(string file_name, string file_name2, boolean transparent, string png_file_name);
#if TT_DIRECTX_TRANSFORM||TT_GDIPLUS
draw_surface surfaces_from_ini_file(ascii_string ini_file_name, boolean warn, boolean palette_ok, int bits_per_pixel, draw_surface *mask_surface); // , bytes color_permutation
#endif

boolean surface_size(draw_surface surface, int &surface_width, int &surface_height, int &bits_per_pixel);

boolean directx_transform_loaded();
color_index closest_color(int red_goal, int green_goal, int blue_goal, int exception);

#if !TT_DIRECT_INPUT
// added on 170800 so can compile ttbug32 for example
void capture_mouse();
#endif

void release_mouse_capture(); // made public even when TT_DIRECT_INPUT on 070602

BOOL GetRGBBitsPerPixel(HDC hdc, PINT pRed, PINT pGreen, PINT pBlue);

draw_surface surface_recolored(draw_surface original_surface, bytes color_permutation, color transparent_color); // new on 291101

coordinate adjust_x_coordinate_for_window_size(coordinate x);
coordinate adjust_y_coordinate_for_window_size(coordinate y);
coordinate return_client_width();
coordinate return_client_height();

// better to have them here than spread out in other files
inline void string_of_guid(LPGUID guid, string &guid_string) {
	UuidToString(guid,(bytes *) &guid_string);
};

inline void release_guid_string(string &guid_string) {
	RpcStringFree((bytes *) &guid_string);
};

boolean guid_from_string(string guid_string, LPGUID guid);

void wait_cursor(); // exported on 150103
void restore_cursor();

#if TT_ALPHA_FEATURE
boolean mouse_moved();
#endif

void initialize_temp_png();

void create_snapshot(Sprite *sprite, string snapshot_file_name, boolean gif);

void release_log_file_names();

boolean initialize_ole();

void empty_message_buffer();

boolean user_wants_to_interrupt_also_process_messages_now(boolean only_look_for_quit_messages);

#endif
