CREATE FUNCTION mn_version()
    RETURNS text AS
    $$SELECT '<NOW>'$$
    LANGUAGE SQL;

/* Intt: marked integer type */
CREATE FUNCTION mn_intt_input(cstring)
    RETURNS intt
    AS 'intt', 'mn_intt_input'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION mn_intt_output(intt)
    RETURNS cstring
    AS 'intt', 'mn_intt_output'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION mn_intt_recv(internal)
    RETURNS intt
    AS 'intt', 'mn_intt_recv'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION mn_intt_send(intt)
    RETURNS bytea
    AS 'intt', 'mn_intt_send'
    LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE intt (
    input = mn_intt_input,
    output = mn_intt_output,
    receive = mn_intt_recv,
    send = mn_intt_send,
    internallength = 5
);

/* Mathematical operators for Intt type */

/* + plus */
CREATE FUNCTION mn_intt_add_intt(intt, intt)
    RETURNS intt
    AS 'intt', 'mn_intt_add_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR + (
    leftarg     = intt,
    rightarg    = intt,
    procedure   = mn_intt_add_intt,
    commutator  = +
);

CREATE FUNCTION mn_int_add_intt(int, intt)
    RETURNS intt
    AS 'intt', 'mn_int_add_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR + (
    leftarg     = int,
    rightarg    = intt,
    procedure   = mn_int_add_intt,
    commutator  = +
);

CREATE FUNCTION mn_intt_add_int(intt, int)
    RETURNS intt
    AS 'intt', 'mn_intt_add_int'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR + (
    leftarg     = intt,
    rightarg    = int,
    procedure   = mn_intt_add_int,
    commutator  = +
);

CREATE FUNCTION mn_intt_unary_add(intt)
    RETURNS intt
    AS 'intt', 'mn_intt_unary_add'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR + (
    rightarg    = intt,
    procedure   = mn_intt_unary_add
);

/* - minus */
CREATE FUNCTION mn_intt_substract_intt(intt, intt)
    RETURNS intt
    AS 'intt', 'mn_intt_substract_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR - (
    leftarg     = intt,
    rightarg    = intt,
    procedure   = mn_intt_substract_intt
);

CREATE FUNCTION mn_int_substract_intt(int, intt)
    RETURNS intt
    AS 'intt', 'mn_int_substract_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR - (
    leftarg     = int,
    rightarg    = intt,
    procedure   = mn_int_substract_intt
);

CREATE FUNCTION mn_intt_substract_int(intt, int)
    RETURNS intt
    AS 'intt', 'mn_intt_substract_int'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR - (
    leftarg     = intt,
    rightarg    = int,
    procedure   = mn_intt_substract_int
);

CREATE FUNCTION mn_intt_negation(intt)
    RETURNS intt
    AS 'intt', 'mn_intt_negation'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR - (
    rightarg    = intt,
    procedure   = mn_intt_negation
);

/* * multiply */
CREATE FUNCTION mn_intt_multiply_intt(intt, intt)
    RETURNS intt
    AS 'intt', 'mn_intt_multiply_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR * (
    leftarg     = intt,
    rightarg    = intt,
    procedure   = mn_intt_multiply_intt,
    commutator  = *
);

CREATE FUNCTION mn_int_multiply_intt(int, intt)
    RETURNS intt
    AS 'intt', 'mn_int_multiply_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR * (
    leftarg     = int,
    rightarg    = intt,
    procedure   = mn_int_multiply_intt,
    commutator  = *
);

CREATE FUNCTION mn_intt_multiply_int(intt, int)
    RETURNS intt
    AS 'intt', 'mn_intt_multiply_int'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR * (
    leftarg     = intt,
    rightarg    = int,
    procedure   = mn_intt_multiply_int,
    commutator  = *
);

/* / divide */
CREATE FUNCTION mn_intt_divide_intt(intt, intt)
    RETURNS intt
    AS 'intt', 'mn_intt_divide_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR / (
    leftarg     = intt,
    rightarg    = intt,
    procedure   = mn_intt_divide_intt
);

CREATE FUNCTION mn_int_divide_intt(int, intt)
    RETURNS intt
    AS 'intt', 'mn_int_divide_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR / (
    leftarg     = int,
    rightarg    = intt,
    procedure   = mn_int_divide_intt
);

CREATE FUNCTION mn_intt_divide_int(intt, int)
    RETURNS intt
    AS 'intt', 'mn_intt_divide_int'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR / (
    leftarg     = intt,
    rightarg    = int,
    procedure   = mn_intt_divide_int
);

