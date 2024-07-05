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
    dv_version                text                   ,
    dv_create_date            date                          ,
    dv_create_time            time                          ,
    dv_cmdline_args           text                  
);

create table customer_address
(
    ca_address_sk             intt               not null,
    ca_address_id             text              not null,
    ca_street_number          text                      ,
    ca_street_name            text                   ,
    ca_street_type            text                      ,
    ca_suite_number           text                      ,
    ca_city                   text                   ,
    ca_county                 text                   ,
    ca_state                  text                       ,
    ca_zip                    text                      ,
    ca_country                text                   ,
    ca_gmt_offset             numeric                  ,
    ca_location_type          text                      ,
    primary key (ca_address_sk)
);

create table customer_demographics
(
    cd_demo_sk                intt               not null,
    cd_gender                 text                       ,
    cd_marital_status         text                       ,
    cd_education_status       text                      ,
    cd_purchase_estimate      intt                       ,
    cd_credit_rating          text                      ,
    cd_dep_count              intt                       ,
    cd_dep_employed_count     intt                       ,
    cd_dep_college_count      intt                       ,
    primary key (cd_demo_sk)
);

create table date_dim
(
    d_date_sk                 intt               not null,
    d_date_id                 text              not null,
    d_date                    date                          ,
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
    d_day_name                text                       ,
    d_quarter_name            text                       ,
    d_holiday                 text                       ,
    d_weekend                 text                       ,
    d_following_holiday       text                       ,
    d_first_dom               intt                       ,
    d_last_dom                intt                       ,
    d_same_day_ly             intt                       ,
    d_same_day_lq             intt                       ,
    d_current_day             text                       ,
    d_current_week            text                       ,
    d_current_month           text                       ,
    d_current_quarter         text                       ,
    d_current_year            text                       ,
    primary key (d_date_sk)
);

create table warehouse
(
    w_warehouse_sk            intt               not null,
    w_warehouse_id            text              not null,
    w_warehouse_name          text                   ,
    w_warehouse_sq_ft         intt                       ,
    w_street_number           text                      ,
    w_street_name             text                   ,
    w_street_type             text                      ,
    w_suite_number            text                      ,
    w_city                    text                   ,
    w_county                  text                   ,
    w_state                   text                       ,
    w_zip                     text                      ,
    w_country                 text                   ,
    w_gmt_offset              numeric                  ,
    primary key (w_warehouse_sk)
);

create table ship_mode
(
    sm_ship_mode_sk           intt               not null,
    sm_ship_mode_id           text              not null,
    sm_type                   text                      ,
    sm_code                   text                      ,
    sm_carrier                text                      ,
    sm_contract               text                      ,
    primary key (sm_ship_mode_sk)
);

create table time_dim
(
    t_time_sk                 intt               not null,
    t_time_id                 text              not null,
    t_time                    intt                       ,
    t_hour                    intt                       ,
    t_minute                  intt                       ,
    t_second                  intt                       ,
    t_am_pm                   text                       ,
    t_shift                   text                      ,
    t_sub_shift               text                      ,
    t_meal_time               text                      ,
    primary key (t_time_sk)
);

create table reason
(
    r_reason_sk               intt               not null,
    r_reason_id               text              not null,
    r_reason_desc             text                     ,
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
    i_item_id                 text              not null,
    i_rec_start_date          date                          ,
    i_rec_end_date            date                          ,
    i_item_desc               text                  ,
    i_current_price           numeric                  ,
    i_wholesale_cost          numeric                  ,
    i_brand_id                intt                       ,
    i_brand                   text                      ,
    i_class_id                intt                       ,
    i_class                   text                      ,
    i_category_id             intt                       ,
    i_category                text                      ,
    i_manufact_id             intt                       ,
    i_manufact                text                      ,
    i_size                    text                      ,
    i_formulation             text                      ,
    i_color                   text                      ,
    i_units                   text                      ,
    i_container               text                      ,
    i_manager_id              intt                       ,
    i_product_name            text                      ,
    primary key (i_item_sk)
);

