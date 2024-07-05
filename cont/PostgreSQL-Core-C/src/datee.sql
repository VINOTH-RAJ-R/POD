/* Datee: marked date type */
CREATE FUNCTION mn_datee_input(cstring)
    RETURNS datee
    AS 'datee', 'mn_datee_input'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION mn_datee_output(datee)
    RETURNS cstring
    AS 'datee', 'mn_datee_output'
    LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE datee (
    input = mn_datee_input,
    output = mn_datee_output,
    internallength = 5
);

/* Date/Time Mathematical Operators */

CREATE FUNCTION mn_datee_add_intt(datee, intt)
    RETURNS datee
    AS 'datee', 'mn_datee_add_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR + (
    leftarg     = datee,
    rightarg    = intt,
    procedure   = mn_datee_add_intt
);

CREATE FUNCTION mn_intt_add_datee(intt, datee)
    RETURNS datee
    AS 'datee', 'mn_intt_add_datee'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR + (
    leftarg     = intt,
    rightarg    = datee,
    procedure   = mn_intt_add_datee
);

CREATE FUNCTION mn_datee_minus_intt(datee, intt)
    RETURNS datee
    AS 'datee', 'mn_datee_minus_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR - (
    leftarg     = datee,
    rightarg    = intt,
    procedure   = mn_datee_minus_intt
);

CREATE FUNCTION mn_datee_difference(datee, datee)
    RETURNS intt
    AS 'datee', 'mn_datee_difference'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR - (
    leftarg     = datee,
    rightarg    = datee,
    procedure   = mn_datee_difference
);

/* Comparison operators for Datee type */

CREATE FUNCTION mn_datee_gt_indexing (datee, datee)
    RETURNS bool
    AS 'datee', 'mn_datee_gt_indexing'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_datee_ge_indexing (datee, datee)
    RETURNS bool
    AS 'datee', 'mn_datee_ge_indexing'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_datee_lt_indexing (datee, datee)
    RETURNS bool
    AS 'datee', 'mn_datee_lt_indexing'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_datee_le_indexing (datee, datee)
    RETURNS bool
    AS 'datee', 'mn_datee_le_indexing'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_datee_eq_indexing (datee, datee)
    RETURNS bool
    AS 'datee', 'mn_datee_eq_indexing'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_datee_ne_indexing (datee, datee)
    RETURNS bool
    AS 'datee', 'mn_datee_ne_indexing'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR > (
    leftarg     = datee,
    rightarg    = datee,
    procedure   = mn_datee_gt_indexing,
    commutator  = <,
    negator     = <=,
    restrict    = scalargtsel,
    join        = scalargtjoinsel,
    MERGES
);
CREATE OPERATOR >= (
    leftarg     = datee,
    rightarg    = datee,
    procedure   = mn_datee_ge_indexing,
    commutator  = <=,
    negator     = <,
    restrict    = scalargesel,
    join        = scalargejoinsel,
    MERGES
);
CREATE OPERATOR < (
    leftarg     = datee,
    rightarg    = datee,
    procedure   = mn_datee_lt_indexing,
    commutator  = >,
    negator     = >=,
    restrict    = scalarltsel,
    join        = scalarltjoinsel,
    MERGES
);
CREATE OPERATOR <= (
    leftarg     = datee,
    rightarg    = datee,
    procedure   = mn_datee_le_indexing,
    commutator  = >=,
    negator     = >,
    restrict    = scalarlesel,
    join        = scalarlejoinsel,
    MERGES
);
CREATE OPERATOR = (
    leftarg     = datee,
    rightarg    = datee,
    procedure   = mn_datee_eq_indexing,
    commutator  = =,
    negator     = <>,
    restrict    = eqsel,
    join        = eqjoinsel,
    HASHES, MERGES
);
CREATE OPERATOR <> (
    leftarg     = datee,
    rightarg    = datee,
    procedure   = mn_datee_ne_indexing,
    commutator  = <>,
    negator     = =,
    restrict    = neqsel,
    join        = neqjoinsel,
    MERGES
);

/* Indexing: btree, hash */

CREATE FUNCTION mn_datee_cmp_indexing(datee, datee)
    RETURNS int
    AS 'datee', 'mn_datee_cmp_indexing'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_datee_sortsupport(internal)
    RETURNS VOID
    AS 'datee', 'mn_datee_sortsupport'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR CLASS mn_datee_ops
    DEFAULT FOR TYPE datee USING btree AS
        OPERATOR    1   <   ,
        OPERATOR    2   <=  ,
        OPERATOR    3   =   ,
        OPERATOR    4   >=  ,
        OPERATOR    5   >   ,
        FUNCTION    1   mn_datee_cmp_indexing(datee, datee),
        FUNCTION    2   mn_datee_sortsupport(internal);

