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
    std::vector<uint32_t> m_data; 
    int m_width;
    int m_height;
};