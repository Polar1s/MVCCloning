#include "MVCStitch.h"
#include "triangulate.h"
#include <cstdio>
#include <cstring>
#include <vector>

using namespace std;
using namespace cv;
using namespace mvc;

MVCStitch::MVCStitch() {
    initOK = false;
    mvc = NULL;
}

MVCStitch::~MVCStitch() {
    if (mvc) {
        delete[] mvc;
        mvc = NULL;
    }
}

void MVCStitch::getContour(vector<Point>& contours) {
    contours.clear();

    // Add brinks for input mask
    Mat tmp = Mat::zeros(h + 2, w + 2, mask.type());
    mask.copyTo(tmp(Rect(1, 1, w, h)));

    // Invoke OpenCV interface
    vector<vector<Point> > _contours;
    vector<Vec4i> hierarchy;
    findContours(tmp, _contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(-1, -1));

    // Retrieve the longest contour as result
    int idx = 0, maxLength = 0;
    int nContours = _contours.size();
    for (int i = 0; i < nContours; i++)
        if ((int)(_contours[i].size()) > maxLength) {
            maxLength = _contours[i].size();
            idx = i;
        }
    
    contours = _contours[idx];
}

void MVCStitch::getMVCCoordinates() {
    int nPoints = boundary.size();
    int nIndexes = boundaryIndex.size();
    int nVertices = MVCVertices.size();

    const double eps = 1e-6;

    // Calculate weights for each MVC vertex
    double *weights = new double[nPoints];
    double *mvcPtr = mvc;
    for (int i = 0; i < nVertices; i++, mvcPtr += nPoints) {
        // Compute weights
        Point vertex = MVCVertices[i];
        double sum = 0.0;

        Vector3d pointX(vertex.x + eps, vertex.y + eps, 0);

        for (int j = 0; j < nIndexes; j++) {
            int idx = boundaryIndex[j];
            Point cur = boundary[idx];
            
            int pre = idx == 0 ? nPoints - 1 : idx - 1;
            int next = idx == nPoints - 1 ? 0 : idx + 1;

            Vector3d p0(boundary[pre].x, boundary[pre].y);
            Vector3d p1(cur.x, cur.y, 1e-5);
            Vector3d p2(boundary[next].x, boundary[next].y);

            Vector3d v0 = p0 - pointX;
            Vector3d v1 = p1 - pointX;
            Vector3d v2 = p2 - pointX;

            double a01 = fabs(angleBetween(v0, v1));
            double a12 = fabs(angleBetween(v1, v2));

            weights[idx] = (tan(a01 / 2.0) + tan(a12 / 2.0)) / (v1.norm() + 0.001);
            sum += weights[idx];
        }

        // Store weights into mvc array
        for (int j = 0; j < nPoints; j++)
            mvcPtr[j] = fabs(sum) >= 1e-12 ? weights[j] / sum : 0.0;
    }

    delete[] weights;
    weights = NULL;
}

bool MVCStitch::insideTriangle(const Point& p, const Point& p0, const Point& p1, const Point& p2, Vector3d& coords) {
    Point v0(p.x - p0.x, p.y - p0.y);
    Point v1(p.x - p1.x, p.y - p1.y);
    Point v2(p.x - p2.x, p.y - p2.y);

    Point v10(p1.x - p0.x, p1.y - p0.y);
    Point v21(p2.x - p1.x, p2.y - p1.y);
    Point v02(p0.x - p2.x, p0.y - p2.y);

    int cross0 = v0.x * v10.y - v10.x * v0.y;
    int cross1 = v1.x * v21.y - v21.x * v1.y;
    int cross2 = v2.x * v02.y - v02.x * v2.y;
    int crossA = v10.x * v02.y - v02.x * v10.y;

    coords.z = (double)cross0 / crossA;
    coords.x = (double)cross1 / crossA;
    coords.y = (double)cross2 / crossA;

    return cross0 <= 0 && cross1 <= 0 && cross2 <= 0 && crossA < 0 ||
        cross0 >= 0 && cross1 >= 0 && cross2 >= 0 && crossA > 0;
}

void MVCStitch::findTriangle() {
    int nTriangles = MVCTriangles.size() / 3;
    MVCRegionPoints.clear();

    // Examine each triangle
    int curIndex = 0;
    for (int i = 0; i < nTriangles; i++, curIndex += 3) {
        // Extract vertices of a triangle
        Point p0 = MVCVertices[MVCTriangles[curIndex]];
        Point p1 = MVCVertices[MVCTriangles[curIndex + 1]];
        Point p2 = MVCVertices[MVCTriangles[curIndex + 2]];

        // First add Vertices (optional?)
        // MVCRegionPoints.push_back(make_pair<>(i, Vector3d(1.0, 0.0, 0.0)));
        // MVCRegionPoints.push_back(make_pair<>(i, Vector3d(0.0, 1.0, 0.0)));
        // MVCRegionPoints.push_back(make_pair<>(i, Vector3d(0.0, 0.0, 1.0)));

        // Calculate rectangle boundary
        int L = MIN(MIN(p0.x, p1.x), p2.x);
        int R = MAX(MAX(p0.x, p1.x), p2.x);
        int T = MIN(MIN(p0.y, p1.y), p2.y);
        int B = MAX(MAX(p0.y, p1.y), p2.y);

        // Find points located inside triangle and their coordinates for interpolation
        Vector3d coords;
        for (int cy = T; cy <= B; cy++)
            for (int cx = L; cx <= R; cx++)
                if (insideTriangle(Point(cx, cy), p0, p1, p2, coords))
                    MVCRegionPoints.push_back(make_pair<>(i, coords));
    }
}

