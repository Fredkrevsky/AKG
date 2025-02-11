#include <Matrix.hpp>
#include <array>
#include <vector>
#include <math.h>
#include <algorithm>


Point3D Point3D::operator*(const TransformMatrix& matrix) const {
    Point3D result;
    result.x = matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z;
    result.y = matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z;
    result.z = matrix[2][0] * x + matrix[2][1] * y + matrix[2][2] * z;
    return result;
}

Point3D& Point3D::operator*=(const TransformMatrix& matrix) {
    double new_x = matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z;
    double new_y = matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z;
    double new_z = matrix[2][0] * x + matrix[2][1] * y + matrix[2][2] * z;
    x = new_x;
    y = new_y;
    z = new_z;
    return *this;
}

Point3D& Point3D::operator+=(const Vector3D& other) {
    x += other[0];
    y += other[1];
    z += other[2];
    return *this;
}


TransformMatrix operator*(const TransformMatrix& a, const TransformMatrix& b) {
    TransformMatrix result{};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            result[i][j] = 0;
            for (int k = 0; k < 3; ++k) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return result;
}

void rotate_vertices(std::vector<Point3D>& vertices, double angleX, double angleY){
    angleX = normalize_angle(angleX);
    angleY = normalize_angle(angleY);
    auto rotationX = createRotationX(angleX);
    auto rotationY = createRotationY(angleY);
    auto rotation = rotationX * rotationY;

    std::ranges::for_each(vertices, [&](auto& vertex) {
        vertex *= rotation;
    });
}

void move_vertices(std::vector<Point3D>& vertices, double dx, double dy){
    Vector3D moving_vector{dx, dy, 0};
    std::ranges::for_each(vertices, [&](auto& vertex) {
        vertex += moving_vector;
    });
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

constexpr double PI = 3.1415926538;
constexpr static double TWO_PI = 2.0 * PI;

double normalize_angle(double angle) {
    angle = fmod(angle + PI, TWO_PI);
    if (angle < 0) {
        angle += TWO_PI;
    }
    return angle - PI;
}