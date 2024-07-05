/* Textt: marked text type */
CREATE FUNCTION mn_textt_input(cstring)
    RETURNS textt
    AS 'textt', 'mn_textt_input'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION mn_textt_output(textt)
    RETURNS cstring
    AS 'textt', 'mn_textt_output'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION mn_textt_recv(internal)
    RETURNS textt
    AS 'textt', 'mn_textt_recv'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION mn_textt_send(textt)
    RETURNS bytea
    AS 'textt', 'mn_textt_send'
    LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE textt (
    input = mn_textt_input,
    output = mn_textt_output,
    receive = mn_textt_recv,
    send = mn_textt_send,
    internallength = VARIABLE,
    storage = extended
); 

/* String operators for Textt type */

CREATE FUNCTION mn_textt_concatenate(textt, textt)
    RETURNS textt
    AS 'textt', 'mn_textt_concatenate'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR || (
    leftarg     = textt,
    rightarg    = textt,
    procedure   = mn_textt_concatenate
);

CREATE FUNCTION mn_textt_like(leftarg textt, rightarg textt)
    RETURNS BOOLEAN
    AS 'textt', 'mn_textt_like'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR ~~ (
    leftarg     = textt,
    rightarg    = textt,
    function    = mn_textt_like
);

CREATE FUNCTION mn_textt_not_like(leftarg textt, rightarg textt)
    RETURNS BOOLEAN
    AS 'textt', 'mn_textt_not_like'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR !~~ (
    leftarg     = textt,
    rightarg    = textt,
    function    = mn_textt_not_like
);

/* Comparison operators for Textt type */

CREATE FUNCTION mn_textt_gt (textt, textt)
    RETURNS bool
    AS 'textt', 'mn_textt_gt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_textt_ge (textt, textt)
    RETURNS bool
    AS 'textt', 'mn_textt_ge'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_textt_lt (textt, textt)
    RETURNS bool
    AS 'textt', 'mn_textt_lt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_textt_le (textt, textt)
    RETURNS bool
    AS 'textt', 'mn_textt_le'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_textt_eq (textt, textt)
    RETURNS bool
    AS 'textt', 'mn_textt_eq'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_textt_ne (textt, textt)
    RETURNS bool
    AS 'textt', 'mn_textt_ne'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR > (
    leftarg     = textt,
    rightarg    = textt,
    procedure   = mn_textt_gt,
    commutator  = <,
    negator     = <=,
    restrict    = scalargtsel,
    join        = scalargtjoinsel,
    MERGES
);
CREATE OPERATOR >= (
    leftarg     = textt,
    rightarg    = textt,
    procedure   = mn_textt_ge,
    commutator  = <=,
    negator     = <,
    restrict    = scalargesel,
    join        = scalargejoinsel,
    MERGES
);
CREATE OPERATOR < (
    leftarg     = textt,
    rightarg    = textt,
    procedure   = mn_textt_lt,
    commutator  = >,
    negator     = >=,
    restrict    = scalarltsel,
    join        = scalarltjoinsel,
    MERGES
);
CREATE OPERATOR <= (
    leftarg     = textt,
    rightarg    = textt,
    procedure   = mn_textt_le,
    commutator  = >=,
    negator     = >,
    restrict    = scalarlesel,
    join        = scalarlejoinsel,
    MERGES
);
CREATE OPERATOR = (
    leftarg     = textt,
    rightarg    = textt,
    procedure   = mn_textt_eq,
    commutator  = =,
    negator     = <>,
    restrict    = eqsel,
    join        = eqjoinsel,
    HASHES, MERGES
);
CREATE OPERATOR <> (
    leftarg     = textt,
    rightarg    = textt,
    procedure   = mn_textt_ne,
    commutator  = <>,
    negator     = =,
    restrict    = neqsel,
    join        = neqjoinsel,
    MERGES
);

/* Indexing: btree, hash */

CREATE FUNCTION mn_textt_cmp(textt, textt)
    RETURNS int
    AS 'textt', 'mn_textt_cmp'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR CLASS mn_textt_ops
    DEFAULT FOR TYPE textt USING btree AS
        OPERATOR    1   <   ,
        OPERATOR    2   <=  ,
        OPERATOR    3   =   ,
        OPERATOR    4   >=  ,
        OPERATOR    5   >   ,
        FUNCTION    1   mn_textt_cmp(textt, textt);

