#include "postgres.h"

#include "fmgr.h"
#include "funcapi.h"
#include "libpq/pqformat.h"

#include "catalog/pg_collation.h"
#include "mb/pg_wchar.h"
#include "miscadmin.h"
#include "utils/builtins.h"
#include "utils/pg_locale.h"

#include "common/hashfn.h"

#define FLEXIBLE_ARRAY_MEMBER
#define TEXTT_ERROR_MSG "invalid input syntax for type marked text"

/************************************************************************************
 * Marked text type
 ************************************************************************************/

typedef struct Textt_internal_t
{
    // 1 when it is a constant, otherwise 0
    char    is_const;
    // parsed as real value (char*) when is_const == 1, otherwise as null id (uint32)
    char    val[FLEXIBLE_ARRAY_MEMBER];
} Textt_internal_t;

typedef struct varlena* Textt;
typedef Textt_internal_t* Textt_i;
typedef struct varlena* Text;

#define DatumGetTextt(X)			((Textt) PG_DETOAST_DATUM(X))
#define DatumGetTexttCopy(X)		((Textt) PG_DETOAST_DATUM_COPY(X))
#define TexttGetDatum(X)			PointerGetDatum(X)
#define PG_GETARG_TEXTT(t)		 	(Textt)DatumGetTextt(PG_GETARG_DATUM(t))
#define PG_GETARG_TEXTT_COPY(t)	 	(Textt)DatumGetTexttCopy(PG_GETARG_DATUM(t))
#define PG_RETURN_TEXTT(x)		 	return TexttGetDatum(x)

#define MN_TEXTT_GET_FLAG(t)		(((Textt_i) VARDATA_ANY(t))->is_const)
#define MN_TEXTT_SET_FLAG(t, f)		(MN_TEXTT_GET_FLAG(t) = f)
#define MN_TEXTT_IS_CONST(t)		(MN_TEXTT_GET_FLAG(t) == 1)
#define MN_TEXTT_IS_MARKEDNULL(t)	(MN_TEXTT_GET_FLAG(t) == 0)
#define MN_TEXTT_GET_VALUE(t)		(((Textt_i) VARDATA_ANY(t))->val)

#define MN_TEXTT_GET_ID(t)			(*(uint32*)(((Textt_i) VARDATA_ANY(t))->val))
#define MN_TEXTT_SET_ID(t, i)		(MN_TEXTT_GET_ID(t) = i)


/************************************************************************************
 * Internal used functions
 ************************************************************************************/

static inline Textt build_textt(size_t length)
{
    Textt to_return = (Textt) palloc(length);
    SET_VARSIZE(to_return, length);
    return to_return;
}

static inline Textt cstring_to_textt(char* str)
{
    size_t internal_length;
    Textt to_return;

    internal_length = sizeof(char) + strlen(str);
    to_return = (Textt) palloc (internal_length + VARHDRSZ);
    SET_VARSIZE(to_return, internal_length + VARHDRSZ);
    MN_TEXTT_SET_FLAG(to_return, 1);
    memcpy(MN_TEXTT_GET_VALUE(to_return), str, strlen(str));

    return to_return;
}

static inline Textt text_to_textt(Text t)
{
    int total_length = sizeof(char) + VARSIZE_ANY(t);
    Textt to_return = build_textt(total_length);

    MN_TEXTT_SET_FLAG(to_return, 1);
    memcpy(MN_TEXTT_GET_VALUE(to_return), VARDATA_ANY(t), VARSIZE_ANY(t));

    return to_return;
}

static inline Textt id_to_textt(uint32 id)
{
    size_t total_length = VARHDRSZ + sizeof(char) + sizeof(uint32);
    Textt to_return = build_textt(total_length);

    MN_TEXTT_SET_FLAG(to_return, 0);
    MN_TEXTT_SET_ID(to_return, id);

    return to_return;
}

