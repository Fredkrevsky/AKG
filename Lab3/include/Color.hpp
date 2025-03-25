#pragma once
#include <cstdint>

namespace Color {

    using RGBA = uint32_t; 

    namespace Basic {
        constexpr RGBA Black       = 0xFF000000;
        constexpr RGBA Red         = 0xFF0000FF;
        constexpr RGBA Green       = 0xFF00FF00;
        constexpr RGBA Blue        = 0xFFFF0000;
        constexpr RGBA White       = 0xFFFFFFFF;
        constexpr RGBA Transparent = 0x00000000;
    }

    RGBA from_gray(uint8_t gray);
    uint8_t to_gray(RGBA color);
    RGBA from_intensity(double intensity) ;
}