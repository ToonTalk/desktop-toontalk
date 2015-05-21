// Copyright (c) 1992-2001.  Ken Kahn, Animated Programs, All rights reserved.
// You may copy and modify this file so long as you keep this copyright notice.

#include <windows.h>

#include "c:\tt\strings.h"

extern "C" int __declspec(dllexport) WEP (int nParam);
extern "C" BOOL __declspec(dllexport) receive(HANDLE handle, void **data, char *types, void ***out, char **out_types, char **to_say, HINSTANCE string_library);


int FAR PASCAL LibMain (HANDLE , WORD , WORD , LPSTR ) {
	  return 1 ;
}

int __declspec(dllexport) WEP (int) {
	  return 1 ;
}

char *copy_string(char *source, int length) {
   if (source == NULL) return(NULL);
   if (length <= 0) length = strlen(source);
   char *destination = (char *) GlobalAlloc(0,length+1); // Can't use library's local storage
   memcpy(destination,source,length);
   destination[length] = '\0'; // terminate the string
   return(destination);
};

char *append_2strings(char *a, char *b) {
  if (a == NULL) return(b);
  if (b == NULL) return(a);
  int a_length = strlen(a);
  int b_length = strlen(b);
  char *result = (char *) GlobalAlloc(0,a_length+b_length+1);
  strcpy(result,a);
  strcpy(result+a_length,b);
  return(result);
};

char *append_3strings(char *a, char *b, char *c) {
  if (a == NULL) return(append_2strings(b,c));
  if (b == NULL) return(append_2strings(a,c));
  if (c == NULL) return(append_2strings(a,b));
  int a_length = strlen(a);
  int b_length = strlen(b);
  int c_length = strlen(c);
  char *result = (char *) GlobalAlloc(0,a_length+b_length+c_length+1);
  strcpy(result,a);
  strcpy(result+a_length,b);
  strcpy(result+a_length+b_length,c);
  return(result);
};

char *append_4strings(char *a, char *b, char *c, char *d) {
  if (a == NULL) return(append_3strings(b,c,d));
  if (b == NULL) return(append_3strings(a,c,d));
  if (c == NULL) return(append_3strings(a,b,d));
  if (d == NULL) return(append_3strings(a,b,c));
  int a_length = strlen(a);
  int b_length = strlen(b);
  int c_length = strlen(c);
  int d_length = strlen(d);
  char *result = (char *) GlobalAlloc(0,a_length+b_length+c_length+d_length+1);
  strcpy(result,a);
  strcpy(result+a_length,b);
  strcpy(result+a_length+b_length,c);
  strcpy(result+a_length+b_length+c_length,d);
  return(result);
};

char *append_6strings(char *a, char *b, char *c, char *d, char *e, char *f) {
  int a_length = strlen(a);
  int b_length = strlen(b);
  int c_length = strlen(c);
  int d_length = strlen(d);
  int e_length = strlen(e);
  int f_length = strlen(f);
  char *result = (char *) GlobalAlloc(0,a_length+b_length+c_length+d_length+e_length+f_length+1);
  strcpy(result,a);
  strcpy(result+a_length,b);
  strcpy(result+a_length+b_length,c);
  strcpy(result+a_length+b_length+c_length,d);
  strcpy(result+a_length+b_length+c_length+d_length,e);
  strcpy(result+a_length+b_length+c_length+d_length+e_length,f);
  return(result);
};

char *last_error() {
	char *error_message_buffer = NULL;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
					  NULL,GetLastError(),
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					  (LPTSTR) &error_message_buffer,0,NULL);
   return(error_message_buffer);
};

void say_last_error(char **to_say) {
	char *error_message_buffer = last_error();
	if (error_message_buffer != NULL) {
		*to_say = copy_string(error_message_buffer,0);
		LocalFree(error_message_buffer);
   };
};

const int max_resource_string_length = 400;

