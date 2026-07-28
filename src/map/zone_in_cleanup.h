#pragma once

// Pure CZone::CharZoneIn status and instance cleanup plan.
namespace zonehelpers
{

struct ZoneInCleanupPlan
{
    bool clearIllusion{};
    bool clearNonInstanceState{};

    auto operator==(const ZoneInCleanupPlan&) const -> bool = default;
};

inline auto PlanZoneInCleanup(const bool hasIllusion, const bool isInstanced) -> ZoneInCleanupPlan
{
    return {
        .clearIllusion         = hasIllusion,
        .clearNonInstanceState = !isInstanced,
    };
}

} // namespace zonehelpers
