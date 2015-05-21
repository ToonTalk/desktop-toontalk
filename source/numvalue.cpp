// Copyright (c) 1992-2005. Ken Kahn, Animated Programs, All rights reserved.

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif
#if !defined(__TT_STRINGS_H)
#include "strings.h"
#endif
#if !defined(__TT_UTILS_H)   
#include "utils.h"
#endif
#if !defined(__TT_HELP_H)   
#include "help.h"
#endif
#if !defined(__TT_NUMBER_H)   
#include "number.h"
#endif
#if TT_XML
#if !defined(__TT_XML_H)
#include "xml.h"
#endif
#endif

#include <math.h>
//#include <iomanip.h>

#ifdef _DEBUG // new on 160103 to debug leaks
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if TT_DEBUG_ON

NumberValue::NumberValue() : exact_flag(TRUE) {
	tt_number_value_count++;
	debug_counter = tt_number_value_count;
	//if (debug_counter == tt_debug_value_target) {
	//	log("Creating debug number value target.");
	//};
	if (tt_debug_mode == 160103) {
		tt_error_file() << "Number value #" << debug_counter << " created." << endl;
	};
};

NumberValue::~NumberValue() {
	tt_number_value_destroyed_count++;
	if (tt_debug_mode == 160103) {
		tt_error_file() << "Number value #" << debug_counter << " deleted." << endl;
	};
};

#endif	

#if !TT_GMP_VERSION_4_1
// moved here from gmp.h since this doesn't open code well (Release version buggy)
#pragma warning(disable : 4146) // added by Ken Kahn on 181202 since otherwise the following generates a spurious warning
unsigned long
mpz_get_ui (mpz_srcptr __gmp_z) __GMP_NOTHROW
{
  mp_ptr __gmp_p = __gmp_z->_mp_d;
  mp_size_t __gmp_n = __gmp_z->_mp_size;
  mp_limb_t __gmp_l = __gmp_p[0];
  if (__GMP_ULONG_MAX <= GMP_NUMB_MASK)
    return __gmp_l & (-(mp_limb_t) (__gmp_n != 0));
#pragma warning(default : 4146)  // Ken Kahn added this
#if GMP_NAIL_BITS != 0	/* redundant #if, shuts up compiler warnings */
  else			/* happens for nails, but not if LONG_LONG_LIMB */
    {			/* assume two limbs are enough to fill an ulong */
      __gmp_n = __GMP_ABS (__gmp_n);
      if (__gmp_n <= 1)
	return __gmp_l & (-(mp_limb_t) (__gmp_n != 0));
      else
	return __gmp_l + (__gmp_p[1] << GMP_NUMB_BITS);
    }
#endif
}
#endif

Relation comparison_to_relation(int comparison) {
	if (comparison == 0) {
		return(EQUAL_RELATION);
	} else if (comparison < 0) {
		return(LESS_THAN_RELATION);
	} else {
		return(GREATER_THAN_RELATION);
	}; 
};

Relation negate_relation(Relation positive) {
	switch (positive) {
		case EQUAL_RELATION:
			return(EQUAL_RELATION);
		case LESS_THAN_RELATION:
			return(GREATER_THAN_RELATION);
		case GREATER_THAN_RELATION:
			return(LESS_THAN_RELATION);
	};
	return(INCOMPARABLE_RELATION); // shouldn't happen
};

int compare_bignum_with_long(bignum_pointer bignum_value, long long_value) {
	// this is a workaround to the fact that gmp31.dll seems to be missing support for mpz_cmp_si and mpz_cmp_ui
	// returns -1, 0, or 1 depending upon whether bignum_value is less than, equal, or greater than long_value
	if (long_value == 0) {
		return(mpz_sgn(bignum_value));
	}; 
	if (long_value > 0) {
		if (mpz_sgn(bignum_value) == 1) { // both positive
			return(mpz_cmpabs_ui(bignum_value,long_value));
		} else {
			return(-1);
		};
	};
	// long_value is negative
	if (mpz_sgn(bignum_value) == -1) { // both negative
#if TT_ALPHA
		return(mpz_cmpabs_ui(bignum_value,-long_value)*-1); // the problem wasn't that this coerced long_value to unsigned - updated 021003
#else
		return(mpz_cmpabs_ui(bignum_value,long_value)*-1); // added *-1 on 200903 since otherwise -1.5 < -10
#endif
	} else {
		return(1);
	};
};

int compare_quotient_with_long(rational_pointer rational_value, long long_value) {
	bignum quotient, ignore_remainder;
	mpz_init(quotient);
	mpz_init(ignore_remainder);
	mpz_tdiv_qr(quotient,ignore_remainder,mpq_numref(rational_value),mpq_denref(rational_value)); 
	int result = compare_bignum_with_long(quotient,long_value);
	mpz_clear(quotient);
	mpz_clear(ignore_remainder);
	return(result);
};

int compare_rational_with_long(rational_pointer rational_value, long long_value) {
	// this is a workaround to the fact that gmp31.dll seems to be missing support for mpq_cmp_si and mpq_cmp_ui
	// returns -1, 0, or 1 depending upon whether rational_value is less than, equal, or greater than long_value
	if (compare_bignum_with_long(mpq_denref(rational_value),1) == 0) { // denominator is 1
		return(compare_bignum_with_long(mpq_numref(rational_value),long_value));
	};
	if (long_value == 0) {
		return(mpz_sgn(mpq_numref(rational_value)));
	};
	if (long_value > 0) {
		if (mpz_sgn(mpq_numref(rational_value)) == 1) { // both positive
			int result = compare_quotient_with_long(rational_value,long_value);
			if (result == 0) {
				return(1); // since there must be a remainder since already did a check for a denominator of 1
			} else {
				return(result);
			};
//			return(mpq_cmp_si(rational_value,long_value,1)); -- should be how to say it
		} else {
			return(-1);
		};
	};
	// long_value is negative
	if (mpz_sgn(mpq_numref(rational_value)) == -1) { // both negative
		int result = compare_quotient_with_long(rational_value,long_value);
		if (result == 0) {
			return(-1); // since there must be a remainder since already did a check for a denominator of 1
		} else {
			return(result);
		};
//		return(mpq_cmp_si(rational_value,long_value,1)); 
	} else {
		return(1);
	};
};

NumberValue *NumberValue::do_operation(NumberOperation operation, NumberValue *other) {
	log("Shouldn't happen. Arithmetic operation not handled right.");
#if TT_DEBUG_ON
	debug_this();
#endif
	return(new LongNumberValue(0L));
};

NumberValue *LongNumberValue::copy() {
	LongNumberValue *result = new LongNumberValue(my_value);
	result->set_exact(exact());
	return(result);
};

boolean LongNumberValue::long_value(long &the_long_value, boolean round_off) {
	the_long_value = my_value;
	return(TRUE);
};

boolean LongNumberValue::double_float_value(double &double_value, boolean ok_to_coerce) {
	if (!ok_to_coerce) return(FALSE);
	double_value = (double) my_value;
	return(TRUE);
};

void LongNumberValue::convert_to_rational(rational &rational_value) {
	mpq_set_si(rational_value,my_value,1);
};

boolean LongNumberValue::equals(NumberValue *other) {
	long others_value;
	if (other->long_value(others_value,FALSE)) {
		return(my_value == others_value);
	} else {
		return(other->equals(this)); // others need to deal with longs but longs don't need to deal with them
	};
};

Relation LongNumberValue::comparison(NumberValue *other) {
	long others_value;
	if (!other->long_value(others_value,FALSE)) {
		return(negate_relation(other->comparison(this)));	
	} else if (my_value == others_value) {
		return(EQUAL_RELATION);
	} else if (my_value < others_value) {
		return(LESS_THAN_RELATION);
	} else {
		return(GREATER_THAN_RELATION);
	};
};

NumberValue *LongNumberValue::do_operation(NumberOperation operation, NumberValue *other) {
	if (unary_operation(operation)) {// new on 120202
		switch (operation) {
			// prior to 020503 these were BigNumberValue which is kind of wasteful
			case INTEGER_PART:  
				return(new LongNumberValue(my_value));
			case FRACTION_PART: 
				return(new LongNumberValue(0L));
			case NUMERATOR: 
				return(new LongNumberValue(my_value));
			case DENOMINATOR: 
				return(new LongNumberValue(1L));
			case SINE: 
				return(new DoubleFloatNumberValue(sin((my_value%360)*radians_per_degree)));
			case COSINE: 
				return(new DoubleFloatNumberValue(sin((my_value%360)*radians_per_degree+half_pi)));
			case TANGENT: 
				return(new DoubleFloatNumberValue(tan((my_value%360)*radians_per_degree)));
			case ARCSINE: 
				return(new DoubleFloatNumberValue(asin((double) my_value)/radians_per_degree));
			case ARCCOSINE: 
				return(new DoubleFloatNumberValue(asin((double) my_value)/radians_per_degree-90.0));		
			case ARCTANGENT: 
				return(new DoubleFloatNumberValue(atan((double) my_value)/radians_per_degree));
			case NATURAL_LOG:
				return(new DoubleFloatNumberValue(log((double) my_value)));
			case LOG10:
				return(new DoubleFloatNumberValue(log10((double) my_value)));
		};
	};
	long result = 0;
	boolean need_bignums = FALSE; // until proven otherwise
	boolean need_rationals = (operation == SINE || operation == COSINE || operation == TANGENT || operation == ARCSINE || operation == ARCCOSINE ||
									  operation == ARCTANGENT || operation == NATURAL_LOG || operation == LOG10);
	long other_value;
	double other_double_float_value;
	if (!need_rationals && other->long_value(other_value,FALSE)) {
		switch (operation) {
			case INCREASE_BY: 
				result = my_value+other_value;
//				tt_error_file() << "Added " << my_value << "+" << other_value << "=" << result << " on frame " << tt_frame_number << endl;
				need_bignums = ((result > 0 && my_value < 0 && other_value < 0) || // underflow
									 (result < 0 && my_value > 0 && other_value > 0)); // overflow
				break;
			case SUBTRACT_FROM: 
				result = my_value-other_value;
				need_bignums = ((result > 0 && my_value < 0 && other_value < 0) || // underflow
									 (result < 0 && my_value > 0 && other_value > 0)); // overflow
				break;
			case MULTIPLY_BY: // 'x' and 'X' take care of elsewhere
				result = my_value*other_value;
				need_bignums = (other_value != 0 && result/other_value != my_value);
				break;
			case DIVIDE_BY: // caller should have checked for zero value
				result = my_value/other_value;
				need_rationals = (result*other_value != my_value);
				break;
			case MODULUS_BY: // caller should have checked for zero value
				result = my_value%other_value; // can't overflow
				break;
			case TO_THE_POWER:
				need_bignums = TRUE; // could handle a few special cases here ...
				break;
			case BITWISE_EXCLUSIVE_OR:
				result = my_value^other_value; 
				break;
			case BITWISE_AND:
				result = my_value&other_value; 
				break;
			case BITWISE_OR:
				result = my_value|other_value; 
				break;
		};
	} else if (other->double_float_value(other_double_float_value,FALSE)) { // new on 130202
		DoubleFloatNumberValue double_float_number_value((double) my_value);
		return(double_float_number_value.do_operation(operation,other));
	} else {
		need_bignums = TRUE; // might need rationals - will find out soon if so
	};
	if (need_rationals) {
		RationalNumberValue rational_value(my_value);
		return(rational_value.do_operation(operation,other));
	} else if (need_bignums) {
		BigNumberValue bignum_value(my_value);
		return(bignum_value.do_operation(operation,other));
	} else {
		return(new LongNumberValue(result));
	};
};

