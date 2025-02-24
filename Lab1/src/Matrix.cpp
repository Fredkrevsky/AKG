#include "Matrix.hpp"
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>
#include <ranges>
#include <thread>
#include <chrono>
#include <future>
#include <iostream>

using namespace std::chrono_literals;

Bitmap::Bitmap(int width, int height) noexcept 
    : m_data(width * height, 0)
    , m_width(width)
    , m_height(height)
    {}

void Bitmap::clear(){
    std::ranges::fill(m_data, 0);
}

const uint8_t* Bitmap::data() const {
    return reinterpret_cast<const uint8_t*>(m_data.data());
}

void Bitmap::draw_faces(const std::span<Point>& points, 
                        const std::span<Face>& faces,
                        const Point& eye,
                        const Point& forward) 
{
    auto draw_line = [&](std::vector<uint32_t>& data, int x1, int y1, int x2, int y2) {
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;
        
        while (true) {
            if (x1 >= 0 && x1 < m_width && y1 >= 0 && y1 < m_height) {
                int index = y1 * m_width + x1;
                data[index] = WHITE;
            }
            else break;

            if (x1 == x2 && y1 == y2) break;
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x1 += sx; }
            if (e2 < dx) { err += dx; y1 += sy; }
        }
    };
    
    auto draw_partial = [&](auto it_begin, auto it_end){
        std::ranges::for_each(it_begin, it_end, [&](const Face& face){
            const size_t face_size = face.size();

            for (size_t i = 0; i < face_size; ++i) {
                const auto& point1 = points[face[i]];
                const auto& point2 = points[face[(i + 1) % face_size]];

                if (point1.z > 0 && point2.z > 0){
                    int screenX1 = static_cast<int>(point1.x);
                    int screenY1 = static_cast<int>(point1.y);
                    int screenX2 = static_cast<int>(point2.x);
                    int screenY2 = static_cast<int>(point2.y);
    
                    draw_line(m_data, screenX1, screenY1, screenX2, screenY2);
                }
            }
        });
    };

    clear();

    std::vector<std::future<std::any>> futures{};

    const int size = faces.size();
    int count_per_thread = size / THREADS_COUNT;
    
    for (int i = 0; i < THREADS_COUNT; ++i) {
        int current_size = (i < THREADS_COUNT - 1)
        ?   count_per_thread
        :   size - (THREADS_COUNT - 1) * count_per_thread;
        
        auto it_begin = std::next(faces.begin(), i * count_per_thread);
        auto it_end = std::next(it_begin, current_size);
        
        futures.emplace_back(thread_pool.add_task(
            draw_partial, it_begin, it_end
        ));
    }
    
    std::ranges::for_each(futures, [](auto& current_future){
        auto fut = current_future.get();
    });
}

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

std::vector<Point> mult(const TransformMatrix& matrix, const std::vector<Point>& points) {
    std::vector<Point> result{};
    std::ranges::transform(points, std::back_inserter(result), [&](const Point& point){
        return matrix * point;
    });
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
    angle = std::fmod(angle, TWO_PI);
    if (angle > PI) angle -= TWO_PI;
    if (angle < -PI) angle += TWO_PI;
    return angle;
}
