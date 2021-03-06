import os
import csv
import random
import math

seed = "algen20"
random.seed(seed)

# utm ccordinates are in meters, so we define our possible label range in kilometers.
# With a real map we can look for fitting numbers here. For example, a strongly zoomed in 
# map would require smaller labels relatively to kilometers

#example values
minx = 40
maxx = 80
miny = 25
maxy = 50

infolder = "countries_wmp"
outfolder = "labels_wmp"
header = True


if not os.path.exists(outfolder):
    os.mkdir(outfolder)

all_labels = []

for file in os.listdir(infolder):
    labels = []
    with open(os.path.join(infolder, file), "r", newline='') as csvfile:
        reader = csv.reader(csvfile)
        if header:
            next(reader)
        for row in reader:
            text = row[0].strip().replace(" ", "_")
            x = str(int(float(row[1])/1000))
            y = str(int(float(row[2])/1000))
            w = str(random.randint(minx, maxx))
            h = str(random.randint(miny, maxy))
            labels.append(" ".join([x, y, w, h, text, "0", x, y+"\n"]))
            all_labels.append(" ".join([x, y, w, h, text, "0", x, y+"\n"]))
    with open(os.path.join(outfolder, file[:len(file)-len(".csv")]), 'w') as labelfile:
        labelfile.write(str(len(labels)) + "\n")
        labelfile.writelines(labels)

with open(os.path.join(outfolder, "world"), 'w') as labelfile:
    labelfile.write(str(len(all_labels)) + "\n")
    labelfile.writelines(all_labels)