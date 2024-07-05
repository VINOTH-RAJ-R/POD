#!/usr/bin/bash

. exp_config.sh

mkdir -p ./data

for size in "${sizes[@]}"; do
    for rate in "${rates[@]}"; do

        # Prepare the databases
        rate_p=$(printf "%.0f" $(bc -l <<< "$rate * 100")) # rate in percentage
        if [ $base_sql = 1 ]; then
            dbname_bs=mn_${exp}_${size}_${rate_p}_base_sql
            dropdb --if-exists ${dbname_bs}
            createdb ${dbname_bs}
        fi
        if [ $marked_sql = 1 ]; then
            dbname_ms=mn_${exp}_${size}_${rate_p}_marked_sql
            dropdb --if-exists ${dbname_ms}
            createdb ${dbname_ms}
            psql ${dbname_ms} -c "CREATE EXTENSION markednullcore;"
        fi
        if [ $marked_marked = 1 ]; then
            dbname_mm=mn_${exp}_${size}_${rate_p}_marked_marked
            dropdb --if-exists ${dbname_mm}
            createdb ${dbname_mm}
            psql ${dbname_mm} -c "CREATE EXTENSION markednullcore;"
        fi

        # Generate data and mark the nulls (if null rate > 0)
        data_path=./data/table_${size}_${rate_p}.sql
        if [ "$rate" = "0" ]; then
            ${DF} --size=${size} ./schemas/schema_not_null.sql > $data_path
        else
            ${DF} --size=${size} --null=${rate} ./schemas/schema.sql > $data_path
        fi
        if [ $marked_marked = 1 ]; then
            ${MARKER} $data_path
            data_path_marked=./data/table_${size}_${rate_p}.marked.sql
        fi

        # Construct the data sql file and insert data into databases
        if [ $base_sql = 1 ]; then
            touch ${dbname_bs}.sql                          # Create a blank data sql file
            echo "BEGIN;" >> ${dbname_bs}.sql               #   Wrap sql with transaction
            cat ./schemas/schema.sql >> ${dbname_bs}.sql    #   Add table schema
            cat $data_path >> ${dbname_bs}.sql              #   Add insert statements
            echo "COMMIT;" >> ${dbname_bs}.sql              #   Commit the transaction
            psql ${dbname_bs} -f ${dbname_bs}.sql -q        # Execute the data sql file
            rm -f ${dbname_bs}.sql                          # Delete the data sql file
        fi
        if [ $marked_sql = 1 ]; then
            touch ${dbname_ms}.sql
            echo "BEGIN;" >> ${dbname_ms}.sql
            cat ./schemas/schemaa.sql >> ${dbname_ms}.sql
            cat $data_path >> ${dbname_ms}.sql
            echo "COMMIT;" >> ${dbname_ms}.sql
            psql ${dbname_ms} -f ${dbname_ms}.sql -q
            rm -f ${dbname_ms}.sql
        fi
        if [ $marked_marked = 1 ]; then
            touch ${dbname_mm}.sql
            echo "BEGIN;" >> ${dbname_mm}.sql
            cat ./schemas/schemaa.sql >> ${dbname_mm}.sql
            cat ${data_path_marked} >> ${dbname_mm}.sql
            echo "COMMIT;" >> ${dbname_mm}.sql
            psql ${dbname_mm} -f ${dbname_mm}.sql -q
            rm -f ${dbname_mm}.sql
        fi
    done
done

rm -r ./data