/* % modulo */
CREATE FUNCTION mn_intt_modulo_intt(intt, intt)
    RETURNS intt
    AS 'intt', 'mn_intt_modulo_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR % (
    leftarg     = intt,
    rightarg    = intt,
    procedure   = mn_intt_modulo_intt
);

CREATE FUNCTION mn_int_modulo_intt(int, intt)
    RETURNS intt
    AS 'intt', 'mn_int_modulo_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR % (
    leftarg     = int,
    rightarg    = intt,
    procedure   = mn_int_modulo_intt
);

CREATE FUNCTION mn_intt_modulo_int(intt, int)
    RETURNS intt
    AS 'intt', 'mn_intt_modulo_int'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR % (
    leftarg     = intt,
    rightarg    = int,
    procedure   = mn_intt_modulo_int
);

/* ^ expo */
CREATE FUNCTION mn_intt_exponential_int(leftarg intt, rightarg int)
    RETURNS numeric AS
    $$
        SELECT leftarg::numeric ^ rightarg::numeric;
    $$
    LANGUAGE SQL;
CREATE OPERATOR ^ (
    leftarg     = intt,
    rightarg    = int,
    FUNCTION    = mn_intt_exponential_int
);

CREATE FUNCTION mn_int_exponential_intt(leftarg int, rightarg intt)
    RETURNS numeric AS
    $$
        SELECT leftarg::numeric ^ rightarg::numeric;
    $$
    LANGUAGE SQL;
CREATE OPERATOR ^ (
    leftarg     = int,
    rightarg    = intt,
    FUNCTION    = mn_int_exponential_intt
);

CREATE FUNCTION mn_intt_absolute(intt)
    RETURNS intt
    AS 'intt', 'mn_intt_absolute'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR @ (
    rightarg    = intt,
    procedure   = mn_intt_absolute
);

CREATE FUNCTION mn_intt_bitwise_and_intt(intt, intt)
    RETURNS intt
    AS 'intt', 'mn_intt_bitwise_and_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR & (
    leftarg     = intt,
    rightarg    = intt,
    procedure   = mn_intt_bitwise_and_intt
);

CREATE FUNCTION mn_intt_bitwise_or_intt(intt, intt)
    RETURNS intt
    AS 'intt', 'mn_intt_bitwise_or_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR | (
    leftarg     = intt,
    rightarg    = intt,
    procedure   = mn_intt_bitwise_or_intt
);

CREATE FUNCTION mn_intt_bitwise_xor_intt(intt, intt)
    RETURNS intt
    AS 'intt', 'mn_intt_bitwise_xor_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR # (
    leftarg     = intt,
    rightarg    = intt,
    procedure   = mn_intt_bitwise_xor_intt
);

CREATE FUNCTION mn_intt_bitwise_not(intt)
    RETURNS intt
    AS 'intt', 'mn_intt_bitwise_not'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR ~ (
    rightarg    = intt,
    procedure   = mn_intt_bitwise_not
);

CREATE FUNCTION mn_intt_left_shift_intt(intt, intt)
    RETURNS intt
    AS 'intt', 'mn_intt_left_shift_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR << (
    leftarg     = intt,
    rightarg    = intt,
    procedure   = mn_intt_left_shift_intt
);

CREATE FUNCTION mn_intt_right_shift_intt(intt, intt)
    RETURNS intt
    AS 'intt', 'mn_intt_right_shift_intt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR >> (
    leftarg     = intt,
    rightarg    = intt,
    procedure   = mn_intt_right_shift_intt
);

/* Comparison operators for Intt type */

