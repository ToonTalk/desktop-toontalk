// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.

// output routines for generating Java code
// Some day this will create an intermediate structure that can
// be optimized

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif   
#if !defined(__TT_GLOBALS_H)   
#include "globals.h"
#endif   
#if !defined(__TT_UTILS_H)
#include "utils.h"
#endif
#if !defined(__TT_SPRITE_H)
#include "sprite.h"
#endif
#if !defined(__TT_SPRITES_H)
#include "sprites.h"
#endif
#if !defined(__TT_HELP_H)
#include "help.h"
#endif
#if !defined(__TT_STRINGS_H)
#include "strings.h"
#endif

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void java_quoted(java_stream &out, const_java_string arg) { // new on 080900
	// replaces \ with \\ for quoted path names
	int length = strlen(arg);
	for (int i = 0; i < length; i++) {
		if (arg[i] == '\\') {
			out << arg[i];
		};
		out << arg[i];
	};
};

void java_declare1(java_stream &out, const_java_string type, const_java_string variable) {
   // just the first part
   java_indent(out);
   out << type << " " << variable << " = ";
};

void java_declare(java_stream &out, const_java_string type, const_java_string variable,
                  Sprite *initial_value) {
   java_indent(out);
   out << type << " " << variable;
   if (initial_value != NULL) {
      out << " = ";
      initial_value->java(out);
   };
   out << ";" << endl;
};

void java_declare(java_stream &out, const_java_string type, const_java_string variable,
                  const_java_string initial_value) {
      java_indent(out);
   out << type << " " << variable;
   out << " = " << initial_value << ";" << endl;
};

void java_declare_new(java_stream &out, const_java_string type, const_java_string variable,
                      const_java_string class_name, const_java_string arg1, const_java_string arg2) {
   java_indent(out);
   out << type << " " << variable << " = new " << class_name << "(";
   if (arg1 != NULL) {
      out << arg1;
      if (arg2 != NULL) {
         out << ", " << arg2;
      };
   };
   out << ");" << endl;
};

void java_declare_new(java_stream &out, const_java_string type, const_java_string variable,
                      const_java_string class_name, int arg1) {
   java_indent(out);
   out << type << " " << variable << " = new " << class_name << "(" << arg1 << ");" << endl;
};

void java_declare_new_array(java_stream &out, const_java_string class_name, const_java_string variable, int size) {
   java_indent(out);
   out << class_name << " " << variable << "[] = new " << class_name << "[" << size << "];" << endl;
};

void java_new(java_stream &out, const_java_string class_name, int arg1) {
   out << "new " << class_name << "(" << arg1 << ")";
};

void java_new(java_stream &out, const_java_string class_name, int arg1, character arg2) {
   out << "new " << class_name << "(" << arg1 << ", '" << arg2 << "')";
};

void java_new(java_stream &out, const_java_string class_name, int arg1, character arg2, int arg3, int arg4, int arg5) { // new on 230700
   out << "new " << class_name << "(" << arg1 << ", '" << arg2 << "', " << arg3 << ", " << arg4 << ", " << arg5 << ")";
};

void java_new(java_stream &out, const_java_string class_name, int arg1, java_string arg2, int arg3, int arg4, int arg5) { // new on 030503
	// note that arg2 is like TT.SUBTRACT_FROM so it isn't quoted
   out << "new " << class_name << "(" << arg1 << ", " << arg2 << ", " << arg3 << ", " << arg4 << ", " << arg5 << ")";
};

void java_new(java_stream &out, const_java_string class_name, java_string arg1, java_string arg2, int arg3, int arg4, int arg5) { // new on 030503
	// note that arg2 is like TT.SUBTRACT_FROM so it isn't quoted
   out << "new " << class_name << "(\"" << arg1 << "\", " << arg2 << ", " << arg3 << ", " << arg4 << ", " << arg5 << ")";
};

void java_new(java_stream &out, const_java_string class_name, const_java_string arg1) {
   out << "new " << class_name << "(" << arg1 << ")";
};

void java_new(java_stream &out, const_java_string class_name, character arg1) {
   out << "new " << class_name << "('" << arg1 << "')";
};

