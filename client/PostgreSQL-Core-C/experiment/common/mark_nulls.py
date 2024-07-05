#! /usr/bin/python3

import random
import sys

NULL_INDICATOR = '\\N'

class CounterThreadSafe:

    _counter = 0

    @staticmethod
    def count():
        CounterThreadSafe._counter += 1
        return str(CounterThreadSafe._counter)

filename = sys.argv[1]


with open(filename, 'r') as sql, open(filename[:-3]+'marked.sql', 'w') as sqll:
    null_count = 0
    for line in sql:
        if line.find(NULL_INDICATOR) != -1:
            words = line.split(NULL_INDICATOR)
            line = ''
            for i in range(len(words) - 1):
                line += words[i]
                line += 'null:' + CounterThreadSafe.count()
            line += words[-1]
        sqll.write(line)
