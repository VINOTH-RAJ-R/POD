/************************************************************************************
 * /src/numericc.c
 * C functions for numericc (marked numeric) data type.
 ************************************************************************************/

#include "markednull.h"
#include "numericc.h"
#include "intt.h"

PG_MODULE_MAGIC;

// I/O Functions

PG_FUNCTION_INFO_V1(mn_numericc_input);
Datum mn_numericc_input(PG_FUNCTION_ARGS)
{
    char* str = PG_GETARG_CSTRING(0);
    Numericc to_return;

	uint32 null_id;

    // input is a marked null
    if (mn_is_str_marked_null(str))
    {
        // get the string after the colon.
        str += 5;

		null_id = mn_str_to_null_identifier(str);
		if (null_id == 0)
		{
            ereport(ERROR,
            (
                errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                errmsg(NUMERICC_ERROR_MSG)
            ));
        }

		to_return = id_to_numericc(null_id);
		PG_RETURN_NUMERICC(to_return);
    }
    // input is a constant
    else
    {
		// START COPY
#ifdef NOT_USED
		Oid			typelem = PG_GETARG_OID(1);
#endif
		int32		typmod = PG_GETARG_INT32(2);
		Node	   *escontext = fcinfo->context;
		Numeric		res;
		const char *cp;
		const char *numstart;
		int			sign;

		/* Skip leading spaces */
		cp = str;
		while (*cp)
		{
			if (!isspace((unsigned char) *cp))
				break;
			cp++;
		}

		/*
		* Process the number's sign. This duplicates logic in set_var_from_str(),
		* but it's worth doing here, since it simplifies the handling of
		* infinities and non-decimal integers.
		*/
		numstart = cp;
		sign = NUMERIC_POS;

		if (*cp == '+')
			cp++;
		else if (*cp == '-')
		{
			sign = NUMERIC_NEG;
			cp++;
		}

		/*
		* Check for NaN and infinities.  We recognize the same strings allowed by
		* float8in().
		*
		* Since all other legal inputs have a digit or a decimal point after the
		* sign, we need only check for NaN/infinity if that's not the case.
		*/
		if (!isdigit((unsigned char) *cp) && *cp != '.')
		{
			/*
			* The number must be NaN or infinity; anything else can only be a
			* syntax error. Note that NaN mustn't have a sign.
			*/
			if (pg_strncasecmp(numstart, "NaN", 3) == 0)
			{
				res = make_result(&const_nan);
				cp = numstart + 3;
			}
			else if (pg_strncasecmp(cp, "Infinity", 8) == 0)
			{
				res = make_result(sign == NUMERIC_POS ? &const_pinf : &const_ninf);
				cp += 8;
			}
			else if (pg_strncasecmp(cp, "inf", 3) == 0)
			{
				res = make_result(sign == NUMERIC_POS ? &const_pinf : &const_ninf);
				cp += 3;
			}
			else
				goto invalid_syntax;

			/*
			* Check for trailing junk; there should be nothing left but spaces.
			*
			* We intentionally do this check before applying the typmod because
			* we would like to throw any trailing-junk syntax error before any
			* semantic error resulting from apply_typmod_special().
			*/
			while (*cp)
			{
				if (!isspace((unsigned char) *cp))
					goto invalid_syntax;
				cp++;
			}

			if (!apply_typmod_special(res, typmod, escontext))
				PG_RETURN_NULL();
		}
		else
		{
			/*
			* We have a normal numeric value, which may be a non-decimal integer
			* or a regular decimal number.
			*/
			NumericVar	value;
			int			base;
			bool		have_error;

			init_var(&value);

			/*
			* Determine the number's base by looking for a non-decimal prefix
			* indicator ("0x", "0o", or "0b").
			*/
			if (cp[0] == '0')
			{
				switch (cp[1])
				{
					case 'x':
					case 'X':
						base = 16;
						break;
					case 'o':
					case 'O':
						base = 8;
						break;
					case 'b':
					case 'B':
						base = 2;
						break;
					default:
						base = 10;
				}
			}
			else
				base = 10;

			/* Parse the rest of the number and apply the sign */
			if (base == 10)
			{
				if (!set_var_from_str(str, cp, &value, &cp, escontext))
					PG_RETURN_NULL();
				value.sign = sign;
			}
			else
			{
				if (!set_var_from_non_decimal_integer_str(str, cp + 2, sign, base,
														&value, &cp, escontext))
					PG_RETURN_NULL();
			}

			/*
			* Should be nothing left but spaces. As above, throw any typmod error
			* after finishing syntax check.
			*/
			while (*cp)
			{
				if (!isspace((unsigned char) *cp))
					goto invalid_syntax;
				cp++;
			}

			if (!apply_typmod(&value, typmod, escontext))
				PG_RETURN_NULL();

			res = make_result_opt_error(&value, &have_error);

			if (have_error)
				ereturn(escontext, (Datum) 0,
						(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
						errmsg("value overflows numeric format")));

			free_var(&value);
		}
		// START COPY
	

		to_return = numeric_to_numericc(res);

		pfree(res);

		PG_RETURN_NUMERICC(to_return);

invalid_syntax:
		ereturn(escontext, (Datum) 0,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				errmsg("invalid input syntax for type %s: \"%s\"",
						"numeric", str)));
    }

    
}

PG_FUNCTION_INFO_V1(mn_numericc_output);
Datum mn_numericc_output(PG_FUNCTION_ARGS)
{
    Numericc numericc = PG_GETARG_NUMERICC(0);
    char numericc_modified = 0;
    char* to_return;

    if (MN_NUMERICC_IS_CONST(numericc))
    {
        Numeric num = numericc_to_numeric(numericc, &numericc_modified);
        // START COPY
        NumericVar	x;

        /*
         * Handle NaN
         */
        if (NUMERIC_IS_NAN(num))
            to_return = pstrdup("NaN");
		else {

			/*
			* Get the number in the variable format.
			*/
			init_var_from_num(num, &x);

			to_return = get_str_from_var(&x);
		}
        // END COPY
		free_var(&x);
        restore_modifired_numericc(numericc, &numericc_modified);
    }
    // it is a marked null
    else
        to_return = psprintf("NULL:%u", MN_NUMERICC_GET_ID(numericc));

    PG_RETURN_CSTRING(to_return);
}

