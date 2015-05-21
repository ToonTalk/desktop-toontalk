// Copyright (c) 1992-2003. Ken Kahn, Animated Programs, All rights reserved.

// Uses Microsoft DirectPlay for long distance birds

#if !defined(__TT_DEFS_H)
#include "defs.h"
#endif

#if TT_DIRECT_PLAY
// no need to include this code if not using it

#include <windows.h>
#include <dplobby.h>

#if !defined(__TT_GLOBALS_H)
#include "globals.h"
#endif
#if !defined(__TT_STRINGS_H)
#include "strings.h"
#endif
#if !defined(__TT_UTILS_H)
#include "utils.h"
#endif
#if !defined(__TT_WINMAIN_H)
#include "winmain.h"
#endif
#if !defined(__TT_HELP_H)
#include "help.h"
#endif
#if !defined(__TT_BIRD_H)
#include "bird.h"
#endif
#if !defined(__TT_PICTURE_H)
#include "picture.h"
#endif
#if !defined(__TT_CLEANUP_H)
#include "cleanup.h"
#endif
#if !defined(__TT_NETWORK_H)
#include "network.h"
#endif

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const GUID toontalk_GUID = { 0x321b0b80, 0x1776, 0x11d4, { 0xaf, 0x1a, 0x0, 0x80, 0xc8, 0x8d, 0xeb, 0x64 } };

LPDIRECTPLAYLOBBY3 direct_play_lobby = NULL;  

void dp_err(HRESULT result, string extra="", boolean exit_afterwards=FALSE);
boolean create_player(LPDIRECTPLAY4A direct_play, boolean host, Nest *nest, string name="");
boolean receive_next_message(Nest *nest);
void process_system_message(DPMSG_GENERIC *message, Nest *nest);
void process_bird_message(Nest *nest, bytes message_buffer, int buffer_size);
//boolean connect_to_address_of_player(LPDIRECTPLAY4A direct_play, bytes player_address, DWORD player_address_size, int nth_address);
boolean describe_address_of_player(LPDIRECTPLAY4A direct_play, bytes player_address, DWORD player_address_size);
boolean connect_to_IP_address(LPDIRECTPLAY4A direct_play_object, string IP_address);

BOOL FAR PASCAL EnumConnections_callback(LPCGUID, VOID*, DWORD, LPCDPNAME, DWORD, VOID* );
BOOL FAR PASCAL EnumSessions_callback(LPCDPSESSIONDESC2 pdpsd, DWORD* pdwTimeout, DWORD dwFlags, VOID* pvContext);

boolean initialize_direct_play() {
	tt_running_direct_play = ini_int("Network","LongDistanceBirds",FALSE,0); // new on 140600 -- FALSE was TRUE prior to 220900 - broke some logging
	tt_save_nests_as_local = ini_int("Network","SaveNestsAndBirdsAsLocal",FALSE,0);
	if (tt_save_nests_as_local) {
		tt_running_direct_play = FALSE; // no point
	};
   if (!tt_running_direct_play) {
      tt_debug_direct_play = FALSE; // so don't get confusing warnings - new on 310702
      return(FALSE);
   };
#if TT_DEBUG_ON
	if (tt_debug_mode == 280400) {
		tt_error_file() << timeGetTime() << "  About to initialize_direct_play." << endl;
	};
#endif
	if (!initialize_ole()) { // updated 311204 !tt_coinitialized && FAILED(CoInitialize(NULL))) {
		return(FALSE);
	};
//	tt_coinitialized = TRUE; // new on 020401 -- commented out 311204
//	if (FAILED(result = CoCreateInstance(CLSID_DirectPlay, NULL, CLSCTX_ALL, IID_IDirectPlay4A, (VOID**) &direct_play))) {
//		tt_error_file() << "Initializing DirectPlay failed with error code = " << (int) result << endl;
//		return(FALSE);
//	};
	HRESULT result;
	if (FAILED(result = CoCreateInstance(CLSID_DirectPlayLobby, NULL,
                                        CLSCTX_INPROC_SERVER, IID_IDirectPlayLobby3A,
                                        (VOID**)&direct_play_lobby))) {
      tt_error_file() << "Initializing DirectPlayLobby failed with error code = " << (int) result << endl;
		return(FALSE);
	};
	 // is 20 a good number - isn't 29 a DirectPlay limit??
	tt_max_direct_play_queue_size = ini_int("Network","MaxDirectPlayQueueSize",FALSE,20);
	tt_max_pending_messages_sent_at_once = ini_int("Network","MaxPendingMessagesSentAtOnce",FALSE,20);
	tt_max_pending_message_count = ini_int("Network","MaxPendingMessagesSentAtOnce",FALSE,50);
	tt_debug_direct_play = ini_int("Network","LogNetworkEvents",FALSE,0);
//	if (direct_play_lobby == NULL) {
//		if (FAILED(result = direct_play->EnumConnections(&toontalk_GUID, EnumConnections_callback, NULL, 0))) {
//			 SAFE_RELEASE(direct_play);
//			 dp_err(result,"Enumerating DirectPlay connections failed.");
//			 return(FALSE);
//		};
//	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 280400) {
		tt_error_file() << timeGetTime() << "  Finished initialize_direct_play." << endl;
	};
#endif
	return(TRUE);
};

