#pragma once
#include <array>
#include <vector>
#include <math.h>

using Vector3D = std::array<double, 3>;
using TransformMatrix = std::array<Vector3D, 3>;
using Face = std::vector<int>;

struct Point3D {

    double x{0.0}, y{0.0}, z{0.0};

    Point3D operator*(const TransformMatrix& matrix) const;
};


TransformMatrix createRotationX(double angle);

TransformMatrix createRotationY(double angle);

double normalize_angle(double angle);