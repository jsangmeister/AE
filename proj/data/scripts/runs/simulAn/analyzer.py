import numpy as np
import scipy as scp
import matplotlib as mpl
mpl.rcParams['text.usetex'] = True
from matplotlib import pyplot as plt
import os
import csv

target = "CountriesRuns.csv"
countrie_sa_data = []
with open(target, 'r') as file:
    file.readline()
    for line in csv.reader(file):
        row = []
        row.append(line[0])
        row.append(int(line[1].strip()))
        row.append(float(line[2].strip()))
        row.append(line[3].strip())
        row.extend([int(x.strip()) for x in line[4:]])
        if row[3] == "log(n)*sqrt(n)" and row[6] != 0 and row[2]==1.0:
            countrie_sa_data.append(row)


target = "CountriesRef.csv"
countrie_ref_data = []
with open(target, 'r') as file:
    file.readline()
    for line in csv.reader(file):
        row = []
        row.append(line[0])
        row.extend([int(x.strip()) for x in line[1:]])
        countrie_ref_data.append(row)


target = "GeneratedRuns.csv"
generated_sa_data = []
with open(target, 'r') as file:
    file.readline()
    for line in csv.reader(file):
        row = []
        row.append(line[0])
        row.append(int(line[1].strip()))
        row.append(float(line[2].strip()))
        row.append(line[3].strip())
        row.extend([int(x.strip()) for x in line[4:]])
        if row[3] == "log(n)*sqrt(n)" and row[6] != 0 and row[2]==1.0:
            generated_sa_data.append(row)


target = "GeneratedRef.csv"
generated_ref_data = []
with open(target, 'r') as file:
    file.readline()
    for line in csv.reader(file):
        row = []
        row.append(line[0])
        row.extend([int(x.strip()) for x in line[1:]])
        generated_ref_data.append(row)
generated_ref_data = generated_ref_data[::-1]


if True:
    maxrun = None
    maxval = 0
    x = []
    y = []
    for run in countrie_sa_data:
        x.append(run[1])
        y.append(run[4]/run[6])
        if (run[4]/run[6]>maxval):
            maxval = run[4]/run[6]
            maxrun = run
    print(maxrun)
    plt.scatter(x, y)

    plt.xscale("log")
    #plt.yscale("log")
    #plt.xlim(0, 320)
    plt.xlabel("Number of Nodes")
    #plt.ylabel(r'$\frac{\text{sol}_\text{san}}{\text{sol}_\text{ref}}$')
    plt.ylabel(r'$\frac{sol_\mathrm{san}}{sol_\mathrm{ref}}$', fontsize = 16)
    plt.title("Cities Instances")
    plt.savefig("figures/countries_labs")
    plt.close()


if True:
    maxrun = None
    maxval = 0
    x = {1.0:[], 0.5:[], 0.3:[], 0.1:[], 0.01:[]}
    y = {1.0:[], 0.5:[], 0.3:[], 0.1:[], 0.01:[]}
    for run in generated_sa_data:
        d = float(run[0].split("_d")[1].replace("_", "."))
        x[d].append(run[1])
        y[d].append(run[4]/run[6])
        if (run[4]/run[6]>maxval):
            maxval = run[4]/run[6]
            maxrun = run
    #print(maxrun)
    legend = []
    for key in x.keys():
        legend.append(str(key))
        plt.scatter(x[key], y[key])
        plt.plot(x[key], y[key])
    #plt.xlim(0, 320)
    plt.xlabel("Number of Nodes")
    plt.xscale("log")
    #plt.ylabel(r'$\frac{\text{sol}_\text{san}}{\text{sol}_\text{ref}}$')
    plt.ylabel(r'$\frac{sol_\mathrm{san}}{sol_\mathrm{ref}}$', fontsize = 16)
    plt.legend(legend)
    plt.title("Generated Instances with different d")
    plt.savefig("figures/generated_labs")
    plt.close()

