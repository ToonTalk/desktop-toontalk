// Copyright (c) 1992-2005. Ken Kahn, Animated Programs, All rights reserved.

// Interface for zipping and unzipping files

#if !defined(__TT_DEFS_H)
#include "defs.h"
#endif
#if !defined(__TT_GLOBALS_H)
#include "globals.h"
#endif
#if !defined(__TT_UTILS_H)
#include "utils.h"
#endif
#if !defined(__TT_HELP_H)
#include "help.h"
#endif

#if !defined(__TT_ZIP_H)
#include "zip.h"
#endif

// completely rewritten on 260603 to use the Dyna Zip library

#if TT_DYNA_ZIP_STATIC
#include "C:\Program Files\DynaZip Max 5.0\samples\Static Lib\MS Visual C\Common\dzsll32.h"
#include "C:\Program Files\DynaZip Max 5.0\samples\Static Lib\MS Visual C\Common\duzsll32.h"
#else
#include "C:\Program Files\DynaZip Max 5.0\samples\DLL\Common\dunzdll.h"
#include "C:\Program Files\DynaZip Max 5.0\samples\DLL\Common\dzipdll.h"
#endif

ZIPCMDSTRUCT zip_commands;
char zip_items_list[MAX_PATH+2]; // 2 extra in case quoted - note this isn't enough room for multiple items

// info on compression levels:

//old (2.98) 975KB
//Took 2183ms to get wide text C:\Documents and Settings\Ken Kahn\My Documents\ToonTalk\Playground2001\Playground2001.xml.cty
//Took 3537ms to zip and save C:\Documents and Settings\Ken Kahn\My Documents\ToonTalk\Playground2001\Playground2001.xml.cty
//
//new default=5 1180KB
//Took 2042ms to get wide text C:\Documents and Settings\Ken Kahn\My Documents\ToonTalk\Playground2001\Playground2001.xml.cty
//Took 3514ms to zip and save C:\Documents and Settings\Ken Kahn\My Documents\ToonTalk\Playground2001\Playground2001.xml.cty
//
//default=9 930KB
//Took 2088ms to get wide text C:\Documents and Settings\Ken Kahn\My Documents\ToonTalk\Playground2001\Playground2001.xml.cty
//Took 3830ms to zip and save C:\Documents and Settings\Ken Kahn\My Documents\ToonTalk\Playground2001\Playground2001.xml.cty
//
//default=10 1433KB
//Took 2096ms to get wide text C:\Documents and Settings\Ken Kahn\My Documents\ToonTalk\Playground2001\Playground2001.xml.cty
//Took 3002ms to zip and save C:\Documents and Settings\Ken Kahn\My Documents\ToonTalk\Playground2001\Playground2001.xml.cty
//
//default=20 679KB
//Took 2139ms to get wide text C:\Documents and Settings\Ken Kahn\My Documents\ToonTalk\Playground2001\Playground2001.xml.cty
//Took 4605ms to zip and save C:\Documents and Settings\Ken Kahn\My Documents\ToonTalk\Playground2001\Playground2001.xml.cty


void init_zip_commands() { 
	ZeroMemory(&zip_commands, sizeof(ZIPCMDSTRUCT)); 
	zip_commands.zipStructSize = sizeof(ZIPCMDSTRUCT);
	zip_commands.compFactor = default_compression_level; // not really needed here
	zip_commands.quietFlag = TRUE; // except when debugging this might be worth turning off
	zip_commands.lpszItemList = zip_items_list;
	zip_commands.dontCompressTheseSuffixesFlag = TRUE;
	zip_commands.lpszStoreSuffixes = "PNG:GIF:JPG"; // no point compressing these - I think it is case insensitive - new on 210703
//#if TT_DEBUG_ON
//	zip_commands.bDiagnostic = TRUE;
//#endif
};

void initialize_zip() {
#if TT_DYNA_ZIP_STATIC
	DzipProcessInit(tt_main_instance);
	DzipThreadInit(); // assumes that only one thread uses this
#endif
	init_zip_commands();
};

UNZIPCMDSTRUCT unzip_commands;
ZIPINFOEX unzip_info;
char unzip_file_spec[MAX_PATH+2]; // 2 extra in case quoted - note this isn't enough room for multiple items

void init_unzip_commands() { 
	ZeroMemory(&unzip_commands, sizeof(UNZIPCMDSTRUCT)); 
	unzip_commands.unzipStructSize = sizeof(UNZIPCMDSTRUCT); 
	unzip_commands.index = -1; 
	unzip_commands.noDirectoryNamesFlag = TRUE; 
	unzip_commands.recurseFlag = TRUE; 
	unzip_commands.noDirectoryItemsFlag = TRUE;
	unzip_commands.quietFlag = TRUE; // except when debugging this might be worth turning off
	ZeroMemory(&unzip_info,sizeof(ZIPINFOEX));
	unzip_info.ulSize = sizeof(ZIPINFOEX);
	unzip_commands.pZinfoEx = &unzip_info;
	unzip_commands.lpszFilespec = unzip_file_spec;
};

