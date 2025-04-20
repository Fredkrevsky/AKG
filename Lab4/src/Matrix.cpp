#include "Matrix.hpp"
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>


Vector4 Vector4::operator*(const TransformMatrix& matrix) const {
    return {
        matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z + matrix[0][3] * w,
        matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z + matrix[1][3] * w,
        matrix[2][0] * x + matrix[2][1] * y + matrix[2][2] * z + matrix[2][3] * w,
        matrix[3][0] * x + matrix[3][1] * y + matrix[3][2] * z + matrix[3][3] * w
    };
}

float Vector4::dot(const Vector4& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vector4 Vector4::cross(const Vector4& other) const {
    return {
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x,
        0.0
    };
}

Vector4 Vector4::operator+(const Vector4& other) const {
    return {x + other.x, y + other.y, z + other.z, w + other.w};
}

Vector4 Vector4::operator-(const Vector4& other) const {
    return {x - other.x, y - other.y, z - other.z, w - other.w};
}

Vector4 Vector4::operator*(float scalar) const {
    return {x * scalar, y * scalar, z * scalar, w * scalar};
}

Vector4& Vector4::operator+=(const Vector4& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;
    return *this;
}

Vector4 &Vector4::operator-=(const Vector4 &other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;
    return *this;
}

Vector4& Vector4::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;
    return *this;
}

Vector4& Vector4::operator*=(const TransformMatrix& matrix) {
    float new_x = matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z + matrix[0][3] * w;
    float new_y = matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z + matrix[1][3] * w;
    float new_z = matrix[2][0] * x + matrix[2][1] * y + matrix[2][2] * z + matrix[2][3] * w;
    float new_w = matrix[3][0] * x + matrix[3][1] * y + matrix[3][2] * z + matrix[3][3] * w;

    x = new_x;
    y = new_y;
    z = new_z;
    w = new_w;

    return *this;
}

Vector4& Vector4::normalize() {
    float length = std::sqrt(x * x + y * y + z * z);
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


Vector4 operator*(const TransformMatrix& matrix, const Vector4& point) {
    Vector4 result{
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

static TransformMatrix create_rotation_matrix_x(float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return {{
        {1, 0, 0, 0},
        {0, c, -s, 0},
        {0, s, c, 0},
        {0, 0, 0, 1}
    }};
}

static TransformMatrix create_rotation_matrix_y(float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return {{
        {c, 0, s, 0},
        {0, 1, 0, 0},
        {-s, 0, c, 0},
        {0, 0, 0, 1}
    }};
}

static TransformMatrix create_rotation_matrix_z(float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return {{
        {c, -s, 0, 0},
        {s, c, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};
}

TransformMatrix create_rotation_matrix(const Vector4& angles) {
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

TransformMatrix create_move_matrix(const Vector4& translation) {
    return {{
        {1, 0, 0, translation.x},
        {0, 1, 0, translation.y},
        {0, 0, 1, translation.z},
        {0, 0, 0, 1}
    }};
}

TransformMatrix create_scale_matrix(float scalar){
    return {{
        {scalar, 0, 0, 0},
        {0, scalar, 0, 0},
        {0, 0, scalar, 0},
        {0, 0, 0, 1}
    }};
}
