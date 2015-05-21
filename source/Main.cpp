// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif   
#if !defined(__TT_STRINGS_H)   
#include "strings.h"
#endif 
#if !defined(__TT_PRGRMMR_H)   
#include "prgrmmr.h"
#endif 
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif   
#if !defined(__TT_CLEANUP_H)   
#include "cleanup.h"
#endif   
#if !defined(__TT_SCREEN_H)   
#include "screen.h"
#endif   
#if !defined(__TT_MARTIAN_H)   
#include "martian.h"
#endif  
#if !defined(__TT_ROBOT_H)   
#include "robot.h"
#endif   
#if !defined(__TT_HELP_H)   
#include "help.h"
#endif   
#if !defined(__TT_HOUSE_H)   
#include "house.h"
#endif   
#if !defined(__TT_PICTURE_H)   
#include "picture.h"
#endif
//#if !defined(__TT_LOG_H)
//#include "log.h"
//#endif   
//#include "icache.h"
#if !defined(__TT_EVENT_H)   
#include "event.h"
#endif   
#if !defined(__TT_DIRTY_H)   
#include "dirty.h"
#endif   
#if !defined(__TT_USER_H)   
#include "user.h"
#endif   
#if !defined(__TT_PUZZLE_H)
#include "puzzle.h"
#endif
#if !defined(__TT_FILE_H)
#include "file.h"
#endif
#if !defined(__TT_LOG_H)
#include "log.h"
#endif
#if !defined(__TT_ZIP_H)
#include "zip.h"
#endif

#if TT_DIRECT_INPUT
#if !defined(__TT_INPUT_H)
#include "input.h"
#endif
#endif
#if TT_DIRECT_PLAY
#if !defined(__TT_NETWORK_H)
#include "network.h"
#endif
#endif
#if TT_XML
#if !defined(__TT_XML_H)
#include "xml.h"
#endif
#endif
 
#if TT_WINDOWS
#include <windows.h>
#include <mmsystem.h>
#if !defined(__TT_WINMAIN_H)   
#include "winmain.h"
#endif   
#endif  
   
//#include <fstream.h>
//#include <strstrea.h> commented out 210703
#include <string.h>
#include <new.h>
#include <shellapi.h>
#include <stdlib.h>

#include "main.h"

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

millisecond elapsed_time = 0; // for benchmarking
millisecond previous_time = 0;
millisecond true_previous_time = 0;

// noticed obsolete on 300904
//#if TT_NEW_IO
//output_string_stream pre_tt_err_file;
//#else
//output_string_stream pre_tt_err_file((char *) tt_temporary_buffer,tt_temporary_buffer_size);
//#endif
//int serial_port_to_open = 0; // none by default

//millisecond initialization_time;
millisecond previous_delta = 0;

//flag name_given = FALSE; // out here because recursive calls may reset it

void out_of_heap_handler() {
	out_of_heap(0); // this one returns useful information
};

#if TT_DEBUG_ON

string trim_spaces(string line) {
   int length = _tcslen(line);
   for (int i = length-1; i >= 0; i--) {
      if (line[i] == ' ' || (line[i] >= '0' && line[i] <= '9') || // numbers handled by tts engine
          (line[i] != '-' && line[i] != 'a' && line[i] != 'A' && line[i] != 'I' &&
           i > 0 && line[i-1] == ' ')) { // single letter or character "a box" and "I am" are OK
         line[i] = NULL; // terminate here
      } else break;
   };
   while (line[0] != NULL) {
      if (line[0] == ' ' || (line[0] >= '0' && line[0] <= '9') ||
          (line[0] != '-' && line[0] != 'a' && line[0] != 'A' && line[0] != 'I' && line[1] == ' ')) { 
			// single letter -- not "minus"
         line++;
      } else break;
   };
   return(line);
};

void add_phrase_ini_entry(output_stream &phrase_ini, int &j, string *old_lines, string line) {
   line = trim_spaces(line);
   if (line == NULL || line[0] == NULL || line[0] == '/') return; // line[0] == '/' added 100899
   if (line[1] != 0) {
		lower_case(line);
		remove_extra_spaces(line);
	};
   for (int i = 0; i < j; i++) {
      if (old_lines[i] != NULL && _tcscmp(old_lines[i],line) == 0) return; // already there
   };
   old_lines[j] = line;
   phrase_ini << (j++) << "=" << line << endl;
};

flag backwards_compatible = FALSE; // remove this once I am ready to re-record VCE file

//boolean is_char_alpha(char c) {
//  if (IsCharAlpha(c)) return(TRUE);
//  if (tt_language == SWEDISH) {
//     string extras = "åÅäÄöÖ";
//     for (int i = 0; i < 6; i++) {
//       if (extras[i] == c) {
//          return(TRUE);
//       };
//     };
//  };
//  return(FALSE);
//};

void add_puzzle_part_to_phrase_ini(output_stream &phrase_ini, int &j, string *old_lines, const_string puzzle_part, long part_length) {
   string part = new character[part_length+1];
   int new_length = 0;
   int k = 0;
   while (puzzle_part[k] != NULL) {
      if (puzzle_part[k] == '(') { // skip the parenthetical remark
         while (puzzle_part[k] != NULL && puzzle_part[k] != ')') {
            k++;
         };
         if (puzzle_part[k] == ')') {
            k++;
         };
         if (puzzle_part[k] == ' ') {
            k++;
         };
      };
      if ((puzzle_part[k] == '\''&& (k == 0 || !IsCharAlpha(puzzle_part[k-1]) || k+1 == part_length || !IsCharAlpha(puzzle_part[k+1]))) ||
//          puzzle_part[k] == '-' || // commented out on 080500 since screws up Portuguese since elsewhere treated like a space
			 puzzle_part[k] == '\\') {
         k++;
		} else if (puzzle_part[k] == '-' && k > 0 && k+1 < part_length && IsCharAlpha(puzzle_part[k-1]) && IsCharAlpha(puzzle_part[k+1])) {
			part[new_length++] = ' '; // new on 080500 to treat hypens between alphabetic characters as a space
			k++;
      } else if (puzzle_part[k] == '\"') { // should really check that non-number follows
         // for things within "" eliminate the quotes and break phrase there
         part[new_length++] = ','; // any terminator will do
         k++;
      } else if (puzzle_part[k] != NULL) {
         part[new_length++] = puzzle_part[k];
         k++;
      };
   };
   part[new_length] = NULL;
   long start = 0;
   for (int i = 0; i < new_length; i++) {
       if (part[i] == '.' || part[i] == '?' || part[i] == '!' ||
           part[i] == ',' || part[i] == ':' || part[i] == '-' ||
           (part[i] >= '0' && part[i] <= '9') ||
           (!backwards_compatible && IsCharAlpha(part[i]) && // a letter all by itself
           !((tt_language == AMERICAN && (part[i] == 'a' || part[i] == 'i'))) &&  // a and I are words in English
//            || (tt_language == SWEDISH && (part[i] == 'i' || part[i] == 'å' || part[i] == 'ö'))) &&
            (i == 0 || part[i-1] == ' ') &&
            (i+1 == new_length || part[i+1] == ' '))) {
          int line_length = i-start;
          string line = new character[line_length+2];
          int m = 0;
          for (int k = start; k < i; k++) {
             line[m++] = part[k];
          };
          line[m] = NULL; // terminate string
          add_phrase_ini_entry(phrase_ini,j,old_lines,line);
          start = i+1;
       };
   };
   delete [] part;
};

const int max_puzzle_size = 6000; // largest is smaller than 6K - was 4000 prior to 200400

void add_puzzles_to_phrase_ini(output_stream &phrase_ini, int &j, string *old_lines, ascii_string puzzle_directory) {
   long length;
   int last_puzzle = 100;
   for (int i = 1; i < last_puzzle; i++) {
      char file_name[256];
      output_string_stream name(file_name,MAX_PATH); // was 256 prior to 240104
      name << puzzle_directory << "p" << i << ".pzl";
      name.put('\0');
//#if TT_UNICODE
		FileInputStream puzzle;
		puzzle.open(file_name,std::ios_base::in); // std::ios_base::nocreate);
//#else
//      ifstream puzzle(file_name,std::ios_base::in|std::ios_base::nocreate);
//#endif
      if (puzzle.empty()) return;
      string puzzle_part = read_lines(&puzzle,max_puzzle_size,length,line_terminator());
      if (puzzle_part == NULL) return;
		const int html_extension_count = 2;
		string html_extensions[html_extension_count] = {"htm","html"};
      if (_tcsncmp(puzzle_part,_T("Skip this puzzle."),_tcslen(_T("Skip this puzzle."))) != 0 &&
			 !already_has_extension(puzzle_part,html_extensions,html_extension_count)) { // new on 030400
//			 !already_has_extension(puzzle_part,"html")) {
        // have a real puzzle
        add_puzzle_part_to_phrase_ini(phrase_ini,j,old_lines,puzzle_part,length);
        for (int k = 0; k < 4; k++) { // skip next 4 parts
           delete [] puzzle_part;
           puzzle_part = read_lines(&puzzle,max_puzzle_size,length,line_terminator());
			  if (puzzle.empty()) {
				  say_error("Something wrong with puzzle file processed to generated phrase.ini"); // added 060400
				  return; 
			  };
        };
        while (puzzle_part != NULL && !puzzle.empty()) {
           delete [] puzzle_part;
           puzzle_part = read_lines(&puzzle,max_puzzle_size,length,line_terminator());
           if (puzzle_part != NULL) add_puzzle_part_to_phrase_ini(phrase_ini,j,old_lines,puzzle_part,length);
        };
      };
//      delete [] puzzle_part; // commented out on 211299 since if end of file was reached might already be deleted
   };
};

const int longest_phrase = 30;

int word_count(string line) {
   int length = _tcslen(line);
   int count = 1;
   for (int i = 0; i < length; i++) {
      if (line[i] == ' ') count++;
      if (count == longest_phrase) return(longest_phrase);
   };
   return(count);
};

void add_line(string line, strings *lines, int *last) {
//if (strcmp(line,"a sidan") == 0) {
//tt_error_file() << "debug";
//};
   line = trim_spaces(line);
   if (line == NULL || line[0] == NULL) return;
   int size_index = word_count(line)-1;
   int last_index = last[size_index];
//	if (size_index == 0) { // debug this
//		tt_error_file() << line << " index=" << last[size_index] << endl;
//	};
   for (int i = 0; i < last_index; i++) {
      string old_line = lines[size_index][i];
      if (old_line != NULL && _tcsicmp(old_line,line) == 0) return; // already there
   };
   lines[size_index][last[size_index]++] = line;
};

