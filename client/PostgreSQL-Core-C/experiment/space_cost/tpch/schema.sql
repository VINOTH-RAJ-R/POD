CREATE TABLE part (
    p_partkey     int,
    p_name        text,
    p_mfgr        text,
    p_brand       text,
    p_type        text,
    p_size        int,
    p_container   text,
    p_retailprice numeric,
    p_comment     text
);

CREATE TABLE region (
    r_regionkey  int,
    r_name       text,         
    r_comment    text          
);

CREATE TABLE nation (
    n_nationkey  int,          
    n_name       text,         
    n_regionkey  int,          
    n_comment    text          
);

CREATE TABLE supplier (
    s_suppkey     int,         
    s_name        text,        
    s_address     text,        
    s_nationkey   int,         
    s_phone       text,        
    s_acctbal     numeric,     
    s_comment     text         
);

CREATE TABLE partsupp (
    ps_partkey     int,        
    ps_suppkey     int,           
    ps_availqty    int,        
    ps_supplycost  numeric,    
    ps_comment     text        
);

CREATE TABLE customer (
    c_custkey     int,         
    c_name        text,        
    c_address     text,        
    c_nationkey   int,         
    c_phone       text,        
    c_acctbal     numeric,     
    c_mktsegment  text,        
    c_comment     text         
);

CREATE TABLE orders (
    o_orderkey       int,      
    o_custkey        int,      
    o_orderstatus    text,     
    o_totalprice     numeric,  
    o_orderdate      date,     
    o_orderpriority  text,     
    o_clerk          text,     
    o_shippriority   int,      
    o_comment        text      
);

CREATE TABLE lineitem (
    l_orderkey       int,      
    l_partkey        int,      
    l_suppkey        int,      
    l_linenumber     int,      
    l_quantity       numeric,  
    l_extendedprice  numeric,  
    l_discount       numeric,  
    l_tax            numeric,  
    l_returnflag     text,     
    l_linestatus     text,     
    l_shipdate       date,    
    l_commitdate     date,    
    l_receiptdate    date,    
    l_shipinstruct   text,    
    l_shipmode       text,    
    l_comment        text     
);