void LongNumberValue::dump(output_stream &stream) {
	stream.put((char) TT_LONG_VALUE); // flag to indicate an integer
	stream.write((character *) &my_value, sizeof(my_value));
};

#if TT_XML
//xml_element *LongNumberValue::xml_create_element(xml_document *document) {
//   return(create_xml_element(L"Integer",document,my_value)); // was L"Integer32Bit"
//};

void LongNumberValue::xml(xml_element *number_element, xml_document *document) { // rewritten 120703
	add_xml_value(number_element,document,my_value);
};

#endif

void LongNumberValue::print(output_stream &stream, boolean definite) {
	if (definite) {
		stream << S(IDS_INTEGER_DEFINITE,"the integer") << space();
	};
	print_integer_with_commas(my_value,stream);
};

java_string LongNumberValue::java_value_string() {
	string result = new char[12]; // 32-bit integer can't be longer than this
	itoa(my_value,result,10);
	return(result);
};

DoubleFloatNumberValue::DoubleFloatNumberValue(double initial_value) : 
	NumberValue(), 
	my_value(initial_value) {
	if (_finite(initial_value) == 0) { // new on 050803
		throw(FLOATING_POINT_NOT_A_NUMBER);
	};
	exact_flag = FALSE; // by its nature
};

NumberValue *DoubleFloatNumberValue::copy() {
	return(new DoubleFloatNumberValue(my_value));
};

boolean DoubleFloatNumberValue::long_value(long &the_long_value, boolean round_off) {
	if (!round_off) return(FALSE);
	the_long_value = (long) my_value;
	return(TRUE);
};

boolean DoubleFloatNumberValue::double_float_value(double &double_value, boolean ok_to_coerce) {
	double_value = my_value;
	return(TRUE);
};

void DoubleFloatNumberValue::convert_to_rational(rational &rational_value) {
	mpq_set_d(rational_value,my_value);
};

boolean DoubleFloatNumberValue::equals(NumberValue *other) {
	double others_value;
	other->double_float_value(others_value,TRUE);
	return(my_value == others_value);
};

Relation DoubleFloatNumberValue::comparison(NumberValue *other) {
	double others_value;
	other->double_float_value(others_value,TRUE);
	if (my_value == others_value) {
		return(EQUAL_RELATION);
	} else if (my_value < others_value) {
		return(LESS_THAN_RELATION);
	} else {
		return(GREATER_THAN_RELATION);
	};
};

int DoubleFloatNumberValue::sign() {
//	return((int) my_value); -- doesn't work since near zero becomes zero -- re-written on 170202
	if (my_value > 0) return(1);
	if (my_value < 0) return(-1);
	return(0);
};

NumberValue *DoubleFloatNumberValue::do_operation(NumberOperation operation, NumberValue *other) {
	if (unary_operation(operation)) {// new on 120202
		switch (operation) {
			case INTEGER_PART: 
				// coerce to (long) since this is the exact result
				if (my_value >= 0) {
					return(new BigNumberValue((long) floor(my_value))); // was ceil prior to 040803
				} else { // new on 040803
					return(new BigNumberValue((long) ceil(my_value))); 
				};
			case FRACTION_PART:
				if (my_value >= 0) {
					return(new DoubleFloatNumberValue(my_value-floor(my_value))); // was ceil prior to 040803
				} else { // new on 040803
					return(new DoubleFloatNumberValue(my_value-ceil(my_value))); 
				};
			case NUMERATOR:
			case DENOMINATOR: {
//				throw; // doesn't really make sense
				// rewritten on 040803 to convert to rational and then try again
				rational my_value_as_rational; // allocate it just in this branch
				mpq_init(my_value_as_rational);
				mpq_set_d(my_value_as_rational,my_value);
				BigNumberValue *result = new BigNumberValue();
				bignum_pointer result_bignum_value;
				result->bignum_value(result_bignum_value);			
				if (operation == NUMERATOR) {
					mpz_set(result_bignum_value,mpq_numref(my_value_as_rational));
				} else {
					mpz_set(result_bignum_value,mpq_denref(my_value_as_rational));
				};
				mpq_clear(my_value_as_rational);
				return(result);
								 };
			case SINE: 
				return(new DoubleFloatNumberValue(sin(my_value*radians_per_degree)));
			case COSINE: 
				return(new DoubleFloatNumberValue(sin(my_value*radians_per_degree+half_pi)));
			case TANGENT: 
				return(new DoubleFloatNumberValue(tan(my_value*radians_per_degree)));
			case ARCSINE: 
				return(new DoubleFloatNumberValue(asin(my_value)/radians_per_degree));
			case ARCCOSINE: 
				return(new DoubleFloatNumberValue(asin(my_value)/radians_per_degree-90.0));
			case ARCTANGENT: 
				return(new DoubleFloatNumberValue(atan(my_value)/radians_per_degree));
			case NATURAL_LOG:
				return(new DoubleFloatNumberValue(log(my_value)));
			case LOG10:
				return(new DoubleFloatNumberValue(log10(my_value)));
		};
	};
	double result = 0.0;
	double other_value;
	other->double_float_value(other_value,TRUE);
	switch (operation) {
		case INCREASE_BY: 
			result = my_value+other_value;
			break;
		case SUBTRACT_FROM: 
			result = my_value-other_value;
			break;
		case MULTIPLY_BY: // 'x' and 'X' take care of elsewhere
			result = my_value*other_value;
			break;
		case DIVIDE_BY: // caller should have checked for zero value
			result = my_value/other_value;
			break;
		case MODULUS_BY: // caller should have checked for zero value
			result = my_value-(ceil(my_value/other_value)*other_value);
			break;
		case TO_THE_POWER:
			result = pow(my_value,other_value);
			break;
		case BITWISE_EXCLUSIVE_OR:
			throw; // doesn't make sense
		case BITWISE_AND:
			throw; // doesn't make sense
		case BITWISE_OR:
			throw; // doesn't make sense
	};
	if (!_finite(result)) {
		throw "Floating point arithmetic returned non-finite result."; // new on 140202 - could throw more useful information
	};
	return(new DoubleFloatNumberValue(result));
};

void DoubleFloatNumberValue::dump(output_stream &stream) {
	stream.put((char) TT_DOUBLE_FLOAT_VALUE); // flag to indicate an integer
	stream.write((character *) &my_value, sizeof(my_value));
};

void DoubleFloatNumberValue::print(output_stream &stream, boolean definite) {
	if (definite) {
		stream << S(IDS_FLOATING_POINT_DEFINITE,"the approximate number") << space();
	};
	stream << std::setprecision(DBL_DIG) << std::setiosflags(std::ios_base::fixed) << my_value;
};

java_string DoubleFloatNumberValue::java_value_string() {
	char buffer[maximum_double_flat_text_size]; // updated 050305 100]; // more than enough
	output_string_stream out(buffer,maximum_double_flat_text_size);
	print(out);
	out.put('\0');
	return(copy_string(buffer));
};

#if TT_XML

//void DoubleFloatNumberValue::xml(xml_element *number_element, xml_document *document) {
//   char value_as_string[128]; // should be long enough - right??
//   output_string_stream stream(value_as_string,sizeof(value_as_string));
//   print(stream);
//   stream.put('\0'); // terminate
//   xml_append_child(create_xml_element(L"FloatingPointNumber64Bit",document,value_as_string),number_element);
//};

// restored the above for all but integers except changed the interface - 050803 - a mistake fixed on 050803

void DoubleFloatNumberValue::xml(xml_element *number_element, xml_document *document) { // rewritten 120703
//	char value_as_string[128]; // should be long enough - right?? -- NO! - updated 050305
	char value_as_string[maximum_double_flat_text_size];
   output_string_stream stream(value_as_string,sizeof(value_as_string));
   print(stream);
   stream.put('\0'); // terminate
	add_xml_value(number_element,document,value_as_string);
};

#endif


BigNumberValue::BigNumberValue(long initial_value) : NumberValue() {
	mpz_init(my_value);
	mpz_set_si(my_value,initial_value);
};

BigNumberValue::BigNumberValue(double initial_value) : NumberValue() {
	mpz_init(my_value);
	mpz_set_d(my_value,initial_value);
	set_exact(FALSE); // new on 220202
};

BigNumberValue::BigNumberValue(string text, int base) { // new on 050302
	mpz_init(my_value);
	set_bignum_value_to_string(my_value,text,base);
};

BigNumberValue::~BigNumberValue() {
	mpz_clear(my_value);
};

NumberValue *BigNumberValue::copy() {
	BigNumberValue *result = new BigNumberValue();
	result->set_exact(exact());
	bignum_pointer bignum_value;
	result->bignum_value(bignum_value);
	mpz_set(bignum_value,my_value);
	return(result);
};

boolean BigNumberValue::long_value(long &the_long_value, boolean round_off) {
//	if (mpz_cmpabs_ui(my_value,max_long) > 0) return(FALSE);
	if (!mpz_fits_slong_p(my_value)) return(FALSE);
	the_long_value = mpz_get_si(my_value);
	return(TRUE);
};

