// Copyright (c) 1992-2005.  Ken Kahn, Animated Programs, All rights reserved.
#ifndef __NUMBERVALUE_H

#define __NUMBERVALUE_H

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif

class NumberValue 
{ // new on 300102 so that a numeric value can be a rational, bignum, or int (32-bit or 64-bit?) 
	public:
#if TT_DEBUG_ON
		NumberValue();
		virtual ~NumberValue();
#else
		NumberValue() : exact_flag(TRUE) {};
		virtual ~NumberValue() {}; // new on 080202
#endif
		virtual NumberValue *copy() = 0;
		virtual boolean long_value(long &the_long_value, boolean round_off=TRUE) = 0;  
		// used by sensors and lots of other code that by default wants to round off
		virtual boolean bignum_value(bignum_pointer &bignum) {
			// , boolean round_off=FALSE) { // used typically where rounding off is wrong - hence the different defaults
			return(FALSE);
		};
		virtual boolean rational_value(rational_pointer &rational) {
			return(FALSE);
		};
		virtual void convert_to_rational(rational &rational_value) = 0; // new on 200202
		virtual boolean double_float_value(double &double_value, boolean ok_to_coerce=FALSE) = 0;
		virtual boolean equals(NumberValue *other) = 0;
		virtual Relation comparison(NumberValue *other) = 0;
		virtual void set_value(long new_value) {};
		virtual void set_value(bignum new_value) {};
		virtual void set_value(rational new_value) {};
		virtual int sign() = 0;
		// these are commented out since they are premature optimizations that interfere with remote controls 
		// (since value is altered rather than recomputed)
//		virtual void negate() = 0;
		virtual boolean complement() = 0;
//		virtual boolean rubout(NumberFormat number_format, int base) = 0; // i.e. when held and the rubout key is pressed
//		virtual NumberValue *add(NumberValue *other) = 0;
//		virtual NumberValue *multiply(NumberValue *other) = 0;
//		virtual NumberValue *divide_by(NumberValue *other) = 0;
//		virtual NumberValue *modulus(NumberValue *other) {
//			log("do later");
//			return(NULL);
//		};
		virtual NumberValue *do_operation(NumberOperation operation, NumberValue *other) = 0;
		void set_exact(boolean new_flag) {
			exact_flag = (flag) new_flag;
		};
		boolean and_exact(boolean new_flag) {
			exact_flag &= new_flag;
			return(exact_flag);
		};
		boolean exact() {
			return(exact_flag);
		};
		virtual void dump(output_stream &stream) = 0; // new on 050302
		virtual void print(output_stream &stream, boolean definite=FALSE) = 0; // new on 050302
		virtual NumberFormat default_number_format() = 0; // new on 020702
#if TT_XML
//      virtual xml_element *xml_create_element(xml_document *document) = 0; // new on 140802
		virtual wide_string xml_tag() = 0; // new on 090703
		virtual void xml(xml_element *number_element, xml_document *document) = 0; // new on 120703
#endif
		virtual java_string java_value_string() = 0; // new on 030503
		virtual boolean round_to_nearest_bignum_value(bignum &bignum_value) { // made virtual on 130505
			return(FALSE);
		};
#if TT_DEBUG_ON
		int debug_counter;
#endif
	protected:
		flag exact_flag; // new on 130202 - should propagate so that if either guys are inexact then the result is as well
};

class BigNumberValue; // forward reference
class RationalNumberValue;

class LongNumberValue : public NumberValue {  
	public:
		LongNumberValue(long initial_value) : NumberValue(), my_value(initial_value) {};
		NumberValue *copy();
		boolean long_value(long &the_long_value, boolean round_off=TRUE);
		boolean double_float_value(double &double_value, boolean ok_to_coerce=FALSE);
		void convert_to_rational(rational &rational_value);
		boolean equals(NumberValue *other);
		Relation comparison(NumberValue *other);
		void set_value(long new_value) {
			my_value = new_value;
		};
		int sign() {
			return(my_value); // has the right negative, 0, and positive properties
		};
		NumberValue *do_operation(NumberOperation operation, NumberValue *other);
//		void negate() {
//			my_value = -my_value;
//		};
		boolean complement() {
			my_value = ~my_value;
			return(TRUE);
		};
		void dump(output_stream &stream);
		void print(output_stream &stream, boolean definite=FALSE);
		java_string java_value_string();
		NumberFormat default_number_format() { // new on 020702
			return(TT_LONG);
		};
#if TT_XML
//      xml_element *xml_create_element(xml_document *document); // new on 140802	
		wide_string xml_tag() { // new on 090703
			return(L"Integer");
		};
		void xml(xml_element *number_element, xml_document *document);
#endif
//		boolean rubout(NumberFormat number_format, int base) {
//			my_value /= base;
//			return(TRUE);
//		};
	protected:
		long my_value;
};

