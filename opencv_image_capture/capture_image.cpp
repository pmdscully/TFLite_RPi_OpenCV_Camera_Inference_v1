
#include "capture_image.h"

#include <opencv2/opencv.hpp>   // Core OpenCV functionality
#include <opencv2/imgproc.hpp> // Image processing functions (cvtColor, resize)
#include <iostream>             // For std::cerr, std::cout (minimized)
#include <fstream>              // Required for std::ofstream (file output)
#include <string>               // For std::string
#include <vector>               // For std::vector<float> return type

/** ----------------------------------------------------
*   - Capture image
*   ----------------------------------------------------
*/


// --- GStreamer Pipeline ---
// Creates the pipeline string for camera capture.
std::string gstreamer_pipeline(int capture_width, int capture_height, int framerate) {
    return "libcamerasrc ! video/x-raw, width=(int)" + std::to_string(capture_width) +
           ", height=(int)" + std::to_string(capture_height) +
           ", framerate=(fraction)" + std::to_string(framerate) +
           "/1 ! videoconvert ! video/x-raw, format=BGR ! appsink drop=true max-buffers=1";
}

// --- Frame Capture ---
// Captures a single frame from the camera.
cv::Mat captureFrame(int capture_width, int capture_height, int framerate) {
    std::string pipeline = gstreamer_pipeline(capture_width, capture_height, framerate);
    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);

    if (!cap.isOpened()) {
        std::cerr << "ERROR: Failed to open camera." << std::endl;
        std::cout << "ERROR: Failed to open camera." << std::endl;
        return cv::Mat();
    }

    cv::Mat frame;
    if (!cap.read(frame)) {
        std::cerr << "ERROR: Capture read error." << std::endl;
        std::cout << "ERROR: Capture read error." << std::endl;
        // cap.release() happens automatically when cap goes out of scope here
        return cv::Mat();
    }
    return frame;
}

/** ----------------------------------------------------
*   - Helpers - Write data to files.
*   ----------------------------------------------------
*/
bool saveImage(const cv::Mat& image, const std::string& filename) {
    if (image.empty()) {
        std::cerr << "ERROR: Cannot save empty image to " << filename << std::endl;
        return false;
    }
    try {
        bool success = cv::imwrite(filename, image);
        if (!success) {
            std::cerr << "ERROR: Failed to save image to " << filename << std::endl;
            return false;
        } else {
            std::cout << "Image saved successfully to " << filename << std::endl;
            return true;
        }
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV Error during image save to " << filename << ": " << e.what() << std::endl;
        return false;
    }
}

bool saveFeaturesText(const float* data, size_t count, const std::string& filename) {
    if (data == nullptr || count == 0) {
        std::cerr << "ERROR: Invalid data provided for saving to " << filename << std::endl;
        return false;
    }

    std::ofstream outputFile((std::string) filename, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "ERROR: Failed to open " << filename << " for writing." << std::endl;
        return false;
    }

    // outputFile << std::fixed << std::setprecision(8); // Optional: set float formatting

    for (size_t i = 0; i < count; ++i) {
        outputFile << data[i] << "\n";
        if (outputFile.fail()) { // Check for errors during write operation
            std::cerr << "ERROR: Failed while writing data to " << filename << std::endl;
            outputFile.close(); // Attempt to close file even on error
            return false;
        }
    }

    outputFile.close();

    // Final check after closing (primarily checks if close itself failed, less common)
    if (outputFile.fail()) {
         std::cerr << "ERROR: File stream indicates failure after closing " << filename << std::endl;
         return false;
    }

    std::cout << "Features data saved successfully to " << filename << std::endl;
    return true;
}

/** ----------------------------------------------------
*   - Helpers - Image Preprocessing for MNIST model
*   ----------------------------------------------------
*/
/**
 * @brief Converts frame to grayscale, resizes to 28x28, normalizes (0.0=black, 1.0=white),
 * and flattens to a 784-element vector.
 * @param inputFrame The input color frame (BGR, assumed cv::Mat).
 * @return std::vector<float> containing 784 normalized pixel values. Empty on error.
 */
std::vector<float> preprocessFrame(const cv::Mat& inputFrame) {
    cv::Mat grayFrame, resizedFrame, floatFrame;
    std::vector<float> outputVector;
    try {
        // Convert to Grayscale
        cv::cvtColor(inputFrame, grayFrame, cv::COLOR_BGR2GRAY);
        // Resize to 28x28
        cv::resize(grayFrame, resizedFrame, cv::Size(28, 28), 0, 0, cv::INTER_AREA);
        resizedFrame.convertTo(floatFrame, CV_32F);

        // Normalize to [0.0, 1.0] where 0.0=black, 1.0=white (MNIST format)
        //////// floatFrame = floatFrame / 255.0;
        // Normalize and invert: Map [0.0, 255.0] to [1.0=black, 0.0=white] (expecting white background and black handwriting)
        // Original black (0.0) becomes 1.0 - (0.0 / 255.0) = 1.0
        // Original white (255.0) becomes 1.0 - (255.0 / 255.0) = 0.0
        floatFrame = 1.0 - (floatFrame / 255.0);
        
        // Truncate (background) white values below a threshold:
        // Values >= 0.15 remain unchanged.
        // cv::threshold(floatFrame, /* src */
                      // floatFrame, /* dst (in-place) */
                      // 0.15,       /* thresh */
                      // 1.0,        /* maxval (unused for this type) */
                      // cv::THRESH_TOZERO);
        
        // Reshape (no data copy if continuous) and get pointer to start
        float* p = floatFrame.ptr<float>(0);
        // Assign data directly to the vector
        outputVector.assign(p, p + floatFrame.total());

    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV Error during preprocessing: " << e.what() << std::endl;
        return {}; // Return empty vector on exception
    }

    return outputVector;
}
