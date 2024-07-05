/************************************************************************************
 * !!!DEPRECATED!!!
 * 
 * This file is an old implementation of NUMERICC data type.
 * It is backup only.
 ************************************************************************************/

/* Numericc: marked numeric type */
CREATE FUNCTION mn_numericc_input(cstring, oid, integer)
    RETURNS numericc
    AS 'numericc', 'mn_numericc_input'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION mn_numericc_output(numericc)
    RETURNS cstring
    AS 'numericc', 'mn_numericc_output'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION mn_numericc_recv(internal, oid, integer)
    RETURNS numericc
    AS 'numericc', 'mn_numericc_recv'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION mn_numericc_send(numericc)
    RETURNS bytea
    AS 'numericc', 'mn_numericc_send'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION mn_numericc_typmod_in(cstring[])
    RETURNS int
    AS 'numericc', 'mn_numericc_typmod_in'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION mn_numericc_typmod_out(int)
    RETURNS cstring
    AS 'numericc', 'mn_numericc_typmod_out'
    LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE numericc (
    input = mn_numericc_input,
    output = mn_numericc_output,
    receive = mn_numericc_recv,
    send = mn_numericc_send,
    typmod_in = mn_numericc_typmod_in,
    typmod_out = mn_numericc_typmod_out,
    internallength = VARIABLE,
    storage = extended
);

/* Mathematical operators for Numericc type */

CREATE FUNCTION mn_numericc_add_numericc(numericc, numericc)
    RETURNS numericc
    AS 'numericc', 'mn_numericc_add_numericc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR + (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_add_numericc,
    commutator  = +
);

CREATE FUNCTION mn_numericc_add_numeric(numericc, numeric)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_add_numeric'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR + (
    leftarg     = numericc,
    rightarg    = numeric,
    procedure   = mn_numericc_add_numeric,
    commutator  = +
);

CREATE FUNCTION mn_numeric_add_numericc(numeric, numericc)
    RETURNS numeric
    AS 'numericc', 'mn_numeric_add_numericc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR + (
    leftarg     = numeric,
    rightarg    = numericc,
    procedure   = mn_numeric_add_numericc,
    commutator  = +
);

CREATE FUNCTION mn_numericc_unary_add(numericc)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_unary_add'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR + (
    rightarg    = numericc,
    procedure   = mn_numericc_unary_add
);

CREATE FUNCTION mn_numericc_substract_numericc(numericc, numericc)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_substract_numericc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR - (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_substract_numericc
);

CREATE FUNCTION mn_numericc_substract_numeric(numericc, numeric)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_substract_numeric'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR - (
    leftarg     = numericc,
    rightarg    = numeric,
    procedure   = mn_numericc_substract_numeric
);

CREATE FUNCTION mn_numeric_substract_numericc(numeric, numericc)
    RETURNS numeric
    AS 'numericc', 'mn_numeric_substract_numericc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR - (
    leftarg     = numeric,
    rightarg    = numericc,
    procedure   = mn_numeric_substract_numericc
);

CREATE FUNCTION mn_numericc_negation(numericc)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_negation'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR - (
    rightarg    = numericc,
    procedure   = mn_numericc_negation
);

CREATE FUNCTION mn_numericc_multiply_numericc(numericc, numericc)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_multiply_numericc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR * (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_multiply_numericc,
    commutator  = *
);

CREATE FUNCTION mn_numericc_multiply_numeric(numericc, numeric)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_multiply_numeric'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR * (
    leftarg     = numericc,
    rightarg    = numeric,
    procedure   = mn_numericc_multiply_numeric,
    commutator  = *
);

CREATE FUNCTION mn_numeric_multiply_numericc(numeric, numericc)
    RETURNS numeric
    AS 'numericc', 'mn_numeric_multiply_numericc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR * (
    leftarg     = numeric,
    rightarg    = numericc,
    procedure   = mn_numeric_multiply_numericc,
    commutator  = *
);

CREATE FUNCTION mn_numericc_divide_numericc(numericc, numericc)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_divide_numericc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR / (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_divide_numericc
);

CREATE FUNCTION mn_numericc_divide_numeric(numericc, numeric)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_divide_numeric'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR / (
    leftarg     = numericc,
    rightarg    = numeric,
    procedure   = mn_numericc_divide_numeric
);

