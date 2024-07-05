#!/bin/python3

from copy import copy
import csv
import random
import argparse
from multiprocessing import Process
import enum
from re import S
import numpy as np

class SqlType(enum.Enum):
    INT = 1
    DATE = 2
    TEXT = 3
    NUMERIC = 4

TPCH_SCHEMA = {
    'part': [
        SqlType.INT,
        SqlType.TEXT,
        SqlType.TEXT,
        SqlType.TEXT,
        SqlType.TEXT,
        SqlType.INT,
        SqlType.TEXT,
        SqlType.NUMERIC,
        SqlType.TEXT
    ],
    'region': [
        SqlType.INT,
        SqlType.TEXT,
        SqlType.TEXT
    ],
    'nation': [
        SqlType.INT,
        SqlType.TEXT,
        SqlType.INT,
        SqlType.TEXT
    ],
    'supplier': [
        SqlType.INT,
        SqlType.TEXT,
        SqlType.TEXT,
        SqlType.INT,
        SqlType.TEXT,
        SqlType.NUMERIC,
        SqlType.TEXT
    ],
    'partsupp': [
        SqlType.INT,
        SqlType.INT,
        SqlType.INT,
        SqlType.NUMERIC,
        SqlType.TEXT
    ],
    'customer': [
        SqlType.INT,
        SqlType.TEXT,
        SqlType.TEXT,
        SqlType.INT,
        SqlType.TEXT,
        SqlType.NUMERIC,
        SqlType.TEXT,
        SqlType.TEXT
    ],
    'orders': [
        SqlType.INT,
        SqlType.INT,
        SqlType.TEXT,
        SqlType.NUMERIC,
        SqlType.DATE,
        SqlType.TEXT,
        SqlType.TEXT,
        SqlType.INT,
        SqlType.TEXT
    ],
    'lineitem': [
        SqlType.INT,
        SqlType.INT,
        SqlType.INT,
        SqlType.INT,
        SqlType.NUMERIC,
        SqlType.NUMERIC,
        SqlType.NUMERIC,
        SqlType.NUMERIC,
        SqlType.TEXT,
        SqlType.TEXT,
        SqlType.DATE,
        SqlType.DATE,
        SqlType.DATE,
        SqlType.TEXT,
        SqlType.TEXT,
        SqlType.TEXT
    ]
}

NOTNULL_COLS = {
    'part': [0], 'partsupp': [0, 1],
    'region': [0], 'customer': [0],
    'nation': [0], 'orders': [0],
    'supplier': [0], 'lineitem': [0, 3],
}

config = {
    'ratio': 10
}

class CounterThreadSafe:
    _counter = 0

    @staticmethod
    def get_counter():
        CounterThreadSafe._counter += 1
        return CounterThreadSafe._counter

def ratio_check(ratio):
    msg = "Null ratio musts be an interger between 0 and 100."
    try:
        ratio = int(ratio)
        if ratio >= 0 and ratio <= 100:
            return ratio
        else:
            raise argparse.ArgumentTypeError(msg)
    except:
        raise argparse.ArgumentTypeError(msg)

def nullify_constant(table, i, value):
    if TPCH_SCHEMA[table][i] == SqlType.INT:
        null_id = int(value) % 2**32
    elif TPCH_SCHEMA[table][i] == SqlType.NUMERIC:
        null_id = int(float(value)) % 2**32
    elif TPCH_SCHEMA[table][i] == SqlType.DATE:
        # use np.datetime64 because python built-in datetime does not support BC dates
        # 01 Jan 4713BC is Julian Day
        null_id = (np.datetime64(value) - np.datetime64('-4713-01-01')).astype(int)
    elif TPCH_SCHEMA[table][i] == SqlType.TEXT:
        null_id = hash(value) % 2**32

    if null_id == 0: # in case of 0
        return 'null:4294967295' # 2^32 - 1
    else:
        return 'null:' + str(null_id)

def nullify(table, ratio):
    with open(table + '.tbl', 'r') as tbl, \
         open(table + '_' + str(ratio) + '_notmarked.csv', 'w') as out, \
         open(table + '_' + str(ratio) + '_marked.csv', 'w') as outt:
         writer = csv.writer(out, quoting=csv.QUOTE_MINIMAL)
         writerr = csv.writer(outt, quoting=csv.QUOTE_MINIMAL)
         for line in csv.reader(tbl, delimiter='|'):
            line = line[:-1]
            linee = copy(line)
            for i, col in enumerate(line):
                if i in NOTNULL_COLS[table]:
                    continue
                if random.randint(1, 100) <= ratio:
                    line[i] = ""
                    linee[i] = nullify_constant(table, i, linee[i])
            writer.writerow(line)
            writerr.writerow(linee)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-n', '--nullratio', metavar='INT', type=ratio_check)
    args = parser.parse_args()
    ratio = args.nullratio
    proc = []
    for table in TPCH_SCHEMA.keys():
        p = Process(target=nullify, args=(table, ratio))
        proc.append(p)
        p.start()
    for p in proc:
        p.join()

if __name__ == "__main__":
    main()