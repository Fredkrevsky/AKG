#pragma once
#include <array>
#include <vector>
#include <cmath>

using Vector3D = std::array<double, 3>;
using TransformMatrix = std::array<Vector3D, 3>;
using Face = std::vector<int>;

constexpr double PI = 3.141592653589793;
constexpr double TWO_PI = 2.0 * PI;

struct Point3D {
    double x{0.0}, y{0.0}, z{0.0};

    inline Point3D operator*(const TransformMatrix& matrix) const;
    inline Point3D& operator*=(const TransformMatrix& matrix);
    inline Point3D& operator+=(const Vector3D& other);
};

inline TransformMatrix operator*(const TransformMatrix& a, const TransformMatrix& b);

void rotate_vertices(std::vector<Point3D>& vertices, double angle_x, double angle_y);
void move_vertices(std::vector<Point3D>& vertices, double dx, double dy);

inline constexpr TransformMatrix createRotationX(double angle) {
    double c = std::cos(angle);
    double s = std::sin(angle);
    return {Vector3D{1, 0, 0}, Vector3D{0, c, -s}, Vector3D{0, s, c}};
}

inline constexpr TransformMatrix createRotationY(double angle) {
    double c = std::cos(angle);
    double s = std::sin(angle);
    return {Vector3D{c, 0, s}, Vector3D{0, 1, 0}, Vector3D{-s, 0, c}};
}

inline constexpr double normalize_angle(double angle) {
    angle = std::fmod(angle + PI, TWO_PI);
    return (angle < 0 ? angle + TWO_PI : angle) - PI;
}