LPDIRECTPLAY4A create_direct_play_object(string IP_address) {
	if (!tt_running_direct_play) return(NULL);
#if TT_DIRECT_PLAY_WARNING
	loading_wait(TRUE,TRUE); // new on 270205
#endif
	if (direct_play_lobby == NULL) { // new on 020401
		initialize_direct_play();
	};
//	if (nth_address >= address_count) return(NULL);
#if TT_DEBUG_ON
	if (tt_debug_mode == 280400) {
		tt_error_file() << timeGetTime() << "  About to create_direct_play_object." << endl;
	};
#endif
	if (!initialize_ole()) { // updated 311204 // !tt_coinitialized && FAILED(CoInitialize(NULL))) { // new on 020401
#if TT_DIRECT_PLAY_WARNING
		done_waiting_for_load(); // new on 270205
#endif
		return(FALSE);
	};
//	tt_coinitialized = TRUE; // new on 020401 -- commented out 311204
	LPDIRECTPLAY4A direct_play_object = NULL;
	HRESULT result;
	if (FAILED(result = CoCreateInstance(CLSID_DirectPlay, NULL, CLSCTX_ALL, IID_IDirectPlay4A, (VOID**) &direct_play_object))) {
		tt_error_file() << "Initializing DirectPlay failed with error code = " << (int) result << endl;
#if TT_DIRECT_PLAY_WARNING
		done_waiting_for_load(); // new on 270205
#endif
		return(NULL);
	};
	if (IP_address != NULL) {
//		if (!connect_to_address_of_player(direct_play_object,player_address,player_address_size,nth_address)) {
			// new on 300400
		if (!connect_to_IP_address(direct_play_object,IP_address)) {
			SAFE_RELEASE(direct_play_object);
#if TT_DIRECT_PLAY_WARNING
			done_waiting_for_load(); // new on 270205
#endif
			return(NULL);
		};
	} else { 
		// should be a server - they don't need a connection IP address
		unsigned char address[1024]; // more than enough
		DWORD address_size = sizeof(address);
		if (FAILED(result = direct_play_lobby->CreateAddress(DPSPGUID_TCPIP,DPAID_INet,
																			  "",1,
																			  address,&address_size))) {
			dp_err(result,"Creating a DirectPlay address failed.");
#if TT_DIRECT_PLAY_WARNING
			done_waiting_for_load(); // new on 270205
#endif
			return(NULL);
		};
		if (FAILED(result = direct_play_object->InitializeConnection(address,0))) {
			dp_err(result,"Failed to make initialize a DirectPlay connection.");
			SAFE_RELEASE(direct_play_object);
			done_waiting_for_load(); // new on 270205
			return(NULL);
		};
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 280400) {
		tt_error_file() << timeGetTime() << "  Finished create_direct_play_object." << endl;
	};
#endif
#if TT_DIRECT_PLAY_WARNING
	done_waiting_for_load(); // new on 270205
#endif
	return(direct_play_object);
};

boolean connect_to_IP_address(LPDIRECTPLAY4A direct_play_object, string IP_address) {
//#if TT_DEBUG_ON
	if (tt_debug_direct_play) {
		tt_error_file() << "Trying to connect to " << IP_address << " at " << timeGetTime() << endl;
	};
//#endif
	unsigned char address[1024]; // more than enough
	DWORD address_size = sizeof(address);
	HRESULT result;
	if (FAILED(result = direct_play_lobby->CreateAddress(DPSPGUID_TCPIP,DPAID_INet,
																		  IP_address,strlen(IP_address)+1, // length includes terminator
																		  address,&address_size))) {
		dp_err(result,"Creating a DirectPlay address failed.");
		return(FALSE);
	};
	if (FAILED(result = direct_play_object->InitializeConnection(address,0))) {
		dp_err(result,append_strings("Failed to make connection to ",IP_address));
#if TT_DEBUG_ON
		if (tt_debug_mode == 280400) {
			tt_error_file() << timeGetTime() << "  Failed to connect_to_IP_address." << endl;
		};
#endif
		return(FALSE);
	};
	return(TRUE);
};

boolean host_a_new_direct_play_session(LPDIRECTPLAY4A direct_play, GUID *guid, Nest *nest, string name) {
	if (direct_play == NULL || !tt_running_direct_play) return(FALSE);
#if TT_DEBUG_ON
	if (tt_debug_mode == 280400) {
		tt_error_file() << timeGetTime() << "  About to host a session." << endl;
	};
#endif
	DPSESSIONDESC2	direct_play_session_description;
	ZeroMemory(&direct_play_session_description, sizeof(direct_play_session_description));
	direct_play_session_description.dwSize = sizeof(direct_play_session_description);
	direct_play_session_description.guidApplication = *guid; //toontalk_GUID;
	direct_play_session_description.lpszSessionNameA = name;
//	direct_play_session_description.dwMaxPlayers = 0; // no maximum - no need to set to 0 since already is set
	// The DPSESSION_KEEPALIVE flag keeps the session alive if players abnormally exit
	direct_play_session_description.dwFlags = DPSESSION_KEEPALIVE | DPSESSION_DIRECTPLAYPROTOCOL ; // | DPSESSION_MIGRATEHOST;
	// Create a new session
	HRESULT result;
	if (FAILED(result = direct_play->Open(&direct_play_session_description, DPOPEN_CREATE))) {
		if (result == DPERR_ALREADYINITIALIZED) {
			return(create_player(direct_play,TRUE,nest));
		} else {
			dp_err(result,"Creating a DirectPlay session failed.");
			return(FALSE);
		};
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 280400) {
		tt_error_file() << timeGetTime() << "  Finished hosting a session. (And about to create player.)" << endl;
	};
#endif
	return(create_player(direct_play,TRUE,nest));
};

