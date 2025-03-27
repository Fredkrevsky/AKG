#include "Raster.hpp"


void Raster::set_eye(const Vector4& eye){
    m_eye = eye;
}

void Raster::set_sun(const Vector4& sun){
    m_sun = sun;
}

Color::RGBA Raster::get_color(const Point& point) {
    const auto& [world, screen, N] = point;
    const Vector4 L = (m_sun - world).normalize();
    
    Color::RGBA Ia = Color::multiply(is, ka); 
    auto val = N.dot(L);
    double coef = kd * std::clamp(val, 0.0, 1.0);

    Color::RGBA Id = Color::multiply(id, coef);

    return Color::add(Ia, Id);
}