boolean BigNumberValue::double_float_value(double &double_value, boolean ok_to_coerce) {
	if (!ok_to_coerce) return(FALSE);
	double_value = mpz_get_d(my_value);
	return(_finite(double_value)); // was just TRUE prior to 200202
};

void BigNumberValue::convert_to_rational(rational &rational_value) {
	mpz_set(mpq_numref(rational_value),my_value);
	mpz_set_si(mpq_denref(rational_value),1);
};

boolean BigNumberValue::bignum_value(bignum_pointer &bignum_value) {
	bignum_value = &my_value[0];
	return(TRUE);
};

boolean BigNumberValue::equals(NumberValue *other) {
	bignum_pointer others_bignum_value;
	if (other->bignum_value(others_bignum_value)) {
		return(mpz_cmp(my_value,others_bignum_value) == 0);
	};
	long others_long_value;
	if (other->long_value(others_long_value,FALSE)) {
		int my_sign = mpz_sgn(my_value);
		if ((others_long_value > 0 && my_sign < 0) || (others_long_value < 0 && my_sign > 0)) {
			return(FALSE); // new on 181203
		};
		return(mpz_cmpabs_ui(my_value,others_long_value) == 0);
	};
	return(other->equals(this)); // other should be a rational and will deal with this
};

Relation BigNumberValue::comparison(NumberValue *other) {
	bignum_pointer others_bignum_value;
	if (other->bignum_value(others_bignum_value)) {
		return(comparison_to_relation(mpz_cmp(my_value,others_bignum_value)));
	};
	long others_long_value;
	if (other->long_value(others_long_value)) {
		return(comparison_to_relation(compare_bignum_with_long(my_value,others_long_value)));
	};
	return(negate_relation(other->comparison(this)));	
};

int BigNumberValue::sign() {
	return(mpz_sgn(my_value)); 
};

//void BigNumberValue::negate() {
//	mpz_neg(my_value,my_value);
//};

boolean BigNumberValue::complement() {
	mpz_com(my_value,my_value);
	return(TRUE);
};

boolean divisible(bignum x, bignum y) { // abstracted on 160503
	// following is better way to do this but not availalbe in 3.1
#if TT_GMP_VERSION_4_1
	return(mpz_divisible_p(x,y)); // new on 170103 - replaces the following
#else
	bignum ignore_remainder; // unclear why this is needed
	mpz_init(ignore_remainder);
	mpz_mod(ignore_remainder,x,y);
	boolean result = (mpz_sgn(ignore_remainder) == 0);
	mpz_clear(ignore_remainder);
	return(result);
#endif
};

boolean divisible(bignum x, unsigned long y) { // abstracted on 160503
	// following is better way to do this but not availalbe in 3.1
#if TT_GMP_VERSION_4_1 // was mistyped prior to 020304 so always used the other code
	return(mpz_divisible_ui_p(x,y)); // new on 170103 - replaces the following
#else
	bignum ignore_remainder; // unclear why this is needed
	mpz_init(ignore_remainder);
	boolean result = (mpz_mod_ui(ignore_remainder,x,y) == 0); // since mpz_mod_ui returns the remainder - updated 211003
//	boolean result = (mpz_mod_ui(ignore_remainder,x,y) != 0); 
	mpz_clear(ignore_remainder);
	return(result);
#endif
};

void signal_error_if_too_many_bits(unsigned _int64 total_bits_needed) {
	// assume there is always a megabyte left or things are really badly off -- 
	// and don't want to call compute_memory_available all the time
	const int one_mega_byte = 8388608; // number of bits in a megabyte 2^23
	if (total_bits_needed > one_mega_byte && (10*total_bits_needed)/8 > compute_memory_available()/sizeof(int)) {
		// converted to bytes and caculated 10%
		string message 
			= S(IDS_OPERATION_NEEDS_MORE_MEMORY_THAN_AVAIALBLE,
				 "computing a number requires more than 10% of the computer's free memory.                             ");
		string message_end = strchr(message,'.');
		if (message_end != NULL) {
         _ui64toa(total_bits_needed/one_mega_byte,message_end+2,10); 
			strcat(message_end," MB"); 
		};
		throw message;
	};
};

void safe_mpz_pow_ui(bignum_pointer result, bignum_pointer base, unsigned long int exp) {
	// new on 050305 to throw an exception if this needs too much memory
	unsigned long int base_bit_count = mpz_sizeinbase(base,2);
	signal_error_if_too_many_bits(base_bit_count*exp);	
	mpz_pow_ui(result,base,exp);
};

void safe_mpz_mul(bignum_pointer result, bignum_pointer b1, bignum_pointer b2) {
	// new on 070305 to throw an exception if this needs too much memory
	unsigned long int bit_count1 = mpz_sizeinbase(b1,2);
	unsigned long int bit_count2 = mpz_sizeinbase(b2,2);
	signal_error_if_too_many_bits(bit_count1+bit_count2);
	mpz_mul(result,b1,b2);
};

NumberValue *do_unary_operation_to_big_number_value(NumberOperation operation, bignum value) {
	double my_double_float_value = mpz_get_d(value);
	if (_finite(my_double_float_value)) {
		switch (operation) {
			case LOG10:
				return(new DoubleFloatNumberValue(log10(my_double_float_value)));
			case NATURAL_LOG:
				return(new DoubleFloatNumberValue(log(my_double_float_value)));
			// more to come
		};
	};
	// new on 050305 to work regardless of size (too big to convert to a double)
	if (operation == LOG10 || operation == NATURAL_LOG) {
		signed long int exp;
		double d = mpz_get_d_2exp(&exp,value);
		double log_d, log_2;
		if (operation == LOG10) {
			log_d = log10(d);
			log_2 = log10(2.0);
		} else {
			log_d = log(d);
			log_2 = log(2.0);
		};
		return(new DoubleFloatNumberValue(log_d+log_2*exp));
	};
	return(NULL);
};

NumberValue *BigNumberValue::do_operation(NumberOperation operation, NumberValue *other) {
	boolean need_rationals = FALSE;
	boolean need_bignums = FALSE;
	long others_long_value;
	double other_double_float_value;
	BigNumberValue *result = NULL;
	if (operation == LOG10 || operation == NATURAL_LOG) { // new on 050305 to do this here rather than in RationalNumberValue
		NumberValue *result = do_unary_operation_to_big_number_value(operation,my_value);
		if (result != NULL) {
			return(result);
		};
	};
	if (other->long_value(others_long_value,FALSE)) { // if other is a long
		result = new BigNumberValue();
		bignum_pointer results_bignum_value;
		result->bignum_value(results_bignum_value);
		switch (operation) {
			case INCREASE_BY:
				if (others_long_value > 0) {
					mpz_add_ui(results_bignum_value,my_value,others_long_value);
				} else {
					mpz_sub_ui(results_bignum_value,my_value,-others_long_value);
				};
				break;
			case SUBTRACT_FROM:
				if (others_long_value > 0) {
					mpz_sub_ui(results_bignum_value,my_value,others_long_value);
				} else {
					mpz_add_ui(results_bignum_value,my_value,-others_long_value);
				};
				break;
			case MULTIPLY_BY:
				mpz_mul_si(results_bignum_value,my_value,others_long_value);
				break;
			case DIVIDE_BY: {
				boolean other_is_positive = (others_long_value > 0); 
				unsigned int abs_others_long_value;
				if (other_is_positive) {
					abs_others_long_value = others_long_value;
				} else {
					abs_others_long_value = -others_long_value;
				};
				need_rationals = !divisible(my_value,abs_others_long_value); 
				if (!need_rationals) {
					// following is better but not supported in 3.1
#if TT_GMP_VERSION_4_1
					bignum quotient;
					mpz_init(quotient);
					mpz_divexact_ui(results_bignum_value,my_value,abs_others_long_value);
					if (!other_is_positive) {
						mpz_neg(results_bignum_value,results_bignum_value);
					};
#else
					bignum divisor;
					mpz_init(divisor);
					mpz_set_si(divisor,others_long_value);
					mpz_cdiv_q(results_bignum_value,my_value,divisor); 
					// following rewritten on 190204 to be the above
					//long quotient;
					//if (others_long_value > 0) {
					//	quotient = mpz_cdiv_ui(my_value,others_long_value);
					//} else {
					//	quotient = mpz_cdiv_ui(my_value,-others_long_value);
					//	quotient = -quotient;
					//};
					//mpz_set_si(results_bignum_value,quotient); // could optimize this better
#endif
				};
				break;
								};
			case MODULUS_BY:
				mpz_mod_ui(results_bignum_value,my_value,abs(others_long_value)); // result can be a long - optimize this... -- is abs the right thing here??
				break;
			case TO_THE_POWER: // new on 130202
				if (others_long_value > 0) {
					safe_mpz_pow_ui(results_bignum_value,my_value,others_long_value);
				} else if (others_long_value < 0) {
					delete result; // bignum won't cut it
					RationalNumberValue *result = new RationalNumberValue();
					rational_pointer results_rational_value;
					result->rational_value(results_rational_value);
					safe_mpz_pow_ui(mpq_numref(results_rational_value),my_value,-others_long_value);
					mpq_inv(results_rational_value,results_rational_value);
					return(result);
				} else if (others_long_value == 0) {
//					mpz_set_ui(results_bignum_value,1);
					delete result;
					return(new LongNumberValue(1));
				};
				break;
			case BITWISE_EXCLUSIVE_OR: {
//				mpz_xor_ui(results_bignum_value,my_value,others_long_value);  // not in 3.1
				bignum others_bignum_value;
				mpz_init(others_bignum_value);
				mpz_set_si(others_bignum_value,others_long_value);
				mpz_xor(results_bignum_value,my_value,others_bignum_value);
				mpz_clear(others_bignum_value);
												};
				break;
			case BITWISE_AND: {
				bignum others_bignum_value;
				mpz_init(others_bignum_value);
				mpz_set_si(others_bignum_value,others_long_value);
				mpz_and(results_bignum_value,my_value,others_bignum_value);
				mpz_clear(others_bignum_value);
									};
				break;
			case BITWISE_OR: {
				bignum others_bignum_value;
				mpz_init(others_bignum_value);
				mpz_set_si(others_bignum_value,others_long_value);
				mpz_ior(results_bignum_value,my_value,others_bignum_value);
				mpz_clear(others_bignum_value);
								  };
				break;
			default:
				need_bignums = TRUE;
		};
	} else if (other->double_float_value(other_double_float_value,FALSE)) { // new on 130202
		double my_value_as_double = mpz_get_d(my_value);
		if (_finite(my_value_as_double)) { // conditional new on 200202
			DoubleFloatNumberValue double_float_number_value(my_value_as_double);
			return(double_float_number_value.do_operation(operation,other));
		} else {
			need_rationals = TRUE;
		};
	} else {
		need_bignums = TRUE;
	};
	if (need_bignums) {
		bignum_pointer others_bignum_value;
		if (other->bignum_value(others_bignum_value)) {
			result = new BigNumberValue();
			bignum_pointer results_bignum_value;
			result->bignum_value(results_bignum_value);
			switch (operation) {
				case INCREASE_BY:
					mpz_add(results_bignum_value,my_value,others_bignum_value);
					break;
				case SUBTRACT_FROM:
					mpz_sub(results_bignum_value,my_value,others_bignum_value);
					break;
				case MULTIPLY_BY:
					safe_mpz_mul(results_bignum_value,my_value,others_bignum_value);
					break;
				case DIVIDE_BY:
					need_rationals = !divisible(my_value,others_bignum_value); 
					if (!need_rationals) {
//						mpz_tdiv_q(my_value,others_bignum_value,results_bignum_value);
						// order of args was wrong prior to 170103
						mpz_divexact(results_bignum_value,my_value,others_bignum_value); // faster and better according to the documentation
					};
					break;
				case MODULUS_BY:
					mpz_mod(results_bignum_value,my_value,others_bignum_value);
					break;
				case BITWISE_EXCLUSIVE_OR:
					mpz_xor(results_bignum_value,my_value,others_bignum_value);  
					break;
				case BITWISE_AND:
					mpz_and(results_bignum_value,my_value,others_bignum_value); 
					break;
				case BITWISE_OR:
					mpz_ior(results_bignum_value,my_value,others_bignum_value); 
					break;
				case TO_THE_POWER: {
					// new on 050305 to report that you can't raise to a power greater than 2^31
					// and deal with a few special cases		
					long my_long_value;
					if (long_value(my_long_value,FALSE)) {
						if (my_long_value == 0 || my_long_value == 1) { // if my value is 0 or 1 then just use that 
							mpz_set_ui(results_bignum_value,my_long_value);
							return(result);
						} else if (my_long_value == -1) {
							// result is 1 or -1 depending upon whether exponent is odd or even
							mpz_mod_ui(results_bignum_value,others_bignum_value,2);
							if (mpz_sgn(results_bignum_value) == 0) { // even
								mpz_set_ui(results_bignum_value,1);
							} else {
								mpz_set_si(results_bignum_value,-1);
							};
							return(result);
						};
					};
					if (mpz_sgn(others_bignum_value) < 0) {
						throw S(IDS_ABORT_NUMBER_TOO_SMALL,"the number would get too small for the computer");
					} else {
						throw S(IDS_ABORT_NUMBER_TOO_LARGE,"the number would get too large for the computer");
					};
										 };
				default:
					need_rationals = TRUE; // or some code needs to be added here
			};
		} else {
			need_rationals = TRUE;
		};
	};
	if (need_rationals) {
		if (result != NULL) delete result;
		RationalNumberValue rational_number_value;
		rational_pointer rational_value;
		rational_number_value.rational_value(rational_value);
		mpq_set_z(rational_value,my_value);
		return(rational_number_value.do_operation(operation,other));
	} else {
		// could optimize this when result is a long to not pay overhead of having a bignum...
		return(result);
	};
};

