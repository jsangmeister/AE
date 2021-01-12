import itertools

ch = range(0, 2)
ib = [0.5]
concurrent = range(1, 3)
focus = range(0, 4)
method = range(-1, 6)
cuts = range(-1, 4)
presolve = range(-1, 3)

list_of_params = (ch, ib, concurrent, focus, method, cuts, presolve)

for i, arr in enumerate(list_of_params):
    if i == 1:
        continue
    output = []
    for oarr in list_of_params:
        output.append(str(oarr[0]))
    for val in arr:
        output[i] = str(val)
        print(" ".join(output))
