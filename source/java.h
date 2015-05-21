// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved
#if !defined(__TT_JAVA_H)

#define __TT_JAVA_H

void java_declare1(java_stream &out, const_java_string type, const_java_string variable);

void java_declare(java_stream &out, const_java_string type, const_java_string variable,
                  Sprite *initial_value=NULL);
void java_declare(java_stream &out, const_java_string type, const_java_string variable,
                  const_java_string initial_value);

void java_declare_new(java_stream &out, const_java_string type, const_java_string variable,
                      const_java_string class_name, const_java_string arg1=NULL, const_java_string arg2=NULL);
void java_declare_new(java_stream &out, const_java_string type, const_java_string variable,
                      const_java_string class_name, int arg1);

void java_declare_new_array(java_stream &out, const_java_string type, const_java_string variable, int size);

void java_new(java_stream &out, const_java_string class_name, int arg1);
void java_new(java_stream &out, const_java_string class_name, character arg1);
void java_new(java_stream &out, const_java_string class_name, int arg1, character arg2);
void java_new(java_stream &out, const_java_string class_name, const_java_string arg1=_T(""));
void java_new(java_stream &out, const_java_string class_name, const_java_string arg1, character arg2);
void java_new(java_stream &out, const_java_string class_name, int arg1, character arg2, int arg3, int arg4, int arg5);
void java_new(java_stream &out, const_java_string class_name, const_java_string arg1, character arg2, int arg3);
void java_new(java_stream &out, const_java_string class_name, const_java_string arg1, character arg2, const_java_string arg3);
void java_new(java_stream &out, const_java_string class_name, const_java_string arg1, character arg2, int arg3, const_java_string arg4);
void java_new(java_stream &out, const_java_string class_name, long arg1, const_java_string arg2, character arg3, int arg4, const_java_string arg5=NULL);
void java_new(java_stream &out, const_java_string class_name, java_string arg1, int arg2, int arg3, int arg4, int arg5);
void java_new(java_stream &out, const_java_string class_name, int arg1, java_string arg2, int arg3, int arg4, int arg5); // new on 030503
void java_new(java_stream &out, const_java_string class_name, java_string arg1, java_string arg2, int arg3, int arg4, int arg5); // new on 030503
void java_new(java_stream &out, const_java_string class_name, long arg1, const_java_string arg2, character arg3, const_java_string arg4, const_java_string arg5=NULL);
void java_new(java_stream &out, const_java_string class_name, const_java_string arg1, const_java_string arg2, character arg3, int arg4, const_java_string arg5=NULL);
void java_new(java_stream &out, const_java_string class_name, long args[], int arg_count, double arg4_double, double arg5_double, const_java_string arg2=NULL, const_java_string arg3=NULL, const_java_string arg4=NULL);
void java_new(java_stream &out, const_java_string class_name, Sprite *arg1, long args[], int arg_count, double arg4_double, double arg5_double, const_java_string arg2=NULL, const_java_string arg3=NULL, const_java_string arg4=NULL);
void java_new(java_stream &out, const_java_string class_name, const_java_string arg1, long args[], int arg_count, double arg4_double, double arg5_double, const_java_string arg2=NULL, const_java_string arg3=NULL, const_java_string arg4=NULL);
void java_new(java_stream &out, const_java_string class_name, int arg1, const_java_string arg2, long args[], int arg_count, double arg4_double, double arg5_double, const_java_string arg3=NULL, const_java_string arg4=NULL, const_java_string arg5=NULL);
void java_new(java_stream &out, const_java_string class_name, Sprite *arg1, Sprite *arg2, Sprite *arg3, Sprite *arg4, Sprite *arg5, Sprite *arg6, Sprite *arg7, string arg8=NULL);
void java_new_quote_arg1(java_stream &out, const_java_string class_name, const_java_string arg1, const_java_string arg2=NULL);
void java_new_quote_arg1(java_stream &out, const_java_string class_name, const_java_string arg1, long args[], int arg_count, double arg4_double, double arg5_double, const_java_string arg2=NULL, const_java_string arg3=NULL, const_java_string arg4=NULL);
void java_new_quote_arg1(java_stream &out, const_java_string class_name, const_java_string arg1, const_java_string arg2, long args[], int arg_count, double arg4_double, double arg5_double, const_java_string arg3=NULL, const_java_string arg4=NULL, const_java_string arg5=NULL);
void java_send_quote_arg6(java_stream &out, const_java_string variable, const_java_string method_name, int arg1, int arg2, int arg3, int arg4, int arg5, const_java_string arg6, int arg7);
void java_new(java_stream &out, const_java_string class_name, long arg1, const_java_string arg2, character arg3, const_java_string arg4, const_java_string arg5, const_java_string arg6);
void java_new(java_stream &out, const_java_string class_name, long arg1, const_java_string arg2, string arg3, const_java_string arg4, const_java_string arg5, const_java_string arg6);

const_java_string java_boolean(boolean flag);
java_string java_array(const_java_string array_name, int index);

void java_assign(java_stream &out, const_java_string variable,
                 Path *path,
                 boolean first=TRUE, const_java_string set_new_value=NULL, const_java_string set_mode=NULL);

