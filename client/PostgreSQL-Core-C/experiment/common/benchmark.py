#!/usr/bin/python3

import argparse
import sys
import statistics
import psycopg2
import csv

class Benchmark:

    conn = None
    repetition = None

    def __init__(self, dbname, host, user, password, port) -> None:
        self.conn = psycopg2.connect(
            dbname=dbname,
            user=user,
            password=password,
            host=host,
            port=port
        )
    
    def set_repetition(self, repetition):
        self.repetition = repetition

    def query(self, sql, fetch=0):
        """
        Execute a SQL statement fetch (or ignore) the result.
        :param sql: the SQL statement of the query
        :param fetch: 0 for do not fetch, 1 for fetch the first result, 2 for fetch all the results
        :return: a list [success_flag, results] success_flag: 1 for success, -1 for not; results: a list of records
        """
        cur = self.conn.cursor()
        to_return = [1, None]
        try:
            cur.execute(sql)
            if fetch == 1:
                to_return[1] = cur.fetchone()[0]
            elif fetch == 2:
                to_return[1] = cur.fetchall()
        except:
            to_return[0] = -1
            self.conn.rollback()
        finally:
            cur.close()
            self.conn.commit()
        return to_return

    def query_get_time(self, sql):
        sql = 'EXPLAIN (ANALYZE, COSTS OFF, TIMING OFF) ' + sql
        time_list = []
        plan = None
        for i in range(self.repetition):
            results = self.query(sql, fetch=2)
            time = str(results[1][-1]).split('Time: ')[1].split(' ms')[0]
            time_list.append(float(time))
            plan = '\n'.join([x[0] for x in results[1]])
        median_time = statistics.median(time_list)
        return format(median_time, '.4f'), plan
    
    def query_get_results(self, sql):
        return self.query(sql, fetch=2)[1]


def main(args):

    benchmark = Benchmark(args.database, args.host, args.username, args.password, args.port)
    benchmark.set_repetition(args.repetition)

    # Run init queries
    init_path = '/'.join(args.filename.split('/')[:-1]) + "/init.sql"
    with open(init_path, 'r') as file:
        queries = file.readlines()
        for sql in queries:
            benchmark.query(sql, 0)

    query_times = []
    query_plans = []
    query_results = []
    with open(args.filename, 'r') as file:
        queries = file.readlines()
        query_count = len(queries)
        print(args.database + ':\t\t[' + '=' * query_count + ']', end='', flush=True)
        
        for i in range(query_count):
            sql = queries[i]
            try:
                query_time, query_plan = benchmark.query_get_time(sql)
            except:
                query_time = '0.000'
                query_plan = 'QUERY FAILED: "' + sql + '"'
                continue
            query_times.append(query_time)
            query_plans.append(query_plan)
            query_results.append(benchmark.query_get_results(sql))
            sys.stdout.write('\b' * (query_count + 1))
            sys.stdout.flush()
            print('>' * (i + 1) + '=' * (query_count - i - 1) + ']', end='', flush=True)
        print('')

    with open(args.output, 'w') as file:
        file.write('\n'.join(query_times))
        print('The results have been saved at \'' + args.output + '\'.')

    plan_path = args.output[:-4] + '_plan.txt'
    with open(plan_path, 'w') as file:
        query_no = 1
        for plan in query_plans:
            file.write('#' + str(query_no) + '\n')
            file.write(plan)
            file.write('\n')
            query_no += 1
        print('The plans have been saved at \'' + plan_path + '\'.')

    results_path = args.output[:-4] + '_results.txt'
    with open(results_path, 'w') as file:
        query_no = 1
        for result in query_results:
            file.write('#' + str(query_no) + '\n')
            writer = csv.writer(file, delimiter=',')
            for row in result:
                writer.writerow(row)
            file.write('\n')
            query_no += 1
        print('The results have been saved at \'' + results_path + '\'.')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        prog='simple_query_benchmark.benchmark',
        add_help=False,
        description='Run simple queries on nullified databases.'
    )
    parser.add_argument('-v', '--version',
                        action='version',
                        version='%(prog) v1 by Guozhi Wang')
    parser.add_argument('-h', '--help',
                        action='help',
                        help='print this help message and exit')
    parser.add_argument('-d', '--database',
                        type=str,
                        help='database name: such as "simple_10mb_1"')
    parser.add_argument('-H', '--host',
                        type=str,
                        default='localhost',
                        help='address of the database host: such as "localhost"')
    parser.add_argument('-u', '--username',
                        type=str,
                        default='guozhi',
                        help='username of the database')
    parser.add_argument('-p', '--password',
                        type=str,
                        default=None,
                        help='password of the database')
    parser.add_argument('-P', '--port',
                        type=int,
                        default=5432,
                        help='port of the database')
    parser.add_argument('-r', '--repetition',
                        type=int,
                        default=20,
                        choices=range(1, 101),
                        help='number of repetition: integer from 1 to 100')
    parser.add_argument('-f', '--filename',
                        type=str,
                        help='name of the sql file')
    parser.add_argument('-o', '--output',
                        type=str,
                        help='name of the output file')
    args = parser.parse_args()

    main(args)