void java_new(java_stream &out, const_java_string class_name, long args[], int arg_count, double arg4_double, double arg5_double, const_java_string arg2, const_java_string arg3, const_java_string arg4) {
   // added arg4_double and arg5_double since speeds are not double floats
	out << "new " << class_name << "(";
   for (int i = 0; i < arg_count; i++) {
		if (i == 4 && arg4_double != 0.0) {
			out << arg4_double;
		} else if (i == 5 && arg5_double != 0.0) {
			out << arg5_double;
		} else {
			out << args[i];
		};
      if (i < arg_count-1) out << ", ";
   };
   if (arg2 != NULL) {
      out << ", " << arg2;
      if (arg3 != NULL) {
         out << ", " << arg3;
			if (arg4 != NULL) {
				out << ", " << arg4;
			};
      };
   };
   out << ")";
};

void java_new(java_stream &out, const_java_string class_name, Sprite *arg1, Sprite *arg2, Sprite *arg3, Sprite *arg4, Sprite *arg5, Sprite *arg6, Sprite *arg7, string arg8) {
   out << "new " << class_name << "(";
   if (arg1 == NULL) {
      out << S(IDS_JAVA_TT_DOT_NOTHING);
   } else {
      arg1->java(out);
   };
   out << ", ";
   if (arg2 == NULL) {
      out << S(IDS_JAVA_TT_DOT_NOTHING);
   } else {
      arg2->java(out);
   };
   out << ", ";
   if (arg3 == NULL) {
      out << S(IDS_JAVA_TT_DOT_NOTHING);
   } else {
      arg3->java(out);
   };
   out << ", ";
   if (arg4 == NULL) {
      out << S(IDS_JAVA_TT_DOT_NOTHING);
   } else {
      arg4->java(out);
   };
   out << ", ";
   if (arg5 == NULL) {
      out << S(IDS_JAVA_TT_DOT_NOTHING);
   } else {
      arg5->java(out);
   };
   out << ", ";
   if (arg6 == NULL) {
      out << S(IDS_JAVA_TT_DOT_NOTHING);
   } else {
      arg6->java(out);
   };
   out << ", ";
   if (arg7 == NULL) {
      out << S(IDS_JAVA_TT_DOT_NOTHING);
   } else {
      arg7->java(out);
   };
	if (arg8 != NULL) { // new on 150201
		out << ", " << arg8;
	};
   out << ")";
};

void java_new(java_stream &out, const_java_string class_name, Sprite *arg1, long args[], int arg_count, double arg4_double, double arg5_double, const_java_string arg2, const_java_string arg3, const_java_string arg4) {
   out << "new " << class_name << "(";
   arg1->java(out);
   out << ", ";
   for (int i = 0; i < arg_count; i++) {
      if (i == 4 && arg4_double != 0.0) {
			out << arg4_double;
		} else if (i == 5 && arg5_double != 0.0) {
			out << arg5_double;
		} else {
			out << args[i];
		};
      if (i < arg_count-1) out << ", ";
   };
   if (arg2 != NULL) {
      out << ", " << arg2;
      if (arg3 != NULL) {
         out << ", " << arg3;
			if (arg4 != NULL) {
				out << ", " << arg4;
			};
      };
   };
   out << ")";
};

void java_new(java_stream &out, const_java_string class_name, const_java_string arg1, long args[], int arg_count, double arg4_double, double arg5_double, const_java_string arg2, const_java_string arg3, const_java_string arg4) {
   out << "new " << class_name << "(" << arg1 << ", ";
   for (int i = 0; i < arg_count; i++) {
      if (i == 4 && arg4_double != 0.0) {
			out << arg4_double;
		} else if (i == 5 && arg5_double != 0.0) {
			out << arg5_double;
		} else {
			out << args[i];
		};
      if (i < arg_count-1) out << ", ";
   };
   if (arg2 != NULL) {
      out << ", " << arg2;
      if (arg3 != NULL) {
         out << ", " << arg3;
			if (arg4 != NULL) {
				out << ", " << arg4;
			};
      };
   };
   out << ")";
};

