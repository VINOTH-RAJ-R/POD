/************************************************************************************
 * /src/textt.c
 * C functions for textt (marked text) data type.
 * 
 * Author:  Guozhi Wang
 ************************************************************************************/

#include "markednull.h"
#include "textt.h"

PG_MODULE_MAGIC;

/************************************************************************************
 * I/O functions
 ************************************************************************************/

PG_FUNCTION_INFO_V1(mn_textt_input);
Datum mn_textt_input(PG_FUNCTION_ARGS)
{
    char*   str = PG_GETARG_CSTRING(0);
    Textt   textt;
    uint32  null_id;

    // input is a marked null
    if (mn_is_str_marked_null(str))
    {
        str += 5;

        null_id = mn_str_to_null_identifier(str);
        if (null_id == 0)
		{
            // ereport(ERROR,
            // (
            //     errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
            //     errmsg(TEXTT_ERROR_MSG)
            // ));
            str -= 5;
            textt = cstring_to_textt(str);
            
        } else {
            textt = id_to_textt(null_id);
        }
    }
    // input is a constant
    else
    {
        textt = cstring_to_textt(str);
    }

    PG_RETURN_TEXTT(textt);
}

PG_FUNCTION_INFO_V1(mn_textt_output);
Datum mn_textt_output(PG_FUNCTION_ARGS)
{
    Textt textt = (Textt) PG_GETARG_POINTER(0);
    char* to_return;

    int length;
    
    if (MN_TEXTT_IS_CONST(textt)) // when it is a constant
    {
        // minus 1 for flag
        length = VARSIZE_ANY_EXHDR(textt) - 1;
        // plus 1 for \0
        to_return = (char*) palloc (length + 1); 
        memcpy(to_return, MN_TEXTT_GET_VALUE(textt), length);
        to_return[length] = '\0';
    }
    else // when it is a marked null
        to_return = psprintf("NULL:%u", MN_TEXTT_GET_ID(textt));

    PG_RETURN_CSTRING(to_return);
}

PG_FUNCTION_INFO_V1(mn_textt_recv);
Datum mn_textt_recv(PG_FUNCTION_ARGS)
{
    StringInfo  buf = (StringInfo) PG_GETARG_POINTER(0);
    Textt       textt;
    char*       str;
    int         string_length;
    uint32      null_id;

    char        const_flag = pq_getmsgbyte(buf);

    if (const_flag)
    {
        string_length = buf->len - buf->cursor;
        textt = build_textt(sizeof(char) + string_length + VARHDRSZ);
        MN_TEXTT_SET_FLAG(textt, 1);
        str = pq_getmsgtext(buf, string_length, NULL);
        memcpy(MN_TEXTT_GET_VALUE(textt), str, string_length);
    }
    else
    {
        null_id = (uint32) pq_getmsgint(buf, sizeof(uint32));
        textt = id_to_textt(null_id);
    }

    PG_RETURN_TEXTT(textt);
}

