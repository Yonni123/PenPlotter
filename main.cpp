#include <iostream>
#include <opencv2/opencv.hpp>
#include <potracelib.h>

potrace_bitmap_t *potrace_bitmap_create_from_array(int width, int height, unsigned char *data, int stride) {
    potrace_bitmap_t *bitmap = (potrace_bitmap_t *) malloc(sizeof(potrace_bitmap_t));
    bitmap->w = width;
    bitmap->h = height;
    bitmap->dy = stride;
    bitmap->map = (potrace_word *) malloc(sizeof(potrace_word) * width * height);
    for (int i = 0; i < width * height; i++) {
        bitmap->map[i] = data[i];
    }
    return bitmap;
}

int main() {
    // Load and display an image using OpenCV
    cv::Mat image = cv::imread("../TestImages/edges.png", cv::IMREAD_COLOR);
    if (image.empty()) {
        std::cout << "Failed to load image." << std::endl;
        return 1;
    }

    cv::imshow("Original Image", image);
    cv::waitKey(0);

    // Convert image to grayscale
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    // Perform thresholding
    cv::Mat thresholdedImage;
    cv::threshold(grayImage, thresholdedImage, 128, 255, cv::THRESH_BINARY);

    // Show the image
    cv::imshow("Thresh", thresholdedImage);
    cv::waitKey(0);

    // Convert the image to a bitmap
    potrace_bitmap_t *bitmap = potrace_bitmap_create_from_array(thresholdedImage.cols, thresholdedImage.rows,
                                                                thresholdedImage.data, thresholdedImage.step);

    return 0;
}