PG_FUNCTION_INFO_V1(mn_numericc_recv);
Datum mn_numericc_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo) PG_GETARG_POINTER(0);

    Numericc to_return;

    char is_const = pq_getmsgbyte(buf);

    if (is_const)
    {
        // COPY BEGIN src/backend/utils/adt/numeric.c -> Datum numeric_recv(PG_FUNCTION_ARGS)
		int32		typmod = PG_GETARG_INT32(2);
		NumericVar	value;
		Numeric		res;
		int			len,
					i;

		init_var(&value);

		len = (uint16) pq_getmsgint(buf, sizeof(uint16));

		alloc_var(&value, len);

		value.weight = (int16) pq_getmsgint(buf, sizeof(int16));
		/* we allow any int16 for weight --- OK? */

		value.sign = (uint16) pq_getmsgint(buf, sizeof(uint16));
		if (!(value.sign == NUMERIC_POS ||
			value.sign == NUMERIC_NEG ||
			value.sign == NUMERIC_NAN ||
			value.sign == NUMERIC_PINF ||
			value.sign == NUMERIC_NINF))
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_BINARY_REPRESENTATION),
					errmsg("invalid sign in external \"numeric\" value")));

		value.dscale = (uint16) pq_getmsgint(buf, sizeof(uint16));
		if ((value.dscale & NUMERIC_DSCALE_MASK) != value.dscale)
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_BINARY_REPRESENTATION),
					errmsg("invalid scale in external \"numeric\" value")));

		for (i = 0; i < len; i++)
		{
			NumericDigit d = pq_getmsgint(buf, sizeof(NumericDigit));

			if (d < 0 || d >= NBASE)
				ereport(ERROR,
						(errcode(ERRCODE_INVALID_BINARY_REPRESENTATION),
						errmsg("invalid digit in external \"numeric\" value")));
			value.digits[i] = d;
		}

		/*
		* If the given dscale would hide any digits, truncate those digits away.
		* We could alternatively throw an error, but that would take a bunch of
		* extra code (about as much as trunc_var involves), and it might cause
		* client compatibility issues.  Be careful not to apply trunc_var to
		* special values, as it could do the wrong thing; we don't need it
		* anyway, since make_result will ignore all but the sign field.
		*
		* After doing that, be sure to check the typmod restriction.
		*/
		if (value.sign == NUMERIC_POS ||
			value.sign == NUMERIC_NEG)
		{
			trunc_var(&value, value.dscale);

			(void) apply_typmod(&value, typmod, NULL);

			res = make_result(&value);
		}
		else
		{
			/* apply_typmod_special wants us to make the Numeric first */
			res = make_result(&value);

			(void) apply_typmod_special(res, typmod, NULL);
		}

		free_var(&value);
		// COPY END src/backend/utils/adt/numeric.c/numeric_recv(PG_FUNCTION_ARGS)

		to_return = numeric_to_numericc(res);
		pfree(res);
    }
    else
    {
        to_return = id_to_numericc((uint32) pq_getmsgint(buf, 4));
    }

    PG_RETURN_NUMERICC(to_return);
}

PG_FUNCTION_INFO_V1(mn_numericc_send);
Datum mn_numericc_send(PG_FUNCTION_ARGS)
{
	Numericc numericc = PG_GETARG_NUMERICC(0);
    char is_header_modified = 0;
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendbyte(&buf, MN_NUMERICC_GET_FLAG(numericc));

	if (MN_NUMERICC_IS_CONST(numericc))
	{
		Numeric 	num = numericc_to_numeric(numericc, &is_header_modified);

		// COPY BEGIN src/backend/utils/adt/numeric.c/numeric_send(PG_FUNCTION_ARGS)
		NumericVar	x;
		int			i;

		init_var_from_num(num, &x);

		pq_sendint16(&buf, x.ndigits);
		pq_sendint16(&buf, x.weight);
		pq_sendint16(&buf, x.sign);
		pq_sendint16(&buf, x.dscale);
		for (i = 0; i < x.ndigits; i++)
			pq_sendint16(&buf, x.digits[i]);
		// COPY END PostgreSQL 14.2

        restore_modifired_numericc(numericc, &is_header_modified);
	}
	else
		pq_sendint32(&buf, MN_NUMERICC_GET_ID(numericc));

	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

PG_FUNCTION_INFO_V1(mn_numericc_typmod_in);
Datum mn_numericc_typmod_in(PG_FUNCTION_ARGS)
{
	ArrayType  *ta = PG_GETARG_ARRAYTYPE_P(0);
	int32	   *tl;
	int			n;
	int32		typmod;

	tl = ArrayGetIntegerTypmods(ta, &n);

	if (n == 2)
	{
		if (tl[0] < 1 || tl[0] > NUMERIC_MAX_PRECISION)
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("NUMERIC precision %d must be between 1 and %d",
							tl[0], NUMERIC_MAX_PRECISION)));
		if (tl[1] < 0 || tl[1] > tl[0])
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("NUMERIC scale %d must be between 0 and precision %d",
							tl[1], tl[0])));
		typmod = ((tl[0] << 16) | tl[1]) + VARHDRSZ;
	}
	else if (n == 1)
	{
		if (tl[0] < 1 || tl[0] > NUMERIC_MAX_PRECISION)
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("NUMERIC precision %d must be between 1 and %d",
							tl[0], NUMERIC_MAX_PRECISION)));
		/* scale defaults to zero */
		typmod = (tl[0] << 16) + VARHDRSZ;
	}
	else
	{
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("invalid NUMERIC type modifier")));
		typmod = 0;				/* keep compiler quiet */
	}

	PG_RETURN_INT32(typmod);
}

PG_FUNCTION_INFO_V1(mn_numericc_typmod_out);
Datum mn_numericc_typmod_out(PG_FUNCTION_ARGS)
{
	int32		typmod = PG_GETARG_INT32(0);
	char	   *res = (char *) palloc(64);

	if (typmod >= 0)
		snprintf(res, 64, "(%d,%d)",
				 ((typmod - VARHDRSZ) >> 16) & 0xffff,
				 (typmod - VARHDRSZ) & 0xffff);
	else
		*res = '\0';

	PG_RETURN_CSTRING(res);
}

/* Casts */

// TODO: Add the missing explicit cast functions and their SQL CREATEs.
// Implicite casts:
//  INTT        --> NUMERICC
//  NUMERIC     --> NUMERICC
// Explicit casts:
//  INTT        <-- NUMERICC
//  NUMERIC     <-- NUMERICC
//  SMALLINT    <-> NUMERICC [TODO]
//  INT         <-> NUMERICC
//  BIGINT      <-> NUMERICC [TODO]
//  REAL        <-> NUMERICC [TODO]
//  DOUBLE      <-> NUMERICC [TODO]
//  INTT        <-> NUMERIC

// I --> N
PG_FUNCTION_INFO_V1(mn_cast_intt_to_numericc);
Datum mn_cast_intt_to_numericc(PG_FUNCTION_ARGS)
{
    Intt intt = PG_GETARG_INTT(0);
    int32 val = MN_INTT_GET_VALUE(intt);
    
    Numericc to_return;
    Numeric res;
    
    if (MN_INTT_IS_MARKEDNULL(intt))
		PG_RETURN_NULL();

	res = int64_to_numeric((int64)val);
    to_return = numeric_to_numericc(res);

    pfree(res);

    PG_RETURN_NUMERICC(to_return);
}

// n --> N
PG_FUNCTION_INFO_V1(mn_cast_numeric_to_numericc);
Datum mn_cast_numeric_to_numericc(PG_FUNCTION_ARGS)
{
    Numeric num = PG_GETARG_NUMERIC(0);
    Numericc to_return = numeric_to_numericc(num);
    PG_RETURN_NUMERICC(to_return);
}

// N --> I
PG_FUNCTION_INFO_V1(mn_cast_numericc_to_intt);
Datum mn_cast_numericc_to_intt(PG_FUNCTION_ARGS)
{
    Numericc numericc = PG_GETARG_NUMERICC(0);
    char is_header_modified = 0;
    int32 res;
    Numeric num;

    if (MN_NUMERICC_IS_MARKEDNULL(numericc))
        PG_RETURN_NULL();
    
    num = numericc_to_numeric(numericc, &is_header_modified);
    res = numeric_int4_opt_error(num, NULL);
    restore_modifired_numericc(numericc, &is_header_modified);

    PG_RETURN_INTT(int_to_intt(res));
}

// N --> n
PG_FUNCTION_INFO_V1(mn_cast_numericc_to_numeric);
Datum mn_cast_numericc_to_numeric(PG_FUNCTION_ARGS)
{
    Numericc numericc = PG_GETARG_NUMERICC(0);
    Numeric to_return;

    if (MN_NUMERICC_IS_MARKEDNULL(numericc))
        PG_RETURN_NULL();
    
    to_return = numericc_to_numeric_copy(numericc);
    PG_RETURN_NUMERIC(to_return);
}