BOOL file_receive(HANDLE handle, void **data, char *types, char *label, char *country_code, 
						void ***out, char **out_types, char **to_say, BOOL *ok_to_speak, HINSTANCE string_library) {
   if (types == NULL) { // want help
		char buffer1[max_resource_string_length];
		LoadString(string_library,IDS_BIRD_TAKES_READ_AND_WRITE_MESSAGES,buffer1,max_resource_string_length);
		char buffer2[max_resource_string_length];
		LoadString(string_library,IDS_READ_WRITE_MESSAGE_DESCRIPTION,buffer2,max_resource_string_length);
	   char buffer3[max_resource_string_length];
	   LoadString(string_library,IDS_READ_WRITE_MESSAGE_DESCRIPTION2,buffer3,max_resource_string_length); // was buffer2 prior to 280201
	   char buffer4[max_resource_string_length];
	   LoadString(string_library,IDS_READ_MANUAL_FOR_MORE_HELP,buffer4,max_resource_string_length);
		*to_say = append_6strings(buffer1,label,". ",buffer2,buffer3,buffer4);
		*ok_to_speak = TRUE; // even though label may be spelled
		return(TRUE);
	};
	if (strcmp(types,"[SLB]") == 0) {
      if (stricmp((char *) data[0],"Read") == 0) {
         long number_of_bytes_to_read = (long) data[1]; // what if it is negative?
         DWORD number_of_bytes_read;
         char *buffer = (char *) GlobalAlloc(0,number_of_bytes_to_read+1);
         if (!ReadFile(handle,buffer,number_of_bytes_to_read,&number_of_bytes_read,NULL)) {
            say_last_error(to_say);
         };
         out_types[0] = copy_string("S",1);
         out[0] = (void * *) GlobalAlloc(0,sizeof(void*)); // new void*[1];
         buffer[number_of_bytes_read] = NULL; // terminate string
         out[0][0] = (void *) buffer;
         return(TRUE);
      } else if (stricmp((char *) data[0],"Read Bytes") == 0) {
         long number_of_bytes_to_read = (long) data[1]; // what if it is negative?
         DWORD number_of_bytes_read;
         char *buffer = (char *) GlobalAlloc(0,number_of_bytes_to_read+1);
         if (!ReadFile(handle,buffer,number_of_bytes_to_read,&number_of_bytes_read,NULL)) {
            say_last_error(to_say);
         };
			out_types[0] = (char *) GlobalAlloc(0,number_of_bytes_read+3); // for [] and terminator
			out[0] = (void * *) GlobalAlloc(0,sizeof(void*)*number_of_bytes_read);
			int out_type_index = 0;
         out_types[0][out_type_index++] = '[';
			for (DWORD i = 0; i < number_of_bytes_read; i++) {
				out_types[0][out_type_index++] = 'L';
				out[0][i] = (void *) buffer[i];
			};
         out_types[0][out_type_index++] = ']';
         out_types[0][out_type_index++] = '\0'; // terminate string
         return(TRUE);
      };
   } else if (strcmp(types,"[SSB]") == 0) {
      if (stricmp((char *) data[0],"Write") == 0) {
         char *buffer = (char *) data[1];
         long number_of_bytes_to_write = strlen(buffer);
         DWORD number_of_bytes_written;
         if (WriteFile(handle,buffer,number_of_bytes_to_write,&number_of_bytes_written,NULL)) {
            out_types[0] = copy_string("L",1);
            out[0] = (void * *) GlobalAlloc(0,sizeof(void*));
            out[0][0] = (void *) number_of_bytes_written;
            return(TRUE);
         } else {
            say_last_error(to_say);
			   return(FALSE);
			};
      };
	} else if (strncmp(types,"[S[",3) == 0) { // looking for [S[LL...LL]B] where S is Write Bytes
		if (stricmp((char *) data[0],"Write Bytes") == 0) {
			int index = 3; // already checked first 3 characters of type
			while (types[index] != '\0') {
				if (types[index] == ']') { // OK so far
					if (types[index+1] == 'B' && types[index+2] == ']') { // everything fine
						index -= 3; // index is now the size of the box of numbers
						unsigned char *buffer = new unsigned char[index];
						for (int i = 0; i < index; i++) {
							buffer[i] = (unsigned char) data[i+1];
						};
                  DWORD number_of_bytes_written;
                  if (WriteFile(handle,buffer,index,&number_of_bytes_written,NULL)) {
                     out_types[0] = copy_string("L",1);
                     out[0] = (void * *) GlobalAlloc(0,sizeof(void*));
                     out[0][0] = (void *) number_of_bytes_written;
                     return(TRUE);
						} else {
                     say_last_error(to_say);
			            return(FALSE);
						};
					} else break; // fall through
				} else if (types[index] == 'L') {
					index++;
				} else { // no good
					break; // fall through to code generated to_say help
				};
			};
		};
	} else if (strcmp(types,"H") == 0) { // destroyed
		if (!CloseHandle(handle)) {
			say_last_error(to_say);
		};
		return(FALSE);
   };
	char buffer1[max_resource_string_length];
	LoadString(string_library,IDS_IMPROPER_READ_OR_WRITE_MESSAGE,buffer1,max_resource_string_length);
	char buffer2[max_resource_string_length];
	LoadString(string_library,IDS_READ_WRITE_MESSAGE_DESCRIPTION,buffer2,max_resource_string_length);
   *to_say = append_2strings(buffer1,buffer2);
	*ok_to_speak = TRUE;
   return(FALSE);
};

