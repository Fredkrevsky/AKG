#pragma once
#include "Matrix.hpp"

class Bitmap final {
public:
    Bitmap(int width, int height) noexcept;
    
    const uint8_t* data() const;
    void clear();
    void draw_faces(const Vertices& points, 
                    const Faces& faces);

private:
    void draw_triangle(Point p1, Point p2, Point p3, uint32_t color);

private:
    std::vector<uint32_t> m_data; 
    std::vector<double> m_z_buffer;
    int m_width;
    int m_height;
};