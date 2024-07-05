SELECT s_name,
       s_address
FROM supplier,
     nation
WHERE s_suppkey IN (
    SELECT ps_suppkey
    FROM partsupp
    WHERE ps_partkey IN (
        SELECT p_partkey
        FROM part
        WHERE p_name LIKE 'forest%'::text
    )
      AND ps_availqty::int > (
        SELECT (0.5 * SUM(l_quantity))::numeric
        FROM lineitem
        WHERE l_partkey = ps_partkey
          AND l_suppkey = ps_suppkey
          AND l_shipdate >= '1994-01-01'::date
          AND l_shipdate < ('1994-01-01'::date + '1 year'::interval)::date
    )
)
  AND s_nationkey = n_nationkey
  AND n_name = 'CANADA'::text
ORDER BY s_name;
