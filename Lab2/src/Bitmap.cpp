#include "Bitmap.hpp"
#include <algorithm>
#include <iostream>

Bitmap::Bitmap(int width, int height) noexcept 
    : m_data(width * height)
    , m_z_buffer(width * height)
    , m_width(width)
    , m_height(height)
    {}

void Bitmap::clear(){
    std::ranges::fill(m_data, 0);
}

const uint8_t* Bitmap::data() const {
    return reinterpret_cast<const uint8_t*>(m_data.data());
}

void Bitmap::draw_faces(const Vertices& points, const Faces& faces) {
    auto draw_triangle = [this](Point p1, Point p2, Point p3, uint32_t color) {
        if (p1.y == p2.y && p1.y == p3.y) return;

        if (p1.y > p2.y) std::swap(p1, p2);
        if (p1.y > p3.y) std::swap(p1, p3);
        if (p2.y > p3.y) std::swap(p2, p3);

        const int x1 = static_cast<int>(p1.x);
        const int y1 = static_cast<int>(p1.y);
        const int x2 = static_cast<int>(p2.x);
        const int y2 = static_cast<int>(p2.y);
        const int x3 = static_cast<int>(p3.x);
        const int y3 = static_cast<int>(p3.y);
        
        const double z1 = p1.z;
        const double z2 = p2.z;
        const double z3 = p3.z;
        
        const int total_height = y3 - y1;
        if (total_height == 0) return;

        int min_i = std::max(-y1, 0);
        int max_i = std::min(y3, m_height) - y1;
        for (int i = min_i; i < max_i; ++i) {
            const bool second_half = i > (y2 - y1) || y2 == y1;
            const int segment_height = second_half ? (y3 - y2) : (y2 - y1);
            if (segment_height == 0) continue;

            const double alpha = static_cast<double>(i) / total_height;
            const double beta = static_cast<double>(second_half ? (i - (y2 - y1)) : i) / segment_height;

            int Ax = x1 + (x3 - x1) * alpha;
            int Ay = y1 + i;
            double Az = z1 + (z3 - z1) * alpha;

            int Bx{}, By{};
            double Bz{};
            if (second_half) {
                Bx = x2 + (x3 - x2) * beta;
                By = y2 + (i - (y2 - y1));
                Bz = z2 + (z3 - z2) * beta;
            } else {
                Bx = x1 + (x2 - x1) * beta;
                By = y1 + i;
                Bz = z1 + (z2 - z1) * beta;
            }

            if (Ax > Bx) {
                std::swap(Ax, Bx);
                std::swap(Ay, By);
                std::swap(Az, Bz);
            }

            int min_x = std::max(Ax, 0);
            int max_x = std::min(Bx, m_width);
            int index = Ay * m_width;

            for (int x = min_x; x < max_x; ++x) {
                double t = (x - Ax) / static_cast<double>(Bx - Ax);
                double z = Az + (Bz - Az) * t;
                if (z < m_z_buffer[index + x]) {
                    m_data[index + x] = color; 
                    m_z_buffer[index + x] = z;
                }
            }
        }
    };

    clear();
    std::ranges::fill(m_z_buffer, 1.0);

    std::ranges::for_each(faces, [&](const Face& face) {
        const auto& point1 = points[face[0]];
        const auto& point2 = points[face[1]];
        const auto& point3 = points[face[2]];
        const uint32_t color = face[3];

        if (point1.w > 0 && point2.w > 0 && point3.w > 0) {
            draw_triangle(point1, point2, point3, color);
        }
    });
}