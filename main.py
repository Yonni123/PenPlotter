import cv2
from Python.EdgeDetection import edge_detect_live, threshold_live
from Python.RasterToVector import to_polygons, draw_polygons, scale_polygons, get_bounds, adjust_polygons_bounds
import matplotlib.pyplot as plt
from Python.GCodeGenerator import get_gcode

IMAGE_PATH = "TestImages/LEVI.jpg"

# Load the image
image = cv2.imread(IMAGE_PATH)

# Perform edge detection
#image = edge_detect_live(image)
image = threshold_live(image)

# Convert the edges to polygons
polygons = to_polygons(image, fill=True)
scale_polygons(polygons, 0.17)

# Get the bounds of the polygons
min_x, max_x, min_y, max_y = get_bounds(polygons)
print("min_x: " + str(min_x) + ", max_x: " + str(max_x) + ", min_y: " + str(min_y) + ", max_y: " + str(max_y))
adjust_polygons_bounds(polygons, 66, 135, 199, 225)
min_x, max_x, min_y, max_y = get_bounds(polygons)
print("min_x: " + str(min_x) + ", max_x: " + str(max_x) + ", min_y: " + str(min_y) + ", max_y: " + str(max_y))

# Draw the polygons
draw_polygons(polygons, show_borders=True, show_filling=False, ax=plt)
plt.grid()
plt.show()

# Convert the polygons to GCode
gcode = get_gcode(polygons, 0, 0, 2, 0)

# Save the GCode to a file
with open("output.gcode", "w") as file:
    file.write(gcode)