boolean join_a_direct_play_session(LPDIRECTPLAY4A direct_play, GUID *guid, Nest *nest) { 
	if (direct_play == NULL || !tt_running_direct_play) return(FALSE);
#if TT_DEBUG_ON
	if (tt_debug_mode == 280400) {
		tt_error_file() << timeGetTime() << "  Trying to join a session." << endl;
	};
#endif
	DPSESSIONDESC2	direct_play_session_description;
	ZeroMemory(&direct_play_session_description, sizeof(DPSESSIONDESC2));
	direct_play_session_description.dwSize = sizeof(DPSESSIONDESC2);
	direct_play_session_description.guidApplication = *guid; //toontalk_GUID;
	// Enum sessions and let DirectPlay decide the timeout
   // Enumerate all the active DirectPlay games on the selected connection
	HRESULT result;
//	tt_error_file() << "About to enumerate at " << timeGetTime() << endl;
   if (FAILED(result = direct_play->EnumSessions(&direct_play_session_description, 0, EnumSessions_callback, nest, 
																 DPENUMSESSIONS_AVAILABLE | DPENUMSESSIONS_ASYNC | DPENUMSESSIONS_RETURNSTATUS))) { 
		// commented out | DPENUMSESSIONS_ASYNC since kept timing out
		dp_err(result,"Enumerating DirectPlay session failed.");
		return(FALSE);
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 280400) {
		tt_error_file() << timeGetTime() << "  Finished trying to join a session." << endl;
	};
#endif
	return(TRUE);
};

//	DPCAPS capabilities;
//	capabilities.dwSize = sizeof(capabilities);
//	direct_play->GetCaps(&capabilities,0); // just curious right now

boolean create_player(LPDIRECTPLAY4A direct_play, boolean host, Nest *nest, string player_name) {
   // Create player based on direct_play_player_name.  
   // Store the player's DPID in direct_play_player_id.
   // Also all DirectPlay messages for this player will signal direct_play_message_event - not currently true
	if (direct_play == NULL || !tt_running_direct_play) return(FALSE);
#if TT_DEBUG_ON
	if (tt_debug_mode == 280400) {
		tt_error_file() << timeGetTime() << "  About to create_player." << endl;
	};
#endif
	HRESULT result;
	DPNAME direct_play_player_name;
   ZeroMemory(&direct_play_player_name, sizeof(direct_play_player_name) );
   direct_play_player_name.dwSize = sizeof(direct_play_player_name);
   direct_play_player_name.lpszShortNameA = player_name; 
	DPID player_id;
   if (FAILED(result = direct_play->CreatePlayer(&player_id,
																 &direct_play_player_name, 
																 NULL, NULL, 0,
																 host?DPPLAYER_SERVERPLAYER:0))) {
		dp_err(result,"Creating a DirectPlay player failed.");
      return(FALSE);
	};
	nest->player_id_assigned(player_id,host);
#if TT_DEBUG_ON
	if (tt_debug_mode == 280400) {
		tt_error_file() << timeGetTime() << "  Finished create_player." << endl;
	};
#endif
	return(TRUE);
};

BOOL FAR PASCAL EnumSessions_callback(LPCDPSESSIONDESC2 const_session_description, DWORD* pdwTimeout, DWORD dwFlags, VOID* pvContext) {
	Nest *nest = (Nest *) pvContext;
	LPDPSESSIONDESC2 session_description = (LPDPSESSIONDESC2) const_session_description; // I assume this safe - it is needed because I cons the description
	if (dwFlags & DPESC_TIMEDOUT) {
//		log("DirectPlay timed out waiting to connect to a nest. Messages sent to that nest are lost. ");
//		nest->player_id_assigned(0,FALSE); // report error to nest
//		tt_error_file() << "Trying again to enumeration sessions at " << timeGetTime() << endl;
		tt_global_picture->do_after(nest->delay_before_another_attempt_to_join_session(),nest,JOIN_A_DIRECT_PLAY_SESSION_CALLBACK); // is 1/2 second a good amount??
      return(FALSE); // The enumeration has timed out, so stop the enumeration.
	};
//   new_session_info = new DPSessionInfo; 
//	ZeroMemory( new_session_info, sizeof(DPSessionInfo) );
// Copy the information into new_session_info
//   new_session_info->guidSession = session_description->guidInstance;
	LPDIRECTPLAY4A direct_play = nest->pointer_to_direct_play();
#if TT_DEBUG_ON
	if (tt_debug_mode == 280400) {
		tt_error_file() << timeGetTime() << "  About to open a connection to a session." << endl;
	};
#endif
	HRESULT result;
	if (FAILED(result = direct_play->Open(session_description, DPOPEN_JOIN))) {
//		if (result == DPERR_CONNECTIONLOST) ... - not sure how to recover...
		dp_err(result,"Joining a DirectPlay session failed.");
		return(FALSE);
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 280400) {
		tt_error_file() << timeGetTime() << "  Finished opening a connection to a session and about to create player." << endl;
	};
#endif
	create_player(direct_play,FALSE,nest);
#if TT_DEBUG_ON
	if (tt_debug_mode == 280400) {
		tt_error_file() << timeGetTime() << "  Finished creating a player." << endl;
	};
#endif
	return(FALSE);
};

void receive_messages() {
	if (!tt_running_direct_play) return;
	Sprites *remaining = tt_networked_nests;
	while (remaining != NULL) {
		Nest *nest = (Nest *) remaining->first();
		while (receive_next_message(nest)) {};
		remaining = remaining->rest();
	};
	// should only be system message now that is sent to proxies
	remaining = tt_nest_proxies;
	while (remaining != NULL) {
		Nest *nest = (Nest *) remaining->first();
		while (TRUE) { // new on 110500 to catch errors here
         if (tt_want_exception_breaks) {
			   if (!receive_next_message(nest)) break;
         } else {
			   try {
				   if (!receive_next_message(nest)) break;
			   } catch (...) {
				   log("Error occurred while trying to process a network message.",FALSE,TRUE);
				   tt_error_file() << " The message was to ";
				   nest->describe_networked_nest(tt_error_file());
				   tt_error_file() << endl;
				   tt_error_file() << " Message ignored." << endl;
			   };
         };
		};
		remaining = remaining->rest();
	};
};

bytes message_buffer = NULL;
DWORD message_buffer_size = 0;