CREATE FUNCTION mn_numeric_divide_numericc(numeric, numericc)
    RETURNS numeric
    AS 'numericc', 'mn_numeric_divide_numericc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR / (
    leftarg     = numeric,
    rightarg    = numericc,
    procedure   = mn_numeric_divide_numericc
);

CREATE FUNCTION mn_numericc_modulo_numericc(numericc, numericc)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_modulo_numericc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR % (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_modulo_numericc
);

CREATE FUNCTION mn_numericc_modulo_numeric(numericc, numeric)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_modulo_numeric'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR % (
    leftarg     = numericc,
    rightarg    = numeric,
    procedure   = mn_numericc_modulo_numeric
);

CREATE FUNCTION mn_numeric_modulo_numericc(numeric, numericc)
    RETURNS numeric
    AS 'numericc', 'mn_numeric_modulo_numericc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR % (
    leftarg     = numeric,
    rightarg    = numericc,
    procedure   = mn_numeric_modulo_numericc
);

CREATE FUNCTION mn_numericc_exponential_numericc(numericc, numericc)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_exponential_numericc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR ^ (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_exponential_numericc
);

CREATE FUNCTION mn_numericc_exponential_numeric(numericc, numeric)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_exponential_numeric'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR ^ (
    leftarg     = numericc,
    rightarg    = numeric,
    procedure   = mn_numericc_exponential_numeric
);

CREATE FUNCTION mn_numeric_exponential_numericc(numeric, numericc)
    RETURNS numeric
    AS 'numericc', 'mn_numeric_exponential_numericc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR ^ (
    leftarg     = numeric,
    rightarg    = numericc,
    procedure   = mn_numeric_exponential_numericc
);

CREATE FUNCTION mn_numericc_absolute(numericc)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_absolute'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR @ (
    rightarg    = numericc,
    procedure   = mn_numericc_absolute
);

/* Comparison operators for Numericc type */

CREATE FUNCTION mn_numericc_gt_indexing (numericc, numericc)
    RETURNS bool
    AS 'numericc', 'mn_numericc_gt_indexing'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_numericc_ge_indexing (numericc, numericc)
    RETURNS bool
    AS 'numericc', 'mn_numericc_ge_indexing'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_numericc_lt_indexing (numericc, numericc)
    RETURNS bool
    AS 'numericc', 'mn_numericc_lt_indexing'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_numericc_le_indexing (numericc, numericc)
    RETURNS bool
    AS 'numericc', 'mn_numericc_le_indexing'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_numericc_eq_indexing (numericc, numericc)
    RETURNS bool
    AS 'numericc', 'mn_numericc_eq_indexing'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_numericc_ne_indexing (numericc, numericc)
    RETURNS bool
    AS 'numericc', 'mn_numericc_ne_indexing'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR > (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_gt_indexing,
    commutator  = <,
    negator     = <=,
    restrict    = scalargtsel,
    join        = scalargtjoinsel,
    MERGES
);
CREATE OPERATOR >= (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_ge_indexing,
    commutator  = <=,
    negator     = <,
    restrict    = scalargesel,
    join        = scalargejoinsel,
    MERGES
);
CREATE OPERATOR < (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_lt_indexing,
    commutator  = >,
    negator     = >=,
    restrict    = scalarltsel,
    join        = scalarltjoinsel,
    MERGES
);
CREATE OPERATOR <= (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_le_indexing,
    commutator  = >=,
    negator     = >,
    restrict    = scalarlesel,
    join        = scalarlejoinsel,
    MERGES
);
CREATE OPERATOR = (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_eq_indexing,
    commutator  = =,
    negator     = <>,
    restrict    = eqsel,
    join        = eqjoinsel,
    HASHES, MERGES
);
CREATE OPERATOR <> (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_ne_indexing,
    commutator  = <>,
    negator     = =,
    restrict    = neqsel,
    join        = neqjoinsel,
    MERGES
);

/* Indexing: btree, hash */

CREATE FUNCTION mn_numericc_cmp_indexing(numericc, numericc)
    RETURNS int
    AS 'numericc', 'mn_numericc_cmp_indexing'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_numericc_sortsupport(internal)
    RETURNS VOID
    AS 'numericc', 'mn_numericc_sortsupport'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_numericc_inrange(numericc, numericc, numericc, BOOLEAN, BOOLEAN)
    RETURNS BOOLEAN
    AS 'numericc', 'mn_numericc_inrange'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR CLASS mn_numericc_ops
    DEFAULT FOR TYPE numericc USING btree AS
        OPERATOR    1   <   ,
        OPERATOR    2   <=  ,
        OPERATOR    3   =   ,
        OPERATOR    4   >=  ,
        OPERATOR    5   >   ,
        FUNCTION    1   mn_numericc_cmp_indexing(numericc, numericc),
        FUNCTION    2   mn_numericc_sortsupport(internal),
        FUNCTION    3   mn_numericc_inrange(numericc, numericc, numericc, BOOLEAN, BOOLEAN);

