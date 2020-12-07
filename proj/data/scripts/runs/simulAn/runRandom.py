import os
import subprocess
import math
import time


runargs = ["../../../../labeler", "-in", "", "-out", "tmpdmp", "-sol", "sa", "-conf", ""]
data_path="../../../instances/valid/generated/"
with open("runsR.csv", 'w') as outfile:
    outfile.write("file, n, temperature, max tries, labels sol, time, lables init, labels max during\n")
data_list = os.listdir(data_path)
data_list.sort()
for data in data_list[::-1]:
    entries = 0
    with open(data_path+data, 'r') as rfile:
        entries = int(rfile.readline().strip())
    for randStart in [1.0]:
        for temperature in [1.0]:
            maxTries=math.log2(entries)*math.sqrt(entries)
            runargs[2] = data_path+data
            runargs[8] = str(temperature) + " " + str(maxTries) + " " + str(randStart)
            p = subprocess.run(args=runargs, stdout=subprocess.PIPE, text=True)
            out = data + ", " + str(entries) + ", " + str(temperature) 
            out += ", log(n)*sqrt(n), " + str(p.stdout)
            print(out, end="")
            with open("runs.csv", 'a') as outfile:
                outfile.write(out)
            time.sleep(1) #reset random number generator in solver