PG_FUNCTION_INFO_V1(mn_textt_send);
Datum mn_textt_send(PG_FUNCTION_ARGS)
{
    Textt textt = PG_GETARG_TEXTT(0);
    StringInfoData buf;

    pq_begintypsend(&buf);
    pq_sendbyte(&buf, MN_TEXTT_GET_FLAG(textt));
    if (MN_TEXTT_IS_CONST(textt))
        pq_sendtext(&buf, MN_TEXTT_GET_VALUE(textt), VARSIZE_ANY_EXHDR(textt) - 1);
    else
        pq_sendint32(&buf, 888);
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/************************************************************************************
 * String Operators
 ************************************************************************************/

PG_FUNCTION_INFO_V1(mn_textt_concatenate);
Datum mn_textt_concatenate(PG_FUNCTION_ARGS)
{
    Textt left  = PG_GETARG_TEXTT(0);
    Textt right = PG_GETARG_TEXTT(1);
    Textt to_return;
    int left_text_length, right_text_length, internal_length;

    if ( MN_TEXTT_IS_MARKEDNULL(left) || MN_TEXTT_IS_MARKEDNULL(right) )
        PG_RETURN_NULL();

    left_text_length = VARSIZE_ANY_EXHDR(left) - 1;
    right_text_length = VARSIZE_ANY_EXHDR(right) - 1;
    internal_length = sizeof(char) + left_text_length + right_text_length; 

    to_return = (Textt) palloc(internal_length + VARHDRSZ);
    SET_VARSIZE(to_return, internal_length + VARHDRSZ);

    MN_TEXTT_SET_FLAG(to_return, 1);

    memcpy(
        MN_TEXTT_GET_VALUE(to_return),
        MN_TEXTT_GET_VALUE(left),
        left_text_length);
    memcpy(
        MN_TEXTT_GET_VALUE(to_return) + left_text_length,
        MN_TEXTT_GET_VALUE(right),
        right_text_length);

    PG_RETURN_TEXTT(to_return);
}

PG_FUNCTION_INFO_V1(mn_textt_like);
Datum mn_textt_like(PG_FUNCTION_ARGS)
{
    Textt str  = PG_GETARG_TEXTT(0);
    Textt pat  = PG_GETARG_TEXTT(1);

    char* s    = MN_TEXTT_GET_VALUE(str);
    int   slen = VARSIZE_ANY_EXHDR(str) - 1;
    char* p    = MN_TEXTT_GET_VALUE(pat);
    int   plen = VARSIZE_ANY_EXHDR(pat) - 1;

    PG_RETURN_BOOL(GenericMatchText(s, slen, p, plen, PG_GET_COLLATION()) == LIKE_TRUE);
}

PG_FUNCTION_INFO_V1(mn_textt_not_like);
Datum mn_textt_not_like(PG_FUNCTION_ARGS)
{
    Textt str  = PG_GETARG_TEXTT(0);
    Textt pat  = PG_GETARG_TEXTT(1);

    char* s    = MN_TEXTT_GET_VALUE(str);
    int   slen = VARSIZE_ANY_EXHDR(str) - 1;
    char* p    = MN_TEXTT_GET_VALUE(pat);
    int   plen = VARSIZE_ANY_EXHDR(pat) - 1;

    PG_RETURN_BOOL(GenericMatchText(s, slen, p, plen, PG_GET_COLLATION()) != LIKE_TRUE);
}

/************************************************************************************
 * Comparison operators
 ************************************************************************************/

// 1 when left > right, -1 when left < right, 0 when left == right
static char mn_textt_cmp_ (Textt left, Textt right)
{
    char to_return;
    int lenl, lenr;

    if (MN_TEXTT_IS_CONST(left))
    {
        // val : val
        if (MN_TEXTT_IS_CONST(right))
        {
            lenl = VARSIZE_ANY_EXHDR(left) - 1;
            lenr = VARSIZE_ANY_EXHDR(right) - 1;
            to_return = strncmp(MN_TEXTT_GET_VALUE(left), MN_TEXTT_GET_VALUE(right), (lenl < lenr) ? lenl : lenr);
            if ((to_return == 0) && (lenl != lenr))
                to_return = (lenl < lenr) ? -1 : 1;
        }
        // val > markednull
        else
            to_return = 1;
    }
    else
    {
        // markednull < val
        if (MN_TEXTT_IS_CONST(right))
            to_return = -1;
        // id : id
        else
            to_return = MN_TEXTT_GET_ID(left) - MN_TEXTT_GET_ID(right);
    }

    return to_return;
}

PG_FUNCTION_INFO_V1(mn_textt_gt);
Datum mn_textt_gt(PG_FUNCTION_ARGS)
{
    Textt left  = PG_ARGISNULL(0) ? NULL : PG_GETARG_TEXTT(0);
    Textt right = PG_ARGISNULL(1) ? NULL : PG_GETARG_TEXTT(1);
    
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1))
        PG_RETURN_NULL();

    PG_RETURN_BOOL(mn_textt_cmp_(left, right) > 0);
}