void BigNumberValue::dump(output_stream &stream) {
	stream.put((char) TT_BIGNUM_VALUE);
	long text_size = mpz_sizeinbase(my_value,dumped_bignum_base)+2; // +2 for string terminator and sign (was +1 prior to 031002)
	string buffer = new char[text_size];
	bignum_to_string(buffer,dumped_bignum_base,my_value,text_size);
	stream.write((character *) &text_size, sizeof(text_size));
	stream.write(buffer,text_size);
	delete [] buffer;
};

string bignum_as_string(bignum number, long &string_length, long &string_base, int maximum_base_10_number_size) {
//   string_base = 10; // try base 10 - since nicer if short enough -- caller sets default now
   int estimated_number_length = mpz_sizeinbase(number,string_base)+1; // +1 added on 031002 for sign
   int total_size = estimated_number_length+1; // +1 for string terminator
   if (total_size > maximum_base_10_number_size) {
      // for large numbers takes too long to save and restore in base 10 (and takes up more room)
      string_base = dumped_bignum_base;
      estimated_number_length = mpz_sizeinbase(number,string_base)+1; // added +1 on 161203 for sign
      total_size = estimated_number_length+4; // extra for 32# and terminator
	} else if (string_base != 10) {
		// added string_base != 10 since size is wrong since it doesn't include this base stuff
		total_size = estimated_number_length+4; // extra for 32# and terminator
	};
	string buffer = new char[total_size];
	ZeroMemory(buffer,total_size); // new on 060503 so that if the estimate is wrong can trim the end and get the true length
   if (string_base != 10) {
      itoa(string_base,buffer,10);
      string_length = strlen(buffer); // should be 2 since 32 is two characters long
      buffer[string_length++] = '#'; // radix indicator
   } else {
      string_length = 0; // nothing prefixing since base 10 is the default
   };
	bignum_to_string(buffer+string_length,string_base,number,total_size);
	string_length += estimated_number_length;
	while (string_length-1 > 0 && buffer[string_length-1] == '\0') { // didn't need that long a string
		string_length--;
	};
	return(buffer);
};

void BigNumberValue::print(output_stream &stream, boolean definite) {
	// added definite on 251104 so this produces "the integer 5" or "an integer with 1000 digits"
   long string_base = 10; // decimal default
	long string_length = mpz_sizeinbase(my_value,string_base); // new on 251104 to find the size to see if we need to make the string at all
	if (stop_generating_help(string_length)) {
		// new on 251104 to deal with numbers that are larger than Marty's maximum utterance
		if (stop_generating_help(40)) { // not even enough room to describe how many digits
			stream << S(IDS_SAID_ENOUGH);
		} else if (sign() < 0) {
			stream << S(IDS_NEGATIVE_INTEGER_INDEFINITE,"a negative integer") << space() << S(IDS_WITH) << space() << string_length << space() << S(IDS_DIGITS,"digits");
		} else {
			stream << S(IDS_POSITIVE_INTEGER_INDEFINITE,"an integer") << space() << S(IDS_WITH) << space() << string_length << space() << S(IDS_DIGITS,"digits");
		};
	} else {
		string string_value = bignum_as_string(my_value,string_length,string_base,max_long); // added max_long on 040803 so Marty doesn't read number as base 32
		if (definite) {
			stream << S(IDS_INTEGER_DEFINITE,"the integer") << space();
		};
		stream.write(string_value,string_length);
		delete [] string_value;
	};
};

java_string BigNumberValue::java_value_string() {
	long string_length;
   long string_base = 10; // decimal default
	return(bignum_as_string(my_value,string_length,string_base));
};

#if TT_XML
//xml_element *BigNumberValue::xml_create_element(xml_document *document) {
//   long string_length;
//   long string_base = 10; // decimal default
//   string string_value = bignum_as_string(my_value,string_length,string_base);
//   xml_element *element = create_xml_element(L"Integer",document,string_value,string_length);
//   delete [] string_value;
//   return(element);
//};

void BigNumberValue::xml(xml_element *number_element, xml_document *document) { // rewritten 120703
	long string_length;
   long string_base = 10; // decimal default -- good idea? asked 120703
   string string_value = bignum_as_string(my_value,string_length,string_base);
	add_xml_value(number_element,document,string_value,string_length);
	delete [] string_value;
};
#endif

//boolean BigNumberValue::rubout(NumberFormat number_format, int base) {
//	mpz_tdiv_q_ui(my_value,my_value,base);
//	return(TRUE);
//};


RationalNumberValue::RationalNumberValue(long initial_value) : NumberValue() {
	mpq_init(my_value);
	if (initial_value != 0) { // most common
		mpq_set_si(my_value,initial_value,1);
	};
};

RationalNumberValue::RationalNumberValue(double initial_value) : NumberValue() {
	mpq_init(my_value);
	mpq_set_d(my_value,initial_value);
	set_exact(FALSE); // new on 220202
};

RationalNumberValue::RationalNumberValue(string numerator, string denominator, int base, boolean canonicalize) { // new on 050302
	mpq_init(my_value);
	set_bignum_value_to_string(mpq_numref(my_value),numerator,base);
	set_bignum_value_to_string(mpq_denref(my_value),denominator,base);
	if (canonicalize) { // sometimes caller knows it is not needed (e.g. was OK when dumped)
		mpq_canonicalize(my_value);
	};
};

RationalNumberValue::~RationalNumberValue() {
	mpq_clear(my_value);
};

NumberValue *RationalNumberValue::copy() {
	RationalNumberValue *result = new RationalNumberValue();
	result->set_exact(exact());
	rational_pointer rational_value;
	result->rational_value(rational_value);
	mpq_set(rational_value,my_value);
	return(result);
};

boolean RationalNumberValue::long_value(long &the_long_value, boolean round_off) {
	if (mpz_cmpabs_ui(mpq_denref(my_value),1) == 0) { // denominator is 1
		// special casing numerator being 1 added on 030302
		if (!mpz_fits_slong_p(mpq_numref(my_value))) { 
			return(FALSE); // too big (or small)
		} else {
			the_long_value = mpz_get_si(mpq_numref(my_value));
			return(TRUE);
		};
	};
	if (!round_off) { // if not rounding off and denominator is not 1
		return(FALSE);
	};
	bignum quotient, ignore_remainder;
	mpz_init(quotient);
	mpz_init(ignore_remainder);
	mpz_tdiv_qr(quotient,ignore_remainder,mpq_numref(my_value),mpq_denref(my_value)); // changed to truncate on 080202
	if (!mpz_fits_slong_p(quotient)) { // mpz_cmpabs_ui(quotient,max_long) > 0) {
		mpz_clear(quotient);
		mpz_clear(ignore_remainder);
		return(FALSE); // too big (or small)
	} else {
		// should remainder really be ignored - why not see if it is larger than half the mpq_denref(my_value)
		// and deal better with negative number round off as well (asked 101202)
		the_long_value = mpz_get_si(quotient);
		mpz_clear(quotient);
		mpz_clear(ignore_remainder);
		return(TRUE);
	};
};