// I --> n
PG_FUNCTION_INFO_V1(mn_cast_intt_to_numeric);
Datum mn_cast_intt_to_numeric(PG_FUNCTION_ARGS)
{
    Intt intt = PG_GETARG_INTT(0);
    int32 val = MN_INTT_GET_VALUE(intt);
    
    Numeric to_return;
    
    if (MN_INTT_IS_MARKEDNULL(intt))
		PG_RETURN_NULL();

	to_return = int64_to_numeric((int64)val);

    PG_RETURN_NUMERIC(to_return);
}

// n --> I
PG_FUNCTION_INFO_V1(mn_cast_numeric_to_intt);
Datum mn_cast_numeric_to_intt(PG_FUNCTION_ARGS)
{
    Numericc numericc = PG_GETARG_NUMERICC(0);
    char is_header_modified = 0;
    Numeric num;
    int32 to_return;

    if (MN_NUMERICC_IS_MARKEDNULL(numericc))
        PG_RETURN_NULL();
    
    num = numericc_to_numeric(numericc, &is_header_modified);
    to_return = numeric_int4_opt_error(num, NULL);
    restore_modifired_numericc(numericc, &is_header_modified);

    PG_RETURN_INT32(to_return);
}

/* Arithmetic Operators */

// TODO: Add special cases for arithmetic operations
// TODO: Add NI and IN operators and their SQL CREATEs
// Operators: [+, u+, -, u-, *, /, %, ^, u@]
// Types: [NN, NI, IN]

Numericc numericc_binary_operator(Numericc left, Numericc right, int operator)
{
    char     is_left_modified  = 0;
    char     is_right_modified = 0;
    Numeric  l, r, res;
    Numericc to_return;

    l = numericc_to_numeric(left, &is_left_modified);
    r = numericc_to_numeric(right, &is_right_modified);
    switch (operator)
    {
    case 1: // +
        res = numeric_add_opt_error(l, r, NULL); break;
    case 2: // -
        res = numeric_sub_opt_error(l, r, NULL); break;
    case 3: // *
        res = numeric_mul_opt_error(l, r, NULL); break;
    case 4: // /
        res = numeric_div_opt_error(l, r, NULL); break;
    case 5: // %
        res = numeric_mod_opt_error(l, r, NULL); break;
    case 6: // ^
        res = numeric_power_opt(l, r); break;

    default:
        ereport(ERROR,
        (
            errcode(ERRCODE_INTERNAL_ERROR),
            errmsg("Invalid operator between NUMERICC and NUMERICC is being called!")
        ));
        break;
    }
    
    restore_modifired_numericc(left, &is_left_modified);
    restore_modifired_numericc(right, &is_right_modified);

    to_return = numeric_to_numericc(res);
    pfree(res);

    return to_return;
}

// N + N
PG_FUNCTION_INFO_V1(mn_numericc_add_numericc);
Datum mn_numericc_add_numericc(PG_FUNCTION_ARGS)
{
    Numericc left  = PG_GETARG_NUMERICC(0);
    Numericc right = PG_GETARG_NUMERICC(1);

    if (MN_NUMERICC_IS_MARKEDNULL(left) || MN_NUMERICC_IS_MARKEDNULL(right)) 
        PG_RETURN_NULL();

    PG_RETURN_NUMERICC(numericc_binary_operator(left, right, 1));
}

// + N
PG_FUNCTION_INFO_V1(mn_numericc_unary_add);
Datum mn_numericc_unary_add(PG_FUNCTION_ARGS)
{
    Numericc right = PG_GETARG_NUMERICC_COPY(0);

	PG_RETURN_NUMERICC(right);
}

// N - N
PG_FUNCTION_INFO_V1(mn_numericc_substract_numericc);
Datum mn_numericc_substract_numericc(PG_FUNCTION_ARGS)
{
    Numericc left  = PG_GETARG_NUMERICC(0);
    Numericc right = PG_GETARG_NUMERICC(1);

    if (MN_NUMERICC_IS_MARKEDNULL(left) || MN_NUMERICC_IS_MARKEDNULL(right)) 
        PG_RETURN_NULL();

    PG_RETURN_NUMERICC(numericc_binary_operator(left, right, 2));
}

// - N
PG_FUNCTION_INFO_V1(mn_numericc_negation);
Datum mn_numericc_negation(PG_FUNCTION_ARGS)
{
    Numericc right = PG_GETARG_NUMERICC(0);
	Numeric num, res;
    char is_right_modified = 0;
    Numericc to_return;

	if (MN_NUMERICC_IS_MARKEDNULL(right))
		PG_RETURN_NULL();

	num = numericc_to_numeric(right, &is_right_modified);

	if (NUMERIC_IS_NAN(num))
		res = make_result(&const_nan);
		
	res = (Numeric) palloc (VARSIZE(num));
	memcpy(res, num, VARSIZE(num));

	if (NUMERIC_NDIGITS(num) != 0)
	{
		if (NUMERIC_IS_SHORT(num))
			res->choice.n_short.n_header = 
				num->choice.n_short.n_header ^ NUMERIC_SHORT_SIGN_MASK;
		else if (NUMERIC_SIGN(num) == NUMERIC_POS)
			res->choice.n_long.n_sign_dscale = 
				NUMERIC_NEG | NUMERIC_DSCALE(num);
		else
			res->choice.n_long.n_sign_dscale = 
				NUMERIC_POS | NUMERIC_DSCALE(num);
	}

    restore_modifired_numericc(right, &is_right_modified);

    to_return = numeric_to_numericc(res);

    pfree(res);

	PG_RETURN_NUMERICC(to_return);
}

// N * N
PG_FUNCTION_INFO_V1(mn_numericc_multiply_numericc);
Datum mn_numericc_multiply_numericc(PG_FUNCTION_ARGS)
{
    Numericc left  = PG_GETARG_NUMERICC(0);
    Numericc right = PG_GETARG_NUMERICC(1);

    if (MN_NUMERICC_IS_MARKEDNULL(left) || MN_NUMERICC_IS_MARKEDNULL(right)) 
        PG_RETURN_NULL();

    PG_RETURN_NUMERICC(numericc_binary_operator(left, right, 3));
}

// N / N
PG_FUNCTION_INFO_V1(mn_numericc_divide_numericc);
Datum mn_numericc_divide_numericc(PG_FUNCTION_ARGS)
{
    Numericc left  = PG_GETARG_NUMERICC(0);
    Numericc right = PG_GETARG_NUMERICC(1);

    if (MN_NUMERICC_IS_MARKEDNULL(left) || MN_NUMERICC_IS_MARKEDNULL(right)) 
        PG_RETURN_NULL();

    PG_RETURN_NUMERICC(numericc_binary_operator(left, right, 4));
}

// N % N
PG_FUNCTION_INFO_V1(mn_numericc_modulo_numericc);
Datum mn_numericc_modulo_numericc(PG_FUNCTION_ARGS)
{
    Numericc left  = PG_GETARG_NUMERICC(0);
    Numericc right = PG_GETARG_NUMERICC(1);

    if (MN_NUMERICC_IS_MARKEDNULL(left) || MN_NUMERICC_IS_MARKEDNULL(right)) 
        PG_RETURN_NULL();

    PG_RETURN_NUMERICC(numericc_binary_operator(left, right, 5));
}