class DoubleFloatNumberValue : public NumberValue {  
	public:
		DoubleFloatNumberValue(double initial_value);
		NumberValue *copy();
		boolean long_value(long &the_long_value, boolean round_off=TRUE);
		boolean double_float_value(double &double_value, boolean ok_to_coerce=FALSE);
		void convert_to_rational(rational &rational_value);
		boolean equals(NumberValue *other);
		Relation comparison(NumberValue *other);
		void set_value(long new_value) {
			my_value = new_value;
		};
		int sign();
		NumberValue *do_operation(NumberOperation operation, NumberValue *other);
//		void negate() {
//			my_value = -my_value;
//		};
		boolean complement() { 
			return(FALSE); // doesn't make sense
		};
		void dump(output_stream &stream);
		void print(output_stream &stream, boolean definite=FALSE);
		java_string java_value_string();
		NumberFormat default_number_format() {
			return(TT_DOUBLE_FLOAT);
		};
#if TT_XML
//      xml_element *xml_create_element(xml_document *document); // new on 160802
		wide_string xml_tag() { // new on 090703
			return(L"FloatingPointNumber64Bit");
		};
		void xml(xml_element *number_element, xml_document *document);
#endif
//		boolean rubout(NumberFormat number_format, int base) { // not clear what keyboard should do to this
//			return(FALSE);
//		};
	protected:
		double my_value;
};

class BigNumberValue : public NumberValue  {  
	public:
		BigNumberValue(long initial_value=0);
		BigNumberValue(double initial_value);
		BigNumberValue(string text, int base);
		~BigNumberValue();
		NumberValue *copy();
		boolean long_value(long &the_long_value, boolean round_off=TRUE);
		boolean double_float_value(double &double_value, boolean ok_to_coerce=FALSE);
		boolean bignum_value(bignum_pointer &bignum);
		void convert_to_rational(rational &rational_value);
		boolean equals(NumberValue *other);
		Relation comparison(NumberValue *other);
		int sign();
		NumberValue *do_operation(NumberOperation operation, NumberValue *other);
//		void negate();
		boolean complement();
		void dump(output_stream &stream);
		void print(output_stream &stream, boolean definite=FALSE);
		java_string java_value_string();
		NumberFormat default_number_format() {
			return(TT_BIGNUM);
		};
#if TT_XML
//      xml_element *xml_create_element(xml_document *document); // new on 160802
		wide_string xml_tag() { // new on 090703
			return(L"Integer");
		};
		void xml(xml_element *number_element, xml_document *document);
#endif
//		boolean rubout(NumberFormat number_format, int base);
	protected:
		bignum my_value;
};

class RationalNumberValue : public NumberValue {  
	public:
		RationalNumberValue(long initial_value=0);
		RationalNumberValue(double initial_value);
		RationalNumberValue(string numerator, string denominator, int base, boolean canonicalize=TRUE);
		~RationalNumberValue();
		NumberValue *copy();
		boolean long_value(long &the_long_value, boolean round_off=TRUE);
		boolean double_float_value(double &double_value, boolean ok_to_coerce=FALSE);
		boolean bignum_value(bignum_pointer &bignum);
		boolean round_to_nearest_bignum_value(bignum &bignum_value);
		boolean rational_value(rational_pointer &rational);
		void convert_to_rational(rational &rational_value);
		boolean equals(NumberValue *other);
		Relation comparison(NumberValue *other);
		int sign();
		NumberValue *do_operation(NumberOperation operation, NumberValue *other);
//		void negate();
		boolean complement();
		void dump(output_stream &stream);
		void print(output_stream &stream, boolean definite=FALSE);
		java_string java_value_string();
		NumberFormat default_number_format() {
			return(TT_RATIONAL);
		};
      wide_string value_as_wide_string(boolean &could_be_integer);
#if TT_XML
//      xml_element *xml_create_element(xml_document *document); // new on 160802
		wide_string xml_tag();
		void xml(xml_element *number_element, xml_document *document);
#endif
//		boolean rubout(NumberFormat number_format, int base);
	protected:
		rational my_value;
};

boolean unary_operation(NumberOperation operation);
NumberValue *load_number_value(InputStream *stream, int version);
NumberValue *number_value(NumberValueFormat value_format, long long_value, double double_float_value, string text_value1, string text_value2);
string bignum_as_string(bignum number, long &string_length, long &string_base, int maximum_base_10_number_size=100); // 100 digits in decimal isn't much overhead and is user friendlier
#endif