boolean try_to_break(string start, int search_length, string line, int size, int line_index, strings *lines, int *last) {
   if (start != NULL) {
      string new_line;
      if (start == line) { // initial segment
         new_line = line+search_length; // remove initial segment
         if (new_line[0] == ' ') { // didn't break the middle of a word
            add_line(copy_string(new_line),lines,last);
            lines[size][line_index] = NULL; // remove old one
            return(TRUE);
         };
      } else {
         int line_length = _tcslen(line);
         new_line = new character[line_length-search_length+1];
         int j = 0;
         while (line != start) {
            new_line[j++] = line[0];
            line++;
         };
         if (new_line[j-1] == ' ' &&  // didn't break a word in beginning
             (j+search_length >= line_length ||  // and of phrase
              line[search_length] == ' ')) { // didn't break a word at end
            new_line[j] = NULL;
            add_line(new_line,lines,last); // preceeding chunk added
            if (j+search_length < line_length) {
               add_line(copy_string(line+search_length),lines,last);
            };
            lines[size][line_index] = NULL; // remove old one
            return(TRUE);
         };
      };
   };
   return(FALSE);
};

void break_phrase(strings *lines, int size, int line_index, int *last) {
   string line = lines[size][line_index];
//if (strcmp(line,S(IDS_OPTIONS_NOTEBOOK_HELP3)) == 0) {
//tt_error_file() << "debug";
//};
   for (int s = size-1; s > 0; s--) { // 0 was 1
      for (int i = 0; i < last[s]; i++) { // start with one smaller until 2 long
         string search = lines[s][i];
         string start = NULL;
         if (search != NULL) start = _tcsstr(line,search);
         if (start != NULL && try_to_break(start,_tcslen(search),line,size,line_index,lines,last)) return;
      };
   };
//   if (tt_language == SWEDISH) {
//      string search, start;
//      search = "det";
//      start = _tcsstr(line,search);
//      if (start != NULL && try_to_break(start,_tcslen(search),line,size,line_index,lines,last)) return;
//      search = "den";
//      start = _tcsstr(line,search);
//      if (start != NULL && try_to_break(start,_tcslen(search),line,size,line_index,lines,last)) return;
//      search = "och";
//      start = _tcsstr(line,search);
//      if (start != NULL && try_to_break(start,_tcslen(search),line,size,line_index,lines,last)) return;
//   };
};

boolean redundant_phrase(string phrase, int max_size_to_check, strings *lines, int *last, boolean top_level) {
   int length = _tcslen(phrase);
//   string subphrase = new char[length+1];
//if (strcmp(phrase,"ett sifferkort") == 0) {
//tt_error_file() << "debug" << endl;
//};
   for (int i = 0; i <= max_size_to_check; i++) {
//      int j = 0;
//      while (phrase[j] != ' ' && j < length) {
//         subphrase[j++] = phrase[j];
//      };
//      subphrase[j] = NULL;
      for (int k = 0; k < last[i]; k++) {
         string existing_phrase = lines[i][k];
         if (existing_phrase != NULL) {
            int subphrase_length = _tcslen(existing_phrase);
            if (subphrase_length <= length &&
                _tcsncmp(phrase,existing_phrase,subphrase_length) == 0 &&
                (subphrase_length == length || phrase[subphrase_length] == ' ')) { // front part is covered
//               tt_error_file() << existing_phrase << endl;
//               tt_error_file() << phrase << endl;
               int new_max_size_to_check = max_size_to_check-(i+1);
               if (top_level) new_max_size_to_check++;
               if (subphrase_length == length || redundant_phrase(phrase+subphrase_length+1,new_max_size_to_check,lines,last,FALSE)) return(TRUE);
            };
         };
      };
   };
//   delete [] subphrase;
   return(FALSE);
};

boolean redundant(int size, int index, strings *lines, int *last) {
   if (lines[size][index] == NULL) return(FALSE);
   return(redundant_phrase(lines[size][index],size-1,lines,last,TRUE));
};

void break_long_phrases(string *old_lines, int length) {
// could pull off words or phrases from either end even if they are shorter than max_ok_size
   strings *lines = new strings[longest_phrase];
   int *last = new int[longest_phrase];
	int i;
   for (i = 0; i < longest_phrase; i++) {
      lines[i] = new string[1000]; // max ini entries of a particular size
      last[i] = 0;
   };
   for (i = 1; i < length; i++) {
      int size = word_count(old_lines[i]);
      lines[size-1][last[size-1]++] = old_lines[i];
   };
//   for (int i = 0; i < longest_phrase; i++) {
//      if (old_lines[i] != NULL) delete [] old_lines[i];
//   };
//   delete [] old_lines;
   const int max_ok_size = 4;
   for (i = longest_phrase-1; i >= max_ok_size; i--) {
      for (int j = 0; j < last[i]; j++) {
          break_phrase(lines,i,j,last);
      };
   };
   // do it again since there are some new short phrases available now
   for (i = longest_phrase-1; i >= max_ok_size; i--) {
      for (int j = 0; j < last[i]; j++) {
          if (lines[i][j] != NULL) break_phrase(lines,i,j,last);
      };
   };
   for (i = longest_phrase-1; i >= max_ok_size; i--) {
      for (int j = 0; j < last[i]; j++) {
          if (lines[i][j] != NULL) break_phrase(lines,i,j,last);
      };
   };
   output_file_stream phrase_new("phrase.new",std::ios_base::out);
   ascii_string old_file_name;
   switch (tt_language) {
      case SWEDISH:
         old_file_name = "\\voicedata\\semarty\\phrase.ini";
         break;
      case BRITISH:
         old_file_name = "\\tt\\uk\\phrase.ini";
         break;
		case BRAZILIAN_PORTUGUESE:
			old_file_name = "\\tt\\port\\phrase.ini";
			break;
		case PORTUGAL_PORTUGUESE:
			old_file_name = "\\tt\\portugal\\phrase.ini";
			break;
      default:
         old_file_name = "phrase.ini";
         break;
   };
//#if TT_UNICODE
	input_file_stream phrase_old(old_file_name,std::ios_base::in); // std::ios_base::nocreate);
//#else
//   ifstream phrase_old(old_file_name,std::ios_base::in|std::ios_base::nocreate);
//#endif
   // output the new stuff making minimal changes to the old
   const int line_size = 256;
   int entry_index = 1;
   character line_buffer[line_size];
   const int phrase_max = 4000;
   strings current_phrases = new string[phrase_max];
   strings final_phrases = new string[phrase_max];
   for (i = 0; i < phrase_max; i++) final_phrases[i] = NULL;
   int phrase_count = 0;
   phrase_old.getline(line_buffer,line_size); // should be [Sentences]
   phrase_new << line_buffer << endl;
   while (phrase_old != NULL) {
      phrase_old.getline(line_buffer,line_size);
      if (phrase_old != NULL) {
         for (int i = 0; i < line_size; i++) {
            if (line_buffer[i] == '=') {
               current_phrases[phrase_count++] = copy_string(line_buffer+i+1);
               break;
            };
         };
      };
   };
   if (current_phrases[phrase_count] == NULL) phrase_count--;
   int last_phrase_index = 0;
   for (i = longest_phrase-1; i > 0; i--) {
      for (int j = 0; j < last[i]; j++) {
         if (redundant(i,j,lines,last)) {
//            tt_error_file() << "Removing redundant " << lines[i][j] << endl;
//            delete [] lines[i][j];
            lines[i][j] = NULL;
         };
      };
   };
   for (i = 0; i < longest_phrase; i++) { // look for ones that are still current
      for (int j = 0; j < last[i]; j++) {
         if (lines[i][j] != NULL) { // not been removed or redundant
            for (int k = 0; k < phrase_count; k++) {
               if (_tcsicmp(current_phrases[k],lines[i][j]) == 0) { // still current
                  lines[i][j] = NULL;
                  final_phrases[k] = current_phrases[k];
                  if (last_phrase_index < k) last_phrase_index = k;
                  break;
               };
            };
         };
      };
   };
   int p = 0;
   for (i = 0; i < longest_phrase; i++) { // look for new ones
      for (int j = 0; j < last[i]; j++) {
         if (lines[i][j] != NULL) { // not been removed
            while (final_phrases[p] != NULL) {
              p++;
            };
            final_phrases[p++] = lines[i][j];
            tt_error_file() << "New " << p << " " << lines[i][j] << endl;
         };
      };
   };
   for (i = p; i < phrase_count; i++) {  // may have fewer phrases than before
       if (final_phrases[i] == NULL) { // take one from the end
          phrase_count--;
          while (final_phrases[phrase_count] == NULL) {
             phrase_count--;
          };
          final_phrases[i] = final_phrases[phrase_count];
          ascii_string number = new char[8];
          itoa(1000000+(phrase_count+1),number,10);
          tt_error_file() << "move s" << (number+1) << ".wav ";
          itoa(1000000+(i+1),number,10);
          tt_error_file() << "s" << (number+1) << ".wav" << endl;
          final_phrases[phrase_count] = NULL;
       };
   };
   for (i = 0; i < phrase_max; i++) {
      if (final_phrases[i] != NULL) phrase_new << (entry_index++) << "=" << final_phrases[i] << endl;
   };
   phrase_new.close();
};

