#pragma once

#include <chrono>
#include <cstdint>

namespace mobcontrolleridledespawn
{
struct Result
{
    bool                 shouldSet;
    std::chrono::seconds duration;
};

// Resolve returns the idle-despawn timer update requested after disengagement.
constexpr auto Resolve(const int16_t idleDespawnSeconds) -> Result
{
    return { idleDespawnSeconds != 0, std::chrono::seconds(idleDespawnSeconds) };
}
} // namespace mobcontrolleridledespawn
