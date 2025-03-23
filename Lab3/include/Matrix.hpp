#pragma once
#include <array>
#include <vector>
#include <cmath>
#include <cstdint>
#include <numbers>
#include <span>

using TransformMatrix = std::array<std::array<double, 4>, 4>;
using Face = std::array<uint32_t, 3>;
using Faces = std::vector<Face>;

constexpr static double PI = std::numbers::pi_v<double>;
constexpr static double TWO_PI = 2.0 * PI;
constexpr static uint32_t WHITE = 0xFFFFFFFF;


struct Point {
    double x{0.0}, y{0.0}, z{0.0}, w{1.0};
    uint32_t color{0xFF'FF'FF'FF};
    
    double dot(const Point& other) const;
    Point cross(const Point& other) const;
    Point operator*(const TransformMatrix& matrix) const;
    Point operator+(const Point& other) const;
    Point operator-(const Point& other) const;
    Point operator*(double scalar) const;

    Point& operator+=(const Point& other);
    Point& operator-=(const Point& other);
    Point& operator*=(double scalar);
    Point& operator*=(const TransformMatrix& matrix);
    
    Point& normalize();

    void set_color(double intensity);
};

Point operator*(const TransformMatrix& matrix, const Point& point);

using Vertices = std::vector<Point>;
using Normal = Point;
using Normals = std::vector<Normal>; 

TransformMatrix operator*(const TransformMatrix& a, const TransformMatrix& b);
TransformMatrix create_rotation_matrix(const Point& rotate_vector);
TransformMatrix create_move_matrix(const Point& move_vector);
TransformMatrix create_scale_matrix(double scalar);

double normalize_angle(double angle);