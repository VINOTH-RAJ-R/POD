/************************************************************************************
 * /src/datee.c
 * C functions for datee (marked date) data type.
 * 
 * Author:  Guozhi Wang
 ************************************************************************************/

#include "markednull.h"
#include "datee.h"

PG_MODULE_MAGIC;

/************************************************************************************
 * Text I/O functions
 ************************************************************************************/

PG_FUNCTION_INFO_V1(mn_datee_input);
Datum mn_datee_input(PG_FUNCTION_ARGS)
{
    char*  str = PG_GETARG_CSTRING(0);
    Node*  escontext = fcinfo->context;
    uint32 null_id;

    // input is a marked null
    if (mn_is_str_marked_null(str))
    {
        //get the string after the colon
        str += 5;

        null_id = mn_str_to_null_identifier(str);
        if (null_id == 0)
            ereport(ERROR,
            (
                errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                errmsg(DATEE_ERROR_MSG)
            ));
        
        PG_RETURN_DATEE(id_to_datee(null_id));
    }
    // input is a constant
    else
        PG_RETURN_DATEE(date_to_datee(str_to_date(str, escontext)));
}

PG_FUNCTION_INFO_V1(mn_datee_output);
Datum mn_datee_output(PG_FUNCTION_ARGS)
{
    Datee datee = PG_GETARG_DATEE(0);
    char*    to_return;

    // it is a constant
    if (datee->is_const)
        to_return = date_to_str(MN_DATEE_GET_VALUE(datee));
    // it is a marked null
    else
        to_return = psprintf("NULL:%u", MN_DATEE_GET_ID(datee));

    PG_RETURN_CSTRING(to_return);
}

// TODO: Binary I/O

// Date/Time Mathematical Operators

PG_FUNCTION_INFO_V1(mn_datee_add_intt);
Datum mn_datee_add_intt(PG_FUNCTION_ARGS)
{
    Datee left = PG_GETARG_DATEE(0);
    Intt right = PG_GETARG_INTT(1);
    DateADT res;

    if (unlikely (MN_INTT_IS_CONST(right) && MN_INTT_GET_VALUE(right) == 0))
        PG_RETURN_DATEE(dup_datee(left));
    
    if (MN_DATEE_IS_MARKEDNULL(left) || MN_INTT_IS_MARKEDNULL(right))
        PG_RETURN_NULL();

    res = MN_DATEE_GET_VALUE(left) + MN_INTT_GET_VALUE(right);

    PG_RETURN_DATEE(date_to_datee(res));
}

PG_FUNCTION_INFO_V1(mn_intt_add_datee);
Datum mn_intt_add_datee(PG_FUNCTION_ARGS)
{
    Intt left   = PG_GETARG_INTT(0);
    Datee right = PG_GETARG_DATEE(1);
    DateADT res;

    if (unlikely (MN_INTT_IS_CONST(left) && MN_INTT_GET_VALUE(left) == 0))
        PG_RETURN_DATEE(dup_datee(right));
    
    if (MN_INTT_IS_MARKEDNULL(left) || MN_DATEE_IS_MARKEDNULL(right))
        PG_RETURN_NULL();

    res = MN_INTT_GET_VALUE(left) + MN_DATEE_GET_VALUE(right);

    PG_RETURN_DATEE(date_to_datee(res));
}

PG_FUNCTION_INFO_V1(mn_datee_minus_intt);
Datum mn_datee_minus_intt(PG_FUNCTION_ARGS)
{
    Datee left = PG_GETARG_DATEE(0);
    Intt right = PG_GETARG_INTT(1);
    DateADT res;

    if (unlikely (MN_INTT_IS_CONST(right) && MN_INTT_GET_VALUE(right) == 0))
        PG_RETURN_DATEE(dup_datee(left));
    
    if (MN_DATEE_IS_MARKEDNULL(left) || MN_INTT_IS_MARKEDNULL(right))
        PG_RETURN_NULL();

    res = MN_DATEE_GET_VALUE(left) - MN_INTT_GET_VALUE(right);

    PG_RETURN_DATEE(date_to_datee(res));
}