boolean receive_next_message(Nest *nest) {
	// caller should not save the string returned here
	LPDIRECTPLAY4A direct_play = nest->pointer_to_direct_play();
	if (direct_play == NULL) return(FALSE); // new on 050500
	DPID from_id;
   DPID to_id;
	DWORD size = message_buffer_size;
	HRESULT result = direct_play->Receive(&from_id,&to_id,DPRECEIVE_ALL,message_buffer,&size);
	switch (result) {
		case DPERR_NOMESSAGES:
			return(FALSE);
		case DP_OK:
			if (from_id == DPID_SYSMSG) {
				process_system_message((DPMSG_GENERIC *) message_buffer,nest);
				return(TRUE);
			} else {
				process_bird_message(nest,message_buffer,size);
				return(TRUE);
			};
		case DPERR_BUFFERTOOSMALL:
			if (message_buffer != NULL) delete [] message_buffer;
			message_buffer = new BYTE[size];
			message_buffer_size = size;
			return(receive_next_message(nest));
		default:
			dp_err(result,"Receiving a DirectPlay message failed. ");
			return(FALSE);
	};
};

void process_system_message(DPMSG_GENERIC *message, Nest *nest) {
  if (tt_debug_direct_play) {
	  tt_error_file() << "Received a system message: ";
  };
  switch(message->dwType) { 
      case DPSYS_ADDGROUPTOGROUP:
        if (tt_debug_direct_play) {
				tt_error_file() << "An existing group has been added to an existing group. " << endl;
        };
        break;
      case DPSYS_ADDPLAYERTOGROUP:
        if (tt_debug_direct_play) {
				tt_error_file() << "An existing player has been added to an existing group. " << endl;
        };
        break;
		case DPSYS_CHAT:
			if (tt_debug_direct_play) {
				tt_error_file() << "A chat message has been received." << endl;
			};
			break;
		case DPSYS_CREATEPLAYERORGROUP:
        if (tt_debug_direct_play) {
				tt_error_file() << "A new player or group has been created. " << endl;
        };
        break;
      case DPSYS_DELETEGROUPFROMGROUP:
        if (tt_debug_direct_play) {
				tt_error_file() << "A group has been removed from a group. " << endl;
        };
        break;
      case DPSYS_DELETEPLAYERFROMGROUP:
        if (tt_debug_direct_play) {
				tt_error_file() << "A player has been removed from a group. " << endl;
        };
        break;
      case DPSYS_DESTROYPLAYERORGROUP:
        if (tt_debug_direct_play) {
				tt_error_file() << "An existing player or group has been destroyed. " << endl;
        };
//		  if ((((DPMSG_DESTROYPLAYERORGROUP *) message)->dwFlags)&DPPLAYER_SERVERPLAYER) { // nest has gone away so let proxy know 
		  {
         DPMSG_DESTROYPLAYERORGROUP *destroy_message = (DPMSG_DESTROYPLAYERORGROUP *) message;
			if (destroy_message->dpId == DPID_SERVERPLAYER) {
			   nest->drop_direct_play_object(); // new on 050500 - so next time a message is sent doesn't waste time trying
			};
		  };
		  break;
		case DPSYS_HOST:
			if (tt_debug_direct_play) {
				tt_error_file() << "The current host has left the session and this application is the new host." << endl;
			};
			break;
		case DPSYS_SECUREMESSAGE:
        if (tt_debug_direct_play) {
				tt_error_file() << "A digitally signed or encrypted message has been received. " << endl;
        };
        break;
      case DPSYS_SENDCOMPLETE:
        if (tt_debug_direct_play) {
				tt_error_file() << "An asynchronous send is complete. " << endl;
        };
        break;
      case DPSYS_SESSIONLOST:
        if (tt_debug_direct_play) {
				tt_error_file() << "The connection with the session has been lost. " << endl;
        };
        break;
      case DPSYS_SETGROUPOWNER:
        if (tt_debug_direct_play) {
				tt_error_file() << "The owner of the group has changed. " << endl;
        };
        break;
      case DPSYS_SETPLAYERORGROUPDATA:
        if (tt_debug_direct_play) {
				tt_error_file() << "Player or group data has changed. " << endl;
        };
        break;
      case DPSYS_SETPLAYERORGROUPNAME:
        if (tt_debug_direct_play) {
			tt_error_file() << "Player or group name has changed. " << endl;
        };
        break;
      case DPSYS_SETSESSIONDESC:
        if (tt_debug_direct_play) {
				tt_error_file() << "Session description has changed. " << endl;
        };
        break;
      case DPSYS_STARTSESSION:
        if (tt_debug_direct_play) {
				tt_error_file() << "Lobby server is requesting that a session be started. " << endl;
        };
        break;
		default:
			tt_error_file() << "An unrecognized system message was received." << endl;
  };
};

void get_item_to_nest(Sprite *item, Nest *nest) {
	if (nest->deleted()) { // could tell sender not to bother
		item->destroy(); // new on 030500
		return;
	} else if (nest->showing_on_a_screen()) {
		city_coordinate lly = -my_random(ideal_screen_height);
		Bird *bird = new Bird(-ideal_screen_width,lly,0,nest);
		item->move_to(-ideal_screen_width,lly);
		bird->add_follower(item,FALSE);
		bird->set_no_home_to_return_to(TRUE);
		nest->pointer_to_background()->add_item(bird);
		bird->receive_item_each(item,NULL,1000);
//		nest->increment_bird_count(); // new on 030500
	} else {
		nest->accept_contents(FALSE,item); // well it really is by a bird but not a local one
	};
	if (item->active()) { // new on 270301 to be consistent with local birds that never stop running the item
		item->recursively_activate_pictures(); 
	};
};

