#pragma once

#include <cstdint>

// Pure SendToZone policy from charutils.

namespace sendtozonehelpers
{
struct SendToZonePlan
{
    uint16_t previousZoneToPersist{};
    bool     savePetZoningInfoBeforeTransition{};
    bool     resetPetZoningInfo{};
    bool     forceSynthCriticalFail{};
    bool     requestZoneChange{};
    bool     clearRequestedWarp{};
    bool     clearSessionZoneIPP{};
    bool     sendZoneLogout{};
    bool     setDisappearStatus{};
    bool     savePetZoningInfoAfterTransition{};

    constexpr auto operator==(const SendToZonePlan&) const -> bool = default;
};

// ShouldRejectPendingZone mirrors the pending-zone early return in SendToZone.
constexpr auto ShouldRejectPendingZone(const bool pendingZone) -> bool
{
    return pendingZone;
}

// ShouldRejectInvalidZoneIP mirrors SendToZone's zero-IP early return.
constexpr auto ShouldRejectInvalidZoneIP(const bool hasValidIP) -> bool
{
    return !hasValidIP;
}

// MakeSendToZonePlan mirrors SendToZone after its pending/IP/player-cap gates.
constexpr auto MakeSendToZonePlan(
    const bool inMogHouse,
    const bool destinationIsCurrentZone,
    const uint16_t previousZone,
    const uint16_t currentZone,
    const bool petPersists,
    const bool hasActiveSynth) -> SendToZonePlan
{
    return {
        .previousZoneToPersist             = (inMogHouse || destinationIsCurrentZone) ? previousZone : currentZone,
        .savePetZoningInfoBeforeTransition = petPersists,
        .resetPetZoningInfo                 = !petPersists,
        .forceSynthCriticalFail             = hasActiveSynth,
        .requestZoneChange                  = true,
        .clearRequestedWarp                 = true,
        .clearSessionZoneIPP                = true,
        .sendZoneLogout                     = true,
        .setDisappearStatus                 = true,
        .savePetZoningInfoAfterTransition  = petPersists,
    };
}
} // namespace sendtozonehelpers
