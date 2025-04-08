# OpenCV_Camera_Inference_v1

This project is tightly coupled with various dependencies. It overwrites (small updates to [edgeimpulse/example-standalone-inferencing-c](https://github.com/edgeimpulse/example-standalone-inferencing-c) repo). It depends on the RPi v4 image (Debian version and library installs) used within our 522 subject.

## Code Updates:

* **Adds directory `opencv_image_capture` containing `capture_image.cpp` and `capture_image.h` files to:**

  * Read a single capture image via `GStreamer`, 
  * Pre-processing on the image data, according to MNIST Dataset's expected format:
    * Inverted (white paper becomes black background)
    * Normalized (black=0.0-white=1.0)
    * Resolution to 28x28.
    * Flatten to 784x1 array
  * Transfer the returned array into `features` array.

* *Contains a main.cpp file for isolated testing:*

  * ```c++
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
        
        // features now ready for use within Edge Impulse...
           
        bool image_saved = saveImage(captured_frame, "output.png");
        bool text_saved = saveFeaturesText(features, 784, "output.txt");
        
        // Print out some data element
        constexpr size_t features_count = sizeof(features) / sizeof(features[0]);
        std::cout << "Static array 'features' size: " << features_count << std::endl;
        std::cout << "Value of features[0]: " << features[0] << std::endl;
    
    }
    ```

* **Replaces the `Makefile`, to include:**

  * ```makefile
    
    # Define OpenCV compiler flags variable
    OPENCV_CFLAGS = $(shell pkg-config --cflags opencv4) # Use 'opencv' if 'opencv4' fails
    # Define OpenCV linker flags variable
    OPENCV_LIBS = $(shell pkg-config --libs opencv4) # Use 'opencv' if 'opencv4' fails
    
    
    # C++ only compiler flags
    CXXFLAGS += -std=c++11  $(OPENCV_CFLAGS)				# Use C++11 standard
    
    # Linker flags
    LDFLAGS += -lm  -lstdc++ $(OPENCV_LIBS) 						# Link to math.h
    
    # Include C++ source code for required libraries
    CXXSOURCES += 	$(wildcard tflite-model/*.cpp) \
                    $(wildcard opencv_image_capture/capture_image.cpp) \
    				$(wildcard edge-impulse-sdk/dsp/kissfft/*.cpp) \
    				...
    ```

* Replaces the top of `source/main.cpp` file:

  * ```c++
    #include <stdio.h>
    //#include "features.h"
    #include <opencv2/core/mat.hpp>   // For cv::Mat
    #include <vector>                  // For std::vector
    #include <algorithm>               // For std::copy
    #include "capture_image.h"
    
    
    #include "edge-impulse-sdk/classifier/ei_run_classifier.h"
    
    // Callback function declaration
    static int get_signal_data(size_t offset, size_t length, float *out_ptr);
    
    // Raw features copied from test sample
    // static const float features[] = {
        // Copy raw features here (e.g. from the 'Model testing' page)
    // 
    static float features[784];
    
    int main(int argc, char **argv) {
    
        int capture_width = 640;
        int capture_height = 480;
        int framerate = 15;
    
        cv::Mat captured_frame = captureFrame(capture_width, capture_height, framerate);
        std::vector<float> processed_data = preprocessFrame(captured_frame);
        
        std::copy(processed_data.begin(), processed_data.end(), features);
    	
        //...
        
    };       
        
    ```

  * 