void generate_phrase_ini() {
   int j = 1;
   ascii_string out_file_name;
   ascii_string orig_file_name;
   switch (tt_language) {
     case SWEDISH:
        out_file_name = "phrase.se";
        orig_file_name = "phrase.orig.se";
        break;
     case BRITISH:
        out_file_name = "phrase.uk";
        orig_file_name = "phrase.orig.uk";
        break;
	  case BRAZILIAN_PORTUGUESE:
		  out_file_name = "phrase.br";
        orig_file_name = "phrase.orig.br";
		  break;
	  case PORTUGAL_PORTUGUESE:
		  out_file_name = "phrase.pt";
        orig_file_name = "phrase.orig.pt";
		  break;
     default:
       out_file_name = "phrase.us";
       orig_file_name = "phrase.orig.us";
   };
//#if TT_UNICODE
	output_file_stream phrase_ini(out_file_name,std::ios_base::out);
	input_file_stream phrase_original(orig_file_name,std::ios_base::in); // std::ios_base::nocreate);
//#else
//   output_file_stream phrase_ini(out_file_name,std::ios_base::out);
//   ifstream phrase_original(orig_file_name,std::ios_base::in|std::ios_base::nocreate);
//#endif
   const int last_marty_talk = 7000; // after 7000 now used for non-spoken stuff
//      const int first_phrase_index = 115;
   string *old_lines = new string[3000];  // max phrase.ini entries
   int i;
   for (i = 0; i < 3000; i++) old_lines[i] = NULL;
   if (phrase_original != NULL) {
      const int line_size = 256;
      character line_buffer[line_size];
      phrase_original.getline(line_buffer,line_size); // should be [Sentences]
      phrase_ini << line_buffer << endl;
      while (phrase_original != NULL) {
         phrase_original.getline(line_buffer,line_size);
         if (phrase_original != NULL) {
            for (int i = 0; i < line_size; i++) {
               if (line_buffer[i] == '=') {
                  add_phrase_ini_entry(phrase_ini,j,old_lines,copy_string(line_buffer+i+1));
//                  phrase_ini << (j++) << "=" << (line_buffer+i+1) << endl;
//                  old_lines[j] = copy_string(line_buffer+i+1);
                  break;
               };
            };
         };
      };
   }; // warn that there is no phrase.orig ??
   string line;
   string next_line = NULL;
   for (i = 1; i < last_marty_talk;) {  // incremented elsewhere
      boolean skip = TRUE;
      int language = (tt_language-AMERICAN); // for computing index offsets
//if (i == IDS_DROP_ON_NUMBER_TO_BREAK_HELP) {
//tt_error_file() << "debug";
//};
      if (i >= IDS_VISIBLE_REMOTE_STATE1 && i < IDS_YES) {
         const int visibility_sensor_count = 3;
         for (int s = 0; s < visibility_sensor_count; s++) {
            if (IDS_VISIBLE_REMOTE_STATE1+s+(visibility_sensor_count*language) == i) {
               skip = FALSE; // in this language
               break;
            };
         };
      } else if (i >= IDS_YES && i < IDS_VK_HOME) {
         const int boolean_sensor_count = 2;
         for (int s = 0; s < boolean_sensor_count; s++) {
            if (IDS_YES+s+(boolean_sensor_count*language) == i) {
               skip = FALSE; // in this language
               break;
            };
         };
      } else if (i >= IDS_VK_HOME && i < IDS_NUMBER_TYPE_INDEFINITE) {
         const int sensor_key_count = 16;
         for (int s = 0; s < sensor_key_count; s++) {
            if (IDS_VK_HOME+s+(sensor_key_count*language) == i) {
               skip = FALSE; // in this language
               break;
            };
         };
      } else { // non-sensor so filter out stuff Marty won't say...
         skip = ((i > 5000 && i < 6000) ||
                 (i >= IDS_BAD_DEMO_LOG && i <= IDS_NARRATION_FILE_SUFFIX) ||
                 (i >= IDS_NATURAL_LANGUAGE && i <= IDS_INSTALL_FIRST) ||
                 (i >= IDS_EXPANDER_KEY_COMMANDS && i <= IDS_COPIER_KEY_COMMANDS) ||
//                 (i >= IDS_LEGO_HELP1 && i <= IDS_LEGO_HELP24) ||
                 (i >= IDS_UUENCODING_HEADER && i <= IDS_BETA_TO_EXPIRE1) ||
                 (i >= IDS_DESTROYED_TWICE && i <= IDS_BRUSH_NUMBER_MISMATCH) ||
                 (i >= IDS_TYPE_NAME1 && i <= IDS_CARDINAL_MOD_IS_9) ||
                 (i >= IDS_BUT_FILE_IS && i <= IDS_RESIND_DAMAGED) ||
                 (i >= IDS_UUENCODING_HEADER && i <= IDS_UUENCODING_HEADER3) ||
                 (i >= IDS_IMAGE_SIZE_WRONG && i < IDS_SPEECH_ENGINE_VOICE) ||
//                 (i >= IDS_DEBUG_START_EVENT && i <= IDS_DEBUG_SERIAL_NUMBER) ||   // past 5000 anyways
                 (i >= IDS_PUZZLE_SINKING && i <= IDS_PUZZLE_RESCUE) ||
                 (i >= IDS_FRONT_WALL && i <= IDS_ROOM_DOOR) || // just used for debuggin
					  i == IDS_UNKNOWN_MCI_ERROR || // is not spoken anymore -- is in talk balloon only
					  i == IDS_TOO_MANY_HOLES ||
                 i == IDS_NEED_CDROM_TO_CONTINUE ||
                 i == IDS_NEED_CDROM_FOR_NARRATION ||
                 i == IDS_THOUSANDS_COMMA ||
                 i == IDS_BETA_EXPIRED ||
                 i == IDS_ASK_NAME_FAILURE ||
                 i == IDS_PROBLEM_LOADING_AN_IMAGE ||
                 i == IDS_IS_PART_OF_ITSELF ||
                 i == IDS_BAD_DAT_FILE ||
                 i == IDS_TOONTALK_BETTER_256 ||
//                 i == IDS_ABANDON_FULL_SCREEN ||
//                 i == IDS_ABANDON_FULL_SCREEN2 ||
                 i == IDS_ASK_NAME_FAILURE ||
                 i == IDS_OUT_OF_MEMORY ||
                 i == IDS_BAD_PUZZLE_FILE ||
                 i == IDS_COMMAND_LINE_ODD_ARGS2 ||
                 i == IDS_NO_HIGH_RES_IN_16_BITS ||
                 i == IDS_BAD_VIDEO_MODE1 ||
                 i == IDS_BAD_VIDEO_MODE2 ||
                 i == IDS_COMMAND_LINE_H_WRONG1 ||
                 i == IDS_COMMAND_LINE_H_WRONG2 ||
                 i == IDS_COMMAND_LINE_B_WRONG ||
                 i == IDS_NO_NAME_IS_YOUR_NAME ||
                 i == IDS_TROUBLE_WANT_TO_CONTINUE ||
                 i == IDS_UNABLE_TO_OPEN_FILE ||
                 i == IDS_SPEECH_ENGINE_VOICE ||
                 i == IDS_TOONTALK_BROKEN ||
                 i == IDS_TOONTALK_USED_AFTER_DESTROYING ||
                 i == IDS_TOONTALK_DESTROY_ON_FLOOR ||
                 i == IDS_TOONTALK_DESTROY_PART_OF_SOMETHING ||
                 i == IDS_BAD_DAT_FILE ||
                 i == IDS_PICK_NONEXISTENT_MOVIE ||
                 i == IDS_FREEING_PROTECTED_CACHE ||
                 i == IDS_CACHING_NON_SAVING ||
                 i == IDS_DESTROYING_SAME_PICTURE_TWICE ||
                 i == IDS_HAS_PARTS_BUT_DOESNT ||
                 i == IDS_HAS_PARTS_BUT_DOESNT2 ||
                 i == IDS_TOONTALK_VERSION_NUMBER ||
                 i == IDS_SORRY_USER_PICTURE_TOO_BIG ||  // this and next only apply to 16 bit version
                 i == IDS_USER_PICTURE_NOT_256_COLORS ||
                 i == IDS_NOT_ENOUGH_MEMORY_FOR_NEXT_SOUND ||
//                 i == IDS_BOX_DOESNT_HAVE_EXPECTED_HOLE ||
//                 i == IDS_REMOVE_FROM_BOX_BUT_NOT_THERE ||
                 i == IDS_UNEXPECTED_NOT_IN_BOX ||
                 i == IDS_ADD_TO_BACKGROUND_ALREADY_THERE ||
//                 i == IDS_ADD_TO_BACKGROUND_BUT_ALREADY_THERE || // obsolete
                 i == IDS_REMOVE_PART_BUT_NO_PARTS ||
                 i == IDS_ROBOT_TRAINING_LOST ||
                 i == IDS_BAD_HOUSE_VIEW ||
                 i == IDS_COMMAND_LINE_ODD_ARGS1 ||
                 i == IDS_COMMAND_LINE_ODD_ARGS2 ||
                 i == IDS_COMMAND_LINE_UNKNOWN_OPTION ||
                 i == IDS_TIME_NEGATIVE ||
                 i == IDS_MEMORY_LOW ||
                 i == IDS_MEMORY_LOW2 ||
                 i == IDS_OUT_OF_MEMORY ||
                 i == IDS_VIDEO_MODE_BAD ||
                 i == IDS_FULL_SCREEN_INITIALIZATION_PROBLEMS ||
                 i == IDS_FULL_SCREEN_PROBLEMS ||
                 i == IDS_TOONTALK_WINDOW_TITLE ||
                 i == IDS_TOONTALK_BETTER_256 ||
                 i == IDS_TOONTALK_WINDOW_TITLE ||
                 i == IDS_SUBTITLES_FILE_SUFFIX ||
                 i == IDS_NOTEBOOK_FUTURE_VERSION ||
                 i == IDS_THIS_VERSION_CANT ||
                 i == IDS_GET_NEW_VERSION ||
                 i == IDS_WELCOME_MESSAGE);
      };
      if (skip) {
         i++;
      } else {
         if (next_line == NULL) {
//if (i == IDS_OPTIONS_NOTEBOOK_HELP3) {
//tt_error_file() << "debug";
//};
            line = (string) S(i++); // throwing away the "const" part of the type here
            if (line != NULL && line[1] != NULL) { // not single letter
					lower_case(line);
					remove_extra_spaces(line); // new on 280999
            };
         } else {
            line = next_line;
            next_line = NULL;
         };
         if (line != NULL) {
            int line_length = _tcslen(line);
            string line_copy = new character[line_length+1];
            int m = 0;
            for (int k = 0; k < line_length; k++) {
               if (k == 0) {
                  while (line[k] == ' ' && k+1 != line_length) {  // eliminate preceeding spaces
                     k++;
                  };
               };
               if (line[k] == '_') k++; // skip over hypen markers as if they weren't there
               if (line[k] == '.' || line[k] == ',' ||
                   line[k] == '?' || line[k] == '!') { // terminators
                  if (k+1 != line_length) { // not last character
                     k++;
                     if (k+1 != line_length) { // start a new one
                        next_line = line+k;
                        break;
                     };
                  };
               } else if (!backwards_compatible && IsCharAlpha(line[k]) &&
                          (tt_language != AMERICAN || (line[k] != 'a' && line[k] != 'i')) && // a and I are words in English
                          (k == 0 || line[k-1] == ' ') &&
                          (k+1 == line_length || line[k+1] == ' ')) { // a letter all by itself
                  int char_start = k;
                  while (line[k] != ' ' && k > 0) {
                     // search back for a space
                     k--;
                  };
                  while (m > 0 && line_copy[m-1] != ' ' && m > 0) {
                     m--;
                  };
                  if (char_start+1 < line_length) { // && k > 0) { // start a new one
                     next_line = line+char_start+1;
                  };
                  break;
               } else if (line[k] >= '0' && line[k] <= '9') {
                  // speech engine doesn't handle things like F1 well
                  int digit = k;
                  while (line[k] != ' ' && k > 0) {
                     // search back for a space
                     k--;
                  };
                  while (m > 0 && line_copy[m-1] != ' ' && m > 0) {
                     m--;
                  };
                  while (((line[digit] >= '0' && line[digit] <= '9') ||
                          line[digit] == ' ') &&
                          digit+1 != line_length) {
                     digit++;
                  };
                  if (digit+1 != line_length) { // start a new one
                     next_line = line+digit;
                  };
                  break;
               } else if (line[k] == '-' || // (line[k] == '-' && !(k == 0 || (line[k-1] == ' ' && k+1 < line_length && line[k+1] == ' '))) ||
                            // speech engine gets confused by - (hypen vs minus)
                           line[k] == '+' || line[k] == '*' || line[k] == '|' ||
                           line[k] == ';' || line[k] == ':' || // _ treated specially since is hypen marker
                           line[k] == '&' || line[k] == '^' || line[k] == '%' ||
                           line[k] == '$' || line[k] == '#' || line[k] == '@' ||
                           line[k] == '~' || line[k] == '`' || line[k] == '<' ||
                           line[k] == '>' || line[k] == '/' || line[k] == '=' ||
                           line[k] == '\r') {
                   if (k+1 != line_length) { // not last character
                     k++;
                     if (k+1 != line_length) { // start a new one
                        next_line = line+k;
                     };
                     break;
                  };
               } else if (line[k] == '\'' && k+2 < line_length && line[k+2] == '\'') {
                  // e.g. 'R'  then skip it
                  k += 2;
                  if (k+1 != line_length) { // start a new one
                     next_line = line+k;
                  };
                  break;
               } else if (line[k] == '(') { // remove parenthetical remark
                  while (k < line_length) {
                     if (line[k] == ')') {
                        if (k+1 < line_length && line[k+1] == ' ') {
                            k++;
                        };
                        break; // for while
                     };
                     k++;
                  };
                  // if ' then skip it unless is part of a contraction (surrouned by letters
               } else if ((line[k] == '\'' && (k == 0 || !IsCharAlpha(line[k-1]) || k+1 == line_length || !IsCharAlpha(line[k+1]))) ||
//                          line[k] == '(' || line[k] == ')' ||
                          line[k] == '\"') {
                  if (k == 0) {
                     while (k+1 < line_length && line[k+1] == ' ') { // eliminate spaces
                        k++;
                     };
                  };
               } else {
//                  if (line[k] != '\r') {
                     line_copy[m++] = line[k];
//                  } else {
//                     line_copy[m++] = ' ';
//                  };
               };
            };
            line_copy[m] = NULL; // terminate it
//            if (tt_language == AMERICAN) {
//               switch (j) {
//                  case 380: // was eliminated so fill with user names
//                  case 396:
//                  case 399:
//                  case 409:
//                  case 591:
//                  case 804:
//                  case 806:
//                  case 810:
//                  case 847:
//                  case 993:
//                  case 995:
//                  case 997:
//                  case 1169:
//                  case 1171:
//                  case 1235:
//                     add_phrase_ini_entry(phrase_ini,j,old_lines,names[name_index++]);
//                     old_lines[j] = names[name_index];
//                     phrase_ini << (j++) << "=" << names[name_index++] << endl;
//                     break;
//               };
//            };
            if (m > 1) {
               add_phrase_ini_entry(phrase_ini,j,old_lines,line_copy);
            };
         };
      };
   };
// taken care of by phrase.orig.se and prhase.orig.us now
//   switch (tt_language) {
//      case SWEDISH:
//         {
//         string extras[] = {"Oj då","Jag kan tyvärr inte komma ihåg vad jag skulle säga",
//                            "något fel", "tyvärr kan", "tar bort", "eller så är en bildfil dålig", // to maintain fewest changes to phrase.ini
//                            "å","Å","ä","Ä","ö","Ö",NULL};
//         int i = 0;
//         while (extras[i] != NULL) {
//            add_phrase_ini_entry(phrase_ini,j,old_lines,extras[i]);
//            i++;
//         };
//         };
//         add_puzzles_to_phrase_ini(phrase_ini,j,old_lines,"c:\\toontalk\\puzzles\\swedish\\");
//         break;
//      case AMERICAN:
//         {
//         string extras[] = {"Ahh","sorry I cant remember what I was going to say",
//                             // "Milli","new","bald","hat"
//                             "big","silly","pink","rule","kick","walk", // from the sentence demos
//                             "boys","girls","dogs","cats",
//                             "three dots", // for "..."
//                             NULL};
//         int i = 0;
//         while (extras[i] != NULL) {
//            add_phrase_ini_entry(phrase_ini,j,old_lines,extras[i]);
//            i++;
//         };
//         };
//         add_puzzles_to_phrase_ini(phrase_ini,j,old_lines,"c:\\tt\\");
//         break;
//   };
   ascii_string puzzle_directory = NULL;
   switch (tt_language) {
      case SWEDISH:
         puzzle_directory = "d:\\tt_saved\\puzzles\\swedish\\";
         break;
      case AMERICAN:
         puzzle_directory = "d:\\tt_saved\\puzzles\\us\\";
         break;
      case BRITISH:
         puzzle_directory = "d:\\toontalk\\puzzles\\uk\\";
         break;
		case BRAZILIAN_PORTUGUESE:
			puzzle_directory = "c:\\program files\\animated programs\\toontalk\\puzzles\\br\\";
			break;
		case PORTUGAL_PORTUGUESE:
			puzzle_directory = "c:\\program files\\animated programs\\toontalk\\puzzles\\pt\\";
			break;
   };
   if (puzzle_directory != NULL) {
      add_puzzles_to_phrase_ini(phrase_ini,j,old_lines,puzzle_directory);
   };
   phrase_ini.close();
   break_long_phrases(old_lines,j);
   // could deallocate but am too lazy...
};
#endif

