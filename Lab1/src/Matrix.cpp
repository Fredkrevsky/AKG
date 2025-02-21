#include "Matrix.hpp"
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>
#include <ranges>
#include <thread>
#include <chrono>
#include <future>

using namespace std::chrono_literals;

Bitmap::Bitmap(int width, int height) 
    : m_thread_pool(THREADS_COUNT)
    , m_data(width * height, 0)
    , m_width(width)
    , m_height(height)
    {}

void Bitmap::clear(){
    std::ranges::fill(m_data, 0);
}

const uint8_t* Bitmap::data() const {
    return reinterpret_cast<const uint8_t*>(m_data.data());
}

void Bitmap::draw_faces(const std::span<Point>& points, const std::span<Face>& faces, int threads_count=1) {
    
    auto draw_line = [&](std::vector<uint32_t>& data, int x1, int y1, int x2, int y2) {

        const int size = data.size();
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;
    
        while (true) {
            int index = y1 * m_width + x1;
            if (index >= 0 && index < size) {
                data[index] = WHITE;
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
    };
    
    auto draw_face = [&](std::vector<uint32_t>& data, const Face& face) {
        const size_t face_size = face.size();
    
        for (size_t i = 0; i < face_size; ++i) {
            const auto& point1 = points[face[i]];
            const auto& point2 = points[face[(i + 1) % face_size]];
    
            auto [x1, y1, z1, w1] = point1;
            auto [x2, y2, z2, w2] = point2;
            draw_line(data, x1, y1, x2, y2);
        }
    };
    
    auto draw_partial = [&](auto it_begin, auto it_end){
        std::ranges::for_each(it_begin, it_end, [&](const Face& face){
            draw_face(m_data, face);
        });
    };

    clear();

    std::vector<std::future<std::any>> futures{};

    const int size = faces.size();
    int count_per_thread = size / threads_count;

    
    for (int i = 0; i < threads_count; ++i){
        int current_size = (i < threads_count - 1)
        ?   count_per_thread
        :   size - (threads_count - 1) * count_per_thread;
        
        auto it_begin = std::next(faces.begin(), i * count_per_thread);
        auto it_end = std::next(it_begin, current_size);
        
        futures.emplace_back(m_thread_pool.add_task(
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
