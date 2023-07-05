#include <iostream>
#include <opencv2/opencv.hpp>
#include <potracelib.h>
#include <bitset>

using namespace std;

potrace_bitmap_t *potrace_bitmap_from_mat(cv::Mat image)
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
    cv::bitwise_not(image, image);

    cv::imshow("image", image);
    cv::waitKey(0);

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
    return bitmap;
}

// This function saves a bitmap as a .bmp file
void save_bitmap(potrace_bitmap_t *bitmap)
{
    // Create a new image
    cv::Mat image(bitmap->h, bitmap->w, CV_8UC1);

    // Copy the bitmap data to the image
    int N = sizeof(potrace_word) * 8;
    int dy = bitmap->dy;
    potrace_word *map = bitmap->map;

    int ii = 2;
    int jj = 8;
    int pix = (map+(jj * dy))[ii / N] & (1ULL << (N - 1 - ii % N)) ? 255 : 0;
    cout << pix << endl;
    return; 
    int pixel = ((map + (jj * dy))[ii / N]) & (1ULL << (N - 1 - ii % N)) ? 255 : 0;
    cout << pixel << " ";

    return;

    for (int i = 0; i < bitmap->h; i++)
    {
        for (int j = 0; j < bitmap->w; j++)
        {
            // int J = bitmap->w - 1 - j;  // OpenCV origo is top left, potrace origo is bottom left, use J for openCV
            int pixel = ((map + j * dy)[i / N]) & (1ULL << (N - 1 - i % N)) ? 255 : 0;
            image.at<uchar>(i, j) = pixel;
        }
        cout << endl;
    }

    // Since white pixels are foreground in potrace, invert the image
    cv::bitwise_not(image, image);
    cv::imwrite("output.bmp", image);
}

int main()
{
    // Load and display an image using OpenCV
    cv::Mat image = cv::imread("../TestImages/cropped.jpg", cv::IMREAD_COLOR);
    if (image.empty())
    {
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
    potrace_bitmap_t *bitmap = potrace_bitmap_from_mat(thresholdedImage);

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
