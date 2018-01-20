#include "MVCStitch.h"
#include <opencv2/opencv.hpp>
#include <cstdio>
#include <string>
#include <sys/time.h>
#include <unistd.h>

using namespace std;
using namespace cv;

int main(int argc, char **argv) {
    if (argc < 4) {
        printf("Usage: mvcclone [source] [target] [output] [offsetY=0] [offsetX=0]\n");
        return 0;
    }

    MVCStitch stitch;

    int offsetY = argc >= 5 ? atoi(argv[4]) : 0;
    int offsetX = argc >= 6 ? atoi(argv[5]) : 0;
    stitch.offset = Point(offsetX, offsetY);

    // Time counter
    struct timeval start, end;
    gettimeofday(&start, NULL);

    stitch.init(string(argv[1]));
    printf("init OK.\n");

    stitch.stitch(string(argv[2]), string(argv[3]));
    printf("Stitching finished.\n");

    gettimeofday(&end, NULL);
    double sec = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) * 1e-6;
    printf("Time elapsed: %.3lfs\n", sec);

    return 0;
}
