#!/bin/bash

rm -f results.csv
for path in data/instances/valid/countries/*; do
    filename=$(basename $path)
    echo $filename
    cnt=$(head -n 1 $path)
    echo $cnt
    if [[ cnt -lt 300 ]]; then
        python3 generate_parameters.py | while read line ; do
            echo "Params: $line"
            cmd="./labeler -in $path -out /dev/null -sol gurobi -conf '$line'"
            echo $cmd
            output=$(./labeler -in $path -out /dev/null -sol gurobi -conf "$line")
            regex=$'([0-9]+)\t([0-9]+)'
            [[ $output =~ $regex ]]
            labels_set=${BASH_REMATCH[1]}
            time=${BASH_REMATCH[2]}
            echo "$filename, $labels_set, $time"
            echo "$filename, $labels_set, $time" >> results.csv
        done
    fi
done