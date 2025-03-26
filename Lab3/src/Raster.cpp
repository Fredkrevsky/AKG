#include "Raster.hpp"

void Raster::set_eye(const Vector4& eye){
    m_eye = eye;
}

void Raster::set_sun(const Vector4& sun){
    m_sun = sun;
}

Color::RGBA Raster::get_color(const Point& p) {
    //const auto& [vertex, normal] = p;

    Color::RGBA Ia = Color::multiply(is, ka); 

    return Ia;
}