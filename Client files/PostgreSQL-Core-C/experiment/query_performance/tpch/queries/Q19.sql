SELECT sum(l_extendedprice * (1::numeric - l_discount)) as revenue
FROM lineitem,
     part
WHERE (
            p_partkey = l_partkey
        AND p_brand = 'Brand#12'::text
        AND p_container in ('SM CASE'::text, 'SM BOX'::text, 'SM PACK'::text, 'SM PKG'::text)
        AND l_quantity >= 1 AND l_quantity <= 1 + 10
        AND p_size between 1 AND 5
        AND l_shipmode in ('AIR'::text, 'AIR REG'::text)
        AND l_shipinstruct = 'DELIVER IN PERSON'::text
    )
   OR (
            p_partkey = l_partkey
        AND p_brand = 'Brand#23'::text
        AND p_container in ('MED BAG'::text, 'MED BOX'::text, 'MED PKG'::text, 'MED PACK'::text)
        AND l_quantity >= 10 AND l_quantity <= 10 + 10
        AND p_size between 1 AND 10
        AND l_shipmode in ('AIR'::text, 'AIR REG'::text)
        AND l_shipinstruct = 'DELIVER IN PERSON'::text
    )
   OR (
            p_partkey = l_partkey
        AND p_brand = 'Brand#34'::text
        AND p_container in ('LG CASE'::text, 'LG BOX'::text, 'LG PACK'::text, 'LG PKG'::text)
        AND l_quantity >= 20 AND l_quantity <= 20 + 10
        AND p_size between 1 AND 15
        AND l_shipmode in ('AIR'::text, 'AIR REG'::text)
        AND l_shipinstruct = 'DELIVER IN PERSON'::text
    );
