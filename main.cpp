#include <iostream>
#include <opencv2/opencv.hpp>
#include <potracelib.h>
#include <bitset>

using namespace std;
using namespace cv;

potrace_bitmap_t potrace_bitmap_from_mat(Mat image)
{
    // Create an instance of the potrace_bitmap_t struct
    potrace_bitmap_t bitmap;
    bitmap.w = image.cols;           // Width of the bitmap
    bitmap.h = image.rows;           // Height of the bitmap

    int N = sizeof(potrace_word) * 8;       // Number of bits per word, every bit is a pixel
    int wWithPad = (bitmap.w / N + 1) * N;  // Width of the bitmap with padding

    bitmap.dy = wWithPad / N;               // Number of words per scanline

    // Allocate memory for the bitmap, we need dy*h potrace_words
    bitmap.map = new potrace_word[bitmap.dy * bitmap.h];

    // Invert the image since potrace uses white for background (0) and black for foreground (1)
    bitwise_not(image, image);

    int currentWord = 0;    // Current word index in the map array
    int currentBit = 0;     // Current bit index in the current word (0 is most significant (left) bit)
    bitmap.map[currentWord] = 0;    // Initialize the first word to 0
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            uchar pixel = image.at<uchar>(i, j)/255;

            // Set or reset the bit in the current word depending on the pixel value
            potrace_word mask = 1ULL << (N - 1 - currentBit);   // Create a mask with a 1 at the current bit position
            if (pixel == 1)
                bitmap.map[currentWord] |= mask;    // Set the bit
            else bitmap.map[currentWord] &= ~mask;   // Reset the bit

            currentBit++;   // Move to the next bit
            if (currentBit == N) {  // If we have filled the current word
                currentBit = 0;     // Reset the bit index
                currentWord++;      // Move to the next word
                bitmap.map[currentWord] = 0;    // Initialize the next word to 0
            }
        }
        // Since every scanLine has dy words, we pad the remaining bit of currentWord with 0
        // Since we sat the word to 0, we just need to go to next word
        // The bits that we didn't visit are already 0
        currentWord++;  // Move to the next word
        currentBit = 0;
    }

    return bitmap;
}

Mat mat_from_potrace_bitmap(potrace_bitmap_t bitmap) {
    int N = sizeof(potrace_word) * 8; // Number of bits per word, every bit is a pixel

    // Create an empty Mat object with the desired size and type
    Mat image(bitmap.h, bitmap.w, CV_8UC1);

    // Loop through all pixels in the Potrace bitmap
    for (int i = 0; i < bitmap.h; i++) {
        for (int j = 0; j < bitmap.w; j++) {
            potrace_word pixel = ((bitmap.map + (i * bitmap.dy))[j / N] & (1ULL << (N - 1 - j % N))) ? 0 : 255;
            uchar pixel_char = static_cast<uchar>(pixel);

            // Set the pixel value in the image
            image.at<uchar>(i, j) = pixel_char;
        }
    }

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
    potrace_bitmap_t bitmap = potrace_bitmap_from_mat(thresholdedImage);

    // Create a parameter structure with default values
    potrace_param_t* param = potrace_param_default();

    // Trace the bitmap to a path
    potrace_state_t* state = potrace_trace(param, &bitmap);

    // Convert the bitmap back to a Mat
    Mat tracedImage = mat_from_potrace_bitmap(bitmap);
    imshow("Traced image", tracedImage);
    waitKey(0);

    return 0;
}