CREATE FUNCTION mn_intt_gt_indexing (intt, intt)
    RETURNS bool
    AS 'intt', 'mn_intt_gt_indexing'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_intt_ge_indexing (intt, intt)
    RETURNS bool
    AS 'intt', 'mn_intt_ge_indexing'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_intt_lt_indexing (intt, intt)
    RETURNS bool
    AS 'intt', 'mn_intt_lt_indexing'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_intt_le_indexing (intt, intt)
    RETURNS bool
    AS 'intt', 'mn_intt_le_indexing'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_intt_eq_indexing (intt, intt)
    RETURNS bool
    AS 'intt', 'mn_intt_eq_indexing'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_intt_ne_indexing (intt, intt)
    RETURNS bool
    AS 'intt', 'mn_intt_ne_indexing'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR > (
    leftarg     = intt,
    rightarg    = intt,
    procedure   = mn_intt_gt_indexing,
    commutator  = <,
    negator     = <=,
    restrict    = scalargtsel,
    join        = scalargtjoinsel,
    MERGES
);
CREATE OPERATOR >= (
    leftarg     = intt,
    rightarg    = intt,
    procedure   = mn_intt_ge_indexing,
    commutator  = <=,
    negator     = <,
    restrict    = scalargesel,
    join        = scalargejoinsel,
    MERGES
);
CREATE OPERATOR < (
    leftarg     = intt,
    rightarg    = intt,
    procedure   = mn_intt_lt_indexing,
    commutator  = >,
    negator     = >=,
    restrict    = scalarltsel,
    join        = scalarltjoinsel,
    MERGES
);
CREATE OPERATOR <= (
    leftarg     = intt,
    rightarg    = intt,
    procedure   = mn_intt_le_indexing,
    commutator  = >=,
    negator     = >,
    restrict    = scalarlesel,
    join        = scalarlejoinsel,
    MERGES
);
CREATE OPERATOR = (
    leftarg     = intt,
    rightarg    = intt,
    procedure   = mn_intt_eq_indexing,
    commutator  = =,
    negator     = <>,
    restrict    = eqsel,
    join        = eqjoinsel,
    HASHES, MERGES
);
CREATE OPERATOR <> (
    leftarg     = intt,
    rightarg    = intt,
    procedure   = mn_intt_ne_indexing,
    commutator  = <>,
    negator     = =,
    restrict    = neqsel,
    join        = neqjoinsel,
    MERGES
);

/* Indexing: btree, hash */

CREATE FUNCTION mn_intt_cmp_indexing(intt, intt)
    RETURNS int
    AS 'intt', 'mn_intt_cmp_indexing'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_intt_sortsupport(internal)
    RETURNS VOID
    AS 'intt', 'mn_intt_sortsupport'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_intt_inrange(intt, intt, intt, BOOLEAN, BOOLEAN)
    RETURNS BOOLEAN
    AS 'intt', 'mn_intt_inrange'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR CLASS mn_intt_ops
    DEFAULT FOR TYPE intt USING btree AS
        OPERATOR    1   <   ,
        OPERATOR    2   <=  ,
        OPERATOR    3   =   ,
        OPERATOR    4   >=  ,
        OPERATOR    5   >   ,
        FUNCTION    1   mn_intt_cmp_indexing(intt, intt),
        FUNCTION    2   mn_intt_sortsupport(internal),
        FUNCTION    3   mn_intt_inrange(intt, intt, intt, BOOLEAN, BOOLEAN);

CREATE FUNCTION mn_intt_hash(intt)
    RETURNS int
    AS 'intt', 'mn_intt_hash'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_intt_hash_extend(intt, bigint)
    RETURNS bigint
    AS 'intt', 'mn_intt_hash_extend'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR CLASS mn_intt_hash_ops
    DEFAULT FOR TYPE intt USING hash AS
        OPERATOR    1   =   ,
        FUNCTION    1   mn_intt_hash(intt),
        FUNCTION    2   mn_intt_hash_extend(intt, bigint);

/* Casts */

-- CREATE CAST (int AS intt) WITH INOUT AS IMPLICIT;
CREATE FUNCTION mn_intt_cast_from_int(int)
RETURNS intt 
    AS 'intt', 'mn_intt_cast_from_int'
    LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (int AS intt)
    WITH FUNCTION mn_intt_cast_from_int(int)
    AS IMPLICIT;

CREATE FUNCTION mn_intt_cast_to_int(intt)
    RETURNS int
    AS 'intt', 'mn_intt_cast_to_int'
    LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (intt AS int)
    WITH FUNCTION mn_intt_cast_to_int(intt)
    AS ASSIGNMENT;

CREATE FUNCTION mn_intt_cast_to_real(origin intt)
    RETURNS real AS
    $$
    SELECT origin::int::real;
    $$
    LANGUAGE SQL;
CREATE CAST (intt AS real)
    WITH FUNCTION mn_intt_cast_to_real
    AS ASSIGNMENT;

CREATE FUNCTION mn_intt_cast_to_double(origin intt)
    RETURNS double precision AS
    $$
    SELECT origin::int::double precision;
    $$
    LANGUAGE SQL;
CREATE CAST (intt AS double precision)
    WITH FUNCTION mn_intt_cast_to_double
    AS ASSIGNMENT;

CREATE FUNCTION mn_intt_cast_to_text(arg intt)
    RETURNS text AS
    $$ SELECT arg::int::text; $$
    LANGUAGE SQL STRICT;
CREATE CAST (intt AS text)
    WITH FUNCTION mn_intt_cast_to_text(intt)
    AS ASSIGNMENT;

CREATE FUNCTION mn_intt_cast_to_varchar(arg intt)
    RETURNS text AS
    $$ SELECT arg::int::varchar; $$
    LANGUAGE SQL STRICT;
