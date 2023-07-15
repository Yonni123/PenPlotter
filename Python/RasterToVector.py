import numpy as np
import potrace
import cv2
import matplotlib.pyplot as plt


class Polygon:
    def __init__(self, points):
        self.points = points
        self.children = ()


# A recursive function that decomposes a curve into polygons
# The recursion takes care of the children of the curve
def decompose_to_polygons(parent, curve):
    points = []
    for segment in curve:
        c = segment.c
        e = segment.end_point
        points.append(c)
        points.append(e)
    points = np.array(points)

    p = Polygon(points)
    parent.children += (p,)

    for child in curve.children:
        decompose_to_polygons(p, child)


def to_polygons(img):
    if len(img.shape) == 3:
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    else:
        gray = img
    data = gray < 128  # Convert to 2D boolean array
    data = np.flipud(data)  # Flip the image vertically, since Potrace's origin is in the top-left corner
    bmp = potrace.Bitmap(data)
    path = bmp.trace(
        turdsize=0,
        turnpolicy=potrace.TURNPOLICY_MINORITY,
        alphamax=0,
        opticurve=1,
        opttolerance=0
    )

    # For my pen plotter, I prefer to convert every curve to a list of points
    # Since I set alphamax to 0, all curves should be corners
    # Connecting the corners with straight lines will result in a polygon
    # The entire image will be a list of polygons
    # This will make it easy for the pen plotter to draw the image
    # All it needs to do is, for each polygon, move the pen to all the points in the polygon
    # Lift the pen, move to the next polygon, and repeat
    root = Polygon(())
    for curve in path.curves_tree:
        decompose_to_polygons(root, curve)
    return root


COLORS = ['b', 'r']         # Colors for the borders of the polygons
FILL_COLORS = ['w', 'k']    # Colors for the fill of the polygons


def draw_poly(poly, show_borders, ax=plt, depth=0):
    fill_color = FILL_COLORS[depth % len(FILL_COLORS) - 1]  # -1 since the first polygon doesn't count
    points = poly.points

    # Fill the polygon
    ax.fill(
        points[:, 0],  # x-coordinates.
        points[:, 1],  # y-coordinates.
        fill_color
    )

    for child in poly.children:
        draw_poly(child, show_borders, ax, depth + 1)

    if show_borders:
        color = COLORS[depth % len(COLORS)]
        ax.plot(
            points[:, 0],  # x-coordinates.
            points[:, 1],  # y-coordinates.
            color + '-'  # Styling (blue, solid line).
        )
        # Draw a line between the first and last points.
        ax.plot(
            [points[0, 0], points[-1, 0]],  # x-coordinates of first and last points.
            [points[0, 1], points[-1, 1]],  # y-coordinates of first and last points.
            color + '-'  # Styling (blue, solid line).
        )


def draw_polygons(root, show_borders=True, ax=plt):
    for polygon in root.children:
        draw_poly(polygon, show_borders, ax, 0)

    # Make ax have the same ratio as the image.
    try:
        ax.set_aspect('equal', adjustable='box', anchor='C')
    except:
        ax.gca().set_aspect('equal', adjustable='box')


if __name__ == "__main__":
    IMAGE_PATH = "../TestImages/more_than_circle.png"
    data = cv2.imread(IMAGE_PATH)
    polygons = to_polygons(data)

    # Plot the polygons and the original image
    fig, (ax1, ax2) = plt.subplots(1, 2)
    # Plot the polygons on the left subplot
    draw_polygons(polygons, show_borders=True, ax=ax1)
    ax1.grid()
    ax1.title.set_text('Polygons')
    # Plot the original image on the right subplot
    ax2.imshow(data)
    ax2.title.set_text('Original Image')
    # Show the figure
    plt.show()
