/************************************************************************************
 * /src/intt.c
 * C functions for intt (marked int) data type.
 * 
 * Author:  Guozhi Wang
 ************************************************************************************/

#include "markednull.h"
#include "intt.h"

PG_MODULE_MAGIC;

/************************************************************************************
 * Text I/O functions
 ************************************************************************************/

PG_FUNCTION_INFO_V1(mn_intt_input);
Datum mn_intt_input(PG_FUNCTION_ARGS)
{
    char* str = PG_GETARG_CSTRING(0);
    Intt to_return;

    uint32 null_id;
    int32 value;

    // input is a marked null
    if (mn_is_str_marked_null(str))
    {
        // get the string after the colon.
        str += 5;

        null_id = mn_str_to_null_identifier(str);
		if (null_id == 0)
            ereport(ERROR,
            (
                errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                errmsg(INTT_ERROR_MSG)
            ));

        to_return = id_to_intt(null_id);
    }
    // input is a constant
    else
    {
        if (! mn_is_str_number(str))
            ereport(ERROR,
            (
                errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                errmsg(INTT_ERROR_MSG)
            ));
        sscanf(str, "%d", &value);

        to_return = int_to_intt(value);
    }
    PG_RETURN_INTT(to_return);
}

PG_FUNCTION_INFO_V1(mn_intt_output);
Datum mn_intt_output(PG_FUNCTION_ARGS)
{
    Intt intt = PG_GETARG_INTT(0);
    char* to_return;

    if (MN_INTT_IS_CONST(intt))
        to_return = psprintf("%d", MN_INTT_GET_VALUE(intt));
    else
        to_return = psprintf("NULL:%u", MN_INTT_GET_ID(intt));

    PG_RETURN_CSTRING(to_return);
}

/************************************************************************************
 * Binary I/O functions
 ************************************************************************************/

PG_FUNCTION_INFO_V1(mn_intt_recv);
Datum mn_intt_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo) PG_GETARG_POINTER(0);
    Intt to_return = (Intt) palloc(INTT_LENGTH);
    
    MN_INTT_SET_FLAG(to_return, pq_getmsgbyte(buf));

    if (MN_INTT_IS_CONST(to_return))
        MN_INTT_SET_VALUE(to_return, pq_getmsgint(buf, 4));
    else
        MN_INTT_SET_ID(to_return, pq_getmsgint(buf, 4));

    PG_RETURN_INTT(to_return);
}

PG_FUNCTION_INFO_V1(mn_intt_send);
Datum mn_intt_send(PG_FUNCTION_ARGS)
{
    Intt intt = PG_GETARG_INTT(0);
    StringInfoData buf;

    pq_begintypsend(&buf);
    pq_sendbyte(&buf, MN_INTT_GET_FLAG(intt));
    if (MN_INTT_GET_FLAG(intt))
        pq_sendint32(&buf, MN_INTT_GET_VALUE(intt));
    else
        pq_sendint32(&buf, MN_INTT_GET_ID(intt));
    
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/************************************************************************************
 * Mathematical Operators
 ************************************************************************************/

/************************************* Addition *************************************/

PG_FUNCTION_INFO_V1(mn_intt_add_intt);
Datum mn_intt_add_intt(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);
    int32 res, l, r;

    if (unlikely (MN_INTT_IS_CONST(left) && (MN_INTT_GET_VALUE(left) == 0)))
        PG_RETURN_INTT(dup_intt(right));
    
    if (unlikely (MN_INTT_IS_CONST(right) && (MN_INTT_GET_VALUE(right) == 0)))
        PG_RETURN_INTT(dup_intt(left));

    if (MN_INTT_IS_MARKEDNULL(left) || MN_INTT_IS_MARKEDNULL(right))
        PG_RETURN_NULL();

    l = MN_INTT_GET_VALUE(left);
    r = MN_INTT_GET_VALUE(right);

    if (unlikely(pg_add_s32_overflow(l, r, &res)))
        ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));

    res = l + r;

    PG_RETURN_INTT(int_to_intt(res));
}

PG_FUNCTION_INFO_V1(mn_int_add_intt);
Datum mn_int_add_intt(PG_FUNCTION_ARGS)
{
    int32 left = PG_GETARG_INT32(0);
    Intt right = PG_GETARG_INTT(1);
    int32 res, r;

    if (unlikely (left == 0))
        PG_RETURN_INTT(dup_intt(right));

    if (unlikely (MN_INTT_IS_CONST(right) && (MN_INTT_GET_VALUE(right) == 0)))
        PG_RETURN_INTT(int_to_intt(left));

    if (MN_INTT_IS_MARKEDNULL(right))
        PG_RETURN_NULL();

    r = MN_INTT_GET_VALUE(right);

    if (unlikely(pg_add_s32_overflow(left, r, &res)))
        ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));

    res = left + r;

    PG_RETURN_INTT(int_to_intt(res));
}

PG_FUNCTION_INFO_V1(mn_intt_add_int);
Datum mn_intt_add_int(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    int32 right = PG_GETARG_INT32(1);
    int32 res, l;

    if (unlikely (MN_INTT_IS_CONST(left) && (MN_INTT_GET_VALUE(left) == 0)))
        PG_RETURN_INTT(int_to_intt(right));

    if (unlikely (right == 0))
        PG_RETURN_INTT(dup_intt(left));

    if (MN_INTT_IS_MARKEDNULL(left))
        PG_RETURN_NULL();

    l = MN_INTT_GET_VALUE(left);

    if (unlikely(pg_add_s32_overflow(l, right, &res)))
        ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));

    res = l + right;

    PG_RETURN_INTT(int_to_intt(res));
}

