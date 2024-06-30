/************************************************************************************
 * /src/numericc.sql
 * SQL statements for numericc (marked numeric) data type.
 ************************************************************************************/

/* I/O */

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

/* Casts */

-- // Implicite casts:
-- //  INTT        --> NUMERICC
-- //  NUMERIC     --> NUMERICC
-- // Explicit casts:
-- //  INTT        <-- NUMERICC
-- //  NUMERIC     <-- NUMERICC
-- //  SMALLINT    <-> NUMERICC [TODO]
-- //  INT         <-> NUMERICC
-- //  BIGINT      <-> NUMERICC [TODO]
-- //  REAL        <-> NUMERICC [TODO]
-- //  DOUBLE      <-> NUMERICC [TODO]
-- //  INTT        <-> NUMERIC

-- // I --> N
CREATE FUNCTION mn_cast_intt_to_numericc(intt)
    RETURNS numericc
    AS 'numericc', 'mn_cast_intt_to_numericc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (intt AS numericc)
    WITH FUNCTION mn_cast_intt_to_numericc(intt)
    AS IMPLICIT;

-- // n --> N
CREATE FUNCTION mn_cast_numeric_to_numericc(numeric)
    RETURNS numericc
    AS 'numericc', 'mn_cast_numeric_to_numericc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (numeric AS numericc)
    WITH FUNCTION mn_cast_numeric_to_numericc(numeric)
    AS IMPLICIT;

-- // N --> I
CREATE FUNCTION mn_cast_numericc_to_intt(numericc)
    RETURNS intt
    AS 'numericc', 'mn_cast_numericc_to_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (numericc AS intt)
    WITH FUNCTION mn_cast_numericc_to_intt(numericc)
    AS ASSIGNMENT;

-- // N --> n
CREATE FUNCTION mn_cast_numericc_to_numeric(numericc)
    RETURNS numeric
    AS 'numericc', 'mn_cast_numericc_to_numeric'
    LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (numericc AS numeric)
    WITH FUNCTION mn_cast_numericc_to_numeric(numericc)
    AS ASSIGNMENT;

-- // I --> n
CREATE FUNCTION mn_cast_intt_to_numeric(intt)
    RETURNS numeric
    AS 'numericc', 'mn_cast_intt_to_numeric'
    LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (intt AS numeric)
    WITH FUNCTION mn_cast_intt_to_numeric(intt)
    AS IMPLICIT;

-- // n --> I
CREATE FUNCTION mn_cast_numeric_to_intt(numeric)
    RETURNS intt
    AS 'numericc', 'mn_cast_numeric_to_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (numeric AS intt)
    WITH FUNCTION mn_cast_numeric_to_intt(numeric)
    AS ASSIGNMENT;

/* Arithmetic Operators */

-- Operators: [+, u+, -, u-, *, /, %, ^, u@]
-- Types: [NN, NI, IN]

-- // N + N
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

-- // + N
CREATE FUNCTION mn_numericc_unary_add(numericc)
    RETURNS numericc
    AS 'numericc', 'mn_numericc_unary_add'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR + (
    rightarg    = numericc,
    procedure   = mn_numericc_unary_add
);

-- // N - N
CREATE FUNCTION mn_numericc_substract_numericc(numericc, numericc)
    RETURNS numericc
    AS 'numericc', 'mn_numericc_substract_numericc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR - (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_substract_numericc
);

-- // - N
CREATE FUNCTION mn_numericc_negation(numericc)
    RETURNS numericc
    AS 'numericc', 'mn_numericc_negation'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR - (
    rightarg    = numericc,
    procedure   = mn_numericc_negation
);

-- // N * N
CREATE FUNCTION mn_numericc_multiply_numericc(numericc, numericc)
    RETURNS numericc
    AS 'numericc', 'mn_numericc_multiply_numericc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR * (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_multiply_numericc,
    commutator  = *
);

-- // N / N
CREATE FUNCTION mn_numericc_divide_numericc(numericc, numericc)
    RETURNS numericc
    AS 'numericc', 'mn_numericc_divide_numericc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR / (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_divide_numericc
);

-- // N % N
CREATE FUNCTION mn_numericc_modulo_numericc(numericc, numericc)
    RETURNS numericc
    AS 'numericc', 'mn_numericc_modulo_numericc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR % (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_modulo_numericc
);