void java_new(java_stream &out, const_java_string class_name, int arg1, const_java_string arg2, long args[], int arg_count, double arg4_double, double arg5_double, const_java_string arg3, const_java_string arg4, const_java_string arg5) {
   out << "new " << class_name << "(" << arg1 << ", " << arg2 << ", ";
   for (int i = 0; i < arg_count; i++) {
      if (i == 4 && arg4_double != 0.0) {
			out << arg4_double;
		} else if (i == 5 && arg5_double != 0.0) {
			out << arg5_double;
		} else {
			out << args[i];
		};
      if (i < arg_count-1) out << ", ";
   };
   if (arg3 != NULL) {
      out << ", " << arg3;
      if (arg4 != NULL) {
         out << ", " << arg4;
			if (arg5 != NULL) {
				out << ", " << arg5;
			};
      };
   };
   out << ")";
};

void java_new(java_stream &out, const_java_string class_name, const_java_string arg1, character arg2) {
   out << "new " << class_name << "(" << arg1 << ", '" << arg2 << "')";
};

void java_new(java_stream &out, const_java_string class_name, const_java_string arg1, character arg2, int arg3) {
   out << "new " << class_name << "(" << arg1 << ", '" << arg2 << "', " << arg3 << ")";
};

void java_new(java_stream &out, const_java_string class_name, const_java_string arg1, character arg2, const_java_string arg3) {
   out << "new " << class_name << "(" << arg1 << ", '" << arg2 << "', " << arg3 << ")";
};

void java_new(java_stream &out, const_java_string class_name, const_java_string arg1, character arg2, int arg3, const_java_string arg4) {
   out << "new " << class_name << "(" << arg1 << ", '" << arg2 << "', " << arg3 << ", " << arg4 << ")";
};

void java_new(java_stream &out, const_java_string class_name, long arg1, const_java_string arg2, character arg3, int arg4, const_java_string arg5) {
   out << "new " << class_name << "(" << arg1 << ", " << arg2 << ", '" << arg3 << "', " << arg4;
   if (arg5 != NULL) {
      out << ", " << arg5;
   };
   out << ")";
};

void java_new(java_stream &out, const_java_string class_name, long arg1, const_java_string arg2, character arg3, const_java_string arg4, const_java_string arg5) {
   out << "new " << class_name << "(" << arg1 << ", " << arg2 << ", '" << arg3 << "', " << arg4;
   if (arg5 != NULL) {
      out << ", " << arg5;
   };
   out << ")";
};

void java_new(java_stream &out, const_java_string class_name, long arg1, const_java_string arg2, character arg3, const_java_string arg4, const_java_string arg5, const_java_string arg6) {
   out << "new " << class_name << "(" << arg1 << ", " << arg2 << ", '" << arg3 << "', " << arg4;
   if (arg5 != NULL) {
      out << ", " << arg5;
   };
	if (arg6 != NULL) {
      out << ", " << arg6;
   };
   out << ")";
};

void java_new(java_stream &out, const_java_string class_name, long arg1, const_java_string arg2, string arg3, const_java_string arg4, const_java_string arg5, const_java_string arg6) {
   // new on 040503 -- arg3 is a named constant - not a string so isn't quoted
	out << "new " << class_name << "(" << arg1 << ", " << arg2 << ", " << arg3 << ", " << arg4;
   if (arg5 != NULL) {
      out << ", " << arg5;
   };
	if (arg6 != NULL) {
      out << ", " << arg6;
   };
   out << ")";
};

void java_new_quote_arg1(java_stream &out, const_java_string class_name, const_java_string arg1, const_java_string arg2) {
//   out << "new " << class_name << "(\"" << arg1 << "\"";
	// updated on 080900 to deal with \ becoming \\ - e.g. in path names
	out << "new " << class_name << "(\"";
	java_quoted(out,arg1);
	out << "\"";
   if (arg2 != NULL) out << ", " << arg2;
   out << ")";
};