CREATE FUNCTION mn_numericc_hash(numericc)
    RETURNS int
    AS 'numericc', 'mn_numericc_hash'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_numericc_hash_extend(numericc, bigint)
    RETURNS bigint
    AS 'numericc', 'mn_numericc_hash_extend'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR CLASS mn_numericc_hash_indexing_ops
    DEFAULT FOR TYPE numericc USING hash AS
        OPERATOR    1   =   ,
        FUNCTION    1   mn_numericc_hash(numericc),
        FUNCTION    2   mn_numericc_hash_extend(numericc, bigint);   

/* Casts */
CREATE FUNCTION mn_numericc_cast_from_numeric(numeric)
    RETURNS numericc
    AS 'numericc', 'mn_numericc_cast_from_numeric'
    LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (numeric AS numericc)
    WITH FUNCTION mn_numericc_cast_from_numeric(numeric)
    AS IMPLICIT;

CREATE FUNCTION mn_numericc_cast_to_numeric(numericc)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_cast_to_numeric'
    LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (numericc AS numeric)
    WITH FUNCTION mn_numericc_cast_to_numeric(numericc)
    AS ASSIGNMENT;

CREATE FUNCTION mn_numericc_cast_from_intt(intt)
    RETURNS numericc
    AS 'numericc', 'mn_numericc_cast_from_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (intt AS numericc)
    WITH FUNCTION mn_numericc_cast_from_intt(intt)
    AS IMPLICIT;

CREATE FUNCTION mn_numeric_cast_from_intt(intt)
    RETURNS numeric
    AS 'numericc', 'mn_numeric_cast_from_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (intt AS numeric)
    WITH FUNCTION mn_numeric_cast_from_intt(intt)
    AS ASSIGNMENT;

CREATE FUNCTION mn_numericc_cast_from_int(int)
    RETURNS numericc
    AS 'numericc', 'mn_numericc_cast_from_int'
    LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (int AS numericc)
    WITH FUNCTION mn_numericc_cast_from_int(int)
    AS IMPLICIT;

CREATE FUNCTION mn_numericc_cast_to_real(origin numericc)
    RETURNS real AS
    $$
    SELECT origin::numeric::real;
    $$
    LANGUAGE SQL;
CREATE CAST (numericc AS real)
    WITH FUNCTION mn_numericc_cast_to_real
    AS ASSIGNMENT;

CREATE FUNCTION mn_numericc_cast_to_double(origin numericc)
    RETURNS double precision AS
    $$
    SELECT origin::numeric::double precision;
    $$
    LANGUAGE SQL;
CREATE CAST (numericc AS double precision)
    WITH FUNCTION mn_numericc_cast_to_double
    AS ASSIGNMENT;

CREATE FUNCTION mn_numericc_cast_to_text(arg numericc)
    RETURNS text AS
    $$ SELECT arg::numeric::text; $$
    LANGUAGE SQL STRICT;
CREATE CAST (numericc AS text)
    WITH FUNCTION mn_numericc_cast_to_text(numericc)
    AS ASSIGNMENT;

CREATE FUNCTION mn_numericc_cast_to_varchar(arg numericc)
    RETURNS text AS
    $$ SELECT arg::numeric::varchar; $$
    LANGUAGE SQL STRICT;
CREATE CAST (numericc AS varchar)
    WITH FUNCTION mn_numericc_cast_to_varchar(numericc)
    AS ASSIGNMENT;

CREATE FUNCTION mn_numericc_cast_to_char(arg numericc)
    RETURNS text AS
    $$ SELECT arg::numeric::char; $$
    LANGUAGE SQL STRICT;
CREATE CAST (numericc AS char)
    WITH FUNCTION mn_numericc_cast_to_char(numericc)
    AS ASSIGNMENT;

/* Aggregations */