CREATE FUNCTION mn_datee_hash(datee)
    RETURNS int
    AS 'datee', 'mn_datee_hash'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_datee_hash_extend(datee, bigint)
    RETURNS bigint
    AS 'datee', 'mn_datee_hash_extend'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR CLASS mn_datee_hash_indexing_ops
    DEFAULT FOR TYPE datee USING hash AS
        OPERATOR    1   =   ,
        FUNCTION    1   mn_datee_hash(datee),
        FUNCTION    2   mn_datee_hash_extend(datee, bigint);

/* Casts */

CREATE FUNCTION mn_datee_cast_from_date(date)
    RETURNS datee
    AS 'datee', 'mn_datee_cast_from_date'
    LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (date AS datee)
    WITH FUNCTION mn_datee_cast_from_date(date)
    AS IMPLICIT;

CREATE FUNCTION mn_datee_cast_to_date(datee)
    RETURNS date
    AS 'datee', 'mn_datee_cast_to_date'
    LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (datee AS date)
    WITH FUNCTION mn_datee_cast_to_date(datee)
    AS ASSIGNMENT;

CREATE FUNCTION mn_datee_cast_to_text(arg datee)
    RETURNS text AS
    $$ SELECT arg::date::text; $$
    LANGUAGE SQL STRICT;
CREATE CAST (datee AS text)
    WITH FUNCTION mn_datee_cast_to_text(datee)
    AS ASSIGNMENT;

CREATE FUNCTION mn_datee_cast_to_varchar(arg datee)
    RETURNS text AS
    $$ SELECT arg::date::varchar; $$
    LANGUAGE SQL STRICT;
CREATE CAST (datee AS varchar)
    WITH FUNCTION mn_datee_cast_to_varchar(datee)
    AS ASSIGNMENT;

CREATE FUNCTION mn_datee_cast_to_char(arg datee)
    RETURNS text AS
    $$ SELECT arg::date::char; $$
    LANGUAGE SQL STRICT;
CREATE CAST (datee AS char)
    WITH FUNCTION mn_datee_cast_to_char(datee)
    AS ASSIGNMENT;

/* Aggregations */

CREATE FUNCTION mn_datee_count_sfunc(bigint, datee)
    RETURNS bigint
    AS 'datee', 'mn_datee_count_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE FUNCTION mn_datee_count_cfunc(bigint, bigint)
    RETURNS bigint
    AS 'datee', 'mn_datee_count_cfunc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE AGGREGATE count (datee)
(
    sfunc = mn_datee_count_sfunc,
    stype = bigint,
    combinefunc = mn_datee_count_cfunc,
    initcond = '0',
    parallel = safe
);

CREATE FUNCTION mn_datee_countt_sfunc(bigint, datee)
    RETURNS bigint
    AS 'datee', 'mn_datee_countt_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE AGGREGATE countt (datee)
(
    sfunc = mn_datee_countt_sfunc,
    stype = bigint,
    combinefunc = mn_datee_count_cfunc,
    initcond = '0',
    parallel = safe
);

CREATE FUNCTION mn_datee_max_sfunc(date, datee)
    RETURNS date
    AS 'datee', 'mn_datee_max_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE FUNCTION mn_datee_max_cfunc(date, date)
    RETURNS date
    AS 'datee', 'mn_datee_max_cfunc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE AGGREGATE max (datee)
(
    sfunc = mn_datee_max_sfunc,
    stype = date,
    combinefunc = mn_datee_max_cfunc,
    parallel = safe
);

CREATE FUNCTION mn_datee_min_sfunc(date, datee)
    RETURNS date
    AS 'datee', 'mn_datee_min_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE FUNCTION mn_datee_min_cfunc(date, date)
    RETURNS date
    AS 'datee', 'mn_datee_min_cfunc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE AGGREGATE min (datee)
(
    sfunc = mn_datee_min_sfunc,
    stype = date,
    combinefunc = mn_datee_min_cfunc,
    parallel = safe
);

/* Additional Functions */

CREATE FUNCTION is_const (datee)
    RETURNS bool
    AS 'datee', 'mn_datee_is_const'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION is_marked_null (datee)
    RETURNS bool
    AS 'datee', 'mn_datee_is_marked_null'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION nullid (datee)
    RETURNS bigint
    AS 'datee', 'mn_datee_nullid'
    LANGUAGE C IMMUTABLE STRICT;