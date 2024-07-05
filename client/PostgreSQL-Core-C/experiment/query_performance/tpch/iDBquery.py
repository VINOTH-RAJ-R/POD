import sys
import psycopg2
import datetime
import statistics


dbname = str(sys.argv[1])
user = str(sys.argv[2])
password = str(sys.argv[3])
host = str(sys.argv[4])
port = str(sys.argv[5])
run_times = 10
skipped_queries = [2, 15, 17, 20, 21]

queries = []
for i in range(23):
    with open('./queries/Q' + str(i) + '.sql', 'r') as file:
        lines = file.readlines()
        sql = ''.join(lines)
        queries.append(sql)

conn = psycopg2.connect(
    dbname=dbname,
    user=user,
    password=password,
    host=host,
    port=port
)


def query(sql, fetch=0):
    """
    Execute a SQL statement fetch (or ignore) the result.
    :param sql: the SQL statement of the query
    :param fetch: 0 for do not fetch, 1 for fetch the first result, 2 for fetch all the results
    :return: a list [success_flag, results] success_flag: 1 for success, -1 for not; results: a list of records
    """
    cur = conn.cursor()
    to_return = [1, None]
    try:
        cur.execute(sql)
        if fetch == 1:
            to_return[1] = cur.fetchone()[0]
        elif fetch == 2:
            to_return[1] = cur.fetchall()
    except:
        to_return[0] = -1
        conn.rollback()
    finally:
        cur.close()
        conn.commit()
    return to_return


def get_query_time(sql):
    sql = 'EXPLAIN (ANALYZE, COSTS OFF, TIMING OFF) ' + sql
    time_list = []
    for i in range(run_times):
        results = query(sql, fetch=2)
        time = str(results[1][-1]).split('Time: ')[1].split(' ms')[0]
        time_list.append(float(time))
    # avg_time = sum(time_list) / len(time_list)
    median_time = statistics.median(time_list)
    return format(median_time, '.4f')


def query_database():
    total_length = 22
    print(dbname + ':\t\t[' + '=' * total_length + ']', end='', flush=True)
    times = []
    query(queries[0], fetch=0)
    query('SET max_parallel_workers = 0;', fetch=0)
    query('SET max_parallel_workers_per_gather = 0;', fetch=0)

    for i in range(1, len(queries)):
        if i in skipped_queries:
            time = -999
        else:
            time = get_query_time(queries[i])
        times.append(time)
        sys.stdout.write('\b' * (total_length + 1))
        print('>' * (i + 1) + '=' * (total_length - i - 1) + ']', end='', flush=True)
    print('')

    return times


if __name__ == '__main__':

    marked = query_database()

    dbname = dbname[:-10] + 'base_sql'
    conn.close()
    conn = psycopg2.connect(
        dbname=dbname,
        user=user,
        password=password,
        host=host,
        port=port
    )
    notmarked = query_database()

    now = str(datetime.datetime.now()).replace(' ', '_').replace(':', '').split('.')[0]
    with open('./results/' + dbname[:-9] + now + '.csv', 'w') as file:
        file.write('query,marked,notmarked\n')
        for i in range(len(marked)):
            file.write(str(i+1) + ',')
            file.write(str(marked[i]) + ',')
            file.write(str(notmarked[i]) + ',')
            file.write(format(float(marked[i])/float(notmarked[i]) - 1, '.4f') + '\n')
    overheads = [float(marked[i])/float(notmarked[i])-1 for i in range(len(marked))]
    print(sum(overheads)/(len(overheads) - len(skipped_queries)))
