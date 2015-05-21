// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif   
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif   
#if !defined(__TT_WINMAIN_H)
#include "winmain.h"
#endif
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif 
#if !defined(__TT_CLEANUP_H)   
#include "cleanup.h"
#endif   
#if !defined(__TT_HELP_H)   
#include "help.h"
#endif   
#if !defined(__TT_CACHE_H)   
#include "cache.h"
#endif   

//#include <fstream>
//#include <strstrea.h> commented out 210703
#include <string.h>
//#include <ctype.h>
// for toupper -- could make sure only one case is in INI file -- but user edits it too

// Windows Header Files:
#include <windows.h>
#include <mmsystem.h>

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

unsigned short max_cache_entries;
unsigned short max_cache_index;
unsigned short cache_entries_in_use;
unsigned short cache_pointer;
unsigned long cache_memory_left;

void compute_cache_memory_max_size() { // abstracted on 260301
// compute following based upon how much memory is available
// it is lowered if memory is low
#if TT_32
   long memory_available = compute_memory_available(FALSE);
#else
	long memory_available = GetFreeSpace(0);
#endif
   // default is 25% of free space (or physical memory for TT32)
	tt_cache_memory_max_size = (memory_available/400)*tt_cache_percent;
#if TT_DEBUG_ON
	if (tt_debug_mode == 3 || tt_debug_mode == 8765) {
		tt_error_file() << "Maximum memory used for caching is " << tt_cache_memory_max_size
							 << " while free memory is " << memory_available << endl;
	};
#endif
};

void initialize_memory_cache() {
	max_cache_index = 0;
	cache_entries_in_use = 0;
	cache_pointer = 0;
	compute_cache_memory_max_size(); // abstracted on 260301
	max_cache_entries = (unsigned short int) (tt_cache_memory_max_size/5000); // typically 500 to 1000 entries
	if (max_cache_entries > 5000) { // was 2000 prior to 121099
		max_cache_entries = 5000;
	} else if (max_cache_entries < 100) {
		max_cache_entries = 100;
	};
	cache_entries = new CacheEntry[max_cache_entries];
	cache_memory_left = tt_cache_memory_max_size;
};

void shrink_memory_cache(int percent) {
	// percent of time recently spent compacting used as percent to shave off
	// not clear this is a good policy
	tt_cache_memory_max_size = ((100-percent)*tt_cache_memory_max_size)/100;
	string to_say = new character[256];
	output_string_stream message(to_say,256);
	message << "Warning. Memory is low. Memory used to speed things up shrunk by " << percent
			  << ". It is now " << tt_cache_memory_max_size << " bytes.";
	message.put('\0');
	say(to_say);
	delete [] to_say;
};
//flag cache_already_freed = FALSE;

//void sound_cache_just_freed() {
//   cache_already_freed = TRUE; // not quite true -- should track this down
//};

void free_memory_cache() {
//   if (tt_shutting_down && cache_already_freed) return;
//   cache_already_freed = TRUE;
#if TT_DEBUG_ON
	unsigned long total_savings = 0;
	if (tt_debug_mode == 3 || tt_debug_mode == 150500) {
		for (int i = 0; i < max_cache_entries; i++) { // was max_cache_index prior to 081102
			if (!cache_entries[i].is_free()) {
				tt_error_file() << "Cache #" << i 
									 << " used count = " << cache_entries[i].return_used_count()
									 << "; savings per use = " << cache_entries[i].return_savings_per_use()
									 << endl << " bytes used = " << cache_entries[i].return_memory_size()
									 << "; speed up = 1/" << cache_entries[i].speed_up_reciprocal()
									 << endl;
				if (cache_entries[i].return_savings_per_use() > 1) {
				  // if savings is 1 may be less than a millisecond
				  // subtract off 1 use for creation of cache
					total_savings += cache_entries[i].return_savings_per_use()*((long) cache_entries[i].return_used_count()-1);
				};
				cache_entries[i].free_bytes();
			};
		};
		tt_error_file() << "Memory used in cache " 
							 << (tt_cache_memory_max_size-cache_memory_left)
							 << " in " << cache_entries_in_use << " entries." << endl;
		tt_error_file() << "Total savings due to stuff currently in the cache is "
							 << total_savings/1000 << endl;
	};
#endif
//	for (int i = 0; i < max_cache_index; i++) { // new on 260301
//		cache_entries[i].free_bytes();
//	};
   // crashes sometimes when doing this with speech on
   // Win95 recovers the memory anyways...
	delete [] cache_entries;
};

