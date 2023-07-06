#include <iostream>
#include <opencv2/opencv.hpp>
#include <potracelib.h>
#include <bitset>

using namespace std;
using namespace cv;

potrace_bitmap_t *potrace_bitmap_from_mat(Mat image)
{
    potrace_bitmap_t *bitmap = (potrace_bitmap_t *)malloc(sizeof(potrace_bitmap_t));
    bitmap->w = image.cols;           // Width of the bitmap
    bitmap->h = image.rows;           // Height of the bitmap
    int N = sizeof(potrace_word) * 8; // Number of bits per word, every bit is a pixel

    // Make the width a multiple of N by padding
    if (bitmap->w % N != 0)
    {
        bitmap->w = (bitmap->w / N + 1) * N;
    }

    bitmap->dy = bitmap->w / N; // Number of words per scanline

    // Allocate memory for the bitmap, we need dy*h potrace_words
    bitmap->map = (potrace_word *)malloc(bitmap->dy * bitmap->h * sizeof(potrace_word));

    // Invert the image since potrace uses white for background (0) and black for foreground (1)
    bitwise_not(image, image);

    imshow("image", image);
    waitKey(0);

    // Loop through all pixels and create potrace_words
    int current_word = 0;
    int current_bit = 0;
    bitmap->map[current_word] = 0;
    for (int i = image.rows - 1; i >= 0; i--)
    { // Bottom to top
        for (int j = 0; j < bitmap->w; j++)
        { // Left to right
            // Get the pixel value (0 or 1)
            int pixel = 0;
            if (j < image.cols)
            {                                        // If still within image
                pixel = image.at<uchar>(i, j) / 255; // Convert pixel to 0 or 1
            }

            // Set the bit in the potrace_word if the pixel is set
            potrace_word mask = (1ULL << (N - 1 - current_bit));
            if (pixel == 1)
            {
                bitmap->map[current_word] |= mask;
            }
            else
            {
                bitmap->map[current_word] &= ~mask;
            }

            // Increment the current bit
            current_bit++;
            if (current_bit == N)
            {
                current_bit = 0;
                current_word++;
                bitmap->map[current_word] = 0;
            }
        }
    }

    // Restore the bitmap's width
    //bitmap->w = image.cols;
    return bitmap;
}

Mat mat_from_potrace_bitmap(potrace_bitmap_t* bitmap) {
    int N = sizeof(potrace_word) * 8; // Number of bits per word, every bit is a pixel

    // Create a black image with the size of the Potrace bitmap
    Mat image(bitmap->h, bitmap->w, CV_8UC1, Scalar(0));

    // Loop through all pixels in the Potrace bitmap
    int current_word = 0;
    int current_bit = 0;
    for (int i = bitmap->h - 1; i >= 0; i--) { // Bottom to top
        for (int j = 0; j < bitmap->w; j++) { // Left to right
            // Get the bit value from the current potrace_word
            potrace_word mask = (1ULL << (N - 1 - current_bit));
            int pixel = (bitmap->map[current_word] & mask) ? 255 : 0;

            // Set the pixel value in the OpenCV Mat
            if (j < image.cols) { // If still within the image bounds
                image.at<uchar>(i, j) = pixel;
            }

            // Increment the current bit
            current_bit++;
            if (current_bit == N) {
                current_bit = 0;
                current_word++;
            }
        }
    }

    // Invert the image back to its original state
    bitwise_not(image, image);

    return image;
}


int main()
{
    // Load and display an image using OpenCV
    Mat image = imread("../TestImages/edges.png", IMREAD_COLOR);
    if (image.empty())
    {
        std::cout << "Failed to load image." << std::endl;
        return 1;
    }

    // Convert image to grayscale
    Mat grayImage;
    cvtColor(image, grayImage, COLOR_BGR2GRAY);

    // Perform thresholding
    Mat thresholdedImage;
    threshold(grayImage, thresholdedImage, 128, 255, THRESH_BINARY);

    // Convert the image to a bitmap
    potrace_bitmap_t *bitmap = potrace_bitmap_from_mat(thresholdedImage);

    // Display the bitmap
    imshow("Potrace", mat_from_potrace_bitmap(bitmap));
    waitKey(0);

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