void java_new_quote_arg1(java_stream &out, const_java_string class_name, const_java_string arg1, long args[], int arg_count, double arg4_double, double arg5_double, const_java_string arg2, const_java_string arg3, const_java_string arg4) {
   out << "new " << class_name << "(\"";
	java_quoted(out,arg1);
	out << "\","; // comma added 281100
   for (int i = 0; i < arg_count; i++) {
      if (i == 4 && arg4_double != 0.0) {
			out << arg4_double;
		} else if (i == 5 && arg5_double != 0.0) {
			out << arg5_double;
		} else {
			out << args[i];
		};
      if (i < arg_count-1) out << ", ";
   };
   if (arg2 != NULL) {
      out << ", " << arg2;
      if (arg3 != NULL) {
         out << ", " << arg3;
			if (arg4 != NULL) {
				out << ", " << arg4;
			};
      };
   };
   out << ")";
};

void java_new_quote_arg1(java_stream &out, const_java_string class_name, const_java_string arg1, const_java_string arg2, long args[], int arg_count, double arg4_double, double arg5_double, const_java_string arg3, const_java_string arg4, const_java_string arg5) {
   out << "new " << class_name << "(\"";
	java_quoted(out,arg1);
	out << "\", " << arg2 << ", ";
   for (int i = 0; i < arg_count; i++) {
      if (i == 4 && arg4_double != 0.0) {
			out << arg4_double;
		} else if (i == 5 && arg5_double != 0.0) {
			out << arg5_double;
		} else {
			out << args[i];
		};
      if (i < arg_count-1) out << ", ";
   };
	if (arg3 != NULL) {
		out << ", " << arg3;
		if (arg4 != NULL) {
			out << ", " << arg4;
			if (arg5 != NULL) {
				out << ", " << arg5;
			};
		};
	};
   out << ")";
};

const_java_string java_boolean(boolean flag) {
   if (flag) {
      return(S(IDS_JAVA_TRUE));
   } else {
      return(S(IDS_JAVA_FALSE));
   };
};

string java_array(const_java_string array_name, int index) {
	int length = _tcslen(array_name)+15;
   string reference_string = new character[length];
   output_string_stream reference(reference_string,length);
   reference << array_name << "[" << index << "]";
   reference.put('\0'); //terminate string
   return(reference_string);
};

void java_assign(java_stream &out, const_java_string variable,
                 Path *path,
                 boolean first, const_java_string set_new_value, const_java_string set_method) {
   java_indent(out);
   out << variable;
   out << " = ";
   path->java(out,first,set_new_value,set_method);
   out << ";" << endl;
};

void java_assign(java_stream &out, const_java_string variable, const_java_string value) {
   java_indent(out);
   out << variable << " = " << value;
   out << ";" << endl;
};

void java_assign(java_stream &out, const_java_string variable, Sprite *value) {
   java_indent(out);
   out << variable << " = ";
   value->java(out);
   out << ";" << endl;
};

void java_assign(java_stream &out, Path *variable, const_java_string value) {
   java_indent(out);
   variable->java(out);
   out << " = ";
   out << value;
   out << ";" << endl;
};

void java_assign_array(java_stream &out, const_java_string variable, int index, Sprite *value) {
   java_indent(out);
   out << variable << "[" << index << "] = ";
   value->java(out);
   out << ";" << endl;
};

void java_assign_array(java_stream &out, const_java_string variable, int index, int value) {
   java_indent(out);
   out << variable << "[" << index << "] = " << value << ";" << endl;
};

void java_assign_array_new(java_stream &out, const_java_string variable, const_java_string index, const_java_string value) {
   java_indent(out);
   out << variable << "[" << index << "] = new " << value << "();";
};

void java_assign_send(java_stream &out, Path *variable, Path *value,
                      const_java_string method_name, const_java_string first_arg, const_java_string second_arg) {
   java_indent(out);
   if (variable != NULL) {
      variable->java(out);
      out << " = ";
   };
   value->java(out);
   if (method_name != NULL) {
      out << "." << method_name << "(";
      if (first_arg != NULL) {
         out << first_arg;
         if (second_arg != NULL) {
            out << ", " << second_arg;
         };
      };
      out << ")";
   };
   out << ";" << endl;
};

