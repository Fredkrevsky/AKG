#include "Raster.hpp"
#include <cmath>


void Raster::set_eye(const glm::vec3& eye){
    m_eye = eye;
}

void Raster::set_sun(const glm::vec3& sun){
    m_sun = sun;
}

Color::RGBA Raster::get_color(const Point& point) {
    const auto& [world, screen, normal] = point;
    
    glm::vec3 N = glm::normalize(normal);
    glm::vec3 L = glm::normalize(m_sun - world);
    glm::vec3 V = glm::normalize(m_eye - world);
    
    Color::RGBA Ia = Color::multiply(ia, ka);
    
    const float NL = glm::dot(N, L);
    if (NL <= 0.0f){
        return Ia;
    }
    float diffuse_coef = kd * NL;
    Color::RGBA Id = Color::multiply(id, diffuse_coef);
    
    glm::vec3 H = glm::normalize(L + V);
    glm::vec3 R = glm::normalize((N * (2.0f * NL)) - L);
    float specular_coef = ks * std::pow(std::max(0.0f, glm::dot(N, H)), a);
    Color::RGBA Is = Color::multiply(is, specular_coef);
    
    return Color::add(Ia, Id, Is);
}