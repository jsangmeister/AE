#!/bin/bash

rm -f results.txt
for filename in data/instances/valid/countries/*; do
    echo $filename
    # cnt=$(head -n 1 $filename)
    # echo $cnt
    # if [[ cnt -lt 1000 ]]; then
        output=$(./labeler -in $filename -out sol.txt -sol rules)
        echo $output
        regex='Solution with ([0-9]+) labels found \(([0-9]+)ms\)'
        [[ $output =~ $regex ]]
        labels_set=${BASH_REMATCH[1]}
        time=${BASH_REMATCH[2]}
        echo "$(basename $filename);$labels_set;$time"
        echo "$(basename $filename);$labels_set;$time" >> results.txt
    # fi
done