PG_FUNCTION_INFO_V1(mn_datee_difference);
Datum mn_datee_difference(PG_FUNCTION_ARGS)
{
    Datee left  = PG_GETARG_DATEE(0);
    Datee right = PG_GETARG_DATEE(1);
    int32 res;

    if (MN_DATEE_IS_MARKEDNULL(left) || MN_DATEE_IS_MARKEDNULL(right))
    {
        if (unlikely (MN_DATEE_GET_ID(left) == MN_DATEE_GET_ID(right)))
            PG_RETURN_INTT(int_to_intt(0));
        else
            PG_RETURN_NULL();
    }

    res = MN_DATEE_GET_VALUE(left) - MN_DATEE_GET_VALUE(right);
    PG_RETURN_DATEE(int_to_intt(res));
}

/************************************************************************************
 * Comparison operators
 ************************************************************************************/

static char mn_datee_cmp_indexing_ (Datee left, Datee right)
{
    if (MN_DATEE_IS_CONST(left))
        if (MN_DATEE_IS_CONST(right)) // val : val
            if (MN_DATEE_GET_VALUE(left) > MN_DATEE_GET_VALUE(right))
                return 1;
            else if (MN_DATEE_GET_VALUE(left) < MN_DATEE_GET_VALUE(right))
                return -1;
            else
                return 0;
        else // val > markednull
            return 1;
    else
        if (MN_DATEE_IS_CONST(right)) // markednull < val
            return -1;
        else // id : id
             if (MN_DATEE_GET_ID(left) > MN_DATEE_GET_ID(right))
                return 1;
            else if (MN_DATEE_GET_ID(left) < MN_DATEE_GET_ID(right))
                return -1;
            else
                return 0;
}

static int mn_datee_fast_cmp_ (Datum x, Datum y, SortSupport ssup)
{
    Datee ix = (Datee) DatumGetPointer(x);
    Datee iy = (Datee) DatumGetPointer(y);

    if (MN_DATEE_GET_FLAG(ix) == MN_DATEE_GET_FLAG(iy))
        return MN_DATEE_GET_VALUE(ix) - MN_DATEE_GET_VALUE(iy);
    else
        return MN_DATEE_GET_FLAG(ix) - MN_DATEE_GET_FLAG(iy);
}

PG_FUNCTION_INFO_V1(mn_datee_gt_indexing);
Datum mn_datee_gt_indexing(PG_FUNCTION_ARGS)
{
    Datee left  = PG_GETARG_DATEE(0);
    Datee right = PG_GETARG_DATEE(1);

    PG_RETURN_BOOL(mn_datee_cmp_indexing_(left, right) > 0);
}

PG_FUNCTION_INFO_V1(mn_datee_ge_indexing);
Datum mn_datee_ge_indexing(PG_FUNCTION_ARGS)
{
    Datee left  = PG_GETARG_DATEE(0);
    Datee right = PG_GETARG_DATEE(1);

    PG_RETURN_BOOL(mn_datee_cmp_indexing_(left, right) >= 0);
}

PG_FUNCTION_INFO_V1(mn_datee_lt_indexing);
Datum mn_datee_lt_indexing(PG_FUNCTION_ARGS)
{
    Datee left  = PG_GETARG_DATEE(0);
    Datee right = PG_GETARG_DATEE(1);

    PG_RETURN_BOOL(mn_datee_cmp_indexing_(left, right) < 0);
}

PG_FUNCTION_INFO_V1(mn_datee_le_indexing);
Datum mn_datee_le_indexing(PG_FUNCTION_ARGS)
{
    Datee left  = PG_GETARG_DATEE(0);
    Datee right = PG_GETARG_DATEE(1);

    PG_RETURN_BOOL(mn_datee_cmp_indexing_(left, right) <= 0);
}

PG_FUNCTION_INFO_V1(mn_datee_eq_indexing);
Datum mn_datee_eq_indexing(PG_FUNCTION_ARGS)
{
    Datee left  = PG_GETARG_DATEE(0);
    Datee right = PG_GETARG_DATEE(1);

    PG_RETURN_BOOL(mn_datee_cmp_indexing_(left, right) == 0);
}

