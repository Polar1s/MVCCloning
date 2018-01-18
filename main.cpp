#include <opencv2/opencv.hpp>
#include <cstdio>
#include "MVCStitch.h"

using namespace std;
using namespace cv;

int main(int argc, char **argv) {
    MVCStitch stitch;

    stitch.init("input.jpg", "mask.jpg");
    stitch.stitch("target.jpg", "output.jpg");

    return 0;
}
