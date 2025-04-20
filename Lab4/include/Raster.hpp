#pragma once
#include "Matrix.hpp"
#include "Color.hpp"
#include "Point.hpp"

class Raster final{
public:
    Raster() noexcept = default;
    ~Raster() = default;

    void set_eye(const Vector4& eye);
    void set_sun(const Vector4& sun);
    
    Color::RGBA get_color(const Point& p);

private:
    constexpr static float a = 16.0;
    constexpr static float ka = 0.1;
    constexpr static float kd = 0.5;
    constexpr static float ks = 0.3;
    
    constexpr static Color::RGBA ia = Color::Basic::White;
    constexpr static Color::RGBA id = Color::Basic::Red;
    constexpr static Color::RGBA is = Color::Basic::Blue;

    Vector4 m_eye, m_sun;
};