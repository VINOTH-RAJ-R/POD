CREATE TABLE part (
    p_partkey     intt,
    p_name        textt,
    p_mfgr        textt,
    p_brand       textt,
    p_type        textt,
    p_size        intt,
    p_container   textt,
    p_retailprice numericc,
    p_comment     textt
);

CREATE TABLE region (
    r_regionkey  intt,
    r_name       textt,         
    r_comment    textt          
);

CREATE TABLE nation (
    n_nationkey  intt,          
    n_name       textt,         
    n_regionkey  intt,          
    n_comment    textt          
);

CREATE TABLE supplier (
    s_suppkey     intt,         
    s_name        textt,        
    s_address     textt,        
    s_nationkey   intt,         
    s_phone       textt,        
    s_acctbal     numericc,     
    s_comment     textt         
);

CREATE TABLE partsupp (
    ps_partkey     intt,        
    ps_suppkey     intt,           
    ps_availqty    intt,        
    ps_supplycost  numericc,    
    ps_comment     textt        
);

CREATE TABLE customer (
    c_custkey     intt,         
    c_name        textt,        
    c_address     textt,        
    c_nationkey   intt,         
    c_phone       textt,        
    c_acctbal     numericc,     
    c_mktsegment  textt,        
    c_comment     textt         
);

CREATE TABLE orders (
    o_orderkey       intt,      
    o_custkey        intt,      
    o_orderstatus    textt,     
    o_totalprice     numericc,  
    o_orderdate      datee,     
    o_orderpriority  textt,     
    o_clerk          textt,     
    o_shippriority   intt,      
    o_comment        textt      
);

CREATE TABLE lineitem (
    l_orderkey       intt,      
    l_partkey        intt,      
    l_suppkey        intt,      
    l_linenumber     intt,      
    l_quantity       numericc,  
    l_extendedprice  numericc,  
    l_discount       numericc,  
    l_tax            numericc,  
    l_returnflag     textt,     
    l_linestatus     textt,     
    l_shipdate       datee,    
    l_commitdate     datee,    
    l_receiptdate    datee,    
    l_shipinstruct   textt,    
    l_shipmode       textt,    
    l_comment        textt     
);