void process_bird_message(Nest *nest, bytes message_buffer, int buffer_size) {
	if (tt_debug_direct_play) {
		tt_error_file() << "Received a message that is " << buffer_size << " bytes long for ";
		nest->describe_networked_nest(tt_error_file());
		tt_error_file() << " at " << timeGetTime() << endl;
	};
	if (message_buffer[0] == first_message_version_number && 
		 message_buffer[1] == MESSAGE_FROM_BIRD_TO_NEST) { // ignore for now but needed for growth
		message_buffer += 2;
		buffer_size -= 2;
	} else {
		log("Warning: ToonTalk received a message from a different version of ToonTalk and so is ignoring it.",FALSE,TRUE);
		return;
	};
	Sprite *item = encoding_to_sprite(message_buffer+2,buffer_size-2,
												 message_buffer[0],(NaturalLanguage) message_buffer[1]); 
	// first byte is the version number
	if (item != NULL) {
		get_item_to_nest(item,nest);
		Sprites *remaining = nest->pointer_to_nest_copies();
		while (remaining != NULL) {
			Nest *nest = (Nest *) remaining->first();
			get_item_to_nest(item->copy(),nest);
			remaining = remaining->rest();
		}; 
//		log("Network message received but couldn't find the nest that is supposed to get it.");
//		item->destroy();
	} else {
		log("Network message received but couldn't reconstruct ToonTalk item.");
	};
};

//bytes address_buffer = NULL;
//DWORD address_buffer_size = 0;

string *get_IP_addresses(LPDIRECTPLAY4A direct_play, DPID player_id, unsigned char &address_count) {
	if (direct_play == NULL || !tt_running_direct_play) return(FALSE);
	byte address_buffer[1024]; // more than enough
	DWORD address_buffer_size = sizeof(address_buffer);
	DWORD size = address_buffer_size;
	HRESULT result = direct_play->GetPlayerAddress(player_id,address_buffer,&size);
	switch (result) {
		case DP_OK:
//			{
//				player_address_size = (unsigned short int) size; // if it is longer than 64K I'm in trouble anyway...
//				bytes result = new byte[size];
//				memcpy(result,address_buffer,size);
//				return(result);
//			};
			{
				int count;
				string *addresses = IP_addresses_of_player(address_buffer,size,count);
				address_count = count;
				return(addresses);
			};
//		case DPERR_BUFFERTOOSMALL:
//			if (address_buffer != NULL) delete [] address_buffer;
//			address_buffer = new BYTE[size];
//			address_buffer_size = size;
//			player_address_size = (unsigned short int) size;
//			return(get_IP_addresses(direct_play,player_id,address_count));
		default:
			dp_err(result,"Getting a DirectPlay player address failed. ");
			address_count = 0;
			return(NULL);
	};
};

/*
int connect_to_nth_address = 0;

BOOL FAR PASCAL EnumAddress_callback(REFGUID guidDataType, DWORD dwDataSize, LPCVOID lpData, LPVOID lpContext) {
	if (guidDataType == DPAID_INet) {
		if (connect_to_nth_address == 0) {
			connect_to_IP_address((LPDIRECTPLAY4A) lpContext, (string) lpData,dwDataSize);
			connect_to_nth_address = -1; // signal that it has done it
			return(FALSE);
		} else {
			connect_to_nth_address--;
			return(TRUE);
		};		
	} else {
		return(TRUE);
	};
};

boolean connect_to_address_of_player(LPDIRECTPLAY4A direct_play, bytes player_address, DWORD player_address_size, int nth_address) {
	connect_to_nth_address = nth_address;
	HRESULT result = direct_play_lobby->EnumAddress(EnumAddress_callback,player_address,player_address_size,direct_play);
	if (connect_to_nth_address >= 0) return(FALSE); // no more addresses
	if (FAILED(result)) {
		dp_err(result,"Failed to enumerate a DirectPlay address.");
		return(FALSE);
	};
	return(TRUE);
};
*/

string *IP_addresses = NULL;
int IP_address_count = 0;

BOOL FAR PASCAL EnumAddress_set_ip_address_callback(REFGUID guidDataType, DWORD dwDataSize, LPCVOID lpData, LPVOID lpContext) {
	if (guidDataType == DPAID_INet) {
		IP_address_count++;
		string new_IP_address = new char[dwDataSize];
		memcpy(new_IP_address,lpData,dwDataSize);
		string *new_IP_addresses = new string[IP_address_count];
		if (IP_addresses != NULL) {
			for (int i = 0; i < IP_address_count-1; i++) {
				new_IP_addresses[i] = IP_addresses[i];
			};
			delete [] IP_addresses;
		};
		new_IP_addresses[IP_address_count-1] = new_IP_address;
		IP_addresses = new_IP_addresses;
	};
	return(TRUE);
};

string *IP_addresses_of_player(bytes player_address, DWORD player_address_size, int &number_of_addresses) {
	if (direct_play_lobby == NULL) return(NULL); // new on 190201 for robustness (e.g. can be called by load_nest and it's mixed up)
	// updated on 020500 to deal with multiple IP addresses
	IP_address_count = 0;
	IP_addresses = NULL; // caller just deal with releasing memory
	HRESULT result = direct_play_lobby->EnumAddress(EnumAddress_set_ip_address_callback,player_address,player_address_size,NULL); // NULL was direct_play prior to 050500
	if (FAILED(result)) {
		dp_err(result,"Failed to enumerate a DirectPlay address.");
		return(NULL);
	};
	number_of_addresses = IP_address_count;
	return(IP_addresses);
};

