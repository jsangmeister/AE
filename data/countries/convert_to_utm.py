import sys
import utm
import csv
import argparse
import os

def convert_file(infile, outfile, header):
    x,y = [], []
    new_rows = []
    with open(infile, 'r', newline='') as csvfile:
        reader = csv.reader(csvfile)
        if header:
            header_csv = next(reader)
            header = [header_csv[0], "utm easting", "utm northing", "zone number", "zone letter"] + header_csv[3:]
            new_rows.append(header)

        for row in reader:
            label, lat, lng = row[0], float(row[1]),float(row[2])
            utmconv = utm.from_latlon(lat, lng)
            x.append(utmconv[0])
            y.append(utmconv[1])
            new_row = [label]
            new_row.extend(utmconv)
            new_row.extend(row[3:])
            new_rows.append(new_row)

    with open(outfile, 'w', newline='') as csvfile:
        writer =  csv.writer(csvfile)
        writer.writerows(new_rows)

    return min(x), max(x), min(y), max(y)


def convert_folder(infol, outfol, hd):
    if not os.path.exists(outfol):
        os.mkdir(outfol)
    for file in os.listdir(infol):
        if os.path.isdir(os.path.join(infol, file)):
            continue

        convert_file(os.path.join(infol, file), os.path.join(outfol, file), hd)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Converts lat, lng to utm. Input must have form: label, lat, lng[ ,...]")

    parser.add_argument('-i', help="Input csv")
    parser.add_argument('-o', help="Output csv")
    parser.add_argument('-hd', help="Set if header line is given", action='store_const', const=True, default=False)
    parser.add_argument('-f', help="Set if -i and -o are folders. Output csv will be named like their input", action='store_const', const=True, default=False)

    if not len(sys.argv) > 1:
        parser.parse_args(["convert_to_utm.py", "-h"])
    args = parser.parse_args()

    if args.f:
        convert_folder(args.i, args.o, args.hd)
    else:
        convert_file(args.i, args.o, args.hd)
