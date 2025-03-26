#pragma once
#include "Matrix.hpp"
#include "Color.hpp"
#include <vector>

struct Point {
    Vector4 vertex;
    Vector4 normal;
};

using Points = std::vector<Point>;