boolean already_opened_tt_err_file = FALSE; // new on 100704

void open_tt_error_file() {
	// following updated on 100704 so pre-init stuff is copied over
	if (already_opened_tt_err_file) return;
	already_opened_tt_err_file = TRUE;
//	boolean changing_names = FALSE;
	//if (tt_err_file_name != NULL) {
	//	delete [] tt_err_file_name;
	//	changing_names = TRUE;
	//};
	string old_file_name = tt_err_file_name;
	if (tt_file_name == NULL) { // new on 110101 for robustness
		tt_file_name = copy_string("NoName");
	};
	tt_err_file_name = new_file_name(tt_file_name,"txt"); // easier to look at as TXT than LOG
	int flags;
	if (old_file_name != NULL) { // simplified on 041003
		tt_err_file.close();
		move_file(old_file_name,tt_err_file_name);
		flags = std::ios_base::out|std::ios_base::app; // append to the end
		delete [] old_file_name;
	} else {
		flags = std::ios_base::out;
	};
   tt_err_file.open(tt_err_file_name,flags);
	if (tt_err_file == NULL) { // new on 290904 for error recovery
		unable_to_write_file(tt_err_file_name); 
	};
//	tt_error_file() << "tt_mouse_mode_on_floor is " << (int) tt_mouse_mode_on_floor << endl;
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
//	tt_error_file() << "test" << endl;
#endif
};

boolean tt_initialize() {
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
   if (tt_temp_files != NULL) return(TRUE); // already initialized
   tt_temp_files = new ascii_string[tt_temp_files_max]; // obsolete??
   open_tt_error_file();
//  delete [] full_file_name;
#if TT_DEBUG_ON
   if (tt_debug_mode == 9999) {
      generate_phrase_ini();
   };
#endif
//   previous_time = timeGetTime(); // commented out on 150204 since gives wrong delta for first cycle
//   true_previous_time = previous_time; // commented out on 150204 since gives wrong delta for first cycle
	if (time_travel_enabled()) { // moved as experiment on 260304
		save_oldest_and_current_version_numbers(0); // to be sure 0.xml is first
		if (tt_video_mode_if_running_demo > 0) {
			tt_video_mode = tt_video_mode_if_running_demo; // new on 071004
		};
	} else if (replaying() && tt_video_mode_if_running_demo > 0) {
		tt_video_mode = tt_video_mode_if_running_demo; 
		// new on 071004 but this branch is rarely taken so flat demos don't replay in recorded resolution
	};
   if (!tt_setmode(tt_video_mode,FALSE)) return(FALSE);
//  performance_timer.timerOn();
//  if (tt_debug_mode == 7) { // mode 7 turns off cache
//    tt_caching_screen = FALSE;
//  };
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
  // my standard behavior for out of heap
#if TT_MICROSOFT
   _set_new_handler(out_of_heap);
#else
   set_new_handler(out_of_heap_handler);
#endif
	initialize_memory_cache();
#if TT_XML
   intialize_xml_tag_table();
#endif
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	initialize_sprite_table();
//	initialization_time = timeGetTime();
//  atexit(standard_exit);
//  cout << 10/tt_debug_mode; // to trigger an error
//  initialize_floor_stacks();
   initialize_alphabet();
   tt_robots_have_names = (flag) ini_int(AC(IDC_SWITCHES),AC(IDC_GENERATE_ROBOT_NAMES),FALSE,0); // re-written 110101
   if (tt_debug_mode == 12345) {
      char name[20];
      for (long i = 1; i <= max_unsigned_short; i++) {
         serial_number_to_name((unsigned short int) i,name);
         tt_error_file() << name << endl;
      };
   };
	tt_max_number_of_holes = ini_int(AC(IDC_SWITCHES),AC(IDC_MAXIMUM_NUMBER_OF_HOLES),FALSE,tt_max_number_of_holes);
	if (tt_max_number_of_holes <= 0) { // new on 070203
		tt_max_number_of_holes = max_long;
	};
	tt_zip_files = ini_int(AC(IDC_SWITCHES),"ZipFiles",FALSE,tt_zip_files); // new on 230203
   // following moved here on 271199 (for wrong reason but OK anyway)
	initialize_keyboard_accelerators(); // no-op if loaded from a DMO file
	// following commented out on 311001 - was never documented
//	tt_using_directx_transform_for_BMPs 
	// = (tt_bits_per_pixel != 8 || ini_int("Switches","UseDirectXTransformForBMPs",FALSE,tt_using_directx_transform_for_BMPs)); 
	// new on 260500 -- 8 bits per pixel checked add 121001
	tt_direct_input_mouse_exclusive_ok  = ini_int("Switches","ExclusiveMouseOK",FALSE,tt_direct_input_mouse_exclusive_ok); 
	// new on 140800
	tt_delay_between_titles =				  ini_int(AC(IDC_SWITCHES),"DelayBetweenTitles",FALSE,tt_delay_between_titles); 
	// new on 200800
	return(TRUE);
};