void initialize_unzip() {
#if TT_DYNA_ZIP_STATIC
	DunzipProcessInit(tt_main_instance);
	DunzipThreadInit();
#endif
	init_unzip_commands();
};

void uninitialize_unzip() { // uninitialize zip and unzip had their names switched prior to 100404
#if TT_DYNA_ZIP_STATIC
	DunzipThreadDeInit();
	DunzipProcessDeInit();
#endif
};

void uninitialize_zip() {
#if TT_DYNA_ZIP_STATIC
	DzipThreadDeInit();
	DzipProcessDeInit();
#endif
};

string open_archive_to_zip(string archive_file_name) {
	return(copy_string(archive_file_name));
};

unsigned long wide_text_length_left = 0; // easier to communicate to callback with globals like this - could package up instead
//const int narrow_chunk_length = 16384;
//const unsigned int wide_chunk_length = narrow_chunk_length/sizeof(wide_character);
wide_string wide_node_text_left = NULL;

void FAR PASCAL zip_wide_string_callback(long lAction, 
														LPVOID lpMemBuf, 
														DWORD *pdwSize, 
														DWORD dwTotalReadL, 
														DWORD dwTotalReadH, 
														DWORD dwTotalWrittenL, 
														DWORD dwTotalWrittenH, 
														LPVOID lpUserData,
														long *plRet) {
	if (lAction == MEM_READ_DATA) {
		// trying CP_ACP as experiment on 090403 was CP_UTF8
		unsigned long buffer_size = *pdwSize;
		unsigned long wide_chunk_size = min(wide_text_length_left,buffer_size); // this assumes each wide character maps to a single narrow one
		int narrow_characters_written = WideCharToMultiByte(CP_ACP,0,wide_node_text_left,wide_chunk_size,(LPSTR) lpMemBuf,buffer_size,NULL,NULL);
		if (narrow_characters_written == 0) { // try again but with pessimistically - allowing each wide to become 2 narrow characters
			wide_chunk_size /= sizeof(wide_character);
			narrow_characters_written = WideCharToMultiByte(CP_ACP,0,wide_node_text_left,wide_chunk_size,(LPSTR) lpMemBuf,buffer_size,NULL,NULL);
		};
		*pdwSize = narrow_characters_written;
		wide_text_length_left -= wide_chunk_size;
		if (wide_text_length_left == 0) { 
			*plRet = MEM_DONE;
		} else {
			*plRet = MEM_CONTINUE;
			wide_node_text_left += wide_chunk_size;
		};
	} else {
		*plRet = MEM_ERROR;
	};
};

CALLBACKSTRUCT write_wide_string_callback_structure;

boolean write_wide_string_to_zip_file(string archive, string file_name, wide_string node_text, int total_length, int compression_level) { // new on 290603
	zip_commands.lpszZIPFile = archive;
	zip_commands.function = ZIP_MEMTOFILE_STREAM;
	quote_file_name(file_name,zip_items_list);
	zip_commands.compFactor = compression_level;
	zip_commands.growExistingFlag = TRUE;
	wide_text_length_left = total_length;
	wide_node_text_left = node_text;
	write_wide_string_callback_structure.dwSize = sizeof(CALLBACKSTRUCT);
	write_wide_string_callback_structure.lpMemToMemProc = (PFNMEMTOMEMCALLBACK) zip_wide_string_callback; // not clear why FARPROC coercion didn't work
	zip_commands.lpCallbackStruct = &write_wide_string_callback_structure;
//	UnwindProtect<boolean> set(tt_ok_to_process_windows_messages,TRUE); // new on 010305 since needed sometimes by zip DLL
	int result = dzip(&zip_commands);
	if (result == ZE_WRITE) { // new on 300904 -- device probably full 
		// Output file write failure, possible disk full - This error can occur when a file write operation is performed on a ZIP file 
		// that is locked or restricted by another process.
		unable_to_write_file(archive);
	};
	return(result == ZE_OK);
};

void open_zip_file(string archive, string file_name, int compression_level) {
	zip_commands.lpszZIPFile = archive;
	zip_commands.function = ZIP_ADD;
	quote_file_name(file_name,zip_items_list);
	zip_commands.compFactor = compression_level;
	zip_commands.growExistingFlag = TRUE;
	// commented out since no need to do this until really writing to the file
	//int result = dzip(&zip_commands);
	//if (result != ZE_OK) {
	//	delete [] zip_commands.lpszItemList;
	//	return(NULL);
	//};
//	return(zip_commands.lpszItemList);
};

