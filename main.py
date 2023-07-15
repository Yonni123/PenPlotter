import cv2
import numpy as np
import Python.EdgeDetection as ED
import Python.RasterToVector as RTV
import matplotlib.pyplot as plt


IMAGE_PATH = "TestImages/nami.jpg"

# Load the image
image = cv2.imread(IMAGE_PATH)

# Perform edge detection
edges = ED.edge_detect_live(image)

# Convert the edges to polygons
polygons = RTV.to_polygons(edges)

# Draw the polygons
RTV.draw_polygons(polygons, show_points=False, random_colors=False, plt=plt)
plt.grid()
plt.show()