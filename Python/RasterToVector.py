import potrace
from PIL import Image
import numpy as np
import matplotlib.pyplot as plt

# Load the image and convert it to black and white
image = Image.open("TestImages/circle.png").convert("L")

# Convert the image to a numpy array
data = np.array(image)

# Convert pixel values to 0 or 1
data = np.where(data > 128, 1, 0)

# Create a bitmap from the array
bmp = potrace.Bitmap(data)

# Trace the bitmap to a path
path = bmp.trace()

# Iterate over path curves
for curve in path:
    print("start_point =", curve.start_point)
    for segment in curve:
        print(segment)
        end_point_x, end_point_y = segment.end_point
        if segment.is_corner:
            c_x, c_y = segment.c
        else:
            c1_x, c1_y = segment.c1
            c2_x, c2_y = segment.c2

# Convert the path to SVG and save it to a file
path.save_svg("TestImages/circle.svg")
