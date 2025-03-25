#include "Bitmap.hpp"
#include <algorithm>

Bitmap::Bitmap(int width, int height) noexcept 
    : m_data(width * height)
    , m_z_buffer(width * height)
    , m_width(width)
    , m_height(height)
    {}

void Bitmap::clear(){
    std::ranges::fill(m_data, 0);
    std::ranges::fill(m_z_buffer, 1.0);
}

const uint8_t* Bitmap::data() const {
    return reinterpret_cast<const uint8_t*>(m_data.data());
}

void Bitmap::draw_triangle(Vector4D v1, Vector4D v2, Vector4D v3) {

    uint32_t c1 = v1.color;
    uint32_t c2 = v2.color;
    uint32_t c3 = v3.color;

    int min_x = std::max(0, static_cast<int>(std::min({v1.x, v2.x, v3.x})));
    int max_x = std::min(m_width - 1, static_cast<int>(std::max({v1.x, v2.x, v3.x})));
    int min_y = std::max(0, static_cast<int>(std::min({v1.y, v2.y, v3.y})));
    int max_y = std::min(m_height - 1, static_cast<int>(std::max({v1.y, v2.y, v3.y})));

    double denominator = (v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y);
    if (std::abs(denominator) < 1e-6) return;

    for (int y = min_y; y <= max_y; ++y) {
        for (int x = min_x; x <= max_x; ++x) {
            double u = ((v2.y - v3.y) * (x - v3.x) + (v3.x - v2.x) * (y - v3.y)) / denominator;
            double v = ((v3.y - v1.y) * (x - v3.x) + (v1.x - v3.x) * (y - v3.y)) / denominator;
            double w = 1.0 - u - v;

            if (u < 0 || v < 0 || w < 0) continue;

            double z = u * v1.z + v * v2.z + w * v3.z;
            int index = y * m_width + x;

            if (z < m_z_buffer[index]){
                uint32_t grey = static_cast<double>(c1 & 255) * u + 
                                static_cast<double>(c2 & 255) * v + 
                                static_cast<double>(c3 & 255) * w;
                uint32_t color = (255 << 24) | (grey << 16) | (grey << 8) | grey;
                m_z_buffer[index] = z;
                m_data[index] = color;
            }
        }
    }
}


void Bitmap::draw_faces(const Vertices& points, const Faces& faces) {
    
    clear();

    std::ranges::for_each(faces, [&](const Face& face) {
        const auto& p1 = points[face[0]];
        const auto& p2 = points[face[1]];
        const auto& p3 = points[face[2]];

        if (p1.w > 0 && p2.w > 0 && p3.w > 0 &&
            p1.color != Color::NO_COLOR && 
            p2.color != Color::NO_COLOR && 
            p3.color != Color::NO_COLOR) 
        {
            draw_triangle(p1, p2, p3);
        }
    });
}