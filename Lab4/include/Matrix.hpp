#pragma once
#include <array>
#include <vector>
#include <cmath>
#include <cstdint>
#include <numbers>
#include <algorithm>
#include <span>


using TransformMatrix = std::array<std::array<float, 4>, 4>;
using Face = std::array<uint32_t, 3>;
using Faces = std::vector<Face>;

constexpr static float PI = std::numbers::pi_v<float>;
constexpr static float TWO_PI = 2.0 * PI;

struct Vector4 {
    float x{0.0}, y{0.0}, z{0.0}, w{1.0};
    
    float dot(const Vector4& other) const;
    Vector4 cross(const Vector4& other) const;
    Vector4 operator*(const TransformMatrix& matrix) const;
    Vector4 operator+(const Vector4& other) const;
    Vector4 operator-(const Vector4& other) const;
    Vector4 operator*(float scalar) const;

    Vector4& operator+=(const Vector4& other);
    Vector4& operator-=(const Vector4& other);
    Vector4& operator*=(float scalar);
    Vector4& operator*=(const TransformMatrix& matrix);
    
    Vector4& normalize();
};

Vector4 operator*(const TransformMatrix& matrix, const Vector4& point);

using Vertex = Vector4;
using Normal = Vector4;
using Vertices = std::vector<Vertex>;
using Normals = std::vector<Normal>;

TransformMatrix operator*(const TransformMatrix& a, const TransformMatrix& b);
TransformMatrix create_rotation_matrix(const Vector4& rotate_vector);
TransformMatrix create_move_matrix(const Vector4& move_vector);
TransformMatrix create_scale_matrix(float scalar);