//void open_zip_file_no_compression(string archive, string file_name) {
//	zip_commands.lpszZIPFile = archive;
//	zip_commands.function = ZIP_ADD;
//	quote_file_name(file_name,zip_items_list);
////	zip_commands.lpszItemList = copy_string(file_name);
//	zip_commands.compFactor = 0;
//	//if (dzip(&zip_commands) != ZE_OK) {
//	//	delete [] zip_commands.lpszItemList;
//	//	return(NULL);
//	//};
//};

boolean delete_zip_file(string archive, string file_name) {
	zip_commands.lpszZIPFile = archive;
	zip_commands.function = ZIP_DELETE;
	quote_file_name(file_name+short_file_name_start(file_name),zip_items_list); // new on 280803 so only delete the short name
//	zip_commands.lpszItemList = quote_file_name(file_name);
	//if (archive == tt_log_out_archive) { // new on 080703 - poor modularity?? -- obsolete 160204
	//	tt_log_out_archive_size--;
	//};
//	UnwindProtect<boolean> set(tt_ok_to_process_windows_messages,TRUE); // new on 010305 since needed sometimes by zip DLL
	int result = dzip(&zip_commands);
	return(result == ZE_OK);
};

boolean write_zip_file(string archive, bytes buffer, unsigned int length, int compression_level) {
	zip_commands.lpszZIPFile = archive;
	zip_commands.function = ZIP_MEMTOFILE;
	zip_commands.lpMemBlock = buffer;
	zip_commands.lMemBlockSize = length;
	zip_commands.compFactor = compression_level; // new on 190703
//	UnwindProtect<boolean> set(tt_ok_to_process_windows_messages,TRUE); // new on 010305 since needed sometimes by zip DLL
	int result = dzip(&zip_commands);
	if (result == ZE_WRITE) { // new on 300904 -- device probably full 
		// Output file write failure, possible disk full - This error can occur when a file write operation is performed on a ZIP file 
		// that is locked or restricted by another process.
		unable_to_write_file(archive);
	};
	zip_commands.lpMemBlock = NULL;
	return(result == ZE_OK);
};

//boolean zip_wide_string_to_index(wide_string node_text, int text_length, string file_name) {
//	zip_commands.lpszItemList = file_name;
//	zip_commands.function = ZIP_MEMTOFILE;
//	zip_commands.lpMemBlock = (bytes) node_text;
//	zip_commands.lMemBlockSize = length*sizeof(wide_character);
//	int result = dzip(&zip_commands);
//	zip_commands.lpszItemList = zip_items_list; // restore things
//	zip_commands.lpMemBlock = NULL;
//	return(result == ZE_OK);
//};

int FAR PASCAL RenameCallback(DZRENAME *rename_structure, void *lpUserData) {
	if (lpUserData != NULL) {
		strcpy(rename_structure->Name,(string) lpUserData);
	};
	return(0);
};

boolean zip_file(string archive, string source, string destination, int compression_level, boolean delete_original, boolean quote_source) { 
	// new on 280603
//#if TT_DEBUG_ON
//	destination = NULL; // experiment 210703
//#endif
	if (source == NULL) {
		return(FALSE);
	};
	zip_commands.lpszZIPFile = archive;
	zip_commands.function = ZIP_ADD;
//	strcpy(zip_items_list,source);
	if (quote_source) { // conditional new on 210803 since could be a list of file names now
		quote_file_name(source,zip_items_list);
	} else {
		strcpy(zip_items_list,source);
	};
	zip_commands.compFactor = compression_level;
	//const ZSO_REALTIVEPATHFLAG = 1;
	//zip_commands.wZipSubOptions = ZSO_REALTIVEPATHFLAG; // destination ignored but should be the relative path of the source
	//zip_commands.includeOnlyFollowingFlag = TRUE;
	//zip_commands.lpszIncludeFollowing = "Media\\*.*"; // make this an arg
	//zip_commands.recurseFlag = TRUE;
	if (destination == NULL) {
		zip_commands.noDirectoryNamesFlag = TRUE;
	} else {
		zip_commands.wZipSubOptions = 1; // ZSO_RELATIVEPATHFLAG;
		zip_commands.recurseFlag = TRUE;
		zip_commands.includeOnlyFollowingFlag = TRUE;
		zip_commands.lpszIncludeFollowing = destination;
//		quote_file_name("C:\\Documents and Settings\\All Users\\Documents\\ToonTalk\\*.*",zip_items_list);
		// experiment 210703
		//zip_commands.lpRenameProc = (FARPROC) RenameCallback;
		//zip_commands.lpRenameUserData = "Media/"; // experiment 210703 was destination;
	};
	zip_commands.deleteOriginalFlag = delete_original;
//	UnwindProtect<boolean> set(tt_ok_to_process_windows_messages,TRUE); // new on 010305
	int result = dzip(&zip_commands);
// tt_error_file() << archive << " " << source << " " << (destination || "no destination") << endl; // temp 201005
// tt_error_file() << result << endl; // temp 201005
	if (result != ZE_OK) {
#if TT_DEBUG_ON
		if (result == ZE_NONE) {
			tt_error_file() << "Nothing to zip. Source is " << (source||"NULL") << endl;
		};
#endif
		if (result != ZE_NONE) { // new on 020504
			// updated on 280704 to output NULL if one is missing
#if TT_DEBUG_ON
			if (source == NULL) debug_this();
#endif
			tt_error_file() << "Zipping failed. Source is " << (source||"NULL") << ", archive is " << (archive||"NULL") << ", and error code is " << result << endl;
		} else if (result == ZE_WRITE) { // new on 300904 -- device probably full 
			// Output file write failure, possible disk full - This error can occur when a file write operation is performed on a ZIP file 
			// that is locked or restricted by another process.
			unable_to_write_file(archive);
		};
	};
	zip_commands.compFactor = default_compression_level; // not clear if it is worth resetting this...
	if (destination == NULL) {
		zip_commands.noDirectoryNamesFlag = FALSE; 
	} else {
		//zip_commands.lpRenameProc = 0;
		//zip_commands.lpRenameUserData = 0;
		zip_commands.wZipSubOptions = 0;
		zip_commands.recurseFlag = FALSE;
		zip_commands.includeOnlyFollowingFlag = FALSE;
		zip_commands.lpszIncludeFollowing = NULL;
	};
	zip_commands.deleteOriginalFlag = FALSE;
	return(result == ZE_OK);
};

