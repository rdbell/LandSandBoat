#pragma once

#include "mountutils.h"

#include <cstdint>

// Pure mountutils::resolveState policy helpers (slice 2839).
//
// Production host: mountutils::resolveState in mountutils.cpp.
// Helpers take host-injected scalars only (no CStatusEffect pointers).

namespace mountutilshelpers
{

// IsChocoboMountPower is true when mount power is regular or noble chocobo.
// Host injects MOUNT_CHOCOBO / MOUNT_NOBLE_CHOCOBO so pure tests can pin the
// classification without depending on base_entity.h MOUNTTYPE.
inline auto IsChocoboMountPower(const uint16_t mountPower, const uint16_t chocoboID, const uint16_t nobleChocoboID) -> bool
{
    return mountPower == chocoboID || mountPower == nobleChocoboID;
}

// PlanResolveMountState is the fully scalar form of resolveState:
//   effectNull → unmounted, mount=0, subPower=0, animation=None
//   else mounted=true, mount=power, subPower=subPower,
//        animation=Chocobo if isChocoboPower else Mount
// Host supplies isChocoboPower (typically via IsChocoboMountPower).
inline auto PlanResolveMountState(const bool effectNull, const uint16_t power, const uint16_t subPower, const bool isChocoboPower)
    -> MountStateResolution
{
    if (effectNull)
    {
        return MountStateResolution{
            .mounted   = false,
            .mount     = 0,
            .subPower  = 0,
            .animation = MountAnimation::None,
        };
    }

    return MountStateResolution{
        .mounted   = true,
        .mount     = power,
        .subPower  = subPower,
        .animation = isChocoboPower ? MountAnimation::Chocobo : MountAnimation::Mount,
    };
}

// PlanResolveMountState with injected chocobo mount IDs (production dual-wire).
// When effectNull, power/subPower/chocobo IDs do not affect the unmounted result.
inline auto PlanResolveMountState(const bool effectNull,
                                  const uint16_t power,
                                  const uint16_t subPower,
                                  const uint16_t chocoboID,
                                  const uint16_t nobleChocoboID) -> MountStateResolution
{
    const bool isChocobo = !effectNull && IsChocoboMountPower(power, chocoboID, nobleChocoboID);
    return PlanResolveMountState(effectNull, power, subPower, isChocobo);
}

} // namespace mountutilshelpers