create table store
(
    s_store_sk                intt               not null,
    s_store_id                text              not null,
    s_rec_start_date          date                          ,
    s_rec_end_date            date                          ,
    s_closed_date_sk          intt                       ,
    s_store_name              text                   ,
    s_number_employees        intt                       ,
    s_floor_space             intt                       ,
    s_hours                   text                      ,
    s_manager                 text                   ,
    s_market_id               intt                       ,
    s_geography_class         text                  ,
    s_market_desc             text                  ,
    s_market_manager          text                   ,
    s_division_id             intt                       ,
    s_division_name           text                   ,
    s_company_id              intt                       ,
    s_company_name            text                   ,
    s_street_number           text                   ,
    s_street_name             text                   ,
    s_street_type             text                      ,
    s_suite_number            text                      ,
    s_city                    text                   ,
    s_county                  text                   ,
    s_state                   text                       ,
    s_zip                     text                      ,
    s_country                 text                   ,
    s_gmt_offset              numeric                  ,
    s_tax_precentage          numeric                  ,
    primary key (s_store_sk)
);

create table call_center
(
    cc_call_center_sk         intt               not null,
    cc_call_center_id         text              not null,
    cc_rec_start_date         date                          ,
    cc_rec_end_date           date                          ,
    cc_closed_date_sk         intt                       ,
    cc_open_date_sk           intt                       ,
    cc_name                   text                   ,
    cc_class                  text                   ,
    cc_employees              intt                       ,
    cc_sq_ft                  intt                       ,
    cc_hours                  text                      ,
    cc_manager                text                   ,
    cc_mkt_id                 intt                       ,
    cc_mkt_class              text                      ,
    cc_mkt_desc               text                  ,
    cc_market_manager         text                   ,
    cc_division               intt                       ,
    cc_division_name          text                   ,
    cc_company                intt                       ,
    cc_company_name           text                      ,
    cc_street_number          text                      ,
    cc_street_name            text                   ,
    cc_street_type            text                      ,
    cc_suite_number           text                      ,
    cc_city                   text                   ,
    cc_county                 text                   ,
    cc_state                  text                       ,
    cc_zip                    text                      ,
    cc_country                text                   ,
    cc_gmt_offset             numeric                  ,
    cc_tax_percentage         numeric                  ,
    primary key (cc_call_center_sk)
);

create table customer
(
    c_customer_sk             intt               not null,
    c_customer_id             text              not null,
    c_current_cdemo_sk        intt                       ,
    c_current_hdemo_sk        intt                       ,
    c_current_addr_sk         intt                       ,
    c_first_shipto_date_sk    intt                       ,
    c_first_sales_date_sk     intt                       ,
    c_salutation              text                      ,
    c_first_name              text                      ,
    c_last_name               text                      ,
    c_preferred_cust_flag     text                       ,
    c_birth_day               intt                       ,
    c_birth_month             intt                       ,
    c_birth_year              intt                       ,
    c_birth_country           text                   ,
    c_login                   text                      ,
    c_email_address           text                      ,
    c_last_review_date        text                      ,
    primary key (c_customer_sk)
);

create table web_site
(
    web_site_sk               intt               not null,
    web_site_id               text              not null,
    web_rec_start_date        date                          ,
    web_rec_end_date          date                          ,
    web_name                  text                   ,
    web_open_date_sk          intt                       ,
    web_close_date_sk         intt                       ,
    web_class                 text                   ,
    web_manager               text                   ,
    web_mkt_id                intt                       ,
    web_mkt_class             text                   ,
    web_mkt_desc              text                  ,
    web_market_manager        text                   ,
    web_company_id            intt                       ,
    web_company_name          text                      ,
    web_street_number         text                      ,
    web_street_name           text                   ,
    web_street_type           text                      ,
    web_suite_number          text                      ,
    web_city                  text                   ,
    web_county                text                   ,
    web_state                 text                       ,
    web_zip                   text                      ,
    web_country               text                   ,
    web_gmt_offset            numeric                  ,
    web_tax_percentage        numeric                  ,
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
    sr_return_amt             numeric                  ,
    sr_return_tax             numeric                  ,
    sr_return_amt_inc_tax     numeric                  ,
    sr_fee                    numeric                  ,
    sr_return_ship_cost       numeric                  ,
    sr_refunded_cash          numeric                  ,
    sr_reversed_charge        numeric                  ,
    sr_store_credit           numeric                  ,
    sr_net_loss               numeric                  ,
    primary key (sr_item_sk, sr_ticket_number)
);

create table household_demographics
(
    hd_demo_sk                intt               not null,
    hd_income_band_sk         intt                       ,
    hd_buy_potential          text                      ,
    hd_dep_count              intt                       ,
    hd_vehicle_count          intt                       ,
    primary key (hd_demo_sk)
);

