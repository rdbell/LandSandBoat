#pragma once

#include "enums/action/proc_kind.h"

#include <cstdint>

// Pure GetSkillchainTier / GetSkillchainSubeffect tables.
// Parity: internal/skillchain Tier / Subeffect.
// Host keeps invalid-range ShowWarning; pure tables cover SC_NONE..SC_DARKNESS_II.

namespace skillchaintableshelpers
{

// SKILLCHAIN_ELEMENT ordinal bounds (battle_entity.h).
constexpr std::uint8_t SCElementNone       = 0;
constexpr std::uint8_t SCElementDarknessII = 16;

constexpr auto IsValidSkillchainElement(const std::uint8_t skillchain) -> bool
{
    return skillchain <= SCElementDarknessII;
}

// Tier table indexed by SKILLCHAIN_ELEMENT 0..16.
constexpr auto GetSkillchainTier(const std::uint8_t skillchain) -> std::uint8_t
{
    if (!IsValidSkillchainElement(skillchain))
    {
        return 0;
    }
    constexpr std::uint8_t tiers[] = {
        0, // SC_NONE
        1, // SC_TRANSFIXION
        1, // SC_COMPRESSION
        1, // SC_LIQUEFACTION
        1, // SC_SCISSION
        1, // SC_REVERBERATION
        1, // SC_DETONATION
        1, // SC_INDURATION
        1, // SC_IMPACTION
        2, // SC_GRAVITATION
        2, // SC_DISTORTION
        2, // SC_FUSION
        2, // SC_FRAGMENTATION
        3, // SC_LIGHT
        3, // SC_DARKNESS
        4, // SC_LIGHT_II
        4, // SC_DARKNESS_II
    };
    return tiers[skillchain];
}

// Subeffect table indexed by SKILLCHAIN_ELEMENT 0..16.
// Light_II / Darkness_II map to Light / Darkness packet subeffects.
constexpr auto GetSkillchainSubeffect(const std::uint8_t skillchain) -> ActionProcSkillChain
{
    if (!IsValidSkillchainElement(skillchain))
    {
        return ActionProcSkillChain::None;
    }
    constexpr ActionProcSkillChain effects[] = {
        ActionProcSkillChain::None,          // SC_NONE
        ActionProcSkillChain::Transfixion,   // SC_TRANSFIXION
        ActionProcSkillChain::Compression,   // SC_COMPRESSION
        ActionProcSkillChain::Liquefaction,  // SC_LIQUEFACTION
        ActionProcSkillChain::Scission,      // SC_SCISSION
        ActionProcSkillChain::Reverberation, // SC_REVERBERATION
        ActionProcSkillChain::Detonation,    // SC_DETONATION
        ActionProcSkillChain::Induration,    // SC_INDURATION
        ActionProcSkillChain::Impaction,     // SC_IMPACTION
        ActionProcSkillChain::Gravitation,   // SC_GRAVITATION
        ActionProcSkillChain::Distortion,    // SC_DISTORTION
        ActionProcSkillChain::Fusion,        // SC_FUSION
        ActionProcSkillChain::Fragmentation, // SC_FRAGMENTATION
        ActionProcSkillChain::Light,         // SC_LIGHT
        ActionProcSkillChain::Darkness,      // SC_DARKNESS
        ActionProcSkillChain::Light,         // SC_LIGHT_II
        ActionProcSkillChain::Darkness,      // SC_DARKNESS_II
    };
    return effects[skillchain];
}

} // namespace skillchaintableshelpers