/********************************** Unary Addition **********************************/
PG_FUNCTION_INFO_V1(mn_intt_unary_add);
Datum mn_intt_unary_add(PG_FUNCTION_ARGS)
{
    Intt right = PG_GETARG_INTT(0);

    PG_RETURN_INTT(dup_intt(right));
}

/*********************************** Substraction ***********************************/
PG_FUNCTION_INFO_V1(mn_intt_substract_intt);
Datum mn_intt_substract_intt(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);
    int32 res, l, r;

    if (unlikely (MN_INTT_IS_CONST(right) && (MN_INTT_GET_VALUE(right) == 0)))
        PG_RETURN_INTT(dup_intt(left));

    if (MN_INTT_IS_MARKEDNULL(left) || MN_INTT_IS_MARKEDNULL(right)) {
        if (unlikely (MN_INTT_GET_ID(left) == MN_INTT_GET_ID(right)))
            PG_RETURN_INTT(int_to_intt(0));
        else
            PG_RETURN_NULL();
    }

    l = MN_INTT_GET_VALUE(left);
    r = MN_INTT_GET_VALUE(right);

    if (unlikely(pg_sub_s32_overflow(l, r, &res)))
        ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));

    res = l - r;

    PG_RETURN_INTT(int_to_intt(res));
}

PG_FUNCTION_INFO_V1(mn_int_substract_intt);
Datum mn_int_substract_intt(PG_FUNCTION_ARGS)
{
    int32 left = PG_GETARG_INT32(0);
    Intt right = PG_GETARG_INTT(1);
    int32 res, r;

    if (MN_INTT_IS_MARKEDNULL(right))
        PG_RETURN_NULL();

    r = MN_INTT_GET_VALUE(right);

    if (unlikely(pg_sub_s32_overflow(left, r, &res)))
        ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));

    res = left - r;

    PG_RETURN_INTT(int_to_intt(res));
}

PG_FUNCTION_INFO_V1(mn_intt_substract_int);
Datum mn_intt_substract_int(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    int32 right = PG_GETARG_INT32(1);
    int32 res, l;

    if (unlikely (right == 0))
        PG_RETURN_INTT(dup_intt(left));

    if (MN_INTT_IS_MARKEDNULL(left))
        PG_RETURN_NULL();

    l = MN_INTT_GET_VALUE(left);

    if (unlikely(pg_sub_s32_overflow(l, right, &res)))
        ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));

    res = l - right;

    PG_RETURN_INTT(int_to_intt(res));
}

/************************************* Negation *************************************/
PG_FUNCTION_INFO_V1(mn_intt_negation);
Datum mn_intt_negation(PG_FUNCTION_ARGS)
{
    Intt right = PG_GETARG_INTT(0);
    int32 r;

    if (MN_INTT_IS_MARKEDNULL(right))
        PG_RETURN_NULL();
    
    r = MN_INTT_GET_VALUE(right);

    if (unlikely(r == PG_INT32_MIN))
        ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));
    
    PG_RETURN_INTT(int_to_intt(-r));
}

/********************************** Multiplication **********************************/
PG_FUNCTION_INFO_V1(mn_intt_multiply_intt);
Datum mn_intt_multiply_intt(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);
    int32 res, l, r;

    if (unlikely (
        (MN_INTT_IS_CONST(left) && (MN_INTT_GET_VALUE(left) == 0)) ||
        (MN_INTT_IS_CONST(right) && (MN_INTT_GET_VALUE(right) == 0))
       ))
        PG_RETURN_INTT(int_to_intt(0));
    
    if (unlikely (MN_INTT_IS_CONST(left) && (MN_INTT_GET_VALUE(left) == 1)))
        PG_RETURN_INTT(dup_intt(right));
    
    if (unlikely (MN_INTT_IS_CONST(right) && (MN_INTT_GET_VALUE(right) == 1)))
        PG_RETURN_INTT(dup_intt(left));

    if (MN_INTT_IS_MARKEDNULL(left) || MN_INTT_IS_MARKEDNULL(right))
        PG_RETURN_NULL();

    l = MN_INTT_GET_VALUE(left);
    r = MN_INTT_GET_VALUE(right);

    if (unlikely(pg_mul_s32_overflow(l, r, &res)))
        ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));

    res = l * r;

    PG_RETURN_INTT(int_to_intt(res));
}

PG_FUNCTION_INFO_V1(mn_int_multiply_intt);
Datum mn_int_multiply_intt(PG_FUNCTION_ARGS)
{
    int32 left = PG_GETARG_INT32(0);
    Intt right = PG_GETARG_INTT(1);
    int32 res, r;

    if (unlikely (left == 0))
        PG_RETURN_INTT(int_to_intt(0));
    
    if (unlikely (left == 1))
        PG_RETURN_INTT(dup_intt(right));

    if (MN_INTT_IS_MARKEDNULL(right))
        PG_RETURN_NULL();

    r = MN_INTT_GET_VALUE(right);

    if (unlikely(pg_mul_s32_overflow(left, r, &res)))
        ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));

    res = left * r;

    PG_RETURN_INTT(int_to_intt(res));
}

