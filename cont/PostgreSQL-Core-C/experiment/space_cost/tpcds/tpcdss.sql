-- 
-- Legal Notice 
-- 
-- This document and associated source code (the "Work") is a part of a 
-- benchmark specification maintained by the TPC. 
-- 
-- The TPC reserves all right, title, and interest to the Work as provided 
-- under U.S. and international laws, including without limitation all patent 
-- and trademark rights therein. 
-- 
-- No Warranty 
-- 
-- 1.1 TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THE INFORMATION 
--     CONTAINED HEREIN IS PROVIDED "AS IS" AND WITH ALL FAULTS, AND THE 
--     AUTHORS AND DEVELOPERS OF THE WORK HEREBY DISCLAIM ALL OTHER 
--     WARRANTIES AND CONDITIONS, EITHER EXPRESS, IMPLIED OR STATUTORY, 
--     INCLUDING, BUT NOT LIMITED TO, ANY (IF ANY) IMPLIED WARRANTIES, 
--     DUTIES OR CONDITIONS OF MERCHANTABILITY, OF FITNESS FOR A PARTICULAR 
--     PURPOSE, OF ACCURACY OR COMPLETENESS OF RESPONSES, OF RESULTS, OF 
--     WORKMANLIKE EFFORT, OF LACK OF VIRUSES, AND OF LACK OF NEGLIGENCE. 
--     ALSO, THERE IS NO WARRANTY OR CONDITION OF TITLE, QUIET ENJOYMENT, 
--     QUIET POSSESSION, CORRESPONDENCE TO DESCRIPTION OR NON-INFRINGEMENT 
--     WITH REGARD TO THE WORK. 
-- 1.2 IN NO EVENT WILL ANY AUTHOR OR DEVELOPER OF THE WORK BE LIABLE TO 
--     ANY OTHER PARTY FOR ANY DAMAGES, INCLUDING BUT NOT LIMITED TO THE 
--     COST OF PROCURING SUBSTITUTE GOODS OR SERVICES, LOST PROFITS, LOSS 
--     OF USE, LOSS OF DATA, OR ANY INCIDENTAL, CONSEQUENTIAL, DIRECT, 
--     INDIRECT, OR SPECIAL DAMAGES WHETHER UNDER CONTRACT, TORT, WARRANTY,
--     OR OTHERWISE, ARISING IN ANY WAY OUT OF THIS OR ANY OTHER AGREEMENT 
--     RELATING TO THE WORK, WHETHER OR NOT SUCH AUTHOR OR DEVELOPER HAD 
--     ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES. 
-- 
-- Contributors:
-- Gradient Systems
--
create table dbgen_version
(
    dv_version                textt                   ,
    dv_create_date            datee                          ,
    dv_create_time            time                          ,
    dv_cmdline_args           textt                  
);

create table customer_address
(
    ca_address_sk             intt               not null,
    ca_address_id             textt              not null,
    ca_street_number          textt                      ,
    ca_street_name            textt                   ,
    ca_street_type            textt                      ,
    ca_suite_number           textt                      ,
    ca_city                   textt                   ,
    ca_county                 textt                   ,
    ca_state                  textt                       ,
    ca_zip                    textt                      ,
    ca_country                textt                   ,
    ca_gmt_offset             numericc                  ,
    ca_location_type          textt                      ,
    primary key (ca_address_sk)
);

create table customer_demographics
(
    cd_demo_sk                intt               not null,
    cd_gender                 textt                       ,
    cd_marital_status         textt                       ,
    cd_education_status       textt                      ,
    cd_purchase_estimate      intt                       ,
    cd_credit_rating          textt                      ,
    cd_dep_count              intt                       ,
    cd_dep_employed_count     intt                       ,
    cd_dep_college_count      intt                       ,
    primary key (cd_demo_sk)
);

create table date_dim
(
    d_date_sk                 intt               not null,
    d_date_id                 textt              not null,
    d_date                    datee                          ,
    d_month_seq               intt                       ,
    d_week_seq                intt                       ,
    d_quarter_seq             intt                       ,
    d_year                    intt                       ,
    d_dow                     intt                       ,
    d_moy                     intt                       ,
    d_dom                     intt                       ,
    d_qoy                     intt                       ,
    d_fy_year                 intt                       ,
    d_fy_quarter_seq          intt                       ,
    d_fy_week_seq             intt                       ,
    d_day_name                textt                       ,
    d_quarter_name            textt                       ,
    d_holiday                 textt                       ,
    d_weekend                 textt                       ,
    d_following_holiday       textt                       ,
    d_first_dom               intt                       ,
    d_last_dom                intt                       ,
    d_same_day_ly             intt                       ,
    d_same_day_lq             intt                       ,
    d_current_day             textt                       ,
    d_current_week            textt                       ,
    d_current_month           textt                       ,
    d_current_quarter         textt                       ,
    d_current_year            textt                       ,
    primary key (d_date_sk)
);

