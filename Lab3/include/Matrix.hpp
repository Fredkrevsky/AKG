#pragma once
#include <array>
#include <vector>
#include <cmath>
#include <cstdint>
#include <numbers>
#include <algorithm>
#include <span>


using TransformMatrix = std::array<std::array<double, 4>, 4>;
using Face = std::array<uint32_t, 3>;
using Faces = std::vector<Face>;

constexpr static double PI = std::numbers::pi_v<double>;
constexpr static double TWO_PI = 2.0 * PI;

struct Vector4D {
    double x{0.0}, y{0.0}, z{0.0}, w{1.0};
    
    double dot(const Vector4D& other) const;
    Vector4D cross(const Vector4D& other) const;
    Vector4D operator*(const TransformMatrix& matrix) const;
    Vector4D operator+(const Vector4D& other) const;
    Vector4D operator-(const Vector4D& other) const;
    Vector4D operator*(double scalar) const;

    Vector4D& operator+=(const Vector4D& other);
    Vector4D& operator-=(const Vector4D& other);
    Vector4D& operator*=(double scalar);
    Vector4D& operator*=(const TransformMatrix& matrix);
    
    Vector4D& normalize();
};

Vector4D operator*(const TransformMatrix& matrix, const Vector4D& point);

using Vertices = std::vector<Vector4D>;
using Normal = Vector4D;
using Normals = std::vector<Normal>; 

TransformMatrix operator*(const TransformMatrix& a, const TransformMatrix& b);
TransformMatrix create_rotation_matrix(const Vector4D& rotate_vector);
TransformMatrix create_move_matrix(const Vector4D& move_vector);
TransformMatrix create_scale_matrix(double scalar);
