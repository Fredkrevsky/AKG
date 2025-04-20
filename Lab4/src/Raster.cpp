#include "Raster.hpp"
#include <cmath>


void Raster::set_eye(const Vector4& eye){
    m_eye = eye;
}

void Raster::set_sun(const Vector4& sun){
    m_sun = sun;
}

Color::RGBA Raster::get_color(const Point& point) {
    const auto& [world, screen, normal] = point;
    
    Vector4 N = normal;
    N.normalize();
    Vector4 L = (m_sun - world).normalize();
    Vector4 V = (m_eye - world).normalize();
    
    Color::RGBA Ia = Color::multiply(ia, ka);
    
    const float NL = N.dot(L);
    if (NL <= 0.0){
        return Ia;
    }
    float diffuse_coef = kd * NL;
    Color::RGBA Id = Color::multiply(id, diffuse_coef);
    
    Vector4 H = (L + V).normalize();
    Vector4 R = ((N * (2.0 * NL)) - L).normalize();
    float specular_coef = ks * std::pow(std::max(0.0f, N.dot(H)), a);
    Color::RGBA Is = Color::multiply(is, specular_coef);
    
    return Color::add(Ia, Id, Is);
}