Numeric numeric_power_opt(Numeric num1, Numeric num2)
{
	Numeric		res;
	NumericVar	arg1;
	NumericVar	arg2;
	NumericVar	result;
	int			sign1,
				sign2;

	/*
	 * Handle NaN and infinities
	 */
	if (NUMERIC_IS_SPECIAL(num1) || NUMERIC_IS_SPECIAL(num2))
	{
		/*
		 * We follow the POSIX spec for pow(3), which says that NaN ^ 0 = 1,
		 * and 1 ^ NaN = 1, while all other cases with NaN inputs yield NaN
		 * (with no error).
		 */
		if (NUMERIC_IS_NAN(num1))
		{
			if (!NUMERIC_IS_SPECIAL(num2))
			{
				init_var_from_num(num2, &arg2);
				if (cmp_var(&arg2, &const_zero) == 0)
					res = (make_result(&const_one));
			}
			res = (make_result(&const_nan));
		}
		if (NUMERIC_IS_NAN(num2))
		{
			if (!NUMERIC_IS_SPECIAL(num1))
			{
				init_var_from_num(num1, &arg1);
				if (cmp_var(&arg1, &const_one) == 0)
					res = (make_result(&const_one));
			}
			res = (make_result(&const_nan));
		}
		/* At least one input is infinite, but error rules still apply */
		sign1 = numeric_sign_internal(num1);
		sign2 = numeric_sign_internal(num2);
		if (sign1 == 0 && sign2 < 0)
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_ARGUMENT_FOR_POWER_FUNCTION),
					 errmsg("zero raised to a negative power is undefined")));
		if (sign1 < 0 && !numeric_is_integral(num2))
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_ARGUMENT_FOR_POWER_FUNCTION),
					 errmsg("a negative number raised to a non-integer power yields a complex result")));

		/*
		 * POSIX gives this series of rules for pow(3) with infinite inputs:
		 *
		 * For any value of y, if x is +1, 1.0 shall be returned.
		 */
		if (!NUMERIC_IS_SPECIAL(num1))
		{
			init_var_from_num(num1, &arg1);
			if (cmp_var(&arg1, &const_one) == 0)
				res = (make_result(&const_one));
		}

		/*
		 * For any value of x, if y is [-]0, 1.0 shall be returned.
		 */
		if (sign2 == 0)
			res = (make_result(&const_one));

		/*
		 * For any odd integer value of y > 0, if x is [-]0, [-]0 shall be
		 * returned.  For y > 0 and not an odd integer, if x is [-]0, +0 shall
		 * be returned.  (Since we don't deal in minus zero, we need not
		 * distinguish these two cases.)
		 */
		if (sign1 == 0 && sign2 > 0)
			res = (make_result(&const_zero));

		/*
		 * If x is -1, and y is [-]Inf, 1.0 shall be returned.
		 *
		 * For |x| < 1, if y is -Inf, +Inf shall be returned.
		 *
		 * For |x| > 1, if y is -Inf, +0 shall be returned.
		 *
		 * For |x| < 1, if y is +Inf, +0 shall be returned.
		 *
		 * For |x| > 1, if y is +Inf, +Inf shall be returned.
		 */
		if (NUMERIC_IS_INF(num2))
		{
			bool		abs_x_gt_one;

			if (NUMERIC_IS_SPECIAL(num1))
				abs_x_gt_one = true;	/* x is either Inf or -Inf */
			else
			{
				init_var_from_num(num1, &arg1);
				if (cmp_var(&arg1, &const_minus_one) == 0)
					res = (make_result(&const_one));
				arg1.sign = NUMERIC_POS;	/* now arg1 = abs(x) */
				abs_x_gt_one = (cmp_var(&arg1, &const_one) > 0);
			}
			if (abs_x_gt_one == (sign2 > 0))
				res = (make_result(&const_pinf));
			else
				res = (make_result(&const_zero));
		}

		/*
		 * For y < 0, if x is +Inf, +0 shall be returned.
		 *
		 * For y > 0, if x is +Inf, +Inf shall be returned.
		 */
		if (NUMERIC_IS_PINF(num1))
		{
			if (sign2 > 0)
				res = (make_result(&const_pinf));
			else
				res = (make_result(&const_zero));
		}

		Assert(NUMERIC_IS_NINF(num1));

		/*
		 * For y an odd integer < 0, if x is -Inf, -0 shall be returned.  For
		 * y < 0 and not an odd integer, if x is -Inf, +0 shall be returned.
		 * (Again, we need not distinguish these two cases.)
		 */
		if (sign2 < 0)
			res = (make_result(&const_zero));

		/*
		 * For y an odd integer > 0, if x is -Inf, -Inf shall be returned. For
		 * y > 0 and not an odd integer, if x is -Inf, +Inf shall be returned.
		 */
		init_var_from_num(num2, &arg2);
		if (arg2.ndigits > 0 && arg2.ndigits == arg2.weight + 1 &&
			(arg2.digits[arg2.ndigits - 1] & 1))
			res = (make_result(&const_ninf));
		else
			res = (make_result(&const_pinf));
	}

	/*
	 * The SQL spec requires that we emit a particular SQLSTATE error code for
	 * certain error conditions.  Specifically, we don't return a
	 * divide-by-zero error code for 0 ^ -1.  Raising a negative number to a
	 * non-integer power must produce the same error code, but that case is
	 * handled in power_var().
	 */
	sign1 = numeric_sign_internal(num1);
	sign2 = numeric_sign_internal(num2);

	if (sign1 == 0 && sign2 < 0)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_ARGUMENT_FOR_POWER_FUNCTION),
				 errmsg("zero raised to a negative power is undefined")));

	/*
	 * Initialize things
	 */
	init_var(&result);
	init_var_from_num(num1, &arg1);
	init_var_from_num(num2, &arg2);

	/*
	 * Call power_var() to compute and return the result; note it handles
	 * scale selection itself.
	 */
	power_var(&arg1, &arg2, &result);

	res = make_result(&result);

	free_var(&result);

	return res;
}

// N ^ N
PG_FUNCTION_INFO_V1(mn_numericc_power_numericc);
Datum mn_numericc_power_numericc(PG_FUNCTION_ARGS)
{
    Numericc left  = PG_GETARG_NUMERICC(0);
    Numericc right = PG_GETARG_NUMERICC(1);

    if (MN_NUMERICC_IS_MARKEDNULL(left) || MN_NUMERICC_IS_MARKEDNULL(right)) 
        PG_RETURN_NULL();

    PG_RETURN_NUMERICC(numericc_binary_operator(left, right, 6));
}

Numeric numeric_abs_opt(Numeric num)
{
	Numeric		res;

	/*
	 * Do it the easy way directly on the packed format
	 */
	res = duplicate_numeric(num);

	if (NUMERIC_IS_SHORT(num))
		res->choice.n_short.n_header =
			num->choice.n_short.n_header & ~NUMERIC_SHORT_SIGN_MASK;
	else if (NUMERIC_IS_SPECIAL(num))
	{
		/* This changes -Inf to Inf, and doesn't affect NaN */
		res->choice.n_short.n_header =
			num->choice.n_short.n_header & ~NUMERIC_INF_SIGN_MASK;
	}
	else
		res->choice.n_long.n_sign_dscale = NUMERIC_POS | NUMERIC_DSCALE(num);

	return (res);
}

// @ N
PG_FUNCTION_INFO_V1(mn_numericc_absolute);
Datum mn_numericc_absolute(PG_FUNCTION_ARGS)
{
    Numericc right = PG_GETARG_NUMERICC(0);
    char is_right_modified = 0;
    Numeric num, res;
    Numericc to_return;

    num = numericc_to_numeric(right, &is_right_modified);
    res = numeric_abs_opt(num);

    restore_modifired_numericc(right, &is_right_modified);

    to_return = numeric_to_numericc(res);
    pfree(res);

    PG_RETURN_NUMERICC(to_return);
}

/* Comparisons */

static int mn_numericc_cmp_consts(Numericc left, Numericc right)
{
	Numeric l = numericc_to_numeric_copy(left);
	Numeric r = numericc_to_numeric_copy(right);

	int to_return = cmp_numerics(l, r);

	pfree(l);
	pfree(r);

	return to_return;
}