void close_zip_file(string ignore_archive) {
	zip_commands.lpMemBlock = NULL;
	zip_commands.compFactor = default_compression_level;
	//if (zip_commands.lpszItemList != NULL) {
	//	delete [] zip_commands.lpszItemList;
	//	zip_commands.lpszItemList = NULL;
	//};	
};

void close_zip_archive(zipFile archive) {
	delete [] archive;
};


// now for interface to unzip functionality

unzFile open_archive_to_unzip(string archive_file_name) {
	if (archive_file_name == NULL || !file_exists(archive_file_name)) return(NULL); // new on 150903
	return(copy_string(archive_file_name));
};

void close_unzip_archive(string archive) {
	delete [] archive;
};

void go_to_first_file_in_unzip_archive(string archive) {
	unzip_commands.lpszZIPFile = archive;
	unzip_commands.index = 0;
	unzip_commands.lStartingOffset = 0;
};

void go_to_next_file_in_unzip_archive(string archive) {
	unzip_commands.lpszZIPFile = archive;
	unzip_commands.index++;
	unzip_commands.lStartingOffset = 0;
};

void go_to_named_file_in_unzip_archive(string archive, string file_name) {
	unzip_commands.lpszZIPFile = archive;
	quote_file_name(file_name,unzip_file_spec);
//	unzip_commands.lpszFilespec = copy_string(file_name); // no need to quote - copy not necessary if caller is more clever - new on 280603
	unzip_commands.lStartingOffset = 0; // new on 070103
//	return(unzip_commands.lpszFilespec);	
};

void open_current_file_in_unzip_archive(unzFile archive) {
  unzip_commands.lStartingOffset = 0; // new on 070103 // more to do?
};

string open_named_file_in_unzip_archive(unzFile archive, string file_name) { // new on 250603
	// returned boolean prior to 110104
//	unzip_commands.lpszFilespec = copy_string(file_name); // no need to quote - copy not necessary if caller is more clever - new on 280603
//	quote_file_name(file_name,unzip_file_spec);
	unzip_commands.lpszZIPFile = archive;
	unzip_commands.lpszFilespec = file_name;
	unzip_commands.lStartingOffset = 0; // new on 070103
	unzip_commands.function = UNZIP_COUNTNAMEDZIPMEMBERS;
//	UnwindProtect<boolean> set(tt_ok_to_process_windows_messages,TRUE); // new on 010305 since needed sometimes by zip DLL -- redone on 050305
	int result = dunzip(&unzip_commands);
	if (result != UE_OK) {
		return(NULL); // as of 110104 returns a boolean indicating success -- updated 110104
	};
	unzip_commands.function = UNZIP_GETNEXTNAMEDZIPINFO; // should only be 1 that matches file_name so go with first one
	unzip_commands.pZinfoEx = &unzip_info; // not clear why this needs to be reset
	result = dunzip(&unzip_commands);
	unzip_commands.lpszFilespec = NULL; // just use the index now (beginning 090703)
	unzip_commands.index = unzip_info.ulindex;
	if (result == UE_OK) { // new on 110104
		return(unzip_info.szFileName);
	} else {
		return(NULL);
	};
};