void java_assign_send(java_stream &out, Path *variable, const_java_string value,
                      const_java_string method_name, const_java_string first_arg, const_java_string second_arg) {
   java_indent(out);
   if (variable != NULL) {
      variable->java(out);
      out << " = ";
   };
   out << value;
   if (method_name != NULL) {
      out << "." << method_name << "(";
      if (first_arg != NULL) {
         out << first_arg;
         if (second_arg != NULL) {
            out << ", " << second_arg;
         };
      };
      out << ")";
   };
   out << ";" << endl;
};

void java_assign_send(java_stream &out, Path *variable, const_java_string value, 
							 const_java_string method_name, Path *first_arg, const_java_string second_arg) { // new on 140101
	java_indent(out);
   if (variable != NULL) {
      variable->java(out);
      out << " = ";
   };
   out << value;
   if (method_name != NULL) {
      out << "." << method_name << "(";
      if (first_arg != NULL) {
         first_arg->java(out);
         if (second_arg != NULL) {
            out << ", " << second_arg;
         };
      };
      out << ")";
   };
   out << ";" << endl;
};

void java_assign_send(java_stream &out, const_java_string variable, const_java_string value, 
							 const_java_string method_name, Path *first_arg, const_java_string second_arg) { // new on 140101
	java_indent(out);
   if (variable != NULL) {
      out << variable << " = ";
   };
   out << value;
   if (method_name != NULL) {
      out << "." << method_name << "(";
      if (first_arg != NULL) {
         first_arg->java(out);
         if (second_arg != NULL) {
            out << ", " << second_arg;
         };
      };
      out << ")";
   };
   out << ";" << endl;
};

void java_assign_send(java_stream &out, const_java_string variable,
                      Path *path, const_java_string method_name,
                      const_java_string first_arg, const_java_string second_arg, const_java_string third_arg) {
   java_indent(out);
   if (variable != NULL) out << variable << " = ";
   path->java(out);
   out << "." << method_name << "(";
   if (first_arg != NULL) {
      out << first_arg;
      if (second_arg != NULL) {
         out << ", " << second_arg;
			if (third_arg != NULL) { // new on 070205
				out << ", " << third_arg;
			};
      };
   };
   out << ");" << endl;
};

void java_assign_send(java_stream &out, const_java_string variable,
                      const_java_string subject, const_java_string method_name,
                      const_java_string first_arg, const_java_string second_arg, const_java_string third_arg) {
   java_indent(out);
   if (variable != NULL) out << variable << " = ";
   out << subject << "." << method_name << "(";
   if (first_arg != NULL) {
      out << first_arg;
      if (second_arg != NULL) {
         out << ", " << second_arg;
			if (third_arg != NULL) { // new on 070205
				out << ", " << third_arg;
			};
      };
   };
   out << ");" << endl;
};

void java_assign_send(java_stream &out, const_java_string variable,
                      const_java_string subject, const_java_string method_name,
                      Path *arg) {
   java_indent(out);
   if (variable != NULL) out << variable << " = ";
   out << subject << "." << method_name << "(";
   arg->java(out);
   out << ");" << endl;
};

void java_assign_new(java_stream &out, const_java_string variable,
                     const_java_string class_name,
                     const_java_string first_arg, const_java_string second_arg) {
   java_indent(out);
   out << variable << " = new " << class_name << "(";
   if (first_arg != NULL) {
      out << first_arg;
      if (second_arg != NULL) {
         out << ", " << second_arg;
      };
   };
   out << ");" << endl;
};

void java_assign_new(java_stream &out, const_java_string variable,
                     const_java_string class_name, int arg1, int arg2, int arg3) {
   java_indent(out);
   out << variable << " = new " << class_name << "(" << arg1 << ", " << arg2 << ", " << arg3 << ");" << endl;
};

