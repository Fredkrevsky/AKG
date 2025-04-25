#pragma once
#include <array>
#include <vector>
#include <cmath>
#include <cstdint>
#include <numbers>
#include <algorithm>
#include <glm/mat4x4.hpp>

using Face = std::array<std::array<uint32_t, 3>, 3>;
using Faces = std::vector<Face>;
using Vertices = std::vector<glm::vec3>;

constexpr static float PI = std::numbers::pi_v<float>;
constexpr static float TWO_PI = 2.0 * PI;

glm::mat4x4 create_rotation_matrix(const glm::vec4& rotate_vector);
glm::mat4x4 create_move_matrix(const glm::vec4& move_vector);
glm::mat4x4 create_scale_matrix(float scalar);
