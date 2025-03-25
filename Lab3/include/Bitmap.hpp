#pragma once
#include "Matrix.hpp"
#include "Point.hpp"

class Bitmap final {
public:
    Bitmap(int width, int height) noexcept;
    
    const uint8_t* data() const;
    void clear();
    void draw(const Points& points,
              const Faces& faces);

private:
    void draw_triangle(Point p1, Point p2, Point p3);

private:
    std::vector<uint32_t> m_data; 
    std::vector<double> m_z_buffer;
    int m_width;
    int m_height;
};