PG_FUNCTION_INFO_V1(mn_datee_ne_indexing);
Datum mn_datee_ne_indexing(PG_FUNCTION_ARGS)
{
    Datee left  = PG_GETARG_DATEE(0);
    Datee right = PG_GETARG_DATEE(1);

    PG_RETURN_BOOL(mn_datee_cmp_indexing_(left, right) != 0);
}

PG_FUNCTION_INFO_V1(mn_datee_cmp_indexing);
Datum mn_datee_cmp_indexing(PG_FUNCTION_ARGS)
{
    Datee left  = PG_GETARG_DATEE(0);
    Datee right = PG_GETARG_DATEE(1);

    PG_RETURN_INT32((int32) mn_datee_cmp_indexing_(left, right));
}

PG_FUNCTION_INFO_V1(mn_datee_sortsupport);
Datum mn_datee_sortsupport(PG_FUNCTION_ARGS)
{
    SortSupport ssup = (SortSupport) PG_GETARG_POINTER(0);

    ssup->comparator = mn_datee_fast_cmp_;

    PG_RETURN_VOID();
}

PG_FUNCTION_INFO_V1(mn_datee_gt);
Datum mn_datee_gt(PG_FUNCTION_ARGS)
{
    Datee left  = PG_GETARG_DATEE(0);
    Datee right = PG_GETARG_DATEE(1);
    
    if (MN_DATEE_IS_CONST(left) && MN_DATEE_IS_CONST(right))
        PG_RETURN_BOOL(
            MN_DATEE_GET_VALUE(left) > MN_DATEE_GET_VALUE(right)
        );
    else
        PG_RETURN_NULL();
}

PG_FUNCTION_INFO_V1(mn_datee_ge);
Datum mn_datee_ge(PG_FUNCTION_ARGS)
{
    Datee left  = PG_GETARG_DATEE(0);
    Datee right = PG_GETARG_DATEE(1);
    
    if (MN_DATEE_IS_CONST(left))
        if (MN_DATEE_IS_CONST(right)) // marked constant : marked constant
            PG_RETURN_BOOL(
                MN_DATEE_GET_VALUE(left) >= MN_DATEE_GET_VALUE(right)
            );
        else // marked constant : marked null
            PG_RETURN_NULL();
    else
        if (MN_DATEE_IS_CONST(right)) // marked null : marked constant
            PG_RETURN_NULL();
        else // marked null : marked null
            if (MN_DATEE_GET_ID(left) == MN_DATEE_GET_ID(right))
                PG_RETURN_BOOL(1);
            else
                PG_RETURN_NULL();
}

PG_FUNCTION_INFO_V1(mn_datee_lt);
Datum mn_datee_lt(PG_FUNCTION_ARGS)
{
    Datee left  = PG_GETARG_DATEE(0);
    Datee right = PG_GETARG_DATEE(1);
    
    if (MN_DATEE_IS_CONST(left) && MN_DATEE_IS_CONST(right))
        PG_RETURN_BOOL(
            MN_DATEE_GET_VALUE(left) < MN_DATEE_GET_VALUE(right)
        );
    else
        PG_RETURN_NULL();
}

PG_FUNCTION_INFO_V1(mn_datee_le);
Datum mn_datee_le(PG_FUNCTION_ARGS)
{
    Datee left  = PG_GETARG_DATEE(0);
    Datee right = PG_GETARG_DATEE(1);
    
    if (MN_DATEE_IS_CONST(left))
        if (MN_DATEE_IS_CONST(right)) // marked constant : marked constant
            PG_RETURN_BOOL(
                MN_DATEE_GET_VALUE(left) <= MN_DATEE_GET_VALUE(right)
            );
        else // marked constant : marked null
            PG_RETURN_NULL();
    else
        if (MN_DATEE_IS_CONST(right)) // marked null : marked constant
            PG_RETURN_NULL();
        else // marked null : marked null
            if (MN_DATEE_GET_ID(left) == MN_DATEE_GET_ID(right))
                PG_RETURN_BOOL(1);
            else
                PG_RETURN_NULL();
}