CREATE FUNCTION mn_numericc_count_sfunc(bigint, numericc)
    RETURNS bigint
    AS 'numericc', 'mn_numericc_count_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE FUNCTION mn_numericc_count_cfunc(bigint, bigint)
    RETURNS bigint
    AS 'numericc', 'mn_numericc_count_cfunc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE AGGREGATE count (numericc)
(
    sfunc = mn_numericc_count_sfunc,
    stype = bigint,
    combinefunc = mn_numericc_count_cfunc,
    initcond = '0',
    parallel = safe
);

CREATE FUNCTION mn_numericc_countt_sfunc(bigint, numericc)
    RETURNS bigint
    AS 'numericc', 'mn_numericc_countt_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE AGGREGATE countt (numericc)
(
    sfunc = mn_numericc_countt_sfunc,
    stype = bigint,
    combinefunc = mn_numericc_count_cfunc,
    initcond = '0',
    parallel = safe
);

CREATE FUNCTION mn_numericc_accum_sfunc(internal, numericc)
    RETURNS internal
    AS 'numericc', 'mn_numericc_accum_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE FUNCTION mn_numericc_accum_cfunc(internal, internal)
    RETURNS internal
    AS 'numericc', 'mn_numericc_accum_cfunc'
    LANGUAGE C IMMUTABLE;
CREATE FUNCTION mn_numericc_accum_serialize(internal)
    RETURNS bytea
    AS 'numericc', 'mn_numericc_accum_serialize'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_numericc_accum_deserialize(bytea, internal)
    RETURNS internal
    AS 'numericc', 'mn_numericc_accum_deserialize'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_numericc_accum_ifunc(internal, numericc)
    RETURNS internal
    AS 'numericc', 'mn_numericc_accum_ifunc'
    LANGUAGE C IMMUTABLE;

CREATE FUNCTION mn_numericc_sum_ffunc(internal)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_sum_ffunc'
    LANGUAGE C IMMUTABLE;
CREATE AGGREGATE sum (numericc)
(
    sfunc        = mn_numericc_accum_sfunc,
    stype        = internal,
    finalfunc    = mn_numericc_sum_ffunc,
    combinefunc  = mn_numericc_accum_cfunc,
    serialfunc   = mn_numericc_accum_serialize,
    deserialfunc = mn_numericc_accum_deserialize,
    mstype       = internal,
    msfunc       = mn_numericc_accum_sfunc,
    minvfunc     = mn_numericc_accum_ifunc,
    mfinalfunc   = mn_numericc_sum_ffunc,
    parallel = safe
);

CREATE FUNCTION mn_numericc_avg_ffunc(internal)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_avg_ffunc'
    LANGUAGE C IMMUTABLE;
CREATE AGGREGATE avg (numericc)
(
    sfunc        = mn_numericc_accum_sfunc,
    stype        = internal,
    finalfunc    = mn_numericc_avg_ffunc,
    combinefunc  = mn_numericc_accum_cfunc,
    serialfunc   = mn_numericc_accum_serialize,
    deserialfunc = mn_numericc_accum_deserialize,
    mstype       = internal,
    msfunc       = mn_numericc_accum_sfunc,
    minvfunc     = mn_numericc_accum_ifunc,
    mfinalfunc   = mn_numericc_avg_ffunc,
    parallel    = safe
);

CREATE FUNCTION mn_numericc_max_sfunc(numeric, numericc)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_max_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE FUNCTION mn_numericc_max_cfunc(numeric, numeric)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_max_cfunc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE AGGREGATE max (numericc)
(
    sfunc = mn_numericc_max_sfunc,
    stype = numeric,
    combinefunc = mn_numericc_max_cfunc,
    parallel = safe
);

CREATE FUNCTION mn_numericc_min_sfunc(numeric, numericc)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_min_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE FUNCTION mn_numericc_min_cfunc(numeric, numeric)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_min_cfunc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE AGGREGATE min (numericc)
(
    sfunc = mn_numericc_min_sfunc,
    stype = numeric,
    combinefunc = mn_numericc_min_cfunc,
    parallel = safe
);

/* Additional Functions */

CREATE FUNCTION is_const (numericc)
    RETURNS bool
    AS 'numericc', 'mn_numericc_is_const'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION is_marked_null (numericc)
    RETURNS bool
    AS 'numericc', 'mn_numericc_is_marked_null'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION nullid (numericc)
    RETURNS bigint
    AS 'numericc', 'mn_numericc_nullid'
    LANGUAGE C IMMUTABLE STRICT;