
#ifndef CAPTURE_IMAGE_H
#define CAPTURE_IMAGE_H

#include <opencv2/opencv.hpp>   // Core OpenCV functionality
#include <opencv2/imgproc.hpp> // Image processing functions (cvtColor, resize)
#include <iostream>             // For std::cerr, std::cout (minimized)
#include <fstream>              // Required for std::ofstream (file output)
#include <string>               // For std::string
#include <vector>               // For std::vector<float> return type

std::string gstreamer_pipeline(int capture_width, int capture_height, int framerate) ;
cv::Mat captureFrame(int capture_width, int capture_height, int framerate);
bool saveImage(const cv::Mat& image, const std::string& filename) ;
bool saveFeaturesText(const float* data, size_t count, const std::string& filename);
std::vector<float> preprocessFrame(const cv::Mat& inputFrame);

#endif //CAPTURE_IMAGE_H
