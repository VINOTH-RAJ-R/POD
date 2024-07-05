#!/usr/bin/bash

### Configurations of the experiment

# Title of the experiment (will be included in database names)
exp="groupby"

# Datafiller python file. See https://www.cri.ensmp.fr/people/coelho/datafiller.html
DF=~/datafiller-master/datafiller

# Sizes in row count
sizes=("50000")

# Null rates in decimal
rates=("0" "0.3" "0.6" "0.9")

# Types of the data to populate, 1 to enable, 0 to disable
# <type of constants>_<type of nulls>
base_sql=1
marked_sql=1
marked_marked=1

# Repetition of queries, a medium time will be recorded
repetition=20

### The followings are included in this repo.
MARKER=../../common/mark_nulls.py
QUERY=../../common/benchmark.py

echo config_loaded