boolean RationalNumberValue::double_float_value(double &double_value, boolean ok_to_coerce) {
	if (!ok_to_coerce) return(FALSE);
	double_value = mpq_get_d(my_value);
	return(_finite(double_value)); // was TRUE prior to 200202
};

boolean RationalNumberValue::bignum_value(bignum_pointer &bignum) {
	// new on 091005 to fix a bug introduced on 130505 and also a good idea
	if (mpz_cmp_ui(mpq_denref(my_value),1) == 0) { // numerator is 1
		bignum = mpq_numref(my_value);
		return(TRUE);
	} else {
		return(FALSE);
	};
};

void RationalNumberValue::convert_to_rational(rational &rational_value) {
	mpq_set(rational_value,my_value);
};

boolean RationalNumberValue::round_to_nearest_bignum_value(bignum &bignum_value) {
	// new on 050305 -- is there any way this might some day return FALSE?
	// "nearest" may be a confusing name since it just is "floor" -- throws away fractional part
	bignum ignore_remainder;
	mpz_init(ignore_remainder);
	mpz_tdiv_qr(bignum_value,ignore_remainder,mpq_numref(my_value),mpq_denref(my_value)); 
	mpz_clear(ignore_remainder);
	return(TRUE);
};
	
boolean RationalNumberValue::rational_value(rational_pointer &rational_value) {
	rational_value = &my_value[0];
	return(TRUE);
};

boolean RationalNumberValue::equals(NumberValue *other) {
	rational_pointer others_rational_value;
	if (other->rational_value(others_rational_value)) {
		return(mpq_equal(others_rational_value,my_value));
	};
	mpz_ptr denominator = mpq_denref(my_value); // don't really understand why I need the cast
	if (compare_bignum_with_long(denominator,1) == 0) { // denominator is 1
		long others_long_value;
		if (other->long_value(others_long_value,FALSE)) {
			return(compare_bignum_with_long(denominator,others_long_value) == 0);
		};
		bignum_pointer others_bignum_value;
		if (other->bignum_value(others_bignum_value)) {
			return(mpz_cmp(denominator,others_bignum_value) == 0);
		};
	};
	return(FALSE); // fractions can't equal integers
};

Relation RationalNumberValue::comparison(NumberValue *other) {
	rational_pointer others_rational_value;
	if (other->rational_value(others_rational_value)) {
		return(comparison_to_relation(mpq_cmp(my_value,others_rational_value))); // switched args on 090202
	};
	long others_long_value;
	if (other->long_value(others_long_value,FALSE)) {
		return(comparison_to_relation(compare_rational_with_long(my_value,others_long_value)));
	};
	bignum_pointer others_bignum_value;
	if (other->bignum_value(others_bignum_value)) {
		if (compare_bignum_with_long(mpq_denref(my_value),1) == 0) { // denominator is 1
			return(comparison_to_relation(mpz_cmp(mpq_denref(my_value),others_bignum_value)));
		} else {
			// need to find the bignum equivalent and compare
			bignum quotient, remainder;
			mpz_init(quotient);
			mpz_init(remainder);
			mpz_cdiv_qr(quotient,remainder,mpq_numref(my_value),mpq_denref(my_value));
			int comparison = mpz_cmp(quotient,others_bignum_value);
			if (comparison == 0) {
				int remainder_comparison = mpz_sgn(remainder);
				mpz_clear(quotient);
				mpz_clear(remainder);
				if (remainder_comparison > 0) {
					return(GREATER_THAN_RELATION); // bigger cause of the remainder
				} else {
					return(EQUAL_RELATION);
				};
			};
			mpz_clear(quotient);
			mpz_clear(remainder);
			return(comparison_to_relation(comparison));
		};
	};
	return(INCOMPARABLE_RELATION); // shouldn't happen
};

int RationalNumberValue::sign() {
	return(mpz_sgn(mpq_numref(my_value)));
};

//void RationalNumberValue::negate() {
//	mpz_neg(mpq_numref(my_value),mpq_numref(my_value));
//};

boolean RationalNumberValue::complement() {
	mpz_com(mpq_numref(my_value),mpq_numref(my_value));
	return(TRUE);
};

void rational_mod(rational_pointer results_rational_value, rational &my_value, rational_pointer others_rational_value) { // abstracted out on 200202
//	mpz_mod(mpq_numref(my_value),mpq_numref(my_value),others_bignum_value);
	// completely re-written on 100202 to divide, truncate, multiply, and subtract to get the "remainder"
	bignum quotient,ignore_remainder;
	mpz_init(quotient);
	mpz_init(ignore_remainder);
	mpq_div(results_rational_value,my_value,others_rational_value);
	mpz_tdiv_qr(quotient,ignore_remainder,mpq_numref(results_rational_value),mpq_denref(results_rational_value)); 
	mpz_clear(ignore_remainder);
	mpz_set(mpq_numref(results_rational_value),quotient);
	mpz_clear(quotient);
	mpz_set_ui(mpq_denref(results_rational_value),1);
	mpq_mul(results_rational_value,results_rational_value,others_rational_value); // mulitply the truncated result with the other value
	mpq_sub(results_rational_value,my_value,results_rational_value);
};

void rational_mod_signed_integer(rational_pointer results_rational_value, rational &my_value, long others_value) { 
	rational others_rational_value;
	mpq_init(others_rational_value);
	mpq_set_si(others_rational_value,others_value,1);
	rational_mod(results_rational_value,my_value,others_rational_value);
	mpq_clear(others_rational_value);
};

double my_value_mod_as_double(rational &my_value, long mod_value) { // new on 200202
	rational my_value_mod;
	mpq_init(my_value_mod);
	rational_mod_signed_integer(my_value_mod,my_value,mod_value);
	double result_as_double = mpq_get_d(my_value_mod);
	mpq_clear(my_value_mod);
	return(result_as_double);
};

NumberValue *do_operation_both_rational(rational_pointer results_rational_value, NumberOperation operation, 
													 rational &my_value, rational_pointer others_rational_value,
													 RationalNumberValue *result) { // abstracted out on 200202
	switch (operation) {
		case INCREASE_BY:
			mpq_add(results_rational_value,my_value,others_rational_value);
			break;
		case SUBTRACT_FROM:
			mpq_sub(results_rational_value,my_value,others_rational_value);
			break;
		case MULTIPLY_BY:
			mpq_mul(results_rational_value,my_value,others_rational_value);
			break;
		case DIVIDE_BY: 
			mpq_div(results_rational_value,my_value,others_rational_value);
			break;
		case MODULUS_BY: 
			rational_mod(results_rational_value,my_value,others_rational_value);
			break;
		case TO_THE_POWER: {
			int others_sign = mpz_sgn(mpq_numref(others_rational_value));
			if (others_sign == 0) {
				delete result;
				return(new LongNumberValue(1));
			} else if (others_sign < 0) {
				mpq_neg(others_rational_value,others_rational_value); // temporarily
			};
			string error_message = NULL; // new on 100305 so these aren't lost if retrying
			try {	
				if (mpz_fits_slong_p(mpq_numref(others_rational_value))) { // was mpz_fits_ulong_p prior to 050305
					int others_numerator_long_value = mpz_get_si(mpq_numref(others_rational_value)); // was mpz_get_ui prior to 050305
					if (mpz_fits_slong_p(mpq_denref(others_rational_value))) {  // was mpz_fits_ulong_p prior to 050305
						int others_denominator_long_value = mpz_get_si(mpq_denref(others_rational_value)); // was mpz_get_ui prior to 050305
						if (others_sign < 0) {
							mpq_neg(others_rational_value,others_rational_value);
						};
						// first put both numerator and denomator to the power of the others numerator
						safe_mpz_pow_ui(mpq_numref(results_rational_value),mpq_numref(my_value),others_numerator_long_value);
						safe_mpz_pow_ui(mpq_denref(results_rational_value),mpq_denref(my_value),others_numerator_long_value);
						if (others_denominator_long_value > 1) { // now compute the roots
							if (others_denominator_long_value%2 == 0 && mpz_sgn(mpq_numref(results_rational_value)) < 0) {
								// new on 100305 to catch even roots of negative numbers
								throw S(IDS_NO_IMAGINARY_NUMBERS,"ToonTalk can't make imaginary numbers");
							};
							int exact_numerator = mpz_root(mpq_numref(results_rational_value),mpq_numref(results_rational_value),
																	 others_denominator_long_value); // fixed on 060302
							int exact_denominator = mpz_root(mpq_denref(results_rational_value),mpq_denref(results_rational_value),
																		others_denominator_long_value);
							mpq_canonicalize(results_rational_value);
							if (others_sign < 0) {
								mpq_inv(results_rational_value,results_rational_value);
							};
							boolean exact = (exact_numerator != 0) && (exact_denominator != 0);
#if !TT_GMP_VERSION_4_1
							// need to work around a bug in 3.1
							// so compute the power of the reciprocal and see the result is what we started with
							if (exact) { // check that it really is
								rational result_check;
								mpq_init(result_check);
								safe_mpz_pow_ui(mpq_numref(result_check),mpq_numref(results_rational_value),others_denominator_long_value);
								safe_mpz_pow_ui(mpq_denref(result_check),mpq_denref(results_rational_value),others_denominator_long_value);
								if (others_numerator_long_value > 1) {
									mpz_root(mpq_numref(result_check),mpq_numref(results_rational_value),others_numerator_long_value); 
									mpz_root(mpq_denref(result_check),mpq_denref(results_rational_value),others_numerator_long_value);
								};
								exact = mpq_equal(result_check,my_value);
								mpq_clear(result_check);
							};
#endif
							if (exact) {
								break; // finished and have an exact answer
							} else { // see if get a better answer from double floats
								double my_value_as_double = mpq_get_d(my_value);
								double others_value_as_double = mpq_get_d(others_rational_value);
								double result_as_double = pow(my_value_as_double,others_value_as_double);
								double error_as_double = my_value_as_double-pow(result_as_double,1.0/others_value_as_double);
								double rational_results_as_double = mpq_get_d(results_rational_value);
								double rational_error_as_double = my_value_as_double-pow(rational_results_as_double,1.0/others_value_as_double);
								if (fabs(rational_error_as_double) > fabs(error_as_double)) {
//									mpq_set_d(results_rational_value,result_as_double); // should really create a double float value
									delete result;
									return(new DoubleFloatNumberValue(result_as_double));
								};
								result->set_exact(FALSE);
								};
							break;
						} else { // power is an integer
							mpq_canonicalize(results_rational_value);
							break;
						};
					};
					delete result;
//					return(NULL); // fail because exponent has true bignums in it // commented out on 050305
				};
			} catch (string s) {
				error_message = s;
			} catch (...) {
				// if can't do it exact above try using doubles instead...
			};
			double my_double_value = mpq_get_d(my_value);
			if (_finite(my_double_value)) {
				double other_double_value  = mpq_get_d(others_rational_value);
				if (_finite(other_double_value)) {
					double result = pow(my_double_value,other_double_value);
					if (_finite(result)) {
						return(new DoubleFloatNumberValue(result));
					};
				};
			};
			if (error_message != NULL) {
				throw error_message;
			};
			throw S(IDS_ABORT_NUMBER_TOO_SMALL_OR_BIG); // worth being more clever here??
				// note that it isn't strictly true there may be a good rational approximation
									};
			break;
		case BITWISE_EXCLUSIVE_OR:
			mpz_xor(mpq_numref(results_rational_value),mpq_numref(my_value),mpq_numref(others_rational_value));
			mpz_xor(mpq_denref(results_rational_value),mpq_denref(my_value),mpq_denref(others_rational_value));
			mpq_canonicalize(results_rational_value);
			break;
		case BITWISE_AND:
			mpz_and(mpq_numref(results_rational_value),mpq_numref(my_value),mpq_numref(others_rational_value));
			mpz_and(mpq_denref(results_rational_value),mpq_denref(my_value),mpq_denref(others_rational_value));
			mpq_canonicalize(results_rational_value);
			break;
		case BITWISE_OR:
			mpz_ior(mpq_numref(results_rational_value),mpq_numref(my_value),mpq_numref(others_rational_value));
			mpz_ior(mpq_denref(results_rational_value),mpq_denref(my_value),mpq_denref(others_rational_value));
			mpq_canonicalize(results_rational_value);
			break;
	};
	return(result);
};

