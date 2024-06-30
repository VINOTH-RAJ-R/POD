SELECT o_orderpriority, COUNT(*) AS order_count
FROM orders
WHERE o_orderdate >= '1993-07-01'::date
  AND o_orderdate < ('1993-07-01'::date + '3 months'::interval)::date
  AND EXISTS(
        SELECT *
        FROM lineitem
        WHERE l_orderkey = o_orderkey
          AND l_commitdate < l_receiptdate
    )
GROUP BY o_orderpriority
ORDER BY o_orderpriority;