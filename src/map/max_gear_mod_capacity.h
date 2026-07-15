#pragma once

#include <cstdint>
#include <span>

// Pure CBattleEntity::getMaxGearMod with fully injected inputs.
// Parity: internal/maxgearmod (slice 1675).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::getMaxGearMod (~2133–2167)
//
// Host retains equip lookup, ITEM_EQUIPMENT / ITEM_WEAPON type checks, and
// CItemEquipment::getModifier resolution. Helpers take only the pre-extracted
// per-slot mod values for slots that held qualifying equip.
//
// LSB scans SLOT_MAIN .. SLOT_BACK exclusive (i < SLOT_BACK), so SLOT_BACK
// itself is never consulted. Host should not inject a BACK slot entry.
//
// LSB stores the running max as uint16 (getModifier returns int16, then is
// widened). Comparison and the returned int16 therefore match the unsigned
// max of the injected values.
//
// Side-effect ShowWarning on non-PC is host-owned; pure helper only returns 0.

namespace maxgearmodhelpers
{

// Mod::NONE — essential sentinel that does nothing.
constexpr std::uint16_t ModNone = 0;

// SLOT_BACK (0x0F) — exclusive upper bound of the LSB scan loop
// (for i = 0; i < SLOT_BACK; ++i).
constexpr std::uint8_t SlotBack = 0x0F;

// Number of slots scanned by getMaxGearMod
// (SLOT_MAIN .. SLOT_BACK exclusive → 15 slots).
constexpr std::size_t GearModSlotCount = 15;

// ResolveMaxGearMod mirrors CBattleEntity::getMaxGearMod.
//
//   if modID == Mod::NONE → 0
//   if !isPC → 0
//   else max of slotMods as uint16, returned as int16
//
// slotMods are host-resolved getModifier(modID) values for equipped
// equipment/weapon slots only (missing / non-qualifying slots omit or 0).
// Empty span yields 0.
inline auto ResolveMaxGearMod(const std::uint16_t modID, const bool isPC, const std::span<const std::int16_t> slotMods)
    -> std::int16_t
{
    if (modID == ModNone)
    {
        return 0;
    }
    if (!isPC)
    {
        return 0;
    }

    // LSB: uint16 maxModValue = 0; compare via uint16 after getModifier(int16).
    std::uint16_t maxModValue = 0;
    for (const std::int16_t v : slotMods)
    {
        const auto modValue = static_cast<std::uint16_t>(v);
        if (modValue > maxModValue)
        {
            maxModValue = modValue;
        }
    }
    return static_cast<std::int16_t>(maxModValue);
}

} // namespace maxgearmodhelpers
