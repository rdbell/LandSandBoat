#pragma once

#include "modifier.h"

#include <cstdint>

// Pure CBattleEntity::addEquipModifiers scaling and SLOT_SUB MAIN_DMG_RANK remap.
// Parity: internal/equipmod (slice 1677).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::addEquipModifiers (~1832–1914)
//
// Host retains m_modStat application, mod-list iteration, GetMLevel / item
// req level / slot reads; pure helpers take injected values only.
//
// Distinct from scaled_item_modifier_capacity (GetScaledItemModifier / 0828),
// which does NOT multiply by mLevel before the family scale. addEquipModifiers
// always multiplies by GetMLevel() first when under-level.

namespace equipmodhelpers
{

// SLOTTYPE pins matching battle_entity.h (SLOT_MAIN / SLOT_SUB).
constexpr std::uint8_t SlotMain = 0x00;
constexpr std::uint8_t SlotSub  = 0x01;

struct EquipModPlan
{
    Mod         targetMod{};
    std::int16_t amount{ 0 };
};

// RemapModForSubSlot: SLOT_SUB arm — MAIN_DMG_RANK applies to SUB_DMG_RANK.
inline auto RemapModForSubSlot(const Mod modID) -> Mod
{
    if (modID == Mod::MAIN_DMG_RANK)
    {
        return Mod::SUB_DMG_RANK;
    }
    return modID;
}

// ScaleUnderlevelAmount: under-level arm of addEquipModifiers for one entry
// (before SLOT_SUB remapping).
//
//   modAmount = mLevel * amount;  // int16 store of int product (LSB wrap)
//   switch mod family:
//     DEF, MAIN_DMG_RATING, SUB_DMG_RATING, RANGED_DMG_RATING: *=3; /=4
//     HP, MP: /=2
//     STR..CHR, ATT, RATT, ACC, RACC, MATT, MACC: /=3
//     default: 0
//   modAmount /= itemLevel
//
// Caller only invokes when mLevel < itemLevel (itemLevel ≥ 1; no /0 path).
inline auto ScaleUnderlevelAmount(const Mod          modID,
                                  const std::int16_t amount,
                                  const std::uint8_t mLevel,
                                  const std::uint8_t itemLevel) -> std::int16_t
{
    // C++ production: int16 modAmount = GetMLevel() * i.getModAmount();
    auto modAmount = static_cast<std::int16_t>(static_cast<int>(mLevel) * static_cast<int>(amount));
    switch (modID)
    {
        case Mod::DEF:
        case Mod::MAIN_DMG_RATING:
        case Mod::SUB_DMG_RATING:
        case Mod::RANGED_DMG_RATING:
            modAmount = static_cast<std::int16_t>(static_cast<int>(modAmount) * 3);
            modAmount /= 4;
            break;
        case Mod::HP:
        case Mod::MP:
            modAmount /= 2;
            break;
        case Mod::STR:
        case Mod::DEX:
        case Mod::VIT:
        case Mod::AGI:
        case Mod::INT:
        case Mod::MND:
        case Mod::CHR:
        case Mod::ATT:
        case Mod::RATT:
        case Mod::ACC:
        case Mod::RACC:
        case Mod::MATT:
        case Mod::MACC:
            modAmount /= 3;
            break;
        default:
            modAmount = 0;
            break;
    }
    modAmount /= static_cast<std::int16_t>(itemLevel);
    return modAmount;
}

// ApplyEquipModPlan: pure plan for one mod-list entry in addEquipModifiers.
// Host then does m_modStat[target] += amount (add) or -= amount (del).
inline auto ApplyEquipModPlan(const Mod          modID,
                              const std::int16_t amount,
                              const std::uint8_t mLevel,
                              const std::uint8_t itemLevel,
                              const std::uint8_t slotID) -> EquipModPlan
{
    auto applied = amount;
    if (mLevel < itemLevel)
    {
        applied = ScaleUnderlevelAmount(modID, amount, mLevel, itemLevel);
    }
    auto target = modID;
    if (slotID == SlotSub)
    {
        target = RemapModForSubSlot(modID);
    }
    return EquipModPlan{ target, applied };
}

} // namespace equipmodhelpers
