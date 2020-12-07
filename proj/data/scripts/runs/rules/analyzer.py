import numpy as np
import scipy as scp
import matplotlib as mpl
mpl.rcParams['text.usetex'] = True
from matplotlib import pyplot as plt
import os
import csv

target = "results_rules.csv"
countrie_rules_data = []
with open(target, 'r') as file:
    file.readline()
    for line in csv.reader(file):
        row = []
        row.append(line[0])
        row.extend([int(x.strip()) for x in line[1:]])
        countrie_rules_data.append(row)


target = "../simulAn/CountriesRuns.csv"
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


countries_sa_best_runs_dict = {}
for run in countrie_sa_data:
    if run[0] not in countries_sa_best_runs_dict:
        countries_sa_best_runs_dict[run[0]] = run
    else:
        if run[4] > countries_sa_best_runs_dict[run[0]][4]:
            countries_sa_best_runs_dict[run[0]] = run


target = "../simulAn/CountriesRef.csv"
countrie_ref_data = []
with open(target, 'r') as file:
    file.readline()
    for line in csv.reader(file):
        row = []
        row.append(line[0])
        row.extend([int(x.strip()) for x in line[1:]])
        countrie_ref_data.append(row)


# rules vs ref
if False:
    x = []
    y = []
    for run in countrie_rules_data:
        sa_entry = countries_sa_best_runs_dict[run[0]]
        x.append(sa_entry[1])
        y.append(run[1]/sa_entry[6])
    plt.scatter(x, y)

    plt.xscale("log")
    #plt.yscale("log")
    # plt.xlim(1, 1320)
    plt.xlabel("Number of Nodes")
    #plt.ylabel(r'$\frac{\text{sol}_\text{san}}{\text{sol}_\text{ref}}$')
    plt.ylabel(r'$\frac{sol_\mathrm{rules}}{sol_\mathrm{ref}}$', fontsize = 16)
    plt.title("Cities Instances")
    plt.savefig("figures/value_rules_vs_ref")
    plt.close()


# rules vs. san
if False:
    x = []
    y = []
    for run in countrie_rules_data:
        sa_entry = countries_sa_best_runs_dict[run[0]]
        x.append(sa_entry[1])
        y.append(run[1]/sa_entry[4])
    plt.axhline(y=1, color='r', linestyle='-')
    plt.scatter(x, y)

    plt.xscale("log")
    #plt.yscale("log")
    # plt.xlim(1, 1320)
    plt.xlabel("Number of Nodes")
    #plt.ylabel(r'$\frac{\text{sol}_\text{san}}{\text{sol}_\text{ref}}$')
    plt.ylabel(r'$\frac{sol_\mathrm{rules}}{sol_\mathrm{san}}$', fontsize = 16)
    plt.title("Cities Instances")
    plt.savefig("figures/value_rules_vs_san")
    plt.close()


# rules vs. san (time)
if True:
    x = []
    y = []
    for run in countrie_rules_data:
        sa_entry = countries_sa_best_runs_dict[run[0]]
        x.append(sa_entry[1])
        if sa_entry[5] == 0:
            if run[2] == 0:
                y.append(1)
            else:
                del x[-1]
        else:
            y.append(run[2]/sa_entry[5])
    plt.axhline(y=1, color='r', linestyle='-')
    plt.scatter(x, y)

    #plt.xlim(0, 320)
    plt.xlabel("Number of Nodes")
    plt.xscale("log")
    # plt.yscale("log")
    #plt.ylabel(r'$\frac{\text{sol}_\text{san}}{\text{sol}_\text{ref}}$')
    plt.ylabel(r'$\frac{time_\mathrm{rules}}{time_\mathrm{san}}$', fontsize = 16)
    plt.title("Cities Instances")
    plt.savefig("figures/time_rules_vs_san")
    plt.close()
