#pragma once
#include "Matrix.hpp"
#include "Color.hpp"
#include <vector>

struct Point {
    Vector4 world;
    Vector4 screen;
    Vector4 normal;
};

using Points = std::vector<Point>;