CREATE FUNCTION mn_textt_hash(textt)
    RETURNS int
    AS 'textt', 'mn_textt_hash'
    LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION mn_textt_hash_extend(textt, bigint)
    RETURNS bigint
    AS 'textt', 'mn_textt_hash_extend'
    LANGUAGE C IMMUTABLE STRICT;
CREATE OPERATOR CLASS mn_textt_hash_ops
    DEFAULT FOR TYPE textt USING hash AS
        OPERATOR    1   =   ,
        FUNCTION    1   mn_textt_hash(textt),
        FUNCTION    2   mn_textt_hash_extend(textt, bigint);

/* Casts */

CREATE FUNCTION mn_text_cast_to_textt(text)
    RETURNS textt
    AS 'textt', 'mn_text_cast_to_textt'
    LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (text AS textt)
    WITH FUNCTION mn_text_cast_to_textt(text)
    AS IMPLICIT;

CREATE FUNCTION mn_textt_cast_to_text(textt)
    RETURNS text
    AS 'textt', 'mn_textt_cast_to_text'
    LANGUAGE C IMMUTABLE STRICT;
CREATE CAST (textt AS text)
    WITH FUNCTION mn_textt_cast_to_text(textt)
    AS ASSIGNMENT;

CREATE FUNCTION mn_textt_cast_to_varchar(arg textt)
    RETURNS text AS
    $$ SELECT arg::text::varchar; $$
    LANGUAGE SQL STRICT;
CREATE CAST (textt AS varchar)
    WITH FUNCTION mn_textt_cast_to_varchar(textt)
    AS ASSIGNMENT;

CREATE FUNCTION mn_textt_cast_to_char(arg textt)
    RETURNS text AS
    $$ SELECT arg::text::char; $$
    LANGUAGE SQL STRICT;
CREATE CAST (textt AS char)
    WITH FUNCTION mn_textt_cast_to_char(textt)
    AS ASSIGNMENT;

/* Aggregations */

CREATE FUNCTION mn_textt_count_sfunc(bigint, textt)
    RETURNS bigint
    AS 'textt', 'mn_textt_count_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE FUNCTION mn_textt_count_cfunc(bigint, bigint)
    RETURNS bigint
    AS 'textt', 'mn_textt_count_cfunc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE AGGREGATE count (textt)
(
    sfunc = mn_textt_count_sfunc,
    stype = bigint,
    combinefunc = mn_textt_count_cfunc,
    initcond = '0',
    parallel = safe
);

CREATE FUNCTION mn_textt_countt_sfunc(bigint, textt)
    RETURNS bigint
    AS 'textt', 'mn_textt_countt_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE AGGREGATE countt (textt)
(
    sfunc = mn_textt_countt_sfunc,
    stype = bigint,
    combinefunc = mn_textt_count_cfunc,
    initcond = '0',
    parallel = safe
);

CREATE FUNCTION mn_textt_max_sfunc(text, textt)
    RETURNS text
    AS 'textt', 'mn_textt_max_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE FUNCTION mn_textt_max_cfunc(text, text)
    RETURNS text
    AS 'textt', 'mn_textt_max_cfunc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE AGGREGATE max (textt)
(
    sfunc = mn_textt_max_sfunc,
    stype = text,
    combinefunc = mn_textt_max_cfunc,
    parallel = safe
);

CREATE FUNCTION mn_textt_min_sfunc(text, textt)
    RETURNS text
    AS 'textt', 'mn_textt_min_sfunc'
    LANGUAGE C IMMUTABLE;
CREATE FUNCTION mn_textt_min_cfunc(text, text)
    RETURNS text
    AS 'textt', 'mn_textt_min_cfunc'
    LANGUAGE C IMMUTABLE STRICT;
CREATE AGGREGATE min (textt)
(
    sfunc = mn_textt_min_sfunc,
    stype = text,
    combinefunc = mn_textt_min_cfunc,
    parallel = safe
);

/* Additional Functions */

CREATE FUNCTION is_const (textt)
    RETURNS bool
    AS 'textt', 'mn_textt_is_const'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION is_marked_null (textt)
    RETURNS bool
    AS 'textt', 'mn_textt_is_marked_null'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION nullid (textt)
    RETURNS bigint
    AS 'textt', 'mn_textt_nullid'
    LANGUAGE C IMMUTABLE STRICT;