PG_FUNCTION_INFO_V1(mn_textt_ge);
Datum mn_textt_ge(PG_FUNCTION_ARGS)
{
    Textt left  = PG_ARGISNULL(0) ? NULL : PG_GETARG_TEXTT(0);
    Textt right = PG_ARGISNULL(1) ? NULL : PG_GETARG_TEXTT(1);
    
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1))
        PG_RETURN_NULL();

    PG_RETURN_BOOL(mn_textt_cmp_(left, right) >= 0);
}

PG_FUNCTION_INFO_V1(mn_textt_lt);
Datum mn_textt_lt(PG_FUNCTION_ARGS)
{
    Textt left  = PG_ARGISNULL(0) ? NULL : PG_GETARG_TEXTT(0);
    Textt right = PG_ARGISNULL(1) ? NULL : PG_GETARG_TEXTT(1);
    
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1))
        PG_RETURN_NULL();

    PG_RETURN_BOOL(mn_textt_cmp_(left, right) < 0);
}

PG_FUNCTION_INFO_V1(mn_textt_le);
Datum mn_textt_le(PG_FUNCTION_ARGS)
{
    Textt left  = PG_ARGISNULL(0) ? NULL : PG_GETARG_TEXTT(0);
    Textt right = PG_ARGISNULL(1) ? NULL : PG_GETARG_TEXTT(1);
    
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1))
        PG_RETURN_NULL();

    PG_RETURN_BOOL(mn_textt_cmp_(left, right) <= 0);
}

PG_FUNCTION_INFO_V1(mn_textt_eq);
Datum mn_textt_eq(PG_FUNCTION_ARGS)
{
    Textt left  = PG_ARGISNULL(0) ? NULL : PG_GETARG_TEXTT(0);
    Textt right = PG_ARGISNULL(1) ? NULL : PG_GETARG_TEXTT(1);
    
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1))
        PG_RETURN_NULL();

    PG_RETURN_BOOL(mn_textt_cmp_(left, right) == 0);
}

PG_FUNCTION_INFO_V1(mn_textt_ne);
Datum mn_textt_ne(PG_FUNCTION_ARGS)
{
    Textt left  = PG_ARGISNULL(0) ? NULL : PG_GETARG_TEXTT(0);
    Textt right = PG_ARGISNULL(1) ? NULL : PG_GETARG_TEXTT(1);
    
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1))
        PG_RETURN_NULL();

    PG_RETURN_BOOL(mn_textt_cmp_(left, right) != 0);
}

PG_FUNCTION_INFO_V1(mn_textt_cmp);
Datum mn_textt_cmp(PG_FUNCTION_ARGS)
{
    Textt left  = PG_ARGISNULL(0) ? NULL : PG_GETARG_TEXTT(0);
    Textt right = PG_ARGISNULL(1) ? NULL : PG_GETARG_TEXTT(1);
    
    if (PG_ARGISNULL(0) || PG_ARGISNULL(1))
        PG_RETURN_NULL();

    PG_RETURN_INT32((int32) mn_textt_cmp_(left, right));
}

/************************************************************************************
 * Cast functions
 ************************************************************************************/

PG_FUNCTION_INFO_V1(mn_textt_cast_to_text);
Datum mn_textt_cast_to_text(PG_FUNCTION_ARGS)
{
    Textt textt = (Textt) PG_GETARG_POINTER(0);
    Text to_return;

    if (MN_TEXTT_IS_MARKEDNULL(textt))
        PG_RETURN_NULL();

    to_return = textt_to_text(textt);

    PG_RETURN_TEXT_P(to_return);
}

PG_FUNCTION_INFO_V1(mn_text_cast_to_textt);
Datum mn_text_cast_to_textt(PG_FUNCTION_ARGS)
{
    Text t = (Text) PG_GETARG_TEXT_P(0);
    Textt to_return = text_to_textt(t);
    PG_RETURN_TEXTT(to_return);
}

