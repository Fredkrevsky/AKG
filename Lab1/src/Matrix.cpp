#include "Matrix.hpp"
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>
#include <ranges>

Point Point::operator*(const TransformMatrix& matrix) const {
    return {
        matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z + matrix[0][3] * w,
        matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z + matrix[1][3] * w,
        matrix[2][0] * x + matrix[2][1] * y + matrix[2][2] * z + matrix[2][3] * w,
        matrix[3][0] * x + matrix[3][1] * y + matrix[3][2] * z + matrix[3][3] * w,
    };
}

double Point::dot(const Point& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Point Point::cross(const Point& other) const {
    return {
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x,
        0.0
    };
}

Point Point::operator+(const Point& other) const {
    return {x + other.x, y + other.y, z + other.z, w + other.w};
}

Point Point::operator-(const Point& other) const {
    return {x - other.x, y - other.y, z - other.z, w - other.w};
}

Point Point::operator*(double scalar) const {
    return {x * scalar, y * scalar, z * scalar, w * scalar};
}

Point& Point::normalize() {
    double length = std::sqrt(x * x + y * y + z * z);

    if (length == 0) return *this;

    x /= length;
    y /= length;
    z /= length;

    return *this;
}

TransformMatrix operator*(const TransformMatrix& a, const TransformMatrix& b) {
    TransformMatrix result{};
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result[i][j] = 0;
            for (int k = 0; k < 4; ++k) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return result;
}

std::vector<Point> mult(const TransformMatrix& matrix, const std::vector<Point>& points) {
    std::vector<Point> result;
    for (const auto& point : points) {
        result.push_back(point * matrix);
    }
    return result;
}

TransformMatrix createRotationX(double angle) {
    double c = std::cos(angle);
    double s = std::sin(angle);
    return {{
        {1, 0, 0, 0},
        {0, c, -s, 0},
        {0, s, c, 0},
        {0, 0, 0, 1}
    }};
}

TransformMatrix createRotationY(double angle) {
    double c = std::cos(angle);
    double s = std::sin(angle);
    return {{
        {c, 0, s, 0},
        {0, 1, 0, 0},
        {-s, 0, c, 0},
        {0, 0, 0, 1}
    }};
}

TransformMatrix createScale(double scalar){
    return {{
        {scalar, 0, 0, 0},
        {0, scalar, 0, 0},
        {0, 0, scalar, 0},
        {0, 0, 0, 1}
    }};
}

double normalize_angle(double angle) {
    angle = std::fmod(angle + PI, TWO_PI);
    return (angle < 0 ? angle + TWO_PI : angle) - PI;
}

void draw_line(Bitmap& bitmap, int width, int height, int x1, int y1, int x2, int y2) {

    const int size = bitmap.size();
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        int index = y1 * width + x1;
        if (index >= 0 && index < size) {
            bitmap[index] = WHITE;
        }
        else {
            break;
        }

        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void draw_face(Bitmap& bitmap, int width, int height, const std::span<Point>& points, const Face& face) {
    const size_t face_size = face.size();

    for (size_t i = 0; i < face_size; ++i) {
        const auto& point1 = points[face[i]];
        const auto& point2 = points[face[(i + 1) % face_size]];

        auto [x1, y1, z1, w1] = point1;
        auto [x2, y2, z2, w2] = point2;
        draw_line(bitmap, width, height, x1, y1, x2, y2);
    }
}

void draw_faces(Bitmap& bitmap, int width, int height, const std::span<Point>& points, const std::span<Face>& faces) {

    std::ranges::fill(bitmap, 0);

    std::ranges::for_each(faces, [&](const Face& face){
        draw_face(bitmap, width, height, points, face);
    });
}