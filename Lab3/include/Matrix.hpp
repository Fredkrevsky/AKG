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
constexpr static uint32_t WHITE = 0xFFFFFFFF;

namespace Color{
    constexpr static uint32_t BLACK = 0xFF00'0000;
    constexpr static uint32_t RED   = 0xFF00'00FF;
    constexpr static uint32_t BLUE  = 0xFF00'FF00;
    constexpr static uint32_t GREEN = 0xFFFF'0000;
    constexpr static uint32_t WHITE = 0xFFFF'FFFF;
    constexpr static uint32_t NO_COLOR = 0;

    inline uint32_t get_mono_color(uint8_t grey){
        return BLACK | (grey << 16) | (grey << 8) | grey;
    }

    inline uint8_t get_grey(uint32_t color){
        return color & 255;
    }

    inline uint32_t from_intensity(double intensity){
        uint8_t grey = static_cast<uint8_t>(std::clamp(0xFF * intensity, 0.0, 255.0));
        uint32_t color = (0xFF << 24) | (grey << 16) | (grey << 8) | grey;
        return color;
    }
}

struct Vector4D {

    double x{0.0}, y{0.0}, z{0.0}, w{1.0};
    uint32_t color{Color::WHITE};
    
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

double normalize_angle(double angle);