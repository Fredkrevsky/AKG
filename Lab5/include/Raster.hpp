#pragma once
#include "Matrix.hpp"
#include "Color.hpp"
#include <string>

class Raster final{
public:
    struct PointData {
        const Vertex& world;
        const Vertex& normal;
        const TextureVertex& texture;
    };

    Raster() noexcept;
    ~Raster() = default;

    void set_eye(const glm::vec3& eye);
    void set_sun(const glm::vec3& sun);
    
    Color::RGBA get_color(const PointData& p);

private:
    std::vector<std::vector<uint32_t>> arr_diffuse;
    std::vector<std::vector<uint32_t>> arr_normal;
    std::vector<std::vector<uint32_t>> arr_specular;

    glm::vec3 m_eye, m_sun;
};