-- // N ^ N
CREATE FUNCTION mn_numericc_power_numericc(numericc, numericc)
    RETURNS numericc
    AS 'numericc', 'mn_numericc_power_numericc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR ^ (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_power_numericc
);

-- // @ N
CREATE FUNCTION mn_numericc_absolute(numericc)
    RETURNS numeric
    AS 'numericc', 'mn_numericc_absolute'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR @ (
    rightarg    = numericc,
    procedure   = mn_numericc_absolute
);

/* Comparisons */

CREATE FUNCTION mn_numericc_gt (numericc, numericc)
    RETURNS bool
    AS 'numericc', 'mn_numericc_gt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_numericc_ge (numericc, numericc)
    RETURNS bool
    AS 'numericc', 'mn_numericc_ge'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_numericc_lt (numericc, numericc)
    RETURNS bool
    AS 'numericc', 'mn_numericc_lt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_numericc_le (numericc, numericc)
    RETURNS bool
    AS 'numericc', 'mn_numericc_le'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_numericc_eq (numericc, numericc)
    RETURNS bool
    AS 'numericc', 'mn_numericc_eq'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_numericc_ne (numericc, numericc)
    RETURNS bool
    AS 'numericc', 'mn_numericc_ne'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR > (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_gt,
    commutator  = <,
    negator     = <=,
    restrict    = scalargtsel,
    join        = scalargtjoinsel,
    MERGES
);
CREATE OPERATOR >= (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_ge,
    commutator  = <=,
    negator     = <,
    restrict    = scalargesel,
    join        = scalargejoinsel,
    MERGES
);
CREATE OPERATOR < (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_lt,
    commutator  = >,
    negator     = >=,
    restrict    = scalarltsel,
    join        = scalarltjoinsel,
    MERGES
);
CREATE OPERATOR <= (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_le,
    commutator  = >=,
    negator     = >,
    restrict    = scalarlesel,
    join        = scalarlejoinsel,
    MERGES
);
CREATE OPERATOR = (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_eq,
    commutator  = =,
    negator     = <>,
    restrict    = eqsel,
    join        = eqjoinsel,
    HASHES, MERGES
);
CREATE OPERATOR <> (
    leftarg     = numericc,
    rightarg    = numericc,
    procedure   = mn_numericc_ne,
    commutator  = <>,
    negator     = =,
    restrict    = neqsel,
    join        = neqjoinsel,
    MERGES
);

/* Indexing and Hashing */

CREATE FUNCTION mn_numericc_cmp(numericc, numericc)
    RETURNS int
    AS 'numericc', 'mn_numericc_cmp'
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
        FUNCTION    1   mn_numericc_cmp(numericc, numericc),
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
CREATE OPERATOR CLASS mn_numericc_hash_ops
    DEFAULT FOR TYPE numericc USING hash AS
        OPERATOR    1   =   ,
        FUNCTION    1   mn_numericc_hash(numericc),
        FUNCTION    2   mn_numericc_hash_extend(numericc, bigint);   

/* Aggregations */

-- // common aggregation functions
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

-- // COUNT, COUNTT
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

-- // SUM
CREATE FUNCTION mn_numericc_sum_ffunc(internal)
    RETURNS numericc
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

-- // AVG
CREATE FUNCTION mn_numericc_avg_ffunc(internal)
    RETURNS numericc
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

-- // MAX
CREATE FUNCTION mn_numericc_max_sfunc(numericc, numericc)
    RETURNS numericc
    AS 'numericc', 'mn_numericc_max_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE FUNCTION mn_numericc_max_cfunc(numericc, numericc)
    RETURNS numericc
    AS 'numericc', 'mn_numericc_max_cfunc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE AGGREGATE max (numericc)
(
    sfunc = mn_numericc_max_sfunc,
    stype = numericc,
    combinefunc = mn_numericc_max_cfunc,
    parallel = safe
);

-- // MIN
CREATE FUNCTION mn_numericc_min_sfunc(numericc, numericc)
    RETURNS numericc
    AS 'numericc', 'mn_numericc_min_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE FUNCTION mn_numericc_min_cfunc(numericc, numericc)
    RETURNS numericc
    AS 'numericc', 'mn_numericc_min_cfunc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE AGGREGATE min (numericc)
(
    sfunc = mn_numericc_min_sfunc,
    stype = numericc,
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
