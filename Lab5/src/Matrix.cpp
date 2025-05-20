#include "Matrix.hpp"
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>

namespace {

glm::mat4x4 create_rotation_matrix_x(float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return glm::mat4x4{
        {1, 0, 0, 0},
        {0, c, -s, 0},
        {0, s, c, 0},
        {0, 0, 0, 1}
    };
}

glm::mat4x4 create_rotation_matrix_y(float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return glm::mat4x4{
        {c, 0, s, 0},
        {0, 1, 0, 0},
        {-s, 0, c, 0},
        {0, 0, 0, 1}
    };
}

glm::mat4x4 create_rotation_matrix_z(float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return glm::mat4x4{
        {c, -s, 0, 0},
        {s, c, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };
}

}

glm::mat4x4 create_rotation_matrix(const glm::vec3& rotation_vector) {
    glm::mat4x4 result{
        {1, 0, 0, 0}, 
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    constexpr float epsilon = 0.0001;

    if (std::abs(rotation_vector.x) > epsilon) {
        result = result * create_rotation_matrix_x(rotation_vector.x);
    }
    if (std::abs(rotation_vector.y) > epsilon) {
        result = result * create_rotation_matrix_y(rotation_vector.y);
    }
    if (std::abs(rotation_vector.z) > epsilon) {
        result = result * create_rotation_matrix_z(rotation_vector.z);
    }

    return result;
}

glm::mat4x4 create_move_matrix(const glm::vec3& move_vector) {
    return glm::mat4x4{
        {1, 0, 0, move_vector.x},
        {0, 1, 0, move_vector.y},
        {0, 0, 1, move_vector.z},
        {0, 0, 0, 1}
    };
}

glm::mat4x4 create_scale_matrix(float scalar){
    return glm::mat4x4{
        {scalar, 0, 0, 0},
        {0, scalar, 0, 0},
        {0, 0, scalar, 0},
        {0, 0, 0, 1}
    };
}
