#!/bin/bash

execute="psql -c"
$execute "DROP EXTENSION IF EXISTS markednullcore CASCADE;"
sudo make install
$execute "CREATE EXTENSION markednullcore;"
make clean