static inline Text textt_to_text(Textt tt)
{
    size_t cstring_length = VARSIZE_ANY_EXHDR(tt) - sizeof(char);
	size_t total_length = VARHDRSZ + cstring_length;

    Text to_return = (Text) palloc(total_length);
    SET_VARSIZE(to_return, total_length);
    memcpy(VARDATA_ANY(to_return), MN_TEXTT_GET_VALUE(tt), total_length);

    return to_return;
}

/*-------------------------------------------------------------------------
 *
 * like.c
 *	  like expression handling code.
 *
 *	 NOTES
 *		A big hack of the regexp.c code!! Contributed by
 *		Keith Parks <emkxp01@mtcc.demon.co.uk> (7/95).
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	src/backend/utils/adt/like.c
 *
 *-------------------------------------------------------------------------
 */
#define LIKE_TRUE						1
#define LIKE_FALSE						0
#define LIKE_ABORT						(-1)


static int	SB_MatchText(const char *t, int tlen, const char *p, int plen,
						 pg_locale_t locale, bool locale_is_c);

static int	MB_MatchText(const char *t, int tlen, const char *p, int plen,
						 pg_locale_t locale, bool locale_is_c);

static int	UTF8_MatchText(const char *t, int tlen, const char *p, int plen,
						   pg_locale_t locale, bool locale_is_c);

static int	SB_IMatchText(const char *t, int tlen, const char *p, int plen,
						  pg_locale_t locale, bool locale_is_c);

static int	GenericMatchText(const char *s, int slen, const char *p, int plen, Oid collation);
static int	Generic_Text_IC_like(text *str, text *pat, Oid collation);

/*--------------------
 * Support routine for MatchText. Compares given multibyte streams
 * as wide characters. If they match, returns 1 otherwise returns 0.
 *--------------------
 */
static inline int
wchareq(const char *p1, const char *p2)
{
	int			p1_len;

	/* Optimization:  quickly compare the first byte. */
	if (*p1 != *p2)
		return 0;

	p1_len = pg_mblen(p1);
	if (pg_mblen(p2) != p1_len)
		return 0;

	/* They are the same length */
	while (p1_len--)
	{
		if (*p1++ != *p2++)
			return 0;
	}
	return 1;
}

/*
 * Formerly we had a routine iwchareq() here that tried to do case-insensitive
 * comparison of multibyte characters.  It did not work at all, however,
 * because it relied on tolower() which has a single-byte API ... and
 * towlower() wouldn't be much better since we have no suitably cheap way
 * of getting a single character transformed to the system's wchar_t format.
 * So now, we just downcase the strings using lower() and apply regular LIKE
 * comparison.  This should be revisited when we install better locale support.
 */

/*
 * We do handle case-insensitive matching for single-byte encodings using
 * fold-on-the-fly processing, however.
 */
static char
SB_lower_char(unsigned char c, pg_locale_t locale, bool locale_is_c)
{
	if (locale_is_c)
		return pg_ascii_tolower(c);
#ifdef HAVE_LOCALE_T
	else if (locale)
		return tolower_l(c, locale->info.lt);
#endif
	else
		return pg_tolower(c);
}


#define NextByte(p, plen)	((p)++, (plen)--)

/* Set up to compile like_match.c for multibyte characters */
#define CHAREQ(p1, p2) wchareq((p1), (p2))
#define NextChar(p, plen) \
	do { int __l = pg_mblen(p); (p) +=__l; (plen) -=__l; } while (0)
#define CopyAdvChar(dst, src, srclen) \
	do { int __l = pg_mblen(src); \
		 (srclen) -= __l; \
		 while (__l-- > 0) \
			 *(dst)++ = *(src)++; \
	   } while (0)

#define MatchText	MB_MatchText

#include "like_match.c"

/* Set up to compile like_match.c for single-byte characters */
#define CHAREQ(p1, p2) (*(p1) == *(p2))
#define NextChar(p, plen) NextByte((p), (plen))
#define CopyAdvChar(dst, src, srclen) (*(dst)++ = *(src)++, (srclen)--)