void MVCStitch::MVCPrecompute() {
    // Region triangulation
    MVCTriangles.clear();
    triangulate(MVCVertices, MVCTriangles, mask);

    int nVertices = MVCVertices.size();
    int nPoints = boundary.size();

    // Calculate MVC coordinates
    if (mvc == NULL)
        mvc = new double[nVertices * nPoints];
    memset(mvc, 0, sizeof(double) * nVertices * nPoints);

    getMVCCoordinates();

    // Calculate region points inside triangular mesh
    findTriangle();
}

void MVCStitch::init(string srcFile, string maskFile) {
    // Read source and mask image
    src = imread(srcFile);
    mask = imread(maskFile, 0);

    // Save image size
    h = src.rows, w = src.cols;
    if (mask.rows != h || mask.cols != w) {
        printf("Error: source and mask are of different sizes\n");
        return;
    }

    // Find contours inside mask
    vector<Point> contours;
    getContour(contours);

    // Save boundary points (by uniquifying contour points)
    boundary.clear();
    int nPoints = contours.size();
    for (int i = 0; i < nPoints; i++)
        if (find(boundary.begin(), boundary.end(), contours[i]) == boundary.end())
            boundary.push_back(contours[i]);

    boundaryIndex.clear();
    nPoints = boundary.size();
    for (int i = 0; i < nPoints; i++) {
        int x = boundary[i].x, y = boundary[i].y;
        if (y < 0 || y >= h || x < 0 || x >= w) continue;
        if (mask.at<uchar>(y, x) > 0)
            boundaryIndex.push_back(i);
    }

    // Precompute MVC Coordinates
    MVCVertices = boundary;
    MVCPrecompute();

    // Set init flag to true
    initOK = true;
}

void MVCStitch::stitch(string targetFile, string outFile) {
    // Skip stitching process if init failed
    if (!initOK)
        return;
    
    // Read target image
    target = imread(targetFile);

    // Create result image according to target image type
    Mat res = target;

    // Calculate color difference at image boundary
    vector<int> boundaryDiff;
    boundaryDiff.clear();
    int nPoints = boundary.size();
    for (int i = 0; i < nPoints; i++) {
        Vec3b colorS = src.at<Vec3b>(boundary[i]);
        Vec3b colorT = target.at<Vec3b>(boundary[i]);
        
        boundaryDiff.push_back(colorS[2] - colorT[2]);
        boundaryDiff.push_back(colorS[1] - colorT[1]);
        boundaryDiff.push_back(colorS[0] - colorT[0]);
    }

    // Apply color difference to MVC vertices and record the difference value
    vector<double> membrane;
    membrane.clear();
    int nVertices = MVCVertices.size();
    double *mvcPtr = mvc;
    for (int i = 0; i < nVertices; i++, mvcPtr += nPoints) {
        double dr = 0.0, dg = 0.0, db = 0.0;
        
        // Compute net color difference
        for (int j = 0; j < nPoints; j++) {
            double weight = mvcPtr[j];
            dr += weight * boundaryDiff[j * 3];
            dg += weight * boundaryDiff[j * 3 + 1];
            db += weight * boundaryDiff[j * 3 + 2];
        }

        // Compute result color
        Vec3b color = target.at<Vec3b>(MVCVertices[i]);
        color[2] = (uchar)MAX(MIN(color[2] + dr, 255), 0);
        color[1] = (uchar)MAX(MIN(color[1] + dg, 255), 0);
        color[0] = (uchar)MAX(MIN(color[0] + db, 255), 0);

        res.at<Vec3b>(MVCVertices[i]) = color;

        membrane.push_back(dr);
        membrane.push_back(dg);
        membrane.push_back(db);
    }

    // Calculate color at all pixels
    int nRegionPoints = MVCRegionPoints.size();
    for (int i = 0; i < nRegionPoints; i++) {
        int no = MVCRegionPoints[i].first;
        Vector3d coords = MVCRegionPoints[i].second;

        int i0 = MVCTriangles[no * 3];
        int i1 = MVCTriangles[no * 3 + 1];
        int i2 = MVCTriangles[no * 3 + 2];

        // Current pixel location
        int x = cvRound(coords.x * MVCVertices[i0].x + coords.y * MVCVertices[i1].x + coords.z * MVCVertices[i2].x);
        int y = cvRound(coords.x * MVCVertices[i0].y + coords.y * MVCVertices[i1].y + coords.z * MVCVertices[i2].y);
        if (y < 0 || y >= h || x < 0 || x >= w) continue;

        // Compute net color difference
        double dr = coords.x * membrane[i0 * 3] + coords.y * membrane[i1 * 3] + coords.z * membrane[i2 * 3];
        double dg = coords.x * membrane[i0 * 3 + 1] + coords.y * membrane[i1 * 3 + 1] + coords.z * membrane[i2 * 3 + 1];
        double db = coords.x * membrane[i0 * 3 + 2] + coords.y * membrane[i1 * 3 + 2] + coords.z * membrane[i2 * 3 + 2];

        // Compute result color
        Vec3b color = target.at<Vec3b>(y, x);
        color[2] = (uchar)MAX(MIN(color[2] + dr, 255), 0);
        color[1] = (uchar)MAX(MIN(color[1] + dg, 255), 0);
        color[0] = (uchar)MAX(MIN(color[0] + db, 255), 0);

        res.at<Vec3b>(y, x) = color;
    }

    // Finally write output image
    imwrite(outFile, res);
}

