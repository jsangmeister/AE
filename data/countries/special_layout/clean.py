import csv

#data from https://simplemaps.com

with open("us_raw.csv", "r", newline='') as usin, open("united-states.csv", "w", newline='') as usout:
    reader, writer = csv.reader(usin), csv.writer(usout)
    for row in reader:
        writer.writerow([row[0] + ", " + row[3]] + row[6:10])

with open("ca_raw.csv", "r", newline='') as cain, open("canada.csv", "w", newline='') as caout:
    reader, writer = csv.reader(cain), csv.writer(caout)
    for row in reader:
        writer.writerow([row[0] + ", " + row[3]] + row[4:8])
