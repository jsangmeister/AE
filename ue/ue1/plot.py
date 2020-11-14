import matplotlib.pyplot as plt
import matplotlib
import math
from dataclasses import dataclass

# stolen from matplotlib.ticker.LogFormatterExponent
class Log2Formatter(matplotlib.ticker.LogFormatter):
    def _num_to_string(self, x, vmin, vmax):
        fx = math.log(x) / math.log(2)
        if abs(fx) > 10000:
            s = '%1.0g' % fx
        elif abs(fx) < 1:
            s = '%1.0g' % fx
        else:
            fd = math.log(vmax - vmin) / math.log(2)
            s = self._pprint_val(fx, fd)
        return "$2^{" + s + "}$"

_k = 1024
_m = _k*_k

Q = [8, 16, 32, 64]
N = [int(pow(2, 18)), int(pow(2, 20)), int(pow(2, 22)), int(pow(2, 24)), int(pow(2, 26))]  # 256k 1M 4M 16M 64M
M = [64*_k, 128*_k, 256*_k, 1*_m, 2*_m, 4*_m, 8*_m]
B = [8*_k, 16*_k, 64*_k, 128*_k, 512*_k]

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
        if any(key in filter and getattr(row, key) != filter[key] for key in ("q", "n", "m", "b")):
            continue
        arr.append(getattr(row, out_key))
    return arr

data_finn = load("finn")
data_joshua = load("joshua")
data_levin = load("levin")

x = filter(data_finn, "n", {"q": 8, "m": 128*_k, "b": 8*_k})
ax = plt.subplot(1, 1, 1)
ax.plot(x, filter(data_finn, "time", {"q": 8, "m": 128*_k, "b": 8*_k}))
ax.plot(x, filter(data_joshua, "time", {"q": 8, "m": 128*_k, "b": 8*_k}))
ax.plot(x, filter(data_levin, "time", {"q": 8, "m": 128*_k, "b": 8*_k}))
ax.grid()
ax.legend(["finn", "joshua", "levin"])
ax.get_xaxis().set_major_formatter(Log2Formatter(base=2))
ax.set_xticks(N)
plt.show()