create table warehouse
(
    w_warehouse_sk            intt               not null,
    w_warehouse_id            textt              not null,
    w_warehouse_name          textt                   ,
    w_warehouse_sq_ft         intt                       ,
    w_street_number           textt                      ,
    w_street_name             textt                   ,
    w_street_type             textt                      ,
    w_suite_number            textt                      ,
    w_city                    textt                   ,
    w_county                  textt                   ,
    w_state                   textt                       ,
    w_zip                     textt                      ,
    w_country                 textt                   ,
    w_gmt_offset              numericc                  ,
    primary key (w_warehouse_sk)
);

create table ship_mode
(
    sm_ship_mode_sk           intt               not null,
    sm_ship_mode_id           textt              not null,
    sm_type                   textt                      ,
    sm_code                   textt                      ,
    sm_carrier                textt                      ,
    sm_contract               textt                      ,
    primary key (sm_ship_mode_sk)
);

create table time_dim
(
    t_time_sk                 intt               not null,
    t_time_id                 textt              not null,
    t_time                    intt                       ,
    t_hour                    intt                       ,
    t_minute                  intt                       ,
    t_second                  intt                       ,
    t_am_pm                   textt                       ,
    t_shift                   textt                      ,
    t_sub_shift               textt                      ,
    t_meal_time               textt                      ,
    primary key (t_time_sk)
);

create table reason
(
    r_reason_sk               intt               not null,
    r_reason_id               textt              not null,
    r_reason_desc             textt                     ,
    primary key (r_reason_sk)
);

create table income_band
(
    ib_income_band_sk         intt               not null,
    ib_lower_bound            intt                       ,
    ib_upper_bound            intt                       ,
    primary key (ib_income_band_sk)
);

create table item
(
    i_item_sk                 intt               not null,
    i_item_id                 textt              not null,
    i_rec_start_date          datee                          ,
    i_rec_end_date            datee                          ,
    i_item_desc               textt                  ,
    i_current_price           numericc                  ,
    i_wholesale_cost          numericc                  ,
    i_brand_id                intt                       ,
    i_brand                   textt                      ,
    i_class_id                intt                       ,
    i_class                   textt                      ,
    i_category_id             intt                       ,
    i_category                textt                      ,
    i_manufact_id             intt                       ,
    i_manufact                textt                      ,
    i_size                    textt                      ,
    i_formulation             textt                      ,
    i_color                   textt                      ,
    i_units                   textt                      ,
    i_container               textt                      ,
    i_manager_id              intt                       ,
    i_product_name            textt                      ,
    primary key (i_item_sk)
);

create table store
(
    s_store_sk                intt               not null,
    s_store_id                textt              not null,
    s_rec_start_date          datee                          ,
    s_rec_end_date            datee                          ,
    s_closed_date_sk          intt                       ,
    s_store_name              textt                   ,
    s_number_employees        intt                       ,
    s_floor_space             intt                       ,
    s_hours                   textt                      ,
    s_manager                 textt                   ,
    s_market_id               intt                       ,
    s_geography_class         textt                  ,
    s_market_desc             textt                  ,
    s_market_manager          textt                   ,
    s_division_id             intt                       ,
    s_division_name           textt                   ,
    s_company_id              intt                       ,
    s_company_name            textt                   ,
    s_street_number           textt                   ,
    s_street_name             textt                   ,
    s_street_type             textt                      ,
    s_suite_number            textt                      ,
    s_city                    textt                   ,
    s_county                  textt                   ,
    s_state                   textt                       ,
    s_zip                     textt                      ,
    s_country                 textt                   ,
    s_gmt_offset              numericc                  ,
    s_tax_precentage          numericc                  ,
    primary key (s_store_sk)
);

