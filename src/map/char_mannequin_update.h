#pragma once

#include "zone_out_capacity.h"

// Pure updateMannequins policy for one Mog Safe item slot.

namespace mannequinupdatehelpers
{
struct MannequinUpdatePlan
{
    bool warnInvalidRace{};
    bool sendSubcontainerPacket{};

    constexpr auto operator==(const MannequinUpdatePlan&) const -> bool = default;
};

// MakeMannequinUpdatePlan mirrors updateMannequins after storage and
// furnishing facts have been resolved for a slot.
constexpr auto MakeMannequinUpdatePlan(
    const bool hasItem,
    const bool isFurnishing,
    const bool installed,
    const bool mannequin,
    const std::uint8_t race) -> MannequinUpdatePlan
{
    if (!hasItem || !zoneouthelpers::IsInstalledMannequin(isFurnishing, installed, mannequin))
    {
        return {};
    }

    return {
        .warnInvalidRace        = zoneouthelpers::ShouldWarnInvalidMannequinRace(race),
        .sendSubcontainerPacket = true,
    };
}
} // namespace mannequinupdatehelpers
