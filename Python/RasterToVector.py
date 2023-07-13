import numpy as np
import potrace
import cv2
import matplotlib.pyplot as plt

IMAGE_PATH = "../TestImages/mikasaedge.jpg"

def draw_corner(points):
    plt.plot(
        points[:, 0],  # x-coordinates.
        points[:, 1],  # y-coordinates.
        'r-'  # Styling (red, circles, dotted).
    )

# Read the image and convert it into a 2D boolean array
data = cv2.imread(IMAGE_PATH, cv2.IMREAD_GRAYSCALE)
data = data > 128

# Show the image
#cv2.imshow("Image", data.astype(np.uint8)*255)
data = np.flipud(data)

#cv2.waitKey(0)

# Create a bitmap from the array
bmp = potrace.Bitmap(data)

# Trace the bitmap to a path
path = bmp.trace(
    turdsize=0,
    turnpolicy=potrace.POTRACE_TURNPOLICY_MINORITY,
    alphamax=0,
    opticurve=1,
    opttolerance=0
)

def get_curves(img):
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    data = gray > 128
    data = np.flipud(data)
    bmp = potrace.Bitmap(data)
    path = bmp.trace(
        turdsize=0,
        turnpolicy=potrace.POTRACE_TURNPOLICY_MINORITY,
        alphamax=0,
        opticurve=0,
        opttolerance=0.2
    )

# Iterate over path curves
for curve in path:
    s = [curve.start_point.x, curve.start_point.y]
    for segment in curve:
        if segment.is_corner:
            pass
            c1 = [segment.c.x, segment.c.y]
            e = [segment.end_point.x, segment.end_point.y]
            points = np.array([s, c1, e])
            draw_corner(points)
        else:
            c1 = [segment.c1.x, segment.c1.y]
            c2 = [segment.c2.x, segment.c2.y]
            e = [segment.end_point.x, segment.end_point.y]
            points = np.array([s, c1, c2, e])
            draw_bezier(points)
        s = e

plt.grid()
plt.show()