BOOL FAR PASCAL EnumAddress_describe_callback(REFGUID guidDataType, DWORD dwDataSize, LPCVOID lpData, LPVOID lpContext) {
	if (guidDataType == DPAID_ComPort) {
		tt_error_file() << "Com port: ..." << endl;
	} else if (guidDataType == DPAID_INet) {
		tt_error_file() << "IP address: " << (string) lpData << endl;
	} else if (guidDataType == DPAID_INetPort) {
		tt_error_file() << "Network port: " << (int) lpData << endl;
	} else if (guidDataType == DPAID_LobbyProvider) {
		tt_error_file() << "Lobby provider: ..." << endl;
	} else if (guidDataType == DPAID_Modem) {
		tt_error_file() << "Modem: " << (string) lpData << endl;
	} else if (guidDataType == DPAID_Phone) {
		tt_error_file() << "Phone number: " << (string) lpData << endl;
	} else if (guidDataType == DPAID_ServiceProvider) {
		tt_error_file() << "Service provider: ..." << endl;
	} else if (guidDataType ==  DPAID_TotalSize) {
		tt_error_file() << "Total size: " << (DWORD) lpData << endl;
	} else if (guidDataType == DPAID_ModemW ) {
		tt_error_file() << "Wide modem: ..." << endl;
	} else if (guidDataType == DPAID_PhoneW) {
		tt_error_file() << "Wide Phone number: ..." << endl;
	} else if (guidDataType == DPAID_INetW) {
		tt_error_file() << "Wide IP address: ..." << endl;
	} else {
		tt_error_file() << "More stuff..." << endl;
	};
	return(TRUE);
};

boolean describe_address_of_player(LPDIRECTPLAY4A direct_play, bytes player_address, DWORD player_address_size) {
	tt_error_file() << "Description of a player: " << endl;
	HRESULT result = direct_play_lobby->EnumAddress(EnumAddress_describe_callback,player_address,player_address_size,direct_play);
	if (FAILED(result)) {
		dp_err(result,"Failed to enumerate a DirectPlay address.");
		return(FALSE);
	};
	tt_error_file() << "End of description of player." << endl;
	return(TRUE);
};

SendStatus send_network_message(LPDIRECTPLAY4A direct_play, DPID player_id, GUID *guid, bytes message, int message_length, Nest *nest_proxy) {
	if (player_id == NULL || guid == NULL || message == NULL) return(TT_SEND_FAIL);
	if (direct_play == NULL || !tt_running_direct_play) return(TT_SEND_OK); // lie so it isn't queued up (new on 140600)
	// should this check the outgoing queue first??
	HRESULT result = direct_play->SendEx(player_id,
													 DPID_SERVERPLAYER, // revisit when nests are copied
													 DPSEND_ASYNC|DPSEND_GUARANTEED|DPSEND_NOSENDCOMPLETEMSG, 
													 // I currently don't care about acknowledgement messages
													 message,message_length,
													 0,0,NULL,NULL);
	if (result != DPERR_PENDING && FAILED(result)) {
		if ((result == DPERR_INVALIDPLAYER || result == DPERR_CONNECTIONLOST) && nest_proxy != NULL) { // maybe the other guy went down and came back (new on 270400)
			// added DPERR_CONNECTIONLOST on 280400
			return(TT_SEND_RETRY);
		};
		dp_err(result,"Failed to send a DirectPlay message.");
		return(TT_SEND_FAIL);
	};
	if (tt_debug_direct_play) {
		tt_error_file() << "Sending a message of " << message_length << " bytes at " << timeGetTime() << endl;
		if (nest_proxy != NULL) {
			tt_error_file() << " Message was sent to ";
			nest_proxy->describe_networked_nest(tt_error_file());
			tt_error_file() << endl;
		};
	};
	return(TT_SEND_OK);
};

int message_queue_size(LPDIRECTPLAY4A direct_play) {
	if (direct_play == NULL || !tt_running_direct_play) return(0);
	DWORD number_of_message;
	HRESULT result = direct_play->GetMessageQueue(0,0,DPMESSAGEQUEUE_SEND,&number_of_message,NULL);
	if (FAILED(result)) { // included result != DPERR_PENDING &&  prior to 050500
		dp_err(result,"Failed to get message queue of DirectPlay.");
		return(0);
	};
	return((int) number_of_message); 
};

void release_direct_play_object(LPDIRECTPLAY4A direct_play) {
	if (direct_play == NULL || !tt_running_direct_play) return;
	direct_play->CancelMessage(0,0); // DirectPlay FAQ recommends this in case the recipients of these messages crashed and didn't clean up
	direct_play->Release();
};

void destroy_player(LPDIRECTPLAY4A direct_play, DPID player_id) {
	if (direct_play == NULL || !tt_running_direct_play) return;
	direct_play->DestroyPlayer(player_id);
};

void release_direct_play() {
	// release connections and other structures too...
//	SAFE_RELEASE(direct_play);
//	for (int i = 0; i < direct_play_object_count; i++) {
//		SAFE_RELEASE(direct_play_objects[i]);
//	};
	SAFE_RELEASE(direct_play_lobby);
	if (message_buffer != NULL) {
		delete [] message_buffer;
		message_buffer = NULL;
		message_buffer_size = 0;
	};
//	if (address_buffer != NULL) {
//		delete [] address_buffer;
//		address_buffer = NULL;
//		address_buffer_size = 0;
//	};
};

void dp_err_log(string message, string extra, boolean exit_afterwards);