void tt_initialize_objects() {
	if (tt_suspensions != NULL) return; // already initialized
  // done later than tt_initialize
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "tt_initialize_objects 1" << endl;
#endif
  tt_suspensions = new Suspensions();
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "tt_initialize_objects 2" << endl;
#endif
  tt_screen = new Screen();
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "tt_initialize_objects 3" << endl;
	tt_error_file().flush();
#endif
  tt_city = new City(); // initializing houses requires that tt_screen is initialized
  #if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "tt_initialize_objects 4" << endl;
	tt_error_file().flush();
#endif
  tt_programmer = new Programmer();
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "tt_initialize_objects 5" << endl;
	tt_error_file().flush();
#endif
//  tt_toolbox = new Toolbox();
  tt_toolbox = NULL; //initialize on need
  tt_default_user_image = new UserImage(); // new on 251000
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "tt_initialize_objects 6" << endl;
	tt_error_file().flush();
#endif
  tt_global_picture = new GlobalPicture();
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "tt_initialize_objects 7" << endl;
	tt_error_file().flush();
#endif
  tt_global_picture->become_blank(); // new on 220499 since used as well as typical blank picture
  tt_martian = new Martian();
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "tt_initialize_objects 8" << endl;
	tt_error_file().flush();
#endif
  // following moved here on 120599 since initialize_sound might use tt_global_picture if something else has hold of the device
#if TT_DIRECTX
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "about to initialize_sound" << endl;
	tt_error_file().flush();
#endif
   if (tt_sound_option != 0 && !initialize_sound()) {
      tt_sound_option = 0;
   };
#if TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
	tt_error_file() << "Finished initialize_sound" << endl;
	tt_error_file().flush();
#endif
#endif
   tt_error_file() << "Record of events for " << tt_user_name << endl;
#if TT_DEBUG_ON||TT_SPECIAL_VERSION_TO_DEBUG_WIN_ME
   tt_error_file() << "Starting at time " << timeGetTime() << endl;
   tt_error_file() << "Bits per pixel: " << tt_bits_per_pixel << endl;
	tt_error_file().flush();
#endif
	if (tt_local_ini_file_uncached_name != NULL) { // new on 020301
		tt_error_file() << "Using " << tt_local_ini_file_uncached_name << " as a source of customizations as well as the usual toontalk.ini" << endl;
	};
  // put this here so if problems tt_martian is notified
//#if !TT_32
//	if (tt_serial_port > 0) {
//		open_serial_port(tt_serial_port);
//	} else {
//		tt_serial_port = 0;
//	};
//#endif
/*
#if TT_BETA
	string replay_file = replay_file_name();
	new_ordering =
		  (replay_file == NULL ||
			(strncmp(replay_file,"explode.",8) != 0 &&
			 strncmp(replay_file,"pongact1.",9) != 0 &&
//          strncmp(replay_file,"pongact2.",9) != 0 &&
			 strncmp(replay_file,"intro.",6) != 0 &&
//			 strncmp(replay_file,"append.",7) != 0 &&
//			 strncmp(replay_file,"fact.",5) != 0 &&
			 strncmp(replay_file,"fibs.",5) != 0 &&
			 strncmp(replay_file,"swap.",5) != 0 
//			 && strncmp(replay_file,"account.",8) != 0
			 ));
#endif
*/
};

// noticed obsolete on 300904
//boolean report_initialization_problems() {
////#if TT_32
//	if (pre_tt_err_file != NULL && pre_tt_err_file.LENGTH > 1) { // added pre_tt_err_file != NULL on 300904
////#else
////  if (pre_tt_err_file.pcount() > 1) {
////#endif
//	  pre_tt_err_file << endl << S(IDS_WANT_TO_LEARN_ABOUT_COMMAND_LINE);
//	  const_string message = (string) tt_temporary_buffer; // pre_tt_err_file.STR;
//     if (message_box(MB_YESNO,message,NULL,FALSE) == IDYES) {
//        launch_help("command");
//     } else {
//        OpenIcon(tt_main_window->get_handle());
//     };
//     boolean ok = (message_box(MB_OKCANCEL|MB_ICONQUESTION,S(IDS_TROUBLE_WANT_TO_CONTINUE)) != IDCANCEL);
//// following is nice but it is too late to change names...
////	  if (ok && tt_user_name == NULL) {
////		  ask_what_name();
////		  if (tt_user_name != NULL) {
////			  compute_tt_file_name();
////			 if (tt_user_parameters != NULL) {
////				  delete tt_user_parameters;
////			 };
////          tt_user_parameters = new Parameters();
////		  } else { // give up
////			  return(FALSE);
////		  };
////	  };
//	  return(ok);
//  };
//  return(TRUE); // no problems
//};
// lego controller needs signal every 2 seconds
millisecond lego_signal_time = 0;
millisecond behind_time = 0;
//#if TT_DIRECTX
//boolean clean_background_last_frame = FALSE;
//#endif

millisecond behind_schedule() {
  return(behind_time);
};

