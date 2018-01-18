#include "MVCStitch.h"

using namespace std;
using namespace cv;

MVCStitch::MVCStitch() {

}

MVCStitch::~MVCStitch() {

}

void MVCStitch::init(string srcFile, string maskFile) {
    // Read source and mask image
    src = imread(srcFile, 0);
    mask = imread(maskFile, 0);

    
}

void MVCStitch::stitch(string targetFile, string outFile) {

}