CREATE CAST (intt AS varchar)
    WITH FUNCTION mn_intt_cast_to_varchar(intt)
    AS ASSIGNMENT;

CREATE FUNCTION mn_intt_cast_to_char(arg intt)
    RETURNS text AS
    $$ SELECT arg::int::char; $$
    LANGUAGE SQL STRICT;
CREATE CAST (intt AS char)
    WITH FUNCTION mn_intt_cast_to_char(intt)
    AS ASSIGNMENT;

/* Aggregations */

CREATE FUNCTION mn_intt_count_sfunc(bigint, intt)
    RETURNS bigint
    AS 'intt', 'mn_intt_count_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE FUNCTION mn_intt_count_cfunc(bigint, bigint)
    RETURNS bigint
    AS 'intt', 'mn_intt_count_cfunc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE AGGREGATE count (intt)
(
    sfunc = mn_intt_count_sfunc,
    stype = bigint,
    combinefunc = mn_intt_count_cfunc,
    initcond = '0',
    parallel = safe
);

CREATE FUNCTION mn_intt_countt_sfunc(bigint, intt)
    RETURNS bigint
    AS 'intt', 'mn_intt_countt_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE AGGREGATE countt (intt)
(
    sfunc = mn_intt_countt_sfunc,
    stype = bigint,
    combinefunc = mn_intt_count_cfunc,
    initcond = '0',
    parallel = safe
);

CREATE FUNCTION mn_intt_sum_sfunc(bigint, intt)
    RETURNS bigint
    AS 'intt', 'mn_intt_sum_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE FUNCTION mn_intt_sum_cfunc(bigint, bigint)
    RETURNS bigint
    AS 'intt', 'mn_intt_sum_cfunc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE AGGREGATE sum (intt)
(
    sfunc = mn_intt_sum_sfunc,
    stype = bigint,
    combinefunc = mn_intt_sum_cfunc,
    parallel = safe
);

CREATE FUNCTION mn_intt_max_sfunc(int, intt)
    RETURNS int
    AS 'intt', 'mn_intt_max_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE FUNCTION mn_intt_max_cfunc(int, int)
    RETURNS int
    AS 'intt', 'mn_intt_max_cfunc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE AGGREGATE max (intt)
(
    sfunc = mn_intt_max_sfunc,
    stype = int,
    combinefunc = mn_intt_max_cfunc,
    parallel = safe
);

CREATE FUNCTION mn_intt_min_sfunc(int, intt)
    RETURNS int
    AS 'intt', 'mn_intt_min_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE FUNCTION mn_intt_min_cfunc(int, int)
    RETURNS int
    AS 'intt', 'mn_intt_min_cfunc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE AGGREGATE min (intt)
(
    sfunc = mn_intt_min_sfunc,
    stype = int,
    combinefunc = mn_intt_min_cfunc,
    parallel = safe
);

CREATE FUNCTION mn_intt_avg_sfunc(internal, intt)
    RETURNS internal
    AS 'intt', 'mn_intt_avg_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE FUNCTION mn_intt_avg_ffunc(internal)
    RETURNS numeric
    AS 'numericc', 'mn_intt_avg_ffunc'
    LANGUAGE C IMMUTABLE;
CREATE FUNCTION mn_intt_avg_cfunc(internal, internal)
    RETURNS internal
    AS 'intt', 'mn_intt_avg_cfunc'
    LANGUAGE C IMMUTABLE;
CREATE FUNCTION mn_intt_avg_serialize(internal)
    RETURNS bytea
    AS 'intt', 'mn_intt_avg_serialize'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_intt_avg_deserialize(bytea, internal)
    RETURNS internal
    AS 'intt', 'mn_intt_avg_deserialize'
    LANGUAGE C IMMUTABLE STRICT;
CREATE AGGREGATE avg (intt)
(
    sfunc       = mn_intt_avg_sfunc,
    stype       = internal,
    finalfunc   = mn_intt_avg_ffunc,
    combinefunc = mn_intt_avg_cfunc,
    serialfunc  = mn_intt_avg_serialize,
    deserialfunc= mn_intt_avg_deserialize,
    parallel = safe
);

/* Additional Functions */

CREATE FUNCTION is_const (intt)
    RETURNS bool
    AS 'intt', 'mn_intt_is_const'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION is_marked_null (intt)
    RETURNS bool
    AS 'intt', 'mn_intt_is_marked_null'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION nullid (intt)
    RETURNS bigint
    AS 'intt', 'mn_intt_nullid'
    LANGUAGE C IMMUTABLE STRICT;