#include <Matrix.hpp>
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>
#include <ranges>

inline Point3D Point3D::operator*(const TransformMatrix& matrix) const {
    return {
        matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z,
        matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z,
        matrix[2][0] * x + matrix[2][1] * y + matrix[2][2] * z
    };
}

inline Point3D& Point3D::operator*=(const TransformMatrix& matrix) {
    return *this = *this * matrix;
}

inline Point3D& Point3D::operator+=(const Vector3D& other) {
    x += other[0];
    y += other[1];
    z += other[2];
    return *this;
}

inline TransformMatrix operator*(const TransformMatrix& a, const TransformMatrix& b) {
    TransformMatrix result{};
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            result[i][j] = a[i][0] * b[0][j] + a[i][1] * b[1][j] + a[i][2] * b[2][j];
    return result;
}

void rotate_vertices(std::vector<Point3D>& vertices, double angle_x, double angle_y) {
    auto rotation = createRotationX(normalize_angle(angle_x)) * createRotationY(normalize_angle(angle_y));
    std::ranges::for_each(vertices, [&](auto& vertex) { vertex *= rotation; });
}

void move_vertices(std::vector<Point3D>& vertices, double dx, double dy) {
    Vector3D move_vector{dx, dy, 0};
    std::ranges::for_each(vertices, [&](auto& vertex) { vertex += move_vector; });
}