BOOL __declspec(dllexport) receive(HANDLE handle, void **data, char *types, char *label, char *country_code, 
											  void ***out, char **out_types, char **to_say, BOOL *ok_to_speak, HINSTANCE string_library) {
   if (types == NULL) { // asking for help
		char buffer1[max_resource_string_length];
		LoadString(string_library,IDS_CREATE_FILE_HELP,buffer1,max_resource_string_length);
		char buffer2[max_resource_string_length];
		LoadString(string_library,IDS_CREATE_FILE_HELP2,buffer2,max_resource_string_length); // was missing prior to 280201
		char buffer3[max_resource_string_length];
		LoadString(string_library,IDS_BIRD_TAKES_MESSAGE_TO_OPEN_A_FILE,buffer3,max_resource_string_length);
		*to_say = append_3strings(buffer1,buffer2,buffer3);
		*ok_to_speak = TRUE;
		return(TRUE);
	};
	if (strcmp(types,"[SSB]") == 0) {
		char *selector = (char *) data[0];
		DWORD creation;
		char *device_control_string = NULL;
		char name[MAX_PATH];
      if (stricmp(selector,"Create File") == 0) {
			creation = CREATE_NEW;
			strcpy(name,(char *) data[1]);
		} else if (stricmp(selector,"Open") == 0) { // could be file, port, etc.
			creation = OPEN_EXISTING;
			strcpy(name,(char *) data[1]);
		} else if (stricmp(selector,"Open Serial Port") == 0) {
			creation = OPEN_EXISTING;
			device_control_string = (char *) data[1];
			strncpy(name,device_control_string,4); // e.g. COM1
         name[4] = '\0'; // terminate string
		} else {
	      char buffer1[max_resource_string_length];
	      LoadString(string_library,IDS_IMPROPER_CREATE_FILE_MESSAGE,buffer1,max_resource_string_length);
	      char buffer2[max_resource_string_length];
	      LoadString(string_library,IDS_CREATE_FILE_HELP,buffer2,max_resource_string_length);
         *to_say = append_2strings(buffer1,buffer2);
			*ok_to_speak = TRUE;
			return(FALSE);
		};
		HANDLE file = CreateFile(name,GENERIC_READ|GENERIC_WRITE,0,NULL,creation,FILE_ATTRIBUTE_NORMAL,NULL); 
		if (file == INVALID_HANDLE_VALUE) {
			char *error_message_buffer = last_error();
			char buffer[max_resource_string_length];
		   LoadString(string_library,IDS_TROUBLE_OPENING_A_FILE,buffer,max_resource_string_length);
			*to_say = append_4strings(buffer, name, ". ", error_message_buffer);
			if (error_message_buffer != NULL) LocalFree(error_message_buffer);
			return(FALSE);
		};
		if (device_control_string != NULL) {
		   DCB device_control_block;   
			if (!BuildCommDCB(device_control_string,&device_control_block) ||
             !SetCommState(file,&device_control_block)) {
				say_last_error(to_say);
			};
		};
      out_types[0] = copy_string("[H]",3);
      out[0] = (void * *) GlobalAlloc(0,3*sizeof(void*)); // new void*[3];
      out[0][0] = (void *) file_receive;  // behavior
      out[0][1] = (void *) file; // file handle
      out[0][2] = copy_string(name,0); // use the file name as a label
	   return(TRUE);
	} else if (strcmp(types,"H") == 0) { // destroyed
		return(FALSE);
	};
	char buffer1[max_resource_string_length];
	LoadString(string_library,IDS_CREATE_FILE_HELP,buffer1,max_resource_string_length);
	char buffer2[max_resource_string_length];
	LoadString(string_library,IDS_CREATE_FILE_HELP2,buffer2,max_resource_string_length);
	char buffer3[max_resource_string_length];
	LoadString(string_library,IDS_READ_MANUAL_FOR_MORE_HELP,buffer3,max_resource_string_length);
   *to_say = append_3strings(buffer1,buffer2,buffer3);
	*ok_to_speak = TRUE;
   return(FALSE);
};

