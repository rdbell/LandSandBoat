#pragma once

#include <cstdint>

namespace mobcontrollercaststopcooldown
{
struct Range
{
    int32_t lower;
    int32_t upper;
};

// Resolve returns the random cooldown window after a mob stops casting.
constexpr auto Resolve(const int32_t magicCooldown) -> Range
{
    return { magicCooldown / 2, magicCooldown };
}
} // namespace mobcontrollercaststopcooldown