void dp_err(HRESULT result, string extra, boolean exit_afterwards) {
	switch (result) {
	 case DPERR_ABORTED:
		dp_err_log("The operation was canceled before it could be completed. ",extra,exit_afterwards);
		break;
	 case DPERR_ACCESSDENIED:
		dp_err_log("The session is full, or an incorrect password was supplied. ",extra,exit_afterwards);
		break;
	 case DPERR_ACTIVEPLAYERS:
		dp_err_log("The requested operation cannot be performed because there are existing active players. ",extra,exit_afterwards);
		break;
	 case DPERR_ALREADYINITIALIZED:
		dp_err_log("This object is already initialized. ",extra,exit_afterwards);
		break;
	 case DPERR_APPNOTSTARTED:
		dp_err_log("The application has not been started yet. ",extra,exit_afterwards);
		break;
	 case DPERR_AUTHENTICATIONFAILED:
		dp_err_log("The password or credentials supplied could not be authenticated. ",extra,exit_afterwards);
		break;
	 case DPERR_BUFFERTOOLARGE:
		dp_err_log("The data buffer is too large to store. ",extra,exit_afterwards);
		break;
	 case DPERR_BUFFERTOOSMALL:
		dp_err_log("The supplied buffer is not large enough to contain the requested data. ",extra,exit_afterwards);
		break;
	 case DPERR_BUSY:
		dp_err_log("A message cannot be sent because the transmission medium is busy. ",extra,exit_afterwards);
		break;
	 case DPERR_CANCELFAILED:
		dp_err_log("The message could not be canceled, possibly because it is a group message that has already been to sent to one or more members of the group. ",extra,exit_afterwards);
		break;
	 case DPERR_CANCELLED:
		dp_err_log("The operation was canceled. ",extra,exit_afterwards);
		break;
	 case DPERR_CANNOTCREATESERVER:
		dp_err_log("The server cannot be created for the new session. ",extra,exit_afterwards);
		break;
	 case DPERR_CANTADDPLAYER:
		dp_err_log("The player cannot be added to the session. ",extra,exit_afterwards);
		break;
	 case DPERR_CANTCREATEGROUP:
		dp_err_log("A new group cannot be created. ",extra,exit_afterwards);
		break;
	 case DPERR_CANTCREATEPLAYER:
		dp_err_log("A new player cannot be created. ",extra,exit_afterwards);
		break;
	 case DPERR_CANTCREATEPROCESS:
		dp_err_log("Cannot start the application. ",extra,exit_afterwards);
		break;
	 case DPERR_CANTCREATESESSION:
		dp_err_log("A new session cannot be created. ",extra,exit_afterwards);
		break;
	 case DPERR_CANTLOADCAPI:
		dp_err_log("No credentials were supplied and the CryptoAPI package (CAPI) to use for cryptography services cannot be loaded. ",extra,exit_afterwards);
		break;
	 case DPERR_CANTLOADSECURITYPACKAGE:
		dp_err_log("The software security package cannot be loaded. ",extra,exit_afterwards);
		break;
	 case DPERR_CANTLOADSSPI:
		dp_err_log("No credentials were supplied, and the Security Support Provider Interface (SSPI) that will prompt for credentials cannot be loaded. ",extra,exit_afterwards);
		break;
	 case DPERR_CAPSNOTAVAILABLEYET:
		dp_err_log("The capabilities of the DirectPlay object have not been determined yet. This error will occur if the DirectPlay object is implemented on a connectivity solution that requires polling to determine available bandwidth and latency. ",extra,exit_afterwards);
		break;
	 case DPERR_CONNECTING:
		dp_err_log("The method is in the process of connecting to the network. The application should keep using the method until it returns DP_OK, indicating successful completion, or until it returns a different error. ",extra,exit_afterwards);
		break;
	 case DPERR_CONNECTIONLOST:
		dp_err_log("The service provider connection was reset while data was being sent. ",extra,exit_afterwards);
		break;
	 case DPERR_ENCRYPTIONFAILED:
		dp_err_log("The requested information could not be digitally encrypted. Encryption is used for message privacy. This error is only relevant in a secure session. ",extra,exit_afterwards);
		break;
	 case DPERR_ENCRYPTIONNOTSUPPORTED:
		dp_err_log("The type of encryption requested in the DPSECURITYDESC structure is not available. This error is only relevant in a secure session. ",extra,exit_afterwards);
		break;
	 case DPERR_EXCEPTION:
		dp_err_log("An exception occurred when processing the request. ",extra,exit_afterwards);
		break;
	 case DPERR_GENERIC:
		dp_err_log("An undefined error condition occurred. ",extra,exit_afterwards);
		break;
	 case DPERR_INVALIDFLAGS:
		dp_err_log("The flags passed to this method are invalid. ",extra,exit_afterwards);
		break;
	 case DPERR_INVALIDGROUP:
		dp_err_log("The group ID is not recognized as a valid group ID for this game session. ",extra,exit_afterwards);
		break;
	 case DPERR_INVALIDINTERFACE:
		dp_err_log("The interface parameter is invalid. ",extra,exit_afterwards);
		break;
	 case DPERR_INVALIDOBJECT:
		dp_err_log("The DirectPlay object pointer is invalid. ",extra,exit_afterwards);
		break;
	 case DPERR_INVALIDPARAMS:
		dp_err_log("One or more of the parameters passed to the method are invalid. ",extra,exit_afterwards);
		break;
	 case DPERR_INVALIDPASSWORD:
		dp_err_log("An invalid password was supplied when attempting to join a session that requires a password. ",extra,exit_afterwards);
		break;
	 case DPERR_INVALIDPLAYER:
		dp_err_log("The player ID is not recognized as a valid player ID for this game session. ",extra,exit_afterwards);
		break;
	 case DPERR_INVALIDPRIORITY:
		dp_err_log("The specified priority is not within the range of allowed priorities, which is inclusively 0-65535. ",extra,exit_afterwards);
		break;
	 case DPERR_LOGONDENIED:
		dp_err_log("The session could not be opened because credentials are required, and either no credentials were supplied, or the credentials were invalid. ",extra,exit_afterwards);
		break;
	 case DPERR_NOCAPS:
		dp_err_log("The communication link that DirectPlay is attempting to use is not capable of this function. ",extra,exit_afterwards);
		break;
	 case DPERR_NOCONNECTION:
		dp_err_log("No communication link was established. ",extra,exit_afterwards);
		break;
	 case DPERR_NOINTERFACE:
		dp_err_log("The interface is not supported. ",extra,exit_afterwards);
		break;
	 case DPERR_NOMESSAGES:
		dp_err_log("There are no messages in the receive queue. ",extra,exit_afterwards);
		break;
	 case DPERR_NONAMESERVERFOUND:
		dp_err_log("No name server (host) could be found or created. A host must exist to create a player. ",extra,exit_afterwards);
		break;
	 case DPERR_NONEWPLAYERS:
		dp_err_log("The session is not accepting any new players. ",extra,exit_afterwards);
		break;
	 case DPERR_NOPLAYERS:
		dp_err_log("There are no active players in the session. ",extra,exit_afterwards);
		break;
	 case DPERR_NOSESSIONS:
		dp_err_log("There are no existing sessions for this game. ",extra,exit_afterwards);
		break;
	 case DPERR_NOTLOBBIED:
		dp_err_log("Returned by the IDirectPlayLobby3::Connect method if the application was not started by using the IDirectPlayLobby3::RunApplication method, or if there is no DPLCONNECTION structure currently initialized for this DirectPlayLobby object. ",extra,exit_afterwards);
		break;
	 case DPERR_NOTLOGGEDIN:
		dp_err_log("An action cannot be performed because a player or client application is not logged on. Returned by the IDirectPlay4::Send method when the client application tries to send a secure message without being logged on. ",extra,exit_afterwards);
		break;
	 case DPERR_OUTOFMEMORY:
		dp_err_log("There is insufficient memory to perform the requested operation. ",extra,exit_afterwards);
		break;
	 case DPERR_PENDING:
		dp_err_log("Not an error, this return indicates that an asynchronous send has reached the point where it is successfully queued. See IDirectPlay4::SendEx for more information. ",extra,exit_afterwards);
		break;
	 case DPERR_PLAYERLOST:
		dp_err_log("A player has lost the connection to the session. ",extra,exit_afterwards);
		break;
	 case DPERR_SENDTOOBIG:
		dp_err_log("The message being sent by the IDirectPlay4::Send method is too large. ",extra,exit_afterwards);
		break;
	 case DPERR_SESSIONLOST:
		dp_err_log("The connection to the session has been lost. ",extra,exit_afterwards);
		break;
	 case DPERR_SIGNFAILED:
		dp_err_log("The requested information could not be digitally signed. Digital signatures are used to establish the authenticity of messages. ",extra,exit_afterwards);
		break;
	 case DPERR_TIMEOUT:
		dp_err_log("The operation could not be completed in the specified time. ",extra,exit_afterwards);
		break;
	 case DPERR_UNAVAILABLE:
		dp_err_log("The requested function is not available at this time. ",extra,exit_afterwards);
		break;
	 case DPERR_UNINITIALIZED:
		dp_err_log("The requested object has not been initialized. ",extra,exit_afterwards);
		break;
	 case DPERR_UNKNOWNAPPLICATION:
		dp_err_log("An unknown application was specified. ",extra,exit_afterwards);
		break;
	 case DPERR_UNKNOWNMESSAGE:
		dp_err_log("The message ID isn't valid. Returned from IDirectPlay4::CancelMessage if the ID of the message to be canceled is invalid. ",extra,exit_afterwards);
		break;
	 case DPERR_UNSUPPORTED:
		dp_err_log("The function or feature is not available in this implementation or on this service provider. Returned from IDirectPlay4::GetGroupConnectionSettings and IDirectPlay4::SetGroupConnectionSettings if they are called from a session that is not a lobby session. Returned from IDirectPlay4::SendEx if the priority or time-out is set, and these are not supported by the service provider and DirectPlay protocol is not on. Returned from IDirectPlay4::GetMessageQueue if you check the send queue and this is not supported by the service provider and DirectPlay protocol is not on. ",extra,exit_afterwards);
		break;
	 case DPERR_USERCANCEL:
		dp_err_log("Can be returned in two ways. 1) The user canceled the connection process during a call to the IDirectPlay4::Open method. 2) The user clicked Cancel in one of the DirectPlay service provider dialog boxes during a call to IDirectPlay4::EnumSessions. ",extra,exit_afterwards);
		break;
//	 case E_UNKNOWN:
//		dp_err_log("Usually indicates a catastrophic failure of some sort; for example, the service provider cannot initialize correctly or a protocol such as IPX is not installed correctly on the machine. ",extra,exit_afterwards);
//		break;
	 default:
		 dp_err_log("Invalid Direct Play error code. ",extra,exit_afterwards);
		 tt_error_file() << "Error code=" << result << endl; // new on 300700
	};
};

