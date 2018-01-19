#include "MVCStitch.h"
#include <opencv2/opencv.hpp>
#include <cstdio>

using namespace std;

int main(int argc, char **argv) {
    MVCStitch stitch;

    stitch.init("input.jpg", "mask.jpg");
    stitch.stitch("target.jpg", "output.jpg");

    return 0;
}
