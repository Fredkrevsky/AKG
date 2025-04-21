#include "Matrix.hpp"
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>

static glm::mat4x4 create_rotation_matrix_x(float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return glm::mat4x4{
        {1, 0, 0, 0},
        {0, c, -s, 0},
        {0, s, c, 0},
        {0, 0, 0, 1}
    };
}

static glm::mat4x4 create_rotation_matrix_y(float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return glm::mat4x4{
        {c, 0, s, 0},
        {0, 1, 0, 0},
        {-s, 0, c, 0},
        {0, 0, 0, 1}
    };
}

static glm::mat4x4 create_rotation_matrix_z(float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return glm::mat4x4{
        {c, -s, 0, 0},
        {s, c, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };
}

glm::mat4x4 create_rotation_matrix(const glm::vec4& angles) {
    glm::mat4x4 result{
        {1, 0, 0, 0}, 
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    if (angles.x != 0) {
        result = result * create_rotation_matrix_x(angles.x);
    }
    if (angles.y != 0) {
        result = result * create_rotation_matrix_y(angles.y);
    }
    if (angles.z != 0) {
        result = result * create_rotation_matrix_z(angles.z);
    }

    return result;
}

glm::mat4x4 create_move_matrix(const glm::vec4& translation) {
    return glm::mat4x4{
        {1, 0, 0, translation.x},
        {0, 1, 0, translation.y},
        {0, 0, 1, translation.z},
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
