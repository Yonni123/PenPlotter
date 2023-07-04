#include <iostream>
#include <opencv2/opencv.hpp>
#include <potracelib.h>

using namespace std;

potrace_bitmap_t *potrace_bitmap_from_mat(cv::Mat image) {
    potrace_bitmap_t *bitmap = (potrace_bitmap_t *) malloc(sizeof(potrace_bitmap_t));
    bitmap->w = image.cols +200;    // Width of the bitmap
    bitmap->h = image.rows;   // Height of the bitmap
    int N = sizeof(potrace_word) * 8;  // Number of bits per word, every bit is a pixel

    // Make the width a multiple of N by padding
    cout << "old w: " << bitmap->w << endl;  // 3
    if (bitmap->w % N != 0) {
        bitmap->w = (bitmap->w / N + 1) * N;
    }
    cout << "new w: " << bitmap->w << endl;  // 32

    bitmap->dy = bitmap->w / N;  // Number of words per scanline
    cout << "dy: " << bitmap->dy << endl;  // 1
    
    // Allocate memory for the bitmap, we need dy*h potrace_words
    bitmap->map = (potrace_word *) malloc(bitmap->dy * bitmap->h * sizeof(potrace_word));

    // Invert the image since potrace uses white for background (0) and black for foreground (1)
    cv::bitwise_not(image, image);

    cv::imshow("image", image);
    cv::waitKey(0);

    // Loop through all pixels and create potrace_words
    int current_word = 0;
    int current_bit = 0;
    for(int i=image.rows-1; i>=0; i--){     // Bottom to top
        for(int j=0; j<bitmap->w; j++){    // Left to right
            // Get the pixel value (0 or 1)
            int pixel = 0;
            if(j < image.cols){    // If still within image
                pixel = image.at<uchar>(i, j)/255;  // Convert pixel to 0 or 1
            }
            
            // Set the bit in the potrace_word if the pixel is set
            potrace_word shift = (1 << (N-1-current_bit));  // Create a mask
            if(pixel == 1){
                bitmap->map[current_word] |= shift;  // Set the bit
            } else {
                bitmap->map[current_word] &= ~shift;  // Clear the bit
            }

            // Increment the current bit
            current_bit++;
            if(current_bit == N){
                current_bit = 0;
                current_word++;
                
                // print the potrace_word for debugging
                cout << bitmap->map[current_word-1] << " ";
            }
        }
        cout << endl;
    }


    return bitmap;
}

// This function saves a bitmap as a .bmp file
void save_bitmap(potrace_bitmap_t *bitmap){
    // Create a new image
    cv::Mat image(bitmap->h, bitmap->w, CV_8UC1);

    // Copy the bitmap data to the image
    int N = sizeof(potrace_word) * 8;
    cout << "N: " << N << endl;  // N = 32
    int dy = bitmap->dy;
    cout << "dy: " << dy << endl;  // dy = 4
    for (int i = 0; i < bitmap->h; i++) {
        for (int j = 0; j < bitmap->w; j++) {
            potrace_word word = (bitmap->map + j*dy)[i/N];  // Get the word containing the pixel
            potrace_word shift = (1 << (N-1-i%N));
            int pixel = (word & shift) ? 255 : 0;
            image.at<uchar>(i, j) = pixel;
        }
    }

    // Save the image
    cv::imwrite("output.bmp", image);
}

int main() {
    // Load and display an image using OpenCV
    cv::Mat image = cv::imread("../TestImages/cropped.jpg", cv::IMREAD_COLOR);
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
    int N = sizeof(potrace_word) * 8;
    int dy = thresholdedImage.cols / N + 1; // Number of words per row
    potrace_bitmap_t *bitmap = potrace_bitmap_from_mat(thresholdedImage);

    //save_bitmap(bitmap);
    
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
