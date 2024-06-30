#!/usr/bin/bash

. exp_config.sh

BASE_PATH=$(pwd)

mkdir -p ./data

for size in "${sizes[@]}"; do
    # Generate data
    cd ${DBGEN_PATH}
    ./dbgen -s ${size}
    cd ${BASE_PATH}
    mkdir -p ./data/${size}GB
    mv ${DBGEN_PATH}/*.tbl ${BASE_PATH}/data/${size}GB

    for rate in "${rates[@]}"; do

        # Mark nulls
        cd ${BASE_PATH}/data/${size}GB
        ${BASE_PATH}/nullify2.py -n ${rate}

        # Prepare the databases
        rate_p=$(printf "%.0f" $(bc -l <<< "$rate * 100")) # rate in percentage
        if [ $base_sql = 1 ]; then
            dbname_bs=mn_${exp}_${size}_${rate_p}_base_sql
            dropdb --if-exists ${dbname_bs}
            createdb ${dbname_bs}
            psql ${dbname_bs} -f ${BASE_PATH}/schema.sql
        fi
        if [ $marked_sql = 1 ]; then
            dbname_ms=mn_${exp}_${size}_${rate_p}_marked_sql
            dropdb --if-exists ${dbname_ms}
            createdb ${dbname_ms}
            psql ${dbname_ms} -c "CREATE EXTENSION markednullcore;"
            psql ${dbname_ms} -f ${BASE_PATH}/schemaa.sql
        fi
        if [ $marked_marked = 1 ]; then
            dbname_mm=mn_${exp}_${size}_${rate_p}_marked_marked
            dropdb --if-exists ${dbname_mm}
            createdb ${dbname_mm}
            psql ${dbname_mm} -c "CREATE EXTENSION markednullcore;"
            psql ${dbname_mm} -f ${BASE_PATH}/schemaa.sql
        fi

        # Insert into database
        for table in "${tables[@]}"; do
            if [ $base_sql = 1 ]; then
                psql ${dbname_bs} -q -c "\\copy $table FROM '${BASE_PATH}/data/${size}GB/${table}_${rate}_notmarked.csv' CSV DELIMITER ',';"
            fi
            if [ $marked_sql = 1 ]; then
                psql ${dbname_ms} -q -c "\\copy $table FROM '${BASE_PATH}/data/${size}GB/${table}_${rate}_notmarked.csv' CSV DELIMITER ',';"
            fi
            if [ $marked_marked = 1 ]; then
                psql ${dbname_mm} -q -c "\\copy $table FROM '${BASE_PATH}/data/${size}GB/${table}_${rate}_marked.csv' CSV DELIMITER ',';"
            fi
        done
    done
done
