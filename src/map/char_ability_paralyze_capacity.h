#pragma once

#include <cstdint>
#include <functional>

// Pure paralysis gate from CCharEntity::OnAbility. Host injects paralysis
// presence, recast application for non-SP abilities, and interrupt packet.

namespace charabilityparalyzehelpers
{

// Recast::Special / Special2 pins (enums/recast.h).
constexpr std::uint16_t RecastSpecial  = 0;
constexpr std::uint16_t RecastSpecial2 = 254;

// ShouldConsumeRecastOnParalyze reports whether SP recast IDs are exempt.
constexpr auto ShouldConsumeRecastOnParalyze(const std::uint16_t recastID) -> bool
{
    return recastID != RecastSpecial && recastID != RecastSpecial2;
}

// Apply returns true when the ability was paralyzed and the host must stop.
template <typename ApplyRecast, typename Interrupt>
inline auto Apply(const bool isParalyzed,
                  const std::uint16_t recastID,
                  ApplyRecast&& applyRecast,
                  Interrupt&& interrupt) -> bool
{
    if (!isParalyzed)
    {
        return false;
    }
    if (ShouldConsumeRecastOnParalyze(recastID))
    {
        std::invoke(applyRecast);
    }
    std::invoke(interrupt);
    return true;
}

} // namespace charabilityparalyzehelpers