NumberValue *RationalNumberValue::do_operation(NumberOperation operation, NumberValue *other) {
	if (unary_operation(operation)) {// new on 120202
		switch (operation) {
			case INTEGER_PART: { 
				BigNumberValue *result = new BigNumberValue();
				bignum_pointer result_bignum_value;
				result->bignum_value(result_bignum_value);
				bignum ignore_remainder;
				mpz_init(ignore_remainder);
				mpz_tdiv_qr(result_bignum_value,ignore_remainder,mpq_numref(my_value),mpq_denref(my_value)); 
				mpz_clear(ignore_remainder);
				if (mpz_fits_slong_p(result_bignum_value)) {
					// new on 181203 to convert to an integer when possible
					long long_result = mpz_get_si(result_bignum_value);
					delete result;
					return(new LongNumberValue(long_result));
				} else {
					return(result);
				};
									 };
			case FRACTION_PART: {
				RationalNumberValue *result = new RationalNumberValue();
				rational_pointer result_rational_value;
				result->rational_value(result_rational_value);
				bignum my_bignum_value;
				mpz_init(my_bignum_value);
				bignum ignore_remainder;
				mpz_init(ignore_remainder);
				mpz_tdiv_qr(my_bignum_value,ignore_remainder,mpq_numref(my_value),mpq_denref(my_value)); 
				mpz_clear(ignore_remainder);
				rational my_truncated_rational_value;
				mpq_init(my_truncated_rational_value);
				mpz_set(mpq_numref(my_truncated_rational_value),my_bignum_value);
				mpq_sub(result_rational_value,my_value,my_truncated_rational_value);
				mpq_clear(my_truncated_rational_value);
				mpz_clear(my_bignum_value);
//				mpz_abs(mpq_numref(result_rational_value),mpq_numref(result_rational_value)); -- no it is better to have a negative fraction since the two parts should add up
				return(result);
									  };
			case NUMERATOR: { 
				BigNumberValue *result = new BigNumberValue();
				bignum_pointer result_bignum_value;
				result->bignum_value(result_bignum_value);
				mpz_set(result_bignum_value,mpq_numref(my_value));
				return(result);
									 };
			case DENOMINATOR: { 
				BigNumberValue *result = new BigNumberValue();
				bignum_pointer result_bignum_value;
				result->bignum_value(result_bignum_value);
				mpz_set(result_bignum_value,mpq_denref(my_value));
				return(result);
									 };
			case SINE: 
				return(new DoubleFloatNumberValue(sin(my_value_mod_as_double(my_value,360)*radians_per_degree))); 
				// parens were off prior to 050305
			case COSINE:
				return(new DoubleFloatNumberValue(sin(my_value_mod_as_double(my_value,360)*radians_per_degree+half_pi))); 
				// parens were off prior to 050305
			case TANGENT: 
				return(new DoubleFloatNumberValue(tan(my_value_mod_as_double(my_value,360)*radians_per_degree))); 
				// parens were off prior to 050305
			case ARCSINE: {
				double my_value_as_double = mpq_get_d(my_value);
				if (!_finite(my_value_as_double)) { // raise an exception (new on 200202)
					throw "too big for 64-bit IEEE float to compute arc sine";
				};
				double result_as_double = asin(my_value_as_double)/radians_per_degree;
				return(new DoubleFloatNumberValue(result_as_double));
						  };
			case ARCCOSINE: {
				double my_value_as_double = mpq_get_d(my_value);
				if (!_finite(my_value_as_double)) { // raise an exception (new on 200202)
					throw "too big for 64-bit IEEE float to compute arc cosine";
				};
				double result_as_double = asin(my_value_as_double)/radians_per_degree-90.0;
				return(new DoubleFloatNumberValue(result_as_double));
						  };
			case ARCTANGENT: {
				double my_value_as_double = mpq_get_d(my_value);
				if (!_finite(my_value_as_double)) { // raise an exception (new on 200202)
					throw "too big for 64-bit IEEE float to compute arc tangent";
				};
				double result_as_double = atan(my_value_as_double)/radians_per_degree;
				return(new DoubleFloatNumberValue(result_as_double));
						  };
			case NATURAL_LOG: {
				double my_value_as_double = mpq_get_d(my_value);
				if (!_finite(my_value_as_double)) { 
					// new on 050305
					bignum my_value_as_bignum;
					mpz_init(my_value_as_bignum);
					if (round_to_nearest_bignum_value(my_value_as_bignum)) {
						NumberValue *result = do_unary_operation_to_big_number_value(operation,my_value_as_bignum);
						mpz_clear(my_value_as_bignum);
						if (result != NULL) {
							return(result);
						};
					};
					mpz_clear(my_value_as_bignum);
					// shouldn't happen since can always convert a rational to a big number
					// raise an exception (new on 200202)
					throw "too big for 64-bit IEEE float to compute natural log"; // string updated on 090104
				};
				double result_as_double = log(my_value_as_double);
				return(new DoubleFloatNumberValue(result_as_double));
						  };
			case LOG10: {
				double my_value_as_double = mpq_get_d(my_value);
				if (!_finite(my_value_as_double)) {
					// new on 050305
					bignum my_value_as_bignum;
					mpz_init(my_value_as_bignum);
					if (round_to_nearest_bignum_value(my_value_as_bignum)) {
						NumberValue *result = do_unary_operation_to_big_number_value(operation,my_value_as_bignum);
						mpz_clear(my_value_as_bignum);
						if (result != NULL) {
							return(result);
						};
					};
					mpz_clear(my_value_as_bignum);
					// raise an exception (new on 200202)
					throw "too big for 64-bit IEEE float to compute log base 10"; // string updated on 090104
				};
				double result_as_double = log10(my_value_as_double);
				return(new DoubleFloatNumberValue(result_as_double));
						  };
		};
	};
	RationalNumberValue *result = new RationalNumberValue();
	rational_pointer results_rational_value;
	result->rational_value(results_rational_value);
	long others_long_value;
	double other_double_float_value;
	bignum_pointer others_bignum_value;
	if (other->long_value(others_long_value,FALSE)) {
		switch (operation) { // these operations are to the numerator (and are kind of weird) --
			// the denominator is 1 - of other's value - is that OK?
			// moved on 090202
//			case '%':
//				mpz_mod_ui(mpq_numref(results_rational_value),mpq_numref(my_value),abs(others_long_value));
//				mpz_set(mpq_denref(results_rational_value),mpq_denref(my_value));
//				mpq_canonicalize(results_rational_value);
//				return(result);
			case BITWISE_EXCLUSIVE_OR: {
				bignum others_bignum_value;
				mpz_init(others_bignum_value);
				mpz_set_si(others_bignum_value,others_long_value);
				mpz_xor(mpq_numref(results_rational_value),mpq_numref(my_value),others_bignum_value);
				mpz_clear(others_bignum_value);
				mpz_set(mpq_denref(results_rational_value),mpq_denref(my_value));
				mpq_canonicalize(results_rational_value);
				return(result);
						 };
			case BITWISE_AND: {
				bignum others_bignum_value;
				mpz_init(others_bignum_value);
				mpz_set_si(others_bignum_value,others_long_value);
				mpz_and(mpq_numref(results_rational_value),mpq_numref(my_value),others_bignum_value);
				mpz_clear(others_bignum_value);
				mpz_set(mpq_denref(results_rational_value),mpq_denref(my_value));
				mpq_canonicalize(results_rational_value);
				return(result);
						 };
			case BITWISE_OR: {
				bignum others_bignum_value;
				mpz_init(others_bignum_value);
				mpz_set_si(others_bignum_value,others_long_value);
				mpz_ior(mpq_numref(results_rational_value),mpq_numref(my_value),others_bignum_value);
				mpz_clear(others_bignum_value);
				mpz_set(mpq_denref(results_rational_value),mpq_denref(my_value));
				mpq_canonicalize(results_rational_value);
				return(result);
						 };
		};
		rational others_rational_value;
		mpq_init(others_rational_value);
		mpz_set_si(mpq_numref(others_rational_value),others_long_value);
		mpz_set_ui(mpq_denref(others_rational_value),1);
		switch (operation) {
			case INCREASE_BY:
				mpq_add(results_rational_value,my_value,others_rational_value);
				break;
			case SUBTRACT_FROM:
				mpq_sub(results_rational_value,my_value,others_rational_value);
				break;
			case MULTIPLY_BY:
				mpq_mul(results_rational_value,my_value,others_rational_value);
				break;
			case DIVIDE_BY: 
				mpq_div(results_rational_value,my_value,others_rational_value);
				break;
			case MODULUS_BY: 
				// completely re-written on 100202 to divide, truncate, multiply, and subtract to get the "remainder"
				mpq_div(results_rational_value,my_value,others_rational_value);
				bignum quotient,ignore_remainder;
				mpz_init(quotient);
				mpz_init(ignore_remainder);
				mpz_tdiv_qr(quotient,ignore_remainder,mpq_numref(results_rational_value),mpq_denref(results_rational_value)); 
				mpz_clear(ignore_remainder);
				mpz_set(mpq_numref(results_rational_value),quotient);
				mpz_clear(quotient);
				mpz_set_ui(mpq_denref(results_rational_value),1);
				mpq_mul(results_rational_value,results_rational_value,others_rational_value); // mulitply the truncated result with the other value
				mpq_sub(results_rational_value,my_value,results_rational_value);
				break;
			case TO_THE_POWER: // new on 120202
				if (others_long_value > 0) {
					safe_mpz_pow_ui(mpq_numref(results_rational_value),mpq_numref(my_value),others_long_value);
					safe_mpz_pow_ui(mpq_denref(results_rational_value),mpq_denref(my_value),others_long_value);
					mpq_canonicalize(results_rational_value);
				} else if (others_long_value < 0) {
					safe_mpz_pow_ui(mpq_numref(results_rational_value),mpq_numref(my_value),-others_long_value);
					safe_mpz_pow_ui(mpq_denref(results_rational_value),mpq_denref(my_value),-others_long_value);
					mpq_canonicalize(results_rational_value);
					mpq_inv(results_rational_value,results_rational_value);
				} else if (others_long_value == 0) {
					delete result;
					return(new LongNumberValue(1));
				};
				break;
		};
		mpq_clear(others_rational_value);
		return(result); 
	} else if (other->bignum_value(others_bignum_value)) {
		switch (operation) { // these operations are to the numerator (and are kind of weird)
//			case '%':
//				mpz_mod(mpq_numref(results_rational_value),mpq_numref(my_value),others_bignum_value);
//				mpz_set(mpq_denref(results_rational_value),mpq_denref(my_value));
//				mpq_canonicalize(results_rational_value);
//				return(result);
			case BITWISE_EXCLUSIVE_OR:
				mpz_xor(mpq_numref(results_rational_value),mpq_numref(my_value),others_bignum_value);
				mpz_set(mpq_denref(results_rational_value),mpq_denref(my_value));
				mpq_canonicalize(results_rational_value);
				return(result);
			case BITWISE_AND:
				mpz_and(mpq_numref(results_rational_value),mpq_numref(my_value),others_bignum_value);
				mpz_set(mpq_denref(results_rational_value),mpq_denref(my_value));
				mpq_canonicalize(results_rational_value);
				return(result);
			case BITWISE_OR:
				mpz_ior(mpq_numref(results_rational_value),mpq_numref(my_value),others_bignum_value);
				mpz_set(mpq_denref(results_rational_value),mpq_denref(my_value));
				mpq_canonicalize(results_rational_value);
				return(result);
			case TO_THE_POWER: {
				// new on 050305 to report that you can't raise to a power greater than 2^31
				boolean absolute_value_less_than_one = (mpz_cmpabs (mpq_numref(my_value),mpq_denref(my_value)) < 0); // numerator less than denominator
				boolean exponent_positive = (mpz_sgn(others_bignum_value) > 0);
				if ((absolute_value_less_than_one && exponent_positive) || (!absolute_value_less_than_one && !exponent_positive)) {
					throw S(IDS_ABORT_NUMBER_TOO_SMALL,"the number would get too small for the computer");
				} else {
					throw S(IDS_ABORT_NUMBER_TOO_LARGE,"the number would get too large for the computer");
				};
									 };
		};
		rational others_rational_value;
		mpq_init(others_rational_value);
		mpz_set(mpq_numref(others_rational_value),others_bignum_value);
		mpz_set_ui(mpq_denref(others_rational_value),1);
		switch (operation) {
			case INCREASE_BY:
				mpq_add(results_rational_value,my_value,others_rational_value);
				break;
			case SUBTRACT_FROM:
				mpq_sub(results_rational_value,my_value,others_rational_value);
				break;
			case MULTIPLY_BY:
				mpq_mul(results_rational_value,my_value,others_rational_value);
				break;
			case DIVIDE_BY: 
				mpq_div(results_rational_value,my_value,others_rational_value);
				break;
			case MODULUS_BY:
				// completely re-written on 100202 to divide, truncate, multiply, and subtract to get the "remainder"
				bignum quotient,ignore_remainder;
				mpz_init(quotient);
				mpz_init(ignore_remainder);
				mpz_tdiv_qr(quotient,ignore_remainder,mpq_numref(results_rational_value),mpq_denref(results_rational_value)); 
				mpz_clear(ignore_remainder);
				mpz_set(mpq_numref(results_rational_value),quotient);
				mpz_clear(quotient);
				mpz_set_ui(mpq_denref(results_rational_value),1);
				mpq_mul(results_rational_value,results_rational_value,others_rational_value); // mulitply the truncated result with the other value
				mpq_sub(results_rational_value,my_value,results_rational_value);
				break;
		};
		mpq_clear(others_rational_value);
		return(result); 
	} else if (other->double_float_value(other_double_float_value,FALSE)) { // new on 130202
		double my_value_as_double = mpq_get_d(my_value);
		if (_finite(my_value_as_double)) { // new on 200202 to check if finite
			DoubleFloatNumberValue double_float_number_value(my_value_as_double);
			return(double_float_number_value.do_operation(operation,other));
		};
	}; // else { -- changed since might be can't convert to a double float
	rational_pointer others_rational_value;
	if (other->rational_value(others_rational_value)) { 
		return(do_operation_both_rational(results_rational_value,operation,my_value,others_rational_value,result));
	} else { // other isn't a rational so create it 
		rational others_value; // allocate it just in this branch
		mpq_init(others_value);
		other->convert_to_rational(others_value);
		others_rational_value = &others_value[0];
		NumberValue *new_result = do_operation_both_rational(results_rational_value,operation,my_value,others_rational_value,result);
		mpq_clear(others_value); // this is why I don't just return the above
		return(new_result); // typically the same as result
	};
};

