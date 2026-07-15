#pragma once

#include <cstdint>
#include <span>
#include <unordered_map>

// Pure CBattleEntity::savePetModifiers planning with fully injected maps.
// Parity: internal/petmodsave (slice 1678).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::savePetModifiers (~1965–2015)
//
// Host retains m_modStat / m_modStatSave storage and applies returned updates.
// Missing current-map keys resolve to 0 (matching unordered_map operator[]
// default-constructed int16). Only keys needing a write are returned.
//
// LSB comment says "if it exists and is different"; the live condition is
// missing key OR saved value != current (update either way).

namespace petmodsavehelpers
{

// Fixed ordered pet-dynamic Mod IDs (raw uint16 pins matching modifier.h /
// OmegaXI mapenum). HTH_SDT (52) precedes IMPACT_SDT (51) by design.
//
// Physical SDT
constexpr std::uint16_t ModSlashSDT   = 49;
constexpr std::uint16_t ModPierceSDT  = 50;
constexpr std::uint16_t ModHTHSDT     = 52;
constexpr std::uint16_t ModImpactSDT  = 51;
// Uncapped Magic
constexpr std::uint16_t ModUDMGMagic  = 389;
// Element SDT
constexpr std::uint16_t ModFireSDT    = 54;
constexpr std::uint16_t ModIceSDT     = 55;
constexpr std::uint16_t ModWindSDT    = 56;
constexpr std::uint16_t ModEarthSDT   = 57;
constexpr std::uint16_t ModThunderSDT = 58;
constexpr std::uint16_t ModWaterSDT   = 59;
constexpr std::uint16_t ModLightSDT   = 60;
constexpr std::uint16_t ModDarkSDT    = 61;
// Element RES_RANK
constexpr std::uint16_t ModFireResRank    = 192;
constexpr std::uint16_t ModIceResRank     = 193;
constexpr std::uint16_t ModWindResRank    = 194;
constexpr std::uint16_t ModEarthResRank   = 195;
constexpr std::uint16_t ModThunderResRank = 196;
constexpr std::uint16_t ModWaterResRank   = 197;
constexpr std::uint16_t ModLightResRank   = 198;
constexpr std::uint16_t ModDarkResRank    = 199;
// Status RES_RANK
constexpr std::uint16_t ModParalyzeResRank   = 1160;
constexpr std::uint16_t ModBindResRank       = 1161;
constexpr std::uint16_t ModSilenceResRank    = 1162;
constexpr std::uint16_t ModSlowResRank       = 1163;
constexpr std::uint16_t ModPoisonResRank     = 1164;
constexpr std::uint16_t ModLightSleepResRank = 1165;
constexpr std::uint16_t ModDarkSleepResRank  = 1166;
constexpr std::uint16_t ModBlindResRank      = 1167;

// Count of mods in PetModsToUpdate (29).
constexpr std::size_t PetModsToUpdateCount = 29;

// PetModsToUpdate returns the fixed ordered list of Mod IDs that
// CBattleEntity::savePetModifiers considers for m_modStatSave updates.
inline auto PetModsToUpdate() -> std::span<const std::uint16_t>
{
    static constexpr std::uint16_t kMods[] = {
        // Physical SDT
        ModSlashSDT,
        ModPierceSDT,
        ModHTHSDT,
        ModImpactSDT,
        // Uncapped Magic
        ModUDMGMagic,
        // Element SDT
        ModFireSDT,
        ModIceSDT,
        ModWindSDT,
        ModEarthSDT,
        ModThunderSDT,
        ModWaterSDT,
        ModLightSDT,
        ModDarkSDT,
        // Element RES_RANK
        ModFireResRank,
        ModIceResRank,
        ModWindResRank,
        ModEarthResRank,
        ModThunderResRank,
        ModWaterResRank,
        ModLightResRank,
        ModDarkResRank,
        // Status RES_RANK
        ModParalyzeResRank,
        ModBindResRank,
        ModSilenceResRank,
        ModSlowResRank,
        ModPoisonResRank,
        ModLightSleepResRank,
        ModDarkSleepResRank,
        ModBlindResRank,
    };
    return std::span<const std::uint16_t>{ kMods };
}

// ShouldUpdateSaved reports whether m_modStatSave should receive currentVal
// for a single pet-dynamic mod.
//
// LSB:
//   if (it == m_modStatSave.end() || it->second != currentVal)
//       m_modStatSave[mod] = currentVal;
//
// hasSaved is false when the key is absent from the saved map.
inline auto ShouldUpdateSaved(const bool hasSaved, const std::int16_t savedVal, const std::int16_t currentVal)
    -> bool
{
    return !hasSaved || savedVal != currentVal;
}

// PlanSavePetModifiers returns the subset of pet-dynamic mods that must be
// written into m_modStatSave, keyed by Mod ID with the current value to store.
//
// Only keys that need a write are present. Mods outside PetModsToUpdate are
// ignored. Missing entries in current resolve to 0 (LSB operator[]).
// Missing entries in saved always need a write.
//
// Host applies: for each (mod, val) in result { m_modStatSave[mod] = val }.
inline auto PlanSavePetModifiers(const std::unordered_map<std::uint16_t, std::int16_t>& current,
                                 const std::unordered_map<std::uint16_t, std::int16_t>& saved)
    -> std::unordered_map<std::uint16_t, std::int16_t>
{
    std::unordered_map<std::uint16_t, std::int16_t> updates;
    for (const auto mod : PetModsToUpdate())
    {
        std::int16_t currentVal = 0;
        if (const auto it = current.find(mod); it != current.end())
        {
            currentVal = it->second;
        }

        bool         hasSaved = false;
        std::int16_t savedVal = 0;
        if (const auto it = saved.find(mod); it != saved.end())
        {
            hasSaved = true;
            savedVal = it->second;
        }

        if (ShouldUpdateSaved(hasSaved, savedVal, currentVal))
        {
            updates[mod] = currentVal;
        }
    }
    return updates;
}

} // namespace petmodsavehelpers