void java_declare_new(java_stream &out, const_java_string variable, const_java_string class_name, Sprite *arg1, long args[], int arg_count, const_java_string arg2, const_java_string arg3) {
   java_indent(out);
   out << class_name << " " << variable << " = new " << class_name << "(";
   arg1->java(out);
   out << ", ";
   for (int i = 0; i < arg_count; i++) {
      out << args[i];
      if (i < arg_count-1) out << ", ";
   };
   if (arg2 != NULL) {
      out << ", " << arg2;
      if (arg3 != NULL) {
         out << ", " << arg3;
      };
   };
   out << ");" << endl;
};

void java_send(java_stream &out, const_java_string variable,
               const_java_string method_name,
               const_java_string first_arg, const_java_string second_arg) {
   java_indent(out);
   out << variable;
   out << "." << method_name << "(";
   if (first_arg != NULL) {
      out << first_arg;
      if (second_arg != NULL) {
         out << ", " << second_arg;
      };
   };
   out << ");" << endl;
};

void java_send(java_stream &out, const_java_string variable, const_java_string method_name, int arg1, int arg2, int arg3, const_java_string arg4, const_java_string arg5) {
   java_indent(out);
   out << variable;
   out << "." << method_name << "(" << arg1 << ", " << arg2 << ", " << arg3;
   if (arg4 != NULL) {
      out << ", " << arg4;
      if (arg5 != NULL) {
         out << ", " << arg5;
      };
   };
   out << ");" << endl;
};

void java_send_quote_arg6(java_stream &out, const_java_string variable, const_java_string method_name, int arg1, int arg2, int arg3, int arg4, int arg5, const_java_string arg6, int arg7) {
   if (arg6 == NULL) {
		return; // warn?? - new on 031100
	};
	java_indent(out);
   out << variable;
   out << "." << method_name << "(" << arg1 << ", " << arg2 << ", " << arg3 << ", " << arg4 << ", " << arg5 <<", \"" << arg6 << "\", " << arg7 << ");" << endl;
};

void java_send(java_stream &out, const_java_string variable,
               const_java_string method_name, Path *first_arg, const_java_string second_arg) {
   java_indent(out);
   out << variable;
   out << "." << method_name << "(";
   if (first_arg != NULL) {
      first_arg->java(out);
      if (second_arg != NULL) {
         out << ", " << second_arg;
      };
   };
   out << ");" << endl;
};

void java_send(java_stream &out, Path *path,
               boolean first, const_java_string set_new_value, const_java_string set_mode) {
   java_indent(out);
   path->java(out,first,set_new_value,set_mode);
   out << ";" << endl;
};

void java_send(java_stream &out, Path *path,
               const_java_string method_name, const_java_string first_arg, const_java_string second_arg) {
   java_indent(out);
   path->java(out);
   out << "." << method_name << "(";
   if (first_arg != NULL) {
      out << first_arg;
      if (second_arg != NULL) {
         out << ", " << second_arg;
      };
   };
   out << ");" << endl;
};

void java_send(java_stream &out, const_java_string variable,
               const_java_string method_name, int first_arg, const_java_string second_arg, const_java_string third_arg) {
   java_indent(out);
   out << variable << "." << method_name << "(" << first_arg;
   if (second_arg != NULL) {
      out << ", " << second_arg;
      if (third_arg != NULL) {
         out << ", " << third_arg;
      };
   };
   out << ");" << endl;
};

void java_send(java_stream &out, const_java_string variable,
               const_java_string method_name, int first_arg, Sprite *second_arg) {
   java_indent(out);
   out << variable << "." << method_name << "(" << first_arg;
   if (second_arg != NULL) {
      out << ", ";
      second_arg->java(out);
   };
   out << ");" << endl;
};

void java_send(java_stream &out, const_java_string variable, const_java_string method_name, Sprite *arg) {
   java_indent(out);
   out << variable << "." << method_name << "(";
   arg->java(out);
   out << ");" << endl;
};

void java_send_this(java_stream &out, const_java_string method_name,
                    const_java_string first_arg, const_java_string second_arg) {
   java_indent(out);
   out << method_name << "(";
   if (first_arg != NULL) {
      out << first_arg;
      if (second_arg != NULL) {
         out << ", " << second_arg;
      };
   };
   out << ");" << endl;
};