static int mn_numericc_cmp_(Numericc left, Numericc right)
{
    if (MN_NUMERICC_IS_CONST(left))
    {
		// val : val
		if (MN_NUMERICC_IS_CONST(right))
			return mn_numericc_cmp_consts(left, right);
		// val > markednull
		else
			return 1;
    }
	else
    {
		//markednull < val
		if (MN_NUMERICC_IS_CONST(right))
			return -1;
		// id : id
		else
			if (MN_NUMERICC_GET_ID(left) > MN_NUMERICC_GET_ID(right))
                return 1;
            else if (MN_NUMERICC_GET_ID(left) < MN_NUMERICC_GET_ID(right))
                return -1;
            else
                return 0;
    }
}

static int mn_numericc_fast_cmp(Datum x, Datum y, SortSupport ssup)
{
	Numericc nx = (Numericc) DatumGetPointer(x);
	Numericc ny = (Numericc) DatumGetPointer(y);
	int result;

	result = mn_numericc_cmp_(nx, ny);

	if ((Pointer) nx != DatumGetPointer(x))
		pfree(nx);
	if ((Pointer) ny != DatumGetPointer(y))
		pfree(ny);

	return result;
}

PG_FUNCTION_INFO_V1(mn_numericc_gt);
Datum mn_numericc_gt(PG_FUNCTION_ARGS)
{
	Numericc left  = PG_GETARG_NUMERICC(0);
	Numericc right = PG_GETARG_NUMERICC(1);

	PG_RETURN_BOOL(mn_numericc_cmp_(left, right) > 0);
}

PG_FUNCTION_INFO_V1(mn_numericc_ge);
Datum mn_numericc_ge(PG_FUNCTION_ARGS)
{
	Numericc left  = PG_GETARG_NUMERICC(0);
	Numericc right = PG_GETARG_NUMERICC(1);

	PG_RETURN_BOOL(mn_numericc_cmp_(left, right) >= 0);
}

PG_FUNCTION_INFO_V1(mn_numericc_lt);
Datum mn_numericc_lt(PG_FUNCTION_ARGS)
{
	Numericc left  = PG_GETARG_NUMERICC(0);
	Numericc right = PG_GETARG_NUMERICC(1);

	PG_RETURN_BOOL(mn_numericc_cmp_(left, right) < 0);
}

PG_FUNCTION_INFO_V1(mn_numericc_le);
Datum mn_numericc_le(PG_FUNCTION_ARGS)
{
	Numericc left  = PG_GETARG_NUMERICC(0);
	Numericc right = PG_GETARG_NUMERICC(1);

	PG_RETURN_BOOL(mn_numericc_cmp_(left, right) <= 0);
}

PG_FUNCTION_INFO_V1(mn_numericc_eq);
Datum mn_numericc_eq(PG_FUNCTION_ARGS)
{
	Numericc left  = PG_GETARG_NUMERICC(0);
	Numericc right = PG_GETARG_NUMERICC(1);

	PG_RETURN_BOOL(mn_numericc_cmp_(left, right) == 0);
}

PG_FUNCTION_INFO_V1(mn_numericc_ne);
Datum mn_numericc_ne(PG_FUNCTION_ARGS)
{
	Numericc left  = PG_GETARG_NUMERICC(0);
	Numericc right = PG_GETARG_NUMERICC(1);

	PG_RETURN_BOOL(mn_numericc_cmp_(left, right) != 0);
}

/* Indexing and Hashing */

PG_FUNCTION_INFO_V1(mn_numericc_cmp);
Datum mn_numericc_cmp(PG_FUNCTION_ARGS)
{
	Numericc left  = PG_GETARG_NUMERICC(0);
	Numericc right = PG_GETARG_NUMERICC(1);

	PG_RETURN_INT32(mn_numericc_cmp_(left, right));
}

PG_FUNCTION_INFO_V1(mn_numericc_sortsupport);
Datum mn_numericc_sortsupport(PG_FUNCTION_ARGS)
{
	SortSupport ssup = (SortSupport) PG_GETARG_POINTER(0);

	ssup->comparator = mn_numericc_fast_cmp;

	PG_RETURN_VOID();
}

PG_FUNCTION_INFO_V1(mn_numericc_inrange);
Datum mn_numericc_inrange(PG_FUNCTION_ARGS)
{
	Numericc vall    = PG_GETARG_NUMERICC(0);
	Numericc basee   = PG_GETARG_NUMERICC(1);
	Numericc offsett = PG_GETARG_NUMERICC(2);
	bool sub         = PG_GETARG_BOOL(3);
	bool less        = PG_GETARG_BOOL(4);
	bool result;
    char is_vall_modified = 0;
    char is_basee_modified = 0;
    char is_offsett_modified = 0;
    
	Numeric val      = numericc_to_numeric(vall, &is_vall_modified);
	Numeric base     = numericc_to_numeric(basee, &is_basee_modified);
	Numeric offset   = numericc_to_numeric(offsett, &is_offsett_modified);

	if (
		MN_NUMERICC_IS_MARKEDNULL(vall) || 
		MN_NUMERICC_IS_MARKEDNULL(basee) ||
		MN_NUMERICC_IS_MARKEDNULL(offsett)
	)
		PG_RETURN_NULL();

	// START COPY
	{
	/*
	 * Reject negative (including -Inf) or NaN offset.  Negative is per spec,
	 * and NaN is because appropriate semantics for that seem non-obvious.
	 */
	if (NUMERIC_IS_NAN(offset) ||
		NUMERIC_IS_NINF(offset) ||
		NUMERIC_SIGN(offset) == NUMERIC_NEG)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PRECEDING_OR_FOLLOWING_SIZE),
				 errmsg("invalid preceding or following size in window function")));

	/*
	 * Deal with cases where val and/or base is NaN, following the rule that
	 * NaN sorts after non-NaN (cf cmp_numerics).  The offset cannot affect
	 * the conclusion.
	 */
	if (NUMERIC_IS_NAN(val))
	{
		if (NUMERIC_IS_NAN(base))
			result = true;		/* NAN = NAN */
		else
			result = !less;		/* NAN > non-NAN */
	}
	else if (NUMERIC_IS_NAN(base))
	{
		result = less;			/* non-NAN < NAN */
	}

	/*
	 * Deal with infinite offset (necessarily +Inf, at this point).
	 */
	else if (NUMERIC_IS_SPECIAL(offset))
	{
		Assert(NUMERIC_IS_PINF(offset));
		if (sub ? NUMERIC_IS_PINF(base) : NUMERIC_IS_NINF(base))
		{
			/*
			 * base +/- offset would produce NaN, so return true for any val
			 * (see in_range_float8_float8() for reasoning).
			 */
			result = true;
		}
		else if (sub)
		{
			/* base - offset must be -inf */
			if (less)
				result = NUMERIC_IS_NINF(val);	/* only -inf is <= sum */
			else
				result = true;	/* any val is >= sum */
		}
		else
		{
			/* base + offset must be +inf */
			if (less)
				result = true;	/* any val is <= sum */
			else
				result = NUMERIC_IS_PINF(val);	/* only +inf is >= sum */
		}
	}

	/*
	 * Deal with cases where val and/or base is infinite.  The offset, being
	 * now known finite, cannot affect the conclusion.
	 */
	else if (NUMERIC_IS_SPECIAL(val))
	{
		if (NUMERIC_IS_PINF(val))
		{
			if (NUMERIC_IS_PINF(base))
				result = true;	/* PINF = PINF */
			else
				result = !less; /* PINF > any other non-NAN */
		}
		else					/* val must be NINF */
		{
			if (NUMERIC_IS_NINF(base))
				result = true;	/* NINF = NINF */
			else
				result = less;	/* NINF < anything else */
		}
	}
	else if (NUMERIC_IS_SPECIAL(base))
	{
		if (NUMERIC_IS_NINF(base))
			result = !less;		/* normal > NINF */
		else
			result = less;		/* normal < PINF */
	}
	else
	{
		/*
		 * Otherwise go ahead and compute base +/- offset.  While it's
		 * possible for this to overflow the numeric format, it's unlikely
		 * enough that we don't take measures to prevent it.
		 */
		NumericVar	valv;
		NumericVar	basev;
		NumericVar	offsetv;
		NumericVar	sum;

		init_var_from_num(val, &valv);
		init_var_from_num(base, &basev);
		init_var_from_num(offset, &offsetv);
		init_var(&sum);

		if (sub)
			sub_var(&basev, &offsetv, &sum);
		else
			add_var(&basev, &offsetv, &sum);

		if (less)
			result = (cmp_var(&valv, &sum) <= 0);
		else
			result = (cmp_var(&valv, &sum) >= 0);

		free_var(&sum);
	}

	PG_FREE_IF_COPY(vall, 0);
	PG_FREE_IF_COPY(basee, 1);
	PG_FREE_IF_COPY(offsett, 2);
	}
	// END COPY

    restore_modifired_numericc(vall, &is_vall_modified);
    restore_modifired_numericc(basee, &is_basee_modified);
    restore_modifired_numericc(offsett, &is_offsett_modified);

	PG_RETURN_BOOL(result);
}

