import matplotlib.pyplot as plt
import matplotlib
import math
from dataclasses import dataclass
import itertools

# stolen from matplotlib.ticker.LogFormatterExponent
class LogFormatter(matplotlib.ticker.LogFormatter):
    def _num_to_string(self, x, vmin, vmax):
        fx = math.log(x) / math.log(self._base)
        if abs(fx) > 10000:
            s = '%1.0g' % fx
        elif abs(fx) < 1:
            s = '%1.0g' % fx
        else:
            fd = math.log(vmax - vmin) / math.log(self._base)
            s = self._pprint_val(fx, fd)
        return "$" + str(int(self._base)) + "^{" + s + "}$"

_k = 1024
_m = _k*_k

Q = [8, 16, 32, 64]
N = [int(pow(2, 18)), int(pow(2, 20)), int(pow(2, 22)), int(pow(2, 24)), int(pow(2, 26))]  # 256k 1M 4M 16M 64M
M = [64*_k, 128*_k, 256*_k, 1*_m, 2*_m, 4*_m, 8*_m]
B = [8*_k, 16*_k, 64*_k, 128*_k, 512*_k]

ALL_PARAMS = ("q", "n", "m", "b")

@dataclass
class Row:
    q: int
    n: int
    m: int
    b: int
    time: int

def load(name):
    arr = []
    with open(f"results-{name}.txt") as f:
        for l in f:
            parts = l.split(" ")
            arr.append(Row(*(int(p.strip()) for p in parts)))
    return arr

def filter(data, out_key, filter):
    arr = []
    for row in data:
        if any(filter.get(key) and getattr(row, key) != filter[key] for key in ALL_PARAMS):
            continue
        arr.append(getattr(row, out_key))
    return arr


data_finn = load("finn")
data_joshua = load("joshua")
data_levin = load("levin")
data_default = load("normal_mergesort")

# plot one
_filter = {"q": 64, "m": 8*_m, "b": 8*_k}

x = filter(data_finn, "n", _filter)
ax = plt.subplot(1, 1, 1)

for data in (data_joshua, data_default):
    _data = filter(data, "time", _filter)
    ax.plot(x, _data)
    ax.scatter(x, _data)
ax.grid()

# ax.legend([f"$m=2^{{{int(math.log(m, 2))}}}$" for m in M])
plt.xlabel("Anzahl Elemente")
plt.ylabel("Laufzeit in ms")
plt.xscale("log")
ax.get_xaxis().set_major_locator(matplotlib.ticker.LogLocator(base=2))
ax.get_xaxis().set_minor_locator(matplotlib.ticker.NullLocator())
ax.get_xaxis().set_major_formatter(LogFormatter(base=2))
ax.set_xticks(N)
# plt.yscale("log")
# ax.get_yaxis().set_major_locator(matplotlib.ticker.LogLocator(base=2))
# ax.get_yaxis().set_minor_locator(matplotlib.ticker.NullLocator())
# ax.get_yaxis().set_major_formatter(LogFormatter(base=2))
# ax.set_yticks(N)
plt.title(", ".join(f"${k}=2^{{{int(math.log(v, 2))}}}$" for k, v in _filter.items() if v is not None))
plt.show()


# # plot all
# for q, n, m, b in itertools.product(Q + [None], N + [None], M + [None], B + [None]):
#     _filter = {"n": n, "q": q, "m": m, "b": b}
#     if list(_filter.values()).count(None) != 1:
#         continue
#     if m and b and (m/b) < 4:
#         continue

#     param = list(_filter.keys())[list(_filter.values()).index(None)]

#     x = filter(data_finn, param, _filter)
#     ax = plt.subplot(1, 1, 1)

#     for data in (data_levin, data_joshua, data_finn):
#         _data = filter(data, "time", _filter)
#         ax.plot(x, _data)
#         ax.scatter(x, _data)
#     ax.grid()

#     ax.legend(["Levin", "Joshua", "Finn"])
#     plt.xscale("log")
#     plt.xlabel([p for p in ALL_PARAMS if _filter.get(p) is None][0].upper() + " in bytes")
#     plt.ylabel("Laufzeit in ms")
#     ax.get_xaxis().set_major_locator(matplotlib.ticker.LogLocator(base=2))
#     ax.get_xaxis().set_minor_locator(matplotlib.ticker.NullLocator())
#     ax.get_xaxis().set_major_formatter(LogFormatter(base=2))
#     if q is None:
#         ax.set_xticks(Q)
#     elif n is None:
#         ax.set_xticks(N)
#     elif m is None:
#         ax.set_xticks(M)
#     elif b is None:
#         ax.set_xticks(B)
#     plt.title(", ".join(f"${k}=2^{{{int(math.log(v, 2))}}}$" for k, v in _filter.items() if v is not None))
#     plt.savefig(f"img/{param}__" + "_".join(f"{k}_{v}" for k, v in _filter.items() if v is not None) + ".pdf")
#     plt.close()