void RationalNumberValue::dump(output_stream &stream) {
	stream.put((char) TT_RATIONAL_VALUE);
	long numerator_text_size = mpz_sizeinbase(mpq_numref(my_value),dumped_bignum_base); 
	long denominator_text_size = mpz_sizeinbase(mpq_denref(my_value),dumped_bignum_base);
	string buffer = new char[max(denominator_text_size,numerator_text_size)+2]; // +1 for sign and +1 for string terminator
	bignum_to_string(buffer,dumped_bignum_base,mpq_numref(my_value),numerator_text_size);
	numerator_text_size = strlen(buffer)+1; // new on 060702 for more accuracy (and somehow without this triggered a bug)
	stream.write((character *) &numerator_text_size, sizeof(numerator_text_size));
	stream.write(buffer,numerator_text_size);
	bignum_to_string(buffer,dumped_bignum_base,mpq_denref(my_value),denominator_text_size);
	denominator_text_size = strlen(buffer)+1; // new on 060702
	stream.write((character *) &denominator_text_size, sizeof(denominator_text_size));
	stream.write(buffer,denominator_text_size);
	delete [] buffer;
};

wide_string RationalNumberValue::value_as_wide_string(boolean &could_be_integer) {
   // this returns a wide string since that is what XML wants
   if (compare_bignum_with_long(mpq_denref(my_value),1) == 0) { // denominator is 1 so just return numerator
      long string_length;
      long string_base = 10; // decimal default
      string string_value = bignum_as_string(mpq_numref(my_value),string_length,string_base);
      wide_string wide_string_value = copy_wide_string(string_value,string_length);
      could_be_integer = (string_length < 10); // don't bother to optimize those between a billion and max_long
      delete [] string_value;
      return(wide_string_value);
   } else {
      long numerator_length;
      long numerator_base = 10;
      string numerator_string = bignum_as_string(mpq_numref(my_value),numerator_length,numerator_base);
      long denominator_length;
      string denominator_string = bignum_as_string(mpq_denref(my_value),denominator_length,numerator_base); 
      // by default use numerator_base here so we don't get confusion like 32#abc/10 but instead we get 32#abc/32#a
      // we can get 10/32#abc...
      wide_string wide_string_value = new wide_character[numerator_length+denominator_length+2]; // for terminator and / 
      copy_wide_string(numerator_string,numerator_length,wide_string_value);
      delete [] numerator_string;
      wide_string_value[numerator_length] = '/';
      copy_wide_string(denominator_string,denominator_length,wide_string_value+numerator_length+1);
      delete [] denominator_string;
      could_be_integer = FALSE;
      return(wide_string_value);
   };
};