void dp_err_log(string message, string extra, boolean exit_afterwards) {
   log(message,FALSE,TRUE);
   log(extra);
   character full_message[512];
   output_string_stream stream(full_message,1024);
   stream << "Terminating due to DirectPlay Error. " << message << " " << extra;
   stream.put('\0');
   if (exit_afterwards) {
		tt_exit_failure(TROUBLE_SHOOT_DIRECTX,full_message);
	};
};

/*
BOOL FAR PASCAL EnumConnections_callback( LPCGUID   pguidSP, 
                                          VOID*     connection, 
                                          DWORD     dwConnectionSize, 
                                          LPCDPNAME pName, 
                                          DWORD     dwFlags, 
                                          VOID*     pvContext ) {
	if (!IsEqualGUID(*pguidSP,DPSPGUID_TCPIP)) { // only allow TCP/IP connections for now
		return(TRUE); // keep enumerating
	};
   // Test if the connection is available by attempting to initialize 
   // the connection
	HRESULT result;
   if (FAILED( result = direct_play->InitializeConnection( connection, 0 ) ) ) {
		 // Don't need the IDirectPlay interface anymore, so release it
       return(TRUE); // Unavailable connection, keep enumerating
   };
//	direct_play_connection = (DPLCONNECTION *) new BYTE[ dwConnectionSize ];
   // Store pointer to GUID 
//   memcpy(direct_play_connection, connection, dwConnectionSize );
   return(FALSE); // Stop enumerating
};
*/


#endif

