#!/usr/bin/bash

. exp_config.sh

mkdir -p ./results/
for size in "${sizes[@]}"; do
    for rate in "${rates[@]}"; do
        rate_p=$(printf "%.0f" $(bc -l <<< "$rate * 100")) # rate in percentage
        dbname_ms=mn_${exp}_${size}_${rate_p}_marked_sql
        # it will also executes queries on the notmarked database
        python3 ./iDBquery.py ${dbname_ms} guozhi 12345678 localhost 5432
    done
done