PG_FUNCTION_INFO_V1(mn_intt_multiply_int);
Datum mn_intt_multiply_int(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    int32 right = PG_GETARG_INT32(1);
    int32 res, l;

    if (unlikely (right == 0))
        PG_RETURN_INTT(int_to_intt(0));
    
    if (unlikely (right == 1))
        PG_RETURN_INTT(dup_intt(left));

    if (MN_INTT_IS_MARKEDNULL(left))
        PG_RETURN_NULL();

    l = MN_INTT_GET_VALUE(left);

    if (unlikely(pg_mul_s32_overflow(l, right, &res)))
        ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));

    res = l * right;

    PG_RETURN_INTT(int_to_intt(res));
}

/************************************* Division *************************************/
PG_FUNCTION_INFO_V1(mn_intt_divide_intt);
Datum mn_intt_divide_intt(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);
    int32 res, l, r;

    if (unlikely (MN_INTT_IS_CONST(left) && (MN_INTT_GET_VALUE(left) == 0)))
        PG_RETURN_INTT(int_to_intt(0));

    if (unlikely (MN_INTT_IS_CONST(right) && (MN_INTT_GET_VALUE(right) == 0)))
        ereport(ERROR,
				(errcode(ERRCODE_DIVISION_BY_ZERO),
				 errmsg("division by zero")));

    if (unlikely (MN_INTT_IS_CONST(right) && (MN_INTT_GET_VALUE(right) == 1)))
        PG_RETURN_INTT(dup_intt(left));

    if (MN_INTT_IS_MARKEDNULL(left) || MN_INTT_IS_MARKEDNULL(right))
        PG_RETURN_NULL();

    l = MN_INTT_GET_VALUE(left);
    r = MN_INTT_GET_VALUE(right);

    if (r == -1)
    {
        if (unlikely(l == PG_INT32_MIN))
			ereport(ERROR,
					(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
					 errmsg("integer out of range")));
		res = -l;
		PG_RETURN_INTT(int_to_intt(res));
    }

    res = l / r;

    PG_RETURN_INTT(int_to_intt(res));
}

PG_FUNCTION_INFO_V1(mn_int_divide_intt);
Datum mn_int_divide_intt(PG_FUNCTION_ARGS)
{
    int32 l = PG_GETARG_INT32(0);
    Intt right = PG_GETARG_INTT(1);
    int32 res, r;

    if (unlikely (MN_INTT_IS_CONST(right) && (MN_INTT_GET_VALUE(right) == 0)))
        ereport(ERROR,
                (errcode(ERRCODE_DIVISION_BY_ZERO),
                    errmsg("division by zero")));
    
    if (unlikely (MN_INTT_IS_CONST(right) && (MN_INTT_GET_VALUE(right) == 1)))
        PG_RETURN_INTT(int_to_intt(l));

    if (MN_INTT_IS_MARKEDNULL(right))
        PG_RETURN_NULL();

    r = MN_INTT_GET_VALUE(right);

    if (r == -1)
    {
        if (unlikely(l == PG_INT32_MIN))
			ereport(ERROR,
					(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
					 errmsg("integer out of range")));
		res = -l;
		PG_RETURN_INT32(res);
    }

    res = l / r;

    PG_RETURN_INTT(int_to_intt(res));
}

PG_FUNCTION_INFO_V1(mn_intt_divide_int);
Datum mn_intt_divide_int(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    int32 r = PG_GETARG_INT32(1);
    int32 res, l;

    if (unlikely (MN_INTT_IS_CONST(left) && (MN_INTT_GET_VALUE(left) == 0)))
        PG_RETURN_INTT(int_to_intt(0));

    if (MN_INTT_IS_MARKEDNULL(left))
        PG_RETURN_NULL();

    l = MN_INTT_GET_VALUE(left);

    if (r == 0)
		ereport(ERROR,
				(errcode(ERRCODE_DIVISION_BY_ZERO),
				 errmsg("division by zero")));

    if (r == -1)
    {
        if (unlikely(l == PG_INT32_MIN))
			ereport(ERROR,
					(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
					 errmsg("integer out of range")));
		res = -l;
		PG_RETURN_INTT(int_to_intt(res));
    }

    res = l / r;

    PG_RETURN_INTT(int_to_intt(res));
}

/************************************* Modulus **************************************/
PG_FUNCTION_INFO_V1(mn_intt_modulo_intt);
Datum mn_intt_modulo_intt(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);

    int32 l, r, res;

    if (unlikely (MN_INTT_IS_CONST(left) && (MN_INTT_GET_VALUE(left) == 0)))
        PG_RETURN_INTT(int_to_intt(0));
    
    if (unlikely (MN_INTT_IS_CONST(right) && (MN_INTT_GET_VALUE(right) == 0)))
        ereport(ERROR,
                (errcode(ERRCODE_DIVISION_BY_ZERO),
                    errmsg("division by zero")));

    if (unlikely ((MN_INTT_IS_CONST(right) && (
            MN_INTT_GET_VALUE(right) == 1 ||
            MN_INTT_GET_VALUE(right) == -1))))
        PG_RETURN_INTT(int_to_intt(0));

    if (MN_INTT_IS_MARKEDNULL(left) || MN_INTT_IS_MARKEDNULL(right))
        PG_RETURN_NULL();

    l = MN_INTT_GET_VALUE(left);
    r = MN_INTT_GET_VALUE(right);
    
    res = l % r;
    PG_RETURN_INTT(int_to_intt(res));
}

