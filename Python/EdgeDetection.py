import cv2
import numpy as np

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

def edge_detect_live(image):
    if len(image.shape) == 3:   # If the image is not grayscale, convert it to grayscale
        image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    # Create a window
    cv2.namedWindow('Canny Edge Detection', cv2.WINDOW_NORMAL)

    # Resize the window to fit the screen
    height = 800
    width = int(image.shape[1] / image.shape[0] * height)
    cv2.resizeWindow('Canny Edge Detection', width, height)

    # Create a trackbar for threshold value and edge thickness
    cv2.createTrackbar('Threshold', 'Canny Edge Detection', 0, 255, update_threshold)
    cv2.createTrackbar('Thickness', 'Canny Edge Detection', 0, 10, update_threshold)

    # Perform initial edge detection
    threshold1 = cv2.getTrackbarPos('Threshold', 'Canny Edge Detection')
    threshold2 = 2 * threshold1
    edges = cv2.Canny(image, threshold1, threshold2)
    edges = cv2.bitwise_not(edges)
    cv2.imshow('Canny Edge Detection', edges)

    # Wait until the user presses 'Esc' key
    while True:
        key = cv2.waitKey(1)
        try:
            threshold1 = cv2.getTrackbarPos('Threshold', 'Canny Edge Detection')
            thickness = cv2.getTrackbarPos('Thickness', 'Canny Edge Detection')
        except:
            break
        if key == 27:  # 'Esc' key pressed
            break

    threshold2 = 2 * threshold1

    # Perform edge detection with the updated threshold values
    edges = cv2.Canny(image, threshold1, threshold2)

    # Make edges thicker
    kernel = np.ones((2, 2), np.uint8)
    edges = cv2.dilate(edges, kernel, iterations=thickness)

    # Invert the image so that the edges are black
    edges = cv2.bitwise_not(edges)

    # Close all windows
    cv2.destroyAllWindows()

    return edges


if __name__ == "__main__":
    input_image = "../TestImages/nami.jpg"
    image = cv2.imread(input_image)
    edges = edge_detect_live(image)

    cv2.imshow('Original', image)
    cv2.imshow('Canny Edge Detection', edges)
    cv2.waitKey(0)

