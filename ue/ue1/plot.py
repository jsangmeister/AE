import matplotlib.pyplot as plt
from collections import defaultdict

_k = 1024
_m = _k*_k

Q = [8, 16, 32, 64]
N = [int(pow(2, 18)), int(pow(2, 20)), int(pow(2, 22)), int(pow(2, 24)), int(pow(2, 26))]  # 256k 1M 4M 16M 64M
M = [64*_k, 128*_k, 256*_k, 1*_m, 2*_m, 4*_m, 8*_m]
B = [8*_k, 16*_k, 64*_k, 128*_k, 512*_k]

def load(name, data_time, data_n):
    with open(f"results-{name}.txt") as f:
        for l in f:
            parts = l.split(" ")
            q, n, m, b, time = [int(p.strip()) for p in parts]
            data_time[name][q][m][b].append(time)
            data_n[name][q][m][b].append(n)

data_n = defaultdict(lambda: defaultdict(lambda: defaultdict(lambda: defaultdict(list))))
data_time = defaultdict(lambda: defaultdict(lambda: defaultdict(lambda: defaultdict(list))))

load("finn", data_time, data_n)
load("joshua", data_time, data_n)
load("levin", data_time, data_n)

print(data_n["finn"][8][128*_k][8*_k])
print(data_time["finn"][8][128*_k][8*_k])

plt.plot(data_n["finn"][8][128*_k][8*_k], data_time["finn"][8][128*_k][8*_k])
plt.plot(data_n["joshua"][8][128*_k][8*_k], data_time["joshua"][8][128*_k][8*_k])
plt.plot(data_n["levin"][8][128*_k][8*_k], data_time["levin"][8][128*_k][8*_k])
plt.grid()
plt.legend(["finn", "joshua", "levin"])
plt.show()