#pragma once
#include "Matrix.hpp"
#include "Color.hpp"
#include <vector>
#include <glm/vec4.hpp>

struct Point {
    glm::vec3 world; 
    glm::vec4 screen;
    glm::vec3 normal;
    glm::vec3 texture;
};

using Points = std::vector<Point>;