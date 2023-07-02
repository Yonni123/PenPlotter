#include <iostream>
#include <opencv2/opencv.hpp>
#include <potracelib.h>

int main() {
    // Load and display an image using OpenCV
    cv::Mat image = cv::imread("../TestImages/nami.jpg", cv::IMREAD_COLOR);
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

    // Create Potrace bitmap
    potrace_bitmap_t bitmap;
    bitmap.w = thresholdedImage.cols;
    bitmap.h = thresholdedImage.rows;
    bitmap.dy = (bitmap.w + sizeof(potrace_word) - 1) / sizeof(potrace_word);
    bitmap.map = new potrace_word[bitmap.dy * bitmap.h];
    std::memcpy(bitmap.map, thresholdedImage.data, bitmap.dy * bitmap.h * sizeof(potrace_word));

    // Create Potrace parameters
    potrace_param_t param;
    param.turdsize = 2;
    param.turnpolicy = POTRACE_TURNPOLICY_BLACK;
    param.alphamax = 0.1;
    param.opticurve = 1;
    param.opttolerance = 0.2;
    param.progress.callback = nullptr;
    param.progress.data = nullptr;
    param.progress.min = 0.0;
    param.progress.max = 1.0;
    param.progress.epsilon = 0.01;

    // Perform tracing
    potrace_state_t *state = potrace_trace(&param, &bitmap);
    potrace_path_t *path = state->plist;

    // Convert traced path to OpenCV Mat
    cv::Mat tracedImage(image.size(), CV_8UC3, cv::Scalar(255, 255, 255));
    for (potrace_path_t *p = path; p != nullptr; p = p->next) {
        potrace_curve_t *curve = &p->curve;
        for (int i = 0; i < curve->n; ++i) {
            if (curve->tag[i] == POTRACE_CURVETO) {
                cv::Point pt1(static_cast<int>(curve->c[i][0].x), static_cast<int>(curve->c[i][0].y));
                cv::Point pt2(static_cast<int>(curve->c[i][1].x), static_cast<int>(curve->c[i][1].y));
                cv::Point pt3(static_cast<int>(curve->c[i][2].x), static_cast<int>(curve->c[i][2].y));
                cv::line(tracedImage, pt1, pt3, cv::Scalar(0, 0, 0));
            }
        }
    }

    // Display the traced image
    cv::imshow("Traced Image", tracedImage);
    cv::waitKey(0);

    // Clean up
    potrace_state_free(state);
    delete[] bitmap.map;

    return 0;
}