create table web_page
(
    wp_web_page_sk            intt               not null,
    wp_web_page_id            text              not null,
    wp_rec_start_date         date                          ,
    wp_rec_end_date           date                          ,
    wp_creation_date_sk       intt                       ,
    wp_access_date_sk         intt                       ,
    wp_autogen_flag           text                       ,
    wp_customer_sk            intt                       ,
    wp_url                    text                  ,
    wp_type                   text                      ,
    wp_char_count             intt                       ,
    wp_link_count             intt                       ,
    wp_image_count            intt                       ,
    wp_max_ad_count           intt                       ,
    primary key (wp_web_page_sk)
);

create table promotion
(
    p_promo_sk                intt               not null,
    p_promo_id                text              not null,
    p_start_date_sk           intt                       ,
    p_end_date_sk             intt                       ,
    p_item_sk                 intt                       ,
    p_cost                    numeric                 ,
    p_response_target         intt                       ,
    p_promo_name              text                      ,
    p_channel_dmail           text                       ,
    p_channel_email           text                       ,
    p_channel_catalog         text                       ,
    p_channel_tv              text                       ,
    p_channel_radio           text                       ,
    p_channel_press           text                       ,
    p_channel_event           text                       ,
    p_channel_demo            text                       ,
    p_channel_details         text                  ,
    p_purpose                 text                      ,
    p_discount_active         text                       ,
    primary key (p_promo_sk)
);

create table catalog_page
(
    cp_catalog_page_sk        intt               not null,
    cp_catalog_page_id        text              not null,
    cp_start_date_sk          intt                       ,
    cp_end_date_sk            intt                       ,
    cp_department             text                   ,
    cp_catalog_number         intt                       ,
    cp_catalog_page_number    intt                       ,
    cp_description            text                  ,
    cp_type                   text                  ,
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
    cr_return_amount          numeric                  ,
    cr_return_tax             numeric                  ,
    cr_return_amt_inc_tax     numeric                  ,
    cr_fee                    numeric                  ,
    cr_return_ship_cost       numeric                  ,
    cr_refunded_cash          numeric                  ,
    cr_reversed_charge        numeric                  ,
    cr_store_credit           numeric                  ,
    cr_net_loss               numeric                  ,
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
    wr_return_amt             numeric                  ,
    wr_return_tax             numeric                  ,
    wr_return_amt_inc_tax     numeric                  ,
    wr_fee                    numeric                  ,
    wr_return_ship_cost       numeric                  ,
    wr_refunded_cash          numeric                  ,
    wr_reversed_charge        numeric                  ,
    wr_account_credit         numeric                  ,
    wr_net_loss               numeric                  ,
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
    ws_wholesale_cost         numeric                  ,
    ws_list_price             numeric                  ,
    ws_sales_price            numeric                  ,
    ws_ext_discount_amt       numeric                  ,
    ws_ext_sales_price        numeric                  ,
    ws_ext_wholesale_cost     numeric                  ,
    ws_ext_list_price         numeric                  ,
    ws_ext_tax                numeric                  ,
    ws_coupon_amt             numeric                  ,
    ws_ext_ship_cost          numeric                  ,
    ws_net_paid               numeric                  ,
    ws_net_paid_inc_tax       numeric                  ,
    ws_net_paid_inc_ship      numeric                  ,
    ws_net_paid_inc_ship_tax  numeric                  ,
    ws_net_profit             numeric                  ,
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
    cs_wholesale_cost         numeric                  ,
    cs_list_price             numeric                  ,
    cs_sales_price            numeric                  ,
    cs_ext_discount_amt       numeric                  ,
    cs_ext_sales_price        numeric                  ,
    cs_ext_wholesale_cost     numeric                  ,
    cs_ext_list_price         numeric                  ,
    cs_ext_tax                numeric                  ,
    cs_coupon_amt             numeric                  ,
    cs_ext_ship_cost          numeric                  ,
    cs_net_paid               numeric                  ,
    cs_net_paid_inc_tax       numeric                  ,
    cs_net_paid_inc_ship      numeric                  ,
    cs_net_paid_inc_ship_tax  numeric                  ,
    cs_net_profit             numeric                  ,
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
    ss_wholesale_cost         numeric                  ,
    ss_list_price             numeric                  ,
    ss_sales_price            numeric                  ,
    ss_ext_discount_amt       numeric                  ,
    ss_ext_sales_price        numeric                  ,
    ss_ext_wholesale_cost     numeric                  ,
    ss_ext_list_price         numeric                  ,
    ss_ext_tax                numeric                  ,
    ss_coupon_amt             numeric                  ,
    ss_net_paid               numeric                  ,
    ss_net_paid_inc_tax       numeric                  ,
    ss_net_profit             numeric                  ,
    primary key (ss_item_sk, ss_ticket_number)
);

