#!/usr/bin/bash

. exp_config.sh

# mkdir -p ./results

for size in "${sizes[@]}"; do
    for rate in "${rates[@]}"; do
        rate_p=$(printf "%.0f" $(bc -l <<< "$rate * 100")) # rate in percentage
        dbname_bs=mn_${exp}_${size}_${rate_p}_base_sql
        dbname_ms=mn_${exp}_${size}_${rate_p}_marked_sql
        sql="select '${size}' size_in_GB, pg_size_pretty(bs) notmarked, pg_size_pretty(ms) marked, (ms::numeric/bs)-1::numeric overhead from (select pg_database_size('${dbname_bs}') bs, pg_database_size('${dbname_ms}') ms) t;"

        psql -c "$sql"

        
        # if [ $base_sql = 1 ]; then
        #     dbname_bs=mn_${exp}_${size}_${rate_p}_base_sql
        #     $QUERY -d ${dbname_bs} -f ./queries/notmarked.sql -o ./results/${dbname_bs}.txt -r $repetition
        # fi
        # if [ $marked_sql = 1 ]; then
        #     dbname_ms=mn_${exp}_${size}_${rate_p}_marked_sql
        #     $QUERY -d ${dbname_ms} -f ./queries/marked.sql -o ./results/${dbname_ms}.txt -r $repetition
        # fi
        # if [ $marked_marked = 1 ]; then
        #     dbname_mm=mn_${exp}_${size}_${rate_p}_marked_marked
        #     $QUERY -d ${dbname_mm} -f ./queries/marked.sql -o ./results/${dbname_mm}.txt -r $repetition
        # fi
    done
done
