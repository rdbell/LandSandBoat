#pragma once

#include <algorithm>
#include <cstdint>

namespace automatoncontrollerhealingthreshold
{
inline auto HealingThreshold(uint8_t light, int16_t modifier) -> float
{
    float threshold = 30.0f;
    switch (light)
    {
        case 1: threshold = 40.0f; break;
        case 2: threshold = 50.0f; break;
        case 3: threshold = 75.0f; break;
    }
    return std::clamp(threshold + modifier, 30.0f, 90.0f);
}
} // namespace automatoncontrollerhealingthreshold
