#! /usr/bin/env python3

import matplotlib.pyplot as plt
import matplotlib.patches as patches
import sys

def usage():
  print("Usage: {} [--no-labels] <instance> [<output>]".format(sys.argv[0]))
  sys.exit(1)

if (len(sys.argv) < 2):
  usage()
drawLabels = True
index = 1
if sys.argv[1] == "--no-labels":
  drawLabels = False
  index = 2

infile = sys.argv[index]
outfile = "result.png"
if len(sys.argv) == index + 2:
  outfile = sys.argv[index + 1]

# Helper function to get uniformly distributed colours for data points
def get_cmap(n, name='hsv'):
  return plt.cm.get_cmap(name, n)

# Read instance
file = open(infile, mode ='r')
lines = file.readlines()
file.close()

# Setup colour map and figure
colormap = get_cmap(int(lines[0])+1)

# Calculate rectangles, points, and bounding box
minx, maxx, miny, maxy = sys.maxsize, -sys.maxsize, sys.maxsize, -sys.maxsize
rects = []
labels = []
circles = []
for i in range(1, len(lines)):
  p = lines[i].split()
  x = int(p[0])
  y = int(p[1])
  l = int(p[2])
  h = int(p[3])
  t = p[4]
  b = (p[5] == '1')
  x2 = int(p[6])
  y2 = int(p[7])
  circlefc=(0.8,0,0)
  if b:
    minx = min(minx, x, x2)
    maxx = max(maxx, x, x2+l)
    miny = min(miny, y, y2-h)
    maxy = max(maxy, y, y2)
    rects.append(patches.Rectangle((x2,y2), l, -h, linewidth=1, edgecolor=colormap(i), facecolor=(0.5,0.5,0.5,0.2)))
    labels.append((x2 + l/2, y2 - h/2, t))
    circlefc=(0,0.8,0)
  else:
    minx = min(minx, x)
    maxx = max(maxx, x)
    miny = min(miny, y)
    maxy = max(maxy, y)
  circles.append(patches.Circle((x,y), radius=0.35, linewidth=1, edgecolor=colormap(i), facecolor=circlefc))
  labels.append((x, y, i))

# "Clever" scaling
print("max: " + str(maxx) + " min:" + str(minx))
factor = 0.4
if (maxx-minx > 100):
  factor *= 0.1
if (maxx-minx > 1000):
  factor *= 0.1
#if (maxx-minx > 1000):
#  factor *= 0.1
#if (maxx-minx > 1000):
#  factor *= 0.1
xwidth = (maxx-minx)*factor
ywidth = (maxy-miny)*factor
fig = plt.figure(figsize=(xwidth, ywidth))
ax = plt.axes()
ax.axes.set_aspect('equal')
ax.grid()

ax.axhline(0, color=(0,0,0,0.25))
ax.axvline(0, color=(0,0,0,0.25))

for rect in rects:
  ax.add_patch(rect)
for circ in circles:
  ax.add_patch(circ)
if drawLabels:
  for label in labels:
    x, y, t = label
    ax.text(x, y, t, horizontalalignment='center', verticalalignment='center')

# Make sure everything is visible
plt.xlim(minx-1, maxx+1)
plt.ylim(miny-1, maxy+1)

plt.savefig(outfile)

