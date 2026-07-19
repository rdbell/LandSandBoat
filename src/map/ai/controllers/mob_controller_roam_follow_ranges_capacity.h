#pragma once

#include <cstdint>

namespace mobcontrollerroamfollowranges
{
struct Ranges
{
    float leash;
    float stop;
};

// Resolve returns the default roam-follow ranges overridden by positive mob modifiers.
constexpr auto Resolve(const int32_t leashModifier, const int32_t stopModifier) -> Ranges
{
    return {
        leashModifier > 0 ? static_cast<float>(leashModifier) : 4.0f,
        stopModifier > 0 ? static_cast<float>(stopModifier) : 2.0f,
    };
}
} // namespace mobcontrollerroamfollowranges
