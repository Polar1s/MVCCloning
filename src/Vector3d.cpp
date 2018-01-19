#include "Vector3d.h"
#include <cmath>

double mvc::angleBetween(const Vector3d& v1, const Vector3d& v2) {
    double cosAngle = v1.dot(v2) / (v1.norm() * v2.norm());
    if (cosAngle > 1.0) cosAngle = 1.0;
    if (cosAngle < -1.0) cosAngle = -1.0;
    return acos(cosAngle);
}