void close_current_file_in_unzip_archive(unzFile archive) {
	//if (unzip_commands.lpszFilespec != NULL) {
	//	delete [] unzip_commands.lpszFilespec;
	//	unzip_commands.lpszFilespec = NULL;
	//};
};

boolean last_read_not_enough_bytes = FALSE;

int read_current_file_in_unzip_archive(string archive, void *buffer, unsigned int length) {
	unzip_commands.lpszZIPFile = archive;
//	unzip_commands.lpszFilespec = quoted_file_name;
	unzip_commands.function = UNZIP_FILETOMEM;
	unzip_commands.lpMemBlock = buffer;
	unzip_commands.lMemBlockSize = length;
#if TT_DEBUG_ON
	millisecond start;
	if (tt_debug_mode == 90703) {
		start = timeGetTime();
	};
#endif
//	UnwindProtect<boolean> set(tt_ok_to_process_windows_messages,TRUE); // new on 010305 since needed sometimes by zip DLL
	int result = dunzip(&unzip_commands);
#if TT_DEBUG_ON
	if (tt_debug_mode == 90703) {
		tt_error_file() << (timeGetTime()-start) << "ms to read " << length << " bytes from " << unzip_commands.index << " on frame " << tt_frame_number << endl;
	};
#endif
	if (result == UE_OK) {
		last_read_not_enough_bytes = FALSE;
		unzip_commands.lStartingOffset += length;
		return(length);
	} else {
		last_read_not_enough_bytes = TRUE;
		return(0); // didn't read anything
	};
};

int tellg_current_file_in_unzip_archive() {
	return(unzip_commands.lStartingOffset);
};

void seekg_current_file_in_unzip_archive(int position) {
	unzip_commands.lStartingOffset = position;
};

//int count_all_zip_members(string archive) { // new on 080703 -- obsolete 160204
//	unzip_commands.lpszZIPFile = archive;
//	unzip_commands.function = UNZIP_COUNTALLZIPMEMBERS;
//	int result = dunzip(&unzip_commands);
//	if (result != UE_OK) {
//		return(0);
//	};
//	return(unzip_commands.returnCount);
//};

int get_unzip_info_for_file(string archive, string file_name) {
	unzip_commands.lpszZIPFile = archive;
//	char saved_unzip_file_spec[MAX_PATH+2];
//	memcpy(saved_unzip_file_spec,unzip_file_spec,MAX_PATH+2); // new on 080703
	unzip_commands.lpszFilespec = file_name;
	int saved_index = unzip_commands.index;
	unzip_commands.index = -1;
//	quote_file_name(file_name,unzip_file_spec); // not needed when file is used within this scope
//	unzip_commands.lpszFilespec = file_name;
	unzip_commands.function = UNZIP_COUNTNAMEDZIPMEMBERS;
//	UnwindProtect<boolean> set(tt_ok_to_process_windows_messages,TRUE); // new on 010305 since needed sometimes by zip DLL
	int result = dunzip(&unzip_commands);
	if (result != UE_OK) {
		unzip_commands.index = saved_index;
		return(min_long);
	};
	unzip_commands.function = UNZIP_GETNEXTNAMEDZIPINFO; // should only be 1 that matches file_name so go with first one
	unzip_commands.pZinfoEx = &unzip_info; // not clear why this needs to be reset
	result = dunzip(&unzip_commands);
	if (result != UE_OK) {
		unzip_commands.index = saved_index;
		unzip_commands.lpszFilespec = NULL; // unzip_file_spec; // restore things
		return(min_long);
	};
	return(saved_index);
};

string narrow_contents_of_current_file_in_archive(string archive, string file_name) {
	int saved_index = get_unzip_info_for_file(archive,file_name);
	if (saved_index == min_long) {
		return(NULL);
	};
	string contents = new char[unzip_info.uloSizeLowPart+1];
	unzip_commands.function = UNZIP_FILETOMEM;
	unzip_commands.lpMemBlock = contents;
	unzip_commands.lMemBlockSize = unzip_info.uloSizeLowPart;
	int saved_starting_offset = unzip_commands.lStartingOffset;
	unzip_commands.lStartingOffset = 0;
//	UnwindProtect<boolean> set(tt_ok_to_process_windows_messages,TRUE); // new on 010305 since needed sometimes by zip DLL
	int result = dunzip(&unzip_commands);
	unzip_commands.index = saved_index;
	unzip_commands.lpszFilespec = NULL; // unzip_file_spec; // restore things
	unzip_commands.lStartingOffset = saved_starting_offset;
	if (result != UE_OK) {
		tt_error_file() << "Error code was " << result << " trying to extract " << file_name << " from " << archive << endl; // new on 140404
		return(NULL);
	};
	contents[unzip_info.uloSizeLowPart] = '\0';
	return(contents);
};