void java_send_this(java_stream &out, const_java_string method_name,
                    int first_arg, Sprite *second_arg) {
   java_indent(out);
   out << method_name << "(" << first_arg;
	if (second_arg != NULL) {
		out << ", ";
		second_arg->java(out);
	};
	out << ");" << endl;
};

void java_send_this(java_stream &out, const_java_string method_name,
                    Sprite *first_arg, const_java_string second_arg) {
   java_indent(out);
   out << method_name << "(";
   if (first_arg != NULL) {
      first_arg->java(out);
      if (second_arg != NULL) {
         out << ", " << second_arg;
      };
   };
   out << ");" << endl;
};

void java_send_array_arg(java_stream &out, const_java_string path, const_java_string method_name, const_java_string array, const_java_string index) {
   java_indent(out);
   out << path << "." << method_name << "(" << array << "[" << index << "]);" << endl;
};

void java_just_send(java_stream &out, const_java_string variable, const_java_string method_name, int arg1) {
   out << variable << "." << method_name << "(" << arg1 << ")";
};

void java_just_send(java_stream &out, const_java_string variable, const_java_string method_name, const_java_string arg1) {
   out << variable << "." << method_name << "(";
   if (arg1 != NULL) out << arg1;
   out << ")";
};

void java_just_send(java_stream &out, Path *path, const_java_string method_name, int arg1) {
   path->java(out);
   out << "." << method_name << "(" << arg1 << ")";
};

void java_begin_block(java_stream &out) {
   java_indent(out);
   out << "{" << endl;
};

void java_end_block(java_stream &out) {
   java_indent(out);
   out << "};" << endl;
};

void java_comment(java_stream &out, const_java_string comment) {
	if (comment == NULL) return;
   java_indent(out);
	out << "// "; 
	// replacing new lines with space new on 210700 since can include user provided names
	int length = strlen(comment);
	for (int i = 0; i < length; i++) {
      if (new_line(comment[i])) { // prior to 090802 was == '\r'
			out << " ";
		} else {
			out << comment[i];
		};
	};
	out << endl;
};

java_string java_quote(const_java_string arg) {
   if (arg == NULL) return(NULL);
   int size = _tcslen(arg);
	int new_size = size*2+3; // doubled on 150201 in case all new lines
   string quote = new character[new_size]; 
   output_string_stream quote_stream(quote,new_size);
   quote_stream << '"'; 
	for (int i = 0; i < size; i++) {
      if (new_line(arg[i])) { // prior to 090802 was == '\r'
			quote_stream << "\\r";
		} else {
			quote_stream.put(arg[i]);
		};
	};
	quote_stream << '"';
   quote_stream.put('\0');
   return(quote);
};

java_string java_new(const_java_string class_name, const_java_string arg1) {
   int size = _tcslen(class_name)+8;
   if (arg1 != NULL) size += _tcslen(arg1);
   string code = new character[size];
   output_string_stream code_stream(code,size);
   code_stream << "new " << class_name << "(";
   if (arg1 != NULL) {
      code_stream << arg1;
   };
   code_stream << ")";
   code_stream.put('\0');
   return(code);
};

string java_array(const_java_string array_name, const_java_string index) {
   int size = _tcslen(array_name)+4+_tcslen(index);
   string code = new character[size];
   output_string_stream code_stream(code,size);
   code_stream << array_name << "[" << index << "]";
   code_stream.put('\0');
   return(code);
};

/* obsoleted on 250101
string java_name_from_text(const_java_string text) {
	// removes spaces and capitalizes following Java conventions
	int length = _tcslen(text);
	string result = new character[length+1];
	int j = 0;
	for (int i = 0; i < length; i++) {
		if (text[i] == ' ' || i == 0) {
			if (i > 0) i++; // skip over it
			character first_letter[2];
			first_letter[0] = text[i];
			first_letter[1] = NULL;
			upper_case(first_letter);
			result[j++] = first_letter[0];
		} else {
			result[j++] = text[i];
		};
	};
	result[j] = '\0';
	return(result);
};
*/

