#pragma once

namespace mobcontrollerpetassist
{
// ShouldAssist reports whether a mob's roaming pet joins its current combat.
constexpr auto ShouldAssist(const bool hasPet, const bool petIsRoaming) -> bool
{
    return hasPet && petIsRoaming;
}
} // namespace mobcontrollerpetassist