boolean one_tt_cycle() {
//	UnwindProtect<boolean> set(tt_ok_to_process_windows_messages,FALSE); 
	// commented out on 050305 since this is too wide a scope -- added just where needed
	// varioius utilities need processing of messages (zip at least) -- goes white and can't do anything otherwise
	// new on 100105 so messages generated here (e.g. by a robot using clipboard) don't throw things off
	millisecond new_time = timeGetTime();
	millisecond current_delta = 0;
	if (true_previous_time != 0) { // conditional new so first cycle has a 0 delta
		tt_true_millisecond_delta = new_time-true_previous_time;
		current_delta = new_time-previous_time;
	};
	previous_time = new_time;
	elapsed_time += current_delta; // for benchmarking
	if (tt_real_time_pause_began > 0) { // new on 130204
		tt_time_spent_paused = new_time-tt_real_time_pause_began;
		tt_real_time_pause_began = 0;
		tt_real_time_pause_ended = new_time;
		tt_current_time_pause_ended = tt_current_time;
	} else {
		tt_time_spent_paused = 0;
	};
//#if TT_DEBUG_ON
//	if (tt_debug_mode == 2308031 && tt_titles_ended_on_frame <= tt_frame_number) {
//		tt_screen->display_debug_info(current_delta);
//	};
//#endif
	if (tt_replay == REPLAY_REPRODUCE_TIMING) { 
		// && tt_titles_ended_on_frame <= tt_frame_number) { //  && tt_titles_ended_on_frame new on 230304
		// beginning 190204 comparing end of pause with current time rather than looking at deltas (since errors can accumulate)
		millisecond relative_real_time = new_time-tt_real_time_pause_ended;
		millisecond relative_toontalk_time = tt_current_time-tt_current_time_pause_ended;
#if TT_DEBUG_ON 
		if (tt_debug_mode == 1995) {
			tt_error_file() << new_time << "-" << tt_real_time_pause_ended << "=" << relative_real_time << " real time." << endl;
			tt_error_file() << tt_current_time << "-" << tt_current_time_pause_ended 
								 << "=" << relative_toontalk_time << " ToonTalk time." << endl;
		};
#endif
		if (relative_real_time < relative_toontalk_time) {
			// don't sleep more than 100ms on any one cycle even if running ahead of schedule
			sleep(min(100,relative_toontalk_time-relative_real_time));
#if TT_DEBUG_ON
		} else if (tt_debug_mode == 110105) {
			tt_error_file() << "Fell " << (relative_real_time-relative_toontalk_time) 
								 << "ms behind schedule on frame " << tt_frame_number << endl;
#endif
		};
		//current_delta -= tt_narration_overhead; // don't count narration overhead
		//if (current_delta > 1000) { // probably paused
		//	current_delta = 1000;
		//};
//		tt_narration_overhead = 0;
//		millisecond extra_time = previous_delta-current_delta;
//#if TT_DEBUG_ON
//		if (tt_debug_mode == 1995) {
//			tt_error_file() << "Current delta=" << current_delta 
//				         << " and previous delta=" << previous_delta 
//							<< " and narration overhead is " << tt_narration_overhead 
//							<< " at time= " << new_time << endl;
//		};
//		if (tt_debug_mode == 180603) {
//			tt_error_file() << "Extra time is " << extra_time << " and current delta is " << current_delta << endl; 
//		};
//		add_to_log_if_interesting_error();
//#endif
//		if (extra_time > 1000) {
//			extra_time = 1000; // recorded such a long delta so that CLOCK_REMOTE works correctly but don't want to sleep that long - 
			// especially if was paused
//		};
//		if (extra_time > 0) {
//			// reproduce timing as well and going too fast
////			  if (behind_time > 0) { // behind schedule so don't sleep
//				if (behind_time < extra_time) { // added Dec 17 '98
//					sleep(extra_time-behind_time);
//					// following added Dec 17 too since every other frame didn't sleep enough
//					previous_time += extra_time-behind_time;
//					behind_time = 0;
//				} else {
//					behind_time -= extra_time;
//				};
////			  } else {
////				  sleep(extra_time);
////			  };
//		} else {
//			behind_time -= extra_time;
//		};
	};
// tt_error_file() << behind_time << " " << tt_frame_number << endl;
	millisecond original_delta;
	if (tt_replay == REPLAY_REPRODUCE_TIMING_ON_NEXT_FRAME) {
		// do nothing - new on 020404 since replaying() has changed its meaning on 200304
		tt_stop_all_pictures = TRUE; // new on 161004 since don't want pictures with speeds to move
	} else if (replaying()) { // && tt_titles_ended_on_frame <= tt_frame_number) { // && tt_titles_ended_on_frame new on 230304
		original_delta = replay_clock();
		previous_delta = original_delta; // for computing extra_time above
      tt_millisecond_delta = (tt_speed*original_delta)/100;
		tt_true_millisecond_delta = tt_millisecond_delta; // new on 111199 so demos show clock sensor correctly
		if (tt_millisecond_delta == 0) {
			if (tt_frame_number >= tt_titles_ended_on_frame) { 
				// condition new on 170903 since titles were staying up too long on replay
				tt_millisecond_delta = 1; // new on 210300 to handle low speeds
			};
		} else if (tt_millisecond_delta > 1000 && tt_log_version_number >= 56) { 
			// new on 191201 -- commented out on 130204 but if this was due to a pause handled below now
			// restored on 170204 since long pauses break subtitles
			// commented out again on 170204 since moved call to synchronize_subtitles -- huh??
			// restored yet again on 161004 since interferes with rerecording time travel segments when pictures have speed
			// made this only apply to new logs on 211004
			tt_millisecond_delta = 1000;
		};
		tt_current_time += tt_millisecond_delta;
#if TT_DEBUG_ON
		if (tt_debug_mode == 101 || tt_debug_mode == 140103) {
			tt_error_file() << tt_millisecond_delta << "ms delta logged." << endl;
		};
#endif
		if (!replaying()) { // reached end of log
//			tt_current_time = elapsed_time;
			if (tt_exit_at_end_of_log && tt_debug_mode != 19) {
				if (tt_saved_city_file_name_when_demo_ends != NULL) {
					tt_city->dump_to_new_file(tt_saved_city_file_name_when_demo_ends,FALSE,FALSE,FALSE);
					delete [] tt_saved_city_file_name_when_demo_ends;
					tt_saved_city_file_name_when_demo_ends = NULL;
				};
				return(FALSE);
			};
		};
	} else { // if (tt_replay != REPLAY_REPRODUCE_TIMING_ON_NEXT_FRAME) { 
		// || tt_titles_ended_on_frame > tt_frame_number) { // || tt_titles_ended_on_frame new on 230304
		// negative of condition moved to beginning of conditional
		// if (tt_still_frame_count != tt_frame_number && tt_programmer->are_titles_over()) { 
		// commented out the above on 140703 since Marty doesn't move during titles - now uses tt_initial_current_time instead
		// condition new on 230803 since don't change the time if about to start replaying log next frame
		// and not the first frame - conditions new on 130703
		original_delta = current_delta; 
		// updated 20204 since narration overhead doesn't matter if not reproducing timing - tt_narration_overhead; 
		// don't count narration overhead
//		tt_narration_overhead = 0;
		if (original_delta > 1000) { // should this apply to adjusted_delta instead?
			// consider any pause greater than 1 second to be exactly 1 second
			original_delta = 1000;			  
		};
		millisecond adjusted_delta = (tt_speed*original_delta)/100;
		if (tt_log_started_frame == tt_frame_number) {
			// new on 161004 -- don't update pictures with speeds until next cycle
			tt_stop_all_pictures = TRUE;
		};
		// was else if prior to 261204 but then if mouse is moving, mouse sensors active and adjusted_delta is 0 then divide by zero 
		// (issues 596 and 598)
		if (adjusted_delta <= 0) { 
			adjusted_delta = 1;
		} else if (adjusted_delta > max_unsigned_short) { // can't log larger numbers
			adjusted_delta = max_unsigned_short; // should only happen if tt_speed is large
		}; // else if (adjusted_delta > 0) { // if negative wrapped around so use previous value
			// could figure out how much it changed
		tt_millisecond_delta = adjusted_delta;
		tt_current_time += tt_millisecond_delta;
		if (tt_debug_mode == 1785) { // want to keep SPK and update DMO
         // might be problems here if tt_speed != 100
			synchronize_narration(tt_millisecond_delta); // just now
		};
	};
	// moved synchronize_subtitles call from here on 170204
   true_previous_time = previous_time;
#if TT_DEBUG_ON
	if (tt_current_time < 0) {
		tt_error_file() << "Warning time is negative " << tt_current_time
							 << " on frame number " << tt_frame_number << endl;
		say_error(IDS_TIME_NEGATIVE);
		tt_current_time = 0; // reset it
		stop_replay();
	};
#endif
	if (tt_logging && tt_record_clock) { // tt_record_clock new on 300104
		if (tt_titles_ended_on_frame > tt_frame_number) { // commented out on 230304 - restored 230304 -- updated 230804
			log_time(1); // new on 300104 -- 
#if TT_DEBUG_ON 
			if (tt_debug_mode == 101) {
				tt_error_file() << "1 ms delta logged since in titles on frame " << tt_frame_number << endl;
			};
#endif
		} else {
			log_time(tt_true_millisecond_delta); 
			// was original_delta prior to 191201 but then the CLOCK_REMOTE doesn't work right in logs where this is more than 1000
#if TT_DEBUG_ON 
			if (tt_debug_mode == 101) {
				tt_error_file() << tt_true_millisecond_delta << "ms delta logged on frame number "  << tt_frame_number << endl;
			};
#endif
		};
		record_special_events();
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	if (replaying()) { //  && tt_titles_ended_on_frame <= tt_frame_number) { //  && tt_titles_ended_on_frame new on 230304
		replay_special_events();
		if (tt_time_spent_paused > 0) { // new on 130204 -- replaying a log that was paused
			if (tt_millisecond_delta > tt_time_spent_paused) {
				tt_millisecond_delta -= tt_time_spent_paused;
				// commented out following on 261204 since this is silly 
				// since just checked that tt_millisecond_delta > tt_time_spent_paused
				//if (tt_millisecond_delta <= 0) { // just to be extra safe as of 211204 -- but issue 596 wasn't in replay
				//	tt_millisecond_delta = 1;
				//};
			//if (tt_true_millisecond_delta-tt_time_spent_paused < 1000 && tt_true_millisecond_delta > 1000) { 
			//	// rewritten on 170204 so if delta not counting pause is less than 1 second adjust things -- commented out on 170204
			//	tt_millisecond_delta = tt_true_millisecond_delta-tt_time_spent_paused;
			//	tt_current_time -= 1000-tt_millisecond_delta;
			//};
//#if TT_DEBUG_ON
//			} else {
//				tt_error_file() << tt_time_spent_paused << " is tt_time_spent_paused and is larger than tt_millisecond_delta " 
									//<< tt_millisecond_delta << " at time " << tt_current_time << endl;
//#endif
				tt_current_time -= tt_time_spent_paused; // new on 170204
			};
			tt_time_spent_paused = 0;
		// following commented out on 160904 since it breaks demos (e.g. fibs2)
		//} else if (tt_millisecond_delta > 1000) { 
			// too long a pause without having a pause - probably a bug - workaround for now 170204
		//	tt_current_time += (tt_millisecond_delta-1000); // prior to 160904 was -=
		//	tt_millisecond_delta = 1000;
		};
	};
   if (tt_subtitles_speed > 0 && tt_titles_ended_on_frame <= tt_frame_number) { // added tt_titles_ended_on_frame on 100204
      synchronize_subtitles();
   };
	if (tt_paragraph_id_just_finished >= 0) { // new on 241199 moved here so this can be recorded
		if (tt_martian == NULL) tt_martian = new Martian(); // new on 141299
		tt_martian->speech_has_finished(tt_paragraph_id_just_finished);
		tt_paragraph_id_just_finished = -1;
	};
#if TT_WINDOWS&&!TT_32&TT_LEGO
	if (tt_lego_in_use == CONTROL_LAB_LEGO_CONTROLLER && 
		lego_signal_time <= tt_current_time) {
		add_to_serial_port_buffer(0x02);
		lego_signal_time = tt_current_time+2000; // every 2 seconds
	};
	if (tt_lego_in_use != NO_LEGO_CONTROLLER) {
		transmit_buffer_to_serial_port();
	};
#endif
#if TT_DIRECT_PLAY
	receive_messages(); // new on 230400
#endif
#if TT_DEBUG_ON
	if (tt_debug_mode == 3) {
		tt_error_file() << (timeGetTime()-new_time) << " elapsed before programmer react" << endl;
	};
#endif
	tt_finish_all_animations = FALSE; // if temporarily set
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	boolean done = tt_programmer->react(tt_screen);
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	if (done) return(!done); // added 7/9/98
   if (currently_paused()) { // programmer paused system
      return(FALSE);
   };
#if TT_DEBUG_ON
	if (tt_debug_mode == 3) {
		tt_error_file() << (timeGetTime()-new_time) << " elapsed before running queues" << endl;
	};
#endif
#if !TT_DIRECTX
	play_current_sound();
#endif
	// moved here so there will be a delay from when a robot
   // acts to blow up a house and it happens so programmer can get out
	if (tt_deleted_houses != NULL) {
		delete_deleted_houses();
	};
	// new ordering is better since it'll respond one frame sooner
	// to sensors
#if TT_DEBUG_ON
	if (tt_debug_mode == 3) {
		tt_error_file() << (timeGetTime()-new_time) << " elapsed before tt_global_picture->update_globals()" << endl;
	};
#endif
	boolean clean_background;
	if (tt_global_picture == NULL) {
		tt_global_picture = new GlobalPicture(); // new on 240105
	};
	tt_global_picture->update_globals();
#if TT_DIRECT_INPUT
   if (tt_joystick_pictures != NULL) {
      for (int i = 0; i < tt_joystick_count; i++) {
         tt_joystick_pictures[i]->update_globals();
      };
   };
#endif
#if !TT_32
   if (tt_hyperbot_picture != NULL) tt_hyperbot_picture->update_globals();
#endif
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
	if (tt_debug_mode == 3) {
		tt_error_file() << (timeGetTime()-new_time) 
							 << " elapsed before tt_screen->update_display()" << endl;
	};
#endif
	{
//		UnwindProtect<boolean> set(tt_ok_to_process_windows_messages,FALSE); // moved/copied here on 050305
		// needed since robots may do things here -- e.g. use clipboard via animation callbacks --
		// but I don't think this is a problem 050305
		clean_background = tt_screen->update_display();
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
	if (tt_debug_mode == 3) {
		tt_error_file() << (timeGetTime()-new_time) 
							 << " elapsed before run_postponed_collision_checks()" << endl;
	};
#endif
   if (tt_frame_number != tt_shift_frame_number) { 
		// condition new on 140599 to avoid spurious collisions
		// what happens if one of the postponed pictures is deleted in the meanwhile??
		run_postponed_collision_checks();
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
#endif
	if (tt_running_robots != NULL && !tt_city->stopped() && (tt_frame_number > tt_titles_ended_on_frame 
		 || (tt_frame_number == tt_titles_ended_on_frame && (tt_log_version_number < 51 || time_travel_enabled())))) {
		// on 161004 experimented with changing (tt_log_version_number < 51 || time_travel_enabled()) 
		// to tt_log_version_number < 51 but
		// it didn't help the bug I was tracking down and probably is wrong
		// since only runs while time travel is enabled but not on replay (I think)
		// tt_frame_number > tt_titles_ended_on_frame new on 230804 since 
		// otherwise it is recorded with 1 ms delta and timers etc don't replay right
#if TT_DEBUG_ON
		if (tt_debug_mode == 201199) {
			tt_error_file() << "Running " << tt_running_robots->length() 
								 << " robots on frame " << tt_frame_number << " at " << timeGetTime() << endl;
		};
#endif
		Sprites *remaining = tt_running_robots;
		Sprites *temp = tt_running_robots;
		tt_running_robots = NULL;
		while (remaining != NULL) {
//			  remaining->first()->update_display();
			Robot *robot = (Robot *) remaining->first();
//			  robot->destroy(); // had better just decrement the ref count - added 310100
			// commented out above on 090400 since causes robots to not be deleted and to run (as orphans)
			// and when robot is prepared for deletion it removes itself from tt_running_robot
			remaining->set_first(NULL);
			if (!robot->prepared_for_deletion()) { // conditional added on 090400 since earlier robot may have destroyed this guy
				if (tt_log_version_number >= 73 && robot->is_action_in_progress_of_any_teammate()) {
					// new on 180405 to do nothing if already running
#if TT_DEBUG_ON
					debug_this(); // for now
#endif
				} else if (robot->showing_on_a_screen()) { // new on 171000
					robot->try_clause();
				} else {
					robot->really_try_clause();
				};
			};
			remaining = remaining->rest();
		};
		delete temp; // just the top level list -- robots aren't there
	};
#if TT_DEBUG_ON
	if (tt_debug_mode == 3) {
		tt_error_file() << (timeGetTime()-new_time) 
							 << " elapsed before run_queue()" << endl;
	};
#endif
//	if (tt_titles_ended_on_frame > tt_frame_number && tt_log_version_number >= 39 && !tt_city->stopped()) {
		// conditional new on 230304 since if double clicking on something to start ToonTalk this is recorded but
		// user events aren't 
		// moved  && !tt_city->stopped() here since if F8 no need to record or replay
	if (tt_frame_number > tt_titles_ended_on_frame 
		 || (tt_frame_number == tt_titles_ended_on_frame && (tt_log_version_number < 51 || time_travel_enabled()))
		 || tt_log_version_number < 38) { // conditional new on 230304 -- was 39 prior to 020404
		// rewritten again on 050404 -- updated on 230804 so doesn't run the frame that titles ended
		if (tt_queue != NULL && !tt_city->stopped()) {
//#if TT_DEBUG_ON
//			UnwindProtect<int> set(tt_debug_mode,100105);
//#endif
			UnwindProtect<boolean> set(tt_ok_to_process_windows_messages,FALSE); // moved/copied here on 050405
			run_queue();
		} else if (tt_log_version_number >= 38) { // condition new on 050404
			if (tt_logging) {
				// new on 280204 
				log_events_counter(0);
			};
			if (replaying()) { // && tt_log_version_number >= 38) { // new on 280204
				unsigned short int events_counter_in;
				replay_events_counter(events_counter_in);
				if (events_counter_in != 0) {
					log(S(IDS_LOG_REPLAY_ERROR),FALSE,!tt_run_demos_despite_anomalies); // last arg was TRUE prior to 311204
					// error message below new on 291204
					tt_error_file() << "When recorded there were " << events_counter_in 
										 << " events during this cycle and now there are none." 
										 <<  " On segment #" << tt_current_log_segment << endl; // new on 160105
					if (tt_time_travel != TIME_TRAVEL_OFF && !tt_run_demos_despite_anomalies) {
#if TT_DEBUG_ON
						debug_this();
#endif
						throw(EVENTS_COUNTER_NON_ZERO_WITH_NO_EVENT_QUEUE); // renamed 030504
					};
				};
			};
		};
	};
#if TT_DEBUG_ON
	add_to_log_if_interesting_error();
	if (tt_debug_mode == 3 || tt_debug_mode == 120105) {
		tt_error_file() << (timeGetTime()-new_time) 
							 << " elapsed before tt_screen->propagate_changes(); time is " << timeGetTime() << endl;
	};
	//if (tt_debug_mode == 250405 && tt_frame_number == 725239) {
	//	Sprites *ignore = new Sprites(); // test on 250405
	//};
#endif
	tt_screen->propagate_changes();
// relies upon tt_finish_all_animations instead now
//     if (tt_backgrounds_still_animating != NULL) {
//        tt_backgrounds_still_animating
//          = tt_backgrounds_still_animating->animate_though_offscreen();
//     };
#if TT_DEBUG_ON
	//if (tt_debug_mode == 250405 && tt_frame_number == 725239) {
	//	Sprites *ignore = new Sprites(); // test on 250405
	//};
	if (tt_debug_mode == 3) {
		tt_error_file() << (timeGetTime()-new_time) 
							 << " elapsed before displaying screen" << endl;
	};
#endif
//#if TT_DIRECTX // assumes one 2 flipping surfaces
//     tt_screen->display(clean_background&&clean_background_last_frame);
//     clean_background_last_frame = clean_background;
//#else
	tt_screen->display(clean_background);
//#endif
   if (tt_mouse_mode == SIMULATED_DIRECTIONAL_PAD_MOUSE_MODE) { // put here on 230402 so directional pad is refreshed properly
      tt_main_window->save_direction_pad_region();
   };
#if !GRAPHICS_TO_MEMORY
	if (tt_page_count > 1) {
		tt_work_page = 1-tt_work_page;
	};
#endif
	if (tt_next_frames_deleted_sprites != NULL) {
		tt_next_frames_deleted_sprites->delete_all();
		delete tt_next_frames_deleted_sprites;
		tt_next_frames_deleted_sprites = NULL;
	};
	if (tt_deleted_sprites != NULL) {
		tt_next_frames_deleted_sprites = tt_deleted_sprites;
//		  Sprites *temp = tt_deleted_sprites;
		// reset first in case in process of deletion cause more deletions
		tt_deleted_sprites = NULL;
//		  delete temp;
	};
	if (tt_deleted_lists != NULL) {
		SpritesList *temp = tt_deleted_lists;
		tt_deleted_lists = NULL;
#if TT_DEBUG_ON
		if (tt_debug_mode != 666)
#endif
		temp->delete_all();
//		  delete tt_deleted_lists;
	};
	if (tt_deleted_memory != NULL) {
		delete tt_deleted_memory;
		tt_deleted_memory = NULL;
	};
	//if (tt_backgrounds_to_delete_next_frame != NULL) { // new on 090804
	//	delete tt_backgrounds_to_delete_next_frame;
	//	tt_backgrounds_to_delete_next_frame = NULL;
	//};
   boolean nothing_changed = (tt_dirty_min_y >= tt_dirty_max_y);
	if (nothing_changed) {
		tt_still_frame_count++;
	};
   if (tt_sleep > 0 && tt_replay != REPLAY_REPRODUCE_TIMING) { 
		// && !tt_programmer->are_titles_over()) { // commented this out on 190301 since it broke tt_sleep
		// removed !nothing_changed || on 150100 since if nothing might have happened because hand is hidden and rest cached
		// and then replay and not everything is cached.
		// also makes the "-y <n>" have a weird special case
      // if something really changed or in titles then implement tt_sleep -- otherwise am running off screen ...
//		  sleep(tt_sleep); // just to slow things down
		millisecond delta = timeGetTime()-new_time;
		// restored the above on 130204 since the following had lots of variance and even had cycles less than tt_sleep
		//static DWORD last_time = timeGetTime();
		//// rewritten 130204 since the old way didn't take into account the DirectX display stuff
		//DWORD now = timeGetTime();
		//millisecond delta = now-last_time;
		//last_time = now;
		if (delta < tt_sleep) {
			sleep(tt_sleep-delta);
		};
	};
//tt_error_file() << timeGetTime() << endl;
#if TT_DIRECT_INPUT
   update_joystick_button_still_down();
#endif
	// alway change frame number (for accurate replay despite caching)
	// and if used by off screen code
	update_keep_every();
	tt_frame_number++;
//#if TT_DEBUG_ON
//	if (tt_debug_mode == 60505 && tt_frame_number == 130000) { // 169939 is when it crashed 145000 was too late
//		_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_CHECK_CRT_DF);
//	};
//	if (tt_debug_mode == 250405 && tt_frame_number == 725240) {
//		_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
//	};
//#endif
	if (tt_current_time >= tt_next_postponed_include_file_xml_time) {
		do_next_postponed_include_file_xml();
	};
	if (tt_current_time >= tt_next_new_log_time && tt_time_travel == TIME_TRAVEL_OFF) { // new on 271199
		// following moved here on 190100 since new log reading is triggered by replay_clock reaching end of log
		// abstracted on 130204 so can be called when hitting pause/play while recording in time travel
		start_time_travelling();
	};
	if (tt_replay == REPLAY_REPRODUCE_TIMING_ON_NEXT_FRAME) {
		set_replay(REPLAY_REPRODUCE_TIMING);
		tt_frame_number--; // so when debugging the frame numbers "line up"
	};
	tt_stop_all_pictures = FALSE; //  new on 161004 -- since already tried to move this cycle
	if (tt_titles_just_ended) { // new on 230803
		// since the following was happening in the middle of a cycle - very confusing
		if ((tt_jump_to_current_log_segment || tt_jump_to_youngest_log_segment) && read_counters_from_archive()) { 
			// new on 190703 since initialization discovered a DMO file that is a time travel archive
			tt_next_new_log_time = max_long; // don't make new logs while replaying
//		     set_replay_file_name(file_name_renumbered("log00000.dmo",tt_current_log_segment)); 
			// new on 200703 since don't want to begin munging the archive name
			set_replay_file_name(append_strings(tt_user_directory,"log00000.dmo")); // will be renumbered before use
			if (time_travel_enabled()) { // condition new on 080604 for robustness
				time_travel(TIME_TRAVEL_ABOUT_TO_START);
				if (tt_starting_log_segment >= 0) {
					jump_to_log_segment(tt_starting_log_segment); // new on 050204
				} else if (tt_jump_to_youngest_log_segment) { // new on 290204 -- currently used to deal with a missing current.cty
					jump_to_log_segment(tt_youngest_log_segment-1); 
					// -1 so you have a few seconds of the old if you click on Play (also last segment may be corrupted)
				} else {
					jump_to_log_segment(tt_oldest_log_segment); // was tt_current_log_segment
				};
				tt_jump_to_current_log_segment = FALSE;
			};
		};
	};
	// prior to 210105 the following was in the conditional above but now need to support drag of DMO files
	if (tt_replay_after_titles != NO_REPLAY) { // conditional added 250100
		set_replay(tt_replay_after_titles);
		tt_replay_after_titles = NO_REPLAY;
		if (tt_titles_just_ended) { 
			open_log(NULL,tt_error_file()); // no-op if no log file name set - moved here on 250100
		} else {
			// new on 210105 if not called directly after initialization
			tt_youngest_log_segment = -1;
			read_counters_from_archive(); // at least to find out what screen size to switch to
			if (tt_video_mode_if_running_demo != tt_video_mode && tt_video_mode_if_running_demo != 0) {
				tt_setmode(tt_video_mode_if_running_demo,TRUE);
//				tt_main_window->set_client_size(tt_screen_width,tt_screen_height);
				tt_main_window->set_window_size(tt_screen_width,tt_screen_height,TRUE);
				tt_main_window->compute_screen_dimensions(tt_screen_width,tt_screen_height);
				release_direct_draw(TRUE);
				tt_main_window->initialize_direct_draw(); // new on 240105
				delete tt_screen;
				tt_screen = new Screen();
			}; 
			tt_city->reset_all_blocks(TRUE); // commented out on 240105 since open_log will do this
			open_log(NULL,tt_error_file()); // no-op if no log file name set - moved here on 250100
			initialize_button_key_substitutions();
			set_recordable_ini_options();
			tt_titles_ended_on_frame = max_long; // tt_frame_number; 
			// to act the same way as if ToonTalk were started with this demo
			tt_logging = FALSE; // can't play and record a demo anymore
//			tt_programmer = new Programmer();
		};
	};
	tt_titles_just_ended = FALSE;
#if TT_DEBUG_ON
   if (tt_debug_mode == 300500) {
		tt_error_file() << "========================== Frame number is now " << tt_frame_number << " ==========================" 
							 << endl;
	};
	if (tt_debug_mode == 3) {
		millisecond now = timeGetTime();
		tt_error_file() << (now-new_time)
							 << " elapsed during cycle " << tt_frame_number << " at time " << now << endl;
	};
	if (tt_debug_mode == 103) {
		tt_error_file() << "Dirty region: " << tt_dirty_min_x << "," << tt_dirty_min_y
							 << " to " << tt_dirty_max_x << "," << tt_dirty_max_y << endl;
	};
	if (tt_frame_number == tt_debug_frame_number && tt_titles_ended_on_frame <= tt_frame_number) {
		debug_this("Just reached debugging frame number.");
	};
	if (tt_debug_mode == 120603) {
		tt_error_file() << tt_current_time << " is the time at the beginning of frame " << tt_frame_number << endl;
	};
   add_to_log_if_interesting_error();
#endif
	return(!done);
};

void delete_deleted_houses() { // abstracted on 290204
	Houses *houses_remaining = tt_deleted_houses;
	tt_deleted_houses = NULL;
	while (houses_remaining != NULL) {
//			  house = houses_remaining->first();
//			  houses_remaining->set_first(NULL);
//			  house->remove_from_screen(tt_screen); // in case it is being viewed
//			  tt_city->remove_house(house);
//			  delete house;
		Houses *new_houses_remaining = houses_remaining->rest();
		houses_remaining->set_rest(NULL);
		delete houses_remaining;
		houses_remaining = new_houses_remaining;
	};
};

void start_time_travelling() { // abstracted 130204
	millisecond time_to_start_log = close_log_and_open_next();
#if TT_DEBUG_ON
	if (time_to_start_log > 1000) { // probably took a long time cause hit a breakpoint
		time_to_start_log = 1000;
	};
#endif
	save_input_events();
	if (tt_current_time >= tt_next_new_log_time) { // didn't encounter any problems
		tt_next_new_log_time = tt_current_time+tt_time_between_new_logs+time_to_start_log;
#if TT_DEBUG_ON
		if (tt_debug_mode == 290903) {
			tt_error_file() << "Just set tt_next_new_log_time to " << tt_next_new_log_time << endl;
		};
#endif
	};
};

void standard_exit(boolean emergency) {
//	MessageBox(NULL,"calling standard_exit","TT",MB_OK); // just for testing!! 270105
	if (tt_standard_exit_started) exit(0);
	tt_standard_exit_started = TRUE;
 //  tt_err_file.close(); // experiment 041003
	//tt_err_file.open("c:\\tt_experiment.txt",std::ios_base::out);
	write_ini_entry("FTP","UserName"," ");
	if (!emergency && time_travel_enabled()) { // !emergency added 030200 tt_time_between_new_logs > 0 added 060300 - removed tt_logging &&  on 190703
		save_city_since_end_of_logging();
	};
	if (tt_programmer != NULL && tt_programmer->kind_of() == PROGRAMMER_AT_FLOOR) {
		// especially important if training a robot
		tt_programmer->programmer_at_floor()->release_items_held();
	};
   if (elapsed_time == 0) elapsed_time++; // exiting just after starting -- don't divide by zero
   long frame_count = tt_frame_number-tt_still_frame_count;
	// the following should be both accurate and work up to 21 million frames
	long fps10; // 10xfps
	if (frame_count >= max_long/10000L && elapsed_time > 1000) { // would overflow so approximate
		// added elapsed_time > 1000 for very short runs on 200804
		fps10 = (frame_count*10)/(elapsed_time/1000L);
	} else {
		fps10 = ((frame_count*10000L)/elapsed_time);
	};
	string want_to_see_log = copy_string(S(IDS_WANT_TO_SEE_LOG,"Do you want to see your log file?")); 
	// do now before string library freed
	quiting_puzzles();
   if (!tt_temporary_user) {
      if (fps10 < 50 && frame_count > 500) { // user customizable??
         trouble_shoot(TROUBLE_SHOOT_TOO_SLOW,NULL,NULL,FALSE,TRUE);
		} else if (fps10 > 400 && frame_count > 500 && tt_video_mode < 1024) { // added 1024 condition on 260300
			// changed it to < from <= on 240400 since if already set at 1024 why give this advice?
			// commented out tt_video_mode == 320 on 060300
//			::CloseWindow(tt_main_window->get_handle()); 
			// new on 310103 -- didn't seem to help - still a bug here (looksbug.dmo running full-screen is an example)
         trouble_shoot(TROUBLE_SHOOT_VERY_FAST_IN_LOW_RESOLUTION,NULL,NULL,FALSE,TRUE);
      };
   };
   write_ini_entry("Switches","ToonTalkStarted","0"); 
	// prior to 150105 was AC(IDC_SWITCHES),AC(IDC_TOONTALK_STARTED) but if aborting loading may not be initialized yet
// commented out the following on 221199 to be more general and it no longer runs after cleanup
//	string message;
	// why doesn't the following at least use C(xx) ??
//	switch (tt_language) {
//		case SWEDISH:
//			message = _T("Vill du se din loggfil?");
//			break;
//		default:
//			message = _T("Do you want to see your log file?"); 
//	};
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
   if (elapsed_time > 0 && frame_count > 0) { // don't bother to show this if did time travelling - new on 050803
	   tt_error_file() << frame_count << " frames in "
							 << (elapsed_time/1000) << " seconds ("
							 << (fps10*0.1) << " FPS)." << " (Started on frame " << tt_still_frame_count << ".)" << endl;
	   if (behind_time > 1000) {
		   tt_error_file() << "Fell " << (behind_time/1000L) << " seconds behind the original demo schedule." << endl;
	   };
   } else { // shouldn't really happen
#if TT_DEBUG_ON // new on 070404 since this is just confusing to users
      tt_error_file() << (tt_frame_number-tt_still_frame_count) << " frames." << endl;
#endif
   };
	string debug_tt_zip_file = NULL;
	if (tt_err_file_important) { // new on 211004
		if (tt_user_directory != NULL) {
			debug_tt_zip_file = new char[MAX_PATH];
			strcpy(debug_tt_zip_file,tt_user_directory);
			itoa(seconds_since_1970(),debug_tt_zip_file+strlen(debug_tt_zip_file),10);
			strcat(debug_tt_zip_file,".zip");
		};
		for (int i = 0; i < 3; i++) {
			tt_error_file() << "*************************************************************************" << endl;
		};
		tt_error_file() << "Sorry, something unexpected happened during this session." << endl;
		tt_error_file() << "YOU CAN HELP US IMPROVE TOONTALK IF YOU SEND US THE FILE NAMED:" << endl << debug_tt_zip_file << endl;
		for (int i = 0; i < 3; i++) {
			tt_error_file() << "*************************************************************************" << endl;
		};
		if (tt_command_line_for_exe_to_run_upon_shutdown != NULL) {
			tt_error_file() << AC(IDC_TOONTALK_EXE_NAME) << " " << tt_command_line_for_exe_to_run_upon_shutdown << endl;
		};
	};
	if (emergency) {
		tt_error_file() << "Emergency exit." << endl;
	} else {
		tt_error_file() << "Normal exit." << endl;
	};
//	tt_err_file.close(); // moved here on 260700
//	tt_err_file.open("pre_cleanup.txt",std::ios_base::out); // new on 260700
//   tt_err_file.flush(); // in case the following crashes the system
//#if TT_DEBUG_ON
//	if (tt_debug_mode != 270200 && tt_debug_mode != 241299)
//#endif
//	cleanup_for_exit(); // moved here from just after closing tt_error_file() so can add to log while cleaning up
//	tt_err_file.close(); // moved here from below on 290999 to avoid code guard errors about fflush
	// moved here on 230300 after clean up since MessageBox interferes with full screen
#if TT_DEBUG_ON
	if (tt_debug_mode == 110704) {
		tt_err_file_important = TRUE;
	};
#endif
	const string see_log_key = "AskIfWantToSeeLogWhenImportant"; // new on 151203
	int show_dialog_option = ini_int("NeverShowTheseDialogsAgain",see_log_key,FALSE,0);
	boolean show_log = // moved here on 100999 since used to crash NT and Win 2000
				(tt_err_file_name != NULL && !tt_temporary_user && 
				 // && tt_trouble_shooting removed on 171203 
				 (tt_err_file_important || tt_err_file_important_but_no_error) &&  // removed on 151203 and restored on 171203
				 // tt_err_file_important_but_no_error added on 031104
				 show_dialog_option == 0 && // added 151203
				 tt_error_file() != NULL && // new on 300904 so don't offer to show if couldn't create it -- updated to use tt_error_file() on 211004
             message_box(MB_ICONQUESTION|MB_YESNO|MB_SYSTEMMODAL,want_to_see_log,NULL,FALSE,see_log_key) == IDYES); // rewritten on 110602
//				 MessageBox(NULL,want_to_see_log,
//							   _T("ToonTalk"),MB_ICONQUESTION|MB_YESNO|MB_SYSTEMMODAL) == IDYES);
	cleanup_for_exit(); // moved here on 260700
//	MessageBox(NULL,"just ran cleanup_for_exit","TT",MB_OK); // just for testing!! 270105
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	tt_err_file.close(); // moved here on 260700
	if (debug_tt_zip_file != NULL) {
		if (tt_log_out_archive != NULL) {
			zip_file(debug_tt_zip_file,tt_log_out_archive);
		} else if (tt_log_out_file_name != NULL) {
			zip_file(debug_tt_zip_file,tt_log_out_file_name);
		} else if (tt_city_name != NULL) {
			string full_file_name = new_file_name(tt_city_name,"xml.cty");
			zip_file(debug_tt_zip_file,full_file_name);
			delete [] full_file_name;
		};
		if (tt_ini_file != NULL) {
			zip_file(debug_tt_zip_file,tt_ini_file);
		};
		if (tt_err_file_name != NULL) {
			zip_file(debug_tt_zip_file,tt_err_file_name);
		};
		if (tt_local_ini_file != NULL) {
			zip_file(debug_tt_zip_file,tt_local_ini_file);
		};
		if (tt_file_name != NULL) {
			string full_file_name = new_file_name(tt_file_name,"usr");
			zip_file(debug_tt_zip_file,full_file_name);
			delete [] full_file_name;
		};
	};
	// following moved here since used above
	if (tt_ini_file != NULL) delete [] tt_ini_file; // new on 030402 -- moved here on 021003 since used while shutting down
	if (tt_log_out_archive != NULL) delete [] tt_log_out_archive; // new on 130104
   if (tt_log_in_archive != NULL) delete [] tt_log_in_archive;
   if (tt_local_ini_file != NULL) delete [] tt_local_ini_file;
	if (tt_file_name != NULL) delete [] tt_file_name;
	if (tt_user_directory != NULL) delete [] tt_user_directory; // new on 251000
	if (tt_city_name != NULL) delete [] tt_city_name;
	if (debug_tt_zip_file != NULL) delete [] debug_tt_zip_file;
#if TT_DYNA_ZIP
	// also moved here on 211004 since used above (possibly)
	uninitialize_zip();
	uninitialize_unzip();
#endif
#if TT_DEBUG_ON
   add_to_log_if_interesting_error();
#endif
	delete [] want_to_see_log;
//#if TT_DEBUG_ON
//	if (tt_debug_mode == 270200 || tt_debug_mode == 241299) tt_err_file.close();
//#endif
//	if (replay_file_name() != NULL) {
//		tt_error_file() << "Originally it took " << tt_current_time/1000 << " seconds ("
//						<< ((tt_frame_number-still_frame_count)*1000L)/tt_current_time << " FPS)." << endl;
//	};
// check for Pentium divide bug
//	double x=8391670, y=3145727;
//	tt_error_file() << "Should be zero " << (x - ((x / y) * y)) << endl;
//	MessageBox(NULL,"Just before PostQuitMessage or shut_down","TT",MB_OK); // just for testing!! 270105
	if (emergency) {
		tt_main_window->shut_down();
	} else {
		PostQuitMessage(0);
//		MessageBox(NULL,"Just ran PostQuitMessage","TT",MB_OK); // just for testing!! 270105
	};
	if (tt_err_file_name != NULL) {
		if (show_log) {
			ShellExecuteG(NULL,"open",tt_err_file_name,"","",SW_SHOW);
		};
		delete [] tt_err_file_name;
	};
};

void update_clock() {
	// called after a pause
	millisecond new_time = timeGetTime();
//   initialization_time += new_time-previous_time; // don't count paused time
	previous_time = new_time;
};