create table call_center
(
    cc_call_center_sk         intt               not null,
    cc_call_center_id         textt              not null,
    cc_rec_start_date         datee                          ,
    cc_rec_end_date           datee                          ,
    cc_closed_date_sk         intt                       ,
    cc_open_date_sk           intt                       ,
    cc_name                   textt                   ,
    cc_class                  textt                   ,
    cc_employees              intt                       ,
    cc_sq_ft                  intt                       ,
    cc_hours                  textt                      ,
    cc_manager                textt                   ,
    cc_mkt_id                 intt                       ,
    cc_mkt_class              textt                      ,
    cc_mkt_desc               textt                  ,
    cc_market_manager         textt                   ,
    cc_division               intt                       ,
    cc_division_name          textt                   ,
    cc_company                intt                       ,
    cc_company_name           textt                      ,
    cc_street_number          textt                      ,
    cc_street_name            textt                   ,
    cc_street_type            textt                      ,
    cc_suite_number           textt                      ,
    cc_city                   textt                   ,
    cc_county                 textt                   ,
    cc_state                  textt                       ,
    cc_zip                    textt                      ,
    cc_country                textt                   ,
    cc_gmt_offset             numericc                  ,
    cc_tax_percentage         numericc                  ,
    primary key (cc_call_center_sk)
);

create table customer
(
    c_customer_sk             intt               not null,
    c_customer_id             textt              not null,
    c_current_cdemo_sk        intt                       ,
    c_current_hdemo_sk        intt                       ,
    c_current_addr_sk         intt                       ,
    c_first_shipto_date_sk    intt                       ,
    c_first_sales_date_sk     intt                       ,
    c_salutation              textt                      ,
    c_first_name              textt                      ,
    c_last_name               textt                      ,
    c_preferred_cust_flag     textt                       ,
    c_birth_day               intt                       ,
    c_birth_month             intt                       ,
    c_birth_year              intt                       ,
    c_birth_country           textt                   ,
    c_login                   textt                      ,
    c_email_address           textt                      ,
    c_last_review_date        textt                      ,
    primary key (c_customer_sk)
);

create table web_site
(
    web_site_sk               intt               not null,
    web_site_id               textt              not null,
    web_rec_start_date        datee                          ,
    web_rec_end_date          datee                          ,
    web_name                  textt                   ,
    web_open_date_sk          intt                       ,
    web_close_date_sk         intt                       ,
    web_class                 textt                   ,
    web_manager               textt                   ,
    web_mkt_id                intt                       ,
    web_mkt_class             textt                   ,
    web_mkt_desc              textt                  ,
    web_market_manager        textt                   ,
    web_company_id            intt                       ,
    web_company_name          textt                      ,
    web_street_number         textt                      ,
    web_street_name           textt                   ,
    web_street_type           textt                      ,
    web_suite_number          textt                      ,
    web_city                  textt                   ,
    web_county                textt                   ,
    web_state                 textt                       ,
    web_zip                   textt                      ,
    web_country               textt                   ,
    web_gmt_offset            numericc                  ,
    web_tax_percentage        numericc                  ,
    primary key (web_site_sk)
);

create table store_returns
(
    sr_returned_date_sk       intt                       ,
    sr_return_time_sk         intt                       ,
    sr_item_sk                intt               not null,
    sr_customer_sk            intt                       ,
    sr_cdemo_sk               intt                       ,
    sr_hdemo_sk               intt                       ,
    sr_addr_sk                intt                       ,
    sr_store_sk               intt                       ,
    sr_reason_sk              intt                       ,
    sr_ticket_number          intt               not null,
    sr_return_quantity        intt                       ,
    sr_return_amt             numericc                  ,
    sr_return_tax             numericc                  ,
    sr_return_amt_inc_tax     numericc                  ,
    sr_fee                    numericc                  ,
    sr_return_ship_cost       numericc                  ,
    sr_refunded_cash          numericc                  ,
    sr_reversed_charge        numericc                  ,
    sr_store_credit           numericc                  ,
    sr_net_loss               numericc                  ,
    primary key (sr_item_sk, sr_ticket_number)
);

create table household_demographics
(
    hd_demo_sk                intt               not null,
    hd_income_band_sk         intt                       ,
    hd_buy_potential          textt                      ,
    hd_dep_count              intt                       ,
    hd_vehicle_count          intt                       ,
    primary key (hd_demo_sk)
);

create table web_page
(
    wp_web_page_sk            intt               not null,
    wp_web_page_id            textt              not null,
    wp_rec_start_date         datee                          ,
    wp_rec_end_date           datee                          ,
    wp_creation_date_sk       intt                       ,
    wp_access_date_sk         intt                       ,
    wp_autogen_flag           textt                       ,
    wp_customer_sk            intt                       ,
    wp_url                    textt                  ,
    wp_type                   textt                      ,
    wp_char_count             intt                       ,
    wp_link_count             intt                       ,
    wp_image_count            intt                       ,
    wp_max_ad_count           intt                       ,
    primary key (wp_web_page_sk)
);

