// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved
#if !defined(__TT_CACHE_H)

#define __TT_CACHE_H

class CacheEntry {
  public:
	  CacheEntry();
	 ~CacheEntry();
	 boolean is_free() {
		 return(memory == NULL);
	 };
	 void free_bytes();
    void free_entry();
    boolean protected_entry();
#if TT_DIRECTX
    void stop_sound();
    void free_surface();
#endif
	 unsigned long speed_up_reciprocal();
	 unsigned short return_savings_per_use() {
		 return(savings_per_use);
	 };
	 void set_savings_per_use(unsigned short savings) {
		 savings_per_use = savings;
	 };
	 unsigned long return_memory_size() {
		 return(memory_size);
	 };
	 void set_memory_size(unsigned long size) {
		 memory_size = size;
	 };
	 unsigned long return_used_count() {
		 return(used_count);
	 };
	 void set_used_count(unsigned long count) {
		  used_count = count;
	 };
	 unsigned short return_cache_id() {
		 return(cache_id);
	 };
	 void set_cache_id(unsigned short id) {
		 cache_id = id;
	 };
	 void set_directx_type(DirectXObject type) {
		 directx_type = type;
#if TT_DEBUG_ON
		 if (tt_debug_mode == 190301) {
			 tt_error_file() << "Setting DirectX type of cache entry #" << cache_entry_debug_counter << " to " << type << endl;
		 };
#endif
	 };
	 work_page return_memory() {
		 return(memory);
	 };
	 void set_memory(work_page new_memory) {
		 memory = new_memory;
	 };
	 void initialize(millisecond candidate_when_cached, work_page new_memory, unsigned long size, unsigned short candidate_savings_per_use, unsigned long candidate_used_count) {
		 when_cached = candidate_when_cached;
		 memory = new_memory;
		 memory_size = size;
		 savings_per_use = candidate_savings_per_use;
		 used_count = candidate_used_count;
	 };
	 void set_when_cached(millisecond when) {
		 when_cached = when;
	 };
	 void increment_used_count() {
		 used_count++;
	 };
	 void increment_cache_id() {
		 cache_id++;
	 };
//	 bytes color_permutation() {
//		 return(permutation);
//	 };
//	 void set_color_permutation(bytes color_permutation) {
//		 permutation = color_permutation;
//	 };
#if TT_DEBUG_ON
	 int return_cache_entry_debug_counter() {
		 return(cache_entry_debug_counter);
	 };
#endif	 
  protected: // were public prior to 260301
	 millisecond when_cached;
	 work_page memory;
	 unsigned long memory_size;
	 unsigned short savings_per_use;
	 unsigned long used_count;
	 unsigned short cache_id;
#if TT_DIRECTX
    DirectXObject directx_type; // : 2;
#else
	 flag protected_entry_flag : 1;
#endif
#if !TT_32
	 HGLOBAL memory_handle;
#endif
#if TT_DEBUG_ON
	 int cache_entry_debug_counter;
#endif
//	 bytes permutation; // new on 200901 -- removed on 291101 since wrong modularity
};

class StretchBlitCacheEntry {
  public:
	 StretchBlitCacheEntry(unsigned short pixel_width,
								  unsigned short pixel_height,
								  unsigned short cache_id,
								  unsigned short cache_index) : // bytes color_permutation
		 pixel_width(pixel_width),
		 pixel_height(pixel_height),
		 cache_id(cache_id),
		 cache_index(cache_index) { // color_permutation(color_permutation)
	 };
	 unsigned short pixel_width;
	 unsigned short pixel_height;
	 unsigned short cache_id; // used to be sure the cache entry is still valid
	 unsigned short cache_index; // where to find the cache entry
//	 bytes color_permutation; // new on 200901
};

class DeletedMemory {
  public:
    DeletedMemory(DeletedMemory *next=NULL) :
       next(next) {};
    virtual ~DeletedMemory();
	 virtual boolean has_member(void *deleted_memory) = 0; // new on 300105
	 virtual boolean remove(void *remove_memory) = 0; // new on 300105
  protected:
     DeletedMemory *next;
};

class DeletedMemoryHandle : public DeletedMemory {
  public:
	DeletedMemoryHandle(work_page memory,
#if !TT_32
					        long size, HGLOBAL memory_handle,
#endif
					        DeletedMemory *next=NULL) :
	  memory(memory),
#if !TT_32	
	  size(size),
	  memory_handle(memory_handle),
#endif
    DeletedMemory(next) {};
	~DeletedMemoryHandle();
  private:
	work_page memory;
#if !TT_32	
	long size;
	HGLOBAL memory_handle;
#endif
};

// Following would be simpler if it were rewritten to use C++ templates

#if TT_DIRECTX
class DeletedMemorySoundBuffer : public DeletedMemory{
   public:
     DeletedMemorySoundBuffer(sound_buffer memory, DeletedMemory *next=NULL) :
      memory(memory),
      DeletedMemory(next) {};
     ~DeletedMemorySoundBuffer();
	  boolean has_member(void *deleted_memory); // new on 300105
	  boolean remove(void *remove_memory);
   private:
     sound_buffer memory;
};

class DeletedMemoryDrawSurface : public DeletedMemory{
   public:
     DeletedMemoryDrawSurface(draw_surface memory, DeletedMemory *next=NULL) :
      memory(memory),
      DeletedMemory(next) {};
     ~DeletedMemoryDrawSurface();
	  boolean has_member(void *deleted_memory); // new on 300105
	  boolean remove(void *remove_memory);
   private:
     draw_surface memory;
};
#endif

void initialize_memory_cache();
void shrink_memory_cache(int percent);
//void sound_cache_just_freed();
void free_memory_cache();
void free_all_cache_entries();
boolean free_cache_index(unsigned long memory_needed,
								 unsigned long candidate_speed_up_reciprocal,
								 unsigned short &cache_index);
unsigned long bottom_percentile_speed_up(int percentile);
void new_sound_entry(int new_entry=-1);
cost compute_savings_per_use(millisecond start_time);

#if TT_DIRECTX
void free_surfaces_in_cache();
#endif

typedef CacheEntry *CacheEntryPointer;
typedef StretchBlitCacheEntry *StretchBlitCacheEntryPointer;

#endif