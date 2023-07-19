import cv2
from Python.EdgeDetection import edge_detect_live
from Python.RasterToVector import to_polygons, draw_polygons, scale_polygons
import matplotlib.pyplot as plt
from Python.GCodeGenerator import get_gcode

IMAGE_PATH = "TestImages/onepiece.png"

# Load the image
image = cv2.imread(IMAGE_PATH)

# Perform edge detection
#image = edge_detect_live(image)

# Convert the edges to polygons
polygons = to_polygons(image)
scale_polygons(polygons, 0.03)

# Draw the polygons
draw_polygons(polygons, show_borders=True, ax=plt)
plt.grid()
plt.show()

# Convert the polygons to GCode
gcode = get_gcode(polygons, 100, 100, 12.3, 10.3)

# Save the GCode to a file
with open("output.gcode", "w") as file:
    file.write(gcode)