create table promotion
(
    p_promo_sk                intt               not null,
    p_promo_id                textt              not null,
    p_start_date_sk           intt                       ,
    p_end_date_sk             intt                       ,
    p_item_sk                 intt                       ,
    p_cost                    numericc                 ,
    p_response_target         intt                       ,
    p_promo_name              textt                      ,
    p_channel_dmail           textt                       ,
    p_channel_email           textt                       ,
    p_channel_catalog         textt                       ,
    p_channel_tv              textt                       ,
    p_channel_radio           textt                       ,
    p_channel_press           textt                       ,
    p_channel_event           textt                       ,
    p_channel_demo            textt                       ,
    p_channel_details         textt                  ,
    p_purpose                 textt                      ,
    p_discount_active         textt                       ,
    primary key (p_promo_sk)
);

create table catalog_page
(
    cp_catalog_page_sk        intt               not null,
    cp_catalog_page_id        textt              not null,
    cp_start_date_sk          intt                       ,
    cp_end_date_sk            intt                       ,
    cp_department             textt                   ,
    cp_catalog_number         intt                       ,
    cp_catalog_page_number    intt                       ,
    cp_description            textt                  ,
    cp_type                   textt                  ,
    primary key (cp_catalog_page_sk)
);

create table inventory
(
    inv_date_sk               intt               not null,
    inv_item_sk               intt               not null,
    inv_warehouse_sk          intt               not null,
    inv_quantity_on_hand      intt                       ,
    primary key (inv_date_sk, inv_item_sk, inv_warehouse_sk)
);

create table catalog_returns
(
    cr_returned_date_sk       intt                       ,
    cr_returned_time_sk       intt                       ,
    cr_item_sk                intt               not null,
    cr_refunded_customer_sk   intt                       ,
    cr_refunded_cdemo_sk      intt                       ,
    cr_refunded_hdemo_sk      intt                       ,
    cr_refunded_addr_sk       intt                       ,
    cr_returning_customer_sk  intt                       ,
    cr_returning_cdemo_sk     intt                       ,
    cr_returning_hdemo_sk     intt                       ,
    cr_returning_addr_sk      intt                       ,
    cr_call_center_sk         intt                       ,
    cr_catalog_page_sk        intt                       ,
    cr_ship_mode_sk           intt                       ,
    cr_warehouse_sk           intt                       ,
    cr_reason_sk              intt                       ,
    cr_order_number           intt               not null,
    cr_return_quantity        intt                       ,
    cr_return_amount          numericc                  ,
    cr_return_tax             numericc                  ,
    cr_return_amt_inc_tax     numericc                  ,
    cr_fee                    numericc                  ,
    cr_return_ship_cost       numericc                  ,
    cr_refunded_cash          numericc                  ,
    cr_reversed_charge        numericc                  ,
    cr_store_credit           numericc                  ,
    cr_net_loss               numericc                  ,
    primary key (cr_item_sk, cr_order_number)
);

create table web_returns
(
    wr_returned_date_sk       intt                       ,
    wr_returned_time_sk       intt                       ,
    wr_item_sk                intt               not null,
    wr_refunded_customer_sk   intt                       ,
    wr_refunded_cdemo_sk      intt                       ,
    wr_refunded_hdemo_sk      intt                       ,
    wr_refunded_addr_sk       intt                       ,
    wr_returning_customer_sk  intt                       ,
    wr_returning_cdemo_sk     intt                       ,
    wr_returning_hdemo_sk     intt                       ,
    wr_returning_addr_sk      intt                       ,
    wr_web_page_sk            intt                       ,
    wr_reason_sk              intt                       ,
    wr_order_number           intt               not null,
    wr_return_quantity        intt                       ,
    wr_return_amt             numericc                  ,
    wr_return_tax             numericc                  ,
    wr_return_amt_inc_tax     numericc                  ,
    wr_fee                    numericc                  ,
    wr_return_ship_cost       numericc                  ,
    wr_refunded_cash          numericc                  ,
    wr_reversed_charge        numericc                  ,
    wr_account_credit         numericc                  ,
    wr_net_loss               numericc                  ,
    primary key (wr_item_sk, wr_order_number)
);

