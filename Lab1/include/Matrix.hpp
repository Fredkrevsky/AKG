#pragma once
#include <array>
#include <vector>
#include <cmath>
#include <cstdint>
#include <span>
#include "ThreadPool.hpp"

using TransformMatrix = std::array<std::array<double, 4>, 4>;
using Face = std::vector<int>;
using Faces = std::vector<Face>;

constexpr double PI = 3.141592653589793;
constexpr double TWO_PI = 2.0 * PI;
constexpr static uint32_t WHITE = 0xFFFFFFFF;

static constexpr int THREADS_COUNT = 12;
static ThreadPool thread_pool{THREADS_COUNT};

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

Point operator*(const TransformMatrix& matrix, const Point& point);

using Vertices = std::vector<Point>;

class Bitmap {
public:
    Bitmap(int width, int height) noexcept;
    
    const uint8_t* data() const;
    void clear();
    void draw_faces(const Vertices& points, 
                    const Faces& faces);

private:

    std::vector<uint32_t> m_data; 
    int m_width;
    int m_height;
};

TransformMatrix operator*(const TransformMatrix& a, const TransformMatrix& b);
Vertices mult(const TransformMatrix& matrix, const Vertices& points);

TransformMatrix create_rotation_matrix(const Point& rotate_vector);
TransformMatrix create_move_matrix(const Point& move_vector);
TransformMatrix create_scale_matrix(double scalar);

double normalize_angle(double angle);