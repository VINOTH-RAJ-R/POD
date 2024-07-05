#include "postgres.h"

#include "fmgr.h"
#include "funcapi.h"
#include "libpq/pqformat.h"

#include "utils/date.h"
#include "utils/datetime.h"
#include "miscadmin.h"
#include "common/hashfn.h"

#include "intt.h"

#define DATEE_ERROR_MSG "invalid input syntax for type marked date"
#define DATEE_LENGTH 5

typedef struct Datee_t
{
    // 1 when it is a constant, otherwise 0
    char is_const;
    // parsed as real value (DateADT/int32) when is_const == 1
    // parsed as null id (uint32) when is_const == 0
    char val[4];
} Datee_t;

typedef Datee_t* Datee;

#define DateeGetDatum(X)		    PointerGetDatum(X)
#define PG_GETARG_DATEE(X)		    ((Datee)PG_GETARG_POINTER(X))
#define PG_RETURN_DATEE(X)		    return DateeGetDatum(X)

#define MN_DATEE_GET_FLAG(d)	    ((d)->is_const)
#define MN_DATEE_SET_FLAG(d, f)	    ((d)->is_const = (f))
#define MN_DATEE_IS_CONST(d)	    ((d)->is_const == 1)
#define MN_DATEE_IS_MARKEDNULL(d)   ((d)->is_const == 0)
#define MN_DATEE_GET_VALUE(d)	    (*(int32*)((d)->val))
#define MN_DATEE_SET_VALUE(d, v)	(*(int32*)((d)->val) = (v))
#define MN_DATEE_GET_ID(d)		    (*(uint32*)((d)->val))
#define MN_DATEE_SET_ID(d, id)	    (*(uint32*)((d)->val) = (id))

static inline Datee date_to_datee(int32 value)
{
    Datee to_return = (Datee) palloc(DATEE_LENGTH);
    MN_DATEE_SET_FLAG(to_return, 1);
    MN_DATEE_SET_VALUE(to_return, value);
    return to_return;
}

static inline Datee id_to_datee(uint32 id)
{
    Datee to_return = (Datee) palloc(DATEE_LENGTH);
    MN_DATEE_SET_FLAG(to_return, 0);
    MN_DATEE_SET_ID(to_return, id);
    return to_return;
}

static inline Datee dup_datee(Datee datee)
{
    Datee to_return = (Datee) palloc(DATEE_LENGTH);
    memcpy(to_return, datee, DATEE_LENGTH);
    return to_return;
}

/************************************************************************************
 * Internal used functions
 ************************************************************************************/

// COPIED FROM src/backend/utils/adt/date.c -> Datum date_in(PG_FUNCTION_ARGS)
static inline DateADT str_to_date(char* str, Node* escontext)
{
    DateADT		date;
	fsec_t		fsec;
	struct pg_tm tt,
			   *tm = &tt;
	int			tzp;
	int			dtype;
	int			nf;
	int			dterr;
	char	   *field[MAXDATEFIELDS];
	int			ftype[MAXDATEFIELDS];
	char		workbuf[MAXDATELEN + 1];
	DateTimeErrorExtra extra;

	dterr = ParseDateTime(str, workbuf, sizeof(workbuf),
						  field, ftype, MAXDATEFIELDS, &nf);
	if (dterr == 0)
		dterr = DecodeDateTime(field, ftype, nf,
							   &dtype, tm, &fsec, &tzp, &extra);
	if (dterr != 0)
	{
		DateTimeParseError(dterr, &extra, str, "date", escontext);
		// PG_RETURN_NULL();
	}

	switch (dtype)
	{
		case DTK_DATE:
			break;

		case DTK_EPOCH:
			GetEpochTime(tm);
			break;

		case DTK_LATE:
			DATE_NOEND(date);
			PG_RETURN_DATEADT(date);

		case DTK_EARLY:
			DATE_NOBEGIN(date);
			PG_RETURN_DATEADT(date);

		default:
			DateTimeParseError(DTERR_BAD_FORMAT, &extra, str, "date", escontext);
			// PG_RETURN_NULL();
            break;
	}

	/* Prevent overflow in Julian-day routines */
	if (!IS_VALID_JULIAN(tm->tm_year, tm->tm_mon, tm->tm_mday))
		ereturn(escontext, (Datum) 0,
				(errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
				 errmsg("date out of range: \"%s\"", str)));

	date = date2j(tm->tm_year, tm->tm_mon, tm->tm_mday) - POSTGRES_EPOCH_JDATE;

	/* Now check for just-out-of-range dates */
	if (!IS_VALID_DATE(date))
		ereturn(escontext, (Datum) 0,
				(errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
				 errmsg("date out of range: \"%s\"", str)));

    return date;
}

// COPIED FROM src/backend/utils/adt/date.c -> Datum date_out(PG_FUNCTION_ARGS)
static inline char* date_to_str(DateADT date)
{
    struct pg_tm tt,
                *tm = &tt;
    char         buf[MAXDATELEN + 1];

    if (DATE_NOT_FINITE(date))
        EncodeSpecialDate(date, buf);
    else
    {
        j2date(date + POSTGRES_EPOCH_JDATE,
                &(tm->tm_year),
                &(tm->tm_mon),
                &(tm->tm_mday)
        );
        EncodeDateOnly(tm, DateStyle, buf);
    }

    return pstrdup(buf);
}