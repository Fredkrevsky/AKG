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
    void draw_triangle(Vector4D p1, Vector4D p2, Vector4D p3);

private:
    std::vector<uint32_t> m_data; 
    std::vector<double> m_z_buffer;
    int m_width;
    int m_height;
};