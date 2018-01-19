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
    // Find contours inside the mask
    void getContour(std::vector<cv::Point>& contours);

    // Precompute MVC coordinates
    void MVCPrecompute();

    // Calculate all MVC coordinates
    void getMVCCoordinates();

    // Find the corresponding triangle for each point inside the mask
    void findTriangle();

    // Return whether a point (p) is inside a triangle (p0, p1, p2) or not
    // and calculate the triangle coordinates
    bool insideTriangle(const cv::Point& p, const cv::Point& p0, const cv::Point& p1, const cv::Point& p2, mvc::Vector3d& coords);

    // Source image
    cv::Mat src;

    // Source image mask
    cv::Mat mask;

    // Target image
    cv::Mat target;

    // Image size
    int h, w;

    // Flag for successful initialization
    bool initOK;

    // Boundary points and indexes
    std::vector<cv::Point> boundary;
    std::vector<int> boundaryIndex;

    // MVC vertex list
    std::vector<cv::Point> MVCVertices;

    // Triangle list
    std::vector<int> MVCTriangles;

    // Region point list
    std::vector<std::pair<int, mvc::Vector3d>> MVCRegionPoints;

    // MVC Coordinates
    double *mvc;

};

#endif