/************************************************************************************
 * Aggregate functions
 ************************************************************************************/

PG_FUNCTION_INFO_V1(mn_textt_count_sfunc);
Datum mn_textt_count_sfunc(PG_FUNCTION_ARGS)
{
    int64 count = PG_GETARG_INT64(0);
    Textt  this = PG_ARGISNULL(1) ? NULL : PG_GETARG_TEXTT(1);

    if (this == NULL)
        PG_RETURN_INT64(count);

    if (MN_TEXTT_IS_CONST(this))
        count += 1;

    PG_RETURN_INT64(count);
}

PG_FUNCTION_INFO_V1(mn_textt_countt_sfunc);
Datum mn_textt_countt_sfunc(PG_FUNCTION_ARGS)
{
    int64 count = PG_GETARG_INT64(0);
    Textt  this = PG_ARGISNULL(1) ? NULL : PG_GETARG_TEXTT(1);

    if (this == NULL)
        PG_RETURN_INT64(count);
    
    count += 1;
    PG_RETURN_INT64(count);
}

PG_FUNCTION_INFO_V1(mn_textt_count_cfunc);
Datum mn_textt_count_cfunc(PG_FUNCTION_ARGS)
{
    PG_RETURN_INT64(PG_GETARG_INT64(0) + PG_GETARG_INT64(1));
}

PG_FUNCTION_INFO_V1(mn_textt_max_sfunc);
Datum mn_textt_max_sfunc(PG_FUNCTION_ARGS)
{
    Text  max  = PG_ARGISNULL(0) ? NULL : (Text) PG_GETARG_TEXT_P(0);
    Textt this = PG_ARGISNULL(1) ? NULL : (Textt) PG_GETARG_TEXTT(1);

    Text val;
    int lenv, lenm;
    char cmp;

    if (max == NULL)
        if (this == NULL || MN_TEXTT_IS_MARKEDNULL(this))
            PG_RETURN_NULL();
        else
            PG_RETURN_TEXT_P(textt_to_text(this));
    else
    {
        if (this == NULL || MN_TEXTT_IS_MARKEDNULL(this))
            PG_RETURN_TEXT_P(max);
        else
        {
            val = textt_to_text(this);
            lenv = VARSIZE_ANY_EXHDR(val) - 1;
            lenm = VARSIZE_ANY_EXHDR(max) - 1;
            cmp = strncmp(VARDATA(val), VARDATA(max), (lenv < lenm) ? lenv : lenm);
            if ((cmp == 0) && (lenv != lenm))
                cmp = (lenv < lenm) ? -1 : 1;
            if (cmp > 0)
                PG_RETURN_TEXT_P(val);
            else {
                pfree(val);
                PG_RETURN_TEXT_P(max);
            }
        }
    }
}

PG_FUNCTION_INFO_V1(mn_textt_max_cfunc);
Datum mn_textt_max_cfunc(PG_FUNCTION_ARGS)
{
    Text left  = PG_GETARG_TEXT_P_COPY(0);
    Text right = PG_GETARG_TEXT_P_COPY(1);

    int lenl = VARSIZE_ANY_EXHDR(left) - 1;
    int lenr = VARSIZE_ANY_EXHDR(right) - 1;
    char cmp = strncmp(VARDATA(left), VARDATA(right), (lenl < lenr) ? lenl : lenr);
    if ((cmp == 0) && (lenl != lenr))
        cmp = (lenl < lenr) ? -1 : 1;
    
    PG_RETURN_TEXT_P(cmp > 0 ? left : right);
}

