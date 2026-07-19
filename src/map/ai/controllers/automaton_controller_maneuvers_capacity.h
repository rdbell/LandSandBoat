#pragma once

#include <cstdint>

namespace automatoncontrollermaneuvers
{
struct Counts
{
    uint8_t fire;
    uint8_t ice;
    uint8_t wind;
    uint8_t earth;
    uint8_t thunder;
    uint8_t water;
    uint8_t light;
    uint8_t dark;
};

inline auto Current(Counts counts) -> Counts
{
    return counts;
}
} // namespace automatoncontrollermaneuvers