PG_FUNCTION_INFO_V1(mn_int_modulo_intt);
Datum mn_int_modulo_intt(PG_FUNCTION_ARGS)
{
    int32 l = PG_GETARG_INT32(0);
    Intt right = PG_GETARG_INTT(1);
    int32 r, res;

    if (unlikely (MN_INTT_IS_CONST(right) && (MN_INTT_GET_VALUE(right) == 0)))
        ereport(ERROR,
                (errcode(ERRCODE_DIVISION_BY_ZERO),
                    errmsg("division by zero")));

    if (unlikely ((MN_INTT_IS_CONST(right) && (
            MN_INTT_GET_VALUE(right) == 1 ||
            MN_INTT_GET_VALUE(right) == -1))))
        PG_RETURN_INTT(int_to_intt(0));

    if (MN_INTT_IS_MARKEDNULL(right))
        PG_RETURN_NULL();

    r = MN_INTT_GET_VALUE(right);

    res = l % r;
    PG_RETURN_INTT(int_to_intt(res));
}

PG_FUNCTION_INFO_V1(mn_intt_modulo_int);
Datum mn_intt_modulo_int(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    int32 r = PG_GETARG_INT32(1);
    int32 l, res;

    if (MN_INTT_IS_MARKEDNULL(left))
        PG_RETURN_NULL();

    l = MN_INTT_GET_VALUE(left);

    if (unlikely(r == 0))
        ereport(ERROR,
				(errcode(ERRCODE_DIVISION_BY_ZERO),
				 errmsg("division by zero")));

    if (r == -1)
        PG_RETURN_INTT(int_to_intt(0));
    
    res = l % r;
    PG_RETURN_INTT(int_to_intt(res));
}

/********************************** Absolute Value **********************************/
PG_FUNCTION_INFO_V1(mn_intt_absolute);
Datum mn_intt_absolute(PG_FUNCTION_ARGS)
{
    Intt right = PG_GETARG_INTT(0);
    int32 r, res;

    if (MN_INTT_IS_MARKEDNULL(right))
        PG_RETURN_NULL();

    r = MN_INTT_GET_VALUE(right);

    if (unlikely(r == PG_INT32_MIN))
        ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));

    res = (r < 0) ? -r : r;

    PG_RETURN_INTT(int_to_intt(res));
}

/*********************************** Bitwise AND ************************************/
PG_FUNCTION_INFO_V1(mn_intt_bitwise_and_intt);
Datum mn_intt_bitwise_and_intt(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);
    int32 res;

    if (MN_INTT_IS_MARKEDNULL(left) || MN_INTT_IS_MARKEDNULL(right))
        PG_RETURN_NULL();
    
    res = MN_INTT_GET_VALUE(left) & MN_INTT_GET_VALUE(right);

    PG_RETURN_INTT(int_to_intt(res));
}

/************************************ Bitwise OR ************************************/
PG_FUNCTION_INFO_V1(mn_intt_bitwise_or_intt);
Datum mn_intt_bitwise_or_intt(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);
    int32 res;

    if (MN_INTT_IS_MARKEDNULL(left) || MN_INTT_IS_MARKEDNULL(right))
        PG_RETURN_NULL();
    
    res = MN_INTT_GET_VALUE(left) | MN_INTT_GET_VALUE(right);

    PG_RETURN_INTT(int_to_intt(res));
}

/*********************************** Bitwise XOR ************************************/
PG_FUNCTION_INFO_V1(mn_intt_bitwise_xor_intt);
Datum mn_intt_bitwise_xor_intt(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);
    int32 res;

    if (MN_INTT_IS_MARKEDNULL(left) || MN_INTT_IS_MARKEDNULL(right))
        PG_RETURN_NULL();
    
    res = MN_INTT_GET_VALUE(left) ^ MN_INTT_GET_VALUE(right);

    PG_RETURN_INTT(int_to_intt(res));
}

/*********************************** Bitwise NOT ************************************/
PG_FUNCTION_INFO_V1(mn_intt_bitwise_not);
Datum mn_intt_bitwise_not(PG_FUNCTION_ARGS)
{
    Intt right = PG_GETARG_INTT(0);
    int32 res;

    if (MN_INTT_IS_MARKEDNULL(right))
        PG_RETURN_NULL();

    res = ~MN_INTT_GET_VALUE(right);

    PG_RETURN_INTT(int_to_intt(res));
}

/************************************ Left Shift ************************************/
PG_FUNCTION_INFO_V1(mn_intt_left_shift_intt);
Datum mn_intt_left_shift_intt(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);
    int32 res;

    if (MN_INTT_IS_MARKEDNULL(left) || MN_INTT_IS_MARKEDNULL(right))
        PG_RETURN_NULL();
    
    res = MN_INTT_GET_VALUE(left) << MN_INTT_GET_VALUE(right);

    PG_RETURN_INTT(int_to_intt(res));
}

