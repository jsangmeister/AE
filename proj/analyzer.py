import numpy as np
import matplotlib as mpl
mpl.rcParams['text.usetex'] = True
from matplotlib import pyplot as plt
import os
import csv
import math

# target = "data/scripts/runs/rules/results_rules.csv"
# countrie_rules_data = []
# with open(target, 'r') as file:
#     file.readline()
#     for line in csv.reader(file):
#         row = []
#         row.append(line[0])
#         row.extend([int(x.strip()) for x in line[1:]])
#         countrie_rules_data.append(row)


target = "data/scripts/runs/simulAn/CountriesRuns.csv"
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


target = "results_gurobi.csv"
countries_ilp_data = []
with open(target, 'r') as file:
    file.readline()
    for line in csv.reader(file):
        row = []
        row.append(line[0])
        row.extend([int(x.strip()) for x in line[1:]])
        countries_ilp_data.append(row)


target = "results_parameters.csv"
countries_parameters_data = {}
with open(target, 'r') as file:
    file.readline()
    for line in csv.reader(file):
        if line[0] not in countries_parameters_data:
            countries_parameters_data[line[0]] = []
        countries_parameters_data[line[0]].append([int(x.strip()) for x in line[1:]])


target = "results_ilppt.csv"
countries_ilppt_data = []
with open(target, 'r') as file:
    file.readline()
    for line in csv.reader(file):
        row = []
        row.append(line[0])
        row.extend([int(x.strip()) for x in line[1:]])
        countries_ilppt_data.append(row)


# ilp vs ilpPt (time)
if True:
    x = []
    y = []
    for i, run in enumerate(countries_ilppt_data):
        sa_entry = countries_sa_best_runs_dict[run[0]]
        ilprun = countries_ilp_data[i]
        x.append(sa_entry[1])
        if run[2] == 0:
            if ilprun[2] == 0:
                y.append(1)
            else:
                del x[-1]
        else:
            y.append(run[2]/ilprun[2])
    plt.axhline(y=1, color='r', linestyle='-')
    plt.scatter(x, y)

    # plt.xscale("log")
    # plt.yscale("log")
    plt.xlabel("Number of Nodes")
    plt.ylabel(r'$\frac{time_\mathrm{ilpPt}}{time_\mathrm{ilp}}$', fontsize = 16)
    plt.title("Cities Instances")
    plt.savefig("figures/time_ilppt_vs_ilp")
    plt.close()


# ilp vs sa
if False:
    x = []
    y = []
    for run in countries_ilp_data:
        sa_entry = countries_sa_best_runs_dict[run[0]]
        x.append(sa_entry[1])
        y.append(sa_entry[6]/run[1])
    plt.scatter(x, y)

    plt.xscale("log")
    #plt.yscale("log")
    # plt.xlim(10, 1000)
    plt.xlabel("Number of Nodes")
    plt.ylabel(r'$\frac{sol_\mathrm{sa}}{sol_\mathrm{ilp}}$', fontsize=16)
    plt.title("Cities Instances")
    plt.savefig("figures/value_sa_vs_ilp")
    plt.close()


# ilp vs sa (time)
if False:
    x = []
    y = []
    for run in countries_ilp_data:
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

    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("Number of Nodes")
    plt.ylabel(r'$\frac{time_\mathrm{ilp}}{time_\mathrm{sa}}$', fontsize = 16)
    plt.title("Cities Instances")
    plt.savefig("figures/time_ilp_vs_sa")
    plt.close()


# gurobi parameters
if False:
    x = []
    y = []
    xh = []
    yh = []
    used_x = set()
    for i in range(24):
        y.append([])
    for country, results in countries_parameters_data.items():
        sa_entry = countries_sa_best_runs_dict[country]
        n = sa_entry[1]

        # only heuristic
        xh.append(n)
        yh.append(results[1][1]/results[0][1])

        if n in used_x:
            continue
        x.append(n)
        used_x.update(range(int(n - math.sqrt(n * 0.1)), int(n + math.sqrt(n * 0.1))))
        for i, run in enumerate(results):
            if i in (2, 4, 8, 15, 20):
                # skip base runs
                continue
            # breakpoint()
            y[i].append(run[1])

    plt.axhline(y=1, color='r', linestyle='-')
    plt.scatter(xh, yh)
    plt.xscale("log")
    # plt.yscale("log")
    plt.xlabel("Number of Nodes")
    plt.ylabel(r'$\frac{time_\mathrm{w/heur}}{time_\mathrm{pure}}$', fontsize = 16)
    plt.title("Cities Instances")
    plt.savefig("figures/time_heuristic_vs_pure")
    plt.close()

    y = [l for l in y if l]
    # breakpoint()
    assert len(y) == 19

    plt.scatter(x, y[0], c="red", marker="x", zorder=100)
    for y_ in y:
        plt.scatter(x, y_, c="gray", marker="x")

    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("Number of Nodes")
    plt.ylabel("Time (ms)")
    plt.title("Cities Instances")
    plt.savefig("figures/time_ilp_parameters")
    plt.close()
