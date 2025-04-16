#include "Color.hpp"

using namespace Color;

RGBA Color::from_gray(uint8_t gray) {
    return 0xFF000000 | (gray << 16) | (gray << 8) | gray;
}

uint8_t Color::to_gray(RGBA color) {
    uint16_t r = (color >> 16) & 0xFF;
    uint16_t g = (color >> 8) & 0xFF;
    uint16_t b = color & 0xFF;
    return static_cast<uint8_t>((r + g + b) / 3);
}

RGBA Color::from_intensity(double intensity) {
    if (intensity <= 0.0) return Basic::Black;
    if (intensity >= 1.0) return Basic::White;
    
    uint8_t gray = static_cast<uint8_t>(std::clamp(std::lround(intensity * 255), 0L, 255L));
    return Color::from_gray(gray);
}

RGBA Color::merge(RGBA c1, RGBA c2) {
    uint8_t r = std::min(((c1 >> 16) & 0xFF) + ((c2 >> 16) & 0xFF), 255U);
    uint8_t g = std::min(((c1 >> 8) & 0xFF) + ((c2 >> 8) & 0xFF), 255U);
    uint8_t b = std::min((c1 & 0xFF) + (c2 & 0xFF), 255U);
    return 0xFF000000 | (r << 16) | (g << 8) | b;
}

RGBA Color::multiply(RGBA color, double factor) {
    factor = std::clamp(factor, 0.0, 1.0);
    uint8_t r = static_cast<uint8_t>(std::lround(((color >> 16) & 0xFF) * factor));
    uint8_t g = static_cast<uint8_t>(std::lround(((color >> 8) & 0xFF) * factor));
    uint8_t b = static_cast<uint8_t>(std::lround((color & 0xFF) * factor));
    return 0xFF000000 | (r << 16) | (g << 8) | b;
}