/*********************************** Right Shift ************************************/
PG_FUNCTION_INFO_V1(mn_intt_right_shift_intt);
Datum mn_intt_right_shift_intt(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);
    int32 res;

    if (MN_INTT_IS_MARKEDNULL(left) || MN_INTT_IS_MARKEDNULL(right))
        PG_RETURN_NULL();
    
    res = MN_INTT_GET_VALUE(left) >> MN_INTT_GET_VALUE(right);
    
    PG_RETURN_INTT(int_to_intt(res));
}

/************************************************************************************
 * Comparison operators
 ************************************************************************************/

// There are two sets of comparison operators: normal comparison operators and fast
// comparison operators.
// This function is called for normal ones: = != <> > >= < <=, sorting and indexing.
// The semantics are:
//      1. Any comparison involving a SQL NULL results in a SQL NULL.
//      2. Two marked nulls sharing an identical null identifier are equal.
//      3. A marked null is less than any constant. 
//      4. A marked null with smaller null identifier is less than another marked null
//         with larger null identifier.
static char mn_intt_cmp_indexing_ (Intt left, Intt right)
{
    if (MN_INTT_IS_CONST(left))
        if (MN_INTT_IS_CONST(right)) // val : val
            if (MN_INTT_GET_VALUE(left) > MN_INTT_GET_VALUE(right))
                return 1;
            else if (MN_INTT_GET_VALUE(left) < MN_INTT_GET_VALUE(right))
                return -1;
            else
                return 0;
        else // val > markednull
            return 1;
    else
        if (MN_INTT_IS_CONST(right)) // markednull < val
            return -1;
        else // id : id
             if (MN_INTT_GET_ID(left) > MN_INTT_GET_ID(right))
                return 1;
            else if (MN_INTT_GET_ID(left) < MN_INTT_GET_ID(right))
                return -1;
            else
                return 0;
}

static int mn_intt_fast_cmp_ (Datum x, Datum y, SortSupport ssup)
{
    Intt ix = (Intt) DatumGetPointer(x);
    Intt iy = (Intt) DatumGetPointer(y);

    if (MN_INTT_GET_FLAG(ix) == MN_INTT_GET_FLAG(iy))
        return MN_INTT_GET_VALUE(ix) - MN_INTT_GET_VALUE(iy);
    else
        return MN_INTT_GET_FLAG(ix) - MN_INTT_GET_FLAG(iy);
}

PG_FUNCTION_INFO_V1(mn_intt_gt_indexing);
Datum mn_intt_gt_indexing(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);

    PG_RETURN_BOOL(mn_intt_cmp_indexing_(left, right) > 0);
}

PG_FUNCTION_INFO_V1(mn_intt_ge_indexing);
Datum mn_intt_ge_indexing(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);

    PG_RETURN_BOOL(mn_intt_cmp_indexing_(left, right) >= 0);
}

PG_FUNCTION_INFO_V1(mn_intt_lt_indexing);
Datum mn_intt_lt_indexing(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);

    PG_RETURN_BOOL(mn_intt_cmp_indexing_(left, right) < 0);
}

PG_FUNCTION_INFO_V1(mn_intt_le_indexing);
Datum mn_intt_le_indexing(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);

    PG_RETURN_BOOL(mn_intt_cmp_indexing_(left, right) <= 0);
}

PG_FUNCTION_INFO_V1(mn_intt_eq_indexing);
Datum mn_intt_eq_indexing(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);

    PG_RETURN_BOOL(mn_intt_cmp_indexing_(left, right) == 0);
}

PG_FUNCTION_INFO_V1(mn_intt_ne_indexing);
Datum mn_intt_ne_indexing(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);

    PG_RETURN_BOOL(mn_intt_cmp_indexing_(left, right) != 0);
}

PG_FUNCTION_INFO_V1(mn_intt_cmp_indexing);
Datum mn_intt_cmp_indexing(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);

    PG_RETURN_INT32((int32) mn_intt_cmp_indexing_(left, right));
}

PG_FUNCTION_INFO_V1(mn_intt_sortsupport);
Datum mn_intt_sortsupport(PG_FUNCTION_ARGS)
{
    SortSupport ssup = (SortSupport) PG_GETARG_POINTER(0);

    ssup->comparator = mn_intt_fast_cmp_;

    PG_RETURN_VOID();
}

PG_FUNCTION_INFO_V1(mn_intt_inrange);
Datum mn_intt_inrange(PG_FUNCTION_ARGS)
{
    Intt vall    = PG_GETARG_INTT(0);
    Intt basee   = PG_GETARG_INTT(1);
    Intt offsett = PG_GETARG_INTT(2);
    bool sub     = PG_GETARG_BOOL(3);
    bool less    = PG_GETARG_BOOL(3);
    int val, base, offset, target;

    if (
        MN_INTT_IS_MARKEDNULL(vall) || 
        MN_INTT_IS_MARKEDNULL(basee) ||
        MN_INTT_IS_MARKEDNULL(offsett)
    )
        PG_RETURN_NULL();
    
    val    = MN_INTT_GET_VALUE(vall);
    base   = MN_INTT_GET_VALUE(basee);
    offset = MN_INTT_GET_VALUE(offsett);

    if (offset)
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_PRECEDING_OR_FOLLOWING_SIZE),
                errmsg("invalid preceding or following size in window function")));

    if (sub)
        target = base - offset;
    else
        target = base + offset;
    
    if (less)
        return val <= target;
    else
        return val >= target;
}

