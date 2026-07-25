#pragma once

#include "entity_spawn_capacity.h"

// Pure HomePoint transition policy from charutils.

namespace homepointtransitionhelpers
{
struct HomePointTransitionPlan
{
    bool rejectAtCap{};
    bool sendCouldNotEnterMessage{};
    bool clearRequestedWarp{};
    bool removeWeakness{};
    bool removeLevelSync{};
    bool clearDeathTime{};
    bool restoreHPMP{};
    bool clearBoundary{};
    bool setHomePointLocation{};
    bool setDisappearStatus{};
    bool setAnimationNone{};
    bool setUpdateHP{};
    bool clearPacketList{};
    bool requestSendToZone{};

    constexpr auto operator==(const HomePointTransitionPlan&) const -> bool = default;
};

// MakeHomePointPlan mirrors HomePoint around its zone-cap and resetHPMP gates.
constexpr auto MakeHomePointPlan(const bool zoneAtPlayerCap, const bool resetHPMP) -> HomePointTransitionPlan
{
    if (entityspawnhelpers::ShouldRejectHomePointAtCap(zoneAtPlayerCap))
    {
        return {
            .rejectAtCap              = true,
            .sendCouldNotEnterMessage = true,
            .clearRequestedWarp       = true,
        };
    }

    const auto shouldResetHPMP = entityspawnhelpers::ShouldResetHPMPOnHomePoint(resetHPMP);
    return {
        .removeWeakness       = shouldResetHPMP,
        .removeLevelSync      = shouldResetHPMP,
        .clearDeathTime       = shouldResetHPMP,
        .restoreHPMP          = shouldResetHPMP,
        .clearBoundary        = true,
        .setHomePointLocation = true,
        .setDisappearStatus   = true,
        .setAnimationNone     = true,
        .setUpdateHP          = true,
        .clearPacketList      = true,
        .requestSendToZone    = true,
    };
}
} // namespace homepointtransitionhelpers