int get_indexed_unzip_info(string archive, int index) {
	unzip_commands.lpszZIPFile = archive;
	int saved_index = unzip_commands.index;
	unzip_commands.index  = index;
	unzip_commands.lpszFilespec = NULL;
	unzip_commands.function = UNZIP_GETINDEXEDZIPINFO; 
	unzip_commands.pZinfoEx = &unzip_info; // not clear why this needs to be reset
//	UnwindProtect<boolean> set(tt_ok_to_process_windows_messages,TRUE); // new on 010305 since needed sometimes by zip DLL
	int result = dunzip(&unzip_commands);
	if (result != UE_OK) {
		unzip_commands.lpszFilespec = unzip_file_spec; // restore things
		return(min_long);
	};
	return(saved_index);
};

string narrow_contents_of_index_file_in_archive(string archive, int index) {
	int saved_index = get_indexed_unzip_info(archive,index);
	if (saved_index == min_long) {
		return(NULL);
	};
	string contents = new char[unzip_info.uloSizeLowPart+1];
	unzip_commands.function = UNZIP_FILETOMEM;
	unzip_commands.lpMemBlock = contents;
	unzip_commands.lMemBlockSize = unzip_info.uloSizeLowPart;
	int saved_starting_offset = unzip_commands.lStartingOffset;
	unzip_commands.lStartingOffset = 0;
//	UnwindProtect<boolean> set(tt_ok_to_process_windows_messages,TRUE); // new on 010305 since needed sometimes by zip DLL
	int result = dunzip(&unzip_commands);
	unzip_commands.index = saved_index; // restore things
//	unzip_commands.lpszFilespec = unzip_file_spec; // restore things
	unzip_commands.lStartingOffset = saved_starting_offset;
	if (result != UE_OK) {
		return(NULL);
	};
	contents[unzip_info.uloSizeLowPart] = '\0';
	return(contents);
};

boolean extract_archive_to_directory(string archive, string file_spec, string destination_directory, 
												 boolean over_write_old_files, // new on 281003
												 boolean preserve_path_names) {  // new on 281003
   // new on 280603
	if (archive == NULL) { // new on 100104 for robustness (can happen if replay log requires a notebook file)
		if (file_spec != NULL) {
			tt_error_file() << "Can't extract " << file_spec << " from the NULL archive." << endl;
		} else {
			tt_error_file() << "extract_archive_to_directory called with NULL archive and file_spec" << endl;
		};
		return(FALSE);
	};
	unzip_commands.lpszZIPFile = archive;
	unzip_commands.lpszFilespec = file_spec; // rewritten 080703
	int saved_index = unzip_commands.index;
	unzip_commands.index = -1;
	unzip_commands.lpszDestination = destination_directory;
	unzip_commands.function = UNZIP_EXTRACT;
	unzip_commands.overWriteFlag = over_write_old_files;
	unzip_commands.noDirectoryNamesFlag = !preserve_path_names;
//	unzip_commands.updateFlag = TRUE; // commented out on 281003 since if file already exists (e.g. a log or a subtitle file) then it should be overridden 
	// and this procuedure should return TRUE
	unzip_commands.lpMemBlock = NULL; // new on 090703 seems to be necessary
	unzip_commands.lMemBlockSize = 0; // just seems tidy - new on 241003
//	unzip_commands.pZinfoEx = &unzip_info; // not clear why this needs to be reset -- experiment 241003
//	UnwindProtect<boolean> set(tt_ok_to_process_windows_messages,TRUE); // new on 010305 since needed sometimes by zip DLL
	int result = dunzip(&unzip_commands);
	// why was this only if debugging prior to 151003
	if (result != UE_OK && result != UE_BORED && result != UE_NOFILE && (over_write_old_files || result != UE_SKIP)) {
		// worth trying to use DeleteFile and then trying again? Something could be left open... asked 280803
		// error message updated on 151003
		// added result != UE_SKIP on 281003 since if not overwriting no problem if some files already exist and are skipped
		if (result != UE_OUTPUT
#if TT_ALPHA_FEATURE
			 || tt_exe_to_run_upon_shutdown == NULL || strstr(tt_exe_to_run_upon_shutdown,file_spec) == NULL
#endif
			) {
			// added conditions on 021103 since it is normal if an EXE was in a DMO file to be unable to overwrite it
			tt_error_file() << "Something went wrong extract_archive_to_directory. Unzip error code is " << result << endl;
			tt_error_file() << "The archive is " << archive << " and the file spec is " << file_spec << " and destination is " << destination_directory << endl;
			tt_err_file_important = TRUE;
		};
	};
	unzip_commands.overWriteFlag = FALSE;
	unzip_commands.noDirectoryNamesFlag = TRUE;
//	unzip_commands.updateFlag = FALSE;
	unzip_commands.index = saved_index;
	unzip_commands.lpszFilespec = NULL; // unzip_file_spec; // restore things
	return(result == UE_OK); // updated 240803 so can respond to a missing file || result == UE_BORED); // return TRUE even if there are no files to extract
};

