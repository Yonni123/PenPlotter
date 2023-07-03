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

// This function saves a bitmap as a .bmp file
void save_bitmap(potrace_bitmap_t *map){
    // Create a new image
    cv::Mat image(map->h, map->w, CV_8UC1);

    // Copy the bitmap data to the image
    for (int i = 0; i < map->h; i++) {
        for (int j = 0; j < map->w; j++) {
            image.at<uchar>(i, j) = map->map[i * map->w + j];
        }
    }

    // Save the image
    cv::imwrite("output.bmp", image);
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

    save_bitmap(bitmap);
    
    // Create a new potrace_state_t
    potrace_param_t *param = potrace_param_default();
    param->turdsize = 2;
    param->turnpolicy = POTRACE_TURNPOLICY_BLACK;
    param->alphamax = 0.1;
    param->opticurve = 1;
    param->opttolerance = 0.2;
    param->progress.callback = NULL;
    param->progress.data = NULL;
    param->progress.min = 0.0;
    param->progress.max = 1.0;
    param->progress.epsilon = 0.0;
    potrace_state_t *state = potrace_trace(param, bitmap);

    return 0;
}
