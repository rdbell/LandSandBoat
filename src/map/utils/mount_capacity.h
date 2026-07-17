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

// ---------------------------------------------------------------------------
// Pure mountutils::packetDefinition policy (slice 2844).
//
// Production host: mountutils::packetDefinition in mountutils.cpp.
// Host injects: mounted (from resolveState), mountPower (state.mount),
// fieldChocobo (CCharEntity::m_FieldChocobo). No entity / status pointers.
//
// MOUNTTYPE pins (base_entity.h) stay as literals so the pure surface remains
// free of entity headers: MOUNT_CHOCOBO = 0, MOUNT_NOBLE_CHOCOBO = 34.
// ---------------------------------------------------------------------------

// PlanMountPacketDefinition is the fully scalar form of packetDefinition:
//   !mounted → ChocoboIndex=0
//   MOUNT_CHOCOBO + fieldChocobo != 0 → index 2, CustomProperties={field, 0}
//   MOUNT_CHOCOBO regular → index 1
//   MOUNT_NOBLE_CHOCOBO → index (mount%8)+2, CustomProperties={0, 1}
//   default → index (mount%8)+1
inline auto PlanMountPacketDefinition(const bool mounted, const uint16_t mountPower, const uint32_t fieldChocobo)
    -> MountPacketDefinition
{
    if (!mounted)
    {
        return MountPacketDefinition{
            .ChocoboIndex = 0,
        };
    }

    // Production MOUNTTYPE pins (base_entity.h).
    constexpr uint16_t MountChocobo      = 0;
    constexpr uint16_t MountNobleChocobo = 34;

    if (mountPower == MountChocobo)
    {
        // Customized / personal field chocobos need ChocoboIndex 2.
        if (fieldChocobo != 0)
        {
            return MountPacketDefinition{
                .ChocoboIndex     = 2,
                .CustomProperties = { fieldChocobo, 0 },
            };
        }

        // Regular Chocobos use 1.
        return MountPacketDefinition{
            .ChocoboIndex = 1,
        };
    }

    if (mountPower == MountNobleChocobo)
    {
        // Noble would be (34%8)+1 = 3 under the generic rule; captures use 4
        // ((mount%8)+2) to indicate a chocobo, with CustomProperties[1] = 1.
        return MountPacketDefinition{
            .ChocoboIndex     = static_cast<uint8_t>((mountPower % 8) + 2),
            .CustomProperties = { 0, 1 },
        };
    }

    // All other mounts return the remainder + 1.
    return MountPacketDefinition{
        .ChocoboIndex = static_cast<uint8_t>((mountPower % 8) + 1),
    };
}

// ---------------------------------------------------------------------------
// Pure Dismount decision policy (slice 2852).
//
// Production host: GP_CLI_COMMAND_ACTION_ACTIONID::Dismount in
// packets/c2s/0x01a_action.cpp. Host injects mounted (typically
// CBattleEntity::isMounted()). No entity / status pointers.
//
// Matches OmegaXI mount.Decision / PlanDismount:
//   !mounted → empty (no mutations)
//   mounted  → remove Mounted status, animation=None, updateHP
// ---------------------------------------------------------------------------

struct DismountDecision
{
    bool           removeStatus{ false };
    MountAnimation animation{ MountAnimation::None };
    bool           updateHP{ false };
};

// PlanDismount is the fully scalar form of the Dismount entity plan.
inline auto PlanDismount(const bool mounted) -> DismountDecision
{
    if (!mounted)
    {
        return DismountDecision{};
    }

    return DismountDecision{
        .removeStatus = true,
        .animation    = MountAnimation::None,
        .updateHP     = true,
    };
}

} // namespace mountutilshelpers
