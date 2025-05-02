#include "Raster.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <iostream>

namespace {

constexpr static int WIDTH = 2048;
constexpr static int HEIGHT = 2048;

std::vector<std::vector<uint32_t>> load_texture(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open texture file: " + filename);
    }

    std::vector<std::vector<uint32_t>> texture(HEIGHT, std::vector<uint32_t>(WIDTH));
    std::vector<uint8_t> row_buffer(WIDTH * 3);

    for (int y = 0; y < HEIGHT; ++y) {
        file.read(reinterpret_cast<char*>(row_buffer.data()), WIDTH * 3);
        
        if (file.gcount() != WIDTH * 3) {
            throw std::runtime_error("Unexpected end of file in texture: " + filename);
        }

        for (int x = 0; x < WIDTH; ++x) {
            uint8_t r = row_buffer[x*3];
            uint8_t g = row_buffer[x*3 + 1];
            uint8_t b = row_buffer[x*3 + 2];
            texture[y][x] = (0xFF << 24) | (b << 16) | (g << 8) | r;
        }
    }

    return texture;
}

glm::vec3 normal_from_color(uint32_t color) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    
    return glm::normalize(glm::vec3{
        r / 255.0f * 2.0f - 1.0f,
        g / 255.0f * 2.0f - 1.0f,
        b / 255.0f * 2.0f - 1.0f
    });
}

Color::RGBA getPixel(const std::vector<std::vector<uint32_t>>& data, float u, float v) {
    u = std::clamp(u, 0.0f, 1.0f);
    v = 1.0f - std::clamp(v, 0.0f, 1.0f);
    
    int x = std::clamp((int)(u * WIDTH), 0, WIDTH - 1);
    int y = std::clamp((int)(v * HEIGHT), 0, HEIGHT - 1);
    
    return static_cast<Color::RGBA>(data[y][x]);
}

}

Raster::Raster() noexcept {
    try {
        arr_diffuse = load_texture(diffuse_path);
        arr_normal = load_texture(normal_path);
        arr_specular = load_texture(specular_path);
        //arr_normal = std::vector<std::vector<uint32_t>>(HEIGHT, std::vector<uint32_t>(WIDTH, 0));
        //arr_specular = std::vector<std::vector<uint32_t>>(HEIGHT, std::vector<uint32_t>(WIDTH, 0));
    } catch (const std::exception& e) {
        std::cerr << "Texture loading error: " << e.what() << std::endl;
        arr_diffuse = std::vector<std::vector<uint32_t>>(HEIGHT, std::vector<uint32_t>(WIDTH, 0));
        arr_normal = std::vector<std::vector<uint32_t>>(HEIGHT, std::vector<uint32_t>(WIDTH, 0));
        arr_specular = std::vector<std::vector<uint32_t>>(HEIGHT, std::vector<uint32_t>(WIDTH, 0));
    }
}

void Raster::set_eye(const glm::vec3& eye) {
    m_eye = eye;
}

void Raster::set_sun(const glm::vec3& sun) {
    m_sun = sun;
}

Color::RGBA Raster::get_color(const Point& point) {
    const auto& [world, screen, normal, texture] = point;
    
    Color::RGBA DColor = getPixel(arr_diffuse, texture.x, texture.y); 
    
    glm::vec3 tex_normal = normal_from_color(getPixel(arr_normal, texture.x, texture.y));
    //glm::vec3 tex_normal = normal;
    
    glm::vec3 N = glm::normalize(normal + tex_normal);
    glm::vec3 L = glm::normalize(m_sun - world);
    glm::vec3 V = glm::normalize(m_eye - world);
    
    Color::RGBA Ia = Color::multiply(DColor, ka);
    
    const float NL = glm::dot(N, L);
    if (NL <= 0.0f) {
        return Ia;
    }
    
    float diffuse_coef = kd * NL * 2;
    
    Color::RGBA Id = Color::multiply(DColor, diffuse_coef);
    
    glm::vec3 H = glm::normalize(L + V);
    
    uint32_t spec_value = getPixel(arr_specular, texture.x, texture.y);
    float specular_intensity = (spec_value & 0xFF) / 255.0f; 

    float specular_coef = ks * specular_intensity * std::pow(std::max(0.0f, glm::dot(N, H)), a);
    Color::RGBA Is = Color::multiply(is, specular_coef);
    
    return Id;
}