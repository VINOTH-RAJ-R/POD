#!/usr/bin/bash

### Configurations of the experiment

# Title of the experiment (will be included in database names)
exp="tpch"

# Path for 'dbgen' folder (TPC-H tool, see https://www.tpc.org/tpch/)
DBGEN_PATH=~/TPC-H/dbgen/

# Sizes of TPC-H datasets in GB
sizes=("1")

# Null rates between 0 to 1 (eg. 0, 0.4, 0.9, 1)
rates=("0")

# Types of the data to populate, 1 to enable, 0 to disable
# <type of constants>_<type of nulls>
base_sql=1
marked_sql=1
marked_marked=0

# Repetition of queries, a medium time will be recorded
repetition=1

#########################################
# The followings should not be changed. #
#########################################

### The followings are included in this repo
MARKER=../../common/mark_nulls.py
QUERY=../../common/benchmark.py

### TPC-H related arguments
tables=("customer" "lineitem" "nation" "orders" "part" "partsupp" "region" "supplier")

echo config_loaded
