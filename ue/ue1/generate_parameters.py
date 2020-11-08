import itertools

k = 1024
m = k*k

Q = [8, 16, 32, 64]
N = [int(pow(2, 14)), int(pow(2, 18)), int(pow(2, 22)), int(pow(2, 26))]
M = [64*k, 128*k, 256*k, 1*m, 2*m, 4*m]
B = [8*k, 16*k, 64*k]

for q, n, m, b in itertools.product(Q, N, M, B):
    if (m/b) / 2 >= 2:  # prevent invalid calls
        print(f"{q} {n} {m} {b}")
