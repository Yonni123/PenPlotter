import numpy as np
import potrace
import cv2
import matplotlib.pyplot as plt


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

    polygons = []
    for curve in path:
        polygon = []
        for segment in curve:
            if segment.is_corner:
                c = segment.c
                e = segment.end_point
                polygon.append(c)
                polygon.append(e)
            else:
                pass  # Since we sat alphamax to 0, all curves should be corners
        polygons.append(polygon)

    return polygons


COLORS = ['b', 'r']
def draw_polygons(polygons, show_points=False, ax=plt, depth=0):
    color = COLORS[depth % 2]
    for polygon in polygons:
        polygon = np.array(polygon)
        ax.plot(
            polygon[:, 0],  # x-coordinates.
            polygon[:, 1],  # y-coordinates.
            color + '-'  # Styling (blue, solid line).
        )
        # Draw a line between the first and last points.
        ax.plot(
            [polygon[0, 0], polygon[-1, 0]],  # x-coordinates of first and last points.
            [polygon[0, 1], polygon[-1, 1]],  # y-coordinates of first and last points.
            color + '-'  # Styling (blue, solid line).
        )
        # ax.fill(polygon[:, 0], polygon[:, 1], color)
        if show_points:
            ax.plot(
                polygon[:, 0],  # x-coordinates.
                polygon[:, 1],  # y-coordinates.
                'bo',  # Styling (blue, circles).
                markersize=1
            )

    #
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
    draw_polygons(polygons, show_points=False, ax=ax1)
    ax1.grid()
    ax1.title.set_text('Polygons')
    # Plot the original image on the right subplot
    ax2.imshow(data)
    ax2.title.set_text('Original Image')
    # Show the figure
    plt.show()