PG_FUNCTION_INFO_V1(mn_textt_min_sfunc);
Datum mn_textt_min_sfunc(PG_FUNCTION_ARGS)
{
    Text  min  = PG_ARGISNULL(0) ? NULL : (Text) PG_GETARG_TEXT_P(0);
    Textt this = PG_ARGISNULL(1) ? NULL : (Textt) PG_GETARG_TEXTT(1);

    Text val;
    int lenv, lenm;
    char cmp;

    if (min == NULL)
        if (this == NULL || MN_TEXTT_IS_MARKEDNULL(this))
            PG_RETURN_NULL();
        else
            PG_RETURN_TEXT_P(textt_to_text(this));
    else
        if (this == NULL || MN_TEXTT_IS_MARKEDNULL(this))
            PG_RETURN_TEXT_P(min);
        else
        {
            val = textt_to_text(this);
            lenv = VARSIZE_ANY_EXHDR(val) - 1;
            lenm = VARSIZE_ANY_EXHDR(min) - 1;
            cmp = strncmp(VARDATA(val), VARDATA(min), (lenv < lenm) ? lenv : lenm);
            if ((cmp == 0) && (lenv != lenm))
                cmp = (lenv < lenm) ? -1 : 1;
            if (cmp < 0)
                PG_RETURN_TEXT_P(val);
            else {
                pfree(val);
                PG_RETURN_TEXT_P(min);
            }
        }
}

PG_FUNCTION_INFO_V1(mn_textt_min_cfunc);
Datum mn_textt_min_cfunc(PG_FUNCTION_ARGS)
{
    Text left  = PG_GETARG_TEXT_P_COPY(0);
    Text right = PG_GETARG_TEXT_P_COPY(1);

    int lenl = VARSIZE_ANY_EXHDR(left) - 1;
    int lenr = VARSIZE_ANY_EXHDR(right) - 1;
    char cmp = strncmp(VARDATA(left), VARDATA(right), (lenl < lenr) ? lenl : lenr);
    if ((cmp == 0) && (lenl != lenr))
        cmp = (lenl < lenr) ? -1 : 1;
    
    PG_RETURN_TEXT_P(cmp < 0 ? left : right);
}

/************************************************************************************
 * Hash Support Functions
 ************************************************************************************/

PG_FUNCTION_INFO_V1(mn_textt_hash);
Datum mn_textt_hash(PG_FUNCTION_ARGS)
{
    Textt textt = PG_GETARG_TEXTT(0);

    uint32 to_return = hash_any((unsigned char*) VARDATA_ANY(textt), VARSIZE_ANY_EXHDR(textt));
    PG_RETURN_INT32(to_return);
}

PG_FUNCTION_INFO_V1(mn_textt_hash_extend);
Datum mn_textt_hash_extend(PG_FUNCTION_ARGS)
{
    Textt textt = PG_GETARG_TEXTT(0);
    uint64 seed = PG_GETARG_INT64(1);
    
    uint64 to_return = hash_any_extended((unsigned char*) VARDATA_ANY(textt), VARSIZE_ANY_EXHDR(textt), seed);
    PG_RETURN_UINT64(to_return);
}

/************************************************************************************
 * Additional Functions
 ************************************************************************************/

PG_FUNCTION_INFO_V1(mn_textt_is_const);
Datum mn_textt_is_const(PG_FUNCTION_ARGS)
{
    Textt textt = PG_GETARG_TEXTT(0);
    PG_RETURN_BOOL(MN_TEXTT_IS_CONST(textt));
}

PG_FUNCTION_INFO_V1(mn_textt_is_marked_null);
Datum mn_textt_is_marked_null(PG_FUNCTION_ARGS)
{
    Textt textt = PG_GETARG_TEXTT(0);
    PG_RETURN_BOOL(MN_TEXTT_IS_MARKEDNULL(textt));
}

PG_FUNCTION_INFO_V1(mn_textt_nullid);
Datum mn_textt_nullid(PG_FUNCTION_ARGS)
{
    Textt textt = PG_GETARG_TEXTT(0);

    if (MN_TEXTT_IS_CONST(textt))
        PG_RETURN_NULL();
    
    PG_RETURN_INT64((int64)MN_TEXTT_GET_ID(textt));
}