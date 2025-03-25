#pragma once
#include "Matrix.hpp"
#include "Color.hpp"
#include <vector>

struct Point {
    Vector4D vertex;
    Vector4D normal;
    Color::RGBA color;
};

using Points = std::vector<Point>;