create table web_sales
(
    ws_sold_date_sk           intt                       ,
    ws_sold_time_sk           intt                       ,
    ws_ship_date_sk           intt                       ,
    ws_item_sk                intt               not null,
    ws_bill_customer_sk       intt                       ,
    ws_bill_cdemo_sk          intt                       ,
    ws_bill_hdemo_sk          intt                       ,
    ws_bill_addr_sk           intt                       ,
    ws_ship_customer_sk       intt                       ,
    ws_ship_cdemo_sk          intt                       ,
    ws_ship_hdemo_sk          intt                       ,
    ws_ship_addr_sk           intt                       ,
    ws_web_page_sk            intt                       ,
    ws_web_site_sk            intt                       ,
    ws_ship_mode_sk           intt                       ,
    ws_warehouse_sk           intt                       ,
    ws_promo_sk               intt                       ,
    ws_order_number           intt               not null,
    ws_quantity               intt                       ,
    ws_wholesale_cost         numericc                  ,
    ws_list_price             numericc                  ,
    ws_sales_price            numericc                  ,
    ws_ext_discount_amt       numericc                  ,
    ws_ext_sales_price        numericc                  ,
    ws_ext_wholesale_cost     numericc                  ,
    ws_ext_list_price         numericc                  ,
    ws_ext_tax                numericc                  ,
    ws_coupon_amt             numericc                  ,
    ws_ext_ship_cost          numericc                  ,
    ws_net_paid               numericc                  ,
    ws_net_paid_inc_tax       numericc                  ,
    ws_net_paid_inc_ship      numericc                  ,
    ws_net_paid_inc_ship_tax  numericc                  ,
    ws_net_profit             numericc                  ,
    primary key (ws_item_sk, ws_order_number)
);

create table catalog_sales
(
    cs_sold_date_sk           intt                       ,
    cs_sold_time_sk           intt                       ,
    cs_ship_date_sk           intt                       ,
    cs_bill_customer_sk       intt                       ,
    cs_bill_cdemo_sk          intt                       ,
    cs_bill_hdemo_sk          intt                       ,
    cs_bill_addr_sk           intt                       ,
    cs_ship_customer_sk       intt                       ,
    cs_ship_cdemo_sk          intt                       ,
    cs_ship_hdemo_sk          intt                       ,
    cs_ship_addr_sk           intt                       ,
    cs_call_center_sk         intt                       ,
    cs_catalog_page_sk        intt                       ,
    cs_ship_mode_sk           intt                       ,
    cs_warehouse_sk           intt                       ,
    cs_item_sk                intt               not null,
    cs_promo_sk               intt                       ,
    cs_order_number           intt               not null,
    cs_quantity               intt                       ,
    cs_wholesale_cost         numericc                  ,
    cs_list_price             numericc                  ,
    cs_sales_price            numericc                  ,
    cs_ext_discount_amt       numericc                  ,
    cs_ext_sales_price        numericc                  ,
    cs_ext_wholesale_cost     numericc                  ,
    cs_ext_list_price         numericc                  ,
    cs_ext_tax                numericc                  ,
    cs_coupon_amt             numericc                  ,
    cs_ext_ship_cost          numericc                  ,
    cs_net_paid               numericc                  ,
    cs_net_paid_inc_tax       numericc                  ,
    cs_net_paid_inc_ship      numericc                  ,
    cs_net_paid_inc_ship_tax  numericc                  ,
    cs_net_profit             numericc                  ,
    primary key (cs_item_sk, cs_order_number)
);

create table store_sales
(
    ss_sold_date_sk           intt                       ,
    ss_sold_time_sk           intt                       ,
    ss_item_sk                intt               not null,
    ss_customer_sk            intt                       ,
    ss_cdemo_sk               intt                       ,
    ss_hdemo_sk               intt                       ,
    ss_addr_sk                intt                       ,
    ss_store_sk               intt                       ,
    ss_promo_sk               intt                       ,
    ss_ticket_number          intt               not null,
    ss_quantity               intt                       ,
    ss_wholesale_cost         numericc                  ,
    ss_list_price             numericc                  ,
    ss_sales_price            numericc                  ,
    ss_ext_discount_amt       numericc                  ,
    ss_ext_sales_price        numericc                  ,
    ss_ext_wholesale_cost     numericc                  ,
    ss_ext_list_price         numericc                  ,
    ss_ext_tax                numericc                  ,
    ss_coupon_amt             numericc                  ,
    ss_net_paid               numericc                  ,
    ss_net_paid_inc_tax       numericc                  ,
    ss_net_profit             numericc                  ,
    primary key (ss_item_sk, ss_ticket_number)
);

