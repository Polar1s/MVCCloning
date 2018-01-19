#ifndef MVC_VECTOR3D_H
#define MVC_VECTOR3D_H

#include <cmath>

namespace mvc {

class Vector3d {
public:
    double x, y, z;

    Vector3d(double x = 0.0, double y = 0.0, double z = 0.0): x(x), y(y), z(z) {}
    ~Vector3d() {}

    Vector3d& operator = (const Vector3d& vec) {
        x = vec.x;
        y = vec.y;
        z = vec.z;
        return *this;
    }

    Vector3d operator + (const Vector3d& vec) const {
        return Vector3d(x + vec.x, y + vec.y, z + vec.z);
    }

    Vector3d operator - (const Vector3d& vec) const {
        return Vector3d(x - vec.x, y - vec.y, z - vec.z);
    }

    Vector3d operator * (double k) const {
        return Vector3d(x * k, y * k, z * k);
    }

    Vector3d operator / (double k) const {
        return Vector3d(x / k, y / k, z / k);
    }

    double dot(const Vector3d& vec) const {
        return x * vec.x + y * vec.y + z * vec.z;
    }

    double norm() const {
        return sqrt(x * x + y * y + z * z);
    }
};

double angleBetween(const Vector3d& v1, const Vector3d& v2);

}

#endif
