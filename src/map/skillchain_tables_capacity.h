#pragma once

#include "enums/action/proc_kind.h"
#include "modifier.h"

#include <cstdint>
#include <list>
#include <optional>
#include <vector>

// Pure GetSkillchainTier / Subeffect / FormSkillchain / MagicElement /
// GetResistanceRankModFromElement tables.
// Parity: internal/skillchain Tier / Subeffect / Form / MagicElements / ResistanceRankMod.
// Host keeps invalid-range ShowWarning; pure tables cover SC_NONE..SC_DARKNESS_II.

namespace skillchaintableshelpers
{

// SKILLCHAIN_ELEMENT ordinal bounds (battle_entity.h).
constexpr std::uint8_t SCElementNone        = 0;
constexpr std::uint8_t SCElementTransfixion = 1;
constexpr std::uint8_t SCElementCompression = 2;
constexpr std::uint8_t SCElementLiquefaction = 3;
constexpr std::uint8_t SCElementScission    = 4;
constexpr std::uint8_t SCElementReverberation = 5;
constexpr std::uint8_t SCElementDetonation  = 6;
constexpr std::uint8_t SCElementInduration  = 7;
constexpr std::uint8_t SCElementImpaction   = 8;
constexpr std::uint8_t SCElementGravitation = 9;
constexpr std::uint8_t SCElementDistortion  = 10;
constexpr std::uint8_t SCElementFusion      = 11;
constexpr std::uint8_t SCElementFragmentation = 12;
constexpr std::uint8_t SCElementLight       = 13;
constexpr std::uint8_t SCElementDarkness    = 14;
constexpr std::uint8_t SCElementLightII     = 15;
constexpr std::uint8_t SCElementDarknessII  = 16;

// ELEMENT ordinals (battleutils.h)
constexpr std::uint8_t ElementNone    = 0;
constexpr std::uint8_t ElementFire    = 1;
constexpr std::uint8_t ElementIce     = 2;
constexpr std::uint8_t ElementWind    = 3;
constexpr std::uint8_t ElementEarth   = 4;
constexpr std::uint8_t ElementThunder = 5;
constexpr std::uint8_t ElementWater   = 6;
constexpr std::uint8_t ElementLight   = 7;
constexpr std::uint8_t ElementDark    = 8;

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

// skillchain_map entry: key is (skill_element, resonance_element).
struct SkillchainMapEntry
{
    std::uint8_t skillElement;
    std::uint8_t resonanceElement;
    std::uint8_t result;
};

// Ordered for first-match parity with std::map iteration (insertion order of LSB map).
// Lookup uses exact pair match, not order of this array for match selection — host
// iterates resonance then skill and takes first map hit; FormSkillchain mirrors that.
constexpr SkillchainMapEntry kSkillchainMap[] = {
    // Level 3
    { SCElementLight, SCElementLight, SCElementLightII },
    { SCElementDarkness, SCElementDarkness, SCElementDarknessII },
    // Level 2
    { SCElementDistortion, SCElementGravitation, SCElementDarkness },
    { SCElementFragmentation, SCElementGravitation, SCElementFragmentation },
    { SCElementGravitation, SCElementDistortion, SCElementDarkness },
    { SCElementFusion, SCElementDistortion, SCElementFusion },
    { SCElementGravitation, SCElementFusion, SCElementGravitation },
    { SCElementFragmentation, SCElementFusion, SCElementLight },
    { SCElementDistortion, SCElementFragmentation, SCElementDistortion },
    { SCElementFusion, SCElementFragmentation, SCElementLight },
    // Level 1 → Level 2
    { SCElementScission, SCElementTransfixion, SCElementDistortion },
    { SCElementImpaction, SCElementLiquefaction, SCElementFusion },
    { SCElementCompression, SCElementDetonation, SCElementGravitation },
    { SCElementReverberation, SCElementInduration, SCElementFragmentation },
    // Level 1
    { SCElementCompression, SCElementTransfixion, SCElementCompression },
    { SCElementReverberation, SCElementTransfixion, SCElementReverberation },
    { SCElementTransfixion, SCElementCompression, SCElementTransfixion },
    { SCElementDetonation, SCElementCompression, SCElementDetonation },
    { SCElementScission, SCElementLiquefaction, SCElementScission },
    { SCElementLiquefaction, SCElementScission, SCElementLiquefaction },
    { SCElementReverberation, SCElementScission, SCElementReverberation },
    { SCElementDetonation, SCElementScission, SCElementDetonation },
    { SCElementInduration, SCElementReverberation, SCElementInduration },
    { SCElementImpaction, SCElementReverberation, SCElementImpaction },
    { SCElementScission, SCElementDetonation, SCElementScission },
    { SCElementCompression, SCElementInduration, SCElementCompression },
    { SCElementImpaction, SCElementInduration, SCElementImpaction },
    { SCElementLiquefaction, SCElementImpaction, SCElementLiquefaction },
    { SCElementDetonation, SCElementImpaction, SCElementDetonation },
};

constexpr auto LookupSkillchainPair(const std::uint8_t skillElement, const std::uint8_t resonanceElement)
    -> std::uint8_t
{
    for (const auto& e : kSkillchainMap)
    {
        if (e.skillElement == skillElement && e.resonanceElement == resonanceElement)
        {
            return e.result;
        }
    }
    return SCElementNone;
}

// FormSkillchain: nested resonance×skill, first map hit wins.
inline auto FormSkillchain(const std::list<std::uint8_t>& resonance, const std::list<std::uint8_t>& skill)
    -> std::uint8_t
{
    for (const auto resonanceElement : resonance)
    {
        for (const auto skillElement : skill)
        {
            const auto formed = LookupSkillchainPair(skillElement, resonanceElement);
            if (formed != SCElementNone)
            {
                return formed;
            }
        }
    }
    return SCElementNone;
}

// Magic elements for a skillchain resonance (ELEMENT ordinals). Max 4.
inline auto GetSkillchainMagicElement(const std::uint8_t skillchain) -> std::vector<std::uint8_t>
{
    if (!IsValidSkillchainElement(skillchain))
    {
        return {};
    }
    switch (skillchain)
    {
        case SCElementNone:
            return {};
        case SCElementTransfixion:
            return { ElementLight };
        case SCElementCompression:
            return { ElementDark };
        case SCElementLiquefaction:
            return { ElementFire };
        case SCElementScission:
            return { ElementEarth };
        case SCElementReverberation:
            return { ElementWater };
        case SCElementDetonation:
            return { ElementWind };
        case SCElementInduration:
            return { ElementIce };
        case SCElementImpaction:
            return { ElementThunder };
        case SCElementGravitation:
            return { ElementDark, ElementEarth };
        case SCElementDistortion:
            return { ElementWater, ElementIce };
        case SCElementFusion:
            return { ElementLight, ElementFire };
        case SCElementFragmentation:
            return { ElementWind, ElementThunder };
        case SCElementLight:
        case SCElementLightII:
            return { ElementLight, ElementFire, ElementWind, ElementThunder };
        case SCElementDarkness:
        case SCElementDarknessII:
            return { ElementDark, ElementEarth, ElementWater, ElementIce };
        default:
            return {};
    }
}

// GetResistanceRankModFromElement — ELEMENT → Mod. Unknown returns FIRE_RES_RANK
// only if host uses .at(); pure returns optional.
inline auto GetResistanceRankModFromElement(const std::uint8_t element) -> std::optional<Mod>
{
    switch (element)
    {
        case ElementFire:
            return Mod::FIRE_RES_RANK;
        case ElementWater:
            return Mod::WATER_RES_RANK;
        case ElementWind:
            return Mod::WIND_RES_RANK;
        case ElementEarth:
            return Mod::EARTH_RES_RANK;
        case ElementThunder:
            return Mod::THUNDER_RES_RANK;
        case ElementIce:
            return Mod::ICE_RES_RANK;
        case ElementLight:
            return Mod::LIGHT_RES_RANK;
        case ElementDark:
            return Mod::DARK_RES_RANK;
        default:
            return std::nullopt;
    }
}

} // namespace skillchaintableshelpers