PG_FUNCTION_INFO_V1(mn_intt_gt_fast);
Datum mn_intt_gt_fast(PG_FUNCTION_ARGS)
{
    Intt left  = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);

    if (MN_INTT_IS_CONST(left) && MN_INTT_IS_CONST(right))
        PG_RETURN_BOOL(
            MN_INTT_GET_VALUE(left) > MN_INTT_GET_VALUE(right)
        );
    else if (
        MN_INTT_IS_MARKEDNULL(left) &&
        MN_INTT_IS_MARKEDNULL(right) &&
        MN_INTT_GET_ID(left) == MN_INTT_GET_ID(right)
    )
        PG_RETURN_BOOL(0);
    else
        PG_RETURN_NULL();
}

PG_FUNCTION_INFO_V1(mn_intt_ge_fast);
Datum mn_intt_ge_fast(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);

    if (MN_INTT_IS_CONST(left))
        if (MN_INTT_IS_CONST(right)) // marked constant : marked constant
            PG_RETURN_BOOL(
                MN_INTT_GET_VALUE(left) >= MN_INTT_GET_VALUE(right)
            );
        else // marked constant : marked null
            PG_RETURN_NULL();
    else
        if (MN_INTT_IS_CONST(right)) // marked null : marked constant
            PG_RETURN_NULL();
        else // marked null : marked null
            if (MN_INTT_GET_ID(left) == MN_INTT_GET_ID(right))
                PG_RETURN_BOOL(1);
            else
                PG_RETURN_NULL();
}

PG_FUNCTION_INFO_V1(mn_intt_lt_fast);
Datum mn_intt_lt_fast(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);

    if (MN_INTT_IS_CONST(left) && MN_INTT_IS_CONST(right))
        PG_RETURN_BOOL(
            MN_INTT_GET_VALUE(left) < MN_INTT_GET_VALUE(right)
        );
    else if (
        MN_INTT_IS_MARKEDNULL(left) &&
        MN_INTT_IS_MARKEDNULL(right) &&
        MN_INTT_GET_ID(left) == MN_INTT_GET_ID(right)
    )
        PG_RETURN_BOOL(0);
    else
        PG_RETURN_NULL();
}

PG_FUNCTION_INFO_V1(mn_intt_le_fast);
Datum mn_intt_le_fast(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);

    if (MN_INTT_IS_CONST(left))
        if (MN_INTT_IS_CONST(right)) // marked constant : marked constant
            PG_RETURN_BOOL(
                MN_INTT_GET_VALUE(left) <= MN_INTT_GET_VALUE(right)
            );
        else // marked constant : marked null
            PG_RETURN_NULL();
    else
        if (MN_INTT_IS_CONST(right)) // marked null : marked constant
            PG_RETURN_NULL();
        else // marked null : marked null
            if (MN_INTT_GET_ID(left) == MN_INTT_GET_ID(right))
                PG_RETURN_BOOL(1);
            else
                PG_RETURN_NULL();
}

PG_FUNCTION_INFO_V1(mn_intt_eq_fast);
Datum mn_intt_eq_fast(PG_FUNCTION_ARGS)
{
    Intt left  = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);

    if (MN_INTT_IS_CONST(left))
        if (MN_INTT_IS_CONST(right))
            PG_RETURN_BOOL(MN_INTT_GET_VALUE(left) == MN_INTT_GET_VALUE(right));
        else
            PG_RETURN_NULL();
    else
        if (MN_INTT_IS_CONST(right))
            PG_RETURN_NULL();
        else
            if (MN_INTT_GET_ID(left) == MN_INTT_GET_ID(right))
                PG_RETURN_BOOL(1);
            else
                PG_RETURN_NULL();
}

PG_FUNCTION_INFO_V1(mn_intt_ne_fast);
Datum mn_intt_ne_fast(PG_FUNCTION_ARGS)
{
    Intt left = PG_GETARG_INTT(0);
    Intt right = PG_GETARG_INTT(1);

    if (MN_INTT_IS_CONST(left))
        if (MN_INTT_IS_CONST(right))
            PG_RETURN_BOOL(MN_INTT_GET_VALUE(left) != MN_INTT_GET_VALUE(right));
        else
            PG_RETURN_NULL();
    else
        if (MN_INTT_IS_CONST(right))
            PG_RETURN_NULL();
        else
            if (MN_INTT_GET_ID(left) == MN_INTT_GET_ID(right))
                PG_RETURN_BOOL(0);
            else
                PG_RETURN_NULL();
}

/************************************************************************************
 * Cast functions
 ************************************************************************************/

PG_FUNCTION_INFO_V1(mn_intt_cast_from_int);
Datum mn_intt_cast_from_int(PG_FUNCTION_ARGS)
{
    PG_RETURN_INTT(int_to_intt(PG_GETARG_INT32(0)));
}

PG_FUNCTION_INFO_V1(mn_intt_cast_to_int);
Datum mn_intt_cast_to_int(PG_FUNCTION_ARGS)
{
    Intt intt = PG_GETARG_INTT(0);

    if (MN_INTT_IS_MARKEDNULL(intt))
        PG_RETURN_NULL();
    else
        PG_RETURN_INT32(MN_INTT_GET_VALUE(intt));
}

/************************************************************************************
 * Aggregate functions
 ************************************************************************************/

