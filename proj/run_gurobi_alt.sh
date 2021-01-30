#!/bin/bash

skipping=0
if (( $# > 0 )); then
    skipping=1
fi

rm -f results.txt
for path in data/instances/valid/countries/*; do
    filename=$(basename $path)
    if [[ $skipping = 1 ]]; then
        if [[ $filename = $1 ]]; then
            skipping=0
        else
            continue
        fi
    fi
    echo $filename
    cnt=$(head -n 1 $path)
    echo $cnt
    if [[ cnt -lt 500 ]]; then
        output=$(./labeler -in $path -out /dev/null -sol ilpAlt)
        echo $output
        regex=$'([0-9]+)\t([0-9]+)'
        [[ $output =~ $regex ]]
        labels_set=${BASH_REMATCH[1]}
        time=${BASH_REMATCH[2]}
        echo "$filename;$labels_set;$time"
        echo "$filename;$labels_set;$time" >> results.txt
    fi
done
