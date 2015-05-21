// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved.
#if !defined(__TT_ZIP_H)

#define __TT_ZIP_H

void initialize_zip();
void initialize_unzip();
void uninitialize_zip();
void uninitialize_unzip();

string open_archive_to_zip(string archive_file_name);

void open_zip_file(string archive, string file_name, int compression_level=default_compression_level);

boolean write_wide_string_to_zip_file(string archive, string file_name, wide_string node_text, int total_length, int compression_level=default_compression_level);

//void open_zip_file_no_compression(string archive, string file_name);

boolean delete_zip_file(string archive, string file_name);

boolean write_zip_file(string archive, bytes buffer, unsigned int length, int compression_level=default_compression_level);

//boolean zip_wide_string_to_index(wide_string node_text, int text_length, string file_name);

boolean zip_file(string archive, string source, 
					  string destination=NULL, int compression_level=default_compression_level, boolean delete_original=FALSE, boolean quote_source=TRUE);

void close_zip_file(string ignore_archive);

void close_zip_archive(zipFile archive);

unzFile open_archive_to_unzip(string archive_file_name);

void close_unzip_archive(string archive);

void open_current_file_in_unzip_archive(unzFile archive);

string open_named_file_in_unzip_archive(unzFile archive, string file_name);

void close_current_file_in_unzip_archive(unzFile archive);

void go_to_first_file_in_unzip_archive(string archive);

void go_to_next_file_in_unzip_archive(string archive);

void go_to_named_file_in_unzip_archive(string archive, string file_name);

int read_current_file_in_unzip_archive(string archive, void *buffer, unsigned int length);

int tellg_current_file_in_unzip_archive();
void seekg_current_file_in_unzip_archive(int position);

//int count_all_zip_members(string archive);

string narrow_contents_of_current_file_in_archive(string archive, string quoted_file_name);
string narrow_contents_of_index_file_in_archive(string archive, int index);

#if !TT_DYNA_ZIP
boolean extract_current_file_in_archive(string archive, string quoted_file_name, string destination_directory);
#endif

boolean extract_archive_to_directory(string archive, string file_spec, string destination_directory,
												 boolean over_write_old_files=TRUE, // new on 281003
												 boolean preserve_path_names=FALSE);

boolean copy_zipped_files_except_logs_after(int last_version, string old_archive, string new_archive);

#endif