boolean free_cache_index(unsigned long size,
								 unsigned long candidate_speed_up_reciprocal,
								 unsigned short &cache_index) {
  if (tt_cache_memory_max_size == 0) return(FALSE); // caching turned off
  if ((long) size > tt_cache_memory_max_size/32) { // new on 031000 so that no single entry can be more than 1/32 of total cache
	  // coerce to long on 121003 to avoid spurious compiler warning
	  return(FALSE);
  };
  boolean looped = FALSE;
  if (size <= cache_memory_left && cache_entries_in_use != max_cache_entries) {
	  while (TRUE) {
		  if (cache_entries[cache_pointer].is_free()) {
#if TT_DIRECTX
           cache_entries[cache_pointer].set_directx_type(NOT_DIRECT_DRAW_OBJECT);
#endif
			  cache_entries_in_use++;
			  if (max_cache_index < max_cache_entries) max_cache_index++;
			  cache_index = cache_pointer;
			  cache_memory_left -= size;
#if TT_DEBUG_ON
           if (tt_debug_mode == 81102) {
              tt_error_file() << cache_memory_left << " remaining cache. " << tt_frame_number << endl;
           };
#endif
			  return(TRUE);
		  };
		  cache_pointer++;
		  if (cache_pointer == max_cache_entries) {
			  if (looped) { // second time around
#if TT_DEBUG_ON
				  say_error(_T("ToonTalk has problems trying to find a free cache entry."));
#endif
				  cache_pointer = 0; // added 121099 because can crash otherwise
				  return(FALSE);
			  };
			  cache_pointer = 0;
			  looped = TRUE;
		  };
	  };
  };
  // sample n elements to find out approximately what constitutes
  unsigned long max_speed_up_reciprocal = bottom_percentile_speed_up(20);
  if (candidate_speed_up_reciprocal > max_speed_up_reciprocal) {
	  // don't bother since candidate is worse than what's there
	  return(FALSE);
  };
  long memory_needed = size-cache_memory_left;
  // memory_needed maybe be negative if there are no more free entries
  int loop_count = 0;
  int entries_freed = 0;
  for (unsigned short int i = (unsigned short int) my_random2(max_cache_index);
		 i <= max_cache_index; i++) {
	  if (i == max_cache_index) {
		  if (loop_count > 4) { // fifth time around
//			  tt_error_file() << "Looping searching for cache entries to free." << endl;
			  return(FALSE);
		  };
		  i = 0;
		  loop_count++; // new on 160799 so it tries again but isn't as fussy
		  max_speed_up_reciprocal = bottom_percentile_speed_up(20*(loop_count+1));
	  };
	  if (!cache_entries[i].is_free()) {
		  if (cache_entries[i].speed_up_reciprocal() > max_speed_up_reciprocal) {
#if TT_DEBUG_ON
			  if (tt_debug_mode == 3) {
				  tt_error_file() << "Freed cache #" << i
										<< " of size " << cache_entries[i].return_memory_size()
										<< " whose speed up is 1/" 
										<< cache_entries[i].speed_up_reciprocal()
										<< " and savings is " 
										<< cache_entries[i].return_savings_per_use()
										<< endl;
			  };
#endif	
			  memory_needed -= cache_entries[i].return_memory_size();
			  entries_freed++;
			  if (entries_freed > 10) { // new on 031000
				  return(FALSE); // not worth throwing away alot for just one new entry
			  };
			  cache_entries[i].free_entry(); // moved here on 260301 in case the conditional above runs
			  if (memory_needed <= 0) {
				  cache_index = i;
				  cache_memory_left -= size;
#if TT_DEBUG_ON
               if (tt_debug_mode == 81102) {
                  tt_error_file() << cache_memory_left << " remaining cache [2]. " << tt_frame_number << endl;
               };
#endif
				  return(TRUE);
			  };
		  };
	  };
	  cache_pointer = (unsigned short) ((cache_pointer+1)%max_cache_entries);	  
  };
  return(FALSE);
};

