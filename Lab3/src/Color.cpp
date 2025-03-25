#include "Color.hpp"
#include <algorithm>
#include <cmath>

using namespace Color;

RGBA Color::from_gray(uint8_t gray) {
    return 0xFF000000 | (gray << 16) | (gray << 8) | gray;
}

uint8_t Color::to_gray(RGBA color) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    return (r + g + b) / 3;
}

RGBA Color::from_intensity(double intensity) {
    if (intensity <= 0.0) return Basic::Black;
    if (intensity >= 1.0) return Basic::White;
    
    uint8_t gray = static_cast<uint8_t>(std::clamp(std::lround(intensity * 255), 0L, 255L));
    return Color::from_gray(gray);
}
