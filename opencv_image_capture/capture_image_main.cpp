#include <opencv2/core/mat.hpp>   // For cv::Mat
#include <vector>                  // For std::vector
#include <algorithm>               // For std::copy
#include <iostream>                // For std::cout
#include <cstddef>                 // For std::size_t (though often included indirectly)

#include "capture_image.h"

/** ----------------------------------------------------
*   - Usage Example:
*   ----------------------------------------------------
*/
int main() {
    int capture_width = 640;
    int capture_height = 480;
    int framerate = 15;

    cv::Mat captured_frame = captureFrame(capture_width, capture_height, framerate);
    std::vector<float> processed_data = preprocessFrame(captured_frame);
    
    static float features[784];
    std::copy(processed_data.begin(), processed_data.end(), features);
       
    bool image_saved = saveImage(captured_frame, "output.png");
    bool text_saved = saveFeaturesText(features, 784, "output.txt");
    
    // Print out some data element
    constexpr size_t features_count = sizeof(features) / sizeof(features[0]);
    std::cout << "Static array 'features' size: " << features_count << std::endl;
    std::cout << "Value of features[0]: " << features[0] << std::endl;

}