PG_FUNCTION_INFO_V1(mn_intt_count_sfunc);
Datum mn_intt_count_sfunc(PG_FUNCTION_ARGS)
{
    int64 count = PG_GETARG_INT64(0);
    Intt  this = PG_ARGISNULL(1) ? NULL : PG_GETARG_INTT(1);

    if (this == NULL)
        PG_RETURN_INT64(count);

    if (MN_INTT_IS_CONST(this))
        count += 1;

    PG_RETURN_INT64(count);
}

PG_FUNCTION_INFO_V1(mn_intt_countt_sfunc);
Datum mn_intt_countt_sfunc(PG_FUNCTION_ARGS)
{
    int64 count = PG_GETARG_INT64(0);
    Intt  this = PG_ARGISNULL(1) ? NULL : PG_GETARG_INTT(1);

    if (this == NULL)
        PG_RETURN_INT64(count);
    
    count += 1;
    PG_RETURN_INT64(count);
}

PG_FUNCTION_INFO_V1(mn_intt_count_cfunc);
Datum mn_intt_count_cfunc(PG_FUNCTION_ARGS)
{
    PG_RETURN_INT64(PG_GETARG_INT64(0) + PG_GETARG_INT64(1));
}

PG_FUNCTION_INFO_V1(mn_intt_sum_sfunc);
Datum mn_intt_sum_sfunc(PG_FUNCTION_ARGS)
{
    int64 sum;
    Intt  this = PG_ARGISNULL(1) ? NULL : PG_GETARG_INTT(1);

    if (PG_ARGISNULL(0))
        if (this == NULL || ! this->is_const)
            PG_RETURN_NULL();
        else 
            PG_RETURN_INT64((int64) MN_INTT_GET_VALUE(this));
    else
    {
        sum = PG_GETARG_INT64(0);
        if (this == NULL || ! this->is_const)
            PG_RETURN_INT64(sum);
        else
            PG_RETURN_INT64(sum + MN_INTT_GET_VALUE(this));
    }
}

PG_FUNCTION_INFO_V1(mn_intt_sum_cfunc);
Datum mn_intt_sum_cfunc(PG_FUNCTION_ARGS)
{
    PG_RETURN_INT64(PG_GETARG_INT64(0) + PG_GETARG_INT64(1));
}

PG_FUNCTION_INFO_V1(mn_intt_max_sfunc);
Datum mn_intt_max_sfunc(PG_FUNCTION_ARGS)
{
    int32 max;
    Intt this = PG_ARGISNULL(1) ? NULL : PG_GETARG_INTT(1);

    if (PG_ARGISNULL(0))
        if (this == NULL || MN_INTT_IS_MARKEDNULL(this))
            PG_RETURN_NULL();
        else
            PG_RETURN_INT32(MN_INTT_GET_VALUE(this));
    else
    {
        max = PG_GETARG_INT32(0);
        if (this == NULL || MN_INTT_IS_MARKEDNULL(this))
            PG_RETURN_INT32(max);
        else
            PG_RETURN_INT32(MN_INTT_GET_VALUE(this) > max ? MN_INTT_GET_VALUE(this) : max);
    }
}

PG_FUNCTION_INFO_V1(mn_intt_max_cfunc);
Datum mn_intt_max_cfunc(PG_FUNCTION_ARGS)
{
    int32 left  = (int32) PG_GETARG_INT32(0);
    int32 right = (int32) PG_GETARG_INT32(1);
    PG_RETURN_INT32(left > right ? left : right);
}

PG_FUNCTION_INFO_V1(mn_intt_min_sfunc);
Datum mn_intt_min_sfunc(PG_FUNCTION_ARGS)
{
    int32 min;
    Intt this = PG_ARGISNULL(1) ? NULL : PG_GETARG_INTT(1);

    if (PG_ARGISNULL(0))
        if (this == NULL || MN_INTT_IS_MARKEDNULL(this))
            PG_RETURN_NULL();
        else
            PG_RETURN_INT32(MN_INTT_GET_VALUE(this));
    else
    {
        min = PG_GETARG_INT32(0);
        if (this == NULL || MN_INTT_IS_MARKEDNULL(this))
            PG_RETURN_INT32(min);
        else
            PG_RETURN_INT32(MN_INTT_GET_VALUE(this) < min ? MN_INTT_GET_VALUE(this) : min);
    }
}

PG_FUNCTION_INFO_V1(mn_intt_min_cfunc);
Datum mn_intt_min_cfunc(PG_FUNCTION_ARGS)
{
    int32 left  = (int32) PG_GETARG_INT32(0);
    int32 right = (int32) PG_GETARG_INT32(1);
    PG_RETURN_INT32(left < right ? left : right);
}

