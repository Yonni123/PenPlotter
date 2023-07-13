import cv2
import numpy as np

input_image = "../TestImages/mikasaedge.jpg"

# Callback function for trackbar
def update_threshold(val):
    threshold1 = cv2.getTrackbarPos('Threshold', 'Canny Edge Detection')
    threshold2 = 2 * threshold1
    thickness = cv2.getTrackbarPos('Thickness', 'Canny Edge Detection')
    edges = cv2.Canny(image, threshold1, threshold2)

    # make edges thicker
    kernel = np.ones((2, 2), np.uint8)
    edges = cv2.dilate(edges, kernel, iterations=thickness)

    # Invert the image so that the edges are black
    edges = cv2.bitwise_not(edges)

    cv2.imshow('Canny Edge Detection', edges)

# Read the image
image = cv2.imread(input_image, cv2.IMREAD_GRAYSCALE)

# Create a window
cv2.namedWindow('Canny Edge Detection', cv2.WINDOW_NORMAL)

# Resize the window to fit the screen
height = 800
width = int(image.shape[1]/image.shape[0]*height)
cv2.resizeWindow('Canny Edge Detection', width, height)

# Create a trackbar for threshold value and edge thickness
cv2.createTrackbar('Threshold', 'Canny Edge Detection', 0, 255, update_threshold)
cv2.createTrackbar('Thickness', 'Canny Edge Detection', 0, 10, update_threshold)

# Initialize the threshold value
threshold1 = cv2.getTrackbarPos('Threshold', 'Canny Edge Detection')
threshold2 = 2 * threshold1

# Perform initial edge detection
edges = cv2.Canny(image, threshold1, threshold2)
edges = cv2.bitwise_not(edges)
cv2.imshow('Canny Edge Detection', edges)

# Wait until the user presses 'Esc' key
while cv2.waitKey(0) != 27:
    pass

# Close all windows
cv2.destroyAllWindows()