Datum mn_numericc_hash_(Numeric key)
{
	Datum		digit_hash;
	Datum		result;
	int			weight;
	int			start_offset;
	int			end_offset;
	int			i;
	int			hash_len;
	NumericDigit *digits;

	if (NUMERIC_IS_SPECIAL(key))
		result = 0;

	weight = NUMERIC_WEIGHT(key);
	start_offset = 0;
	end_offset = 0;

	digits = NUMERIC_DIGITS(key);
	for (i = 0; i < NUMERIC_NDIGITS(key); i++)
	{
		if (digits[i] != (NumericDigit) 0)
			break;

		start_offset++;

		weight--;
	}

	if (NUMERIC_NDIGITS(key) == start_offset)
		result = -1;

	for (i = NUMERIC_NDIGITS(key) - 1; i >= 0; i--)
	{
		if (digits[i] != (NumericDigit) 0)
			break;

		end_offset++;
	}

	Assert(start_offset + end_offset < NUMERIC_NDIGITS(key));

	hash_len = NUMERIC_NDIGITS(key) - start_offset - end_offset;
	digit_hash = hash_any((unsigned char *) (NUMERIC_DIGITS(key) + start_offset),
						  hash_len * sizeof(NumericDigit));

	result = digit_hash ^ weight;

	return result;
}

Datum mn_numericc_hash_extend_(Numeric key, uint64 seed)
{
	Datum		digit_hash;
	Datum		result;
	int			weight;
	int			start_offset;
	int			end_offset;
	int			i;
	int			hash_len;
	NumericDigit *digits;

	if (NUMERIC_IS_SPECIAL(key))
		result = seed;

	weight = NUMERIC_WEIGHT(key);
	start_offset = 0;
	end_offset = 0;

	digits = NUMERIC_DIGITS(key);
	for (i = 0; i < NUMERIC_NDIGITS(key); i++)
	{
		if (digits[i] != (NumericDigit) 0)
			break;

		start_offset++;

		weight--;
	}

	if (NUMERIC_NDIGITS(key) == start_offset)
		result = seed - 1;

	for (i = NUMERIC_NDIGITS(key) - 1; i >= 0; i--)
	{
		if (digits[i] != (NumericDigit) 0)
			break;

		end_offset++;
	}

	Assert(start_offset + end_offset < NUMERIC_NDIGITS(key));

	hash_len = NUMERIC_NDIGITS(key) - start_offset - end_offset;
	digit_hash = hash_any_extended((unsigned char *) (NUMERIC_DIGITS(key)
													  + start_offset),
								   hash_len * sizeof(NumericDigit),
								   seed);

	result = UInt64GetDatum(DatumGetUInt64(digit_hash) ^ weight);

	return result;
}

PG_FUNCTION_INFO_V1(mn_numericc_hash);
Datum mn_numericc_hash(PG_FUNCTION_ARGS)
{
	Numericc numericc = PG_GETARG_NUMERICC(0);
	char is_header_modified = 0;
    Datum to_return;

    if (MN_NUMERICC_IS_CONST(numericc))
    {
        Numeric num = numericc_to_numeric(numericc, &is_header_modified);
        to_return = mn_numericc_hash_(num);
        restore_modifired_numericc(numericc, &is_header_modified);
        to_return = to_return | 0x80000000;
    }
    else
    {
        uint32 nullid = MN_NUMERICC_GET_FLAG(numericc);
        to_return = hash_any((unsigned char *) &nullid, 4);
        to_return = to_return & 0x7FFFFFFF;
    }

    PG_RETURN_DATUM(to_return);
}

PG_FUNCTION_INFO_V1(mn_numericc_hash_extend);
Datum mn_numericc_hash_extend(PG_FUNCTION_ARGS)
{
	Numericc numericc = PG_GETARG_NUMERICC(0);
    uint64 seed = PG_GETARG_INT64(1);
    char is_header_modified = 0;
    Datum to_return;

    if (MN_NUMERICC_IS_CONST(numericc))
    {
        Numeric num = numericc_to_numeric(numericc, &is_header_modified);
        to_return = mn_numericc_hash_extend_(num, seed);
        restore_modifired_numericc(numericc, &is_header_modified);
        to_return = to_return | 0x80000000;
    }
	else
	{
		uint32 id = MN_NUMERICC_GET_ID(numericc);
        to_return = hash_any_extended((unsigned char *) &id, 4, seed);
        to_return = to_return & 0x7FFFFFFF;
	}

    PG_RETURN_DATUM(to_return);
}

/* Aggregations */

// common aggregation functions

PG_FUNCTION_INFO_V1(mn_numericc_accum_sfunc);
Datum mn_numericc_accum_sfunc(PG_FUNCTION_ARGS)
{
    NumericAggState* state = PG_ARGISNULL(0) ? NULL : (NumericAggState *) PG_GETARG_POINTER(0);
	Numericc this = PG_ARGISNULL(1) ? NULL : PG_GETARG_NUMERICC(1);
    char is_header_modified = 0;
    Numeric num;

	if (state == NULL)
		state = makeNumericAggState(fcinfo, false);

	if (this != NULL && MN_NUMERICC_IS_CONST(this))
    {
        num = numericc_to_numeric(this, &is_header_modified);
		do_numeric_accum(state, num);
        restore_modifired_numericc(this, &is_header_modified);
    }

	PG_RETURN_POINTER(state);
}