void java_assign(java_stream &out, const_java_string variable, const_java_string value);

void java_assign(java_stream &out, Path *variable, const_java_string value);

void java_assign(java_stream &out, const_java_string variable, Sprite *value);

void java_assign_array(java_stream &out, const_java_string variable, int index, Sprite *value);
void java_assign_array(java_stream &out, const_java_string variable, int index, int value);
void java_assign_array_new(java_stream &out, const_java_string variable, const_java_string index, const_java_string value);

void java_assign_send(java_stream &out, Path *variable, Path *value,
                      const_java_string method_name=NULL,
                      const_java_string first_arg=NULL, const_java_string second_arg=NULL);
void java_assign_send(java_stream &out, Path *variable, const_java_string value,
                      const_java_string method_name,
                      const_java_string first_arg=NULL, const_java_string second_arg=NULL);
void java_assign_send(java_stream &out, const_java_string variable,
                      Path *path, const_java_string method_name,
                      const_java_string first_arg=NULL, const_java_string second_arg=NULL, const_java_string third_arg=NULL);
void java_assign_send(java_stream &out, const_java_string variable,
                      const_java_string subject, const_java_string method_name,
                      Path *first_arg);
void java_assign_send(java_stream &out, const_java_string variable, const_java_string value, 
							 const_java_string method_name, Path *first_arg, const_java_string second_arg);
void java_assign_send(java_stream &out, Path *variable, const_java_string value, 
							 const_java_string method_name, Path *first_arg, const_java_string second_arg);

void java_assign_new(java_stream &out, const_java_string variable,
                     const_java_string class_name,
                     const_java_string first_arg=NULL, const_java_string second_arg=NULL);
void java_assign_new(java_stream &out, const_java_string variable,
                     const_java_string class_name, int arg1, int arg2, int arg3);
void java_declare_new(java_stream &out, const_java_string variable, const_java_string class_name, Sprite *arg1,
                      long args[], int arg_count, const_java_string arg2, const_java_string arg3);

void java_send(java_stream &out, const_java_string variable, 
               const_java_string method_name, const_java_string first_arg=NULL, const_java_string second_arg=NULL);
               
void java_send(java_stream &out, const_java_string variable, const_java_string method_name, int arg1 ,int arg2, int arg3,
               const_java_string arg4=NULL, const_java_string arg5=NULL);

void java_send(java_stream &out, const_java_string variable,
               const_java_string method_name, Path *first_arg, const_java_string second_arg=NULL);

void java_send(java_stream &out, Path *path,
               boolean first=TRUE, const_java_string set_new_value=NULL, const_java_string set_mode=NULL);

void java_send(java_stream &out, Path *path,
               const_java_string method_name, const_java_string first_arg=NULL, const_java_string second_arg=NULL);

void java_send(java_stream &out, const_java_string variable,
               const_java_string method_name, int first_arg, const_java_string second_arg=NULL, const_java_string third_arg=NULL);

void java_send(java_stream &out, const_java_string variable,
               const_java_string method_name, int first_arg, Sprite *second_arg);

void java_send(java_stream &out, const_java_string variable, const_java_string method_name, Sprite *arg);

void java_send_this(java_stream &out,
                    const_java_string method_name,
                    const_java_string first_arg=NULL, const_java_string second_arg=NULL);
void java_send_this(java_stream &out, const_java_string method_name,
                    Sprite *first_arg, const_java_string second_arg=NULL);

void java_assign_send(java_stream &out, const_java_string variable,
                      const_java_string subject, const_java_string method_name,
                      const_java_string first_arg=NULL, const_java_string second_arg=NULL, const_java_string third_arg=NULL);

void java_send_array_arg(java_stream &out, const_java_string path,
                         const_java_string method_name, const_java_string array, const_java_string index);

void java_just_send(java_stream &out, const_java_string variable, const_java_string method_name, int arg1);
void java_send_this(java_stream &out, const_java_string method_name, int first_arg, Sprite *second_arg=NULL);
void java_just_send(java_stream &out, const_java_string variable, const_java_string method_name, const_java_string arg1=NULL);
void java_just_send(java_stream &out, Path *path, const_java_string method_name, int arg1);

void java_begin_block(java_stream &out);

void java_end_block(java_stream &out);

void java_comment(java_stream &out, const_java_string comment);

java_string java_quote(const_java_string arg);

java_string java_new(const_java_string class_name, const_java_string arg1);

java_string java_array(const_java_string array_name, const_java_string index);

//java_string java_name_from_text(const_java_string text);
java_string java_type_from_sprite_type(SpriteType type);

void java_if(java_stream &out, const_java_string part1, const_java_string part2, const_java_string part3, const_java_string part4);

java_string java_language(NaturalLanguage language);

int java_index(Sprite *sprite);
boolean has_java_index(Sprite *sprite);
void release_java_sprites();

java_string java_apply(java_string method_name, java_string arg1=NULL);

void java_int_args(java_stream &out, long args[], int arg_count);

java_string java_operation(NumberOperation operation);

#endif