void RationalNumberValue::print(output_stream &stream, boolean definite) {
	long numerator_text_size = mpz_sizeinbase(mpq_numref(my_value),10); 
	long denominator_text_size = mpz_sizeinbase(mpq_denref(my_value),10);
	if (stop_generating_help(numerator_text_size+denominator_text_size+2)) {
		// new on 251104 to deal with numbers that are larger than Marty's maximum utterance
		if (stop_generating_help(40)) { // not even enough room to describe how many digits
			stream << S(IDS_SAID_ENOUGH);
		} else if (sign() < 0) {
			stream << S(IDS_NEGATIVE_RATIONAL_INDEFINITE,"a negative rational number") << space();
			if (numerator_text_size > 13) {
				stream << S(IDS_WITH) << space() << numerator_text_size << space() << S(IDS_DIGITS,"digits");
			} else {
				char buffer[15];
				bignum_to_string(buffer,10,mpq_numref(my_value),15);
				stream << buffer;
			};
			stream << space() << S(IDS_OVER) << space();
			if (denominator_text_size > 13) {
				stream << denominator_text_size << space() << S(IDS_DIGITS,"digits");
			} else {
				char buffer[15];
				bignum_to_string(buffer,10,mpq_denref(my_value),15);
				stream << buffer;
			};
		} else {
			stream << S(IDS_POSITIVE_RATIONAL_INDEFINITE,"a rational number") << space();
			if (numerator_text_size > 13) {
				stream << S(IDS_WITH) << space() << numerator_text_size << space() << S(IDS_DIGITS,"digits");
			} else {
				char buffer[15];
				bignum_to_string(buffer,10,mpq_numref(my_value),15);
				stream << buffer;
			};
			stream << space() << S(IDS_OVER) << space();
			if (denominator_text_size > 13) {
				stream << denominator_text_size << space() << S(IDS_DIGITS,"digits");
			} else {
				char buffer[15];
				bignum_to_string(buffer,10,mpq_denref(my_value),15);
				stream << buffer;
			};
		};
		return;
	};
	if (definite) {
		stream << S(IDS_RATIONAL_DEFINITE,"the rational number") << space();
	};
	string buffer = new char[max(denominator_text_size,numerator_text_size)+2]; // prior to 281004 was +1 but need room for terminator and sign
	bignum_to_string(buffer,10,mpq_numref(my_value),numerator_text_size);
	// new on 020702 since don't want this to terminate an output string
	if (numerator_text_size-1 > 0 && buffer[numerator_text_size-1] == '\0') { // didn't need that long a string
		// what if off by more than one character???
		numerator_text_size--;
	};
//	stream.write(buffer,numerator_text_size); 
	stream << buffer; // rewritten on 281004 since may now have a minus sign -- removed  << endl on 251104
	if (compare_bignum_with_long(mpq_denref(my_value),1) != 0) { // denominator is not 1
		stream.put('/');
		bignum_to_string(buffer,10,mpq_denref(my_value),denominator_text_size);
		// new on 020702 since don't want this to terminate an output string
		if (denominator_text_size-1 > 0 && buffer[denominator_text_size-1] == '\0') { // didn't need that long a string
			// what if off by more than one character???
			denominator_text_size--;
		};
//		stream.write(buffer,denominator_text_size);
		stream << buffer; // rewritten on 281004 even though shouldn't be negative -- removed  << endl on 251104
	};
	delete [] buffer;
};

java_string RationalNumberValue::java_value_string() {
	long numerator_text_size = mpz_sizeinbase(mpq_numref(my_value),10); 
	long denominator_text_size = mpz_sizeinbase(mpq_denref(my_value),10);
	int total_size = denominator_text_size+numerator_text_size+3; // for slash and for terminating NULL -- and 1 more on 051103 for negative numbers
	string buffer = new char[total_size];
	ZeroMemory(buffer,total_size); // new on 060503 so that if the estimate is wrong can trim the end and get the true length
	bignum_to_string(buffer,10,mpq_numref(my_value),numerator_text_size);
	numerator_text_size = strlen(buffer); // rewritten on 051103 since if negative might lose a digit otherwise
	//while (numerator_text_size-1 > 0 && buffer[numerator_text_size-1] == '\0') { // didn't need that long a string
	//	// what if off by more than one character??? -- changed to while on 060503
	//	numerator_text_size--;
	//};
	if (compare_bignum_with_long(mpq_denref(my_value),1) != 0) { // denominator is not 1
		buffer[numerator_text_size++] = '/';
		bignum_to_string(buffer+numerator_text_size,10,mpq_denref(my_value),denominator_text_size);
	};
	return(buffer);
};

//	wide_string wide_result;
//	long text_length;
//	NumberValue *temp_value = copy();
//	NumberValue *ten_to_some_power = new LongNumberValue(10);
//	NumberValue *power = new LongNumberValue(tt_digits_after_decimal_point_in_java);
//	ten_to_some_power = ten_to_some_power->do_operation(TO_THE_POWER,power);
//#if TT_DEBUG_ON
//	Rational *temp_ten_to_some_power_number = new Rational(0,0,0,0,0,ten_to_some_power,TT_BIGNUM_DECIMAL);
//	temp_ten_to_some_power_number->current_wide_text(wide_result,text_length);
//#endif
//	delete power;
//	temp_value = temp_value->do_operation(MULTIPLY_BY,ten_to_some_power);
//#if TT_DEBUG_ON
//	Rational *temp_number1 = new Rational(0,0,0,0,0,temp_value,TT_BIGNUM_DECIMAL);
//	temp_number1->current_wide_text(wide_result,text_length);
//#endif
//	temp_value = temp_value->do_operation(INTEGER_PART,NULL);
//#if TT_DEBUG_ON
//	Rational *temp_number2 = new Rational(0,0,0,0,0,temp_value,TT_BIGNUM_DECIMAL);
//	temp_number2->current_wide_text(wide_result,text_length);
//#endif
//	temp_value = temp_value->do_operation(DIVIDE_BY,ten_to_some_power);
//	Rational *temp_number = new Rational(0,0,0,0,0,temp_value,TT_BIGNUM_DECIMAL);
//	delete ten_to_some_power;
//	temp_number->current_wide_text(wide_result,text_length);
//	string result = copy_narrow_string(wide_result,text_length);
////	delete [] wide_result; // held on to by temp_number
//	temp_number->destroy();
//	return(result);

#if TT_XML

//void RationalNumberValue::xml(xml_element *number_element, xml_document *document) { // restored on 050803 except new arg list and name
//   boolean could_be_integer;
//   wide_string string_value = value_as_wide_string(could_be_integer);
//   xml_element *element;
//   if (could_be_integer) {
//      element = create_xml_element(L"Integer",document,string_value); // new on 071002 since Integer is just an optimization of rational
//   } else {
//      element = create_xml_element(L"RationalNumber",document,string_value);
//   };
//   delete [] string_value;
//   xml_append_child(element,number_element);
//};

wide_string RationalNumberValue::xml_tag() { // new on 120703 to preserve the old optimization in xml_create_element
   if (compare_bignum_with_long(mpq_denref(my_value),1) == 0) { // denominator is 1 so just return numerator
		return(L"Integer");
	} else {
		return(L"RationalNumber");
	};
};

void RationalNumberValue::xml(xml_element *number_element, xml_document *document) { // rewritten 120703
	boolean could_be_integer;
   wide_string string_value = value_as_wide_string(could_be_integer);
   add_xml_value(number_element,document,string_value);
	delete [] string_value;
};

#endif

//boolean RationalNumberValue::rubout(NumberFormat number_format, int base) {
//	switch (number_format) {
//		case TT_LONG:
//		case TT_BIGNUM:
//	//	if (compare_bignum_with_long(mpq_denref(my_value),1) == 0) { // denominator is 1 -- could optimize this by becoming a bignum or long
//			mpz_tdiv_q_ui(mpq_numref(my_value),mpq_numref(my_value),base);
//			mpq_canonicalize(my_value);
//			return(TRUE);
//		case TT_RATIONAL:
//			mpz_tdiv_q_ui(mpq_denref(my_value),mpq_denref(my_value),base);
//			if (compare_bignum_with_long(mpq_denref(my_value),0) == 0) { // last digit so became zero
//				mpz_set_ui(mpq_denref(my_value),1); // reset it to one
//			};
//			mpq_canonicalize(my_value);
//			return(TRUE);
//		default:
//			return(FALSE); // too hard to deal with decimals or integer plus fraction formats here
//	};
//};

boolean unary_operation(NumberOperation operation) {
	switch (operation) {
		case INTEGER_PART:
		case FRACTION_PART:
		case NUMERATOR:
		case DENOMINATOR:
		case SINE:
		case COSINE:
		case TANGENT:
		case ARCSINE:
		case ARCCOSINE:
		case ARCTANGENT:
		case NATURAL_LOG:
		case LOG10:
			return(TRUE);
		default:
			return(FALSE);
	};
};

NumberValue *load_number_value(InputStream *stream, int version) {
	unsigned char type = stream->get();
	switch (type) {
		case TT_LONG_VALUE: {
			long my_value;
			stream->read((string) &my_value, sizeof(my_value));
			return(new LongNumberValue(my_value));
						  };
		case TT_DOUBLE_FLOAT_VALUE: {
			double my_value;
			stream->read((string) &my_value, sizeof(my_value));
			return(new DoubleFloatNumberValue(my_value));
						  };
		case TT_BIGNUM_VALUE: {
			long text_size;
			stream->read((string) &text_size, sizeof(text_size));
			if (text_size < 0) return(NULL); // new on 020702 to be more robust - could also check for too large a value
			string my_value = new char[text_size];
			stream->read((string) my_value,text_size);
			BigNumberValue *result = new BigNumberValue(my_value,dumped_bignum_base);
			delete [] my_value;
			return(result);
							 };
		case TT_RATIONAL_VALUE: {
			long numerator_text_size, denominator_text_size; 
			stream->read((string) &numerator_text_size, sizeof(numerator_text_size));
			if (numerator_text_size < 0) return(NULL); // new on 211104 for robustness
			string numerator_string = new char[numerator_text_size];
			stream->read((string) numerator_string,numerator_text_size);
			stream->read((string) &denominator_text_size, sizeof(denominator_text_size));
			if (denominator_text_size < 0) return(NULL); // new on 211104 for robustness (happened when describing a number since trying ignore using too small a buffer)
			string denominator_string = new char[denominator_text_size];
			stream->read((string) denominator_string,denominator_text_size);
			RationalNumberValue *result = new RationalNumberValue(numerator_string,denominator_string,dumped_bignum_base,FALSE);
			delete [] numerator_string;
			delete [] denominator_string;
			return(result);
							 };
		default:
#if TT_DEBUG_ON
			if (type != NOTHING_MARKER) {
				log("Loading a value in the old file format whose value can't be interpreted.");
			};
#endif
			return(NULL); // OK if type is NOTHING_MARKER -- otherwise warn??
	};
};

NumberValue *number_value(NumberValueFormat value_format, long long_value, double double_float_value, string text_value1, string text_value2) {
	switch(value_format) {
		case TT_LONG_VALUE:
			return(new LongNumberValue(long_value));
		case TT_DOUBLE_FLOAT_VALUE:
			return(new DoubleFloatNumberValue(double_float_value));
		case TT_BIGNUM_VALUE: 
			return(new BigNumberValue(text_value1,dumped_bignum_base));
		case TT_RATIONAL_VALUE:
			return(new RationalNumberValue(text_value1,text_value2,dumped_bignum_base,FALSE));
		default:
#if TT_DEBUG_ON
			tt_error_file() << "number_value called with invalid value_format." << endl;
#endif
			return(NULL);
	};
};