PG_FUNCTION_INFO_V1(mn_numericc_accum_cfunc);
Datum mn_numericc_accum_cfunc(PG_FUNCTION_ARGS)
{
	NumericAggState *state1;
	NumericAggState *state2;
	MemoryContext agg_context;
	MemoryContext old_context;

	if (!AggCheckCallContext(fcinfo, &agg_context))
		elog(ERROR, "aggregate function called in non-aggregate context");

	state1 = PG_ARGISNULL(0) ? NULL : (NumericAggState *) PG_GETARG_POINTER(0);
	state2 = PG_ARGISNULL(1) ? NULL : (NumericAggState *) PG_GETARG_POINTER(1);

	if (state2 == NULL)
		PG_RETURN_POINTER(state1);

	/* manually copy all fields from state2 to state1 */
	if (state1 == NULL)
	{
		old_context = MemoryContextSwitchTo(agg_context);

		state1 = makeNumericAggStateCurrentContext(false);
		state1->N = state2->N;
		state1->NaNcount = state2->NaNcount;
		state1->pInfcount = state2->pInfcount;
		state1->nInfcount = state2->nInfcount;
		state1->maxScale = state2->maxScale;
		state1->maxScaleCount = state2->maxScaleCount;

		accum_sum_copy(&state1->sumX, &state2->sumX);

		MemoryContextSwitchTo(old_context);

		PG_RETURN_POINTER(state1);
	}

	state1->N += state2->N;
	state1->NaNcount += state2->NaNcount;
	state1->pInfcount += state2->pInfcount;
	state1->nInfcount += state2->nInfcount;

	if (state2->N > 0)
	{
		/*
		 * These are currently only needed for moving aggregates, but let's do
		 * the right thing anyway...
		 */
		if (state2->maxScale > state1->maxScale)
		{
			state1->maxScale = state2->maxScale;
			state1->maxScaleCount = state2->maxScaleCount;
		}
		else if (state2->maxScale == state1->maxScale)
			state1->maxScaleCount += state2->maxScaleCount;

		/* The rest of this needs to work in the aggregate context */
		old_context = MemoryContextSwitchTo(agg_context);

		/* Accumulate sums */
		accum_sum_combine(&state1->sumX, &state2->sumX);

		MemoryContextSwitchTo(old_context);
	}
	PG_RETURN_POINTER(state1);
}

PG_FUNCTION_INFO_V1(mn_numericc_accum_serialize);
Datum mn_numericc_accum_serialize(PG_FUNCTION_ARGS)
{
	NumericAggState *state;
	StringInfoData buf;
	Datum		temp;
	bytea	   *sumX;
	bytea	   *result;
	NumericVar	tmp_var;

	/* Ensure we disallow calling when not in aggregate context */
	if (!AggCheckCallContext(fcinfo, NULL))
		elog(ERROR, "aggregate function called in non-aggregate context");

	state = (NumericAggState *) PG_GETARG_POINTER(0);

	/*
	 * This is a little wasteful since make_result converts the NumericVar
	 * into a Numeric and numeric_send converts it back again. Is it worth
	 * splitting the tasks in numeric_send into separate functions to stop
	 * this? Doing so would also remove the fmgr call overhead.
	 */
	init_var(&tmp_var);
	accum_sum_final(&state->sumX, &tmp_var);

	temp = DirectFunctionCall1(numeric_send,
							   NumericGetDatum(make_result(&tmp_var)));
	sumX = DatumGetByteaPP(temp);
	free_var(&tmp_var);

	pq_begintypsend(&buf);

	/* N */
	pq_sendint64(&buf, state->N);

	/* sumX */
	pq_sendbytes(&buf, VARDATA_ANY(sumX), VARSIZE_ANY_EXHDR(sumX));

	/* maxScale */
	pq_sendint32(&buf, state->maxScale);

	/* maxScaleCount */
	pq_sendint64(&buf, state->maxScaleCount);

	/* NaNcount */
	pq_sendint64(&buf, state->NaNcount);

	/* pInfcount */
	pq_sendint64(&buf, state->pInfcount);

	/* nInfcount */
	pq_sendint64(&buf, state->nInfcount);

	result = pq_endtypsend(&buf);

	PG_RETURN_BYTEA_P(result);
}

