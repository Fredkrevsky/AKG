#include "Matrix.hpp"
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>


Point Point::operator*(const TransformMatrix& matrix) const {
    return {
        matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z + matrix[0][3] * w,
        matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z + matrix[1][3] * w,
        matrix[2][0] * x + matrix[2][1] * y + matrix[2][2] * z + matrix[2][3] * w,
        matrix[3][0] * x + matrix[3][1] * y + matrix[3][2] * z + matrix[3][3] * w
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

Point& Point::operator+=(const Point& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;
    return *this;
}

Point &Point::operator-=(const Point &other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;
    return *this;
}

Point& Point::operator*=(double scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;
    return *this;
}

Point& Point::operator*=(const TransformMatrix& matrix) {
    double new_x = matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z + matrix[0][3] * w;
    double new_y = matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z + matrix[1][3] * w;
    double new_z = matrix[2][0] * x + matrix[2][1] * y + matrix[2][2] * z + matrix[2][3] * w;
    double new_w = matrix[3][0] * x + matrix[3][1] * y + matrix[3][2] * z + matrix[3][3] * w;

    x = new_x;
    y = new_y;
    z = new_z;
    w = new_w;

    return *this;
}

Point& Point::normalize() {
    double length = std::sqrt(x * x + y * y + z * z);
    if (length > 1e-6) { 
        x /= length;
        y /= length;
        z /= length;
    } else { 
        x = y = 0;
        z = 1;
    }
    return *this;
}

void Point::set_color(double intensity) {
    uint8_t grey = static_cast<uint8_t>(std::clamp(0xFF * intensity, 0.0, 255.0));
    color = (0xFF << 24) | (grey << 16) | (grey << 8) | grey;
}


Point operator*(const TransformMatrix& matrix, const Point& point) {
    Point result{
        matrix[0][0] * point.x + matrix[0][1] * point.y + matrix[0][2] * point.z + matrix[0][3] * point.w,
        matrix[1][0] * point.x + matrix[1][1] * point.y + matrix[1][2] * point.z + matrix[1][3] * point.w,
        matrix[2][0] * point.x + matrix[2][1] * point.y + matrix[2][2] * point.z + matrix[2][3] * point.w,
        matrix[3][0] * point.x + matrix[3][1] * point.y + matrix[3][2] * point.z + matrix[3][3] * point.w
    };
    return result;
}

TransformMatrix operator*(const TransformMatrix& a, const TransformMatrix& b) {
    TransformMatrix result{};
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return result;
}

static TransformMatrix create_rotation_matrix_x(double angle) {
    double c = std::cos(angle);
    double s = std::sin(angle);
    return {{
        {1, 0, 0, 0},
        {0, c, -s, 0},
        {0, s, c, 0},
        {0, 0, 0, 1}
    }};
}

static TransformMatrix create_rotation_matrix_y(double angle) {
    double c = std::cos(angle);
    double s = std::sin(angle);
    return {{
        {c, 0, s, 0},
        {0, 1, 0, 0},
        {-s, 0, c, 0},
        {0, 0, 0, 1}
    }};
}

static TransformMatrix create_rotation_matrix_z(double angle) {
    double c = std::cos(angle);
    double s = std::sin(angle);
    return {{
        {c, -s, 0, 0},
        {s, c, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};
}

TransformMatrix create_rotation_matrix(const Point& angles) {
    TransformMatrix result = {{
        {1, 0, 0, 0}, 
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};

    if (angles.x != 0) {
        result = result * create_rotation_matrix_x(angles.x);
    }
    if (angles.y != 0) {
        result = result * create_rotation_matrix_y(angles.y);
    }
    if (angles.z != 0) {
        result = result * create_rotation_matrix_z(angles.z);
    }

    return result;
}

TransformMatrix create_move_matrix(const Point& translation) {
    return {{
        {1, 0, 0, translation.x},
        {0, 1, 0, translation.y},
        {0, 0, 1, translation.z},
        {0, 0, 0, 1}
    }};
}

TransformMatrix create_scale_matrix(double scalar){
    return {{
        {scalar, 0, 0, 0},
        {0, scalar, 0, 0},
        {0, 0, scalar, 0},
        {0, 0, 0, 1}
    }};
}

double normalize_angle(double angle) {
    angle = std::fmod(angle, TWO_PI);
    if (angle > PI) angle -= TWO_PI;
    if (angle < -PI) angle += TWO_PI;
    return angle;
}
