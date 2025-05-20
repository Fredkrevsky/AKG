#pragma once
#include <array>
#include <vector>
#include <cmath>
#include <cstdint>
#include <numbers>
#include <algorithm>
#include <glm/mat4x4.hpp>

using Face = std::array<std::array<uint32_t, 3>, 3>;
using Vertex = glm::vec3;
using ScreenVertex = glm::vec4;
using TextureVertex = glm::vec2;

using Faces = std::vector<Face>;
using Vertices = std::vector<Vertex>;
using ScreenVertices = std::vector<ScreenVertex>;
using TextureVertices = std::vector<TextureVertex>;

constexpr float PI = std::numbers::pi_v<float>;
constexpr float TWO_PI = 2.f * PI;

glm::mat4 create_rotation_matrix(const glm::vec3& rotate_vector);
glm::mat4 create_move_matrix(const glm::vec3& move_vector);
glm::mat4 create_scale_matrix(float scalar);
