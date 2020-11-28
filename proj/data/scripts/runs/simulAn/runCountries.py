import os
import subprocess
import math
import time


runargs = ["../../../../labeler", "-in", "", "-out", "tmpdmp", "-sol", "sa", "-conf", ""]
data_path="../../../instances/valid/countries/"
with open("runs.csv", 'w') as outfile:
    outfile.write("file, n, temperature, max tries, labels sol, time, lables init, labels max during\n")
data_list = os.listdir(data_path)
data_list.sort()
for data in data_list:
    entries = 0
    with open(data_path+data, 'r') as rfile:
        entries = int(rfile.readline().strip())
    for temperature in [0.75, 0.9, 1.05, 1.2]:
        maxTries=math.log2(entries)
        runargs[2] = data_path+data
        runargs[8] = str(temperature) + " " + str(maxTries)
        p = subprocess.run(args=runargs, stdout=subprocess.PIPE, text=True)
        out = data + ", " + str(entries) + ", " + str(temperature) 
        out += ", log(n), " + str(p.stdout)
        print(out, end="")
        with open("runs.csv", 'a') as outfile:
            outfile.write(out)
        time.sleep(1) #reset random number generator in solver

        for d in [1, 2, 3]:
            maxTries=d*math.sqrt(entries)
            runargs[2] = data_path + data
            runargs[8] = str(temperature) + " " + str(maxTries)
            p = subprocess.run(args=runargs, stdout=subprocess.PIPE, text=True)
            out = data + ", " + str(entries) + ", " + str(temperature) 
            out += ", " + str(d) + "*sqrt(n), " + str(p.stdout)
            print(out, end="")
            with open("runs.csv", 'a') as outfile:
                outfile.write(out)
            time.sleep(1)

        maxTries=math.log2(entries)*math.sqrt(entries)
        runargs[2] = data_path+data
        runargs[8] = str(temperature) + " " + str(maxTries)
        p = subprocess.run(args=runargs, stdout=subprocess.PIPE, text=True)
        out = data + ", " + str(entries) + ", " + str(temperature) 
        out += ", log(n)*sqrt(n), " + str(p.stdout)
        print(out, end="")
        with open("runs.csv", 'a') as outfile:
            outfile.write(out)
        time.sleep(1) #reset random number generator in solver