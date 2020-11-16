import itertools

k = 1024
m = k*k

#Q = [8, 16, 32, 64]
Q = [64]
N = [int(pow(2, 18)), int(pow(2, 20)), int(pow(2, 22)), int(pow(2, 24)), int(pow(2, 26))]  # 256k 1M 4M 16M 64M
#M = [64*k, 128*k, 256*k, 1*m, 2*m, 4*m, 8*m]
M = [8*m]
#B = [8*k, 16*k, 64*k, 128*k, 512*k]
B = [8*k]

for q, n, m, b in itertools.product(Q, N, M, B):
    if (m/b) >= 4:  # prevent invalid calls
        print(f"{q} {n} {m} {b}")
