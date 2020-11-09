import itertools
import random
import math

LABEL_WIDTH_INTERVAL = [20, 200]
LABEL_HEIGHT_INTERVAL = [10, 100]

LABEL_COUNTS = [100, 1000, 10000, 100000, 1000000]
LABEL_DENSITY = [0.01, 0.1, 0.3, 0.5, 1]

def main():
    for n, d in itertools.product(LABEL_COUNTS, LABEL_DENSITY[::-1]):
        avg_width = sum(LABEL_WIDTH_INTERVAL) / 2
        avg_height = sum(LABEL_HEIGHT_INTERVAL) / 2
        avg_area = avg_width * avg_height
        total_avg_area = avg_area * n
        area_needed = total_avg_area / d
        max_abs_coords = math.ceil(math.sqrt(area_needed) / 2)
        d_str = str(d).replace(".", "_")
        with open(f"instances/generated_n{n}_d{d_str}", 'w') as labelfile:
            labelfile.write(f"{n}")
            for i in range(n):
                w = random.randint(LABEL_WIDTH_INTERVAL[0], LABEL_WIDTH_INTERVAL[1])
                h = random.randint(LABEL_HEIGHT_INTERVAL[0], LABEL_HEIGHT_INTERVAL[1])
                x = random.randint(-max_abs_coords, max_abs_coords)
                y = random.randint(-max_abs_coords, max_abs_coords)
                labelfile.write(f"\n{x} {y} {w} {h} {i} 0 0 0")


if __name__ == "__main__":
    main()