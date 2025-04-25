#include "Raster.hpp"
#include <cmath>
#include <fstream>
#include <iostream>

namespace {

constexpr static int WIDTH = 2048;
constexpr static int HEIGHT = 2048;

std::vector<std::vector<uint32_t>> load_texture(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open texture file");
    }

    std::vector<std::vector<uint32_t>> texture(HEIGHT, std::vector<uint32_t>(WIDTH));
    std::vector<uint8_t> row_buffer(WIDTH * 3);

    for (int y = 0; y < HEIGHT; ++y) {
        file.read(reinterpret_cast<char*>(row_buffer.data()), WIDTH * 3);
        
        if (file.gcount() != WIDTH * 3) {
            throw std::runtime_error("Unexpected end of file");
        }

        for (int x = 0; x < WIDTH; ++x) {
            uint8_t r = row_buffer[x * 3];
            uint8_t g = row_buffer[x * 3 + 1];
            uint8_t b = row_buffer[x * 3 + 2];
            texture[y][x] = (0xFF << 24) | (b << 16) | (g << 8) | r;
        }
    }

    return texture;
}

Color::RGBA getPixel(const std::vector<std::vector<uint32_t>>& data, float x, float y){
    if (x < 0 || x > 1 || y < 0 || y > 1){
        return Color::RGBA{0xFF0000FF};
    }

    int row = static_cast<int>((1 - y) * (HEIGHT - 1));
    int col = static_cast<int>(x * (WIDTH - 1));
    return static_cast<Color::RGBA>(data[row][col]);
}

}

Raster::Raster() noexcept {
    arr_diffuse = load_texture(diffuse_path);
    //arr_normal = load_texture(normal_path);
}

void Raster::set_eye(const glm::vec3& eye){
    m_eye = eye;
}

void Raster::set_sun(const glm::vec3& sun){
    m_sun = sun;
}

Color::RGBA Raster::get_color(const Point& point) {
    const auto& [world, screen, normal, texture] = point;
    
    glm::vec3 N = glm::normalize(normal);
    glm::vec3 L = glm::normalize(m_sun - world);
    glm::vec3 V = glm::normalize(m_eye - world);
    
    //Color::RGBA AColor = static_cast<Color::RGBA>(arr_diffuse, texture.x, texture.y); 
    Color::RGBA Ia = Color::multiply(ia, ka);
    
    const float NL = glm::dot(N, L);
    if (NL <= 0.0f){
        return Ia;
    }
    float diffuse_coef = kd * NL;

    Color::RGBA DColor = getPixel(arr_diffuse, texture.x, texture.y); 
    Color::RGBA Id = Color::multiply(DColor, diffuse_coef);
    
    glm::vec3 H = glm::normalize(L + V);
    glm::vec3 R = glm::normalize((N * (2.0f * NL)) - L);
    float specular_coef = ks * std::pow(std::max(0.0f, glm::dot(N, H)), a);
    Color::RGBA Is = Color::multiply(is, specular_coef);
    
    return Id;//Color::add(Ia, Id, Is);
}