if True:
    maxrun = None
    maxval = 0
    x = []
    y = []
    for i in range(len(countrie_sa_data)):
        if(countrie_sa_data[i][5]==0):
            countrie_sa_data[i][5]=1
        if(countrie_ref_data[i][3]==0):
            countrie_ref_data[i][3]=1
        x.append(countrie_ref_data[i][1])
        y.append(countrie_sa_data[i][5]/countrie_ref_data[i][3])
    plt.scatter(x, y)
    #plt.xlim(0, 320)
    plt.xlabel("Number of Nodes")
    plt.xscale("log")
    plt.yscale("log")
    #plt.ylabel(r'$\frac{\text{sol}_\text{san}}{\text{sol}_\text{ref}}$')
    plt.ylabel(r'$\frac{time_\mathrm{san}}{time_\mathrm{ref}}$', fontsize = 16)
    plt.title("Cities Instances")
    plt.savefig("figures/countries_time_rel")
    plt.close()


if True:
    x = {1.0:[], 0.5:[], 0.3:[], 0.1:[], 0.01:[]}
    y = {1.0:[], 0.5:[], 0.3:[], 0.1:[], 0.01:[]}
    for i in range(len(generated_sa_data)):
        d = float(generated_sa_data[i][0].split("_d")[1].replace("_", "."))
        x[d].append(generated_sa_data[i][1])
        if(generated_sa_data[i][5]==0):
            generated_sa_data[i][5]=1
        if(generated_ref_data[i][3]==0):
            generated_ref_data[i][3]=1
        y[d].append(generated_sa_data[i][5]/generated_ref_data[i][3])
    #print(maxrun)
    legend = []
    for key in x.keys():
        legend.append(str(key))
        plt.scatter(x[key], y[key])
        plt.plot(x[key], y[key])
    #plt.xlim(0, 320)
    plt.xlabel("Number of Nodes")
    plt.xscale("log")
    plt.yscale("log")
    #plt.ylabel(r'$\frac{\text{sol}_\text{san}}{\text{sol}_\text{ref}}$')
    plt.ylabel(r'$\frac{time_\mathrm{san}}{time_\mathrm{ref}}$', fontsize = 16)
    plt.legend(legend)
    plt.title("Generated Instances with different d")
    plt.savefig("figures/generated_time_rel")
    plt.close()


if True:
    x = []
    y = []
    for i in range(len(countrie_sa_data)):
        if(countrie_sa_data[i][5]==0):
            countrie_sa_data[i][5]=1
        x.append(countrie_sa_data[i][1])
        y.append(countrie_sa_data[i][5])
    plt.scatter(x, y)
    #plt.xlim(0, 320)
    plt.xlabel("Number of Nodes")
    plt.xscale("log")
    plt.yscale("log")
    #plt.ylabel(r'$\frac{\text{sol}_\text{san}}{\text{sol}_\text{ref}}$')
    plt.ylabel(r'$time_\mathrm{san}$', fontsize = 12)
    plt.title("Cities Instances")
    plt.savefig("figures/countries_time_abs")
    plt.close()


if True:
    x = {1.0:[], 0.5:[], 0.3:[], 0.1:[], 0.01:[]}
    y = {1.0:[], 0.5:[], 0.3:[], 0.1:[], 0.01:[]}
    for i in range(len(generated_sa_data)):
        d = float(generated_sa_data[i][0].split("_d")[1].replace("_", "."))
        x[d].append(generated_sa_data[i][1])
        if(generated_sa_data[i][5]==0):
            generated_sa_data[i][5]=1
        if(generated_ref_data[i][3]==0):
            generated_ref_data[i][3]=1
        y[d].append(generated_sa_data[i][5])
    #print(maxrun)
    legend = []
    for key in x.keys():
        legend.append(str(key))
        plt.scatter(x[key], y[key])
        plt.plot(x[key], y[key])
    #plt.xlim(0, 320)
    plt.xlabel("Number of Nodes")
    plt.xscale("log")
    plt.yscale("log")
    #plt.ylabel(r'$\frac{\text{sol}_\text{san}}{\text{sol}_\text{ref}}$')
    plt.ylabel(r'$time_\mathrm{san}$', fontsize = 12)
    plt.legend(legend)
    plt.title("Generated Instances with different d")
    plt.savefig("figures/generated_time_abs")
    plt.close()