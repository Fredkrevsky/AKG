#pragma once
#include <cstdint>
#include <algorithm>
#include <cmath>
#include <concepts>

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
    RGBA from_intensity(double intensity);
    RGBA merge(RGBA c1, RGBA c2);
    
    template<typename ...Args>
    requires (std::same_as<Args, RGBA> && ...)
    RGBA add(RGBA c1, RGBA c2, Args ...ts){
        if constexpr (sizeof...(ts) == 0){
            return merge(c1, c2);
        } else {
            return add(merge(c1, c2), ts...);
        }
    }

    RGBA multiply(RGBA color, double factor);
}