#define MatchText	SB_MatchText

#include "like_match.c"

/* setup to compile like_match.c for single byte case insensitive matches */
#define MATCH_LOWER(t) SB_lower_char((unsigned char) (t), locale, locale_is_c)
#define NextChar(p, plen) NextByte((p), (plen))
#define MatchText SB_IMatchText

#include "like_match.c"

/* setup to compile like_match.c for UTF8 encoding, using fast NextChar */

#define NextChar(p, plen) \
	do { (p)++; (plen)--; } while ((plen) > 0 && (*(p) & 0xC0) == 0x80 )
#define MatchText	UTF8_MatchText

#include "like_match.c"

/* Generic for all cases not requiring inline case-folding */
static inline int
GenericMatchText(const char *s, int slen, const char *p, int plen, Oid collation)
{
	if (collation && !lc_ctype_is_c(collation) && collation != DEFAULT_COLLATION_OID)
	{
		pg_locale_t locale = pg_newlocale_from_collation(collation);

		if (locale && !locale->deterministic)
			ereport(ERROR,
					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
					 errmsg("nondeterministic collations are not supported for LIKE")));
	}

	if (pg_database_encoding_max_length() == 1)
		return SB_MatchText(s, slen, p, plen, 0, true);
	else if (GetDatabaseEncoding() == PG_UTF8)
		return UTF8_MatchText(s, slen, p, plen, 0, true);
	else
		return MB_MatchText(s, slen, p, plen, 0, true);
}

static inline int
Generic_Text_IC_like(text *str, text *pat, Oid collation)
{
	char	   *s,
			   *p;
	int			slen,
				plen;
	pg_locale_t locale = 0;
	bool		locale_is_c = false;

	if (lc_ctype_is_c(collation))
		locale_is_c = true;
	else if (collation != DEFAULT_COLLATION_OID)
	{
		if (!OidIsValid(collation))
		{
			/*
			 * This typically means that the parser could not resolve a
			 * conflict of implicit collations, so report it that way.
			 */
			ereport(ERROR,
					(errcode(ERRCODE_INDETERMINATE_COLLATION),
					 errmsg("could not determine which collation to use for ILIKE"),
					 errhint("Use the COLLATE clause to set the collation explicitly.")));
		}
		locale = pg_newlocale_from_collation(collation);

		if (locale && !locale->deterministic)
			ereport(ERROR,
					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
					 errmsg("nondeterministic collations are not supported for ILIKE")));
	}

	/*
	 * For efficiency reasons, in the single byte case we don't call lower()
	 * on the pattern and text, but instead call SB_lower_char on each
	 * character.  In the multi-byte case we don't have much choice :-(. Also,
	 * ICU does not support single-character case folding, so we go the long
	 * way.
	 */

	if (pg_database_encoding_max_length() > 1 || (locale && locale->provider == COLLPROVIDER_ICU))
	{
		pat = DatumGetTextPP(DirectFunctionCall1Coll(lower, collation,
													 PointerGetDatum(pat)));
		p = VARDATA_ANY(pat);
		plen = VARSIZE_ANY_EXHDR(pat);
		str = DatumGetTextPP(DirectFunctionCall1Coll(lower, collation,
													 PointerGetDatum(str)));
		s = VARDATA_ANY(str);
		slen = VARSIZE_ANY_EXHDR(str);
		if (GetDatabaseEncoding() == PG_UTF8)
			return UTF8_MatchText(s, slen, p, plen, 0, true);
		else
			return MB_MatchText(s, slen, p, plen, 0, true);
	}
	else
	{
		p = VARDATA_ANY(pat);
		plen = VARSIZE_ANY_EXHDR(pat);
		s = VARDATA_ANY(str);
		slen = VARSIZE_ANY_EXHDR(str);
		return SB_IMatchText(s, slen, p, plen, locale, locale_is_c);
	}
}
