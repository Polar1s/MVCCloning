# MVCCloning
MVC Cloning for Media Computing Course (Fall 2017)

## Prerequisites
CMake (version 3.5)

OpenCV

CGAL

## Build
```
mkdir build
cd build
cmake ..
make
```

## Run
Input images: input.bmp, target.jpg
```
./mvcclone [source] [target] [output] [offsetY=0] [offsetX=0]
```

To run examples inside "demo/" folder, execute the shell script demo/test.sh. The results are saved as "output*.jpg".
```
cd demo
./test.sh
```

## Disclaimer

This software is exclusively used for course project. All rights reserved.
