#!/bin/python3

import pandas as pd

ratios = [1]
sizes = [1000, 2000, 4000, 8000, 16000, 32000, 64000, 128000, 256000, 512000]

df = pd.DataFrame()
column_index = 0

for r in ratios:
    for s in sizes:
        marked = pd.read_csv('./results/' + str(s) + '_' + str(r) + '_marked.txt', header=None, skip_blank_lines=False)
        notmarked = pd.read_csv('./results/' + str(s) + '_' + str(r) + '_notmarked.txt', header=None, skip_blank_lines=False)
        df[column_index] = marked
        column_index += 1
        df[column_index] = notmarked
        column_index += 1
        df[column_index] = df[column_index-2]/df[column_index-1]
        column_index += 1

df.to_csv('combined_results.csv', header=None, index=None)