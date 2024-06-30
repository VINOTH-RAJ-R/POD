#!/usr//bin/bash

# Requirements:
#	dsdgen			- A TPC-DS tool. See http://www.tpc.org/tpcds/. Not provided within this repo.
#	PostgreSQL		- A relational database server. See https://www.postgresql.org. Not provided within this repo.
#                     Please make sure that 'psql', 'createdb' and 'dropdb' are in the PATH.

TPCDS_PATH=~/TPC-DS/tools # Path for TPC-DS (dsdgen)
BASE_PATH=$(pwd)

mkdir -p ${BASE_PATH}/data

# Populate datasets using dsdgen
cd $TPCDS_PATH
./dsdgen -DIR ${BASE_PATH}/data -FORCE

# Create database and tables
dbname_bs='tpcds_1gb_notmarked'
dbname_ms='tpcds_1gb_marked'
dropdb --if-exists ${dbname_bs}
dropdb --if-exists ${dbname_ms}
createdb ${dbname_bs}
createdb ${dbname_ms}
psql ${dbname_bs} -f ${TPCDS_PATH}/'tpcds.sql'
psql ${dbname_ms} -c 'CREATE EXTENSION markednullcore;'
psql ${dbname_ms} -f ${BASE_PATH}/'tpcdss.sql'

# Load data into the databases
cd ${BASE_PATH}/data
for i in ./*; do
    table=${i##*/}
    table=${table/.dat/}
    sed 's/|$//' $i > /tmp/$i
    psql ${dbname_bs} -q -c "TRUNCATE $table"
    psql ${dbname_bs} -c "\\copy $table FROM '/tmp/$i' CSV DELIMITER '|' ENCODING 'latin1'"
    psql ${dbname_ms} -q -c "TRUNCATE $table"
    psql ${dbname_ms} -c "\\copy $table FROM '/tmp/$i' CSV DELIMITER '|' ENCODING 'latin1'"
done

rm -rf ./data