PG_FUNCTION_INFO_V1(mn_datee_eq);
Datum mn_datee_eq(PG_FUNCTION_ARGS)
{
    Datee left  = PG_GETARG_DATEE(0);
    Datee right = PG_GETARG_DATEE(1);
    
    if (MN_DATEE_IS_CONST(left))
        if (MN_DATEE_IS_CONST(right)) // marked constant : marked constant
            PG_RETURN_BOOL(
                MN_DATEE_GET_VALUE(left) == MN_DATEE_GET_VALUE(right)
            );
        else // marked constant : marked null
            PG_RETURN_NULL();
    else
        if (MN_DATEE_IS_CONST(right)) // marked null : marked constant
            PG_RETURN_NULL();
        else // marked null : marked null
            if (MN_DATEE_GET_ID(left) == MN_DATEE_GET_ID(right))
                PG_RETURN_BOOL(1);
            else
                PG_RETURN_NULL();
}

PG_FUNCTION_INFO_V1(mn_datee_ne);
Datum mn_datee_ne(PG_FUNCTION_ARGS)
{
    Datee left  = PG_GETARG_DATEE(0);
    Datee right = PG_GETARG_DATEE(1);
    
    if (MN_DATEE_IS_CONST(left))
        if (MN_DATEE_IS_CONST(right)) // marked constant : marked constant
            PG_RETURN_BOOL(
                MN_DATEE_GET_VALUE(left) != MN_DATEE_GET_VALUE(right)
            );
        else // marked constant : marked null
            PG_RETURN_NULL();
    else
        if (MN_DATEE_IS_CONST(right)) // marked null : marked constant
            PG_RETURN_NULL();
        else // marked null : marked null
            if (MN_DATEE_GET_ID(left) == MN_DATEE_GET_ID(right))
                PG_RETURN_BOOL(0);
            else
                PG_RETURN_NULL();
}

/************************************************************************************
 * Cast functions
 ************************************************************************************/

PG_FUNCTION_INFO_V1(mn_datee_cast_from_date);
Datum mn_datee_cast_from_date(PG_FUNCTION_ARGS)
{
    DateADT date = PG_GETARG_DATEADT(0);
    PG_RETURN_POINTER(date_to_datee(date));
}

PG_FUNCTION_INFO_V1(mn_datee_cast_to_date);
Datum mn_datee_cast_to_date(PG_FUNCTION_ARGS)
{
    Datee datee = PG_GETARG_DATEE(0);

    if (MN_DATEE_IS_MARKEDNULL(datee))
        PG_RETURN_NULL();
    
    PG_RETURN_DATEADT(MN_DATEE_GET_VALUE(datee));
}

/************************************************************************************
 * Aggregate functions
 ************************************************************************************/

PG_FUNCTION_INFO_V1(mn_datee_count_sfunc);
Datum mn_datee_count_sfunc(PG_FUNCTION_ARGS)
{
    int64 count = PG_GETARG_INT64(0);
    Datee this = PG_ARGISNULL(1) ? NULL : PG_GETARG_DATEE(1);

    if (this == NULL)
        PG_RETURN_INT64(count);

    if (MN_DATEE_IS_CONST(this))
        count += 1;

    PG_RETURN_INT64(count);
}

PG_FUNCTION_INFO_V1(mn_datee_countt_sfunc);
Datum mn_datee_countt_sfunc(PG_FUNCTION_ARGS)
{
    int64 count = PG_GETARG_INT64(0);
    Datee this = PG_ARGISNULL(1) ? NULL : PG_GETARG_DATEE(1);

    if (this == NULL)
        PG_RETURN_INT64(count);
    
    count += 1;
    PG_RETURN_INT64(count);
}

PG_FUNCTION_INFO_V1(mn_datee_count_cfunc);
Datum mn_datee_count_cfunc(PG_FUNCTION_ARGS)
{
    PG_RETURN_INT64(PG_GETARG_INT64(0) + PG_GETARG_INT64(1));
}