void free_all_cache_entries() {
	for (int i = 0; i < max_cache_entries; i++) { // prior to 081102 was max_cache_index
		if (!cache_entries[i].is_free() && 
			 !cache_entries[i].protected_entry()) { // unless protected
			cache_entries[i].free_entry();
		};
	};
	cache_entries_in_use = 0;
	compute_cache_memory_max_size(); // new on 260301
};

unsigned long bottom_percentile_speed_up(int percentile) {
	const int sample_size = 20;
	unsigned long samples[sample_size];
	int sample_count = 0;
	int i;
	boolean looped = FALSE;
	for (i = my_random2(max_cache_index); i <= max_cache_index; i++) {
		if (i == max_cache_index) {
			if (looped) {
				break;
			} else {
				i = 0;
				looped = TRUE;
			};
		};
		if (!cache_entries[i].is_free()) {
			samples[sample_count++] = cache_entries[i].speed_up_reciprocal();
			if (sample_count == sample_size) break;
		};
// following meant too much time was used searching unused entries	  
//	  cache_pointer = (cache_pointer+1)%max_cache_entries;	  
	};
	// bubble sort them
	for (i = 0; i < sample_count-1; i++) {
		if (samples[i] < samples[i+1]) { // out of order
			unsigned long temp = samples[i];
			samples[i] = samples[i+1];
			samples[i+1] = temp;
			if (i > 1) i -= 2;
		};
	};
	return(samples[(percentile*sample_count)/100]);
};

work_page allocate_bytes(long size 
#if !TT_32
,HGLOBAL &memory_handle
#endif 
							) {
#if !TT_32
	if (size > max_allocation) {
		// why did this have GMEM_ZEROINIT?? 
		memory_handle = global_alloc(GMEM_FIXED,size); 
		return((work_page) GlobalLock(memory_handle));
	};
	memory_handle = NULL;
#endif
	return(new unsigned char[size]);
};

#if TT_DEBUG_ON
int cache_entry_debug_count = 0;
#endif

CacheEntry::CacheEntry() {
	when_cached = 0;
	savings_per_use = 0;
	memory = NULL;
#if !TT_32
	memory_handle = NULL;
#endif
	memory_size = 0;
	used_count = 0;
	cache_id = 0;
#if TT_DIRECTX
   directx_type = NOT_DIRECT_DRAW_OBJECT;
#else
	protected_entry_flag = FALSE;
#endif
//	permutation = NULL;
#if TT_DEBUG_ON
	cache_entry_debug_counter = cache_entry_debug_count++;
	if (tt_debug_mode == 190301) {
		tt_error_file() << "Creating cache entry #" << cache_entry_debug_counter << " of type " << directx_type << endl;
	};
#endif
};

CacheEntry::~CacheEntry() {
//tt_error_file() << "Deleting a cache entry of size " << memory_size << endl;
	if (memory != NULL) free_bytes();
//	if (memory != NULL) delete [] memory;
};

void CacheEntry::free_entry() {
//tt_error_file() << "Freeing a cache entry of size " << memory_size << endl;
   cache_memory_left += memory_size;
#if TT_DEBUG_ON
   if (tt_debug_mode == 81102) {
      tt_error_file() << cache_memory_left << " remaining cache after free_entry. " << tt_frame_number << endl;
   };
#endif
   free_bytes();
   cache_id++;
};

#if TT_DIRECTX
boolean CacheEntry::protected_entry() {
   if (memory != NULL && directx_type == DIRECT_SOUND_BUFFER) {
      DWORD status;
      HRESULT result = ((sound_buffer) memory)->GetStatus(&status);
      return(result == DS_OK && status&DSBSTATUS_PLAYING); // protected since it is playing
   };
   return(FALSE);
};
#else
boolean CacheEntry::protected_entry() {
   return(protected_entry_flag);
};
#endif

