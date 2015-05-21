// Copyright (c) 1992-2001.  Ken Kahn, Animated Programs, All rights reserved
#if !defined(__TT_REMOTE_H)

#define __TT_REMOTE_H

class CollisionRecord {
	public:
		CollisionRecord(Sprite *collidee,
//							 city_coordinate llx, city_coordinate lly,
							 TTRegion *region, 
							 CollisionDirection direction,
							 millisecond frame_number);
		~CollisionRecord();
		Sprite *collidee;
//		city_coordinate llx_before, lly_before;
		TTRegion *region;
		CollisionDirection direction;
		millisecond frame_number;
};

// a holder of information about who is connected to the controlee
class Remote {
  public:
	 Remote(Picture *controlee);
	 ~Remote();
//	 void copy_stuff_inside_to(Picture *other);
	 void update_globals();
//    void update_room_decoration();
//    void update_house_decoration();
//    void update_roof_decoration();
#if TT_DIRECT_INPUT
    void update_joystick_globals(short int joystick_number);
#endif
#if !TT_32
    void update_hyperbot_globals();
#endif
	 void changed(RemoteIdentifier identifier, user_coordinate new_value);
	 void broadcast_new_value(RemoteIdentifier identifier, NumberValue *value, Remote_Integer *exception);
	 void changed_appearance(RemoteIdentifier identifier, Sprite *new_appearance);
    void blankness_changed(boolean blank_now);
	 void not_clean(CleanStatus status);
//	 void set_speed_changed(boolean new_flag) {
//		 speed_changed_flag = (flag) new_flag;
//	 };
//	 boolean speed_changed() {
//		 return(speed_changed_flag);
//	 };
	 void changed_vacuum_status(boolean in_vacuum);
//	 void set_background(Background *new_floor);
//	 void changed_floor(Background *controlees_floor);
	 void no_longer_on_screen();
	 void ignore_collision();
	 void no_longer_colliding();
	 void follower_added(Sprite *leader, Sprite *new_follower);
	 void follower_removed(Sprite *leader, Sprite *old_follower);
	 void followers_changed();
	 void leader_changed(); 
	 boolean add_remote(RemoteIdentifier identifier, Sprite *remote_integer);
	 void remove_remote(RemoteIdentifier identifier, Sprite *remote);
//	 void display_changed(RemoteIdentifier identifier, long new_value);
//	 void deleted(RemoteIdentifier identifier) {
//		 remote_components[identifier] = NULL;
//	 };
	 boolean collision_status_wanted();
    void check_collisions(boolean new_collidee=FALSE);
//	 void colliding_with(Sprite *collidee);
//	 Sprite *collidee_with_most_penetration();
//	 Sprites *all_collidees();
	 CollisionDirection collision_direction(Sprite *collidee);
	 CollisionDirection left_right_collision(TTRegion &controlee_region,
														  TTRegion &collidee_region,
														  city_coordinate controlee_delta,
														  city_coordinate collidee_delta);
	 CollisionDirection up_down_collision(TTRegion &controlee_region,
													  TTRegion &collidee_region,
													  city_coordinate controlee_delta,
													  city_coordinate collidee_delta);
	 void uncollide(user_coordinate new_value);
//	 boolean y_collision(Sprite *collidee);
	 void set_controlee(Picture *new_controlee) {
		 controlee = new_controlee;
	 };
	 void controlee_deleted();
	 void remote_notebook_added(Notebook *notebook);
	 void remote_notebook_deleted(Notebook *notebook);
//	 Under_Sprite_Background *pointer_to_under_sprite_background() {
//		  return(under_sprite_background);
//	 };
#if TT_DEBUG_ON
    Sprites *remotes_for(RemoteIdentifier identifier);
#else
	 Sprites *remotes_for(RemoteIdentifier identifier) {
		 return(remotes[identifier-first_identifier]);
	 };
#endif
	 boolean currently_colliding() {
		 return(collision != NULL);
	 };
//	 user_coordinate current_x_penetration() {
//		 return(new_x_collidees_count); //x_penetration);
//	 };
//	 user_coordinate current_y_penetration() {
//		 return(new_y_collidees_count); //y_penetration);
//	 };
//	 CollisionDirection previous_collision_direction() {
//		 return(last_collision_direction);
//	 };
//	 unsigned short int return_new_collidees_count() {
//		 return(new_x_collidees_count+new_y_collidees_count);
//	 };
  private:
	 SpritesPointer *remotes;
    RemoteIdentifier first_identifier;
    unsigned char length;
//	 IntegerPointer remote_components[2];
	 Picture *controlee;
//	 Sprite *collidee;
	 CollisionRecord *collision;
//	 Sprites *old_collidees;
//	 Sprites *current_collidees;
//	 unsigned short int new_x_collidees_count;
//	 unsigned short int new_y_collidees_count;
//	 flag current_collidees_frame_number_bit : 1; // enough to know when to switch
//	 flag outside_flag : 1; 
//	 flag different_floors_blanked_components : 1;
//	 flag vacuum_blanked_components : 1;
//	 Under_Sprite_Background *under_sprite_background;
//	 CollisionDirection last_collision_direction; // buggy : 3;
	 Sprites *remote_notebooks;
//	 user_coordinate x_penetration, y_penetration;
//    flag speed_changed_flag : 1;
//	 flag x_collided : 1;
//	 flag y_collided : 1;
};

user_coordinate handle_lego_sensors(RemoteIdentifier identifier);
#if !TT_32
user_coordinate handle_lego_motors(RemoteIdentifier identifier, user_coordinate new_value);

user_coordinate set_motor_value(int outlet, user_coordinate new_power_level);
void initialize_lego_state(int input_port_count, int output_port_count);
void free_lego_state();
#endif

#if TT_DIRECT_INPUT
boolean joystick_button_still_down(int joystick_number, int joystick_button_number);
#endif


#endif
