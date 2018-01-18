#ifndef MVCSTITCH_H
#define MVCSTITCH_H

#include <opencv2/opencv.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <string>

class MVCStitch {
public:
    MVCStitch();
    ~MVCStitch();

    // Initialize source patch and mask
    void init(std::string srcFile, std::string maskFile);

    // Clone source image to target image, then save the result
    void stitch(std::string targetFile, std::string outFile);

private:
    // Source image
    cv::Mat src;

    // Source image mask
    cv::Mat mask;

    // Target image
    cv::Mat target;
}

#endif