void CacheEntry::free_bytes() {
  // this assumes caller has already checked that there is memory in this entry
	if (memory == NULL) {
//		say(S(IDS_CACHE_FREED_TWICE));
		return;
	};
//tt_error_file() << "Freeing bytes of a cache entry of size " << memory_size << endl;
#if TT_DEBUG_ON
	if (tt_debug_mode == 190301) {
		tt_error_file() << "Freeing bytes of cache entry #" << cache_entry_debug_counter << " of type " << directx_type << endl;
	};
#endif
#if TT_CAREFUL
	if (protected_entry() && !tt_shutting_down) { // !tt_shutting_down added 161299
//		tt_error_file() << "Freeing protected cache entry." << endl;
		say_error(IDS_FREEING_PROTECTED_CACHE);
		return;
	};
#endif
#if !TT_32
	if (memory_handle != NULL) { //memory_size > max_allocation) {
		GlobalUnlock(memory_handle);
		GlobalFree(memory_handle);
		memory = NULL;
		memory_handle = NULL;
		return;
	};
#endif
#if TT_DIRECTX
   switch (directx_type) {
      case DIRECT_SOUND_BUFFER:
         ((sound_buffer) memory)->Stop();
         ((sound_buffer) memory)->Release();
			if (tt_deleted_memory != NULL) {
				tt_deleted_memory->remove(memory); // new on 300105
			};
         break;
      case DIRECT_DRAW_SURFACE:
//#if TT_DEBUG_ON
//			// what about tt_debugging_leaks?
//			if (!about_to_quit()) { // this was here tt_debug_mode == 190301 &&  -- very bad leak -- maybe I meant tt_debug_mode != 190301 && -- luckily only happened with TT_DEBUG_ON
//				ULONG count = ((draw_surface) memory)->Release();
////				tt_error_file() << "Free bytes: ref count after release is " << count << " cache entry # " << cache_entry_debug_counter << " size is " << memory_size << endl;
//				if (count == 0) {
////					log("Zero ref count");
//				} else {
//               if (count > 1) { // otherwise not worth worrying about, right?
//						// what about when there are many copies of the same image out there?? - asked 290103
//                  log("A problem in CacheEntry::free_bytes() since something else is holding on to this surface");
//               };
//					while (((draw_surface) memory)->Release() > 0) { // new on 090401 to be sure it is really freed since might be causing problems resuming ToonTalk and getting
//						// the error: This surface cannot be restored because it was created in a different mode. 
//					}; 
//				};
//			};
//#else
			while (((draw_surface) memory)->Release() > 0) { // new on 090401 to be sure it is really freed since might be causing problems resuming ToonTalk and getting
				// the error: This surface cannot be restored because it was created in a different mode. 
			};
			if (tt_deleted_memory != NULL) {
				tt_deleted_memory->remove(memory); // new on 300105
			};
//#endif
         break;
      case NOT_DIRECT_DRAW_OBJECT:
         delete [] memory;
   };
   directx_type = NOT_DIRECT_DRAW_OBJECT;
   memory = NULL;
#else
	delete [] memory;
	memory = NULL;
#endif
	cache_entries_in_use--; // new on 160799
};

#if TT_DIRECTX
void CacheEntry::stop_sound() {
   if (memory != NULL && directx_type == DIRECT_SOUND_BUFFER) {
      ((sound_buffer) memory)->Stop();
   };
};

void free_surfaces_in_cache() {
   for (int i = 0; i < max_cache_entries; i++) { // prior to 081102
      cache_entries[i].free_surface();
   };
};

void CacheEntry::free_surface() {
   if (memory != NULL && directx_type == DIRECT_DRAW_SURFACE) {
      free_entry();
   };
};
#endif

unsigned long CacheEntry::speed_up_reciprocal() {
	if (protected_entry() || memory_size == 0) return(0); // don't release this while in use
	// memory_size can equal 0 if memory is low (disjunct added on 241001)
	const unsigned long savings = savings_per_use*used_count;
	const unsigned long duration = tt_current_time-when_cached;
	if (duration < savings) {
		// treat it as equal instead
		return(memory_size);
	};
#if TT_CAREFUL
	if (savings == 0) {
		say_error(IDS_CACHING_NON_SAVING);
		return(max_unsigned_long);
	};
#endif	
	const unsigned long duration_savings = duration/savings;
	const unsigned long result = duration_savings*memory_size;
	// duration_savings is reciprocal of time savings per time unit
	// since duration>savings it will be non-zero
	// alternative is to use floating point and let it range from 0 to 1
	// note if speed is > .5 then reciprocal will be 1 
	// ok since probably no cache item could have 50% speedup per byte used
	if (result/memory_size != duration_savings) {
		// overflow -- insignificant speed up
		return(max_unsigned_long);
	} else {
		return(result);
	};
};

