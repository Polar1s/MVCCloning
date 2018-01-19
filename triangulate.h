#ifndef TRIANGULATE_H
#define TRIANGULATE_H

#include <opencv2/opencv.hpp>
#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>

// Delaunay triangulation
void triangulate(std::vector<cv::Point>& points, std::vector<int>& tris, const cv::Mat& mask);

#endif