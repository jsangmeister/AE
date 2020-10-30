import sys
import utm
import csv
import argparse

# Converts ellipsoid coordinates into 2-D coordinates with mercator projection

parser = argparse.ArgumentParser(description="Converts lat, lng to utm. Input must have form: label, lat, lng[ ,...]")

parser.add_argument('-i', help="Input csv")
parser.add_argument('-o', help="Output csv")
parser.add_argument('-hd', help="Set if header line is given", action='store_const', const=True, default=False)

if not len(sys.argv) > 1:
    parser.parse_args(["convert_to_utm.py", "-h"])
args = parser.parse_args()

x,y = [], []
new_rows = []
with open(args.i, 'r', newline='') as csvfile:
    reader = csv.reader(csvfile)
    if args.hd:
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

with open(args.o, 'w', newline='') as csvfile:
    writer =  csv.writer(csvfile)
    writer.writerows(new_rows)

print("Min easting:  %f" % min(x))
print("Max easting:  %f" % max(x))
print("Min northing: %f" % min(y))
print("Max Northing: %f" % max(y))