PG_FUNCTION_INFO_V1(mn_datee_max_sfunc);
Datum mn_datee_max_sfunc(PG_FUNCTION_ARGS)
{
    DateADT max;
    Datee this = PG_ARGISNULL(1) ? NULL : PG_GETARG_DATEE(1);

    if (PG_ARGISNULL(0))
        if (this == NULL || MN_DATEE_IS_MARKEDNULL(this))
            PG_RETURN_NULL();
        else
            PG_RETURN_DATEADT(MN_DATEE_GET_VALUE(this));
    else
    {
        max = PG_GETARG_DATEADT(0);
        if (this == NULL || MN_DATEE_IS_MARKEDNULL(this))
            PG_RETURN_DATEADT(max);
        else
            PG_RETURN_DATEADT(MN_DATEE_GET_VALUE(this) > max ? MN_DATEE_GET_VALUE(this) : max);
    }
}

PG_FUNCTION_INFO_V1(mn_datee_max_cfunc);
Datum mn_datee_max_cfunc(PG_FUNCTION_ARGS)
{
	int left  = PG_GETARG_INT32(0);
    int right = PG_GETARG_INT32(1);
    PG_RETURN_INT32(left > right ? left : right);
}

PG_FUNCTION_INFO_V1(mn_datee_min_sfunc);
Datum mn_datee_min_sfunc(PG_FUNCTION_ARGS)
{
    DateADT min;
    Datee this = PG_ARGISNULL(1) ? NULL : PG_GETARG_DATEE(1);

    if (PG_ARGISNULL(0))
        if (this == NULL || MN_DATEE_IS_MARKEDNULL(this))
            PG_RETURN_NULL();
        else
            PG_RETURN_DATEADT(MN_DATEE_GET_VALUE(this));
    else
    {
        min = PG_GETARG_DATEADT(0);
        if (this == NULL || MN_DATEE_IS_MARKEDNULL(this))
            PG_RETURN_DATEADT(min);
        else
            PG_RETURN_DATEADT(MN_DATEE_GET_VALUE(this) < min ? MN_DATEE_GET_VALUE(this) : min);
    }
}

PG_FUNCTION_INFO_V1(mn_datee_min_cfunc);
Datum mn_datee_min_cfunc(PG_FUNCTION_ARGS)
{
	int left  = PG_GETARG_INT32(0);
    int right = PG_GETARG_INT32(1);
    PG_RETURN_DATEADT(left < right ? left : right);
}

/************************************************************************************
 * Hash Support Functions
 ************************************************************************************/

PG_FUNCTION_INFO_V1(mn_datee_hash);
Datum mn_datee_hash(PG_FUNCTION_ARGS)
{
    Datee date = PG_GETARG_DATEE(0);
    int32 to_return = 0;
    
    if (MN_DATEE_IS_CONST(date))
        to_return = MN_DATEE_GET_VALUE(date) | 0x80000000; // turn the first bit 1
    else
        to_return = MN_DATEE_GET_ID(date) & 0x7FFFFFFF; // turn the first bit 0
    
    PG_RETURN_INT32(to_return);
}

PG_FUNCTION_INFO_V1(mn_datee_hash_extend);
Datum mn_datee_hash_extend(PG_FUNCTION_ARGS)
{
    Datee date = PG_GETARG_DATEE(0);
    uint64 seed = PG_GETARG_INT64(1);
    uint64 to_return = hash_any_extended((unsigned char *) &MN_DATEE_GET_FLAG(date), 5, seed);
    PG_RETURN_UINT64(to_return);
}

/************************************************************************************
 * Additional Functions
 ************************************************************************************/

PG_FUNCTION_INFO_V1(mn_datee_is_const);
Datum mn_datee_is_const(PG_FUNCTION_ARGS)
{
    Datee right = PG_GETARG_DATEE(0);
    PG_RETURN_BOOL(MN_DATEE_IS_CONST(right));
}

PG_FUNCTION_INFO_V1(mn_datee_is_marked_null);
Datum mn_datee_is_marked_null(PG_FUNCTION_ARGS)
{
    Datee right = PG_GETARG_DATEE(0);
    PG_RETURN_BOOL(MN_DATEE_IS_MARKEDNULL(right));
}

PG_FUNCTION_INFO_V1(mn_datee_nullid);
Datum mn_datee_nullid(PG_FUNCTION_ARGS)
{
    Datee datee = PG_GETARG_DATEE(0);

    if (MN_DATEE_IS_CONST(datee))
        PG_RETURN_NULL();
    
    PG_RETURN_INT64((int64)MN_DATEE_GET_ID(datee));
}
