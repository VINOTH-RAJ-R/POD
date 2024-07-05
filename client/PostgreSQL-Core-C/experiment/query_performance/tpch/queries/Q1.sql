select l_returnflag,
       l_linestatus,
       max(l_quantity)                                       as sum_qty,
       max(l_extendedprice)                                  as sum_base_price,
       max(l_extendedprice * (1::numeric - l_discount))               as sum_disc_price,
       max(l_extendedprice * (1::numeric - l_discount) * (1::numeric + l_tax)) as sum_charge,
       max(l_quantity)                                       as avg_qty,
       max(l_extendedprice)                                  as avg_price,
       max(l_discount)                                       as avg_disc,
       count(*)                                              as count_order
from lineitem
where l_shipdate <= (date '1998-12-01' - interval '90' day)::date
group by l_returnflag,
         l_linestatus
order by l_returnflag,
         l_linestatus;