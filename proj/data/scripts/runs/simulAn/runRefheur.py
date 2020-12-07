import os
import subprocess
import math
import time


runargs = ["../../../../labeler", "-in", "", "-out", "tmpdmp", "-sol", "ref"]
data_path="../../../instances/valid/generated/"
#data_path="../../../instances/valid/countries/"
with open("runs.csv", 'w') as outfile:
    outfile.write("file, n, labels sol, time\n")
data_list = os.listdir(data_path)
data_list.sort()
for data in data_list:
    entries = 0
    with open(data_path+data, 'r') as rfile:
        entries = int(rfile.readline().strip())
    runargs[2] = data_path+data
    p = subprocess.run(args=runargs, stdout=subprocess.PIPE, text=True)
    out = data + ", " +  str(p.stdout)
    print(out, end="")
    with open("runs.csv", 'a') as outfile:
        outfile.write(out)
    time.sleep(1)
