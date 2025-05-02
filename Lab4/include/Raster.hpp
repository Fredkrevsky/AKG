#pragma once
#include "Matrix.hpp"
#include "Color.hpp"
#include "Point.hpp"
#include <string>

class Raster final{
public:
    Raster() noexcept;
    ~Raster() = default;

    void set_eye(const glm::vec3& eye);
    void set_sun(const glm::vec3& sun);
    
    Color::RGBA get_color(const Point& p);

private:
    constexpr static float a = 16.0;
    constexpr static float ka = 0.1;
    constexpr static float kd = 0.5;
    constexpr static float ks = 0.3;
    
    constexpr static Color::RGBA ia = Color::Basic::White;
    constexpr static Color::RGBA id = Color::Basic::White;
    constexpr static Color::RGBA is = Color::Basic::White;

    const std::string diffuse_path = "../models/diffuse.raw";
    const std::string normal_path = "../models/normal.raw";
    const std::string specular_path = "../models/specular.raw";

    std::vector<std::vector<uint32_t>> arr_diffuse;
    std::vector<std::vector<uint32_t>> arr_normal;
    std::vector<std::vector<uint32_t>> arr_specular;

    glm::vec3 m_eye, m_sun;
};

