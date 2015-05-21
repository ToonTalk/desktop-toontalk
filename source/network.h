// Copyright (c) 1992-2000.  Ken Kahn, Animated Programs, All rights reserved

#ifndef __TT_NETWORK_H

#define __TT_NETWORK_H

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif

enum SendStatus {TT_SEND_OK,TT_SEND_RETRY,TT_SEND_FAIL};

boolean initialize_direct_play();
LPDIRECTPLAY4A create_direct_play_object(string IP_address);
boolean host_a_new_direct_play_session(LPDIRECTPLAY4A direct_play, GUID *guid, Nest *nest, string name="");
boolean join_a_direct_play_session(LPDIRECTPLAY4A direct_play, GUID *guid, Nest *nest);
void receive_messages();
SendStatus send_network_message(LPDIRECTPLAY4A direct_play, DPID player_id, GUID *guid, bytes message, int message_length, Nest *nest_proxy);
int message_queue_size(LPDIRECTPLAY4A direct_play);
void release_direct_play_object(LPDIRECTPLAY4A direct_play);
void destroy_player(LPDIRECTPLAY4A direct_play, DPID player_id);
//bytes get_player_address(LPDIRECTPLAY4A direct_play, DPID player_id, unsigned short int &player_address_size);
string *get_IP_addresses(LPDIRECTPLAY4A direct_play, DPID player_id, unsigned char &address_count);
string *IP_addresses_of_player(bytes player_address, DWORD player_address_size, int &number_of_addresses);
void release_direct_play();

#endif;