PG_FUNCTION_INFO_V1(mn_numericc_accum_deserialize);
Datum mn_numericc_accum_deserialize(PG_FUNCTION_ARGS)
{
	bytea	   *sstate;
	NumericAggState *result;
	Datum		temp;
	NumericVar	tmp_var;
	StringInfoData buf;

	if (!AggCheckCallContext(fcinfo, NULL))
		elog(ERROR, "aggregate function called in non-aggregate context");

	sstate = PG_GETARG_BYTEA_PP(0);

	/*
	 * Copy the bytea into a StringInfo so that we can "receive" it using the
	 * standard recv-function infrastructure.
	 */
	initStringInfo(&buf);
	appendBinaryStringInfo(&buf,
						   VARDATA_ANY(sstate), VARSIZE_ANY_EXHDR(sstate));

	result = makeNumericAggStateCurrentContext(false);

	/* N */
	result->N = pq_getmsgint64(&buf);

	/* sumX */
	temp = DirectFunctionCall3(numeric_recv,
							   PointerGetDatum(&buf),
							   ObjectIdGetDatum(InvalidOid),
							   Int32GetDatum(-1));
	init_var_from_num(DatumGetNumeric(temp), &tmp_var);
	accum_sum_add(&(result->sumX), &tmp_var);

	/* maxScale */
	result->maxScale = pq_getmsgint(&buf, 4);

	/* maxScaleCount */
	result->maxScaleCount = pq_getmsgint64(&buf);

	/* NaNcount */
	result->NaNcount = pq_getmsgint64(&buf);

	/* pInfcount */
	result->pInfcount = pq_getmsgint64(&buf);

	/* nInfcount */
	result->nInfcount = pq_getmsgint64(&buf);

	pq_getmsgend(&buf);
	pfree(buf.data);

	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(mn_numericc_accum_ifunc);
Datum mn_numericc_accum_ifunc(PG_FUNCTION_ARGS)
{
	NumericAggState* state = PG_ARGISNULL(0) ? NULL : (NumericAggState *) PG_GETARG_POINTER(0);
	Numericc this = PG_ARGISNULL(1) ? NULL : PG_GETARG_NUMERICC(1);
    char is_header_modified = 0;
    bool is_reversed;
    Numeric num;

	if (state == NULL)
		elog(ERROR, "mn_numericc_accum_ifunc called with NULL state");

	if (this != NULL && MN_NUMERICC_IS_CONST(this))
	{
        num = numericc_to_numeric(this, &is_header_modified);
        is_reversed = do_numeric_discard(state, num);
        restore_modifired_numericc(this, &is_header_modified);
		if (!is_reversed)
			PG_RETURN_NULL();
	}

	PG_RETURN_POINTER(state);
}

// COUNT, COUNTT
PG_FUNCTION_INFO_V1(mn_numericc_count_sfunc);
Datum mn_numericc_count_sfunc(PG_FUNCTION_ARGS)
{
    int64 count = PG_GETARG_INT64(0);
    Numericc  this = PG_ARGISNULL(1) ? NULL : PG_GETARG_NUMERICC(1);

    if (this == NULL)
        PG_RETURN_INT64(count);

    if (MN_NUMERICC_IS_CONST(this))
        count += 1;

    PG_RETURN_INT64(count);
}

PG_FUNCTION_INFO_V1(mn_numericc_countt_sfunc);
Datum mn_numericc_countt_sfunc(PG_FUNCTION_ARGS)
{
    int64 count = PG_GETARG_INT64(0);
    Numericc  this = PG_ARGISNULL(1) ? NULL : PG_GETARG_NUMERICC(1);

    if (this == NULL)
        PG_RETURN_INT64(count);
    
    count += 1;
    PG_RETURN_INT64(count);
}

PG_FUNCTION_INFO_V1(mn_numericc_count_cfunc);
Datum mn_numericc_count_cfunc(PG_FUNCTION_ARGS)
{
    PG_RETURN_INT64(PG_GETARG_INT64(0) + PG_GETARG_INT64(1));
}

// SUM
PG_FUNCTION_INFO_V1(mn_numericc_sum_ffunc);
Datum mn_numericc_sum_ffunc(PG_FUNCTION_ARGS)
{
	NumericAggState *state;
	NumericVar	sumX_var;
    Numericc    to_return;

	state = PG_ARGISNULL(0) ? NULL : (NumericAggState *) PG_GETARG_POINTER(0);

	/* If there were no non-null inputs, return NULL */
	if (state == NULL || NA_TOTAL_COUNT(state) == 0)
		PG_RETURN_NULL();

	if (state->NaNcount > 0)	/* there was at least one NaN input */
		PG_RETURN_NUMERIC(make_result(&const_nan));

	/* adding plus and minus infinities gives NaN */
	if (state->pInfcount > 0 && state->nInfcount > 0)
		PG_RETURN_NUMERIC(make_result(&const_nan));
	if (state->pInfcount > 0)
		PG_RETURN_NUMERIC(make_result(&const_pinf));
	if (state->nInfcount > 0)
		PG_RETURN_NUMERIC(make_result(&const_ninf));

	init_var(&sumX_var);
	accum_sum_final(&state->sumX, &sumX_var);
    to_return = numeric_to_numericc(make_result(&sumX_var));

	free_var(&sumX_var);

	PG_RETURN_NUMERICC(to_return);
}

// AVG

PG_FUNCTION_INFO_V1(mn_numericc_avg_ffunc);
Datum mn_numericc_avg_ffunc(PG_FUNCTION_ARGS)
{
	NumericAggState *state;
	Datum		N_datum;
	Datum		sumX_datum;
	NumericVar	sumX_var;
    Numeric     res;
    Numericc    to_return;

	state = PG_ARGISNULL(0) ? NULL : (NumericAggState *) PG_GETARG_POINTER(0);

	/* If there were no non-null inputs, return NULL */
	if (state == NULL || NA_TOTAL_COUNT(state) == 0)
		PG_RETURN_NULL();

	if (state->NaNcount > 0)	/* there was at least one NaN input */
		PG_RETURN_NUMERIC(make_result(&const_nan));

	/* adding plus and minus infinities gives NaN */
	if (state->pInfcount > 0 && state->nInfcount > 0)
		PG_RETURN_NUMERIC(make_result(&const_nan));
	if (state->pInfcount > 0)
		PG_RETURN_NUMERIC(make_result(&const_pinf));
	if (state->nInfcount > 0)
		PG_RETURN_NUMERIC(make_result(&const_ninf));

	N_datum = NumericGetDatum(int64_to_numeric(state->N));

	init_var(&sumX_var);
	accum_sum_final(&state->sumX, &sumX_var);
	sumX_datum = NumericGetDatum(make_result(&sumX_var));
	free_var(&sumX_var);

	res = DatumGetNumeric(DirectFunctionCall2(numeric_div, sumX_datum, N_datum));
    to_return = numeric_to_numericc(res);
    
    pfree(res);
    PG_RETURN_NUMERICC(to_return);
}

PG_FUNCTION_INFO_V1(mn_intt_avg_ffunc);
Datum mn_intt_avg_ffunc(PG_FUNCTION_ARGS)
{
	Intt_avg_state_t* state = PG_ARGISNULL(0) ? NULL : (Intt_avg_state_t*) PG_GETARG_POINTER(0);
    Numeric sum, count, res;
    Numericc to_return;

	if (state == NULL)
		PG_RETURN_NULL();

	sum   = int64_to_numeric(state->sum);
	count = int64_to_numeric(state->count);
    res   = numeric_div_opt_error(sum, count, NULL);
    to_return = numeric_to_numericc(res);

	pfree(sum);
	pfree(count);
    pfree(res);

	PG_RETURN_NUMERICC(to_return);
}

// MAX

PG_FUNCTION_INFO_V1(mn_numericc_max_sfunc);
Datum mn_numericc_max_sfunc(PG_FUNCTION_ARGS)
{
	Numericc max  = PG_ARGISNULL(0) ? NULL : PG_GETARG_NUMERICC(0);
    Numericc this = PG_ARGISNULL(1) ? NULL : PG_GETARG_NUMERICC(1);

	if (max == NULL)
		if (this == NULL || MN_NUMERICC_IS_MARKEDNULL(this))
			PG_RETURN_NULL();
		else
			PG_RETURN_NUMERICC(duplicate_numericc(this));
	else
		if (this == NULL || MN_NUMERICC_IS_MARKEDNULL(this))
			PG_RETURN_NUMERICC(max);
		else
			if (mn_numericc_cmp_(this, max) > 0)
				PG_RETURN_NUMERICC(duplicate_numericc(this));
			else
				PG_RETURN_NUMERICC(max);
}

PG_FUNCTION_INFO_V1(mn_numericc_max_cfunc);
Datum mn_numericc_max_cfunc(PG_FUNCTION_ARGS)
{
	Numericc old = PG_ARGISNULL(0) ? NULL : PG_GETARG_NUMERICC(0);
    Numericc new = PG_ARGISNULL(1) ? NULL : PG_GETARG_NUMERICC(1);

	if (new == NULL)
		PG_RETURN_NUMERICC(old);
	
	if (old == NULL)
	{
		old = duplicate_numericc(new);
		PG_RETURN_NUMERICC(old);
	}

	if (mn_numericc_cmp_(old, new) > 0)
		PG_RETURN_NUMERICC(old);
	else
		PG_RETURN_NUMERICC(new);
}

// MIN

PG_FUNCTION_INFO_V1(mn_numericc_min_sfunc);
Datum mn_numericc_min_sfunc(PG_FUNCTION_ARGS)
{
	Numericc min  = PG_ARGISNULL(0) ? NULL : PG_GETARG_NUMERICC(0);
    Numericc this = PG_ARGISNULL(1) ? NULL : PG_GETARG_NUMERICC(1);

	if (min == NULL)
		if (this == NULL || MN_NUMERICC_IS_MARKEDNULL(this))
			PG_RETURN_NULL();
		else
			PG_RETURN_NUMERICC(duplicate_numericc(this));
	else
		if (this == NULL || MN_NUMERICC_IS_MARKEDNULL(this))
			PG_RETURN_NUMERICC(min);
		else
			if (mn_numericc_cmp_(this, min) < 0)
				PG_RETURN_NUMERICC(duplicate_numericc(this));
			else
				PG_RETURN_NUMERICC(min);
}

PG_FUNCTION_INFO_V1(mn_numericc_min_cfunc);
Datum mn_numericc_min_cfunc(PG_FUNCTION_ARGS)
{
	Numericc old = PG_ARGISNULL(0) ? NULL : PG_GETARG_NUMERICC(0);
    Numericc new = PG_ARGISNULL(1) ? NULL : PG_GETARG_NUMERICC(1);

	if (new == NULL)
		PG_RETURN_NUMERICC(old);
	
	if (old == NULL)
	{
		old = duplicate_numericc(new);
		PG_RETURN_NUMERICC(old);
	}

	if (mn_numericc_cmp_(old, new) < 0)
		PG_RETURN_NUMERICC(old);
	else
		PG_RETURN_NUMERICC(new);
}


/* Additional Functions */

PG_FUNCTION_INFO_V1(mn_numericc_is_const);
Datum mn_numericc_is_const(PG_FUNCTION_ARGS)
{
    Numericc right = PG_GETARG_NUMERICC(0);
    PG_RETURN_BOOL(MN_NUMERICC_IS_CONST(right));
}

PG_FUNCTION_INFO_V1(mn_numericc_is_marked_null);
Datum mn_numericc_is_marked_null(PG_FUNCTION_ARGS)
{
    Numericc right = PG_GETARG_NUMERICC(0);
    PG_RETURN_BOOL(MN_NUMERICC_IS_MARKEDNULL(right));
}

PG_FUNCTION_INFO_V1(mn_numericc_nullid);
Datum mn_numericc_nullid(PG_FUNCTION_ARGS)
{
    Numericc numericc = PG_GETARG_NUMERICC(0);

    if (MN_NUMERICC_IS_CONST(numericc))
        PG_RETURN_NULL();
    
    PG_RETURN_INT64((int64)MN_NUMERICC_GET_ID(numericc));
}

