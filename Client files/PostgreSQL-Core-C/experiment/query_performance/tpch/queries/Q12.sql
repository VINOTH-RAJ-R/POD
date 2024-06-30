SELECT l_shipmode,
       sum(case
               when o_orderpriority = '1-URGENT'::text
                   OR o_orderpriority = '2-HIGH'::text
                   then 1
               else 0
           end) as high_line_count,
       sum(case
               when o_orderpriority <> '1-URGENT'::text
                   AND o_orderpriority <> '2-HIGH'::text
                   then 1
               else 0
           end) AS low_line_count
FROM orders,
     lineitem
WHERE o_orderkey = l_orderkey
  AND l_shipmode in ('MAIL'::text, 'SHIP'::text)
  AND l_commitdate < l_receiptdate
  AND l_shipdate < l_commitdate
  AND l_receiptdate >= date '1994-01-01'
  AND l_receiptdate < (date '1994-01-01' + interval '1' year)::date
GROUP BY l_shipmode
ORDER BY l_shipmode;