PG_FUNCTION_INFO_V1(mn_intt_avg_sfunc);
Datum mn_intt_avg_sfunc(PG_FUNCTION_ARGS)
{
    Intt_avg_state_t* state = PG_ARGISNULL(0) ? NULL : (Intt_avg_state_t*) PG_GETARG_POINTER(0);
    Intt this = PG_ARGISNULL(1) ? NULL : (Intt) PG_GETARG_POINTER(1);

    Intt_avg_state_t* to_return;
    MemoryContext agg_context;
    MemoryContext old_context;

    if (state == NULL)
        if (this == NULL || ! this->is_const)
            PG_RETURN_NULL();
        else
        {
            if (!AggCheckCallContext(fcinfo, &agg_context))
                elog(ERROR, "aggregate function called in non-aggregate context");

            old_context = MemoryContextSwitchTo(agg_context);

            to_return = (Intt_avg_state_t*) palloc0 (sizeof(Intt_avg_state_t));
            to_return->count = 1;
            to_return->sum = (int64) MN_INTT_GET_VALUE(this);

            MemoryContextSwitchTo(old_context);

            PG_RETURN_POINTER(to_return);
        }
    else
        if (this == NULL || ! this->is_const)
            PG_RETURN_POINTER(state);
        else
        {
            state->count += 1;
            state->sum += (int64) MN_INTT_GET_VALUE(this);
            PG_RETURN_POINTER(state);
        }
}

PG_FUNCTION_INFO_V1(mn_intt_avg_cfunc);
Datum mn_intt_avg_cfunc(PG_FUNCTION_ARGS)
{
    Intt_avg_state_t* old = PG_ARGISNULL(0) ? NULL : (Intt_avg_state_t*)PG_GETARG_POINTER(0);
    Intt_avg_state_t* new = PG_ARGISNULL(1) ? NULL : (Intt_avg_state_t*)PG_GETARG_POINTER(1);
    
    // new is null, return old (old can also be null)
    if (new == NULL)
        PG_RETURN_POINTER(old);
    
    // move new to old then return, because the new is not long-lived
    if (old == NULL)
    {
        old = (Intt_avg_state_t*) palloc (sizeof(Intt_avg_state_t));
        old->sum = new->sum;
        old->count = new->count;
        PG_RETURN_POINTER(old);
    }

    // both not null, combine and return
    old->sum += new->sum;
    old->count += new->count;
    PG_RETURN_POINTER(old);
}

PG_FUNCTION_INFO_V1(mn_intt_avg_serialize);
Datum mn_intt_avg_serialize(PG_FUNCTION_ARGS)
{
    Intt_avg_state_t* state = (Intt_avg_state_t*) PG_GETARG_POINTER(0);
    StringInfoData buf;
    bytea *to_return;

    pq_begintypsend(&buf);
    pq_sendint64(&buf, state->sum);
    pq_sendint64(&buf, state->count);
    to_return = pq_endtypsend(&buf);

    PG_RETURN_BYTEA_P(to_return);
}

PG_FUNCTION_INFO_V1(mn_intt_avg_deserialize);
Datum mn_intt_avg_deserialize(PG_FUNCTION_ARGS)
{
    bytea *state_bin = PG_GETARG_BYTEA_PP(0);
    Intt_avg_state_t* to_return = (Intt_avg_state_t*) palloc (sizeof (Intt_avg_state_t));

    StringInfoData buf;
    initStringInfo(&buf);
    appendBinaryStringInfo(&buf, VARDATA_ANY(state_bin), VARSIZE_ANY_EXHDR(state_bin));

    to_return->sum = pq_getmsgint64(&buf);
    to_return->count = pq_getmsgint64(&buf);

    pq_getmsgend(&buf);
    pfree(buf.data);

    PG_RETURN_POINTER(to_return);
}

/************************************************************************************
 * Hash Support Functions
 ************************************************************************************/

PG_FUNCTION_INFO_V1(mn_intt_hash);
Datum mn_intt_hash(PG_FUNCTION_ARGS)
{
    Intt it = (Intt) PG_GETARG_POINTER(0);
    int32 to_return;
    
    if (MN_INTT_IS_CONST(it))
        to_return = MN_INTT_GET_VALUE(it) | 0x80000000; // turn the first bit 1
    else
        to_return = MN_INTT_GET_ID(it) & 0x7FFFFFFF; // turn the first bit 0
    
    PG_RETURN_INT32(to_return);
}

PG_FUNCTION_INFO_V1(mn_intt_hash_extend);
Datum mn_intt_hash_extend(PG_FUNCTION_ARGS)
{
    Intt it = PG_GETARG_INTT(0);
    uint64 seed = PG_GETARG_INT64(1);
    uint64 to_return = hash_any_extended((unsigned char *) &MN_INTT_GET_FLAG(it), 5, seed);
    PG_RETURN_UINT64(to_return);
}

/************************************************************************************
 * Additional Functions
 ************************************************************************************/

PG_FUNCTION_INFO_V1(mn_intt_is_const);
Datum mn_intt_is_const(PG_FUNCTION_ARGS)
{
    Intt right = PG_GETARG_INTT(0);
    PG_RETURN_BOOL(MN_INTT_IS_CONST(right));
}

PG_FUNCTION_INFO_V1(mn_intt_is_marked_null);
Datum mn_intt_is_marked_null(PG_FUNCTION_ARGS)
{
    Intt right = PG_GETARG_INTT(0);
    PG_RETURN_BOOL(MN_INTT_IS_MARKEDNULL(right));
}

PG_FUNCTION_INFO_V1(mn_intt_nullid);
Datum mn_intt_nullid(PG_FUNCTION_ARGS)
{
    Intt intt = PG_GETARG_INTT(0);

    if (MN_INTT_IS_CONST(intt))
        PG_RETURN_NULL();
    
    PG_RETURN_INT64((int64)MN_INTT_GET_ID(intt));
}