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
    cout << bitmap.dy * bitmap.h << endl;

    // Invert the image since potrace uses white for background (0) and black for foreground (1)
    bitwise_not(image, image);

    cout << "going int othe loop" << endl;

    int currentWord = 0;    // Current word index in the map array
    int currentBit = 0;     // Current bit index in the current word
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
        // Since every scanLine changes the word, we need to move to the next word
        // Since we sat the next word to 0, we don't need to do anything else
        // The bits that we didn't visit are already 0
        currentWord++;  // Move to the next word
        currentBit = 0;
        if(i == 1) break;   // Only do the first few row for debugging
    }

    return bitmap;
}

Mat mat_from_potrace_bitmap(potrace_bitmap_t* bitmap) {
    int N = sizeof(potrace_word) * 8; // Number of bits per word, every bit is a pixel

    // Create an empty Mat object with the desired size and type
    //Mat image(bitmap->h, bitmap->w, CV_8UC1, Scalar(0));

    // Loop through all pixels in the Potrace bitmap
    for (int y = 0; y < bitmap->h; y++) { // Top to bottom
        for (int x = 0; x < bitmap->w; x++) { // Left to right
            // Get the pixel value from the Potrace bitmap using the formula
            int pixel = ((bitmap->map + (y * bitmap->dy))[x / N] & (1ULL << (N - 1 - x % N))) ? 0 : 255;

            // Set the pixel value in the OpenCV Mat
            //image.at<uchar>(bitmap->h - 1 - y, x) = pixel;
            cout << pixel/255 << " ";
        }
        cout << endl;
    }

    //return image;
    return Mat();
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
    imshow("image", image);
    waitKey(0);

    // Convert image to grayscale
    Mat grayImage;
    cvtColor(image, grayImage, COLOR_BGR2GRAY);
    imshow("grayImage", grayImage);
    waitKey(0);

    // Perform thresholding
    Mat thresholdedImage;
    threshold(grayImage, thresholdedImage, 128, 255, THRESH_BINARY);
    imshow("thresholdedImage", thresholdedImage);
    waitKey(0);

    // Convert the image to a bitmap
    potrace_bitmap_t bitmap = potrace_bitmap_from_mat(thresholdedImage);
    cout << "bitmap created" << endl;

    // Display the bitmap
    //Mat out = mat_from_potrace_bitmap(bitmap);
    //imshow("bitmap", out);
    //waitKey(0);

    Mat clone = image.clone();
    imshow("clone", clone);
    waitKey(0);

    return 0;
}