const int chunk_size = 32*1024;
byte chunk_buffer[chunk_size];

unsigned int total_size_remaining; // assumes its size fits in 32 bits


CALLBACKSTRUCT read_write_callback_structure;

void FAR PASCAL read_write_callback(long lAction, 
												LPVOID lpMemBuf, 
												DWORD *pdwSize, 
												DWORD dwTotalReadL, 
												DWORD dwTotalReadH, 
												DWORD dwTotalWrittenL, 
												DWORD dwTotalWrittenH, 
												LPVOID lpUserData,
												long *plRet) {
	if (lAction == MEM_READ_DATA) {
		// trying CP_ACP as experiment on 090403 was CP_UTF8
		unsigned long buffer_size = *pdwSize;
		int current_chunk_size = min(total_size_remaining,buffer_size);
		unzip_commands.lMemBlockSize = current_chunk_size;
		unzip_commands.lpMemBlock = lpMemBuf;
//		UnwindProtect<boolean> set(tt_ok_to_process_windows_messages,TRUE); // new on 010305 since needed sometimes by zip DLL
		int result = dunzip(&unzip_commands);
		if (result != ZE_OK) {
			*plRet = MEM_ERROR;
		} else {
			total_size_remaining -= current_chunk_size;
			*pdwSize = current_chunk_size;
			if (total_size_remaining == 0) { 
				*plRet = MEM_DONE;
			} else {
				*plRet = MEM_CONTINUE;
				unzip_commands.lStartingOffset += current_chunk_size;
			};
		};
	} else {
		*plRet = MEM_ERROR;
	};
};

boolean copy_indexed_zip_file(int index_in_old, string old_archive, int index_in_new, string new_archive) { // new on 130703
	int saved_index = get_indexed_unzip_info(old_archive,index_in_old);
	if (saved_index == min_long) {
		return(FALSE);
	};
	total_size_remaining = unzip_info.uloSizeLowPart;
	int saved_starting_offset = unzip_commands.lStartingOffset;
	unzip_commands.lStartingOffset = 0;
	unzip_commands.function = UNZIP_FILETOMEM;
	unzip_commands.lpMemBlock = chunk_buffer;
	// now for new_archive stuff
	zip_commands.lpszZIPFile = new_archive;
	zip_commands.function = ZIP_MEMTOFILE_STREAM;
	zip_commands.compFactor = maximum_compression_level; // following doesn't seem to work -- unzip_info.wcOption;
	zip_commands.growExistingFlag = TRUE;
	quote_file_name(unzip_info.szFileName,zip_items_list);
	read_write_callback_structure.dwSize = sizeof(CALLBACKSTRUCT);
	read_write_callback_structure.lpMemToMemProc = (PFNMEMTOMEMCALLBACK) read_write_callback; // not clear why FARPROC coercion didn't work
	zip_commands.lpCallbackStruct = &read_write_callback_structure;
//	UnwindProtect<boolean> set(tt_ok_to_process_windows_messages,TRUE); // new on 010305 since needed sometimes by zip DLL
	int result = dzip(&zip_commands);
// restore things
	unzip_commands.index = saved_index; 
	unzip_commands.lStartingOffset = saved_starting_offset;
	zip_commands.lpMemBlock = NULL;
	if (result == ZE_WRITE) { // new on 300904 -- device probably full 
		// Output file write failure, possible disk full - This error can occur when a file write operation is performed on a ZIP file 
		// that is locked or restricted by another process.
		unable_to_write_file(new_archive);
	};
	return(result == ZE_OK); // simplified on 011004
};

