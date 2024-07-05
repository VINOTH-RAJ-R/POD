#include <math.h>

#include "postgres.h"

#include "fmgr.h"
#include "funcapi.h"
#include "libpq/pqformat.h"

#include "common/int.h"
#include "common/hashfn.h"

#define INTT_ERROR_MSG "invalid input syntax for type marked integer"
#define INTT_LENGTH 5

typedef struct Intt_t
{
    // 1 when it is a constant, otherwise 0
    char    is_const;
    // parsed as real value (int32) when is_const == 1, otherwise as null id(uint32)
    char    val[4];
} Intt_t;

typedef struct Intt_avg_state_t
{
    int64 sum;
    int64 count;
} Intt_avg_state_t;

typedef Intt_t* Intt;

#define InttGetDatum(X)			 PointerGetDatum(X)
#define PG_GETARG_INTT(X)		 ((Intt)PG_GETARG_POINTER(X))
#define PG_RETURN_INTT(X)		 return InttGetDatum(X)

#define MN_INTT_GET_FLAG(i)		 ((i)->is_const)
#define MN_INTT_SET_FLAG(i, f)	 ((i)->is_const = (f))
#define MN_INTT_IS_CONST(i)		 ((i)->is_const == 1)
#define MN_INTT_IS_MARKEDNULL(i) ((i)->is_const == 0)
#define MN_INTT_GET_VALUE(i)	 (*(int32*)((i)->val))
#define MN_INTT_SET_VALUE(i, v)	 (*(int32*)((i)->val) = (v))
#define MN_INTT_GET_ID(i)		 (*(uint32*)((i)->val))
#define MN_INTT_SET_ID(i, id)	 (*(uint32*)((i)->val) = (id))

/************************************************************************************
 * Internal used functions
 ************************************************************************************/

static inline Intt int_to_intt(int32 value)
{
    Intt intt = (Intt) palloc(INTT_LENGTH);
    MN_INTT_SET_FLAG(intt, 1);
    MN_INTT_SET_VALUE(intt, value);
    return intt;
}

static inline Intt id_to_intt(uint32 id)
{
    Intt intt = (Intt) palloc(INTT_LENGTH);
    MN_INTT_SET_FLAG(intt, 0);
    MN_INTT_SET_ID(intt, id);
    return intt;
}

static inline Intt dup_intt(Intt intt)
{
    Intt to_return = (Intt) palloc(INTT_LENGTH);
    memcpy(to_return, intt, INTT_LENGTH);
    return to_return;
}
