#include <Matrix.hpp>
#include <array>
#include <vector>
#include <math.h>


Point3D Point3D::operator*(const TransformMatrix& matrix) const {
    Point3D result;
    result.x = matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z;
    result.y = matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z;
    result.z = matrix[2][0] * x + matrix[2][1] * y + matrix[2][2] * z;
    return result;
}


TransformMatrix createRotationX(double angle) {
    return {
        Vector3D{1, 0, 0},
        Vector3D{0, cos(angle), -sin(angle)},
        Vector3D{0, sin(angle), cos(angle)}
    };
}

TransformMatrix createRotationY(double angle) {
    return {
        Vector3D{cos(angle), 0, sin(angle)},
        Vector3D{0, 1, 0},
        Vector3D{-sin(angle), 0, cos(angle)}
    };
}

constexpr static double TWO_PI = 2.0 * M_PI;

double normalize_angle(double angle) {
    angle = fmod(angle + M_PI, TWO_PI);
    if (angle < 0) {
        angle += TWO_PI;
    }
    return angle - M_PI;
}