boolean copy_zipped_files_except_logs_after(int last_version, string old_archive, string new_archive) {
	// completely rewritten on 150204 since the old version didn't deal properly with non-log files in the archive (e.g. notebook, subtitles, etc)
	if (!CopyFile(old_archive,new_archive,FALSE)) { // easier to copy the whole thing and then delete the old ones
		unable_to_write_file(new_archive); // new on 011004
		return(FALSE);
	};
	char log_file_name[MAX_PATH];
	char city_file_name[MAX_PATH]; // new on 050504
	last_version++;
	file_name_renumbered("log00000.dmo",last_version,log_file_name);
	file_name_renumbered("city00000.cty",last_version,city_file_name);
	while (delete_zip_file(new_archive,log_file_name)) {
		delete_zip_file(new_archive,city_file_name);
	   char temporary_file_copy[MAX_PATH]; // new on 160204 to delete the temporary files as well
		int temp_directory_length = strlen(tt_extracted_file_temp_directory);
		memcpy(temporary_file_copy,tt_extracted_file_temp_directory,temp_directory_length);
		strcpy(temporary_file_copy+temp_directory_length,log_file_name);
		::DeleteFile(temporary_file_copy);
		// following copied from above to clean up city as well as log file
		memcpy(temporary_file_copy,tt_extracted_file_temp_directory,temp_directory_length);
		strcpy(temporary_file_copy+temp_directory_length,city_file_name);
		::DeleteFile(temporary_file_copy);
		last_version++;
		file_name_renumbered("log00000.dmo",last_version,log_file_name);
		file_name_renumbered("city00000.cty",last_version,city_file_name);
	};
	return(TRUE);
};

//int copy_zipped_files_before_file(string last_file_to_keep, string old_archive, string new_archive) {
	// keeps the files before and including last_file_to_keep in the new_archive
	// I wonder if I kept sizes of archives if I could simply truncate the file? - probably not since tables wrong
//	int file_count = count_all_zip_members(old_archive);
//	int saved_index = get_unzip_info_for_file(old_archive,last_file_to_keep);
//	if (saved_index == min_long) {
//		return(0);
//	};
//	int last_file_index = unzip_info.ulindex;
//	zip_commands.lpszZIPFile = new_archive;
//	zip_commands.function = ZIP_DELETE;
//	int i = 0; // moved out here on 061003
//#if TT_ALPHA
//	if (tt_keep_all_time_travel_segments && tt_log_count_max > 0 && file_count > tt_log_count_max) { // new on 061003
//		// don't copy more than tt_log_count_max since rest are already "deleted"
//		i = max(0,last_file_index-tt_log_count_max);
//	};
//#endif
//	for (; i <= last_file_index; i++) {
//		if (!copy_indexed_zip_file(i,old_archive,i,new_archive)) {
//			return(0);
//		};
//	};
//	return(last_file_index+1); // +1 since started from zero - this returns the new number of items in the archive
//};

// following seemed to work but then file indices were changed
////	for (int i = unzip_info.ulindex+1; i < file_count; i++) { // already deleted unzip_info.ulindex
//	for (int i = file_count-1; i > first_file_index; i--) {
//		if (get_indexed_unzip_info(old_archive,i) == min_long) { // -1 is there was no old index
//			return(FALSE);
//		};
////		result = dunzip(&unzip_commands);
//		zip_commands.lpszItemList = unzip_info.szFileName;
//		int result = dzip(&zip_commands);
//		if (result != ZE_OK) {
//			return(FALSE); // warn??
//		};
//	};
//	return(TRUE);
//};

byte ZippedInputStream::get() {
	unsigned char buffer[1];
	read_current_file_in_unzip_archive(archive,buffer,1);
	return(buffer[0]);
};

byte ZippedInputStream::peek() {
	unsigned char buffer[1];
	read_current_file_in_unzip_archive(archive,buffer,1);
	seekg_current_file_in_unzip_archive(tellg_current_file_in_unzip_archive()-1);
	return(buffer[0]);
};

void ZippedInputStream::read(char *bytes, int count) {
	read_current_file_in_unzip_archive(archive,bytes,count);
};

void ZippedInputStream::close() {
	close_current_file_in_unzip_archive(archive);
};

int ZippedInputStream::tellg() {
	return(tellg_current_file_in_unzip_archive());
};

void ZippedInputStream::seekg(int location)  {
	seekg_current_file_in_unzip_archive(location);
};

boolean ZippedInputStream::empty() {
	return(last_read_not_enough_bytes);
};


#if !TT_DYNA_ZIP
boolean extract_current_file_in_archive(string archive, string file_name, string destination_directory) {
	unzip_commands.lpszZIPFile = archive;
	unzip_commands.lpszFilespec = file_name; // rewritten 080703
	int saved_index = unzip_commands.index;
	unzip_commands.index = -1;
//	quote_file_name(file_name,unzip_file_spec);
//	unzip_commands.lpszFilespec = file_name;
	unzip_commands.lpszDestination = destination_directory;
	unzip_commands.function = UNZIP_EXTRACT;
//	UnwindProtect<boolean> set(tt_ok_to_process_windows_messages,TRUE); // new on 010305 since needed sometimes by zip DLL
	int result = dunzip(&unzip_commands);
	unzip_commands.index = saved_index;
	unzip_commands.lpszFilespec = NULL; // unzip_file_spec; // restore things
	return(result == UE_OK);	
};
#endif