java_string java_type_from_sprite_type(SpriteType type) { // new on 250101
	switch (type) {
		case PICTURE:
			return("A_PICTURE");
		case TEXT:
			return("A_TEXT_PAD");
		case INTEGER:
			return("AN_INTEGER_PAD");
		case PROGRAM_PAD:
			return("A_NOTEBOOK");
		case CUBBY:
			return("A_BOX");
		case BOMB:
			return("A_BOMB");
		case BIRD:
			return("A_BIRD");
		case NEST:
			return("A_NEST");
		case SOUND:
			return("A_SOUND");
		case REMOTE_PICTURE:
			return("A_REMOTE_CONTROL");	
		case ROBOT:
			return("A_ROBOT");
		default:
			return("A_PICTURE"); // warn?
			};
};

void java_if(java_stream &out, const_java_string part1, const_java_string part2, const_java_string part3, const_java_string part4) {
	java_indent(out);
	out << "if (" << part1 << part2 << part3 << part4 << ")";
};


java_string java_language(NaturalLanguage language) { // new on 141100
	// could have been added to javacom.rc instead
	java_string names[] = {"TT.DEFAULT_LANGUAGE","TT.AMERICAN_ENGLISH","TT.SWEDISH",
								  "TT.BRAZILIAN_PORTUGUESE","TT.GERMAN","TT.FRENCH","TT.SPANISH",
								  "TT.BRITISH_ENGLISH","TT.TURKISH","TT.JAPANESE","TT.EUROPEAN_PORTUGUESE",
								  "TT.ITALIAN",
								  "TT.UNKNOWN_LANGUAGE"};

	return(names[(int) language]);
};

Sprites *java_sprites = NULL;

boolean has_java_index(Sprite *sprite) {
	if (java_sprites == NULL) return(FALSE);
	return(java_sprites->member_cons(sprite) != NULL);
};

int java_index(Sprite *sprite) { // generalized and moved here on 041200
   int answer = 1; 
	Sprites *remaining = java_sprites;
	while (remaining != NULL) {
		if (remaining->first() == sprite) {
			return(answer);
		};
		answer++;
		remaining = remaining->rest();
	};
	insert_at_end(sprite,java_sprites);
#if TT_DEBUG_ON
	if (tt_debug_mode == 301100 && -tt_debug_target == answer) {
		log("Generating Java for debug target (sorta)");
	};
#endif
	return(answer);
};

void release_java_sprites() {
	just_destroy_list(java_sprites);
};

java_string java_apply(java_string method_name, java_string arg1) { // could add more args later
	int size = strlen(method_name);
	size += 3; // for parens and the like
	if (arg1 != NULL) {
		size += strlen(arg1);
	};
	string buffer = new char[size];
	output_string_stream stream(buffer,size);
	stream << method_name << "(";
	if (arg1 != NULL) {
		stream << arg1;
	};
	stream << ")";
	stream.put('\0');
	return(buffer);
};

void java_int_args(java_stream &out, long args[], int arg_count) { // new on 130701
	for (int i = 0; i < arg_count; i++) {
      out << args[i];
      if (i < arg_count-1) out << ", ";
   };
};

java_string java_operation(NumberOperation operation) {
	java_string operations[] = 
		{"TT.SUBTRACT_FROM","TT.MULTIPLY_BY","TT.INCREASE_BY","TT.DIVIDE_BY","TT.MODULUS_BY","TT.MAKE_EQUAL","TT.TO_THE_POWER",
		"TT.BITWISE_EXCLUSIVE_OR","TT.BITWISE_AND","TT.BITWISE_OR","TT.LAST_BINARY_NUMBER_OPERATION","TT.INTEGER_PART","TT.FRACTION_PART",
		"TT.NUMERATOR","TT.DENOMINATOR","TT.SINE","TT.COSINE","TT.TANGENT","TT.ARCSINE","TT.ARCCOSINE","TT.ARCTANGENT","TT.NATURAL_LOG",
		"TT.LOG10","TT.COMPLEMENT","TT.NO_NUMBER_OPERATION"};
	return(operations[operation]);
};