DeletedMemory::~DeletedMemory() {
   if (next != NULL) delete next;
};

DeletedMemoryHandle::~DeletedMemoryHandle() {
#if TT_32
	delete [] memory;
#else
	if (memory_handle != NULL) { //size > max_allocation) {
		GlobalUnlock(memory_handle);
		GlobalFree(memory_handle);
	} else {
		delete [] memory;
	};
#endif
};

#if TT_DIRECTX
DeletedMemorySoundBuffer::~DeletedMemorySoundBuffer() {
   memory->Release();
};

boolean DeletedMemorySoundBuffer::has_member(void *deleted_memory) {
	if (deleted_memory == memory) {
		return(TRUE);
	} else if (next == NULL) {
		return(FALSE);
	} else {
		return(next->has_member(deleted_memory));
	};
};

boolean DeletedMemorySoundBuffer::remove(void *remove_memory) {
	if (remove_memory == memory) {
		memory = NULL;
		return(TRUE);
	} else if (next == NULL) {
		return(FALSE);
	} else {
		return(next->remove(remove_memory));
	};
};

DeletedMemoryDrawSurface::~DeletedMemoryDrawSurface() {
#if TT_DEBUG_ON
	MEMORYSTATUS memory_status;
	long memory_available_before;
   memory_status.dwLength = sizeof(MEMORYSTATUS);
	if (tt_debug_mode == 280204) {
		GlobalMemoryStatus(&memory_status);
	   memory_available_before = memory_status.dwAvailPageFile;
	};
	ULONG count = memory->Release();
	if (tt_debug_mode == 280204) {
		GlobalMemoryStatus(&memory_status);
		long memory_available_after = memory_status.dwAvailPageFile;
		tt_error_file() << "~DeletedMemoryDrawSurface"
							 << " and available memory grew by " << (memory_available_after-memory_available_before) << endl;
	};
	if (tt_debug_mode == 190301 || tt_debug_mode == 31001) {
		tt_error_file() << "DeletedMemoryDrawSurface: ref count after release is " << count << endl;
		if (count == 0) {
			log("Zero ref count");
		};
	};
#else
	memory->Release();
#endif
};

boolean DeletedMemoryDrawSurface::has_member(void *deleted_memory) {
	if (deleted_memory == memory) {
		return(TRUE);
	} else if (next == NULL) {
		return(FALSE);
	} else {
		return(next->has_member(deleted_memory));
	};
};

boolean DeletedMemoryDrawSurface::remove(void *remove_memory) {
	if (remove_memory == memory) {
		memory = NULL;
		return(TRUE);
	} else if (next == NULL) {
		return(FALSE);
	} else {
		return(next->remove(remove_memory));
	};
};

#endif

#if TT_DIRECTX
void new_sound_entry(int ) {
};
#else
void new_sound_entry(int new_entry) {
	if (new_entry == sound_entry_in_use) return;
	if (sound_entry_in_use >= 0) {
		cache_entries[sound_entry_in_use].protected_entry_flag = FALSE;
//      cache_entries[sound_entry_in_use].stop_sound();
	};
	sound_entry_in_use = new_entry;
	if (sound_entry_in_use >= 0) {
		cache_entries[sound_entry_in_use].protected_entry_flag = TRUE;
	};
};
#endif

const cost max_cost = 255; // unsigned char 

cost compute_savings_per_use(millisecond start_time) {
	millisecond savings = timeGetTime()-start_time;
	if (savings <= 0) { // negative after about a million seconds of use
		return(1); // round up if savings = 0
	} else if (savings > max_cost) {
		return(max_cost);
	} else {
		return((cost) savings);
	};
};




