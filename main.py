import cv2
from Python.EdgeDetection import edge_detect_live
from Python.RasterToVector import to_polygons
from Python.RasterToVector import draw_polygons
import matplotlib.pyplot as plt
from Python.GCodeGenerator import get_gcode

IMAGE_PATH = "TestImages/more_than_circle.png"

# Load the image
image = cv2.imread(IMAGE_PATH)

# Perform edge detection
#edges = edge_detect_live(image)

# Convert the edges to polygons
polygons = to_polygons(image)

# Draw the polygons
draw_polygons(polygons, show_borders=True, ax=plt)
plt.grid()
plt.show()

# Convert the polygons to GCode
gcode = get_gcode(polygons, 0, 0, '12.3', '10.3')

# Save the GCode to a file
with open("output.gcode", "w") as file:
    file.write(gcode)
