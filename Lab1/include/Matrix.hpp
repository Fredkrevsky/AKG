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
    Point3D& operator*=(const TransformMatrix& matrix);
    Point3D& operator+=(const Vector3D& other);
};

TransformMatrix operator*(const TransformMatrix& a, const TransformMatrix& b);

void rotate_vertices(std::vector<Point3D>& vertices, double angleX, double angleY);
void move_vertices(std::vector<Point3D>& vertices, double dx, double dy);

static TransformMatrix createRotationX(double angle);
static TransformMatrix createRotationY(double angle);
double normalize_angle(double angle);