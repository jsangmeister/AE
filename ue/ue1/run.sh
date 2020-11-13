python3 generate_parameters.py | while read line ; do
    q=$(echo $line | cut -d " " -f1)
    args=$(echo $line | cut -d " " -f1 --complement)
    read -r -a args <<< "$args"

    cmd="./mergesort$q"

    date
    output=$(eval $cmd ${args[@]})
    err=$?
    if [ $err -ne 0 ]; then
        msg="error $err"
    else
        msg=$(echo $output | cut -d " " -f4)  # this is the time
    fi
    echo "$q ${args[@]} $msg" | tee -a results.txt
done
