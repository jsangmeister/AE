python3 generate_parameters.py | while read line ; do
    q=$(echo $line | cut -d " " -f1)
    args=$(echo $line | cut -d " " -f1 --complement)
    read -r -a args <<< "$args"

    cmd="./mergesort$q"

    output=$(eval $cmd ${args[@]})
    time=$(echo $output | cut -d " " -f4)
    echo "$q ${args[@]} $time" | tee -a results.txt
done