#pragma once
#include <array>
#include <vector>
#include <cmath>
#include <cstdint>
#include <span>

using TransformMatrix = std::array<std::array<double, 4>, 4>;
using Bitmap = std::vector<uint32_t>;
using Face = std::vector<int>;

constexpr double PI = 3.141592653589793;
constexpr double TWO_PI = 2.0 * PI;

constexpr static uint32_t WHITE = 0xFFFFFFFF;

struct Point {
    double x{0.0}, y{0.0}, z{0.0}, w{1.0};

    double dot(const Point& other) const;
    Point cross(const Point& other) const;
    Point operator*(const TransformMatrix& matrix) const;
    Point operator+(const Point& other) const;
    Point operator-(const Point& other) const;
    Point operator*(double scalar) const;

    Point& normalize();
};

TransformMatrix operator*(const TransformMatrix& a, const TransformMatrix& b);
std::vector<Point> mult(const TransformMatrix& matrix, const std::vector<Point>& points);

TransformMatrix createRotationX(double angle);
TransformMatrix createRotationY(double angle);
TransformMatrix createScale(double scalar);

double normalize_angle(double angle);

void draw_line(Bitmap& bitmap, int width, int height, int x1, int y1, int x2, int y2);
void draw_face(Bitmap& bitmap, int width, int height, const std::span<Point>& points, const Face& face);
void draw_faces(Bitmap& bitmap, int width, int height, const std::span<Point>& points, const std::span<Face>& faces);