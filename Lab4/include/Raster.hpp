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
    constexpr static double a = 16.0;
    constexpr static double ka = 0.1;
    constexpr static double kd = 0.0;
    constexpr static double ks = 1.0;
    
    constexpr static Color::RGBA ia = Color::Basic::White;
    constexpr static Color::RGBA id = Color::Basic::Red;
    constexpr static Color::RGBA is = Color::Basic::Blue;

    Vector4 m_eye, m_sun;
};