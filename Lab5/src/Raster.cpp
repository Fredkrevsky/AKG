#include "Raster.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <iostream>

namespace {

constexpr int NUMBER_TEXTURES = 4;
constexpr int TEXTURE_WIDTH = 2048;
constexpr int TEXTURE_HEIGHT = 2048;

constexpr float a = 250.0;
constexpr float ka = 0.1;
constexpr float kd = 0.5;
constexpr float ks = 0.3;

std::vector<std::vector<uint32_t>> load_texture(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open texture file: " + filename);
    }

    std::vector<std::vector<uint32_t>> texture(TEXTURE_HEIGHT, std::vector<uint32_t>(TEXTURE_WIDTH));
    std::vector<uint8_t> row_buffer(TEXTURE_WIDTH * 3);

    for (int y = 0; y < TEXTURE_HEIGHT; ++y) {
        file.read(reinterpret_cast<char*>(row_buffer.data()), TEXTURE_WIDTH * 3);
        
        if (file.gcount() != TEXTURE_WIDTH * 3) {
            throw std::runtime_error("Unexpected end of file in texture: " + filename);
        }

        for (int x = 0; x < TEXTURE_WIDTH; ++x) {
            uint8_t r = row_buffer[x * 3];
            uint8_t g = row_buffer[x * 3 + 1];
            uint8_t b = row_buffer[x * 3 + 2];
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
    
    int x = std::clamp((int)(u * TEXTURE_WIDTH), 0, TEXTURE_WIDTH - 1);
    int y = std::clamp((int)(v * TEXTURE_HEIGHT), 0, TEXTURE_HEIGHT - 1);
    
    return static_cast<Color::RGBA>(data[y][x]);
}

} // namespace

Raster::Raster() noexcept {
    std::vector<std::string> diffuse = {
        "../model/Knight/Textures/hellknight_body.raw",
        "../model/Knight/Textures/hellknight_head.raw",
        "../model/Knight/Textures/hellknight_legs.raw",
        "../model/Knight/Textures/hellknight_eyes.raw",
    };
    std::vector<std::string> normal = {
        "../model/Knight/Textures/hellknight_body_n.raw",
        "../model/Knight/Textures/hellknight_head_n.raw",
        "../model/Knight/Textures/hellknight_legs_n.raw",
        "../model/Knight/Textures/hellknight_eyes_n.raw",
    };
    std::vector<std::string> specular = {
        "../model/Knight/Textures/hellknight_body_s.raw",
        "../model/Knight/Textures/hellknight_head_s.raw",
        "../model/Knight/Textures/hellknight_legs_s.raw",
        "../model/Knight/Textures/hellknight_eyes_s.raw",
    };

    try {
        for (int i = 0; i < NUMBER_TEXTURES; ++i){
            arr_diffuse.push_back(load_texture(diffuse[i]));
            arr_normal.push_back(load_texture(normal[i]));
            arr_specular.push_back(load_texture(specular[i]));
        }
    } catch (const std::exception& e) {
        std::cerr << "Texture loading error: " << e.what() << std::endl;
        for (int i = 0; i < NUMBER_TEXTURES; ++i){
            arr_diffuse.push_back(std::vector<std::vector<uint32_t>>(TEXTURE_HEIGHT, std::vector<uint32_t>(TEXTURE_WIDTH, 0)));
            arr_normal.push_back(std::vector<std::vector<uint32_t>>(TEXTURE_HEIGHT, std::vector<uint32_t>(TEXTURE_WIDTH, 0)));
            arr_specular.push_back(std::vector<std::vector<uint32_t>>(TEXTURE_HEIGHT, std::vector<uint32_t>(TEXTURE_WIDTH, 0)));
        }
    }
}

void Raster::reset_texture(){
    index = 0;
}

void Raster::next_texture() {
    index = (index + 1) % NUMBER_TEXTURES;
}

void Raster::set_eye(const glm::vec3& eye) {
    m_eye = eye;
}

void Raster::set_sun(const glm::vec3& sun) {
    m_sun = sun;
}

Color::RGBA Raster::get_color(const PointData& point) {
    const auto& [world, normal, texture] = point;
    
    Color::RGBA DColor = getPixel(arr_diffuse[index], texture.x, texture.y); 
    glm::vec3 tex_normal = normal;
    
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
    uint32_t spec_value = getPixel(arr_specular[index], texture.x, texture.y);
    float specular_coef = ks * std::pow(std::max(0.0f, glm::dot(N, H)), a);
    Color::RGBA Is = Color::multiply(spec_value, specular_coef);
    
    return Id;
}